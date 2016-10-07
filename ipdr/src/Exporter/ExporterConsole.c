/** @file Exporter/ExporterConsole.c
 *
 * Implementation of client (transmitter) related console commands.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: ExporterConsole.c,v 1.36 2005/05/09 14:51:03 dmitryk Exp $
 *
 */

#include <IPDR/Config.h>
#include <Exporter/ExporterConsole.h>

const char ExporterConsole_rcsid[] = "@(#) $Id: ExporterConsole.c,v 1.36 2005/05/09 14:51:03 dmitryk Exp $";
const char ExporterConsole_h_rcsid[] = EXPORTER_CONSOLE_H_RCSID;

#include <stdlib.h>
#include <stdio.h>

#include <Exporter/Templates.h>
#include <Exporter/Version.h>
#include <Exporter/ExporterMsgs.h>

/* For the rcsid's "global" names: */

#include <Exporter/Exporter.h>
#include <Exporter/ExporterConsole.h>
#include <Exporter/ExporterMsgs.h>
#include <Exporter/ExporterProtocol.h>
#include <Exporter/ProtocolDebug.h>
#include <Exporter/Templates.h>
#include <Exporter/Version.h>
#include <Exporter/VolatileQueue.h>
#include <Exporter/VQHandler.h>
#include <Common/ChunksManager.h>
#include <Common/CommonMsgs.h>
#include <Common/CommonUtil.h>
#include <Common/Console.h>
#include <Common/DynamicArray.h>
#include <Common/Log.h>
#include <Common/MemoryManagement.h>
#include <Common/ParametersHandler.h>
#include <Common/Protocol.h>
#include <Platforms/Generic/GenericPlatform.h>
#include <Transports/TCP/TCPMsgs.h>
#include <Transports/TCP/TCPTransport.h>

/*
 * Forward declarations
 */

#if 1 || defined(ENABLE_SIMULATOR)
static IPDR_Status Exporter_AddSessionCommand(ConsoleSession *, FILE *, int , const char * const [], void *);
static IPDR_Status Exporter_DeleteSessionCommand(ConsoleSession *, FILE *, int , const char * const [], void *);
static IPDR_Status Exporter_AddCollectorCommand(ConsoleSession *, FILE *, int , const char * const [], void *);
static IPDR_Status Exporter_DeleteCollectorCommand(ConsoleSession *, FILE *, int , const char * const [], void *);
static IPDR_Status Exporter_SetConnectionParamsCommand(ConsoleSession *, FILE *, int , const char * const [], void *);
static IPDR_Status Exporter_DeleteCollectorFromSessionCommand(ConsoleSession *, FILE *, int , const char * const [], void *);
static IPDR_Status Exporter_StartSessionCommand(ConsoleSession *, FILE *, int , const char * const [], void *);
static IPDR_Status Exporter_StopSessionCommand(ConsoleSession *, FILE *, int , const char * const [], void *);
static IPDR_Status Exporter_ReconfigExporterCommand(ConsoleSession *, FILE *, int , const char * const [], void *);
#endif
#if 1 || defined(ENABLE_SIMULATOR)
static IPDR_Status Exporter_ApplyTemplatesCommand(ConsoleSession *, FILE *, int, const char * const [], void *);
#endif
static IPDR_Status Exporter_GetSessionsCommand(ConsoleSession *, FILE *, int , const char * const [], void *);
static IPDR_Status Exporter_GetTemplatesCommand(ConsoleSession *, FILE *, int, const char * const [], void *);
static IPDR_Status Exporter_GetVersionCommand(ConsoleSession *, FILE *, int, const char * const [], void *);
static IPDR_Status Exporter_GetTimeCommand(ConsoleSession *, FILE *, int, const char * const [], void *);
static IPDR_Status Exporter_SetTimeCommand(ConsoleSession *, FILE *, int, const char * const [], void *);
static IPDR_Status Exporter_GetParametersCommand(ConsoleSession *, FILE *, int, const char * const [], void *);
static IPDR_Status Exporter_GetParamCommand(ConsoleSession *, FILE *, int, const char * const [], void *);
static IPDR_Status Exporter_SetParamCommand(ConsoleSession *, FILE *, int, const char * const [], void *);
static IPDR_Status Exporter_LogCommand(ConsoleSession *, FILE *, int, const char * const [], void *);
static IPDR_Status Exporter_SetDocumentIDCommand(ConsoleSession *, FILE *, int, const char * const [], void *);


/**
 * List of client (transmitter) console commands.
 */
