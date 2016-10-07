/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/

/******************************************************
 * File                 : descriptor.c                *
 * Description          : This file contains the XML  * 
 *                        parsing functions needed for* 
 *                        generation of descriptor.   *
 * Author               : Infosys Tech Ltd            *
 * Modification History :                             *
 *--------------------------------------------------- *
 * Date       Name        Change/Description          *
 *--------------------------------------------------- *
 * 26Jan02    Infosys       Draft Version             *
 ******************************************************/
 
#include <stdio.h>
#include "expat.h"
#include "common/descriptor.h"
#include "utils/IPDRCommon.h"
#include "utils/IPDRMemory.h"
#include "utils/utils.h"
#include "curl/curl.h"
#include "curl/types.h"
#include "curl/easy.h"
#include "common/getFile.h"
 
#include "common/IPDRDocWriter.h"

static char gVersion[10];

/*!
 *  NAME:
 *      start() - This function is called as any start tag  
 *                           for an element is encountered..
 *
 *  DESCRIPTION:
 *      1. This function gets the Attribute Name, Attribute 
 *         Type & Attribute description (Optional or Mandatory) 
 *         & put these values in dynamic
 *         linked list.
 * 
 *  ARGUMENTS:
 *       - data {IN}
 *       -  name {IN}
 *       -  atts {IN} 
 *
 *  RETURNS  
 *      Returns void
 *    
 */
 
