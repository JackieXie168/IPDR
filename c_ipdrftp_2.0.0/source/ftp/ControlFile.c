/**
 *
 * Copyright (C) 2002, The IPDR Organization, all rights reserved.
 * The use and distribution of this software is governed by the terms of
 * the license agreement which can be found in the file LICENSE.TXT at
 * the top of this source tree.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND, either express or implied.
 *
 *
 * File                 : ControlFile.c			         
 * Description          : Contains all the ControlFile      
                          functions      
 * Author               : Infosys Tech Ltd           
 * Modification History :                            
 *---------------------------------------------------
 * Date       Name             Change/Description    
 *---------------------------------------------------
 * 01/29/02  Infosys           Created  
 *
 *
 */

#include <errno.h>

#include "ftp/ControlFile.H"
#include "ftp/ErrorCodes.H"

/*!
 *  NAME:
 *      controlFileFromLocation() - creates instance of type (ControlFile*)
 *                                            
 *
 *  DESCRIPTION:1.This functions instantiates a variable of type ControlFile*
 *                and allocates memory for it.
 *		        2.It returns NULL in case their is any error. 
 *		
 *  
 *  ARGUMENTS:
 *       -  pErrorList (IN / OUT)
 *       -  controlFileLocation (IN)	
 *
 *  RETURNS 	
 *	     Returns ControlFile*  if the operation is successful or NULL
 * 		 in case of failure.
 */
