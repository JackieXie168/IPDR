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
 * File                 : GetFileStream.c			         
 * Description          : Contains a utility function for      
 *                        getting file stream from URI.
 * Author               : Infosys Tech Ltd           
 * Modification History :                            
 *---------------------------------------------------
 * Date       Name             Change/Description    
 *---------------------------------------------------
 * 03/01/02  Infosys           Created                        
 *
 * 
 */

#include "ftp/GetFileFromURL.H"
#include "ftp/ErrorCodes.H"

/*!
 *  NAME:
 *      getFileStreamFromURL() - This function returns file stream for URL.
 *
 *  DESCRIPTION:1.This function gets the file from specified URI using
 *                FTP or HTTP protocol and stores them as given 
 *                LocalFileName.It opens the file at localPath and returns file stream.
 *	
 *  ARGUMENTS:
 *       -	pErrorList {IN / OUT}
 *       -  remoteURL {IN}
 *       -  localPath {IN} 
 *
 *  RETURNS 	
 *	     Returns FILE* stream on success or NULL.
 * 		 
 */
FILE* getFileStreamFromURL(ListErrorStruct* pErrorList,
							char* remoteURL,
							char* localPath)
{
	int errorCode;
	FILE* stream;
#ifdef DEBUG_IPDRFTP
	printf("Entering getFileStreamFromURL()\n");
#endif
	if(!remoteURL || !localPath){/*Error handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getFileStreamFromURL");
		return NULL;
	}

	/*get control file from URL.*/
	if((get_File(remoteURL,localPath,
				"",	&errorCode))==1){/*error handling*/
		ftpErrorHandler(pErrorList,
					 errorCode,
					 "getFileStreamFromURL");
		return NULL;
	}
	//open file at localPath
	if((stream = fopen(localPath,"r"))==NULL){/*Error Handling*/
		ftpErrorHandler(pErrorList, 
					 IPDR_FOPEN_FAILED,
					 "getFileStreamFromURL");
	}
#ifdef DEBUG_IPDRFTP
	printf("Leaving getFileStreamFromURL()\n");
#endif
	return stream;
}
