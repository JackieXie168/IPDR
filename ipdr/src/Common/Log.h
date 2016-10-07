/** @file Common/Log.h
 *
 * Header file for IPDR logging service, as implemented in Common/Log.c.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: Log.h,v 1.13 2005/04/21 11:31:22 dmitryk Exp $ Copyright (c)
 *
 */

#ifndef _LOG_H_
#define _LOG_H_

#include <IPDR/Config.h>
#define LOG_H_RCSID "@(#) $Id: Log.h,v 1.13 2005/04/21 11:31:22 dmitryk Exp $" IPDR_COPYRIGHT_STR

#if defined(GLOBAL_RENAME)
#define Log_CheckLogLevel           IPDR__Log_CheckLogLevel
#define Log_CheckLogLevelByName     IPDR__Log_CheckLogLevelByName
#define Log_Disable                 IPDR__Log_Disable
#define Log_Enable                  IPDR__Log_Enable
#define Log_FillStrHdr              IPDR__Log_FillStrHdr
#define Log_Finalize                IPDR__Log_Finalize
#define Log_Initialize              IPDR__Log_Initialize
#define Log_LogPlatformError        IPDR__Log_LogPlatformError
#define Log_LogPlatformSocketError  IPDR__Log_LogPlatformSocketError
#define Log_OutputMessage           IPDR__Log_OutputMessage
#define Log_RegisterConsoleCommands IPDR__Log_RegisterConsoleCommands
#define Log_RegisterModule          IPDR__Log_RegisterModule
#define Log_SetChangeCallback       IPDR__Log_SetChangeCallback
#define Log_SetLoggerAddress        IPDR__Log_SetLoggerAddress
#define Log_SetModuleDebugLevel     IPDR__Log_SetModuleDebugLevel
#define Log_SetModuleMoreDebugLevel IPDR__Log_SetModuleMoreDebugLevel
#define Log_SetOutputCallback       IPDR__Log_SetOutputCallback
#define Log_SetOutputCallbackDetailed IPDR__Log_SetOutputCallbackDetailed
#define Log_Start                   IPDR__Log_Start
#define Log_VOutputStr              IPDR__Log_VOutputStr
#define Log_VOutputStrByName        IPDR__Log_VOutputStrByName
#define Log_h_rcsid                 IPDR__Log_h_rcsid
#define Log_rcsid                   IPDR__Log_rcsid
#define Module_Destructor           IPDR__Module_Destructor
#define Message_Destructor          IPDR__Message_Destructor
#define Module_Destructor           IPDR__Module_Destructor
#endif

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include <IPDR/Defs.h>
#include <IPDR/IPDR.h>
#include <Common/DynamicArray.h>

#ifdef __cplusplus
extern "C" {
#endif

#if 0
} /* put this here to un-confuse the Emacs's automatic indentation */
#endif


/** Opaque type for module IDs.
 */
typedef int IPDR_ModuleID;

#define IPDR_HOST_APP_MODULE_ID      0 /**< "<code>Host</code>" module. */
#define IPDR_PLATFORM_MODULE_ID      1 /**< "<code>Platform</code>" module. */


/* generic log messages */

#define MSG_LOG_UNKNOWN_MSG         /* 0*/  "<unknown message>"
#define MSG_LOG_ENTER               /* 1*/  "Enter"
#define MSG_LOG_LEAVE               /* 2*/  "Leave"
#define MSG_LOG_LEAVE_ERR           /* 3*/  "Error - Status=%d"
#define MSG_LOG_PLATFORM_ERR        /* 4*/  "Error - Func=%.99s, Error=%.99s"
#define MSG_LOG_PLATFORM_SOCKET_ERR /* 5*/  "Error - Func=%.99s, Error=%.99s (addr=%.30s socket=" Platform_SOCKET_FMT ")"
#define MSG_LOG_PLATFORM_CONN_ERR   /* 6*/  "Error - Func=%.99s, Error=%.99s, Status=%.99s, for %.120s"
#define MSG_LOG_PLATFORM_CONN_EOF   /* 7*/  "End of input: Func=%.99s, Status=%.99s, for %.120s"

struct IPDR_tag_Console;

