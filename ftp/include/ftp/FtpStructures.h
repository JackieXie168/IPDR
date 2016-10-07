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
 * File                 : FtpStructures.H            
 * Description          : Contains All Data Structures
 * Author               : Infosys Tech Ltd            
 * Modification History :                             
 *----------------------------------------------------
 * Date       Name        Change/Description          
 *----------------------------------------------------
 * 01/29/02   Infosys      Created                    
 *
 */


#ifndef _FTP_STRUCTURES_H_
#define _FTP_STRUCTURES_H_

#include<stdio.h> 
#include<string.h>
#include<stdlib.h>

/*! 
 * Defining Constant for Version in a ControlFile
 */
#define VERSION_NUMBER      "Version 3" 
#define RANGEFILE_SUFFIX    "-range-file" 


/*! 
 * Defining Constants for RollOver Policy
 */
#define DOC_COUNT_POLICY 1
#define TIME_POLICY 0


/*!
 * Defining Constants for size of string variables
 */
#define MAX_CONTROLFILE_NAME_POLICY 	40		
#define MAX_SEQNBR_LEN  			    40
#define MAX_RANGEFILE_CONTENTS 			100
#define MAX_VERSION_LEN     			30
#define MAX_DOCUMENT_NAME			    200
#define BUFSIZE							8192

 
/*!
 * Defining Constants for path of local files
 */                 
#define LOCAL_RANGEFILE            "xRangeFileX.txt"
#define LOCAL_CONTROLFILE		   "xControlFileX.txt" 	
#define LOCAL_CAPABILITYFILE       "xCapabilityFileX.xml" 

/*!
 * Defining Constants capability XML file.
 */  
#define DATA_PRIMITIVEITEM			"Pull"
#define VALUE_PROTOCOLMAPPING		"File"
#define ATTR_PROTOCOLMAPPING		"protocolMapping"
#define ELEMENT_PRIMITIVEITEM		"primitiveItem"
#define ELEMENT_GROUPINFOITEM		"groupInfoItem"
#define ELEMENT_GROUPID				"groupId"
#define ELEMENT_CTRLDIR				"controlFileDirectory"
#define ELEMENT_CTRLPREFIX			"controlFilePrefix"
#define ELEMENT_CTRLNAMEPOLICY		"controlFileNamePolicy"
#define ELEMENT_CTRLSUFFIX			"controlFileSuffix"

#define ELEMENT_BRRECORD										"BRRecord"
#define ELEMENT_HOMESERVICEPROVIDERTYPE					"homeServiceProviderType"
#define ELEMENT_HOMESERVICEPROVIDER							"homeServiceProvider"
#define ELEMENT_HOMESERVICEPROVIDERNAME							"homeServiceProviderName"
#define ELEMENT_ACCESSPROVIDERTYPE							"accessProviderType"
#define ELEMENT_ACCESSPROVIDER							"accessProvider"
#define ELEMENT_ACCESSPROVIDERNAME							"accessProviderName"
#define ELEMENT_CONTRACTREFERENCE							"contractReference"
#define ELEMENT_BUSINESSRELATIONSHIPID							"businessRelationshipID"
#define ELEMENT_SUMMARYXCHGHRS							"summaryXchgHrs"
#define ELEMENT_SETTLEMENTXCGHDAYS							"settlementXcghDays"
#define ELEMENT_NETPOSXCHGDAYS							"netPosXchgDays"
#define ELEMENT_EFFECTIVEDATETIME							"effectiveDateTime"
#define ELEMENT_RESPONSE							"response"
#define ELEMENT_XCHGPROTOCOL							"xchgProtocol"
#define ELEMENT_XCHGSLA							"xchgSLA"
#define ELEMENT_XCHGENCODING							"xchgEncoding"
#define ELEMENT_DISPUTERESOLUTION							"disputeResolution"
#define ELEMENT_LOCATIONINFO							"locationInfo"
#define ELEMENT_BRCONTACTNAME							"BRContactName"
#define ELEMENT_REJECTRETURN							"rejectReturn"
#define ELEMENT_FREQUENCYOFAUDITRECONCILIATION							"frequencyOfAuditReconciliation"
#define ELEMENT_XCHGCURRENCYTYPE							"xchgCurrencyType"
#define ELEMENT_UNITOFMEASURE							"unitOfMeasure"
#define ELEMENT_CHARGEABLEUNIT							"chargeableUnit"
#define ELEMENT_RATEPERCHARGEABLEUNIT							"ratePerChargeableUnit"
#define ELEMENT_CLEARINGINTERMEDIARYNAME							"clearingIntermediaryName"
#define ELEMENT_BILLINGINTERMEDIARYNAME							"billingIntermediaryName"
#define ELEMENT_SENDINGSERVICEPROVIDERDISPOSITION							"sendingServiceProviderDisposition"
#define ELEMENT_LASTCHANGEDDATETIME							"lastChangedDateTime"
#define ELEMENT_FILERETENTIONDAYS							"fileRetentionDays"

