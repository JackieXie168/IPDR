/** @file Exporter/VolatileQueue.h
 *
 * Header file for volatile queue operation (the queue which holds
 * unacknowledged records), as defined in Exporter/VolatileQueue.c.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: VolatileQueue.h,v 1.11 2005/04/21 11:30:21 dmitryk Exp $
 *
 * The volatile queue is used to store unacknowledged
 *   records in memory. The queue supports the following
 *   operations:
 *
 *  <ul>
 *    <li>VolatileQueue_AllocateRecord() - allocates memory for a record within
 *                      the queue
 *    <li>VolatileQueue_PushRecord() - adds a record to the queue.
 *    <li>VolatileQueue_PeekRecord() - get the next record from the queue,
 *                  without removing it.
 *    <li>VolatileQueue_RemoveRecords() - removes records from the queue.
 *    <li>VolatileQueue_Rollback() - sets the peek position to the head of the
 *                queue.
 *  </ul>
 */
#ifndef _IPDR_VOLATILE_QUEUE_H_
#define _IPDR_VOLATILE_QUEUE_H_

#include <IPDR/Config.h>
#define VOLATILE_QUEUE_H_RCSID "@(#) $Id: VolatileQueue.h,v 1.11 2005/04/21 11:30:21 dmitryk Exp $" IPDR_COPYRIGHT_STR

#if defined(GLOBAL_RENAME)
#define VolatileQueue_Destroy       	IPDR__VolatileQueue_Destroy
#define VolatileQueue_GetOutstandingRecordsCount IPDR__VolatileQueue_GetOutstandingRecordsCount
#define VolatileQueue_Initialize    	IPDR__VolatileQueue_Initialize
#define VolatileQueue_PeekRecord    	IPDR__VolatileQueue_PeekRecord
#define VolatileQueue_PushRecord    	IPDR__VolatileQueue_PushRecord
#define VolatileQueue_ReleaseRecord 	IPDR__VolatileQueue_ReleaseRecord
#define VolatileQueue_RemoveRecords 	IPDR__VolatileQueue_RemoveRecords
#define VolatileQueue_Rollback      	IPDR__VolatileQueue_Rollback
#define VolatileQueue_RemoveRecords_All IPDR__VolatileQueue_RemoveRecords_All
#define VolatileQueue_h_rcsid       	IPDR__VolatileQueue_h_rcsid
#define VolatileQueue_rcsid         	IPDR__VolatileQueue_rcsid
#endif

#include <IPDR/Defs.h>
#include <Common/Protocol.h>
#include <Common/ChunksManager.h>
#include <Common/ParametersHandler.h>
#include <Common/ConfigurationNames.h>

#ifdef __cplusplus
extern "C" {
#endif

#if 0
} /* put this here to un-confuse Emacs's automatic indentation */
#endif

typedef struct
{
    ChunksManager ChunksMgr;
    Log *Logger;

    BufferHandle PeekPosition; /**< current peek position */
    IPDR_BOOL PeekStarted;    /**< indicates that peeking of the queue has started
                                  (used to eliminate cyclic peeks) */

    unsigned long QueueSize;            /**< number of records in queue */
    unsigned long OutstandingRecords;   /**< number of unacknowledged records */
}
VolatileQueue;

#define VolatileQueue_IsEmpty(VQ) ((VQ)->QueueSize == 0)

IPDR_Status VolatileQueue_Initialize(IPDR_PlatformServices *PS,
                                      Log *L,
                                      ParametersHandler *ParamsHandler,
                                      VolatileQueue *VQ);
IPDR_Status VolatileQueue_Destroy(VolatileQueue *VQ);
IPDR_Status VolatileQueue_AllocateRecord(VolatileQueue *VQ,
                                          unsigned int Size,
                                          BufferHandle *B);
IPDR_Status VolatileQueue_ReleaseRecord(VolatileQueue *VQ,
                                         BufferHandle *B,
                                         unsigned int Size);
IPDR_Status VolatileQueue_PushRecord(VolatileQueue *VQ, BufferHandle *B);
IPDR_Status VolatileQueue_PeekRecord(VolatileQueue *VQ, BufferHandle *B);
IPDR_Status VolatileQueue_RemoveRecords(VolatileQueue *VQ,
                                         IPDR_DSN_TYPE DSN,
                                         unsigned int *RemovedRecords);
IPDR_Status VolatileQueue_Rollback(VolatileQueue *VQ);
int VolatileQueue_GetOutstandingRecordsCount(VolatileQueue *VQ);
IPDR_Status
VolatileQueue_RemoveRecords_All(VolatileQueue *VQ,
                            unsigned int *RemovedRecords);

#ifdef __cplusplus
}
#endif

#endif
