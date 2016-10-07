/** @file Exporter/Exporter.c
 *
 * Implementation of the IPDR client (transmitter) and initial communication with
 * the server (collector).
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: Exporter.c,v 1.110 2005/05/29 14:50:08 dmitryk Exp $
 *
 * All the API functions are here, plus the functions for handling
 * initial communication with the server (collector) (they are set up by
 * IPDR_StartExporter()). Once communication is established, most of
 * the processing is done by functions in
 * Exporter/ExporterProtocol.c. WHen the connection is disconnected,
 * processing uses functions in this module.
 */

#include <IPDR/Config.h>
#include <Exporter/ExporterMsgs.h>
#include <Exporter/Exporter.h>
#include <Exporter/ExporterMIB.h>
#include <Exporter/ExporterProtocol.h>
#include <IPDR/IPDR.h>
#include <Exporter/Version.h>

const char Exporter_rcsid[] = "@(#) $Id: Exporter.c,v 1.110 2005/05/29 14:50:08 dmitryk Exp $" IPDR_COPYRIGHT_STR;

#include <stdlib.h> /* for abort() in IPDR_InitExporter() */

const char Exporter_h_rcsid[]  = EXPORTER_H_RCSID;
const char IPDR_h_rcsid[]   = IPDR_H_RCSID;
const char Defs_h_rcsid[]    = IPDR_DEFS_H_RCSID;
const char Config_h_rcsid[]  = IPDR_CONFIG_H_RCSID;
const char Version_h_rcsid[] = VERSION_H_RCSID " " IPDR_VER;
const char ExporterMsgs_h_rcsid[] = EXPORTER_MSGS_H_RCSID;


#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include <IPDR/CRANE.h>

#if defined(VXWORKS) || defined(_VXWORKS)
#include <taskLib.h>
#endif
#include <Exporter/Templates.h>
#include <Exporter/ExporterProtocol.h>
#include <Exporter/ExporterConsole.h>
#include <Exporter/ProtocolDebug.h>
#include <Exporter/VQHandler.h>

#include <Common/Protocol.h>
#include <Common/Transport.h>
#include <Common/CommonUtil.h>
#include <Common/Log.h>
#include <Common/CommonMsgs.h>
#include <Transports/TCP/TCPMsgs.h>

#ifdef ENABLE_SIMULATOR
#include <Exporter/HostApp.h>
#endif

#ifdef DEBUG_MEMORY
#include "dmalloc.h"
unsigned long dmalloc_markVal;
unsigned long dmalloc_markValInit = 0;
#endif

#define INITIAL_SESSIONS_ARRAY_CAPACITY 4
#define INITIAL_TEMPLATES_ARRAY_CAPACITY 16
#define INITIAL_COLLECTORS_ARRAY_CAPACITY 3
#define INITIAL_COLLECTOR_CONNECTIONS_ARRAY_CAPACITY 256
#define INITIAL_COLLECTOR_COLLECTORSESSIONS_ARRAY_CAPACITY 256
/*
 * Forward declarations
 */

static void Exporter_ConnectCallback(Connection C, IPDR_IPAddress Addr,
                                   unsigned short Port, void *Arg);
static void Exporter_DisconnectCallback(Connection C, void *Arg);
static void Exporter_DataCallback(Connection C, BufferHandle *BH,
                                unsigned int MsgLen, void *Arg);
static IPDR_Status Exporter_InitializeProtocolInfo(Exporter *E);
static IPDR_Status Exporter_DeleteSessionInternal(Exporter *E, Session* S);
static IPDR_Status Exporter_RemoveCollectorInternal(Exporter *E,Collector *C);
static IPDR_Status Exporter_RemoveCollectorFromSession_Internal(Exporter *E, Session *S, Collector *C);

IPDR_Status Exporter_CreateTemplateInternal(Exporter* E,
											Session* S,
                                           int TemplateID,
                                           const char *SchemaName,
											const char *TypeName,
                                           int NumKeys,
                                           const IPDR_Key *Keys,
                                           IPDR_BOOL StatusFlag,
                                           IPDR_TemplateHandle *TH);

IPDR_Status Exporter_AddKeyInternal(IPDR_SessionHandle SessionHandle,
						IPDR_TemplateHandle TH,
						const IPDR_Key *CKey);

IPDR_Status Exporter_AddKeyInternal_All(IPDR_ExporterHandle ExporterHandle,
						int TemplateID,
						const IPDR_Key *CKey);

IPDR_API IPDR_SendDataCompat(IPDR_ExporterHandle ExporterHandle,
						IPDR_TemplateHandle TemplateHandle,
						const void *Data);

static void Exporter_AutoconnectCallback(void *Arg);

static void
ExporterProtocol_WaitConnectTimeoutCallback(void *Arg);

IPDR_Status
Exporter_InitDebugLevels(Exporter *E);

/**
 * Handlers for transport layer connections, for initial connection.
 */
ConnectionHandlers Exporter_DefaultHandlers = {
    Exporter_ConnectCallback,
    Exporter_DisconnectCallback,
    Exporter_DataCallback,
};


/**
 * Vector of names of the enum ServerState.
 */
const char* ServerStateStr[] = {
    "disconnected",
    "initiation",
    "ready",
    "active",
};

/** Initialize the messages for "Exporter', "Queue", "Protocol" modules.
 * @param L the log
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status ExporterMsgs_Initialize(Log *L)
{
    Log_RegisterModule(L, EXPORTER_MODULE_ID, "Exporter");
    Log_RegisterModule(L, QUEUE_MODULE_ID, "Queue");
    Log_RegisterModule(L, PROTOCOL_MODULE_ID, "Protocol");

    return IPDR_OK;
}


/**
 * Initialize the parameters for a client.
 *
 * @param E exporter handle (pointer)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static IPDR_Status
Exporter_InitializeParameters(Exporter *E)
{
#if defined(USE_WINDOWSIZE)
    E->Parameters.WindowSize =
        ParametersHandler_GetIntParam(& (E->ParamsHandler),
                                       STR_configuration_windowSize);
#endif

    E->Parameters.WindowTimeout =
        ParametersHandler_GetIntParam(& (E->ParamsHandler),
                                      STR_configuration_windowTimeout);

    E->Parameters.ProtocolLevelAddr =
        TL_ParseAddress(ParametersHandler_GetStringParam(& (E->ParamsHandler),
                                      STR_configuration_protocolLevelAddr));

    E->Parameters.StatisticsInterval =
        ParametersHandler_GetIntParam(& (E->ParamsHandler),
                                      STR_configuration_statisticsInterval);

    Util_StrDupAndReplace(EXPORTER_PLATFORM,
                          ParametersHandler_GetStringParam(& (E->ParamsHandler),
                                                           STR_configuration_vendorString),
                          & (E->Parameters.VendorString));

	/* IPDR Parameters */
    E->Parameters.KeepAliveInterval =
        ParametersHandler_GetIntParam(& (E->ParamsHandler),
                                      STR_configuration_keepAliveInterval);

    E->Parameters.AutoconnectInterval =
        ParametersHandler_GetIntParam(& (E->ParamsHandler),
                                      STR_configuration_autoconnectInterval);

    E->Parameters.TemplateAckTimeout =
        ParametersHandler_GetIntParam(& (E->ParamsHandler),
                                      STR_configuration_templateAckTimeout);

    E->Parameters.ConnectResponseTimeout =
        ParametersHandler_GetIntParam(& (E->ParamsHandler),
                                      STR_configuration_connectResponseTimeout);

    E->Parameters.WaitBestCollectorTimeout =
        ParametersHandler_GetIntParam(& (E->ParamsHandler),
                                      STR_configuration_waitBestCollectorTimeout);

	E->Parameters.DataAckTimeInterval =
		ParametersHandler_GetIntParam(& (E->ParamsHandler),
                                      STR_configuration_dataAckTimeInterval);

	E->Parameters.DataAckSequenceInterval =
		ParametersHandler_GetIntParam(& (E->ParamsHandler),
                                      STR_configuration_dataAckSequenceInterval);

    E->Parameters.caps.MultiSession =
        ParametersHandler_GetIntParam(& (E->ParamsHandler),
                                      STR_configuration_capabilities_multiSession);

    E->Parameters.caps.TemplateNegotiation =
        ParametersHandler_GetIntParam(& (E->ParamsHandler),
                                      STR_configuration_capabilities_templateNegotiation);

    E->Parameters.caps.StructureSupport =
        ParametersHandler_GetIntParam(& (E->ParamsHandler),
                                      STR_configuration_capabilities_structureSupport);

    return IPDR_OK;
}


/*
 **********************
 * IPDR API function *
 **********************
 */
const char*
IPDR_StatusMsg(IPDR_ExporterHandle ExporterHandle,
                IPDR_Status Status)
{
	static char buf[256];
	memset(buf, 0, 256);
    typedef struct { IPDR_Status code; const char* msg; } CodeAndMsg_t;

    static CodeAndMsg_t codeAndMsg[] = {
        { IPDR_OK,                          "Result OK" },
        { IPDR_ERR,                         "General error" },
        { IPDR_ERR_INVALID_VALUE,           "Invalid value" },
        { IPDR_ERR_ALLOCATION_FAILED,       "Memory allocation failed" },
        { IPDR_ERR_NO_AVAILABLE_TIMERS,     "No available timers" },
        { IPDR_ERR_TIMER_NOT_FOUND,         "Timer not found" },
        { IPDR_ERR_TIMER_ALREADY_STARTED,   "Timer already started" },
        { IPDR_ERR_TIMER_NOT_STARTED,       "Timer not started" },
        { IPDR_ERR_PERSISTENCE_ERROR,       "Persistence error" },
        { IPDR_ERR_COMMUNICATION_ERROR,     "Communication error" },
        { IPDR_ERR_ADDRESS_ALREADY_IN_USE,  "Address already in use, restricted, or invalid" },
        { IPDR_ERR_OPERATION_WOULD_BLOCK,   "Operation would block" },
        { IPDR_ERR_SESSION_ALREADY_EXISTS,  "Session already exists" },
        { IPDR_ERR_UNABLE_TO_FIND_SESSION,  "Unable to find session" },
        { IPDR_ERR_PORT_ALREADY_IN_USE,     "Port already in use" },
        { IPDR_ERR_COLLECTOR_ALREADY_EXISTS,"Collector already exists" },
        { IPDR_ERR_COLLECTOR_NOT_FOUND,     "Collector not found" },
        { IPDR_ERR_TEMPLATE_ALREADY_EXISTS, "Template already exists" },
        { IPDR_ERR_TEMPLATE_NOT_FOUND,      "Template not found" },
        { IPDR_ERR_KEY_NOT_FOUND,           "Key not found" },
        { IPDR_ERR_KEY_ALREADY_EXISTS,      "Key already exists" },
        { IPDR_ERR_QUEUE_FULL,              "Queue full" },
        { IPDR_ERR_NOT_RUNNING,             "Not running" },
        { IPDR_ERR_MIB_VERSION_MISMATCH,    "MIB version mismatch" },
        { IPDR_ERR_MIB_NODE_NOT_FOUND,      "MIB node not found" },
        { IPDR_ERR_MIB_NODE_NOT_WRITABLE,   "MIB node not writable" },
        { IPDR_ERR_NO_ACTIVE_COLLECTOR,     "No active collector or collector disconnected" },
        { IPDR_ERR_ALREADY_RUNNING,         "Already running" },
		{ IPDR_ERR_COLLECTOR_ALREADY_EXISTS,"Collector already exists" },
        { IPDR_CMD_EXIT,                    "'exit' command" },
        { IPDR_CMD_NOT_FOUND,               "Command not found" },
        { IPDR_CMD_HELP,                    "'help' command" },
		{ IPDR_ERR_TEMPLATE_DISABLED,       "Template Disabled" },
		{ IPDR_ERR_ACTIVE_COLLECTOR_EXISTS, "Active collector exists" },
		{ IPDR_ERR_NOT_IMPLEMENTED	  , 	"Function is not implemented" },
        { IPDR_ERR_SESSION_IS_RUNNING	  , "Session is running, must be stoped" },
		{ IPDR_ERR_SESSION_IS_NOT_RUNNING	, "Session is not running, must be started" },
		{ IPDR_ERR_COLLECTOR_NOT_IN_SESSION	,"Collector not Associated with session" },
		{ IPDR_ERR_MEMORY_FREE				,"Trying to free already freed memeory" },
		{ IPDR_ERR_SESSION_LOGICALLY_RUN	, "Logically sturting running session" },
		{ IPDR_ERR_SESSION_LOGICALLY_STOPED	, "Logically stoping not running session" },
		{ IPDR_ERR_OPERATION_WOULD_BLOCK	, "Operation not complete (posible timeout)" },
		{ IPDR_ERR_COLLECTORSESSION_REASSOC	, "Reasociating collector to session (posible changing priotity)" },
		{ 0, 0 }
    };

    CodeAndMsg_t* cm;
    for (cm = codeAndMsg; cm->msg != 0; cm++) {
        if (cm->code == Status) {
			sprintf(buf, "%s - %s <%u>", SUCCESS(cm->code)?"SUCCESS":"FAILURE", cm->msg, Status);
            return buf;
        }
    }

    /* If C != NULL, we should use an area within it; but the
       following should suffice because we're just constructing a
       message and that situation shouldn't occur anyway. */

    { /* Unknown status code: construct a useful message */
        sprintf(buf, "FAILURE - Unknown Result Code <%du>", Status);
        return buf;
    }
}


/*
 **********************
 * IPDR API function *
 **********************
 */
const char*
IPDR_EventMsg(const IPDR_EventData* Event)
{
    if (Event) {
        switch (Event->EventID) {
        case IPDR_EVENT_SERVER_CONNECTED:          return "connected";
        case IPDR_EVENT_ALL_SERVERS_DISCONNECTED:  return "disconnected all";
        case IPDR_EVENT_SERVER_DISCONNECTED:
            switch (Event->Extra.Disconnected.Reason) {
            case IPDR_EVENT_DISCONNECTED_INITIAL:  return "disconnected(initial)";
            case IPDR_EVENT_DISCONNECTED_TIMEOUT:  return "disconnected(timeout)";
            case IPDR_EVENT_DISCONNECTED_TCP:      return "disconnected(TCP)";
            case IPDR_EVENT_DISCONNECTED_STOP:     return "disconnected(stop)";
            case IPDR_EVENT_DISCONNECTED_TOO_SLOW: return "disconnected(too slow)";
            default:                                return "disconnected";
            }
        case IPDR_EVENT_RECORD_LOST:               return "lost record";
        case IPDR_EVENT_ACTIVE_SERVER_CHANGED:     return "server (collector) changed";
        case IPDR_EVENT_QUEUE_EMPTY:               return "queue empty";
        case IPDR_EVENT_ERROR:
            switch (Event->Extra.Error.Reason) {
            case IPDR_EVENT_ERROR_UNKNOWN_SESSION: return "error(unknown session)";
            case IPDR_EVENT_ERROR_UNKNOWN_SERVER:  return "error(unknown server (collector))";
            default:                                return "error";
            }
        default:                                    return "???";
        }
    } else {
        return ("<empty event>");
    }
}


/**
 * Initialize a statistics structure.
 *
 * @param PS platform services
 * @param Now gets current time (may be NULL)
 * @param PrevStatistics old structure to zero out (may be NULL)
 * @param Statistics structure to initialize (may be NULL)
 */
