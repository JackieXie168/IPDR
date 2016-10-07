/**  * @file Common/Log.c
 *
 * Implementation of the IPDR logging service.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 */

#include <IPDR/Config.h>
const char Log_rcsid[] = "@(#) $Id: Log.c,v 1.10 2005/05/09 13:30:01 dmitryk Exp $" IPDR_COPYRIGHT_STR;

#include <Common/Log.h>

const char Log_h_rcsid[] = LOG_H_RCSID;


#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <Common/CommonUtil.h>
#include <Common/Console.h>
#include <Common/Transport.h>
#include <Common/MemoryManagement.h>
#include <Common/Console.h>

#include <IPDR/IPDR.h>

static const char* SyslogSeverityMapping[7];

static IPDR_Status Log_GetDebugLevelCommand(ConsoleSession *, FILE *, int, const char * const [], void *);
static IPDR_Status Log_SetDebugLevelCommand(ConsoleSession *, FILE *, int, const char * const [], void *);
static IPDR_Status Log_GetLoggerAddressCommand(ConsoleSession *, FILE *, int, const char * const [], void *);
static IPDR_Status Log_SetLoggerAddressCommand(ConsoleSession *, FILE *, int, const char * const [], void *);

static ConsoleCommand Log_ConsoleCommands[] = {
    { "get_debug_level", "get_debug_level [<module>]",
      Log_GetDebugLevelCommand },
    { "set_debug_level", "set_debug_level <module>|* <level>",
      Log_SetDebugLevelCommand },
    { "get_logger_addr", "get_logger_addr",
      Log_GetLoggerAddressCommand },
    { "set_logger_addr", "set_logger_addr <address> <port> [ <facility> [ <machineName> ] ]",
      Log_SetLoggerAddressCommand },
    { NULL, NULL, NULL }
};

#define LOG_PLATFORM (L->PS)
#define LOG_INITIAL_NUM_MODULES 16
#define LOG_MAX_MSG_SIZE (IPDR_MAX_ERROR_LENGTH+1)

static
IPDR_Status Log_OutputMessage2(Log *L, int repeats, const char* msg,
                                IPDR_ModuleID ID, IPDR_DebugLevel Level,
                                const char *Func);

/**
 *   Deletes resources associated with a log module
 *
 * @param PS platform services
 * @param Obj object to delete
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
void Module_Destructor(IPDR_PlatformServices *PS, void *Obj)
{
    Module *M = (Module *) Obj;
    strncpy(M->eyecatcher, "Dmodule", sizeof M->eyecatcher);
    Object_Release(PS, M->Name);
    strncpy(M->eyecatcher, "Xmodule", sizeof M->eyecatcher);
}

/**
 * Delete resources associated with a message.
 * @note not static --- used by MemoryManagement.c
 *
 * @param PS
 * @param Obj
 */
void Message_Destructor(IPDR_PlatformServices *PS, void *Obj)
{
}

/**
 *   Initialize the log service
 *
 * @param L logger
 * @param PS platform services
 * @param ComponentName name of logged component
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status Log_Initialize(Log *L,
                            IPDR_PlatformServices *PS,
                            const char *ComponentName)
{
    IPDR_Status Status;

    strncpy(L->eyecatcher, "LOG    ", sizeof L->eyecatcher);
    SyslogSeverityMapping[IPDR_LEVEL_MUTE]    = "---"; /* "Mute   "; */
    SyslogSeverityMapping[IPDR_LEVEL_FATAL]   = "Ftl"; /* "Fatal  "; */
    SyslogSeverityMapping[IPDR_LEVEL_ERROR]   = "Err"; /* "Error  "; */
    SyslogSeverityMapping[IPDR_LEVEL_WARNING] = "Wrn"; /* "Warning"; */
    SyslogSeverityMapping[IPDR_LEVEL_INFO]    = "Inf"; /* "Info   "; */
    SyslogSeverityMapping[IPDR_LEVEL_DEBUG]   = "Dbg"; /* "Debug  "; */
    SyslogSeverityMapping[IPDR_LEVEL_DEBUG2]  = "dbg"; /* "Debug2 "; */

    L->PS = PS;
    L->LoggerAddr = IPDR_DEFAULT_ADDRESS;
    L->LoggerPort = 0;
    L->Enabled = IPDR_FALSE;
    L->BypassCallback = IPDR_FALSE;
    L->ChangeCallback = NULL;
    L->ChangeCallbackArg = NULL;
    L->OutputCallback = NULL;
    L->OutputCallbackDetailed = NULL;
    L->OutputCallbackArg = NULL;
    L->saveI = -1;

    Status = DynamicArray_Create(PS, & (L->Modules),
                                 LOG_INITIAL_NUM_MODULES);
    if (FAILURE(Status))
        return Status;

    Status = Util_StrDup(PS, ComponentName, & (L->ComponentName));
    if (FAILURE(Status))
        return Status;

    Platform_CreateUDPSocket(PS, & L->LogFD);

    Status = Log_RegisterModule(L, IPDR_HOST_APP_MODULE_ID, "Host");
    if (FAILURE(Status))
        return Status;

    return Log_RegisterModule(L, IPDR_PLATFORM_MODULE_ID, "Platform");
}

