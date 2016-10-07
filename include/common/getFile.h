/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/


#ifndef __CURL_GETFILE_H
#define __CURL_GETFILE_H

#include "curl/easy.h"
#include "curl/curl.h"
#include "curl/types.h"

struct FtpFile {
  char *filename;
  FILE *stream;
};

int my_fwrite(void *buffer, size_t size, size_t nmemb, void *stream);

int getFile(char* RemoteURI, 
			char* LocalFileName, 
			char* configParams,
			int* pErrorCode);

#endif
