/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/



/*****************************************************
 * File                 : IPDRReocrdHelper           *
 * Description          :                            *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 *                                                   *
 *****************************************************/

#include "xdr/IPDRXDRRecordHelper.h"

/*!
 *  NAME:
 *      writeIPDRXDRRecordHelper() - used to write an ipdrRecord structure to the output
 *                                stream.
 *
 *  DESCRIPTION:
 *      - This function will be used to write an ipdrRecord structure to the output
 *        stream
 *      - This function will internally call writeFNFDataHelper and the basic write
 *        function write_long() to actually marshall the data in the XDR format to
 *		  the output stream.
 *
 *	ARGUMENTS:
 *      - IPDRCommonParameters (IN/OUT)
 *	    - IPDRRecord {IN}
 *		- pErrorCode { IN / OUT }
 *
 *  RETURNS
 *	      Returns Zero or appropriate error code
*/

int writeIPDRXDRRecordHelper(
						  IPDRCommonParameters* pIPDRCommonParameters,
						  IPDRRecord* pIPDRRecord,
						  int* pErrorCode
						 )
{
	int errorSeverity = 0;

	if(pIPDRRecord == NULL || pIPDRCommonParameters == NULL) {
	    *pErrorCode = IPDR_NULL_INPUT_PARAMETER;
	    return (IPDR_ERROR);
	}

	write_long(pIPDRRecord->descriptorID_, pIPDRCommonParameters->pStreamHandle_);

	errorSeverity = writeFNFDataHelper(pIPDRCommonParameters, pIPDRRecord->pFNFData_, pErrorCode);
	if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, "writeFNFDataHelper");
	}
	else if (errorSeverity == IPDR_ERROR) {
		pIPDRCommonParameters->DocState_ = NONWRITABLE;
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "writeFNFDataHelper");
		return (IPDR_ERROR);
	}

	return (IPDR_OK);
}

/*!
 *  NAME:
 *      readIPDRXDRRecordHelper() - 	This function reads the IPDR Record
 *
 *  DESCRIPTION:
 *      This function1
 *	      - Reads & Validates descriptor Id
 *	      - Populates the IPDRRecord structure with the above read value
 *		  - Calls "readFNFDataHelper"
 *
 *	ARGUMENTS:
 *      - IPDRCommonParameters (IN/OUT)
 *	    - IPDRRecord {IN/OUT}
 *		- pErrorCode { IN / OUT }
 *
 *  RETURNS
 *	      Returns Zero or appropriate error code
*/

/*
int readIPDRXDRRecordHelper(
						 IPDRCommonParameters* pIPDRCommonParameters,
						 IPDRRecord* pIPDRRecord,
						 int* pErrorCode
						)
*/
int readIPDRXDRRecordHelper(
						 IPDRCommonParameters* pIPDRCommonParameters,
						 IPDRRecord* pIPDRRecord,
						 ListAttributeDescriptor* pListServiceAttributeDescriptor,
						 int* pErrorCode
						)
{
	int errorSeverity = 0;
	int descriptorID = 0;

	if(pIPDRRecord == NULL || 
		pIPDRCommonParameters == NULL 
	  ) {
	    *pErrorCode = IPDR_NULL_INPUT_PARAMETER;
	    return (IPDR_ERROR);
	}


	descriptorID = read_long(pIPDRCommonParameters->pStreamHandle_);
	pIPDRRecord->descriptorID_ = descriptorID;

	errorSeverity = readFNFDataHelper(
									pIPDRCommonParameters, 
									pIPDRRecord->pFNFData_, 
									descriptorID, 
						 			pListServiceAttributeDescriptor,
									pErrorCode
									 );

/*
	errorSeverity = readFNFDataHelper(
									pIPDRCommonParameters, 
									pIPDRRecord->pFNFData_, 
									descriptorID, 
									pErrorCode
									 );
*/
	if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, "readFNFDataHelper");
	}
	else if (errorSeverity == IPDR_ERROR) {
		pIPDRCommonParameters->DocState_ = NONREADABLE; 
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "readFNFDataHelper");
		return (IPDR_ERROR);
	}	
	
	return (IPDR_OK);
}