/**
 *   Finalize the log service
 *
 * @param L logger
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status Log_Finalize(Log *L)
{
    int i;

    for (i = 0; i < L->saveI; i++) {
        if (L->save[i].repeats > 0) {
            (void)Log_OutputMessage2(L, L->save[i].repeats,
                                     L->save[i].str, L->save[i].moduleID,
                                     L->save[i].level, L->save[i].func);
        }
    }

    strncpy(L->eyecatcher, "Dlog   ", sizeof L->eyecatcher);
    Platform_CloseAndInvalidate(LOG_PLATFORM, &L->LogFD);
    DynamicArray_Delete(LOG_PLATFORM, & (L->Modules));
    Object_Release(LOG_PLATFORM, L->ComponentName);
    Object_Release(LOG_PLATFORM, L->LoggerMachineName);
    strncpy(L->eyecatcher, "Xlog   ", sizeof L->eyecatcher);

    return IPDR_OK;
}

/**
 *   Initialize the log service
 *
 * @param L logger
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status Log_Start(Log *L)
{
    return IPDR_OK;
}

/**
 *   Register a new module within the log service
 *
 * @param L logger
 * @param ID module id (unique within the system)
 * @param Name module name (unique within the system)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status Log_RegisterModule(Log *L, IPDR_ModuleID ID, const char *Name)
{
    Module *M;
    IPDR_Status Status;

    if (L->Modules.Length > ID &&
        L->Modules.Data[ID] != NULL)
        return IPDR_ERR;

    Status = Object_Allocate(LOG_PLATFORM, sizeof(Module),
                             Module_Destructor, (void **) & M);
    if (FAILURE(Status))
        return Status;
    strncpy(M->eyecatcher, "MODULE ", sizeof M->eyecatcher);

    Status = Util_StrDup(LOG_PLATFORM, Name, & (M->Name));
    if (FAILURE(Status)) {
        Object_Release(LOG_PLATFORM, M);
        return Status;
    }

    Status = DynamicArray_SetElementAt(LOG_PLATFORM, & (L->Modules),
                                       ID, M);
    if (FAILURE(Status)) {
        Object_Release(LOG_PLATFORM, M);
        return Status;
    }

#if 1 && defined(DEBUG_MODE)
    M->Level = IPDR_LEVEL_INFO;
#else
    M->Level = IPDR_LEVEL_WARNING;
#endif

    return IPDR_OK;
}

/**
 *   Set the current debug level of a specific module (only
 *   messages within lower or equal debug level will be
 *   outputted from the module)
 *
 * @param L logger
 * @param Name module name
 * @param Level module debug level
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status Log_SetModuleDebugLevel(Log *L, const char *Name, IPDR_DebugLevel Level)
{
    int i;
    Module *M;

    for (i = 0; i < L->Modules.Length; i++) {
        M = (Module *) L->Modules.Data[i];
        if (M == NULL || M->Name == NULL)
            continue;
        if (Name != NULL && ! strcmp(Name, M->Name))
            M->Level = Level;
    }

    if (L->ChangeCallback)
        L->ChangeCallback(L->ChangeCallbackArg);

    return IPDR_OK;
}

IPDR_Status Log_SetModuleMoreDebugLevel(Log *L, const char *Name, IPDR_DebugLevel Level)
{
    int i;
    Module *M;

    for (i = 0; i < L->Modules.Length; i++) {
        M = (Module *) L->Modules.Data[i];
        if (M == NULL || M->Name == NULL)
            continue;
        if (Name != NULL && ! strcmp(Name, M->Name) /*&& Level > M->Level*/)
            M->Level = Level;
    }

    if (L->ChangeCallback)
        L->ChangeCallback(L->ChangeCallbackArg);

    return IPDR_OK;
}

