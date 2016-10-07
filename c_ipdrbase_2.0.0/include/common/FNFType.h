/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/


/*****************************************************
 * File                 : FNFType                    *
 * Description          :                            *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 *                                                   *
 *****************************************************/

#ifndef _FNFTYPE_H
#define _FNFTYPE_H

#include "utils/IPDRCommon.h"
#include "utils/utils.h"
#include "utils/errorHandler.h"
#include "utils/errorCode.h"
#include "utils/IPDRMemory.h"

/*! 
 * This function checks whether Type of input Attribute Descriptor belongs to
 * basic primitive data types.	
 */
	
int FNFTypeCheck(
				 FNFType* FNFType, 
				 int* errorCode
				 );

#endif

