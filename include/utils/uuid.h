/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/


/*****************************************************
 * File                 : uuid.c                     *
 * Description          : functions for generating   *
 *                        UUID                       *
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

#ifndef WIN32
#include <netinet/in.h>
#endif

  #undef uuid_t
  typedef struct _uuid_t {
      unsigned32          time_low;
      unsigned16          time_mid;
      unsigned16          time_hi_and_version;
      unsigned8           clock_seq_hi_and_reserved;
      unsigned8           clock_seq_low;
      byte                node[6];
  } uuid_t;

  /* uuid_create -- generate a UUID */
  int uuid_create(uuid_t * uuid);

  /* uuid_create_from_name -- create a UUID using a "name"
     from a "name space" */
  void uuid_create_from_name(
    uuid_t * uuid,        /* resulting UUID */
    uuid_t nsid,          /* UUID to serve as context, so identical
                             names from different name spaces generate
                             different UUIDs */
    void * name,          /* the name from which to generate a UUID */
    int namelen           /* the length of the name */
  );

  /* uuid_compare --  Compare two UUID's "lexically" and return
          -1   u1 is lexically before u2
           0   u1 is equal to u2
           1   u1 is lexically after u2
     Note:   lexical ordering is not temporal ordering!
  */
  int uuid_compare(uuid_t *u1, uuid_t *u2);
