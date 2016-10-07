/** @file Exporter/ExporterProtocol.c
 *
 * Implementation of functions handling the client (transmitter) side of the IPDR
 * protocol.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: ExporterProtocol.c,v 1.115 2005/05/23 11:51:22 dmitryk Exp $
 *
 * The initial connection handling is done by Exporter/Exporter.c, which
 * switches to here for handling the protocol. This module uses
 * functions in Exporter/Exporter.c on disconnection.
 */

#include <IPDR/Config.h>
#include <Common/Protocol.h>
#include <Exporter/ExporterProtocol.h>

const char
ExporterProtocol_rcsid[] = "@(#) $Id: ExporterProtocol.c,v 1.115 2005/05/23 11:51:22 dmitryk Exp $" ;
const char ExporterProtocol_h_rcsid[] = EXPORTER_PROTOCOL_H_RCSID;
const char Protocol_h_rcsid[] = PROTOCOL_H_RCSID;

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <Common/Transport.h>
#include <Common/CommonUtil.h>
#include <Exporter/Templates.h>
#include <Exporter/ExporterMsgs.h>
#include <Exporter/ProtocolDebug.h>

/**
 * Handlers for transport layer connections, afte#ifdef NOTDONEYET
		Session_SendOutstandingData(E, S);
#endifr initial connection.
 */
ConnectionHandlers
ExporterProtocol_ConnectionHandlers = {
    ExporterProtocol_ConnectCallback,
    ExporterProtocol_DisconnectCallback,
    ExporterProtocol_DataCallback,
};

/* Internal utility functions*/

static IPDR_BOOL ExporterProtocol_NeedSessionID(unsigned char MessageID);
static IPDR_Status ExporterProtocol_CollectorSession(Collector *Coll, Session *S, CollectorSession** CS, IPDR_BOOL *High, IPDR_BOOL allStates);
static IPDR_Status ExporterProtocol_SyncTemplateDataAll(Exporter *E, Session *S);
static void ExporterProtocol_ConnectWaitWaitBestCollectorCallback(void *Arg);

size_t
ExporterProtocol_GetFieldDescriptorDataLength( Key* K );

size_t
ExporterProtocol_GetTemplateBlockDataLength( Template* T );

size_t
ExporterProtocol_GetTemplateDataMsgLen(TransmissionContext *TC);

size_t
ExporterProtocol_GetTemplatesResponseMsgLen(TransmissionContext *TC);

size_t
ExporterProtocol_GetSessionBlockDataLength( Session *S );

size_t
ExporterProtocol_GetSessionsResponseMsgLen(Collector *Coll, unsigned int *NumSessions);

IPDR_Status
ExporterProtocol_WriteShort( unsigned short Short, unsigned char** Buffer );

IPDR_Status
ExporterProtocol_WriteInt( unsigned int Int, unsigned char** Buffer );

IPDR_Status
ExporterProtocol_WriteTime( IPDR_Time Time, unsigned char** Buffer );

IPDR_Status
ExporterProtocol_WriteLong( long long Long, unsigned char** Buffer );

IPDR_Status
ExporterProtocol_WriteUF8String( const char* String, unsigned char** Buffer );

IPDR_Status
ExporterProtocol_WriteFieldDescriptorBlock( Key* K, unsigned char** Buffer);

IPDR_Status
ExporterProtocol_WriteTemplateBlock( Template *T, unsigned char** Buffer);

IPDR_Status
ExporterProtocol_WriteTemplateData( TransmissionContext *TC, unsigned char *Buffer, IPDR_BOOL Negotiable);

IPDR_Status
ExporterProtocol_ReadTemplateShort(Session* S, short* Short, unsigned char** Buffer, int* LengthLeft);

IPDR_Status
ExporterProtocol_ReadTemplateInt(Session* S, int* Ints, unsigned char** Buffer, int* LengthLeft);


IPDR_Status
ExporterProtocol_ReadTemplateString(Session* S, char** String, unsigned char** Buffer, int* LengthLeft);

IPDR_Status
ExporterProtocol_ReadFieldDescriptorBlock(Session* S, Template* T, unsigned char** Buffer, int *LengthLeft);

IPDR_Status
ExporterProtocol_ReadTemplateBlock(Session* S, Template** T, unsigned char** Buffer, int* LengthLeft);

IPDR_Status
ExporterProtocol_ReadTemplateByte(Session* S, unsigned char* Byte, unsigned
char** Buffer, int* LengthLeft);

IPDR_Status
ExporterProtocol_GetTemplates( TransmissionContext *TC, unsigned char *Buffer, int RequestID);

static void
ExporterProtocol_KeepAliveInTimerCallback(void *Arg);

static void
ExporterProtocol_KeepAliveOutTimerCallback(void *Arg);

static void
ExporterProtocol_ConnectResponseTimeoutCallback(void *Arg);

static void
ExporterProtocol_TmplDataTimerCallback(void *Arg);

static void
ExporterProtocol_SetCollectorCapabilities(Exporter *E, Collector *Coll, unsigned int CollCaps);

static int
ExporterProtocol_GetCollectorCapabilities(Collector *Coll);

IPDR_Status
ExporterProtocol_AutoconnectCallback(Exporter *E, Collector *Coll, Connection C)
{
	IPDR_Status Status;
	Status = ExporterProtocol_SendConnect(E, NULL, Coll, C);
	if (FAILURE(Status))
	{
		Log_Write3(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_ERROR, "ExporterProtocol_AutoconnectCallback",
					MSG_EXPORTER_LOG_CONNECT_ERROR,
					"ExporterProtocol_SendConnect", IPDR_StatusMsg(E,Status), TL_GetConnectionInfoStr(C));
	}
	return Status;
}

/**
 * Callback invoked when a server (collector) connects.
 *
 * @param Conn  connected connection
 * @param Addr  remove address
 * @param Port  remove port
 * @param Arg  additional argument (Server)
 */
void
ExporterProtocol_ConnectCallback(Connection Conn,
                               IPDR_IPAddress Addr,
                               unsigned short Port, void *Arg)
{
}


/**
 * Callback invoked when a server (collector) disconnects.
 *
 * @param Conn disconnected connection
 * @param Arg additional argument
 */
void ExporterProtocol_DisconnectCallback(Connection Conn, void *Arg)
{
    Exporter*			E = (Exporter*)Arg;
    IPDRConnection*		IC = (IPDRConnection *)Conn->Owner;
	Collector*			Coll = (Collector *)IC->Owner;
	CollectorSession*	CS;
	IPDR_BOOL			WasActive;
	int					Index ;

	Conn->Owner = NULL;

	Log_StateTransition(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_DEBUG,
                        "ExporterProtocol_DisconnectCallback", Conn,
                        IC->State, IPDR_SERVER_DISCONNECTED,
                        TL_GetConnectionInfoStr(Conn));

	/* for each Session of this connection, need to have if we had active session there */
	for( Index = 0; Index < Coll->CollectorSessions.Length; Index++ )
	{
		CS = (CollectorSession*)Coll->CollectorSessions.Data[Index];

		/* If it is active CollectorSession need to select new server now*/
		WasActive = (CS->CSState == IPDR_CS_ACTIVE && CS->SessionPtr->ActiveCS == CS) ;
		CS->CSState = IPDR_CS_DISCONNECTED;
		CS->SessionConnection = NULL ;

		if(WasActive)
			ExporterProtocol_SelectCollector( E, CS->SessionPtr, IPDR_FALSE, NULL);
	}


	ExporterProtocol_ResetIPDRConnection( E, IC );
}


/**
 * Callback invoked when data arrives from a server (collector)
 *
 * @param C connection
 * @param BH buffer
 * @param MsgLen message length
 * @param Arg callback argument (Server)
 */
void
ExporterProtocol_DataCallback(Connection C, BufferHandle *BH,
                            unsigned int MsgLen, void *Arg)
{
	Exporter 		*E = (Exporter *) Arg;
	Session 		*S = NULL;
	IPDRConnection 	*IC = C->Owner;
	Collector 		*Coll ;
	IPDR_BOOL 		needsSessionID ;

	/*char *Buf;*/
	char	Buf[EXPORTER_PROTOCOL_MAX_MSG];
    MessageHeader *H;
	TL_PABuf paBuf;

    if (BH == NULL || MsgLen == 0)
        return ;

	memcpy(Buf, BH->Data, MsgLen > EXPORTER_PROTOCOL_MAX_MSG ? EXPORTER_PROTOCOL_MAX_MSG : MsgLen);

    H = (MessageHeader *) Buf;

	if(IC == NULL)
	{
		Log_Write3(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
					"ExporterProtocol_DataCallback", MSG_EXPORTER_UNEXPECTED_MSG,
					TCP_PrintConnectionFull(C, paBuf), H->MessageID, "NO CONNECTION");
		TL_HandleDisconnection(C);
		ChunksManager_ReleaseBuffer(BH, MsgLen);
		return ;
	}
	Coll = IC->Owner;

	if(Coll == NULL)
	{
    	Log_Write3(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
					"ExporterProtocol_DataCallback", MSG_EXPORTER_UNEXPECTED_MSG,
					TCP_PrintConnectionFull(C, paBuf), H->MessageID, "NO COLLECTOR");
		TL_HandleDisconnection(C);
		ChunksManager_ReleaseBuffer(BH, MsgLen);
		return ;
	 }

    /*Buf = BH->Data;*/
    if( EXPORTER_PROTOCOL_MAX_MSG <= MsgLen )
    {
		Log_Write1(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
                   "ExporterProtocol_DataCallback", MSG_EXPORTER_MESSAGE_TOO_LONG,
				   TCP_PrintConnectionFull(C, paBuf));
		TL_HandleDisconnection(C);
		ChunksManager_ReleaseBuffer(BH, MsgLen);
		return ;
    }

    switch (H->MessageID) {
    case IPDR_MSG_DATA:
    case IPDR_MSG_DATA_ACK:
		/*case IPDR_MSG_KEEP_ALIVE:*/
        /* case IPDR_MSG_DATA_NACK:*/
        if (Log_ShouldWrite(&E->ExporterLog, PROTOCOL_MODULE_ID, IPDR_LEVEL_DEBUG)) {
            ProtocolDebug_PrintIncomingMessage(&E->ExporterLog, C, Buf, IPDR_LEVEL_DEBUG);
        }
        break;
    default:
        if (Log_ShouldWrite(&E->ExporterLog, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO)) {
            ProtocolDebug_PrintIncomingMessage(&E->ExporterLog, C, Buf, IPDR_LEVEL_INFO);
        }
    }

    /* find the session associated with the message */

	needsSessionID = ExporterProtocol_NeedSessionID(H->MessageID);

    if (needsSessionID && H->SessionID > 0 ) {
    	int i;
        for (i = 0; i < E->Sessions.Length; i++) {
            Session *S1 = (Session *) E->Sessions.Data[i];
            if (S1 != NULL && S1->ID == H->SessionID) {
                S = S1;
                break ;
            }
        }
    }

    if (S == NULL && needsSessionID) {
        /* We got an unexpected message - we don't don't know about
         * this session, so complain and disconnect */
		Session Sess;
        IPDR_EventData eventData;
        EventDataInit(&eventData, IPDR_EVENT_ERROR, E, NULL, NULL);
        eventData.Extra.Error.Reason = IPDR_EVENT_ERROR_UNKNOWN_SESSION;
        eventData.SessionID = H->SessionID;
        Exporter_TriggerEvent(E, IPDR_LEVEL_ERROR,
                            &eventData, NULL,
                            MSG_EXPORTER_UNKNOWN_SESSION,
                            TL_GetConnectionInfoStr(C), H->SessionID, H->MessageID);

		Log_Write3(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
			 "				ExporterProtocol_DataCallback", MSG_EXPORTER_UNKNOWN_SESSION,
                            TL_GetConnectionInfoStr(C), H->SessionID, H->MessageID);

		ExporterProtocol_KeepAliveInTimerReset(IC);

		Sess.ID = H->SessionID;
		if(FAILURE(ExporterProtocol_SendError(E, &Sess, Coll, C, CollectorNotInSession, MSG_EXPORTER_COLLECTOR_NOT_IN_SESSION)))
		{
			TL_HandleDisconnection(C);
		}

		ChunksManager_ReleaseBuffer(BH, MsgLen);
		return;
    }

    switch (H->MessageID) {

    case IPDR_MSG_FLOW_START:
        ExporterProtocol_HandleFlowStart(E, S, Coll, H, C);
        break;

    case IPDR_MSG_FLOW_STOP:
        ExporterProtocol_HandleFlowStop(E, S, Coll, H, C);
        break;

	case IPDR_MSG_MODIFY_TEMPLATE:
		ExporterProtocol_HandleModifyTemplate(E, S, Coll, H, C);
		break;

    case IPDR_MSG_FINAL_TEMPLATE_DATA_ACK:
        ExporterProtocol_HandleFinalTmplDataACK(E, S, Coll, H, C);
        break;

	case IPDR_MSG_START_NEGOTIATION:
		ExporterProtocol_HandleStartNegotiation(E, S, Coll, H, C);
		break;

	case IPDR_MSG_GET_SESSIONS:
		ExporterProtocol_HandleGetSessions(E, S, Coll, H, C);
		break;

	case IPDR_MSG_GET_TEMPLATES:
		ExporterProtocol_HandleGetTemplates(E, S, Coll, H, C);
		break;

    case IPDR_MSG_DATA_ACK:
        ExporterProtocol_HandleDataACK(E, S, Coll, H, C);
        break;

	case IPDR_MSG_CONNECT:
        ExporterProtocol_HandleConnect(E, S, Coll, H, C);
        break;

	case IPDR_MSG_CONNECT_RESPONSE:
        ExporterProtocol_HandleConnectResponse(E, S, Coll, H, C);
        break;

	case IPDR_MSG_DISCONNECT:
		ExporterProtocol_HandleDisconnect(E, S, Coll, H, C);
		break;

	case IPDR_MSG_ERROR:
		ExporterProtocol_HandleError(E, S, Coll, H, C);
		break;

	case IPDR_MSG_KEEP_ALIVE:
		ExporterProtocol_HandleKeepAlive(E, S, Coll, H, C);
		break;

    default:
        ExporterProtocol_HandleUnknownMsg(E, S, Coll, H, C);
        break;
    }
	/* Can not call ReleaseMessage here because of no padding, may be releaseBuffer will do */
    /*ChunksManager_ReleaseMessage(BH);*/
    ChunksManager_ReleaseBuffer(BH, MsgLen);
}


/**
 * Move to the next transmission context if needed (i.e.  the queue of
 * the current context is empty and a next context is available),
 * otherwise send any outstanding data.
 *
 * @param E exporter handle
 * @param S session handle (pointer)
 * @param Coll collector handle
 * @param DebugMsg context message for any debug message produced by this function
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
ExporterProtocol_FlushRecordsQueue(Exporter *E, Session *S,
                                 Collector *Coll, const char *DebugMsg)
{
	return Session_SendOutstandingData(E, S);
}


/**
 * Set the header fields within a message.
 *
 * @param S session handle (pointer)
 * @param Msg message to update
 * @param MessageID message id to put in the header
 * @param MessageLength length of the message
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status ExporterProtocol_FillHeader(Session *S, char *Msg,
                                       unsigned char MessageID,
                                       unsigned long MessageLength)
{
    MessageHeader *H = (MessageHeader *) Msg;

    H->Version = IPDR_PROTOCOL_VERSION;
    H->SessionID = S ? S->ID : 0;
    H->MessageFlags = 0;
    H->MessageID = MessageID;
    IPDR_SET_HTONL(H->MessageLength, MessageLength);

    return IPDR_OK;
}


/**
 * Handles a flow start event recived from a collector
 *
 * @param E exporter handle
 * @param S session handle (pointer)
 * @param Coll collector handle (pointer)
 * @param Msg message header from the collector
 * @param Conn The Connection between S to Coll
 */
IPDR_Status ExporterProtocol_HandleFlowStart(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	const MessageHeader *Msg,
                                        	Connection Conn)
{
	IPDR_BOOL			takeThisOne = IPDR_FALSE;
	IPDR_BOOL			forceThisOne = IPDR_FALSE;
	CollectorSession		*CS	= NULL;
	IPDRConnection *IC = Conn->Owner;
	IPDR_Status			Status;

	ExporterProtocol_KeepAliveInTimerReset(IC);

	Status = ExporterProtocol_CollectorSession(Coll, S, &CS, &takeThisOne, IPDR_TRUE);

	if( CS == NULL )
	{
		Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
			 "ExporterProtocol_HandleFlowStart", MSG_EXPORTER_COLLECTOR_SESSION_FAILED);
		Status = ExporterProtocol_SendError(E, S, Coll, Conn, CollectorNotConfigured, MSG_EXPORTER_COLLECTOR_SESSION_FAILED);
		if(FAILURE(Status))
		{
			TL_HandleDisconnection(Conn);
		}
		return IPDR_ERR;
	}


	if(IC->State != IPDR_CONNECTION_CONNECTED)
	{
		Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
			 "ExporterProtocol_HandleFlowStart", MSG_EXPORTER_COLLECTOR_NOT_CONNECTED);
		Status = ExporterProtocol_SendError(E, S, Coll, Conn, InvalidConnectionState, MSG_EXPORTER_COLLECTOR_NOT_CONNECTED);
		if(FAILURE(Status))
		{
			TL_HandleDisconnection(Conn);
		}
		return IPDR_ERR;
	}

	/*if(S->Running == IPDR_FALSE)
	{
		Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
			 "ExporterProtocol_HandleFlowStart", MSG_EXPORTER_COLLECTOR_NOT_CONNECTED);
		Status = ExporterProtocol_SendError(E, S, Coll, Conn, MessageInvalidForState, MSG_EXPORTER_WRONG_COLLECTOR_STATE);
		if(FAILURE(Status))
		{
			TL_HandleDisconnection(Conn);
		}
		return IPDR_ERR;
	}*/

	CS->CSState = IPDR_CS_INITIATION;
	CS->SessionConnection = IC ;

	if(E->FlowStartCallback != NULL)
		E->FlowStartCallback(S, Coll, &forceThisOne, E->FlowStartCallbackArg);

	if( !takeThisOne && !forceThisOne)
	{
		if(S->ActiveCS == NULL)
		{
			/* Now, setup wait for best collector and sent TEMPLATE DATA later*/
			if(S->waitBestCollectorTimerID == NULL)
				Platform_RegisterTimer(EXPORTER_PLATFORM,
								ExporterProtocol_ConnectWaitWaitBestCollectorCallback,
								CS,
								E->Parameters.WaitBestCollectorTimeout,
								& (S->waitBestCollectorTimerID));
			return IPDR_OK;
		}
		else
		{
			Status = ExporterProtocol_SendTemplateData(E, S, CS->Collector,
								   CS->SessionConnection->TCPConnection, IPDR_FALSE);
			if(FAILURE(Status))
			{
				TL_HandleDisconnection(CS->SessionConnection->TCPConnection);
				Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO,
				          "ExporterProtocol_HandleFlowStart", MSG_EXPORTER_SEND_TEMPLATE_DATA_FAILED);
			}
			return Status;
		}
	}

	if(S->waitBestCollectorTimerID != NULL)
		Platform_UnregisterTimerAndClear(EXPORTER_PLATFORM, &S->waitBestCollectorTimerID);

	return ExporterProtocol_SelectCollector(E, S, IPDR_FALSE, forceThisOne==IPDR_TRUE?CS:NULL);
}

