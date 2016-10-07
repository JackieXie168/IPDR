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
* Description          : This file defines all the variables and    *  
*                         function in the schema.c                  *
* Author               : Infosys Tech Ltd.                          *
* Modification History :                                            *
* ----------------------------------------------------------------  *
*    Date      Name     Change/Description                          * 
*                                                                   *
*                                                                   *
* ----------------------------------------------------------------  *
********************************************************************/

#ifndef _SCHEMA_H
#define _SCHEMA_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <malloc.h>
#include "curl/curl.h"
#include "curl/types.h"
#include "curl/easy.h"
#include "utils/hashtable.h"
#include "utils/IPDRCommon.h"
#include "utils/errorCode.h"
#include "common/descriptor.h"
#include "utils/IPDRMemory.h"
#include "utils/serialize.h"
#include "common/getFile.h"
#include "expat/expat.h"
#include "utils/dynamicArray.h"
#include "utils/utils.h"


#define DEBUG_SCHEMA_LOOKUP		0
#define LOOKUP_FINAL_SCHEMA		"ipdr:IPDRType"


#define SCHEMA_LOOKUP_DERIVED_DATETIME                         "dateTime"
#define SCHEMA_LOOKUP_DERIVED_DATETIME_MILISEC                 "dateTimeMsec"
#define SCHEMA_LOOKUP_DERIVED_IPV4ADDR                         "ipV4Addr"
#define SCHEMA_LOOKUP_DERIVED_IPV6ADDR                         "ipV6Addr"
#define SCHEMA_LOOKUP_DERIVED_UUID                             "UUID"
#define SCHEMA_LOOKUP_DERIVED_IPADDR                           "ipAddr"
#define SCHEMA_LOOKUP_DERIVED_DATETIME_MICROSEC                   "dateTimeUsec"

#define SCHEMA_LOOKUP_DERIVED_STRUCTURE                          "structure"


#define SCHEMA_LOOKUP_LONG                   "int" 
#define SCHEMA_LOOKUP_LONG_OTHER					"integer"
#define SCHEMA_LOOKUP_U_LONG                 "unsignedInt"
#define SCHEMA_LOOKUP_HYPER                  "long"
#define SCHEMA_LOOKUP_U_HYPER                "unsignedLong"
#define SCHEMA_LOOKUP_FLOAT                  "float"
#define SCHEMA_LOOKUP_DOUBLE                 "double"
#define SCHEMA_LOOKUP_STRING                 "string"
#define SCHEMA_LOOKUP_BYTE_ARRAY					"byte array"
#define SCHEMA_LOOKUP_HEX_BIN_ARRAY				"hexbinary"
#define SCHEMA_LOOKUP_BOOLEAN						"boolean"
#define SCHEMA_LOOKUP_SHORT						"short"
#define SCHEMA_LOOKUP_U_SHORT						"unsignedShort"
#define SCHEMA_LOOKUP_BYTE							"byte"
#define SCHEMA_LOOKUP_U_BYTE						"unsignedByte"

//IPDR v3.5
#define SCHEMA_LOOKUP_ARRAY            	 	"array"
#define SCHEMA_LOOKUP_STRUCTURE              "structure"

/* 8 primitive AttributeTypes to be written in descriptor */
#define SCHEMA_LOOKUP_WRITE_LONG            	"INTEGER" 
#define SCHEMA_LOOKUP_WRITE_U_LONG          	"UNSIGNED_INTEGER"
#define SCHEMA_LOOKUP_WRITE_HYPER           	"HYPER"
#define SCHEMA_LOOKUP_WRITE_U_HYPER         	"UNSIGNED_HYPER"
#define SCHEMA_LOOKUP_WRITE_FLOAT           	"FLOAT"
#define SCHEMA_LOOKUP_WRITE_DOUBLE          	"DOUBLE"
#define SCHEMA_LOOKUP_WRITE_STRING          	"UTF8_ARRAY"
#define SCHEMA_LOOKUP_WRITE_BYTE_ARRAY	    	"BYTE_ARRAY"
#define SCHEMA_LOOKUP_WRITE_HEX_BIN_ARRAY	   "HEX_BIN_ARRAY"
#define SCHEMA_LOOKUP_WRITE_BOOLEAN	    		"BOOLEAN"
#define SCHEMA_LOOKUP_WRITE_SHORT	    		"SHORT"
#define SCHEMA_LOOKUP_WRITE_U_SHORT	    		"UNSIGNED_SHORT"
#define SCHEMA_LOOKUP_WRITE_BYTE	    			"BYTE"
#define SCHEMA_LOOKUP_WRITE_U_BYTE	    		"UNSIGNED_BYTE"

//IPDR v3.5
#define SCHEMA_LOOKUP_WRITE_ARRAY            "ARRAY"
#define SCHEMA_LOOKUP_WRITE_STRUCTURE        "STRUCTURE"



typedef struct SchemaCommon{
	int recordFlag_;
	int recordComplexFlag_;
	int complexTypeFlag_;
	int serviceFlag_;
	int docFlag_;
	int unitFlag_;
	int statusFlag_;
	int referenceFlag_;
	ListNameSpaceInfo* pListNameSpaceInfo_;
	char* targetNameSpace_;
	char* nameSpaceId_;
	char* schemaLocation_;
	char* extensionBase_;
	char* oldDoc_;
	SchemaLookup* pSchemaLookup_;
	SchemaElement* pSchemaElement_;
	List* lstReference_;
	List* lstMinOccurs_;
	List* lstMaxOccurs_;

	List* lstSchemaElementReference_;
	List* lstSchemaElementMinOccurs_;
	List* lstSchemaElementMaxOccurs_;
	
} SchemaCommon;

SchemaCommon* newSchemaCommon(void);

int getStringNoTabs(char* dest, char* src);

int parseSchemaLookup(char* SchemaURI, 
				SchemaCommon* pSchemaCommon,
                                /* New parameter added to pass Schema version */
                                IPDRCommonParameters* pIPDRCommonParameters,
				int *pErrorCode);

int generateSchemaLookup(
		char* SchemaURI,
		SchemaLookup* pSchemaLookup,
		ListNameSpaceInfo** pListNameSpaceInfo,
                /* New parameter added to pass Schema version */
                IPDRCommonParameters* pIPDRCommonParameters,
		int* pErrorCode);

int populateAttributeTypeSL(char* attributeType);

int getPrimitiveTypeSL(char* attributeType, 
      char* attributeDerivedType);

int isPrimitiveSL(char *tempNewAttrType);

int getFileNameSL(char* URI, char* name);

int getXMLCharSL(char* tmpXMLChar);

int setSchemaData(
				  IPDRCommonParameters *pIPDRCommonParameters,
				  ListNameSpaceInfo* schemaNameSpaces,
				  ListServiceDefinitionURI* ServiceDefintionURI, 
				  char* defaultNameSpace,int* pErrorCode);


int freeSchemaCommon(SchemaCommon* pSchemaCommon);

int setSchemaVersion(float ver);

#endif
