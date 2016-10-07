/**
 * Copyright (C) 2002, The IPDR Organization, all rights reserved.
 * The use and distribution of this software is governed by the terms of
 * the license agreement which can be found in the file LICENSE.TXT at
 * the top of this source tree.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND, either express or implied.
 *
 *
 * File                 : IPDRCommon.h                
 * Description          : Contains All CapabilityFile 
 *                        function declarations       
 * Author               : Infosys Tech Ltd            
 * Modification History :                             
 *----------------------------------------------------
 * Date       Name        Change/Description          
 *----------------------------------------------------
 * 01/29/02   Infosys      Created                    
 *
 */

#ifndef _GETFILE_FROM_URL_H_
#define _GETFILE_FROM_URL_H_

#include "curl/curl.h"
#include "curl/types.h"
#include "curl/easy.h"
#include "ftp/FtpStructures.H"

struct FileFtp {
  char *filename;
  FILE *stream;
};

int my_fwrite_(void *buffer, size_t size, size_t nmemb, void *stream);

int get_File(char* RemoteURI, 
			char* LocalFileName, 
			char* configParams,
			int* pErrorCode);

/*Utility function*/
FILE* getFileStreamFromURL(ListErrorStruct* pErrorList,
						 char* remoteURL,
						 char* localPath);

#endif //_GETFILE_FROM_URL_H_
