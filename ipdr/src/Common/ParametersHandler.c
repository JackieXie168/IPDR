/** @file Common/ParametersHandler.c
 *
 * Functions implementing parameters retrieval using a given callback
 * function.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: ParametersHandler.c,v 1.12 2005/05/11 12:25:16 dmitryk Exp $
 *
 */

#include <IPDR/Config.h>
const char ParametersHandler_rcsid[] = "@(#) $Id: ParametersHandler.c,v 1.12 2005/05/11 12:25:16 dmitryk Exp $" IPDR_COPYRIGHT_STR;

#include <Common/ParametersHandler.h>

const char ParametersHandler_h_rcsid[] = PARAMETERS_HANDLER_H_RCSID;

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <Common/CommonUtil.h>
#include <Common/ConfigurationNames.h>
#include <Common/Log.h>
#include <Common/CommonMsgs.h>
#include <Exporter/Exporter.h>

static Log *ParamsLog = NULL ;
static unsigned long IPDRConfigurationMAX = 0 ;

static struct IPDR_tag_Configuration IPDRConfiguration[] = {
	{ 0, STR_configuration_exporterAddress										, "", "0.0.0.0", 0 },
	{ 1, STR_configuration_exporterPort											, "", "4737", 0 },
	{ 2, STR_configuration_logging												, "", "", 0 },
	{ 3, STR_configuration_logging_levels_moduleExporter						, "", "4", 0 }, /*IPDR_LEVEL_INFO = 4*/
	{ 4, STR_configuration_logging_levels_modulePlatform						, "", "4", 0 }, /*IPDR_LEVEL_INFO = 4*/
	{ 5, STR_configuration_logging_levels_moduleTCP								, "", "4", 0 }, /*IPDR_LEVEL_INFO = 4*/
	{ 6, STR_configuration_logging_levels_moduleHost							, "", "4", 0 }, /*IPDR_LEVEL_INFO = 4*/
	{ 7, STR_configuration_logging_levels_moduleQueue							, "", "4", 0 }, /*IPDR_LEVEL_INFO = 4*/
	{ 8, STR_configuration_logging_levels_moduleMemory							, "", "4", 0 }, /*IPDR_LEVEL_INFO = 4*/
	{ 9, STR_configuration_logging_levels_moduleProtocol						, "", "4", 0 }, /*IPDR_LEVEL_INFO = 4*/
	{ 10, STR_configuration_logging_levels_moduleJNI							, "", "4", 0 }, /*IPDR_LEVEL_INFO = 4*/
	{ 11, STR_configuration_logging_levels_moduleSERVER							, "", "4", 0 }, /*IPDR_LEVEL_INFO = 4*/
	{ 12, STR_configuration_logging_loggerAddress 								, "", "", 0 },
	{ 13, STR_configuration_logging_loggerPort									, "", "", 0 },
	{ 14, STR_configuration_protocolInfoAddr									, "", "0.0.0.0", 0 },
	{ 15, STR_configuration_protocolInfoPort									, "", "4737", 0 },
	{ 16, STR_configuration_statisticsInterval									, "", "60000", 0 },
	{ 17, STR_configuration_protocolLevelAddr									, "", "0.0.0.0", 0 },
	{ 18, STR_configuration_keepAliveInterval									, "", "10000", 0 },
	{ 19, STR_configuration_autoconnectInterval									, "", "10000", 0 },
	{ 20, STR_configuration_templateAckTimeout									, "", "15000", 0 },
	{ 21, STR_configuration_connectResponseTimeout								, "", "15000", 0 },
	{ 22, STR_configuration_waitBestCollectorTimeout							, "", "60000", 0 },
	{ 23, STR_configuration_dataAckTimeInterval									, "", "1000", 0 },
	{ 24, STR_configuration_dataAckSequenceInterval								, "", "50", 0 },
	{ 25, STR_configuration_capabilities_multiSession							, "", "1", 0 },
	{ 26, STR_configuration_capabilities_templateNegotiation					, "", "1", 0 },
	{ 27, STR_configuration_capabilities_structureSupport						, "", "0", 0 },
	{ 28, STR_configuration_vendorString										, "", "IPDR.org - IPDR Exporter", 0 },
	{ 29, STR_configuration_windowSize											, "", "1000000", 0 },
	{ 30, STR_configuration_windowTimeout										, "", "10000", 0 },
	{ 31, STR_configuration_console_consolePort									, "", "40000", 0 },
	{ 32, STR_configuration_console_consolePassword								, "", "", 0 },
	{ 33, STR_configuration_transport_tcpTransport_flushInterval				, "", "500", 0 },
	{ 34, STR_configuration_transport_tcpTransport_dataRcvChunkSize				, "", "16384", 0 },
	{ 35, STR_configuration_transport_tcpTransport_dataRcvInitialNumChunks		, "", "2", 0 },
	{ 36, STR_configuration_transport_tcpTransport_dataRcvMaxMemory				, "", "0" },
	{ 37, STR_configuration_transport_tcpTransport_dataSndChunkSize				, "", "4096", 0 },
	{ 38, STR_configuration_transport_tcpTransport_dataSndInitialNumChunks		, "", "2", 0 },
	{ 39, STR_configuration_transport_tcpTransport_dataSndMaxMemory				, "", "0", 0 },
	{ 40, STR_configuration_transport_tcpTransport_sendBuffersChunkSize			, "", "4096", 0 },
	{ 41, STR_configuration_transport_tcpTransport_sendBuffersInitialNumChunks	, "", "2", 0 },
	{ 42, STR_configuration_transport_tcpTransport_sendBuffersMaxMemory			, "", "0", 0 },
	{ 43, STR_configuration_queueChunkSize										, "", "16384", 0 },
	{ 44, STR_configuration_queueInitialNumChunks								, "", "4", 0 },
	{ 45, STR_configuration_queueMaxMemory										, "", "0", 0 },
	{ 0xFFFFFFFF, "0", "0", "0" , 0}
};

