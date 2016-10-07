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
 * File                 : IPDRReadUtils.c			         
 * Description          : Contains functions required by read/write interface.     
 * Author               : Infosys Tech Ltd           
 * Modification History :                            
 *---------------------------------------------------
 * Date       Name             Change/Description    
 *---------------------------------------------------
 * 01/29/02  Infosys           Created  
 *
 */


#include "ipdrInterface/IPDRReadTool.h"

/*!
 *  NAME:
 *      generateRawData() - This function validates the Input 
 *					             Command Line Arguments 
 *  DESCRIPTION:
 *      This function converts the FNFData back into raw data in the form of
 *        csv file
 *  
 *  ARGUMENTS:
 *		
 *			pIPDRCommonParameters   (IN)
 *			pFNFData                (IN)
 *          pListServiceInfo        (IN)
 *          pErrorCode               (IN/OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */



int generateRawData(
					IPDRCommonParameters* pIPDRCommonParameters,
					FNFData* pFNFData,
					ListServiceInfo* pListServiceInfo,
					int* pErrorCode
					)
{
	int matchFlag = 0;
	int errorSeverity = 0;
	int countListIPDRData = 0;
	int countListAttrDesc = 0;
	int countListServInfo = 0;
	int countListTemp = 0;
	int lengthCurrentServInfo = 0;
	int lengthAttrDesc = 0;
	int count = 0;
	int countListNameSpaceInfo = 0;
	int commaCount = 0;
	int countRec = 0;

	char* tempString;
	char* token1;
	char seps1[]   = "$";
	char seps2[]   = ":";
	char* pAttributeName;
	char* strAttributeName;
	char* pServInfoAttrName;
	char* strServInfoAttrName;
	char* pNameSpaceURI;
	char* pNameSpaceID;

	ListIPDRData* pCurrentIPDRData = NULL;
	ListAttributeDescriptor* pCurrentAttrDesc = NULL;
	ListAttributeDescriptor* pCurrentServInfo = NULL;
	ListServiceInfo* pCurrent = NULL;
	ListNameSpaceInfo* pCurrentListNameSpaceInfo = NULL;

	if(pIPDRCommonParameters == NULL || 
		pListServiceInfo == NULL ||
		pFNFData == NULL) {

	    *pErrorCode = IPDR_NULL_INPUT_PARAMETER;
	    return (IPDR_ERROR);
	}

    tempString = (char*) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char));

	if(tempString == NULL) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		return(IPDR_ERROR);
	}
	
	errorSeverity = readIPDR(pIPDRCommonParameters, &pFNFData, pErrorCode);
	if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, "readIPDR"); 
	}
	else if (errorSeverity == IPDR_ERROR) {
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "readIPDR"); 
		return (IPDR_ERROR);
	}

	pCurrent = pListServiceInfo;
 
	for (countListTemp = 0; countListTemp < getLengthListServiceInfo
		(pListServiceInfo); countListTemp++) {
	
	
		while ((pFNFData != NULL) &&
			(strcmp(pFNFData->typeName_, 
			pCurrent->pServiceInfo_->pFNFType_->serviceType_) == 0)) {
				
			
		#ifdef DEBUG_IPDR  
		{
			printf("\n\n\n\nPrinting FNFDATA");
			printFNFData(pFNFData);
 			printf("\n\n\n\n Finished Printing FNFDATA");	
		}
		#endif
		
							
		if(strcmp(pFNFData->typeName_,
 			pCurrent->pServiceInfo_->pFNFType_->serviceType_) == 0) {
					
		pCurrentServInfo = pCurrent->pServiceInfo_->pFNFType_->
					                            pListAttributeDescriptor_;
			
 		pCurrentIPDRData = pFNFData->pListIPDRData_;
			
		lengthCurrentServInfo = getLengthListAttributeDescriptor
			(pCurrentServInfo); 

		commaCount = 0;

		for (countListServInfo = 0; countListServInfo < 
			lengthCurrentServInfo; countListServInfo++) {	
						
			matchFlag=0;
			pCurrentAttrDesc = pFNFData->pListAttributeDescriptor_;
					
			lengthAttrDesc = getLengthListAttributeDescriptor
				(pCurrentAttrDesc);
		
			for (countListAttrDesc = 0; 
			(countListAttrDesc < lengthAttrDesc) && (matchFlag != 1); 
			countListAttrDesc++) {


			/* 
			Getting the atribute name from  the List Serv Info and the 
			IPDR Data
			*/
			pServInfoAttrName = (char *) calloc(MAX_ATTRIBUTE_LENGTH, 
				sizeof(char));
	    	strServInfoAttrName = (char *) calloc(MAX_ATTRIBUTE_LENGTH, 
				sizeof(char));
			pAttributeName = (char *) calloc(MAX_ATTRIBUTE_LENGTH, 
				sizeof(char));
	    	strAttributeName = (char *) calloc(MAX_ATTRIBUTE_LENGTH, 
				sizeof(char));
			pNameSpaceURI = (char *) calloc(MAX_ATTRIBUTE_LENGTH,
				sizeof(char));
			pNameSpaceID = (char *) calloc(MAX_ATTRIBUTE_LENGTH, 
				sizeof(char));

			if(pServInfoAttrName == NULL || strServInfoAttrName == NULL ||
				pAttributeName == NULL || strAttributeName == NULL ||
				pNameSpaceURI == NULL) {
				
				*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
				return(IPDR_ERROR);
			}
	
			strcpy(strServInfoAttrName,	
				 pCurrentServInfo->pAttributeDescriptor_->attributeName_);

			count = 0;
			token1 = strtok( strServInfoAttrName, seps1 );
			while( token1 != NULL ) {
					if(count==1)
						 strcpy(pNameSpaceURI,token1);
					if(count==2)
					strcpy(pServInfoAttrName,token1);
					token1 = strtok( NULL, seps1 );
					count++;
			}	



			/*Getting the name space ID from the ListNamespaceInfo*/
			pCurrentListNameSpaceInfo = pCurrent->pServiceInfo_->pListNameSpaceInfo_;

			for (countListNameSpaceInfo = 0; 
				countListNameSpaceInfo < getLengthListNameSpaceInfo
				(pCurrent->pServiceInfo_->pListNameSpaceInfo_); 
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
			 
				strcpy(pAttributeName,pNameSpaceID);
				strcat(pAttributeName, ":");
				strcat(pAttributeName, pServInfoAttrName);
		   }
		   else {
				strcpy(pAttributeName,pServInfoAttrName);
		   }
			    

			if(strcmp(pCurrentAttrDesc->pAttributeDescriptor_->attributeName_, 
				pAttributeName) == 0) {

				convertToUpper((char*)pCurrentAttrDesc->
					pAttributeDescriptor_->attributeType_, tempString);
										
				if (strcmp("INTEGER", tempString) == 0) {
					fprintf(pCurrent->pServiceInfo_->fp,"%ld", 
					pCurrentIPDRData->IPDRDataUnion_.IPDRLong_);		
		
				}
				else if (strcmp("UNSIGNED_INTEGER", tempString) == 0) {
					fprintf(pCurrent->pServiceInfo_->fp,"%ld", 
					pCurrentIPDRData->IPDRDataUnion_.IPDRULong_);
					
				}
				else if (strcmp("HYPER", tempString) == 0) {
					 fprintf(pCurrent->pServiceInfo_->fp,"%.lf", 
					 (double) (pCurrentIPDRData->IPDRDataUnion_.IPDRLongLong_));
				 

				}
				else if (strcmp("UNSIGNED_HYPER", tempString) == 0) {
					  fprintf(pCurrent->pServiceInfo_->fp,"%.lf", 
					  (pCurrentIPDRData->IPDRDataUnion_.IPDRULongLong_));
					  

				}
				else if (strcmp("FLOAT", tempString) == 0) {
					  fprintf(pCurrent->pServiceInfo_->fp,"%f", 
					  pCurrentIPDRData->IPDRDataUnion_.IPDRFloat_);
					  

				}
				else if (strcmp("DOUBLE", tempString) == 0) {
					 fprintf(pCurrent->pServiceInfo_->fp,"%lf", 
					 pCurrentIPDRData->IPDRDataUnion_.IPDRDouble_);
					 
				}
				else if (strcmp("BYTE_ARRAY", tempString) == 0) {
					  fprintf(pCurrent->pServiceInfo_->fp,"%s", 
					  pCurrentIPDRData->IPDRDataUnion_.IPDRByteArray_);
					  

				}

				else if (strcmp("UTF8_ARRAY", tempString) == 0) {
					  fprintf(pCurrent->pServiceInfo_->fp,"%s", 
					  pCurrentIPDRData->IPDRDataUnion_.IPDRUTF8Array_);		  
					  
				}


				if(commaCount < lengthCurrentServInfo - 1) {
					 fprintf(pCurrent->pServiceInfo_->fp,"%c",',');	
					 	commaCount++;
				}
			
          						
				matchFlag = 1 ;	
				pCurrentIPDRData = pCurrentIPDRData->pNext_;				
				
				}	
				free(pServInfoAttrName);
				free(strServInfoAttrName);
				free(pAttributeName);
				free(strAttributeName);
				free(pNameSpaceURI);
				free(pNameSpaceID);

				pCurrentAttrDesc = pCurrentAttrDesc->pNext_;		
				}

				if((matchFlag != 1) && (commaCount < lengthCurrentServInfo - 1)) {			
				  fprintf(pCurrent->pServiceInfo_->fp,
						  "%c",',');
				  commaCount++;
				}
				pCurrentServInfo = pCurrentServInfo->pNext_;
			}	
		}

		fprintf(pCurrent->pServiceInfo_->fp, "\n");	

		freeFNFData(pFNFData);
		
		pFNFData = newFNFData();
		
		errorSeverity = readIPDR(pIPDRCommonParameters, &pFNFData, pErrorCode);
		if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "readIPDR"); 
		}
		else if (errorSeverity == IPDR_ERROR) {
			exceptionHandler(pIPDRCommonParameters, pErrorCode, "readIPDR"); 
			return (IPDR_ERROR);
		}
		printf("\t[%d]", countRec);
		countRec++;
	
	} 
		if((countListTemp == getLengthListServiceInfo(pListServiceInfo) - 1)
			&& (pFNFData!=NULL) ) {
			 pCurrent = pListServiceInfo;
			countListTemp = -1;
		}
		else {
			pCurrent = pCurrent->pNext_;
		}			

	}
	
	for (countListTemp = 0; countListTemp < getLengthListServiceInfo
		(pListServiceInfo); countListTemp++) {
		pListServiceInfo->pServiceInfo_->fp;
	}
	
	free(tempString);

	return (IPDR_OK);
} 

