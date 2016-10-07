/** @file Exporter/VolatileQueue.c
 *
 * Implementation of a volatile queue.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: VolatileQueue.c,v 1.16 2005/04/21 11:30:21 dmitryk Exp $
 *
 * The volatile queue is used to store unacknowledged
 *   records in memory. The queue supports the following
 *   operations:
 *
 *  <ul>
 *  <li>VolatileQueue_AllocateRecord() - allocates memory for a record within
 *                      the queue
 *  <li>VolatileQueue_PushRecord() - adds a record to the queue.
 *  <li>VolatileQueue_PeekRecord() - get the next record from the queue,
 *                  without removing it.
 *  <li>VolatileQueue_RemoveRecords() - removes records from the queue.
 *  <li>VolatileQueue_Rollback() - sets the peek position to the head of the
 *                queue.
 *  </ul>
 */

#include <IPDR/Config.h>
const char VolatileQueue_rcsid[] = "@(#) $Id: VolatileQueue.c,v 1.16 2005/04/21 11:30:21 dmitryk Exp $" IPDR_COPYRIGHT_STR;

#include <Exporter/VolatileQueue.h>

const char VolatileQueue_h_rcsid[] = VOLATILE_QUEUE_H_RCSID;

#include <stdio.h>

#include <Exporter/ExporterMsgs.h>
#include <Common/Protocol.h>
#include <Common/ConfigurationNames.h>