IPDR_Status ParametersHandler_ParametrsInit(IPDR_ExporterHandle ExporterHandle, ParametersHandler *P)
{
	unsigned long	Index = 0;
	const char*		Value = NULL ;
	Exporter *E = ExporterHandle;
	if( !E )
		return IPDR_ERR;

	ParamsLog = &(E->ExporterLog);

	if(ParamsLog)
		Log_Write(ParamsLog, IPDR_PLATFORM_MODULE_ID, IPDR_LEVEL_INFO,
                           "ParametersHandler_ParametrsInit",
                           "Configuration Initialization...");

	if ( (!P || ! P->ParamsFunc) && ParamsLog )
		Log_Write(ParamsLog, IPDR_PLATFORM_MODULE_ID, IPDR_LEVEL_WARNING,
                           "ParametersHandler_ParametrsInit",
                           "Parametrs Handler Func is NULL, All configuration Will Initialized Default...");

	for( Index = 0 ; IPDRConfiguration[Index].ID != 0xFFFFFFFF ; Index++)
	{
		if ( P && P->ParamsFunc)
		{
			Value = P->ParamsFunc(IPDRConfiguration[Index].Name, P->ParamsArg);
			if (Value != NULL)
			{
				strncpy(IPDRConfiguration[Index].Value, Value, MAX_CONFIG_STR_LEN);
				IPDRConfiguration[Index].Value[MAX_CONFIG_STR_LEN-1] = '\0';
				IPDRConfiguration[Index].Initialized = 1 ;
				if(ParamsLog)
				{
					Log_Write3(ParamsLog, IPDR_PLATFORM_MODULE_ID, IPDR_LEVEL_INFO,
									"ParametersHandler_ParametrsInit",
									"<%.98s>=<%.98s>[%.98s]",
									IPDRConfiguration[Index].Name,
									IPDRConfiguration[Index].Value,
									IPDRConfiguration[Index].DefValue);
				}
}
			else
			{
				IPDRConfiguration[Index].Value[0] = '\0' ;
				IPDRConfiguration[Index].Initialized = 0 ;
				if(ParamsLog)
				{
					Log_Write3(ParamsLog, IPDR_PLATFORM_MODULE_ID, IPDR_LEVEL_INFO,
									"ParametersHandler_ParametrsInit",
									"<%.98s>=<%.98s>[%.98s]",
									IPDRConfiguration[Index].Name,
									IPDRConfiguration[Index].Value,
									IPDRConfiguration[Index].DefValue);
				}
			}
		}

	}
	IPDRConfigurationMAX = Index-1 ;
	return IPDR_OK ;
}

