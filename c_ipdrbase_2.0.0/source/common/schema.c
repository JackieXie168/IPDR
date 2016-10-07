/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/


/**********************************************************************
* File                 : Schema.c                                     *
* Description          : The Schema file sets the XML data into       * 
*                        the hashtable                                *
* Author               : Infosys Tech Ltd.                            *   
* Modification History :                                              * 
* ----------------------------------------------------------------    *
*    Date      Name     Change/Description                            * 
*                                                                     * 
*                                                                     *
* ----------------------------------------------------------------    * 
**********************************************************************/

#include "common/schema.h"
#include "common/IPDRDocWriter.h"

char gSchemaVersion[10];

static char serviceType[50];

/*
*NAME:
*     setSchemaData() 
*
*DESCRIPTION:
*   This method will help in populating SchemaLookupHashTable in the Schema 
* structure. It will assign the value of ListNameSpaceInfo to member 
* variable pListNameSpaceInfo_ and  serviceDefinitionURIs to member variable 
* pListServiceDefinitionURI_. It will parse the service definition schemas 
* and store the attribute name,type and  presence (required or optional). 
* It will make Schema validation flag to true after successful completion 
* of its functionality. (this flag will not be exposed to calling 
* application)
* 
*ARGUMENTS:
* - Linked List of NameSpaceInfo { IN }
* - Linked List of ServiceDefinitionURIs {IN } 
*      - String containg Default NameSpace {IN}
*      - Pointer to IPDRCommonParameters {OUT}
*      - Integer variable to store the error code if it originates
*        in the function {OUT}   
*
*RETURNS 
*     Returns Zero or appropriate error code
*/

int setSchemaData( 
				   IPDRCommonParameters *pIPDRCommonParameters,
				   ListNameSpaceInfo* pListNameSpaceInfo,
				   ListServiceDefinitionURI* pListServiceDefinitionURI,
				   char* defaultNameSpace,
				   int* pErrorCode
				  )
{
	int errorSeverity = 0;
	char tempSpaceURI[MAX_LEN];
	char tempDefaultSpaceURI[MAX_LEN];
	SchemaLookup* pSchemaLookup = NULL;
	ListNameSpaceInfo* pTempListNameSpaceInfo = NULL;
	ListNameSpaceInfo* pCurrentListNameSpaceInfo = NULL;
	ListServiceDefinitionURI* pFirstListServiceDefinitionURI = NULL;
	ListServiceDefinitionURI* pTempListServiceDefinitionURI = NULL;
	ListNameSpaceInfo* pSchemaListNameSpaceInfo = NULL;

	/* Checking for NULL input parameters */
	/*if ( pListNameSpaceInfo == NULL || defaultNameSpace == NULL ||
		pListServiceDefinitionURI == NULL || pIPDRCommonParameters == NULL) */
	if ( defaultNameSpace == NULL ||
		pListServiceDefinitionURI == NULL || pIPDRCommonParameters == NULL) {
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		return(IPDR_ERROR);
	}

	/* Setting Schema flag to TRUE */
	pIPDRCommonParameters->pSchema_->SchemaValidationFlag_= IPDR_FALSE;

    /* Copying the elements of pListNameSpaceInfo into
	   pTempListNameSpaceInfo */

	pCurrentListNameSpaceInfo = pListNameSpaceInfo;
	while(pListNameSpaceInfo != NULL) {
		appendListNameSpaceInfo(&pTempListNameSpaceInfo,
						pListNameSpaceInfo->NameSpaceInfo_.nameSpaceID_,
						pListNameSpaceInfo->NameSpaceInfo_.nameSpaceURI_); 
		
		pListNameSpaceInfo = pListNameSpaceInfo->pNext_; 					 
	}
	pListNameSpaceInfo = pCurrentListNameSpaceInfo;
	

	/* 
	Validation 
	   1. If namespaceId is NULL, then nameSpaceURI must correspond to
	      defaultNameSpace
	   2. If namespaceId is NOT NULL, then nameSpaceURI must NOT 
	      correspond to defaultNameSpace
	Returns ERROR if any of above validation fails
	*/

	pCurrentListNameSpaceInfo = pTempListNameSpaceInfo;
	while(pTempListNameSpaceInfo != NULL) {
		if(strcmp(pTempListNameSpaceInfo->NameSpaceInfo_.nameSpaceID_, "") == 0) {
			convertTolower(pTempListNameSpaceInfo->NameSpaceInfo_.nameSpaceURI_, tempSpaceURI);
			convertTolower(defaultNameSpace, tempDefaultSpaceURI);
			/*if(strcmp(pTempListNameSpaceInfo->NameSpaceInfo_.nameSpaceURI_, defaultNameSpace) != 0)
			strcpy(pTempListNameSpaceInfo->NameSpaceInfo_.nameSpaceURI_, tempSpaceURI);*/
			if(strcmp(tempSpaceURI, tempDefaultSpaceURI) != 0) {
				*pErrorCode = IPDR_MISMATCH_NS_DFLT_NS;
				freeListNameSpaceInfo(&pTempListNameSpaceInfo);
				return(IPDR_ERROR);
			}
		} else {
			if(strcmp(pTempListNameSpaceInfo->NameSpaceInfo_.nameSpaceURI_, defaultNameSpace) == 0) {
				*pErrorCode = IPDR_MACTH_ONS_DNS;
				freeListNameSpaceInfo(&pTempListNameSpaceInfo);
				return(IPDR_ERROR);
			}
		}
		pTempListNameSpaceInfo = pTempListNameSpaceInfo->pNext_; 
	}
	pTempListNameSpaceInfo = pCurrentListNameSpaceInfo;


    /* Copying the elements of pListServiceDefinitionURI into
	   pTempListServiceDefinitionURI */
    
	pFirstListServiceDefinitionURI = pListServiceDefinitionURI;
	while(pListServiceDefinitionURI != NULL) {
		appendListServiceDefinitionURI(&pTempListServiceDefinitionURI,
										pListServiceDefinitionURI->pServiceDefintionURI_
										);
		pListServiceDefinitionURI = pListServiceDefinitionURI->pNext_; 
	}
	pListServiceDefinitionURI = pFirstListServiceDefinitionURI;


	/* Parsing all the serviceDefinitionURIs & populating 
	   pIPDRCommonParameters->pSchema_->pListSchemaLookup_ */
	
	pFirstListServiceDefinitionURI = pTempListServiceDefinitionURI;

	while (pTempListServiceDefinitionURI != NULL) {
		
		pSchemaLookup = newSchemaLookup();

		errorSeverity = generateSchemaLookup(
			                    pTempListServiceDefinitionURI->pServiceDefintionURI_,
								pSchemaLookup,
								&pSchemaListNameSpaceInfo,
								pIPDRCommonParameters,
								pErrorCode
								);
		if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "generateSchemaDescriptor");	
		}
		else if (errorSeverity == IPDR_ERROR) {
			exceptionHandler(pIPDRCommonParameters, pErrorCode, "generateSchemaDescriptor");	
			freeListNameSpaceInfo(&pTempListNameSpaceInfo);
			freeListServiceDefinitionURI(&pTempListServiceDefinitionURI);  
			freeSchemaLookup(pSchemaLookup); 
			return (IPDR_ERROR);
		}			


		#ifdef DEBUG_IPDR	
			printListSchemaElement(pSchemaLookup->pListSchemaElement_);
			printListSchemaElement(pSchemaLookup->pListServiceElement_);
		#endif

		pIPDRCommonParameters->pSchema_->pListNameSpaceInfo_ = pSchemaListNameSpaceInfo; 
		
		if(strcmp(pSchemaLookup->serviceType_, "") == 0) {
			*pErrorCode = IPDR_ST_FOR_SCHEMA_ABSENT;
			freeListNameSpaceInfo(&pTempListNameSpaceInfo);
			freeListServiceDefinitionURI(&pTempListServiceDefinitionURI);  
			freeSchemaLookup(pSchemaLookup); 
			return( IPDR_ERROR );   
		}
		
		
		appendListSchemaLookup(&pIPDRCommonParameters->pSchema_->pListSchemaLookup_,
								pSchemaLookup);

		freeSchemaLookup(pSchemaLookup);
		
		pTempListServiceDefinitionURI = pTempListServiceDefinitionURI->pNext_; 
	}
	pTempListServiceDefinitionURI = pFirstListServiceDefinitionURI;


	/* Setting Schema flag to TRUE */
	pIPDRCommonParameters->pSchema_->SchemaValidationFlag_= IPDR_TRUE;

	freeListNameSpaceInfo(&pTempListNameSpaceInfo);
	freeListServiceDefinitionURI(&pTempListServiceDefinitionURI);  

	return (IPDR_OK);
}