IPDR_Status Log_CheckLogLevelByName(Log *L, const char *Name, IPDR_DebugLevel Lvl)
{
    int i;
    Module *M;

    for (i = 0; i < L->Modules.Length; i++) {
        M = (Module *) L->Modules.Data[i];
        if (M == NULL || M->Name == NULL)
            continue;
        if (Name != NULL && ! strcmp(Name, M->Name)) {
            return Log_CheckLogLevel(L, i, Lvl);
        }
    }

    /* Unknown module name: use default */
    return Log_CheckLogLevel(L, IPDR_PLATFORM_MODULE_ID, Lvl);
}

/**
 *   Enable logging.
 *
 * @param L log
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status Log_Enable(Log *L)
{
    L->Enabled = IPDR_TRUE;
    return IPDR_OK;
}

/**
 *   Disable logging.
 *
 * @param L log
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status Log_Disable(Log *L)
{
    L->Enabled = IPDR_FALSE;
    return IPDR_OK;
}

/**
 *   The function outputs a log message to the IPDR client
 *   (transmitter) log.
 *
 * @param L logger
 * @param ModuleID module identifier (either
 *              IPDR_PLATFORM_MODULE_ID or
 *              IPDR_HOST_MODULE_ID)
 * @param Level message debug level
 * @param Func function originating the message
 * @param Str message (can be a printf style format string)
 * @param Args additional arguments in case of a formatted msg
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status Log_VOutputStr(Log *L, IPDR_ModuleID ModuleID, IPDR_DebugLevel Level,
                            const char *Func, const char *Str, va_list Args)
{
    if (! Func) { Func = ""; }
    if (! Str) { Str = ""; }
#if defined(VXWORKS) || defined(_VXWORKS)
    /* XXX #error VXWORKS problem (bug?) with vsprintf */
    vsprintf (L->msgStr, Str, Args);
#elif _MSC_VER
    _vsnprintf (L->msgStr, LOG_MAX_MSG_SIZE, Str, Args);

    L->msgStr[LOG_MAX_MSG_SIZE-1] = '\0';
#else
    vsnprintf (L->msgStr, LOG_MAX_MSG_SIZE, Str, Args);
    L->msgStr[LOG_MAX_MSG_SIZE-1] = '\0';
#endif

    return Log_OutputMessage(L, L->msgStr, ModuleID, Level, Func);
}

IPDR_Status Log_VOutputStrByName(Log *L, const char* ModuleName, IPDR_DebugLevel Level,
                                  const char *Func, const char *Str, va_list Args)
{
    int i;
    Module *M;

    for (i = 0; i < L->Modules.Length; i++) {
        M = (Module *) L->Modules.Data[i];
        if (M == NULL || M->Name == NULL)
            continue;
        if (ModuleName != NULL && ! strcmp(ModuleName, M->Name)) {
			if(Log_ShouldWrite(L, i, Level))
            	return Log_VOutputStr(L, i, Level, Func, Str, Args);
			return IPDR_OK;
        }
    }

    if(Log_ShouldWrite(L, IPDR_PLATFORM_MODULE_ID, Level))
    	return Log_VOutputStr(L, IPDR_PLATFORM_MODULE_ID, Level, Func, Str, Args);
	return IPDR_OK;
}


