/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/


/*****************************************************
 * File                 : IPDRReadTool               *                       
 * Description          :                            *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *                     
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 * 16Feb2002                                         *
 *****************************************************/

#include "include/IPDRReadTool.h"

int main(
		 int argc, 
		 char *argv[]
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
	unsigned int ipdrVersionFlag = 0;
	unsigned int lengthFileName = 0;
	unsigned int lenStrServInfoAttrName = 0;
	int countListNameSpaceInfo = 0;

	char* ipdrVersion = NULL;
	char* outDirectory= NULL;
	char* fileName = NULL;
	char* pServInfoAttrName= NULL;
	char* strServInfoAttrName= NULL;
	char* pTempServInfoAttrName= NULL;
	char* pNameSpaceURI= NULL;
	char* pNameSpaceID= NULL;
					 
	int* pErrorCode = NULL;
   char* lastSlash= NULL;
   char* pTemp= NULL;
   int found = 0;
   int lastDot = 0;
   int index = 0;


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
	ListNameSpaceInfo* pCurrentListNameSpaceInfo = NULL;

	pErrorCode = (int *) calloc(1, sizeof(int));
	outDirectory =  (char *) calloc(MAX_ATTRIBUTE_NAME, sizeof(char));
	ipdrVersion =  (char *) calloc(10, sizeof(char));

   pFNFData = newFNFData();

	if(pFNFData == NULL || pErrorCode == NULL || outDirectory == NULL) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		return(IPDR_ERROR);
	}


	/* Intializing structures */
	pIPDRCommonParameters = newIPDRCommonParameters();
	pIPDRHeader = newIPDRHeader();

	errorSeverity = populateIPDRCommonParameters(pIPDRCommonParameters, pErrorCode);
	if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, "populateIPDRCommonParameters"); 
	}
	else if (errorSeverity == IPDR_ERROR) {
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "populateIPDRCommonParameters"); 
			printExceptionStructure (pIPDRCommonParameters->pExceptionStructure_); 
		return (IPDR_ERROR);
	}

	
	/*Validate the commandline parameters*/
	errorSeverity = validateInputParams(argc, argv, &schemaValFlag,
		&outDirFlag, &ipdrVersionFlag, outDirectory, ipdrVersion, pIPDRCommonParameters,pErrorCode);
	if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "validateInputParams"); 
	}
	else if (errorSeverity == IPDR_ERROR) {
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "validateInputParams"); 
			printExceptionStructure (pIPDRCommonParameters->pExceptionStructure_); 
		return (IPDR_ERROR);
	}

	/* IPDR v3.5, changes for vesrion*/
	if((ipdrVersionFlag == 1) && (schemaValFlag == 1)) {
		printf("Please set either -ipdrversion/-schemavalidation but not both\n");
		return (IPDR_ERROR);
	} else if((ipdrVersionFlag == 0) && (schemaValFlag == 0)) {
		printf("Please set either -ipdrversion/-schemavalidation\n");
		return(IPDR_ERROR);
	}

	if(ipdrVersionFlag == 1) {
      printf("\n ipdrversion is ON, Calling setVersion\n");
      printf("\n ipdrversion is %s\n", ipdrVersion);
		if((strcmp(ipdrVersion,IPDR_VERSION_3_5) != 0) && (strcmp(ipdrVersion,IPDR_VERSION_3_1) != 0)) {
			printf("The ipdr version, %s, ", ipdrVersion);
			printf("is not supported\n");
			return(IPDR_ERROR);
		} else {
			if((strcmp(ipdrVersion,IPDR_VERSION_3_5) == 0)) {
				printf("Setting the ipdr Version to %s\n", ipdrVersion);
				strcpy(pIPDRCommonParameters->schemaVersion, IPDR_VERSION_3_5);
				/*setVersion(3.5);*/
			}
			else if((strcmp(ipdrVersion,IPDR_VERSION_3_1) == 0)) {
				printf("Setting the ipdr Version to %s\n", ipdrVersion);
				strcpy(pIPDRCommonParameters->schemaVersion, IPDR_VERSION_3_1);
				/*setVersion(3.1);*/
			}
		}
	}

