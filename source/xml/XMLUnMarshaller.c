/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
D
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/


/******************************************************
 * File                 : XMLUnMarshaller.c           *
 * Description          : This file contains the XML  *
 *                        parsing functions needed for*
 *                        XMLUnMarshalling            *
 * Author               : Infosys Tech Ltd            *
 * Modification History :                             *
 *--------------------------------------------------- *
 * Date       Name        Change/Description          *
 *--------------------------------------------------- *
 * 24-Feb-02  Infosys					              *
 ******************************************************/

#include <stdio.h>
#include "expat.h"
#include <string.h>
#include <stdlib.h>
#include "xml/XMLUnMarshaller.h"

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
 *       -	data {IN}
 *       -  name {IN}
 *       -  attr {IN}
 *
 *  RETURNS
 *	     Returns void
 *
 */

static void start(void *data, const char *name, const char **attr)
{
  int count1 = 0;
  int index = 0;
  int errorCode = 0;
  unsigned int count = 0;
  unsigned int lenDocId = 0;
  unsigned int lenIPDRRecorderInfo = 0;
  char nameSpaceId[MAX_NAMESPACE_URI] = "";
  char tmpNameSpaceID[MAX_NAMESPACE_URI] = "";


  XMLUnMarshallerCommon* pXMLUnMarshallerCommon = NULL;

  pXMLUnMarshallerCommon = (XMLUnMarshallerCommon*) data;

  sprintf(pXMLUnMarshallerCommon->OldattributeName_, "%s", pXMLUnMarshallerCommon->attributeName_);
  sprintf(pXMLUnMarshallerCommon->attributeName_, "%s", name);

	if((pXMLUnMarshallerCommon->flagValue_ == IPDR_FALSE) && (pXMLUnMarshallerCommon->StructureFlagValue_ == 0)) {
		pXMLUnMarshallerCommon->StructureFlagValue_ = 1;
  		pXMLUnMarshallerCommon->flagValue_ = IPDR_TRUE;
  		pXMLUnMarshallerCommon->EndFlagValue_ = IPDR_FALSE;
	}
	else if((pXMLUnMarshallerCommon->flagValue_ == IPDR_TRUE) && (pXMLUnMarshallerCommon->StructureFlagValue_ == 1)
				&& (pXMLUnMarshallerCommon->ComplexFlagValue_ == 0)){
  		if((strcmp(name, "IPDRDoc")) && (strcmp(name, "IPDR")) && 
			(strcmp(pXMLUnMarshallerCommon->attributeName_, IPDR_XML_SEQ_NUM))  &&
			(strcmp(pXMLUnMarshallerCommon->attributeName_, "IPDRCreationTime"))) {
  			sprintf(pXMLUnMarshallerCommon->OldComplexattributeName_, "%s", pXMLUnMarshallerCommon->OldattributeName_);
			pXMLUnMarshallerCommon->ComplexFlagValue_ = 1;
		}
	}
	else if((pXMLUnMarshallerCommon->flagValue_ == IPDR_TRUE) && (pXMLUnMarshallerCommon->StructureFlagValue_ == 1)
				&& (pXMLUnMarshallerCommon->ComplexFlagValue2_ == 0)){
  		if((strcmp(name, "IPDRDoc")) && (strcmp(name, "IPDR")) && 
			(strcmp(pXMLUnMarshallerCommon->attributeName_, IPDR_XML_SEQ_NUM))  &&
			(strcmp(pXMLUnMarshallerCommon->attributeName_, "IPDRCreationTime"))) {
  			sprintf(pXMLUnMarshallerCommon->OldComplexattributeName_, "%s", pXMLUnMarshallerCommon->OldattributeName_);
			pXMLUnMarshallerCommon->ComplexFlagValue2_ = 1;
		}
	}

	if(!pXMLUnMarshallerCommon->IPDRDocEnd_)
		pXMLUnMarshallerCommon->IPDRDocEnd_ = (!strcmp(name, "IPDRDoc.End"));

  for (index = 0; attr[index]; index += 2) {

	if(!strcmp(attr[index], "xmlns")) {
		sprintf(pXMLUnMarshallerCommon->pGIPDRHeader_->defaultNameSpaceURI_, "%s", attr[index + 1]);
	}
	if(!strcmp(attr[index], "IPDRRecorderInfo")) {
		lenIPDRRecorderInfo = strlen(attr[index + 1]) + 1;
		pXMLUnMarshallerCommon->pGIPDRHeader_->ipdrRecorderInfo_ = (char *) calloc (lenIPDRRecorderInfo, sizeof(char));
		sprintf(pXMLUnMarshallerCommon->pGIPDRHeader_->ipdrRecorderInfo_, "%s", attr[index + 1]);
	}

	if(!strcmp(attr[index], "docId")) {
		lenDocId = strlen(attr[index + 1]) + 1;
		pXMLUnMarshallerCommon->pGIPDRHeader_->docID_ = (char *) calloc (lenDocId, sizeof(char));
		sprintf(pXMLUnMarshallerCommon->pGIPDRHeader_->docID_, "%s", attr[index + 1]);
	}

	if(!strcmp(attr[index], "creationTime"))
		convTimeToLong(
                      (char *) attr[index + 1], 
                      &pXMLUnMarshallerCommon->pGIPDRHeader_->startTime_, &errorCode);

	if(!strcmp(attr[index], "version")) {
		if(gVersion[0] != '\0') {
			if(strcmp(attr[index + 1], IPDR_VERSION_3_0) == 0) {
				printf("The version in the xml file is %s\n", attr[index + 1]);
			}
			else if(strcmp(gVersion, attr[index + 1]) != 0) {
				printf("The version in the xml file is %s\n", attr[index + 1]);
				exit(-1);
			}
		} else {
			if(strcmp(attr[index + 1],IPDR_VERSION_3_1) == 0) {
				strcpy(gVersion, IPDR_VERSION_3_1);
			} else if(strcmp(attr[index + 1],IPDR_VERSION_3_5) == 0) {
				strcpy(gVersion, IPDR_VERSION_3_5);
			}
		}
		pXMLUnMarshallerCommon->pGIPDRHeader_->majorVersion_ = atoi(attr[index + 1]);
		pXMLUnMarshallerCommon->pGIPDRHeader_->minorVersion_ = 0;
	}

	if(!strcmp(attr[index], "xsi:type")) {
		sprintf(pXMLUnMarshallerCommon->serviceType_, "%s", attr[index + 1]);
	}

	if(isNameSpaceInfo((char *) attr[index])) {
		if(strcmp(attr[index], "xmlns:xsi")) {

			nameSpaceId[0] = '\0';
			for(count = 6, count1= 0; count < strlen(attr[index]); count++, count1++)
				nameSpaceId[count1] = attr[index][count];
			appendListNameSpaceInfo(
				&pXMLUnMarshallerCommon->pGIPDRHeader_->pOtherNameSpaceInfo_, 
						nameSpaceId, 
						(char *) attr[index+1]);
			    
				for(count = 0; count < MAX_NAMESPACE_URI; count++){
					nameSpaceId[count] = '\0';
				}
				
		}
	}

	if(!strcmp(attr[index], "xsi:schemaLocation")) {
		sprintf(pXMLUnMarshallerCommon->schemaLocation_, "%s", attr[index + 1]);
										   
	}

	if(!strcmp(attr[index], "xsi:noNamespaceSchemaLocation")) {
		sprintf(pXMLUnMarshallerCommon->schemaLocation_, "%s", attr[index + 1]);
	
	    pXMLUnMarshallerCommon->token_ = strtok(pXMLUnMarshallerCommon->schemaLocation_," ");

	    for(count=1; pXMLUnMarshallerCommon->token_ != NULL; count++ )  {
      
           appendListServiceDefinitionURI(&pXMLUnMarshallerCommon->pGIPDRHeader_->pListServiceDefinitionURI_,
			                            pXMLUnMarshallerCommon->token_); 
										
		  pXMLUnMarshallerCommon->token_ = strtok( NULL, " ");
	  }
	
		strcpy(pXMLUnMarshallerCommon->schemaLocation_, ""); 
	}
  }

  pXMLUnMarshallerCommon->Depth_++;
}  /* End of start handler */


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
 *       -	data {IN}
 *       -  name {IN}
 *
 *  RETURNS
 *	     Returns void
 *
 */

