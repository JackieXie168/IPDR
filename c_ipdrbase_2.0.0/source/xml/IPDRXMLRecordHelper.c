/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/


/*****************************************************
 * File                 : IPDRXMLRecordHelper        *
 * Description          :                            *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 * 24-Feb-02                                         *
 *****************************************************/


#include "common/schema.h"
#include "xml/IPDRXMLRecordHelper.h"
#include "utils/dynamicArray.h"
#include "utils/schemaUtil.h"

/*!
 *  NAME:
 *      readIPDRXMLRecordHelper() - Reads an FNFData structure from 
 *									the XML stream.    
 *                     
 *  DESCRIPTION:
 *     	-This function will be used to read an FNFData structure 
 *        
 *		-This fxn calls readXMLRecordData UnMarshalling function to 
 *		 actually read the  FNFData from XML Stream.
 *  
 *	ARGUMENTS:
 *      - IPDRCommonParameters (IN/OUT)
 *      - FNFData {IN}
 *		- pErrorCode {IN / OUT } 
 *
 *  RETURNS 	
 *	    Returns Zero or appropriate error code
*/

int readIPDRXMLRecordHelper(
							 IPDRCommonParameters* pIPDRCommonParameters, 
							 FNFData** pFNFData, 
							 int* pErrorCode
							)
{
	int FNFDataFound = 0;
	int count = 0;
	int errorSeverity = 0;
	void* IPDRValue = NULL;
	ListIPDRData* pFirst = NULL;
	ListAttributeDescriptor* pCurrent = NULL;
	ListAttributeDescriptor* pListAttributeDescriptor = NULL;


	if (pIPDRCommonParameters->pListFNFData_ == NULL) {
		freeFNFData(*pFNFData);
		*pFNFData = NULL;
		return (IPDR_OK);
	} 

	if(pFNFData == NULL || pIPDRCommonParameters == NULL) {
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
   		return(IPDR_ERROR);
	}


if((strcmp(pIPDRCommonParameters->schemaVersion,IPDR_VERSION_3_5)) == 0) {
 	if ((pIPDRCommonParameters->pSchema_->pListSchemaLookup_ == NULL)) {
		pCurrent = pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_;
		errorSeverity = populateAttributeDescriptor( 	pIPDRCommonParameters,
																			&((*pFNFData)),
																			pErrorCode);
		if (errorSeverity == IPDR_ERROR) {
				pIPDRCommonParameters->DocState_ = NONWRITABLE; 
				exceptionHandler(pIPDRCommonParameters, pErrorCode, "populateAttributeDescriptor"); 
				return (IPDR_ERROR);
		}
		
		pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_ = pCurrent;
		errorSeverity = populateIPDRData( 	pIPDRCommonParameters,
															&((*pFNFData)),
															pErrorCode);
		if (errorSeverity == IPDR_ERROR) {
				pIPDRCommonParameters->DocState_ = NONWRITABLE; 
				exceptionHandler(pIPDRCommonParameters, pErrorCode, "populateIPDRData"); 
				return (IPDR_ERROR);
		}

	} else if ((pIPDRCommonParameters->pSchema_ != NULL) && (pIPDRCommonParameters->pSchema_->pListSchemaLookup_ != NULL)) {
		pCurrent = pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_;
		errorSeverity = populateListAttributeDescriptor( 	pIPDRCommonParameters,
																			&((*pFNFData)),
																			pErrorCode);
		if (errorSeverity == IPDR_ERROR) {
				pIPDRCommonParameters->DocState_ = NONWRITABLE; 
				exceptionHandler(pIPDRCommonParameters, pErrorCode, "populateListAttributeDescriptor"); 
				return (IPDR_ERROR);
		}
		
		pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_ = pCurrent;
		errorSeverity = populateListIPDRData( 	pIPDRCommonParameters,
															&((*pFNFData)),
															pErrorCode);
		if (errorSeverity == IPDR_ERROR) {
				pIPDRCommonParameters->DocState_ = NONWRITABLE; 
				exceptionHandler(pIPDRCommonParameters, pErrorCode, "populateListIPDRData"); 
				return (IPDR_ERROR);
		}
	}
	else if (errorSeverity == IPDR_ERROR) {
		pIPDRCommonParameters->DocState_ = NONREADABLE; 
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "readIPDRXMLRecordHelper"); 
		return (IPDR_ERROR);
	}

	errorSeverity = 
			validateAttributeName(pIPDRCommonParameters, (*pFNFData)->pListAttributeDescriptor_, pErrorCode);
	if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "validateAttributeName"); 
	}
	else if (errorSeverity == IPDR_ERROR) {
			pIPDRCommonParameters->DocState_ = NONWRITABLE; 
			exceptionHandler(pIPDRCommonParameters, pErrorCode, "validateAttributeName"); 
			return (IPDR_ERROR);
	}
	freeHeadListFNFData(&pIPDRCommonParameters->pListFNFData_);

 	if ((pIPDRCommonParameters->pSchema_ != NULL) && (pIPDRCommonParameters->pSchema_->pListSchemaLookup_ != NULL)) {

		/*Need to make use of the Schema attribute types*/
		errorSeverity = getComplexAttributeTypeFromSchema(pIPDRCommonParameters,((*pFNFData)) , pErrorCode);
    	if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "readIPDRXMLRecordHelper");	
		}
		else if (errorSeverity == IPDR_ERROR) {
			pIPDRCommonParameters->DocState_ = NONREADABLE; 
			exceptionHandler(pIPDRCommonParameters, pErrorCode, "readIPDRXMLRecordHelper"); 
			return (IPDR_ERROR);
		}

		errorSeverity = convDerivedTypeToAttributeTypeFor35Version(*pFNFData, pErrorCode);
    	if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "readIPDRXMLRecordHelper");	
		}
		else if (errorSeverity == IPDR_ERROR) {
			pIPDRCommonParameters->DocState_ = NONREADABLE; 
			exceptionHandler(pIPDRCommonParameters, pErrorCode, "readIPDRXMLRecordHelper"); 
			return (IPDR_ERROR);
		}
	}
} else {

	pCurrent = pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_;

	while (pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_ != NULL) {
		
		appendListAttributeDescriptor(&((*pFNFData)->pListAttributeDescriptor_),
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeType_,
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->description_,
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->derivedType_, 
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_, 
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->pListAttrNameList_,
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->pListAttrClassList_, 
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->pListAttrMaxOccursList_, 
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_); 
	
		pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_ = pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pNext_; 

	}
	
	pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_ = pCurrent;

	errorSeverity = 
			validateAttributeName(pIPDRCommonParameters, (*pFNFData)->pListAttributeDescriptor_, pErrorCode);
	if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "validateAttributeName"); 
	}
	else if (errorSeverity == IPDR_ERROR) {
			pIPDRCommonParameters->DocState_ = NONWRITABLE; 
			exceptionHandler(pIPDRCommonParameters, pErrorCode, "validateAttributeName"); 
			return (IPDR_ERROR);
	}
	
	pFirst = pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_;

	while (pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_ != NULL) {

	if (pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->IPDRDataType_ == IPDRDATA_HEXADECARRAY) {
				
			  appendListIPDRData(&((*pFNFData)->pListIPDRData_ ),
				pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->IPDRDataUnion_.IPDRByteArray_,
							IPDRDATA_HEXADECARRAY);
		}
		else if (pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->IPDRDataType_ == IPDRDATA_DOUBLE) {
			IPDRValue = (double *) &pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->IPDRDataUnion_.IPDRDouble_;
			  appendListIPDRData(&((*pFNFData)->pListIPDRData_ ),
							IPDRValue,
							IPDRDATA_DOUBLE);
		}
		else if (pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->IPDRDataType_ == IPDRDATA_FLOAT) {
		 IPDRValue = (float *) &pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->IPDRDataUnion_.IPDRFloat_;
		  appendListIPDRData(&((*pFNFData)->pListIPDRData_ ),
							IPDRValue,
							IPDRDATA_FLOAT);
		}
		else if (pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->IPDRDataType_ == IPDRDATA_LONG) {
			appendListIPDRData(&((*pFNFData)->pListIPDRData_ ),
							(void *) pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->IPDRDataUnion_.IPDRLong_,
							IPDRDATA_LONG);
		}
		else if (pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->IPDRDataType_ == IPDRDATA_LONGLONG) {
			appendListIPDRData(&((*pFNFData)->pListIPDRData_ ),
							(void *) &(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->IPDRDataUnion_.IPDRLongLong_),
							IPDRDATA_LONGLONG);
		}
		else if (pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->IPDRDataType_ == IPDRDATA_ULONG) {
			appendListIPDRData(&((*pFNFData)->pListIPDRData_ ),
							(void *) pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->IPDRDataUnion_.IPDRULong_,
							IPDRDATA_ULONG);
		}
		else if (pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->IPDRDataType_ == IPDRDATA_ULONGLONG) {
			appendListIPDRData(&((*pFNFData)->pListIPDRData_ ),
							(void *) &(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->IPDRDataUnion_.IPDRULongLong_),
							IPDRDATA_ULONGLONG);
		}
		else if (pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->IPDRDataType_ == IPDRDATA_STRING) {
			appendListIPDRData(&((*pFNFData)->pListIPDRData_ ),
				pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_,
							IPDRDATA_STRING);
		}
		else if (pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->IPDRDataType_ == IPDRDATA_BOOLEAN) {
			appendListIPDRData(&((*pFNFData)->pListIPDRData_ ),
							pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->IPDRDataUnion_.IPDRBoolean_,
							IPDRDATA_BOOLEAN);
		}
		else if (pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->IPDRDataType_ == IPDRDATA_SHORT) {
			IPDRValue = (short int *) &pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->IPDRDataUnion_.IPDRShort_;
			appendListIPDRData(&((*pFNFData)->pListIPDRData_ ),
							IPDRValue,
							IPDRDATA_SHORT);
		}
		else if (pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->IPDRDataType_ == IPDRDATA_USHORT) {
			IPDRValue = (unsigned short int *) &pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->IPDRDataUnion_.IPDRUShort_;
			appendListIPDRData(&((*pFNFData)->pListIPDRData_ ),
							IPDRValue,
							IPDRDATA_USHORT);
		}
		else if (pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->IPDRDataType_ == IPDRDATA_BYTE) {
			appendListIPDRData(&((*pFNFData)->pListIPDRData_ ),
							(void *) pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->IPDRDataUnion_.IPDRByte_,
							IPDRDATA_BYTE);
		}
		else if (pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->IPDRDataType_ == IPDRDATA_UBYTE) {
			appendListIPDRData(&((*pFNFData)->pListIPDRData_ ),
							(void *) pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->IPDRDataUnion_.IPDRByte_,
							IPDRDATA_BYTE);
		}

		pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_ = pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->pNext_; 
	}
	
	pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_ = pFirst;
	
	
	strcpy((*pFNFData)->typeName_, pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->typeName_);
    
	freeHeadListFNFData(&pIPDRCommonParameters->pListFNFData_);


 if ((pIPDRCommonParameters->pSchema_ != NULL) && (pIPDRCommonParameters->pSchema_->pListSchemaLookup_ != NULL)) {

	errorSeverity = getAttributeTypeFromSchema(pIPDRCommonParameters, *pFNFData, pErrorCode);
    if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, "readIPDRXMLRecordHelper");	
	}
	else if (errorSeverity == IPDR_ERROR) {
		pIPDRCommonParameters->DocState_ = NONREADABLE; 
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "readIPDRXMLRecordHelper"); 
		return (IPDR_ERROR);
	}

	errorSeverity = convDerivedTypeToAttributeType(*pFNFData, pErrorCode);
    if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, "readIPDRXMLRecordHelper");	
	}
	else if (errorSeverity == IPDR_ERROR) {
		pIPDRCommonParameters->DocState_ = NONREADABLE; 
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "readIPDRXMLRecordHelper"); 
		return (IPDR_ERROR);
	}
}
}
	return (IPDR_OK);
}

int populateListAttributeDescriptor(
							 IPDRCommonParameters* pIPDRCommonParameters, 
							 FNFData** pFNFData, 
							 int* pErrorCode
							)
{
	char* pFNFtypeName = NULL;
	char* tempString = NULL;
	char* serviceTypeToken = NULL;
	char* nameSpaceFnfID = NULL;
	char* nameSpaceFnfURI = NULL;
	char* FNFAttributeName = NULL;
	char* tempFNFAttributeName = NULL;
	int errorSeverity = 0;
	int countVal = 0;
   int cntVal = 1;
	SchemaElement* pSchemaElement = NULL;
	ListAttributeDescriptor* pCurrentListAttributeDescriptor = NULL;
	ListIPDRData* pCurrentListIPDRData = NULL;

	nameSpaceFnfID = (char*) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char)); 
	nameSpaceFnfURI = (char*) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char)); 
	pFNFtypeName = (char *) calloc(strlen(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->typeName_) + 1, sizeof(char));
	tempString = (char *) calloc(strlen(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->typeName_) + 1, sizeof(char));
	if(nameSpaceFnfID == NULL || pFNFtypeName == NULL ||
		nameSpaceFnfURI == NULL || tempString == NULL) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		return(IPDR_ERROR);
	}

	strcpy(tempString, pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->typeName_);
	serviceTypeToken = strtok(tempString, ":");

	if(!strcmp(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->typeName_,serviceTypeToken)) 
		strcpy(pFNFtypeName, pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->typeName_);
	else {
		getIPDRSubstring(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->typeName_, pFNFtypeName, ':');
	}	
	free(tempString);

	strcpy((*pFNFData)->typeName_, pFNFtypeName);

	pCurrentListAttributeDescriptor = pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_;
	pCurrentListIPDRData = pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_;
	while(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_ != NULL) {
		
		errorSeverity = getNameSpaceID(nameSpaceFnfID, 
			pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);
		if(errorSeverity == IPDR_ERROR){
			*pErrorCode = IPDR_GETID_NULL;
			free(pFNFtypeName);
			free(nameSpaceFnfID);
			free(nameSpaceFnfURI);	
			return (IPDR_ERROR);
		} 
		
		errorSeverity = getNameSpaceURI(pIPDRCommonParameters->pSchema_->pListNameSpaceInfo_,
										nameSpaceFnfID, 
										nameSpaceFnfURI);
		if(errorSeverity == IPDR_ERROR){
			*pErrorCode = IPDR_GETURI_NULL;
			free(pFNFtypeName);
			free(nameSpaceFnfID);
			free(nameSpaceFnfURI);	
			return(IPDR_WARNING);
		}
		
		FNFAttributeName = (char *) calloc (strlen(pFNFtypeName) +
										   strlen(nameSpaceFnfURI) +
										   strlen (pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_) + 25, 
										   sizeof(char));
		
		tempFNFAttributeName = (char *) calloc (MAX_ATTRIBUTE_NAME * 2, sizeof(char));
	
		getIPDRSubstring(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_, tempFNFAttributeName, ':');

		sprintf(FNFAttributeName, "%s%s%s%s%s", 
								   pFNFtypeName, "$", 
								   nameSpaceFnfURI, "$", 
								   tempFNFAttributeName);								   

		pSchemaElement = newSchemaElement();
		
		errorSeverity = getSchemElementFromAttributeName(pIPDRCommonParameters,
										pSchemaElement, 
										FNFAttributeName, 
										pErrorCode);
		if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "getSchemElementFromAttributeName");	
		}
		else if (errorSeverity == IPDR_ERROR) {
			exceptionHandler(pIPDRCommonParameters, pErrorCode, "getSchemElementFromAttributeName");	
			free(pFNFtypeName);
			free(nameSpaceFnfID);
			free(nameSpaceFnfURI);	
			freeSchemaElement(pSchemaElement); 
			return (IPDR_ERROR);
		}
		
		if(pSchemaElement == NULL){
			*pErrorCode = IPDR_NO_SUCH_ATTRIBUTE_IN_SCHEMA;
			free(pFNFtypeName);
			free(nameSpaceFnfID);
			free(nameSpaceFnfURI);				
			freeSchemaElement(pSchemaElement); 
			return (IPDR_WARNING);
		}
		
		if(pSchemaElement->noOfElements_ != 0) {

			if(isPrimitiveComplexType(pSchemaElement->complexAttributeType_) == IPDR_FALSE) {
				appendListAttributeDescriptor(&((*pFNFData)->pListAttributeDescriptor_),
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeType_,
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->description_,
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->derivedType_,
										pSchemaElement->complexAttributeType_,
										pSchemaElement->pListReference_,
										pSchemaElement->pListMinOccurs_,
									   pSchemaElement->pListMaxOccurs_,		
										pSchemaElement->noOfElements_);


				appendListAttributeDescriptor(&((*pFNFData)->pListServiceAttributeDescriptor_),
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeType_,
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->description_,
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->derivedType_,
										pSchemaElement->complexAttributeType_,
										pSchemaElement->pListReference_,
										pSchemaElement->pListMinOccurs_,
									   pSchemaElement->pListMaxOccurs_,		
										pSchemaElement->noOfElements_);

				populateComplexDataType(pFNFData, pIPDRCommonParameters, pSchemaElement, 
												pSchemaElement->complexAttributeType_,pErrorCode);
				while(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_ != NULL) {
					if (strcmp(tempFNFAttributeName, pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_) != 0) {
					break;
					}

					pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_ = 
						pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pNext_; 
	 			}
			} else {
				appendListAttributeDescriptor(&((*pFNFData)->pListAttributeDescriptor_),
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeType_,
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->description_,
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->derivedType_,
										pSchemaElement->complexAttributeType_,
										pSchemaElement->pListReference_,
										pSchemaElement->pListMinOccurs_,
									   pSchemaElement->pListMaxOccurs_,		
										pSchemaElement->noOfElements_);

				appendListAttributeDescriptor(&((*pFNFData)->pListServiceAttributeDescriptor_),
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeType_,
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->description_,
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->derivedType_,
										pSchemaElement->complexAttributeType_,
										pSchemaElement->pListReference_,
										pSchemaElement->pListMinOccurs_,
									   pSchemaElement->pListMaxOccurs_,		
										pSchemaElement->noOfElements_);

			}


		while(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_ != NULL) {
			if (strcmp(tempFNFAttributeName, pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_) != 0) {
				break;
			}

			pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_ = 
				pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pNext_; 
	 	}

		freeSchemaElement(pSchemaElement); 
		if (tempFNFAttributeName != NULL) {
			free(tempFNFAttributeName);		
			tempFNFAttributeName = NULL;
		}
	} else {
			appendListAttributeDescriptor(&((*pFNFData)->pListAttributeDescriptor_),
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeType_,
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->description_,
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->derivedType_,
										pSchemaElement->complexAttributeType_,
										pSchemaElement->pListReference_,
										pSchemaElement->pListMinOccurs_,
									   pSchemaElement->pListMaxOccurs_,		
										pSchemaElement->noOfElements_);

			appendListAttributeDescriptor(&((*pFNFData)->pListServiceAttributeDescriptor_),
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeType_,
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->description_,
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->derivedType_,
										pSchemaElement->complexAttributeType_,
										pSchemaElement->pListReference_,
										pSchemaElement->pListMinOccurs_,
									   pSchemaElement->pListMaxOccurs_,		
										pSchemaElement->noOfElements_);


			if(isPrimitiveComplexType(pSchemaElement->complexAttributeType_) == IPDR_FALSE) {
				populateComplexDataType(pFNFData, pIPDRCommonParameters, pSchemaElement, 
												pSchemaElement->complexAttributeType_,pErrorCode);
			}

			pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_ = 
				pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pNext_; 

		freeSchemaElement(pSchemaElement); 
	}
	}
	pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_ = pCurrentListAttributeDescriptor;
	pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_ = pCurrentListIPDRData;

	free(pFNFtypeName);
	free(nameSpaceFnfID);
	free(nameSpaceFnfURI);
	free(tempFNFAttributeName);
	return (IPDR_OK);
}

int populateAttributeDescriptor(
							 IPDRCommonParameters* pIPDRCommonParameters, 
							 FNFData** pFNFData, 
							 int* pErrorCode
							)
{
	char* pFNFtypeName = NULL;
	char* tempString = NULL;
	char* serviceTypeToken = NULL;
	char* nameSpaceFnfID = NULL;
	char* nameSpaceFnfURI = NULL;
	char* FNFAttributeName = NULL;
	char* tempFNFAttributeName = NULL;
	int errorSeverity = 0;
	int countVal = 0;
   int cntVal = 1;
	int noOfElements = 0;
	int lCopyDataFlag = 1;
	static int lArrFndFlag = 0;
	SchemaElement* pSchemaElement = NULL;
	ListAttributeDescriptor* pCurrentListAttributeDescriptor = NULL;
	ListAttributeDescriptor* pTempListAttributeDescriptor = NULL;
	ListIPDRData* pCurrentListIPDRData = NULL;

	nameSpaceFnfID = (char*) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char)); 
	nameSpaceFnfURI = (char*) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char)); 
	pFNFtypeName = (char *) calloc(strlen(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->typeName_) + 1, sizeof(char));
	tempString = (char *) calloc(strlen(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->typeName_) + 1, sizeof(char));
	if(nameSpaceFnfID == NULL || pFNFtypeName == NULL ||
		nameSpaceFnfURI == NULL || tempString == NULL) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		return(IPDR_ERROR);
	}

	strcpy(tempString, pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->typeName_);
	serviceTypeToken = strtok(tempString, ":");

	if(!strcmp(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->typeName_,serviceTypeToken)) 
		strcpy(pFNFtypeName, pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->typeName_);
	else {
		getIPDRSubstring(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->typeName_, pFNFtypeName, ':');
	}	
	free(tempString);

	strcpy((*pFNFData)->typeName_, pFNFtypeName);

	pCurrentListAttributeDescriptor = pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_;
	pTempListAttributeDescriptor = pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_;
	pCurrentListIPDRData = pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_;
	while(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_ != NULL) {

		lCopyDataFlag = 1;
		noOfElements = 0;
		tempFNFAttributeName = (char *) calloc (MAX_ATTRIBUTE_NAME * 2, sizeof(char));
		/*getIPDRSubstring(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_, tempFNFAttributeName, ':');*/
		strcpy(tempFNFAttributeName, pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);

		while(pTempListAttributeDescriptor != NULL) {
			if (strcmp(tempFNFAttributeName, 
				pTempListAttributeDescriptor->pAttributeDescriptor_->attributeName_) != 0) {
				noOfElements = noOfElements - 1;
				break;
			} else {
				noOfElements = noOfElements + 1;
				pTempListAttributeDescriptor = pTempListAttributeDescriptor->pNext_;
				lCopyDataFlag = 0;
			}
		}

		
		if(pTempListAttributeDescriptor == NULL) {
			if(noOfElements != 0)
				noOfElements = noOfElements - 1;
		
			if(lArrFndFlag == 1) {
				noOfElements = -1;
			}
					
			if(noOfElements > 0) {
				noOfElements = -1;
				lArrFndFlag = 1;
			}
		}
		
		if(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->pListAttrNameList_ != NULL) {
   if(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_ == NULL) {
		pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_ = (char *) calloc (MAX_ATTRIBUTE_TYPE, sizeof(char));
	
			strcpy(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_, pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);
			strcat(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_, "Element");
			} else {
				strcpy(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_, pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);
				strcat(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_, "Element");

			}
		}

	   if ((pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_ != NULL) &&
				lCopyDataFlag == 0) {
				appendListAttributeDescriptor(&((*pFNFData)->pListAttributeDescriptor_),
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeType_,
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->description_,
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->derivedType_,
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_,
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->pListAttrNameList_,
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->pListAttrClassList_,
									   pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->pListAttrMaxOccursList_,		
										noOfElements);


				appendListAttributeDescriptor(&((*pFNFData)->pListServiceAttributeDescriptor_),
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeType_,
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->description_,
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->derivedType_,
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_,
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->pListAttrNameList_,
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->pListAttrClassList_,
									   pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->pListAttrMaxOccursList_,		
										noOfElements);
		}

		if (tempFNFAttributeName != NULL) {
			free(tempFNFAttributeName);		
			tempFNFAttributeName = NULL;
		}

		if(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->pListAttrNameList_ != NULL) {
			populateComplexType(pFNFData, pIPDRCommonParameters,  
									  pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_,pErrorCode);
		}

		if(pTempListAttributeDescriptor == NULL)
			break;

			pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_ = 
			pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pNext_; 
	}
	pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_ = pCurrentListAttributeDescriptor;
	pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_ = pCurrentListIPDRData;

	free(pFNFtypeName);
	free(nameSpaceFnfID);
	free(nameSpaceFnfURI);

	return (IPDR_OK);
}

