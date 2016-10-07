/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/


/*****************************************************
 * File                 : IPDRReadTool.h             *
 * Description          :                            *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 * 02/18/02					Created                  *
 *****************************************************/



#ifndef _IPDR_READTOOL_H
#define _IPDR_READTOOL_H

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

#include "utils/IPDRMemory.h"
#include "common/descriptor.h"
#include "utils/errorCode.h"
#include "utils/utils.h"
#include "common/IPDRDocWriter.h"
#include "common/IPDRDocReader.h"
#include "utils/UUIDUtil.h"
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>
#include "common/getFile.h"


#define MAX_STR_LENGTH		1000

/* Structures for the list of service information */
typedef struct ServiceInfo
{
	FILE* fp;
	FNFType* pFNFType_;
	ListNameSpaceInfo* pListNameSpaceInfo_;
} ServiceInfo;


typedef struct ListServiceInfo
{
	ServiceInfo* pServiceInfo_;
	struct ListServiceInfo* pNext_;
} ListServiceInfo;



/* Functions for using the list of service information */
int appendListServiceInfo(
						 ListServiceInfo** pHeadRef, 
						 FILE* fp,
						 FNFType* pFNFType,
						 ListNameSpaceInfo* pListNameSpaceInfo
						 );

int addListServiceInfo(ListServiceInfo** pHeadRef, 
					  FILE* fp, 
					  FNFType* pFNFType,
					  ListNameSpaceInfo* pListNameSpaceInfo
					  );
int printListServiceInfo(ListServiceInfo* pListServiceInfo);

ServiceInfo* newServiceInfo(void);

int freeListServiceInfo(ListServiceInfo **pHeadRef);
int freeServiceInfo(ServiceInfo* ServiceInfo);

int generateRawData(
					IPDRCommonParameters* pIPDRCommonParameters,
					FNFData* pFNFData,
					ListServiceInfo* pListServiceInfo,
					int* pErrorCode
					);


int validateReadInputParams(
						int argCount, 
						char* argValues[],
						unsigned int* schemaValFlag,
						unsigned int* outDirFlag,
						char* outFileDir,
						IPDRCommonParameters* pIPDRCommonParameters,
						int* pErrorCode
					   );

int populateReadIPDRCommonParam(
								IPDRCommonParameters* pIPDRCommonParameters,
								int* pErrorCode
								);
int getLengthListServiceInfo(
							 ListServiceInfo* pListServiceInfo
							 );

int printReadUsage();


#endif


