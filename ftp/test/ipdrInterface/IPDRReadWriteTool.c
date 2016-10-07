/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/


/*****************************************************
 * File                 : IPDRReadWriteTool.c            *
 * Description          :                            *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 * 01/22/02					Created                  *
 *****************************************************/                      
                                                                           
#include "ipdrInterface/IPDRWriteTool.h"
#include "ipdrInterface/IPDRReadTool.h"
#include "ipdrInterface/IPDRFTPInterface.h"


/*!
 *  NAME:
 *      testWriteFTP() - Called in Producer flow.
 *  DESCRIPTION:
 *				 
 *  
 *  ARGUMENTS:
 *		
 *			docType (IN)
 *          testDataFiles (IN)
 *			outFile (IN)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int testWriteFTP(
		 int docType, 
		 char** testDataFiles,
		 char* outFile
		 )
{
	unsigned int maxLength = 0;
	unsigned int numOfFiles = 0;
	unsigned int roundRobFlag = 0;
	int recordCount = 0;
	int errorSeverity = 0;

	int* pErrorCode = NULL;
	hyper* pCurrTime = NULL;
	List* pFileRecList = NULL;
	List* pFileNamesList = NULL;		
        IPDRCommonParameters* pIPDRCommonParameters;
	ListNameSpaceInfo* pListNameSpaceInfo; 
	ListServiceDefinitionURI* pListServiceDefinitionURI;
	ListSchemaNameSpace* pListSchemaNameSpace = NULL;
	ListNameSpaceInfo* pCurrentListNameSpaceInfo = NULL;
	  
	char* ipdrRecorderInfo = "IPDRDocTest";
	char* defaultNameSpace = "http://www.ipdr.org/namespaces/ipdr";
	char* docID;
	int index = 0;

	pIPDRCommonParameters = newIPDRCommonParameters();
	pListNameSpaceInfo = NULL;
	pListServiceDefinitionURI = NULL;
 
	pErrorCode = (int *) calloc(1, sizeof(int));
	pCurrTime = (hyper *) calloc(1, sizeof(*pCurrTime));
	docID = (char *) calloc(MAX_ATTRIBUTE_NAME, sizeof(char));

	if(pErrorCode == NULL || pCurrTime == NULL || docID == NULL ) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		exceptionHandler(pIPDRCommonParameters, pErrorCode, 
			"main"); 
	}

	
  
	//!populateWriteIPDRCommonParam(pIPDRCommonParameters, pErrorCode);
	errorSeverity = populateWriteIPDRCommonParam(pIPDRCommonParameters, pErrorCode);
	if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "populateWriteIPDRCommonParam"); 
	}
	else if (errorSeverity == IPDR_ERROR) {
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "populateWriteIPDRCommonParam"); 
		printExceptionStructure (pIPDRCommonParameters->pExceptionStructure_); 
		free(docID);
		free(pCurrTime);
		free(pErrorCode);		
		freeIPDRCommonParameters(pIPDRCommonParameters);
		return (IPDR_ERROR);
	}

		                                                   	
//	errorSeverity = validateInputParams(argc, argv, &numOfFiles, 
//		&roundRobFlag,&recordCount,&pFileNamesList,pIPDRCommonParameters,
//		pErrorCode);


	if (docType == 0)
	{
			pIPDRCommonParameters->DocType_= XDR;
					
	}
	else if  (docType == 1) 
	{
			pIPDRCommonParameters->DocType_ = XML;
				
	}


	if ((pIPDRCommonParameters->pStreamHandle_ = 
		fopen(outFile, "w+")) == NULL) {
			*pErrorCode = (IPDR_INVALID_FILE);
			return(IPDR_ERROR);
	}
			/*add all the filenames to a list*/
    
	for(index=0; testDataFiles[index];index++){
		appendNode(&pFileNamesList,testDataFiles[index]);
	}
	numOfFiles = index;

   	/* To read the contents of the files into strings */
	errorSeverity = readFileContents(&pFileRecList,pFileNamesList,
		numOfFiles, &pListServiceDefinitionURI,&pListNameSpaceInfo,pErrorCode);
	if (errorSeverity == IPDR_WARNING) {
   errorHandler(pIPDRCommonParameters, pErrorCode,"readFileContents"); 
  }
  else if (errorSeverity == IPDR_ERROR) {
   pIPDRCommonParameters->DocState_ = NONWRITABLE; 
   exceptionHandler(pIPDRCommonParameters, pErrorCode, "readFileContents"); 
   return (IPDR_ERROR);
  }
 
 
			
	/* To calculate the length of the longest file*/
	errorSeverity = findMaxLength(pFileRecList, numOfFiles, &maxLength,
		pErrorCode);
		if (errorSeverity == IPDR_WARNING) {
	   errorHandler(pIPDRCommonParameters, pErrorCode,"findMaxLength"); 
	  }
	  else if (errorSeverity == IPDR_ERROR) {
	   pIPDRCommonParameters->DocState_ = NONWRITABLE; 
	   exceptionHandler(pIPDRCommonParameters, pErrorCode, "findMaxLength"); 
	   return (IPDR_ERROR);
	  }
	
	/* getTime (in milliseconds) and UUID to send to writeHeader*/
     getTime(pCurrTime, pErrorCode);
	 getUUID(docID, pErrorCode);

     #ifdef DEBUG_IPDR
	 {
	 printf("\n\n Printing ListServiceDefinitionURI ");
	 printListDerviceDefinitionURI(pListServiceDefinitionURI);
	 }
	#endif

	 errorSeverity = setSchemaData(
								  pIPDRCommonParameters, 
								  pListNameSpaceInfo, 
								  pListServiceDefinitionURI, 
								  defaultNameSpace,
								  pErrorCode
								 );

	if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, "setSchemaData");	
	}
	else if (errorSeverity == IPDR_ERROR) {
	
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "setSchemaData");	
		return (IPDR_ERROR);
	}

        pCurrentListNameSpaceInfo = pListNameSpaceInfo;
        while(pListNameSpaceInfo != NULL) {
                appendListSchemaNameSpace(&pListSchemaNameSpace,
                    pListNameSpaceInfo->NameSpaceInfo_.nameSpaceURI_);
                    pListNameSpaceInfo = pListNameSpaceInfo->pNext_;
        }
        pListNameSpaceInfo = pCurrentListNameSpaceInfo;


	 errorSeverity = writeHeader(pIPDRCommonParameters, *pCurrTime, 
		 ipdrRecorderInfo, defaultNameSpace, pListNameSpaceInfo, 
		 pListServiceDefinitionURI, docID,pListSchemaNameSpace, pErrorCode);

	 if (errorSeverity == IPDR_WARNING) {
	   errorHandler(pIPDRCommonParameters, pErrorCode,"writeHeader"); 
	  }
	  else if (errorSeverity == IPDR_ERROR) {
	   pIPDRCommonParameters->DocState_ = NONWRITABLE; 
	   exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeHeader"); 
	   return (IPDR_ERROR);
	  }

	  
	/* Print the records present in input files into the xdr file*/
	errorSeverity = printRecords(pFileRecList, maxLength,numOfFiles,
		  roundRobFlag,recordCount,pIPDRCommonParameters,
		  pListServiceDefinitionURI,pErrorCode);

	
	if (errorSeverity == IPDR_WARNING) {
	   errorHandler(pIPDRCommonParameters, pErrorCode,"printRecords"); 
	  }
	  else if (errorSeverity == IPDR_ERROR) {
	   pIPDRCommonParameters->DocState_ = NONWRITABLE; 
	   exceptionHandler(pIPDRCommonParameters, pErrorCode, "printRecords"); 
	   return (IPDR_ERROR);
	  }

    errorSeverity = writeDocEnd(pIPDRCommonParameters, pErrorCode);

	 if (errorSeverity == IPDR_WARNING) {
	   errorHandler(pIPDRCommonParameters, pErrorCode,"writeDocEnd"); 
	  }
	  else if (errorSeverity == IPDR_ERROR) {
	   pIPDRCommonParameters->DocState_ = NONWRITABLE; 
	   exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeDocEnd"); 
	   printExceptionStructure (pIPDRCommonParameters->pExceptionStructure_); 
	   free(docID);
	   free(pCurrTime);
	   free(pErrorCode);
	   freeIPDRCommonParameters(pIPDRCommonParameters);
	   return (IPDR_ERROR);
	  }
		
	freeListNameSpaceInfo(&pListNameSpaceInfo);
	freeListServiceDefinitionURI(&pListServiceDefinitionURI);
	free(docID);
	free(pCurrTime);
	freeList(&pFileRecList);
	free(pErrorCode);
	return(IPDR_OK);

}

