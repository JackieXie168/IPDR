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
 * File                 : CapabilityFile.c			         
 * Description          : Contains all the CapabilityFile      
 *                        related functions      
 * Author               : Infosys Tech Ltd           
 * Modification History :                            
 *---------------------------------------------------
 * Date       Name             Change/Description    
 *---------------------------------------------------
 * 01/29/02  Infosys           Created                        
 *
 * 
 */


#include "ftp/CapabilityFile.H"
#include "ftp/ErrorCodes.H"

//int depth;


/*!
 *  NAME:
 *      newCapabilityFile() - creates instance of type (CapabilityFile*)
 *                                            
 *
 *  DESCRIPTION: 1.The function allocates memory for CapabilityFile variable.
 *				 2.It assigns its member variables to NULL.				
 *		
 *  
 *  ARGUMENTS:
 *       -  pErrorList (IN / OUT)
 *       -	capabilityFileURL (IN) 
 *        
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
CapabilityFile* newCapabilityFile(ListErrorStruct* pErrorList)
{
	CapabilityFile* pCapabilityFile;

#ifdef DEBUG_IPDRFTP
	printf("Entering newCapabilityFile()\n");
#endif

    pCapabilityFile = (CapabilityFile*)malloc(sizeof(CapabilityFile));
	if(!pCapabilityFile){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_MALLOC_FAILED,
					 "newCapabilityFile");
		return NULL;
	}
	/*Assigning members of pCapabilityFile to NULL*/
	pCapabilityFile->subscriptionGroupList_ = NULL;
	pCapabilityFile->capabilityFileURL_ = NULL;

#ifdef DEBUG_IPDRFTP
	printf("Leaving newCapabilityFile()\n");
#endif

	return(pCapabilityFile);
}

/*!
 *  NAME:
 *      freeCapabilityFile() - free memory for passed variable.
 *                                            
 *
 *  DESCRIPTION: The function frees memory for CapabilityFile pointer 
 *               variable.
 *       
 *		
 *  ARGUMENTS:
 *       -	pCapabilityFile (IN / OUT) 
 *        
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int freeCapabilityFile(CapabilityFile* pCapabilityFile)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering freeCapabilityFile()\n");
#endif

	if(!pCapabilityFile)
		return 0;
	if(pCapabilityFile->capabilityFileURL_)
		free(pCapabilityFile->capabilityFileURL_);
	/*free subscription group list*/
	if(pCapabilityFile->subscriptionGroupList_)
		freeSubscriptionGroupList(pCapabilityFile->subscriptionGroupList_);
	free(pCapabilityFile);
	pCapabilityFile = NULL;

#ifdef DEBUG_IPDRFTP
	printf("Leaving freeCapabilityFile()\n");
#endif

	return 0;
}


 /*!
 *  NAME:
 *      addGroup() - Adds a pSubscriptionGroup to SubscriptionGroupList
 *                                            
 *
 *  DESCRIPTION: 1.The function checks for validity of subscriptionGroup to be added.
 *               2.Creates a new node for SubscriptionGroupList.
 *               3.Appends the new node to the capabilityFile subscriptionGroupList.
 *  
 *  ARGUMENTS:
 *       -	pErrorList (IN / OUT)
 *       -  pCapabilityFile (IN / OUT)
 *       -  pSubscriptionGroup (IN)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int addGroup(ListErrorStruct* pErrorList,
			 CapabilityFile* pCapabilityFile,  
        	 SubscriptionGroup* pSubscriptionGroup) 
{
	/*used to allocate memory for new node*/
	SubscriptionGroupList* pCurrent;
	/*used to traverse through the subscription group list.*/
	SubscriptionGroupList* pTemp = NULL;
	int errorCode;

#ifdef DEBUG_IPDRFTP
	printf("Entering addGroup()\n");
#endif

	if(!pSubscriptionGroup || !pCapabilityFile){/*Error handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "addGroup");
		return 1;
	}

	/*GroupId validation*/
	if((errorCode = isGroupIdUnique(pCapabilityFile->subscriptionGroupList_,
					pSubscriptionGroup->groupID_))!=0){
		ftpErrorHandler(pErrorList,
			         errorCode,
					 "addGroup");
		return 1;
	}

	pCurrent = newSubscriptionGroupList(pErrorList, pSubscriptionGroup);
	if(!pCurrent){/*Error handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_COULDNT_CREATE_NEWNODE,
					 "addGroup");
		return 1;
	}
	/*append new node to subscriptionGroupList_ of pCapabilityFile*/
	if(!pCapabilityFile->subscriptionGroupList_)
		pCapabilityFile->subscriptionGroupList_ = pCurrent;
	else{
		pTemp = pCapabilityFile->subscriptionGroupList_;
		for( ;pTemp->nextSubscriptionGroup_; )
			pTemp = pTemp->nextSubscriptionGroup_;
		pTemp->nextSubscriptionGroup_ = pCurrent;
	}

#ifdef DEBUG_IPDRFTP
	printf("Leaving addGroup()\n");
#endif

	return 0;
}

/*!
 *  NAME:
 *      newSubscriptionGroupList() - creates structure of type
 *                             (SubscriptionGroupList*).
 *                                            
 *
 *  DESCRIPTION:1.Checks validity of input parameters.
 *              2.Creates a new node and assigns values to it.
 *		
 *  ARGUMENTS:
 *       -  pErrorList (IN / OUT)
 *       -	pSubscriptionGroup (IN / OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
SubscriptionGroupList*
newSubscriptionGroupList(ListErrorStruct* pErrorList,
						 SubscriptionGroup* pSubscriptionGroup)
{
	/*represents new subscription group list node.*/
	SubscriptionGroupList* pSubsGroupList;
	int errorCode = 0;
#ifdef DEBUG_IPDRFTP
	printf("Entering newSubscriptionGroupList()\n");
#endif
	if(!pSubscriptionGroup){/*Error handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "newSubscriptionGroupList");
		return NULL;
	}
	/*validate input subscription group*/
	if((errorCode = validateSubscriptionGroup(*pSubscriptionGroup))!=0){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 errorCode,
					 "newSubscriptionGroupList");
		return NULL;
	}
	/*Allocate memory for subscription group list node.*/
	pSubsGroupList = (SubscriptionGroupList*)malloc(sizeof(SubscriptionGroupList));
	if(!pSubsGroupList){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_MALLOC_FAILED,
					 "newSubscriptionGroupList");
		return NULL;
	}

	pSubsGroupList->nextSubscriptionGroup_ = NULL;
	/*Allocating memory for pSubsGroupList members*/
	pSubsGroupList->subscriptionGroup_.groupID_ =
		(char*)malloc(strlen(pSubscriptionGroup->groupID_)+1);
	pSubsGroupList->subscriptionGroup_.controlFileSuffix_ =
		(char*)malloc(strlen(pSubscriptionGroup->controlFileSuffix_)+1);
	pSubsGroupList->subscriptionGroup_.controlFilePrefix_ =
		(char*)malloc(strlen(pSubscriptionGroup->controlFilePrefix_)+1);
	pSubsGroupList->subscriptionGroup_.controlFileNamePolicy_ =
		(char*)malloc(strlen(pSubscriptionGroup->controlFileNamePolicy_)+1);
	pSubsGroupList->subscriptionGroup_.controlFileDirectory_ =
		(char*)malloc(strlen(pSubscriptionGroup->controlFileDirectory_)+1);

	/*checking malloc failure*/
	if(!pSubsGroupList->subscriptionGroup_.groupID_ ||
		!pSubsGroupList->subscriptionGroup_.controlFileSuffix_ ||
		!pSubsGroupList->subscriptionGroup_.controlFilePrefix_ ||
		!pSubsGroupList->subscriptionGroup_.controlFileNamePolicy_||
		!pSubsGroupList->subscriptionGroup_.controlFileDirectory_){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_MALLOC_FAILED,
					 "newSubscriptionGroupList");
		free (pSubsGroupList);
		return NULL; 
	}
	/*Polulating pSubsGroupList->subscriptionGroup_ members*/
	strcpy(pSubsGroupList->subscriptionGroup_.groupID_ ,
		pSubscriptionGroup->groupID_);
	strcpy(pSubsGroupList->subscriptionGroup_.controlFileSuffix_ ,
		pSubscriptionGroup->controlFileSuffix_);
	strcpy(pSubsGroupList->subscriptionGroup_.controlFilePrefix_ ,
		pSubscriptionGroup->controlFilePrefix_);
	strcpy(pSubsGroupList->subscriptionGroup_.controlFileNamePolicy_,
		pSubscriptionGroup->controlFileNamePolicy_);	
	strcpy(pSubsGroupList->subscriptionGroup_.controlFileDirectory_,
		pSubscriptionGroup->controlFileDirectory_);
	
	/*optional controlFileDirectoryURL member*/
	if(pSubscriptionGroup->controlFileLocalDirectory_){
		pSubsGroupList->subscriptionGroup_.controlFileLocalDirectory_ =
			(char*)malloc(strlen(pSubscriptionGroup->controlFileLocalDirectory_)+1);
		strcpy(pSubsGroupList->subscriptionGroup_.controlFileLocalDirectory_,
			pSubscriptionGroup->controlFileLocalDirectory_);
	}
	else 
		pSubsGroupList->subscriptionGroup_.controlFileLocalDirectory_ = NULL;
#ifdef DEBUG_IPDRFTP
	printf("Leaving newSubscriptionGroupList()\n");
#endif
	return pSubsGroupList;
}

