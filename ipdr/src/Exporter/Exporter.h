/** @file Exporter/Exporter.h
 *
 * Header file containing the main client (transmitter) data structures, and for
 * functions defined in Exporter/Exporter.c.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: Exporter.h,v 1.56 2005/05/09 14:51:03 dmitryk Exp $
 *
 * The file contains definitions of the #TransmissionContext, #Session,
 * #Server, #Exporter, #QueueHandler data structures.
 */
#ifndef _IPDR_EXPORTER_H_
#define _IPDR_EXPORTER_H_

#include <IPDR/Config.h>

#define EXPORTER_H_RCSID "@(#) $Id: Exporter.h,v 1.56 2005/05/09 14:51:03 dmitryk Exp $" IPDR_COPYRIGHT_STR

#if defined(GLOBAL_RENAME)
#define Exporter_AddCollectorInternal	IPDR__Exporter_AddCollectorInternal
#define Exporter_BuildRecord             IPDR__Exporter_BuildRecord
#define Exporter_CreateSessionInternal   IPDR__Exporter_CreateSessionInternal
#define Exporter_Destructor              IPDR__Exporter_Destructor
#define Exporter_PrepareToSend           IPDR__Exporter_PrepareToSend
#define Exporter_Reconfigure             IPDR__Exporter_Reconfigure
#define Exporter_SendRecord              IPDR__Exporter_SendRecord
#define Exporter_TriggerEvent            IPDR__Exporter_TriggerEvent
#define Exporter_WindowSizeTimerCallback IPDR__Exporter_WindowSizeTimerCallback
#define Exporter_h_rcsid                 IPDR__Exporter_h_rcsid
#define Exporter_rcsid                   IPDR__Exporter_rcsid
#define EventDataInit			IPDR__EventDataInit
#define Session_Destructor		IPDR__Session_Destructor
#define Session_HandleTemplateChanges	IPDR__Session_HandleTemplateChanges
#define Session_SelectActiveServer	IPDR__Session_SelectActiveServer
#define Session_SendOutstandingData	IPDR__Session_SendOutstandingData
#define Session_Destructor		IPDR__Session_Destructor
#define Session_HandleTemplateChanges	IPDR__Session_HandleTemplateChanges
#define Session_SelectActiveServer	IPDR__Session_SelectActiveServer
#define Session_SendOutstandingData	IPDR__Session_SendOutstandingData
#define TransmissionContext_Destructor	IPDR__TransmissionContext_Destructor
#define Collector_Destructor		IPDR__Collector_Destructor
#define CollectorSession_Destructor		IPDR_CollectorSession_Destructor
#define Connection_Destructor		IPDR__Connection_Destructor

#endif

#ifdef __cplusplus
extern "C" {
#endif

#if 0
} /* put this here to un-confuse Emacs's automatic indentation */
#endif

#include <IPDR/Defs.h>
#include <IPDR/IPDR.h>
#include <Common/MemoryManagement.h>
#include <Common/Transport.h>
#include <Common/DynamicArray.h>
#include <Common/Console.h>
#include <Common/ParametersHandler.h>
#include <Common/ChunksManager.h>
#include <Common/Protocol.h>
#include <Common/ConfigurationNames.h>
#include <Exporter/Templates.h>

#define EXPORTER_PLATFORM (E->PS)
#define EXPORTER_TRANSPORT (E->TransportHandle)
#define EXPORTER_QHANDLER ((QueueHandler *) E->QHandler)
#define EXPORTER_LOG (&(E->ExporterLog))

#define SESSION_PLATFORM (S->ExporterPtr->PS)
#define SESSION_LOG (&(S->ExporterPtr->ExporterLog))

struct IPDR_tag_Template;

#define PRIORITY_MAX 2147483647

/** Connection State
*/
enum ConnectionState {
	IPDR_CONNECTION_DISCONNECTED	= 0,	/**< Initial state */
	IPDR_CONNECTION_WAITRESPONSE	= 1,	/**< Connection is waiting for connect response message */
	IPDR_CONNECTION_CONNECTED		= 2		/**< Connected */
};

enum CollectorSessionState
{
	IPDR_CS_DISCONNECTED	= 0,	/**< Initial */
	IPDR_CS_INITIATION		= 2,	/**< Got flow start, sending template data */
	IPDR_CS_READY			= 3,	/**< Not active, but ready to have session */
	IPDR_CS_ACTIVE			= 4		/**< In active session */
};

extern const char* ServerStateStr[];

