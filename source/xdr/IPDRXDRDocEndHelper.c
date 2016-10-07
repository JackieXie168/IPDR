/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/



/*****************************************************
 * File                 : IPDRDocEndHelper           *
 * Description          :                            *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 *                                                   *
 *****************************************************/

#include "xdr/IPDRXDRDocEndHelper.h"

/*!
 *  NAME:
 *      readIPDRXDRDocEndHelper() - This function will return DocEnd structure.
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


int readIPDRXDRDocEndHelper(
						 IPDRCommonParameters* pIPDRCommonParameters, 
						 DocEnd* pDocEnd, 
						 int* pErrorCode
						)	
{	
	hyper* pTmpHyper = NULL;
	
	if(pDocEnd == NULL) {
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		return(IPDR_ERROR);
	}


	pDocEnd->count_ = read_long(pIPDRCommonParameters->pStreamHandle_); 	
	pTmpHyper = read_longlong(pIPDRCommonParameters->pStreamHandle_);	
    pDocEnd->endTime_ = (hyper) *pTmpHyper;
	
	free (pTmpHyper);
	return (IPDR_OK);	
}


/*!
 *  NAME:
 *      writeIPDRXDRDocEndHelper() - takes DocEnd structure as input and writes its XDR
 *                                equivalent on output stream.
 *
 *  DESCRIPTION:
 *       - This function will be used to write an IPDRDocEnd structure to the output 
 *				stream.
 *	     - This function will internally call the basic write functions (eg 
 *         write_long()) to actually marshall the data in the XDR format to the 
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

 	
int writeIPDRXDRDocEndHelper(
						  IPDRCommonParameters* pIPDRCommonParameters, 
						  DocEnd* pDocEnd, 
						  int* pErrorCode
						 )	
{	
	if(pDocEnd == NULL) {
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		return(IPDR_ERROR);
	}
	
	write_long(pDocEnd->count_, pIPDRCommonParameters->pStreamHandle_);	
	write_longlong(pDocEnd->endTime_, pIPDRCommonParameters->pStreamHandle_);	
	
	return (IPDR_OK);	
}
