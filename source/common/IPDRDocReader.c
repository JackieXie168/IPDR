/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/

/*****************************************************
 * File                 : IPDRDocReader              *
 * Description          :                            *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 *                                                   *
 *****************************************************/

#include "common/IPDRDocReader.h"

/*!
 *  NAME:
 *      readHeader() - 	reads the IPDR Doc header & changes the state of the document 
 *
 *  DESCRIPTION:
 *      This function
 *		 -	Calls readIPDRXDRHeaderHelper
 *		 -	Checks for IPDR_INDEFINITE_LENGTH_INDICATOR marker
 *		 -	Makes the document state as READABLE 
 *  
 *  ARGUMENTS:
 *       -	IPDRCommonParameters (IN/OUT)	
 * 		 -	IPDRHeader (IN/OUT)
 *
 *  RETURNS 	
 *	     Returns Zero or appropriate error code
*/

int readHeader(
			   IPDRCommonParameters* pIPDRCommonParameters, 
			   IPDRHeader* pIPDRHeader, 
			   int* pErrorCode
			  )
{			
	int errorSeverity = 0;
	
	if(pIPDRHeader == NULL || pIPDRCommonParameters == NULL) {
	    *pErrorCode = IPDR_NULL_INPUT_PARAMETER;
	    return (IPDR_ERROR);
	}
    
	/* Populate IPDRDoc header structure with values in XDR document*/		
	if(pIPDRCommonParameters->DocType_ == XDR) {
		errorSeverity = readIPDRXDRHeaderHelper(pIPDRCommonParameters, pIPDRHeader, pErrorCode);		
		if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "readIPDRXDRHeaderHelper");	
		}
		else if (errorSeverity == IPDR_ERROR) {
			pIPDRCommonParameters->DocState_ = NONREADABLE; 
			exceptionHandler(pIPDRCommonParameters, pErrorCode, "readIPDRXDRHeaderHelper");	
			return (IPDR_ERROR);
		}
	}
	else if (pIPDRCommonParameters->DocType_ == XML) {
		errorSeverity = readIPDRXMLHeaderHelper(pIPDRCommonParameters, pIPDRHeader, pErrorCode);		
		if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "readIPDRXMLHeaderHelper");	
		}
		else if (errorSeverity == IPDR_ERROR) {
			pIPDRCommonParameters->DocState_ = NONREADABLE; 
			exceptionHandler(pIPDRCommonParameters, pErrorCode, "readIPDRXMLHeaderHelper");	
			return (IPDR_ERROR);
		}
	}
	else {
		*pErrorCode = IPDR_INVALID_DOCTYPE;
		return (IPDR_ERROR);
	}

	pIPDRCommonParameters->DocState_ = READABLE;

	return (IPDR_OK);	
}


/*!
 *  NAME:
 *      readIPDR() - 	reads IPDR data and/or descriptor from the XDR file 
 *
 *  DESCRIPTION:
 *      This function
 *       - Calls readIPDRStreamElementHelper
 *		 - Depending on the value of descriminator, calls following functions
 *		   a. Calls "addDescriptor", if value of descriminator is 1
 *		   b. Increments the value of ipdrCount, if value of descriminator is 2
 *		   c. Returns null to Stream Pointer, if value of descriminator is 3	
 *		 - Calls validateAgainstSchema
 *
 *  ARGUMENTS:
 *       -	IPDRCommonParameters (IN/OUT)	
 * 		 -	FNFData (IN/OUT)
 *
 *  RETURNS 	
 *	     Returns Zero or appropriate error code
*/