SchemaCommon* newSchemaCommon(void)
{
	SchemaCommon* pSchemaCommon = NULL;

	pSchemaCommon = (SchemaCommon *) 
		calloc (1, sizeof(SchemaCommon));
	pSchemaCommon->pSchemaLookup_ = newSchemaLookup();
	pSchemaCommon->pSchemaLookup_->serviceType_ = NULL;
	pSchemaCommon->recordFlag_ = 0;
   pSchemaCommon->complexTypeFlag_ == 0;
	pSchemaCommon->serviceFlag_ = 0;
	pSchemaCommon->docFlag_ = 0;
	pSchemaCommon->unitFlag_ = 0;
	pSchemaCommon->statusFlag_ = 0;
	pSchemaCommon->referenceFlag_ = 0;

	pSchemaCommon->pListNameSpaceInfo_ = NULL;
	
	pSchemaCommon->targetNameSpace_ = (char *) 
		calloc (MAX_NAMESPACE_URI, sizeof(char));

	pSchemaCommon->schemaLocation_ = (char *) 
		calloc (MAX_NAMESPACE_URI, sizeof(char));
	
	pSchemaCommon->nameSpaceId_ = NULL;

	pSchemaCommon->oldDoc_ = NULL;
	return (pSchemaCommon);

}


int freeSchemaCommon(SchemaCommon* pSchemaCommon)
{
	freeSchemaElement(pSchemaCommon->pSchemaElement_);
	freeSchemaLookup(pSchemaCommon->pSchemaLookup_);
	freeListNameSpaceInfo(&(pSchemaCommon->pListNameSpaceInfo_));
	free(pSchemaCommon->targetNameSpace_);
	free(pSchemaCommon->nameSpaceId_);
	free(pSchemaCommon->schemaLocation_);
	free(pSchemaCommon->extensionBase_);
	freeList(&(pSchemaCommon->lstMinOccurs_));
	freeList(&(pSchemaCommon->lstReference_));
	freeList(&(pSchemaCommon->lstSchemaElementMinOccurs_));
	freeList(&(pSchemaCommon->lstSchemaElementMaxOccurs_));
	freeList(&(pSchemaCommon->lstSchemaElementReference_));
	free(pSchemaCommon);
	return (IPDR_OK);

}


int getStringNoTabs(char* dest, char* src)
{
	int i = 0;
	int j = 0;

	if ((dest == NULL) || (src == NULL))
		printf("\nInput string passed as NULL");
	for (i = 0; src[i] == 9; i++);
	while (src[i] != '\0') dest[j++] = src[i++];
	dest[j] = '\0';
	return 0;
}



