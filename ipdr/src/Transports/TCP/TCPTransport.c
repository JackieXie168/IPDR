/** @file Transports/TCP/TCPTransport.c
 *
 * @brief Implementation of IPDR's TCP based transport.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * @note
 * XXX Some of the code here is not needed any more. There used to
 * be combined code for the client (transmitter) and server; the client (transmitter) is "passive"
 * (accepts connections) and the server (collector) is "active" (tries to
 * connect). But it's not a lot of code, and does no particular harm
 * (besides making things a bit more complicated than necessary).
 */

#include <IPDR/Config.h>
const char TCPTransport_rcsid[] = "@(#) $Id: TCPTransport.c,v 1.32 2005/05/17 10:19:29 dmitryk Exp $" IPDR_COPYRIGHT_STR ;

#include <Transports/TCP/TCPTransport.h>
#include <Transports/TCP/TCPMsgs.h>

#include <Common/Transport.h>

const char TCPTransport_h_rcsid[] = TCP_TRANSPORT_H_RCSID;
const char Transport_h_rcsid[] = TRANSPORT_H_RCSID;
const char TCPMsgs_h_rcsid[] = TCP_MSGS_H_RCSID;


#include <fcntl.h>
#include <string.h>
#include <stdio.h>

#include <IPDR/Defs.h>
#include <Common/Protocol.h>
#include <Common/ConfigurationNames.h>

/*************** DANGER, WILL ROBINSON ********************************
 * There's a problem with disconnections ... both the receive and the *
 * send can notice and they can both try to clean up.  Not a pretty   *
 * sight. So, we have to keep a reference count on the connection, to *
 * ensure that both the receive and the send have finished before     *
 * cleanup happens (if one gets a TCP "reset", the other will         *
 * eventually get it also).                                           *
 **********************************************************************/

#define TRANSPORT_PLATFORM (T->PS)
#define TRANSPORT_LOG (T->Logger)
#define CONNECTION_LOG(C) ((/*(TCPTransport *)*/ (C)->Transport)->Logger)

#define MAX_RECV_MESSAGE_LENGTH 20000 /* a hack to prevent problems with corrupted messages */

#define RECV_TIMEOUT 4000
/* milliseconds: set to 0 for no timeout */

static void TCP_ConnectionCallback(IPDR_SocketHandle FD, void *Arg);
static void TCP_ConnectionWriteCallback(IPDR_SocketHandle FD, void *Arg);
static IPDR_Status TCP_SendBuffers(TCPConnection *C);


IPDR_Status TCP_ReceiveHeader(	TCPTransport *T,
								IPDR_SocketHandle FD,
								int Timeout,
								MessageHeader* Header );

IPDR_Status TCP_ReceiveMsgBody(	TCPConnection *C,
                         		int Timeout,
                         		BufferHandle *BH,
								MessageHeader* Header );

IPDR_Status TCP_CreateConnection(	TCPTransport *T,
								IPDR_SocketHandle FD,
								IPDR_IPAddress RemoteAddr,
								unsigned short RemotePort,
								TCPConnection **C );


IPDR_Status TCPMsgs_Initialize(Log *L)
{
    return Log_RegisterModule(L, TCP_MODULE_ID, "TCP");
}

IPDR_IPAddress TCP_ParseAddress(const char *Str)
{
    unsigned int Bytes[4];
    IPDR_IPAddress Res;

	if( Str && strlen(Str) > 0)
    	sscanf(Str, "%u.%u.%u.%u", & Bytes[0], & Bytes[1], & Bytes[2], & Bytes[3]);
	else
		Bytes[0] = Bytes[1] = Bytes[2] = Bytes[3] = 0;
    Res = (Bytes[0] << 24) | (Bytes[1] << 16) | (Bytes[2] << 8) | Bytes[3];
    return Res;
}

char *TCP_PrintAddress(IPDR_IPAddress Addr, char* AddressStr)
{
    /* Or use inet_ntoa() */
    sprintf(AddressStr, "%u.%u.%u.%u",
            (Addr>>24) & 0xffu,
            (Addr>>16) & 0xffu,
            (Addr>> 8) & 0xffu,
            (Addr>> 0) & 0xffu);

    return AddressStr;
}

char *TCP_PrintAddressAndPort(IPDR_IPAddress Addr, unsigned int Port, char* AddressStr)
{
    /* Or use inet_ntoa() */
    sprintf(AddressStr, "%d.%d.%d.%d:%u",
            (Addr>>24) & 0xff,
            (Addr>>16) & 0xff,
            (Addr>> 8) & 0xff,
            (Addr>> 0) & 0xff,
            Port);

    return AddressStr;
}

char *TCP_PrintConnectionRemote(Connection C, char *AddressStr)
{
	/* Or use inet_ntoa() */
    sprintf(AddressStr, "%d.%d.%d.%d:%u(%d.%d.%d.%d)",
            (C->RemoteAddr>>24) & 0xff,
            (C->RemoteAddr>>16) & 0xff,
            (C->RemoteAddr>> 8) & 0xff,
            (C->RemoteAddr>> 0) & 0xff,
            C->RemotePort,
			(C->DummyAddr>>24) & 0xff,
            (C->DummyAddr>>16) & 0xff,
            (C->DummyAddr>> 8) & 0xff,
            (C->DummyAddr>> 0) & 0xff);

    return AddressStr;
}
char *TCP_PrintConnectionLocale(Connection C, char *AddressStr)
{
	/* Or use inet_ntoa() */
    sprintf(AddressStr, "%d.%d.%d.%d:%u",
            (C->LocaleAddr>>24) & 0xff,
            (C->LocaleAddr>>16) & 0xff,
            (C->LocaleAddr>> 8) & 0xff,
            (C->LocaleAddr>> 0) & 0xff,
            C->LocalePort);

    return AddressStr;
}
char *TCP_PrintConnectionFull(Connection C, char *AddressStr)
{
	/* Or use inet_ntoa() */
    sprintf(AddressStr, "%d.%d.%d.%d:%u(%d.%d.%d.%d)->%d.%d.%d.%d:%u",
            (C->RemoteAddr>>24) & 0xff,
            (C->RemoteAddr>>16) & 0xff,
            (C->RemoteAddr>> 8) & 0xff,
            (C->RemoteAddr>> 0) & 0xff,
            C->RemotePort,
			(C->DummyAddr>>24) & 0xff,
            (C->DummyAddr>>16) & 0xff,
            (C->DummyAddr>> 8) & 0xff,
            (C->DummyAddr>> 0) & 0xff,
			(C->LocaleAddr>>24) & 0xff,
            (C->LocaleAddr>>16) & 0xff,
            (C->LocaleAddr>> 8) & 0xff,
            (C->LocaleAddr>> 0) & 0xff,
            C->LocalePort);

    return AddressStr;
}

/* The following function shouldn't be in this file - it ought to be
 * part of Log.c, because it uses internal knowledge about the Log
 * structure; but we had some problems with cirucular header files
 * when we tried to put it in Log.h. No doubt this can be solved some
 * time. */

static
IPDR_Status Log_LogPlatformConnError(Log *L,
                                      IPDR_ModuleID ModuleID,
                                      const char *Func,
                                      const char *PlatformFunc,
                                      Connection Conn,
                                      IPDR_Status Status)
{
    Platform_GetLastErrorMessage(L->PS, L->msgStrErr);

    /* XXX Special case for end-of-file ... this is not as clean as it
     * should be because the generic platform reports end-of-file as a
     * connection error (it is, in a way) ... we depend on the error
     * code being "No error" [which it isn't really] to avoid a
     * confusing error message.
     */

    if (Status == IPDR_ERR_COMMUNICATION_ERROR &&
        0 == strcmp(L->msgStrErr, IPDR_NO_ERROR_ERROR_MESSAGE)) {
        Log_Write3(L, ModuleID, IPDR_LEVEL_WARNING, Func,
                   MSG_LOG_PLATFORM_CONN_EOF,
                   PlatformFunc,
                   IPDR_StatusMsg(NULL, Status),
                   TL_GetConnectionInfoStr(Conn));

    } else {
        /* XXX should signal error event */
        Log_Write4(L, ModuleID, IPDR_LEVEL_ERROR, Func,
                   MSG_LOG_PLATFORM_CONN_ERR,
                   PlatformFunc, L->msgStrErr,
                   IPDR_StatusMsg(NULL, Status),
                   TL_GetConnectionInfoStr(Conn));
    }
    return IPDR_OK;
}

/* This function also ought to be in Log.c */

IPDR_Status Log_LogPlatformSocketError(Log *L,
                                        IPDR_ModuleID ModuleID,
                                        const char *Func,
                                        const char *PlatformFunc,
                                        IPDR_IPAddress Addr,
                                        unsigned short Port,
                                        IPDR_SocketHandle FD)
{
    TL_PABuf paBuf;
    Platform_GetLastErrorMessage(L->PS, L->msgStrErr);
    /* XXX should signal error event */
    Log_Write4(L, ModuleID, IPDR_LEVEL_ERROR, Func,
               MSG_LOG_PLATFORM_SOCKET_ERR,
               PlatformFunc, L->msgStrErr,
               TL_PrintAddressAndPort(Addr, Port, paBuf), Platform_SOCKET_DISPLAY(FD));
    return IPDR_OK;
}

