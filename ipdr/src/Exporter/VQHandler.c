/** @file Exporter/VQHandler.c
 *
 * QueueHandler which uses a VolatileQueue to store the queued
 * records.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: VQHandler.c,v 1.21 2005/04/26 13:52:41 dmitryk Exp $
 *
 * The VQHandler uses a volatile queue in order to store records until
 * they are acknowledged by the server (collector).
 */

#include <IPDR/Config.h>
const char VQHandler_rcsid[] = "@(#) $Id: VQHandler.c,v 1.21 2005/04/26 13:52:41 dmitryk Exp $" IPDR_COPYRIGHT_STR;

#include <Exporter/VQHandler.h>

const char VQHandler_h_rcsid[] = VQ_HANDLER_H_RCSID;

#ifdef VQ_HANDLER_SUPPORT

#include <Exporter/VolatileQueue.h>
#include <Exporter/Exporter.h>
#include <Exporter/ExporterMsgs.h>

static
IPDR_Status VQHandler_Initialize(Exporter *E, void *Arg)
{
    return IPDR_OK;
}

static
IPDR_Status VQHandler_Destroy(Exporter *E)
{
    return IPDR_OK;
}

static
IPDR_Status VQHandler_CreateSession(Exporter *E, Session *S)
{
    return IPDR_OK;
}

static
IPDR_Status VQHandler_DeleteSession(Exporter *E, Session *S)
{
    return IPDR_OK;
}

static
IPDR_Status VQHandler_CreateTransmissionContext(Exporter *E,
                                                 Session *S,
                                                 TransmissionContext *TC)
{
    VolatileQueue *VQ;
    IPDR_Status Status;

    /* allocate a new volatile queue */
    Platform_Check_MEMORYALLOCATE(E->PS, VQ, {});

    Status = VolatileQueue_Initialize(E->PS,
                                      & (E->ExporterLog),
                                      & (E->ParamsHandler),
                                      VQ);
    if (FAILURE(Status)) {
        Platform_MemoryFree(E->PS, VQ);
        return IPDR_OK;
    }

    TC->QueueHandlerData = VQ;

    return IPDR_OK;
}

static
IPDR_Status VQHandler_DeleteTransmissionContext(Exporter *E,
                                                 Session *S,
                                                 TransmissionContext *TC)
{
    VolatileQueue *VQ = (VolatileQueue *) TC->QueueHandlerData;
    VolatileQueue_Destroy(VQ);
    Platform_MemoryFree(E->PS, VQ);
    TC->QueueHandlerData = NULL;
    return IPDR_OK;
}

static
IPDR_Status VQHandler_SendData(Exporter *E, Session *S,
                                struct IPDR_tag_Template *Tmpl,
                                const void *Data)
{
    unsigned long RecordSize;
    BufferHandle BH;
    IPDR_Status Status;
    TransmissionContext *TC = S->ActiveContext;
    VolatileQueue *VQ = (VolatileQueue *) TC->QueueHandlerData;

    Status = Exporter_BuildRecord(& (VQ->ChunksMgr),
                                S, Tmpl, Data, & BH, & RecordSize);
    if (FAILURE(Status))
        return Status;

    Status = VolatileQueue_PushRecord(VQ, & BH);
    if (FAILURE(Status))
    {
        Status = VolatileQueue_ReleaseRecord(VQ, & BH, RecordSize);
        return Status;
    }

    return IPDR_OK;
}

static
IPDR_Status VQHandler_SendOutstandingData(Exporter *E, Session *S)
{
    BufferHandle BH;
    TransmissionContext *TC = S->ActiveContext;
    VolatileQueue *VQ = (VolatileQueue *) TC->QueueHandlerData;
    IPDR_Status ReturnStatus = IPDR_OK;
	unsigned long OutstandingRecords = 0 ;

	if(S->Retransmite == IPDR_TRUE)
	{
		if(VQ->QueueSize > 0 && VQ->OutstandingRecords > 0)
		{
			OutstandingRecords = VQ->OutstandingRecords;
			VQ->PeekStarted = IPDR_FALSE;
			INIT_BUFFER_HANDLE(VQ->PeekPosition);
		}
		S->Retransmite = IPDR_FALSE;
	}


    for (;;) {
        IPDR_Status Status;

        /* The "Immediate" flag here is the recommended value: there's
         * little advantage in trying to delay outputting the data
         * because most TCP stacks wait 200 ms before trying sending a
         * short packet. (And if the platform implementation has
         * turned on the "no delay" option for the socket ... well,
         * they shouldn't have done that because it's only for highly
         * interactive applications, such as telnet.) */

        IPDR_BOOL Immediate = IPDR_TRUE;

        ReturnStatus = Exporter_PrepareToSend(E, S) ;
        /* Bad return code from Exporter_PrepareToSend means that the window
           size is full or there's no active server (collector), so we won't send any
           more records and we should break out of this loop. */
        if (FAILURE(ReturnStatus)) {
            break;
        }

        /* peek the next record in the queue */
        Status = VolatileQueue_PeekRecord(VQ, & BH);

        Log_OutputStr2(&E->ExporterLog, QUEUE_MODULE_ID, IPDR_LEVEL_DEBUG,
                       "VQHandler_SendOutstandingData",
                       "peekStarted=%d PeekPosition.Data=0x%p",
                       VQ->PeekStarted, VQ->PeekPosition.Data);

        if (FAILURE(Status) || BUFFER_IS_NULL(BH))
            break;

        /* Send the record (if we have failed send, the disconnect callback
           will be triggered at the client which will result in a rollback
           once we're able to connect again */
		if(OutstandingRecords > 0)
			OutstandingRecords--;
        ReturnStatus = Exporter_SendRecord(E, S, & BH, Immediate, OutstandingRecords > 0);
        if (FAILURE(ReturnStatus)) {
            break;
        }
    }

    return ReturnStatus;
}

