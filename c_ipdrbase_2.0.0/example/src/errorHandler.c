/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/



/*****************************************************
 * File                 : errorHandler  		     *
 * Description          :                            *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 *                                                   *
 *****************************************************/

#include "utils/errorHandler.h"
#include "utils/errorCode.h"
#include "expat/expat.h"


/*!
 *  NAME: 
 *      freeListErrorStructure() - for memory deallocation.
 *
 *  DESCRIPTION:
 *      1. This function has to append new node to error list. 
 *	
 *  ARGUMENTS:
 *       -	This function returns ErrorStructure, appending it to 
 *          listErrorStructure (part of IPDRCommonParameters).
 *			{IN/OUT}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


void freeListErrorStructure(ListErrorStructure** pHeadRef)
{
   /*!
	* To free the memory allocated for a particular Error Structure
	*/
	ListErrorStructure* current;
	ListErrorStructure* pNext_;

	current = *pHeadRef ;

	if (current == NULL)
		return;

    while (current != NULL) {
        pNext_ = current->pNext_;
		
	    free(current->ErrStruct_.errorMessage_);
	    free(current->ErrStruct_.functionName_);

        free(current);
        current = pNext_;
    }
    if (current != NULL) free(current);
	return;
}

/*!
 *  NAME: 
 *      addListErrorStructure() - To add a new node to error list
 *
 *  DESCRIPTION:
 *      1. This function adds a new node to error list. 
 *	
 *  ARGUMENTS:
 *       -	This function returns ErrorStructure, appending it to 
 *          listErrorStructure (part of IPDRCommonParameters).
 *			{IN/OUT}
 *		 -	errorCode {IN}
 *		 -	functionName {IN}
 *		 -	offsetValue {IN}  {It is applicable in case of readIPDR, 
 *		    otherwise it will have value zero.}
 *		 -	recordCount {IN} { It is applicable in case of 
 *		    writeIPDR, otherwise it will have value zero.}
 *       -  error message.
 *       -  error severity.
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


int addListErrorStructure(
						 ListErrorStructure** pHeadRef, 
						 int errorCode, 
						 hyper offsetValue, 
						 char *functionName, 
						 char *errorMessage, 
						 int recordCount, 
						 int errorSeverity
						 ) 
{
	unsigned int lenFunctionName = 0;
	ListErrorStructure* newlistErrorStructure = 
	 (ListErrorStructure *) calloc (1, sizeof(struct ListErrorStructure));

	if (newlistErrorStructure == NULL) {
		return (IPDR_OK);	
	}

	if (pHeadRef == NULL) {
		return (IPDR_OK);	
	}
	
	newlistErrorStructure->ErrStruct_.errorCode_ = errorCode;
	newlistErrorStructure->ErrStruct_.errorMessage_ = (char *)
		                               calloc(MAX_ERROR_MESSAGE, sizeof(char));
	if (newlistErrorStructure->ErrStruct_.errorMessage_ == NULL) {
		return (IPDR_OK);	
	}

	if (errorMessage == NULL) {
		errorMessage = (char *) calloc (MAX_ERROR_MESSAGE, sizeof(char));
		strcpy(errorMessage, IPDR_NULL);
	}
	strcpy(newlistErrorStructure->ErrStruct_.errorMessage_, 
		                                   errorMessage);
	newlistErrorStructure->ErrStruct_.errorSeverity_ = 
		errorSeverity;
	
	if (functionName == NULL) {
		lenFunctionName = strlen(IPDR_NULL) + 1;
		functionName = (char *) calloc (lenFunctionName, sizeof(char));
		strcpy(functionName, IPDR_NULL);
	} 

	lenFunctionName = strlen(functionName) + 1;
	newlistErrorStructure->ErrStruct_.functionName_ = (char *)
		                            calloc(lenFunctionName, sizeof(char));
    if (newlistErrorStructure->ErrStruct_.functionName_ == NULL) {
		return (IPDR_OK);	
	}


	strcpy(newlistErrorStructure->ErrStruct_.functionName_, 
		                                         functionName);
	

	newlistErrorStructure->ErrStruct_.offsetValue_ = offsetValue;
	newlistErrorStructure->ErrStruct_.recordCount_ = recordCount;
	
	newlistErrorStructure->pNext_ = *pHeadRef; 
	*pHeadRef = newlistErrorStructure; 
	
	return (IPDR_OK);
}

/*!
 *  NAME: 
 *      getLengthListErrorStructure() - To calculate length of error list
 *
 *  DESCRIPTION:
 *      1. This function calculates length of error list. 
 *	
 *  ARGUMENTS:
 *       -	This function returns ErrorStructure, appending it to 
 *          listErrorStructure (part of IPDRCommonParameters).
 *			{IN/OUT}
 *
 *  RETURNS :	
 *	     returns error list length.
 */