/*!
 *  NAME:
 *      testReadFTP() - Called in Consumer flow.
 *  DESCRIPTION:
 *				 
 *  
 *  ARGUMENTS:
 *		
 *			docType (IN)
 *          inputFile (IN)
 *			outFile (IN)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int testReadFTP(
		 int docType, 
		 char* inputFile,
		 char* outFile
		 )
{
	int errorSeverity;
	int countList = 0;
	int count = 0;
	int commaCount = 0;
	int countComma = 0;
	int lengthAttrDesc = 0;
	int lengthServURI = 0;
	unsigned int schemaValFlag = 0;
	unsigned int outDirFlag = 0;
	//!
	unsigned int lengthFileName = 0;
	unsigned int lenStrServInfoAttrName = 0;
	int countListNameSpaceInfo = 0;
	//!
	char* fileName;
	char* outDirectory;
	//!
	char* pServInfoAttrName;
	char* strServInfoAttrName;
	char* pTempServInfoAttrName;
	char* pNameSpaceURI;
	char* pNameSpaceID;
	char* token;
	//!
	int* pErrorCode = NULL;

	FILE* fp;
	FNFType** pFNFType;
	IPDRCommonParameters* pIPDRCommonParameters;
	IPDRHeader* pIPDRHeader;
	FNFData* pFNFData = NULL;
	ListServiceInfo* pListServiceInfo = NULL;
	ListNameSpaceInfo* pCurr = NULL;
	ListServiceDefinitionURI* pCurrent = NULL;
	ListAttributeDescriptor* pCurrentAttrDesc = NULL;
	ListNameSpaceInfo* pListNameSpaceInfo = NULL;
	//!
	ListNameSpaceInfo* pCurrentListNameSpaceInfo = NULL;
	//!

	pErrorCode = (int *) calloc(1, sizeof(int));
	//!fileName = (char *) calloc(MAX_ATTRIBUTE_NAME, sizeof(char));
	outDirectory =  (char *) calloc(MAX_ATTRIBUTE_NAME, sizeof(char));

    //strcpy(outDirectory,outFile);

    pFNFData = newFNFData();

	if(pFNFData == NULL || pErrorCode == NULL || outDirectory == NULL) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		return(IPDR_ERROR);
	}


	/* Intializing structures */
	pIPDRCommonParameters = newIPDRCommonParameters();
	pIPDRHeader = newIPDRHeader();

	errorSeverity = populateReadIPDRCommonParam(pIPDRCommonParameters, pErrorCode);
	if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "populateReadIPDRCommonParam"); 
	}
	else if (errorSeverity == IPDR_ERROR) {
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "populateReadIPDRCommonParam"); 
		#ifdef DEBUG_IPDR
		{
			printExceptionStructure (pIPDRCommonParameters->pExceptionStructure_); 
		}
		#endif
		return (IPDR_ERROR);
	}

	if (docType == 0)
	{
			pIPDRCommonParameters->DocType_= XDR;
					
	}
	else if  (docType == 1) 
	{
			pIPDRCommonParameters->DocType_ = XML;
				
	}

	pIPDRCommonParameters->pStreamName_ = (char*) calloc((strlen(inputFile) + 1), sizeof(char));

	strcpy(pIPDRCommonParameters->pStreamName_, 
                                      inputFile);

	pIPDRCommonParameters->pStreamHandle_ = fopen(inputFile, "r");
	if(!pIPDRCommonParameters->pStreamHandle_)
		return (IPDR_ERROR);
		

	/* Read the header and get the default name space and Service 
	Definition URI list*/
    errorSeverity = readHeader(pIPDRCommonParameters, pIPDRHeader, pErrorCode);
	
	if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "readHeader"); 
	}
	else if (errorSeverity == IPDR_ERROR) {
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "readHeader"); 
		#ifdef DEBUG_IPDR
		{
			printExceptionStructure (pIPDRCommonParameters->pExceptionStructure_); 
		}
		#endif
		return (IPDR_ERROR);
	}

	#ifdef DEBUG_IPDR 
		printListFNFData(pIPDRCommonParameters->pListFNFData_);
    #endif

	if(schemaValFlag) {
		errorSeverity = setSchemaData(
									  pIPDRCommonParameters, 
									  pIPDRHeader->pOtherNameSpaceInfo_, 
									  pIPDRHeader->pListServiceDefinitionURI_, 
									  pIPDRHeader->defaultNameSpaceURI_,   
									  pErrorCode
									 );
		if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "setSchemaData"); 
		}
		else if (errorSeverity == IPDR_ERROR) {
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "setSchemaData"); 
			#ifdef DEBUG_IPDR
			{
				printExceptionStructure (pIPDRCommonParameters->pExceptionStructure_); 
			}
			#endif
		return (IPDR_ERROR);
		}
	}
	
	/* Traversing the ListServiceDefinitionURI in IPDRHeader*/
	pCurrent =  pIPDRHeader->pListServiceDefinitionURI_;
	pCurr = pIPDRHeader->pOtherNameSpaceInfo_;
	
	lengthServURI = getLengthListServiceDefinitionURI
		(pIPDRHeader->pListServiceDefinitionURI_);

    pFNFType = (FNFType**) calloc (lengthServURI, sizeof(FNFType*));
	while (pCurrent != NULL) {

		/* Calls generateDescriptor with the input schema URI and gets 
		corresponing FNFType*/

		pFNFType[countList] = newFNFType();	

		// Descriptor from Schema
		errorSeverity = generateSchemaDescriptor(pCurrent->pServiceDefintionURI_, 
			 pFNFType[countList],&pListNameSpaceInfo, pErrorCode);
		if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "generateSchemaDescriptor"); 
		}
		else if (errorSeverity == IPDR_ERROR) {
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "generateSchemaDescriptor"); 
			#ifdef DEBUG_IPDR
			{
				printExceptionStructure (pIPDRCommonParameters->pExceptionStructure_); 
			}
			#endif
		return (IPDR_ERROR);
		}


		lengthAttrDesc = getLengthListAttributeDescriptor
			(pFNFType[countList]->pListAttributeDescriptor_);
		//!
		lengthFileName = strlen(pFNFType[countList]->serviceType_) + strlen(outFile) + 5;
		fileName = (char *) calloc (100, sizeof(char));
		if (fileName == NULL) {
			*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
			return(IPDR_ERROR);
		}
		//!

		strcpy(fileName, pFNFType[countList]->serviceType_);
		strcat(fileName, ".csv");
        outDirFlag = 1;
		if(outDirFlag == 1) {
		strcpy(outDirectory,outFile);
		//strcat(outDirectory, "\\");
		strcat(outDirectory, fileName);
		strcpy(fileName,outDirectory);
		}
	
		if((fp = fopen(fileName, "w")) == NULL) {
			 *pErrorCode = IPDR_INVALID_FILE;
			return(IPDR_ERROR);
		}

		/*Printing the uri on the first line of the output file(s)*/
		#ifdef DEBUG_IPDR 
			printf("\nFileName Number %d is %s ",countList,fileName);
		#endif
	    fprintf(fp, "%s", pCurrent->pServiceDefintionURI_);

		if(lengthAttrDesc == 1) {
		fprintf(fp, "%c", ',');
		}
		else {
			for(countComma = 0; countComma < (lengthAttrDesc - 1); 
			countComma++) {
				fprintf(fp, "%c", ',');
			}
		}

		fprintf(fp, "%c", '\n');

		if (((strcmp( pCurr->NameSpaceInfo_.nameSpaceID_, "") == 0)||
					  (strcmp( pCurr->NameSpaceInfo_.nameSpaceID_, "ipdr") == 0))) {
		fprintf(fp, "%s", pCurr->NameSpaceInfo_.nameSpaceURI_);
		}
		else {
		fprintf(fp, "%s%c%s",pCurr->NameSpaceInfo_.nameSpaceID_,'$',
			pCurr->NameSpaceInfo_.nameSpaceURI_);

		}


		if(lengthAttrDesc == 1) {
			fprintf(fp, "%c", ',');
		}
		else {
			for(countComma = 0; countComma < (lengthAttrDesc - 1);
			countComma++) {
				fprintf(fp, "%c", ',');
			}
		}
		fprintf(fp, "%c", '\n');

	

	    /*Printing the attribute name column on the first line of the output 
		  file(s*/
		#ifdef DEBUG_IPDR
		{
		printf("\nprinting List Attr Desc");
		printListAttributeDescriptor(pFNFType[countList]->pListAttributeDescriptor_);
		}
		#endif

		pCurrentAttrDesc = pFNFType[countList]->pListAttributeDescriptor_;

		commaCount = 0;
	
		while (pCurrentAttrDesc != NULL) {
			lenStrServInfoAttrName = strlen(pCurrentAttrDesc->pAttributeDescriptor_->attributeName_) + 1;
			strServInfoAttrName = (char *) calloc (lenStrServInfoAttrName, sizeof(char));

			pServInfoAttrName = (char *) calloc(MAX_ATTRIBUTE_LENGTH, 
				sizeof(char));
	    	pTempServInfoAttrName = (char *) calloc(MAX_ATTRIBUTE_LENGTH, 
				sizeof(char));
			//!
			pNameSpaceURI = (char *) calloc(MAX_ATTRIBUTE_LENGTH,
				sizeof(char));
			pNameSpaceID = (char *) calloc(MAX_ATTRIBUTE_LENGTH, 
				sizeof(char));
			//!

			strcpy(strServInfoAttrName,
				pCurrentAttrDesc->pAttributeDescriptor_->attributeName_);
		
			token = strtok( strServInfoAttrName, "$" );
			while( token != NULL ) {
				if(count==1)
					strcpy(pNameSpaceURI,token);
				if(count==2)
					strcpy(pTempServInfoAttrName,token);
				token = strtok( NULL, "$" );
				count++;
			}

			//!
			/*Getting the name space ID from the ListNamespaceInfo*/
			pCurrentListNameSpaceInfo = pListNameSpaceInfo;

			for (countListNameSpaceInfo = 0; 
	            countListNameSpaceInfo < getLengthListNameSpaceInfo(pListNameSpaceInfo); 
				countListNameSpaceInfo++) { 

				if(strcmp(pNameSpaceURI, 
					pCurrentListNameSpaceInfo->NameSpaceInfo_.nameSpaceURI_)==0) {
					strcpy(pNameSpaceID,
						pCurrentListNameSpaceInfo->NameSpaceInfo_.nameSpaceID_);
				}

				pCurrentListNameSpaceInfo = pCurrentListNameSpaceInfo->pNext_;
			}
			
		   if (!((strcmp(pNameSpaceID, "") == 0)||
				(strcmp(pNameSpaceID, "ipdr") == 0))) {
			 
				strcpy(pServInfoAttrName,pNameSpaceID);
				strcat(pServInfoAttrName, ":");
				strcat(pServInfoAttrName, pTempServInfoAttrName);
		   }
		   else {
				strcpy(pServInfoAttrName,pTempServInfoAttrName);
		   }
		   //!
			  
		    fprintf(fp,"%s", pServInfoAttrName);
			if(commaCount < lengthAttrDesc - 1) {
			fprintf(fp,"%c", ',');
			commaCount++;
			}
			
		    pCurrentAttrDesc = pCurrentAttrDesc->pNext_;	
			//!
			free(pNameSpaceURI);
			free(pNameSpaceID);
			free(pTempServInfoAttrName);
			//!
			free(pServInfoAttrName);
			free(strServInfoAttrName);
		}
		fprintf(fp,"\n");
		
		errorSeverity = appendListServiceInfo(&pListServiceInfo, fp, 
			pFNFType[countList], pListNameSpaceInfo);

		if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "appendListServiceInfo"); 
		}
		else if (errorSeverity == IPDR_ERROR) {
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "appendListServiceInfo"); 
			#ifdef DEBUG_IPDR
			{
				printExceptionStructure (pIPDRCommonParameters->pExceptionStructure_); 
			}
			#endif
		return (IPDR_ERROR);
		}
 
		pCurrent = pCurrent->pNext_;	
		pCurr = pCurr->pNext_;
		countList++;
	  
	  			
	}				
	
	/*To generate the csv file containing raw data values*/
	errorSeverity = generateRawData(pIPDRCommonParameters, pFNFData, pListServiceInfo, pErrorCode);
	if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "generateRawData"); 
	}
	else if (errorSeverity == IPDR_ERROR) {
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "generateRawData"); 
		#ifdef DEBUG_IPDR
		{
			printExceptionStructure (pIPDRCommonParameters->pExceptionStructure_); 
		}
		#endif
		return (IPDR_ERROR);
	}

	printf("\n Document decoded Successfully\n");

	for (countList = 0; (countList < lengthServURI); countList++) {
		freeFNFType(pFNFType[countList]);
			
	}
	//!
	if(fileName)
		free(fileName);
	//!
	free(outDirectory);
	free(pFNFType);
	freeIPDRCommonParameters(pIPDRCommonParameters);
	free(pErrorCode);

	return (0);
}

