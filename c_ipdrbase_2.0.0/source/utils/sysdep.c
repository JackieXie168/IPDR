/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/


/**
 * File                 :sysdep.c                    
 * Description          : Contains all system calls  
                                                      
 * Author               : Infosys Tech Ltd           
 * Modification History :                            
 *---------------------------------------------------
 * Date       Name             Change/Description    
 *---------------------------------------------------
 * 12/19/01  Infosys           Created                        
 ****************************************************
 * Refernce         : UUID Code is refernced from    
 *					 <draft-leach-uuids-guids-01.txt>
 */
  #include <stdio.h>
  #include "utils/sysdep.h"

  #ifdef __hpux
       #define uuid_sysinfo  ksysinfo
  #else
       #define uuid_sysinfo  sysinfo
  #endif



 /*!
 *  NAME:
 *      get_ieee_node_identifier() - system dependent call to get IEEE node ID.
 *
 *  DESCRIPTION:
 *                1.  system dependent call to get IEEE node ID.
 *                2.  This sample implementation generates a random node ID.
 *	
 *  ARGUMENTS:
 *       -	node (IN / OUT)	
 *
 *  RETURNS 	
 *			Void 
 */

  void get_ieee_node_identifier(uuid_node_t *node) {
    char seed[16];
    FILE * fd;
    static inited = 0;
    static uuid_node_t saved_node;

    if (!inited) {
        fd = fopen("nodeid", "rb");
        if (fd) {
             fread(&saved_node, sizeof(uuid_node_t), 1, fd);
             fclose(fd);
        }
        else {
             get_random_info(seed);
             seed[0] |= 0x80;
             memcpy(&saved_node, seed, sizeof(uuid_node_t));
             fd = fopen("nodeid", "wb");
             if (fd) {
                    fwrite(&saved_node, sizeof(uuid_node_t), 1, fd);
                    fclose(fd);
             };
        };
        inited = 1;
    };

    *node = saved_node;
  }

 /*!
 *  NAME:
 *      get_system_time() - system dependent call to get the current system time.
 *
 *  DESCRIPTION:
 *                1.  system dependent call to get the current system time.
 *                2.  Returned as 100ns ticks since Oct 15, 1582, but resolution 
 *                    may be less than 100ns.
 *	
 *  ARGUMENTS:
 *       -	uuid_time (IN / OUT)	
 *
 *  RETURNS 	
 *			Void
 */

  #ifdef _WINDOWS_

  void get_system_time(uuid_time_t *uuid_time) {
    ULARGE_INTEGER time;

    GetSystemTimeAsFileTime((FILETIME *)&time);

   /*! NT keeps time in FILETIME format which is 100ns ticks since
      Jan 1, 1601.  UUIDs use time in 100ns ticks since Oct 15, 1582.
      The difference is 17 Days in Oct + 30 (Nov) + 31 (Dec)
      + 18 years and 5 leap days.
   */

      time.QuadPart +=
            (unsigned __int64) (1000*1000*10)       // seconds
          * (unsigned __int64) (60 * 60 * 24)       // days
          * (unsigned __int64) (17+30+31+365*18+5); // # of days

    *uuid_time = time.QuadPart;

  };


 /*!
 *  NAME:
 *      get_random_info() - generates random information.
 *
 *  DESCRIPTION:
 *                1.  This function generates random information.
 *	
 *  ARGUMENTS:
 *       -	seed (IN / OUT)	
 *
 *  RETURNS 	
 *			Void
 */

  void get_random_info(char seed[16]) {
    MD5_CTX c;
    typedef struct {
        MEMORYSTATUS m;
        SYSTEM_INFO s;
        FILETIME t;
        LARGE_INTEGER pc;
        DWORD tc;
        DWORD l;
        char hostname[MAX_COMPUTERNAME_LENGTH + 1];
    } randomness;
    randomness r;

    MD5Init(&c);
    /*! memory usage stats */
    GlobalMemoryStatus(&r.m);
    /*! random system stats */
    GetSystemInfo(&r.s);
    /*! 100ns resolution (nominally) time of day */
    GetSystemTimeAsFileTime(&r.t);
    /*! high resolution performance counter */
    QueryPerformanceCounter(&r.pc);
    /*! milliseconds since last boot */
    r.tc = GetTickCount();
    r.l = MAX_COMPUTERNAME_LENGTH + 1;

    GetComputerName(r.hostname, &r.l );
    MD5Update(&c, &r, sizeof(randomness));
    MD5Final(seed, &c);
  };
  #else

  
 /*!
 *  NAME:
 *      get_system_time() - system dependent call to get the current system time.
 *
 *  DESCRIPTION:
 *                1.  system dependent call to get the current system time.
 *                2.  Returned as 100ns ticks since Oct 15, 1582, but resolution 
 *                    may be less than 100ns.
 *	
 *  ARGUMENTS:
 *       -	uuid_time (IN / OUT)	
 *
 *  RETURNS 	
 *			Void
 */

  void get_system_time(uuid_time_t *uuid_time)
  {
      struct timeval tp;

      gettimeofday(&tp, (struct timezone *)0);

      /*! Offset between UUID formatted times and Unix formatted times.
         UUID UTC base time is October 15, 1582.
         Unix base time is January 1, 1970.
      */
      *uuid_time = (tp.tv_sec * 10000000) + (tp.tv_usec * 10) +
        I64(0x01B21DD213814000);
  }

 /*!
 *  NAME:
 *      get_random_info() - generates random information.
 *
 *  DESCRIPTION:
 *                1.  This function generates random information.
 *	
 *  ARGUMENTS:
 *       -	seed (IN / OUT)	
 *
 *  RETURNS 	
 *			Void
 */

  void get_random_info(char seed[16]) {
    MD5_CTX c;
    typedef struct {
        struct uuid_sysinfo s;
        struct timeval t;
        char hostname[257];
    } randomness;
    randomness r;

    MD5Init(&c);
    sysinfo(&r.s);
    gettimeofday(&r.t, (struct timezone *)0);
    gethostname(r.hostname, 256);
    MD5Update(&c, &r, sizeof(randomness));
    MD5Final(seed, &c);
  }

  #endif