/*!
 *  NAME:
 *      freeSubscriptionGroupList() - free memory for SubscriptionGroupList.
 *                                            
 *
 *  DESCRIPTION: The function frees memory for SubscriptionGroupList node by node.
 *       
 *		
 *  ARGUMENTS:
 *       -	pCapabilityFile (IN / OUT) 
 *        
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int freeSubscriptionGroupList(SubscriptionGroupList* subsGroupList)
{
	SubscriptionGroupList* tempSubsGroupList = NULL;
#ifdef DEBUG_IPDRFTP
	printf("Entering freeSubscriptionGroupList()\n");
#endif
	/*free memory of all the nodes of subscription group linked list.*/
	for(;subsGroupList;){
		tempSubsGroupList = subsGroupList->nextSubscriptionGroup_;
		free(subsGroupList);
		subsGroupList = tempSubsGroupList;
	}
#ifdef DEBUG_IPDRFTP
	printf("Leaving freeSubscriptionGroupList()\n");
#endif
	return 0;
}

/*!
 *  NAME:
 *      deleteGroup() - delete SubscriptionGroup from linked list.
 *                                            
 *
 *  DESCRIPTION:1.Returns error if the subscription group list is empty.
 *              2.search for the group to be deleted.
 *              3.If a match is found then the group is deleted else an error is returned.
 *		  
 *  ARGUMENTS:
 *       -	pErrorList (IN / OUT)
 *       -  pCapabilityFile (IN / OUT)
 *       -  subscriptionGroupID (IN)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int deleteGroup(ListErrorStruct* pErrorList,
				CapabilityFile* pCapabilityFile ,
				char* subscriptionGroupID)
{
	/*pCurrent will be the node to be deleted.*/
	SubscriptionGroupList* pCurrent = NULL;
	/*pPrevious will be the node previous to pCurrent*/
	SubscriptionGroupList* pPrevious = NULL;
#ifdef DEBUG_IPDRFTP
	printf("Entering deleteGroup()\n");