static
IPDR_Status VQHandler_ACK(Exporter *E, Session *S, CollectorSession *CS,
                           IPDR_DSN_TYPE DSN,
                           unsigned int *RemovedRecords)
{
    TransmissionContext *TC = S->ActiveContext;
    VolatileQueue *VQ = (VolatileQueue *) TC->QueueHandlerData;

    VolatileQueue_RemoveRecords(VQ, DSN, RemovedRecords);
    S->Statistics.AcknowledgedRecords += *RemovedRecords;
    /* The ACK might not have come on the active server (collector), so
       don't use S->ActiveServer */
    CS->Statistics.AcknowledgedRecords += *RemovedRecords;

    return IPDR_OK;
}

static
IPDR_Status VQHandler_CleanUP(Exporter *E, Session *S)
{
    unsigned int RemovedRecords;
    TransmissionContext *TC = S->ActiveContext;
    VolatileQueue *VQ = (VolatileQueue *) TC->QueueHandlerData;

	VolatileQueue_RemoveRecords_All(VQ, &RemovedRecords);

	S->Statistics.LostRecords += RemovedRecords;
	/*if (CS)
	{
		ACS->Statistics.LostRecords += RemovedRecords;
	}*/

	VolatileQueue_Rollback(VQ);

	return IPDR_OK;
}

/* NACK has been removed from the protocol, so this function is no
 * longer needed.
 */

static
IPDR_Status VQHandler_NACK(Exporter *E, Session *S, CollectorSession *CS,
                            IPDR_DSN_TYPE DSN)
{
    unsigned int RemovedRecords;
    TransmissionContext *TC = S->ActiveContext;
    VolatileQueue *VQ = (VolatileQueue *) TC->QueueHandlerData;

    VolatileQueue_RemoveRecords(VQ, DSN, &RemovedRecords);
    S->Statistics.AcknowledgedRecords += RemovedRecords;
    /* The NACK might not have come on the active server (collector), so
       don't use S->ActiveServer */
    CS->Statistics.AcknowledgedRecords += RemovedRecords;

    VolatileQueue_Rollback(VQ);

    return IPDR_OK;
}

static
IPDR_Status VQHandler_Rollback(Exporter *E, Session *S)
{
    TransmissionContext *TC = S->ActiveContext;
    VolatileQueue *VQ = (VolatileQueue *) TC->QueueHandlerData;
    return VolatileQueue_Rollback(VQ);
}

static
unsigned long VQHandler_OutstandingDataCount(Exporter *E, Session *S)

{
    TransmissionContext *TC = S->ActiveContext;
    if (TC == NULL) {
        return 0;
    } else {
        VolatileQueue *VQ = (VolatileQueue *) TC->QueueHandlerData;
        if (VQ == NULL) {
            return 0;
        } else {
            return VQ->OutstandingRecords;
        }
    }
}

static
unsigned long VQHandler_UnacknowledgedDataCount(Exporter *E,
                                                Session *S)
{
    TransmissionContext *TC = S->ActiveContext;
    if (TC == NULL) {
        return 0;
    } else {
        VolatileQueue *VQ = (VolatileQueue *) TC->QueueHandlerData;
        if (VQ == NULL) {
            return 0;
        } else {
            return VQ->QueueSize;
        }
    }
}

static
unsigned long VQHandler_UsedMemory(Exporter *E,
                                   Session *S)
{
    TransmissionContext *TC = S->ActiveContext;
    if (TC == NULL) {
        return 0;
    } else {
        VolatileQueue *VQ = (VolatileQueue *) TC->QueueHandlerData;
        if (VQ == NULL) {
            return 0;
        } else {
            return VQ->ChunksMgr.UsedMemory;
        }
    }
}

static
IPDR_Status VQHandler_Reconfigure(Exporter *E,
                                   Session *S)
{
    VolatileQueue *VQ;
    TransmissionContext *TC = S->ActiveContext;
    if (TC == NULL)
        return IPDR_OK;

    VQ = (VolatileQueue *) TC->QueueHandlerData;
    ChunksManager_Reconfigure(& (VQ->ChunksMgr),
                              STR_configuration_queueChunkSize,
                              STR_configuration_queueInitialNumChunks,
                              STR_configuration_queueMaxMemory,
                              & (E->ParamsHandler));

    return IPDR_OK;
}

QueueHandler VQHandler = {
    VQHandler_Initialize,
    VQHandler_Destroy,
    VQHandler_CreateSession,
    VQHandler_DeleteSession,
    VQHandler_CreateTransmissionContext,
    VQHandler_DeleteTransmissionContext,
    VQHandler_SendData,
    VQHandler_SendOutstandingData,
    VQHandler_ACK,
    VQHandler_NACK,
    VQHandler_Rollback,
    VQHandler_OutstandingDataCount,
    VQHandler_UnacknowledgedDataCount,
    VQHandler_UsedMemory,
    VQHandler_Reconfigure,
	VQHandler_CleanUP
};

#endif