static void start(
		void *data, 
		const char *el, 
		const char **attr) 
{
	int i = 0;
	int tmpLen = 0;
	char* tmp = NULL;
   static int cnt = 0; 

	SchemaCommon *pSchemaCommon = NULL;
	pSchemaCommon = (SchemaCommon *) data;

	if (strcmp(el, "schema") == 0) {
		while (attr[i] != NULL) {

			if (strcmp(attr[i], "targetNamespace") == 0) {
				i++;
				strcpy(pSchemaCommon->targetNameSpace_, attr[i]);
			}
   
			if (( attr[i][0] == 'x' ) &&
				( attr[i][1] == 'm' ) &&
				( attr[i][2] == 'l' ) &&
				( attr[i][3] == 'n' ) &&
				( attr[i][4] == 's' )) {
				if (strcmp(pSchemaCommon->targetNameSpace_, attr[i + 1]) == 0) {
					tmpLen = 0;
					tmpLen = strlen(attr[i]) + 1;
					pSchemaCommon->nameSpaceId_ = (char *) calloc (tmpLen, sizeof(char));
		
					getIPDRSubstring((char *) attr[i], pSchemaCommon->nameSpaceId_, ':');
				} 
				i++;
			}
                        if ( strcmp(attr[i], "version") == 0) {
                             i++;
                             printf("the version read from schema is: %s\n", attr[i]);
                             cnt = cnt + 1;
                             if (cnt == 1) {
                                if(strstr(attr[i],IPDR_VERSION_3_5) != NULL){
				    							setSchemaVersion(3.5);
                                }
                                else if((strstr(attr[i],IPDR_VERSION_3_0) != NULL) || (strstr(attr[i],IPDR_VERSION_3_1) != NULL)){
				    							setSchemaVersion(3.1);
                                }
                             }
			}
			i++;            
		}
	}

 
 

 /* Getting extension base */
 	if (strcmp(el, "extension") == 0) {
            if (strcmp(attr[i], "base") == 0) {
   		i++;
   		tmpLen = 0;
   		tmpLen = strlen(attr[i]) + 1;
   		pSchemaCommon->extensionBase_ = (char *) calloc (tmpLen, sizeof(char));
   		strcpy(pSchemaCommon->extensionBase_, attr[i]);
  	    }
 	}
 
 /* Getting include schemaLocation_ */
 if (strcmp(el, "include") == 0) {
        if (strcmp(attr[i], "schemaLocation") == 0) {
   i++;
   strcpy(pSchemaCommon->schemaLocation_, attr[i]);
  }
 }


 /* Getting include schemaLocation_ */
 if (strcmp(el, "import") == 0) {
  while(attr[i] != NULL) {
   if (strcmp(attr[i], "schemaLocation") == 0) {
    i++;
    strcpy(pSchemaCommon->schemaLocation_, attr[i]);
   }
   i++;
  }
 }

 /* New code ends */


	if (strcmp(el, "element") == 0) {
		while (attr[i] != NULL) {
			if (strcmp(attr[i], "name") == 0) {
				pSchemaCommon->recordFlag_ = 1;
				i++;
				tmpLen = 0;
				if ((pSchemaCommon->nameSpaceId_ == NULL) || 
					(strcmp(pSchemaCommon->nameSpaceId_, "ipdr") == 0)) {
				tmpLen = strlen(attr[i]) + 1;
				if (pSchemaCommon->pSchemaElement_ != NULL) {
					if (pSchemaCommon->pSchemaElement_->documentation_ != NULL) {
						free(pSchemaCommon->pSchemaElement_->documentation_);
						pSchemaCommon->pSchemaElement_->documentation_ = NULL;
					}
					
					free(pSchemaCommon->pSchemaElement_);
					pSchemaCommon->pSchemaElement_ = NULL;
				}
				
				pSchemaCommon->pSchemaElement_ = (SchemaElement *) calloc (1, sizeof(SchemaElement));
				pSchemaCommon->pSchemaElement_->attributeName_ = (char *) calloc (tmpLen, sizeof(char));
				strcpy(pSchemaCommon->pSchemaElement_->attributeName_, attr[i]);
				} else {
					if (pSchemaCommon->pSchemaElement_ != NULL) {
					
						free(pSchemaCommon->pSchemaElement_);
						pSchemaCommon->pSchemaElement_ = NULL;
					}
				

				tmpLen = strlen(attr[i]) + strlen(pSchemaCommon->nameSpaceId_) + 5;
				
				pSchemaCommon->pSchemaElement_ = (SchemaElement *) calloc (1, sizeof(SchemaElement));
				pSchemaCommon->pSchemaElement_->attributeName_ = (char *) calloc (tmpLen, sizeof(char));
				strcpy(pSchemaCommon->pSchemaElement_->attributeName_, pSchemaCommon->nameSpaceId_);
				strcat(pSchemaCommon->pSchemaElement_->attributeName_, ":");
				strcat(pSchemaCommon->pSchemaElement_->attributeName_, attr[i]);
				}
			}

			if ((strcmp(attr[i], "type") == 0) && (pSchemaCommon->pSchemaElement_ != NULL))  {
				i++;
				tmpLen = 0;
				tmpLen = strlen(attr[i]) + 1;
				pSchemaCommon->pSchemaElement_->attributeType_ = (char *) calloc (tmpLen, sizeof(char));
				strcpy(pSchemaCommon->pSchemaElement_->attributeType_, attr[i]);
				
				pSchemaCommon->pSchemaElement_->pAttributeOptional_ = 0; /* Mandatory */
		
				pSchemaCommon->pSchemaElement_->derivedType_ = (char *) calloc (tmpLen, sizeof(char));
				strcpy(pSchemaCommon->pSchemaElement_->derivedType_, attr[i]);

				pSchemaCommon->pSchemaElement_->complexAttributeType_ = (char *) calloc (tmpLen, sizeof(char));
				strcpy(pSchemaCommon->pSchemaElement_->complexAttributeType_, attr[i]);
				}
			if (strcmp(attr[i], "ref") == 0) {
            if(pSchemaCommon->complexTypeFlag_ == 0) {
					i++;
					tmpLen = 0;
					tmpLen = strlen(attr[i]) + 1;
					tmp = (char *) calloc (tmpLen, sizeof(char));
					getIPDRSubstring((char *) attr[i], tmp, ':');

					appendNode(&(pSchemaCommon->lstReference_),
						tmp);
					appendNode(&(pSchemaCommon->lstMinOccurs_),
						"Mandatory");
					appendNode(&(pSchemaCommon->lstMaxOccurs_), "0");
					free(tmp);
            }
            else {
					i++;
					tmpLen = 0;
					tmpLen = strlen(attr[i]) + 1;
					tmp = (char *) calloc (tmpLen, sizeof(char));
					getIPDRSubstring((char *) attr[i], tmp, ':');

					appendNode(&(pSchemaCommon->lstSchemaElementReference_),
						tmp);
					appendNode(&(pSchemaCommon->lstSchemaElementMinOccurs_),
						"Mandatory");
					appendNode(&(pSchemaCommon->lstSchemaElementMaxOccurs_), "0");
					pSchemaCommon->recordFlag_ = 0;
					free(tmp);
				}
			}

			if (strcmp(attr[i], "minOccurs") == 0) {
            if(pSchemaCommon->complexTypeFlag_ == 0) {
				i++;
				tmpLen = 0;
				tmpLen = strlen(attr[i]) + 1;
				tmp = (char *) calloc (tmpLen, sizeof(char));
				strcpy(tmp, attr[i]);
				tmpLen = 0;
				tmpLen = getLength(pSchemaCommon->lstMinOccurs_);
				freeListNode(&(pSchemaCommon->lstMinOccurs_), tmpLen);
            if(strcmp(tmp,"0") == 0)
					appendNode(&(pSchemaCommon->lstMinOccurs_), "Optional");
            else
					appendNode(&(pSchemaCommon->lstMinOccurs_), "Mandatory");
				appendNode(&(pSchemaCommon->lstMaxOccurs_), "0");
				free(tmp);
				}
				else {
				i++;
				tmpLen = 0;
				tmpLen = strlen(attr[i]) + 1;
				tmp = (char *) calloc (tmpLen, sizeof(char));
				strcpy(tmp, attr[i]);
				tmpLen = 0;
				tmpLen = getLength(pSchemaCommon->lstSchemaElementMinOccurs_);
				freeListNode(&(pSchemaCommon->lstSchemaElementMinOccurs_), tmpLen);
            if(strcmp(tmp,"0") == 0)
					appendNode(&(pSchemaCommon->lstSchemaElementMinOccurs_), "Optional");
            else
					appendNode(&(pSchemaCommon->lstSchemaElementMinOccurs_), "Mandatory");
				free(tmp);
				}
			}

			/*IPDR v3.5, Arrays*/
			if((strcmp(gSchemaVersion,IPDR_VERSION_3_5)) == 0) {
			if (strcmp(attr[i], "maxOccurs") == 0) {
            if(pSchemaCommon->complexTypeFlag_ == 0) {
				i++;
				tmpLen = 0;
				tmpLen = strlen(attr[i]) + 1;
				tmp = (char *) calloc (tmpLen, sizeof(char));
				strcpy(tmp, attr[i]);
				tmpLen = 0;
				tmpLen = getLength(pSchemaCommon->lstMaxOccurs_);
				freeListNode(&(pSchemaCommon->lstMaxOccurs_), tmpLen);
				if(strcmp(tmp, "unbounded") == 0) {
					pSchemaCommon->pSchemaElement_->noOfElements_ = -1;
					appendNode(&(pSchemaCommon->lstMaxOccurs_), "-1");
				}
				else {
					pSchemaCommon->pSchemaElement_->noOfElements_ = atoi(tmp);
					appendNode(&(pSchemaCommon->lstMaxOccurs_), tmp);
				}

				free(tmp);
				}
				else {
				i++;
				tmpLen = 0;
				tmpLen = strlen(attr[i]) + 1;
				tmp = (char *) calloc (tmpLen, sizeof(char));
				strcpy(tmp, attr[i]);
				tmpLen = 0;
				tmpLen = getLength(pSchemaCommon->lstSchemaElementMaxOccurs_);
				freeListNode(&(pSchemaCommon->lstSchemaElementMaxOccurs_), tmpLen);
				if(strcmp(tmp, "unbounded") == 0) {
					appendNode(&(pSchemaCommon->lstSchemaElementMaxOccurs_), "-1");
				}
				else {
					appendNode(&(pSchemaCommon->lstSchemaElementMaxOccurs_), tmp);
				}
				free(tmp);
				}
			}
			}
			i++;
		}
	} 


/* IPDR v3.5,  Enum requirement. Check for version */
 if((strcmp(gSchemaVersion,IPDR_VERSION_3_5)) == 0)
 {
	if (strcmp(el, "restriction") == 0) {
		while (attr[i] != NULL) {
			if (strcmp(attr[i], "base") == 0) {
				i++;
				tmpLen = 0;
				tmpLen = strlen(attr[i]) + 1;
				if (pSchemaCommon->pSchemaElement_ != NULL) {
					pSchemaCommon->pSchemaElement_->attributeType_ = (char *) calloc (tmpLen, sizeof(char));
					strcpy(pSchemaCommon->pSchemaElement_->attributeType_, attr[i]);
				
					pSchemaCommon->pSchemaElement_->pAttributeOptional_ = 0; 

					pSchemaCommon->pSchemaElement_->derivedType_ = (char *) calloc (tmpLen, sizeof(char));
					strcpy(pSchemaCommon->pSchemaElement_->derivedType_, attr[i]);
				}
         }
         i++;
     }
   }
}

	if ((strcmp(el, "enumeration") == 0) && (pSchemaCommon->pSchemaElement_ != NULL)) {
		while (attr[i] != NULL) {
			if (strcmp(attr[i], "value") == 0) {
				i++;
			#ifdef DEBUG_IPDR	
				printf("\nAppending %s", (char *) attr[i]);
			#endif
				appendNode(&(pSchemaCommon->pSchemaElement_->pListAttributeEnum_),
					(char *) attr[i]);
				}
			i++;
		}
	}

	if ((strcmp(el, "documentation") == 0) && (pSchemaCommon->pSchemaElement_ != NULL)) {
		pSchemaCommon->docFlag_ = 1;
	}

	if ((strcmp(el, "ipdr:units") == 0) && (pSchemaCommon->pSchemaElement_ != NULL)) {
		pSchemaCommon->unitFlag_ = 1;
	}

	if ((strcmp(el, "ipdr:status") == 0) && (pSchemaCommon->pSchemaElement_ != NULL)) {
		pSchemaCommon->statusFlag_ = 1;
	}

	if ((strcmp(el, "ipdr:reference") == 0) && (pSchemaCommon->pSchemaElement_ != NULL)) {
		pSchemaCommon->referenceFlag_ = 1;
	}

	if ((strcmp(el, "complexType") == 0) && (pSchemaCommon->serviceFlag_ == 0)) {
        	if(attr[i] == NULL){
				pSchemaCommon->complexTypeFlag_ = 1;
				tmpLen = 0;
				tmpLen = strlen("structure") + 1;
				pSchemaCommon->pSchemaElement_->attributeType_ = (char *) calloc (tmpLen, sizeof(char));
				strcpy(pSchemaCommon->pSchemaElement_->attributeType_, "structure");
				
				pSchemaCommon->pSchemaElement_->pAttributeOptional_ = 0; /* Mandatory */
		
				pSchemaCommon->pSchemaElement_->derivedType_ = (char *) calloc (tmpLen, sizeof(char));
				strcpy(pSchemaCommon->pSchemaElement_->derivedType_, "structure");
         }
			else {
				while (attr[i] != NULL) {
					if (strcmp(attr[i], "name") == 0) {
						i++;
						tmpLen = 0;
						tmpLen = strlen(attr[i]) + 1;
						pSchemaCommon->pSchemaLookup_->serviceType_ = (char *) calloc (tmpLen, sizeof(char));
						strcpy(pSchemaCommon->pSchemaLookup_->serviceType_, attr[i]);
						pSchemaCommon->pSchemaElement_->attributeName_ = (char *) calloc (tmpLen, sizeof(char));
						strcpy(pSchemaCommon->pSchemaElement_->attributeName_, attr[i]);
						pSchemaCommon->serviceFlag_ = 0;
						pSchemaCommon->complexTypeFlag_ = 1;
						pSchemaCommon->recordFlag_ = 1;
					}
					i++;
				}
			}
   }

	if ((strcmp(el, "complexContent") == 0)) {
		pSchemaCommon->serviceFlag_ = 1;
		pSchemaCommon->complexTypeFlag_ = 0;
		pSchemaCommon->recordFlag_ = 0;
	}

	while (attr[i] != NULL) {
		i++;
	}
 
	data = (char *) el;
}  