int populateComplexDataType(
							 FNFData** pFNFData,
							 IPDRCommonParameters* pIPDRCommonParameters, 
							 SchemaElement* pSchemaElement,
							 char* pComplexAttributeName,
							 int *pErrorCode 
)
{

	int errorSeverity = 0;
	char* pFNFtypeName = NULL;
	char* tempString = NULL;
	char* serviceTypeToken = NULL;
	char* nameSpaceFnfID = NULL;
	char* nameSpaceFnfURI = NULL;
	char* FNFAttributeName = NULL;
	char* tempFNFAttributeName = NULL;
	SchemaElement* plSchemaElement = NULL;
	ListAttributeDescriptor* pCurrentListAttributeDescriptor = NULL;

	nameSpaceFnfID = (char*) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char)); 
	nameSpaceFnfURI = (char*) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char)); 
	pFNFtypeName = (char *) calloc(strlen(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->typeName_) + 1, sizeof(char));
	tempString = (char *) calloc(strlen(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->typeName_) + 1, sizeof(char));
	if(nameSpaceFnfID == NULL || pFNFtypeName == NULL ||
		nameSpaceFnfURI == NULL || tempString == NULL) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		return(IPDR_ERROR);
	}
	
	strcpy(tempString, pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->typeName_);
	serviceTypeToken = strtok(tempString, ":");
	
	if(!strcmp(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->typeName_,serviceTypeToken)) 
		strcpy(pFNFtypeName, pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->typeName_);
	else {
		getIPDRSubstring(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->typeName_, pFNFtypeName, ':');
	}	
	free(tempString);

	pCurrentListAttributeDescriptor = pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_;
		
		errorSeverity = getNameSpaceID(nameSpaceFnfID, 
			pSchemaElement->complexAttributeType_);
		if(errorSeverity == IPDR_ERROR){
			*pErrorCode = IPDR_GETID_NULL;
			free(pFNFtypeName);
			free(nameSpaceFnfID);
			free(nameSpaceFnfURI);	
			return (IPDR_ERROR);
		} 
		
		errorSeverity = getNameSpaceURI(pIPDRCommonParameters->pSchema_->pListNameSpaceInfo_,
										nameSpaceFnfID, 
										nameSpaceFnfURI);
		if(errorSeverity == IPDR_ERROR){
			*pErrorCode = IPDR_GETURI_NULL;
			free(pFNFtypeName);
			free(nameSpaceFnfID);
			free(nameSpaceFnfURI);	
			return(IPDR_WARNING);
		}
		
		FNFAttributeName = (char *) calloc (strlen(pFNFtypeName) +
										   strlen(nameSpaceFnfURI) +
										   strlen (pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_) + 25, 
										   sizeof(char));
		
		tempFNFAttributeName = (char *) calloc (MAX_ATTRIBUTE_NAME * 2, sizeof(char));
	
		getIPDRSubstring(pComplexAttributeName, tempFNFAttributeName, ':');

		sprintf(FNFAttributeName, "%s%s%s%s%s", 
								   pFNFtypeName, "$", 
								   nameSpaceFnfURI, "$", 
								   tempFNFAttributeName);								   

		pSchemaElement = newSchemaElement();
		
		errorSeverity = getSchemElementFromAttributeName(pIPDRCommonParameters,
										pSchemaElement, 
										FNFAttributeName, 
										pErrorCode);
		if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "getSchemElementFromAttributeName");	
		}
		else if (errorSeverity == IPDR_ERROR) {
			exceptionHandler(pIPDRCommonParameters, pErrorCode, "getSchemElementFromAttributeName");	
			free(pFNFtypeName);
			free(nameSpaceFnfID);
			free(nameSpaceFnfURI);	
			freeSchemaElement(pSchemaElement); 
			return (IPDR_ERROR);
		}
		
		if(pSchemaElement == NULL){
			*pErrorCode = IPDR_NO_SUCH_ATTRIBUTE_IN_SCHEMA;
			free(pFNFtypeName);
			free(nameSpaceFnfID);
			free(nameSpaceFnfURI);				
			freeSchemaElement(pSchemaElement); 
			return (IPDR_WARNING);
		}
		
			appendListAttributeDescriptor(&((*pFNFData)->pListServiceAttributeDescriptor_),
										pSchemaElement->attributeName_,
   									"UTF8_ARRAY",
   									"OPTIONAL",
   									"UTF8_ARRAY",
										pSchemaElement->complexAttributeType_,
										pSchemaElement->pListReference_,
										pSchemaElement->pListMinOccurs_,
									   pSchemaElement->pListMaxOccurs_,		
										pSchemaElement->noOfElements_);

	while(pSchemaElement->pListReference_ != NULL) {
		
		sprintf(FNFAttributeName, "%s%s%s%s%s", 
								   pFNFtypeName, "$", 
								   nameSpaceFnfURI, "$", 
								   pSchemaElement->pListReference_->node);								   

		plSchemaElement = newSchemaElement();
		
		errorSeverity = getSchemElementFromAttributeName(pIPDRCommonParameters,
										plSchemaElement, 
										FNFAttributeName, 
										pErrorCode);
		if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "getSchemElementFromAttributeName");	
		}
		else if (errorSeverity == IPDR_ERROR) {
			exceptionHandler(pIPDRCommonParameters, pErrorCode, "getSchemElementFromAttributeName");	
			free(pFNFtypeName);
			free(nameSpaceFnfID);
			free(nameSpaceFnfURI);	
			freeSchemaElement(plSchemaElement); 
			return (IPDR_ERROR);
		}
		
		if(plSchemaElement == NULL){
			*pErrorCode = IPDR_NO_SUCH_ATTRIBUTE_IN_SCHEMA;
			free(pFNFtypeName);
			free(nameSpaceFnfID);
			free(nameSpaceFnfURI);				
			freeSchemaElement(plSchemaElement); 
			return (IPDR_WARNING);
		}
		
			appendListAttributeDescriptor(&((*pFNFData)->pListServiceAttributeDescriptor_),
										plSchemaElement->attributeName_,
   									"UTF8_ARRAY",
   									"OPTIONAL",
   									"UTF8_ARRAY",
										plSchemaElement->complexAttributeType_,
										plSchemaElement->pListReference_,
										plSchemaElement->pListMinOccurs_,
									   plSchemaElement->pListMaxOccurs_,		
										plSchemaElement->noOfElements_);

			if(isPrimitiveComplexType(plSchemaElement->complexAttributeType_) == IPDR_FALSE) {
				populateComplexDataType(pFNFData, pIPDRCommonParameters,plSchemaElement, 
												plSchemaElement->complexAttributeType_,
												pErrorCode);
			}

			pSchemaElement->pListReference_ = pSchemaElement->pListReference_->pNext;	

	}
	 
	freeSchemaElement(pSchemaElement); 
	freeSchemaElement(plSchemaElement); 
   free(nameSpaceFnfURI);
   free(nameSpaceFnfID);
	if (tempFNFAttributeName != NULL) {
		free(tempFNFAttributeName);		
		tempFNFAttributeName = NULL;
	}

	return(IPDR_OK);

}

int populateComplexType(
							 FNFData** pFNFData,
							 IPDRCommonParameters* pIPDRCommonParameters, 
							 ListAttributeDescriptor* pListAttributeDescriptor,
							 int *pErrorCode 
)
{

	int errorSeverity = 0;
	int noOfElements = 0;
	int elementCount = 0;
	static int lArrayFoundFlag = 0;

	char* pFNFtypeName = NULL;
	char* tempString = NULL;
	char* serviceTypeToken = NULL;
	char* nameSpaceFnfID = NULL;
	char* nameSpaceFnfURI = NULL;
	char* FNFAttributeName = NULL;
	char* tempFNFAttributeName = NULL;
	char lelemcount[50];
	char tempAttributeName[MAX_ATTRIBUTE_NAME];
	List* pattrNameList=NULL;
	List* pTempList=NULL;
   List* pattrMaxOccursList=NULL;
   List* pCurrentListAttrNameList=NULL;
   List* pTempCurrentListAttrNameList=NULL;
	ListAttributeDescriptor* pCurrentListAttributeDescriptor = NULL;
	ListAttributeDescriptor* pTempXMLCurrent = NULL;


	nameSpaceFnfID = (char*) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char)); 
	nameSpaceFnfURI = (char*) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char)); 
	pFNFtypeName = (char *) calloc(strlen(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->typeName_) + 1, sizeof(char));
	tempString = (char *) calloc(strlen(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->typeName_) + 1, sizeof(char));
	if(nameSpaceFnfID == NULL || pFNFtypeName == NULL ||
		nameSpaceFnfURI == NULL || tempString == NULL) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		return(IPDR_ERROR);
	}
	
	strcpy(tempString, pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->typeName_);
	serviceTypeToken = strtok(tempString, ":");
	
	if(!strcmp(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->typeName_,serviceTypeToken)) 
		strcpy(pFNFtypeName, pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->typeName_);
	else {
		getIPDRSubstring(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->typeName_, pFNFtypeName, ':');
	}	
	free(tempString);

	pCurrentListAttributeDescriptor = pListAttributeDescriptor;
		
		tempFNFAttributeName = (char *) calloc (MAX_ATTRIBUTE_NAME * 2, sizeof(char));
	
		getIPDRSubstring(pListAttributeDescriptor->pAttributeDescriptor_->attributeName_, tempFNFAttributeName, ':');

		strcpy(tempFNFAttributeName, pListAttributeDescriptor->pAttributeDescriptor_->attributeName_);
		strcat(tempFNFAttributeName, "Element");

   if(pListAttributeDescriptor->pAttributeDescriptor_->complexType_ == NULL) {
		pListAttributeDescriptor->pAttributeDescriptor_->complexType_ = (char *) calloc (MAX_ATTRIBUTE_TYPE, sizeof(char));
	
			strcpy(pListAttributeDescriptor->pAttributeDescriptor_->complexType_, tempFNFAttributeName);
			} else {
				strcpy(pListAttributeDescriptor->pAttributeDescriptor_->complexType_, tempFNFAttributeName);
			}
		

		pCurrentListAttrNameList = pListAttributeDescriptor->pAttributeDescriptor_->pListAttrNameList_;
		pTempCurrentListAttrNameList = pListAttributeDescriptor->pAttributeDescriptor_->pListAttrNameList_;
		while(pCurrentListAttrNameList != NULL) {
			while(pTempCurrentListAttrNameList != NULL) {
				strcpy(tempAttributeName, pCurrentListAttrNameList->node);
				if(strcmp(tempAttributeName, pTempCurrentListAttrNameList->node) != 0) {
					appendNode (&(pattrNameList),
									 pCurrentListAttrNameList->node);
					elementCount = elementCount - 1;
					sprintf(lelemcount, "%d", elementCount);
					appendNode (&(pattrMaxOccursList),
									 lelemcount);
					break;
				} else {
					elementCount = elementCount + 1;
					pTempCurrentListAttrNameList = pTempCurrentListAttrNameList->pNext;
				}
			}
			pCurrentListAttrNameList = pCurrentListAttrNameList->pNext;
		}

		if(pTempCurrentListAttrNameList == NULL) {
			elementCount = elementCount - 1;
			
			if(lArrayFoundFlag == 1) {
				elementCount =  -1;
			}

			if(elementCount > 0) {
				elementCount =  -1;
				lArrayFoundFlag = 1;
			}
				
				appendNode (&(pattrNameList),
									 tempAttributeName);
				sprintf(lelemcount, "%d", elementCount);
				appendNode (&(pattrMaxOccursList),
									 lelemcount);
		}

			appendListAttributeDescriptor(&((*pFNFData)->pListServiceAttributeDescriptor_),
										 tempFNFAttributeName,
   									"UTF8_ARRAY",
   									"OPTIONAL",
   									"UTF8_ARRAY",
										NULL,
										pattrNameList,
										pListAttributeDescriptor->pAttributeDescriptor_->pListAttrClassList_,
										pattrMaxOccursList,
										noOfElements);

	pTempList = pattrNameList;
	while(pattrNameList != NULL) {
		pTempXMLCurrent = pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListServiceAttributeDescriptor_;	
		while(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListServiceAttributeDescriptor_ != NULL) {
			if(strcmp(pattrNameList->node, pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListServiceAttributeDescriptor_->pAttributeDescriptor_->attributeName_) == 0) {
				if(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListServiceAttributeDescriptor_->pAttributeDescriptor_->pListAttrNameList_ != NULL) {
					
					populateComplexType(pFNFData, pIPDRCommonParameters, 
												pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListServiceAttributeDescriptor_, pErrorCode);
					appendListAttributeDescriptor(&((*pFNFData)->pListServiceAttributeDescriptor_),
										pattrNameList->node,
   									"UTF8_ARRAY",
   									"OPTIONAL",
   									"UTF8_ARRAY",
										pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListServiceAttributeDescriptor_->pAttributeDescriptor_->complexType_,
									   NULL,
										NULL,
										NULL,
										noOfElements);
				} 
				pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListServiceAttributeDescriptor_ = pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListServiceAttributeDescriptor_->pNext_;
			} else {
				break;
			}
		}	
		pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListServiceAttributeDescriptor_ = pTempXMLCurrent;

	if(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListServiceAttributeDescriptor_ != NULL) {
		if(strcmp(pattrNameList->node, pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListServiceAttributeDescriptor_->pAttributeDescriptor_->attributeName_) != 0) {
		appendListAttributeDescriptor(&((*pFNFData)->pListServiceAttributeDescriptor_),
										pattrNameList->node,
   									"UTF8_ARRAY",
   									"OPTIONAL",
   									"UTF8_ARRAY",
										NULL,
									   NULL,
										NULL,
										NULL,
										noOfElements);
		}
		} else {
		appendListAttributeDescriptor(&((*pFNFData)->pListServiceAttributeDescriptor_),
										pattrNameList->node,
   									"UTF8_ARRAY",
   									"OPTIONAL",
   									"UTF8_ARRAY",
										NULL,
									   NULL,
										NULL,
										NULL,
										noOfElements);

		}
		pattrNameList = pattrNameList->pNext;	
	}
	pattrNameList = pTempList;
	 
	if (tempFNFAttributeName != NULL) {
		free(tempFNFAttributeName);		
		tempFNFAttributeName = NULL;
	}

	if (pattrNameList != NULL) {
       freeList(&(pattrNameList));
       pattrNameList = NULL;
   }

	if (pattrMaxOccursList != NULL) {
       freeList(&(pattrMaxOccursList));
       pattrMaxOccursList = NULL;
   }


	return(IPDR_OK);

}


int populateListIPDRData( 	IPDRCommonParameters* pIPDRCommonParameters, 
							 		FNFData** pFNFData, 
							 		int* pErrorCode
							   )
{
	char* pFNFtypeName = NULL;
	char* tempString = NULL;
	char* serviceTypeToken = NULL;
	char* nameSpaceFnfID = NULL;
	char* nameSpaceFnfURI = NULL;
	char* FNFAttributeName = NULL;
	char* tempFNFAttributeName = NULL;
	int errorSeverity = 0;
	int countVal = 0;
   int cntVal = 1;
	int dataType[MAX_IPDR_STRING];
	void* dataValue[MAX_IPDR_STRING];
	SchemaElement* pSchemaElement = NULL;
	SchemaElement* plSchemaElement = NULL;
	ListAttributeDescriptor* pCurrentListAttributeDescriptor = NULL;
	ListIPDRData* pCurrentListIPDRData = NULL;
	ListIPDRComplexData* pListIPDRComplexData[MAX_IPDR_STRING];

	memset(dataValue, 0, sizeof(dataValue));
	memset(dataType, 0, sizeof(dataType));
	memset(&pListIPDRComplexData, 0, sizeof(pListIPDRComplexData));

	nameSpaceFnfID = (char*) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char)); 
	nameSpaceFnfURI = (char*) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char)); 
	pFNFtypeName = (char *) calloc(strlen(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->typeName_) + 1, sizeof(char));
	tempString = (char *) calloc(strlen(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->typeName_) + 1, sizeof(char));
	if(nameSpaceFnfID == NULL || pFNFtypeName == NULL ||
		nameSpaceFnfURI == NULL || tempString == NULL) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		return(IPDR_ERROR);
	}

	strcpy(tempString, pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->typeName_);
	serviceTypeToken = strtok(tempString, ":");

	if(!strcmp(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->typeName_,serviceTypeToken)) 
		strcpy(pFNFtypeName, pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->typeName_);
	else {
		getIPDRSubstring(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->typeName_, pFNFtypeName, ':');
	}	
	free(tempString);

	pCurrentListAttributeDescriptor = pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_;
	pCurrentListIPDRData = pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_;
	while(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_ != NULL) {
		
		errorSeverity = getNameSpaceID(nameSpaceFnfID, 
			pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);
		if(errorSeverity == IPDR_ERROR){
			*pErrorCode = IPDR_GETID_NULL;
			free(pFNFtypeName);
			free(nameSpaceFnfID);
			free(nameSpaceFnfURI);	
			return (IPDR_ERROR);
		} 
		
		errorSeverity = getNameSpaceURI(pIPDRCommonParameters->pSchema_->pListNameSpaceInfo_,
										nameSpaceFnfID, 
										nameSpaceFnfURI);
		if(errorSeverity == IPDR_ERROR){
			*pErrorCode = IPDR_GETURI_NULL;
			free(pFNFtypeName);
			free(nameSpaceFnfID);
			free(nameSpaceFnfURI);	
			return(IPDR_WARNING);
		}
		
		FNFAttributeName = (char *) calloc (strlen(pFNFtypeName) +
										   strlen(nameSpaceFnfURI) +
										   strlen (pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_) + 25, 
										   sizeof(char));
		
		tempFNFAttributeName = (char *) calloc (MAX_ATTRIBUTE_NAME * 2, sizeof(char));
	
		getIPDRSubstring(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_, tempFNFAttributeName, ':');

		sprintf(FNFAttributeName, "%s%s%s%s%s", 
								   pFNFtypeName, "$", 
								   nameSpaceFnfURI, "$", 
								   tempFNFAttributeName);								   

		pSchemaElement = newSchemaElement();
		
		errorSeverity = getSchemElementFromAttributeName(pIPDRCommonParameters,
										pSchemaElement, 
										FNFAttributeName, 
										pErrorCode);
		if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "getSchemElementFromAttributeName");	
		}
		else if (errorSeverity == IPDR_ERROR) {
			exceptionHandler(pIPDRCommonParameters, pErrorCode, "getSchemElementFromAttributeName");	
			free(pFNFtypeName);
			free(nameSpaceFnfID);
			free(nameSpaceFnfURI);	
			freeSchemaElement(pSchemaElement); 
			return (IPDR_ERROR);
		}
		
		if(pSchemaElement == NULL){
			*pErrorCode = IPDR_NO_SUCH_ATTRIBUTE_IN_SCHEMA;
			free(pFNFtypeName);
			free(nameSpaceFnfID);
			free(nameSpaceFnfURI);				
			freeSchemaElement(pSchemaElement); 
			return (IPDR_WARNING);
		}
		
		if(pSchemaElement->noOfElements_ != 0) {
			
			if(isPrimitiveComplexType(pSchemaElement->complexAttributeType_) == IPDR_FALSE) {
				countVal = 0;
				while(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_ != NULL) {
					if (strcmp(tempFNFAttributeName, pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_) != 0) {
					break;
					}
					appendListIPDRComplexData(&pListIPDRComplexData[countVal], 
													pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_, IPDRDATA_STRUCTURE);
					countVal = countVal + 1;

					pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_ = 
						pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pNext_; 

      			pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_ =
      				pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->pNext_;
	 			}
    			appendListIPDRWithComplexData(&((*pFNFData)->pListIPDRData_),pListIPDRComplexData, IPDRDATA_STRUCTURE);
			} else {

				countVal = 0;
				cntVal = 1;
      		while(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_ != NULL) {
      			if (strcmp(tempFNFAttributeName, pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_) != 0) {
      				break;
      			}

      			if (pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->IPDRDataType_ == IPDRDATA_STRING) {
      					dataValue[countVal] = (void *) calloc (1, strlen((char *)pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_) +1);
          				strcpy((char *)dataValue[countVal],(char *)pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_);		
      				dataType[cntVal] = IPDRDATA_STRING;	
      			}

      			pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_ = 
      				pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pNext_; 
      
      			pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_ =
      				pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->pNext_;
      			countVal = countVal + 1;
      			cntVal = cntVal + 1;
      	 }
      	 dataType[0] = IPDRDATA_ARRAY;
      	 appendListIPDRDataComplex(&((*pFNFData)->pListIPDRData_), dataValue, (int *)&dataType);

			freeSchemaElement(pSchemaElement); 
			if (tempFNFAttributeName != NULL) {
				free(tempFNFAttributeName);		
				tempFNFAttributeName = NULL;
			}
	 	}
	} else {

			if(isPrimitiveComplexType(pSchemaElement->complexAttributeType_) == IPDR_FALSE) {
				countVal = 0;
				appendListIPDRComplexData(&pListIPDRComplexData[countVal], 
													pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_, IPDRDATA_STRUCTURE);
    			appendListIPDRWithComplexData(&((*pFNFData)->pListIPDRData_),pListIPDRComplexData, IPDRDATA_STRUCTURE);
				pListIPDRComplexData[countVal] = NULL;
			} else {
	
				if (pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->IPDRDataType_ == IPDRDATA_STRING) {
					appendListIPDRData(&((*pFNFData)->pListIPDRData_ ),
						pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_,
								IPDRDATA_STRING);
				}
			}
			pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_ = 
				pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pNext_; 

      	pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_ =
      		pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->pNext_;
		}
	}
	pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_ = pCurrentListAttributeDescriptor;
	pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_ = pCurrentListIPDRData;

	free(pFNFtypeName);
	free(nameSpaceFnfID);
	free(nameSpaceFnfURI);
   free(tempFNFAttributeName);
}

int populateIPDRData( 	IPDRCommonParameters* pIPDRCommonParameters, 
							 	FNFData** pFNFData, 
							 	int* pErrorCode
						  )
{
	char* pFNFtypeName = NULL;
	char* tempString = NULL;
	char* serviceTypeToken = NULL;
	char* nameSpaceFnfID = NULL;
	char* nameSpaceFnfURI = NULL;
	char* FNFAttributeName = NULL;
	char* tempFNFAttributeName = NULL;
	int errorSeverity = 0;
	int countVal = 0;
   int cntVal = 1;
	int dataType[MAX_IPDR_STRING];
	void* dataValue[MAX_IPDR_STRING];
	SchemaElement* pSchemaElement = NULL;
	SchemaElement* plSchemaElement = NULL;
	ListAttributeDescriptor* pCurrentListAttributeDescriptor = NULL;
	ListAttributeDescriptor* pCurrentFNFDataAttributeDescriptor = NULL;
	ListIPDRData* pCurrentListIPDRData = NULL;
	ListIPDRComplexData* pListIPDRComplexData[MAX_IPDR_STRING];

	memset(dataValue, 0, sizeof(dataValue));
	memset(dataType, 0, sizeof(dataType));
	memset(&pListIPDRComplexData, 0, sizeof(pListIPDRComplexData));

	nameSpaceFnfID = (char*) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char)); 
	nameSpaceFnfURI = (char*) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char)); 
	pFNFtypeName = (char *) calloc(strlen(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->typeName_) + 1, sizeof(char));
	tempString = (char *) calloc(strlen(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->typeName_) + 1, sizeof(char));
	if(nameSpaceFnfID == NULL || pFNFtypeName == NULL ||
		nameSpaceFnfURI == NULL || tempString == NULL) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		return(IPDR_ERROR);
	}

	strcpy(tempString, pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->typeName_);
	serviceTypeToken = strtok(tempString, ":");

	if(!strcmp(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->typeName_,serviceTypeToken)) 
		strcpy(pFNFtypeName, pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->typeName_);
	else {
		getIPDRSubstring(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->typeName_, pFNFtypeName, ':');
	}	
	free(tempString);

	pCurrentListAttributeDescriptor = pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_;
	pCurrentListIPDRData = pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_;
	while(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_ != NULL) {
		
		tempFNFAttributeName = (char *) calloc (MAX_ATTRIBUTE_NAME * 2, sizeof(char));
	
		/*getIPDRSubstring(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_, tempFNFAttributeName, ':');*/
		strcpy(tempFNFAttributeName, pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);

		pCurrentFNFDataAttributeDescriptor = (*pFNFData)->pListAttributeDescriptor_;
		while(pCurrentFNFDataAttributeDescriptor != NULL) {
			if(strcmp(tempFNFAttributeName, pCurrentFNFDataAttributeDescriptor->pAttributeDescriptor_->attributeName_) == 0)     {
				break;
			} else {
				pCurrentFNFDataAttributeDescriptor = pCurrentFNFDataAttributeDescriptor->pNext_;
			}
		}

		if(pCurrentFNFDataAttributeDescriptor->pAttributeDescriptor_->noOfElements_ != 0) {
			
			if(isPrimitiveComplexType(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_) == IPDR_FALSE) {
				countVal = 0;
				while(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_ != NULL) {
					if (strcmp(tempFNFAttributeName, pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_) != 0) {
					break;
					}
					appendListIPDRComplexData(&pListIPDRComplexData[countVal], 
													pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_, IPDRDATA_STRUCTURE);
					countVal = countVal + 1;

					pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_ = 
						pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pNext_; 

      			pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_ =
      				pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->pNext_;
	 			}
    			appendListIPDRWithComplexData(&((*pFNFData)->pListIPDRData_),pListIPDRComplexData, IPDRDATA_STRUCTURE);
			} else {

				countVal = 0;
				cntVal = 1;
      		while(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_ != NULL) {
      			if (strcmp(tempFNFAttributeName, pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_) != 0) {
      				break;
      			}

      			if (pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->IPDRDataType_ == IPDRDATA_STRING) {
      					dataValue[countVal] = (void *) calloc (1, strlen((char *)pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_) +1);
          				strcpy((char *)dataValue[countVal],(char *)pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_);		
      				dataType[cntVal] = IPDRDATA_STRING;	
      			}

      			pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_ = 
      				pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pNext_; 
      
      			pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_ =
      				pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->pNext_;
      			countVal = countVal + 1;
      			cntVal = cntVal + 1;
      	 }
      	 dataType[0] = IPDRDATA_ARRAY;
      	 appendListIPDRDataComplex(&((*pFNFData)->pListIPDRData_), dataValue, (int *)&dataType);

			freeSchemaElement(pSchemaElement); 
			if (tempFNFAttributeName != NULL) {
				free(tempFNFAttributeName);		
				tempFNFAttributeName = NULL;
			}
	 	}
	} else {

			if(isPrimitiveComplexType(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_) == IPDR_FALSE) {
				countVal = 0;
				appendListIPDRComplexData(&pListIPDRComplexData[countVal], 
													pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_, IPDRDATA_STRUCTURE);
    			appendListIPDRWithComplexData(&((*pFNFData)->pListIPDRData_),pListIPDRComplexData, IPDRDATA_STRUCTURE);
				pListIPDRComplexData[countVal] = NULL;

			} else {
	
				if (pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->IPDRDataType_ == IPDRDATA_STRING) {
					appendListIPDRData(&((*pFNFData)->pListIPDRData_ ),
						pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_,
								IPDRDATA_STRING);
				}
			}
			pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_ = 
				pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_->pNext_; 

      	pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_ =
      		pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_->pNext_;
		}
	}
	pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_ = pCurrentListAttributeDescriptor;
	pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListIPDRData_ = pCurrentListIPDRData;

	free(pFNFtypeName);
	free(nameSpaceFnfID);
	free(nameSpaceFnfURI);
   free(tempFNFAttributeName);
}

