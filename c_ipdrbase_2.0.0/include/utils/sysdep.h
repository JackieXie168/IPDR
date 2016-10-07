/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/



/*****************************************************
 * File                 :sysdep.c                    *
 * Description          : Contains all system calls  *
 *                                                   *  
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name             Change/Description    *
 *---------------------------------------------------*
 * 12/19/01  Infosys           Created               *         
 *****************************************************
 * Refernce         : UUID Code is refernced from    *
 *					 <draft-leach-uuids-guids-01.txt>*
 *****************************************************/

#ifndef _SYSDEP_H
#define _SYSDEP_H



  #ifdef WIN32
  #include <windows.h>
  #include "utils/IPDRCommon.h"
  #else
  #include <sys/types.h>
  #include <sys/time.h>
  #include <sys/sysinfo.h>
  #include "utils/IPDRCommon.h"
  #endif

  /* change to point to where MD5 .h's live */
  /* get MD5 sample implementation from RFC 1321 */

  #include "utils/global.h"
  #include "utils/md5.h"

  /* set the following to the number of 100ns ticks of the actual
  resolution of
  your system's clock */
  #define UUIDS_PER_TICK 1024

  /* Set the following to a call to acquire a system wide global lock
  */
  #define LOCK
  #define UNLOCK

  typedef unsigned long   unsigned32;
  typedef unsigned short  unsigned16;
  typedef unsigned char   unsigned8;


 /* Set this to what your compiler uses for 64 bit data type */
  #ifdef WIN32
  #define unsigned64_t unsigned __int64
  #define I64(C) C
  #else
  #define unsigned64_t unsigned long long
  #define I64(C) C##LL
  #endif


  typedef unsigned64_t uuid_time_t;
  typedef struct {
    char nodeID[6];
  } uuid_node_t;

  void get_ieee_node_identifier(uuid_node_t *node);
  void get_system_time(uuid_time_t *uuid_time);
  void get_random_info(char seed[16]);

#endif
