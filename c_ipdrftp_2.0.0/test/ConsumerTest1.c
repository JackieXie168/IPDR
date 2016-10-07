/**
 * File                 : Consumer5.2.1.c			         
 * Description          : Contains consumer flow for integration test 5.2.1.      
 * Author               : Infosys Tech Ltd           
 * Modification History :                            
 *---------------------------------------------------
 * Date       Name             Change/Description    
 *---------------------------------------------------
 * 01/29/02  Infosys           Created  
 *
 *
 * Copyright (C) 2002, The IPDR Organization, all rights reserved.
 * The use and distribution of this software is governed by the terms of
 * the license agreement which can be found in the file LICENSE.TXT at
 * the top of this source tree.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND, either express or implied.
 */


#include <stdlib.h>
#include <stdio.h>

#include "ftp/IpdrFtp.H"


int main()
{
	ListErrorStruct* errorList = newListErrorStructure();
	CapabilityFile* capabilityFile ;
	SubscriptionGroupList subscriptionGroupList;
	SubscriptionGroupList* tempSubsList = NULL;
	ListGroupID groupIDs;
	ListGroupID* tempgroupIDs = NULL;
	int result = 0;
	int index = 0;
	SubscriptionGroup* subscriptionGroup1 = 
		newSubscriptionGroup(errorList,"GPRS","c:/GPRS","GPRS-","NNNNN",".ctrl");
	SubscriptionGroup subscriptionGroup2 = {NULL,"c:/Optical","Optical-","NN",".optics"};
	SubscriptionGroup* subscriptionGroup3 = 
		newSubscriptionGroup(errorList,"GSM","c:\\directory","Infosys3","NNN","CAPS");

	ControlFile* ctrlFile = NULL;
	char docName[100] = "/home/vishal/document-";
        unsigned int docLen = strlen(docName);
	char controlFileDir[200] =  "";
	char buffer[200] = "";
	char policySeqNbr[50] = "";
	char rangeFileName[50] = "";
	char controlFileName[100] = "";
	char controlFileURL[200] = "";
	char rangeFileURL[200] = "";
	long sequenceNbr = 0;
	char lastSeqNbr[50] = "";
	char firstSeqNbr[50] = "";
	SubscriptionGroup* subsGroupTest;//for subscription group API's testing
        char capabilityFileURL[200] = "";

//To check genaration of XML
	char capabilityFileXMLString[4000] = "";

//Test 5.1.1.1
	//Test Case 1
	/*
	printf("Please enter capabilityfile url:");
	scanf("%s",capabilityFileURL);
	capabilityFile = readCapabilityFile(errorList ,capabilityFileURL);
	*/
	//capabilityFile = readCapabilityFile(errorList ,"http://192.168.122.208/IPDR/Capability_Producer.xml");
	capabilityFile = readCapabilityFile(errorList ,"file://mohsaswks15333/share/MyXML.xml");
	if(!capabilityFile)
		printLastError(errorList);
	

//Test 5.1.1.2 :- Adding subscription groups to the list
	//Test Case 1
	/*result = addGroup(errorList ,capabilityFile ,subscriptionGroup1);
	if(result!=0)
		printLastError(errorList);
      */  
	//get subscription group with groupId Voip from linked list. 
    result = getGroup(errorList ,capabilityFile ,"Voip" ,&subsGroupTest);
	if(result == 0)
	{
		printf("match found\n");
	}
	else 
		printLastError(errorList);

//Test 5.1.1.5 :-Testing for getGroupIDs from the subscriptionGroupList
	//Test Case 1
	result = getGroupIDs(errorList ,capabilityFile ,&groupIDs);
	if(result == 0)
	{
		printf("linklist is not empty\n");
		tempgroupIDs = &groupIDs;
		for( ;tempgroupIDs; )
		{
			printf("%s\n",tempgroupIDs->groupID_);
			tempgroupIDs = tempgroupIDs->nextGroupID_;
		}
	}
	else
		printLastError(errorList);

//Test 5.1.1.7 :- Testing for retrieving all subscription groups.
	//Test Case 1
	result = getAllGroups(errorList ,capabilityFile ,&subscriptionGroupList);
	if(result == 0)
	{
		printf("linklist is not empty\n");
		tempSubsList = &subscriptionGroupList;
	    for( ;tempSubsList; )
		{
			printf("%s  %s %s %s %s",
				tempSubsList->subscriptionGroup_.groupID_,
				tempSubsList->subscriptionGroup_.controlFileDirectory_,
				tempSubsList->subscriptionGroup_.controlFilePrefix_,
				tempSubsList->subscriptionGroup_.controlFileNamePolicy_,
				tempSubsList->subscriptionGroup_.controlFileSuffix_);
			if(tempSubsList->subscriptionGroup_.controlFileLocalDirectory_)
				printf(" %s",tempSubsList->subscriptionGroup_.controlFileLocalDirectory_);
			printf("\n");
			tempSubsList = tempSubsList->nextSubscriptionGroup_;
		}
	}
	else
		printLastError(errorList);

	
//Test 5.1.4.1 :- Testing write operations on control file
//getting range file location
	if((result = getControlFileDirectory(errorList,subsGroupTest,controlFileDir))!=0)
		printLastError(errorList);
	if((result = getRangeFileName(errorList,subsGroupTest,rangeFileName))!=0)
		printLastError(errorList);
	else{
		strcpy(rangeFileURL,controlFileDir);
		strcat(rangeFileURL,rangeFileName);
	}
	if((result = getFirstSequenceNbr(errorList,rangeFileURL,firstSeqNbr))!=0)
		printLastError(errorList);
	if((result = getLastSequenceNbr(errorList,rangeFileURL,lastSeqNbr))!=0)
		printLastError(errorList);

	sequenceNbr = atol(firstSeqNbr);

//getting last sequence number from rangefile	
	while(sequenceNbr <= atol(lastSeqNbr)){
//getting control file location
	result = getControlFileName(errorList,subsGroupTest,controlFileName,sequenceNbr);
	if(result!=0)
		printLastError(errorList);
	strcpy(controlFileURL,controlFileDir);
	strcat(controlFileURL,controlFileName);
//Reading control files from URI
	ctrlFile = controlFileFromURL(errorList,controlFileURL);
	while(isEnd(errorList,ctrlFile)!=1){
		if((result=getDocName(errorList,ctrlFile,docName))!=0)
			break;
		if(strcmp(docName,"NULL")==0)
		{
			refreshControlFile(errorList,ctrlFile);
		}
		else
		{
			puts(docName);
		}
	}
	result = incrementSequence(errorList ,subsGroupTest ,&sequenceNbr);
    if(result!=0)
        printLastError(errorList);
	}

	//Print error messages.
	printAllErrors(errorList);
	
	freeListErrorStruct(errorList);
	exit(0);
}
