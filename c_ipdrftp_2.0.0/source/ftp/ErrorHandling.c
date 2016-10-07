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
 * File                 : ErrorHandling.c			         
 * Description          : Contains all the error handling      
                          related functions      
 * Author               : Infosys Tech Ltd           
 * Modification History :                            
 *-------------------------------------------------------
 * Date       Name             Change/Description    
 *-------------------------------------------------------
 * 01/29/02  Infosys           Created   
 *
 *
 */
 

#include <stdio.h>
#include "ftp/ErrorCodes.H"

/*!
 *  NAME:
 *      ftpErrorHandler() - handles occurence of an error condition.
 *                                            
 *
 *  DESCRIPTION: 1.This functions handles any error conditon 
 *                 and populates ListErrorStruct.
 *  
 *  ARGUMENTS:
 *       -  pErrorList (IN / OUT)
 *       -	errorCode (IN)
 *       -  functionName (IN) 
 *        
 *  RETURNS 	
 *	     Returns Zero 
 */
int
ftpErrorHandler(ListErrorStruct* pErrorList,
			 int errorCode,
			 char* functionName)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering ftpErrorHandler()\n");
#endif
	/*go to end of linked list*/
	for(;pErrorList && pErrorList->nextErrorStructureList_;)
		pErrorList = pErrorList->nextErrorStructureList_;

	/*the first node is already there*/
	if(pErrorList->errorStructure_)
		pErrorList->nextErrorStructureList_ = 
			createListErrorNode(errorCode,
								functionName,
								getErrorMesg(errorCode));

	/*first node is not populated.*/
	else
		pErrorList->errorStructure_ =
			newErrorStructure(errorCode,
							  functionName,
							  getErrorMesg(errorCode));
#ifdef DEBUG_IPDRFTP
	printf("Leaving ftpErrorHandler()\n");
#endif
	return 0;	
}

/*!
 *  NAME:
 *      createListErrorNode() - Creats new errorlist node.
 *                                            
 *
 *  DESCRIPTION: 1.This function creates a new node for ListErrorStruct 
 *                 populates it with error code,function name and error message.
 *  
 *  ARGUMENTS:
 *       -  errorCode (IN)
 *       -  functionName (IN)
 *       -  errorMessage (IN)	
 *        
 *  RETURNS 	
 *	     Returns pErrorList
 */
ListErrorStruct*
createListErrorNode(int errorCode,
					char* functionName,
					char* errorMessage)
{
	ListErrorStruct* pErrorList;
	pErrorList = 
		(ListErrorStruct*)malloc(sizeof(ListErrorStruct));
#ifdef DEBUG_IPDRFTP
	printf("Entering createListErrorNode()\n");
#endif
	if(pErrorList == NULL)
		return NULL;
	
	/*create errorStructure*/
	pErrorList->errorStructure_ = 
		newErrorStructure(errorCode,
						  functionName,
						  errorMessage);
		
	pErrorList->nextErrorStructureList_ = NULL;
#ifdef DEBUG_IPDRFTP
	printf("Leaving createListErrorNode()\n");
#endif
	return (pErrorList);
}

/*!
 *  NAME:
 *      newErrorStructure() - allocates memory for error structure and populates it.
 *                                            
 *
 *  DESCRIPTION: 1.This function allocates memory for newErrorStructure and is called
 *					by createListErrorNode.  
 *
 *  ARGUMENTS:
 *       -  errorCode (IN)
 *       -  functionName (IN)
 *       -  errorMessage (IN)
 *        
 *  RETURNS 	
 *	     Returns pointer of type ErrorStructure.
 */
ErrorStruct*
newErrorStructure(int errorCode,
				  char* functionName,
				  char* errorMessage)
{
	ErrorStruct* pErrorStructure =
		(ErrorStruct*)malloc(sizeof(ErrorStruct));
#ifdef DEBUG_IPDRFTP
	printf("Entering newErrorStructure()\n");
#endif
	if(!pErrorStructure)
		return NULL;

	pErrorStructure->errorCode_ = errorCode;

	pErrorStructure->errorMessage_ = 
		(char*)malloc(strlen(errorMessage)+1);
	strcpy(pErrorStructure->errorMessage_,
		   errorMessage);
	
	pErrorStructure->functionName_ = 
		(char*)malloc(strlen(functionName)+1);
	strcpy(pErrorStructure->functionName_,
		   functionName);

	free(errorMessage);
#ifdef DEBUG_IPDRFTP
	printf("Leaving newErrorStructure()\n");
#endif
	return (pErrorStructure);
}

