/* Copyright (C) 2002, The IPDR Organization, all rights reserved.  
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/


/*****************************************************
 * File                 : IPDRWriteTool.c            *
 * Description          :                            *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 * 01/22/02		  Created                    *
 *****************************************************/                      
                                                                           
#include "include/IPDRWriteTool.h"


char*  getStringBetweenDelims(char* string, char* startPoint, char* endPoint)
{
    char* val;
    int i = 0;
    size_t diff = 0;
                                                                                                                             
    if (startPoint == NULL)
    {
       startPoint = string;
    }
    if (endPoint == NULL)
    {
       endPoint = string + (strlen(string) - 1);
    }
                                                                                                                             
    diff = (endPoint - startPoint )+1;
    val = (char *) malloc(strlen(string) + 10);
    strncpy(val, (const char*)(startPoint), diff);
    val[(endPoint - startPoint +1)] = '\0';
                                                                                                                             
    return val;
}
 
 /*! main() takes command line arguments:-doctype=<XDR|XML> 
   -outfile=<OutputFileName> -recordcount=<No Of Records>
   -roundrobin <TestDataFile1> <TestDataFile2> <TestDataFile3> 
 */

int main(
		 int argc, 
		 char *argv[]
		 )
{
	unsigned int maxLength = 0;
	unsigned int numOfFiles = 0;
	unsigned int roundRobFlag = 0;
	unsigned int schemaValFlag = 0;
	unsigned int ipdrVersionFlag = 0;
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
	char* docID = NULL;
	char* ipdrVersion = NULL;
   int i = 0;

	pIPDRCommonParameters = newIPDRCommonParameters();
	pListNameSpaceInfo = NULL;
	pListServiceDefinitionURI = NULL;
 
	pErrorCode = (int *) calloc(1, sizeof(int));
	pCurrTime = (hyper *) calloc(1, sizeof(*pCurrTime));
	docID = (char *) calloc(MAX_ATTRIBUTE_NAME, sizeof(char));
	ipdrVersion = (char *) calloc(10, sizeof(char));

	if(pErrorCode == NULL || pCurrTime == NULL || docID == NULL ) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		exceptionHandler(pIPDRCommonParameters, pErrorCode, 
			"main"); 
	}

	
  
	errorSeverity = populateIPDRCommonParameters(pIPDRCommonParameters, pErrorCode);
	if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "populateIPDRCommonParameters"); 
	}
	else if (errorSeverity == IPDR_ERROR) {
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "populateIPDRCommonParameters"); 
		printExceptionStructure (pIPDRCommonParameters->pExceptionStructure_); 
		free(docID);
		free(pCurrTime);
		free(pErrorCode);		
		freeIPDRCommonParameters(pIPDRCommonParameters);
		return (IPDR_ERROR);
	}
	
	errorSeverity = validateInputParams(argc, argv, &numOfFiles, 
		&roundRobFlag, &schemaValFlag, &ipdrVersionFlag, ipdrVersion, &recordCount,
		&pFileNamesList,pIPDRCommonParameters, pErrorCode);
	
	if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "validateInputParams"); 
	}
	else if (errorSeverity == IPDR_ERROR) {
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "validateInputParams"); 
		printExceptionStructure (pIPDRCommonParameters->pExceptionStructure_); 
		free(docID);
		free(pCurrTime);
		free(pErrorCode);
		return (IPDR_ERROR);
	}

	/*IPDR v3.5, changes for vesrion*/
	if((ipdrVersionFlag == 1) && (schemaValFlag == 1)) {
		printf("Please set either -ipdrversion/-schemavalidation but not both\n");
		return (IPDR_ERROR);
	} else if((ipdrVersionFlag == 0) && (schemaValFlag == 0)) {
		printf("Please set either -ipdrversion/-schemavalidation\n");
		return(IPDR_ERROR);
	}

	if(ipdrVersionFlag == 1) {
		if((strcmp(ipdrVersion,IPDR_VERSION_3_5) != 0) && (strcmp(ipdrVersion,IPDR_VERSION_3_1) != 0)) {
			printf("The ipdr version, %s, ", ipdrVersion);
			printf("is not supported\n");
			return(IPDR_ERROR);
		} else {
			if((strcmp(ipdrVersion,IPDR_VERSION_3_5) == 0)) {
				strcpy(pIPDRCommonParameters->schemaVersion, IPDR_VERSION_3_5);
			}
			else if((strcmp(ipdrVersion,IPDR_VERSION_3_1) == 0)) {
				strcpy(pIPDRCommonParameters->schemaVersion, IPDR_VERSION_3_1);
			}
		}
	}

   /* To read the contents of the files into strings */
	errorSeverity = readFileContents(&pFileRecList,pFileNamesList,
		numOfFiles, &pListServiceDefinitionURI,&pListNameSpaceInfo,pErrorCode);
	if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "readFileContents"); 
	}
	else if (errorSeverity == IPDR_ERROR) {
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "readFileContents"); 
		printExceptionStructure (pIPDRCommonParameters->pExceptionStructure_); 
		free(docID);
		free(pCurrTime);
		free(pErrorCode);	
		freeIPDRCommonParameters(pIPDRCommonParameters);
		return (IPDR_ERROR);
	}
 
        
			
	/* To calculate the length of the longest file*/
	errorSeverity = findMaxLength(pFileRecList, numOfFiles, &maxLength,
		pErrorCode);
	if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, "findMaxLength"); 
	}
	else if (errorSeverity == IPDR_ERROR) {
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "findMaxLength"); 
		printExceptionStructure (pIPDRCommonParameters->pExceptionStructure_); 
		free(docID);
		free(pCurrTime);
		free(pErrorCode);
		freeIPDRCommonParameters(pIPDRCommonParameters);
		return (IPDR_ERROR);
	}
         
	/* getTime (in milliseconds) and UUID to send to writeHeader*/
    errorSeverity = getTime(pCurrTime, pErrorCode);
	if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, "getTime"); 
	}
	else if (errorSeverity == IPDR_ERROR) {
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "getTime"); 
		printExceptionStructure (pIPDRCommonParameters->pExceptionStructure_); 
		free(docID);
		free(pCurrTime);
		free(pErrorCode);		
		freeIPDRCommonParameters(pIPDRCommonParameters);
		return (IPDR_ERROR);
	}

	errorSeverity = getUUID(docID, pErrorCode);
	if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, "getUUID"); 
	}
	else if (errorSeverity == IPDR_ERROR) {
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "getUUID"); 
		printExceptionStructure (pIPDRCommonParameters->pExceptionStructure_); 
		free(docID);
		free(pCurrTime);
		free(pErrorCode);		
		freeIPDRCommonParameters(pIPDRCommonParameters);
		return (IPDR_ERROR);
	}

/* IPDR v3.5, changes for vesrion*/
       /*if(setVersion(3.5) == 1)
       	printf("Changed the version\n");
		 else
       	printf("sorry no version change\n");*/

	if(schemaValFlag) {
      printf("\nSchema Validation is ON, Calling setSchemaData");
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
			printExceptionStructure (pIPDRCommonParameters->pExceptionStructure_); 
			free(docID);
			free(pCurrTime);
			free(pErrorCode);
			freeIPDRCommonParameters(pIPDRCommonParameters);
			return (IPDR_ERROR);
		}
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
		 pListServiceDefinitionURI, docID, pListSchemaNameSpace, pErrorCode);

	if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "writeHeader"); 
	}
	else if (errorSeverity == IPDR_ERROR) {
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeHeader"); 
		printExceptionStructure (pIPDRCommonParameters->pExceptionStructure_); 
		free(docID);
		free(pCurrTime);
		free(pErrorCode);
		freeIPDRCommonParameters(pIPDRCommonParameters);

		return (IPDR_ERROR);
	}

	  
	/* Print the records present in input files into the xdr file*/
	errorSeverity = printRecords(pFileRecList, maxLength,numOfFiles,
		  roundRobFlag,recordCount,pIPDRCommonParameters,
		  pListServiceDefinitionURI,pErrorCode);

	
	if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "printRecords"); 
	}
	else if (errorSeverity == IPDR_ERROR) {
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "printRecords"); 
		printExceptionStructure (pIPDRCommonParameters->pExceptionStructure_); 
		free(docID);
		free(pCurrTime);
		free(pErrorCode);
		freeIPDRCommonParameters(pIPDRCommonParameters);
		return (IPDR_ERROR);
	}

    errorSeverity = writeDocEnd(pIPDRCommonParameters, pErrorCode);
	if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "writeDocEnd"); 
	}
	else if (errorSeverity == IPDR_ERROR) {
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeDocEnd"); 
		printExceptionStructure (pIPDRCommonParameters->pExceptionStructure_); 
		free(docID);
		free(pCurrTime);
		free(pErrorCode);
		freeIPDRCommonParameters(pIPDRCommonParameters);
		return (IPDR_ERROR);
	}
	printListErrorStructure (pIPDRCommonParameters->pListErrorStructure_); 
	freeListNameSpaceInfo(&pListNameSpaceInfo);
	free(docID);
	free(pCurrTime);
	free(pErrorCode);

	freeList(&(pFileRecList));
    /*
	freeIPDRCommonParameters(pIPDRCommonParameters);
    */ 

	freeListSchemaNameSpace(&(pListSchemaNameSpace));
	freeListServiceDefinitionURI(&(pListServiceDefinitionURI));
	
	return(IPDR_OK);

}



/*!
 *  NAME:
 *      populateIPDRCommonParameters() - Function to populate 
 *                                       IPDRCommonParameters Structure
 *  DESCRIPTION:
 *				 populates IPDRCommonParameters Structure
 *  
 *  ARGUMENTS:
 *		
 *			pIPDRCommonParameters (IN/OUT)
 *          pErrorCode (IN/OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int populateIPDRCommonParameters(
		IPDRCommonParameters* pIPDRCommonParameters,
		int* pErrorCode
		)
{
	int numOfBuckets = 9;
	if(pIPDRCommonParameters == NULL) {
	    *pErrorCode = IPDR_NULL_INPUT_PARAMETER;
	    return (IPDR_ERROR);
	}

	pIPDRCommonParameters->DocState_ = NONWRITABLE;
	pIPDRCommonParameters->DocType_ = 0;
	pIPDRCommonParameters->descriptorCount_ = 1;
	pIPDRCommonParameters->pDescriptorLookupHashTable_ = 
		                      HashTableCreate(numOfBuckets);
	pIPDRCommonParameters->IPDRCount_.ipdrRecordCount_ = 1;
	pIPDRCommonParameters->pDescriptorLookupList_ = NULL;
	pIPDRCommonParameters->pListErrorStructure_ = NULL;                    
	
	return(IPDR_OK);
}


 /*!
 *  NAME:
 *      readFileContents() - Read all the input files into strings.
 *                              
 *
 *  DESCRIPTION:
 *      This method takes as input the input File list reads all the input
 *      files into strings. It reads the nameSpaceURI and the schemaURI 
 *      from each input file.
 *		. 
 *  
 *  ARGUMENTS:
 *       -	pFileList (IN/OUT)	
 *       -  pFileNamesList (IN)
 *       -  numOfFiles (IN)
 *       -  ListServiceDefinitionURI (IN/OUT)
 *       -  ListNameSpaceInfo   (IN/OUT)
 *       -  pErrorCode (IN/OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


int readFileContents(
					 List** pFileList,
					 List* pFileNamesList,
					 unsigned int numOfFiles,
					 ListServiceDefinitionURI** pListServiceDefinitionURI,
					 ListNameSpaceInfo** pListNameSpaceInfo,
					 int* pErrorCode
					 )
					 
{

	unsigned int countFileChars = 0;
	unsigned int countArguments = 0;
	unsigned int countNumFiles = 0;
	unsigned int lenFileName = 0;
	int lineFeed;
	
	char* nameSpaceURI = NULL;
	char* nSpaceURI = NULL;
	char* schemaURI = NULL;
	char* pNameSpaceID = NULL;
	char* URI = NULL;
	char* attributeNames = NULL;
	char* strFileContents = NULL;
	char* fileNames = NULL;
	char* readString = NULL;
	char* token;
	char seps[]   = ",";
	unsigned int fileLength = 0;
	List* pCurrent = NULL;

	FILE* fp;
	char* defaultNameSpace = "http://www.ipdr.org/namespaces/ipdr";
	char tempString1[1000];
	char tempString2[1000];

	if(pFileNamesList == NULL) {
	    *pErrorCode = IPDR_NULL_INPUT_PARAMETER;
	    return (IPDR_ERROR);
	}

          
	attributeNames = (char *) calloc(MAX_ATTRIBUTE_NAME * 100, sizeof(char));
	nameSpaceURI = (char *) calloc(MAX_NAMESPACE_URI, sizeof(char));
	readString = (char*) calloc(MAX_SERVICE_DEFINITION_URI, sizeof(char));
	schemaURI = (char *) calloc(MAX_SERVICE_DEFINITION_URI, sizeof(char));
	nSpaceURI = (char *) calloc(MAX_NAMESPACE_URI, sizeof(char));
	pNameSpaceID = (char *) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char));
	URI = (char *) calloc(MAX_NAMESPACE_URI, sizeof(char));


	if(	attributeNames == NULL || nameSpaceURI == NULL || readString == NULL 
		|| nSpaceURI == NULL || schemaURI == NULL || pNameSpaceID == NULL ||
		URI == NULL) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		return(IPDR_ERROR);
	}
	

	pCurrent = pFileNamesList;
	/* Read all the input files into strings */
	for (countNumFiles = 1; countNumFiles <= numOfFiles; countNumFiles++) {


		lenFileName = strlen(pCurrent->node) + 1;
		fileNames = (char *) calloc (lenFileName, sizeof(char));
		if (fileNames == NULL) {
			*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
			return(IPDR_ERROR);
		}
		

		getNode(pFileNamesList, countNumFiles, fileNames);

		fileLength = getFileLength(fileNames) + 1;

		strFileContents = (char *) calloc (fileLength, sizeof(char));
		
		if (strFileContents == NULL) {
			*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
			return(IPDR_ERROR);
		}
		

		fp = fopen(fileNames, "r");
    	/* 
		Reading ServiceDefinitionURI and NameSpaceURI from the 
		input file 
		*/
		fscanf(fp,"%s",	readString);
		strcpy(	schemaURI , strtok(readString, seps));
		
		#ifdef DEBUG_IPDR
			printf( "\nschemaURI(URI)= %s\n", schemaURI );
		#endif
		
		appendListServiceDefinitionURI(pListServiceDefinitionURI, schemaURI);

		fscanf(fp,"%s",readString);
		/* Getting the  nameSpaceURI string with comma trimmed */
		strcpy(	nameSpaceURI , strtok(readString, seps));
			

		if((strchr( nameSpaceURI, '$' ))==NULL){
			strcpy(URI, nameSpaceURI);
			strcpy(pNameSpaceID, "");
		}
		else {
			strcpy(nSpaceURI,nameSpaceURI);
		    token = strtok( nSpaceURI, "$");
		    strcpy(pNameSpaceID,token);
		    token = strtok( NULL, "$" );
			strcpy(URI, token);	
		}

			
		convertToUpper(defaultNameSpace, tempString1);
		convertToUpper(URI, tempString2);
      if(strcmp(tempString1, tempString2) == 0) {
			URI = NULL;
		   /*pListNameSpaceInfo = NULL;*/
      } else {
			appendListNameSpaceInfo(pListNameSpaceInfo, pNameSpaceID,
					URI);
		}

		/*fscanf(fp,"%s",attributeNames);*/
		fgets(attributeNames,10000,fp);
		fgets(attributeNames,10000,fp);
		/*getc(fp);*/
		/*check what the character is?
		remove comment*/
		lineFeed = getc(fp);

		if(lineFeed != 10) {
			strFileContents[0] = (char)lineFeed;
			/* Read the records from the files*/
			for (countFileChars = 1;
				(strFileContents[countFileChars] = getc(fp)) != EOF; 
				countFileChars++);
		} else {
			for (countFileChars = 0;
				(strFileContents[countFileChars] = getc(fp)) != EOF; 
				countFileChars++);
		}

		strFileContents[countFileChars]='\0';	
		appendNode(pFileList, strFileContents);	

		fclose(fp);
		free(fileNames);
		free(strFileContents);	
		pCurrent = pCurrent->pNext;
		
	}
   free(attributeNames);