/*
   if(setVersion(3.5) == 1)
     printf("Changed the version\n");
	else
     printf("sorry no version change\n");
*/

	/* Read the header and get the default name space and Service 
	Definition URI list*/
    errorSeverity = readHeader(pIPDRCommonParameters, pIPDRHeader, pErrorCode);
	if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "readHeader"); 
	}
	else if (errorSeverity == IPDR_ERROR) {
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "readHeader"); 
			printExceptionStructure (pIPDRCommonParameters->pExceptionStructure_); 
		return (IPDR_ERROR);
	}

	#ifdef DEBUG_IPDR 
		printListFNFData(pIPDRCommonParameters->pListFNFData_);
    #endif

	if(schemaValFlag) {
      printf("\nSchema Validation is ON, Calling setSchemaData");
		errorSeverity = setSchemaData(
									  pIPDRCommonParameters, 
									  pIPDRHeader->pOtherNameSpaceInfo_, 
									  pIPDRHeader->pListServiceDefinitionURI_, 
									  pIPDRHeader->defaultNameSpaceURI_,   
									  pErrorCode
									 );
	        printf("\nReturn value from setSchemaData = %d", errorSeverity);								   
		if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "setSchemaData"); 
		}
		else if (errorSeverity == IPDR_ERROR) {
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "setSchemaData"); 
				printExceptionStructure (pIPDRCommonParameters->pExceptionStructure_); 
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

		errorSeverity = generateSchemaDescriptor(pCurrent->pServiceDefintionURI_, 
			 pFNFType[countList],&pListNameSpaceInfo, pErrorCode);
		if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "generateSchemaDescriptor"); 
		}
		else if (errorSeverity == IPDR_ERROR) {
			exceptionHandler(pIPDRCommonParameters, pErrorCode, "generateSchemaDescriptor"); 
				printExceptionStructure (pIPDRCommonParameters->pExceptionStructure_); 
		return (IPDR_ERROR);
		}

		lengthAttrDesc = getLengthListAttributeDescriptor
			(pFNFType[countList]->pListAttributeDescriptor_);

		fileName = (char *) calloc (MAX_IPDR_STRING, sizeof(char));
		if (fileName == NULL) {
			*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
			return(IPDR_ERROR);
		}	

		/*strcpy(fileName, pFNFType[countList]->serviceType_);
		strcat(fileName, ".csv");*/
                pTemp = pIPDRCommonParameters->pStreamName_;
                lastSlash = pTemp;
                found = 0;
                count = 0;
                lastDot = 0;
                while ( *pTemp != '\0' ) {
#ifdef WINDOWS
                    if ( *pTemp == '\\' ) { found = 1; lastSlash = pTemp; }
#else
                    if ( *pTemp == '/' ) { found = 1; lastSlash = pTemp; }
#endif
                    pTemp++;
                }
                if ( found == 1 ) { ++lastSlash; } /* skip everything up to and including the final slash */
                /* Remove the .xdr or .xml extn */
                found = 0;
                while (*lastSlash != '\0')
                {
                    if (*lastSlash == '.') {
                        lastDot = count;
                        found = 1;
                    }
                    fileName[count] = *lastSlash;
                    count++;
                    lastSlash++;
      //strcpy(fileName, lastSlash );
                }
                if (found == 1) { 
                    for ( index=lastDot; index<count; index++ ) fileName[index]=(char)0;
                    fileName[lastDot]='\0';
                }
      strcat(fileName, ".csv");

      if(outDirFlag == 1) {
#ifdef WINDOWS
         strcat(outDirectory, "\\");
#else
         strcat(outDirectory, "/");
#endif
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

		if (pCurr != NULL) {
		if (((strcmp( pCurr->NameSpaceInfo_.nameSpaceID_, "") == 0)||
					  (strcmp( pCurr->NameSpaceInfo_.nameSpaceID_, "ipdr") == 0))) {
			if(strcmp(pCurr->NameSpaceInfo_.nameSpaceURI_,"http://www.ipdr.org/namespaces/ipdr") == 0)
				fprintf(fp, "%s", "http://www.ipdr.org/namespaces/IPDR");
			else
				fprintf(fp, "%s", pCurr->NameSpaceInfo_.nameSpaceURI_);
		}
		else {
			fprintf(fp, "%s%c%s",pCurr->NameSpaceInfo_.nameSpaceID_,'$',
			pCurr->NameSpaceInfo_.nameSpaceURI_);
		}
		} else {
			fprintf(fp, "%s", "http://www.ipdr.org/namespaces/IPDR");
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
			char* token;

			lenStrServInfoAttrName = strlen(pCurrentAttrDesc->pAttributeDescriptor_->attributeName_) + 1;
			strServInfoAttrName = (char *) calloc (MAX_ATTRIBUTE_LENGTH, sizeof(char));
			
			pServInfoAttrName = (char *) calloc(MAX_ATTRIBUTE_LENGTH, 
				sizeof(char));
			pTempServInfoAttrName = (char *) calloc(MAX_ATTRIBUTE_LENGTH, 
				sizeof(char));
			pNameSpaceURI = (char *) calloc(MAX_ATTRIBUTE_LENGTH,
				sizeof(char));
			pNameSpaceID = (char *) calloc(MAX_ATTRIBUTE_LENGTH, 
				sizeof(char));

	    	
			strcpy(strServInfoAttrName,
				pCurrentAttrDesc->pAttributeDescriptor_->attributeName_);

		
			token = strtok( strServInfoAttrName, "$" );
	
			count = 0;
			while( token != NULL ) {
					if(count==1)
						 strcpy(pNameSpaceURI,token);
					if(count==2)
					strcpy(pTempServInfoAttrName,token);
					token = strtok( NULL, "$" );
					count++;
			}
			
		
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
		
                   count = 0;	
		   if (!((strcmp(pNameSpaceID, "") == 0)||
				(strcmp(pNameSpaceID, "ipdr") == 0))) {
                           token = strtok( pTempServInfoAttrName, ":" );
			while( token != NULL ) {
					if(count==1)
					    strcpy(pTempServInfoAttrName,token);
					token = strtok( NULL, ":" );
					count++;
			}	
			 
				strcpy(pServInfoAttrName,pNameSpaceID);
				strcat(pServInfoAttrName, ":");
				strcat(pServInfoAttrName, pTempServInfoAttrName);
		   }
		   else {
				strcpy(pServInfoAttrName,pTempServInfoAttrName);
		   }
			
			//if((isPrimitiveComplexType(pCurrentAttrDesc->pAttributeDescriptor_->complexType_) == IPDR_FALSE)) 
		   	getElementName(pServInfoAttrName, 
							 	pCurrentAttrDesc->pAttributeDescriptor_,
							 	pFNFType[countList]->pListServiceAttributeDescriptor_);			
		    fprintf(fp,"%s", pServInfoAttrName);
			if(commaCount < lengthAttrDesc - 1) {
				fprintf(fp,"%c", ',');
				commaCount++;
			}

		    pCurrentAttrDesc = pCurrentAttrDesc->pNext_;	
	
			free(pNameSpaceURI);
			free(pNameSpaceID);
			free(pTempServInfoAttrName);
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
				printExceptionStructure (pIPDRCommonParameters->pExceptionStructure_); 
			return (IPDR_ERROR);
		}
 
		pCurrent = pCurrent->pNext_;	
		if (pCurr != NULL) pCurr = pCurr->pNext_;
		countList++;
	  
	  			
	}

				
	
	/*To generate the csv file containing raw data values*/
	errorSeverity = generateRawData(pIPDRCommonParameters, pFNFData, pListServiceInfo, pErrorCode);
	if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "generateRawData"); 
	}
	else if (errorSeverity == IPDR_ERROR) {
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "generateRawData"); 
			printExceptionStructure (pIPDRCommonParameters->pExceptionStructure_); 
		return (IPDR_ERROR);
	}
	
		printListErrorStructure (pIPDRCommonParameters->pListErrorStructure_); 

	printf("\n Document decoded Successfully\n");


	for (countList = 0; (countList < lengthServURI); countList++) {
		freeFNFType(pFNFType[countList]);
			
	}

	if (fileName != NULL)
	free(fileName);
	free(outDirectory);
	free(pFNFType);
	freeIPDRCommonParameters(pIPDRCommonParameters);
	free(pErrorCode);
	freeListServiceInfo(&(pListServiceInfo));
	freeIPDRHeader(pIPDRHeader);

	return (0);
}

int getElementName(char* pServInfoAttrName, 
						 AttributeDescriptor* pAttributeDescriptor,
						 ListAttributeDescriptor* pListServiceAttributeDescriptor
						)
{
	char* pAttrName = NULL;
   char* tempString = NULL;
	char* tempComplexType = NULL;
	char* serviceTypeToken = NULL;

if(pAttributeDescriptor->complexType_[0] != '\0') {
	tempString = (char *) calloc(strlen(pAttributeDescriptor->complexType_) + 1, sizeof(char));
	tempComplexType = (char *) calloc(strlen(pAttributeDescriptor->complexType_) + 1, sizeof(char));
	strcpy(tempString, pAttributeDescriptor->complexType_);
	serviceTypeToken = strtok(tempString, ":");
	if(strcmp(pAttributeDescriptor->complexType_,serviceTypeToken)) {
		getIPDRSubstring(pAttributeDescriptor->complexType_, 
								tempComplexType, ':');
   	strcpy(pAttributeDescriptor->complexType_,tempComplexType);
	}
	free(tempString);
	free(tempComplexType);
}
	
	if(isPrimitiveComplexType(pAttributeDescriptor->complexType_) == IPDR_FALSE) {
		tempString = (char *) calloc(strlen(pAttributeDescriptor->attributeName_) + 1, sizeof(char));
		tempComplexType = (char *) calloc(strlen(pAttributeDescriptor->attributeName_) + 1, sizeof(char));
		strcpy(tempString, pAttributeDescriptor->attributeName_);
						serviceTypeToken = strtok(tempString, ":");
	
		if(strcmp(pAttributeDescriptor->attributeName_,serviceTypeToken)) {
			getIPDRSubstring(pAttributeDescriptor->attributeName_, tempComplexType, ':');
			getIPDRSubstring(tempComplexType, tempString, '$');
   		//strcpy(pAttributeDescriptor->attributeName_,tempString);
		}	

		//strcpy(pServInfoAttrName, pAttributeDescriptor->attributeName_);
		strcpy(pServInfoAttrName, tempString);
		free(tempString);
		free(tempComplexType);
		getComplexElementName(pServInfoAttrName,
									 pAttributeDescriptor->complexType_,
									 pListServiceAttributeDescriptor);
	 	if(pAttributeDescriptor->noOfElements_ != 0) {
			pAttrName = (char *) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char));
			strcpy(pAttrName, "( ");
			strncat(pAttrName, pServInfoAttrName, strlen(pServInfoAttrName));
			strcat(pAttrName, " )");
			strcpy(pServInfoAttrName, pAttrName);
			free(pAttrName);
		}
	} else if(pAttributeDescriptor->noOfElements_ != 0) {
			pAttrName = (char *) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char));
			strcpy(pAttrName, "( ");
			strncat(pAttrName, pServInfoAttrName, strlen(pServInfoAttrName));
			strcat(pAttrName, " )");
			strcpy(pServInfoAttrName, pAttrName);
			free(pAttrName);
		}

	return (IPDR_OK);
}

