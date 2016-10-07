/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/


/*****************************************************
 * File                 : IPDRDocWriter              *
 * Description          :                            *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 *                                                   *
 *****************************************************/

#ifndef _IPDRDOCWRITER_H
#define _IPDRDOCWRITER_H

#include <stdlib.h>

#include "utils/IPDRCommon.h"
#include "utils/errorCode.h"
#include "utils/utils.h"
#include "utils/errorHandler.h"
#include "utils/IPDRMemory.h"

#include "common/IPDRHeader.h"
#include "xdr/IPDRXDRHeaderHelper.h"
#include "common/FNFType.h"
#include "common/FNFData.h"
#include "common/schema.h"
#include "utils/schemaUtil.h"
#include "utils/hashtable.h"
#include "xdr/IPDRStreamElementHelper.h"
#include "xdr/IPDRRecord.h"
#include "xml/IPDRXMLHeaderHelper.h"
#include "xml/IPDRXMLRecordHelper.h"
#include "xml/IPDRXMLDocEndHelper.h"

int writeHeader(
				IPDRCommonParameters* IPDRCommonParameters, 
				hyper time, 
				char* ipdrRecorderInfo, 
				char* defaultNameSpace, 
				ListNameSpaceInfo* pOtherNameSpaces, 
				ListServiceDefinitionURI* pListServiceDefinitionURI, 
				char* docId, 
                                ListSchemaNameSpace* pListSchemaNameSpace,
				int* errorCode
				);

int createFNFData(
				  IPDRCommonParameters* IPDRCommonParameters, 
				  FNFType* pFNFType_, 
				  ListIPDRData* pListIPDRData_, 
				  FNFData *FNFData, 
				  int* errorCode
				 );

int writeIPDR(
			  IPDRCommonParameters* IPDRCommonParameters, 
			  FNFData* FNFData, 
			  int* errorCode
			  );

int writeDocEnd(
				IPDRCommonParameters* IPDRCommonParameters, 
				int* errorCode
				);

/* IPDR v3.5, change for version*/
int setVersion(float ver);

#endif