static void
StatisticsInitialize(IPDR_PlatformServices *PS,
                     IPDR_Time *Now,
                     IPDR_Statistics *PrevStatistics,
                     IPDR_Statistics *Statistics)
{
#ifdef DEBUG_MEMORY
    dmalloc_markVal = dmalloc_mark();    {
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
    if (dmalloc_markValInit == 0) {
        dmalloc_markValInit = dmalloc_markVal;
    }
#endif

    if (Now) {
        Platform_GetTime(PS, Now);
    }
    if (Statistics) {
        Statistics->TransmittedRecords  = 0;
        Statistics->AcknowledgedRecords = 0;
        Statistics->EnqueuedRecords     = 0;
        Statistics->LostRecords         = 0;
    }
    if (PrevStatistics) {
        PrevStatistics->TransmittedRecords  = 0;
        PrevStatistics->AcknowledgedRecords = 0;
        PrevStatistics->EnqueuedRecords     = 0;
        PrevStatistics->LostRecords         = 0;
    }
}


/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_GetSessionStatistics(IPDR_ExporterHandle ExporterHandle,
                           IPDR_SessionHandle SessionHandle,
                           IPDR_Time *Now,
                           IPDR_Statistics *Statistics,
                           unsigned long *QueuedOutstandingRecords,
                           unsigned long *QueuedUnacknowledgedRecords)
{
	Exporter *E = ExporterHandle;
    Session *S = SessionHandle;
    static const IPDR_Statistics zeroStats = { 0, 0, 0, 0 };
    if (Statistics) {
        *Statistics = zeroStats;
    }
    if (QueuedOutstandingRecords)    { *QueuedOutstandingRecords = 0; }
    if (QueuedUnacknowledgedRecords) { *QueuedUnacknowledgedRecords = 0; }
    if (Now) { *Now = 0; }
    if (! E || ! S) {
        return IPDR_ERR_INVALID_VALUE;
    }

    Platform_Lock(EXPORTER_PLATFORM);

    if (Now) {
        Platform_GetTime(EXPORTER_PLATFORM, Now);
    }

    if (Statistics) {
        *Statistics = S->Statistics;
    }

    if (QueuedOutstandingRecords) {
        *QueuedOutstandingRecords = EXPORTER_QHANDLER->OutstandingDataCount(E, S);
    }

    if (QueuedUnacknowledgedRecords) {
        *QueuedUnacknowledgedRecords = EXPORTER_QHANDLER->UnacknowledgedDataCount(E, S);
    }

    Platform_Unlock(EXPORTER_PLATFORM);
    return IPDR_OK;
}


/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_GetCollectorStatisticsAndState(IPDR_ExporterHandle ExporterHandle,
                                  IPDR_SessionHandle SessionHandle,
                                  IPDR_CollectorHandle CollectorHandle,
                                  IPDR_Time *Now,
                                  IPDR_Statistics *Statistics,
                                  IPDR_EventData* LastEvent)
{
    Exporter *E = ExporterHandle;
    Session *S = SessionHandle;
	Collector *C = CollectorHandle;
    int Index;
    static const IPDR_Statistics zeroStats = { 0, 0, 0, 0 };
    if (Statistics) { *Statistics = zeroStats; }
    if (Now) { * Now = 0; }
    if (LastEvent) { /* Set a default value in case we don't have a current server (collector) */
        EventDataInit(LastEvent, IPDR_EVENT_SERVER_DISCONNECTED, NULL, NULL, NULL);
        LastEvent->Extra.Disconnected.Reason = IPDR_EVENT_DISCONNECTED_INITIAL;
    }
    if (! E || ! S) {
        return IPDR_ERR_INVALID_VALUE;
    }

    Platform_Lock(EXPORTER_PLATFORM);

    if (Now) {
        Platform_GetTime(EXPORTER_PLATFORM, Now);
    }

    for (Index = 0; Index < S->CollectorSessions.Length; Index++) {
        CollectorSession *CS = (CollectorSession *) S->CollectorSessions.Data[Index];
        if (CS->Collector == C ) {
			if (Statistics) {
                *Statistics = CS->Statistics;
            }
            if (LastEvent) {
                *LastEvent = CS->LastEvent;
            }
            Platform_Unlock(EXPORTER_PLATFORM);
            return IPDR_OK;
        }
    }
    Platform_Unlock(EXPORTER_PLATFORM);
    StatisticsInitialize(EXPORTER_PLATFORM, NULL, NULL, Statistics); /* zero return value */
    return IPDR_ERR_COLLECTOR_NOT_IN_SESSION;
}

/*
 **********************
 * IPDR API function *
 **********************
*/
static void
StatisticsCalculateRates(IPDR_PlatformServices *PS,
                         IPDR_Time PrevTime,
                         const IPDR_Statistics *Statistics,
                         const IPDR_Statistics *PrevStatistics,
                         IPDR_Time *Time,
                         IPDR_Statistics *IntervalStatistics,
                         IPDR_Statistics *Rate100)
{
    unsigned int timeInterval;
    Platform_GetTime(PS, Time);
    timeInterval = *Time - PrevTime;

    IntervalStatistics->TransmittedRecords  = Statistics->TransmittedRecords  - PrevStatistics->TransmittedRecords;
    IntervalStatistics->AcknowledgedRecords = Statistics->AcknowledgedRecords - PrevStatistics->AcknowledgedRecords;
    IntervalStatistics->EnqueuedRecords     = Statistics->EnqueuedRecords     - PrevStatistics->EnqueuedRecords;
    IntervalStatistics->LostRecords         = Statistics->LostRecords         - PrevStatistics->LostRecords;

    if (timeInterval > 0) {
        Rate100->TransmittedRecords  = IntervalStatistics->TransmittedRecords  * 100 / timeInterval;
        Rate100->AcknowledgedRecords = IntervalStatistics->AcknowledgedRecords * 100 / timeInterval;
        Rate100->EnqueuedRecords     = IntervalStatistics->EnqueuedRecords     * 100 / timeInterval;
        Rate100->LostRecords         = IntervalStatistics->LostRecords         * 100 / timeInterval;
    } else {
        Rate100->TransmittedRecords  = 0;
        Rate100->AcknowledgedRecords = 0;
        Rate100->EnqueuedRecords     = 0;
        Rate100->LostRecords         = 0;
    }
}


/**
 * Callback triggered in order to report statistics to the log..
 *
 * @param Arg callback argument (Exporter)
 */
static void
Exporter_StatisticsReportCallback(void *Arg)
{
	Exporter *E = (Exporter *) Arg;
    int i, j;
    Session *S;

    /*NOTE: MIB default comes from ClientMIBConstants.c*/
    Platform_Lock(EXPORTER_PLATFORM);

    for (i = 0; i < E->Sessions.Length; i++) {
        char timeStrBuf[IPDR_TIME_FORMAT_LENGTH + 1]; /* +1 for "Z" */

        Platform_FormatTimeGMTNow(EXPORTER_PLATFORM, timeStrBuf);
        strcat(timeStrBuf, "Z"); /* mark it as "GMT" */
        S = (Session *) E->Sessions.Data[i];

        {
            unsigned int numAllocs;
            unsigned int numFrees;
            size_t bytesAllocated;
            IPDR_Statistics IntervalStatistics;
            IPDR_Statistics Rate100;
            Platform_MemoryStatistics(EXPORTER_PLATFORM, &numAllocs, &numFrees, &bytesAllocated);
            StatisticsCalculateRates(EXPORTER_PLATFORM,
                                     S->PrevStatisticsTime, &S->Statistics, &S->PrevStatistics,
                                     &S->PrevStatisticsTime, &IntervalStatistics, &Rate100);
#ifdef DEBUG_MODE
            /* Note: IPDR_VER is provided by "-D" flag in compilation */
            Log_Write2(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_INFO, "",
                       MSG_EXPORTER_BUILT_INFO, timeStrBuf, IPDR_VER);
#endif
            Log_Write2(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_INFO, "",
                       MSG_EXPORTER_SESSION_STATISTICS1, S->ID, timeStrBuf);
            Log_Write6(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_INFO, "",
                       MSG_EXPORTER_SESSION_STATISTICS2,
                       EXPORTER_QHANDLER->OutstandingDataCount(E, S),
                       EXPORTER_QHANDLER->UnacknowledgedDataCount(E, S),
                       numAllocs, numFrees, (long unsigned)bytesAllocated,
                       EXPORTER_QHANDLER->UsedMemory(E, S));
            Log_Write5(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_INFO, "",
                       MSG_EXPORTER_SESSION_STATISTICS3,
                       "TX  :",
                       S->Statistics.TransmittedRecords,
                       IntervalStatistics.TransmittedRecords,
                       Rate100.TransmittedRecords/100, Rate100.TransmittedRecords%100);
            Log_Write5(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_INFO, "",
                       MSG_EXPORTER_SESSION_STATISTICS3,
                       "ACK :",
                       S->Statistics.AcknowledgedRecords,
                       IntervalStatistics.AcknowledgedRecords,
                       Rate100.AcknowledgedRecords/100, Rate100.AcknowledgedRecords%100);
            Log_Write5(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_INFO, "",
                       MSG_EXPORTER_SESSION_STATISTICS3,
                       "enq :",
                       S->Statistics.EnqueuedRecords,
                       IntervalStatistics.EnqueuedRecords,
                       Rate100.EnqueuedRecords/100, Rate100.EnqueuedRecords%100);
            Log_Write5(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_INFO, "",
                       MSG_EXPORTER_SESSION_STATISTICS3,
                       "lost:",
                       S->Statistics.LostRecords,
                       IntervalStatistics.LostRecords,
                       Rate100.LostRecords/100, Rate100.LostRecords%100);

            S->PrevStatistics = S->Statistics; /* S->PrevStatisticsTime was set by StatisticsCalculateRates above */
        }

        for (j = 0; j < S->CollectorSessions.Length; j++) {
            IPDR_Statistics IntervalStatistics;
            IPDR_Statistics Rate100;
            TL_PABuf paBuf;
            CollectorSession *CS = (CollectorSession *) S->CollectorSessions.Data[j];
            StatisticsCalculateRates(EXPORTER_PLATFORM,
                                     CS->PrevStatisticsTime, &CS->Statistics, &CS->PrevStatistics,
                                     &CS->PrevStatisticsTime, &IntervalStatistics, &Rate100);
			if( CS->SessionConnection && CS->SessionConnection->TCPConnection && CS->Collector )
            	Log_Write2(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_INFO, "",
                	       MSG_EXPORTER_SERVER_STATISTICS1,
                    	   TL_PrintAddressAndPort(CS->Collector->Addr, CS->Collector->Port, paBuf),
                       		TL_GetConnectionInfoStr(CS->SessionConnection->TCPConnection));
            Log_Write5(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_INFO, "",
                       MSG_EXPORTER_SERVER_STATISTICS2,
                       "TX :",
                       CS->Statistics.TransmittedRecords,
                       IntervalStatistics.TransmittedRecords,
                       Rate100.TransmittedRecords/100, Rate100.TransmittedRecords%100);
            Log_Write5(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_INFO, "",
                       MSG_EXPORTER_SERVER_STATISTICS2,
                       "ACK:",
                       CS->Statistics.AcknowledgedRecords,
                       IntervalStatistics.AcknowledgedRecords,
                       Rate100.AcknowledgedRecords/100, Rate100.AcknowledgedRecords%100);
            CS->PrevStatistics = CS->Statistics; /* Srv->PrevStatisticsTime was set by StatisticsCalculateRates above */
        }

#ifdef DEBUG_MEMORY
        {
            dmalloc_message("======================== %.40s", timeStrBuf);
            dmalloc_log_changed(dmalloc_markVal, 1, 1, 1);
            dmalloc_markVal = dmalloc_mark();
#if 0
            dmalloc_message("===+==+==+==+==+==+===== %.40s", timeStrBuf);
            dmalloc_log_changed(dmalloc_markValInit, 1, 1, 1);
#endif
        }

        /* This is a bit of a kludge: it outputs chunk information if
         * an internal queue is being used (see IPDR_SetQueueHandler).
         * For debugging only, of course.
         */
        if (S->ActiveContext) {
            TransmissionContext *ctxt = S->ActiveContext;
            for (; ctxt; ctxt = ctxt->NextContext) {
                if (E->QHandler == &VQHandler) {
                    ChunksManager_Dump(&((VolatileQueue*)ctxt->QueueHandlerData)->ChunksMgr, "");
                }
            }
        }
#endif
    }

    Platform_Unlock(EXPORTER_PLATFORM);
}

/**
 * Callback triggered in order to allow to connect to the
 * configured collectors
 * @param Arg callback argument (Exporter)
*/
static void
Exporter_AutoconnectCallback(void *Arg)
{
    Exporter *E = (Exporter *) Arg;
	IPDR_PlatformServices *PS = E->PS;
    Collector *Coll;
	IPDRConnection *Conn ;
	Connection C;
    IPDR_Status Status;
	int Index, Index1 ;

    Platform_Lock(EXPORTER_PLATFORM);

	for (Index = 0; Index < E->Collectors.Length; Index++)
	{
		Coll = (Collector *) E->Collectors.Data[Index];
		for (Index1 = 0; Index1 < Coll->IPDRConnections.Length; Index1++)
	    {
			Conn = (IPDRConnection *)Coll->IPDRConnections.Data[Index1];
			if( (Conn->Autoconnect == IPDR_TRUE) && (Conn->TCPConnection == NULL) )
			{
				Status = TL_CreateConnection(E->TransportHandle, E->LocalAddr, Coll->Addr, Coll->Port, (TCPConnection **)&Conn->TCPConnection);
			    if (FAILURE(Status))
				{
					Log_Write3(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_ERROR, "Exporter_AutoconnectCallback",
								MSG_EXPORTER_LOG_CONNECT_ERROR,
								"TL_CreateConnection", IPDR_StatusMsg(E,Status), TL_GetConnectionInfoStr(C));
					continue;
				}
				Conn->TCPConnection->Owner = Conn ;
			}
			C = Conn->TCPConnection	;
			if((Conn->Autoconnect == IPDR_TRUE) && (Conn->State == IPDR_CONNECTION_DISCONNECTED))
			{
				Status = TL_Connect(PS, C);
				if (FAILURE(Status))
				{
					Log_Write3(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_ERROR, "Exporter_AutoconnectCallback",
								MSG_EXPORTER_LOG_CONNECT_ERROR,
								"TL_Connect", IPDR_StatusMsg(E,Status), TL_GetConnectionInfoStr(C));
					TL_HandleDisconnection(C);
					continue;
				}
				Status = ExporterProtocol_AutoconnectCallback(E, Coll, C);
				if(FAILURE(Status))
				{
					TL_HandleDisconnection(C);
				}
			}
	    }
	}

	Platform_Unlock(EXPORTER_PLATFORM);
}

/**
 * Delete resources associated with a transmission context.
 * @note not static --- used by MemoryManagement.c
 *
 * @param PS platform services
 * @param Obj object to delete
 */
void
TransmissionContext_Destructor(IPDR_PlatformServices *PS,
                               void *Obj)
{
    TransmissionContext *TC = (TransmissionContext *) Obj;
    Session *S = TC->SessionPtr;
    Exporter *E = S->ExporterPtr;

    strncpy(TC->eyecatcher, "Dxmtctx", sizeof TC->eyecatcher);
    DynamicArray_Delete(PS, & (TC->Templates));
    EXPORTER_QHANDLER->DeleteTransmissionContext(E, S, TC);
    strncpy(TC->eyecatcher, "Xxmtctx", sizeof TC->eyecatcher);
}


/**
 * Delete resources associated with a client.
 * @note not static --- used by MemoryManagement.c
 *
 * @param PS platform services
 * @param Obj object to delete
 */
void
Exporter_Destructor(IPDR_PlatformServices *PS,
                  void *Obj)
{

    Exporter *E = (Exporter *) Obj;

    strncpy(E->eyecatcher, "Dclient", sizeof E->eyecatcher);

    Object_Release(PS, E->Parameters.VendorString);
	Platform_MemoryFree(EXPORTER_PLATFORM, E->CSessions);

    Platform_UnregisterTimerAndClear(PS, &E->StatisticsTimerID);
	Platform_UnregisterTimerAndClear(PS, &E->AutoconnectTimerID);

    Platform_UnregisterSocketAndClear(EXPORTER_PLATFORM, &E->ProtocolInfoEvent);
    Platform_CloseAndInvalidate(EXPORTER_PLATFORM, &E->ProtocolInfoSocket);

    EXPORTER_QHANDLER->Destroy(E);

	DynamicArray_Delete(PS, & (E->Sessions));
	DynamicArray_Delete(PS, & (E->Collectors));
	DynamicArray_Delete(PS, & (E->DummyCollectors));
	DynamicArray_Delete(PS, & (E->CollectorSessions));
	Console_Delete(& (E->ConsolePtr));

    TL_Shutdown(E->TransportHandle);

    Log_Disable(& E->ExporterLog);

    Log_Finalize(& E->ExporterLog);
    strncpy(E->eyecatcher, "Xclient", sizeof E->eyecatcher);
}


/**
 * Delete resources associated with a session.
 * @note not static
 * @param PS platform services
 * @param Obj object to delete
 */
void
Session_Destructor(IPDR_PlatformServices *PS,
                   void *Obj)
{
    Session *S = (Session *) Obj;
    Exporter *E = S->ExporterPtr;

    TransmissionContext *TC = S->ActiveContext, *NextTC;

    strncpy(S->eyecatcher, "Dsessio", sizeof S->eyecatcher);

	DynamicArray_Delete(PS, & (S->CollectorSessions));
	DynamicArray_Delete(PS, & (S->Templates));

    EXPORTER_QHANDLER->DeleteSession(E, S);

    while (TC != NULL) {
        NextTC = TC->NextContext;
        Object_Release(PS, TC);
        TC = NextTC;
    }

    Object_Release(PS, S->Name);
    Object_Release(PS, S->Description);

    strncpy(S->eyecatcher, "Xsessio", sizeof S->eyecatcher);
}

void
Collector_Destructor(IPDR_PlatformServices *PS,
                  void *Obj)
{
	Collector *C = (Collector *)Obj;

	Exporter *E = C->ExporterPtr;
	Collector *Coll;
	int Index;

	strncpy(C->eyecatcher, "Dcollector", sizeof C->eyecatcher);

	/* Remove the Collector from Exporter's Collector list */
	for (Index = 0; Index < E->Collectors.Length; Index++)
	{
        Coll = (Collector *) E->Collectors.Data[Index];
        if (Coll != NULL && Coll == C && Coll->Addr == C->Addr && strcmp(Coll->Name, C->Name) == 0)
		{
			DynamicArray_RemoveAndShiftNoFree(PS, & (E->Collectors), Index);
        }
    }

	for (Index = 0; Index < E->DummyCollectors.Length; Index++)
	{
        Coll = (Collector *) E->DummyCollectors.Data[Index];
        if (Coll == C)
		{
			DynamicArray_RemoveAndShiftNoFree(PS, & (E->DummyCollectors), Index);
        }
    }

	DynamicArray_Delete(PS, & (C->IPDRConnections));
	DynamicArray_Delete(PS, & (C->CollectorSessions));

	Object_Release(PS, C->Name);

	strncpy(C->eyecatcher, "Xcollector", sizeof C->eyecatcher);
}

void
Connection_Destructor(IPDR_PlatformServices *PS,
                  void *Obj)
{
	IPDRConnection *C = (IPDRConnection *)Obj;
	Exporter *E = C->ExporterPtr;

	strncpy(C->eyecatcher, "Dconnect", sizeof(C->eyecatcher));

	if(C->KeepAliveOUTTimerID != NULL)
	{
		Platform_UnregisterTimerAndClear(EXPORTER_PLATFORM, &C->KeepAliveOUTTimerID);
		C->KeepAliveOUTTimerID = NULL;
	}

	if(C->KeepAliveINTimerID != NULL)
	{
		Platform_UnregisterTimerAndClear(EXPORTER_PLATFORM, &C->KeepAliveINTimerID);
		C->KeepAliveINTimerID = NULL;
	}

	if(C->TCPConnection != NULL)
	{
		TL_HandleDisconnection(C->TCPConnection);
		/*Platform_SleepMS(PS, 1000);*/
	}

	C->TCPConnection = NULL ;
	C->Owner = NULL ;
	C->ExporterPtr = NULL ;

	C->State = IPDR_CONNECTION_DISCONNECTED ;

	strncpy(C->eyecatcher, "Xconnect", sizeof C->eyecatcher);
}

void CollectorSession_Destructor(IPDR_PlatformServices *PS, void *Obj)
{
	CollectorSession*	CS = (CollectorSession *)Obj;
	Session* 			S = CS->SessionPtr ;
	Exporter*			E = S->ExporterPtr;
	Collector* 			C = CS->Collector ;
	int					Index ;


	strncpy(CS->eyecatcher, "Dcollsess", sizeof CS->eyecatcher);

	if(CS->TmplDataTimer != NULL)
    	Platform_UnregisterTimerAndClear(EXPORTER_PLATFORM, &CS->TmplDataTimer);

	if(CS->WindowSizeTimer != NULL)
    	Platform_UnregisterTimerAndClear(EXPORTER_PLATFORM, &CS->WindowSizeTimer);

	/* Remove the CollectorSession from Exporter's Collector list */
	for (Index = 0; Index < E->CollectorSessions.Length; Index++)
		if( CS == E->CollectorSessions.Data[Index] )
			DynamicArray_RemoveAndShiftNoFree(PS, & (E->CollectorSessions), Index);

	/* Remove the Collector Session from its  session */
	for (Index = 0; Index < S->CollectorSessions.Length; Index++)
		if( CS == S->CollectorSessions.Data[Index] )
			DynamicArray_RemoveAndShiftNoFree(PS, & (S->CollectorSessions), Index);

	/* Remove the Collector Session from its Collector */
	for (Index = 0; Index < C->CollectorSessions.Length; Index++)
		if( CS == C->CollectorSessions.Data[Index] )
			DynamicArray_RemoveAndShiftNoFree(PS, & (C->CollectorSessions), Index);

	strncpy(CS->eyecatcher, "Xcollsess", sizeof CS->eyecatcher);
}

IPDR_Status Exporter_AddIPDRConnectionToCollector(	Exporter *E,
													Collector *Coll,
													Connection C )
{
	IPDR_Status Status;
	IPDRConnection *IC;
	int Index ;

	Platform_Lock(EXPORTER_PLATFORM);


	Status = Object_Allocate(E->PS, sizeof(IPDRConnection),
                            		Connection_Destructor,
                             		(void **) & IC);
	if(FAILURE(Status))
	{
		Log_Write1(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
                	   "Exporter_ConnectCallback", "Object_Allocate Error, Status: %d",
                	   Status);
		Platform_Unlock(EXPORTER_PLATFORM);
		return Status ;
	}

	strncpy(IC->eyecatcher, "CONNEC", sizeof(IC->eyecatcher));
	IC->State = IPDR_CONNECTION_DISCONNECTED ;
	IC->Autoconnect = (Coll->NumOfConnections == 0)?IPDR_FALSE:IPDR_TRUE;
	IC->TCPConnection = C;
	IC->Owner = Coll;
	IC->ConnectResponseTimerID = NULL;
	IC->KeepAliveINTimerID = NULL;
	IC->KeepAliveOUTTimerID = NULL;
	IC->RemoteKeepAliveInterval = (E->Parameters.KeepAliveInterval)+500;
	IC->isDummy = Coll->isDummy ;
	IC->ExporterPtr = E;

	if(C != NULL)
		C->Owner = IC;

	Status = DynamicArray_AppendElement(E->PS, &(Coll->IPDRConnections), IC, &Index);
	if(FAILURE(Status))
	{
		Log_Write1(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
                	   "Exporter_ConnectCallback", "DynamicArray_AppendElement Error, Status: %d",
                	   Status);
		Platform_Unlock(EXPORTER_PLATFORM);
		return Status ;
	}

	Platform_Unlock(EXPORTER_PLATFORM);

	return IPDR_OK;
}


/**
 * Callback invoked when a server (collector) connects.
 * @param Conn connected connection
 * @param Addr remove address
 * @param Port remove port
 * @param Arg  callback argument (Exporter)
 */
static void
Exporter_ConnectCallback(Connection Conn,
                       IPDR_IPAddress Addr,
                       unsigned short Port,
                       void *Arg)
{
    Exporter *E = (Exporter*)Arg;
	Collector *Coll = NULL;
	IPDRConnection *IC = NULL;
	IPDR_Status Status;
	/*int Index ;*/
	TL_PABuf paBuf;

	Platform_Lock(EXPORTER_PLATFORM);

    Log_StateTransition(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_DEBUG,
                        "Exporter_ConnectCallback", Conn,
                        IPDR_CS_DISCONNECTED, IPDR_CS_DISCONNECTED,
                        TL_PrintAddressAndPort(Addr, Port, paBuf));

	/*for (Index = 0; Index < E->DummyCollectors.Length; Index++)
	{
        Coll = (Collector *) E->DummyCollectors.Data[Index];
        if (Coll->Addr == Conn->RemoteAddr)
			break ;

		Coll = NULL ;
    }*/

	if(Coll == NULL)
	{
		if(FAILURE(Exporter_AddCollectorInternal(E,
									TL_PrintAddressAndPort(Conn->RemoteAddr, Conn->RemotePort, paBuf),
									Conn->RemoteAddr,
									Conn->RemotePort,
									0,IPDR_TRUE,
									&Coll)))
		{
			Log_Write1(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
					"Exporter_ConnectCallback", MSG_EXPORTER_UNKNOWN_SERVER,
					TL_PrintAddressAndPort(Conn->RemoteAddr, Conn->RemotePort, paBuf));
			Platform_Unlock(EXPORTER_PLATFORM);
			return ;
		}

		/*for (Index = 0; Index < E->DummyCollectors.Length; Index++)
		{
			Coll = (Collector *) E->DummyCollectors.Data[Index];
			if (Coll->Addr == Conn->RemoteAddr &&
				Coll->Port == Conn->RemotePort &&
				strcmp(Coll->Name, TL_PrintAddressAndPort(Conn->RemoteAddr, Conn->RemotePort, paBuf)) == 0)
				break ;

			Coll = NULL ;
		}*/
	}

	if(Coll == NULL)
	{
		Log_Write1(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
				"Exporter_ConnectCallback", "Failed To Create Dummy Collector, Remote Addres: %.98s",
				TL_GetConnectionInfoStr(Conn));
		Platform_Unlock(EXPORTER_PLATFORM);
		return ;
	}

	Status = Exporter_AddIPDRConnectionToCollector(	E, Coll, Conn );
	if(FAILURE(Status))
	{
		Log_Write1(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
				"Exporter_ConnectCallback", "Failed To Add IPDRConnection To Collector, Remote Addres: %.98s",
				TL_GetConnectionInfoStr(Conn));
		Platform_Unlock(EXPORTER_PLATFORM);
		return ;
	}

	IC = (IPDRConnection *)Conn->Owner;

	if(IC->ConnectResponseTimerID != NULL)
		Platform_UnregisterTimerAndClear(EXPORTER_PLATFORM, & (IC->ConnectResponseTimerID));

	Platform_RegisterTimer(EXPORTER_PLATFORM,
    						ExporterProtocol_WaitConnectTimeoutCallback,
							IC,
                            E->Parameters.ConnectResponseTimeout,
                            & (IC->ConnectResponseTimerID));

	ExporterProtocol_ConnectCallback(Conn, Addr, Port, Arg);

	Platform_Unlock(EXPORTER_PLATFORM);

	return ;
}

static void
ExporterProtocol_WaitConnectTimeoutCallback(void *Arg)
{
	IPDRConnection *IC = (IPDRConnection *)Arg ;
	Collector *Coll = (Collector *)IC->Owner;
	Exporter *E = Coll->ExporterPtr;

	Platform_Lock(EXPORTER_PLATFORM);

	if(IC->ConnectResponseTimerID != NULL)
		Platform_UnregisterTimerAndClear(EXPORTER_PLATFORM, & (IC->ConnectResponseTimerID));

	Log_Write1(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_ERROR, "ExporterProtocol_WaitConnectTimeoutCallback",
				MSG_EXPORTER_NO_CONNECT_MESSAGE, Coll->Name);

	/*if( IC->TCPConnection == NULL )
	{
		Log_Write1(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_ERROR, "ExporterProtocol_WaitConnectTimeoutCallback",
				MSG_EXPORTER__RESET_TIMER_NEEDED, Coll->Name);
		Platform_Unlock(EXPORTER_PLATFORM);
		return;
	}*/

	Object_Release(EXPORTER_PLATFORM, Coll);

	Platform_Unlock(EXPORTER_PLATFORM);
}


/**
 * Callback invoked when a server (collector) disconnects.
 * @param Conn connected connection
 * @param Arg  callback argument (Exporter)
 */
static void
Exporter_DisconnectCallback(Connection Conn,
                          void *Arg)
{
    Exporter *E = (Exporter*)Arg;

    Log_StateTransition(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_DEBUG,
                        "Exporter_DisconnectCallback", Conn,
                        IPDR_CS_DISCONNECTED, IPDR_CS_DISCONNECTED,
                        TL_GetConnectionInfoStr(Conn));

	if( Conn->Owner != NULL )
		ExporterProtocol_DisconnectCallback(Conn, E);
}


/**
 * Callback invoked when data arrives on a connection or no data
 * arrives within the timeout.
 *
 * @param Conn   connected connection
 * @param BH     received buffer (NULL if timeout)
 * @param MsgLen length of message in the buffer (0 if timeout)
 * @param Arg    callback argument (Exporter)
 */
static void
Exporter_DataCallback(Connection Conn,
                    BufferHandle *BH,
                    unsigned int MsgLen,
                    void *Arg)
{
	Exporter 		*E = (Exporter *) Arg;
	Platform_Lock(EXPORTER_PLATFORM);
	ExporterProtocol_DataCallback(Conn, BH, MsgLen, Arg);
	Platform_Unlock(EXPORTER_PLATFORM);
	return ;
}

/**
 * Callback invoked upon data timeout with no ACK.
 *
 * @param Arg callback argument (Server)
 */
void
Exporter_WindowSizeTimerCallback(void *Arg)
{
    CollectorSession *CS = (CollectorSession *) Arg;
    Session *S = CS->SessionPtr;
    Exporter *E = S->ExporterPtr;
	IPDRConnection *IC = CS->SessionConnection;
	Connection Conn = NULL;
    IPDR_EventData eventData;

	Platform_Lock(EXPORTER_PLATFORM);

	if(IC != NULL)
	{
		Conn = IC->TCPConnection;
	}

#if defined(DEBUG_PRINTF) && defined(_VXWORKS_DEBUG)
    countTick =  tickGet ();
    printf("\n window size timer callback , ticks [%ld]", countTick);
#endif

    Platform_UnregisterTimerAndClear(EXPORTER_PLATFORM, &CS->WindowSizeTimer);

    EventDataInit(&eventData, IPDR_EVENT_SERVER_DISCONNECTED, E, CS, S);
    eventData.Extra.Disconnected.Reason = IPDR_EVENT_DISCONNECTED_TIMEOUT;
    eventData.Extra.Disconnected.TimeoutMillisec = E->Parameters.WindowTimeout;

    if (IC == NULL || CS->CSState == IPDR_CS_DISCONNECTED) {
        eventData.IC = NULL;
        Exporter_TriggerEvent(E, IPDR_LEVEL_WARNING, &eventData, CS,
                            "Acknowledgment timeout (%d msec): Collector=none, outstanding=%lu, unACKed=%lu, Session=%u [%.40s] (%.80s)",
                            E->Parameters.WindowTimeout,
                            EXPORTER_QHANDLER->OutstandingDataCount(E, S),
                            EXPORTER_QHANDLER->UnacknowledgedDataCount(E, S),
                            S->ID, S->Name, S->Description);
    } else {
        eventData.IC = IC;
        Exporter_TriggerEvent(E, IPDR_LEVEL_WARNING, &eventData, CS,
                            "Acknowledgment timeout (%d msec): %.98s, outstanding=%lu, unACKed=%lu, Session=%u [%.40s] (%.80s)",
                            E->Parameters.WindowTimeout,
                            TL_GetConnectionInfoStr(Conn),
                            EXPORTER_QHANDLER->OutstandingDataCount(E, S),
                            EXPORTER_QHANDLER->UnacknowledgedDataCount(E, S),
                            S->ID, S->Name, S->Description);
    }
#ifdef OBSOLETE
    switch (CS->CSState) {
    case IPDR_CS_ACTIVE: /* shouldn't happen, but let's be cautious */
        Log_StateTransition(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO,
                            "Exporter_WindowSizeTimerCallback", Conn, CS->CSState, IPDR_SERVER_PAUSED,
                            "acknolwedgment timeout");
        CS->CSState = IPDR_CS_READY;
        break;
    default:
        break;
    }
#endif
    Platform_Unlock(EXPORTER_PLATFORM);
}

/**
 * Create a new work context within a session, which will
 * be used to queue new records from now on.
 *
 * @param E Exporter owning the session
 * @param S Session
 * @param Tmpls templates configuration associated with the
 *           context
 * @param Negotiable indicates whether templates negotiation
 *                can take place on the session
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static IPDR_Status
Session_CreateNewWorkContext(Exporter *E,
                             Session *S,
                             DynamicArray *Tmpls,
                             IPDR_BOOL Negotiable)
{
    IPDR_Status Status;
    TransmissionContext *TC;

    Status = Object_Allocate(EXPORTER_PLATFORM, sizeof(TransmissionContext),
                             TransmissionContext_Destructor,
                             (void **) & TC);
    if (FAILURE(Status))
        return Status;
    strncpy(TC->eyecatcher, "XMTCTX", sizeof TC->eyecatcher);

    Status = DynamicArray_Copy(EXPORTER_PLATFORM, Tmpls, & (TC->Templates));
    if (FAILURE(Status)) {
        Object_Release(EXPORTER_PLATFORM, TC);
        return Status;
    }

    Status = EXPORTER_QHANDLER->CreateTransmissionContext(E, S, TC);
    if (FAILURE(Status)) {
        Object_Release(EXPORTER_PLATFORM, TC);
        return Status;
    }

    TC->SessionPtr = S;
    TC->NextContext = NULL;
    TC->Negotiable = Negotiable;

    if (S->WorkContext == NULL) {
		unsigned short newConfigID = 1;
		if(E->NewConfigurationIDCallback != NULL)
			E->NewConfigurationIDCallback(S->ID, &newConfigID, E->NewConfigurationIDCallbackArg);
        S->WorkContext = TC;
        TC->ConfigurationID = newConfigID;
    } else {
        TC->ConfigurationID = S->WorkContext->ConfigurationID + 1;
        if (TC->ConfigurationID == 0)
            TC->ConfigurationID++;
		if(E->ConfigurationIDChangedCallback != NULL)
			E->ConfigurationIDChangedCallback(S->ID, TC->ConfigurationID, E->ConfigurationIDChangedCallbackArg);
        S->WorkContext->NextContext = TC;
        S->WorkContext = TC;
    }

    /*if (S->ActiveContext == NULL)*/
	S->ActiveContext = S->WorkContext;

    return IPDR_OK;
}