ControlFile* controlFileFromLocation(ListErrorStruct* pErrorList,
								     char* controlFileLocation)							
{
	/*Used to instantiate control file structure.*/
	ControlFile* pControlFile;
#ifdef DEBUG_IPDRFTP
	printf("Entering controlFileFromLocation()\n");
#endif
	if(!controlFileLocation){/*Error handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "controlFileFromLocation");
		return NULL;
	}

	pControlFile = (ControlFile*)malloc(sizeof(ControlFile));
	if(!pControlFile){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_MALLOC_FAILED,
					 "controlFileFromLocation");
		return NULL;
	}
	
	pControlFile->controlFileLocation_ = 
		(char*)malloc(strlen(controlFileLocation)+1);	
	if(!pControlFile->controlFileLocation_){/*Error handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_MALLOC_FAILED,
					 "controlFileFromLocation");
		free(pControlFile);
		return NULL;
	}
	/*Populating structure members*/
	strcpy(pControlFile->controlFileLocation_ ,controlFileLocation);
	pControlFile->controlFileURL_ = NULL;
	pControlFile->countNoOfVer_ = 0 ;
	pControlFile->docCount_ = 0;
	pControlFile->controlFilePointer_ = NULL;
#ifdef DEBUG_IPDRFTP
	printf("Leaving controlFileFromLocation()\n");
#endif
	return (pControlFile);
}

/*!
 *  NAME:
 *      controlFileFromURL() - creates instance of type (ControlFile*)
 *                                            
 *  DESCRIPTION:1.This functions instantiates a variable of type ControlFile*
 *                and allocates memory for it.
 *				2.Copies the control file URL to the control file structure.
 *				3.Gets control file from URL and makes a local copy.
 *       		
 *  
 *  ARGUMENTS:
 *       -  pErrorList (IN / OUT)
 *       -  controlFileURL (IN)	
 *
 *  RETURNS 	
 *	     Returns ControlFile* if the operation is successful or NULL
 * 		 in case of failure.
 */
ControlFile* controlFileFromURL(ListErrorStruct* pErrorList,
								char* controlFileURL)
{
	/*Used to instantiate control file structure variable.*/
	ControlFile* pControlFile;
	/*Passed in get_File()*/
	int errorCode = 0;
#ifdef DEBUG_IPDRFTP
	printf("Entering controlFileFromURL()\n");
#endif
	if(!controlFileURL){/*Error handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "controlFileFromURL");
		return NULL;
	}
	
	pControlFile = (ControlFile*)malloc(sizeof(ControlFile));
	if(!pControlFile){/*Error handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_MALLOC_FAILED,
					 "controlFileFromURL");
		return NULL;
	}

	pControlFile->controlFileURL_ = 
		(char*)malloc(strlen(controlFileURL)+1);
	if(!pControlFile->controlFileURL_){/*Error handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_MALLOC_FAILED,
					 "controlFileFromLocation");
		free(pControlFile);
		return NULL;
	}
	/*Populating control file structure members*/
	strcpy(pControlFile->controlFileURL_ ,controlFileURL);
	pControlFile->controlFileLocation_ = NULL;
	pControlFile->countNoOfVer_ = 0 ;
	pControlFile->docCount_ = 0;
	pControlFile->controlFilePointer_ = NULL;

	/*get control file from remote URL and make local copy.*/
	if((get_File(pControlFile->controlFileURL_,LOCAL_CONTROLFILE,
				"",	&errorCode))==1){
		ftpErrorHandler(pErrorList,/*error handling*/
					 errorCode,
					 "controlFileFromURL");
		free(pControlFile);
		return NULL;
	}
#ifdef DEBUG_IPDRFTP
	printf("Leaving controlFileFromURL()\n");
#endif
	return (pControlFile);
}

/*!
 *  NAME:
 *      freeControlFile() - free memory allocation of ControlFile variable.
 *                                            
 *
 *  DESCRIPTION:1.frees memory for control file structure.
 *       
 *  ARGUMENTS:
 *       -	pControlFile (IN / OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or 1
 * 		 in case of failure.
 */
int freeControlFile(ControlFile* pControlFile)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering freeControlFile()\n");
#endif
	if(!pControlFile)
		return 0;

	if(pControlFile->controlFileURL_)
		free(pControlFile->controlFileURL_);

	if(pControlFile->controlFileLocation_)
		free(pControlFile->controlFileLocation_);

	pControlFile->controlFilePointer_ = NULL;

	free(pControlFile);
#ifdef DEBUG_IPDRFTP
	printf("Leaving freeControlFile()\n");
#endif
	return 0;
}

/*!
 *  NAME:
 *      writeStart() - writes start of a control file.
 *                                            
 *
 *  DESCRIPTION:1.This functions opens the control file at control file location in "w" mode
 *                and writes the starting version string into it.
 *		
 *		
 *  ARGUMENTS:
 *       -	pErrorList (IN / OUT)
 *       -  pControlFile (IN / OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int writeStart(ListErrorStruct* pErrorList ,
			   ControlFile* pControlFile)
{
	FILE* stream;
    char buffer[MAX_VERSION_LEN];
#ifdef DEBUG_IPDRFTP
	printf("Entering writeStart()\n");
#endif
	if(!pControlFile->controlFileLocation_){/*Error handling*/
		ftpErrorHandler(pErrorList, 
					 IPDR_CTRLFILE_LOCATION_NULL,
					 "writeStart");
		return 1; 
	}

    if((stream = fopen(pControlFile->controlFileLocation_,"w"))==NULL){
		perror("Error while fopen");
		printf("The error is : %d, %s\n", errno, pControlFile->controlFileLocation_);
		ftpErrorHandler(pErrorList, /*Error Handling*/
					 IPDR_FOPEN_FAILED,
					 "writeStart");
		return 1; 
	}

    strcpy(buffer,VERSION_NUMBER);
    strcat(buffer,"\n");
    fputs(buffer,stream);
	pControlFile->docCount_ = 0;/*no document written yet*/
    fclose(stream);
#ifdef DEBUG_IPDRFTP
	printf("Leaving writeStart()\n");
#endif
    return 0;
}

/*!
 *  NAME:
 *      writeEnd() - writes end of a control file.
 *                                            
 *
 *  DESCRIPTION:1.This function appends the end version string into the control file
 *                and closes the file.
 *		
 *  ARGUMENTS:
 *       -	pErrorList (IN / OUT)
 *       -  pControlFile (IN / OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int writeEnd(ListErrorStruct* pErrorList ,
			 ControlFile* pControlFile)
{
	char buffer[MAX_VERSION_LEN];
#ifdef DEBUG_IPDRFTP
	printf("Entering writeEnd()\n");
#endif
	if(!pControlFile->controlFileLocation_){/*Error handling*/
		ftpErrorHandler(pErrorList, 
					 IPDR_CTRLFILE_LOCATION_NULL,
					 "writeEnd");
		return 1; 
	}

	/*open the file if not open in append mode.*/
	if(!pControlFile->controlFilePointer_){
		if((pControlFile->controlFilePointer_ = 
			fopen(pControlFile->controlFileLocation_,"a"))==NULL){
			ftpErrorHandler(pErrorList, /*Error Handling*/
						 IPDR_FOPEN_FAILED,
						 "writeEnd");
			return 1; 
		}
	}
	/*write version string*/
    strcpy(buffer ,VERSION_NUMBER);
    strcat(buffer,"\n");
    fputs(buffer ,pControlFile->controlFilePointer_);
	/*close the file*/
	fclose(pControlFile->controlFilePointer_);
	pControlFile->controlFilePointer_ = NULL;
