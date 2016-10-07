/**
 * Copyright (C) 2002, The IPDR Organization, all rights reserved.
 * The use and distribution of this software is governed by the terms of
 * the license agreement which can be found in the file LICENSE.TXT at
 * the top of this source tree.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND, either express or implied.
 *
 *
 * File                 : IPDRCommon.h                
 * Description          : Contains mnemonics for error
 *                        codes                        
 * Author               : Infosys Tech Ltd            
 * Modification History :                             
 *----------------------------------------------------
 * Date       Name        Change/Description          
 *----------------------------------------------------
 * 01/29/02   Infosys      Created                    
 *
 *
 */

/*List of mnemonics for Error Codes*/

#ifndef _ERRORCODE_H_
#define _ERRORCODE_H_

#include "ftp/FtpStructures.H"

/* LIBCURL ERRORS */
#define IPDR_CURL_COULDNT_CONNECT					2000
#define IPDR_CURL_FTP_COULDNT_RETR_FILE			2001
#define IPDR_CURL_FTP_ACCESS_DENIED				2002
#define IPDR_CURL_LOCAL_FILENAME_NULL				2003
#define IPDR_CURL_LOCAL_FILENAME_BLANK				2004
#define IPDR_CURL_WRITE_ERROR						2005
#define IPDR_CURL_GENERAL_ERROR					2006
#define IPDR_CURL_NULL_URI							2007

/*Memory errors*/
#define IPDR_MALLOC_FAILED							2011
#define IPDR_GROUPID_NULL_OR_BLANK					2012
#define IPDR_CTRLFILE_DIRECTORY_NULL_OR_BLANK		2013
#define IPDR_CTRLFILE_PREFIX_NULL					2014
#define IPDR_CTRLFILE_NAMEPOLICY_NULL_OR_BLANK		2015
#define IPDR_CTRLFILE_SUFFIX_NULL					2016
#define IPDR_CAPABILITYFILE_URL_NULL_OR_BLANK		2017
#define IPDR_FUNCTION_PARAMETER_NULL				2018

/*Errors on Requirements*/
#define IPDR_INVALID_SEQUENCE_NUMBER				2021
#define IPDR_COULDNT_CREATE_NEWNODE					2022
#define IPDR_SUBSGROUP_NOT_FOUND					2023
#define IPDR_SUBSGROUP_LIST_EMPTY					2024
#define IPDR_GROUPID_NOT_UNIQUE						2025
#define IPDR_URL_INVALID							2026
#define IPDR_GROUPID_CAPABILITY_NOT_UNIQUE			2027
#define IPDR_LOCATION_URL_COEXIST					2028
#define IPDR_CTRLFILE_LOCATION_NULL					2029
#define IPDR_INVALID_CAPABILITY_XML					2030
//No error message for empty sequence number.This is not an error.
#define IPDR_EMPTY_SEQUENCE_NUMBER					2031
#define	IPDR_LOCAL_DIR_NOT_SET						2032

/*Errors on File operations*/
#define IPDR_FOPEN_FAILED							2041


/*Function declarations for errorHandling*/

int 
ftpErrorHandler(ListErrorStruct* pErrorList,
				 int errorCode,
				 char* functionName);

ListErrorStruct* createListErrorNode(int errorCode,
										char* functionName,
										char* errorMessage);

ErrorStruct* newErrorStructure(int errorCode,
								  char* functionName,
								  char* errorMessage);

ListErrorStruct* newListErrorStructure();

char* getErrorMesg(int errorCode);

int validateSubscriptionGroup(SubscriptionGroup subscriptionGroup);

int validateSubsGroupFields(char* groupId ,
							char* controlFileDirectory,
							char* controlFilePrefix,
							char* controlFileNamePolicy,
							char* controlFileSuffix);

int validateSubsGroupList(SubscriptionGroupList* pSubsGroupList);

int isGroupIdUnique(SubscriptionGroupList* const pSubsGroupList,
					char* groupId);	

int validateSequenceNbr(char* sequenceNbr);

int validateURL(char* URL);	

int freeListErrorStruct(ListErrorStruct* listError);

int printLastError(ListErrorStruct* listError);	

int printAllErrors(ListErrorStruct* listError);			  

#endif //_ERRORCODE_H_
