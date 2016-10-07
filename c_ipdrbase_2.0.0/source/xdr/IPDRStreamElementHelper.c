/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/




/*****************************************************
 * File                 : IPDRStreamElementHelper    *
 * Description          :                            *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 *                                                   *
 *****************************************************/

#include "xdr/IPDRStreamElementHelper.h"


/*!
 *  NAME:
 *      writeIPDRStreamElementHelper() - calls the appropriate function to write an 
 *                                       ipdrStreamElement to the output stream.   
 *                  
 *  DESCRIPTION:
 *      -This function will be used to call the appropriate function to write an 
 *       ipdrStreamElement to the output stream
 *      -Based on the value of the descriminator, it would call either of the three 
 *       functions: 
 *		 writeRecordDescriptorHelper, writeIPDRRecordDescriptorHelper, 
 *       writeIPDRXDRDocEndHelper
 *		-These called functions would actually write the ipdrStreamElement in XDR 
 *       format to the output stream.
 *
 *	ARGUMENTS:
 *      - IPDRCommonParameters (IN/OUT)
 *	    - IPDRStreamElement {IN}
 *		- pErrorCode { IN / OUT } 
 *
 *  RETURNS 	
 *	      Returns Zero or appropriate error code
*/

int writeIPDRStreamElementHelper(
								 IPDRCommonParameters* pIPDRCommonParameters, 
								 IPDRStreamElement* pIPDRStreamElement, 
								 int* pErrorCode
								)
{
   int descriminator = 0;
	int errorSeverity = 0;

	DocEnd* pDocEnd;
	RecordDescriptor* pRecordDescriptor; 
	IPDRRecord* pIPDRRecord;

	if(pIPDRStreamElement == NULL || pIPDRCommonParameters == NULL) {
	    *pErrorCode = IPDR_NULL_INPUT_PARAMETER;
	    return (IPDR_ERROR);
	}

	descriminator = pIPDRStreamElement->descriminator_;

	write_long(pIPDRStreamElement->descriminator_, pIPDRCommonParameters->pStreamHandle_);
	
	switch (descriminator)
	{
		case STREAM_RECORD_DESCRIPTOR:

			pRecordDescriptor = pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pRecordDescriptor_;    
			
			/*call writeRecordDescriptorHelper passing descriptor structure.*/
			errorSeverity = writeRecordDescriptorHelper(pIPDRCommonParameters, pRecordDescriptor, pErrorCode);
			if (errorSeverity == IPDR_WARNING) {
				errorHandler(pIPDRCommonParameters, pErrorCode, "writeRecordDescriptorHelper");	
			}
			else if (errorSeverity == IPDR_ERROR) {
				pIPDRCommonParameters->DocState_ = NONWRITABLE; 
				exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeRecordDescriptorHelper");	
				return (IPDR_ERROR);
			}	

			break;

		case STREAM_IPDR_RECORD:

			pIPDRRecord = pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_;
			
			/*call writeIPDRXDRRecordHelper passing Record structure.*/
			errorSeverity = writeIPDRXDRRecordHelper(pIPDRCommonParameters, pIPDRRecord, pErrorCode);
			if (errorSeverity == IPDR_WARNING) {
				errorHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXDRRecordHelper");	
			}
			else if (errorSeverity == IPDR_ERROR) {
				pIPDRCommonParameters->DocState_ = NONWRITABLE; 
				exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXDRRecordHelper");	
				return (IPDR_ERROR);
			}			

			break;
		
		case STREAM_DOC_END:

			pDocEnd = pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pDocEnd_;     

			/*call writeIPDRDocEndHelper passing docEnd structure.*/
			errorSeverity = writeIPDRXDRDocEndHelper(pIPDRCommonParameters, pDocEnd, pErrorCode);
			if (errorSeverity == IPDR_WARNING) {
				errorHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXDRDocEndHelper");	
			}
			else if (errorSeverity == IPDR_ERROR) {
				pIPDRCommonParameters->DocState_ = NONWRITABLE; 
				exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeIPDRXDRDocEndHelper");	
				return (IPDR_ERROR);
			}			

			break;
	}

	return (IPDR_OK);
}


