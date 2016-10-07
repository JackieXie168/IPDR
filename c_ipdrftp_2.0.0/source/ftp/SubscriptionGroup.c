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
 * File                 : SubscriptionGroup.c			         
 * Description          : Contains all the SubscriptionGroup      
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
 

#include"ftp/SubscriptionGroup.H"
#include"ftp/ErrorCodes.H"

/*!
 *  NAME:
 *      newSubscriptionGroup() - creates instance of type (SubscriptionGroup*).
 *                                            
 *
 *  DESCRIPTION:1.Allocates memory for subscription group structure and populates it.
 *				2.Before populating it validates input parameters and returns NULL if error.
 *		
 *  ARGUMENTS:
 *       -	pErrorList (IN / OUT)
 *       -  groupID (IN)
 *       -  controlFileDirectory (IN)
 *       -  controlFilePrefix (IN)
 *       -	controlFileNamePolicy (IN)
 *       -  controlFileSuffix (IN)
 *       -  controlFileDirURL (IN) 
 *        
 *  RETURNS 	
 *	     Returns SubscriptionGroup* if the operation is successful or NULL
 * 		 in case of failure.
 */
SubscriptionGroup* 
newSubscriptionGroup(ListErrorStruct* pErrorList,
					 char* groupID,
					 char* controlFileDirectory,
					 char* controlFilePrefix,
					 char* controlFileNamePolicy,
					 char* controlFileSuffix)
{
	SubscriptionGroup* pSubscriptionGroup;

	int errorCode = validateSubsGroupFields(groupID,
											controlFileDirectory,
											controlFilePrefix,
											controlFileNamePolicy,
											controlFileSuffix);
#ifdef DEBUG_IPDRFTP
	printf("Entering newSubscriptionGroup()\n");
#endif

	if(errorCode!=0){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_GROUPID_NULL_OR_BLANK,
					 "newSubscriptionGroup");
		return NULL;
	}

	pSubscriptionGroup = 
		(SubscriptionGroup*)malloc(sizeof(SubscriptionGroup));
		                           
	if(!pSubscriptionGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_MALLOC_FAILED,
					 "newSubscriptionGroup");
		return (pSubscriptionGroup); 
	}
	
	/*Allocating pSubscriptionGroup fields*/
	pSubscriptionGroup->groupID_ = 
		(char*)malloc(strlen(groupID)+1);
	pSubscriptionGroup->controlFileSuffix_ = 
		(char*)malloc(strlen(controlFileSuffix)+1);
	pSubscriptionGroup->controlFilePrefix_ = 
		(char*)malloc(strlen(controlFilePrefix)+1);
	pSubscriptionGroup->controlFileNamePolicy_ = 
		(char*)malloc(strlen(controlFileNamePolicy)+1);
	pSubscriptionGroup->controlFileDirectory_ = 
		(char*)malloc(strlen(controlFileDirectory)+1);

    if(!pSubscriptionGroup->groupID_ || 
		!pSubscriptionGroup->controlFileSuffix_ ||
		!pSubscriptionGroup->controlFilePrefix_ ||
		!pSubscriptionGroup->controlFileNamePolicy_ ||
		!pSubscriptionGroup->controlFileDirectory_){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_MALLOC_FAILED,
					 "newSubscriptionGroup");
		free (pSubscriptionGroup);
		return NULL; 
	}

	strcpy(pSubscriptionGroup->controlFileNamePolicy_ ,controlFileNamePolicy);
	strcpy(pSubscriptionGroup->controlFileDirectory_ ,controlFileDirectory);
	strcpy(pSubscriptionGroup->groupID_ ,groupID);
	strcpy(pSubscriptionGroup->controlFilePrefix_ ,controlFilePrefix);
	strcpy(pSubscriptionGroup->controlFileSuffix_ ,controlFileSuffix);

	/*handling of optional controlFileDirURL*/
	pSubscriptionGroup->controlFileLocalDirectory_ = NULL;
#ifdef DEBUG_IPDRFTP
	printf("Leaving newSubscriptionGroup()\n");
#endif
    return (pSubscriptionGroup);		                           
}

SubscriptionGroup* 
newSubscriptionGroupWithBrRecord(ListErrorStruct* pErrorList,
					 char* groupID,
					 char* controlFileDirectory,
					 char* controlFilePrefix,
					 char* controlFileNamePolicy,
					 char* controlFileSuffix,
                BRRecord brRecord)
{
	SubscriptionGroup* pSubscriptionGroup;

	int errorCode = validateSubsGroupFields(groupID,
											controlFileDirectory,
											controlFilePrefix,
											controlFileNamePolicy,
											controlFileSuffix);
#ifdef DEBUG_IPDRFTP
	printf("Entering newSubscriptionGroup()\n");
#endif

	if(errorCode!=0){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_GROUPID_NULL_OR_BLANK,
					 "newSubscriptionGroup");
		return NULL;
	}

	pSubscriptionGroup = 
		(SubscriptionGroup*)malloc(sizeof(SubscriptionGroup));
		                           
	if(!pSubscriptionGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_MALLOC_FAILED,
					 "newSubscriptionGroup");
		return (pSubscriptionGroup); 
	}
	
	/*Allocating pSubscriptionGroup fields*/
	pSubscriptionGroup->groupID_ = 
		(char*)malloc(strlen(groupID)+1);
	pSubscriptionGroup->controlFileSuffix_ = 
		(char*)malloc(strlen(controlFileSuffix)+1);
	pSubscriptionGroup->controlFilePrefix_ = 
		(char*)malloc(strlen(controlFilePrefix)+1);
	pSubscriptionGroup->controlFileNamePolicy_ = 
		(char*)malloc(strlen(controlFileNamePolicy)+1);
	pSubscriptionGroup->controlFileDirectory_ = 
		(char*)malloc(strlen(controlFileDirectory)+1);

   pSubscriptionGroup->brRecord_.homeServiceProviderType_ = brRecord.homeServiceProviderType_;
   pSubscriptionGroup->brRecord_.homeServiceProvider_ = 
		(char*)malloc(strlen(brRecord.homeServiceProvider_)+1);
   pSubscriptionGroup->brRecord_.homeServiceProviderName_ = 
		(char*)malloc(strlen(brRecord.homeServiceProviderName_)+1);
   pSubscriptionGroup->brRecord_.accessProviderType_ = brRecord.accessProviderType_;
   pSubscriptionGroup->brRecord_.accessProvider_ = 
		(char*)malloc(strlen(brRecord.accessProvider_)+1);
   pSubscriptionGroup->brRecord_.accessProviderName_ = 
		(char*)malloc(strlen(brRecord.accessProviderName_)+1);
   pSubscriptionGroup->brRecord_.contractReference_ = 
		(char*)malloc(strlen(brRecord.contractReference_)+1);
   pSubscriptionGroup->brRecord_.businessRelationshipID_ = 
		(char*)malloc(strlen(brRecord.businessRelationshipID_)+1);
   pSubscriptionGroup->brRecord_.summaryXchgHrs_ = brRecord.summaryXchgHrs_;
   pSubscriptionGroup->brRecord_.settlementXcghDays_ = 
		(char*)malloc(strlen(brRecord.settlementXcghDays_)+1);
   pSubscriptionGroup->brRecord_.effectiveDateTime_ = 
		(char*)malloc(strlen(brRecord.effectiveDateTime_)+1);
   pSubscriptionGroup->brRecord_.response_ = brRecord.response_;
   pSubscriptionGroup->brRecord_.xchgProtocol_ = brRecord.xchgProtocol_;
   pSubscriptionGroup->brRecord_.xchgSLA_ = brRecord.xchgSLA_;
   pSubscriptionGroup->brRecord_.xchgEncoding_ = brRecord.xchgEncoding_;
   pSubscriptionGroup->brRecord_.disputeResolution_ = brRecord.disputeResolution_;
   pSubscriptionGroup->brRecord_.locationInfo_ = 
		(char*)malloc(strlen(brRecord.locationInfo_)+1);
   pSubscriptionGroup->brRecord_.BRContactName_ = 
		(char*)malloc(strlen(brRecord.BRContactName_)+1);
   pSubscriptionGroup->brRecord_.rejectReturn_ = brRecord.rejectReturn_;
   pSubscriptionGroup->brRecord_.frequencyOfAuditReconciliation_ = brRecord.frequencyOfAuditReconciliation_;
   pSubscriptionGroup->brRecord_.xchgCurrencyType_ = 
		(char*)malloc(strlen(brRecord.xchgCurrencyType_)+1);
   pSubscriptionGroup->brRecord_.unitOfMeasure_ = brRecord.unitOfMeasure_;
   pSubscriptionGroup->brRecord_.chargeableUnit_ = brRecord.chargeableUnit_;
   pSubscriptionGroup->brRecord_.ratePerChargeableUnit_ = brRecord.ratePerChargeableUnit_;
   pSubscriptionGroup->brRecord_.clearingIntermediaryName_ = 
		(char*)malloc(strlen(brRecord.clearingIntermediaryName_)+1);
   pSubscriptionGroup->brRecord_.billingIntermediaryName_ = 
		(char*)malloc(strlen(brRecord.billingIntermediaryName_)+1);
   pSubscriptionGroup->brRecord_.sendingServiceProviderDisposition_ = brRecord.sendingServiceProviderDisposition_;
   pSubscriptionGroup->brRecord_.lastChangedDateTime_ = 
		(char*)malloc(strlen(brRecord.lastChangedDateTime_)+1);
   pSubscriptionGroup->brRecord_.fileRetentionDays_ = brRecord.fileRetentionDays_;
    