/*
	free(nameSpaceURI);
	free(readString);
	free(schemaURI);
*/
	free(nSpaceURI);
	free(pNameSpaceID);
	free(URI);


	return(IPDR_OK);
}


/*!
 *  NAME:
 *      validateInputParams() - This function validates the Input 
 *					             Command Line Arguments 
 *  DESCRIPTION:
 *      This function validates the Input Command Line Arguments for this 
 *      test tool.
 *  
 *  ARGUMENTS:
 *		
 *			argCount              (IN)
 *			argValues             (IN)
 *			numOfFiles            (IN/OUT)
 *			robFlag               (IN/OUT)
 *			recCount			  (IN/OUT)
 *          pFileNamesList        (IN/OUT)
 *          pIPDRCommonParameters (IN/OUT)
 *			pErrorCode            (IN/OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


int validateInputParams(
						int argCount, 
						char* argValues[],
						unsigned int* numOfFiles,
						unsigned int* robFlag,
						unsigned int* schemaValFlag, 
                  unsigned int* versionFlag, 
                  char* ipdrVer,
						int* recCount,
						List** pFileNamesList,
						IPDRCommonParameters* pIPDRCommonParameters,	
						int* pErrorCode
					   )
{
	unsigned int countFileChars = 0;
	unsigned int countArguments = 0;
	unsigned int countValidArgs = 0;
        unsigned int lenStreamName = 0;
	unsigned int outFileFlag = 0;
	int strLength = 0;
	unsigned int lenOutFile = 0;

	char* strFile;
	char* docType;
	char* docTypeExt;
	char* fExtension;
	char* outFileName = NULL;
	char* tempString;
	char* strTemp;

	if( argValues == NULL || robFlag == NULL ||	recCount == NULL ||
		pIPDRCommonParameters == NULL  ) {
	    *pErrorCode = IPDR_NULL_INPUT_PARAMETER;
	    return (IPDR_ERROR);
	}

	strFile = (char *) calloc(MAX_REC_STRING, sizeof(char));
	docType = (char *) calloc(MAX_STR_LENGTH, sizeof(char));
	docTypeExt= (char *) calloc(MAX_STR_LENGTH, sizeof(char));
	fExtension= (char *) calloc(MAX_STR_LENGTH, sizeof(char));
	tempString = (char*) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char));
	strTemp = (char*) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char));

	if(strFile == NULL || docType == NULL || 
		docTypeExt == NULL || fExtension == NULL || 
		tempString == NULL || strTemp == NULL ) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		return(IPDR_ERROR);
	}
	
	/* Validate for commandline parameters */

	for	(countArguments = 1, *numOfFiles = 0, countValidArgs = 1;
			(countArguments < (unsigned int)(argCount));
			 countArguments++) {
			/*Catches the commandline parameter -roundrobin*/
			convertToUpper(argValues[countArguments], tempString);

			if ((strcmp(tempString,	"-SCHEMAVALIDATION")) == 0) {
				*schemaValFlag = 1;
				countValidArgs++;
			}

			if ((strcmp(tempString,	"-ROUNDROBIN"))==0){
				*robFlag = 1;
				countValidArgs++;
			}
			/*Catches the commandline parameter -doctype Also reads the 
			value for doctype from the next commandline parameter*/
			if ((strcmp(tempString,"-DOCTYPE")) == 0){
				if (!(countArguments == (unsigned int)(argCount-1))){
					strcpy(docType,argValues[countArguments+1]);
					
					convertToUpper(docType, strTemp);
					if (((strcmp(strTemp,IPDR_XDR)) == 0))
					{
						pIPDRCommonParameters->DocType_= XDR;
						countValidArgs+=2;
						countArguments++;
					}
					else if  (((strcmp(strTemp,IPDR_XML))==0)) 
					{
						pIPDRCommonParameters->DocType_ = XML;
						countValidArgs+=2;
						countArguments++;
					}
				}
			
			}
				/*Catches the commandline parameter -outfile. Also reads 
				the value for outfile from the next commandline parameter*/
			if ((strcmp(tempString,"-OUTFILE")) == 0){
			
				if(!(countArguments==(unsigned int)(argCount-1))) {
					if(argValues[countArguments+1][0]=='-') {
						*pErrorCode = IPDR_MISSING_OR_INVALID_OUTFILE;
						return(IPDR_ERROR);
					} 
					else {
						lenOutFile = strlen(argValues[countArguments+1]) + 1;
						outFileName = (char *) calloc (lenOutFile, sizeof(char));
						if (outFileName == NULL) {
							*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
							return(IPDR_ERROR);
						}
						strcpy(outFileName,argValues[countArguments+1]);
						/* Sets the outFileFlag if the file parameter is 
						present on the comandline*/
						outFileFlag = 1;
						countValidArgs += 2;
						countArguments++;
					}

			}
		}

		/*Catches the commandline parameter -ipdrversion*/
		if ((strcmp(tempString,	"-IPDRVERSION")) == 0) {
			*versionFlag = 1;

			if(!(countArguments==(unsigned int)(argCount-1))) {
				if(argValues[countArguments+1][0]=='-') {
					*pErrorCode = IPDR_MISSING_OR_INVALID_OUTFILE;
						printf("IPDR_MISSING_OR_INVALID_OUTFILE");
					return(IPDR_ERROR);
				} 
				else {
					strcpy(ipdrVer,argValues[countArguments+1]);
					countValidArgs += 2;
					countArguments++;
				}
			}
		}

		/*Catches the commandline parameter -recordcount and reads its 
			value*/
	if ((strcmp(tempString,"-RECORDCOUNT"))==0) {
				if(!(countArguments==(unsigned int)(argCount-1))) {
					countFileChars=0;
					while (argValues[countArguments+1][countFileChars]!='\0') {
						if(!isdigit(argValues[countArguments+1][countFileChars])){
								printf("\nIPDR_INVALID_RECORDCOUNT\n");
							*pErrorCode = IPDR_INVALID_RECORDCOUNT;
							return(IPDR_ERROR);
						}
						countFileChars++;
					}
					*recCount=atoi(argValues[countArguments+1]); 
						
					if (*recCount<0) {
							printf("\nIPDR_INVALID_RECORDCOUNT\n");
						*pErrorCode = IPDR_INVALID_RECORDCOUNT;
						return(IPDR_ERROR);
					} else 	{
							countValidArgs+=2;
							countArguments++;
					}
				}
			}

	
		/*Catches the input files*/
				if(fopen(argValues[countArguments],"r") != NULL) {
	
					char* token;
					char* fileName;
					char seps[]   = ".";
					char* str;
					
					unsigned int fileLength = 0;
					fileLength = strlen(argValues[countArguments]) + 1;
					
					str = (char *) calloc (fileLength, sizeof(char));
					fileName = (char *) calloc (fileLength, sizeof(char));
					
			

					if(fileName == NULL || str == NULL ) {
					*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
					return(IPDR_ERROR);
					}

					strcpy(str,argValues[countArguments]);

					token = strtok( str, seps );

					while( token != NULL ) {
					  strcpy(fileName,token);
					  token = strtok( NULL, seps );

					}
					convertToUpper(fileName, strTemp);

					if ((strcmp(strTemp,"CSV")==0)) {
					
					countValidArgs++;
					appendNode(pFileNamesList, argValues[countArguments]);
					(*numOfFiles)++;
				   }
				free(fileName);
				free(str);
			}
		



}


    /* Checks if number of valid input parameters is same as the number of 
     input parameters*/

	if ((countValidArgs <= (unsigned int) (argCount-1))||(argCount == 1)) {
			printf("\nIPDR_MISSING_OR_INVALID_COMMANDLINE_PARAMS");
		printUsage();
		*pErrorCode = IPDR_MISSING_OR_INVALID_COMMANDLINE_PARAMS;
		return(IPDR_ERROR);
	}

   /* Appends .xdr or .xml to the input file if it does not have .xdr or
   .xml extension*/
	if (outFileFlag==1)	{
		
			convertToUpper(docType, strTemp);
			sprintf(docTypeExt,"%c%s",'.', strTemp);
		
			subString(outFileName,fExtension,strlen(outFileName)-4,4);
			convertToUpper(fExtension, strTemp);
			if (!(strcmp(strTemp,docTypeExt)==0)) {

			strcat(outFileName,"."); 
			strcat(outFileName,docType);
		             }
                        if (outFileName != NULL) {
                        lenStreamName = strlen(outFileName) + 1;
                                  } else {
                         lenStreamName = strlen(IPDR_NULL) + 1;
                                  }
                     pIPDRCommonParameters->pStreamName_ = (char *) calloc
                                              (lenStreamName, sizeof(char));
                     if (outFileName != NULL) {
                     strcpy(pIPDRCommonParameters->pStreamName_, outFileName);
                     } else {
                     strcpy(pIPDRCommonParameters->pStreamName_, IPDR_NULL);
                     }

	
		
			if ((pIPDRCommonParameters->pStreamHandle_ = 
			         fopen(pIPDRCommonParameters->pStreamName_, "wb+")) 
                                      == NULL) {
			*pErrorCode = (IPDR_INVALID_FILE);
			return(IPDR_ERROR);
		
	} 


}
	free(strTemp);
	free(tempString);
	free(docTypeExt);
	if (outFileName != NULL)
		free(outFileName);
    free(strFile);
	free(docType);
	free(fExtension);
 
	return(IPDR_OK);
}