int readIPDR(
			 IPDRCommonParameters* pIPDRCommonParameters, 
			 FNFData** pFNFData, 
			 int* pErrorCode
			)
{
	int descriptorID = 0;
	int errorSeverity = 0;
	int descriminator = 0;
	static lFound = 1;

	IPDRStreamElement* pIPDRStreamElement;
	RecordDescriptor* pRecordDescriptor;
	ListAttributeDescriptor* pCurrent;
	ListAttributeDescriptor* pListAttributeDescriptorTemp = NULL;
	static ListAttributeDescriptor* pListServiceAttributeDescriptorTemp = NULL;
	ListIPDRData* pFirst;
	void* IPDRValue =  NULL;
	
	if(pFNFData == NULL || pIPDRCommonParameters == NULL) {
	    *pErrorCode = IPDR_NULL_INPUT_PARAMETER;
	    return (IPDR_ERROR);
	}

	if (pIPDRCommonParameters->DocState_ != READABLE) {
		*pErrorCode = IPDR_INVALID_DOCSTATE_MAKE_READABLE;
		return (IPDR_ERROR);
	}

	if(pIPDRCommonParameters->DocType_ == XDR) {
		descriminator = read_long(pIPDRCommonParameters->pStreamHandle_);  
		
		switch (descriminator)
		{
			case STREAM_RECORD_DESCRIPTOR:
				
				pIPDRStreamElement = 
						newIPDRStreamElement(STREAM_RECORD_DESCRIPTOR);
				
				if(pIPDRStreamElement == NULL) {
					*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
					return(IPDR_ERROR);
				}
				
				pIPDRStreamElement->descriminator_ = descriminator;
				
				errorSeverity = readIPDRStreamElementHelper(pIPDRCommonParameters, pIPDRStreamElement,
																			 pListServiceAttributeDescriptorTemp,
																			pErrorCode);	
				if (errorSeverity == IPDR_WARNING) {
					errorHandler(pIPDRCommonParameters, pErrorCode, "readIPDRStreamElementHelper");	
				}
				else if (errorSeverity == IPDR_ERROR) {
					pIPDRCommonParameters->DocState_ = NONREADABLE; 
					exceptionHandler(pIPDRCommonParameters, pErrorCode, "readIPDRStreamElementHelper");	
					return (IPDR_ERROR);
				}

				errorSeverity = addDescriptor(pIPDRCommonParameters, pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pRecordDescriptor_, pErrorCode);
				if (errorSeverity == IPDR_WARNING) {
					errorHandler(pIPDRCommonParameters, pErrorCode, "addDescriptor");	
				}
				else if (errorSeverity == IPDR_ERROR) {
					pIPDRCommonParameters->DocState_ = NONREADABLE; 
					exceptionHandler(pIPDRCommonParameters, pErrorCode, "addDescriptor");	
					return (IPDR_ERROR);
				}

				pCurrent = pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pRecordDescriptor_->pListServiceAttributeDescriptor_;

				while (pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pRecordDescriptor_->pListServiceAttributeDescriptor_ != NULL) {
					appendListAttributeDescriptor(&(pListServiceAttributeDescriptorTemp),
									pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pRecordDescriptor_->pListServiceAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
									pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pRecordDescriptor_->pListServiceAttributeDescriptor_->pAttributeDescriptor_->attributeType_,
									pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pRecordDescriptor_->pListServiceAttributeDescriptor_->pAttributeDescriptor_->description_,
									pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pRecordDescriptor_->pListServiceAttributeDescriptor_->pAttributeDescriptor_->derivedType_,
								   pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pRecordDescriptor_->pListServiceAttributeDescriptor_->pAttributeDescriptor_->complexType_, 
								   pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pRecordDescriptor_->pListServiceAttributeDescriptor_->pAttributeDescriptor_->pListAttrNameList_, 
								   pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pRecordDescriptor_->pListServiceAttributeDescriptor_->pAttributeDescriptor_->pListAttrClassList_, 
								   pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pRecordDescriptor_->pListServiceAttributeDescriptor_->pAttributeDescriptor_->pListAttrMaxOccursList_, 
								   pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pRecordDescriptor_->pListServiceAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ );
				
					pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pRecordDescriptor_->pListServiceAttributeDescriptor_ = pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pRecordDescriptor_->pListServiceAttributeDescriptor_->pNext_; 
				}

				pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pRecordDescriptor_->pListServiceAttributeDescriptor_ = pCurrent;

				freeIPDRStreamElement (pIPDRStreamElement);

			case STREAM_IPDR_RECORD:

				pRecordDescriptor = newRecordDescriptor();
				pIPDRStreamElement = newIPDRStreamElement(STREAM_IPDR_RECORD);
				
				if(pIPDRStreamElement == NULL || pRecordDescriptor == NULL) {
					*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
					return(IPDR_ERROR);
				}
				
				if(descriminator == 2)
					pIPDRStreamElement->descriminator_ = descriminator;
				/*errorSeverity = readIPDRStreamElementHelper(pIPDRCommonParameters, 
																		  pIPDRStreamElement, 
																		  pErrorCode);	*/
				errorSeverity = readIPDRStreamElementHelper(pIPDRCommonParameters, 
																		  pIPDRStreamElement, 
																		  pListServiceAttributeDescriptorTemp,
																		  pErrorCode);	
				if (errorSeverity == IPDR_WARNING) {
					errorHandler(pIPDRCommonParameters, pErrorCode, "readIPDRStreamElementHelper");	
				}
				else if (errorSeverity == IPDR_ERROR) {
					pIPDRCommonParameters->DocState_ = NONREADABLE; 
					exceptionHandler(pIPDRCommonParameters, pErrorCode, "readIPDRStreamElementHelper");	
					return (IPDR_ERROR);
				}
				
				descriptorID = pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->descriptorID_; 

				errorSeverity = getDescriptor(pIPDRCommonParameters, descriptorID, pRecordDescriptor, pErrorCode);
				if (errorSeverity == IPDR_WARNING) {
					errorHandler(pIPDRCommonParameters, pErrorCode, "getDescriptor");	
				}
				else if (errorSeverity == IPDR_ERROR) {
					pIPDRCommonParameters->DocState_ = NONREADABLE; 
					exceptionHandler(pIPDRCommonParameters, pErrorCode, "getDescriptor");	
					return (IPDR_ERROR);
				}

				/* Populating FNFData */
				pCurrent = pRecordDescriptor->pListAttributeDescriptor_;

				while (pRecordDescriptor->pListAttributeDescriptor_ != NULL) {
					appendListAttributeDescriptor(&((*pFNFData)->pListAttributeDescriptor_),
							pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
							pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->attributeType_,
							pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->description_,
							pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->derivedType_,
							pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->complexType_, 
							pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->pListAttrNameList_, 
							pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->pListAttrClassList_, 
							pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->pListAttrMaxOccursList_, 
							pRecordDescriptor->pListAttributeDescriptor_->pAttributeDescriptor_->noOfElements_ );
				
					pRecordDescriptor->pListAttributeDescriptor_ = pRecordDescriptor->pListAttributeDescriptor_->pNext_; 
				}
				
				pRecordDescriptor->pListAttributeDescriptor_ = pCurrent;

/*if(lFound == 1) {*/
				pCurrent = pListServiceAttributeDescriptorTemp;

				while (pListServiceAttributeDescriptorTemp != NULL) {
					appendListAttributeDescriptor(&((*pFNFData)->pListServiceAttributeDescriptor_),
							pListServiceAttributeDescriptorTemp->pAttributeDescriptor_->attributeName_,
							pListServiceAttributeDescriptorTemp->pAttributeDescriptor_->attributeType_,
							pListServiceAttributeDescriptorTemp->pAttributeDescriptor_->description_,
							pListServiceAttributeDescriptorTemp->pAttributeDescriptor_->derivedType_,
							pListServiceAttributeDescriptorTemp->pAttributeDescriptor_->complexType_, 
							pListServiceAttributeDescriptorTemp->pAttributeDescriptor_->pListAttrNameList_, 
							pListServiceAttributeDescriptorTemp->pAttributeDescriptor_->pListAttrClassList_, 
							pListServiceAttributeDescriptorTemp->pAttributeDescriptor_->pListAttrMaxOccursList_, 
							pListServiceAttributeDescriptorTemp->pAttributeDescriptor_->noOfElements_ );
				
					pListServiceAttributeDescriptorTemp = pListServiceAttributeDescriptorTemp->pNext_; 
				}
				pListServiceAttributeDescriptorTemp = pCurrent;

				lFound = 0;
/*}*/

				strcpy((*pFNFData)->typeName_, pRecordDescriptor->typeName_); 
				
				
				pFirst = pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_;

				while (pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_ != NULL) {
					
				    if (pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_->IPDRDataType_ == IPDRDATA_HEXADECARRAY) {
						  	
						  appendListIPDRData(&((*pFNFData)->pListIPDRData_ ),
										pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_->IPDRDataUnion_.IPDRByteArray_,
										IPDRDATA_HEXADECARRAY);
					}
					else if (pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_->IPDRDataType_ == IPDRDATA_DOUBLE) {
						IPDRValue = (double *) &pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_->IPDRDataUnion_.IPDRDouble_;
						  appendListIPDRData(&((*pFNFData)->pListIPDRData_ ),
										IPDRValue,
										IPDRDATA_DOUBLE);
					}
					else if (pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_->IPDRDataType_ == IPDRDATA_FLOAT) {
					IPDRValue = (float *) &pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_->IPDRDataUnion_.IPDRFloat_;
					  appendListIPDRData(&((*pFNFData)->pListIPDRData_ ),
										IPDRValue,
										IPDRDATA_FLOAT);
					}
					else if (pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_->IPDRDataType_ == IPDRDATA_LONG) {
						appendListIPDRData(&((*pFNFData)->pListIPDRData_ ),
										(void *) pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_->IPDRDataUnion_.IPDRLong_,
										IPDRDATA_LONG);
					}
					else if (pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_->IPDRDataType_ == IPDRDATA_LONGLONG) {
						appendListIPDRData(&((*pFNFData)->pListIPDRData_ ),
										(void *) &(pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_->IPDRDataUnion_.IPDRLongLong_),
										IPDRDATA_LONGLONG);
					}
					else if (pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_->IPDRDataType_ == IPDRDATA_ULONG) {
						appendListIPDRData(&((*pFNFData)->pListIPDRData_ ),
										(void *) pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_->IPDRDataUnion_.IPDRULong_,
										IPDRDATA_ULONG);
					}
					else if (pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_->IPDRDataType_ == IPDRDATA_ULONGLONG) {
						appendListIPDRData(&((*pFNFData)->pListIPDRData_ ),
										(void *) &(pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_->IPDRDataUnion_.IPDRULongLong_),
										IPDRDATA_ULONGLONG);
					}
					else if (pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_->IPDRDataType_ == IPDRDATA_STRING) {
						appendListIPDRData(&((*pFNFData)->pListIPDRData_ ),
										pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_->IPDRDataUnion_.IPDRUTF8Array_,
										IPDRDATA_STRING);
					}
					else if (pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_->IPDRDataType_ == IPDRDATA_BOOLEAN) {
					/*IPDRValue = (bool *) &pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_->IPDRDataUnion_.IPDRBoolean_;
					  appendListIPDRData(&((*pFNFData)->pListIPDRData_ ),
										IPDRValue,
										IPDRDATA_BOOLEAN);
					*/
					  appendListIPDRData(&((*pFNFData)->pListIPDRData_ ),
										pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_->IPDRDataUnion_.IPDRBoolean_,
										IPDRDATA_BOOLEAN);
					}
					else if (pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_->IPDRDataType_ == IPDRDATA_SHORT) {
					  appendListIPDRData(&((*pFNFData)->pListIPDRData_ ),
										(void *) pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_->IPDRDataUnion_.IPDRShort_,
										IPDRDATA_SHORT);
					}
					else if (pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_->IPDRDataType_ == IPDRDATA_USHORT) {
					  appendListIPDRData(&((*pFNFData)->pListIPDRData_ ),
										(void *) (pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_->IPDRDataUnion_.IPDRUShort_),
										IPDRDATA_USHORT);
					}
   				else if (pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_->IPDRDataType_ == IPDRDATA_ARRAY) {
    					appendListArrayToIPDRData(&((*pFNFData)->pListIPDRData_),
        				pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_->pListIPDRArrayData_,pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_->noOfElements_, IPDRDATA_ARRAY);
   			  }
   			  else if (pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_->IPDRDataType_ == IPDRDATA_STRUCTURE) {
        				appendListStructureToIPDRData(&((*pFNFData)->pListIPDRData_),
						pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_->pListIPDRStructureData_,
        				IPDRDATA_STRUCTURE);
   			  }
					else if (pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_->IPDRDataType_ == IPDRDATA_BYTE) {
						appendListIPDRData(&((*pFNFData)->pListIPDRData_ ),
										(void *) pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_->IPDRDataUnion_.IPDRByte_,
										IPDRDATA_BYTE);
					}
					else if (pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_->IPDRDataType_ == IPDRDATA_UBYTE) {
						appendListIPDRData(&((*pFNFData)->pListIPDRData_ ),
										(void *) pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_->IPDRDataUnion_.IPDRByte_,
										IPDRDATA_UBYTE);
					}

					pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_ = pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_->pNext_; 
				}
				
				pIPDRStreamElement->pStreamElement_->StreamElementUnion_.pIPDRRecord_->pFNFData_->pListIPDRData_ = pFirst;

     
		if ((pIPDRCommonParameters->pSchema_ != NULL) && (pIPDRCommonParameters->pSchema_->pListSchemaLookup_ != NULL)) {
			
            /* Code change to now use the IPDRCommonParameters structure member to check schema version */
					errorSeverity = validateAgainstSchema(pIPDRCommonParameters, *pFNFData, pErrorCode);
					if (errorSeverity == IPDR_WARNING) {
						errorHandler(pIPDRCommonParameters, pErrorCode, "validateAgainstSchema");	
					}
					else if (errorSeverity == IPDR_ERROR) {
						pIPDRCommonParameters->DocState_ = NONREADABLE; 
						exceptionHandler(pIPDRCommonParameters, pErrorCode, "validateAgainstSchema");	
						return (IPDR_ERROR);
					}
			}

				freeIPDRStreamElement (pIPDRStreamElement);
				freeRecordDescriptor (pRecordDescriptor);
			
				break;

			case STREAM_DOC_END:
				/*
				   On getting the descriminator as 3, does nothing & returns. 
				   Calling application (In this case, main), will be calling readDocEnd explicitly
				   to read the DocEnd structure.
				*/
				freeFNFData(*pFNFData);
				*pFNFData = NULL;
				break;

			default:
				*pErrorCode = IPDR_INVALID_DESCRIMINATOR;
				return (IPDR_ERROR);
				break;
		}
	}
	else if (pIPDRCommonParameters->DocType_ == XML) {
		errorSeverity = readIPDRXMLRecordHelper(pIPDRCommonParameters, pFNFData, pErrorCode); 
		if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "readIPDRXMLRecordHelper");	
		}
		else if (errorSeverity == IPDR_ERROR) {
			pIPDRCommonParameters->DocState_ = NONREADABLE; 
			exceptionHandler(pIPDRCommonParameters, pErrorCode, "readIPDRXMLRecordHelper");	
			return (IPDR_ERROR);
		}
		if(*pFNFData != NULL) {
			
		if ((pIPDRCommonParameters->pSchema_ != NULL) && (pIPDRCommonParameters->pSchema_->pListSchemaLookup_ != NULL)) {
			
            /* Code change to now use the IPDRCommonParameters structure member to check schema version */
					errorSeverity = validateAgainstSchema(pIPDRCommonParameters, *pFNFData, pErrorCode);
					if (errorSeverity == IPDR_WARNING) {
						errorHandler(pIPDRCommonParameters, pErrorCode, "validateAgainstSchema");	
					}
					else if (errorSeverity == IPDR_ERROR) {
						pIPDRCommonParameters->DocState_ = NONREADABLE; 
						exceptionHandler(pIPDRCommonParameters, pErrorCode, "validateAgainstSchema");	
						return (IPDR_ERROR);
					}
			}
		}
	}
	else {
		*pErrorCode = IPDR_INVALID_DOCTYPE;
		return (IPDR_ERROR);
	}
	
	pIPDRCommonParameters->IPDRCount_.ipdrRecordCount_++;


	return (IPDR_OK);
}

