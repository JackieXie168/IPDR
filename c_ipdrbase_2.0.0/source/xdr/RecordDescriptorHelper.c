/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/



/*****************************************************
 * File                 : RecordDescriptorHelper     *
 * Description          :                            *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 *                                                   *
 *****************************************************/

#include "xdr/RecordDescriptorHelper.h"

/*!
 *  NAME:
 *      writeRecordDescriptorHelper() - used to write an ipdrRecordDescriptor 
 *                                       structure to the output stream.   
 *                                        
 *                  
 *  DESCRIPTION:
 *      -This function will be used to write an ipdrRecordDescriptor structure to the
 *       output stream.
 *      -This function internally calls writeAttributeDescriptor() and the basic 
 *		 write unctions(eg write_long()) to actually marshall the data in the XDR 
 *   	 format to the output stream.
 *
 *	ARGUMENTS:
 *      - IPDRCommonParameters (IN/OUT)
 *	    - RecordDescriptor {IN}
 *      - pError Code {IN/OUT}
 *
 *  RETURNS 	
 *	      Returns Zero or appropriate error code
*/

int writeRecordDescriptorHelper(
								IPDRCommonParameters* pIPDRCommonParameters, 
								RecordDescriptor* pRecordDescriptor, 
								int* pErrorCode
							   )
{
	int count = 0;
	int errorSeverity = 0;
	int attributeDescriptorLength = 0;
	ListAttributeDescriptor* pCurrentListAttributeDescriptor = NULL;

	if(pRecordDescriptor == NULL || pIPDRCommonParameters == NULL) {
	    *pErrorCode = IPDR_NULL_INPUT_PARAMETER;
	    return (IPDR_ERROR);
	}

	write_long(pRecordDescriptor->descriptorID_, pIPDRCommonParameters->pStreamHandle_);
	
	write_long(strlen(pRecordDescriptor->typeName_), pIPDRCommonParameters->pStreamHandle_);
	write_wstring((wchar *) pRecordDescriptor->typeName_, pIPDRCommonParameters->pStreamHandle_, pIPDRCommonParameters->schemaVersion);
	
	attributeDescriptorLength = getLengthListAttributeDescriptor(pRecordDescriptor->pListAttributeDescriptor_);
	write_long(attributeDescriptorLength, pIPDRCommonParameters->pStreamHandle_);
	
	pCurrentListAttributeDescriptor = pRecordDescriptor->pListAttributeDescriptor_;
	for(count = 0; count < attributeDescriptorLength; count++) {
		
		errorSeverity =	writeAttributeDescriptorHelper(pIPDRCommonParameters, pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_, pRecordDescriptor->pListServiceAttributeDescriptor_,pErrorCode);
		if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "writeAttributeDescriptorHelper");	
		}
		else if (errorSeverity == IPDR_ERROR) {
			pIPDRCommonParameters->DocState_ = NONWRITABLE; 
			exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeAttributeDescriptorHelper");	
			return (IPDR_ERROR);
		}			

		pRecordDescriptor->pListAttributeDescriptor_ = pRecordDescriptor->pListAttributeDescriptor_->pNext_;
	}
	pRecordDescriptor->pListAttributeDescriptor_ = pCurrentListAttributeDescriptor;

	return (IPDR_OK);
}

/*!
 *  NAME:
 *      readRecordDescriptorHelper() - This function reads the Record descriptor from 
 *                                     the IPDR records
 *                                                
 *  DESCRIPTION:
 *      	This function
 *           -Reads descriptor Id
 *			 -Read number of attributes. This will denote the number of 
 *            AttributeDescriptor
 *			 -Populates the RecordDescriptor structure with the above read values
 *			 -Calls readAttributeDescriptorHelper, for reading the each of the 
 *            attribute.
 *
 *	ARGUMENTS:
 *      - IPDRCommonParameters (IN/OUT)
 *	    - RecordDescriptor (IN/OUT)
 *      - pError Code {IN/OUT}
 *
 *  RETURNS 	
 *	      Returns Zero or appropriate error code
*/