if(!pSubscriptionGroup->groupID_ || 
		!pSubscriptionGroup->controlFileSuffix_ ||
		!pSubscriptionGroup->controlFilePrefix_ ||
		!pSubscriptionGroup->controlFileNamePolicy_ ||
		!pSubscriptionGroup->controlFileDirectory_ ||
      !pSubscriptionGroup->brRecord_.homeServiceProvider_ ||
      !pSubscriptionGroup->brRecord_.homeServiceProviderName_ ||
      !pSubscriptionGroup->brRecord_.accessProvider_ ||
      !pSubscriptionGroup->brRecord_.accessProviderName_ ||
      !pSubscriptionGroup->brRecord_.contractReference_ ||
      !pSubscriptionGroup->brRecord_.businessRelationshipID_ ||
      !pSubscriptionGroup->brRecord_.settlementXcghDays_ ||
      !pSubscriptionGroup->brRecord_.effectiveDateTime_ ||
      !pSubscriptionGroup->brRecord_.locationInfo_ ||
      !pSubscriptionGroup->brRecord_.BRContactName_ ||
      !pSubscriptionGroup->brRecord_.xchgCurrencyType_ ||
      !pSubscriptionGroup->brRecord_.clearingIntermediaryName_ ||
      !pSubscriptionGroup->brRecord_.billingIntermediaryName_ ||
      !pSubscriptionGroup->brRecord_.lastChangedDateTime_){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_MALLOC_FAILED,
					 "newSubscriptionGroup");
		free (pSubscriptionGroup);
		return NULL; 
	}

	strcpy(pSubscriptionGroup->controlFileNamePolicy_ ,controlFileNamePolicy);
	strcpy(pSubscriptionGroup->controlFileDirectory_ ,controlFileDirectory);
	strcpy(pSubscriptionGroup->groupID_ ,groupID);
	strcpy(pSubscriptionGroup->controlFilePrefix_ ,controlFilePrefix);
	strcpy(pSubscriptionGroup->controlFileSuffix_ ,controlFileSuffix);

   strcpy(pSubscriptionGroup->brRecord_.homeServiceProvider_, brRecord.homeServiceProvider_);
   strcpy(pSubscriptionGroup->brRecord_.homeServiceProviderName_, brRecord.homeServiceProviderName_);
   strcpy(pSubscriptionGroup->brRecord_.accessProvider_, brRecord.accessProvider_);
   strcpy(pSubscriptionGroup->brRecord_.accessProviderName_, brRecord.accessProviderName_);
   strcpy(pSubscriptionGroup->brRecord_.contractReference_, brRecord.contractReference_);
   strcpy(pSubscriptionGroup->brRecord_.businessRelationshipID_, brRecord.businessRelationshipID_);
   strcpy(pSubscriptionGroup->brRecord_.settlementXcghDays_, brRecord.settlementXcghDays_);
   strcpy(pSubscriptionGroup->brRecord_.effectiveDateTime_, brRecord.effectiveDateTime_);
   strcpy(pSubscriptionGroup->brRecord_.locationInfo_, brRecord.locationInfo_);
   strcpy(pSubscriptionGroup->brRecord_.BRContactName_, brRecord.BRContactName_);
   strcpy(pSubscriptionGroup->brRecord_.xchgCurrencyType_, brRecord.xchgCurrencyType_); 
   strcpy(pSubscriptionGroup->brRecord_.clearingIntermediaryName_, brRecord.clearingIntermediaryName_); 
   strcpy(pSubscriptionGroup->brRecord_.billingIntermediaryName_, brRecord.billingIntermediaryName_); 
   strcpy(pSubscriptionGroup->brRecord_.lastChangedDateTime_, brRecord.lastChangedDateTime_);

	/*handling of optional controlFileDirURL*/
	pSubscriptionGroup->controlFileLocalDirectory_ = NULL;
#ifdef DEBUG_IPDRFTP
	printf("Leaving newSubscriptionGroup()\n");
#endif
    return (pSubscriptionGroup);		                           
}
  
