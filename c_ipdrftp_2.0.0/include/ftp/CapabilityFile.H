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
 * File                 : IPDRCommon.h                *
 * Description          : Contains All CapabilityFile *
 *                        function declarations       *
 * Author               : Infosys Tech Ltd            *
 * Modification History :                             *
 *----------------------------------------------------*
 * Date       Name        Change/Description          *
 *----------------------------------------------------*
 * 01/29/02   Infosys      Created                    *
 *
 *
 */

#ifndef _CAPABILITYFILE_H_
#define _CAPABILITYFILE_H_

#include "ftp/FtpStructures.H"
#include "ftp/SubscriptionGroup.H"
#include "expat/expat.h"
#include "ftp/GetFileFromURL.H"

CapabilityFile* newCapabilityFile(ListErrorStruct* pErrorList);
								  

SubscriptionGroupList* 
newSubscriptionGroupList(ListErrorStruct* pErrorList,
						 SubscriptionGroup* pSubscriptionGroup);

int freeCapabilityFile(CapabilityFile* pCapabilityFile);

int freeSubscriptionGroupList(SubscriptionGroupList* subsGroupList);

int addGroup(ListErrorStruct* pErrorList ,
			 CapabilityFile* pCapabilityFile ,
			 SubscriptionGroup* pSubscriptionGroup);//tested

int deleteGroup(ListErrorStruct* pErrorList ,
				CapabilityFile* pCapabilityFile ,
				char* groupID );//tested

int getAllGroups(ListErrorStruct* pErrorList ,
				 CapabilityFile* pCapabilityFile, 
				 SubscriptionGroupList* pSubscriptionGroupList);//tested

int getGroup(ListErrorStruct* pErrorList ,
			 CapabilityFile* pCapabilityFile, 
			 char* groupID, 
			 SubscriptionGroup** pSubscriptionGroup);//tested

int getGroupIDs(ListErrorStruct* pErrorList ,
				CapabilityFile* pCapabilityFile,
				ListGroupID* groupIDs);//tested

int toXML(ListErrorStruct* pErrorList ,
		  CapabilityFile* pCapabilityFile,
		  char*  capabilityFileXMLString);

CapabilityFile* readCapabilityFile(ListErrorStruct* pErrorList ,
							       char* capabilityFileXML);

int createListGroupID(ListGroupID** pFirstListGroupID,
					  char* groupID);

/*Functions for parsing CapabilityFile.xml*/
int parseXMLCapabilityFile(ListErrorStruct* pErrorList ,
						   CapabilityXMLInfo* pCapabilityInfo);
					   //CapabilityFile* pCapabilityFile , 
					   //SubscriptionGroupList** pSubsGroupList);

static void
startElement(void *userData, const char *name, const char **atts);

static void
endElement(void *userData, const char *name);

static void
dataHandler(void *userData, const char *data,int len);


#endif //_CAPABILITYFILE_H_

