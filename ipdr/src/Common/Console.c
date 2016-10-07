/** @file Common/Console.c
 *
 * Implementation of the IPDR console.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: Console.c,v 1.11 2005/04/21 11:31:22 dmitryk Exp $
 *
 * The file contains the implementation of the IPDR console (default,
 * port is set by #DEFAULT_console_consolePort).  The console accepts
 * TCP connections, interprets text commands and invokes the
 * corresponding functions.
 *
 * @see Exporter/ExporterConsole.c
 */

#include <IPDR/Config.h>
const char Console_rcsid[] = "@(#) $Id: Console.c,v 1.11 2005/04/21 11:31:22 dmitryk Exp $" IPDR_COPYRIGHT_STR;

#include <Common/Console.h>

const char Console_h_rcsid[] = CONSOLE_H_RCSID;

#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include <Common/MemoryManagement.h>
#include <Common/CommonUtil.h>
#include <Common/Transport.h>

#define CONSOLE_PLATFORM (C->PS)
#define CONSOLE_LOG      (C->Logger)
#define LOGIN_MESSAGE    ("Welcome to IPDR console.\r\n" PASSWORD_PROMPT)
#define PROMPT           "IPDR>> "
#define PASSWORD_PROMPT  "password: "
#define MAX_LINE_LEN     2048
#define MAX_COMMAND_ARGS 64
#define COMMENT_DELIMITER '#'
#define CMD_DELIMITER     ';'

static IPDR_Status Console_HelpCommand(ConsoleSession *, FILE *, int, const char * const [], void *);
static IPDR_Status Console_HelpModuleCommand(ConsoleSession *, FILE *, int, const char * const [], void *);
static IPDR_Status Console_HelpCommandCommand(ConsoleSession *, FILE *, int, const char * const [], void *);
static IPDR_Status Console_EchoCommand(ConsoleSession *, FILE *, int, const char * const [], void *);
#ifdef ENABLE_SIMULATOR
static IPDR_Status Console_LoadFileCommand(ConsoleSession *, FILE *, int, const char * const [], void *);
#endif

static ConsoleCommand ConsoleCommands[] = {
    {"help", "help", Console_HelpCommand },
    {"helpm", "helpm <module>", Console_HelpModuleCommand },
    {"helpc", "helpc <command>", Console_HelpCommandCommand },
    {"echo", "echo", Console_EchoCommand },
#ifdef ENABLE_SIMULATOR
#ifdef STD_FILE_OPERATIONS_SUPPORT
    {"load", "load <filename>", Console_LoadFileCommand },
#endif
#endif
    {"exit", "exit", NULL }, /* special convention: NULL for exit */
    {"stop", "stop", NULL }, /* special convention: NULL for exit */
    {NULL, NULL, NULL}
};

/**
 *   Deletes resources associated with a console module
 *   structure
 *   @note not static --- used by MemoryManagement.c
 *
 * @param PS platform services
 * @param Obj object to delete
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */

void ConsoleModule_Destructor(IPDR_PlatformServices *PS, void *Obj)
{
    ConsoleModule *M = (ConsoleModule *) Obj;
    strncpy(M->eyecatcher, "Dcmodul", sizeof M->eyecatcher);
    Object_Release(PS, M->Name);
    strncpy(M->eyecatcher, "Xcmodul", sizeof M->eyecatcher);
}

/**
 *   Deletes resources associated with a console session
 *   structure
 *   @note not static --- used by MemoryManagement.c
 *
 * @param PS platform services
 * @param Obj object to delete
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
void ConsoleSession_Destructor(IPDR_PlatformServices *PS, void *Obj)
{
    ConsoleSession *S = (ConsoleSession *) Obj;
    strncpy(S->eyecatcher, "Dcsessn", sizeof S->eyecatcher);
    Platform_UnregisterSocketAndClear(PS, &S->SocketEventID);
    Platform_CloseAndInvalidate(PS, &S->FD);
    strncpy(S->eyecatcher, "Xcsessn", sizeof S->eyecatcher);
}

/**
 *   Find a console command given its name
 *
 * @param C console
 * @param Command command name
 * @param CM module containing the command (output)
 *
 * @return Command structure or NULL if command not found
 */
static
ConsoleCommand *Console_FindCommandByName(Console *C, const char *Command,
                                          ConsoleModule **CM)
{
    int i;
    ConsoleModule *M;
    ConsoleCommand *CC;

    for (i = 0; i < C->Modules.Length; i++) {
        M = (ConsoleModule *) C->Modules.Data[i];
        CC = M->Commands;
        while (CC->Name != NULL) {
            if (! strcmp(CC->Name, Command)) {
                *CM = M;
                return CC;
            }
            CC++;
        }
    }
    *CM = NULL;
    return NULL;
}

/**
 *   Find a console module given its name
 *
 * @param C console
 * @param ModuleName module name
 *
 * @return Module structure or NULL if module not found
 */