static ConsoleCommand
Exporter_ConsoleCommands[] = {
#if 1 || defined(ENABLE_SIMULATOR)
    { "add_session", "add_session <id> <name> <description>",
      Exporter_AddSessionCommand },
    { "delete_session", "delete_session <id>",
      Exporter_DeleteSessionCommand },
    { "add_collector" ,"add_collector <name> <ip> [<port> <connections>]",
      Exporter_AddCollectorCommand },
    { "delete_collector", "delete_collector <name>",
      Exporter_DeleteCollectorCommand },
	{ "assign_collector_to_session", "assign_collector_to_session <session id> <collector name> <priority>",
	  Exporter_SetConnectionParamsCommand },
    { "delete_collector_from_session", "delete_collector_from_session <name> <id>",
      Exporter_DeleteCollectorFromSessionCommand },
    { "start_session" ,"start_session <ID> <Negotiate (1/0)>",
      Exporter_StartSessionCommand },
    { "stop_session" ,"stop_session <ID> <reason (1..7)> <timeout (ms)>",
      Exporter_StopSessionCommand },
    { "reconfig_exporter" ,"reconfig_exporter []",
      Exporter_ReconfigExporterCommand },
#endif
    { "get_sessions", "get_sessions",
      Exporter_GetSessionsCommand },
#if 1 || defined(ENABLE_SIMULATOR)
    { "apply_templates", "apply_templates",
      Exporter_ApplyTemplatesCommand },
#endif
    { "get_templates", "get_templates <session_id>",
      Exporter_GetTemplatesCommand },
    { "set_document_id", "set_document_id <session_id> <document_id>",
      Exporter_SetDocumentIDCommand },
    { "version", "version",
      Exporter_GetVersionCommand },
    { "time", "time",
      Exporter_GetTimeCommand },
    { "set_time", "set_time <epoch_seconds>",
      Exporter_SetTimeCommand },
    { "parameters", "parameters",
      Exporter_GetParametersCommand },
    { "get_param", "get_param <name>",
      Exporter_GetParamCommand },
    { "set_param", "set_param <name> <value>",
      Exporter_SetParamCommand },
    { "log", "log [print|callback]",
      Exporter_LogCommand },
    { NULL, NULL, NULL }
};