/*!
 *  NAME:
 *      readIPDRStreamElementHelper() - This function reads the IPDR records
 *                  
 *  DESCRIPTION:
 *      This function
 *       - Checks the state of the document, it should be READABLE
 *		 - Reads the value of descriminator
 *			 a.Calls "readRecordDescriptorHelper", if value of descriminator is 1
 *			 b.Calls "readIPDRXDRRecordHelper", if value of descriminator is 2
 *			 c.Returns 0 to readIPDR, if value of descriminator is 3	
 *
 *	ARGUMENTS:
 *      - IPDRCommonParameters (IN/OUT)
 *	    - IPDRStreamElement (IN/OUT)
 *		- errorCode { IN / OUT } 
 *
 *  RETURNS 	
 *	      Returns Zero or appropriate error code
*/

int readIPDRStreamElementHelper(
								IPDRCommonParameters *pIPDRCommonParameters, 
								IPDRStreamElement* pIPDRStreamElement,
								ListAttributeDescriptor* pListServiceAttributeDescriptor,
								int* pErrorCode
							   )
{
	int descriminator = 0;
	int errorSeverity = 0;

	if(pIPDRStreamElement == NULL || pIPDRCommonParameters == NULL) {
	    *pErrorCode = IPDR_NULL_INPUT_PARAMETER;
	    return (IPDR_ERROR);
	}
	
	if(pIPDRStreamElement->descriminator_ != 1 && pIPDRStreamElement->descriminator_ != 2) {
			descriminator = read_long(pIPDRCommonParameters->pStreamHandle_); 
		}
		else{
			descriminator = pIPDRStreamElement->descriminator_;
		}
				
	switch(descriminator)
	{
		case STREAM_RECORD_DESCRIPTOR:
			errorSeverity = readRecordDescriptorHelper(
										pIPDRCommonParameters, 
										pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pRecordDescriptor_, 
										pErrorCode); 
			if (errorSeverity == IPDR_WARNING) {
				errorHandler(pIPDRCommonParameters, pErrorCode, "readRecordDescriptorHelper");
			}
			else if (errorSeverity == IPDR_ERROR) {
				pIPDRCommonParameters->DocState_ = NONREADABLE; 
				exceptionHandler(pIPDRCommonParameters, pErrorCode, "readRecordDescriptorHelper");
				return (IPDR_ERROR);
			}	
		
			/* this function checks for duplicate attribute names*/
			errorSeverity = validateAttributeName(pIPDRCommonParameters, pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pRecordDescriptor_->pListAttributeDescriptor_, pErrorCode);
			if (errorSeverity == IPDR_WARNING) {
				errorHandler(pIPDRCommonParameters, pErrorCode, "validateAttributeName"); 
			}
			else if (errorSeverity == IPDR_ERROR) {
				pIPDRCommonParameters->DocState_ = NONWRITABLE; 
				exceptionHandler(pIPDRCommonParameters, pErrorCode, "validateAttributeName"); 
				return (IPDR_ERROR);
			}
		
			break;
		
		case STREAM_IPDR_RECORD:

			errorSeverity = readIPDRXDRRecordHelper(
								pIPDRCommonParameters,
								pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_, 
								pListServiceAttributeDescriptor,
								pErrorCode); 
/*
			errorSeverity = readIPDRXDRRecordHelper(
								pIPDRCommonParameters,
								pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_, 
								pErrorCode); 
*/
			if (errorSeverity == IPDR_WARNING) {
				errorHandler(pIPDRCommonParameters, pErrorCode, "readIPDRXDRRecordHelper");
			}
			else if (errorSeverity == IPDR_ERROR) {
				pIPDRCommonParameters->DocState_ = NONREADABLE; 
				exceptionHandler(pIPDRCommonParameters, pErrorCode, "readIPDRXDRRecordHelper");
				return (IPDR_ERROR);
			}

			break;

		case STREAM_DOC_END:
             /*			
			 On getting the descriminator as 3, does nothing & returns. 
			 Calling application (In this case, main), will be calling readDocEnd explicitly
			 to read the DocEnd structure.
			 */
			
			break;
		
		default:
		
			*pErrorCode = IPDR_INVALID_DESCRIMINATOR;
			return (IPDR_ERROR);
			break;
	}

	return (IPDR_OK);
}
