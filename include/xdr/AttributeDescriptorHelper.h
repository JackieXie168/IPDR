/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/



/*****************************************************
 * File                 : AttributeDescriptorHelper  *
 * Description          :                            *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 *                                                   *
 *****************************************************/

#ifndef _ATTRIBUTEDESCRIPTORHELPER_H
#define _ATTRIBUTEDESCRIPTORHELPER_H

#include "utils/IPDRCommon.h"
#include "utils/errorCode.h"
#include "xdr/XDRMarshaller.h"
#include "xdr/XDRUnMarshaller.h"
#include "utils/utils.h"
#include "utils/errorHandler.h"
#include "utils/IPDRMemory.h"

/*!
 * This function writes the XDR equivalent on output stream.
 */
int writeAttributeDescriptorHelper(
							IPDRCommonParameters* pIPDRCommonParameters, 
							AttributeDescriptor* pAttributeDescriptor,
							ListAttributeDescriptor* pListServiceAttributeDescriptor, 
							int* pErrorCode
							);

/*!
 * This function returns AttributeDescriptor structure
 */
int readAttributeDescriptorHelper( 	IPDRCommonParameters *pIPDRCommonParameters, 
												AttributeDescriptor *pAttributeDescriptor, 
												ListAttributeDescriptor**	pLisAttributeDescriptor, 
												ListAttributeDescriptor**	pListServiceAttributeDescriptor,
												int*  attributeType,
												int* pErrorCode
											);

/*!
 * This function returns complex AttributeDescriptor structure
 */
int readCompositeDescriptor(char* attrName,
									 int arrayflag, 
	  								 ListAttributeDescriptor** pLisAttributeDescriptor, 
									 ListAttributeDescriptor** pListServiceAttributeDescriptor,
								  	 IPDRCommonParameters* IPDRCommonParameters,
								  	 int* pErrorCode
									);

/*!
 * This function writes the complex XDR equivalent on output stream.
 */
int writeComplexAttributeDescriptorHelper(
							IPDRCommonParameters* pIPDRCommonParameters, 
							AttributeDescriptor* pAttributeDescriptor,
							ListAttributeDescriptor* pListServiceAttributeDesriptor,
							int* pErrorCode
							);

#endif


