/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/


/**********************************************************************
* File                 : schemaUtil.c                                 *
* Description          : The Schema file for validation               * 
* Author               : Infosys Tech Ltd.                            *   
* Modification History :                                              * 
* ----------------------------------------------------------------    *
*    Date      Name     Change/Description                            * 
*                                                                     * 
*                                                                     *
* ----------------------------------------------------------------    * 
**********************************************************************/

#include "utils/schemaUtil.h"
#include "utils/dynamicArray.h"


/*!
*NAME:
*     validateAgainstSchema() 
*
*DESCRIPTION:
*     This method is used to validate a given FNFData object against a 
*      schema object.
* 
*ARGUMENTS:
* - Poiter to IPDRCommomnParameters{IN}
* - Pointer to FNFData {IN}
*      - Integer variable to store the error code if it originates
*        in the function {OUT}
*
* RETURNS 
* Returns Zero or appropriate error code
*/


int validateAgainstSchema(
	   IPDRCommonParameters *pIPDRCommonParameters,
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
	char* tempAttributeString;
	char* pFNFAttributeName;
	int lenDerivedType = 0;
	int* attSequence = NULL;
	int attSeqFNFData = 0;
	

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
	errorSeverity = getMandatoryAttributesFromSchema(
									pIPDRCommonParameters, 
									&pListAttrName, 
									pFNFtypeName, 
									pErrorCode);
	if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, "getMandatoryAttributesFromSchema");	
	}
	else if (errorSeverity == IPDR_ERROR) {
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "getMandatoryAttributesFromSchema");	
		free(pFNFtypeName);
		free(nameSpaceFnfID);
		free(nameSpaceFnfURI);
		return (IPDR_ERROR);
	}			

	errorSeverity = checkForMandatoryAttrInFNFData(
								   pListAttrName,
								   pFNFData->pListAttributeDescriptor_,
								   pErrorCode);
	if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, "checkForMandatoryAttrInFNFData");	
	}
	else if (errorSeverity == IPDR_ERROR) {
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "checkForMandatoryAttrInFNFData");	
		free(pFNFtypeName);
		free(nameSpaceFnfID);
		free(nameSpaceFnfURI);	
		return (IPDR_ERROR);
	}

	
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
		
		FNFAttributeName = (char *) calloc (strlen(pFNFtypeName) +
										   strlen(nameSpaceFnfURI) +
										   strlen (pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_) + 25, 
										   sizeof(char));
		
		
		tempAttributeString = (char *) calloc (strlen(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_) + 1, sizeof(char));
		pFNFAttributeName = (char *) calloc (strlen(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_) + 1, sizeof(char));

		strcpy(tempAttributeString, pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);
		serviceTypeToken = strtok(tempAttributeString, ":");
		
		if(!strcmp(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,serviceTypeToken)) 
			strcpy(pFNFAttributeName, pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);
		else {
			strcpy(pFNFAttributeName, pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);
			/*
			getIPDRSubstring(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_
								, pFNFAttributeName, ':');
			*/
		}
		
		free(tempAttributeString);

		sprintf(FNFAttributeName, "%s%s%s%s%s", 
								   pFNFtypeName, "$", 
								   nameSpaceFnfURI, "$", 
								   pFNFAttributeName);								   
		
		free(pFNFAttributeName);

		pSchemaElement = newSchemaElement();
		
		attSequence = (int *) calloc (1, sizeof(int));

		errorSeverity = getAttributeNameSeq(pIPDRCommonParameters,
										FNFAttributeName,
										attSequence,
										pErrorCode);
		if (*attSequence == 0) {
			*pErrorCode = IPDR_NO_SUCH_ATTRIBUTE_IN_SCHEMA;
			return(IPDR_WARNING);
		} 
		else {
			if (*attSequence <= attSeqFNFData) {
			*pErrorCode = IPDR_INVALID_ORDER;
			return(IPDR_WARNING);
			}
		}
		

		attSeqFNFData = *attSequence;


		free(attSequence);

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
		
		free(FNFAttributeName);
		
		
		if (strcmp(pSchemaElement->attributeType_, pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeType_) != 0) {
			*pErrorCode = IPDR_DATATYPE_MISMATCH_IN_SCHEMA_AND_FNFDATA;
			free(pFNFtypeName);
			free(nameSpaceFnfID);
			free(nameSpaceFnfURI);				
			return(IPDR_WARNING);
		}
		strcpy(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->derivedType_, pSchemaElement->derivedType_);    
		freeSchemaElement(pSchemaElement); 

			
		

		pFNFData->pListAttributeDescriptor_ = pFNFData->pListAttributeDescriptor_->pNext_; 
	}
	pFNFData->pListAttributeDescriptor_ = pCurrentListAttributeDescriptor;
	
	free(pFNFtypeName);
	free(nameSpaceFnfID);
	free(nameSpaceFnfURI);

	if (pListAttrName != NULL)
		freeList(&pListAttrName);


	return (IPDR_OK);
}


