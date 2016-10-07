/*! Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/


/**
 * File                 : Utils.c			         
 * Description          : Contains all the Util      
                          functions      
 * Author               : Infosys Tech Ltd           
 * Modification History :                            
 *---------------------------------------------------
 * Date       Name             Change/Description    
 *---------------------------------------------------
 * 12/19/01  Infosys           Created                        
 */

#include "utils/utils.h"
#include "utils/errorCode.h"


 /*!
 *  NAME:
 *      convByteArrayToUUID() - converts the byte array into UUID
 *                              (string format)
 *
 *  DESCRIPTION:
 *      This method takes as input the UUID byte array and converts 
 *		it into string format as per pattern specified in the NDM-U
 *		3.0 specification. That string is then returned as output.
 *  
 *  ARGUMENTS:
 *       -	UUID (IN)	
 *       -  strUUID (IN/OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate                                  
 * 		 error code in case of failure.
 */


int convByteArrayToUUID(unsigned char* UUID, char* strUUID, int* pErrorCode)
{
	int counter = 0;
	int tempInt = 0;

	char* tempHex;

	if ((UUID == NULL) || (strUUID == NULL)) {
	/* Returns Appropriate Error Structure*/
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		return (IPDR_WARNING);
	}
	
	/* Memory allocation for temporary variable*/

	tempHex = (char*) calloc(3, sizeof(char));

	if (tempHex == NULL) {
	/* Returns Appropriate Error Structure*/
		*pErrorCode = IPDR_MEMORY_ALLOCATION_FAILED;
		return (IPDR_WARNING);
	} 

	/*!
	 * Converts to appropriate hex representation in specified format
	 * which represents pair of 2 bytes
	 */
	for (counter=0; counter <= 15; counter++) {
	    tempInt = (int)(UUID[counter]);
        sprintf(tempHex, "%02x", tempInt);
        strcat(strUUID,tempHex);

		if ((counter == 3) || (counter == 5) || (counter == 7) || 
			                                   (counter == 9)) {
	        strcat(strUUID,DASH);
		}
	}
	 	 
	free(tempHex);

	/*return success*/	
	return (IPDR_OK);
}

 /*!
 *  NAME:
 *      convUUIDToByteArray() - converts the UUID into Byte Array.
 *                              
 *
 *  DESCRIPTION:
 *      This method takes as input the UUID String as per pattern 
 *      specified in the NDM-U3.0 specification and converts 
 *		it into Byte Array 
 *		. 
 *  
 *  ARGUMENTS:
 *       -	UUID (IN)	
 *       -  byteUUID (IN/OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int convUUIDToByteArray(
					    char* UUID, 
						unsigned char* byteUUID,
						int* pErrorCode
						)
{
	int counter = 0;
	int count = 0;
	int tempInt = 0;
	int tempHex = 0;
	int tempHex1 = 0;
	int flag = 0;
	int hCount = 0;

	unsigned char byte;

	if ((UUID == NULL) || (byteUUID == NULL)){
	/* Returns Appropriate Error Structure*/
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		return (IPDR_WARNING);
	}


	/* Checks for length of UUID, It should be 36 characters*/
	if(strlen(UUID) != LENGTH_UUID_STRING) {
		*pErrorCode = IPDR_INVALID_UUID;
		return (IPDR_WARNING);
	}
	
	/* This iterative loop checks for dash at 8,13,18 & 23rd positions*/
	for (counter=0; UUID[counter] != '\0'; counter++) {
		if (UUID[counter] == '-'){
			if(counter == 8 && UUID[counter + 5] == '-'  && UUID[counter + 10] == '-' && UUID[counter + 15] == '-')
			flag = 1;
			if(counter == 13 && UUID[counter - 5] == '-'  && UUID[counter + 5] == '-' && UUID[counter + 10] == '-')
			flag = 1;
			if(counter == 18 && UUID[counter - 10] == '-'  && UUID[counter - 5] == '-' && UUID[counter + 5] == '-')
			flag = 1;
			if(counter == 23 && UUID[counter + 5] == '-'  && UUID[counter + 10] == '-' && UUID[counter + 15] == '-')
			flag = 1;
			hCount++;
		}

	}
	/*!
	 * temperary varaible checks for number of presence of dash.
	 * if it is greater than 4, then error
	 */
	if(hCount > 4) {
		*pErrorCode = IPDR_INVALID_UUID;
		return (IPDR_WARNING);
	}
	/*!
	 * temperary varaible checks for presence of dash at specified locations.
	 */
	if (flag == 0) {
		*pErrorCode = IPDR_INVALID_UUID;
		return (IPDR_WARNING);
	}
	
	 /*! Iterative loop checks whether all characters are hex or not.*/
	for (counter=0; UUID[counter] != '\0'; counter++) {
		if(!isxdigit(toupper(UUID[counter])) && UUID[counter] != '-') {
			*pErrorCode = IPDR_INVALID_UUID;
			return (IPDR_WARNING);
		}
	}
	/*!
	 * This function reads the UUID and converts it into byte array.
	 */
	for (counter=0, count=0; UUID[counter] != '\0'; counter++, count++) {

	if(UUID[counter] != '-') {

		 if(((int)(UUID[counter]) <= 57) && ((int)(UUID[counter]) >= 48)) {

		    tempHex =(UUID[counter]) - 48;

		 } else  {

			tempHex = toupper(UUID[counter]) - 55;
		 }

		 counter = counter + 1;

	     if((UUID[counter] <= 57) && (UUID[counter]) >= 48) {

		    tempHex1 = (UUID[counter]) - 48;

		 } else {

			tempHex1 = toupper(UUID[counter]) - 55;
		 }


	    tempInt = (tempHex) * 16 + (tempHex1);

		byte = tempInt;

		byteUUID[count]=byte;

		}

	else {
		count=count-1;
	}
	 
	}
	/*return success*/
	return (IPDR_OK);
}



 /*!
 *  NAME:
 *      convLongToTime() - converts the 64 bits time representation 
 *						   into standard time format string.
 *                          
 *
 *  DESCRIPTION:
 *      This method takes as input the time in the form of long 
 *      data type and converts it into string format as per pattern 
 *      specified in the NDM-U 3.0 specification. The string is 
 *		returned as output.
 *	
 *  ARGUMENTS:
 *       -	pTimeMilliSec (IN)	
 *		 -  timeStr (IN / OUT)	
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


int convLongToTime(
				   hyper* pTimeMilliSec,
				   char* timeStr,
				   int* pErrorCode
				  )
{
		
		struct tm* newTime = NULL;
		char *tempStr;
		char mnth[3];
		char day[3];
		char hour[3];
		char min[3];
		char sec[3];
		char temp[3];
		char milliSec[4];
		
		int milliSeconds=0;
		int count = 0;
		int start = 0;
		int length = 0;
		int value = 0;
		long TimeSec = 0;
		double TimeSec1 = 0;

		if ((pTimeMilliSec == NULL) || (timeStr == NULL)) {
	    /* Returns Appropriate Error Structure*/
			*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		    return (IPDR_WARNING);
		}
		
		tempStr = (char*) calloc(100, sizeof(char));
		sprintf(tempStr, "%.lf", (double)*pTimeMilliSec);

		length = strlen(tempStr);
		if(length < 3)
			start = 0;
		else 
			start =length - 3;

		for(count = start ; count <= length; count++)
		{
			temp[value] = tempStr[count];
			value++;
		}

		milliSeconds = atoi(temp);


		TimeSec1 = ((double)(*pTimeMilliSec) - milliSeconds)/1000;
		TimeSec = (long)TimeSec1;
		newTime = gmtime( &TimeSec );

		/*
		newTime = localtime( &TimeSec );
		*/
		
		sprintf(timeStr, "%d%s", newTime->tm_year+1900, DASH);
		sprintf(mnth, "%d", newTime->tm_mon+1);
		if (newTime->tm_mon+1 < 10) {
		    strcat(timeStr,ZERO);
		}
		strcat(timeStr,mnth);
		strcat(timeStr,DASH);
	    sprintf(day, "%d", newTime->tm_mday);
		if (newTime->tm_mday < 10) {
		    strcat(timeStr,ZERO);
		}
		strcat(timeStr,day);
		strcat(timeStr,T);
	    sprintf(hour, "%d", newTime->tm_hour);
		if (newTime->tm_hour < 10) {
		    strcat(timeStr,ZERO);
		}
	
		strcat(timeStr,hour);
		strcat(timeStr,COLON);
		sprintf(min, "%d", newTime->tm_min);
		if ( newTime->tm_min < 10) {
		    strcat(timeStr,ZERO);
		}
		strcat(timeStr,min);
		strcat(timeStr,COLON);
		sprintf(sec, "%d", newTime->tm_sec);
		if (newTime->tm_sec < 10) {
		    strcat(timeStr,ZERO);
		}
		strcat(timeStr,sec);
		strcat(timeStr,DOT);
		
		if(milliSeconds == 0) {
				strcat(timeStr, "000");
		}
		else {
				sprintf(milliSec, "%d", milliSeconds);

				if ((milliSeconds >0 ) && (milliSeconds <10)){

						strcat(timeStr, "00");
						strcat(timeStr, milliSec);
					}
				else if ((milliSeconds >=10) && (milliSeconds <100)){
						strcat(timeStr, "0");
						strcat(timeStr, milliSec);
				}
				else {
					
						strcat(timeStr, milliSec);
				}

		}


		strcat(timeStr,Z);
		free(tempStr);
		/*return success*/
		return (IPDR_OK);
}


 /*!
 *  NAME:
 *      convLongToDateTime() - converts the long time representation 
 *			   into standard time format string without milliseconds.
 *                          
 *
 *  DESCRIPTION:
 *      This method takes as input the time in the form of long 
 *      data type and converts it into string format as per pattern 
 *      specified in the NDM-U 3.0 specification. The string is 
 *		returned as output.
 *	
 *  ARGUMENTS:
 *       -	pTimelong (IN)	
 *		 -  timeStr (IN / OUT)	
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


int convLongToDateTime(
					   long* pTimelong,
					   char* timeStr,
					   int* pErrorCode
					  )
{
		
		struct tm* newTime = NULL;
		char mnth[3];
		char day[3];
		char hour[3];
		char min[3];
		char sec[3];

		long TimeSec = 0;

		if ((pTimelong == NULL) || (timeStr == NULL)) {
	    /* Returns Appropriate Error Structure*/
		    *pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		    return (IPDR_WARNING);
		}
		
		/*!Converts input time into string format as per pattern 
        specified in the NDM-U 3.0 specification*/

		TimeSec = (int)((*pTimelong)-(*pTimelong) % 1000) / 1000;
	
		gmtime( &TimeSec );
			
		newTime = localtime( &TimeSec );
		
		sprintf(timeStr, "%d%s", newTime->tm_year + 1900, DASH);

		sprintf(mnth, "%d", newTime->tm_mon + 1);

		if (newTime->tm_mon < 10) {
		    strcat(timeStr,ZERO);
		}

		strcat(timeStr,mnth);
			
		strcat(timeStr,DASH);

	    sprintf(day, "%d", newTime->tm_mday);

		if (newTime->tm_mday < 10) {
		    strcat(timeStr,ZERO);
		}
		
		strcat(timeStr,day);
		
		strcat(timeStr,T);

	    sprintf(hour, "%d", newTime->tm_hour);
		if (newTime->tm_hour < 10) {
		    strcat(timeStr,ZERO);
		}
		strcat(timeStr,hour);
		strcat(timeStr,COLON);

		sprintf(min, "%d", newTime->tm_min);
		if (newTime->tm_min < 10) {
		    strcat(timeStr,ZERO);
		}
		strcat(timeStr,min);
		strcat(timeStr,COLON);
		
		sprintf(sec, "%d", newTime->tm_sec);
		if (newTime->tm_sec < 10) {
		    strcat(timeStr,ZERO);
		}
		strcat(timeStr,sec);

		strcat(timeStr,Z);

		/*return success*/
		return (IPDR_OK);
}