int getComplexElementName(char* pServInfoAttrName,
								  char* pComplexType, 
								  ListAttributeDescriptor* pListServiceAttributeDescriptor
								 )
{
	int count = 0;
	int countNext = 0;
   int cnt = 0;
	int lLoopFound = 0;
	int errorSeverity = 0;
	int attributeDescriptorLength = 0;
	int typeCode = 0;
	int noOfElements = 0;
	char* tempfieldName = NULL;
	char* tempComplexType = NULL;
   char* tempString = NULL;
	char* serviceTypeToken = NULL;
 	ListAttributeDescriptor* pCurrentListServiceAttributeDescriptor = NULL;
 	ListAttributeDescriptor* pOldListServiceAttributeDescriptor = NULL;
 	ListAttributeDescriptor* pTempOldListServiceAttributeDescriptor = NULL;
	List* pcurrent = NULL;
	List* pcurrentMaxOccrsList = NULL;

	pCurrentListServiceAttributeDescriptor = pListServiceAttributeDescriptor;
   pOldListServiceAttributeDescriptor = pListServiceAttributeDescriptor;
   pTempOldListServiceAttributeDescriptor = pListServiceAttributeDescriptor;

	attributeDescriptorLength = getLengthListAttributeDescriptor(pListServiceAttributeDescriptor);
	for (count = 0; count < attributeDescriptorLength; count++) {

		if(lLoopFound == 1)
         break;

		tempString = (char *) calloc(strlen(pListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_) + 1, sizeof(char));
		tempComplexType = (char *) calloc(strlen(pListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_) + 1, sizeof(char));
		strcpy(tempString, pListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_);
			serviceTypeToken = strtok(tempString, ":");
	
		if(strcmp(pListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_,serviceTypeToken)) {
			getIPDRSubstring(pListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_, tempComplexType, ':');
			getIPDRSubstring(tempComplexType, tempString, '$');
   		strcpy(pListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_,tempString);
		}	
		free(tempString);
		free(tempComplexType);
	
		if(strcmp(pComplexType, 
					 pListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_) == 0) {
			pcurrent = pListServiceAttributeDescriptor->pAttributeDescriptor_->pListAttrNameList_;
			if(pListServiceAttributeDescriptor->pAttributeDescriptor_->pListAttrMaxOccursList_ != NULL) {
				pcurrentMaxOccrsList = pListServiceAttributeDescriptor->pAttributeDescriptor_->pListAttrMaxOccursList_;
			}
			strcat(pServInfoAttrName, "[ ");
			while((pcurrent != NULL))	{
				lLoopFound = 1;
				pOldListServiceAttributeDescriptor = pTempOldListServiceAttributeDescriptor;
				for (countNext = 0; countNext < attributeDescriptorLength; countNext++) {
					if(strcmp(pcurrent->node, 
					   pOldListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_) == 0) {
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
						if(isPrimitiveComplexType(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_) == IPDR_TRUE) {
              		if((pcurrentMaxOccrsList != NULL)) {
							if((strcmp(pcurrentMaxOccrsList->node, "-1") == 0) || 
								(strcmp(pcurrentMaxOccrsList->node, "0") != 0)) {
								strcat(pServInfoAttrName,"( ");
								strcat(pServInfoAttrName, pcurrent->node);
								strcat(pServInfoAttrName," )");
							}
							else {
								strcat(pServInfoAttrName, pcurrent->node);
							}
							strcat(pServInfoAttrName, ";");
							}
			  		} else {

						strcat(pServInfoAttrName, 
									pOldListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_);

						getComplexElementName(pServInfoAttrName,
													 pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_,
													 pTempOldListServiceAttributeDescriptor);
			  		}
					break;
				}
				pOldListServiceAttributeDescriptor = pOldListServiceAttributeDescriptor->pNext_;
			}
			pcurrent = pcurrent->pNext;
			if(pcurrentMaxOccrsList != NULL)
				pcurrentMaxOccrsList = pcurrentMaxOccrsList->pNext;
		}
		}
		pListServiceAttributeDescriptor = pListServiceAttributeDescriptor->pNext_;
	}
	
   pListServiceAttributeDescriptor = pCurrentListServiceAttributeDescriptor;
	tempfieldName = (char *) calloc(strlen(pServInfoAttrName) + 1, sizeof(char));
   if(pServInfoAttrName[strlen(pServInfoAttrName)-1] == ';')
		strncpy(tempfieldName, pServInfoAttrName, (strlen(pServInfoAttrName)-1));
	else
		strncpy(tempfieldName, pServInfoAttrName, (strlen(pServInfoAttrName)));

	strcpy(pServInfoAttrName,tempfieldName);
	strcat(pServInfoAttrName," ]");
	
	free(tempfieldName);

	return(IPDR_OK);
}

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
	int baseFlag = 0;
	int noOfElements = 0;
	int i;

	//char fieldValue[MAX_ATTRIBUTE_LENGTH];
	//char tempfieldValue[MAX_ATTRIBUTE_LENGTH];


	char* fieldValue = NULL;
	char* tempfieldValue = NULL;
	char* tempString=NULL;
	char* token1=NULL;
	char seps1[]   = "$";
	char seps2[]   = ":";
	char* pAttributeName=NULL;
	char* strAttributeName=NULL;
	char* pServInfoAttrName=NULL;
	char* strServInfoAttrName=NULL;
	char* pNameSpaceURI=NULL;
	char* pNameSpaceID=NULL;

	ListIPDRData* pCurrentIPDRData = NULL;
	ListAttributeDescriptor* pCurrentAttrDesc = NULL;
	ListAttributeDescriptor* pCurrentServInfo = NULL;
	ListAttributeDescriptor* pCurrentListServiceInfo = NULL;
	ListServiceInfo* pCurrent = NULL;
	ListNameSpaceInfo* pCurrentListNameSpaceInfo = NULL;
 	ListIPDRStructureData* pCurrentListIPDRStructureData = NULL;

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

	fieldValue = (char *) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char));

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
					
		pCurrentServInfo = pCurrent->pServiceInfo_->pFNFType_->pListAttributeDescriptor_;
		pCurrentListServiceInfo = pCurrent->pServiceInfo_->pFNFType_->pListServiceAttributeDescriptor_;
			
 		pCurrentIPDRData = pFNFData->pListIPDRData_;
			
		lengthCurrentServInfo = getLengthListAttributeDescriptor
			(pCurrentServInfo); 

		commaCount = 0;

		for (countListServInfo = 0; countListServInfo < 
			lengthCurrentServInfo; countListServInfo++) {	
						
			matchFlag=0;
			pCurrentAttrDesc = pFNFData->pListAttributeDescriptor_;
			/*pCurrentAttrDesc = pCurrent->pServiceInfo_->pFNFType_->pListAttributeDescriptor_;*/
					
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

			/*token1 = strtok( pServInfoAttrName, seps2 );
		   strcpy(pServInfoAttrName,token1);*/
                        
			
			count = 0;
		   if (!((strcmp(pNameSpaceID, "") == 0)||
				(strcmp(pNameSpaceID, "ipdr") == 0))) {
			
                           token1 = strtok( pServInfoAttrName, seps2 );
			while( token1 != NULL ) {
					if(count==1)
					    strcpy(pServInfoAttrName,token1);
					token1 = strtok( NULL, seps2 );
					count++;
			}	
			 
				strcpy(pAttributeName,pNameSpaceID);
				strcat(pAttributeName, ":");
				strcat(pAttributeName, pServInfoAttrName);
		   }
		   else {
				strcpy(pAttributeName,pServInfoAttrName);
		   }
			    
			/*NEW CODE ADDED*/
			/*
			if(!((strcmp(pNameSpaceID, "") == 0))) {
				strcpy(tempString, pNameSpaceID);
				strcat(tempString,":");
				strcat(tempString, pCurrentAttrDesc->pAttributeDescriptor_->attributeName_);
				strcpy(pCurrentAttrDesc->pAttributeDescriptor_->attributeName_, tempString);
			} else {
				strcpy(tempString, pCurrentAttrDesc->pAttributeDescriptor_->attributeName_);
			}
			*/
			/*NEW CODE ADDED*/

			if(strcmp(pCurrentAttrDesc->pAttributeDescriptor_->attributeName_, 
				pAttributeName) == 0) {
			/*if(strcmp(pCurrentAttrDesc->pAttributeDescriptor_->attributeName_, 
				pCurrentServInfo->pAttributeDescriptor_->attributeName_) == 0) */


			//if((isPrimitiveComplexType(pCurrentAttrDesc->pAttributeDescriptor_->complexType_) == IPDR_FALSE) &&
			//	(pCurrentAttrDesc->pAttributeDescriptor_->noOfElements_ != 0)) 
			if((isPrimitiveComplexType(pCurrentAttrDesc->pAttributeDescriptor_->complexType_) == IPDR_FALSE)) {
				/*if(pCurrentAttrDesc->pAttributeDescriptor_->noOfElements_ == -1) {
					noOfElements = pCurrentIPDRData->pListIPDRStructureData_->noOfElements_;
				} else {
					noOfElements = pCurrentAttrDesc->pAttributeDescriptor_->noOfElements_;
				}*/

				if(pCurrentServInfo->pAttributeDescriptor_->noOfElements_ == -1) {
					noOfElements = pCurrentIPDRData->pListIPDRStructureData_->noOfElements_;
				} else {
					noOfElements = pCurrentAttrDesc->pAttributeDescriptor_->noOfElements_;
				}
			
				if(noOfElements > 0) {	
				pCurrentListIPDRStructureData = pCurrentIPDRData->pListIPDRStructureData_;
				strcpy(fieldValue,"(");
				for(i=0; i<noOfElements;i++) {
					getComplexFieldValue(pIPDRCommonParameters,
												fieldValue,
												pCurrentAttrDesc->pAttributeDescriptor_,
												pFNFData->pListServiceAttributeDescriptor_,							
												pCurrent->pServiceInfo_,
												pCurrentAttrDesc->pAttributeDescriptor_->complexType_,
									  			pCurrentIPDRData,
												pErrorCode
											  );
					strcat(fieldValue,";");
					pCurrentIPDRData->pListIPDRStructureData_ = pCurrentIPDRData->pListIPDRStructureData_->pNext_;
				}
					tempfieldValue = (char *) calloc(strlen(fieldValue) + 1, sizeof(char));
					strncpy(tempfieldValue, fieldValue, (strlen(fieldValue)-1));
					strcpy(fieldValue,tempfieldValue);
					free(tempfieldValue);
					strcat(fieldValue,")");
					fprintf(pCurrent->pServiceInfo_->fp, "%s", fieldValue);
				} else {
					fieldValue[0] = '\0';
					getComplexFieldValue(pIPDRCommonParameters,	
												fieldValue,
												pCurrentAttrDesc->pAttributeDescriptor_,
												pFNFData->pListServiceAttributeDescriptor_,							
												pCurrent->pServiceInfo_,
												pCurrentAttrDesc->pAttributeDescriptor_->complexType_,
									  			pCurrentIPDRData,
												pErrorCode
											  );
					fprintf(pCurrent->pServiceInfo_->fp, "%s", fieldValue);
				}
				if(commaCount < lengthCurrentServInfo - 1) {
					 fprintf(pCurrent->pServiceInfo_->fp,"%c",',');	
					 	commaCount++;
				}
			} 
         /*else if(pCurrentAttrDesc->pAttributeDescriptor_->noOfElements_ != 0)*/
         else if(pCurrentServInfo->pAttributeDescriptor_->noOfElements_ != 0) {
					//array
					getFieldValue(	pCurrentAttrDesc->pAttributeDescriptor_, 
										pCurrent->pServiceInfo_,
									 	pCurrentIPDRData,
										pErrorCode);
				if(commaCount < lengthCurrentServInfo - 1) {
					 fprintf(pCurrent->pServiceInfo_->fp,"%c",',');	
					 	commaCount++;
				}
			}else {

				convertToUpper((char*)pCurrentAttrDesc->
					pAttributeDescriptor_->attributeType_, tempString);
										
				if (strcmp("INTEGER", tempString) == 0) {
					fprintf(pCurrent->pServiceInfo_->fp,"%ld", 
					pCurrentIPDRData->IPDRDataUnion_.IPDRLong_);		
		
				}
				else if ((strcmp("UNSIGNED_INTEGER", tempString) == 0)) {
					/*fprintf(pCurrent->pServiceInfo_->fp,"%ld", 
					pCurrentIPDRData->IPDRDataUnion_.IPDRULong_);*/
					fprintf(pCurrent->pServiceInfo_->fp,"%u", 
					pCurrentIPDRData->IPDRDataUnion_.IPDRULong_);
					
				}
				else if (strcmp("HYPER", tempString) == 0) {
					 /*fprintf(pCurrent->pServiceInfo_->fp,"%.lf", 
					 (double) (pCurrentIPDRData->IPDRDataUnion_.IPDRLongLong_));*/
					 fprintf(pCurrent->pServiceInfo_->fp,"%lld", 
					 (pCurrentIPDRData->IPDRDataUnion_.IPDRLongLong_));
				 

				}
				else if (strcmp("UNSIGNED_HYPER", tempString) == 0) {
					  /*fprintf(pCurrent->pServiceInfo_->fp,"%.lf", 
					  (pCurrentIPDRData->IPDRDataUnion_.IPDRULongLong_));*/
					  fprintf(pCurrent->pServiceInfo_->fp,"%llu", 
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
					  writeEscapedChar((char *)pCurrentIPDRData->IPDRDataUnion_.IPDRUTF8Array_));		  
					  
				}
				else if (strcmp("BOOLEAN", tempString) == 0) {
					  if(strcmp(pCurrentIPDRData->IPDRDataUnion_.IPDRBoolean_, "0") == 0) {
							fprintf(pCurrent->pServiceInfo_->fp,"%s", "false");
						} else if(strcmp(pCurrentIPDRData->IPDRDataUnion_.IPDRBoolean_, "1") == 0) {
							fprintf(pCurrent->pServiceInfo_->fp,"%s", "true");
						} else if(strcmp(pCurrentIPDRData->IPDRDataUnion_.IPDRBoolean_, "false") == 0) {
							fprintf(pCurrent->pServiceInfo_->fp,"%s", "false");
						} else if(strcmp(pCurrentIPDRData->IPDRDataUnion_.IPDRBoolean_, "true") == 0) {
							fprintf(pCurrent->pServiceInfo_->fp,"%s", "true");
						}

					  /*fprintf(pCurrent->pServiceInfo_->fp,"%s", 
					  pCurrentIPDRData->IPDRDataUnion_.IPDRBoolean_);*/
					  
				}
				else if (strcmp("SHORT", tempString) == 0) {
					fprintf(pCurrent->pServiceInfo_->fp,"%ld", 
					pCurrentIPDRData->IPDRDataUnion_.IPDRShort_);
					
				}
				else if (strcmp("UNSIGNED_SHORT", tempString) == 0) {
					fprintf(pCurrent->pServiceInfo_->fp,"%u", 
					pCurrentIPDRData->IPDRDataUnion_.IPDRUShort_);
				}
				else if (strcmp("BYTE", tempString) == 0) {
					fprintf(pCurrent->pServiceInfo_->fp,"%d", 
					pCurrentIPDRData->IPDRDataUnion_.IPDRByte_);
				}
				else if (strcmp("UNSIGNED_BYTE", tempString) == 0) {
					fprintf(pCurrent->pServiceInfo_->fp,"%d", 
					pCurrentIPDRData->IPDRDataUnion_.IPDRByte_);
				}

				if(commaCount < lengthCurrentServInfo - 1) {
					 fprintf(pCurrent->pServiceInfo_->fp,"%c",',');	
					 	commaCount++;
				}
			}
          						
				matchFlag = 1 ;	
				baseFlag = 1;

				pCurrentIPDRData = pCurrentIPDRData->pNext_;				
				
				}	
				free(pServInfoAttrName);
				free(strServInfoAttrName);
				free(pAttributeName);
				free(strAttributeName);
				free(pNameSpaceURI);
				free(pNameSpaceID);

				pCurrentAttrDesc = pCurrentAttrDesc->pNext_;		

				
				if (baseFlag == 0) {
				if (pCurrentIPDRData != NULL)
				pCurrentIPDRData = pCurrentIPDRData->pNext_;				
				}
				
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


		/*freeFNFData(pFNFData);*/
		
		pFNFData = newFNFData();
		baseFlag = 0;
		
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
	free(fieldValue);

	return (IPDR_OK);
} 
			