int readRecordDescriptorHelper(
							   IPDRCommonParameters* pIPDRCommonParameters, 
							   RecordDescriptor* pRecordDescriptor, 
							   int* pErrorCode
							  )
{
	int count = 0;
	int errorSeverity = 0;
	int serviceTypeLength = 0;
	int listAttributeDescriptorLength = 0;
	wchar* tmpString = NULL;

	int attributeType;

	ListAttributeDescriptor* pListAttributeDescriptor = NULL;
	ListAttributeDescriptor* pListServiceAttributeDescriptor = NULL;

	ListAttributeDescriptor* pCurrentListAttributeDescriptor = NULL;
	ListAttributeDescriptor* pCurrentListServiceAttributeDescriptor = NULL;

	AttributeDescriptor* pAttributeDescriptor;
	

	if(pRecordDescriptor == NULL || pIPDRCommonParameters == NULL) {
	    *pErrorCode = IPDR_NULL_INPUT_PARAMETER;
	    return (IPDR_ERROR);
	}
	
	pRecordDescriptor->descriptorID_ = read_long(pIPDRCommonParameters->pStreamHandle_);

	serviceTypeLength = read_long(pIPDRCommonParameters->pStreamHandle_);
	tmpString = read_wstring(serviceTypeLength, pIPDRCommonParameters->pStreamHandle_, pIPDRCommonParameters->schemaVersion);
	strcpy(pRecordDescriptor->typeName_, (char *) tmpString);
	free(tmpString);
	tmpString = NULL;
	
	listAttributeDescriptorLength = read_long(pIPDRCommonParameters->pStreamHandle_);  
	for (count = 0; count < listAttributeDescriptorLength; count++ ) {
		
	   pAttributeDescriptor = newAttributeDescriptor();
		
		if(pAttributeDescriptor == NULL) {
			*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
			return(IPDR_ERROR);
		}		
		
		errorSeverity = readAttributeDescriptorHelper(pIPDRCommonParameters, 
																	 pAttributeDescriptor, 
																	 &pListAttributeDescriptor, 
												                &pListServiceAttributeDescriptor,
												                &attributeType,
																	 pErrorCode);
		if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "readAttributeDescriptorHelper");	
		}
		else if (errorSeverity == IPDR_ERROR) {
			pIPDRCommonParameters->DocState_ = NONREADABLE; 
			exceptionHandler(pIPDRCommonParameters, pErrorCode, "readAttributeDescriptorHelper");	
			return (IPDR_ERROR);
		}

		if(attributeType == IPDRDATA_STRUCTURE) {
			pCurrentListAttributeDescriptor = pListAttributeDescriptor;
			while(pListAttributeDescriptor != NULL) {
				appendListAttributeDescriptor(
					&pRecordDescriptor->pListAttributeDescriptor_, 
					pListAttributeDescriptor->pAttributeDescriptor_->attributeName_, 
					pListAttributeDescriptor->pAttributeDescriptor_->attributeType_, 
					pListAttributeDescriptor->pAttributeDescriptor_->description_, 
					pListAttributeDescriptor->pAttributeDescriptor_->derivedType_,
					pListAttributeDescriptor->pAttributeDescriptor_->complexType_,
					pListAttributeDescriptor->pAttributeDescriptor_->pListAttrNameList_,
					pListAttributeDescriptor->pAttributeDescriptor_->pListAttrClassList_,
					pListAttributeDescriptor->pAttributeDescriptor_->pListAttrMaxOccursList_,
					pListAttributeDescriptor->pAttributeDescriptor_->noOfElements_);

				pListAttributeDescriptor = pListAttributeDescriptor->pNext_;
			}
			pListAttributeDescriptor = pCurrentListAttributeDescriptor;

			pCurrentListServiceAttributeDescriptor = pListServiceAttributeDescriptor;
			while(pListServiceAttributeDescriptor != NULL) {
				appendListAttributeDescriptor(
					&pRecordDescriptor->pListServiceAttributeDescriptor_, 
					pListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_, 
					pListServiceAttributeDescriptor->pAttributeDescriptor_->attributeType_, 
					pListServiceAttributeDescriptor->pAttributeDescriptor_->description_, 
					pListServiceAttributeDescriptor->pAttributeDescriptor_->derivedType_,
					pListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_,
					pListServiceAttributeDescriptor->pAttributeDescriptor_->pListAttrNameList_,
					pListServiceAttributeDescriptor->pAttributeDescriptor_->pListAttrClassList_,
					pListServiceAttributeDescriptor->pAttributeDescriptor_->pListAttrMaxOccursList_,
					pListServiceAttributeDescriptor->pAttributeDescriptor_->noOfElements_);

				pListServiceAttributeDescriptor = pListServiceAttributeDescriptor->pNext_;
			}
			pListServiceAttributeDescriptor = pCurrentListServiceAttributeDescriptor;
		} else {	

			appendListAttributeDescriptor(
				&pRecordDescriptor->pListAttributeDescriptor_, 
				pAttributeDescriptor->attributeName_, 
				pAttributeDescriptor->attributeType_, 
				pAttributeDescriptor->description_, 
				pAttributeDescriptor->derivedType_,
				pAttributeDescriptor->complexType_,
				pAttributeDescriptor->pListAttrNameList_,
				pAttributeDescriptor->pListAttrClassList_,
				pAttributeDescriptor->pListAttrMaxOccursList_,
				pAttributeDescriptor->noOfElements_);

			appendListAttributeDescriptor(
				&pRecordDescriptor->pListServiceAttributeDescriptor_, 
				pAttributeDescriptor->attributeName_, 
				pAttributeDescriptor->attributeType_, 
				pAttributeDescriptor->description_, 
				pAttributeDescriptor->derivedType_,
				pAttributeDescriptor->complexType_,
				pAttributeDescriptor->pListAttrNameList_,
				pAttributeDescriptor->pListAttrClassList_,
				pAttributeDescriptor->pListAttrMaxOccursList_,
				pAttributeDescriptor->noOfElements_);
		}
	        
			freeAttributeDescriptor (pAttributeDescriptor);
			pAttributeDescriptor = NULL;
			freeListAttributeDescriptor(&pListAttributeDescriptor);
			pListAttributeDescriptor = NULL;
    		freeListAttributeDescriptor(&pListServiceAttributeDescriptor);
			pListServiceAttributeDescriptor = NULL;
	}
	
	return (IPDR_OK);
}	