/*!
 *  NAME:
 *      convIntToDateTime() - converts the int time representation 
 *			   into standard time format string without milliseconds.
 *                          
 *
 *  DESCRIPTION:
 *      This method takes as input the time in the form of int 
 *      data type and converts it into string format as per pattern 
 *      specified in the NDM-U 3.0 specification. The string is 
 *		returned as output.
 *	
 *  ARGUMENTS:
 *       -	pTimelnt (IN)	
 *		 -  timeStr (IN / OUT)	
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


int convIntToDateTime(
					   int* pTimelnt,
					   char* timeStr,
					   int* pErrorCode
					  )
{
		
		struct tm* newTime = NULL;
		char mnth[3];
		char day[3];
		char hour[3];
		char min[3];
		char sec[3];

		long TimeSec = 0;

		if ((pTimelnt == NULL) || (timeStr == NULL)) {
	    /* Returns Appropriate Error Structure*/
		    *pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		    return (IPDR_WARNING);
		}
				
		TimeSec =  (long) *pTimelnt;
		newTime = gmtime(&TimeSec);

		sprintf(timeStr, "%d%s", newTime->tm_year + 1900, DASH);

		sprintf(mnth, "%d", newTime->tm_mon + 1);

		if (newTime->tm_mon < 10) {
		    strcat(timeStr,ZERO);
		}

		strcat(timeStr,mnth);
		strcat(timeStr,DASH);
	    sprintf(day, "%d", newTime->tm_mday);

		if (newTime->tm_mday < 10) {
		    strcat(timeStr,ZERO);
		}
		
		strcat(timeStr,day);
		strcat(timeStr,T);
	    sprintf(hour, "%d", newTime->tm_hour);
		if (newTime->tm_hour < 10) {
		    strcat(timeStr,ZERO);
		}
		strcat(timeStr,hour);
		strcat(timeStr,COLON);
		sprintf(min, "%d", newTime->tm_min);
		if (newTime->tm_min  < 10) {
		    strcat(timeStr,ZERO);
		}
		strcat(timeStr,min);
		strcat(timeStr,COLON);
		sprintf(sec, "%d", newTime->tm_sec);
		if (newTime->tm_sec  < 10) {
		    strcat(timeStr,ZERO);
		}
		strcat(timeStr,sec);
		strcat(timeStr,Z);

		/*return success*/
		return (IPDR_OK);
}



 /*!
 *  NAME:
 *      getTime() - This function calculates the time difference
 *
 *  DESCRIPTION:
 *     	This is a utility function. It returns the number of 
 *      milliseconds between current time and EPOCH Jan 1 1970, 
 *   	0:00 GMT.
 *	
 *  ARGUMENTS:
 *        -	time (IN / OUT)
 *      
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int getTime(
			hyper* time,
			int* pErrorCode
		   ) 
{
	
	 struct timeb tstruct;

	 if (time == NULL) {
	 /* Returns Appropriate Error Structure*/
		 *pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		 return (IPDR_WARNING);
	 }

	 ftime(&tstruct);

	 /*! Returns time in millisec by adding time in seconds to 
	 the no of milliseconds*/

	 *time =(hyper)((hyper)(tstruct.time) * 1000 + 
		                     (hyper) tstruct.millitm);

	/*return success*/
     return (IPDR_OK);
}


 /*!
 *  NAME:
 *      validateIPv4Addr() - validated IP Address for IPv4
 *
 *  DESCRIPTION:
 *     		This method will validate whether the input IP Address 
 *          is in proper format for IPv4.
 *  ARGUMENTS:
 *        -	IPAddr{IN}
 *      
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int validateIPv4Addr(char* IPAddr, int* pErrorCode)
{
	int count = 0;
	int counter = 0;
	int incr = 0;
	int flag = 0;
	int inc = 0;
	char str[20];

	if (IPAddr == NULL) {
	/* Returns Appropriate Error Structure*/
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		return (IPDR_WARNING);
	}

    /*! Validates for delimiter*/

	while (IPAddr[count] != '\0') {
	    if (IPAddr[count] == '.') {
			flag=1;
		} else if (!isdigit(IPAddr[count])) {
			*pErrorCode = IPDR_INVALID_DELIMITER_FOR_ADDR_STRING;
			return (IPDR_WARNING);
		}
	count++;
	}

	if (flag == 0)	{
		*pErrorCode = IPDR_NO_DELIMITER_FOR_ADDR_STRING;
        return (IPDR_WARNING);   
	}

	 /*! Checks individual 'bytes' of IPAddress for integer 
	                            representation and for being < 256 */
	while (incr < 4) {
	for (count = counter, inc = 0; IPAddr[count] != '.' && IPAddr[count] 
		!= '\0'; count++, inc++) {
		str[inc]=IPAddr[count];
		if (!isdigit(IPAddr[count])) {
			*pErrorCode = IPDR_INVALID_DIGIT_FOR_ADDR_STRING;
				return (IPDR_WARNING); 
		}
	}
		str[inc] ='\0';
			
	if ((count-counter > 3) || (count-counter == 0)) {
		*pErrorCode = IPDR_INVALID_LENGTH_OF_BLOCK_FOR_ADDR_STRING;
		return (IPDR_WARNING);
	}
	else if(atoi(str) > 255)  {
		*pErrorCode = IPDR_INVALID_VALUE_OF_BLOCK_FOR_ADDR_STRING;
		return (IPDR_WARNING); 
	}


	counter = count;
		
		
	incr++;
	/*! Validates whether all 4 'bytes' of the address string 
	                                                 are present*/
	if (incr<4 && IPAddr[counter] == '\0')	{
		*pErrorCode = IPDR_INVALID_LENGTH_OF_ADDR_STRING;
	    return (IPDR_WARNING); 
	}

	counter++;

	}
	/*return success*/
	return (IPDR_OK);
}




 /*!
 *  NAME:
 *      validateIPv6Addr() - validated IP Address for IPv6
 *
 *  DESCRIPTION:
 *     		This method will validate whether the input IP Address 
 *          is in proper format for IPv6.
 *  ARGUMENTS:
 *        -	IPAddr{IN}
 *      
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int validateIPv6Addr(char* IPAddr, int* pErrorCode)
{
	int count = 0;
	int counter = 0;
	int incr = 0;
	int flag = 0;

    
	if (IPAddr == NULL) {
	/* Returns Appropriate Error Structure*/
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		return (IPDR_WARNING);
	}

	 /*! Validates for delimiter*/
	while (IPAddr[count] != '\0') {
		if (IPAddr[count] == ':') {
			flag=1;
		} else if (!isxdigit(IPAddr[count])) {
				*pErrorCode = IPDR_INVALID_DELIMITER_FOR_ADDR_STRING;
				return (IPDR_WARNING);
		}

	count++;
	}

	if (flag == 0) {
		*pErrorCode = IPDR_NO_DELIMITER_FOR_ADDR_STRING;
        return (IPDR_WARNING);   
	}

	/*! Checks individual 'bytes' of IPAddress for integer 
	                            representation and for being < 256 */
	while (incr < 8) {

	for (count = counter; IPAddr[count] != ':' && IPAddr[count] != '\0';
	                                                   count++)	{
     if (!isxdigit(IPAddr[count])) {
		 	*pErrorCode = IPDR_INVALID_DIGIT_FOR_ADDR_STRING;
		 return(IPDR_WARNING);
	 }
	}

		if ((count-counter > 4) || (count-counter == 0)) {		
			*pErrorCode = IPDR_INVALID_LENGTH_OF_BLOCK_FOR_ADDR_STRING;
			return (IPDR_WARNING);
		}

		counter = count;
				
		incr++;
		
		/*! Validates whether all 4 'bytes' of the address string 
	                                                 are present*/
		if (incr < 8 && IPAddr[counter] == '\0')  {
				*pErrorCode = IPDR_INVALID_LENGTH_OF_ADDR_STRING;
			return (IPDR_WARNING); 
		}


		counter++;
	}
	/*return success*/	
	return (IPDR_OK);
}

 /*!
 *  NAME:
 *      convTimeToLong() - Converts time String(with 
 *                             milliseconds) in NDM-U format to 
 *                             Long
 *
 *  DESCRIPTION:
 *      This method takes as input the time in string format as per
 *      pattern specified in the NDM-U 3.0 specification. Then it 
 *		converts the string to long form. The input string contains 
 *      time upto seconds . It uses the IsLeapYear, GetDaysInMonth
 *      and subString functions.
 *	
 *  ARGUMENTS:
 *       -	pTimeMilliSec (IN / OUT)	
 *		 -  timeStr (IN) 	
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int convTimeToLong(
				   char* timeStr,
				   hyper* pTimeMilliSec,
				   int* pErrorCode
				  )
{
	int count = 0;

	int daysInYear = 0 ;
	int daysInMnth=0;
    int totalDays = 0;
    
	char strYear[5];
	char strMnth[3];
	char strDay[3];
	char strHour[3];
	char strMin[3];
	char strSec[3];
	char strMilli[4];

	if (( timeStr == NULL) || (pTimeMilliSec == NULL )){
	/* Returns Appropriate Error Structure*/
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
	     return (IPDR_WARNING);
	 }

   /*! Calculating no. of days*/

	subString(timeStr, strYear, 0, 4);
	subString(timeStr, strMnth, 5, 2);
	subString(timeStr, strDay, 8, 2);
	subString(timeStr, strHour, 11, 2);
	subString(timeStr, strMin, 14, 2);
	subString(timeStr, strSec, 17, 2);
	subString(timeStr, strMilli, 20, 3);


	if (atoi(strYear) < 1970)  {
		*pErrorCode = IPDR_INVALID_YEAR_IN_TIME_FORMAT;
		return (IPDR_WARNING);
	}
			
	if(atoi(strMnth) > 12 || atoi(strMnth) < 0) {
		*pErrorCode = IPDR_INVALID_MONTH_IN_TIME_FORMAT;
		return (IPDR_WARNING);
	}
	if((atoi(strDay) > GetDaysInMonth(atoi(strMnth), atoi(strYear)) || 
		atoi(strDay) < 0)) {
		*pErrorCode = IPDR_INVALID_DAYS_IN_TIME_FORMAT;
		return (IPDR_WARNING);
	}


	if(atoi(strHour) > 24 || atoi(strHour) < 0) {
		*pErrorCode = IPDR_INVALID_HOURS_IN_TIME_FORMAT;
		return (IPDR_WARNING);
	}

	if(atoi(strMin) > 60 || atoi(strMin) < 0) {
		*pErrorCode = IPDR_INVALID_MIN_IN_TIME_FORMAT;
		return (IPDR_WARNING);
	}

	if(atoi(strSec) > 60 || atoi(strSec) < 0) {
		*pErrorCode = IPDR_INVALID_SEC_IN_TIME_FORMAT;
		return (IPDR_WARNING);
	}

	for (count = 1; count < atoi(strMnth); count++ )
	{
		daysInMnth=daysInMnth+GetDaysInMonth(count,atoi(strYear));
	}

	for (count = 1970; count < atoi(strYear); count++ )
	{
		if(IsLeapYear(count))
			daysInYear = daysInYear + 366;
		else
			daysInYear = daysInYear + 365;
	}

	

	if (strcmp(strMilli, "") == 0) strcpy(strMilli, "0");   

	totalDays = daysInYear + daysInMnth + atoi(strDay) - 1;
			                                           
	*pTimeMilliSec = (hyper)(atoi(strMilli) + (hyper)((atoi(strSec) + 
		               (hyper)((atoi(strMin) + (hyper)((atoi(strHour) + 
					                    totalDays*24)*60))*60))*1000));
    
	/*return success*/			
	return (IPDR_OK);
}