typedef void (*Log_SettingsChangedCallback)(void *Arg);
typedef void (*Log_OutputCallback)(const char *Msg, void *Arg);
typedef void (*Log_OutputCallbackDetailed)(IPDR_DebugLevel Level,
                                           const char *LevelStr,
                                           const char *ExporterName,
                                           const char *Module,
                                           const char *Func,
                                           const char *Msg,
                                           void *Arg);

typedef int MessageID_t;

typedef struct
{
    const char *Msg;
}
Message;

typedef struct
{
    char eyecatcher[8]; /* Gets "MODULE " */
    IPDR_DebugLevel Level;
    char *Name;
}
Module;

#define LOG_SAVE_STR_LENGTH 300
/* If LOG_SAVE_STR_NUM is greater than 1, the statistics messages look
   weird (because some lines repeat). So, take a chance that there
   aren't cascades of messages and only look 1 line back (this won't
   work if there is a cascade of double messages, i.e., one error
   causing 2 messages). */
#define LOG_SAVE_STR_NUM 1 /*10*/
#define LOG_MAX_REPEATS 999
#define LOG_MAX_REPEAT_INTVL 120 /* seconds */

struct LogSaveStr
{ /* information needed for catching repetitive messages */
    int repeats;
    int firstTime; /* time stamp of first time saved */
    IPDR_ModuleID moduleID;
    IPDR_DebugLevel level;
    const char *func;
    char str[LOG_SAVE_STR_LENGTH];
};

typedef struct IPDR_tag_Log
{
    char eyecatcher[8]; /* Gets "LOG    " */
    char *ComponentName;
    DynamicArray Modules;
    IPDR_PlatformServices *PS;
    IPDR_IPAddress LoggerAddr;
    unsigned short LoggerPort;
    unsigned int LoggerFacility;
    char* LoggerMachineName;

    IPDR_SocketHandle LogFD;
    IPDR_BOOL Enabled;

    IPDR_BOOL BypassCallback; /* for debugging: turn off the callback and use regular printf */

    Log_SettingsChangedCallback ChangeCallback;
    void *ChangeCallbackArg;

    Log_OutputCallback OutputCallback;
    Log_OutputCallbackDetailed OutputCallbackDetailed;
    void *OutputCallbackArg;

    int saveI; /* index of last entry in save[] ... initially -1 */
    struct LogSaveStr save[LOG_SAVE_STR_NUM];

    char msgStr[IPDR_MAX_ERROR_LENGTH+1]; /* used to output messages and avoid stack space */
    char msgStrWithHdr[IPDR_MAX_ERROR_LENGTH+101]; /* ditto (for simple calback) */
    char msgStrErr[IPDR_MAX_ERROR_LENGTH+1]; /* ditto (for socketError, platformError) */
}
Log;

#ifdef __cplusplus
extern "C"
{
#endif

    IPDR_Status Log_Initialize(Log *L, IPDR_PlatformServices *PS, const char *ComponentName);
    IPDR_Status Log_Start(Log *L);
    IPDR_Status Log_Finalize(Log *L);
    IPDR_Status Log_RegisterModule(Log *L, IPDR_ModuleID ID, const char *Name);
    IPDR_Status Log_SetModuleDebugLevel(Log *L, const char *Name, IPDR_DebugLevel Level);
    IPDR_Status Log_SetModuleMoreDebugLevel(Log *L, const char *Name, IPDR_DebugLevel Level);
    IPDR_Status Log_VOutputStr(Log *L, IPDR_ModuleID ModuleID, IPDR_DebugLevel Level,
                                const char *Func, const char *Msg, va_list Args);
    IPDR_Status Log_VOutputStrByName(Log *L, const char* ModuleName, IPDR_DebugLevel Level,
                                      const char *Func, const char *Msg, va_list Args);
    IPDR_Status Log_SetLoggerAddress(Log *L, IPDR_IPAddress Addr, unsigned short Port,
                                      unsigned int Facility, const char* MachineName);

    IPDR_Status Log_CheckLogLevel(Log *L, IPDR_ModuleID ModuleID, IPDR_DebugLevel Lvl);
    IPDR_Status Log_CheckLogLevelByName(Log *L, const char* Name, IPDR_DebugLevel Lvl);

    IPDR_Status Log_LogPlatformError(Log *L, IPDR_ModuleID ModuleID,
                                      const char *Func, const char *PlatformFunc);
    IPDR_Status Log_LogPlatformSocketError(Log *L, IPDR_ModuleID ModuleID,
                                            const char *Func, const char *PlatformFunc,
                                            IPDR_IPAddress Addr, unsigned short Port, IPDR_SocketHandle FD);

/* The following declaration is in TCPTransport.c for now, because of circular header files:
   IPDR_Status Log_LogPlatformConnError(Log *L, IPDR_ModuleID ModuleID,
   const char *Func, const char *PlatformFunc, Connection Conn);
*/

    IPDR_Status Log_Enable(Log *L);
    IPDR_Status Log_Disable(Log *L);

    IPDR_Status Log_RegisterConsoleCommands(Log *L, struct IPDR_tag_Console *C);

    IPDR_Status Log_SetChangeCallback(Log *L, Log_SettingsChangedCallback CB,
                                       void *CallbackArg);

    IPDR_Status Log_SetOutputCallback(Log *L, Log_OutputCallback CB,
                                       void *CallbackArg);
    IPDR_Status Log_SetOutputCallbackDetailed(Log *L, Log_OutputCallbackDetailed CB,
                                               void *CallbackArg);

    IPDR_Status Log_OutputMessage(Log *L, const char *Msg, IPDR_ModuleID ID,
                                   IPDR_DebugLevel Level, const char *Func);

    void Module_Destructor(IPDR_PlatformServices *PS, void *Obj);
    void Message_Destructor(IPDR_PlatformServices *PS, void *Obj);

#ifdef __cplusplus
}
#endif

