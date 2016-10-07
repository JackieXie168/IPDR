/** @file Exporter/ExporterMsgs.h
 *
 * Header file for messages used in "Exporter" (transmitter), "Queue",
 * and "Protocol" modules.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: ExporterMsgs.h,v 1.28 2005/04/21 11:30:09 dmitryk Exp $
 *
 */
#ifndef _EXPORTER_MSGS_H_
#define _EXPORTER_MSGS_H_
#include <IPDR/Config.h>
#define EXPORTER_MSGS_H_RCSID "@(#) $Id: ExporterMsgs.h,v 1.28 2005/04/21 11:30:09 dmitryk Exp $" IPDR_COPYRIGHT_STR

#if defined(GLOBAL_RENAME)
#define ExporterMsgs_Initialize IPDR__ExporterMsgs_Initialize
#define ExporterMsgs_h_rcsid    IPDR__ExporterMsgs_h_rcsid
#define ExporterMsgs_rcsid      IPDR__ExporterMsgs_rcsid
#endif

#include <Common/Log.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EXPORTER_MODULE_ID                10
#define QUEUE_MODULE_ID                   11
#define PROTOCOL_MODULE_ID                12

#define MSG_QUEUE_PUSH_RECORD                /*16*/  "Pushed record - DSN=%llu  Size=%d"
#define MSG_QUEUE_PEEK_RECORD                /*17*/  "Peeked record - DSN=%llu"
#define MSG_QUEUE_ROLLBACK_QUEUE             /*18*/  "Rolled back queue"
#define MSG_QUEUE_REMOVE_RECORDS             /*19*/  "Removed records - DSN=%llu, Size=%lu, Outstanding=%lu, Removed=%u"
#define MSG_QUEUE_WILD_REMOVE_RECORDS        /*20*/  "Removed records with wild DSN=%llu, queuedDSN=%llu, Size=%lu"
#define MSG_EXPORTER_ADD_SESSION               /*32*/  "Add session - ID=%d, Name=%.40s, Description=%.80s"
#define MSG_EXPORTER_REMOVE_SESSION            /*33*/  "Remove session - ID=%d"
#define MSG_EXPORTER_ADD_SERVER                /*34*/  "Add Collector - Name=%.40s, Addr=%.98s, Num Of Connections=%d (%.98s)"
#define MSG_EXPORTER_REMOVE_SERVER             /*35*/  "Remove Collector - Addr=%.98s"
#define MSG_EXPORTER_CREATE_TEMPLATE           /*36*/  "Create template - ID=%d, Description=%.80s"
#define MSG_EXPORTER_CREATE_KEY                /*37*/  "Create Key - Template=%d (%.99s), ID=%d, Type=%x, Name=%.40s, Label=%.40s, Help=%.80s, Attrs=%x"
#define MSG_EXPORTER_WRONG_CONFIGURATION_ID    /*38*/  "Configuration ID mismatch - Expected=%d, Received=%d"
#define MSG_EXPORTER_SESSION_TEMPLATES_CHANGED /*39*/  "Session templates reconfigured - Session=%.40s"
#define MSG_EXPORTER_TEMPLATE_INFO             /*40*/  "  Template ID=%d, Description(%.80s)"
#define MSG_EXPORTER_KEY_INFO                  /*41*/  "    Key ID=%d, Name(%.80s), Attrs=%x"
#define MSG_EXPORTER_UNKNOWN_SERVER            /*42*/  "Unknown server (collector): %.98s"
#define MSG_EXPORTER_UNKNOWN_SESSION           /*43*/  "Unknown session request: %.98s, Session=%d, Message=%d"
#define MSG_EXPORTER_SESSION_STATISTICS1       /*44*/  "-) Session Statistics: ID=%d at %.40s"
#define MSG_EXPORTER_SESSION_STATISTICS2       /*45*/  "   %lu outstanding, %lu unACKed (%u allocs, %u frees, %lu bytesAlloc, %lu qBytes)"
#define MSG_EXPORTER_SESSION_STATISTICS3       /*46*/  "   %-5s%11lu tot%7lu intvl%6lu.%02lu/sec"
#define MSG_EXPORTER_SERVER_STATISTICS1        /*49*/   "   %.98s (%.98s)"
#define MSG_EXPORTER_SERVER_STATISTICS2        /*50*/  "   %-5s%11lu tot%7lu intvl%6lu.%02lu/sec"
#define MSG_EXPORTER_PROTOCOL_INFO             /*52*/  "Received protocol info request from %.98s (%.98s boot time: %.49sZ), response port=%u"
#define MSG_EXPORTER_BUILT_INFO                /*53*/  "%.40s Build info: %.80s"
#define MSG_EXPORTER_RECEIVED_START            /*54*/  "Received start from %.98s"
#define MSG_EXPORTER_TRIGGER_EVENT             /*55*/  "Event[%.99s]: %.255s"
#define MSG_EXPORTER_FLUSH_RECORDS_QUEUE       /*56*/  "Flushing records queue (%.50s) for %.98s, State=%.20s, unACK=%lu"
#define MSG_EXPORTER_ASSOC_COLL_SESSION			/*57*/ "Set Collector %.40s, Addr=%.98s at Session ID=%d, Priority %d"
#define MSG_EXPORTER_ASSOC_COLL_SESSION_U		/*58*/ "Association already exists, updating. Collector %.40s, Addr=%.98s at Session ID=%d, Priority %d"