/**
 * Select a new active server for a given session.
 *
 * @param E Exporter owning the session
 * @param S Session
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
Session_SelectActiveCollector(Exporter *E, Session *S)
{

    return IPDR_OK;
}

/**
 * Apply template configuration changes to a session given an array of
 * template change blocks (as defined in the IPDR protocol).
 *
 * @param E Exporter owning the session
 * @param S Session
 * @param NumChanges number of template change blocks
 * @param TCBs array of template change blocks
 * @param Force if true, a new context will be created even when
 *           no changes where supplied
 * @param CopyFromExporter  the existing configuration should be
 *                    based on the client templates
 * @param Negotiable indicates whether templates negotiation
 *                can take place on the session
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
Session_HandleTemplateChanges(Exporter *E, Session *S,
                              int NumChanges,
                              unsigned char *TCBs,
                              IPDR_BOOL Force,
                              IPDR_BOOL CopyFromExporter,
                              IPDR_BOOL Negotiable)
{
#ifdef NOTDONEYET
    TransmissionContext*	TC = S->WorkContext;
    int 					i, j;
    unsigned short 			TemplateID;
    Template 				*Tmpl, *NewTmpl, *Tmpl1;
    IPDR_BOOL 				TemplateChanged = IPDR_FALSE;
    IPDR_Status 			Status;
    DynamicArray 			Tmpls, *Source;
    const KeyBlock *		KB;
    Key *					K;
    size_t 					BlockLength;

	/*const TemplateChangeBlockHeader *TCB = TCBs;*/

    if (! CopyFromExporter)
        Source = & (TC->Templates);
    else
        Source = & (S->Templates);

    /* check if something actually changed compared to the
       current configuration */

    if (! Force) {
        for (i = 0; i < NumChanges; i++) {

            TemplateID = IPDR_NTOHS(TCB->TemplateID);

            Tmpl = NULL;
            for (j = 0; j < Source->Length; j++) {
                Tmpl1 = (Template *) Source->Data[j];
                if (Tmpl1 != NULL && Tmpl1->TemplateID == TemplateID) {
                    Tmpl = Tmpl1;
                    break;
                }
            }

            if (Tmpl == NULL)
                continue;

            KB = (const KeyBlock *) ((const char *) TCB + sizeof(TemplateChangeBlockHeader));
            Template_IsModified(Tmpl, IPDR_NTOHS(TCB->NumKeys), KB, & TemplateChanged);

            if (TemplateChanged) {
                TemplateChanged = IPDR_TRUE;
                break ;
            }

            BlockLength =
                sizeof(TemplateChangeBlockHeader) +
                IPDR_NTOHS(TCB->NumKeys) * sizeof(KeyBlock);
            TCB = (const TemplateChangeBlockHeader *) ((const char *) TCB + BlockLength);
        }

        /* if nothing changed in the templates configuration, return */
        if (! TemplateChanged)
            return IPDR_OK;
    }

    Status = DynamicArray_Create(EXPORTER_PLATFORM, & Tmpls, 0);
    if (FAILURE(Status))
        return Status;
    Status = DynamicArray_Copy(EXPORTER_PLATFORM, Source, & Tmpls);
    if (FAILURE(Status))
        return Status;

    TCB = TCBs;

    /* perform modifications to the needed keys */
    for (i = 0; i < NumChanges; i++) {

        TemplateID = IPDR_NTOHS(TCB->TemplateID);

        Tmpl = NULL;
        for (j = 0; j < Source->Length; j++) {
            Tmpl1 = (Template *) Source->Data[j];
            if (Tmpl1 != NULL && Tmpl1->TemplateID == TemplateID) {
                Tmpl = Tmpl1;
                break;
            }
        }

        if (Tmpl != NULL) {
            KB = (const KeyBlock *) ((const char *) TCB + sizeof(TemplateChangeBlockHeader));
            Template_ModifyTemplate(S, Tmpl, IPDR_NTOHS(TCB->NumKeys), KB, & NewTmpl);
            Tmpls.Data[j] = NewTmpl;
            Object_Release(EXPORTER_PLATFORM, Tmpl);
        }

        BlockLength =
            sizeof(TemplateChangeBlockHeader) +
            IPDR_NTOHS(TCB->NumKeys) * sizeof(KeyBlock);
        TCB = (const TemplateChangeBlockHeader *) ((const char *) TCB + BlockLength);
    }

    Log_Write1(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_INFO, "Session_HandleTemplateChanged",
               MSG_EXPORTER_SESSION_TEMPLATES_CHANGED, S->Name);
    for (i = 0; i < Tmpls.Length; i++) {
        Tmpl = (Template *) Tmpls.Data[i];
        if (Tmpl == NULL)
            continue;
        Log_Write2(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_INFO, "Session_HandleTemplateChanged",
                   MSG_EXPORTER_TEMPLATE_INFO, Tmpl->TemplateID, Tmpl->SchemaName);
        for (j = 0; j < Tmpl->Keys.Length; j++) {
            K = (Key *) Tmpl->Keys.Data[j];
            Log_Write3(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_DEBUG, "Session_HandleTemplateChanged",
                       MSG_EXPORTER_KEY_INFO, K->KeyID, K->Name, K->Enabled);
        }
    }

    Status = Session_CreateNewWorkContext(E, S, & Tmpls, Negotiable);
    if (FAILURE(Status)) {
        DynamicArray_Delete(EXPORTER_PLATFORM, & Tmpls);
        return Status;
    }

    /* trigger the templates change callback */
    Exporter_TriggeTemplateChangedCallback(S);

    DynamicArray_Delete(EXPORTER_PLATFORM, & Tmpls);
#endif
    return IPDR_OK;
}


/**
 * Send any outstanding (unacknowledged) data within a session.
 *
 * @param E Exporter owning the session
 * @param S Session
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
Session_SendOutstandingData(Exporter *E,
                            Session *S)
{
    /* check if we've recursively been called. If we did don't do anything
       since the caller will continue sending */
	if( S->Running == IPDR_FALSE)
		return IPDR_ERR_SESSION_LOGICALLY_STOPED;

    if (S->SendingData)
        return IPDR_OK;

    S->SendingData = IPDR_TRUE;
    EXPORTER_QHANDLER->SendOutstandingData(E, S);
    S->SendingData = IPDR_FALSE;

    return IPDR_OK;
}


IPDR_Status
Exporter_InitDebugLevels(Exporter *E)
{
	Log_SetModuleMoreDebugLevel(& (E->ExporterLog), "Exporter",
								ParametersHandler_GetUShortParam(& (E->ParamsHandler),
								STR_configuration_logging_levels_moduleExporter));

    Log_SetModuleMoreDebugLevel(& (E->ExporterLog), "Platform",
								ParametersHandler_GetUShortParam(& (E->ParamsHandler),
								STR_configuration_logging_levels_modulePlatform));

    Log_SetModuleMoreDebugLevel(& (E->ExporterLog), "TCP",
								ParametersHandler_GetUShortParam(& (E->ParamsHandler),
								STR_configuration_logging_levels_moduleTCP));

    Log_SetModuleMoreDebugLevel(& (E->ExporterLog), "Host",
								ParametersHandler_GetUShortParam(& (E->ParamsHandler),
								STR_configuration_logging_levels_moduleHost));

    Log_SetModuleMoreDebugLevel(& (E->ExporterLog), "Queue",
								ParametersHandler_GetUShortParam(& (E->ParamsHandler),
								STR_configuration_logging_levels_moduleQueue));

    Log_SetModuleMoreDebugLevel(& (E->ExporterLog), "Memory",
								ParametersHandler_GetUShortParam(& (E->ParamsHandler),
								STR_configuration_logging_levels_moduleMemory));

    Log_SetModuleMoreDebugLevel(& (E->ExporterLog), "Protocol",
								ParametersHandler_GetUShortParam(& (E->ParamsHandler),
								STR_configuration_logging_levels_moduleProtocol));

    Log_SetModuleMoreDebugLevel(& (E->ExporterLog), "JNI",
								ParametersHandler_GetUShortParam(& (E->ParamsHandler),
								STR_configuration_logging_levels_moduleJNI)); // only on server

    Log_SetModuleMoreDebugLevel(& (E->ExporterLog), "SERVER",
								ParametersHandler_GetUShortParam(& (E->ParamsHandler),
								STR_configuration_logging_levels_moduleSERVER)); // only on server
	return IPDR_OK;
}

/*
 **********************
 * IPDR API function *
 **********************
*/
IPDR_Status
IPDR_InitExporter(IPDR_PlatformServices *PS,
                 const char *ExporterName,
                 IPDR_ParametersHandlerFunc ParamsHandler,
                 void *ParamsHandlerArg,
                 IPDR_LoadMIBFunc LoadMIB,
                 void *LoadMIBArg,
                 IPDR_SaveMIBFunc SaveMIB,
                 void *SaveMIBArg,
                 Exporter **CH)
{
    IPDR_Status Status;
    Exporter *E;
    const char *LocalAddr;
	IPDR_IPAddress IPAddr;
	unsigned short IPPort;

    if (! PS || ! CH) {
        return IPDR_ERR_INVALID_VALUE;
    }

    *CH = NULL;

    Templates_Initialize();

    Status = Object_Allocate(PS, sizeof(Exporter), Exporter_Destructor,
                             (void **) & E);
    if (FAILURE(Status))
    {
        return Status;
    }

	memset(E, 0, sizeof(Exporter));

    strncpy(E->eyecatcher, "EXPORTER ", sizeof E->eyecatcher);

    if (ExporterName == NULL || strlen(ExporterName) == 0) {
        ExporterName = "IPDR";
    }

    Platform_GetTime(PS, & E->ExporterStartTime); /* Needed for START_ACK */

    Log_Initialize(& (E->ExporterLog), PS, ExporterName); /* This also makes a copy of ExporterName */
    ExporterMsgs_Initialize(& (E->ExporterLog));
    CommonMsgs_Initialize(& (E->ExporterLog));
    TCPMsgs_Initialize(& (E->ExporterLog));
    Log_Enable(& (E->ExporterLog));

    {
        char buf1[IPDR_TIME_FORMAT_LENGTH + 1];
        char buf2[IPDR_TIME_FORMAT_LENGTH + 1];
        Log_Write4(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_INFO,
                   "IPDR_InitExporter", MSG_EXPORTER_STARTED,
                   E->ExporterStartTime, E->ExporterStartTime,
                   Platform_FormatTimeLocal(PS, E->ExporterStartTime, buf1),
                   Platform_FormatTimeGMT(PS, E->ExporterStartTime, buf2));
    }

    E->PS = PS;
    E->ParamsHandler.ParamsFunc = ParamsHandler;
    E->ParamsHandler.ParamsArg = ParamsHandlerArg;

#ifdef VQ_HANDLER_SUPPORT
    E->QHandler = & VQHandler;
    E->QHandlerInitArg = NULL;
#else
    E->QHandler = NULL;
#endif

    E->CSessions = NULL;

	E->IsInsideCallback = IPDR_FALSE;

    *CH = E; /* So that IPDR_ParamsHandler can use it for looking at the MIB contents. */

	ParametersHandler_ParametrsInit(E, & (E->ParamsHandler));

	Exporter_InitDebugLevels( E );

    LocalAddr = ParametersHandler_GetStringParam(& (E->ParamsHandler),
                                                 STR_configuration_logging_loggerAddress);
    if (LocalAddr == NULL || strlen(LocalAddr) == 0)
        IPAddr = IPDR_DEFAULT_ADDRESS;
    else
        IPAddr = TL_ParseAddress(LocalAddr);

	IPPort = ParametersHandler_GetUShortParam(& (E->ParamsHandler),
                                                 STR_configuration_logging_loggerPort);

	/* #define SYSLOG_FACILITY 184 */
	Log_SetLoggerAddress(EXPORTER_LOG, IPAddr, IPPort, 184, "");

    Platform_SetExporterHandle(PS, E); /* So that Platform can write to log. */

#if defined(SNMP_SUPPORT)
	Platform_MIB_SetExporterHandle(PS, E);
#endif

    Status = DynamicArray_Create(PS, & (E->Sessions),
                                 INITIAL_SESSIONS_ARRAY_CAPACITY);
    if (FAILURE(Status)) {
        Platform_UnsetExporterHandle(PS);
        *CH = NULL;
        Object_Release(PS, E);
        return Status;
    }
    /*Status = DynamicArray_Create(PS, & (S->Templates),
                                 INITIAL_TEMPLATES_ARRAY_CAPACITY);
	*/
    if (FAILURE(Status)) {
        Platform_UnsetExporterHandle(PS);
        *CH = NULL;
        Object_Release(PS, E);
        return Status;
    }
    LocalAddr = ParametersHandler_GetStringParam(& (E->ParamsHandler),
                                                 STR_configuration_exporterAddress);
    if (LocalAddr == NULL || strlen(LocalAddr) == 0)
        E->LocalAddr = IPDR_DEFAULT_ADDRESS;
    else
        E->LocalAddr = TL_ParseAddress(LocalAddr);

    E->LocalPort =
        (unsigned short) ParametersHandler_GetIntParam(& (E->ParamsHandler),
                                                       STR_configuration_exporterPort);

    Exporter_InitializeParameters(E);

    Console_Initialize(PS, & (E->ConsolePtr), & (E->ParamsHandler),
                       & (E->ExporterLog));
    Log_RegisterConsoleCommands(& (E->ExporterLog), & (E->ConsolePtr));
    ExporterConsole_RegisterConsoleCommands(E, & (E->ConsolePtr));
#ifdef ENABLE_SIMULATOR
    HApp_RegisterConsoleCommands(E, & (E->ConsolePtr));
#endif

    E->Started = IPDR_FALSE;
    E->TemplateChangedCallback = NULL;
    E->FlowStartCallback = NULL;
	E->StartNegotiationCallback = NULL;
	E->NewConfigurationIDCallback = NULL;
	E->ConfigurationIDChangedCallback = NULL;

    *CH = E; /* Should already be set, but let's be sure. */

    /* The following will be optimized away by any decent compiler. It
     * is here because there are too many dependencies in the code on
     * the sizes of short, int, long ... when these are removed, this
     * test can be removed. */

    if (sizeof (signed char)        != 1 ||
        sizeof (unsigned char)      != 1 ||
        sizeof (short signed int)   != 2 ||
        sizeof (short unsigned int) != 2 ||
        sizeof (signed int)         != 4 ||
        sizeof (unsigned int)       != 4 ||
        sizeof (signed long)        != 4 ||
        sizeof (unsigned long)      != 4 ||
		sizeof (signed long long) 	!= 8 ||
		sizeof (unsigned long long) != 8 ||
		sizeof (IPDR_IPAddress)		!= 4 ||
        sizeof (ProtocolUnsigned8)  != 1 ||
        sizeof (ProtocolSigned8)    != 1 ||
        sizeof (ProtocolSigned16)   != 2 ||
        sizeof (ProtocolUnsigned16) != 2 ||
        sizeof (ProtocolSigned32)   != 4 ||
        sizeof (ProtocolUnsigned32) != 4 ||
		sizeof (ProtocolSigned64) 	!= 8 ||
		sizeof (ProtocolUnsigned64)	!= 8) {
#if defined(STD_FILE_OPERATIONS_SUPPORT)
        fprintf(stderr, "*** size of char/short/int/long not what was expected: %u/%u/%u/%u at %.200s:%d\n",
                sizeof (char), sizeof (short), sizeof (int), sizeof (long), __FILE__, __LINE__);
#endif
        Log_Write5(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_FATAL,
                   "IPDR_InitExporter",
                   "size of char/short/int/long/long long - not what was expected: %u/%u/%u/%u/%u",
                   sizeof (char), sizeof (short), sizeof (int), sizeof (long), sizeof(long long));
        abort();
        return IPDR_ERR;
    }

    return IPDR_OK;
}


