/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/


/*****************************************************
 * File                 : MD5.C - RSA Data Security  *
 * Description          : Contains all the RSA Data  *
 *                        security related functions.*
 *                                                   *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name             Change/Description    *
 *---------------------------------------------------*
 * 12/19/01  Infosys           Created               *         
 *****************************************************
 * Refernce        : MD5 Message-Digest Algorithm    *
 *                   Code is refernced from          *
 *                   http://www.rfc.net/rfc1321.html *
 *****************************************************/


/* MD5 context. */

typedef struct {
  UINT4 state[4];                                   /* state (ABCD) */
  UINT4 count[2];        /* number of bits, modulo 2^64 (lsb first) */
  unsigned char buffer[64];                         /* input buffer */

} MD5_CTX; 

void MD5Init PROTO_LIST ((MD5_CTX *));
void MD5Update PROTO_LIST
((MD5_CTX *, unsigned char *, unsigned int)); 

void MD5Final PROTO_LIST ((unsigned char [16], MD5_CTX *));
