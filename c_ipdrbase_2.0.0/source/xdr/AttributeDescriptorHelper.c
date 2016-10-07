/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/


/*****************************************************
 * File                 : AttributeDescriptorHelper  *
 * Description          :                            *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 *                                                   *
 *****************************************************/

#include "xdr/AttributeDescriptorHelper.h"

/*!
 *  NAME:
 *      writeAttributeDescriptorHelper() - writes the XDR equivalent on 
 *										   output stream.
 *
 *  DESCRIPTION:
 *      Writes an AttributeDescriptor structure to the output stream.
 *		This function internally uses the basic write functions (eg 
 *      write_long())to actually marshall the data in the XDR format to 
 *		output stream.   
 *  
 *  ARGUMENTS:
 *       - IPDRCommonParameters (IN/OUT)	
 * 		 - AttributeDescriptor {IN}
 *		 - errorCode (IN?OUT}
 *
 *  RETURNS 	
 *	     Returns Zero or appropriate error code
 */

int writeAttributeDescriptorHelper(
							IPDRCommonParameters* pIPDRCommonParameters, 
							AttributeDescriptor* pAttributeDescriptor,
							ListAttributeDescriptor* pListServiceAttributeDescriptor, 
							int* pErrorCode
							)
{
	int typeCode = 0;
	int ltypeCode = 0;
	int attributeNameLength = 0;
   char* tempComplexType = NULL; 
   char * tempString = NULL;
	char* serviceTypeToken = NULL;
	
	if(pAttributeDescriptor == NULL ||
		pIPDRCommonParameters == NULL
	  ) {
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		return(IPDR_ERROR);
	}


if(pAttributeDescriptor->complexType_[0] != '\0') {
	tempString = (char *) calloc(strlen(pAttributeDescriptor->complexType_) + 1, sizeof(char));
	tempComplexType = (char *) calloc(strlen(pAttributeDescriptor->complexType_) + 1, sizeof(char));
	strcpy(tempString, pAttributeDescriptor->complexType_);
	serviceTypeToken = strtok(tempString, ":");
	
	if(strcmp(pAttributeDescriptor->complexType_,serviceTypeToken)) {
		getIPDRSubstring(pAttributeDescriptor->complexType_, tempComplexType, ':');
   	strcpy(pAttributeDescriptor->complexType_,tempComplexType);
	}	
	free(tempString);
	free(tempComplexType);
}

  	if(isPrimitiveComplexType(pAttributeDescriptor->complexType_) == IPDR_FALSE) {
		writeComplexAttributeDescriptorHelper(pIPDRCommonParameters, 
														  pAttributeDescriptor, 
														  pListServiceAttributeDescriptor,
														  pErrorCode);
	} else {
		attributeNameLength = strlen(pAttributeDescriptor->attributeName_);
		write_long(attributeNameLength, 
			pIPDRCommonParameters->pStreamHandle_);
		write_wstring((wchar *) pAttributeDescriptor->attributeName_, 
			pIPDRCommonParameters->pStreamHandle_, pIPDRCommonParameters->schemaVersion);

		if(pAttributeDescriptor->noOfElements_ !=0) {
			ltypeCode = convAttributeTypeToTypeCode(pAttributeDescriptor->attributeType_);
			typeCode = IPDRDATA_ARRAY | ltypeCode;
			write_long(typeCode, pIPDRCommonParameters->pStreamHandle_);

			write_long(pAttributeDescriptor->noOfElements_, pIPDRCommonParameters->pStreamHandle_);
		} else {
			typeCode = convAttributeTypeToTypeCode(pAttributeDescriptor->attributeType_);

         /* Adding code to convert typecode in case of version 3.1*/
         if((strcmp(pIPDRCommonParameters->schemaVersion, IPDR_VERSION_3_5)) != 0) {
             typeCode = typeCode & 0x0000000F;
         }
			write_long(typeCode, pIPDRCommonParameters->pStreamHandle_);
		}
	}
	
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      writeComplexAttributeDescriptorHelper() - writes the complex XDR equivalent on 
 *										   output stream.
 *
 *  DESCRIPTION:
 *      Writes an complex AttributeDescriptor structure to the output stream.
 *		This function internally uses the basic write functions (eg 
 *      write_long())to actually marshall the complex data in the XDR format to 
 *		output stream.   
 *  
 *  ARGUMENTS:
 *       - IPDRCommonParameters (IN/OUT)	
 * 		- AttributeDescriptor {IN}
			- ListAttributeDescriptor {IN}
 *		 	- errorCode (IN?OUT}
 *
 *  RETURNS 	
 *	     Returns Zero or appropriate error code
 */
int writeComplexAttributeDescriptorHelper(
							IPDRCommonParameters* pIPDRCommonParameters, 
							AttributeDescriptor* pAttributeDescriptor,
							ListAttributeDescriptor* pListServiceAttributeDescriptor,
							int* pErrorCode
							)
{
	int typeCode = 0;
	int ltypeCode = 0;
	int attributeNameLength = 0;
	int attributeDescriptorLength = 0;
	int i;
	int count;
	int countNext;
	char* tempComplexType;
   char * tempString;
	char* serviceTypeToken = NULL;

	List* pcurrentMaxOccrsList = NULL;
	List* pcurrent = NULL;
 	ListAttributeDescriptor* pOldListServiceAttributeDescriptor = NULL;
 	ListAttributeDescriptor* pCurrentListServiceAttributeDescriptor = NULL;
 	ListAttributeDescriptor* pTempOldListServiceAttributeDescriptor = NULL;
	ListAttributeDescriptor* pCurrent = NULL;
	
	if(pAttributeDescriptor == NULL ||
		pIPDRCommonParameters == NULL
	  ) {
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		return(IPDR_ERROR);
	}

	pCurrentListServiceAttributeDescriptor = pListServiceAttributeDescriptor;
   pOldListServiceAttributeDescriptor = pListServiceAttributeDescriptor;
   pTempOldListServiceAttributeDescriptor = pListServiceAttributeDescriptor;

	attributeNameLength = strlen(pAttributeDescriptor->attributeName_);
	write_long(attributeNameLength, 
			pIPDRCommonParameters->pStreamHandle_);
	write_wstring((wchar *) pAttributeDescriptor->attributeName_, 
			pIPDRCommonParameters->pStreamHandle_, pIPDRCommonParameters->schemaVersion);

	if(pAttributeDescriptor->noOfElements_ !=0) {
		ltypeCode = IPDRDATA_STRUCTURE;
		typeCode = IPDRDATA_ARRAY | ltypeCode;
		write_long(typeCode, pIPDRCommonParameters->pStreamHandle_);

		pCurrent = pListServiceAttributeDescriptor;
		attributeDescriptorLength = getLengthListAttributeDescriptor(pCurrent);
		for (count = 0; count < attributeDescriptorLength; count++) {
			if(strcmp(pAttributeDescriptor->complexType_, 
						 pCurrent->pAttributeDescriptor_->attributeName_) == 0) {
				break;
			}
			pCurrent = pCurrent->pNext_;
		}
		write_long(getLength(pCurrent->pAttributeDescriptor_->pListAttrNameList_), pIPDRCommonParameters->pStreamHandle_);

		write_long(pAttributeDescriptor->noOfElements_, pIPDRCommonParameters->pStreamHandle_);
	} else {
		typeCode = IPDRDATA_STRUCTURE;
		write_long(typeCode, pIPDRCommonParameters->pStreamHandle_);
		pCurrent = pListServiceAttributeDescriptor;
		attributeDescriptorLength = getLengthListAttributeDescriptor(pCurrent);
		for (count = 0; count < attributeDescriptorLength; count++) {
			if(strcmp(pAttributeDescriptor->complexType_, 
						 pCurrent->pAttributeDescriptor_->attributeName_) == 0) {
				break;
			}
			pCurrent = pCurrent->pNext_;
		}
		write_long(getLength(pCurrent->pAttributeDescriptor_->pListAttrNameList_), pIPDRCommonParameters->pStreamHandle_);
	}

	attributeDescriptorLength = getLengthListAttributeDescriptor(pListServiceAttributeDescriptor);
	
	for (count = 0; count < attributeDescriptorLength; count++) {
		if(strcmp(pAttributeDescriptor->complexType_, 
						 pListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_) == 0) {
				pcurrent = pListServiceAttributeDescriptor->pAttributeDescriptor_->pListAttrNameList_;
			if(pListServiceAttributeDescriptor->pAttributeDescriptor_->pListAttrMaxOccursList_ != NULL) {
					pcurrentMaxOccrsList = pListServiceAttributeDescriptor->pAttributeDescriptor_->pListAttrMaxOccursList_;
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
						if(isPrimitiveComplexType(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_) == IPDR_FALSE) {
								writeComplexAttributeDescriptorHelper(pIPDRCommonParameters, 
																			 pOldListServiceAttributeDescriptor->pAttributeDescriptor_,
																			 pTempOldListServiceAttributeDescriptor, 
																			 pErrorCode);
				} else {
						attributeNameLength = strlen(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_);
						write_long(attributeNameLength, 
										pIPDRCommonParameters->pStreamHandle_);
						write_wstring((wchar *) pOldListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_, 
											pIPDRCommonParameters->pStreamHandle_, pIPDRCommonParameters->schemaVersion);


              		if((pcurrentMaxOccrsList != NULL)) {
							if((strcmp(pcurrentMaxOccrsList->node, "0") != 0)) {
								ltypeCode = convAttributeTypeToTypeCode(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->attributeType_);  
								typeCode = IPDRDATA_ARRAY | ltypeCode;
								write_long(typeCode, pIPDRCommonParameters->pStreamHandle_);

								write_long(atoi(pcurrentMaxOccrsList->node), pIPDRCommonParameters->pStreamHandle_);
							} else {
								typeCode = convAttributeTypeToTypeCode(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->attributeType_);
								write_long(typeCode, pIPDRCommonParameters->pStreamHandle_);
							}
              		}
					}
				}
				pOldListServiceAttributeDescriptor = pOldListServiceAttributeDescriptor->pNext_;
			}
			pcurrent = pcurrent->pNext;
			if(pcurrentMaxOccrsList != NULL)
				pcurrentMaxOccrsList = pcurrentMaxOccrsList->pNext;
		  }
		}		
		pListServiceAttributeDescriptor = pListServiceAttributeDescriptor->pNext_;
	}

   pListServiceAttributeDescriptor = pCurrentListServiceAttributeDescriptor;
	
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      readAttributeDescriptorHelper() - called by the 
 *										  readRecordDescriptorHelper function
 *
 *  DESCRIPTION:
 *     	This function returns the Attribute descriptor structure.
 *		This function:
 *	     - Reads AttributeName
 *	     - Reads TypeCode3
 *		 - Populates the AttributeDescriptor structure with the above read 
 *         values.
 *  
 *  ARGUMENTS:
 *       - IPDRCommonParameters (IN/OUT)
 *		 - AttributeDescriptor (IN/OUT)
 *		 - errorCode (IN?OUT}
 *
 *  RETURNS 	
 *	     Returns Zero or appropriate error code
 */

