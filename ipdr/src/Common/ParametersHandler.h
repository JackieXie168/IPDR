/** @file Common/ParametersHandler.h
 *
 * Functions implementing parameters retrieval using a given callback
 * function.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: ParametersHandler.h,v 1.12 2005/05/11 12:48:29 dmitryk Exp $
 *
 */

#ifndef _PARAMETERS_HANDLER_H_
#define _PARAMETERS_HANDLER_H_

#include <IPDR/Config.h>
#define PARAMETERS_HANDLER_H_RCSID "@(#) $Id: ParametersHandler.h,v 1.12 2005/05/11 12:48:29 dmitryk Exp $" IPDR_COPYRIGHT_STR

#if defined(GLOBAL_RENAME)
#define ParametersHandler_GetIntParam    IPDR__ParametersHandler_GetIntParam
#define ParametersHandler_GetLongParam   IPDR__ParametersHandler_GetLongParam
#define ParametersHandler_GetShortParam  IPDR__ParametersHandler_GetShortParam
#define ParametersHandler_GetStringParam IPDR__ParametersHandler_GetStringParam
#define ParametersHandler_GetUIntParam   IPDR__ParametersHandler_GetUIntParam
#define ParametersHandler_h_rcsid        IPDR__ParametersHandler_h_rcsid
#define ParametersHandler_rcsid          IPDR__ParametersHandler_rcsid
#endif

#include <IPDR/Defs.h>
#include <IPDR/IPDR.h>
#include <IPDR/Config.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    IPDR_ParametersHandlerFunc ParamsFunc;
    void *ParamsArg;
}
ParametersHandler;

unsigned long ParametersHandler_GetNextParam(unsigned long Index, char** Name, char** Value, char** DefValue);
IPDR_Status ParametersHandler_GetNextParamName(unsigned long *Index, char** Name, char** Value, char** DefValue);
IPDR_Status ParametersHandler_GetFirstParamName(unsigned long *Index, char** Name, char** Value, char** DefValue);

unsigned long ParametersHandler_GetIndexByName(const char *Param);

IPDR_Status ParametersHandler_ParametrsInit(IPDR_ExporterHandle ExporterHandle, ParametersHandler *P);
IPDR_Status ParametersHandler_ParametrsDeInit(IPDR_ExporterHandle ExporterHandle);

const char *ParametersHandler_GetStringParam(ParametersHandler *P, const char *Param);
int ParametersHandler_GetIntParam(ParametersHandler *P, const char *Param);
unsigned int ParametersHandler_GetUIntParam(ParametersHandler *P, const char *Param);
short ParametersHandler_GetShortParam(ParametersHandler *P, const char *Param);
unsigned short ParametersHandler_GetUShortParam(ParametersHandler *P, const char *Param);

const char *ParametersHandler_GetStringParamByIndex(ParametersHandler *P, unsigned long Index);
int ParametersHandler_GetIntParamByIndex(ParametersHandler *P, unsigned long Index);
unsigned int ParametersHandler_GetUIntParamByIndex(ParametersHandler *P, unsigned long Index);
short ParametersHandler_GetShortParamByIndex(ParametersHandler *P, unsigned long Index);
unsigned short ParametersHandler_GetUShortParamByIndex(ParametersHandler *P, unsigned long Index);

IPDR_Status ParametersHandler_SetStringParam(ParametersHandler *P, const char *Param, const char *Value);
IPDR_Status ParametersHandler_SetIntParam(ParametersHandler *P, const char *Param, int Value);
IPDR_Status ParametersHandler_SetUIntParam(ParametersHandler *P, const char *Param, unsigned int Value);
IPDR_Status ParametersHandler_SetShortParam(ParametersHandler *P, const char *Param, short Value);
IPDR_Status ParametersHandler_SetUShortParam(ParametersHandler *P, const char *Param, unsigned short Value);

#ifdef __cplusplus
}
#endif

#endif