/*!
 *  NAME:
 *      writeIPDRXMLHeaderHelper() - writes an FNFData structure to the output stream.    
 *                     
 *  DESCRIPTION:
 *     	-This function will be used to write an FNFData structure to the output 
 *       stream. 
 *		-This fxn calls write_xml Marshalling function to actually write the 
 *		 output data into XML Stream.
 *  
 *	ARGUMENTS:
 *      - IPDRCommonParameters (IN/OUT)
 *      - FNFData {IN}
 *		- pErrorCode {IN / OUT } 
 *
 *  RETURNS 	
 *	    Returns Zero or appropriate error code
*/

int writeIPDRXMLRecordHelper(
							 IPDRCommonParameters* pIPDRCommonParameters, 
							 FNFData* pFNFData, 
							 int* pErrorCode
							)
{
	int count = 0;
	int cnt = 0;
	int errorSeverity = 0;
	int attributeDescriptorLength = 0;
	int typeCode = 0;
	char *resultGet = NULL;
	char derivedType[100];
	char boolValue[10];
	char* resultString;
	char *tempXMLString = NULL;
	char *stringIPDRCreationTime = NULL;
	char* serviceTypeToken = NULL;
 	ListIPDRData* pCurrentListIPDRData = NULL;
 	ListIPDRArrayData* pCurrentListIPDRArrayData = NULL;
 	ListAttributeDescriptor* pCurrentListAttributeDescriptor = NULL;
 	ListIPDRStructureData* pCurrentListIPDRStructureData = NULL;

	
	hyper* IPDRCreationTime = (hyper *) malloc (sizeof(hyper));


	if(pFNFData == NULL || pIPDRCommonParameters == NULL) {
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		return(IPDR_ERROR);
	}

	resultGet = (char *) calloc (sizeof(char), MAX_XML_STRING_LENGTH);
	tempXMLString = (char*) calloc(MAX_XML_STRING_LENGTH, sizeof(char));
	stringIPDRCreationTime = (char*) calloc(MAX_XML_STRING_LENGTH, sizeof(char));

	if(IPDRCreationTime == NULL  || tempXMLString == NULL || stringIPDRCreationTime == NULL) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		free(resultGet);
		free(tempXMLString);
		free(IPDRCreationTime);
		free(stringIPDRCreationTime);
		return(IPDR_ERROR);
	}

	pCurrentListIPDRData = pFNFData->pListIPDRData_;

   pCurrentListAttributeDescriptor = pFNFData->pListAttributeDescriptor_;

	sprintf(tempXMLString, " <IPDR xsi:type=\"%s\">\n", pFNFData->typeName_);
	write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
	errorSeverity = getTime(IPDRCreationTime, pErrorCode);
	if (errorSeverity == IPDR_WARNING) {
	errorHandler(pIPDRCommonParameters, pErrorCode, "getTime"); 
	}
	else if (errorSeverity == IPDR_ERROR) {
		pIPDRCommonParameters->DocState_ = NONWRITABLE;
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLRecordHelper"); 
		free(resultGet);
		free(tempXMLString);
		free(IPDRCreationTime);
		free(stringIPDRCreationTime);
		return (IPDR_ERROR);
	}

	errorSeverity = convLongToTimeHeader(IPDRCreationTime, stringIPDRCreationTime, pErrorCode);
	if (errorSeverity == IPDR_WARNING) {
	errorHandler(pIPDRCommonParameters, pErrorCode, "convLongToTime"); 
	}
	else if (errorSeverity == IPDR_ERROR) {
		pIPDRCommonParameters->DocState_ = NONWRITABLE;
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLRecordHelper"); 
		free(resultGet);
		free(tempXMLString);
		free(IPDRCreationTime);
		free(stringIPDRCreationTime);
		return (IPDR_ERROR);
	}

	sprintf(tempXMLString, "<%s>%s</%s>\n", "IPDRCreationTime",
					stringIPDRCreationTime,"IPDRCreationTime");

	write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);

	sprintf(tempXMLString, "<%s>%d</%s>\n", IPDR_XML_SEQ_NUM,
				(pIPDRCommonParameters->IPDRCount_.ipdrRecordCount_),IPDR_XML_SEQ_NUM);
	write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);

	attributeDescriptorLength = getLengthListAttributeDescriptor(pFNFData->pListAttributeDescriptor_);	

	for (count = 0; count < attributeDescriptorLength; count++) {

if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_[0] != '\0') {
   char* tempComplexType; 
   char * tempString;
	tempString = (char *) calloc(strlen(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_) + 1, sizeof(char));
	tempComplexType = (char *) calloc(strlen(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_) + 1, sizeof(char));
	strcpy(tempString, pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_);
	serviceTypeToken = strtok(tempString, ":");
	
	if(strcmp(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_,serviceTypeToken)) {
		getIPDRSubstring(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_, tempComplexType, ':');
   	strcpy(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_,tempComplexType);
	}	
	free(tempString);
	free(tempComplexType);
}

	if(isPrimitiveComplexType(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_) == IPDR_FALSE)
	{
		/*Check for noOfElements and loop that many time for Array of Structures
		For unbounded... get the noOfelements from the FNFData and assign it to the descriptor noOfelements*/
		if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ == -1) {
				pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->pListIPDRStructureData_->noOfElements_;
		}

		if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
			pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->pListIPDRStructureData_->noOfElements_;
			pCurrentListIPDRStructureData = pFNFData->pListIPDRData_->pListIPDRStructureData_;
			for(cnt = 0;cnt < (pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_);
                cnt++) { 
				sprintf(tempXMLString, "<%s>\n", 
					pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);
				write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
				writeIPDRXMLComplexRecordHelper(pFNFData,
												  pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_,
												  pIPDRCommonParameters, pErrorCode);
				sprintf(tempXMLString, "</%s>\n", 
					pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);
				write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
				pFNFData->pListIPDRData_->pListIPDRStructureData_ = 
				pFNFData->pListIPDRData_->pListIPDRStructureData_->pNext_;
			}
			pFNFData->pListIPDRData_->pListIPDRStructureData_ = pCurrentListIPDRStructureData;
		}
		else {
				sprintf(tempXMLString, "<%s>\n", 
					pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);
				write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
				writeIPDRXMLComplexRecordHelper(pFNFData,
												  pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_,
												  pIPDRCommonParameters, pErrorCode);
				sprintf(tempXMLString, "</%s>\n", 
					pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);
				write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
		}
		pFNFData->pListAttributeDescriptor_ = pFNFData->pListAttributeDescriptor_->pNext_;
		pFNFData->pListIPDRData_ = pFNFData->pListIPDRData_->pNext_;
		continue;
	}
	

	typeCode = convAttributeTypeToTypeCode(
		pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeType_);

	if(!strcmp(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeType_, 
		pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->derivedType_)) {

		switch (typeCode)
		{
			case IPDRDATA_LONG:
			  if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ == -1) {
			  	pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
			  }
	
			  if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
			  	pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
				pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
				for(cnt = 0;cnt < (pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_);
                cnt++) { 
					sprintf(tempXMLString, "<%s>%d</%s>\n", 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
						pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRLong_, 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);

					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);

					pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;
				}
			  }
				else {	
					sprintf(tempXMLString, "<%s>%d</%s>\n", 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
						pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRLong_, 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);

					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
				}

                    break;

			case IPDRDATA_ULONG: 
			  if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ == -1) {
			  	pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
			  }
			  if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
			  	pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
				pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
				for(cnt = 0;cnt < (pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_);
                cnt++) { 
					sprintf(tempXMLString, "<%s>%u</%s>\n", 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
						pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRULong_, 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);

					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);

					pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;
				}
			  }
			  else {	
				sprintf(tempXMLString, "<%s>%u</%s>\n", 
					pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
					pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRULong_, 
					pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);
					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
				}

                    break;

         case IPDRDATA_LONGLONG: 
			  if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ == -1) {
			  	pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
			  }
			  if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
			  	pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
				pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
				for(cnt = 0;cnt < (pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_);
                cnt++) { 
					/*sprintf(tempXMLString, "<%s>%.lf</%s>\n", */
					sprintf(tempXMLString, "<%s>%lld</%s>\n", 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
						pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRLongLong_, 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);

					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);

					pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;
				}
			  }
			  else {	
				/*sprintf(tempXMLString, "<%s>%.lf</%s>\n", */
				sprintf(tempXMLString, "<%s>%lld</%s>\n", 
					pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
					pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRLongLong_, 
					pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);
					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
				}

                    break;

         case IPDRDATA_ULONGLONG: 
			  if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ == -1) {
			  	pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
			  }
			  if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
				pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
				for(cnt = 0;cnt < (pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_);
                cnt++) { 
					/*sprintf(tempXMLString, "<%s>%Ld</%s>\n", */
					sprintf(tempXMLString, "<%s>%llu</%s>\n", 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
						pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRULongLong_, 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);

					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);

					pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;
				}
			  }
			  else {	
				/*sprintf(tempXMLString, "<%s>%Ld</%s>\n", */
				sprintf(tempXMLString, "<%s>%llu</%s>\n", 
					pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
					pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRULongLong_, 
					pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);
					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
				}

                    break;

         case IPDRDATA_FLOAT: 
			  if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ == -1) {
			  	pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
			  }
			  if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
			  	pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
				pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
				for(cnt = 0;cnt < (pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_);
                cnt++) { 
					sprintf(tempXMLString, "<%s>%f</%s>\n", 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
						pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRFloat_, 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);

					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);

					pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;
				}
			  }
			  else {	
				sprintf(tempXMLString, "<%s>%f</%s>\n", 
					pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
					pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRFloat_, 
					pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);
					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
				}

					break;

         case IPDRDATA_DOUBLE: 
			  if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ == -1) {
			  	pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
			  }
			  if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
			  	pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
				pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
				for(cnt = 0;cnt < (pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_);
                cnt++) { 
					sprintf(tempXMLString, "<%s>%lf</%s>\n", 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
						pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRDouble_, 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);

					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);

					pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;
				}
			  }
			  else {	
				sprintf(tempXMLString, "<%s>%lf</%s>\n", 
					pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
					pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRDouble_, 
					pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);
					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
				}

					break;

         case IPDRDATA_HEXADECARRAY: 
			  if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ == -1) {
			  	pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
			  }
			  if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
			  	pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
				pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
				for(cnt = 0;cnt < (pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_);
                cnt++) { 
					sprintf(tempXMLString, "<%s>%s</%s>\n", 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
						pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRByteArray_, 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);

					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);

					pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;
				}
			  }
			  else {	
				sprintf(tempXMLString, "<%s>%s</%s>\n", 
					pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
					pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRByteArray_, 
					pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);
					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
				}

					break;

         case IPDRDATA_STRING: 
			  if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ == -1) {
			  	pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
			  }
			  if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
			  	pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
				pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
				for(cnt = 0;cnt < (pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_);
                cnt++) { 
					resultString = (char *) calloc(strlen((char *)pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_) + 10, sizeof(char));
					convToEntityReference((char *)pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_, resultString);
					
					sprintf(tempXMLString, "<%s>%s</%s>\n", 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
						resultString, 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);

					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);

					pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;
				}
			  }
			  else {	
					resultString = (char *) calloc(strlen((char *)pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_) + 10, sizeof(char));
					convToEntityReference((char *)pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, resultString);
					
					sprintf(tempXMLString, "<%s>%s</%s>\n", 
					pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
					resultString, 
					pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);
					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
					free(resultString);
				} 

					break;

         case IPDRDATA_BOOLEAN: 
			  if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ == -1) {
			  	pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
			  }
			  if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
			  	pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
				pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
				for(cnt = 0;cnt < (pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_);
                cnt++) {
					if(strcmp(pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRBoolean_, "0") == 0) {
						strcpy(boolValue, "false");
					} else if(strcmp(pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRBoolean_, "1") == 0) {
						strcpy(boolValue, "true");
					}
					/*
					sprintf(tempXMLString, "<%s>%s</%s>\n", 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
						pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRBoolean_, 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);
					*/
					sprintf(tempXMLString, "<%s>%s</%s>\n", 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
						boolValue,
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);

					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);

					pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;
				}
			  }
			  else {	
					if(strcmp(pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRBoolean_, "0") == 0) {
						strcpy(boolValue, "false");
					} else if(strcmp(pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRBoolean_, "1") == 0) {
						strcpy(boolValue, "true");
					}
					sprintf(tempXMLString, "<%s>%s</%s>\n", 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
						boolValue,
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);
					/*
					sprintf(tempXMLString, "<%s>%s</%s>\n", 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
						pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRBoolean_, 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);
					*/
					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
				}
					break;

			case IPDRDATA_SHORT: 
			  if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ == -1) {
			  	pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
			  }
			  if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
			  	pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
				pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
				for(cnt = 0;cnt < (pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_);
                cnt++) { 
					sprintf(tempXMLString, "<%s>%d</%s>\n", 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
						pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRShort_, 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);

					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);

					pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;
				}
			  }
			  else {	
				sprintf(tempXMLString, "<%s>%d</%s>\n", 
					pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
					pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRShort_, 
					pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);
					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
			  }
           break;

			case IPDRDATA_USHORT: 
			  if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ == -1) {
			  	pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
			  }
			  if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
			  	pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
				pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
				for(cnt = 0;cnt < (pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_);
                cnt++) { 
					sprintf(tempXMLString, "<%s>%u</%s>\n", 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
						pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUShort_, 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);

					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);

					pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;
				}
			  }
			  else {	
				sprintf(tempXMLString, "<%s>%u</%s>\n", 
					pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
					pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUShort_, 
					pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);
					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
			  }
               break;

			case IPDRDATA_BYTE:
			case IPDRDATA_UBYTE:
			  if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ == -1) {
			  	pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
			  }
	
			  if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
			  	pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
				pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
				for(cnt = 0;cnt < (pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_);
                cnt++) { 
					sprintf(tempXMLString, "<%s>%d</%s>\n", 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
						pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRByte_, 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);

					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);

					pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;
				}
			  }
				else {	
					sprintf(tempXMLString, "<%s>%d</%s>\n", 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
						pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRByte_, 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);

					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
				}

                    break;
			
			default:
					return(IPDR_NON_PRIMITIVE_DATA_TYPE);
					break;
        }
	} else {

		convertToUpper(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->derivedType_, derivedType); 
		strcpy(resultGet,"");

		if(!strcmp(derivedType, "IPV4ADDR")) {
			  if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ == -1) {
			  	pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
			  }
			  if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
			  	pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
				pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
				for(cnt = 0;cnt < (pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_);
                cnt++) { 
					errorSeverity = convIntToIPv4Addr(pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRLong_, resultGet, pErrorCode);   
					if (errorSeverity == IPDR_WARNING) {
						errorHandler(pIPDRCommonParameters, pErrorCode, "convIntToIPv4Addr"); 
					}
					else if (errorSeverity == IPDR_ERROR) {
						pIPDRCommonParameters->DocState_ = NONWRITABLE;
						exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLRecordHelper"); 
						free(resultGet);
						free(tempXMLString);
						free(IPDRCreationTime);
						free(stringIPDRCreationTime);
						return (IPDR_ERROR);
					}
					sprintf(tempXMLString, "<%s>%s</%s>\n", 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
						resultGet, 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);

					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);

					pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;
				}
			  }
			  else {
				errorSeverity = convIntToIPv4Addr(pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRLong_, resultGet, pErrorCode);   
				if (errorSeverity == IPDR_WARNING) {
					errorHandler(pIPDRCommonParameters, pErrorCode, "convIntToIPv4Addr"); 
				}
				else if (errorSeverity == IPDR_ERROR) {
					pIPDRCommonParameters->DocState_ = NONWRITABLE;
					exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLRecordHelper"); 
					free(resultGet);
					free(tempXMLString);
					free(IPDRCreationTime);
					free(stringIPDRCreationTime);
					return (IPDR_ERROR);
				}
				sprintf(tempXMLString, "<%s>%s</%s>\n", 
					pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
					resultGet, 
					pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);
				write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
			}
		}

		else if(!strcmp(derivedType,"IPV6ADDR")) {
			  if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ == -1) {
			  	pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
			  }
			  if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
			  	pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
				pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
				for(cnt = 0;cnt < (pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_);
                cnt++) { 
					/*errorSeverity = convByteArrayToIPV6Addr(pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRByteArray_, resultGet, pErrorCode);
					if (errorSeverity == IPDR_WARNING) {
						errorHandler(pIPDRCommonParameters, pErrorCode, "convByteArrayToIPV6Addr"); 
					}
					else if (errorSeverity == IPDR_ERROR) {
						pIPDRCommonParameters->DocState_ = NONWRITABLE;
						exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLRecordHelper"); 
						free(resultGet);
						free(tempXMLString);
						free(IPDRCreationTime);
						free(stringIPDRCreationTime);
						return (IPDR_ERROR);
					}
					sprintf(tempXMLString, "<%s>%s</%s>\n", 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
						resultGet, 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);*/
					sprintf(tempXMLString, "<%s>%s</%s>\n", 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
						pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRByteArray_, 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);

					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);

					pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;
				}
			  }
			  else {
					/*errorSeverity = convByteArrayToIPV6Addr(pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRByteArray_, resultGet, pErrorCode);   
					if (errorSeverity == IPDR_WARNING) {
						errorHandler(pIPDRCommonParameters, pErrorCode, "convByteArrayToIPV6Addr"); 
					}
					else if (errorSeverity == IPDR_ERROR) {
						pIPDRCommonParameters->DocState_ = NONWRITABLE;
						exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLRecordHelper"); 
						free(resultGet);
						free(tempXMLString);
						free(IPDRCreationTime);
						free(stringIPDRCreationTime);
						return (IPDR_ERROR);
					}
					sprintf(tempXMLString, "<%s>%s</%s>\n", 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
						resultGet, 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);*/
					sprintf(tempXMLString, "<%s>%s</%s>\n", 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
						pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRByteArray_, 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);

					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
		  	}
		}

		else if(!strcmp(derivedType,"UUID")) {
			  if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ == -1) {
			  	pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
			  }
			if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
			  	pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
				pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
				for(cnt = 0;cnt < (pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_);
                cnt++) { 
					/*errorSeverity = convByteArrayToUUID(pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRByteArray_, resultGet, pErrorCode);   
					if (errorSeverity == IPDR_WARNING) {
						errorHandler(pIPDRCommonParameters, pErrorCode, "convByteArrayToUUID"); 
					}
					else if (errorSeverity == IPDR_ERROR) {
						pIPDRCommonParameters->DocState_ = NONWRITABLE;
						exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLRecordHelper"); 
						free(resultGet);
						free(tempXMLString);
						free(IPDRCreationTime);
						free(stringIPDRCreationTime);
						return (IPDR_ERROR);
					}
					sprintf(tempXMLString, "<%s>%s</%s>\n", 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
						resultGet, 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);*/
					sprintf(tempXMLString, "<%s>%s</%s>\n", 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
						pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRByteArray_, 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);

					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);

					pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;
				}
			  }
			else {
					/*errorSeverity = convByteArrayToUUID(pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRByteArray_, resultGet, pErrorCode);   
					if (errorSeverity == IPDR_WARNING) {
						errorHandler(pIPDRCommonParameters, pErrorCode, "convByteArrayToUUID"); 
					}
					else if (errorSeverity == IPDR_ERROR) {
						pIPDRCommonParameters->DocState_ = NONWRITABLE;
						exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLRecordHelper"); 
						free(resultGet);
						free(tempXMLString);
						free(IPDRCreationTime);
						free(stringIPDRCreationTime);
						return (IPDR_ERROR);
					}
					sprintf(tempXMLString, "<%s>%s</%s>\n", 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
						resultGet, 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);*/
					sprintf(tempXMLString, "<%s>%s</%s>\n", 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
						pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRByteArray_, 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);

					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
			}
		}
		else if(!strcmp(derivedType,"DATETIME")) {
			  if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ == -1) {
			  	pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
			  }
			if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
			  	pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
				pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
				for(cnt = 0;cnt < (pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_);
                cnt++) { 
					errorSeverity = convIntToDateTime(&pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRLong_, resultGet, pErrorCode);   
					if (errorSeverity == IPDR_WARNING) {
						errorHandler(pIPDRCommonParameters, pErrorCode, "convIntToDateTime"); 
					}
					else if (errorSeverity == IPDR_ERROR) {
						pIPDRCommonParameters->DocState_ = NONWRITABLE;
						exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLRecordHelper"); 
						free(resultGet);
						free(tempXMLString);
						free(IPDRCreationTime);
						free(stringIPDRCreationTime);
						return (IPDR_ERROR);
					}
					sprintf(tempXMLString, "<%s>%s</%s>\n", 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
						resultGet, 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);

					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);

					pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;
				}
			  }
			else {
				errorSeverity = convIntToDateTime(&pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRLong_, resultGet, pErrorCode);
				if (errorSeverity == IPDR_WARNING) {
				errorHandler(pIPDRCommonParameters, pErrorCode, "convIntToDateTime"); 
				}
				else if (errorSeverity == IPDR_ERROR) {
					pIPDRCommonParameters->DocState_ = NONWRITABLE;
					exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLRecordHelper"); 
					free(resultGet);
					free(tempXMLString);
					free(IPDRCreationTime);
					free(stringIPDRCreationTime);
					return (IPDR_ERROR);
				}
				sprintf(tempXMLString, "<%s>%s</%s>\n", 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
						resultGet, 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);

				write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
			}
		}
		else if(!strcmp(derivedType, "DATETIMEMSEC")) {
			  if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ == -1) {
			  	pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
			  }
			if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
			  	pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
				pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
				for(cnt = 0;cnt < (pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_);
                cnt++) { 
					errorSeverity = convLongToTime(&pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRLongLong_, resultGet, pErrorCode);   
					if (errorSeverity == IPDR_WARNING) {
						errorHandler(pIPDRCommonParameters, pErrorCode, "convLongToTime"); 
					}
					else if (errorSeverity == IPDR_ERROR) {
						pIPDRCommonParameters->DocState_ = NONWRITABLE;
						exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLRecordHelper"); 
						free(resultGet);
						free(tempXMLString);
						free(IPDRCreationTime);
						free(stringIPDRCreationTime);
						return (IPDR_ERROR);
					}
					sprintf(tempXMLString, "<%s>%s</%s>\n", 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
						resultGet, 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);

					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);

					pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;
				}
			  }
			else {
				errorSeverity = convLongToTime(&pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRLongLong_, resultGet, pErrorCode);   
				if (errorSeverity == IPDR_WARNING) {
					errorHandler(pIPDRCommonParameters, pErrorCode, "convLongToTime"); 
				}
				else if (errorSeverity == IPDR_ERROR) {
					pIPDRCommonParameters->DocState_ = NONWRITABLE;
					exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLRecordHelper"); 
					free(resultGet);
					free(tempXMLString);
					free(IPDRCreationTime);
					free(stringIPDRCreationTime);
					return (IPDR_ERROR);
				}
				sprintf(tempXMLString, "<%s>%s</%s>\n", 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
						resultGet, 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);

				write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
			}
		} 
		else if(!strcmp(derivedType,"IPADDR")) {
			  if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ == -1) {
			  	pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
			  }
			if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
			  	pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
				pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
				for(cnt = 0;cnt < (pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_);
                cnt++) { 
					/*errorSeverity = convByteArrayToIPAddr(pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRByteArray_, resultGet, pErrorCode);   
					if (errorSeverity == IPDR_WARNING) {
						errorHandler(pIPDRCommonParameters, pErrorCode, "convByteArrayToIPAddr"); 
					}
					else if (errorSeverity == IPDR_ERROR) {
						pIPDRCommonParameters->DocState_ = NONWRITABLE;
						exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLRecordHelper"); 
						free(resultGet);
						free(tempXMLString);
						free(IPDRCreationTime);
						free(stringIPDRCreationTime);
						return (IPDR_ERROR);
					}
					sprintf(tempXMLString, "<%s>%s</%s>\n", 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
						resultGet, 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);*/
					sprintf(tempXMLString, "<%s>%s</%s>\n", 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
						pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRByteArray_, 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);

					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);

					pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;
				}
			  }
			else {
				/*errorSeverity = convByteArrayToIPAddr(pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRByteArray_, resultGet, pErrorCode);   
				if (errorSeverity == IPDR_WARNING) {
					errorHandler(pIPDRCommonParameters, pErrorCode, "convByteArrayToIPAddr"); 
				}
				else if (errorSeverity == IPDR_ERROR) {
					pIPDRCommonParameters->DocState_ = NONWRITABLE;
					exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLRecordHelper"); 
					free(resultGet);
					free(tempXMLString);
					free(IPDRCreationTime);
					free(stringIPDRCreationTime);
					return (IPDR_ERROR);
				}
				sprintf(tempXMLString, "<%s>%s</%s>\n", 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
						resultGet, 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);*/
				sprintf(tempXMLString, "<%s>%s</%s>\n", 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
						pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRByteArray_, 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);

				write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
			}
		}
		else if(!strcmp(derivedType, "DATETIMEUSEC")) {
			  if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ == -1) {
			  	pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
			  }
			if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
			  	pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
				pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
				for(cnt = 0;cnt < (pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_);
                cnt++) { 
					errorSeverity = convLongMicroToTime(pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRLongLong_, resultGet, pErrorCode);   
					if (errorSeverity == IPDR_WARNING) {
						errorHandler(pIPDRCommonParameters, pErrorCode, "convLongMicroToTime"); 
					}
					else if (errorSeverity == IPDR_ERROR) {
						pIPDRCommonParameters->DocState_ = NONWRITABLE;
						exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLRecordHelper"); 
						free(resultGet);
						free(tempXMLString);
						free(IPDRCreationTime);
						free(stringIPDRCreationTime);
						return (IPDR_ERROR);
					}
					sprintf(tempXMLString, "<%s>%s</%s>\n", 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
						resultGet, 
						pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);

					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);

					pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;
				}
			  }
			else {
				errorSeverity = convLongMicroToTime(&pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRLongLong_, resultGet, pErrorCode);   
				if (errorSeverity == IPDR_WARNING) {
					errorHandler(pIPDRCommonParameters, pErrorCode, "convLongMicroToTime"); 
				}
				else if (errorSeverity == IPDR_ERROR) {
					pIPDRCommonParameters->DocState_ = NONWRITABLE;
					exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLRecordHelper"); 
					free(resultGet);
					free(tempXMLString);
					free(IPDRCreationTime);
					free(stringIPDRCreationTime);
					return (IPDR_ERROR);
				}
				sprintf(tempXMLString, "<%s>%s</%s>\n", 
					pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
					resultGet, 
					pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);
				write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
			}
		} 
	}
		pFNFData->pListAttributeDescriptor_ = pFNFData->pListAttributeDescriptor_->pNext_;
		pFNFData->pListIPDRData_ = pFNFData->pListIPDRData_->pNext_;
	}

	sprintf(tempXMLString, "%s\n", "</IPDR>");
	write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);

	pFNFData->pListIPDRData_ = pCurrentListIPDRData;

  	pFNFData->pListAttributeDescriptor_ = pCurrentListAttributeDescriptor;

 

	free(resultGet);
	free(tempXMLString);
	free(IPDRCreationTime);
	free(stringIPDRCreationTime);

	return (IPDR_OK);
}


