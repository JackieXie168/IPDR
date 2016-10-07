/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/

/******************************************************
 * File                 : descriptor.h                *
 * Description          :			      * 
 * Author               : Infosys Tech Ltd            *
 * Modification History :                             *
 *--------------------------------------------------- *
 * Date       Name        Change/Description          *
 *--------------------------------------------------- *
 * 26Jan02    Infosys       Draft Version             *
 ******************************************************/

#ifndef _DESCRIPTOR_H
#define _DESCRIPTOR_H

#include "utils/IPDRCommon.h"
#include "utils/utils.h"
#include "utils/dynamicArray.h"


#define IPDR_SCHEMA                    "schema"
#define IPDR_COMPLEX_TYPE              "complexType"
#define IPDR_COMPLEXCONTENT            "complexContent"
#define IPDR_ELEMENT                   "element"
#define IPDR_NAME                      "name"
#define IPDR_TYPE                      "type"
#define IPDR_MINOCCURS                 "minOccurs"
#define IPDR_MAXOCCURS                 "maxOccurs"
#define IPDR_REF                       "ref"
#define IPDR_FINAL_SCHEMA					"ipdr:IPDRType"
#define IPDR_EXTENSION						"extension"
#define IPDR_INCLUDE							"include"
#define IPDR_IMPORT							"import"
#define IPDR_SCHEMA_LOCATION				"schemaLocation"
#define IPDR_BASE								"base"
#define IPDR_RESTRICTION					"restriction"

/* 8 primitive AttributeTypes to READ from Schema */
#define IPDR_LONG                   "int" 
#define IPDR_LONG_OTHER					"integer"
#define IPDR_U_LONG                 "unsignedInt"
#define IPDR_HYPER                  "long"
#define IPDR_U_HYPER                "unsignedLong"
#define IPDR_FLOAT                  "float"
#define IPDR_DOUBLE                 "double"
#define IPDR_STRING                 "string"
#define IPDR_BYTE_ARRAY					"byte array"
#define IPDR_HEX_BIN_ARRAY				"hexbinary"
#define IPDR_BOOLEAN						"boolean"
#define IPDR_SHORT						"short"
#define IPDR_U_SHORT						"unsignedShort"
#define IPDR_BYTE							"byte"
#define IPDR_U_BYTE						"unsignedByte"

/* 8 primitive AttributeTypes to be written in descriptor */
#define IPDR_DESCRIPTOR_LONG           "INTEGER" 
#define IPDR_DESCRIPTOR_U_LONG         "UNSIGNED_INTEGER"
#define IPDR_DESCRIPTOR_HYPER          "HYPER"
#define IPDR_DESCRIPTOR_U_HYPER        "UNSIGNED_HYPER"
#define IPDR_DESCRIPTOR_FLOAT          "FLOAT"
#define IPDR_DESCRIPTOR_DOUBLE         "DOUBLE"
#define IPDR_DESCRIPTOR_STRING         "UTF8_ARRAY"
#define IPDR_DESCRIPTOR_BYTE_ARRAY	   "BYTE_ARRAY"
#define IPDR_DESCRIPTOR_HEX_BIN_ARRAY	"HEX_BIN_ARRAY"
#define IPDR_DESCRIPTOR_BOOLEAN	    	"BOOLEAN"
#define IPDR_DESCRIPTOR_SHORT	    		"SHORT"
#define IPDR_DESCRIPTOR_U_SHORT	    	"UNSIGNED_SHORT"
#define IPDR_DESCRIPTOR_BYTE	    		"BYTE"
#define IPDR_DESCRIPTOR_U_BYTE	    	"UNSIGNED_BYTE"


/* Upper Derived Type */
#define IPDR_DERIVED_U_DATETIME         	"DATETIME"	 
#define IPDR_DERIVED_U_DATETIME_MILISEC	"DATETIMEMSEC"   
#define IPDR_DERIVED_U_IPV4ADDR				"IPV4ADDR"  
#define IPDR_DERIVED_U_IPV6ADDR				"IPV6ADDR"  
#define IPDR_DERIVED_U_UUID					"UUID"  

/* Attribute Class */
#define IPDR_OPTIONAL                  "Optional"
#define IPDR_MANDATORY                 "Mandatory"
#define IPDR_TARGET_NAMESPACE				"targetNamespace"

int Depth;

typedef struct DescriptorCommon {
    FNFType *pFNFType_;
    List *pAttrNameList_;
    List *pAttrTypeList_;
    List *pAttrClassList_;
    List *pAttrClassNameList_;
    List *pAttrMaxOccursList_;
    List *pComplexAttrTypeList_;
    List *pSchemaElementAttrClassList_;
    List *pSchemaElementAttrNameList_;
    List *pSchemaElementAttrMaxOcursList_;
    List *pTempListAttrNameList_;
    List *pTempListAttrClassList_;
    List *pTempListAttrMaxOccursList_;
    char *attrNameNode_;
    char *attrTypeNode_;
    char *attrClassNode_;
	int nameTypeFlag_;
	char* targetNameSpace_;
	char* nameSpaceId_;
	char* extensionBase_;
	char* schemaLocation_;
	int* pErrorCode_;
   int recordFlag_;
   int complexTypeFlag_;
	int serviceFlag_;
	ListNameSpaceInfo* pListNameSpaceInfo_;
} DescriptorCommon;

static void start(void *data, const char *el, const char **attr);

static void end(void *userData, const char *name);

int generateDescriptor(char* SchemaFile, 
					   FNFType* pFNFType,
					   char** OptionalAttributes,
					   int* pErrorCode);

int generateSchemaDescriptor(char* SchemaURI, 
        FNFType* pFNFType,
		ListNameSpaceInfo** pListNameSpaceInfo,
		int *pErrorCode);

int getPrimitiveType(char* attributeType, 
					 char* attributeDerivedType);

int isPrimitive(char* tempNewAttrType);

int populateAttributeType(char* attributeType);

int freeDescriptorCommon(DescriptorCommon* pDescriptorCommon);

int initDescriptorCommon(DescriptorCommon* pDescriptorCommon,
						 int* pErrorCode);

int parseSchema(
		char* SchemaURI, 
      	DescriptorCommon* pDescriptorCommon,
		int *pErrorCode);

int getFileName(char* URI, char* name);

int getSchemaDescriptor(char** OptionalAttributes, 
				  ListAttributeDescriptor* pListAttributeDescriptor,
				  FNFType* pFNFType);

int getSchemaDescriptoriForVersion(char** OptionalAttributes, 
				  ListAttributeDescriptor* pListAttributeDescriptor,
				  FNFType* pFNFType);

int	populateListNameSpaceInfo(
		ListNameSpaceInfo* tempListNameSpaceInfo, 
		ListNameSpaceInfo* pListNameSpaceInfo);
#endif
