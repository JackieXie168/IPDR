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
#include "ipdrInterface/IPDRFTPInterface.h"
	

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
	char docName[200] = "/home/girishmd/c_ipdrftp_1.0.0/test/Voip/Test-";
	char controlFileLocalDir[200] =  "";
	char groupId[50] ="";
	char controlFilePrefix[200] = "";
	char controlFileSuffix[200] = "";
	char controlFileNamePolicy[100] = "";
	char buffer[200] = "";
	char maxSeqNbr[50] = "";
	char policySeqNbr[50] = "";
	char rangeFileName[50] = "";
	char rangeFileURL[200] = "";
	char controlFileName[100] = "";
	char controlFileURL[200] = "";
	char controlFileLocation[200] = "";
	char rangeFileLocation[200] = "";
	long sequenceNbr = 0;
	char strSeqNbr[50] = "";
	char capabilityFileURL[200] = "";
	int length = 0;
	SubscriptionGroup* subsGroupTest;//for subscription group API's testing

//To check genaration of XML
	//char capabilityFileXMLString[2000] = "";
	char capabilityFileXMLString[200000] = "";
	char** inputFiles = (char**)malloc(sizeof(char*));
	//inputFiles[0] = "C:/ipdrftp@28032002/Windows/test/testSM.csv";
	inputFiles[0] = "/home/girishmd/c_ipdrftp_1.0.0/test/testData/testVOIP.csv";
	//inputFiles[1] = "C:/IPDR_FTP_Deliverable/IPDR_FTP_LIB_C/testWS.csv";
	inputFiles[1] = NULL;

//Test 5.1.1.1
	//Test Case 1
	//capabilityFile = readCapabilityFile(errorList ,"file://mohsaswks15333/share/MyXML.xml");
	//capabilityFile = readCapabilityFile(errorList ,"file://blrkec32567d/home/girishmd/c_ipdrftp_1.0.0/test/testData/Capability.xml");
	//capabilityFile = readCapabilityFile(errorList ,"file://172.21.19.68/home/girishmd/c_ipdrftp_1.0.0/test/testData/Capability.xml");
	capabilityFile = readCapabilityFile(errorList ,"file:///home/girishmd/c_ipdrftp_1.0.0/test/testData/Capability.xml");
	if(!capabilityFile)
		printLastError(errorList);
	
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

//generation of XML file
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

//Test 5.1.2.3 :- Get rangeFileName()
	//if((result = setControlFileLocalDirectory(errorList,subsGroupTest,"c:/share/Voip/"))!=0)
	//if((result = setControlFileLocalDirectory(errorList,subsGroupTest,"file://172.21.19.68/home/girishmd/c_ipdrftp_1.0.0/test/Voip/"))!=0)
	if((result = setControlFileLocalDirectory(errorList,subsGroupTest,"file:///home/girishmd/c_ipdrftp_1.0.0/test/Voip/"))!=0)
		printLastError(errorList);
	if((result = getControlFileLocalDirectory(errorList,subsGroupTest,controlFileLocalDir))!=0)
		printLastError(errorList);
	else
		printf("ControlFileLocalDirectory=%s\n",controlFileLocalDir);

	//Test Case 1
	if((result = getRangeFileName(errorList,subsGroupTest,rangeFileName))!=0)
		printLastError(errorList);
	else{
		strcpy(rangeFileLocation,controlFileLocalDir);
		strcat(rangeFileLocation,rangeFileName);
		printf("rangeFileLocation=%s\n",rangeFileLocation);
	}

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

//Test 5.1.2.7 :- Increment Sequence
	if((result = getFirstSequenceNbr(errorList,rangeFileLocation,strSeqNbr))!=0)
		printLastError(errorList);
	else
		printf("FirstSequenceNbr=%s\n",strSeqNbr);


//Test 5.1.3.2 :- get last sequence number
	if((result = getLastSequenceNbr(errorList,rangeFileLocation,strSeqNbr))!=0)
		printLastError(errorList);

//Test 5.1.4.1 :- Testing write operations on control file

	if((result = getLastSequenceNbr(errorList,rangeFileLocation,strSeqNbr))!=0)
		printLastError(errorList);

	sequenceNbr = atol(strSeqNbr);
	if(strlen(strSeqNbr)!=0){
		if((result = incrementSequence(errorList ,subsGroupTest ,&sequenceNbr))!=0)
			printLastError(errorList);
	}
	getControlFileName(errorList,subsGroupTest,controlFileName,sequenceNbr);
	//strcpy(controlFileLocation,controlFileLocalDir);
	strcpy(controlFileLocation,"/home/girishmd/c_ipdrftp_1.0.0/test/Voip/");
	strcat(controlFileLocation,controlFileName);
	
	if((ctrlFile = controlFileFromLocation(errorList,controlFileLocation))==NULL)
		printLastError(errorList);
	else{
		writeStart(errorList,ctrlFile);
	
		for(index = 0;index <50 ; index++)
		{
			sprintf(buffer,"%d",index);
			length = strlen(docName);
			strcat(docName,buffer);
			testWriteFTP(1,inputFiles,docName);
			if((result=addDocName(errorList,ctrlFile,docName))!=0)
				break;
			docName[length] = '\0';
			buffer[0] = 0;
		}

		writeEnd(errorList,ctrlFile);
		freeControlFile(ctrlFile);

		//set last sequence number
		if((result = getSequenceBasedOnPolicy(errorList ,subsGroupTest ,sequenceNbr ,policySeqNbr))!=0)
			printLastError(errorList);
	strcpy(rangeFileLocation,"/home/girishmd/c_ipdrftp_1.0.0/test/Voip/Voip-range-file");
		result = setLastSequenceNbr(errorList,rangeFileLocation,policySeqNbr);
		if(result!=0)
			printLastError(errorList);
	}

	//Print error messages.
	printAllErrors(errorList);
	
	freeListErrorStruct(errorList);
	exit(0);
}
