/** @file Common/Transport.h
 *
 * Header file for the IPDR transport abstraction.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: Transport.h,v 1.14 2005/05/11 12:25:16 dmitryk Exp $
 *
 * The file contains data structures used by the transport layer as
 * well as macros used to actually call the specific transport
 * (currently the macros always call the TCP implementation).
 *
 * @see Transport/TCP/TCPTransport.c
 *
 * @note This code was originally designed to allow for generic connections
 * and transport. For now, it is specific to TCP only; but the types
 * Connection and Transport have been kept to allow for the more
 * generic version ... in the meantime, the compiler can catch typing
 * errors.
 */
#ifndef _TRANSPORT_H_
#define _TRANSPORT_H_

#include <IPDR/Config.h>
#define TRANSPORT_H_RCSID "@(#) $Id: Transport.h,v 1.14 2005/05/11 12:25:16 dmitryk Exp $" IPDR_COPYRIGHT_STR

#define Transport_h_rcsid IPDR__Transport_h_rcsid

#include <IPDR/IPDR.h>
#include <Common/Platform.h>
#include <Common/ChunksManager.h>


#ifdef __cplusplus
extern "C" {
#endif

#if 0
/* The generic way: */
typedef void *Connection;
typedef void *Transport;
#else
/* The include for Transport/TCP/TCPTransport.h must be after the
 * definition of ConnectionHandlers */
typedef struct IPDR_tag_TCPConnection *Connection;
typedef struct IPDR_tag_TCPTransport  *Transport;
#endif

#include <Transports/TCP/TCPTransport.h>


/**
 * The connection handlers structure is used to receive notifications
 * in cases of a connection, disconnection and arrival of data.
 */
typedef struct IPDR_tag_ConnectionHandlers
{
  void (*Connect)(Connection C, IPDR_IPAddress  Addr, unsigned short Port, void *Arg);
  void (*Disconnect)(Connection C, void *Arg);
  void (*Data)(Connection C, BufferHandle *BH, unsigned int MessageLength, void *Arg);
}
ConnectionHandlers;

#define TL_InitializePassive(A1, A2, A3, A4, A5, A6, A7, A8, A9) \
       TCP_InitializePassive(A1, A2, A3, A4, A5, A6, A7, A8, A9)
#define TL_InitializeActive(A1, A2, A3, A4, A5, A6) \
       TCP_InitializeActive(A1, A2, A3, A4, A5, A6)
#define TL_Shutdown(A1) TCP_Shutdown(A1)

/*#define TL_Connect(A1, A2, A3, A4, A5, A6, A7, A8) \
       TCP_Connect(A1, A2, A3, A4, A5, A6, A7, A8)*/
#define TL_Disconnect(A1) \
TCP_Disconnect(A1)
#define	TL_CreateConnection(A1, A2, A3, A4, A5) TCP_CreateConnectionIdle(A1, A2, A3, A4, A5)
#define TL_Connect(A1, A2)	TCP_Connect(A1, A2)
#define TL_GetSockName(A1, A2)	TCP_GetSockName(A1, A2)
#define TL_Send(A1, A2, A3) TCP_Send(A1, A2, A3)
#define TL_Receive(A1, A2, A3) TCP_Receive(A1, A2, A3)
#define TL_ResetConnection(A1) TCP_ResetConnection(A1)
#define TL_SetConnectionHandlers(A1, A2, A3) \
TCP_SetConnectionHandlers(A1, A2, A3)
#define TL_GetConnectionInfo(A1, A2, A3) TCP_GetConnectionInfo(A1, A2, A3)
#define TL_GetConnectionInfoStr(C) TCP_GetConnectionInfoStr(C)
typedef char TL_PABuf[256];
#define TL_PrintAddress(Addr, Buf) TCP_PrintAddress(Addr, Buf)
#define TL_PrintAddressAndPort(Addr, Port, Buf) TCP_PrintAddressAndPort(Addr, Port, Buf)
#define TL_ParseAddress(Str) TCP_ParseAddress(Str)
#define TL_PrintConnectionRemote(Conn, Str) TCP_PrintConnectionRemote(Conn, Str)
#define TL_PrintConnectionLocale(Conn, Str) TCP_PrintConnectionLocale(Conn, Str)
#define TL_PrintConnectionFull(Conn, Str) TCP_PrintConnectionFull(Conn, Str)

#define TL_GetReceiveChunksManager(A1, A2) \
       TCP_GetReceiveChunksManager(A1, A2)
#define TL_GetSendChunksManager(A1, A2) \
       TCP_GetSendChunksManager(A1, A2)
#define TL_Reconfigure(A1) TCP_Reconfigure(A1)

#define TL_HandleDisconnection(C) TCP_HandleDisconnection(C)

#ifdef __cplusplus
}
#endif

#endif
