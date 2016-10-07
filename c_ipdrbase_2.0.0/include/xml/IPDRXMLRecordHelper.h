/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/



/*****************************************************
 * File                 : IPDRXMLRecordHelper		 *
 * Description          :                            *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 * 24-Feb-02                                         *
 *****************************************************/

#ifndef _IPDRXMLRECORDHELPER_H
#define _IPDRXMLRECORDHELPER_H

#include "utils/IPDRCommon.h"
#include "utils/errorCode.h"
#include "utils/utils.h"
#include "utils/errorHandler.h"
#include "utils/IPDRMemory.h"
#include "xml/XMLMarshaller.h"
#include "xml/XMLUnMarshaller.h"
#include "utils/serialize.h"



/*! 	 
 * This function takes Record structure as input and writes
 * its XML equivalent on output stream.
 */	


int writeIPDRXMLRecordHelper(
						  IPDRCommonParameters* IPDRCommonParameters,
						  FNFData* FNFData,
						  int* pErrorCode
							);

 
/*! 	 
 * This function reads the FNF Record Structure 
 * from it's XML equivalent on output stream.
 */	

int readIPDRXMLRecordHelper(
						 IPDRCommonParameters* IPDRCommonParameters, 
						 FNFData** FNFData, 
						 int* pErrorCode
							);

/*! 	 
 * This function takes FNF Data as input and converts 
 * the derived types into primitive types depending upon the 
 * derived type data type.
 */	

int	convDerivedTypeToAttributeType(
								   FNFData *pFNFData, 
								   int* pErrorCode
								  );


int getAttributeTypeFromSchema(IPDRCommonParameters* pIPDRCommonParameters,
                                            FNFData* pFNFData,
                                            int* pErrorCode
                                            );

int convToEntityReference(char* inputString, char* outputString);

/*! 	 
 * This function takes Record structure as input and writes
 * its complex XML equivalent on output stream.
 */	
int writeIPDRXMLComplexRecordHelper(FNFData* pFNFData,
										  char* pComplexType,
										  IPDRCommonParameters* pIPDRCommonParameters,
										  int* pErrorCode
										 );

int isPrimitiveComplexType(char *tempNewAttrType);

int populateListAttributeDescriptor(
							 IPDRCommonParameters* pIPDRCommonParameters, 
							 FNFData** pFNFData, 
							 int* pErrorCode
							);

int populateAttributeDescriptor(
							 IPDRCommonParameters* pIPDRCommonParameters, 
							 FNFData** pFNFData, 
							 int* pErrorCode
							);

int populateComplexDataType(
							 FNFData** pFNFData,
							 IPDRCommonParameters* pIPDRCommonParameters, 
							 SchemaElement* pSchemaElement,
							 char* pComplexAttributeName,
							 int *pErrorCode 
);

int populateListIPDRData( 	IPDRCommonParameters* pIPDRCommonParameters, 
							 		FNFData** pFNFData, 
							 		int* pErrorCode
							   );

int populateComplexDataTypeWithSchema(char* pComplexType,
												  FNFData*		 pFNFData,
												  ListIPDRComplex** pListIPDRComplex,
												  int*	pErrorCode
												 );


int populateIPDRData( 	IPDRCommonParameters* pIPDRCommonParameters, 
							 	FNFData** pFNFData, 
							 	int* pErrorCode
						  );

int populateComplexType(
							 FNFData** pFNFData,
							 IPDRCommonParameters* pIPDRCommonParameters, 
							 ListAttributeDescriptor* pListAttributeDescriptor,
							 int *pErrorCode 
							);


/*! 	 
 * This function takes FNF Data as input and converts 
 * the derived types into primitive types depending upon the 
 * derived type data type for complex data.
 */	
int	convDerivedTypeToAttributeTypeFor35Version(
													FNFData *pFNFData, 
													int* pErrorCode
													);

int getComplexAttributeTypeFromSchema(
							   IPDRCommonParameters* pIPDRCommonParameters,
                               FNFData* pFNFData,
                               int* pErrorCode
                              );

int populateArrayDataTypeWithSchema( AttributeDescriptor* pAttributeDecriptor,
												FNFData*		 pFNFData,
												ListIPDRData** pListIPDRData,
												int*	pErrorCode);

#endif