#endif
	if(!subscriptionGroupID || !pCapabilityFile){/*Error handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "deleteGroup");
		return 1;
	}
	/*if subscription group list is empty*/
	if(pCapabilityFile->subscriptionGroupList_==NULL){/*Error handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_SUBSGROUP_LIST_EMPTY,
					 "deleteGroup");
		return 1;
	}
	/*search the node to be deleted*/
	pCurrent = pCapabilityFile->subscriptionGroupList_;
	for( ;pCurrent && 
		  strcmp(pCurrent->subscriptionGroup_.groupID_ ,
		  subscriptionGroupID); ){
		pPrevious = pCurrent;
		pCurrent = pCurrent->nextSubscriptionGroup_;
	}
	/*match not found*/
	if(!pCurrent){/*Error handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_SUBSGROUP_NOT_FOUND,
					 "deleteGroup");
		return 1;
	}

	/*Match Found and delete the node.*/
	if(pCurrent == pCapabilityFile->subscriptionGroupList_){
		if(!pCurrent->nextSubscriptionGroup_)
			pCapabilityFile->subscriptionGroupList_ = NULL;
		else
			pCapabilityFile->subscriptionGroupList_ = 
			                    pCurrent->nextSubscriptionGroup_;
	}
	else{
		if(!pCurrent->nextSubscriptionGroup_)
			pPrevious->nextSubscriptionGroup_ = NULL;
		else
			pPrevious->nextSubscriptionGroup_ = 
			              pCurrent->nextSubscriptionGroup_;
	}
	free(pCurrent);/*to free we can have a method*/
#ifdef DEBUG_IPDRFTP
	printf("Leaving deleteGroup()\n");
#endif
	return 0;  /*deleted successfully*/
}

/*!
 *  NAME:
 *      getAllGroups() - gives beginning node of the 
 *                  SubscriptionGroupList to the calling application.
 *                                            
 *
 *  DESCRIPTION: 1.This function gives the pointer to the first subscription
 *                 group.
 *		         2.If the list is empty it returns an error. 
 *		
 *  
 *  ARGUMENTS:
 *       -	pErrorList (IN / OUT)
 *       -  pCapabilityFile (IN)
 *       -  pSubscriptionGroupList (IN / OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int getAllGroups(ListErrorStruct* pErrorList,
				 CapabilityFile* pCapabilityFile,
				 SubscriptionGroupList* pSubscriptionGroupList)
{
#ifdef DEBUG_IPDRFTP
	printf("Entering getAllGroups()\n");
#endif
	if(!pSubscriptionGroupList || !pCapabilityFile){/*Error handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getAllGroups");
		return 1;
	}
	/*get all groups*/
	if(pCapabilityFile->subscriptionGroupList_){
		*pSubscriptionGroupList = *(pCapabilityFile->subscriptionGroupList_);
#ifdef DEBUG_IPDRFTP
	printf("Leaving getAllGroups()\n");
#endif
		return 0; /*linklist is not empty*/
	}
	
	else{/*list is empty*/
		ftpErrorHandler(pErrorList,/*Error handling*/
					 IPDR_SUBSGROUP_LIST_EMPTY,
					 "getAllGroups");
		return 1;
	}
}

/*!
 *  NAME:
 *      getGroup() - gives the calling application pointer to the 
 *              group that matches groupID.
 *                                            
 *
 *  DESCRIPTION:1.The function searches for groupId in the subscriptionGroup list.
 *              2.It returns error if the list is empty or match is not found for 
 *		          passed groupId. 
 *  
 *  ARGUMENTS:
 *       -	pErrorList (IN / OUT)
 *       -  pCapabilityFile (IN)
 *       -  groupID (IN)
 *       -  pSubscriptionGroup (IN / OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int getGroup(ListErrorStruct* pErrorList ,
			 CapabilityFile* pCapabilityFile, 
			 char* groupID, 
			 SubscriptionGroup** pSubscriptionGroup)
{
	/*It will point to the subscription that matches groupID*/
	SubscriptionGroupList* pCurrent  = NULL;
#ifdef DEBUG_IPDRFTP
	printf("Entering getGroup()\n");
#endif
	if(!pCapabilityFile || !pSubscriptionGroup || !groupID){/*Error handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getAllGroups");
		*pSubscriptionGroup = NULL;
		return 1;
	}
	/*subscription group list is empty*/
	if(!pCapabilityFile->subscriptionGroupList_){/*Error handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_SUBSGROUP_LIST_EMPTY,
					 "getGroup");
		*pSubscriptionGroup = NULL;
		return 1;
	}

	pCurrent= pCapabilityFile->subscriptionGroupList_;

	/*search for the subscription group*/
	for( ;pCurrent && 
		  (strcmp(pCurrent->subscriptionGroup_.groupID_,
		  groupID)!=0); ){
		pCurrent =   pCurrent->nextSubscriptionGroup_;
	}

	if(pCurrent){/*subscription group found*/
		*pSubscriptionGroup  = &(pCurrent->subscriptionGroup_);
#ifdef DEBUG_IPDRFTP
	printf("Leaving getGroup()\n");
#endif
		return 0;
	}
	else{/*subscription group not found*/
		ftpErrorHandler(pErrorList,/*Error handling*/
					 IPDR_SUBSGROUP_NOT_FOUND,
					 "getGroup");
		*pSubscriptionGroup = NULL;
		return 1;
	}
}

/*!
 *  NAME:
 *      getGroupIDs() - creates and returns a linked list of all pGroupIDs.
 *                                            
 *
 *  DESCRIPTION:1.This function creates a linked list of groupIds.
 *              2.Assigns the linked list to input parameter.
 *		
 *		
 *  
 *  ARGUMENTS:
 *       -	pErrorList (IN / OUT)
 *       -  pCapabilityFile (IN) 
 *       -  pGroupIDs (IN / OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int getGroupIDs(ListErrorStruct* pErrorList ,
				CapabilityFile* pCapabilityFile,
				ListGroupID* pGroupIDs)	
{
	/*used to traverse the subscription group list.*/
	SubscriptionGroupList* pCurrent;
	/*Points to the groupID list.*/
	ListGroupID* pFirstListGroupID = NULL;
#ifdef DEBUG_IPDRFTP
	printf("Entering getGroupIDs()\n");
#endif
	if(!pCapabilityFile || !pGroupIDs){/*Error handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_FUNCTION_PARAMETER_NULL,
					 "getAllGroups");
		return 1;
	}

	if(!pCapabilityFile->subscriptionGroupList_){/*Error handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_SUBSGROUP_LIST_EMPTY,
					 "getGroupIDs");
		return 1;
	}
	
	pCurrent = pCapabilityFile->subscriptionGroupList_;

	/*create groupID's linked list*/
	for( ;pCurrent; ){
		createListGroupID(&pFirstListGroupID,
			              pCurrent->subscriptionGroup_.groupID_);
		pCurrent = pCurrent->nextSubscriptionGroup_;
	}

	/*assign linked list to funcrion argument.*/
	if(pFirstListGroupID){
		*pGroupIDs = *pFirstListGroupID;
		free(pFirstListGroupID);//to remove memory leak.
	}
	else{/*list is empty*/
		ftpErrorHandler(pErrorList,/*Error handling*/
					 IPDR_SUBSGROUP_LIST_EMPTY,
					 "getGroupIDs");
		return 1;
	}

#ifdef DEBUG_IPDRFTP
	printf("Leaving getGroupIDs()\n");
#endif
	return 0;
}

/*!
 *  NAME:
 *      createListGroupID() - creates linked list of pGroupIDs
 *                                            
 *
 *  DESCRIPTION:1.It is called by getGroupIDs().
 *              2.It creates a linked list of GroupIds. 
 *		
 *		
 *  
 *  ARGUMENTS:
 *       -	pFirstListGroupID (IN / OUT)
 *       -  groupID (IN)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int createListGroupID(ListGroupID** pFirstListGroupID, 
					  char* groupID)  
{
	/*used to allocate memory for new groupID's list node*/
	ListGroupID* pCurrent;
	/*used to traverse the groupID's list*/
	ListGroupID* pTemp = NULL;;
#ifdef DEBUG_IPDRFTP
	printf("Entering createListGroupID()\n");
#endif
	/*allocate memory for new groupId list node and populate it.*/
	pCurrent = (ListGroupID*)malloc(sizeof(ListGroupID));
	pCurrent->groupID_ = (char*)malloc(strlen(groupID)+1);
	strcpy(pCurrent->groupID_,groupID);
	pCurrent->nextGroupID_ = NULL;

	/*append new groupId list node to groupID's linked list.*/
	if(!*pFirstListGroupID)
		*pFirstListGroupID = pCurrent;
	else{
		pTemp = *pFirstListGroupID;
		for( ;pTemp->nextGroupID_; )
			pTemp = pTemp->nextGroupID_;
		pTemp->nextGroupID_ = pCurrent;
	}
#ifdef DEBUG_IPDRFTP
	printf("Leaving createListGroupID()\n");
#endif
	return 0;
}

/*!
 *  NAME:
 *      toXML() - generates xml string.
 *                                            
 *
 *  DESCRIPTION:1.This function populates the input XMLString with string
 *                which is in xml format.
 *		        2.It copies the SubscriptionGroup List information into the string 
 *		          in XML format.
 *  
 *  ARGUMENTS:
 *       -	pErrorList (IN / OUT)
 *       -  pCapabilityFile (IN / OUT)
 *       -  XMLString (IN)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int toXML(ListErrorStruct* pErrorList ,
		  CapabilityFile* pCapabilityFile,
		  char* XMLString)
{
	/*Used to copy temporary strings to be concatenated to XMLString.*/
	char tempXMLString[300];
	/*Used to traverse the subscription group list*/
	/*and point to the current subscription group*/
	SubscriptionGroupList* pCurrent = NULL;
#ifdef DEBUG_IPDRFTP
	printf("Entering toXML()\n");
#endif
	if(!pCapabilityFile || !XMLString){/*error handling*/
		ftpErrorHandler(pErrorList,
			         IPDR_FUNCTION_PARAMETER_NULL,
					 "toXML");
		return 1;
	}

	pCurrent = pCapabilityFile->subscriptionGroupList_;

	/*Copy intial xml information into xml string*/
	strcpy(XMLString,"<CapabilityRsp>\n<supportedProtocolList>\n");
    strcat(XMLString,
		"<supportedProtocolItem version=\"3.0\" protocolMapping=\"File\" encoding=\"XML\">\n");
	strcat(XMLString,
		"<primitiveList>\n<primitiveItem>Pull</primitiveItem>\n</primitiveList>\n");
	strcat(XMLString,"<extension>\n");
	strcat(XMLString,"<groupInfoList>\n");

	/*Traverse the SubscriptionGroup List and fill the XMLString*/
	for( ;pCurrent; ){
		sprintf(tempXMLString ,"<%s>\n",ELEMENT_GROUPINFOITEM);
		strcat(XMLString ,tempXMLString);

		sprintf(tempXMLString,"<%s>%s</%s>\n", 
			ELEMENT_GROUPID,pCurrent->subscriptionGroup_.groupID_,
			ELEMENT_GROUPID);
		strcat(XMLString,tempXMLString);

		sprintf(tempXMLString,"<%s>%s</%s>\n", 
			ELEMENT_CTRLDIR,pCurrent->subscriptionGroup_.controlFileDirectory_,
			ELEMENT_CTRLDIR);
		strcat(XMLString ,tempXMLString);

		sprintf(tempXMLString,"<%s>%s</%s>\n", 
			ELEMENT_CTRLPREFIX,pCurrent->subscriptionGroup_.controlFilePrefix_,
			ELEMENT_CTRLPREFIX);
		strcat(XMLString ,tempXMLString);

		sprintf(tempXMLString,"<%s>%s</%s>\n", 
			ELEMENT_CTRLNAMEPOLICY,pCurrent->subscriptionGroup_.controlFileNamePolicy_,
			ELEMENT_CTRLNAMEPOLICY);
		strcat(XMLString ,tempXMLString);

		sprintf(tempXMLString,"<%s>%s</%s>\n", 
			ELEMENT_CTRLSUFFIX,pCurrent->subscriptionGroup_.controlFileSuffix_,
			ELEMENT_CTRLSUFFIX);
		strcat(XMLString ,tempXMLString);


		if(pCurrent->subscriptionGroup_.brRecordFlag_ == 1) {
			sprintf(tempXMLString ,"<%s>\n",ELEMENT_BRRECORD);
			strcat(XMLString ,tempXMLString);
	
			sprintf(tempXMLString,"<%s>%d</%s>\n", 
				ELEMENT_HOMESERVICEPROVIDERTYPE,pCurrent->subscriptionGroup_.brRecord_.homeServiceProviderType_,
				ELEMENT_HOMESERVICEPROVIDERTYPE);
			strcat(XMLString ,tempXMLString);

			sprintf(tempXMLString,"<%s>%s</%s>\n", 
				ELEMENT_HOMESERVICEPROVIDER,pCurrent->subscriptionGroup_.brRecord_.homeServiceProvider_,
				ELEMENT_HOMESERVICEPROVIDER);
			strcat(XMLString ,tempXMLString);

			sprintf(tempXMLString,"<%s>%s</%s>\n", 
				ELEMENT_HOMESERVICEPROVIDERNAME,pCurrent->subscriptionGroup_.brRecord_.homeServiceProviderName_,
				ELEMENT_HOMESERVICEPROVIDERNAME);
			strcat(XMLString ,tempXMLString);

			sprintf(tempXMLString,"<%s>%d</%s>\n", 
				ELEMENT_ACCESSPROVIDERTYPE,pCurrent->subscriptionGroup_.brRecord_.accessProviderType_,
				ELEMENT_ACCESSPROVIDERTYPE);
			strcat(XMLString ,tempXMLString);

			sprintf(tempXMLString,"<%s>%s</%s>\n", 
				ELEMENT_ACCESSPROVIDER,pCurrent->subscriptionGroup_.brRecord_.accessProvider_,
				ELEMENT_ACCESSPROVIDER);
			strcat(XMLString ,tempXMLString);

			sprintf(tempXMLString,"<%s>%s</%s>\n", 
				ELEMENT_ACCESSPROVIDERNAME,pCurrent->subscriptionGroup_.brRecord_.accessProviderName_,
				ELEMENT_ACCESSPROVIDERNAME);
			strcat(XMLString ,tempXMLString);

			sprintf(tempXMLString,"<%s>%s</%s>\n", 
				ELEMENT_CONTRACTREFERENCE,pCurrent->subscriptionGroup_.brRecord_.contractReference_,
				ELEMENT_CONTRACTREFERENCE);
			strcat(XMLString ,tempXMLString);

			sprintf(tempXMLString,"<%s>%s</%s>\n", 
				ELEMENT_BUSINESSRELATIONSHIPID,pCurrent->subscriptionGroup_.brRecord_.businessRelationshipID_,
				ELEMENT_BUSINESSRELATIONSHIPID);
			strcat(XMLString ,tempXMLString);

			sprintf(tempXMLString,"<%s>%d</%s>\n", 
				ELEMENT_SUMMARYXCHGHRS,pCurrent->subscriptionGroup_.brRecord_.summaryXchgHrs_,
				ELEMENT_SUMMARYXCHGHRS);
			strcat(XMLString ,tempXMLString);

			sprintf(tempXMLString,"<%s>%s</%s>\n", 
				ELEMENT_SETTLEMENTXCGHDAYS,pCurrent->subscriptionGroup_.brRecord_.settlementXcghDays_,
				ELEMENT_SETTLEMENTXCGHDAYS);
			strcat(XMLString ,tempXMLString);

			sprintf(tempXMLString,"<%s>%s</%s>\n", 
				ELEMENT_NETPOSXCHGDAYS,pCurrent->subscriptionGroup_.brRecord_.netPosXchgDays_,
				ELEMENT_NETPOSXCHGDAYS);
			strcat(XMLString ,tempXMLString);

			sprintf(tempXMLString,"<%s>%s</%s>\n", 
				ELEMENT_EFFECTIVEDATETIME,pCurrent->subscriptionGroup_.brRecord_.effectiveDateTime_,
				ELEMENT_EFFECTIVEDATETIME);
			strcat(XMLString ,tempXMLString);

			sprintf(tempXMLString,"<%s>%d</%s>\n", 
				ELEMENT_RESPONSE,pCurrent->subscriptionGroup_.brRecord_.response_,
				ELEMENT_RESPONSE);
			strcat(XMLString ,tempXMLString);

			sprintf(tempXMLString,"<%s>%d</%s>\n", 
				ELEMENT_XCHGPROTOCOL,pCurrent->subscriptionGroup_.brRecord_.xchgProtocol_,
				ELEMENT_XCHGPROTOCOL);
			strcat(XMLString ,tempXMLString);

			sprintf(tempXMLString,"<%s>%d</%s>\n", 
				ELEMENT_XCHGSLA,pCurrent->subscriptionGroup_.brRecord_.xchgSLA_,
				ELEMENT_XCHGSLA);
			strcat(XMLString ,tempXMLString);

			sprintf(tempXMLString,"<%s>%d</%s>\n", 
				ELEMENT_XCHGENCODING,pCurrent->subscriptionGroup_.brRecord_.xchgEncoding_,
				ELEMENT_XCHGENCODING);
			strcat(XMLString ,tempXMLString);

			sprintf(tempXMLString,"<%s>%d</%s>\n", 
				ELEMENT_DISPUTERESOLUTION,pCurrent->subscriptionGroup_.brRecord_.disputeResolution_,
				ELEMENT_DISPUTERESOLUTION);
			strcat(XMLString ,tempXMLString);

			sprintf(tempXMLString,"<%s>%s</%s>\n", 
				ELEMENT_BRCONTACTNAME,pCurrent->subscriptionGroup_.brRecord_.BRContactName_,
				ELEMENT_BRCONTACTNAME);
			strcat(XMLString ,tempXMLString);

			sprintf(tempXMLString,"<%s>%d</%s>\n", 
				ELEMENT_REJECTRETURN,pCurrent->subscriptionGroup_.brRecord_.rejectReturn_,
				ELEMENT_REJECTRETURN);
			strcat(XMLString ,tempXMLString);

		sprintf(tempXMLString,"<%s>%d</%s>\n", 
		ELEMENT_FREQUENCYOFAUDITRECONCILIATION,pCurrent->subscriptionGroup_.brRecord_.frequencyOfAuditReconciliation_,
				ELEMENT_FREQUENCYOFAUDITRECONCILIATION);
			strcat(XMLString ,tempXMLString);

			sprintf(tempXMLString,"<%s>%s</%s>\n", 
				ELEMENT_XCHGCURRENCYTYPE,pCurrent->subscriptionGroup_.brRecord_.xchgCurrencyType_,
				ELEMENT_XCHGCURRENCYTYPE);
			strcat(XMLString ,tempXMLString);

			sprintf(tempXMLString,"<%s>%d</%s>\n", 
				ELEMENT_UNITOFMEASURE,pCurrent->subscriptionGroup_.brRecord_.unitOfMeasure_,
				ELEMENT_UNITOFMEASURE);
			strcat(XMLString ,tempXMLString);

			sprintf(tempXMLString,"<%s>%d</%s>\n", 
				ELEMENT_CHARGEABLEUNIT,pCurrent->subscriptionGroup_.brRecord_.chargeableUnit_,
				ELEMENT_CHARGEABLEUNIT);
			strcat(XMLString ,tempXMLString);

			sprintf(tempXMLString,"<%s>%d</%s>\n", 
				ELEMENT_RATEPERCHARGEABLEUNIT,pCurrent->subscriptionGroup_.brRecord_.ratePerChargeableUnit_,
				ELEMENT_RATEPERCHARGEABLEUNIT);
			strcat(XMLString ,tempXMLString);

			sprintf(tempXMLString,"<%s>%s</%s>\n", 
				ELEMENT_CLEARINGINTERMEDIARYNAME,pCurrent->subscriptionGroup_.brRecord_.clearingIntermediaryName_,
				ELEMENT_CLEARINGINTERMEDIARYNAME);
			strcat(XMLString ,tempXMLString);

			sprintf(tempXMLString,"<%s>%s</%s>\n", 
				ELEMENT_BILLINGINTERMEDIARYNAME,pCurrent->subscriptionGroup_.brRecord_.billingIntermediaryName_,
				ELEMENT_BILLINGINTERMEDIARYNAME);
			strcat(XMLString ,tempXMLString);

			sprintf(tempXMLString,"<%s>%d</%s>\n", 
				ELEMENT_SENDINGSERVICEPROVIDERDISPOSITION,pCurrent->subscriptionGroup_.brRecord_.sendingServiceProviderDisposition_,
				ELEMENT_SENDINGSERVICEPROVIDERDISPOSITION);
			strcat(XMLString ,tempXMLString);

			sprintf(tempXMLString,"<%s>%s</%s>\n", 
				ELEMENT_LASTCHANGEDDATETIME,pCurrent->subscriptionGroup_.brRecord_.lastChangedDateTime_,
				ELEMENT_LASTCHANGEDDATETIME);
			strcat(XMLString ,tempXMLString);

			sprintf(tempXMLString,"<%s>%d</%s>\n", 
				ELEMENT_FILERETENTIONDAYS,pCurrent->subscriptionGroup_.brRecord_.fileRetentionDays_,
				ELEMENT_FILERETENTIONDAYS);
			strcat(XMLString ,tempXMLString);

			sprintf(tempXMLString ,"</%s>\n",ELEMENT_BRRECORD);
			strcat(XMLString ,tempXMLString);
		
		}
		
		/*Copying optional control file directory parameter.*/
		/*
		if(pCurrent->subscriptionGroup_.controlFileDirectoryURL_)
			sprintf(tempXMLString,"<%s>%s</%s>\n", 
				ELEMENT_CTRLDIRURL,pCurrent->subscriptionGroup_.controlFileDirectoryURL_,
				ELEMENT_CTRLDIRURL);
		else
			sprintf(tempXMLString,"<%s></%s>\n", 
			ELEMENT_CTRLDIRURL,ELEMENT_CTRLDIRURL);
		strcat(XMLString ,tempXMLString);
		*/

		sprintf(tempXMLString ,"</%s>\n",ELEMENT_GROUPINFOITEM);
		strcat(XMLString ,tempXMLString);

		/*Move to next subscription group.*/
		pCurrent = pCurrent->nextSubscriptionGroup_;
	}/*for loop ends here*/

	/*Appending last xml information to xml string.*/
	strcat(XMLString,"</groupInfoList>\n");
	strcat(XMLString,"</extension>\n</supportedProtocolItem>\n");
	strcat(XMLString,"</supportedProtocolList>\n</CapabilityRsp>\n");
#ifdef DEBUG_IPDRFTP
	printf("Leaving toXML()\n");
#endif
	return 0;
}

