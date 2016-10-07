/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/



/*****************************************************
 * File                 : IPDRMemory.h		     *
 * Description          : Memory functions           *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 * 15/01/02   Infosys      Created                   *
 *****************************************************/

#ifndef _IPDRMEMORY_H
#define _IPDRMEMORY_H

#include <ctype.h>
#include <sys/malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <sys/timeb.h>

#include "utils/IPDRCommon.h"
#include "utils/errorCode.h"


/* Function prototypes for DocEnd */
DocEnd* newDocEnd(void);
int freeDocEnd(DocEnd* pDocEnd);
int printDocEnd(DocEnd* pDocEnd);

/* Function prototypes for IPDRStreamElement */
IPDRStreamElement* newIPDRStreamElement(int StreamElementFlag);
int freeIPDRStreamElement(IPDRStreamElement* pIPDRStreamElement);
int printIPDRStreamElement(IPDRStreamElement* pIPDRStreamElement);


/* Function prototypes for StreamElement */
StreamElement* newStreamElement(int StreamElementFlag);
int freeStreamElement(StreamElement* pStreamElement);
int printStreamElement(StreamElement* pStreamElement);


/* Function prototypes for ListServiceDefinitionURI */
int addListServiceDefinitionURI(
				ListServiceDefinitionURI** pHeadRef, 
				char* serviceDefinitionURI
				);
int appendListServiceDefinitionURI(
				ListServiceDefinitionURI **pHeadRef,
				char* serviceDefinitionURI
				);
int getLengthListServiceDefinitionURI(
				ListServiceDefinitionURI* 
				pListServiceDefinitionURI
				);
int freeListServiceDefinitionURI(ListServiceDefinitionURI **pHeadRef);
int printListServiceDefinitionURI(
				ListServiceDefinitionURI* pListServiceDefinitionURI
				);


/* Function prototypes for ListIPDRData */
int addListIPDRData(
			ListIPDRData** pHeadRef, 
			void* dataValue,
			int dataType
			); 
int appendListIPDRData(
			ListIPDRData** pheadRef, 
	 		void* dataValue,
			int dataType
			); 

int freeListIPDRData(ListIPDRData** pHeadRef);
int freeListNodeIPDRData(ListIPDRData** pHeadRef, int node);

int getLengthListIPDRData(ListIPDRData* pListIPDRData);
int printListIPDRData(ListIPDRData* pListIPDRData);

/* Function prototypes for ListNameSpaceInfo */
int addListNameSpaceInfo(
			ListNameSpaceInfo** pHeadRef, 
			char* NameSpaceInfoURI, 
			char* NameSpaceInfoID
			);
int appendListNameSpaceInfo(ListNameSpaceInfo **pHeadRef,
			char* NameSpaceInfoURI, 
			char* NameSpaceInfoID
			);
int freeListNameSpaceInfo(ListNameSpaceInfo **pHeadRef);
int printListNameSpaceInfo(ListNameSpaceInfo* pListNameSpaceInfo);
int getLengthListNameSpaceInfo(ListNameSpaceInfo* pListNameSpaceInfo);


/* Function prototypes for ListAttributeDescriptor */
int addListAttributeDescriptor(
			ListAttributeDescriptor** pHeadRef, 
			char* attributeName, 
			char* attributeType,
			char* description,
			char* derivedType
			);
int appendListAttributeDescriptor(ListAttributeDescriptor** pHeadRef, 
      char* attributeName, 
      char* attributeType,
	  char* description,
	  char* derivedType,
	  char* complexType,
	  List* attrNameList,
     List* attrClassList,
     List* attrMaxOccursList,
     int noOfElements
	);

int checkAttribute(
			char *attribute, 
			ListAttributeDescriptor* pListAttributeDescriptor
			);
int freeListAttributeDescriptor(ListAttributeDescriptor** pHeadRef);
int getLengthListAttributeDescriptor(
			ListAttributeDescriptor* pListAttributeDescriptor);
