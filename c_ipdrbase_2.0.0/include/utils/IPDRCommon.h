/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/



/******************************************************
 * File                 : IPDRCommon.h                *
 * Description          : Contains All Data Structures*
 * Author               : Infosys Tech Ltd            *
 * Modification History :                             *
 *----------------------------------------------------*
 * Date       Name        Change/Description          *
 *----------------------------------------------------*
 * 01/11/02   Infosys      Created                    *
 *****************************************************/

#ifndef _IPDRCOMMON_H
#define _IPDRCOMMON_H

#include <stdio.h>
#include <stdlib.h>
#include "utils/hashtable.h"
#include "utils/dynamicArray.h"
/*! 
 * Defining Constants for Version major
 */
#define COMPACT_IPDR_VERSION 		3

/*! 
 * Defining Constants for Version minor
 */
#define COMPACT_IPDR_VERSION_MINOR 		1


/*! 
 * Defining Constants for Indefinite Length Indicator
 */

#define IPDR_INDEFINITE_LENGTH_INDICATOR 		-1 

/*! 
 * Defining Constants for Debugging in IPDR C Libraries
 */

#define DEBUG_ME			 		1

#define IPDR_VERSION_3_5      "3.5"
#define IPDR_VERSION_3_1		"3.1"
#define IPDR_VERSION_3_0		"3.0"

/*!
 * Defining Constants for size of string variables
 */

#define MAX_ATTRIBUTE_NAME		      1000	
#define MAX_ATTRIBUTE_TYPE				100
#define MAX_ATTRIBUTE_CLASS			100
#define MAX_ERROR_MESSAGE				100
#define MAX_NAMESPACE_URI				100
#define MAX_SERVICE_DEFINITION_URI	100
#define MAX_ATTRIBUTE_LENGTH        1000
#define MAX_IPDR_STRING					1000
#define MAX_LEN							1000
#define HASH_BUCKETS						10
#define MAX_SCHEMA_VERSION				10

/*!
 * Defining Constants for 8 primitive data types
 */
/*
#define IPDRDATA_LONG				1
#define IPDRDATA_ULONG				2
#define IPDRDATA_LONGLONG			3
#define IPDRDATA_ULONGLONG			4
#define IPDRDATA_FLOAT				5
#define IPDRDATA_DOUBLE				6
#define IPDRDATA_BYTEARRAY			7
#define IPDRDATA_UTF8ARRAY			8
#define IPDRDATA_ARRAY				9
#define IPDRDATA_STRUCTURE			10
*/

#define IPDRDATA_LONG					0x00000021
#define IPDRDATA_ULONG					0x00000022
#define IPDRDATA_LONGLONG				0x00000023
#define IPDRDATA_ULONGLONG				0x00000024
#define IPDRDATA_FLOAT					0x00000025
#define IPDRDATA_DOUBLE					0x00000026
#define IPDRDATA_HEXADECARRAY			0x00000027
#define IPDRDATA_STRING					0x00000028
#define IPDRDATA_BOOLEAN				0x00000029
#define IPDRDATA_BYTE					0x0000002a
#define IPDRDATA_UBYTE					0x0000002b
#define IPDRDATA_SHORT					0x0000002c
#define IPDRDATA_USHORT					0x0000002d
#define IPDRDATA_ARRAY					0x40000000
#define IPDRDATA_STRUCTURE				0x80000000

/*!
 * Defining Constants for different types of Stream ELements
 */

#define STREAM_RECORD_DESCRIPTOR	1
#define STREAM_IPDR_RECORD			2
#define STREAM_DOC_END				3

/*!
 * Defining Constants for IPDR Doc types
 */

#define IPDR_XDR					"XDR"
#define IPDR_XML					"XML"

/*!
 * Defining Constants for handling NULL while displaying
 */

#define IPDR_NULL					"<null>"

#define TRUE 	1
#define FALSE 	0