static
IPDR_Status TCP_InitializeParameters(TCPTransport *T)
{
    T->FlushInterval =
        ParametersHandler_GetIntParam(& (T->ParamsHandler),
                                      STR_configuration_transport_tcpTransport_flushInterval);
    return IPDR_OK;
}

static
void TCP_FlushTimerCallback(void *Arg)
{
    TCPConnection *C = /*(TCPConnection *)*/ Arg;
    TCPTransport *T = /*(TCPTransport *)*/ C->Transport;

    Platform_Lock(TRANSPORT_PLATFORM);
    /* ++C->InUseCount; ... this is done by TCP_SendBuffers() */

    Log_OutputStr1(TRANSPORT_LOG, TCP_MODULE_ID, IPDR_LEVEL_DEBUG, "TCP_FlushTimerCallback",
                   "TCP_FlushTimerCallback %.99s", TCP_GetConnectionInfoStr(C));

    TCP_SendBuffers(C);

    /* --C->InUseCount; */
    Platform_Unlock(TRANSPORT_PLATFORM);
}

static
IPDR_Status TCP_AllocateConnection(TCPTransport *T,
                                    IPDR_IPAddress Addr,
                                    unsigned short Port,
                                    ChunkAllocator Alloc,
                                    TCPConnection **C)
{
    IPDR_Status Status;
    TCPConnection *C1;
    ChunkAllocator SendBuffersAlloc;

    Status = Platform_MEMORYALLOCATE(TRANSPORT_PLATFORM, C1);
    if (FAILURE(Status)) {
        /* XXX should signal error event */
        Log_Write1(TRANSPORT_LOG, TCP_MODULE_ID, IPDR_LEVEL_ERROR,
                   "TCP_AllocateConnection", MSG_TCP_ALLOC_FAILED, sizeof (TCPConnection));
        X_MEMPANIC(C1, __FILE__, __LINE__);
        return Status;
    }

    strncpy(C1->eyecatcher, "TCPCONN", sizeof C1->eyecatcher);
    C1->RemoteAddr = Addr;
    C1->RemotePort = Port;
    C1->Transport = /*(TCPTransport *)*/ T;
    C1->Handlers = NULL;
    C1->Arg = NULL;
    C1->ConnectionTimer = NULL;
    C1->FlushTimer = NULL;
    C1->WriteEventID = NULL;
    C1->ReadEventID = NULL;
    C1->Disconnected = IPDR_FALSE;
    C1->InUseCount = 0;

    Status = ChunksManager_Initialize(
        0,
        0,
        0,
        0,
        STR_configuration_transport_tcpTransport_dataRcvChunkSize,
        STR_configuration_transport_tcpTransport_dataRcvInitialNumChunks,
        STR_configuration_transport_tcpTransport_dataRcvMaxMemory,
        & T->ParamsHandler,
        Alloc,
        TRANSPORT_LOG,
        & (C1->ReceivedData));

    if (FAILURE(Status)) {
        /* XXX should signal error event */
        Log_Write(TRANSPORT_LOG, TCP_MODULE_ID, IPDR_LEVEL_ERROR,
                  "TCP_AllocateConnection(ReceivedData)", MSG_TCP_INIT_CHUNKS_FAILED);
        strncpy(C1->eyecatcher, "Dtcpcon", sizeof C1->eyecatcher);
        Platform_MemoryFree(TRANSPORT_PLATFORM, C1);
        return Status;
    }

    ChunksManager_GetDefaultChunkAllocator(T->PS, & SendBuffersAlloc);
    Status = ChunksManager_Initialize(
        0,
        0,
        0,
        CHUNK_TAG_RELEASE_AFTER_SEND,
        STR_configuration_transport_tcpTransport_dataSndChunkSize,
        STR_configuration_transport_tcpTransport_dataSndInitialNumChunks,
        STR_configuration_transport_tcpTransport_dataSndMaxMemory,
        & T->ParamsHandler,
        Alloc,
        TRANSPORT_LOG,
        & (C1->SentData));
    if (FAILURE(Status)) {
        /* XXX should signal error event */
        Log_Write(TRANSPORT_LOG, TCP_MODULE_ID, IPDR_LEVEL_ERROR,
                  "TCP_AllocateConnection(SentData)", MSG_TCP_INIT_CHUNKS_FAILED);
        ChunksManager_Destroy(& (C1->ReceivedData));
        strncpy(C1->eyecatcher, "Dtcpcon", sizeof C1->eyecatcher);
        Platform_MemoryFree(TRANSPORT_PLATFORM, C1);
        return Status;
    }

    Status = ChunksManager_Initialize(
        0,
        0,
        0,
        0,
        STR_configuration_transport_tcpTransport_sendBuffersChunkSize,
        STR_configuration_transport_tcpTransport_sendBuffersInitialNumChunks,
        STR_configuration_transport_tcpTransport_sendBuffersMaxMemory,
        & T->ParamsHandler,
        SendBuffersAlloc,
        TRANSPORT_LOG,
        & (C1->SendBuffers));
    if (FAILURE(Status)) {
        ChunksManager_Destroy(& (C1->SentData));
        ChunksManager_Destroy(& (C1->ReceivedData));
        /* XXX should signal error event */
        Log_Write(TRANSPORT_LOG, TCP_MODULE_ID, IPDR_LEVEL_ERROR,
                  "TCP_AllocateConnection(SendBuffers)", MSG_TCP_INIT_CHUNKS_FAILED);
        strncpy(C1->eyecatcher, "Dtcpcon", sizeof C1->eyecatcher);
        Platform_MemoryFree(TRANSPORT_PLATFORM, C1);
        return Status;
    }

    *C = C1;

    return IPDR_OK;
}

/**
 * Handle disconnection of the socket
 *   associated with a connection.
 *
 * @param C connection
 *
 */
void TCP_HandleDisconnection(TCPConnection *C)
{
    TCPTransport *T = /*(TCPTransport *)*/ C->Transport;
    IPDR_Status Status;
    TCPConnection *C1;

    Platform_Lock(TRANSPORT_PLATFORM);

    Log_Write3(TRANSPORT_LOG, TCP_MODULE_ID, IPDR_LEVEL_DEBUG,
               "TCP_HandleDisconnection",
               MSG_TCP_DISCONNECT,
               TCP_GetConnectionInfoStr(C), Platform_SOCKET_DISPLAY(C->FD), C->InUseCount);

    /* It is possible that a problem will be discovered on just the read
       or the write (e.g., out of memory) ... we force a problem on the
       other side of the socket by closing it here (actually, doing a
       shutdown ... a close could deallocate the FD, which might be a
       bad idea if there's still another read or write open on
       socket). */

    Status = Platform_Shutdown(TRANSPORT_PLATFORM, C->FD);
    if (FAILURE(Status)) {
        Log_LogPlatformConnError(TRANSPORT_LOG, TCP_MODULE_ID,
                                 "TCP_HandleDisconnection",
                                 "Platform_Shutdown", C, Status);
    }

    if (C->InUseCount > 0) {
        Platform_Unlock(TRANSPORT_PLATFORM);

        /* There should be no hurry to unregisters the callbacks ...
           we'll eventually take care of them (and the rest of the
           connection) when the use count drops to zero (e.g., we got an
           error while doing a read; but there was also a write going on
           ... the write will find the problem also and the removal of the
           connection will happen then. */

        return ;
    }

    Platform_UnregisterSocketAndClear(TRANSPORT_PLATFORM, &C->ReadEventID);
    Platform_UnregisterSocketAndClear(TRANSPORT_PLATFORM, &C->WriteEventID);

    Platform_UnregisterTimerAndClear(TRANSPORT_PLATFORM, &C->ConnectionTimer);
    Platform_UnregisterTimerAndClear(TRANSPORT_PLATFORM, &C->FlushTimer);

    /* delete the connection (in case a passive connection) */
    C1 = T->Connections;
    if (C == C1) {
        T->Connections = C->NextConnection;
    } else {
        while (C1 != NULL) {
            if (C1->NextConnection == C) {
                C1->NextConnection = C->NextConnection;
                break ;
            }
            C1 = C1->NextConnection;
        }
    }

	if (C->Handlers != NULL) {
        C->Handlers->Disconnect(C, C->Arg);
    }

	Platform_SleepMS(TRANSPORT_PLATFORM, 1);

    Log_Write2(TRANSPORT_LOG, TCP_MODULE_ID, IPDR_LEVEL_DEBUG, "TCP_HandleDisconnection",
               MSG_TCP_CLOSE, TCP_GetConnectionInfoStr(C), Platform_SOCKET_DISPLAY(C->FD));
    Platform_CloseAndInvalidate(TRANSPORT_PLATFORM, &C->FD);

    ChunksManager_Destroy(& (C->SentData));
    ChunksManager_Destroy(& (C->ReceivedData));
    ChunksManager_Destroy(& (C->SendBuffers));

    strncpy(C->eyecatcher, "Dtcpcon", sizeof C->eyecatcher);
    Platform_MemoryFree(TRANSPORT_PLATFORM, C);

    Platform_Unlock(TRANSPORT_PLATFORM);
}

