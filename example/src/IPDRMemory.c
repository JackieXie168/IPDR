/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/



/*********************************************************
 * File                 : IPDRMemory.c					 * 
 * Description          : Contains all the Memory		 *
 *                        allocation and free	         *
 *                        functionalities of structures  *
 * Author               : Infosys Tech Ltd               *
 * Modification History :                                *
 *-------------------------------------------------------*
 * Date       Name             Change/Description	     *
 *-------------------------------------------------------*
 * 12/19/01  Infosys           Created                   *       
 *********************************************************/

#include <string.h>
#include "utils/IPDRMemory.h"
#include "utils/errorHandler.h"
#include "common/schema.h"


/*!
 *  NAME:
 *      newDocEnd() - Memory allocation of DocEnd structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory allocation of 
 *         DocEnd structure & its members.
 *	
 *  ARGUMENTS:
 *       -	void 
 *
 *  RETURNS 	
 *	     Returns Pointer to DocEnd structure
 */


DocEnd* newDocEnd(void)
{
	DocEnd* pDocEnd;
	pDocEnd = (DocEnd *) calloc (1, sizeof(DocEnd));
	pDocEnd->count_ = 0;
	pDocEnd->endTime_ = 0;
	return (pDocEnd);
}



/*!
 *  NAME:
 *      freeDocEnd() - Memory deallocation of 
 *      DocEnd structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory deallocation of 
 *         DocEnd structure & its members.
 *	
 *  ARGUMENTS:
 *       -	DocEnd {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
  

int freeDocEnd(DocEnd* pDocEnd)
{
	if (pDocEnd == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	free(pDocEnd);

	return (IPDR_OK);
}


/*!
 *  NAME:
 *      printDocEnd() - prints DocEnd structure
 *
 *  DESCRIPTION:
 *      1. This function prints the 
 *         DocEnd structure & its members.
 *	
 *  ARGUMENTS:
 *       -	pDocEnd {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int printDocEnd(DocEnd* pDocEnd)
{
	if (pDocEnd == NULL) {
        return (IPDR_NULL_INPUT_PARAMETER);
	}

	printf("\nDocEnd->endTime = %.lf", (double)pDocEnd->endTime_);
	printf("\nDocEnd->count = %d", pDocEnd->count_);
	
	return (IPDR_OK);

}

/*!
 *  NAME:
 *      newIPDRStreamElement() - Memory allocation of IPDRStreamElement 
 *                               structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory allocation of 
 *         IPDRStreamElement structure & its members.
 *	
 *  ARGUMENTS:
 *       -	StreamElementFlag 
 *
 *  RETURNS 	
 *	     Returns Pointer to IPDRStreamElement structure
 */


IPDRStreamElement* newIPDRStreamElement(int StreamElementFlag)
{
	IPDRStreamElement* pIPDRStreamElement;
	pIPDRStreamElement = (IPDRStreamElement *) calloc 
		(1, sizeof(IPDRStreamElement));

	pIPDRStreamElement->pStreamElement_ = 
		newStreamElement(StreamElementFlag);

	return (pIPDRStreamElement);
}



/*!
 *  NAME:
 *      freeIPDRStreamElement() - Memory deallocation of 
 *      IPDRStreamElement structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory deallocation of 
 *         IPDRStreamElement structure & its members.
 *	
 *  ARGUMENTS:
 *       -	IPDRStreamElement {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
  

int freeIPDRStreamElement(IPDRStreamElement* pIPDRStreamElement)
{
	if (pIPDRStreamElement == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	
	freeStreamElement(pIPDRStreamElement->pStreamElement_);
	free(pIPDRStreamElement);
	return (IPDR_OK);
}


/*!
 *  NAME:
 *      printIPDRStreamElement() - prints 
 *               IPDRStreamElement structure
 *
 *  DESCRIPTION:
 *      1. This function prints the 
 *         IPDRStreamElement structure & its members.
 *	
 *  ARGUMENTS:
 *       -	pIPDRStreamElement {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int printIPDRStreamElement(IPDRStreamElement* pIPDRStreamElement)
{
	if (pIPDRStreamElement == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	printf("\nIPDRStreamElement->descriminator_ = %d", 
		pIPDRStreamElement->descriminator_);
	printStreamElement(pIPDRStreamElement->pStreamElement_);

	return (IPDR_OK);
}


/*!
 *  NAME:
 *      newStreamElement() - Memory allocation of StreamElement structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory allocation of 
 *         StreamElement structure & its members.
 *	
 *  ARGUMENTS:
 *       -	StreamElementFlag 
 *
 *  RETURNS 	
 *	     Returns Pointer to StreamElement structure
 */


StreamElement* newStreamElement(int StreamElementFlag)
{
	StreamElement* pStreamElement;
	pStreamElement = (StreamElement *) calloc (1, sizeof(StreamElement));

	if (StreamElementFlag == STREAM_RECORD_DESCRIPTOR) {
		pStreamElement->StreamElementUnion_.pRecordDescriptor_ = 
			newRecordDescriptor();
		pStreamElement->StreamElementFlag_ = StreamElementFlag;
	}

	if (StreamElementFlag == STREAM_IPDR_RECORD) {
		pStreamElement->StreamElementUnion_.pIPDRRecord_ = 
			newIPDRRecord();
		pStreamElement->StreamElementFlag_ = StreamElementFlag;
	}


	if (StreamElementFlag == STREAM_DOC_END) {
		pStreamElement->StreamElementUnion_.pDocEnd_ = 
			(DocEnd *) calloc (1, sizeof(DocEnd));
		pStreamElement->StreamElementUnion_.pDocEnd_->count_ = 0;
		pStreamElement->StreamElementUnion_.pDocEnd_->endTime_ = 0;
		pStreamElement->StreamElementFlag_ = StreamElementFlag;
	}

	return (pStreamElement);

}

/*!
 *  NAME:
 *      freeStreamElement() - Memory deallocation of 
 *      StreamElement structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory deallocation of 
 *         StreamElement structure & its members.
 *	
 *  ARGUMENTS:
 *       -	StreamElement {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int freeStreamElement(StreamElement* pStreamElement)
{
	if (pStreamElement == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}
	
	if (pStreamElement->StreamElementFlag_ == STREAM_RECORD_DESCRIPTOR) {
		freeRecordDescriptor(pStreamElement->StreamElementUnion_.pRecordDescriptor_);
	}

	if (pStreamElement->StreamElementFlag_ == STREAM_IPDR_RECORD) {
		freeIPDRRecord(pStreamElement->StreamElementUnion_.pIPDRRecord_);
	}

	if (pStreamElement->StreamElementFlag_ == STREAM_DOC_END) {
		freeDocEnd(pStreamElement->StreamElementUnion_.pDocEnd_);
	}

	free(pStreamElement);
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      printStreamElement() - prints StreamElement structure
 *
 *  DESCRIPTION:
 *      1. This function prints the 
 *         StreamElement structure & its members.
 *	
 *  ARGUMENTS:
 *       -	pStreamElement {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int printStreamElement(StreamElement* pStreamElement)
{
	if (pStreamElement == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	if (pStreamElement->StreamElementFlag_ == 
		STREAM_RECORD_DESCRIPTOR) {
		printRecordDescriptor(
			pStreamElement->StreamElementUnion_.pRecordDescriptor_);
	}

	if (pStreamElement->StreamElementFlag_ == STREAM_IPDR_RECORD) {
		printf("\nflag = %d", pStreamElement->StreamElementFlag_);
		printIPDRRecord(
			pStreamElement->StreamElementUnion_.pIPDRRecord_);
	}

	if (pStreamElement->StreamElementFlag_ == STREAM_DOC_END) {
		printf("\n Count = %d", 
			pStreamElement->StreamElementUnion_.pDocEnd_->count_);
		printf("\n End Time = %ld", 
			pStreamElement->StreamElementUnion_.pDocEnd_->endTime_);
	}

	return (IPDR_OK);
}


/*!
 *  NAME:
 *      addListServiceDefinitionURI() - adds node to the 
 *           ServiceDefinitionURI list.
 * 
 *  DESCRIPTION:
 *      1. This function adds the node to the ServiceDefinitionURI
 *         list with the values passed in the input parameters
 *	
 *  ARGUMENTS:
 *       - pHeadRef, pointer to ListServiceDefinitionURI {IN/OUT}
 *       - serviceDefinitionURI {IN}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


int addListServiceDefinitionURI(
						ListServiceDefinitionURI** pHeadRef, 
						char* serviceDefinitionURI)
{
	unsigned int lenServiceDefURI = 0;

	ListServiceDefinitionURI* pListServiceDefinitionURI = 
	(ListServiceDefinitionURI *) calloc (1, sizeof(ListServiceDefinitionURI));
	
	if (pListServiceDefinitionURI == NULL) {
		return (IPDR_MEMORY_ALLOCATION_FAILED);
	}
	
	
	lenServiceDefURI = strlen(serviceDefinitionURI) + 1;
	pListServiceDefinitionURI->pServiceDefintionURI_ = (char *) 
		calloc (lenServiceDefURI, sizeof(char));

	if (pListServiceDefinitionURI->pServiceDefintionURI_ == NULL) {
		return (IPDR_MEMORY_ALLOCATION_FAILED);
	}

	strcpy(pListServiceDefinitionURI->pServiceDefintionURI_, 
		serviceDefinitionURI);
	pListServiceDefinitionURI->pNext_ = *pHeadRef;
	*pHeadRef = pListServiceDefinitionURI;

	return (IPDR_OK);
}


/*!
 *  NAME:
 *      appendListServiceDefinitionURI() - appends node to the 
 *           ServiceDefinitionURI list.
 * 
 *  DESCRIPTION:
 *      1. This function appends the node to the ServiceDefinitionURI
 *         list with the values passed in the input parameters
 *	
 *  ARGUMENTS:
 *       - pHeadRef, pointer to ListServiceDefinitionURI {IN/OUT}
 *       - serviceDefinitionURI {IN}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


int appendListServiceDefinitionURI(ListServiceDefinitionURI **pHeadRef,
								   char* serviceDefinitionURI)
{
	ListServiceDefinitionURI* pCurrent = *pHeadRef;
	if (pHeadRef == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}
 
	if (serviceDefinitionURI == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}
 
	if (pCurrent == NULL) {
		addListServiceDefinitionURI(pHeadRef, serviceDefinitionURI);
	} else {
		while (pCurrent->pNext_ != NULL) {
			pCurrent = pCurrent->pNext_;
		}
        addListServiceDefinitionURI
		(&(pCurrent->pNext_), serviceDefinitionURI);
	}
	
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      getLengthListServiceDefinitionURI() - calculates length of  
 *          the ServiceDefinitionURI list.
 * 
 *  DESCRIPTION:
 *      1. This function calculates length of the ServiceDefinitionURI
 *         list
 *	
 *  ARGUMENTS:
 *       - pListServiceDefinitionURI {IN/OUT}
 *
 *  RETURNS 	
 *	     Returns length if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int getLengthListServiceDefinitionURI(
			ListServiceDefinitionURI* pListServiceDefinitionURI)
{
	int count = 0;
	ListServiceDefinitionURI* pCurrent = pListServiceDefinitionURI;

	if (pListServiceDefinitionURI == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	for (pCurrent = pListServiceDefinitionURI; pCurrent != NULL; 
	pCurrent = pCurrent->pNext_)
		count++;
	return (count);
}


/*!
 *  NAME:
 *      freeListServiceDefinitionURI() - Memory deallocation of 
 *      ListServiceDefinitionURI structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory deallocation of 
 *         ListServiceDefinitionURI structure & its members.
 *	
 *  ARGUMENTS:
 *       -	ListServiceDefinitionURI {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int freeListServiceDefinitionURI(ListServiceDefinitionURI **pHeadRef)
{
	ListServiceDefinitionURI* pCurrent;
	ListServiceDefinitionURI* pNext_;

	if (pHeadRef == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}
 
	pCurrent = *pHeadRef;
        while (pCurrent != NULL) {
            pNext_ = pCurrent->pNext_;
	    free(pCurrent->pServiceDefintionURI_);
	    free(pCurrent);
	    pCurrent = pNext_;
    }
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      printListServiceDefinitionURI() - prints 
 *         ListServiceDefinitionURI structure
 *
 *  DESCRIPTION:
 *      1. This function prints the 
 *         ListServiceDefinitionURI structure & its members.
 *	
 *  ARGUMENTS:
 *       -	pListServiceDefinitionURI {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int printListServiceDefinitionURI(
				ListServiceDefinitionURI* pListServiceDefinitionURI)
{
	ListServiceDefinitionURI* pCurrent = pListServiceDefinitionURI;
	int count = 0;
	if (pCurrent == NULL)  
		return(IPDR_NULL_LINKLIST);
	
	while (pCurrent != NULL) {

		printf("\n\nDisplaying Record %d", count + 1);
		printf("\nService Definition URI\t= %s", 
			pCurrent->pServiceDefintionURI_);		

	    pCurrent = pCurrent->pNext_;	
	    count++;
	}
	
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      newIPDRRecord() - Memory allocation of IPDRRecord structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory allocation of 
 *         IPDRRecord structure & its members.
 *	
 *  ARGUMENTS:
 *       -	void 
 *
 *  RETURNS 	
 *	     Returns Pointer to IPDRRecord structure
 */



IPDRRecord* newIPDRRecord(void)
{
	IPDRRecord* pIPDRRecord;
	pIPDRRecord = (IPDRRecord *) calloc (1, sizeof(IPDRRecord));
	pIPDRRecord->pFNFData_ = newFNFData();
	return (pIPDRRecord);
}

/*!
 *  NAME:
 *      freeIPDRRecord() - Memory deallocation of 
 *      IPDRRecord structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory deallocation of 
 *         IPDRRecord structure & its members.
 *	
 *  ARGUMENTS:
 *       -	IPDRRecord {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int freeIPDRRecord(IPDRRecord* pIPDRRecord)
{
	if (pIPDRRecord == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	freeFNFData(pIPDRRecord->pFNFData_);
	free(pIPDRRecord);
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      printIPDRRecord() - prints 
 *         IPDRRecord structure
 *
 *  DESCRIPTION:
 *      1. This function prints the 
 *         IPDRRecord structure & its members.
 *	
 *  ARGUMENTS:
 *       -	pIPDRRecord {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int printIPDRRecord(IPDRRecord* pIPDRRecord)
{
	if (pIPDRRecord == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	printf("\npIPDRRecord->descriptorID_ = %d", 
		pIPDRRecord->descriptorID_);
	printFNFData(pIPDRRecord->pFNFData_);

	return (IPDR_OK);
}


/*!
 *  NAME:
 *      newFNFData() - Memory allocation of FNFData structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory allocation of 
 *         FNFData structure & its members.
 *	
 *  ARGUMENTS:
 *       -	void 
 *
 *  RETURNS 	
 *	     Returns Pointer to FNFData structure
 */


FNFData* newFNFData(void)
{
	FNFData* pFNFData ;
	pFNFData = (FNFData *) calloc (1, sizeof(FNFData));
	pFNFData->typeName_ = (char *) calloc (MAX_ATTRIBUTE_TYPE, sizeof(char));
	pFNFData->pListAttributeDescriptor_ = NULL;
	pFNFData->pListIPDRData_ = NULL;
	return (pFNFData);
}

/*!
 *  NAME:
 *      freeFNFData() - Memory deallocation of 
 *      FNFData structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory deallocation of 
 *         FNFData structure & its members.
 *	
 *  ARGUMENTS:
 *       -	FNFData {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int freeFNFData(FNFData* pFNFData)
{
	if (pFNFData == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	free(pFNFData->typeName_);
	pFNFData->typeName_ = NULL;
	freeListAttributeDescriptor(&(pFNFData->pListAttributeDescriptor_));
	pFNFData->pListAttributeDescriptor_ = NULL;
	freeListIPDRData(&(pFNFData->pListIPDRData_));
	pFNFData->pListIPDRData_ = NULL;
	free (pFNFData);
	pFNFData = NULL;
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      printFNFdata() - prints 
 *         FNFdata structure
 *
 *  DESCRIPTION:
 *      1. This function prints the 
 *         FNFdata structure & its members.
 *	
 *  ARGUMENTS:
 *       -	pFNFdata {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int printFNFData(FNFData* pFNFData)
{
	if (pFNFData == NULL) {
		printf("\nNULL\n");
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	printf("\nFNFData->typeName_ = %s", pFNFData->typeName_);
	printListAttributeDescriptor(pFNFData->pListAttributeDescriptor_);
	printListIPDRData(pFNFData->pListIPDRData_);

	return (IPDR_OK);
}


/*!
 *  NAME:
 *      newRecordDescriptor() - Memory allocation of RecordDescriptor 
 *      structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory allocation of 
 *         RecordDescriptor structure & its members.
 *	
 *  ARGUMENTS:
 *       -	void 
 *
 *  RETURNS 	
 *	     Returns Pointer to RecordDescriptor structure
 */