static void start(void* data, 
      const char *name, 
      const char **atts) 
{
 int count = 0;
 int rec = 0;
 int nextCount = 0;
 int nodeCount = 0;
 int tempLen = 0;
 static int cnt = 0;
 unsigned int lenExtensionBase = 0;
 unsigned int lenNameSpaceId = 0;
 char* attributeName = NULL;
 char* tmp = NULL;
 
 DescriptorCommon *pDescriptorCommon = NULL;
 
 pDescriptorCommon = (DescriptorCommon *) data;
 
	if ((strcmp(name, IPDR_COMPLEX_TYPE) == 0) && (pDescriptorCommon->serviceFlag_ == 0)) {
        	if(atts[count] == NULL){
				pDescriptorCommon->complexTypeFlag_ = 1;
            if(pDescriptorCommon->complexTypeFlag_ == 1) {
    				appendNode(&pDescriptorCommon->pAttrTypeList_, 
         					  "structure");   
    				appendNode(&pDescriptorCommon->pComplexAttrTypeList_, 
      		              "structure");   
					pDescriptorCommon->nameTypeFlag_ = 0;
				}
			}
         else {
				while(atts[count] != NULL) {
					if (strcmp(atts[count], "name") == 0) {
						count++;
  						strcpy(pDescriptorCommon->pFNFType_->serviceType_, atts[count]);        
				pDescriptorCommon->recordFlag_ = 1;
				if (pDescriptorCommon->nameTypeFlag_ != 0) {
					appendNode(&pDescriptorCommon->pAttrTypeList_, 
						IPDR_STRING);
				pDescriptorCommon->nameTypeFlag_ = 0;
 				}
	if ((pDescriptorCommon->nameSpaceId_ == NULL) || 
			(strcmp(pDescriptorCommon->nameSpaceId_, "ipdr") == 0)) {
			appendNode(&pDescriptorCommon->pAttrNameList_, 
				(char *)atts[count]);        
			} else {
				tempLen = strlen(atts[count]) + strlen(pDescriptorCommon->nameSpaceId_) + 5;
				attributeName = (char *) calloc (tempLen, sizeof(char));
				strcpy(attributeName, pDescriptorCommon->nameSpaceId_);
				strcat(attributeName, ":");
				strcat(attributeName, (char *) atts[count]);
				appendNode(&pDescriptorCommon->pAttrNameList_, 
				attributeName);        
				free(attributeName);
				}
	pDescriptorCommon->nameTypeFlag_ = 1;
				pDescriptorCommon->complexTypeFlag_ = 1;
            if(pDescriptorCommon->complexTypeFlag_ == 1) {
    				appendNode(&pDescriptorCommon->pAttrTypeList_, 
         					  "structure");   
    				appendNode(&pDescriptorCommon->pComplexAttrTypeList_, 
      		              "structure");   
					pDescriptorCommon->nameTypeFlag_ = 0;
				}
					}
					count++;
				}
				pDescriptorCommon->complexTypeFlag_ = 1;
				pDescriptorCommon->serviceFlag_ = 0;
			}
	}

	if (strcmp(name, IPDR_COMPLEXCONTENT) == 0) {
		pDescriptorCommon->complexTypeFlag_ = 0;
		pDescriptorCommon->recordFlag_ = 0;
		pDescriptorCommon->serviceFlag_ = 1;
	}

 if (strcmp(name, IPDR_SCHEMA) == 0) {
        while (atts[count] != NULL) {
   if (strcmp(atts[count], IPDR_TARGET_NAMESPACE) == 0) {
    count++;
    strcpy(pDescriptorCommon->targetNameSpace_, atts[count]);
   }
 
   
   if (( atts[count][0] == 'x' ) &&
    ( atts[count][1] == 'm' ) &&
    ( atts[count][2] == 'l' ) &&
    ( atts[count][3] == 'n' ) &&
    ( atts[count][4] == 's' )) {
    if (strcmp(pDescriptorCommon->targetNameSpace_, atts[count + 1]) == 0) {
     lenNameSpaceId = strlen(atts[count]) + 1;
	 pDescriptorCommon->nameSpaceId_ = (char *) calloc (lenNameSpaceId, sizeof(char));

     getIPDRSubstring((char *) atts[count], pDescriptorCommon->nameSpaceId_, ':');
    } 
    count++;
   }
   /* Adding code to read the schema version */
   if ( strcmp(atts[count], "version") == 0) {
     count++;
	#ifdef DEBUG_IPDR	
     printf("the version read from schema is: %s\n", atts[count]);
	#endif
     cnt = cnt + 1;
     if (cnt == 1) {
        if(strstr(atts[count],IPDR_VERSION_3_5) != NULL){
           strcpy(gVersion, IPDR_VERSION_3_5);
        }
        else if((strstr(atts[count],IPDR_VERSION_3_0) != NULL) || (strstr(atts[count],IPDR_VERSION_3_1) != NULL)){
           strcpy(gVersion, IPDR_VERSION_3_1);
        }
     }
   }
   count++;            
  }
}
 
 
 

 /* Getting extension base */
 if (strcmp(name, IPDR_EXTENSION) == 0) {
        if (strcmp(atts[count], IPDR_BASE) == 0) {
   count++;
   lenExtensionBase = strlen(atts[count]) + 1;
   pDescriptorCommon->extensionBase_ = (char *) calloc (lenExtensionBase, sizeof(char));
   strcpy(pDescriptorCommon->extensionBase_, atts[count]);
  }
 }
 
 /* Getting include schemaLocation_ */
 if (strcmp(name, IPDR_INCLUDE) == 0) {
        if (strcmp(atts[count], IPDR_SCHEMA_LOCATION) == 0) {
   count++;
   strcpy(pDescriptorCommon->schemaLocation_, atts[count]);
  }
 }
 

 /* Getting include schemaLocation_ */
 if (strcmp(name, IPDR_IMPORT) == 0) {
  while(atts[count] != NULL) {
   if (strcmp(atts[count], IPDR_SCHEMA_LOCATION) == 0) {
    count++;
    strcpy(pDescriptorCommon->schemaLocation_, atts[count]);
   }
   count++;
  }
 }
 
 if (strcmp(name, IPDR_ELEMENT) == 0) {
   while (atts[count] != NULL) {
   if (strcmp(atts[count], IPDR_NAME) == 0) {
				pDescriptorCommon->recordFlag_ = 1;
                count++;
				if (pDescriptorCommon->nameTypeFlag_ != 0) {
					appendNode(&pDescriptorCommon->pAttrTypeList_, 
						IPDR_STRING);
				pDescriptorCommon->nameTypeFlag_ = 0;
 				}

	if ((pDescriptorCommon->nameSpaceId_ == NULL) || 
			(strcmp(pDescriptorCommon->nameSpaceId_, "ipdr") == 0)) {
			appendNode(&pDescriptorCommon->pAttrNameList_, 
				(char *)atts[count]);        
			} else {
				tempLen = strlen(atts[count]) + strlen(pDescriptorCommon->nameSpaceId_) + 5;
				attributeName = (char *) calloc (tempLen, sizeof(char));
				strcpy(attributeName, pDescriptorCommon->nameSpaceId_);
				strcat(attributeName, ":");
				strcat(attributeName, (char *) atts[count]);
				appendNode(&pDescriptorCommon->pAttrNameList_, 
				attributeName);        
				free(attributeName);
				}
	pDescriptorCommon->nameTypeFlag_ = 1;
				
   }

   if (strcmp(atts[count], IPDR_TYPE) == 0) {
                count++;
    		appendNode(&pDescriptorCommon->pAttrTypeList_, 
      			     (char *)atts[count]);   
    		appendNode(&pDescriptorCommon->pComplexAttrTypeList_, 
      			     (char *)atts[count]);   
			pDescriptorCommon->nameTypeFlag_ = 0;
   }
   
   
	if (strcmp(atts[count], IPDR_REF) == 0) {
      if(pDescriptorCommon->complexTypeFlag_ == 1) {
			pDescriptorCommon->recordFlag_ = 0;
			appendNode ( &pDescriptorCommon->pSchemaElementAttrClassList_, IPDR_MANDATORY);
			appendNode ( &pDescriptorCommon->pSchemaElementAttrMaxOcursList_, "0");
         count++;
			tempLen = 0;
			tempLen = strlen(atts[count]) + 1;
			tmp = (char *) calloc (tempLen, sizeof(char));
			getIPDRSubstring((char *) atts[count], tmp, ':');
			appendNode ( &pDescriptorCommon->pSchemaElementAttrNameList_, tmp);
			free(tmp);
		}
		else {
			pDescriptorCommon->recordFlag_ = 0;
			appendNode ( &pDescriptorCommon->pAttrClassList_, IPDR_MANDATORY);
			appendNode(&pDescriptorCommon->pAttrMaxOccursList_, "0");
         count++;
			tempLen = 0;
			tempLen = strlen(atts[count]) + 1;
			tmp = (char *) calloc (tempLen, sizeof(char));
			getIPDRSubstring((char *) atts[count], tmp, ':');
			appendNode ( &pDescriptorCommon->pAttrClassNameList_, tmp);
			free(tmp);
		}
	}

	if (strcmp(atts[count], IPDR_MINOCCURS) == 0) {
        count++;
      if(pDescriptorCommon->complexTypeFlag_ == 1) {
			if (strcmp(atts[count], "0") == 0) {
				nodeCount = getLength(pDescriptorCommon->pSchemaElementAttrClassList_);
				freeListNode(&(pDescriptorCommon->pSchemaElementAttrClassList_), nodeCount);
				appendNode(&pDescriptorCommon->pSchemaElementAttrClassList_, 
							  IPDR_OPTIONAL);
			}
		}
		else {
			if (strcmp(atts[count], "0") == 0) {
				nodeCount = getLength(pDescriptorCommon->pAttrClassList_);
				freeListNode(&(pDescriptorCommon->pAttrClassList_), nodeCount);
				appendNode(&pDescriptorCommon->pAttrClassList_, 
								IPDR_OPTIONAL);
			}
		} 
    } 

if((strcmp(gVersion,IPDR_VERSION_3_5)) == 0){
	if (strcmp(atts[count], IPDR_MAXOCCURS) == 0) {
        count++;
      if(pDescriptorCommon->complexTypeFlag_ == 1) {
			if (strcmp(atts[count], "unbounded") == 0) {
				nodeCount = getLength(pDescriptorCommon->pSchemaElementAttrMaxOcursList_);
				freeListNode(&(pDescriptorCommon->pSchemaElementAttrMaxOcursList_), nodeCount);
				appendNode(&pDescriptorCommon->pSchemaElementAttrMaxOcursList_, "-1");
			}
			else if(atoi(atts[count]) > 0) {
				nodeCount = getLength(pDescriptorCommon->pSchemaElementAttrMaxOcursList_);
				freeListNode(&(pDescriptorCommon->pSchemaElementAttrMaxOcursList_), nodeCount);
				tempLen = 0;
				tempLen = strlen(atts[count]) + 1;
				tmp = (char *) calloc (tempLen, sizeof(char));
				strcpy(tmp, atts[count]);
				appendNode(&pDescriptorCommon->pSchemaElementAttrMaxOcursList_, tmp);
				free(tmp);
			}
		}
		else {
			if (strcmp(atts[count], "unbounded") == 0) {
				nodeCount = getLength(pDescriptorCommon->pAttrMaxOccursList_);
				freeListNode(&(pDescriptorCommon->pAttrMaxOccursList_), nodeCount);
				appendNode(&pDescriptorCommon->pAttrMaxOccursList_, "-1");
			}
			else if(atoi(atts[count]) > 0) {
				nodeCount = getLength(pDescriptorCommon->pAttrMaxOccursList_);
				freeListNode(&(pDescriptorCommon->pAttrMaxOccursList_), nodeCount);
				tempLen = 0;
				tempLen = strlen(atts[count]) + 1;
				tmp = (char *) calloc (tempLen, sizeof(char));
				strcpy(tmp, atts[count]);
				appendNode(&pDescriptorCommon->pAttrMaxOccursList_, tmp);
				free(tmp);
			}
		} 
    } else {
       count++;
    }
    } else {
       count++;
    }

	}
}

/* IPDR v3.5,  Enum requirement. Check for version */
 if((strcmp(gVersion,IPDR_VERSION_3_5)) == 0)
 {
	if (strcmp(name, IPDR_RESTRICTION) == 0) {
		while (atts[count] != NULL) {
			if (strcmp(atts[count], "base") == 0) {
				count++;
    				appendNode(&pDescriptorCommon->pAttrTypeList_, 
         				   (char *)atts[count]);
    				/*appendNode(&pDescriptorCommon->pComplexAttrTypeList_, 
      			     (char *)atts[count]);*/
				pDescriptorCommon->nameTypeFlag_ = 0;
                         }
                    count++;
                }
        }
 }
}  
 
