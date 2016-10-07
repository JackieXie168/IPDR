/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "curl/curl.h"
#include "curl/types.h"
#include "curl/easy.h"

#include "common/IPDRDocWriter.h"
#include "common/IPDRDocReader.h"
#include "utils/UUIDUtil.h"

#include "common/schema.h"
#include "common/descriptor.h"
#include "common/getFile.h"
#include "utils/errorHandler.h"
#include "utils/IPDRCommon.h"
#include "utils/errorCode.h"

#define NO_OF_RECORDS           1000
#define PRINT_RECORD_COUNT      1

#define MAX_OPTIONAL_ATTRIBUTES 4

int writeRawIPDRDoc();
int writeIPDRDoc(int flag);
int readIPDRDoc(int flag);


int printUsage()
{

    printf("\nCorrect Usage is:");
    printf("\n./testXML <read/write> <schema Validation ON/OFF> ");  
    printf("\ne.g. ./testXML write OFF\n");
    return 0;
}


int main(int argc, char *argv[])
{
    int flag = 0;

    if (argc != 3)  {
        printUsage();
        return 0;
    }

    if ((strcmp(argv[2], "ON") != 0) && (strcmp(argv[2], "OFF") != 0)) {
        printUsage();
        return 0;
    }

    if ((strcmp(argv[1], "write") != 0) && (strcmp(argv[1], "read") != 0)) {
        printUsage();
        return 0;
    }

    if (strcmp(argv[1], "write") == 0) {
        if (strcmp(argv[2], "ON") == 0) 
			writeIPDRDoc(1);
        if (strcmp(argv[2], "OFF") == 0) writeRawIPDRDoc();
    }

    if (strcmp(argv[1], "read") == 0) {
        if (strcmp(argv[2], "ON") == 0) readIPDRDoc(1);
        if (strcmp(argv[2], "OFF") == 0) readIPDRDoc(0);
    }

    return 0;

}


int readIPDRDoc(int flag)
{
	IPDRCommonParameters* pIPDRCommonParameters;
	IPDRHeader* pIPDRHeader;
	FNFData* pFNFData;
	DocEnd* pDocEnd;
	int* pErrorCode = NULL;
	int numOfBuckets = 10; 
	void* streamHandle;
	int numOfElements = 1;

        /*----------------*/
        /* Initialization */
        /*----------------*/
	pIPDRCommonParameters = newIPDRCommonParameters();
	pIPDRHeader = newIPDRHeader();
	pDocEnd = newDocEnd(); 
	pErrorCode = (int *) calloc(1, 10);

	if ((streamHandle = fopen("IPDRXMLDoc.xml", "rb+")) == NULL) {
			printf("\nError in opening file");
		return -1;
	} else {
		printf("\nSuccess in opening file ");
	}

	initIPDRCommonParameters(
		pIPDRCommonParameters,
		XML,
		streamHandle,
                "IPDRXMLDoc.xml", 
		NONREADABLE,
		1,
		numOfBuckets,
		NULL,
		1,
		NULL,
		NULL,
		NULL
		);

        /*----------------*/
        /* Reading Header */
        /*----------------*/
	readHeader(pIPDRCommonParameters, pIPDRHeader, pErrorCode);
	
	#ifdef DEBUG_IPDR
		printf("\n\n\t\t Decoded Value\n");
		printf("IPDR Header\n");
		printIPDRHeader(pIPDRHeader);
	#endif

        /*---------------------*/
        /* Setting Schema Data */
        /*---------------------*/
    if (flag == 1) {
		printf("\nSchema Validation is ON ");
		setSchemaData(
						pIPDRCommonParameters, 
						pIPDRHeader->pOtherNameSpaceInfo_, 
						pIPDRHeader->pListServiceDefinitionURI_, 
						pIPDRHeader->defaultNameSpaceURI_,
						pErrorCode
					);
	
    } else {
        printf("\nSchema Validation is OFF ");
    }
	
        /*-----------------------*/
        /* Reading IPDR Document */
        /*-----------------------*/
	
	pFNFData = newFNFData(); 
	
	readIPDR(pIPDRCommonParameters, &pFNFData, pErrorCode);
	
	while(pFNFData != NULL) {
                if (PRINT_RECORD_COUNT == 1) printf("\t(%d) ", numOfElements);
		numOfElements++;
                if(numOfElements % 100 == 0) 
					printFNFData(pFNFData); 
		freeFNFData(pFNFData);

		pFNFData = newFNFData(); 
		readIPDR(pIPDRCommonParameters, &pFNFData, pErrorCode);
	}
	
        /*----------------*/
        /* Reading DocEnd */
        /*----------------*/
	readDocEnd(pIPDRCommonParameters, pDocEnd, pErrorCode);
   printListErrorStructure(pIPDRCommonParameters->pListErrorStructure_);

	free(pErrorCode);
	freeListSchemaNameSpace(&pIPDRHeader->pListSchemaNameSpace_);  
	freeIPDRHeader(pIPDRHeader);
	freeDocEnd(pDocEnd);
	freeIPDRCommonParameters(pIPDRCommonParameters);

	return (IPDR_OK);	

}