RecordDescriptor* newRecordDescriptor(void)
{
	RecordDescriptor* pRecordDescriptor;
	pRecordDescriptor = (RecordDescriptor *) calloc 
		(1, sizeof(RecordDescriptor));
	pRecordDescriptor->typeName_ = (char *) calloc (MAX_ATTRIBUTE_TYPE, sizeof(char));
	pRecordDescriptor->pListAttributeDescriptor_ = NULL;
	return (pRecordDescriptor);
}


/*!
 *  NAME:
 *      initRecordDescriptor() - Initializes RecordDescriptor 
 *      structure
 *
 *  DESCRIPTION:
 *      1. This function initializes the RecordDescriptor
 *         with the values passed as in input parameters
 *	
 *  ARGUMENTS:
 *       -	pRecordDescriptor {IN/OUT} 
 *       -	descriptorID {IN} 
 *       -	typeName {IN} 
 *       -	pListAttributeDescriptor {IN} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


int initRecordDescriptor(
				RecordDescriptor* pRecordDescriptor,
				int descriptorID,
				char* typeName,
				ListAttributeDescriptor* pListAttributeDescriptor)
{
	if (pRecordDescriptor == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	if (typeName == NULL)
		strcpy(typeName, IPDR_NULL);

	pRecordDescriptor->descriptorID_ = descriptorID;
	strcpy(pRecordDescriptor->typeName_, typeName);
	pRecordDescriptor->pListAttributeDescriptor_ = 
		pListAttributeDescriptor;

	return (IPDR_OK);
}

/*!
 *  NAME:
 *      freeRecordDescriptor() - Memory deallocation of 
 *      RecordDescriptor structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory deallocation of 
 *         RecordDescriptor structure & its members.
 *	
 *  ARGUMENTS:
 *       -	RecordDescriptor {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int freeRecordDescriptor(RecordDescriptor* pRecordDescriptor)
{
	if (pRecordDescriptor == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	free(pRecordDescriptor->typeName_);
	freeListAttributeDescriptor(
		&pRecordDescriptor->pListAttributeDescriptor_);
	free(pRecordDescriptor);

	return (IPDR_OK);
}

/*!
 *  NAME:
 *      printRecordDescriptor() - prints 
 *         RecordDescriptor structure
 *
 *  DESCRIPTION:
 *      1. This function prints the 
 *         RecordDescriptor structure & its members.
 *	
 *  ARGUMENTS:
 *       -	pRecordDescriptor {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int printRecordDescriptor(RecordDescriptor* pRecordDescriptor)
{
	if (pRecordDescriptor == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	printf("\npRecordDescriptor->descriptorID_ = %d", 
		pRecordDescriptor->descriptorID_);
	printf("\npRecordDescriptor->typeName_ = %s", 
		pRecordDescriptor->typeName_);
	printListAttributeDescriptor(
		pRecordDescriptor->pListAttributeDescriptor_);

	return (IPDR_OK);
}



/*!
 *  NAME:
 *      addListIPDRData() - adds node to the 
 *           IPDRData list.
 * 
 *  DESCRIPTION:
 *      1. This function adds the node to the IPDRData
 *         list with the values passed in the input parameters
 *	
 *  ARGUMENTS:
 *       - pHeadRef, pointer to ListIPDRData {IN/OUT}
 *       - dataValue {IN}
 *       - dataType {IN}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


int addListIPDRData(ListIPDRData** pHeadRef, 
			 void* dataValue,
			 int dataType
		) 
{

	ListIPDRData* pListIPDRData = (ListIPDRData *)
	                 calloc (1, sizeof(struct ListIPDRData));

	if (pHeadRef == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	if (dataValue == NULL) {
		if(dataType == 7 || dataType == 8)
			return (IPDR_NULL_INPUT_PARAMETER);
	}

	switch (dataType)
	{
	case IPDRDATA_LONG:
		pListIPDRData->IPDRDataUnion_.IPDRLong_ = (int)dataValue;
		pListIPDRData->IPDRDataType_ = dataType;
		break;

	case IPDRDATA_ULONG:
		pListIPDRData->IPDRDataUnion_.IPDRULong_ = 
			(unsigned int)dataValue;
		pListIPDRData->IPDRDataType_ = dataType;
		break;

	case IPDRDATA_LONGLONG:
		pListIPDRData->IPDRDataUnion_.IPDRLongLong_ = *((hyper *)dataValue);
		pListIPDRData->IPDRDataType_ = dataType;
		break;

	case IPDRDATA_ULONGLONG:
		pListIPDRData->IPDRDataUnion_.IPDRULongLong_ = 
			*((unsigned hyper *)dataValue);
		pListIPDRData->IPDRDataType_ = dataType;
		break;

	case IPDRDATA_FLOAT:
		pListIPDRData->IPDRDataUnion_.IPDRFloat_ = *((float *) dataValue);
		pListIPDRData->IPDRDataType_ = dataType;
		break;


	case IPDRDATA_DOUBLE:
		pListIPDRData->IPDRDataUnion_.IPDRDouble_ = *((double *) dataValue);
		pListIPDRData->IPDRDataType_ = dataType;
		break;

	case IPDRDATA_BYTEARRAY:
		pListIPDRData->IPDRDataUnion_.IPDRByteArray_ = 
			(byte *)calloc(MAX_IPDR_STRING, sizeof(char));
		strcpy((char *)pListIPDRData->IPDRDataUnion_.IPDRByteArray_, 
			dataValue);
		pListIPDRData->IPDRDataType_ = dataType;
		break;


	case IPDRDATA_UTF8ARRAY:
		pListIPDRData->IPDRDataUnion_.IPDRUTF8Array_ = 
		(wchar *)calloc(MAX_IPDR_STRING, sizeof(char));
		strcpy((char *) pListIPDRData->IPDRDataUnion_.IPDRUTF8Array_, 
		dataValue);
		pListIPDRData->IPDRDataType_ = dataType;
		break;

	default:
		break;
	}

	
	pListIPDRData->pNext_ = *pHeadRef; 
	*pHeadRef = pListIPDRData; 

	return (IPDR_OK);
}


/*!
 *  NAME:
 *      appendListIPDRData() - appends node to the 
 *           IPDRData list.
 * 
 *  DESCRIPTION:
 *      1. This function appends the node to the IPDRData
 *         list with the values passed in the input parameters
 *	
 *  ARGUMENTS:
 *       - pHeadRef, pointer to ListIPDRData {IN/OUT}
 *       - dataValue {IN}
 *       - dataType {IN}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int appendListIPDRData(struct ListIPDRData** pHeadRef, 
	 			 void* dataValue,
				 int dataType
) 
{
	ListIPDRData* pListIPDRData = NULL;
	ListIPDRData* pCurrent = *pHeadRef;

	if (pHeadRef == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	if(dataValue == NULL) {
		if (dataType == 7 || dataType == 8) {
		    return (IPDR_NULL_INPUT_PARAMETER);
		}
	}

	if (pCurrent == NULL) {
		pListIPDRData = (ListIPDRData *)
	                 calloc (1, sizeof(struct ListIPDRData));

	if (pHeadRef == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	if (dataValue == NULL) {
		if(dataType == 7 || dataType == 8)
			return (IPDR_NULL_INPUT_PARAMETER);
	}

	switch (dataType)
	{
	case IPDRDATA_LONG:
		pListIPDRData->IPDRDataUnion_.IPDRLong_ = (int)dataValue;
		pListIPDRData->IPDRDataType_ = dataType;
		break;

	case IPDRDATA_ULONG:
		pListIPDRData->IPDRDataUnion_.IPDRULong_ = 
			(unsigned int)dataValue;
		pListIPDRData->IPDRDataType_ = dataType;
		break;

	case IPDRDATA_LONGLONG:
		pListIPDRData->IPDRDataUnion_.IPDRLongLong_ = *((hyper *)dataValue);
		pListIPDRData->IPDRDataType_ = dataType;
		break;

	case IPDRDATA_ULONGLONG:
		pListIPDRData->IPDRDataUnion_.IPDRULongLong_ = 
			*((unsigned hyper *)dataValue);
		pListIPDRData->IPDRDataType_ = dataType;
		break;

	case IPDRDATA_FLOAT:
		pListIPDRData->IPDRDataUnion_.IPDRFloat_ = *((float *) dataValue);
		pListIPDRData->IPDRDataType_ = dataType;
		break;


	case IPDRDATA_DOUBLE:
		pListIPDRData->IPDRDataUnion_.IPDRDouble_ = *((double *) dataValue);
		pListIPDRData->IPDRDataType_ = dataType;
		break;

	case IPDRDATA_BYTEARRAY:
		pListIPDRData->IPDRDataUnion_.IPDRByteArray_ = 
			(byte *)calloc(MAX_IPDR_STRING, sizeof(char));
		strcpy((char *)pListIPDRData->IPDRDataUnion_.IPDRByteArray_, 
			dataValue);
		pListIPDRData->IPDRDataType_ = dataType;
		break;


	case IPDRDATA_UTF8ARRAY:
		pListIPDRData->IPDRDataUnion_.IPDRUTF8Array_ = 
		(wchar *)calloc(MAX_IPDR_STRING, sizeof(char));
		strcpy((char *) pListIPDRData->IPDRDataUnion_.IPDRUTF8Array_, 
		dataValue);
		pListIPDRData->IPDRDataType_ = dataType;
		break;

	default:
		break;
	}

	
	pListIPDRData->pNext_ = *pHeadRef; 
	*pHeadRef = pListIPDRData; 

	} else {
		    while (pCurrent->pNext_ != NULL) {
		        pCurrent = pCurrent->pNext_;
			}
			pListIPDRData = (ListIPDRData *)
	                 calloc (1, sizeof(struct ListIPDRData));

	if (pHeadRef == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	if (dataValue == NULL) {
		if(dataType == 7 || dataType == 8)
			return (IPDR_NULL_INPUT_PARAMETER);
	}

	switch (dataType)
	{
	case IPDRDATA_LONG:
		pListIPDRData->IPDRDataUnion_.IPDRLong_ = (int)dataValue;
		pListIPDRData->IPDRDataType_ = dataType;
		break;

	case IPDRDATA_ULONG:
		pListIPDRData->IPDRDataUnion_.IPDRULong_ = 
			(unsigned int)dataValue;
		pListIPDRData->IPDRDataType_ = dataType;
		break;

	case IPDRDATA_LONGLONG:
		pListIPDRData->IPDRDataUnion_.IPDRLongLong_ = *((hyper *)dataValue);
		pListIPDRData->IPDRDataType_ = dataType;
		break;

	case IPDRDATA_ULONGLONG:
		pListIPDRData->IPDRDataUnion_.IPDRULongLong_ = 
		*((unsigned hyper *)dataValue);
		pListIPDRData->IPDRDataType_ = dataType;
		break;

	case IPDRDATA_FLOAT:
		pListIPDRData->IPDRDataUnion_.IPDRFloat_ = *((float *) dataValue);
		pListIPDRData->IPDRDataType_ = dataType;
		break;


	case IPDRDATA_DOUBLE:
		pListIPDRData->IPDRDataUnion_.IPDRDouble_ = *((double *) dataValue);
		pListIPDRData->IPDRDataType_ = dataType;
		break;

	case IPDRDATA_BYTEARRAY:
		pListIPDRData->IPDRDataUnion_.IPDRByteArray_ = 
			(byte *)calloc(MAX_IPDR_STRING, sizeof(char));
		strcpy((char *) pListIPDRData->IPDRDataUnion_.IPDRByteArray_, 
			dataValue);
		pListIPDRData->IPDRDataType_ = dataType;
		break;


	case IPDRDATA_UTF8ARRAY:
		pListIPDRData->IPDRDataUnion_.IPDRUTF8Array_ = 
		(wchar *)calloc(MAX_IPDR_STRING, sizeof(char));
		strcpy((char *) pListIPDRData->IPDRDataUnion_.IPDRUTF8Array_, 
		dataValue);
		pListIPDRData->IPDRDataType_ = dataType;
		break;

	default:
		break;
	}

	
	pListIPDRData->pNext_ = NULL; 
	pCurrent->pNext_ = pListIPDRData; 

		}
	return (IPDR_OK);
}


/*!
 *  NAME:
 *      freeListIPDRData() - Memory deallocation of 
 *      ListIPDRData structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory deallocation of 
 *         ListIPDRData structure & its members.
 *	
 *  ARGUMENTS:
 *       -	ListIPDRData {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int freeListIPDRData(ListIPDRData** pHeadRef)
{
	ListIPDRData* pCurrent;
	ListIPDRData* pNext;
		
	if (pHeadRef == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	pCurrent = *pHeadRef;
    while (pCurrent != NULL) {
        pNext = pCurrent->pNext_;
		
		if (pCurrent->IPDRDataType_ == IPDRDATA_BYTEARRAY)
			free(pCurrent->IPDRDataUnion_.IPDRByteArray_);  

		if (pCurrent->IPDRDataType_ == IPDRDATA_UTF8ARRAY) {
			free(pCurrent->IPDRDataUnion_.IPDRUTF8Array_);  
			
		}

	    free(pCurrent);
        pCurrent = pNext;
    }
    free(pCurrent);
	return (IPDR_OK);
}


/*!
 *  NAME:
 *      freeListIPDRData() - Memory deallocation of 
 *      ListIPDRData structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory deallocation of 
 *         ListIPDRData structure & its members.
 *	
 *  ARGUMENTS:
 *       -	ListIPDRData {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int freeListNodeIPDRData(ListIPDRData** pHeadRef, int node)
{
	ListIPDRData* pCurrent = NULL;
	ListIPDRData* pPrevious = NULL;
	int recordCount = 1;

	for (pCurrent = *pHeadRef, recordCount = 1; pCurrent != NULL; pCurrent = pCurrent->pNext_, recordCount++) {
		if (recordCount == node) 
			break;

		pPrevious = pCurrent;
	}


	if (pCurrent == NULL)
		return (IPDR_OK);

	if (pPrevious != NULL) {
        pPrevious->pNext_ = pCurrent->pNext_;
    } else {
        *pHeadRef = pCurrent->pNext_;
    }

	if (pCurrent->IPDRDataType_ == IPDRDATA_BYTEARRAY)
		free(pCurrent->IPDRDataUnion_.IPDRByteArray_);  
	if (pCurrent->IPDRDataType_ == IPDRDATA_UTF8ARRAY) 
		free(pCurrent->IPDRDataUnion_.IPDRUTF8Array_);  
			
	free(pCurrent);

	return (IPDR_OK);
}




/*!
 *  NAME:
 *      getLengthListIPDRData() - calculates length of  
 *          the IPDRData list.
 * 
 *  DESCRIPTION:
 *      1. This function calculates length of the IPDRData
 *         list
 *	
 *  ARGUMENTS:
 *       - pListIPDRData {IN/OUT}
 *
 *  RETURNS 	
 *	     Returns length if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int getLengthListIPDRData(ListIPDRData* pListIPDRData)
{
	int count = 0;
	ListIPDRData* pCurrent = pListIPDRData;
	if (pListIPDRData == NULL) {
			return (IPDR_NULL_INPUT_PARAMETER);
		
                /*if(pListIPDRData->IPDRDataType_ == 7 || pListIPDRData->IPDRDataType_ == 8) {
			return (IPDR_NULL_INPUT_PARAMETER);
		}*/
	}
	for (pCurrent = pListIPDRData; pCurrent != NULL; 
	pCurrent = pCurrent->pNext_)
		count++;
	return (count);
}