/**
 * Get the string for a particular state.
 */
#define GetCSStateStr(CSState) \
(((CSState) >= IPDR_CS_DISCONNECTED && (CSState) <= IPDR_CS_ACTIVE) ? ServerStateStr[CSState] : "unknown-state")

#define GetServerStateStr(State) \
(((State) >= IPDR_SERVER_DISCONNECTED && (State) <= IPDR_SERVER_UNPAUSED) ? ServerStateStr[State] : "unknown-state")

/** The initial set of transport handlers.
*/
extern ConnectionHandlers Exporter_DefaultHandlers;

/* See comment in Exporter_PrepareToSend() on why we've removed the
 * WindowSize. */

/* #define USE_WINDOWSIZE */


/**
 * Configurable Exporter parameters.
 */
typedef struct
{
	IPDR_BOOL MultiSession;		/* Defines if multisession capability is supported */
	IPDR_BOOL TemplateNegotiation;	/* Defines if the Exporter supports template negotiation */
	IPDR_BOOL StructureSupport;	/* Defines if the Exporter supports structure fields */
}
ExporterCapabilities;

#define CollectorCapabilities ExporterCapabilities
/**
 * structure holding configurable exporter parameters
*/
typedef struct
{
#if defined(USE_WINDOWSIZE)
    long WindowSize;             /**< Data layer window size */
#endif
    int WindowTimeout;           /**< Timeout before changing server
                                  * (collector) when no ACK is
                                  * received (this used to also use
                                  * the window size, but that code was
                                  * removed as being a wrong
                                  * design). */
    IPDR_IPAddress ProtocolLevelAddr; /**< IP Address will be send to collector in connect message, if NULL wil be sent real socket address */
    int StatisticsInterval;      /**< Interval between stats report */
    char *VendorString;          /**< Vendor specific string attached
                                  * to the client (transmitter) (copied from
                                  * IPDR_InitExporter(), using
                                  * Util_StrDup() as needed).  */
	int	KeepAliveInterval ;		 /**<  Period in seconds for sending "Keep Alive" message */
	int AutoconnectInterval;	 /**< Period in seconds for automatically connecting
								   to active Collectors*/
	int TemplateAckTimeout;		 /**<  Period in seconds for receiving "Final Template Acknowledge"
									by collector when switching to the new one. If the selected
									Collector does not send the acknowledge withing this interval,
									the connection is closed and exporter switches to another Collector*/
	int ConnectResponseTimeout ; /**<  Period in seconds for receiving "Connect Response" message after
									connect message is sent */
	int	WaitBestCollectorTimeout ;/**<  Period in seconds, the Exported supposed to wait Collectors to be connected,
									before choosing Collector for Active Session. This period starts counting
									after first "Connect" or "Connect Response" Message for specific Session
									in a case that all the Collectors are disconnected (including Startup) */
	int DataAckTimeInterval;		/**< Period in seconds when the acknowledge must be sent by collector*/
	int DataAckSequenceInterval;	/**< Number of messages when the acknowledge must be sent by collector*/

	ExporterCapabilities	caps ; /* Capabilities structure */
}
ExporterParameters;


/**
 * Contains a specific templates configuration plus data attached to
 * the context by the queue handler (for example a volatile queue).
 */
typedef struct IPDR_tag_TransmissionContext
{
    char eyecatcher[8];		/**< Gets "XMTCTX" */
    void *QueueHandlerData; /**< Data attached to the context by the
                             * queue handler (for example the queue
                             * itself) */

    DynamicArray Templates;			/*Templates in this context*/
    unsigned short ConfigurationID;	/*Configuration ID (incremented on each change)*/
    IPDR_BOOL Negotiable;			/*This context may be negotiable or not.*/

    struct IPDR_tag_Session *SessionPtr;	/*Back pointer to session object containing this context */
    struct IPDR_tag_TransmissionContext *NextContext; /*Next context to be set*/
}
TransmissionContext;


/**
 * Structure containing information about a collector.
 */
typedef struct IPDR_tag_Collector
{
	char eyecatcher[11]; /**< Gets "COLLECTOR" */

	DynamicArray 	CollectorSessions;
	DynamicArray 	IPDRConnections;

	char*			Name;     /**< The collector's (server's) name. */
	IPDR_IPAddress	Addr;     /**< The collector's (server's) IP address. */
	unsigned short	Port;     /**< The collector's (server's) "port". */
	unsigned short	NumOfConnections; /**< Number of connections to initiate to this collector */
	CollectorCapabilities	Capabilities;     /**< The individual collector's (server's) "capabilities". */
	struct IPDR_tag_Exporter *ExporterPtr; /**< Back pointer to the exporter structure */

	IPDR_BOOL		isDummy ;
}
Collector;


