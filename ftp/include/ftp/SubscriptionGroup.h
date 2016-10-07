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
 * File                 : SubscriptionGroup.H            
 * Description          : Contains All SubscriptionGroup 
 *                        function declarations.         
 * Author               : Infosys Tech Ltd               
 * Modification History :                                
 *-------------------------------------------------------
 * Date       Name        Change/Description             
 *-------------------------------------------------------
 * 01/29/02   Infosys      Created                       
 *
 */

#ifndef _SUBSCRIPTIONGROUP_H_
#define _SUBSCRIPTIONGROUP_H_

#include"ftp/FtpStructures.H"

SubscriptionGroup* 
newSubscriptionGroup(ListErrorStruct* pErrorList,
					 char* groupID,
					 char* controlFileDirectory,
					 char* controlFilePrefix,
					 char* controlFileNamePolicy,
					 char* controlFileSuffix);

int freeSubscriptionGroup(SubscriptionGroup* subscriptionGroup);

int getGroupID(ListErrorStruct* pErrorList ,
			   SubscriptionGroup* pSubsGroup,
			   char* groupID);

int getControlFileDirectory(ListErrorStruct* pErrorList ,
							SubscriptionGroup* pSubsGroup,
							char* controlFileDir);

int getControlFilePrefix(ListErrorStruct* pErrorList ,
						 SubscriptionGroup* pSubsGroup,
						 char* controlFilePrefix);

int getControlFileNamePolicy(ListErrorStruct* pErrorList ,
							 SubscriptionGroup* pSubsGroup,
							 char* namePolicy);

int getControlFileSuffix(ListErrorStruct* pErrorList ,
						 SubscriptionGroup* pSubsGroup,
						 char* fileSuffix);

int getControlFileLocalDirectory(ListErrorStruct* pErrorList ,
						 SubscriptionGroup* pSubsGroup,
						 char* localDir);

int getHomeServiceProviderType(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int homeServiceProviderType);

int getHomeServiceProvider(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* homeServiceProvider);

int getHomeServiceProviderName(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* homeServiceProviderName);

int getAccessProviderType(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int accessProviderType);

int getAccessProvider(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* accessProvider);

int getAccessProviderName(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* accessProviderName);

int getContractReference(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* contractReference);

int getBusinessRelationshipID(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* businessRelationshipID);

int getSummaryXchgHrs(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int summaryXchgHrs);

int getSettlementXcghDays(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* settlementXcghDays);

int getNetPosXchgDays(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* netPosXchgDays);

int getEffectiveDateTime(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* effectiveDateTime);

int getResponse(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int response);

int getXchgProtocol(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int xchgProtocol);

int getXchgSLA(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int xchgSLA);

int getXchgEncoding(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int xchgEncoding);

int getDisputeResolution(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int disputeResolution);

int getLocationInfo(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* locationInfo);

int getBRContactName(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* BRContactName);

int getRejectReturn(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int rejectReturn);

int getFrequencyOfAuditReconciliation(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int frequencyOfAuditReconciliation);

int getXchgCurrencyType(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* xchgCurrencyType);

int getUnitOfMeasure(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int unitOfMeasure);

int getChargeableUnit(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int chargeableUnit);

int getRatePerChargeableUnit(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int ratePerChargeableUnit);

int getClearingIntermediaryName(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* clearingIntermediaryName);

int getBillingIntermediaryName(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* billingIntermediaryName);

int getSendingServiceProviderDisposition(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int sendingServiceProviderDisposition);

int getLastChangedDateTime(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* lastChangedDateTime);	

int getFileRetentionDays(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int fileRetentionDays);

int setGroupID(ListErrorStruct* pErrorList ,
			   SubscriptionGroup* pSubsGroup,
			   char* groupID);

int setControlFileDirectory(ListErrorStruct* pErrorList ,
							SubscriptionGroup* pSubsGroup,
							char* controlFileDirectory);

int setControlFilePrefix (ListErrorStruct* pErrorList ,
						  SubscriptionGroup* pSubsGroup,
						  char* controlFilePrefix);

int setControlFileNamePolicy(ListErrorStruct* pErrorList ,
							 SubscriptionGroup* pSubsGroup,
							 char* controlFileNamePolicy);

int setControlFileSuffix(ListErrorStruct* pErrorList ,
						 SubscriptionGroup* pSubsGroup,
						 char* controlFileSuffix);

int setControlFileLocalDirectory(ListErrorStruct* pErrorList ,
								SubscriptionGroup* pSubsGroup,
								 char* localDir);

int setHomeServiceProviderType(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int homeServiceProviderType);

int setHomeServiceProvider(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* homeServiceProvider);

int setHomeServiceProviderName(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* homeServiceProviderName);

int setAccessProviderType(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int accessProviderType);

int setAccessProvider(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* accessProvider);

int setAccessProviderName(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* accessProviderName);

int setContractReference(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* contractReference);

int setBusinessRelationshipID(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* businessRelationshipID);

int setSummaryXchgHrs(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int summaryXchgHrs);

int setSettlementXcghDays(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* settlementXcghDays);

int setNetPosXchgDays(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* netPosXchgDays);

int setEffectiveDateTime(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* effectiveDateTime);

int setResponse(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int response);

int setXchgProtocol(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int xchgProtocol);

int setXchgSLA(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int xchgSLA);

int setXchgEncoding(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int xchgEncoding);

int setDisputeResolution(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int disputeResolution);

int setLocationInfo(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* locationInfo);

int setBRContactName(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* BRContactName);

int setRejectReturn(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int rejectReturn);

int setFrequencyOfAuditReconciliation(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int frequencyOfAuditReconciliation);

int setXchgCurrencyType(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* xchgCurrencyType);

int setUnitOfMeasure(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int unitOfMeasure);

int setChargeableUnit(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int chargeableUnit);

int setRatePerChargeableUnit(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int ratePerChargeableUnit);

int setClearingIntermediaryName(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* clearingIntermediaryName);

int setBillingIntermediaryName(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* billingIntermediaryName);

int setSendingServiceProviderDisposition(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int sendingServiceProviderDisposition);

int setLastChangedDateTime(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							char* lastChangedDateTime);	

int setFileRetentionDays(ListErrorStruct* pErrorList ,
						 	SubscriptionGroup* pSubsGroup,
							int fileRetentionDays);

int getRangeFileName (ListErrorStruct* pErrorList ,
					  SubscriptionGroup* pSubsGroup,
					  char* rangeFileName);

int getControlFileName(ListErrorStruct* pErrorList,
					   SubscriptionGroup* pSubsGroup,
					   char* controlFileName,
					   long sequenceNbr);

int getMaxSequenceNumber(ListErrorStruct* pErrorList ,
						 SubscriptionGroup* pSubsGroup,
						 char* maxSequenceNumber);

int getSequenceBasedOnPolicy(ListErrorStruct* pErrorList ,
							 SubscriptionGroup* pSubsGroup,
							 long seqNbr ,
							 char* policyBasedSeqNbr);

int incrementSequence(ListErrorStruct* pErrorList,
					  SubscriptionGroup* pSubsGroup,
					  long* sequenceNbr);

int decrementSequence(ListErrorStruct* pErrorList,
					  SubscriptionGroup* pSubsGroup,
					  long* sequenceNbr);

SubscriptionGroup* 
newSubscriptionGroupWithBrRecord(ListErrorStruct* pErrorList,
					 char* groupID,
					 char* controlFileDirectory,
					 char* controlFilePrefix,
					 char* controlFileNamePolicy,
					 char* controlFileSuffix,
                BRRecord brRecord);

#endif //_SUBSCRIPTIONGROUP_H_