int printListAttributeDescriptor(
			ListAttributeDescriptor* pListAttributeDescriptor);


/* Function prototypes for IPDRRecord */
IPDRRecord* newIPDRRecord(void);
int freeIPDRRecord(IPDRRecord* pIPDRRecord);
int printIPDRRecord(IPDRRecord* pIPDRRecord);


/* Function prototypes for FNFData */
FNFData* newFNFData(void);
int freeFNFData(FNFData* pFNFData);
int printFNFData(FNFData* pFNFData);

/* Function prototypes for XMLFNFData */
XMLFNFData* newXMLFNFData(void);
int freeXMLFNFData(XMLFNFData* pXMLFNFData);
int printXMLFNFData(XMLFNFData* pXMLFNFData);


/* Function prototypes for RecordDescriptor */
RecordDescriptor* newRecordDescriptor(void);
/*
int initRecordDescriptor(
			RecordDescriptor* pRecordDescriptor,
			int descriptorID,
			char* typeName,
			ListAttributeDescriptor* pListAttributeDescriptor
			);
*/
int initRecordDescriptor(
			RecordDescriptor* pRecordDescriptor,
			int descriptorID,
			char* typeName,
			ListAttributeDescriptor* pListAttributeDescriptor,
			ListAttributeDescriptor* pListServiceAttributeDescriptor
			);
int freeRecordDescriptor(RecordDescriptor* pRecordDescriptor);
int printRecordDescriptor(RecordDescriptor* pRecordDescriptor);


/* Function prototypes for IPDRData */
IPDRData* newIPDRData(int IPDRDataType);
int freeIPDRData(IPDRData* pIPDRData);
int printIPDRData(IPDRData* pIPDRData);

/* Function prototypes for IPDRCommonParameters */
IPDRCommonParameters* newIPDRCommonParameters(void);
int initIPDRCommonParameters(
				IPDRCommonParameters* pIPDRCommonParameters,
				DocType docType,
				void* streamHandle,
                char* streamName,
				DocState docState,
				long ipdrCount,
				int noOfBuckets,
				ListAttributeDescriptor* ListAttributeDescriptor,
				int descriptorCount,
				ListErrorStructure* ListErrorStructure,
				ErrorStructure* pExceptionStructure,
				ListFNFData* pListFNFData
				);
int freeIPDRCommonParameters(
			IPDRCommonParameters* pIPDRCommonParameters);
int printIPDRCommonParameters(
			IPDRCommonParameters* pIPDRCommonParameters);

/* Function prototypes for IPDRHeader */
IPDRHeader* newIPDRHeader(void);
int initIPDRHeader(
		   IPDRHeader* pIPDRHeader,
		   int version,
		   char* ipdrRecorderInfo,
		   hyper startTime,
		   char* defaultNameSpaceURI,
		   char* docId,
		   ListNameSpaceInfo* pListNameSpaceInfo,
		   ListServiceDefinitionURI* pListServiceDefinitionURI
		  );
int freeIPDRHeader(IPDRHeader* pIPDRHeader);
int printIPDRHeader(IPDRHeader* pIPDRHeader);


/* Function prototypes for NameSpaceInfo */
NameSpaceInfo* newNameSpaceInfo(void);
int initNameSpaceInfo(
			  NameSpaceInfo* pNameSpaceInfo,
			  char* nameSpaceURI,
			  char* nameSpaceID
			  );
int freeNameSpaceInfo(NameSpaceInfo* pNameSpaceInfo);
int printNameSpaceInfo(NameSpaceInfo* pNameSpaceInfo);


/* Function prototypes for AttributeDescriptor */
AttributeDescriptor* newAttributeDescriptor(void);
int initAttributeDescriptor(
			AttributeDescriptor* pAttributeDescriptor,
			char* AttributeName,
			char* AttributeType,
			char* AttributeDescription,
			char* AttributeDerivedType
							);
