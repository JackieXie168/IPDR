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
 * File                 : RangeFile.c			         
 * Description          : Contains all the RangeFile      
                          functions      
 * Author               : Infosys Tech Ltd           
 * Modification History :                            
 *---------------------------------------------------
 * Date       Name             Change/Description    
 *---------------------------------------------------
 * 01/29/02  Infosys           Created   
 *
 *
 * 
 */
 

#include "ftp/RangeFile.H"
#include "ftp/ErrorCodes.H"


/*!
 *  NAME:
 *      getFirstSequenceNbr() - reads first sequence number 
 *                           from rangefile.
 *                                            
 *
 *  DESCRIPTION:1.Gets remote file from URL and makes a local copy.
 *				2.Reads first sequence number from the range file.
 *		
 *  
 *  ARGUMENTS:
 *       -	pErrorList (IN / OUT)
 *       -  rangeFilePath (IN)
 *       -  firstSeqNbr (IN / OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int getFirstSequenceNbr(ListErrorStruct* pErrorList,
						char* rangeFile,
						char* firstSeqNbr)
{
	FILE* stream;
	int index = 0;	 
	char c = 0;
	int error = 0;

#ifdef DEBUG_IPDRFTP
	printf("Entering getFirstSequenceNbr()\n");
#endif

	if(!firstSeqNbr || !rangeFile){/*Error handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getFirstSequenceNbr");
		return 1;
	}
	/*if the range file corresponds to range file URL*/
	if(validateURL(rangeFile)==0){
		/*get range file from URL.*/
		if((get_File(rangeFile,LOCAL_RANGEFILE,"",&error))==1){
			/*error handling*/
			ftpErrorHandler(pErrorList,
						 error,
						 "getFirstSequenceNbr");
			return 1;
		}
		/*open the file in read mode*/
		stream  = fopen(LOCAL_RANGEFILE , "r" );
	}
	/*rangeFile corresponds to renge file location and open it.*/
	else 
		stream = fopen(rangeFile,"r");

	if(stream == NULL){/*Error Handling*/
		ftpErrorHandler(pErrorList, 
					 IPDR_FOPEN_FAILED,
					 "getFirstSequenceNbr");
		return 1; 
	}

	/*read first sequenceNbr from file*/
	while((c=fgetc(stream))!='-' && c!=EOF){	         
		firstSeqNbr[index] = c;	    
		index++;	    
	}	   
	firstSeqNbr[index] = '\0';
	fclose(stream);
	
	/*checking validity of sequence number.*/
	if((error = validateSequenceNbr(firstSeqNbr))!=0){
		/*empty sequence number*/
		if(error == IPDR_EMPTY_SEQUENCE_NUMBER)
			return 0;
		ftpErrorHandler(pErrorList, /*Error Handling*/
					 error,
					 "getFirstSequenceNbr");
		firstSeqNbr[0] = 0;
		return 1; 
	}
#ifdef DEBUG_IPDRFTP
	printf("Leaving getFirstSequenceNbr()\n");
#endif
	return 0;
}


/*!
 *  NAME:
 *      getLastSequenceNbr() - reads Last sequence number from 
 *                           range file.  
 *                                            
 *
 *  DESCRIPTION:1.Gets range file from URL and makes a local copy of the file.
 *				2.Reads last sequence number from the file.
 *		
 *  
 *  ARGUMENTS:
 *       -	pErrorList (IN / OUT)
 *       -  rangeFilePath (IN)
 *       -  lastSeqNbr (IN / OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int getLastSequenceNbr(ListErrorStruct* pErrorList ,
					   char* rangeFile, 
					   char* lastSeqNbr)
{
	FILE* stream;
	int index = 0;	 
	char c = 0;	 
	int errorCode = 0;
#ifdef DEBUG_IPDRFTP
	printf("Entering getLastSequenceNbr()\n");
#endif
	if(!lastSeqNbr){/*Error handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getLastSequenceNbr");
		return 1;
	}
	/*if rangeFile corresponds to range file URL.*/
	if(validateURL(rangeFile)==0){
		/*get range file from URL.*/
		if((get_File(rangeFile,LOCAL_RANGEFILE,"",&errorCode))==1){
			ftpErrorHandler(pErrorList,/*error handling*/
						 errorCode,
						 "getLastSequenceNbr");
			return 1;
		}
		/*open the file in read mode*/
		stream  = fopen(LOCAL_RANGEFILE , "r" );
	}
	/*rangeFile corresponds to range file location.*/
	else
		stream = fopen(rangeFile,"r");
	if(stream == NULL){ /*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FOPEN_FAILED,
					 "getLastSequenceNbr");
		return 1; 
	}

	while((c=fgetc(stream))!='-' && c!=EOF);	   
	while((c=fgetc(stream))!=EOF && c!='\n' && c!=13){
		lastSeqNbr[index] = c;	    
		index++;	    
	}	
	lastSeqNbr[index] = '\0';
	fclose(stream);

	/*checking validity of sequence number.*/
	if((errorCode = validateSequenceNbr(lastSeqNbr))!=0){
		/*empty sequence number*/
		if(errorCode == IPDR_EMPTY_SEQUENCE_NUMBER)
			return 0;
		ftpErrorHandler(pErrorList, /*Error Handling*/
					 errorCode,
					 "getLastSequenceNbr");
		lastSeqNbr[0] = 0;
		fclose(stream);
		return 1; 
	}
#ifdef DEBUG_IPDRFTP
	printf("Leaving getLastSequenceNbr()\n");
#endif
	return 0;
}


