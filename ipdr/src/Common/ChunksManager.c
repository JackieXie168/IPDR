/** @file Common/ChunksManager.c
 *
 * Implementation of the chunk based memory management.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: ChunksManager.c,v 1.13 2005/04/21 11:31:22 dmitryk Exp $
 *
 * @see Common/ChunksManager.h
 *
 * Large memory buffers (chunks) are preallocated or allocated on a
 * need basis by the manager. These are then sub-allocated as needeed.
 *
 *  In addition to buffer based operation, the manager supports
 *  'message' based operations, the only difference is that the
 *  buffer size should be passed to buffer based function and in
 *  case of 'message' based oprations, the size is retrieved from
 *  the IPDR message header.
 *
 *  The chunk manager suports custom chunk allocators (this
 *  allows for example to place hooks to JNI based allocator
 *  in order to avoid memory copies when using Java when the
 *  VM supports pinning)
 *
 *  Tags can be associated with the manager. This can be used
 *  by users of the manager to identify a specific behaviour
 *  of the manager. The only currently defined tag is 'release
 *  after send' which indicates to the transport layer to
 *  release a buffer after it was successfully sent (control
 *  messages are freed after being sent, while data messages
 *  are held in the queue until an application level ack).
 *
 *  Buffers within the chunk manager are stored with no
 *  additional data overhead per buffer, this allows us to do
 *  bulk sends to the transport layer (an entire chunk can be
 *  sent with one 'send' operation) and to reduce the memory
 *  footprint of the client (transmitter).
 */

#include <IPDR/Config.h>
const char ChunksManager_rcsid[] = "@(#) $Id: ChunksManager.c,v 1.13 2005/04/21 11:31:22 dmitryk Exp $" IPDR_COPYRIGHT_STR;

#include <Common/ChunksManager.h>

const char ChunksManager_h_rcsid[] = CHUNKS_MANAGER_H_RCSID;

#include <IPDR/Defs.h>

#include <Common/Protocol.h>
#include <Common/Platform.h>
#include <Common/Log.h>
#include <Common/CommonMsgs.h>
#include <string.h>

#define CHUNK_SIZE_FACTOR 4
#define MAX_PARAM_NAME_LENGTH 64
#define MIN_MESSAGE_SIZE 32

static IPDR_Status ChunksManager_ReleaseChunk(ChunkHeader *CH);


/** Some macros for debugging */

#define CH_0(CH) ((const char *) (CH) + sizeof(ChunkHeader))
#define CH_OFFSET(CH, offsetPtr) ((offsetPtr) - CH_0(CH))
#define CH_START_OFFSET(CH) ((CH)->DataStart - CH_0(CH))
#define CH_END_OFFSET(CH)   ((CH)->DataEnd - CH_0(CH))
#define CH_CHSIZE(CH) ((CH)->ChunkSize)

#ifdef DEBUG_CHUNKSMANAGER

static
char* CHDescr(const ChunkHeader *CH) {
    static char buf[200];
    sprintf(buf, "ch-%p/%u:sz=%u(end-start=%u):%u-%u, extra=0x%p",
            CH->Manager, CH->Manager->ChunkTags,
            CH_CHSIZE(CH),
            CH->DataEnd - CH->DataStart, CH_START_OFFSET(CH), CH_END_OFFSET(CH), CH->AllocateFuncExtraData);
    return buf;
}

static
char* MgrDescr(const ChunksManager *mgr) {
    static char buf[200];
    sprintf(buf, "chmgr-%p/%u:chSz=%u,iChSz=%u,maxMem=%u,used=%u",
            mgr, mgr->ChunkTags, mgr->ChunkSize, mgr->InitialChunkSize, mgr->MaxMemUsage, mgr->UsedMemory);
    return buf;
}

#endif

/**
 * Chunk allocator function, which uses the standard abstraction layer
 * MemoryAllocate function.
 *
 * @param Size chunk size
 * @param AllocateArg additional argument (the platform services)
 * @param C allocated chunk (output)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 * */
static IPDR_Status
ChunksManager_DefaultAllocateFunc(size_t Size,
                                  void *AllocateArg,
                                  ChunkHeader **C)
{
    IPDR_PlatformServices *PS = (IPDR_PlatformServices *) AllocateArg;
    IPDR_Status rc = Platform_MEMORYALLOCATESIZE(PS, Size, *C);
    if (FAILURE(rc)) {
        X_MEMPANICSIZE(Size, __FILE__, __LINE__);
    }
    return rc;
}