static
ConsoleModule *Console_FindModuleByName(Console *C, const char *ModuleName)
{
    ConsoleModule *M;
    int i;

    for (i = 0; i < C->Modules.Length; i++) {
        M = (ConsoleModule *) C->Modules.Data[i];
        if (! strcmp(M->Name, ModuleName))
            return M;
    }

    return NULL;
}

/**
 *   Activate a command from the console
 *
 * @param C console
 * @param S session (may be null)
 * @param Out output file/socket (used if S is NULL)
 * @param Cmd command line
 * @param ParsedCommand resulting parsed command
 * @param ParsedArgc resulting number of parsed arguments
 * @param ParsedArgs the parsed arguments
 * @param RemainingCmd anything after the command (e.g., after "#", ";")
 */
IPDR_Status Console_ActivateCommand(Console *C, ConsoleSession *S,
                                     FILE *Out, char *Cmd,
                                     const char **ParsedCommand,
                                     int *ParsedArgc,
                                     const char ***ParsedArgs,
                                     char **RemainingCmd,
									 char *CmdRes,
									 int CmdResLen)
{
    char *Command, *Ptr;
    static const char *Args[MAX_COMMAND_ARGS]; /* "static" so we can return it (being a bit lazy) */
    int Argc = 0;
    ConsoleCommand *CC;
    ConsoleModule *M;
    IPDR_Status Status = IPDR_OK;
    int CmdDelimSeen = 0;
	char *CmdResults = CmdRes ;
	int CmdResultsLen = CmdResLen ;

    if (ParsedCommand) { *ParsedCommand = ""; }
    if (ParsedArgc) { *ParsedArgc = 0; }
    if (ParsedArgs) { *ParsedArgs = Args; }
    if (RemainingCmd) { *RemainingCmd = 0; }
	if (CmdResults && CmdResultsLen > 0) { CmdResults[0] = '\0'; }

    if (S && S->Echo) {
        Console_Println(S, Out, "Command: %.199s", Cmd);
    }

    if (! Cmd)
        return IPDR_OK;

    /* Skip initial blanks */
    for (; isspace((unsigned)Cmd[0]); Cmd++) {
        if (Cmd[0] == COMMENT_DELIMITER) {
/*          Console_Println(S, Out, "Skipping: `%.99s'\n", Cmd); */
            return IPDR_OK;
        }
    }

    /* it would be nice to just do Ptr = strch(Cmd, ' ') but we need
       to take care of all the space characters, plus the possibility
       of a comment or end-of-command characater. */

    for (Ptr = Cmd; *Ptr != '\0' && ! isspace((unsigned)*Ptr) && *Ptr != COMMENT_DELIMITER && *Ptr != CMD_DELIMITER; Ptr++) { }

    if (*Ptr == CMD_DELIMITER) {
        CmdDelimSeen = 1;
    }

    if (*Ptr != '\0') {
        *Ptr = '\0';
        Ptr++;
    }

    Command = Cmd;

    if (ParsedCommand) { *ParsedCommand = Command; }

    if (! strlen(Command))
        return IPDR_OK;

    /* parse the parameters */
    Argc = 0;
    for (; ! CmdDelimSeen && Ptr != '\0' && isspace((unsigned)*Ptr); Ptr++) { }

    while (! CmdDelimSeen && *Ptr != '\0' && *Ptr != COMMENT_DELIMITER && *Ptr != CMD_DELIMITER) {

        if (Argc >= MAX_COMMAND_ARGS)
            break;

        if (*Ptr == '"') {
            Ptr++;
            Args[Argc++] = Ptr;
            for (; *Ptr != '"' && *Ptr != '\0'; Ptr++) { }
        } else if (*Ptr == '\'') {
            Ptr++;
            Args[Argc++] = Ptr;
            for (; *Ptr != '\'' && *Ptr != '\0'; Ptr++) { }
        } else {
            Args[Argc++] = Ptr;
            for (; ! isspace((unsigned)*Ptr) && *Ptr != '\0' && *Ptr != COMMENT_DELIMITER && *Ptr != CMD_DELIMITER; Ptr++) { }
            if (*Ptr == CMD_DELIMITER) {
                CmdDelimSeen = 1;
            }
        }
        if (*Ptr == '\0') {
            break;
        }
        *Ptr = '\0';
        Ptr++;
        for (; isspace((unsigned)*Ptr) && *Ptr != '\0' && *Ptr != COMMENT_DELIMITER && *Ptr != CMD_DELIMITER; Ptr++) { }
    }

    if (*Ptr == CMD_DELIMITER) {
        CmdDelimSeen = 1;
        Ptr++;
    }

    if (ParsedArgc) { *ParsedArgc = Argc; }
    if (ParsedArgs) { *ParsedArgs = Args; }

	if( CmdResults  && CmdResultsLen > 0)
	{
		strncpy(CmdResults, Command, CmdResultsLen);
		CmdResults[CmdResultsLen-1] = '\0';
		CmdResults = strlen(CmdResults) + CmdResults ;
		CmdResultsLen -=  strlen(CmdResults);
		
		strncpy(CmdResults, "=", CmdResultsLen);
		CmdResults[CmdResultsLen-1] = '\0';
		CmdResults = strlen(CmdResults) + CmdResults ;
		CmdResultsLen -=  strlen(CmdResults);
	}
	
    CC = Console_FindCommandByName(C, Command, & M);
    if (CC == NULL) {
        if (RemainingCmd) { *RemainingCmd = Ptr; }
		if( CmdResults  && CmdResultsLen > 0)
		{
			strncpy(CmdResults, "ERROR", CmdResultsLen);
			CmdResults[CmdResultsLen-1] = '\0';
			CmdResults = strlen(CmdResults) + CmdResults ;
			CmdResultsLen -=  strlen(CmdResults);
		
			strncpy(CmdResults, ";", CmdResultsLen);
			CmdResults[CmdResultsLen-1] = '\0';
			CmdResults = strlen(CmdResults) + CmdResults ;
			CmdResultsLen -=  strlen(CmdResults);
		}
        
		return IPDR_CMD_NOT_FOUND;
    }

    if (CC->CommandFunc == NULL) { /* It's an "exit" type command */
		if( CmdResults  && CmdResultsLen > 0)
		{
			strncpy(CmdResults, "ERROR", CmdResultsLen);
			CmdResults[CmdResultsLen-1] = '\0';
			CmdResults = strlen(CmdResults) + CmdResults ;
			CmdResultsLen -=  strlen(CmdResults);
	
			strncpy(CmdResults, ";", CmdResultsLen);
			CmdResults[CmdResultsLen-1] = '\0';
			CmdResults = strlen(CmdResults) + CmdResults ;
			CmdResultsLen -=  strlen(CmdResults);
		}
        return IPDR_CMD_EXIT;
    } else {
        Status = CC->CommandFunc(S, Out, Argc, Args, M->ContextArg);
        switch (Status) {
        case IPDR_OK:
            if (RemainingCmd) { *RemainingCmd = Ptr; }
            break;
        case IPDR_CMD_HELP:
            if (RemainingCmd) { *RemainingCmd = Ptr; }
            return Status;
            break;
        default:
			if( CmdResults  && CmdResultsLen > 0)
			{
				strncpy(CmdResults, FAILURE(Status)?"ERROR":"OK", CmdResultsLen);
				CmdResults[CmdResultsLen-1] = '\0';
				CmdResults = strlen(CmdResults) + CmdResults ;
				CmdResultsLen -=  strlen(CmdResults);
		
				strncpy(CmdResults, ";", CmdResultsLen);
				CmdResults[CmdResultsLen-1] = '\0';
				CmdResults = strlen(CmdResults) + CmdResults ;
				CmdResultsLen -=  strlen(CmdResults);
			}
            return Console_Println(S, Out, "Error during command invocation. Status = %.50s, cmd: `%.199s'", IPDR_StatusMsg(NULL, Status), Cmd);
        }
    }

    if (CmdDelimSeen && Ptr[0] != '\0') {
		if( CmdResults  && CmdResultsLen > 0)
		{
			strncpy(CmdResults, FAILURE(Status)?"ERROR":"OK", CmdResultsLen);
			CmdResults[CmdResultsLen-1] = '\0';
			CmdResults = strlen(CmdResults) + CmdResults ;
			CmdResultsLen -=  strlen(CmdResults);
	
			strncpy(CmdResults, ";", CmdResultsLen);
			CmdResults[CmdResultsLen-1] = '\0';
			CmdResults = strlen(CmdResults) + CmdResults ;
			CmdResultsLen -=  strlen(CmdResults);
		}
        return Console_ActivateCommand(C, S, Out, Ptr, ParsedCommand, ParsedArgc, ParsedArgs, RemainingCmd, CmdResults, CmdResultsLen);
    } else {
        if (RemainingCmd) { *RemainingCmd = Ptr; }
		if( CmdResults  && CmdResultsLen > 0)
		{
			strncpy(CmdResults, FAILURE(Status)?"ERROR":"OK", CmdResultsLen);
			CmdResults[CmdResultsLen-1] = '\0';
			CmdResults = strlen(CmdResults) + CmdResults ;
			CmdResultsLen -=  strlen(CmdResults);
	
			strncpy(CmdResults, ";", CmdResultsLen);
			CmdResults[CmdResultsLen-1] = '\0';
			CmdResults = strlen(CmdResults) + CmdResults ;
			CmdResultsLen -=  strlen(CmdResults);
		}
        return IPDR_OK;
    }
}