int readAttributeDescriptorHelper( 	IPDRCommonParameters *pIPDRCommonParameters, 
												AttributeDescriptor *pAttributeDescriptor, 
												ListAttributeDescriptor**	pListAttributeDescriptor, 
												ListAttributeDescriptor**	pListServiceAttributeDescriptor,
												int*  attributeType,
												int* pErrorCode
											)
{
	int attributeNameLength = 0;
	int attributeTypeCode = 0;
	int errorSeverity = 0;
	int maxLen = 0;
	int arrayFlag = 0;
	char typeCode[50];
   wchar* tmpString = NULL;


	if(pAttributeDescriptor == NULL ||
		pIPDRCommonParameters == NULL
	  ) {
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		return(IPDR_ERROR);
	}

	attributeNameLength = read_long(pIPDRCommonParameters->pStreamHandle_);

	tmpString = read_wstring(attributeNameLength, 
		pIPDRCommonParameters->pStreamHandle_, pIPDRCommonParameters->schemaVersion);

	strcpy(pAttributeDescriptor->attributeName_, (char *) tmpString);
	free(tmpString);

	attributeTypeCode = read_long(pIPDRCommonParameters->pStreamHandle_);

	sprintf(typeCode, "%0x", attributeTypeCode);
	if((strlen(typeCode) > 4) && (typeCode[0] == '4') || (typeCode[0] == 'c')) {
		attributeTypeCode = attributeTypeCode & 0xBFFFFFFF;
		arrayFlag = 1;
	}
   /* Adding code to convert typeCode if version = 3.1 */
   if((strcmp(pIPDRCommonParameters->schemaVersion, IPDR_VERSION_3_5)) != 0) {
       attributeTypeCode = attributeTypeCode | 0x00000020;
   }

	*attributeType = attributeTypeCode;
	
	if(attributeTypeCode == IPDRDATA_STRUCTURE) {
		readCompositeDescriptor(pAttributeDescriptor->attributeName_,
										arrayFlag, 
										pListAttributeDescriptor, 
										pListServiceAttributeDescriptor,
									   pIPDRCommonParameters,
										pErrorCode
									  );
		return(IPDR_OK);
	} else {
		if(arrayFlag == 1) {
			maxLen = read_long(pIPDRCommonParameters->pStreamHandle_);
			pAttributeDescriptor->noOfElements_ = maxLen;
		}
	}

	errorSeverity = convTypeCodeToAttributeType(attributeTypeCode, pAttributeDescriptor->attributeType_);
	if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, "convTypeCodeToAttributeType");	
	}
	else if (errorSeverity == IPDR_ERROR) {
		pIPDRCommonParameters->DocState_ = NONWRITABLE; 
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "convTypeCodeToAttributeType");	
		return (IPDR_ERROR);
	}			
	strcpy(pAttributeDescriptor->description_,IPDR_NULL);
	strcpy(pAttributeDescriptor->derivedType_, pAttributeDescriptor->attributeType_);
	strcpy(pAttributeDescriptor->complexType_, "string");

	
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      readCompositeDescriptor() - called by the 
 *										  readRecordDescriptorHelper function
 *
 *  DESCRIPTION:
 *     	This function returns the complex Attribute descriptor structure.
 *		This function:
 *	     - Reads AttributeName
 *	     - Reads TypeCode3
 *		 - Populates the AttributeDescriptor structure with the above read 
 *         values.
 *  
 *  ARGUMENTS:
 *       - IPDRCommonParameters (IN/OUT)
 *		 - AttributeDescriptor (IN/OUT)
 *		 - errorCode (IN?OUT}
 *
 *  RETURNS 	
 *	     Returns Zero or appropriate error code
 */