/**
 * Structure containing information about a ipdr connection.
 */
typedef struct IPDR_tag_Connection
{
	char 					eyecatcher[10]; /**< Gets "CONNECT" */
	enum ConnectionState	State;			/**< State of this connection */
	IPDR_BOOL				Autoconnect;	/**< This connection should be initiated by Exporter */
	Connection				TCPConnection;	/**< TCP Specific Connection information */
	Collector				*Owner;			/**< Collector, owning this connection */

	IPDR_TimerID 			ConnectResponseTimerID;		/**< Timer for waiting CONNECT RESPONSE */
	IPDR_TimerID 			KeepAliveINTimerID;			/**< Timer for incoming "keep alive" messages */
	IPDR_TimerID 			KeepAliveOUTTimerID;		/**< Timer for outgoing "keep alive" messages */
	int						RemoteKeepAliveInterval;	/**< Interval for outgoing "keep alive" messages */
	IPDR_BOOL				isDummy;

	struct IPDR_tag_Exporter *ExporterPtr;
}
IPDRConnection;

/**
 * Structure containing information about a server (collector).
 */
typedef struct IPDR_tag_CollectorSession
{
    char eyecatcher[9]; /**< Gets "COLLSESS" */

	enum CollectorSessionState CSState;			/**< Collector session state */

	struct IPDR_tag_Exporter *ExporterPtr;		/**< Back pointer to Exporter object */
    struct IPDR_tag_Session *SessionPtr;		/**< Pointer to session object */
	struct IPDR_tag_Collector* Collector;		/**< Pointer to Collector object */
	struct IPDR_tag_Connection *SessionConnection;	/**< Transport level connection */

	int		Priority; /**< The collector's (server's) priority (lower number is higher priority). */
	IPDR_BOOL 	 Negotiable;
    IPDR_TimerID TmplDataTimer;		/**< used for data */
    IPDR_TimerID WindowSizeTimer;	/**< */

    IPDR_Statistics Statistics;     /**< The current statistics */
    IPDR_Time PrevStatisticsTime;   /**< When the \a PrevStatistics were taken */
    IPDR_Statistics PrevStatistics; /**< the \a CurrStatistics at time \a PrevStatistics */

    IPDR_EventData LastEvent;
}
CollectorSession;

/**
 * Structure containing a set of redundent servers (collectors) plus a list of
 * transmission contexts used to queue records.
 * It represents Session entity as specified by IPDR-SP protocol
 *
 */
typedef struct IPDR_tag_Session
{
    char eyecatcher[8]; 				/**< Gets "SESSION" */
    IPDR_Collector *CCollectors;				/**< Used by IPDR_GetCollectorsList() */
	int CCollectorsLength;

    TransmissionContext *ActiveContext; /**< head of the contexts list */
    TransmissionContext *WorkContext;   /**< tail of the contexts list */

	char	DocumentID[16];				/**< Current DocumentID */
    IPDR_DSN_TYPE DSN;                 	/**< current sequence number */
    IPDR_BOOL SyncNeeded;              	/**< flag indicating whether DSN
                                         * sync is needed for the next
                                         * record */
    IPDR_BOOL SendingData;             	/**< true if we're currently
                                         * sending data within the
                                         * session */
	IPDR_BOOL Retransmite;
	IPDR_BOOL Running; 					/**< Flag defining if the Session may be running or it has been
										  *  stopped using IPDR_SessionStop */
	IPDR_BOOL Negotiable;				/**< true if the session will negotiate the
										 * templates at first opportunity */
	IPDR_BOOL TemplatesChnged;			/**< none active collectors templates syncronization flag */

    CollectorSession *ActiveCS;			/**< currently active server (collector) */
    /*DynamicArray Collectors;*/		/**< Collectors connected to this session */
    DynamicArray CollectorSessions;		/**< CollectorSession objects */
    DynamicArray Templates;				/**< Templates existing in this session*/

    unsigned char ID;
    char *Name;
    char *Description;

    struct IPDR_tag_Exporter *ExporterPtr;

    IPDR_TimerID waitBestCollectorTimerID; /**< waitBestCollector timeout */

    IPDR_Statistics Statistics;     	/**< The current statistics */
    IPDR_Time PrevStatisticsTime;   	/**< When the \a PrevStatistics were taken */
    IPDR_Statistics PrevStatistics; 	/**< the \a CurrStatistics at time \a PrevStatistics */
}
Session;