/**
 * Handles a flow stop event recived from a collector
 *
 * @param E exporter handle
 * @param S session handle (pointer)
 * @param Coll collector handle (pointer)
 * @param Msg message header from the collector
 * @param Conn The Connection between S to Coll
 */
IPDR_Status ExporterProtocol_HandleFlowStop(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	const MessageHeader *Msg,
                                        	Connection Conn)
{
	int Index;
	CollectorSession *SCS;
	IPDRConnection *IC = Conn->Owner;
	IPDR_Status Status;

	ExporterProtocol_KeepAliveInTimerReset(Conn->Owner);

	for (Index = 0; Index < S->CollectorSessions.Length; Index++)
	{
		SCS = (CollectorSession *) S->CollectorSessions.Data[Index];
		if(IC == SCS->SessionConnection)
		{
			if(SCS->CSState != IPDR_CS_READY && SCS->CSState != IPDR_CS_ACTIVE)
			{

				Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
					  "ExporterProtocol_HandleFlowStop", MSG_EXPORTER_WRONG_COLLECTOR_STATE);
				Status = ExporterProtocol_SendError(E, S, Coll, Conn, MessageInvalidForState, MSG_EXPORTER_WRONG_COLLECTOR_STATE);
				if(FAILURE(Status))
				{
					TL_HandleDisconnection(Conn);
				}
			}
			SCS->CSState = IPDR_CS_DISCONNECTED ;
		}
	}
    return ExporterProtocol_SelectCollector( E, S, IPDR_FALSE, NULL);
}


/**
 * Handles a modify template event recived from a collector
 *
 * @param E exporter handle
 * @param S session handle (pointer)
 * @param Coll collector handle (pointer)
 * @param Msg message header from the collector
 * @param Conn The Connection between S to Coll
 */
IPDR_Status ExporterProtocol_HandleModifyTemplate(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	const MessageHeader *Msg,
                                        	Connection Conn)
{
	IPDRConnection*					IC;
	unsigned int					MsgLen;
	unsigned char*					ReadBuffer;
	int								Index, TmplIndex, IndexTC, IndexNew, iTCFld, iNewFld;
	int								LengthLeft;
	ModifyTemplateResponseMessage	H ;
	TransmissionContext*				TC;
	DynamicArray 					NewTmpls;
	Template						*T, *TemplTC ;
	Key 							*TCKey, *NewKey;
    unsigned char 					ConfigurationID ;
    unsigned short 					NumOfTemplates ;
	CollectorSession				*CS ;
	IPDR_Status						Status;
	IPDR_BOOL						TemplatesChanged = IPDR_FALSE;

	if(!E || !S || !Coll || !Conn || !(Conn->Owner) || !Msg)
		return IPDR_ERR ;

	IC = Conn->Owner;
	MsgLen = IPDR_NTOHL(Msg->MessageLength);
	TC = S->ActiveContext;

	ExporterProtocol_KeepAliveInTimerReset(IC);

	/* check for out of the blue ACK */
	if( MsgLen < sizeof(ModifyTemplateResponseMessage) )
	{
		Log_Write2(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO,
                   "ExporterProtocol_HandleModifyTemplate",
		   MSG_EXPORTER_TEMPLATE_RESPONSE_INVALID_VALUE,
		   MsgLen, sizeof(ModifyTemplateResponseMessage));
		return IPDR_OK;
	}
	memcpy( &H, Msg, sizeof(ModifyTemplateResponseMessage) );
	ConfigurationID = IPDR_NTOHS(H.ConfigurationID);
	NumOfTemplates = IPDR_NTOHL(H.NumOfTemplates);

	if (ConfigurationID != TC->ConfigurationID)
	{
        /* XXX should signal error event */
        Log_Write2(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
		           "ExporterProtocol_HandleModifyTemplate",
			   MSG_EXPORTER_WRONG_CONFIGURATION_ID, TC->ConfigurationID,ConfigurationID);
        return IPDR_OK;
    }

	// The template is not negotiable, Modify Templateis not allowed
	if (TC->Negotiable != IPDR_TRUE)
	{
		Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_WARNING,
			 "ExporterProtocol_HandleModifyTemplate", MSG_EXPORTER_INVALID_CAPABILITIES);
		ExporterProtocol_SendError(E, S, Coll, Conn, MessageInvalidForCapabilities, MSG_EXPORTER_INVALID_CAPABILITIES);
		return IPDR_OK ;
	}

	Status = ExporterProtocol_CollectorSession(Coll, S, &CS, NULL, IPDR_TRUE);
	if( FAILURE(Status))
	{
		Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
			 "ExporterProtocol_HandleModifyTemplate", MSG_EXPORTER_COLLECTOR_SESSION_FAILED);
		ExporterProtocol_SendError(E, S, Coll, Conn, CollectorNotConfigured, MSG_EXPORTER_COLLECTOR_SESSION_FAILED);
		return Status ;
	}

	/* Read the received templates into array */
	Status = DynamicArray_Create(EXPORTER_PLATFORM, &NewTmpls, 0);
	if( FAILURE(Status) )
	{
		Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
			 "ExporterProtocol_HandleModifyTemplate", MSG_EXPORTER_CREATE_DYNAMIC_ARRAY_FAILED);
		return Status;
	}

	ReadBuffer = (unsigned char*)((size_t)Msg + sizeof(ModifyTemplateResponseMessage));
	for( Index = 0; Index < NumOfTemplates; Index++ )
	{
		/* Calculate, how much of message left to read. Do not allow to overflow this value */
		LengthLeft = MsgLen - ((size_t)ReadBuffer - ((size_t)Msg + sizeof(ModifyTemplateResponseMessage)));
		Status = ExporterProtocol_ReadTemplateBlock(S, &T, &ReadBuffer, &LengthLeft );
		if( FAILURE(Status) )
		{
			Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO,
				  "ExporterProtocol_HandleModifyTemplate", MSG_EXPORTER_READ_TEMPLATE_BLOCK_FAILED);
			break ;
		}
		Status = DynamicArray_AddElement(EXPORTER_PLATFORM, & (NewTmpls), T, &TmplIndex);
	}

	/*Status = IPDR_OK;*/
	for(IndexTC = 0 ; IndexTC < TC->Templates.Length; IndexTC++)
	{
		TemplTC = (Template *) TC->Templates.Data[IndexTC];
		for(IndexNew = 0 ; IndexNew < NewTmpls.Length; IndexNew++)
		{
			T = (Template *) NewTmpls.Data[IndexNew];
			if(TemplTC->TemplateID == T->TemplateID)
			{
				int keysFound = 0;
				for(iTCFld = 0 ; iTCFld < TemplTC->Keys.Length ; iTCFld++)
				{
					TCKey = (Key *) TemplTC->Keys.Data[iTCFld];
					for(iNewFld = 0 ; iNewFld < T->Keys.Length ; iNewFld++)
					{
						NewKey = (Key *) T->Keys.Data[iNewFld];
						if(TCKey->KeyID == NewKey->KeyID)
						{
							keysFound++;
							if((TCKey->KeyTypeID &~IPDR_TYPE_VAR_LENGTH) != NewKey->KeyTypeID)
							{
								Status = IPDR_ERR;
								break ;
							}
							if(strcmp(TCKey->Name, NewKey->Name) != 0)
							{
								Status = IPDR_ERR;
								break ;
							}

						}
					}
					if(Status == IPDR_ERR)
						break;
				}
				if(keysFound != TemplTC->Keys.Length || Status == IPDR_ERR)
				{
					Status = IPDR_ERR;
					break ;
				}
				else
				{
					for(iTCFld = 0 ; iTCFld < TemplTC->Keys.Length ; iTCFld++)
					{
						TCKey = (Key *) TemplTC->Keys.Data[iTCFld];
						for(iNewFld = 0 ; iNewFld < T->Keys.Length ; iNewFld++)
						{
							NewKey = (Key *) T->Keys.Data[iNewFld];
							if(TCKey->KeyID == NewKey->KeyID)
							{
								if(TCKey->Enabled != NewKey->Enabled)
								{
									TemplatesChanged = IPDR_TRUE;
									TCKey->Enabled = NewKey->Enabled;
									Log_Write4(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_DEBUG,
										  "ExporterProtocol_HandleModifyTemplate",
										  MSG_EXPORTER_TEMPLATE_CHANGE_RECIVED,
										  TemplTC->TemplateID,
										  TCKey->KeyID,
										  TCKey->Name,
										  TCKey->Enabled == IPDR_TRUE ? "Enable" : "Disable");
								}
							}
						}
					}
				}
			}
			if(Status == IPDR_ERR)
				break;
		}
		if(Status == IPDR_ERR)
			break;
	}

	if(SUCCESS(Status))
	{
		if(CS->TmplDataTimer != NULL)
    		Platform_UnregisterTimerAndClear(EXPORTER_PLATFORM, &CS->TmplDataTimer);

		if(TemplatesChanged == IPDR_TRUE)
		{
			S->TemplatesChnged = IPDR_TRUE;
			Exporter_TriggeTemplateChangedCallback(S);

			EXPORTER_QHANDLER->CleanUP(E, S);
			Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO,
						  "ExporterProtocol_HandleModifyTemplate",
							"Template Changed, Must CleanUP Queue.");

			for(IndexTC = 0 ; IndexTC < TC->Templates.Length; IndexTC++)
			{
				TemplTC = (Template *) TC->Templates.Data[IndexTC];
				for(iTCFld = 0 ; iTCFld < TemplTC->Keys.Length ; iTCFld++)
				{
					TCKey = (Key *) TemplTC->Keys.Data[iTCFld];
					Log_Write4(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_DEBUG,
						  "ExporterProtocol_HandleModifyTemplate",
						  MSG_EXPORTER_TEMPLATE_CHANGE_SENT,
						  TemplTC->TemplateID,
						  TCKey->KeyID,
						  TCKey->Name,
						  TCKey->Enabled == IPDR_TRUE ? "Enable" : "Disable");
				}
			}
		}

		Status = ExporterProtocol_SendModifyTemplateResponse(E, S, Coll, Conn);
		if(FAILURE(Status))
		{
			TL_HandleDisconnection(Conn);
		}
	}
	else
	{
		Log_Write(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_WARNING,
			  "ExporterProtocol_HandleModifyTemplate", MSG_EXPORTER_BROKEN_TEMPLATE);
		Status = ExporterProtocol_SendError(E, S, Coll, Conn, BrokenTemplate, MSG_EXPORTER_BROKEN_TEMPLATE);
		if(FAILURE(Status))
		{
			TL_HandleDisconnection(Conn);
		}
	}


	DynamicArray_Delete(EXPORTER_PLATFORM, &(NewTmpls));

    /* check if there is a need to change the currently active server (collector) */
    if (S->ActiveCS == NULL || CS->Priority < S->ActiveCS->Priority) {
		Session_SelectActiveCollector(E, S);
    }

    /* handle the changes placed within the message */
    /*Session_HandleTemplateChanges(E, S, NumOfTemplates,
                                  (unsigned char*)((size_t)Msg +
									sizeof(ModifyTemplateResponseMessage)),
                                  IPDR_FALSE, IPDR_FALSE, IPDR_FALSE);*/

    /* send any outstanding data */
    ExporterProtocol_FlushRecordsQueue(E, S, Coll, "TmplDataACK");

    return IPDR_OK;
}

/**
 * Handle a FINAL TMPL DATA ACK arriving from a collector.
 *
 * @param E exporter handle
 * @param S session handle (pointer)
 * @param Coll collector originating the message
 * @param Msg received message
 * @param Conn The Connection between S to Coll
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status ExporterProtocol_HandleFinalTmplDataACK(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	const MessageHeader *Msg,
                                        	Connection Conn)
{
	CollectorSession* 	CS ;
	IPDR_Status			Status ;
	IPDR_BOOL			takeThisOne ;

	ExporterProtocol_KeepAliveInTimerReset(Conn->Owner);

	Status = ExporterProtocol_CollectorSession(Coll, S, &CS, &takeThisOne, IPDR_FALSE);

	if( CS == NULL )
	{
		Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
			 "ExporterProtocol_HandleFinalTmplDataACK", MSG_EXPORTER_COLLECTOR_SESSION_FAILED);
		Status = ExporterProtocol_SendError(E, S, Coll, Conn, CollectorNotConfigured, MSG_EXPORTER_COLLECTOR_SESSION_FAILED);
		if(FAILURE(Status))
		{
			TL_HandleDisconnection(Conn);
		}
		return IPDR_ERR;
	}

	if(CS->TmplDataTimer != NULL)
    	Platform_UnregisterTimerAndClear(EXPORTER_PLATFORM, &CS->TmplDataTimer);

	if( takeThisOne && S->Running == IPDR_TRUE)
	{
		/* This collector was sent supposed to be the active one */
		/* Send session start to this one */
		Status = ExporterProtocol_SendSessionStart(E, S, CS->Collector, CS->SessionConnection->TCPConnection);
		if(FAILURE(Status))
		{
			Log_Write1(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
			 "ExporterProtocol_HandleFinalTmplDataACK", MSG_EXPORTER_SEND_SESSION_START_FAILED,
			 TL_GetConnectionInfoStr(Conn));
			 TL_HandleDisconnection(Conn);
			 return IPDR_ERR;

		}
		S->ActiveCS = CS ;
		S->Retransmite = IPDR_TRUE;
		CS->CSState = IPDR_CS_ACTIVE;
		Status = Session_SendOutstandingData(E, S);
		/* Send NN template data to rest of collectors in this session  */
		ExporterProtocol_SyncTemplateDataAll(E, S);

		Log_StateTransition(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO,
							"ExporterProtocol_HandleFinalTmplDataACK", Conn, CS->CSState, IPDR_CS_ACTIVE,
							"got template final data ack - ready to send data");
		ExporterProtocol_FlushRecordsQueue(E, S, CS->Collector, "FinalTmplDataACK");
	}
	else
	{
		/* check if the state is correct*/
		if(CS->CSState != IPDR_CS_INITIATION)
		{
			Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
			 	  "ExporterProtocol_HandleFlowStart", MSG_EXPORTER_COLLECTOR_NOT_INITIATION);
			Status = ExporterProtocol_SendError(E, S, Coll, Conn, MessageInvalidForState, MSG_EXPORTER_COLLECTOR_NOT_INITIATION);
			if(FAILURE(Status))
			{
				TL_HandleDisconnection(Conn);
			}
			return IPDR_OK;
		}
		CS->CSState = IPDR_CS_READY;
		Log_StateTransition(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO,
                            "ExporterProtocol_HandleFinalTmplDataACK", Conn, CS->CSState, IPDR_CS_READY,
                            "got template final data ack - not ActiveCS");
	}
    return IPDR_OK;
}

/**
 * Handle a START NEGOTIATE message from a collector.
 *
 * @param E exporter handle
 * @param S session handle (pointer)
 * @param Coll server
 * @param Msg received message
 * @param Conn connection
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status ExporterProtocol_HandleStartNegotiation(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	const MessageHeader *Msg,
                                        	Connection Conn)
{
	IPDR_BOOL Negotiate = 	IPDR_FALSE ;
	CollectorSession* 	CS;
	IPDR_Status 		Status ;

	ExporterProtocol_KeepAliveInTimerReset(Conn->Owner);

	if(E->StartNegotiationCallback != NULL)
		E->StartNegotiationCallback(S, Coll, &Negotiate, E->StartNegotiationCallbackArg);

	if(Negotiate == IPDR_TRUE && S->Negotiable == IPDR_TRUE)
	{
		if( FAILURE(ExporterProtocol_CollectorSession(Coll, S, &CS, NULL, IPDR_TRUE)))
		{
			Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO,
				          "ExporterProtocol_HandleStartNegotiation", MSG_EXPORTER_SEND_NEGOTIATION_REJECT_FAILED);
			TL_HandleDisconnection(Conn);
			return IPDR_ERR;
		}
		if( CS->CSState != IPDR_CS_ACTIVE )
		{
			/* TODO: set up correct message defines*/
			Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO,
				          "ExporterProtocol_HandleStartNegotiation", "StartNegotiation is invalid in this state");
			ExporterProtocol_SendError(E, S, Coll, Conn, MessageInvalidForState, "StartNegotiation is invalid in this state");
			return IPDR_ERR;
		}
		CS->CSState = IPDR_CS_INITIATION;
		ExporterProtocol_FlushRecordsQueue(E, S, Coll, "StartNegotiate");

		Status = ExporterProtocol_SendSessionStop(E, S, Coll, Conn, IPDR_STOP_REASON_RENEGOTIATION, "StartNegotiate");
		if(FAILURE(Status))
		{
			Log_Write1(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO,
				   "ExporterProtocol_HandleStartNegotiation",
				   MSG_EXPORTER_SEND_SESSION_STOP_FAILED,"StartNegotiate");
			TL_HandleDisconnection(Conn);
			return IPDR_ERR;
		}
		Status = ExporterProtocol_SendTemplateData(E, S, Coll, Conn, IPDR_TRUE);
		if(FAILURE(Status))
		{
			Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO,
				          "ExporterProtocol_HandleStartNegotiation", MSG_EXPORTER_SEND_TEMPLATE_DATA_FAILED);
			TL_HandleDisconnection(Conn);
			return IPDR_ERR;
		}
	}
	else
	{
		Status = ExporterProtocol_SendStartNegotiationReject(E, S, Coll, Conn);
		if(FAILURE(Status))
		{
			Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO,
				          "ExporterProtocol_HandleStartNegotiation", MSG_EXPORTER_SEND_NEGOTIATION_REJECT_FAILED);
			TL_HandleDisconnection(Conn);
			return IPDR_ERR;
		}
	}
    return IPDR_OK;
}

