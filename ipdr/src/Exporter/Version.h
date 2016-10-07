/** @file Version.h
 *
 * The version constant for the client (transmitter).
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: Version.h,v 1.9 2005/01/30 15:41:26 guyg Exp $
 * 
 */
#ifndef _VERSION_H_
#define _VERSION_H_

#include <IPDR/Config.h>
#define VERSION_H_RCSID "@(#) $Id: Version.h,v 1.9 2005/01/30 15:41:26 guyg Exp $" IPDR_COPYRIGHT_STR

/* Note: IPDR_VER is provided by "-D" flag in compilation */
/* #define EXPORTER_VERSION "2.1.2" */
#define EXPORTER_VERSION IPDR_VER

#if defined(GLOBAL_RENAME)
#define Version_h_rcsid IPDR__Version_h_rcsid
#endif

#endif
