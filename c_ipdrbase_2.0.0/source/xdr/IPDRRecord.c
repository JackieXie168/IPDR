/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/



/*****************************************************
 * File                 : IPDRRecord                 *
 * Description          :                            *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 *                                                   *
 *****************************************************/

#include "xdr/IPDRRecord.h"

/*!
 *  NAME:
 *  createDescriptor() - This function creates descriptor according to NDM-U 3.0 
 *                       specification and adds the same in Descriptor Lookup Table. 
 *                     
 *  DESCRIPTION:
 *     	  - This function takes the set of AttributeDescriptor from 
 *          AttributeDescriptor HashTable and adds the same in Descriptor Lookup 
 *			table with new Descriptor ID (generated sequentially)
 *        - It creates Descriptor structure according to NDM-U 3.0 specifications
 *		  - This function updates the global parameter DescriptorCount
 *		  - Associate the new descriptor with the stream element
 *		  - Calls function writeIPDRStreamElementHelper
 *
 *	ARGUMENTS:
 *        -	IPDRCommonParameters {IN/OUT}
 *        - AttributeDescriptorHashTable {IN }
 *		  - pErrorCode { IN / OUT }
 *
 *  RETURNS 	
 *	      Returns Zero or appropriate error code
*/

int createDescriptor(
					 IPDRCommonParameters *pIPDRCommonParameters, 
					 ListAttributeDescriptor* pListAttributeDescriptor, 
					 FNFData* pFNFData, 
					 int* pErrorCode
					)	
{	
	int errorSeverity;
	int result = 0;
   unsigned int length = 0;
	char* tempString;
	FNFType* pFNFType; 
	ListAttributeDescriptor* pCurrent = NULL;
	IPDRStreamElement* pIPDRStreamElement = NULL;

 	
	if(pListAttributeDescriptor == NULL ||
	   pFNFData == NULL || 
	   pIPDRCommonParameters == NULL 
	  ) {
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		return (IPDR_ERROR);
	}
	
	pIPDRStreamElement = newIPDRStreamElement(STREAM_RECORD_DESCRIPTOR);
	if(pIPDRStreamElement == NULL) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		return(IPDR_ERROR);
	}

	/*
    1. This function takes the set of AttributeDescriptor from 
 	   AttributeDescriptorHashTable and adds the same in Descriptor 
       Lookup table with new Descriptor ID (generated sequentially).
    */
	

	pFNFType = newFNFType(); 
	strcpy(pFNFType->serviceType_, pFNFData->typeName_);
	pFNFType->pListAttributeDescriptor_ = pListAttributeDescriptor; 

    length = getLengthSerializeFNFType(pFNFType);
    tempString = (char *) calloc(sizeof(char), length);
	
	if(tempString == NULL ) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		return(IPDR_ERROR);
	}

	errorSeverity = serializeFNFTypeStructure(pFNFType, tempString, pErrorCode);
	if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, "serializeFNFTypeStructure");
	}
	else if (errorSeverity == IPDR_ERROR) {
		pIPDRCommonParameters->DocState_ = NONWRITABLE; 
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "serializeFNFTypeStructure");
		return (IPDR_ERROR);
	}	

	HashTablePut(pIPDRCommonParameters->pDescriptorLookupHashTable_, tempString, (void *) pIPDRCommonParameters->descriptorCount_);
	
	/*  2. It creates Descriptor structure according to NDM-U 3.0 specifications.*/
	pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pRecordDescriptor_->descriptorID_ = pIPDRCommonParameters->descriptorCount_;
	
	pCurrent = pFNFData->pListAttributeDescriptor_; 
	while(pFNFData->pListAttributeDescriptor_ != NULL){
		appendListAttributeDescriptor(&pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pRecordDescriptor_->pListAttributeDescriptor_,
										pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,   
										pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeType_,
										pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->description_,
										pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->derivedType_, 
										pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_, 
										pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->pListAttrNameList_, 
										pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->pListAttrClassList_, 
										pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->pListAttrMaxOccursList_, 
										pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ 
									);
		pFNFData->pListAttributeDescriptor_ = pFNFData->pListAttributeDescriptor_->pNext_;  
	}
	pFNFData->pListAttributeDescriptor_ = pCurrent;	
	
	pCurrent = pFNFData->pListServiceAttributeDescriptor_; 
	while(pFNFData->pListServiceAttributeDescriptor_ != NULL){
		appendListAttributeDescriptor(&pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pRecordDescriptor_->pListServiceAttributeDescriptor_,
										pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->attributeName_,   
										pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->attributeType_,
										pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->description_,
										pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->derivedType_, 
										pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->complexType_, 
										pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->pListAttrNameList_, 
										pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->pListAttrClassList_, 
										pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->pListAttrMaxOccursList_, 
										pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ 
									);
		pFNFData->pListServiceAttributeDescriptor_ = pFNFData->pListServiceAttributeDescriptor_->pNext_;  
	}
	pFNFData->pListServiceAttributeDescriptor_ = pCurrent;	

	strcpy(pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pRecordDescriptor_->typeName_, pFNFData->typeName_);   

    /* 3. This function updates the global parameter DescriptorCount */
	pIPDRCommonParameters->descriptorCount_ ++;

    /* 4. Associate the new descriptor with the stream element.*/

	pIPDRStreamElement->descriminator_ = 1;	

	/* 5. Calls function writeIPDRStreamElementHelper */	
	
	errorSeverity = writeIPDRStreamElementHelper(pIPDRCommonParameters, pIPDRStreamElement, pErrorCode);	
	if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRStreamElementHelper");
	}
	else if (errorSeverity == IPDR_ERROR) {
		pIPDRCommonParameters->DocState_ = NONWRITABLE; 
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRStreamElementHelper");
		return (IPDR_ERROR);
	}	

	free (tempString);
	freeFNFType (pFNFType); 
	freeIPDRStreamElement(pIPDRStreamElement);
	return (IPDR_OK);	
}