int IsLeapYear(int nYear) {

  if ((nYear % 4 == 0) && (nYear % 100 != 0 || nYear % 400 == 0))
    return (1);
  else
	/*return success*/	
	 return (IPDR_OK);
}

int GetDaysInMonth(int month, int nYear)
{
  int nDaysPerMonthLocal[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 
                                                             31, 30, 31};
  /* account for leap years */
  nDaysPerMonthLocal[1] = (IsLeapYear(nYear)) ? 29 : 28;

  return((int)nDaysPerMonthLocal[month-1]);
}

int subString(char *source, char *dest, int start, int end)
{
   int count = 0;
   if (source == NULL || dest == NULL) 
	   return (1);
   for(count = 0; count < end; count++) {
	   dest[count] = source[start+count];
   }
   dest[count] = '\0';
	/*return success*/	
   return (IPDR_OK);
}


 /*!
 *  NAME:
 *      convDateTimeToLong() - Converts time String(without 
 *                             milliseconds) in NDM-U format to 
 *                             Long
 *
 *  DESCRIPTION:
 *      This method takes as input the time in string format as per
 *      pattern specified in the NDM-U 3.0 specification. Then it 
 *		converts the string to long form. The input string contains 
 *      time upto seconds only. It uses the IsLeapYear, 
 *      GetDaysInMonth and subString functions.
 *	
 *  ARGUMENTS:
 *       -	pTimeMilliSec (IN / OUT)	
 *		 -  timeStr (IN) 	
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int convDateTimeToLong(
				   char* timeStr,
				   hyper* pTimeMilliSec,
				   int* pErrorCode
				  )
{
	int count = 0;

	int daysInYear = 0 ;
	int daysInMnth=0;
    int totalDays = 0;
    
	char strYear[5];
	char strMnth[3];
	char strDay[3];
	char strHour[3];
	char strMin[3];
	char strSec[3];
	char strMilli[4];

	if (( timeStr == NULL) || (pTimeMilliSec == NULL )){
	/* Returns Appropriate Error Structure*/
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
	     return (IPDR_WARNING);
	 }

   /*! Calculating no. of days*/

	subString(timeStr, strYear, 0, 4);

	subString(timeStr, strMnth, 5, 2);
	
	subString(timeStr, strDay, 8, 2);

	subString(timeStr, strHour, 11, 2);

	subString(timeStr, strMin, 14, 2);

	subString(timeStr, strSec, 17, 2);

	subString(timeStr, strMilli, 20, 3);

	
	if (atoi(strYear) < 1970)  {
		*pErrorCode = IPDR_INVALID_YEAR_IN_TIME_FORMAT;
		return (IPDR_WARNING);
	}
			
	if(atoi(strMnth) > 12 || atoi(strMnth) < 0) {
		*pErrorCode = IPDR_INVALID_MONTH_IN_TIME_FORMAT;
		return (IPDR_WARNING);
	}


	if((atoi(strDay) > GetDaysInMonth(atoi(strMnth), atoi(strYear)) || 
		atoi(strDay) < 0)) {
		*pErrorCode = IPDR_INVALID_DAYS_IN_TIME_FORMAT;
		return (IPDR_WARNING);
	}


	if(atoi(strHour) > 24 || atoi(strHour) < 0) {
		*pErrorCode = IPDR_INVALID_HOURS_IN_TIME_FORMAT;
		return (IPDR_WARNING);
	}

	if(atoi(strMin) > 60 || atoi(strMin) < 0) {
		*pErrorCode = IPDR_INVALID_MIN_IN_TIME_FORMAT;
		return (IPDR_WARNING);
	}

	if(atoi(strSec) > 60 || atoi(strSec) < 0) {
		*pErrorCode = IPDR_INVALID_SEC_IN_TIME_FORMAT;
		return (IPDR_WARNING);
	}

	for (count = 1; count < atoi(strMnth); count++ )
	{
		daysInMnth=daysInMnth+GetDaysInMonth(count,atoi(strYear));
	}

	for (count = 1970; count < atoi(strYear); count++ )
	{
		if(IsLeapYear(count))
			daysInYear = daysInYear + 366;
		else
			daysInYear = daysInYear + 365;
	}
	   
	totalDays = daysInYear + daysInMnth + atoi(strDay) - 1;
			                                           
	*pTimeMilliSec =  ((hyper)((atoi(strSec) + (hyper)((atoi(strMin) + 
		(hyper)((atoi(strHour) + totalDays*24)*60))*60))*1000));
    
	/*return success*/			
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      convDateTimeToInt() - Converts time String(without 
 *                             milliseconds) in NDM-U format to 
 *                             int
 *
 *  DESCRIPTION:
 *      This method takes as input the time in string format as per
 *      pattern specified in the NDM-U 3.0 specification. Then it 
 *		converts the string to int form. The input string contains 
 *      time upto seconds only. It uses the IsLeapYear, 
 *      GetDaysInMonth and subString functions.
 *	
 *  ARGUMENTS:
 *       -	pTimeMilliSec (IN / OUT)	
 *		 -  timeStr (IN) 	
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int convDateTimeToInt(
				   char* timeStr,
				   int* pTimeMilliSec,
				   int* pErrorCode
				  )
{
	int count = 0;

	int daysInYear = 0 ;
	int daysInMnth=0;
    int totalDays = 0;
    
	char strYear[5];
	char strMnth[3];
	char strDay[3];
	char strHour[3];
	char strMin[3];
	char strSec[3];

	if (( timeStr == NULL) || (pTimeMilliSec == NULL )){
	/* Returns Appropriate Error Structure*/
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
	     return (IPDR_WARNING);
	 }

   /*! Calculating no. of days*/

	subString(timeStr, strYear, 0, 4);

	subString(timeStr, strMnth, 5, 2);
	
	subString(timeStr, strDay, 8, 2);

	subString(timeStr, strHour, 11, 2);

	subString(timeStr, strMin, 14, 2);

	subString(timeStr, strSec, 17, 2);

	if (atoi(strYear) < 1970)  {
		*pErrorCode = IPDR_INVALID_YEAR_IN_TIME_FORMAT;
		return (IPDR_WARNING);
	}
			
	if(atoi(strMnth) > 12 || atoi(strMnth) < 0) {
		*pErrorCode = IPDR_INVALID_MONTH_IN_TIME_FORMAT;
		return (IPDR_WARNING);
	}


	if((atoi(strDay) > GetDaysInMonth(atoi(strMnth), atoi(strYear)) || 
		atoi(strDay) < 0)) {
		*pErrorCode = IPDR_INVALID_DAYS_IN_TIME_FORMAT;
		return (IPDR_WARNING);
	}

	if(atoi(strHour) > 24 || atoi(strHour) < 0) {
		*pErrorCode = IPDR_INVALID_HOURS_IN_TIME_FORMAT;
		return (IPDR_WARNING);
	}

	if(atoi(strMin) > 60 || atoi(strMin) < 0) {
		*pErrorCode = IPDR_INVALID_MIN_IN_TIME_FORMAT;
		return (IPDR_WARNING);
	}

	if(atoi(strSec) > 60 || atoi(strSec) < 0) {
		*pErrorCode = IPDR_INVALID_SEC_IN_TIME_FORMAT;
		return (IPDR_WARNING);
	}

	for (count = 1; count < atoi(strMnth); count++ )
	{
		daysInMnth=daysInMnth+GetDaysInMonth(count,atoi(strYear));
	}

	for (count = 1970; count < atoi(strYear); count++ )
	{
		if(IsLeapYear(count))
			daysInYear = daysInYear + 366;
		else
			daysInYear = daysInYear + 365;
	}
	   
	totalDays = daysInYear + daysInMnth + atoi(strDay) - 1;
			                                           
	*pTimeMilliSec =  (atoi(strSec) + ((atoi(strMin) + 
		((atoi(strHour) + totalDays*24)*60))*60));
    
	/*return success*/			
	return (IPDR_OK);
}