int checkForMandatoryAttrInFNFData(List* pListAttrName, ListAttributeDescriptor* pListAttributeDescriptor, int* pErrorCode)
{
	int flag = 0;
	char* attributeName = NULL;
	List* pCurrentListAttrName = NULL;
	ListAttributeDescriptor* pCurrentListAttributeDescriptor = NULL;
	
	if(pListAttrName == NULL || pListAttributeDescriptor == NULL) {
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		return(IPDR_ERROR);
	}
	attributeName = (char *) calloc(MAX_IPDR_STRING, sizeof(char));
	
	pCurrentListAttrName = pListAttrName;
	while(pListAttrName != NULL) {
		flag = 0;
	
		
		pCurrentListAttributeDescriptor = pListAttributeDescriptor;
		while ((pListAttributeDescriptor != NULL) && (flag == 0)) {
			
			getSchemaAttributeName(attributeName, pListAttrName->node,pErrorCode);

			if(strcmp(pListAttributeDescriptor->pAttributeDescriptor_->attributeName_, attributeName) == 0) {
				flag =1;
			} else {
				pListAttributeDescriptor = pListAttributeDescriptor->pNext_; 	
			}
		}

		pListAttributeDescriptor = pCurrentListAttributeDescriptor;
		if(flag != 1) {
			*pErrorCode = IPDR_MANDATORY_ATTR_NOT_FOUND;
			return(IPDR_WARNING);
		}
		
		pListAttrName = pListAttrName->pNext; 
	}
	pListAttrName = pCurrentListAttrName;
	
	free(attributeName);
	return(IPDR_OK);
}

int	getMandatoryAttributesFromSchema(
									IPDRCommonParameters* pIPDRCommonParameters, 
									List** pListAttrName, 
									char* serviceType, 
									int* pErrorCode)
{
	ListSchemaLookup* pCurrentListSchemaLookup = NULL; 
	ListSchemaElement*  pCurrentListSchemaElement = NULL;
	
	if(pIPDRCommonParameters == NULL || 
		serviceType == NULL) {
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		return(IPDR_ERROR);
	}

	pCurrentListSchemaLookup = pIPDRCommonParameters->pSchema_->pListSchemaLookup_;
	while(pIPDRCommonParameters->pSchema_->pListSchemaLookup_ != NULL ) {
		if(strcmp(serviceType, pIPDRCommonParameters->pSchema_->pListSchemaLookup_->pSchemaLookup_->serviceType_) != 0) {
			pIPDRCommonParameters->pSchema_->pListSchemaLookup_ = pIPDRCommonParameters->pSchema_->pListSchemaLookup_->pNext_; 			
		} else {
			pCurrentListSchemaElement = pIPDRCommonParameters->pSchema_->pListSchemaLookup_->pSchemaLookup_->pListSchemaElement_;
			while(pIPDRCommonParameters->pSchema_->pListSchemaLookup_->pSchemaLookup_->pListSchemaElement_ != NULL) {
				if(pIPDRCommonParameters->pSchema_->pListSchemaLookup_->pSchemaLookup_->pListSchemaElement_->pSchemaElement_->pAttributeOptional_ == 1) {
					appendNode(pListAttrName, pIPDRCommonParameters->pSchema_->pListSchemaLookup_->pSchemaLookup_->pListSchemaElement_->pSchemaElement_->attributeName_); 
				}
				pIPDRCommonParameters->pSchema_->pListSchemaLookup_->pSchemaLookup_->pListSchemaElement_ = 
					pIPDRCommonParameters->pSchema_->pListSchemaLookup_->pSchemaLookup_->pListSchemaElement_->pNext_; 
			}
			pIPDRCommonParameters->pSchema_->pListSchemaLookup_->pSchemaLookup_->pListSchemaElement_ = pCurrentListSchemaElement;
			break;
		}

	}

	pIPDRCommonParameters->pSchema_->pListSchemaLookup_ = pCurrentListSchemaLookup;

	return(IPDR_OK);
}


