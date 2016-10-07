/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/



/*****************************************************
 * File                 : IPDRHeaderHelper           *
 * Description          :                            *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 *                                                   *
 *****************************************************/

#include "xdr/IPDRXDRHeaderHelper.h"
#include "common/IPDRDocWriter.h"


/*!
 *  NAME:
 *      writeIPDRXDRHeaderHelper() - writes an ipdrHeader structure to the output stream.    
 *                     
 *  DESCRIPTION:
 *     	-This function will be used to write an ipdrHeader structure to the output 
 *       stream. 
 *		-This function will internally call the function writeNameSpaceInfoHelper and 
 *       basic write functions (eg write_long()) to actually marshall the data in the 
 *   	 XDR format to the output stream.
 *  
 *	ARGUMENTS:
 *      - IPDRCommonParameters (IN/OUT)
 *      - IPDRHeader {IN}
 *		- pErrorCode {IN / OUT } 
 *
 *  RETURNS 	
 *	    Returns Zero or appropriate error code
*/


int writeIPDRXDRHeaderHelper(
						     IPDRCommonParameters* pIPDRCommonParameters, 
						     IPDRHeader* pIPDRHeader, 
						     int* pErrorCode
						    )	
{
	int count = 0;
	int errorSeverity = 0;
	int otherNameSpaceInfoLength = 0;
	int lengthDocID = 0;
	byte* byteUUID;

	if(pIPDRHeader == NULL || pIPDRCommonParameters == NULL) {
	    *pErrorCode = IPDR_NULL_INPUT_PARAMETER;
	    return (IPDR_ERROR);
	}

		/* IPDR v3.5 ,change for version*/
        
        /* Code Change to read the pIPDRCommonParameters structure member to check the schema version */
        if((strcmp(pIPDRCommonParameters->schemaVersion, IPDR_VERSION_3_1)) == 0)
	   		write_long(COMPACT_IPDR_VERSION, pIPDRCommonParameters->pStreamHandle_);	
        else if((strcmp(pIPDRCommonParameters->schemaVersion, IPDR_VERSION_3_5)) == 0)
	   		write_long(4, pIPDRCommonParameters->pStreamHandle_);
        else
           return (IPDR_ERROR);	

	
	write_long(strlen(pIPDRHeader->ipdrRecorderInfo_), pIPDRCommonParameters->pStreamHandle_);
	write_wstring((wchar *) pIPDRHeader->ipdrRecorderInfo_, pIPDRCommonParameters->pStreamHandle_, pIPDRCommonParameters->schemaVersion);
	
	write_longlong(pIPDRHeader->startTime_, pIPDRCommonParameters->pStreamHandle_);	
	
	write_long(strlen(pIPDRHeader->defaultNameSpaceURI_), pIPDRCommonParameters->pStreamHandle_);
	write_wstring((wchar *) pIPDRHeader->defaultNameSpaceURI_, pIPDRCommonParameters->pStreamHandle_, pIPDRCommonParameters->schemaVersion);

	otherNameSpaceInfoLength = getLengthListNameSpaceInfo(pIPDRHeader->pOtherNameSpaceInfo_);
	if(otherNameSpaceInfoLength == IPDR_NULL_INPUT_PARAMETER)
		write_long(0, pIPDRCommonParameters->pStreamHandle_);
	else
		write_long(otherNameSpaceInfoLength, pIPDRCommonParameters->pStreamHandle_);

if(pIPDRHeader->pOtherNameSpaceInfo_ != NULL) {	
	
	for(count = 0; count < otherNameSpaceInfoLength; count++ ) {
		
		errorSeverity = writeNameSpaceInfoHelper(pIPDRCommonParameters, &pIPDRHeader->pOtherNameSpaceInfo_->NameSpaceInfo_, pErrorCode);	
		if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "writeNameSpaceInfoHelper");	
		}
		else if (errorSeverity == IPDR_ERROR) {
			pIPDRCommonParameters->DocState_ = NONWRITABLE; 
			exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeNameSpaceInfoHelper");	
			return (IPDR_ERROR);
		}

		pIPDRHeader->pOtherNameSpaceInfo_ = pIPDRHeader->pOtherNameSpaceInfo_->pNext_;  
	}
}

	write_long(getLengthListServiceDefinitionURI(pIPDRHeader->pListServiceDefinitionURI_), pIPDRCommonParameters->pStreamHandle_);
	
	while (pIPDRHeader->pListServiceDefinitionURI_ != NULL) {
		
		write_long(strlen(pIPDRHeader->pListServiceDefinitionURI_->pServiceDefintionURI_), pIPDRCommonParameters->pStreamHandle_);    
		write_wstring((wchar *) pIPDRHeader->pListServiceDefinitionURI_->pServiceDefintionURI_, pIPDRCommonParameters->pStreamHandle_, pIPDRCommonParameters->schemaVersion);    
				
		pIPDRHeader->pListServiceDefinitionURI_ = pIPDRHeader->pListServiceDefinitionURI_->pNext_;  
	}
	
	lengthDocID = strlen(pIPDRHeader->docID_) - 4;
	write_long(lengthDocID/2 , pIPDRCommonParameters->pStreamHandle_);  	
	byteUUID = (byte* ) calloc (1, lengthDocID + 1);
	
	errorSeverity = convUUIDToByteArray(pIPDRHeader->docID_, byteUUID, pErrorCode); 
	if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, "convUUIDToByteArray"); 
	}
	else if (errorSeverity == IPDR_ERROR) {
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "convUUIDToByteArray"); 
		return (IPDR_ERROR);
	}	

	write_noctet_array(byteUUID, pIPDRCommonParameters->pStreamHandle_, lengthDocID/2, pIPDRCommonParameters->schemaVersion);	
	
	/* Indefinite length indicator */	
	write_long(IPDR_INDEFINITE_LENGTH_INDICATOR, pIPDRCommonParameters->pStreamHandle_); 
	
	free(byteUUID);
	return(IPDR_OK);	
}