/*!
 *  NAME:
 *      findMaxLength() - Find the length of the longest 
 *						node of the linked list
 *  DESCRIPTION:
 *				  In a linked list it find the finds the length 
 *				  of the longest string i.e. the file with the greatest 
 *                length.
 *  
 *  ARGUMENTS:
 *		
 *			pFileList (IN)
 *			numFiles  (IN)
 *			maxValue  (IN/OUT)
 *			pErrorCode (IN/OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


int findMaxLength(
				  List* pFileList, 
				  unsigned int numFiles, 
				  unsigned int* maxValue,
				  int* pErrorCode
				  )
{
	unsigned int nodeLength = 0;
	unsigned int countFiles = 0;
	unsigned int maxrecords = 0;
	unsigned int rec = 0;
	unsigned int count = 0;
	int fileLength = 0;
	char *strFileContents = NULL;
	List* pCurrent = NULL;

	if( pFileList == NULL || maxValue == NULL ) {
	    *pErrorCode = IPDR_NULL_INPUT_PARAMETER;
	    return (IPDR_ERROR);
	}

	pCurrent = pFileList;
	for (countFiles = 0; (countFiles < numFiles); countFiles++) {

			
		fileLength = strlen(pCurrent->node) + 1;
		strFileContents = (char *) calloc (fileLength, sizeof(char));
			
		if (strFileContents == NULL) {
			*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
			return(IPDR_ERROR);
		}
		
		getNode(pFileList, countFiles+1, strFileContents);
		nodeLength = strlen(strFileContents);
		

		for (count = 0; count <= (strlen(strFileContents)); count++) {
			if((strFileContents[count] == '\n') ||
				(strFileContents[count] == '\0')) {
				rec++;
			}
		}

		maxrecords = rec - 1;
	
		if (maxrecords > (*maxValue)) {
			*maxValue = maxrecords;
		}
	
	    maxrecords = 0;
		rec = 0;

		free(strFileContents);
		pCurrent = pCurrent->pNext;
	}
	


	return(IPDR_OK);
}


/*!
 *  NAME:
 *      printRecords() - Contains Round Robin & Load Functions. 
 *								
 *  DESCRIPTION:
 *				  Function containing operations for RoundRobin 
 *				  and Load
 *  
 *  ARGUMENTS:
 *		
 *			pFileContentsList		 (IN)
 *			maxFileLength			 (IN)
 *			numOfFiles				 (IN)
 *			robFlag					 (IN)  
 *			recordCount				 (IN) 
 *          pIPDRCommonParameters    (IN)
 *			pListServiceDefinitionURI(IN)
 *          pErrorCode				 (IN/OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


int printRecords(
				 List* pFileContentsList, 
				 unsigned int maxFileLength, 
				 unsigned int numOfFiles,
				 unsigned int robFlag,
				 unsigned int recordCount,
				 IPDRCommonParameters* pIPDRCommonParameters,
				 ListServiceDefinitionURI* pListServiceDefinitionURI,
				 int* pErrorCode
				 )
{
	int errorSeverity = 0;

	if( pFileContentsList == NULL || pIPDRCommonParameters == NULL
		|| pListServiceDefinitionURI == NULL) {
	    *pErrorCode = IPDR_NULL_INPUT_PARAMETER;
	    return (IPDR_ERROR);
	}
		
	if((robFlag)&&(recordCount)) {

	errorSeverity = loadAndRoundRobinProcessing(pFileContentsList, 
		maxFileLength, 
		numOfFiles,recordCount,pIPDRCommonParameters,
		pListServiceDefinitionURI,
		pErrorCode);
                                                                            
	if (errorSeverity == IPDR_WARNING) { 
		errorHandler(pIPDRCommonParameters, pErrorCode, 
		   "loadAndRoundRobinProcessing"); 
	  }
	 else if (errorSeverity == IPDR_ERROR) {
		exceptionHandler(pIPDRCommonParameters, pErrorCode, 
		   "loadAndRoundRobinProcessing"); 
			printExceptionStructure (pIPDRCommonParameters->pExceptionStructure_); 
		return (IPDR_ERROR);
	 }
 
	
	} else if ((robFlag)&&(!(recordCount))){
		errorSeverity = roundRobinProcessing(pFileContentsList, 
			maxFileLength, numOfFiles, pIPDRCommonParameters, 
			pListServiceDefinitionURI,pErrorCode);

	  if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, 
		   "roundRobinProcessing"); 
	  }
	  else if (errorSeverity == IPDR_ERROR) {
		exceptionHandler(pIPDRCommonParameters, pErrorCode, 
		   "roundRobinProcessing"); 
			printExceptionStructure (pIPDRCommonParameters->pExceptionStructure_); 
		return (IPDR_ERROR);
	 }
 
	} else if (!(robFlag)&&(recordCount)) {
	 errorSeverity = loadProcessing(pFileContentsList, numOfFiles, 
		 recordCount, pIPDRCommonParameters,pListServiceDefinitionURI,
		 pErrorCode);
	  if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, "loadProcessing"); 
	  }
	  else if (errorSeverity == IPDR_ERROR) {
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "loadProcessing");
			printExceptionStructure (pIPDRCommonParameters->pExceptionStructure_); 
		return (IPDR_ERROR);
	 }
	} else if(!(robFlag)&&(!(recordCount))) {
	 errorSeverity = simpleProcessing(pFileContentsList,numOfFiles,
                 pIPDRCommonParameters, pListServiceDefinitionURI, 
				 pErrorCode);
	if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, "simpleProcessing"); 
	}
	else if (errorSeverity == IPDR_ERROR) {
	   exceptionHandler(pIPDRCommonParameters, pErrorCode, "simpleProcessing"); 
			printExceptionStructure (pIPDRCommonParameters->pExceptionStructure_); 
	   return (IPDR_ERROR);
	 }
	}

	return(IPDR_OK);
}

/*!
 *  NAME:
 *      loadAndRoundRobinProcessing() -  Will read records from all the 
 *                                  file strings in a roundrobin fashion               
 *                                          
 *
 *  DESCRIPTION:
 *		Conditon for RoundRobin and Load -will read one record from each 
 *      file string in a roundrobin fashion untill the end of each string 
 *      or untill the number of records mentioned in load are done 
 *      (whichever is greater) - if the records in a file string are over 
 *      it will wait for the records in all the file strings to be over for
 *      one pass through all the file strings before starting from the 
 *      beginning.
 *		. 
 *  
 *  ARGUMENTS:
 *       -	pFileContentsList		(IN)
 *       -  maxFileLength           (IN)
 *       -  numOfFiles				(IN)
 *       -  recordCount             (IN)
 *       -  pIPDRCommonParameters	(IN)
 *       -  pListServiceDefinitionURI(IN)
 *       -  pErrorCode				(IN/OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
	



int loadAndRoundRobinProcessing(
								List* pFileContentsList, 
								unsigned int maxFileLength, 
								unsigned int numOfFiles,
								unsigned int recordCount,
								IPDRCommonParameters* pIPDRCommonParameters,
								ListServiceDefinitionURI* pListServiceDefinitionURI,
								int* pErrorCode
								)
{

	unsigned int countFileChars = 0;
	unsigned int countLineChars = 0;
	unsigned int countRecords = 0;
	unsigned int countFile = 0;
	unsigned int maxFileLengthFlag = 0;
	int errorSeverity = 0;
	int fileLength = 0;
	
	char* strFile;
	char* strFileContents = NULL;
	int* position;
	unsigned int* count;
	FNFType** pFNFType;
	List* pCurrentFile = NULL;
	ListServiceDefinitionURI* pCurrent = NULL;
	ListNameSpaceInfo** pListNameSpaceInfo;

	if(pFileContentsList == NULL || pIPDRCommonParameters == NULL ||
	 pListServiceDefinitionURI == NULL) {
	    *pErrorCode = IPDR_NULL_INPUT_PARAMETER;
	    return (IPDR_ERROR);
	}

	strFile = (char *) calloc (MAX_REC_STRING, sizeof(char));
	position = (int *) calloc (MAX_STR_LENGTH, sizeof(int));
	count = (unsigned int *) calloc (MAX_ATTRIBUTE_LENGTH, sizeof(unsigned int));
	pFNFType = (FNFType**) calloc (numOfFiles, sizeof(FNFType*));
	pListNameSpaceInfo = (ListNameSpaceInfo**) 
		calloc (numOfFiles, sizeof(ListNameSpaceInfo*));

	if(strFile == NULL || position == NULL || count == NULL || 
		pFNFType == NULL || pListNameSpaceInfo == NULL) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		return(IPDR_ERROR);
	}
	
	/* Getting the FNfType and ListNameSpaceInfo for each 
	 ServiceDefintionURI*/
	pCurrent = pListServiceDefinitionURI;

	for (countFile = 1; (countFile <= numOfFiles); countFile++) {
		
		pFNFType[countFile] = newFNFType();
		pListNameSpaceInfo[countFile] = NULL;
		
		errorSeverity = 
			generateSchemaDescriptor(pCurrent->pServiceDefintionURI_,         
		  pFNFType[countFile], &pListNameSpaceInfo[countFile], pErrorCode);

		if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "generateSchemaDescriptor"); 
			printListErrorStructure(pIPDRCommonParameters->pListErrorStructure_);  
		}
		 else if (errorSeverity == IPDR_ERROR) {
			 exceptionHandler(pIPDRCommonParameters, pErrorCode, "generateSchemaDescriptor"); 
				printExceptionStructure (pIPDRCommonParameters->pExceptionStructure_); 
			 return (IPDR_ERROR);
		 }


		pCurrent = pCurrent->pNext_;
	}

	/* Reading from each file one record at a time till all the file 
	strings are  done and then starting from first file again uptill the 
	recordcount*/


	countRecords = 0;
	
	while(countRecords<recordCount)	
	{
	
		while((maxFileLengthFlag != 1) )
		{

		pCurrentFile = pFileContentsList;
		for (countFile=1; (countFile <= numOfFiles); countFile++) 
		{
			
			fileLength = strlen(pCurrentFile->node) + 1;
			strFileContents = (char *) calloc (fileLength, sizeof(char));
			
			if (strFileContents == NULL) {
				*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
				return(IPDR_ERROR);
			}

		    countFileChars = position[countFile];
			getNode(pFileContentsList, countFile, strFileContents);
		
			 if ((strFileContents[countFileChars]!='\0')) {
	
				for (countLineChars = 0; 
				    (strFileContents[countFileChars]!='\n') && 
					(strFileContents[countFileChars]!='\0');
				    countFileChars++, countLineChars++) {
					strFile[countLineChars] = strFileContents[countFileChars];	
				}
					
				strFile[countLineChars]='\0';
				#ifdef DEBUG_IPDR 
					printf("\nstrFile=%s",strFile);
				#endif
			
				if(countRecords < recordCount ) {
				errorSeverity = writeRecords(strFile,pIPDRCommonParameters,
					pFNFType[countFile], pListNameSpaceInfo[countFile],
					pErrorCode);
					printf("\t[%d]", countRecords);
					countRecords++;	 
				}

				count[countFile]++;
				
				if (errorSeverity == IPDR_WARNING) {
				  errorHandler(pIPDRCommonParameters, pErrorCode, "writeRecords"); 
				}
				else if (errorSeverity == IPDR_ERROR) {
				  exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeRecords"); 
				   printExceptionStructure (pIPDRCommonParameters->pExceptionStructure_); 
				  return (IPDR_ERROR);
				 }

				if(strFileContents[countFileChars] != '\0') {
				countFileChars++;
				}
				position[countFile]=countFileChars;

				if((count[countFile] == maxFileLength)) {
					maxFileLengthFlag=1;
				}	
		
			}
				free(strFileContents);
				pCurrentFile = pCurrentFile->pNext;
			}
		}
		
			if (maxFileLengthFlag == 1) {
				for (countFile = 1; (countFile <= numOfFiles); countFile++) {
					position[countFile]=0;
					count[countFile] = 0;
				}
				maxFileLengthFlag=0; 
			}
	
		
	}

	for (countFile = 1; (countFile <= numOfFiles); countFile++) {
/*
			freeFNFType(pFNFType[countFile]);
*/
			freeListNameSpaceInfo(&pListNameSpaceInfo[countFile]);
				
	}
	free(strFile);
	return(IPDR_OK);
}
	

/*!
 *  NAME:
 *      roundRobinProcessing() -  Will read records from all the file 
 *									  strings in a roundrobin fashion
 *                              
 *
 *  DESCRIPTION:
 *    Will read records from all the file strings in a roundrobin fashion
 *    until the end of each file string i.e. the last record in each file.
 *		. 
 *  ARGUMENTS:
 *       -	pFileContentsList		(IN)	
 *       -  maxFileLength			(IN)
 *       -  numOfFiles				(IN)
 *       -  pIPDRCommonParameters	(IN)
 *       -  pListServiceDefinitionURI(IN)
 *       -  pErrorCode               (IN/OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
	

	
int roundRobinProcessing (
						List* pFileContentsList, 
						unsigned int maxFileLength, 
						unsigned int numOfFiles,
						IPDRCommonParameters* pIPDRCommonParameters,
						ListServiceDefinitionURI* pListServiceDefinitionURI,
						int* pErrorCode
						)

{
	unsigned int countFileChars = 0;
	unsigned int countLineChars = 0;
	unsigned int countFile = 0;
	unsigned int maxFileLengthFlag = 0;
	unsigned int fileFlag = 0;
	int errorSeverity = 0;
	int fileLength = 0;
	int countRec = 0;
	
	char* strFile;
	char* strFileContents = NULL;
	int* position;
	unsigned int* count;

	FNFType** pFNFType;
	List* pCurrentFile = NULL;
	ListServiceDefinitionURI* pCurrent = NULL;
	ListNameSpaceInfo** pListNameSpaceInfo;

	if(pFileContentsList == NULL || pIPDRCommonParameters == NULL ||
	 pListServiceDefinitionURI == NULL) {
	    *pErrorCode = IPDR_NULL_INPUT_PARAMETER;
	    return (IPDR_ERROR);
	}

	strFile = (char *) calloc(MAX_REC_STRING, sizeof(char));
	position = (int *) calloc(MAX_STR_LENGTH, sizeof(int));
	pFNFType = (FNFType**) calloc (numOfFiles, sizeof(FNFType*));
	count = (unsigned int *) calloc (MAX_ATTRIBUTE_LENGTH, sizeof(unsigned int));
	
	pListNameSpaceInfo = (ListNameSpaceInfo**) 
		calloc (numOfFiles, sizeof(ListNameSpaceInfo*));

	if(strFile == NULL || position == NULL  || count == NULL ||
		pFNFType == NULL || pListNameSpaceInfo == NULL) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		return(IPDR_ERROR);
	}
		
	/* Getting the FNfType and ListNameSpaceInfo for each 
	 ServiceDefintionURI*/
	pCurrent = pListServiceDefinitionURI;
	for (countFile = 1; (countFile <= numOfFiles); countFile++) {
		
			pFNFType[countFile] = newFNFType();
			pListNameSpaceInfo[countFile] = NULL;

			errorSeverity = 
				generateSchemaDescriptor(pCurrent->pServiceDefintionURI_, 
				pFNFType[countFile], &pListNameSpaceInfo[countFile], 
				pErrorCode);

			if (errorSeverity == IPDR_WARNING) {
			 errorHandler(pIPDRCommonParameters, pErrorCode, "generateSchemaDescriptor"); 
			 }
			else if (errorSeverity == IPDR_ERROR) {
			   exceptionHandler(pIPDRCommonParameters, pErrorCode, 
				   "generateSchemaDescriptor"); 
			  return (IPDR_ERROR);
			 }
			pCurrent = pCurrent->pNext_;
	}

	/* Reading from each file one record at a time till the total number 
	of file strings are  done*/


	while( maxFileLengthFlag != 1)	{
	
		pCurrentFile = pFileContentsList;
		for (countFile = 1; ( countFile <= numOfFiles ); 
		countFile++) {

			fileLength = strlen(pCurrentFile->node) + 1;
			strFileContents = (char *) calloc (fileLength, sizeof(char));
			
			if (strFileContents == NULL) {
				*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
				return(IPDR_ERROR);
			}
		
	   		countFileChars = position[countFile];
			getNode( pFileContentsList, countFile, strFileContents);
			if(strFileContents[countFileChars]!='\0') {

					count[countFile]++;
				for (countLineChars = 0; 
					(strFileContents[countFileChars]!='\n') &&
						(strFileContents[countFileChars] != '\0');
					countFileChars++,countLineChars++) {
					strFile[countLineChars] = 
						strFileContents[countFileChars];
				}
						
				strFile[countLineChars]='\0';
				
				#ifdef DEBUG_IPDR 
					printf("\nstrFile=%s",strFile);
				#endif

				errorSeverity = writeRecords(strFile,pIPDRCommonParameters,
					pFNFType[countFile], pListNameSpaceInfo[countFile],
				  pErrorCode);
				if (errorSeverity == IPDR_WARNING) {
				  errorHandler(pIPDRCommonParameters, pErrorCode, 
					  "writeRecords"); 
				}
				else if (errorSeverity == IPDR_ERROR) {
				  exceptionHandler(pIPDRCommonParameters, pErrorCode, 
					  "writeRecords"); 
				  return (IPDR_ERROR);
				 }
				printf("\t[%d]", countRec);
				countRec++;

				if(strFileContents[countFileChars] != '\0') {
				countFileChars++;
				}
				position[countFile] = countFileChars;

				if((count[countFile] == maxFileLength)) {
				maxFileLengthFlag=1;
				}
	
			}
			free(strFileContents);
			pCurrentFile = pCurrentFile->pNext;
		
		}

	
	}

	for (countFile = 1; (countFile <= numOfFiles); countFile++) {
/*
		freeFNFType(pFNFType[countFile]);
*/
		freeListNameSpaceInfo(&pListNameSpaceInfo[countFile]);
			
	}
	free(strFile);
	return(IPDR_OK);
}
		
/*!
 *  NAME:
 *      loadProcessing() -  Reads records from one file string at a time 
 *                         till the end of each string or untill the number in
 *                          recordcount
 *                              
 *
 *  DESCRIPTION:
 *    Will read records from one file string at a time till the end of
 *    each string or untill the number of records mentioned in recordcount 
 *    are over whichever is greater
 *		. 
 *  
 *  ARGUMENTS:
 *       -	pFileContentsList		(IN)	
 *       -  numOfFiles				(IN)
 *       -  recordCount             (IN)
 *       -  pIPDRCommonParameters	(IN)
 *       -  pListServiceDefinitionURI(IN)
 *       -  pErrorCode				(IN/OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
	


int loadProcessing(
				List* pFileContentsList, 
				unsigned int numOfFiles,
				unsigned int recordCount,
				IPDRCommonParameters* pIPDRCommonParameters,
				ListServiceDefinitionURI* pListServiceDefinitionURI,
				int* pErrorCode
				)

{
	unsigned int countFileChars = 0;
	unsigned int countRecords=0;
	unsigned int countLineChars=0;
	unsigned int countFile =0;
	int errorSeverity = 0;
	int fileLength = 0;
	
	char* strFile;
	char* strFileContents = NULL;
	List* pCurrentFile = NULL;
	ListServiceDefinitionURI* pCurrent = NULL;
	FNFType** pFNFType;
	ListNameSpaceInfo** pListNameSpaceInfo;


	if(pFileContentsList == NULL || pIPDRCommonParameters == NULL ||
	 pListServiceDefinitionURI == NULL) {
	    *pErrorCode = IPDR_NULL_INPUT_PARAMETER;
	    return (IPDR_ERROR);
	}

	strFile = (char *) calloc(MAX_REC_STRING, sizeof(char));
	pFNFType = (FNFType**) calloc (numOfFiles, sizeof(FNFType*));	
	pListNameSpaceInfo = (ListNameSpaceInfo**) 
		calloc (numOfFiles, sizeof(ListNameSpaceInfo*));


	if(strFile == NULL || pFNFType == NULL || pListNameSpaceInfo == NULL) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		return(IPDR_ERROR);
	}


	pCurrent = pListServiceDefinitionURI;
	for (countFile = 1; (countFile <= numOfFiles); countFile++) {
		
			pFNFType[countFile] = newFNFType();
			pListNameSpaceInfo[countFile] = NULL;

			errorSeverity = 
				generateSchemaDescriptor(pCurrent->pServiceDefintionURI_, 
				pFNFType[countFile], &pListNameSpaceInfo[countFile], 
				pErrorCode);

			if (errorSeverity == IPDR_WARNING) {
			 errorHandler(pIPDRCommonParameters, pErrorCode, "generateSchemaDescriptor"); 
			 }
			else if (errorSeverity == IPDR_ERROR) {
			   exceptionHandler(pIPDRCommonParameters, pErrorCode, 
				   "generateSchemaDescriptor"); 
			  return (IPDR_ERROR);
			 }
			pCurrent = pCurrent->pNext_;
	}

	/* Reading from each file at a time till the total number of file 
	strings are  done and then starting from first file again uptill 
	recordcount*/


	while (countRecords<recordCount) {
			pCurrent = pListServiceDefinitionURI;

		/* Getting the FNfType and ListNameSpaceInfo for each 
			ServiceDefintionURI*/
		pCurrentFile = pFileContentsList;
		for (countFile = 1;(countFile <= numOfFiles); countFile++) {

			fileLength = strlen(pCurrentFile->node) + 1;
			strFileContents = (char *) calloc (fileLength, sizeof(char));
			
			if (strFileContents == NULL) {
				*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
				return(IPDR_ERROR);
			}

			countFileChars = 0;
			getNode(pFileContentsList, countFile, strFileContents);
		
			while ((strFileContents[countFileChars]!='\0') && 
				(countRecords<recordCount)) {
						for (countLineChars = 0;
						(strFileContents[countFileChars] != '\n') &&
							(strFileContents[countFileChars] != '\0');
						countFileChars++, countLineChars++) {
						strFile[countLineChars] = 
							strFileContents[countFileChars];
					}
					strFile[countLineChars]='\0';
					
					#ifdef DEBUG_IPDR 
						printf("\nstrFile=%s",strFile);
					#endif

					errorSeverity = writeRecords(strFile,
						pIPDRCommonParameters, pFNFType[countFile],
						pListNameSpaceInfo[countFile], pErrorCode);
				
					 if (errorSeverity == IPDR_WARNING) {
					   errorHandler(pIPDRCommonParameters, pErrorCode, 
						   "writeRecords"); 
					 }
					 else if (errorSeverity == IPDR_ERROR) {
					   exceptionHandler(pIPDRCommonParameters, pErrorCode, 
						   "writeRecords"); 
					   return (IPDR_ERROR);
					 }
 

					fflush(pIPDRCommonParameters->pStreamHandle_);

					countFileChars++;
					printf("\t[%d]", countRecords);
					countRecords++;
				
			}
			
		     pCurrent = pCurrent->pNext_;
			 free(strFileContents);
			 pCurrentFile = pCurrentFile->pNext;
		}

	}
	

	for (countFile = 1; (countFile <= numOfFiles); countFile++) {
                /* 
		freeFNFType(pFNFType[countFile]);
                */
		freeListNameSpaceInfo(&pListNameSpaceInfo[countFile]);			
	}

	free(strFile);
	return(IPDR_OK);
}