/**
 * Initialize a new volatile queue.
 *
 * @param PS platform services
 * @param L logger
 * @param ParamsHandler handler for getting parameters
 * @param VQ queue to initialize
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
VolatileQueue_Initialize(IPDR_PlatformServices *PS,
                         Log *L,
                         ParametersHandler *ParamsHandler,
                         VolatileQueue *VQ)
{
    IPDR_Status Status;
    ChunkAllocator Alloc;

    VQ->Logger = L;

    ChunksManager_GetDefaultChunkAllocator(PS, & Alloc);
    Status = ChunksManager_Initialize(0,
                                      0,
                                      0,
                                      0,
                                      STR_configuration_queueChunkSize,
                                      STR_configuration_queueInitialNumChunks,
                                      STR_configuration_queueMaxMemory,
                                      ParamsHandler,
                                      Alloc,
                                      L,
                                      & (VQ->ChunksMgr));
    if (FAILURE(Status))
        return Status;

    INIT_BUFFER_HANDLE(VQ->PeekPosition);

    VQ->QueueSize = 0;
    VQ->PeekStarted = IPDR_FALSE;
    VQ->OutstandingRecords = 0;

    return IPDR_OK;
}

/**
 * Delete a volaile queue.
 *
 * @param VQ queue to delete
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
VolatileQueue_Destroy(VolatileQueue *VQ)
{
    return ChunksManager_Destroy(& (VQ->ChunksMgr));
}

#if 0
(apparently not used ... but it should be)

/**
 * Allocate a new record within the queue.
 *
 * @param VQ queue to allocate record within
 * @param Size record size
 * @param B allocated record (output)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
VolatileQueue_AllocateRecord(VolatileQueue *VQ,
                             unsigned int Size,
                             BufferHandle *B)
{
    return ChunksManager_AllocateBuffer(& (VQ->ChunksMgr), Size, B);
}

#endif

/**
 * Push a record to the queue.
 *
 * @param VQ queue to add record to
 * @param B record to push
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
VolatileQueue_PushRecord(VolatileQueue *VQ, BufferHandle *B)
{
    if (VQ->PeekStarted && BUFFER_IS_NULL(VQ->PeekPosition))
        VQ->PeekPosition = *B;
    VQ->QueueSize++;

    return IPDR_OK;
}

/**
 * Get a record from the queue.
 *   Note the side-effect in here of setting PeekStarted,
 *   so that a push adjust the peek position; and RemoveRecords
 *   resets the peek.
 *
 * @param VQ queue to get record from
 * @param B peeked record (output)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
VolatileQueue_PeekRecord(VolatileQueue *VQ, BufferHandle *B)
{
    if (BUFFER_IS_NULL(VQ->PeekPosition)) {
        if (! VQ->PeekStarted) {
            ChunksManager_GetFirstBuffer(& (VQ->ChunksMgr), B);
            if (! BUFFER_IS_NULL(*B)) {
                ChunksManager_GetNextMessage(B, & (VQ->PeekPosition));
                VQ->OutstandingRecords++;
            }
        } else {
            INIT_BUFFER_HANDLE(*B);
        }
        VQ->PeekStarted = IPDR_TRUE;
        return IPDR_OK;
    }

    *B = VQ->PeekPosition;
    VQ->OutstandingRecords++;
    ChunksManager_GetNextMessage(& (VQ->PeekPosition), & (VQ->PeekPosition));
    VQ->PeekStarted = IPDR_TRUE;

    return IPDR_OK;
}

IPDR_Status
VolatileQueue_RemoveRecords_All(VolatileQueue *VQ,
                            unsigned int *RemovedRecords)
{
    DataMessage *Header;
    IPDR_DSN_TYPE LastDSN;
    BufferHandle BH;

    *RemovedRecords = 0;

    if Log_ShouldWrite(VQ->Logger, QUEUE_MODULE_ID, IPDR_LEVEL_DEBUG) {
        ChunksManager_Dump(&VQ->ChunksMgr, " +++ VolatileQueue_RemoveRecords_All +++");
    }

    ChunksManager_GetFirstBuffer(& (VQ->ChunksMgr), & BH);

    while (! BUFFER_IS_NULL(BH)) {
		Header = (DataMessage *) BH.Data;
		LastDSN = IPDR_NTOHLL(Header->sequenceNum);


        if (BH.Data == VQ->PeekPosition.Data) {
            if Log_ShouldWrite(VQ->Logger, QUEUE_MODULE_ID, IPDR_LEVEL_DEBUG) {
                Log_OutputStr(VQ->Logger, QUEUE_MODULE_ID, IPDR_LEVEL_DEBUG, "VolatileQueue_RemoveRecords_All",
                              "BH.Data == VQ->PeedkPosition.Data");
            }
            INIT_BUFFER_HANDLE(VQ->PeekPosition);
            VQ->PeekStarted = IPDR_FALSE;
        }

        ChunksManager_ReleaseAndGetNextMessage(& BH);

        VQ->QueueSize--;
        VQ->OutstandingRecords--;

        (*RemovedRecords)++;
	}
    Log_OutputStr4(VQ->Logger, QUEUE_MODULE_ID, IPDR_LEVEL_DEBUG, "VolatileQueue_RemoveRecords_All",
                   "after: DSN(%llu) peekStarted=%d BH.Data=0x%p PeekPosition.Data=0x%p",
                   LastDSN, VQ->PeekStarted, BH.Data, VQ->PeekPosition.Data);

    Log_Write4(VQ->Logger, QUEUE_MODULE_ID, IPDR_LEVEL_DEBUG,
               "VolatileQueue_RemoveRecords", MSG_QUEUE_REMOVE_RECORDS,
               LastDSN, VQ->QueueSize, VQ->OutstandingRecords, *RemovedRecords);

    if Log_ShouldWrite(VQ->Logger, QUEUE_MODULE_ID, IPDR_LEVEL_DEBUG) {
        ChunksManager_Dump(&VQ->ChunksMgr, " +++ VolatileQueue_RemoveRecords_All (after) +++");
    }

    return IPDR_OK;
}
/**
 * Remove records with a smaller DSN then the one specified
 *   (Actually older ones, because we have to worry about wrap-around).
 *   Also resets whether we're peeking.
 *
 * @param VQ queue to get record from
 * @param DSN sequence number up to which to remove
 * @param RemovedRecords number of removed records (output)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
VolatileQueue_RemoveRecords(VolatileQueue *VQ,
                            IPDR_DSN_TYPE DSN,
                            unsigned int *RemovedRecords)
{
    DataMessage *Header;
    IPDR_DSN_TYPE CurrentDSN;
    BufferHandle BH;

    *RemovedRecords = 0;

    if (IPDR_INVALID_DSN(DSN)) {
        return IPDR_OK;
    }

    if Log_ShouldWrite(VQ->Logger, QUEUE_MODULE_ID, IPDR_LEVEL_DEBUG) {
        ChunksManager_Dump(&VQ->ChunksMgr, " +++ VolatileQueue_RemoveRecords +++");
    }

    ChunksManager_GetFirstBuffer(& (VQ->ChunksMgr), & BH);

    while (! BUFFER_IS_NULL(BH)) {
        enum IPDR_compareDSN_t comparison;
        Header = (DataMessage *) BH.Data;
		CurrentDSN = IPDR_NTOHLL(Header->sequenceNum);

        /* Exit if the current dsn is larger then the acknowledged DSN,
           handled in a modulo 2^31 fashion. */

        comparison = compareDSN(CurrentDSN, DSN);

        if(Log_ShouldWrite(VQ->Logger, QUEUE_MODULE_ID, IPDR_LEVEL_DEBUG)) {
            const char *comp;
            switch (comparison) {
            case IPDR_compareDSN_less:    comp = "<"; break;
            case IPDR_compareDSN_equal:   comp = "="; break;
            case IPDR_compareDSN_greater: comp = ">"; break;
            case IPDR_compareDSN_unknown: comp = "?"; break;
            default:                       comp = "-"; break;
            }
            Log_OutputStr6(VQ->Logger, QUEUE_MODULE_ID, IPDR_LEVEL_DEBUG, "VolatileQueue_RemoveRecords",
                           "currentDSN(%llu) %.20s DSN(%llu) peekStarted=%d BH.Data=0x%p PeekPosition.Data=0x%p",
                           CurrentDSN, comp, DSN, VQ->PeekStarted, BH.Data, VQ->PeekPosition.Data);
        }

        /* XXX We could put in a sanity test here, verifying that if
           CurrentDSN > DSN, then CurrentDSN == DSN+1.  If this fails,
           it indicates the server is very confused ...  we can't do
           much with it, except log the fact. */

        if (comparison == IPDR_compareDSN_greater) {
            break;
        }

        if (comparison == IPDR_compareDSN_unknown) {
            /* XXX should signal error event */
            Log_Write3(VQ->Logger, QUEUE_MODULE_ID, IPDR_LEVEL_ERROR,
                       "VolatileQueue_RemoveRecords", MSG_QUEUE_WILD_REMOVE_RECORDS,
                       DSN, CurrentDSN, VQ->QueueSize);
            break;
        }

        if (BH.Data == VQ->PeekPosition.Data) {
            if Log_ShouldWrite(VQ->Logger, QUEUE_MODULE_ID, IPDR_LEVEL_DEBUG) {
                Log_OutputStr(VQ->Logger, QUEUE_MODULE_ID, IPDR_LEVEL_DEBUG, "VolatileQueue_RemoveRecords",
                              "BH.Data == VQ->PeedkPosition.Data");
            }
            INIT_BUFFER_HANDLE(VQ->PeekPosition);
            VQ->PeekStarted = IPDR_FALSE;
        }

        ChunksManager_ReleaseAndGetNextMessage(& BH);

        VQ->QueueSize--;
        VQ->OutstandingRecords--;

        (*RemovedRecords)++;
    }

    Log_OutputStr4(VQ->Logger, QUEUE_MODULE_ID, IPDR_LEVEL_DEBUG, "VolatileQueue_RemoveRecords",
                   "after: DSN(%llu) peekStarted=%d BH.Data=0x%p PeekPosition.Data=0x%p",
                   DSN, VQ->PeekStarted, BH.Data, VQ->PeekPosition.Data);

    Log_Write4(VQ->Logger, QUEUE_MODULE_ID, IPDR_LEVEL_DEBUG,
               "VolatileQueue_RemoveRecords", MSG_QUEUE_REMOVE_RECORDS,
               DSN, VQ->QueueSize, VQ->OutstandingRecords, *RemovedRecords);

    if Log_ShouldWrite(VQ->Logger, QUEUE_MODULE_ID, IPDR_LEVEL_DEBUG) {
        ChunksManager_Dump(&VQ->ChunksMgr, " +++ VolatileQueue_RemoveRecords (after) +++");
    }

    return IPDR_OK;
}