/*!
 *  NAME:
 *      newListErrorStructure() - allocates memory for ListErrorStruct* variable.
 *                                            
 *
 *  DESCRIPTION: 1.This function allocates memory for ListErrorStruct.
 *               2.It is called by Producer or Concumer application.
 *		
 *  
 *  ARGUMENTS:
 *       -  void
 *        
 *  RETURNS 	
 *	     Returns pointer of type ListErrorStruct.
 */
ListErrorStruct*
newListErrorStructure()
{
	ListErrorStruct* pErrorList = 
	    (ListErrorStruct*)malloc(sizeof(ListErrorStruct));
#ifdef DEBUG_IPDRFTP
	printf("Entering newListErrorStructure()\n");
#endif
	pErrorList->errorStructure_ = NULL;
	pErrorList->nextErrorStructureList_ = NULL;
#ifdef DEBUG_IPDRFTP
	printf("Leaving newListErrorStructure()\n");
#endif
	return (pErrorList);
}

/*!
 *  NAME:
 *      getErrorMesg() - gets appropriate error message.
 *                                            
 *
 *  DESCRIPTION: 1.This function returns error message corresponding to errorCode. 			
 *		
 *  
 *  ARGUMENTS:
 *       -	errorCode (IN) 
 *        
 *  RETURNS 	
 *	     Returns errorMessage.
 */
char*
getErrorMesg(int errorCode)				
{
	char* errorMessage =
		(char*)calloc(100,sizeof(char));
#ifdef DEBUG_IPDRFTP
	printf("Entering getErrorMesg()\n");
#endif
	switch(errorCode){
	case IPDR_CURL_COULDNT_CONNECT:
		strcpy(errorMessage, "Connection couldn't be established.");
		break;
	case IPDR_CURL_FTP_COULDNT_RETR_FILE:
		strcpy(errorMessage, "Could not retrieve the file at URL.");
		break;
	case IPDR_CURL_FTP_ACCESS_DENIED:
		strcpy(errorMessage, "Access denied on the file specified by URL.");
		break;
	case IPDR_CURL_LOCAL_FILENAME_NULL:
		strcpy(errorMessage, "Local file name can not be null.");
		break;
	case IPDR_CURL_LOCAL_FILENAME_BLANK:
		strcpy(errorMessage, "Local file name can not be blank");
		break;
	case IPDR_CURL_WRITE_ERROR:
		strcpy(errorMessage, "Could not write to the local file.");
		break;
	case IPDR_CURL_GENERAL_ERROR:
		strcpy(errorMessage, "Error ocurred while retrieving file.");
		break;
	case IPDR_CURL_NULL_URI:
		strcpy(errorMessage, "URL can not be null or blank.");
		break;
	/*Memory related errors*/
	case IPDR_MALLOC_FAILED:
		strcpy(errorMessage, "Memory allocation failed for the variable.");
		break;
	case IPDR_GROUPID_NULL_OR_BLANK:
		strcpy(errorMessage, "Group ID can not be null or blank.");
		break;
	case IPDR_CTRLFILE_DIRECTORY_NULL_OR_BLANK:
		strcpy(errorMessage, "Control file directory can not be null or blank.");
		break;
	case IPDR_CTRLFILE_PREFIX_NULL:
		strcpy(errorMessage, "Control file prefix can not be null.");
		break;
	case IPDR_CTRLFILE_NAMEPOLICY_NULL_OR_BLANK:
		strcpy(errorMessage, "Control file name policy can not be null or blank.");
		break;
	case IPDR_CTRLFILE_SUFFIX_NULL:
		strcpy(errorMessage, "Control file suffix can not be null.");
		break;
	case IPDR_CAPABILITYFILE_URL_NULL_OR_BLANK:
		strcpy(errorMessage, "Capability File URL can not be null or blank.");
		break;
	case IPDR_FUNCTION_PARAMETER_NULL:
		strcpy(errorMessage, "Arguments to this function cannot be NULL.");
		break;
	/*Requirement specific errors.*/
	case IPDR_INVALID_SEQUENCE_NUMBER:
		strcpy(errorMessage, "Invalid sequence number for the control file.");
		break;
	case IPDR_COULDNT_CREATE_NEWNODE:
		strcpy(errorMessage, "Could not create a new node in the SubscriptionGroup list.");
		break;
	case IPDR_SUBSGROUP_NOT_FOUND:
		strcpy(errorMessage, "Subscription Group not found in the Subscription Group list.");
		break;
	case IPDR_SUBSGROUP_LIST_EMPTY:
		strcpy(errorMessage, "Subscription Group list is empty.");
		break;
	case IPDR_GROUPID_NOT_UNIQUE:
		strcpy(errorMessage, "Subscription Group groupId has to be unique");
		break;
	case IPDR_URL_INVALID:
		strcpy(errorMessage, "Input URL is invalid");
		break;
	case IPDR_GROUPID_CAPABILITY_NOT_UNIQUE:
		strcpy(errorMessage, "In xml capability file groupIds are not unique.");
		break;
	case IPDR_LOCATION_URL_COEXIST:
		strcpy(errorMessage, "In control file structure both location and URL cannot exist.");
		break;
	case IPDR_CTRLFILE_LOCATION_NULL:
		strcpy(errorMessage, "In control file structure location cannot be NULL for ");
		break;
	case IPDR_INVALID_CAPABILITY_XML:
		strcpy(errorMessage, "The xml capability file is invalid.");
		break;
	case IPDR_LOCAL_DIR_NOT_SET:
		strcpy(errorMessage, "The control file local directory value not set for this group.");
		break;
	/*errors on file operations*/
	case IPDR_FOPEN_FAILED:
		strcpy(errorMessage, "File open operation failed.");
		break;

	default:
		strcpy(errorMessage,"Unknown error occured");
	}
#ifdef DEBUG_IPDRFTP
	printf("Leaving getErrorMesg()\n");
#endif
	return (errorMessage);
}