/**
 *   Chunk deallocator function, which uses the standard
 *   abstraction layer MemoryFree function.
 *
 * @param C chunk to free
 * @param AllocateArg additional argument (the platform services)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static IPDR_Status
ChunksManager_DefaultFreeFunc(ChunkHeader *C,
                              void *AllocateArg)
{
    IPDR_PlatformServices *PS = (IPDR_PlatformServices *) AllocateArg;
    return Platform_MemoryFree(PS, (void *) C);
}

/**
 *   Free the memory allocated by all unused chunks
 *
 * @param Mgr chunks manager
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static IPDR_Status
ChunksManager_DeallocateFreeChunks(ChunksManager *Mgr)
{
    ChunkHeader *Header, *NextHeader;

#ifdef DEBUG_CHUNKSMANAGER
    printf(" +++ ChunksManager_DeallocateFreeChunks[%.99s]\n", MgrDescr(Mgr));
#endif

    Header = Mgr->FreeList;
    while (Header != NULL) {
        NextHeader = Header->NextChunk;
        Mgr->UsedMemory -= Header->ChunkSize;
        Mgr->Allocator.FreeFunc(Header, Mgr->Allocator.AllocateArg);
        Header = NextHeader ;
    }
    Mgr->FreeList = NULL;

    return IPDR_OK;
}

/**
 *   Allocate a new chunk within the chunks manager, with
 *   the current chunk size.
 *
 * @param Mgr chunks manager
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static IPDR_Status
ChunksManager_AllocateChunk(ChunksManager *Mgr)
{
    ChunkHeader *CH;
    IPDR_Status Status;

    if (Mgr->MaxMemUsage > 0 &&
        Mgr->UsedMemory + Mgr->ChunkSize > Mgr->MaxMemUsage)
        return IPDR_ERR_QUEUE_FULL;

    Status = Mgr->Allocator.AllocateFunc(Mgr->ChunkSize + sizeof(ChunkHeader),
                                         Mgr->Allocator.AllocateArg, & CH);
    if (FAILURE(Status))
        return Status;

    CH->Manager = Mgr;
    CH->ChunkSize = Mgr->ChunkSize;
    CH->DataEnd = (char *) CH + sizeof(ChunkHeader);
    CH->DataStart = (char *) CH + sizeof(ChunkHeader);

    /* You must NOT do this:
     *    CH->AllocateFuncExtraData = NULL;
     * because the AllocateFunc could use AllocateFuncExtraData for
     * its own purposes (AllocateFunc allocated this chunk and it
     * owns AllocateFuncExtraData). */

    if (Mgr->FreeList != NULL)
        Mgr->FreeList->PrevChunk = CH;

    CH->NextChunk = Mgr->FreeList;
    CH->PrevChunk = NULL;

    Mgr->FreeList = CH;
    Mgr->UsedMemory += Mgr->ChunkSize;

#ifdef TRACE_OBJECTS
    {
        char buf[199];
        sprintf(buf, "AllocateChunk %.99s", MgrDescr(Mgr));
        ChunksManager_Dump(Mgr, buf);
    }
#endif

    return IPDR_OK;
}

/**
 *   Creates a chunk allocator structure with the default
 *   chunk allocation functions (which use the abstraction
 *   layer)
 *
 * @param PS platform services
 * @param Allocator allocator structure to fill (output)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
ChunksManager_GetDefaultChunkAllocator(IPDR_PlatformServices *PS,
                                       ChunkAllocator *Allocator)
{
    Allocator->AllocateFunc = ChunksManager_DefaultAllocateFunc;
    Allocator->FreeFunc = ChunksManager_DefaultFreeFunc;
    Allocator->AllocateArg = PS;
    return IPDR_OK;
}

/**
 *   Initialize a new chunk manager.
 *
 *   The parameters handler can use the overide parameter
 *   values given to the manager. The following parameters
 *   can be specified:
 *
 *   <ul>
 *   <li>&lt;name&gt;_chunk_size - initial chunk size
 *   <li>&lt;name&gt;_max_mem    - maximum memory usage
 *   <li>&lt;name&gt;_initial_num_chunks - initial number of chunks to
 *                               allocate.
 *   </ul>
 *
 * @param InitialChunkSize initial chunk size in bytes
 * @param InitialNumChunks number of chunks to preallocate
 * @param MaxMemUsage maximum memory used by the manager
 * @param ChunkTags tags to associate with chunks allocated
 *               by the manager
 * @param NameChunkSize name of parameter with chunk size
 * @param NameInitialNumChunks name of parameter with initial number of chunks
 * @param NameMaxMemory name of parameter with max memory
 * @param ParamsHandler parameters handler used to overide
 *                   parameters values specified in the first
 *                   three parameters.
 * @param Allocator chunk allocator used to allocate chunks
 * @param L logger associated with the manager
 * @param Mgr chunks manager structure to fill (output)
 *
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
ChunksManager_Initialize(unsigned int InitialChunkSize,
                         unsigned int InitialNumChunks,
                         unsigned int MaxMemUsage,
                         unsigned int ChunkTags,
                         const char *NameChunkSize,
                         const char *NameInitialNumChunks,
                         const char *NameMaxMemory,
                         ParametersHandler *ParamsHandler,
                         ChunkAllocator Allocator,
                         Log *L,
                         ChunksManager *Mgr)
{
    unsigned int i;

    Mgr->ChunkSize = ParametersHandler_GetUIntParam(ParamsHandler,
                                                    NameChunkSize);
    Mgr->MaxMemUsage = ParametersHandler_GetUIntParam(ParamsHandler,
                                                      NameMaxMemory);

    InitialNumChunks = ParametersHandler_GetUIntParam(ParamsHandler,
                                                      NameInitialNumChunks);

    if (InitialNumChunks < 2) {
        InitialNumChunks = 2; /* Quietly fix this because algorithms
                               * require a minimum of 2 chunks. */
    }

    Mgr->InitialChunkSize = Mgr->ChunkSize;
    Mgr->Allocator = Allocator;
    Mgr->UsedMemory = 0;
    Mgr->Head = Mgr->Tail = Mgr->FreeList = NULL;
    Mgr->ChunkTags = ChunkTags;
    Mgr->Logger = L;

    for (i = 0; i < InitialNumChunks; i++)
        ChunksManager_AllocateChunk(Mgr);

#ifdef TRACE_OBJECTS
    {
        char buf[199];
        sprintf(buf, "Initial %.99s", MgrDescr(Mgr));
        ChunksManager_Dump(Mgr, buf);
    }
