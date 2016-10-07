/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/


/*****************************************************
 * File                 : XMLUnMarshaller.h          *
 * Description          :                            *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 * 24-Feb-02                                         *
 *****************************************************/

#ifndef _XMLUNMARSHALLER_H
#define _XMLUNMARSHALLER_H

#include "utils/IPDRCommon.h"
#include "utils/errorCode.h"
#include "utils/utils.h"
#include "utils/errorHandler.h"
#include "utils/IPDRMemory.h"

#include "xml/IPDRXMLHeaderHelper.h"
#include "xml/IPDRXMLRecordHelper.h"
#include "xml/IPDRXMLDocEndHelper.h"


#define IPDR_XML_MAX_SCHEMA_LOCATION	400


#define IPDR_XML_SEQ_NUM                "seqNum"


/* All common variables shared across XMLUnMarshaller */

typedef struct XMLUnMarshallerCommon {
	char *Buff_;
	char *serviceType_;		
	char *schemaLocation_;
	char *token_;
	char *attributeName_;
	char *attributeValue_;
	int Depth_;
	int IPDRDocEnd_;
	int flagValue_;
	char *OldattributeName_;
	char *OldComplexattributeName_;
	int StructureFlagValue_;
	int ComplexFlagValue_;
	int ComplexFlagValue2_;
	int EndFlagValue_;
	IPDRHeader *pGIPDRHeader_;
   List* pAttrNameList_;
   List* pAttrNameList2_;
	ListAttributeDescriptor* pListAttributeDescriptor_;
	ListAttributeDescriptor* pListServiceAttributeDescriptor_;
	ListIPDRData* pListIPDRData_;
	ListFNFData* pListFNFData_;
	ListIPDRComplex* pListIPDRComplex_;
	DocEnd* pGDocEnd_;
	} XMLUnMarshallerCommon;

int readXMLStreamData(
			   IPDRCommonParameters *pIPDRCommonParameters,
			   IPDRHeader *pIPDRHeader,
			   int* pErrorCode
			  );


int isNameSpaceInfo(char *source);



int initXMLUnMarshallerCommon(
							  IPDRCommonParameters *pIPDRCommonParameters,
							  XMLUnMarshallerCommon* pXMLUnMarshallerCommon
							 );

int freeXMLUnMarshallerCommon(
							  XMLUnMarshallerCommon* pXMLUnMarshallerCommon
							  );

int getIDListNameSpaceInfo(
						   ListNameSpaceInfo* pListNameSpaceInfo,
						   char* nameSpaceURI,
						   char* nameSpaceID,
						   int* pErrorCode
						   );
int getXMLChar(char* tmpXMLChar);
#endif