int getAttributeNameSeq(
			IPDRCommonParameters* pIPDRCommonParameters,
			char* attributeName,
			int* attSequence,
			int* pErrorCode)
{
	int flag = 0;
	int sequenceNo = 0;
	ListSchemaLookup* pCurrentListSchemaLookup = NULL; 
	ListSchemaElement*  pCurrentListSchemaElement = NULL;
	char* serviceType;
	
	if(pIPDRCommonParameters == NULL || attributeName == NULL) {
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		return(IPDR_ERROR);
	}

	/* Getting service Type */
	serviceType = (char *) calloc(strlen(attributeName), sizeof(char));
	getServiceType(serviceType, attributeName, pErrorCode);

	pCurrentListSchemaLookup = pIPDRCommonParameters->pSchema_->pListSchemaLookup_;

	while(pIPDRCommonParameters->pSchema_->pListSchemaLookup_ != NULL && flag == 0) {
		if(strcmp(serviceType, pIPDRCommonParameters->pSchema_->pListSchemaLookup_->pSchemaLookup_->serviceType_) != 0) {
			pIPDRCommonParameters->pSchema_->pListSchemaLookup_ = pIPDRCommonParameters->pSchema_->pListSchemaLookup_->pNext_; 			
		} else {
			
			pCurrentListSchemaElement = pIPDRCommonParameters->pSchema_->pListSchemaLookup_->pSchemaLookup_->pListSchemaElement_;
			while(pIPDRCommonParameters->pSchema_->pListSchemaLookup_->pSchemaLookup_->pListSchemaElement_ != NULL && flag == 0) {
				/*
				printf("\nAttribute in Schema = %s", pIPDRCommonParameters->pSchema_->pListSchemaLookup_->pSchemaLookup_->pListSchemaElement_->pSchemaElement_->attributeName_);
				*/
				if(strcmp(attributeName, pIPDRCommonParameters->pSchema_->pListSchemaLookup_->pSchemaLookup_->pListSchemaElement_->pSchemaElement_->attributeName_) != 0) {
					pIPDRCommonParameters->pSchema_->pListSchemaLookup_->pSchemaLookup_->pListSchemaElement_ = 
						pIPDRCommonParameters->pSchema_->pListSchemaLookup_->pSchemaLookup_->pListSchemaElement_->pNext_; 
					sequenceNo++;
				} else {
					*attSequence = sequenceNo + 1;
					flag = 1;
					break;
				}				
			}
			pIPDRCommonParameters->pSchema_->pListSchemaLookup_->pSchemaLookup_->pListSchemaElement_ = pCurrentListSchemaElement;
	
			if (flag == 0) {
			*attSequence = 0;
			break;
			}
		}
	}
	pIPDRCommonParameters->pSchema_->pListSchemaLookup_ = pCurrentListSchemaLookup;
	
	free(serviceType); 
	return(IPDR_OK);
}


