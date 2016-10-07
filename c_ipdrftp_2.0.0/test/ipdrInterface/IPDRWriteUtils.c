/**
 * Copyright (C) 2002, The IPDR Organization, all rights reserved.
 * The use and distribution of this software is governed by the terms of
 * the license agreement which can be found in the file LICENSE.TXT at
 * the top of this source tree.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND, either express or implied.
 *
 *
 * File                 : IPDRWriteUtils.c			         
 * Description          : Contains functions required by read/write interface.     
 * Author               : Infosys Tech Ltd           
 * Modification History :                            
 *---------------------------------------------------
 * Date       Name             Change/Description    
 *---------------------------------------------------
 * 01/29/02  Infosys           Created  
 *
 */


#include "ipdrInterface/IPDRWriteTool.h"


/*!
 *  NAME:
 *      populateWriteIPDRCommonParam() - Function to populate 
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

int populateWriteIPDRCommonParam(
								IPDRCommonParameters* pIPDRCommonParameters,
								int* pErrorCode
								)
{
	int numOfBuckets = 9;
	//!
	if(pIPDRCommonParameters == NULL) {
	    *pErrorCode = IPDR_NULL_INPUT_PARAMETER;
	    return (IPDR_ERROR);
	}
	//!

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
	//!
	unsigned int lenFileName = 0;
	//!
	
	char* nameSpaceURI;
	char* nSpaceURI;
	char* schemaURI;
	char* pNameSpaceID;
	char* URI;
	char* attributeNames;
	char* strFileContents = NULL;
	char* fileNames = NULL;
	char* readString;
	char* token;
	char seps[]   = ",";
	//!
	unsigned int fileLength = 0;
	List* pCurrent = NULL;
	//!

	FILE* fp;

	if(pFileNamesList == NULL) {
	    *pErrorCode = IPDR_NULL_INPUT_PARAMETER;
	    return (IPDR_ERROR);
	}
/*!
	fileNames = (char *) calloc(MAXIMUM_NODE_SIZE, sizeof(char));           
	strFileContents = (char *) calloc(MAXIMUM_NODE_SIZE, sizeof(char));
!*/
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

	//!
	pCurrent = pFileNamesList;
	//!
	/* Read all the input files into strings */
	for (countNumFiles = 1; countNumFiles <= numOfFiles; countNumFiles++) {
		//!
		lenFileName = strlen(pCurrent->node) + 1;
		fileNames = (char *) calloc (lenFileName, sizeof(char));
		if (fileNames == NULL) {
			*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
			return(IPDR_ERROR);
		}
		//!
		getNode(pFileNamesList, countNumFiles, fileNames);
		//!
		fileLength = getFileLength(fileNames) + 1;
		strFileContents = (char *) calloc (fileLength, sizeof(char));
		if (strFileContents == NULL) {
			*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
			return(IPDR_ERROR);
		}
		//!
		if((fp = fopen(fileNames, "r"))==NULL){
			*pErrorCode = IPDR_UNREADABLE_ISTREAM;
			return 1;
		}
			
    	/* Reading ServiceDefinitionURI and NameSpaceURI from the 
		input file */
		fscanf(fp,"%s",	readString);
		strcpy(	schemaURI , strtok(readString, seps));
		#ifdef DEBUG_IPDR
			printf( "\nschemaURI(URI)= %s\n", schemaURI );
		#endif
		appendListServiceDefinitionURI(pListServiceDefinitionURI, 
			schemaURI);

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
	
		appendListNameSpaceInfo(pListNameSpaceInfo, pNameSpaceID,
					URI);

		fscanf(fp,"%s",attributeNames);				
		getc(fp);
		getc(fp);
		/* Read the records from the files*/
		for (countFileChars = 0;
			(strFileContents[countFileChars] = getc(fp)) != EOF; 
			countFileChars++);
		strFileContents[countFileChars]='\0';	
		appendNode(pFileList, strFileContents);	

		fclose(fp);
		//!
		free(fileNames);
		pCurrent = pCurrent->pNext;
		//!
	}

	free(URI);
	free(pNameSpaceID);
	free(schemaURI);
	free(nameSpaceURI);
    free(attributeNames);
	/*!
	free(strFileContents);
	free(fileNames);
	!*/
	free(readString);
	free(nSpaceURI);

	return(IPDR_OK);
}