/* Derived Type */
#define IPDR_DERIVED_DATETIME                           	"dateTime"
#define IPDR_DERIVED_DATETIME_MILISEC                   	"dateTimeMsec"
#define IPDR_DERIVED_IPV4ADDR                           	"ipV4Addr"
#define IPDR_DERIVED_IPV6ADDR                           	"ipV6Addr"
#define IPDR_DERIVED_UUID                               	"UUID"
#define IPDR_DERIVED_IPADDR                           	"ipAddr"
#define IPDR_DERIVED_DATETIME_MICROSEC                   "dateTimeUsec"



/*!
 * Defining Constants for Utils functionality
 */

#define DASH 						"-"
#define HYPHEN 					-
#define ZERO 						"0"
#define T 							"T"	
#define COLON 						":"
#define DOT 						"."
#define Z 							"Z"

/*! 
 * Definition for Hyper data type 
 */

#ifdef WIN32
        #define hyper __int64
        #define u_hyper unsigned __int64
#else
        #define hyper long long
        #define u_hyper unsigned long long
#endif
 
/*! 
 * Definition for Byte data type 
 */

typedef unsigned char byte;

typedef unsigned char bool;

/*! 
 * Definition for wide character data type 
 */

typedef unsigned short wchar;

/*!
 * Defining Enumerated datatype for DocType
 */

typedef enum DocType
{
	XDR = 0,
	XML = 1
} DocType;

/*!
 * Defining Enumerated datatype for DocState
 */

typedef enum DocState
{
	WRITABLE = 0,
	NONWRITABLE = 1,
	READABLE = 2,
	NONREADABLE = 3
} DocState;


/*! 
 * Defining Structure of Attribute Descriptor
 */

typedef struct AttributeDescriptor
{
 char* attributeName_;
 char* attributeType_;
 char* description_;
 char* derivedType_;

 char* complexType_;
 int   noOfElements_;
 List* pListAttrNameList_;
 List* pListAttrClassList_;
 List* pListAttrMaxOccursList_;

} AttributeDescriptor;


/*! 
 * Linked List for List of Attribute Descriptor structures.
 */

typedef struct ListAttributeDescriptor
{
 
 AttributeDescriptor* pAttributeDescriptor_;
 struct ListAttributeDescriptor* pNext_;
} ListAttributeDescriptor;


/*!
 *  Structure for Descriptor Look Up hash table. 
 *  This hash table will be having list of all 
 *  the descriptor created by API.
 *  Key = Attribute Descriptor Hash Table
 *  Value = Descriptor ID.
 */

typedef struct DescriptorLookupHashTable
{
	ListAttributeDescriptor* pKeyListAttributeDescriptorHashTable_; 
	int ValueDescriptorID_;                
} DescriptorLookupHashTable;


/*!
 * Defining Structure of NameSpaceInfo
 */

typedef struct NameSpaceInfo
{
	char* nameSpaceURI_;
	char* nameSpaceID_;
}NameSpaceInfo;


/*!
 * Linked list for listNameSpaceInfo. 
 */

typedef struct ListNameSpaceInfo{
	NameSpaceInfo NameSpaceInfo_;
	struct ListNameSpaceInfo* pNext_;
} ListNameSpaceInfo;

/*!
 * Defining Structure of SchemaLookupValue
 */

typedef struct SchemaLookupValue
{
	int Occurence_;
	char* format_;

} SchemaLookupValue;

/*!
 * Defining Structure of SchemaLookupKey
 */

typedef struct SchemaLookupKey
{
	AttributeDescriptor* pAttributeDescriptor_;
	NameSpaceInfo* pNameSpaceInfo_;
	char* serviceDefinitionURI_;

} SchemaLookupKey;

/*!
 * Defining Structure of SchemaLookupHashTable
 */

typedef struct SchemaLookupHashTable
{
	SchemaLookupValue* pSchemaLookupValue_;
	SchemaLookupKey* pSchemaLookupKey_;

} SchemaLookupHashTable;

/*!
 * Defining Structure of URI
 */

typedef struct URI
{
	char* protocol_;
	int   port_;
	char* host_;
	char* file_;
} URI;

