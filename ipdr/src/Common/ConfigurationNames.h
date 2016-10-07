/** @file Common/ConfigurationNames.h
 *
 * Macros for defining all the configuration names, togethre
 * with default values.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: ConfigurationNames.h,v 1.1 2005/04/21 11:58:24 dmitryk Exp $
 *
 * You should use these definitions, to avoid typing errors.
 */


#ifndef _CONFIGURATION_NAMES_H_
#define _CONFIGURATION_NAMES_H_

#include <IPDR/Config.h>
#define CONFIGURATION_NAMES_H_RCSID "@(#) $Id: ConfigurationNames.h,v 1.1 2005/04/21 11:58:24 dmitryk Exp $" IPDR_COPYRIGHT_STR


#define MAX_CONFIG_STR_LEN 120

/* The following #define is for the longest possible configuration
   string, including trailers (that is, watch out for
   "configuration.logging.levels.module%.39s", which allows a module
   name on the end. */

/* strlen("configuration.transport.tcpTransport.sendBuffersInitialNumChunks") == 64
   You can find them all (if you're worried that the value isn't big
   enough) by this Unix command:
       strings ./client/Source/IPDR/bin/solaris_opt/IPDRsim |
          fgrep 'configuration.' | sort | uniq |
          perl -n -e 'chomp; print length, " $_\n"' |
          sort -n -r | head
*/

/* Used to build up a few of the strings: */

#define STR__ChunkSize        "ChunkSize"
#define STR__InitialNumChunks "InitialNumChunks"
#define STR__MaxMemory        "MaxMemory"


#define STR_queue__                               "queue"
#define STR_dataRcv__                             "dataRcv"
#define STR_dataSnd__                             "dataSnd"
#define STR_sendBuffers__                         "sendBuffers"