/*!
 *  NAME:
 *      convDerivedTypeToAttributeType() - changes derived data 
 *				values to primitive types for derived type fields    
 *                     
 *  DESCRIPTION:
 *     	-This function will be used to convert FNFData structure for  
 *       the derived type data values.
 *		-This fxn calls utils  functions to actually convert the 
 *		 data from derived to primitive types.
 *  
 *	ARGUMENTS:
 *
 *      - FNFData {IN}
 *		- pErrorCode {IN / OUT } 
 *
 *  RETURNS 	
 *	    Returns Zero or appropriate error code
*/

int	convDerivedTypeToAttributeType(FNFData *pFNFData, int* pErrorCode)
{
	int count = 0;
	int lengthAttributeDescriptor = 0;
	char* tempString;
	char* endPtr;
	void* data = NULL;
	double tempDouble = 0;
	float tempFloat = 0;
	int tempLong = 0;
	void* IPDRValue = NULL;
	IPDRData* IPDRData = NULL;
	ListAttributeDescriptor* pCurrentDesc = pFNFData->pListAttributeDescriptor_;
	ListIPDRData* pCurrentData = pFNFData->pListIPDRData_; 
	ListIPDRData* newListIPDRData = NULL;
	ListIPDRData* pFirst = NULL;

	data = (void *) malloc (MAX_IPDR_STRING);
	tempString = (char *) calloc (MAX_XML_STRING_LENGTH, sizeof(char));


	if(data == NULL  || tempString == NULL) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		free(data); 
		free(tempString); 
		return(IPDR_ERROR);
	}
	
	lengthAttributeDescriptor = getLengthListAttributeDescriptor(pFNFData->pListAttributeDescriptor_); 

	for(count = 0;  count < lengthAttributeDescriptor; count++) {
		convertToUpper(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->derivedType_, tempString); 

		if(!strcmp(tempString,"IPV4ADDR")) {
			convIPv4AddrToInt((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, 
								&tempLong, pErrorCode);   
			appendListIPDRData(&newListIPDRData, (void *) tempLong, IPDRDATA_LONG);
		} 
		else if(!strcmp(tempString, "IPV6ADDR")) {
			/*convIPV6AddrToByteArray((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, 
									(unsigned char*) data, pErrorCode);   
			appendListIPDRData(&newListIPDRData, (void *) data, IPDRDATA_HEXADECARRAY);*/
			appendListIPDRData(&newListIPDRData, (void *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, IPDRDATA_HEXADECARRAY);
		} 
		else if(!strcmp(tempString, "UUID")) {
			/*convUUIDToByteArray((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_,
									(unsigned char*) data, pErrorCode);   
			appendListIPDRData(&newListIPDRData, (void *) data, IPDRDATA_HEXADECARRAY);*/
			appendListIPDRData(&newListIPDRData, (void *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, IPDRDATA_HEXADECARRAY);
		} 
		else if(!strcmp(tempString, "DATETIME")) {
			convDateTimeToInt((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, 
									(int*) data, pErrorCode);
			appendListIPDRData(&newListIPDRData, (void *) (*(int *) data), IPDRDATA_LONG);
		} 
		else if(!strcmp(tempString, "DATETIMEMSEC")) {
			convTimeToLong((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, 
								(hyper*) data, pErrorCode);   
			appendListIPDRData(&newListIPDRData, data, IPDRDATA_LONGLONG);
		} 
		else if(!strcmp(tempString, "INTEGER")) {
			IPDRData = newIPDRData(IPDRDATA_LONG);
			IPDRData->IPDRDataUnion_.IPDRLong_ = atoi((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_);
			appendListIPDRData(&newListIPDRData, 
				(void *) IPDRData->IPDRDataUnion_.IPDRLong_, 
				IPDRDATA_LONG);
			free(IPDRData);
		} 
		else if(!strcmp(tempString, "UNSIGNED_INTEGER")) {
			IPDRData = newIPDRData(IPDRDATA_ULONG);
			/*IPDRData->IPDRDataUnion_.IPDRULong_ = atoi((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_);*/
			IPDRData->IPDRDataUnion_.IPDRULong_ = strtoul((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, NULL, 10);
			appendListIPDRData(&newListIPDRData, 
				(void *) IPDRData->IPDRDataUnion_.IPDRULong_, 
				IPDRDATA_ULONG);
			free(IPDRData);
		} 
		else if(!strcmp(tempString, "HYPER")) {
			IPDRData = newIPDRData(IPDRDATA_LONGLONG);
			IPDRData->IPDRDataUnion_.IPDRLongLong_=
				(hyper) strtod((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, &endPtr);			
			appendListIPDRData(&newListIPDRData, 
				(void *) &(IPDRData->IPDRDataUnion_.IPDRLongLong_), 
				IPDRDATA_LONGLONG);
			free(IPDRData);
		} 
		else if(!strcmp(tempString, "UNSIGNED_HYPER")) {
			IPDRData = newIPDRData(IPDRDATA_ULONGLONG);
			IPDRData->IPDRDataUnion_.IPDRULongLong_=
				(u_hyper) strtod((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, &endPtr);			
			appendListIPDRData(&newListIPDRData, 
				(void *) &(IPDRData->IPDRDataUnion_.IPDRULongLong_), 
				IPDRDATA_LONGLONG);
			free(IPDRData);
			/*
			strcpy(data, (char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_);
			appendListIPDRData(&newListIPDRData, (void *) &(pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRULongLong_), IPDRDATA_ULONGLONG);
			*/
		} 
		else if(!strcmp(tempString, "FLOAT")) {
			IPDRData = newIPDRData(IPDRDATA_FLOAT);
			IPDRData->IPDRDataUnion_.IPDRFloat_ = 
			(float)atof((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_);
			IPDRValue = (float *) &IPDRData->IPDRDataUnion_.IPDRFloat_;
			appendListIPDRData(&newListIPDRData, IPDRValue, IPDRDATA_FLOAT);
			free(IPDRData);
		} 
		else if(!strcmp(tempString, "DOUBLE")) {
			IPDRData = newIPDRData(IPDRDATA_DOUBLE);
			IPDRData->IPDRDataUnion_.IPDRDouble_= strtod((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, &endPtr);
			IPDRValue = (double *) &IPDRData->IPDRDataUnion_.IPDRDouble_;
			appendListIPDRData(&newListIPDRData, IPDRValue, IPDRDATA_DOUBLE);
		free(IPDRData);
		} 
		else if(!strcmp(tempString, "BYTE_ARRAY")) {
			appendListIPDRData(&newListIPDRData, pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, IPDRDATA_HEXADECARRAY);
		} 
		else if(!strcmp(tempString, "UTF8_ARRAY")) {
			appendListIPDRData(&newListIPDRData, pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, IPDRDATA_STRING);
		}
		else if(!strcmp(tempString, "IPADDR")) {
			/*convIPAddrToByteArray((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, 
									(unsigned char*) data, pErrorCode);   
			appendListIPDRData(&newListIPDRData, (void *) data, IPDRDATA_HEXADECARRAY);*/
			appendListIPDRData(&newListIPDRData, (void *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, IPDRDATA_HEXADECARRAY);
		} 
		else if(!strcmp(tempString, "DATETIMEUSEC")) {
			convMicroTimeToLong((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, 
								(hyper*) data, pErrorCode);   
			appendListIPDRData(&newListIPDRData, data, IPDRDATA_LONGLONG);
		} 
		else if(!strcmp(tempString, "BOOLEAN")) {
			appendListIPDRData(&newListIPDRData, 
				pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRBoolean_, 
				IPDRDATA_BOOLEAN);
			free(IPDRData);
		} 
		else if(!strcmp(tempString, "SHORT")) {
			IPDRData = newIPDRData(IPDRDATA_SHORT);
			IPDRData->IPDRDataUnion_.IPDRShort_ = atoi((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_);
			/*
			IPDRValue = (short int *) &IPDRData->IPDRDataUnion_.IPDRShort_;
			appendListIPDRData(&newListIPDRData, 
				IPDRValue, 
				IPDRDATA_SHORT);
			*/
			appendListIPDRData(&newListIPDRData, 
				(void *)IPDRData->IPDRDataUnion_.IPDRShort_, 
				IPDRDATA_SHORT);
			free(IPDRData);
		} 
		else if(!strcmp(tempString, "UNSIGNED_SHORT")) {
			IPDRData = newIPDRData(IPDRDATA_USHORT);
			IPDRData->IPDRDataUnion_.IPDRUShort_ = atoi((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_);
			/*
			IPDRValue = (unsigned short int *) &IPDRData->IPDRDataUnion_.IPDRShort_;
			appendListIPDRData(&newListIPDRData, 
				IPDRValue, 
				IPDRDATA_USHORT);
			*/
			appendListIPDRData(&newListIPDRData, 
				(void *) IPDRData->IPDRDataUnion_.IPDRShort_, 
				IPDRDATA_USHORT);
			free(IPDRData);
		} 
		else if(!strcmp(tempString, "BYTE")) {
			IPDRData = newIPDRData(IPDRDATA_BYTE);
			IPDRData->IPDRDataUnion_.IPDRByte_ = atoi((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_);
			appendListIPDRData(&newListIPDRData, 
				(void *) IPDRData->IPDRDataUnion_.IPDRByte_, 
				IPDRDATA_BYTE);
			free(IPDRData);
		} 
		else if(!strcmp(tempString, "UNSIGNED_BYTE")) {
			IPDRData = newIPDRData(IPDRDATA_BYTE);
			IPDRData->IPDRDataUnion_.IPDRByte_ = atoi((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_);
			appendListIPDRData(&newListIPDRData, 
				(void *) IPDRData->IPDRDataUnion_.IPDRByte_, 
				IPDRDATA_BYTE);
			free(IPDRData);
		} 

		pFNFData->pListAttributeDescriptor_ = pFNFData->pListAttributeDescriptor_->pNext_;
		pFNFData->pListIPDRData_ = pFNFData->pListIPDRData_->pNext_; 
	}
	
	pFNFData->pListAttributeDescriptor_ = pCurrentDesc;
	
	pFNFData->pListIPDRData_ = pCurrentData;
	freeListIPDRData(&(pFNFData->pListIPDRData_)); 
	pFNFData->pListIPDRData_ = NULL;
	pFirst = newListIPDRData;

	while (newListIPDRData != NULL) {
		
		if (newListIPDRData->IPDRDataType_ == IPDRDATA_HEXADECARRAY) {
			 appendListIPDRData(&(pFNFData->pListIPDRData_ ),
							newListIPDRData->IPDRDataUnion_.IPDRByteArray_,
							IPDRDATA_HEXADECARRAY);
		}
		else if (newListIPDRData->IPDRDataType_ == IPDRDATA_DOUBLE) {
			IPDRValue = (double *) &newListIPDRData->IPDRDataUnion_.IPDRDouble_;
			  appendListIPDRData(&(pFNFData->pListIPDRData_ ),
							IPDRValue,
							IPDRDATA_DOUBLE);
		}
		else if (newListIPDRData->IPDRDataType_ == IPDRDATA_FLOAT) {
		 IPDRValue = (float *) &newListIPDRData->IPDRDataUnion_.IPDRFloat_;
		  appendListIPDRData(&(pFNFData->pListIPDRData_ ),
							IPDRValue,
							IPDRDATA_FLOAT);
		}
		else if (newListIPDRData->IPDRDataType_ == IPDRDATA_LONG) {
			appendListIPDRData(&(pFNFData->pListIPDRData_ ),
							(void *) newListIPDRData->IPDRDataUnion_.IPDRLong_,
							IPDRDATA_LONG);
		}
		else if (newListIPDRData->IPDRDataType_ == IPDRDATA_LONGLONG) {
			appendListIPDRData(&(pFNFData->pListIPDRData_ ),
							(void *) &(newListIPDRData->IPDRDataUnion_.IPDRLongLong_),
							IPDRDATA_LONGLONG);
		}
		else if (newListIPDRData->IPDRDataType_ == IPDRDATA_ULONG) {
			appendListIPDRData(&(pFNFData->pListIPDRData_ ),
							(void *) newListIPDRData->IPDRDataUnion_.IPDRULong_,
							IPDRDATA_ULONG);
		}
		else if (newListIPDRData->IPDRDataType_ == IPDRDATA_ULONGLONG) {
			appendListIPDRData(&(pFNFData->pListIPDRData_ ),
							(void *) &(newListIPDRData->IPDRDataUnion_.IPDRULongLong_),
							IPDRDATA_ULONGLONG);
		}
		else if (newListIPDRData->IPDRDataType_ == IPDRDATA_STRING) {
			appendListIPDRData(&(pFNFData->pListIPDRData_),
							newListIPDRData->IPDRDataUnion_.IPDRUTF8Array_,
							IPDRDATA_STRING);
		}
		else if (newListIPDRData->IPDRDataType_ == IPDRDATA_BOOLEAN) {
			appendListIPDRData(&(pFNFData->pListIPDRData_ ),
							newListIPDRData->IPDRDataUnion_.IPDRBoolean_,
							IPDRDATA_BOOLEAN);
		}
		else if (newListIPDRData->IPDRDataType_ == IPDRDATA_SHORT) {
			/*
			IPDRValue = (short int *) &newListIPDRData->IPDRDataUnion_.IPDRShort_;
			appendListIPDRData(&(pFNFData->pListIPDRData_ ),
							IPDRValue,
							IPDRDATA_SHORT);
			*/
			appendListIPDRData(&(pFNFData->pListIPDRData_ ),
							(void *) newListIPDRData->IPDRDataUnion_.IPDRShort_,
							IPDRDATA_SHORT);
		}
		else if (newListIPDRData->IPDRDataType_ == IPDRDATA_USHORT) {
			/*
			IPDRValue = (unsigned short int *) &newListIPDRData->IPDRDataUnion_.IPDRUShort_;
			appendListIPDRData(&(pFNFData->pListIPDRData_ ),
							IPDRValue,
							IPDRDATA_USHORT);*/
			appendListIPDRData(&(pFNFData->pListIPDRData_ ),
							(void *)newListIPDRData->IPDRDataUnion_.IPDRUShort_,
							IPDRDATA_USHORT);
		}
		else if (newListIPDRData->IPDRDataType_ == IPDRDATA_BYTE) {
			appendListIPDRData(&(pFNFData->pListIPDRData_ ),
							(void *) newListIPDRData->IPDRDataUnion_.IPDRByte_,
							IPDRDATA_BYTE);
		}
		else if (newListIPDRData->IPDRDataType_ == IPDRDATA_UBYTE) {
			appendListIPDRData(&(pFNFData->pListIPDRData_ ),
							(void *) newListIPDRData->IPDRDataUnion_.IPDRByte_,
							IPDRDATA_BYTE);
		}

		newListIPDRData = newListIPDRData->pNext_; 
	}

	
	newListIPDRData = pFirst;
	freeListIPDRData(&(newListIPDRData));  
	free(data); 
	free(tempString); 
	return (IPDR_OK);	 
}

/*!
 *  NAME:
 *      convDerivedTypeToAttributeTypeFor35Version() - changes derived data 
 *				values to primitive types for derived complex type fields    
 *                     
 *  DESCRIPTION:
 *     	-This function will be used to convert FNFData structure for  
 *       the derived complex type data values.
 *		-This fxn calls utils  functions to actually convert the 
 *		 data from derived to primitive types.
 *  
 *	ARGUMENTS:
 *
 *      - FNFData {IN}
 *		- pErrorCode {IN / OUT } 
 *
 *  RETURNS 	
 *	    Returns Zero or appropriate error code
*/
int	convDerivedTypeToAttributeTypeFor35Version(FNFData *pFNFData, int* pErrorCode)
{
	int count = 0;
	int cnt = 0;
	int lengthAttributeDescriptor = 0;
	int tempLong = 0;
	char* tempString;
	char* endPtr;
	void* data = NULL;
	double tempDouble = 0;
	float tempFloat = 0;
	void* IPDRValue = NULL;
	IPDRData* IPDRData = NULL;
	ListAttributeDescriptor* pCurrentDesc = pFNFData->pListAttributeDescriptor_;
	ListIPDRData* pCurrentData = pFNFData->pListIPDRData_; 
	ListIPDRData* newListIPDRData = NULL;
	ListIPDRData* pFirst = NULL;
	ListIPDRComplex* pListIPDRComplex = NULL;
	ListIPDRComplexData* pListIPDRComplexData[MAX_IPDR_STRING];
	ListIPDRStructureData* pCurrentListIPDRStructureData = NULL;

	data = (void *) malloc (MAX_IPDR_STRING);
	tempString = (char *) calloc (MAX_XML_STRING_LENGTH, sizeof(char));
	memset(&pListIPDRComplexData, 0, sizeof(pListIPDRComplexData));

	if(data == NULL  || tempString == NULL) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		free(data); 
		free(tempString); 
		return(IPDR_ERROR);
	}
	
	lengthAttributeDescriptor = getLengthListAttributeDescriptor(pFNFData->pListAttributeDescriptor_); 

	for(count = 0;  count < lengthAttributeDescriptor; count++) {
		if(isPrimitiveComplexType(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_) == IPDR_FALSE)
		{
			if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ == -1) {
				pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->pListIPDRStructureData_->noOfElements_;
			}

			if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
   			pCurrentListIPDRStructureData = pFNFData->pListIPDRData_->pListIPDRStructureData_;
				for(cnt = 0;cnt < (pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_);
                cnt++) {
					pListIPDRComplex = NULL;
					populateComplexDataTypeWithSchema(
														 pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_,
														 pFNFData,
														 &pListIPDRComplex,
														 pErrorCode
														 );
					appendListIPDRComplexData(&pListIPDRComplexData[cnt], 
														pListIPDRComplex, 
														IPDRDATA_STRUCTURE);
   				pFNFData->pListIPDRData_->pListIPDRStructureData_ = 
   				pFNFData->pListIPDRData_->pListIPDRStructureData_->pNext_;
				}
				appendListIPDRWithComplexData(&((newListIPDRData)),pListIPDRComplexData, IPDRDATA_STRUCTURE);
   			pFNFData->pListIPDRData_->pListIPDRStructureData_ = pCurrentListIPDRStructureData;
			} else {
					pListIPDRComplex = NULL;
					populateComplexDataTypeWithSchema(
														 pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_,
														 pFNFData,
														 &pListIPDRComplex,
														 pErrorCode
														 );
					appendListIPDRComplexData(&pListIPDRComplexData[0], 
														pListIPDRComplex, 
														IPDRDATA_STRUCTURE);
				appendListIPDRWithComplexData(&((newListIPDRData)),pListIPDRComplexData, IPDRDATA_STRUCTURE);
				pListIPDRComplexData[0] = NULL;
			}
		
		} else if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ != 0){
			/*Arrays dataTypes*/
			populateArrayDataTypeWithSchema( pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_,
														 pFNFData,
														&newListIPDRData,
														pErrorCode);
		} else {
		convertToUpper(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->derivedType_, tempString); 

		if(!strcmp(tempString,"IPV4ADDR")) {
			convIPv4AddrToInt((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, 
								&tempLong, pErrorCode);   
			appendListIPDRData(&newListIPDRData, (void *) tempLong, IPDRDATA_LONG);
		} 
		else if(!strcmp(tempString, "IPV6ADDR")) {
			/*convIPV6AddrToByteArray((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, 
									(unsigned char*) data, pErrorCode);
			appendListIPDRData(&newListIPDRData, (void *) data, IPDRDATA_HEXADECARRAY);*/
			appendListIPDRData(&newListIPDRData, (void *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, IPDRDATA_HEXADECARRAY);
		} 
		else if(!strcmp(tempString, "UUID")) {
			/*convUUIDToByteArray((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_,
									(unsigned char*) data, pErrorCode);
			appendListIPDRData(&newListIPDRData, (void *) data, IPDRDATA_HEXADECARRAY);*/
			appendListIPDRData(&newListIPDRData, (void *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, IPDRDATA_HEXADECARRAY);
		} 
		else if(!strcmp(tempString, "DATETIME")) {
			convDateTimeToInt((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, 
									(int*) data, pErrorCode);
			appendListIPDRData(&newListIPDRData, (void *) (*(int *) data), IPDRDATA_LONG);
		} 
		else if(!strcmp(tempString, "DATETIMEMSEC")) {
			convTimeToLong((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, 
								(hyper*) data, pErrorCode);   
			appendListIPDRData(&newListIPDRData, data, IPDRDATA_LONGLONG);
		} 
		else if(!strcmp(tempString, "INTEGER")) {
			IPDRData = newIPDRData(IPDRDATA_LONG);
			IPDRData->IPDRDataUnion_.IPDRLong_ = atoi((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_);
			appendListIPDRData(&newListIPDRData, 
				(void *) IPDRData->IPDRDataUnion_.IPDRLong_, 
				IPDRDATA_LONG);
			free(IPDRData);
		} 
		else if(!strcmp(tempString, "UNSIGNED_INTEGER")) {
			IPDRData = newIPDRData(IPDRDATA_ULONG);
			/*IPDRData->IPDRDataUnion_.IPDRULong_ = atoi((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_);*/
			IPDRData->IPDRDataUnion_.IPDRULong_ = strtoul((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, NULL, 10);
			appendListIPDRData(&newListIPDRData, 
				(void *) IPDRData->IPDRDataUnion_.IPDRULong_, 
				IPDRDATA_ULONG);
			free(IPDRData);
		} 
		else if(!strcmp(tempString, "HYPER")) {
			IPDRData = newIPDRData(IPDRDATA_LONGLONG);
			IPDRData->IPDRDataUnion_.IPDRLongLong_=
				(hyper) strtod((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, &endPtr);			
			appendListIPDRData(&newListIPDRData, 
				(void *) &(IPDRData->IPDRDataUnion_.IPDRLongLong_), 
				IPDRDATA_LONGLONG);
			free(IPDRData);
		} 
		else if(!strcmp(tempString, "UNSIGNED_HYPER")) {
			IPDRData = newIPDRData(IPDRDATA_ULONGLONG);
			IPDRData->IPDRDataUnion_.IPDRULongLong_=
				(u_hyper) strtod((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, &endPtr);			
			appendListIPDRData(&newListIPDRData, 
				(void *) &(IPDRData->IPDRDataUnion_.IPDRULongLong_), 
				IPDRDATA_ULONGLONG);
			free(IPDRData);
			/*
			strcpy(data, (char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_);
			appendListIPDRData(&newListIPDRData, (void *) &(pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRULongLong_), IPDRDATA_ULONGLONG);
			*/
		} 
		else if(!strcmp(tempString, "FLOAT")) {
			IPDRData = newIPDRData(IPDRDATA_FLOAT);
			IPDRData->IPDRDataUnion_.IPDRFloat_ = 
			(float)atof((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_);
			IPDRValue = (float *) &IPDRData->IPDRDataUnion_.IPDRFloat_;
			appendListIPDRData(&newListIPDRData, IPDRValue, IPDRDATA_FLOAT);
			free(IPDRData);
		} 
		else if(!strcmp(tempString, "DOUBLE")) {
			IPDRData = newIPDRData(IPDRDATA_DOUBLE);
			IPDRData->IPDRDataUnion_.IPDRDouble_= strtod((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, &endPtr);
			IPDRValue = (double *) &IPDRData->IPDRDataUnion_.IPDRDouble_;
			appendListIPDRData(&newListIPDRData, IPDRValue, IPDRDATA_DOUBLE);
		free(IPDRData);
		} 
		else if(!strcmp(tempString, "BYTE_ARRAY")) {
			appendListIPDRData(&newListIPDRData, pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, IPDRDATA_HEXADECARRAY);
		} 
		else if(!strcmp(tempString, "UTF8_ARRAY")) {
			appendListIPDRData(&newListIPDRData, pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, IPDRDATA_STRING);
		}
		else if(!strcmp(tempString, "IPADDR")) {
			/*convIPAddrToByteArray((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, 
									(unsigned char*) data, pErrorCode);   
			appendListIPDRData(&newListIPDRData, (void *) data, IPDRDATA_HEXADECARRAY);*/
			appendListIPDRData(&newListIPDRData, (void *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, IPDRDATA_HEXADECARRAY);
		} 
		else if(!strcmp(tempString, "DATETIMEUSEC")) {
			convMicroTimeToLong((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, 
								(hyper*) data, pErrorCode);   
			appendListIPDRData(&newListIPDRData, data, IPDRDATA_LONGLONG);
		} 
		else if(!strcmp(tempString, "BOOLEAN")) {
			appendListIPDRData(&newListIPDRData, 
				pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, 
				IPDRDATA_BOOLEAN);
		} 
		else if(!strcmp(tempString, "SHORT")) {
			IPDRData = newIPDRData(IPDRDATA_SHORT);
			IPDRData->IPDRDataUnion_.IPDRShort_ = atoi((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_);
			/*
			IPDRValue = (short int *) &IPDRData->IPDRDataUnion_.IPDRShort_;
			appendListIPDRData(&newListIPDRData, 
				IPDRValue, 
				IPDRDATA_SHORT);
			*/
			appendListIPDRData(&newListIPDRData, 
				(void *)IPDRData->IPDRDataUnion_.IPDRShort_, 
				IPDRDATA_SHORT);
			free(IPDRData);
		} 
		else if(!strcmp(tempString, "UNSIGNED_SHORT")) {
			IPDRData = newIPDRData(IPDRDATA_USHORT);
			IPDRData->IPDRDataUnion_.IPDRUShort_ = atoi((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_);
			/*
			IPDRValue = (unsigned short int *) &IPDRData->IPDRDataUnion_.IPDRShort_;
			appendListIPDRData(&newListIPDRData, 
				IPDRValue, 
				IPDRDATA_USHORT);
			*/
			appendListIPDRData(&newListIPDRData, 
				(void *) IPDRData->IPDRDataUnion_.IPDRUShort_, 
				IPDRDATA_USHORT);
			free(IPDRData);
		}
		else if(!strcmp(tempString, "BYTE")) {
			IPDRData = newIPDRData(IPDRDATA_BYTE);
			IPDRData->IPDRDataUnion_.IPDRByte_ = atoi((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_);
			appendListIPDRData(&newListIPDRData, 
				(void *) IPDRData->IPDRDataUnion_.IPDRByte_, 
				IPDRDATA_BYTE);
			free(IPDRData);
		} 
		else if(!strcmp(tempString, "UNSIGNED_BYTE")) {
			IPDRData = newIPDRData(IPDRDATA_UBYTE);
			IPDRData->IPDRDataUnion_.IPDRByte_ = atoi((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_);
			appendListIPDRData(&newListIPDRData, 
				(void *) IPDRData->IPDRDataUnion_.IPDRByte_, 
				IPDRDATA_BYTE);
			free(IPDRData);
		} 
	} 

		pFNFData->pListAttributeDescriptor_ = pFNFData->pListAttributeDescriptor_->pNext_;
		pFNFData->pListIPDRData_ = pFNFData->pListIPDRData_->pNext_; 
	}
	
	pFNFData->pListAttributeDescriptor_ = pCurrentDesc;
	
	pFNFData->pListIPDRData_ = pCurrentData;
	freeListIPDRData(&(pFNFData->pListIPDRData_)); 
	pFNFData->pListIPDRData_ = NULL;
	pFirst = newListIPDRData;

	while (newListIPDRData != NULL) {
		
		if (newListIPDRData->IPDRDataType_ == IPDRDATA_HEXADECARRAY) {
			 appendListIPDRData(&(pFNFData->pListIPDRData_ ),
							newListIPDRData->IPDRDataUnion_.IPDRByteArray_,
							IPDRDATA_HEXADECARRAY);
		}
		else if (newListIPDRData->IPDRDataType_ == IPDRDATA_DOUBLE) {
			IPDRValue = (double *) &newListIPDRData->IPDRDataUnion_.IPDRDouble_;
			  appendListIPDRData(&(pFNFData->pListIPDRData_ ),
							IPDRValue,
							IPDRDATA_DOUBLE);
		}
		else if (newListIPDRData->IPDRDataType_ == IPDRDATA_FLOAT) {
		 IPDRValue = (float *) &newListIPDRData->IPDRDataUnion_.IPDRFloat_;
		  appendListIPDRData(&(pFNFData->pListIPDRData_ ),
							IPDRValue,
							IPDRDATA_FLOAT);
		}
		else if (newListIPDRData->IPDRDataType_ == IPDRDATA_LONG) {
			appendListIPDRData(&(pFNFData->pListIPDRData_ ),
							(void *) newListIPDRData->IPDRDataUnion_.IPDRLong_,
							IPDRDATA_LONG);
		}
		else if (newListIPDRData->IPDRDataType_ == IPDRDATA_LONGLONG) {
			appendListIPDRData(&(pFNFData->pListIPDRData_ ),
							(void *) &(newListIPDRData->IPDRDataUnion_.IPDRLongLong_),
							IPDRDATA_LONGLONG);
		}
		else if (newListIPDRData->IPDRDataType_ == IPDRDATA_ULONG) {
			appendListIPDRData(&(pFNFData->pListIPDRData_ ),
							(void *) newListIPDRData->IPDRDataUnion_.IPDRULong_,
							IPDRDATA_ULONG);
		}
		else if (newListIPDRData->IPDRDataType_ == IPDRDATA_ULONGLONG) {
			appendListIPDRData(&(pFNFData->pListIPDRData_ ),
							(void *) &(newListIPDRData->IPDRDataUnion_.IPDRULongLong_),
							IPDRDATA_ULONGLONG);
		}
		else if (newListIPDRData->IPDRDataType_ == IPDRDATA_STRING) {
			appendListIPDRData(&(pFNFData->pListIPDRData_),
							newListIPDRData->IPDRDataUnion_.IPDRUTF8Array_,
							IPDRDATA_STRING);
		}
		else if (newListIPDRData->IPDRDataType_ == IPDRDATA_BOOLEAN) {
			appendListIPDRData(&(pFNFData->pListIPDRData_ ),
							newListIPDRData->IPDRDataUnion_.IPDRBoolean_,
							IPDRDATA_BOOLEAN);
		}
		else if (newListIPDRData->IPDRDataType_ == IPDRDATA_SHORT) {
			/*
			IPDRValue = (short int *) &newListIPDRData->IPDRDataUnion_.IPDRShort_;
			appendListIPDRData(&(pFNFData->pListIPDRData_ ),
							IPDRValue,
							IPDRDATA_SHORT);
			*/
			appendListIPDRData(&(pFNFData->pListIPDRData_ ),
							(void *) newListIPDRData->IPDRDataUnion_.IPDRShort_,
							IPDRDATA_SHORT);
		}
		else if (newListIPDRData->IPDRDataType_ == IPDRDATA_USHORT) {
			/*
			IPDRValue = (unsigned short int *) &newListIPDRData->IPDRDataUnion_.IPDRUShort_;
			appendListIPDRData(&(pFNFData->pListIPDRData_ ),
							IPDRValue,
							IPDRDATA_USHORT);*/
			appendListIPDRData(&(pFNFData->pListIPDRData_ ),
							(void *)newListIPDRData->IPDRDataUnion_.IPDRUShort_,
							IPDRDATA_USHORT);
		}
   	else if (newListIPDRData->IPDRDataType_ == IPDRDATA_ARRAY) {
			appendListArrayToIPDRData(&pFNFData->pListIPDRData_,
    			newListIPDRData->pListIPDRArrayData_,
				newListIPDRData->noOfElements_,
    			IPDRDATA_ARRAY);
   	}
   	else if (newListIPDRData->IPDRDataType_ == IPDRDATA_STRUCTURE) {
    		appendListStructureToIPDRData(&pFNFData->pListIPDRData_,
				newListIPDRData->pListIPDRStructureData_,
        		IPDRDATA_STRUCTURE);
   	}
		else if (newListIPDRData->IPDRDataType_ == IPDRDATA_BYTE) {
			appendListIPDRData(&(pFNFData->pListIPDRData_ ),
							(void *) newListIPDRData->IPDRDataUnion_.IPDRByte_,
							IPDRDATA_BYTE);
		}
		else if (newListIPDRData->IPDRDataType_ == IPDRDATA_UBYTE) {
			appendListIPDRData(&(pFNFData->pListIPDRData_ ),
							(void *) newListIPDRData->IPDRDataUnion_.IPDRByte_,
							IPDRDATA_BYTE);
		}

		newListIPDRData = newListIPDRData->pNext_; 
	}

	
	newListIPDRData = pFirst;
	freeListIPDRData(&(newListIPDRData));  
	free(data); 
	free(tempString); 
	return (IPDR_OK);	 
}


/*!
*NAME:
*     getAttributeTypeFromSchema() 
*
*DESCRIPTION:
*     This function is called by the XML Read/Write tool to populate 
*      its FNFData.
* 
*ARGUMENTS:
* - Poiter to IPDRCommomnParameters{IN}
* - Pointer to FNFData {IN}
*      - Integer variable to store the error code if it originates
*        in the function {OUT}
*
* RETURNS 
*  Returns Zero or appropriate error code
*/
int getAttributeTypeFromSchema(
							   IPDRCommonParameters* pIPDRCommonParameters,
                               FNFData* pFNFData,
                               int* pErrorCode
                              )
{
	int errorSeverity = 0;
	char* pFNFtypeName = NULL;
	char* tempString = NULL;
	char* serviceTypeToken = NULL;
	char* nameSpaceFnfID = NULL;
	char* nameSpaceFnfURI = NULL;
	char* FNFAttributeName = NULL;
	char* tempFNFAttributeName = NULL;
	List* pListAttrName = NULL;
	SchemaElement* pSchemaElement;
	ListAttributeDescriptor* pCurrentListAttributeDescriptor = NULL; 

	if(pIPDRCommonParameters == NULL || pFNFData == NULL) {
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		return(IPDR_ERROR);
	}
	if(pIPDRCommonParameters->pSchema_->pListSchemaLookup_ == NULL) {
		*pErrorCode = IPDR_NULL_SCHEMA_LOOKUP;
		return(IPDR_WARNING);
	}
	nameSpaceFnfID = (char*) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char)); 
	nameSpaceFnfURI = (char*) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char)); 
	pFNFtypeName = (char *) calloc(strlen(pFNFData->typeName_) + 1, sizeof(char));
	tempString = (char *) calloc(strlen(pFNFData->typeName_) + 1, sizeof(char));
	if(nameSpaceFnfID == NULL || pFNFtypeName == NULL ||
		nameSpaceFnfURI == NULL || tempString == NULL) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		return(IPDR_ERROR);
	}
	
	strcpy(tempString, pFNFData->typeName_);
	serviceTypeToken = strtok(tempString, ":");
	
	if(!strcmp(pFNFData->typeName_,serviceTypeToken)) 
		strcpy(pFNFtypeName, pFNFData->typeName_);
	else {
		getIPDRSubstring(pFNFData->typeName_, pFNFtypeName, ':');
	}	
	free(tempString);
	
	pCurrentListAttributeDescriptor = pFNFData->pListAttributeDescriptor_;
	while(pFNFData->pListAttributeDescriptor_ != NULL) {
		errorSeverity = getNameSpaceID(nameSpaceFnfID, 
			pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);
		if(errorSeverity == IPDR_ERROR){
			*pErrorCode = IPDR_GETID_NULL;
			free(pFNFtypeName);
			free(nameSpaceFnfID);
			free(nameSpaceFnfURI);	
			return (IPDR_ERROR);
		} 
		errorSeverity = getNameSpaceURI(pIPDRCommonParameters->pSchema_->pListNameSpaceInfo_,
										nameSpaceFnfID, 
										nameSpaceFnfURI);
		if(errorSeverity == IPDR_ERROR){
			*pErrorCode = IPDR_GETURI_NULL;
			free(pFNFtypeName);
			free(nameSpaceFnfID);
			free(nameSpaceFnfURI);	
			return(IPDR_WARNING);
		}
		
		FNFAttributeName = (char *) calloc(strlen(pFNFtypeName) +
										   strlen(nameSpaceFnfURI) +
										   strlen (pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_) + 5, 
										   sizeof(char));

		/*tempFNFAttributeName = (char *) calloc (MAX_ATTRIBUTE_NAME * 2, sizeof(char));*/
	
		/*getIPDRSubstring(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_, tempFNFAttributeName, ':');*/

		/*strcpy(tempFNFAttributeName, pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);*/
		
		sprintf(FNFAttributeName, "%s%s%s%s%s", 
								   pFNFtypeName, "$", 
								   nameSpaceFnfURI, "$", 
								   pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);
	
/*	
		if (tempFNFAttributeName != NULL) {
			free(tempFNFAttributeName);		
			tempFNFAttributeName = NULL;
		}
*/

		pSchemaElement = newSchemaElement();
		
 		errorSeverity = getSchemElementFromAttributeName(pIPDRCommonParameters,
										pSchemaElement, 
										FNFAttributeName, 
										pErrorCode);
		if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "getSchemElementFromAttributeName");	
		}
		else if (errorSeverity == IPDR_ERROR) {
			exceptionHandler(pIPDRCommonParameters, pErrorCode, "getSchemElementFromAttributeName");	
			free(FNFAttributeName);
			free(pFNFtypeName);
			free(nameSpaceFnfID);
			free(nameSpaceFnfURI);	
			freeSchemaElement(pSchemaElement); 
			return (IPDR_ERROR);
		}	
		free(FNFAttributeName);

		if(pSchemaElement->attributeName_ == NULL){
			*pErrorCode = IPDR_NO_SUCH_ATTRIBUTE_IN_SCHEMA;
			free(pFNFtypeName);
			free(nameSpaceFnfID);
			free(nameSpaceFnfURI);				
			freeSchemaElement(pSchemaElement); 
			return (IPDR_WARNING);
		}

		strcpy(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeType_, pSchemaElement->attributeType_);   
		/*int len = strlen(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeType_);
		pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeType_[len+1] = '\0';*/
		strcpy(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->derivedType_, pSchemaElement->derivedType_);    
/*
		printf("The pAttributeDescriptor_->attributeType_ : %s\n", pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeType_);
		printf("The pSchemaElement->attributeType_ : %s\n", pSchemaElement->attributeType_);*/

		freeSchemaElement(pSchemaElement); 

		pFNFData->pListAttributeDescriptor_ = pFNFData->pListAttributeDescriptor_->pNext_; 
	}
	pFNFData->pListAttributeDescriptor_ = pCurrentListAttributeDescriptor;
	free(pFNFtypeName);
	free(nameSpaceFnfID);
	free(nameSpaceFnfURI);
	return (IPDR_OK);
}

/*!
*NAME:
*     getComplexAttributeTypeFromSchema() 
*
*DESCRIPTION:
*     This function is called by the XML Read/Write tool to populate 
*      its FNFData for complex datatype.
* 
*ARGUMENTS:
* - Poiter to IPDRCommomnParameters{IN}
* - Pointer to FNFData {IN}
*      - Integer variable to store the error code if it originates
*        in the function {OUT}
*
* RETURNS 
*  Returns Zero or appropriate error code
*/
int getComplexAttributeTypeFromSchema(
							   IPDRCommonParameters* pIPDRCommonParameters,
                               FNFData* pFNFData,
                               int* pErrorCode
                              )
{
	int errorSeverity = 0;
	char* pFNFtypeName = NULL;
	char* tempString = NULL;
	char* serviceTypeToken = NULL;
	char* nameSpaceFnfID = NULL;
	char* nameSpaceFnfURI = NULL;
	char* FNFAttributeName = NULL;
	char* tempFNFAttributeName = NULL;
	char* tempComplexType1 = NULL;
   char * tempString1 = NULL;
	List* pListAttrName = NULL;
	SchemaElement* pSchemaElement;
	ListAttributeDescriptor* pCurrentListAttributeDescriptor = NULL; 
	ListAttributeDescriptor* pCurrentListServiceAttributeDescriptor = NULL; 


	if(pIPDRCommonParameters == NULL || pFNFData == NULL) {
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		return(IPDR_ERROR);
	}
	if(pIPDRCommonParameters->pSchema_->pListSchemaLookup_ == NULL) {
		*pErrorCode = IPDR_NULL_SCHEMA_LOOKUP;
		return(IPDR_WARNING);
	}
	nameSpaceFnfID = (char*) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char)); 
	nameSpaceFnfURI = (char*) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char)); 
	pFNFtypeName = (char *) calloc(strlen(pFNFData->typeName_) + 1, sizeof(char));
	tempString = (char *) calloc(strlen(pFNFData->typeName_) + 1, sizeof(char));
	if(nameSpaceFnfID == NULL || pFNFtypeName == NULL ||
		nameSpaceFnfURI == NULL || tempString == NULL) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		return(IPDR_ERROR);
	}
	
	strcpy(tempString, pFNFData->typeName_);
	serviceTypeToken = strtok(tempString, ":");
	
	if(!strcmp(pFNFData->typeName_,serviceTypeToken)) 
		strcpy(pFNFtypeName, pFNFData->typeName_);
	else {
		getIPDRSubstring(pFNFData->typeName_, pFNFtypeName, ':');
	}	
	free(tempString);
	
	pCurrentListAttributeDescriptor = pFNFData->pListAttributeDescriptor_;
	while(pFNFData->pListAttributeDescriptor_ != NULL) {
		errorSeverity = getNameSpaceID(nameSpaceFnfID, 
			pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);
		if(errorSeverity == IPDR_ERROR){
			*pErrorCode = IPDR_GETID_NULL;
			free(pFNFtypeName);
			free(nameSpaceFnfID);
			free(nameSpaceFnfURI);	
			return (IPDR_ERROR);
		} 
		errorSeverity = getNameSpaceURI(pIPDRCommonParameters->pSchema_->pListNameSpaceInfo_,
										nameSpaceFnfID, 
										nameSpaceFnfURI);
		if(errorSeverity == IPDR_ERROR){
			*pErrorCode = IPDR_GETURI_NULL;
			free(pFNFtypeName);
			free(nameSpaceFnfID);
			free(nameSpaceFnfURI);	
			return(IPDR_WARNING);
		}
		
		FNFAttributeName = (char *) calloc(strlen(pFNFtypeName) +
										   strlen(nameSpaceFnfURI) +
										   strlen (pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_) + 5, 
										   sizeof(char));

		/*tempFNFAttributeName = (char *) calloc (MAX_ATTRIBUTE_NAME * 2, sizeof(char));*/
	
		/*getIPDRSubstring(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_, tempFNFAttributeName, ':');*/

		/*strcpy(tempFNFAttributeName, pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);*/
		
		sprintf(FNFAttributeName, "%s%s%s%s%s", 
								   pFNFtypeName, "$", 
								   nameSpaceFnfURI, "$", 
								   pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);
	
/*	
		if (tempFNFAttributeName != NULL) {
			free(tempFNFAttributeName);		
			tempFNFAttributeName = NULL;
		}
*/

		pSchemaElement = newSchemaElement();
		
 		errorSeverity = getSchemElementFromAttributeName(pIPDRCommonParameters,
										pSchemaElement, 
										FNFAttributeName, 
										pErrorCode);
		if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "getSchemElementFromAttributeName");	
		}
		else if (errorSeverity == IPDR_ERROR) {
			exceptionHandler(pIPDRCommonParameters, pErrorCode, "getSchemElementFromAttributeName");	
			free(FNFAttributeName);
			free(pFNFtypeName);
			free(nameSpaceFnfID);
			free(nameSpaceFnfURI);	
			freeSchemaElement(pSchemaElement); 
			return (IPDR_ERROR);
		}	
		free(FNFAttributeName);

		if(pSchemaElement->attributeName_ == NULL){
			*pErrorCode = IPDR_NO_SUCH_ATTRIBUTE_IN_SCHEMA;
			free(pFNFtypeName);
			free(nameSpaceFnfID);
			free(nameSpaceFnfURI);				
			freeSchemaElement(pSchemaElement); 
			return (IPDR_WARNING);
		}

		pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeType_[0] = '\0';
		pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->derivedType_[0] = '\0';
		strcpy(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeType_, pSchemaElement->attributeType_);   
		strcpy(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->derivedType_, pSchemaElement->derivedType_);    
		/*int len = strlen(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeType_);
		pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeType_[len+1] = '\0';*/
/*
		printf("The pAttributeDescriptor_->attributeType_ : %s\n", pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeType_);
		printf("The pSchemaElement->attributeType_ : %s\n", pSchemaElement->attributeType_);*/

		freeSchemaElement(pSchemaElement); 

		pFNFData->pListAttributeDescriptor_ = pFNFData->pListAttributeDescriptor_->pNext_; 
	}
	pFNFData->pListAttributeDescriptor_ = pCurrentListAttributeDescriptor;

	pCurrentListServiceAttributeDescriptor = pFNFData->pListServiceAttributeDescriptor_;
	while(pFNFData->pListServiceAttributeDescriptor_ != NULL) {
		errorSeverity = getNameSpaceID(nameSpaceFnfID, 
			pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->attributeName_);
		if(errorSeverity == IPDR_ERROR){
			*pErrorCode = IPDR_GETID_NULL;
			free(pFNFtypeName);
			free(nameSpaceFnfID);
			free(nameSpaceFnfURI);	
			return (IPDR_ERROR);
		} 
		errorSeverity = getNameSpaceURI(pIPDRCommonParameters->pSchema_->pListNameSpaceInfo_,
										nameSpaceFnfID, 
										nameSpaceFnfURI);
		if(errorSeverity == IPDR_ERROR){
			*pErrorCode = IPDR_GETURI_NULL;
			free(pFNFtypeName);
			free(nameSpaceFnfID);
			free(nameSpaceFnfURI);	
			return(IPDR_WARNING);
		}
		
		FNFAttributeName = (char *) calloc(strlen(pFNFtypeName) +
										   strlen(nameSpaceFnfURI) +
										   strlen (pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->attributeName_) + 5, 
										   sizeof(char));

		/*tempFNFAttributeName = (char *) calloc (MAX_ATTRIBUTE_NAME * 2, sizeof(char));*/
	
		/*getIPDRSubstring(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_, tempFNFAttributeName, ':');*/

		/*strcpy(tempFNFAttributeName, pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);*/

		FNFAttributeName[0] = '\0';

		if(strstr(pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->attributeName_, pFNFtypeName)
          == NULL) {

			sprintf(FNFAttributeName, "%s%s%s%s%s", 
								   pFNFtypeName, "$", 
								   nameSpaceFnfURI, "$", 
								   pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->attributeName_);
		} else {
			tempString1 = (char *) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char));
   		tempComplexType1 = (char *) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char));			 
	
			strcpy(FNFAttributeName, pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->attributeName_);
			getIPDRSubstring(pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
                        tempComplexType1, ':');
         getIPDRSubstring(tempComplexType1, tempString1, '$');
         strcpy(pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->attributeName_,tempString1);

			free(tempString1);
			free(tempComplexType1);
		}
	
/*	
		if (tempFNFAttributeName != NULL) {
			free(tempFNFAttributeName);		
			tempFNFAttributeName = NULL;
		}
*/

		pSchemaElement = newSchemaElement();
		
 		errorSeverity = getSchemElementFromAttributeName(pIPDRCommonParameters,
										pSchemaElement, 
										FNFAttributeName, 
										pErrorCode);
		if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "getSchemElementFromAttributeName");	
		}
		else if (errorSeverity == IPDR_ERROR) {
			exceptionHandler(pIPDRCommonParameters, pErrorCode, "getSchemElementFromAttributeName");	
			free(FNFAttributeName);
			free(pFNFtypeName);
			free(nameSpaceFnfID);
			free(nameSpaceFnfURI);	
			freeSchemaElement(pSchemaElement); 
			return (IPDR_ERROR);
		}	
		free(FNFAttributeName);

		if(pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->complexType_ != NULL){
		if(strcmp(pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->complexType_,"structure") != 0)
		{
			if(pSchemaElement->attributeName_ == NULL){
				*pErrorCode = IPDR_NO_SUCH_ATTRIBUTE_IN_SCHEMA;
				free(pFNFtypeName);
				free(nameSpaceFnfID);
				free(nameSpaceFnfURI);				
				freeSchemaElement(pSchemaElement); 
				return (IPDR_WARNING);
			}

		pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->attributeType_[0] = '\0';
		pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->derivedType_[0] = '\0';
		strcpy(pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->attributeType_, pSchemaElement->attributeType_);   
		strcpy(pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->derivedType_, pSchemaElement->derivedType_);    
		}
		}
		freeSchemaElement(pSchemaElement); 

		pFNFData->pListServiceAttributeDescriptor_ = pFNFData->pListServiceAttributeDescriptor_->pNext_; 
	}
	pFNFData->pListServiceAttributeDescriptor_ = pCurrentListServiceAttributeDescriptor;

	free(pFNFtypeName);
	free(nameSpaceFnfID);
	free(nameSpaceFnfURI);
	return (IPDR_OK);
}

int convToEntityReference(char* inputString, char* outputString)
{
	char* tempString;
	char* tempString1;
	char* token;
	
	tempString = (char *) calloc(strlen(inputString) + 1, sizeof(char));
	tempString1 = (char *) calloc(strlen(inputString) + 1, sizeof(char));
	
	strcpy(tempString, inputString);
	token = strtok(tempString, "&");
	if(!strcmp(inputString, token)) 
		strcpy(outputString, inputString);
	else {
		getIPDRSubstring(inputString, tempString1, '&');
		sprintf(outputString, "%s%s%s", tempString, "&amp;", tempString1);   
	}

	free(tempString);
	free(tempString1); 

	tempString = (char *) calloc(strlen(inputString) + 1, sizeof(char));
	tempString1 = (char *) calloc(strlen(inputString) + 1, sizeof(char));
	
	strcpy(tempString, inputString);
	token = strtok(tempString, "<");
	if(strcmp(inputString, token)) {
		getIPDRSubstring(inputString, tempString1, '<');
		sprintf(outputString, "%s%s%s", tempString, "&lt;", tempString1);   
	}

	free(tempString);
	free(tempString1); 
	
	return (IPDR_OK);
}


int isPrimitiveComplexType(char *tempNewAttrType)
{

	if((tempNewAttrType == NULL)) {
		return(IPDR_ERROR);
	}

	if(tempNewAttrType[0] == '\0') {
		return(IPDR_ERROR);
	}

	if(strcmp(tempNewAttrType,"type") == 0) {
		return(IPDR_ERROR);
	}

	
   if ((strcmp(tempNewAttrType, SCHEMA_LOOKUP_LONG) == 0) ||
		(strcmp(tempNewAttrType, SCHEMA_LOOKUP_LONG_OTHER) == 0) ||
        (strcmp(tempNewAttrType, SCHEMA_LOOKUP_U_LONG) == 0) ||
        (strcmp(tempNewAttrType, SCHEMA_LOOKUP_HYPER) == 0) ||
        (strcmp(tempNewAttrType, SCHEMA_LOOKUP_U_HYPER) == 0) ||
		(strcmp(tempNewAttrType, SCHEMA_LOOKUP_FLOAT) == 0) ||
		(strcmp(tempNewAttrType, SCHEMA_LOOKUP_DOUBLE) == 0) ||
		(strcmp(tempNewAttrType, SCHEMA_LOOKUP_STRING) == 0) ||
		(strcmp(tempNewAttrType, SCHEMA_LOOKUP_BYTE_ARRAY) == 0) ||
		(strcmp(tempNewAttrType, SCHEMA_LOOKUP_HEX_BIN_ARRAY) == 0) ||
		(strcmp(tempNewAttrType, SCHEMA_LOOKUP_ARRAY) == 0) ||
		(strcmp(tempNewAttrType, SCHEMA_LOOKUP_STRUCTURE) == 0) ||
		(strcmp(tempNewAttrType, SCHEMA_LOOKUP_BOOLEAN) == 0) ||
		(strcmp(tempNewAttrType, SCHEMA_LOOKUP_SHORT) == 0) ||
		(strcmp(tempNewAttrType, SCHEMA_LOOKUP_U_SHORT) == 0) ||
		(strcmp(tempNewAttrType, SCHEMA_LOOKUP_BYTE) == 0) ||
		(strcmp(tempNewAttrType, SCHEMA_LOOKUP_U_BYTE) == 0) ||
		(strcmp(tempNewAttrType,SCHEMA_LOOKUP_DERIVED_DATETIME) == 0) ||
		(strcmp(tempNewAttrType,SCHEMA_LOOKUP_DERIVED_DATETIME_MILISEC) == 0) ||
		(strcmp(tempNewAttrType,SCHEMA_LOOKUP_DERIVED_IPV4ADDR) == 0) ||
		(strcmp(tempNewAttrType,SCHEMA_LOOKUP_DERIVED_IPV6ADDR) == 0) ||
		(strcmp(tempNewAttrType,SCHEMA_LOOKUP_DERIVED_IPADDR) == 0) ||
		(strcmp(tempNewAttrType,SCHEMA_LOOKUP_DERIVED_DATETIME_MICROSEC) == 0) ||
		(strcmp(tempNewAttrType,SCHEMA_LOOKUP_DERIVED_UUID) == 0))
		return (IPDR_TRUE);
   else
    return (IPDR_FALSE);
}


/*!
 *  NAME:
 *      writeIPDRXMLComplexRecordHelper() - writes an FNFData structure to the output stream for complex data.    
 *                     
 *  DESCRIPTION:
 *    -This function will be used to write an FNFData structure to the output 
 *       stream. 
 *		-This fxn calls write_xml Marshalling function to actually write the 
 *		 output data into XML Stream.
 *  
 *	ARGUMENTS:
 *      - IPDRCommonParameters (IN/OUT)
 *      - FNFData {IN}
 *		- pErrorCode {IN / OUT } 
 *
 *  RETURNS 	
 *	    Returns Zero or appropriate error code
*/
writeIPDRXMLComplexRecordHelper(FNFData *pFNFData,char *pComplexType,IPDRCommonParameters* pIPDRCommonParameters,
										  int* pErrorCode)
{
	int count = 0;
	int countNext = 0;
   int cnt = 0;
	int errorSeverity = 0;
	int attributeDescriptorLength = 0;
	int typeCode = 0;
   int finalListFlag = 0;
	int noOfElements = 0;
	char *resultGet = NULL;
	char derivedType[100];
	char* tempComplexType;
   char * tempString;
	char* resultString;
	char* serviceTypeToken = NULL;
	char *tempXMLString = NULL;
	char *stringIPDRCreationTime = NULL;
 	ListIPDRData* pCurrentListIPDRData = NULL;
 	ListAttributeDescriptor* pCurrentListServiceAttributeDescriptor = NULL;
 	ListAttributeDescriptor* pOldListServiceAttributeDescriptor = NULL;
 	ListAttributeDescriptor* pTempOldListServiceAttributeDescriptor = NULL;
 	ListAttributeDescriptor* pCurrServiceList = NULL;
   ListIPDRComplexData* pTempListIPDRComplexData = NULL;
   ListIPDRComplex* pTempListIPDRComplex = NULL;
   ListIPDRArrayData* pCurrentListIPDRArrayData = NULL;
	List* pcurrentMaxOccrsList = NULL;
	List* pcurrent = NULL;

	hyper* IPDRCreationTime = (hyper *) malloc (sizeof(hyper));


	if(pFNFData == NULL || pIPDRCommonParameters == NULL) {
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		return(IPDR_ERROR);
	}

	resultGet = (char *) calloc (sizeof(char), MAX_XML_STRING_LENGTH);
	tempXMLString = (char*) calloc(MAX_XML_STRING_LENGTH, sizeof(char));
	stringIPDRCreationTime = (char*) calloc(MAX_XML_STRING_LENGTH, sizeof(char));

	if(IPDRCreationTime == NULL  || tempXMLString == NULL || stringIPDRCreationTime == NULL) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		free(resultGet);
		free(tempXMLString);
		free(IPDRCreationTime);
		free(stringIPDRCreationTime);
		return(IPDR_ERROR);
	}

	attributeDescriptorLength = getLengthListAttributeDescriptor(pFNFData->pListServiceAttributeDescriptor_);

	tempString = (char *) calloc(strlen(pComplexType) + 1, sizeof(char));
	tempComplexType = (char *) calloc(strlen(pComplexType) + 1, sizeof(char));
	strcpy(tempString, pComplexType);
	serviceTypeToken = strtok(tempString, ":");
	
	if(strcmp(pComplexType,serviceTypeToken)) {
		getIPDRSubstring(pComplexType, tempComplexType, ':');
   	strcpy(pComplexType,tempComplexType);
	}	
	free(tempString);
	free(tempComplexType);

	pCurrentListServiceAttributeDescriptor = pFNFData->pListServiceAttributeDescriptor_;
   pOldListServiceAttributeDescriptor = pFNFData->pListServiceAttributeDescriptor_;
   pTempOldListServiceAttributeDescriptor = pFNFData->pListServiceAttributeDescriptor_;
   pTempListIPDRComplex = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_-> pListIPDRComplex_;


	for (count = 0; count < attributeDescriptorLength; count++) {
		if(strcmp(pComplexType, 
					 pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->attributeName_) == 0) {
			pcurrent = pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->pListAttrNameList_;
			if(pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->pListAttrMaxOccursList_ != NULL) {
				pcurrentMaxOccrsList = pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->pListAttrMaxOccursList_;
			}
			while((pcurrent != NULL))	{
				pOldListServiceAttributeDescriptor = pTempOldListServiceAttributeDescriptor;
				for (countNext = 0; countNext < attributeDescriptorLength; countNext++) {
				if(strcmp(pcurrent->node, 
					   pOldListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_) == 0) {
						tempString = (char *) calloc(strlen(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_) + 1, sizeof(char));
						tempComplexType = (char *) calloc(strlen(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_) + 1, sizeof(char));
						strcpy(tempString, pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_);
						serviceTypeToken = strtok(tempString, ":");
	
				if(strcmp(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_,serviceTypeToken)) {
					getIPDRSubstring(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_, 
								tempComplexType, ':');
   				strcpy(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_,tempComplexType);
				}	
				free(tempString);
				free(tempComplexType);
					if(isPrimitiveComplexType(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_) == IPDR_TRUE) {

					typeCode = convAttributeTypeToTypeCode(
											pOldListServiceAttributeDescriptor->pAttributeDescriptor_->attributeType_);

              	/*if(!strcmp(pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->attributeType_, 
              		pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->derivedType_)) */
              	if(!strcmp(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->attributeType_, 
              		pOldListServiceAttributeDescriptor->pAttributeDescriptor_->derivedType_)) {
              
              		switch (typeCode)
              		{
              			case IPDRDATA_LONG:

              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
              						sprintf(tempXMLString, "<%s>%d</%s>\n", 
              							pcurrent->node,
              							pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRLong_, 
              							pcurrent->node);
              						write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
              				}
              				else {
              						sprintf(tempXMLString, "<%s>%d</%s>\n", 
              							pcurrent->node,
              							pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRLong_, 
              							pcurrent->node);
              						write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
              				}
                          break;
              
              			case IPDRDATA_ULONG: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
              						sprintf(tempXMLString, "<%s>%u</%s>\n", 
              							pcurrent->node,
              							pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRULong_, 
              							pcurrent->node);
              						write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
              				}
              				else {
              					sprintf(tempXMLString, "<%s>%u</%s>\n", 
              					pcurrent->node,
              					pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRULong_, 
              					pcurrent->node);
              						write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
              				}
                          break;
              
                       case IPDRDATA_LONGLONG: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
              						/*sprintf(tempXMLString, "<%s>%.lf</%s>\n", */
              						sprintf(tempXMLString, "<%s>%lld</%s>\n",
              							pcurrent->node,
              							pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRLongLong_, 
              							pcurrent->node);
              						write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
              				}
              				else {
              					sprintf(tempXMLString, "<%s>%lld</%s>\n", 
              						pcurrent->node,
              						pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRLongLong_, 
              						pcurrent->node);
              						write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
              				}
                         break;
              
                       case IPDRDATA_ULONGLONG: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
              						/*sprintf(tempXMLString, "<%s>%Ld</%s>\n", */
              						sprintf(tempXMLString, "<%s>%llu</%s>\n", 
              							pcurrent->node,
              							pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRULongLong_, 
              							pcurrent->node);
              						write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
              				}
              				else {
              				/*sprintf(tempXMLString, "<%s>%Ld</%s>\n", */
              				sprintf(tempXMLString, "<%s>%llu</%s>\n", 
              					pcurrent->node,
              					pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRULongLong_, 
              					pcurrent->node);
              						write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
              				}
                          break;
              
                       case IPDRDATA_FLOAT: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
              						sprintf(tempXMLString, "<%s>%f</%s>\n", 
              							pcurrent->node,
              							pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRFloat_, 
              							pcurrent->node);
              						write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
              				}
              				else {
              					sprintf(tempXMLString, "<%s>%f</%s>\n", 
              						pcurrent->node,
              						pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRFloat_, 
              						pcurrent->node);
              						write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
              				}
              				break;
              
                       case IPDRDATA_DOUBLE: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
              						sprintf(tempXMLString, "<%s>%lf</%s>\n", 
              							pcurrent->node,
              							pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRDouble_, 
              							pcurrent->node);
              						write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
              				}
              				else {
              					sprintf(tempXMLString, "<%s>%lf</%s>\n", 
              						pcurrent->node,
              						pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRDouble_, 
              						pcurrent->node);
              						write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
              				}
              				break;
              
                       case IPDRDATA_HEXADECARRAY: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
              						sprintf(tempXMLString, "<%s>%s</%s>\n", 
              							pcurrent->node,
              							pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRByteArray_, 
              							pcurrent->node);
              						write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
              				}
              				else {
              					sprintf(tempXMLString, "<%s>%s</%s>\n", 
              						pcurrent->node,
              						pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRByteArray_, 
              						pcurrent->node);
              						write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
              				}
              				break;
              
                       case IPDRDATA_STRING: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
              						resultString = (char *) calloc(strlen((char *)pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_) + 10, sizeof(char));
              						convToEntityReference((char *)pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_, resultString);
              					
              						sprintf(tempXMLString, "<%s>%s</%s>\n", 
              							pcurrent->node,
              							resultString, 
              							pcurrent->node);
              						free(resultString); 
              
              						write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
              				}
              				else {
              					resultString = (char *) calloc(strlen((char *)pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_) + 10, sizeof(char));
              					convToEntityReference((char *)pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_, resultString);
              					
              					sprintf(tempXMLString, "<%s>%s</%s>\n", 
              					pcurrent->node,
              					resultString, 
              					pcurrent->node);
              					free(resultString); 
              					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
              				}
              				break;
              			
                       case IPDRDATA_BOOLEAN: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
              						sprintf(tempXMLString, "<%s>%s</%s>\n", 
              							pcurrent->node,
              							pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRBoolean_, 
              							pcurrent->node);
              						write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
              				}
              				else {
              					sprintf(tempXMLString, "<%s>%s</%s>\n", 
              						pcurrent->node,
              						pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_-> pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRBoolean_,
              						pcurrent->node);
              					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
              				}
              				break;
              
              			case IPDRDATA_SHORT: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
              						sprintf(tempXMLString, "<%s>%d</%s>\n", 
              							pcurrent->node,
              							pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRShort_, 
              							pcurrent->node);
              						write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
              				}
              				else {
              					sprintf(tempXMLString, "<%s>%d</%s>\n", 
              						pcurrent->node,
              						pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_-> pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRShort_,
              						pcurrent->node);
              					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
              				}
                          break;
              
              			case IPDRDATA_USHORT: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
              						sprintf(tempXMLString, "<%s>%u</%s>\n", 
              							pcurrent->node,
              							pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUShort_, 
              							pcurrent->node);
              						write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
              				}
              				else {
              				sprintf(tempXMLString, "<%s>%u</%s>\n", 
              					pcurrent->node,
              					pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_-> pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUShort_,
              					pcurrent->node);
              					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
              				}
                          break;
              			case IPDRDATA_BYTE:

              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
              						sprintf(tempXMLString, "<%s>%d</%s>\n", 
              							pcurrent->node,
              							pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRByte_, 
              							pcurrent->node);
              						write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
              				}
              				else {
              						sprintf(tempXMLString, "<%s>%d</%s>\n", 
              							pcurrent->node,
              							pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRByte_, 
              							pcurrent->node);
              						write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
              				}
                          break;
              
              			default:
              					return(IPDR_NON_PRIMITIVE_DATA_TYPE);
              					break;
                      }
              	} else {
              
              		/*convertToUpper(pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->derivedType_, derivedType);*/
              		convertToUpper(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->derivedType_, derivedType);
              		strcpy(resultGet,"");
              
              		if(!strcmp(derivedType, "IPV4ADDR")) {
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              				if((pcurrentMaxOccrsList != NULL)) {
              					noOfElements = atoi(pcurrentMaxOccrsList->node);
              				}
              
              				if (noOfElements > 0) {
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
              						errorSeverity = convIntToIPv4Addr(pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRLong_, resultGet, pErrorCode);   
              						if (errorSeverity == IPDR_WARNING) {
              							errorHandler(pIPDRCommonParameters, pErrorCode, "convIntToIPv4Addr"); 
              						}
              						else if (errorSeverity == IPDR_ERROR) {
              							pIPDRCommonParameters->DocState_ = NONWRITABLE;
              							exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLRecordHelper"); 
              							free(resultGet);
              							free(tempXMLString);
              							free(IPDRCreationTime);
              							free(stringIPDRCreationTime);
              							return (IPDR_ERROR);
              						}
              						sprintf(tempXMLString, "<%s>%s</%s>\n", 
              							pcurrent->node,
              							resultGet, 
              							pcurrent->node);
              						write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
              				}
              				else {
              					errorSeverity = convIntToIPv4Addr(pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRLong_, resultGet, pErrorCode);   
              					if (errorSeverity == IPDR_WARNING) {
              						errorHandler(pIPDRCommonParameters, pErrorCode, "convIntToIPv4Addr"); 
              					}
              					else if (errorSeverity == IPDR_ERROR) {
              						pIPDRCommonParameters->DocState_ = NONWRITABLE;
              						exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLRecordHelper"); 
              						free(resultGet);
              						free(tempXMLString);
              						free(IPDRCreationTime);
              						free(stringIPDRCreationTime);
              						return (IPDR_ERROR);
              					}
              					sprintf(tempXMLString, "<%s>%s</%s>\n", 
              							pcurrent->node,
              							resultGet, 
              							pcurrent->node);
              					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
              				}
              		}
              		else if(!strcmp(derivedType,"IPV6ADDR")) {
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
              						/*errorSeverity = convByteArrayToIPV6Addr(pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRByteArray_, resultGet, pErrorCode);   
              						if (errorSeverity == IPDR_WARNING) {
              							errorHandler(pIPDRCommonParameters, pErrorCode, "convByteArrayToIPV6Addr"); 
              						}
              						else if (errorSeverity == IPDR_ERROR) {
              							pIPDRCommonParameters->DocState_ = NONWRITABLE;
              							exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLRecordHelper"); 
              							free(resultGet);
              							free(tempXMLString);
              							free(IPDRCreationTime);
              							free(stringIPDRCreationTime);
              							return (IPDR_ERROR);
              						}
              						sprintf(tempXMLString, "<%s>%s</%s>\n", 
              							pcurrent->node,
              							resultGet, 
              							pcurrent->node);*/
              						sprintf(tempXMLString, "<%s>%s</%s>\n", 
              							pcurrent->node,
              							pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRByteArray_, 
              							pcurrent->node);
              						write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
              				}
              				else {
              					/*errorSeverity = convByteArrayToIPV6Addr(pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRByteArray_, resultGet, pErrorCode);   
              					if (errorSeverity == IPDR_WARNING) {
              						errorHandler(pIPDRCommonParameters, pErrorCode, "convByteArrayToIPV6Addr"); 
              					}
              					else if (errorSeverity == IPDR_ERROR) {
              						pIPDRCommonParameters->DocState_ = NONWRITABLE;
              						exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLRecordHelper"); 
              						free(resultGet);
              						free(tempXMLString);
              						free(IPDRCreationTime);
              						free(stringIPDRCreationTime);
              						return (IPDR_ERROR);
              					}
              					sprintf(tempXMLString, "<%s>%s</%s>\n", 
              							pcurrent->node,
              							resultGet, 
              							pcurrent->node);*/
              					sprintf(tempXMLString, "<%s>%s</%s>\n", 
              							pcurrent->node,
              							pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRByteArray_, 
              							pcurrent->node);

              					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
              				}
              		}
              		else if(!strcmp(derivedType,"UUID")) {
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
              						/*errorSeverity = convByteArrayToUUID(pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRByteArray_, resultGet, pErrorCode);   
              						if (errorSeverity == IPDR_WARNING) {
              							errorHandler(pIPDRCommonParameters, pErrorCode, "convByteArrayToUUID"); 
              						}
              						else if (errorSeverity == IPDR_ERROR) {
              							pIPDRCommonParameters->DocState_ = NONWRITABLE;
              							exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLRecordHelper"); 
              							free(resultGet);
              							free(tempXMLString);
              							free(IPDRCreationTime);
              							free(stringIPDRCreationTime);
              							return (IPDR_ERROR);
              						}
              						sprintf(tempXMLString, "<%s>%s</%s>\n", 
              							pcurrent->node,
              							resultGet, 
              							pcurrent->node);*/
              						sprintf(tempXMLString, "<%s>%s</%s>\n", 
              							pcurrent->node,
              							pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRByteArray_, 
              							pcurrent->node);
              						write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
              				}
              				else {
              					/*errorSeverity = convByteArrayToUUID(pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRByteArray_, resultGet, pErrorCode);   
              					if (errorSeverity == IPDR_WARNING) {
              						errorHandler(pIPDRCommonParameters, pErrorCode, "convByteArrayToUUID"); 
              					}
              					else if (errorSeverity == IPDR_ERROR) {
              						pIPDRCommonParameters->DocState_ = NONWRITABLE;
              						exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLRecordHelper"); 
              						free(resultGet);
              						free(tempXMLString);
              						free(IPDRCreationTime);
              						free(stringIPDRCreationTime);
              						return (IPDR_ERROR);
              					}
              					sprintf(tempXMLString, "<%s>%s</%s>\n", 
              							pcurrent->node,
              							resultGet, 
              							pcurrent->node);*/
              					sprintf(tempXMLString, "<%s>%s</%s>\n", 
              							pcurrent->node,
              							pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRByteArray_, 
              							pcurrent->node);
              					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
              				}
              		}
              		else if(!strcmp(derivedType,"DATETIME")) {
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
              						errorSeverity = convIntToDateTime(&pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRLong_, resultGet, pErrorCode);   
              						if (errorSeverity == IPDR_WARNING) {
              							errorHandler(pIPDRCommonParameters, pErrorCode, "convIntToDateTime"); 
              						}
              						else if (errorSeverity == IPDR_ERROR) {
              							pIPDRCommonParameters->DocState_ = NONWRITABLE;
              							exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLRecordHelper"); 
              							free(resultGet);
              							free(tempXMLString);
              							free(IPDRCreationTime);
              							free(stringIPDRCreationTime);
              							return (IPDR_ERROR);
              						}
              						sprintf(tempXMLString, "<%s>%s</%s>\n", 
              							pcurrent->node,
              							resultGet, 
              							pcurrent->node);
              						write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
              				}
              				else {
              					errorSeverity = convIntToDateTime(&pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRLong_, resultGet, pErrorCode);
              					if (errorSeverity == IPDR_WARNING) {
              						errorHandler(pIPDRCommonParameters, pErrorCode, "convIntToDateTime"); 
              					}
              					else if (errorSeverity == IPDR_ERROR) {
              						pIPDRCommonParameters->DocState_ = NONWRITABLE;
              						exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLRecordHelper"); 
              						free(resultGet);
              						free(tempXMLString);
              						free(IPDRCreationTime);
              						free(stringIPDRCreationTime);
              						return (IPDR_ERROR);
              					}
              					sprintf(tempXMLString, "<%s>%s</%s>\n", 
              							pcurrent->node,
              							resultGet, 
              							pcurrent->node);
              					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
              				}
              		}
              		else if(!strcmp(derivedType, "DATETIMEMSEC")) {
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
              						errorSeverity = convLongToTime(&pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRLongLong_, resultGet, pErrorCode);   
              						if (errorSeverity == IPDR_WARNING) {
              							errorHandler(pIPDRCommonParameters, pErrorCode, "convLongToTime"); 
              						}
              						else if (errorSeverity == IPDR_ERROR) {
              							pIPDRCommonParameters->DocState_ = NONWRITABLE;
              							exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLRecordHelper"); 
              							free(resultGet);
              							free(tempXMLString);
              							free(IPDRCreationTime);
              							free(stringIPDRCreationTime);
              							return (IPDR_ERROR);
              						}
              						sprintf(tempXMLString, "<%s>%s</%s>\n", 
              							pcurrent->node,
              							resultGet, 
              							pcurrent->node);
              						write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
              				}
              				else {
              					errorSeverity = convLongToTime(&pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRLongLong_, resultGet, pErrorCode);   
              					if (errorSeverity == IPDR_WARNING) {
              						errorHandler(pIPDRCommonParameters, pErrorCode, "convLongToTime"); 
              					}
              					else if (errorSeverity == IPDR_ERROR) {
              						pIPDRCommonParameters->DocState_ = NONWRITABLE;
              						exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLRecordHelper"); 
              						free(resultGet);
              						free(tempXMLString);
              						free(IPDRCreationTime);
              						free(stringIPDRCreationTime);
              						return (IPDR_ERROR);
              					}
              					sprintf(tempXMLString, "<%s>%s</%s>\n", 
              							pcurrent->node,
              							resultGet, 
              							pcurrent->node);
              					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
              				}
              		} 
              		else if(!strcmp(derivedType,"IPADDR")) {
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
              						/*errorSeverity = convByteArrayToIPAddr(pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRByteArray_, resultGet, pErrorCode);   
              						if (errorSeverity == IPDR_WARNING) {
              							errorHandler(pIPDRCommonParameters, pErrorCode, "convByteArrayToIPAddr"); 
              						}
              						else if (errorSeverity == IPDR_ERROR) {
              							pIPDRCommonParameters->DocState_ = NONWRITABLE;
              							exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLRecordHelper"); 
              							free(resultGet);
              							free(tempXMLString);
              							free(IPDRCreationTime);
              							free(stringIPDRCreationTime);
              							return (IPDR_ERROR);
              						}
              						sprintf(tempXMLString, "<%s>%s</%s>\n", 
              							pcurrent->node,
              							resultGet, 
              							pcurrent->node);*/
              						sprintf(tempXMLString, "<%s>%s</%s>\n", 
              							pcurrent->node,
              							pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRByteArray_, 
              							pcurrent->node);
              						write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
              				}
              				else {
              					/*errorSeverity = convByteArrayToIPAddr(pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRByteArray_, resultGet, pErrorCode);   
              					if (errorSeverity == IPDR_WARNING) {
              						errorHandler(pIPDRCommonParameters, pErrorCode, "convByteArrayToIPAddr"); 
              					}
              					else if (errorSeverity == IPDR_ERROR) {
              						pIPDRCommonParameters->DocState_ = NONWRITABLE;
              						exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLRecordHelper"); 
              						free(resultGet);
              						free(tempXMLString);
              						free(IPDRCreationTime);
              						free(stringIPDRCreationTime);
              						return (IPDR_ERROR);
              					}*/
              						sprintf(tempXMLString, "<%s>%s</%s>\n", 
              							pcurrent->node,
              							pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRByteArray_, 
              							pcurrent->node);
              						write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
              				}
              		}
              		else if(!strcmp(derivedType, "DATETIMEUSEC")) {
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
              						errorSeverity = convLongMicroToTime(&pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRByteArray_, resultGet, pErrorCode);   
              						if (errorSeverity == IPDR_WARNING) {
              							errorHandler(pIPDRCommonParameters, pErrorCode, "convLongMicroToTime"); 
              						}
              						else if (errorSeverity == IPDR_ERROR) {
              							pIPDRCommonParameters->DocState_ = NONWRITABLE;
              							exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLRecordHelper"); 
              							free(resultGet);
              							free(tempXMLString);
              							free(IPDRCreationTime);
              							free(stringIPDRCreationTime);
              							return (IPDR_ERROR);
              						}
              						sprintf(tempXMLString, "<%s>%s</%s>\n", 
              							pcurrent->node,
              							resultGet, 
              							pcurrent->node);
              						write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
              				}
              				else {
              					errorSeverity = convLongMicroToTime(&pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRLongLong_, resultGet, pErrorCode);   
              					if (errorSeverity == IPDR_WARNING) {
              						errorHandler(pIPDRCommonParameters, pErrorCode, "convMicroTimeToLong"); 
              					}
              					else if (errorSeverity == IPDR_ERROR) {
              						pIPDRCommonParameters->DocState_ = NONWRITABLE;
              						exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLRecordHelper"); 
              						free(resultGet);
              						free(tempXMLString);
              						free(IPDRCreationTime);
              						free(stringIPDRCreationTime);
              						return (IPDR_ERROR);
              					}
              					sprintf(tempXMLString, "<%s>%s</%s>\n", 
              						pcurrent->node,
              						resultGet, 
              						pcurrent->node);
              					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
              				}
              		} 
              	}
				}
				else {
					pCurrServiceList = pTempOldListServiceAttributeDescriptor;
					sprintf(tempXMLString, "<%s>\n", 
						pOldListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_);
					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
				   pFNFData->pListServiceAttributeDescriptor_ = pTempOldListServiceAttributeDescriptor;	
					writeIPDRXMLComplexRecordHelper(pFNFData,
													  pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_,
													  pIPDRCommonParameters, pErrorCode);
					sprintf(tempXMLString, "</%s>\n", 
						pOldListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_);
					write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
				}
			}
			pOldListServiceAttributeDescriptor = pOldListServiceAttributeDescriptor->pNext_;
		}
					pcurrent = pcurrent->pNext;
					if(pcurrentMaxOccrsList != NULL)
						pcurrentMaxOccrsList = pcurrentMaxOccrsList->pNext;
					pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_ = 
					pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pNext_;
			}
		}		
		pFNFData->pListServiceAttributeDescriptor_ = pFNFData->pListServiceAttributeDescriptor_->pNext_;
      pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_-> pListIPDRComplex_ = pTempListIPDRComplex;
	}
	
   pFNFData->pListServiceAttributeDescriptor_ = pCurrentListServiceAttributeDescriptor;

	free(resultGet);
	free(tempXMLString);
	free(IPDRCreationTime);
	free(stringIPDRCreationTime);


	return (IPDR_OK);
}