/**
 * Register exporter related commands within the console.
 *
 * @param E exporter
 * @param C console
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status
ExporterConsole_RegisterConsoleCommands(Exporter *E,
                                      Console *C)
{
    return Console_RegisterModule(C, "exporter", E, Exporter_ConsoleCommands);
}


/**
 * Display the list of sessions on the console.
 *
 * @param CS console sessions
 * @param Out where to write any output
 * @param Argc number of arguments
 * @param Argv arguments
 * @param Arg context argument (Exporter)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static IPDR_Status
Exporter_GetSessionsCommand(ConsoleSession *CS,
                          FILE *Out,
                          int Argc,
                          const char * const Argv[],
                          void *Arg)
{
    Exporter *E = (Exporter *) Arg;
    int i, j;

	for (i = 0; i < E->Sessions.Length; i++) {
        Session *S = (Session *) E->Sessions.Data[i];
        char nowStr[IPDR_TIME_FORMAT_LENGTH+1];
        if (S == NULL) {
            Console_Println(CS, Out, "... nulled session (index: %d)", i);
        } else {
            Console_Println(CS, Out, "ID: %d, Name: %.40s, Description: %.80s, Started: %.80s [at: %s]",
                            S->ID, S->Name, S->Description, S->Running==IPDR_TRUE?"True":"False", Platform_FormatTimeLocalNow(E->PS, nowStr));
            Console_Println(CS, Out, "         %lu TX, %lu ACK, %lu enq, %lu lost, %lu outstanding, %lu unACKed",
                            S->Statistics.TransmittedRecords,
                            S->Statistics.AcknowledgedRecords,
                            S->Statistics.EnqueuedRecords,
                            S->Statistics.LostRecords,
                            EXPORTER_QHANDLER->OutstandingDataCount(E, S),
                            EXPORTER_QHANDLER->UnacknowledgedDataCount(E, S));
            for (j = 0; j < S->CollectorSessions.Length; j++) {
                const CollectorSession *__CS = (CollectorSession *) S->CollectorSessions.Data[j];
                if (__CS == NULL) {
                    Console_Println(CS, Out, "  ... nulled CS (index: %d)", j);
                } else {
                    TL_PABuf paBuf;
                    Console_Println(CS, Out, "  CS %.40s: %.50s, Priority: %d",
                                    __CS ->Collector->Name,
                                    TL_PrintAddressAndPort(__CS->Collector->Addr, __CS->Collector->Port, paBuf),
                                    __CS->Priority);
                    Console_Println(CS, Out, "           %.80s: %lu TX, %lu ACK, %lu enq, %lu lost",
                                    IPDR_EventMsg(&__CS->LastEvent),
                                    __CS->Statistics.TransmittedRecords,
                                    __CS->Statistics.AcknowledgedRecords,
                                    __CS->Statistics.EnqueuedRecords,
                                    __CS->Statistics.LostRecords);
                }
            }
			Console_Println(CS, Out, "");
        }
    }
    return IPDR_OK;
}

#if 1 || defined(ENABLE_SIMULATOR)


/**
 * Add a new session to the client.
 *
 * @param S console session
 * @param Out where to write any output
 * @param Argc number of arguments
 * @param Argv arguments
 * @param Arg context argument (Exporter)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static IPDR_Status
Exporter_AddSessionCommand(ConsoleSession *S,
                         FILE *Out,
                         int Argc,
                         const char * const Argv[],
                         void *Arg)
{
    Exporter *E = (Exporter *) Arg;
    IPDR_SessionHandle SH;
    IPDR_Status Status;

    if (Argc != 3) {
        Console_Println(S, Out, "add_session: Invalid number of arguments");
        return IPDR_OK;
    }

    Status = IPDR_CreateSession(E, Argv[1], (unsigned char) atoi(Argv[0]),
                                 Argv[2], & SH);
    if (FAILURE(Status))
        return Status;

    return Status;
}

#endif
#if 1 || defined(ENABLE_SIMULATOR)

static IPDR_Status
Exporter_ReconfigExporterCommand(ConsoleSession *S,
                         FILE *Out,
                         int Argc,
                         const char * const Argv[],
                         void *Arg)
{
	Exporter *E = (Exporter *) Arg;
    IPDR_Status Status;

    if (Argc != 0) {
        Console_Println(S, Out, "restart_exporter: Invalid number of arguments");
        return IPDR_OK;
    }

    Status = IPDR_ReconfigExporter( E );
    if (FAILURE(Status))
        return Status;

    return Status;
}


/**
 * Delete a session from the client.
 *
 * @param S console session
 * @param Out where to write any output
 * @param Argc number of arguments
 * @param Argv arguments
 * @param Arg context argument (Exporter)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static IPDR_Status
Exporter_DeleteSessionCommand(ConsoleSession *S,
                            FILE *Out,
                            int Argc,
                            const char * const Argv[],
                            void *Arg)
{
    IPDR_Status Status;
    Exporter *E = (Exporter *) Arg;
    IPDR_SessionHandle SH;

    if (Argc != 1) {
        Console_Println(S, Out, "delete_session: Invalid number of arguments.");
        return IPDR_OK;
    }

    Status = IPDR_GetSession(E, (unsigned char) atoi(Argv[0]), & SH, NULL, NULL);
    if (FAILURE(Status))
	{
		Console_Println(S, Out, "delete_session: Invalid session ID.");
        return Status;
	}

    Status = IPDR_DeleteSession(E, SH);
    if (FAILURE(Status))
        return Status;

    return IPDR_OK;
}

#endif

#if 1 || defined(ENABLE_SIMULATOR)


/**
 * Add a server (collector) to a session.
 *
 * @param S console session
 * @param Out where to write any output
 * @param Argc number of arguments
 * @param Argv arguments
 * @param Arg context argument (Exporter)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static IPDR_Status
Exporter_AddCollectorCommand(ConsoleSession *S,
                        FILE *Out,
                        int Argc,
                        const char * const Argv[],
                        void *Arg)
{
    Exporter *E = (Exporter *) Arg;
    IPDR_Status Status;

    if (Argc != 2 && Argc != 4) {
        Console_Println(S, Out, "add_collector: Invalid number of arguments.");
        return IPDR_OK;
    }

    Status = IPDR_CreateCollector(E, Argv[0], TL_ParseAddress(Argv[1]),
									((Argc == 2)?(0):((unsigned short) atoi(Argv[2]))),
									((Argc == 2)?(0):((unsigned short) atoi(Argv[3]))),
									NULL);
    if (FAILURE(Status))
        return Status;

    return IPDR_OK;
}

static IPDR_Status
Exporter_SetDocumentIDCommand(ConsoleSession *S,
                        FILE *Out,
                        int Argc,
                        const char * const Argv[],
                        void *Arg)
{
    Exporter *E = (Exporter *) Arg;
	IPDR_SessionHandle SH;
    IPDR_Status Status;

    if (Argc != 2) {
        Console_Println(S, Out, "set_document_id: Invalid number of arguments.");
        return IPDR_OK;
    }

	Status = IPDR_GetSession(E , atoi(Argv[0]), &SH, NULL, NULL);
    if (FAILURE(Status))
        return Status;

    Status = IPDR_SetDocumentID(SH, Argv[1]);
    if (FAILURE(Status))
        return Status;

    return IPDR_OK;
}

#endif
#if 1 || defined(ENABLE_SIMULATOR)
/**
 * Delete a collector from a session.
 *
 * @param S console session
 * @param Out where to write any output
 * @param Argc number of arguments
 * @param Argv arguments
 * @param Arg context argument (Exporter)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static IPDR_Status
Exporter_DeleteCollectorFromSessionCommand(ConsoleSession *S,
                           FILE *Out,
                           int Argc,
                           const char * const Argv[],
                           void *Arg)
{
    Exporter *E = (Exporter *) Arg;
	IPDR_SessionHandle SH;
	IPDR_CollectorHandle CH;
    IPDR_Status Status;

    if (Argc != 2) {
        Console_Println(S, Out, "delete_collector_from_session: Invalid number of arguments.");
        return IPDR_OK;
    }

	Status = IPDR_GetSession(E, atoi(Argv[1]), &SH, NULL, NULL);
	if(FAILURE(Status))
	{
        Console_Println(S, Out, "delete_collector_from_session: unable to find session.");
        return IPDR_OK;
	}

	Status = IPDR_GetCollector(E, Argv[0], &CH, NULL, NULL, NULL);
	if(FAILURE(Status))
	{
        Console_Println(S, Out, "delete_collector_from_session: unable to find collector.");
        return IPDR_OK;
	}

	Status = IPDR_RemoveCollectorFromSession(E ,SH, CH);
	if(FAILURE(Status))
	{
        Console_Println(S, Out, "delete_collector_from_session: unable to remove collector from session.");
        return IPDR_OK;
	}

	return IPDR_OK;
}

#endif
#if 1 || defined(ENABLE_SIMULATOR)
/**
 * Delete a server (collector) from a session.
 *
 * @param S console session
 * @param Out where to write any output
 * @param Argc number of arguments
 * @param Argv arguments
 * @param Arg context argument (Exporter)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static IPDR_Status
Exporter_DeleteCollectorCommand(ConsoleSession *S,
                           FILE *Out,
                           int Argc,
                           const char * const Argv[],
                           void *Arg)
{
    Exporter *E = (Exporter *) Arg;
    IPDR_CollectorHandle CH ;
    IPDR_Status Status;
    if (Argc != 1) {
        Console_Println(S, Out, "delete_collector: Invalid number of arguments.");
        return IPDR_OK;
    }
	Status = IPDR_GetCollector(E,Argv[0],&CH,NULL,NULL,NULL);
	if( FAILURE(Status) )
	{
        Console_Println(S, Out, "delete_collector: invalid collector name");
        return IPDR_OK;
	}
    Status = IPDR_RemoveCollector(E, CH);
    return Status ;
}

/**
 * Associate Collector (IP) to Session and set priority
 *
 * @param S console session
 * @param Out where to write any output
 * @param Argc number of arguments
 * @param Argv arguments
 * @param Arg context argument (Exporter)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */

