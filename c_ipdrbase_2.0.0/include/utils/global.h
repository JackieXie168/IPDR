/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/



/*****************************************************
 * File                 : global.c                   *
 * Description          : RSAREF types and constants *
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

/* 
 * PROTOTYPES should be set to one if and only if the 
 * compiler supports function argument prototyping.

 * The following makes PROTOTYPES default to 0 if 
 * it has not already
 */


#ifndef PROTOTYPES

#define PROTOTYPES 0

#endif

/* POINTER defines a generic pointer type */

typedef unsigned char *POINTER;

/* UINT2 defines a two byte word */

typedef unsigned short int UINT2;

/* UINT4 defines a four byte word */

typedef unsigned long int UINT4;

/* PROTO_LIST is defined depending on how PROTOTYPES is defined above.

If using PROTOTYPES, then PROTO_LIST returns the list, otherwise it
returns an empty list. 
 */

#if PROTOTYPES

#define PROTO_LIST(list) list

#else

#define PROTO_LIST(list) ()

#endif


