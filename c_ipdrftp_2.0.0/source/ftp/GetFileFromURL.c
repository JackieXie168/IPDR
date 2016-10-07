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
 * File                 : get_File.c					      
 * Description          : This file contains the function 	
 *                        to get .xsd file from specified 
 *                        URI using FTP or HTTP protocol. 
 * Author               : Infosys Tech Ltd                
 * Modification History :                                 
 *------------------------------------------------------- 
 * Date       Name        Change/Description              
 *------------------------------------------------------- 
 * 26Jan02    Infosys       Draft Version                 
 *
 *
 *
 */


#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "ftp/GetFileFromURL.H"
#include "ftp/ErrorCodes.H"

/*!
 *  NAME:my_fwrite()- This function is called as any start tag  
 *                    for an element is encountered..
 *
 *  DESCRIPTION:1.This function writes data into the local file 
 *                from remote file.
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

int my_fwrite_(void *buffer, size_t size, size_t nmemb, void *stream)
{
  struct FileFtp *out=(struct FileFtp *)stream;

  if(out && !out->stream) 
  {
    /* open file for writing */
    out->stream=fopen(out->filename, "w");
    if(!out->stream)
      return -1; /* failure, can't open file to write */
  }

  return fwrite(buffer, size, nmemb, out->stream);
}

/*!
 *  NAME:
 *      get_File() - This function gets file from URL.
 *
 *  DESCRIPTION:1.This function gets the file from specified URI using
 *                FTP or HTTP protocol and stores them as given 
 *                LocalFileName
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
int get_File(char* RemoteURI, 
			char* LocalFileName, 
			char* configParams,
			int* pErrorCode)
{
	CURL *curl;
	CURLcode res;

	char* URI = RemoteURI;
	char* FileName = LocalFileName;
	char* Params = configParams;
    
    struct FileFtp ftpfile;
    ftpfile.filename = FileName;/* name to store the file as if succesful */
    ftpfile.stream = NULL;

#ifdef DEBUG_IPDRFTP
	printf("Entering get_File()\n");
#endif
	/*Error Handling  */
	if (RemoteURI == NULL || strlen(RemoteURI) == 0) {
	  *pErrorCode = IPDR_CURL_NULL_URI;
	  return (1);
	}
	/*validate URL*/
	if((*pErrorCode = validateURL(RemoteURI))!=0)
		return 1;

	if (LocalFileName == NULL) {
	  *pErrorCode = IPDR_CURL_LOCAL_FILENAME_NULL;
	  return (1);
	}
	if (LocalFileName[0] == 0) {
	  *pErrorCode = IPDR_CURL_LOCAL_FILENAME_BLANK;
	  return (1);
	}

	curl_global_init(CURL_GLOBAL_DEFAULT);
  
	curl = curl_easy_init();

	if(curl){ 
		/*Get curl 7.9.2 from specified FTP/HTTP site: */
		curl_easy_setopt(curl, CURLOPT_URL,URI);

		/*Define our callback to get called when there's data to be written */
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_fwrite_);

		/*Set a pointer to our struct to pass to the callback */
		curl_easy_setopt(curl, CURLOPT_FILE, &ftpfile);

		/*Switch on full protocol/debug output */
		curl_easy_setopt(curl, CURLOPT_VERBOSE, TRUE);

		res = curl_easy_perform(curl);
    
		/*always cleanup */
		curl_easy_cleanup(curl);

		if(CURLE_OK != res && res != CURLE_PARTIAL_FILE){/*curl failed*/
			fprintf(stderr, "curl told us %d\n", res);
		}

	}

	if(ftpfile.stream)
		fclose(ftpfile.stream); /*close the local file */

	curl_global_cleanup();

#ifdef DEBUG_IPDRFTP
	printf("Leaving get_File()\n");
#endif

  /* Error Handling */
	switch (res){
	case CURLE_OK:
		return (0);
	case CURLE_PARTIAL_FILE:
		return (0);
	case CURLE_COULDNT_CONNECT:
		*pErrorCode = IPDR_CURL_COULDNT_CONNECT;
		break;
	case CURLE_FTP_COULDNT_RETR_FILE:
		*pErrorCode = IPDR_CURL_FTP_COULDNT_RETR_FILE;
		break;
	case CURLE_FTP_ACCESS_DENIED:
		*pErrorCode = IPDR_CURL_FTP_ACCESS_DENIED;
		break;
	case CURLE_WRITE_ERROR:
		*pErrorCode = IPDR_CURL_WRITE_ERROR;
		break;
	default:
		*pErrorCode = IPDR_CURL_GENERAL_ERROR;
		break;
	}
	return 1;
}