/*!
 *  NAME:
 *      end() - This function is called as any end tag  
 *                         for an element is encountered..
 *
 *  DESCRIPTION:
 *      1. This function populates FNFType with the values read for Attribute 
 *         Name, Attribute Type & Attribute description (Optional or 
 *         Mandatory)
 *      2. This function also performs the validation for Optional Attributes
 *         passed as input parameter
 * 
 *  ARGUMENTS:
 *       - data {IN}
 *       -  name {IN}
 *
 *  RETURNS  
 *      Returns void
 *    
 */
static void end(void *data, const char *name) 
{
 int attributeCount = 0;
 int attributeStatus = 0;
 int count = 0;
 int errorSeverity = 0;
 int tmpNoOfElements = 0;
 unsigned int length = 0;
 char *tempAttrName = NULL;
 char *tempAttrType = NULL;
 char *tempNewAttrType = NULL;
 char *tempAttrClass = NULL;
 char *tempNoOfElements = NULL;
 char *tempAttrDerivedType = NULL;
 char *tempComplexAttrType = NULL;
 List* tempAttrNameList_ = NULL;
 List* tempAttrclassList_ = NULL;
 List* tempAttrMaxOccursList_ = NULL;
 
 DescriptorCommon *pDescriptorCommon = NULL;
 
 pDescriptorCommon = (DescriptorCommon *) data;

	if (strcmp(name, IPDR_COMPLEX_TYPE) == 0) {
		if(pDescriptorCommon->serviceFlag_ == 0) {
			pDescriptorCommon->recordFlag_ = 1;
		}
			while(pDescriptorCommon->pSchemaElementAttrNameList_ != NULL) {
				appendNode (&(pDescriptorCommon->pTempListAttrNameList_),
								 pDescriptorCommon->pSchemaElementAttrNameList_->node);
				pDescriptorCommon->pSchemaElementAttrNameList_ = 
				pDescriptorCommon->pSchemaElementAttrNameList_->pNext;
			}

			while(pDescriptorCommon->pSchemaElementAttrClassList_ != NULL) {
				appendNode (&(pDescriptorCommon->pTempListAttrClassList_),
								 pDescriptorCommon->pSchemaElementAttrClassList_->node);
				pDescriptorCommon->pSchemaElementAttrClassList_ = 
				pDescriptorCommon->pSchemaElementAttrClassList_->pNext;
			}

			while(pDescriptorCommon->pSchemaElementAttrMaxOcursList_ != NULL) {
				appendNode (&(pDescriptorCommon->pTempListAttrMaxOccursList_),
								 pDescriptorCommon->pSchemaElementAttrMaxOcursList_->node);
				pDescriptorCommon->pSchemaElementAttrMaxOcursList_ = 
				pDescriptorCommon->pSchemaElementAttrMaxOcursList_->pNext;
			}
	}

 if ((strcmp(name, IPDR_ELEMENT) == 0) || (strcmp(name, IPDR_COMPLEX_TYPE) == 0)) {
 if (pDescriptorCommon->recordFlag_ == 1) {  
  for (count = 0; 
    count < getLength(pDescriptorCommon->pAttrNameList_); 
    count++) {
                errorSeverity = getNode(pDescriptorCommon->pAttrNameList_, count + 1, 
     pDescriptorCommon->attrNameNode_);
        }
  
        for (
   count = 0; 
   count < getLength(pDescriptorCommon->pAttrTypeList_);
   count++) {
                errorSeverity = getNode(pDescriptorCommon->pAttrTypeList_, count + 1, 
     pDescriptorCommon->attrTypeNode_);
        }
 
        for (
   count = 0; 
   count < getLength(pDescriptorCommon->pAttrClassList_); 
   count++) {
                errorSeverity = getNode(pDescriptorCommon->pAttrClassList_, count + 1, 
     pDescriptorCommon->attrClassNode_);
        }

   count = getLength(pDescriptorCommon->pAttrNameList_);
   count = count - 1;
   if(count >= 0) {
   tempAttrName = (char *) calloc (MAX_ATTRIBUTE_NAME, sizeof(char));
   errorSeverity = getNode(pDescriptorCommon->pAttrNameList_, count + 1, 
    tempAttrName);
 
   tempAttrType = (char *) calloc (MAX_ATTRIBUTE_TYPE, sizeof(char));
    errorSeverity = getNode(pDescriptorCommon->pAttrTypeList_, count + 1, 
    tempAttrType);
   tempNewAttrType = (char *) calloc (MAX_ATTRIBUTE_TYPE, sizeof(char));   
   
   errorSeverity = getIPDRSubstring(tempAttrType, tempNewAttrType, ':');
  
   tempAttrDerivedType = (char *) calloc (MAX_ATTRIBUTE_TYPE, sizeof(char));
   strcpy(tempAttrDerivedType, tempNewAttrType);
 
   tempComplexAttrType = (char *) calloc (MAX_ATTRIBUTE_TYPE, sizeof(char));
    errorSeverity = getNode(pDescriptorCommon->pComplexAttrTypeList_, count + 1, 
    tempComplexAttrType);

   if (isPrimitive(tempNewAttrType) == IPDR_FALSE) {
    getPrimitiveType(tempNewAttrType, tempAttrDerivedType);
   }
 
   populateAttributeType(tempNewAttrType);
   populateAttributeType(tempAttrDerivedType);

   tempAttrClass = (char *) calloc (MAX_ATTRIBUTE_CLASS, sizeof(char));
   errorSeverity = getNode(pDescriptorCommon->pAttrClassList_, count + 1, 
    tempAttrClass);

   tempNoOfElements = (char *) calloc (MAX_ATTRIBUTE_CLASS, sizeof(char));
   errorSeverity = getNode(pDescriptorCommon->pAttrMaxOccursList_, count + 1, 
    tempNoOfElements);

	if(strcmp(tempNoOfElements, "-1") == 0) {
		tmpNoOfElements = -1;
	}
	else if(strcmp(tempNoOfElements, "0") == 0) {
		tmpNoOfElements = 0;
	}
	else if(atoi(tempNoOfElements) > 0) {
		tmpNoOfElements = atoi(tempNoOfElements);
	}

 	tempAttrNameList_ = pDescriptorCommon->pTempListAttrNameList_;
 	tempAttrclassList_ = pDescriptorCommon->pTempListAttrClassList_;
 	tempAttrMaxOccursList_ = pDescriptorCommon->pTempListAttrMaxOccursList_;

   pDescriptorCommon->pTempListAttrNameList_ = NULL;
   pDescriptorCommon->pTempListAttrClassList_ = NULL;
   pDescriptorCommon->pTempListAttrMaxOccursList_ = NULL;

   if ((strcmp(tempAttrClass, IPDR_OPTIONAL) != 0) || 
    ((strcmp(tempAttrClass, IPDR_OPTIONAL) == 0))) {
    appendListAttributeDescriptor
    (&pDescriptorCommon->pFNFType_->pListAttributeDescriptor_,
     tempAttrName, 
     tempNewAttrType, 
     tempAttrClass,
     tempAttrDerivedType,
     tempComplexAttrType,
     tempAttrNameList_,
     tempAttrclassList_,
     tempAttrMaxOccursList_,
	  tmpNoOfElements);
	 }


  free(tempAttrName);
  free(tempAttrType);
  free(tempNewAttrType);
  free(tempAttrClass);
  free(tempAttrDerivedType);
  free(tempComplexAttrType);
  free(tempNoOfElements);
  }
  }
  }


	if (strcmp(name, IPDR_SCHEMA) == 0) {

	ListAttributeDescriptor * pCurrentListAttributeDescriptor = pDescriptorCommon->pFNFType_->pListAttributeDescriptor_;
	List* pCurrAttrClassList_ = pDescriptorCommon->pAttrClassList_;
	List* pCurrAttrClassNameList_ = pDescriptorCommon->pAttrClassNameList_;
	List* pCurrAttrMaxOccursList_ = pDescriptorCommon->pAttrMaxOccursList_;
   	
   tempNewAttrType = (char *) calloc (MAX_ATTRIBUTE_TYPE, sizeof(char));   
	/*while(pDescriptorCommon->pAttrClassList_ != NULL) */
	while(pDescriptorCommon->pAttrClassNameList_ != NULL) {
		while(pDescriptorCommon->pFNFType_->pListAttributeDescriptor_ != NULL) {
			getIPDRSubstring(pDescriptorCommon->pFNFType_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_, tempNewAttrType, ':');
			if(strcmp(pDescriptorCommon->pAttrClassNameList_->node, tempNewAttrType) == 0) {
				strcpy(pDescriptorCommon->pFNFType_->pListAttributeDescriptor_->pAttributeDescriptor_->description_,
				pDescriptorCommon->pAttrClassList_->node);
				pDescriptorCommon->pFNFType_->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = atoi(pDescriptorCommon->pAttrMaxOccursList_->node);
			}
			pDescriptorCommon->pFNFType_->pListAttributeDescriptor_ = pDescriptorCommon->pFNFType_->pListAttributeDescriptor_->pNext_;
		}
		pDescriptorCommon->pAttrClassList_ = pDescriptorCommon->pAttrClassList_->pNext;
		pDescriptorCommon->pAttrClassNameList_ = pDescriptorCommon->pAttrClassNameList_->pNext;
		pDescriptorCommon->pAttrMaxOccursList_ = pDescriptorCommon->pAttrMaxOccursList_->pNext;

		pDescriptorCommon->pFNFType_->pListAttributeDescriptor_ = pCurrentListAttributeDescriptor;
	}
	free(tempNewAttrType);
		pDescriptorCommon->pAttrClassList_ = pCurrAttrClassList_;
		pDescriptorCommon->pAttrClassNameList_ = pCurrAttrClassNameList_;
		pDescriptorCommon->pAttrMaxOccursList_ = pCurrAttrMaxOccursList_;
	
	}
}  
 
 
 