/*!
 *  NAME:
 *      validateReadInputParams() - This function validates the Input           
 *					             Command Line Arguments 
 *  DESCRIPTION:
 *      This function validates the Input Command Line Arguments for this 
 *       test tool.
 *  
 *  ARGUMENTS:
 *		
 *			argCount				 (IN)
 *			argValues				 (IN)
 *			schemaValFlag			 (IN/OUT)
 *			pIPDRCommonParameters    (IN/OUT)
 *			pErrorCode				 (IN/OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


int validateReadInputParams(
						int argCount, 
						char* argValues[],
						unsigned int* schemaValFlag,
						unsigned int* outDirFlag,
						char* outFileDir,
						IPDRCommonParameters* pIPDRCommonParameters,
						int* pErrorCode
					   )
{
	unsigned int countFileChars = 0;
	unsigned int countArguments = 0;
	unsigned int countValidArgs = 0;
	unsigned int lengthInFile = 0;
	unsigned int lenDocType = 0;
	char* docType;
	char* inFileName = NULL;
	char* tempString;
	char* strTemp;

	if(argValues == NULL || schemaValFlag == NULL ||
	   outDirFlag == NULL || outFileDir == NULL || 
	    pIPDRCommonParameters == NULL) {
		
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
	    return (IPDR_ERROR);
	}



	tempString = (char*) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char));
	strTemp = (char*) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char));

	if(tempString == NULL || strTemp == NULL ) {

		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		return(IPDR_ERROR);
	}
		
	/* Validate for commandline parameters */

	for	(countArguments = 1, countValidArgs=1;
		(countArguments < (unsigned int)(argCount));
		countArguments++) {
		
		/*Catches the commandline parameter -schemavalidation*/
		convertToUpper(argValues[countArguments], tempString);
		if ((strcmp(tempString,	"-SCHEMAVALIDATION")) == 0) {
			*schemaValFlag = 1;
			countValidArgs++;






		}
		/*
		Catches the commandline parameter -doctype Also reads the 
		value for doctype from the next commandline parameter
		*/
		if ((strcmp(tempString,"-DOCTYPE")) == 0) {
			if (!(countArguments == (unsigned int)(argCount-1))) {
	                        lenDocType = strlen(argValues[countArguments+1]) + 1;
				docType = (char *) calloc (lenDocType, sizeof(char));
				if (docType == NULL) {
					*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
					return(IPDR_ERROR);
				}
		
				strcpy(docType,argValues[countArguments+1]);

				convertToUpper(docType, strTemp);
				if (((strcmp(strTemp,IPDR_XDR)) == 0)) {
					pIPDRCommonParameters->DocType_= XDR;
					countValidArgs += 2;
					countArguments++;
				} else if (((strcmp(strTemp,IPDR_XML)) == 0)) {
					pIPDRCommonParameters->DocType_ = XML;
					countValidArgs += 2;
					countArguments++;
				}
			}
			
		}

		/*Gets the output file directory*/
		if ((strcmp(tempString,"-OUTFILEDIR")) == 0){
			*outDirFlag = 1;
	
			if(!(countArguments==(unsigned int)(argCount-1))) {
				if(argValues[countArguments+1][0]=='-') {
					*pErrorCode = IPDR_MISSING_OR_INVALID_OUTFILE;

						printf("IPDR_MISSING_OR_INVALID_OUTFILE");

					return(IPDR_ERROR);
				} 
				else {
					strcpy(outFileDir,argValues[countArguments+1]);
					countValidArgs += 2;
					countArguments++;
		
				}

			}
		}
				
		/*Catches the input file*/
		if(fopen(argValues[countArguments],"r") != NULL) {	
			countValidArgs++;
			lengthInFile = strlen(argValues[countArguments]) + 1;
			inFileName = (char *) calloc (lengthInFile, sizeof(char));
			if(inFileName == NULL) {
				*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
				return(IPDR_ERROR);
		}
	pIPDRCommonParameters->pStreamName_ = (char*) calloc((strlen(argValues[countArguments]) + 1), sizeof(char));
	

			strcpy(pIPDRCommonParameters->pStreamName_, 
                                      argValues[countArguments]);
	
			pIPDRCommonParameters->pStreamHandle_ = 
                                    fopen(pIPDRCommonParameters->pStreamName_, "r");
		}
	}
	

 /* Checks if number of valid input parameters is same as the number of 
     input parameters*/

	if ((countValidArgs < (unsigned int) (argCount) )||(argCount == 1)){
		printf("\nIPDR_MISSING_OR_INVALID_COMMANDLINE_PARAMS");
		printReadUsage();
		*pErrorCode = IPDR_MISSING_OR_INVALID_COMMANDLINE_PARAMS;
		return(IPDR_ERROR);
	}
	

	free(tempString);  
	free(strTemp);
	free(docType);
	if (inFileName != NULL)
		free(inFileName);

	return(IPDR_OK);
}