/*!
 *  NAME:
 *      printListIPDRData() - prints 
 *         ListIPDRData structure
 *
 *  DESCRIPTION:
 *      1. This function prints the 
 *         ListIPDRData structure & its members.
 *	
 *  ARGUMENTS:
 *       -	pListIPDRData {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int printListIPDRData(ListIPDRData* pListIPDRData)
{
	ListIPDRData* pCurrent = pListIPDRData;
	int count = 0;

	if (pListIPDRData == NULL) {
		if(pListIPDRData->IPDRDataType_ == 7 || pListIPDRData->IPDRDataType_ == 8) 
			return (IPDR_NULL_INPUT_PARAMETER);
	}

	while (pCurrent != NULL) {
	    printf("\n\nDisplaying Record %d", count + 1);
			if (pCurrent->IPDRDataType_ == IPDRDATA_LONG) 
				printf("\nIPDRLong_ \t= %d", 
				pCurrent->IPDRDataUnion_.IPDRLong_);
			if (pCurrent->IPDRDataType_ == IPDRDATA_ULONG) 
				printf("\nIPDRULong_ \t= %d", 
				pCurrent->IPDRDataUnion_.IPDRULong_);
			if (pCurrent->IPDRDataType_ == IPDRDATA_LONGLONG) 
				printf("\nIPDRLongLong_ \t= %.lf", 
				(double) pCurrent->IPDRDataUnion_.IPDRLongLong_);
	 		if (pCurrent->IPDRDataType_ == IPDRDATA_ULONGLONG) 
				printf("\nIPDRULongLong_ \t= %.lf", 
				pCurrent->IPDRDataUnion_.IPDRULongLong_);
	 		if (pCurrent->IPDRDataType_ == IPDRDATA_FLOAT) 
				printf("\nIPDRFloat_ \t= %f", 
				pCurrent->IPDRDataUnion_.IPDRFloat_);
	 		if (pCurrent->IPDRDataType_ == IPDRDATA_DOUBLE) 
				printf("\nIPDRDouble_ \t= %lf", 
				pCurrent->IPDRDataUnion_.IPDRDouble_);
	 		if (pCurrent->IPDRDataType_ == IPDRDATA_BYTEARRAY) 
				printf("\nIPDRByteArray_ \t= %s", 
				pCurrent->IPDRDataUnion_.IPDRByteArray_);
			if (pCurrent->IPDRDataType_ == IPDRDATA_UTF8ARRAY) 
				printf("\nIPDRUTF8Array_ \t= %s", 
				pCurrent->IPDRDataUnion_.IPDRUTF8Array_);
	    pCurrent = pCurrent->pNext_;	
	    count++;
	}
	return (IPDR_OK);
}



/*!
 *  NAME:
 *      newIPDRData() - Memory allocation of IPDRData structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory allocation of 
 *         IPDRData structure & its members.
 *	
 *  ARGUMENTS:
 *       -	IPDRDataFlag 
 *
 *  RETURNS 	
 *	     Returns Pointer to IPDRData structure
 */

IPDRData* newIPDRData(int IPDRDataFlag)
{
	IPDRData* pIPDRData;
	pIPDRData = (IPDRData *) calloc(1, sizeof(IPDRData));
	
	if (IPDRDataFlag == IPDRDATA_LONG) {
		pIPDRData->IPDRDataUnion_.IPDRLong_ = 0;
		pIPDRData->IPDRDataFlag_ = IPDRDataFlag;
	}
	if (IPDRDataFlag == IPDRDATA_ULONG) { 
		pIPDRData->IPDRDataUnion_.IPDRULong_ = 0;
		pIPDRData->IPDRDataFlag_ = IPDRDataFlag;
	}
	if (IPDRDataFlag == IPDRDATA_LONGLONG) {
		pIPDRData->IPDRDataUnion_.IPDRLongLong_ = 0;
		pIPDRData->IPDRDataFlag_ = IPDRDataFlag;
	}
	if (IPDRDataFlag == IPDRDATA_ULONGLONG) {
		pIPDRData->IPDRDataUnion_.IPDRULongLong_ = 0;
		pIPDRData->IPDRDataFlag_ = IPDRDataFlag;
	}
	if (IPDRDataFlag == IPDRDATA_FLOAT) {
		pIPDRData->IPDRDataUnion_.IPDRFloat_ = 0;
		pIPDRData->IPDRDataFlag_ = IPDRDataFlag;
	}
	if (IPDRDataFlag == IPDRDATA_DOUBLE) {
		pIPDRData->IPDRDataUnion_.IPDRDouble_ = 0;
		pIPDRData->IPDRDataFlag_ = IPDRDataFlag;
	}
	if (IPDRDataFlag == IPDRDATA_BYTEARRAY) {
		pIPDRData->IPDRDataUnion_.IPDRByteArray_ = 
			(byte *) calloc(MAX_IPDR_STRING, sizeof(char));
		pIPDRData->IPDRDataFlag_ = IPDRDataFlag;
	}

	
	if (IPDRDataFlag == IPDRDATA_UTF8ARRAY) {
		pIPDRData->IPDRDataUnion_.IPDRUTF8Array_ = 
		(wchar *) calloc(MAX_IPDR_STRING, sizeof(char));
		pIPDRData->IPDRDataFlag_ = IPDRDataFlag;
	}
	

	return (pIPDRData);
}

/*!
 *  NAME:
 *      freeIPDRData() - Memory deallocation of 
 *      IPDRData structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory deallocation of 
 *         IPDRData structure & its members.
 *	
 *  ARGUMENTS:
 *       -	IPDRData {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int freeIPDRData(IPDRData* pIPDRData)
{
	if (pIPDRData == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	if (pIPDRData->IPDRDataFlag_ == IPDRDATA_BYTEARRAY) {
		free(pIPDRData->IPDRDataUnion_.IPDRByteArray_);
		pIPDRData->IPDRDataUnion_.IPDRByteArray_ = NULL;
	}
	
	if (pIPDRData->IPDRDataFlag_ == IPDRDATA_UTF8ARRAY) {
		free(pIPDRData->IPDRDataUnion_.IPDRUTF8Array_);
 		pIPDRData->IPDRDataUnion_.IPDRUTF8Array_ = NULL;
	}
	

	free(pIPDRData);
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      printIPDRData() - prints 
 *         IPDRData structure
 *
 *  DESCRIPTION:
 *      1. This function prints the 
 *         IPDRData structure & its members.
 *	
 *  ARGUMENTS:
 *       -	pIPDRData {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int printIPDRData(IPDRData* pIPDRData)
{
	if (pIPDRData == NULL) {
        return (IPDR_NULL_INPUT_PARAMETER);
	}
	if (pIPDRData->IPDRDataFlag_ == IPDRDATA_LONG)
		printf("\nIPDRDATA_LONG = %d", 
		pIPDRData->IPDRDataUnion_.IPDRLong_);
	if (pIPDRData->IPDRDataFlag_ == IPDRDATA_ULONG)
		printf("\nIPDR_ULONG = %u", 
		pIPDRData->IPDRDataUnion_.IPDRULong_);
	if (pIPDRData->IPDRDataFlag_ == IPDRDATA_LONGLONG)
		printf("\nIPDR_LONGLONG = %lf", 
		pIPDRData->IPDRDataUnion_.IPDRLongLong_);
	if (pIPDRData->IPDRDataFlag_ == IPDRDATA_FLOAT)
		printf("\nIPDR_FLOAT = %f", 
		pIPDRData->IPDRDataUnion_.IPDRFloat_);
	if (pIPDRData->IPDRDataFlag_ == IPDRDATA_DOUBLE)
		printf("\nIPDR_DOUBLE = %d", 
		pIPDRData->IPDRDataUnion_.IPDRDouble_);
	if (pIPDRData->IPDRDataFlag_ == IPDRDATA_BYTEARRAY)
		printf("\nIPDR_BYTEARRAY = %s", 
		pIPDRData->IPDRDataUnion_.IPDRByteArray_);
	if (pIPDRData->IPDRDataFlag_ == IPDRDATA_UTF8ARRAY)
		printf("\nIPDR_UTF8ARRAY = %s", 
		pIPDRData->IPDRDataUnion_.IPDRUTF8Array_);

	return (IPDR_OK);
}


/*!
 *  NAME:
 *      newIPDRCommonParameters() - Memory allocation of 
 *      IPDRCommonParameters structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory allocation of 
 *         IPDRCommonParameters structure & its members.
 *	
 *  ARGUMENTS:
 *       -	void 
 *
 *  RETURNS 	
 *	     Returns Pointer to IPDRCommonParameters structure
 */

IPDRCommonParameters* newIPDRCommonParameters(void)
{
	IPDRCommonParameters* pIPDRCommonParameters;
	pIPDRCommonParameters = (IPDRCommonParameters *) 
		calloc(1, sizeof(IPDRCommonParameters));
	pIPDRCommonParameters->pDescriptorLookupHashTable_ = NULL;
	pIPDRCommonParameters->pDescriptorLookupList_ = NULL;
	pIPDRCommonParameters->pListErrorStructure_ = NULL;
	pIPDRCommonParameters->pSchema_ = newSchema();
	pIPDRCommonParameters->pExceptionStructure_ = newExceptionStructure();
	pIPDRCommonParameters->pListFNFData_ = NULL;
	pIPDRCommonParameters->pDocEnd_ = newDocEnd();
	return (pIPDRCommonParameters);
}


/*!
 *  NAME:
 *      initIPDRCommonParameters() - Initializes IPDRCommonParameters 
 *      structure
 *
 *  DESCRIPTION:
 *      1. This function initializes the IPDRCommonParameters
 *         with the values passed as in input parameters
 *	
 *  ARGUMENTS:
 *       -	pIPDRCommonParameters {IN/OUT} 
 *       -	docType {IN} 
 *       -	streamHandle {IN} 
 *       -	docState {IN} 
 *       -	ipdrCount {IN} 
 *       -	DescriptorLookupHashTable {IN} 
 *       -	ListAttributeDescriptor {IN} 
 *       -	descriptorCount {IN} 
 *       -	ListErrorStructure {IN} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


int initIPDRCommonParameters(
				IPDRCommonParameters* pIPDRCommonParameters,
				DocType docType,
				void* streamHandle,
                                char* streamName,
				DocState docState,
				long ipdrCount,
				int noOfBuckets,
				ListAttributeDescriptor* ListAttributeDescriptor,
				int descriptorCount,
				ListErrorStructure* ListErrorStructure,
				ErrorStructure* pExceptionStructure,
				ListFNFData* pListFNFData
				)
{
        unsigned int lenStreamName = 0;
	if (pIPDRCommonParameters == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	pIPDRCommonParameters->DocType_ = docType;
	pIPDRCommonParameters->pStreamHandle_ = streamHandle;

        if (streamName != NULL) {
            lenStreamName = strlen(streamName) + 1;
        } else {
            lenStreamName = strlen(IPDR_NULL) + 1;
        }
        pIPDRCommonParameters->pStreamName_ = (char *) calloc
                                              (lenStreamName, sizeof(char));
        if (streamName != NULL) {
        strcpy(pIPDRCommonParameters->pStreamName_, streamName);
        } else {
        strcpy(pIPDRCommonParameters->pStreamName_, IPDR_NULL);
        }
	pIPDRCommonParameters->DocState_ = docState;
	pIPDRCommonParameters->IPDRCount_.ipdrRecordCount_ = 
		(long) ipdrCount;
	pIPDRCommonParameters->pDescriptorLookupHashTable_ = 
		HashTableCreate(noOfBuckets);
	pIPDRCommonParameters->pDescriptorLookupList_ = 
		ListAttributeDescriptor;
	pIPDRCommonParameters->descriptorCount_ = descriptorCount;
	pIPDRCommonParameters->pListErrorStructure_ = ListErrorStructure;
	pIPDRCommonParameters->pListFNFData_ = pListFNFData;
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      freeIPDRCommonParameters() - Memory deallocation of 
 *      IPDRCommonParameters structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory deallocation of 
 *         IPDRCommonParameters structure & its members.
 *	
 *  ARGUMENTS:
 *       -	IPDRCommonParameters {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int freeIPDRCommonParameters(
			IPDRCommonParameters* pIPDRCommonParameters)
{
	if (pIPDRCommonParameters == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}
	
	if (pIPDRCommonParameters->pStreamHandle_ != NULL)
	fclose(pIPDRCommonParameters->pStreamHandle_);
	
        if (pIPDRCommonParameters->pStreamName_ != NULL)
               free(pIPDRCommonParameters->pStreamName_);
	freeListErrorStructure(&(pIPDRCommonParameters->pListErrorStructure_));
	
	freeListAttributeDescriptor(
		&pIPDRCommonParameters->pDescriptorLookupList_);
	freeSchemaData(pIPDRCommonParameters->pSchema_);
	
	free(pIPDRCommonParameters->pExceptionStructure_->errorMessage_);
	free(pIPDRCommonParameters->pExceptionStructure_);
	
	HashTableDestroy(pIPDRCommonParameters->pDescriptorLookupHashTable_);  

	freeDocEnd(pIPDRCommonParameters->pDocEnd_);
	
	if (pIPDRCommonParameters->pListFNFData_ != NULL) {
		freeListFNFData(&(pIPDRCommonParameters->pListFNFData_));
	}
	
	free(pIPDRCommonParameters);

	return (IPDR_OK);
}

/*!
 *  NAME:
 *      printIPDRCommonParameters() - prints 
 *         IPDRCommonParameters structure
 *
 *  DESCRIPTION:
 *      1. This function prints the 
 *         IPDRCommonParameters structure & its members.
 *	
 *  ARGUMENTS:
 *       -	pIPDRCommonParameters {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int printIPDRCommonParameters(
		IPDRCommonParameters* pIPDRCommonParameters)
{
	if (pIPDRCommonParameters == NULL) {
        return (IPDR_NULL_INPUT_PARAMETER);
	}
	printf("\nIPDRCommonParameters->DocState = %d", 
		pIPDRCommonParameters->DocState_);
	printf("\nIPDRCommonParameters->descriptorCount = %d", 
		pIPDRCommonParameters->descriptorCount_);
	printf("\nIPDRCommonParameters->DocType = %d", 
		pIPDRCommonParameters->DocType_);
	printf("\nIPDRCommonParameters->IPDRCount.ipdrRecordCount = %d",
		pIPDRCommonParameters->IPDRCount_.ipdrRecordCount_);
	printf("\nIPDRCommonParameters->IPDRCount.offsetValue = %d", 
		pIPDRCommonParameters->IPDRCount_.offsetValue_);
	if (pIPDRCommonParameters->pStreamHandle_ != NULL)
	printf("\nIPDRCommonParameters->StreamHandle = %d", 
		pIPDRCommonParameters->pStreamHandle_);
	else
		printf("\nIPDRCommonParameters->StreamHandle = <null>");

	printListAttributeDescriptor
		(pIPDRCommonParameters->pDescriptorLookupList_);
	printListErrorStructure
		(pIPDRCommonParameters->pListErrorStructure_);

	
	return (IPDR_OK);
}


/*!
 *  NAME:
 *      addListNameSpaceInfo() - adds node to the 
 *           NameSpaceInfo list.
 * 
 *  DESCRIPTION:
 *      1. This function adds the node to the NameSpaceInfo
 *         list with the values passed in the input parameters
 *	
 *  ARGUMENTS:
 *       - pHeadRef, pointer to ListNameSpaceInfo {IN/OUT}
 *       - NameSpaceInfoURI {IN}
 *       - NameSpaceInfoID {IN}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int addListNameSpaceInfo(
			ListNameSpaceInfo** pHeadRef, 
			char* NameSpaceInfoURI, 
			char* NameSpaceInfoID)
{
	unsigned int lenNameSpaceID = 0;
	unsigned int lenNameSpaceURI = 0;
	
	ListNameSpaceInfo* pListNameSpaceInfo = (ListNameSpaceInfo *) 
		calloc (1, sizeof(ListNameSpaceInfo));

	if (pListNameSpaceInfo == NULL) {
		return (IPDR_MEMORY_ALLOCATION_FAILED);
	}

	lenNameSpaceID = strlen(NameSpaceInfoID) + 1;
	pListNameSpaceInfo->NameSpaceInfo_.nameSpaceID_ = (char *) calloc 
		(lenNameSpaceID, sizeof(char));
	if (pListNameSpaceInfo->NameSpaceInfo_.nameSpaceID_ == NULL) {
		return (IPDR_MEMORY_ALLOCATION_FAILED);
	}


	lenNameSpaceURI = strlen(NameSpaceInfoURI) + 1;
	pListNameSpaceInfo->NameSpaceInfo_.nameSpaceURI_ = (char *) calloc 
		(lenNameSpaceURI, sizeof(char));
	if (pListNameSpaceInfo->NameSpaceInfo_.nameSpaceURI_ == NULL) {
		return (IPDR_MEMORY_ALLOCATION_FAILED);
	}

	strcpy(pListNameSpaceInfo->NameSpaceInfo_.nameSpaceID_, 
		NameSpaceInfoID);
	strcpy(pListNameSpaceInfo->NameSpaceInfo_.nameSpaceURI_, 
		NameSpaceInfoURI);
	
	pListNameSpaceInfo->pNext_ = *pHeadRef;
	*pHeadRef = pListNameSpaceInfo;

	return (IPDR_OK);
}

/*!
 *  NAME:
 *      appendListNameSpaceInfo() - appends node to the 
 *           NameSpaceInfo list.
 * 
 *  DESCRIPTION:
 *      1. This function appends the node to the NameSpaceInfo
 *         list with the values passed in the input parameters
 *	
 *  ARGUMENTS:
 *       - pHeadRef, pointer to ListNameSpaceInfo {IN/OUT}
 *       - NameSpaceInfoURI {IN}
 *       - NameSpaceInfoID {IN}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int appendListNameSpaceInfo(ListNameSpaceInfo **pHeadRef,
							char* NameSpaceInfoID,
							char* NameSpaceInfoURI 
							)
{
	ListNameSpaceInfo* pCurrent = *pHeadRef;
	
	if (pHeadRef == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}
 
	if ((NameSpaceInfoURI == NULL) || (NameSpaceInfoID == NULL)) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	if (pCurrent == NULL) {
		addListNameSpaceInfo(pHeadRef, NameSpaceInfoURI, NameSpaceInfoID);
	} else {
    
		while (pCurrent->pNext_ != NULL) {
			pCurrent = pCurrent->pNext_;
		}
        addListNameSpaceInfo
		(&(pCurrent->pNext_), NameSpaceInfoURI, NameSpaceInfoID);
	}
	
	return (IPDR_OK);
}


/*!
 *  NAME:
 *      getLengthListNameSpaceInfo() - calculates length of  
 *          the NameSpaceInfo list.
 * 
 *  DESCRIPTION:
 *      1. This function calculates length of the NameSpaceInfo
 *         list
 *	
 *  ARGUMENTS:
 *       - pListNameSpaceInfo {IN/OUT}
 *
 *  RETURNS 	
 *	     Returns length if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int getLengthListNameSpaceInfo(ListNameSpaceInfo* pListNameSpaceInfo)
{
	int count = 0;
	ListNameSpaceInfo* pCurrent = pListNameSpaceInfo;

	if (pListNameSpaceInfo == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	for (pCurrent = pListNameSpaceInfo; pCurrent != NULL; 
	pCurrent = pCurrent->pNext_)
		count++;
	return (count);
}


/*!
 *  NAME:
 *      freeListNameSpaceInfo() - Memory deallocation of 
 *      ListNameSpaceInfo structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory deallocation of 
 *         ListNameSpaceInfo structure & its members.
 *	
 *  ARGUMENTS:
 *       -	ListNameSpaceInfo {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


int freeListNameSpaceInfo(ListNameSpaceInfo **pHeadRef)
{
	ListNameSpaceInfo* pCurrent;
	ListNameSpaceInfo* pNext_;
	if (pHeadRef == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}
 
	pCurrent = *pHeadRef;
    while (pCurrent != NULL) {
        pNext_ = pCurrent->pNext_;
    
		free(pCurrent->NameSpaceInfo_.nameSpaceID_);
		free(pCurrent->NameSpaceInfo_.nameSpaceURI_);
		free(pCurrent);
		pCurrent = pNext_;
    }
    free(pCurrent);
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      printListNameSpaceInfo() - prints 
 *         ListNameSpaceInfo structure
 *
 *  DESCRIPTION:
 *      1. This function prints the 
 *         ListNameSpaceInfo structure & its members.
 *	
 *  ARGUMENTS:
 *       -	pListNameSpaceInfo {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int printListNameSpaceInfo(ListNameSpaceInfo* pListNameSpaceInfo)
{
	ListNameSpaceInfo* pCurrent = pListNameSpaceInfo;
	int count = 0;
	if(pCurrent == NULL)  
		return(IPDR_NULL_LINKLIST);
	
	while (pCurrent != NULL) {
		printf("\n\nDisplaying Record %d", count + 1);
		printf("\nNameSpaceInfo URI \t= %s", 
			pCurrent->NameSpaceInfo_.nameSpaceURI_);
		printf("\nNameSpaceInfo ID \t= %s", 
			pCurrent->NameSpaceInfo_.nameSpaceID_);

	    pCurrent = pCurrent->pNext_;	
	    count++;
	}
	
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      newIPDRHeader() - Memory allocation of IPDRHeader structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory allocation of 
 *         IPDRHeader structure & its members.
 *	
 *  ARGUMENTS:
 *       -	void 
 *
 *  RETURNS 	
 *	     Returns Pointer to IPDRHeader structure
 */

