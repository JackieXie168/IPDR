/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/


/*****************************************************
 * File                 : IPDRXMLHeaderHelper        *
 * Description          :                            *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 * 24-Feb-02                                         *
 *****************************************************/

#ifndef _IPDRXMLHEADERHELPER_H
#define _IPDRXMLHEADERHELPER_H

#include "utils/IPDRCommon.h"
#include "utils/errorCode.h"
#include "utils/utils.h"
#include "utils/errorHandler.h"
#include "utils/IPDRMemory.h"

#include "xml/XMLMarshaller.h"
#include "xml/XMLUnMarshaller.h"

/*! 	 
 * This function takes Header structure as input and writes
 * its XML equivalent on output stream.
 */	

int writeIPDRXMLHeaderHelper(
						  IPDRCommonParameters* IPDRCommonParameters, 
						  IPDRHeader* IPDRHeader, 
						  int* pErrorCode
							);	

/*! 	 
 * This function reads Header structure 
 * from its XML equivalent stream.
 */	

int readIPDRXMLHeaderHelper(
						 IPDRCommonParameters* pIPDRCommonParameters, 
						 IPDRHeader *pIPDRHeader, 
						 int* pErrorCode
							);	


#endif