/**
 * Handle a GET SESSIONS message from a collector.
 *
 * @param E exporter handle
 * @param S session handle (pointer)
 * @param Coll collector
 * @param Msg received message
 * @param Conn connection
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status ExporterProtocol_HandleGetSessions(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	const MessageHeader *Msg,
                                        	Connection Conn)
{
    const GetSessionsMessage *GSM = (const GetSessionsMessage *) Msg;
	unsigned short RequestID = IPDR_NTOHS(GSM->RequestID);

	ExporterProtocol_KeepAliveInTimerReset(Conn->Owner);

    return ExporterProtocol_SendGetSessionsResponse(E, S, Coll, Conn, RequestID);
}

/**
 * Handle a GET TMPLS message from a server (collector).
 *
 * @param E exporter handle
 * @param S session handle (pointer)
 * @param Coll collector
 * @param Msg received message
 * @param Conn The Connection between S to Coll
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status ExporterProtocol_HandleGetTemplates(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	const MessageHeader *Msg,
                                        	Connection Conn)
{
    const GetTemplatesMessage *GTH = (const GetTemplatesMessage *) Msg;
	unsigned short RequestID = IPDR_NTOHS(GTH->RequestID);

	ExporterProtocol_KeepAliveInTimerReset(Conn->Owner);

    return ExporterProtocol_SendGetTemplatesResponse(E, S, Coll, Conn, RequestID);
}

/**
 * Handle a DATA ACK message from a server (collector).
 *
 * @param E exporter handle
 * @param S session handle (pointer)
 * @param Coll collector originating the message
 * @param Msg received message
 * @param Conn connection handle
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status ExporterProtocol_HandleDataACK(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	const MessageHeader *Msg,
                                        	Connection Conn)
{
   	const DataACKMessage*	DAM = (const DataACKMessage *) Msg;
    IPDR_DSN_TYPE			DSN = 0 ;
    unsigned short 			ConfigurationID = IPDR_NTOHS(DAM->ConfigurationID);
	CollectorSession* 		CS ;
    TransmissionContext*	TC = S->ActiveContext;
	IPDR_Status				Status ;

	ExporterProtocol_KeepAliveInTimerReset(Conn->Owner);

	DSN = IPDR_NTOHLL(DAM->sequenceNum);

	Log_Write4(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_DEBUG,
                   "ExporterProtocol_HandleDataACK", MSG_EXPORTER_DATA_ACK,
		   DSN, EXPORTER_QHANDLER->OutstandingDataCount(E, S),
		   EXPORTER_QHANDLER->UnacknowledgedDataCount(E, S), TL_GetConnectionInfoStr(Conn));

	Status = ExporterProtocol_CollectorSession(Coll, S, &CS, NULL, IPDR_TRUE );

	if( FAILURE(Status) || CS == NULL)
	{
		Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
			 "ExporterProtocol_HandleDataACK", MSG_EXPORTER_COLLECTOR_SESSION_FAILED);
		Status = ExporterProtocol_SendError(E, S, Coll, Conn, CollectorNotConfigured, MSG_EXPORTER_COLLECTOR_SESSION_FAILED);
		if(FAILURE(Status))
		{
			TL_HandleDisconnection(Conn);
		}
		return Status;
	}

    /* Because IPDR_INVALID_DSN is negative, it ought not have any
       effect ... but just to be sure, let's short-circuit the whole
       execution path below. */
    if (IPDR_INVALID_DSN(DSN))
        return IPDR_OK;

    if (TC->ConfigurationID != ConfigurationID)
        return IPDR_OK;

    if (S->ActiveCS == CS)
	{
        unsigned int RemovedRecords;
        EXPORTER_QHANDLER->ACK(E, S, CS, DSN, &RemovedRecords);
        if (RemovedRecords > 0)
		{
            if (EXPORTER_QHANDLER->UnacknowledgedDataCount(E, S) <= 0 &&
                EXPORTER_QHANDLER->OutstandingDataCount(E, S) <= 0)
			{
                IPDR_EventData eventData;

                Platform_UnregisterTimerAndClear(EXPORTER_PLATFORM, &CS->WindowSizeTimer);

                EventDataInit(&eventData, IPDR_EVENT_QUEUE_EMPTY, E, CS, S);
                Exporter_TriggerEvent(E, IPDR_LEVEL_INFO, &eventData, NULL, /* don't record this event in server's "last event" */
                                    MSG_EXPORTER_QUEUE_EMPTIED,
                                    TL_GetConnectionInfoStr(CS->SessionConnection->TCPConnection),
                                    EXPORTER_QHANDLER->OutstandingDataCount(E, S),
                                    EXPORTER_QHANDLER->UnacknowledgedDataCount(E, S),
                                    S->ID, S->Name, S->Description);
			}
			else
			{
				Platform_ReregisterTimer(EXPORTER_PLATFORM,
                                         Exporter_WindowSizeTimerCallback, CS,
                                         E->Parameters.WindowTimeout,
                                         &CS->WindowSizeTimer);
			}
			/* We don't need to check whether:
			EXPORTER_QHANDLER->OutstandingDataCount(C, S) <
			C->Parameters.WindowSize)
			because the flush will do this implicitly (at most WindowSize
			records will be outstanding). */
        	ExporterProtocol_FlushRecordsQueue(E, S, Coll, "DataACK");
		}
	}
	else
	{
        /* When we received an data ack from a server (collector) with a higher
           priority, which is paused, reselect the active server (collector). Note
           that this means that an ACK from a lower priority server (collector)
           will be ignored; but that shouldn't matter because the
           higher priority server (collector) will be ACKing as well. In case the
           higher priority server (collector) does time out, the lower priority
           server (collector) will be re-sent everything, which is safe, even
           though perhaps not desirable (but also quite
           unlikely). Also note that this code does behave correctly
           in the situation with a single server (collector) that times out and
           then sends an ACK. */
        /* The fact that we got a DATA ACK indicates that that this
           server (collector) ought to be put into a more ready state if it isn't
           already */
		if (S->ActiveCS == NULL ||
			( (CS->CSState == IPDR_CS_READY ) && CS->Priority < S->ActiveCS->Priority) )
		{
        		Log_StateTransition(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO,
                                "ExporterProtocol_HandleDataACK",
								CS->SessionConnection->TCPConnection,
								CS->CSState, CS->CSState,
               	                (S->ActiveCS ? MSG_EXPORTER_DATA_ACK1 : MSG_EXPORTER_DATA_ACK2));

			Session_SelectActiveCollector(E, S);
			if (S->ActiveCS != NULL && S->ActiveCS->Collector != NULL &&
				S->ActiveCS->SessionConnection != NULL &&
				S->ActiveCS->SessionConnection->TCPConnection != NULL &&
				(S->ActiveCS->CSState == IPDR_CS_ACTIVE))
			{
                /* (recursively) do it with the new active server (collector) */
                return ExporterProtocol_HandleDataACK(E, S, S->ActiveCS->Collector,
													Msg, S->ActiveCS->SessionConnection->TCPConnection);
            }
			else
			{
            	return IPDR_OK;
            }
        }
		else
		{
            /* It's normal to get an ACK from a lower priority server (collector),
               if a fail-back has occurred. Just throw them away.
            */
            Log_StateTransition(EXPORTER_LOG, PROTOCOL_MODULE_ID,
                                IPDR_LEVEL_INFO,
                                "ExporterProtocol_HandleDataACK", CS->SessionConnection->TCPConnection, CS->CSState, CS->CSState,
                                MSG_EXPORTER_DATA_ACK3);
        }
    }
    return IPDR_OK;
}


/**
 * Handles a connect event recived from a collector
 *
 * @param E exporter handle
 * @param S session handle (pointer)
 * @param Coll collector handle (pointer)
 * @param Msg message header from the collector
 * @param Conn The Connection between S to Coll
 */
IPDR_Status ExporterProtocol_HandleConnect(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	const MessageHeader *Msg,
                                        	Connection Conn)
{
	IPDR_Status Status = IPDR_OK;
	IPDRConnection *IC = Conn->Owner;
	const ConnectMessage *CM = (const ConnectMessage *)Msg;
	int clientCapabilities = 0, Index;
	Collector *RealCollector = NULL ;
	TL_PABuf paBuf;

	IPDR_IPAddress Addr = IPDR_NTOHL(CM->Addr);
	Conn->DummyAddr = Addr ;


	if(Coll->isDummy == IPDR_TRUE)
	{
		for (Index = 0; Index < E->Collectors.Length; Index++)
		{
			RealCollector = (Collector *) E->Collectors.Data[Index];
			if (Addr == RealCollector->Addr)
				break ;

			RealCollector = NULL ;
		}

		if(RealCollector == NULL)
		{
			Log_Write1(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
					"ExporterProtocol_HandleConnect", MSG_EXPORTER_UNKNOWN_SERVER,
					TL_PrintConnectionFull(Conn, paBuf));

			Status = ExporterProtocol_SendError(E, S, Coll, Conn, CollectorNotConfigured, MSG_EXPORTER_UNKNOWN_SERVER2);
			if(FAILURE(Status))
			{
				TL_HandleDisconnection(Conn);
			}
			return Status ;
		}

		if(IC->ConnectResponseTimerID != NULL)
			Platform_UnregisterTimerAndClear(EXPORTER_PLATFORM, & (IC->ConnectResponseTimerID));

		if(IC->TCPConnection)
			IC->TCPConnection->Owner = NULL ;
		IC->TCPConnection = NULL ;

		Status = Exporter_AddIPDRConnectionToCollector(	E, RealCollector, Conn );
		if(FAILURE(Status))
		{
			Log_Write1(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
					"Exporter_ConnectCallback", "Failed To Add IPDRConnection To Collector, Addres: %.98s",
					TL_PrintConnectionFull(Conn, paBuf));

			Object_Release(EXPORTER_PLATFORM, Coll);
			return Status;
		}
		Object_Release(EXPORTER_PLATFORM, Coll);
		IC = Conn->Owner;
	}

	if(IC->State != IPDR_CONNECTION_DISCONNECTED)
	{
		Log_Write3(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
                   "ExporterProtocol_HandleConnect", MSG_EXPORTER_UNEXPECTED_CONNECT,
                   TL_PrintConnectionFull(Conn, paBuf), "CONNECTED", "");

		Status = ExporterProtocol_SendError(E, S, RealCollector, Conn, InvalidConnectionState, MSG_EXPORTER_WRONG_COLLECTOR_STATE);
		if(FAILURE(Status))
		{
			TL_HandleDisconnection(Conn);
		}
		return Status ;
	}

	clientCapabilities = IPDR_NTOHL(CM->Capabilities);
	ExporterProtocol_SetCollectorCapabilities(E, RealCollector, clientCapabilities);
	IC->RemoteKeepAliveInterval = IPDR_NTOHL(CM->keepAliveInterval);

	Status = ExporterProtocol_SendConnectResponse(E, S, RealCollector, Conn);
	if(FAILURE(Status))
	{
		Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
                  "ExporterProtocol_HandleConnect", MSG_EXPORTER_SEND_CONNECT_RESPONSE_ERROR);
		TL_HandleDisconnection(Conn);
		return Status ;
	}

	IC->State = IPDR_CONNECTION_CONNECTED;
	if(IC->KeepAliveINTimerID != NULL)
	{
		Platform_UnregisterTimerAndClear(EXPORTER_PLATFORM, &IC->KeepAliveINTimerID);
		IC->KeepAliveINTimerID = NULL;
	}
	Platform_RegisterPeriodicTimer(EXPORTER_PLATFORM,
							ExporterProtocol_KeepAliveInTimerCallback,
							IC,
							(E->Parameters.KeepAliveInterval)+500,
							& (IC->KeepAliveINTimerID));

	if(IC->KeepAliveOUTTimerID != NULL)
	{
		Platform_UnregisterTimerAndClear(EXPORTER_PLATFORM, &IC->KeepAliveOUTTimerID);
		IC->KeepAliveOUTTimerID = NULL;
	}
	Platform_RegisterPeriodicTimer(EXPORTER_PLATFORM,
							ExporterProtocol_KeepAliveOutTimerCallback,
							IC,
							((IC->RemoteKeepAliveInterval)*1000)-500,
							& (IC->KeepAliveOUTTimerID));

	return IPDR_OK ;
}


/**
 * Handles a connect response event recived from a collector
 *
 * @param E exporter handle
 * @param S session handle (pointer)
 * @param Coll collector handle (pointer)
 * @param Msg message header from the collector
 * @param Conn The Connection between S to Coll
 */
IPDR_Status ExporterProtocol_HandleConnectResponse(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	const MessageHeader *Msg,
                                        	Connection Conn)
{
	IPDR_Status Status = IPDR_OK;
	IPDRConnection *IC = Conn->Owner;
	const ConnectResponseMessage *CRM = (const ConnectResponseMessage *)Msg;
	int clientCapabilities = 0;

	if(IC->State != IPDR_CONNECTION_WAITRESPONSE)
	{
		Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
                  "ExporterProtocol_HandleConnectResponse",
                  MSG_EXPORTER_STATE_NOT_WAITRESPONSE);
		Status = ExporterProtocol_SendError(E, S, Coll, Conn, InvalidConnectionState, MSG_EXPORTER_WRONG_COLLECTOR_STATE);
		if(FAILURE(Status))
		{
			TL_HandleDisconnection(Conn);
		}
		return Status ;
	}

	Platform_Lock(EXPORTER_PLATFORM);

	Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO,
                  "ExporterProtocol_HandleConnectResponse",
                  MSG_EXPORTER_CONNECT_RESPONSE_RECEIVED);
	Platform_UnregisterTimerAndClear(EXPORTER_PLATFORM, &IC->ConnectResponseTimerID);
	IC->ConnectResponseTimerID = NULL;

	IC->State = IPDR_CONNECTION_CONNECTED;
	IC->RemoteKeepAliveInterval = IPDR_NTOHL(CRM->keepAliveInterval);
	clientCapabilities = IPDR_NTOHL(CRM->Capabilities);
	ExporterProtocol_SetCollectorCapabilities(E, Coll, clientCapabilities);
	if(IC->KeepAliveINTimerID != NULL)
	{
		Platform_UnregisterTimerAndClear(EXPORTER_PLATFORM, &IC->KeepAliveINTimerID);
		IC->KeepAliveINTimerID = NULL;
	}
	Platform_RegisterPeriodicTimer(EXPORTER_PLATFORM,
							ExporterProtocol_KeepAliveInTimerCallback,
							IC,
							(E->Parameters.KeepAliveInterval)+500,
							& (IC->KeepAliveINTimerID));

	if(IC->KeepAliveOUTTimerID != NULL)
	{
		Platform_UnregisterTimerAndClear(EXPORTER_PLATFORM, &IC->KeepAliveOUTTimerID);
		IC->KeepAliveOUTTimerID = NULL;
	}
	Platform_RegisterPeriodicTimer(EXPORTER_PLATFORM,
							ExporterProtocol_KeepAliveOutTimerCallback,
							IC,
							((IC->RemoteKeepAliveInterval)*1000)-500,
							& (IC->KeepAliveOUTTimerID));

	Platform_Unlock(EXPORTER_PLATFORM);
	return IPDR_OK;
}


/**
 * Handles a disconnect event recived from a collector
 *
 * @param E exporter handle
 * @param S session handle (pointer)
 * @param Coll collector handle (pointer)
 * @param Msg message header from the collector
 * @param Conn The Connection between S to Coll
 */
IPDR_Status ExporterProtocol_HandleDisconnect(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	const MessageHeader *Msg,
                                        	Connection Conn)
{
	IPDRConnection*			IC = Conn->Owner;

	if( IC == NULL )
	{
		Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
                  "ExporterProtocol_HandleDisconnect",
                  MSG_NO_CONNECTION_OWNER);
		return IPDR_ERR ;
	}

	TL_HandleDisconnection(Conn);
	return IPDR_OK;
}

/**
 * Handles an error event recived from a collector
 *
 * @param E exporter handle
 * @param S session handle (pointer)
 * @param Coll collector handle (pointer)
 * @param Msg message header from the collector
 * @param Conn The Connection between S to Coll
 */