int populateComplexDataTypeWithSchema(char* pComplexType,
												  FNFData*		 pFNFData,
												  ListIPDRComplex** pListIPDRComplex,
												  int*	pErrorCode)
{
	int count = 0;
	int countNext = 0;
   int cnt = 0;
   int lLoopFlag = 0;
	int errorSeverity = 0;
	int attributeDescriptorLength = 0;
	int typeCode = 0;
   int finalListFlag = 0;
	int noOfElements = 0;
	int tempLong = 0;
	char *resultGet = NULL;
	char derivedType[100];
	char* tempComplexType;
   char * tempString;
	char* resultString;
	char* serviceTypeToken = NULL;
	char* endPtr;
	char *tempXMLString = NULL;
	char *stringIPDRCreationTime = NULL;
	void* dataValue[MAX_IPDR_STRING];
	void* IPDRValue = NULL;
	void* data = NULL;

 	ListIPDRData* pCurrentListIPDRData = NULL;
 	IPDRData* pIPDRData = NULL;
 	ListAttributeDescriptor* pCurrentListServiceAttributeDescriptor = NULL;
 	ListAttributeDescriptor* pOldListServiceAttributeDescriptor = NULL;
 	ListAttributeDescriptor* pTempOldListServiceAttributeDescriptor = NULL;
 	ListAttributeDescriptor* pCurrServiceList = NULL;
   ListIPDRComplexData* pTempListIPDRComplexData = NULL;
   ListIPDRComplex* pTempListIPDRComplex = NULL;
   ListIPDRArrayData* pCurrentListIPDRArrayData = NULL;
	List* pcurrentMaxOccrsList = NULL;
	List* pcurrent = NULL;

	hyper* IPDRCreationTime = (hyper *) malloc (sizeof(hyper));
	data = (void *) malloc (MAX_IPDR_STRING);


/*
	if(pFNFData == NULL || pIPDRCommonParameters == NULL) {
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		return(IPDR_ERROR);
	}
*/

	resultGet = (char *) calloc (sizeof(char), MAX_XML_STRING_LENGTH);
	tempXMLString = (char*) calloc(MAX_XML_STRING_LENGTH, sizeof(char));
	stringIPDRCreationTime = (char*) calloc(MAX_XML_STRING_LENGTH, sizeof(char));

	if(IPDRCreationTime == NULL  || tempXMLString == NULL || stringIPDRCreationTime == NULL) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		free(resultGet);
		free(tempXMLString);
		free(IPDRCreationTime);
		free(stringIPDRCreationTime);
		return(IPDR_ERROR);
	}

	attributeDescriptorLength = getLengthListAttributeDescriptor(pFNFData->pListServiceAttributeDescriptor_);

	tempString = (char *) calloc(strlen(pComplexType) + 1, sizeof(char));
	tempComplexType = (char *) calloc(strlen(pComplexType) + 1, sizeof(char));
	strcpy(tempString, pComplexType);
	serviceTypeToken = strtok(tempString, ":");
	
	if(strcmp(pComplexType,serviceTypeToken)) {
		getIPDRSubstring(pComplexType, tempComplexType, ':');
   	strcpy(pComplexType,tempComplexType);
	}	
	free(tempString);
	free(tempComplexType);

	pCurrentListServiceAttributeDescriptor = pFNFData->pListServiceAttributeDescriptor_;
   pOldListServiceAttributeDescriptor = pFNFData->pListServiceAttributeDescriptor_;
   pTempOldListServiceAttributeDescriptor = pFNFData->pListServiceAttributeDescriptor_;
   pTempListIPDRComplex = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_-> pListIPDRComplex_;


	for (count = 0; count < attributeDescriptorLength; count++) {
		if(strcmp(pComplexType, 
					 pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->attributeName_) == 0) {
			pcurrent = pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->pListAttrNameList_;
			if(pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->pListAttrMaxOccursList_ != NULL) {
				pcurrentMaxOccrsList = pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->pListAttrMaxOccursList_;
			}
			while((pcurrent != NULL))	{
				pOldListServiceAttributeDescriptor = pTempOldListServiceAttributeDescriptor;
				for (countNext = 0; countNext < attributeDescriptorLength; countNext++) {
				if(strcmp(pcurrent->node, 
					   pOldListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_) == 0) {
						tempString = (char *) calloc(strlen(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_) + 1, sizeof(char));
						tempComplexType = (char *) calloc(strlen(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_) + 1, sizeof(char));
						strcpy(tempString, pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_);
						serviceTypeToken = strtok(tempString, ":");
	
				if(strcmp(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_,serviceTypeToken)) {
					getIPDRSubstring(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_, 
								tempComplexType, ':');
   				strcpy(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_,tempComplexType);
				}	
				free(tempString);
				free(tempComplexType);
					if(isPrimitiveComplexType(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_) == IPDR_TRUE) {

					typeCode = convAttributeTypeToTypeCode(
											pOldListServiceAttributeDescriptor->pAttributeDescriptor_->attributeType_);

              	/*if(!strcmp(pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->attributeType_, 
              		pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->derivedType_)) */
              	if(!strcmp(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->attributeType_, 
              		pOldListServiceAttributeDescriptor->pAttributeDescriptor_->derivedType_)) {
              
              		switch (typeCode)
              		{
              			case IPDRDATA_LONG:
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
									pIPDRData = newIPDRData(IPDRDATA_LONG);
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) {
										pIPDRData->IPDRDataUnion_.IPDRLong_ = atoi((char *)pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_);
   									dataValue[cnt] = (void *)pIPDRData->IPDRDataUnion_.IPDRLong_; 
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_LONG);
              				}
              				else {
									pIPDRData = newIPDRData(IPDRDATA_LONG);
   								pIPDRData->IPDRDataUnion_.IPDRLong_ = atoi((char *)pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_);
   								dataValue[0] = (void *)pIPDRData->IPDRDataUnion_.IPDRLong_;
   								appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_LONG);
              				}
                        break;

              			case IPDRDATA_ULONG: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}

              				if (noOfElements > 0) {
									pIPDRData = newIPDRData(IPDRDATA_ULONG);
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										pIPDRData->IPDRDataUnion_.IPDRULong_ = strtoul((char *) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_, NULL, 10);
   									dataValue[cnt] = (void *)pIPDRData->IPDRDataUnion_.IPDRULong_; 
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_ULONG);
              				}
              				else {
									pIPDRData = newIPDRData(IPDRDATA_ULONG);
   								pIPDRData->IPDRDataUnion_.IPDRULong_ = strtoul((char *) pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_, NULL, 10);
   								dataValue[0] = (void *)pIPDRData->IPDRDataUnion_.IPDRULong_;
   								appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_ULONG);
              				}
                          break;
              
                       case IPDRDATA_LONGLONG: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
									pIPDRData = newIPDRData(IPDRDATA_LONGLONG);
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										pIPDRData->IPDRDataUnion_.IPDRLongLong_=
											(hyper) strtod((char *) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_, &endPtr);			
											 dataValue[cnt] = (void *)&pIPDRData->IPDRDataUnion_.IPDRLongLong_;
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_LONGLONG);
              				}
              				else {
									pIPDRData = newIPDRData(IPDRDATA_LONGLONG);
										pIPDRData->IPDRDataUnion_.IPDRLongLong_=
											(hyper) strtod((char *) pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_, &endPtr);			
   								dataValue[0] = (void *)&pIPDRData->IPDRDataUnion_.IPDRLongLong_;
   								appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_LONGLONG);
              				}
                         break;
              
                       case IPDRDATA_ULONGLONG: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
									pIPDRData = newIPDRData(IPDRDATA_ULONGLONG);
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										pIPDRData->IPDRDataUnion_.IPDRULongLong_=
								(u_hyper) strtod((char *) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_, &endPtr);
										dataValue[cnt] = (void *)&pIPDRData->IPDRDataUnion_.IPDRULongLong_;
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_ULONGLONG);
              				}
              				else {
									pIPDRData = newIPDRData(IPDRDATA_ULONGLONG);
										pIPDRData->IPDRDataUnion_.IPDRULongLong_=
								(u_hyper) strtod((char *) pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_, &endPtr);
									dataValue[0] = (void *)&pIPDRData->IPDRDataUnion_.IPDRULongLong_;
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_ULONGLONG);
              				}
                          break;
              
                       case IPDRDATA_FLOAT: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
									pIPDRData = newIPDRData(IPDRDATA_FLOAT);
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										pIPDRData->IPDRDataUnion_.IPDRFloat_ = 
										(float)atof((char *) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_);
										IPDRValue = (float *) &pIPDRData->IPDRDataUnion_.IPDRFloat_;
										dataValue[cnt] = IPDRValue;
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_FLOAT);
              				}
              				else {
									pIPDRData = newIPDRData(IPDRDATA_FLOAT);
										pIPDRData->IPDRDataUnion_.IPDRFloat_ = 
										(float)atof((char *) pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_);
									IPDRValue = (float *) &pIPDRData->IPDRDataUnion_.IPDRFloat_;
									dataValue[0] = IPDRValue;
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_FLOAT);
              				}
              				break;
              
                       case IPDRDATA_DOUBLE: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
										pIPDRData = newIPDRData(IPDRDATA_DOUBLE);
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										pIPDRData->IPDRDataUnion_.IPDRDouble_= strtod((char *) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_, &endPtr);
										IPDRValue = (double *) &pIPDRData->IPDRDataUnion_.IPDRDouble_;
										dataValue[cnt] = IPDRValue;
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_DOUBLE);
              				}
              				else {
										pIPDRData = newIPDRData(IPDRDATA_DOUBLE);
										pIPDRData->IPDRDataUnion_.IPDRDouble_= strtod((char *) pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_, &endPtr);
										IPDRValue = (double *) &pIPDRData->IPDRDataUnion_.IPDRDouble_;
										dataValue[0] = IPDRValue;
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_DOUBLE);
              				}
              				break;
              
                       case IPDRDATA_HEXADECARRAY: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										dataValue[cnt] = (void *) calloc(1, strlen((char *)pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_)+1);
										strcpy(dataValue[cnt], (char *)pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_);
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_HEXADECARRAY);
              				}
              				else {
										dataValue[0] = (void *) calloc(1, strlen((char *)pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_)+1);
										strcpy(dataValue[cnt], (char *)pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_);
										strcpy(dataValue[0], (char *)pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_);
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_HEXADECARRAY);
              				}
              				break;
              
                       case IPDRDATA_STRING: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										dataValue[cnt] = (void *) calloc(1, strlen((char *)pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_)+1);
										strcpy(dataValue[cnt], (char *)pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_);
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_STRING);
              				}
              				else {
									dataValue[0] = (void *) calloc(1, strlen((char *)pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_)+1);
									strcpy(dataValue[0], (char *)pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_);
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_STRING);
              				}
              				break;
              			
                       case IPDRDATA_BOOLEAN: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										dataValue[cnt] = (void *) calloc(1, strlen((char *)pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_)+1);
										strcpy(dataValue[cnt], (char *)pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_);
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_BOOLEAN);
              				}
              				else {
									dataValue[0] = (void *) calloc(1, strlen((char *)pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_)+1);
									strcpy(dataValue[0], (char *)pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_);
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_BOOLEAN);
              				}
              				break;
              
              			case IPDRDATA_SHORT: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
										pIPDRData = newIPDRData(IPDRDATA_SHORT);
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										pIPDRData->IPDRDataUnion_.IPDRShort_ = atoi((char *) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_);
										dataValue[cnt] = (void *)pIPDRData->IPDRDataUnion_.IPDRShort_;
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_SHORT);
              				}
              				else {
										pIPDRData = newIPDRData(IPDRDATA_SHORT);
										pIPDRData->IPDRDataUnion_.IPDRShort_ = atoi((char *) pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_);
										dataValue[0] = (void *)pIPDRData->IPDRDataUnion_.IPDRShort_;
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_SHORT);
              				}
                          break;
              
              			case IPDRDATA_USHORT: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
										pIPDRData = newIPDRData(IPDRDATA_USHORT);
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										pIPDRData->IPDRDataUnion_.IPDRShort_ = atoi((char *) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_);
										dataValue[cnt] = (void *)pIPDRData->IPDRDataUnion_.IPDRShort_;
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_USHORT);
              				}
              				else {
										pIPDRData->IPDRDataUnion_.IPDRShort_ = atoi((char *) pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_);
										dataValue[0] = (void *)pIPDRData->IPDRDataUnion_.IPDRShort_;
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_USHORT);
              				}
                          break;
              
              			default:
              					return(IPDR_NON_PRIMITIVE_DATA_TYPE);
              					break;
                      }
              	} else {
              
              		/*convertToUpper(pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->derivedType_, derivedType); */
              		convertToUpper(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->derivedType_, derivedType); 
              		strcpy(resultGet,"");
              
              		if(!strcmp(derivedType, "IPV4ADDR")) {
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										convIPv4AddrToInt((char *) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_, 
																&tempLong, pErrorCode);
										dataValue[cnt] = (void *) tempLong; 
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_LONG);
              				}
              				else {
										convIPv4AddrToInt((char *) pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_, 
																&tempLong, pErrorCode);
										dataValue[0] = (void *) tempLong; 
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_LONG);
              				}
              		}
              		else if(!strcmp(derivedType,"IPV6ADDR")) {
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
									/*convIPV6AddrToByteArray((char *) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_, (unsigned char*) data, pErrorCode);
									dataValue[cnt] = (void *) data;*/ 
									dataValue[cnt] =  pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_;
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_HEXADECARRAY);
              				}
              				else {
									/*convIPV6AddrToByteArray((char *) pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_, (unsigned char*) data, pErrorCode); 
									dataValue[0] = (void *) data;*/ 
									dataValue[0] = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_; 
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_HEXADECARRAY);
              				}
              		}
              		else if(!strcmp(derivedType,"UUID")) {
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										/*convUUIDToByteArray((char *) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_,
												(unsigned char*) data, pErrorCode);   
									   dataValue[cnt] = (void *) data; */
									   dataValue[cnt] = (void *) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_; 
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_HEXADECARRAY);
              				}
              				else {
										/*convUUIDToByteArray((char *) pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_,
												(unsigned char*) data, pErrorCode);   
									   dataValue[0] = (void *) data; */
									   dataValue[0] = (void *) pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_; 
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_HEXADECARRAY);
              				}
              		}
              		else if(!strcmp(derivedType,"DATETIME")) {
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										convDateTimeToInt((char *) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_, 
											(int*) data, pErrorCode);
										dataValue[cnt] = (void *) (*(int *) data);
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_LONG);
              				}
              				else {
										convDateTimeToInt((char *) pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_, 
											(int*) data, pErrorCode);
									dataValue[0] = (void *) (*(int *) data);
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_LONG);
              				}
              		}
              		else if(!strcmp(derivedType, "DATETIMEMSEC")) {
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										convTimeToLong((char *) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_, 
										(hyper*) data, pErrorCode);   
										dataValue[cnt] = data;
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_LONGLONG);
              				}
              				else {
										convTimeToLong((char *) pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_, 
										(hyper*) data, pErrorCode);   
										dataValue[0] = data;
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_LONGLONG);
              				}
              		} 
              		else if(!strcmp(derivedType,"IPADDR")) {
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
									/*convIPAddrToByteArray((char *) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_, 
												(unsigned char*) data, pErrorCode);
									dataValue[cnt] = (void *) data;*/
									dataValue[cnt] = (void *) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_;
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_HEXADECARRAY);
              				}
              				else {
									/*convIPAddrToByteArray((char *) pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_, 
												(unsigned char*) data, pErrorCode);
									dataValue[0] = (void *) data;*/
									dataValue[0] = (void *)pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_;
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_HEXADECARRAY);
              				}
              		}
              		else if(!strcmp(derivedType, "DATETIMEUSEC")) {
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										convMicroTimeToLong((char *) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_, 
										(hyper*) data, pErrorCode);   
										dataValue[cnt] = data;
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_LONGLONG);
              				}
              				else {
										convMicroTimeToLong((char *) pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_, 
										(hyper*) data, pErrorCode);   
										dataValue[0] = data;
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_LONGLONG);
              				}
              		} 
              	}
				}
				else {
					pCurrServiceList = pTempOldListServiceAttributeDescriptor;
					populateComplexDataTypeWithSchema(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_,
														 pFNFData,
														 pListIPDRComplex,
														 pErrorCode);
				}
				break;
			}
			pOldListServiceAttributeDescriptor = pOldListServiceAttributeDescriptor->pNext_;
		}
					pcurrent = pcurrent->pNext;
					if(pcurrentMaxOccrsList != NULL)
						pcurrentMaxOccrsList = pcurrentMaxOccrsList->pNext;
					pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_ = 
					pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pNext_;
			}
		}		
		pFNFData->pListServiceAttributeDescriptor_ = pFNFData->pListServiceAttributeDescriptor_->pNext_;
      pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_-> pListIPDRComplex_ = pTempListIPDRComplex;
	}
	
   pFNFData->pListServiceAttributeDescriptor_ = pCurrentListServiceAttributeDescriptor;

	free(resultGet);
	free(tempXMLString);
	free(IPDRCreationTime);
	free(stringIPDRCreationTime);
   free(data);


	return (IPDR_OK);
}