IPDRHeader* newIPDRHeader(void)
{
	IPDRHeader* pIPDRHeader;
	pIPDRHeader = (IPDRHeader *) calloc(1, sizeof(IPDRHeader));
	pIPDRHeader->majorVersion_ = 0;
	pIPDRHeader->minorVersion_ = 0;
	pIPDRHeader->ipdrRecorderInfo_ = NULL;
	pIPDRHeader->startTime_ = 0;
	pIPDRHeader->defaultNameSpaceURI_ = NULL;
	pIPDRHeader->docID_ = NULL;
	pIPDRHeader->defaultNameSpaceURI_ = (char *) calloc 
	  (MAX_NAMESPACE_URI, sizeof(char));
	pIPDRHeader->pListServiceDefinitionURI_ = NULL;
	pIPDRHeader->pOtherNameSpaceInfo_ = NULL;
	pIPDRHeader->pListSchemaNameSpace_ = NULL;

	return (pIPDRHeader);	
}


/*!
 *  NAME:
 *      initIPDRHeader() - Initializes IPDRHeader 
 *      structure
 *
 *  DESCRIPTION:
 *      1. This function initializes the IPDRHeader
 *         with the values passed as in input parameters
 *	
 *  ARGUMENTS:
 *       -	pIPDRHeader {IN/OUT} 
 *       -	version {IN} 
 *       -	ipdrRecorderInfo {IN} 
 *       -	startTime {IN} 
 *       -	defaultNameSpaceURI {IN} 
 *       -	docId {IN} 
 *       -	pListNameSpaceInfo {IN} 
 *       -	pListServiceDefinitionURI {IN} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int initIPDRHeader(
				   IPDRHeader* pIPDRHeader,
				   int majorVersion,
				   char* ipdrRecorderInfo,
				   hyper startTime,
				   char* defaultNameSpaceURI,
				   char* docId,
				   ListNameSpaceInfo* pListNameSpaceInfo,
				   ListServiceDefinitionURI* pListServiceDefinitionURI
				  )
{
	unsigned int lenDocId = 0;
	unsigned int lenIPDRRecorderInfo = 0;

	if (pIPDRHeader == NULL)
		return (IPDR_NULL_INPUT_PARAMETER);

	if (ipdrRecorderInfo == NULL)
		strcpy(ipdrRecorderInfo, IPDR_NULL);

	if (defaultNameSpaceURI == NULL)
		strcpy(defaultNameSpaceURI, IPDR_NULL);

	if (docId == NULL)
		strcpy(docId, IPDR_NULL);
	
	pIPDRHeader->majorVersion_ = majorVersion;
	pIPDRHeader->minorVersion_ = 0;

	lenIPDRRecorderInfo = strlen(ipdrRecorderInfo) + 1;
	pIPDRHeader->ipdrRecorderInfo_ = (char *) calloc (lenIPDRRecorderInfo, sizeof(char));

	strcpy(pIPDRHeader->ipdrRecorderInfo_, ipdrRecorderInfo);
	pIPDRHeader->startTime_ = 12345;
	strcpy(pIPDRHeader->defaultNameSpaceURI_, defaultNameSpaceURI);
	lenDocId = strlen(docId) + 1;
	pIPDRHeader->docID_ = (char *) calloc (lenDocId, sizeof(char));
	strcpy(pIPDRHeader->docID_, docId);
	pIPDRHeader->pOtherNameSpaceInfo_ = pListNameSpaceInfo;
	pIPDRHeader->pListServiceDefinitionURI_ = pListServiceDefinitionURI;

	return (IPDR_OK);
}

/*!
 *  NAME:
 *      freeIPDRHeader() - Memory deallocation of 
 *      IPDRHeader structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory deallocation of 
 *         IPDRHeader structure & its members.
 *	
 *  ARGUMENTS:
 *       -	IPDRHeader {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */



int freeIPDRHeader(IPDRHeader* pIPDRHeader)
{
	if (pIPDRHeader == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	free(pIPDRHeader->ipdrRecorderInfo_);
	free(pIPDRHeader->defaultNameSpaceURI_);
	pIPDRHeader->defaultNameSpaceURI_ = NULL;

	free(pIPDRHeader->docID_);
	freeListServiceDefinitionURI(&(pIPDRHeader->pListServiceDefinitionURI_));
	freeListNameSpaceInfo(&pIPDRHeader->pOtherNameSpaceInfo_);
	free(pIPDRHeader);


	return (IPDR_OK);
}

/*!
 *  NAME:
 *      printIPDRHeader() - prints 
 *         IPDRHeader structure
 *
 *  DESCRIPTION:
 *      1. This function prints the 
 *         IPDRHeader structure & its members.
 *	
 *  ARGUMENTS:
 *       -	pIPDRHeader {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int printIPDRHeader(IPDRHeader* pIPDRHeader)
{
	if (pIPDRHeader == NULL) {
        return (IPDR_NULL_INPUT_PARAMETER);
	}
	printf("\nIPDRHeader->majorVersion = %d", pIPDRHeader->majorVersion_);
	printf("\nIPDRHeader->minorVersion = %d", pIPDRHeader->minorVersion_);
	printf("\nIPDRHeader->ipdrRecorderInfo = %s", 
		pIPDRHeader->ipdrRecorderInfo_);
	printf("\nIPDRHeader->startTime = %.lf", 
		(double)pIPDRHeader->startTime_);
	printf("\nIPDRHeader->defaultNameSpaceURI = %s", 
		pIPDRHeader->defaultNameSpaceURI_);
	printListNameSpaceInfo(pIPDRHeader->pOtherNameSpaceInfo_);
	printListServiceDefinitionURI(
		pIPDRHeader->pListServiceDefinitionURI_);	
	printf("\nIPDRHeader->docId = %s", pIPDRHeader->docID_);

	return (IPDR_OK);

}

/*!
 *  NAME:
 *      newNameSpaceInfo() - Memory allocation of NameSpaceInfo 
 *      structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory allocation of 
 *         NameSpaceInfo structure & its members.
 *	
 *  ARGUMENTS:
 *       -	void 
 *
 *  RETURNS 	
 *	     Returns Pointer to NameSpaceInfo structure
 */

NameSpaceInfo* newNameSpaceInfo(void)
{
	NameSpaceInfo* pNameSpaceInfo;
	pNameSpaceInfo = (NameSpaceInfo *) calloc(1, sizeof(NameSpaceInfo));

	pNameSpaceInfo->nameSpaceURI_ = (char *) calloc(MAX_NAMESPACE_URI, sizeof(char));
	return (pNameSpaceInfo);
}


/*!
 *  NAME:
 *      initNameSpaceInfo() - Initializes NameSpaceInfo 
 *      structure
 *
 *  DESCRIPTION:
 *      1. This function initializes the NameSpaceInfo
 *         with the values passed as in input parameters
 *	
 *  ARGUMENTS:
 *       -	pNameSpaceInfo {IN/OUT} 
 *       -	nameSpaceURI {IN} 
 *       -	nameSpaceID {IN} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int initNameSpaceInfo(
					  NameSpaceInfo* pNameSpaceInfo,
					  char* nameSpaceURI,
					  char* nameSpaceID
					  )
{
	if (pNameSpaceInfo == NULL)
		return (IPDR_NULL_INPUT_PARAMETER);

	if (nameSpaceURI == NULL)
		strcpy(nameSpaceURI, IPDR_NULL);

	if (nameSpaceID == NULL)
		strcpy(nameSpaceID, IPDR_NULL);
	
	strcpy(pNameSpaceInfo->nameSpaceID_, nameSpaceID);
	strcpy(pNameSpaceInfo->nameSpaceURI_, nameSpaceURI);

	return (IPDR_OK);
}


/*!
 *  NAME:
 *      freeNameSpaceInfo() - Memory deallocation of 
 *      NameSpaceInfo structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory deallocation of 
 *         NameSpaceInfo structure & its members.
 *	
 *  ARGUMENTS:
 *       -	NameSpaceInfo {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int freeNameSpaceInfo(NameSpaceInfo* pNameSpaceInfo)
{
	if (pNameSpaceInfo == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	free(pNameSpaceInfo->nameSpaceID_);
	free(pNameSpaceInfo->nameSpaceURI_);
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      printNameSpaceInfo() - prints 
 *         NameSpaceInfo structure
 *
 *  DESCRIPTION:
 *      1. This function prints the 
 *         NameSpaceInfo structure & its members.
 *	
 *  ARGUMENTS:
 *       -	pNameSpaceInfo {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


int printNameSpaceInfo(NameSpaceInfo* pNameSpaceInfo)
{
	if (pNameSpaceInfo == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	printf("\nNameSpaceInfo URI = %s", pNameSpaceInfo->nameSpaceURI_);
	printf("\nNameSpaceInfo ID = %s", pNameSpaceInfo->nameSpaceID_);
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      newAttributeDescriptor() - Memory allocation of 
 *      AttributeDescriptor structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory allocation of 
 *         AttributeDescriptor structure & its members.
 *	
 *  ARGUMENTS:
 *       -	void 
 *
 *  RETURNS 	
 *	     Returns Pointer to AttributeDescriptor structure
 */


AttributeDescriptor* newAttributeDescriptor(void)
{

	AttributeDescriptor* pAttrDescr;
	pAttrDescr = (AttributeDescriptor *) calloc (1, sizeof
		                              (AttributeDescriptor));
	
	pAttrDescr->attributeName_ = (char *) calloc (MAX_ATTRIBUTE_NAME, sizeof(char));
	pAttrDescr->attributeType_ = (char *) calloc (MAX_ATTRIBUTE_TYPE, sizeof(char));
	pAttrDescr->description_ = (char *) calloc (MAX_ATTRIBUTE_CLASS, sizeof(char));
	pAttrDescr->derivedType_ = (char *) calloc (MAX_ATTRIBUTE_TYPE, sizeof(char));


	return (pAttrDescr);
}


/*!
 *  NAME:
 *      initAttributeDescriptor() - Initializes AttributeDescriptor 
 *      structure
 *
 *  DESCRIPTION:
 *      1. This function initializes the AttributeDescriptor
 *         with the values passed as in input parameters
 *	
 *  ARGUMENTS:
 *       -	pAttributeDescriptor {IN/OUT} 
 *       -	AttributeName {IN} 
 *       -	AttributeType {IN} 
 *       -	AttributeDescription {IN} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int initAttributeDescriptor(
							AttributeDescriptor* pAttributeDescriptor,
							char* AttributeName,
							char* AttributeType,
							char* AttributeDescription,
							char* AttributeDerivedType
							)
{
	if (pAttributeDescriptor == NULL)
		return (IPDR_NULL_INPUT_PARAMETER);

	if (AttributeName == NULL)
		strcpy(AttributeName, IPDR_NULL);

	if (AttributeType == NULL)
		strcpy(AttributeType, IPDR_NULL);
	
	if (AttributeDescription == NULL)
		strcpy(AttributeDescription, IPDR_NULL);

	pAttributeDescriptor->attributeName_ = AttributeName;
	pAttributeDescriptor->attributeType_ = AttributeType;
	pAttributeDescriptor->description_ = AttributeDescription;

	if (AttributeDerivedType == NULL)
		strcpy(pAttributeDescriptor->derivedType_, AttributeType);
	else
		strcpy(pAttributeDescriptor->derivedType_, AttributeDerivedType);

	return (IPDR_OK);
}

/*!
 *  NAME:
 *      printAttributeDescriptor() - prints 
 *         AttributeDescriptor structure
 *
 *  DESCRIPTION:
 *      1. This function prints the 
 *         AttributeDescriptor structure & its members.
 *	
 *  ARGUMENTS:
 *       -	pAttributeDescriptor {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


int printAttributeDescriptor(AttributeDescriptor* pAttributeDescriptor)
{
	if (pAttributeDescriptor == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}
	printf("\nAttribute Name = %s", 
		            pAttributeDescriptor->attributeName_);
	printf("\nAttribute Type = %s", 
		            pAttributeDescriptor->attributeType_);
	printf("\nAttribute description = %s", 
		            pAttributeDescriptor->description_);
	printf("\nAttribute derived type = %s", 
		            pAttributeDescriptor->derivedType_);

	return (IPDR_OK);
}

/*!
 *  NAME:
 *      freeAttributeDescriptor() - Memory deallocation of 
 *      AttributeDescriptor structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory deallocation of 
 *         AttributeDescriptor structure & its members.
 *	
 *  ARGUMENTS:
 *       -	AttributeDescriptor {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int freeAttributeDescriptor(AttributeDescriptor* pAttributeDescriptor)
{
	if (pAttributeDescriptor == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	free(pAttributeDescriptor->attributeName_);
	free(pAttributeDescriptor->attributeType_);
	free(pAttributeDescriptor->description_);
	free(pAttributeDescriptor->derivedType_);
	free(pAttributeDescriptor);
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      appendListAttributeDescriptor() - appends node to the 
 *           AttributeDescriptor list.
 * 
 *  DESCRIPTION:
 *      1. This function appends the node to the AttributeDescriptor
 *         list with the values passed in the input parameters
 *	
 *  ARGUMENTS:
 *       - pHeadRef, pointer to ListAttributeDescriptor {IN/OUT}
 *       - attributeName {IN}
 *       - attributeType {IN}
 *       - description {IN}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
 
int appendListAttributeDescriptor(ListAttributeDescriptor** pHeadRef, 
      char* attributeName, 
      char* attributeType,
	  char* description,
	  char* derivedType
	)
{
	int lenAttributeName = 0;
	int lenAttributeType = 0;
	int lenDescription = 0;
	int lenDerivedType = 0;

	ListAttributeDescriptor* pListAttributeDescriptor = NULL;
 
	ListAttributeDescriptor* pCurrent = *pHeadRef;
 
	/* Pointer to Linked list is NULL */
	if (pHeadRef == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}
 
	/* Validating input parameters for NULL value */
	if ((attributeName == NULL) || (attributeType == NULL) || (derivedType == NULL)) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}
 
	if (pCurrent == NULL) {
		pListAttributeDescriptor = 
	(ListAttributeDescriptor *) calloc (1, sizeof(
	struct ListAttributeDescriptor));
 
	/* Allocating memory to AttributeDescriptor */
	pListAttributeDescriptor->pAttributeDescriptor_
        = (AttributeDescriptor*)calloc(1, sizeof(AttributeDescriptor));
 
	if (pListAttributeDescriptor == NULL) {
		return (IPDR_MEMORY_ALLOCATION_FAILED);
	}
  
	/* Allocating memory to AttributeDescriptor->description MAX*/
	pListAttributeDescriptor->pAttributeDescriptor_->description_ = 
		(char *) calloc (MAX_ATTRIBUTE_CLASS, sizeof(char));
	
	if (pListAttributeDescriptor->pAttributeDescriptor_->description_== 
		NULL) {
		return (IPDR_MEMORY_ALLOCATION_FAILED);
	}
 
	strcpy(pListAttributeDescriptor->pAttributeDescriptor_->description_,
               description);


	/* Allocating memory to AttributeDescriptor->attributeName */
	lenAttributeName = strlen(attributeName) + 1;
	pListAttributeDescriptor->pAttributeDescriptor_->attributeName_ = 
          (char *) calloc (lenAttributeName, sizeof(char));
  
	if (pListAttributeDescriptor->pAttributeDescriptor_->attributeName_== 
		NULL) {
		return (IPDR_MEMORY_ALLOCATION_FAILED);
	}
 
	strcpy(pListAttributeDescriptor->pAttributeDescriptor_->attributeName_,
               attributeName);

	/* Allocating memory to AttributeDescriptor->attributeType */
	lenAttributeType = strlen(attributeType) + 1;
	pListAttributeDescriptor->pAttributeDescriptor_->attributeType_ = 
          (char *) calloc (lenAttributeType, sizeof(char));
  
	if (pListAttributeDescriptor->pAttributeDescriptor_->attributeType_== 
		NULL) {
		return (IPDR_MEMORY_ALLOCATION_FAILED);
	}
 	strcpy(pListAttributeDescriptor->pAttributeDescriptor_->attributeType_,
               attributeType);
	
	
	/* Allocating memory to AttributeDescriptor->derivedType MAX*/
	pListAttributeDescriptor->pAttributeDescriptor_->derivedType_ = 
		(char *) calloc (MAX_ATTRIBUTE_TYPE, sizeof(char));
	
	if (pListAttributeDescriptor->pAttributeDescriptor_->derivedType_== 
		NULL) {
		return (IPDR_MEMORY_ALLOCATION_FAILED);
	}
 
	strcpy(pListAttributeDescriptor->pAttributeDescriptor_->derivedType_,
               derivedType);


	pListAttributeDescriptor->pNext_ = *pHeadRef; 
	*pHeadRef = pListAttributeDescriptor; 
 
	} else {
    
		while (pCurrent->pNext_ != NULL) {
			pCurrent = pCurrent->pNext_;
		}
    pListAttributeDescriptor = 
	(ListAttributeDescriptor *) calloc (1, sizeof(
	struct ListAttributeDescriptor));
 
	pListAttributeDescriptor->pAttributeDescriptor_
        = (AttributeDescriptor*)calloc(1, sizeof(AttributeDescriptor));
 
	if (pListAttributeDescriptor == NULL) {
		return (IPDR_MEMORY_ALLOCATION_FAILED);
	}
  
	/* Allocating memory to AttributeDescriptor->description MAX */
	pListAttributeDescriptor->pAttributeDescriptor_->description_ = 
		(char *) calloc (MAX_ATTRIBUTE_CLASS, sizeof(char));

	if (pListAttributeDescriptor->pAttributeDescriptor_->description_== 
		NULL) {
		return (IPDR_MEMORY_ALLOCATION_FAILED);
	}
 
	strcpy(pListAttributeDescriptor->pAttributeDescriptor_->description_,
               description);

	/* Allocating memory to AttributeDescriptor->attributeName */
	lenAttributeName = strlen(attributeName) + 1;
	pListAttributeDescriptor->pAttributeDescriptor_->attributeName_ = 
          (char *) calloc (lenAttributeName, sizeof(char));
  
	if (pListAttributeDescriptor->pAttributeDescriptor_->attributeName_== 
		NULL) {
		return (IPDR_MEMORY_ALLOCATION_FAILED);
	}
 
	strcpy(pListAttributeDescriptor->pAttributeDescriptor_->attributeName_,
               attributeName);

	/* Allocating memory to AttributeDescriptor->attributeType */
	lenAttributeType = strlen(attributeType) + 1;
	pListAttributeDescriptor->pAttributeDescriptor_->attributeType_ = 
          (char *) calloc (lenAttributeType, sizeof(char));
  
	if (pListAttributeDescriptor->pAttributeDescriptor_->attributeType_== 
		NULL) {
		return (IPDR_MEMORY_ALLOCATION_FAILED);
	}
 	strcpy(pListAttributeDescriptor->pAttributeDescriptor_->attributeType_,
               attributeType);
	
	/* Allocating memory to AttributeDescriptor->derivedType */
	pListAttributeDescriptor->pAttributeDescriptor_->derivedType_ = 
		(char *) calloc (MAX_ATTRIBUTE_TYPE, sizeof(char));

	if (pListAttributeDescriptor->pAttributeDescriptor_->derivedType_== 
		NULL) {
		return (IPDR_MEMORY_ALLOCATION_FAILED);
	}
	strcpy(pListAttributeDescriptor->pAttributeDescriptor_->derivedType_,
               derivedType);
	
	pListAttributeDescriptor->pNext_ = NULL; 
	pCurrent->pNext_ = pListAttributeDescriptor; 
 
	}
	
	return (IPDR_OK);
}