/**
 *   Send any pending data over TCP.
 *   Assumes that it is called in the context of a PlatformLock
 *
 * @param C connection
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
#include <Exporter/ProtocolDebug.h>

static
IPDR_Status TCP_SendBuffers(TCPConnection *C)
{
    TCPTransport *T = /*(TCPTransport *)*/ C->Transport;

    ++C->InUseCount;

    Log_OutputStr1(TRANSPORT_LOG, TCP_MODULE_ID, IPDR_LEVEL_DEBUG, "TCP_SendBuffers",
                   "%.120s", TCP_GetConnectionInfoStr(C));

    if (Log_ShouldWrite(TRANSPORT_LOG, TCP_MODULE_ID, IPDR_LEVEL_DEBUG)) {
        BufferHandle SBH;
        ChunksManager_Dump(&C->SendBuffers, " +++ TCP_SendBuffers (SendBuffers) +++");
        ChunksManager_Dump(&C->SentData, " +++ TCP_SendBuffers (Sent) +++");
        Log_OutputStr(TRANSPORT_LOG, TCP_MODULE_ID, IPDR_LEVEL_DEBUG, "TCP_SendBuffers",
                      "+++ dumping send buffers +++");
        for (ChunksManager_GetFirstBuffer(&C->SendBuffers, &SBH);
             ! BUFFER_IS_NULL(SBH);
             ChunksManager_GetNextBuffer(&SBH, sizeof (TCPSendBuffer), &SBH)) {
            TCPSendBuffer *SB = (TCPSendBuffer *)SBH.Data;
            const char *p = SB->StartBuffer.Data;
            Log_OutputStr1(TRANSPORT_LOG, TCP_MODULE_ID, IPDR_LEVEL_DEBUG, "TCP_SendBuffers",
                           "SB->Size = %d", SB->Size);
            while (p < SB->StartBuffer.Data + SB->Size) {
                const MessageHeader *Header = (const MessageHeader *) p;
                Log_OutputStr6(TRANSPORT_LOG, TCP_MODULE_ID, IPDR_LEVEL_DEBUG, "TCP_SendBuffers",
                               "Version=%u MessageID=%u (0x%02x) MessageFlags=0x%04x SessionID=%d MessageLength=%lu",
                               Header->Version, Header->MessageID, Header->MessageID, Header->MessageFlags,
                               Header->SessionID, (unsigned long)(IPDR_NTOHL(Header->MessageLength)));
                /* ProtocolDebug_PrintOutgoingMessage(TRANSPORT_LOG, C, p, IPDR_LEVEL_DEBUG); */
                if (IPDR_NTOHL(Header->MessageLength) < sizeof *Header ||
					 TCP_TRUNSPORT_MAX_MSG <= IPDR_NTOHL(Header->MessageLength) ) { /* sanity check */
                    /* XXX should signal error event */
                    Log_OutputStr1(TRANSPORT_LOG, TCP_MODULE_ID, IPDR_LEVEL_ERROR, "TCP_SendBuffers",
                                   "Bad message length: %lu", (unsigned long)(IPDR_NTOHL(Header->MessageLength)));
                    break;
                }
                p += IPDR_NTOHL(Header->MessageLength);
            }
        }
        Log_OutputStr(TRANSPORT_LOG, TCP_MODULE_ID, IPDR_LEVEL_DEBUG, "TCP_SendBuffers",
                      "+++ END dumping send buffers +++");
    }

    /* send the currently queued send buffers */

    for (;;) {
        BufferHandle SBH;
        TCPSendBuffer *SB;
        IPDR_Status Status;
        unsigned int WB;

        ChunksManager_GetFirstBuffer(& (C->SendBuffers), & SBH);
        if (BUFFER_IS_NULL(SBH))
            break ;

        SB = (TCPSendBuffer *) SBH.Data;

        Log_OutputStr3(TRANSPORT_LOG, TCP_MODULE_ID, IPDR_LEVEL_DEBUG, "TCP_SendBuffers",
                       "send(%u)@0x%p %.99s", SB->Size, SB->StartBuffer.Data, TCP_GetConnectionInfoStr(C));

        Status = Platform_Send(TRANSPORT_PLATFORM, C->FD,
                               SB->StartBuffer.Data, SB->Size, & WB);

        Log_OutputStr3(TRANSPORT_LOG, TCP_MODULE_ID, IPDR_LEVEL_DEBUG, "TCP_SendBuffers",
                       "send(%u) wrote %u bytes with rc(%.99s)",
                       SB->Size, WB, IPDR_StatusMsg(NULL, Status));

        if (FAILURE(Status)) {
            /* XXX should signal error event */
            Log_Write3(CONNECTION_LOG(C), TCP_MODULE_ID, IPDR_LEVEL_ERROR, "TCP_SendBuffers",
                       MSG_TCP_SEND_FAILED,
                       TCP_GetConnectionInfoStr(C),
                       IPDR_StatusMsg(NULL, Status), Platform_SOCKET_DISPLAY(C->FD));
            --C->InUseCount;
            TCP_HandleDisconnection(C);
            return Status;
        }

        if (WB < SB->Size) {

            /* if the transport is responsible for releasing the chunks,
               release the sent bytes */
            if (BUFFER_TAGS(SB->StartBuffer) & CHUNK_TAG_RELEASE_AFTER_SEND) {
                ChunksManager_ReleaseBuffer(& (SB->StartBuffer), WB);
            } else {
                Log_OutputStr3(TRANSPORT_LOG, TCP_MODULE_ID, IPDR_LEVEL_DEBUG, "TCP_SendBuffers",
                               "WB(%u) < SB->Size(%u) SB->StartBuffer.Data=0x%p",
                               WB, SB->Size, SB->StartBuffer.Data)
                    SB->StartBuffer.Data += WB;
                SB->Size -= WB;
                Log_OutputStr2(TRANSPORT_LOG, TCP_MODULE_ID, IPDR_LEVEL_DEBUG, "TCP_SendBuffers",
                               " ... becomes SB->Size(%u) SB->StartBuffer.Data=0x%p",
                               SB->Size, SB->StartBuffer.Data);
                    }

            if (C->WriteEventID == NULL) {
#ifdef TRACE_MAINFUNC
                fprintf(stderr, "RegisterWriteSocket " Platform_SOCKET_FMT " [SendBuffers] ConnectionCallback\n", Platform_SOCKET_DISPLAY(C->FD)); fflush(stderr);
#endif
                Platform_RegisterWriteSocket(TRANSPORT_PLATFORM, C->FD,
                                             TCP_ConnectionWriteCallback, C,
                                             & (C->WriteEventID));
            } else {
#ifdef TRACE_MAINFUNC
                fprintf(stderr, "(null)WriteSocket " Platform_SOCKET_FMT " [SendBuffers] ConnectionCallback\n", Platform_SOCKET_DISPLAY(C->FD)); fflush(stderr);
#endif
            }

            --C->InUseCount;
            return IPDR_OK;
        }

        /* release the send buffer if it was completely sent */
        if (BUFFER_TAGS(SB->StartBuffer) & CHUNK_TAG_RELEASE_AFTER_SEND) {
            ChunksManager_ReleaseBuffer(& (SB->StartBuffer), SB->Size);
        } else {
            Log_OutputStr3(TRANSPORT_LOG, TCP_MODULE_ID, IPDR_LEVEL_DEBUG, "TCP_SendBuffers",
                           "WB(%u) == SB->Size(%u) SB->StartBuffer.Data=0x%p",
                           WB, SB->Size, SB->StartBuffer.Data)
                SB->StartBuffer.Data += SB->Size;
            SB->Size = 0;
            Log_OutputStr2(TRANSPORT_LOG, TCP_MODULE_ID, IPDR_LEVEL_DEBUG, "TCP_SendBuffers",
                           " ... becomes SB->Size(%u) SB->StartBuffer.Data=0x%p",
                           SB->Size, SB->StartBuffer.Data);
                }

        ChunksManager_ReleaseBuffer(& SBH, sizeof(TCPSendBuffer));
    }

    Platform_UnregisterSocketAndClear(TRANSPORT_PLATFORM, &C->WriteEventID);

    --C->InUseCount;
    return IPDR_OK;
}

/**
 *   Callback triggered whenever a socket becomes writable
 *   (used for performing non blocking writes).
 *
 * @param FD file (socket) descriptor
 * @param Arg extra argument to the callback
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static void TCP_ConnectionWriteCallback(IPDR_SocketHandle FD, void *Arg)
{
    TCPConnection *C = /*(TCPConnection *)*/ Arg;
    TCPTransport *T = /*(TCPTransport *)*/ C->Transport;

    Platform_Lock(TRANSPORT_PLATFORM);
    /** ++C->InUseCount; **/
    TCP_SendBuffers(C);
    /** --C->InUseCount; **/
    Platform_Unlock(TRANSPORT_PLATFORM);
}