/*!
 *  NAME:
 *      validateWriteInputParams() - This function validates the Input 
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


int validateWriteInputParams(
						int argCount, 
						char* argValues[],
						unsigned int* numOfFiles,
						unsigned int* robFlag,
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
						#ifdef DEBUG_IPDR 
							printf("IPDR_MISSING_OR_INVALID_OUTFILE");
						#endif
						return(IPDR_ERROR);
					} 
					else {
						//!
						lenOutFile = strlen(argValues[countArguments+1]) + 1;
						outFileName = (char *) calloc (lenOutFile, sizeof(char));
						if (outFileName == NULL) {
							*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
							return(IPDR_ERROR);
						}
						//!
						strcpy(outFileName,argValues[countArguments+1]);
						/* Sets the outFileFlag if the file parameter is 
						present on the comandline*/
						outFileFlag = 1;
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
							#ifdef DEBUG_IPDR 
								printf("\nIPDR_INVALID_RECORDCOUNT\n");
							#endif
							*pErrorCode = IPDR_INVALID_RECORDCOUNT;
							return(IPDR_ERROR);
						}
						countFileChars++;
					}
					*recCount=atoi(argValues[countArguments+1]); 
						
					if (*recCount<0) {
						#ifdef DEBUG_IPDR 
							printf("\nIPDR_INVALID_RECORDCOUNT\n");
						#endif
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
					//!
					unsigned int fileLength = 0;
					fileLength = strlen(argValues[countArguments]) + 1;
					str = (char *) calloc (fileLength, sizeof(char));
					fileName = (char *) calloc (fileLength, sizeof(char));
					//!

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
			}

}


    /* Checks if number of valid input parameters is same as the number of 
     input parameters*/

	if ((countValidArgs <= (unsigned int) (argCount-1))||(argCount == 1)) {
		#ifdef DEBUG_IPDR 
			printf("\nIPDR_MISSING_OR_INVALID_COMMANDLINE_PARAMS");
		#endif
		printWriteUsage();
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

		if (outFileName != NULL)
			lenStreamName = strlen(outFileName) + 1;
		else 
			lenStreamName = strlen(IPDR_NULL) + 1;

        pIPDRCommonParameters->pStreamName_ = (char *) calloc
            (lenStreamName, sizeof(char));

        if (outFileName != NULL) 
			strcpy(pIPDRCommonParameters->pStreamName_, outFileName);
		else 
			strcpy(pIPDRCommonParameters->pStreamName_, IPDR_NULL);

		if ((pIPDRCommonParameters->pStreamHandle_ = 
		         fopen(outFileName, "w+")) == NULL) {
			*pErrorCode = (IPDR_INVALID_FILE);
			return(IPDR_ERROR);
		
	} 

}
	free(strTemp);
	free(tempString);
	free(docTypeExt);
	if(outFileName)
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
	/*!	
		if (nodeLength > (*maxValue)) 
			*maxValue=nodeLength;
	!*/
		for (count = 0; count <= (strlen(strFileContents)); count++) {
			if((strFileContents[count] == '\n') ||
				(strFileContents[count] == '\0')) {
				rec++;
			}
		}
		maxrecords = rec - 1;
		if (maxrecords > (*maxValue)) 
			*maxValue = maxrecords;

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
	   	#ifdef DEBUG_IPDR
		{
			printExceptionStructure (pIPDRCommonParameters->pExceptionStructure_); 
		}
		#endif
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
	   	#ifdef DEBUG_IPDR
		{
			printExceptionStructure (pIPDRCommonParameters->pExceptionStructure_); 
		}
		#endif
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
	   	#ifdef DEBUG_IPDR
		{
			printExceptionStructure (pIPDRCommonParameters->pExceptionStructure_); 
		}
		#endif
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
	   #ifdef DEBUG_IPDR
	   {
			printExceptionStructure (pIPDRCommonParameters->pExceptionStructure_); 
	   }
	   #endif
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
	count = (unsigned int *) calloc (MAX_ATTRIBUTE_LENGTH, sizeof(int));
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
			// freeFNFType(pFNFType[countFile]);
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
	count = (unsigned int *) calloc (MAX_ATTRIBUTE_LENGTH, sizeof(int));
	
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
		// freeFNFType(pFNFType[countFile]);
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
		// freeFNFType(pFNFType[countFile]);
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

	pFNFType = (FNFType**) calloc (numOfFiles, sizeof(FNFType*));
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
	int count = 0;
	int countFileChars = 0 ;
	int countListNameSpaceInfo = 0;
	int errorSeverity = 0;
	char* token = NULL;
	char* strRead = NULL;
	char* tempString = NULL;
	char* stopstring = NULL;
	char* pNameSpcID = NULL;
	char* pNameSpaceID = NULL;
	char* pNameSpaceURI = NULL;
	char* pAttributeName = NULL;
	char* strAttributeName = NULL;
	char seps[]   = "$";

	FNFData* pFNFData;
	ListIPDRData* pListIPDRData = NULL;
	IPDRData* IPDRData = NULL;
	ListAttributeDescriptor* pCurrentListAttrDesc = NULL;
	ListNameSpaceInfo* pCurrentListNameSpaceInfo = NULL;

    void* IPDRValue =  NULL;
	int isPresentFlag = 0;
	FNFType* pTempFNFType = NULL;

	ListAttributeDescriptor* pFirstAttrDesc = NULL;
	ListAttributeDescriptor* pFirstListAttributeDescriptor = NULL;

	if(strTemp == NULL || pIPDRCommonParameters == NULL ||
	 pFNFType == NULL) {
	    *pErrorCode = IPDR_NULL_INPUT_PARAMETER;
	    return (IPDR_ERROR);
	}


	strRead = (char *) calloc(MAX_IPDR_STRING, sizeof(char));
	tempString = (char *) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char));

	pFNFData = newFNFData();
	pTempFNFType = newFNFType();

	if(strRead == NULL || tempString == NULL || pFNFData == NULL ||
		pTempFNFType == NULL) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		return(IPDR_ERROR);
	}

	strcpy(pTempFNFType->serviceType_,pFNFType->serviceType_);

	pFirstListAttributeDescriptor = pFNFType->pListAttributeDescriptor_;	
	
	while (pFNFType->pListAttributeDescriptor_ != NULL) {
		appendListAttributeDescriptor(&pCurrentListAttrDesc,
										pFNFType->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
										pFNFType->pListAttributeDescriptor_->pAttributeDescriptor_->attributeType_,
										pFNFType->pListAttributeDescriptor_->pAttributeDescriptor_->description_,
										pFNFType->pListAttributeDescriptor_->pAttributeDescriptor_->derivedType_,
	  				 pFNFType->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_,
	  			 	 pFNFType->pListAttributeDescriptor_->pAttributeDescriptor_->pListAttrNameList_,
     				 pFNFType->pListAttributeDescriptor_->pAttributeDescriptor_->pListAttrClassList_,
     				 pFNFType->pListAttributeDescriptor_->pAttributeDescriptor_->pListAttrMaxOccursList_,
     				 pFNFType->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_
									 );
		pFNFType->pListAttributeDescriptor_ = pFNFType->pListAttributeDescriptor_->pNext_; 
	}
	
	pFirstAttrDesc = pCurrentListAttrDesc;
	
	pFNFType->pListAttributeDescriptor_ = pFirstListAttributeDescriptor;	

		pCurrentListAttrDesc = pFNFType->pListAttributeDescriptor_;
	 	
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
	
	
		for (strIndex = 0,isPresentFlag = 0; (strTemp[countFileChars] != ',') && 
			(strTemp[countFileChars] != '\n') && 
			(strTemp[countFileChars] != '\0');
		     strIndex++, countFileChars++,isPresentFlag++) {
				strRead[strIndex] = strTemp[countFileChars];
		}
		strRead[strIndex]='\0';				

		pNameSpaceID = (char *) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char));
		pNameSpcID = (char *) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char));
		pNameSpaceURI = (char *) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char));

		if(pNameSpaceID == NULL || pNameSpaceURI == NULL || pNameSpcID == NULL) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		//free (pAttributeName);
		free (strAttributeName);
		return(IPDR_ERROR);
		}

		convertToUpper((char*)pCurrentListAttrDesc->
			pAttributeDescriptor_->attributeType_, tempString);
		
		/* Collect all the non-blank attributes */
		if(!(isPresentFlag == 0)) 
		{
				if (strcmp("INTEGER", tempString) == 0) {

				IPDRData = newIPDRData(IPDRDATA_LONG);
				IPDRData->IPDRDataUnion_.IPDRLong_ = atoi(strRead);
				appendListIPDRData(&pListIPDRData, 
					(void *) IPDRData->IPDRDataUnion_.IPDRLong_, 
					IPDRDATA_LONG);
				freeIPDRData(IPDRData);

				}
				else if (strcmp("UNSIGNED_INTEGER", tempString) == 0) {

				IPDRData = newIPDRData(IPDRDATA_ULONG);
				IPDRData->IPDRDataUnion_.IPDRULong_ = atoi(strRead);
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
					(float *) &IPDRData->IPDRDataUnion_.IPDRFloat_;

				appendListIPDRData(&pListIPDRData,(float*)IPDRValue, IPDRDATA_FLOAT);
				freeIPDRData(IPDRData);

				}
				else if (strcmp("DOUBLE", tempString) == 0) {

				IPDRData = newIPDRData(IPDRDATA_DOUBLE);
				IPDRData->IPDRDataUnion_.IPDRDouble_= strtod(strRead, 
					&stopstring);
				
				IPDRValue = 
					(float *) &IPDRData->IPDRDataUnion_.IPDRDouble_;

				appendListIPDRData(&pListIPDRData, (float*)IPDRValue, IPDRDATA_DOUBLE);

				freeIPDRData(IPDRData);
				}
				else if (strcmp("BYTE_ARRAY", tempString) == 0) {
					//IPDRData = newIPDRData(IPDRDATA_BYTEARRAY);
					IPDRData = newIPDRData(IPDRDATA_HEXADECARRAY);
					strcpy((char*)IPDRData->IPDRDataUnion_.IPDRByteArray_ ,strRead);
					/*appendListIPDRData(&pListIPDRData, IPDRData->
						IPDRDataUnion_.IPDRByteArray_, IPDRDATA_BYTEARRAY);*/
					appendListIPDRData(&pListIPDRData, IPDRData->
						IPDRDataUnion_.IPDRByteArray_, IPDRDATA_HEXADECARRAY);
					freeIPDRData(IPDRData);
				}
				else if (strcmp("UTF8_ARRAY", tempString) == 0) {
					//IPDRData = newIPDRData(IPDRDATA_UTF8ARRAY);
					IPDRData = newIPDRData(IPDRDATA_STRING);
					strcpy((char *)IPDRData->IPDRDataUnion_.IPDRUTF8Array_ ,strRead);
					/*appendListIPDRData(&pListIPDRData, IPDRData->
						IPDRDataUnion_.IPDRUTF8Array_, IPDRDATA_UTF8ARRAY);*/
					appendListIPDRData(&pListIPDRData, IPDRData->
						IPDRDataUnion_.IPDRUTF8Array_, IPDRDATA_STRING);
//					freeIPDRData(IPDRData);
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
					strcat(pNameSpcID, ":" );
					strcat(pNameSpcID, pAttributeName);
					strcpy(pAttributeName, pNameSpcID);
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

	/*
	pCurrentListAttrDesc = pFirstAttrDesc;
	freeListAttributeDescriptor (&pCurrentListAttrDesc);
	*/
           
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


	freeFNFType(pTempFNFType);
	freeListIPDRData (&pListIPDRData);
	free(strRead);
	free(tempString);
	return (IPDR_OK);
}

	 
/*!
 *  NAME:
 *      printWriteUsage() - Prints test toll usage syntax. 
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

int printWriteUsage()
{
	printf("\nCorrect Input String is: -doctype XDR/XML -outfile OutputFileName <-recordcount> <No Of Records> <-roundrobin> TestDataFile1 <TestDataFile2> <TestDataFile3>");
	return (IPDR_OK);
}
	
