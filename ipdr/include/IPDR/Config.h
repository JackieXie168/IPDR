/** @file Config.h
 *
 * Control of the availability of features in the IPDR client (transmitter) API.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: Config.h,v 1.7 2005/01/30 15:41:23 guyg Exp $
 *
 * This file contains a number of macros whose presence or absence controls
 * whether code is generated for various features.
 */
#ifndef _IPDR_CONFIG_H_

/** Macro to prevent double inclusion of this header */
#define _IPDR_CONFIG_H_

#if defined(IPDR_COPYRIGHT_WITH_IDENT)
#define IPDR_COPYRIGHT_STR " Copyright (c) 2005 IPDR Organization, All Rights Reserved\n* The use and distribution of this software is governed by the terms of\n* the license agreement which can be found in the file LICENSE.TXT at\n* the top of this source tree.\n*\n* This software is distributed on an \"AS\nIS\" basis, WITHOUT WARRANTY OF\n* ANY KIND, either express or implied."
#else
#define IPDR_COPYRIGHT_STR
#endif


/** Version control information */
#define IPDR_CONFIG_H_RCSID "@(#) $Id: Config.h,v 1.7 2005/01/30 15:41:23 guyg Exp $" IPDR_COPYRIGHT_STR

/** Define GLOBAL_RENAME if you want all globals to be prefixed by
 *  "IPDR__" to prevent name clashes. Don't do this if your linker
 *  can't handle long names.
 */

#if 0 && ! DOXYGEN_PROCESSING
#define GLOBAL_RENAME
#endif

#if defined(GLOBAL_RENAME)
/** Renaming global version identifier (to avoid name clashes) */
#define Config_h_rcsid IPDR__Config_h_rcsid
#endif

/** @defgroup IPDR_Config Configuration macros
 * @{
 */

/** Define ENABLE_SIMULATOR in case you wish to include support for
 *  the Host Application Simulator, allowing you to test the client
 *  (transmitter) on a given platform before integrating to another
 *  application. This feature is obsolete and not properly supported.
 *  Instead, look at the sample programs.
 *
 *  @note You can control the "console" feature (whether for testing
 *  or for deployment) by the <code>configuration.console.consolePort</code>
 *  parameter.
 */

#if 0 || DOXYGEN_PROCESSING
#define ENABLE_SIMULATOR
#endif

/** Define STD_FILE_OPERATIONS_SUPPORT if the target system supports
 *  standard file operations (using fopen, fread ...). Undefining the
 *  flag will disable support for sending data from a file in the
 *  simulator and using a startup script in the console.
 */

#if 1 || DOXYGEN_PROCESSING
#define STD_FILE_OPERATIONS_SUPPORT
#endif

/** Define VQ_HANDLER_SUPPORT, if you want the client (transmitter) to
 *  be compiled with support for an internal queueing mechanism.
 *  Currently, this is the only kind of queueing supported.
 */

#if 1 || DOXYGEN_PROCESSING
#define VQ_HANDLER_SUPPORT
#endif

#if 0

/** Define EXT_QUEUE_HANDLER_SUPPORT if you want the client
 *  (transmitter) to be compiled with support for external queuing
 *  mechanism (no queuing is internally done) (the client can be
 *  compiled with support for both queueing modes).
 *
 * This has been removed because it has not been tested: 2002/03/07.
 */

#if 0 || DOXYGEN_PROCESSING
#define EXT_QUEUE_HANDLER_SUPPORT
#endif

#endif

#if 0 /* || DOXYGEN_PROCESSING */

/** Define PERSISTENT_SESSIONS if you want the sessions and servers
 *  (collectors) to be stored in non-volatile memory. This should
 *  normally be turned off because reloading the MIB from non-volatile
 *  memory won't actually create the sessions; instead you should use
 *  the commands.configurationCommandString to create them.
 *
 *  @warning This is no longer used.
 */

#define PERSISTENT_SESSIONS
#endif

#if 0 /* || DOXYGEN_PROCESSING */

/** Define PERSISTENT_TEMPLATES if you want the current set of key
 *  attributes to be stored within non-volatile memory (if not
 *  defined, key attributes will be updated from server (server) after
 *  templates negotiation. This should normally be turned off because
 *  template negotiation should be done each time the client
 *  (transmitter) (re)starts.
 *
 *  @warning This is no longer used.
 */

#define PERSISTENT_TEMPLATES
#endif

#if 0 /* || DOXYGEN_PROCESSING */

/** Define PERSISTENT_CONFIGURATION if you want configuration
 *  parameters to be stored in non-volatile memory.
 *
 *  @warning This is no longer used.
 */
#define PERSISTENT_CONFIGURATION
#endif

/** Define IPDR_utilities if you want extra utility functions for
 *  debugging.
 */

#if 1 || DOXYGEN_PROCESSING
#define IPDR_UTILITIES
#endif

/*@}*/

#endif