/**
 *   Callback triggered whenever a socket becomes readable.
 *
 * @param FD socket file descriptor
 * @param Arg callback argument (the connection)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static void TCP_ConnectionCallback(IPDR_SocketHandle FD,
                                   void *Arg)
{
    unsigned int B;
    unsigned int MessageLength, Pos, RemainingBytes;
    unsigned int TotalBufferSize;
    TCPConnection *C = /*(TCPConnection *)*/ Arg;
    TCPTransport *T = /*(TCPTransport *)*/ C->Transport;
    MessageHeader *Header;

	/* This temporary header is here because the data is not padded any more and we can not work with
	arbitrary chanks ...*/
	MessageHeader tmpHeader;

    IPDR_Status Status;
    BufferHandle BH, NewBH;
    Platform_Lock(TRANSPORT_PLATFORM);

    ++C->InUseCount;

    Log_Write1(CONNECTION_LOG(C), TCP_MODULE_ID, IPDR_LEVEL_DEBUG,
               "TCP_ConnectionCallback",
               "%.199s", TCP_GetConnectionInfoStr(C));

    if (! IPDR_SocketEq(FD, C->FD)) {
        Log_Write3(CONNECTION_LOG(C), TCP_MODULE_ID, IPDR_LEVEL_FATAL,
                   "TCP_ConnectionCallback",
                   MSG_TCP_INVALID_CONN_FD,
                   TCP_GetConnectionInfoStr(C),
                   Platform_SOCKET_DISPLAY(FD), Platform_SOCKET_DISPLAY(C->FD));
        --C->InUseCount;
        TCP_HandleDisconnection(C);
        return ;
    }

    /* get a maximum size buffer from the receive chunks manager and read
       data into it */

    Status = ChunksManager_AllocateMaxBuffer(& (C->ReceivedData), & BH,
                                             & TotalBufferSize);
    if (FAILURE(Status)) {
        /* XXX should signal error event */
        Log_Write2(CONNECTION_LOG(C), TCP_MODULE_ID, IPDR_LEVEL_ERROR, "TCP_ConnectionCallback[1]",
                   MSG_TCP_ALLOC_MAXBUFFER_FAILED,
                   TCP_GetConnectionInfoStr(C), Platform_SOCKET_DISPLAY(FD));
        --C->InUseCount;
        TCP_HandleDisconnection(C);
        Platform_Unlock(TRANSPORT_PLATFORM);
        return ;
    }

    Platform_Unlock(TRANSPORT_PLATFORM);
    Status = Platform_Recv(TRANSPORT_PLATFORM, FD, BH.Data, TotalBufferSize,
                           RECV_TIMEOUT, & B);

    Log_Write4(CONNECTION_LOG(C), TCP_MODULE_ID, IPDR_LEVEL_DEBUG,
               "TCP_ConnectionCallback",
               "recv %.199s bytes=%u (bufsize=%u) status=%.99s",
               TCP_GetConnectionInfoStr(C), B, TotalBufferSize, IPDR_StatusMsg(NULL, Status));

    /* if the connection is closed (since another thread is currently closing it,
       do nothing) */

    Platform_Lock(TRANSPORT_PLATFORM);

    if (C->Disconnected) {
#ifdef TRACE_MAINFUNC
        fprintf(stderr, "*** after Recv[1] ... disconnected\n"); fflush(stderr);
#endif
        --C->InUseCount;
        Platform_Unlock(TRANSPORT_PLATFORM);
        return ;
    }

    if (FAILURE(Status)) {
        Log_LogPlatformConnError(TRANSPORT_LOG, TCP_MODULE_ID,
                                 "TCP_ConnectionCallback",
                                 "Platform_Recv[1]", C, Status);
        /* if the socket is still registered for write, do nothing (used to solve
           problems with implementations which try to invoke the write callback
           in any case, even if it was unregistered, since the unregistering
           is queued for later invocation */
        if (C->WriteEventID != NULL) {
            ChunksManager_TruncateChunk(& BH);
            --C->InUseCount;
            Platform_Unlock(TRANSPORT_PLATFORM);
            return ;
        }

        --C->InUseCount;
        TCP_HandleDisconnection(C);
        Platform_Unlock(TRANSPORT_PLATFORM);
        return ;
    } else if (B == 0) {
        ChunksManager_TruncateChunk(& BH);
        /* no data was received due to a timeout */
        Log_Write2(TRANSPORT_LOG, TCP_MODULE_ID, IPDR_LEVEL_DEBUG, "TCP_ConnectionCallback",
                   "No data due to timeout: %.199s handlers=%p",
                   TCP_GetConnectionInfoStr(C), C->Handlers);
        if (C->Handlers != NULL) {
            C->Handlers->Data(C, NULL, 0, C->Arg);
        }
        --C->InUseCount;
        Platform_Unlock(TRANSPORT_PLATFORM);
        return ;
    }

    Pos = 0;
    RemainingBytes = B;

    /* We are still platform locked here. We keep things locked,
       unlocking whenever a receive is done. */

    while (RemainingBytes > 0) {
#ifdef TRACE_MAINFUNC
        fprintf(stderr, "Remaining bytes: %d\n", RemainingBytes); fflush(stderr);
#endif
        if (RemainingBytes >= sizeof(MessageHeader)) {
            Header = (MessageHeader *) BH.Data;
			memcpy( &tmpHeader, BH.Data, sizeof(MessageHeader));
            //MessageLength = IPDR_NTOHL(Header->MessageLength);
			Header = &tmpHeader ;
			MessageLength = IPDR_NTOHL(Header->MessageLength);
            if (MessageLength < sizeof *Header ||
                Header->Version != IPDR_PROTOCOL_VERSION) {
                /* XXX should signal error event */
                Log_Write7(TRANSPORT_LOG, TCP_MODULE_ID, IPDR_LEVEL_ERROR, "TCP_ConnectionCallback",
                           MSG_TCP_RECEIVE_CORRUPTED,
                           TCP_GetConnectionInfoStr(C),
                           MessageLength,
                           Header->Version,
                           Header->MessageID,
                           Header->MessageFlags,
                           Header->SessionID,
                           Platform_SOCKET_DISPLAY(FD));
                /* This is a disaster ... all we can do is shut things down
                 * and hope that when we reconnect, everything will get
                 * reset. But we have a problem: if there's a pending write,
                 * it won't see the problem because it's not due to an I/O
                 * error. So, let's try to force an error just in case (not
                 * guaranteed to always work on all platforms ... but this is
                 * a situation that should never occur anyway). Note that
                 * there's a close inside TCP_HandleDisconnection() .. it'll
                 * just get an error. */
                --C->InUseCount;
                Platform_CloseAndInvalidate(TRANSPORT_PLATFORM, &FD); /* Maybe not needed but no harm done */
                TCP_HandleDisconnection(C);
                Platform_Unlock(TRANSPORT_PLATFORM);
                return ;
            }
            if (RemainingBytes >= MessageLength) {

                Log_Write3(TRANSPORT_LOG, TCP_MODULE_ID, IPDR_LEVEL_DEBUG, "TCP_ConnectionCallback",
                           MSG_TCP_RECEIVE,
                           TCP_GetConnectionInfoStr(C),
                           MessageLength, Platform_SOCKET_DISPLAY(FD));

                /* if this is the last message in the chunk, truncate the chunk
                   to point after the last message */
                if (RemainingBytes == MessageLength) {
                    ChunksManager_TruncateChunkAfter(& BH, MessageLength);
                }

                Log_Write3(TRANSPORT_LOG, TCP_MODULE_ID, IPDR_LEVEL_DEBUG, "TCP_ConnectionCallback",
                           "Got data: %.199s handlers=%p length=%d",
                           TCP_GetConnectionInfoStr(C), C->Handlers, MessageLength);
				C->Handlers->Data(C, & BH, MessageLength, C->Arg);
                BH.Data += MessageLength;
                RemainingBytes -= MessageLength;
                Pos += MessageLength;

            } else {

                /* check if there is enough room for the rest of the message */
                if (Pos + MessageLength <= TotalBufferSize) {
                    Platform_Unlock(TRANSPORT_PLATFORM);
                    Status = Platform_Recv(TRANSPORT_PLATFORM, FD,
                                           BH.Data + RemainingBytes,
                                           MessageLength - RemainingBytes,
                                           RECV_TIMEOUT, & B);
                    Platform_Lock(TRANSPORT_PLATFORM);
                    /* if the connection is closed (since another thread is
                       currently closing it, do nothing) */
                    if (C->Disconnected) {
#ifdef TRACE_MAINFUNC
                        fprintf(stderr, "*** after Recv[2] ... disconnected\n"); fflush(stderr);
#endif
                        --C->InUseCount;
                        Platform_Unlock(TRANSPORT_PLATFORM);
                        return ;
                    }

                    if (FAILURE(Status)) {
                        Log_LogPlatformConnError(TRANSPORT_LOG, TCP_MODULE_ID,
                                                 "TCP_ConnectionCallback",
                                                 "Platform_Recv[2]", C, Status);
                        if (C->WriteEventID != NULL) { /* See comment above about registered for write */
                            ChunksManager_TruncateChunk(& BH);
                            --C->InUseCount;
                            Platform_Unlock(TRANSPORT_PLATFORM);
                            return ;
                        }

                        --C->InUseCount;
                        TCP_HandleDisconnection(C);
                        Platform_Unlock(TRANSPORT_PLATFORM);
                        return ;
                    }
                    RemainingBytes += B;
                } else {

                    /* there is not enough space for the message, allocate
                       a new buffer for the message, and copy the split message */

                    /* A quick check that the message length is valid ... */

                    if (MessageLength >= MAX_RECV_MESSAGE_LENGTH) {
                        Status = IPDR_ERR_ALLOCATION_FAILED; /* Don't even try this */
                    } else {
                        Status = ChunksManager_AllocateBuffer(& (C->ReceivedData),
                                                              MessageLength,
                                                              & NewBH);
                    }
                    if (FAILURE(Status)) {
                        /* XXX should signal error event */
                        Log_Write3(CONNECTION_LOG(C), TCP_MODULE_ID, IPDR_LEVEL_ERROR, "TCP_ConnectionCallback",
                                   MSG_TCP_ALLOC_RECVBUFFER_FAILED,
                                   MessageLength,
                                   TCP_GetConnectionInfoStr(C), Platform_SOCKET_DISPLAY(FD));
                        --C->InUseCount;
                        TCP_HandleDisconnection(C);
                        Platform_Unlock(TRANSPORT_PLATFORM);
                        return ;
                    }

                    memcpy(NewBH.Data, BH.Data, RemainingBytes);
                    ChunksManager_TruncateChunk(& BH); /* XXX Perhaps ReleaseBuffer would be safer (and still correct)
                                                        * ... TruncateChunk call depends on AllocateBuffer having got
                                                        * a *new* chunk */

                    BH = NewBH;
                    Pos = 0;
                    TotalBufferSize = MessageLength;
                }
            }
        } else {

            if (Pos + (int) sizeof(MessageHeader) <= TotalBufferSize) {
                Platform_Unlock(TRANSPORT_PLATFORM);
                Status = Platform_Recv(TRANSPORT_PLATFORM, FD,
                                       BH.Data + RemainingBytes,
                                       sizeof(MessageHeader) - RemainingBytes,
                                       RECV_TIMEOUT, & B);
                Platform_Lock(TRANSPORT_PLATFORM);
                /* if the connection is closed (since another thread is currently closing it,
                   do nothing) */
                if (C->Disconnected) {
#ifdef TRACE_MAINFUNC
                    fprintf(stderr, "*** after Recv[3] ... disconnected\n"); fflush(stderr);
#endif
                    --C->InUseCount;
                    Platform_Unlock(TRANSPORT_PLATFORM);
                    return ;
                }
                if (FAILURE(Status)) {
                    Log_LogPlatformConnError(TRANSPORT_LOG, TCP_MODULE_ID,
                                             "TCP_ConnectionCallback",
                                             "Platform_Recv[3]", C, Status);
                    if (C->WriteEventID != NULL) { /* See comment above about registered for write */
                        ChunksManager_TruncateChunk(& BH);
                        --C->InUseCount;
                        Platform_Unlock(TRANSPORT_PLATFORM);
                        return ;
                    }
                    --C->InUseCount;
                    TCP_HandleDisconnection(C);
                    Platform_Unlock(TRANSPORT_PLATFORM);
                    return ;
                }
                RemainingBytes += B;
            } else {

                /* there is not enough space for the header, allocate
                   a new buffer for the message, and copy the splitted message */

                Status = ChunksManager_AllocateMaxBuffer(& (C->ReceivedData),
                                                         & NewBH,
                                                         & TotalBufferSize);

                if (FAILURE(Status)) {
                    /* XXX should signal error event */
                    Log_Write2(CONNECTION_LOG(C), TCP_MODULE_ID, IPDR_LEVEL_ERROR, "TCP_ConnectionCallback[2]",
                               MSG_TCP_ALLOC_MAXBUFFER_FAILED,
                               TCP_GetConnectionInfoStr(C), Platform_SOCKET_DISPLAY(FD));
                    --C->InUseCount;
                    TCP_HandleDisconnection(C);
                    Platform_Unlock(TRANSPORT_PLATFORM);
                    return ;
                }

                memcpy(NewBH.Data, BH.Data, RemainingBytes);
                ChunksManager_TruncateChunk(& BH);

                BH = NewBH;
                Pos = 0;
            }
        }
    }

    --C->InUseCount;
    Platform_Unlock(TRANSPORT_PLATFORM);
}

