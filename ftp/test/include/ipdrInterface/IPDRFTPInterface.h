/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/



/*****************************************************
 * File                 : IPDRFTPInterface.h		 *
 * Description          : Interface functions with   *
 *						  FTP module.				 *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 * 07/03/02   Infosys      Created                   *
 *****************************************************/

#ifndef _IPDR_INTERFACE_H_
#define _IPDR_INTERFACE_H_


#include "common/IPDRDocWriter.h"
#include "common/IPDRDocReader.h"
#include "utils/UUIDUtil.h"

#include "common/schema.h"
#include "common/descriptor.h"
#include "common/getFile.h"
#include "utils/errorHandler.h"
#include "utils/IPDRCommon.h"
#include "utils/errorCode.h"

int testWriteFTP(
		 int docType, 
		 char** testDataFiles,
		 char* outFile
		 );

int testReadFTP(
		 int docType, 
		 char* inputFile,
		 char* outFile
		 );

#endif