int getSchemElementFromAttributeName(IPDRCommonParameters* pIPDRCommonParameters, 
									 SchemaElement* pSchemaElement, 
									 char* attributeName, 
									 int* pErrorCode)
{
	int flag = 0;
	ListSchemaLookup* pCurrentListSchemaLookup = NULL; 
	ListSchemaElement*  pCurrentListSchemaElement = NULL;
	char* serviceType;
	
	if(pIPDRCommonParameters == NULL || pSchemaElement == NULL ||
		attributeName == NULL) {
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		return(IPDR_ERROR);
	}

	/* Getting service Type */
	serviceType = (char *) calloc(strlen(attributeName), sizeof(char));
	getServiceType(serviceType, attributeName, pErrorCode);

	pCurrentListSchemaLookup = pIPDRCommonParameters->pSchema_->pListSchemaLookup_;

	while(pIPDRCommonParameters->pSchema_->pListSchemaLookup_ != NULL && flag == 0) {
		if(strcmp(serviceType, pIPDRCommonParameters->pSchema_->pListSchemaLookup_->pSchemaLookup_->serviceType_) != 0) {
			pIPDRCommonParameters->pSchema_->pListSchemaLookup_ = pIPDRCommonParameters->pSchema_->pListSchemaLookup_->pNext_; 			
		} else {
			
			pCurrentListSchemaElement = pIPDRCommonParameters->pSchema_->pListSchemaLookup_->pSchemaLookup_->pListSchemaElement_;
			while(pIPDRCommonParameters->pSchema_->pListSchemaLookup_->pSchemaLookup_->pListSchemaElement_ != NULL && flag == 0) {
				
				if(strcmp(attributeName, pIPDRCommonParameters->pSchema_->pListSchemaLookup_->pSchemaLookup_->pListSchemaElement_->pSchemaElement_->attributeName_) != 0) {
					pIPDRCommonParameters->pSchema_->pListSchemaLookup_->pSchemaLookup_->pListSchemaElement_ = 
						pIPDRCommonParameters->pSchema_->pListSchemaLookup_->pSchemaLookup_->pListSchemaElement_->pNext_; 
				} else {
					initSchemaElement(
						  pSchemaElement,
						  pIPDRCommonParameters->pSchema_->pListSchemaLookup_->pSchemaLookup_->pListSchemaElement_->pSchemaElement_->attributeName_,
						  pIPDRCommonParameters->pSchema_->pListSchemaLookup_->pSchemaLookup_->pListSchemaElement_->pSchemaElement_->attributeType_,
						  pIPDRCommonParameters->pSchema_->pListSchemaLookup_->pSchemaLookup_->pListSchemaElement_->pSchemaElement_->documentation_,
						  pIPDRCommonParameters->pSchema_->pListSchemaLookup_->pSchemaLookup_->pListSchemaElement_->pSchemaElement_->derivedType_,
						  pIPDRCommonParameters->pSchema_->pListSchemaLookup_->pSchemaLookup_->pListSchemaElement_->pSchemaElement_->unit_,
						  pIPDRCommonParameters->pSchema_->pListSchemaLookup_->pSchemaLookup_->pListSchemaElement_->pSchemaElement_->reference_,
						  pIPDRCommonParameters->pSchema_->pListSchemaLookup_->pSchemaLookup_->pListSchemaElement_->pSchemaElement_->status_,
						  pIPDRCommonParameters->pSchema_->pListSchemaLookup_->pSchemaLookup_->pListSchemaElement_->pSchemaElement_->pListAttributeEnum_,
						  pIPDRCommonParameters->pSchema_->pListSchemaLookup_->pSchemaLookup_->pListSchemaElement_->pSchemaElement_->pAttributeOptional_
						);
					flag = 1;
					break;
				}				
			}
			pIPDRCommonParameters->pSchema_->pListSchemaLookup_->pSchemaLookup_->pListSchemaElement_ = pCurrentListSchemaElement;
			if (flag == 0) {
			break;
			}
	
		}
	}
	pIPDRCommonParameters->pSchema_->pListSchemaLookup_ = pCurrentListSchemaLookup;
	

	free(serviceType); 
	return(IPDR_OK);
}