IPDR_Status ParametersHandler_ParametrsDeInit(IPDR_ExporterHandle ExporterHandle)
{
	ParamsLog = NULL ;
	IPDRConfigurationMAX = 0 ;
	return IPDR_OK ;
}

IPDR_Status ParametersHandler_GetFirstParamName(unsigned long *Index, char** Name, char** Value, char** DefValue)
{
	if( ! ParamsLog )
		return IPDR_ERR;

	if(IPDRConfigurationMAX == 0)
	{
		Log_Write(ParamsLog, IPDR_PLATFORM_MODULE_ID, IPDR_LEVEL_ERROR,
							"ParametersHandler_GetFirstParamName",
							"Configuration Not Initialized !!! ");
		(*Name) = NULL ;
		(*Value) = NULL ;
		(*DefValue) = NULL ;
		(*Index) = 0xFFFFFFFF ;
		return IPDR_ERR ;
	}

	(*Name) = IPDRConfiguration[0].Name ;
	(*Value) = IPDRConfiguration[0].Value ;
	(*DefValue) = IPDRConfiguration[0].DefValue ;
	(*Index) = IPDRConfiguration[0].ID ;
	return IPDR_OK;
}

IPDR_Status ParametersHandler_GetNextParamName(unsigned long *Index, char** Name, char** Value, char** DefValue)
{
	if(	IPDRConfigurationMAX == 0 ||
		(*Index) >= IPDRConfigurationMAX ||
		(*Index) == 0xFFFFFFFF)
	{
		(*Name) = NULL ;
		(*Value) = NULL ;
		(*DefValue) = NULL ;
		(*Index) = 0xFFFFFFFF ;
		return IPDR_ERR ;
	}

	(*Name) = IPDRConfiguration[(*Index)+1].Name ;
	(*Value) = IPDRConfiguration[(*Index)+1].Value ;
	(*DefValue) = IPDRConfiguration[(*Index)+1].DefValue ;
	(*Index) = IPDRConfiguration[(*Index)+1].ID ;
	return IPDR_OK;
}

unsigned long ParametersHandler_GetNextParam(unsigned long Index, char** Name, char** Value, char** DefValue)
{
	if(Index == 0xFFFFFFFF || IPDRConfigurationMAX < Index)
	{
		(*Name) = NULL ;
		(*Value) = NULL ;
		(*DefValue) = NULL ;
		return 0xFFFFFFFF ;
	}

	(*Name) = IPDRConfiguration[Index].Name ;
	(*Value) = IPDRConfiguration[Index].Value ;
	(*DefValue) = IPDRConfiguration[Index].DefValue ;
	return IPDRConfiguration[Index+1].ID ;
}

unsigned long ParametersHandler_GetIndexByName(const char *Param)
{
	unsigned long Index = 0;
	if( ! ParamsLog )
		return 0xFFFFFFFF;


	if(Param == NULL)
	{
		Log_Write(ParamsLog, IPDR_PLATFORM_MODULE_ID, IPDR_LEVEL_ERROR,
							"ParametersHandler_GetIndexByName",
							"Parametr Name is NULL");
		return 0xFFFFFFFF ;
	}

	for( Index = 0 ; IPDRConfiguration[Index].ID != 0xFFFFFFFF ; Index++)
	{
		if(strcmp(Param, IPDRConfiguration[Index].Name) == 0)
			return Index ;
	}
	Log_Write1(ParamsLog, IPDR_PLATFORM_MODULE_ID, IPDR_LEVEL_ERROR,
							"ParametersHandler_GetIndexByName",
							"Parametr Name <%.98s> Not Found.", Param);
	return 0xFFFFFFFF ;
}

