/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/


/*****************************************************
 * File                 : IPDRXMLHeaderHelper        *
 * Description          :                            *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 * 24-Feb-02                                         *
 *****************************************************/

#include "xml/IPDRXMLHeaderHelper.h"

/*!
 *  NAME:
 *      writeIPDRXMLHeaderHelper() - writes an ipdrHeader structure to the 
 *										output stream.    
 *                     
 *  DESCRIPTION:
 *     	-This function will be used to write an ipdrHeader structure to the output 
 *       stream. 
 *		-This fxn calls write_xml Marshalling function to actually write the 
 *		 output data into XML Stream.
 *  
 *	ARGUMENTS:
 *      - IPDRCommonParameters (IN/OUT)
 *      - IPDRHeader {IN}
 *		- pErrorCode {IN / OUT } 
 *
 *  RETURNS 	
 *	    Returns Zero or appropriate error code
*/


int writeIPDRXMLHeaderHelper(
						  IPDRCommonParameters* pIPDRCommonParameters, 
						  IPDRHeader* pIPDRHeader, 
						  int* pErrorCode
						 )	
{
	int count = 0;
	int errorSeverity = 0;
	int otherNameSpaceInfoLength = 0;
	int serviceDefinitionURILength = 0;
	int schemaNameSpaceLength = 0;

	char *tempXMLString = NULL;
	char *stringCreationTime = NULL;
	ListNameSpaceInfo* pTempListNameSpaceInfo = NULL;
	ListSchemaNameSpace* pFirstListSchemaNameSpace = NULL;


	if(pIPDRHeader == NULL || pIPDRCommonParameters == NULL) {
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		return(IPDR_ERROR);
	}

	tempXMLString = (char*) calloc(MAX_XML_STRING_LENGTH, sizeof(char));
	stringCreationTime = (char*) calloc(MAX_XML_STRING_LENGTH, sizeof(char));

	if(tempXMLString == NULL || stringCreationTime == NULL) { 
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		free(tempXMLString);
		free(stringCreationTime);
		return(IPDR_ERROR);
	}

	/* COMPACT_IPDR_VERSION is the hard coded pIPDRHeader */

	sprintf(tempXMLString, "%s\n", "<?xml version=\"1.0\"?>");
	write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
	
	sprintf(tempXMLString, "<IPDRDoc xmlns=\"%s\"\n", pIPDRHeader->defaultNameSpaceURI_);
	write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);