/*!
 *  NAME:
 *      readDocEnd() - 		This function reads the docEnd
 *
 *  DESCRIPTION:
 *      This function
 *		- Calls "readIPDRXDRDocEndHelper"
 *		- Sets the state of document to NONREADABLE
 *
 *  ARGUMENTS:
 *      - IPDRCommonParameters (IN/OUT)	
 * 		- DocEnd (IN/OUT)
 *
 *  RETURNS 	
 *	     Returns Zero or appropriate error code
*/

int readDocEnd(IPDRCommonParameters* pIPDRCommonParameters, 
			   DocEnd* pDocEnd, 
			   int* pErrorCode)
{
	int errorSeverity = 0;
	
	if(pDocEnd == NULL || pIPDRCommonParameters == NULL) {
	    *pErrorCode = IPDR_NULL_INPUT_PARAMETER;
	    return (IPDR_ERROR);
	}
	
	if (pIPDRCommonParameters->DocState_ != READABLE) {
		*pErrorCode = IPDR_INVALID_DOCSTATE_MAKE_READABLE;
		return (IPDR_ERROR);
	}

	if(pIPDRCommonParameters->DocType_ == XDR) {
		errorSeverity = readIPDRXDRDocEndHelper(pIPDRCommonParameters, pDocEnd, pErrorCode); 
		if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "readIPDRXDRDocEndHelper");	
		}
		else if (errorSeverity == IPDR_ERROR) {
			pIPDRCommonParameters->DocState_ = NONREADABLE; 
			exceptionHandler(pIPDRCommonParameters, pErrorCode, "readIPDRXDRDocEndHelper");	
			return (IPDR_ERROR);
		}
	}
	else if (pIPDRCommonParameters->DocType_ == XML) {
		errorSeverity = readIPDRXMLDocEndHelper(pIPDRCommonParameters, pDocEnd, pErrorCode); 
		if (errorSeverity == IPDR_WARNING) {
			errorHandler(pIPDRCommonParameters, pErrorCode, "readIPDRXMLDocEndHelper");	
		}
		else if (errorSeverity == IPDR_ERROR) {
			pIPDRCommonParameters->DocState_ = NONREADABLE; 
			exceptionHandler(pIPDRCommonParameters, pErrorCode, "readIPDRXMLDocEndHelper");	
			return (IPDR_ERROR);
		}
	}
	else {
		*pErrorCode = IPDR_INVALID_DOCTYPE;
		return (IPDR_ERROR);
	}	

	pIPDRCommonParameters->DocState_ = NONREADABLE;
	
	return (IPDR_OK);
}