/*!
 *  NAME:
 *      populateReadIPDRCommonParam() - This function populates
 *                                       IPDRCommonParameters
 *  DESCRIPTION:
 *      This function is called to initialise IPDRCommonParameters
 *  
 *  ARGUMENTS:
 *		
 *			pIPDRCommonParameters   (IN)
 *			pErrorCode            (IN/OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


int populateReadIPDRCommonParam(
							IPDRCommonParameters* pIPDRCommonParameters,
							int* pErrorCode
								)
{
	int numOfBuckets = 9;

	pIPDRCommonParameters->DocState_ = NONREADABLE;
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
 *      printReadUsage() - Prints test tool usage syntax. 
 *								
 *  DESCRIPTION:
 *				  This Functions is called whenver there are any  
 *				  invalid command line arguments.
 *  ARGUMENTS:		
 *			Void	
 *
 *  RETURNS:	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int printReadUsage()
{
	printf("\nCorrect Input String is: Infile.xdr -doctype XDR <-schemavalidation> -outFiledir <-output directory>");
	return (IPDR_OK);
}




/*!
 *  NAME:
 *      appendListServiceInfo() - Appends a node to the Service Info List 
 *
 *  DESCRIPTION: Adds  one node to the Service Info List which contains 
 *               the FNFType and File pointer
 *      
 *  ARGUMENTS:
 *			- pHeadRef        (IN/OUT)
 *			- FILE            (IN)
 *			- FNFType         (IN)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */



int appendListServiceInfo(
						 ListServiceInfo** pHeadRef, 
						 FILE* fp,
						 FNFType* pFNFType,
						 ListNameSpaceInfo* pListNameSpaceInfo
						 )
{

	ListServiceInfo* pCurrent = *pHeadRef;
	if (pHeadRef == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}
 
	if (pFNFType == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}


	if (pCurrent == NULL) {
		addListServiceInfo(pHeadRef, fp, pFNFType, pListNameSpaceInfo);
	} else {
    
		while (pCurrent->pNext_ != NULL) {
			pCurrent = pCurrent->pNext_;
		}
        addListServiceInfo
		(&(pCurrent->pNext_),fp, pFNFType, pListNameSpaceInfo);
	}
	
	return (IPDR_OK);
		
}


/*!
 *  NAME:
 *      addListServiceInfo() - Adds information to one node of the Service 
 *                             Info List 
 *
 *  DESCRIPTION: Adds information to one node of the Service Info List 
 *               which contains the FNFType and File pointer
 *    
 *  ARGUMENTS:
 *		- ListServiceInfo (IN/OUT)
 *      - FILE            (IN)
 *      - FNFType         (IN)
 *			
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int addListServiceInfo(
					   ListServiceInfo** pHeadRef,                        
					   FILE* fp, 
					   FNFType* pFNFType,
					   ListNameSpaceInfo* pListNameSpaceInfo
					  )
{
	ListServiceInfo* pListServiceInfo = (ListServiceInfo *) 
		                     malloc (sizeof(ListServiceInfo));
	
	if (pListServiceInfo == NULL) {
		return (IPDR_MEMORY_ALLOCATION_FAILED);
	}

	pListServiceInfo->pServiceInfo_ = newServiceInfo();

	if (pListServiceInfo->pServiceInfo_ == NULL) {
		return (IPDR_MEMORY_ALLOCATION_FAILED);
	}
	
	pListServiceInfo->pServiceInfo_->fp = fp;
	pListServiceInfo->pServiceInfo_->pFNFType_->serviceType_ = 
		pFNFType->serviceType_;
	pListServiceInfo->pServiceInfo_->pFNFType_->pListAttributeDescriptor_ 
		= pFNFType->pListAttributeDescriptor_;
    pListServiceInfo->pServiceInfo_->pFNFType_ = pFNFType;
	appendListNameSpaceInfo( 
		&(pListServiceInfo->pServiceInfo_->pListNameSpaceInfo_),
		pListNameSpaceInfo->NameSpaceInfo_.nameSpaceID_,
		pListNameSpaceInfo->NameSpaceInfo_.nameSpaceURI_);

	pListServiceInfo->pNext_ = *pHeadRef;
	*pHeadRef = pListServiceInfo;

	return (IPDR_OK);
}

/*!
 *  NAME:
 *      printListServiceInfo() - Prints the Service Info List 
 *
 *  DESCRIPTION:  Prints the Service Info List
 *  
 *  ARGUMENTS:
 *		- pListServiceInfo (IN/OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int printListServiceInfo(ListServiceInfo* pListServiceInfo)
{
	ListServiceInfo* pCurrent = pListServiceInfo;
	int count = 0;
	if (pCurrent == NULL) 
		return(IPDR_NULL_LINKLIST);
	
	while (pCurrent != NULL) {
		#ifdef DEBUG_IPDR 
		{
			printf("\n\nDisplaying Record %d", count + 1);
			printf("\nService Type = %s", pCurrent->pServiceInfo_->
				pFNFType_->serviceType_);
			printFNFType(pCurrent->pServiceInfo_->pFNFType_);
			printListNameSpaceInfo(pCurrent->pServiceInfo_->pListNameSpaceInfo_);
		}
        #endif
	    pCurrent = pCurrent->pNext_;	
	    count++;
	}
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      newServiceInfo() - Initializes the Service Info List 
 *
 *  DESCRIPTION:  Initializes the Service Info List
 *  
 *  ARGUMENTS:
 *		- void
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


ServiceInfo* newServiceInfo(void)
{
	ServiceInfo* pServiceInfo;
	pServiceInfo = (ServiceInfo *) malloc (sizeof(ServiceInfo));
	pServiceInfo->fp = NULL;
	pServiceInfo->pFNFType_ = newFNFType();
	pServiceInfo->pListNameSpaceInfo_ = NULL;
	return (pServiceInfo);
}


/*!
 *  NAME:
 *      getLengthListServiceInfo() - Finds the length of the Service Info 
 *                                  List 
 *
 *  DESCRIPTION:  Finds the length of the Service Info List
 *  
 *  ARGUMENTS:
 *		- pListServiceInfo (IN)
 *
 *  RETURNS 	
 *	     Returns the length
 */