/*!
 *  NAME:
 *      freeSubscriptionGroup() - free memory for pSubscriptionGroup.
 *                                            
 *
 *  DESCRIPTION:1.Frees memory for subscription group.
 *       
 *		
 *		
 *  
 *  ARGUMENTS:
 *       -	pSubscriptionGroup (IN /OUT)
 *        
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int freeSubscriptionGroup(SubscriptionGroup* pSubscriptionGroup)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering freeSubscriptionGroup()\n");
#endif
	if(!pSubscriptionGroup)
		return 0;
	free(pSubscriptionGroup->controlFileDirectory_);
	free(pSubscriptionGroup->controlFileNamePolicy_);
	free(pSubscriptionGroup->controlFilePrefix_);
	free(pSubscriptionGroup->controlFileSuffix_);
	free(pSubscriptionGroup->groupID_);
	if(pSubscriptionGroup->controlFileLocalDirectory_)
		free(pSubscriptionGroup->controlFileLocalDirectory_);

	if(pSubscriptionGroup->brRecord_.homeServiceProvider_)
   	free(pSubscriptionGroup->brRecord_.homeServiceProvider_);
	if(pSubscriptionGroup->brRecord_.homeServiceProviderName_)
   	free(pSubscriptionGroup->brRecord_.homeServiceProviderName_);
	if(pSubscriptionGroup->brRecord_.accessProvider_)
   	free(pSubscriptionGroup->brRecord_.accessProvider_);
	if(pSubscriptionGroup->brRecord_.accessProviderName_)
   	free(pSubscriptionGroup->brRecord_.accessProviderName_);
	if(pSubscriptionGroup->brRecord_.contractReference_)
   	free(pSubscriptionGroup->brRecord_.contractReference_);
	if(pSubscriptionGroup->brRecord_.businessRelationshipID_)
   	free(pSubscriptionGroup->brRecord_.businessRelationshipID_);
	if(pSubscriptionGroup->brRecord_.settlementXcghDays_)
   	free(pSubscriptionGroup->brRecord_.settlementXcghDays_);
	if(pSubscriptionGroup->brRecord_.effectiveDateTime_)
   	free(pSubscriptionGroup->brRecord_.effectiveDateTime_);
	if(pSubscriptionGroup->brRecord_.locationInfo_)
   	free(pSubscriptionGroup->brRecord_.locationInfo_);
	if(pSubscriptionGroup->brRecord_.BRContactName_)
   	free(pSubscriptionGroup->brRecord_.BRContactName_);
	if(pSubscriptionGroup->brRecord_.xchgCurrencyType_)
   	free(pSubscriptionGroup->brRecord_.xchgCurrencyType_); 
	if(pSubscriptionGroup->brRecord_.clearingIntermediaryName_)
   	free(pSubscriptionGroup->brRecord_.clearingIntermediaryName_); 
	if(pSubscriptionGroup->brRecord_.billingIntermediaryName_)
   	free(pSubscriptionGroup->brRecord_.billingIntermediaryName_); 
	if(pSubscriptionGroup->brRecord_.lastChangedDateTime_)
		free(pSubscriptionGroup->brRecord_.lastChangedDateTime_);

	free(pSubscriptionGroup);
#ifdef DEBUG_IPDRFTP
	printf("Leaving freeSubscriptionGroup()\n");
#endif
	return 0;
}

/*!
 *  NAME:
 *      getGroupID() - gets groupID from pSubscriptionGroup.
 *                                            
 *
 *  DESCRIPTION:1.Gets groupId from subscription group and copies it
 *                into function argument groupID.
 *		
 *  
 *  ARGUMENTS:
 *       -	pErrorList (IN / OUT)
 *       -  pSubsGroup (IN)
 *       -  groupID (IN / OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int getGroupID(ListErrorStruct * pErrorList ,
			   SubscriptionGroup* pSubsGroup,
			   char* groupID)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering getGroupID()\n");
#endif
	if(!groupID || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getGroupID");
		return 1;
	}

	strcpy(groupID,pSubsGroup->groupID_);
#ifdef DEBUG_IPDRFTP
	printf("Leaving getGroupID()\n");
#endif
	return 0;
}


/*!
 *  NAME:
 *      getControlFileDirectory() - gets controlFileDirectory from pSubscriptionGroup.
 *                                            
 *
 *  DESCRIPTION:1.Gets controlFileDirectory from subscription group and copies it
 *                into function argument controlFileDir.
 *       
 *	
 *  ARGUMENTS:
 *       -	pErrorList (IN / OUT)
 *       -  pSubsGroup (IN)
 *       -  controlFileDir (IN / OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int getControlFileDirectory(ListErrorStruct* pErrorList ,
							SubscriptionGroup* pSubsGroup,
							char* controlFileDir)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering getControlFileDirectory()\n");
#endif
	if(!controlFileDir || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getControlFileDirectory");
		return 1;
	}

	strcpy(controlFileDir,pSubsGroup->controlFileDirectory_);
	//Added to validate '/' at the end of controlFileDirectory
	if(controlFileDir[strlen(controlFileDir) -1] != '/')
		strcat(controlFileDir , "/");
#ifdef DEBUG_IPDRFTP
	printf("Leaving getControlFileDirectory()\n");
#endif
	return 0;
}


/*!
 *  NAME:
 *      getControlFilePrefix() - gets controlFilePrefix from pSubscriptionGroup.
 *                                            
 *
 *  DESCRIPTION:1.Gets controlFilePrefix from subscription group and copies it
 *                into function argument controlFilePrefix.
 *       
 *	
 *  ARGUMENTS:
 *       -	pErrorList (IN / OUT)
 *       -  pSubsGroup (IN)
 *       -  controlFilePrefix (IN / OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int getControlFilePrefix(ListErrorStruct* pErrorList ,
						 SubscriptionGroup* pSubsGroup,
						 char* controlFilePrefix)
{
#ifdef DEBUG_IPDRFTP
	printf("Inside getControlFilePrefix()\n");
#endif
	if(!controlFilePrefix || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getControlFilePrefix");
		return 1;
	}

	strcpy(controlFilePrefix,pSubsGroup->controlFilePrefix_);
	return 0;

}

/*!
 *  NAME:
 *      getControlFileNamePolicy() - gets controlFilePrefix from pSubscriptionGroup.
 *                                            
 *
 *  DESCRIPTION:1.Gets namePolicy from subscription group and copies it
 *                into function argument namePolicy.
 *       

 *  ARGUMENTS:
 *       -	pErrorList (IN / OUT)
 *       -  pSubsGroup (IN)
 *       -  namePolicy (IN / OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int getControlFileNamePolicy(ListErrorStruct* pErrorList ,
							 SubscriptionGroup* pSubsGroup,
							 char* namePolicy)
{
#ifdef DEBUG_IPDRFTP
	printf("Inside getControlFileNamePolicy()\n");
#endif
	if(!namePolicy || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getControlFileNamePolicy");
		return 1;
	}

	strcpy(namePolicy ,pSubsGroup->controlFileNamePolicy_);
	return 0;
}

/*!
 *  NAME:
 *      getControlFileSuffix() - gets controlFileSuffix from SubscriptionGroup.
 *                                            
 *
 *  DESCRIPTION:1.Gets controlFileSuffix from subscription group and copies it
 *                into function argument fileSuffix.
 *       
 *  
 *  ARGUMENTS:
 *       -	pErrorList (IN / OUT)
 *       -  pSubsGroup (IN)
 *       -  fileSuffix (IN / OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int getControlFileSuffix(ListErrorStruct* pErrorList ,
						 SubscriptionGroup* pSubsGroup,
						 char* fileSuffix)
{
#ifdef DEBUG_IPDRFTP
	printf("Inside getControlFileSuffix()\n");
#endif
	if(!fileSuffix || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getControlFileSuffix");
		return 1;
	}

	strcpy(fileSuffix ,pSubsGroup->controlFileSuffix_);
	return 0;
}

/*!
 *  NAME:
 *      getURL() - gets controlFileDirectoryURL.
 *                                            
 *
 *  DESCRIPTION:1.Gets controlFileDirectiryURL from subscription group and copies it
 *                into function argument URL.
 *       
 *
 *  ARGUMENTS:
 *       -	pErrorList (IN / OUT)
 *       -  pSubsGroup (IN)
 *       -  URL (OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int getControlFileLocalDirectory(ListErrorStruct* pErrorList ,
								SubscriptionGroup* pSubsGroup,
								char* localDir)
{
#ifdef DEBUG_IPDRFTP
	printf("Inside getControlFileDirectoryURL()\n");
#endif
	if(!localDir || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getControlFileLocalDirectory");
		return 1;
	}

	if(pSubsGroup->controlFileLocalDirectory_){
		strcpy(localDir, pSubsGroup->controlFileLocalDirectory_);
		if(localDir[strlen(localDir) -1] != '/')
			strcat(localDir , "/");
	}

	else{ 
		ftpErrorHandler(pErrorList,
					 IPDR_LOCAL_DIR_NOT_SET,
					 "getControlFileLocalDirectory");
		return 1;
	}
		//strcpy(localDir,"NULL");
    return 0;
}

/*!
 *  NAME:
 *      setGroupID() - sets GroupID.
 *                                            
 *
 *  DESCRIPTION:1.this function copies groupId into subscription group.
 *       
 *		
 *  ARGUMENTS:
 *       -	pErrorList (IN / OUT)
 *       -  pSubsGroup (IN / OUT)
 *       -  groupID (IN)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int setGroupID(ListErrorStruct* pErrorList ,
			   SubscriptionGroup* pSubsGroup,
			   char* groupID)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering setGroupID()\n");
#endif
	if(!groupID || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "setGroupID");
		return 1;
	}
	if(strlen(groupID)==0){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_GROUPID_NULL_OR_BLANK,
					 "setGroupID");
		return 1;
	}

	/*reallocating memory if required*/
	if(strlen(groupID) > strlen(pSubsGroup->groupID_))
		pSubsGroup->groupID_ = 
			realloc(pSubsGroup->groupID_,(strlen(groupID)+1));

	strcpy(pSubsGroup->groupID_, groupID);
#ifdef DEBUG_IPDRFTP
	printf("Leaving setGroupID()\n");
#endif
	return 0;
}


/*!
 *  NAME:
 *      setControlFileDirectory() - sets controlFileDirectory.
 *                                            
 *
 *  DESCRIPTION:1.This function copies control file directory into subscription group.
 *       
 *		
 *  ARGUMENTS:
 *       -	pErrorList (IN / OUT)
 *       -  pSubsGroup (IN / OUT)
 *       -  controlFileDirectory (IN)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int setControlFileDirectory(ListErrorStruct* pErrorList ,
							SubscriptionGroup* pSubsGroup,
							char* controlFileDirectory)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering setControlFileDirectory()\n");
#endif
	if(!controlFileDirectory || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "setControlFileDirectory");
		return 1;
	}
	if(strlen(controlFileDirectory)==0){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_CTRLFILE_DIRECTORY_NULL_OR_BLANK,
					 "setControlFileDirectory");
		return 1;
	}
	/*reallocating memory if required*/
	if(strlen(controlFileDirectory) > strlen(pSubsGroup->controlFileDirectory_))
		pSubsGroup->controlFileDirectory_ = realloc(pSubsGroup->controlFileDirectory_,(strlen(controlFileDirectory)+1));

	strcpy(pSubsGroup->controlFileDirectory_,controlFileDirectory);
#ifdef DEBUG_IPDRFTP
	printf("Leaving setControlFileDirectory()\n");
#endif
	return 0;
}

/*!
 *  NAME:
 *      setControlFilePrefix() - sets controlFilePrefix.
 *                                            
 *
 *  DESCRIPTION:1.This function copies controlFilePrefix to subscription group.
 *       
 *		
 *  ARGUMENTS:
 *       -	pErrorList (IN / OUT)
 *       -  pSubsGroup (IN / OUT)
 *       -  controlFilePrefix (IN)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int setControlFilePrefix (ListErrorStruct* pErrorList ,
						  SubscriptionGroup* pSubsGroup,
						  char* controlFilePrefix)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering setControlFilePrefix()\n");
#endif
	if(!controlFilePrefix || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "setControlFilePrefix");
		return 1;
	}

	/*reallocating memory if required*/
	if(strlen(controlFilePrefix) > strlen(pSubsGroup->controlFilePrefix_))
		pSubsGroup->controlFilePrefix_ = 
			realloc(pSubsGroup->controlFilePrefix_,(strlen(controlFilePrefix)+1));

	strcpy(pSubsGroup->controlFilePrefix_,controlFilePrefix);
#ifdef DEBUG_IPDRFTP
	printf("Leaving setControlFilePrefix()\n");
#endif
	return 0;
}