/*!
 *  NAME:
 *      isPrimitive() - This function populates the attributeType
 *                      to be one among the 8 primitive types
 *
 *  DESCRIPTION:
 *      1. This function populates the attributeType
 *         to be one among the 8 primitive types
 *
 *  ARGUMENTS:
 *       - attributeType {IN}
 *
 *  RETURNS  
 *      Returns IPDR_TRUE if it is among primitive types,
 *           else returns IPDR_FALSE
 *    
 */
 
int isPrimitive(char *tempNewAttrType)
{
   if  (	(strcmp(tempNewAttrType, IPDR_LONG) == 0) ||
        	(strcmp(tempNewAttrType, IPDR_LONG_OTHER) == 0) ||
        	(strcmp(tempNewAttrType, IPDR_U_LONG) == 0) ||
        	(strcmp(tempNewAttrType, IPDR_HYPER) == 0) ||
        	(strcmp(tempNewAttrType, IPDR_U_HYPER) == 0) ||
  		  	(strcmp(tempNewAttrType, IPDR_FLOAT) == 0) ||
        	(strcmp(tempNewAttrType, IPDR_DOUBLE) == 0) ||
			(strcmp(tempNewAttrType, IPDR_BOOLEAN) == 0) ||
			(strcmp(tempNewAttrType, IPDR_SHORT) == 0) ||
			(strcmp(tempNewAttrType, IPDR_U_SHORT) == 0) ||
  			(strcmp(tempNewAttrType, IPDR_STRING) == 0) ||
  			(strcmp(tempNewAttrType, IPDR_HEX_BIN_ARRAY) == 0) ||
  			(strcmp(tempNewAttrType, IPDR_BYTE) == 0) ||
  			(strcmp(tempNewAttrType, IPDR_U_BYTE) == 0) ||
  			(strcmp(tempNewAttrType, IPDR_BYTE_ARRAY) == 0))
  return (IPDR_TRUE);
   else
    return (IPDR_FALSE);
}
 

/*!
 *  NAME:
 *      getPrimitiveType() - This function gets the primitive type of the 
 *                           attribute passed
 *
 *  DESCRIPTION:
 *      1. This function gets the primitive type of the 
 *         attribute passed
 *
 *  ARGUMENTS:
 *       - attributeType {IN}
 *       -  attributeDerivedType {IN}
 *
 *  RETURNS  
 *      Returns Zero if the operation is successful or appropriate
 *    error code in case of failure.
 *    
 */
 
int getPrimitiveType(char* attributeType, 
      char* attributeDerivedType)
{
 int flag = IPDR_FALSE;
 
 if (strcmp(attributeDerivedType, 
  IPDR_DERIVED_DATETIME) == 0) {
  strcpy(attributeType, IPDR_LONG);
  flag = IPDR_TRUE;
 }
 
 if (strcmp(attributeDerivedType, 
  IPDR_DERIVED_DATETIME_MILISEC) == 0) {
  strcpy(attributeType, IPDR_HYPER);
  flag = IPDR_TRUE;
 }
 
 if (strcmp(attributeDerivedType, 
  IPDR_DERIVED_IPV4ADDR) == 0) {
  strcpy(attributeType, IPDR_LONG); 
  flag = IPDR_TRUE;
 }
 
 if (strcmp(attributeDerivedType, 
  IPDR_DERIVED_IPV6ADDR) == 0) {
  strcpy(attributeType, IPDR_BYTE_ARRAY);
  flag = IPDR_TRUE;
 }
 
 if (strcmp(attributeDerivedType, 
  IPDR_DERIVED_UUID) == 0) {
  strcpy(attributeType, IPDR_BYTE_ARRAY);
  flag = IPDR_TRUE;
 }
 
 if (strcmp(attributeDerivedType, 
  IPDR_DERIVED_IPADDR) == 0) {
  strcpy(attributeType, IPDR_BYTE_ARRAY);
  flag = IPDR_TRUE;
 }

 if (strcmp(attributeDerivedType, 
  IPDR_DERIVED_DATETIME_MICROSEC) == 0) {
  strcpy(attributeType, IPDR_HYPER);
  flag = IPDR_TRUE;
 }

 if (flag == IPDR_FALSE){
  strcpy(attributeType, IPDR_STRING);
  strcpy(attributeDerivedType, IPDR_STRING);
} 
 return (IPDR_OK);
}
 
/*!
 *  NAME:
 *      populateAttributeType() - This function populates the attributeType
 *                      to be one among the 8 primitive types
 *
 *  DESCRIPTION:
 *      1. This function populates the attributeType
 *         to be one among the 8 primitive types
 *
 *  ARGUMENTS:
 *       - attributeType {IN}
 *
 *  RETURNS  
 *      Returns IPDR_TRUE if it is among primitive types,
 *           else returns IPDR_FALSE
 *    
 */
 

