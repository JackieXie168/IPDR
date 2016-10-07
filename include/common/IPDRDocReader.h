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

#ifndef _IPDRDOCREADER_H
#define _IPDRDOCREADER_H

#include "utils/IPDRCommon.h"
#include "utils/errorCode.h"
#include "utils/utils.h"
#include "utils/errorHandler.h"
#include "utils/IPDRMemory.h"
#include "common/schema.h"
#include "utils/schemaUtil.h"

#include "xdr/IPDRXDRHeaderHelper.h"
#include "xdr/IPDRStreamElementHelper.h"
#include "xdr/IPDRXDRRecordHelper.h"
#include "xdr/IPDRXDRDocEndHelper.h"
#include "xdr/IPDRRecord.h"

#include "xml/IPDRXMLHeaderHelper.h"
#include "xml/IPDRXMLRecordHelper.h"
#include "xml/IPDRXMLDocEndHelper.h"


int readHeader(
			   IPDRCommonParameters *IPDRCommonParameters, 
			   IPDRHeader *IPDRHeader, 
			   int* errorCode
			  );

int readIPDR(
			 IPDRCommonParameters *IPDRCommonParameters, 
			 FNFData** FNFData, 
			 int* errorCode
			 );

int readDocEnd(
			   IPDRCommonParameters *IPDRCommonParameters, 
			   DocEnd *DocEnd, 
			   int* errorCode
			  );

#endif