/*!
 *  NAME:
 *      createIPDRRecordStructure() -  Function to associate the FNFData with 
 *                                     IPDRRecord structure. 
 *                     
 *  DESCRIPTION:
 *     	  	This function takes the descriptor ID as input retrieve the set of data 
 *          value pertaining to that Descriptor ID from FNFData and populates an 
 *			IPDRRecord structure as out parameter.
 *
 *	ARGUMENTS:
 *        -	IPDRCommonParameters {IN/OUT}
 *        -	descriptorID {IN }
 *	      - FNFData {IN}
 *		  - IPDRRecord {OUT}
 *		  - errorCode { IN / OUT }
 *
 *  RETURNS 	
 *	      Returns Zero or appropriate error code
*/


int createIPDRRecordStructure(IPDRCommonParameters* pIPDRCommonParameters, 
							  int descriptorID, 
                              FNFData* pFNFData, 
							  IPDRRecord* pIPDRRecord, 
							  int* pErrorCode
							 )
{
	if(pFNFData == NULL ||
	   pIPDRRecord == NULL || 
	   pIPDRCommonParameters == NULL  
	  ) {
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		return (IPDR_ERROR);
	}

	pIPDRRecord->descriptorID_ = descriptorID; 
	pIPDRRecord->pFNFData_  = pFNFData; 
	
	return (IPDR_OK);
}


/*!
 *  NAME:
 *      addDescriptor() -  This function adds a new descriptor to the 
 *                         DescriptorLookUpHashTable (part of IPDRCommonParameter).
 *                     
 *  DESCRIPTION:
 *     	This function generates the descriptor for this type & registers it.
 *
 *	ARGUMENTS:
 *        -	IPDRCommonParameters {IN/OUT}
 *        -	RecordDescriptor (IN)
 *		  - pErrorCode { IN / OUT }
 *	      
 *  RETURNS 	
 *	      Returns Zero or appropriate error code
*/