int writeIPDRDoc(int flag )
{
	int count;
	IPDRCommonParameters* pIPDRCommonParameters;
	FNFType* pFNFType; 
	ListNameSpaceInfo* pListNameSpaceInfo; 
	ListServiceDefinitionURI* pListServiceDefinitionURI;
    ListSchemaNameSpace* pListSchemaNameSpace = NULL;
	FNFData* pFNFData;
	char *OptionalAttributes[MAX_OPTIONAL_ATTRIBUTES];
	void* streamHandle;
	int numOfBuckets = 9; 

	ListIPDRData* pListIPDRData = NULL;
	
	int* pErrorCode = NULL;
	void* data = NULL;
	void* dataUTF8 = NULL;
        float dataFloat = 11.2233f;

	hyper* startTime = (hyper *) calloc(1, sizeof(hyper));
	char* docId =(char *) calloc(sizeof(char), 100);	

	char* ipdrRecorderInfo = "testRecorderInfo";
	char* defaultNameSpace = "http://www.ipdr.org/namespaces/ipdr";

	char *RemoteURI;
	char configParams[] = {""};


	/*----------------*/
    /* Initialization */
    /*----------------*/
	pErrorCode = (int *) calloc(1, 10);
	RemoteURI = (char *) calloc(sizeof(char), 100);
    //strcpy(RemoteURI, "WS3.0-A.0.xsd");
    strcpy(RemoteURI, "structure.xsd");
    //strcpy(RemoteURI, "Array.xsd");
    //strcpy(RemoteURI, "structureWithinStructure.xsd");

	pIPDRCommonParameters = newIPDRCommonParameters();
	pFNFData = newFNFData();
	pFNFType = newFNFType();
	pListNameSpaceInfo = NULL;
	pListServiceDefinitionURI = NULL;
	
	if ((streamHandle = fopen("IPDRXMLDoc.xml", "w+")) == NULL) {
		printf("\nError in opening file");
		return -1;
	} 

	initIPDRCommonParameters(
		pIPDRCommonParameters,
		XML,
		streamHandle,
        "IPDRXMLDoc.xml",
		NONWRITABLE,
		1,
		numOfBuckets,
		NULL,
		1,
		NULL,
		NULL,
		NULL
		);

	appendListNameSpaceInfo(
		&pListNameSpaceInfo, "WS", "http://www.ws.com/namespaces/ipdr");

	//appendListServiceDefinitionURI(&pListServiceDefinitionURI, 
	//	"WS3.0-A.0.xsd");
	appendListServiceDefinitionURI(&pListServiceDefinitionURI, 
		"structure.xsd");
	//appendListServiceDefinitionURI(&pListServiceDefinitionURI, 
	//	"structureWithinStructure.xsd");
	//appendListServiceDefinitionURI(&pListServiceDefinitionURI, 
	//	"Array.xsd");
				
	appendListSchemaNameSpace(&pListSchemaNameSpace,
		"http://www.ipdr.org/namespaces/ipdr");

	
	OptionalAttributes[0] = (char *) calloc (sizeof(char), MAX_ATTRIBUTE_NAME);
	strcpy(OptionalAttributes[0], "qosDelivered");
	OptionalAttributes[1] = NULL;
	
	count = generateDescriptor(
			RemoteURI, 
			pFNFType,
			OptionalAttributes,
			pErrorCode
			);

	/*----------------------*/
	/* populating Data List */
	/*----------------------*/

/*
//Data for WS3...xsd
	for(count = 0; count < 1; count++) {
		dataUTF8 = (void *) calloc (1, 100);
		strcpy((char*)dataUTF8 , "IPDR1");
		appendListIPDRData(&pListIPDRData, dataUTF8, IPDRDATA_STRING);
		free(dataUTF8);
	}
	
	for(count = 0; count < 5; count++) {
		data = (void *) 2000;
		appendListIPDRData(&pListIPDRData, data, IPDRDATA_LONG);	
	}
*/

/*
//data for structure.xsd

	void* dataStructure[10];
	void* dataStructure1[10];
	void* dataStructure2[10];
	int dataType[10];
	ListIPDRComplex* pListIPDRComplex = NULL;
	ListIPDRComplexData* pListIPDRComplexData[10];

	memset(&dataStructure, 0, sizeof(dataStructure));
	memset(&dataStructure1, 0, sizeof(dataStructure1));
	memset(&dataStructure2, 0, sizeof(dataStructure2));
	memset(&pListIPDRComplexData, 0, sizeof(pListIPDRComplexData));
	
	for(count = 0; count < 2; count++) {
		dataStructure[count] = (void *) calloc (1, 10);
		strcpy((char *)dataStructure[count],"Girish");
		dataType[count] = IPDRDATA_STRING;
	}
	dataType[0] = IPDRDATA_ARRAY;
   dataType[2] = IPDRDATA_STRING;
	appendListIPDRDataComplex(&pListIPDRData, dataStructure, (int *)&dataType);

	for(count = 0; count < 2; count++) {
		data = (void *) 2000;
		appendListIPDRData(&pListIPDRData, data, IPDRDATA_LONG);	
	}
	
	dataStructure1[0] = (void *) 2000;
	appendListIPDRComplex(&pListIPDRComplex, dataStructure1, IPDRDATA_LONG); 		

	dataStructure1[0] = (void *) 2000;
	appendListIPDRComplex(&pListIPDRComplex, dataStructure1, IPDRDATA_LONG); 		

	for(count = 0; count < 2; count++) {
		dataStructure1[count] = (void *) 2000;
	}
	appendListIPDRComplex(&pListIPDRComplex, dataStructure1, IPDRDATA_LONG); 		

	appendListIPDRComplexData(&pListIPDRComplexData[0], pListIPDRComplex, IPDRDATA_STRUCTURE);	

	dataStructure2[0] = (void *) 2000;
	appendListIPDRComplex(&pListIPDRComplex, dataStructure2, IPDRDATA_LONG); 		

	dataStructure2[0] = (void *) 2000;
	appendListIPDRComplex(&pListIPDRComplex, dataStructure2, IPDRDATA_LONG); 		

	for(count = 0; count < 2; count++) {
		dataStructure2[count] = (void *) 2000;
	}
	appendListIPDRComplex(&pListIPDRComplex, dataStructure2, IPDRDATA_LONG); 		

	appendListIPDRComplexData(&pListIPDRComplexData[1], pListIPDRComplex, IPDRDATA_STRUCTURE);	

	appendListIPDRWithComplexData(&pListIPDRData,pListIPDRComplexData, IPDRDATA_STRUCTURE);
	
	free(dataStructure);
	free(dataStructure1);
	free(dataStructure2);
*/

//Array.xsd data

	void* dataStructure[10];
	int dataType[10];

	memset(&dataStructure, 0, sizeof(dataStructure));
	
	for(count = 0; count < 2; count++) {
		dataStructure[count] = (void *) calloc (1, 10);
		strcpy((char *)dataStructure[count],"Girish");
		dataType[count] = IPDRDATA_STRING;
	}
	dataType[0] = IPDRDATA_ARRAY;
   dataType[2] = IPDRDATA_STRING;
	appendListIPDRDataComplex(&pListIPDRData, dataStructure, (int *)&dataType);

	for(count = 0; count < 2; count++) {
		data = (void *) 2000;
		appendListIPDRData(&pListIPDRData, data, IPDRDATA_LONG);	
	}

/*
//data for structureWithinstruture.xsd

	void* dataStructure[10];
	ListIPDRComplex* pListIPDRComplex = NULL;
	ListIPDRComplexData* pListIPDRComplexData[10];

	memset(&dataStructure, 0, sizeof(dataStructure));
	memset(&pListIPDRComplexData, 0, sizeof(pListIPDRComplexData));

	dataStructure[0] = (void *) calloc (1, 10);
	strcpy((char *)dataStructure[0],"Girish");
	appendListIPDRComplex(&pListIPDRComplex, dataStructure, IPDRDATA_STRING);

	strcpy((char *)dataStructure[0],"BCC Layout");
	appendListIPDRComplex(&pListIPDRComplex, dataStructure, IPDRDATA_STRING);
	
	strcpy((char *)dataStructure[0],"Bangalore");
	appendListIPDRComplex(&pListIPDRComplex, dataStructure, IPDRDATA_STRING);

	appendListIPDRComplexData(&pListIPDRComplexData[0], pListIPDRComplex, IPDRDATA_STRUCTURE);	

	appendListIPDRWithComplexData(&pListIPDRData,pListIPDRComplexData, IPDRDATA_STRUCTURE);

	//free(dataStructure);
*/
	
	getTime(startTime, pErrorCode);

	getUUID(docId, pErrorCode);

	/*---------------------*/
        /* Setting Schema Data */
	/*---------------------*/
        if (flag == 1) {
        printf("\nSchema Validation is ON ");
	setSchemaData(
		  pIPDRCommonParameters, 
		  pListNameSpaceInfo, 
		  pListServiceDefinitionURI, 
		  defaultNameSpace,
		  pErrorCode);
    } else {
            printf("\nSchema Validation is OFF ");
    }
	
	/*----------------*/
	/* Writing Header */
	/*----------------*/
	writeHeader(pIPDRCommonParameters, 
		   *startTime, 
		   ipdrRecorderInfo,
		   defaultNameSpace, 
		   pListNameSpaceInfo, 
		   pListServiceDefinitionURI, 
		   docId, 
           pListSchemaNameSpace,
		   pErrorCode
		  );
	
	freeListSchemaNameSpace(&pListSchemaNameSpace);

	/*-----------------*/
    /* Writing Records */
	/*-----------------*/
    createFNFData(
		  pIPDRCommonParameters, 
		  pFNFType ,
		  pListIPDRData, 
		  pFNFData, 
		  pErrorCode
		 );

	for(count = 0; count < NO_OF_RECORDS; count++) {
	
	if (PRINT_RECORD_COUNT == 1) printf("\t(%d) ", count);	
	
    writeIPDR(
		  pIPDRCommonParameters, 
		  pFNFData, 
		  pErrorCode);
	}

	/*----------------*/
	/* Writing DocEnd */
	/*----------------*/
	writeDocEnd(pIPDRCommonParameters, pErrorCode);
	
	/*---------*/
	/* cleanUp */
	/*---------*/
	free(OptionalAttributes[0]);
	free(pErrorCode);
	free(docId);
	free(startTime);
	freeIPDRCommonParameters(pIPDRCommonParameters);
	freeFNFType(pFNFType); 
	freeListIPDRData(&(pListIPDRData));
	free(RemoteURI);
	freeFNFData(pFNFData);
	freeListNameSpaceInfo(&(pListNameSpaceInfo));
	freeListServiceDefinitionURI(&(pListServiceDefinitionURI));


	return (IPDR_OK);

}