/*!
 *  NAME:
 *      setControlFileNamePolicy() - sets controlFileNamePolicy.
 *                                            
 *
 *  DESCRIPTION:1.This function copies ControlFileNamePolicy to subscription group.
 *       
 *		
 *  ARGUMENTS:
 *       -	pErrorList (IN / OUT)
 *       -  pSubsGroup (IN / OUT)
 *       -  controlFileNamePolicy (IN)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int setControlFileNamePolicy(ListErrorStruct* pErrorList ,
							 SubscriptionGroup* pSubsGroup,
							 char* controlFileNamePolicy)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering setControlFileNamePolicy()\n");
#endif
	if(!controlFileNamePolicy || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "setControlFileNamePolicy");
		return 1;
	}
	if(strlen(controlFileNamePolicy)==0){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_CTRLFILE_NAMEPOLICY_NULL_OR_BLANK,
					 "setControlFileNamePolicy");
		return 1;
	}

	/*reallocating memory if required*/
	if(strlen(controlFileNamePolicy) > strlen(pSubsGroup->controlFileNamePolicy_))
		pSubsGroup->controlFileNamePolicy_ = 
			realloc(pSubsGroup->controlFileNamePolicy_,(strlen(controlFileNamePolicy)+1));

	strcpy(pSubsGroup->controlFileNamePolicy_,controlFileNamePolicy);
#ifdef DEBUG_IPDRFTP
	printf("Leaving setControlFileNamePolicy()\n");
#endif
	return 0;
}

/*!
 *  NAME:
 *      setControlFileSuffix() - sets controlFileSuffix.
 *                                            
 *
 *  DESCRIPTION:1.This function copies controlFileSuffix to subscription group.
 *       
 *  
 *  ARGUMENTS:
 *       -	pErrorList (IN / OUT)
 *       -  pSubsGroup (IN / OUT)
 *       -  controlFileSuffix (IN)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int setControlFileSuffix(ListErrorStruct* pErrorList ,
						 SubscriptionGroup* pSubsGroup,
						 char* controlFileSuffix)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering setControlFileSuffix()\n");
#endif
	if(!controlFileSuffix || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "setControlFileSuffix");
		return 1;
	}

	/*reallocating memory if required*/
	if(strlen(controlFileSuffix) > strlen(pSubsGroup->controlFileSuffix_))
		pSubsGroup->controlFileSuffix_ = 
			realloc(pSubsGroup->controlFileSuffix_,(strlen(controlFileSuffix)+1));

	strcpy(pSubsGroup->controlFileSuffix_ ,controlFileSuffix);
#ifdef DEBUG_IPDRFTP
	printf("Leaving setControlFileSuffix()\n");
#endif
	return 0;
}

/*!
 *  NAME:
 *      setControlFileDirectoryURL() - sets controlFileDirectoryURL.
 *                                            
 *
 *  DESCRIPTION:1.This function copies URL to subscription group.
 *       
 *
 *  ARGUMENTS:
 *       -	pErrorList (IN / OUT)
 *       -  pSubsGroup (IN / OUT)
 *       -  URL (IN)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int setControlFileLocalDirectory(ListErrorStruct* pErrorList ,
								SubscriptionGroup* pSubsGroup,
								char* localDir)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering setControlFileLocalDirectory()\n");
#endif
	if(!localDir || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "setControlFileDirectoryURL");
		return 1;
	}
	/*allocating or reallocating memory if required*/
	if(!pSubsGroup->controlFileLocalDirectory_ || 
	   strlen(localDir) > strlen(pSubsGroup->controlFileLocalDirectory_))
		pSubsGroup->controlFileLocalDirectory_ = 
			realloc(pSubsGroup->controlFileLocalDirectory_,(strlen(localDir)+1));

	strcpy(pSubsGroup->controlFileLocalDirectory_,localDir);
#ifdef DEBUG_IPDRFTP
	printf("Leaving setControlFileLocalDirectory()\n");
#endif
	return 0;
}