int addDescriptor(IPDRCommonParameters *pIPDRCommonParameters, 
				  RecordDescriptor* pRecordDescriptor, 
                  int* pErrorCode)
{
	char* tempString;
	char* descriptorID;

	int errorSeverity = 0;
	int tempDescriptorID = 0;
	unsigned int length = 0;
	
	FNFType* pFNFType;
	
	if(pRecordDescriptor == NULL ||
	   pIPDRCommonParameters == NULL 
	  ) {
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		return (IPDR_ERROR);
	}

	pFNFType = newFNFType(); 
	descriptorID = (char *) calloc(sizeof(char), MAX_IPDR_STRING);
	
	if(descriptorID == NULL || pFNFType == NULL) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		return(IPDR_ERROR);
	}
	
	strcpy(pFNFType->serviceType_, pRecordDescriptor->typeName_);
	pFNFType->pListAttributeDescriptor_ = pRecordDescriptor->pListAttributeDescriptor_;

    length = getLengthSerializeFNFType(pFNFType);
    tempString = (char *) calloc(sizeof(char), length);
	
	if(tempString == NULL ) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		return(IPDR_ERROR);
	}

	errorSeverity = serializeFNFTypeStructure(pFNFType, tempString, pErrorCode);
	if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, "serializeFNFTypeStructure");
	}
	else if (errorSeverity == IPDR_ERROR) {
		pIPDRCommonParameters->DocState_ = NONREADABLE; 
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "serializeFNFTypeStructure");
		return (IPDR_ERROR);
	}	

	tempDescriptorID = pRecordDescriptor->descriptorID_;
	sprintf(descriptorID, "%d", tempDescriptorID);

	HashTablePutstr(pIPDRCommonParameters->pDescriptorLookupHashTable_, descriptorID, tempString);

	pIPDRCommonParameters->descriptorCount_ ++;
	
	free(tempString);
	free(descriptorID);
	freeFNFType(pFNFType); 

	return (IPDR_OK);
}

/*!
 *  NAME:
 *      getDescriptor() -  This function gets descriptor 
 *                         
 *                     
 *  DESCRIPTION:
 *     	This function gets descriptor.
 *
 *	ARGUMENTS:
 *        -	IPDRCommonParameters {IN/OUT}
 *		  - descriptorID { IN / OUT } 
 *        -	pRecordDescriptor (IN)
 *		  - errorCode { IN / OUT }
 *	      
 *  RETURNS 	
 *	      Returns Zero or appropriate error code
*/


int getDescriptor(
				  IPDRCommonParameters* pIPDRCommonParameters, 
				  int descriptorID, 
				  RecordDescriptor* pRecordDescriptor, 
				  int* pErrorCode
				 )
{
	char* tempString = NULL;
	char* key;
	int result = 0;
	int check = 0;
	int errorSeverity = 0;
	
	if(pRecordDescriptor == NULL ||
	   pIPDRCommonParameters == NULL 
	  ) {
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		return (IPDR_ERROR);
	}

	key = (char *) calloc(sizeof(char), MAX_ATTRIBUTE_LENGTH);
	if(key == NULL) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		return(IPDR_ERROR);
	}

	sprintf(key, "%d", descriptorID);
	
	check = HashTableContainsKey(pIPDRCommonParameters->pDescriptorLookupHashTable_, key);
	if(check = 0) {
		*pErrorCode = IPDR_NO_SUCH_DESCRIPTOR;
		return (IPDR_ERROR);
	}

	tempString = (char *) HashTableGet(pIPDRCommonParameters->pDescriptorLookupHashTable_, key);
	
	errorSeverity = deserializeFNFTypeStructure(tempString, pRecordDescriptor);
	if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, "deserializeFNFTypeStructure");
	}
	else if (errorSeverity == IPDR_ERROR) {
		pIPDRCommonParameters->DocState_ = NONREADABLE; 
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "deserializeFNFTypeStructure");
		return (IPDR_ERROR);
	}	

	if (pRecordDescriptor == NULL)
		return (IPDR_ERROR);

	pRecordDescriptor->descriptorID_ = descriptorID; 
	
	free(key);

	return (IPDR_OK);
}