IPDR_Status TCP_CreateConnectionIdle(TCPTransport *T,
									IPDR_IPAddress LocalAddr,
									IPDR_IPAddress RemoteAddr,
									unsigned short RemotePort,
									TCPConnection **Conn)
{
   	IPDR_SocketHandle NewSocketFD;
    IPDR_Status Status;


	Status = Platform_CreateTCPSocket(T->PS, & (NewSocketFD));
    if (FAILURE(Status))
	{
        Log_LogPlatformError(TRANSPORT_LOG, TCP_MODULE_ID,
                             "TCP_CreateConnectionIdle",
                             "Platform_CreateTCPSocket");
        return Status;
    }

	Status = Platform_Bind(T->PS, NewSocketFD, LocalAddr, 0);
    if (FAILURE(Status)) {
        Log_LogPlatformSocketError(TRANSPORT_LOG, TCP_MODULE_ID,
                                   "TCP_CreateConnectionIdle",
                                   "Platform_Bind", LocalAddr, 0, NewSocketFD);
        Platform_CloseAndInvalidate(T->PS, &T->SocketFD);
        return Status;
    }

	Status = TCP_CreateConnection(T, NewSocketFD, RemoteAddr, RemotePort, Conn);
    if (FAILURE(Status))
	{
        Log_LogPlatformError(TRANSPORT_LOG, TCP_MODULE_ID,
                             "TCP_CreateConnectionIdle",
                             "TCP_CreateConnection");
        return Status;
    }

	return IPDR_OK ;
}

IPDR_Status TCP_Connect(IPDR_PlatformServices *PS,
						TCPConnection *Conn)
{
	IPDR_Status Status;

	Status = Platform_Connect(PS, Conn->FD, Conn->RemoteAddr, Conn->RemotePort);
	if(FAILURE(Status))
	{
	   Log_LogPlatformError(CONNECTION_LOG(Conn), TCP_MODULE_ID,
                             "TCP_Connect",
                             "Platform_Connect");
		return Status;
	}

	Status = Platform_GetSockName(PS, Conn->FD, &(Conn->LocaleAddr), &(Conn->LocalePort));
	if(FAILURE(Status))
	{
	   Log_LogPlatformError(CONNECTION_LOG(Conn), TCP_MODULE_ID,
                             "TCP_Connect",
                             "Platform_GetSockName");
		return Status;
	}

	return IPDR_OK ;
}

IPDR_Status TCP_GetSockName(IPDR_PlatformServices *PS,
							TCPConnection *Conn)
{
	IPDR_Status Status;

	Status = Platform_GetSockName(PS, Conn->FD, &(Conn->LocaleAddr), &(Conn->LocalePort));
	if(FAILURE(Status))
	{
	   Log_LogPlatformError(CONNECTION_LOG(Conn), TCP_MODULE_ID,
                             "TCP_GetSockName",
                             "Platform_GetSockName");
		return Status;
	}

	return IPDR_OK ;
}

/**
 *   Callback triggered whenever a socket becomes a listening
 *   socket becomes readable (a connection arrives).
 *
 * @param FD socket file descriptor
 * @param Arg callback argument (the transport)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static
void TCP_AcceptCallback(IPDR_SocketHandle FD,
                        void *Arg)
{
    TCPTransport *T = /*(TCPTransport *)*/ Arg;
    TCPConnection *C;
    IPDR_SocketHandle NewSocketFD;
    IPDR_Status Status;
    unsigned short RemotePort;
    IPDR_IPAddress RemoteAddr;
	TL_PABuf paBuf2;

    Status = Platform_Accept(TRANSPORT_PLATFORM, FD, & RemoteAddr, & RemotePort,
                             & NewSocketFD);

	Platform_Lock(TRANSPORT_PLATFORM);

    if (FAILURE(Status)) {
        Log_LogPlatformSocketError(TRANSPORT_LOG, TCP_MODULE_ID,
                                   "TCP_AcceptCallback",
                                   "Platform_Accept", RemoteAddr, RemotePort, FD);
        Platform_Unlock(TRANSPORT_PLATFORM);
        return ;
    }

    Status = Platform_SetExporterSocketParams(TRANSPORT_PLATFORM,
                                            NewSocketFD,
                                            T->ParamsHandler.ParamsFunc,
                                            T->ParamsHandler.ParamsArg);
    if (FAILURE(Status)) {
        Log_LogPlatformSocketError(TRANSPORT_LOG, TCP_MODULE_ID,
                                   "TCP_AcceptCallback",
                                   "Platform_SetExporterSocketParams",
                                   RemoteAddr, RemotePort, NewSocketFD);
        Platform_CloseAndInvalidate(TRANSPORT_PLATFORM, &NewSocketFD);
        Platform_Unlock(TRANSPORT_PLATFORM);
        return ;
    }

    Log_Write1(TRANSPORT_LOG, TCP_MODULE_ID, IPDR_LEVEL_INFO, "TCP_AcceptCallback",
               "CONNECT from %.98s",
               TCP_PrintAddressAndPort(RemoteAddr, RemotePort, paBuf2));

    /* create a new connection */

	Status = TCP_CreateConnection(	T, NewSocketFD,
									RemoteAddr, RemotePort, &C) ;

    if (FAILURE(Status)) {
        Log_LogPlatformError(TRANSPORT_LOG, TCP_MODULE_ID,
                             "TCP_AcceptCallback",
                             "TCP_CreateConnection");
        Platform_CloseAndInvalidate(TRANSPORT_PLATFORM, &NewSocketFD);
        Platform_Unlock(TRANSPORT_PLATFORM);
        return ;
    }

	Status = Platform_GetSockName(TRANSPORT_PLATFORM, C->FD, &(C->LocaleAddr), &(C->LocalePort));
	if(FAILURE(Status))
	{
	   Log_LogPlatformError(CONNECTION_LOG(C), TCP_MODULE_ID,
                             "TCP_AcceptCallback",
                             "Platform_GetSockName");
		return ;
	}

    if (C->Handlers != NULL)
        C->Handlers->Connect(C, C->RemoteAddr,
                             C->RemotePort, C->Arg);

    /* In case something goes wrong on the server (collector) (misconfiguration or
     * just bad implementation), we need to watch out for a new
     * connection when there is an already existing connection for
     * that "port". We destroy the old connection and also the new
     * connection; the server (collector) should just try another connection.
     *
     * XXX Although this is safer, it can lead to a "denial of
     * service" attack, by repeatedly connecting to the client
     * (transmitter).  The solution is to instead delay this decision
     * until a START is received and do the disconnect if there is
     * more than one connection.
     */

    {
        IPDR_BOOL duplicateCX = IPDR_FALSE;
        TCPConnection *CX = T->Connections;
        while (CX) {
            TCPConnection *nextCX = CX->NextConnection; /* save "next" because CX might be deallocated */
            if (CX != C && CX->RemoteAddr == RemoteAddr && CX->RemotePort == RemotePort) {
                /* XXX should signal error event */
                Log_Write1(TRANSPORT_LOG, TCP_MODULE_ID, IPDR_LEVEL_ERROR,
                           "TCP_AcceptCallback", MSG_TCP_DUPLICATE_ACCEPT,
                           TCP_GetConnectionInfoStr(CX));
                TCP_HandleDisconnection(CX); /* destroy old connection */
                Platform_SleepMS(TRANSPORT_PLATFORM, 1000); /* avoid rapid connect/disconnect loop */
                duplicateCX = IPDR_TRUE;
            }
            CX = nextCX;
        }
        if (duplicateCX) {
            TCP_HandleDisconnection(C); /* and destroy the new connection as well */
            Platform_SleepMS(TRANSPORT_PLATFORM, 1000); /* avoid rapid connect/disconnect loop */
        }
    }

    Platform_Unlock(TRANSPORT_PLATFORM);
}