/*!
 *  NAME:
 *      checkAttribute() - function checks whether Attribute is optional 
 *                         or not.
 *
 *  DESCRIPTION:
 *      1. function checks whether Attribute is optional or not.
 *		2. depending upon type i.e optional, Manadatory & does not exist
 *         it returns integer value.
 *	
 *  ARGUMENTS:
 *       -	attribute name {IN} (to be checked)
 *       -  list of attribute descriptor {IN/OUT} 
 *                 (attribute name to be checked 
 *                    with respect to this list).
 *
 *  RETURNS 	
 *		Returns 0, if attribute exists & Optional
 *		Returns 1, if attribute exists & Mandatory
 *		Returns 2, if attribute does not exists
 */

int checkAttribute(char *attribute, 
			ListAttributeDescriptor* pListAttributeDescriptor)
{
	ListAttributeDescriptor* pCurrent = pListAttributeDescriptor;
	int count = 0;
	
	if (pCurrent == NULL)  
		return(IPDR_NULL_LINKLIST);
	
	while (pCurrent != NULL) {
	
	    if ((strcmp(attribute, 
			pCurrent->pAttributeDescriptor_->attributeName_) == 0) 
			&& (strcmp(pCurrent->pAttributeDescriptor_->description_, 
			"Optional") == 0))
			return (IPDR_OK);
		
		if ((strcmp(attribute, 
			pCurrent->pAttributeDescriptor_->attributeName_) == 0) 
			&& (strcmp(pCurrent->pAttributeDescriptor_->description_, 
			"Optional") != 0))
		
			return 1;

	    pCurrent = pCurrent->pNext_;	
	    count++;
	}
	
	return 2;
}


/*!
 *  NAME:
 *      freeListAttributeDescriptor() - Memory deallocation of 
 *      ListAttributeDescriptor structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory deallocation of 
 *         ListAttributeDescriptor structure & its members.
 *	
 *  ARGUMENTS:
 *       -	ListAttributeDescriptor {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

 
int freeListAttributeDescriptor(ListAttributeDescriptor** pHeadRef)
{
	ListAttributeDescriptor* pCurrent;
	ListAttributeDescriptor* pNext_;
 
	if (pHeadRef == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}
 
	pCurrent = *pHeadRef;
    while (pCurrent != NULL) {
		if (pCurrent->pAttributeDescriptor_ != NULL) {
        pNext_ = pCurrent->pNext_;
		free(pCurrent->pAttributeDescriptor_->attributeName_);
		free(pCurrent->pAttributeDescriptor_->attributeType_);
		free(pCurrent->pAttributeDescriptor_->description_);
		free(pCurrent->pAttributeDescriptor_->derivedType_);
		
		free (pCurrent->pAttributeDescriptor_);
		pCurrent->pAttributeDescriptor_ = NULL;
		free(pCurrent);
		
		pCurrent = pNext_;
		}
    }
    free(pCurrent);


	return (IPDR_OK);
}


/*!
 *  NAME:
 *      getLengthListAttributeDescriptor() - calculates length of  
 *          the AttributeDescriptor list.
 * 
 *  DESCRIPTION:
 *      1. This function calculates length of the AttributeDescriptor
 *         list
 *	
 *  ARGUMENTS:
 *       - pListAttributeDescriptor {IN/OUT}
 *
 *  RETURNS 	
 *	     Returns length if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int getLengthListAttributeDescriptor(
				ListAttributeDescriptor* pListAttributeDescriptor)
{
	int count = 0;
	ListAttributeDescriptor* pCurrent = pListAttributeDescriptor;

	if (pListAttributeDescriptor == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	for (pCurrent = pListAttributeDescriptor; pCurrent != NULL; 
	pCurrent = pCurrent->pNext_)
		count++;
	return (count);
}

/*!
 *  NAME:
 *      printListAttributeDescriptor() - prints 
 *         ListAttributeDescriptor structure
 *
 *  DESCRIPTION:
 *      1. This function prints the 
 *         ListAttributeDescriptor structure & its members.
 *	
 *  ARGUMENTS:
 *       -	pListAttributeDescriptor {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


int printListAttributeDescriptor(
			ListAttributeDescriptor* pListAttributeDescriptor)
{
	ListAttributeDescriptor* pCurrent = pListAttributeDescriptor;
	int count = 0;
	
	if(pCurrent == NULL) 
		return(IPDR_NULL_LINKLIST);
	
	while (pCurrent != NULL) {

		printf("\n\nDisplaying Record %d", count + 1);
		printf("\nAttribute Name \t= %s", 
			pCurrent->pAttributeDescriptor_->attributeName_);
		printf("\nAttribute Type \t= %s", 
			pCurrent->pAttributeDescriptor_->attributeType_);
		printf("\nAttribute Class = %s", 
			pCurrent->pAttributeDescriptor_->description_);
		printf("\nAttribute derived Type = %s", 
			pCurrent->pAttributeDescriptor_->derivedType_);

	    pCurrent = pCurrent->pNext_;	
	    count++;
	}
	
	return (IPDR_OK);
}




/*!
 *  NAME:
 *      freeListNodeAttributeDescriptor(int node) - Memory 
 *      deallocation of ListAttributeDescriptor structure for
 *      the specified node
 *
 *  DESCRIPTION:
 *      1. This function performs the memory deallocation of 
 *         specified node of ListAttributeDescriptor structure 
 *         & its members.
 *	
 *  ARGUMENTS:
 *       -	ListAttributeDescriptor {IN/OUT} 
 *       -  node {IN}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int freeListNodeAttributeDescriptor(ListAttributeDescriptor** pHeadRef,
									int node)
{
	
	ListAttributeDescriptor *pCurrent = NULL;
	ListAttributeDescriptor *pPrevious = NULL;
	int recordCount = 1;

	for (pCurrent = *pHeadRef, recordCount = 1; pCurrent != NULL; pCurrent = pCurrent->pNext_, recordCount++) {
		if (recordCount == node) 
			break;

		pPrevious = pCurrent;
	}

	if (pCurrent == NULL)
		return (IPDR_OK);

	if (pPrevious != NULL) {
        pPrevious->pNext_ = pCurrent->pNext_;
    } else {
        *pHeadRef = pCurrent->pNext_;
    }

	freeAttributeDescriptor(pCurrent->pAttributeDescriptor_);
	free(pCurrent);

	return (IPDR_OK);
}


/*!
 *  NAME:
 *      newFNFType() - Memory allocation of 
 *      FNFType structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory allocation of 
 *         FNFType structure & its members.
 *	
 *  ARGUMENTS:
 *       -	void 
 *
 *  RETURNS 	
 *	     Returns Pointer to FNFType structure
 */


 
FNFType* newFNFType(void)
{
 
	FNFType* pFNFType;
	pFNFType = (FNFType *) calloc (1, sizeof(FNFType));
	pFNFType->serviceType_ = (char *) calloc (MAX_IPDR_STRING, sizeof(char));
	pFNFType->pListAttributeDescriptor_ = NULL;
	return (pFNFType);
}

/*!
 *  NAME:
 *      initFNFType() - Initializes FNFType 
 *      structure
 *
 *  DESCRIPTION:
 *      1. This function initializes the FNFType
 *         with the values passed as in input parameters
 *	
 *  ARGUMENTS:
 *       -	pFNFType {IN/OUT} 
 *       -	AttributeName {IN} 
 *       -	AttributeType {IN} 
 *       -	AttributeDescription {IN} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int initFNFType(
				FNFType* pFNFType, 
				char* serviceType,
				ListAttributeDescriptor* pListAttributeDescriptor
				)
{
	if (pFNFType == NULL) 
		return (IPDR_NULL_INPUT_PARAMETER);

	if (serviceType == NULL)
		strcpy(pFNFType->serviceType_, IPDR_NULL);

	strcpy(pFNFType->serviceType_, serviceType);
	pFNFType->pListAttributeDescriptor_ = pListAttributeDescriptor;

	return (IPDR_OK);

}

/*!
 *  NAME:
 *      freeFNFType() - Memory deallocation of FNFType structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory deallocation of FNFType 
 *         structure & its members.
 *	
 *  ARGUMENTS:
 *       -	FNFType {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
  
int freeFNFType(FNFType* pFNFType)
{
	if (pFNFType == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}
	free(pFNFType->serviceType_);
	freeListAttributeDescriptor(&(pFNFType->pListAttributeDescriptor_));
	free(pFNFType);
	
	
	return (IPDR_OK);
}


/*!
 *  NAME:
 *      printFNFType() - prints 
 *         FNFType structure
 *
 *  DESCRIPTION:
 *      1. This function prints the 
 *         FNFType structure & its members.
 *	
 *  ARGUMENTS:
 *       -	pFNFType {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


int printFNFType(FNFType* pFNFType)
{
	if (pFNFType == NULL) {
        return (IPDR_NULL_INPUT_PARAMETER);
	}

	if (pFNFType->serviceType_ != NULL)
	printf("\nFNFType->serviceType_ = %s", pFNFType->serviceType_);
	if (pFNFType->pListAttributeDescriptor_ != NULL)
	printListAttributeDescriptor(pFNFType->pListAttributeDescriptor_);
		
	return (IPDR_OK);
}


/*!
* NAME:
*      newSchema() 
*
* DESCRIPTION:
*      This method is used to initialize a  schema object. 
*  
* ARGUMENTS:
*      NONE
*
*  RETURNS  
*      Returns pointer to the initialized schema object.
*/

Schema* newSchema()
{
	Schema* pSchema = (Schema *) calloc (1, sizeof(Schema));
	pSchema->pListSchemaLookup_ = NULL;
	pSchema->pListNameSpaceInfo_ = NULL;
	pSchema->SchemaValidationFlag_ = IPDR_FALSE;
	return pSchema;
}




/*!
* NAME:
*      appendListFNFData() 
*
* DESCRIPTION:
*      This method appends a node to IPDRData List
*  
* ARGUMENTS:
*      NONE
*
*  RETURNS  
*      Returns Error if unsuccessful.
*/

int appendListFNFData(
					ListFNFData** pHeadRef,
					char* typeName,
					ListAttributeDescriptor* pListAttributeDescriptor,
					ListIPDRData* pListIPDRData)
{
	ListFNFData* pCurrent = *pHeadRef;
	if (pHeadRef == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}
 
	if ((typeName == NULL) || (pListAttributeDescriptor == NULL) || 
		(pListIPDRData == NULL)) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	if (pCurrent == NULL) {
		addListFNFData(pHeadRef, typeName, 
			pListAttributeDescriptor, pListIPDRData);
	} else {
    
		while (pCurrent->pNext_ != NULL) {
			pCurrent = pCurrent->pNext_;
		}
        addListFNFData
		(&(pCurrent->pNext_), typeName, 
		pListAttributeDescriptor, pListIPDRData);
	}
	
	return (IPDR_OK);

}

