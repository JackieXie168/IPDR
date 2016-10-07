/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/



/*****************************************************
 * File                 : NameSpaceInfoHelper        *
 * Description          :                            *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 *                                                   *
 *****************************************************/

#include "xdr/NameSpaceInfoHelper.h"

/*!
 *  NAME:
 *      writeNameSpaceInfoHelper() - used to write an namespaceinfo 
 *                                       structure to the output stream.   
 *                                        
 *                  
 *  DESCRIPTION:
 *      -This function will be used to write an namespaceinfo structure to the
 *       output stream.
 *
 *	ARGUMENTS:
 *      - IPDRCommonParameters (IN/OUT)
 *	    - NameSpaceInfo {IN}
 *      - error Code {IN/OUT}
 *
 *  RETURNS 	
 *	      Returns Zero or appropriate error code
*/

int writeNameSpaceInfoHelper(
							 IPDRCommonParameters* pIPDRCommonParameters, 
							 NameSpaceInfo* pNameSpaceInfo, 
							 int* pErrorCode
							)
{
	int nameSpaceIDLength = 0;
	int nameSpaceURILength = 0;

	if(pNameSpaceInfo == NULL || pIPDRCommonParameters == NULL) {
	    *pErrorCode = IPDR_NULL_INPUT_PARAMETER;
	    return (IPDR_ERROR);
	}
	
	nameSpaceURILength = strlen(pNameSpaceInfo->nameSpaceURI_);
	write_long(nameSpaceURILength, pIPDRCommonParameters->pStreamHandle_);
	write_wstring((wchar *) pNameSpaceInfo->nameSpaceURI_, pIPDRCommonParameters->pStreamHandle_, pIPDRCommonParameters->schemaVersion);
		
	nameSpaceIDLength = strlen(pNameSpaceInfo->nameSpaceID_);
	write_long(nameSpaceIDLength, pIPDRCommonParameters->pStreamHandle_);
	write_wstring((wchar *) pNameSpaceInfo->nameSpaceID_, pIPDRCommonParameters->pStreamHandle_, pIPDRCommonParameters->schemaVersion);
	
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      readNameSpaceInfoHelper() - This function reads the Name Space Info.
 *                                                
 *  DESCRIPTION:
 *      	This function
 *           -Reads descriptor Id
 *			 -Reads NameSpaceID, NameSpaceURI
 *			 -Populates the NameSpaceInfo structure with the above read values
 *
 *	ARGUMENTS:
 *      - IPDRCommonParameters (IN/OUT)
 *	    - NameSpaceInfo (IN/OUT)
 *      - error Code {IN/OUT}
 *
 *  RETURNS 	
 *	      Returns Zero or appropriate error code
*/

int readNameSpaceInfoHelper(
							IPDRCommonParameters* pIPDRCommonParameters, 
							NameSpaceInfo* pNameSpaceInfo, 
							int* pErrorCode
						   )
{
	int nameSpaceIDLength = 0;
	int nameSpaceURILength = 0;

	if(pNameSpaceInfo == NULL || pIPDRCommonParameters == NULL) {
	    *pErrorCode = IPDR_NULL_INPUT_PARAMETER;
	    return (IPDR_ERROR);
	}
	
	nameSpaceURILength = read_long(pIPDRCommonParameters->pStreamHandle_);
	pNameSpaceInfo->nameSpaceURI_ = (char *) read_wstring(nameSpaceURILength, pIPDRCommonParameters->pStreamHandle_, pIPDRCommonParameters->schemaVersion);
		
	nameSpaceIDLength = read_long(pIPDRCommonParameters->pStreamHandle_);
	pNameSpaceInfo->nameSpaceID_ = (char *) read_wstring(nameSpaceIDLength, pIPDRCommonParameters->pStreamHandle_, pIPDRCommonParameters->schemaVersion);

	return (IPDR_OK);
}