IPDR_Status ExporterProtocol_HandleError(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	const MessageHeader *Msg,
                                        	Connection Conn)
{
	IPDR_Time 		ErrorTime;
	const ErrorMessage*	EM;
	enum ErrorCode ErrCode;
	unsigned int 	MsgLen;
	char			Buf[EXPORTER_PROTOCOL_MAX_MSG];

	ExporterProtocol_KeepAliveInTimerReset(Conn->Owner);

	EM = (const ErrorMessage*)Msg;
	ErrorTime = IPDR_NTOHL(EM->Timestamp);
    ErrCode = IPDR_NTOHS(EM->ErrorCode);
    MsgLen = IPDR_NTOHL(EM->DescrLength);
	memset(Buf, 0, EXPORTER_PROTOCOL_MAX_MSG);
	strncpy(Buf, (char*)((size_t)EM + sizeof(ErrorMessage)), MsgLen);
	Buf[MsgLen] = 0;
	Log_Write3(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_INFO,
               "ExporterProtocol_HandleError", MSG_EXPORTER_ERROR_RECEIVED,
			   TL_GetConnectionInfoStr(Conn), ErrCode, Buf);
	return IPDR_OK;
}

IPDR_Status ExporterProtocol_HandleKeepAlive(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	const MessageHeader *Msg,
                                        	Connection Conn)
{
	TL_PABuf paBuf;
	IPDRConnection *IC = (IPDRConnection *)Conn->Owner;
	ExporterProtocol_KeepAliveInTimerReset(IC);
	Log_Write1(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_INFO,
               "ExporterProtocol_HandleKeepAlive",
               MSG_EXPORTER_SEND_KEEPALIVE_RECEIVED,
			   TL_PrintConnectionRemote(Conn, paBuf));
	return IPDR_OK;
}

/**
 * Sends a connect event to a collector
 *
 * @param E exporter handle
 * @param S session handle (pointer)
 * @param Coll collector handle (pointer)
 * @param Conn The Connection between S to Coll
 */
IPDR_Status ExporterProtocol_SendConnect(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	Connection Conn)
{
	IPDR_Status Status;
	int exporterCapabilities = 0;
	int collectorCapabilities = 0;
    BufferHandle BH;
	/*MessageHeader	*MH;*/
    ConnectMessage *CM;
    ChunksManager *CMgr;
	IPDRConnection *IC = Conn->Owner;

	char*	vendorID = E->Parameters.VendorString;
	int vendorIDLength = strlen(vendorID);

	ExporterProtocol_KeepAliveOutTimerReset(IC);


    if (IC->State != IPDR_CONNECTION_DISCONNECTED) {
        /* XXX should signal error event */
        Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
                  "ExporterProtocol_SendConnect",
                  MSG_EXPORTER_STATE_NOT_DISCONNECTED);
        return IPDR_ERR;
    }

    TL_GetSendChunksManager(Conn, & CMgr);
    Status = ChunksManager_AllocateBuffer(CMgr,
                                          sizeof(ConnectMessage)+vendorIDLength+1,
										  & BH);
    if (FAILURE(Status))
        return Status;

    ExporterProtocol_FillHeader(S, BH.Data, IPDR_MSG_CONNECT,
                              sizeof(ConnectMessage)+vendorIDLength+1);

    CM = (ConnectMessage *) BH.Data;

	exporterCapabilities = Exporter_GetCapabilities(E);
	ExporterProtocol_SetCollectorCapabilities(E, Coll, exporterCapabilities);
	collectorCapabilities = ExporterProtocol_GetCollectorCapabilities(Coll);

	IPDR_SET_HTONL(CM->Addr, ((E->Parameters.ProtocolLevelAddr==0)?Conn->LocaleAddr:E->Parameters.ProtocolLevelAddr));
	IPDR_SET_HTONS(CM->Port, Conn->LocalePort);
	IPDR_SET_HTONL(CM->Capabilities, collectorCapabilities);
	IPDR_SET_HTONL(CM->keepAliveInterval, E->Parameters.KeepAliveInterval/1000);
	IPDR_SET_HTONL(CM->vendorIDLength, vendorIDLength);
	strncpy(BH.Data+sizeof(ConnectMessage), vendorID, vendorIDLength);

    if (Log_ShouldWrite(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO)) {
        ProtocolDebug_PrintOutgoingMessage(EXPORTER_LOG, Conn, BH.Data, IPDR_LEVEL_INFO);
    }

    Status = TL_Send(Conn, & BH, IPDR_TRUE);
	if(FAILURE(Status))
	{
		IPDR_LogWrite(E,"ExporterProtocol", IPDR_LEVEL_INFO, "ExporterProtocol_SendConnect",
					MSG_EXPORTER_SEND_CONNECT_FAILED, TL_GetConnectionInfoStr(Conn));
		return Status;
	}

	Log_Write2(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_DEBUG,
					"Exporter_AutoconnectCallback",
					MSG_EXPORTER_SEND_CONNECT_TARGET,
					TL_GetConnectionInfoStr(Conn), E->Parameters.ConnectResponseTimeout);

	if(IC->ConnectResponseTimerID != NULL)
		Platform_UnregisterTimerAndClear(EXPORTER_PLATFORM, & (IC->ConnectResponseTimerID));

	Platform_RegisterTimer(EXPORTER_PLATFORM,
    						ExporterProtocol_ConnectResponseTimeoutCallback,
							IC,
                            E->Parameters.ConnectResponseTimeout,
                            & (IC->ConnectResponseTimerID));

	IC->State = IPDR_CONNECTION_WAITRESPONSE;

    return Status;
}


IPDR_Status ExporterProtocol_SendConnectResponse(Exporter *E,
											Session *S,
											Collector *Coll,
											Connection Conn)
{
	IPDR_Status Status;
	int  collectorCapabilities = 0;
    BufferHandle BH;
    ConnectResponseMessage *CRM;
    ChunksManager *CMgr;
	IPDRConnection *IC = Conn->Owner;

	char*	vendorID = E->Parameters.VendorString;
	int vendorIDLength = strlen(vendorID);

	ExporterProtocol_KeepAliveOutTimerReset(IC);


    if (IC->State != IPDR_CONNECTION_DISCONNECTED) {
        /* XXX should signal error event */
        Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
                  "ExporterProtocol_SendConnectResponse",
                  MSG_EXPORTER_STATE_NOT_DISCONNECTED);
        return IPDR_ERR;
    }

    TL_GetSendChunksManager(Conn, & CMgr);
    Status = ChunksManager_AllocateBuffer(CMgr,
                                          sizeof(ConnectResponseMessage)+vendorIDLength+1,
										  & BH);
    if (FAILURE(Status))
        return Status;

    ExporterProtocol_FillHeader(S, BH.Data, IPDR_MSG_CONNECT_RESPONSE,
                              sizeof(ConnectResponseMessage)+vendorIDLength+1);

    CRM = (ConnectResponseMessage *) BH.Data;

	collectorCapabilities = ExporterProtocol_GetCollectorCapabilities(Coll);

	IPDR_SET_HTONL(CRM->Capabilities, collectorCapabilities);
	IPDR_SET_HTONL(CRM->keepAliveInterval, E->Parameters.KeepAliveInterval/1000);
	IPDR_SET_HTONL(CRM->vendorIDLength, vendorIDLength);
	strncpy(BH.Data+sizeof(ConnectResponseMessage), vendorID, vendorIDLength);

    if (Log_ShouldWrite(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO)) {
        ProtocolDebug_PrintOutgoingMessage(EXPORTER_LOG, Conn, BH.Data, IPDR_LEVEL_INFO);
    }

    Status = TL_Send(Conn, & BH, IPDR_TRUE);
	if(FAILURE(Status))
	{
		IPDR_LogWrite(E,"ExporterProtocol", IPDR_LEVEL_ERROR, "ExporterProtocol_SendConnectResponse",
					MSG_EXPORTER_SEND_CONNECT_RESPONSE_FAILED, TL_GetConnectionInfoStr(Conn));
		return Status;
	}

	IPDR_LogWrite(E,"ExporterProtocol", IPDR_LEVEL_INFO, "ExporterProtocol_SendConnectResponse",
					MSG_EXPORTER_SEND_CONNECT_RESPONSE_TARGET, TL_GetConnectionInfoStr(Conn));

    return IPDR_OK;
}

IPDR_Status ExporterProtocol_SendDisconnect(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	Connection Conn)
{
	IPDR_Status 		Status;
	BufferHandle 		BH;
    DisconnectMessage*	DM;
    ChunksManager*		CMgr;
	IPDRConnection*		IC = Conn->Owner;

	ExporterProtocol_KeepAliveOutTimerReset(IC);

    if (IC->State == IPDR_CONNECTION_DISCONNECTED) {
        /* XXX should signal error event */
        Log_Write3(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
                  "ExporterProtocol_SendDisconnect", MSG_EXPORTER_UNEXPECTED_MSG,
				TL_GetConnectionInfoStr(Conn), IPDR_MSG_DISCONNECT, "DISCONNECTED");
        return IPDR_ERR;
    }

    TL_GetSendChunksManager(Conn, & CMgr);
    Status = ChunksManager_AllocateBuffer(CMgr,
                                          sizeof(DisconnectMessage),
										  & BH);
    if (FAILURE(Status))
    {
    	Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
		  "ExporterProtocol_SendDisconnect", MSG_EXPORTER_CHUNKS_MANAGER_FAILED);
        return Status;
    }

    ExporterProtocol_FillHeader(S, BH.Data, IPDR_MSG_DISCONNECT,
                              sizeof(DisconnectMessage));

    DM = (DisconnectMessage *) BH.Data;

    if (Log_ShouldWrite(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO)) {
        ProtocolDebug_PrintOutgoingMessage(EXPORTER_LOG, Conn, BH.Data, IPDR_LEVEL_INFO);
    }

    Status = TL_Send(Conn, & BH, IPDR_TRUE);
	if(FAILURE(Status))
	{
		Log_Write1(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
			   "ExporterProtocol_SendDisconnect",
			   MSG_EXPORTER_SEND_DISCONNECT_FAILED, TL_GetConnectionInfoStr(Conn));
		return Status;
	}

	Log_Write1(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_DEBUG,
		   "ExporterProtocol_SendDisconnect",
		   MSG_EXPORTER_SEND_DISCONNECT_TARGET, TL_GetConnectionInfoStr(Conn));

	if(IC->KeepAliveOUTTimerID != NULL)
	{
		Platform_UnregisterTimerAndClear(EXPORTER_PLATFORM, &IC->KeepAliveOUTTimerID);
		IC->KeepAliveOUTTimerID = NULL;
	}

	if(IC->KeepAliveINTimerID != NULL)
	{
		Platform_UnregisterTimerAndClear(EXPORTER_PLATFORM, &IC->KeepAliveINTimerID);
		IC->KeepAliveINTimerID = NULL;
	}

	IC->State = IPDR_CONNECTION_DISCONNECTED;
    return IPDR_OK;
}


/**
 * Send an ERROR message to a collector.
 *
 * @param E exporter handle (pointer)
 * @param S session handle (pointer)
 * @param Coll collector to which to send the message
 * @param Conn connection
 * @param ErrCode the error code
 * @param Msg the message to send (may be null or zero-length).
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status ExporterProtocol_SendError(Exporter *E,Session *S,Collector *Coll,Connection Conn,enum ErrorCode ErrCode,const char* Msg)
{
	IPDR_Status Status;
	BufferHandle BH;
	ChunksManager *CMgr;
    ErrorMessage *EM;
    IPDR_Time Now;
	IPDRConnection *IC = Conn->Owner;
    unsigned int MsgLen = (Msg == NULL) ? 0 : strlen(Msg);
    Platform_GetTime(EXPORTER_PLATFORM, &Now);

	if(Coll->isDummy == IPDR_FALSE)
		ExporterProtocol_KeepAliveOutTimerReset(IC);

	if (IC->TCPConnection == NULL /*||
	((IC->State == IPDR_CONNECTION_DISCONNECTED ||
	IC->State == IPDR_CONNECTION_WAITRESPONSE) && (Coll->isDummy == IPDR_FALSE))*/)
	{
        /* XXX should signal error event */
        Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
                  "ExporterProtocol_SendErrorMsg",
                  MSG_EXPORTER_NOT_READY_SEND);
        return IPDR_ERR_NO_ACTIVE_COLLECTOR;
    }

    TL_GetSendChunksManager(Conn, & CMgr);
    Status = ChunksManager_AllocateBuffer(CMgr,
                                          sizeof (ErrorMessage) + MsgLen + 1,
                                          & BH);
    if (FAILURE(Status))
    {
    	Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
		  "ExporterProtocol_SendError", MSG_EXPORTER_CHUNKS_MANAGER_FAILED);
        return Status;
    }


    EM = (ErrorMessage*)BH.Data;
    ExporterProtocol_FillHeader(S, BH.Data, IPDR_MSG_ERROR,
                              	sizeof (ErrorMessage)  + MsgLen + 1 );

    IPDR_SET_HTONL(EM->Timestamp, Now);
    IPDR_SET_HTONS(EM->ErrorCode, ErrCode);
    IPDR_SET_HTONL(EM->DescrLength, MsgLen);
    /* The description comes after the ErrorMessage ... it would have
     * been nice to have had a char Descr[0] at the end of the
     * struct,so we could do "memset(&EM->Data, ...)" but some
     * incorrect compilers don't allow this. */
    memset(BH.Data + sizeof (ErrorMessage), 0, MsgLen + 1);
    strncpy(BH.Data + sizeof (ErrorMessage), Msg, MsgLen);

    if (Log_ShouldWrite(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO)) {
        ProtocolDebug_PrintOutgoingMessage(EXPORTER_LOG, Conn, BH.Data, IPDR_LEVEL_INFO);
    }

    Status = TL_Send(Conn, & BH, IPDR_TRUE);
    if (FAILURE(Status))
	{
		Log_Write1(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO,
					"ExporterProtocol_SendError",
					MSG_EXPORTER_ERROR_SEND_FAILED, TL_GetConnectionInfoStr(Conn));
		return Status;
	}

	/*TODO: output detailed error message*/
	Log_Write1(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_DEBUG,
					"ExporterProtocol_SendError",
					MSG_EXPORTER_ERROR_SEND_TARGET, TL_GetConnectionInfoStr(Conn));

    return IPDR_OK;
}

IPDR_Status ExporterProtocol_SendKeepAlive(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	Connection Conn)
{
	IPDR_Status 	Status;
	BufferHandle 	BH;
    ChunksManager*	CMgr;
	IPDRConnection*	IC = NULL;
	TL_PABuf paBuf;

	if(!Conn)
		return IPDR_OK ;

	IC = Conn->Owner;

	Platform_Lock(EXPORTER_PLATFORM);

	if (IC->State != IPDR_CONNECTION_CONNECTED) {
        /* XXX should signal error event */
        Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
                  "ExporterProtocol_SendKeepAlive", MSG_EXPORTER_CONNECTION_MUST_BE_SET);

		Platform_Unlock(EXPORTER_PLATFORM);
        return IPDR_ERR;
    }

	ExporterProtocol_KeepAliveOutTimerReset(IC);

    TL_GetSendChunksManager(Conn, & CMgr);
    Status = ChunksManager_AllocateBuffer(CMgr,
                                          sizeof(MessageHeader),
										  & BH);
    if (FAILURE(Status))
	{
		Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
			   "ExporterProtocol_SendKeepAlive", MSG_EXPORTER_CHUNKS_MANAGER_FAILED);

		Platform_Unlock(EXPORTER_PLATFORM);
        return Status;
	}

    ExporterProtocol_FillHeader(S, BH.Data, IPDR_MSG_KEEP_ALIVE,
                              sizeof(MessageHeader));

    if (Log_ShouldWrite(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO)) {
        ProtocolDebug_PrintOutgoingMessage(EXPORTER_LOG, Conn, BH.Data, IPDR_LEVEL_INFO);
    }

    Status = TL_Send(Conn, & BH, IPDR_TRUE);
	if(FAILURE(Status))
	{
		Log_Write1(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO,
					"ExporterProtocol_SendKeepAlive",
					MSG_EXPORTER_SEND_KEEPALIVE_FAILED, TL_GetConnectionInfoStr(Conn));
		TL_HandleDisconnection(Conn);

		Platform_Unlock(EXPORTER_PLATFORM);
		return Status;
	}

	Log_Write1(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO,
					"ExporterProtocol_SendKeepAlive",
					MSG_EXPORTER_SEND_KEEPALIVE_TARGET, TL_PrintConnectionRemote(Conn,paBuf));

	Platform_Unlock(EXPORTER_PLATFORM);
    return Status;
}

IPDR_Status ExporterProtocol_SendSessionStart(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	Connection Conn)
{
	IPDR_Status 			Status;
	BufferHandle 			BH;
	IPDRConnection*			IC = Conn->Owner;
	SessionStartMessage*	SSM;
	ChunksManager*			CMgr;

	ExporterProtocol_KeepAliveOutTimerReset(IC);

	TL_GetSendChunksManager(Conn, & CMgr);
    Status = ChunksManager_AllocateBuffer(CMgr, sizeof(SessionStartMessage), &BH);
    if (FAILURE(Status))
	{
		Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
			  "ExporterProtocol_SendSessionStart", MSG_EXPORTER_CHUNKS_MANAGER_FAILED);
        return Status;
	}

    ExporterProtocol_FillHeader(S, BH.Data, IPDR_MSG_SESSION_START, sizeof(SessionStartMessage));

	SSM = (SessionStartMessage*)BH.Data;

    IPDR_SET_HTONL(SSM->exporterBootTime, E->ExporterStartTime);
    IPDR_SET_HTONLL(SSM->firstRecordSequenceNumber, S->DSN);
    IPDR_SET_HTONLL(SSM->droppedRecordCount, 0);
    IPDR_SET_HTONL(SSM->ackTimeInterval,((E->Parameters.DataAckTimeInterval)/1000) );
    IPDR_SET_HTONL(SSM->ackSequenceInterval, E->Parameters.DataAckSequenceInterval );
    memcpy( SSM->documentId, S->DocumentID, sizeof(S->DocumentID) );
    SSM->primary = 1 ;

	Log_Write2(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO,
			"ExporterProtocol_SendSessionStart",
			"ackTimeInterval=%lu, dataAckSequenceInterval=%lu.",
			(unsigned long)IPDR_NTOHL(SSM->ackTimeInterval),
			(unsigned long)IPDR_NTOHL(SSM->ackSequenceInterval));


    if (Log_ShouldWrite(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO)) {
        ProtocolDebug_PrintOutgoingMessage(EXPORTER_LOG, Conn, BH.Data, IPDR_LEVEL_INFO);
    }

	Status = TL_Send(Conn, & BH, IPDR_TRUE);
	if(FAILURE(Status))
	{
		Log_Write1(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO,
					"ExporterProtocol_SendSessionStart",
					MSG_EXPORTER_SEND_SESSION_START_FAILED, TL_GetConnectionInfoStr(Conn));
		return Status;
	}

	Log_Write1(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_DEBUG,
					"ExporterProtocol_SendSessionStart",
					MSG_EXPORTER_SEND_SESSION_START_TARGET, TL_GetConnectionInfoStr(Conn));
	return IPDR_OK;
}

