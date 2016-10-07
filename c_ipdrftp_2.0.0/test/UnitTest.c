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
 * File                 : IPDR_Producer_Flow.c			         
 * Description          : Contails unit test cases.     
 * Author               : Infosys Tech Ltd           
 * Modification History :                            
 *---------------------------------------------------
 * Date       Name             Change/Description    
 *---------------------------------------------------
 * 01/29/02  Infosys           Created  
 * 
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
	char docName[200] = "Test-";
	char controlFileDir[200] =  "";
	char groupId[50] ="";
	char controlFilePrefix[200] = "";
	char controlFileSuffix[200] = "";
	char controlFileNamePolicy[100] = "";
	char buffer[200] = "";
	char maxSeqNbr[50] = "";
	char policySeqNbr[50] = "";
	char rangeFileName[50] = "";
	char rangeFileURL[200] = "";
	char controlFileLocalDir[200] = "";
	char controlFileName[100] = "";
	char controlFileURL[200] = "";
	char controlFileLocation[200] = "";
	char rangeFileLocation[200] = "";
	long sequenceNbr = 0;
	char strSeqNbr[50] = "";
	char capabilityFileURL[200] = "";
	int length = 0;
	FILE* stream;
	SubscriptionGroup* subsGroupTest;//for subscription group API's testing

//To check genaration of XML
	char capabilityFileXMLString[2000] = "";
	char** inputFiles = (char**)malloc(sizeof(char*));
	inputFiles[0] = "c:/share/testSM.csv";
	//inputFiles[1] = "C:/IPDR_FTP_Deliverable/IPDR_FTP_LIB_C/testWS.csv";
	inputFiles[1] = NULL;

//Test 5.1.1.1
	//Test Case 1
	/*
	printf("Please enter capabilityfile url:");
	scanf("%s",capabilityFileURL);
	capabilityFile = readCapabilityFile(errorList ,capabilityFileURL);
	if(!capabilityFile)
		printLastError(errorList);
	*/
	capabilityFile = readCapabilityFile(errorList ,"file://mohsaswks15333/share/MyXML.xml");
	if(!capabilityFile)
		printLastError(errorList);
	//Test Case 2
	//capabilityFile = readCapabilityFile(errorList ,"http://193.168.122.208/MyXML.xml");
	//Test Case 3
	//capabilityFile = readCapabilityFile(errorList ,"http://192.168.122.208/IPDRWeb/Capability_empty.xml");
	//Test Case 4
	//capabilityFile = readCapabilityFile(errorList,"http://192.168.122.208/ipdr/Capability_NoSubscription.xml"); //Testing newCapabilityFile()
	//Test Case 5
	//capabilityFile = readCapabilityFile(errorList ,"http://192.168.122.208/ipdr/Capability_AttributeMissing.xml");


//Test 5.1.1.2 :- Adding subscription groups to the list
	//Test Case 1
	/*
	result = addGroup(errorList ,capabilityFile ,subscriptionGroup1);
	if(result!=0)
		printLastError(errorList);
	//Test Case 2
	result = addGroup(errorList ,capabilityFile ,&subscriptionGroup2);
	if(result!=0)
		printLastError(errorList);
*/
	//Test Case 3
/*
	if((result = addGroup(errorList ,capabilityFile ,subscriptionGroup1))!=0)
		printf("Subscription group with groupId = %s cannot be added\n",subscriptionGroup3->groupID_);
	if((result = addGroup(errorList ,capabilityFile ,subscriptionGroup1))!=0)
		printf("Subscription group with groupId = %s cannot be added\n",subscriptionGroup3->groupID_);
*/

	//Test Case 4
	//if((result = addGroup(errorList ,capabilityFile ,subscriptionGroup3))!=0)
	//	printf("Subscription group with groupId = %s cannot be added\n",subscriptionGroup3->groupID_);