/**
 *   Function called whenever data arrives on a connection
 *   to the console.
 *
 * @param FD socket file descriptor
 * @param Arg context arg (ConsoleSession structure)
 *
 */
static
void Console_ConnectionCallback(IPDR_SocketHandle FD, void *Arg)
{
    ConsoleSession *CS;
    Console *C;
    char c;
    unsigned int RB, i;
    IPDR_Status Status;
    FILE *Out = NULL; /* We're in a socket callback, so we use the console session and don't output to a file. */

    CS = (ConsoleSession *) Arg;
    C = CS->ConsolePtr;

    Status = Platform_Recv(CONSOLE_PLATFORM, FD, & c, sizeof(char), -1, & RB);
    if (FAILURE(Status)) {
        for (i = 0; i < C->Sessions.Length; i++) {
            if (CS == C->Sessions.Data[i]) {
                DynamicArray_RemoveAndShift(CONSOLE_PLATFORM, & (C->Sessions), i);
                return ;
            }
        }
        return;
    }

    if (RB == 0)
        return ;

    if (CS->CommandIndex < MAX_COMMAND_LINE_LEN - 1) {

        /*Ignoring the 'delete' and the 'backspace' char*/
        if (c == '\r' == c == '\b'|| c == 127)
            return ;
        /*Ignoring the arrows chars*/
        else if (c == 0x1b)
            CS->SkipControlChar = 1;
        else if (CS->SkipControlChar == 1 && c == 0x5b)
            CS->SkipControlChar = 2;
        else if (c >= 0x41 && c <= 0x44 && CS->SkipControlChar == 2)
            return ;
        else if (c != '\n' && c != '\r') {
            CS->CommandBuffer[CS->CommandIndex] = c;
            CS->CommandIndex++;
            CS->SkipControlChar = 0;
        }
    }

    /* check if we reached an end of line */

    if (c == '\n') {

        CS->CommandBuffer[CS->CommandIndex] = '\0';

#if 0
        /* This code isn't quite right: it disallows a password of "exit"
           or "stop", but it's just a debugging interface anyway. */
        if (0 == strcmp(CS->CommandBuffer, "exit") ||
            0 == strcmp(CS->CommandBuffer, "stop") ||
            0 == strcmp(CS->CommandBuffer, "quit") ||
            CS->CommandBuffer[0] == 4 /* ^D */  ) {
            for (i = 0; i < C->Sessions.Length; i++) {
                if (CS == C->Sessions.Data[i]) {
                    DynamicArray_RemoveAndShift(CONSOLE_PLATFORM, & (C->Sessions), i);
#ifdef ENABLE_SIMULATOR
                    if (0 == strcmp(CS->CommandBuffer, "stop")) {
                        Platform_Stop(C->PS);
                    }
#endif
                    return ;
                }
            }
        }
#endif

        if (! CS->LoggedIn) {
            CS->CommandIndex = 0;
            if (0 == strcmp(CS->CommandBuffer, C->Password)) {
                CS->LoggedIn = IPDR_TRUE;
                Console_Println(CS, Out, "Logged in.");
                Console_Print(CS, Out, PROMPT);
                return ;
            } else {
                Console_Println(CS, Out, "Wrong password (Note: password is case sensitive).");
                Console_Print(CS, Out, PASSWORD_PROMPT);
                return ;
            }
        }

        if (strlen(CS->CommandBuffer) > 0) {
            const char* Command;
            TL_PABuf paBuf;
            IPDR_Status rc =
                Console_ActivateCommand(C, CS, Out, CS->CommandBuffer, &Command, NULL, NULL, NULL, NULL, 0);
            switch (rc) {
            case IPDR_CMD_EXIT:
                Console_Println(CS, Out, "Exiting.", Command);
                Log_Write1(CONSOLE_LOG, IPDR_PLATFORM_MODULE_ID, IPDR_LEVEL_INFO,
                           "ConsoleSession_Destructor",
                           "Console disconnection from %.99s",
                           TL_PrintAddressAndPort(CS->RemoteAddr, CS->RemotePort, paBuf));
                for (i = 0; i < C->Sessions.Length; i++) {
                    if (CS == C->Sessions.Data[i]) {
                        DynamicArray_RemoveAndShift(CONSOLE_PLATFORM, & (C->Sessions), i);
                        return ;
                    }
                }
                break;
            case IPDR_CMD_NOT_FOUND:
                Console_Println(CS, Out, "Unable to find command: `%.199s'.", Command);
                break;
            default:
                break;
            }
        }

        Console_Print(CS, Out, PROMPT);

        CS->CommandIndex = 0;
        CS->SkipControlChar = 0;

        return ;
    }
}

