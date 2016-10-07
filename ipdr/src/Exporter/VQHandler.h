/** @file Exporter/VQHandler.h
 *
 * Header file for functions implemented in Exporter/VQHandler.c.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: VQHandler.h,v 1.8 2005/01/30 15:41:26 guyg Exp $
 * 
 */

#include <IPDR/Config.h>
#define VQ_HANDLER_H_RCSID "@(#) $Id: VQHandler.h,v 1.8 2005/01/30 15:41:26 guyg Exp $" IPDR_COPYRIGHT_STR

#if defined(GLOBAL_RENAME)
#define VQHandler_h_rcsid IPDR__VQHandler_h_rcsid
#define VQHandler_rcsid   IPDR__VQHandler_rcsid
#endif

#include <Exporter/Exporter.h>

#ifdef VQ_HANDLER_SUPPORT
extern QueueHandler VQHandler;
#endif