#define MAX_EVENT_MSG_LEN 512


/**
 * Structure containing information about a IPDR exporter.
 * Its purpose is to contain the fields specified above for use of
 *
 *
 */
typedef struct IPDR_tag_Exporter
{
    char eyecatcher[8];								/**< Gets "EXPORTER " */
    IPDR_BOOL Started;								/**< Boolean field, true if the exporter was started, false otherwise */
    IPDR_Time ExporterStartTime;					/**< Exporter start time */
    IPDR_PlatformServices *PS;						/**< Pointer to platform services, used by this exporter */

    ExporterParameters Parameters;					/**< Exporter parameters structure */

    IPDR_IPAddress LocalAddr;						/**< Local address this exporter listening on*/
    unsigned short LocalPort;						/**< Local port this exporter listening on*/

    IPDR_IPAddress ProtocolInfoAddr;				/**< Local address this exporter listening on for version discovery*/
    unsigned short ProtocolInfoPort;				/**< Local port this exporter listening on for version discovery*/
    IPDR_SocketHandle ProtocolInfoSocket;			/**< Socket used for version discovery*/
	IPDR_SocketEventID ProtocolInfoEvent;			/**< Event which fires than version discovery*/

    Transport TransportHandle;						/**< Transport object */
    Console ConsolePtr;								/**< Console object */

    DynamicArray Sessions;							/**< List of the session this exporter runs */
	DynamicArray Collectors;						/**< List of collectors allowed to be connected with this exporter */
	DynamicArray DummyCollectors;						/**< List of collectors allowed to be connected with this exporter */
	DynamicArray CollectorSessions;					/**< List of the connections of collectors with sessions */

	IPDR_TimerID StatisticsTimerID;					/**< Timer used for printing the statistics */
	IPDR_TimerID AutoconnectTimerID;				/**< Timer used for autoconnection */

    ParametersHandler ParamsHandler;				/**< TBD */
    /*ParametersHandler ParamsHandler;*/				/**< TBD */

	IPDR_BOOL IsInsideCallback;

    IPDR_TemplateChangedCallback TemplateChangedCallback; /**< Pointer to function specified by host application */
    void *TemplateChangedArg;						/**< Additional data transferred to the callback */

    IPDR_EventCallbackDetailed EventCallbackDetailed;/**< Pointer to function specified by host application */
    void *EventCallbackDetailedArg;					/**< Additional data transferred to the callback */

	IPDR_FlowStartCallback FlowStartCallback;		/**< Pointer to function specified by host application */
	void *FlowStartCallbackArg;						/**< Additional data transferred to the callback */

	IPDR_StartNegotiationCallback StartNegotiationCallback; /**< Pointer to function specified by host application */
	void *StartNegotiationCallbackArg;				/**< Additional data transferred to the callback */

	IPDR_NewConfigurationIDCallback NewConfigurationIDCallback; /**< Pointer to function specified by host application */
	void *NewConfigurationIDCallbackArg;				/**< Additional data transferred to the callback */

	IPDR_ConfigurationIDChangedCallback ConfigurationIDChangedCallback;
	void *ConfigurationIDChangedCallbackArg;

    Log ExporterLog;								/**< Log object */

    void *SaveMIBArg;

    void *QHandlerData; /**< optional data attached by the queue handler */
    void *QHandlerInitArg; /**< argument passed to the queue handler
                            * initialization function */
    struct IPDR_tag_QueueHandler *QHandler;			/**< Queue service object */

    unsigned char *CSessions;				/** Used by IPDR_GetSessionsList() */
	IPDR_Collector *CCollectors;					/**< Used by IPDR_GetCollectorsList() */

    char EventLine[MAX_EVENT_MSG_LEN+1]; /**< used to output Event
                                          * messages and avoid stack
                                          * space */
}
Exporter;


/**
 * Structure containing callbacks for handling data queueing (actual
 * implemtation may vary depending on the queuing model).
 */