/*!
 * Defining SchemaValidationFlag
 * Used to specify the URI to the function containing 
 * the Schema file. 
 * e.g. Protocol: http, Port: -1, Host: www.starwave.com, File: /people/naughtonComplete.
 * Uri will be represented as:
 * http://www.starwave.com/people/naughton/
 */

typedef enum SchemaValidationFlag
{            
	IPDR_FALSE = 0,            
	IPDR_TRUE = 1
} SchemaValidationFlag;


/*!
 * Defining IPDRCount
 * This is used in ErrorStructure for
 * populating ipdrRecordCount(In case of Write) &
 * for offsetValue (In case of Read)
 */

typedef union IPDRCount
{
	int ipdrRecordCount_;
	hyper offsetValue_;
} IPDRCount;


/*!
 * Structure for Error. 
 * It will come as return type whenever an error occurs.
 */

typedef struct ErrorStructure{
	int errorCode_;
	hyper offsetValue_;
	char* functionName_;
	char* errorMessage_;
	int recordCount_;
	int errorSeverity_;
} ErrorStructure;


/*!
 * Linked list for listErrorStructure. 
 */

typedef struct ListErrorStructure{
	ErrorStructure ErrStruct_;
	struct ListErrorStructure* pNext_;
} ListErrorStructure;




/*!
 * Linked list for ListServiceDefinitionURI
 */

typedef struct ListServiceDefinitionURI {
	char* pServiceDefintionURI_;
	struct ListServiceDefinitionURI* pNext_;
} ListServiceDefinitionURI;

/*!
 * Linked list for ListSchemaNameSpace
 */

typedef struct ListSchemaNameSpace {
	char* pSchemaNameSpace_;
	struct ListSchemaNameSpace* pNext_;
} ListSchemaNameSpace;


/*!
 * Defining Structure of Header
 */
typedef struct IPDRHeader
{
	int majorVersion_;
	int minorVersion_;
	char* ipdrRecorderInfo_;
	hyper startTime_;
	char* defaultNameSpaceURI_;
	ListNameSpaceInfo* pOtherNameSpaceInfo_;
	ListServiceDefinitionURI* pListServiceDefinitionURI_;
	char* docID_;
	ListSchemaNameSpace* pListSchemaNameSpace_;
} IPDRHeader;


/*! 
 * Defining Structure of RecordDescriptor
 */
typedef struct RecordDescriptor
{
	int descriptorID_;
	char* typeName_;
	ListAttributeDescriptor* pListAttributeDescriptor_;
	ListAttributeDescriptor* pListServiceAttributeDescriptor_;
} RecordDescriptor;


typedef struct ListRecordDescriptor
{
	RecordDescriptor* pRecordDescriptor_;
	struct ListRecordDescriptor* pNext_;
} ListRecordDescriptor;


/*!
 * Union for IPDRData. 
 * It is used to map the input Data Values with the C datatypes.
 */
typedef union IPDRDataUnion
{
	int IPDRLong_;
	unsigned int IPDRULong_;
	hyper IPDRLongLong_;
	u_hyper IPDRULongLong_;
	float IPDRFloat_;
	double IPDRDouble_;
	byte* IPDRByteArray_;
	wchar* IPDRUTF8Array_;
	bool* IPDRBoolean_;
	int IPDRByte_;
   unsigned int IPDRUShort_;
   int IPDRShort_;
} IPDRDataUnion;


/*!
 * Structure for IPDRData. 
 * It is used to map the input Data Values with the C datatypes.
 */

typedef struct IPDRData
{
	IPDRDataUnion IPDRDataUnion_;
	int IPDRDataFlag_;
} IPDRData;

/*!
 * Structure for Array IPDRData. 
 * It is used to map the input Array Data Values with the C datatypes.
 */
typedef struct ListIPDRArrayData
{
	int IPDRDataType_;
	IPDRDataUnion IPDRDataUnion_;

   struct ListIPDRArrayData *pNext_;
} ListIPDRArrayData; 


typedef struct ListIPDRComplex
{
	int IPDRDataType_;
   int noOfElements_;	

	ListIPDRArrayData* pListIPDRArrayData_;

   struct ListIPDRComplex *pNext_;
} ListIPDRComplex;

