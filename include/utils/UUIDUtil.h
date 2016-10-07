/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/



/*****************************************************
 * File                 : UUIDUtil.h                 *
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

#ifndef _UUIDUTIL_H
#define _UUIDUTIL_H

#include "sysdep.h"
#include <stdio.h>
#include "utils/uuid.h"
#include "utils/errorCode.h"

int getUUID(char* uuid, int* pErrorCode);
void puid(uuid_t u, char* myUUID); 

#endif