/*!
 *  NAME:
 *      simpleProcessing() -  Will read records from one file string at a 
 *                           time
 *                              
 *
 *  DESCRIPTION:
 *    Reads records from one file string at a time till the end of
 *     each string (i.e. till the last record in each file)
 *		. 
 *  
 *  ARGUMENTS:
 *       -	pFileContentsList		(IN)	
 *       -  numOfFiles				(IN)
 *       -  pIPDRCommonParameters	(IN)
 *       -  pListServiceDefinitionURI(IN)
 *       -  pErrorCode				(IN/OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


int simpleProcessing(
					List* pFileContentsList, 
					int numOfFiles, 
					IPDRCommonParameters* pIPDRCommonParameters,
					ListServiceDefinitionURI* pListServiceDefinitionURI,
	  				int* pErrorCode
					)
{	
	unsigned int countFileChars = 0;
	unsigned int countLineChars = 0;
	int countFile = 0;
	int errorSeverity = 0;
	int fileLength = 0;
	int countRec = 0;

	char* strFile;
	char* strFileContents = NULL;
	FNFType** pFNFType;
	List* pCurrentFile = NULL;
	ListServiceDefinitionURI* pCurrent = NULL;
	ListNameSpaceInfo** pListNameSpaceInfo;

	if(pFileContentsList == NULL || pIPDRCommonParameters == NULL ||
	 pListServiceDefinitionURI == NULL) {
	    *pErrorCode = IPDR_NULL_INPUT_PARAMETER;
	    return (IPDR_ERROR);
	}

	strFile = (char *) calloc(MAX_REC_STRING, sizeof(char));
	pFNFType = (FNFType**) calloc (numOfFiles + 1, sizeof(FNFType*));
	pListNameSpaceInfo = (ListNameSpaceInfo**) 
		calloc (numOfFiles, sizeof(ListNameSpaceInfo*));

	if(strFile == NULL || pFNFType == NULL
		|| pListNameSpaceInfo == NULL) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		return(IPDR_ERROR);
	}


	/* Getting the FNfType and ListNameSpaceInfo for each 
	ServiceDefintionURI*/
	pCurrent = pListServiceDefinitionURI;
	pCurrentFile = pFileContentsList;

	for (countFile = 1; countFile <= numOfFiles; 
	countFile++) {

		fileLength = strlen(pCurrentFile->node) + 1;
			strFileContents = (char *) calloc (fileLength, sizeof(char));
			
		if (strFileContents == NULL) {
			*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
			return(IPDR_ERROR);
		}


	 	pFNFType[countFile] = newFNFType();
		pListNameSpaceInfo[countFile] = NULL;

	    errorSeverity =  
		  generateSchemaDescriptor(pCurrent->pServiceDefintionURI_,  
		  pFNFType[countFile], &pListNameSpaceInfo[countFile],  pErrorCode);


	  if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, "generateSchemaDescriptor"); 
		}
	  else if (errorSeverity == IPDR_ERROR) {
	   exceptionHandler(pIPDRCommonParameters, pErrorCode, "generateSchemaDescriptor"); 
	  return (IPDR_ERROR);
	
	 }
     /* Reading from each file at a time till the total number of file 
	  strings are  done*/
		countFileChars = 0;
		getNode(pFileContentsList, countFile, strFileContents);
		/*Reading each file string till the end*/
		while (strFileContents[countFileChars]!='\0')	{
			for	(countLineChars = 0;
				(strFileContents[countFileChars] != '\n') &&
					(strFileContents[countFileChars] != '\0');
				countFileChars++, countLineChars++) {
				strFile[countLineChars] = strFileContents[countFileChars];
			}
                                                                           
			strFile[countLineChars]='\0';
			#ifdef DEBUG_IPDR 
				printf("\nstrFile=%s",strFile);
			#endif

			#ifdef DEBUG_IPDR 
			{
			printf("\n\nPrinting List Name Space Info");
			printListNameSpaceInfo(pListNameSpaceInfo[countFile]);
			}
			#endif

			errorSeverity = writeRecords(strFile,pIPDRCommonParameters,
				pFNFType[countFile],pListNameSpaceInfo[countFile], 
				pErrorCode);
 
			 if (errorSeverity == IPDR_WARNING) {
			   errorHandler(pIPDRCommonParameters, pErrorCode, "writeRecords"); 
			 }
			 else if (errorSeverity == IPDR_ERROR) {
			   exceptionHandler(pIPDRCommonParameters, pErrorCode, 
				   "writeRecords"); 
			   return (IPDR_ERROR);
			 }
			printf("\t[%d]", countRec);
			countRec++;


			countFileChars++;
		}
		
	 pCurrent = pCurrent->pNext_;
	 free(strFileContents);
	 pCurrentFile = pCurrentFile->pNext;
	}


	for (countFile = 1; (countFile <= numOfFiles); countFile++) {
		freeFNFType(pFNFType[countFile]);
		freeListNameSpaceInfo(&pListNameSpaceInfo[countFile]);			
	}

	free(strFile);
	return(IPDR_OK); 
}
 
		

/*!
 *  NAME:
 *      writeRecords() - Contains operations for writing each record.
 *								
 *  DESCRIPTION:
 *				 Function containing operations for making FNFData from 
 *               the input records and passin tit to writeIPDR to write 
 *               the data in output xdr/xml file.
 *
 *  
 *  ARGUMENTS:
 *		
 *			strTemp                (IN)
 *			filePtr                (IN)
 *          pIPDRCommonParameters  (IN)
 *          SchemaURI              (IN)
 *          pErrorCode			   (IN/OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


int writeRecords(
				 char* strTemp, 
				 IPDRCommonParameters* pIPDRCommonParameters,
				 FNFType* pFNFType,
				 ListNameSpaceInfo* pListNameSpaceInfo,
			     int* pErrorCode
			  )
{
	int strIndex = 0;
	int index1 = 0;
	int count = 0;
	int countFileChars = 0 ;
	int countListNameSpaceInfo = 0;
	int errorSeverity = 0;
	int count2 = 0;
	int len2 = 0;
	int flag2 = 0;
	int lFoundFlag = 0;
	char* token = NULL;
	char* strRead = NULL;
	char* strReadTemp = NULL;
	char* strArrayElemnts = NULL;
	char* tempString = NULL;
	char* tempString1 = NULL;
	char* tempComplexType;
	char* stopstring = NULL;
	char* pNameSpcID = NULL;
	char* pNameSpaceID = NULL;
	char* pNameSpaceURI = NULL;
	char* pAttributeName = NULL;
	char* strAttributeName = NULL;
	char seps[]   = "$";

   static int i;
   static int lFlag = 0;
   int j = 0;
   int k = 0;
   int validToken = 1;
	int isPresentFlag = 0;
   char tempBuff[MAX_LEN];
   char* tempStr = NULL;
   char* delimIndex = NULL;
	char* serviceTypeToken = NULL;

	FNFData* pFNFData;
	FNFType* pTempFNFType = NULL;
	ListIPDRData* pListIPDRData = NULL;
	ListIPDRComplex* pListIPDRComplex = NULL;
	IPDRData* IPDRData = NULL;
	ListAttributeDescriptor* pCurrentListAttrDesc = NULL;
	ListAttributeDescriptor* pCurrentListServiceAttrDesc = NULL;
	ListNameSpaceInfo* pCurrentListNameSpaceInfo = NULL;
	ListAttributeDescriptor* pCurrent = NULL;
	ListAttributeDescriptor* pTempFNFTypeCurrent = NULL;
	ListIPDRComplexData* pListIPDRComplexData[MAX_IPDR_STRING];
   void* IPDRValue =  NULL;

   tempStr = (char*)malloc(MAX_LEN);
   memset(&pListIPDRComplexData, 0, sizeof(pListIPDRComplexData));

	/*
	ListAttributeDescriptor* pFirstAttrDesc = NULL;
	ListAttributeDescriptor* pFirstListAttributeDescriptor = NULL;
	*/

	if(strTemp == NULL || pIPDRCommonParameters == NULL ||
	 pFNFType == NULL) {
	    *pErrorCode = IPDR_NULL_INPUT_PARAMETER;
	    return (IPDR_ERROR);
	}


	strRead = (char *) calloc(MAX_IPDR_STRING, sizeof(char));
	strReadTemp = (char *) calloc(MAX_IPDR_STRING, sizeof(char));
	strArrayElemnts = (char *) calloc(MAX_IPDR_STRING, sizeof(char));
	tempString = (char *) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char));

	pFNFData = newFNFData();
	pTempFNFType = newFNFType();

	if(strRead == NULL || tempString == NULL || pFNFData == NULL ||
		pTempFNFType == NULL) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		return(IPDR_ERROR);
	}

	strcpy(pTempFNFType->serviceType_,pFNFType->serviceType_);



	pCurrentListAttrDesc = pFNFType->pListAttributeDescriptor_;
	pCurrentListServiceAttrDesc = pFNFType->pListServiceAttributeDescriptor_;
	 	
   if (strTemp[countFileChars] == '\0') 
		printf("error");

	for (; (strTemp[countFileChars]!='\0')&&(pCurrentListAttrDesc != NULL);
    	countFileChars++) {                                              

		pAttributeName = (char *) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char));
		strAttributeName = (char *) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char));

		if(pAttributeName == NULL || strAttributeName == NULL) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		return(IPDR_ERROR);
		}
	
		/*Handling to escape ","*/
		encodeCharacter(strTemp,',');	
		for (strIndex = 0,isPresentFlag = 0; (strTemp[countFileChars] != ',') && 
			(strTemp[countFileChars] != '\n') && 
			(strTemp[countFileChars] != '\0');
		     strIndex++, countFileChars++,isPresentFlag++) {
				strRead[strIndex] = strTemp[countFileChars];
		}
		strRead[strIndex]='\0';
		decodeCharacter(strRead,',');
				
		pNameSpaceID = (char *) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char));
		pNameSpcID = (char *) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char));
		pNameSpaceURI = (char *) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char));

		if(pNameSpaceID == NULL || pNameSpaceURI == NULL || pNameSpcID == NULL) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		free (strAttributeName);
		return(IPDR_ERROR);
		}