/**
 *   Function called whenever a new connection attempt is
 *   made
 *
 * @param FD socket file descriptor
 * @param Arg context arg (ConsoleSession structure)
 *
 */
static
void Console_AcceptCallback(IPDR_SocketHandle FD, void *Arg)
{
    unsigned short RemotePort;
    IPDR_IPAddress RemoteAddr;
    IPDR_SocketHandle NewSocketFD;
    IPDR_Status Status;
    ConsoleSession *CS;
    int Index;
    FILE *Out = NULL; /* We're in a socket callback, so we use the console session and don't output to a file. */
    TL_PABuf paBuf;

    Console *C = (Console *) Arg;

    Status = Platform_Accept(CONSOLE_PLATFORM, FD, & RemoteAddr, & RemotePort,
                             & NewSocketFD);
    if (FAILURE(Status))
        return;

    Status = Object_Allocate(CONSOLE_PLATFORM, sizeof(ConsoleSession),
                             ConsoleSession_Destructor, (void **) & CS);
    if (FAILURE(Status)) {
        Platform_CloseAndInvalidate(CONSOLE_PLATFORM, &NewSocketFD);
        return;
    }
    strncpy(CS->eyecatcher, "CSESSN ", sizeof CS->eyecatcher);

    CS->FD = NewSocketFD;
    CS->ConsolePtr = C;
    CS->CommandIndex = 0;
    CS->SkipControlChar = 0;
    CS->RemoteAddr = RemoteAddr;
    CS->RemotePort = RemotePort;
    CS->Echo = IPDR_FALSE;

    if (C->Password != NULL)
        CS->LoggedIn = IPDR_FALSE;
    else
        CS->LoggedIn = IPDR_TRUE;

#ifdef TRACE_MAINFUNC
    fprintf(stderr, "RegisterReadSocket %d [Console_AcceptCallback] Console_ConnectionCallback\n", NewSocketFD); fflush(stderr);
#endif

    Log_Write1(CONSOLE_LOG, IPDR_PLATFORM_MODULE_ID, IPDR_LEVEL_INFO,
               "Console_AcceptCallback",
               "Console connection from %.99s",
               TL_PrintAddressAndPort(RemoteAddr, RemotePort, paBuf));

    Status = Platform_RegisterReadSocket(CONSOLE_PLATFORM,
                                         NewSocketFD, Console_ConnectionCallback,
                                         CS,
                                         & (CS->SocketEventID));
    if (FAILURE(Status)) {
        Object_Release(CONSOLE_PLATFORM, CS);
        return ;
    }

    Status = DynamicArray_AddElement(CONSOLE_PLATFORM, & (C->Sessions),
                                     CS, & Index);
    if (FAILURE(Status)) {
        Object_Release(CONSOLE_PLATFORM, CS);
        return ;
    }

    if (C->Password != NULL)
        Console_Print(CS, Out, LOGIN_MESSAGE);
    else
        Console_Print(CS, Out, PROMPT);
}

