/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/


/*****************************************************
 * File                 : IPDRXMLDocEndHelper        *
 * Description          :                            *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 * 24-Feb-02                                         *
 *****************************************************/

#include "xml/IPDRXMLDocEndHelper.h"

/*!
 *  NAME:
 *      readIPDRXMLDocEndHelper() - This function will return DocEnd 
 *									structure.
 *
 *  DESCRIPTION:
 *      This function
 *		 - Reads count
 *		 - Reads endTime
 *		 - Populates the DocEnd structure with the above read values
 *  
 *  ARGUMENTS:
 *       -	IPDRCommonParameters (IN/OUT)
 *		 -  DocEnd (IN/OUT)
 *		 -	errorCode (IN/OUT)
 *
 *  RETURNS 	
 *	     Returns Zero or appropriate error code
 */

int readIPDRXMLDocEndHelper(
						 IPDRCommonParameters* pIPDRCommonParameters, 
						 DocEnd* pDocEnd, 
						 int* pErrorCode
						)	
{	
	int errorSeverity = 0;

	if(pDocEnd == NULL || pIPDRCommonParameters == NULL) {
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		return(IPDR_ERROR);
	}
	
	pDocEnd->count_ = pIPDRCommonParameters->pDocEnd_->count_;
	pDocEnd->endTime_ = pIPDRCommonParameters->pDocEnd_->endTime_;

	return (IPDR_OK);	
}



/*!
 *  NAME:
 *      writeIPDRXMLDocEndHelper() - takes DocEnd structure as input and 
 *                              writes its XML equivalent on output stream.
 *
 *  DESCRIPTION:
 *       - This function will be used to write an IPDRDocEnd structure to the output 
 *				stream.
 *	     - This function will internally call the basic write functions (eg 
 *         write_xml()) to actually marshall the data in the XML format to the 
 *		   output stream.
 *  
 *  ARGUMENTS:
 *       - IPDRCommonParameters (IN/OUT)
 *		 - DocEnd (IN/OUT)
 *		 - errorCode (IN/OUT)
 *
 *  RETURNS 	
 *	      Returns Zero or appropriate error code
 */

 	
int writeIPDRXMLDocEndHelper(
						  IPDRCommonParameters* pIPDRCommonParameters, 
						  DocEnd* pDocEnd, 
						  int* pErrorCode
						 )	
{	
	char *tempXMLString = NULL;
	char *stringEndTime = NULL;
	int errorSeverity = 0;

	if(pDocEnd == NULL || pIPDRCommonParameters == NULL) {
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		return(IPDR_ERROR);
	}

	tempXMLString = (char*) calloc(sizeof(char), MAX_XML_STRING_LENGTH);
	stringEndTime = (char*) calloc(sizeof(char), MAX_XML_STRING_LENGTH);

	if(tempXMLString == NULL || stringEndTime == NULL) { 
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		return(IPDR_ERROR);
	}
	
	sprintf(tempXMLString, "<%s\n", "IPDRDoc.End");
	write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);

	sprintf(tempXMLString, "%s\"%d\"", "count=",(int)pDocEnd->count_);
	write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);

	errorSeverity = convLongToTimeHeader(&pDocEnd->endTime_, stringEndTime, pErrorCode);
	if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, "convLongToTime"); 
	}
	else if (errorSeverity == IPDR_ERROR) {
		pIPDRCommonParameters->DocState_ = NONWRITABLE;
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXMLDocEndHelper"); 
		free(tempXMLString);
		free(stringEndTime);
		return (IPDR_ERROR);
	}

	sprintf(tempXMLString, "%s\"%s\"%s", " endTime=", stringEndTime, ">");
	write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);

	sprintf(tempXMLString, "</%s>\n", "IPDRDoc.End");
	write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);

	/* XML Document Closing Tag </IPDRDoc> */
	sprintf(tempXMLString, "%s\n", "</IPDRDoc>");
	write_xml(tempXMLString, pIPDRCommonParameters->pStreamHandle_);
	
	free(tempXMLString);
	free(stringEndTime);

	return (IPDR_OK);	
}