#ifdef DEBUG_IPDRFTP
	printf("Leaving writeEnd()\n");
#endif
    return 0;
}

/*!
 *  NAME:
 *      addDocName() - adds document name to control file.
 *                                            
 *
 *  DESCRIPTION:1.This function appends document name into the control file
 *				  and increments document count.
 *		
 *  ARGUMENTS:
 *       -	pErrorList (IN / OUT)
 *       -  pControlFile (IN / OUT)
 *       -  docName (IN)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int addDocName(ListErrorStruct* pErrorList ,
			   ControlFile* pControlFile,
			   char* docName)
{
	char buffer[MAX_DOCUMENT_NAME];
	unsigned int length = 0;
#ifdef DEBUG_IPDRFTP
	printf("Entering addDocName()\n");
#endif
	if(!pControlFile->controlFileLocation_){/*Error handling*/
		ftpErrorHandler(pErrorList, 
					 IPDR_CTRLFILE_LOCATION_NULL,
					 "addDocName");
		return 1; 
	}

	/*open the file if not open in append mode.*/
	if(!pControlFile->controlFilePointer_){
		if((pControlFile->controlFilePointer_ = 
			fopen(pControlFile->controlFileLocation_,"a"))==NULL){
			ftpErrorHandler(pErrorList, /*Error Handling*/
						 IPDR_FOPEN_FAILED,
						 "addDocName");
			return 1; 
		}
	}
	/*write docName into file*/
    strcpy(buffer ,docName);
	length = strlen(docName);
	buffer[length] = '\n'; 
    buffer[length+1] = '\0';
	fputs(buffer ,pControlFile->controlFilePointer_);
	fflush(pControlFile->controlFilePointer_);
	pControlFile->docCount_++;/*incrementing docCount*/
#ifdef DEBUG_IPDRFTP
	printf("Leaving addDocName()\n");
#endif
	return 0;
}

/*!
 *  NAME:
 *      getDocName() - gets document name from control file.
 *                                            
 *
 *  DESCRIPTION:1.This functions reads line from control file.
 *              2.If the read line is version string then increments the number of versions.
 *				3.Now if number of versions is "1" then it reads another line and repeats step 1 and 2.
 *				4.If the read line is not version string then it is document name and is copied 
 *                into input string.Return.Else end of file is reached and number of versions is 2.Return.	
 * 
 *  ARGUMENTS:
 *       -	pErrorList (IN / OUT)
 *       -  pControlFile (IN)
 *       -  docName (IN / OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int getDocName(ListErrorStruct* pErrorList,
			   ControlFile* pControlFile, 
			   char* docName)
{	
	char buffer[200];
    char c = 0;
    int index  = 0;
#ifdef DEBUG_IPDRFTP
	printf("Entering getDocName()\n");
#endif
	if(!pControlFile || !docName){/*Error handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getDocName");
		return 1;
	}
	/*controlFileLocation_ and controlFileURL_ cannot co-exist*/
	if(pControlFile->controlFileLocation_ && pControlFile->controlFileURL_){
		ftpErrorHandler(pErrorList,
					 IPDR_LOCATION_URL_COEXIST,
					 "getDocName");
		return 1;
	}
	/*Initialize the File pointer and open it in read mode.*/
    if(!pControlFile->controlFilePointer_){
		/*opening local copy*/
        if(pControlFile->controlFileURL_){
			if((pControlFile->controlFilePointer_ =
				fopen(LOCAL_CONTROLFILE ,"r"))==NULL){
				ftpErrorHandler(pErrorList, /*Error Handling*/
							 IPDR_FOPEN_FAILED,
							 "getDocName");
				return 1; 
			}
		}
		/*opening control file at control file location.*/
		if(pControlFile->controlFileLocation_){
			if((pControlFile->controlFilePointer_ =
				fopen(pControlFile->controlFileLocation_ ,"r"))==NULL){
				ftpErrorHandler(pErrorList, /*Error Handling*/
							 IPDR_FOPEN_FAILED,
							 "getDocName");
				return 1; 
			}
		}
    }

    for( ;/*!feof(controlFile->controlFilePointer_)*/; ){
        index = 0;
		/*read line from file*/
        while((c=fgetc(pControlFile->controlFilePointer_))!='\n' && c!=EOF &&c!=13){
            buffer[index] = c;
            index ++;
        }/*while ends*/
        buffer[index] = '\0';
		//this is to move the pointer if it reads ascii value 13.
		if(c == 13)
			fgetc(pControlFile->controlFilePointer_);

        if(strcmp(buffer,VERSION_NUMBER)==0){
			/*first version string encountered*/
            if(pControlFile->countNoOfVer_==0){
                pControlFile->countNoOfVer_++;
                continue;
            }
            else{/*end reached.2nd version string encountered*/
                 pControlFile->countNoOfVer_++;/*countNoOfVer_ = 2*/
                 buffer[0] = 0;
				 strcpy(docName,buffer);
#ifdef DEBUG_IPDRFTP
	printf("Leaving getDocName()\n");
#endif
                 return 0;
            }
        }
        break;
    }/*for ends here*/

    if(c == EOF)
        strcpy(buffer, "NULL");

    strcpy(docName,buffer);