static void
end(void *data, const char *el) {
	int i = 0;
	int count = 0;
	SchemaCommon *pSchemaCommon = NULL;
	ListSchemaElement* pCurrent = NULL;
	List* pCurrentRef = NULL;
	List* pCurrentOpt = NULL;
	List* pCurrentMax = NULL;
	List* pFirstRef = NULL;
	List* pFirstOpt = NULL;
	List* pFirstMax = NULL;

	pSchemaCommon = (SchemaCommon *) data;

	if(strcmp(el, "complexType") == 0) {
		if(pSchemaCommon->serviceFlag_ == 0) {
			pSchemaCommon->recordFlag_ = 1;
		}

		while(pSchemaCommon->lstSchemaElementReference_ != NULL) {
			appendNode (&(pSchemaCommon->pSchemaElement_->pListReference_),
								 pSchemaCommon->lstSchemaElementReference_->node);
			pSchemaCommon->lstSchemaElementReference_ = pSchemaCommon->lstSchemaElementReference_->pNext;
		}

		while(pSchemaCommon->lstSchemaElementMinOccurs_ != NULL) {
			appendNode (&(pSchemaCommon->pSchemaElement_->pListMinOccurs_),
								 pSchemaCommon->lstSchemaElementMinOccurs_->node);
			pSchemaCommon->lstSchemaElementMinOccurs_ = pSchemaCommon->lstSchemaElementMinOccurs_->pNext;
		}

		while(pSchemaCommon->lstSchemaElementMaxOccurs_ != NULL) {
			appendNode (&(pSchemaCommon->pSchemaElement_->pListMaxOccurs_),
								 pSchemaCommon->lstSchemaElementMaxOccurs_->node);
			pSchemaCommon->lstSchemaElementMaxOccurs_ = pSchemaCommon->lstSchemaElementMaxOccurs_->pNext;
		}
	}


	if ((strcmp(el, "element") == 0) || (strcmp(el, "complexType") == 0)) {
		if (pSchemaCommon->recordFlag_ == 1) {
			if ((strcmp(pSchemaCommon->pSchemaElement_->attributeName_, "IPDR") != 0) &&
			(strcmp(pSchemaCommon->pSchemaElement_->attributeName_, "IPDRDoc") != 0) &&
			(strcmp(pSchemaCommon->pSchemaElement_->attributeName_, "IPDRDoc.End") != 0) &&
			(strcmp(pSchemaCommon->pSchemaElement_->attributeName_, "IPDRType") != 0))
			{
				appendListSchemaElement( &pSchemaCommon->pSchemaLookup_->pListSchemaElement_,
												 pSchemaCommon->pSchemaElement_);
			}

			if (pSchemaCommon->pSchemaElement_->documentation_ != NULL) {
				free(pSchemaCommon->pSchemaElement_->documentation_);
				pSchemaCommon->pSchemaElement_->documentation_ = NULL;
			}

			if (pSchemaCommon->pSchemaElement_->attributeName_ != NULL) {
				free(pSchemaCommon->pSchemaElement_->attributeName_);
				pSchemaCommon->pSchemaElement_->attributeName_ = NULL;
			}

			if (pSchemaCommon->pSchemaElement_->attributeType_ != NULL) {
				free(pSchemaCommon->pSchemaElement_->attributeType_);
				pSchemaCommon->pSchemaElement_->attributeType_ = NULL;
			}

			if (pSchemaCommon->pSchemaElement_->complexAttributeType_ != NULL) {
				free(pSchemaCommon->pSchemaElement_->complexAttributeType_);
				pSchemaCommon->pSchemaElement_->complexAttributeType_ = NULL;
			}

			if (pSchemaCommon->pSchemaElement_->derivedType_ != NULL) {
				free(pSchemaCommon->pSchemaElement_->derivedType_);
				pSchemaCommon->pSchemaElement_->derivedType_ = NULL;
			}

			if (pSchemaCommon->pSchemaElement_->reference_ != NULL) {
				free(pSchemaCommon->pSchemaElement_->reference_);
				pSchemaCommon->pSchemaElement_->reference_ = NULL;
			}

			if (pSchemaCommon->pSchemaElement_->unit_ != NULL) {
				free(pSchemaCommon->pSchemaElement_->unit_);
				pSchemaCommon->pSchemaElement_->unit_ = NULL;
			}

			if (pSchemaCommon->pSchemaElement_->pListAttributeEnum_ != NULL) {
				printList(pSchemaCommon->pSchemaElement_->pListAttributeEnum_);
				freeList(&(pSchemaCommon->pSchemaElement_->pListAttributeEnum_));
				pSchemaCommon->pSchemaElement_->pListAttributeEnum_ = NULL;
			}

			if (pSchemaCommon->pSchemaElement_->pListReference_ != NULL) {
				printList(pSchemaCommon->pSchemaElement_->pListReference_);
				freeList(&(pSchemaCommon->pSchemaElement_->pListReference_));
				pSchemaCommon->pSchemaElement_->pListReference_ = NULL;
			}

			if (pSchemaCommon->pSchemaElement_->pListMinOccurs_ != NULL) {
				printList(pSchemaCommon->pSchemaElement_->pListMinOccurs_);
				freeList(&(pSchemaCommon->pSchemaElement_->pListMinOccurs_));
				pSchemaCommon->pSchemaElement_->pListMinOccurs_ = NULL;
			}

			if (pSchemaCommon->pSchemaElement_->pListMaxOccurs_ != NULL) {
				printList(pSchemaCommon->pSchemaElement_->pListMaxOccurs_);
				freeList(&(pSchemaCommon->pSchemaElement_->pListMaxOccurs_));
				pSchemaCommon->pSchemaElement_->pListMaxOccurs_ = NULL;
			}

			pSchemaCommon->recordFlag_ = 0;
		}
	}


	if (strcmp(el, "schema") == 0) {
		if(pSchemaCommon->pSchemaElement_ != NULL) {
			if (pSchemaCommon->pSchemaElement_->documentation_ != NULL) {
				free(pSchemaCommon->pSchemaElement_->documentation_);
				pSchemaCommon->pSchemaElement_->documentation_ = NULL;
			}
		}

		if (pSchemaCommon->pSchemaElement_ != NULL) {
				free(pSchemaCommon->pSchemaElement_);
				pSchemaCommon->pSchemaElement_ = NULL;
		}

		/*
		 * Traversing ListSchemaElement
		 * Looking for Attribute Name
		 * Comparing it with entire Attribute Reference
		 * Also keep incrementing Attribue isOptional list
		 * if attribute name matches, break
		 * if isOptional field is , populate O 
		 */     

		pCurrent = pSchemaCommon->pSchemaLookup_->pListSchemaElement_;

		pCurrentRef = pSchemaCommon->lstReference_;
		pCurrentOpt = pSchemaCommon->lstMinOccurs_;
		pCurrentMax = pSchemaCommon->lstMaxOccurs_;

		pFirstRef = pCurrentRef;
		pFirstOpt = pCurrentOpt;
		pFirstMax = pCurrentMax;

		while ((pCurrent != NULL) && (pCurrentRef != NULL)) {
			pCurrentRef = pFirstRef;
			pCurrentOpt = pFirstOpt;
			pCurrentMax = pFirstMax;

			while ((strcmp(pCurrent->pSchemaElement_->attributeName_, pCurrentRef->node) != 0) && (pCurrentRef != NULL) && (pCurrentRef->pNext != NULL)) {
				pCurrentRef = pCurrentRef->pNext;
				pCurrentOpt = pCurrentOpt->pNext;
				pCurrentMax = pCurrentMax->pNext;
			}

			pCurrent->pSchemaElement_->pAttributeOptional_ = 0;

			if ((strcmp(pCurrent->pSchemaElement_->attributeName_, pCurrentRef->node) == 0) && (strcmp(pCurrentOpt->node, "Optional") == 0)) {
				pCurrent->pSchemaElement_->pAttributeOptional_ = 1;
			}

			if ((strcmp(pCurrent->pSchemaElement_->attributeName_, pCurrentRef->node) == 0)) {
				if(strcmp(pCurrentMax->node,"0") == 0) {
					pCurrent->pSchemaElement_->noOfElements_ = 0;
				}
				else if(strcmp(pCurrentMax->node,"unbounded") == 0) {
					pCurrent->pSchemaElement_->noOfElements_ = -1;
				}
				else {
					pCurrent->pSchemaElement_->noOfElements_ = atoi(pCurrentMax->node);
				}
			}
			pCurrent = pCurrent->pNext_;	
			count++;
		}
	}
}  /* End of end handler */