#define STR_addServer                             "addServer"
#define STR_addServerAddress                      "addServerAddress"
#define STR_addServerExecute                      "addServerExecute"
#define STR_addServerName                         "addServerName"
#define STR_addServerPort                         "addServerPort"
#define STR_addServerPriority                     "addServerPriority"
#define STR_addServerSessionID                    "addServerSessionID"
#define STR_addSession                            "addSession"
#define STR_addSessionDescription                 "addSessionDescription"
#define STR_addSessionExecute                     "addSessionExecute"
#define STR_addSessionID                          "addSessionID"
#define STR_addSessionName                        "addSessionName"
#define STR_client                                "client"
#define STR_exporterAddress                       "exporterAddress"
#define STR_exporterPort                          "exporterPort"
#define STR_commands                              "commands"
#define STR_configuration                         "configuration"
#define STR_configurationCommandResult            "configurationCommandResult"
#define STR_configurationCommandString            "configurationCommandString"
#define STR_console                               "console"
#define STR_consolePassword                       "consolePassword"
#define STR_consolePort                           "consolePort"
#define STR_crane                                 "crane"
#define STR_crane                                 "crane"
#define STR_dataRcvChunkSize                      STR_dataRcv__ STR__ChunkSize
#define STR_dataRcvInitialNumChunks               STR_dataRcv__ STR__InitialNumChunks
#define STR_dataRcvMaxMemory                      STR_dataRcv__ STR__MaxMemory
#define STR_dataSndChunkSize                      STR_dataSnd__ STR__ChunkSize
#define STR_dataSndInitialNumChunks               STR_dataSnd__ STR__InitialNumChunks
#define STR_dataSndMaxMemory                      STR_dataSnd__ STR__MaxMemory
#define STR_dod                                   "dod"
#define STR_enterprises                           "enterprises"
#define STR_flushInterval                         "flushInterval"
#define STR_internet                              "internet"
#define STR_keyAttributes                         "keyAttributes"
#define STR_keyAttributesKeyID                    "keyAttributesKeyID"
#define STR_keyAttributesSessionID                "keyAttributesSessionID"
#define STR_keyAttributesTemplateID               "keyAttributesTemplateID"
#define STR_keyHelp                               "keyHelp"
#define STR_keyID                                 "keyID"
#define STR_keyLabel                              "keyLabel"
#define STR_keyName                               "keyName"
#define STR_keyTemplateID                         "keyTemplateID"
#define STR_keyTypeID                             "keyTypeID"
#define STR_keysAttributesEntry                   "keysAttributesEntry"
#define STR_keysAttributesTable                   "keysAttributesTable"
#define STR_keysEntry                             "keysEntry"
#define STR_keysTable                             "keysTable"
#define STR_levels                                "levels"
#define STR_loggerAddress                         "loggerAddress"
#define STR_loggerPort                            "loggerPort"
#define STR_logging                               "logging"
#define STR_moduleExporter                        "moduleExporter"
#define STR_moduleHost                            "moduleHost"
#define STR_moduleMemory                          "moduleMemory"
#define STR_modulePlatform                        "modulePlatform"
#define STR_moduleProtocol                        "moduleProtocol"
#define STR_moduleQueue                           "moduleQueue"
#define STR_moduleTCP                             "moduleTCP"
#define STR_org                                   "org"
#define STR_private                               "private"
#define STR_protocolInfoAddr                      "protocolInfoAddr"
#define STR_protocolInfoPort                      "protocolInfoPort"
#define STR_queueChunkSize                        STR_queue__ STR__ChunkSize
#define STR_queueInitialNumChunks                 STR_queue__ STR__InitialNumChunks
#define STR_queueMaxMemory                        STR_queue__ STR__MaxMemory
#define STR_removeServer                          "removeServer"
#define STR_removeServerAddress                   "removeServerAddress"
#define STR_removeServerExecute                   "removeServerExecute"
#define STR_removeServerPort                      "removeServerPort"
#define STR_removeServerSessionID                 "removeServerSessionID"
#define STR_removeSession                         "removeSession"
#define STR_removeSessionExecute                  "removeSessionExecute"
#define STR_removeSessionID                       "removeSessionID"
#define STR_sendBuffersChunkSize                  STR_sendBuffers__ STR__ChunkSize
#define STR_sendBuffersInitialNumChunks           STR_sendBuffers__ STR__InitialNumChunks
#define STR_sendBuffersMaxMemory                  STR_sendBuffers__ STR__MaxMemory
#define STR_server                                "server"
#define STR_serverAcknowledgedRecords             "serverAcknowledgedRecords"
#define STR_serverAddress                         "serverAddress"
#define STR_serverEnqueuedRecords                 "serverEnqueuedRecords"
#define STR_serverLostRecords                     "serverLostRecords"
#define STR_serverName                            "serverName"
#define STR_serverPort                            "serverPort"
#define STR_serverPriority                        "serverPriority"
#define STR_serverSessionID                       "serverSessionID"
#define STR_serverStatus                          "serverStatus"
#define STR_serverTransmittedRecords              "serverTransmittedRecords"
#define STR_serversEntry                          "serversEntry"
#define STR_serversTable                          "serversTable"
#define STR_sessionAcknowledgedRecords            "sessionAcknowledgedRecords"
#define STR_sessionDescription                    "sessionDescription"
#define STR_sessionEnqueuedRecords                "sessionEnqueuedRecords"
#define STR_sessionID                             "sessionID"
#define STR_sessionLostRecords                    "sessionLostRecords"
#define STR_sessionName                           "sessionName"
#define STR_sessionOutstandingRecords             "sessionOutstandingRecords"
#define STR_sessionTransmittedRecords             "sessionTransmittedRecords"
#define STR_sessionUnacknowledgedRecords          "sessionUnacknowledgedRecords"
#define STR_sessionsEntry                         "sessionsEntry"
#define STR_sessionsTable                         "sessionsTable"
#define STR_statisticsInterval                    "statisticsInterval"
#define STR_tcpTransport                          "tcpTransport"
#define STR_templateDescription                   "templateDescription"
#define STR_templateID                            "templateID"
#define STR_templatesEntry                        "templatesEntry"
#define STR_templatesTable                        "templatesTable"
#define STR_protocolLevelAddr                     "protocolLevelAddr"
#define STR_transport                             "transport"
#define STR_vendorString                          "vendorString"
#define STR_windowSize                            "windowSize"
#define STR_windowTimeout                         "windowTimeout"

