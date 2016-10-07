/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/



/*****************************************************
 * File                 : IPDRRecord                 *
 * Description          :                            *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 *                                                   *
 *****************************************************/

#ifndef _IPDRRECORD_H
#define _IPDRRECORD_H

#include "utils/IPDRCommon.h"
#include "utils/errorCode.h"
#include "utils/utils.h"
#include "utils/errorHandler.h"
#include "utils/IPDRMemory.h"

#include "utils/hashtable.h"
#include "xdr/IPDRStreamElementHelper.h"


#define MAXSIZE 100


int createDescriptor(
					 IPDRCommonParameters *IPDRCommonParameters, 
					 ListAttributeDescriptor* pListAttributeDescriptor, 
					 FNFData* pFNFData, 
					 int* errorCode
					 );	


int createIPDRRecordStructure(
							  IPDRCommonParameters* IPDRCommonParameters, 
							  int descriptorID, 
							  FNFData* FNFData, 
							  IPDRRecord* IPDRRecord, 
							  int* errorCode
							 );


int addDescriptor(
				  IPDRCommonParameters *IPDRCommonParameters, 
				  RecordDescriptor* pRecordDescriptor_, 
                  int* errorCode
				  );

int getDescriptor(
				  IPDRCommonParameters *IPDRCommonParameters, 
				  int descriptorID, 
				  RecordDescriptor* pRecordDescriptor, 
				  int* errorCode
				  );

#endif