/*!
 *  NAME:
 *      validateSubsGroupList() - validates SubscriptionGroupList
 *                                            
 *
 *  DESCRIPTION: 1.This function validates subscriptionGroupList by calling 			
 *		           validateSubscriptionGroup() for all the subscription groups.
 *  
 *  ARGUMENTS:
 *       -	pSubsGroupList (IN) 
 *        
 *  RETURNS 	
 *	     Returns appropriate error code on error else it returns 0.
 */
int validateSubsGroupList(SubscriptionGroupList* pSubsGroupList)
{
	SubscriptionGroupList* pOuterTemp = pSubsGroupList;
	SubscriptionGroupList* pInnerTemp = NULL;
	int result = 0;
	int errorCode = 0;
#ifdef DEBUG_IPDRFTP
	printf("Entering validateSubsGroupList()\n");
#endif
	if(!pSubsGroupList)/*list is empty*/
		return 0;

	/*validating values of group members*/
	for(;pOuterTemp;){
		if((errorCode = validateSubscriptionGroup(pOuterTemp->subscriptionGroup_))!=0)
			return errorCode;
		pOuterTemp = pOuterTemp->nextSubscriptionGroup_;
	}
	pOuterTemp = pSubsGroupList;
	/*check whether groupIds are unique*/
	for(;pOuterTemp;){
		pInnerTemp = pOuterTemp->nextSubscriptionGroup_;
		for(;pInnerTemp;){
			if((result = strcmp(pOuterTemp->subscriptionGroup_.groupID_,
				pInnerTemp->subscriptionGroup_.groupID_))==0)
				return IPDR_GROUPID_CAPABILITY_NOT_UNIQUE;
			pInnerTemp = pInnerTemp->nextSubscriptionGroup_;
		}
		pOuterTemp = pOuterTemp->nextSubscriptionGroup_;
	}				             
#ifdef DEBUG_IPDRFTP
	printf("Leaving validateSubsGroupList()\n");
#endif
	return 0;
}

/*!
 *  NAME:
 *      validateSubscriptionGroup() - validates input subscription group.
 *                                            
 *
 *  DESCRIPTION: 1.This function validates input subscription group by calling 
 *				   validateSubsGroupFields().	  
 *
 *  ARGUMENTS:
 *       -  subscriptionGroup (IN)
 *        
 *  RETURNS 	
 *	     Returns appropriate error code on error else it returns 0.
 */