static IPDR_Status Exporter_SetConnectionParamsCommand(ConsoleSession *S,
                           FILE *Out,
                           int Argc,
                           const char * const Argv[],
                           void *Arg)
{
    Exporter *E = (Exporter *) Arg;
    IPDR_Status Status;
	unsigned char SessionID ;
	const char* CollectorName ;
	unsigned char Priority ;

	IPDR_SessionHandle 		SH;
	IPDR_CollectorHandle	CH ;

    if (Argc != 3) {
        Console_Println(S, Out, "assign_collector_to_session: Invalid number of arguments.");
        return IPDR_OK;
    }

	SessionID = atoi(Argv[0]);
	CollectorName = (const char*)Argv[1] ;
	Priority = atoi(Argv[2]);

	if( SessionID <= 0 )
	{
		Console_Println(S, Out, "assign_collector_to_session: Invalid session id.");
        return IPDR_OK;
	}
	Status = IPDR_GetSession(E,SessionID,&SH,NULL,NULL);
	if( FAILURE(Status) )
	{
		Console_Println(S, Out, "assign_collector_to_session: Invalid session");
        return IPDR_OK;
	}

	Status = IPDR_GetCollector(E, CollectorName, &CH, NULL, NULL, NULL );

	if( FAILURE(Status) )
	{
		Console_Println(S, Out, "assign_collector_to_session: Invalid collector");
        return IPDR_OK;
	}

	/*

	*/
	IPDR_AssocCollectorSession(E,SH,CH,Priority);

    return IPDR_OK;
}

static IPDR_Status
Exporter_StartSessionCommand(ConsoleSession *S,
                           FILE *Out,
                           int Argc,
                           const char * const Argv[],
                           void *Arg)
{
    Exporter *E = (Exporter *) Arg;
    IPDR_Status Status;
	unsigned char SessionID ;
	IPDR_BOOL	Negotiate;

	IPDR_SessionHandle 		SH;

	if (Argc != 2) {
        Console_Println(S, Out, "start_session: Invalid number of arguments.");
        return IPDR_OK;
    }

	SessionID = atoi(Argv[0]);
	Negotiate = atoi(Argv[1]);

	if( SessionID <= 0 )
	{
		Console_Println(S, Out, "start_session: Invalid session id.");
        return IPDR_OK;
	}
	Status = IPDR_GetSession(E,SessionID,&SH,NULL,NULL);
	if( FAILURE(Status) )
	{
		Console_Println(S, Out, "start_session: Invalid session");
        return IPDR_OK;
	}

	Status = IPDR_StartSession( SH, Negotiate );
	if( FAILURE(Status) )
	{
		Console_Println(S, Out, "start_session: Session Start Failed");
        return IPDR_OK;
	}
	return IPDR_OK;
}
static IPDR_Status
Exporter_StopSessionCommand(ConsoleSession *S,
                           FILE *Out,
                           int Argc,
                           const char * const Argv[],
                           void *Arg)
{
    Exporter *E = (Exporter *) Arg;
    IPDR_Status Status;
	unsigned char SessionID ;
	IPDR_Time Timeout;
	unsigned int Reason;

	IPDR_SessionHandle 		SH;

	if (Argc != 3) {
        Console_Println(S, Out, "stop_session: Invalid number of arguments.");
        return IPDR_OK;
    }

	SessionID = atoi(Argv[0]);
	Reason = atoi(Argv[1]);
	Timeout = atol(Argv[2]);

	if( SessionID <= 0 )
	{
		Console_Println(S, Out, "stop_session: Invalid session id.");
        return IPDR_OK;
	}
	Status = IPDR_GetSession(E,SessionID,&SH,NULL,NULL);
	if( FAILURE(Status) )
	{
		Console_Println(S, Out, "stop_session: Invalid session");
        return IPDR_OK;
	}

	Status = IPDR_StopSession( 	SH, Reason, Timeout );
	if( FAILURE(Status) )
	{
		Console_Println(S, Out, "stop_session: Session Stop Failed");
        return IPDR_OK;
	}
	return IPDR_OK;
}