int populateAttributeType(char* attributeType)
{
 if ((strcmp(attributeType, IPDR_LONG) == 0) ||
  (strcmp(attributeType, IPDR_LONG_OTHER) == 0)) {
  
	strcpy(attributeType, IPDR_DESCRIPTOR_LONG);
 }
 
 else if ((strcmp(attributeType, IPDR_U_LONG) == 0)) { 
  strcpy(attributeType, IPDR_DESCRIPTOR_U_LONG);
 }
 
 else if (strcmp(attributeType, IPDR_HYPER) == 0) {
  strcpy(attributeType, IPDR_DESCRIPTOR_HYPER);
 }
 
 else if (strcmp(attributeType, IPDR_U_HYPER) == 0) {
  strcpy(attributeType, IPDR_DESCRIPTOR_U_HYPER);
 }
 
 else if (strcmp(attributeType, IPDR_FLOAT) == 0) {
  strcpy(attributeType, IPDR_DESCRIPTOR_FLOAT);
 }
 
 else if (strcmp(attributeType, IPDR_DOUBLE) == 0) {
  strcpy(attributeType, IPDR_DESCRIPTOR_DOUBLE);
 }
 
 else if (strcmp(attributeType, IPDR_STRING) == 0) {
  strcpy(attributeType, IPDR_DESCRIPTOR_STRING);
 }
 
 else if ((strcmp(attributeType, IPDR_BYTE_ARRAY) == 0) ||
			(strcmp(attributeType, IPDR_HEX_BIN_ARRAY) == 0)) {
  strcpy(attributeType, IPDR_DESCRIPTOR_BYTE_ARRAY);
 }
 
 if (strcmp(attributeType, IPDR_BOOLEAN) == 0) {
  strcpy(attributeType, IPDR_DESCRIPTOR_BOOLEAN);
 }

 if (strcmp(attributeType, IPDR_SHORT) == 0) {
  strcpy(attributeType, IPDR_DESCRIPTOR_SHORT);
 }

 if (strcmp(attributeType, IPDR_U_SHORT) == 0) {
  strcpy(attributeType, IPDR_DESCRIPTOR_U_SHORT);
 }

 if (strcmp(attributeType, IPDR_BYTE) == 0) {
  strcpy(attributeType, IPDR_DESCRIPTOR_BYTE);
 }

 if (strcmp(attributeType, IPDR_U_BYTE) == 0) {
  strcpy(attributeType, IPDR_DESCRIPTOR_U_BYTE);
 }

 return (IPDR_OK);
}
 

/*!
 *  NAME:
 *      freeDescriptorCommon() - Memory deallocation of DescriptorCommon structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory deallocation of DescriptorCommon 
 *         structure & its members.
 * 
 *  ARGUMENTS:
 *       - DescriptorCommon {IN/OUT} 
 *
 *  RETURNS  
 *      Returns Zero if the operation is successful or appropriate
 *    error code in case of failure.
 */
 

int freeDescriptorCommon(DescriptorCommon* pDescriptorCommon)
{
 
 int count = 0;
 int attributeCount = 0;
 
 free(pDescriptorCommon->nameSpaceId_);
 free(pDescriptorCommon->extensionBase_);
 free(pDescriptorCommon->schemaLocation_);
 freeListNameSpaceInfo (&(pDescriptorCommon->pListNameSpaceInfo_));
 freeListAttributeDescriptor(&(pDescriptorCommon->pFNFType_->pListAttributeDescriptor_));
 free(pDescriptorCommon->pFNFType_->serviceType_);
 free(pDescriptorCommon->pFNFType_);
 free(pDescriptorCommon->attrClassNode_);
 free(pDescriptorCommon->attrNameNode_);
 free(pDescriptorCommon->attrTypeNode_);
 free(pDescriptorCommon->targetNameSpace_);
 free(pDescriptorCommon->pErrorCode_);
 freeList(&(pDescriptorCommon->pAttrNameList_));
 freeList(&(pDescriptorCommon->pAttrTypeList_));
 freeList(&(pDescriptorCommon->pAttrClassList_));
 freeList(&(pDescriptorCommon->pAttrClassNameList_));
 freeList(&(pDescriptorCommon->pAttrMaxOccursList_));
 freeList(&(pDescriptorCommon->pComplexAttrTypeList_));
 freeList(&(pDescriptorCommon->pSchemaElementAttrClassList_));
 freeList(&(pDescriptorCommon->pSchemaElementAttrNameList_));
 freeList(&(pDescriptorCommon->pSchemaElementAttrMaxOcursList_));
 freeList(&(pDescriptorCommon->pTempListAttrNameList_));
 freeList(&(pDescriptorCommon->pTempListAttrClassList_));
 freeList(&(pDescriptorCommon->pTempListAttrMaxOccursList_));
 
 free(pDescriptorCommon);

 return (IPDR_OK);
}
 
/*!
 *  NAME:
 *      initDescriptorCommon() - Memory allocation of DescriptorCommon structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory allocation of DescriptorCommon 
 *         structure & its members.
 * 
 *  ARGUMENTS:
 *       - DescriptorCommon {IN/OUT} 
 *       - pErrorCode {IN/OUT} 
 *
 *  RETURNS  
 *      Returns Zero if the operation is successful or appropriate
 *    error code in case of failure.
 */
 

int initDescriptorCommon(DescriptorCommon* pDescriptorCommon,
       int* pErrorCode)
{
 
 pDescriptorCommon->pAttrNameList_ = NULL;
 pDescriptorCommon->pAttrTypeList_ = NULL;
 pDescriptorCommon->pAttrClassList_ = NULL;
 pDescriptorCommon->pAttrClassNameList_ = NULL;
 pDescriptorCommon->pAttrMaxOccursList_ = NULL;
 pDescriptorCommon->pComplexAttrTypeList_ = NULL;
 pDescriptorCommon->pSchemaElementAttrClassList_ = NULL;
 pDescriptorCommon->pSchemaElementAttrNameList_ = NULL;
 pDescriptorCommon->pSchemaElementAttrMaxOcursList_ = NULL;
 pDescriptorCommon->pTempListAttrNameList_ = NULL;
 pDescriptorCommon->pTempListAttrClassList_ = NULL;
 pDescriptorCommon->pTempListAttrMaxOccursList_ = NULL;
 pDescriptorCommon->pListNameSpaceInfo_ = NULL;
 pDescriptorCommon->nameTypeFlag_ = 0;
 pDescriptorCommon->nameSpaceId_ = NULL;
 pDescriptorCommon->recordFlag_ = 0;
 pDescriptorCommon->complexTypeFlag_ = 0;
 pDescriptorCommon->serviceFlag_ = 0;
 
 pDescriptorCommon->attrNameNode_ = (char *) calloc 
  (MAX_ATTRIBUTE_NAME, sizeof(char));
  if (pDescriptorCommon->attrNameNode_ == NULL) {
  *pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
  return (IPDR_ERROR);
 }
 
 pDescriptorCommon->attrTypeNode_ = (char *) calloc 
  (MAX_ATTRIBUTE_TYPE, sizeof(char));
  if (pDescriptorCommon->attrTypeNode_ == NULL) {
  *pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
  return (IPDR_ERROR);
 }
 
 pDescriptorCommon->attrClassNode_ = (char *) calloc 
  (MAX_ATTRIBUTE_CLASS, sizeof(char));
  if (pDescriptorCommon->attrClassNode_ == NULL) {
  *pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
  return (IPDR_ERROR);
 }
 
 pDescriptorCommon->targetNameSpace_ = (char *) calloc 
  (MAX_NAMESPACE_URI, sizeof(char));
  if (pDescriptorCommon->targetNameSpace_ == NULL) {
  *pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
  return (IPDR_ERROR);
 }
 
 pDescriptorCommon->schemaLocation_ = (char *) calloc 
  (MAX_NAMESPACE_URI, sizeof(char));
  if (pDescriptorCommon->schemaLocation_ == NULL) {
  *pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
  return (IPDR_ERROR);
 }
 
 pDescriptorCommon->pErrorCode_ = (int *) calloc 
  (1, sizeof(int));
 if (pDescriptorCommon->pErrorCode_ == NULL) {
  *pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
  return (IPDR_ERROR);
 }
 
 pDescriptorCommon->pFNFType_ = newFNFType();
 if (pDescriptorCommon->pFNFType_ == NULL) {
  *pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
  return (IPDR_ERROR);
 }
 
 return (IPDR_OK);
}
 