if(pCurrentListAttrDesc->pAttributeDescriptor_->complexType_[0] != '\0') {
		tempString1 = (char *) calloc(strlen(pCurrentListAttrDesc->pAttributeDescriptor_->complexType_) + 1, sizeof(char));
		tempComplexType = (char *) calloc(strlen(pCurrentListAttrDesc->pAttributeDescriptor_->complexType_) + 1, sizeof(char));
		strcpy(tempString1, pCurrentListAttrDesc->pAttributeDescriptor_->complexType_);
						serviceTypeToken = strtok(tempString1, ":");
	
		if(strcmp(pCurrentListAttrDesc->pAttributeDescriptor_->complexType_,serviceTypeToken)) {
			getIPDRSubstring(pCurrentListAttrDesc->pAttributeDescriptor_->complexType_, 
								tempComplexType, ':');
   		strcpy(pCurrentListAttrDesc->pAttributeDescriptor_->complexType_,tempComplexType);
		}	
		free(tempString1);
		free(tempComplexType);
}

	if(isPrimitiveComplexType(pCurrentListAttrDesc->pAttributeDescriptor_->complexType_) == IPDR_FALSE) {
		if(!(isPresentFlag == 0)) 
		{
			if(pCurrentListAttrDesc->pAttributeDescriptor_->noOfElements_ != 0) {
				/*Array Of Structures*/
            strReadTemp = getStringBetweenDelims(strRead, (Findindex(strRead,'(') + 1) , (Findrindex(strRead, ')') -1) );
				strcpy(strRead, strReadTemp);
            encodeSpecialChar(strRead);
				encodeStringsBetweenDelims(strRead, '[', ']');

           /* while ((strRead[i] != ';') && (strRead[i] != '\0')) 
            {
                tempBuff[i] = strRead[i];
                i++;
            }
            tempBuff[i] = '\0';*/
       
            strcpy(tempStr, strRead);
				token = strtok(strRead,";");
            delimIndex = Findindex(tempStr, ';');
            if (delimIndex != NULL)
               delimIndex++;
            tempStr = delimIndex;
				decodeStringsBetweenDelims(token,'[',']');
            token = getStringBetweenDelims(token, (Findindex(token,'[') + 1) , (Findrindex(token, ']') -1) );

            encodeSpecialChar(token);
				encodeStringsBetweenDelims(token, '[', ']');

				/*while(token != NULL) */
				/*while(tempStr[i] != '\0' || validToken == 1) */
				while(token[i] != '\0' || validToken == 1) {
				pListIPDRComplex = NULL;
            delimIndex = delimIndex + k;
            if (delimIndex != NULL)
               delimIndex++;
					generateStructureData( token,
											  pCurrentListAttrDesc->pAttributeDescriptor_->complexType_,
											  pCurrentListAttrDesc->pAttributeDescriptor_,
											  pCurrentListServiceAttrDesc,
											  &pListIPDRComplex,
											  &pListIPDRData,
											  &pTempFNFType												  
											);

					appendListIPDRComplexData(&pListIPDRComplexData[j],
    												pListIPDRComplex,
    												IPDRDATA_STRUCTURE);
               validToken = 0;
               if ( (delimIndex == NULL) && (validToken == 0))
                break;

               if ( (strcmp(delimIndex,"") == 0) && validToken == 0)
						break;

               k = 0;

					if(tempStr[i] == '\0')
						break;
               while ((tempStr[i] != ';') && (tempStr[i] != '\0'))
               {
                   tempBuff[k] = tempStr[i];
                   validToken = 1;
                   k++;
                   i++;
               }
               tempBuff[k] = '\0';
               strcpy(token, tempBuff);
               decodeStringsBetweenDelims(token,'[',']');
               token = getStringBetweenDelims(token, (Findindex(token,'[') + 1) , (Findrindex(token, ']') -1) );
               if (tempStr[i] != '\0')
               {
                  i++;
               }
					j++;
				}
				appendListIPDRWithComplexData(&(pListIPDRData),pListIPDRComplexData, IPDRDATA_STRUCTURE);
   			memset(&pListIPDRComplexData, 0, sizeof(pListIPDRComplexData));
				i = 0;

			} else {
            strReadTemp = getStringBetweenDelims(strRead, Findindex(strRead,'[') + 1 , Findrindex(strRead, ']') -1 );
				strcpy(strRead, strReadTemp);
            encodeSpecialChar(strRead);
				encodeStringsBetweenDelims(strRead, '(', ')');
				encodeStringsBetweenDelims(strRead, '[', ']');
			
				pListIPDRComplex = NULL;
				generateStructureData( strRead,
											  pCurrentListAttrDesc->pAttributeDescriptor_->complexType_,
											  pCurrentListAttrDesc->pAttributeDescriptor_,
											  pCurrentListServiceAttrDesc,
											  &pListIPDRComplex,
											  &pListIPDRData,
											  &pTempFNFType												  
											);
				appendListIPDRComplexData(&pListIPDRComplexData[0],
    												pListIPDRComplex,
    												IPDRDATA_STRUCTURE);

				appendListIPDRWithComplexData(&(pListIPDRData),pListIPDRComplexData, IPDRDATA_STRUCTURE);
				pListIPDRComplexData[0] = NULL;
			}
				 strcpy(strAttributeName,	
					 pCurrentListAttrDesc->pAttributeDescriptor_->
					 attributeName_);

				 count = 0;
				 token = strtok( strAttributeName, seps );
				 while( token != NULL )
				  {
					   if(count==1)
					   strcpy(pNameSpaceURI,token);
					   if(count==2)
						strcpy(pAttributeName,token);
					  /* Get next token: */
					  token = strtok( NULL, seps );
					  count++;
				  }
				

				 /*Getting the name space ID from the ListNamespaceInfo*/
				 pCurrentListNameSpaceInfo = pListNameSpaceInfo;

				 for (countListNameSpaceInfo = 0; 
				  countListNameSpaceInfo < getLengthListNameSpaceInfo
					  (pListNameSpaceInfo); 
				  countListNameSpaceInfo++) { 
					 if(strcmp(pNameSpaceURI, 
						 pCurrentListNameSpaceInfo->
						 NameSpaceInfo_.nameSpaceURI_)==0) {
						 strcpy(pNameSpaceID,
						   pCurrentListNameSpaceInfo->NameSpaceInfo_.nameSpaceID_);
					 }

				  pCurrentListNameSpaceInfo->pNext_;
				  }
				 /*For substituting	the Namespace URI with Namespace IDs*/	

				 if (!((strcmp(pNameSpaceID, "") == 0)||
					  (strcmp(pNameSpaceID, "ipdr") == 0))) {
					 strcpy(pNameSpcID,pNameSpaceID);
					
     				len2 = 0;
					len2 = strlen(pAttributeName);
					for (count2 = 0; count2 < len2; count2++) {
						if (pAttributeName[count2] == ':') {
							flag2 = 1;
							break;
						}
					}

					if (flag2 == 0) {
					strcat(pNameSpcID, ":" );
					strcat(pNameSpcID, pAttributeName);
					strcpy(pAttributeName, pNameSpcID);
					}

				 }
			appendListAttributeDescriptor(
					&pTempFNFType->pListAttributeDescriptor_, 
					pAttributeName, 
					pCurrentListAttrDesc->pAttributeDescriptor_->attributeType_, 
					pCurrentListAttrDesc->pAttributeDescriptor_->description_,
					pCurrentListAttrDesc->pAttributeDescriptor_->derivedType_,
					pCurrentListAttrDesc->pAttributeDescriptor_->complexType_,
					pCurrentListAttrDesc->pAttributeDescriptor_->pListAttrNameList_,
					pCurrentListAttrDesc->pAttributeDescriptor_->pListAttrClassList_,
					pCurrentListAttrDesc->pAttributeDescriptor_->pListAttrMaxOccursList_,
					pCurrentListAttrDesc->pAttributeDescriptor_->noOfElements_);

			pCurrentListAttrDesc = pCurrentListAttrDesc->pNext_; 

			lFlag = 0;
		
			free(pNameSpcID);
			free(pNameSpaceID);
			free(pNameSpaceURI);
			free(strAttributeName);
			free(pAttributeName);
		}

		if(lFlag == 0) { 
		   pCurrent = pCurrentListServiceAttrDesc;
			while(pCurrent != NULL)
			{
				lFoundFlag = 0;
				pTempFNFTypeCurrent = pTempFNFType->pListServiceAttributeDescriptor_;
				while(pTempFNFTypeCurrent != NULL) {
					if(strcmp(pCurrent->pAttributeDescriptor_->attributeName_, 
				  		pTempFNFTypeCurrent->pAttributeDescriptor_->attributeName_) == 0) {
						lFoundFlag = 1;
						break;
					}
					pTempFNFTypeCurrent = pTempFNFTypeCurrent->pNext_;
				}
	
				if(lFoundFlag == 0) {	
					appendListAttributeDescriptor(
						&pTempFNFType->pListServiceAttributeDescriptor_, 
						pCurrent->pAttributeDescriptor_->attributeName_, 
						pCurrent->pAttributeDescriptor_->attributeType_, 
						pCurrent->pAttributeDescriptor_->description_,
						pCurrent->pAttributeDescriptor_->derivedType_,
						pCurrent->pAttributeDescriptor_->complexType_,
						pCurrent->pAttributeDescriptor_->pListAttrNameList_,
						pCurrent->pAttributeDescriptor_->pListAttrClassList_,
						pCurrent->pAttributeDescriptor_->pListAttrMaxOccursList_,
						pCurrent->pAttributeDescriptor_->noOfElements_);
				}
				pCurrent = pCurrent->pNext_;
			}
			lFlag = 1;
		}
		
			continue;

	} else if(pCurrentListAttrDesc->pAttributeDescriptor_->noOfElements_ != 0) {
		if(!(isPresentFlag == 0)) 
		{
			/*Array*/
			if(strRead[0] == '(') {
				count = 1;
				for(strIndex = 1,index1=0; (strRead[count] != ')'); strIndex++,index1++,count++) {
					strArrayElemnts[index1] = strRead[strIndex];
				}
			}
			generateArrayData(strArrayElemnts,
										pCurrentListAttrDesc->pAttributeDescriptor_,
										&pListIPDRData,
										&pTempFNFType
									  );

				 strcpy(strAttributeName,	
					 pCurrentListAttrDesc->pAttributeDescriptor_->
					 attributeName_);

				 count = 0;
				 token = strtok( strAttributeName, seps );
				 while( token != NULL )
				  {
					   if(count==1)
					   strcpy(pNameSpaceURI,token);
					   if(count==2)
						strcpy(pAttributeName,token);
					  /* Get next token: */
					  token = strtok( NULL, seps );
					  count++;
				  }
				

				 /*Getting the name space ID from the ListNamespaceInfo*/
				 pCurrentListNameSpaceInfo = pListNameSpaceInfo;

				 for (countListNameSpaceInfo = 0; 
				  countListNameSpaceInfo < getLengthListNameSpaceInfo
					  (pListNameSpaceInfo); 
				  countListNameSpaceInfo++) { 
					 if(strcmp(pNameSpaceURI, 
						 pCurrentListNameSpaceInfo->
						 NameSpaceInfo_.nameSpaceURI_)==0) {
						 strcpy(pNameSpaceID,
						   pCurrentListNameSpaceInfo->NameSpaceInfo_.nameSpaceID_);
					 }

				  pCurrentListNameSpaceInfo->pNext_;
				  }
				 /*For substituting	the Namespace URI with Namespace IDs*/	

				 if (!((strcmp(pNameSpaceID, "") == 0)||
					  (strcmp(pNameSpaceID, "ipdr") == 0))) {
					 strcpy(pNameSpcID,pNameSpaceID);
					
     				len2 = 0;
					len2 = strlen(pAttributeName);
					for (count2 = 0; count2 < len2; count2++) {
						if (pAttributeName[count2] == ':') {
							flag2 = 1;
							break;
						}
					}

					if (flag2 == 0) {
					strcat(pNameSpcID, ":" );
					strcat(pNameSpcID, pAttributeName);
					strcpy(pAttributeName, pNameSpcID);
					}

				 }
			appendListAttributeDescriptor(
					&pTempFNFType->pListAttributeDescriptor_, 
					pAttributeName, 
					pCurrentListAttrDesc->pAttributeDescriptor_->attributeType_, 
					pCurrentListAttrDesc->pAttributeDescriptor_->description_,
					pCurrentListAttrDesc->pAttributeDescriptor_->derivedType_,
					pCurrentListAttrDesc->pAttributeDescriptor_->complexType_,
					pCurrentListAttrDesc->pAttributeDescriptor_->pListAttrNameList_,
					pCurrentListAttrDesc->pAttributeDescriptor_->pListAttrClassList_,
					pCurrentListAttrDesc->pAttributeDescriptor_->pListAttrMaxOccursList_,
					pCurrentListAttrDesc->pAttributeDescriptor_->noOfElements_);

			pCurrentListAttrDesc = pCurrentListAttrDesc->pNext_; 
		
			free(pNameSpcID);
			free(pNameSpaceID);
			free(pNameSpaceURI);
			free(strAttributeName);
			free(pAttributeName);
			continue;
		}
	} else {

		convertToUpper((char*)pCurrentListAttrDesc->
			pAttributeDescriptor_->attributeType_, tempString);
		
		/* Collect all the non-blank attributes */
		if(!(isPresentFlag == 0)) 
		{
				if ((strcmp("INTEGER", tempString) == 0)) {

				IPDRData = newIPDRData(IPDRDATA_LONG);
				IPDRData->IPDRDataUnion_.IPDRLong_ = atoi(strRead);
				appendListIPDRData(&pListIPDRData, 
					(void *) IPDRData->IPDRDataUnion_.IPDRLong_, 
					IPDRDATA_LONG);
				freeIPDRData(IPDRData);

				}
				else if (strcmp("UNSIGNED_INTEGER", tempString) == 0) {

				IPDRData = newIPDRData(IPDRDATA_ULONG);
				IPDRData->IPDRDataUnion_.IPDRULong_ = strtoul(strRead, NULL, 10);
				appendListIPDRData(&pListIPDRData, 
					(void *) IPDRData->IPDRDataUnion_.IPDRULong_, 
					IPDRDATA_ULONG);
				freeIPDRData(IPDRData);

				}
				else if (strcmp("HYPER", tempString) == 0) {
					IPDRData = newIPDRData(IPDRDATA_LONGLONG);
				
					IPDRData->IPDRDataUnion_.IPDRLongLong_ = 
						(hyper) strtod(strRead,&stopstring);

					appendListIPDRData(&pListIPDRData, 
						(void *) &(IPDRData->IPDRDataUnion_.IPDRLongLong_), 
						IPDRDATA_LONGLONG);
					freeIPDRData(IPDRData);
				}
				else if (strcmp("UNSIGNED_HYPER", tempString) == 0) {
					IPDRData = newIPDRData(IPDRDATA_ULONGLONG);
					IPDRData->IPDRDataUnion_.IPDRULongLong_ = 
						(hyper) strtod(strRead,&stopstring);
					appendListIPDRData(&pListIPDRData, 
						(void *) &(IPDRData->IPDRDataUnion_.IPDRULongLong_), 
						IPDRDATA_ULONGLONG);
					freeIPDRData(IPDRData);
				}
				else if (strcmp("FLOAT", tempString) == 0) {
				IPDRData = newIPDRData(IPDRDATA_FLOAT);
				IPDRData->IPDRDataUnion_.IPDRFloat_ = (float)atof(strRead);
				
				 IPDRValue = 
					 &IPDRData->IPDRDataUnion_.IPDRFloat_;

				appendListIPDRData(&pListIPDRData, IPDRValue, IPDRDATA_FLOAT);
				freeIPDRData(IPDRData);

				}
				else if (strcmp("DOUBLE", tempString) == 0) {

				IPDRData = newIPDRData(IPDRDATA_DOUBLE);
				IPDRData->IPDRDataUnion_.IPDRDouble_= strtod(strRead, 
					&stopstring);
				
				IPDRValue = 
					&IPDRData->IPDRDataUnion_.IPDRDouble_;

				appendListIPDRData(&pListIPDRData, IPDRValue, IPDRDATA_DOUBLE);

				freeIPDRData(IPDRData);
				}
				else if (strcmp("BYTE_ARRAY", tempString) == 0) {
					IPDRData = newIPDRData(IPDRDATA_HEXADECARRAY);
					strcpy((char *)IPDRData->IPDRDataUnion_.IPDRByteArray_ ,strRead);
					appendListIPDRData(&pListIPDRData, IPDRData->
						IPDRDataUnion_.IPDRByteArray_, IPDRDATA_HEXADECARRAY);
					freeIPDRData(IPDRData);
				}
				else if (strcmp("UTF8_ARRAY", tempString) == 0) {
					IPDRData = newIPDRData(IPDRDATA_STRING);
					encodeSpecialChar(strRead);
					decodeSpecialChar(strRead);
					strcpy((char *)IPDRData->IPDRDataUnion_.IPDRUTF8Array_ ,strRead);
					appendListIPDRData(&pListIPDRData, IPDRData->
						IPDRDataUnion_.IPDRUTF8Array_, IPDRDATA_STRING);

					freeIPDRData(IPDRData);
				}
				else if (strcmp("BOOLEAN", tempString) == 0) {
					IPDRData = newIPDRData(IPDRDATA_BOOLEAN);
					if(strcmp(strRead, "false") == 0) {
						strcpy((char *)IPDRData->IPDRDataUnion_.IPDRBoolean_ ,"0");
					} else if(strcmp(strRead, "true") == 0) {
						strcpy((char *)IPDRData->IPDRDataUnion_.IPDRBoolean_ ,"1");
					} 
					appendListIPDRData(&pListIPDRData, 
						IPDRData->IPDRDataUnion_.IPDRBoolean_, 
						IPDRDATA_BOOLEAN);

					freeIPDRData(IPDRData);
				}
				if (strcmp("SHORT", tempString) == 0) {

				IPDRData = newIPDRData(IPDRDATA_SHORT);
				IPDRData->IPDRDataUnion_.IPDRShort_ = atoi(strRead);
				appendListIPDRData(&pListIPDRData, 
					(void *)IPDRData->IPDRDataUnion_.IPDRShort_, 
					IPDRDATA_SHORT);
				freeIPDRData(IPDRData);

				}
				if (strcmp("UNSIGNED_SHORT", tempString) == 0) {

				IPDRData = newIPDRData(IPDRDATA_USHORT);
				IPDRData->IPDRDataUnion_.IPDRUShort_ = atoi(strRead);
				appendListIPDRData(&pListIPDRData, 
					(void *)IPDRData->IPDRDataUnion_.IPDRUShort_, 
					IPDRDATA_USHORT);
				freeIPDRData(IPDRData);

				}
				if ((strcmp("BYTE", tempString) == 0)) {

				IPDRData = newIPDRData(IPDRDATA_BYTE);
				IPDRData->IPDRDataUnion_.IPDRByte_ = atoi(strRead);
				appendListIPDRData(&pListIPDRData, 
					(void *) IPDRData->IPDRDataUnion_.IPDRByte_, 
					IPDRDATA_BYTE);
				freeIPDRData(IPDRData);

				}
				if ((strcmp("UNSIGNED_BYTE", tempString) == 0)) {

				IPDRData = newIPDRData(IPDRDATA_UBYTE);
				IPDRData->IPDRDataUnion_.IPDRByte_ = atoi(strRead);
				appendListIPDRData(&pListIPDRData, 
					(void *) IPDRData->IPDRDataUnion_.IPDRByte_, 
					IPDRDATA_UBYTE);
				freeIPDRData(IPDRData);

				}
			
				 strcpy(strAttributeName,	
					 pCurrentListAttrDesc->pAttributeDescriptor_->
					 attributeName_);

				 count = 0;
				 token = strtok( strAttributeName, seps );
				 while( token != NULL )
				  {
					   if(count==1)
					   strcpy(pNameSpaceURI,token);
					   if(count==2)
						strcpy(pAttributeName,token);
					  /* Get next token: */
					  token = strtok( NULL, seps );
					  count++;
				  }
				

				 /*Getting the name space ID from the ListNamespaceInfo*/
				 pCurrentListNameSpaceInfo = pListNameSpaceInfo;

				 for (countListNameSpaceInfo = 0; 
				  countListNameSpaceInfo < getLengthListNameSpaceInfo
					  (pListNameSpaceInfo); 
				  countListNameSpaceInfo++) { 
					 if(strcmp(pNameSpaceURI, 
						 pCurrentListNameSpaceInfo->
						 NameSpaceInfo_.nameSpaceURI_)==0) {
						 strcpy(pNameSpaceID,
						   pCurrentListNameSpaceInfo->NameSpaceInfo_.nameSpaceID_);
					 }

				  pCurrentListNameSpaceInfo->pNext_;
				  }
				 /*For substituting	the Namespace URI with Namespace IDs*/	

				 if (!((strcmp(pNameSpaceID, "") == 0)||
					  (strcmp(pNameSpaceID, "ipdr") == 0))) {
					 strcpy(pNameSpcID,pNameSpaceID);
					
     				len2 = 0;
					len2 = strlen(pAttributeName);
					for (count2 = 0; count2 < len2; count2++) {
						if (pAttributeName[count2] == ':') {
							flag2 = 1;
							break;
						}
					}

					if (flag2 == 0) {
					strcat(pNameSpcID, ":" );
					strcat(pNameSpcID, pAttributeName);
					strcpy(pAttributeName, pNameSpcID);
					}

				 }
				
		/* The descriptor is created for the input data string
				 corresponding to the attribute values that are populated*/		

			    appendListAttributeDescriptor(
					&pTempFNFType->pListAttributeDescriptor_, 
					pAttributeName, 
					pCurrentListAttrDesc->pAttributeDescriptor_->attributeType_, 
					pCurrentListAttrDesc->pAttributeDescriptor_->description_,
					pCurrentListAttrDesc->pAttributeDescriptor_->derivedType_,
					pCurrentListAttrDesc->pAttributeDescriptor_->complexType_,
					pCurrentListAttrDesc->pAttributeDescriptor_->pListAttrNameList_,
					pCurrentListAttrDesc->pAttributeDescriptor_->pListAttrClassList_,
					pCurrentListAttrDesc->pAttributeDescriptor_->pListAttrMaxOccursList_,
					pCurrentListAttrDesc->pAttributeDescriptor_->noOfElements_);
		}

	}
	
			
		/* Populating pListIPDRData from each record in the input file*/
	
		
		pCurrentListAttrDesc = pCurrentListAttrDesc->pNext_; 
		
		free(pNameSpcID);
		free(pNameSpaceID);
		free(pNameSpaceURI);
		free(strAttributeName);
		free(pAttributeName);
	
	
	}
		
	#ifdef DEBUG_IPDR
	{
		printf("\nPrinting tempFNFType");
		printFNFType(pTempFNFType);
	}
	#endif

          
	errorSeverity = createFNFData(pIPDRCommonParameters, pTempFNFType ,
		pListIPDRData, pFNFData, pErrorCode);

	 if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, "createFNFData"); 
		}
	 else if (errorSeverity == IPDR_ERROR) {
	   exceptionHandler(pIPDRCommonParameters, pErrorCode, "createFNFData"); 
	  return (IPDR_ERROR);
	
	 }

	#ifdef DEBUG_IPDR
	{
		printf("\nPrinting FNFData");
		printFNFData(pFNFData);
	}
	#endif

	errorSeverity = writeIPDR(pIPDRCommonParameters, pFNFData, pErrorCode);

	if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, "writeIPDR"); 
	}
	else if (errorSeverity == IPDR_ERROR) {
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDR"); 
	  return (IPDR_ERROR);	
	 }


	freeFNFData(pFNFData);
	freeFNFType(pTempFNFType);
	freeListIPDRData (&pListIPDRData);
	free(strRead);
	free(strReadTemp);
	free(strArrayElemnts);
	free(tempString);
	return (IPDR_OK);
}


	 
/*!
 *  NAME:
 *      printUsage() - Prints test toll usage syntax. 
 *								
 *  DESCRIPTION:
 *				  This Functions is called whenver there are any  
 *				  invalid command line arguments.
 *  
 *  ARGUMENTS:
 *		
 *			VOID
 *			
 *  RETURNS:	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int printUsage()
{
	printf("\nCorrect Input String is: -doctype XDR/XML -outfile OutputFileName <-schemavalidation> <-ipdrversion 3.5/3.1> <-recordcount> <No Of Records> <-roundrobin> TestDataFile1 <TestDataFile2> <TestDataFile3>");
	return (IPDR_OK);
}

int generateArrayData(char* pArrayElements,
							 AttributeDescriptor* pAttributeDescriptor,
							 ListIPDRData** pListIPDRData,
							 FNFType** pTempFNFType
							)
{
	int strIndex = 0;
	int count = 0;
	int countFileChars = 0 ;
	int countListNameSpaceInfo = 0;
	int errorSeverity = 0;
	int count2 = 0;
	int len2 = 0;
	int flag2 = 0;
	char* token = NULL;
	char* strRead = NULL;
	char* strArrayElemnts = NULL;
	char* tempString = NULL;
	char* stopstring = NULL;
	char* pNameSpcID = NULL;
	char* pNameSpaceID = NULL;
	char* pNameSpaceURI = NULL;
	char* pAttributeName = NULL;
	char* strAttributeName = NULL;
	char seps[]   = "$";
	IPDRData* IPDRData = NULL;
   void* IPDRValue =  NULL;

	void* dataValue[MAX_IPDR_STRING];
   int dataType[MAX_IPDR_STRING];
    
   memset(dataValue, 0, sizeof(dataValue));
   memset(dataType, 0, sizeof(dataType));

	strRead = (char *) calloc(MAX_IPDR_STRING, sizeof(char));
	tempString = (char *)(calloc(MAX_IPDR_STRING, strlen(pAttributeDescriptor->attributeType_+1)));

	strcpy(tempString, (char*)pAttributeDescriptor->attributeType_);
		
		/* Collect all the non-blank attributes */
		if (strcmp("INTEGER", tempString) == 0) {
			token = strtok( pArrayElements, ";");
			IPDRData = newIPDRData(IPDRDATA_LONG);
			while(token != NULL) {
				strcpy(strRead, token);
				token = strtok( NULL, ";");
		
				IPDRData->IPDRDataUnion_.IPDRLong_ = atoi(strRead);
				dataValue[count] = (void *) IPDRData->IPDRDataUnion_.IPDRLong_;
				count++;
			}
			dataType[0] = IPDRDATA_ARRAY;
    		dataType[1] = IPDRDATA_LONG;
    		appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
			freeIPDRData(IPDRData);
		}
		else if (strcmp("UNSIGNED_INTEGER", tempString) == 0) {
			token = strtok( pArrayElements, ";");
			IPDRData = newIPDRData(IPDRDATA_ULONG);
			while(token != NULL) {
				strcpy(strRead, token);
				token = strtok( NULL, ";");

				IPDRData->IPDRDataUnion_.IPDRULong_ = strtoul(strRead, NULL, 10);
				dataValue[count] = (void *)IPDRData->IPDRDataUnion_.IPDRULong_;
				count++;
			}
			dataType[0] = IPDRDATA_ARRAY;
    		dataType[1] = IPDRDATA_ULONG;
    		appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
			freeIPDRData(IPDRData);

		}
		else if (strcmp("HYPER", tempString) == 0) {
			token = strtok( pArrayElements, ";");
			IPDRData = newIPDRData(IPDRDATA_LONGLONG);
			while(token != NULL) {
				strcpy(strRead, token);
				token = strtok( NULL, ";");
				IPDRData->IPDRDataUnion_.IPDRLongLong_ = 
						(hyper) strtod(strRead,&stopstring);

				dataValue[count] = (void *) &(IPDRData->IPDRDataUnion_.IPDRLongLong_);
				count++;
			}
			dataType[0] = IPDRDATA_ARRAY;
    		dataType[1] = IPDRDATA_LONGLONG;
    		appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
			freeIPDRData(IPDRData);
		}
		else if (strcmp("UNSIGNED_HYPER", tempString) == 0) {
			token = strtok( pArrayElements, ";");
			IPDRData = newIPDRData(IPDRDATA_ULONGLONG);
			while(token != NULL) {
				strcpy(strRead, token);
				token = strtok( NULL, ";");
				IPDRData->IPDRDataUnion_.IPDRULongLong_ = 
						(u_hyper) strtod(strRead,&stopstring);
				dataValue[count] = (void *) &(IPDRData->IPDRDataUnion_.IPDRULongLong_);
				count++;
			}
			dataType[0] = IPDRDATA_ARRAY;
    		dataType[1] = IPDRDATA_ULONGLONG;
    		appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
			freeIPDRData(IPDRData);
		}
		else if (strcmp("FLOAT", tempString) == 0) {
			token = strtok( pArrayElements, ";");
			IPDRData = newIPDRData(IPDRDATA_FLOAT);
			while(token != NULL) {
				strcpy(strRead, token);
				token = strtok( NULL, ";");
				IPDRData->IPDRDataUnion_.IPDRFloat_ = (float)atof(strRead);
				IPDRValue = 
					 &IPDRData->IPDRDataUnion_.IPDRFloat_;
				dataValue[count] = IPDRValue;
				count++;
			}
			dataType[0] = IPDRDATA_ARRAY;
    		dataType[1] = IPDRDATA_FLOAT;
    		appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
			freeIPDRData(IPDRData);

		}
		else if (strcmp("DOUBLE", tempString) == 0) {
			token = strtok( pArrayElements, ";");
			IPDRData = newIPDRData(IPDRDATA_DOUBLE);
			while(token != NULL) {
				strcpy(strRead, token);
				token = strtok( NULL, ";");
				IPDRData->IPDRDataUnion_.IPDRDouble_= (double)strtod(strRead, &stopstring);
				IPDRValue = 
					&IPDRData->IPDRDataUnion_.IPDRDouble_;
				dataValue[count] = IPDRValue;
				count++;
			}
			dataType[0] = IPDRDATA_ARRAY;
    		dataType[1] = IPDRDATA_DOUBLE;
    		appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
			freeIPDRData(IPDRData);
		}
		else if (strcmp("BYTE_ARRAY", tempString) == 0) {
			token = strtok( pArrayElements, ";");
			IPDRData = newIPDRData(IPDRDATA_HEXADECARRAY);
			while(token != NULL) {
				strcpy(strRead, token);
				token = strtok( NULL, ";");
				strcpy((char *)IPDRData->IPDRDataUnion_.IPDRByteArray_ ,strRead);
				dataValue[count] = (void *) calloc (1, strlen(strRead)+1);
    			strcpy((char *)dataValue[count], (char *)IPDRData->IPDRDataUnion_.IPDRByteArray_);
				count++;
			}
			dataType[0] = IPDRDATA_ARRAY;
    		dataType[1] = IPDRDATA_HEXADECARRAY;
    		appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
			freeIPDRData(IPDRData);
		}
		else if (strcmp("UTF8_ARRAY", tempString) == 0) {
			token = strtok( pArrayElements, ";");
			IPDRData = newIPDRData(IPDRDATA_STRING);
			while(token != NULL) {
				strcpy(strRead, token);
				token = strtok( NULL, ";");
				encodeSpecialChar(strRead);
				decodeSpecialChar(strRead);
				strcpy((char *)IPDRData->IPDRDataUnion_.IPDRUTF8Array_ ,strRead);
				dataValue[count] = (void *) calloc (1, strlen(strRead)+1);
    			strcpy((char *)dataValue[count], (char *)IPDRData->IPDRDataUnion_.IPDRUTF8Array_);
				count++;
			}
			dataType[0] = IPDRDATA_ARRAY;
    		dataType[1] = IPDRDATA_STRING;
    		appendListIPDRDataComplex(&(*pListIPDRData), dataValue, dataType);
			freeIPDRData(IPDRData);
		}
	free(tempString);

	return (IPDR_OK);
}