#ifdef DEBUG_IPDRFTP
	printf("Leaving getDocName()\n");
#endif
    return 0; 
}

/*!
 *  NAME:
 *      isEnd() - reads control file and checks whether end is reached.
 *                                            
 *
 *  DESCRIPTION:1.If number of versions is 2 then it returns 1 else it returns 0.
 *       
 *  ARGUMENTS:
 *       -	pErrorList (IN / OUT)
 *       -  pControlFile (IN / OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int isEnd(ListErrorStruct* pErrorList ,
		  ControlFile* pControlFile)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering isEnd()\n");
#endif
	if(!pControlFile){/*Error handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getDocName");
		return 1;
	}

	if(pControlFile->countNoOfVer_==2){
		fclose(pControlFile->controlFilePointer_);
		pControlFile->controlFilePointer_ = NULL;
		return 1;
	}
	return 0; 
}

/*!
 *  NAME:
 *      getDocCount() - gets document count.
 *                                            
 *
 *  DESCRIPTION:1.Gets document count from control file structure.
 *       
 *		
 *  ARGUMENTS:
 *       -	pErrorList (IN / OUT)
 *       -  pControlFile (IN) 
 *       -  docCount (IN / OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int getDocCount(ListErrorStruct* pErrorList ,
				ControlFile* pControlFile, 
				int* docCount)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering getDocCount()\n");
#endif
	if(!docCount || !pControlFile){/*Error handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getDocCount");
		return 1;
	}
	*docCount = pControlFile->docCount_;
#ifdef DEBUG_IPDRFTP
	printf("Leaving getDocCount()\n");
#endif
	return 0;
}


/*!
 *  NAME:
 *      refreshControlFile() - gets control file from remote URL 
 *                         and refreshes local copy.
 *                                            
 *
 *  DESCRIPTION:1.This function gets control file from URL and makes a local copy.
 *       
 *		
 *  ARGUMENTS:
 *       -	pErrorList (IN / OUT)
 *       -  pControlFile (IN)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int refreshControlFile(ListErrorStruct* pErrorList ,
					   ControlFile* pControlFile)
{
	int errorCode = 0;
#ifdef DEBUG_IPDRFTP
	printf("Entering refreshControlFile()\n");
#endif
	if(!pControlFile){/*Error handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getDocName");
		return 1;
	}

	/*get control file from URL.*/
	if((get_File(pControlFile->controlFileURL_,LOCAL_CONTROLFILE,
				"",	&errorCode))==1){/*error handling*/
		ftpErrorHandler(pErrorList,
					 errorCode,
					 "refreshControlFile");
		return 1;
	}
#ifdef DEBUG_IPDRFTP
	printf("Leaving refreshControlFile()\n");
#endif
	return 0;
}