/*!
 *  NAME:
 *      readCapabilityFile() - reads capabilityFile.xml and populates
 *                          CapabilityFile*.
 *                                            
 *
 *  DESCRIPTION:1.This functions instantiates an instance of CapabilityFile*.
 *              2.It then retrieves xml file from URL and makes a local copy.
 *		        3.The local xml copy is parsed and the information is retrieved in SubscriptionGroupList.
 *				4.It validates the parsed information about SubscriptionGroups and returns in case of 
 *		          error else assigns the list to subscriptionGroupList member of CapabilityFile structure.
 *  
 *  ARGUMENTS:
 *       -	pErrorList (IN / OUT)
 *       -  pCapabilityFile (IN / OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
CapabilityFile* readCapabilityFile(ListErrorStruct* pErrorList ,
						           char* capabilityFileURL)
{
	/*This will contain the parsed subscription group information*/
	//SubscriptionGroupList* pSubscriptionGroupList = NULL;
	CapabilityXMLInfo capabilityXMLInfo;
	/*It is passed to get_File()*/
	int error = 0;
	/*Returned to the calling application.*/
	CapabilityFile* pCapabilityFile;
#ifdef DEBUG_IPDRFTP
	printf("Entering readCapabilityFile()\n");
#endif
	if(!capabilityFileURL){/*error handling*/
		ftpErrorHandler(pErrorList,
			         IPDR_FUNCTION_PARAMETER_NULL,
					 "readCapabilityFile");
		return NULL;
	}
	
	/*Allocate memory for pCapabilityFile and its members*/
    pCapabilityFile = (CapabilityFile*)malloc(sizeof(CapabilityFile));
	if(!pCapabilityFile){/*Error Handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_MALLOC_FAILED,
					 "readCapabilityFile");
		return NULL;
	}

	pCapabilityFile->subscriptionGroupList_ = NULL;
	pCapabilityFile->capabilityFileURL_ =
		(char*)malloc(strlen(capabilityFileURL)+1);
	if(!pCapabilityFile->capabilityFileURL_){/*error handling*/
		ftpErrorHandler(pErrorList,
					 IPDR_MALLOC_FAILED,
					 "readCapabilityFile");
		freeCapabilityFile(pCapabilityFile);
		return NULL;
	}
	strcpy(pCapabilityFile->capabilityFileURL_ ,capabilityFileURL);

	/*Get file from remote URL*/
	if((get_File(pCapabilityFile->capabilityFileURL_,
		    LOCAL_CAPABILITYFILE,"",&error))==1){/*Error handling*/
		ftpErrorHandler(pErrorList,
					 error,
					 "readCapabilityFile");
		freeCapabilityFile(pCapabilityFile);
		return NULL;
	}
	
	/*Parse the local copy and ...*/
	/*store SubscriptionGroupInfo in pSubscriptionGroupList*/
	capabilityXMLInfo.subsGroupList_ = NULL;
	strcpy(capabilityXMLInfo.primitiveItemValue_ ,"");
	strcpy(capabilityXMLInfo.protocolMapping_ ,"");
	if((error = parseXMLCapabilityFile(pErrorList ,
		                   &capabilityXMLInfo))!=0){
		ftpErrorHandler(pErrorList,
					 IPDR_INVALID_CAPABILITY_XML,
					 "readCapabilityFile");
		freeCapabilityFile(pCapabilityFile);
		return NULL;
	};

	/*validate parsed subscription group information stored */
	/*in pSubscriptionGroupList and handling any error.*/
	if((error = validateSubsGroupList(capabilityXMLInfo.subsGroupList_))!=0){
		ftpErrorHandler(pErrorList,
					 error,
					 "readCapabilityFile");
		freeSubscriptionGroupList(capabilityXMLInfo.subsGroupList_);
		return NULL;
	}

	/*assign parsed information to capabilityFile variable.*/
	pCapabilityFile->subscriptionGroupList_ = capabilityXMLInfo.subsGroupList_;
