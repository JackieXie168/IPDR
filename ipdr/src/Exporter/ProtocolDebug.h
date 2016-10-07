/** @file Exporter/ProtocolDebug.h
 *
 * Header file for functions defined in Exporter/ProtocolDebug.c
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: ProtocolDebug.h,v 1.8 2005/01/30 15:41:26 guyg Exp $
 * 
 */
#ifndef _PROTOCOL_DEBUG_H_
#define _PROTOCOL_DEBUG_H_

#include <IPDR/Config.h>
#define PROTOCOL_DEBUG_H_RCSID "@(#) $Id: ProtocolDebug.h,v 1.8 2005/01/30 15:41:26 guyg Exp $" IPDR_COPYRIGHT_STR

#if defined(GLOBAL_RENAME)
#define ProtocolDebug_PrintIncomingMessage IPDR__ProtocolDebug_PrintIncomingMessage
#define ProtocolDebug_PrintOutgoingMessage IPDR__ProtocolDebug_PrintOutgoingMessage
#define ProtocolDebug_h_rcsid              IPDR__ProtocolDebug_h_rcsid
#define ProtocolDebug_rcsid                IPDR__ProtocolDebug_rcsid
#endif

#include <Exporter/Exporter.h>

#ifdef __cplusplus
extern "C" {
#endif

IPDR_Status ProtocolDebug_PrintOutgoingMessage(Log *Logger,
                                                Connection Conn,
                                                const char *Msg,
                                                IPDR_DebugLevel Lvl);
IPDR_Status ProtocolDebug_PrintIncomingMessage(Log *Logger,
                                                Connection Conn,
                                                const char *Msg,
                                                IPDR_DebugLevel Lvl);

#ifdef __cplusplus
}
#endif

#endif