typedef struct ListIPDRComplexData
{
	int IPDRDataType_;
	int noOfelements_;
   ListIPDRComplex* pListIPDRComplex_;
	struct ListIPDRComplexData *pNext_;
}ListIPDRComplexData;

/*!
 * Structure for structure IPDRData. 
 * It is used to map the input structure Data Values with the C datatypes.
 */
typedef struct ListIPDRStructureData
{
	int noOfElements_;
   ListIPDRComplexData* pListIPDRComplexData_;

	struct ListIPDRStructureData* pNext_;
} ListIPDRStructureData;
	

/*!
 * Linked List for IPDRData. 
 */

typedef struct ListIPDRData {
	int IPDRDataType_;
	IPDRDataUnion IPDRDataUnion_;

	/*IPDR v3.5, structures and Arrays*/
   int noOfElements_;	
	ListIPDRArrayData *pListIPDRArrayData_; /*Array*/
	ListIPDRStructureData *pListIPDRStructureData_; /*Structures*/

	struct ListIPDRData* pNext_;
} ListIPDRData;

/*!
 * Strcuture for FNFData. 
 * It is used to associate the both Data 
 * and Descriptor with FNFData
 * TypeName: This contains the Service Type depending 
 * upon which the associated schema will be picked.
 */

typedef struct FNFData
{             
	char* typeName_;
	ListAttributeDescriptor* pListAttributeDescriptor_;
   /*IPDR v3.5*/
	ListAttributeDescriptor* pListServiceAttributeDescriptor_;
	ListIPDRData* pListIPDRData_;
} FNFData;

/*!
 * Strcuture for XMLFNFData. 
 * It is used to associate the both Data 
 * and Descriptor with XMLFNFData
 * TypeName: This contains the Service Type depending 
 * upon which the associated schema will be picked.
 */

typedef struct XMLFNFData
{             
	char* typeName_;
	int XMLFlag_;
	ListAttributeDescriptor* pListAttributeDescriptor_;
   /*IPDR v3.5*/
	ListAttributeDescriptor* pListServiceAttributeDescriptor_;
	ListIPDRData* pListIPDRData_;
} XMLFNFData;

/*!
 * Linked List for XMLFNFData. 
 */

typedef struct ListFNFData {
	XMLFNFData* pXMLFNFData_;
	struct ListFNFData* pNext_;
} ListFNFData;

/*!
 * Structure for IPDRRecord as defined in NDM-U 3.0 
 * except Length * (API will assign Indefinite Length Indicator = -1 at run time). 
 * API will assign Union Discriminator  = 2 at run time.
 */

typedef struct IPDRRecord
{
	int descriptorID_;
	FNFData* pFNFData_;
} IPDRRecord;

/*!
 * Linked List for IPDRRecord
 */

typedef struct ListIPDRRecord
{
	IPDRRecord* pIPDRRecord_;
	struct ListIPDRRecord* pNext_;
} ListIPDRRecord;



/*! 
 * Defining Structure of DocEnd
 */	

typedef struct DocEnd
{
	int count_;
	hyper endTime_;
} DocEnd;

/*!
 * Defining union of streamElement
 */	

typedef union StreamElementUnion
{
	RecordDescriptor* pRecordDescriptor_;
	IPDRRecord* pIPDRRecord_;
	DocEnd* pDocEnd_;
} StreamElementUnion;

/*!
 * Defining Structure of streamElement
 */

typedef struct StreamElement
{
	StreamElementUnion StreamElementUnion_;
	int StreamElementFlag_;
} StreamElement;


/*!
 * Defining Structure of IPDRStreamElement
 */	

typedef struct IPDRStreamElement
{
	int descriminator_;
	StreamElement* pStreamElement_;
} IPDRStreamElement;


/*!
 * Structure for IPDRDoc as defined in NDM-U 3.0.
 */

typedef struct IPDRDoc
{            
	IPDRHeader* pIPDRHeader_;   
	ListRecordDescriptor* pListRecordDescriptor_;
	ListIPDRRecord* pListIPDRRecord_;
	DocEnd* pDocEnd_;	
} IPDRDoc;

