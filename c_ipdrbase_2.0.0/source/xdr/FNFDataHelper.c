/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/



/*****************************************************
 * File                 : FNFDataHelper              *
 * Description          :                            *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 * 20/01/2001              created                   *
 *****************************************************/


#include "xdr/FNFDataHelper.h"


/*!
 *  NAME:
 *      writeFNFDataHelper() - writes the XDR equivalent of FNFData structure on 
 *                             output stream
 *
 *  DESCRIPTION:
 *      - This function will be used to write an FNFData structure to the output stream.
 *		- This function internally uses the basic write functions (eg write_long()) to 
 *		  actually marshall the data in the XDR format to the output stream.
 *
 *  ARGUMENTS:
 *      - IPDRCommonParameters (IN/OUT)
 *	    - FNFData {IN}
 *
 *  RETURNS 	
 *	    Returns Zero or appropriate error code
*/


int writeFNFDataHelper(IPDRCommonParameters* pIPDRCommonParameters, 
					   FNFData* pFNFData, int* pErrorCode)
{
	int count = 0;
	int cnt = 0;
	int typeCode = 0;
	int listIPDRDataLength = 0;
	int byteArrayLength = 0;
   char* tempComplexType = NULL; 
   char * tempString = NULL;
	char* serviceTypeToken=NULL;
		
	ListIPDRData* pCurrentListIPDRData = NULL;
	ListAttributeDescriptor* pCurrentListAttributeDescriptor = NULL; 
 	ListIPDRArrayData* pCurrentListIPDRArrayData = NULL;
 	ListIPDRStructureData* pCurrentListIPDRStructureData = NULL;

	if(pFNFData == NULL || pIPDRCommonParameters == NULL) {
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		return(IPDR_ERROR);
	}
	
	pCurrentListIPDRData = pFNFData->pListIPDRData_; 
	pCurrentListAttributeDescriptor = pFNFData->pListAttributeDescriptor_; 
	
	listIPDRDataLength = getLengthListIPDRData(pFNFData->pListIPDRData_);
	write_long(IPDR_INDEFINITE_LENGTH_INDICATOR, pIPDRCommonParameters->pStreamHandle_); 
	
	for (count = 0; count < listIPDRDataLength; count++)
	{

if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_[0] != '\0') {
   	tempString = (char *) calloc(strlen(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_) + 1, sizeof(char));
   	tempComplexType = (char *) calloc(strlen(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_) + 1, sizeof(char));
   	strcpy(tempString, pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_);
   	serviceTypeToken = strtok(tempString, ":");
   	
   	if(strcmp(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_,serviceTypeToken)) {
   		getIPDRSubstring(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_, tempComplexType, ':');
      	strcpy(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_,tempComplexType);
   	}	
   	free(tempString);
   	free(tempComplexType);
}

   	if(isPrimitiveComplexType(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_) == IPDR_FALSE)
   	{
   		/*Check for noOfElements and loop that many time for Array of Structures
   		For unbounded... get the noOfelements from the FNFData and assign it to the descriptor noOfelements*/
   		if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ == -1) {
   				pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->pListIPDRStructureData_->noOfElements_;
   		}
   
   		if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
   				pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->pListIPDRStructureData_->noOfElements_;
				write_long(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_, pIPDRCommonParameters->pStreamHandle_);
   			pCurrentListIPDRStructureData = pFNFData->pListIPDRData_->pListIPDRStructureData_;
   			for(cnt = 0;cnt < (pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_);
                   cnt++) { 
   				writeIPDRXDRComplexRecordHelper(pFNFData,
   												  pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_,
   												  pIPDRCommonParameters, 
													  pErrorCode);
   				pFNFData->pListIPDRData_->pListIPDRStructureData_ = 
   				pFNFData->pListIPDRData_->pListIPDRStructureData_->pNext_;
   			}
   			pFNFData->pListIPDRData_->pListIPDRStructureData_ = pCurrentListIPDRStructureData;
   		}
   		else {
   				writeIPDRXDRComplexRecordHelper(pFNFData,
   												  	pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_,
   												  	pIPDRCommonParameters, 
														pErrorCode);
   		}
   		pFNFData->pListAttributeDescriptor_ = pFNFData->pListAttributeDescriptor_->pNext_;
   		pFNFData->pListIPDRData_ = pFNFData->pListIPDRData_->pNext_;
   		continue;
   	}

		typeCode = convAttributeTypeToTypeCode(
		pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->attributeType_);
		
		switch (typeCode)
		{
			case IPDRDATA_LONG: 
			  		if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ == -1) {
			  			pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
			  		}

			  		if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
			  			pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
						/*writing number of Elements*/
						write_long(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_, pIPDRCommonParameters->pStreamHandle_);
						pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
						for(cnt = 0;cnt < (pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_);
                	cnt++) { 
							/*write_long((int) pFNFData->pListIPDRData_->pListIPDRArrayData_->IPDRDataUnion_.IPDRLong_, 
											pIPDRCommonParameters->pStreamHandle_);*/
							write_long((int) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRLong_, 
											pIPDRCommonParameters->pStreamHandle_);

							pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;
						}
					} else {
						write_long((int) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRLong_, 
										pIPDRCommonParameters->pStreamHandle_);
					}
               break;

			case IPDRDATA_ULONG: 
			  		if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ == -1) {
			  			pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
			  		}

			  		if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
			  			pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
						/*writing number of Elements*/
						write_long(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_, pIPDRCommonParameters->pStreamHandle_);
						pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
						for(cnt = 0;cnt < (pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_);
                	cnt++) { 
							write_u_long((unsigned int) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRULong_, 
												pIPDRCommonParameters->pStreamHandle_);

							pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;
						}
					} else {
						write_u_long((unsigned int) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRULong_, 
						pIPDRCommonParameters->pStreamHandle_);
					}
               break;

         case IPDRDATA_LONGLONG: 
			  		if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ == -1) {
			  			pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
			  		}

			  		if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
			  			pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
						/*writing number of Elements*/
						write_long(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_, pIPDRCommonParameters->pStreamHandle_);
						pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
						for(cnt = 0;cnt < (pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_);
                	cnt++) { 
							write_longlong((hyper) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRLongLong_, 
												pIPDRCommonParameters->pStreamHandle_);

							pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;
						}
					} else {
						write_longlong((hyper) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRLongLong_, 
											pIPDRCommonParameters->pStreamHandle_);
					}
               break;

         case IPDRDATA_ULONGLONG: 
			  		if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ == -1) {
			  			pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
			  		}

			  		if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
			  			pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
						/*writing number of Elements*/
						write_long(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_, pIPDRCommonParameters->pStreamHandle_);
						pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
						for(cnt = 0;cnt < (pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_);
                	cnt++) { 
							write_u_longlong((u_hyper) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRULongLong_, 
													pIPDRCommonParameters->pStreamHandle_);

							pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;
						}
					} else {
							write_u_longlong((u_hyper) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRULongLong_, 
													pIPDRCommonParameters->pStreamHandle_);
					}
               break;

         case IPDRDATA_FLOAT: 
			  		if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ == -1) {
			  			pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
			  		}

			  		if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
			  			pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
						/*writing number of Elements*/
						write_long(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_, pIPDRCommonParameters->pStreamHandle_);
						pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
						for(cnt = 0;cnt < (pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_);
                	cnt++) { 
							write_float((float) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRFloat_, 
											pIPDRCommonParameters->pStreamHandle_);

							pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;
						}
					} else {
						write_float((float) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRFloat_, 
										pIPDRCommonParameters->pStreamHandle_);
					}
               break;

         case IPDRDATA_DOUBLE: 
			  		if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ == -1) {
			  			pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
			  		}

			  		if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
			  			pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
						/*writing number of Elements*/
						write_long(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_, pIPDRCommonParameters->pStreamHandle_);
						pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
						for(cnt = 0;cnt < (pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_);
                	cnt++) { 
							write_double((double) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRDouble_, 
											 pIPDRCommonParameters->pStreamHandle_);

							pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;
						}
					} else {
						write_double((double) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRDouble_, 
										 pIPDRCommonParameters->pStreamHandle_);
					}
               break;

         case IPDRDATA_HEXADECARRAY: 
			  		if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ == -1) {
			  			pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
			  		}

			  		if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
			  			pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
						/*writing number of Elements*/
						write_long(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_, pIPDRCommonParameters->pStreamHandle_);
						pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
						for(cnt = 0;cnt < (pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_);
                	cnt++) { 
							byteArrayLength = getByteArrayLength(pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRByteArray_);
							write_long(byteArrayLength, pIPDRCommonParameters->pStreamHandle_);
							write_noctet_array(pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRByteArray_, pIPDRCommonParameters->pStreamHandle_, byteArrayLength, pIPDRCommonParameters->schemaVersion);     

							pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;
						}
					} else {
						byteArrayLength = getByteArrayLength(pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRByteArray_);
						write_long(byteArrayLength, pIPDRCommonParameters->pStreamHandle_);
						write_noctet_array(pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRByteArray_, pIPDRCommonParameters->pStreamHandle_, byteArrayLength, pIPDRCommonParameters->schemaVersion);     
					}
					break;

         case IPDRDATA_STRING: 
			  		if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ == -1) {
			  			pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
			  		}

			  		if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
			  			pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
						/*writing number of Elements*/
						write_long(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_, pIPDRCommonParameters->pStreamHandle_);
						pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
						for(cnt = 0;cnt < (pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_);
                	cnt++) { 
							write_long(strlen((char *) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_), pIPDRCommonParameters->pStreamHandle_);
				   		write_wstring((wchar *)pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_, pIPDRCommonParameters->pStreamHandle_, pIPDRCommonParameters->schemaVersion);

							pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;
						}
					} else {
						write_long(strlen((char *) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_), pIPDRCommonParameters->pStreamHandle_);
				   	write_wstring((wchar *)pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_, pIPDRCommonParameters->pStreamHandle_, pIPDRCommonParameters->schemaVersion);
					}
					break;
			
			case IPDRDATA_BOOLEAN: 
			  		if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ == -1) {
			  			pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
			  		}

			  		if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
			  			pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
						/*writing number of Elements*/
						write_long(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_, pIPDRCommonParameters->pStreamHandle_);
						pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
						for(cnt = 0;cnt < (pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_);
                	cnt++) {
							if(strcmp(pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRBoolean_, "0") == 0) { 
								/*write_long((int) 0, pIPDRCommonParameters->pStreamHandle_);*/
								write_boolean("false", pIPDRCommonParameters->pStreamHandle_);
							} else if(strcmp(pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRBoolean_, "1") == 0) {
								/*write_long((int) 1, pIPDRCommonParameters->pStreamHandle_);*/
								write_boolean("true", pIPDRCommonParameters->pStreamHandle_);
							}

							pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;
						}
					} else {
							if(strcmp(pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRBoolean_, "0") == 0) { 
								/*write_long((int) 0, pIPDRCommonParameters->pStreamHandle_);*/
								write_boolean("false", pIPDRCommonParameters->pStreamHandle_);
							} else if(strcmp(pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRBoolean_, "1") == 0) {
								/*write_long((int) 1, pIPDRCommonParameters->pStreamHandle_);*/
								write_boolean("true", pIPDRCommonParameters->pStreamHandle_);
							}
					}
               break;

			case IPDRDATA_SHORT: 
			  		if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ == -1) {
			  			pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
			  		}

			  		if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
			  			pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
						write_long(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_, pIPDRCommonParameters->pStreamHandle_);
						pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
						for(cnt = 0;cnt < (pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_);
                	cnt++) { 
							/*write_long((int) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRShort_, 
											pIPDRCommonParameters->pStreamHandle_);*/
							write_short((short int) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRShort_, 
											pIPDRCommonParameters->pStreamHandle_);

							pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;
						}
					} else {
						write_short((short int) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRShort_, 
										pIPDRCommonParameters->pStreamHandle_);
						/*write_long((int) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRShort_, 
										pIPDRCommonParameters->pStreamHandle_);*/
					}
               break;

			case IPDRDATA_USHORT: 
			  		if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ == -1) {
			  			pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
			  		}

			  		if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
			  			pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
						write_long(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_, pIPDRCommonParameters->pStreamHandle_);
						pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
						for(cnt = 0;cnt < (pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_);
                	cnt++) { 
							write_u_long((unsigned int) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUShort_, pIPDRCommonParameters->pStreamHandle_);
							/*write_u_short((unsigned int) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUShort_, pIPDRCommonParameters->pStreamHandle_);*/

							pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;
						}
					} else {
						write_u_long((unsigned int) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUShort_, pIPDRCommonParameters->pStreamHandle_);
						/*write_u_short((unsigned int) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRUShort_, pIPDRCommonParameters->pStreamHandle_);*/
					}
               break;

			case IPDRDATA_UBYTE: 
			  		if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ == -1) {
			  			pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
			  		}

			  		if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
			  			pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
						/*writing number of Elements*/
						write_long(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_, pIPDRCommonParameters->pStreamHandle_);
						pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
						for(cnt = 0;cnt < (pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_);
                	cnt++) { 
							write_long((int) pFNFData->pListIPDRData_->pListIPDRArrayData_->IPDRDataUnion_.IPDRByte_, 
											pIPDRCommonParameters->pStreamHandle_);
							/*write_byte((int) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRByte_, 
											pIPDRCommonParameters->pStreamHandle_);*/

							pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;
						}
					} else {
						/*write_byte((int) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRByte_, 
										pIPDRCommonParameters->pStreamHandle_);*/
						write_long((int) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRByte_, 
										pIPDRCommonParameters->pStreamHandle_);
					}
               break;

			case IPDRDATA_BYTE: 
			  		if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ == -1) {
			  			pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
			  		}

			  		if(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
			  			pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = pFNFData->pListIPDRData_->noOfElements_; 
						/*writing number of Elements*/
						write_long(pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_, pIPDRCommonParameters->pStreamHandle_);
						pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRArrayData_;
						for(cnt = 0;cnt < (pFNFData->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_);
                	cnt++) { 
							/*write_long((int) pFNFData->pListIPDRData_->pListIPDRArrayData_->IPDRDataUnion_.IPDRLong_, 
											pIPDRCommonParameters->pStreamHandle_);*/
							write_byte((int) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRByte_, 
											pIPDRCommonParameters->pStreamHandle_);

							pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;
						}
					} else {
						write_byte((int) pFNFData->pListIPDRData_->IPDRDataUnion_.IPDRByte_, 
										pIPDRCommonParameters->pStreamHandle_);
					}
               break;

			default:
					*pErrorCode = IPDR_NON_PRIMITIVE_DATA_TYPE;
					return (IPDR_ERROR);
					break;
        }
		pFNFData->pListIPDRData_ = pFNFData->pListIPDRData_->pNext_; 
		pFNFData->pListAttributeDescriptor_ = pFNFData->pListAttributeDescriptor_->pNext_;
	}

	pFNFData->pListIPDRData_ = 	pCurrentListIPDRData; 
	pFNFData->pListAttributeDescriptor_ = pCurrentListAttributeDescriptor; 
	
	return (IPDR_OK);
}