/*!
 *  NAME:
 *      setFirstSequenceNbr() - writes first sequence number to 
 *                           range file.
 *                                            
 *
 *  DESCRIPTION:1.Reads last sequence number from range file location.
 *				2.Makes a range file string by appending last sequence number to
 *				  input first sequence number.
 *				3.Write the string at range file location.
 *		
 *  
 *  ARGUMENTS:
 *       -	pErrorList (IN / OUT)
 *       -  rangeFilePath (IN)
 *       -  firstSeqNbr (IN)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int setFirstSequenceNbr(ListErrorStruct* pErrorList ,
						char* rangeFilePath, 
						char* firstSeqNbr)
{
	char lastSeqNbr[MAX_SEQNBR_LEN];
	char buffer[MAX_RANGEFILE_CONTENTS];
	FILE* stream;
	int errorCode = 0;
	int index = 0;	 
	char c = 0;	
#ifdef DEBUG_IPDRFTP
	printf("Entering setFirstSequenceNbr()\n");
#endif
	if(!rangeFilePath || !firstSeqNbr){/*Error handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "setFirstSequenceNbr");
		return 1;
	}

	/*checking validity of input sequence number.*/
	if((errorCode = validateSequenceNbr(firstSeqNbr))!=0){
		ftpErrorHandler(pErrorList, /*Error Handling*/
					 IPDR_INVALID_SEQUENCE_NUMBER,
					 "setFirstSequenceNbr");
		return 1; 
	}
					 
	/*get LastSequenceNumber from range file.*/
	if((stream  = fopen(rangeFilePath, "r")) == NULL )
		strcpy(lastSeqNbr,firstSeqNbr);
	else{
		while((c=fgetc(stream))!='-' && c!=EOF);	   
		while((c=fgetc(stream))!='\n' && c!=EOF && c!= 13){
			lastSeqNbr[index] = c;	    
			index++;	    
		}/*while ends*/
		lastSeqNbr[index] = '\0';
		/*validating lastseqNbr*/
		if((errorCode = validateSequenceNbr(lastSeqNbr))!=0)
			strcpy(lastSeqNbr,firstSeqNbr); 
		fclose(stream);
	}

	/*write to rangeFile*/
	strcpy(buffer ,firstSeqNbr);
	strcat(buffer ,"-");
	strcat(buffer ,lastSeqNbr);

	if((stream  = fopen(rangeFilePath, "w" )) == NULL ){
		ftpErrorHandler(pErrorList, /*Error Handling*/
					 IPDR_FOPEN_FAILED,
					 "setFirstSequenceNbr");
		return 1; 
	}
    fputs(buffer,stream);
    fclose(stream);
#ifdef DEBUG_IPDRFTP
	printf("Entering setFirstSequenceNbr()\n");
#endif
	return 0;
}


/*!
 *  NAME:
 *      setLastSequenceNbr() - writes Last sequence number to 
 *                           range file.
 *                                            
 *
 *  DESCRIPTION:1.Reads first sequence number from control file location.
 *				2.Makes range file string.
 *		        3.Writes range file string at range file location.
 *		
 *  
 *  ARGUMENTS:
 *       -	pErrorList (IN / OUT)
 *       -  rangeFilePath (IN)
 *       -  lastSeqNbr (IN)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int setLastSequenceNbr(ListErrorStruct* pErrorList ,
						  char* rangeFilePath, 
						  char* lastSeqNbr)
{
	char firstSeqNbr[MAX_SEQNBR_LEN];
	char buffer[MAX_RANGEFILE_CONTENTS];
	FILE* stream;
	int errorCode = 0;
	int index = 0;	 
	char c = 0;	
#ifdef DEBUG_IPDRFTP
	printf("Entering getFileStreamFromURL()\n");
#endif
	if(!rangeFilePath || !lastSeqNbr){/*Error handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "setLastSequenceNbr");
		return 1;
	}
	/*checking validity of input sequence number.*/
	if((errorCode = validateSequenceNbr(lastSeqNbr))!=0){
		ftpErrorHandler(pErrorList, /*Error Handling*/
					 IPDR_INVALID_SEQUENCE_NUMBER,
					 "setLastSequenceNbr");
		return 1; 
	}
	
	/*get FirstSequenceNumber */
	if((stream  = fopen(rangeFilePath, "r")) == NULL)
		/*copy lastSeqNbr to firstSeqNbr*/
		strcpy(firstSeqNbr,lastSeqNbr);

	else{/*read from file*/
		while((c=fgetc(stream))!='-' && c!=EOF){	         
			firstSeqNbr[index] = c;	    
			index++;	    
		}
		firstSeqNbr[index] = '\0';
		/*validating firstSeqNbr and ensure that firstSeqNbr is not greater than lastSeqNbr*/
		if((errorCode = validateSequenceNbr(firstSeqNbr))!=0)
			strcpy(firstSeqNbr,lastSeqNbr);
		fclose(stream); /*firstSeqNbr populated*/
	} 

	/*write to rangeFile*/
	strcpy(buffer ,firstSeqNbr);
	strcat(buffer ,"-");
	strcat(buffer ,lastSeqNbr);

	if((stream  = fopen(rangeFilePath, "w" )) == NULL ){
		ftpErrorHandler(pErrorList, /*Error Handling*/
					 IPDR_FOPEN_FAILED,
					 "setLastSequenceNbr");
		return 1; 
	}  

    fputs(buffer,stream);
    fclose(stream);
#ifdef DEBUG_IPDRFTP
	printf("Entering getFileStreamFromURL()\n");
#endif
	return 0;
}