/**
 *   Register log related console commands
 *
 * @param L logger
 * @param C console
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status Log_RegisterConsoleCommands(Log *L, struct IPDR_tag_Console *C)
{
    return Console_RegisterModule(C, "log", L, Log_ConsoleCommands);
}

/**
 *   Console function for getting module debug level
 *
 * @param S console session
 * @param Out output stream
 * @param Argc number of arguments
 * @param Argv arguments
 * @param Arg context argument (log)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static
IPDR_Status Log_GetDebugLevelCommand(ConsoleSession *S, FILE *Out, int Argc,
                                      const char * const Argv[], void *Arg)
{
    Module *M;
    int i;
    Log *L = (Log *) Arg;

    if (Argc == 0) {
        for (i = 0; i < L->Modules.Length; i++) {
            M = (Module *) L->Modules.Data[i];
            if (M == NULL || M->Name == NULL)
                continue;
            Console_Println(S, Out, "%-10s: %d", M->Name, M->Level);
        }
    } else {
        for (i = 0; i < L->Modules.Length; i++) {
            M = (Module *) L->Modules.Data[i];
            if (M == NULL || M->Name == NULL)
                continue;
            if (! strcmp(M->Name, Argv[0])) {
                Console_Println(S, Out, "%-10s: %d", M->Name, M->Level);
                break ;
            }
        }
    }

    return IPDR_OK;
}

/**
 *   Console function for setting module debug level
 *
 * @param S console session
 * @param Out output stream
 * @param Argc number of arguments
 * @param Argv arguments
 * @param Arg context argument (log)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static
IPDR_Status Log_SetDebugLevelCommand(ConsoleSession *S, FILE *Out, int Argc,
                                      const char * const Argv[], void *Arg)
{
    Log *L = (Log *) Arg;

    if (Argc != 2) {
        Console_Println(S, Out, "set_debug_level: Invalid number of arguments.");
        return IPDR_OK;
    }

    if (0 == strcmp(Argv[0], "*")) {
        int i;
        for (i = 0; i < L->Modules.Length; i++) {
            const Module * M = (Module *) L->Modules.Data[i];
            if (M == NULL || M->Name == NULL)
                continue;
            (void) Log_SetModuleDebugLevel(L, M->Name, atoi(Argv[1]));
        }
    } else {
        return Log_SetModuleDebugLevel(L, Argv[0], atoi(Argv[1]));
    }

    return IPDR_OK;
}

/**
 *   Console function for getting the logger application address
 *
 * @param S console session
 * @param Out output stream
 * @param Argc number of arguments
 * @param Argv arguments
 * @param Arg context argument (log)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static
IPDR_Status Log_GetLoggerAddressCommand(ConsoleSession *S, FILE *Out, int Argc,
                                         const char * const Argv[], void *Arg)
{
    Log *L = (Log *) Arg;
    TL_PABuf paBuf;

    if (Argc != 0) {
        Console_Println(S, Out, "get_logger_addr: Invalid number of arguments.");
        return IPDR_OK;
    }

    Console_Println(S, Out, "Logger Address=%.99s facility=%d machine=%.99s",
                    TL_PrintAddressAndPort(L->LoggerAddr, L->LoggerPort, paBuf),
                    L->LoggerFacility, L->LoggerMachineName);

    return IPDR_OK;
}

/**
 *   Console function for setting the logger application address
 *
 * @param S console session
 * @param Out output stream
 * @param Argc number of arguments
 * @param Argv arguments
 * @param Arg context argument (log)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static
IPDR_Status Log_SetLoggerAddressCommand(ConsoleSession *S, FILE *Out, int Argc,
                                         const char * const Argv[], void *Arg)
{
    Log *L = (Log *) Arg;
    int Facility;
    const char* MachineName;
    IPDR_Status result;
    TL_PABuf paBuf;

    if (Argc < 2) {
        Console_Println(S, Out, "set_logger_addr: Invalid number of arguments.");
        return IPDR_OK;
    }
    if (Argc >= 3) {
        Facility = atoi(Argv[2]);
    } else {
        Facility = 1; /* local */
    }
    if (Argc >= 4) {
        MachineName = Argv[3];
    } else {
        MachineName = NULL;
    }

    result= Log_SetLoggerAddress(L, TL_ParseAddress(Argv[0]), atoi(Argv[1]), Facility, MachineName);

    if (SUCCESS(result)) {
        Console_Println(S, Out, "Logger Address=%.99s facility=%d machine=%.99s",
                        TL_PrintAddressAndPort(L->LoggerAddr, L->LoggerPort, paBuf),
                        L->LoggerFacility, L->LoggerMachineName);
    }

    return result;
}

