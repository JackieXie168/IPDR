/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/



/*****************************************************
 * File                 : UUIDUtil.c                 *
 * Description          : functions for generating   *
 *                        UUID                       *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name             Change/Description    *
 *---------------------------------------------------*
 * 12/19/01  Infosys           Created               *         
 *****************************************************
 * Refernce         : UUID Code is refernced from    *
 *		    <draft-leach-uuids-guids-01.txt> *
 *****************************************************/

#include "utils/UUIDUtil.h"


/* sample uuid: "f81d4fae-7dec-11d0-a765-00a0c91e6bf6" */
void puid(uuid_t u, char* myUUID) 
{
	int nodeIndex;
	int count = 0;

	count = sprintf(myUUID, "%8.8x-%4.4x-%4.4x-%2.2x%2.2x-", u.time_low, 
		u.time_mid, u.time_hi_and_version, u.clock_seq_hi_and_reserved,
        u.clock_seq_low);
	
	for (nodeIndex = 0; nodeIndex < 6; nodeIndex++) {
	sprintf(myUUID + count + (2 * nodeIndex), "%2.2x", u.node[nodeIndex]);
	}
}

 
int getUUID(char* uuid, int* pErrorCode)
{
	uuid_t u;
	uuid_create(&u);
	puid(u, uuid);
	return (IPDR_OK);
}

