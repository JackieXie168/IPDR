/** @file Exporter/ExporterProtocol.h
 *
 * Header file containing prototypes for functions which send/handle
 * the IPDR protocol messages, as defined in Exporter/ExporterProtocol.c.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: ExporterProtocol.h,v 1.26 2005/03/17 12:26:47 dmitryk Exp $
 *
 */
#ifndef _EXPORTER_PROTOCOL_H_
#define _EXPORTER_PROTOCOL_H_

#include <IPDR/Config.h>
#define EXPORTER_PROTOCOL_H_RCSID "@(#) $Id: ExporterProtocol.h,v 1.26 2005/03/17 12:26:47 dmitryk Exp $" IPDR_COPYRIGHT_STR

#if defined(GLOBAL_RENAME)
#define ExporterProtocol_DisconnectCallback   IPDR__ExporterProtocol_DisconnectCallback
#define ExporterProtocol_FillHeader           IPDR__ExporterProtocol_FillHeader
#define ExporterProtocol_FlushRecordsQueue    IPDR__ExporterProtocol_FlushRecordsQueue
#define ExporterProtocol_HandleGetSessions    IPDR__ExporterProtocol_HandleGetSessions
#define ExporterProtocol_HandleGetTmpl        IPDR__ExporterProtocol_HandleGetTmpl
#define ExporterProtocol_HandleStartNegotiate IPDR__ExporterProtocol_HandleStartNegotiate
#define ExporterProtocol_HandleStart          IPDR__ExporterProtocol_HandleStart
#define ExporterProtocol_SendTemplateData     IPDR__ExporterProtocol_SendTemplateData
#define ExporterProtocol_rcsid                IPDR__ExporterProtocol_rcsid
#endif

#include <IPDR/IPDR.h>
#include <Exporter/Exporter.h>
#include <Common/MemoryManagement.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EXPORTER_PROTOCOL_MAX_MSG	1024*32


IPDR_Status ExporterProtocol_HandleFlowStart(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	const MessageHeader *Msg,
                                        	Connection Conn);

IPDR_Status ExporterProtocol_HandleFlowStop(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	const MessageHeader *Msg,
                                        	Connection Conn);

IPDR_Status ExporterProtocol_HandleModifyTemplate(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	const MessageHeader *Msg,
                                        	Connection Conn);

IPDR_Status ExporterProtocol_HandleFinalTmplDataACK(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	const MessageHeader *Msg,
                                        	Connection Conn);

IPDR_Status ExporterProtocol_HandleStartNegotiation(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	const MessageHeader *Msg,
                                        	Connection Conn);

IPDR_Status ExporterProtocol_HandleGetSessions(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	const MessageHeader *Msg,
                                        	Connection Conn);

IPDR_Status ExporterProtocol_HandleGetTemplates(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	const MessageHeader *Msg,
                                        	Connection Conn);

IPDR_Status ExporterProtocol_HandleDataACK(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	const MessageHeader *Msg,
											Connection Conn);

IPDR_Status ExporterProtocol_HandleConnect(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	const MessageHeader *Msg,
                                        	Connection Conn);

IPDR_Status ExporterProtocol_HandleConnectResponse(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	const MessageHeader *Msg,
                                        	Connection Conn);

IPDR_Status ExporterProtocol_HandleDisconnect(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	const MessageHeader *Msg,
                                        	Connection Conn);

IPDR_Status ExporterProtocol_HandleError(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	const MessageHeader *Msg,
                                        	Connection Conn);

IPDR_Status ExporterProtocol_HandleKeepAlive(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	const MessageHeader *Msg,
                                        	Connection Conn);

IPDR_Status ExporterProtocol_HandleUnknownMsg(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	const MessageHeader *Msg,
                                        	Connection Conn);

IPDR_Status ExporterProtocol_FillHeader(Session *S,
                                       char *Msg,
                                       unsigned char MessageID,
                                       unsigned long MessageLength);

IPDR_Status ExporterProtocol_FlushRecordsQueue(Exporter *E,
                                              Session *S,
                                              Collector *Coll,
                                              const char *DebugMsg);

void ExporterProtocol_DisconnectCallback(Connection C,
                                       		void *Arg);

void ExporterProtocol_DataCallback(Connection C,
									BufferHandle *BH,
                            		unsigned int MsgLen,
									void *Arg);
void ExporterProtocol_ConnectCallback(Connection C,
                                      IPDR_IPAddress Addr,
                                      unsigned short Port,
									  void *Arg);

IPDR_Status ExporterProtocol_SendConnect(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	Connection Conn);

IPDR_Status ExporterProtocol_SendConnectResponse(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	Connection Conn);

IPDR_Status ExporterProtocol_SendDisconnect(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	Connection Conn);

IPDR_Status ExporterProtocol_SendError(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	Connection Conn,
											enum ErrorCode ErrCode,
											const char* Msg);

IPDR_Status ExporterProtocol_SendKeepAlive(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	Connection Conn);

IPDR_Status ExporterProtocol_SendSessionStart(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	Connection Conn);

IPDR_Status ExporterProtocol_SendSessionStop(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	Connection Conn,
											unsigned short reasonCode,
											const char* ReasonDescripsion);

IPDR_Status ExporterProtocol_SendTemplateData(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	Connection Conn,
											IPDR_BOOL negotiatible);

IPDR_Status ExporterProtocol_SendModifyTemplateResponse(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	Connection Conn);

IPDR_Status ExporterProtocol_SendStartNegotiationReject(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	Connection Conn);

IPDR_Status ExporterProtocol_SendGetSessionsResponse(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	Connection Conn,
											int RequestID);

IPDR_Status ExporterProtocol_SendGetTemplatesResponse(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	Connection Conn,
											int RequestID);

IPDR_Status ExporterProtocol_SendData(Exporter *E,
											Session *S,
                                        	Collector *Coll,
                                        	Connection Conn,
											IPDR_TemplateHandle TemplateHandle,
											const void *Data);
void
ExporterProtocol_KeepAliveOutTimerReset(IPDRConnection * IC);

void
ExporterProtocol_KeepAliveInTimerReset(IPDRConnection * IC);

IPDR_Status
ExporterProtocol_AutoconnectCallback(Exporter *E,
									Collector *Coll,
									Connection C);
IPDR_Status
ExporterProtocol_CloseTLConnection(Exporter* E,IPDRConnection *IC);

IPDR_Status
ExporterProtocol_ResetIPDRConnection(Exporter* E, IPDRConnection *IC);

IPDR_Status
ExporterProtocol_SelectCollector(Exporter* E, Session* S,
								IPDR_BOOL forceNegotiate,
								CollectorSession *forceCS);


#ifdef __cplusplus
}
#endif

#endif