#define MSG_EXPORTER_UNEXPECTED_MSG            /*60*/  "Unexpected message from %.98s, MessageID=%02x, State=%.20s"
#define MSG_EXPORTER_UNEXPECTED_START          /*61*/  "Unexpected start from Addr=%.30s, Port=%u State=%.20s, for %.98s"
#define MSG_EXPORTER_UNEXPECTED_CONNECT        /*62*/  "Unexpected connect from %.98s State=%.20s, for %.98s"
#define MSG_EXPORTER_UNEXPECTED_DATA_ACK       /*63*/  "Unexpected DATA ACK for non-paused server (collector) %.98s, State=%.20s"
#define MSG_EXPORTER_NOT_READY_SEND            /*64*/  "Cannot send on disconnected Collector"
#define MSG_EXPORTER_NO_CONN                   /*65*/  "No connection"
#define MSG_EXPORTER_MIB_CMD_ADD_SERVER        /*66*/  "MIB command addServer (transmitter) result: %.98s sessionID=%d (%.98s: %.98s) priority=%d addr=%.50s name=%.98s"
#define MSG_EXPORTER_MIB_CMD_REMOVE_SERVER     /*67*/  "MIB command removeServer (transmitter) result: %.98s sessionID=%d (%.98s: %98s) addr=%.50s"
#define MSG_EXPORTER_MIB_CMD_ADD_SESSION       /*68*/  "MIB command addSession result: %.98s sessionID=%d (%.98s: %98s)"
#define MSG_EXPORTER_MIB_CMD_REMOVE_SESSION    /*68*/  "MIB command removeSession result: %.98s sessionID=%d (%.98s: %98s)"
#define MSG_EXPORTER_MIB_CMD_COMMAND           /*69*/  "MIB command result: %.98s for command: %.199s"
#define MSG_EXPORTER_DUPLICATE_KEY             /*70*/  "Duplicate Key - Template=%d (%.99s), new(id=%d,type=%x,name=%.40s: %.98s) prev(id=%d,type=%x,name=%.40s: %.98s)"
#define MSG_EXPORTER_NO_SESSIONS               /*71*/  "No sessions (possibly not authorized) for server (collector) %.98s (%.98s)"
#define MSG_EXPORTER_BAD_PROTOCOL_REQUEST_LEN  /*72*/  "Protocol info request from %.98s: invalid length=%u"
#define MSG_EXPORTER_BAD_PROTOCOL_REQUEST	   /*73*/  "Protocol info request from %.98s: invalid protocol identifier: '%c%c%c%c'"
#define MSG_EXPORTER_UNKNOWN_MSG               /*74*/  "Received unknown message from collector: %u (0x%xu)"
#define MSG_EXPORTER_STATE_TRANSITION          /*90*/  "State transition on %.98s: %.20s => %.20s (%.40s)"
#define MSG_EXPORTER_DATA_ACK                  /*91*/  "DATA ACK DSN=%llu, Outstanding=%lu, UnACKEDed=%lu on %.98s"
#define MSG_EXPORTER_DATA_NACK                 /*92*/  "DATA NACK DSN=%llu, Outstanding=%lu, UnACKEDed=%lu on %.98s"
#define MSG_EXPORTER_STARTED                   /*93*/  "Exporter (transmitter) started at %ld (0x%08lx): %.50s local, %.50sZ"
#define MSG_EXPORTER_MIB_LOAD_FAILED           /*94*/  "Load of MIB version %d from '%.149s' failed: %.50s"
#define MSG_EXPORTER_MIB_LOAD_CALLBACK_FAILED  /*95*/  "Load of MIB failed: %.50s"
#define MSG_EXPORTER_MIB_NO_PERSISTENT         /*95*/  "Load of MIB version %d from '%.149s' but feature not enabled"