int validateSubscriptionGroup(SubscriptionGroup subscriptionGroup)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering validateSubscriptionGroup()\n");
#endif
	return (validateSubsGroupFields(subscriptionGroup.groupID_,
									subscriptionGroup.controlFileDirectory_,
									subscriptionGroup.controlFilePrefix_,
									subscriptionGroup.controlFileNamePolicy_,
									subscriptionGroup.controlFileSuffix_));
}

/*!
 *  NAME:
 *      validateSubsGroupFields() - validates members of subscription group.
 *                                            
 *
 *  DESCRIPTION: 1.This function validates groupId,controlFileDirectory,controlFilePrefix
 *				   controlFilePrefix ,controlFileNamePolicy and controlFileSuffix.	  
 *
 *  ARGUMENTS:
 *       -  groupID (IN)
 *       -	controlFileDirectory (IN)
 *       -  controlFilePrefix (IN)
 *       -  controlFileNamePolicy (IN)
 *       -  controlFileSuffix (IN) 
 *        
 *  RETURNS 	
 *	     Returns appropriate error code on error else it returns 0.
 */
int validateSubsGroupFields(char* groupID ,
							char* controlFileDirectory,
							char* controlFilePrefix,
							char* controlFileNamePolicy,
							char* controlFileSuffix)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering validateSubsGroupFields()\n");
#endif
	if(!groupID || strlen(groupID)==0)
		return IPDR_GROUPID_NULL_OR_BLANK;
	
	if(!controlFileDirectory || strlen(controlFileDirectory)==0)
		return IPDR_CTRLFILE_DIRECTORY_NULL_OR_BLANK;
	
	if(!controlFilePrefix)
		return IPDR_CTRLFILE_PREFIX_NULL;
	
	if(!controlFileNamePolicy || strlen(controlFileNamePolicy)==0)
		return IPDR_CTRLFILE_NAMEPOLICY_NULL_OR_BLANK;
	
	if(!controlFileSuffix)
		return IPDR_CTRLFILE_SUFFIX_NULL;
#ifdef DEBUG_IPDRFTP
	printf("Leaving validateSubsGroupFields()\n");
#endif
	return 0;
}

/*!
 *  NAME:
 *      isGroupIdUnique() - checks whether groupId passed is unique or not.
 *                                            
 *
 *  DESCRIPTION: 1.This function checks if the groupId already exists in subscription
 *				   group list.	   
 *
 *  ARGUMENTS:
 *       -  pSubsGroupList (IN)
 *       -	groupId (IN) 
 *        
 *  RETURNS 	
 *	     Returns appropriate error code on error else it returns 0.
 */
int isGroupIdUnique(SubscriptionGroupList* const pSubsGroupList,
					char* groupId)
{
	SubscriptionGroupList* pTemp = pSubsGroupList;
#ifdef DEBUG_IPDRFTP
	printf("Entering isGroupIdUnique()\n");
#endif
	if(!groupId || strlen(groupId) == 0)
		return IPDR_GROUPID_NULL_OR_BLANK;
	for(;pTemp;){
		if(strcmp(pTemp->subscriptionGroup_.groupID_,groupId)==0)
			return IPDR_GROUPID_NOT_UNIQUE;
		pTemp = pTemp->nextSubscriptionGroup_;
	}
#ifdef DEBUG_IPDRFTP
	printf("Leaving isGroupIdUnique()\n");
#endif
	return 0;
}

/*!
 *  NAME:
 *      validateSequenceNbr() - checks whether sequence number is valid.
 *                                            
 *
 *  DESCRIPTION: 1.This fuction returns error if the sequence number is blank or 
 *                 contains charachers other that 0-9.
 *
 *  ARGUMENTS:
 *       -  pSubsGroupList (IN)
 *       -	groupId (IN) 
 *        
 *  RETURNS 	
 *	     Returns appropriate error code on error else it returns 0.
 */
int validateSequenceNbr(char* sequenceNbr)
{
	unsigned int length;
	unsigned int index;
#ifdef DEBUG_IPDRFTP
	printf("Entering validateSequenceNbr()\n");
#endif
	if((length = strlen(sequenceNbr)) == 0)
		return IPDR_EMPTY_SEQUENCE_NUMBER;
	for(index = 0;index < length;index++)
		if(sequenceNbr[index] < '0' || sequenceNbr[index] > '9')
			return IPDR_INVALID_SEQUENCE_NUMBER;
#ifdef DEBUG_IPDRFTP
	printf("Leaving validateSequenceNbr()\n");
#endif
	return 0;
}

