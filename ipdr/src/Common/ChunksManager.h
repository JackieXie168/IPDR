/** @file Common/ChunksManager.h
 *
 * Header file for chunk based memory management, as implemented in
 * Common/ChunksManager.c.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: ChunksManager.h,v 1.10 2005/01/30 15:41:24 guyg Exp $
 * 
 * The file contains definitions of functions and structures for the
 * chunk based memory management of IPDR.
 * ...
 */

#ifndef _CHUNKS_MANAGER_H_
#define _CHUNKS_MANAGER_H_

#include <IPDR/Config.h>
#define CHUNKS_MANAGER_H_RCSID "@(#) $Id: ChunksManager.h,v 1.10 2005/01/30 15:41:24 guyg Exp $" IPDR_COPYRIGHT_STR

#if defined(GLOBAL_RENAME)
#define ChunksManager_AllocateBuffer           IPDR__ChunksManager_AllocateBuffer
#define ChunksManager_AllocateMaxBuffer        IPDR__ChunksManager_AllocateMaxBuffer
#define ChunksManager_Destroy                  IPDR__ChunksManager_Destroy
#define ChunksManager_Dump                     IPDR__ChunksManager_Dump
#define ChunksManager_FreeAllChunks            IPDR__ChunksManager_FreeAllChunks
#define ChunksManager_FreeUntil                IPDR__ChunksManager_FreeUntil
#define ChunksManager_GetChunkInfo             IPDR__ChunksManager_GetChunkInfo
#define ChunksManager_GetDefaultChunkAllocator IPDR__ChunksManager_GetDefaultChunkAllocator
#define ChunksManager_GetFirstBuffer           IPDR__ChunksManager_GetFirstBuffer
#define ChunksManager_GetFirstChunk            IPDR__ChunksManager_GetFirstChunk
#define ChunksManager_GetLastBuffer            IPDR__ChunksManager_GetLastBuffer
#define ChunksManager_GetNextBuffer            IPDR__ChunksManager_GetNextBuffer
#define ChunksManager_GetNextChunk             IPDR__ChunksManager_GetNextChunk
#define ChunksManager_GetNextMessage           IPDR__ChunksManager_GetNextMessage
#define ChunksManager_Initialize               IPDR__ChunksManager_Initialize
#define ChunksManager_Reconfigure              IPDR__ChunksManager_Reconfigure
#define ChunksManager_ReleaseAndGetNextBuffer  IPDR__ChunksManager_ReleaseAndGetNextBuffer
#define ChunksManager_ReleaseAndGetNextMessage IPDR__ChunksManager_ReleaseAndGetNextMessage
#define ChunksManager_ReleaseBuffer            IPDR__ChunksManager_ReleaseBuffer
#define ChunksManager_ReleaseMessage           IPDR__ChunksManager_ReleaseMessage
#define ChunksManager_TruncateChunk            IPDR__ChunksManager_TruncateChunk
#define ChunksManager_TruncateChunkAfter       IPDR__ChunksManager_TruncateChunkAfter
#define ChunksManager_h_rcsid                  IPDR__ChunksManager_h_rcsid
#define ChunksManager_rcsid                    IPDR__ChunksManager_rcsid
#endif

#include <stddef.h>
#include <stdio.h>

#include <IPDR/Defs.h>
#include <IPDR/IPDR.h>
#include <Common/ParametersHandler.h>
#include <Common/Log.h>

#ifdef __cplusplus
extern "C" {
#endif

#if 0
} /* put this here to un-confuse Emacs's automatic indentation */
#endif

/* Chunk flag: when the tag is set, buffers are released immediately
   after being sent by the transport layer.
 */
#define CHUNK_TAG_RELEASE_AFTER_SEND 0x0001

/* misc macros */
#define BUFFER_IS_NULL(BH) ((BH).Data == NULL)
#define BUFFER_TAGS(BH) ((BH).Chunk->Manager->ChunkTags)
#define INIT_BUFFER_HANDLE(BH) (BH).Data = NULL

struct IPDR_tag_ChunksManager;

/**
 * header placed before each chunks data.
 */
typedef struct IPDR_tag_ChunkHeader
{
    struct IPDR_tag_ChunksManager *Manager;
    struct IPDR_tag_ChunkHeader *NextChunk;
    struct IPDR_tag_ChunkHeader *PrevChunk;
    char *DataEnd;   /**< end of data within the chunk */
    char *DataStart; /**< start of data within the chunk */
    size_t ChunkSize;
    void *AllocateFuncExtraData; /**< left unitialized/unused by all
                                  * these functions but might be set
                                  * by the AllocateFunc */
}
ChunkHeader;