if(pIPDRHeader->pOtherNameSpaceInfo_ != NULL) {	
	otherNameSpaceInfoLength = getLengthListNameSpaceInfo(pIPDRHeader->pOtherNameSpaceInfo_);
	pTempListNameSpaceInfo = pIPDRHeader->pOtherNameSpaceInfo_;  

	for(count = 0; count < otherNameSpaceInfoLength; count++ ) {

	if (strcmp(pIPDRHeader->defaultNameSpaceURI_, pIPDRHeader->pOtherNameSpaceInfo_->NameSpaceInfo_.nameSpaceURI_)) {
	sprintf(tempXMLString, "xmlns:%s=\"%s\"\n", 
		pIPDRHeader->pOtherNameSpaceInfo_->NameSpaceInfo_.nameSpaceID_, 
		pIPDRHeader->pOtherNameSpaceInfo_->NameSpaceInfo_.nameSpaceURI_);

		write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
	}
	pIPDRHeader->pOtherNameSpaceInfo_ = pIPDRHeader->pOtherNameSpaceInfo_->pNext_;  
	}
}

	/*sprintf(tempXMLString, "%s\n", "xmlns:xsi=\"http://www.w3.org/2000/10/XMLSchema-instance\"");*/
	sprintf(tempXMLString, "%s\n", "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"");
	write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
	

	if(pIPDRHeader->pListSchemaNameSpace_ != NULL){
		
		pFirstListSchemaNameSpace = pIPDRHeader->pListSchemaNameSpace_;
		schemaNameSpaceLength = getLengthListSchemaNameSpace(pIPDRHeader->pListSchemaNameSpace_);  
		
		sprintf(tempXMLString, "%s", "xsi:schemaLocation=\"");
        write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
		for(count = 0; count <= schemaNameSpaceLength; count++) {
			if(count != schemaNameSpaceLength) {
				/*sprintf(tempXMLString, "%s %s \n", 
					pIPDRHeader->pListSchemaNameSpace_->pSchemaNameSpace_,
					pIPDRHeader->pListServiceDefinitionURI_->pServiceDefintionURI_);*/
				sprintf(tempXMLString, "%s %s", 
					pIPDRHeader->pListSchemaNameSpace_->pSchemaNameSpace_,
					pIPDRHeader->pListServiceDefinitionURI_->pServiceDefintionURI_);
				
				write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
				pIPDRHeader->pListServiceDefinitionURI_ = pIPDRHeader->pListServiceDefinitionURI_->pNext_;  
				pIPDRHeader->pListSchemaNameSpace_ =  pIPDRHeader->pListSchemaNameSpace_->pNext_;
			} else {
				sprintf(tempXMLString, "%s \n", "\"");
				write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
			}
		}
		pIPDRHeader->pListSchemaNameSpace_ = pFirstListSchemaNameSpace;
	} else {

		serviceDefinitionURILength = 
			getLengthListServiceDefinitionURI(pIPDRHeader->pListServiceDefinitionURI_);

		sprintf(tempXMLString, "%s", "xsi:noNamespaceSchemaLocation=\"");
        write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
		
		for(count = 0; count <= serviceDefinitionURILength; count++) {
			if(count != serviceDefinitionURILength) {
				/*sprintf(tempXMLString, "%s \n", 
					pIPDRHeader->pListServiceDefinitionURI_->pServiceDefintionURI_);  */
				sprintf(tempXMLString, "%s", 
					pIPDRHeader->pListServiceDefinitionURI_->pServiceDefintionURI_);  
				
				write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
				pIPDRHeader->pListServiceDefinitionURI_ = pIPDRHeader->pListServiceDefinitionURI_->pNext_;  
   			} else {
				sprintf(tempXMLString, "%s \n", "\"");
				write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
			}
		}
	}
	
	sprintf(tempXMLString, "docId=\"%s\"\n", pIPDRHeader->docID_);
	write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);

	errorSeverity = convLongToTimeHeader(&pIPDRHeader->startTime_, stringCreationTime, pErrorCode);
	if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLHeaderHelper"); 
	}
	else if (errorSeverity == IPDR_ERROR) {
		pIPDRCommonParameters->DocState_ = NONWRITABLE;
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLHeaderHelper"); 
		free(tempXMLString);
		free(stringCreationTime);
		return (IPDR_ERROR);
	}

	sprintf(tempXMLString, "creationTime=\"%s\"\n", stringCreationTime);
	write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);

	sprintf(tempXMLString, "IPDRRecorderInfo=\"%s\"\n", pIPDRHeader->ipdrRecorderInfo_);
	write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);

	sprintf(tempXMLString, "version=\"%d.%d\">\n", pIPDRHeader->majorVersion_, pIPDRHeader->minorVersion_);
	write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);

	free(tempXMLString);
	free(stringCreationTime);

	return (IPDR_OK);	
}



/*!
 *  NAME:
 *      readIPDRXMLHeaderHelper() - 	This function reads the IPDR Doc header 
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


int readIPDRXMLHeaderHelper(
						 IPDRCommonParameters* pIPDRCommonParameters, 
						 IPDRHeader *pIPDRHeader, 
						 int* pErrorCode
						   )	
{	
	int errorSeverity = 0;
	
	if(pIPDRHeader == NULL || pIPDRCommonParameters == NULL) {
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		return(IPDR_ERROR);
	}

	errorSeverity = readXMLStreamData(pIPDRCommonParameters, pIPDRHeader, pErrorCode);
	if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, "readIPDRXMLHeaderHelper"); 
	}
	else if (errorSeverity == IPDR_ERROR) {
		pIPDRCommonParameters->DocState_ = NONREADABLE;
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "readIPDRXMLHeaderHelper"); 
		return (IPDR_ERROR);
	}

	if (pIPDRHeader->majorVersion_ != COMPACT_IPDR_VERSION) {
		*pErrorCode = IPDR_INVALID_VERSION;
		return(IPDR_ERROR);
    }
	
	if(pIPDRHeader->startTime_ <= 0 ) {
		*pErrorCode = IPDR_XML_NULL_TIME;
		return (IPDR_ERROR);
	}
	
	if(pIPDRHeader->defaultNameSpaceURI_ == NULL) {
		*pErrorCode = IPDR_XML_NULL_DNS;
		return (IPDR_ERROR);
	}
	
	if(pIPDRHeader->ipdrRecorderInfo_ == NULL) {
		*pErrorCode = IPDR_XML_NULL_RI;
		return (IPDR_ERROR);
	}
	if(pIPDRHeader->pListServiceDefinitionURI_ == NULL) {
		*pErrorCode = IPDR_XML_NULL_SD;
		return (IPDR_ERROR);
	}
	if(pIPDRHeader->pOtherNameSpaceInfo_ == NULL) {
		*pErrorCode = IPDR_XML_NULL_ONS;
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

	pIPDRCommonParameters->DocState_ = READABLE;

	return (IPDR_OK);	
}