IPDR_Status ExporterProtocol_SendSessionStop(Exporter *E,Session *S,Collector *Coll,
					     Connection Conn,unsigned short reasonCode,	const char* ReasonDescripsion)
{
	IPDR_Status Status;
    BufferHandle BH;
    ChunksManager *CMgr;
    SessionStopMessage *SSM;
    IPDRConnection *IC = NULL;
    unsigned int MsgLen = (ReasonDescripsion == NULL) ? 0 : strlen(ReasonDescripsion);


	if( !E || !S || !Coll )
		return IPDR_ERR_INVALID_VALUE;

	if( Conn == NULL || Conn->Owner == NULL)
	{
		Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
                  "ExporterProtocol_SendSessionStop", MSG_EXPORTER_COLLECTOR_NOT_CONNECTED);
		return IPDR_ERR_NO_ACTIVE_COLLECTOR;
	}

	IC = Conn->Owner;

	ExporterProtocol_KeepAliveOutTimerReset(IC);

	if (IC->TCPConnection == NULL ||
	    IC->State == IPDR_CONNECTION_DISCONNECTED ||
	    IC->State == IPDR_CONNECTION_WAITRESPONSE)
	{
        /* XXX should signal error event */
        Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
                  "ExporterProtocol_SendSessionStop", MSG_EXPORTER_NOT_READY_SEND);
        return IPDR_ERR_NO_ACTIVE_COLLECTOR;
    }

    TL_GetSendChunksManager(Conn, & CMgr);
    Status = ChunksManager_AllocateBuffer(CMgr,
                                          sizeof (SessionStopMessage) + MsgLen + 1,
                                          & BH);
    if (FAILURE(Status))
    {
    	Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
		   "ExporterProtocol_SendSessionStop", MSG_EXPORTER_CHUNKS_MANAGER_FAILED);
        return Status;
    }


    SSM = (SessionStopMessage*)BH.Data;
    ExporterProtocol_FillHeader(S, BH.Data, IPDR_MSG_SESSION_STOP,
                              	sizeof (SessionStopMessage)  + MsgLen + 1 );

    IPDR_SET_HTONS(SSM->reasonCode, reasonCode);
    IPDR_SET_HTONL(SSM->reasonInfoLength, MsgLen);
    /* The description comes after the ErrorMessage ... it would have
     * been nice to have had a char Descr[0] at the end of the
     * struct,so we could do "memset(&EM->Data, ...)" but some
     * incorrect compilers don't allow this. */
    memset(BH.Data + sizeof (SessionStopMessage), 0, MsgLen + 1);
    strncpy(BH.Data + sizeof (SessionStopMessage), ReasonDescripsion, MsgLen);

    if (Log_ShouldWrite(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO)) {
		ProtocolDebug_PrintOutgoingMessage(EXPORTER_LOG, Conn, BH.Data, IPDR_LEVEL_INFO);
    }

    Status = TL_Send(Conn, & BH, IPDR_TRUE);
    if (FAILURE(Status))
	{
		Log_Write1(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
					"ExporterProtocol_SendSessionStop",
					MSG_EXPORTER_ERROR_SEND_FAILED, TL_GetConnectionInfoStr(Conn));
		return Status;
	}

	Log_Write1(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_DEBUG,
					"ExporterProtocol_SendError",
					MSG_EXPORTER_ERROR_SEND_TARGET, TL_GetConnectionInfoStr(Conn));

    return IPDR_OK;
}

static IPDR_Status
ExporterProtocol_SyncTemplateDataAll(Exporter *E, Session *S)
{
	IPDR_Status Status ;
	int i ;
	for (i = 0; i < S->CollectorSessions.Length; i++) {
		CollectorSession *CS = (CollectorSession *) S->CollectorSessions.Data[i];
		if(CS == S->ActiveCS)
			continue;

		if(	CS->Collector != NULL &&
			CS->SessionConnection != NULL &&
			CS->SessionConnection->TCPConnection != NULL &&
			(CS->CSState == IPDR_CS_INITIATION ||
			(S->TemplatesChnged == IPDR_TRUE && CS->CSState == IPDR_CS_READY)))
		{
			CS->CSState = IPDR_CS_INITIATION ;
			Status = ExporterProtocol_SendTemplateData(E, S, CS->Collector,
								   CS->SessionConnection->TCPConnection, IPDR_FALSE);
			if(FAILURE(Status))
			{
				TL_HandleDisconnection(CS->SessionConnection->TCPConnection);
				Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO,
				          "ExporterProtocol_SyncTemplateDataAll", MSG_EXPORTER_SEND_TEMPLATE_DATA_FAILED);
			}
		}
	}
	S->TemplatesChnged = IPDR_FALSE;
	return IPDR_OK;
}

/**
 * Finds the collector in session and determines its priority rank.
 *
 * @param Coll collector handle (pointer)
 * @param S session handle (pointer)
 * @param CS a pointer to the CollectorSession handle of the requested collector
 * @param High indicates if Coll has the highest priority is S
 * @param allStates determnes whether to search for Coll between disconnected collectors as well.
 *
 * @return #IPDR_OK on success; #IPDR_ERR otherwise.
 */
static IPDR_Status
ExporterProtocol_CollectorSession(Collector *Coll, Session *S, CollectorSession** CS, IPDR_BOOL *High, IPDR_BOOL allStates)
{
	/*Find CollectorSession...	*/
	int Index = 0;
	if(Coll == NULL || S == NULL || CS == NULL)
		return IPDR_ERR;

	(*CS) = NULL ;
	if( High != NULL )
	{
		(*High) = PRIORITY_MAX;
		for (Index = 0; Index < S->CollectorSessions.Length; Index++)
		{
			(*CS) = (CollectorSession *) S->CollectorSessions.Data[Index];
				/* Find out maximum priority for the session
				if we have maximum one, we have not to wait for timeout */
			if( ((*CS) != NULL) && (High != NULL) && ((*CS)->Priority < (*High)) &&
				((*CS)->CSState == IPDR_CS_READY ||
				(*CS)->CSState == IPDR_CS_ACTIVE ||
				(*CS)->CSState == IPDR_CS_INITIATION ||
				allStates == IPDR_TRUE))
				(*High) = (*CS)->Priority ;
			(*CS) = NULL ;
		}
	}
	for (Index = 0; Index < S->CollectorSessions.Length; Index++)
	{
		(*CS) = (CollectorSession *) S->CollectorSessions.Data[Index];
		if (((*CS) != NULL) && ((*CS)->Collector == Coll))
		{
			if( High != NULL )
				(*High) = (((*High) - (*CS)->Priority) > -1);
			return IPDR_OK ;
		}
		(*CS) = NULL ;
	}
	return IPDR_ERR;
}

size_t
ExporterProtocol_GetFieldDescriptorDataLength( Key* K )
{
	size_t			Size ;


	Size = 	sizeof(unsigned int) + /* key type id */
			sizeof(unsigned int) + /* key id */
			sizeof(unsigned int) +
			strlen(K->Name) + 		/* key name */
			1 ;						/* enabled */
	return Size ;
}

size_t
ExporterProtocol_GetTemplateBlockDataLength( Template* T )
{
	size_t			Size ;
	int				Index ;

	Size = 	sizeof(unsigned short) + /* template id*/
			sizeof(unsigned int) + strlen(T->SchemaName) +
			sizeof(unsigned int) + strlen(T->TypeName) +
			sizeof(unsigned int) ; /* number of fields */

	for( Index = 0 ; Index < T->Keys.Length ; Index++ )
	{
		Size += ExporterProtocol_GetFieldDescriptorDataLength( T->Keys.Data[Index] );
	}
	return Size ;
}

size_t
ExporterProtocol_GetTemplateDataMsgLen(TransmissionContext *TC)
{
	size_t			Size ;
	int				Index ;

	Size = sizeof( TmplDataMessage ) ;
	for( Index = 0 ; Index < TC->Templates.Length ; Index++ )
	{
		Size += ExporterProtocol_GetTemplateBlockDataLength( TC->Templates.Data[Index] );
	}
	return Size ;
}

IPDR_Status
ExporterProtocol_WriteShort( unsigned short Short, unsigned char** Buffer )
{
	memcpy(*Buffer, &Short, sizeof(unsigned short) );
	*Buffer += sizeof(unsigned short);
	return IPDR_OK;
}

IPDR_Status
ExporterProtocol_WriteInt( unsigned int Int, unsigned char** Buffer )
{
	memcpy(*Buffer, &Int, sizeof(unsigned int) );
	*Buffer += sizeof(unsigned int);
	return IPDR_OK;
}

IPDR_Status
ExporterProtocol_WriteLong( long long Long, unsigned char** Buffer )
{
	memcpy(*Buffer, &Long, sizeof(long long) );
	*Buffer += sizeof(long long);
	return IPDR_OK;
}


IPDR_Status
ExporterProtocol_WriteTime( IPDR_Time Time, unsigned char** Buffer )
{
	memcpy(*Buffer, &Time, sizeof(IPDR_Time) );
	*Buffer += sizeof(IPDR_Time);
	return IPDR_OK;
}

IPDR_Status
ExporterProtocol_WriteUF8String( const char* String, unsigned char** Buffer )
{
	size_t			StringLength ;
	unsigned char*	WriteBuffer ;

	WriteBuffer = *Buffer ;

	if( String == NULL )
		StringLength = 0 ;
	else
		StringLength = strlen(String);

	ExporterProtocol_WriteInt(htonl(StringLength), &WriteBuffer);
	if( StringLength > 0 )
	{
		memcpy( WriteBuffer, String, StringLength );
		WriteBuffer += StringLength ;
	}
	*Buffer = WriteBuffer ;
	return IPDR_OK;
}

/**>
 *	struct FieldDescriptor{
 * 		int typeId; 	/ ID of Field Type /
 *		int fieldId; 	/ unqualified Field code that can be used /
 *			 			/ as alternative accessor handles to Fields /
 *		UTF8String fieldName; 	/ Note that Field names are to be qualified /
 *								/ with the Namespace name, as an example: /
 *								/ http://www.ipdr.org/namespace:movieId /
 *								/ The namespace MUST match one of those /
 *								/ targeted by the schema or schema imports /
 *		boolean isEnabled 		/ true=enabled Field false=disabled Field /
 *	};
 *
 *
*/
IPDR_Status
ExporterProtocol_WriteFieldDescriptorBlock( Key* K, unsigned char** Buffer)
{
	unsigned char*	WriteBuffer ;
	IPDR_Status		Status = IPDR_OK;
	unsigned char*	pEnabled ;
	WriteBuffer = *Buffer ;

	ExporterProtocol_WriteInt(htonl(K->KeyTypeID &~IPDR_TYPE_VAR_LENGTH), &WriteBuffer);
	ExporterProtocol_WriteInt(htonl(K->KeyID), &WriteBuffer);

	Status = ExporterProtocol_WriteUF8String( K->Name, &WriteBuffer );
	if( FAILURE(Status) )
	{
		return Status ;
	}

	pEnabled = (unsigned char*)WriteBuffer ;
	*pEnabled = K->Enabled ? 1 : 0 ;
	WriteBuffer += sizeof(unsigned char);

	*Buffer = WriteBuffer;

	return IPDR_OK;
}

/**<
 *	struct TemplateBlock{
 *		short templateId; 	/ ID of Template - context within configId /
 *							/ Provides numeric Identifier to /
 * 							/ IPDR service specification for context of /
 *							/ Session/config /
 *		UTF8String schemaName; / Reference to IPDR service specification /
 *		UTF8String typeName; / Reference to IPDR service specification /
 *		FieldDescriptor fields<>; / Fields in this Template /
 * 	}
*/
IPDR_Status
ExporterProtocol_WriteTemplateBlock( Template *T, unsigned char** Buffer)
{
	int				Index ;
	unsigned char*	WriteBuffer ;
	IPDR_Status		Status = IPDR_OK;

	WriteBuffer = *Buffer;

	ExporterProtocol_WriteShort(htons(T->TemplateID), &WriteBuffer);

	Status = ExporterProtocol_WriteUF8String( T->SchemaName, &WriteBuffer );
	if( FAILURE(Status) )
	{
		return Status ;
	}

	Status = ExporterProtocol_WriteUF8String( T->TypeName, &WriteBuffer );
	if( FAILURE(Status) )
	{
		return Status ;
	}

	ExporterProtocol_WriteInt(htonl(T->Keys.Length), &WriteBuffer);

	for( Index = 0 ; Index < T->Keys.Length ; Index++ )
	{
		Status = ExporterProtocol_WriteFieldDescriptorBlock( T->Keys.Data[Index], &WriteBuffer );
		if( FAILURE(Status) )
		{
			break ;
		}
	}
	*Buffer = WriteBuffer;
	return Status ;
}

/**<
 *	struct TemplateData
 *	{
 *	 struct IPDRStreamingHeader header;
 *	 short configId; / Identifies context of Template definitions /
 *					 / Changes in Template MUST use a different /
 *					 / configId (0 if unused) /
 *   char flags; / LSB 0=NN 1=N; rest of bits Unused (reserved)/
 *	 TemplateBlock templates<>; / Definitions of Templates supported /
 *	};

*/
IPDR_Status
ExporterProtocol_WriteTemplateData( TransmissionContext *TC, unsigned char *Buffer, IPDR_BOOL Negotiable)
{
	int 					Index ;
	unsigned char* 			WriteBuffer ;
	IPDR_Status 			Status = IPDR_OK;
	TmplDataMessage*	TDHeader ;

	TDHeader = (TmplDataMessage *)Buffer;

	IPDR_SET_HTONS(TDHeader->ConfigurationID, TC->ConfigurationID);
	TDHeader->Flags = Negotiable ? 1 : 0 ;
	IPDR_SET_HTONL(TDHeader->NumTemplates, TC->Templates.Length);
	WriteBuffer = Buffer + sizeof( TmplDataMessage ) ;

	for( Index = 0 ; Index < TC->Templates.Length ; Index++ )
	{
		Status = ExporterProtocol_WriteTemplateBlock( TC->Templates.Data[Index], &WriteBuffer );
		if( FAILURE(Status) )
		{
			break ;
		}
	}
	return Status ;
}

IPDR_Status
ExporterProtocol_ReadTemplateByte(Session* S, unsigned char* Byte, unsigned char** Buffer, int* LengthLeft)
{
	Exporter *E = S->ExporterPtr;

	if( *LengthLeft < sizeof(unsigned char) )
	{
		Log_Write3(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
			  "ExporterProtocol_ReadTemplateInt",
			  MSG_EXPORTER_READ_TEMPLATE_INVALID_VALUE,"unsigned char",sizeof(unsigned char),*LengthLeft);
		return IPDR_ERR_INVALID_VALUE ;
	}
	memcpy( Byte, *Buffer, sizeof(unsigned char) );
	*Buffer += sizeof(unsigned char);
	*LengthLeft -= sizeof(unsigned char);
	return IPDR_OK;
}


IPDR_Status
ExporterProtocol_ReadTemplateShort(Session* S, short* Short, unsigned char** Buffer, int* LengthLeft)
{
	Exporter *E = S->ExporterPtr;
	short nShort;

	if( *LengthLeft < sizeof(short) )
	{
		Log_Write3(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
			  "ExporterProtocol_ReadTemplateShort",
			  MSG_EXPORTER_READ_TEMPLATE_INVALID_VALUE,"short",sizeof(short),*LengthLeft);
		return IPDR_ERR_INVALID_VALUE ;
	}
	memcpy( &nShort, *Buffer, sizeof(short) );
	*Buffer += sizeof(short);
	*LengthLeft -= sizeof(short);
	*Short = ntohs(nShort);
	return IPDR_OK;
}

IPDR_Status
ExporterProtocol_ReadTemplateInt(Session* S, int* Int, unsigned char** Buffer, int* LengthLeft)
{
	int nInt;
	Exporter *E = S->ExporterPtr;

	if( *LengthLeft < sizeof(int) )
	{
		Log_Write3(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
			  "ExporterProtocol_ReadTemplateInt",
			  MSG_EXPORTER_READ_TEMPLATE_INVALID_VALUE,"int",sizeof(int),*LengthLeft);
		return IPDR_ERR_INVALID_VALUE ;
	}
	memcpy( &nInt, *Buffer, sizeof(int) );
	*Buffer += sizeof(int);
	*LengthLeft -= sizeof(int);
	*Int = ntohl(nInt);
	return IPDR_OK;
}