/**
 *   Initialize the console given the platform services and
 *   the console port.
 *
 * @param PS platform services
 * @param C console
 * @param ParamsHandler parameters handler for retrieving
 *                   console parameters
 * @param Logger log used for the console
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status Console_Initialize(IPDR_PlatformServices *PS, Console *C,
                                ParametersHandler *ParamsHandler,
                                Log *Logger)
{
    IPDR_Status Status;

    C->ConsolePort = ParametersHandler_GetIntParam(ParamsHandler,
                                                   STR_configuration_console_consolePort);

    Util_StrDup(PS,
                ParametersHandler_GetStringParam(ParamsHandler,
                                                 STR_configuration_console_consolePassword),
                & (C->Password));

    C->AcceptEventID = NULL;
    IPDR_SetInvalidSocket(&C->AcceptSocket);
    C->PS = PS;
    C->Logger = Logger;

    Status = DynamicArray_Create(CONSOLE_PLATFORM, & (C->Modules), 16);
    if (FAILURE(Status))
        return Status;
    Status = DynamicArray_Create(CONSOLE_PLATFORM, & (C->Sessions), 16);
    if (FAILURE(Status))
        return Status;

    return Console_RegisterModule(C, "console", C, ConsoleCommands);
}

/**
 *   Reconfigure an existing console
 *
 * @param C console
 * @param ParamsHandler parameters handler for retrieving
 *                   console parameters
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status Console_Reconfigure(Console *C,
                                 ParametersHandler *ParamsHandler)
{
    IPDR_Status Status;
    int Port;
    const char *Password;

    if (! C) {
        return IPDR_OK;
    }

    Port = ParametersHandler_GetIntParam(ParamsHandler,
                                         STR_configuration_console_consolePort);

    if (C->ConsolePort != Port) {

        /* in case the port has changed, close all currently open sockets */

        C->ConsolePort = Port;

        Platform_UnregisterSocketAndClear(CONSOLE_PLATFORM, &C->AcceptEventID);
        Platform_CloseAndInvalidate(CONSOLE_PLATFORM, &C->AcceptSocket);

        DynamicArray_Reset(CONSOLE_PLATFORM, & (C->Sessions), 4);

        if (C->ConsolePort) {
            Status = Platform_CreateTCPSocket(CONSOLE_PLATFORM ,
                                              & (C->AcceptSocket));
            if (SUCCESS(Status)) {
                Status = Platform_Bind(CONSOLE_PLATFORM, C->AcceptSocket, 0,
                                       C->ConsolePort);
                if (FAILURE(Status)) {
                    Log_LogPlatformError(CONSOLE_LOG, IPDR_PLATFORM_MODULE_ID,
                                         "Console_Reconfigure", "Platform_Bind");
                    Platform_CloseAndInvalidate(CONSOLE_PLATFORM, &C->AcceptSocket);
                } else {
                    Status = Platform_Listen(CONSOLE_PLATFORM, C->AcceptSocket, 16);
                    if (FAILURE(Status)) {
                        Log_LogPlatformError(CONSOLE_LOG, IPDR_PLATFORM_MODULE_ID,
                                             "Console_Reconfigure", "Platform_Listen");
                        Platform_CloseAndInvalidate(CONSOLE_PLATFORM, &C->AcceptSocket);
                    } else {
#ifdef TRACE_MAINFUNC
                        fprintf(stderr, "RegisterReadSocket %d [Console_Reconfigure] Console_AcceptCallback\n", C->AcceptSocket); fflush(stderr);
#endif
                        Status = Platform_RegisterReadSocket(CONSOLE_PLATFORM,
                                                             C->AcceptSocket,
                                                             Console_AcceptCallback, C,
                                                             & (C->AcceptEventID));
                        if (FAILURE(Status)) {
                            Log_LogPlatformError(CONSOLE_LOG, IPDR_PLATFORM_MODULE_ID,
                                                 "Console_Reconfigure",
                                                 "Platform_RegisterReadSocket");
                            Platform_CloseAndInvalidate(CONSOLE_PLATFORM, &C->AcceptSocket);
                        }
                    }
                }
            }
        } else {
            Log_LogPlatformError(CONSOLE_LOG, IPDR_PLATFORM_MODULE_ID,
                                 "Console_Reconfigure",
                                 "Platform_CreateTCPSocket");
        }
    }

    Password = ParametersHandler_GetStringParam(ParamsHandler,
                                                STR_configuration_console_consolePassword);

    Util_StrDupAndReplace(C->PS, Password, & (C->Password));

    return IPDR_OK;
}


