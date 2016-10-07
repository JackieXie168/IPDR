/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/



/*****************************************************
 * File                 : FNFDataHelper              *
 * Description          :                            *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 *                                                   *
 *****************************************************/

#ifndef _FNFDATAHELPER_H
#define _FNFDATAHELPER_H

#include <stdio.h>
#include <string.h>
#include <sys/malloc.h>
#include <ctype.h>
#include <stdlib.h>

#include "utils/IPDRCommon.h"
#include "utils/errorCode.h"
#include "utils/utils.h"
#include "utils/errorHandler.h"
#include "utils/IPDRMemory.h"

#include "xdr/XDRMarshaller.h"
#include "xdr/XDRUnMarshaller.h"
#include "xdr/IPDRRecord.h"


int writeFNFDataHelper(
					   IPDRCommonParameters* IPDRCommonParameters,
					   FNFData* FNFData,
					   int* errorCode
					  );

int readFNFDataHelper(
                 IPDRCommonParameters *pIPDRCommonParameters,
                 FNFData* pFNFData,
                 int descriptorID,
                 ListAttributeDescriptor* pListServiceAttributeDescriptor,
                 int* pErrorCode
                 );


int readComplexFNFDataHelper(char* pcomplexType,
					  					IPDRCommonParameters *pIPDRCommonParameters,
					  					RecordDescriptor* pRecordDescriptor, 
										ListIPDRComplex** pListIPDRComplex,
					  					int* pErrorCode
									);


int writeIPDRXDRComplexRecordHelper(FNFData *pFNFData,
										  char *pComplexType,
										  IPDRCommonParameters* pIPDRCommonParameters,
										  int* pErrorCode
										 );

int getByteArrayLength(byte* byteArray);

#endif