int getServiceType(char* serviceType, char* attributeName, int* pErrorCode)
{
	char seps[] = "$";
	char* token;
	char* tmpString;
	int tokenCount = 0;
	unsigned int strLength = 0;

	if(serviceType == NULL || attributeName == NULL ) {
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		return(IPDR_ERROR);
	}	

	strLength = strlen(attributeName) + 1;
	tmpString = (char *) calloc (strLength, sizeof(char));
	strcpy(tmpString, attributeName);

	token = strtok( tmpString, seps );

	while( token != NULL )
	{
	  if (tokenCount == 0) strcpy(serviceType, token);
	  token = strtok( NULL, seps );
	  tokenCount++;
	}

	free(tmpString);
	free(token);

   return (IPDR_OK);
}

int getSchemaAttributeName(char* attributeName, char* key, int* pErrorCode)
{
	char seps[] = "$";
	char* token;
	char* tmpString;
	int tokenCount = 0;
	unsigned int strLength = 0;

	if(attributeName == NULL || key == NULL ) {
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		return(IPDR_ERROR);
	}	

	strLength = strlen(key) + 1;
	tmpString = (char *) calloc (strLength, sizeof(char));
	strcpy(tmpString, key);

	token = strtok( tmpString, seps );

	while( token != NULL )
	{
	  if (tokenCount == 2) strcpy(attributeName, token);
	  token = strtok( NULL, seps );
	  tokenCount++;
	}

	free(tmpString);
	free(token);

   return (IPDR_OK);
}

int getSchemaAttributeType(IPDRCommonParameters* pIPDRCommonParameters, 
							char* attributeName,
							char* schemaAttributeType,
							int* pErrorCode)
{
	int errorSeverity = 0;
	SchemaElement* pSchemaElement;
	pSchemaElement = newSchemaElement();
	
	errorSeverity = getSchemElementFromAttributeName(
									pIPDRCommonParameters,
									pSchemaElement, 
									attributeName, 
									pErrorCode);
	if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, "getSchemElementFromAttributeName");	
	}
	else if (errorSeverity == IPDR_ERROR) {
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "getSchemElementFromAttributeName");	
		freeSchemaElement(pSchemaElement); 
		return (IPDR_ERROR);
	}
	
	strcpy(schemaAttributeType, pSchemaElement->attributeType_);
	
	return (IPDR_OK);
}


int getSchemaDerivedType(IPDRCommonParameters* pIPDRCommonParameters, 
							char* attributeName,
							char* schemaDerivedType,
							int* pErrorCode)
{
	int errorSeverity = 0;
	SchemaElement* pSchemaElement;
	pSchemaElement = newSchemaElement();
	
	errorSeverity = getSchemElementFromAttributeName(
									pIPDRCommonParameters,
									pSchemaElement, 
									attributeName, 
									pErrorCode);
	if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, "getSchemElementFromAttributeName");	
	}
	else if (errorSeverity == IPDR_ERROR) {
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "getSchemElementFromAttributeName");	
		freeSchemaElement(pSchemaElement); 
		return (IPDR_ERROR);
	}
	
	strcpy(schemaDerivedType, pSchemaElement->derivedType_);
	
	freeSchemaElement(pSchemaElement);
	
	return (IPDR_OK);
}

int getSchemaDocumentation(IPDRCommonParameters* pIPDRCommonParameters, 
							char* attributeName,
							char* schemaDocumentation,
							int* pErrorCode)
{
	int errorSeverity = 0;
	SchemaElement* pSchemaElement;
	pSchemaElement = newSchemaElement();
	
	errorSeverity = getSchemElementFromAttributeName(
									pIPDRCommonParameters,
									pSchemaElement, 
									attributeName, 
									pErrorCode);
	if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, "getSchemElementFromAttributeName");	
	}
	else if (errorSeverity == IPDR_ERROR) {
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "getSchemElementFromAttributeName");	
		freeSchemaElement(pSchemaElement); 
		return (IPDR_ERROR);
	}
	
	strcpy(schemaDocumentation, pSchemaElement->documentation_);
	
	freeSchemaElement(pSchemaElement);
	
	return (IPDR_OK);
}