#endif

#if 1 || defined(ENABLE_SIMULATOR)


/**
 * Run apply templates from the console.
 *
 * @param S console session
 * @param Out where to write any output
 * @param Argc number of arguments
 * @param Argv arguments
 * @param Arg context argument (Exporter)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static IPDR_Status
Exporter_ApplyTemplatesCommand(ConsoleSession *S,
                             FILE *Out,
                             int Argc,
                             const char * const Argv[],
                             void *Arg)
{
	Exporter*			E = (Exporter *) Arg;
	IPDR_SessionHandle 	SH;
	IPDR_Status			Status ;

    if (Argc != 1) {
        Console_Println(S, Out, "apply_templates: Invalid number of arguments.");
        return IPDR_OK;
    }

    Status = IPDR_GetSession(E, (unsigned char) atoi(Argv[0]), &SH, NULL, NULL);
    if (FAILURE(Status))
        return Status;

    Status = IPDR_ApplyTemplates(SH);
    if (FAILURE(Status))
        return Status;
	return IPDR_OK;
}

#endif


/**
 * Display the templates configuration of a session in the console.
 *
 * @param CS console session
 * @param Out where to write any output
 * @param Argc number of arguments
 * @param Argv arguments
 * @param Arg context argument (Exporter)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static IPDR_Status
Exporter_GetTemplatesCommand(ConsoleSession *CS,
                           FILE *Out,
                           int Argc,
                           const char * const Argv[],
                           void *Arg)
{
    Exporter *E = (Exporter *) Arg;
    Session *S;
    TransmissionContext *TC;
    Template *Tmpl;
    Key *K;
    IPDR_Status Status;
    int i, j;
    DynamicArray *Tmpls;

    if (Argc != 1) {
        Console_Println(CS, Out, "get_templates: Invalid number of arguments.");
        return IPDR_OK;
    }

	Status = IPDR_GetSession(E, (unsigned char) atoi(Argv[0]), & S, NULL, NULL);
    if (FAILURE(Status))
		return Status;

	TC = S->ActiveContext;
	if(!TC)
	{
        Console_Println(CS, Out, "No ActiveContext in Session: %d.",
                        (unsigned char) atoi(Argv[0]));
		return IPDR_ERR;
	}

	Tmpls = & (TC->Templates);

    for (i = 0; i < Tmpls->Length; i++) {
        Tmpl = (Template *) Tmpls->Data[i];
        if (Tmpl == NULL)
            continue;

        Console_Println(CS, Out, "Template ID: %d, Attrs: %x, Description: %.80s",
                        Tmpl->TemplateID, (int)Tmpl->Enabled, Tmpl->SchemaName);

        for (j = 0; j < Tmpl->Keys.Length ; j++) {
            K = (Key *) Tmpl->Keys.Data[j];
            Console_Println(CS, Out, "  Key ID: %d, Type: %d, Name: %.60s, Label: %.60s, Help: %.80s, Attrs: %s", (int) K->KeyID, K->KeyTypeID, K->Name, K->Label,
                            K->Help, K->Enabled?"Enabled":"Disabled");
        }
    }

    return IPDR_OK;
}


/**
 * Display the client version on the console.
 *
 * @param S console session
 * @param Out where to write any output
 * @param Argc number of arguments
 * @param Argv arguments
 * @param Arg context argument (Exporter)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static IPDR_Status
Exporter_GetVersionCommand(ConsoleSession *S,
                         FILE *Out,
                         int Argc,
                         const char * const Argv[],
                         void *Arg)
{
    Console_Println(S, Out, "IPDR Exporter (Transmitter) V%.40s, Built: %.40s %.40s",
                    EXPORTER_VERSION, __DATE__, __TIME__);
    Console_Println(S, Out, "Config:"
#if 1 || defined(ENABLE_SIMULATOR)
                    " simulator"
#endif
#ifdef STD_FILE_OPERATIONS_SUPPORT
                    " std-file-ops"
#endif
#ifdef VQ_HANDLER_SUPPORT
                    " vq-handler"
#endif
#ifdef EXT_QUEUE_HANDLER_SUPPORT
                    " ext-queue-handler"
#endif
#ifdef PERSISTENT_SESSIONS
                    " persistent-sessions"
#endif
#ifdef PERSISTENT_TEMPLATES
                    " persistent-template"
#endif
#ifdef PERSISTENT_CONFIGURATION
                    " persistent-config"
#endif
        );

    return IPDR_OK;
}

/**
 * Display thetime
 *
 * @param S console session
 * @param Out where to write any output
 * @param Argc number of arguments
 * @param Argv arguments
 * @param Arg context argument (Exporter)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static IPDR_Status
Exporter_GetTimeCommand(ConsoleSession *S,
                      FILE *Out,
                      int Argc,
                      const char * const Argv[],
                      void *Arg)
{
    Exporter *E = (Exporter *) Arg;
    IPDR_Time now;
    char buf1[IPDR_TIME_FORMAT_LENGTH + 1];
    char buf2[IPDR_TIME_FORMAT_LENGTH + 1];
    IPDR_Status Status = Platform_GetTime(E->PS, &now);
    if (FAILURE(Status)) {
        char msg[IPDR_MAX_ERROR_LENGTH+1];
        Platform_GetLastErrorMessage(E->PS, msg);
        Console_Println(S, Out, "*** GetTime got result %.100s", msg);
    }
    Console_Println(S, Out, "\tExporter started at %ld (0x%08lx): %.50s local, %.50sZ",
                    E->ExporterStartTime, E->ExporterStartTime,
                    Platform_FormatTimeLocal(E->PS, E->ExporterStartTime, buf1),
                    Platform_FormatTimeGMT(E->PS, E->ExporterStartTime, buf2));
    Console_Println(S, Out, "\tExporter has been running %ld seconds and it is now %ld (0x%08lx): %.50s local, %.50sZ",
                    now - E->ExporterStartTime,
                    now, now,
                    Platform_FormatTimeLocal(E->PS, now, buf1),
                    Platform_FormatTimeGMT(E->PS, now, buf2));
    return IPDR_OK;
}


/**
 * Set the time.
 * Intended to be used this way to set the time:
 *  <code>(perl -e 'print "set_time ", time(), "\n"';
 *  echo time) | telnet xx.xx.xx.xx 40000</code>
 * @param S console session
 * @param Out where to write any output
 * @param Argc number of arguments
 * @param Argv arguments
 * @param Arg context argument (Exporter)
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static IPDR_Status
Exporter_SetTimeCommand(ConsoleSession *S,
                      FILE *Out,
                      int Argc,
                      const char * const Argv[],
                      void *Arg)
{
    Exporter *E = (Exporter *) Arg;
    IPDR_Time newTime;
    IPDR_Status Status;
    char newTimeStr[IPDR_TIME_FORMAT_LENGTH+1];
    if (Argc != 1) {
        Console_Println(S, Out, "set_time: Invalid number of arguments");
        return IPDR_OK;
    }
    newTime = atoi(Argv[0]);
    Platform_FormatTimeLocal(E->PS, newTime, newTimeStr);
    Status = Platform_SetTime(E->PS, newTime);
    if (SUCCESS(Status)) {
        Console_Println(S, Out, "Time set to %lu (%.60s)", newTime, newTimeStr);
        /* Output a warning level message because this time change
         * could cause some things to misbehave. */
        Log_OutputStr2(&E->ExporterLog, EXPORTER_MODULE_ID, IPDR_LEVEL_WARNING, "ExporterConsole",
                       "Time set to %lu (%.60s)", newTime, newTimeStr);
    } else {
        char msg[IPDR_MAX_ERROR_LENGTH+1];
        Platform_GetLastErrorMessage(E->PS, msg);
        Log_OutputStr3(&E->ExporterLog, EXPORTER_MODULE_ID, IPDR_LEVEL_ERROR, "ExporterConsole",
                       "Failed time set to %lu (%.60s): %.100s", newTime, newTimeStr, msg);
        Console_Println(S, Out, "Could not set time to %lu (%.60s): %.100s",
                        newTime, newTimeStr, msg);
    }
    return IPDR_OK;
}