#ifdef DEBUG_IPDRFTP
	printf("Leaving readCapabilityFile()\n");
#endif	
	return (pCapabilityFile);
}

/*!
 *  NAME:
 *      parseXMLCapabilityFile() - parses local copy of CapabilityFile.xml
 *                                            
 *
 *  DESCRIPTION:1.This function actually parses xml capability file.
 *              2.It initialises data structures where the parsed information is to be stored and 
 *		          other structures required by "expat".
 *		
 *  ARGUMENTS:
 *       -	pErrorList (IN / OUT)
 *       -  pCapabilityFile (IN)
 *       -  pSubsGroupList (OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int parseXMLCapabilityFile(ListErrorStruct* pErrorList ,
						   CapabilityXMLInfo* pCapabilityXMLInfo)					       
{
  /*This will store information in xml file.*/
  char buf[BUFSIZE];
  /*This will store parsed subscription group information.*/
  void* userData = NULL;
  /*Variables required by expat*/
  XML_Parser parser = XML_ParserCreate(NULL);
  int done;
  int depth = 0;
  FILE* stream; 
#ifdef DEBUG_IPDRFTP
	printf("Entering parseXMLCapabilityFile()\n");
#endif
  //userData = (SubscriptionGroupList**)pSubsGroupList;
	userData = (CapabilityXMLInfo*)pCapabilityXMLInfo;
  stream = fopen(LOCAL_CAPABILITYFILE,"r");
  XML_SetUserData(parser, userData);
  XML_SetElementHandler(parser ,startElement ,endElement);
  XML_SetCharacterDataHandler(parser ,dataHandler);

  do {
	size_t len = fread(buf, 1, sizeof(buf), stream);
    done = len < sizeof(buf);
    if (!XML_Parse(parser, buf, len, done)) {
      fprintf(stderr,
	      "%s at line %d\n",
	      XML_ErrorString(XML_GetErrorCode(parser)),
	      XML_GetCurrentLineNumber(parser));
      return 1;
    }
  } while (!done);

  //userData = (SubscriptionGroupList**)XML_GetUserData(parser);
  userData = (CapabilityXMLInfo*)XML_GetUserData(parser);
  //pSubsGroupList = (SubscriptionGroupList**)userData;
  pCapabilityXMLInfo = (CapabilityXMLInfo*)userData;

  XML_ParserFree(parser);
#ifdef DEBUG_IPDRFTP
	printf("Leaving parseXMLCapabilityFile()\n");
#endif
  return 0;
}

/*!
 *  NAME:
 *      startElement() - This function is called as any start tag  
 *                   for an element is encountered.
 *
 *  DESCRIPTION:1.This function keeps track of starting elements.
 *              2.It allocates memory for a new subscription group list node
 *                before any subscription group is parsed.
 *				3.It compares start element name with subscription group start elements              
 *                and stores flag to handle the data value corresponding to start element. 
 * 
 *  ARGUMENTS:
 *       - userData {IN}
 *       - name {IN}
 *       - atts (IN)
 *
 *  RETURNS  
 *      Returns void
 *    
 */