/*!
 *  NAME:
 *      parseSchema() - This function parses the schema file
 *
 *  DESCRIPTION:
 *      1. This function This function parses the schema file
 * 
 *  ARGUMENTS:
 *       - SchemaURI {IN} 
 *       - DescriptorCommon {IN/OUT} 
 *       - pErrorCode {IN/OUT} 
 *
 *  RETURNS  
 *      Returns Zero if the operation is successful or appropriate
 *    error code in case of failure.
 */
 
int parseSchema(char* SchemaURI, 
				DescriptorCommon* pDescriptorCommon,
				int *pErrorCode)
 
{
	FILE *fp;
	void* userData = NULL;
	int count = 0;
	int retValue = 0;
	unsigned int lengthSchemaFile = 0;
	char *Buff = NULL;
	char* configParams = NULL;
	char* SchemaFile = NULL;
	XML_Parser xmlParser;


	configParams = (char *) calloc (MAX_IPDR_STRING, sizeof(char));
	SchemaFile = (char *) calloc (MAX_IPDR_STRING, sizeof(char));

	retValue = getFileName(SchemaURI, SchemaFile);
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
		*pErrorCode = IPDR_DESCRIPTOR_SCHEMA_FILE_NOT_FOUND;
		free(configParams);
		free(SchemaFile);
		return (IPDR_ERROR);
	}

	if (SchemaFile == NULL) {
		*pErrorCode = IPDR_DESCRIPTOR_SCHEMA_FILE_NULL;
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
		*pErrorCode = IPDR_DESCRIPTOR_PARSER_MEMORY_ALLOC_FAILED;
		free(configParams);
		free(SchemaFile);
		free(Buff);
		return (IPDR_ERROR);
	}

	XML_SetElementHandler(xmlParser, start, end);

	/* Opening the file passed as the input parameter */
	if ((fp = fopen(SchemaFile, "rw+")) == NULL) {
		*pErrorCode = IPDR_DESCRIPTOR_FILE_NOT_FOUND;
		free(configParams);
		free(SchemaFile);
		free(Buff);
		return (IPDR_ERROR);
	} 

	initDescriptorCommon(pDescriptorCommon, pErrorCode);
	userData = (DescriptorCommon *) pDescriptorCommon;
	XML_SetUserData(xmlParser, userData);

	/* Parsing the Schema file */
	for (;;) {
		int done;
		int xmlFileLength;

		xmlFileLength = fread(Buff, 1, lengthSchemaFile, fp);
		if (ferror(fp)) {
			*pErrorCode = IPDR_DESCRIPTOR_READ_ERROR;
			free(configParams);
			free(SchemaFile);
			free(Buff);
			return (IPDR_ERROR);
		}

		done = feof(fp);

		if (! XML_Parse(xmlParser, Buff, xmlFileLength, done)) {
			/*
			*pErrorCode = IPDR_DESCRIPTOR_PARSER_READ_ERROR;
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

	/* Returning back the FNFType structure to calling function */
	userData = (DescriptorCommon *) XML_GetUserData(xmlParser);
	pDescriptorCommon = (DescriptorCommon *) userData;

	free(Buff);
	free(configParams);
	free(SchemaFile);
	return (IPDR_OK);
}
 
int generateDescriptor(char* SchemaURI, 
					   FNFType* pFNFType,
					   char** OptionalAttributes,
					   int* pErrorCode)
{
	unsigned int lengthList = 0;
	unsigned int count = 0;
	int flag = 0;
	int myFlag = 0;
	int errorSeverity = 0;

	char* tempAttributeName;
	char* tempSchemaURI = NULL;

	FNFType* pNewFNFType;
	DescriptorCommon* pDescriptorCommon;
	ListAttributeDescriptor* pCurrentListAttributeDescriptor = NULL;

	if (pFNFType == NULL) {
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		return(IPDR_ERROR);
	}

	pNewFNFType = newFNFType();

	/* 
	* Initializing the pDescriptorCommon structure
	* This structure is used to share the common parameters between 
	* XML parsing functions 
	*/ 
	tempSchemaURI = (char *) calloc ( 500, sizeof(char));

	pDescriptorCommon = (DescriptorCommon *) calloc 
						(1, sizeof(DescriptorCommon));
	
	if (pDescriptorCommon == NULL) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		return (IPDR_ERROR);
	}

	if (strlen(SchemaURI) == 0) {
		*pErrorCode = IPDR_NO_SD_URI;
		return (IPDR_ERROR);
	}

	strcpy(tempSchemaURI, SchemaURI);


   /* 
	* getAll specifies that all the mandatory & 
	* optional attributes are to be populated into FNFType 
	*/

	do {
		errorSeverity = parseSchema(
								tempSchemaURI, 
								pDescriptorCommon,
								pErrorCode);
		if (errorSeverity == IPDR_ERROR) {
			free(tempSchemaURI);
			freeFNFType(pNewFNFType);
			return (IPDR_ERROR);
		}

		if (flag == 0) {
			strcpy(pNewFNFType->serviceType_, pDescriptorCommon->pFNFType_->serviceType_);
			flag = 1;
		}

		lengthList = getLengthListAttributeDescriptor
					(pDescriptorCommon->pFNFType_->pListAttributeDescriptor_);

		pCurrentListAttributeDescriptor = pDescriptorCommon->pFNFType_->pListAttributeDescriptor_;
		
		for (count = 0; count < lengthList; count++) {
			tempAttributeName = (char *) calloc (2 * MAX_ATTRIBUTE_NAME, sizeof(char));

			if (pDescriptorCommon->pFNFType_->pListAttributeDescriptor_ != NULL) {
				if (pDescriptorCommon->nameSpaceId_ == NULL) {
					*pErrorCode = IPDR_XML_INPUT_FILE_INVALID;
					return(IPDR_ERROR);
				}

				if (strcmp(pDescriptorCommon->nameSpaceId_, "ipdr") != 0) {
					strcat(tempAttributeName, pDescriptorCommon->nameSpaceId_);
					strcat(tempAttributeName, ":");
				}

				strcat(tempAttributeName, pDescriptorCommon->pFNFType_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);

				appendListAttributeDescriptor(&pNewFNFType->pListAttributeDescriptor_, 
											   tempAttributeName,
											   pDescriptorCommon->pFNFType_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeType_,
											   pDescriptorCommon->pFNFType_->pListAttributeDescriptor_->pAttributeDescriptor_->description_,
											   pDescriptorCommon->pFNFType_->pListAttributeDescriptor_->pAttributeDescriptor_->derivedType_,
             pDescriptorCommon->pFNFType_->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_, 
             pDescriptorCommon->pFNFType_->pListAttributeDescriptor_->pAttributeDescriptor_->pListAttrNameList_, 
             pDescriptorCommon->pFNFType_->pListAttributeDescriptor_->pAttributeDescriptor_->pListAttrClassList_, 
             pDescriptorCommon->pFNFType_->pListAttributeDescriptor_->pAttributeDescriptor_->pListAttrMaxOccursList_, 
             pDescriptorCommon->pFNFType_->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_); 

				pDescriptorCommon->pFNFType_->pListAttributeDescriptor_ = pDescriptorCommon->pFNFType_->pListAttributeDescriptor_->pNext_;

				free(tempAttributeName);
			}
		}

		pDescriptorCommon->pFNFType_->pListAttributeDescriptor_ = pCurrentListAttributeDescriptor;

		if (strcmp(pDescriptorCommon->extensionBase_, IPDR_FINAL_SCHEMA) != 0) {
			strcpy(tempSchemaURI, pDescriptorCommon->schemaLocation_);
		}
	} 
	while (strcmp(pDescriptorCommon->extensionBase_, IPDR_FINAL_SCHEMA) != 0);

	if (pNewFNFType->serviceType_ != NULL)
		strcpy(pFNFType->serviceType_, pNewFNFType->serviceType_);
	
	errorSeverity = getSchemaDescriptor(OptionalAttributes, pNewFNFType->pListAttributeDescriptor_, pFNFType);
	if (errorSeverity == IPDR_ERROR) {
		free(tempSchemaURI);
		freeFNFType(pNewFNFType);
		freeDescriptorCommon(pDescriptorCommon);
		return (IPDR_ERROR);
	}
	

	free(tempSchemaURI);
	freeDescriptorCommon(pDescriptorCommon);
	freeFNFType(pNewFNFType);

	return (IPDR_OK);
}
 