/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_StartExporter(IPDR_ExporterHandle ExporterHandle)
{
    Exporter*		E = ExporterHandle;
    IPDR_Status 	Status;
    ChunkAllocator 	Alloc;
	int 			i, j ;

    if (! E) {
        return IPDR_ERR_INVALID_VALUE;
    }

    if (Platform_IsRunning(EXPORTER_PLATFORM)) {
        return IPDR_ERR_ALREADY_RUNNING;
    }


    EXPORTER_QHANDLER->Initialize(E, E->QHandlerInitArg);

    Console_Start(& (E->ConsolePtr));

    ChunksManager_GetDefaultChunkAllocator(E->PS, & Alloc);

    /* initialize the transport layer */
    Status = TL_InitializePassive(E->PS, & (E->ExporterLog), Alloc, E->LocalAddr,
                                  E->LocalPort,
                                  & Exporter_DefaultHandlers, E,
                                  & (E->ParamsHandler),
                                  & (E->TransportHandle));
    if (FAILURE(Status)) {
        return Status;
    }

    /* copy the templates lists to the sessions */
    for (i = 0; i < E->Sessions.Length; i++) {
        Session *S = (Session *) E->Sessions.Data[i];

    	for (j = 0; j < S->Templates.Length; j++) {
	    	/* reset all templates associated with the Session */
			Template *Tmpl = (Template *) S->Templates.Data[j];
			if (Tmpl != NULL)
				Template_Reset(S, Tmpl);
		}
        Session_CreateNewWorkContext(E, S, & (S->Templates), IPDR_TRUE);
    }

    /* once the sessions and servers (collectors) are updated, the rest of the
       tables can be updated once */

    if (E->Parameters.StatisticsInterval) {
        Status =
            Platform_RegisterPeriodicTimer(EXPORTER_PLATFORM,
                                           Exporter_StatisticsReportCallback, E,
                                           E->Parameters.StatisticsInterval,
                                           & (E->StatisticsTimerID));
		if (FAILURE(Status))
		{
            return Status;
        }
    }

	if(E->Parameters.AutoconnectInterval)
	{
		Status = Platform_RegisterPeriodicTimer(EXPORTER_PLATFORM,
        	                                   Exporter_AutoconnectCallback, E,
            	                               E->Parameters.AutoconnectInterval,
                	                           & (E->AutoconnectTimerID));
		if( FAILURE(Status) )
		{
			return Status;
		}
	}

    Status = Exporter_InitializeProtocolInfo(E);
    if (FAILURE(Status))
    {
    	return Status;
    }

    /* start the platform services */
    Status = Platform_Start(EXPORTER_PLATFORM);

    if (SUCCESS(Status)) {
        E->Started = IPDR_TRUE;
		/*IPDR_SleepMS(E, 10*1000);*/
    }

	{
		int Index ;
		for( Index = 0 ; Index < E->Sessions.Length ; Index++ )
		{
			Session *S = (Session *)E->Sessions.Data[Index];
			if( S != NULL )
			{
				S->Running = IPDR_TRUE;
			}
		}
	}

    return Status;
}


/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_BOOL
IPDR_IsRunning(IPDR_ExporterHandle ExporterHandle)
{
    Exporter *E = ExporterHandle;
    if (! E) {
        return IPDR_FALSE;
    }
    return Platform_IsRunning(EXPORTER_PLATFORM);
}


/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_DeleteExporter(IPDR_ExporterHandle ExporterHandle)
{
    Exporter *E = ExporterHandle;
    IPDR_PlatformServices *PS;

    if (! E) {
        return IPDR_ERR_INVALID_VALUE;
    }

    PS = EXPORTER_PLATFORM;

    /* We must *not* grab the lock in this case ... otherwise we get a
     * deadlock because the main Platform code needs to also grab
     * locks. However, we want to ensure that any other IPDR_xxx()
     * function is out of the way (e.g., if there's a IPDR_SendData()
     * active, let's do our best to let it finish first). */

#if defined(SNMP_SUPPORT)
	Platform_MIB_UnSetExporterHandle(PS);
#endif

    Platform_Lock(PS);
    /* There's a very good chance that no IPDR_xxx() function is
     * operating now (however, the lock is released around the
     * select(), so we just have to hope that nothing happens there,
     * which is a pretty good bet. Anyway, we're going down, so we can
     * absorb a 0.00001% chance of going up in a puff of smoke. */
    Platform_Unlock(PS);

	Platform_Stop(PS);

	Platform_UnsetExporterHandle(PS);

    /* We need to release the lock before freeing the memory. If
     *  somnebody tries to do something with the platform at this
     *  point, they'll die in a puff of smoke anyway. But we're
     *  guaranteed that nobody is using the memory, so it's safe. */

	ParametersHandler_ParametrsDeInit(E);

	Object_Release(PS, E);

    return IPDR_OK;
}


/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_DrainQueue(IPDR_ExporterHandle ExporterHandle,
                 IPDR_SessionHandle SessionHandle,
                 IPDR_Time Timeout)
{
    Exporter *E = ExporterHandle;
    Session *S = SessionHandle;
    IPDR_Time TimeLeft = Timeout;
    IPDR_Time SleepInterval = 1000; /* 1 second */

    if (! E || ! S) {
        return IPDR_ERR_INVALID_VALUE;
    }

    if (! Platform_IsRunning(EXPORTER_PLATFORM)) {
        return IPDR_ERR_NOT_RUNNING;
    }

	Session_SendOutstandingData(E, S);

    if (Timeout > 0 && Timeout < SleepInterval) {
        SleepInterval = Timeout;
    }

    /* This code is a little crude; we ought to use the platform timer
     * facilities for more precise counting down; but that's more
     * complex with callbacks, etc. */

    while (Timeout == 0 || TimeLeft > 0) {
        Platform_Lock(EXPORTER_PLATFORM);
        if (EXPORTER_QHANDLER->OutstandingDataCount(E, S) == 0 &&
            EXPORTER_QHANDLER->UnacknowledgedDataCount(E, S) == 0) {
            Platform_Unlock(EXPORTER_PLATFORM);
            return IPDR_OK;
        }
        Platform_Unlock(EXPORTER_PLATFORM);

        Platform_SleepMS(EXPORTER_PLATFORM, SleepInterval);
        TimeLeft -= SleepInterval;
    }
	EXPORTER_QHANDLER->CleanUP(E, S);
    return IPDR_ERR_OPERATION_WOULD_BLOCK;
}


/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_CreateSession(IPDR_ExporterHandle ExporterHandle,
                    const char *Name, unsigned char ID,
                    const char *Description,
                    IPDR_SessionHandle* SH)
{
    Exporter *E = ExporterHandle;

    return Exporter_CreateSessionInternal(E, Name, ID, Description, SH);
}


/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_DeleteSession(IPDR_ExporterHandle ExporterHandle,
                    IPDR_SessionHandle SessionHandle)
{
    Exporter *E = ExporterHandle;
    Session* S = SessionHandle;

    return Exporter_DeleteSessionInternal(E, S);
}

/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_API
IPDR_SetDocumentID( IPDR_SessionHandle SessionHandle, const char* DocumentID )
{
	Session* S = SessionHandle;
	Exporter* E = S->ExporterPtr;

	if(S->ActiveCS != NULL || S->Running == IPDR_TRUE)
	{
		Log_Write2(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_ERROR,
			   "IPDR_SetDocumentID", MSG_EXPORTER_ACTIVE_COLLECTOR_EXISTS, S->ID, S->Name);
		return IPDR_ERR_SESSION_IS_RUNNING;
	}
	memset(S->DocumentID, 0, sizeof(S->DocumentID));
	memcpy(S->DocumentID, DocumentID, sizeof(S->DocumentID));
	S->DSN = 0;
	Log_Write1(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_INFO,
			   "IPDR_SetDocumentID", "DocumentID Changed, SessionID: <%u>", S->ID);
	return IPDR_OK;
}

/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_API
IPDR_SetSessionNegotiable( 	IPDR_SessionHandle SessionHandle,
							IPDR_BOOL Negotiable )
{
	Session* S = SessionHandle;
	Exporter *E = S->ExporterPtr;

	Platform_Lock(EXPORTER_PLATFORM);

	if(S->ActiveCS != NULL || S->Running == IPDR_TRUE)
	{
		Log_Write2(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_INFO,
			   "IPDR_SetSessionNegotiable", MSG_EXPORTER_ACTIVE_COLLECTOR_EXISTS, S->ID, S->Name);
		Platform_Unlock(EXPORTER_PLATFORM);
		return IPDR_ERR_SESSION_IS_RUNNING;
	}

	S->Negotiable = Negotiable ;

	Platform_Unlock(EXPORTER_PLATFORM);
	return IPDR_OK;
}

/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_API
IPDR_StartSession( 	IPDR_SessionHandle SessionHandle,
					IPDR_BOOL Negotiate )
{
	Session *S = SessionHandle;
	Exporter *E = S->ExporterPtr;

	S->Running = IPDR_TRUE;

	if(S->ActiveCS != NULL)
	{
		Log_Write2(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_INFO,
			   "IPDR_StartSession", MSG_EXPORTER_ACTIVE_COLLECTOR_EXISTS, S->ID, S->Name);
		return IPDR_ERR_SESSION_LOGICALLY_RUN;
	}

	return ExporterProtocol_SelectCollector( E, S, Negotiate, NULL);
}

/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_API
IPDR_StopSession( 	IPDR_SessionHandle SessionHandle,
					enum IPDR_SessionStopReason Reason,
                 	IPDR_Time Timeout )
{
	Session *S = SessionHandle;
	Exporter *E = S->ExporterPtr;
	CollectorSession *CS = NULL;
	Collector *C = NULL;
	Connection Conn = NULL;
	IPDR_Status Status;

	Platform_Lock(EXPORTER_PLATFORM);

	S->Running = IPDR_FALSE;

	if(S->ActiveCS == NULL)
	{
		Log_Write2(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_INFO,
			   "IPDR_StopSession", MSG_EXPORTER_NO_ACTIVE_COLLECTOR, S->ID, S->Name);

		Platform_Unlock(EXPORTER_PLATFORM);

		EXPORTER_QHANDLER->CleanUP(E, S);

		return IPDR_ERR_SESSION_LOGICALLY_STOPED;
	}

	CS = S->ActiveCS;

	if(	CS->Collector == NULL ||
		CS->SessionConnection == NULL ||
		CS->SessionConnection->TCPConnection == NULL)
	{
		Log_Write2(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_INFO,
			   "IPDR_StopSession", MSG_EXPORTER_NO_ACTIVE_COLLECTOR, S->ID, S->Name);
		Platform_Unlock(EXPORTER_PLATFORM);

		EXPORTER_QHANDLER->CleanUP(E, S);

		return IPDR_ERR_SESSION_LOGICALLY_STOPED;
	}

	Platform_Unlock(EXPORTER_PLATFORM);
	Status = IPDR_DrainQueue(E, S, Timeout);
	Platform_Lock(EXPORTER_PLATFORM);
	if(FAILURE(Status))
	{
		Log_Write(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_INFO,
			   "IPDR_StopSession", MSG_EXPORTER_QUEUE_NOT_DRAINED);
	}
	/*TODO: create reason codes and descriptions for session stop */
	C = CS->Collector;
	if(CS->SessionConnection != NULL && CS->SessionConnection->TCPConnection  != NULL)
	{
		Conn = CS->SessionConnection->TCPConnection;

		Status = ExporterProtocol_SendSessionStop(E, S, C, Conn, Reason, "API CALL");
		if(FAILURE(Status))
			CS->CSState = IPDR_CS_DISCONNECTED;
		else
			CS->CSState = IPDR_CS_READY;
	}
	else
	{
		CS->CSState = IPDR_CS_DISCONNECTED;
	}

	S->ActiveCS = NULL;

	Platform_Unlock(EXPORTER_PLATFORM);
	return IPDR_OK;
}

/*
 **********************
 * IPDR API function (CRANE Compat) *
 **********************
 */
IPDR_API
IPDR_AddCollectorCompat(IPDR_ExporterHandle ExporterHandle,
						IPDR_SessionHandle SessionHandle,
						const char *Name,
						IPDR_IPAddress Addr,
						unsigned short Port,
						int Priority)
{
	IPDR_CollectorHandle CollectorHandle;

	IPDR_Status Status ;
	Status = IPDR_CreateCollector(ExporterHandle,Name,Addr,Port,0, &CollectorHandle);
	if(FAILURE(Status) || CollectorHandle == NULL)
	{
		return Status ;
	}

	/*Status = IPDR_GetCollector(ExporterHandle, Name, &CollectorHandle, NULL, NULL, NULL);
	if(FAILURE(Status))
	{
		return Status ;
	}*/

	Status = IPDR_AssocCollectorSession(ExporterHandle,SessionHandle,CollectorHandle,Priority);
	if(FAILURE(Status))
	{
		return Status ;
	}
	return IPDR_OK;
}

/*
 **********************
 * CRANE API function *
 **********************
 */
IPDR_API
IPDR_GetServerStatisticsAndState(IPDR_ExporterHandle ExporterHandle,
                                  IPDR_SessionHandle SessionHandle,
                                  IPDR_IPAddress Addr,
                                  unsigned short  Port,
                                  IPDR_Time *Now,
                                  IPDR_Statistics *Statistics,
                                  IPDR_EventData* LastEvent)
{
    Exporter *E = ExporterHandle;
    Session *S = SessionHandle;
    int Index;
    static const IPDR_Statistics zeroStats = { 0, 0, 0, 0 };
    if (Statistics) { *Statistics = zeroStats; }
    if (Now) { * Now = 0; }
    if (LastEvent)
	{ /* Set a default value in case we don't have a current server (collector) */
        EventDataInit(LastEvent, IPDR_EVENT_SERVER_DISCONNECTED, NULL, NULL, NULL);
        LastEvent->Extra.Disconnected.Reason = IPDR_EVENT_DISCONNECTED_INITIAL;
    }
    if (! E || ! S)
	{
        return IPDR_ERR_INVALID_VALUE;
    }
    Platform_Lock(EXPORTER_PLATFORM);

    if (Now)
	{
        Platform_GetTime(EXPORTER_PLATFORM, Now);
    }

    for (Index = 0; Index < S->CollectorSessions.Length; Index++)
	{
        CollectorSession *CS = (CollectorSession *) S->CollectorSessions.Data[Index];
		if(CS != NULL && CS->Collector != NULL)
		{
			Collector *C = CS->Collector;

			if (C->Addr == Addr && C->Port == Port)
			{
				if (Statistics) {
					*Statistics = CS->Statistics;
				}
				if (LastEvent) {
					*LastEvent = CS->LastEvent;
				}
				Platform_Unlock(EXPORTER_PLATFORM);
				return IPDR_OK;
			}
		}
    }
    Platform_Unlock(EXPORTER_PLATFORM);
    StatisticsInitialize(EXPORTER_PLATFORM, NULL, NULL, Statistics); /* zero return value */
    return IPDR_ERR_COLLECTOR_NOT_FOUND;
}

CRANE_Status CRANE_GetTemplate(CRANE_ClientHandle ClientHandle,
			       int TemplateID,
			       CRANE_TemplateHandle *TH)
{
	fprintf(stderr,"CRANE_GetTemplate NOT IMPLEMENTED");
	return IPDR_ERR_NOT_IMPLEMENTED;
}

CRANE_Status CRANE_CreateStatusTemplate(CRANE_ClientHandle ClientHandle,
		                        int TemplateID,
				        const char *Description,
				        int NumKeys,
				        const CRANE_Key *Keys,
					CRANE_TemplateHandle *TH)
{
	fprintf(stderr,"CRANE_CreateStatusTemplate NOT IMPLEMENTED");
	return IPDR_ERR_NOT_IMPLEMENTED;
}

CRANE_Status CRANE_DeleteTemplate(CRANE_ClientHandle ClientHandle,
				  CRANE_TemplateHandle TH)
{
	fprintf(stderr,"CRANE_DeleteTemplate NOT IMPLEMENTED");
	return IPDR_ERR_NOT_IMPLEMENTED;
}

/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_CreateCollector(IPDR_ExporterHandle ExporterHandle,
                const char *Name,
                IPDR_IPAddress Addr,
                unsigned short Port,
                unsigned short NumberOfActive,
				IPDR_CollectorHandle* CH)
{
    Exporter *E = ExporterHandle;

	if(E->IsInsideCallback)
		return IPDR_ERR_API_CALL_FROM_CALLBACK ;

	return Exporter_AddCollectorInternal(E, Name, Addr, Port, NumberOfActive, IPDR_FALSE, CH);
}

/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_AddCollector(IPDR_ExporterHandle ExporterHandle,
                const char *Name,
                IPDR_IPAddress Addr,
                unsigned short Port,
                unsigned short NumberOfActive)
{
    Exporter *E = ExporterHandle;

	if(E->IsInsideCallback)
		return IPDR_ERR_API_CALL_FROM_CALLBACK ;

	return Exporter_AddCollectorInternal(E, Name, Addr, Port, NumberOfActive, IPDR_FALSE, NULL);
}

IPDR_API
IPDR_GetCollector(IPDR_ExporterHandle ExporterHandle,
				const char* Name,
                IPDR_CollectorHandle *CH,
                IPDR_IPAddress *Addr,
		 		unsigned short* Port,
                unsigned short* NumberOfActive)
{
    Exporter *E = ExporterHandle;
	Collector *Coll;
    int Index;

    if (! E || !Name || !CH) {
        return IPDR_ERR_INVALID_VALUE;
    }

    Platform_Lock(EXPORTER_PLATFORM);

	for (Index = 0; Index < E->Collectors.Length; Index++)
	{
        Coll = (Collector *) E->Collectors.Data[Index];
        if (strcmp(Coll->Name, Name) == 0)
		{
			*CH = Coll;
			if(Addr) *Addr = Coll->Addr;
			if(Port) *Port = Coll->Port;
			if(NumberOfActive) *NumberOfActive = Coll->NumOfConnections;
			Platform_Unlock(EXPORTER_PLATFORM);
			return IPDR_OK;
        }
    }

    Platform_Unlock(EXPORTER_PLATFORM);
	return IPDR_ERR_COLLECTOR_NOT_FOUND;
}


/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_API
IPDR_AssocCollectorSession(
			IPDR_ExporterHandle ExporterHandle,
			IPDR_SessionHandle SessionHandle,
			IPDR_CollectorHandle CollectorHandle,
			int Priority)
{
	Exporter* E = ExporterHandle;
	Session* S = SessionHandle;
	Collector* C = CollectorHandle ;

	return Exporter_AssocCollectorSession_Internal(E,S,C,Priority);
}

/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_API
IPDR_RemoveCollectorFromSession(IPDR_ExporterHandle ExporterHandle,
                   IPDR_SessionHandle SessionHandle,
                   IPDR_CollectorHandle CollectorHandle )
{
	Exporter* E = ExporterHandle;
	Session* S = SessionHandle;
	Collector* C = CollectorHandle ;

	return Exporter_RemoveCollectorFromSession_Internal(E,S,C);
}