/*!
 *  NAME:
 *      convIPV6AddrToByteArray() - converts the IPV6Addr into Byte Array.
 *                              
 *
 *  DESCRIPTION:
 *      This method takes the IPV6Addr string format as per pattern 
 *      specified in the NDM-U 3.0 specification as input and converts 
 *      it into Byte array. 
 *  
 *  ARGUMENTS:
 *        -strIPV6Addr(IN)
 *        -byteIPV6Addr(IN/OUT)
 *      
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


int convIPV6AddrToByteArray(
					    char* strIPV6AddrSource, 
						unsigned char* byteIPV6Addr,
						int* pErrorCode
						)
{
	int counter = 0;
	int count = 0;
	int tempInt = 0;
	int tempHex = 0;
	int tempHex1 = 0;
	int flag = 0;
	int hCount = 0;
	int errorCode=0;
	int countHexDig=0;

	unsigned char byte;
	char *tempStr;
	char* strIPV6Addr;
	char* strByte;

	if ((strIPV6AddrSource == NULL) || (byteIPV6Addr == NULL) ){
	/* Returns Appropriate Error Structure*/
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	/* Memory allocation for temporary variable*/

	tempStr = (char*) calloc(5, sizeof(char));
	strIPV6Addr  = (char*) calloc(16, sizeof(char));
	strByte=(char*) calloc(5, sizeof(char));

	/*
	errorCode = validateIPv6Addr(strIPV6Addr);
	*/

	if(errorCode)	{
		return(errorCode);
	}


	for (counter=0; strIPV6AddrSource[counter] != '\0'; counter++) {
		for(count=0;(strIPV6AddrSource[counter] != ':')&&
			           (strIPV6AddrSource[counter] != 0);count++) {	
			strByte[count] = strIPV6AddrSource[counter];
			counter++;	
		}
		strByte[count] = '\0';
		sprintf(tempStr,"%04s", strByte);
		strcat(strIPV6Addr,tempStr);
	}


	/*!
	 * This function reads the strIPV6Addr and converts it into byte array.
	 */
	for (counter=0, count=0; strIPV6Addr[counter] != '\0'; counter++, 
		                                                    count++) {


		 if(((int)(strIPV6Addr[counter]) <= 57) && ((int)
			                   (strIPV6Addr[counter]) >= 48))	{
			 tempHex =(strIPV6Addr[counter]) - 48;

		 } 
		 	 
		 else {
			tempHex = toupper(strIPV6Addr[counter]) - 55;
		 }
		 
		 counter++;

	     if(((int)(strIPV6Addr[counter]) <= 57) && 
					((int) (strIPV6Addr[counter]) >= 48))	{
		    tempHex1 = (strIPV6Addr[counter]) - 48;
		 } 
		
		 else {
			tempHex1 = toupper(strIPV6Addr[counter]) - 55;
		 }
	
	    tempInt = (tempHex) * 16 + (tempHex1);

		byte = tempInt;

		byteIPV6Addr[count]=byte;	 
	}
	/*return success*/
	return (IPDR_OK);
}