int getFieldValue(AttributeDescriptor* pAttributeDescriptor, 
						ServiceInfo* pServiceInfo,
						ListIPDRData*  pCurrentIPDRData,
						int* pErrorCode)
{
	char* fieldValue = NULL;
	char* tempfieldValue = NULL;
	char* tempString = NULL;
	int i;
	ListIPDRArrayData* pCurrent = NULL;

   tempString = (char*) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char));

	if(tempString == NULL) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		return(IPDR_ERROR);
	}

	fieldValue = (char *) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char));
	tempfieldValue = (char *) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char));

	pCurrent = pCurrentIPDRData->pListIPDRArrayData_;
	strcpy(fieldValue, "(");
	for(i=0; i < pCurrentIPDRData->noOfElements_; i++) {
				//convertToUpper((char*) pAttributeDescriptor->attributeType_, tempString);
				strcpy(tempString, (char*) pAttributeDescriptor->attributeType_);
										
				if (strcmp("INTEGER", tempString) == 0) {
					sprintf(tempfieldValue,"%ld", 
					pCurrentIPDRData->pListIPDRArrayData_->IPDRDataUnion_.IPDRLong_);
					strcat(fieldValue, tempfieldValue);
		
				}
				else if ((strcmp("UNSIGNED_INTEGER", tempString) == 0) || ((strcmp("UNSIGNED_INTEGERDOUBLE", tempString) == 0))) {
					sprintf(tempfieldValue,"%u", 
					pCurrentIPDRData->pListIPDRArrayData_->IPDRDataUnion_.IPDRULong_);
					strcat(fieldValue, tempfieldValue);
					
				}
				else if (strcmp("HYPER", tempString) == 0) {
					 sprintf(tempfieldValue,"%.lf", 
					 (double) (pCurrentIPDRData->pListIPDRArrayData_->IPDRDataUnion_.IPDRLongLong_));
					 /*sprintf(tempfieldValue,"%lld", 
					 (pCurrentIPDRData->pListIPDRArrayData_->IPDRDataUnion_.IPDRLongLong_));*/
					strcat(fieldValue, tempfieldValue);
				 

				}
				else if (strcmp("UNSIGNED_HYPER", tempString) == 0) {
					 /*sprintf(tempfieldValue,"%.lf", 
					  (pCurrentIPDRData->pListIPDRArrayData_->IPDRDataUnion_.IPDRULongLong_));*/
					 sprintf(tempfieldValue,"%llu", 
					  (pCurrentIPDRData->pListIPDRArrayData_->IPDRDataUnion_.IPDRULongLong_));
					strcat(fieldValue, tempfieldValue);
					  

				}
				else if (strcmp("FLOAT", tempString) == 0) {
					  sprintf(tempfieldValue,"%f", 
					   pCurrentIPDRData->pListIPDRArrayData_->IPDRDataUnion_.IPDRFloat_);
					  strcat(fieldValue, tempfieldValue);
					  

				}
				else if (strcmp("DOUBLE", tempString) == 0) {
					 sprintf(tempfieldValue,"%lf", 
					 pCurrentIPDRData->pListIPDRArrayData_->IPDRDataUnion_.IPDRDouble_);
					  strcat(fieldValue, tempfieldValue);
					 
				}
				else if (strcmp("BYTE_ARRAY", tempString) == 0) {
					  sprintf(tempfieldValue,"%s", 
					  pCurrentIPDRData->pListIPDRArrayData_->IPDRDataUnion_.IPDRByteArray_);
					  strcat(fieldValue, tempfieldValue);
					  

				}
				else if (strcmp("UTF8_ARRAY", tempString) == 0) {
					  sprintf(tempfieldValue,"%s", 
					  writeEscapedChar((char *)pCurrentIPDRData->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_));
					  strcat(fieldValue, tempfieldValue);
					  
				}
				else if (strcmp("BOOLEAN", tempString) == 0) {
					  sprintf(tempfieldValue,"%s", 
					  pCurrentIPDRData->pListIPDRArrayData_->IPDRDataUnion_.IPDRBoolean_);		  
					  strcat(fieldValue, tempfieldValue);
					  
				}
				else if (strcmp("SHORT", tempString) == 0) {
					  sprintf(tempfieldValue,"%ld", 
					  pCurrentIPDRData->pListIPDRArrayData_->IPDRDataUnion_.IPDRShort_);		  
					  strcat(fieldValue, tempfieldValue);
					  
				}
				else if (strcmp("UNSIGNED_SHORT", tempString) == 0) {
					  sprintf(tempfieldValue,"%u", 
					  pCurrentIPDRData->pListIPDRArrayData_->IPDRDataUnion_.IPDRUShort_);		  
					  strcat(fieldValue, tempfieldValue);
					  
				}
				strcat(fieldValue, ";");
				pCurrentIPDRData->pListIPDRArrayData_ = pCurrentIPDRData->pListIPDRArrayData_->pNext_;
	}
				strncpy(tempfieldValue, fieldValue, (strlen(fieldValue)-1));
				strcpy(fieldValue,tempfieldValue);
				strcat(fieldValue, ")");
				//strcat(fieldValue, ",");
	fprintf(pServiceInfo->fp, "%s", fieldValue);
	pCurrentIPDRData->pListIPDRArrayData_ = pCurrent;

	free(tempString);
	free(fieldValue);
	free(tempfieldValue);

	return(IPDR_OK);
}