/** string-ize macro. */
#define Str(x) #x

/** string-ize macro.*/
#define Xstr(x) Str(x)


/**
 * Display the parameters.
 * @param S console session
 * @param Out where to write any output
 * @param Argc number of arguments
 * @param Argv arguments
 * @param Arg context argument (Exporter)
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static IPDR_Status
Exporter_GetParametersCommand(ConsoleSession *S,
                            FILE *Out,
                            int Argc,
                            const char * const Argv[],
                            void *Arg)
{
	unsigned long Index;
	char* Name ;
	char* Value ;
	char* DefValue ;

	int padding = 0 ;
	char padName[MAX_CONFIG_STR_LEN];
	static const char* blanks = "                                                    ";

	for( Index = 0 ; (Index = ParametersHandler_GetNextParam(Index, &Name, &Value, &DefValue)) != 0xFFFFFFFF ; )
	{
		padding = 70 - strlen(Name);
		if(padding < 0)
		{
			strncpy(padName, Name, sizeof(padName));
			padName[sizeof(padName) - 1] = '\0' ;
		}
		else
		{
			sprintf(padName, "%.199s", blanks);
			padName[padding] = '\0' ;
			sprintf(padName, "%.199s%.199s", padName, Name);
		}
		Console_Println(S, Out, "%.199s = <%.199s> [default <%.199s>]",padName,Value,DefValue);
	}

    return IPDR_OK;
}


/**
 * Helper function: display a single MIB's node
 *
 * @param S console session
 * @param Out display file
 * @param E Exporter
 * @param OID object id
 * @param value gets the value
 * @param type the OID's type
 * @param persistent is it persistent?
 * @param writable is it writable
 */
 /*
static void
showMIBnode(ConsoleSession *S,
            FILE *Out,
            Exporter *E,
            const IPDR_OID *OID,
            const char* value,
            unsigned int type,
            int persistent,
            int writable)
{
    int i;
    const char* sep;
    IPDR_OIDname oidName;
    char extraBuf[IPDR_MAX_OID_SIZE * (sizeof("-2147483647") + sizeof(char))];

    IPDR_OIDtoName(E, OID, &oidName, extraBuf);
    if (persistent || writable) {
        Console_Print(S, Out, "[%s%s]",
                      (persistent ? "P" : ""),
                      (writable ? "w" : ""));
    }
    sep = " ";
    for (i = 0; i < OID->Length; i++) {
        Console_Print(S, Out, "%s%d", sep, OID->OIDArray[i]);
        sep = ".";
    }
    sep = " ";
    for (i = 0; i < oidName.Length; i++) {
        Console_Print(S, Out, "%s%s", sep, oidName.NameArray[i]);
        sep = ".";
    }
    Console_Print(S, Out, "\t");

    switch(type) {
    case (unsigned)(-1):
        if (value) {
            Console_Print(S, Out, "\?\?\?(%s)", value);
        } else {
            Console_Print(S, Out, "<node>");
        }
        break;
    case XDR_TYPE_IPV4ADDR:
        if (value) {
            Console_Print(S, Out, "IPv4(%s)", value);
        } else {
            Console_Print(S, Out, "IPv4(null)");
        }
        break;
    case XDR_TYPE_BOOLEAN:
        if (value == NULL) {
            Console_Print(S, Out, "bool(null)");
        } else if (0 == strcmp(value, "0")) {
            Console_Print(S, Out, "false");
        } else if (0 == strcmp(value, "1")) {
            Console_Print(S, Out, "true");
        } else {
            Console_Print(S, Out, "bool(%s)", value);
        }
        break;
    case XDR_TYPE_UINT:
    case XDR_TYPE_INT:
    case XDR_TYPE_ULONG:
    case XDR_TYPE_LONG:
        if (value) {
            Console_Print(S, Out, "%s", value);
        } else {
            Console_Print(S, Out, "number(null)");
        }
        break;
    case XDR_TYPE_STRING:
        if (value) {
            Console_Print(S, Out, "\"%s\"", value);
        } else {
            Console_Print(S, Out, "string(null)");
        }
        break;
    default:
        Console_Print(S, Out, "type(");
        {
            IPDR_BOOL quoted = IPDR_FALSE;
            switch (type) {
            case XDR_TYPE_BOOLEAN   : Console_Print(S, Out, "boolean"); break;
            case XDR_TYPE_UBYTE    : Console_Print(S, Out, "uint_8"); break;
            case XDR_TYPE_BYTE     : Console_Print(S, Out, "int_8"); break;
            case XDR_TYPE_USHORT   : Console_Print(S, Out, "uint_16"); break;
            case XDR_TYPE_SHORT    : Console_Print(S, Out, "int_16"); break;
            case XDR_TYPE_UINT   : Console_Print(S, Out, "uint_32"); break;
            case XDR_TYPE_INT    : Console_Print(S, Out, "int_32"); break;
            case XDR_TYPE_ULONG   : Console_Print(S, Out, "uint_64"); break;
            case XDR_TYPE_LONG    : Console_Print(S, Out, "int_64"); break;
            case XDR_TYPE_FLOAT     : Console_Print(S, Out, "float"); break;
            case XDR_TYPE_DOUBLE    : Console_Print(S, Out, "double"); break;
            case XDR_TYPE_STRING    : Console_Print(S, Out, "string"); quoted = IPDR_TRUE; break;
            case XDR_TYPE_IPV4ADDR : Console_Print(S, Out, "ipv4_addr"); quoted = IPDR_TRUE; break;
            case XDR_TYPE_IPV6ADDR : Console_Print(S, Out, "ipv6_addr"); quoted = IPDR_TRUE; break;
            case XDR_TYPE_DATETIME  : Console_Print(S, Out, "time_sec"); break;
            case XDR_TYPE_TIMEMSEC : Console_Print(S, Out, "time_msec"); break;
            case XDR_TYPE_TIMEUSEC : Console_Print(S, Out, "time_usec"); break;
#if defined(IPDR_COMPLEX_VARYING_IMPLEMENTED)
            case IPDR_TYPE_BLOB      : Console_Print(S, Out, "blob"); quoted = IPDR_TRUE; break;
#endif
            default:
                Console_Print(S, Out, "%d", type);
            }
            Console_Print(S, Out, ")");
            if (value) {
                if (quoted) {
                    Console_Print(S, Out, "(\"%s\"", value);
                } else {
                    Console_Print(S, Out, "(%s)", value);
                }
            } else {
                Console_Print(S, Out, "(null)");
            }
        }
    }
}
*/