IPDR_Status
TCP_CreateConnection(	TCPTransport *T,
						IPDR_SocketHandle FD,
						IPDR_IPAddress RemoteAddr,
						unsigned short RemotePort,
						TCPConnection **Conn)
{
	IPDR_Status Status;
	TCPConnection	*C ;


    Status = TCP_AllocateConnection(T, RemoteAddr, RemotePort,
                                    T->RcvAllocator, &C);
    if (FAILURE(Status)) {
        Log_LogPlatformError(TRANSPORT_LOG, TCP_MODULE_ID,
                             "TCP_CreateConnection",
                             "Platform_AllocateConnection");
        return Status;
    }

	*Conn = C ;
	C->FD = FD;
    C->Handlers = T->DefaultHandlers;
    C->Arg = T->DefaultArg;
	C->Owner = NULL ;
    C->NextConnection = T->Connections;
	C->DummyAddr = 0;
    T->Connections = C;

    Status = Platform_RegisterReadSocket(TRANSPORT_PLATFORM,
										C->FD, TCP_ConnectionCallback,
										C, &(C->ReadEventID));

    Status = Platform_RegisterPeriodicTimer(TRANSPORT_PLATFORM,
                                            TCP_FlushTimerCallback,
                                            C, T->FlushInterval,
                                            &(C->FlushTimer));

    Log_Write3(TRANSPORT_LOG, TCP_MODULE_ID, IPDR_LEVEL_DEBUG, "TCP_AcceptCallback",
               "Register FlushTimerCallback - %.98s, Socket=" Platform_SOCKET_FMT " status = %.99s",
               TCP_GetConnectionInfoStr(C), Platform_SOCKET_DISPLAY(C->FD), IPDR_StatusMsg(NULL, Status));

	return Status;
}

/**
 *   Initializes the TCP transport in passive mode.
 *
 * @param PS platform services
 * @param Logger for any messages
 * @param RcvAllocator allocator which will be used for received
 *                  chunks
 * @param LocalAddr local ip address.
 * @param LocalPort local port number.
 * @param DefaultHandlers callback which will be called when
 *                     a connection arrives.
 * @param DefaultArg argument passed to default handlers.
 * @param ParamsHandler handler for transport specific params
 * @param Handle transport handle (output)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status TCP_InitializePassive(IPDR_PlatformServices *PS,
                                   Log *Logger,
                                   ChunkAllocator RcvAllocator,
                                   IPDR_IPAddress LocalAddr,
                                   unsigned short LocalPort,
                                   ConnectionHandlers *DefaultHandlers,
                                   void *DefaultArg,
                                   ParametersHandler *ParamsHandler,
                                   Transport *Handle)
{
    TCPTransport *T;
    IPDR_Status Status;

    Status = Platform_MEMORYALLOCATE(PS, T);
    if (FAILURE(Status)) {
        /* XXX should signal error event */
        Log_Write1(TRANSPORT_LOG, TCP_MODULE_ID, IPDR_LEVEL_ERROR,
                   "TCP_InitializePassive", MSG_TCP_ALLOC_FAILED, sizeof (TCPTransport));
        X_MEMPANIC(T, __FILE__, __LINE__);
        return Status;
    }

    T->PS = PS;
    T->Logger = Logger;
    T->RcvAllocator = RcvAllocator;

    T->Connections = NULL;

    T->DefaultHandlers = DefaultHandlers;
    T->DefaultArg = DefaultArg;

    T->LocalPort = LocalPort;
    T->LocalAddr = LocalAddr;

    if (ParamsHandler)
        T->ParamsHandler = *ParamsHandler;
    else {
        T->ParamsHandler.ParamsFunc = NULL;
        T->ParamsHandler.ParamsArg = NULL;
    }

    Status = Platform_CreateTCPSocket(PS, & (T->SocketFD));
    if (FAILURE(Status)) {
        Log_LogPlatformError(TRANSPORT_LOG, TCP_MODULE_ID,
                             "TCP_InitializePassive",
                             "Platform_CreateTCPSocket");
        return Status;
    }

    Status = Platform_Bind(PS, T->SocketFD, LocalAddr, LocalPort);
    if (FAILURE(Status)) {
        Log_LogPlatformSocketError(TRANSPORT_LOG, TCP_MODULE_ID,
                                   "TCP_InitializePassive",
                                   "Platform_Bind", T->LocalAddr, T->LocalPort, T->SocketFD);
        Platform_CloseAndInvalidate(PS, &T->SocketFD);
        return Status;
    }

    Status = Platform_Listen(PS, T->SocketFD, 32);
    if (FAILURE(Status)) {
        Log_LogPlatformSocketError(TRANSPORT_LOG, TCP_MODULE_ID,
                                   "TCP_InitializePassive",
                                   "Platform_Listen", T->LocalAddr, T->LocalPort, T->SocketFD);
        Platform_CloseAndInvalidate(PS, &T->SocketFD);
        return Status;
    }

    /* associate a callback with the file descriptor */

#ifdef TRACE_MAINFUNC
    fprintf(stderr, "RegisterReadSocket %d [InitPassive] AcceptCallback\n", T->SocketFD); fflush(stderr);
#endif
    Status = Platform_RegisterReadSocket(PS, T->SocketFD,
                                         TCP_AcceptCallback, T,
                                         & (T->AcceptEventID));
    if (FAILURE(Status)) {
        Log_LogPlatformSocketError(TRANSPORT_LOG, TCP_MODULE_ID,
                                   "TCP_InitializePassive",
                                   "Platform_RegisterReadSocket", T->LocalAddr, T->LocalPort, T->SocketFD);
        Platform_CloseAndInvalidate(PS, &T->SocketFD);
        return Status;
    }

    TCP_InitializeParameters(T);

    *Handle = T;

    return IPDR_OK;
}

/**
 *   Shutdown a transport.
 *
 * @param Handle transport handle
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status TCP_Shutdown(Transport Handle)
{
    TCPTransport *T = /*(TCPTransport *)*/ Handle;
    TCPConnection *C1, *C;

    if (T == NULL) {
        return IPDR_OK;
    }

    C = /*(TCPConnection *)*/ T->Connections;

    /* Note: no check is done for C->InUseCount ... this code should
       only be used when doing a final cleanup. */

    while (C != NULL) {
        C1 = C->NextConnection;
        Log_Write2(TRANSPORT_LOG, TCP_MODULE_ID, IPDR_LEVEL_DEBUG, "TCP_Shutdown",
                   MSG_TCP_CLOSE, TCP_GetConnectionInfoStr(C), Platform_SOCKET_DISPLAY(C->FD));
        Platform_UnregisterSocketAndClear(TRANSPORT_PLATFORM, &C->ReadEventID);
        Platform_UnregisterSocketAndClear(TRANSPORT_PLATFORM, &C->WriteEventID);
        Platform_CloseAndInvalidate(TRANSPORT_PLATFORM, &C->FD);
        ChunksManager_Destroy(& (C->ReceivedData));
        ChunksManager_Destroy(& (C->SentData));
        ChunksManager_Destroy(& (C->SendBuffers));
        strncpy(C->eyecatcher, "Dtcpcon", sizeof C->eyecatcher);
        Platform_MemoryFree(TRANSPORT_PLATFORM, C);
        C = C1;
    }

    Platform_UnregisterSocketAndClear(TRANSPORT_PLATFORM, &T->AcceptEventID);
    if (IPDR_IsValidSocket(T->SocketFD)) {
        Log_Write2(TRANSPORT_LOG, TCP_MODULE_ID, IPDR_LEVEL_DEBUG, "TCP_Shutdown",
                   MSG_TCP_CLOSE, "accept-socket", Platform_SOCKET_DISPLAY(T->SocketFD));
        Platform_CloseAndInvalidate(TRANSPORT_PLATFORM, &T->SocketFD);
    }

    Platform_MemoryFree(TRANSPORT_PLATFORM, T);

    return IPDR_OK;
}