typedef struct IPDR_tag_QueueHandler
{
    IPDR_Status (*Initialize)(Exporter *E, void *Arg);
    IPDR_Status (*Destroy)(Exporter *E);

    IPDR_Status (*CreateSession)(Exporter *E, Session *S);
    IPDR_Status (*DeleteSession)(Exporter *E, Session *S);
    IPDR_Status (*CreateTransmissionContext)(Exporter *E, Session *S,
                                              TransmissionContext *TC);
    IPDR_Status (*DeleteTransmissionContext)(Exporter *E, Session *S,
                                              TransmissionContext *TC);
    IPDR_Status (*SendData)(Exporter *E, Session *S,
                             struct IPDR_tag_Template *Tmpl, const void *Data);
    IPDR_Status (*SendOutstandingData)(Exporter *E, Session *S);
    IPDR_Status (*ACK)(Exporter *E, Session *S, CollectorSession *CS,
                        IPDR_DSN_TYPE DSN,
                        unsigned int *RemovedRecords);
    IPDR_Status (*NACK)(Exporter *E, Session *S, CollectorSession *CS,
                         IPDR_DSN_TYPE DSN);
    IPDR_Status (*Rollback)(Exporter *E, Session *S);
    unsigned long (*OutstandingDataCount)(Exporter *E, Session *S);
    unsigned long (*UnacknowledgedDataCount)(Exporter *E, Session *S);
    unsigned long (*UsedMemory)(Exporter *E, Session *S);
    IPDR_Status (*Reconfigure)(Exporter *E, Session *S);
	IPDR_Status (*CleanUP)(Exporter *E, Session *S);
}
QueueHandler;


IPDR_Status Session_SelectActiveCollector(Exporter *E, Session *S);

IPDR_Status Session_SendOutstandingData(Exporter *E, Session *S);

IPDR_Status Session_HandleTemplateChanges(Exporter *E, Session *S,
                                           int NumChanges,
                                           unsigned char *TCBs,
                                           IPDR_BOOL Force,
                                           IPDR_BOOL CopyFromExporter,
                                           IPDR_BOOL Negotiable);

void EventDataInit(IPDR_EventData *,
                   enum IPDR_Event_t,
                   Exporter *E,
                   CollectorSession *CS,
                   Session *S);


/*PRINTFLIKE5*/
void Exporter_TriggerEvent(Exporter *E, IPDR_DebugLevel LoggingLevel,
                         const IPDR_EventData* EventData, CollectorSession *CS,
                         const char *Msg, ...);

IPDR_Status Exporter_Reconfigure(Exporter *E);

IPDR_Status Exporter_BuildRecord(ChunksManager *CMgr, Session *S,
                                struct IPDR_tag_Template *Tmpl,
                                const void *Data,
                                BufferHandle *BH, unsigned long *RecordSize);

IPDR_Status Exporter_SendRecord(Exporter *E, Session *S, BufferHandle *BH, IPDR_BOOL Immediate, IPDR_BOOL duplicate);

IPDR_Status Exporter_PrepareToSend(Exporter *E, Session *S);

IPDR_Status Exporter_CreateSessionInternal(Exporter *E,
                                          const char *Name, unsigned char ID,
                                          const char *Description,
                                          IPDR_SessionHandle *SH);

IPDR_Status IPDR_GetTemplateInternal(IPDR_SessionHandle SessionHandle,
										int TemplateID,
										char **SchemaName,
										char **TypeName,
										int *NumKeys,
										IPDR_TemplateHandle *TH);

IPDR_Status Exporter_AddCollectorInternal(Exporter *E,
                                      const char *Name,
                                      IPDR_IPAddress Addr,
                                      unsigned short Port,
                                      unsigned short NumberOfActive,
									  IPDR_BOOL isDummy,
									  IPDR_CollectorHandle* CH);

IPDR_Status Exporter_AssocCollectorSession_Internal(Exporter* E,
											Session* S,
											Collector* C,
											unsigned short Priority);

IPDR_Status Exporter_TriggeTemplateChangedCallback(Session *S);

IPDR_Status Exporter_AddIPDRConnectionToCollector(	Exporter *E,
													Collector *Coll,
													Connection C );

void TransmissionContext_Destructor(IPDR_PlatformServices *PS, void *Obj);

void Exporter_Destructor(IPDR_PlatformServices *PS, void *Obj);

void Session_Destructor(IPDR_PlatformServices *PS, void *Obj);

void Collector_Destructor(IPDR_PlatformServices *PS, void *Obj);

void CollectorSession_Destructor(IPDR_PlatformServices *PS, void *Obj);

void Connection_Destructor(IPDR_PlatformServices *PS, void *Obj);

void Exporter_WindowSizeTimerCallback(void *Arg);

int Exporter_GetCapabilities(Exporter *E);


#ifdef __cplusplus
}
#endif

#endif
