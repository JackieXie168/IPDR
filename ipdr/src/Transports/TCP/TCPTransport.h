/** @file Transports/TCP/TCPTransport.h
 *
 * Header for the IPDR TCP based transport layer, as
 * implemented in Transport/TCP/TCPTransport.c.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: TCPTransport.h,v 1.17 2005/05/17 10:19:29 dmitryk Exp $
 *
 */
#ifndef _TCP_TRANSPORT_H_
#define _TCP_TRANSPORT_H_

#include <IPDR/Config.h>
#define TCP_TRANSPORT_H_RCSID "@(#) $Id: TCPTransport.h,v 1.17 2005/05/17 10:19:29 dmitryk Exp $" IPDR_COPYRIGHT_STR

#if defined(GLOBAL_RENAME)
#define TCPTransport_h_rcsid        IPDR__TCPTransport_h_rcsid
#define TCPTransport_rcsid          IPDR__TCPTransport_rcsid
#define TCP_Connect                 IPDR__TCP_Connect
#define TCP_Disconnect              IPDR__TCP_Disconnect
#define TCP_GetConnectionInfo       IPDR__TCP_GetConnectionInfo
#define TCP_GetConnectionInfoStr    IPDR__TCP_GetConnectionInfoStr
#define TCP_GetReceiveChunksManager IPDR__TCP_GetReceiveChunksManager
#define TCP_GetSendChunksManager    IPDR__TCP_GetSendChunksManager
#define TCP_HandleDisconnection     IPDR__TCP_HandleDisconnection
#define TCP_InitializePassive       IPDR__TCP_InitializePassive
#define TCP_ParseAddress            IPDR__TCP_ParseAddress
#define TCP_PrintAddress            IPDR__TCP_PrintAddress
#define TCP_PrintAddressAndPort     IPDR__TCP_PrintAddressAndPort
#define TCP_Receive                 IPDR__TCP_Receive
#define TCP_Reconfigure             IPDR__TCP_Reconfigure
#define TCP_ResetConnection         IPDR__TCP_ResetConnection
#define TCP_Send                    IPDR__TCP_Send
#define TCP_SetConnectionHandlers   IPDR__TCP_SetConnectionHandlers
#define TCP_Shutdown                IPDR__TCP_Shutdown
#endif

#include <IPDR/Defs.h>
#include <Common/Transport.h>
#include <Common/ParametersHandler.h>
#include <Common/ConfigurationNames.h>
#include <Common/ChunksManager.h>

#ifdef __cplusplus
extern "C" {
#endif

#if 0
} /* put this here to un-confuse Emacs's automatic indentation */
#endif

#define TCP_TRUNSPORT_MAX_MSG	1024*1024*32

typedef struct
{
    BufferHandle StartBuffer;
    unsigned int Size;
}
TCPSendBuffer;

/**
 * structure containing information about
 * a connection made using the TCP transport.
 */
typedef struct IPDR_tag_TCPConnection
{
    char eyecatcher[8]; /**< "TCPCONN" */
    IPDR_IPAddress RemoteAddr; /**< remote ip address */
    unsigned short RemotePort;  /**< remote port */
    IPDR_IPAddress LocaleAddr; /**< Locale ip address */
    unsigned short LocalePort;  /**< Locale port */

	IPDR_IPAddress DummyAddr; /**< Protocol Dummy address */

    IPDR_SocketHandle FD;            /**< socket file descriptor */
    IPDR_SocketEventID ReadEventID;  /**< handle to socket read event handler */
    IPDR_SocketEventID WriteEventID; /**< handle to socket write event handler */

    struct IPDR_tag_ConnectionHandlers *Handlers; /**< callbacks associated with the connection */
    void *Arg;                                     /**< argument passed to callbacks */
	void *Owner;

    struct IPDR_tag_TCPConnection *NextConnection; /**< next connection in list */
    struct IPDR_tag_TCPTransport *Transport; /**< transport associated with the connection */

    ChunksManager ReceivedData; /**< manager holding references to received data */
    ChunksManager SentData;     /**< chunk manager for sent data */
    ChunksManager SendBuffers;  /**< queue of buffers to be sent */

    IPDR_TimerID ConnectionTimer; /**< connection timer (for an
                                    *   active connection */
    IPDR_TimerID FlushTimer; /**< timer used for flushing pending messages */

    IPDR_BOOL Disconnected;  /**< true if the connection is manually disconnected */

    int InUseCount; /**< Incremented each time a function needs the
                     *   connection ...  we need this because there
                     *   can be both a receive and a send happening at
                     *   the same time on the same connection; if an
                     *   error occurs we have to wait until both have
                     *   stopped using the connection before we can
                     *   destroy it they'll both see the error, so
                     *   it's OK for the 2nd one to destroy the
                     *   connection.  InUseCount is 0 when nobody is
                     *   using the connection and it's safe to
                     *   delete. This means that the InUseCount must
                     *   decremented before calling
                     *   TCP_HandleDisconnection()
                     */

    char descrStr[256]; /**< Used by TCP_GetConnectionInfoStr */
}
TCPConnection;