int generateStructureData( char* pArrayElements,
									char* pComplexType,
   								AttributeDescriptor* pAttributeDescriptor,
									ListAttributeDescriptor* pListServiceAttributeDescriptor,
									ListIPDRComplex** pListIPDRComplex,
   								ListIPDRData** pListIPDRData,
   								FNFType** pTempFNFType
								)
{
	int strIndex = 0;
	int count = 0;
	int len = 0;
	int countNext = 0;
   int cnt = 0;
	int lreturn = IPDR_ERROR;
	int errorSeverity = 0;
	int attributeDescriptorLength = 0;
	int typeCode = 0;
	int noOfElements = 0;
	int byteArraylength = 0;
	int utfArrayLength = 0;
	char* tempComplexType;
   char * tempString;
	char* tempComplexType1;
   char * tempString1;
	char* resultString;
	char* serviceTypeToken = NULL;
	char* stopstring = NULL;
	char* token = NULL;
	char* structure_token = NULL;
	char* tokenValue = NULL;
	char* strRead = NULL;
	char* strArrayElemnts = NULL;
	char* tokentemp = NULL;

   int j = 0;
   int k = 0;
   static int i;
   char tempBuff[MAX_LEN];
   char tempArrayElements[MAX_LEN];

 	ListAttributeDescriptor* pCurrentListServiceAttributeDescriptor = NULL;
 	ListAttributeDescriptor* pOldListServiceAttributeDescriptor = NULL;
 	ListAttributeDescriptor* pTempOldListServiceAttributeDescriptor = NULL;
 	ListAttributeDescriptor* pCurrServiceList = NULL;
	List* pcurrentMaxOccrsList = NULL;
	List* pcurrent = NULL;

	void* dataValue[MAX_IPDR_STRING];
	IPDRData* pIPDRData; 
	hyper* tempHyper;
	u_hyper* tempUHyper;
	wchar* tempUTFArray;

	strRead = (char *) calloc(MAX_IPDR_STRING, sizeof(char));
	strArrayElemnts = (char *) calloc(MAX_IPDR_STRING, sizeof(char));

	memset(dataValue, 0, sizeof(dataValue));


/*
	if(pIPDRCommonParameters == NULL || pRecordDescriptor == NULL) {
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		return(IPDR_ERROR);
	}
*/
	attributeDescriptorLength = getLengthListAttributeDescriptor(pListServiceAttributeDescriptor);

	tempString = (char *) calloc(strlen(pComplexType) + 1, sizeof(char));
	tempComplexType = (char *) calloc(strlen(pComplexType) + 1, sizeof(char));
	strcpy(tempString, pComplexType);
	serviceTypeToken = strtok(tempString, ":");
	
	if(strcmp(pComplexType,serviceTypeToken)) {
		getIPDRSubstring(pComplexType, tempComplexType, ':');
   	strcpy(pComplexType,tempComplexType);
	}	
	free(tempString);
	free(tempComplexType);

	pCurrentListServiceAttributeDescriptor = pListServiceAttributeDescriptor;
   pOldListServiceAttributeDescriptor = pListServiceAttributeDescriptor;
   pTempOldListServiceAttributeDescriptor = pListServiceAttributeDescriptor;

	tempString1 = (char *) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char));
	tempComplexType1 = (char *) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char));

	for (count = 0; count < attributeDescriptorLength; count++) {
			strcpy(tempString1, pListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_);
			serviceTypeToken = strtok(tempString1, ":");
	
			if(strcmp(pListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_,serviceTypeToken)) {
				getIPDRSubstring(pListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_, 
								tempComplexType1, ':');
				getIPDRSubstring(tempComplexType1, tempString1, '$');
   			strcpy(pListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_,tempString1);
			}	

		if(strcmp(pComplexType, 
					 pListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_) == 0) {
			pcurrent = pListServiceAttributeDescriptor->pAttributeDescriptor_->pListAttrNameList_;
			if(pListServiceAttributeDescriptor->pAttributeDescriptor_->pListAttrMaxOccursList_ != NULL) {
				pcurrentMaxOccrsList = pListServiceAttributeDescriptor->pAttributeDescriptor_->pListAttrMaxOccursList_;
			}

      while ((pArrayElements[i] != ';') && (pArrayElements[i] != '\0')) {
         tempBuff[i] = pArrayElements[i];

         
         i++;
      }
      tempBuff[i] = '\0';
      decodeSpecialChar(tempBuff);
      
      strcpy(tempArrayElements, pArrayElements);

		while(pArrayElements[j] != '\0') {
			while((pcurrent != NULL))	{
				pOldListServiceAttributeDescriptor = pTempOldListServiceAttributeDescriptor;
				for (countNext = 0; countNext < attributeDescriptorLength; countNext++) {
				if(strcmp(pcurrent->node, 
					   pOldListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_) == 0) {

					if(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_[0] != '\0') {
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
				}
					if((lreturn = isPrimitiveComplexType(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_)) == IPDR_TRUE) {

					typeCode = convAttributeTypeToTypeCode(
											pOldListServiceAttributeDescriptor->pAttributeDescriptor_->attributeType_);

              		switch (typeCode)
              		{
              			case IPDRDATA_LONG:

              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
										decodeStringsBetweenDelims(tempBuff,'(',')');
            						tokentemp = getStringBetweenDelims(tempBuff, Findindex(tempBuff,'(') + 1 , Findrindex(tempBuff, ')') -1 );
										strcpy(strRead, tokentemp);
										token = strtok( strRead, ";");
										while(token != NULL) {
											token = strtok( NULL, ";");
											noOfElements++;
										}
									}
									else if(atoi(pcurrentMaxOccrsList->node) > 0){
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
										decodeStringsBetweenDelims(pArrayElements,'(',')');
            						tokentemp = getStringBetweenDelims(tempBuff, Findindex(tempBuff,'(') + 1 , Findrindex(tempBuff, ')') -1 );
									}
              				}
              
              				if (noOfElements > 0) {
									pIPDRData = newIPDRData(IPDRDATA_LONG);
									tokenValue = strtok( tokentemp, ";");

              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										strcpy(strRead, tokenValue);
										tokenValue = strtok( NULL, ";");
										pIPDRData->IPDRDataUnion_.IPDRLong_ = atoi(strRead);
      								dataValue[cnt] = (void *)pIPDRData->IPDRDataUnion_.IPDRLong_;
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_LONG);
              				}
              				else {
									pIPDRData = newIPDRData(IPDRDATA_LONG);
									strcpy(strRead, tempBuff);
									pIPDRData->IPDRDataUnion_.IPDRLong_ = atoi(strRead);
									dataValue[0] = (void *)pIPDRData->IPDRDataUnion_.IPDRLong_;
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_LONG);
              				}
								free(pIPDRData);
                        break;
              
              			case IPDRDATA_ULONG: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
										decodeStringsBetweenDelims(tempBuff,'(',')');
            						tokentemp = getStringBetweenDelims(tempBuff, Findindex(tempBuff,'(') + 1 , Findrindex(tempBuff, ')') -1 );
										strcpy(strRead, tokentemp);
										token = strtok( strRead, ";");
										while(token != NULL) {
											token = strtok( NULL, ";");
											noOfElements++;
										}
									}
									else if(atoi(pcurrentMaxOccrsList->node) > 0){
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
										decodeStringsBetweenDelims(pArrayElements,'(',')');
            						tokentemp = getStringBetweenDelims(tempBuff, Findindex(tempBuff,'(') + 1 , Findrindex(tempBuff, ')') -1 );
									}
              				}
              
              				if (noOfElements > 0) {
									pIPDRData = newIPDRData(IPDRDATA_ULONG);
									tokenValue = strtok( tokentemp, ";");

              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										strcpy(strRead, tokenValue);
										tokenValue = strtok( NULL, ";");
										pIPDRData->IPDRDataUnion_.IPDRULong_ = atoi(strRead);
      								dataValue[cnt] = (void *)pIPDRData->IPDRDataUnion_.IPDRULong_;
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_ULONG);
              				}
              				else {
									pIPDRData = newIPDRData(IPDRDATA_ULONG);
									strcpy(strRead, tempBuff);
									pIPDRData->IPDRDataUnion_.IPDRULong_ = atoi(strRead);
									dataValue[0] = (void *)pIPDRData->IPDRDataUnion_.IPDRULong_;
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_ULONG);
              				}
								free(pIPDRData);
                        break;
              
                       case IPDRDATA_LONGLONG: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
										decodeStringsBetweenDelims(tempBuff,'(',')');
            						tokentemp = getStringBetweenDelims(tempBuff, Findindex(tempBuff,'(') + 1 , Findrindex(tempBuff, ')') -1 );
										strcpy(strRead, tokentemp);
										token = strtok( strRead, ";");
										while(token != NULL) {
											token = strtok( NULL, ";");
											noOfElements++;
										}
									}
									else if(atoi(pcurrentMaxOccrsList->node) > 0){
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
										decodeStringsBetweenDelims(pArrayElements,'(',')');
            						tokentemp = getStringBetweenDelims(tempBuff, Findindex(tempBuff,'(') + 1 , Findrindex(tempBuff, ')') -1 );
									}
              				}
              
              				if (noOfElements > 0) {
									pIPDRData = newIPDRData(IPDRDATA_LONGLONG);
									tokenValue = strtok( tokentemp, ";");

              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										strcpy(strRead, tokenValue);
										tokenValue = strtok( NULL, ";");
										pIPDRData->IPDRDataUnion_.IPDRLongLong_ = (hyper) strtod(strRead,&stopstring);
      								dataValue[cnt] = (void *)&(pIPDRData->IPDRDataUnion_.IPDRLongLong_);
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_LONGLONG);
              				}
              				else {
									pIPDRData = newIPDRData(IPDRDATA_LONGLONG);
									decodeStringsBetweenDelims(pArrayElements,'(',')');
									strcpy(strRead, tempBuff);
									pIPDRData->IPDRDataUnion_.IPDRLongLong_ = (hyper) strtod(strRead,&stopstring);
      							dataValue[0] = (void *)&(pIPDRData->IPDRDataUnion_.IPDRLongLong_);
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_LONGLONG);
              				}
								freeIPDRData (pIPDRData);
                         break;
              
                       case IPDRDATA_ULONGLONG: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
										decodeStringsBetweenDelims(tempBuff,'(',')');
            						tokentemp = getStringBetweenDelims(tempBuff, Findindex(tempBuff,'(') + 1 , Findrindex(tempBuff, ')') -1 );
										strcpy(strRead, tokentemp);
										token = strtok( strRead, ";");
										while(token != NULL) {
											token = strtok( NULL, ";");
											noOfElements++;
										}
									}
									else if(atoi(pcurrentMaxOccrsList->node) > 0){
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
										decodeStringsBetweenDelims(pArrayElements,'(',')');
            						tokentemp = getStringBetweenDelims(tempBuff, Findindex(tempBuff,'(') + 1 , Findrindex(tempBuff, ')') -1 );
									}
              				}
              
              				if (noOfElements > 0) {
									pIPDRData = newIPDRData(IPDRDATA_ULONGLONG);
									tokenValue = strtok( tokentemp, ";");

              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										strcpy(strRead, tokenValue);
										tokenValue = strtok( NULL, ";");
										pIPDRData->IPDRDataUnion_.IPDRULongLong_ = (u_hyper) strtod(strRead,&stopstring);
      								dataValue[cnt] = (void *)&(pIPDRData->IPDRDataUnion_.IPDRULongLong_);
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_ULONGLONG);
              				}
              				else {
									pIPDRData = newIPDRData(IPDRDATA_ULONGLONG);
									strcpy(strRead, tempBuff);
									pIPDRData->IPDRDataUnion_.IPDRULongLong_ = (u_hyper) strtod(strRead,&stopstring);
      							dataValue[0] = (void *)&(pIPDRData->IPDRDataUnion_.IPDRULongLong_);
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_ULONGLONG);
              				}
								freeIPDRData (pIPDRData);
                        break;
              
                       case IPDRDATA_FLOAT: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
										decodeStringsBetweenDelims(tempBuff,'(',')');
            						tokentemp = getStringBetweenDelims(tempBuff, Findindex(tempBuff,'(') + 1 , Findrindex(tempBuff, ')') -1 );
										strcpy(strRead, tokentemp);
										token = strtok( strRead, ";");
										while(token != NULL) {
											token = strtok( NULL, ";");
											noOfElements++;
										}
									}
									else if(atoi(pcurrentMaxOccrsList->node) > 0){
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
										decodeStringsBetweenDelims(pArrayElements,'(',')');
            						tokentemp = getStringBetweenDelims(tempBuff, Findindex(tempBuff,'(') + 1 , Findrindex(tempBuff, ')') -1 );
									}
              				}
              
              				if (noOfElements > 0) {
									pIPDRData = newIPDRData(IPDRDATA_FLOAT);
									tokenValue = strtok( tokentemp, ";");

              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										strcpy(strRead, tokenValue);
										tokenValue = strtok( NULL, ";");
				   					pIPDRData->IPDRDataUnion_.IPDRFloat_ = (float)atof(strRead);
      								dataValue[cnt] = (void *) &(pIPDRData->IPDRDataUnion_.IPDRFloat_);
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_FLOAT);
              				}
              				else {
									pIPDRData = newIPDRData(IPDRDATA_FLOAT); 
									strcpy(strRead, tempBuff);
				   				pIPDRData->IPDRDataUnion_.IPDRFloat_ = 
															(float)atof(strRead);
      							dataValue[0] = (void *)&(pIPDRData->IPDRDataUnion_.IPDRFloat_);
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_FLOAT);
              				}
								freeIPDRData (pIPDRData);
              				break;
              
                       case IPDRDATA_DOUBLE: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
										decodeStringsBetweenDelims(tempBuff,'(',')');
            						tokentemp = getStringBetweenDelims(tempBuff, Findindex(tempBuff,'(') + 1 , Findrindex(tempBuff, ')') -1 );
										strcpy(strRead, tokentemp);
										token = strtok( strRead, ";");
										while(token != NULL) {
											token = strtok( NULL, ";");
											noOfElements++;
										}
									}
									else if(atoi(pcurrentMaxOccrsList->node) > 0){
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
										decodeStringsBetweenDelims(pArrayElements,'(',')');
            						tokentemp = getStringBetweenDelims(tempBuff, Findindex(tempBuff,'(') + 1 , Findrindex(tempBuff, ')') -1 );
									}
              				}
              
              				if (noOfElements > 0) {
									pIPDRData = newIPDRData(IPDRDATA_DOUBLE);
									tokenValue = strtok( tokentemp, ";");

              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										strcpy(strRead, tokenValue);
										tokenValue = strtok( NULL, ";");
				   					pIPDRData->IPDRDataUnion_.IPDRDouble_ = (double )strtod(strRead, &stopstring);
      								dataValue[cnt] = (void *)&(pIPDRData->IPDRDataUnion_.IPDRDouble_);
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_DOUBLE);
              				}
              				else {
									pIPDRData = newIPDRData(IPDRDATA_DOUBLE); 
									strcpy(strRead, tempBuff);
				   				pIPDRData->IPDRDataUnion_.IPDRDouble_ = 
															(double )strtod(strRead, &stopstring);
      							dataValue[0] = (void *)&(pIPDRData->IPDRDataUnion_.IPDRDouble_);
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_DOUBLE);
              				}
								freeIPDRData (pIPDRData);
              				break;
              
                       case IPDRDATA_HEXADECARRAY: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
										decodeStringsBetweenDelims(tempBuff,'(',')');
            						tokentemp = getStringBetweenDelims(tempBuff, Findindex(tempBuff,'(') + 1 , Findrindex(tempBuff, ')') -1 );
										strcpy(strRead, tokentemp);
										token = strtok( strRead, ";");
										while(token != NULL) {
											token = strtok( NULL, ";");
											noOfElements++;
										}
									}
									else if(atoi(pcurrentMaxOccrsList->node) > 0) {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
										decodeStringsBetweenDelims(pArrayElements,'(',')');
            						tokentemp = getStringBetweenDelims(tempBuff, Findindex(tempBuff,'(') + 1 , Findrindex(tempBuff, ')') -1 );
									}
              				}
              
              				if (noOfElements > 0) {
									pIPDRData = newIPDRData(IPDRDATA_HEXADECARRAY);
									tokenValue = strtok( tokentemp, ";");

              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										strcpy(strRead, tokenValue);
										tokenValue = strtok( NULL, ";");
										pIPDRData->IPDRDataFlag_ = IPDRDATA_HEXADECARRAY;
										strcpy((char *)pIPDRData->IPDRDataUnion_.IPDRByteArray_ ,strRead);
										dataValue[cnt] = (void *) calloc (1, (strlen(strRead)+1));
      								strcpy((char *)dataValue[cnt], (char *)pIPDRData->IPDRDataUnion_.IPDRByteArray_);
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_HEXADECARRAY);
              				}
              				else {
									pIPDRData = (IPDRData *) malloc(sizeof(IPDRData));
									strcpy(strRead, tempBuff);
									pIPDRData->IPDRDataFlag_ = IPDRDATA_HEXADECARRAY;
									strcpy((char *)pIPDRData->IPDRDataUnion_.IPDRByteArray_ ,strRead);
									dataValue[0] = (void *) calloc (1, (strlen(strRead)+1));
      							strcpy((char *)dataValue[0], (char *)pIPDRData->IPDRDataUnion_.IPDRByteArray_);
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_HEXADECARRAY);
              				}
								freeIPDRData (pIPDRData);
              				break;
              
                       case IPDRDATA_STRING: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
										decodeStringsBetweenDelims(tempBuff,'(',')');
            						tokentemp = getStringBetweenDelims(tempBuff, Findindex(tempBuff,'(') + 1 , Findrindex(tempBuff, ')') -1 );
										strcpy(strRead, tokentemp);
										token = strtok( strRead, ";");
										while(token != NULL) {
											token = strtok( NULL, ";");
											noOfElements++;
										}
									}
									else if(atoi(pcurrentMaxOccrsList->node) > 0) {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
										decodeStringsBetweenDelims(tempBuff,'(',')');
            						tokentemp = getStringBetweenDelims(tempBuff, Findindex(tempBuff,'(') + 1 , Findrindex(tempBuff, ')') -1 );
									}
              				}
              
              				if (noOfElements > 0) {
									pIPDRData = newIPDRData(IPDRDATA_STRING);
									tokenValue = strtok( tokentemp, ";");

              					for(cnt = 0;cnt < noOfElements; cnt++) { 
      								decodeSpecialChar(tokenValue);
										strcpy(strRead, tokenValue);
										tokenValue = strtok( NULL, ";");
										strcpy((char *)pIPDRData->IPDRDataUnion_.IPDRUTF8Array_ ,strRead);
										dataValue[cnt] = (void *) calloc (1,  (strlen(strRead)+1));
      								strcpy((char *)dataValue[cnt], (char *)pIPDRData->IPDRDataUnion_.IPDRUTF8Array_);
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_STRING);
              				}
              				else {
									pIPDRData = newIPDRData(IPDRDATA_STRING);
									strcpy(strRead, tempBuff);
									strcpy((char *)pIPDRData->IPDRDataUnion_.IPDRUTF8Array_ ,strRead);
									dataValue[0] = (void *) calloc (1, strlen(strRead)+1);
    								strcpy((char *)dataValue[0], (char *)pIPDRData->IPDRDataUnion_.IPDRUTF8Array_);
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_STRING);
              				}
								freeIPDRData (pIPDRData);
              				break;
              			
                       case IPDRDATA_BOOLEAN: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
										decodeStringsBetweenDelims(tempBuff,'(',')');
            						tokentemp = getStringBetweenDelims(tempBuff, Findindex(tempBuff,'(') + 1 , Findrindex(tempBuff, ')') -1 );
										strcpy(strRead, tokentemp);
										token = strtok( strRead, ";");
										while(token != NULL) {
											token = strtok( NULL, ";");
											noOfElements++;
										}
									}
									else if(atoi(pcurrentMaxOccrsList->node) > 0){
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
										decodeStringsBetweenDelims(pArrayElements,'(',')');
            						tokentemp = getStringBetweenDelims(tempBuff, Findindex(tempBuff,'(') + 1 , Findrindex(tempBuff, ')') -1 );
									}
              				}
              
              				if (noOfElements > 0) {
									pIPDRData = newIPDRData(IPDRDATA_BOOLEAN);
									tokenValue = strtok( tokentemp, ";");

              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										strcpy(strRead, tokenValue);
										tokenValue = strtok( NULL, ";");
										strcpy((char *)pIPDRData->IPDRDataUnion_.IPDRBoolean_, strRead);
										dataValue[cnt] = (void *) calloc (1, strlen(strRead)+1);
    									strcpy((char *)dataValue[cnt], (char *)pIPDRData->IPDRDataUnion_.IPDRBoolean_);
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_BOOLEAN);
              				}
              				else {
									pIPDRData = newIPDRData(IPDRDATA_BOOLEAN);
									strcpy(strRead, tempBuff);
									strcpy((char *)pIPDRData->IPDRDataUnion_.IPDRBoolean_, strRead);
									dataValue[0] = (void *) calloc (1, strlen(strRead)+1);
    								strcpy((char *)dataValue[0], (char *)pIPDRData->IPDRDataUnion_.IPDRBoolean_);
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_BOOLEAN);
              				}
								freeIPDRData (pIPDRData);
              				break;
              
              			case IPDRDATA_SHORT: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
										decodeStringsBetweenDelims(tempBuff,'(',')');
            						tokentemp = getStringBetweenDelims(tempBuff, Findindex(tempBuff,'(') + 1 , Findrindex(tempBuff, ')') -1 );
										strcpy(strRead, tokentemp);
										token = strtok( strRead, ";");
										while(token != NULL) {
											token = strtok( NULL, ";");
											noOfElements++;
										}
									}
									else if(atoi(pcurrentMaxOccrsList->node) > 0){
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
										decodeStringsBetweenDelims(pArrayElements,'(',')');
            						tokentemp = getStringBetweenDelims(tempBuff, Findindex(tempBuff,'(') + 1 , Findrindex(tempBuff, ')') -1 );
									}
              				}
              
              				if (noOfElements > 0) {
									pIPDRData = newIPDRData(IPDRDATA_SHORT);
									tokenValue = strtok( tokentemp, ";");

              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										strcpy(strRead, tokenValue);
										tokenValue = strtok( NULL, ";");
										pIPDRData->IPDRDataUnion_.IPDRShort_ = atoi(strRead);
      								dataValue[cnt] = (void *)&pIPDRData->IPDRDataUnion_.IPDRShort_;
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_SHORT);
              				}
              				else {
									pIPDRData = newIPDRData(IPDRDATA_SHORT);
									strcpy(strRead, tempBuff);
									pIPDRData = newIPDRData(IPDRDATA_SHORT);
									pIPDRData->IPDRDataUnion_.IPDRShort_ = atoi(strRead);
									dataValue[0] = (void *)&pIPDRData->IPDRDataUnion_.IPDRShort_;
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_SHORT);
              				}
								free(pIPDRData);
                        break;
              
              			case IPDRDATA_USHORT: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
										decodeStringsBetweenDelims(tempBuff,'(',')');
            						tokentemp = getStringBetweenDelims(tempBuff, Findindex(tempBuff,'(') + 1 , Findrindex(tempBuff, ')') -1 );
										strcpy(strRead, tokentemp);
										token = strtok( strRead, ";");
										while(token != NULL) {
											token = strtok( NULL, ";");
											noOfElements++;
										}
									}
									else if(atoi(pcurrentMaxOccrsList->node) > 0){
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
										decodeStringsBetweenDelims(pArrayElements,'(',')');
            						tokentemp = getStringBetweenDelims(tempBuff, Findindex(tempBuff,'(') + 1 , Findrindex(tempBuff, ')') -1 );
									}
              				}
              
              				if (noOfElements > 0) {
									pIPDRData = newIPDRData(IPDRDATA_USHORT);
									tokenValue = strtok( tokentemp, ";");

              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										strcpy(strRead, tokenValue);
										tokenValue = strtok( NULL, ";");
										pIPDRData->IPDRDataUnion_.IPDRUShort_ = atoi(strRead);
      								dataValue[cnt] = (void *)&pIPDRData->IPDRDataUnion_.IPDRUShort_;
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_USHORT);
              				}
              				else {
									pIPDRData = newIPDRData(IPDRDATA_USHORT);
									strcpy(strRead, tempBuff);
									pIPDRData = newIPDRData(IPDRDATA_USHORT);
									pIPDRData->IPDRDataUnion_.IPDRUShort_ = atoi(strRead);
      							dataValue[0] = (void *)&pIPDRData->IPDRDataUnion_.IPDRUShort_;
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_USHORT);
              				}
								free(pIPDRData);
                        break;
              
              			default:
              					return(IPDR_NON_PRIMITIVE_DATA_TYPE);
              					break;
                      }
				} else if(lreturn == IPDR_FALSE){
					pCurrServiceList = pTempOldListServiceAttributeDescriptor;
				   pListServiceAttributeDescriptor = pTempOldListServiceAttributeDescriptor;

					token = strtok(tempBuff,";");
					decodeStringsBetweenDelims(token, '[',']');						
            	token = getStringBetweenDelims(token, Findindex(token,'[') + 1 , Findrindex(token, ']') -1 );
      			decodeSpecialChar(token);
					generateStructureData(token,
									pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_,
   								pAttributeDescriptor,
									pListServiceAttributeDescriptor,
									pListIPDRComplex,
   								pListIPDRData,
   								pTempFNFType
								);
				}
				  break;
			}
			pOldListServiceAttributeDescriptor = pOldListServiceAttributeDescriptor->pNext_;
		}
					pcurrent = pcurrent->pNext;
					if(pcurrentMaxOccrsList != NULL)
						pcurrentMaxOccrsList = pcurrentMaxOccrsList->pNext;
        k = 0;
			i++;
        while ((tempArrayElements[i] != ';') && (tempArrayElements[i] != '\0')) {
         tempBuff[k] = tempArrayElements[i];
         i++;
         k++;
      }
      tempBuff[k] = '\0';
        j++; 
			}
        j++; 
			}
		}		
		pListServiceAttributeDescriptor = pListServiceAttributeDescriptor->pNext_;
		i = 0;
	}
	
   pListServiceAttributeDescriptor = pCurrentListServiceAttributeDescriptor;

	free(strRead);
	free(strArrayElemnts);
	free(tempString1);
	free(tempComplexType1);

	return (IPDR_OK);
}


