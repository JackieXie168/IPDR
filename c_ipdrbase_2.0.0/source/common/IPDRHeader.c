/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/

/*****************************************************
 * File                 : IPDRHeader                 *
 * Description          :                            *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 *                                                   *
 *****************************************************/

#include "common/IPDRHeader.h"


/*!
 *  NAME:
 *      validateHeader() - This function performs various validations required 
 *                         for Header Creation.
 *                     
 *  DESCRIPTION:
 *     	Array of Other NameSpace Items must be equal to or less then Service 
 *      Definition Items otherwise Error.
 *  
 *	ARGUMENTS:
 *      - IPDRHeader { IN / OUT }
 *		- pErrorCode {IN / OUT } 
 *
 *  RETURNS 	
 *	    Returns Zero or appropriate error code
*/


/* 
	Array of Other NameSpace Items must be equal to or 
    less then Service Definition Items otherwise Error.	
*/	          
int validateHeader(
				   IPDRHeader *pIPDRHeader, 
				   int* pErrorCode
				  )	
{	
	char* nameSpaceURI = NULL;
	char tempSpaceURI[MAX_LEN];
	    
	ListNameSpaceInfo* pTempNameSpaceInfo = NULL;
	
	if(pIPDRHeader == NULL) {
	    *pErrorCode = IPDR_NULL_INPUT_PARAMETER;
	    return (IPDR_ERROR);
	}

	nameSpaceURI = (char*) calloc(1, MAX_NAMESPACE_URI); 
	
	if(nameSpaceURI == NULL ) {
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		return(IPDR_ERROR);
	}	

 
	/* Validation for default name space when namespace ID is
	 * zero length string                                   
     */
 		  
	pTempNameSpaceInfo = pIPDRHeader->pOtherNameSpaceInfo_;
	while (pTempNameSpaceInfo  != NULL) {
		if(strcmp(pTempNameSpaceInfo->NameSpaceInfo_.nameSpaceID_,"") 
			      == 0 ){
			strcpy(nameSpaceURI,
				   pTempNameSpaceInfo->NameSpaceInfo_.nameSpaceURI_);
			convertTolower(nameSpaceURI, tempSpaceURI);
			if (strcmp(tempSpaceURI,pIPDRHeader->defaultNameSpaceURI_) !=0 ){
			/*if (strcmp(nameSpaceURI,pIPDRHeader->defaultNameSpaceURI_) !=0 )*/
				#ifdef DEBUG_IPDR
					printf("\n NameSpaceURI has to be default !");
				#endif
				*pErrorCode = IPDR_MISMATCH_NS_DFLT_NS;
				return(IPDR_ERROR);   
			}
		}
		else {
			strcpy(nameSpaceURI,
				   pTempNameSpaceInfo->NameSpaceInfo_.nameSpaceURI_);
			/*
			convertTolower(nameSpaceURI, tempSpaceURI);
			if (strcmp(tempSpaceURI,pIPDRHeader->defaultNameSpaceURI_) !=0 )
			*/
			if (strcmp(nameSpaceURI,pIPDRHeader->defaultNameSpaceURI_) == 0 ){
				#ifdef DEBUG_IPDR
					printf("\n NameSpaceURI has to be other than default !");
				#endif
				*pErrorCode = IPDR_MISMATCH_NS_DFLT_NS;
				return(IPDR_ERROR);
			}
		}

		pTempNameSpaceInfo  = pTempNameSpaceInfo->pNext_;
	}

	free(nameSpaceURI);     
	return ( IPDR_OK);

}