#endif

    return IPDR_OK;
}

/**
 *   Reconfigure a chunks manager after changes in parameters
 *   of the parameters handler
 *
 * @param Mgr chunks manager.
 * @param NameChunkSize name of parameter with chunk size
 * @param NameInitialNumChunks name of parameter with initial number of chunks
 * @param NameMaxMemory name of parameter with max memory
 * @param ParamsHandler parameters handler to use
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
ChunksManager_Reconfigure(ChunksManager *Mgr,
                          const char *NameChunkSize,
                          const char *NameInitialNumChunks,
                          const char *NameMaxMemory,
                          ParametersHandler *ParamsHandler)
{
    unsigned int S;

    if (! Mgr) {
        return IPDR_OK;
    }

    S = ParametersHandler_GetUIntParam(ParamsHandler,
                                       NameChunkSize);

    /* in case the initial chunk size was changed, change the current chunk
       size */

    if (S != Mgr->InitialChunkSize) {
        Mgr->ChunkSize = S;
        Mgr->InitialChunkSize = S;
    }

    Mgr->MaxMemUsage = ParametersHandler_GetUIntParam(ParamsHandler,
                                                      NameMaxMemory);

    return IPDR_OK;
}

/**
 *   Free all resources associated with a chunks manager.
 *
 * @param Mgr chunks manager.
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
ChunksManager_Destroy(ChunksManager *Mgr)
{
    ChunkHeader *Header, *NextHeader;
    int i;

#ifdef TRACE_OBJECTS
    {
        char buf[199];
        sprintf(buf, "Destroy %.99s", MgrDescr(Mgr));
        ChunksManager_Dump(Mgr, buf);
    }
#endif

    if (Mgr == NULL) {
        /* XXX Should output error message to log ... except we have
         * no log because Mgr is null */
        return IPDR_ERR;
    }

    Header = Mgr->Head;
    for (i = 0; i < 2; i++) {
        while (Header != NULL) {
            NextHeader = Header->NextChunk;
            Mgr->Allocator.FreeFunc(Header, Mgr->Allocator.AllocateArg);
            Header = NextHeader ;
        }
        Header = Mgr->FreeList;
    }

    return IPDR_OK;
}

/**
 *   Allocate a buffer within the chunks manager.
 *
 * @param Mgr chunks manager.
 * @param Size buffer size
 * @param Handle handle to the newly allocated buffer (output)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
ChunksManager_AllocateBuffer(ChunksManager *Mgr,
                             size_t Size,
                             BufferHandle *Handle)
{
    ChunkHeader *CH = NULL;
    ChunkHeader *TempCH;
    unsigned int NewChunkSize;
    IPDR_Status Status;

#ifdef DEBUG_CHUNKSMANAGER
    printf(" +++ ChunksManager_AllocateBuffer[%.99s](%u", MgrDescr(Mgr), Size);
#endif

    /* pad to the next 32 bit boundary */

/*    if (Size % 4 != 0)
        Size += (4 - (Size % 4));
*/
#ifdef DEBUG_CHUNKSMANAGER
    printf(" -> %u)\n", Size);