/**
 * structure containing information about
 * IPDR's TCP based transport layer.
 * This is *only* for passive ("accept") sockets.
 */
typedef struct IPDR_tag_TCPTransport
{
    IPDR_PlatformServices *PS; /**< platform abstraction layer */
    ChunkAllocator RcvAllocator; /**< allocator for allocating
                                  *   received messages */
    struct IPDR_tag_Log *Logger;

    TCPConnection *Connections; /**< list of connections */
    struct IPDR_tag_ConnectionHandlers *DefaultHandlers; /**< default handlers used by
                                                           *   passive connections */
    void *DefaultArg; /**< argument passed to default handlers */

    IPDR_SocketHandle SocketFD; /**< socket FD for passive socket */
    IPDR_SocketEventID AcceptEventID;  /**< read event handler ID for accept */

    IPDR_IPAddress LocalAddr; /**< local address of the transport */
    unsigned short LocalPort;  /**< local port number */

    ParametersHandler ParamsHandler;

    int MinReconnectInterval;
    int MaxReconnectInterval;
    int FlushInterval;
    int ConnectionTimeout;
}
TCPTransport;



IPDR_Status TCP_InitializePassive(IPDR_PlatformServices *PS,
                                   Log *Logger,
                                   ChunkAllocator RcvAllocator,
                                   IPDR_IPAddress LocalAddr,
                                   unsigned short LocalPort,
                                   struct IPDR_tag_ConnectionHandlers *DefaultHandlers,
                                   void *Arg,
                                   ParametersHandler *ParamsHandler,
                                   Transport *Handle);

IPDR_Status TCP_Shutdown(Transport Handle);

IPDR_Status TCP_Disconnect(Connection C); /* For tear-down of a connection ... for errors, use TCP_HandleDisconnection */

void TCP_HandleDisconnection(Connection C); /* When error is detected, use this to reset the connection */

IPDR_Status TCP_Connect(IPDR_PlatformServices *PS,
						TCPConnection *Conn);

IPDR_Status TCP_GetSockName(IPDR_PlatformServices *PS,
							TCPConnection *Conn);

IPDR_Status TCP_CreateConnectionIdle(TCPTransport *T,
									IPDR_IPAddress LocalAddr,
									IPDR_IPAddress RemoteAddr,
									unsigned short RemotePort,
									TCPConnection **Conn);

IPDR_Status TCP_Send(Connection C,
                      BufferHandle *BH, IPDR_BOOL Immediate);

IPDR_Status TCP_Receive(TCPConnection *C, int Timeout,
                         BufferHandle *BH);

IPDR_Status TCP_ResetConnection(Connection C);

IPDR_Status TCP_SetConnectionHandlers(Connection C,
                                       struct IPDR_tag_ConnectionHandlers *Handlers,
                                       void *Args);

IPDR_Status TCP_GetConnectionInfo(Connection C,
                                   IPDR_IPAddress *Addr, unsigned short *Port);

const char* TCP_GetConnectionInfoStr(Connection C);

IPDR_Status TCP_GetReceiveChunksManager(Connection C,
                                         ChunksManager **Mgr);

IPDR_Status TCP_GetSendChunksManager(Connection C,
                                      ChunksManager **Mgr);

IPDR_Status TCP_Reconfigure(Transport Handle);

char *TCP_PrintAddress(IPDR_IPAddress Addr, char *AddressStr);
char *TCP_PrintAddressAndPort(IPDR_IPAddress Addr, unsigned int Port, char *AddressStr);
char *TCP_PrintConnectionRemote(Connection C, char *AddressStr);
char *TCP_PrintConnectionLocale(Connection C, char *AddressStr);
char *TCP_PrintConnectionFull(Connection C, char *AddressStr);

IPDR_IPAddress TCP_ParseAddress(const char *Str);


#ifdef __cplusplus
}
#endif

#endif