int getLengthListErrorStructure(ListErrorStructure* pListErrorStructure)
{
	int count = 0;
	ListErrorStructure* current; 

	if (pListErrorStructure == NULL) {
		return (IPDR_OK);
	}
	current = pListErrorStructure;

	for (
		current = pListErrorStructure; 
		current != NULL; 
		current = current->pNext_) {
		count++;
	}
	return (count);
}

/*!
 *  NAME: 
 *      printListErrorStructure() - To print error list
 *
 *  DESCRIPTION:
 *      1. This function prints the complete error list. 
 *	
 *  ARGUMENTS:
 *       -	This function returns ErrorStructure, appending it to 
 *          listErrorStructure (part of IPDRCommonParameters).
 *			{IN/OUT}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */



int printListErrorStructure(ListErrorStructure *pListErrorStructure)
{
	ListErrorStructure* current = pListErrorStructure;
	int count = 0;

	if(current == NULL)  {
	    return (IPDR_OK);
	}

	while (current != NULL) {

		printf("\n\nDisplaying Record %d", count + 1);
		printf("\nError Code \t= %d", current->ErrStruct_.errorCode_);
		if (current->ErrStruct_.errorMessage_ == NULL)
			strcpy(current->ErrStruct_.errorMessage_, IPDR_NULL);
		printf("\nError Message \t= %s", current->ErrStruct_.
			                                       errorMessage_);
		printf("\nError Severity \t= %d", current->ErrStruct_.
			                                      errorSeverity_);
		if (current->ErrStruct_.functionName_ == NULL)
			strcpy(current->ErrStruct_.functionName_, IPDR_NULL);

		printf("\nFunction Name \t= %s", current->ErrStruct_.
			                                       functionName_);
		printf("\nRecord Count \t= %d", current->ErrStruct_.
		                                        recordCount_);
		current = current->pNext_;	
	}
	count++;
	
	return (IPDR_OK);
}

/*!
 *  NAME: 
 *      appendListErrorStructure() - To append a new node to error list
 *
 *  DESCRIPTION:
 *      1. This function appends a new node to error list. 
 *	
 *  ARGUMENTS:
 *       -	This function returns ErrorStructure, appending it to 
 *          listErrorStructure (part of IPDRCommonParameters).
 *			{IN/OUT}
 *		 -	errorCode {IN}
 *		 -	functionName {IN}
 *		 -	offsetValue {IN}  {It is applicable in case of readIPDR, 
 *		    otherwise it will have value zero.}
 *		 -	recordCount {IN} { It is applicable in case of 
 *		    writeIPDR, otherwise it will have value zero.}
 *       -  error message.
 *       -  error severity.
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


int appendListErrorStructure
                (
				 struct ListErrorStructure** pHeadRef, 
	 			 int errorCode, 
				 hyper offsetValue, 
				 char *functionName, 
				 char *errorMessage, 
				 int recordCount, 
				 int errorSeverity
				 ) 
{
	ListErrorStructure* current;

	if (pHeadRef == NULL)
		return (IPDR_OK);
	
	current = *pHeadRef;

	if (current == NULL) {
	    if (addListErrorStructure(pHeadRef, errorCode, offsetValue, 
			functionName, errorMessage, recordCount, errorSeverity)) {
		    return (IPDR_OK);
		}
	} else {
		while (current->pNext_ != NULL) {
			current = current->pNext_;
			}
	        if (addListErrorStructure(&(current->pNext_), errorCode, 
			   offsetValue, functionName, errorMessage, 
			   recordCount, errorSeverity)) {
			   return (IPDR_OK); 
		   }
	}
	return (IPDR_OK);
}


/*!
 *  NAME: 
 *      getErrorMessage() - generates an Error Message 
 *							corresponding to errorCode.
 *
 *  DESCRIPTION:
 *      1. This function returns an error message as out parameter.
 *		2. Each Error Message corresponds to Error Code.
 *	
 *  ARGUMENTS:
 *		 -	errorCode {IN}
 *		 -	errorMessage {IN/ OUT}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful. 
 */