/* IPDR Additional configuration */
#define STR_capabilities							"capabilities"
#define STR_keepAliveInterval						"keepAliveInterval"
#define STR_autoconnectInterval						"autoconnectInterval"
#define STR_templateAckTimeout						"templateAckTimeout"
#define STR_connectResponseTimeout					"connectResponseTimeout"
#define STR_waitBestCollectorTimeout				"waitBestCollectorTimeout"
#define STR_dataAckTimeInterval						"dataAckTimeInterval"
#define STR_dataAckSequenceInterval					"dataAckSequenceInterval"
#define STR_multiSession							"multiSession"
#define STR_templateNegotiation						"templateNegotiation"
#define STR_structureSupport						"structureSupport"


/* For Exporter: */
#define STR_moduleS								"module%.39s" /* trailing string gets module name for debug level */
#define STR_moduleExporter						"moduleExporter"
#define STR_modulePlatform						"modulePlatform"
#define STR_moduleTCP							"moduleTCP"
#define STR_moduleHost							"moduleHost"
#define STR_moduleQueue							"moduleQueue"
#define STR_moduleMemory						"moduleMemory"
#define STR_moduleProtocol						"moduleProtocol"
#define STR_moduleJNI							"moduleJNI"
#define STR_moduleSERVER						"moduleSERVER"

#define STR_configuration_exporterAddress          (STR_configuration "." STR_exporterAddress)
#define STR_configuration_exporterPort             (STR_configuration "." STR_exporterPort)
#define STR_configuration_logging                (STR_configuration "." STR_logging)
#define STR_configuration_logging_levels_moduleS (STR_configuration "." STR_logging "." STR_levels "." STR_moduleS)
#define STR_configuration_logging_levels_moduleExporter		(STR_configuration "." STR_logging "." STR_levels "." STR_moduleExporter)
#define STR_configuration_logging_levels_modulePlatform		(STR_configuration "." STR_logging "." STR_levels "." STR_modulePlatform)
#define STR_configuration_logging_levels_moduleTCP				(STR_configuration "." STR_logging "." STR_levels "." STR_moduleTCP)
#define STR_configuration_logging_levels_moduleHost			(STR_configuration "." STR_logging "." STR_levels "." STR_moduleHost)
#define STR_configuration_logging_levels_moduleQueue			(STR_configuration "." STR_logging "." STR_levels "." STR_moduleQueue)
#define STR_configuration_logging_levels_moduleMemory			(STR_configuration "." STR_logging "." STR_levels "." STR_moduleMemory)
#define STR_configuration_logging_levels_moduleProtocol		(STR_configuration "." STR_logging "." STR_levels "." STR_moduleProtocol)
#define STR_configuration_logging_levels_moduleJNI	(STR_configuration "." STR_logging "." STR_levels "." STR_moduleJNI)
#define STR_configuration_logging_levels_moduleSERVER	(STR_configuration "." STR_logging "." STR_levels "." STR_moduleSERVER)
#define STR_configuration_logging_loggerAddress  (STR_configuration "." STR_logging "." STR_loggerAddress)
#define STR_configuration_logging_loggerPort     (STR_configuration "." STR_logging "." STR_loggerPort)
#define STR_configuration_protocolInfoAddr       (STR_configuration "." STR_protocolInfoAddr)
#define STR_configuration_protocolInfoPort       (STR_configuration "." STR_protocolInfoPort)
#define STR_configuration_statisticsInterval     (STR_configuration "." STR_statisticsInterval)
#define STR_configuration_protocolLevelAddr        (STR_configuration "." STR_protocolLevelAddr)