int getLengthListServiceInfo(ListServiceInfo* pListServiceInfo)
{
	int count = 0;
	ListServiceInfo* pCurrent = pListServiceInfo;

	if (pListServiceInfo == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	for (pCurrent = pListServiceInfo; pCurrent != NULL; 
	pCurrent = pCurrent->pNext_)
		count++;
	return (count);
}

/*!
 *  NAME:
 *      freeListServiceInfo() - Memory deallocation of 
 *      ListServiceInfo structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory deallocation of 
 *         ListServiceInfo structure & its members.
 *	
 *  ARGUMENTS:
 *       -	ListServiceInfo {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
  

int freeListServiceInfo(ListServiceInfo **pHeadRef)
{
	ListServiceInfo* pCurrent;
	ListServiceInfo* pNext_;
	if (pHeadRef == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}
 
	pCurrent = *pHeadRef;
    while (pCurrent != NULL) {
        pNext_ = pCurrent->pNext_;
    
		freeServiceInfo(pCurrent->pServiceInfo_);
		
		free(pCurrent);
		pCurrent = pNext_;
    }
    free(pCurrent);
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      freeServiceInfo() - Memory deallocation of 
 *      ServiceInfo structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory deallocation of 
 *         ServiceInfo structure & its members.
 *	
 *  ARGUMENTS:
 *       -	IPDRRecord {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int freeServiceInfo(ServiceInfo* pServiceInfo)
{
	if (pServiceInfo == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}
	fclose(pServiceInfo->fp);  
	freeFNFType(pServiceInfo->pFNFType_);
	freeListNameSpaceInfo(&(pServiceInfo->pListNameSpaceInfo_));
	free(pServiceInfo);
	return (IPDR_OK);
}