/*!
 *  NAME:
 *      getRangeFileName() - gets rangeFile name.
 *                                            
 *
 *  DESCRIPTION:1.This function copies groupID + "-range-file" to subscription group.
 *       
 *		
 *  ARGUMENTS:
 *       -	pErrorList (IN / OUT)
 *       -  pSubsGroup (IN)
 *       -  rangeFileName (IN / OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int getRangeFileName(ListErrorStruct* pErrorList ,
					  SubscriptionGroup* pSubsGroup,
					  char* rangeFileName)
{
#ifdef DEBUG_IPDRFTP
	printf("Inside getRangeFileName()\n");
#endif
	if(!rangeFileName || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getRangeFileName");
		return 1;
	}

	strcpy(rangeFileName,pSubsGroup->groupID_);
	strcat(rangeFileName,RANGEFILE_SUFFIX);
	return 0;
}


/*!
 *  NAME:
 *      getControlFileName() - gets  control file name.
 *                                            
 *
 *  DESCRIPTION:1.This function gets sequence based on policy and copies
 *                controlFilePrefix + sequencebasedOnPolicy +controlFileSuffix to
 *		          controlFileName.
 *		
 *  
 *  ARGUMENTS:
 *       -	pErrorList (IN / OUT)
 *       -  pSubsGroup (IN)
 *       -  controlFileName (IN / OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int getControlFileName(ListErrorStruct* pErrorList,
					   SubscriptionGroup* pSubsGroup,
					   char* controlFileName,
					   long sequenceNbr)
{
	char strMaxSeqNbr[MAX_SEQNBR_LEN];
	char seqBasedOnPolicy[MAX_SEQNBR_LEN];
	long maxSeqNbr ;
#ifdef DEBUG_IPDRFTP
	printf("Entering getControlFileName()\n");
#endif
	if(!controlFileName || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getControlFileName");
		return 1;
	}

	getMaxSequenceNumber(pErrorList ,pSubsGroup ,strMaxSeqNbr);
	maxSeqNbr = atol(strMaxSeqNbr);

	if(sequenceNbr > maxSeqNbr || sequenceNbr < 0){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_INVALID_SEQUENCE_NUMBER,
					 "getControlFileName");
		return 1;
	}
		
	getSequenceBasedOnPolicy(pErrorList,pSubsGroup,sequenceNbr,seqBasedOnPolicy);

	strcpy(controlFileName ,pSubsGroup->controlFilePrefix_);
	strcat(controlFileName ,seqBasedOnPolicy);
	strcat(controlFileName ,pSubsGroup->controlFileSuffix_);
#ifdef DEBUG_IPDRFTP
	printf("Leaving getControlFileName()\n");
#endif
	return 0;
}


/*!
 *  NAME:
 *      getMaxSequenceNumber() - gets maximum sequence number.
 *                                            
 *
 *  DESCRIPTION:1.This function gets controlFile name policy and then replaces 
 *                each 'N' by '9' and copies it to maxSequenceNumber.
 *		
 *  
 *  ARGUMENTS:
 *       -	pErrorList (IN / OUT)
 *       -  pSubsGroup (IN)
 *       -  maxSequenceNumber (IN / OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int getMaxSequenceNumber (ListErrorStruct* pErrorList ,
						  SubscriptionGroup* pSubsGroup,
						  char* maxSequenceNumber)
{
	int policyLength = 0;
	int index = 0;
#ifdef DEBUG_IPDRFTP
	printf("Entering getMaxSequenceNumber()\n");
#endif
	if(!maxSequenceNumber || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getMaxSequenceNumber");
		return 1;
	}

	policyLength = strlen(pSubsGroup->controlFileNamePolicy_);
	for(index = 0 ;index< policyLength;index++)
		maxSequenceNumber[index] = '9';
	maxSequenceNumber[index] = '\0';
#ifdef DEBUG_IPDRFTP
	printf("Leaving getMaxSequenceNumber()\n");
#endif
	return 0;
}

/*!
 *  NAME:
 *      getSequenceBasedOnPolicy() - gets sequence based on policy.
 *                                            
 *
 *  DESCRIPTION:1.This function converts input sequence number into policy based
 *                sequence number and copies it to policyBasedSeqNbr.
 *		
 *  
 *  ARGUMENTS:
 *       -	pErrorList (IN / OUT)
 *       -  pSubsGroup (IN)
 *       -  seqNbr (IN)
 *       -  policyBasedSeqNbr (IN / OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int getSequenceBasedOnPolicy (ListErrorStruct* pErrorList ,
							  SubscriptionGroup* pSubsGroup,
							  long seqNbr ,
							  char* policyBasedSeqNbr)
{
	int policyLength = 0;
	int index = 0;	    
	int seqNbrLength = 0;
	char strSeqNbr[MAX_SEQNBR_LEN];
#ifdef DEBUG_IPDRFTP
	printf("Entering getSequenceBasedOnPolicy()\n");
#endif
	if(!policyBasedSeqNbr || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getSequenceBasedOnPolicy");
		return 1;
	}

	sprintf(strSeqNbr ,"%d",seqNbr);
	policyLength = strlen(pSubsGroup->controlFileNamePolicy_);
    seqNbrLength = strlen(strSeqNbr);

	
	if(policyLength < seqNbrLength){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_INVALID_SEQUENCE_NUMBER,
					 "getSequenceBasedOnPolicy");
		return 1;
	}
	
	for(index = 0 ;index < (policyLength-seqNbrLength) ;index++)
		policyBasedSeqNbr[index] = '0';
	policyBasedSeqNbr[index] = '\0';

	strcat(policyBasedSeqNbr,strSeqNbr);
#ifdef DEBUG_IPDRFTP
	printf("Leaving getSequenceBasedOnPolicy()\n");
#endif
	return 0;
}

/*!
 *  NAME:
 *      incrementSequence() - increments sequence
 *                                            
 *
 *  DESCRIPTION:1.This function increments sequence number.
 *				2.If sequence is < 0 or >maxSequence then return error.
 *		
 *  
 *  ARGUMENTS:
 *       -	pErrorList (IN / OUT)
 *       -  pSubsGroup (IN)
 *       -  seqNbr (IN/OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int incrementSequence(ListErrorStruct* pErrorList,
					  SubscriptionGroup* pSubsGroup,
					  long* sequenceNbr)
{
	char strMaxSeqNbr[MAX_SEQNBR_LEN];
	long maxSeqNbr ;
#ifdef DEBUG_IPDRFTP
	printf("Entering incrementSequence()\n");
#endif
	if(!sequenceNbr || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "incrementSequence");
		return 1;
	}

	getMaxSequenceNumber(pErrorList ,pSubsGroup ,strMaxSeqNbr);
	maxSeqNbr = atol(strMaxSeqNbr);

	if(*sequenceNbr > maxSeqNbr || *sequenceNbr < 0){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_INVALID_SEQUENCE_NUMBER,
					 "incrementSequence");
		return 1; 
	}

	if(*sequenceNbr == maxSeqNbr)
		*sequenceNbr = 0;
	else
		(*sequenceNbr)++;
#ifdef DEBUG_IPDRFTP
	printf("Leaving incrementSequence()\n");
#endif
	return 0; 
}

/*!
 *  NAME:
 *      decrementSequence() - decrements sequence
 *                                            
 *
 *  DESCRIPTION:1.This function decrements sequence number.
 *				2.If sequence is < 0 or >maxSequence then return error.
 *		
 *        
 *  ARGUMENTS:
 *       -	pErrorList (IN / OUT)
 *       -  pSubsGroup (IN)
 *       -  seqNbr (IN/OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int decrementSequence(ListErrorStruct* pErrorList,
					  SubscriptionGroup* pSubsGroup,
					  long* sequenceNbr)
{
	char strMaxSeqNbr[MAX_SEQNBR_LEN];
	long maxSeqNbr ;
#ifdef DEBUG_IPDRFTP
	printf("Entering decrementSequence()\n");
#endif
	if(!sequenceNbr || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "decrementSequence");
		return 1;
	}

	getMaxSequenceNumber(pErrorList ,pSubsGroup ,strMaxSeqNbr);
	maxSeqNbr = atol(strMaxSeqNbr);

	if(*sequenceNbr > maxSeqNbr || *sequenceNbr < 0){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_INVALID_SEQUENCE_NUMBER,
					 "decrementSequence");
		return 1; 
	}

	if(*sequenceNbr == 0)
		*sequenceNbr = maxSeqNbr;
	else
		(*sequenceNbr)--;
#ifdef DEBUG_IPDRFTP
	printf("Leaving decrementSequence()\n");
#endif
	return 0; 
}

int getHomeServiceProviderType(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int homeServiceProviderType)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering getHomeServiceProviderType()\n");
#endif
	if(!pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getGroupID");
		return 1;
	}

	homeServiceProviderType = pSubsGroup->brRecord_.homeServiceProviderType_;

#ifdef DEBUG_IPDRFTP
	printf("Leaving getHomeServiceProviderType()\n");
#endif
	return 0;

}

int getHomeServiceProvider(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* homeServiceProvider)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering getHomeServiceProvider()\n");
#endif
	if(!homeServiceProvider || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getHomeServiceProvider");
		return 1;
	}

	strcpy(homeServiceProvider,pSubsGroup->brRecord_.homeServiceProvider_);
#ifdef DEBUG_IPDRFTP
	printf("Leaving getHomeServiceProvider()\n");
#endif
	return 0;
}

int getHomeServiceProviderName(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* homeServiceProviderName)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering getHomeServiceProviderName()\n");
#endif
	if(!homeServiceProviderName || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getHomeServiceProviderName");
		return 1;
	}

	strcpy(homeServiceProviderName,pSubsGroup->brRecord_.homeServiceProviderName_);
#ifdef DEBUG_IPDRFTP
	printf("Leaving getHomeServiceProviderName()\n");
#endif
	return 0;
}

int getAccessProviderType(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int accessProviderType)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering getAccessProviderType()\n");
#endif
	if(!pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getGroupID");
		return 1;
	}

	accessProviderType = pSubsGroup->brRecord_.accessProviderType_;

#ifdef DEBUG_IPDRFTP
	printf("Leaving getAccessProviderType()\n");
#endif

	return 0;
}

int getAccessProvider(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* accessProvider)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering getAccessProvider()\n");
#endif
	if(!accessProvider || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getAccessProvider");
		return 1;
	}

	strcpy(accessProvider,pSubsGroup->brRecord_.accessProvider_);
#ifdef DEBUG_IPDRFTP
	printf("Leaving getAccessProvider()\n");
#endif
	return 0;
}

int getAccessProviderName(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* accessProviderName)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering getAccessProviderName()\n");
#endif
	if(!accessProviderName || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getAccessProviderName");
		return 1;
	}

	strcpy(accessProviderName,pSubsGroup->brRecord_.accessProviderName_);
#ifdef DEBUG_IPDRFTP
	printf("Leaving getAccessProviderName()\n");
#endif
	return 0;
}

int getContractReference(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* contractReference)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering getContractReference()\n");
#endif
	if(!contractReference || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getContractReference");
		return 1;
	}

	strcpy(contractReference,pSubsGroup->brRecord_.contractReference_);
#ifdef DEBUG_IPDRFTP
	printf("Leaving getContractReference()\n");
#endif
	return 0;
}

int getBusinessRelationshipID(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* businessRelationshipID)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering getBusinessRelationshipID()\n");
#endif
	if(!businessRelationshipID || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getBusinessRelationshipID");
		return 1;
	}

	strcpy(businessRelationshipID,pSubsGroup->brRecord_.businessRelationshipID_);
#ifdef DEBUG_IPDRFTP
	printf("Leaving getBusinessRelationshipID()\n");
#endif
	return 0;
}

int getSummaryXchgHrs(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int summaryXchgHrs)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering getSummaryXchgHrs()\n");
#endif
	if(!pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getSummaryXchgHrs");
		return 1;
	}

	summaryXchgHrs = pSubsGroup->brRecord_.summaryXchgHrs_;

#ifdef DEBUG_IPDRFTP
	printf("Leaving getSummaryXchgHrs()\n");
#endif
	return 0;
}

int getSettlementXcghDays(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* settlementXcghDays)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering getSettlementXcghDays()\n");
#endif
	if(!settlementXcghDays || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getSettlementXcghDays");
		return 1;
	}

	strcpy(settlementXcghDays,pSubsGroup->brRecord_.settlementXcghDays_);
#ifdef DEBUG_IPDRFTP
	printf("Leaving getSettlementXcghDays()\n");
#endif
	return 0;
}

int getNetPosXchgDays(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* netPosXchgDays)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering getNetPosXchgDays()\n");
#endif
	if(!netPosXchgDays || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getNetPosXchgDays");
		return 1;
	}

	strcpy(netPosXchgDays,pSubsGroup->brRecord_.netPosXchgDays_);
#ifdef DEBUG_IPDRFTP
	printf("Leaving getNetPosXchgDays()\n");
#endif
	return 0;
}

int getEffectiveDateTime(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* effectiveDateTime)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering getEffectiveDateTime()\n");
#endif
	if(!effectiveDateTime || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getEffectiveDateTime");
		return 1;
	}

	strcpy(effectiveDateTime,pSubsGroup->brRecord_.effectiveDateTime_);
#ifdef DEBUG_IPDRFTP
	printf("Leaving getEffectiveDateTime()\n");
#endif
	return 0;
}

int getResponse(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int response)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering getResponse()\n");
#endif
	if(!pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getResponse");
		return 1;
	}

	response = pSubsGroup->brRecord_.response_;

#ifdef DEBUG_IPDRFTP
	printf("Leaving getResponse()\n");
#endif
	return 0;
}

int getXchgProtocol(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int xchgProtocol)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering getXchgProtocol()\n");
#endif
	if(!pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getXchgProtocol");
		return 1;
	}

	xchgProtocol = pSubsGroup->brRecord_.xchgProtocol_;

#ifdef DEBUG_IPDRFTP
	printf("Leaving getXchgProtocol()\n");
#endif
	return 0;
}

int getXchgSLA(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int xchgSLA)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering getXchgSLA()\n");
#endif
	if(!pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getXchgSLA");
		return 1;
	}

	xchgSLA = pSubsGroup->brRecord_.xchgSLA_;

#ifdef DEBUG_IPDRFTP
	printf("Leaving getXchgSLA()\n");
#endif
	return 0;
}

int getXchgEncoding(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int xchgEncoding)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering getXchgEncoding()\n");
#endif
	if(!pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getXchgEncoding");
		return 1;
	}

	xchgEncoding = pSubsGroup->brRecord_.xchgEncoding_;

#ifdef DEBUG_IPDRFTP
	printf("Leaving getXchgEncoding()\n");
#endif
	return 0;
}

int getDisputeResolution(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int disputeResolution)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering getDisputeResolution()\n");
#endif
	if(!pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getDisputeResolution");
		return 1;
	}

	disputeResolution = pSubsGroup->brRecord_.disputeResolution_;

#ifdef DEBUG_IPDRFTP
	printf("Leaving getDisputeResolution()\n");
#endif
	return 0;
}

int getLocationInfo(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* locationInfo)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering getLocationInfo()\n");
#endif
	if(!locationInfo || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getLocationInfo");
		return 1;
	}

	strcpy(locationInfo,pSubsGroup->brRecord_.locationInfo_);
#ifdef DEBUG_IPDRFTP
	printf("Leaving getLocationInfo()\n");
#endif
	return 0;
}

int getBRContactName(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* BRContactName)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering getBRContactName()\n");
#endif
	if(!BRContactName || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getBRContactName");
		return 1;
	}

	strcpy(BRContactName,pSubsGroup->brRecord_.BRContactName_);
#ifdef DEBUG_IPDRFTP
	printf("Leaving getBRContactName()\n");
#endif
	return 0;
}

int getRejectReturn(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int rejectReturn)
{

#ifdef DEBUG_IPDRFTP
	printf("Entering getRejectReturn()\n");
#endif
	if(!pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getRejectReturn");
		return 1;
	}

	rejectReturn = pSubsGroup->brRecord_.rejectReturn_;

#ifdef DEBUG_IPDRFTP
	printf("Leaving getRejectReturn()\n");
#endif
	return 0;
}

int getFrequencyOfAuditReconciliation(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int frequencyOfAuditReconciliation)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering getFrequencyOfAuditReconciliation()\n");
#endif
	if(!pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getFrequencyOfAuditReconciliation");
		return 1;
	}

	frequencyOfAuditReconciliation = pSubsGroup->brRecord_.frequencyOfAuditReconciliation_;

#ifdef DEBUG_IPDRFTP
	printf("Leaving getFrequencyOfAuditReconciliation()\n");
#endif
	return 0;
}

int getXchgCurrencyType(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* xchgCurrencyType)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering getXchgCurrencyType()\n");
#endif
	if(!xchgCurrencyType || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getXchgCurrencyType");
		return 1;
	}

	strcpy(xchgCurrencyType,pSubsGroup->brRecord_.xchgCurrencyType_);
#ifdef DEBUG_IPDRFTP
	printf("Leaving getXchgCurrencyType()\n");
#endif
	return 0;
}

int getUnitOfMeasure(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int unitOfMeasure)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering getUnitOfMeasure()\n");
#endif
	if(!pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getUnitOfMeasure");
		return 1;
	}

	unitOfMeasure = pSubsGroup->brRecord_.unitOfMeasure_;

#ifdef DEBUG_IPDRFTP
	printf("Leaving getUnitOfMeasure()\n");
#endif
	return 0;
}

int getChargeableUnit(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int chargeableUnit)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering getChargeableUnit()\n");
#endif
	if(!pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getChargeableUnit");
		return 1;
	}

	chargeableUnit = pSubsGroup->brRecord_.chargeableUnit_;

#ifdef DEBUG_IPDRFTP
	printf("Leaving getChargeableUnit()\n");
#endif
	return 0;
}

int getRatePerChargeableUnit(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int ratePerChargeableUnit)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering getRatePerChargeableUnit()\n");
#endif
	if(!pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getRatePerChargeableUnit");
		return 1;
	}

	ratePerChargeableUnit = pSubsGroup->brRecord_.ratePerChargeableUnit_;

#ifdef DEBUG_IPDRFTP
	printf("Leaving getRatePerChargeableUnit()\n");
#endif
	return 0;
}

int getClearingIntermediaryName(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* clearingIntermediaryName)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering getClearingIntermediaryName()\n");
#endif
	if(!clearingIntermediaryName || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getClearingIntermediaryName");
		return 1;
	}

	strcpy(clearingIntermediaryName,pSubsGroup->brRecord_.clearingIntermediaryName_);
#ifdef DEBUG_IPDRFTP
	printf("Leaving getClearingIntermediaryName()\n");
#endif
	return 0;
}

int getBillingIntermediaryName(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* billingIntermediaryName)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering getBillingIntermediaryName()\n");
#endif
	if(!billingIntermediaryName || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getBillingIntermediaryName");
		return 1;
	}

	strcpy(billingIntermediaryName,pSubsGroup->brRecord_.billingIntermediaryName_);
#ifdef DEBUG_IPDRFTP
	printf("Leaving getBillingIntermediaryName()\n");
#endif
	return 0;

}

int getSendingServiceProviderDisposition(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int sendingServiceProviderDisposition)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering getSendingServiceProviderDisposition()\n");
#endif
	if(!pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getSendingServiceProviderDisposition");
		return 1;
	}

	sendingServiceProviderDisposition = pSubsGroup->brRecord_.sendingServiceProviderDisposition_;

#ifdef DEBUG_IPDRFTP
	printf("Leaving getSendingServiceProviderDisposition()\n");
#endif
	return 0;
}

int getLastChangedDateTime(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* lastChangedDateTime)	
{
#ifdef DEBUG_IPDRFTP
	printf("Entering getLastChangedDateTime()\n");
#endif
	if(!lastChangedDateTime || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getLastChangedDateTime");
		return 1;
	}

	strcpy(lastChangedDateTime,pSubsGroup->brRecord_.lastChangedDateTime_);
#ifdef DEBUG_IPDRFTP
	printf("Leaving getLastChangedDateTime()\n");
#endif
	return 0;
}

int getFileRetentionDays(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int fileRetentionDays)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering getFileRetentionDays()\n");
#endif
	if(!pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getFileRetentionDays");
		return 1;
	}

	fileRetentionDays = pSubsGroup->brRecord_.fileRetentionDays_;

#ifdef DEBUG_IPDRFTP
	printf("Leaving getFileRetentionDays()\n");
#endif
	return 0;
}

int setHomeServiceProviderType(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int homeServiceProviderType)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering setHomeServiceProviderType()\n");
#endif
	if(!pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "setHomeServiceProviderType");
		return 1;
	}

	pSubsGroup->brRecord_.homeServiceProviderType_ = homeServiceProviderType;

#ifdef DEBUG_IPDRFTP
	printf("Leaving setHomeServiceProviderType()\n");
#endif
	return 0;
}

int setHomeServiceProvider(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* homeServiceProvider)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering setHomeServiceProvider()\n");
#endif
	if(!homeServiceProvider || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "setHomeServiceProvider");
		return 1;
	}
	if(strlen(homeServiceProvider)==0){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_GROUPID_NULL_OR_BLANK,
					 "setHomeServiceProvider");
		return 1;
	}

	/*reallocating memory if required*/
	if(strlen(homeServiceProvider) > strlen(pSubsGroup->brRecord_.homeServiceProvider_))
		pSubsGroup->brRecord_.homeServiceProvider_ = 
			realloc(pSubsGroup->brRecord_.homeServiceProvider_,(strlen(homeServiceProvider)+1));

	strcpy(pSubsGroup->brRecord_.homeServiceProvider_, homeServiceProvider);