int getSchemaDescriptor(char** OptionalAttributes, 
      ListAttributeDescriptor* pListAttributeDescriptor,
      FNFType* pFNFType)
{
	int count = 0;
	ListAttributeDescriptor* pCurrent = pListAttributeDescriptor;

	pFNFType->pListAttributeDescriptor_ = NULL;
	pFNFType->pListServiceAttributeDescriptor_ = NULL;
	while (pCurrent != NULL) {
		if 
		(((isExisting(pCurrent->pAttributeDescriptor_->attributeName_, OptionalAttributes)) &&
		(strcmp(pCurrent->pAttributeDescriptor_->description_, IPDR_OPTIONAL) == 0)) ||
		(strcmp(pCurrent->pAttributeDescriptor_->description_, IPDR_MANDATORY) == 0))
		{
			appendListAttributeDescriptor(
			 &pFNFType->pListAttributeDescriptor_,
			 pCurrent->pAttributeDescriptor_->attributeName_,
			 pCurrent->pAttributeDescriptor_->attributeType_,
			 pCurrent->pAttributeDescriptor_->description_,
			 pCurrent->pAttributeDescriptor_->derivedType_,
          pCurrent->pAttributeDescriptor_->complexType_, 
          pCurrent->pAttributeDescriptor_->pListAttrNameList_, 
          pCurrent->pAttributeDescriptor_->pListAttrClassList_,
          pCurrent->pAttributeDescriptor_->pListAttrMaxOccursList_,
          pCurrent->pAttributeDescriptor_->noOfElements_ 
			 );
		}
		pCurrent = pCurrent->pNext_; 
	}

	pCurrent = pListAttributeDescriptor;
	while (pCurrent != NULL) {

			appendListAttributeDescriptor(
			 &pFNFType->pListServiceAttributeDescriptor_,
			 pCurrent->pAttributeDescriptor_->attributeName_,
			 pCurrent->pAttributeDescriptor_->attributeType_,
			 pCurrent->pAttributeDescriptor_->description_,
			 pCurrent->pAttributeDescriptor_->derivedType_,
          pCurrent->pAttributeDescriptor_->complexType_, 
          pCurrent->pAttributeDescriptor_->pListAttrNameList_, 
          pCurrent->pAttributeDescriptor_->pListAttrClassList_,
          pCurrent->pAttributeDescriptor_->pListAttrMaxOccursList_,
          pCurrent->pAttributeDescriptor_->noOfElements_ 
			 );
		pCurrent = pCurrent->pNext_; 
	}

	count = getLengthListAttributeDescriptor(pListAttributeDescriptor);

	return (IPDR_OK);
}

int getSchemaDescriptorForVersion(char** OptionalAttributes, 
      ListAttributeDescriptor* pListAttributeDescriptor,
      FNFType* pFNFType)
{
	int count = 0;
	ListAttributeDescriptor* pCurrent = pListAttributeDescriptor;

	pFNFType->pListAttributeDescriptor_ = NULL;
	pFNFType->pListServiceAttributeDescriptor_ = NULL;
	while (pCurrent != NULL) {
		if 
		(((strcmp(pCurrent->pAttributeDescriptor_->description_, IPDR_OPTIONAL) == 0)) ||
		(strcmp(pCurrent->pAttributeDescriptor_->description_, IPDR_MANDATORY) == 0))
		{
			appendListAttributeDescriptor(
			 &pFNFType->pListAttributeDescriptor_,
			 pCurrent->pAttributeDescriptor_->attributeName_,
			 pCurrent->pAttributeDescriptor_->attributeType_,
			 pCurrent->pAttributeDescriptor_->description_,
			 pCurrent->pAttributeDescriptor_->derivedType_,
          pCurrent->pAttributeDescriptor_->complexType_, 
          pCurrent->pAttributeDescriptor_->pListAttrNameList_, 
          pCurrent->pAttributeDescriptor_->pListAttrClassList_,
          pCurrent->pAttributeDescriptor_->pListAttrMaxOccursList_,
          pCurrent->pAttributeDescriptor_->noOfElements_ 
			 );
		}
		pCurrent = pCurrent->pNext_; 
	}

	pCurrent = pListAttributeDescriptor;
	while (pCurrent != NULL) {

			appendListAttributeDescriptor(
			 &pFNFType->pListServiceAttributeDescriptor_,
			 pCurrent->pAttributeDescriptor_->attributeName_,
			 pCurrent->pAttributeDescriptor_->attributeType_,
			 pCurrent->pAttributeDescriptor_->description_,
			 pCurrent->pAttributeDescriptor_->derivedType_,
          pCurrent->pAttributeDescriptor_->complexType_, 
          pCurrent->pAttributeDescriptor_->pListAttrNameList_, 
          pCurrent->pAttributeDescriptor_->pListAttrClassList_,
          pCurrent->pAttributeDescriptor_->pListAttrMaxOccursList_,
          pCurrent->pAttributeDescriptor_->noOfElements_ 
			 );
		pCurrent = pCurrent->pNext_; 
	}

	count = getLengthListAttributeDescriptor(pListAttributeDescriptor);

	return (IPDR_OK);
}
 