#endif

    /* check if there is room for the current tail of the list */
    TempCH = Mgr->Tail;
    if (TempCH != NULL) {
        if (TempCH->DataEnd + Size <=
            (char *) TempCH + sizeof(ChunkHeader) + TempCH->ChunkSize) {
            /* the chunk can fit into the current tail of the chunks list */
            CH = TempCH;
#ifdef DEBUG_CHUNKSMANAGER
            printf(" ... ChunksManager_AllocateBuffer[%.99s](%u) ... fit in current tail (%.99s)\n",
                   MgrDescr(Mgr), Size, CHDescr(TempCH));
#endif
        }
    }

    while (CH == NULL) {
        /* if there is no room in the current tail, check if there is a
           large enough chunk in the free list and use it */
        TempCH = Mgr->FreeList;
        while (TempCH != NULL) {
            if (TempCH->ChunkSize >= Size) {

                CH = TempCH;
                CH->DataEnd = (char *) CH + sizeof(ChunkHeader);
                CH->DataStart = (char *) CH + sizeof(ChunkHeader);

                if (CH->PrevChunk == NULL)
                    Mgr->FreeList = CH->NextChunk;
                else
                    CH->PrevChunk->NextChunk = CH->NextChunk;

                if (CH->NextChunk != NULL)
                    CH->NextChunk->PrevChunk = CH->PrevChunk;

                CH->NextChunk = NULL;
                CH->PrevChunk = Mgr->Tail;

                if (Mgr->Tail != NULL)
                    Mgr->Tail->NextChunk = CH;
                else
                    Mgr->Head = CH;
                Mgr->Tail = CH;

#ifdef DEBUG_CHUNKSMANAGER
                printf(" ... ChunksManager_AllocateBuffer[%.99s](%u) ... fit in free list chunk (%.99s)\n",
                       MgrDescr(Mgr), Size, CHDescr(CH));
#endif

                break;
            }
            TempCH = TempCH->NextChunk;
        }

        if (CH != NULL)
            break ;

#ifdef DEBUG_CHUNKSMANAGER
        printf(" ... ChunksManager_AllocateBuffer[%.99s](%u) ... could not fit in free list chunk\n",
               MgrDescr(Mgr), Size);
#endif

        /* if we didn't find a large enough chunk in the free list, allocate
           a new chunk */

        /* if there is no room to place the current buffer in a chunk, increase
           the default chunk size */

        NewChunkSize = Mgr->ChunkSize;
        if (NewChunkSize < Size) {
#ifdef TRACE_OBJECTS
            Log_Write2(Mgr->Logger, MEMORY_MODULE_ID,
                       IPDR_LEVEL_INFO,
                       "ChunksManager_AllocateBuffer:DeallocateFreeChunks : NewChunkSize < Size",
                       MSG_CHUNKS_MGR_OUT_OF_MEMORY, Size,
                       Mgr->MaxMemUsage, Mgr->ChunkSize, Mgr->InitialChunkSize, Mgr->UsedMemory);
#endif
            /* in case we resize the chunk, deallocate any previously allocated
               chunks, in order to accomodate for the new chunk size, since the
               other chunks might be too small */
            ChunksManager_DeallocateFreeChunks(Mgr);
            NewChunkSize = Size * CHUNK_SIZE_FACTOR;
        }

        /* If there is a memory limit, resize the chunk so it will fit the
           current memory limits */
        if (Mgr->MaxMemUsage > 0 &&
            NewChunkSize + Mgr->UsedMemory > Mgr->MaxMemUsage)
            NewChunkSize = Mgr->MaxMemUsage - Mgr->UsedMemory;

        /* If the buffer can't be fitted within the suggested chunk
           size, return an error. Chances are, the caller will
           register an error; the message we output here gives a bit
           more information. */
        if (NewChunkSize < Size) {
            Log_Write5(Mgr->Logger, MEMORY_MODULE_ID,
                       IPDR_LEVEL_WARNING, "ChunksManager_AllocateBuffer : NewChunkSize < Size",
                       MSG_CHUNKS_MGR_OUT_OF_MEMORY, Size,
                       Mgr->MaxMemUsage, Mgr->ChunkSize, Mgr->InitialChunkSize, Mgr->UsedMemory);
            return IPDR_ERR_ALLOCATION_FAILED;
        }

        Mgr->ChunkSize = NewChunkSize;

        Status = ChunksManager_AllocateChunk(Mgr);
        if (FAILURE(Status)) {
            Log_Write5(Mgr->Logger, MEMORY_MODULE_ID,
                       IPDR_LEVEL_ERROR, "ChunksManager_AllocateBuffer : Allocate failed",
                       MSG_CHUNKS_MGR_OUT_OF_MEMORY, Size,
                       Mgr->MaxMemUsage, Mgr->ChunkSize, Mgr->InitialChunkSize, Mgr->UsedMemory);
            return Status;
        }

        /* retry to get a free chunk (there should be a chunk now, since
           we allocated a large enough chunk */
    }

    Handle->Chunk = CH;
    Handle->Data = CH->DataEnd;
    CH->DataEnd += Size;

#ifdef DEBUG_CHUNKSMANAGER
    printf(" ... ChunksManager_AllocateBuffer[%.99s](%u) ... at offset %u (%u from start) (%.99s)\n",
           MgrDescr(Mgr), Size, CH_OFFSET(CH, Handle->Data), Handle->Data - CH->DataStart, CHDescr(CH));
#endif

    /* For debugging purpose, smear the buffer */

    memset(Handle->Data, 0xa1, Size);

    return IPDR_OK;
}

/**
 *   Allocate a buffer with a maximum size. The following
 *   operations are done:
 *   - If the remaining space in the current chunk is larger
 *     than MIN_MESSAGE_SIZE + sizeof(MessageHeader) the
 *     rest of the chunk is allocated and returned.
 *   - Otherwise, a new chunk is used and the entire chunk
 *     is allocated.
 *
 * @param Mgr chunks manager.
 * @param Handle handle to allocated buffer (output)
 * @param Size size of allocated buffer (output)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
ChunksManager_AllocateMaxBuffer(ChunksManager *Mgr,
                                BufferHandle *Handle,
                                unsigned int *Size)
{
    ChunkHeader *CH;
    IPDR_Status Status;

    /* if there is an existing chunk, allocate the data from it. */

    if (Mgr->Tail != NULL) {
        *Size =
            (char *) Mgr->Tail + sizeof(ChunkHeader) + Mgr->Tail->ChunkSize -
            Mgr->Tail->DataEnd;
        /* check if the remaining space is large enough */
        if (*Size > sizeof(MessageHeader) + MIN_MESSAGE_SIZE) {
            Handle->Data = Mgr->Tail->DataEnd;
            Handle->Chunk = Mgr->Tail;
            Mgr->Tail->DataEnd =
                (char *) Mgr->Tail + sizeof(ChunkHeader) + Mgr->Tail->ChunkSize;
            return IPDR_OK;
        }
    }

    if (Mgr->FreeList == NULL) {
        Status = ChunksManager_AllocateChunk(Mgr);
        if (FAILURE(Status)) {
            Log_Write5(Mgr->Logger, MEMORY_MODULE_ID,
                       IPDR_LEVEL_ERROR, "ChunksManager_AllocateBuffer",
                       MSG_CHUNKS_MGR_OUT_OF_MEMORY, *Size,
                       Mgr->MaxMemUsage, Mgr->ChunkSize, Mgr->InitialChunkSize, Mgr->UsedMemory);
            return Status;
        }
    }

    CH = Mgr->FreeList;
    CH->DataEnd = (char *) CH + sizeof(ChunkHeader) + CH->ChunkSize;
    CH->DataStart = (char *) CH + sizeof(ChunkHeader);

    Handle->Chunk = CH;
    Handle->Data = CH->DataStart;
    *Size = CH->ChunkSize;

    Mgr->FreeList = CH->NextChunk;
    if (Mgr->FreeList != NULL)
        Mgr->FreeList->PrevChunk = NULL;

    if (Mgr->Tail == NULL) {
        Mgr->Tail = Mgr->Head = CH;
        CH->PrevChunk = NULL;
        CH->NextChunk = NULL;
    } else {
        Mgr->Tail->NextChunk = CH;
        CH->PrevChunk = Mgr->Tail;
        CH->NextChunk = NULL;
        Mgr->Tail = CH;
    }

    /* if the list was previously empty it should point to the new chunk */
    if (Mgr->Head == NULL)
        Mgr->Head = CH;

    return IPDR_OK;
}

