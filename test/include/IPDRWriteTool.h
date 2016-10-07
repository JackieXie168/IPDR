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
#include <sys/malloc.h>
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
#include "curl/curl.h"
#include "curl/types.h"
#include "curl/easy.h"
#include "common/getFile.h"

#define MAX_REC_STRING		40000
#define MAX_STR_LENGTH		10000
#define LEN_FILE_EXT		400
#define MAX_LEN_EXTENSION	400


char* decodeEscapedStr(char* toDecode, char delim);
char* decodeSpecialChar(char* str);
char* decodeCharacter(char *string, char delim);
char* encodeEscapedStr(char* toEncode, char delim);
char* encodeCharacter(char *string, char delim);
char* encodeSpecialChar(char* str);
char* encodeStr(char* sourceString);
char* decodeStr(char* sourceString);
char* encodeStringsBetweenDelims(char *string, char startDelim, char endDelim);
char* decodeStringsBetweenDelims(char *string, char startDelim, char endDelim);
char* getStringBetweenDelims(char* string, char* startPoint, char* endPoint);

int writeRecords(
				 char* strTemp, 
				 IPDRCommonParameters* pIPDRCommonParameters,
				 FNFType* pFNFType,
				 ListNameSpaceInfo* pListNameSpaceInfo,
			     int* pErrorCode
			  );

int populateIPDRCommonParameters(
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

int printUsage(); 

int validateInputParams(
				int argCount, 
				char* argValues[],
				unsigned int* numOfFiles,
				unsigned int* robFlag,
				unsigned int* schemaValFlag, 
            unsigned int* versionFlag, 
            char* ipdrVer,
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

int generateArrayData(char* pArrayElements,
							 AttributeDescriptor* pAttributeDescriptor,
							 ListIPDRData** pListIPDRData,
							 FNFType** pTempFNFType
							);

int generateStructureData( char* pArrayElements,
									char* pcomplexType,
   								AttributeDescriptor* pAttributeDescriptor,
									ListAttributeDescriptor* pListServiceAttributeDescriptor,
									ListIPDRComplex** pListIPDRComplex,
   								ListIPDRData** pListIPDRData,
   								FNFType** pTempFNFType
								);

char* Findindex(char *cPtr, char c);
char * Findrindex (char *s, int c);

#endif