/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_API
IPDR_RemoveCollector(IPDR_ExporterHandle ExporterHandle,
                   IPDR_CollectorHandle CollectorHandle)
{
    Exporter *E = ExporterHandle;
	Collector* C = CollectorHandle;

	return Exporter_RemoveCollectorInternal(E, C);
}


/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_GetSession(IPDR_ExporterHandle ExporterHandle,
                 unsigned char ID,
                 IPDR_SessionHandle *SH,
                 const char** Name,
                 const char** Description)
{
    Exporter *E = ExporterHandle;
    Session *S1;
    int i;

    if (! E) {
        return IPDR_ERR_INVALID_VALUE;
    }

    Platform_Lock(EXPORTER_PLATFORM);

    for (i = 0; i < E->Sessions.Length; i++) {
        S1 = (Session *) E->Sessions.Data[i];
        if (S1->ID == ID) {
            if (SH) { *SH = S1; }
            if (Name) { *Name = S1->Name; }
            if (Description) { *Description = S1->Description; }
            Platform_Unlock(EXPORTER_PLATFORM);
            return IPDR_OK;
        }
    }

    Platform_Unlock(EXPORTER_PLATFORM);
    if (SH) { *SH = NULL; }
    if (Name) { *Name = NULL; }
    if (Description) { * Description = NULL; }

    return IPDR_ERR_UNABLE_TO_FIND_SESSION;
}

IPDR_Status
Exporter_AddKeyInternal(IPDR_SessionHandle SessionHandle,
             IPDR_TemplateHandle TH,
             const IPDR_Key *CKey)
{
    Session *S = SessionHandle;
	Exporter *E = NULL;
    Template *Tmpl;
    IPDR_Status Status;

    if (! S || ! CKey) {
        return IPDR_ERR_INVALID_VALUE;
    }

	E = S->ExporterPtr;

    Platform_Lock(SESSION_PLATFORM);

	if(TH.s != S->ID || TH.i >= S->Templates.Length){
        Platform_Unlock(SESSION_PLATFORM);
        return IPDR_ERR_TEMPLATE_NOT_FOUND;
    }

    Tmpl = (Template *) S->Templates.Data[TH.i];
    if (Tmpl == NULL) {
        Platform_Unlock(SESSION_PLATFORM);
        return IPDR_ERR_TEMPLATE_NOT_FOUND;
    }

    Status = Template_AddKey(S, Tmpl,
                             CKey->KeyID, CKey->KeyTypeID,
                             CKey->KeyName, CKey->KeyLabel,
                             CKey->KeyHelp, CKey->Enabled,
                             CKey->AccessType,
                             CKey->ExtraData,
                             CKey->KeyAccessor);

#ifdef NOTDONEYET
    ExporterMIB_UpdateTemplates(S);
#endif
    Platform_Unlock(SESSION_PLATFORM);
    return Status;
}
/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_AddKey(IPDR_SessionHandle SessionHandle,
             IPDR_TemplateHandle TH,
             const IPDR_Key *CKey)
{
    Session *S = SessionHandle;
	Exporter *E = NULL;

    if ( !S ) {
        return IPDR_ERR_INVALID_VALUE;
    }

	E = S->ExporterPtr;
	if(E->IsInsideCallback)
		return IPDR_ERR_API_CALL_FROM_CALLBACK;

    return Exporter_AddKeyInternal(SessionHandle,TH,CKey);
}


/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
Exporter_AddKeyInternal_All(IPDR_ExporterHandle ExporterHandle,
             int TemplateID,
             const IPDR_Key *CKey)
{
    Template *Tmpl;
	Session *S;
	Exporter* E = ExporterHandle;
    IPDR_Status Status, Status1 = IPDR_OK;
	int j;
	IPDR_TemplateHandle TH;

    if (! E || ! CKey) {
        return IPDR_ERR_INVALID_VALUE;
    }

    Platform_Lock(SESSION_PLATFORM);

	for (j = 0; j < E->Sessions.Length; j++) {
        S = (Session *) E->Sessions.Data[j];

		Status = IPDR_GetTemplateInternal(S, TemplateID, NULL, NULL, NULL, &TH);
		if(SUCCESS(Status))
		{
			Tmpl = (Template *) S->Templates.Data[TH.i];
			if (Tmpl == NULL) {
				Status1 = IPDR_ERR_TEMPLATE_NOT_FOUND;
				continue ;
			}

			Status = Template_AddKey(S, Tmpl,
									CKey->KeyID, CKey->KeyTypeID,
									CKey->KeyName, CKey->KeyLabel,
									CKey->KeyHelp, CKey->Enabled,
									CKey->AccessType,
									CKey->ExtraData,
									CKey->KeyAccessor);
			if(FAILURE(Status))
				Status1 = Status;
		}
	}
#ifdef NOTDONEYET
    ExporterMIB_UpdateTemplates(S);
#endif
    Platform_Unlock(SESSION_PLATFORM);
    return Status1;
}

/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_RemoveKey(IPDR_SessionHandle SessionHandle,
                IPDR_TemplateHandle TH,
                unsigned int KeyID)
{
    Session *S = SessionHandle;
    Template *Tmpl;
    Key *K;
    int i;

    if ( !S ) {
        return IPDR_ERR_INVALID_VALUE;
    }

    Platform_Lock(SESSION_PLATFORM);

	if(TH.s != S->ID || TH.i >= S->Templates.Length){
        Platform_Unlock(SESSION_PLATFORM);
        return IPDR_ERR_TEMPLATE_NOT_FOUND;
    }

    Tmpl = (Template *) S->Templates.Data[TH.i];
    if (Tmpl == NULL) {
        Platform_Unlock(SESSION_PLATFORM);
        return IPDR_ERR_TEMPLATE_NOT_FOUND;
    }

    for (i = 0; i < Tmpl->Keys.Length; i++) {
        K = (Key *) Tmpl->Keys.Data[i];
        if (K->KeyID == KeyID) {
            DynamicArray_RemoveAndShift(SESSION_PLATFORM, & (Tmpl->Keys), i);
            Platform_Unlock(SESSION_PLATFORM);
            return IPDR_OK;
        }
    }
#ifdef NOTDONEYET
    ExporterMIB_UpdateTemplates(S);
#endif
    Platform_Unlock(SESSION_PLATFORM);

    return IPDR_ERR_KEY_NOT_FOUND;
}


/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_RemoveKey_All(IPDR_ExporterHandle ExporterHandle,
                int TemplateID,
                unsigned int KeyID)
{
	Exporter* E = ExporterHandle;
    Template *Tmpl;
	Session* S;
	IPDR_Status Status = IPDR_OK;
    Key *K;
    int i, j;
	IPDR_TemplateHandle TH;

    if (! E) {
        return IPDR_ERR_INVALID_VALUE;
    }

    Platform_Lock(SESSION_PLATFORM);

	for (j = 0; j < E->Sessions.Length; i++) {
        S = (Session *) E->Sessions.Data[i];

		Status = IPDR_GetTemplateInternal(S, TemplateID, NULL, NULL, NULL, &TH);
		if(SUCCESS(Status))
		{
			Tmpl = (Template *) S->Templates.Data[TH.i];
			if (Tmpl == NULL) {
				Status = IPDR_ERR_TEMPLATE_NOT_FOUND;
			}

			for (i = 0; i < Tmpl->Keys.Length; i++) {
				K = (Key *) Tmpl->Keys.Data[i];
				if (K->KeyID == KeyID) {
					DynamicArray_RemoveAndShift(SESSION_PLATFORM, & (Tmpl->Keys), i);
				}
			}
		}
	}
#ifdef NOTDONEYET
    ExporterMIB_UpdateTemplates(S);
#endif
    Platform_Unlock(SESSION_PLATFORM);

    return Status;
}


/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_API
IPDR_EnableKey(IPDR_SessionHandle SessionHandle,
                IPDR_TemplateHandle TH,
                unsigned int KeyID,
				IPDR_BOOL Enabled)
{
    Session *S = SessionHandle;
    Template *Tmpl;
    Key *K;
    int i;

    if (! S)
        return IPDR_ERR_INVALID_VALUE;

    Platform_Lock(SESSION_PLATFORM);

	if(TH.s != S->ID || TH.i >= S->Templates.Length){
        Platform_Unlock(SESSION_PLATFORM);
        return IPDR_ERR_TEMPLATE_NOT_FOUND;
    }

    Tmpl = (Template *) S->Templates.Data[TH.i];
    if (Tmpl == NULL) {
        Platform_Unlock(SESSION_PLATFORM);
        return IPDR_ERR_TEMPLATE_NOT_FOUND;
    }

    for (i = 0; i < Tmpl->Keys.Length; i++) {
        K = (Key *) Tmpl->Keys.Data[i];
        if (K->KeyID == KeyID) {
            K->Enabled = Enabled;
            Platform_Unlock(SESSION_PLATFORM);
            return IPDR_OK;
        }
    }
#ifdef NOTDONEYET
    ExporterMIB_UpdateTemplates(S);
#endif
    Platform_Unlock(SESSION_PLATFORM);

    return IPDR_ERR_KEY_NOT_FOUND;
}


/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_CreateTemplate(IPDR_ExporterHandle ExporterHandle,
					IPDR_SessionHandle SessionHandle,
                     int TemplateID,
                     const char *SchemaName,
				     const char *TypeName,
                     int NumKeys,
                     const IPDR_Key *Keys,
                     IPDR_TemplateHandle *TH)
{
    Exporter *E = ExporterHandle;
	Session *S = SessionHandle ;
    return Exporter_CreateTemplateInternal(E, S, TemplateID, SchemaName, TypeName,
                                         NumKeys, Keys, IPDR_FALSE, TH);
}


/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_CreateTemplate_All(IPDR_ExporterHandle ExporterHandle,
					 int TemplateID,
                     const char *SchemaName,
				     const char *TypeName,
                     int NumKeys,
                     const IPDR_Key *Keys)
{
    Exporter *E = ExporterHandle;
	Session *S = NULL ;
	IPDR_Status Status, Status1 = IPDR_OK;
	int i ;
	IPDR_TemplateHandle TH;
	for (i = 0; i < E->Sessions.Length; i++) {
        S = (Session *) E->Sessions.Data[i];
		Status = Exporter_CreateTemplateInternal(E, S, TemplateID, SchemaName, TypeName,
                                         NumKeys, Keys, IPDR_FALSE, &TH);
		if(FAILURE(Status))
			Status1 = Status;
    }
    return Status1;
}

/**
 * Create a template (internal).
 *
 * @see IPDR_CreateTemplate
 *
 * @param E client (transmitter) handle (pointer)
 * @param S session handle
 * @param TemplateID template
 * @param SchemaName schema name of the template
 * @param TypeName type name of the template
 * @param NumKeys number of \@ Keys
 * @param Keys keys in the template
 * @param StatusFlag true for Status template, false otherwise
 * @param TH template handle (updated)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise. */
IPDR_Status
Exporter_CreateTemplateInternal(Exporter* E,
							  Session* S,
                              int TemplateID,
                              const char *SchemaName,
						      const char *TypeName,
                              int NumKeys,
                              const IPDR_Key *Keys,
                              IPDR_BOOL StatusFlag,
                              IPDR_TemplateHandle *TH)
{
    IPDR_Status Status;
    Template *Tmpl;
    int i;

    if ( !TemplateID || !E || !S || !TH || (NumKeys > 0 && !Keys)) {
        return IPDR_ERR_INVALID_VALUE;
    }

    Platform_Lock(EXPORTER_PLATFORM);

    if (SUCCESS(IPDR_GetTemplate(S, TemplateID, NULL, NULL, NULL, TH))) {
        Platform_Unlock(EXPORTER_PLATFORM);
        return IPDR_ERR_TEMPLATE_ALREADY_EXISTS;
    }

    /* create a new template */
    Status = Template_Create(S, TemplateID,
                             SchemaName, TypeName, NumKeys, StatusFlag, & Tmpl);
    if (FAILURE(Status)) {
        Platform_Unlock(EXPORTER_PLATFORM);
        return Status;
    }

    /* add the requested keys to the template */
    for (i = 0; i < NumKeys; i++) {
		Status = Template_AddKey(S,
								Tmpl,
								Keys[i].KeyID, Keys[i].KeyTypeID,
								Keys[i].KeyName, Keys[i].KeyLabel,
								Keys[i].KeyHelp, Keys[i].Enabled,
								Keys[i].AccessType,
								Keys[i].ExtraData,
								Keys[i].KeyAccessor);
        if (FAILURE(Status)) {
            Object_Release(EXPORTER_PLATFORM, Tmpl);
            Platform_Unlock(EXPORTER_PLATFORM);
            return Status ;
        }
    }
	Template_Reset(S, Tmpl);

	TH->s = S->ID;

    Status = DynamicArray_AddElement(EXPORTER_PLATFORM, & (S->Templates),
                                     Tmpl, &TH->i);
    if (FAILURE(Status)) {
        Object_Release(EXPORTER_PLATFORM, Tmpl);
        Platform_Unlock(EXPORTER_PLATFORM);
        return Status;
    }
    Platform_Unlock(EXPORTER_PLATFORM);
    return IPDR_OK;
}


/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_DeleteTemplate(IPDR_SessionHandle SessionHandle,
                     IPDR_TemplateHandle TH)
{
    Session *S = SessionHandle;
    IPDR_Status Status;

    if (! S || TH.s != S->ID || TH.i >= S->Templates.Length) {
        return IPDR_ERR_INVALID_VALUE;
    }

    Platform_Lock(SESSION_PLATFORM);
    Status = DynamicArray_Remove(SESSION_PLATFORM, & (S->Templates), TH.i);
    Platform_Unlock(SESSION_PLATFORM);
    return Status;
}


/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_DeleteTemplate_All(IPDR_ExporterHandle ExporterHandle,
                     int TemplateID)
{
	Session *S ;
	Exporter* E = ExporterHandle;

	IPDR_Status Status, Status1 = IPDR_OK;
	IPDR_TemplateHandle TH;
	int i;

    if (! E) {
        return IPDR_ERR_INVALID_VALUE;
    }

    Platform_Lock(SESSION_PLATFORM);
	for (i = 0; i < E->Sessions.Length; i++) {
        S = (Session *) E->Sessions.Data[i];

		Status = IPDR_GetTemplateInternal(S, TemplateID, NULL, NULL, NULL, &TH);
		if(SUCCESS(Status))
		{
   			Status = DynamicArray_Remove(SESSION_PLATFORM, & (S->Templates), TH.i);

			if(FAILURE(Status))
				Status1 = Status;
		}
	}
    Platform_Unlock(SESSION_PLATFORM);
    return Status1;
}


/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_API
IPDR_SendData(IPDR_ExporterHandle ExporterHandle,
				IPDR_SessionHandle SessionHandle,
               IPDR_TemplateHandle TemplateHandle,
               const void *Data)
{
    Exporter*	E = ExporterHandle;
	Session*	S = SessionHandle;
	return ExporterProtocol_SendData(E, S, NULL, NULL, TemplateHandle, Data);
}
IPDR_API
IPDR_SendDataCompat(IPDR_ExporterHandle ExporterHandle,
               IPDR_TemplateHandle TemplateHandle,
               const void *Data)
{
	return IPDR_ERR;
}

/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_API
IPDR_SendData_All(IPDR_ExporterHandle ExporterHandle,
				int TemplateID,
               	const void *Data)
{
	Exporter *E = ExporterHandle;
	Session * S = NULL;
	IPDR_Status Status, Status1 = IPDR_OK;
	IPDR_TemplateHandle TemplateHandle;
	int j ;
	for (j = 0; j < E->Sessions.Length; j++) {
        S = (Session *) E->Sessions.Data[j];

		Status = IPDR_GetTemplateInternal(S, TemplateID, NULL, NULL, NULL, &TemplateHandle);
		if(SUCCESS(Status))
		{
			Status = IPDR_SendData(ExporterHandle, S, TemplateHandle, Data);

			if(FAILURE(Status))
				Status1 = Status;
		}
	}
	return Status1;
}

IPDR_API
IPDR_EnableKey_All(IPDR_SessionHandle SessionHandle,
					int TemplateID,
					unsigned int KeyID,
					IPDR_BOOL Enabled)
{
	Session * S = SessionHandle;
	Exporter *E = S->ExporterPtr;
	IPDR_Status Status, Status1 = IPDR_OK;
	IPDR_TemplateHandle TemplateHandle;
	int j ;
	for (j = 0; j < E->Sessions.Length; j++) {
        S = (Session *) E->Sessions.Data[j];

		Status = IPDR_GetTemplateInternal(S, TemplateID, NULL, NULL, NULL, &TemplateHandle);
		if(SUCCESS(Status))
		{
			Status = IPDR_EnableKey(SessionHandle, TemplateHandle, KeyID, Enabled);

			if(FAILURE(Status))
				Status1 = Status;
		}
	}
	return Status1;
}

/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_GetTemplate(IPDR_SessionHandle SessionHandle,
                  int TemplateID,
                  char **SchemaName,
				  char **TypeName,
                  int *NumKeys,
                  IPDR_TemplateHandle *TH)
{
	if (! SessionHandle || ! TH) {
        return IPDR_ERR_INVALID_VALUE;
    }

    return IPDR_GetTemplateInternal(SessionHandle, TemplateID, SchemaName, TypeName, NumKeys, TH);
}

IPDR_Status
IPDR_GetTemplateInternal(IPDR_SessionHandle SessionHandle,
                  int TemplateID,
                  char **SchemaName,
				  char **TypeName,
                  int *NumKeys,
                  IPDR_TemplateHandle *TH)
{
	Session *S = SessionHandle ;
    int i;
    Template *Tmpl;

    if (! S || ! TH) {
        return IPDR_ERR_INVALID_VALUE;
    }

	TH->i = 0xFFFFFFFFu;
    Platform_Lock(SESSION_PLATFORM);

    for (i = 0; i < S->Templates.Length; i++) {
        Tmpl = (Template *) S->Templates.Data[i];
        if (Tmpl != NULL && Tmpl->TemplateID == TemplateID) {
            TH->i = i;
			TH->s = S->ID;
			if (SchemaName) (*SchemaName) = Tmpl->SchemaName;
			if (TypeName) (*TypeName) = Tmpl->TypeName;
			if (NumKeys) (*NumKeys) = Tmpl->Keys.Length;
            Platform_Unlock(SESSION_PLATFORM);
            return IPDR_OK;
        }
    }

    Platform_Unlock(SESSION_PLATFORM);
    return IPDR_ERR_TEMPLATE_NOT_FOUND;
}

/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_ApplyTemplates(IPDR_SessionHandle SessionHandle)
{
    Session *S = SessionHandle;
	Exporter *E = S->ExporterPtr;
    Template *Tmpl;
    int i;

    if (!S) {
        return IPDR_ERR_INVALID_VALUE;
    }

    if (!Platform_IsRunning(SESSION_PLATFORM)) {
        return IPDR_ERR_NOT_RUNNING;
    }

    Platform_Lock(SESSION_PLATFORM);

    if (! S->ExporterPtr->Started) {
        Platform_Unlock(SESSION_PLATFORM);
        return IPDR_OK;
    }

	if(S->Running == IPDR_TRUE)
	{
		Log_Write1(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_ERROR,
				"IPDR_ApplyTemplates",
				MSG_EXPORTER_SESSION_MUST_STOP3,
				S->ID);
		Platform_Unlock(SESSION_PLATFORM);
		return IPDR_ERR_SESSION_IS_RUNNING;
	}

    /* reset all templates associated with the client */

    for (i = 0; i < S->Templates.Length; i++) {
        Tmpl = (Template *) S->Templates.Data[i];
        if (Tmpl != NULL)
            Template_Reset(S, Tmpl);
    }

    /* start a new work context based on the client templates
       once the templates are applied */
	Session_CreateNewWorkContext(S->ExporterPtr, S, & (S->Templates), IPDR_TRUE);
	ExporterProtocol_FlushRecordsQueue(S->ExporterPtr, S, NULL, "ApplyTemplates");

    /*for (i = 0; i < E->Sessions.Length; i++) {
        S = (Session *) E->Sessions.Data[i];
        Session_CreateNewWorkContext(E, S, & (E->Templates), IPDR_TRUE);
        ExporterProtocol_FlushRecordsQueue(E, S, NULL, "ApplyTemplates");
    }*/

    Platform_Unlock(SESSION_PLATFORM);

    return IPDR_OK;
}

/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_ApplyTemplates_All(IPDR_ExporterHandle ExporterHandle)
{
	Exporter* E = ExporterHandle;
	Session* S ;
	int i ;
	IPDR_Status Status = IPDR_OK ;

	Platform_Lock(EXPORTER_PLATFORM);

	for (i = 0; i < E->Sessions.Length; i++) {
        S = (Session *) E->Sessions.Data[i];
        Status = IPDR_ApplyTemplates(S);
		if( FAILURE(Status) )
		{
			Platform_Unlock(EXPORTER_PLATFORM);
			return Status ;
		}
    }

	Platform_Unlock(EXPORTER_PLATFORM);
	return Status ;
}