/**
 *  Set the Logger IP Address.
 *
 * @param L log
 * @param Addr The logger IP address
 * @param Port The logger output port number.
 * @param Facility The "facility", as defined for SYSLOG.
 * @param MachineName The client's (trasnmitter's) identification, as defined for SYSLOG.
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status Log_SetLoggerAddress(Log *L,
                                  IPDR_IPAddress Addr,
                                  unsigned short Port,
                                  unsigned int Facility,
                                  const char* MachineName)
{
    IPDR_Status Status;
    char hostnameBuf[257];

    L->LoggerAddr = Addr;
    L->LoggerPort = Port;
    L->LoggerFacility = Facility;
    if (! MachineName || MachineName[0] == '\0') {
        /* Try to guess the machine's IP address. ' */
        /* MachineName = TL_PrintAddress(TCPTransport.LocalAddr, paBuf); */
        Platform_GetHostName(LOG_PLATFORM, hostnameBuf, sizeof hostnameBuf);
        MachineName = hostnameBuf;
    }
    Status = Util_StrDup(L->PS, MachineName, & (L->LoggerMachineName));
    if (FAILURE(Status))
        return Status;

    if (L->ChangeCallback)
        L->ChangeCallback(L->ChangeCallbackArg);

    return IPDR_OK;
}

/**
 *   The function checks whether a specific module within the
 *   client (transmitter) outputs log messages at the given level.
 *
 * @param L log
 * @param ModuleID module identifier (either
 *              #IPDR_PLATFORM_MODULE_ID or
 *              #IPDR_HOST_MODULE_ID)
 * @param Lvl the level to test against
 *
 * @return 1 if the module is outputing log messages at the
 *   requested level, 0 otherwise
 */
IPDR_Status Log_CheckLogLevel(Log *L, IPDR_ModuleID ModuleID,
                      IPDR_DebugLevel Lvl)
{
    if (L && L->Enabled &&
        ((Module *) L->Modules.Data[ModuleID])->Level >= Lvl)
        return IPDR_ERR;
    return IPDR_OK;
}

IPDR_Status Log_LogPlatformError(Log *L, IPDR_ModuleID ModuleID,
                                  const char *Func, const char *PlatformFunc)
{
    Platform_GetLastErrorMessage(LOG_PLATFORM, L->msgStrErr);
    /* XXX should signal error event? */
    Log_Write2(L, ModuleID, IPDR_LEVEL_ERROR, Func,
               MSG_LOG_PLATFORM_ERR,
               PlatformFunc, L->msgStrErr);
    return IPDR_OK;
}

IPDR_Status Log_SetChangeCallback(Log *L, Log_SettingsChangedCallback CB,
                                   void *CallbackArg)
{
    L->ChangeCallback = CB;
    L->ChangeCallbackArg = CallbackArg;
    return IPDR_OK;
}

IPDR_Status Log_SetOutputCallback(Log *L, Log_OutputCallback CB, void *Arg)
{
    L->OutputCallback = CB;
    L->OutputCallbackDetailed = NULL;
    L->OutputCallbackArg = Arg;
    return IPDR_OK;
}

IPDR_Status Log_SetOutputCallbackDetailed(Log *L, Log_OutputCallbackDetailed CB, void *Arg)
{
    L->OutputCallback = NULL;
    L->OutputCallbackDetailed = CB;
    L->OutputCallbackArg = Arg;
    return IPDR_OK;
}