/*!
 *  NAME:
 *      validateURL() - checks whether URL is valid.
 *                                            
 *
 *  DESCRIPTION: 1.This function validates URL and returns erro code if
 *                 URL is invalid.
 *
 *
 *  ARGUMENTS:
 *       -  pSubsGroupList (IN)
 *       -	groupId (IN) 
 *        
 *  RETURNS 	
 *	     Returns appropriate error code on error else it returns 0.
 */
int validateURL(char* URL)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering validateURL()\n");
#endif
#ifdef WIN32
	if((memicmp(URL,"ftp",3))==0)
		return 0;
	if((memicmp(URL,"file",4))==0)
		return 0;
	if((memicmp(URL,"http",4))==0)
		return 0;
#endif
#ifndef WIN32
	if((strncasecmp(URL,"ftp",3))==0)
		return 0;
	if((strncasecmp(URL,"file",4))==0)
		return 0;
	if((strncasecmp(URL,"http",4))==0)
		return 0;
#endif
#ifdef DEBUG_IPDRFTP
	printf("Leaving validateURL()\n");
#endif
	return IPDR_URL_INVALID;
}

/*!
 *  NAME:
 *      freeListErrorStruct() - frees memory for error structure list.
 *                                            
 *
 *  DESCRIPTION: 1.frees memory for linked list of error structures.
 *
 *  ARGUMENTS:
 *       -  listError (IN)

 *        
 *  RETURNS 	
 *        Returns 0.
 */
int freeListErrorStruct(ListErrorStruct* listError)
{
	ListErrorStruct* tempListError = NULL;
#ifdef DEBUG_IPDRFTP
	printf("Entering freeListErrorStruct()\n");
#endif
	/*free memory of all the nodes of subscription group linked list.*/
	for(;listError;){
		tempListError = listError->nextErrorStructureList_;
		if(listError->errorStructure_){
			free(listError->errorStructure_->errorMessage_);
			free(listError->errorStructure_->functionName_);
			free(listError->errorStructure_);
		}
		free(listError);
		listError = tempListError;
	}
#ifdef DEBUG_IPDRFTP
	printf("Leaving freeListErrorStruct()\n");
#endif
	return 0;
}

/*!
 *  NAME:
 *      printLastError() - prints last error.
 *                                            
 *
 *  DESCRIPTION: Traverses error list and prints function name and error message
 *				for the last error.
 *
 *  ARGUMENTS:
 *       -  pListError (IN)

 *        
 *  RETURNS 	
 *        Returns 0.
 */
int printLastError(ListErrorStruct* pListError)
{
	ListErrorStruct* pTempListError = pListError;
#ifdef DEBUG_IPDRFTP
	printf("Entering printLastError()\n");
#endif
	/*Traverse error list and go to end*/
	for(;pTempListError && pTempListError->nextErrorStructureList_;)
		pTempListError = pTempListError->nextErrorStructureList_;
	printf("In function = %s  ",pTempListError->errorStructure_->functionName_);
	printf("error message = %s\n",pTempListError->errorStructure_->errorMessage_);
#ifdef DEBUG_IPDRFTP
	printf("Leaving printLastError()\n");
#endif
	return 0;
}

/*!
 *  NAME:
 *      printAllErrors() - prints error information for all errors.
 *                                            
 *
 *  DESCRIPTION: Prints function name and error message for all errors.
 *
 *  ARGUMENTS:
 *       -  pListError (IN)
 *
 *        
 *  RETURNS 	
 *        Returns 0.
 */
int printAllErrors(ListErrorStruct* pListError)
{
	ListErrorStruct* pTempListError = pListError;
#ifdef DEBUG_IPDRFTP
	printf("Entering printAllErrors()\n");
#endif
	/*Traverse error list and print errors*/
	for(;pTempListError;){
		if(pTempListError->errorStructure_){
			printf("In function = %s ",pTempListError->errorStructure_->functionName_);
			printf("error message = %s\n",pTempListError->errorStructure_->errorMessage_);
		}
		pTempListError = pTempListError->nextErrorStructureList_;
	}
#ifdef DEBUG_IPDRFTP
	printf("Leaving printAllErrors()\n");
#endif
	return 0;
}