#define STR_configuration_keepAliveInterval					(STR_configuration "." STR_keepAliveInterval)
#define STR_configuration_autoconnectInterval				(STR_configuration "." STR_autoconnectInterval)
#define STR_configuration_templateAckTimeout				(STR_configuration "." STR_templateAckTimeout)
#define STR_configuration_connectResponseTimeout			(STR_configuration "." STR_connectResponseTimeout)
#define STR_configuration_waitBestCollectorTimeout			(STR_configuration "." STR_waitBestCollectorTimeout)
#define STR_configuration_dataAckTimeInterval				(STR_configuration "." STR_dataAckTimeInterval)
#define STR_configuration_dataAckSequenceInterval			(STR_configuration "." STR_dataAckSequenceInterval)
#define STR_configuration_capabilities_multiSession			(STR_configuration "." STR_capabilities "." STR_multiSession)
#define STR_configuration_capabilities_templateNegotiation	(STR_configuration "." STR_capabilities "." STR_templateNegotiation)
#define STR_configuration_capabilities_structureSupport		(STR_configuration "." STR_capabilities "." STR_structureSupport)

#define STR_configuration_vendorString           (STR_configuration "." STR_vendorString)
#define STR_configuration_windowSize             (STR_configuration "." STR_windowSize)
#define STR_configuration_windowTimeout          (STR_configuration "." STR_windowTimeout)

/* For Console: */

#define STR_configuration_console_consolePort          (STR_configuration "." STR_console "." STR_consolePort)
#define STR_configuration_console_consolePassword      (STR_configuration "." STR_console "." STR_consolePassword)

/* For TCPTransport: */

#define STR_configuration_transport_tcpTransport_                             STR_configuration "." STR_transport "." STR_tcpTransport "."

#define STR_configuration_transport_tcpTransport_flushInterval                (STR_configuration_transport_tcpTransport_ STR_flushInterval)
#define STR_configuration_transport_tcpTransport_dataRcvChunkSize             (STR_configuration_transport_tcpTransport_ STR_dataRcvChunkSize)
#define STR_configuration_transport_tcpTransport_dataRcvInitialNumChunks      (STR_configuration_transport_tcpTransport_ STR_dataRcvInitialNumChunks)
#define STR_configuration_transport_tcpTransport_dataRcvMaxMemory             (STR_configuration_transport_tcpTransport_ STR_dataRcvMaxMemory)
#define STR_configuration_transport_tcpTransport_dataSndChunkSize             (STR_configuration_transport_tcpTransport_ STR_dataSndChunkSize)
#define STR_configuration_transport_tcpTransport_dataSndInitialNumChunks      (STR_configuration_transport_tcpTransport_ STR_dataSndInitialNumChunks)
#define STR_configuration_transport_tcpTransport_dataSndMaxMemory             (STR_configuration_transport_tcpTransport_ STR_dataSndMaxMemory)
#define STR_configuration_transport_tcpTransport_sendBuffersChunkSize         (STR_configuration_transport_tcpTransport_ STR_sendBuffersChunkSize)
#define STR_configuration_transport_tcpTransport_sendBuffersInitialNumChunks  (STR_configuration_transport_tcpTransport_ STR_sendBuffersInitialNumChunks)
#define STR_configuration_transport_tcpTransport_sendBuffersMaxMemory         (STR_configuration_transport_tcpTransport_ STR_sendBuffersMaxMemory)

/* For VolatileQueue: */

#define STR_configuration_queueChunkSize         (STR_configuration "." STR_queueChunkSize)
#define STR_configuration_queueInitialNumChunks  (STR_configuration "." STR_queueInitialNumChunks)
#define STR_configuration_queueMaxMemory         (STR_configuration "." STR_queueMaxMemory)

typedef struct IPDR_tag_Configuration
{
	unsigned long	ID ;
	char			Name[MAX_CONFIG_STR_LEN] ;
	char			Value[MAX_CONFIG_STR_LEN] ;
	char			DefValue[MAX_CONFIG_STR_LEN] ;
	unsigned char	Initialized ;
}IPDR_Configuration;

#endif