IPDR_Status Log_OutputMessage(Log *L,
                               const char *Msg,
                               IPDR_ModuleID ID,
                               IPDR_DebugLevel Level,
                               const char *Func)
{
    IPDR_Time c;
    int i;
    Platform_GetTime(LOG_PLATFORM, &c);

    for (i = 0; i < L->saveI; i++) {
        if (L->save[i].level == Level &&
            L->save[i].moduleID == ID &&
            0 == strncmp(Msg, L->save[i].str, sizeof L->save[i].str) &&
            0 == strcmp(Func, L->save[i].func)) {
            IPDR_Status rc = IPDR_OK;
            L->save[i].repeats++;
            if (L->save[i].repeats >= LOG_MAX_REPEATS ||
                c - L->save[i].firstTime >= LOG_MAX_REPEAT_INTVL) {
                rc = Log_OutputMessage2(L, L->save[i].repeats, L->save[i].str,
                                        L->save[i].moduleID, L->save[i].level, L->save[i].func);
                for (; i < L->saveI; i++) {
                    L->save[i] = L->save[i+1];
                }
                L->saveI--;
            }

            return rc;
        }
    }

    if (L->saveI >= (LOG_SAVE_STR_NUM - 1)) {
        if (L->save[0].repeats > 0) {
            (void)Log_OutputMessage2(L, L->save[0].repeats, L->save[0].str,
                                     L->save[0].moduleID, L->save[0].level,
                                     L->save[0].func);
        }
        for (i = 0; i < (LOG_SAVE_STR_NUM-1); i++) {
            L->save[i] = L->save[i+1];
        }
        L->saveI--;
    }
    L->saveI++;
    L->save[L->saveI].repeats = 0;
    L->save[L->saveI].firstTime = c;
    L->save[L->saveI].level = Level;
    L->save[L->saveI].moduleID = ID;
    L->save[L->saveI].func = Func;
    strncpy(L->save[L->saveI].str, Msg, sizeof L->save[0].str);

    return Log_OutputMessage2(L, 0, Msg, ID, Level, Func);
}

/* helper */
static
IPDR_Status Log_OutputMessage2(Log *L, int repeats, const char* msg,
                                IPDR_ModuleID ID,
                                IPDR_DebugLevel Level,
                                const char *Func)