static void
startElement(void *userData, const char *name, const char **atts)
{
	/*This will store subscription group information*/
	//SubscriptionGroupList** pSubsGroupList = (SubscriptionGroupList**)userData;
	CapabilityXMLInfo* pCapabilityXMLInfo = (CapabilityXMLInfo*)userData;
	/*This is used to allocate memory for new subscription group list node.*/
	SubscriptionGroupList* pCurrent = NULL;
	/*This is used to traverse to the end of the subscription group list.*/
	SubscriptionGroupList* pTemp = NULL;
	int index = 0;

	for (index = 0; atts[index]; index += 2) {
		if(strcmp(atts[index],ATTR_PROTOCOLMAPPING)==0)
			strcpy(pCapabilityXMLInfo->protocolMapping_,atts[index + 1]);
	}

	//pTemp = *pSubsGroupList;
	pTemp = pCapabilityXMLInfo->subsGroupList_;
	
	if(strcmp(pCapabilityXMLInfo->protocolMapping_,VALUE_PROTOCOLMAPPING) == 0 &&
		strcmp(pCapabilityXMLInfo->primitiveItemValue_,DATA_PRIMITIVEITEM) == 0)
	{
	/*Allocate memory for new node and initialise its members to NULL.*/
		if((strcmp(name,ELEMENT_GROUPINFOITEM))==0)
		{
			pCurrent = (SubscriptionGroupList*)malloc
				(sizeof(SubscriptionGroupList));
			pCurrent->subscriptionGroup_.groupID_ = NULL;
			pCurrent->subscriptionGroup_.controlFileDirectory_ = NULL;
			pCurrent->subscriptionGroup_.controlFilePrefix_ = NULL;
			pCurrent->subscriptionGroup_.controlFileNamePolicy_ = NULL;
			pCurrent->subscriptionGroup_.controlFileSuffix_ = NULL;
			pCurrent->subscriptionGroup_.controlFileLocalDirectory_ = NULL;

			pCurrent->subscriptionGroup_.brRecordFlag_ = 0;
   		pCurrent->subscriptionGroup_.brRecord_.homeServiceProviderType_ = 0;
   		pCurrent->subscriptionGroup_.brRecord_.homeServiceProvider_ = NULL;
   		pCurrent->subscriptionGroup_.brRecord_.homeServiceProviderName_ = NULL;
   		pCurrent->subscriptionGroup_.brRecord_.accessProviderType_ = 0;
   		pCurrent->subscriptionGroup_.brRecord_.accessProvider_ = NULL;
   		pCurrent->subscriptionGroup_.brRecord_.accessProviderName_ = NULL;
   		pCurrent->subscriptionGroup_.brRecord_.contractReference_ = NULL;
   		pCurrent->subscriptionGroup_.brRecord_.businessRelationshipID_ = NULL;
   		pCurrent->subscriptionGroup_.brRecord_.summaryXchgHrs_ = 0;
   		pCurrent->subscriptionGroup_.brRecord_.settlementXcghDays_ = NULL;
   		pCurrent->subscriptionGroup_.brRecord_.netPosXchgDays_ = NULL;
   		pCurrent->subscriptionGroup_.brRecord_.effectiveDateTime_ = NULL;
   		pCurrent->subscriptionGroup_.brRecord_.response_ = 0;
   		pCurrent->subscriptionGroup_.brRecord_.xchgProtocol_ = 0;
   		pCurrent->subscriptionGroup_.brRecord_.xchgSLA_ = 0;
   		pCurrent->subscriptionGroup_.brRecord_.xchgEncoding_ = 0;
   		pCurrent->subscriptionGroup_.brRecord_.disputeResolution_ = 0;
   		pCurrent->subscriptionGroup_.brRecord_.locationInfo_ = NULL;
   		pCurrent->subscriptionGroup_.brRecord_.BRContactName_ = NULL;
   		pCurrent->subscriptionGroup_.brRecord_.rejectReturn_ = 0;
   		pCurrent->subscriptionGroup_.brRecord_.frequencyOfAuditReconciliation_ = 0;
   		pCurrent->subscriptionGroup_.brRecord_.xchgCurrencyType_ = NULL;
   		pCurrent->subscriptionGroup_.brRecord_.unitOfMeasure_ = 0;
   		pCurrent->subscriptionGroup_.brRecord_.chargeableUnit_ = 0;
   		pCurrent->subscriptionGroup_.brRecord_.ratePerChargeableUnit_ = 0;
   		pCurrent->subscriptionGroup_.brRecord_.clearingIntermediaryName_ = NULL;
   		pCurrent->subscriptionGroup_.brRecord_.billingIntermediaryName_ = NULL;
   		pCurrent->subscriptionGroup_.brRecord_.sendingServiceProviderDisposition_ = 0;
   		pCurrent->subscriptionGroup_.brRecord_.lastChangedDateTime_ = NULL;
   		pCurrent->subscriptionGroup_.brRecord_.fileRetentionDays_ = 0;

			pCurrent->nextSubscriptionGroup_ = NULL;

			/*If pSubsGroupList is empty*/
			if(!pCapabilityXMLInfo->subsGroupList_)
				pCapabilityXMLInfo->subsGroupList_ = pCurrent;
			/*If pSubsGroupList is not empty then go to its end*/
			else
			{	
				for( ;pTemp->nextSubscriptionGroup_;)
					pTemp = pTemp->nextSubscriptionGroup_;
				pTemp->nextSubscriptionGroup_ = pCurrent;
			}
		}
		/*if the first element is not "groupInfoItem"*/
		else
		{
			for(;pTemp && pTemp->nextSubscriptionGroup_;)
				pTemp = pTemp->nextSubscriptionGroup_;
			pCurrent = pTemp;
		}
	}

	//value will be handled in data handler
	if((strcmp(name,ELEMENT_PRIMITIVEITEM))==0)
	{
		strcpy(pCapabilityXMLInfo->primitiveItemValue_ ,"1");
	}

	if(pCurrent){
	
		if((strcmp(name,ELEMENT_GROUPID))==0)
		{
			pCurrent->subscriptionGroup_.groupID_ = "1";
		}
		if((strcmp(name,ELEMENT_CTRLDIR))==0)
		{
			pCurrent->subscriptionGroup_.controlFileDirectory_ = "1";
		}
		if((strcmp(name,ELEMENT_CTRLPREFIX))==0)
		{
			pCurrent->subscriptionGroup_.controlFilePrefix_ = "1";
		}
		if((strcmp(name,ELEMENT_CTRLNAMEPOLICY))==0)
		{
			pCurrent->subscriptionGroup_.controlFileNamePolicy_ = "1";
		}
		if((strcmp(name,ELEMENT_CTRLSUFFIX))==0)
		{
			pCurrent->subscriptionGroup_.controlFileSuffix_ = "1";
		}

		//BRRecord
		if((strcmp(name,ELEMENT_BRRECORD))==0)
		{
			pCurrent->subscriptionGroup_.brRecordFlag_ = 1;
		}

      if(pCurrent->subscriptionGroup_.brRecordFlag_ == 1) {
   		if((strcmp(name,ELEMENT_HOMESERVICEPROVIDERTYPE))==0)
   		{
   			pCurrent->subscriptionGroup_.brRecord_.homeServiceProviderType_ = 1;
   		}
   		if((strcmp(name,ELEMENT_HOMESERVICEPROVIDER))==0)
   		{
   			pCurrent->subscriptionGroup_.brRecord_.homeServiceProvider_ = "1";
   		}
   		if((strcmp(name,ELEMENT_HOMESERVICEPROVIDERNAME))==0)
   		{
   			pCurrent->subscriptionGroup_.brRecord_.homeServiceProviderName_ = "1";
   		}
   		if((strcmp(name,ELEMENT_ACCESSPROVIDERTYPE))==0)
   		{
   			pCurrent->subscriptionGroup_.brRecord_.accessProviderType_ = 1;
   		}
   		if((strcmp(name,ELEMENT_ACCESSPROVIDER))==0)
   		{
   			pCurrent->subscriptionGroup_.brRecord_.accessProvider_ = "1";
   		}
   		if((strcmp(name,ELEMENT_ACCESSPROVIDERNAME))==0)
   		{
   			pCurrent->subscriptionGroup_.brRecord_.accessProviderName_ = "1";
   		}
   		if((strcmp(name,ELEMENT_CONTRACTREFERENCE))==0)
   		{
   			pCurrent->subscriptionGroup_.brRecord_.contractReference_ = "1";
   		}
   		if((strcmp(name,ELEMENT_BUSINESSRELATIONSHIPID))==0)
   		{
   			pCurrent->subscriptionGroup_.brRecord_.businessRelationshipID_ = "1";
   		}
   		if((strcmp(name,ELEMENT_SUMMARYXCHGHRS))==0)
   		{
   			pCurrent->subscriptionGroup_.brRecord_.summaryXchgHrs_ = 1;
   		}
   		if((strcmp(name,ELEMENT_SETTLEMENTXCGHDAYS))==0)
   		{
   			pCurrent->subscriptionGroup_.brRecord_.settlementXcghDays_ = "1";
   		}
   		if((strcmp(name,ELEMENT_NETPOSXCHGDAYS))==0)
   		{
   			pCurrent->subscriptionGroup_.brRecord_.netPosXchgDays_ = "1";
   		}
   		if((strcmp(name,ELEMENT_EFFECTIVEDATETIME))==0)
   		{
   			pCurrent->subscriptionGroup_.brRecord_.effectiveDateTime_ = "1";
   		}
   		if((strcmp(name,ELEMENT_RESPONSE))==0)
   		{
   			pCurrent->subscriptionGroup_.brRecord_.response_ = 1;
   		}
   		if((strcmp(name,ELEMENT_XCHGPROTOCOL))==0)
   		{
   			pCurrent->subscriptionGroup_.brRecord_.xchgProtocol_ = 1;
   		}
   		if((strcmp(name,ELEMENT_XCHGSLA))==0)
   		{
   			pCurrent->subscriptionGroup_.brRecord_.xchgSLA_ = 1;
   		}
   		if((strcmp(name,ELEMENT_XCHGENCODING))==0)
   		{
   			pCurrent->subscriptionGroup_.brRecord_.xchgEncoding_ = 1;
   		}
   		if((strcmp(name,ELEMENT_DISPUTERESOLUTION))==0)
   		{
   			pCurrent->subscriptionGroup_.brRecord_.disputeResolution_ = 1;
   		}
   		if((strcmp(name,ELEMENT_LOCATIONINFO))==0)
   		{
   			pCurrent->subscriptionGroup_.brRecord_.locationInfo_ = "1";
   		}
   		if((strcmp(name,ELEMENT_BRCONTACTNAME))==0)
   		{
   			pCurrent->subscriptionGroup_.brRecord_.BRContactName_ = "1";
   		}
   		if((strcmp(name,ELEMENT_REJECTRETURN))==0)
   		{
   			pCurrent->subscriptionGroup_.brRecord_.rejectReturn_ = 1;
   		}
   		if((strcmp(name,ELEMENT_FREQUENCYOFAUDITRECONCILIATION))==0)
   		{
   			pCurrent->subscriptionGroup_.brRecord_.frequencyOfAuditReconciliation_ = 1;
   		}
   		if((strcmp(name,ELEMENT_XCHGCURRENCYTYPE))==0)
   		{
   			pCurrent->subscriptionGroup_.brRecord_.xchgCurrencyType_ = "1";
   		}
   		if((strcmp(name,ELEMENT_UNITOFMEASURE))==0)
   		{
   			pCurrent->subscriptionGroup_.brRecord_.unitOfMeasure_ = 1;
   		}
   		if((strcmp(name,ELEMENT_CHARGEABLEUNIT))==0)
   		{
   			pCurrent->subscriptionGroup_.brRecord_.chargeableUnit_ = 1;
   		}
   		if((strcmp(name,ELEMENT_RATEPERCHARGEABLEUNIT))==0)
   		{
   			pCurrent->subscriptionGroup_.brRecord_.ratePerChargeableUnit_ = 1;
   		}
   		if((strcmp(name,ELEMENT_CLEARINGINTERMEDIARYNAME))==0)
   		{
   			pCurrent->subscriptionGroup_.brRecord_.clearingIntermediaryName_ = "1";
   		}
   		if((strcmp(name,ELEMENT_BILLINGINTERMEDIARYNAME))==0)
   		{
   			pCurrent->subscriptionGroup_.brRecord_.billingIntermediaryName_ = "1";
   		}
   		if((strcmp(name,ELEMENT_SENDINGSERVICEPROVIDERDISPOSITION))==0)
   		{
   			pCurrent->subscriptionGroup_.brRecord_.sendingServiceProviderDisposition_ = 1;
   		}
   		if((strcmp(name,ELEMENT_LASTCHANGEDDATETIME))==0)
   		{
   			pCurrent->subscriptionGroup_.brRecord_.lastChangedDateTime_ = "1";
   		}
   		if((strcmp(name,ELEMENT_FILERETENTIONDAYS))==0)
   		{
   			pCurrent->subscriptionGroup_.brRecord_.fileRetentionDays_ = 1;
   		}
		}
	}

	//depth += 1;	
}

/*!
 *  NAME:
 *      dataHandler() - This function handles values of XML elements.
 *
 *  DESCRIPTION:1.This function is called by expat library as any data value is encountered.
 *				2.It copies the data value to current subscription group list node.
 * 
 *  ARGUMENTS:
 *       - userData (IN / OUT)
 *       - data (IN)
 *       - len (IN)
 *
 *  RETURNS  
 *      Returns void
 *    
 */