/*!
 *  NAME:
 *      convByteArrayToIPV6Addr() - converts the byte array into IPV6Addr
 *                              (string format)
 *
 *  DESCRIPTION:
 *      This method takes as input the IPV6Addr byte array and converts 
 *		it into string format as per pattern specified in the NDM-U
 *		3.0 specification. That string is then returned as output.
 *  
 *  ARGUMENTS:
 *       -	byteIPV6Addr (IN)	
 *       -  strIPV6Addr (IN/OUT)
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


int convByteArrayToIPV6Addr(
							unsigned char* byteIPV6Addr,
							char* strIPV6Addr,
							int* pErrorCode
							)
{
	int counter = 0;
	int tempInt = 0;

	char* tempHex = NULL;

	if ((byteIPV6Addr == NULL) || (strIPV6Addr == NULL)) {
	/* Returns Appropriate Error Structure*/
		return (IPDR_NULL_INPUT_PARAMETER);
	}

	/* Memory allocation for temporary variable*/
	tempHex = (char*) calloc(6, sizeof(char));

	if (tempHex == NULL) {
	/* Returns Appropriate Error Structure*/
		return (IPDR_MEMORY_ALLOCATION_FAILED);
	} 

	/*!
	 * Converts to appropriate hex representation in specified format
	 * which represents pair of 2 bytes
	 */

	for (counter=0; counter<16; counter++) {
	
	    tempInt = (int)(byteIPV6Addr[counter]);
		
        sprintf(tempHex, "%x", tempInt);
        strcat(strIPV6Addr,tempHex);

		if ((counter == 1) || (counter == 3) || (counter == 5) || 
		       (counter == 7) ||(counter == 9)||(counter == 11) 
											||(counter == 13)){
	        strcat(strIPV6Addr,COLON);
		}
	}
	 	 
	free(tempHex);

	/*return success*/	
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      convIntToIPv4Addr() - Converts the input integer(32 bit) into an
 *							  IPv4 address string
 *
 *  DESCRIPTION:
 *      This method takes as input a 32 bit integer and converts it into 
 *      an  IPv4 address string. That integer is then returned as output.
 *  
 *  ARGUMENTS:
 *       -  IPV4 (IN)
 *       -	strIPV4 (IN/OUT)	
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


int convIntToIPv4Addr(int IPV4, char* strIPV4, int* pErrorCode)
{
	int count = 0;
	int shiftVal = 0;
    int addr = 0;
	char* strTemp;

	strTemp = (char*) calloc(16, sizeof(char));

	
	if (strIPV4 == NULL) {
	/* Returns Appropriate Error Structure*/
		return (IPDR_NULL_INPUT_PARAMETER);	
	}
	

	for(count=1;count<=4;count++) {
	
		shiftVal = 8*(4-count);
		addr=((IPV4 >> shiftVal) & 0xFF);

		if(count<4)
		sprintf(strTemp,"%d""%s",addr,".");
		else
		sprintf(strTemp,"%d",addr);

		strcat(strIPV4,strTemp);

	}
	free(strTemp);

	return(IPDR_OK);	
}


/*!
 *  NAME:
 *      convIPv4AddrToInt() - Converts the IPV4Addr string into an 
 *                            integer(32 bit)
 *
 *  DESCRIPTION:
 *      This method takes as input the IPV4Addr address string and 
 *      converts it into an integer. That integer is then returned as 
 *      output.
 *  
 *  ARGUMENTS:
 *       -  strIPV4 (IN)
 *       -	addr (IN/OUT)	
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


int convIPv4AddrToInt(char* strIPV4, int* addr, int* pErrorCode)
{
	char tempStr[4];
	int count = 0;
	int count1 = 0;
	int count2 = 0;

	int IPV4[4];


	if ((strIPV4 == NULL) || (addr == NULL)) {
	/* Returns Appropriate Error Structure*/
		return (IPDR_NULL_INPUT_PARAMETER);
	}
	
	#ifdef DEBUG_IPDR
	printf("\n IPV4 Len (%d) strIPV4(%s)", (int)strlen(strIPV4), strIPV4);
	#endif

	for(count = 0; count < (int)strlen(strIPV4); count++) {
		if (strIPV4[count] != '.') {
			tempStr[count1++] = strIPV4[count];
		} else {
			tempStr[count1] = '\0';
			IPV4[count2++] = atoi(tempStr);
			count1 = 0;
		}
	}
	
	tempStr[count1] = '\0';
	IPV4[count2++] = atoi(tempStr);

	for (count=0; count < 4; count++) {
		*addr = ((((int)*addr) << 8) + IPV4[count]);
	}
	return (IPDR_OK);	
}


 /*!
 *  NAME:
 *      isExisting() - function checks sub string exist in given string.
 *
 *  DESCRIPTION:
 *      1. Utility function. used for checking of existence of a sub string in a string.
 *	
 *  ARGUMENTS:
 *       -	Actual string {IN/OUT}
 *       -  sub string {IN/OUT}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int isExisting(char *srcString, char **characterArray)
{
	int count = 0;
	int attributeCount = 0;

	attributeCount = getAttributeLength(characterArray);

	for (count = 0; count < attributeCount; count++) {
		if (strcmp(srcString, characterArray[count]) == 0)
			return (1); /*String found*/
	}
	return (0); /*String not found*/

}

 /*!
 *  NAME:
 *      getIPDRSubstring() - function returns sub string of a given string.
 *
 *  DESCRIPTION:
 *      1. Utility function. returns sub string of a given string.
 *	
 *  ARGUMENTS:
 *       -	Actual string {IN/OUT}
 *       -  sub string {IN/OUT}
 *       -  starting location of sub string.
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int getIPDRSubstring(
					 char *srcString, 
					 char *destString, 
					 int checkChar
					 )
{
	unsigned int srcStrLen = 0;
	unsigned int count = 0;
	int newCount = 0;
	int flag = 0;
	
	srcStrLen = strlen(srcString);
	if (srcStrLen == 0) {
		destString = "";
		/*return success*/	
		return (IPDR_OK);
	}
		
	for (count = 0, newCount = 0; count < srcStrLen; count++) {
		if (srcString[count] == checkChar) flag = 1;
		if (flag == 1) destString[newCount++] = srcString[count + 1];		
	}
	
	if (flag == 1) destString[newCount] = '\0';
	if (flag == 0) strcpy(destString, srcString);
	
	/*return success*/	
	return (IPDR_OK);
}

 /*!
 *  NAME:
 *      getAttributeLength() - function for returning length of Attribute Name.
 *
 *  DESCRIPTION:
 *      1. This function returns length of Attribute Name.
 *	
 *  ARGUMENTS:
 *       -	Attribute Name {IN/OUT}
 *
 *  RETURNS 	
 *	     Returns length of input Attribute Name.
 */

int getAttributeLength(char **OptionalAttributes)
{
	int count = 0;
	while (*OptionalAttributes++ != NULL) count++;
	return count;
}


/*!
 *  NAME:
 *      convAttributeTypeToTypeCode() - function returns typeCode 
 *                                      for given Attribute Type
 *
 *  DESCRIPTION:
 *      1. Utility function. 
 *	
 *  ARGUMENTS:
 *       -	AttributeType {IN}
 *
 *  RETURNS 	
 *	     Returns type code
 */
int convAttributeTypeToTypeCode(char* AttributeType)
{
	
	char* tempAttributeType;
        unsigned int length = 0;

        length = strlen(AttributeType) + 1;
	tempAttributeType = (char *) calloc (length, sizeof(char));
	
	convertToUpper(AttributeType, tempAttributeType);
	
	if (strcmp("INTEGER", tempAttributeType) == 0) {
		free(tempAttributeType);
		return(IPDRDATA_LONG);
	}

	else if (strcmp("UNSIGNED_INTEGER", tempAttributeType) == 0) {
		free(tempAttributeType);
		return (IPDRDATA_ULONG);
	}

	else if (strcmp("HYPER", tempAttributeType) == 0) {
		free(tempAttributeType);
		return (IPDRDATA_LONGLONG);
	}

	else if (strcmp("UNSIGNED_HYPER", tempAttributeType) == 0) {
		free(tempAttributeType);
		return (IPDRDATA_ULONGLONG);
	}

	else if (strcmp("FLOAT", tempAttributeType) == 0) {
		free(tempAttributeType);
		return (IPDRDATA_FLOAT);
	}

	else if (strcmp("DOUBLE", tempAttributeType) == 0) {
		free(tempAttributeType);
		return (IPDRDATA_DOUBLE);
	}

	else if (strcmp("BYTE_ARRAY", tempAttributeType) == 0) {
		free(tempAttributeType);
		return (IPDRDATA_BYTEARRAY);
	}

	else if (strcmp("UTF8_ARRAY", tempAttributeType) == 0) {
		free(tempAttributeType);
		return (IPDRDATA_UTF8ARRAY);
	}

	else {
		free(tempAttributeType);
		return(IPDR_NON_PRIMITIVE_DATA_TYPE);
	}
}