int getComplexFieldValue(	IPDRCommonParameters* pIPDRCommonParameters,
									char* fieldValue,
									AttributeDescriptor* pAttributeDescriptor,
								 	ListAttributeDescriptor* pListServiceAttributeDescriptor,
									ServiceInfo* pServiceInfo,
								   char *pComplexType,	
								  	ListIPDRData* pListIPDRData,
									int* pErrorCode
								)
{
	int count = 0;
	int countNext = 0;
   int cnt = 0;
	int errorSeverity = 0;
	int attributeDescriptorLength = 0;
	int typeCode = 0;
	int noOfElements = 0;

	char* tempfieldValue = NULL;
	char* tempComplexType = NULL;
   char * tempString = NULL;
   char * tempStringType = NULL;
	char* serviceTypeToken = NULL;
	char derivedType[100];
	char *resultGet = NULL;
	char *tempXMLString = NULL;

 	ListIPDRData* pCurrentListIPDRData = NULL;
 	ListAttributeDescriptor* pCurrentListServiceAttributeDescriptor = NULL;
 	ListAttributeDescriptor* pOldListServiceAttributeDescriptor = NULL;
 	ListAttributeDescriptor* pTempOldListServiceAttributeDescriptor = NULL;
 	ListAttributeDescriptor* pCurrServiceList = NULL;
   ListIPDRComplexData* pTempListIPDRComplexData = NULL;
   ListIPDRComplex* pTempListIPDRComplex = NULL;
   ListIPDRArrayData* pCurrentListIPDRArrayData = NULL;
	List* pcurrentMaxOccrsList = NULL;
	List* pcurrent = NULL;


	resultGet = (char *) calloc (sizeof(char), MAX_XML_STRING_LENGTH);
   tempString = (char*) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char));
	tempXMLString = (char*) calloc(MAX_XML_STRING_LENGTH, sizeof(char));

	if(tempString == NULL) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		free(resultGet);
   	free(tempXMLString);
		return(IPDR_ERROR);
	}

	tempfieldValue = (char *) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char));
	if(tempfieldValue == NULL) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		free(resultGet);
   	free(tempXMLString);
		return(IPDR_ERROR);
	}

	tempStringType = (char *) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char));
	tempComplexType = (char *) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char));

	pCurrentListIPDRData = pListIPDRData;
	pCurrentListServiceAttributeDescriptor = pListServiceAttributeDescriptor;
   pOldListServiceAttributeDescriptor = pListServiceAttributeDescriptor;
   pTempOldListServiceAttributeDescriptor = pListServiceAttributeDescriptor;
   pTempListIPDRComplex = pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_;

	strcat(fieldValue, "[");

	attributeDescriptorLength = getLengthListAttributeDescriptor(pListServiceAttributeDescriptor);
	for (count = 0; count < attributeDescriptorLength; count++) {
		//tempStringType = (char *) calloc(strlen(pListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_) + 1, sizeof(char));
		//tempComplexType = (char *) calloc(strlen(pListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_) + 1, sizeof(char));
		/*strcpy(tempStringType, pAttributeDescriptor->attributeName_);*/
		strcpy(tempStringType, pListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_);
		serviceTypeToken = strtok(tempStringType, ":");
	
		if(strcmp(pListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_,serviceTypeToken)) {
			getIPDRSubstring(pListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_, tempComplexType, ':');
			getIPDRSubstring(tempComplexType, tempStringType, '$');
   		strcpy(pListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_,tempStringType);
		}	
		//free(tempStringType);
		//free(tempComplexType);
	
		if(strcmp(pComplexType, 
					 pListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_) == 0) {
			pcurrent = pListServiceAttributeDescriptor->pAttributeDescriptor_->pListAttrNameList_;
			if(pListServiceAttributeDescriptor->pAttributeDescriptor_->pListAttrMaxOccursList_ != NULL) {
				pcurrentMaxOccrsList = pListServiceAttributeDescriptor->pAttributeDescriptor_->pListAttrMaxOccursList_;
			}
			while((pcurrent != NULL))	{
				pOldListServiceAttributeDescriptor = pTempOldListServiceAttributeDescriptor;
				for (countNext = 0; countNext < attributeDescriptorLength; countNext++) {
					//tempStringType = (char *) calloc(strlen(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_) + 1, sizeof(char));
					//tempComplexType = (char *) calloc(strlen(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_) + 1, sizeof(char));
					strcpy(tempStringType, pOldListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_);
								serviceTypeToken = strtok(tempStringType, ":");
	
				if(strcmp(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_,serviceTypeToken)) {
					getIPDRSubstring(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_, tempComplexType, ':');
					getIPDRSubstring(tempComplexType, tempStringType, '$');
   				strcpy(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_,tempStringType);
				}	
				//free(tempStringType);
				//free(tempComplexType);
	
				if(strcmp(pcurrent->node, 
					   pOldListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_) == 0) {
/*
						tempString = (char *) calloc(strlen(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_) + 1, sizeof(char));
						tempComplexType = (char *) calloc(strlen(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_) + 1, sizeof(char));
						strcpy(tempString, pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_);
						char* serviceTypeToken = strtok(tempString, ":");
	
					if(strcmp(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_,serviceTypeToken)) {
						getIPDRSubstring(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_, 
								tempComplexType, ':');
   					strcpy(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_,tempComplexType);
					}	
					free(tempString);
					free(tempComplexType);
*/
					if(isPrimitiveComplexType(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_) != IPDR_FALSE) {

				typeCode = convAttributeTypeToTypeCode(
   				pOldListServiceAttributeDescriptor->pAttributeDescriptor_->attributeType_);
									
			/*	if(!strcmp(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->attributeType_,
                      pOldListServiceAttributeDescriptor->pAttributeDescriptor_->derivedType_)) */

					switch(typeCode)
					{
						case IPDRDATA_LONG:
              	if((pcurrentMaxOccrsList != NULL)) {
						if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              			noOfElements = 
									pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
						}
						else {
              			noOfElements = atoi(pcurrentMaxOccrsList->node);
						}
              	}
              
              	if (noOfElements > 0) {
						strcat(fieldValue, "("); 
              		pCurrentListIPDRArrayData = pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              		for(cnt = 0;cnt < noOfElements; cnt++) { 
              			sprintf(tempfieldValue, "%ld", 
              							pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRLong_);
							strcat(fieldValue, tempfieldValue);
							strcat(fieldValue, ";"); 
              			pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              		}
						strncpy(tempfieldValue, fieldValue, (strlen(fieldValue)-1));
						strcpy(fieldValue,tempfieldValue);
						strcat(fieldValue, ")");
              	} else {
						sprintf(tempfieldValue,"%ld", 
              					pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRLong_);
						strcat(fieldValue, tempfieldValue);
					}
					break;

					case IPDRDATA_ULONG:
              	if((pcurrentMaxOccrsList != NULL)) {
						if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              			noOfElements = 
									pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
						}
						else {
              			noOfElements = atoi(pcurrentMaxOccrsList->node);
						}
              	}
              
              	if (noOfElements > 0) {
						strcat(fieldValue, "("); 
              		pCurrentListIPDRArrayData = pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              		for(cnt = 0;cnt < noOfElements; cnt++) { 
              			sprintf(tempfieldValue, "%u", 
              							pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRULong_);
							strcat(fieldValue, tempfieldValue);
							strcat(fieldValue, ";"); 
              			pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              		}
						strncpy(tempfieldValue, fieldValue, (strlen(fieldValue)-1));
						strcpy(fieldValue,tempfieldValue);
						strcat(fieldValue, ")");
              	} else {
						sprintf(tempfieldValue,"%u", 
              							pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRULong_);
						strcat(fieldValue, tempfieldValue);
					}
					
				break;

				case IPDRDATA_LONGLONG:
              	if((pcurrentMaxOccrsList != NULL)) {
						if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              			noOfElements = 
									pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
						}
						else {
              			noOfElements = atoi(pcurrentMaxOccrsList->node);
						}
              	}
              
              	if (noOfElements > 0) {
						strcat(fieldValue, "("); 
              		pCurrentListIPDRArrayData = pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              		for(cnt = 0;cnt < noOfElements; cnt++) { 
              			sprintf(tempfieldValue, "%.lf", 
              							(double)pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRLongLong_);
              			/*sprintf(tempfieldValue, "%lld", 
              							pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRLongLong_);*/
							strcat(fieldValue, tempfieldValue);
							strcat(fieldValue, ";"); 
              			pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              		}
						strncpy(tempfieldValue, fieldValue, (strlen(fieldValue)-1));
						strcpy(fieldValue,tempfieldValue);
						strcat(fieldValue, ")");
              	} else {
					 	sprintf(tempfieldValue,"%lld", 
              							pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRLongLong_);
					 	/*sprintf(tempfieldValue,"%.lf", 
              							(double)pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRLongLong_);*/
						strcat(fieldValue, tempfieldValue);
					}
				break; 
		
				case IPDRDATA_ULONGLONG:
              	if((pcurrentMaxOccrsList != NULL)) {
						if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              			noOfElements = 
									pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
						}
						else {
              			noOfElements = atoi(pcurrentMaxOccrsList->node);
						}
              	}
              
              	if (noOfElements > 0) {
						strcat(fieldValue, "("); 
              		pCurrentListIPDRArrayData = pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              		for(cnt = 0;cnt < noOfElements; cnt++) { 
              			/*sprintf(tempfieldValue, "%.lf", 
              							pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRULongLong_);*/
              			sprintf(tempfieldValue, "%llu", 
              							pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRULongLong_);
							strcat(fieldValue, tempfieldValue);
							strcat(fieldValue, ";"); 
              			pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              		}
						strncpy(tempfieldValue, fieldValue, (strlen(fieldValue)-1));
						strcpy(fieldValue,tempfieldValue);
						strcat(fieldValue, ")");
              	} else {
					 	/*sprintf(tempfieldValue,"%.lf", 
              							pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRULongLong_);*/
					 	sprintf(tempfieldValue,"%llu", 
              							pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRULongLong_);
						strcat(fieldValue, tempfieldValue);
					}
				break;

				case IPDRDATA_FLOAT:
              	if((pcurrentMaxOccrsList != NULL)) {
						if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              			noOfElements = 
									pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
						}
						else {
              			noOfElements = atoi(pcurrentMaxOccrsList->node);
						}
              	}
              
              	if (noOfElements > 0) {
						strcat(fieldValue, "("); 
              		pCurrentListIPDRArrayData = pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              		for(cnt = 0;cnt < noOfElements; cnt++) { 
              			sprintf(tempfieldValue, "%f", 
              							pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRFloat_);
							strcat(fieldValue, tempfieldValue);
							strcat(fieldValue, ";"); 
              			pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              		}
						strncpy(tempfieldValue, fieldValue, (strlen(fieldValue)-1));
						strcpy(fieldValue,tempfieldValue);
						strcat(fieldValue, ")");
              	} else {
					  sprintf(tempfieldValue,"%f", 
              							pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRFloat_);
					  strcat(fieldValue, tempfieldValue);
					}
				break;	  

				case IPDRDATA_DOUBLE:
              	if((pcurrentMaxOccrsList != NULL)) {
						if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              			noOfElements = 
									pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
						}
						else {
              			noOfElements = atoi(pcurrentMaxOccrsList->node);
						}
              	}
              
              	if (noOfElements > 0) {
						strcat(fieldValue, "("); 
              		pCurrentListIPDRArrayData = pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              		for(cnt = 0;cnt < noOfElements; cnt++) { 
              			sprintf(tempfieldValue, "%lf", 
              							pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRDouble_);
							strcat(fieldValue, tempfieldValue);
							strcat(fieldValue, ";"); 
              			pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              		}
						strncpy(tempfieldValue, fieldValue, (strlen(fieldValue)-1));
						strcpy(fieldValue,tempfieldValue);
						strcat(fieldValue, ")");
              	} else {
					 	sprintf(tempfieldValue,"%lf", 
              							pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRDouble_);
					  	strcat(fieldValue, tempfieldValue);
					}
				break;

				case IPDRDATA_HEXADECARRAY:	 
              	if((pcurrentMaxOccrsList != NULL)) {
						if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              			noOfElements = 
									pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
						}
						else {
              			noOfElements = atoi(pcurrentMaxOccrsList->node);
						}
              	}
              
              	if (noOfElements > 0) {
						strcat(fieldValue, "("); 
              		pCurrentListIPDRArrayData = pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              		for(cnt = 0;cnt < noOfElements; cnt++) { 
              			sprintf(tempfieldValue, "%s", 
              							pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRByteArray_);
							strcat(fieldValue, tempfieldValue);
							strcat(fieldValue, ";"); 
              			pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              		}
						strncpy(tempfieldValue, fieldValue, (strlen(fieldValue)-1));
						strcpy(fieldValue,tempfieldValue);
						strcat(fieldValue, ")");
              	} else {
					  sprintf(tempfieldValue,"%s", 
              							pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRByteArray_);
					  strcat(fieldValue, tempfieldValue);
					}
				break;					  

				case IPDRDATA_STRING:
              	if((pcurrentMaxOccrsList != NULL)) {
						if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              			noOfElements = 
									pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
						}
						else {
              			noOfElements = atoi(pcurrentMaxOccrsList->node);
						}
              	}
              
              	if (noOfElements > 0) {
						strcat(fieldValue, "("); 
              		pCurrentListIPDRArrayData = pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              		for(cnt = 0;cnt < noOfElements; cnt++) { 
              			sprintf(tempfieldValue, "%s", 
              							writeEscapedChar((char *)pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_));
							strcat(fieldValue, tempfieldValue);
							strcat(fieldValue, ";"); 
              			pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              		}
						strncpy(tempfieldValue, fieldValue, (strlen(fieldValue)-1));
						strcpy(fieldValue,tempfieldValue);
						strcat(fieldValue, ")");
              	} else {
					  sprintf(tempfieldValue,"%s", 
              							writeEscapedChar((char *)pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_));
					  strcat(fieldValue, tempfieldValue);
					}
					 break;

				case IPDRDATA_BOOLEAN: 
              	if((pcurrentMaxOccrsList != NULL)) {
						if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              			noOfElements = 
									pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
						}
						else {
              			noOfElements = atoi(pcurrentMaxOccrsList->node);
						}
              	}
              
              	if (noOfElements > 0) {
						strcat(fieldValue, "("); 
              		pCurrentListIPDRArrayData = pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              		for(cnt = 0;cnt < noOfElements; cnt++) { 
              			sprintf(tempfieldValue, "%s", 
              							pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRBoolean_);
							strcat(fieldValue, tempfieldValue);
							strcat(fieldValue, ";"); 
              			pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              		}
						strncpy(tempfieldValue, fieldValue, (strlen(fieldValue)-1));
						strcpy(fieldValue,tempfieldValue);
						strcat(fieldValue, ")");
              	} else {
					  sprintf(tempfieldValue,"%s", 
              							pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRBoolean_);
					  strcat(fieldValue, tempfieldValue);
					}
					 break;
 
				case IPDRDATA_SHORT:
              	if((pcurrentMaxOccrsList != NULL)) {
						if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              			noOfElements = 
									pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
						}
						else {
              			noOfElements = atoi(pcurrentMaxOccrsList->node);
						}
              	}
              
              	if (noOfElements > 0) {
						strcat(fieldValue, "("); 
              		pCurrentListIPDRArrayData = pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              		for(cnt = 0;cnt < noOfElements; cnt++) { 
              			sprintf(tempfieldValue, "%ld", 
              							pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRShort_);
							strcat(fieldValue, tempfieldValue);
							strcat(fieldValue, ";"); 
              			pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              		}
						strncpy(tempfieldValue, fieldValue, (strlen(fieldValue)-1));
						strcpy(fieldValue,tempfieldValue);
						strcat(fieldValue, ")");
              	} else {
						sprintf(tempfieldValue,"%ld", 
              					pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRShort_);
						strcat(fieldValue, tempfieldValue);
					}
				break;

				case IPDRDATA_USHORT:
              	if((pcurrentMaxOccrsList != NULL)) {
						if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              			noOfElements = 
									pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
						}
						else {
              			noOfElements = atoi(pcurrentMaxOccrsList->node);
						}
              	}
              
              	if (noOfElements > 0) {
						strcat(fieldValue, "("); 
              		pCurrentListIPDRArrayData = pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              		for(cnt = 0;cnt < noOfElements; cnt++) { 
              			sprintf(tempfieldValue, "%u", 
              							pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUShort_);
							strcat(fieldValue, tempfieldValue);
							strcat(fieldValue, ";"); 
              			pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              		}
						strncpy(tempfieldValue, fieldValue, (strlen(fieldValue)-1));
						strcpy(fieldValue,tempfieldValue);
						strcat(fieldValue, ")");
              	} else {
						sprintf(tempfieldValue,"%u", 
              					pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUShort_);
						strcat(fieldValue, tempfieldValue);
					}
					break;
					
					 default:
   					return(IPDR_NON_PRIMITIVE_DATA_TYPE);
                  break;
				}
				strcat(fieldValue, ";");
				break;
			} else {
					//The child element of the structure is a structure			
					getComplexFieldValue(pIPDRCommonParameters,
												fieldValue,
												pOldListServiceAttributeDescriptor->pAttributeDescriptor_,
												pTempOldListServiceAttributeDescriptor,							
												pServiceInfo,
												pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_,
									  			pListIPDRData,
												pErrorCode
											  );	
					strcat(fieldValue, "]");
			 }
			}
				pOldListServiceAttributeDescriptor = pOldListServiceAttributeDescriptor->pNext_;
		}
				pcurrent = pcurrent->pNext;
				if(pcurrentMaxOccrsList != NULL)
						pcurrentMaxOccrsList = pcurrentMaxOccrsList->pNext;
				pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_ = 
				pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pNext_;
		}
	  }		
		pListServiceAttributeDescriptor = pListServiceAttributeDescriptor->pNext_;
      pListIPDRData->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_ = pTempListIPDRComplex;
	}
	
   pListServiceAttributeDescriptor = pCurrentListServiceAttributeDescriptor;
	
	strncpy(tempfieldValue, fieldValue, (strlen(fieldValue)-1));
	strcpy(fieldValue,tempfieldValue);
	strcat(fieldValue, "]");

	while(tempfieldValue != NULL) {
		free(tempfieldValue);	
		tempfieldValue = NULL;
	}

	while(tempString != NULL) {
		free(tempString);	
		tempString = NULL;
	}

