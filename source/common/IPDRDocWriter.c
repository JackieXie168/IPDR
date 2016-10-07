/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/
 
/*****************************************************
 * File                 : IPDRDocWriter              *
 * Description          :                            *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 *                                                   *
 *****************************************************/
 
#include "common/IPDRDocWriter.h"
 
/*!
 *  NAME:
 *      writeHeader() - creates Header and writes the XDR equivalent on an output 
 *                      stream. 
 *
 *  DESCRIPTION:
 *      This function will perform following functionalities:
 *       -Takes the input parameter and create a Header structure as defined in 
 *        NDM3.0.
 *   -It will call "validateHeader" function.
 *   -After that it will call "writeIPDRXDRHeaderHelper" function to convert input 
 *        structure in XDR format and write it over output stream.
 *  
 *  ARGUMENTS:
 *          -  IPDRCommonParameters (IN/OUT)
 *        -  time {IN} {This variable contains the value which is out parameter 
 *      of function getTime }
 *   -  ipdrRecorderInfo {IN}
 *   -  defaultNameSpace {IN}
 *   -  otherNameSpaces {IN}
 *   -  serviceDefs {IN}
 *   -  docID {IN}  {This variable contains the value which is out parameter 
 *             of function get_uuid }
 *   -  Version No   {IN, Global constant}
 *   -  pErrorCode {IN/OUT} 
 *
 *  RETURNS  
 *      Returns Zero or appropriate error code
*/
 