int getErrorMessage (char* errorMessage, int* errorCode)
{

	switch (*errorCode) {
	case IPDR_UNKNOWN_NS_PREFIX:
		strcpy(errorMessage, "Unknown namespace prefix is present in service definition URI");
		break;
	case IPDR_NO_SD_FILE:
		strcpy(errorMessage, "No Service Definition file is present");
		break;
	case IPDR_INVALID_SD_FILE:
		strcpy(errorMessage, "Invalid Service Definition file.");
		break;
	case IPDR_NO_SD_URI:
		strcpy(errorMessage, "Service Definition URI is NULL");
		break;
	case IPDR_NULL_UUID:
		strcpy(errorMessage, "UUID is generated as NULL");
		break;
	case IPDR_NULL_TIME:
		strcpy(errorMessage, "Time is generated as NULL");
		break;
	case IPDR_INVALID_TIME_FORMAT:
		strcpy(errorMessage, "Invalid Time Format");
		break;
	case IPDR_INVALID_DOCTYPE:
		strcpy(errorMessage, "Invalid DocType passed, should be either XDR or XML ");
		break;
	case IPDR_INVALID_STREAM_HANDLE:
		strcpy(errorMessage, "StreamHandle should not be NULL");
		break;
	case IPDR_UNREADABLE_ISTREAM:
		strcpy(errorMessage, "Input Stream is not readable");
		break;
	case IPDR_UNWRITABLE_OSTREAM:
		strcpy(errorMessage, "Output Stream is not writable");
		break;
	case IPDR_INVALID_DOCSTATE_MAKE_READABLE:
		strcpy(errorMessage, "Invalid DocState, should be READABLE");
		break;
	case IPDR_INVALID_DOCSTATE_MAKE_UNREADABLE:
		strcpy(errorMessage, "Invalid DocState, should be UNREADABLE");
		break;
	case IPDR_INVALID_DOCSTATE_MAKE_WRITABLE:
		strcpy(errorMessage, "Invalid DocState, should be WRITEABLE");
		break;
	case IPDR_INVALID_DOCSTATE_MAKE_UNWRITABLE:
		strcpy(errorMessage, "Invalid DocState, should be UNWRITEABLE");
		break;
	case IPDR_NULL_INPUT_PARAMETER:
		strcpy(errorMessage, "Input parameter passed as NULL");
		break;
	case IPDR_MISMATCH_NS_SD:
		strcpy(errorMessage, "Mismatch in number of elements in nameSpaceInfo & serviceDefinitionURI arrays.");
		break;
	case IPDR_LESS_SD_THAN_NS:
		strcpy(errorMessage, "Number of elements in serviceDefinitionURI should not be less than number of elements in nameSpaceInfo array.");
		break;
	case IPDR_NON_PRIMITIVE_DATA_TYPE:
		strcpy(errorMessage, "Data type encountered other than 8 primitive types.");
		break;
	case IPDR_MISMATCH_DESCRIPTOR_DATA_ARRAY:
		strcpy(errorMessage, "Mismatch in length of descriptor & data array.");
		break;
	case IPDR_MISMATCH_ATTRIBUTE_DATA_ARRAY:
		strcpy(errorMessage, "Mismatch in AttributeType & Input Data Type");
		break;
	case IPDR_NO_SCHEMA:
		strcpy(errorMessage, "No Schema is available");
		break;
	case IPDR_NO_DESCID:
		strcpy(errorMessage, "Descriptor ID not found.");
		break;
	case IPDR_INVALID_DESCRIMINATOR:
		strcpy(errorMessage, "Invalid descriminator");
		break;
	case IPDR_INVALID_VERSION:
		strcpy(errorMessage, "Invalid IPDRDoc Version");
		break;
	case IPDR_INVALID_IPV4ADDR:
		strcpy(errorMessage, "Invalid Ipv4Addr");
		break;
	case IPDR_INVALID_IPV6ADDR:
		strcpy(errorMessage, "Invalid Ipv6Addr");
		break;
	case IPDR_MISMATCH_XML_TAGS:
		strcpy(errorMessage, "Mismatch in XML tags found.");
		break;
	case IPDR_NO_DOCEND:
		strcpy(errorMessage, "DocEnd not found");
		break;
	case IPDR_INVALID_FILE:
		strcpy(errorMessage, "Invalid file.");
		break;
	case IPDR_INVALID_OBJECT:
		strcpy(errorMessage, "Object <ObjectName> passed as NULL.");
		break;
	case IPDR_INVALID_URL:
		strcpy(errorMessage, "Invalid URL");
		break;
	case IPDR_INVALID_LOGIN:
		strcpy(errorMessage, "Login/Password is not correct.");
		break;
	case IPDR_NO_SUCH_FILE:
		strcpy(errorMessage, "File does not exist.");
		break;
	case IPDR_INVALID_COMPACT_FORMAT:
		strcpy(errorMessage, "Mismatch in Version");
		break;
	case IPDR_CONNECTION_FAILED:
		strcpy(errorMessage, "Connection Failed");
		break;
	case IPDR_PARSING_ERROR:
		strcpy(errorMessage, "Parsing Error");
		break;
	case IPDR_MISMATCH_OPTIONAL_ATTRNAME_SCHEMA:
		strcpy(errorMessage, "Optional Attributes Name does not match with Schema optional Attributes");
		break;
	case IPDR_MISMATCH_INPUT_DATA_SCHEMA_LOOKUP:
		strcpy(errorMessage, "Mismatch in attributes between input data and Schema Look up table.");
		break;
	case IPDR_INVALID_LENGTH_SD:
		strcpy(errorMessage, "Length of ServiceDefinitionURIs array is less than the length of otherNameSpaces");
		break;
	case IPDR_INVALID_LENGTH_SD_NO_DFLT_NS:
		strcpy(errorMessage, "Length of ServiceDefinitionURIs array is greater than the length of otherNameSpaces & defaultNameSpace is NULL");
		break;
	case IPDR_DUPLICATE_ATTRIBUTE_NAME:
		strcpy(errorMessage, "Encountered duplicate AttributeName");
		break;
	case IPDR_NULL_INPUT_DATA:
		strcpy(errorMessage, "Input data passed as NULL");
		break;
	case IPDR_NO_SUCH_DESCRIPTOR:
		strcpy(errorMessage, "Descriptor is not found for encountered descriptorID");
		break;
	case IPDR_MANDATORY_ATTR_NOT_FOUND:
		strcpy(errorMessage, "Mandatory attribute defined in schema is not found");
		break;
	case IPDR_MISMATCH_INPUT_WITH_SCHEMA_DATATYPE:
		strcpy(errorMessage, "Mismatch between Input data type and schema data type.");
		break;
	case IPDR_NO_SUCH_ATTRIBUTE_IN_SCHEMA:
		strcpy(errorMessage, "Attribute doesn't exist in Schema.");
		break;
	case IPDR_EOF_ENCOUNTERED:
		strcpy(errorMessage, "Cannot proceed, EOF encountered");
		break;
	case IPDR_EOF_NOT_FOUND_IN_IPDRDOC:
		strcpy(errorMessage, "EOF Not found at the end of IPDRDocument");
		break;
	case IPDR_MISMATCH_IN_ENCODED_DECODED_RECORDS:
		strcpy(errorMessage, "Mismatch in number of decoded & encoded  records in the IPDR Document.");
		break;
	case IPDR_MISMATCH_ATTRTAGS_IPDRDOC_SCHEMA:
		strcpy(errorMessage, "Mismatch in attribute tags present in IPDRDoc and Schema");
		break;
	case IPDR_DUPLICATE_DESCID:
		strcpy(errorMessage, "Encountered duplicate descriptorID");
		break;
	case IPDR_INVALID_DOCTYPE_MAKE_XML:
		strcpy(errorMessage, "Invalid DocType passed, should be XML");
		break;
	case IPDR_INVALID_SCHEMA:
		strcpy(errorMessage, "Invalid Schema");
		break;
	case IPDR_MEMORY_ALLOCATION_FAILED:
		strcpy(errorMessage, "Memory allocation failed");
		break;
	case IPDR_NO_MANADATORY_PARAMS:
		strcpy(errorMessage, "Mandatory Parameters are not provided");
		break;
	case IPDR_NO_HYPHEN_AND_EQUAL:
		strcpy(errorMessage, "Hyphen/ Equal not provided in command line text.");
		break;
	case IPDR_LESS_THAN_TWO_FILES_FOR_RR:
		strcpy(errorMessage, "Less than two files for round robin.");
		break;
	case IPDR_INVALID_INPUT_PARAM_NAME:
		strcpy(errorMessage, "Invalid Input parameter name.");
		break;
	case IPDR_INVALID_INPUT_PARAM_VALUE:
		strcpy(errorMessage, "Invalid Input parameter value.");
		break;
	case IPDR_BLANK_ATTRIBUTE:
		strcpy(errorMessage, "Attribute cannot be passed as NULL.");
		break;
	case IPDR_INVALID_UUID:
		strcpy(errorMessage, "Invalid UUID Format");
		break;
	case IPDR_INVALID_DELIMITER_FOR_ADDR_STRING:
		strcpy(errorMessage, "Invalid delimiter for address string");
		break;
	case IPDR_NO_DELIMITER_FOR_ADDR_STRING:
		strcpy(errorMessage, "No delimiter for address string");
		break;
	case IPDR_INVALID_DIGIT_FOR_ADDR_STRING:
		strcpy(errorMessage, "Invalid digit for address string");
		break;
	case IPDR_INVALID_LENGTH_OF_BLOCK_FOR_ADDR_STRING:
		strcpy(errorMessage, "Invalid length of block for address string");
		break;
	case IPDR_INVALID_VALUE_OF_BLOCK_FOR_ADDR_STRING:
		strcpy(errorMessage, "Invalid value of block for address string");
		break;
	case IPDR_INVALID_LENGTH_OF_ADDR_STRING:
		strcpy(errorMessage, "Invalid length of address string");
		break;
	case IPDR_NULL_LINKLIST:
		strcpy(errorMessage, "Linked List is pointing to NULL");
		break;
	case IPDR_INVALID_YEAR_IN_TIME_FORMAT:
		strcpy(errorMessage, "Invalid Year in Time format");
		break;
	case IPDR_INVALID_MONTH_IN_TIME_FORMAT:
		strcpy(errorMessage, "Invalid Month in Time format");
		break;
	case IPDR_INVALID_DAYS_IN_TIME_FORMAT:
		strcpy(errorMessage, "Invalid Days in Time format");
		break;
	case IPDR_INVALID_HOURS_IN_TIME_FORMAT:
		strcpy(errorMessage, "Invalid Hours in Time format");
		break;
	case IPDR_INVALID_MIN_IN_TIME_FORMAT:
		strcpy(errorMessage, "Invalid Minute in Time format");
		break;
	case IPDR_INVALID_SEC_IN_TIME_FORMAT:
		strcpy(errorMessage, "Invalid Second in Time format");
		break;
	case IPDR_SUBSCRIPT_OUT_OF_RANGE:
		strcpy(errorMessage, "Subscript out of range");
		break;
	case IPDR_INVALID_RECORDCOUNT:
		strcpy(errorMessage, "Invalid Record Count");
		break;
	case IPDR_MISSING_OR_INVALID_COMMANDLINE_PARAMS:
		strcpy(errorMessage, "Missing or invalid command line parameters");
		break;
	case IPDR_MISSING_OR_INVALID_OUTFILE:
		strcpy(errorMessage, "Missing or invalid output file");
		break;
	case IPDR_ST_FOR_SCHEMA_ABSENT:
		strcpy(errorMessage, "Service Type is Null");
		break;
	case IPDR_MISMATCH_NS_DFLT_NS:
		strcpy(errorMessage, "For the given NameSpace ID, NameSpace URI is not equal to default");
		break;
	case IPDR_MACTH_ONS_DNS:
		strcpy(errorMessage, "For the given NameSpace ID, NameSpace URI is equal to default");
		break;
	case IPDR_MISMATCH_TRGT_NS_ONS:
		strcpy(errorMessage, "Target NameSpace does not match");
		break;
	case IPDR_MISMATCH_TRGT_NS_DFLT_NS:
		strcpy(errorMessage, "Target NameSpace is not Default");
		break;
	case IPDR_NULL_KEY_VALUE:
		strcpy(errorMessage, "Either Key or Value is Null");
		break;
	case IPDR_NULL_SCHEMA_LOOKUP:
		strcpy(errorMessage, "The Schema Lookup Table is not populated");
		break;
	case IPDR_DATATYPE_MISMATCH_IN_SCHEMA_AND_FNFDATA:
		strcpy(errorMessage, "Data Type for attribute of Schema and corresponding attribute of FNFData is not matching");
		break;
	case IPDR_NULL_DATA_VALUE:
		strcpy(errorMessage, "Null Input Data Value");
		break;
	case IPDR_HASHGET_NULL:
		strcpy(errorMessage, "HashGet failed");
		break;
	case IPDR_GETID_NULL:
		strcpy(errorMessage, "NameSpaceID not obtained");
		break;
    case IPDR_GETNAME_NULL:
		strcpy(errorMessage, "Service name not obtained");
		break;
    case IPDR_GETURI_NULL:
		strcpy(errorMessage, "Service URI  not obtained");
		break;
    case IPDR_KEYTOK_FAIL :
		strcpy(errorMessage, "The tokenisation of the key failed !");
		break;
    case IPDR_XML_INPUT_FILE_NULL:
		strcpy(errorMessage, "XML Stream Input is NULL");
		break;
    case IPDR_XML_INPUT_FILE_INVALID:
		strcpy(errorMessage, "XML Stream Input is Invalid");
		break;
    case IPDR_XML_MEMORY_ALLOCATION_FAILED:
		strcpy(errorMessage, "Unable to allocate Memory in XML Parser");
		break;
    case IPDR_XML_PARSER_MEMORY_ALLOC_FAILED:
		strcpy(errorMessage, "Unable to allocate Memory in XML Parser");
		break;

    case IPDR_XML_READ_ERROR:		
		strcpy(errorMessage, "Unable to Read XML Stream");
		break;

    case IPDR_XML_PARSER_READ_ERROR:
		strcpy(errorMessage, "Unable to Read XML Stream");
		break;

    case IPDR_DESCRIPTOR_SCHEMA_FILE_NULL:		
		strcpy(errorMessage, " Descriptor schema file is null");
		break;

    case IPDR_DESCRIPTOR_SCHEMA_FILE_INVALID:		
		strcpy(errorMessage, "Input Schema file is invalid");
		break;

    case IPDR_DESCRIPTOR_PARSER_MEMORY_ALLOC_FAILED:		
		strcpy(errorMessage, "Parser memory allocation failed");
		break;

    case IPDR_DESCRIPTOR_FILE_NOT_FOUND:		
		strcpy(errorMessage, "Descriptor file not found");
		break;

    case IPDR_DESCRIPTOR_READ_ERROR:		
		strcpy(errorMessage, "Descriptor cannot read schema file");
		break;

    case IPDR_DESCRIPTOR_PARSER_READ_ERROR:		
		strcpy(errorMessage, "Descriptor cannot parse schema file");
		break;

    case IPDR_DESCRIPTOR_SCHEMA_FILE_NOT_FOUND:		
		strcpy(errorMessage, "Descriptor schema file not found");
		break;

    case IPDR_CURLE_COULDNT_CONNECT:		
		strcpy(errorMessage, "Cannot connect");
		break;

    case IPDR_CURLE_FTP_COULDNT_RETR_FILE:	
		strcpy(errorMessage, "Curl ftp could not retrieve file");
		break;

    case IPDR_CURLE_FTP_ACCESS_DENIED:		
		strcpy(errorMessage, "Curl ftp access denied");
		break;

    case IPDR_CURLE_LOCAL_FILENAME_NULL:		
		strcpy(errorMessage, "Null local file");
		break;

    case IPDR_CURLE_LOCAL_FILENAME_BLANK:		
		strcpy(errorMessage, "Blank local file name");
		break;
    case IPDR_CURLE_WRITE_ERROR:		
		strcpy(errorMessage, "Curl write Error");
		break;

    case IPDR_CURLE_GENERAL_ERROR:		
		strcpy(errorMessage, "Curl Error");
		break;
	case IPDR_XML_NULL_TIME:		
		strcpy(errorMessage, "XML creation is NULL");
		break;
    case IPDR_XML_NULL_DOCID:		
		strcpy(errorMessage, "XML docID is NULL");
		break;    
	case IPDR_XML_NULL_ONS:		
		strcpy(errorMessage, "XML Other NamespaceInfo is NULL");
		break;	
	case IPDR_XML_NULL_SD:		
		strcpy(errorMessage, "XML Service Definition URI is NULL");
		break;
	case IPDR_XML_NULL_RI:		
		strcpy(errorMessage, "XML Recorder Info is NULL");
		break;
	case IPDR_XML_NULL_DNS:		
		strcpy(errorMessage, "XML Default Namespace is NULL");
		break;
	case IPDR_INVALID_ORDER:
		strcpy(errorMessage, "Order of FNFData attributes is not matching with Schema attribute order");
		break;
    case IPDR_LOOKUP_PARSER_MEMORY_ALLOC_FAILED:
	    strcpy(errorMessage, "Memory allocation failed for Lookup parser");
	    break;
    case IPDR_LOOKUP_FILE_NOT_FOUND:
	    strcpy(errorMessage, "Lookup file not found");
	    break;
    case IPDR_LOOKUP_READ_ERROR:
	    strcpy(errorMessage, "Lookup file is not readable");
	    break;
    case IPDR_LOOKUP_PARSER_READ_ERROR:
    	strcpy(errorMessage, "Error occurred while parsing the lookup schema");
	    break;
    case IPDR_USER_AND_SCHEMA_VERSION_MISMATCH_ERROR:
    	strcpy(errorMessage, "Mismatch of versions between the User and the Schema inputted");
	    break;
	default:
		if (*errorCode > EXPAT_ERROR_CODE) {
			strcpy(errorMessage, "XML parser error: ");
			strcat(errorMessage , (char *) XML_ErrorString(*errorCode - EXPAT_ERROR_CODE));
		} else {

	/* Default Errors */
			strcpy(errorMessage, "Unknown Error Encountered");
		}
	break;
	}
	return (IPDR_OK);
}