//Test 5.1.1.3 :- Testing delete a group
	//Test case 1
	/*
	if((result = deleteGroup(errorList ,capabilityFile ,"GPRS"))==0)
		printf("Group Deleted successfully\n");
	else 
		printf("Node to be deleted not found\n");
	//Test case 2
	
	if((result = deleteGroup(errorList ,capabilityFile ,"Voip"))==0)
		printf("Group Deleted successfully\n");
	else 
		printLastError(errorList);

	if((result = deleteGroup(errorList ,capabilityFile ,"GSM"))==0)
		printf("Group Deleted successfully\n");
	else 
		printLastError(errorList);

	if((result = deleteGroup(errorList ,capabilityFile ,"GPRS"))==0)
		printf("Group Deleted successfully\n");
	else 
		printLastError(errorList);

	//Test case 3
	if((result = deleteGroup(errorList ,capabilityFile ,"GSM")==0))
		printf("Group Deleted successfully\n");
	else 
		printf("Node to be deleted not found\n");
*/
//Test 5.1.1.4 :- Testing for getGroup from the list.
	//Test Case 1
	//result = getGroup(errorList ,capabilityFile ,"SAN" ,&subsGroupTest);
	//Test Case 2
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
	//Test Case 2
	//result = getGroupIDs(errorList ,capabilityFile ,&groupIDs);
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

//Test 5.1.1.6 :- Testing generation of XML file
	//Test Case 1
	//result = toXML(errorList ,capabilityFile ,capabilityFileXMLString);
	//Test Case 2
	result = toXML(errorList ,capabilityFile ,capabilityFileXMLString);
	if(result ==0)
		puts(capabilityFileXMLString);
	else
		printLastError(errorList);
	

//Test 5.1.1.7 :- Testing for retrieving all subscription groups.
	//Test Case 1
	result = getAllGroups(errorList ,capabilityFile ,&subscriptionGroupList);
	//Test Case 2
	//result = getAllGroups(errorList ,capabilityFile ,&subscriptionGroupList);
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

//Test 5.1.2.1 :- Testing get operations on subscription group.
	//Test Case 1
	
	if((result = getGroupID(errorList,subsGroupTest,groupId))==0)
		printf("GroupID=%s\n",groupId);
	if((getControlFileDirectory(errorList,subsGroupTest,controlFileDir))==0)
		printf("ControlFileDirectory=%s\n",controlFileDir);
	if((getControlFilePrefix(errorList,subsGroupTest,controlFilePrefix))==0)
		printf("ControlFilePrefix=%s\n",controlFilePrefix);
	if((getControlFileNamePolicy(errorList,subsGroupTest,controlFileNamePolicy))==0);
		printf("ControlFileNamePolicy=%s\n",controlFileNamePolicy);
	if((getControlFileSuffix(errorList,subsGroupTest,controlFileSuffix))==0)
		printf("ControlFileSuffix=%s\n",controlFileSuffix);;
	if((setControlFileLocalDirectory(errorList ,subsGroupTest ,"c:/share/Voip"))!=0)
		printLastError(errorList);
	if((getControlFileLocalDirectory(errorList,subsGroupTest,controlFileLocalDir))==0)
		printf("ControlFileLocalDirectory=%s\n",controlFileLocalDir);
	
	
//Test 5.1.2.2 :- Testing set operations
	//Test Case 1
	/*
	
	if((setGroupID(errorList ,subsGroupTest ,"SAN-Chennai"))!=0)
		printLastError(errorList);
	if((setControlFileDirectory(errorList ,subsGroupTest ,"infosys/chennai/caps/"))!=0)
		printLastError(errorList);
	if((setControlFilePrefix(errorList ,subsGroupTest ,"Sholing"))!=0)
		printLastError(errorList);
	if((setControlFileNamePolicy(errorList ,subsGroupTest ,"NNNNNN"))!=0)
		printLastError(errorList);
	if((setControlFileSuffix(errorList ,subsGroupTest ,".csg-chennai"))!=0)
		printLastError(errorList);
	if((setControlFileLocalDirectory(errorList ,subsGroupTest ,"c:/share/Voip"))!=0)
		printLastError(errorList);
	*/
	//Test Case 2
	/*
	if((setGroupID(errorList ,subsGroupTest ,""))!=0)
		printLastError(errorList);
	if((setControlFileDirectory(errorList ,subsGroupTest ,""))!=0)
		printLastError(errorList);
	if((setControlFilePrefix(errorList ,subsGroupTest ,""))!=0)
		printLastError(errorList);
	if((setControlFileNamePolicy(errorList ,subsGroupTest ,""))!=0)
		printLastError(errorList);
	if((setControlFileSuffix(errorList ,subsGroupTest ,""))!=0)
		printLastError(errorList);
	if((setControlFileDirectoryURL(errorList ,subsGroupTest ,""))!=0)
		printLastError(errorList);
	*/