/*!
 *  NAME:
 *      readIPDRXDRHeaderHelper() - 	This function reads the IPDR Doc header 
 *                     
 *  DESCRIPTION:
 *     	This function
 *          -  Reads & Validates the Version. It should not be off
 *			-  Reads ipdrRecoderInfo
 *			-  Reads startTime
 *			-  Reads defaultNameSpaceURI
 *			-  Reads the Length for otherNameSpaces
 *			-  Reads otherNameSpaces
 *			-  Reads the Length for serviceDefinitionURI
 *			-  Reads serviceDefinitionURI
 *			-  Reads the length for docId
 *			-  Reads docId
 *			-  Populates the IPDRHeader structure with the above read values.
 *
 *	ARGUMENTS:
 *        -  IPDRCommonParameters (IN/OUT)
 *	      -  IPDRHeader (IN/OUT)
 *		  -	 errorCode	(IN/OUT)
 *
 *  RETURNS 	
 *	      Returns Zero or appropriate error code
*/



int readIPDRXDRHeaderHelper(
						 IPDRCommonParameters* pIPDRCommonParameters, 
						 IPDRHeader* pIPDRHeader, 
						 int* pErrorCode
						)	
{			
	int count = 0;		
	int uriLength = 0;
	int docIDLength = 0;
	int errorSeverity = 0;
	int nameSpaceIdLength = 0;
	int ipdrCompactFormat = 0;
	int recorderInfoLength = 0;
	int nameSpaceURILength = 0;
	int serviceDefURILength = 0;
	int defaultNameSpaceLength = 0;
	int otherNameSpaceInfolength = 0;

	char* nameSpaceID;
	char* nameSpaceURI;
	char* serviceDefinitionURI;
	hyper* pTmpHyper = NULL;
	byte* byteUUID;

	wchar* tmpString = NULL;
	wchar* tmpNameSpaceID = NULL;
	wchar* tmpNameSpaceURI = NULL;
	
	if(pIPDRHeader == NULL || pIPDRCommonParameters == NULL) {
	    *pErrorCode = IPDR_NULL_INPUT_PARAMETER;
	    return (IPDR_ERROR);
	}

	nameSpaceID = (char *) calloc(sizeof(char), MAX_IPDR_STRING);
	nameSpaceURI = (char *) calloc(sizeof(char), MAX_IPDR_STRING);
	serviceDefinitionURI = (char *) calloc(sizeof(char), MAX_IPDR_STRING);
	
	if(nameSpaceID == NULL || nameSpaceURI == NULL ||
		serviceDefinitionURI == NULL) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		return(IPDR_ERROR);
	}	

	pIPDRHeader->majorVersion_ = read_long(pIPDRCommonParameters->pStreamHandle_);

	/* IPDR v3.5 ,change for version*/
        /* Code Change to read the pIPDRCommonParameters structure member to check the schema version */ 
        if(pIPDRCommonParameters->schemaVersion[0] != '\0') {
        if((strcmp(pIPDRCommonParameters->schemaVersion, IPDR_VERSION_3_1)) == 0)
		{
			if (pIPDRHeader->majorVersion_ != COMPACT_IPDR_VERSION) {
				*pErrorCode = IPDR_INVALID_VERSION;
				return (IPDR_ERROR);
			}
		}
        else if((strcmp(pIPDRCommonParameters->schemaVersion, IPDR_VERSION_3_5)) == 0)
		{
			if (pIPDRHeader->majorVersion_ != 4) {
				*pErrorCode = IPDR_INVALID_VERSION;
				return (IPDR_ERROR);
			}
		}
   	else
      	return (IPDR_ERROR);	
	} else {
			if (pIPDRHeader->majorVersion_ == COMPACT_IPDR_VERSION) {
            strcpy(pIPDRCommonParameters->schemaVersion, IPDR_VERSION_3_1);
			} else if (pIPDRHeader->majorVersion_ == 4) {
            strcpy(pIPDRCommonParameters->schemaVersion, IPDR_VERSION_3_5);
			}
	}
		
	

	pIPDRHeader->minorVersion_ = COMPACT_IPDR_VERSION_MINOR;
	
	pIPDRCommonParameters->DocState_ = READABLE;

	recorderInfoLength = read_long(pIPDRCommonParameters->pStreamHandle_);

	pIPDRHeader->ipdrRecorderInfo_ = (char *) read_wstring(recorderInfoLength, pIPDRCommonParameters->pStreamHandle_, pIPDRCommonParameters->schemaVersion);		
	
	pTmpHyper = read_longlong(pIPDRCommonParameters->pStreamHandle_);		
	pIPDRHeader->startTime_ = (hyper) *pTmpHyper;

	defaultNameSpaceLength = read_long(pIPDRCommonParameters->pStreamHandle_);
	free(pIPDRHeader->defaultNameSpaceURI_);
	pIPDRHeader->defaultNameSpaceURI_ = NULL;
	pIPDRHeader->defaultNameSpaceURI_ = (char *) read_wstring(defaultNameSpaceLength, pIPDRCommonParameters->pStreamHandle_, pIPDRCommonParameters->schemaVersion);
	
	otherNameSpaceInfolength = read_long(pIPDRCommonParameters->pStreamHandle_);