static void 
attributeValueHandler(
		void *data, 
		const char *attributeValue, 
		int len)
{
	char* tmpDoc = NULL;
	char* tmpDoc2 = NULL;

	char* tmpXMLChar = NULL;
	char* tmpXMLString = NULL;
	int lenBuffer = 0;
	int count = 0;

	SchemaCommon *pSchemaCommon = NULL;
	pSchemaCommon = (SchemaCommon *) data;

	lenBuffer = len;
	if ((len > 1) && (pSchemaCommon->pSchemaElement_ != NULL) && (pSchemaCommon->docFlag_ == 1))  {
		tmpDoc = (char *) calloc (len + 10, sizeof(char));
		tmpDoc2 = (char *) calloc (len + 10, sizeof(char));
		tmpXMLChar = (char *) calloc (10, sizeof(char));

		if (attributeValue[len] == 38) {
			pSchemaCommon->docFlag_ = 1;
	
			for (count = 0;;count++) {
				if (attributeValue[len] == 59) break;
				tmpXMLChar[count] = attributeValue[len];
				len++;
			}
			tmpXMLChar[count] = 59;
			tmpXMLChar[count + 1] = 0;
		
			len++;

			len = lenBuffer;
			pSchemaCommon->oldDoc_ = (char *) calloc (400, sizeof(char));
			strncpy(tmpDoc, attributeValue, len);

			tmpDoc[len] = '\0';
			getStringNoTabs(pSchemaCommon->oldDoc_, tmpDoc);

			if (getXMLCharSL(tmpXMLChar) != 0) {
				tmpXMLString = (char *) calloc (2, sizeof(char));
				tmpXMLString[0] = getXMLCharSL(tmpXMLChar);
				tmpXMLString[1] = 0;
				strcat(pSchemaCommon->oldDoc_, tmpXMLString);
				free(tmpXMLString);
			}
	

		} else {
			pSchemaCommon->docFlag_ = 0;
			strncpy(tmpDoc, attributeValue, len);
			tmpDoc[len] = '\0';

			getStringNoTabs(tmpDoc2, tmpDoc);
		

			if (pSchemaCommon->oldDoc_ == NULL) {
				if (pSchemaCommon->pSchemaElement_->documentation_ != NULL) {
							
					free(pSchemaCommon->pSchemaElement_->documentation_);
					pSchemaCommon->pSchemaElement_->documentation_ = NULL;
				}
				
				pSchemaCommon->pSchemaElement_->documentation_ = (char *) calloc (400, sizeof(char));
				strcpy(pSchemaCommon->pSchemaElement_->documentation_, tmpDoc2);
			}
			else {
				if (pSchemaCommon->pSchemaElement_->documentation_ != NULL) {
							
					free(pSchemaCommon->pSchemaElement_->documentation_);
					pSchemaCommon->pSchemaElement_->documentation_ = NULL;
				}
				pSchemaCommon->pSchemaElement_->documentation_ = (char *) calloc (400, sizeof(char));
				strcat(pSchemaCommon->pSchemaElement_->documentation_, pSchemaCommon->oldDoc_);
				strcat(pSchemaCommon->pSchemaElement_->documentation_, tmpDoc2);

				free(pSchemaCommon->oldDoc_);
				pSchemaCommon->oldDoc_ = NULL;
			}

		}
		free(tmpDoc);
      tmpDoc = NULL;
		free(tmpDoc2);
      tmpDoc2 = NULL;
		free(tmpXMLChar);
      tmpXMLChar = NULL;
		} 

	/* Reading units */
	if ((len > 1) && (pSchemaCommon->pSchemaElement_ != NULL) && (pSchemaCommon->unitFlag_ == 1))  {
		pSchemaCommon->pSchemaElement_->unit_ = (char *) calloc (len + 1, sizeof(char));
		strncpy(pSchemaCommon->pSchemaElement_->unit_, attributeValue, len);
		pSchemaCommon->pSchemaElement_->unit_[len] = '\0';
		pSchemaCommon->unitFlag_ = 0;
	}

	/* Reading status */
	if ((len > 1) && (pSchemaCommon->pSchemaElement_ != NULL) && (pSchemaCommon->statusFlag_ == 1))  {
		pSchemaCommon->pSchemaElement_->status_ = (char *) calloc (len + 1, sizeof(char));
		strncpy(pSchemaCommon->pSchemaElement_->status_, attributeValue, len);
		pSchemaCommon->pSchemaElement_->status_[len] = '\0';
		pSchemaCommon->statusFlag_ = 0;
	}

	/* Reading reference */
	if ((len > 1) && (pSchemaCommon->pSchemaElement_ != NULL) && (pSchemaCommon->referenceFlag_ == 1))  {
		pSchemaCommon->pSchemaElement_->reference_ = (char *) calloc (len + 1, sizeof(char));
		strncpy(pSchemaCommon->pSchemaElement_->reference_, attributeValue, len);
		pSchemaCommon->pSchemaElement_->reference_[len] = '\0';
		pSchemaCommon->referenceFlag_ = 0;
	}

	return ;
	}


int parseSchemaLookup(char* SchemaURI, 
				SchemaCommon* pSchemaCommon,
				   IPDRCommonParameters *pIPDRCommonParameters,
				int *pErrorCode)
 