/**
 *   Start the console (start accepting connections)
 *
 * @param C console
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status Console_Start(Console *C)
{
    IPDR_Status Status;

    if (! C->ConsolePort)
        return IPDR_OK;

    Status = Platform_CreateTCPSocket(CONSOLE_PLATFORM , & (C->AcceptSocket));
    if (FAILURE(Status)) {
        Log_LogPlatformError(CONSOLE_LOG, IPDR_PLATFORM_MODULE_ID,
                             "Console_Start",
                             "Platform_CreateTCPSocket");
        return Status;
    }

    Status = Platform_Bind(CONSOLE_PLATFORM, C->AcceptSocket, 0, C->ConsolePort);
    if (FAILURE(Status)) {
        Log_LogPlatformError(CONSOLE_LOG, IPDR_PLATFORM_MODULE_ID,
                             "Console_Start",
                             "Platform_Bind");
        Platform_CloseAndInvalidate(CONSOLE_PLATFORM, &C->AcceptSocket);
        return Status;
    }

    Status = Platform_Listen(CONSOLE_PLATFORM, C->AcceptSocket, 16);
    if (FAILURE(Status)) {
        Log_LogPlatformError(CONSOLE_LOG, IPDR_PLATFORM_MODULE_ID,
                             "Console_Start",
                             "Platform_Listen");
        Platform_CloseAndInvalidate(CONSOLE_PLATFORM, &C->AcceptSocket);
        return Status;
    }

#ifdef TRACE_MAINFUNC
    fprintf(stderr, "RegisterReadSocket %d [Console_Start] Console_AcceptCallback\n", C->AcceptSocket); fflush(stderr);
#endif
    Status = Platform_RegisterReadSocket(CONSOLE_PLATFORM, C->AcceptSocket,
                                         Console_AcceptCallback, C,
                                         & (C->AcceptEventID));
    if (FAILURE(Status)) {
        Log_LogPlatformError(CONSOLE_LOG, IPDR_PLATFORM_MODULE_ID,
                             "Console_Start",
                             "Platform_RegisterReadSocket");
        Platform_CloseAndInvalidate(CONSOLE_PLATFORM, &C->AcceptSocket);
        return Status;
    }

#if 0 && defined(ENABLE_SIMULATOR)
#ifdef STD_FILE_OPERATIONS_SUPPORT
    /* This feature has been removed */
    if (C->StartupScript != NULL) {
        const char *Argv[1];
        Argv[0] = C->StartupScript;
        Console_LoadFileCommand(NULL, 1, Argv, C);
    }
#endif
#endif

    return IPDR_OK;
}