/**
 * Reads a string from a template.
 *
 * @param S session handle (pointer)
 * @param String will hold the string read from the template
 * @param Buffer the source of the string
 * @param LengthLeft determnes the size left in the buffer.
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
*/
IPDR_Status
ExporterProtocol_ReadTemplateString(Session* S, char** String, unsigned char** Buffer, int* LengthLeft)
{
	int			StringLen;
	IPDR_Status Status = IPDR_OK ;
	Exporter* 	E = S->ExporterPtr;

	Status = ExporterProtocol_ReadTemplateInt(S, &StringLen, Buffer, LengthLeft);

	if( StringLen == 0 )
	{
		*String = NULL ;
		return IPDR_OK;
	}
	if( *LengthLeft < StringLen  )
	{
		Log_Write3(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
			  "ExporterProtocol_ReadTemplateString",
			  MSG_EXPORTER_READ_TEMPLATE_INVALID_VALUE,"int",sizeof(int),*LengthLeft);
		return IPDR_ERR_INVALID_VALUE ;
	}

	Status = Object_Allocate(SESSION_PLATFORM, StringLen+1, NULL, (void **)String);
	if( FAILURE(Status) )
	{
		return Status ;
	}
	memcpy( *String, *Buffer, StringLen );
	*Buffer+= StringLen;
	*LengthLeft-=StringLen;
	(*String)[StringLen] = '\0' ;

	return IPDR_OK;
}

/**
 * reads the fields data from the template.
 *
 * @param S session handle (pointer)
 * @param T the template that holds the block to be read
 * @param Buffer the source of the block
 * @param LengthLeft determnes the size left in the buffer.
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
*/
IPDR_Status
ExporterProtocol_ReadFieldDescriptorBlock(Session* S, Template* T, unsigned char** Buffer, int *LengthLeft)
{
	int						TypeID ;
	int						FieldID ;
	char*					FieldName;
	unsigned char			Enabled;
	IPDR_Status 			Status = IPDR_OK ;

	Status = ExporterProtocol_ReadTemplateInt(S, &TypeID, Buffer, LengthLeft);
	if( FAILURE(Status) )
	{
		return Status;
	}

	Status = ExporterProtocol_ReadTemplateInt(S, &FieldID, Buffer, LengthLeft);
	if( FAILURE(Status) )
	{
		return Status;
	}

	Status = ExporterProtocol_ReadTemplateString(S, &FieldName, Buffer, LengthLeft);
	if( FAILURE(Status) )
	{
		return Status;
	}

	Status = ExporterProtocol_ReadTemplateByte(S, &Enabled, Buffer, LengthLeft);
	if( FAILURE(Status) )
	{
		Object_Release(SESSION_PLATFORM, FieldName);
		return Status;
	}
	Status = Template_AddKey(S, T, FieldID, TypeID, FieldName, NULL, NULL, Enabled, 0, NULL, NULL);
	if( FAILURE(Status) )
	{
		Object_Release(SESSION_PLATFORM, FieldName);
		return Status;
	}
	Object_Release(SESSION_PLATFORM, FieldName);
	return Status;
}
/**
 * reads all data from the template.
 *
 * @param S session handle (pointer)
 * @param T the template that holds the block to be read
 * @param Buffer the source of the block
 * @param LengthLeft determnes the size left in the buffer.
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
*/
IPDR_Status
ExporterProtocol_ReadTemplateBlock(Session* S, Template** T, unsigned char** Buffer, int *LengthLeft)
{
	int 					Index ;
	short					TemplateId ;
	char*					SchemaName;
	char*					TypeName;
	int						NumFields ;
	IPDR_Status 			Status = IPDR_OK ;
	Exporter* E = S->ExporterPtr;

	Status = ExporterProtocol_ReadTemplateShort(S, &TemplateId, Buffer, LengthLeft);
	if( FAILURE(Status) )
	{
		return Status;
	}

	Status = ExporterProtocol_ReadTemplateString(S, &SchemaName, Buffer, LengthLeft);
	if( FAILURE(Status) )
	{
		return Status;
	}

	Status = ExporterProtocol_ReadTemplateString(S, &TypeName, Buffer, LengthLeft);
	if( FAILURE(Status) )
	{
		Object_Release(SESSION_PLATFORM, SchemaName);
		return Status;
	}

	Status = ExporterProtocol_ReadTemplateInt(S, &NumFields, Buffer, LengthLeft);
	if( FAILURE(Status) )
	{
		Object_Release(SESSION_PLATFORM, SchemaName);
		Object_Release(SESSION_PLATFORM, TypeName);
		return Status;
	}


	Status = Template_Create(S, TemplateId, SchemaName, TypeName, 0, IPDR_FALSE, T);
	if( FAILURE(Status) )
	{
		Object_Release(SESSION_PLATFORM, SchemaName);
		Object_Release(SESSION_PLATFORM, TypeName);
		return Status;
	}
	Object_Release(SESSION_PLATFORM, SchemaName);
	Object_Release(SESSION_PLATFORM, TypeName);

	for( Index = 0 ; Index  < NumFields ; Index++ )
	{
		Status = ExporterProtocol_ReadFieldDescriptorBlock(S, *T, Buffer, LengthLeft);
		if( FAILURE(Status) )
		{
			Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO,
				  "ExporterProtocol_ReadTemplateBlock", MSG_EXPORTER_READ_FIELD_DESCRIPTOR_FAILED);
			Object_Release(SESSION_PLATFORM, *T);
			break ;
		}
	}
	return Status ;
}


/**
 * Sends a TEMPLATE DATA message to a collector.
 *
 * @param E exporter handle
 * @param S session handle (pointer)
 * @param Coll collector to which to send the message
 * @param Conn connection between Coll and S
 * @param Negotiable true if the template is negotiable
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status ExporterProtocol_SendTemplateData(Exporter *E,
											Session *S,
											Collector *Coll,
											Connection Conn,
											IPDR_BOOL Negotiable)
{
	size_t 				Size;
    BufferHandle 		BH;
    IPDR_Status 		Status;
	TransmissionContext* TC = S->ActiveContext;
    unsigned int 		NumTemplates;
    unsigned char 		MsgID;
    ChunksManager*		CMgr;
	IPDRConnection*		IC = Conn->Owner;
	CollectorSession*	CS = NULL;

	Status = ExporterProtocol_CollectorSession(Coll, S, &CS, NULL, IPDR_TRUE);

	ExporterProtocol_KeepAliveOutTimerReset(IC);


    if (Conn == NULL || IC->State == IPDR_CONNECTION_DISCONNECTED || IC->State == IPDR_CONNECTION_WAITRESPONSE)
	{
        /* XXX should signal error event */
        Log_Write1(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
			   "ExporterProtocol_SendTmplData", MSG_EXPORTER_NOT_READY_SEND2,  TL_GetConnectionInfoStr(Conn));
        return IPDR_ERR_NO_ACTIVE_COLLECTOR;
    }

	if(CS == NULL)
	{
		Log_Write2(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
			   "ExporterProtocol_SendTmplData", MSG_EXPORTER_NO_SESSIONS,
			   TL_GetConnectionInfoStr(Conn),  TL_GetConnectionInfoStr(Conn));
		return IPDR_ERR_UNABLE_TO_FIND_SESSION ;
	}

    /* calculate the size of the message */

	CS->Negotiable = (S->Negotiable & Negotiable & Coll->Capabilities.TemplateNegotiation);

    DynamicArray_CountObjects(EXPORTER_PLATFORM, & (TC->Templates),
                              & NumTemplates);

    Size = ExporterProtocol_GetTemplateDataMsgLen(TC);

    /* allocate a buffer for the message */
    TL_GetSendChunksManager(Conn, & CMgr);
    Status = ChunksManager_AllocateBuffer(CMgr, Size, & BH);
    if (FAILURE(Status))
	{
		Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
		   	   "ExporterProtocol_SendTemplateData", MSG_EXPORTER_CHUNKS_MANAGER_FAILED);
        return Status;
	}


    MsgID = IPDR_MSG_TEMPLATE_DATA;
    ExporterProtocol_FillHeader(S, BH.Data, MsgID, Size);

	Status = ExporterProtocol_WriteTemplateData(TC, BH.Data, CS->Negotiable);
    if (FAILURE(Status))
	{
		/*NOTE: ExporterProtocol_WriteTemplateData always returns IPDR_OK !!! */
        	return Status;
	}

    if (Log_ShouldWrite(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO)) {
        ProtocolDebug_PrintOutgoingMessage(EXPORTER_LOG, Conn, BH.Data, IPDR_LEVEL_INFO);
    }

    Status = TL_Send(Conn, & BH, IPDR_TRUE);
	if(FAILURE(Status))
	{
		return Status;
	}

	if(CS->TmplDataTimer != NULL)
		Platform_UnregisterTimerAndClear(EXPORTER_PLATFORM, &CS->TmplDataTimer);

	Platform_RegisterTimer(EXPORTER_PLATFORM,
    						ExporterProtocol_TmplDataTimerCallback,
							CS,
                            E->Parameters.TemplateAckTimeout,
                            & (CS->TmplDataTimer));

	return Status;
}

IPDR_Status ExporterProtocol_SendModifyTemplateResponse(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	Connection Conn)
{
	size_t 				Size;
    BufferHandle 		BH;
    IPDR_Status 		Status;
	TransmissionContext* TC = S->ActiveContext;
    unsigned int 		NumTemplates;
    unsigned char 		MsgID;
    ChunksManager*		CMgr;
	IPDRConnection*		IC = Conn->Owner;
	CollectorSession*	CS = NULL;

	Status = ExporterProtocol_CollectorSession(Coll, S, &CS, NULL, IPDR_TRUE);

	ExporterProtocol_KeepAliveOutTimerReset(IC);


    if (Conn == NULL || IC->State == IPDR_CONNECTION_DISCONNECTED || IC->State == IPDR_CONNECTION_WAITRESPONSE)
	{
        /* XXX should signal error event */
        Log_Write1(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
			   "ExporterProtocol_SendModifyTemplateResponse", MSG_EXPORTER_NOT_READY_SEND2,
			   TL_GetConnectionInfoStr(Conn));
			   return IPDR_ERR_NO_ACTIVE_COLLECTOR;
    }

	if(CS == NULL)
	{
		Log_Write2(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
			   "ExporterProtocol_SendModifyTemplateResponse", MSG_EXPORTER_NO_SESSIONS,
			   TL_GetConnectionInfoStr(Conn),  TL_GetConnectionInfoStr(Conn));
		return IPDR_ERR_UNABLE_TO_FIND_SESSION ;
	}

    /* calculate the size of the message */

    DynamicArray_CountObjects(EXPORTER_PLATFORM, & (TC->Templates),
                              & NumTemplates);

    Size = ExporterProtocol_GetTemplateDataMsgLen(TC);

    /* allocate a buffer for the message */
    TL_GetSendChunksManager(Conn, & CMgr);
    Status = ChunksManager_AllocateBuffer(CMgr, Size, & BH);
    if (FAILURE(Status))
	{
		Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
		   	   "ExporterProtocol_SendModifyTemplateResponse", MSG_EXPORTER_CHUNKS_MANAGER_FAILED);
        return Status;
	}


    MsgID = IPDR_MSG_MODIFY_TEMPLATE_RESPONSE;
    ExporterProtocol_FillHeader(S, BH.Data, MsgID, Size);

	Status = ExporterProtocol_WriteTemplateData(TC, BH.Data, CS->Negotiable);
    if (FAILURE(Status))
	{
		/*NOTE: ExporterProtocol_WriteTemplateData always returns IPDR_OK !!! */
        	return Status;
	}

    if (Log_ShouldWrite(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO)) {
        ProtocolDebug_PrintOutgoingMessage(EXPORTER_LOG, Conn, BH.Data, IPDR_LEVEL_INFO);
    }

    Status = TL_Send(Conn, & BH, IPDR_TRUE);
	if(FAILURE(Status))
	{
		return Status;
	}

	if(CS->TmplDataTimer != NULL)
		Platform_UnregisterTimerAndClear(EXPORTER_PLATFORM, &CS->TmplDataTimer);

	Platform_RegisterTimer(EXPORTER_PLATFORM,
    						ExporterProtocol_TmplDataTimerCallback,
							CS,
                            E->Parameters.TemplateAckTimeout,
                            & (CS->TmplDataTimer));
    return Status;
}

IPDR_Status ExporterProtocol_SendStartNegotiationReject(Exporter *E,Session *S,Collector *Coll,Connection Conn)
{
	IPDR_Status 	Status;
	BufferHandle 	BH;
	ChunksManager*	CMgr;
	IPDRConnection*	IC = Conn->Owner;

	if (IC->State != IPDR_CONNECTION_CONNECTED) {
        /* XXX should signal error event */
        Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
                  "ExporterProtocol_SendStartNegotiationReject",
                  MSG_EXPORTER_CONNECTION_MUST_BE_SET);
        return IPDR_ERR;
    }

	ExporterProtocol_KeepAliveOutTimerReset(IC);

    TL_GetSendChunksManager(Conn, & CMgr);
    Status = ChunksManager_AllocateBuffer(CMgr,
                                          sizeof(MessageHeader),
										  & BH);
    if (FAILURE(Status))
	{
		Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
		   	  "ExporterProtocol_SendStartNegotiationReject", MSG_EXPORTER_CHUNKS_MANAGER_FAILED);
        	return Status;
	}

    ExporterProtocol_FillHeader(S, BH.Data, IPDR_MSG_START_NEGOTIATION_REJECT,
                              sizeof(MessageHeader));

    if (Log_ShouldWrite(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO)) {
        ProtocolDebug_PrintOutgoingMessage(EXPORTER_LOG, Conn, BH.Data, IPDR_LEVEL_INFO);
    }

    Status = TL_Send(Conn, & BH, IPDR_TRUE);
	if(FAILURE(Status))
	{
		Log_Write1(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO,
					"ExporterProtocol_SendStartNegotiationReject",
					MSG_EXPORTER_SEND_KEEPALIVE_FAILED, TL_GetConnectionInfoStr(Conn));
		return Status;
	}

	Log_Write1(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_DEBUG,
					"ExporterProtocol_SendStartNegotiationReject",
					MSG_EXPORTER_SEND_KEEPALIVE_TARGET, TL_GetConnectionInfoStr(Conn));
    return Status;
}

size_t
ExporterProtocol_GetSessionBlockDataLength( Session *S )
{
	size_t	Size = sizeof(SessionBlock) + strlen(S->Name) + strlen(S->Description);
	return Size ;
}

size_t
ExporterProtocol_GetSessionsResponseMsgLen(Collector *Coll, unsigned int *NumSessions)
{
	size_t			Size = sizeof( GetSessionsResponseMessage ) ;
	int				Index ;

	(*NumSessions) = 0;
	for( Index = 0 ; Index < Coll->CollectorSessions.Length ; Index++ )
	{
		CollectorSession *CS = (CollectorSession *)Coll->CollectorSessions.Data[Index];
		if(CS == NULL || CS->SessionPtr == NULL)
			continue;
		(*NumSessions)++;
		Size += ExporterProtocol_GetSessionBlockDataLength( CS->SessionPtr );
	}
	return Size ;
}

IPDR_Status ExporterProtocol_SendGetSessionsResponse(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	Connection Conn,
											int RequestID)
{
	size_t 				Size;
    BufferHandle 		BH;
    IPDR_Status 		Status;
	int					Index;
	unsigned int 		NumSessions;
    unsigned char 		MsgID = IPDR_MSG_GET_SESSIONS_RESPONSE;
	unsigned char 		*p1;
    ChunksManager*		CMgr;
	IPDRConnection*		IC = Conn->Owner;
	/*CollectorSession*	CS = NULL;*/
	GetSessionsResponseMessage	*GTR;

	/*Status = ExporterProtocol_CollectorSession(Coll, S, &CS, NULL, IPDR_TRUE);*/

	ExporterProtocol_KeepAliveOutTimerReset(IC);

    if (Conn == NULL || IC->State == IPDR_CONNECTION_DISCONNECTED ||
		IC->State == IPDR_CONNECTION_WAITRESPONSE)
	{
        /* XXX should signal error event */
        Log_Write1(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
                  "ExporterProtocol_SendGetSessionsResponse",
                  MSG_EXPORTER_NOT_READY_SEND2,  TL_GetConnectionInfoStr(Conn));
        return IPDR_ERR_NO_ACTIVE_COLLECTOR;
    }

	/*if(CS == NULL)
	{
		Log_Write2(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
					"ExporterProtocol_SendGetSessionsResponse",
					MSG_EXPORTER_NO_SESSIONS,  TL_GetConnectionInfoStr(Conn),
					TL_GetConnectionInfoStr(Conn));
		return IPDR_ERR_UNABLE_TO_FIND_SESSION ;
	}*/

	    /* calculate the size of the message */
	Size = ExporterProtocol_GetSessionsResponseMsgLen(Coll, & NumSessions);

	    /* allocate a buffer for the message */
    TL_GetSendChunksManager(Conn, & CMgr);
    Status = ChunksManager_AllocateBuffer(CMgr, Size, & BH);
    if (FAILURE(Status))
	{
    	Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
		   "ExporterProtocol_SendGetSessionsResponse", MSG_EXPORTER_CHUNKS_MANAGER_FAILED);
        return Status;
	}

    ExporterProtocol_FillHeader(S, BH.Data, MsgID, Size);

	GTR = (GetSessionsResponseMessage *)BH.Data;

	IPDR_SET_HTONS(GTR->RequestID, RequestID);
	IPDR_SET_HTONL(GTR->NumSessions, NumSessions);

	p1 = BH.Data + sizeof(GetSessionsResponseMessage);
	for( Index = 0 ; Index < Coll->CollectorSessions.Length ; Index++ )
	{
		unsigned int sl;
		ProtocolUnsigned32 nsl, ackTimeInterval, ackSequenceInterval;
		Session *SS = NULL ;
		CollectorSession *CS = (CollectorSession *)Coll->CollectorSessions.Data[Index];
		if(CS == NULL || CS->SessionPtr == NULL)
			continue;

		SS = CS->SessionPtr;
		memcpy(p1, &(SS->ID), sizeof(unsigned char));
		p1 += sizeof(unsigned char) * 2;
		sl = strlen(SS->Name);
		IPDR_SET_HTONL(nsl, sl);
		memcpy(p1, &nsl, sizeof(nsl));
		p1 += sizeof(nsl);
		memcpy(p1, SS->Name, sl);
		p1 += sl;
		sl = strlen(SS->Description);
		IPDR_SET_HTONL(nsl, sl);
		memcpy(p1, &nsl, sizeof(nsl));
		p1 += sizeof(nsl);
		memcpy(p1, SS->Description, sl);
		p1 += sl;
		IPDR_SET_HTONL(ackTimeInterval, E->Parameters.DataAckTimeInterval);
		IPDR_SET_HTONL(ackSequenceInterval, E->Parameters.DataAckSequenceInterval);
		memcpy(p1, &ackTimeInterval, sizeof(ackTimeInterval));
		p1 += sizeof(ackTimeInterval);
		memcpy(p1, &ackSequenceInterval, sizeof(ackSequenceInterval));
		p1 += sizeof(ackSequenceInterval);
	}

	if (Log_ShouldWrite(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO)) {
        ProtocolDebug_PrintOutgoingMessage(EXPORTER_LOG, Conn, BH.Data, IPDR_LEVEL_INFO);
    }

    Status = TL_Send(Conn, & BH, IPDR_TRUE);
	if(FAILURE(Status))
	{
		return Status;
	}
    return IPDR_OK;
}