static void
dataHandler(void *userData, const char *data,int len)
{
	char temp[150] = "";
	//SubscriptionGroupList** pSubsGroupList = (SubscriptionGroupList**)userData;
	CapabilityXMLInfo* pCapabilityXMLInfo = (CapabilityXMLInfo*)userData;
	SubscriptionGroupList* pCurrent;
	//SubscriptionGroupList* pTemp = *pSubsGroupList;
	SubscriptionGroupList* pTemp = pCapabilityXMLInfo->subsGroupList_;
	int index = 0;

	/*checking validity of data and if it is in quotes*/
	if(data && len>2 && data[0] == '"'){
		/*filtering '"' from value*/
		for(index = 0 ; index < (len-2) ; index++)
			temp[index] = data[index+1];
		temp[index] = 0;
	}

	/*checking validity of data if not in quotes*/
	else if(data && len > 0 && data[0]!='"'){
		strncpy(temp,data,len);
		temp[len] = 0;
	}

	/*Traverse to the last node of subscription group list*/
	for( ;pTemp && pTemp->nextSubscriptionGroup_; )
			pTemp = pTemp->nextSubscriptionGroup_;
	pCurrent = pTemp;
	
	if(strncmp(pCapabilityXMLInfo->primitiveItemValue_,"1",1)==0)
		strcpy(pCapabilityXMLInfo->primitiveItemValue_ ,temp);

	/*populating the subscriptionGroup*/
	if(pCurrent && pCurrent->subscriptionGroup_.groupID_){
		if((strncmp(pCurrent->subscriptionGroup_.groupID_ ,"1" ,1))==0){
			if(strlen(temp)!=0){
				pCurrent->subscriptionGroup_.groupID_ = 
					(char*)malloc(strlen(temp)+1);
				strcpy(pCurrent->subscriptionGroup_.groupID_ ,temp);
			}
			else
				pCurrent->subscriptionGroup_.groupID_ = NULL;
		}
	}
	if(pCurrent && pCurrent->subscriptionGroup_.controlFileDirectory_){
		if(strncmp(pCurrent->subscriptionGroup_.controlFileDirectory_ ,
			"1" ,1)==0){
			if(strlen(temp)!=0){
				pCurrent->subscriptionGroup_.controlFileDirectory_ = 
					(char*)malloc(strlen(temp)+1);
				strcpy(pCurrent->subscriptionGroup_.controlFileDirectory_ ,temp);
			}
			else
				pCurrent->subscriptionGroup_.controlFileDirectory_ = NULL;
		}
	}
	if(pCurrent && pCurrent->subscriptionGroup_.controlFilePrefix_){
		if(strncmp(pCurrent->subscriptionGroup_.controlFilePrefix_ ,
			"1" ,1)==0){
			if(strlen(temp)!=0){
				pCurrent->subscriptionGroup_.controlFilePrefix_ = 
					(char*)malloc(strlen(temp)+1);
				strcpy(pCurrent->subscriptionGroup_.controlFilePrefix_ ,temp);
			}
			else
				pCurrent->subscriptionGroup_.controlFilePrefix_ = NULL;
		}
	}
	if(pCurrent && pCurrent->subscriptionGroup_.controlFileNamePolicy_){
		if(strncmp(pCurrent->subscriptionGroup_.controlFileNamePolicy_ ,
			"1" ,1)==0){
			if(strlen(temp)!=0){
				pCurrent->subscriptionGroup_.controlFileNamePolicy_ = 
					(char*)malloc(strlen(temp)+1);
				strcpy(pCurrent->subscriptionGroup_.controlFileNamePolicy_ ,temp);
			}
			else
				pCurrent->subscriptionGroup_.controlFileNamePolicy_ = NULL;
		}
	}
	if(pCurrent && pCurrent->subscriptionGroup_.controlFileSuffix_){
		if(strncmp(pCurrent->subscriptionGroup_.controlFileSuffix_ ,
			"1" ,1)==0){
			if(strlen(temp)!=0){
				pCurrent->subscriptionGroup_.controlFileSuffix_ = 
					(char*)malloc(strlen(temp)+1);
				strcpy(pCurrent->subscriptionGroup_.controlFileSuffix_ ,temp);
			}
			else
				pCurrent->subscriptionGroup_.controlFileSuffix_ = NULL;
		}
	}


   if(pCurrent && (pCurrent->subscriptionGroup_.brRecordFlag_ == 1)) {
		if(pCurrent->subscriptionGroup_.brRecord_.homeServiceProviderType_ == 1){
			if(strlen(temp)!=0){
				pCurrent->subscriptionGroup_.brRecord_.homeServiceProviderType_ = atoi(temp);
			}
			else
				pCurrent->subscriptionGroup_.brRecord_.homeServiceProviderType_ = -1;
		}
		if(pCurrent && pCurrent->subscriptionGroup_.brRecord_.homeServiceProvider_){
		if(strncmp(pCurrent->subscriptionGroup_.brRecord_.homeServiceProvider_ ,
			"1" ,1)==0){
			if(strlen(temp)!=0){
				pCurrent->subscriptionGroup_.brRecord_.homeServiceProvider_ = 
					(char*)malloc(strlen(temp)+1);
				strcpy(pCurrent->subscriptionGroup_.brRecord_.homeServiceProvider_ ,temp);
			}
			else
				pCurrent->subscriptionGroup_.brRecord_.homeServiceProvider_ = NULL;
		}
		}
		if(pCurrent && pCurrent->subscriptionGroup_.brRecord_.homeServiceProviderName_){
		if(strncmp(pCurrent->subscriptionGroup_.brRecord_.homeServiceProviderName_ ,
			"1" ,1)==0){
			if(strlen(temp)!=0){
				pCurrent->subscriptionGroup_.brRecord_.homeServiceProviderName_ = 
					(char*)malloc(strlen(temp)+1);
				strcpy(pCurrent->subscriptionGroup_.brRecord_.homeServiceProviderName_ ,temp);
			}
			else
				pCurrent->subscriptionGroup_.brRecord_.homeServiceProviderName_ = NULL;
		}
		}
		if(pCurrent->subscriptionGroup_.brRecord_.accessProviderType_ == 1){
			if(strlen(temp)!=0){
				pCurrent->subscriptionGroup_.brRecord_.accessProviderType_ = atoi(temp);
			}
			else
				pCurrent->subscriptionGroup_.brRecord_.accessProviderType_ = -1;
		}
		if(pCurrent && pCurrent->subscriptionGroup_.brRecord_.accessProvider_){
		if(strncmp(pCurrent->subscriptionGroup_.brRecord_.accessProvider_ ,
			"1" ,1)==0){
			if(strlen(temp)!=0){
				pCurrent->subscriptionGroup_.brRecord_.accessProvider_ = 
					(char*)malloc(strlen(temp)+1);
				strcpy(pCurrent->subscriptionGroup_.brRecord_.accessProvider_ ,temp);
			}
			else
				pCurrent->subscriptionGroup_.brRecord_.accessProvider_ = NULL;
		}
		}
		if(pCurrent && pCurrent->subscriptionGroup_.brRecord_.accessProviderName_){
		if(strncmp(pCurrent->subscriptionGroup_.brRecord_.accessProviderName_ ,
			"1" ,1)==0){
			if(strlen(temp)!=0){
				pCurrent->subscriptionGroup_.brRecord_.accessProviderName_ = 
					(char*)malloc(strlen(temp)+1);
				strcpy(pCurrent->subscriptionGroup_.brRecord_.accessProviderName_ ,temp);
			}
			else
				pCurrent->subscriptionGroup_.brRecord_.accessProviderName_ = NULL;
		}
		}
		if(pCurrent && pCurrent->subscriptionGroup_.brRecord_.contractReference_){
		if(strncmp(pCurrent->subscriptionGroup_.brRecord_.contractReference_ ,
			"1" ,1)==0){
			if(strlen(temp)!=0){
				pCurrent->subscriptionGroup_.brRecord_.contractReference_ = 
					(char*)malloc(strlen(temp)+1);
				strcpy(pCurrent->subscriptionGroup_.brRecord_.contractReference_ ,temp);
			}
			else
				pCurrent->subscriptionGroup_.brRecord_.contractReference_ = NULL;
		}
		}
		if(pCurrent && pCurrent->subscriptionGroup_.brRecord_.businessRelationshipID_){
		if(strncmp(pCurrent->subscriptionGroup_.brRecord_.businessRelationshipID_ ,
			"1" ,1)==0){
			if(strlen(temp)!=0){
				pCurrent->subscriptionGroup_.brRecord_.businessRelationshipID_ = 
					(char*)malloc(strlen(temp)+1);
				strcpy(pCurrent->subscriptionGroup_.brRecord_.businessRelationshipID_ ,temp);
			}
			else
				pCurrent->subscriptionGroup_.brRecord_.businessRelationshipID_ = NULL;
		}
		}
		if(pCurrent->subscriptionGroup_.brRecord_.summaryXchgHrs_ == 1){
			if(strlen(temp)!=0){
				pCurrent->subscriptionGroup_.brRecord_.summaryXchgHrs_ = atoi(temp);
			}
			else
				pCurrent->subscriptionGroup_.brRecord_.summaryXchgHrs_ = -1;
		}
		if(pCurrent && pCurrent->subscriptionGroup_.brRecord_.settlementXcghDays_){
		if(strncmp(pCurrent->subscriptionGroup_.brRecord_.settlementXcghDays_ ,
			"1" ,1)==0){
			if(strlen(temp)!=0){
				pCurrent->subscriptionGroup_.brRecord_.settlementXcghDays_ = 
					(char*)malloc(strlen(temp)+1);
				strcpy(pCurrent->subscriptionGroup_.brRecord_.settlementXcghDays_ ,temp);
			}
			else
				pCurrent->subscriptionGroup_.brRecord_.settlementXcghDays_ = NULL;
		}
		}
		if(pCurrent && pCurrent->subscriptionGroup_.brRecord_.netPosXchgDays_){
		if(strncmp(pCurrent->subscriptionGroup_.brRecord_.netPosXchgDays_ ,
			"1" ,1)==0){
			if(strlen(temp)!=0){
				pCurrent->subscriptionGroup_.brRecord_.netPosXchgDays_ = 
					(char*)malloc(strlen(temp)+1);
				strcpy(pCurrent->subscriptionGroup_.brRecord_.netPosXchgDays_ ,temp);
			}
			else
				pCurrent->subscriptionGroup_.brRecord_.netPosXchgDays_ = NULL;
		}
		}
		if(pCurrent && pCurrent->subscriptionGroup_.brRecord_.effectiveDateTime_){
		if(strncmp(pCurrent->subscriptionGroup_.brRecord_.effectiveDateTime_ ,
			"1" ,1)==0){
			if(strlen(temp)!=0){
				pCurrent->subscriptionGroup_.brRecord_.effectiveDateTime_ = 
					(char*)malloc(strlen(temp)+1);
				strcpy(pCurrent->subscriptionGroup_.brRecord_.effectiveDateTime_ ,temp);
			}
			else
				pCurrent->subscriptionGroup_.brRecord_.effectiveDateTime_ = NULL;
		}
		}
		if(pCurrent->subscriptionGroup_.brRecord_.response_ == 1){
			if(strlen(temp)!=0){
				pCurrent->subscriptionGroup_.brRecord_.response_ = atoi(temp);
			}
			else
				pCurrent->subscriptionGroup_.brRecord_.response_ = -1;
		}
		if(pCurrent->subscriptionGroup_.brRecord_.xchgProtocol_ == 1){
			if(strlen(temp)!=0){
				pCurrent->subscriptionGroup_.brRecord_.xchgProtocol_ = atoi(temp);
			}
			else
				pCurrent->subscriptionGroup_.brRecord_.xchgProtocol_ = -1;
		}
		if(pCurrent->subscriptionGroup_.brRecord_.xchgSLA_ == 1){
			if(strlen(temp)!=0){
				pCurrent->subscriptionGroup_.brRecord_.xchgSLA_ = atoi(temp);
			}
			else
				pCurrent->subscriptionGroup_.brRecord_.xchgSLA_ = -1;
		}
		if(pCurrent->subscriptionGroup_.brRecord_.xchgEncoding_ == 1){
			if(strlen(temp)!=0){
				pCurrent->subscriptionGroup_.brRecord_.xchgEncoding_ = atoi(temp);
			}
			else
				pCurrent->subscriptionGroup_.brRecord_.xchgEncoding_ = -1;
		}
		if(pCurrent->subscriptionGroup_.brRecord_.disputeResolution_ == 1){
			if(strlen(temp)!=0){
				pCurrent->subscriptionGroup_.brRecord_.disputeResolution_ = atoi(temp);
			}
			else
				pCurrent->subscriptionGroup_.brRecord_.disputeResolution_ = -1;
		}
		if(pCurrent && pCurrent->subscriptionGroup_.brRecord_.locationInfo_){
		if(strncmp(pCurrent->subscriptionGroup_.brRecord_.locationInfo_ ,
			"1" ,1)==0){
			if(strlen(temp)!=0){
				pCurrent->subscriptionGroup_.brRecord_.locationInfo_ = 
					(char*)malloc(strlen(temp)+1);
				strcpy(pCurrent->subscriptionGroup_.brRecord_.locationInfo_ ,temp);
			}
			else
				pCurrent->subscriptionGroup_.brRecord_.locationInfo_ = NULL;
		}
		}
		if(pCurrent && pCurrent->subscriptionGroup_.brRecord_.BRContactName_){
		if(strncmp(pCurrent->subscriptionGroup_.brRecord_.BRContactName_ ,
			"1" ,1)==0){
			if(strlen(temp)!=0){
				pCurrent->subscriptionGroup_.brRecord_.BRContactName_ = 
					(char*)malloc(strlen(temp)+1);
				strcpy(pCurrent->subscriptionGroup_.brRecord_.BRContactName_ ,temp);
			}
			else
				pCurrent->subscriptionGroup_.brRecord_.BRContactName_ = NULL;
		}
		}
		if(pCurrent->subscriptionGroup_.brRecord_.rejectReturn_ == 1){
			if(strlen(temp)!=0){
				pCurrent->subscriptionGroup_.brRecord_.rejectReturn_ = atoi(temp);
			}
			else
				pCurrent->subscriptionGroup_.brRecord_.rejectReturn_ = -1;
		}
		if(pCurrent->subscriptionGroup_.brRecord_.frequencyOfAuditReconciliation_ == 1){
			if(strlen(temp)!=0){
				pCurrent->subscriptionGroup_.brRecord_.frequencyOfAuditReconciliation_ = atoi(temp);
			}
			else
				pCurrent->subscriptionGroup_.brRecord_.frequencyOfAuditReconciliation_ = -1;
		}
		if(pCurrent && pCurrent->subscriptionGroup_.brRecord_.xchgCurrencyType_){
		if(strncmp(pCurrent->subscriptionGroup_.brRecord_.xchgCurrencyType_ ,
			"1" ,1)==0){
			if(strlen(temp)!=0){
				pCurrent->subscriptionGroup_.brRecord_.xchgCurrencyType_ = 
					(char*)malloc(strlen(temp)+1);
				strcpy(pCurrent->subscriptionGroup_.brRecord_.xchgCurrencyType_ ,temp);
			}
			else
				pCurrent->subscriptionGroup_.brRecord_.xchgCurrencyType_ = NULL;
		}
		}
		if(pCurrent->subscriptionGroup_.brRecord_.unitOfMeasure_ == 1){
			if(strlen(temp)!=0){
				pCurrent->subscriptionGroup_.brRecord_.unitOfMeasure_ = atoi(temp);
			}
			else
				pCurrent->subscriptionGroup_.brRecord_.unitOfMeasure_ = -1;
		}
		if(pCurrent->subscriptionGroup_.brRecord_.chargeableUnit_ == 1){
			if(strlen(temp)!=0){
				pCurrent->subscriptionGroup_.brRecord_.chargeableUnit_ = atoi(temp);
			}
			else
				pCurrent->subscriptionGroup_.brRecord_.chargeableUnit_ = -1;
		}
		if(pCurrent->subscriptionGroup_.brRecord_.ratePerChargeableUnit_ == 1){
			if(strlen(temp)!=0){
				pCurrent->subscriptionGroup_.brRecord_.ratePerChargeableUnit_ = atoi(temp);
			}
			else
				pCurrent->subscriptionGroup_.brRecord_.ratePerChargeableUnit_ = -1;
		}
		if(pCurrent && pCurrent->subscriptionGroup_.brRecord_.clearingIntermediaryName_){
		if(strncmp(pCurrent->subscriptionGroup_.brRecord_.clearingIntermediaryName_ ,
			"1" ,1)==0){
			if(strlen(temp)!=0){
				pCurrent->subscriptionGroup_.brRecord_.clearingIntermediaryName_ = 
					(char*)malloc(strlen(temp)+1);
				strcpy(pCurrent->subscriptionGroup_.brRecord_.clearingIntermediaryName_ ,temp);
			}
			else
				pCurrent->subscriptionGroup_.brRecord_.clearingIntermediaryName_ = NULL;
		}
		}
		if(pCurrent && pCurrent->subscriptionGroup_.brRecord_.billingIntermediaryName_){
		if(strncmp(pCurrent->subscriptionGroup_.brRecord_.billingIntermediaryName_ ,
			"1" ,1)==0){
			if(strlen(temp)!=0){
				pCurrent->subscriptionGroup_.brRecord_.billingIntermediaryName_ = 
					(char*)malloc(strlen(temp)+1);
				strcpy(pCurrent->subscriptionGroup_.brRecord_.billingIntermediaryName_ ,temp);
			}
			else
				pCurrent->subscriptionGroup_.brRecord_.billingIntermediaryName_ = NULL;
		}
		}
		if(pCurrent->subscriptionGroup_.brRecord_.sendingServiceProviderDisposition_ == 1){
			if(strlen(temp)!=0){
				pCurrent->subscriptionGroup_.brRecord_.sendingServiceProviderDisposition_ = atoi(temp);
			}
			else
				pCurrent->subscriptionGroup_.brRecord_.sendingServiceProviderDisposition_ = -1;
		}
		if(pCurrent && pCurrent->subscriptionGroup_.brRecord_.lastChangedDateTime_){
		if(strncmp(pCurrent->subscriptionGroup_.brRecord_.lastChangedDateTime_ ,
			"1" ,1)==0){
			if(strlen(temp)!=0){
				pCurrent->subscriptionGroup_.brRecord_.lastChangedDateTime_ = 
					(char*)malloc(strlen(temp)+1);
				strcpy(pCurrent->subscriptionGroup_.brRecord_.lastChangedDateTime_ ,temp);
			}
			else
				pCurrent->subscriptionGroup_.brRecord_.lastChangedDateTime_ = NULL;
		}
		}
		if(pCurrent->subscriptionGroup_.brRecord_.fileRetentionDays_ == 1){
			if(strlen(temp)!=0){
				pCurrent->subscriptionGroup_.brRecord_.fileRetentionDays_ = atoi(temp);
			}
			else
				pCurrent->subscriptionGroup_.brRecord_.fileRetentionDays_ = -1;
		}
	}

}