{
	FILE *fp;
	char *Buff = NULL;
	XML_Parser xmlParser;
	void* userData = NULL;
	int count = 0;
	int retValue = 0;
	char* configParams = NULL;
	char* SchemaFile = NULL;
	unsigned int lengthSchemaFile = 0;
   static int cnt = 0;


	configParams = (char *) calloc (MAX_IPDR_STRING, sizeof(char));
	SchemaFile = (char *) calloc (MAX_IPDR_STRING, sizeof(char));

	
	retValue = getFileNameSL(SchemaURI, SchemaFile);
	if (retValue == IPDR_ERROR) {
		return (IPDR_ERROR);
	}

	if (strcmp (SchemaURI, SchemaFile) != 0) {
		retValue = getFile(SchemaURI, 
						   SchemaFile, 
						   configParams,
						   pErrorCode);
		if (retValue == IPDR_ERROR) {
			free(configParams);
			free(SchemaFile);
			return (IPDR_ERROR);
		}
	}


	if (fopen(SchemaFile, "r+" ) == NULL) {
		*pErrorCode = IPDR_DESCRIPTOR_SCHEMA_FILE_INVALID;		
		free(configParams);
		free(SchemaFile);
		return (IPDR_ERROR);
	}

	if (SchemaFile == NULL) {
		*pErrorCode = IPDR_DESCRIPTOR_SCHEMA_FILE_INVALID;
		free(configParams);
		free(SchemaFile);
		return (IPDR_WARNING);
	}

	if (SchemaFile[0] == 0) {
		*pErrorCode = IPDR_DESCRIPTOR_SCHEMA_FILE_INVALID;
		free(configParams);
		free(SchemaFile);
		return (IPDR_WARNING);
	}

	
	lengthSchemaFile = getFileLength(SchemaFile) + 1;
	Buff = (char *) calloc (lengthSchemaFile, sizeof(char));
		
	if (Buff == NULL) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		free(configParams);
		free(SchemaFile);		
		return (IPDR_ERROR);
	}

	/* Creating the XML Parser instance */
	xmlParser = XML_ParserCreate(NULL);
	if (!xmlParser) {
		*pErrorCode = IPDR_LOOKUP_PARSER_MEMORY_ALLOC_FAILED;
		free(configParams);
		free(SchemaFile);
		free(Buff);
		return (IPDR_ERROR);
	}

	XML_SetElementHandler(xmlParser, start, end);
	XML_SetCharacterDataHandler(xmlParser, attributeValueHandler);

	/* Opening the file passed as the input parameter */
	if ((fp = fopen(SchemaFile, "rw+")) == NULL) {
		*pErrorCode = IPDR_LOOKUP_FILE_NOT_FOUND;
		free(configParams);
		free(SchemaFile);
		free(Buff);
		return (IPDR_ERROR);
	} 

	userData = (SchemaCommon *) pSchemaCommon;
	XML_SetUserData(xmlParser, userData);

	/* Parsing the Schema file */
	for (;;) {
		int done;
		int xmlFileLength;

		xmlFileLength = fread(Buff, 1, lengthSchemaFile, fp);
		if (ferror(fp)) {
			*pErrorCode = IPDR_LOOKUP_READ_ERROR;
			free(configParams);
			free(SchemaFile);
			free(Buff);
			return (IPDR_ERROR);
		}

		done = feof(fp);

		if (! XML_Parse(xmlParser, Buff, xmlFileLength, done)) {
			/* 
			*pErrorCode = IPDR_LOOKUP_PARSER_READ_ERROR;
			*/

			*pErrorCode = XML_GetErrorCode(xmlParser) + EXPAT_ERROR_CODE;
			free(configParams);
			free(SchemaFile);
			free(Buff);
			return (IPDR_ERROR);
		}

		if (done)
			break;
	}
		/*IPDR v3.5, version changes*/
        if(cnt == 0) {
            if(strcmp(gSchemaVersion,IPDR_VERSION_3_5) == 0){
               if (pIPDRCommonParameters->schemaVersion[0] == '\0') {
                  /* Set the structure member */
                  strcpy(pIPDRCommonParameters->schemaVersion, IPDR_VERSION_3_5);
               }
            }
            else if(strcmp(gSchemaVersion,IPDR_VERSION_3_1) == 0){
               if (pIPDRCommonParameters->schemaVersion[0] == '\0') {
                  strcpy(pIPDRCommonParameters->schemaVersion, IPDR_VERSION_3_1);
               }
            }
            cnt = cnt + 1;
        }


	/* Returning back the FNFType structure to calling function */
	userData = (SchemaCommon *) XML_GetUserData(xmlParser);
	pSchemaCommon = (SchemaCommon *) userData;

	free(Buff);
	free(configParams);
	free(SchemaFile);
	return (IPDR_OK);
}


int	appendListListSchemaElement(
				ListSchemaElement** pHeadRef,
				ListSchemaElement** pListSchemaElement)
{
	ListSchemaElement* pOld = NULL;
	ListSchemaElement* pCurrent = NULL;
	pOld = *pHeadRef;

	*pHeadRef = *pListSchemaElement;

	pCurrent = *pHeadRef;

	while (pCurrent->pNext_ != NULL)
		pCurrent = pCurrent->pNext_;


	pCurrent->pNext_ = pOld;
	return (IPDR_OK);
}