int writeHeader(
    IPDRCommonParameters* pIPDRCommonParameters, 
    hyper time, 
    char* ipdrRecorderInfo, 
    char* defaultNameSpace, 
    ListNameSpaceInfo* pOtherNameSpaces, 
    ListServiceDefinitionURI* pListServiceDefinitionURI, 
    char* docId, 
	ListSchemaNameSpace* pListSchemaNameSpace,
    int* pErrorCode
      )
{
 
 /* Populate header structure with values passed as input*/
 int errorSeverity = 0; 
 unsigned int lenDocId = 0;
 unsigned int lenIPDRRecorderInfo = 0;

 IPDRHeader *pIPDRHeader;  
 ListSchemaNameSpace* pFirstListSchemaNameSpace = NULL;
 
  /*pOtherNameSpaces == NULL ||*/ 
 if(ipdrRecorderInfo == NULL || 
  defaultNameSpace == NULL ||
     pListServiceDefinitionURI == NULL ||
  docId == NULL ||
  pIPDRCommonParameters == NULL
     ) {
     *pErrorCode = IPDR_NULL_INPUT_PARAMETER;
     return (IPDR_ERROR);
 }
 
 pIPDRHeader = newIPDRHeader();
 
 if(pIPDRHeader == NULL) {
  *pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
  return(IPDR_ERROR);
 }
 
 pIPDRHeader->startTime_ = time;

/* IPDR v3.5 changes , Check for version here*/
/* Code Change to read the pIPDRCommonParameters structure member to check the schema version */
if((strcmp(pIPDRCommonParameters->schemaVersion, IPDR_VERSION_3_1)) == 0)
{
    printf("In version is 3.1\n");
 pIPDRHeader->majorVersion_ = COMPACT_IPDR_VERSION;
 pIPDRHeader->minorVersion_ = COMPACT_IPDR_VERSION_MINOR;
	#ifdef DEBUG_IPDR
    printf("major version: %d minor version: %d, pIPDRHeader->majorVersion_,pIPDRHeader->minorVersion_\n");
	#endif
}
/* Code Change to read the pIPDRCommonParameters structure member to check the schema version */
else if((strcmp(pIPDRCommonParameters->schemaVersion, IPDR_VERSION_3_5)) == 0)
{
 	pIPDRHeader->majorVersion_ = 3;
 	pIPDRHeader->minorVersion_ = 5;
	#ifdef DEBUG_IPDR
   printf("major version: %d minor version: %d, pIPDRHeader->majorVersion_,pIPDRHeader->minorVersion_\n");
	#endif
}
else
  return(IPDR_ERROR);

 strcpy(pIPDRHeader->defaultNameSpaceURI_, defaultNameSpace);
 lenDocId = strlen(docId) + 1;
 pIPDRHeader->docID_ = (char *) calloc (lenDocId, sizeof(char));
 if(pIPDRHeader->docID_ == NULL) {
  *pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
  return(IPDR_ERROR);
 }
 
 strcpy(pIPDRHeader->docID_, docId);
 
 lenIPDRRecorderInfo = strlen(ipdrRecorderInfo) + 1;
 pIPDRHeader->ipdrRecorderInfo_ = (char *) calloc (lenIPDRRecorderInfo, sizeof(char));
 if(pIPDRHeader->ipdrRecorderInfo_ == NULL) {
  *pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
  return(IPDR_ERROR);
 }
 
 strcpy(pIPDRHeader->ipdrRecorderInfo_, ipdrRecorderInfo);
 
 pIPDRHeader->pListServiceDefinitionURI_ = pListServiceDefinitionURI;
 pIPDRHeader->pOtherNameSpaceInfo_ = pOtherNameSpaces;
 
 if(pListSchemaNameSpace != NULL) {
	 pFirstListSchemaNameSpace = pListSchemaNameSpace;
	 while(pListSchemaNameSpace != NULL) {
		appendListSchemaNameSpace(&pIPDRHeader->pListSchemaNameSpace_,
			                       pListSchemaNameSpace->pSchemaNameSpace_);
		
		pListSchemaNameSpace = pListSchemaNameSpace->pNext_;
	 }
	 pListSchemaNameSpace = pFirstListSchemaNameSpace;
 }			
 
 /* 
    Validate header. Check if other namespace items are equal to
    or less than service definition items
 */
 
	errorSeverity = validateHeader(pIPDRHeader, pErrorCode);
	if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, "validateHeader"); 
	}
	else if (errorSeverity == IPDR_ERROR) {
	pIPDRCommonParameters->DocState_ = NONWRITABLE; 
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "validateHeader"); 
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
    
 /* Write the validated header structure in the output stream.*/
 if(pIPDRCommonParameters->DocType_ == XDR) {
  errorSeverity = writeIPDRXDRHeaderHelper(pIPDRCommonParameters, pIPDRHeader, pErrorCode);
  if (errorSeverity == IPDR_WARNING) {
   errorHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXDRHeaderHelper"); 
  }
  else if (errorSeverity == IPDR_ERROR) {
   pIPDRCommonParameters->DocState_ = NONWRITABLE; 
   exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXDRHeaderHelper"); 
   return (IPDR_ERROR);
  }
 }
 else if (pIPDRCommonParameters->DocType_ == XML) {
  errorSeverity = writeIPDRXMLHeaderHelper(pIPDRCommonParameters, pIPDRHeader, pErrorCode);
  if (errorSeverity == IPDR_WARNING) {
   errorHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLHeaderHelper"); 
  }
  else if (errorSeverity == IPDR_ERROR) {
   pIPDRCommonParameters->DocState_ = NONWRITABLE; 
   exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLHeaderHelper"); 
   return (IPDR_ERROR);
  }  
 }
 else {
  *pErrorCode = IPDR_INVALID_DOCTYPE;
  return (IPDR_ERROR);
 }
 
 pIPDRCommonParameters->DocState_ = WRITABLE; 
 
 if(pIPDRHeader->pListSchemaNameSpace_ != NULL) {
	 freeListSchemaNameSpace(&pIPDRHeader->pListSchemaNameSpace_); 
 }
 freeIPDRHeader (pIPDRHeader);

 return (IPDR_OK); 
}
 
/*!
 *  NAME:
 *      createFNFData() - generates the FNFData structure.
 *
 *  DESCRIPTION:
 *      This function takes FNFType and Data Value array as input and 
 *      finally generates a composite structure FNF Data. 
 *
 *  ARGUMENTS:
 *      - IPDRCommonParameters {IN/ OUT}
 *     - FNFType  {IN/OUT}
 *      - ListIPDRData {IN/OUT}
 *  - errorCode {IN/OUT}
 *  - FNFData {IN/OUT}
 *      - pErrorCode {IN/OUT} 
 *
 *  RETURNS  
 *     Returns Zero or appropriate error code
*/
 