#ifdef DEBUG_IPDRFTP
	printf("Leaving setHomeServiceProvider()\n");
#endif
	return 0;
}

int setHomeServiceProviderName(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* homeServiceProviderName)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering setHomeServiceProvider()\n");
#endif
	if(!homeServiceProviderName || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "setHomeServiceProviderName");
		return 1;
	}
	if(strlen(homeServiceProviderName)==0){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_GROUPID_NULL_OR_BLANK,
					 "setHomeServiceProviderName");
		return 1;
	}

	/*reallocating memory if required*/
	if(strlen(homeServiceProviderName) > strlen(pSubsGroup->brRecord_.homeServiceProviderName_))
		pSubsGroup->brRecord_.homeServiceProviderName_ = 
			realloc(pSubsGroup->brRecord_.homeServiceProviderName_,(strlen(homeServiceProviderName)+1));

	strcpy(pSubsGroup->brRecord_.homeServiceProviderName_, homeServiceProviderName);
#ifdef DEBUG_IPDRFTP
	printf("Leaving setHomeServiceProviderName()\n");
#endif
	return 0;
}

int setAccessProviderType(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int accessProviderType)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering setAccessProviderType()\n");
#endif
	if(!pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "setAccessProviderType");
		return 1;
	}

	pSubsGroup->brRecord_.accessProviderType_ = accessProviderType;