int generateSchemaLookup(
		char* SchemaURI,
		SchemaLookup* pSchemaLookup,
		ListNameSpaceInfo** pListNameSpaceInfo,
		IPDRCommonParameters *pIPDRCommonParameters,
		int* pErrorCode)
{
	SchemaCommon* pSchemaCommon = NULL;
	SchemaElement* pSchemaElement = NULL;
	ListSchemaElement* pCurrentListSchemaElement = NULL;
	ListSchemaElement* pListSchemaElement = NULL;
	ListSchemaElement* pListServiceElement = NULL;
	ListSchemaElement* pOldListSchemaListElement = NULL;
	ListSchemaElement* pCurrent = NULL;
	List* pCurrRef = NULL;

	int errorSeverity = 0;
	int flag = 0;
	int myFlag = 0;
	int count = 0;
	int lengthList = 0;
	int loopFlag = 0;
	int lenServType = 0;
	char* tempAttributeName = NULL;
	char* tempAttributeType = NULL;
	char* tempAttribName = NULL;
	char* tempSchemaURI = NULL;
	char* tempFileName = NULL;
   static int lFound = 0;
   static int lFnd = 0;
	
	tempSchemaURI = (char *) calloc (MAX_NAMESPACE_URI, sizeof(char));
	strcpy(tempSchemaURI, SchemaURI);

	do {
		pSchemaCommon = newSchemaCommon();

		tempFileName = (char *) calloc (MAX_IPDR_STRING, sizeof(char));
		getFileNameSL(tempSchemaURI, tempFileName);

		if(strcmp(tempSchemaURI, "IPDRTypes.xsd") == 0) {
			loopFlag++;
			continue;
		}

		errorSeverity = parseSchemaLookup(
								tempSchemaURI,
								pSchemaCommon,
								pIPDRCommonParameters,							
								pErrorCode);

		if (errorSeverity == IPDR_ERROR) {
			free(tempSchemaURI);
			return (IPDR_ERROR);
		}

		appendListNameSpaceInfo(
								pListNameSpaceInfo,
								pSchemaCommon->nameSpaceId_,
								pSchemaCommon->targetNameSpace_);

		if (pSchemaLookup->serviceType_ == NULL) {
			lenServType = strlen(pSchemaCommon->pSchemaLookup_->serviceType_) + 1;
			pSchemaLookup->serviceType_ = (char *) calloc (lenServType, sizeof(char));
			strcpy(pSchemaLookup->serviceType_, pSchemaCommon->pSchemaLookup_->serviceType_);
		} else {
			free(pSchemaCommon->pSchemaLookup_->serviceType_);
			lenServType = 0;
			lenServType = strlen(pSchemaLookup->serviceType_) + 1;
			pSchemaCommon->pSchemaLookup_->serviceType_ = (char *) calloc (lenServType, sizeof(char));
			strcpy(pSchemaCommon->pSchemaLookup_->serviceType_, pSchemaLookup->serviceType_);
		}

		lengthList = getLengthListSchemaElement
			(pSchemaCommon->pSchemaLookup_->pListSchemaElement_);

		pCurrentListSchemaElement = pSchemaCommon->pSchemaLookup_->pListSchemaElement_;

		for (count = 0; count < lengthList; count++) {
			tempAttributeName = (char *) calloc (2 * MAX_ATTRIBUTE_NAME, sizeof(char));

			strcpy(tempAttributeName, pSchemaCommon->pSchemaLookup_->serviceType_);
			strcat(tempAttributeName, "$");
			strcat(tempAttributeName, pSchemaCommon->targetNameSpace_);
			strcat(tempAttributeName, "$");
			strcat(tempAttributeName, pSchemaCommon->pSchemaLookup_->pListSchemaElement_->pSchemaElement_->attributeName_);

			pSchemaElement = newSchemaElement();

			pSchemaElement->attributeName_ = (char *) calloc (MAX_IPDR_STRING, sizeof(char));
			strcpy(pSchemaElement->attributeName_, tempAttributeName);


			tempAttributeType = (char *) calloc (MAX_IPDR_STRING, sizeof(char));
			strcpy(tempAttributeType, pSchemaCommon->pSchemaLookup_->pListSchemaElement_->pSchemaElement_->attributeType_);
			
			pSchemaElement->attributeType_ = (char *) calloc (MAX_IPDR_STRING, sizeof(char));
			getIPDRSubstring(tempAttributeType, pSchemaElement->attributeType_,
':');


			free(tempAttributeType);
         tempAttributeType = NULL;
	
			pSchemaElement->derivedType_ = (char *) calloc (MAX_IPDR_STRING, sizeof(char));
			pSchemaElement->complexAttributeType_ = (char *) calloc (MAX_IPDR_STRING, sizeof(char));
		
			/*  new start code */
			
			strcpy(pSchemaElement->derivedType_, pSchemaElement->attributeType_);
			strcpy(pSchemaElement->complexAttributeType_, pSchemaElement->attributeType_);


			if (isPrimitiveSL(pSchemaElement->attributeType_) == IPDR_FALSE) {
				getPrimitiveTypeSL(pSchemaElement->attributeType_, pSchemaElement->derivedType_);
			}
 
			
			populateAttributeTypeSL(pSchemaElement->attributeType_);
			
			populateAttributeTypeSL(pSchemaElement->derivedType_);
			/* end code */
				

			pSchemaElement->documentation_ = (char *) calloc (MAX_IPDR_STRING, sizeof(char));
			if (pSchemaCommon->pSchemaLookup_->pListSchemaElement_->pSchemaElement_->documentation_ != NULL)
			strcpy(pSchemaElement->documentation_, pSchemaCommon->pSchemaLookup_->pListSchemaElement_->pSchemaElement_->documentation_);
			else
			strcpy(pSchemaElement->documentation_, IPDR_NULL);


			pSchemaElement->pAttributeOptional_ = pSchemaCommon->pSchemaLookup_->pListSchemaElement_->pSchemaElement_->pAttributeOptional_;
			
		   pSchemaElement->pListAttributeEnum_ = pSchemaCommon->pSchemaLookup_->pListSchemaElement_->pSchemaElement_->pListAttributeEnum_;
		
			pSchemaElement->pListReference_ = pSchemaCommon->pSchemaLookup_->pListSchemaElement_->pSchemaElement_->pListReference_;
			
			pSchemaElement->pListMinOccurs_ = pSchemaCommon->pSchemaLookup_->pListSchemaElement_->pSchemaElement_->pListMinOccurs_;

			pSchemaElement->pListMaxOccurs_ = pSchemaCommon->pSchemaLookup_->pListSchemaElement_->pSchemaElement_->pListMaxOccurs_;

			pSchemaElement->noOfElements_ = pSchemaCommon->pSchemaLookup_->pListSchemaElement_->pSchemaElement_->noOfElements_;

			pSchemaElement->reference_ = (char *) calloc (MAX_IPDR_STRING, sizeof(char));
			if (pSchemaCommon->pSchemaLookup_->pListSchemaElement_->pSchemaElement_->reference_ != NULL)
			strcpy(pSchemaElement->reference_, pSchemaCommon->pSchemaLookup_->pListSchemaElement_->pSchemaElement_->reference_);
			else
			strcpy(pSchemaElement->reference_, IPDR_NULL);


			pSchemaElement->status_ = (char *) calloc (MAX_IPDR_STRING, sizeof(char));
			if (pSchemaCommon->pSchemaLookup_->pListSchemaElement_->pSchemaElement_->status_ != NULL)
			strcpy(pSchemaElement->status_, pSchemaCommon->pSchemaLookup_->pListSchemaElement_->pSchemaElement_->status_);
			else
			strcpy(pSchemaElement->status_, IPDR_NULL);
			

			pSchemaElement->unit_ = (char *) calloc (MAX_IPDR_STRING, sizeof(char));
			if (pSchemaCommon->pSchemaLookup_->pListSchemaElement_->pSchemaElement_->unit_ != NULL) 
			strcpy(pSchemaElement->unit_, pSchemaCommon->pSchemaLookup_->pListSchemaElement_->pSchemaElement_->unit_);
			else
			strcpy(pSchemaElement->unit_, IPDR_NULL);


         pCurrRef = pSchemaCommon->lstReference_; 
			while ((pCurrRef != NULL)) {
				tempAttribName = (char *) calloc (2 * MAX_ATTRIBUTE_NAME, sizeof(char));
				strcpy(tempAttribName, pSchemaCommon->pSchemaLookup_->serviceType_);
				strcat(tempAttribName, "$");
				strcat(tempAttribName, pSchemaCommon->targetNameSpace_);
				strcat(tempAttribName, "$");
				strcat(tempAttribName, pCurrRef->node);
         	if((strcmp(pSchemaElement->attributeName_, tempAttribName) == 0)) {
           		appendListSchemaElement( &pListServiceElement, pSchemaElement);
				}
            lFound = 1;
				pCurrRef = pCurrRef->pNext;
				free(tempAttribName);
            tempAttribName = NULL;
         }
         
         appendListSchemaElement( &pListSchemaElement, pSchemaElement);


			pSchemaCommon->pSchemaLookup_->pListSchemaElement_ = pSchemaCommon->pSchemaLookup_->pListSchemaElement_->pNext_;

			free(tempAttributeName);
         tempAttributeName = NULL;
			freeSchemaElement(pSchemaElement);
         pSchemaElement = NULL;
		}

		if (pSchemaLookup->pListSchemaElement_ == NULL) {
			pSchemaLookup->pListSchemaElement_ = pListSchemaElement;
		} else {
			appendListListSchemaElement(
				&pSchemaLookup->pListSchemaElement_,
				&pListSchemaElement);
		}

		if(pListServiceElement != NULL) {
			if (pSchemaLookup->pListServiceElement_ == NULL) {
				pSchemaLookup->pListServiceElement_ = pListServiceElement;
			} else {
				appendListListSchemaElement(
					&pSchemaLookup->pListServiceElement_,
					&pListServiceElement);
			}
		}

		pListSchemaElement = NULL;
		pListServiceElement = NULL;

		if (pSchemaCommon->extensionBase_ == NULL) {
			if (loopFlag == 0) loopFlag = 2;
			else loopFlag++;
			if (pSchemaCommon->schemaLocation_ != NULL) {
				free(pSchemaCommon->schemaLocation_);
				pSchemaCommon->schemaLocation_ = NULL;
			}
		} else {
		if (strcmp(pSchemaCommon->extensionBase_, LOOKUP_FINAL_SCHEMA) != 0) {
			strcpy(tempSchemaURI, pSchemaCommon->schemaLocation_);
		} else {
			loopFlag++;
			strcpy(tempSchemaURI, pSchemaCommon->schemaLocation_);
			}	
		}

		free(tempFileName);

		pSchemaCommon->pSchemaLookup_->pListSchemaElement_ = pCurrentListSchemaElement;;
	} 
	while (loopFlag != 2);

	
	free(tempSchemaURI);
	return (IPDR_OK);
}


 
int isPrimitiveSL(char *tempNewAttrType)
{
   if ((strcmp(tempNewAttrType, SCHEMA_LOOKUP_LONG) == 0) ||
		(strcmp(tempNewAttrType, SCHEMA_LOOKUP_LONG_OTHER) == 0) ||
        (strcmp(tempNewAttrType, SCHEMA_LOOKUP_U_LONG) == 0) ||
        (strcmp(tempNewAttrType, SCHEMA_LOOKUP_HYPER) == 0) ||
        (strcmp(tempNewAttrType, SCHEMA_LOOKUP_U_HYPER) == 0) ||
		(strcmp(tempNewAttrType, SCHEMA_LOOKUP_FLOAT) == 0) ||
		(strcmp(tempNewAttrType, SCHEMA_LOOKUP_DOUBLE) == 0) ||
		(strcmp(tempNewAttrType, SCHEMA_LOOKUP_STRING) == 0) ||
		(strcmp(tempNewAttrType, SCHEMA_LOOKUP_BOOLEAN) == 0) ||
		(strcmp(tempNewAttrType, SCHEMA_LOOKUP_SHORT) == 0) ||
		(strcmp(tempNewAttrType, SCHEMA_LOOKUP_U_SHORT) == 0) ||
		(strcmp(tempNewAttrType, SCHEMA_LOOKUP_BYTE_ARRAY) == 0) || 
		(strcmp(tempNewAttrType, SCHEMA_LOOKUP_HEX_BIN_ARRAY) == 0) || 
		(strcmp(tempNewAttrType, SCHEMA_LOOKUP_BYTE) == 0) || 
		(strcmp(tempNewAttrType, SCHEMA_LOOKUP_U_BYTE) == 0) || 
		(strcmp(tempNewAttrType, SCHEMA_LOOKUP_ARRAY) == 0) ||
		(strcmp(tempNewAttrType, SCHEMA_LOOKUP_STRUCTURE) == 0))
		return (IPDR_TRUE);
   else
    return (IPDR_FALSE);
}
 
 
int getPrimitiveTypeSL(char* attributeType, 
      char* attributeDerivedType)
{
 int flag = IPDR_FALSE;
 
 if (strcmp(attributeDerivedType, 
  SCHEMA_LOOKUP_DERIVED_DATETIME) == 0) {
  strcpy(attributeType, SCHEMA_LOOKUP_LONG);
  flag = IPDR_TRUE;
 }
 
 if (strcmp(attributeDerivedType, 
  SCHEMA_LOOKUP_DERIVED_DATETIME_MILISEC) == 0) {
  strcpy(attributeType, SCHEMA_LOOKUP_HYPER);
  flag = IPDR_TRUE;
 }
 
 if (strcmp(attributeDerivedType, 
  SCHEMA_LOOKUP_DERIVED_IPV4ADDR) == 0) {
  strcpy(attributeType, SCHEMA_LOOKUP_LONG); 
  flag = IPDR_TRUE;
 }
 
 if (strcmp(attributeDerivedType, 
  SCHEMA_LOOKUP_DERIVED_IPV6ADDR) == 0) {
  strcpy(attributeType, SCHEMA_LOOKUP_BYTE_ARRAY);
  flag = IPDR_TRUE;
 }
 
 if (strcmp(attributeDerivedType, 
  SCHEMA_LOOKUP_DERIVED_UUID) == 0) {
  strcpy(attributeType, SCHEMA_LOOKUP_BYTE_ARRAY);
  flag = IPDR_TRUE;
 }

 if (strcmp(attributeDerivedType, 
  SCHEMA_LOOKUP_DERIVED_STRUCTURE) == 0) {
  strcpy(attributeType, SCHEMA_LOOKUP_STRUCTURE);
  flag = IPDR_TRUE;
 }
 
 if (strcmp(attributeDerivedType, 
  SCHEMA_LOOKUP_DERIVED_IPADDR) == 0) {
  strcpy(attributeType, SCHEMA_LOOKUP_BYTE_ARRAY);
  flag = IPDR_TRUE;
 }
 
 if (strcmp(attributeDerivedType, 
  SCHEMA_LOOKUP_DERIVED_DATETIME_MICROSEC) == 0) {
  strcpy(attributeType, SCHEMA_LOOKUP_HYPER);
  flag = IPDR_TRUE;
 }

 if (flag == IPDR_FALSE){
  strcpy(attributeType, SCHEMA_LOOKUP_STRING);
  strcpy(attributeDerivedType, SCHEMA_LOOKUP_STRING);
} 
 return (IPDR_OK);
}
 