int populateArrayDataTypeWithSchema( AttributeDescriptor* pAttributeDescriptor,
												FNFData*		 pFNFData,
												ListIPDRData** pListIPDRData,
												int*	pErrorCode)
{
	int strIndex = 0;
	int index = 0;
	int count = 0;
	int countFileChars = 0 ;
	int countListNameSpaceInfo = 0;
	int errorSeverity = 0;
	int count2 = 0;
	int len2 = 0;
	int flag2 = 0;
   int dataType[MAX_IPDR_STRING];
	int noOfElements = 0;
	int tempLong = 0;
	int cnt = 0;
	char* token = NULL;
	char* strRead = NULL;
	char* strArrayElemnts = NULL;
	char* tempString = NULL;
	char* stopstring = NULL;
	char* pNameSpcID = NULL;
	char* pNameSpaceID = NULL;
	char* pNameSpaceURI = NULL;
	char* pAttributeName = NULL;
	char* strAttributeName = NULL;
	char seps[]   = "$";
	char* endPtr;
	void* data = NULL;
	void* pIPDRValue = NULL;
	void* dataValue[MAX_IPDR_STRING];
	double tempDouble = 0;
	float tempFloat = 0;
	IPDRData* pIPDRData;
   ListIPDRArrayData* pCurrentListIPDRArrayData = NULL;

    
   memset(dataValue, 0, sizeof(dataValue));
   memset(dataType, 0, sizeof(dataType));

	strRead = (char *) calloc(MAX_IPDR_STRING, sizeof(char));
	tempString = (char *)(calloc(MAX_IPDR_STRING, strlen(pAttributeDescriptor->derivedType_+1)));
	data = (void *) malloc (MAX_IPDR_STRING);

		convertToUpper(pAttributeDescriptor->derivedType_, tempString); 

		if(!strcmp(tempString,"IPV4ADDR")) {
				if(pAttributeDescriptor->noOfElements_ == -1) {
              	noOfElements = pFNFData->pListIPDRData_->noOfElements_;
				}
				else {
              	noOfElements = pFNFData->pListIPDRData_->noOfElements_;
				}
              
             if (noOfElements > 0) {
					pIPDRData = newIPDRData(IPDRDATA_LONG);
              	pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
              	for(cnt = 0;cnt < noOfElements; cnt++) {
						convIPv4AddrToInt((char *) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_, 
								&tempLong, pErrorCode);   
   					dataValue[cnt] = (void *)tempLong; 
              		pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              	}
					dataType[0] = IPDRDATA_ARRAY;
   				dataType[1] = IPDRDATA_LONG;
					appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
					freeIPDRData(pIPDRData);
             }
             else {
					pIPDRData = newIPDRData(IPDRDATA_LONG);
					convIPv4AddrToInt((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, 
								&tempLong, pErrorCode);   
   				dataValue[0] = (void *)tempLong;
					dataType[0] = IPDRDATA_ARRAY;
   				dataType[1] = IPDRDATA_LONG;
					appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
					freeIPDRData(pIPDRData);
              }
		} 
		else if(!strcmp(tempString, "IPV6ADDR")) {
				if(pAttributeDescriptor->noOfElements_ == -1) {
              	noOfElements = pFNFData->pListIPDRData_->noOfElements_;
				}
				else {
              	noOfElements = pFNFData->pListIPDRData_->noOfElements_;
				}
              
             if (noOfElements > 0) {
					pIPDRData = newIPDRData(IPDRDATA_HEXADECARRAY);
              	pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
              	for(cnt = 0;cnt < noOfElements; cnt++) {
						/*convIPV6AddrToByteArray((char *) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_, 
									(unsigned char*) data, pErrorCode);   
   					dataValue[cnt] = (void *)data;*/ 
   					dataValue[cnt] = pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_;
              		pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              	}
					dataType[0] = IPDRDATA_ARRAY;
   				dataType[1] = IPDRDATA_HEXADECARRAY;
					appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
					freeIPDRData(pIPDRData);
             }
             else {
					pIPDRData = newIPDRData(IPDRDATA_HEXADECARRAY);
					/*convIPV6AddrToByteArray((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, 
									(unsigned char*) data, pErrorCode);
   				dataValue[0] = (void *)data;*/
   				dataValue[0] = pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_;
					dataType[0] = IPDRDATA_ARRAY;
   				dataType[1] = IPDRDATA_HEXADECARRAY;
					appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
					freeIPDRData(pIPDRData);
              }
		} 
		else if(!strcmp(tempString, "UUID")) {
				if(pAttributeDescriptor->noOfElements_ == -1) {
              	noOfElements = pFNFData->pListIPDRData_->noOfElements_;
				}
				else {
              	noOfElements = pFNFData->pListIPDRData_->noOfElements_;
				}
              
             if (noOfElements > 0) {
					pIPDRData = newIPDRData(IPDRDATA_HEXADECARRAY);
              	pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
              	for(cnt = 0;cnt < noOfElements; cnt++) {
						/*convUUIDToByteArray((char *) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_,
									(unsigned char*) data, pErrorCode);   
   					dataValue[cnt] = (void *)data; */
   					dataValue[cnt] = (void *)pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_; 
              		pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              	}
					dataType[0] = IPDRDATA_ARRAY;
   				dataType[1] = IPDRDATA_HEXADECARRAY;
					appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
					freeIPDRData(pIPDRData);
             }
             else {
					pIPDRData = newIPDRData(IPDRDATA_HEXADECARRAY);
					/*convUUIDToByteArray((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_,
									(unsigned char*) data, pErrorCode);   
   				dataValue[0] = (void *)data;*/
   				dataValue[0] = (void *)pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_;
					dataType[0] = IPDRDATA_ARRAY;
   				dataType[1] = IPDRDATA_HEXADECARRAY;
					appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
					freeIPDRData(pIPDRData);
              }
		} 
		else if(!strcmp(tempString, "DATETIME")) {
				if(pAttributeDescriptor->noOfElements_ == -1) {
              	noOfElements = pFNFData->pListIPDRData_->noOfElements_;
				}
				else {
              	noOfElements = pFNFData->pListIPDRData_->noOfElements_;
				}
              
             if (noOfElements > 0) {
					pIPDRData = newIPDRData(IPDRDATA_LONG);
              	pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
              	for(cnt = 0;cnt < noOfElements; cnt++) {
					convDateTimeToInt((char *) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_, 
									(int*) data, pErrorCode);
   					dataValue[cnt] = (void *)data; 
              		pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              	}
					dataType[0] = IPDRDATA_ARRAY;
   				dataType[1] = IPDRDATA_LONG;
					appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
					freeIPDRData(pIPDRData);
             }
             else {
					pIPDRData = newIPDRData(IPDRDATA_LONGLONG);
					convDateTimeToInt((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, 
									(int*) data, pErrorCode);
   				dataValue[0] = (void *)data;
					dataType[0] = IPDRDATA_ARRAY;
   				dataType[1] = IPDRDATA_LONG;
					appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
					freeIPDRData(pIPDRData);
              }
		} 
		else if(!strcmp(tempString, "DATETIMEMSEC")) {
				if(pAttributeDescriptor->noOfElements_ == -1) {
              	noOfElements = pFNFData->pListIPDRData_->noOfElements_;
				}
				else {
              	noOfElements = pFNFData->pListIPDRData_->noOfElements_;
				}
              
             if (noOfElements > 0) {
					pIPDRData = newIPDRData(IPDRDATA_LONGLONG);
              	pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
              	for(cnt = 0;cnt < noOfElements; cnt++) {
						convTimeToLong((char *) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_, 
								(hyper*) data, pErrorCode);   
   					dataValue[cnt] = (void *)data; 
              		pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              	}
					dataType[0] = IPDRDATA_ARRAY;
   				dataType[1] = IPDRDATA_LONGLONG;
					appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
					freeIPDRData(pIPDRData);
             }
             else {
					pIPDRData = newIPDRData(IPDRDATA_LONGLONG);
					convTimeToLong((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, 
								(hyper*) data, pErrorCode);   
   				dataValue[0] = (void *)data;
					dataType[0] = IPDRDATA_ARRAY;
   				dataType[1] = IPDRDATA_LONGLONG;
					appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
					freeIPDRData(pIPDRData);
              }
		} 
		else if(!strcmp(tempString, "IPADDR")) {
				if(pAttributeDescriptor->noOfElements_ == -1) {
              	noOfElements = pFNFData->pListIPDRData_->noOfElements_;
				}
				else {
              	noOfElements = pFNFData->pListIPDRData_->noOfElements_;
				}
              
             if (noOfElements > 0) {
					pIPDRData = newIPDRData(IPDRDATA_HEXADECARRAY);
              	pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
              	for(cnt = 0;cnt < noOfElements; cnt++) {
						/*convIPAddrToByteArray((char *) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_, 
									(unsigned char*) data, pErrorCode);   
   					dataValue[cnt] = (void *)data;*/
   					dataValue[cnt] = (void *)pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_;
              		pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              	}
					dataType[0] = IPDRDATA_ARRAY;
   				dataType[1] = IPDRDATA_HEXADECARRAY;
					appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
					freeIPDRData(pIPDRData);
             }
             else {
					pIPDRData = newIPDRData(IPDRDATA_HEXADECARRAY);
					/*convIPAddrToByteArray((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, 
									(unsigned char*) data, pErrorCode);   
   				dataValue[0] = (void *)data;*/
   				dataValue[0] = (void *)pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_;
					dataType[0] = IPDRDATA_ARRAY;
   				dataType[1] = IPDRDATA_HEXADECARRAY;
					appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
					freeIPDRData(pIPDRData);
              }
		} 
		else if(!strcmp(tempString, "DATETIMEUSEC")) {
				if(pAttributeDescriptor->noOfElements_ == -1) {
              	noOfElements = pFNFData->pListIPDRData_->noOfElements_;
				}
				else {
              	noOfElements = pFNFData->pListIPDRData_->noOfElements_;
				}
              
             if (noOfElements > 0) {
					pIPDRData = newIPDRData(IPDRDATA_LONGLONG);
              	pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
              	for(cnt = 0;cnt < noOfElements; cnt++) {
						convMicroTimeToLong((char *) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_, 
										    		(hyper*) data, pErrorCode);   
   					dataValue[cnt] = data; 
              		pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              	}
					dataType[0] = IPDRDATA_ARRAY;
   				dataType[1] = IPDRDATA_LONGLONG;
					appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
					freeIPDRData(pIPDRData);
             }
             else {
					pIPDRData = newIPDRData(IPDRDATA_LONGLONG);
					convMicroTimeToLong((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, 
										    		(hyper*) data, pErrorCode);   
   				dataValue[0] = data;
					dataType[0] = IPDRDATA_ARRAY;
   				dataType[1] = IPDRDATA_LONGLONG;
					appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
					freeIPDRData(pIPDRData);
              }
		} 
		else if(!strcmp(tempString, "BOOLEAN")) {
				if(pAttributeDescriptor->noOfElements_ == -1) {
              	noOfElements = pFNFData->pListIPDRData_->noOfElements_;
				}
				else {
              	noOfElements = pFNFData->pListIPDRData_->noOfElements_;
				}
              
             if (noOfElements > 0) {
					pIPDRData = newIPDRData(IPDRDATA_BOOLEAN);
              	pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
              	for(cnt = 0;cnt < noOfElements; cnt++) {
						strcpy((char *)pIPDRData->IPDRDataUnion_.IPDRBoolean_ ,(char *)pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_);
						dataValue[count] = (void *) calloc (1, strlen((char *)pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_)+1);
    					strcpy((char *)dataValue[count], (char *)pIPDRData->IPDRDataUnion_.IPDRBoolean_);
              		pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              	}
					dataType[0] = IPDRDATA_ARRAY;
   				dataType[1] = IPDRDATA_BOOLEAN;
					appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
					freeIPDRData(pIPDRData);
             }
             else {
					pIPDRData = newIPDRData(IPDRDATA_BOOLEAN);
					strcpy((char *)pIPDRData->IPDRDataUnion_.IPDRBoolean_ ,(char *)pFNFData->pListIPDRData_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_);
					dataValue[0] = (void *) calloc (1, strlen((char *)pFNFData->pListIPDRData_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_)+1);
    				strcpy((char *)dataValue[0], (char *)pIPDRData->IPDRDataUnion_.IPDRBoolean_);
					dataType[0] = IPDRDATA_ARRAY;
   				dataType[1] = IPDRDATA_BOOLEAN;
					appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
					freeIPDRData(pIPDRData);
              }
		} 
		else if(!strcmp(tempString, "SHORT")) {
				if(pAttributeDescriptor->noOfElements_ == -1) {
              	noOfElements = pFNFData->pListIPDRData_->noOfElements_;
				}
				else {
              	noOfElements = pFNFData->pListIPDRData_->noOfElements_;
				}
              
             if (noOfElements > 0) {
					pIPDRData = newIPDRData(IPDRDATA_SHORT);
              	pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
              	for(cnt = 0;cnt < noOfElements; cnt++) {
						pIPDRData->IPDRDataUnion_.IPDRShort_ = atoi((char *)pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_);
   					dataValue[cnt] = (void *)pIPDRData->IPDRDataUnion_.IPDRShort_; 
              		pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              	}
					dataType[0] = IPDRDATA_ARRAY;
   				dataType[1] = IPDRDATA_SHORT;
					appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
					freeIPDRData(pIPDRData);
             }
             else {
					pIPDRData = newIPDRData(IPDRDATA_SHORT);
   				pIPDRData->IPDRDataUnion_.IPDRShort_ = atoi((char *)pFNFData->pListIPDRData_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_);
   				dataValue[0] = (void *)pIPDRData->IPDRDataUnion_.IPDRShort_;
					dataType[0] = IPDRDATA_ARRAY;
   				dataType[1] = IPDRDATA_SHORT;
					appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
					freeIPDRData(pIPDRData);
              }
		} 
		else if(!strcmp(tempString, "UNSIGNED_SHORT")) {
				if(pAttributeDescriptor->noOfElements_ == -1) {
              	noOfElements = pFNFData->pListIPDRData_->noOfElements_;
				}
				else {
              	noOfElements = pFNFData->pListIPDRData_->noOfElements_;
				}
              
             if (noOfElements > 0) {
					pIPDRData = newIPDRData(IPDRDATA_USHORT);
              	pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
              	for(cnt = 0;cnt < noOfElements; cnt++) {
						pIPDRData->IPDRDataUnion_.IPDRUShort_ = atoi((char *)pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_);
   					dataValue[cnt] = (void *)pIPDRData->IPDRDataUnion_.IPDRUShort_; 
              		pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              	}
					dataType[0] = IPDRDATA_ARRAY;
   				dataType[1] = IPDRDATA_USHORT;
					appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
					freeIPDRData(pIPDRData);
             }
             else {
					pIPDRData = newIPDRData(IPDRDATA_USHORT);
   				pIPDRData->IPDRDataUnion_.IPDRUShort_ = atoi((char *)pFNFData->pListIPDRData_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_);
   				dataValue[0] = (void *)pIPDRData->IPDRDataUnion_.IPDRUShort_;
					dataType[0] = IPDRDATA_ARRAY;
   				dataType[1] = IPDRDATA_USHORT;
					appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
					freeIPDRData(pIPDRData);
              }
		}
		/* Collect all the non-blank attributes */
		else if (strcmp("INTEGER", tempString) == 0) {
				if(pAttributeDescriptor->noOfElements_ == -1) {
              	noOfElements = pFNFData->pListIPDRData_->noOfElements_;
				}
				else {
              	noOfElements = pAttributeDescriptor->noOfElements_;
				}
              
             if (noOfElements > 0) {
					pIPDRData = newIPDRData(IPDRDATA_LONG);
              	pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
              	for(cnt = 0;cnt < noOfElements; cnt++) {
						pIPDRData->IPDRDataUnion_.IPDRLong_ = atoi((char *)pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_);
   					dataValue[cnt] = (void *)pIPDRData->IPDRDataUnion_.IPDRLong_; 
              		pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              	}
					dataType[0] = IPDRDATA_ARRAY;
   				dataType[1] = IPDRDATA_LONG;
					appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
					freeIPDRData(pIPDRData);
             }
             else {
					pIPDRData = newIPDRData(IPDRDATA_LONG);
   				pIPDRData->IPDRDataUnion_.IPDRLong_ = atoi((char *)pFNFData->pListIPDRData_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_);
   				dataValue[0] = (void *)pIPDRData->IPDRDataUnion_.IPDRLong_;
					dataType[0] = IPDRDATA_ARRAY;
   				dataType[1] = IPDRDATA_LONG;
					appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
					freeIPDRData(pIPDRData);
              }
		}
		else if (strcmp("UNSIGNED_INTEGER", tempString) == 0) {
				if(pAttributeDescriptor->noOfElements_ == -1) {
              	noOfElements = pFNFData->pListIPDRData_->noOfElements_;
				}
				else {
              	noOfElements = pFNFData->pListIPDRData_->noOfElements_;
				}
              
             if (noOfElements > 0) {
					pIPDRData = newIPDRData(IPDRDATA_ULONG);
              	pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
              	for(cnt = 0;cnt < noOfElements; cnt++) {
						pIPDRData->IPDRDataUnion_.IPDRULong_ = strtoul((char *)pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_, NULL, 10);
   					dataValue[cnt] = (void *)pIPDRData->IPDRDataUnion_.IPDRULong_; 
              		pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              	}
					dataType[0] = IPDRDATA_ARRAY;
   				dataType[1] = IPDRDATA_ULONG;
					appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
					freeIPDRData(pIPDRData);
             }
             else {
					pIPDRData = newIPDRData(IPDRDATA_ULONG);
   				pIPDRData->IPDRDataUnion_.IPDRULong_ = atoi((char *)pFNFData->pListIPDRData_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_);
   				dataValue[0] = (void *)pIPDRData->IPDRDataUnion_.IPDRULong_;
					dataType[0] = IPDRDATA_ARRAY;
   				dataType[1] = IPDRDATA_ULONG;
					appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
					freeIPDRData(pIPDRData);
              }
		}
		else if (strcmp("HYPER", tempString) == 0) {
				if(pAttributeDescriptor->noOfElements_ == -1) {
              	noOfElements = pFNFData->pListIPDRData_->noOfElements_;
				}
				else {
              	noOfElements = pFNFData->pListIPDRData_->noOfElements_;
				}
              
             if (noOfElements > 0) {
					pIPDRData = newIPDRData(IPDRDATA_LONGLONG);
              	pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
              	for(cnt = 0;cnt < noOfElements; cnt++) {
   				pIPDRData->IPDRDataUnion_.IPDRLongLong_ = (hyper)strtod((char *)pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_, &stopstring);
   					dataValue[cnt] = (void *)&(pIPDRData->IPDRDataUnion_.IPDRLongLong_); 
              		pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              	}
					dataType[0] = IPDRDATA_ARRAY;
   				dataType[1] = IPDRDATA_LONGLONG;
					appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
					freeIPDRData(pIPDRData);
             }
             else {
					pIPDRData = newIPDRData(IPDRDATA_LONGLONG);
   				pIPDRData->IPDRDataUnion_.IPDRLongLong_ = (hyper)strtod((char *)pFNFData->pListIPDRData_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_, &stopstring);
   				dataValue[0] = (void *)&(pIPDRData->IPDRDataUnion_.IPDRLongLong_); 
					dataType[0] = IPDRDATA_ARRAY;
   				dataType[1] = IPDRDATA_LONGLONG;
					appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
					freeIPDRData(pIPDRData);
              }
		}
		else if (strcmp("UNSIGNED_HYPER", tempString) == 0) {
				if(pAttributeDescriptor->noOfElements_ == -1) {
              	noOfElements = pFNFData->pListIPDRData_->noOfElements_;
				}
				else {
              	noOfElements = pFNFData->pListIPDRData_->noOfElements_;
				}
              
             if (noOfElements > 0) {
					pIPDRData = newIPDRData(IPDRDATA_ULONGLONG);
              	pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
              	for(cnt = 0;cnt < noOfElements; cnt++) {
   				pIPDRData->IPDRDataUnion_.IPDRULongLong_ = (u_hyper)strtod((char *)pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_, &stopstring);
   					dataValue[cnt] = (void *)&(pIPDRData->IPDRDataUnion_.IPDRULongLong_); 
              		pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              	}
					dataType[0] = IPDRDATA_ARRAY;
   				dataType[1] = IPDRDATA_ULONGLONG;
					appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
					freeIPDRData(pIPDRData);
             }
             else {
					pIPDRData = newIPDRData(IPDRDATA_ULONGLONG);
   				pIPDRData->IPDRDataUnion_.IPDRULongLong_ = (u_hyper)strtod((char *)pFNFData->pListIPDRData_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_, &stopstring);
   				dataValue[0] = (void *)&(pIPDRData->IPDRDataUnion_.IPDRULongLong_); 
					dataType[0] = IPDRDATA_ARRAY;
   				dataType[1] = IPDRDATA_ULONGLONG;
					appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
					freeIPDRData(pIPDRData);
              }
		}
		else if (strcmp("FLOAT", tempString) == 0) {
				if(pAttributeDescriptor->noOfElements_ == -1) {
              	noOfElements = pFNFData->pListIPDRData_->noOfElements_;
				}
				else {
              	noOfElements = pFNFData->pListIPDRData_->noOfElements_;
				}
              
             if (noOfElements > 0) {
					pIPDRData = newIPDRData(IPDRDATA_FLOAT);
              	pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
              	for(cnt = 0;cnt < noOfElements; cnt++) {
					pIPDRData->IPDRDataUnion_.IPDRFloat_ = (float)atof((char *)pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_);
					pIPDRValue = &pIPDRData->IPDRDataUnion_.IPDRFloat_;
   					dataValue[cnt] = pIPDRValue; 
              		pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              	}
					dataType[0] = IPDRDATA_ARRAY;
   				dataType[1] = IPDRDATA_FLOAT;
					appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
					freeIPDRData(pIPDRData);
             }
             else {
					pIPDRData = newIPDRData(IPDRDATA_ULONGLONG);
					pIPDRData->IPDRDataUnion_.IPDRFloat_ = (float)atof((char *)pFNFData->pListIPDRData_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_);
					pIPDRValue = &pIPDRData->IPDRDataUnion_.IPDRFloat_;
   				dataValue[0] = pIPDRValue; 

					dataType[0] = IPDRDATA_ARRAY;
   				dataType[1] = IPDRDATA_FLOAT;
					appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
					freeIPDRData(pIPDRData);
              }
		}
		else if (strcmp("DOUBLE", tempString) == 0) {
				if(pAttributeDescriptor->noOfElements_ == -1) {
              	noOfElements = pFNFData->pListIPDRData_->noOfElements_;
				}
				else {
              	noOfElements = pFNFData->pListIPDRData_->noOfElements_;
				}
              
             if (noOfElements > 0) {
					pIPDRData = newIPDRData(IPDRDATA_DOUBLE);
              	pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
              	for(cnt = 0;cnt < noOfElements; cnt++) {
   				pIPDRData->IPDRDataUnion_.IPDRDouble_ = (double)strtod((char *)pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_, &stopstring);
					pIPDRValue = &pIPDRData->IPDRDataUnion_.IPDRDouble_;
   					dataValue[cnt] = pIPDRValue; 
              		pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              	}
					dataType[0] = IPDRDATA_ARRAY;
   				dataType[1] = IPDRDATA_DOUBLE;
					appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
					freeIPDRData(pIPDRData);
             }
             else {
					pIPDRData = newIPDRData(IPDRDATA_DOUBLE);
   				pIPDRData->IPDRDataUnion_.IPDRDouble_ = (double)strtod((char *)pFNFData->pListIPDRData_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_, &stopstring);
					pIPDRValue = &pIPDRData->IPDRDataUnion_.IPDRDouble_;
   				dataValue[0] = pIPDRValue;
					dataType[0] = IPDRDATA_ARRAY;
   				dataType[1] = IPDRDATA_DOUBLE;
					appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
					freeIPDRData(pIPDRData);
              }
		}
		else if (strcmp("BYTE_ARRAY", tempString) == 0) {
				if(pAttributeDescriptor->noOfElements_ == -1) {
              	noOfElements = pFNFData->pListIPDRData_->noOfElements_;
				}
				else {
              	noOfElements = pFNFData->pListIPDRData_->noOfElements_;
				}
              
             if (noOfElements > 0) {
					pIPDRData = newIPDRData(IPDRDATA_HEXADECARRAY);
              	pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
              	for(cnt = 0;cnt < noOfElements; cnt++) {
						strcpy((char *)pIPDRData->IPDRDataUnion_.IPDRByteArray_ ,(char *)pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_);
						dataValue[cnt] = (void *) calloc (1, strlen((char *)pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_)+1);
    					strcpy((char *)dataValue[cnt], (char *)pIPDRData->IPDRDataUnion_.IPDRByteArray_);
              		pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              	}
					dataType[0] = IPDRDATA_ARRAY;
   				dataType[1] = IPDRDATA_HEXADECARRAY;
					appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
					freeIPDRData(pIPDRData);
             }
             else {
					pIPDRData = newIPDRData(IPDRDATA_HEXADECARRAY);
					strcpy((char *)pIPDRData->IPDRDataUnion_.IPDRByteArray_ ,(char *)pFNFData->pListIPDRData_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_);
					dataValue[0] = (void *) calloc (1, strlen((char *)pFNFData->pListIPDRData_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_)+1);
    				strcpy((char *)dataValue[0], (char *)pIPDRData->IPDRDataUnion_.IPDRByteArray_);
					dataType[0] = IPDRDATA_ARRAY;
   				dataType[1] = IPDRDATA_HEXADECARRAY;
					appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
					freeIPDRData(pIPDRData);
              }
		}
		else if (strcmp("UTF8_ARRAY", tempString) == 0) {
				if(pAttributeDescriptor->noOfElements_ == -1) {
              	noOfElements = pFNFData->pListIPDRData_->noOfElements_;
				}
				else {
              	noOfElements = pFNFData->pListIPDRData_->noOfElements_;
				}
              
             if (noOfElements > 0) {
					pIPDRData = newIPDRData(IPDRDATA_STRING);
              	pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
              	for(cnt = 0;cnt < noOfElements; cnt++) {
						strcpy((char *)pIPDRData->IPDRDataUnion_.IPDRUTF8Array_ ,(char *)pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_);
						dataValue[cnt] = (void *) calloc (1, strlen((char *)pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_)+1);
    					strcpy((char *)dataValue[cnt], (char *)pIPDRData->IPDRDataUnion_.IPDRUTF8Array_);
              		pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              	}
					dataType[0] = IPDRDATA_ARRAY;
   				dataType[1] = IPDRDATA_STRING;
					appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
					freeIPDRData(pIPDRData);
             }
             else {
					pIPDRData = newIPDRData(IPDRDATA_STRING);
					strcpy((char *)pIPDRData->IPDRDataUnion_.IPDRUTF8Array_ ,(char *)pFNFData->pListIPDRData_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_);
					dataValue[0] = (void *) calloc (1, strlen((char *)pFNFData->pListIPDRData_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_)+1);
    				strcpy((char *)dataValue[0], (char *)pIPDRData->IPDRDataUnion_.IPDRUTF8Array_);
					dataType[0] = IPDRDATA_ARRAY;
   				dataType[1] = IPDRDATA_STRING;
					appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
					freeIPDRData(pIPDRData);
              }
		}
	
	free(tempString);

	return (IPDR_OK);
}