/**
 *   Free all memory allocated in the chunk after the given
 *   handle
 *
 * @param Pos position to truncate chunk at
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
ChunksManager_TruncateChunk(BufferHandle *Pos)
{
#ifdef DEBUG_CHUNKSMANAGER
    printf(" +++ ChunksManager_TruncateChunk[%.99s](%.99s)\n",
           MgrDescr(Pos->Chunk->Manager), CHDescr(Pos->Chunk));
#endif

    Pos->Chunk->DataEnd = Pos->Data;
    if (Pos->Chunk->DataEnd == Pos->Chunk->DataStart)
        return ChunksManager_ReleaseChunk(Pos->Chunk);
    return IPDR_OK;
}

/**
 *   Free all memory allocated in the chunk before the given
 *   handle
 *
 * @param Pos position to truncate chunk at
 * @param Size buffer size
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static IPDR_Status
ChunksManager_TruncateChunkBefore(BufferHandle *Pos,
                                  size_t Size)
{
    Pos->Chunk->DataStart = Pos->Data + Size;
    if (Pos->Chunk->DataEnd == Pos->Chunk->DataStart)
        return ChunksManager_ReleaseChunk(Pos->Chunk);
    return IPDR_OK;
}

/**
 *   Free all memory allocated in the chunk after the given
 *   handle
 *
 * @param Pos position to truncate chunk at
 * @param Size buffer size
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
ChunksManager_TruncateChunkAfter(BufferHandle *Pos,
                                 size_t Size)
{
    Pos->Chunk->DataEnd = Pos->Data + Size;
    if (Pos->Chunk->DataEnd == Pos->Chunk->DataStart)
        return ChunksManager_ReleaseChunk(Pos->Chunk);
    return IPDR_OK;
}

/**
 *   Return the given chunk to the list of free chunks
 *
 * @param CH chunk to release
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static IPDR_Status
ChunksManager_ReleaseChunk(ChunkHeader *CH)
{
    ChunksManager *Mgr = CH->Manager;

#ifdef DEBUG_CHUNKSMANAGER
    printf(" ... ChunksManager_ReleaseChunk[%.99s] chunk %.99s\n",
           MgrDescr(Mgr), CHDescr(CH));
#endif

    CH->DataEnd = (char *) CH + sizeof(ChunkHeader);
    CH->DataStart = (char *) CH + sizeof(ChunkHeader);

    /* remove the chunk from the chunks queue */
    if (CH->PrevChunk == NULL)
        Mgr->Head = CH->NextChunk;
    else
        CH->PrevChunk->NextChunk = CH->NextChunk;
    if (CH->NextChunk == NULL)
        Mgr->Tail = CH->PrevChunk;
    else
        CH->NextChunk->PrevChunk = CH->PrevChunk;

    /* add the chunk to the free list */
    CH->NextChunk = Mgr->FreeList;
    CH->PrevChunk = NULL;
    if (Mgr->FreeList != NULL)
        Mgr->FreeList->PrevChunk = CH;
    Mgr->FreeList = CH;

    return IPDR_OK;
}

/**
 *   Free a buffer of a given size (in order to have any
 *   effect, the buffer has to be the first or the last buffer
 *   in a chunk)
 *
 * @param BH handle to the buffer to release
 * @param BufferSize buffer size
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
ChunksManager_ReleaseBuffer(BufferHandle *BH,
                            size_t BufferSize)
{
#ifdef DEBUG_CHUNKSMANAGER
    printf(" +++ ChunksManager_ReleaseBuffer[%.99s](%.99s)(%u) data-start=%u, dataOffset=%u\n",
           MgrDescr(BH->Chunk->Manager), CHDescr(BH->Chunk), BufferSize,
           BH->Data - BH->Chunk->DataStart, CH_OFFSET(BH->Chunk, BH->Data));
#endif

    /* if the buffer is not at the beginning of the chunk, do nothing */
    if (BH->Data == BH->Chunk->DataStart) {
        BH->Chunk->DataStart += BufferSize;
    }
    if (BH->Data + BufferSize == BH->Chunk->DataEnd) {
        BH->Chunk->DataEnd -= BufferSize;
    }

    if (BH->Chunk->DataStart >= BH->Chunk->DataEnd) {
        return ChunksManager_ReleaseChunk(BH->Chunk);
    }

    return IPDR_OK;
}