/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_GetSessionsList(IPDR_ExporterHandle ExporterHandle,
                      int *NumSessions,
                      unsigned char **Sessions)
{
    Exporter *E = ExporterHandle;
    int i;

    if (! E) {
        return IPDR_ERR_INVALID_VALUE;
    }

    if (NumSessions) { *NumSessions = 0; }
    if (Sessions)    { *Sessions = NULL; }

    Platform_Lock(EXPORTER_PLATFORM);

    Platform_MemoryFree(EXPORTER_PLATFORM, E->CSessions);
    Platform_Check_MEMORYALLOCATEVEC(EXPORTER_PLATFORM, E->Sessions.Length, E->CSessions,
    {
        E->CSessions = NULL;
        Platform_Unlock(EXPORTER_PLATFORM);
    });

    for (i = 0; i < E->Sessions.Length; i++) {
        Session *S = (Session *) E->Sessions.Data[i];
        E->CSessions[i] = S->ID;
    }

    if (NumSessions) { *NumSessions = i; }
    if (Sessions)    { *Sessions = E->CSessions; }

    Platform_Unlock(EXPORTER_PLATFORM);

    return IPDR_OK;
}

/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_API
IPDR_GetCollectorInSessionList(IPDR_ExporterHandle ExporterHandle,
					IPDR_SessionHandle SessionHandle,
					unsigned int *NumCollectors,
					IPDR_Collector **Collectors)
{
    Exporter *E = ExporterHandle;
    Session* S = SessionHandle;
    unsigned int i;

    if (! E || ! S) {
        return IPDR_ERR_INVALID_VALUE;
    }

    if (NumCollectors){ *NumCollectors = 0; }
    if (Collectors){ *Collectors = NULL; }

    Platform_Lock(EXPORTER_PLATFORM);

    for (i = 0; i < S->CCollectorsLength; i++)
	{
        S->CCollectors[i].Name = NULL;
    }

    Platform_MemoryFree(EXPORTER_PLATFORM, S->CCollectors);

    Platform_Check_MEMORYALLOCATEVEC(EXPORTER_PLATFORM, S->CollectorSessions.Length, S->CCollectors,
    {
		S->CCollectorsLength = 0;
        S->CCollectors= NULL;
        Platform_Unlock(EXPORTER_PLATFORM);
    });

    for (S->CCollectorsLength = 0; S->CCollectorsLength < S->CollectorSessions.Length; S->CCollectorsLength++)
	{
		CollectorSession *CS = (CollectorSession *)S->CollectorSessions.Data[S->CCollectorsLength];
        Collector *C = CS->Collector;
		if(C == NULL)
			continue;
        S->CCollectors[S->CCollectorsLength].Name = C->Name;
        S->CCollectors[S->CCollectorsLength].Addr = C->Addr;
        S->CCollectors[S->CCollectorsLength].Port = C->Port;
	}

    if (NumCollectors){ *NumCollectors = S->CCollectorsLength; }
    if (Collectors){ *Collectors = S->CCollectors; }

    Platform_Unlock(EXPORTER_PLATFORM);
    return IPDR_OK;
}

/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_GetCollectorList(IPDR_ExporterHandle ExporterHandle,
                     unsigned int *NumCollectors,
                     IPDR_Collector **Collectors)
{

    Exporter *E = ExporterHandle;
	int i;

 	if (! E || ! NumCollectors || !Collectors)
	{
        return IPDR_ERR_INVALID_VALUE;
    }

	Platform_Lock(EXPORTER_PLATFORM);

	(*NumCollectors) = E->Collectors.Length ;

	Platform_MemoryFree(EXPORTER_PLATFORM, E->CCollectors);

	Platform_Check_MEMORYALLOCATEVEC(EXPORTER_PLATFORM, (*NumCollectors), E->CCollectors,
	{
		E->CCollectors= NULL;
		Platform_Unlock(EXPORTER_PLATFORM);
	});

	for (i = 0; i < E->Collectors.Length; i++) {
		Collector *C = (Collector *) E->Collectors.Data[i];

		E->CCollectors[i].Name = C->Name;
		E->CCollectors[i].Addr = C->Addr;
		E->CCollectors[i].Port = C->Port;
	}

    (*Collectors) = E->CCollectors;
	Platform_Unlock(EXPORTER_PLATFORM);
    return IPDR_OK;
}


/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_SetExporterEventHandlerDetailed(IPDR_ExporterHandle ExporterHandle,
                                    IPDR_EventCallbackDetailed Callback,
                                    void *Arg)
{
    Exporter *E = ExporterHandle;
    if (! E) {
        return IPDR_ERR_INVALID_VALUE;
    }

    Platform_Lock(EXPORTER_PLATFORM);

    E->EventCallbackDetailed = Callback;
    E->EventCallbackDetailedArg = Arg;

    Platform_Unlock(EXPORTER_PLATFORM);

    return IPDR_OK;
}

/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_API
IPDR_SetStartNegotiationCallback(IPDR_ExporterHandle ExporterHandle,
                                 IPDR_StartNegotiationCallback Callback,
                                 void *Arg)
{
	Exporter *E = ExporterHandle;
	if (! E) {
		return IPDR_ERR_INVALID_VALUE;
	}

	Platform_Lock(EXPORTER_PLATFORM);

	E->StartNegotiationCallback = Callback;
	E->StartNegotiationCallbackArg = Arg;

	Platform_Unlock(EXPORTER_PLATFORM);

	return IPDR_OK;
}

/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_API
IPDR_SetFlowStartCallback(IPDR_ExporterHandle ExporterHandle,
                                 IPDR_FlowStartCallback Callback,
                                 void *Arg)
{
	Exporter *E = ExporterHandle;
	if (! E) {
		return IPDR_ERR_INVALID_VALUE;
	}

	Platform_Lock(EXPORTER_PLATFORM);

	E->FlowStartCallback = Callback;
	E->FlowStartCallbackArg = Arg;

	Platform_Unlock(EXPORTER_PLATFORM);

	return IPDR_OK;
}


/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_SetTemplateChangedCallback(IPDR_ExporterHandle ExporterHandle,
                                 IPDR_TemplateChangedCallback Callback,
                                 void *Arg)
{
    Exporter *E = ExporterHandle;
    if (! E) {
        return IPDR_ERR_INVALID_VALUE;
    }

    Platform_Lock(EXPORTER_PLATFORM);

    E->TemplateChangedCallback = Callback;
    E->TemplateChangedArg = Arg;

    Platform_Unlock(EXPORTER_PLATFORM);

    return IPDR_OK;
}

/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_SetNewConfigurationIDCallback(IPDR_ExporterHandle ExporterHandle,
                                 IPDR_NewConfigurationIDCallback Callback,
                                 void *Arg)
{
    Exporter *E = ExporterHandle;
    if (! E) {
        return IPDR_ERR_INVALID_VALUE;
    }

    Platform_Lock(EXPORTER_PLATFORM);

    E->NewConfigurationIDCallback = Callback;
    E->NewConfigurationIDCallbackArg = Arg;

    Platform_Unlock(EXPORTER_PLATFORM);

    return IPDR_OK;
}

/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_SetConfigurationIDChangedCallback(IPDR_ExporterHandle ExporterHandle,
                                 IPDR_ConfigurationIDChangedCallback Callback,
                                 void *Arg)
{
    Exporter *E = ExporterHandle;
    if (! E) {
        return IPDR_ERR_INVALID_VALUE;
    }

    Platform_Lock(EXPORTER_PLATFORM);

    E->ConfigurationIDChangedCallback = Callback;
    E->ConfigurationIDChangedCallbackArg = Arg;

    Platform_Unlock(EXPORTER_PLATFORM);

    return IPDR_OK;
}

/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_SetModuleMoreDebugLevel(IPDR_ExporterHandle ExporterHandle,
                              const char *ModuleName,
                              IPDR_DebugLevel L)
{
	char ConfigName[MAX_CONFIG_STR_LEN] ;
    Exporter *E = ExporterHandle;
    if (! E || !ModuleName) {
        return IPDR_ERR_INVALID_VALUE;
    }

	sprintf(ConfigName, STR_configuration_logging_levels_moduleS, ModuleName);
	ParametersHandler_SetUIntParam(& (E->ParamsHandler), ConfigName, L);
    return Log_SetModuleMoreDebugLevel(EXPORTER_LOG, ModuleName, L);
}


/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_CheckLogLevel(IPDR_ExporterHandle ExporterHandle,
                    const char *ModuleName,
                    IPDR_DebugLevel Lvl)
{
    Exporter *E = ExporterHandle;
    if (! E) {
        return IPDR_ERR_INVALID_VALUE;
    }

    return Log_CheckLogLevelByName(EXPORTER_LOG, ModuleName, Lvl);
}


/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_LogWrite(IPDR_ExporterHandle ExporterHandle,
               const char* ModuleName,
               IPDR_DebugLevel Level,
               const char *Func,
               const char *Str,
               ...)
{
    va_list Args;
    IPDR_Status Status;
    Exporter *E = ExporterHandle;

    if (! E) {
        return IPDR_ERR_INVALID_VALUE;
    }

    va_start(Args, Str);
    Status = Log_VOutputStrByName(EXPORTER_LOG, ModuleName, Level, Func, Str, Args);
	va_end(Args);

    return Status;
}


/**
 * Create a structure containing all currently enabled keys
 *   and trigger the templates change callback.
 *
 * @param S exporter handle (pointer)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
Exporter_TriggeTemplateChangedCallback(Session *S)
{
    unsigned int NumTemplates;
    IPDR_Template *Tmpls;
    IPDR_Key *Keys;
    Template *Tmpl/*, *Tmpl1*/;
    Key *K/*, K1*/;
    TransmissionContext *TC;
	IPDR_TemplateHandle TH;
    unsigned int i, j, k/*, m*/;

    if (S->ExporterPtr->TemplateChangedCallback == NULL)
        return IPDR_OK;

	TC = S->ActiveContext;

    DynamicArray_CountObjects(SESSION_PLATFORM, & (TC->Templates), & NumTemplates);

    Platform_Check_MEMORYALLOCATEVEC(SESSION_PLATFORM, NumTemplates, Tmpls, {});

    for (j = 0, i = 0; i < TC->Templates.Length; i++) {
        Tmpl = (Template *) TC->Templates.Data[i];
        if (Tmpl == NULL)
            continue;
		TH.i = i;
		TH.s = S->ID;
        Tmpls[j].TemplateID = Tmpl->TemplateID;
        Tmpls[j].SchemaName = Tmpl->SchemaName;
		Tmpls[j].TypeName = Tmpl->TypeName;
        Tmpls[j].NumKeys = Tmpl->Keys.Length;

        Platform_Check_MEMORYALLOCATEVEC(SESSION_PLATFORM, Tmpl->Keys.Length, Keys, {});

        for (k = 0; k < Tmpl->Keys.Length; k++) {

            K = (Key *) Tmpl->Keys.Data[k];
            Keys[k].KeyID = K->KeyID;
            Keys[k].KeyTypeID = K->KeyTypeID;
            Keys[k].KeyName = K->Name;
            Keys[k].KeyLabel = K->Label;
            Keys[k].KeyHelp = K->Help;
            Keys[k].Enabled = K->Enabled;
            Keys[k].AccessType = K->AccessType;
            Keys[k].ExtraData = K->ExtraData;
            Keys[k].KeyAccessor = K->KeyAccessor;

            /*if (TC == NULL || j >= TC->Templates.Length || TC->Templates.Data[j] == NULL)
				continue;

			Tmpl1 = (Template *) TC->Templates.Data[j];

			*//* find the matching key *//*
			for (m = 0; m < Tmpl1->Keys.Length; m++) {
				K1 = (Key *) Tmpl1->Keys.Data[m];
				if (K1->KeyID == K->KeyID)
					Keys[k].Enabled = K1->Enabled;
			}*/
        }
        Tmpls[j].Keys = Keys;
        j++;

    }

	if(S->ExporterPtr->TemplateChangedCallback != NULL)
    	S->ExporterPtr->TemplateChangedCallback(S, Tmpls, NumTemplates, S->ExporterPtr->TemplateChangedArg);

	for (j = 0, i = 0; i < TC->Templates.Length; i++)
	{
		Tmpl = (Template *) TC->Templates.Data[i];
		if (Tmpl == NULL)
            continue;

		Tmpls[j].SchemaName = NULL ;
		Tmpls[j].TypeName = NULL ;
		for (k = 0; k < Tmpl->Keys.Length; k++)
		{
			K = (Key *) Tmpl->Keys.Data[k];
			if (K->KeyID == Tmpls[j].Keys[k].KeyID)
			{
				K->Enabled = Tmpls[j].Keys[k].Enabled;
				Keys[k].KeyName = NULL ;
				Keys[k].KeyLabel = NULL ;
				Keys[k].KeyHelp = NULL ;
				Keys[k].ExtraData = NULL ;
				Keys[k].KeyAccessor = NULL ;
			}
		}
		j++;
	}
    for (j = 0; j < NumTemplates; j++)
        Platform_MemoryFree(SESSION_PLATFORM, Tmpls[j].Keys);
    Platform_MemoryFree(SESSION_PLATFORM, Tmpls);

    return IPDR_OK;
}


/**
 * Initialize an event structure.
 *
 * @param EventData structure to update
 * @param EventID ID to put into it
 * @param E exporter handle (pointer) (may be NULL)
 * @param CS Collector Session handle (pointer)
 * @param Sess session (may be NULL)
 */
void
EventDataInit(IPDR_EventData* EventData,
              enum IPDR_Event_t EventID,
              Exporter *E,
              CollectorSession *CS,
              Session *Sess)
{
    EventData->EventID                     = EventID;
    if (CS) {
        EventData->IC                  = CS->SessionConnection;
    } else {
        EventData->IC                  = NULL;
    }
    if (E && Sess) {
        EventData->OutstandingDataCount    = EXPORTER_QHANDLER->OutstandingDataCount(E, Sess);
        EventData->UnacknowledgedDataCount = EXPORTER_QHANDLER->UnacknowledgedDataCount(E, Sess);
    } else {
        EventData->OutstandingDataCount    = 0;
        EventData->UnacknowledgedDataCount = 0;
    }
    if (Sess) {
        EventData->SessionID               = Sess->ID;
        EventData->SessionName             = Sess->Name;
        EventData->SessionDescription      = Sess->Description;
    } else {
        EventData->SessionID               = 0;
        EventData->SessionName             = "";
        EventData->SessionDescription      = "";
    }
}


/**
 * Cause an event's triggers to be fired.
 *
 * @param E exporter handle (pointer) handle
 * @param loggingLevel logging level (debug, info, etc.)
 * @param EventData data to be updated
 * @param CS Collector session for record event in LastEvent -- NULL if you don't want it recorded
 * @param Msg message to be put into the event data
 * @param ... parameters to Msg
 */
void
Exporter_TriggerEvent(Exporter *E,
                    IPDR_DebugLevel loggingLevel,
                    const IPDR_EventData* EventData,
                    CollectorSession *CS,
                    const char *Msg,
                    ...)
{
    const char* eventStr;
    va_list Args;

    eventStr = IPDR_EventMsg(EventData);

    if (EventData->EventID == IPDR_EVENT_RECORD_LOST) {
        loggingLevel = IPDR_LEVEL_DEBUG; /* Prevent a flood of messages */
    }

    va_start(Args, Msg);
    vsprintf(E->EventLine, Msg, Args);
    va_end(Args);

    Log_Write2(EXPORTER_LOG, EXPORTER_MODULE_ID, loggingLevel,
               "Exporter_TriggerEvent",
               MSG_EXPORTER_TRIGGER_EVENT,
               eventStr, E->EventLine);

    if (E->EventCallbackDetailed != NULL) {
        E->EventCallbackDetailed(EventData, E->EventLine, E->EventCallbackDetailedArg);
    }

    if (CS) {
        CS->LastEvent = *EventData;
    }
}


/**
 * Callback triggered whenever a protocol info request arrives.
 *
 * @param S socket
 * @param Arg additional argument (client)
 */
static void
Exporter_ProtocolInfoCallback(IPDR_SocketHandle S,
                            void *Arg)
{
    IPDR_IPAddress RemoteAddr;
    unsigned short RemotePort;
    IPDR_Status Status;
    ProtocolInfoRequestMessage RequestMsg;
    ProtocolInfoResponseMessage ResponseMsg;
    unsigned int ReadBytes;

    Exporter *E = (Exporter *) Arg;

    memset(&ResponseMsg, sizeof ResponseMsg, 0); /* ensure zeros in padding areas */

    Status = Platform_RecvFrom(EXPORTER_PLATFORM, S,
                               (char *) & RequestMsg,
                               sizeof(RequestMsg),
                               & RemoteAddr,
                               & RemotePort,
                               & ReadBytes);
    if (FAILURE(Status) || ReadBytes == 0) /* do nothing if it's just a timer going off */
        return ;

    /* Sanity check the remote message; but keep going even if it's
     * suspcious (log a message, though). This is because the current
     * server (collector) might not behaving properly; and we don want to get the
     * connection going. */

    if (ReadBytes < sizeof (ProtocolInfoRequestMessage)) {
        TL_PABuf paBuf;
        Log_Write2(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_WARNING,
                   "Exporter_ProtocolInfoCallback",
                   MSG_EXPORTER_BAD_PROTOCOL_REQUEST_LEN,
                   TL_PrintAddressAndPort(RemoteAddr, RemotePort, paBuf),
                   ReadBytes);
    } else {
        const ProtocolInfoRequestMessage* r = (const ProtocolInfoRequestMessage*)&RequestMsg;
        char buf[IPDR_TIME_FORMAT_LENGTH+1];
        TL_PABuf paBuf1, paBuf2;
        if (r->MSG[0] != 'I' ||
            r->MSG[1] != 'P' ||
            r->MSG[2] != 'D' ||
            r->MSG[3] != 'R') {
            Log_Write5(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_WARNING,
                       "Exporter_ProtocolInfoCallback",
                       MSG_EXPORTER_BAD_PROTOCOL_REQUEST,
                       TL_PrintAddressAndPort(RemoteAddr, RemotePort, paBuf1),
                       r->MSG[0], r->MSG[1], r->MSG[2], r->MSG[3]);
        }

        Log_Write4(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_INFO,
                   "Exporter_ProtocolInfoCallback",
                   MSG_EXPORTER_PROTOCOL_INFO,
                   TL_PrintAddressAndPort(RemoteAddr, RemotePort, paBuf1),
                   TL_PrintAddress(IPDR_NTOHL(r->ServerAddress), paBuf2),
                   Platform_FormatTimeGMT(EXPORTER_PLATFORM, IPDR_NTOHL(r->ServerBootTime), buf),
                   E->LocalPort);
    }

    IPDR_SET_HTONL(ResponseMsg.AdditionalProtocolsCount, 0);
    IPDR_SET_HTONL(ResponseMsg.DefaultProtocol.TransportType, IPDR_TRANSPORT_TCP);
    IPDR_SET_HTONL(ResponseMsg.DefaultProtocol.ProtocolVersion, IPDR_PROTOCOL_VERSION);
    IPDR_SET_HTONS(ResponseMsg.DefaultProtocol.PortNumber, E->LocalPort);
    IPDR_SET_HTONS(ResponseMsg.DefaultProtocol.Reserved, 0);

    Platform_SendTo(EXPORTER_PLATFORM, S, (const char *) & ResponseMsg,
                    sizeof(ResponseMsg), RemoteAddr, RemotePort);
}

/**
 * Initialize a UDP socket which will listen for protocol info
 *   messages.
 *
 * @param E - exporter handle (pointer)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static IPDR_Status
Exporter_InitializeProtocolInfo(Exporter *E)
{
    const char *Addr;
    IPDR_Status Status;

    Addr = ParametersHandler_GetStringParam(& (E->ParamsHandler),
                                            STR_configuration_protocolInfoAddr);
    if (Addr == NULL)
        Addr = ParametersHandler_GetStringParam(& (E->ParamsHandler),
                                                STR_configuration_exporterAddress);

    if (Addr == NULL)
        E->ProtocolInfoAddr = IPDR_DEFAULT_ADDRESS;
    else
        E->ProtocolInfoAddr = TL_ParseAddress(Addr);

    E->ProtocolInfoPort =
        (unsigned short) ParametersHandler_GetIntParam(& (E->ParamsHandler),
                                                       STR_configuration_protocolInfoPort);
    if (E->ProtocolInfoPort == 0) { /* Documented convention in IPDR.h. */
        IPDR_SetInvalidSocket(&E->ProtocolInfoSocket);
        return IPDR_OK;
    }

    /* create a UDP socket which will listen to request on the info port */

    Status = Platform_CreateUDPSocket(EXPORTER_PLATFORM,
                                      & (E->ProtocolInfoSocket));
    if (FAILURE(Status)) {
        IPDR_SetInvalidSocket(&E->ProtocolInfoSocket);
        return Status;
    }

    Status = Platform_Bind(EXPORTER_PLATFORM, E->ProtocolInfoSocket,
                           E->ProtocolInfoAddr, E->ProtocolInfoPort);
    if (FAILURE(Status)) {
        Platform_CloseAndInvalidate(EXPORTER_PLATFORM, &E->ProtocolInfoSocket);
        return Status;
    }

#ifdef TRACE_MAINFUNC
    fprintf(stderr, "RegisterReadSocket %d [UDP] ProtocolInfo\n", E->ProtocolInfoSocket); fflush(stderr);