//Test 5.1.2.3 :- Get rangeFileName()
	if((result = getControlFileDirectory(errorList,subsGroupTest,controlFileDir))!=0)
		printLastError(errorList);
	else
		printf("ControlFileDirectory=%s\n",controlFileDir);

	//Test Case 1
	if((result = getRangeFileName(errorList,subsGroupTest,rangeFileName))!=0)
		printLastError(errorList);
	else{
		strcpy(rangeFileURL,controlFileDir);
		strcat(rangeFileURL,rangeFileName);
		strcpy(rangeFileLocation,controlFileLocalDir);
		strcat(rangeFileLocation,rangeFileName);
		printf("rangeFileURL=%s\n",rangeFileURL);
		printf("rangeFileLocation=%s\n",rangeFileLocation);
	}

	
//Test 5.1.2.4 :- Get controlFileURL

	//Test Case 1
	/*
	if((result = getControlFileName(errorList,subsGroupTest,controlFileName,123))==0)
		printf("ControlFileName=%s\n",controlFileName);
	else 
		printLastError(errorList);
		*/
	//Test Case 2
	/*
	setControlFileNamePolicy(errorList ,subsGroupTest ,"NN");
	getControlFileName(errorList,subsGroupTest,buffer,123);
	//Test Case 3
	setControlFileNamePolicy(errorList ,subsGroupTest ,"NNN");
	getControlFileName(errorList,subsGroupTest,buffer,123);
	*/

//Test 5.1.2.5 :- Testing getMaxSequenceNumber()
	//Test Case 1
	if((result = getMaxSequenceNumber(errorList ,subsGroupTest ,maxSeqNbr))!=0)
		printLastError(errorList);
	else
		printf("MaxSequenceNumber=%s\n",maxSeqNbr);

//Test 5.1.2.6 :- Testing getSequenceBasedOnPolicy
	//Test Case 1
	if((result = getSequenceBasedOnPolicy(errorList ,subsGroupTest ,sequenceNbr ,policySeqNbr))!=0)
		printLastError(errorList);
	else 
		printf("SequenceBasedOnPolicy=%s\n",policySeqNbr);

	//Test Case 2
	//setControlFileNamePolicy(errorList ,subsGroupTest ,"NN");
	//result = getSequenceBasedOnPolicy(errorList ,subsGroupTest ,123 ,policySeqNbr);	
	//if((result = setControlFileNamePolicy(errorList ,subsGroupTest ,"NN"))!=0)
	//	printLastError(errorList);
//Test 5.1.2.7 :- Increment Sequence
	/*
	//Test Case 1
	sequenceNbr = 11;
	result = incrementSequence(errorList ,subsGroupTest ,&sequenceNbr);
	//Test Case 2
	//sequenceNbr = 99;
	result = incrementSequence(errorList ,subsGroupTest ,&sequenceNbr);
	//Test Case 3
	sequenceNbr = 1000;
	result = incrementSequence(errorList ,subsGroupTest ,&sequenceNbr);
	*/
	
//Test 5.1.2.7 :- Decrement Sequence
	/*
	//Test Case 1
	sequenceNbr = 11;
	result = decrementSequence(errorList ,subsGroupTest ,&sequenceNbr);
	//Test Case 2
	sequenceNbr = 0;
	result = decrementSequence(errorList ,subsGroupTest ,&sequenceNbr);
	//Test Case 3
	sequenceNbr = 100;
	result = decrementSequence(errorList ,subsGroupTest ,&sequenceNbr);
	*/