//#define TEST_LOG_WITH_MACRO

#ifdef TEST_LOG_WITH_MACRO
#define Log_ShouldWrite(L, ID, Lvl) \
   ((L)->Enabled && ((Module *) (L)->Modules.Data[ID])->Level >= (Lvl))
#else
#define Log_ShouldWrite(L, ID, Lvl) ( Log_ShouldWriteD(L, ID, Lvl) )
int Log_ShouldWriteD(Log *L, int ID, int Lvl);
#endif

/* We used to have macros and functions that depended on varargs. However,
   some systems don't implement varargs correctly (at least, with passing
   varags to other functions), so everything that requires varargs is
   done outside, using sprintf.
   [snprintf would be better, but it's not ANSI C. Anyway, we make sure
   that all our messages have maximum string lengths.]
*/

#define Log_OutputStr(L, ID, Lvl, Func, Str) \
{ if Log_ShouldWrite(L, ID, Lvl) \
  { sprintf((L)->msgStr, Str); Log_OutputMessage(L, (L)->msgStr, ID, Lvl, Func); }}
#define Log_OutputStr1(L, ID, Lvl, Func, Str, Arg1) \
{ if Log_ShouldWrite(L, ID, Lvl) \
  { sprintf((L)->msgStr, Str, Arg1); Log_OutputMessage(L,(L)->msgStr, ID, Lvl, Func); }}
#define Log_OutputStr2(L, ID, Lvl, Func, Str, Arg1, Arg2) \
{ if Log_ShouldWrite(L, ID, Lvl) \
  { sprintf((L)->msgStr, Str, Arg1, Arg2); Log_OutputMessage(L,(L)->msgStr, ID, Lvl, Func); }}
#define Log_OutputStr3(L, ID, Lvl, Func, Str, Arg1, Arg2, Arg3) \
{ if Log_ShouldWrite(L, ID, Lvl) \
  { sprintf((L)->msgStr, Str, Arg1, Arg2, Arg3); Log_OutputMessage(L,(L)->msgStr, ID, Lvl, Func); }}
#define Log_OutputStr4(L, ID, Lvl, Func, Str, Arg1, Arg2, Arg3, Arg4) \
{ if Log_ShouldWrite(L, ID, Lvl) \
  { sprintf((L)->msgStr, Str, Arg1, Arg2, Arg3, Arg4); Log_OutputMessage(L,(L)->msgStr, ID, Lvl, Func); }}
#define Log_OutputStr5(L, ID, Lvl, Func, Str, Arg1, Arg2, Arg3, Arg4, Arg5) \
{ if Log_ShouldWrite(L, ID, Lvl) \
  { sprintf((L)->msgStr, Str, Arg1, Arg2, Arg3, Arg4, Arg5); Log_OutputMessage(L,(L)->msgStr, ID, Lvl, Func); }}