/*!
 *  NAME:
 *      convTypeCodeToAttributeType() - function convert typeCode to attributeType
 *
 *  DESCRIPTION:
 *      1. Utility function. 
 *	
 *  ARGUMENTS:
 *       -	typeCode {IN/OUT}
 *       -  attributeType {IN/OUT}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int convTypeCodeToAttributeType(int typeCode, char* attributeType)
{
	
	if (typeCode == 1) 
		strcpy(attributeType, "INTEGER");

	else if (typeCode == 2) 
		strcpy(attributeType, "UNSIGNED_INTEGER");

	else if (typeCode == 3) 
		strcpy(attributeType, "HYPER");

	else if (typeCode == 4) 
		strcpy(attributeType, "UNSIGNED_HYPER");

	else if (typeCode == 5) 
		strcpy(attributeType, "FLOAT");

	else if (typeCode == 6) 
		strcpy(attributeType, "DOUBLE");

	else if (typeCode == 7) 
		strcpy(attributeType, "BYTE_ARRAY");
	
	else if(typeCode == 8) 
		strcpy(attributeType, "UTF8_ARRAY");

	return (IPDR_OK);
}

/*!
 *  NAME:
 *      convertToUpper() - function converts string to upper case
 *
 *  DESCRIPTION:
 *      1. Utility function. 
 *	
 *  ARGUMENTS:
 *       -	inputString {IN/OUT}
 *       -  upperCaseString {IN/OUT}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int convertToUpper(
				   char* inputString, 
				   char* upperCaseString
				   )
{
	unsigned int count = 0;
	unsigned int length = 0;

	if (inputString == NULL)
	return (IPDR_NULL_INPUT_PARAMETER);
	length = strlen(inputString);

	for (count = 0; count < length; count++) {
		if ((inputString[count] >= 97) && (inputString[count] <= 123)) {
		upperCaseString[count] = inputString[count] - 32;
		} else {
		upperCaseString[count] = inputString[count];
		}
	}
	upperCaseString[count] = '\0';
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      serializeFNFTypeStructure() - function converts FNFData structure to string
 *
 *  DESCRIPTION:
 *      1. Utility function. 
 *	
 *  ARGUMENTS:
 *       -	FNFType {IN/OUT}
 *       -  result {IN/OUT}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */ 
int serializeFNFTypeStructure(FNFType* pFNFType, char* result, int* pErrorCode)
{
	FNFType* pCurrent;
	char* tempString;
	char* tempString1;
	char* resultString;
    unsigned int length = 0;
    unsigned int lenString = 0;

    length = getLengthSerializeFNFType(pFNFType);

	pCurrent = pFNFType;
	
 	tempString1 = (char *) calloc (length, sizeof(char));
	resultString = (char *) calloc (sizeof(char), length);
         
		
	sprintf(tempString1, "%s^", pCurrent->serviceType_);

	while(pCurrent->pListAttributeDescriptor_ != NULL)
	{
            lenString = 10 + 
            strlen(pCurrent->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_) + 
            strlen(pCurrent->pListAttributeDescriptor_->pAttributeDescriptor_->attributeType_ ) + 
            strlen(pCurrent->pListAttributeDescriptor_->pAttributeDescriptor_->description_) +  
	    strlen(pCurrent->pListAttributeDescriptor_->pAttributeDescriptor_->derivedType_);
	    tempString = (char *) calloc (lenString, sizeof(char));
		sprintf(tempString, "%s*%s*%s*%s$", 
            pCurrent->pListAttributeDescriptor_->pAttributeDescriptor_->attributeName_,
            pCurrent->pListAttributeDescriptor_->pAttributeDescriptor_->attributeType_ ,
            pCurrent->pListAttributeDescriptor_->pAttributeDescriptor_->description_, 
	    pCurrent->pListAttributeDescriptor_->pAttributeDescriptor_->derivedType_);
		strcat(resultString, tempString);
	        free(tempString);
		pCurrent->pListAttributeDescriptor_ = pCurrent->pListAttributeDescriptor_->pNext_;
	}
	
	strcat(tempString1, resultString);
	strcpy(result, tempString1);

	free(resultString);
	free(tempString1);	

	return (IPDR_OK);
}

/*!
 *  NAME:
 *      serializeFNFDataStructure() - function converts FNFData structure to string
 *
 *  DESCRIPTION:
 *      1. Utility function. 
 *	
 *  ARGUMENTS:
 *       -	FNFData {IN/OUT}
 *       -  result {IN/OUT}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int serializeFNFDataStructure(FNFData* pFNFData, char* result, int* pErrorCode)
{
	ListAttributeDescriptor* pCurrent = NULL;
	char* tempString;
	char* tempString1;
	char* resultString;
        unsigned int length = 0;
        unsigned int lengthStr = 0;

        length = getLengthSerializeFNFData(pFNFData);

	pCurrent = pFNFData->pListAttributeDescriptor_; 
	
 	tempString1 = (char*) calloc(sizeof(char) , length);
	resultString = (char*) calloc(sizeof(char), length);
		
	sprintf(tempString1, "%s^", pFNFData->typeName_);

	while(pCurrent != NULL)
	{
                lengthStr = 10 +
                            strlen(pCurrent->pAttributeDescriptor_->attributeName_) +
                            strlen(pCurrent->pAttributeDescriptor_->attributeType_ ) +
                            strlen(pCurrent->pAttributeDescriptor_->description_) + 
			    strlen(pCurrent->pAttributeDescriptor_->derivedType_);
              
	        tempString = (char *) calloc (lengthStr, sizeof(char));

		sprintf(tempString, "%s*%s*%s*%s$", 
                            pCurrent->pAttributeDescriptor_->attributeName_,
                            pCurrent->pAttributeDescriptor_->attributeType_ ,
                            pCurrent->pAttributeDescriptor_->description_, 
			    pCurrent->pAttributeDescriptor_->derivedType_);
		strcat(resultString, tempString);
	        free(tempString);
		pCurrent = pCurrent->pNext_;
	}
	
	strcat(tempString1, resultString);
	strcpy(result, tempString1);
	

	free(resultString);
	free(tempString1);	
	freeListAttributeDescriptor (&pCurrent);
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      deserializeFNFTypeStructure() - function string to FNFType Structure
 *
 *  DESCRIPTION:
 *      1. Utility function. 
 *	
 *  ARGUMENTS:
 *       -	inputString {IN/}
 *       -  pRecordDescriptor {IN/OUT}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int deserializeFNFTypeStructure(char * inputString, RecordDescriptor* pRecordDescriptor)
{
	int pos = 0;
	int posFlag = 1;
	char* tempString;
	char* serviceTypeString;
	char temp[2];

	AttributeDescriptor* pDescriptorNode;

	pDescriptorNode = newAttributeDescriptor(); 
	
	tempString = (char*) calloc(sizeof(char), MAX_ATTRIBUTE_LENGTH);

        /* Getting length of serviceTypeString */
	for (pos = 0; inputString[pos] != '^'; pos++);
	serviceTypeString = (char *) calloc (pos + 2, sizeof(char));
	
	for (pos = 0; inputString[pos] != '^'; pos++){
		sprintf(temp, "%c", inputString[pos]);
		strcat(serviceTypeString , temp);
	}
	
	strcpy(pRecordDescriptor->typeName_,  serviceTypeString);

	for(pos = pos + 1; inputString[pos] != '\0'; pos++) {
		if (inputString[pos] != '$') {
			if ( inputString[pos] != '*') {
				sprintf(temp, "%c", inputString[pos]);
				strcat(tempString, temp);
			}
			else {
				if (posFlag == 1) {
					   /*
                                         if (strlen(tempString) > MAX_ATTRIBUTE_NAME) {
                                         realloc(pDescriptorNode->attributeName_, strlen(tempString) + 1); 
                                         if (pDescriptorNode->attributeName_ == NULL) {
                                          return (IPDR_ERROR);
                                         }
                                        }
					 */

					strcpy(pDescriptorNode->attributeName_ , tempString);
					strcpy(tempString, "");
				}
				if (posFlag == 2) {
					strcpy(pDescriptorNode->attributeType_ , tempString);
					strcpy(tempString, "");
				}
				if (posFlag == 3) {
					strcpy(pDescriptorNode->description_  , tempString);
					strcpy(tempString, "");
				}
				posFlag++;
			}
		}
		else {
			if (posFlag == 4) {
				strcpy(pDescriptorNode->derivedType_  , tempString);
				strcpy(tempString, "");
			}
			posFlag = 1;
			appendListAttributeDescriptor(
                                 &pRecordDescriptor->pListAttributeDescriptor_, 
				 pDescriptorNode->attributeName_,
                                 pDescriptorNode->attributeType_,
				 pDescriptorNode->description_,
				 pDescriptorNode->derivedType_);
		}
	}

	free(tempString);
	free(serviceTypeString);
	freeAttributeDescriptor(pDescriptorNode);

	
	return (IPDR_OK);
}



	/*!
 *  NAME:
 *      convLongToTimeHeader() - converts the 64 bits time representation 
 *						   into standard time format string(with GMT 
 *                         adjustment)
 *                          
 *
 *  DESCRIPTION:
 *      This method takes as input the time in the form of long 
 *      data type and converts it into string format(with GMT adjustment) 
 *      as per pattern specified in the NDM-U 3.0 specification. The string is 
 *		returned as output.
 *	
 *  ARGUMENTS:
 *       -	pTimeMilliSec (IN)	
 *		 -  timeStr (IN / OUT)	
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


int convLongToTimeHeader(
				   hyper* pTimeMilliSec,
				   char* timeStr,
				   int* pErrorCode
				  )
{
		
		struct tm* newTime = NULL;
		char *tempStr;
		char mnth[3];
		char day[3];
		char hour[3];
		char min[3];
		char sec[3];
		char temp[3];
		char milliSec[4];
		
		int milliSeconds=0;
		int count = 0;
		int start = 0;
		int length = 0;
		int value = 0;
		long TimeSec = 0;
		double TimeSec1 = 0;

		if ((pTimeMilliSec == NULL) || (timeStr == NULL)) {
	    /* Returns Appropriate Error Structure*/
			*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		    return (IPDR_WARNING);
		}
		
		
		tempStr = (char*) calloc(100, sizeof(char));

		sprintf(tempStr, "%.lf", (double)*pTimeMilliSec);
	
		length = strlen(tempStr);

		if(length < 3)
			start = 0;
		else 
			start =length - 3;

		for(count = start; count <= length; count++)
		{
			temp[value] = tempStr[count];
			value++;
		}
		milliSeconds = atoi(temp);
		
		TimeSec1 = ((double)(*pTimeMilliSec) - milliSeconds)/1000;

		TimeSec = (long)TimeSec1;

		time( &TimeSec );

		newTime = gmtime( &TimeSec );
			
		
		sprintf(timeStr, "%d%s", newTime->tm_year+1900, DASH);

		sprintf(mnth, "%d", newTime->tm_mon+1);

		if (newTime->tm_mon < 10) {
		    strcat(timeStr,ZERO);
		}

		strcat(timeStr,mnth);
			
		strcat(timeStr,DASH);

	    sprintf(day, "%d", newTime->tm_mday);

		if (newTime->tm_mday < 10) {
		    strcat(timeStr,ZERO);
		}
		
		strcat(timeStr,day);
		
		strcat(timeStr,T);

	    sprintf(hour, "%d", newTime->tm_hour);

		if (newTime->tm_hour < 10) {
		    strcat(timeStr,ZERO);
		}
	
		strcat(timeStr,hour);

		strcat(timeStr,COLON);

		sprintf(min, "%d", newTime->tm_min);

		if (newTime->tm_min < 10) {
		    strcat(timeStr,ZERO);
		}

		strcat(timeStr,min);

		strcat(timeStr,COLON);
		
		sprintf(sec, "%d", newTime->tm_sec);

		if (newTime->tm_sec < 10) {
		    strcat(timeStr,ZERO);
		}

		strcat(timeStr,sec);

		strcat(timeStr,DOT);
		
		/*if(milliSeconds != 0) {
			sprintf(milliSec, "%d", milliSeconds);
		}
		else {
			sprintf(milliSec, "%s", "000");
		}

		strcat(timeStr,milliSec);*/

		if(milliSeconds == 0) {
				strcat(timeStr, "000");
		}
		else {
				sprintf(milliSec, "%d", milliSeconds);

				if ((milliSeconds >0 ) && (milliSeconds <10)){

						strcat(timeStr, "00");
						strcat(timeStr, milliSec);
					}
				else if ((milliSeconds >=10) && (milliSeconds <100)){
						strcat(timeStr, "0");
						strcat(timeStr, milliSec);
				}
				else {
					
						strcat(timeStr, milliSec);
				}

		}


		strcat(timeStr,Z);
		
		free(tempStr);
		/*return success*/
		return (IPDR_OK);
}