int	appendListListAttributeDescriptor(
				ListAttributeDescriptor** pHeadRef,
				ListAttributeDescriptor** pListAttributeDescriptor)
{
	ListAttributeDescriptor* pOld = NULL;
	ListAttributeDescriptor* pCurrent = NULL;
	pOld = *pHeadRef;

	*pHeadRef = *pListAttributeDescriptor;

	pCurrent = *pHeadRef;

	while (pCurrent->pNext_ != NULL)
		pCurrent = pCurrent->pNext_;


	pCurrent->pNext_ = pOld;
	return (IPDR_OK);
}

int generateSchemaDescriptor(char* SchemaURI, 
							 FNFType* pNewFNFType,
							 ListNameSpaceInfo** pListNameSpaceInfo,
							 int *pErrorCode)
{
	unsigned int lengthList = 0;
	unsigned int count = 0;
	int flag = 0;
	int myFlag = 0;
	int errorSeverity = 0;
	
	char* tempAttributeName;
	char* tempSchemaURI = NULL;
	char *OptionalAttributes[4];

	DescriptorCommon* pDescriptorCommon;
	ListAttributeDescriptor* pCurrentListAttributeDescriptor = NULL;
	ListAttributeDescriptor* pListAttributeDescriptor = NULL;
	FNFType* pTempFNFType = NULL;

   pTempFNFType = newFNFType();

	OptionalAttributes[0] = (char *) calloc (sizeof(char), MAX_ATTRIBUTE_NAME);
   OptionalAttributes[0] = NULL;

	/* 
	* Initializing the pDescriptorCommon structure
	* This structure is used to share the common parameters between 
	* XML parsing functions 
	*/ 
	tempSchemaURI = (char *) calloc ( 500, sizeof(char));

	pDescriptorCommon = (DescriptorCommon *) calloc 
							(1, sizeof(DescriptorCommon));
	
	if (pDescriptorCommon == NULL) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		free(tempSchemaURI);
		return (IPDR_ERROR);
	}

	strcpy(tempSchemaURI, SchemaURI);

	/* 
	* getAll specifies that all the mandatory & 
	* optional attributes are to be populated into FNFType 
	*/

	do {
		errorSeverity = parseSchema(
									tempSchemaURI, 
									pDescriptorCommon,
									pErrorCode);

		if (errorSeverity == IPDR_ERROR) {
			free(tempSchemaURI);
			return (IPDR_ERROR);
		}

		appendListNameSpaceInfo(
								pListNameSpaceInfo,
								pDescriptorCommon->nameSpaceId_,
								pDescriptorCommon->targetNameSpace_);

		if (flag == 0) {
			strcpy(pNewFNFType->serviceType_, pDescriptorCommon->pFNFType_->serviceType_);
			flag = 1;
		}

		lengthList = getLengthListAttributeDescriptor
			(pDescriptorCommon->pFNFType_->pListAttributeDescriptor_);

		pCurrentListAttributeDescriptor = pDescriptorCommon->pFNFType_->pListAttributeDescriptor_;

		for (count = 0; count < lengthList; count++) {
			tempAttributeName = (char *) calloc (2 * MAX_ATTRIBUTE_NAME, sizeof(char));

			strcpy(tempAttributeName, pNewFNFType->serviceType_);
			strcat(tempAttributeName, "$");
			strcat(tempAttributeName, pDescriptorCommon->targetNameSpace_);
			strcat(tempAttributeName, "$");
			strcat(tempAttributeName, pDescriptorCommon->pFNFType_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_);

			appendListAttributeDescriptor(
										  &pListAttributeDescriptor, 
										  tempAttributeName,
										  pDescriptorCommon->pFNFType_->pListAttributeDescriptor_->pAttributeDescriptor_->attributeType_,
										  pDescriptorCommon->pFNFType_->pListAttributeDescriptor_->pAttributeDescriptor_->description_,
										  pDescriptorCommon->pFNFType_->pListAttributeDescriptor_->pAttributeDescriptor_->derivedType_,
             pDescriptorCommon->pFNFType_->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_, 
             pDescriptorCommon->pFNFType_->pListAttributeDescriptor_->pAttributeDescriptor_->pListAttrNameList_, 
             pDescriptorCommon->pFNFType_->pListAttributeDescriptor_->pAttributeDescriptor_->pListAttrClassList_,
             pDescriptorCommon->pFNFType_->pListAttributeDescriptor_->pAttributeDescriptor_->pListAttrMaxOccursList_,
             pDescriptorCommon->pFNFType_->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_
										  );

			pDescriptorCommon->pFNFType_->pListAttributeDescriptor_ = pDescriptorCommon->pFNFType_->pListAttributeDescriptor_->pNext_;

			free(tempAttributeName);
		}

if(strcmp(gVersion, IPDR_VERSION_3_1) == 0) {

		if (pNewFNFType->pListAttributeDescriptor_== NULL) {
			pNewFNFType->pListAttributeDescriptor_ = pListAttributeDescriptor;
		} else {
			appendListListAttributeDescriptor(
				&pNewFNFType->pListAttributeDescriptor_,
				&pListAttributeDescriptor);
		}
} else if(strcmp(gVersion, IPDR_VERSION_3_5) == 0) {

		if (pTempFNFType->pListAttributeDescriptor_== NULL) {
			pTempFNFType->pListAttributeDescriptor_ = pListAttributeDescriptor;
		} else {
			appendListListAttributeDescriptor(
				&pTempFNFType->pListAttributeDescriptor_,
				&pListAttributeDescriptor);
		}
}
		pListAttributeDescriptor = NULL;

		if (strcmp(pDescriptorCommon->extensionBase_, IPDR_FINAL_SCHEMA) != 0) {
			strcpy(tempSchemaURI, pDescriptorCommon->schemaLocation_);
		}

	} 

	
	while (strcmp(pDescriptorCommon->extensionBase_, IPDR_FINAL_SCHEMA) != 0);


	pDescriptorCommon->pFNFType_->pListAttributeDescriptor_ = 	pCurrentListAttributeDescriptor;

if(strcmp(gVersion, IPDR_VERSION_3_5) == 0) {
	errorSeverity = getSchemaDescriptorForVersion(OptionalAttributes, pTempFNFType->pListAttributeDescriptor_, pNewFNFType);
	if (errorSeverity == IPDR_ERROR) {
		free(tempSchemaURI);
		freeFNFType(pNewFNFType);
		freeDescriptorCommon(pDescriptorCommon);
		return (IPDR_ERROR);
	}
}

	free(tempSchemaURI);
	freeDescriptorCommon(pDescriptorCommon);
	freeFNFType(pTempFNFType);

	return (IPDR_OK);
 
}
 

int getFileName(char* URI, char* name)
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