char* encodeStringsBetweenDelims(char *string, char startDelim, char endDelim)
{
    int counter = 0;
    char* delim = "[]";
    char* delimIndex;
    char* startIndex ;
    char* endIndex;
    char* preDelim;
    char* postDelim;
    char* toEncode;
    char* val;
                                                                                                                             
    delimIndex = Findindex(string, startDelim);
                                                                                                                             
    while (delimIndex != NULL)
    {
        startIndex = delimIndex + 1;
        endIndex   = Findindex(startIndex, endDelim);
                                                                                                                             
        preDelim = getStringBetweenDelims(string, NULL, startIndex -2 );
        toEncode = getStringBetweenDelims(string, startIndex - 1, endIndex);
        postDelim = getStringBetweenDelims(string, endIndex +1 ,  NULL);
                                                                                                                             
        strcpy(string , (strcat( strcat(preDelim, (const char*)encodeStr(toEncode)), (const char*)postDelim)));
        delimIndex = Findindex(endIndex + 1 , startDelim);
                                                                                                                             
        free(preDelim);
        free(toEncode);
        free(postDelim);
    }
    return string;
}


char* encodeStr(char* sourceString)
{
    int count = 0;
    char* preDelim;
    char* postDelim;
    char* delimIndex;
                                                                                                                             
    delimIndex = Findindex(sourceString, '%');
    while (delimIndex != NULL)
    {
           preDelim = getStringBetweenDelims(sourceString, NULL, delimIndex - 1);
           postDelim = getStringBetweenDelims(sourceString,delimIndex+1, NULL);
           strcpy(sourceString, strcat(strcat(preDelim, "%%"), postDelim));
                                                                                                                             
           delimIndex = Findindex(delimIndex+2, '%');
           break;
    }
                                                                                                                             
    while (sourceString[count] != '\0')
    {
       if(sourceString[count] == ';')
           sourceString[count] = '%';
       count++;
    }
                                                                                                                             
    return sourceString;
}

