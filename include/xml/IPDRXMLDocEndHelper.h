/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/


/*****************************************************
 * File                 : IPDRXMLDocEndHelper        *
 * Description          :                            *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 * 24-Feb-02                                         *
 *****************************************************/

#ifndef _IPDRXMLDOCENDHELPER_H
#define _IPDRXMLDOCENDHELPER_H

#include "utils/IPDRCommon.h"
#include "utils/errorCode.h"
#include "utils/utils.h"
#include "utils/errorHandler.h"
#include "utils/IPDRMemory.h"

#include "xml/XMLMarshaller.h"
#include "xml/XMLUnMarshaller.h"



/*! 	 
 * This function takes DocEnd structure as input and writes
 * its XML equivalent on output stream.
 */	

int writeIPDRXMLDocEndHelper(
						  IPDRCommonParameters* IPDRCommonParameters, 
						  DocEnd* pDocEnd, 
						  int* errorCode
						 );	

/*! 	 
 * This function will return DocEnd structure	 
 */	

int readIPDRXMLDocEndHelper(
						IPDRCommonParameters *IPDRCommonParameters, 
						DocEnd *DocEnd, 
						int* errorCode
						);	


#endif