int freeAttributeDescriptor(AttributeDescriptor* pAttributeDescriptor);
int printAttributeDescriptor(AttributeDescriptor* pAttributeDescriptor);
int freeListNodeAttributeDescriptor(ListAttributeDescriptor** pHeadRef, int node);


/* Function prototypes for FNFType */
int initFNFType(
				FNFType* pFNFType, 
				char* serviceType,
				ListAttributeDescriptor* pListAttributeDescriptor
				);

FNFType* newFNFType(void);
int freeFNFType(FNFType* pFNFType);
int printFNFType(FNFType* pFNFType);

Schema* newSchema(void);

int addListFNFData(
					ListFNFData** pHeadRef,
					char* typeName,
					ListAttributeDescriptor* pListAttributeDescriptor,
					ListIPDRData* pListIPDRData);

int appendListFNFData(
					ListFNFData** pHeadRef,
					char* typeName,
					ListAttributeDescriptor* pListAttributeDescriptor,
					ListIPDRData* pListIPDRData);
int getLengthListFNFData(ListFNFData* pListFNFData);
int freeListFNFData(ListFNFData **pHeadRef);
int freeHeadListFNFData(ListFNFData **pHeadRef);
int printListFNFData(ListFNFData* pListFNFData);

int freeSchemaData(Schema* pSchema);
ListOfKeys* newListOfKeys();
int freeListOfKeys(ListOfKeys** pHeadRef);

int addListSchemaNameSpace(
						ListSchemaNameSpace** pHeadRef, 
						char* schemaNameSpace);

int appendListSchemaNameSpace(ListSchemaNameSpace **pHeadRef,
								   char* schemaNameSpace);
int getLengthListSchemaNameSpace(
			ListSchemaNameSpace* pListSchemaNameSpace);

int freeListSchemaNameSpace(ListSchemaNameSpace **pHeadRef);

int printListSchemaNameSpace(
				ListSchemaNameSpace* pListSchemaNameSpace);

/* Function prototypes for SchemaElement */

SchemaElement* newSchemaElement(void);

int initSchemaElement(SchemaElement* pSchemaElement, 
					  char* attributeName,
 					  char* attributeType,
					  char* complexAttributeType,
 			          char* documentation,
 			          char* derivedType,
			          char* unit,
			          char* reference,
			          char* status,
			          List* pListAttributeEnum_,
                   List* pListReference_,
                   List* pListMinOccurs_,
					  	 List* pListMaxOccurs_,
                 	 int noOfElements_,
			          AttributeOptional pAttributeOptional_
					);

int freeSchemaElement(SchemaElement * pSchemaElement);

int printSchemaElement(SchemaElement * pSchemaElement);


/* Function prototypes for ListAttributeEnum */
int addListAttributeEnum(ListAttributeEnum **pHeadRef, 
						 char* attributeEnum);

int appendListAttributeEnum(ListAttributeEnum **pHeadRef,
							char* attributeEnum
						   );

int getLengthListAttributeEnum(ListAttributeEnum * pListAttributeEnum);

int freeListAttributeEnum(ListAttributeEnum **pHeadRef);

int printListAttributeEnum(
				ListAttributeEnum* pListAttributeEnum);


/* Function prototypes for ListSchemaElement */

int addListSchemaElement(ListSchemaElement **pHeadRef, 
						 SchemaElement* pSchemaElement);

int appendListSchemaElement(ListSchemaElement **pHeadRef,
							SchemaElement* pSchemaElement);

int getLengthListSchemaElement(ListSchemaElement * pListSchemaElement);

int freeListSchemaElement(ListSchemaElement **pHeadRef);

int printListSchemaElement(
				ListSchemaElement* pListSchemaElement);

/* Function prototypes for SchemaLookUp */
int printSchemaLookup(SchemaLookup* pSchemaLookup);