#ifdef DEBUG_IPDRFTP
	printf("Leaving setAccessProviderType()\n");
#endif
	return 0;
}

int setAccessProvider(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* accessProvider)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering setAccessProvider()\n");
#endif
	if(!accessProvider || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "setAccessProvider");
		return 1;
	}
	if(strlen(accessProvider)==0){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_GROUPID_NULL_OR_BLANK,
					 "setAccessProvider");
		return 1;
	}

	/*reallocating memory if required*/
	if(strlen(accessProvider) > strlen(pSubsGroup->brRecord_.accessProvider_))
		pSubsGroup->brRecord_.accessProvider_ = 
			realloc(pSubsGroup->brRecord_.accessProvider_,(strlen(accessProvider)+1));

	strcpy(pSubsGroup->brRecord_.accessProvider_, accessProvider);
#ifdef DEBUG_IPDRFTP
	printf("Leaving setAccessProvider()\n");
#endif
	return 0;
}

int setAccessProviderName(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* accessProviderName)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering setAccessProviderName()\n");
#endif
	if(!accessProviderName || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "setAccessProviderName");
		return 1;
	}
	if(strlen(accessProviderName)==0){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_GROUPID_NULL_OR_BLANK,
					 "setAccessProviderName");
		return 1;
	}

	/*reallocating memory if required*/
	if(strlen(accessProviderName) > strlen(pSubsGroup->brRecord_.accessProviderName_))
		pSubsGroup->brRecord_.accessProviderName_ = 
			realloc(pSubsGroup->brRecord_.accessProviderName_,(strlen(accessProviderName)+1));

	strcpy(pSubsGroup->brRecord_.accessProviderName_, accessProviderName);
#ifdef DEBUG_IPDRFTP
	printf("Leaving setAccessProviderName()\n");
#endif
	return 0;

}

int setContractReference(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* contractReference)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering setContractReference()\n");
#endif
	if(!contractReference || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "setContractReference");
		return 1;
	}
	if(strlen(contractReference)==0){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_GROUPID_NULL_OR_BLANK,
					 "setContractReference");
		return 1;
	}

	/*reallocating memory if required*/
	if(strlen(contractReference) > strlen(pSubsGroup->brRecord_.contractReference_))
		pSubsGroup->brRecord_.contractReference_ = 
			realloc(pSubsGroup->brRecord_.contractReference_,(strlen(contractReference)+1));

	strcpy(pSubsGroup->brRecord_.contractReference_, contractReference);
#ifdef DEBUG_IPDRFTP
	printf("Leaving setContractReference()\n");
#endif
	return 0;

}

int setBusinessRelationshipID(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* businessRelationshipID)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering setBusinessRelationshipID()\n");
#endif
	if(!businessRelationshipID || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "setBusinessRelationshipID");
		return 1;
	}
	if(strlen(businessRelationshipID)==0){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_GROUPID_NULL_OR_BLANK,
					 "setBusinessRelationshipID");
		return 1;
	}

	/*reallocating memory if required*/
	if(strlen(businessRelationshipID) > strlen(pSubsGroup->brRecord_.businessRelationshipID_))
		pSubsGroup->brRecord_.businessRelationshipID_ = 
			realloc(pSubsGroup->brRecord_.businessRelationshipID_,(strlen(businessRelationshipID)+1));

	strcpy(pSubsGroup->brRecord_.businessRelationshipID_, businessRelationshipID);
#ifdef DEBUG_IPDRFTP
	printf("Leaving setBusinessRelationshipID()\n");
#endif
	return 0;

}

int setSummaryXchgHrs(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int summaryXchgHrs)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering setSummaryXchgHrs()\n");
#endif
	if(!pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "setSummaryXchgHrs");
		return 1;
	}

	pSubsGroup->brRecord_.summaryXchgHrs_ = summaryXchgHrs;

#ifdef DEBUG_IPDRFTP
	printf("Leaving setSummaryXchgHrs()\n");
#endif
	return 0;
}

int setSettlementXcghDays(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* settlementXcghDays)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering setSettlementXcghDays()\n");
#endif
	if(!settlementXcghDays || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "setSettlementXcghDays");
		return 1;
	}
	if(strlen(settlementXcghDays)==0){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_GROUPID_NULL_OR_BLANK,
					 "setSettlementXcghDays");
		return 1;
	}

	/*reallocating memory if required*/
	if(strlen(settlementXcghDays) > strlen(pSubsGroup->brRecord_.settlementXcghDays_))
		pSubsGroup->brRecord_.settlementXcghDays_ = 
			realloc(pSubsGroup->brRecord_.settlementXcghDays_,(strlen(settlementXcghDays)+1));

	strcpy(pSubsGroup->brRecord_.settlementXcghDays_, settlementXcghDays);
#ifdef DEBUG_IPDRFTP
	printf("Leaving setSettlementXcghDays()\n");
#endif
	return 0;
}

int setNetPosXchgDays(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* netPosXchgDays)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering setNetPosXchgDays()\n");
#endif
	if(!netPosXchgDays || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "setNetPosXchgDays");
		return 1;
	}
	if(strlen(netPosXchgDays)==0){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_GROUPID_NULL_OR_BLANK,
					 "setNetPosXchgDays");
		return 1;
	}

	/*reallocating memory if required*/
	if(strlen(netPosXchgDays) > strlen(pSubsGroup->brRecord_.netPosXchgDays_))
		pSubsGroup->brRecord_.netPosXchgDays_ = 
			realloc(pSubsGroup->brRecord_.netPosXchgDays_,(strlen(netPosXchgDays)+1));

	strcpy(pSubsGroup->brRecord_.netPosXchgDays_, netPosXchgDays);
#ifdef DEBUG_IPDRFTP
	printf("Leaving setNetPosXchgDays()\n");
#endif
	return 0;
}

int setEffectiveDateTime(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* effectiveDateTime)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering setEffectiveDateTime()\n");
#endif
	if(!effectiveDateTime || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "setEffectiveDateTime");
		return 1;
	}
	if(strlen(effectiveDateTime)==0){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_GROUPID_NULL_OR_BLANK,
					 "setEffectiveDateTime");
		return 1;
	}

	/*reallocating memory if required*/
	if(strlen(effectiveDateTime) > strlen(pSubsGroup->brRecord_.effectiveDateTime_))
		pSubsGroup->brRecord_.effectiveDateTime_ = 
			realloc(pSubsGroup->brRecord_.effectiveDateTime_,(strlen(effectiveDateTime)+1));

	strcpy(pSubsGroup->brRecord_.effectiveDateTime_, effectiveDateTime);
#ifdef DEBUG_IPDRFTP
	printf("Leaving setEffectiveDateTime()\n");
#endif
	return 0;
}

int setResponse(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int response)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering setResponse()\n");
#endif
	if(!pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "setResponse");
		return 1;
	}

	pSubsGroup->brRecord_.response_ = response;

#ifdef DEBUG_IPDRFTP
	printf("Leaving setResponse()\n");
#endif
	return 0;
}

int setXchgProtocol(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int xchgProtocol)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering setXchgProtocol()\n");
#endif
	if(!pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "setXchgProtocol");
		return 1;
	}

	pSubsGroup->brRecord_.xchgProtocol_ = xchgProtocol;

#ifdef DEBUG_IPDRFTP
	printf("Leaving setXchgProtocol()\n");
#endif
	return 0;
}

int setXchgSLA(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int xchgSLA)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering setXchgSLA()\n");
#endif
	if(!pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "setXchgSLA");
		return 1;
	}

	pSubsGroup->brRecord_.xchgSLA_ = xchgSLA;

#ifdef DEBUG_IPDRFTP
	printf("Leaving setXchgSLA()\n");
#endif
	return 0;
}

int setXchgEncoding(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int xchgEncoding)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering setXchgEncoding()\n");
#endif
	if(!pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "setXchgEncoding");
		return 1;
	}

	pSubsGroup->brRecord_.xchgEncoding_ = xchgEncoding;

#ifdef DEBUG_IPDRFTP
	printf("Leaving setXchgEncoding()\n");
#endif
	return 0;
}

