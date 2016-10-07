/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/


/*****************************************************
 * File                 : FNFData                    *
 * Description          :                            *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 *                                                   *
 *****************************************************/

#include "common/FNFData.h"

/*!
 *  NAME:
 *      FNFDataCheck() - This function does validations over Input Data.
 *
 *  DESCRIPTION:
 *      - This function retrieve the data set from the addresses given 
 *        in as array of pointers.
 *		- After that it will do the following validations:
 *		  a.Number of elements in the Data Array should be equal 
 *          to descriptor elements.
 *		  b.Type of Descriptor should match with type of Input Data.
 *  
 *  ARGUMENTS:
 *      - IPDRValue {IN}
 * 		- AttributeDescriptorHashTable {IN}
 *
 *  RETURNS 	
 *	    Returns Zero or appropriate error code
 */

	
int FNFDataCheck(
				 ListIPDRData* pIPDRData, 
				 FNFType* pFNFType , 
				 int* pErrorCode
				 )
{	
	int count = 0;
	int listIPDRDataLength = 0;
	int listAttributeDescriptorLength = 0;

	if(pIPDRData == NULL || pFNFType == NULL) {
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		return(IPDR_ERROR);
	}

	listIPDRDataLength = getLengthListIPDRData(pIPDRData);
	listAttributeDescriptorLength = 
		getLengthListAttributeDescriptor
		(pFNFType->pListAttributeDescriptor_);  

	if (listIPDRDataLength != listAttributeDescriptorLength) {	
		*pErrorCode = IPDR_MISMATCH_DESCRIPTOR_DATA_ARRAY;
		return(IPDR_ERROR);
	}

	for (count = 0; count < listIPDRDataLength; count++) {

		if ((pIPDRData->IPDRDataType_ == IPDRDATA_HEXADECARRAY)
			 && (pIPDRData->IPDRDataUnion_.IPDRByteArray_ == NULL)
		   ) {
			*pErrorCode = IPDR_NULL_DATA_VALUE;
			return (IPDR_ERROR);
		}

		if ((pIPDRData->IPDRDataType_ == IPDRDATA_STRING)
			 && (pIPDRData->IPDRDataUnion_.IPDRUTF8Array_ == NULL)
		   ) {
			*pErrorCode = IPDR_NULL_DATA_VALUE;
			return (IPDR_ERROR);
		}

		pIPDRData = pIPDRData->pNext_;  
	}
	return (IPDR_OK);
}