/*!
 *  NAME: 
 *      errorHandler() - returns an Error Structure, which API will 
 *                       pass on to the Calling application.
 *
 *  DESCRIPTION:
 *      1. This function has to be called whenever any function 
 *         returns Error Code other than 0. 
 *		2. It takes the Error Code, Function Name & a pointer to 
 *		   the Error Structure as input and writes the Error Code, 
 *		   Function Name & Description (corresponding to that Error
 *		   Code) to that Error Structure which is passed on to the 
 *		   calling application and can be used for further tracking
 *		   the Errors. 
 *	
 *  ARGUMENTS:
 *       -	This function returns ErrorStructure, appending it to 
 *          listErrorStructure (part of IPDRCommonParameters).
 *			{IN/OUT}
 *		 -	errorCode {IN}
 *		 -	functionName {IN}
 *		 -	offsetValue {IN}  {It is applicable in case of readIPDR, 
 *		    otherwise it will have value zero.}
 *		 -	recordCount {IN} { It is applicable in case of 
 *		    writeIPDR, otherwise it will have value zero.}	
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int errorHandler(
				 IPDRCommonParameters* pIPDRCommonParameters, 
				 int* errorCode, 
				 char* functionName
				 )
{
	char* errorMessage = (char *) malloc (MAX_ERROR_MESSAGE);
	getErrorMessage(errorMessage, errorCode);

	appendListErrorStructure(
		&pIPDRCommonParameters->pListErrorStructure_,
		*errorCode,
		pIPDRCommonParameters->IPDRCount_.offsetValue_,
		functionName,
		errorMessage,
		pIPDRCommonParameters->IPDRCount_.ipdrRecordCount_,
		IPDR_WARNING);

        free(errorMessage);
	return (IPDR_OK);

}


/*!
 *  NAME:
 *      newExceptionStructure() - Memory allocation of 
 *                                exception structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory allocation of 
 *         exception structure & its members.
 *	
 *  ARGUMENTS:
 *       -	void
 *
 *  RETURNS 	
 *	     Returns Pointer to exception structure
 */