{
    int notYetOutput = 1;
    Module *M = (Module *) L->Modules.Data[ID];

    /* The old code mapped FATAL=>2, ... DEBUG2=>7, calculated by:
       #define SYSLOG_FACILITY 184
       int SyslogPriority = SyslogSeveretyMapping[Level] | SYSLOG_FACILITY
       and printed this priority as the first field.
       This has been changed to output a string.
    */

    if ((L->LoggerAddr != 0 && L->LoggerPort != 0) ||
        (L->OutputCallback && ! L->BypassCallback)) {
/*
        Need to format in standard RFC 3164 form ... the "module" must be no more than 32 characters, terminated by ":" (not part of the 32)  ...
            note date format (with extra blank) and that it is LOCAL time
            ... total packet must be <= 1024 bytes
            ... note addition of "IPDR_" to beginning of "module"
            ... the "priority" item at the beginning (in angle brackets) is
            ... facility * 8 + severity, so facility=19 (local3) error = 155
          <155> Jul  1 17:23:47 192.168.8.88 IPDR transmitter: Exporter_TriggerEvent: Connected(START): Server=10.0.0.213, Port=15001, outstanding=0, unACKed=100, Session=101 [session1] (NE/one)Connected: 10.0.0.40
*/

        if (0 && repeats > 0) {
            sprintf(L->msgStrWithHdr, "%.7s <%.40s> [%-8.8s] %.99s: (repeated %d times) %.199s\n",
                    SyslogSeverityMapping[Level],
                    L->ComponentName, M->Name, Func, repeats, msg);
        } else {
            if (1) {
                int priority = L->LoggerFacility * 8;
                char timeStrBuf[IPDR_TIME_FORMAT_LENGTH];
                switch (Level) { /* translate to RFC 3164 values */
                case IPDR_LEVEL_FATAL:   priority += 0; break; /* emergency     */
                /* case xxx:              priority += 1; break; |* alert         */
                /* case xxx:              priority += 2; break; |* critical      */
                case IPDR_LEVEL_ERROR:   priority += 3; break; /* error         */
                case IPDR_LEVEL_WARNING: priority += 4; break; /* warning       */
                default:                  priority += 5; break; /* notice        */
                case IPDR_LEVEL_INFO:    priority += 6; break; /* informational */
                case IPDR_LEVEL_DEBUG:
                case IPDR_LEVEL_DEBUG2:  priority += 7; break; /* debug         */
                }
                if (! L->LoggerMachineName || L->LoggerMachineName[0] == '\0') {
                    /* see Log_SetLoggerAddress for similar code. */
                    char hostnameBuf[257];
                    Platform_GetHostName(LOG_PLATFORM, hostnameBuf, sizeof hostnameBuf);
                    Util_StrDup(LOG_PLATFORM, hostnameBuf, & (L->LoggerMachineName));
                }
                sprintf(L->msgStrWithHdr, "<%d> %.99s %.255s %.32s: %.99s[%.99s]: %.199s",
                        priority, Platform_FormatTimeSyslogNow(LOG_PLATFORM, timeStrBuf),
                        L->LoggerMachineName, L->ComponentName, Func, M->Name, msg);
            } else { /* older format, not used */
                sprintf(L->msgStrWithHdr, "%.7s <%.40s> [%-8.8s] %.99s: %.199s\n",
                        SyslogSeverityMapping[Level],
                        L->ComponentName, M->Name, Func, msg);
            }
        }

        if (L->LoggerAddr != 0 && L->LoggerPort != 0 &&
            SUCCESS(Platform_SendTo(LOG_PLATFORM, L->LogFD, L->msgStrWithHdr,
                                        strlen(L->msgStrWithHdr),
                                        L->LoggerAddr, L->LoggerPort))) {
            /* output to syslog via UDP is in addition to any output
             * to callback or print */
            if (0) {
                notYetOutput = 0;
            }
        }

        if (L->OutputCallback && ! L->BypassCallback) {
            L->OutputCallback(L->msgStrWithHdr, L->OutputCallbackArg);
            notYetOutput = 0;
        }
    }

    if (L->OutputCallbackDetailed && ! L->BypassCallback) {
        if (repeats > 0) {
            sprintf(L->msgStrWithHdr, "(repeated %d times) %.199s",
                    repeats, msg);
            L->OutputCallbackDetailed(Level, SyslogSeverityMapping[Level],
                                      L->ComponentName, M->Name, Func,
                                      L->msgStrWithHdr,
                                      L->OutputCallbackArg);
        } else {
            L->OutputCallbackDetailed(Level, SyslogSeverityMapping[Level],
                                      L->ComponentName, M->Name, Func,
                                      msg,
                                      L->OutputCallbackArg);
        }
        notYetOutput = 0;
    }

#ifdef STD_FILE_OPERATIONS_SUPPORT
    if (notYetOutput) {
        char timeStrBuf[IPDR_TIME_FORMAT_LENGTH + 1];
        if (repeats > 0) {
            fprintf(stderr, "%.30sZ %.7s <%.40s> [%-8.8s] %.99s: (repeated %d times) %.199s\n",
                    Platform_FormatTimeGMTNow(LOG_PLATFORM, timeStrBuf),
                    SyslogSeverityMapping[Level],
                    L->ComponentName, M->Name, Func, repeats, msg);
        } else {
            fprintf(stderr, "%.30sZ %.7s <%.40s> [%-8.8s] %.99s: %.199s\n",
                    Platform_FormatTimeGMTNow(LOG_PLATFORM, timeStrBuf),
                    SyslogSeverityMapping[Level],
                    L->ComponentName, M->Name, Func, msg);
        }
        fflush(stderr);
    }
#endif
    return IPDR_OK;
}

#ifdef TEST_LOG_WITH_MACRO
#else
int Log_ShouldWriteD(Log *L, int ID, int Lvl)
{
	Module *M = NULL;
	if(!L)
		return 0 ;
	M = (Module *)L->Modules.Data[ID];
	if(!M)
		return 0 ;
    return (L->Enabled && M->Level >= Lvl);
}
#endif