#define MSG_EXPORTER_LOG_CONNECT_ERROR		   /*100*/ "%s return %.20s, Connection to  %.98s"
#define MSG_EXPORTER_NOT_READY_SEND2			/*101*/"Cannot send on disconnected Collector <%.98s>"

#define MSG_EXPORTER_DISABLED_TEMPLATE	       /*102*/ "Disabled template-ID: %d failed function"

#define MSG_EXPORTER_QUEUE_EMPTIED	       /*103*/ "Emptied queue: %.98s, outstanding=%lu, unACKed=%lu, Session=%d [%.40s] (%.80s)"
#define MSG_EXPORTER_ACTIVE_COLLECTOR_CHANGED     /*104*/ "Active collector changed (from %.98s) %.98s, outstanding=%lu, unACKed=%lu, Session=%u [%.40s] (%.80s)"

#define MSG_EXPORTER_DISCONNECT_TRANSPORT1    /*105*/ "Error - <TL_ResetConnection> Disconnect Transport Level."
#define MSG_EXPORTER_DISCONNECT_TRANSPORT2    /*106*/ "Error - <SendSessionStop> Disconnect Transport Level."
#define MSG_EXPORTER_DISCONNECT_TRANSPORT3    /*107*/ "Error - <SendSessionStart> Disconnect Transport Level."

#define MSG_EXPORTER_UNEXPECTED_STATE               /*108*/ "Error - Unexpected state of selected CollectorSession."
#define MSG_EXPORTER_NEW_ACTIVE_COLLECTOR           /*109*/ "New active collector - Active collector has changed."
#define MSG_EXPORTER_NEW_ACTIVE_COLLECTOR_ROLLBACK  /*110*/ "New active collector - Rollback."
#define MSG_EXPORTER_NEW_ACTIVE_COLLECTOR_SYNC      /*111*/ "New active collector - Sync."

#define MSG_EXPORTER_TEMPLATE_RESPONSE_INVALID_VALUE /*112*/ "Error - Template response is of size:%d, while the size of \"ModifyTemplateResponseMessage\" is:%d."
#define MSG_EXPORTER_COLLECTOR_SESSION_FAILED     /*113*/ "Collector is not configured for this session"
#define MSG_EXPORTER_SEND_TEMPLATE_DATA_FAILED    /*114*/ "Error - Send TEMPLATE DATE failed"