#endif
    Status = Platform_RegisterReadSocket(EXPORTER_PLATFORM,
                                         E->ProtocolInfoSocket,
                                         Exporter_ProtocolInfoCallback,
                                         E,
                                         & (E->ProtocolInfoEvent));
    if (FAILURE(Status)) {
        Platform_CloseAndInvalidate(EXPORTER_PLATFORM, &E->ProtocolInfoSocket);
    }

    return IPDR_OK;
}


/**
 * Handle reconfiguration of the client (transmitter) due to changes in parameters
 * (this may close the current connections open by the client (transmitter) to
 * server (collector)s and open connections to the console).
 *
 * @param E exporter handle (pointer) to reconfigure
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
Exporter_Reconfigure(Exporter *E)
{
    Session *S;
	Collector *Coll;
	IPDRConnection *IC;
    int i, j;
    IPDR_IPAddress Addr;
    unsigned short Port;
    ChunkAllocator Alloc;
    const char *LocalAddr;
	IPDR_BOOL transportRestarted = IPDR_FALSE;

    if (! E) {
        return IPDR_OK;
    }

	Platform_Lock(EXPORTER_PLATFORM);

#if defined(SNMP_SUPPORT)
	Platform_MIB_UnSetExporterHandle(E->PS);
#endif

	Platform_UnregisterTimerAndClear(EXPORTER_PLATFORM, &(E->StatisticsTimerID));
	Platform_UnregisterTimerAndClear(EXPORTER_PLATFORM, &(E->AutoconnectTimerID));

    /* reconfigure the console */
    Console_Reconfigure(& (E->ConsolePtr), & (E->ParamsHandler));

    /* reconfigure the transport layer - (used to reconfigure memory usage) */
    TL_Reconfigure(EXPORTER_TRANSPORT);

    /* check if the client port / address have changed and recreate
       the transport layer if needed */

    LocalAddr = ParametersHandler_GetStringParam(& (E->ParamsHandler),
                                                 STR_configuration_exporterAddress);
    if (LocalAddr == NULL || strlen(LocalAddr) == 0)
        Addr = IPDR_DEFAULT_ADDRESS;
    else
        Addr = TL_ParseAddress(LocalAddr);

    Port =
        (unsigned short) ParametersHandler_GetIntParam(& (E->ParamsHandler),
                                                       STR_configuration_exporterPort);

    if (E->LocalAddr != Addr || E->LocalPort != Port) {

		transportRestarted = IPDR_TRUE ;
        E->LocalAddr = Addr;
        E->LocalPort = Port;

		for ( i = 0 ; i < E->Sessions.Length ; i++ )
		{
			S = (Session *)E->Sessions.Data[i];
			if(S != NULL)
			{
				IPDR_StopSession(S, IPDR_STOP_REASON_EXIT, 1000);
			}
		}
		for ( i = 0 ; i < E->DummyCollectors.Length ; i++ )
		{
			Coll = (Collector *)E->DummyCollectors.Data[i];
			if(Coll != NULL)
			{
				for ( j = 0 ; j < Coll->IPDRConnections.Length ; j++ )
				{
					IC = (IPDRConnection *)Coll->IPDRConnections.Data[j];
					if(IC && IC->TCPConnection)
					{
						TL_HandleDisconnection(IC->TCPConnection);
					}
				}
			}
		}
		for ( i = 0 ; i < E->Collectors.Length ; i++ )
		{
			Coll = (Collector *)E->Collectors.Data[i];
			if(Coll != NULL)
			{
				for ( j = 0 ; j < Coll->IPDRConnections.Length ; j++ )
				{
					IC = (IPDRConnection *)Coll->IPDRConnections.Data[j];
					if(IC && IC->TCPConnection)
					{
						TL_HandleDisconnection(IC->TCPConnection);
					}
				}
			}
		}

        /* shutdown the current client's transport layer */
        TL_Shutdown(EXPORTER_TRANSPORT);

        /* recreate a new transport layer */
        ChunksManager_GetDefaultChunkAllocator(E->PS, & Alloc);

        /* initialize the transport layer */
        /* We don't check the return code because what can we do
           anyway if it's bad? But this is not good style ... */
        (void)TL_InitializePassive(E->PS, & (E->ExporterLog), Alloc, E->LocalAddr, E->LocalPort,
                                   & Exporter_DefaultHandlers, E,
                                   & (E->ParamsHandler),
                                   & (E->TransportHandle));
	}

    /* reinitialize the client parameters */
    Exporter_InitializeParameters(E);

	Exporter_InitDebugLevels(E);

    /* reconfigure the current queues */
    for (i = 0; i < E->Sessions.Length; i++) {
        S = (Session *) E->Sessions.Data[i];
        if (EXPORTER_QHANDLER) {
            EXPORTER_QHANDLER->Reconfigure(E, S);
        }
    }

	if (E->Parameters.StatisticsInterval)
	{
		Platform_RegisterPeriodicTimer(EXPORTER_PLATFORM,
									Exporter_StatisticsReportCallback, E,
									E->Parameters.StatisticsInterval,
									& (E->StatisticsTimerID));
    }

	if(E->Parameters.AutoconnectInterval)
	{
		Platform_RegisterPeriodicTimer(EXPORTER_PLATFORM,
									Exporter_AutoconnectCallback, E,
									E->Parameters.AutoconnectInterval,
									& (E->AutoconnectTimerID));
	}

#if defined(SNMP_SUPPORT)
	Platform_MIB_SetExporterHandle(E->PS, E);
#endif

	if(transportRestarted == IPDR_TRUE)
	{
		for ( i = 0 ; i < E->Sessions.Length ; i++ )
		{
			S = (Session *)E->Sessions.Data[i];
			if(S != NULL)
			{
				IPDR_StartSession( S, S->Negotiable );
			}
		}
	}


	Platform_Unlock(EXPORTER_PLATFORM);

    return IPDR_OK;
}

IPDR_API
IPDR_ReconfigExporter(IPDR_ExporterHandle ExporterHandle)
{
	return Exporter_Reconfigure(ExporterHandle);
}

int
Exporter_GetCapabilities(Exporter *E)
{
	int  exporterCapabilities = 0;
	if(E->Parameters.caps.StructureSupport == IPDR_TRUE)
		exporterCapabilities = exporterCapabilities | 0x1u ;
	if(E->Parameters.caps.MultiSession == IPDR_TRUE)
		exporterCapabilities = exporterCapabilities | 0x2u ;
	if(E->Parameters.caps.TemplateNegotiation == IPDR_TRUE)
		exporterCapabilities = exporterCapabilities | 0x4u ;
	return exporterCapabilities ;
}


/*
 **********************
 * IPDR API function *
 **********************
 */
int IPDR_MIBVersion(IPDR_ExporterHandle ExporterHandle)
{
    return 3;
}


IPDR_API
IPDR_ParamGet(	IPDR_ExporterHandle ExporterHandle,
				const char *Name,
				char *Value, int valueLen)
{
	Exporter *E = ExporterHandle ;
	if(! E || !Name || !Value || ! valueLen)
		return IPDR_ERR ;

	{
		const char* p = ParametersHandler_GetStringParam(& (E->ParamsHandler), Name);
		if(p)
		{
			strncpy(Value, p, valueLen);
		}
	}
	return IPDR_OK ;
}

IPDR_API
IPDR_GetFirstParam(	IPDR_ExporterHandle ExporterHandle,
					unsigned long *Index,
					char** Name, int nameLen,
					char** Value, int valueLen,
					char** DefValue, int defValueLen)
{
	IPDR_Status Status = IPDR_OK;
	Exporter *E = ExporterHandle ;
	if(! E || !Name || !Value || ! valueLen)
		return IPDR_ERR ;

	{
		char *Name1, *Value1, *DefValue1;
		Status = ParametersHandler_GetFirstParamName(Index, &Name1, &Value1, &DefValue1);
		if(SUCCESS(Status))
		{
			strncpy((char *)Name, Name1, nameLen);
			strncpy((char *)Value, Value1, valueLen);
			strncpy((char *)DefValue, DefValue1, defValueLen);
		}
	}
	return Status ;
}


IPDR_API
IPDR_GetNextParam(	IPDR_ExporterHandle ExporterHandle,
					unsigned long *Index,
					char** Name, int nameLen,
					char** Value, int valueLen,
					char** DefValue, int defValueLen)
{
	IPDR_Status Status = IPDR_OK;
	Exporter *E = ExporterHandle ;
	if(! E || !Name || !Value || ! valueLen)
		return IPDR_ERR ;

	{
		char *Name1, *Value1, *DefValue1;
		Status = ParametersHandler_GetNextParamName(Index, &Name1, &Value1, &DefValue1);
		if(SUCCESS(Status))
		{
			strncpy((char *)Name, Name1, nameLen);
			strncpy((char *)Value, Value1, valueLen);
			strncpy((char *)DefValue, DefValue1, defValueLen);
		}
	}
	return Status ;
}



IPDR_API
IPDR_ParamSet(	IPDR_ExporterHandle ExporterHandle,
				const char *Name,
				const char *Value)
{
	Exporter *E = ExporterHandle ;
	if(! E || !Name || !Value)
		return IPDR_ERR ;

	return ParametersHandler_SetStringParam(& (E->ParamsHandler), Name, Value);
}


/**
 * Given a chunks manager, session, template and raw data construct a
 * new record in the chunks manager.
 *
 * @param CMgr chunks manager.
 * @param S session handle (pointer).
 * @param Tmpl template.
 * @param Data the data to be output (as described by the template)
 * @param BH buffer handle referencing the new record (output)
 * @param RecordSize size of generated record (output)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
Exporter_BuildRecord(ChunksManager *CMgr,
                   Session *S,
                   struct IPDR_tag_Template *Tmpl,
                   const void *Data,
                   BufferHandle *BH,
                   unsigned long *RecordSize)
{
    DataMessage *Header;

    IPDR_Status Status = Tmpl->BuildFunc(CMgr, S, (Template *) Tmpl,
                                          Data, BH, RecordSize);
    if (FAILURE(Status))
        return Status;

    Header = (DataMessage *) BH->Data;
    ExporterProtocol_FillHeader(S, BH->Data, IPDR_MSG_DATA, (*RecordSize));

    IPDR_SET_HTONS(Header->TemplateID, Tmpl->TemplateID);
	IPDR_SET_HTONS(Header->ConfigurationID, S->ActiveContext->ConfigurationID);
    IPDR_SET_HTONLL(Header->sequenceNum, S->DSN);
	IPDR_SET_HTONL(Header->dataLength, (*RecordSize) - sizeof(DataMessage));
    return IPDR_OK;
}


/**
 * Called before sending a record in order to validate the window
 * size.
 *
 * @param E exporter hadle (pointer)
 * @param S session handle (pointer).
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
Exporter_PrepareToSend(Exporter *E,
                     Session *S)
{
    CollectorSession *CS;
    TransmissionContext *TC = S->ActiveContext;

    if (TC == NULL)
        return IPDR_ERR_NO_ACTIVE_COLLECTOR;

    CS = S->ActiveCS;
    if (CS == NULL || /*! Srv->Conn || */CS->CSState != IPDR_CS_ACTIVE) {
        return IPDR_ERR_NO_ACTIVE_COLLECTOR;
    }

    /* The following if statement is wrong ... it sets the timeout
       only when we exceed the window. But we should have the timer
       going for all records, using the window to stop sending
       anything more unless we have an acknowledgment. Instead, the
       code is now in Exporter_SendRecord(). */

    /* if (EXPORTER_QHANDLER->OutstandingDataCount(C, S) >
       C->Parameters.WindowSize &&
       Srv->WindowSizeTimer == NULL) {
    */

    /* There is no good reason to check for window size ... the TCP buffer
       will almost always fill up sooner unless something is awfully wrong
       with the receiver (the TCP layer is ACKing but the collector can't
       handle the records. So, don't bother doing this test. */

#if defined(USE_WINDOWSIZE)
    if (EXPORTER_QHANDLER->OutstandingDataCount(E, S) >= E->Parameters.WindowSize) {
        /* Window is full ... don't send anything.  Note that this isn't
           an error in the normal sense ... it's just our way of
           indicating the window is full and we're not sending anything
           more. */
		Log_Write2(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_INFO,
			   "Exporter_PrepareToSend", "Sent Packets <%lu> and WindowSize is <%li> wait for next try..." , EXPORTER_QHANDLER->OutstandingDataCount(E, S), E->Parameters.WindowSize);
        return IPDR_ERR;
    }
#endif

    return IPDR_OK;
}


/**
 * Send a record to the currently active server.
 *
 * @param E exporter handle (pointer)
 * @param S session handle (pointer).
 * @param BH buffer handle referencing the record.
 * @param Immediate whether or not to send this immediately or try
 *          to delay it to get more into the buffer (recommended
 *          to leave this as TRUE because if we're outputting to
 *          TCP, the TCP stack will typically delay 200ms anyway).
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
Exporter_SendRecord(Exporter *E,
                  Session *S,
                  BufferHandle *BH,
                  IPDR_BOOL Immediate,
				  IPDR_BOOL duplicate)
{
    IPDR_Status Status;
    CollectorSession *CS = S->ActiveCS;

    if (CS == NULL
		|| CS->SessionConnection == NULL
		|| CS->SessionConnection->TCPConnection == NULL
		|| CS->CSState != IPDR_CS_ACTIVE) {
        return IPDR_ERR_NO_ACTIVE_COLLECTOR;
    }

	((DataMessage *) BH->Data)->Flags = 0;

	/* it changed in IPDR no need o fsync flag only duplicate flag need
    if (S->SyncNeeded)
        ((DataMessage *) BH->Data)->Flags |= IPDR_SYNC_FLAG;
	*/
    if (duplicate)
        ((DataMessage *) BH->Data)->Flags |= IPDR_DUPLICATE_FLAG;

    switch (((MessageHeader*)(BH->Data))->MessageID) {
    case IPDR_MSG_DATA:
        /* Register a timer if there isn't one already (i.e., we're the
           first record that hasn't been ACKed). */
        if (CS->WindowSizeTimer == NULL) {
            Platform_RegisterTimer(EXPORTER_PLATFORM,
                                   Exporter_WindowSizeTimerCallback, CS,
                                   E->Parameters.WindowTimeout,
                                   & (CS->WindowSizeTimer));
        }
        /*FALLTHROUGH*/
    case IPDR_MSG_DATA_ACK:
        if (Log_ShouldWrite(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_DEBUG)) {
            ProtocolDebug_PrintOutgoingMessage(EXPORTER_LOG, CS->SessionConnection->TCPConnection, BH->Data, IPDR_LEVEL_DEBUG);
        }
        break;
    default:
        if (Log_ShouldWrite(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_INFO)) {
            ProtocolDebug_PrintOutgoingMessage(EXPORTER_LOG, CS->SessionConnection->TCPConnection, BH->Data, IPDR_LEVEL_INFO);
        }
    }

    Status = TL_Send(CS->SessionConnection->TCPConnection, BH, Immediate);
    if (FAILURE(Status))
        return Status;

    S->Statistics.TransmittedRecords++;
    CS->Statistics.TransmittedRecords++;

    S->SyncNeeded = IPDR_FALSE;

    return IPDR_OK;
}

/**
 * Create a new session with the client.
 * @see IPDR_CreateSession
 *
 * @param ExporterHandle handle to a previously created client
 * @param Name session name
 * @param ID session ID
 * @param Description session description
 * @param SH handle to the newly created session
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
Exporter_CreateSessionInternal(IPDR_ExporterHandle ExporterHandle,
                             const char *Name,
                             unsigned char ID,
                             const char *Description,
                             IPDR_SessionHandle *SH)
{
    Exporter *E = ExporterHandle;
    Session *S;
    int Index;
    IPDR_Status Status;

	if(!ID || !E || !Name)
		return IPDR_ERR_INVALID_VALUE;

    Platform_Lock(EXPORTER_PLATFORM);

    *SH = NULL; /* what we return if there's an error */

    if (SUCCESS(IPDR_GetSession(E, ID, SH, NULL, NULL))) {
        Platform_Unlock(EXPORTER_PLATFORM);
        return IPDR_ERR_SESSION_ALREADY_EXISTS;
    }

#if 0
    if (0) {
        unsigned int numSessions;
        DynamicArray_CountObjects(EXPORTER_PLATFORM, &C->Sessions, &numSessions);
        if (numSessions > 0) {
            /* XXX Restriction: only one session because the templates'
             * data are "global" and not specialized to a single session
             * (if desired). This will need to be fixed in the future; but
             * for now, just document it and make sure that mistakes
             * aren't made. */
            return IPDR_ERR_TOO_MANY_SESSIONS;
        }
    }
#endif

    Status = Object_Allocate(EXPORTER_PLATFORM, sizeof(Session),
                             Session_Destructor,
                             (void **) & S);

    if (FAILURE(Status)) {
        Platform_Unlock(EXPORTER_PLATFORM);
        return Status;
    }
    strncpy(S->eyecatcher, "SESSION", sizeof S->eyecatcher);


    Status = Util_StrDup(EXPORTER_PLATFORM, Name, & (S->Name));
    if (FAILURE(Status)) {
        Object_Release(EXPORTER_PLATFORM, S);
        Platform_Unlock(EXPORTER_PLATFORM);
        return Status;
    }

    Status = Util_StrDup(EXPORTER_PLATFORM, Description, & (S->Description));
    if (FAILURE(Status)) {
        Object_Release(EXPORTER_PLATFORM, S);
        Platform_Unlock(EXPORTER_PLATFORM);
        return Status;
    }

    S->ID = ID;
    S->ExporterPtr = E;
    S->ActiveContext = S->WorkContext = NULL;
    S->ActiveCS = NULL;
    S->DSN = IPDR_INITIAL_DSN;
    S->SyncNeeded = IPDR_TRUE;
    S->SendingData = IPDR_FALSE;
	S->Retransmite = IPDR_FALSE;
	S->Running = IPDR_FALSE; /*Platform_IsRunning(EXPORTER_PLATFORM);*/
    S->CCollectors = NULL;
	S->TemplatesChnged = IPDR_TRUE;
	S->Negotiable = E->Parameters.caps.TemplateNegotiation;
	S->CCollectorsLength = 0;

    EXPORTER_QHANDLER->CreateSession(E, S);

    StatisticsInitialize(EXPORTER_PLATFORM, &S->PrevStatisticsTime, &S->PrevStatistics, &S->PrevStatistics);

    Status = DynamicArray_AddElement(EXPORTER_PLATFORM, & (E->Sessions), S,
                                     & Index);
    if (FAILURE(Status)) {
        Object_Release(EXPORTER_PLATFORM, S);
        Platform_Unlock(EXPORTER_PLATFORM);
        return Status;
    }

    if (E->Started) {
		Session_CreateNewWorkContext(E, S, & (S->Templates), IPDR_TRUE);
    }

    *SH = (IPDR_SessionHandle) S;

    Log_Write3(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_INFO,
               "Exporter_CreateSessionInternal",
               MSG_EXPORTER_ADD_SESSION, ID, Name, Description);

    Platform_Unlock(EXPORTER_PLATFORM);
    return IPDR_OK;
}


