/** @file Exporter/ExporterConsole.h
 *
 * Header file for registering client (transmitter) related console commands,
 * defined in Exporter/ExporterConsole.c.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: ExporterConsole.h,v 1.9 2005/01/30 15:41:25 guyg Exp $
 * 
 */
#ifndef _EXPORTER_CONSOLE_H_
#define _EXPORTER_CONSOLE_H_

#include <IPDR/Config.h>

#define EXPORTER_CONSOLE_H_RCSID "@(#) $Id: ExporterConsole.h,v 1.9 2005/01/30 15:41:25 guyg Exp $" IPDR_COPYRIGHT_STR

#if defined(GLOBAL_RENAME)
#define ExporterConsole_h_rcsid                 IPDR__ExporterConsole_h_rcsid
#define ExporterConsole_rcsid                   IPDR__ExporterConsole_rcsid
#define ExporterConsole_RegisterConsoleCommands IPDR__ExporterConsole_RegisterConsoleCommands
#endif

#include <Common/Console.h>
#include <Exporter/Exporter.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Register client (transmitter) related commands within the console.
 *
 * @param Clnt client (transmitter)
 * @param C console
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status ExporterConsole_RegisterConsoleCommands(Exporter *E, Console *C);

#ifdef __cplusplus
}
#endif

#endif