char* decodeStringsBetweenDelims(char *string, char startDelim, char endDelim)
{
    int counter = 0;
    char* delim = "[]";
    char* delimIndex;
    char* startIndex ;
    char* endIndex;
    char* preDelim;
    char* postDelim;
    char* toDecode;
    char* val;
                                                                                                                             
    delimIndex = Findindex(string, startDelim);
                                                                                                                             
    while (delimIndex != NULL)
    {
        startIndex = delimIndex + 1;
        endIndex   = Findindex(startIndex, endDelim);
                                                                                                                             
        preDelim = getStringBetweenDelims(string, NULL, startIndex -2 );
        toDecode = getStringBetweenDelims(string, startIndex - 1, endIndex);
        postDelim = getStringBetweenDelims(string, endIndex +1 ,  NULL);
                                                                                                                             
        strcpy(string , (strcat( strcat(preDelim, (const char*)decodeStr(toDecode)), (const char*)postDelim)));
        delimIndex = Findindex(endIndex + 1 , startDelim);
                                                                                                                             
        free(preDelim);
        free(toDecode);
        free(postDelim);
    }
    return string;
}

char* decodeStr(char* sourceString)
{
    int count = 0;
    char* delimIndex;
    char* preDelim;
    char* postDelim;
                                                                                                                             
    while (sourceString[count] != '\0')
    {
        if(sourceString[count] == '%' && sourceString[count + 1] == '%')
        {
            count = count + 2;
        }
        else if(sourceString[count] == '%' && sourceString[count + 1] != '%')
        {
            sourceString[count] = ';';
            count++;
        }
        else
            count++;
    }
    delimIndex = strstr(sourceString, "%%");
    while (delimIndex != NULL)
    {
           preDelim = getStringBetweenDelims(sourceString, NULL, delimIndex - 1);
           postDelim = getStringBetweenDelims(sourceString,delimIndex+2, NULL);
           strcpy(sourceString, strcat(strcat(preDelim, "%"), postDelim));
                                                                                                                             
           delimIndex = Findindex(delimIndex+2, '%');
           break;
    }
    return sourceString;
}

char* encodeSpecialChar(char* str)
{
    /* First encode all special characters */

    str = encodeCharacter(str, '[');
    str = encodeCharacter(str, ']');
    str = encodeCharacter(str, '(');
    str = encodeCharacter(str, ')');
    str = encodeCharacter(str, ';');
    str = encodeCharacter(str, ',');
    str = encodeCharacter(str, '\\');

    return str;
}

char* encodeCharacter(char *string, char delim)
{
    int counter = 0;
    char* delimIndex;
    char* startIndex ;
    char* endIndex;
    char* preDelim;
    char* postDelim;
    char* toEncode;
    char* val;
    char* escapedChar;
    char interimStr[3];

    interimStr[0] = '\\';
    interimStr[1] = delim;
    interimStr[2] = '\0';


    delimIndex = strstr(string, interimStr);

    while (delimIndex != NULL)
    {
        startIndex = delimIndex;
        endIndex   = delimIndex + 2;

        preDelim = getStringBetweenDelims(string, NULL, startIndex - 1);
        toEncode = getStringBetweenDelims(string, startIndex, endIndex - 1);
        postDelim = getStringBetweenDelims(string, endIndex,  NULL);

        strcpy(string , (strcat( strcat(preDelim, (const char*)encodeEscapedStr(toEncode, delim)), (const char*)postDelim)));
        delimIndex = strstr(endIndex , interimStr);
        free(preDelim);
        free(toEncode);
        free(postDelim);

    }
    return string;
}

char* encodeEscapedStr(char* toEncode, char delim)
{
            switch(delim)
            {
            case '[':
                  toEncode[1] = '+';
                  break;
            case ']':
                  toEncode[1] = '-';
                  break;
            case '(':
                  toEncode[1] = '@';
                  break;
            case ')':
                  toEncode[1] = '#';
                  break;
           case ';':
                  toEncode[1] = '=';
                  break;
           case ',':
                  toEncode[1] = '$';
                  break;
           case '\\':
                  toEncode[1] = '|';
                  break;
           default:
                  break;
           }
   toEncode[2] = '\0';
   return toEncode;

}

char* decodeSpecialChar(char* str)
{
    str = decodeCharacter(str, '|');
    str = decodeCharacter(str, '$');
    str = decodeCharacter(str, '=');
    str = decodeCharacter(str, '#');
    str = decodeCharacter(str, '@');
    str = decodeCharacter(str, '-');
    str = decodeCharacter(str, '+');

    return str;
}

char* decodeCharacter(char *string, char delim)
{
    int counter = 0;
    char* delimIndex;
    char* startIndex ;
    char* endIndex;
    char* preDelim;
    char* postDelim;
    char* toEncode;
    char* val;
    char* escapedChar;
    char interimStr[3];
    char tail[2];

    tail[0] = delim;
    tail[1] = '\0';

    interimStr[0] = '\\';
    interimStr[1] = delim;
    interimStr[2] = '\0';


    delimIndex = strstr(string, interimStr);

    while (delimIndex != NULL)
    {
        startIndex = delimIndex;
        endIndex   = delimIndex + 2;

        preDelim = getStringBetweenDelims(string, NULL, startIndex - 1);
        toEncode = getStringBetweenDelims(string, startIndex, endIndex - 1);
        postDelim = getStringBetweenDelims(string, endIndex,  NULL);

        strcpy(string , (strcat( strcat(preDelim, (const char*)decodeEscapedStr(toEncode, delim)), (const char*)postDelim)));
        delimIndex = strstr(endIndex, interimStr);
        free(preDelim);
        free(toEncode);
        free(postDelim);

    }
    return string;
}

char* decodeEscapedStr(char* toDecode, char delim)
{
            switch(delim)
            {
            case '+':
                  toDecode[0] = '[';
                  break;
            case '-':
                  toDecode[0] = ']';
                  break;
            case '@':
                  toDecode[0] = '(';
                  break;
            case '#':
                  toDecode[0] = ')';
                  break;
           case '=':
                  toDecode[0] = ';';
                  break;
           case '$':
                  toDecode[0] = ',';
                  break;
           case '|':
                  toDecode[0] = '\\';
                  break;
           default:
                  break;
           }
   toDecode[1] = '\0';
   return toDecode;

}

char* Findindex(char *cPtr, char c)
{
	int lFound = 0;

   while(*cPtr != '\0')
   {
      if(*cPtr == c) {
			lFound = 1;
      	break;
		} else {
      	cPtr++;
		}
    }

	 if(lFound == 0) 
		return NULL;
	 else
    	return cPtr;
}

char * Findrindex (char *s, int c)
{
  return strrchr (s, c);
}

/*
char* Findrindex(char *cPtr, char c)
{
	int lFound = 0;
   char* ltempPtr;
   int inner, outer= strlen(cPtr)-2;
   char temp;

	ltempPtr = cPtr;
   for (inner=0; inner < outer; inner++, outer--)
   {   
      temp = s[inner];
      s[inner] = s[outer];
      s[outer] = temp;
   }

   while(*cPtr != '\0')
   {
      if(*cPtr == c) {
			lFound = 1;
      	break;
		} else {
      	cPtr++;
		}
    }

	 if(lFound == 0) 
		return NULL;
	 else
    	return cPtr;
}
*/
