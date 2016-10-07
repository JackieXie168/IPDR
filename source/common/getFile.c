/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/


/**********************************************************
 * File                 : getFile.c					      *
 * Description          : This file contains the function *	
 *                        to get .xsd file from specified *
 *                        URI using FTP or HTTP protocol. *
 * Author               : Infosys Tech Ltd                *
 * Modification History :                                 *
 *------------------------------------------------------- *
 * Date       Name        Change/Description              *
 *------------------------------------------------------- *
 * 26Jan02    Infosys       Draft Version                 *
 **********************************************************/


#include <stdio.h>
#include <sys/malloc.h>
#include <stdlib.h>
#include <string.h>
#include "curl/curl.h"
#include "curl/types.h"
#include "curl/easy.h"
#include "common/getFile.h"
#include "utils/errorCode.h"
#include "utils/IPDRCommon.h"

/*!
 *  NAME:my_fwrite()- This function is called as any start tag  
 *                    for an element is encountered..
 *
 *  DESCRIPTION:
 *      1. This function writes data into the local file 
 *         from remote file.
 *	
 *  ARGUMENTS:
 *       -	buffer {IN}
 *       -  size {IN}
 *       -  nmemb {IN} 
 *       -  stream {IN} 
 *
 *  RETURNS 	
 *	     Returns int
 * 		 
 */

int my_fwrite(void *buffer, size_t size, size_t nmemb, void *stream)
{
  struct FtpFile *out=(struct FtpFile *)stream;
  if(out && !out->stream) 
  {
    /* open file for writing */
    out->stream=fopen(out->filename, "wb");
    if(!out->stream)
      return -1; /* failure, can't open file to write */
  }
  return fwrite(buffer, size, nmemb, out->stream);
}

/*!
 *  NAME:
 *      getFile() - This function is called as any start tag  
 *                           for an element is encountered..
 *
 *  DESCRIPTION:
 *      1. This function gets the file from specified URI using
 *         FTP or HTTP protocol and stores them as given 
 *         LocalFileName
 *	
 *  ARGUMENTS:
 *       -	RemoteURI {IN}
 *       -  LocalFileName {IN}
 *       -  configParams {IN} 
 *
 *  RETURNS 	
 *	     Returns int
 * 		 
 */

int getFile(char* RemoteURI, 
			char* LocalFileName, 
			char* configParams,
			int* pErrorCode)
{
  CURL *curl;
  int res;

  
 char* URI = RemoteURI;
 char* FileName = LocalFileName;
 char* Params = configParams;
 
struct FtpFile ftpfile;
ftpfile.filename = FileName;
ftpfile.stream = NULL;
 
 strcpy(Params,"");

  if (RemoteURI == NULL) {
	  *pErrorCode = IPDR_NULL_INPUT_PARAMETER;
	  return (IPDR_WARNING);
  }

  if (LocalFileName == NULL) {
	  *pErrorCode = IPDR_CURLE_LOCAL_FILENAME_NULL;
	  return (IPDR_WARNING);
  }

  if (LocalFileName[0] == 0) {
	  *pErrorCode = IPDR_CURLE_LOCAL_FILENAME_BLANK;
	  return (IPDR_ERROR);
  }

  curl_global_init(CURL_GLOBAL_DEFAULT);
  
  curl = curl_easy_init();
  if(curl) 
  {
    /* Get curl 7.9.2 from specified FTP/HTTP site: */
    curl_easy_setopt(curl, CURLOPT_URL,URI);
    /* Define our callback to get called when there's data to be written */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_fwrite);
    /* Set a pointer to our struct to pass to the callback */
    curl_easy_setopt(curl, CURLOPT_FILE, &ftpfile);

    /* Switch on full protocol/debug output */
	 #ifndef TRUE
		#define TRUE 1
	 #endif
    curl_easy_setopt(curl, CURLOPT_VERBOSE, TRUE);


    res = curl_easy_perform(curl);

    
    /* always cleanup */
    curl_easy_cleanup(curl);

    if(CURLE_OK != res) 
	{
      /* we failed */
      fprintf(stderr, "curl told us %d\n", res);
    }
  }

  if(ftpfile.stream)
    fclose(ftpfile.stream); /* close the local file */

  curl_global_cleanup();

	
  /* Error Handling */
	switch (res)
	{
	case CURLE_OK:
		return (IPDR_OK);
	case CURLE_COULDNT_CONNECT:
		*pErrorCode = IPDR_CURLE_COULDNT_CONNECT;
		break;
	case CURLE_FTP_COULDNT_RETR_FILE:
		*pErrorCode = IPDR_CURLE_FTP_COULDNT_RETR_FILE;
		break;
	case CURLE_FTP_ACCESS_DENIED:
		*pErrorCode = IPDR_CURLE_FTP_ACCESS_DENIED;
		break;
	case CURLE_WRITE_ERROR:
		*pErrorCode = IPDR_CURLE_WRITE_ERROR;
		break;
	default:
		*pErrorCode = IPDR_CURLE_GENERAL_ERROR;
		return (IPDR_ERROR);
		break;
	}

  return (IPDR_WARNING);
}
