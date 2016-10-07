/** @file Common/CommonMsgs.h
 *
 * Log messages output by the common module, as implemented in Common/CommonMsgs.c.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: CommonMsgs.h,v 1.8 2005/01/30 15:41:24 guyg Exp $
 * 
 */
#ifndef _COMMON_MSGS_H_
#define _COMMON_MSGS_H_

#include <IPDR/Config.h>
#define COMMON_MSGS_H_RCSID "@(#) $Id: CommonMsgs.h,v 1.8 2005/01/30 15:41:24 guyg Exp $" IPDR_COPYRIGHT_STR

#if defined(GLOBAL_RENAME)
#define CommonMsgs_Initialize IPDR__CommonMsgs_Initialize
#define CommonMsgs_h_rcsid    IPDR__CommonMsgs_h_rcsid
#define CommonMsgs_rcsid      IPDR__CommonMsgs_rcsid
#endif

#define MEMORY_MODULE_ID   2

#define MSG_MEMORY_ALLOCATE_OBJECT     /*16*/ "Allocating object - Addr=%x, Size=%d"
#define MSG_MEMORY_RELEASE_OBJECT      /*17*/ "Release object - Addr=%x, RC=%d"
#define MSG_MEMORY_DELETE_OBJECT       /*18*/ "Delete object - Addr=%x"
#define MSG_MEMORY_DUPLICATE_OBJECT    /*19*/ "Duplicate object - Addr=%x, RC=%d"
#define MSG_CHUNKS_MGR_OUT_OF_MEMORY   /*20*/ "Memory limit exceeded - Requested=%u, Max=%u ChunkSz=%u (ChunkSzInitial=%u Used=%u)"
#define MSG_CHUNKS_MGR_EMPTY_CHUNK     /*21*/ "Empty chunk reached"

#include <IPDR/IPDR.h>
#include <Common/Log.h>

#ifdef __cplusplus
extern "C" {
#endif

IPDR_Status CommonMsgs_Initialize(Log *L);

#ifdef __cplusplus
}
#endif

#endif