static void end(void *data, const char *name)
{

  	XMLUnMarshallerCommon* pXMLUnMarshallerCommon = NULL;
	ListIPDRComplexData* pListIPDRComplexData[10];
	ListIPDRData* pCurrentListIPDRData = NULL;
	ListAttributeDescriptor* pCurrent = NULL;
	ListFNFData* pXMLCurrent = NULL;
	static char lName[50];
	static countValue = 0;
	int count = 0;
	int retVal = 1;
	int lComplexFlag = 0;

  	pXMLUnMarshallerCommon = (XMLUnMarshallerCommon*) data;

  	memset(pListIPDRComplexData, 0 ,sizeof(pListIPDRComplexData));

	if((pXMLUnMarshallerCommon->ComplexFlagValue_ == 1) && (pXMLUnMarshallerCommon->EndFlagValue_ == 1)) {
		
		if(pXMLUnMarshallerCommon->ComplexFlagValue2_ == 1) {
			pXMLUnMarshallerCommon->ComplexFlagValue2_ = 0;
			strncpy(lName, (char *)name, strlen(name));
			lComplexFlag = 1;
			appendNode(&pXMLUnMarshallerCommon->pAttrNameList_,(char *)name);
		} else {
			pXMLUnMarshallerCommon->ComplexFlagValue_ = 0;

   		appendListAttributeDescriptor(&pXMLUnMarshallerCommon->pListAttributeDescriptor_,
   											(char *)name,
   											"UTF8_ARRAY",
   											"OPTIONAL",
   											"UTF8_ARRAY",
   											NULL,
												pXMLUnMarshallerCommon->pAttrNameList_,
												NULL,
												NULL,
												0);
			freeList(&(pXMLUnMarshallerCommon->pAttrNameList_));
			pXMLUnMarshallerCommon->pAttrNameList_ = NULL;
		}

			if(lComplexFlag == 1) {
				lComplexFlag = 0;
   			appendListAttributeDescriptor(&pXMLUnMarshallerCommon->pListServiceAttributeDescriptor_,
   											lName,
   											"UTF8_ARRAY",
   											"OPTIONAL",
   											"UTF8_ARRAY",
   											NULL,
												pXMLUnMarshallerCommon->pAttrNameList2_,
												NULL,
												NULL,
												0);
				freeList(&(pXMLUnMarshallerCommon->pAttrNameList2_));
				pXMLUnMarshallerCommon->pAttrNameList2_ = NULL;
		}

			if(pXMLUnMarshallerCommon->pListIPDRComplex_ != NULL) {
				appendListIPDRComplexData(&pListIPDRComplexData[count], pXMLUnMarshallerCommon->pListIPDRComplex_,IPDRDATA_STRUCTURE);
		
				appendListIPDRWithComplexData(&pXMLUnMarshallerCommon->pListIPDRData_, 
												pListIPDRComplexData, 
												IPDRDATA_STRUCTURE);

				pXMLUnMarshallerCommon->pListIPDRComplex_ = NULL;
			}
	}

	if ((pXMLUnMarshallerCommon->StructureFlagValue_ == 1) && (pXMLUnMarshallerCommon->EndFlagValue_ == 0)) {
		pXMLUnMarshallerCommon->StructureFlagValue_ = 0;
		pXMLUnMarshallerCommon->EndFlagValue_ = 1;
	}

  if(!strcmp(name, "IPDR"))	{
	  
	appendListFNFData(
		&pXMLUnMarshallerCommon->pListFNFData_,
		pXMLUnMarshallerCommon->serviceType_,
		pXMLUnMarshallerCommon->pListAttributeDescriptor_,
		pXMLUnMarshallerCommon->pListIPDRData_);

	pCurrent = pXMLUnMarshallerCommon->pListServiceAttributeDescriptor_;
	pXMLCurrent = pXMLUnMarshallerCommon->pListFNFData_;

	while (pXMLUnMarshallerCommon->pListServiceAttributeDescriptor_ != NULL) {
		while(pXMLCurrent->pNext_ != NULL) {
			pXMLCurrent = pXMLCurrent->pNext_;
		}
		
		appendListAttributeDescriptor(&(pXMLCurrent->pXMLFNFData_->pListServiceAttributeDescriptor_),
										pXMLUnMarshallerCommon->pListServiceAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
										pXMLUnMarshallerCommon->pListServiceAttributeDescriptor_->pAttributeDescriptor_->attributeType_,
										pXMLUnMarshallerCommon->pListServiceAttributeDescriptor_->pAttributeDescriptor_->description_,
										pXMLUnMarshallerCommon->pListServiceAttributeDescriptor_->pAttributeDescriptor_->derivedType_, 
										pXMLUnMarshallerCommon->pListServiceAttributeDescriptor_->pAttributeDescriptor_->complexType_, 
										pXMLUnMarshallerCommon->pListServiceAttributeDescriptor_->pAttributeDescriptor_->pListAttrNameList_,
										pXMLUnMarshallerCommon->pListServiceAttributeDescriptor_->pAttributeDescriptor_->pListAttrClassList_, 
										pXMLUnMarshallerCommon->pListServiceAttributeDescriptor_->pAttributeDescriptor_->pListAttrMaxOccursList_, 
										pXMLUnMarshallerCommon->pListServiceAttributeDescriptor_->pAttributeDescriptor_->noOfElements_); 
	
		pXMLUnMarshallerCommon->pListServiceAttributeDescriptor_ = pXMLUnMarshallerCommon->pListServiceAttributeDescriptor_->pNext_; 

	}
	
	pXMLUnMarshallerCommon->pListServiceAttributeDescriptor_ = pCurrent;
	

	freeListIPDRData(&(pXMLUnMarshallerCommon->pListIPDRData_));
	pXMLUnMarshallerCommon->pListIPDRData_ = NULL;

	freeListAttributeDescriptor(&(pXMLUnMarshallerCommon->pListAttributeDescriptor_));
	pXMLUnMarshallerCommon->pListAttributeDescriptor_ = NULL;

	freeListAttributeDescriptor(&(pXMLUnMarshallerCommon->pListServiceAttributeDescriptor_));
	pXMLUnMarshallerCommon->pListServiceAttributeDescriptor_ = NULL;

	}

    pXMLUnMarshallerCommon->Depth_--;
}  /* End of end handler */