/*!
 *  NAME:
 *      readFNFDataHelper() - This function returns FNFData structure
 *  DESCRIPTION:
 *      This function:
 *	     -Checks for IPDR_INDEFINITE_LENGTH_INDICATOR marker
 *		 -Based on the attribute type, calls unmarsheller functions to read the data 
 *         values from XDR document.
 *  ARGUMENTS:
 *		- IPDRCommonParameters (IN/OUT)
 *		- FNFData (IN)
 *      - descriptorID (IN)
 *  RETURNS 	
 *	     Returns Zero or appropriate error code
*/
int readFNFDataHelper(
					  IPDRCommonParameters *pIPDRCommonParameters,
					  FNFData* pFNFData, 
					  int descriptorID, 
					  ListAttributeDescriptor* pListServiceAttributeDescriptor,
					  int* pErrorCode
					  )
{
   int count = 0;
   int cnt = 0;
   int countVal = 0;
   int cntVal = 0;
	int typeCode = 0;
	int errorSeverity = 0;
	int utfArrayLength = 0;
	int byteArraylength = 0;
	int inDefiniteLengthIndicator = 0;
	int listAttributeDescriptorLength = 0;
	int* value;
   char* tempComplexType; 
   char * tempString;

	hyper* tempHyper;
	u_hyper* tempUHyper;
	wchar* tempUTFArray;

	RecordDescriptor* pRecordDescriptor = NULL;
	ListIPDRComplex* pListIPDRComplex = NULL;
	IPDRData* pIPDRData = NULL; 
	ListAttributeDescriptor* pCurrent = NULL;
	ListIPDRComplexData* pListIPDRComplexData[MAX_IPDR_STRING];
	void* IPDRValue =  NULL;
	char* serviceTypeToken = NULL;
	
	void* dataValue[MAX_IPDR_STRING];
	int dataType[MAX_IPDR_STRING];
	char boolValue[3];

	memset(dataValue, 0, sizeof(dataValue));
	memset(dataType, 0, sizeof(dataType));
	memset(&pListIPDRComplexData, 0, sizeof(pListIPDRComplexData));

	if(pFNFData == NULL || pIPDRCommonParameters == NULL) {
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		return(IPDR_ERROR);
	}

	inDefiniteLengthIndicator = read_long(
		                        pIPDRCommonParameters->pStreamHandle_);
	if (inDefiniteLengthIndicator != IPDR_INDEFINITE_LENGTH_INDICATOR) 
		return (IPDR_INVALID_COMPACT_FORMAT);

	pRecordDescriptor = newRecordDescriptor();

	errorSeverity = getDescriptor(pIPDRCommonParameters, descriptorID, 
		                           pRecordDescriptor,  pErrorCode);
	if (errorSeverity == IPDR_WARNING) {
		errorHandler(pIPDRCommonParameters, pErrorCode, "getDescriptor");	
	}
	else if (errorSeverity == IPDR_ERROR) {
		pIPDRCommonParameters->DocState_ = NONREADABLE; 
		exceptionHandler(pIPDRCommonParameters, pErrorCode, "getDescriptor");
		freeRecordDescriptor(pRecordDescriptor);
		return (IPDR_ERROR);
	}

	pCurrent = pListServiceAttributeDescriptor;

	while (pListServiceAttributeDescriptor != NULL) {
		appendListAttributeDescriptor(&(pRecordDescriptor->pListServiceAttributeDescriptor_),
							pListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_,
							pListServiceAttributeDescriptor->pAttributeDescriptor_->attributeType_,
							pListServiceAttributeDescriptor->pAttributeDescriptor_->description_,
							pListServiceAttributeDescriptor->pAttributeDescriptor_->derivedType_,
							pListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_, 
							pListServiceAttributeDescriptor->pAttributeDescriptor_->pListAttrNameList_, 
							pListServiceAttributeDescriptor->pAttributeDescriptor_->pListAttrClassList_, 
							pListServiceAttributeDescriptor->pAttributeDescriptor_->pListAttrMaxOccursList_, 
							pListServiceAttributeDescriptor->pAttributeDescriptor_->noOfElements_ );
				
		pListServiceAttributeDescriptor = pListServiceAttributeDescriptor->pNext_; 
	}
	pListServiceAttributeDescriptor = pCurrent;
	
	pCurrent = pRecordDescriptor->pListAttributeDescriptor_;

	listAttributeDescriptorLength = getLengthListAttributeDescriptor(
		                            pRecordDescriptor->pListAttributeDescriptor_); 
	
	for (count = 0; count < listAttributeDescriptorLength; count++)
	{
		if(pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_[0] != '\0') {
   		tempString = (char *) calloc(strlen(pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_) + 1, sizeof(char));
   		tempComplexType = (char *) calloc(strlen(pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_) + 1, sizeof(char));
   		strcpy(tempString, pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_);
   		serviceTypeToken = strtok(tempString, ":");
   	
   		if(strcmp(pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_,serviceTypeToken)) {
   			getIPDRSubstring(pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_, tempComplexType, ':');
      		strcpy(pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_,tempComplexType);
   		}	
   		free(tempString);
   		free(tempComplexType);
		}

		/*Check for ComplexType*/
		if(isPrimitiveComplexType(pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_)
					== IPDR_FALSE) {

			/*check for noOfElements*/
   		if(pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ == -1) {
   			pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = 
										read_long(pIPDRCommonParameters->pStreamHandle_);
   		}

   		if(pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
   			for(cnt = 0;cnt < (pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_);
                   cnt++) {
					pListIPDRComplex = NULL; 
					readComplexFNFDataHelper(
										pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_,
					  						 pIPDRCommonParameters,
					  						 pRecordDescriptor, 
											 &pListIPDRComplex, 
					                   pErrorCode
											);
					appendListIPDRComplexData(&pListIPDRComplexData[cnt], 
														pListIPDRComplex, 
														IPDRDATA_STRUCTURE);
				}
				appendListIPDRWithComplexData(&((pFNFData)->pListIPDRData_),pListIPDRComplexData, IPDRDATA_STRUCTURE);
			} else {
					pListIPDRComplex = NULL; 
					readComplexFNFDataHelper(
									pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_,
					  						 pIPDRCommonParameters,
					  						 pRecordDescriptor, 
											 &pListIPDRComplex, 
					                   pErrorCode
											);
					appendListIPDRComplexData(&pListIPDRComplexData[0], 
														pListIPDRComplex, 
														IPDRDATA_STRUCTURE);
					appendListIPDRWithComplexData(&((pFNFData)->pListIPDRData_),pListIPDRComplexData, IPDRDATA_STRUCTURE);
					pListIPDRComplexData[0] = NULL;
			}
			pRecordDescriptor->pListAttributeDescriptor_ = 
				pRecordDescriptor->pListAttributeDescriptor_->pNext_;  
   		continue;
		}
 
		typeCode = convAttributeTypeToTypeCode(
		pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->attributeType_);
		switch (typeCode)
		{
			case IPDRDATA_LONG: 
			  		if(pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ != 0) {
			  			pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = 
														read_long(pIPDRCommonParameters->pStreamHandle_); 
			  		}

			  		if(pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
						for (countVal = 0; countVal < pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_; countVal++) {
      					dataValue[countVal] = (void *)read_long(pIPDRCommonParameters->pStreamHandle_);
						}
						dataType[0] = IPDRDATA_ARRAY;
						dataType[1] = IPDRDATA_LONG;
      	 			appendListIPDRDataComplex(&((pFNFData)->pListIPDRData_), dataValue, dataType);
					} else {
						appendListIPDRData(&pFNFData->pListIPDRData_, 
						(void *) read_long(pIPDRCommonParameters->pStreamHandle_), 
						IPDRDATA_LONG);  
					}
					break;

			case IPDRDATA_ULONG: 
			  		if(pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ != 0) {
			  			pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = 
														read_long(pIPDRCommonParameters->pStreamHandle_); 
			  		}

			  		if(pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
						for (countVal = 0; countVal < pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_; countVal++) {
      					dataValue[countVal] = (void *)read_u_long(pIPDRCommonParameters->pStreamHandle_);
						}
						dataType[0] = IPDRDATA_ARRAY;
						dataType[1] = IPDRDATA_ULONG;
      	 			appendListIPDRDataComplex(&((pFNFData)->pListIPDRData_), dataValue, dataType);
					} else {
						appendListIPDRData(&pFNFData->pListIPDRData_, 
						(void *) read_u_long(pIPDRCommonParameters->pStreamHandle_), 
						IPDRDATA_ULONG);  
					}
					break;

			case IPDRDATA_LONGLONG: 
			  		if(pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ != 0) {
			  			pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = 
														read_long(pIPDRCommonParameters->pStreamHandle_); 
			  		}

			  		if(pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
						pIPDRData = newIPDRData(IPDRDATA_LONGLONG); 
						for (countVal = 0; countVal < pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_; countVal++) {
							tempHyper = read_longlong(pIPDRCommonParameters->pStreamHandle_);
				   		pIPDRData->IPDRDataUnion_.IPDRLongLong_ = (hyper) *tempHyper;
      					dataValue[countVal] = (void *)&(pIPDRData->IPDRDataUnion_.IPDRLongLong_);
						}
						dataType[0] = IPDRDATA_ARRAY;
						dataType[1] = IPDRDATA_LONGLONG;
      	 			appendListIPDRDataComplex(&((pFNFData)->pListIPDRData_), dataValue, dataType);
					} else {
						pIPDRData = newIPDRData(IPDRDATA_LONGLONG); 
						tempHyper = read_longlong(pIPDRCommonParameters->pStreamHandle_);
				   	pIPDRData->IPDRDataUnion_.IPDRLongLong_ = (hyper) *tempHyper;

				   	appendListIPDRData(&pFNFData->pListIPDRData_, 
						(void *) &(pIPDRData->IPDRDataUnion_.IPDRLongLong_),
						IPDRDATA_LONGLONG);
					}
					free (tempHyper);
					freeIPDRData (pIPDRData);
					break;

			case IPDRDATA_ULONGLONG: 
			  		if(pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ != 0) {
			  			pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = 
														read_long(pIPDRCommonParameters->pStreamHandle_); 
			  		}

			  		if(pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
						pIPDRData = newIPDRData(IPDRDATA_ULONGLONG); 
						for (countVal = 0; countVal < pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_; countVal++) {
							tempUHyper = read_u_longlong(pIPDRCommonParameters->pStreamHandle_);
				   		pIPDRData->IPDRDataUnion_.IPDRULongLong_ = (u_hyper) *tempUHyper;
      					dataValue[countVal] = (void *)&(pIPDRData->IPDRDataUnion_.IPDRULongLong_);
						}
						dataType[0] = IPDRDATA_ARRAY;
						dataType[1] = IPDRDATA_ULONGLONG;
      	 			appendListIPDRDataComplex(&((pFNFData)->pListIPDRData_), dataValue, dataType);
					} else {
						pIPDRData = newIPDRData(IPDRDATA_ULONGLONG); 
						tempUHyper = read_u_longlong(pIPDRCommonParameters->pStreamHandle_);
						pIPDRData->IPDRDataUnion_.IPDRULongLong_  = (u_hyper) *tempUHyper;
					
						appendListIPDRData(&pFNFData->pListIPDRData_, 
										(void *) &(pIPDRData->IPDRDataUnion_.IPDRULongLong_),
										IPDRDATA_ULONGLONG);
					}
					free (tempUHyper);
					freeIPDRData (pIPDRData);
					break;
					
            case IPDRDATA_FLOAT: 
			  		if(pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ != 0) {
			  			pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = 
														read_long(pIPDRCommonParameters->pStreamHandle_); 
			  		}

			  		if(pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
						pIPDRData = newIPDRData(IPDRDATA_FLOAT); 
						for (countVal = 0; countVal < pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_; countVal++) {
				   		pIPDRData->IPDRDataUnion_.IPDRFloat_ = read_float(pIPDRCommonParameters->pStreamHandle_);
      					dataValue[countVal] = (float *) &(pIPDRData->IPDRDataUnion_.IPDRFloat_);
						}
						dataType[0] = IPDRDATA_ARRAY;
						dataType[1] = IPDRDATA_FLOAT;
      	 			appendListIPDRDataComplex(&((pFNFData)->pListIPDRData_), dataValue, dataType);
					} else {
						pIPDRData = newIPDRData(IPDRDATA_FLOAT); 
						pIPDRData->IPDRDataUnion_.IPDRFloat_ = 
							read_float(pIPDRCommonParameters->pStreamHandle_);
						IPDRValue = (float *) &pIPDRData->IPDRDataUnion_.IPDRFloat_;
					
						appendListIPDRData(&pFNFData->pListIPDRData_, 
												IPDRValue,
												IPDRDATA_FLOAT);                      
					}
					
					freeIPDRData (pIPDRData);
					break;
            
			case IPDRDATA_DOUBLE: 
			  		if(pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ != 0) {
			  			pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = 
														read_long(pIPDRCommonParameters->pStreamHandle_); 
			  		}

			  		if(pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
						pIPDRData = newIPDRData(IPDRDATA_DOUBLE); 
						for (countVal = 0; countVal < pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_; countVal++) {
				   		pIPDRData->IPDRDataUnion_.IPDRDouble_ = read_double(pIPDRCommonParameters->pStreamHandle_);
      					dataValue[countVal] = (double *) &(pIPDRData->IPDRDataUnion_.IPDRDouble_);
						}
						dataType[0] = IPDRDATA_ARRAY;
						dataType[1] = IPDRDATA_DOUBLE;
      	 			appendListIPDRDataComplex(&((pFNFData)->pListIPDRData_), dataValue, dataType);
					} else {
						pIPDRData = newIPDRData(IPDRDATA_DOUBLE); 
						pIPDRData->IPDRDataUnion_.IPDRDouble_ = 
							read_double(pIPDRCommonParameters->pStreamHandle_);
						IPDRValue = (double *) &pIPDRData->IPDRDataUnion_.IPDRDouble_;
					
						appendListIPDRData(&pFNFData->pListIPDRData_, 
											IPDRValue, 
											IPDRDATA_DOUBLE);                      
					}
					
					freeIPDRData (pIPDRData);
					break;
 
			case IPDRDATA_HEXADECARRAY: 
			  		if(pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ != 0) {
			  			pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = 
														read_long(pIPDRCommonParameters->pStreamHandle_); 
			  		}

			  		if(pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
						pIPDRData = (IPDRData *) malloc(sizeof(IPDRData));
						for (countVal = 0; countVal < pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_; countVal++) {
							pIPDRData->IPDRDataFlag_ = IPDRDATA_HEXADECARRAY;
							byteArraylength = 
								read_long(pIPDRCommonParameters->pStreamHandle_);
							pIPDRData->IPDRDataUnion_.IPDRByteArray_ = 
								read_octet_array(byteArraylength, 										 
													pIPDRCommonParameters->pStreamHandle_, pIPDRCommonParameters->schemaVersion);
							dataValue[countVal] = (void *) calloc (1, (byteArraylength+1));
      					strcpy((char *)dataValue[countVal], (char *)pIPDRData->IPDRDataUnion_.IPDRByteArray_);
						}
						dataType[0] = IPDRDATA_ARRAY;
						dataType[1] = IPDRDATA_HEXADECARRAY;
      	 			appendListIPDRDataComplex(&((pFNFData)->pListIPDRData_), dataValue, dataType);
					} else {
						pIPDRData = (IPDRData *) malloc(sizeof(IPDRData));
						pIPDRData->IPDRDataFlag_ = IPDRDATA_HEXADECARRAY;
						byteArraylength = 
							read_long(pIPDRCommonParameters->pStreamHandle_);
						pIPDRData->IPDRDataUnion_.IPDRByteArray_ = 
							read_octet_array(byteArraylength, 										 
												pIPDRCommonParameters->pStreamHandle_, pIPDRCommonParameters->schemaVersion);
					
						appendListIPDRData(&pFNFData->pListIPDRData_, 
											pIPDRData->IPDRDataUnion_.IPDRByteArray_, 
											IPDRDATA_HEXADECARRAY);
					}
					
					freeIPDRData (pIPDRData);
					break; 
	        
			case IPDRDATA_STRING: 
			  		if(pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ != 0) {
			  			pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = 
														read_long(pIPDRCommonParameters->pStreamHandle_); 
			  		}

			  		if(pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
						for (countVal = 0; countVal < pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_; countVal++) {
							utfArrayLength = 
								read_long(pIPDRCommonParameters->pStreamHandle_);
				   		tempUTFArray = (wchar *) read_wstring(utfArrayLength, pIPDRCommonParameters->pStreamHandle_, pIPDRCommonParameters->schemaVersion);
							dataValue[countVal] = (void *) calloc (1,  (utfArrayLength+1));
      					strcpy((char *)dataValue[countVal], (char *)tempUTFArray);
						}
						dataType[0] = IPDRDATA_ARRAY;
						dataType[1] = IPDRDATA_STRING;
      	 			appendListIPDRDataComplex(&((pFNFData)->pListIPDRData_), dataValue, dataType);
					} else {
						utfArrayLength = 
							read_long(pIPDRCommonParameters->pStreamHandle_);
				   	tempUTFArray = (wchar *) read_wstring(utfArrayLength, pIPDRCommonParameters->pStreamHandle_, pIPDRCommonParameters->schemaVersion);
					
						appendListIPDRData(&pFNFData->pListIPDRData_, 
											tempUTFArray, 
											IPDRDATA_STRING);
					}

					free(tempUTFArray); 
					break;
					
			case IPDRDATA_BOOLEAN: 
			  		if(pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ != 0) {
			  			pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = 
														read_long(pIPDRCommonParameters->pStreamHandle_); 
			  		}

			  		if(pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
						for (countVal = 0; countVal < pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_; countVal++) {
      					/*dataValue[countVal] = (void *)read_long(pIPDRCommonParameters->pStreamHandle_);*/
      					dataValue[countVal] = (void *)read_boolean(pIPDRCommonParameters->pStreamHandle_);
						}
						dataType[0] = IPDRDATA_ARRAY;
						dataType[1] = IPDRDATA_BOOLEAN;
      	 			appendListIPDRDataComplex(&((pFNFData)->pListIPDRData_), dataValue, dataType);
					} else {
						boolValue[0] = '\0';
						/*int* value = (void *) read_long(pIPDRCommonParameters->pStreamHandle_);*/
						value = (void *) read_boolean(pIPDRCommonParameters->pStreamHandle_);
						sprintf(boolValue, "%d", value);
						appendListIPDRData(&pFNFData->pListIPDRData_, 
						boolValue,
						IPDRDATA_BOOLEAN);  
					}
					break;

			case IPDRDATA_SHORT: 
			  		if(pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ != 0) {
			  			pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = 
														read_long(pIPDRCommonParameters->pStreamHandle_); 
			  		}

			  		if(pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
						for (countVal = 0; countVal < pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_; countVal++) {
      					/*dataValue[countVal] = (void *)read_long(pIPDRCommonParameters->pStreamHandle_);*/
      					dataValue[countVal] = (void *)read_short(pIPDRCommonParameters->pStreamHandle_);
						}
						dataType[0] = IPDRDATA_ARRAY;
						dataType[1] = IPDRDATA_SHORT;
      	 			appendListIPDRDataComplex(&((pFNFData)->pListIPDRData_), dataValue, dataType);
					} else {
						/*appendListIPDRData(&pFNFData->pListIPDRData_, 
						(void *) read_long(pIPDRCommonParameters->pStreamHandle_), 
						IPDRDATA_SHORT);*/
						appendListIPDRData(&pFNFData->pListIPDRData_, 
						(void *) read_short(pIPDRCommonParameters->pStreamHandle_), 
						IPDRDATA_SHORT);
					}
					break;

			case IPDRDATA_USHORT: 
			  		if(pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ != 0) {
			  			pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = 
														read_long(pIPDRCommonParameters->pStreamHandle_); 
			  		}

			  		if(pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
						for (countVal = 0; countVal < pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_; countVal++) {
      					dataValue[countVal] = (void *)read_u_long(pIPDRCommonParameters->pStreamHandle_);
      					/*dataValue[countVal] = (void *)read_u_short(pIPDRCommonParameters->pStreamHandle_);*/
						}
						dataType[0] = IPDRDATA_ARRAY;
						dataType[1] = IPDRDATA_USHORT;
      	 			appendListIPDRDataComplex(&((pFNFData)->pListIPDRData_), dataValue, dataType);
					} else {
						appendListIPDRData(&pFNFData->pListIPDRData_, 
						(void *) read_u_long(pIPDRCommonParameters->pStreamHandle_), 
						IPDRDATA_USHORT);  
						/*appendListIPDRData(&pFNFData->pListIPDRData_, 
						(void *) read_u_short(pIPDRCommonParameters->pStreamHandle_), 
						IPDRDATA_USHORT);*/
					}
					break;
			case IPDRDATA_BYTE: 
			  		if(pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ != 0) {
			  			pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = 
														read_long(pIPDRCommonParameters->pStreamHandle_); 
			  		}

			  		if(pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
						for (countVal = 0; countVal < pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_; countVal++) {
      					dataValue[countVal] = (void *)read_byte(pIPDRCommonParameters->pStreamHandle_);
						}
						dataType[0] = IPDRDATA_ARRAY;
						dataType[1] = IPDRDATA_BYTE;
      	 			appendListIPDRDataComplex(&((pFNFData)->pListIPDRData_), dataValue, dataType);
					} else {
						appendListIPDRData(&pFNFData->pListIPDRData_, 
						(void *) read_byte(pIPDRCommonParameters->pStreamHandle_), 
						IPDRDATA_BYTE);  
					}
					break;
			case IPDRDATA_UBYTE: 
			  		if(pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ != 0) {
			  			pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ = 
														read_long(pIPDRCommonParameters->pStreamHandle_); 
			  		}

			  		if(pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ > 0) {
						for (countVal = 0; countVal < pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_; countVal++) {
      					/*dataValue[countVal] = (void *)read_byte(pIPDRCommonParameters->pStreamHandle_);*/
      					dataValue[countVal] = (void *)read_long(pIPDRCommonParameters->pStreamHandle_);
						}
						dataType[0] = IPDRDATA_ARRAY;
						dataType[1] = IPDRDATA_LONG;
      	 			appendListIPDRDataComplex(&((pFNFData)->pListIPDRData_), dataValue, dataType);
					} else {
						/*appendListIPDRData(&pFNFData->pListIPDRData_, 
						(void *) read_byte(pIPDRCommonParameters->pStreamHandle_), 
						IPDRDATA_BYTE);*/
						appendListIPDRData(&pFNFData->pListIPDRData_, 
						(void *) read_long(pIPDRCommonParameters->pStreamHandle_), 
						IPDRDATA_LONG);
					}
					break;
					
			default:
					*pErrorCode = IPDR_NON_PRIMITIVE_DATA_TYPE;
					return (IPDR_ERROR);
					break;
        } 
		
		pRecordDescriptor->pListAttributeDescriptor_ = 
			pRecordDescriptor->pListAttributeDescriptor_->pNext_;  
	} 

	pRecordDescriptor->pListAttributeDescriptor_ = pCurrent;
	
	freeRecordDescriptor(pRecordDescriptor);
		
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      readComplexFNFDataHelper() - This function returns IPDRComplex structure
 *  DESCRIPTION:
 *      This function:
 *	     -Checks for IPDR_INDEFINITE_LENGTH_INDICATOR marker
 *		 -Based on the attribute type, calls unmarsheller functions to read the data 
 *         values from XDR document.
 *  ARGUMENTS:
 *		- IPDRCommonParameters (IN/OUT)
 *		- RecordDescriptor (IN)
 *		- ListIPDRComplex (OUT)
 *    - ComplexType (IN)
 *    - descriptorID (IN)
 *  RETURNS 	
 *	     Returns Zero or appropriate error code
*/
int readComplexFNFDataHelper(char* pComplexType,
					  					IPDRCommonParameters *pIPDRCommonParameters,
					  					RecordDescriptor* pRecordDescriptor, 
					  					ListIPDRComplex** pListIPDRComplex, 
					  					int* pErrorCode
									)
{
	int count = 0;
	int countNext = 0;
   int cnt = 0;
	int lreturn = IPDR_ERROR;
	int errorSeverity = 0;
	int attributeDescriptorLength = 0;
	int typeCode = 0;
	int noOfElements = 0;
	int byteArraylength = 0;
	int utfArrayLength = 0;
	char* tempComplexType;
   char * tempString;
	char* resultString;
	char* serviceTypeToken = NULL;
	void* dataValue[MAX_IPDR_STRING];
 	ListAttributeDescriptor* pCurrentListServiceAttributeDescriptor = NULL;
 	ListAttributeDescriptor* pOldListServiceAttributeDescriptor = NULL;
 	ListAttributeDescriptor* pTempOldListServiceAttributeDescriptor = NULL;
 	ListAttributeDescriptor* pCurrServiceList = NULL;
	List* pcurrentMaxOccrsList = NULL;
	List* pcurrent = NULL;
	IPDRData* pIPDRData; 
	hyper* tempHyper;
	u_hyper* tempUHyper;
	wchar* tempUTFArray;

	memset(dataValue, 0, sizeof(dataValue));

	if(pIPDRCommonParameters == NULL || pRecordDescriptor == NULL) {
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		return(IPDR_ERROR);
	}

	attributeDescriptorLength = getLengthListAttributeDescriptor(pRecordDescriptor->pListServiceAttributeDescriptor_);

	tempString = (char *) calloc(strlen(pComplexType) + 1, sizeof(char));
	tempComplexType = (char *) calloc(strlen(pComplexType) + 1, sizeof(char));
	strcpy(tempString, pComplexType);
	serviceTypeToken = strtok(tempString, ":");
	
	if(strcmp(pComplexType,serviceTypeToken)) {
		getIPDRSubstring(pComplexType, tempComplexType, ':');
   	strcpy(pComplexType,tempComplexType);
	}	
	free(tempString);
	free(tempComplexType);

	pCurrentListServiceAttributeDescriptor = pRecordDescriptor->pListServiceAttributeDescriptor_;
   pOldListServiceAttributeDescriptor = pRecordDescriptor->pListServiceAttributeDescriptor_;
   pTempOldListServiceAttributeDescriptor = pRecordDescriptor->pListServiceAttributeDescriptor_;


	for (count = 0; count < attributeDescriptorLength; count++) {
		if(strcmp(pComplexType, 
					 pRecordDescriptor->pListServiceAttributeDescriptor_->pAttributeDescriptor_->attributeName_) == 0) {
			pcurrent = pRecordDescriptor->pListServiceAttributeDescriptor_->pAttributeDescriptor_->pListAttrNameList_;
			if(pRecordDescriptor->pListServiceAttributeDescriptor_->pAttributeDescriptor_->pListAttrMaxOccursList_ != NULL) {
				pcurrentMaxOccrsList = pRecordDescriptor->pListServiceAttributeDescriptor_->pAttributeDescriptor_->pListAttrMaxOccursList_;
			}
			while((pcurrent != NULL))	{
				pOldListServiceAttributeDescriptor = pTempOldListServiceAttributeDescriptor;
				for (countNext = 0; countNext < attributeDescriptorLength; countNext++) {
				if(strcmp(pcurrent->node, 
					   pOldListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_) == 0) {

					if(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_[0] != '\0') {
						tempString = (char *) calloc(strlen(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_) + 1, sizeof(char));
						tempComplexType = (char *) calloc(strlen(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_) + 1, sizeof(char));
						strcpy(tempString, pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_);
						serviceTypeToken = strtok(tempString, ":");
	
					if(strcmp(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_,serviceTypeToken)) {
						getIPDRSubstring(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_, 
								tempComplexType, ':');
   					strcpy(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_,tempComplexType);
					}	
					free(tempString);
					free(tempComplexType);
				}
					if((lreturn = isPrimitiveComplexType(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_)) == IPDR_TRUE) {

					typeCode = convAttributeTypeToTypeCode(
											pOldListServiceAttributeDescriptor->pAttributeDescriptor_->attributeType_);

              		switch (typeCode)
              		{
              			case IPDRDATA_LONG:

              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "0") != 0) {
              						noOfElements = read_long(pIPDRCommonParameters->pStreamHandle_); 
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
      								dataValue[cnt] = (void *)read_long(pIPDRCommonParameters->pStreamHandle_);
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_LONG);
              				}
              				else {
									dataValue[0] = (void *) read_long(pIPDRCommonParameters->pStreamHandle_);
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_LONG);
              				}
                        break;
              
              			case IPDRDATA_ULONG: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "0") != 0) {
              						noOfElements = read_long(pIPDRCommonParameters->pStreamHandle_);
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
      								dataValue[cnt] = (void *)read_u_long(pIPDRCommonParameters->pStreamHandle_);
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_ULONG);
              				}
              				else {
									dataValue[0] = (void *) read_u_long(pIPDRCommonParameters->pStreamHandle_);
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_ULONG);
              				}
                        break;
              
                       case IPDRDATA_LONGLONG: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "0") != 0) {
              						noOfElements = read_long(pIPDRCommonParameters->pStreamHandle_);
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
									pIPDRData = newIPDRData(IPDRDATA_LONGLONG); 
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										tempHyper = read_longlong(pIPDRCommonParameters->pStreamHandle_);
				   					pIPDRData->IPDRDataUnion_.IPDRLongLong_ = (hyper) *tempHyper;
      								dataValue[cnt] = (void *)&(pIPDRData->IPDRDataUnion_.IPDRLongLong_);
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_LONGLONG);
              				}
              				else {
									pIPDRData = newIPDRData(IPDRDATA_LONGLONG); 
									tempHyper = read_longlong(pIPDRCommonParameters->pStreamHandle_);
				   				pIPDRData->IPDRDataUnion_.IPDRLongLong_ = (hyper) *tempHyper;
      							dataValue[0] = (void *)&(pIPDRData->IPDRDataUnion_.IPDRLongLong_);
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_LONGLONG);
              				}
								free(tempHyper);
								freeIPDRData (pIPDRData);
                         break;
              
                       case IPDRDATA_ULONGLONG: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "0") != 0) {
              						noOfElements = read_long(pIPDRCommonParameters->pStreamHandle_);
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
									pIPDRData = newIPDRData(IPDRDATA_ULONGLONG); 
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										tempUHyper = read_u_longlong(pIPDRCommonParameters->pStreamHandle_);
				   					pIPDRData->IPDRDataUnion_.IPDRULongLong_ = (hyper) *tempUHyper;
      								dataValue[cnt] = (void *)&(pIPDRData->IPDRDataUnion_.IPDRULongLong_);
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_ULONGLONG);
              				}
              				else {
									pIPDRData = newIPDRData(IPDRDATA_ULONGLONG); 
									tempUHyper = read_u_longlong(pIPDRCommonParameters->pStreamHandle_);
				   				pIPDRData->IPDRDataUnion_.IPDRULongLong_ = (hyper) *tempUHyper;
      							dataValue[0] = (void *)&(pIPDRData->IPDRDataUnion_.IPDRULongLong_);
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_ULONGLONG);
              				}
								free(tempUHyper);
								freeIPDRData (pIPDRData);
                        break;
              
                       case IPDRDATA_FLOAT: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "0") != 0) {
              						noOfElements = read_long(pIPDRCommonParameters->pStreamHandle_);
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
									pIPDRData = newIPDRData(IPDRDATA_FLOAT); 
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
				   					pIPDRData->IPDRDataUnion_.IPDRFloat_ = 
															read_float(pIPDRCommonParameters->pStreamHandle_);
      								dataValue[cnt] = (float *) &(pIPDRData->IPDRDataUnion_.IPDRFloat_);
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_FLOAT);
              				}
              				else {
									pIPDRData = newIPDRData(IPDRDATA_FLOAT); 
				   				pIPDRData->IPDRDataUnion_.IPDRFloat_ = 
															read_float(pIPDRCommonParameters->pStreamHandle_);
      							dataValue[0] = (void *)&(pIPDRData->IPDRDataUnion_.IPDRFloat_);
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_FLOAT);
              				}
								freeIPDRData (pIPDRData);
              				break;
              
                       case IPDRDATA_DOUBLE: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "0") != 0) {
              						noOfElements = read_long(pIPDRCommonParameters->pStreamHandle_);
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
									pIPDRData = newIPDRData(IPDRDATA_DOUBLE); 
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
				   					pIPDRData->IPDRDataUnion_.IPDRDouble_ = 
															read_float(pIPDRCommonParameters->pStreamHandle_);
      								dataValue[cnt] = (void *)&(pIPDRData->IPDRDataUnion_.IPDRDouble_);
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_DOUBLE);
              				}
              				else {
									pIPDRData = newIPDRData(IPDRDATA_DOUBLE); 
				   				pIPDRData->IPDRDataUnion_.IPDRDouble_ = 
															read_float(pIPDRCommonParameters->pStreamHandle_);
      							dataValue[0] = (void *)&(pIPDRData->IPDRDataUnion_.IPDRDouble_);
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_DOUBLE);
              				}
								freeIPDRData (pIPDRData);
              				break;
              
                       case IPDRDATA_HEXADECARRAY: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "0") != 0) {
              						noOfElements = read_long(pIPDRCommonParameters->pStreamHandle_);
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
									pIPDRData = (IPDRData *) malloc(sizeof(IPDRData));
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										pIPDRData->IPDRDataFlag_ = IPDRDATA_HEXADECARRAY;
										byteArraylength = 
												read_long(pIPDRCommonParameters->pStreamHandle_);
										pIPDRData->IPDRDataUnion_.IPDRByteArray_ = 
												read_octet_array(byteArraylength, 										 
														pIPDRCommonParameters->pStreamHandle_, pIPDRCommonParameters->schemaVersion);
										dataValue[cnt] = (void *) calloc (1, (byteArraylength+1));
      								strcpy((char *)dataValue[cnt], (char *)pIPDRData->IPDRDataUnion_.IPDRByteArray_);
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_HEXADECARRAY);
              				}
              				else {
									pIPDRData = (IPDRData *) malloc(sizeof(IPDRData));
									pIPDRData->IPDRDataFlag_ = IPDRDATA_HEXADECARRAY;
									byteArraylength = 
											read_long(pIPDRCommonParameters->pStreamHandle_);
									pIPDRData->IPDRDataUnion_.IPDRByteArray_ = 
											read_octet_array(byteArraylength, 										 
														pIPDRCommonParameters->pStreamHandle_, pIPDRCommonParameters->schemaVersion);
									dataValue[0] = (void *) calloc (1, (byteArraylength+1));
      							strcpy((char *)dataValue[0], (char *)pIPDRData->IPDRDataUnion_.IPDRByteArray_);
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_HEXADECARRAY);
              				}
								freeIPDRData (pIPDRData);
              				break;
              
                       case IPDRDATA_STRING: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "0") != 0) {
              						noOfElements = read_long(pIPDRCommonParameters->pStreamHandle_);
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										utfArrayLength = 
											read_long(pIPDRCommonParameters->pStreamHandle_);
				   					tempUTFArray = 
											(wchar *) read_wstring(utfArrayLength, pIPDRCommonParameters->pStreamHandle_, pIPDRCommonParameters->schemaVersion);
										dataValue[cnt] = (void *) calloc (1,  (utfArrayLength+1));
      								strcpy((char *)dataValue[cnt], (char *)tempUTFArray);
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_STRING);
              				}
              				else {
									utfArrayLength = 
											read_long(pIPDRCommonParameters->pStreamHandle_);
				   				tempUTFArray = 
										(wchar *) read_wstring(utfArrayLength, pIPDRCommonParameters->pStreamHandle_, pIPDRCommonParameters->schemaVersion);
									dataValue[0] = (void *) calloc (1,  (utfArrayLength+1));
      							strcpy((char *)dataValue[0], (char *)tempUTFArray);
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_STRING);
              				}
								free(tempUTFArray); 
              				break;
              			
                       case IPDRDATA_BOOLEAN: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "0") != 0) {
              						noOfElements = read_long(pIPDRCommonParameters->pStreamHandle_);
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
      								dataValue[cnt] = (void *)read_long(pIPDRCommonParameters->pStreamHandle_);
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_BOOLEAN);
              				}
              				else {
      								dataValue[0] = (void *)read_long(pIPDRCommonParameters->pStreamHandle_);
										appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_BOOLEAN);
              				}
              				break;
              
              			case IPDRDATA_SHORT: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "0") != 0) {
              						noOfElements = read_long(pIPDRCommonParameters->pStreamHandle_);
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
      								/*dataValue[cnt] = (void *)read_long(pIPDRCommonParameters->pStreamHandle_);*/
      								dataValue[cnt] = (void *)read_short(pIPDRCommonParameters->pStreamHandle_);
										appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_SHORT);
              					}
              				}
              				else {
      								/*dataValue[0] = (void *)read_long(pIPDRCommonParameters->pStreamHandle_);*/
      								dataValue[0] = (void *)read_short(pIPDRCommonParameters->pStreamHandle_);
										appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_SHORT);
              				}
                        break;
              
              			case IPDRDATA_USHORT: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "0") != 0) {
              						noOfElements = read_long(pIPDRCommonParameters->pStreamHandle_);
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
      								dataValue[cnt] = (void *)read_u_long(pIPDRCommonParameters->pStreamHandle_);
										appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_USHORT);
              					}
              				}
              				else {
      								dataValue[0] = (void *)read_u_long(pIPDRCommonParameters->pStreamHandle_);
										appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_USHORT);
              				}
                        break;

              			case IPDRDATA_BYTE:

              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "0") != 0) {
              						noOfElements = read_long(pIPDRCommonParameters->pStreamHandle_); 
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
      								dataValue[cnt] = (void *)read_byte(pIPDRCommonParameters->pStreamHandle_);
              					}
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_LONG);
              				}
              				else {
									dataValue[0] = (void *) read_byte(pIPDRCommonParameters->pStreamHandle_);
									appendListIPDRComplex(&(*pListIPDRComplex), dataValue, IPDRDATA_LONG);
              				}
                        break;
              
              			default:
              					return(IPDR_NON_PRIMITIVE_DATA_TYPE);
              					break;
                      }
				  break;
				} else if(lreturn == IPDR_FALSE){
					pCurrServiceList = pTempOldListServiceAttributeDescriptor;
				   pRecordDescriptor->pListServiceAttributeDescriptor_ = pTempOldListServiceAttributeDescriptor;	
					/*pListIPDRComplex = NULL;*/
					readComplexFNFDataHelper(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_,
					  						 		 pIPDRCommonParameters,
					  						       pRecordDescriptor, 
					  						       pListIPDRComplex, 
					                         pErrorCode
											      );
				}
			}
			pOldListServiceAttributeDescriptor = pOldListServiceAttributeDescriptor->pNext_;
		}
					pcurrent = pcurrent->pNext;
					if(pcurrentMaxOccrsList != NULL)
						pcurrentMaxOccrsList = pcurrentMaxOccrsList->pNext;
			}
		}		
		pRecordDescriptor->pListServiceAttributeDescriptor_ = pRecordDescriptor->pListServiceAttributeDescriptor_->pNext_;
	}
	
   pRecordDescriptor->pListServiceAttributeDescriptor_ = pCurrentListServiceAttributeDescriptor;


	return (IPDR_OK);

}