/**
 *   Free a message (in order to have any
 *   effect, the buffer has to be the first or the last buffer
 *   in a chunk)
 *
 * @param BH handle to the message to release
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
ChunksManager_ReleaseMessage(BufferHandle *BH)
{
    unsigned int MessageLength = IPDR_NTOHL(((MessageHeader *) BH->Data)->MessageLength);
#ifdef DEBUG_CHUNKSMANAGER
    printf(" +++ ChunksManager_ReleaseMessage[%.99s](%.99s) MessageID=%u, Length=%ul\n",
           MgrDescr(BH->Chunk->Manager), CHDescr(BH->Chunk),
           ((MessageHeader *) BH->Data)->MessageID, MessageLength);
#endif
    return ChunksManager_ReleaseBuffer(BH, MessageLength);

    /* The above "return ChunksManager_ReleaseBuffer()" replaces the
       following code which appears to have a typo (the first if has
       "!=" instead of "=="). */

#if 0
    /* if the buffer is not at the beginning of the chunk, do nothing */
    if (BH->Data != BH->Chunk->DataStart)
        BH->Chunk->DataStart += MessageLength;

    if (BH->Data + MessageLength == BH->Chunk->DataEnd)
        BH->Chunk->DataEnd -= MessageLength;

    if (BH->Chunk->DataStart >= BH->Chunk->DataEnd)
        ChunksManager_ReleaseChunk(BH->Chunk);

    return IPDR_OK;
#endif
}

/**
 *   Given a buffer handle and a buffer size, returns a
 *   handle to the next buffer in the chunk.
 *
 * @param CurrentBuffer handle to the current buffer
 * @param BufferSize size of the current buffer.
 * @param NextBufferHandle reference to the next buffer (output)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
ChunksManager_GetNextBuffer(BufferHandle *CurrentBuffer,
                            size_t BufferSize,
                            BufferHandle *NextBufferHandle)
{
    /* !!! NOTE - CurrentBuffer and NextBufferHandle can point to the same
       structure  !!! */

#ifdef DEBUG_CHUNKSMANAGER
    printf(" +++ ChunksManager_GetNextBuffer[%.99s](%.99s)(%u)\n",
           MgrDescr(CurrentBuffer->Chunk->Manager), CHDescr(CurrentBuffer->Chunk), BufferSize);
#endif
    NextBufferHandle->Data = CurrentBuffer->Data + BufferSize;
    if (NextBufferHandle->Data >= CurrentBuffer->Chunk->DataEnd) {
        NextBufferHandle->Chunk = CurrentBuffer->Chunk->NextChunk;
        if (NextBufferHandle->Chunk != NULL) {
            NextBufferHandle->Data = NextBufferHandle->Chunk->DataStart;
            if (NextBufferHandle->Chunk->DataStart > NextBufferHandle->Chunk->DataEnd) {
                /* XXX should signal error event */
                Log_Write(CurrentBuffer->Chunk->Manager->Logger, MEMORY_MODULE_ID,
                          IPDR_LEVEL_ERROR, "ChunksManager_GetNextBuffer",
                          MSG_CHUNKS_MGR_EMPTY_CHUNK);
                /* Maybe die here? It's a nasty error ... */
            }
        } else {
            NextBufferHandle->Data = NULL;
        }

        return IPDR_OK;
    }

    NextBufferHandle->Chunk = CurrentBuffer->Chunk;
    return IPDR_OK;
}

/**
 *   Free the given buffer and sets CurrentBuffer to point
 *   to the next buffer.
 *
 * @param CurrentBuffer handle to the current buffer (input/output)
 * @param BufferSize size of the current buffer.
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
ChunksManager_ReleaseAndGetNextBuffer(BufferHandle *CurrentBuffer,
                                      size_t BufferSize)
{
/* Should be equivalent to:
   BufferHandle NextBuffer;
   ChunksManager_GetNextBuffer(CurrentBuffer, BufferSize, &NextBuffer);
   ChunksManager_ReleaseBuffer(CurrentBuffer, BufferSize);
   *CurrentBuffer = NextBuffer;
   */
    ChunkHeader *NextChunk = CurrentBuffer->Chunk->NextChunk;

#ifdef DEBUG_CHUNKSMANAGER
    printf(" +++ ChunksManager_ReleaseAndGetNextBuffer[%.99s](%.99s)(%u)\n",
           MgrDescr(CurrentBuffer->Chunk->Manager), CHDescr(CurrentBuffer->Chunk), BufferSize);
#endif

    if (CurrentBuffer->Data == CurrentBuffer->Chunk->DataStart) {
        CurrentBuffer->Chunk->DataStart += BufferSize;
        if (CurrentBuffer->Chunk->DataStart >= CurrentBuffer->Chunk->DataEnd)
            ChunksManager_ReleaseChunk(CurrentBuffer->Chunk);
    }

    CurrentBuffer->Data = CurrentBuffer->Data + BufferSize;
    if (CurrentBuffer->Data >= CurrentBuffer->Chunk->DataEnd) {
        CurrentBuffer->Chunk = NextChunk;
        if (NextChunk == NULL)
            CurrentBuffer->Data = NULL;
        else
            CurrentBuffer->Data = NextChunk->DataStart;
    }

    return IPDR_OK;
}

/**
 *   Given a handle to a message, outputs a handle to the
 *   next message in the manager.
 *
 * @param CurrentBuffer handle to the current message
 * @param NextBufferHandle handle to the next message (output)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
ChunksManager_GetNextMessage(BufferHandle *CurrentBuffer,
                             BufferHandle *NextBufferHandle)
{
	MessageHeader	Hdr ;
    unsigned int CurrentBufferSize ;


	memcpy( &Hdr, CurrentBuffer->Data, sizeof(MessageHeader) );

	CurrentBufferSize = IPDR_NTOHL(Hdr.MessageLength);

    /* Note: The is probably equivalent to
       return ChunksManager_GetNextBuffer(CurrentBuffer, CurrentBufferSize, NextBufferHandle);
    */