/*!
 *  NAME:
 *      attributeValueHandler() - This function is called as any character
 *                    handler for an element is encountered..
 *
 *  DESCRIPTION:
 *		   This function populates ListAttributeDescriptor & ListIPDRData
 *		   with the values read for Attribute Name, Attribute Type &
 *		   Attribute description (Optional or Mandatory)
 *
 *  ARGUMENTS:
 *       -	data {IN}
 *       -  attributeValue {IN}
 *		 -  len {IN}
 *
 *  RETURNS
 *	     Returns void
 *
 */

static void attributeValueHandler(void *data, const char *attributeValue, int len)
{
	char *buffer = NULL;
	int errorCode = 0;
	int recCount = 0;
	int count = 0;
	int lenBuffer = 0;
	int i;
   int j;
	int lspaceFound = 0;
	char* tmpXMLChar = NULL;
	char* tmpXMLString = NULL;
	char attributeValue1[MAX_IPDR_STRING];
	char lTempAttributeName[MAX_IPDR_STRING];
	void* dataValue[MAX_IPDR_STRING];
	XMLUnMarshallerCommon* pXMLUnMarshallerCommon = NULL;
	ListIPDRComplex* pCurrentListIPDRComplex = NULL;


	memset(dataValue, 0 ,sizeof(dataValue));


	lenBuffer = len;

	/*Remove blanks before and after the data values in attributeValue*/
/*
	if(len > 1) {
	i=0;
	j=0;
	while ( attributeValue[i] != '\0' )
	{
		if ( !isspace(attributeValue[i]) )
		{
			attributeValue1[j]=attributeValue[i];
			j++;
		}
		i++;
	}
	attributeValue1[j]='\0';

	attributeValue = attributeValue1;
	}
*/

if(len > 1) {
	while (isspace(*attributeValue)) {
     ++attributeValue;
	  lspaceFound = 1;
	  len = len - 1;
	}
}




	if ((len >= 1) && (attributeValue[0] != '\n') && (attributeValue[0] != ' ') && (attributeValue[0] != '\t')) { 
/*	if ((len >= 1) && (attributeValue[0] != '\n')  && (attributeValue[0] != '\t')) */
	
	buffer = (char*) calloc(len + 10 , sizeof(char));

	pXMLUnMarshallerCommon = (XMLUnMarshallerCommon*) data;
	tmpXMLChar = (char *) calloc (10, sizeof(char));

	/*if(dataValue[0] == NULL)
		dataValue[0] = (void *) calloc (1, strlen(pXMLUnMarshallerCommon->attributeValue_) + 1);*/

	if (attributeValue[len] == 38) {
		for (count = 0;;count++) {
		if (attributeValue[len] == 59) break;
			tmpXMLChar[count] = attributeValue[len];
			len++;
		}
		tmpXMLChar[count] = 59;
		tmpXMLChar[count + 1] = 0;
	
		len++;
	}

	strncpy(buffer, attributeValue, len);
	buffer[len] = 0;

	if (pXMLUnMarshallerCommon->flagValue_ == IPDR_TRUE) {
		sprintf(pXMLUnMarshallerCommon->attributeValue_, "%s", buffer);
		if (getXMLChar(tmpXMLChar) != 0) {
			tmpXMLString = (char *) calloc (2, sizeof(char));
			tmpXMLString[0] = getXMLChar(tmpXMLChar);
			tmpXMLString[1] = 0;
			strcat(pXMLUnMarshallerCommon->attributeValue_, tmpXMLString);
			free(tmpXMLString);
		}
	}
	else {
		strcat(pXMLUnMarshallerCommon->attributeValue_, buffer);
		
		recCount = getLengthListAttributeDescriptor(
			pXMLUnMarshallerCommon->pListAttributeDescriptor_);

		freeListNodeAttributeDescriptor(
			&(pXMLUnMarshallerCommon->pListAttributeDescriptor_), recCount);
		pXMLUnMarshallerCommon->pListAttributeDescriptor_ = NULL;
	
		recCount = getLengthListIPDRData(
			pXMLUnMarshallerCommon->pListIPDRData_);

		freeListNodeIPDRData(
			&(pXMLUnMarshallerCommon->pListIPDRData_), recCount);
	        pXMLUnMarshallerCommon->pListAttributeDescriptor_ = NULL;

		strcpy(pXMLUnMarshallerCommon->attributeName_,"");
	}
	pXMLUnMarshallerCommon->flagValue_ = IPDR_FALSE;
	
	if (strcmp(buffer, "\n")  && strcmp(pXMLUnMarshallerCommon->serviceType_, "") &&
		strcmp(pXMLUnMarshallerCommon->attributeValue_, "") && (!pXMLUnMarshallerCommon->IPDRDocEnd_) &&
		(strcmp(pXMLUnMarshallerCommon->attributeName_, IPDR_XML_SEQ_NUM))  &&
		(strcmp(pXMLUnMarshallerCommon->attributeName_, "IPDRCreationTime")) &&
		strcmp(buffer, "\t") && strcmp(pXMLUnMarshallerCommon->attributeName_, "")) {

		if(lspaceFound == 1) {
			/*strcat(lTempAttributeName, " ");
			strcat(lTempAttributeName,pXMLUnMarshallerCommon->attributeName_);*/

			sprintf(lTempAttributeName, " %s", pXMLUnMarshallerCommon->attributeValue_);	
			strcpy(pXMLUnMarshallerCommon->attributeValue_,lTempAttributeName);
			lspaceFound = 0;	
		}

   	if((pXMLUnMarshallerCommon->ComplexFlagValue_ == 1)) {
			if((pXMLUnMarshallerCommon->ComplexFlagValue2_ == 1)) {
					appendNode(&pXMLUnMarshallerCommon->pAttrNameList2_,pXMLUnMarshallerCommon->attributeName_);
			} else {
					appendNode(&pXMLUnMarshallerCommon->pAttrNameList_,pXMLUnMarshallerCommon->attributeName_);
			}

			if(dataValue[0] == NULL)
				dataValue[0] = (void *) calloc (1, strlen(pXMLUnMarshallerCommon->attributeValue_) + 1);	

			strcpy((char *)dataValue[0],pXMLUnMarshallerCommon->attributeValue_);

			if(strcmp(pXMLUnMarshallerCommon->attributeName_,pXMLUnMarshallerCommon->OldattributeName_) == 0) {
				pCurrentListIPDRComplex = pXMLUnMarshallerCommon->pListIPDRComplex_;
				while(pXMLUnMarshallerCommon->pListIPDRComplex_->pNext_ != NULL) {
					pXMLUnMarshallerCommon->pListIPDRComplex_ = 
						pXMLUnMarshallerCommon->pListIPDRComplex_->pNext_;
				}
				appendListIPDRArrayData(&pXMLUnMarshallerCommon->pListIPDRComplex_->pListIPDRArrayData_, pXMLUnMarshallerCommon->attributeValue_, IPDRDATA_STRING);
				pXMLUnMarshallerCommon->pListIPDRComplex_->noOfElements_ =
                       pXMLUnMarshallerCommon->pListIPDRComplex_->noOfElements_ + 1;
				pXMLUnMarshallerCommon->pListIPDRComplex_ = pCurrentListIPDRComplex;
			}
			else {
				appendListIPDRComplex(&pXMLUnMarshallerCommon->pListIPDRComplex_, dataValue, IPDRDATA_STRING);
			}
			free(dataValue[0]);
			dataValue[0] = NULL;
   	}
   	else {
   		appendListAttributeDescriptor(&pXMLUnMarshallerCommon->pListAttributeDescriptor_,
   													pXMLUnMarshallerCommon->attributeName_,
   													"UTF8_ARRAY",
   													"OPTIONAL",
   													"UTF8_ARRAY",
   													NULL,NULL,NULL,NULL,0);
   
   		appendListIPDRData( &pXMLUnMarshallerCommon->pListIPDRData_, pXMLUnMarshallerCommon->attributeValue_,
   									IPDRDATA_STRING);
   	}
	}

	if(pXMLUnMarshallerCommon->IPDRDocEnd_)	{
		if(!strcmp(pXMLUnMarshallerCommon->attributeName_,"count") && (pXMLUnMarshallerCommon->pGDocEnd_->count_ == 0))
			pXMLUnMarshallerCommon->pGDocEnd_->count_ = atoi(pXMLUnMarshallerCommon->attributeValue_);
		if(!strcmp(pXMLUnMarshallerCommon->attributeName_,"endTime") && (pXMLUnMarshallerCommon->pGDocEnd_->endTime_ == 0))
			convTimeToLong(pXMLUnMarshallerCommon->attributeValue_, &pXMLUnMarshallerCommon->pGDocEnd_->endTime_, &errorCode);
	}
	free(buffer);
	free(tmpXMLChar);
}
}