/*!
 * Structure for FNFType as defined in NDM-U 3.0.
 */

typedef struct FNFType
{
    char* serviceType_;
    ListAttributeDescriptor* pListAttributeDescriptor_;
    /*IPDR v3.5*/
    ListAttributeDescriptor* pListServiceAttributeDescriptor_;
} FNFType;

/*!
 * Defining Structure of List of Keys
 */
typedef struct ListOfKeys {
	char* key_;
	struct ListOfKeys* pNext_;
} ListOfKeys;


/*!
 * Defining Structure of Schema
 * SchemaLookupHashTable, can store the hashTable 
 * which is generated from the Schema.
 */


/*!
 * Defining enumerations for Attribute for its occurences
 * can be either mandatory or optional.
 */

typedef enum AttributeOptional
{
	IPDR_MANDATORY = 0,
	IPDR_OPTIONAL = 1
} AttributeOptional;

/*!
 * Defining Structure of List of Attribute Enumerations
 */

typedef struct ListAttributeEnum {
	char* pAttributeEnum_;
	struct ListAttributeEnum * pNext_;
} ListAttributeEnum;

/*!
 * Defining Structure of SchemaElement IPDR v3.5
 */


/*!
 * Defining Structure of SchemaElement
 */

typedef struct SchemaElement {
	char* attributeName_;
	char* attributeType_;
	char* complexAttributeType_;
 	char* documentation_;
 	char* derivedType_;
	char* unit_;
 	char* reference_;
 	char* status_;
	List* pListAttributeEnum_;
	List* pListReference_;
	List* pListMinOccurs_;
	List* pListMaxOccurs_;
 	AttributeOptional pAttributeOptional_;
   int noOfElements_;
} SchemaElement;


/*!
 * Defining Structure of ListSchemaElement
 */
typedef struct ListSchemaElement {
	SchemaElement* pSchemaElement_;
	struct ListSchemaElement* pNext_;
} ListSchemaElement;

/*!
 * Defining Structure of SchemaLookup
 */
typedef struct SchemaLookup {
	char* serviceType_;
	ListSchemaElement* pListSchemaElement_;
	ListSchemaElement* pListServiceElement_;
} SchemaLookup;

/*!
 * Defining Structure of ListSchemaLookup
 */
typedef struct ListSchemaLookup {
	SchemaLookup* pSchemaLookup_;
	struct ListSchemaLookup* pNext_;
} ListSchemaLookup;


typedef struct Schema
{         
	ListSchemaLookup* pListSchemaLookup_;
	ListNameSpaceInfo* pListNameSpaceInfo_;
	int SchemaValidationFlag_;
}Schema;
 
/*!
 *  This structure contains the parameters used by most 
 *  of the functions & This function will be passed across 
 *  various functions as an input parameter.
 */
typedef struct IPDRCommonParameters
{
	DocType DocType_;
	void* pStreamHandle_;            
   char* pStreamName_;
	DocState DocState_;            
	IPDRCount IPDRCount_; /* Union of ipdrRecordCount & offsetValue */
	HashTable* pDescriptorLookupHashTable_;
	ListAttributeDescriptor* pDescriptorLookupList_;
	Schema* pSchema_;
	int descriptorCount_;
	ListErrorStructure* pListErrorStructure_;
	ErrorStructure* pExceptionStructure_;
	ListFNFData *pListFNFData_;
	DocEnd *pDocEnd_;
   /* New member to store schema version */
   char schemaVersion[MAX_SCHEMA_VERSION];
} IPDRCommonParameters;

typedef struct ComplexElement {
	char* attributeName_;
	char* attributeType_;
 	char* documentation_;
 	char* derivedType_;
	char* unit_;
 	char* reference_;
 	char* status_;
	List* plstReference_;
	List* plstMinOccurs_;
	List* pListAttributeEnum_;
 	AttributeOptional pAttributeOptional_;
} ComplexElement;

typedef struct ListComplexElement {

   ComplexElement* pComplexElement_;
   struct ListComplexElement *pNext_;

} ListComplexElement;

#endif