typedef struct BRRecord{
	int homeServiceProviderType_;
	char* homeServiceProvider_;
	char* homeServiceProviderName_;
	int accessProviderType_;
	char* accessProvider_;
	char* accessProviderName_;
	char* contractReference_;
	char* businessRelationshipID_;
	int summaryXchgHrs_;
	char* settlementXcghDays_;
	char* netPosXchgDays_;
	char* effectiveDateTime_;
	int response_;
	int xchgProtocol_;
	int xchgSLA_;
	int xchgEncoding_;
	int disputeResolution_;
	char* locationInfo_;
	char* BRContactName_;
	int rejectReturn_;
	int frequencyOfAuditReconciliation_;
	char* xchgCurrencyType_;
	int unitOfMeasure_;
	int chargeableUnit_;
	int ratePerChargeableUnit_;
	char* clearingIntermediaryName_;
	char* billingIntermediaryName_;
	int sendingServiceProviderDisposition_;
	char* lastChangedDateTime_;	
	int fileRetentionDays_;
}BRRecord;

/*! 
 * Defining Structure of SubscriptionGroup
 */
typedef struct SubscriptionGroup{
	char*   groupID_;
	char*   controlFileDirectory_;
	char*   controlFilePrefix_;
	char*   controlFileNamePolicy_;
	char*   controlFileSuffix_;
	char*   controlFileLocalDirectory_;
   int     brRecordFlag_;
	BRRecord brRecord_; 
}SubscriptionGroup;


/*! 
 * Defining Structure of SubscriptionGroupList
 */
typedef struct SubscriptionGroupList{             
	SubscriptionGroup              subscriptionGroup_; 
	struct SubscriptionGroupList*  nextSubscriptionGroup_;
}SubscriptionGroupList;


/*! 
 * Defining Structure of CapabilityFile
 */
typedef struct CapabilityFile{           
	SubscriptionGroupList*  subscriptionGroupList_;   
	char*                   capabilityFileURL_;          
}CapabilityFile;


/*! 
 * Defining Structure of ControlFile
 */
typedef struct ControlFile{
	char*   controlFileURL_;
	char*   controlFileLocation_;
	int     docCount_;
	int     countNoOfVer_; 
	FILE*   controlFilePointer_;
//	int     controlFileOffset_;	
} ControlFile;


/*! 
 * Defining Structure of ListGroupID
 */
typedef struct ListGroupID{    
	char*               groupID_;  
	struct ListGroupID* nextGroupID_;
}ListGroupID;


/*! 
 * Defining Structure of ErrorStructure
 */
typedef struct ErrorStruct{
	int   errorCode_;
	char* functionName_;
	char* errorMessage_;
}ErrorStruct;

/*! 
 * Defining Structure of Attribute Descriptor
 */
typedef struct ListErrorStruct{     
	struct ErrorStruct*     errorStructure_;    
	struct ListErrorStruct* nextErrorStructureList_;
}ListErrorStruct;

typedef struct CapabilityXMLInfo{
	char					protocolMapping_[20];
	char					primitiveItemValue_[20];
	SubscriptionGroupList*  subsGroupList_;
}CapabilityXMLInfo;

#endif //_FTP_STRUCTURES_H_