/*!
 *  NAME:
 *      endElement() - This function is called as any end tag  
 *                         for an element is encountered..
 *
 *  DESCRIPTION:1.This is required by by the expat library for parsing.
 *
 *  ARGUMENTS:
 *       - userData {IN / OUT}
 *       - name {IN}
 *
 *  RETURNS  
 *      Returns void
 *    
 */
static void
endElement(void *userData, const char *name)
{
	//SubscriptionGroupList** pSubsGroupList = (SubscriptionGroupList**)userData;
	CapabilityXMLInfo* pCapabilityXMLInfo = (CapabilityXMLInfo*)userData;
	SubscriptionGroupList* pCurrent = NULL;
	SubscriptionGroupList* pTemp = pCapabilityXMLInfo->subsGroupList_;

	/*Traverse to the last node of subscription group list*/
	for( ;pTemp && pTemp->nextSubscriptionGroup_; )
			pTemp = pTemp->nextSubscriptionGroup_;
	pCurrent = pTemp;

	/*checking for any missing parameter value*/
	if(pCurrent){
		if((strcmp(name,ELEMENT_GROUPID))==0 &&
			(strcmp(pCurrent->subscriptionGroup_.groupID_,"1"))==0)
		{
			pCurrent->subscriptionGroup_.groupID_ = NULL;
		}
		if((strcmp(name,ELEMENT_CTRLDIR))==0 &&
			(strcmp(pCurrent->subscriptionGroup_.controlFileDirectory_,"1"))==0)
		{
			pCurrent->subscriptionGroup_.controlFileDirectory_ = NULL;
		}
		if((strcmp(name,ELEMENT_CTRLPREFIX))==0 &&
			(strcmp(pCurrent->subscriptionGroup_.controlFilePrefix_,"1"))==0)
		{
			pCurrent->subscriptionGroup_.controlFilePrefix_ = "";
		}
		if((strcmp(name,ELEMENT_CTRLNAMEPOLICY))==0 &&
			(strcmp(pCurrent->subscriptionGroup_.controlFileNamePolicy_,"1"))==0)
		{
			pCurrent->subscriptionGroup_.controlFileNamePolicy_ = NULL;
		}
		if((strcmp(name,ELEMENT_CTRLSUFFIX))==0 &&
			(strcmp(pCurrent->subscriptionGroup_.controlFileSuffix_,"1"))==0)
		{
			pCurrent->subscriptionGroup_.controlFileSuffix_ = "";
		}
		
	}
	//depth --;
}