int readCompositeDescriptor(char* attrName,
									 int arrayflag, 
	  								 ListAttributeDescriptor** pListAttributeDescriptor, 
									 ListAttributeDescriptor** pListServiceAttributeDescriptor,
									 IPDRCommonParameters* pIPDRCommonParameters,
									 int* pErrorCode
									)
{
	int compLength;
	int childArrayFlag = 0;
	int childMaxLen = 0;
	int maxLen = 0;
	int i;
	int attributeNameLength = 0;
	int attributeTypeCode = 0;
	int errorSeverity = 0;
	static int AttributeFlag = 0;
	char complexType[MAX_LEN];
	char typeCode[MAX_LEN];
	char maxLenTemp[MAX_LEN];
   wchar* tmpString = NULL;
	AttributeDescriptor* pAttributeDescriptor = NULL;

	pAttributeDescriptor = newAttributeDescriptor();

	compLength = read_long(pIPDRCommonParameters->pStreamHandle_);

   if(arrayflag == 1) 
		maxLen = read_long(pIPDRCommonParameters->pStreamHandle_);

	strcpy(complexType, attrName);
	strcat(complexType,"Type");

if(AttributeFlag == 0) {
	appendListAttributeDescriptor(&(*pListAttributeDescriptor),
		attrName,
   	"UTF8_ARRAY",
   	"OPTIONAL",
   	"UTF8_ARRAY",
		complexType,
		NULL,
      NULL,
		NULL,
      maxLen	
   );
	AttributeFlag = 1;
} else {
	appendListAttributeDescriptor(&(*pListServiceAttributeDescriptor),
		attrName,
   	"UTF8_ARRAY",
   	"OPTIONAL",
   	"UTF8_ARRAY",
		complexType,
		NULL,
      NULL,
		NULL,
      maxLen	
   );
	AttributeFlag = 0;
}

	for(i = 0; i < compLength; i++) {
		childMaxLen = 0;
		childArrayFlag = 0;
   	attributeNameLength = read_long(pIPDRCommonParameters->pStreamHandle_);
   
   	tmpString = read_wstring(attributeNameLength, 
   		pIPDRCommonParameters->pStreamHandle_, pIPDRCommonParameters->schemaVersion);
   
   	strcpy(pAttributeDescriptor->attributeName_, (char *) tmpString);
   	free(tmpString);
   
   	attributeTypeCode = read_long(pIPDRCommonParameters->pStreamHandle_);
   
   	sprintf(typeCode, "%0x", attributeTypeCode);
   	if((strlen(typeCode) > 4) && (typeCode[0] == '4') || (typeCode[0] == 'c')) {
   		attributeTypeCode = attributeTypeCode & 0xBFFFFFFF;
   		childArrayFlag = 1;
   	}
   	
   	if(attributeTypeCode == IPDRDATA_STRUCTURE) {
			AttributeFlag = 1;
   		readCompositeDescriptor(pAttributeDescriptor->attributeName_,
											childArrayFlag, 
   										&(*pListAttributeDescriptor), 
   										&(*pListServiceAttributeDescriptor),
											pIPDRCommonParameters,
											pErrorCode
   									  );
   	} else {
   		if(childArrayFlag == 1) {
   			childMaxLen = read_long(pIPDRCommonParameters->pStreamHandle_);
   			pAttributeDescriptor->noOfElements_ = childMaxLen;
   		}
			errorSeverity = convTypeCodeToAttributeType(attributeTypeCode, pAttributeDescriptor->attributeType_);
			if (errorSeverity == IPDR_WARNING) {
				errorHandler(pIPDRCommonParameters, pErrorCode, "readCompositeDescriptor");	
			}
			else if (errorSeverity == IPDR_ERROR) {
				pIPDRCommonParameters->DocState_ = NONWRITABLE; 
				exceptionHandler(pIPDRCommonParameters, pErrorCode, "readCompositeDescriptor");	
				return (IPDR_ERROR);
			}			
			appendListAttributeDescriptor(&(*pListServiceAttributeDescriptor),
													pAttributeDescriptor->attributeName_,
   												pAttributeDescriptor->attributeType_,
   												"OPTIONAL",
   												pAttributeDescriptor->attributeType_,
													"string",
													NULL,
      											NULL,
													NULL,
      											0	
   											  );
			AttributeFlag = 0;
   	}
		appendNode (&(pAttributeDescriptor->pListAttrNameList_),
							 pAttributeDescriptor->attributeName_);
		appendNode(&(pAttributeDescriptor->pListAttrClassList_), "MANDATORY");
		sprintf(maxLenTemp,"%d", childMaxLen);
		appendNode(&(pAttributeDescriptor->pListAttrMaxOccursList_), maxLenTemp);
	}
	appendListAttributeDescriptor(&(*pListServiceAttributeDescriptor),
											complexType,
   										"UTF8_ARRAY",
   										"OPTIONAL",
   										"UTF8_ARRAY",
   										"STRUCTURE",
											pAttributeDescriptor->pListAttrNameList_,
											pAttributeDescriptor->pListAttrClassList_,
      									pAttributeDescriptor->pListAttrMaxOccursList_,
      									0	
   									  );

	freeAttributeDescriptor (pAttributeDescriptor);

	return(IPDR_OK);
	
}