/**
 * Display a MIB entry
 *
 * @param S console session
 * @param Out where to write any output
 * @param Argc number of arguments
 * @param Argv arguments
 * @param Arg context argument (Exporter)
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static
IPDR_Status
Exporter_GetParamCommand(ConsoleSession *S,
                     FILE *Out,
                     int Argc,
                     const char * const Argv[],
                     void *Arg)
{
	char paramValue[MAX_CONFIG_STR_LEN];
    Exporter *E = (Exporter *) Arg;
    if (Argc != 1) {
        Console_Println(S, Out, "get_param: Invalid number of arguments");
        return IPDR_OK;
    }

	IPDR_Status Status = IPDR_ParamGet( E, Argv[0], paramValue, MAX_CONFIG_STR_LEN);
    if (FAILURE(Status))
	{
		Console_Println(S, Out, "get_param: Parametr <%.98s> Not Found.", Argv[0]);
        return Status;
	}

	Console_Println(S, Out, "<%.98s> = <%.98s>", Argv[0], paramValue);

    return IPDR_OK;
}

/**
 * Set a MIB entry.
 *
 * @param S console session
 * @param Out where to write any output
 * @param Argc number of arguments
 * @param Argv arguments
 * @param Arg context argument (Exporter)
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
*/
static
IPDR_Status
Exporter_SetParamCommand(ConsoleSession *S,
                                  FILE *Out,
                                  int Argc,
                                  const char * const Argv[],
                                  void *Arg)
{
	Exporter *E = (Exporter *) Arg;
    if (Argc != 2) {
        Console_Println(S, Out, "get_param: Invalid number of arguments");
        return IPDR_OK;
    }

	IPDR_Status Status = IPDR_ParamSet( E, Argv[0], Argv[1]);
    if (FAILURE(Status))
	{
		Console_Println(S, Out, "get_param: Parametr <%.98s> Not Found.", Argv[0]);
        return Status;
	}

	Console_Println(S, Out, "<%.98s> = <%.98s>", Argv[0], Argv[1]);

    return IPDR_OK;
}