const char *ParametersHandler_GetStringParamByIndex(ParametersHandler *P, unsigned long Index)
{
	if( ! ParamsLog )
		return NULL;

	if (0xFFFFFFFF == Index || IPDRConfigurationMAX < Index)
	{
		Log_Write1(ParamsLog, IPDR_PLATFORM_MODULE_ID, IPDR_LEVEL_ERROR,
							"ParametersHandler_GetStringParamByIndex",
							"Parametr Index <%lu> Invalid, Return NULL", Index);
		return NULL;
	}
	if(IPDRConfiguration[Index].Initialized == 1)
	{
		return IPDRConfiguration[Index].Value ;
	}
	else
	{
		return IPDRConfiguration[Index].DefValue ;
	}
}
int ParametersHandler_GetIntParamByIndex(ParametersHandler *P, unsigned long Index)
{
	if( ! ParamsLog )
		return 0;

	if (0xFFFFFFFF == Index || IPDRConfigurationMAX < Index)
	{
		Log_Write1(ParamsLog, IPDR_PLATFORM_MODULE_ID, IPDR_LEVEL_ERROR,
							"ParametersHandler_GetIntParamByIndex",
							"Parametr Index <%lu> Invalid, Return <0>", Index);
		return 0;
	}

	if(IPDRConfiguration[Index].Initialized == 1)
	{
		return atoi(IPDRConfiguration[Index].Value) ;
	}
	else
	{
		return atoi(IPDRConfiguration[Index].DefValue) ;
	}
}
unsigned int ParametersHandler_GetUIntParamByIndex(ParametersHandler *P, unsigned long Index)
{
	if( ! ParamsLog )
		return 0;

	if (0xFFFFFFFF == Index || IPDRConfigurationMAX < Index)
	{
		Log_Write1(ParamsLog, IPDR_PLATFORM_MODULE_ID, IPDR_LEVEL_ERROR,
							"ParametersHandler_GetUIntParamByIndex",
							"Parametr Index <%lu> Invalid, Return <0>", Index);
		return 0;
	}

	if(IPDRConfiguration[Index].Initialized == 1)
	{
		return atoi(IPDRConfiguration[Index].Value) ;
	}
	else
	{
		return atoi(IPDRConfiguration[Index].DefValue) ;
	}
}
short ParametersHandler_GetShortParamByIndex(ParametersHandler *P, unsigned long Index)
{
	if( ! ParamsLog )
		return 0;

	if (0xFFFFFFFF == Index || IPDRConfigurationMAX < Index)
	{
		Log_Write1(ParamsLog, IPDR_PLATFORM_MODULE_ID, IPDR_LEVEL_ERROR,
							"ParametersHandler_GetShortParamByIndex",
							"Parametr Index <%lu> Invalid, Return <0>", Index);
		return 0;
	}

	if(IPDRConfiguration[Index].Initialized == 1)
	{
		return atoi(IPDRConfiguration[Index].Value) ;
	}
	else
	{
		return atoi(IPDRConfiguration[Index].DefValue) ;
	}
}
unsigned short ParametersHandler_GetUShortParamByIndex(ParametersHandler *P, unsigned long Index)
{
	if( ! ParamsLog )
		return 0;

	if (0xFFFFFFFF == Index || IPDRConfigurationMAX < Index)
	{
		Log_Write1(ParamsLog, IPDR_PLATFORM_MODULE_ID, IPDR_LEVEL_ERROR,
							"ParametersHandler_GetUShortParamByIndex",
							"Parametr Index <%lu> Invalid, Return <0>", Index);
		return 0;
	}

	if(IPDRConfiguration[Index].Initialized == 1)
	{
		return atoi(IPDRConfiguration[Index].Value) ;
	}
	else
	{
		return atoi(IPDRConfiguration[Index].DefValue) ;
	}
}

const char *ParametersHandler_GetStringParam(ParametersHandler *P,
                                             const char *Param)
{
	if (! Param)
		return NULL;

	return ParametersHandler_GetStringParamByIndex(P, ParametersHandler_GetIndexByName(Param));
}