/**
 *   Sends a buffer through a TCP connection (the write
 *   operation is non blocking and any data which can't
 *   be written will be queued).
 *
 * @param C connection to send through
 * @param BH buffer to send
 * @param Immediate whether or not to send immediately or try
 *    to combine into a larger buffer (recommend this should be TRUE
 *    because underlying TCP stack should try to combine (waiting up
 *    to 200ms).
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status TCP_Send(Connection C,
                      BufferHandle *BH,
                      IPDR_BOOL Immediate)
{
    IPDR_Status Status;
    MessageHeader Header;
    BufferHandle SBH;
    TCPSendBuffer *SB;
    int BufferLength;

    TCPConnection *TC = /*(TCPConnection *)*/ C;

	memcpy(&Header, BH->Data,sizeof(MessageHeader));
    BufferLength = IPDR_NTOHL(Header.MessageLength);

    /*** ++TC->InUseCount; ***/

    Log_Write3(CONNECTION_LOG(TC), TCP_MODULE_ID, IPDR_LEVEL_DEBUG, "TCP_Send",
               MSG_TCP_SEND,
               TCP_GetConnectionInfoStr(TC),
               BufferLength, Platform_SOCKET_DISPLAY(TC->FD));

    /* check if the current list of send buffers is empty, if it is, create
       a new SendBuffer and set it to point to the current buffer */
    ChunksManager_GetLastBuffer(& (TC->SendBuffers),
                                sizeof(TCPSendBuffer), & SBH);
    if (BUFFER_IS_NULL(SBH)) {
        Status = ChunksManager_AllocateBuffer(& (TC->SendBuffers),
                                              sizeof(TCPSendBuffer),
                                              & SBH);
        if (FAILURE(Status)) {
            /* XXX should signal error event */
            Log_Write3(CONNECTION_LOG(TC), TCP_MODULE_ID, IPDR_LEVEL_ERROR, "TCP_Send[1]",
                       MSG_TCP_ALLOC_SENDBUFFER_FAILED,
                       sizeof(TCPSendBuffer),
                       TCP_GetConnectionInfoStr(TC), Platform_SOCKET_DISPLAY(TC->FD));
            /** --TC->InUseCount; **/
            TCP_HandleDisconnection(TC);
            return Status;
        }
        SB = (TCPSendBuffer *) SBH.Data;
        SB->StartBuffer = *BH;
        SB->Size = BufferLength;
        Log_OutputStr3(CONNECTION_LOG(TC), TCP_MODULE_ID, IPDR_LEVEL_DEBUG, "TCP_Send",
                       "buffer-null: SB->Size(%u) SB->StartBuffer.Data=0x%p, Immediate=%d",
                       SB->Size, SB->StartBuffer.Data, Immediate);

        if (! Immediate) {
            /** --TC->InUseCount; **/
            return IPDR_OK;
        }
    } else {
        SB = (TCPSendBuffer *) SBH.Data;
        Log_OutputStr3(CONNECTION_LOG(TC), TCP_MODULE_ID, IPDR_LEVEL_DEBUG, "TCP_Send",
                       "buffer-non-null: SB->Size(%u) SB->StartBuffer.Data=0x%p, Immediate=%d",
                       SB->Size, SB->StartBuffer.Data, Immediate);
            if (SB->StartBuffer.Data + SB->Size == BH->Data) {
                SB->Size += BufferLength;
                if (! Immediate) {
                    /** --TC->InUseCount; **/
                    return IPDR_OK;
                }
            } else {
                if (! Immediate && TC->WriteEventID == NULL) {
                    Status = TCP_SendBuffers(TC);
                    if (FAILURE(Status)) {
                        /* assume that SendBuffers has already logged the error */
                        /** --TC->InUseCount; **/
                        return Status;
                    }
                }

                Status = ChunksManager_AllocateBuffer(& (TC->SendBuffers),
                                                      sizeof(TCPSendBuffer),
                                                      & SBH);
                if (FAILURE(Status)) {
                    /* XXX should signal error event */
                    Log_Write3(CONNECTION_LOG(TC), TCP_MODULE_ID, IPDR_LEVEL_ERROR, "TCP_Send[2]",
                               MSG_TCP_ALLOC_SENDBUFFER_FAILED,
                               sizeof(TCPSendBuffer),
                               TCP_GetConnectionInfoStr(TC), Platform_SOCKET_DISPLAY(TC->FD));
                    /** --TC->InUseCount; **/
                    TCP_HandleDisconnection(TC);
                    return Status;
                }
                SB = (TCPSendBuffer *) SBH.Data;
                SB->StartBuffer = *BH;
                SB->Size = BufferLength;

                if (! Immediate) {
                    /** --TC->InUseCount; **/
                    return IPDR_OK;
                }
            }
    }

    /* if there is already a write callback registered (due to a previous
       blocking write, do nothing) and return since the callback will later
       be invoked and the data will be sent */

    if (TC->WriteEventID != NULL) {
        Log_OutputStr2(CONNECTION_LOG(TC), TCP_MODULE_ID, IPDR_LEVEL_DEBUG, "TCP_Send",
                       "Already has write callback (%p) %.125s", TC->WriteEventID, TCP_GetConnectionInfoStr(TC));
        /** --TC->InUseCount; **/
        return IPDR_OK;
    }

    /** --TC->InUseCount; **/
    return TCP_SendBuffers(TC);
}

/**
 *   Receives a buffer from a connection.
 *
 * @param C connection to send through
 * @param Timeout receive timeout (not supported)
 * @param BH received buffer (output)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status TCP_Receive(Connection C,
                         int Timeout,
                         BufferHandle *BH)
{
    TCPConnection *TC = /*(TCPConnection *)*/ C;
    TCPTransport *T = /*(TCPTransport *)*/ TC->Transport;

    MessageHeader Header;
    unsigned int MessageLength;
    IPDR_Status Status;

    if (! IPDR_IsValidSocket(TC->FD))
        return IPDR_ERR_COMMUNICATION_ERROR;

    /** ++TC->InUseCount; **/
	Status = TCP_ReceiveHeader(	T, TC->FD, Timeout, &Header);
	if (FAILURE(Status)) {
		Log_LogPlatformConnError(TRANSPORT_LOG, TCP_MODULE_ID,
                                     "TCP_Receive",
                                     "Platform_Recv", TC, Status);
		return Status;
	}

	Status = TCP_ReceiveMsgBody( C, Timeout,
                         		BH,
								&Header );

	MessageLength = IPDR_NTOHL(Header.MessageLength);

    if (FAILURE(Status)) {
        /* XXX should signal error event */
        Log_Write3(CONNECTION_LOG(TC), TCP_MODULE_ID, IPDR_LEVEL_ERROR, "TCP_Receive",
                   MSG_TCP_ALLOC_RECVBUFFER_FAILED,
                   MessageLength,
                   TCP_GetConnectionInfoStr(TC), Platform_SOCKET_DISPLAY(TC->FD));
        /** --TC->InUseCount; **/
        TCP_HandleDisconnection(TC);
        return Status;
    }
    return IPDR_OK;
}

/**
 * Reads Message header
 * @param T - transport services
 * @param FD - socket file handle
 * @param Timeout receive timeout (not supported)
 * @param Header - message header received (output)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */

IPDR_Status TCP_ReceiveHeader(	TCPTransport *T,
								IPDR_SocketHandle FD,
                         		int Timeout,
                         		MessageHeader* Header)
{
    unsigned int RB, Pos ;
    IPDR_Status Status;
    char *Ptr;

    Ptr = (char *)Header;
    Pos = 0;

    while (Pos < sizeof(MessageHeader)) {
        Status = Platform_Recv(TRANSPORT_PLATFORM, FD, Ptr + Pos,
                               sizeof(MessageHeader) - Pos, Timeout, & RB);
        if (FAILURE(Status)) {
            return Status;
        }
        Pos += RB;
    }

	return IPDR_OK ;
}

/**
 * 	Reads rest of the message after reading message header
 *
 * @param C connection to send through
 * @param Timeout receive timeout (not supported)
 * @param BH received buffer (output)
 * @param Header already received message header
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status TCP_ReceiveMsgBody(	TCPConnection *C,
                         		int Timeout,
                         		BufferHandle *BH,
								MessageHeader* Header )
{
    TCPConnection *TC = /*(TCPConnection *)*/ C;
    TCPTransport *T = /*(TCPTransport *)*/ TC->Transport;

    unsigned int RB, Pos, MessageLength;
    IPDR_Status Status;

    if (! IPDR_IsValidSocket(TC->FD))
        return IPDR_ERR_COMMUNICATION_ERROR;

    /** ++TC->InUseCount; **/
    MessageLength = IPDR_NTOHL(Header->MessageLength);

    Status = ChunksManager_AllocateBuffer(& (TC->ReceivedData),
                                          MessageLength,
                                          BH);
    if (FAILURE(Status)) {
        /* XXX should signal error event */
        Log_Write3(CONNECTION_LOG(TC), TCP_MODULE_ID, IPDR_LEVEL_ERROR, "TCP_ReceiveMsgBody",
                   MSG_TCP_ALLOC_RECVBUFFER_FAILED,
                   MessageLength,
                   TCP_GetConnectionInfoStr(TC), Platform_SOCKET_DISPLAY(TC->FD));
        return Status;
    }

    memcpy(BH->Data, & Header, sizeof(MessageHeader));
    MessageLength -= sizeof(MessageHeader);
    Pos = 0;

    while (Pos < MessageLength) {
        Status = Platform_Recv(TRANSPORT_PLATFORM, TC->FD,
                               BH->Data + Pos + sizeof(MessageHeader),
                               MessageLength - Pos, Timeout, & RB);
        if (FAILURE(Status)) {
            Log_LogPlatformConnError(TRANSPORT_LOG, TCP_MODULE_ID,
                                     "TCP_ReceiveMsgBody",
                                     "Platform_Recv", TC, Status);
            /** --TC->InUseCount; **/
            return IPDR_ERR_COMMUNICATION_ERROR;
        }
        Pos += RB;
    }

    Log_Write3(TRANSPORT_LOG, TCP_MODULE_ID, IPDR_LEVEL_DEBUG, "TCP_Receive",
               MSG_TCP_RECEIVE,
               TCP_GetConnectionInfoStr(TC),
               MessageLength, Platform_SOCKET_DISPLAY(TC->FD));

    /** --TC->InUseCount; **/
    return IPDR_OK;
}