int createFNFData(
      IPDRCommonParameters* pIPDRCommonParameters, 
      FNFType* pFNFType, 
      ListIPDRData* pListIPDRData, 
      FNFData* pFNFData, 
      int* pErrorCode
     )
{ 
 
 int errorSeverity = 0;
 int numAttributes = 0;
 int count = 0;
 void* IPDRValue = NULL;
 ListAttributeDescriptor* pFirstListAttributeDescriptor = NULL;
 ListAttributeDescriptor* pFirstListServiceAttributeDescriptor = NULL;
 ListIPDRData* pFirstListIPDRData = NULL;
 
 if(pFNFType == NULL || 
  pListIPDRData == NULL ||
  pFNFData == NULL || 
  pIPDRCommonParameters == NULL
     ) {
     *pErrorCode = IPDR_NULL_INPUT_PARAMETER;
     return (IPDR_ERROR);
 }
 
 /* 
    This function takes attribute Descriptor array as input and perform validations   
    This function will take the complete set of attributeDescriptors and checks whether 
    that descriptor is valid or not 
 */
 
 errorSeverity = FNFTypeCheck(pFNFType, pErrorCode); 
    if (errorSeverity == IPDR_WARNING) {
  errorHandler(pIPDRCommonParameters, pErrorCode, "FNFTypeCheck"); 
 }
 else if (errorSeverity == IPDR_ERROR) {
  pIPDRCommonParameters->DocState_ = NONWRITABLE; 
  exceptionHandler(pIPDRCommonParameters, pErrorCode, "FNFTypeCheck"); 
  return (IPDR_ERROR);
 }
 
 /* this function checks for duplicate attribute names*/
 

 errorSeverity = validateAttributeName(pIPDRCommonParameters, pFNFType->pListAttributeDescriptor_, pErrorCode);
 if (errorSeverity == IPDR_WARNING) {
  errorHandler(pIPDRCommonParameters, pErrorCode, "populateAttributeDescriptorHashTable"); 
 }
 else if (errorSeverity == IPDR_ERROR) {
  pIPDRCommonParameters->DocState_ = NONWRITABLE; 
  exceptionHandler(pIPDRCommonParameters, pErrorCode, "populateAttributeDescriptorHashTable"); 
  return (IPDR_ERROR);
 }
 
   
    /* 
   This function takes input data value array as input and checks:
   Length of Descriptor and Data Array should be same. 
    */ 
 
 errorSeverity = FNFDataCheck(pListIPDRData , pFNFType, pErrorCode);
    if (errorSeverity == IPDR_WARNING) {
  errorHandler(pIPDRCommonParameters, pErrorCode, "FNFDataCheck"); 
 }
 else if (errorSeverity == IPDR_ERROR) {
  pIPDRCommonParameters->DocState_ = NONWRITABLE; 
  exceptionHandler(pIPDRCommonParameters, pErrorCode, "FNFDataCheck"); 
  return (IPDR_ERROR);
 }
   
 /* After these validations, this function populates FNFData and pass as out parameter */ 
 
 strcpy(pFNFData->typeName_, pFNFType->serviceType_);
 
 pFirstListAttributeDescriptor = pFNFType->pListAttributeDescriptor_;
 while(pFNFType->pListAttributeDescriptor_ != NULL) {
	appendListAttributeDescriptor(&pFNFData->pListAttributeDescriptor_,
		                           pFNFType->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,   
								   pFNFType->pListAttributeDescriptor_->pAttributeDescriptor_->attributeType_,
								   pFNFType->pListAttributeDescriptor_->pAttributeDescriptor_->description_ ,
								   pFNFType->pListAttributeDescriptor_->pAttributeDescriptor_->derivedType_, 
								   pFNFType->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_, 
								   pFNFType->pListAttributeDescriptor_->pAttributeDescriptor_->pListAttrNameList_, 
								   pFNFType->pListAttributeDescriptor_->pAttributeDescriptor_->pListAttrClassList_, 
								   pFNFType->pListAttributeDescriptor_->pAttributeDescriptor_->pListAttrMaxOccursList_, 
								   pFNFType->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ 
								);
	pFNFType->pListAttributeDescriptor_ = pFNFType->pListAttributeDescriptor_->pNext_; 
 }
 pFNFType->pListAttributeDescriptor_ = pFirstListAttributeDescriptor;

 pFirstListServiceAttributeDescriptor = pFNFType->pListServiceAttributeDescriptor_;
 while(pFNFType->pListServiceAttributeDescriptor_ != NULL) {
	appendListAttributeDescriptor(&pFNFData->pListServiceAttributeDescriptor_,
		                           pFNFType->pListServiceAttributeDescriptor_->pAttributeDescriptor_->attributeName_,   
								   pFNFType->pListServiceAttributeDescriptor_->pAttributeDescriptor_->attributeType_,
								   pFNFType->pListServiceAttributeDescriptor_->pAttributeDescriptor_->description_ ,
								   pFNFType->pListServiceAttributeDescriptor_->pAttributeDescriptor_->derivedType_, 
								   pFNFType->pListServiceAttributeDescriptor_->pAttributeDescriptor_->complexType_, 
								   pFNFType->pListServiceAttributeDescriptor_->pAttributeDescriptor_->pListAttrNameList_, 
								   pFNFType->pListServiceAttributeDescriptor_->pAttributeDescriptor_->pListAttrClassList_, 
								   pFNFType->pListServiceAttributeDescriptor_->pAttributeDescriptor_->pListAttrMaxOccursList_, 
								   pFNFType->pListServiceAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ 
								);
	pFNFType->pListServiceAttributeDescriptor_ = pFNFType->pListServiceAttributeDescriptor_->pNext_; 
 }
 pFNFType->pListServiceAttributeDescriptor_ = pFirstListServiceAttributeDescriptor;

 pFirstListIPDRData = pListIPDRData;

 while (pListIPDRData != NULL) {
   
   if (pListIPDRData->IPDRDataType_ == IPDRDATA_HEXADECARRAY) {
      appendListIPDRData(&pFNFData->pListIPDRData_,
        pListIPDRData->IPDRDataUnion_.IPDRByteArray_,
        IPDRDATA_HEXADECARRAY);
   }
   else if (pListIPDRData->IPDRDataType_ == IPDRDATA_DOUBLE) {
      IPDRValue = (double *) &pListIPDRData->IPDRDataUnion_.IPDRDouble_;
      appendListIPDRData(&pFNFData->pListIPDRData_,
        IPDRValue,
        IPDRDATA_DOUBLE);
   }
   else if (pListIPDRData->IPDRDataType_ == IPDRDATA_FLOAT) {
     IPDRValue = (float *) &pListIPDRData->IPDRDataUnion_.IPDRFloat_;
    appendListIPDRData(&pFNFData->pListIPDRData_,
        IPDRValue,
       IPDRDATA_FLOAT);
   }
   else if (pListIPDRData->IPDRDataType_ == IPDRDATA_LONG) {
    appendListIPDRData(&pFNFData->pListIPDRData_,
        (void *) pListIPDRData->IPDRDataUnion_.IPDRLong_,
        IPDRDATA_LONG);
   }
   else if (pListIPDRData->IPDRDataType_ == IPDRDATA_LONGLONG) {
    appendListIPDRData(&pFNFData->pListIPDRData_,
        (void *) &(pListIPDRData->IPDRDataUnion_.IPDRLongLong_),
        IPDRDATA_LONGLONG);
   }
   else if (pListIPDRData->IPDRDataType_ == IPDRDATA_ULONG) {
    appendListIPDRData(&pFNFData->pListIPDRData_,
        (void *) pListIPDRData->IPDRDataUnion_.IPDRULong_,
        IPDRDATA_ULONG);
   }
   else if (pListIPDRData->IPDRDataType_ == IPDRDATA_ULONGLONG) {
    appendListIPDRData(&pFNFData->pListIPDRData_,
        (void *) &(pListIPDRData->IPDRDataUnion_.IPDRULongLong_),
        IPDRDATA_ULONGLONG);
   }
   else if (pListIPDRData->IPDRDataType_ == IPDRDATA_STRING) {
    appendListIPDRData(&pFNFData->pListIPDRData_,
        pListIPDRData->IPDRDataUnion_.IPDRUTF8Array_,
        IPDRDATA_STRING);
   }
   else if (pListIPDRData->IPDRDataType_ == IPDRDATA_ARRAY) {
    appendListArrayToIPDRData(&pFNFData->pListIPDRData_,
        	pListIPDRData->pListIPDRArrayData_,pListIPDRData->noOfElements_,
			IPDRDATA_ARRAY);
   }
   else if (pListIPDRData->IPDRDataType_ == IPDRDATA_STRUCTURE) {
    appendListStructureToIPDRData(&pFNFData->pListIPDRData_,
			pListIPDRData->pListIPDRStructureData_,
        	IPDRDATA_STRUCTURE);
   }
   else if (pListIPDRData->IPDRDataType_ == IPDRDATA_BOOLEAN) {
    appendListIPDRData(&pFNFData->pListIPDRData_,
        pListIPDRData->IPDRDataUnion_.IPDRBoolean_,
       	IPDRDATA_BOOLEAN);
   }
   else if (pListIPDRData->IPDRDataType_ == IPDRDATA_SHORT) {
	/*
     IPDRValue = (short int *) &pListIPDRData->IPDRDataUnion_.IPDRShort_;
    appendListIPDRData(&pFNFData->pListIPDRData_,
        IPDRValue,
       IPDRDATA_SHORT);
	*/
    appendListIPDRData(&pFNFData->pListIPDRData_,
        (void *) pListIPDRData->IPDRDataUnion_.IPDRShort_,
       IPDRDATA_SHORT);
   }
   else if (pListIPDRData->IPDRDataType_ == IPDRDATA_USHORT) {
	/*
     IPDRValue = (unsigned short int *) &pListIPDRData->IPDRDataUnion_.IPDRUShort_;
    appendListIPDRData(&pFNFData->pListIPDRData_,
        IPDRValue,
       IPDRDATA_USHORT);
	*/
    appendListIPDRData(&pFNFData->pListIPDRData_,
        (void *) pListIPDRData->IPDRDataUnion_.IPDRUShort_,
       IPDRDATA_USHORT);
   }
   else if (pListIPDRData->IPDRDataType_ == IPDRDATA_BYTE) {
    appendListIPDRData(&pFNFData->pListIPDRData_,
        (void *) pListIPDRData->IPDRDataUnion_.IPDRByte_,
        IPDRDATA_BYTE);
   }
   else if (pListIPDRData->IPDRDataType_ == IPDRDATA_UBYTE) {
    appendListIPDRData(&pFNFData->pListIPDRData_,
        (void *) pListIPDRData->IPDRDataUnion_.IPDRByte_,
        IPDRDATA_UBYTE);
   }
 
   pListIPDRData = pListIPDRData->pNext_; 
  }
  
  pListIPDRData = pFirstListIPDRData;
 
 return (IPDR_OK); 
}
 