int setDisputeResolution(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int disputeResolution)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering setDisputeResolution()\n");
#endif
	if(!pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "setDisputeResolution");
		return 1;
	}

	pSubsGroup->brRecord_.disputeResolution_ = disputeResolution;

#ifdef DEBUG_IPDRFTP
	printf("Leaving setDisputeResolution()\n");
#endif
	return 0;
}

int setLocationInfo(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* locationInfo)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering setLocationInfo()\n");
#endif
	if(!locationInfo || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "setLocationInfo");
		return 1;
	}
	if(strlen(locationInfo)==0){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_GROUPID_NULL_OR_BLANK,
					 "setLocationInfo");
		return 1;
	}

	/*reallocating memory if required*/
	if(strlen(locationInfo) > strlen(pSubsGroup->brRecord_.locationInfo_))
		pSubsGroup->brRecord_.locationInfo_ = 
			realloc(pSubsGroup->brRecord_.locationInfo_,(strlen(locationInfo)+1));

	strcpy(pSubsGroup->brRecord_.locationInfo_, locationInfo);
#ifdef DEBUG_IPDRFTP
	printf("Leaving setLocationInfo()\n");
#endif
	return 0;
}


int setBRContactName(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* BRContactName)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering setBRContactName()\n");
#endif
	if(!BRContactName || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "setBRContactName");
		return 1;
	}
	if(strlen(BRContactName)==0){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_GROUPID_NULL_OR_BLANK,
					 "setBRContactName");
		return 1;
	}

	/*reallocating memory if required*/
	if(strlen(BRContactName) > strlen(pSubsGroup->brRecord_.BRContactName_))
		pSubsGroup->brRecord_.BRContactName_ = 
			realloc(pSubsGroup->brRecord_.BRContactName_,(strlen(BRContactName)+1));

	strcpy(pSubsGroup->brRecord_.BRContactName_, BRContactName);
#ifdef DEBUG_IPDRFTP
	printf("Leaving setBRContactName()\n");
#endif
	return 0;
}

int setRejectReturn(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int rejectReturn)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering setRejectReturn()\n");
#endif
	if(!pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "setRejectReturn");
		return 1;
	}

	pSubsGroup->brRecord_.rejectReturn_ = rejectReturn;

#ifdef DEBUG_IPDRFTP
	printf("Leaving setRejectReturn()\n");
#endif
	return 0;
}

int setFrequencyOfAuditReconciliation(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int frequencyOfAuditReconciliation)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering setFrequencyOfAuditReconciliation()\n");
#endif
	if(!pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "setFrequencyOfAuditReconciliation");
		return 1;
	}

	pSubsGroup->brRecord_.frequencyOfAuditReconciliation_ = frequencyOfAuditReconciliation;

#ifdef DEBUG_IPDRFTP
	printf("Leaving setFrequencyOfAuditReconciliation()\n");
#endif
	return 0;
}

int setXchgCurrencyType(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* xchgCurrencyType)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering setXchgCurrencyType()\n");
#endif
	if(!xchgCurrencyType || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "setXchgCurrencyType");
		return 1;
	}
	if(strlen(xchgCurrencyType)==0){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_GROUPID_NULL_OR_BLANK,
					 "setXchgCurrencyType");
		return 1;
	}

	/*reallocating memory if required*/
	if(strlen(xchgCurrencyType) > strlen(pSubsGroup->brRecord_.xchgCurrencyType_))
		pSubsGroup->brRecord_.xchgCurrencyType_ = 
			realloc(pSubsGroup->brRecord_.xchgCurrencyType_,(strlen(xchgCurrencyType)+1));

	strcpy(pSubsGroup->brRecord_.xchgCurrencyType_, xchgCurrencyType);
#ifdef DEBUG_IPDRFTP
	printf("Leaving setXchgCurrencyType()\n");
#endif
	return 0;
}

int setUnitOfMeasure(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int unitOfMeasure)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering setUnitOfMeasure()\n");
#endif
	if(!pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "setUnitOfMeasure");
		return 1;
	}

	pSubsGroup->brRecord_.unitOfMeasure_ = unitOfMeasure;

#ifdef DEBUG_IPDRFTP
	printf("Leaving setUnitOfMeasure()\n");
#endif
	return 0;
}

int setChargeableUnit(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int chargeableUnit)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering setChargeableUnit()\n");
#endif
	if(!pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "setChargeableUnit");
		return 1;
	}

	pSubsGroup->brRecord_.chargeableUnit_ = chargeableUnit;

#ifdef DEBUG_IPDRFTP
	printf("Leaving setChargeableUnit()\n");
#endif
	return 0;
}

int setRatePerChargeableUnit(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int ratePerChargeableUnit)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering setRatePerChargeableUnit()\n");
#endif
	if(!pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "setRatePerChargeableUnit");
		return 1;
	}

	pSubsGroup->brRecord_.ratePerChargeableUnit_ = ratePerChargeableUnit;

#ifdef DEBUG_IPDRFTP
	printf("Leaving setRatePerChargeableUnit()\n");
#endif
	return 0;
}

int setClearingIntermediaryName(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* clearingIntermediaryName)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering setClearingIntermediaryName()\n");
#endif
	if(!clearingIntermediaryName || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "setClearingIntermediaryName");
		return 1;
	}
	if(strlen(clearingIntermediaryName)==0){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_GROUPID_NULL_OR_BLANK,
					 "setClearingIntermediaryName");
		return 1;
	}

	/*reallocating memory if required*/
	if(strlen(clearingIntermediaryName) > strlen(pSubsGroup->brRecord_.clearingIntermediaryName_))
		pSubsGroup->brRecord_.clearingIntermediaryName_ = 
			realloc(pSubsGroup->brRecord_.clearingIntermediaryName_,(strlen(clearingIntermediaryName)+1));

	strcpy(pSubsGroup->brRecord_.clearingIntermediaryName_, clearingIntermediaryName);
#ifdef DEBUG_IPDRFTP
	printf("Leaving setClearingIntermediaryName()\n");
#endif
	return 0;
}

int setBillingIntermediaryName(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* billingIntermediaryName)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering setBillingIntermediaryName()\n");
#endif
	if(!billingIntermediaryName || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "setBillingIntermediaryName");
		return 1;
	}
	if(strlen(billingIntermediaryName)==0){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_GROUPID_NULL_OR_BLANK,
					 "setBillingIntermediaryName");
		return 1;
	}

	/*reallocating memory if required*/
	if(strlen(billingIntermediaryName) > strlen(pSubsGroup->brRecord_.billingIntermediaryName_))
		pSubsGroup->brRecord_.billingIntermediaryName_ = 
			realloc(pSubsGroup->brRecord_.billingIntermediaryName_,(strlen(billingIntermediaryName)+1));

	strcpy(pSubsGroup->brRecord_.billingIntermediaryName_, billingIntermediaryName);
#ifdef DEBUG_IPDRFTP
	printf("Leaving setBillingIntermediaryName()\n");
#endif
	return 0;
}

int setSendingServiceProviderDisposition(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int sendingServiceProviderDisposition)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering setSendingServiceProviderDisposition()\n");
#endif
	if(!pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "setSendingServiceProviderDisposition");
		return 1;
	}

	pSubsGroup->brRecord_.sendingServiceProviderDisposition_ = sendingServiceProviderDisposition;

#ifdef DEBUG_IPDRFTP
	printf("Leaving setSendingServiceProviderDisposition()\n");
#endif
	return 0;
}

int setLastChangedDateTime(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* lastChangedDateTime)	
{
#ifdef DEBUG_IPDRFTP
	printf("Entering setLastChangedDateTime()\n");
#endif
	if(!lastChangedDateTime || !pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "setLastChangedDateTime");
		return 1;
	}
	if(strlen(lastChangedDateTime)==0){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_GROUPID_NULL_OR_BLANK,
					 "setLastChangedDateTime");
		return 1;
	}

	/*reallocating memory if required*/
	if(strlen(lastChangedDateTime) > strlen(pSubsGroup->brRecord_.lastChangedDateTime_))
		pSubsGroup->brRecord_.lastChangedDateTime_ = 
			realloc(pSubsGroup->brRecord_.lastChangedDateTime_,(strlen(lastChangedDateTime)+1));

	strcpy(pSubsGroup->brRecord_.lastChangedDateTime_, lastChangedDateTime);
#ifdef DEBUG_IPDRFTP
	printf("Leaving setLastChangedDateTime()\n");
#endif
	return 0;
}

int setFileRetentionDays(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int fileRetentionDays)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering setFileRetentionDays()\n");
#endif
	if(!pSubsGroup){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "setFileRetentionDays");
		return 1;
	}

	pSubsGroup->brRecord_.fileRetentionDays_ = fileRetentionDays;

#ifdef DEBUG_IPDRFTP
	printf("Leaving setFileRetentionDays()\n");
#endif
	return 0;
}