/**
 * handle to a buffer within a chunk (contains a pointer to the actual
 * data as well as to the chunk header.
 */
typedef struct IPDR_tag_BufferHandle
{
    ChunkHeader *Chunk;
    char *Data;
}
BufferHandle;

typedef IPDR_Status (*ChunkAllocateFunc)(size_t Size,
                                          void *AllocateArg,
                                          ChunkHeader **C);
typedef IPDR_Status (*ChunkFreeFunc)(ChunkHeader *C,
                                      void *AllocateArg);

/**
 * structure containing pointers to functions
 * for allocation / deallocation chunks.
 */
typedef struct
{
    ChunkAllocateFunc AllocateFunc;
    ChunkFreeFunc FreeFunc;
    void *AllocateArg;
}
ChunkAllocator;

/**
 * structure containing information about a chunks manager (includes a
 * link of used chunks, free chunks, and the total amount of memory
 * used by the manager.
 */
typedef struct IPDR_tag_ChunksManager
{
    Log *Logger;
    ChunkHeader *Head;
    ChunkHeader *Tail;
    ChunkHeader *FreeList;

    ChunkAllocator Allocator;

    unsigned int ChunkSize;
    unsigned int InitialChunkSize;
    unsigned int MaxMemUsage;
    unsigned int UsedMemory;
    unsigned int ChunkTags;
}
ChunksManager;


IPDR_Status
ChunksManager_GetDefaultChunkAllocator(IPDR_PlatformServices *PS,
                                       ChunkAllocator *Allocator);

IPDR_Status
ChunksManager_Initialize(size_t InitialChunkSize,
                         unsigned int InitialNumChunks,
                         size_t MaxMemUsage,
                         unsigned int Tags,
                         const char *NameChunkSize,
                         const char *NameInitialNumChunks,
                         const char *NameMaxMemory,
                         ParametersHandler *ParamsHandler,
                         ChunkAllocator Allocator,
                         Log *L,
                         ChunksManager *Mgr);

IPDR_Status ChunksManager_Destroy(ChunksManager *Mgr);
IPDR_Status ChunksManager_AllocateBuffer(ChunksManager *Mgr,
                                          size_t Size,
                                          BufferHandle *Handle);
IPDR_Status ChunksManager_AllocateMaxBuffer(ChunksManager *Mgr,
                                             BufferHandle *Handle,
                                             size_t *Size);

IPDR_Status ChunksManager_TruncateChunk(BufferHandle *Handle);
IPDR_Status ChunksManager_TruncateChunkAfter(BufferHandle *Handle,
                                              size_t Size);

IPDR_Status ChunksManager_ReleaseBuffer(BufferHandle *Handle,
                                         size_t BufferSize);
IPDR_Status ChunksManager_ReleaseMessage(BufferHandle *Handle);
IPDR_Status ChunksManager_GetNextBuffer(BufferHandle *CurrentBuffer,
                                         size_t CurrentBufferSize,
                                         BufferHandle *NextBufferHandle);
IPDR_Status ChunksManager_GetNextMessage(BufferHandle *CurrentBuffer,
                                          BufferHandle *NextBufferHandle);
IPDR_Status ChunksManager_FreeAllChunks(ChunksManager *Mgr);
IPDR_Status ChunksManager_FreeUntil(BufferHandle *BH, size_t Size);

IPDR_Status ChunksManager_GetFirstBuffer(ChunksManager *Mgr,
                                          BufferHandle *BH);
IPDR_Status ChunksManager_GetLastBuffer(ChunksManager *Mgr,
                                         size_t BufferSize,
                                         BufferHandle *BH);

IPDR_Status ChunksManager_ReleaseAndGetNextBuffer(BufferHandle *Handle,
                                                   size_t BufferSize);
IPDR_Status ChunksManager_ReleaseAndGetNextMessage(BufferHandle *Handle);

ChunkHeader *ChunksManager_GetFirstChunk(ChunksManager *Mgr);
ChunkHeader *ChunksManager_GetNextChunk(ChunkHeader *CurrentChunk);
IPDR_Status ChunksManager_GetChunkInfo(ChunkHeader *CH,
                                        size_t *StartOffset,
                                        size_t *EndOffset);
IPDR_Status ChunksManager_Reconfigure(ChunksManager *Mgr,
                                       const char *NameChunkSize,
                                       const char *NameInitialNumChunks,
                                       const char *NameMaxMemory,
                                       ParametersHandler *ParamsHandler);

void ChunksManager_Dump(ChunksManager *Mgr, const char *Title);

#ifdef __cplusplus
}
#endif

#endif