/*!
 *  NAME:
 *      readXMLStreamData() - 	This function reads the IPDR XML Doc 
 *
 *  DESCRIPTION:
 *     	This function
 *          -  Reads & Validates the Version. It should not be off
 *			-  Reads ipdrRecoderInfo
 *			-  Reads startTime
 *			-  Reads defaultNameSpaceURI
 *			-  Reads the Length for otherNameSpaces
 *			-  Reads otherNameSpaces
 *			-  Reads the Length for serviceDefinitionURI
 *			-  Reads serviceDefinitionURI
 *			-  Reads the length for docId
 *			-  Reads docId
 *			-  Populates the IPDRHeader structure with the above read values.
 *			-  Populates ListFNFData
 *			-  Populates Doc End Structure.
 *
 *	ARGUMENTS:
 *        -  IPDRCommonParameters (IN/OUT)
 *	      -  IPDRHeader (IN/OUT)
 *		  -	 pErrorCode	(IN/OUT)
 *
 *  RETURNS
 *	      Returns Zero or appropriate error code
*/
int readXMLStreamData(
			   IPDRCommonParameters *pIPDRCommonParameters,
			   IPDRHeader *pIPDRHeader,
			   int* pErrorCode
			  )
{

  FILE *fp;
  char tmpNameSpaceID[MAX_NAMESPACE_URI] = "";
  int count = 0;
  int tempLength = 0;
  int errorSeverity = 0;
  unsigned int lenDocId = 0;
  unsigned int lenIPDRRecorderInfo = 0;
  unsigned int xmlFileLength = 0;
  ListServiceDefinitionURI* pCurrentListServiceDefURI = NULL;
  ListFNFData* pCurrentListFNFData = NULL;
  ListFNFData* pIPDRCommonParametersCurrent = NULL;
  ListNameSpaceInfo* pCurrentNameSpaceInfo = NULL;
  ListAttributeDescriptor* pCurrent = NULL;
  XML_Parser xmlParser = XML_ParserCreate(NULL);
  XMLUnMarshallerCommon* pXMLUnMarshallerCommon = NULL;

  void* userData = NULL;

	/*Added this line to populate the gVersion from IPDRCommonParameters*/
   strcpy(gVersion, pIPDRCommonParameters->schemaVersion);

  pXMLUnMarshallerCommon = (XMLUnMarshallerCommon *) malloc (sizeof(XMLUnMarshallerCommon));

  initXMLUnMarshallerCommon(pIPDRCommonParameters, pXMLUnMarshallerCommon);
  xmlFileLength = getFileLength(pIPDRCommonParameters->pStreamName_);

  if (! xmlParser) {
	*pErrorCode = IPDR_XML_PARSER_MEMORY_ALLOC_FAILED;
    return (IPDR_ERROR);
  }

  XML_SetElementHandler(xmlParser, start, end);

  XML_SetCharacterDataHandler(xmlParser, attributeValueHandler);

  fp = pIPDRCommonParameters->pStreamHandle_;

  if (fp == NULL) {
	  *pErrorCode = IPDR_XML_INPUT_FILE_NULL;
	  return (IPDR_ERROR);
  }

 userData = (XMLUnMarshallerCommon *) pXMLUnMarshallerCommon;

 XML_SetUserData(xmlParser, userData);


  for (;;) {
    int done;
    int len;

	len = fread(pXMLUnMarshallerCommon->Buff_, 1, xmlFileLength, fp);

    if (ferror(fp)) {
	 *pErrorCode = IPDR_XML_INPUT_FILE_INVALID;
      return (IPDR_ERROR);
    }

    done = feof(fp);

    if (!XML_Parse(xmlParser, pXMLUnMarshallerCommon->Buff_, len, done)) {
	 /*
	 *pErrorCode = IPDR_XML_PARSER_READ_ERROR;
	 */
	 *pErrorCode = XML_GetErrorCode(xmlParser) + EXPAT_ERROR_CODE;
		
      return (IPDR_ERROR);
    }

    if (done)
      break;
  }
  if(strcmp(pXMLUnMarshallerCommon->schemaLocation_, "") != 0) {	
	  pXMLUnMarshallerCommon->token_ = strtok(pXMLUnMarshallerCommon->schemaLocation_," ");

	  for(count=1; pXMLUnMarshallerCommon->token_ != NULL; count++ )  {
      
		  if ( (count % 2) == 0)  {
  			  appendListServiceDefinitionURI(&pXMLUnMarshallerCommon->pGIPDRHeader_->pListServiceDefinitionURI_,
									pXMLUnMarshallerCommon->token_);
		  }  else {
			  appendListSchemaNameSpace(&pIPDRHeader->pListSchemaNameSpace_,
										 pXMLUnMarshallerCommon->token_);
		  }
		  pXMLUnMarshallerCommon->token_ = strtok( NULL, " ");
	  }
  }
 if (pXMLUnMarshallerCommon->pGIPDRHeader_->pOtherNameSpaceInfo_ != NULL) 	{
	  pCurrentNameSpaceInfo = pXMLUnMarshallerCommon->pGIPDRHeader_->pOtherNameSpaceInfo_;
	  while(pXMLUnMarshallerCommon->pGIPDRHeader_->pOtherNameSpaceInfo_ != NULL) {      
		  appendListNameSpaceInfo(&pIPDRHeader->pOtherNameSpaceInfo_,
			pXMLUnMarshallerCommon->pGIPDRHeader_->pOtherNameSpaceInfo_->NameSpaceInfo_.nameSpaceID_,
			pXMLUnMarshallerCommon->pGIPDRHeader_->pOtherNameSpaceInfo_->NameSpaceInfo_.nameSpaceURI_);	
		  
		  pXMLUnMarshallerCommon->pGIPDRHeader_->pOtherNameSpaceInfo_ = pXMLUnMarshallerCommon->pGIPDRHeader_->pOtherNameSpaceInfo_->pNext_; 
	  }
	  pXMLUnMarshallerCommon->pGIPDRHeader_->pOtherNameSpaceInfo_ = pCurrentNameSpaceInfo;	  		  
  } else {
	  appendListNameSpaceInfo(&pIPDRHeader->pOtherNameSpaceInfo_,
		                      "", pXMLUnMarshallerCommon->pGIPDRHeader_->defaultNameSpaceURI_); 
  }

  pIPDRHeader->majorVersion_ = pXMLUnMarshallerCommon->pGIPDRHeader_->majorVersion_;
  pIPDRHeader->minorVersion_ = pXMLUnMarshallerCommon->pGIPDRHeader_->minorVersion_;
  pIPDRHeader->startTime_ = pXMLUnMarshallerCommon->pGIPDRHeader_->startTime_;

  lenIPDRRecorderInfo = strlen(pXMLUnMarshallerCommon->pGIPDRHeader_->ipdrRecorderInfo_) + 1;
	pIPDRHeader->ipdrRecorderInfo_ = (char *) calloc (lenIPDRRecorderInfo, sizeof(char));
	if(pIPDRHeader->ipdrRecorderInfo_ == NULL) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		return(IPDR_ERROR);
	}

  strcpy(pIPDRHeader->ipdrRecorderInfo_, pXMLUnMarshallerCommon->pGIPDRHeader_->ipdrRecorderInfo_);
  free(pXMLUnMarshallerCommon->pGIPDRHeader_->ipdrRecorderInfo_);

  lenDocId = strlen(pXMLUnMarshallerCommon->pGIPDRHeader_->docID_) + 1;
  pIPDRHeader->docID_ = (char *) calloc (lenDocId, sizeof(char));
  if(pIPDRHeader->docID_ == NULL) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		return(IPDR_ERROR);
  }
  if(pXMLUnMarshallerCommon->pGIPDRHeader_->docID_ == NULL) {
		*pErrorCode = IPDR_XML_NULL_DOCID;
		return (IPDR_ERROR);
  }
  strcpy(pIPDRHeader->docID_, pXMLUnMarshallerCommon->pGIPDRHeader_->docID_);

  free(pXMLUnMarshallerCommon->pGIPDRHeader_->docID_);

  strcpy(pIPDRHeader->defaultNameSpaceURI_, pXMLUnMarshallerCommon->pGIPDRHeader_->defaultNameSpaceURI_);

  pCurrentListServiceDefURI = pXMLUnMarshallerCommon->pGIPDRHeader_->pListServiceDefinitionURI_;
  while(pXMLUnMarshallerCommon->pGIPDRHeader_->pListServiceDefinitionURI_ != NULL) {	
	  appendListServiceDefinitionURI(&pIPDRHeader->pListServiceDefinitionURI_,
			pXMLUnMarshallerCommon->pGIPDRHeader_->pListServiceDefinitionURI_->pServiceDefintionURI_);
			
	  pXMLUnMarshallerCommon->pGIPDRHeader_->pListServiceDefinitionURI_ =
			pXMLUnMarshallerCommon->pGIPDRHeader_->pListServiceDefinitionURI_->pNext_;
  }
  pXMLUnMarshallerCommon->pGIPDRHeader_->pListServiceDefinitionURI_= pCurrentListServiceDefURI;

 /* Populating List FNFData*/
  
  pCurrentListFNFData= pXMLUnMarshallerCommon->pListFNFData_;
  while(pXMLUnMarshallerCommon->pListFNFData_ != NULL) {
	  	appendListFNFData(&pIPDRCommonParameters->pListFNFData_,
		                 pXMLUnMarshallerCommon->pListFNFData_->pXMLFNFData_->typeName_,
						 pXMLUnMarshallerCommon->pListFNFData_->pXMLFNFData_->pListAttributeDescriptor_,
						 pXMLUnMarshallerCommon->pListFNFData_->pXMLFNFData_->pListIPDRData_
					);

		pCurrent = pXMLUnMarshallerCommon->pListFNFData_->pXMLFNFData_->pListServiceAttributeDescriptor_;
		pIPDRCommonParametersCurrent = pIPDRCommonParameters->pListFNFData_;
		while(pXMLUnMarshallerCommon->pListFNFData_->pXMLFNFData_->pListServiceAttributeDescriptor_ != NULL) {
			while(pIPDRCommonParameters->pListFNFData_->pNext_ != NULL) {
				pIPDRCommonParameters->pListFNFData_ = pIPDRCommonParameters->pListFNFData_->pNext_;
			}

			appendListAttributeDescriptor(&(pIPDRCommonParameters->pListFNFData_->pXMLFNFData_->pListServiceAttributeDescriptor_),
										pXMLUnMarshallerCommon->pListFNFData_->pXMLFNFData_->pListServiceAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
										pXMLUnMarshallerCommon->pListFNFData_->pXMLFNFData_->pListServiceAttributeDescriptor_->pAttributeDescriptor_->attributeType_,
										pXMLUnMarshallerCommon->pListFNFData_->pXMLFNFData_->pListServiceAttributeDescriptor_->pAttributeDescriptor_->description_,
										pXMLUnMarshallerCommon->pListFNFData_->pXMLFNFData_->pListServiceAttributeDescriptor_->pAttributeDescriptor_->derivedType_, 
										pXMLUnMarshallerCommon->pListFNFData_->pXMLFNFData_->pListServiceAttributeDescriptor_->pAttributeDescriptor_->complexType_, 
										pXMLUnMarshallerCommon->pListFNFData_->pXMLFNFData_->pListServiceAttributeDescriptor_->pAttributeDescriptor_->pListAttrNameList_,
										pXMLUnMarshallerCommon->pListFNFData_->pXMLFNFData_->pListServiceAttributeDescriptor_->pAttributeDescriptor_->pListAttrClassList_, 
										pXMLUnMarshallerCommon->pListFNFData_->pXMLFNFData_->pListServiceAttributeDescriptor_->pAttributeDescriptor_->pListAttrMaxOccursList_, 
										pXMLUnMarshallerCommon->pListFNFData_->pXMLFNFData_->pListServiceAttributeDescriptor_->pAttributeDescriptor_->noOfElements_); 
	
		pXMLUnMarshallerCommon->pListFNFData_->pXMLFNFData_->pListServiceAttributeDescriptor_ = pXMLUnMarshallerCommon->pListFNFData_->pXMLFNFData_->pListServiceAttributeDescriptor_->pNext_; 

		}
		pXMLUnMarshallerCommon->pListFNFData_->pXMLFNFData_->pListServiceAttributeDescriptor_ = pCurrent;
		freeHeadListFNFData(&pXMLUnMarshallerCommon->pListFNFData_);
  }
  pXMLUnMarshallerCommon->pListFNFData_ = pCurrentListFNFData;
  pIPDRCommonParameters->pListFNFData_ = pIPDRCommonParametersCurrent;

  	/* For Doc End */	
  pIPDRCommonParameters->pDocEnd_->count_  = pXMLUnMarshallerCommon->pGDocEnd_->count_;
  pIPDRCommonParameters->pDocEnd_->endTime_ = pXMLUnMarshallerCommon->pGDocEnd_->endTime_;

  free(pXMLUnMarshallerCommon->pGIPDRHeader_->defaultNameSpaceURI_);
  freeListServiceDefinitionURI(&(pXMLUnMarshallerCommon->pGIPDRHeader_->pListServiceDefinitionURI_));
  freeListNameSpaceInfo(&pXMLUnMarshallerCommon->pGIPDRHeader_->pOtherNameSpaceInfo_);
  free(pXMLUnMarshallerCommon->pGIPDRHeader_);
  freeDocEnd(pXMLUnMarshallerCommon->pGDocEnd_);

  
  freeXMLUnMarshallerCommon(pXMLUnMarshallerCommon);
  
  return (IPDR_OK);

}  