/*!
 *  NAME:
 *      writeIPDR() - converts the Raw input into well-formed XDR document.
 *
 *  DESCRIPTION:
 *      - This function takes FNFData structure as input
 *      - Calls function validateAgainstSchema
 *  - Checks for input descriptor (AttributeDescriptorHashTable) in Lookup Hash                    
 *             table(DescriptorLookupHashTable). This function will return Descriptor ID. 
 *
 *  ARGUMENTS:
 *      - IPDRCommonParameters {IN/OUT}
 *     - FNFData {IN}
 *      - pErrorCode {IN/OUT} 
 *
 *  RETURNS  
 *       Returns Zero or appropriate error code
*/
 
int writeIPDR(
     IPDRCommonParameters* pIPDRCommonParameters, 
     FNFData* pFNFData, 
     int* pErrorCode
    )
{
 int errorSeverity = 0;
 int descriptorID = 0;
 
 char* tempString;
 char* resultGet;
 unsigned int length = 0;
 void* IPDRValue = NULL;
 
 IPDRStreamElement* pIPDRStreamElement;
 ListAttributeDescriptor* pCurrentListAttributeDescriptor = NULL;
 ListIPDRData* pCurrentListIPDRData = NULL;
 
 if(pFNFData == NULL || 
  pIPDRCommonParameters == NULL
     ) {
     *pErrorCode = IPDR_NULL_INPUT_PARAMETER;
     return (IPDR_ERROR);
 }
 

 pIPDRStreamElement = newIPDRStreamElement(STREAM_IPDR_RECORD);
 
 length = getLengthSerializeFNFData(pFNFData);
    tempString = (char *) calloc(sizeof(char), length);
 
  
 if(tempString == NULL || pIPDRStreamElement == NULL) {
  *pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
  return(IPDR_ERROR);
 }
 
 if ((int)pIPDRCommonParameters->DocState_ != WRITABLE) {
  *pErrorCode = IPDR_INVALID_DOCSTATE_MAKE_WRITABLE;
  return(IPDR_ERROR);
 }
    
 
 
 		if ((pIPDRCommonParameters->pSchema_ != NULL) && (pIPDRCommonParameters->pSchema_->pListSchemaLookup_ != NULL)) {
		errorSeverity = validateAgainstSchema(pIPDRCommonParameters, pFNFData, pErrorCode);
		 if (errorSeverity == IPDR_WARNING) {
		  errorHandler(pIPDRCommonParameters, pErrorCode, "validateAgainstSchema"); 
		 }
		 else if (errorSeverity == IPDR_ERROR) {
		  pIPDRCommonParameters->DocState_ = NONWRITABLE; 
		  exceptionHandler(pIPDRCommonParameters, pErrorCode, "validateAgainstSchema"); 
		  return (IPDR_ERROR);
		 }
	 }
 
    
 if(pIPDRCommonParameters->DocType_ == XDR) {
 
  errorSeverity = serializeFNFDataStructure(pFNFData, tempString, pErrorCode);
  if (errorSeverity == IPDR_WARNING) {
   errorHandler(pIPDRCommonParameters, pErrorCode, "serializeFNFDataStructure"); 
  }
  else if (errorSeverity == IPDR_ERROR) {
   pIPDRCommonParameters->DocState_ = NONWRITABLE; 
   exceptionHandler(pIPDRCommonParameters, pErrorCode, "serializeFNFDataStructure"); 
   return (IPDR_ERROR);
  }

  resultGet = (char *) HashTableGet(pIPDRCommonParameters->pDescriptorLookupHashTable_, tempString); 
  if (resultGet !=NULL)
   descriptorID = atoi(resultGet);
  else 
   descriptorID = 0;
 
  if (descriptorID == 0) {
   errorSeverity = createDescriptor(pIPDRCommonParameters, pFNFData->pListAttributeDescriptor_, pFNFData, pErrorCode); 
   if (errorSeverity == IPDR_WARNING) {
    errorHandler(pIPDRCommonParameters, pErrorCode, "createDescriptor"); 
   }
   else if (errorSeverity == IPDR_ERROR) {
    pIPDRCommonParameters->DocState_ = NONWRITABLE; 
    exceptionHandler(pIPDRCommonParameters, pErrorCode, "createDescriptor"); 
    return (IPDR_ERROR);
   }
 
   descriptorID = pIPDRCommonParameters->descriptorCount_ - 1;
  }
 
  
  pIPDRStreamElement->descriminator_ = STREAM_IPDR_RECORD;
 
  pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->descriptorID_  = descriptorID;  
  
  
  strcpy(pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->typeName_, pFNFData->typeName_);
  
  pCurrentListAttributeDescriptor = pFNFData->pListAttributeDescriptor_;
  while (pFNFData->pListAttributeDescriptor_ != NULL) {
   appendListAttributeDescriptor(&(pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListAttributeDescriptor_),
           pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
           pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeType_,
           pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->description_,
           pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->derivedType_ ,
								   pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_, 
								   pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->pListAttrNameList_, 
								   pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->pListAttrClassList_,
								   pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->pListAttrMaxOccursList_, 
								   pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ 
           );
   pFNFData->pListAttributeDescriptor_ = pFNFData->pListAttributeDescriptor_->pNext_; 
  }
  pFNFData->pListAttributeDescriptor_ = pCurrentListAttributeDescriptor;
 
  pCurrentListAttributeDescriptor = pFNFData->pListServiceAttributeDescriptor_;
  while (pFNFData->pListServiceAttributeDescriptor_ != NULL) {
   appendListAttributeDescriptor(&(pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListServiceAttributeDescriptor_),
           pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
           pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->attributeType_,
           pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->description_,
           pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->derivedType_ ,
								   pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->complexType_, 
								   pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->pListAttrNameList_, 
								   pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->pListAttrClassList_,
								   pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->pListAttrMaxOccursList_, 
								   pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ 
           );
   pFNFData->pListServiceAttributeDescriptor_ = pFNFData->pListServiceAttributeDescriptor_->pNext_; 
  }
  pFNFData->pListServiceAttributeDescriptor_ = pCurrentListAttributeDescriptor;

  pCurrentListIPDRData = pFNFData->pListIPDRData_;
 
  while (pFNFData->pListIPDRData_ != NULL) {
	switch (pFNFData->pListIPDRData_->IPDRDataType_)
	{
	case IPDRDATA_HEXADECARRAY:
		appendListIPDRData(&(pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_),
        pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRByteArray_,
        IPDRDATA_HEXADECARRAY);
		break;
	case IPDRDATA_DOUBLE:
		  IPDRValue = (double *) &pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRDouble_;
      appendListIPDRData(&(pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_),
        IPDRValue,
        IPDRDATA_DOUBLE);
		  break;

	case IPDRDATA_FLOAT:
   IPDRValue = (float *) &pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRFloat_;
    appendListIPDRData(&(pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_),
        IPDRValue,
        IPDRDATA_FLOAT);
		break;

	case IPDRDATA_LONG:
		   appendListIPDRData(&(pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_),
        (void *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRLong_,
        IPDRDATA_LONG);
		break;

	case IPDRDATA_LONGLONG:
		appendListIPDRData(&(pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_),
        (void *) &(pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRLongLong_),
        IPDRDATA_LONGLONG);
   
		break;

	case IPDRDATA_ULONG:
		    appendListIPDRData(&(pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_),
        (void *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRULong_,
        IPDRDATA_ULONG);
		break;

	case IPDRDATA_ULONGLONG:
		appendListIPDRData(&(pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_),
        (void *) &(pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRULongLong_),
        IPDRDATA_ULONGLONG);
		break;
	case IPDRDATA_STRING:
		    appendListIPDRData(&(pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_),
        pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_,
        IPDRDATA_STRING);
		  break;
   case IPDRDATA_BOOLEAN:
    	appendListIPDRData(&(pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_),
        pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRBoolean_,
       IPDRDATA_BOOLEAN);
		break;
	case IPDRDATA_SHORT:
     	/*IPDRValue = (short int *) &pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRShort_;
    	appendListIPDRData(&(pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_),
        IPDRValue,
       IPDRDATA_SHORT);*/
    	appendListIPDRData(&(pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_),
        (void *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRShort_,
       IPDRDATA_SHORT);
		break;
   case IPDRDATA_USHORT:
     	/*IPDRValue = (unsigned short int *) &pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUShort_;
    	appendListIPDRData(&(pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_),
        IPDRValue,
       IPDRDATA_USHORT);*/
    	appendListIPDRData(&(pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_),
        (void *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUShort_,
       IPDRDATA_USHORT);
		 break;
	case IPDRDATA_ARRAY:
    		appendListArrayToIPDRData(&(pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_),
        		pFNFData->pListIPDRData_->pListIPDRArrayData_,
				pFNFData->pListIPDRData_->noOfElements_,
				IPDRDATA_ARRAY);
		break;
	case IPDRDATA_STRUCTURE:
        appendListStructureToIPDRData(&(pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_),
			pFNFData->pListIPDRData_->pListIPDRStructureData_,
        	IPDRDATA_STRUCTURE);
		break;
	case IPDRDATA_BYTE:
		   appendListIPDRData(&(pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_),
        (void *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRByte_,
        IPDRDATA_BYTE);
		break;
	case IPDRDATA_UBYTE:
		   appendListIPDRData(&(pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_),
        (void *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRByte_,
        IPDRDATA_UBYTE);
		break;

	default:
		break;


	}
   
   pFNFData->pListIPDRData_ = pFNFData->pListIPDRData_->pNext_; 
  }
  
  pFNFData->pListIPDRData_ = pCurrentListIPDRData;
 

  errorSeverity = writeIPDRStreamElementHelper(pIPDRCommonParameters, pIPDRStreamElement, pErrorCode);
  if (errorSeverity == IPDR_WARNING) {
   errorHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRStreamElementHelper"); 
  }
  else if (errorSeverity == IPDR_ERROR) {
   pIPDRCommonParameters->DocState_ = NONWRITABLE; 
   exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRStreamElementHelper"); 
   return (IPDR_ERROR);
  }
 }
 else if(pIPDRCommonParameters->DocType_ == XML) {
  errorSeverity = writeIPDRXMLRecordHelper(pIPDRCommonParameters, pFNFData, pErrorCode);
  if (errorSeverity == IPDR_WARNING) {
   errorHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLRecordHelper"); 
  }
  else if (errorSeverity == IPDR_ERROR) {
   pIPDRCommonParameters->DocState_ = NONWRITABLE; 
   exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLRecordHelper"); 
   return (IPDR_ERROR);
  }
 }
 else {
  *pErrorCode = IPDR_INVALID_DOCTYPE;
  return (IPDR_ERROR);
 }
 
 pIPDRCommonParameters->IPDRCount_.ipdrRecordCount_++;
 
 free(tempString);
 freeIPDRStreamElement (pIPDRStreamElement);
 
 return (IPDR_OK);
}
 
/*!
 *  NAME:
 *      writeDocEnd() - This function creates Document End structure and writes its 
 *                      XDR equivalent on the output stream.
 *
 *  DESCRIPTION:
 *      -This function creates the Document End structure as defined in NDM-U 3.0 and 
 *       calls function (writeDocEndHelper) for Marshalling. 
 *      -This function will call getTime function to get the endTime in 
 *   milliseconds
 *
 *  ARGUMENTS:
 *      - IPDRCommonParameters {IN/OUT}
 *      - endTime{IN}
 *      - pErrorCode {IN/OUT} 
 *
 *  RETURNS  
 *     Returns Zero or appropriate error code
*/
 

int writeDocEnd(
    IPDRCommonParameters* pIPDRCommonParameters, 
    int* pErrorCode
      )
{
 int errorSeverity = 0;
 
 hyper* endTime;
 DocEnd* pDocEnd;
 IPDRStreamElement* pIPDRStreamElement;
 
 if(pIPDRCommonParameters == NULL) {
     *pErrorCode = IPDR_NULL_INPUT_PARAMETER;
     return (IPDR_ERROR);
 }
 
 endTime = (hyper *) calloc (1, sizeof(hyper));
 pDocEnd = newDocEnd(); 
 
 if(endTime == NULL) {
 
  *pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
  return(IPDR_ERROR);
 }
 
 if ((int)pIPDRCommonParameters->DocState_ != WRITABLE) {
  *pErrorCode = IPDR_INVALID_DOCSTATE_MAKE_WRITABLE;
  return(IPDR_ERROR);
 }
 
 pDocEnd->count_ = (int)pIPDRCommonParameters->IPDRCount_.ipdrRecordCount_ - 1;
    
 errorSeverity = getTime(endTime, pErrorCode);
 if (errorSeverity == IPDR_WARNING) {
  errorHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRStreamElementHelper"); 
 }
 else if (errorSeverity == IPDR_ERROR) {
  pIPDRCommonParameters->DocState_ = NONWRITABLE; 
  exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRStreamElementHelper"); 
  return (IPDR_ERROR);
 } 
 
 pDocEnd->endTime_ = *endTime;
 
 if(pIPDRCommonParameters->DocType_ == XDR) {
  pIPDRStreamElement = newIPDRStreamElement(STREAM_DOC_END);
  pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pDocEnd_->count_ = pDocEnd->count_;
  pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pDocEnd_->endTime_ = pDocEnd->endTime_;
  
  pIPDRStreamElement->descriminator_ = 3;
  
  errorSeverity = writeIPDRStreamElementHelper(pIPDRCommonParameters, pIPDRStreamElement, pErrorCode);
  if (errorSeverity == IPDR_WARNING) {
   errorHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRStreamElementHelper"); 
  }
  else if (errorSeverity == IPDR_ERROR) {
   pIPDRCommonParameters->DocState_ = NONWRITABLE; 
   exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRStreamElementHelper"); 
   freeIPDRStreamElement(pIPDRStreamElement);
   freeDocEnd(pDocEnd);
   free (endTime);
    return (IPDR_ERROR);
  }
 freeIPDRStreamElement(pIPDRStreamElement);
 }
 else if(pIPDRCommonParameters->DocType_ == XML) {
  errorSeverity = writeIPDRXMLDocEndHelper(pIPDRCommonParameters, pDocEnd, pErrorCode); 
  if (errorSeverity == IPDR_WARNING) {
   errorHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLDocEndHelper"); 
  }
  else if (errorSeverity == IPDR_ERROR) {
   pIPDRCommonParameters->DocState_ = NONWRITABLE; 
   exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLDocEndHelper"); 
   freeDocEnd(pDocEnd);
   free (endTime); 
   return (IPDR_ERROR);
  } 
 }
 else {
  *pErrorCode = IPDR_INVALID_DOCTYPE;
  freeDocEnd(pDocEnd);
  free (endTime);
  return (IPDR_ERROR);
 }
 pIPDRCommonParameters->DocState_ = NONWRITABLE;
  
 freeDocEnd(pDocEnd);
 free (endTime);
 return (IPDR_OK);
}

int setVersion(float ver)
{
	if(ver <= 3.5)
		return 1;
	else
		return 0;
}