/**
 * Delete a previously created session.
 * @see IPDR_DeleteSession
 *
 * @param ExporterHandle handle to a previously created client
 * @param S handle to the session
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static IPDR_Status
Exporter_DeleteSessionInternal(IPDR_ExporterHandle ExporterHandle,
                             Session* S)
{
    Exporter *E = ExporterHandle;
    int i, j;
    IPDR_Status Status;

	if(!E || !S)
		return IPDR_ERR_INVALID_VALUE;

	Platform_Lock(EXPORTER_PLATFORM);
	if(S->ActiveCS != NULL || S->Running == IPDR_TRUE)
	{
		Log_Write2(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_INFO,
			   "Exporter_DeleteSessionInternal", MSG_EXPORTER_ACTIVE_COLLECTOR_EXISTS, S->ID, S->Name);
		Platform_Unlock(EXPORTER_PLATFORM);
		return IPDR_ERR_ACTIVE_COLLECTOR_EXISTS;
	}

    for (i = 0; i < E->Sessions.Length; i++)
	{
        Session *S1 = (Session *) E->Sessions.Data[i];
        if (S1 == S)
		{
            Log_Write1(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_INFO,
                       "Exporter_DeleteSessionInternal",
                       MSG_EXPORTER_REMOVE_SESSION, S->ID);

			for (j = 0; j < S->CollectorSessions.Length; j++)
			{
				CollectorSession *CS = (CollectorSession *)S->CollectorSessions.Data[j];
				if(CS != NULL && CS->Collector && CS->SessionConnection != NULL &&
					CS->SessionConnection->TCPConnection)
				{
					Status = ExporterProtocol_SendError(E, S, CS->Collector, CS->SessionConnection->TCPConnection, CollectorNotInSession, MSG_EXPORTER_COLLECTOR_NOT_IN_SESSION);
					if(FAILURE(Status))
					{
						TL_HandleDisconnection(CS->SessionConnection->TCPConnection);
					}
				}
			}

            Status = DynamicArray_RemoveAndShift(EXPORTER_PLATFORM, & (E->Sessions), i);

			Platform_Unlock(EXPORTER_PLATFORM);
            return Status;
        }
    }

    Platform_Unlock(EXPORTER_PLATFORM);
    return IPDR_ERR_UNABLE_TO_FIND_SESSION;
}


/**
 * Add a collector to a list of servers (collectors) list.
 * @see IPDR_AddCollector
 *
 * @param E handle to a previously created exporter
 * @param Name name of server (collector)
 * @param Addr IP address of the server (collector)
 * @param Port port number of the server (collector)
 * @param NumberOfActive number of active collectors to allow
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
Exporter_AddCollectorInternal(Exporter *E,
                         const char *Name,
                         IPDR_IPAddress Addr,
                         unsigned short Port,
                         unsigned short NumberOfActive,
						 IPDR_BOOL isDummy,
						 IPDR_CollectorHandle* CH)
{
    Collector*		Coll;
    IPDR_Status		Status;
	int 			Index;
    TL_PABuf 		paBuf;

    if (!Name) {
        Name = "";
    }

	if(CH) *CH = NULL ;

    Platform_Lock(EXPORTER_PLATFORM);

	for (Index = 0; isDummy == IPDR_FALSE && Index < E->Collectors.Length; Index++) {
        Coll = (Collector *) E->Collectors.Data[Index];
        if (Coll->Addr == Addr || strcmp(Coll->Name, Name) == 0) {
			Log_Write1(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_INFO,
                       "Exporter_AddCollectorInternal", "Collector '%s' already exists", Name);

			Platform_Unlock(EXPORTER_PLATFORM);
            return IPDR_ERR_COLLECTOR_ALREADY_EXISTS;
        }
    }

    Status = Object_Allocate(EXPORTER_PLATFORM, sizeof(Collector),
                             Collector_Destructor,
                             (void **) & Coll);
    if (FAILURE(Status)) {
        Platform_Unlock(EXPORTER_PLATFORM);
        return Status;
    }

	Status = DynamicArray_Create(EXPORTER_PLATFORM, & (Coll->IPDRConnections),
                                 INITIAL_COLLECTOR_CONNECTIONS_ARRAY_CAPACITY);

	if (FAILURE(Status)) {
        Object_Release(EXPORTER_PLATFORM, Coll);
        Platform_Unlock(EXPORTER_PLATFORM);
        return Status;
    }

	Status = DynamicArray_Create(EXPORTER_PLATFORM, & (Coll->CollectorSessions),
                                 INITIAL_COLLECTOR_COLLECTORSESSIONS_ARRAY_CAPACITY);

	if (FAILURE(Status)) {
        Object_Release(EXPORTER_PLATFORM, Coll);
        Platform_Unlock(EXPORTER_PLATFORM);
        return Status;
    }

    strncpy(Coll->eyecatcher, "COLLECTOR", sizeof(Coll->eyecatcher));
    {
        //char* tmpName;
        Status = Util_StrDup(EXPORTER_PLATFORM, Name, &(Coll->Name));
        //Coll->Name = tmpName;
    }
    if (FAILURE(Status)) {
        Object_Release(EXPORTER_PLATFORM, Coll);
        Platform_Unlock(EXPORTER_PLATFORM);
        return Status;
    }

    Coll->Addr = Addr;
    Coll->Port = Port;
    Coll->NumOfConnections = NumberOfActive;
	Coll->ExporterPtr = E;
	Coll->isDummy = isDummy ;

	for(Index = 0; Index < NumberOfActive; Index++)
	{
		Status = Exporter_AddIPDRConnectionToCollector(	E, Coll, NULL );
		if(FAILURE(Status))
		{
			Log_Write(EXPORTER_LOG, PROTOCOL_MODULE_ID, IPDR_LEVEL_ERROR,
               	   "Exporter_AddCollectorInternal", "Failed To Add IPDRConnection To Collector");
		}
	}

	if(IPDR_TRUE == isDummy)
	{
		Status = DynamicArray_AddElement(EXPORTER_PLATFORM,
									& (E->DummyCollectors), Coll,
									& Index);
	}
    else
	{
		Status = DynamicArray_AddElement(EXPORTER_PLATFORM,
									& (E->Collectors), Coll,
									& Index);
	}

	if (FAILURE(Status)) {
        Object_Release(EXPORTER_PLATFORM, Coll);
        Platform_Unlock(EXPORTER_PLATFORM);
        return Status;
    }


    Log_Write4(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_INFO,
               "Exporter_AddCollectorInternal",
               MSG_EXPORTER_ADD_SERVER, Name,
               TL_PrintAddressAndPort(Addr, Port, paBuf),
               NumberOfActive,(IPDR_TRUE==isDummy)?"Dummy":"Real");

	if(CH) *CH = Coll ;

    Platform_Unlock(EXPORTER_PLATFORM);
    return IPDR_OK;
}


/**
 * Associates collector with the session
 * @see IPDR_AssocCollectorSession
 *
 * @param E - Exporter
 * @param S - Session to associate
 * @param C - Collector to associate
 * @param Priority - Priority to associate
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status Exporter_AssocCollectorSession_Internal(Exporter* E,
											Session* S,
											Collector* C,
											unsigned short Priority)
{
	int 				Index, SIndex, CIndex ;
    IPDR_Status 		Status;
	CollectorSession* 	CS = NULL ;
	CollectorSession* 	lookForCS = NULL ;
	TL_PABuf 			paBuf;

	if ( !E || !S || !C ) {
        return IPDR_ERR_INVALID_VALUE;
    }

	Platform_Lock(EXPORTER_PLATFORM);

	if(S->Running == IPDR_TRUE)
	{
		Log_Write2(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_ERROR,
				"Exporter_AssocCollectorSession_Internal",
				MSG_EXPORTER_SESSION_MUST_STOP2,
				S->ID, C->Name);
		Platform_Unlock(EXPORTER_PLATFORM);
		return IPDR_ERR_SESSION_IS_RUNNING;
	}


	for(Index = 0;Index < E->CollectorSessions.Length ; Index++)
	{
		lookForCS = E->CollectorSessions.Data[Index];

		if( (lookForCS->SessionPtr == S) && (lookForCS->Collector == C) )
		{
			CS = lookForCS;
		}
	}

	/* Association exists, just change priority*/
	if( CS != NULL )
	{
		Log_Write4(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_INFO,
               "Exporter_AssocCollectorSession_Internal",
               MSG_EXPORTER_ASSOC_COLL_SESSION,
			   C->Name,
               TL_PrintAddress(C->Addr, paBuf),
			   S->ID,
               Priority);
		CS->Priority = Priority;
		Platform_Unlock(EXPORTER_PLATFORM);
		return IPDR_ERR_COLLECTORSESSION_REASSOC;
	}

	/* Association does not exists yet. Need to make a new one*/
	Status = Object_Allocate(EXPORTER_PLATFORM, sizeof(CollectorSession),
							CollectorSession_Destructor,
							(void **) & CS);
	if (FAILURE(Status)) {
		Platform_Unlock(EXPORTER_PLATFORM);
		return Status;
	}

	strncpy(CS->eyecatcher, "COLLSESS", sizeof(CS->eyecatcher));

	CS->CSState = IPDR_CS_DISCONNECTED;
	CS->ExporterPtr = E;
	CS->SessionPtr = S;
	CS->Collector = C;
	CS->SessionConnection = NULL;
	CS->Priority = Priority;

    /*
	TODO:
	TmplDataTimer;
    WindowSizeTimer;
	  IPDR_Statistics Statistics;
    IPDR_Time PrevStatisticsTime;
	    IPDR_Statistics PrevStatistics;
	    IPDR_EventData LastEvent;*/


	Status = DynamicArray_AppendElement(EXPORTER_PLATFORM, & (E->CollectorSessions), CS, &Index);
    if (FAILURE(Status)) {
        Object_Release(EXPORTER_PLATFORM, CS);
        Platform_Unlock(EXPORTER_PLATFORM);
        return Status;
    }


    Status = DynamicArray_AppendElement(EXPORTER_PLATFORM, & (S->CollectorSessions), CS, &SIndex);
    if (FAILURE(Status)) {
		Object_Release(EXPORTER_PLATFORM, CS);
        Platform_Unlock(EXPORTER_PLATFORM);
        return Status;
    }

    Status = DynamicArray_AppendElement(EXPORTER_PLATFORM, & (C->CollectorSessions), CS, &CIndex);
    if (FAILURE(Status)) {
		Object_Release(EXPORTER_PLATFORM, CS);
        Platform_Unlock(EXPORTER_PLATFORM);
        return Status;
    }

	Log_Write4(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_INFO,
               "Exporter_AssocCollectorSession_Internal",
               MSG_EXPORTER_ASSOC_COLL_SESSION,
			   C->Name,
               TL_PrintAddress(C->Addr, paBuf),
			   S->ID,
               Priority);

#ifdef NOTDONEYET
    ExporterMIB_UpdateTemplates(S);
#endif
    Platform_Unlock(EXPORTER_PLATFORM);

	return IPDR_OK;
}


static IPDR_Status
Exporter_RemoveCollectorFromSession_Internal(Exporter *E, Session *S, Collector *C)
{
    int Index;
    IPDR_Status Status = IPDR_OK;
	CollectorSession *CS = NULL ;
	TL_PABuf paBuf;

	if(!E || !S || !C)
		return IPDR_ERR_INVALID_VALUE;

    Log_Write1(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_INFO,
                       "Exporter_RemoveCollectorFromSession_Internal",
                       MSG_EXPORTER_REMOVE_SERVER,
                       TL_PrintAddressAndPort(C->Addr, C->Port, paBuf));

	Platform_Lock(EXPORTER_PLATFORM);

	for (Index = 0; Index < S->CollectorSessions.Length; Index++)
	{
		CS = (CollectorSession *)S->CollectorSessions.Data[Index];
		if(CS != NULL && CS->Collector == C )
		{
			if(CS->CSState == IPDR_CS_ACTIVE || S->Running == IPDR_TRUE)
			{
				IPDRConnection *IC = CS->SessionConnection;

				Log_Write3(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_ERROR,
						"Exporter_RemoveCollectorFromSession_Internal",
						MSG_EXPORTER_SESSION_MUST_STOP,
						S->ID, C->Name, TL_GetConnectionInfoStr((IC!=NULL)?(IC->TCPConnection):NULL));

				Platform_Unlock(EXPORTER_PLATFORM);
				return IPDR_ERR_SESSION_IS_RUNNING;
			}
			break ;
		}
		CS = NULL;
	}

	if( CS == NULL )
	{
		Log_Write1(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_ERROR,
					"Exporter_RemoveCollectorFromSession_Internal",
					MSG_EXPORTER_UNKNOWN_SERVER,
					C->Name);

		Platform_Unlock(EXPORTER_PLATFORM);
		return IPDR_ERR_COLLECTOR_NOT_FOUND;
	}

	IPDRConnection *IC = CS->SessionConnection;
	if(IC != NULL && IC->TCPConnection != NULL)
	{
		Connection Conn = IC->TCPConnection;
		if(FAILURE(ExporterProtocol_SendError(E, S, C, Conn, CollectorNotInSession, MSG_EXPORTER_COLLECTOR_NOT_IN_SESSION)))
		{
			TL_HandleDisconnection(Conn);
		}
	}
	Object_Release(EXPORTER_PLATFORM, CS);

    Platform_Unlock(EXPORTER_PLATFORM);

    return Status;
}

IPDR_API
IPDR_MIB_Start(IPDR_PlatformServices *PS)
{
#if defined(SNMP_SUPPORT)
	return Platform_MIB_Start(PS);
#else
	return IPDR_OK ;
#endif
}

IPDR_API
IPDR_MIB_Stop(IPDR_PlatformServices *PS)
{
#if defined(SNMP_SUPPORT)
	Platform_MIB_Shop(PS);
#endif
	return IPDR_OK ;
}


IPDR_BOOL
IPDR_MIB_IsRunning(IPDR_PlatformServices *PS)
{
#if defined(SNMP_SUPPORT)
	return Platform_MIB_IsRunning(PS);
#else
	return IPDR_FALSE;
#endif
}

/**
 * Remove a previously added collector)
 * @see IPDR_RemoveCollector
 *
 * @param E - Exporter
 * @param C - Collector to remove
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static IPDR_Status
Exporter_RemoveCollectorInternal(Exporter* E, Collector* C)
{
    int Index;

	TL_PABuf paBuf;

	if(!E || !C)
		return IPDR_ERR_INVALID_VALUE;

    Log_Write1(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_INFO,
                       "Exporter_RemoveCollectorInternal",
                       MSG_EXPORTER_REMOVE_SERVER,
                       TL_PrintAddressAndPort(C->Addr, C->Port, paBuf));

    Platform_Lock(EXPORTER_PLATFORM);

	for (Index = 0; Index < E->Collectors.Length; Index++)
	{
		if( E->Collectors.Data[Index] == C )
			break ;
	}

	if( Index >= E->Collectors.Length )
	{
		Log_Write1(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_ERROR,
		"Exporter_RemoveCollectorInternal",
		MSG_EXPORTER_UNKNOWN_SERVER,
		C->Name);
		/* DONE: IPDR_LEVEL_ERROR because this function is called after "IPDR_GetCollector" returns IPDR_OK in "Exporter_DeleteCollectorCommand"*/
		Platform_Unlock(EXPORTER_PLATFORM);
		return IPDR_ERR_COLLECTOR_NOT_FOUND;
	}

	for (Index = 0; Index < C->CollectorSessions.Length; Index++)
	{
		CollectorSession *CS = (CollectorSession *)C->CollectorSessions.Data[Index];
		if( CS != NULL )
		{
			Session *S = CS->SessionPtr;
			if(CS->CSState == IPDR_CS_ACTIVE || S->Running == IPDR_TRUE)
			{

				IPDRConnection *IC = CS->SessionConnection;

				Log_Write3(EXPORTER_LOG, EXPORTER_MODULE_ID, IPDR_LEVEL_ERROR,
						"Exporter_RemoveCollectorInternal",
						MSG_EXPORTER_SESSION_MUST_STOP,
						S->ID, C->Name, TL_GetConnectionInfoStr((IC!=NULL)?(IC->TCPConnection):NULL));

				Platform_Unlock(EXPORTER_PLATFORM);
				return IPDR_ERR_SESSION_IS_RUNNING;
			}
		}
	}

	Object_Release(EXPORTER_PLATFORM, C);
	//Status = DynamicArray_RemoveAndShift(EXPORTER_PLATFORM, & (E->Collectors), Index);

    Platform_Unlock(EXPORTER_PLATFORM);

    return IPDR_OK;
}


/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_SetLogOutputAddress(IPDR_ExporterHandle ExporterHandle,
                          IPDR_IPAddress Addr,
                          unsigned short Port,
                          unsigned int Facility,
                          const char* MachineName)
{
    Exporter *E = ExporterHandle;
    if (! E) {
        return IPDR_ERR_INVALID_VALUE;
    }
    return Log_SetLoggerAddress(EXPORTER_LOG, Addr, Port, Facility, MachineName);
}


/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_SetLogOutputCallbackDetailed(IPDR_ExporterHandle ExporterHandle,
                                   IPDR_LogCallbackDetailed CB,
                                   void *Arg)
{
    Exporter *E = ExporterHandle;
    if (! E) {
        return IPDR_ERR_INVALID_VALUE;
    }
    return Log_SetOutputCallbackDetailed(EXPORTER_LOG, CB, Arg);
}


#if defined(IPDR_PLATFORM_INDICATORS)
#include "Platforms/Generic/GenericPlatform.h"


/**
 * Get the enabled/stopped values from the platform.
 *
 * @note Could be IPDR API function.
 * Might be useful for debugging; but removed for now.
 * @param C
 * @param Enabled
 * @param Stopped
 */
void
IPDR_Indicators(IPDR_ExporterHandle ExporterHandle,
                 int *Enabled,
                 int *Stopped)
{
    GenericPlatform_Indicators(EXPORTER_PLATFORM->PlatformArg, Enabled, Stopped);
}
#endif


#if defined(IPDR_UTILITIES)


/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_DoConsoleCommand(IPDR_ExporterHandle ExporterHandle, FILE* Output, char* Cmd,
                       const char **ParsedCommand,
                       int *ParsedArgc,
                       const char ***ParsedArgs,
                       char ** RemainingCmd,
						char *CmdRes,
						int CmdResLen)
{
    Exporter *E = ExporterHandle;
    if (! E || ! Cmd) {
        return IPDR_ERR_INVALID_VALUE;
    }
    return Console_ActivateCommand(&E->ConsolePtr, NULL, Output, Cmd,
                                   ParsedCommand, ParsedArgc, ParsedArgs,
                                   RemainingCmd,
								   CmdRes, CmdResLen);
}

#endif


/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_MemoryAllocate(IPDR_ExporterHandle ExporterHandle,
                     size_t Size,
                     void **Buf) {
    Exporter *E = ExporterHandle;
    if (! E || !Buf ) {
        return IPDR_ERR_INVALID_VALUE;
    }
    /* Note the "X_" ... that's because most memory allocations are
     * done by the object handler or dynamic array handler; but this
     * one here is for bog standard users, so we allow it. */

    return X_Platform_MemoryAllocate(E->PS, Size, Buf);
}


/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_MemoryFree(IPDR_ExporterHandle ExporterHandle,
                 void *Buf) {
    Exporter *E = ExporterHandle;
	if (! E ) {
        return IPDR_ERR_INVALID_VALUE;
    }
    return Platform_MemoryFree(E->PS, Buf);
}


/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_MemoryStatistics(IPDR_ExporterHandle ExporterHandle,
                       unsigned int *numAllocs,
                       unsigned int *numFrees,
                       size_t *bytesAllocated) {
    Exporter *E = ExporterHandle;
	if (! E ) {
        return IPDR_ERR_INVALID_VALUE;
    }
    return Platform_MemoryStatistics(E->PS, numAllocs, numFrees, bytesAllocated);
}


/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_RegisterTimer(IPDR_ExporterHandle ExporterHandle,
                    IPDR_TimerCallback_t Callback,
                    void *Arg,
                    int Timeout,
                    IPDR_TimerID *ID) {
    Exporter *E = ExporterHandle;
	if (! E ) {
        return IPDR_ERR_INVALID_VALUE;
    }
    return Platform_RegisterTimer(E->PS, Callback, Arg, Timeout, ID);
}


/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_ReregisterTimer(IPDR_ExporterHandle ExporterHandle,
                      IPDR_TimerCallback_t Callback,
                      void *Arg,
                      int Timeout,
                      IPDR_TimerID* ID) {
    Exporter *E = ExporterHandle;
	if (! E ) {
        return IPDR_ERR_INVALID_VALUE;
    }
    return Platform_RegisterTimer(E->PS, Callback, Arg, Timeout, ID);
}


/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_RegisterPeriodicTimer(IPDR_ExporterHandle ExporterHandle,
                            IPDR_TimerCallback_t Callback,
                            void *Arg,
                            int Timeout,
                            IPDR_TimerID *ID) {
    Exporter *E = ExporterHandle;
	if (! E ) {
        return IPDR_ERR_INVALID_VALUE;
    }
    return Platform_RegisterPeriodicTimer(E->PS, Callback, Arg, Timeout, ID);
}


/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_UnregisterTimer(IPDR_ExporterHandle ExporterHandle,
                      IPDR_TimerID ID)
{
    Exporter *E = ExporterHandle;
	if (! E ) {
        return IPDR_ERR_INVALID_VALUE;
    }
    return Platform_UnregisterTimer(E->PS, ID);
}


/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_GetTime(IPDR_ExporterHandle ExporterHandle,
              IPDR_Time *Time)
{
    Exporter *E = ExporterHandle;
	if (! E ) {
        return IPDR_ERR_INVALID_VALUE;
    }
    return Platform_GetTime(E->PS, Time);
}


/*
 **********************
 * IPDR API function *
 **********************
 */
IPDR_Status
IPDR_SleepMS(IPDR_ExporterHandle ExporterHandle,
              long Interval)
{
    Exporter *E = ExporterHandle;
 	if (! E ) {
        return IPDR_ERR_INVALID_VALUE;
    }
	return Platform_SleepMS(E->PS, Interval);
}


/*
 **********************
 * IPDR API function *
 **********************
 */
char *
IPDR_FormatTimeLocalNow(IPDR_ExporterHandle ExporterHandle,
                         char* Buf)
{
    Exporter *E = ExporterHandle;
	return Platform_FormatTimeLocalNow(E->PS, Buf);
}


/*
 **********************
 * IPDR API function *
 **********************
 */
char *
IPDR_FormatTimeGMTNow(IPDR_ExporterHandle ExporterHandle,
                       char* Buf)
{
    Exporter *E = ExporterHandle;
    return Platform_FormatTimeGMTNow(E->PS, Buf);
}


/*
 **********************
 * IPDR API function *
 **********************
 */
char *
IPDR_FormatTimeLocal(IPDR_ExporterHandle ExporterHandle,
                      IPDR_Time Time,
                      char* Buf)
{
    Exporter *E = ExporterHandle;
    return Platform_FormatTimeLocal(E->PS, Time, Buf);
}


/*
 **********************
 * IPDR API function *
 **********************
 */
char *
IPDR_FormatTimeGMT(IPDR_ExporterHandle ExporterHandle,
                    IPDR_Time Time,
                    char* Buf)
{
    Exporter *E = ExporterHandle;

    return Platform_FormatTimeGMT(E->PS, Time, Buf);
}