#ifdef DEBUG_CHUNKSMANAGER
    printf(" +++ ChunksManager_GetNextMessage[%.99s](%.99s)\n",
           MgrDescr(CurrentBuffer->Chunk->Manager), CHDescr(CurrentBuffer->Chunk));
#endif

    NextBufferHandle->Data = CurrentBuffer->Data + CurrentBufferSize;
    if (NextBufferHandle->Data >= CurrentBuffer->Chunk->DataEnd) {
        NextBufferHandle->Chunk = CurrentBuffer->Chunk->NextChunk;
        if (NextBufferHandle->Chunk != NULL) {
            NextBufferHandle->Data = NextBufferHandle->Chunk->DataStart;
            if (NextBufferHandle->Chunk->DataStart > NextBufferHandle->Chunk->DataEnd) {
                /* XXX should signal error event */
                Log_Write(CurrentBuffer->Chunk->Manager->Logger, MEMORY_MODULE_ID,
                          IPDR_LEVEL_ERROR, "ChunksManager_GetNextMessage",
                          MSG_CHUNKS_MGR_EMPTY_CHUNK);
                /* Maybe die here? It's a nasty error ... */
            }
        } else {
            NextBufferHandle->Data = NULL;
        }

        return IPDR_OK;
    }

    NextBufferHandle->Chunk = CurrentBuffer->Chunk;
    return IPDR_OK;
}

/**
 *   Given a handle to a message, frees the message and
 *   returns a handle to the next message.
 *
 * @param CurrentBuffer handle to the current message (in/out)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
ChunksManager_ReleaseAndGetNextMessage(BufferHandle *CurrentBuffer)
{
    /* Note: The is probably equivalent to
       return ChunksManager_ReleaseAndGetNextBuffer(CurrentBuffer, IPDR_NTOHL(...->MessageLength));
    */
    ChunkHeader *NextChunk = CurrentBuffer->Chunk->NextChunk;
    unsigned int MessageSize ;
	MessageHeader	Hdr ;

	memcpy( &Hdr, CurrentBuffer->Data, sizeof(MessageHeader) );
	MessageSize = IPDR_NTOHL(Hdr.MessageLength);

#ifdef DEBUG_CHUNKSMANAGER
    printf(" +++ ChunksManager_ReleaseAndGetNextMessage[%.99s](%.99s) size=%u\n",
           MgrDescr(CurrentBuffer->Chunk->Manager), CHDescr(CurrentBuffer->Chunk), MessageSize);
#endif

    if (CurrentBuffer->Data == CurrentBuffer->Chunk->DataStart) {
        CurrentBuffer->Chunk->DataStart += MessageSize;
        if (CurrentBuffer->Chunk->DataStart >= CurrentBuffer->Chunk->DataEnd)
            ChunksManager_ReleaseChunk(CurrentBuffer->Chunk);
    }

    CurrentBuffer->Data += MessageSize;
    if (CurrentBuffer->Data >= CurrentBuffer->Chunk->DataEnd) {
        CurrentBuffer->Chunk = NextChunk;
        if (NextChunk == NULL)
            CurrentBuffer->Data = NULL;
        else
            CurrentBuffer->Data = NextChunk->DataStart;
    }

    return IPDR_OK;
}

/**
 *   Free all buffers up to a given position
 *
 * @param BH position
 * @param Size buffer size
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
ChunksManager_FreeUntil(BufferHandle *BH, size_t Size)
{
    ChunkHeader *CH, *NextCH;
    ChunksManager *Mgr = BH->Chunk->Manager;

#ifdef DEBUG_CHUNKSMANAGER
    printf(" +++ ChunksManager_FreeUntil[%.99s](%.99s)(%u)\n",
           MgrDescr(BH->Chunk->Manager), CHDescr(BH->Chunk), Size);
#endif

    CH = Mgr->Head;

    while (CH != NULL) {

        if (BH->Chunk == CH) {
            CH->PrevChunk = NULL; /* Already done (we should be the 1st on the list) */
            return ChunksManager_TruncateChunkBefore(BH, Size);
        }

        NextCH = CH->NextChunk;

        if (Mgr->FreeList != NULL)
            Mgr->FreeList->PrevChunk = CH;
        CH->NextChunk = Mgr->FreeList;
        CH->PrevChunk = NULL;
        Mgr->FreeList = CH;

        CH = NextCH;
        Mgr->Head = NextCH;
    }

    Mgr->Head = Mgr->Tail = NULL;

    return IPDR_OK;
}


/**
 *   Retruns all chunks in a manager to the free list
 *
 * @param Mgr chunks manager
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
ChunksManager_FreeAllChunks(ChunksManager *Mgr)
{
    /* look for chunks we a zero refcount (this can happen in case the
       reference count was decreased from Java, which can only control
       the refence count but not actually move chunks to the free list */

    ChunkHeader *CH, *NextCH;

#ifdef DEBUG_CHUNKSMANAGER
    printf(" +++ ChunksManager_FreeAllChunks[%.99s]\n",
           MgrDescr(Mgr));
#endif

    CH = Mgr->Head;

    while (CH != NULL) {
        NextCH = CH->NextChunk;

        if (Mgr->FreeList != NULL)
            Mgr->FreeList->PrevChunk = CH;

        CH->NextChunk = Mgr->FreeList;
        CH->PrevChunk = NULL;

        Mgr->FreeList = CH;

        CH = NextCH;
    }

    Mgr->Head = Mgr->Tail = NULL;

    return IPDR_OK;
}

