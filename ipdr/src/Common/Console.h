/** @file Common/Console.h
 *
 * Header file for the IPDR console, as implemented in Common/Console.c.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: Console.h,v 1.9 2005/04/21 11:31:22 dmitryk Exp $
 * 
 * The file contains data structures / prototypes relating to the
 * IPDR console.
 */
#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <IPDR/Config.h>
#define CONSOLE_H_RCSID "@(#) $Id: Console.h,v 1.9 2005/04/21 11:31:22 dmitryk Exp $" IPDR_COPYRIGHT_STR

#if defined(GLOBAL_RENAME)
#define Console_ActivateCommand   IPDR__ActivateCommand
#define Console_Delete            IPDR__Console_Delete
#define Console_Initialize        IPDR__Console_Initialize
#define Console_Print             IPDR__Console_Print
#define Console_Println           IPDR__Console_Println
#define Console_Reconfigure       IPDR__Console_Reconfigure
#define Console_RegisterModule    IPDR__Console_RegisterModule
#define Console_Start             IPDR__Console_Start
#define Console_h_rcsid           IPDR__Console_h_rcsid
#define Console_rcsid             IPDR__Console_rcsid
#define ConsoleModule_Destructor  IPDR__ConsoleModule_Destructor
#define ConsoleSession_Destructor IPDR__ConsoleSession_Destructor
#endif

#include <stdio.h>

#include <Common/DynamicArray.h>
#include <Common/ParametersHandler.h>
#include <Common/Log.h>
#include <Common/ConfigurationNames.h>

#ifdef __cplusplus
extern "C" {
#endif

#if 0
} /* put this here to un-confuse Emacs's automatic indentation */
#endif

#define MAX_COMMAND_LINE_LEN 2048

/**
 * structure containing information about the console.
 */
typedef struct IPDR_tag_Console
{
    char eyecatcher[8]; /**< Gets "CONSOLE " */
    IPDR_SocketHandle AcceptSocket;   /**< socket accepting TCP connections */
    IPDR_SocketEventID AcceptEventID; /**< event handler for socket */
    IPDR_PlatformServices *PS;        /**< platform services */
    unsigned short ConsolePort;        /**< console port number */
    DynamicArray Modules;              /**< array of modules */
    DynamicArray Sessions;             /**< array of open sessions */
    char *Password;                    /**< console password */
    Log *Logger;                       /**< logger used for the console */
#ifdef ENABLE_SIMULATOR
    char *StartupScript;
#endif
}
Console;

/**
 * structure containing information about a connection to the console.
 */
typedef struct
{
    char eyecatcher[8]; /**< Gets "CSESSN " */
    IPDR_SocketEventID SocketEventID; /**< socket event handler */
    IPDR_SocketHandle FD;             /**< socket FD */
    FILE *Out;                         /**< output stream corresponding
                                          to the socket */
    Console *ConsolePtr;               /**< console associated with the
                                          session */
    IPDR_BOOL LoggedIn;               /**< flag indicating whether the
                                          user logged in */
    IPDR_BOOL Echo;                   /**< echo input lines? */
    IPDR_IPAddress RemoteAddr;
    unsigned short  RemotePort;

    char CommandBuffer[MAX_COMMAND_LINE_LEN];
    int CommandIndex;
    int SkipControlChar;
}
ConsoleSession;

/**
 * Function pointer, called by the command interpreter.
 *
 * @param S console session (may be NULL)
 * @param Out output file/socket (used if S is NULL)
 * @param Argc number of arguments
 * @param Argv arguments
 * @param Context context argument (NULL)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Status (*ConsoleCommandFunc)(ConsoleSession *S,
                                           FILE *Out, /* used if S is NULL */
                                           int Argc,
                                           const char * const Argv[],
                                           void *Context);


/**
 * information about a single command supported by the console.
 */
typedef struct
{
    const char *Name;               /**< command name */
    const char *Help;               /**< command help string */
    ConsoleCommandFunc CommandFunc; /**< function implementing the command */
}
ConsoleCommand;

/**
 * information about a module (collection of commands) within the
 * console.
 */
typedef struct
{
    char eyecatcher[8]; /**< Gets "CMODULE" */
    void *ContextArg;
    char *Name;
    ConsoleCommand *Commands;
}
ConsoleModule;



IPDR_Status Console_Initialize(IPDR_PlatformServices *PS, Console *C,
                                ParametersHandler *ParamsHandler,
                                Log *Logger);
IPDR_Status Console_Start(Console *C);
IPDR_Status Console_Delete(Console *C);
IPDR_Status Console_RegisterModule(Console *C, const char *ModuleName,
                                    void *ContextArg,
                                    ConsoleCommand *Commands);
/*PRINTFLIKE3*/
IPDR_Status Console_Print(ConsoleSession *S, FILE *Out, const char *FmtStr, ...);
/*PRINTFLIKE3*/
IPDR_Status Console_Println(ConsoleSession *S, FILE *Out, const char *FmtStr, ...);
IPDR_Status Console_Reconfigure(Console *C, ParametersHandler *ParamsHandler);
IPDR_Status Console_ActivateCommand(Console *C, ConsoleSession *S, FILE *Out, char *Cmd,
                                     const char **ParsedCommand,
                                     int *ParsedArgc,
                                     const char ***ParsedArgs,
                                     char **RemainingCmd,
									 char *CmdRes,
									 int CmdResLen);

void ConsoleSession_Destructor(IPDR_PlatformServices *PS, void *Obj);
void ConsoleModule_Destructor(IPDR_PlatformServices *PS, void *Obj);
void NameDef_Destructor(IPDR_PlatformServices *PS, void *Obj);

#ifdef __cplusplus
}
#endif

#endif