//Test 5.1.3.1 :- get first sequence number
	//Test Case 1
	//Test Case 2
	/*
	if((result = getFirstSequenceNbr(errorList,rangeFileURL,strSeqNbr))!=0)
		printLastError(errorList);
	else
		printf("FirstSequenceNbr=%s\n",strSeqNbr);
		*/
	if((result = getFirstSequenceNbr(errorList,rangeFileLocation,strSeqNbr))!=0)
		printLastError(errorList);
	else
		printf("FirstSequenceNbr=%s\n",strSeqNbr);


//Test 5.1.3.2 :- get last sequence number
	//Test Case 1
	//Test Case 2
	/*
	if((result = getLastSequenceNbr(errorList,rangeFileURL,strSeqNbr))!=0)
		printLastError(errorList);
	*/
	if((result = getLastSequenceNbr(errorList,rangeFileLocation,strSeqNbr))!=0)
		printLastError(errorList);

//Test 5.1.3.3 :- set first sequence number
	//Test Case 1
	/*
	if((result = setFirstSequenceNbr(errorList,rangeFileLocation,"34"))!=0)
		printLastError(errorList);
		*/
	//Test Case 2
	//result = setFirstSequenceNbr(errorList,buffer,"");
	
	//result = getLastSequenceNbr(errorList,buffer,strSeqNbr);

//Test 5.1.3.4 :- set last sequence number
	//Test Case 1
	//result = setLastSequenceNbr(errorList,buffer,"87");
	//Test Case 2
	//result = setLastSequenceNbr(errorList,buffer,"");

//Test 5.1.4.1 :- Testing write operations on control file

	getControlFileLocalDirectory(errorList,subsGroupTest,controlFileLocalDir);
	strcpy(rangeFileLocation,controlFileLocalDir);
	strcat(rangeFileLocation,rangeFileName);
	if((result = getLastSequenceNbr(errorList,rangeFileLocation,strSeqNbr))!=0)
		printLastError(errorList);

	sequenceNbr = atol(strSeqNbr);
	getControlFileName(errorList,subsGroupTest,controlFileName,sequenceNbr);
	strcpy(controlFileLocation,controlFileLocalDir);
	strcat(controlFileLocation,controlFileName);
	//Test Case 1
/*	
	if((ctrlFile = controlFileFromLocation(errorList,controlFileLocation))==NULL)
		printLastError(errorList);

	writeStart(errorList,ctrlFile);
	
	for(index = 0;index <50 ; index++)
	{
		sprintf(buffer,"%d",index);
		length = strlen(docName);
		strcat(docName,buffer);
		//testWriteFTP(1,inputFiles,docName);
		if((result=addDocName(errorList,ctrlFile,docName))!=0)
			break;
		docName[length] = '\0';
		buffer[0] = 0;
	}

	writeEnd(errorList,ctrlFile);
	freeControlFile(ctrlFile);

*/
//Test 5.1.4.2 :- Testing read operations from control file
	//Test Case 1
	//Test Case 2
	strcpy(controlFileURL,controlFileDir);
	strcat(controlFileURL,controlFileName);
	printf("controlFileURL=%s\n",controlFileURL);
	ctrlFile = controlFileFromURL(errorList,controlFileURL);
	while(isEnd(errorList,ctrlFile)!=1){
		if((result=getDocName(errorList,ctrlFile,docName))!=0)
			break;
		if(strcmp(docName,"NULL")==0)
		{
			refreshControlFile(errorList,ctrlFile);
		}
		else
			puts(docName);
	}
	
	//Testing getFileStreamFromURL().
	
	stream = getFileStreamFromURL(errorList,"http://192.168.122.208/ipdr/Capability_Test.xml","YourXML.xml");
	fgets(capabilityFileXMLString,1000,stream);
	puts(capabilityFileXMLString);
	fclose(stream);
	

	//Print error messages.
	printAllErrors(errorList);
	
	freeListErrorStruct(errorList);
	exit(0);
}