/*!
* NAME:
*      getLengthListFNFData() 
*
* DESCRIPTION:
*      This method returns the no of nodes of IPDR Data list
*  
* ARGUMENTS:
*      NONE
*
*  RETURNS  
*     no of nodes of IPDR Data list
*/
int getLengthListFNFData(ListFNFData* pListFNFData)
{
	int count = 0;
	ListFNFData* pCurrent = pListFNFData;

	if (pListFNFData == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	for (pCurrent = pListFNFData; pCurrent != NULL; 
	pCurrent = pCurrent->pNext_)
		count++;
	return (count);
}



/*!
* NAME:
*      freeListFNFData() 
*
* DESCRIPTION:
*      This method free the IPDRData List
*  
* ARGUMENTS:
*      NONE
*
*  RETURNS  
*     Error, if unSuccessful
*/
int freeListFNFData(ListFNFData **pHeadRef)
{
	ListFNFData* pCurrent;
    ListFNFData* pNext_;
    if (pHeadRef == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
    }
 
    pCurrent = *pHeadRef;
    while (pCurrent != NULL) {
	    pNext_ = pCurrent->pNext_;
                        
        free(pCurrent->pXMLFNFData_->typeName_);
        freeListAttributeDescriptor(&pCurrent->pXMLFNFData_->pListAttributeDescriptor_);  
        freeListIPDRData(&pCurrent->pXMLFNFData_->pListIPDRData_);
        
        free(pCurrent->pXMLFNFData_);

        free(pCurrent);
        pCurrent = pNext_;
    }
    free(pCurrent);
    return (IPDR_OK);
}

 

 

 

/*!
* NAME:
*      addListFNFData() 
*
* DESCRIPTION:
*      This method adds a node to IPDRData List
*  
* ARGUMENTS:
*      NONE
*
*  RETURNS  
*      Returns Error if unsuccessful.
*/


int addListFNFData(
                                                            ListFNFData** pHeadRef,
                                                            char* typeName,
                                                            ListAttributeDescriptor* pListAttributeDescriptor,
                                                            ListIPDRData* pListIPDRData)
{
            
            ListAttributeDescriptor* pCurrentListAttributeDescriptor = NULL;
            ListIPDRData* pFirst = NULL;
            void* IPDRValue = NULL;
    
 
            ListFNFData* pListFNFData = (ListFNFData *) 
                calloc (1, sizeof(ListFNFData));
            
            pListFNFData->pXMLFNFData_ = newXMLFNFData();
            
            if (pListFNFData == NULL) {
                return (IPDR_MEMORY_ALLOCATION_FAILED);
            }
 
 
            if (pListFNFData->pXMLFNFData_ == NULL) {
                return (IPDR_MEMORY_ALLOCATION_FAILED);
            }
                  
            pFirst = pListIPDRData;
 
    while (pListIPDRData != NULL) {
        if (pListIPDRData->IPDRDataType_ == IPDRDATA_BYTEARRAY) {
        appendListIPDRData(&pListFNFData->pXMLFNFData_->pListIPDRData_,
        pListIPDRData->IPDRDataUnion_.IPDRByteArray_,
        IPDRDATA_BYTEARRAY);
     }
     else if (pListIPDRData->IPDRDataType_  == IPDRDATA_DOUBLE) {
      IPDRValue = (double *) &pListIPDRData->IPDRDataUnion_.IPDRDouble_;
        appendListIPDRData(&pListFNFData->pXMLFNFData_->pListIPDRData_,
          IPDRValue,
          IPDRDATA_DOUBLE);
     }
     else if (pListIPDRData->IPDRDataType_  == IPDRDATA_FLOAT) {
      IPDRValue = (float *) &pListIPDRData->IPDRDataUnion_.IPDRFloat_;
       appendListIPDRData(&pListFNFData->pXMLFNFData_->pListIPDRData_,
          IPDRValue,
          IPDRDATA_FLOAT);
     }
     else if (pListIPDRData->IPDRDataType_  == IPDRDATA_LONG) {
      appendListIPDRData(&pListFNFData->pXMLFNFData_->pListIPDRData_,
          (void *) pListIPDRData->IPDRDataUnion_.IPDRLong_,
          IPDRDATA_LONG);
     }
     else if (pListIPDRData->IPDRDataType_ == IPDRDATA_LONGLONG) {
      appendListIPDRData(&pListFNFData->pXMLFNFData_->pListIPDRData_,
          (void *) &(pListIPDRData->IPDRDataUnion_.IPDRLongLong_),
          IPDRDATA_LONGLONG);
     }
     else if (pListIPDRData->IPDRDataType_ == IPDRDATA_ULONG) {
      appendListIPDRData(&pListFNFData->pXMLFNFData_->pListIPDRData_,
          (void *) pListIPDRData->IPDRDataUnion_.IPDRULong_,
          IPDRDATA_ULONG);
     }
     else if (pListIPDRData->IPDRDataType_ == IPDRDATA_ULONGLONG) {
      appendListIPDRData(&pListFNFData->pXMLFNFData_->pListIPDRData_,
          (void *) &(pListIPDRData->IPDRDataUnion_.IPDRULongLong_),
          IPDRDATA_ULONGLONG);
     }
     else if (pListIPDRData->IPDRDataType_ == IPDRDATA_UTF8ARRAY) {
      appendListIPDRData(&pListFNFData->pXMLFNFData_->pListIPDRData_,
          pListIPDRData->IPDRDataUnion_.IPDRUTF8Array_,
          IPDRDATA_UTF8ARRAY);
     }
 
     pListIPDRData = pListIPDRData->pNext_; 
	}
    pListIPDRData = pFirst;            
    pCurrentListAttributeDescriptor = pListAttributeDescriptor;
 
    while (pListAttributeDescriptor != NULL) {
    appendListAttributeDescriptor(&(pListFNFData->pXMLFNFData_->pListAttributeDescriptor_),
             pListAttributeDescriptor->pAttributeDescriptor_->attributeName_,
             pListAttributeDescriptor->pAttributeDescriptor_->attributeType_,
             pListAttributeDescriptor->pAttributeDescriptor_->description_,
             pListAttributeDescriptor->pAttributeDescriptor_->derivedType_); 
    
    pListAttributeDescriptor = pListAttributeDescriptor->pNext_; 
    }
    
    pListAttributeDescriptor = pCurrentListAttributeDescriptor;
    
    strcpy(pListFNFData->pXMLFNFData_->typeName_, typeName);
    pListFNFData->pXMLFNFData_->XMLFlag_ = 0;
    pListFNFData->pNext_ = *pHeadRef;
    *pHeadRef = pListFNFData;        
 
    return (IPDR_OK);
}

/*!
* NAME:
*      freeHeadListFNFData() 
*
* DESCRIPTION:
*      This method free the first node of IPDRData List
*  
* ARGUMENTS:
*      NONE
*
*  RETURNS  
*     Error, if unSuccessful
*/


int freeHeadListFNFData(ListFNFData **pHeadRef)
{
    ListFNFData* pCurrent;
    ListFNFData* pNext_;
    if(pHeadRef == NULL) {
       return (IPDR_NULL_INPUT_PARAMETER);
    }
    pCurrent = *pHeadRef;
	pNext_ = pCurrent->pNext_;

		free(pCurrent->pXMLFNFData_->typeName_);
		freeListAttributeDescriptor(&(pCurrent->pXMLFNFData_->pListAttributeDescriptor_));  
                pCurrent->pXMLFNFData_->pListAttributeDescriptor_ = NULL;
		freeListIPDRData(&pCurrent->pXMLFNFData_->pListIPDRData_);
		free(pCurrent->pXMLFNFData_);

		free(pCurrent);
		pCurrent = pNext_;

	*pHeadRef = pCurrent;
    return (IPDR_OK);

}


int printListFNFData(ListFNFData* pListFNFData)
{
	ListFNFData* pCurrent = pListFNFData;
	int count = 0;
	if(pCurrent == NULL) 
		return(IPDR_NULL_LINKLIST);
	
	while (pCurrent != NULL) {

		printf("\n\nDisplaying Record %d", count + 1);
		printf("\npXMLFNFData->typeName \t= %s", 
			pCurrent->pXMLFNFData_->typeName_);
		printListAttributeDescriptor(
			pCurrent->pXMLFNFData_->pListAttributeDescriptor_);
		printListIPDRData(
			pCurrent->pXMLFNFData_->pListIPDRData_);

	    pCurrent = pCurrent->pNext_;	
	    count++;
	}
	
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      newXMLFNFData() - Memory allocation of FNFData structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory allocation of 
 *         FNFData structure & its members.
 *	
 *  ARGUMENTS:
 *       -	void 
 *
 *  RETURNS 	
 *	     Returns Pointer to FNFData structure
 */


XMLFNFData* newXMLFNFData(void)
{
	XMLFNFData* pXMLFNFData ;
	pXMLFNFData = (XMLFNFData *) calloc (1, sizeof(XMLFNFData));
	pXMLFNFData->typeName_ = (char *) calloc (sizeof(char), 
		MAX_ATTRIBUTE_TYPE);
	pXMLFNFData->pListAttributeDescriptor_ = NULL;
	pXMLFNFData->pListIPDRData_ = NULL;
	return (pXMLFNFData);
}

/*!
 *  NAME:
 *      freeXMLFNFData() - Memory deallocation of 
 *      XMLFNFData structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory deallocation of 
 *         XMLFNFData structure & its members.
 *	
 *  ARGUMENTS:
 *       -	XMLFNFData {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int freeXMLFNFData(XMLFNFData* pXMLFNFData)
{
	if (pXMLFNFData == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	free(pXMLFNFData->typeName_);
	freeListAttributeDescriptor(&(pXMLFNFData->pListAttributeDescriptor_));
	freeListIPDRData(&(pXMLFNFData->pListIPDRData_));

	return (IPDR_OK);
}

/*!
 *  NAME:
 *      printXMLFNFdata() - prints 
 *         XMLFNFdata structure
 *
 *  DESCRIPTION:
 *      1. This function prints the 
 *         XMLFNFdata structure & its members.
 *	
 *  ARGUMENTS:
 *       -	pXMLFNFdata {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int printXMLFNFData(XMLFNFData* pXMLFNFData)
{
	if (pXMLFNFData == NULL) {
		printf("\nNULL\n");
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	printf("\nXMLFNFData->typeName_ = %s", pXMLFNFData->typeName_);
	printListAttributeDescriptor(pXMLFNFData->pListAttributeDescriptor_);
	printListIPDRData(pXMLFNFData->pListIPDRData_);

	return (IPDR_OK);
}

int freeSchemaData(Schema* pSchema)
{
	freeListNameSpaceInfo(&(pSchema->pListNameSpaceInfo_));
	pSchema->SchemaValidationFlag_ = IPDR_FALSE;
	freeListSchemaLookup(&pSchema->pListSchemaLookup_); 

	free(pSchema);

	return (IPDR_OK);
}

/*!
* NAME:
*      newListOfKeys() 
*
* DESCRIPTION:
*      This method is used to initialize the member variable ListOfKeys 
* of Schema object.
* 
*  
* ARGUMENTS:
*      NONE
*
*  RETURNS  
*      Returns pointer to the initialized ListOfKeys .
*/

ListOfKeys* newListOfKeys()
{

	ListOfKeys* pListOfKeys;
        pListOfKeys = (ListOfKeys*) 
		                          calloc(1, sizeof(struct ListOfKeys));
    pListOfKeys->key_ = (char*) calloc(MAX_ATTRIBUTE_LENGTH, sizeof(char));
	pListOfKeys->pNext_ = NULL;
	return ( pListOfKeys);
}

int freeListOfKeys(ListOfKeys** pHeadRef)
{
	ListOfKeys* pCurrent;
	ListOfKeys* pNext_;
 
	if (pHeadRef == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}
 
	pCurrent = *pHeadRef;
    while (pCurrent != NULL) {
        pNext_ = pCurrent->pNext_;
    
		free(pCurrent->key_);
		free(pCurrent);
		pCurrent = pNext_;
    }
    free(pCurrent);

	return (IPDR_OK);
}

/*!
 *  NAME:
 *      addListSchemaNameSpace() - adds node to the 
 *           List SchemaNameSpace. 
 * 
 *  DESCRIPTION:
 *      1. This function adds the node to the SchemaNameSpace
 *         list with the values passed in the input parameters
 *	
 *  ARGUMENTS:
 *       - pHeadRef, pointer to ListSchemaNameSpace {IN/OUT}
 *       - SchemaNameSpace {IN}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


int addListSchemaNameSpace(
						ListSchemaNameSpace** pHeadRef, 
						char* schemaNameSpace)
{
	unsigned int lenSchemaNameSpace = 0;

	ListSchemaNameSpace* pListSchemaNameSpace = 
	(ListSchemaNameSpace *) calloc(1, sizeof(ListSchemaNameSpace));
	
	if (pListSchemaNameSpace == NULL) {
		return (IPDR_MEMORY_ALLOCATION_FAILED);
	}
	
	
	lenSchemaNameSpace = strlen(schemaNameSpace) + 1;
	pListSchemaNameSpace->pSchemaNameSpace_ = (char *) 
		calloc (lenSchemaNameSpace, sizeof(char));

	if (pListSchemaNameSpace->pSchemaNameSpace_ == NULL) {
		return (IPDR_MEMORY_ALLOCATION_FAILED);
	}

	strcpy(pListSchemaNameSpace->pSchemaNameSpace_, 
		schemaNameSpace);
	pListSchemaNameSpace->pNext_ = *pHeadRef;
	*pHeadRef = pListSchemaNameSpace;

	return (IPDR_OK);
}


/*!
 *  NAME:
 *      appendListSchemaNameSpace() - appends node to the 
 *           SchemaNameSpace list.
 * 
 *  DESCRIPTION:
 *      1. This function appends the node to the SchemaNameSpace
 *         list with the values passed in the input parameters
 *	
 *  ARGUMENTS:
 *       - pHeadRef, pointer to ListSchemaNameSpace {IN/OUT}
 *       - schemaNameSpace {IN}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


int appendListSchemaNameSpace(ListSchemaNameSpace **pHeadRef,
								   char* schemaNameSpace)
{
	ListSchemaNameSpace* pCurrent = *pHeadRef;
	if (pHeadRef == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}
 
	if (schemaNameSpace == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}
 
	if (pCurrent == NULL) {
		addListSchemaNameSpace(pHeadRef, schemaNameSpace);
	} else {
		while (pCurrent->pNext_ != NULL) {
			pCurrent = pCurrent->pNext_;
		}
        addListSchemaNameSpace
		(&(pCurrent->pNext_), schemaNameSpace);
	}
	
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      getLengthListSchemaNameSpace() - calculates length of  
 *          the schemaNameSpace list.
 * 
 *  DESCRIPTION:
 *      1. This function calculates length of the schemaNameSpace
 *         list
 *	
 *  ARGUMENTS:
 *       - pListSchemaNameSpace {IN/OUT}
 *
 *  RETURNS 	
 *	     Returns length if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int getLengthListSchemaNameSpace(
			ListSchemaNameSpace* pListSchemaNameSpace)
{
	int count = 0;
	ListSchemaNameSpace* pCurrent = pListSchemaNameSpace;

	if (pListSchemaNameSpace == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	for (pCurrent = pListSchemaNameSpace; pCurrent != NULL; 
	pCurrent = pCurrent->pNext_)
		count++;
	return (count);
}


/*!
 *  NAME:
 *      freeListSchemaNameSpace() - Memory deallocation of 
 *      ListSchemaNameSpace structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory deallocation of 
 *         ListSchemaNameSpace structure & its members.
 *	
 *  ARGUMENTS:
 *       -	ListSchemaNameSpace {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int freeListSchemaNameSpace(ListSchemaNameSpace **pHeadRef)
{
	ListSchemaNameSpace* pCurrent;
	ListSchemaNameSpace* pNext_;

	if (pHeadRef == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}
 
	pCurrent = *pHeadRef;
        while (pCurrent != NULL) {
            pNext_ = pCurrent->pNext_;
	    free(pCurrent->pSchemaNameSpace_);
	    free(pCurrent);
	    pCurrent = pNext_;
    }
    free(pCurrent);
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      printListSchemaNameSpace() - prints 
 *         ListSchemaNameSpace structure
 *
 *  DESCRIPTION:
 *      1. This function prints the 
 *         ListSchemaNameSpace structure & its members.
 *	
 *  ARGUMENTS:
 *       -	pListSchemaNameSpace {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int printListSchemaNameSpace(
				ListSchemaNameSpace* pListSchemaNameSpace)
{
	ListSchemaNameSpace* pCurrent = pListSchemaNameSpace;
	int count = 0;
	if(pCurrent == NULL) 
		return(IPDR_NULL_LINKLIST);
	
	while (pCurrent != NULL) {

		printf("\n\nDisplaying Record %d", count + 1);
		printf("\nSchema NameSpace\t= %s", 
			pCurrent->pSchemaNameSpace_);		

	    pCurrent = pCurrent->pNext_;	
	    count++;
	}
	
	return (IPDR_OK);
}


/*!
 *  NAME:
 *      newSchemaElement() - Memory allocation of Schema Element structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory allocation of 
 *         SchemaElement structure & its members.
 *	
 *  ARGUMENTS:
 *       -	void 
 *
 *  RETURNS 	
 *	     Returns Pointer to SchemaElement structure
 */

SchemaElement* newSchemaElement(void)
{
	SchemaElement* pSchemaElement;
	pSchemaElement = (SchemaElement *) calloc(1, sizeof(SchemaElement));
	pSchemaElement->attributeName_ = NULL;
	pSchemaElement->attributeType_ = NULL; 
	pSchemaElement->derivedType_ = NULL; 
	pSchemaElement->documentation_ = NULL; 
	pSchemaElement->reference_ = NULL; 
	pSchemaElement->status_ = NULL; 
	pSchemaElement->unit_ = NULL; 
	pSchemaElement->pListAttributeEnum_ = NULL;
	pSchemaElement->pAttributeOptional_ = (AttributeOptional)IPDR_OPTIONAL;  

	return (pSchemaElement);	
}


/*!
 *  NAME:
 *      initSchemaElement() - Initializes SchemaElement 
 *      structure
 *
 *  DESCRIPTION:
 *      1. This function initializes the SchemaElement
 *         with the values passed as in input parameters
 *	
 *  ARGUMENTS:
 *       -	SchemaElement {IN/OUT} 
 *       -	attributeName {IN} 
 *       -	attributeType {IN} 
 *       -	description {IN} 
 *       -	derivedType {IN} 
 *       -	unit {IN} 
 *       -	reference {IN} 
 *       -	status {IN} 
 *       -  ListAttributeEnum {IN}
 *       -  AttributeOptional {IN}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int initSchemaElement(SchemaElement* pSchemaElement, 
					  char* attributeName,
 					  char* attributeType,
 			          char* documentation,
 			          char* derivedType,
			          char* unit,
			          char* reference,
			          char* status,
			          List* pListAttributeEnum_,
			          AttributeOptional pAttributeOptional_
					)
{
	unsigned int lenAttributeName = 0;
	unsigned int lenAttributeType = 0;
	unsigned int lenDerivedType = 0;
	unsigned int lenDocumentation = 0;
	unsigned int lenUnit = 0;
	unsigned int lenRefernce = 0;
	unsigned int lenStatus = 0;
	unsigned int lenNULL = 0;
	List* pCurrentListAttributeEnum = NULL; 
   ListComplexElement* pCurrentListComplexElement;

	lenNULL = strlen(IPDR_NULL) + 1;

	if (pSchemaElement == NULL)
		return (IPDR_NULL_INPUT_PARAMETER);

	if (attributeName == NULL) {
		pSchemaElement->attributeName_ = (char *) calloc (lenNULL, sizeof(char));
		strcpy(pSchemaElement->attributeName_, IPDR_NULL);
	} else {
		lenAttributeName = strlen(attributeName) + 1;
		pSchemaElement->attributeName_ = (char *) calloc (lenAttributeName, sizeof(char));
		strcpy(pSchemaElement->attributeName_, attributeName);
	}

	if (attributeType == NULL) {
		pSchemaElement->attributeType_ = (char *) calloc (lenNULL, sizeof(char));
		strcpy(pSchemaElement->attributeType_, IPDR_NULL);
	} else {
		lenAttributeType = strlen(attributeType) + 1;
		pSchemaElement->attributeType_ = (char *) calloc (lenAttributeType, sizeof(char));
		strcpy(pSchemaElement->attributeType_, attributeType);
	}

	if (derivedType == NULL) {
		pSchemaElement->derivedType_ = (char *) calloc (lenNULL, sizeof(char));
		strcpy(pSchemaElement->derivedType_, IPDR_NULL);
	} else {
		lenDerivedType = strlen(derivedType) + 1;
		pSchemaElement->derivedType_ = (char *) calloc (lenDerivedType, sizeof(char));
		strcpy(pSchemaElement->derivedType_, derivedType);
	}
	
	if (documentation == NULL) {
		pSchemaElement->documentation_ = (char *) calloc (lenNULL, sizeof(char));
		strcpy(pSchemaElement->documentation_, IPDR_NULL);
	} else {
		lenDocumentation  = strlen(documentation) + 1;
		pSchemaElement->documentation_ = (char *) calloc (lenDocumentation, sizeof(char));
		strcpy(pSchemaElement->documentation_, documentation);
	}
	
	if (reference == NULL) {
		pSchemaElement->reference_ = (char *) calloc (lenNULL, sizeof(char));
		strcpy(pSchemaElement->reference_, IPDR_NULL);
	} else {
		lenRefernce = strlen(reference) + 1;
		pSchemaElement->reference_ = (char *) calloc (lenRefernce, sizeof(char));
		strcpy(pSchemaElement->reference_, reference);
	}
	
	
	if (status == NULL) {
		pSchemaElement->status_ = (char *) calloc (lenNULL, sizeof(char));
		strcpy(pSchemaElement->status_, IPDR_NULL);
	} else {
		lenStatus = strlen(status) + 1;
		pSchemaElement->status_ = (char *) calloc (lenStatus, sizeof(char));
		strcpy(pSchemaElement->status_, status);
	}
	
	if (unit == NULL) {
		pSchemaElement->unit_ = (char *) calloc (lenNULL, sizeof(char));
		strcpy(pSchemaElement->unit_, IPDR_NULL);
	} else {
		lenUnit = strlen(unit) + 1;
		pSchemaElement->unit_ = (char *) calloc (lenUnit, sizeof(char));
		strcpy(pSchemaElement->unit_, unit);
	}
	
	pCurrentListAttributeEnum = pListAttributeEnum_;
	while(pListAttributeEnum_ != NULL) {
		appendNode (&(pSchemaElement->pListAttributeEnum_),
							 pListAttributeEnum_->node);
		pListAttributeEnum_ = pListAttributeEnum_->pNext;
	}
	pListAttributeEnum_ = pCurrentListAttributeEnum;
	
	pSchemaElement->pAttributeOptional_ = pAttributeOptional_; 


   //IPDR v3.5, structures
   /*
   if(pSchemaElement->attributeType_ == SCHEMA_LOOKUP_WRITE_STRCUTURE)
   {
		pCurrentListComplexElement = pListComplexElement_;
		while(pListComplexElement_ != NULL) {
			appendListComplexElement (&(pSchemaElement->pListComplexElement_),
							 pListComplexElement_->pComplexElement_);
			pListComplexElement_ = pListComplexElement_->pNext_;
		}
		pListComplexElement_ = pCurrentListComplexElement;
      pSchemaElement->boundedStrcutureValue_ = boundedStrcutureValue;
   }

   if(pSchemaElement->attributeType_ == SCHEMA_LOOKUP_WRITE_ARRAY)
   	pSchemaElement->boundedArrayValue_ = boundedArrayValue;
   */

	return (IPDR_OK);
}

/*!
 *  NAME:
 *      freeSchemaElement() - Memory deallocation of 
 *      SchemaElement structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory deallocation of 
 *         SchemaElement structure & its members.
 *	
 *  ARGUMENTS:
 *       -	SchemaElement {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */



int freeSchemaElement(SchemaElement * pSchemaElement)
{
	if (pSchemaElement == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}
	
	if (pSchemaElement->attributeName_ != NULL) {
		free(pSchemaElement->attributeName_);
		pSchemaElement->attributeName_ = NULL;
	}

	if (pSchemaElement->attributeType_ != NULL) {
		free(pSchemaElement->attributeType_); 
		pSchemaElement->attributeType_ = NULL;
	}

	if (pSchemaElement->derivedType_ != NULL) {
		free(pSchemaElement->derivedType_); 
		pSchemaElement->derivedType_ = NULL;
	}

	if (pSchemaElement->documentation_ != NULL) {
		free(pSchemaElement->documentation_); 
		pSchemaElement->documentation_ = NULL;
	}

	if (pSchemaElement->reference_ != NULL) {
		free(pSchemaElement->reference_); 
		pSchemaElement->reference_ = NULL;
	}

	if (pSchemaElement->status_ != NULL) {
		free(pSchemaElement->status_); 
		pSchemaElement->status_ = NULL;
	}

	if (pSchemaElement->unit_ != NULL) {
		free(pSchemaElement->unit_); 
		pSchemaElement->unit_ = NULL;
	}

	if (pSchemaElement->pListAttributeEnum_ != NULL) {
		freeList(&(pSchemaElement->pListAttributeEnum_));
		pSchemaElement->pListAttributeEnum_ = NULL;
	}

	free(pSchemaElement);

	return (IPDR_OK);
}

/*!
 *  NAME:
 *      printSchemaElement() - prints 
 *         SchemaElement structure
 *
 *  DESCRIPTION:
 *      1. This function prints the 
 *         SchemaElement structure & its members.
 *	
 *  ARGUMENTS:
 *       -	SchemaElement {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int printSchemaElement(SchemaElement * pSchemaElement)
{
	if (pSchemaElement == NULL) {
        return (IPDR_NULL_INPUT_PARAMETER);
	}
	

	printf("\npSchemaElement->attributeName_ = %s", 
							pSchemaElement->attributeName_);
	printf("\npSchemaElement->attributeType_ = %s", 
							pSchemaElement->attributeType_);
	printf("\npSchemaElement->derivedType_ = %s", 
							pSchemaElement->derivedType_);
	printf("\npSchemaElement->documentation_ = %s", 
							pSchemaElement->documentation_);
	printf("\npSchemaElement->reference_ = %s", 
							pSchemaElement->reference_);
	printf("\npSchemaElement->status_ = %s", 
							pSchemaElement->status_);
	printf("\npSchemaElement->unit_ = %s", 
							pSchemaElement->unit_);
	printList(pSchemaElement->pListAttributeEnum_); 


	return (IPDR_OK);

}


/*!
 *  NAME:
 *      addListAttributeEnum() - adds node to the 
 *           AttributeEnum list.
 * 
 *  DESCRIPTION:
 *      1. This function adds the node to the AttributeEnum
 *         list with the values passed in the input parameters
 *	
 *  ARGUMENTS:
 *       - pHeadRef, pointer to ListAttributeEnum {IN/OUT}
 *       - AttributeEnum {IN}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


int addListAttributeEnum(ListAttributeEnum **pHeadRef, 
						 char* attributeEnum)
{
	unsigned int lenAttributeEnum = 0;

	ListAttributeEnum* pListAttributeEnum = 
	(ListAttributeEnum *) calloc(1, sizeof(ListAttributeEnum));
	
	if (pListAttributeEnum == NULL) {
		return (IPDR_MEMORY_ALLOCATION_FAILED);
	}
	
	lenAttributeEnum = strlen(attributeEnum) + 1;
	pListAttributeEnum->pAttributeEnum_ = (char *) 
					calloc (lenAttributeEnum, sizeof(char));

	if (pListAttributeEnum->pAttributeEnum_ == NULL) {
		return (IPDR_MEMORY_ALLOCATION_FAILED);
	}

	strcpy(pListAttributeEnum->pAttributeEnum_, attributeEnum);
	pListAttributeEnum->pNext_ = *pHeadRef;
	*pHeadRef = pListAttributeEnum;

	return (IPDR_OK);
}


/*!
 *  NAME:
 *      appendListAttributeEnum() - appends node to the 
 *           AttributeEnum list.
 * 
 *  DESCRIPTION:
 *      1. This function appends the node to the AttributeEnum
 *         list with the values passed in the input parameters
 *	
 *  ARGUMENTS:
 *       - pHeadRef, pointer to ListAttributeEnum {IN/OUT}
 *       - AttributeEnum {IN}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


int appendListAttributeEnum(ListAttributeEnum **pHeadRef,
							char* attributeEnum
						   )
{
	ListAttributeEnum* pCurrent = *pHeadRef;
	if (pHeadRef == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}
 
	if (attributeEnum == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}
 
	if (pCurrent == NULL) {
		addListAttributeEnum(pHeadRef, attributeEnum);
	} else {
		while (pCurrent->pNext_ != NULL) {
			pCurrent = pCurrent->pNext_;
		}
        addListAttributeEnum(&(pCurrent->pNext_), attributeEnum);
	}
	
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      getLengthListAttributeEnum() - calculates length of  
 *          the AttributeEnum list.
 * 
 *  DESCRIPTION:
 *      1. This function calculates length of the AttributeEnum
 *         list
 *	
 *  ARGUMENTS:
 *       - pListAttributeEnum {IN/OUT}
 *
 *  RETURNS 	
 *	     Returns length if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int getLengthListAttributeEnum(ListAttributeEnum * pListAttributeEnum)
{
	int count = 0;
	ListAttributeEnum* pCurrent = pListAttributeEnum;

	if (pListAttributeEnum == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	for (pCurrent = pListAttributeEnum; pCurrent != NULL; 
	pCurrent = pCurrent->pNext_)
		count++;
	return (count);
}


/*!
 *  NAME:
 *      freeListAttributeEnum() - Memory deallocation of 
 *      ListAttributeEnum structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory deallocation of 
 *         ListAttributeEnum structure & its members.
 *	
 *  ARGUMENTS:
 *       -	ListAttributeEnum {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int freeListAttributeEnum(ListAttributeEnum **pHeadRef)
{
	ListAttributeEnum* pCurrent;
	ListAttributeEnum* pNext_;

	if (pHeadRef == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}
 
	pCurrent = *pHeadRef;
        while (pCurrent != NULL) {
            pNext_ = pCurrent->pNext_;
	    free(pCurrent->pAttributeEnum_);
	    free(pCurrent);
	    pCurrent = pNext_;
    }
    free(pCurrent);
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      printListAttributeEnum() - prints 
 *         ListAttributeEnum structure
 *
 *  DESCRIPTION:
 *      1. This function prints the 
 *         ListAttributeEnum structure & its members.
 *	
 *  ARGUMENTS:
 *       -	pListAttributeEnum {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int printListAttributeEnum(
				ListAttributeEnum* pListAttributeEnum)
{
	ListAttributeEnum* pCurrent = pListAttributeEnum;
	int count = 0;
	if (pCurrent == NULL)
		return(IPDR_NULL_LINKLIST);
	
	while (pCurrent != NULL) {

		printf("\n\nDisplaying Record %d", count + 1);
		printf("\nAttribute Enumeration\t= %s", 
			pCurrent->pAttributeEnum_);		

	    pCurrent = pCurrent->pNext_;	
	    count++;
	}
	
	return (IPDR_OK);
}


/*!
 *  NAME:
 *      addListSchemaElement() - adds node to the 
 *           SchemaElement list.
 * 
 *  DESCRIPTION:
 *      1. This function adds the node to the SchemaElement
 *         list with the values passed in the input parameters
 *	
 *  ARGUMENTS:
 *       - pHeadRef, pointer to ListAttributeEnum {IN/OUT}
 *       - SchemaElement {IN}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


int addListSchemaElement(ListSchemaElement **pHeadRef, 
						 SchemaElement* pSchemaElement)
{

	ListSchemaElement* pListSchemaElement = 
	(ListSchemaElement *) calloc(1, sizeof(ListSchemaElement));
	
	if (pListSchemaElement == NULL) {
		return (IPDR_MEMORY_ALLOCATION_FAILED);
	}
	
	pListSchemaElement->pSchemaElement_ = newSchemaElement(); 
					
	if (pListSchemaElement->pSchemaElement_ == NULL) {
		return (IPDR_MEMORY_ALLOCATION_FAILED);
	}
	
	initSchemaElement(pListSchemaElement->pSchemaElement_,
					  pSchemaElement->attributeName_,
					  pSchemaElement->attributeType_,
					  pSchemaElement->documentation_,
					  pSchemaElement->derivedType_,
					  pSchemaElement->unit_,
					  pSchemaElement->reference_,
					  pSchemaElement->status_,
					  pSchemaElement->pListAttributeEnum_,
					  pSchemaElement->pAttributeOptional_
   				  );

	pListSchemaElement->pNext_ = *pHeadRef;
	*pHeadRef = pListSchemaElement;

	return (IPDR_OK);
}


/*!
 *  NAME:
 *      appendListSchemaElement() - appends node to the 
 *           SchemaElement list.
 * 
 *  DESCRIPTION:
 *      1. This function appends the node to the SchemaElement
 *         list with the values passed in the input parameters
 *	
 *  ARGUMENTS:
 *       - pHeadRef, pointer to ListSchemaElement {IN/OUT}
 *       - SchemaElement {IN}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


int appendListSchemaElement(ListSchemaElement **pHeadRef,
							SchemaElement* pSchemaElement
						   )
{
	ListSchemaElement* pCurrent = *pHeadRef;
	if (pHeadRef == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}
 
	if (pSchemaElement == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}
 
	if (pCurrent == NULL) {
		addListSchemaElement(pHeadRef, pSchemaElement);
	} else {
		while (pCurrent->pNext_ != NULL) {
			pCurrent = pCurrent->pNext_;
		}
        addListSchemaElement(&(pCurrent->pNext_), pSchemaElement);
	}
	
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      getLengthListSchemaElement() - calculates length of  
 *          the SchemaElement list.
 * 
 *  DESCRIPTION:
 *      1. This function calculates length of the SchemaElement
 *         list
 *	
 *  ARGUMENTS:
 *       - pListSchemaElement {IN/OUT}
 *
 *  RETURNS 	
 *	     Returns length if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int getLengthListSchemaElement(ListSchemaElement * pListSchemaElement)
{
	int count = 0;
	ListSchemaElement* pCurrent = pListSchemaElement;

	if (pListSchemaElement == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	for (pCurrent = pListSchemaElement; pCurrent != NULL; 
	pCurrent = pCurrent->pNext_)
		count++;
	return (count);
}


/*!
 *  NAME:
 *      freeListSchemaElement() - Memory deallocation of 
 *      ListSchemaElement structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory deallocation of 
 *         ListSchemaElement structure & its members.
 *	
 *  ARGUMENTS:
 *       -	ListSchemaElement {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int freeListSchemaElement(ListSchemaElement **pHeadRef)
{
	ListSchemaElement* pCurrent;
	ListSchemaElement* pNext_;

	if (pHeadRef == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}
 
	pCurrent = *pHeadRef;
        while (pCurrent != NULL) {
            pNext_ = pCurrent->pNext_;
	    freeSchemaElement(pCurrent->pSchemaElement_);
	    free(pCurrent);
	    pCurrent = pNext_;
    }
    free(pCurrent);
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      printListSchemaElement() - prints 
 *         ListSchemaElement structure
 *
 *  DESCRIPTION:
 *      1. This function prints the 
 *         ListSchemaElement structure & its members.
 *	
 *  ARGUMENTS:
 *       -	pListSchemaElement {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int printListSchemaElement(
				ListSchemaElement* pListSchemaElement)
{
	ListSchemaElement* pCurrent = pListSchemaElement;
	int count = 0;
	if (pCurrent == NULL) 
		return(IPDR_NULL_LINKLIST);
	
	printf("\n");
	while (pCurrent != NULL) {
		printf("\n\n\nPrinting Schema Element %d", count + 1);
		printSchemaElement(pCurrent->pSchemaElement_); 
	    pCurrent = pCurrent->pNext_;	
	    count++;
	}
	
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      newSchemaLookup() - Memory allocation of SchemaLookup structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory allocation of 
 *         SchemaLookup structure & its members.
 *	
 *  ARGUMENTS:
 *       -	void 
 *
 *  RETURNS 	
 *	     Returns Pointer to SchemaLookup structure
 */
SchemaLookup* newSchemaLookup(void)
{
	SchemaLookup* pSchemaLookup;
	pSchemaLookup = (SchemaLookup *) calloc(1, sizeof(SchemaLookup));
	pSchemaLookup->serviceType_ = NULL;
	pSchemaLookup->pListSchemaElement_ = NULL;

	return (pSchemaLookup);	
}


/*!
 *  NAME:
 *      freeSchemaLookup() - Memory deallocation of 
 *      SchemaLookup structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory deallocation of 
 *         SchemaLookup structure & its members.
 *	
 *  ARGUMENTS:
 *       -	SchemaLookup {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int freeSchemaLookup(SchemaLookup* pSchemaLookup)
{
	if (pSchemaLookup == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}
	
	free(pSchemaLookup->serviceType_);
	freeListSchemaElement(&(pSchemaLookup->pListSchemaElement_));	
	free(pSchemaLookup);
	
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      printSchemaLookup() - prints 
 *         SchemaLookup structure
 *
 *  DESCRIPTION:
 *      1. This function prints the 
 *         SchemaLookup structure & its members.
 *	
 *  ARGUMENTS:
 *       -	SchemaLookup {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int printSchemaLookup(SchemaLookup* pSchemaLookup)
{
	if (pSchemaLookup == NULL) {
        return (IPDR_NULL_INPUT_PARAMETER);
	}
	
	printf("\npSchemaLookup->serviceType_ = %s", 
							pSchemaLookup->serviceType_);
	printListSchemaElement(pSchemaLookup->pListSchemaElement_); 

	return (IPDR_OK);

}

/*!
 *  NAME:
 *      addListSchemaLookup() - adds node to the 
 *           SchemaLookup list.
 * 
 *  DESCRIPTION:
 *      1. This function adds the node to the SchemaLookup
 *         list with the values passed in the input parameters
 *	
 *  ARGUMENTS:
 *       - pHeadRef, pointer to ListSchemaLookup {IN/OUT}
 *       - SchemaLookup {IN}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


int addListSchemaLookup(ListSchemaLookup **pHeadRef, 
						 SchemaLookup* pSchemaLookup)
{
	ListSchemaElement* pCurrentListSchemaElement = NULL;
	int lenServType = 0;

	ListSchemaLookup* pListSchemaLookup = 
	(ListSchemaLookup *) calloc(1, sizeof(ListSchemaLookup));
	
	if (pListSchemaLookup == NULL) {
		return (IPDR_MEMORY_ALLOCATION_FAILED);
	}
	
	pListSchemaLookup->pSchemaLookup_ = newSchemaLookup(); 
					
	if (pListSchemaLookup->pSchemaLookup_ == NULL) {
		return (IPDR_MEMORY_ALLOCATION_FAILED);
	}
	
	lenServType = strlen(pSchemaLookup->serviceType_) + 1;
	pListSchemaLookup->pSchemaLookup_->serviceType_ = 
		(char *) calloc (lenServType, sizeof(char));
	strcpy(pListSchemaLookup->pSchemaLookup_->serviceType_, pSchemaLookup->serviceType_); 
	
	pCurrentListSchemaElement = pSchemaLookup->pListSchemaElement_;
	while(pSchemaLookup->pListSchemaElement_ != NULL) {
		appendListSchemaElement(&pListSchemaLookup->pSchemaLookup_->pListSchemaElement_,
							 pSchemaLookup->pListSchemaElement_->pSchemaElement_);  
		pSchemaLookup->pListSchemaElement_ = pSchemaLookup->pListSchemaElement_->pNext_; 
	}
	pSchemaLookup->pListSchemaElement_ = pCurrentListSchemaElement;

	pListSchemaLookup->pNext_ = *pHeadRef;
	*pHeadRef = pListSchemaLookup;

	return (IPDR_OK);
}


/*!
 *  NAME:
 *      appendListSchemaLookup() - appends node to the 
 *           ListSchemaLookup.
 * 
 *  DESCRIPTION:
 *      1. This function appends the node to the ListSchemaLookup
 *         with the values passed in the input parameters
 *	
 *  ARGUMENTS:
 *       - pHeadRef, pointer to ListSchemaLookup {IN/OUT}
 *       - SchemaLookup {IN}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


int appendListSchemaLookup(ListSchemaLookup **pHeadRef,
							SchemaLookup* pSchemaLookup
						   )
{
	ListSchemaLookup* pCurrent = *pHeadRef;
	if (pHeadRef == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}
 
	if (pSchemaLookup == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}
 
	if (pCurrent == NULL) {
		addListSchemaLookup(pHeadRef, pSchemaLookup);
	} else {
		while (pCurrent->pNext_ != NULL) {
			pCurrent = pCurrent->pNext_;
		}
        addListSchemaLookup(&(pCurrent->pNext_), pSchemaLookup);
	}
	
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      getLengthListSchemaLookup() - calculates length of  
 *          the SchemaElement list.
 * 
 *  DESCRIPTION:
 *      1. This function calculates length of the SchemaElement
 *         list
 *	
 *  ARGUMENTS:
 *       - pListSchemaElement {IN/OUT}
 *
 *  RETURNS 	
 *	     Returns length if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int getLengthListSchemaLookup(ListSchemaLookup* pListSchemaLookup)
{
	int count = 0;
	ListSchemaLookup* pCurrent = pListSchemaLookup;

	if (pListSchemaLookup == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	for (pCurrent = pListSchemaLookup; pCurrent != NULL; 
	pCurrent = pCurrent->pNext_)
		count++;
	return (count);
}


/*!
 *  NAME:
 *      freeListSchemaLookup() - Memory deallocation of 
 *      ListSchemaLookup structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory deallocation of 
 *         ListSchemaLookup structure & its members.
 *	
 *  ARGUMENTS:
 *       -	ListSchemaLookup {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int freeListSchemaLookup(ListSchemaLookup **pHeadRef)
{
	ListSchemaLookup* pCurrent;
	ListSchemaLookup* pNext_;

	if (pHeadRef == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}
 
	pCurrent = *pHeadRef;
        while (pCurrent != NULL) {
            pNext_ = pCurrent->pNext_;
	    freeSchemaLookup(pCurrent->pSchemaLookup_);
	    free(pCurrent);
	    pCurrent = pNext_;
    }
    free(pCurrent);
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      printListSchemaLookup() - prints 
 *         ListSchemaLookup structure
 *
 *  DESCRIPTION:
 *      1. This function prints the 
 *         ListSchemaLookup structure & its members.
 *	
 *  ARGUMENTS:
 *       -	ListSchemaLookup {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int printListSchemaLookup(
				ListSchemaLookup* pListSchemaLookup)
{
	ListSchemaLookup* pCurrent = pListSchemaLookup;
	int count = 0;
	if (pCurrent == NULL) 
		return(IPDR_NULL_LINKLIST);
	
	while (pCurrent != NULL) {
		
		printSchemaLookup(pCurrent->pSchemaLookup_); 
	    pCurrent = pCurrent->pNext_;	
	    count++;
	}
	
	return (IPDR_OK);
}

//IPDR v3.5, structures and arrays
int appendListComplexElement(ListComplexElement **pHeadRef,
							ComplexElement* pComplexElement
						   )
{
	ListComplexElement* pCurrent = *pHeadRef;
	if (pHeadRef == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}
 
	if (pComplexElement == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}
 
	if (pCurrent == NULL) {
		addListComplexElement(pHeadRef, pComplexElement);
	} else {
		while (pCurrent->pNext_ != NULL) {
			pCurrent = pCurrent->pNext_;
		}
        addListComplexElement(&(pCurrent->pNext_), pComplexElement);
	}
	
	return (IPDR_OK);
}

int addListComplexElement(ListComplexElement **pHeadRef, 
						 ComplexElement* pComplexElement)
{

	ListComplexElement* pListComplexElement = 
	(ListComplexElement *) calloc(1, sizeof(ListComplexElement));
	
	if (pListComplexElement == NULL) {
		return (IPDR_MEMORY_ALLOCATION_FAILED);
	}
	
	pListComplexElement->pComplexElement_ = newComplexElement(); 
					
	if (pListComplexElement->pComplexElement_ == NULL) {
		return (IPDR_MEMORY_ALLOCATION_FAILED);
   }

	initComplexElement(pListComplexElement->pComplexElement_,
					  pComplexElement->attributeName_,
					  pComplexElement->attributeType_,
					  pComplexElement->documentation_,
					  pComplexElement->derivedType_,
					  pComplexElement->unit_,
					  pComplexElement->reference_,
					  pComplexElement->status_,
					  pComplexElement->pListAttributeEnum_,
					  pComplexElement->pAttributeOptional_);

	pListComplexElement->pNext_ = *pHeadRef;
	*pHeadRef = pListComplexElement;

	return (IPDR_OK);
}

int initComplexElement(ComplexElement* pComplexElement, 
					  char* attributeName,
 					  char* attributeType,
 			          char* documentation,
 			          char* derivedType,
			          char* unit,
			          char* reference,
			          char* status,
			          List* pListAttributeEnum_,
			          AttributeOptional pAttributeOptional_
					)
{
	unsigned int lenAttributeName = 0;
	unsigned int lenAttributeType = 0;
	unsigned int lenDerivedType = 0;
	unsigned int lenDocumentation = 0;
	unsigned int lenUnit = 0;
	unsigned int lenRefernce = 0;
	unsigned int lenStatus = 0;
	unsigned int lenNULL = 0;
	List* pCurrentListAttributeEnum = NULL; 

	lenNULL = strlen(IPDR_NULL) + 1;

	if (pComplexElement == NULL)
		return (IPDR_NULL_INPUT_PARAMETER);

	if (attributeName == NULL) {
		pComplexElement->attributeName_ = (char *) calloc (lenNULL, sizeof(char));
		strcpy(pComplexElement->attributeName_, IPDR_NULL);
	} else {
		lenAttributeName = strlen(attributeName) + 1;
		pComplexElement->attributeName_ = (char *) calloc (lenAttributeName, sizeof(char));
		strcpy(pComplexElement->attributeName_, attributeName);
	}

	if (attributeType == NULL) {
		pComplexElement->attributeType_ = (char *) calloc (lenNULL, sizeof(char));
		strcpy(pComplexElement->attributeType_, IPDR_NULL);
	} else {
		lenAttributeType = strlen(attributeType) + 1;
		pComplexElement->attributeType_ = (char *) calloc (lenAttributeType, sizeof(char));
		strcpy(pComplexElement->attributeType_, attributeType);
	}

	if (derivedType == NULL) {
		pComplexElement->derivedType_ = (char *) calloc (lenNULL, sizeof(char));
		strcpy(pComplexElement->derivedType_, IPDR_NULL);
	} else {
		lenDerivedType = strlen(derivedType) + 1;
		pComplexElement->derivedType_ = (char *) calloc (lenDerivedType, sizeof(char));
		strcpy(pComplexElement->derivedType_, derivedType);
	}
	
	if (documentation == NULL) {
		pComplexElement->documentation_ = (char *) calloc (lenNULL, sizeof(char));
		strcpy(pComplexElement->documentation_, IPDR_NULL);
	} else {
		lenDocumentation  = strlen(documentation) + 1;
		pComplexElement->documentation_ = (char *) calloc (lenDocumentation, sizeof(char));
		strcpy(pComplexElement->documentation_, documentation);
	}
	
	if (reference == NULL) {
		pComplexElement->reference_ = (char *) calloc (lenNULL, sizeof(char));
		strcpy(pComplexElement->reference_, IPDR_NULL);
	} else {
		lenRefernce = strlen(reference) + 1;
		pComplexElement->reference_ = (char *) calloc (lenRefernce, sizeof(char));
		strcpy(pComplexElement->reference_, reference);
	}
	
	
	if (status == NULL) {
		pComplexElement->status_ = (char *) calloc (lenNULL, sizeof(char));
		strcpy(pComplexElement->status_, IPDR_NULL);
	} else {
		lenStatus = strlen(status) + 1;
		pComplexElement->status_ = (char *) calloc (lenStatus, sizeof(char));
		strcpy(pComplexElement->status_, status);
	}
	
	if (unit == NULL) {
		pComplexElement->unit_ = (char *) calloc (lenNULL, sizeof(char));
		strcpy(pComplexElement->unit_, IPDR_NULL);
	} else {
		lenUnit = strlen(unit) + 1;
		pComplexElement->unit_ = (char *) calloc (lenUnit, sizeof(char));
		strcpy(pComplexElement->unit_, unit);
	}
	
	pCurrentListAttributeEnum = pListAttributeEnum_;
	while(pListAttributeEnum_ != NULL) {
		appendNode (&(pComplexElement->pListAttributeEnum_),
							 pListAttributeEnum_->node);
		pListAttributeEnum_ = pListAttributeEnum_->pNext;
	}
	pListAttributeEnum_ = pCurrentListAttributeEnum;
	
	pComplexElement->pAttributeOptional_ = pAttributeOptional_; 

	return (IPDR_OK);
}

ComplexElement* newComplexElement(void)
{
	ComplexElement* pComplexElement;
	pComplexElement = (ComplexElement *) calloc(1, sizeof(ComplexElement));
	pComplexElement->attributeName_ = NULL;
	pComplexElement->attributeType_ = NULL; 
	pComplexElement->derivedType_ = NULL; 
	pComplexElement->documentation_ = NULL; 
	pComplexElement->reference_ = NULL; 
	pComplexElement->status_ = NULL; 
	pComplexElement->unit_ = NULL; 
	pComplexElement->pListAttributeEnum_ = NULL;
	pComplexElement->pAttributeOptional_ = (AttributeOptional)IPDR_OPTIONAL;  
	//pComplexElement->pAttributeOptional_ = 1;  

	return (pComplexElement);	
}