/*!
 *  NAME:
 *      getByteArrayLength() - function for returning length of Attribute Name.
 *
 *  DESCRIPTION:
 *      1. This function returns length of Attribute Name.
 *	
 *  ARGUMENTS:
 *       -	byte Array {IN/OUT}
 *
 *  RETURNS 	
 *	     Returns length of input Attribute Name.
 */

int getByteArrayLength(byte* byteArray)
{
	int count = 0;
	while (byteArray[count] != '\0') {
		count++;
	}
	return count;
}

/*!
 *  NAME:
 *      writeIPDRXDRComplexRecordHelper() - writes the complex XDR equivalent of FNFData structure on 
 *                             output stream
 *
 *  DESCRIPTION:
 *      - This function will be used to write an FNFData structure to the output stream.
 *		- This function internally uses the basic write functions (eg write_long()) to 
 *		  actually marshall the data in the XDR format to the output stream.
 *
 *  ARGUMENTS:
 *     - IPDRCommonParameters (IN/OUT)
 *	    - FNFData {IN}
 *     - ComplexType (IN)
 *
 *  RETURNS 	
 *	    Returns Zero or appropriate error code
*/
writeIPDRXDRComplexRecordHelper(FNFData *pFNFData,char *pComplexType,IPDRCommonParameters* pIPDRCommonParameters,
										  int* pErrorCode)
{
	int count = 0;
	int countNext = 0;
   int cnt = 0;
	int errorSeverity = 0;
	int attributeDescriptorLength = 0;
	int typeCode = 0;
   int finalListFlag = 0;
	int noOfElements = 0;
	int byteArrayLength = 0;
	char* tempComplexType;
   char * tempString;
	char* resultString;
	char* serviceTypeToken = NULL;
 	ListIPDRData* pCurrentListIPDRData = NULL;
 	ListAttributeDescriptor* pCurrentListServiceAttributeDescriptor = NULL;
 	ListAttributeDescriptor* pOldListServiceAttributeDescriptor = NULL;
 	ListAttributeDescriptor* pTempOldListServiceAttributeDescriptor = NULL;
 	ListAttributeDescriptor* pCurrServiceList = NULL;
   ListIPDRComplexData* pTempListIPDRComplexData = NULL;
   ListIPDRComplex* pTempListIPDRComplex = NULL;
   ListIPDRArrayData* pCurrentListIPDRArrayData = NULL;
	List* pcurrentMaxOccrsList = NULL;
	List* pcurrent = NULL;

	if(pFNFData == NULL || pIPDRCommonParameters == NULL) {
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		return(IPDR_ERROR);
	}

	attributeDescriptorLength = getLengthListAttributeDescriptor(pFNFData->pListServiceAttributeDescriptor_);

	tempString = (char *) calloc(strlen(pComplexType) + 1, sizeof(char));
	tempComplexType = (char *) calloc(strlen(pComplexType) + 1, sizeof(char));
	strcpy(tempString, pComplexType);
	serviceTypeToken = strtok(tempString, ":");
	
	if(strcmp(pComplexType,serviceTypeToken)) {
		getIPDRSubstring(pComplexType, tempComplexType, ':');
   	strcpy(pComplexType,tempComplexType);
	}	
	free(tempString);
	free(tempComplexType);

	pCurrentListServiceAttributeDescriptor = pFNFData->pListServiceAttributeDescriptor_;
   pOldListServiceAttributeDescriptor = pFNFData->pListServiceAttributeDescriptor_;
   pTempOldListServiceAttributeDescriptor = pFNFData->pListServiceAttributeDescriptor_;
   pTempListIPDRComplex = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_-> pListIPDRComplex_;


	for (count = 0; count < attributeDescriptorLength; count++) {
		if(strcmp(pComplexType, 
					 pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->attributeName_) == 0) {
			pcurrent = pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->pListAttrNameList_;
			if(pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->pListAttrMaxOccursList_ != NULL) {
				pcurrentMaxOccrsList = pFNFData->pListServiceAttributeDescriptor_->pAttributeDescriptor_->pListAttrMaxOccursList_;
			}
			while((pcurrent != NULL))	{
				pOldListServiceAttributeDescriptor = pTempOldListServiceAttributeDescriptor;
				for (countNext = 0; countNext < attributeDescriptorLength; countNext++) {
				if(strcmp(pcurrent->node, 
					   pOldListServiceAttributeDescriptor->pAttributeDescriptor_->attributeName_) == 0) {
						tempString = (char *) calloc(strlen(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_) + 1, sizeof(char));
						tempComplexType = (char *) calloc(strlen(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_) + 1, sizeof(char));
						strcpy(tempString, pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_);
						serviceTypeToken = strtok(tempString, ":");
	
				if(strcmp(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_,serviceTypeToken)) {
					getIPDRSubstring(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_, 
								tempComplexType, ':');
   				strcpy(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_,tempComplexType);
				}	
				free(tempString);
				free(tempComplexType);
					if(isPrimitiveComplexType(pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_) == IPDR_TRUE) {

					typeCode = convAttributeTypeToTypeCode(
											pOldListServiceAttributeDescriptor->pAttributeDescriptor_->attributeType_);

              		switch (typeCode)
              		{
              			case IPDRDATA_LONG:

              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									write_long(noOfElements, pIPDRCommonParameters->pStreamHandle_);
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										write_long((int) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRLong_, 
													  pIPDRCommonParameters->pStreamHandle_);
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
              				}
              				else {
										write_long((int) pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRLong_, 
													  pIPDRCommonParameters->pStreamHandle_);
              				}
                        break;
              
              			case IPDRDATA_ULONG: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									write_long(noOfElements, pIPDRCommonParameters->pStreamHandle_);
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
										write_u_long((unsigned int) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRULong_, 
														 pIPDRCommonParameters->pStreamHandle_);

              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
              				}
              				else {
									write_u_long((unsigned int) pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRULong_, 
														 pIPDRCommonParameters->pStreamHandle_);
              				}
                        break;
              
                       case IPDRDATA_LONGLONG: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									write_long(noOfElements, pIPDRCommonParameters->pStreamHandle_);
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										write_longlong((hyper) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRLongLong_, 
															pIPDRCommonParameters->pStreamHandle_);
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
              				}
              				else {
										write_longlong((hyper) pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRLongLong_, 
															pIPDRCommonParameters->pStreamHandle_);
              				}
                         break;
              
                       case IPDRDATA_ULONGLONG: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									write_long(noOfElements, pIPDRCommonParameters->pStreamHandle_);
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										write_u_longlong((u_hyper) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRULongLong_, 
													pIPDRCommonParameters->pStreamHandle_);

              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
              				}
              				else {
										write_u_longlong((u_hyper) pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRULongLong_, 
													pIPDRCommonParameters->pStreamHandle_);
              				}
                        break;
              
                       case IPDRDATA_FLOAT: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									write_long(noOfElements, pIPDRCommonParameters->pStreamHandle_);
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										write_float((float) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRFloat_, 
											pIPDRCommonParameters->pStreamHandle_);

              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
              				}
              				else {
										write_float((float) pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRFloat_, 
											pIPDRCommonParameters->pStreamHandle_);
              				}
              				break;
              
                       case IPDRDATA_DOUBLE: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									write_long(noOfElements, pIPDRCommonParameters->pStreamHandle_);
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
											write_double((double) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRDouble_, 
											 pIPDRCommonParameters->pStreamHandle_);

              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
              				}
              				else {
									write_double((double) pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRDouble_, 
											 pIPDRCommonParameters->pStreamHandle_);
              				}
              				break;
              
                       case IPDRDATA_HEXADECARRAY: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									write_long(noOfElements, pIPDRCommonParameters->pStreamHandle_);
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										byteArrayLength = getByteArrayLength(pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRByteArray_);
										write_long(byteArrayLength, pIPDRCommonParameters->pStreamHandle_);
										write_noctet_array(pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRByteArray_, pIPDRCommonParameters->pStreamHandle_, byteArrayLength, pIPDRCommonParameters->schemaVersion);     

              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
              				}
              				else {
										byteArrayLength = getByteArrayLength(pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRByteArray_);
										write_long(byteArrayLength, pIPDRCommonParameters->pStreamHandle_);
										write_noctet_array(pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRByteArray_, pIPDRCommonParameters->pStreamHandle_, byteArrayLength, pIPDRCommonParameters->schemaVersion);     
              				}
              				break;
              
                       case IPDRDATA_STRING: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									write_long(noOfElements, pIPDRCommonParameters->pStreamHandle_);
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										write_long(strlen((char *) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_), pIPDRCommonParameters->pStreamHandle_);
				   					write_wstring((wchar *)pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUTF8Array_, pIPDRCommonParameters->pStreamHandle_, pIPDRCommonParameters->schemaVersion);
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
              				}
              				else {
										write_long(strlen((char *) pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_), pIPDRCommonParameters->pStreamHandle_);
				   					write_wstring((wchar *)pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUTF8Array_, pIPDRCommonParameters->pStreamHandle_, pIPDRCommonParameters->schemaVersion);
              				}
              				break;
              			
                       case IPDRDATA_BOOLEAN: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									write_long(noOfElements, pIPDRCommonParameters->pStreamHandle_);
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										if(strcmp(pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRBoolean_, "0") == 0) { 
											write_boolean("false", pIPDRCommonParameters->pStreamHandle_);
											/*write_long((int) 0, pIPDRCommonParameters->pStreamHandle_);*/
										} else if(strcmp(pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRBoolean_, "1 ") == 0) {
											write_boolean("true", pIPDRCommonParameters->pStreamHandle_);
											/*write_long((int) 1, pIPDRCommonParameters->pStreamHandle_);*/
										}
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
              				}
              				else {
										if(strcmp(pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRBoolean_, "0")) { 
											write_boolean("false", pIPDRCommonParameters->pStreamHandle_);
											/*write_long((int) 0, pIPDRCommonParameters->pStreamHandle_);*/
										} else if(strcmp(pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRBoolean_, "1")) {
											write_boolean("true", pIPDRCommonParameters->pStreamHandle_);
											/*write_long((int) 1, pIPDRCommonParameters->pStreamHandle_);*/
										}
              				}
              				break;
              
              			case IPDRDATA_SHORT: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									write_long(noOfElements, pIPDRCommonParameters->pStreamHandle_);
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										/*write_long((int)pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRShort_, 
											pIPDRCommonParameters->pStreamHandle_);*/
										write_short((short int)pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRShort_, 
											pIPDRCommonParameters->pStreamHandle_);

              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
              				}
              				else {
										/*write_long((int)pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRShort_, 
											pIPDRCommonParameters->pStreamHandle_);*/
										write_short((short int)pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRShort_, 
											pIPDRCommonParameters->pStreamHandle_);
              				}
                        break;
              
              			case IPDRDATA_USHORT: 
              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									write_long(noOfElements, pIPDRCommonParameters->pStreamHandle_);
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										/*write_u_long((unsigned int)pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUShort_, 
											pIPDRCommonParameters->pStreamHandle_);*/
										write_u_short((unsigned int)pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRUShort_, 
											pIPDRCommonParameters->pStreamHandle_);

              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
              				}
              				else {
										/*write_u_long((unsigned int)pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUShort_, 
											pIPDRCommonParameters->pStreamHandle_);*/
										write_u_short((unsigned int)pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRUShort_, 
											pIPDRCommonParameters->pStreamHandle_);
              				}
                        break;

              			case IPDRDATA_BYTE:

              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									write_long(noOfElements, pIPDRCommonParameters->pStreamHandle_);
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										write_byte((int) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRByte_, 
													  pIPDRCommonParameters->pStreamHandle_);
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
              				}
              				else {
										write_byte((int) pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRByte_, 
													  pIPDRCommonParameters->pStreamHandle_);
              				}
                        break;

							case IPDRDATA_UBYTE: 

              				if((pcurrentMaxOccrsList != NULL)) {
									if(strcmp(pcurrentMaxOccrsList->node, "-1") == 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									}
									else {
              						noOfElements = atoi(pcurrentMaxOccrsList->node);
									}
              				}
              
              				if (noOfElements > 0) {
              						noOfElements = 
									pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->noOfElements_;
									write_long(noOfElements, pIPDRCommonParameters->pStreamHandle_);
              					pCurrentListIPDRArrayData = pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_;
              					for(cnt = 0;cnt < noOfElements; cnt++) { 
										write_long((int) pCurrentListIPDRArrayData->IPDRDataUnion_.IPDRByte_, 
													  pIPDRCommonParameters->pStreamHandle_);
              					   pCurrentListIPDRArrayData = pCurrentListIPDRArrayData->pNext_;	
              					}
              				}
              				else {
										write_long((int) pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pListIPDRArrayData_->IPDRDataUnion_.IPDRByte_, 
													  pIPDRCommonParameters->pStreamHandle_);
              				}
                        break;
              
              			default:
              					return(IPDR_NON_PRIMITIVE_DATA_TYPE);
              					break;
                      }
				}
				else {
					pCurrServiceList = pTempOldListServiceAttributeDescriptor;
				   pFNFData->pListServiceAttributeDescriptor_ = pTempOldListServiceAttributeDescriptor;	
					writeIPDRXDRComplexRecordHelper(pFNFData,
													  pOldListServiceAttributeDescriptor->pAttributeDescriptor_->complexType_,
													  pIPDRCommonParameters, pErrorCode);
				}
			}
			pOldListServiceAttributeDescriptor = pOldListServiceAttributeDescriptor->pNext_;
		}
					pcurrent = pcurrent->pNext;
					if(pcurrentMaxOccrsList != NULL)
						pcurrentMaxOccrsList = pcurrentMaxOccrsList->pNext;
					pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_ = 
					pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_->pListIPDRComplex_->pNext_;
			}
		}		
		pFNFData->pListServiceAttributeDescriptor_ = pFNFData->pListServiceAttributeDescriptor_->pNext_;
      pFNFData->pListIPDRData_->pListIPDRStructureData_->pListIPDRComplexData_-> pListIPDRComplex_ = pTempListIPDRComplex;
	}
	
   pFNFData->pListServiceAttributeDescriptor_ = pCurrentListServiceAttributeDescriptor;


	return (IPDR_OK);
}
