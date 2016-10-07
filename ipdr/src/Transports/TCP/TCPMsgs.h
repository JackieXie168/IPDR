/** @file Transports/TCP/TCPMsgs.h
 *
 * Header file for messages used in "TCP" module.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: TCPMsgs.h,v 1.7 2005/01/30 15:41:27 guyg Exp $
 *
 */

#ifndef _TCP_MSGS_H_
#define _TCP_MSGS_H_

#include <IPDR/Config.h>
#define TCP_MSGS_H_RCSID "@(#) $Id: TCPMsgs.h,v 1.7 2005/01/30 15:41:27 guyg Exp $" IPDR_COPYRIGHT_STR

#if defined(GLOBAL_RENAME)
#define TCPMsgs_Initialize IPDR__TCPMsgs_Initialize
#define TCPMsgs_h_rcsid    IPDR__TCPMsgs_h_rcsid
#define TCPMsgs_rcsid      IPDR__TCPMsgs_rcsid
#endif

#include <Common/Log.h>

#define TCP_MODULE_ID 3

#define MSG_TCP_ACCEPT                     /*16*/  ("Accepted connection - %.98s, Socket=" Platform_SOCKET_FMT)
#define MSG_TCP_CONNECT                    /*17*/  ("Connected - %.98s, Socket=" Platform_SOCKET_FMT)
#define MSG_TCP_DISCONNECT                 /*18*/  ("Disconnected - %.98s, Socket=" Platform_SOCKET_FMT " (useCount=%d)")
#define MSG_TCP_RECEIVE                    /*19*/  ("Received data - Addr=%.98s, Size=%d, Socket=" Platform_SOCKET_FMT)
#define MSG_TCP_SEND                       /*20*/  ("Send data - Addr=%.98s, Size=%d, FD=" Platform_SOCKET_FMT)
#define MSG_TCP_RECEIVE_CORRUPTED          /*21*/  ("Received data with bad header: %.98s, Size=%d Version=%d MsgID=%d Flags=0x%02x Session=%d, Socket=" Platform_SOCKET_FMT)
#define MSG_TCP_CLOSE                      /*22*/  ("Close socket %.98s socket=" Platform_SOCKET_FMT)
#define MSG_TCP_SEND_FAILED                /*23*/  ("TCP Send %.98s failed Status=%.99s, Socket=" Platform_SOCKET_FMT)
#define MSG_TCP_ALLOC_SENDBUFFER_FAILED    /*24*/  ("Alloc TCPSendBuffer failed size=%d, %.98s: forcing disconnect (Socket=" Platform_SOCKET_FMT ")")
#define MSG_TCP_ALLOC_MAXBUFFER_FAILED     /*25*/  ("Alloc TCP max buffer (read) failed, Addr=%.98s: forcing disconnect (Socket=" Platform_SOCKET_FMT ")")
#define MSG_TCP_ALLOC_FAILED               /*26*/  "TCP Memory Alloc failed size=%d"
#define MSG_TCP_INIT_CHUNKS_FAILED         /*27*/  "TCP Initialization of chunks failed"
#define MSG_TCP_ALLOC_RECVBUFFER_FAILED    /*28*/  ("Alloc TCPRecvBuffer failed size=%d, %.98s: forcing disconnect (Socket=" Platform_SOCKET_FMT ")")
#define MSG_TCP_INVALID_CONN_FD            /*29*/  ("Invalid FileDescriptor (socket number) on callback, %.98s, FD=" Platform_SOCKET_FMT ", ConnFD=" Platform_SOCKET_FMT ": forcing disconnect")
#define MSG_TCP_DUPLICATE_ACCEPT           /*30*/  "Duplicate connection to %.120s - disconnecting existing and accepting new"
#define MSG_TCP_REFUSED_ACCEPT             /*31*/  "Refused connection to %.120s - could not free existing duplicate connection"


#ifdef __cplusplus
extern "C"
{
#endif

IPDR_Status TCPMsgs_Initialize(Log *L);

#ifdef __cplusplus
}
#endif

#endif
