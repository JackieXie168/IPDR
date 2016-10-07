/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/



/********************************************************************
* File                 : Schema.h                                   * 
* Description          : This file defines validateAgainstSchema    *  
* Author               : Infosys Tech Ltd.                          *
* Modification History :                                            *
* ----------------------------------------------------------------  *
*    Date      Name     Change/Description                          * 
*                                                                   *
*                                                                   *
* ----------------------------------------------------------------  *
********************************************************************/

#ifndef _SCHEMAUTIL_H
#define _SCHEMAUTIL_H

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils/IPDRCommon.h"
#include "utils/IPDRMemory.h"
#include "utils/utils.h"
#include "utils/errorHandler.h"
#include "utils/serialize.h"
#include "utils/errorCode.h"
#include "utils/dynamicArray.h"



int validateAgainstSchema(
           IPDRCommonParameters *pIPDRCommonParameters,
           FNFData* pFNFData,
           int* pErrorCode
          );

int getSchemElementFromAttributeName(IPDRCommonParameters* pIPDRCommonParameters, 
									 SchemaElement* pSchemaElement, 
									 char* attributeName, 
									 int* pErrorCode);

int	getMandatoryAttributesFromSchema(
									IPDRCommonParameters* pIPDRCommonParameters, 
									List** pListAttrName, 
									char* serviceType, 
									int* pErrorCode);

int checkForMandatoryAttrInFNFData(
								   List* pListAttrName, 
								   ListAttributeDescriptor* pListAttributeDescriptor, 
								   int* pErrorCode);

int getServiceType(
				   char* serviceType, 
				   char* attributeName, 
				   int* pErrorCode
				   );

int getSchemaAttributeName(char* attributeName, char* key, int* pErrorCode);

int getAttributeNameSeq(
			IPDRCommonParameters* pIPDRCommonParameters,
			char* attributeName,
			int* attSequence,
			int* pErrorCode);

int	getMandatoryAttributesFromSchemaForStructures(
									IPDRCommonParameters* pIPDRCommonParameters,
									ListSchemaElement* pListSchemaElement, 
									List** pListAttrName, 
									char* serviceType, 
									int* pErrorCode);

int checkForMandatoryAttrInFNFDataForStructures(
											List* pListAttrName, 
											ListAttributeDescriptor* pListServiceAttributeDescriptor, 
											int* pErrorCode);

int	validateArraySizeForAttributesFromSchemaForStructures(
									IPDRCommonParameters* pIPDRCommonParameters,
									ListSchemaElement* pListSchemaElement, 
									ListAttributeDescriptor* pListServiceAttributeDescriptor, 
									char* serviceType, 
									int* pErrorCode);

#endif