short ParametersHandler_GetShortParam(ParametersHandler *P,
                                      const char *Param)
{
	if (! Param)
		return 0xFFFF;

    return ParametersHandler_GetShortParamByIndex(P, ParametersHandler_GetIndexByName(Param));
}

int ParametersHandler_GetIntParam(ParametersHandler *P,
                                  const char *Param)
{
	if (! Param)
		return 0xFFFFFFFF;

    return ParametersHandler_GetIntParamByIndex(P, ParametersHandler_GetIndexByName(Param));
}

unsigned int ParametersHandler_GetUIntParam(ParametersHandler *P,
                                            const char *Param)
{
	if (! Param)
		return 0xFFFFFFFF;

    return ParametersHandler_GetUIntParamByIndex(P, ParametersHandler_GetIndexByName(Param));
}

unsigned short ParametersHandler_GetUShortParam(ParametersHandler *P,
                                    const char *Param)
{
	if (! Param)
		return 0xFFFF;

    return ParametersHandler_GetUShortParamByIndex(P, ParametersHandler_GetIndexByName(Param));
}

IPDR_Status ParametersHandler_SetStringParam(ParametersHandler *P, const char *Param, const char *Value)
{
    unsigned long Index;
	if (! Param)
		return IPDR_ERR_INVALID_VALUE;

    Index = ParametersHandler_GetIndexByName(Param);
	if(0xFFFFFFFF == Index || Value == NULL || IPDRConfigurationMAX < Index)
		return IPDR_ERR_INVALID_VALUE ;


	strncpy(IPDRConfiguration[Index].Value, Value, MAX_CONFIG_STR_LEN);
	IPDRConfiguration[Index].Initialized = 1 ;
	return IPDR_OK ;
}
IPDR_Status ParametersHandler_SetIntParam(ParametersHandler *P, const char *Param, int Value)
{
    unsigned long Index;
	if (! Param)
		return IPDR_ERR_INVALID_VALUE;

    Index = ParametersHandler_GetIndexByName(Param);
	if(0xFFFFFFFF == Index || IPDRConfigurationMAX < Index)
		return IPDR_ERR_INVALID_VALUE ;

	sprintf(IPDRConfiguration[Index].Value, "%d", Value);
	IPDRConfiguration[Index].Initialized = 1 ;
	return IPDR_OK ;
}
IPDR_Status ParametersHandler_SetUIntParam(ParametersHandler *P, const char *Param, unsigned int Value)
{
    unsigned long Index;
	if (! Param)
		return IPDR_ERR_INVALID_VALUE;

    Index = ParametersHandler_GetIndexByName(Param);
	if(0xFFFFFFFF == Index || IPDRConfigurationMAX < Index)
		return IPDR_ERR_INVALID_VALUE ;


	sprintf(IPDRConfiguration[Index].Value, "%u", Value);
	IPDRConfiguration[Index].Initialized = 1 ;
	return IPDR_OK ;
}
IPDR_Status ParametersHandler_SetShortParam(ParametersHandler *P, const char *Param, short Value)
{
    unsigned long Index;
	if (! Param)
		return IPDR_ERR_INVALID_VALUE;

    Index = ParametersHandler_GetIndexByName(Param);
	if(0xFFFFFFFF == Index || IPDRConfigurationMAX < Index)
		return IPDR_ERR_INVALID_VALUE ;


	sprintf(IPDRConfiguration[Index].Value, "%d", Value);
	IPDRConfiguration[Index].Initialized = 1 ;
	return IPDR_OK ;
}
IPDR_Status ParametersHandler_SetUShortParam(ParametersHandler *P, const char *Param, unsigned short Value)
{
    unsigned long Index;
	if (! Param)
		return IPDR_ERR_INVALID_VALUE;

    Index = ParametersHandler_GetIndexByName(Param);
	if(0xFFFFFFFF == Index || IPDRConfigurationMAX < Index)
		return IPDR_ERR_INVALID_VALUE ;


	sprintf(IPDRConfiguration[Index].Value, "%u", Value);
	IPDRConfiguration[Index].Initialized = 1 ;
	return IPDR_OK ;
}