int freeSchemaLookup(SchemaLookup * pSchemaLookup);

SchemaLookup* newSchemaLookup(void);

/* Function prototypes for ListSchemaLookup */
int addListSchemaLookup(ListSchemaLookup **pHeadRef, 
						 SchemaLookup* pSchemaLookup);

int appendListSchemaLookup(ListSchemaLookup **pHeadRef,
							SchemaLookup* pSchemaLookup
						   );

int getLengthListSchemaLookup(ListSchemaLookup* pListSchemaLookup);

int freeListSchemaLookup(ListSchemaLookup **pHeadRef);

int printListSchemaLookup(
				ListSchemaLookup* pListSchemaLookup);

/*IPDR v3.5, structures and arrays*/
int initComplexElement(ComplexElement* pComplexElement, 
					  char* attributeName,
 					  char* attributeType,
 			          char* documentation,
 			          char* derivedType,
			          char* unit,
			          char* reference,
			          char* status,
                  List* plstReference_,
                  List* plstMinOccurs_,
			          List* pListAttributeEnum_,
			          AttributeOptional pAttributeOptional_
					);

int addListComplexElement(ListComplexElement **pHeadRef, 
						 ComplexElement* pComplexElement);


int appendListComplexElement(ListComplexElement **pHeadRef,
							ComplexElement* pComplexElement);

ComplexElement* newComplexElement(void);

int freeListComplexElement(ListComplexElement **pHeadRef);

int freeComplexElement(ComplexElement * pComplexElement);

int appendListComplexElementWithSchemaElement(ListComplexElement **pHeadRef,
															 ListSchemaElement* pListSchemaElement);

int getLengthListComplexElement(ListComplexElement * pListComplexElement);

int printListComplexElement( ListComplexElement* pListComplexElement);

int printComplexElement(ComplexElement * pComplexElement);

int appendListIPDRDataComplex(struct ListIPDRData** pHeadRef, 
	 			 void* dataValue[],
				 int dataType[]
); 

int appendListIPDRComplexData(struct ListIPDRComplexData** pHeadRef, 
				 struct ListIPDRComplex* pListIPDRComplex,
				 int dataType
); 
 
int appendListIPDRArrayData(struct ListIPDRArrayData** pHeadRef, 
	 			 void* dataValue,
				 int dataType
);
 
int appendListArrayToIPDRData(struct ListIPDRData** pHeadRef, 
				 struct ListIPDRArrayData *pListIPDRArrayData,
				 int noOfElements,
				 int dataType
);

int appendListStructureToIPDRData(struct ListIPDRData** pHeadRef, 
				 struct ListIPDRStructureData *pListIPDRStructureData,
				 int dataType
);

int appendListIPDRComplex(struct ListIPDRComplex** pHeadRef, 
	 			 void* dataValue[MAX_LEN],
				 int dataType
);

int appendListIPDRWithComplexData(struct ListIPDRData** pHeadRef, 
										struct ListIPDRComplexData* pListIPDRComplexData[], 
										int dataType
);

int appendListStrutureToIPDRData(struct ListIPDRStructureData** pHeadRef, 
				 struct ListIPDRComplexData *pListIPDRComplexData,
				 int dataType
);
 
int appendListIPDRArrayDataToComplexData(struct ListIPDRArrayData** pHeadRef, 
	 			 IPDRDataUnion ipdrDataUnion,
				 int dataType
);
 
int appendListIPDRComplexToComplexData(struct ListIPDRComplex** pHeadRef, struct ListIPDRComplex* pListIPDRComplex);

int freeListIPDRComplex(ListIPDRComplex** pHeadRef);

int freeListIPDRArrayData(ListIPDRArrayData** pHeadRef);

int freeListIPDRStructureData(ListIPDRStructureData** pHeadRef);

int freeListIPDRComplexData(ListIPDRComplexData** pHeadRef);
 
#endif