size_t
ExporterProtocol_GetTemplatesResponseMsgLen(TransmissionContext *TC)
{
	size_t			Size ;
	int				Index ;

	Size = sizeof( GetTemplatesResponseMessage ) ;
	for( Index = 0 ; Index < TC->Templates.Length ; Index++ )
	{
		Size += ExporterProtocol_GetTemplateBlockDataLength( TC->Templates.Data[Index] );
	}
	return Size ;
}

IPDR_Status
ExporterProtocol_GetTemplates( TransmissionContext *TC, unsigned char *Buffer, int RequestID)
{
	int 					Index ;
	unsigned char* 			WriteBuffer ;
	IPDR_Status 			Status = IPDR_OK;
	GetTemplatesResponseMessage* TDHeader ;

	TDHeader = (GetTemplatesResponseMessage *)Buffer;

	IPDR_SET_HTONS(TDHeader->ConfigurationID, TC->ConfigurationID);
	IPDR_SET_HTONS(TDHeader->RequestID, RequestID);
	IPDR_SET_HTONL(TDHeader->NumTemplates, TC->Templates.Length);
	WriteBuffer = Buffer + sizeof( GetTemplatesResponseMessage ) ;

	for( Index = 0 ; Index < TC->Templates.Length ; Index++ )
	{
		Status = ExporterProtocol_WriteTemplateBlock( TC->Templates.Data[Index], &WriteBuffer );
		if( FAILURE(Status) )
		{
			break ;
		}
	}
	return Status ;
}

/**
 * Send a GET TMPL RSP message to a server .
 *
 * @param E exporter handle
 * @param S session handle (pointer)NumSessions
 * @param Coll collector
 * @param Conn connection
 * @param RequestID id associated with the reply.
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status ExporterProtocol_SendGetTemplatesResponse(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	Connection Conn,
											int RequestID)
{
	size_t 				Size;
    BufferHandle 		BH;
    IPDR_Status 		Status;
	TransmissionContext* TC = S->ActiveContext;
    unsigned int 		NumTemplates;
    unsigned char 		MsgID = IPDR_MSG_GET_TEMPLATES_RESPONSE;
    ChunksManager*		CMgr;
	IPDRConnection*		IC = Conn->Owner;
	/*CollectorSession*	CS = NULL;*/

	/*Status = ExporterProtocol_CollectorSession(Coll, S, &CS, NULL, IPDR_TRUE);*/
	if(S->Running != IPDR_TRUE)
		return IPDR_ERR_SESSION_LOGICALLY_STOPED;

	ExporterProtocol_KeepAliveOutTimerReset(IC);


    if (Conn == NULL || IC->State == IPDR_CONNECTION_DISCONNECTED || IC->State == IPDR_CONNECTION_WAITRESPONSE)
	{
        /* XXX should signal error event */
        Log_Write1(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
                  "ExporterProtocol_SendGetTemplatesResponse",
                  MSG_EXPORTER_NOT_READY_SEND2,  TL_GetConnectionInfoStr(Conn));
        return IPDR_ERR_NO_ACTIVE_COLLECTOR;
    }

	/*if(CS == NULL)
	{
		Log_Write2(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
					"ExporterProtocol_SendGetTemplatesResponse",
					MSG_EXPORTER_NO_SESSIONS,  TL_GetConnectionInfoStr(Conn),  TL_GetConnectionInfoStr(Conn));
		return IPDR_ERR_UNABLE_TO_FIND_SESSION ;
	}*/

    /* calculate the size of the message */

    DynamicArray_CountObjects(EXPORTER_PLATFORM, & (TC->Templates),
                              & NumTemplates);

    Size = ExporterProtocol_GetTemplatesResponseMsgLen(TC);

    /* allocate a buffer for the message */
    TL_GetSendChunksManager(Conn, & CMgr);
    Status = ChunksManager_AllocateBuffer(CMgr, Size, & BH);
    if (FAILURE(Status))
	{
    	Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
		   "ExporterProtocol_SendGetTemplatesResponse", MSG_EXPORTER_CHUNKS_MANAGER_FAILED);
        return Status;
	}

    ExporterProtocol_FillHeader(S, BH.Data, MsgID, Size);

	Status = ExporterProtocol_GetTemplates(TC, BH.Data, RequestID);
    if (FAILURE(Status))
	{
		/*NOTE: ExporterProtocol_WriteTemplateData always returns IPDR_OK !!! */
        return Status;
	}

    if (Log_ShouldWrite(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO)) {
        ProtocolDebug_PrintOutgoingMessage(EXPORTER_LOG, Conn, BH.Data, IPDR_LEVEL_INFO);
    }

    Status = TL_Send(Conn, & BH, IPDR_TRUE);
	if(FAILURE(Status))
	{
		return Status;
	}
    return IPDR_OK;
}

IPDR_Status ExporterProtocol_SendData(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	Connection Conn,
											IPDR_TemplateHandle TH,
											const void *Data)
{
	Template*	T ;
	CollectorSession *ACS = S->ActiveCS;
	IPDRConnection *IC = NULL;
	IPDR_Status Status = IPDR_OK;

    if (!E || !S || !Data) {
        return IPDR_ERR_INVALID_VALUE;
    }

	if(S->Running != IPDR_TRUE)
		return IPDR_ERR_SESSION_IS_NOT_RUNNING;

    if (! Platform_IsRunning(EXPORTER_PLATFORM)) {
        return IPDR_ERR_NOT_RUNNING;
    }

    Platform_Lock(EXPORTER_PLATFORM);

	if(	!S->ActiveContext ||
		S->ActiveContext->Templates.Length <= TH.i ||
		TH.s != S->ID)
	{
		/*TODO: add errorCode and message*/
		Platform_Unlock(EXPORTER_PLATFORM);
		return IPDR_ERR_INVALID_VALUE;
	}

	T = (Template *) S->ActiveContext->Templates.Data[TH.i];
    if (!T || ! T->Enabled)
	{
		Log_Write1(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_INFO,"IPDR_SendData",MSG_EXPORTER_DISABLED_TEMPLATE,T->TemplateID);
		Platform_Unlock(EXPORTER_PLATFORM);
		return IPDR_ERR_TEMPLATE_DISABLED;
		/* NOT_DONE: Add event??? DONE: New IPDR_ERR in IPDR.H ; New field in "IPDR_API IPDR_StatusMsg" ; New Msg in ExporterMsgs,h */
	}

	Status = EXPORTER_QHANDLER->SendData(E, S, T, Data);

	if(ACS != NULL)
		IC = ACS->SessionConnection;

	if(IC != NULL)
		Conn = IC->TCPConnection;

	ExporterProtocol_KeepAliveOutTimerReset(IC);

    if (FAILURE(Status)) {
		const char* serverStr;
        IPDR_EventData eventData;
        EventDataInit(&eventData, IPDR_EVENT_RECORD_LOST, E, ACS, S);

        if (ACS && IC && Conn && ACS->CSState == IPDR_CS_ACTIVE) {
			eventData.IC = NULL;
			serverStr = TL_GetConnectionInfoStr(Conn);
		} else {
			eventData.IC = IC;
			serverStr = "Server=none";
		}
		S->Statistics.LostRecords++;
		if (ACS) {
			ACS->Statistics.LostRecords++;
		}

    	Exporter_TriggerEvent(E, IPDR_LEVEL_ERROR, &eventData, NULL, /* don't record this event with Collector*/
        	                        MSG_EXPORTER_RECORD_LOST,
            	                    serverStr, S->DSN,
                	                EXPORTER_QHANDLER->OutstandingDataCount(E, S),
                    	            EXPORTER_QHANDLER->UnacknowledgedDataCount(E, S),
                        	        S->ID, S->Name, S->Description);

		Platform_Unlock(EXPORTER_PLATFORM);
		return Status;
	}

	S->Statistics.EnqueuedRecords++;
	if (ACS) {
		ACS->Statistics.EnqueuedRecords++;
	}
	IPDR_INCR_DSN(S->DSN);

	/* We ignore SendOutstandingData's return code because it just
       indicates that something went wrong whilst trying to send all
       the records on the queue and a server failover should fix
       things up. */
	(void)EXPORTER_QHANDLER->SendOutstandingData(E,S);
	Platform_Unlock(EXPORTER_PLATFORM);
	return Status;
}

/**
 * Handle an unknown message from a server (collector).
 *
 * @param E exporter handle
 * @param S session handle (pointer)
 * @param Coll collector originating the message
 * @param Msg received message
 * @param Conn connection
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
ExporterProtocol_HandleUnknownMsg(Exporter *E, Session *S, Collector *Coll, const MessageHeader *Msg, Connection Conn)
{
    char errorMsg[200];

    Log_Write2(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_WARNING,
               "ExporterProtocol_DataCallback",
               MSG_EXPORTER_UNKNOWN_MSG,
               Msg->MessageID, Msg->MessageID);

    sprintf(errorMsg, MSG_EXPORTER_UNKNOWN_MSG, Msg->MessageID, Msg->MessageID);
    ExporterProtocol_SendError(E, S, Coll, Conn, MessageDecodeError, errorMsg);

    return IPDR_OK;
}

/**
 * Tests is a message requires SessionID to be sent
 *
 * @param MessageID received message ID
 *
 * @return #IPDR_TRUE if so; #IPDR_FALSE otherwise.
 */

static
IPDR_BOOL ExporterProtocol_NeedSessionID(unsigned char MessageID)
{
    switch (MessageID) {

    case IPDR_MSG_FLOW_START:				return IPDR_TRUE;
    case IPDR_MSG_FLOW_STOP:				return IPDR_TRUE;
    case IPDR_MSG_MODIFY_TEMPLATE:			return IPDR_TRUE;
    case IPDR_MSG_FINAL_TEMPLATE_DATA_ACK:	return IPDR_TRUE;
    case IPDR_MSG_START_NEGOTIATION:		return IPDR_TRUE;
    case IPDR_MSG_GET_SESSIONS:				return IPDR_FALSE;
    case IPDR_MSG_GET_TEMPLATES:			return IPDR_TRUE;
    case IPDR_MSG_DATA_ACK:					return IPDR_TRUE;

    case IPDR_MSG_CONNECT:					return IPDR_FALSE;
    case IPDR_MSG_CONNECT_RESPONSE:			return IPDR_FALSE;
    case IPDR_MSG_DISCONNECT:				return IPDR_FALSE;
    case IPDR_MSG_ERROR:					return IPDR_FALSE;
    case IPDR_MSG_KEEP_ALIVE:				return IPDR_FALSE;

    case IPDR_MSG_SESSION_START:			return IPDR_TRUE;
    case IPDR_MSG_SESSION_STOP:				return IPDR_TRUE;
    case IPDR_MSG_TEMPLATE_DATA:			return IPDR_TRUE;
    case IPDR_MSG_MODIFY_TEMPLATE_RESPONSE:	return IPDR_TRUE;
    case IPDR_MSG_START_NEGOTIATION_REJECT:	return IPDR_TRUE;
    case IPDR_MSG_GET_SESSIONS_RESPONSE:	return IPDR_FALSE;
    case IPDR_MSG_GET_TEMPLATES_RESPONSE:	return IPDR_FALSE;
    case IPDR_MSG_DATA:						return IPDR_TRUE;
    }
    return IPDR_FALSE;
}


void
ExporterProtocol_KeepAliveInTimerReset(IPDRConnection * IC)
{
	Collector *Coll = NULL;
	Exporter * E = NULL;

	if(IC == NULL)
		return ;

	Coll = IC->Owner;
	E = IC->ExporterPtr;

	Platform_Lock(EXPORTER_PLATFORM);

	if(IC->KeepAliveINTimerID != NULL)
	{
		Platform_UnregisterTimerAndClear(EXPORTER_PLATFORM, &IC->KeepAliveINTimerID);
		IC->KeepAliveINTimerID = NULL;
	}
	if(IC->State == IPDR_CONNECTION_CONNECTED)
	{
		Platform_RegisterPeriodicTimer(EXPORTER_PLATFORM,
								ExporterProtocol_KeepAliveInTimerCallback,
								IC,
								(E->Parameters.KeepAliveInterval)+500,
								& (IC->KeepAliveINTimerID));
	}
	Platform_Unlock(EXPORTER_PLATFORM);
}

void
ExporterProtocol_KeepAliveOutTimerReset(IPDRConnection * IC)
{
	Collector *Coll = NULL;
	Exporter * E = NULL;

	if(IC == NULL)
		return ;

	Coll = IC->Owner;
	E = IC->ExporterPtr;

	Platform_Lock(EXPORTER_PLATFORM);

	if(IC->KeepAliveOUTTimerID != NULL)
	{
		Platform_UnregisterTimerAndClear(EXPORTER_PLATFORM, &IC->KeepAliveOUTTimerID);
		IC->KeepAliveOUTTimerID = NULL;
	}

	if(IC->State == IPDR_CONNECTION_CONNECTED)
	{
		Platform_RegisterPeriodicTimer(EXPORTER_PLATFORM,
								ExporterProtocol_KeepAliveOutTimerCallback,
								IC,
								((IC->RemoteKeepAliveInterval)*1000)-500,
								& (IC->KeepAliveOUTTimerID));
	}
	Platform_Unlock(EXPORTER_PLATFORM);
}

static void
ExporterProtocol_KeepAliveInTimerCallback(void *Arg)
{
	IPDRConnection * IC = (IPDRConnection *)Arg;
	Collector *Coll = IC->Owner;
	Exporter * E = IC->ExporterPtr;
	Connection Conn = IC->TCPConnection;

	if(IC == NULL)
		return ;

	if(! IC->TCPConnection || ! Coll || ! E)
	{
		return ;
	}

	Platform_Lock(EXPORTER_PLATFORM);

	Log_Write(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_ERROR,
               "ExporterProtocol_KeepAliveInTimerCallback",
               MSG_EXPORTER_NO_KEEPALIVE_RECEIVED);

	ExporterProtocol_SendError(E, NULL, Coll, Conn, KeepAliveExpired, MSG_EXPORTER_NO_KEEPALIVE_RECEIVED);

	TL_HandleDisconnection(IC->TCPConnection);

	Platform_Unlock(EXPORTER_PLATFORM);
}

static void
ExporterProtocol_KeepAliveOutTimerCallback(void *Arg)
{
	IPDRConnection * IC = (IPDRConnection *)Arg;
	Collector *Coll = IC->Owner;
	Exporter * E = IC->ExporterPtr;

	if(IC == NULL)
		return ;

	if(! IC->TCPConnection || ! Coll || ! E)
	{
		return ;
	}

	Platform_Lock(EXPORTER_PLATFORM);

	/*TODO: print info message, message mus be #define XXXXXX "xxxxxx" */
	/*NOTE: about what??? */
	ExporterProtocol_SendKeepAlive(E, NULL, Coll, IC->TCPConnection);

	Platform_Unlock(EXPORTER_PLATFORM);
}
/**
 * Callback triggered in when ConnectResponse did not
 * received in time
 * @param Arg callback argument (IPDRConnection)
*/
static void
ExporterProtocol_ConnectResponseTimeoutCallback(void *Arg)
{
	IPDRConnection *IC = (IPDRConnection *)Arg ;
	Collector *Coll = (Collector *)IC->Owner;
	Exporter *E = Coll->ExporterPtr;

	Platform_Lock(EXPORTER_PLATFORM);

	Log_Write1(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_ERROR, "ExporterProtocol_ConnectResponseTimeoutCallback",
				MSG_EXPORTER_NO_CONNECT_RESPONSE, Coll->Name);

	if( IC->TCPConnection == NULL )
	{
		Log_Write1(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_ERROR, "ExporterProtocol_ConnectResponseTimeoutCallback",
				MSG_EXPORTER__RESET_TIMER_NEEDED, Coll->Name);
		Platform_Unlock(EXPORTER_PLATFORM);
		return;
	}

	ExporterProtocol_HandleDisconnect(E, NULL, Coll, NULL, IC->TCPConnection);

	Platform_Unlock(EXPORTER_PLATFORM);
}

/**
 * Callback triggered in when a flow sart event is
 * received from a collector
 * @param Arg callback argument (handle to a CollectorSession)
*/
static void
ExporterProtocol_ConnectWaitWaitBestCollectorCallback(void *Arg)
{
	CollectorSession* CS = (CollectorSession* )Arg;
	Exporter* E = CS->Collector->ExporterPtr;
	Session *S = CS->SessionPtr;

	if(S->waitBestCollectorTimerID != NULL)
		Platform_UnregisterTimerAndClear(EXPORTER_PLATFORM, &S->waitBestCollectorTimerID);

	ExporterProtocol_SelectCollector( E, CS->SessionPtr, IPDR_FALSE, NULL);
	return ;
}