/*!
 *  NAME:
 *      getFileLength() - Calculates the length of file
 *
 *  DESCRIPTION:
 *      This method takes filename as input and Calculates the length of file
 *          in bytes
 *	
 *  ARGUMENTS:
 *       -  fileName (IN)	
 *
 *  RETURNS 	
 *	     Returns length of the file if the operation is successful or -1 
 *            in case of failure.
 */

long getFileLength(char* fileName)
{
	int result = 0;
#ifdef WIN32
	struct _stat buf;
    result = _stat(fileName, &buf);
#else
	struct stat buf;
    result = stat(fileName, &buf);
#endif

   if (result != 0)
		buf.st_size = -1;
   return (buf.st_size);
}



/*!
 *  NAME:
 *      getLengthSerializeFNFType() - Calculates the length of 
 *               FNFType for which serialized string is to be 
 *               generated
 *
 *  DESCRIPTION:
 *      This method takes FNFType as input and Calculates 
 *               the length of members in bytes
 * 
 *  ARGUMENTS:
 *       -  pFNFType (IN) 
 *
 *  RETURNS  
 *      Returns length of the serialized string if the operation is successful or -1 
 *            in case of failure.
 */
 
int getLengthSerializeFNFType(FNFType* pFNFType)
{
 unsigned int lenSerializeString = 0;
 unsigned int count = 0;
 ListAttributeDescriptor* pCurrent = NULL;
 
 lenSerializeString = strlen(pFNFType->serviceType_) + 1;
 if (pFNFType->pListAttributeDescriptor_ != NULL) {
  pCurrent = pFNFType->pListAttributeDescriptor_;
 
  while (pCurrent != NULL) {
   lenSerializeString = lenSerializeString +
    strlen(pCurrent->pAttributeDescriptor_->attributeName_) + 1;
   lenSerializeString = lenSerializeString +
    strlen(pCurrent->pAttributeDescriptor_->attributeType_) + 1;
   lenSerializeString = lenSerializeString +
    strlen(pCurrent->pAttributeDescriptor_->description_) + 1;
   lenSerializeString = lenSerializeString +
    strlen(pCurrent->pAttributeDescriptor_->derivedType_) + 1;
 
   /* Include No of delimeters to be added for one AttributeDescriptor */
   lenSerializeString = lenSerializeString + 4;
 
   pCurrent = pCurrent->pNext_; 
   count++;
  } 
 
  return (lenSerializeString);
 } else {
  return (-1);
 }
}
 

 
/*!
 *  NAME:
 *      getLengthSerializeFNFData() - Calculates the length of 
 *               FNFData for which serialized string is to be 
 *               generated
 *
 *  DESCRIPTION:
 *      This method takes FNFData as input and Calculates 
 *               the length of members in bytes
 * 
 *  ARGUMENTS:
 *       -  pFNFData (IN) 
 *
 *  RETURNS  
 *      Returns length of the serialized string if the operation is successful or -1 
 *            in case of failure.
 */
 
int getLengthSerializeFNFData(FNFData* pFNFData)
{
 unsigned int lenSerializeString = 0;
 unsigned int count = 0;
 ListAttributeDescriptor* pCurrent = NULL;

 lenSerializeString = strlen(pFNFData->typeName_) + 1;
 if (pFNFData->pListAttributeDescriptor_ != NULL) {
  pCurrent = pFNFData->pListAttributeDescriptor_;

  while (pCurrent != NULL) {
   lenSerializeString = lenSerializeString +
    strlen(pCurrent->pAttributeDescriptor_->attributeName_) + 1;
   lenSerializeString = lenSerializeString +
    strlen(pCurrent->pAttributeDescriptor_->attributeType_) + 1;
   lenSerializeString = lenSerializeString +
    strlen(pCurrent->pAttributeDescriptor_->description_) + 1;
   lenSerializeString = lenSerializeString +
    strlen(pCurrent->pAttributeDescriptor_->derivedType_) + 1;

   /* Include No of delimeters to be added for one AttributeDescriptor */
   lenSerializeString = lenSerializeString + 4;

   pCurrent = pCurrent->pNext_;
   count++;
  }
  return (lenSerializeString);
 } else {
  return (-1);
 }
}


/*!
*NAME:
*     getNameSpaceID() 
*
*DESCRIPTION:
*     This method is used to get the service type from a given key.
* 
*ARGUMENTS:
* - String storing the value of key {IN}
* - String having the service ID {OUT}
*
* RETURNS 
*  Returns Zero or appropriate error code
*/
int getNameSpaceID(
				   char* service,
				   char* key
				  )
{
 char* temp;
 char* token;
 temp = (char *) calloc(strlen(key) + 1, sizeof(char));
 strcpy(temp, key);
 token = strtok(temp, ":");
 if(!strcmp(key,token)) strcpy(service,"ipdr");
 else 
  strcpy(service, token);
 free(temp);
 return (IPDR_OK);
}
 
 