#define MSG_EXPORTER_READ_TEMPLATE_INVALID_VALUE  /*115*/ "Error - Reading data type: %s Of size:%d while size of data left is:%d."
#define MSG_EXPORTER_CHUNKS_MANAGER_FAILED	  /*116*/ "Error - AllocateBuffer failed"
#define MSG_EXPORTER_ACTIVE_COLLECTOR_EXISTS      /*117*/ "Error - An active collector exists in Session=%d [%.40s]"
#define MSG_EXPORTER_NO_ACTIVE_COLLECTOR          /*118*/ "Error - Tried to stop Session=%d [%.40s], while an active collector is disconnected or does not exist"
#define MSG_EXPORTER_QUEUE_NOT_DRAINED		  /*119*/ "Queue not drained with either invalid values, exporter not running or timeout to short"
#define MSG_EXPORTER_MESSAGE_TOO_LONG		 /*120*/ "Error - Message too long (%.98s)"
#define MSG_EXPORTER_COLLECTOR_NOT_CONNECTED     /*121*/ "Collector is not in CONNECTED state"
#define MSG_EXPORTER_WRONG_COLLECTOR_STATE       /*122*/ "Collector is not in needed state"
#define MSG_EXPORTER_SEND_SESSION_START_FAILED   /*123*/ "SessionStart send failed to '%.98s'"
#define MSG_EXPORTER_SEND_SESSION_START_TARGET   /*124*/ "SessionStart was sent to '%.98s'"
#define MSG_EXPORTER_ERROR_SEND_FAILED           /*125*/ "Error send failed to '%.98s'"
#define MSG_EXPORTER_ERROR_SEND_TARGET           /*126*/ "Error was sent to '%.98s'"
#define MSG_EXPORTER_BROKEN_TEMPLATE		 /*127*/ "BrokenTemplate"
#define MSG_EXPORTER_CONNECTION_MUST_BE_SET	 /*128*/ "Must be CONNECTED state"
#define MSG_EXPORTER_READ_FIELD_DESCRIPTOR_FAILED /*129*/ "Read Field Descriptor Block from a Template Block failed"
#define MSG_EXPORTER_READ_TEMPLATE_BLOCK_FAILED   /*130*/ "Read full Template Block failed"
#define MSG_EXPORTER_DATA_ACK1			 /*131*/ "Got data ack from a collector with an higher priority : reselect active collector"
#define MSG_EXPORTER_DATA_ACK2			 /*132*/ "Got data ack from a new collector"
#define MSG_EXPORTER_DATA_ACK3			 /*133*/ "Got data ack for low-priority or non-ready collector"
#define MSG_EXPORTER_SEND_CONNECT_RESPONSE_FAILED   /*134*/ "ConnectResponse send failed to '%.98s'"
#define MSG_EXPORTER_SEND_CONNECT_RESPONSE_TARGET   /*135*/ "ConnectResponse was sent to '%.98s'"
#define MSG_EXPORTER_SEND_DISCONNECT_FAILED   	 /*136*/ "Disconnect send failed to '%.98s'"
#define MSG_EXPORTER_SEND_DISCONNECT_TARGET   	 /*137*/ "Disconnect was sent to '%.98s'"
#define MSG_EXPORTER_SEND_KEEPALIVE_FAILED   	 /*138*/ "KeepAlive send failed to '%.98s'"
#define MSG_EXPORTER_SEND_KEEPALIVE_TARGET   	 /*139*/ "KeepAlive to '%.98s'"
#define MSG_EXPORTER_SEND_KEEPALIVE_RECEIVED   	 /*140*/ "KeepAlive from: %.98s"
#define MSG_EXPORTER_SEND_CONNECT_RESPONSE_ERROR /*141*/ "ExporterProtocol_SendConnectResponse Error"
#define MSG_EXPORTER_SEND_SESSION_STOP_FAILED    /*142*/ "Error - ExporterProtocol_SendSessionStop with reason: %s failed"
#define MSG_EXPORTER_NO_KEEPALIVE_RECEIVED	/*143*/ "KeepAlive was NOT received before timer elapsed, Disconnectiong Client..."
#define MSG_EXPORTER_RECORD_LOST		/*144*/ "Record lost (send): %.98s, DSN=%llu, outstanding=%lu, unACKed=%lu, Session=%u [%.40s] (%.80s)"