/**
 *   Delete the console and close any connections
 *
 * @param C console
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status Console_Delete(Console *C)
{
    Object_Release(CONSOLE_PLATFORM, C->Password);
    Platform_UnregisterSocketAndClear(CONSOLE_PLATFORM, &C->AcceptEventID);
    Platform_CloseAndInvalidate(CONSOLE_PLATFORM, &C->AcceptSocket);
    DynamicArray_Delete(CONSOLE_PLATFORM, & (C->Modules));
    DynamicArray_Delete(CONSOLE_PLATFORM, & (C->Sessions));
    return IPDR_OK;
}

/**
 *   Register a module within the console given the module
 *   name and the list of commands supported by the module
 *
 * @param C console
 * @param ModuleName name of module
 * @param ContextArg extra argument passed to the command
 *                functions
 * @param Commands array of commands terminated by a command
 *              with a NULL name.
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status Console_RegisterModule(Console *C, const char *ModuleName,
                                    void *ContextArg,
                                    ConsoleCommand *Commands)
{
    ConsoleModule *M;
    IPDR_Status Status;
    int Index;

    Status = Object_Allocate(CONSOLE_PLATFORM, sizeof(ConsoleModule),
                             ConsoleModule_Destructor, (void **) & M);
    if (FAILURE(Status))
        return Status;
    strncpy(M->eyecatcher, "CMODULE", sizeof M->eyecatcher);

    M->ContextArg = ContextArg;
    M->Commands = Commands;

    Status = Util_StrDup(CONSOLE_PLATFORM, ModuleName, & (M->Name));
    if (FAILURE(Status)) {
        Object_Release(CONSOLE_PLATFORM, M);
        return Status;
    }

    Status = DynamicArray_AddElement(CONSOLE_PLATFORM, & (C->Modules),
                                     M, & Index);
    if (FAILURE(Status))
        return Status;

    return IPDR_OK;
}

/**
 *   Display help on all commands supported by the console.
 *
 * @param S session
 * @param Out output file/socket (used if S is NULL)
 * @param Argc number of arguments
 * @param Argv arguments
 * @param Arg context argument (NULL)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static
IPDR_Status Console_HelpCommand(ConsoleSession *S, FILE *Out, int Argc,
                                 const char * const Argv[], void *Arg)
{
    Console *C = (Console *) Arg;
    ConsoleModule *M;
    ConsoleCommand *CC;
    int i;

    for (i = 0; i < C->Modules.Length; i++) {
        M = (ConsoleModule *) C->Modules.Data[i];
        Console_Println(S, Out, "%.99s commands:", M->Name);
        CC = M->Commands;
        while (CC->Name != NULL) {
            Console_Println(S, Out, "  %.99s: %.199s", CC->Name, CC->Help);
            CC++;
        }
    }

    return IPDR_CMD_HELP;
}

/**
 *   Display help on all commands supported by a module.
 *
 * @param S console session
 * @param Out output file/socket (used if S is NULL)
 * @param Argc number of arguments
 * @param Argv arguments
 * @param Arg context argument (NULL)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static
IPDR_Status Console_HelpModuleCommand(ConsoleSession *S, FILE *Out, int Argc,
                                       const char * const Argv[], void *Arg)
{
    Console *C = (Console *) Arg;
    ConsoleModule *M;
    ConsoleCommand *CC;

    if (Argc != 1) {
        Console_Println(S, Out, "Invalid number of arguments.");
        return IPDR_OK;
    }

    M = Console_FindModuleByName(C, Argv[0]);
    if (M == NULL) {
        int i;
        Console_Println(S, Out, "Unable to find module %.99s.  Possibilities are:", Argv[0]);
        for (i = 0; i < C->Modules.Length; i++) {
            Console_Println(S, Out, "    %.99s", ((ConsoleModule *)C->Modules.Data[i])->Name);
        }
        return IPDR_OK;
    }

    CC = M->Commands;
    while (CC->Name != NULL) {
        Console_Println(S, Out, "  %.99s: %.199s", CC->Name, CC->Help);
        CC++;
    }

    return IPDR_OK;
}

/**
 *   Display help on a specific command.
 *
 * @param S console session
 * @param Out output file/socket (used if S is NULL)
 * @param Argc number of arguments
 * @param Argv arguments
 * @param Arg context argument (NULL)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static
IPDR_Status Console_HelpCommandCommand(ConsoleSession *S, FILE *Out, int Argc,
                                        const char * const Argv[], void *Arg)
{
    Console *C = (Console *) Arg;
    ConsoleModule *M;
    ConsoleCommand *CC;

    if (Argc != 1) {
        Console_Println(S, Out, "Invalid number of arguments.");
        return IPDR_OK;
    }

    CC = Console_FindCommandByName(C, Argv[0], & M);
    if (CC == NULL) {
        int i;
        Console_Println(S, Out, "Unable to find command %.99s.  Possibilities are:", Argv[0]);
        for (i = 0; i < C->Modules.Length; i++) {
            M = (ConsoleModule *) C->Modules.Data[i];
            CC = M->Commands;
            Console_Println(S, Out, "  module: %.99s", M->Name);
            for (; CC->Name; CC++) {
                Console_Println(S, Out, "    %.199s", CC->Name);
            }
        }
    } else {
        Console_Println(S, Out, "  %.99s: %.199s", CC->Name, CC->Help);
    }

    return IPDR_OK;
}

/**
 *   Load and execute a file containing console commands
 *
 * @param S session console
 * @param Argc number of arguments
 * @param Argv arguments
 * @param Arg context argument (Console)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
#ifdef ENABLE_SIMULATOR
#ifdef STD_FILE_OPERATIONS_SUPPORT
static
IPDR_Status Console_LoadFileCommand(ConsoleSession *S, FILE *Out, int Argc,
                                     const char * const Argv[], void *Arg)
{
#ifdef STD_FILE_OPERATIONS_SUPPORT
    FILE *F;
    char Line[MAX_COMMAND_LINE_LEN];
    Console *C = (Console *) Arg;

    if (Argc != 1) {
        Console_Println(S, Out, "Invalid number of arguments.");
        return IPDR_OK;
    }

    F = fopen(Argv[0], "r");
    if (F == NULL) {
        Console_Println(S, Out, "Unable to open file \"%.299s\"", Argv[0]);
        return IPDR_OK;
    }

    while (fgets(Line, MAX_COMMAND_LINE_LEN, F) != NULL) {
        const char* Command;
        IPDR_Status rc;
        while (1) {
            if (Line[strlen(Line)-1] == '\n' || Line[strlen(Line)-1] == '\r')
                Line[strlen(Line)-1] = 0;
            else
                break ;
        }

        if (S != NULL) {
            Console_Print(S, Out, PROMPT);
            Console_Println(S, Out, Line);
        }

        rc = Console_ActivateCommand(C, S, Line, &Command, NULL, NULL);
        switch (rc) {
        case IPDR_CMD_EXIT:
            break;
        case IPDR_CMD_NOT_FOUND:
            Console_Println(S, Out, "Unable to find command: `%.199s'.", Command);
        }
    }
    fclose(F);
#endif
    return IPDR_OK;
}
#endif
#endif

/**
 *   Print a string to the console
 *
 * @param S session to output string to
 * @param Out output file/socket (used if S is NULL)
 * @param FmtStr format string
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status Console_Print(ConsoleSession *S, FILE *Out, const char *FmtStr, ...)
{
#ifdef STD_FILE_OPERATIONS_SUPPORT
    va_list Args;
    char Line[MAX_LINE_LEN];
    unsigned int WB;
    Console *C;


    va_start(Args, FmtStr);
#ifndef _MSC_VER
    vsprintf (Line, FmtStr, Args),
#else
        _vsnprintf (Line, MAX_LINE_LEN, FmtStr, Args),
#endif
        va_end(Args);

    if (S == NULL) {
        if (Out == NULL) {
            fprintf(stderr, "%s", Line);
        } else {
            fprintf(Out, "%s", Line);
        }
        return IPDR_OK;
    }

    C = S->ConsolePtr;
    return Platform_Send(CONSOLE_PLATFORM, S->FD, Line, strlen(Line), & WB);
#else
    return IPDR_OK;
#endif
}

/**
 *   Print a string to the console with an end of line char
 *
 * @param S session to output string to
 * @param Out output file/socket (used if S is NULL)
 * @param FmtStr format string
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status Console_Println(ConsoleSession *S, FILE *Out, const char *FmtStr, ...)
{
#ifdef STD_FILE_OPERATIONS_SUPPORT
    va_list Args;
    char Line[MAX_LINE_LEN];
    const char *EolStr = "\r\n";
    unsigned int WB;
    Console *C;

    va_start(Args, FmtStr);
#ifndef _MSC_VER
    vsprintf (Line, FmtStr, Args),
#else
        _vsnprintf (Line, MAX_LINE_LEN, FmtStr, Args),
#endif
        va_end(Args);

    if (S == NULL) {
        if (Out == NULL) {
            fprintf(stderr, "%s\n", Line);
        } else {
            fprintf(Out, "%s\n", Line);
        }
        return IPDR_OK;
    }

    C = S->ConsolePtr;
    Platform_Send(CONSOLE_PLATFORM, S->FD, Line, strlen(Line), & WB);
    return Platform_Send(CONSOLE_PLATFORM, S->FD, EolStr, 2, & WB);
#else
    return IPDR_OK;
#endif
}


/**
 * Set echoing on/off
 *
 * @param S session console
 * @param Out output file/socket (used if S is NULL)
 * @param Argc number of arguments
 * @param Argv arguments
 * @param Arg context argument (Console)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
static
IPDR_Status Console_EchoCommand(ConsoleSession *S, FILE *Out, int Argc,
                                 const char * const Argv[], void *Arg)
{
    /* Console *C = (Console *) Arg; */

    if (Argc != 0) {
        Console_Println(S, Out, "Invalid number of arguments.");
        return IPDR_OK;
    }

    Console_Println(S, Out, "Echoing commands.");

    S->Echo = IPDR_TRUE;

    return IPDR_OK;
}