/*!
*NAME:
*     getAttributeName() 
*
*DESCRIPTION:
*     This method extracts the attribute name from the given Key.
* 
*ARGUMENTS:
* - String containing the key {IN}
* - String containing Attribute Name {OUT}
*
* RETURNS 
*  Returns Zero or appropriate error code
*/

 
int getAttributeName(
      char* name,
      char* key
     ) 
{
 int i = 0;
 int flag = 0;
 if(!(strchr( key, ':' ))){
  strcpy(name,key);
 }
    else {
  while (key[i] != '\0') {
   if(key[i] == ':') flag = i; 
   i++;
  }
  i = 0;
  if(flag != 0){
   flag++;
   while(key[i] != '\0'){
    name[i] = key[flag];
    i++;
    flag++;
   }
  }
 }
 if ( name == NULL )
  return ( IPDR_ERROR );
 else
  return ( IPDR_OK);
}

 

/*!
*NAME:
*     getNameSpaceURI() 
*
*DESCRIPTION:
*     This method is used to get the NameSpaceURI correponding to a 
* particular NameSpaceID.
* 
*ARGUMENTS:
* - Poiter to List of NameSpaceInfo {IN}
* - String having serviceID {IN}
*      - String storing NameSpaceURI {OUT}
*
* RETURNS 
*  Returns Zero or appropriate error code
*/

int getNameSpaceURI( 
     ListNameSpaceInfo* pListSpaceInfo,
     char* nameSpaceID, 
     char* nameSpaceFnfURI
     )
{
 if( pListSpaceInfo == NULL || nameSpaceID == NULL)
  return(IPDR_WARNING);
 while(pListSpaceInfo != NULL ){
  if(!strcmp(pListSpaceInfo->NameSpaceInfo_.nameSpaceID_, nameSpaceID)) {
   strcpy(nameSpaceFnfURI,pListSpaceInfo->NameSpaceInfo_.nameSpaceURI_); 
   break;
  }
  pListSpaceInfo = pListSpaceInfo->pNext_;
 }
 if(nameSpaceFnfURI == NULL)
        return( IPDR_ERROR);
 return( IPDR_OK);
 
}

/*!
*NAME:
*     convertTolower() 
*
*DESCRIPTION:
*     This method is used to convert the given string into the lower case 
* 
*ARGUMENTS:
* - Poiter to input string {IN}
* - Pointer to converted string {OUT}
*
* RETURNS 
*  Returns Zero on success
*/
 
int convertTolower(
				   char* inputString,
				   char* lowerCaseString
				  )
{
	unsigned int count = 0;
	unsigned int length = 0;

	if (inputString == NULL)
	return (IPDR_NULL_INPUT_PARAMETER);
	length = strlen(inputString);

	for (count = 0; count < length; count++) {
		if ((inputString[count] >= 65) && (inputString[count] <= 90)) {
		lowerCaseString[count] = inputString[count] + 32;
		} else {
		lowerCaseString[count] = inputString[count];
		}
	}
	lowerCaseString[count] = '\0';
	return (IPDR_OK);
}


/*!
*NAME:
*     tokenizeKey() 
*
*DESCRIPTION:
*     This function tokenises the incoming key, and stores the result
* in various variables.
* 
*ARGUMENTS:
* - String containing the Key {IN}
* - String storing the serviceType {OUT}
*      - String storing NameSpaceURI {OUT}
*      - String storing the Attribute Name {OUT}
*
* RETURNS 
*  Returns Zero or appropriate error code
*/


int tokenizeKey(
				char* serviceType,
				char* nameSpaceURI, 
				char* attributeName,
				char* key
				)
{
   char* token;
   char seps[] = "$";
   int tokenCount = 0;

   char *tmpString;
   unsigned int strLength = 0;

   strLength = strlen(key) + 1;
   tmpString = (char *) calloc (strLength, sizeof(char));
   strcpy(tmpString, key);

   token = strtok( tmpString, seps );

   while( token != NULL )
   {
      if (tokenCount == 0) strcpy(serviceType, token);
      if (tokenCount == 1) strcpy(nameSpaceURI, token);
      if (tokenCount == 2) strcpy(attributeName, token);
      token = strtok( NULL, seps );
      tokenCount++;
 }

   free(tmpString);
   free(token);

   return (IPDR_OK);
}



/*!
*NAME:
*     validateDocID() 
*
*DESCRIPTION:
*     This function validates the docID.
* 
*ARGUMENTS:
* - docID {IN}
* - pErrorCode {IN/OUT}
*
* RETURNS 
*  Returns Zero or appropriate error code
*/

int validateDocID(char* docID, int* pErrorCode) 
{
	int counter = 0;
	int flag = 0;
	int hCount = 0;

	if (docID == NULL){
		*pErrorCode = IPDR_NULL_INPUT_PARAMETER;
		return (IPDR_ERROR);
	}

	if(strlen(docID) != LENGTH_UUID_STRING) {
		*pErrorCode = IPDR_INVALID_UUID;
		return (IPDR_ERROR);
	}
	
	for (counter=0; docID[counter] != '\0'; counter++) {
		if (docID[counter] == '-'){
			if(counter == 8 && docID[counter + 5] == '-'  && docID[counter + 10] == '-' && docID[counter + 15] == '-')
			flag = 1;
			if(counter == 13 && docID[counter - 5] == '-'  && docID[counter + 5] == '-' && docID[counter + 10] == '-')
			flag = 1;
			if(counter == 18 && docID[counter - 10] == '-'  && docID[counter - 5] == '-' && docID[counter + 5] == '-')
			flag = 1;
			if(counter == 23 && docID[counter + 5] == '-'  && docID[counter + 10] == '-' && docID[counter + 15] == '-')
			flag = 1;
			hCount++;
		}
	}

	if(hCount > 4) {
		*pErrorCode = IPDR_INVALID_UUID;
		return (IPDR_ERROR);
	}

	if (flag == 0) {
		*pErrorCode = IPDR_INVALID_UUID;
		return (IPDR_ERROR);
	}
	
	for (counter=0; docID[counter] != '\0'; counter++) {
		if(!isxdigit(toupper(docID[counter])) && docID[counter] != '-') {
			*pErrorCode = IPDR_INVALID_UUID;
			return (IPDR_ERROR);
		}
	}
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      validateAttributeName() - This function put Input Descriptor 
 *                                        in a Name-Type Code structure (Hash Table).   
 *                     
 *  DESCRIPTION:
 *     	- This function retrieve the Name-Type pair set from the addresses given in 
 *        Input as array of pointers
 *      - Call hashTableCreation & hashTablePut function for creating 
 *        AttributeDescriptor Hash Table (as a set of Name-Type pair) 
 *		  for validating for dublicate attibute names
 *
 *	ARGUMENTS:
 *      - ListAttributeDescriptor {IN/ OUT}
 *      - AttributeDescriptorHashTable {local variable}
 *		- pErrorCode { IN / OUT }
 *
 *  RETURNS 	
 *	    Returns Zero or appropriate error code
*/

int validateAttributeName(
						 IPDRCommonParameters *pIPDRCommonParameters, 
						 ListAttributeDescriptor *pListAttributeDescriptor, 
						 int* pErrorCode
						)	
{	

	char* resultGet = NULL;
	
	int count = 0;
	int numOfBuckets = 0;
	int listAttributeDescriptorLength = 0;
	
	HashTable* AttributeDescriptorHashTable;

	if(pListAttributeDescriptor == NULL || 
		pIPDRCommonParameters == NULL) {
	    *pErrorCode = IPDR_NULL_INPUT_PARAMETER;
	    return (IPDR_ERROR);
	}
	
	/* Number of Hash Table elements (Buckets) = RECORDS */
	numOfBuckets = HASH_BUCKETS;
	AttributeDescriptorHashTable = HashTableCreate(numOfBuckets); 
	listAttributeDescriptorLength = getLengthListAttributeDescriptor(pListAttributeDescriptor);
    
	/*
	This function will insert a Attribute Descriptor in the hash table and perform the 
        validation for dublicate Attribute Name Check using hash table properties. It will take Attribute 
	Name as Key for hash table and hash table has a property that Key cannot be dublicated.	
    */	
	for(count = 0; count < listAttributeDescriptorLength; count++) {

		resultGet = (char *) HashTableGet(
			        AttributeDescriptorHashTable, 
				    pListAttributeDescriptor->pAttributeDescriptor_->attributeName_
					);

		if (resultGet == NULL) {
		
			HashTablePutstr(AttributeDescriptorHashTable, 
                            pListAttributeDescriptor->pAttributeDescriptor_->attributeName_, 
                            pListAttributeDescriptor->pAttributeDescriptor_->attributeType_);	
		
			pListAttributeDescriptor = pListAttributeDescriptor->pNext_; 
		}
		else {
			*pErrorCode = IPDR_DUPLICATE_ATTRIBUTE_NAME;
			return(IPDR_ERROR);
		}
	}
	
	HashTableDestroy (AttributeDescriptorHashTable);
	
	return (IPDR_OK);
}
