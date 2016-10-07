/** @file Defs.h
 *
 *   Generic type definitions used by the IPDR code.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: Defs.h,v 1.9 2005/01/30 15:41:23 guyg Exp $
 * 
 */

#ifndef _IPDR_DEFS_H_
#define _IPDR_DEFS_H_

#include <IPDR/Config.h>

#define IPDR_DEFS_H_RCSID "@(#) $Id: Defs.h,v 1.9 2005/01/30 15:41:23 guyg Exp $" IPDR_COPYRIGHT_STR

#define Defs_h_rcsid IPDR__Defs_h_rcsid

#ifndef _MSC_VER
#include <sys/types.h> /* work-around bug in header files */
#include <netinet/in.h>
#else
#include <winsock2.h>
#endif

typedef int   IPDR_BOOL;
typedef short IPDR_WORD;
typedef int   IPDR_DWORD;

typedef unsigned char    uint8;
typedef unsigned int     uint32;
typedef   signed int      int32;
typedef unsigned short   uint16;

#ifdef NEEDS_BOOL
/* The following are only for sun compat=4 */
typedef int bool;
#define false 0
#define true  1
#endif

#ifndef TRUE
#define TRUE  1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define IPDR_FALSE 0
#define IPDR_TRUE  1

#ifdef NEED_NET_BYTEORDER_DEFS
#include <inttypes.h>
uint32_t htonl(unint32_t hostlong);
uint16_t htons(uint16_t hostshort);
uint32_t ntohl(uint32_t netlong);
uint16_t ntohs(uint16_t netshort);
#endif

# if __BYTE_ORDER == __BIG_ENDIAN
/* The host byte order is the same as network byte order,
   so these functions are all just identity.  */
# define ntohll(x)       (x)
# define htonll(x)       (x)
# else
#  if __BYTE_ORDER == __LITTLE_ENDIAN
#   define ntohll(x)     __bswap_64 (x)
#   define htonll(x)     __bswap_64 (x)
#  endif
# endif

#define IPDR_SET_HTONS(s, v) (s).v16 = htons(v)
#define IPDR_SET_HTONL(s, v) (s).v32 = htonl(v)
#define IPDR_SET_HTONLL(s, v) (s).v64 = htonll(v)
#define IPDR_NTOHS(v) ntohs((v).v16)
#define IPDR_NTOHL(v) ntohl((v).v32)
#define IPDR_NTOHLL(v) ntohll((v).v64)


/** @defgroup IPDR_Socket Socket definitions
 *
 *  Ttransitional; see ::IPDR_SocketHandle
 *  @{
 */

/** The output format to use with printf when displaying a socket,
 *  used with #IPDR_SOCKET_DISPLAY:<br>
 *  <code>printf("socket: " IPDR_SOCKET_FMT "\\n", IPDR_SOCKET_DISPLAY(sock))</code>.
 */
#define Platform_SOCKET_FMT "%d"

/** Macro to get the contents of the socket for output using
 *  #IPDR_SOCKET_FMT:<br>
 *  <code>printf("socket: " IPDR_SOCKET_FMT "\\n", IPDR_SOCKET_DISPLAY(sock))</code>
 */
#define Platform_SOCKET_DISPLAY(sock) ((sock).socketFd)

/* @} */

#endif
