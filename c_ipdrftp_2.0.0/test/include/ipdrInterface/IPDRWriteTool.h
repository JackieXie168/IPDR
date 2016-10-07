/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/


/*****************************************************
 * File                 : IPDRWriteTool.h            *
 * Description          :                            *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 * 01/22/02					Created                  *
 *****************************************************/

#ifndef _IPDRWRITETOOL_H
#define _IPDRWRITETOOL_H

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <ctype.h>

#include "utils/IPDRMemory.h"
#include "utils/IPDRCommon.h"
#include "common/descriptor.h"
#include "utils/errorCode.h"
#include "utils/utils.h"
#include "common/IPDRDocWriter.h"
#include "common/IPDRDocReader.h"
#include "utils/UUIDUtil.h"
#include "utils/dynamicArray.h"

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>
#include "common/getFile.h"


#define MAX_REC_STRING		40000
#define MAX_STR_LENGTH		1000
#define LEN_FILE_EXT		100
#define MAX_LEN_EXTENSION	100



int writeRecords(
				 char* strTemp, 
				 IPDRCommonParameters* pIPDRCommonParameters,
				 FNFType* pFNFType,
				 ListNameSpaceInfo* pListNameSpaceInfo,
			     int* pErrorCode
			  );

int populateWriteIPDRCommonParam(
			IPDRCommonParameters* pIPDRCommonParameters,
			int* pErrorCode 
			);


int printRecords(
			List* pFileContentsList, 
			unsigned int maxFileLength, 
			unsigned int numOfFiles, 
			unsigned int robFlag,
			unsigned int recordCount,
			IPDRCommonParameters* pIPDRCommonParameters,
			ListServiceDefinitionURI* pListServiceDefinitionURI,
			int* pErrorCode 
			);

int findMaxLength(
			List* pFileList, 
			unsigned int numFiles, 
			unsigned int* maxValue,
			int* pErrorCode 
			);

int printWriteUsage(); 

int validateWriteInputParams(
				int argCount, 
				char* argValues[],
				unsigned int* numOfFiles,
				unsigned int* robFlag,
				int* recCount,
				List** pFileNamesList,
				IPDRCommonParameters* pIPDRCommonParameters,	
				int* pErrorCode
			   );

int readFileContents(
					 List** pFileList,
					 List* pFileNamesList,
					 unsigned int numOfFiles,
					 ListServiceDefinitionURI** pListServiceDefinitionURI,
					 ListNameSpaceInfo** pListNameSpaceInfo,
					 int* pErrorCode
					 );
					 
int loadProcessing(
			List* pFileContentsList, 
			unsigned int numOfFiles,
			unsigned int recordCount,
			IPDRCommonParameters* pIPDRCommonParameters,
			ListServiceDefinitionURI* pListServiceDefinitionURI,
			int* pErrorCode 
			);

int roundRobinProcessing(
			List* pFileContentsList, 
			unsigned int maxFileLength, 
			unsigned int numOfFiles,
			IPDRCommonParameters* pIPDRCommonParameters,
			ListServiceDefinitionURI* pListServiceDefinitionURI,
			int* pErrorCode 
			);

int loadAndRoundRobinProcessing(
			List* pFileContentsList, 
			unsigned int maxFileLength, 
			unsigned int numOfFiles,
			unsigned int recordCount,
			IPDRCommonParameters* pIPDRCommonParameters,
			ListServiceDefinitionURI* pListServiceDefinitionURI,
			int* pErrorCode 
			);

int simpleProcessing(
			List* pFileContentsList, 
			int numOfFiles, 
			IPDRCommonParameters* pIPDRCommonParameters,
			ListServiceDefinitionURI* pListServiceDefinitionURI,
			int* pErrorCode 
			);
#endif