/**
 * Set the peek position to the beginning of the queue.
 *
 * @param VQ queue to rollback
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
VolatileQueue_Rollback(VolatileQueue *VQ)
{
    INIT_BUFFER_HANDLE(VQ->PeekPosition);
    VQ->OutstandingRecords = 0;
    VQ->PeekStarted = IPDR_FALSE;

    Log_Write(VQ->Logger, QUEUE_MODULE_ID, IPDR_LEVEL_DEBUG,
              "VolatileQueue_Rollback", MSG_QUEUE_ROLLBACK_QUEUE);

    return IPDR_OK;
}

/**
 * Get the number of outstanding records (unacknowledged records).
 *
 * @param VQ queue to rollback
 *
 * @return Number of outstanding records
 */
int
VolatileQueue_GetOutstandingRecordsCount(VolatileQueue *VQ)
{
    return VQ->OutstandingRecords;
}

/* ********************************************************** */

#if 0
static
unsigned long VolatileQueue_UsedMemory(VolatileQueue *VQ)
{
    return VQ->ChunksMgr.UsedMemory;
}
#endif

/**
 * Free the last record allocated on the queue.
 *
 * @param VQ queue to rollback
 * @param B allocated record
 * @param Size record size
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
VolatileQueue_ReleaseRecord(VolatileQueue *VQ,
                            BufferHandle *B,
                            unsigned int Size)
{
    return ChunksManager_ReleaseBuffer(B, Size);
}