#define MSG_EXPORTER_ERROR_RECEIVED		/*145*/ "Error received from: %.98s, Error Code %d, Error Desc: %s"
#define MSG_EXPORTER_SEND_CONNECT_FAILED	/*146*/ "Connect send failed to '%.98s'"
#define MSG_EXPORTER_SEND_CONNECT_TARGET	/*147*/ "Connect send to '%.98s', waiting for %d secs"
#define MSG_EXPORTER_CONNECT_RESPONSE_RECEIVED  /*148*/ "Connect Response received"
#define MSG_EXPORTER_STATE_NOT_WAITRESPONSE     /*149*/ "IC->State != IPDR_CONNECTION_WAITRESPONSE"
#define MSG_EXPORTER_STATE_NOT_DISCONNECTED     /*150*/ "IC->State != IPDR_CONNECTION_DISCONNECTED"
#define MSG_EXPORTER_COLLECTOR_NOT_INITIATION   /*151*/ "Collector is not in INITIATION state"
#define MSG_EXPORTER_SEND_NEGOTIATION_REJECT_FAILED	/*152*/ "Error - ExporterProtocol_SendStartNegotiationReject failed"
#define MSG_NO_CONNECTION_OWNER			/*153*/ "Connection owner is NULL"
#define MSG_EXPORTER_CREATE_DYNAMIC_ARRAY_FAILED /*154*/ "Error - DynamicArray_Create with capacity failed"
#define MSG_EXPORTER_NO_CONNECT_RESPONSE	/*155*/ "Connect response did not come for collector '%s'"
#define MSG_EXPORTER__RESET_TIMER_NEEDED	/*156*/ "Somebody else already recognized, that the connection is dead, but did not reset a timer '%s'"
#define MSG_EXPORTER_DIFFERENT_ACTIVE_COLLECTOR	/*157*/ "Error - Active collector is not the same as the not responding collector"

#define MSG_EXPORTER_INVALID_CAPABILITIES	/*158*/ "The template is not negotiable, Modify Template is not allowed"
#define MSG_EXPORTER_TEMPLATE_CHANGE_RECIVED	/*159*/ "Modification received from collector in Template ID=%d: Key ID:%d,Name:%s Access:%s"
#define MSG_EXPORTER_TEMPLATE_CHANGE_SENT	/*160*/ "Modification response sent to collector in Template ID=%d: Key ID:%d,Name:%s Access:%s"
#define MSG_EXPORTER_UNKNOWN_SESSION2           /*161*/  "Unknown session request"
#define MSG_EXPORTER_NO_TEMPLATEDATA_ACK	/*162*/ "Template Data Ack NOT Received, SessionID: %d, <'%.98s'>"
#define MSG_EXPORTER_UNKNOWN_COLLECTOR            /*163*/  "Unknown Collector"
#define MSG_EXPORTER_SESSION_MUST_STOP            /*164*/  "Session ID=%d on Collector '%.98s'<'%.98s'> Must Stop First"
#define MSG_EXPORTER_SESSION_MUST_STOP2            /*165*/  "Session ID=%d on Collector '%.98s' Must Stop First"
#define MSG_EXPORTER_SESSION_MUST_STOP3            /*165*/  "Session ID=%d Must Stop First"
#define MSG_EXPORTER_COLLECTOR_NOT_IN_SESSION            /*166*/  "Collector not in session"
#define MSG_EXPORTER_NO_CONNECT_MESSAGE	/*167*/ "Connect message did not come for collector '%s'"
#define MSG_EXPORTER_UNKNOWN_SERVER2            /*168*/  "Unknown Collector"
IPDR_Status ExporterMsgs_Initialize(Log *L);

#ifdef __cplusplus
}
#endif

#endif