ErrorStructure* newExceptionStructure(void)
{
	ErrorStructure* pExceptionStructure;
	pExceptionStructure = (ErrorStructure *) calloc (1, sizeof(ErrorStructure));

	pExceptionStructure->errorMessage_ = (char *) calloc(1, MAX_ERROR_MESSAGE);
	
	return (pExceptionStructure);
}


/*!
 *  NAME:
 *      freeExceptionStructure() - Memory deallocation of 
 *                           ExceptionStructure structure
 *
 *  DESCRIPTION:
 *      1. This function performs the memory deallocation of 
 *         ExceptionStructure structure & its members.
 *	
 *  ARGUMENTS:
 *       -	ExceptionStructure {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int freeExceptionStructure(ErrorStructure* pExceptionStructure)
{
	if (pExceptionStructure == NULL) {
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	free(pExceptionStructure->errorMessage_);
	free(pExceptionStructure->functionName_);
	free(pExceptionStructure);

	return (IPDR_OK);
}


/*!
 *  NAME:
 *      printExceptionStructure() - prints 
 *                           ExceptionStructure structure
 *
 *  DESCRIPTION:
 *      1. This function prints the 
 *         ExceptionStructure structure & its members.
 *	
 *  ARGUMENTS:
 *       -	pExceptionStructure {IN/OUT} 
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


int printExceptionStructure(ErrorStructure* pExceptionStructure)
{
	if (pExceptionStructure == NULL) {
        return (IPDR_NULL_INPUT_PARAMETER);
	}
		printf("\npExceptionStructure->errorCode = %d", pExceptionStructure->errorCode_);
		printf("\npExceptionStructure->errorMessage = %s", pExceptionStructure->errorMessage_);
		printf("\npExceptionStructure->errorSeverity = %d", pExceptionStructure->errorSeverity_);
		printf("\npExceptionStructure->functionName = %s", pExceptionStructure->functionName_);
		printf("\npExceptionStructure->recordCount = %d", pExceptionStructure->recordCount_);
	return (IPDR_OK);

}


/*!
 *  NAME: 
 *      exceptionHandler() - returns an exception Structure, which API will 
 *                       pass on to the Calling application.
 *
 *  DESCRIPTION:
 *      1. This function has to be called whenever any function 
 *         returns exception Code other than 0. 
 *		2. It takes the exception Code, Function Name & a pointer to 
 *		   the exception Structure as input and writes the Error Code, 
 *		   Function Name & Description (corresponding to that Error
 *		   Code) to that exception Structure which is passed on to the 
 *		   calling application and can be used for further tracking
 *		   the exceptions.
 *	
 *  ARGUMENTS:
 *       -	This function returns ExceptionStructure
 *			{IN/OUT}
 *		 -	errorCode {IN}
 *		 -	functionName {IN}
 *
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int exceptionHandler(
				 IPDRCommonParameters* pIPDRCommonParameters, 
				 int* pErrorCode, 
				 char* functionName
				 )
{
	unsigned int lenFunctionName = 0;

	char* errorMessage = (char *) malloc (MAX_ERROR_MESSAGE);
	getErrorMessage(errorMessage, pErrorCode);
	
	pIPDRCommonParameters->pExceptionStructure_->errorCode_ = *pErrorCode;
	strcpy(pIPDRCommonParameters->pExceptionStructure_->errorMessage_, errorMessage);
	pIPDRCommonParameters->pExceptionStructure_->errorSeverity_ = IPDR_ERROR;

	lenFunctionName = strlen(functionName) + 1;
	pIPDRCommonParameters->pExceptionStructure_->functionName_ = (char *) calloc (lenFunctionName, sizeof(char));
		strcpy(pIPDRCommonParameters->pExceptionStructure_->functionName_, functionName); 
	pIPDRCommonParameters->pExceptionStructure_->offsetValue_ = 
		pIPDRCommonParameters->IPDRCount_.offsetValue_;
	pIPDRCommonParameters->pExceptionStructure_->recordCount_ = 
		pIPDRCommonParameters->IPDRCount_.ipdrRecordCount_;
        free(errorMessage);

	return (IPDR_OK);

}