/*!
 *  NAME:
 *      isNameSpaceInfo() - Checks wheather the given XML string
 *							contains Some NameSpace Info.	
 *
 *  DESCRIPTION:
 *		 This function Checks wheather the given XML string
 *		 contains Some NameSpace Info in XML Header.	
 *
 *  ARGUMENTS:
 *       -	XML String { IN }
 *
 *  RETURNS
 *	     Returns One if the given string does contains NameSpace Info
 * 		 else it returns zero.
 */


int isNameSpaceInfo(char *source) 
{
  char tempStr[7];


  strncpy(tempStr, source, 6);
  tempStr[6] = '\0';

  if(!strcmp(tempStr, "xmlns:"))
	  return (1);
  else
	  return (0);
}


/*!
 *  NAME:
 *      initXMLUnMarshallerCommon() - Initializes XMLUnMarshallerCommon
 *      structure
 *
 *  DESCRIPTION:
 *		 This function initializes the XMLUnMarshallerCommon
 *
 *
 *  ARGUMENTS:
 *       -	XMLUnMarshallerCommon {IN/OUT}
 *
 *  RETURNS
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int initXMLUnMarshallerCommon(
							  IPDRCommonParameters *pIPDRCommonParameters,
							  XMLUnMarshallerCommon* pXMLUnMarshallerCommon
							 )
{
	unsigned int xmlFileLength = 0;	

if (pXMLUnMarshallerCommon == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}
	xmlFileLength = getFileLength(pIPDRCommonParameters->pStreamName_) + 1;
	pXMLUnMarshallerCommon->Buff_ = (char*) calloc (xmlFileLength, sizeof(char));

	pXMLUnMarshallerCommon->serviceType_ = (char*) calloc (MAX_IPDR_STRING, sizeof(char));
	pXMLUnMarshallerCommon->schemaLocation_ = (char*) calloc (IPDR_XML_MAX_SCHEMA_LOCATION, sizeof(char));
	pXMLUnMarshallerCommon->attributeName_ = (char*) calloc (MAX_ATTRIBUTE_NAME, sizeof(char));
	pXMLUnMarshallerCommon->attributeValue_ =  (char*) calloc (MAX_IPDR_STRING, sizeof(char));
	pXMLUnMarshallerCommon->OldattributeName_ = (char*) calloc (MAX_ATTRIBUTE_NAME, sizeof(char));
	pXMLUnMarshallerCommon->OldComplexattributeName_ = (char*) calloc (MAX_ATTRIBUTE_NAME, sizeof(char));
	pXMLUnMarshallerCommon->Depth_ = 0;
	pXMLUnMarshallerCommon->IPDRDocEnd_ = 0;
	pXMLUnMarshallerCommon->pGIPDRHeader_ = newIPDRHeader();
	pXMLUnMarshallerCommon->pGDocEnd_ = newDocEnd();
	pXMLUnMarshallerCommon->pListAttributeDescriptor_ = NULL;
	pXMLUnMarshallerCommon->pListServiceAttributeDescriptor_ = NULL;
	pXMLUnMarshallerCommon->pListIPDRData_ = NULL;
	pXMLUnMarshallerCommon->pListFNFData_ = NULL;
   pXMLUnMarshallerCommon->pAttrNameList_ = NULL;
   pXMLUnMarshallerCommon->pAttrNameList2_ = NULL;
	pXMLUnMarshallerCommon->pListIPDRComplex_ = NULL;
	pXMLUnMarshallerCommon->flagValue_ = 0;
	pXMLUnMarshallerCommon->StructureFlagValue_ = 0;
	pXMLUnMarshallerCommon->ComplexFlagValue_ = 0;
	pXMLUnMarshallerCommon->ComplexFlagValue2_ = 0;
	pXMLUnMarshallerCommon->EndFlagValue_ = 0;

	return (IPDR_OK);
}

/*!
 *  NAME:
 *      freeXMLUnMarshallerCommon() - memory deallocation of
 *							XMLUnMarshallerCommon structure.
 *
 *  DESCRIPTION:
 *      1. This function performs the memory deallocation of
 *         XMLUnMarshallerCommon structure & its members.
 *
 *  ARGUMENTS:
 *
 *		 -  pXMLUnMarshallerCommon (IN/OUT)
 *
 *  RETURNS
 *	     Returns Zero or appropriate error code
 */