#define Log_OutputStr6(L, ID, Lvl, Func, Str, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) \
{ if Log_ShouldWrite(L, ID, Lvl) \
  { sprintf((L)->msgStr, Str, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6); Log_OutputMessage(L,(L)->msgStr, ID, Lvl, Func); }}
#define Log_OutputStr7(L, ID, Lvl, Func, Str, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) \
{ if Log_ShouldWrite(L, ID, Lvl) \
  { sprintf((L)->msgStr, Str, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7); Log_OutputMessage(L,(L)->msgStr, ID, Lvl, Func); }}
#define Log_OutputStr8(L, ID, Lvl, Func, Str, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) \
{ if Log_ShouldWrite(L, ID, Lvl) \
  { sprintf((L)->msgStr, Str, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8); Log_OutputMessage(L,(L)->msgStr, ID, Lvl, Func); }}
#define Log_OutputStr9(L, ID, Lvl, Func, Str, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) \
{ if Log_ShouldWrite(L, ID, Lvl) \
  { sprintf((L)->msgStr, Str, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9); Log_OutputMessage(L,(L)->msgStr, ID, Lvl, Func); }}
#define Log_OutputStr10(L, ID, Lvl, Func, Str, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10) \
{ if Log_ShouldWrite(L, ID, Lvl) \
  { sprintf((L)->msgStr, Str, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10); Log_OutputMessage(L,(L)->msgStr, ID, Lvl, Func); }}

#define Log_StateTransition(L, ID, Lvl, Func, Conn, OldState, NewState, ConditionStr) /*\*/
/*{if Log_ShouldWrite(L, ID, Lvl) \*/
/*  { Log_Write4(L, ID, Lvl, Func, MSG_EXPORTER_STATE_TRANSITION, TL_GetConnectionInfoStr(Conn), GetCSStateStr(OldState), GetCSStateStr(NewState), ConditionStr); } }*/

/* Convenience macros, to avoid making changes from the old way to the new style */

#define Log_Write(L, ID, Lvl, Func, MsgID)                           Log_OutputStr(L, ID, Lvl, Func, MsgID)
#define Log_Write1(L, ID, Lvl, Func, MsgID, A1)                      Log_OutputStr1(L, ID, Lvl, Func, MsgID, A1)
#define Log_Write2(L, ID, Lvl, Func, MsgID, A1,A2)                   Log_OutputStr2(L, ID, Lvl, Func, MsgID, A1,A2)
#define Log_Write3(L, ID, Lvl, Func, MsgID, A1,A2,A3)                Log_OutputStr3(L, ID, Lvl, Func, MsgID, A1,A2,A3)
#define Log_Write4(L, ID, Lvl, Func, MsgID, A1,A2,A3,A4)             Log_OutputStr4(L, ID, Lvl, Func, MsgID, A1,A2,A3,A4)
#define Log_Write5(L, ID, Lvl, Func, MsgID, A1,A2,A3,A4,A5)          Log_OutputStr5(L, ID, Lvl, Func, MsgID, A1,A2,A3,A4,A5)
#define Log_Write6(L, ID, Lvl, Func, MsgID, A1,A2,A3,A4,A5,A6)       Log_OutputStr6(L, ID, Lvl, Func, MsgID, A1,A2,A3,A4,A5,A6)
#define Log_Write7(L, ID, Lvl, Func, MsgID, A1,A2,A3,A4,A5,A6,A7)    Log_OutputStr7(L, ID, Lvl, Func, MsgID, A1,A2,A3,A4,A5,A6,A7)
#define Log_Write8(L, ID, Lvl, Func, MsgID, A1,A2,A3,A4,A5,A6,A7,A8) Log_OutputStr8(L, ID, Lvl, Func, MsgID, A1,A2,A3,A4,A5,A6,A7,A8)
#define Log_Write9(L, ID, Lvl, Func, MsgID, A1,A2,A3,A4,A5,A6,A7,A8,A9) Log_OutputStr9(L, ID, Lvl, Func, MsgID, A1,A2,A3,A4,A5,A6,A7,A8,A9)
#define Log_Write10(L, ID, Lvl, Func, MsgID, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10) Log_OutputStr10(L, ID, Lvl, Func, MsgID, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10)

#ifdef __cplusplus
}
#endif

#endif