if(otherNameSpaceInfolength != 0) {
    for (count = 0; count < otherNameSpaceInfolength; count++){			

		nameSpaceIdLength =  read_long(pIPDRCommonParameters->pStreamHandle_);
		tmpNameSpaceURI = read_wstring(nameSpaceIdLength, pIPDRCommonParameters->pStreamHandle_, pIPDRCommonParameters->schemaVersion);
		nameSpaceIdLength = read_long(pIPDRCommonParameters->pStreamHandle_);
		tmpNameSpaceID = read_wstring(nameSpaceIdLength, pIPDRCommonParameters->pStreamHandle_, pIPDRCommonParameters->schemaVersion);
/*
		tmpNameSpaceID = read_wstring(nameSpaceIdLength, pIPDRCommonParameters->pStreamHandle_, pIPDRCommonParameters->schemaVersion);
		nameSpaceIdLength = read_long(pIPDRCommonParameters->pStreamHandle_);
		tmpNameSpaceURI = read_wstring(nameSpaceIdLength, pIPDRCommonParameters->pStreamHandle_, pIPDRCommonParameters->schemaVersion);
*/
		appendListNameSpaceInfo(&pIPDRHeader->pOtherNameSpaceInfo_, (char *) tmpNameSpaceID, (char *) tmpNameSpaceURI);
		free(tmpNameSpaceID);
		free(tmpNameSpaceURI);
	}
}
	serviceDefURILength = read_long(pIPDRCommonParameters->pStreamHandle_);

    for (count = 0; count < serviceDefURILength; count++){			
		
		uriLength = read_long(pIPDRCommonParameters->pStreamHandle_);
		tmpString = read_wstring(uriLength, pIPDRCommonParameters->pStreamHandle_, pIPDRCommonParameters->schemaVersion);
		appendListServiceDefinitionURI(&pIPDRHeader->pListServiceDefinitionURI_,(char *) tmpString);
		free(tmpString);
		tmpString = NULL;
	}
	
	docIDLength = read_long(pIPDRCommonParameters->pStreamHandle_);
	pIPDRHeader->docID_ = (char *) calloc (1, (2 * docIDLength) + 5);
	byteUUID = read_octet_array(docIDLength,pIPDRCommonParameters->pStreamHandle_, pIPDRCommonParameters->schemaVersion);
	
	errorSeverity = convByteArrayToUUID(byteUUID, pIPDRHeader->docID_, pErrorCode); 
	if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, "convByteArrayToUUID"); 
	}
	else if (errorSeverity == IPDR_ERROR) {
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "convByteArrayToUUID"); 
		return (IPDR_ERROR);
	}	
	
	errorSeverity = validateDocID(pIPDRHeader->docID_, pErrorCode); 
	if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, "validateDocID"); 
	}
	else if (errorSeverity == IPDR_ERROR) {
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "validateDocID"); 
		return (IPDR_ERROR);
	}	

	/*Indefinite Length Indicator*/
	ipdrCompactFormat = read_long(pIPDRCommonParameters->pStreamHandle_);
	
	free(byteUUID);
	free(nameSpaceID);
	free(nameSpaceURI);
	free(serviceDefinitionURI);
	free(pTmpHyper);

	return (IPDR_OK);	

}