/**
 * Set the logging value (print/callback)
 * @param S console session
 * @param Out where to write any output
 * @param Argc number of arguments
 * @param Argv arguments
 * @param Arg context argument (Exporter)
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static IPDR_Status
Exporter_LogCommand(ConsoleSession *S,
                  FILE *Out,
                  int Argc,
                  const char * const Argv[],
                  void *Arg)
{
    Exporter *E = (Exporter *) Arg;

    if (Argc == 0) {
        if (E->ExporterLog.LoggerAddr && E->ExporterLog.LoggerPort) {
            TL_PABuf paBuf;
            Console_Println(S, Out, "Log messages are sent to %.50s%.50s%.50s",
                            TL_PrintAddressAndPort(E->ExporterLog.LoggerAddr, E->ExporterLog.LoggerPort, paBuf),
                            (E->ExporterLog.OutputCallback ? " (bypassing callback)" : ""),
                            (E->ExporterLog.BypassCallback ? " (ignoring bypass)" : ""));
        } else if (E->ExporterLog.BypassCallback) {
            Console_Println(S, Out, "Log messages are printed to console %.50s",
                            (E->ExporterLog.OutputCallback ? " (bypassing callback)" : ""));
        } else {
            if (E->ExporterLog.OutputCallback || E->ExporterLog.OutputCallbackDetailed) {
                Console_Println(S, Out, "Log messages are sent to callback");
            } else {
                Console_Println(S, Out, "Log messages are printed to console because of no callback");
            }
        }
        return IPDR_OK;
    }

    if (Argc != 1) {
        Console_Println(S, Out, "log: Invalid number of arguments.");
        return IPDR_OK;
    }

    if (0 == strcmp(Argv[0], "print")) {
        E->ExporterLog.BypassCallback = IPDR_TRUE;
        return Exporter_LogCommand(S, Out, 0, Argv, Arg); /* Display the new status. */
    } else if (0 == strcmp(Argv[0], "callback")) {
        E->ExporterLog.BypassCallback = IPDR_FALSE;
        return Exporter_LogCommand(S, Out, 0, Argv, Arg); /* Display the new status. */
    } else {
        Console_Println(S, Out, "Argument must be 'print' or 'callback'");
    }

    return IPDR_OK;
}