/**
 *   Disconnects a TCP connection
 *
 * @param C connection to disconnect
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status TCP_Disconnect(Connection C)
{
    TCPConnection *C1, *TC = /*(TCPConnection *)*/ C;
    TCPTransport *T = /*(TCPTransport *)*/ TC->Transport;

#ifdef TRACE_MAINFUNC
    fprintf(stderr, "TCP_Disconnect " Platform_SOCKET_FMT "\n", Platform_SOCKET_DISPLAY(TC->FD)); fflush(stderr);
#endif

    /* We're not checking TC->InUseCount here ... unlike
     * TCP_HandleDisconnection() ... this is probably OK because
     * TCP_Disconnect is called only by client(transmitter)/session
     * destructors or when we get connections that we don't know
     * about. */

    TC->Disconnected = IPDR_TRUE;

    Platform_UnregisterTimerAndClear(TRANSPORT_PLATFORM, &TC->FlushTimer);

    Platform_UnregisterSocketAndClear(TRANSPORT_PLATFORM, &TC->ReadEventID);
    Platform_UnregisterSocketAndClear(TRANSPORT_PLATFORM, &TC->WriteEventID);
    Log_Write2(TRANSPORT_LOG, TCP_MODULE_ID, IPDR_LEVEL_DEBUG, "TCP_Disconnect",
               MSG_TCP_CLOSE, TCP_GetConnectionInfoStr(TC), Platform_SOCKET_DISPLAY(TC->FD));
    Platform_CloseAndInvalidate(TRANSPORT_PLATFORM, &TC->FD);

    /* remove the connection from the connections list */

    C1 = T->Connections;
    if (C1 == TC)
        T->Connections = TC->NextConnection;
    else {
        while (C1 != NULL) {
            if (C1->NextConnection == TC) {
                C1->NextConnection = TC->NextConnection;
                break ;
            }
        C1 = C1->NextConnection;
        }
    }

    if (TC->Handlers != NULL)
        TC->Handlers->Disconnect(C, TC->Arg);

    ChunksManager_Destroy(& (TC->ReceivedData));
    ChunksManager_Destroy(& (TC->SentData));
    ChunksManager_Destroy(& (TC->SendBuffers));
    strncpy(TC->eyecatcher, "Dtcpcon", sizeof TC->eyecatcher);
    Platform_MemoryFree(TRANSPORT_PLATFORM, TC);

    return IPDR_OK;
}

/**
 *   Empties any internal buffers used for sending data
 *   on the connection
 *
 * @param C connection to change handlers of.
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status TCP_ResetConnection(Connection C)
{
    TCPConnection *TC = /*(TCPConnection *)*/ C;
    TCPTransport *T = /*(TCPTransport *)*/ TC->Transport;

    ChunksManager_FreeAllChunks(& (TC->SentData));
    ChunksManager_FreeAllChunks(& (TC->SendBuffers));

    Platform_UnregisterSocketAndClear(TRANSPORT_PLATFORM, &TC->WriteEventID);

    /* This clears only outgoing data ... don't unregister TC->ReadEventID */

    return IPDR_OK;
}

/**
 *   Changes the connection handlers associated with a
 *   connection
 *
 * @param C connection to change handlers of.
 * @param Handlers new connection handlers
 * @param Arg new argument
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status TCP_SetConnectionHandlers(Connection C,
                                       ConnectionHandlers *Handlers,
                                       void *Arg)
{
    TCPConnection *TC = /*(TCPConnection *)*/ C;
    TC->Handlers = Handlers;
    TC->Arg = Arg;
    return IPDR_OK;
}

/**
 *   Get address/port of a connection.
 *
 * @param C connection to get info of.
 * @param Addr remote address (output)
 * @param Port remote port (output)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status TCP_GetConnectionInfo(Connection C,
                                   IPDR_IPAddress *Addr,
                                   unsigned short *Port)
{
    TCPConnection *TC = /*(TCPConnection *)*/ C;
    *Addr = TC->RemoteAddr;
    *Port = TC->RemotePort;
    return IPDR_OK;
}

/* A string with the connection information. If
   DETAILED_CONNECTION_INFO is defined, more details are provided (for
   debugging).
*/

/* #define DETAILED_CONNECTION_INFO */

const char* TCP_GetConnectionInfoStr(TCPConnection *TC)
{
    if (TC) {
        /* The first two fields must be of the form
           "Server=xx.xx.xx.xx, Port=xx"
           because this is used by the TriggerEvent and some applications
           rely on the format of the string (bad way of doing things, but
           that's the way things are until we fix them). */
        if (strncmp(TC->eyecatcher, "TCPCONN", sizeof TC->eyecatcher) == 0) {
            TL_PABuf paBuf;
#     if 0
            char buf1[50], buf2[50]; /* for timer descriptions (debugging) */
            TCPTransport *T = /*(TCPTransport *)*/ TC->Transport;
#     endif
#     if defined(DETAILED_CONNECTION_INFO)
            sprintf(TC->descrStr, "Collector=%.50s, [s=" Platform_SOCKET_FMT "]%.20s%.20s,%.20s%.20s%.20s%.60s%.60suse=%d",
                    TCP_PrintConnectionRemote(TC, paBuf),
                    Platform_SOCKET_DISPLAY(TC->FD),
                    ((TC->Disconnected) ? "{disc}" : ""),
                    (TC->ReadEventID ? "read" : ""),
                    (TC->WriteEventID ? "write" : ""),
                    (TC->Handlers ? "" : "(noHndlr)"),
                    (TC->Transport ? "" : "(noTrnsprt)"),
                    "", /* Platform_DescribeTimer(T->PS, "conn", TC->ConnectionTimer, buf1), */
                    "", /* Platform_DescribeTimer(T->PS, "flush", TC->FlushTimer, buf2), */
                    TC->InUseCount);
#     else
            sprintf(TC->descrStr, "Collector=%.50s",
                    TCP_PrintConnectionRemote(TC, paBuf));
#     endif
            return TC->descrStr;
        } else {
            static char iMsg[40];
            sprintf(iMsg, "<invalid connection(%.8s)>", TC->eyecatcher);
            return iMsg;
        }
    } else {
        return "<null connection>";
    }
}

/**
 *   Get the chunks manager for received data on a connection
 *
 * @param TC connection to get info of.
 * @param Mgr chunks manager
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status TCP_GetReceiveChunksManager(TCPConnection *TC,
                                         ChunksManager **Mgr)
{
    *Mgr = & (TC->ReceivedData);
    return IPDR_OK;
}

/**
 *   Get the chunks manager for sending control messages
 *   on a connection
 *
 * @param TC connection to get info of.
 * @param Mgr chunks manager
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status TCP_GetSendChunksManager(TCPConnection *TC,
                                      ChunksManager **Mgr)
{
    *Mgr = & (TC->SentData);
    return IPDR_OK;
}

/**
 *   reconfigure the transport parameters
 *
 * @param TH transport
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status TCP_Reconfigure(Transport TH)
{
    TCPTransport *T = /*(TCPTransport *)*/ TH;
    TCPConnection *C;

    if (! T) {
        return IPDR_OK;
    }

    for (C = T->Connections; C != NULL; C = C->NextConnection) {
        ChunksManager_Reconfigure(& (C->ReceivedData),
                                  STR_configuration_transport_tcpTransport_dataRcvChunkSize,
                                  STR_configuration_transport_tcpTransport_dataRcvInitialNumChunks,
                                  STR_configuration_transport_tcpTransport_dataRcvMaxMemory,
                                  & (T->ParamsHandler));
        ChunksManager_Reconfigure(& (C->SentData),
                                  STR_configuration_transport_tcpTransport_dataSndChunkSize,
                                  STR_configuration_transport_tcpTransport_dataSndInitialNumChunks,
                                  STR_configuration_transport_tcpTransport_dataSndMaxMemory,
                                  & (T->ParamsHandler));
        ChunksManager_Reconfigure(& (C->SendBuffers),
                                  STR_configuration_transport_tcpTransport_sendBuffersChunkSize,
                                  STR_configuration_transport_tcpTransport_sendBuffersInitialNumChunks,
                                  STR_configuration_transport_tcpTransport_sendBuffersMaxMemory,
                                  & (T->ParamsHandler));
    }

    TCP_InitializeParameters(T);

    return IPDR_OK;
}
