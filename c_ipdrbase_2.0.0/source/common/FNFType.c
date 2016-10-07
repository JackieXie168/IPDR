/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/


/*****************************************************
 * File                 : FNFType                    *
 * Description          :                            *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 *                                                   *
 *****************************************************/

#include "common/FNFType.h"


/*!
 *  NAME:
 *       FNFTypeCheck() - This function does validations on Input Descriptor. 
 *
 *  DESCRIPTION:
 *     	This function checks whether Type of input Attribute Descriptor	belongs 
 *      to basic primitive data types.	
 *
 *  ARGUMENTS:
 *       -	FNFType {IN/OUT}
 *		 -	errorCode {IN/OUT}
 *
 *  RETURNS 	
 *	     Returns Zero or appropriate error code
 */
 
 
int FNFTypeCheck(
				 FNFType* pFNFType, 
				 int* pErrorCode
				)
{	
	int count = 0;
	int errorSeverity = 0;
	int listAttributeDescriptorLength = 0;
	
	char* tempString;
	
	ListAttributeDescriptor* pCurrent = NULL;
	
	if(pFNFType == NULL) {
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		return(IPDR_ERROR);
	}

	pCurrent = pFNFType->pListAttributeDescriptor_;

	listAttributeDescriptorLength = getLengthListAttributeDescriptor(pCurrent);
	for(count = 0; count < listAttributeDescriptorLength; count++ )	{	
			
		tempString = (char *) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char));
	
		if(tempString == NULL ) {
			*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
			return(IPDR_ERROR);
		}

		errorSeverity = convertToUpper((char *)pCurrent->pAttributeDescriptor_->attributeType_, tempString);
		if (errorSeverity != IPDR_OK) {
			return (IPDR_ERROR);
		}
		if (strcmp("INTEGER", tempString) == 0);
		else if (strcmp("UNSIGNED_INTEGER", tempString) == 0);
		else if (strcmp("HYPER", tempString) == 0);
		else if (strcmp("UNSIGNED_HYPER", tempString) == 0);
		else if (strcmp("FLOAT", tempString) == 0);
		else if (strcmp("DOUBLE", tempString) == 0);
		else if (strcmp("BYTE_ARRAY", tempString) == 0);
		else if (strcmp("UTF8_ARRAY", tempString) == 0);
		else if (strcmp("BOOLEAN", tempString) == 0);
		else if (strcmp("SHORT", tempString) == 0);
		else if (strcmp("UNSIGNED_SHORT", tempString) == 0);
		else if (strcmp("BYTE", tempString) == 0);
		else if (strcmp("UNSIGNED_BYTE", tempString) == 0);
		else { 
			*pErrorCode = IPDR_NON_PRIMITIVE_DATA_TYPE;
			free(tempString);
			return(IPDR_WARNING);
		}
		pCurrent = pCurrent->pNext_; 
		free(tempString);
	}	
	
	freeListAttributeDescriptor(&pCurrent);
	return (IPDR_OK);	
}