int freeXMLUnMarshallerCommon(
							  XMLUnMarshallerCommon* pXMLUnMarshallerCommon
							  )
{

	if (pXMLUnMarshallerCommon == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	free(pXMLUnMarshallerCommon->Buff_);
		pXMLUnMarshallerCommon->Buff_ = NULL;
    free(pXMLUnMarshallerCommon->serviceType_);
		pXMLUnMarshallerCommon->serviceType_ = NULL;
	
	if (strcmp(pXMLUnMarshallerCommon->schemaLocation_, "" ) != 0) {
		free(pXMLUnMarshallerCommon->schemaLocation_);
		pXMLUnMarshallerCommon->schemaLocation_ = NULL;
	}

	free(pXMLUnMarshallerCommon->attributeName_);
		pXMLUnMarshallerCommon->attributeName_ = NULL;
	free(pXMLUnMarshallerCommon->attributeValue_);
		pXMLUnMarshallerCommon->attributeValue_ = NULL;
	free(pXMLUnMarshallerCommon->OldattributeName_);
		pXMLUnMarshallerCommon->OldattributeName_ = NULL;
	free(pXMLUnMarshallerCommon->OldComplexattributeName_);
		pXMLUnMarshallerCommon->OldComplexattributeName_ = NULL;

	free(pXMLUnMarshallerCommon);
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      getIDListNameSpaceInfo() - Gets NameSpace ID
 *							for given NameSpace URI.
 *
 *  DESCRIPTION:
 *      This function fetches the NameSpaceId from the 
 *      look up for Id & URI.
 *
 *  ARGUMENTS:
 *
 *		 -  pListNameSpaceInfo (IN/OUT)
 *		 -  nameSpaceURI (IN/OUT)
 *		 -  nameSpaceID  (IN/OUT)
 *		 -  pErrorCode   (IN/OUT)
 *
 *  RETURNS
 *	     Returns Zero or appropriate error code
 */


int getIDListNameSpaceInfo(ListNameSpaceInfo* pListNameSpaceInfo,
						   char* nameSpaceURI,
						   char* nameSpaceID,
						   int* pErrorCode)
{
	ListNameSpaceInfo* pCurrent = pListNameSpaceInfo;
	int flag = 0;

	if (pCurrent == NULL) {
		*pErrorCode = IPDR_NULL_LINKLIST;
		return(IPDR_WARNING);
	}
	
	while (pCurrent != NULL) {
		if (strcmp(nameSpaceURI, pCurrent->NameSpaceInfo_.nameSpaceURI_) == 0) {
			strcpy(nameSpaceID, pCurrent->NameSpaceInfo_.nameSpaceID_);
			flag = 1;
			break;
		}		
		pCurrent = pCurrent->pNext_;		    
	}

	if (flag == 0)
			strcpy(nameSpaceID, " ");
	
	return (IPDR_OK);
}


int getXMLChar(char* tmpXMLChar)
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