/*
	if(tempfieldValue != NULL)
		free(tempfieldValue);

	if (tempString != NULL)
		free(tempString);
*/

	if (tempStringType != NULL)
		free(tempStringType);

	if (tempComplexType != NULL)
		free(tempComplexType);

	tempfieldValue = NULL;
	tempString = NULL;
   tempComplexType = NULL;
	tempStringType = NULL;

	free(resultGet);
   free(tempXMLString);

	return(IPDR_OK);
}


/*!
 *  NAME:
 *      validateInputParams() - This function validates the Input           
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


int validateInputParams(
						int argCount, 
						char* argValues[],
						unsigned int* schemaValFlag,
						unsigned int* outDirFlag,
						unsigned int* versionFlag,
						char* outFileDir,
						char* ipdrVer,
						IPDRCommonParameters* pIPDRCommonParameters,
						int* pErrorCode
					   )
{
	unsigned int countFileChars = 0;
	unsigned int countArguments = 0;
	unsigned int countValidArgs = 0;
	unsigned int lengthInFile = 0;
	unsigned int lenDocType = 0;
	unsigned int lFileLength = 0;
	char* docType = NULL;
	char* inFileName = NULL;
	char* tempString = NULL;
	char* strTemp = NULL;

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
				
		/*Catches the input file*/
		if(fopen(argValues[countArguments],"rb+") != NULL) {
		   lFileLength = getFileLength(argValues[countArguments]);
			if(lFileLength == 0) {
				*pErrorCode = IPDR_NO_DATA_IN_FILE;
				return(IPDR_ERROR);
			}
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
                                    fopen(pIPDRCommonParameters->pStreamName_, "rb+");
		}
	}
	

 /* Checks if number of valid input parameters is same as the number of 
     input parameters*/

	if ((countValidArgs < (unsigned int) (argCount) )||(argCount == 1)){
		printf("\nIPDR_MISSING_OR_INVALID_COMMANDLINE_PARAMS");
		printUsage();
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
 *      populateIPDRCommonParameters() - This function populates
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


int populateIPDRCommonParameters(
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
 *      printUsage() - Prints test tool usage syntax. 
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

int printUsage()
{
	printf("\nCorrect Input String is: Infile.xdr -doctype XDR <-schemavalidation> <-ipdrversion 3.5/3.1> -outFiledir <-output directory>");
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



int freeListServiceInfo(ListServiceInfo** pHeadRef)
{
	ListServiceInfo* pCurrent;
	ListServiceInfo* pNext_;

	if (pHeadRef == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}
 
	pCurrent = *pHeadRef;
        while (pCurrent != NULL) {
            pNext_ = pCurrent->pNext_;
		freeListNameSpaceInfo(&(pCurrent->pServiceInfo_->pListNameSpaceInfo_));
		free(pCurrent->pServiceInfo_);
	    free(pCurrent);
	    pCurrent = pNext_;
    }
    free(pCurrent);
	return (IPDR_OK);
}

char* writeEscapedChar(char* str)
{
    /* First encode all special characters */
    //str = encodeStr(str);
    str = insertEscChar(str, '\\');
    str = insertEscChar(str, '[');
    str = insertEscChar(str, ']');
    str = insertEscChar(str, '(');
    str = insertEscChar(str, ')');
    str = insertEscChar(str, ';');
    str = insertEscChar(str, ',');
    //str = insertEscChar(str, '\\');
                                                                                                                             
    return str;
}

char* insertEscChar(char *string, char delim)
{
    int counter = 0;
    char* delimIndex; 
    char* startIndex ;
    char* endIndex;
    char* preDelim;
    char* postDelim;
    char* toEncode;
    char* val = NULL;
    char* escapedChar;
    char interimStr[3];
    char tail[3];

    tail[0] = '\\';
    tail[1] = delim;
    tail[2] = '\0';
    val = (char *) malloc (strlen(string) + 20);

    delimIndex = Findindex(string, delim);
    if (delimIndex == NULL)
       return string;

    while (delimIndex != NULL)
    {
        startIndex = delimIndex;
        endIndex   = delimIndex + 1;

        preDelim = getStringBetweenDelims(string, NULL, startIndex - 1);
        preDelim = strcat(preDelim, tail);
        postDelim = getStringBetweenDelims(string, endIndex,  NULL);
        

        //strcpy(string , (strcat( strcat(preDelim, tail), (const char*)postDelim)));
        //strcpy(string , ( strcat(preDelim, postDelim)));
        strcpy(val , ( strcat(preDelim, postDelim)));
        delimIndex = Findindex(delimIndex+1, delim);
		  strcpy(string, val);
		  if(delimIndex != NULL)
		  		delimIndex = delimIndex + 1;
        //free(preDelim);
        //free(postDelim);
    }
    strcpy(string, val);
    free(val);
    return string;
}

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
