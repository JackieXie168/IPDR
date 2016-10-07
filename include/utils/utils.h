/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/



/*****************************************************
 * File                 : utils.h		     *
 * Description          : Utility Functions          *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 * 24-Feb-02   Infosys				     *
 *****************************************************/

#ifndef _UTILS_H
#define _UTILS_H

#include <ctype.h>
#include <sys/malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef WIN32
#if	HAVE_UNISTD_H
  #define uuid_t unistd_uuid_t
  #include <unistd.h>
  #undef uuid_t
#endif
//#include <unistd.h>
#endif


#include "utils/IPDRCommon.h"
#include "utils/IPDRMemory.h"
#include "utils/errorHandler.h"


#define LENGTH_UUID_STRING 36
int getLengthSerializeFNFType(FNFType* pFNFType);
int getLengthSerializeFNFData(FNFData* pFNFData);

int convByteArrayToUUID(
						unsigned char* UUID, 
						char* strUUID, 
						int* pErrorCode
					   );


int convUUIDToByteArray(
					    char* UUID, 
						unsigned char* byteUUID,
						int* pErrorCode
						);

int convLongToTime(
				   hyper* pTimeMilliSec,
				   char* timeStr,
				   int* pErrorCode
				  );


int convLongToDateTime(
					   long* pTimelong, 
					   char* timeStr, 
					   int* pErrorCode
					  );

int convIntToDateTime(
					   int* pTimelnt,
					   char* timeStr,
					   int* pErrorCode
					  );

int convDateTimeToInt(
					   char* timeStr,
					   int* pTimeMilliSec,
					   int* pErrorCode
					 );

int convTimeToLong(
				   char* timeStr,
				   hyper* pTimeMilliSec,
				   int* pErrorCode
				  );

int convDateTimeToLong(
					   char* timeStr,
					   hyper* pTimeSec,
					   int* pErrorCode
					  );


int getTime(
			hyper* time, 
			int* pErrorCode
			); 

int validateIPv4Addr(
					 char *IPAddr, 
					 int* pErrorCode
					);


int validateIPv6Addr(
					 char *IPAddr, 
					 int* pErrorCode
					);

int IsLeapYear(
			   int nYear
			  ); 


int GetDaysInMonth(
				   int month, 
				   int nYear
				   );

int subString(
			  char *source, 
			  char *dest, 
			  int start, 
			  int end
			  );


	
int convIntToIPv4Addr(
					  int IPV4, 
					  char* strIPV4, 
					  int* pErrorCode
					 );

int convIPv4AddrToInt(
					  char *strIPV4, 
					  int* addr, 
					  int* pErrorCode
					 );

int convByteArrayToIPV6Addr(
							unsigned char* byteIPV6Addr,
							char* strIPV6Addr,
							int* pErrorCode
						   );

int convIPV6AddrToByteArray(
							char* strIPV6AddrSource, 
							unsigned char* byteIPV6Addr,
							int* pErrorCode
						   );

int convLongToTimeHeader(
				   hyper* pTimeMilliSec,
				   char* timeStr,
				   int* pErrorCode
				  );

int getIPDRSubstring(
					 char* srcString, 
					 char* destString, 
					 int checkChar
					);

int isExisting(
			   char* srcString, 
			   char** characterArray
			   );


int getAttributeLength(char** OptionalAttributes);

int convAttributeTypeToTypeCode(char* attributeType);

int convTypeCodeToAttributeType(int typeCode, char* attributeType);

int convertToUpper(
				   char* inputString, 
				   char* upperCaseString
				   );

int serializeFNFTypeStructure(FNFType* pFNFType, char* result, int* pErrorCode);

int serializeFNFDataStructure(
					FNFData* pFNFData, 
					char* result, 
					int* pErrorCode
					);

int deserializeFNFTypeStructure(
					char * inputString, 
					RecordDescriptor* pRecordDescriptor
					);

long getFileLength(char* fileName);

int convertTolower(
				   char* inputString, 
				   char* lowerCaseString
				   );

int getNameSpaceID(char* service,char* key);

int getAttributeName(char* name, char* key) ;

int tokenizeKey(
				char* serviceType, 
				char* nameSpaceURI, 
				char* attributeName, 
				char* key
				);

int getNameSpaceURI( 
					ListNameSpaceInfo* pListSpaceInfo,
					char* nameSpaceID, 
					char* nameSpaceFnfURI
					);


int validateDocID(char* docID, int* pErrorCode); 

int validateAttributeName(
						 IPDRCommonParameters *IPDRCommonParameters, 
						 ListAttributeDescriptor *pListAttributeDescriptor, 
						 int* pErrorCode
						);

int convMicroTimeToLong(
				   char* timeStr,
				   hyper* pTimeMicroSec,
				   int* pErrorCode
				  );


int convIPAddrToByteArray(
					    char* strIPAddrSource, 
						unsigned char* byteIPAddr,
						int* pErrorCode
						);


int convByteArrayToIPAddr(
							unsigned char* byteIPAddr,
							char* strIPAddr,
							int* pErrorCode
							);

#endif