int populateAttributeTypeSL(char* attributeType)
{

 if ((strcmp(attributeType, SCHEMA_LOOKUP_LONG) == 0) ||
  (strcmp(attributeType, SCHEMA_LOOKUP_LONG_OTHER) == 0)) {
  strcpy(attributeType, SCHEMA_LOOKUP_WRITE_LONG);
 }
 
 else if ((strcmp(attributeType, SCHEMA_LOOKUP_U_LONG) == 0)) { 
  strcpy(attributeType, SCHEMA_LOOKUP_WRITE_U_LONG);
 }
 
 else if (strcmp(attributeType, SCHEMA_LOOKUP_HYPER) == 0) {
  strcpy(attributeType, SCHEMA_LOOKUP_WRITE_HYPER);
 }
 
 else if (strcmp(attributeType, SCHEMA_LOOKUP_U_HYPER) == 0) {
  strcpy(attributeType, SCHEMA_LOOKUP_WRITE_U_HYPER);
 }
 
 else if (strcmp(attributeType, SCHEMA_LOOKUP_FLOAT) == 0) {
  strcpy(attributeType, SCHEMA_LOOKUP_WRITE_FLOAT);
 }
 
 else if (strcmp(attributeType, SCHEMA_LOOKUP_DOUBLE) == 0) {
  strcpy(attributeType, SCHEMA_LOOKUP_WRITE_DOUBLE);
 }
 
 else if (strcmp(attributeType, SCHEMA_LOOKUP_STRING) == 0) {
  strcpy(attributeType, SCHEMA_LOOKUP_WRITE_STRING);
 }
 
 else if (strcmp(attributeType, SCHEMA_LOOKUP_BOOLEAN) == 0) {
  strcpy(attributeType, SCHEMA_LOOKUP_WRITE_BOOLEAN);
 }

 else if (strcmp(attributeType, SCHEMA_LOOKUP_SHORT) == 0) {
  strcpy(attributeType, SCHEMA_LOOKUP_WRITE_SHORT);
 }

 else if (strcmp(attributeType, SCHEMA_LOOKUP_U_SHORT) == 0) {
  strcpy(attributeType, SCHEMA_LOOKUP_WRITE_U_SHORT);
 }

 else if (strcmp(attributeType, SCHEMA_LOOKUP_BYTE_ARRAY) == 0) {
  strcpy(attributeType, SCHEMA_LOOKUP_WRITE_BYTE_ARRAY);
 }
 
 else if (strcmp(attributeType, SCHEMA_LOOKUP_HEX_BIN_ARRAY) == 0) {
  strcpy(attributeType, SCHEMA_LOOKUP_WRITE_BYTE_ARRAY);
 }

 else if (strcmp(attributeType, SCHEMA_LOOKUP_STRUCTURE) == 0) {
  strcpy(attributeType, SCHEMA_LOOKUP_WRITE_STRUCTURE);
 }

 else if (strcmp(attributeType, SCHEMA_LOOKUP_BYTE) == 0) {
  strcpy(attributeType, SCHEMA_LOOKUP_WRITE_BYTE);
 }
 
 else if (strcmp(attributeType, SCHEMA_LOOKUP_U_BYTE) == 0) {
  strcpy(attributeType, SCHEMA_LOOKUP_WRITE_U_BYTE);
 }
 
 return (IPDR_OK);
}
 


int getFileNameSL(char* URI, char* name)
{
	char* token = NULL;
	char seps[] = "/";
	char* tempStr = NULL;

	tempStr = (char *) calloc (100, sizeof(char));

	strcpy(tempStr, URI);

	token = strtok(tempStr, seps);
	while (token != NULL) {
		strcpy(name, token);
		token = strtok(NULL, seps);
	}

	free(token);
	token = NULL;
	free(tempStr);
	return (IPDR_OK);
}


int getXMLCharSL(char* tmpXMLChar)
{
	if (tmpXMLChar == NULL)
		return (0);	/* ASCII value of ' ' */	
	else if (strcmp(tmpXMLChar, "&amp;") == 0)
		return (38);	/* ASCII value of '&' */
	else if (strcmp(tmpXMLChar, "&lt;") == 0)
		return (60);	/* ASCII value of '<' */
	else
		return (0);	/* ASCII value of ' ' */
	
}

/* IPDR v3.5, change for version */
int setSchemaVersion(float ver)
{

   printf("The ver in setSchemaVersion ChangeVersion is:%f\n", ver);

   sprintf(gSchemaVersion,"%1.1f",ver);

   printf("The setSchemaVersion global version is:%s\n", gSchemaVersion);

   return (IPDR_OK);
}
