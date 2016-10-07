/**
 * File                 : Producer5.2.6.			         
 * Description          : This contains producer flow for Integration test 5.2.6. 
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
#include "ftp/IpdrFtp.H"

int main()
{
	ListErrorStruct* errorList = newListErrorStructure();
	CapabilityFile* capabilityFile ;
	SubscriptionGroupList subscriptionGroupList;
	SubscriptionGroupList* tempSubsList = NULL;
	ListGroupID* tempgroupIDs = NULL;
	int result = 0;
	int index = 0;
	ControlFile* ctrlFile = NULL;
	char docName[100] = "/home/vishal/document-";
    unsigned int docLen = strlen(docName);
	char buffer[200] = "";
	char policySeqNbr[50] = "";
	char rangeFileName[50] = "";
	char controlFileLocalDir[200] = "";
	char controlFileName[50] = "";
	char controlFileLocation[200] = "";
	char rangeFileLocation[200] = "";
	long sequenceNbr = 0;
	char strSeqNbr[50] = "";
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
	capabilityFile = readCapabilityFile(errorList ,"file://mohsaswks15333/share/MyXML.xml");
	if(!capabilityFile)
		printLastError(errorList);
	
        
	//get subscription group with groupId Voip from linked list. 
    result = getGroup(errorList ,capabilityFile ,"Voip" ,&subsGroupTest);
	if(result == 0)
	{
		printf("match found\n");
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
	if((result = setControlFileLocalDirectory(errorList,subsGroupTest,"c:/share/Voip"))!=0)
		printLastError(errorList);
	if((result = getControlFileLocalDirectory(errorList,subsGroupTest,controlFileLocalDir))!=0)
		printLastError(errorList);
	if((result = getRangeFileName(errorList,subsGroupTest,rangeFileName))!=0)
		printLastError(errorList);
	else{
		strcpy(rangeFileLocation,controlFileLocalDir);
		strcat(rangeFileLocation,rangeFileName);
	}

	if((result = getLastSequenceNbr(errorList,rangeFileLocation,strSeqNbr))!=0)
		printLastError(errorList);
	sequenceNbr = atol(strSeqNbr);
	if(strlen(strSeqNbr)!=0){
		if((result = incrementSequence(errorList ,subsGroupTest ,&sequenceNbr))!=0)
			printLastError(errorList);
	}

	result = getControlFileName(errorList,subsGroupTest,controlFileName,sequenceNbr);
	if(result!=0)
		printLastError(errorList);
	strcpy(controlFileLocation,controlFileLocalDir);
	strcat(controlFileLocation,controlFileName);
//writing to control file at its location
	if((ctrlFile = controlFileFromLocation(errorList,controlFileLocation))==NULL)
		printLastError(errorList);
	else{
		writeStart(errorList,ctrlFile);
		for(index = 0;index <100000 ; index++)
		{
			sprintf(buffer,"%d",index);
			strcat(docName,buffer);
			if((result=addDocName(errorList,ctrlFile,docName))!=0)
				break;
			puts(docName);
			docName[docLen] = '\0';
			buffer[0] = 0;
		}
		writeEnd(errorList,ctrlFile);
		freeControlFile(ctrlFile);
		if((result = getSequenceBasedOnPolicy(errorList ,subsGroupTest ,sequenceNbr ,policySeqNbr))!=0)
			printLastError(errorList);
		result = setLastSequenceNbr(errorList,rangeFileLocation,policySeqNbr);
		if(result!=0)
			printLastError(errorList);
	}

	//Print error messages.
	printAllErrors(errorList);
	
	freeListErrorStruct(errorList);
	exit(0);
}