/**
 * Callback triggered in when a collector did not responde to
 * to a template data related event send from the exporter
 * @param Arg callback argument (handle to a CollectorSession)
*/
static void
ExporterProtocol_TmplDataTimerCallback(void *Arg)
{
    CollectorSession *CS = (CollectorSession *) Arg;
    Session *S = NULL;
	Collector *Coll = NULL;
    IPDRConnection *IC = NULL;
	Exporter *E = NULL;
	Connection Conn = NULL;
	IPDR_Status Status ;

	if(CS)
	{
		if(CS->SessionPtr && CS->ExporterPtr &&
			CS->Collector && CS->SessionConnection &&
			CS->SessionConnection->TCPConnection)
		{
			E = CS->ExporterPtr;
			S = CS->SessionPtr;
			Coll = CS->Collector;
			IC = CS->SessionConnection;
			Conn = IC->TCPConnection;

			Status = ExporterProtocol_SendError(E, S, Coll, Conn, TemplateDataAckExpired, "");
			if(FAILURE(Status))
			{
				Log_Write1(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_WARNING,
						   "ExporterProtocol_TmplDataTimerCallback",
						   MSG_EXPORTER_ERROR_SEND_FAILED, TL_GetConnectionInfoStr(Conn));

				TL_HandleDisconnection(Conn);
			}

			Log_Write2(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_WARNING,
						   "ExporterProtocol_TmplDataTimerCallback",
						   MSG_EXPORTER_NO_TEMPLATEDATA_ACK, S->ID, TL_GetConnectionInfoStr(Conn));

			CS->CSState = IPDR_CS_DISCONNECTED ;

			if(S->ActiveCS == CS)
			{
				ExporterProtocol_SelectCollector( E, S, IPDR_FALSE, NULL );
			}
		}
	}
}

/**
 * Performs a selection of a new collector from the session
 * @param E exporter handle
 * @param S the session from which the new collector is selected
 * @param forceNegotiate TRUE if a negotation must accure
 * @param forceCS is not NULL if the specified collector must be the selected one
*/
IPDR_Status
ExporterProtocol_SelectCollector( Exporter* E, Session* S, IPDR_BOOL forceNegotiate, CollectorSession *forceCS)
{
	int i;
	IPDR_Status Status;
	IPDR_BOOL needsRollback;
    CollectorSession *OldCS;
    CollectorSession *SelectedCS = NULL;

    if (S == NULL) {
        return IPDR_OK; /* Can happen when shutting everything down. */
    }

	Platform_Lock(EXPORTER_PLATFORM);

    OldCS = S->ActiveCS;

    S->ActiveCS = NULL;

	if(forceCS == NULL)
	{
		for (i = 0; i < S->CollectorSessions.Length; i++) {
			CollectorSession *CS = (CollectorSession *) S->CollectorSessions.Data[i];
			switch (CS->CSState) {
			case IPDR_CS_INITIATION:
			case IPDR_CS_READY:
			case IPDR_CS_ACTIVE:
				if (SelectedCS == NULL || CS->Priority < SelectedCS->Priority)
				{
					SelectedCS = CS;
				}
				break;
			default:
				break;
			}
		}
	}
	else
	{
		SelectedCS = forceCS;
	}

    if (SelectedCS == NULL)
	{
		/* if we have no HIGH and our state collector  */
        IPDR_EventData eventData;
        if (OldCS != NULL && OldCS->WindowSizeTimer != NULL)
		{
            Platform_UnregisterTimerAndClear(EXPORTER_PLATFORM,
                                             &OldCS->WindowSizeTimer);
            if (OldCS->SessionConnection != NULL && OldCS->SessionConnection->TCPConnection != NULL)
			{
                TL_ResetConnection(OldCS->SessionConnection->TCPConnection);
				/* See comment below for TCP_ResetConnection */
            }
        }
        EventDataInit(&eventData, IPDR_EVENT_ALL_SERVERS_DISCONNECTED, E, SelectedCS, S);
        Exporter_TriggerEvent(E, IPDR_LEVEL_INFO, &eventData, NULL, "All collectors disconnected");
    }
	else if (SelectedCS != OldCS)
	{
		/* if we have one to negotiate with TODO: WindowSizeTimer */
        if (OldCS != NULL && OldCS->WindowSizeTimer != NULL)
		{
            Platform_UnregisterTimerAndClear(EXPORTER_PLATFORM,
                                             &OldCS->WindowSizeTimer);
        }

        if (OldCS != NULL &&
			OldCS->SessionConnection != NULL &&
			OldCS->SessionConnection->TCPConnection != NULL)
		{
			/*if we choose not currently active ecollector (Active Collector changed)*/
            IPDR_IPAddress OldAddr;
            unsigned short OldPort;
            IPDR_EventData eventData;
            TL_PABuf paBuf;
            EventDataInit(&eventData, IPDR_EVENT_ACTIVE_SERVER_CHANGED, E, SelectedCS, S);
			/* If an active collector didn't exist NULL; Otherwize, use previous active collector*/
			if (OldCS != NULL && OldCS->SessionConnection != NULL)
				eventData.Extra.ServerChanged.PreviousCollector = OldCS->SessionConnection;
			else
				eventData.Extra.ServerChanged.PreviousCollector = NULL;

            TL_GetConnectionInfo(OldCS->SessionConnection->TCPConnection, & OldAddr, & OldPort);
            Exporter_TriggerEvent(E, IPDR_LEVEL_WARNING, &eventData, SelectedCS,
								MSG_EXPORTER_ACTIVE_COLLECTOR_CHANGED,
                                TL_PrintAddressAndPort(OldAddr, OldPort, paBuf),
                                TL_GetConnectionInfoStr(OldCS->SessionConnection->TCPConnection),
                                EXPORTER_QHANDLER->OutstandingDataCount(E, S),
                                EXPORTER_QHANDLER->UnacknowledgedDataCount(E, S),
                                S->ID, S->Name, S->Description);

			/* Active collector has changed, reset the previous connection
			(Empties any internal buffers used for sending (outgoing) data and unregister+clear socket)
			since it might try to send obsolete data later */
            Status = TL_ResetConnection(OldCS->SessionConnection->TCPConnection);
			if(FAILURE(Status))
			{
				Log_StateTransition(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO,
                                "ExporterProtocol_SelectCollector", OldCS->SessionConnection->TCPConnection,
                                OldCS->CSState, IPDR_CS_DISCONNECTED,
                                MSG_EXPORTER_DISCONNECT_TRANSPORT1);
				ExporterProtocol_CloseTLConnection(E,OldCS->SessionConnection);
				Platform_Unlock(EXPORTER_PLATFORM);
				return IPDR_OK;
			}
			/* if we already have an active collector and it's not the same as the new one, stop session*/
			Status = ExporterProtocol_SendSessionStop(E, S, OldCS->Collector,
								  OldCS->SessionConnection->TCPConnection,
								  IPDR_STOP_REASON_SWITCHCOLLECTOR,
								  MSG_EXPORTER_NEW_ACTIVE_COLLECTOR);
			if(FAILURE(Status))
			{
				Log_StateTransition(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO,
                                "ExporterProtocol_SelectCollector", OldCS->SessionConnection->TCPConnection,
                                OldCS->CSState, IPDR_CS_DISCONNECTED,
                                MSG_EXPORTER_DISCONNECT_TRANSPORT2);
				ExporterProtocol_CloseTLConnection(E,OldCS->SessionConnection);
				Platform_Unlock(EXPORTER_PLATFORM);
				return IPDR_OK;
			}
			OldCS->CSState = IPDR_CS_READY;
        }

        switch (SelectedCS->CSState) {
            case IPDR_CS_READY:
                /* There are two cases when a server (collector) is un-paused:
                   - the same server (collector) is being re-selected (OldSrv == NULL)
                   - a new (higher priority) server (collector) is being selected (OldSrv != NULL)

                   In theory, there shouldn't be any problem with doing a
                   rollback in all cases (although it would be less
                   efficient); in practice, there are subtle interactions
                   between the queue and the list of buffers to be sent (kept
                   by the transport layer).

                   A rollback assumes that the output is freshly
                   connected. But that's not true in the case of a "paused"
                   server (collector) ... the connection is still connected. We could
                   disconnect it --- but then it would have been better to
                   have done the disconnect when the timeout happened (in
                   fact, that might be a good idea anyway because the
                   disconnection might jolt the collector into doing
                   something). Otherwise, we must *not* do a rollback
                   ... there could be records waiting to be sent (or even
                   partially sent), and a rollback would smash them.

                   This analysis applies for both cases of un-pausing. We can
                   prune the queue (using the DSN from the DATA ACK) but we
                   must do nothing with the data that is "pending" in the
                   connection layer. If we do want to do a rollback, then we
                   *must* force a disconnection first, to ensure that all the
                   "pending" data are thrown away.

                   However, we have another problem ... the sequencing
                   of the DSNs. If we switch servers (collectors), then
                   there will be a gap in the sequence numbers unless
                   we also do a SyncNeeded. So, even though there is
                   no rollback, a SYNC message is sent (when the
                   server (collector) is switched).

                   [Note added later: based on other considerations, it seemed
                   best for timeouts to cause a disconnect anyway. So, we'll
                   never see the "paused" state. But leave the code in, just
                   in case we reverse this decision.
                */
                needsRollback = IPDR_FALSE;
                if (OldCS != NULL) { /* We're switching from another server (collector) */
                    S->SyncNeeded = IPDR_TRUE;
                }
                break;
            default:
                needsRollback = IPDR_FALSE;
                S->SyncNeeded = IPDR_TRUE; /* sync is needed after server (collector) change */
		}

		if (Log_ShouldWrite(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_DEBUG))
		{
			if (needsRollback)
			{
				Log_StateTransition(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO,
						    "ExporterProtocol_SelectCollector", SelectedCS->SessionConnection->TCPConnection,
						    SelectedCS->CSState, IPDR_CS_ACTIVE, MSG_EXPORTER_NEW_ACTIVE_COLLECTOR_ROLLBACK);
			}
			if (S->SyncNeeded)
			{
            Log_StateTransition(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO,
						    "ExporterProtocol_SelectCollector", SelectedCS->SessionConnection->TCPConnection,
						    SelectedCS->CSState, IPDR_CS_ACTIVE, MSG_EXPORTER_NEW_ACTIVE_COLLECTOR_SYNC);
			}


       	}
        /* The following event is also done by ExporterProtocol_HandleStart: */
        if (SelectedCS->SessionConnection != NULL &&
			SelectedCS->SessionConnection->TCPConnection != NULL)
		{
			/*if we choose not currently activ ecollector*/
            IPDR_EventData eventData;
            EventDataInit(&eventData, IPDR_EVENT_SERVER_CONNECTED, E, SelectedCS, S);
            Exporter_TriggerEvent(E, IPDR_LEVEL_WARNING, &eventData, SelectedCS,
                                "Connected: %.98s, outstanding=%lu, unACKed=%lu, Session=%u [%.40s] (%.80s)",
                                TL_GetConnectionInfoStr(SelectedCS->SessionConnection->TCPConnection),
                                EXPORTER_QHANDLER->OutstandingDataCount(E, S),
                                EXPORTER_QHANDLER->UnacknowledgedDataCount(E, S),
                                S->ID, S->Name, S->Description);

			switch(SelectedCS->CSState){
				case IPDR_CS_READY:
					if(forceNegotiate == IPDR_FALSE)
					{
						Status = TL_ResetConnection(SelectedCS->SessionConnection->TCPConnection);
						if(FAILURE(Status))
						{
							Log_StateTransition(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO,
								    "ExporterProtocol_SelectCollector",
								    SelectedCS->SessionConnection->TCPConnection, SelectedCS->CSState,
								    IPDR_CS_DISCONNECTED, MSG_EXPORTER_DISCONNECT_TRANSPORT1);
							TL_HandleDisconnection(SelectedCS->SessionConnection->TCPConnection);
							Platform_Unlock(EXPORTER_PLATFORM);
							return IPDR_OK;
						}
						if(S->Running == IPDR_TRUE)
						{
							Status = ExporterProtocol_SendSessionStart(E, S, SelectedCS->Collector, SelectedCS->SessionConnection->TCPConnection);
							if(FAILURE(Status))
							{
								Log_StateTransition(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO,
										"ExporterProtocol_SelectCollector",
										SelectedCS->SessionConnection->TCPConnection,
												SelectedCS->CSState, IPDR_CS_DISCONNECTED,
										MSG_EXPORTER_DISCONNECT_TRANSPORT3);
								TL_HandleDisconnection(SelectedCS->SessionConnection->TCPConnection);
								Platform_Unlock(EXPORTER_PLATFORM);
								return IPDR_OK;
							}
							SelectedCS->CSState = IPDR_CS_ACTIVE;
							S->ActiveCS = SelectedCS;
							S->Retransmite = IPDR_TRUE;
							Status = Session_SendOutstandingData(E, S);
						}
						break;
					}
				case IPDR_CS_INITIATION:
					Status = ExporterProtocol_SendTemplateData(E, S,
								SelectedCS->Collector,
								SelectedCS->SessionConnection->TCPConnection,
								(OldCS==NULL || IPDR_CS_INITIATION==IPDR_TRUE)?IPDR_TRUE:IPDR_FALSE);
					if(FAILURE(Status))
					{
						Log_StateTransition(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO,
								    "ExporterProtocol_SelectCollector",
								    SelectedCS->SessionConnection->TCPConnection,
								    SelectedCS->CSState, IPDR_CS_DISCONNECTED,
								    MSG_EXPORTER_SEND_TEMPLATE_DATA_FAILED);
						TL_HandleDisconnection(SelectedCS->SessionConnection->TCPConnection);
						Platform_Unlock(EXPORTER_PLATFORM);
						return IPDR_OK;
					}
					break;
				default:
					Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
						"ExporterProtocol_SelectCollector",MSG_EXPORTER_UNEXPECTED_STATE);
					break;
			}
        }

		if (needsRollback)
		{
            EXPORTER_QHANDLER->Rollback(E, S);
        }
    }
	else if (SelectedCS == OldCS)
	{
		/* nothing todo just log */
		Log_StateTransition(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_DEBUG,
							"Session_SelectActiveCollector", SelectedCS->SessionConnection->TCPConnection,
							SelectedCS->CSState, IPDR_SERVER_ACTIVE,
							"got same active server (collector)");
        SelectedCS->CSState = IPDR_CS_ACTIVE;
        S->ActiveCS = SelectedCS;
    }
	Platform_Unlock(EXPORTER_PLATFORM);
	return IPDR_OK;
}

/**
 * Fully closes a connection
 * @param E exporter handle
 * @param IC Contains information about a ipdr connection (IPDRConnection structure)
*/
IPDR_Status
ExporterProtocol_ResetIPDRConnection(Exporter* E, IPDRConnection *IC)
{
	if(IC->ConnectResponseTimerID != NULL)
	{
		Platform_UnregisterTimerAndClear(EXPORTER_PLATFORM, &IC->ConnectResponseTimerID);
		IC->ConnectResponseTimerID = NULL;
	}
	if(IC->KeepAliveINTimerID != NULL)
	{
		Platform_UnregisterTimerAndClear(EXPORTER_PLATFORM, &IC->KeepAliveINTimerID);
		IC->KeepAliveINTimerID = NULL;
	}
	if(IC->KeepAliveOUTTimerID != NULL)
	{
		Platform_UnregisterTimerAndClear(EXPORTER_PLATFORM, &IC->KeepAliveOUTTimerID);
		IC->KeepAliveOUTTimerID = NULL;
	}

	IC->TCPConnection = NULL ;
	IC->State = IPDR_CONNECTION_DISCONNECTED;
	return IPDR_OK;
}

/**
 * Fully closes a connection
 * @param E exporter handle
 * @param IC Contains information about a ipdr connection (IPDRConnection structure)
*/
IPDR_Status
ExporterProtocol_CloseTLConnection(Exporter* E, IPDRConnection *IC)
{
	Connection C = IC->TCPConnection;

	ExporterProtocol_ResetIPDRConnection( E, IC );

	if( C && IPDR_IsValidSocket(C->FD) )
		TL_HandleDisconnection(C);

	return IPDR_OK;
}

int
ExporterProtocol_GetCollectorCapabilities(Collector *Coll)
{
	int  collectorCapabilities = 0;
	if(Coll->Capabilities.StructureSupport == IPDR_TRUE)
		collectorCapabilities = collectorCapabilities | 0x1u ;
	if(Coll->Capabilities.MultiSession == IPDR_TRUE)
		collectorCapabilities = collectorCapabilities | 0x2u ;
	if(Coll->Capabilities.TemplateNegotiation == IPDR_TRUE)
		collectorCapabilities = collectorCapabilities | 0x4u ;
	return collectorCapabilities ;
}

void
ExporterProtocol_SetCollectorCapabilities(Exporter *E, Collector *Coll, unsigned int CollCaps)
{
	int		exporterCapabilities = 0;
	int		collectorCapabilities = 0;
	if(E->Parameters.caps.StructureSupport == IPDR_TRUE)
		exporterCapabilities = exporterCapabilities | 0x1u ;
	if(E->Parameters.caps.MultiSession == IPDR_TRUE)
		exporterCapabilities = exporterCapabilities | 0x2u ;
	if(E->Parameters.caps.TemplateNegotiation == IPDR_TRUE)
		exporterCapabilities = exporterCapabilities | 0x4u ;

	collectorCapabilities = exporterCapabilities & CollCaps ;

	Coll->Capabilities.StructureSupport = (collectorCapabilities & 0x1u)?IPDR_TRUE:IPDR_FALSE;
	Coll->Capabilities.MultiSession = (collectorCapabilities & 0x2u)?IPDR_TRUE:IPDR_FALSE;
	Coll->Capabilities.TemplateNegotiation = (collectorCapabilities & 0x4u)?IPDR_TRUE:IPDR_FALSE;
}