/**
 *   Retruns a reference to the first allocated buffer in
 *   the manager
 *
 * @param Mgr chunks manager
 * @param BH handle to the first allocated buffer (output)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
ChunksManager_GetFirstBuffer(ChunksManager *Mgr,
                             BufferHandle *BH)
{
    if (Mgr->Head == NULL) {
        INIT_BUFFER_HANDLE(*BH);
        return IPDR_OK;
    }
    if (Mgr->Head->DataStart >= Mgr->Head->DataEnd) {
        INIT_BUFFER_HANDLE(*BH);
        return IPDR_OK;
    }

    BH->Chunk = Mgr->Head;
    BH->Data = Mgr->Head->DataStart;
    return IPDR_OK;
}

/**
 * Return a reference to the last allocated buffer in
 *   the manager.
 *
 * @param Mgr chunks manager
 * @param BufferSize size of buffer (this implies that the function
 *          can only be used by fixed size buffers)
 * @param BH handle to the first allocated buffer (output)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
ChunksManager_GetLastBuffer(ChunksManager *Mgr,
                            size_t BufferSize,
                            BufferHandle *BH)
{
    if (Mgr->Tail == NULL || Mgr->Tail->DataStart >= Mgr->Tail->DataEnd) {
        INIT_BUFFER_HANDLE(*BH);
        return IPDR_OK;
    }

    BH->Chunk = Mgr->Tail;
    BH->Data = Mgr->Tail->DataEnd - BufferSize;
    return IPDR_OK;
}

/**
 *   Returns the first chunk in a manager
 *
 * @param Mgr chunks manager
 *
 * @return First chunk in the manager
 */
ChunkHeader *ChunksManager_GetFirstChunk(ChunksManager *Mgr)
{
    return Mgr->Head;
}

/**
 *   Given a chunk, returns a pointer to the next chunk in
 *   the manager
 *
 * @param CurrentChunk reference to current chunk
 *
 * RETURN VALUE:
 *   Reference to the next chunk
 */
ChunkHeader *ChunksManager_GetNextChunk(ChunkHeader *CurrentChunk)
{
    if (CurrentChunk == NULL)
        return NULL;
    return CurrentChunk->NextChunk;
}

/**
 *   Given a chunk header, returns the starting and end
 *   offset of data within the chunk relative to the chunk
 *   header.
 *
 * @param CH chunk header
 * @param StartOffset offset of data start (output)
 * @param EndOffset offset of data end (output)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
ChunksManager_GetChunkInfo(ChunkHeader *CH,
                           unsigned int *StartOffset,
                           unsigned int *EndOffset)
{
    *StartOffset = CH->DataStart - (char *) CH;
    *EndOffset = CH->DataEnd - (char *) CH;
    return IPDR_OK;
}

/**
 *   Dumps the contents of the chunk manager to the log
 *
 * @param Mgr chunks manager.
 * @param Title title to place before the dump
 */
void
ChunksManager_Dump(ChunksManager *Mgr, const char *Title)
{
    ChunkHeader *CH;
    int StartOffset, EndOffset;

    if (Mgr == NULL) {
        /* Need error message to log ... but we have no log */
        return;
    }

    CH = Mgr->Head;

    Log_OutputStr2(Mgr->Logger, MEMORY_MODULE_ID, IPDR_LEVEL_DEBUG, "ChunksManager_Dump", "ChunksManager: 0x%p - %.99s", Mgr, Title);
    Log_OutputStr (Mgr->Logger, MEMORY_MODULE_ID, IPDR_LEVEL_DEBUG, "ChunksManager_Dump", "-------------------------------------");
    Log_OutputStr5(Mgr->Logger, MEMORY_MODULE_ID, IPDR_LEVEL_DEBUG, "ChunksManager_Dump", "chunkSize=%u initialChunkSize=%u maxMemUsage=%u usedMem=%u chunkTags=%u",
                   Mgr->ChunkSize, Mgr->InitialChunkSize, Mgr->MaxMemUsage, Mgr->UsedMemory, Mgr->ChunkTags);
    Log_OutputStr2(Mgr->Logger, MEMORY_MODULE_ID, IPDR_LEVEL_DEBUG, "ChunksManager_Dump", "Head: 0x%p, Tail: 0x%p", Mgr->Head, Mgr->Tail);


    while (CH != NULL) {
        StartOffset = CH->DataStart - (char *) CH;
        EndOffset = CH->DataEnd - (char *) CH;
        Log_OutputStr6(Mgr->Logger, MEMORY_MODULE_ID, IPDR_LEVEL_DEBUG, "ChunksManager_Dump", "Alloc Chunk: 0x%p, Size: %d, StartOffset: %d, EndOffset: %d, Next: 0x%p, Prev:0x%p",
                       CH, CH->ChunkSize, StartOffset, EndOffset,
                       CH->NextChunk, CH->PrevChunk);
        CH = CH->NextChunk;
    }

    CH = Mgr->FreeList;
    while (CH != NULL) {
        Log_OutputStr3(Mgr->Logger, MEMORY_MODULE_ID, IPDR_LEVEL_DEBUG, "ChunksManager_Dump", "Free Chunk: 0x%p, Next: 0x%p, Prev: 0x%p",
                       CH, CH->NextChunk, CH->PrevChunk);
        CH = CH->NextChunk;
    }
}