int writeRawIPDRDoc()
{
	int count;
	IPDRCommonParameters* pIPDRCommonParameters;
	ListNameSpaceInfo* pListNameSpaceInfo; 
	ListServiceDefinitionURI* pListServiceDefinitionURI;
	ListSchemaNameSpace* pListSchemaNameSpace = NULL;


	FNFData* pFNFData;
	void* streamHandle;
	int errorSeverity = 0;
	int numOfBuckets = 9; 
 
	int* pErrorCode = NULL;
	int* data = NULL;
	void* dataUTF8 = NULL;
	hyper* startTime = (hyper *) calloc(1, sizeof(hyper));
	char* docId = (char *) calloc(sizeof(char), 100); 
 
	char* ipdrRecorderInfo = "testRecorderInfo";
	char* defaultNameSpace = "http://www.ipdr.org/namespaces/ipdr";
	 
	pErrorCode = (int *) calloc(1, 10);
 
	/*---------------*/
	/* Intialization */
	/*---------------*/
	 
	pIPDRCommonParameters = newIPDRCommonParameters();
	pFNFData = newFNFData();
	pListNameSpaceInfo = NULL;
	pListServiceDefinitionURI = NULL;
	 
	if ((streamHandle = fopen("IPDRXMLDoc.xml", "w+")) == NULL) {
	printf("\nError in opening file");
	return -1;
	} else {
	#ifdef IPDR_DEBUG
	printf("\nSuccess in opening file ");
	#endif
	}
	 
	initIPDRCommonParameters(
		pIPDRCommonParameters,
		XML,
		streamHandle,
		"IPDRXMLDoc.xml",
		NONWRITABLE,
		1,
		numOfBuckets,
		NULL,
		1,
		NULL,
		NULL,
		NULL
	);
 

	appendListNameSpaceInfo(
		&pListNameSpaceInfo, "", "http://www.ipdr.org/namespaces/ipdr");
 
	appendListSchemaNameSpace(&pListSchemaNameSpace,
		"http://www.bestvendor.com/ipdr/namespaces");

 
	appendListServiceDefinitionURI(&pListServiceDefinitionURI, 
		"VoIP3.0-A.0ID.xsd");
 

	strcpy(pFNFData->typeName_, "testService");
	//appendListAttributeDescriptor(&pFNFData->pListAttributeDescriptor_, "tryUTF8", "UTF8_Array", "", "UTF8_Array");
	//appendListAttributeDescriptor(&pFNFData->pListAttributeDescriptor_, "tryLong", "integer", "", "integer");
	//appendListAttributeDescriptor(&pFNFData->pListAttributeDescriptor_, "try1UTF8", "UTF8_Array", "", "UTF8_Array");
	//appendListAttributeDescriptor(&pFNFData->pListAttributeDescriptor_, "try1Long", "integer", "", "integer");
 
  
	dataUTF8 = (void *) calloc (1, 100);
	strcpy((char*)dataUTF8 , "IPDR1");
	//appendListIPDRData(&pFNFData->pListIPDRData_, dataUTF8, IPDRDATA_UTF8ARRAY);
	free(dataUTF8);
	 
	data = (void *) 1000;
	appendListIPDRData(&pFNFData->pListIPDRData_, data, IPDRDATA_LONG); 
	 
	dataUTF8 = (void *) calloc (1, 100);
	strcpy((char*)dataUTF8 , "IPDR2");
	//appendListIPDRData(&pFNFData->pListIPDRData_, dataUTF8, IPDRDATA_UTF8ARRAY);
	free(dataUTF8);
 
	data = (void *) 2000;
	appendListIPDRData(&pFNFData->pListIPDRData_, data, IPDRDATA_LONG); 
	 

	errorSeverity = getTime(startTime, pErrorCode);
	if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, "getTime"); 
	}
	else if (errorSeverity == IPDR_ERROR) {
	if (DEBUG_ME) printf("\n Error in getTime");
	exceptionHandler(pIPDRCommonParameters, pErrorCode, "getTime"); 
	#ifdef IPDR_DEBUG
	{
	printExceptionStructure(pIPDRCommonParameters->pExceptionStructure_); 
	}
	#endif
	  
	return (IPDR_ERROR);
	}
 
	errorSeverity = getUUID(docId, pErrorCode);
	if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, "getUUID"); 
	}
	else if (errorSeverity == IPDR_ERROR) {
		if (DEBUG_ME) printf("\n Error in getUUID");
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "getUUID"); 
  
	#ifdef IPDR_DEBUG
	{
		printExceptionStructure(pIPDRCommonParameters->pExceptionStructure_); 
	}
		#endif
  
	return (IPDR_ERROR);
	}
 
	/*----------------*/
	/* Writing Header */
	/*----------------*/
	errorSeverity = writeHeader(pIPDRCommonParameters, 
          *startTime, 
          ipdrRecorderInfo,
          defaultNameSpace, 
          pListNameSpaceInfo, 
          pListServiceDefinitionURI, 
          docId, 
		  pListSchemaNameSpace,
          pErrorCode
         );
	if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, "writeHeader"); 
	}
	else if (errorSeverity == IPDR_ERROR) {
	if (DEBUG_ME) printf("\n Error in writeHeader");
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeHeader"); 
	#ifdef IPDR_DEBUG
	{
	printExceptionStructure(pIPDRCommonParameters->pExceptionStructure_); 
	}
	#endif  
	  
	return (IPDR_ERROR);
	}

	freeListSchemaNameSpace(&pListSchemaNameSpace);

	for(count = 0; count < NO_OF_RECORDS; count++) {
 
	if (PRINT_RECORD_COUNT == 1) {
		printf("\t(%d) ", count); 
	}
 
	/*-----------------*/
	/* Writing Records */
	/*-----------------*/
	errorSeverity = writeIPDR(
          pIPDRCommonParameters, 
          pFNFData, 
          pErrorCode
			);
	if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, "writeIPDR"); 
	}
	else if (errorSeverity == IPDR_ERROR) {
		if (DEBUG_ME) printf("\n Error in writeIPDR");
			exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDR"); 
		#ifdef IPDR_DEBUG
		{
			printExceptionStructure(pIPDRCommonParameters->pExceptionStructure_); 
		}
		#endif   
   
	return (IPDR_ERROR);
	}
 
	}
 
	/*----------------*/
	/* Writing DocEnd */
	/*----------------*/
	errorSeverity = writeDocEnd(pIPDRCommonParameters, pErrorCode);
	if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, "writeDocEnd"); 
	}
	else if (errorSeverity == IPDR_ERROR) {
		if (DEBUG_ME) printf("\n Error in writeDocEnd");
			exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeDocEnd"); 
  
		#ifdef IPDR_DEBUG
		{
		printExceptionStructure(pIPDRCommonParameters->pExceptionStructure_); 
		}
		#endif  
  
	return (IPDR_ERROR);
	}
 
		#ifdef DEBUG_IPDR
		printListErrorStructure(pIPDRCommonParameters->pListErrorStructure_);
		#endif 
 
	if (DEBUG_ME) 
	printf("\nDocument encoded successfully\n");
 
	free(pErrorCode);
	free(docId);
	free(startTime);
	freeListServiceDefinitionURI(&pListServiceDefinitionURI);
	freeListNameSpaceInfo(&pListNameSpaceInfo);
	freeIPDRCommonParameters(pIPDRCommonParameters);
	freeFNFData(pFNFData);
	 
	return (IPDR_OK);
 
}
 

 


