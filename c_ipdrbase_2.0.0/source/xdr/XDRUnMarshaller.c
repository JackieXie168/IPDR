/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/



/*****************************************************
 * File                 : XDRUnMarshaller            *
 * Description          :                            *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 *                                                   *
 *****************************************************/

#include "xdr/XDRUnMarshaller.h"
#include "utils/IPDRCommon.h"


/*
 * Marsheller Helper Header Library. 
 * This will be created as a separate header file & 
 * the functions will be available to the above C Program.   
 * Header File Name: "XDRMarshaller.h"
 */

/*!
 *  NAME:
 *      read_long() -  Unmarshalling of long value
 *
 *  DESCRIPTION:
 *         Unmarshalling of long value
 *  
 *  ARGUMENTS:
 *	    - pStream {IN}
 *
 *  RETURNS 	
 *	     Returns the value which has been read from input stream
 */


int read_long(void* pStream)
{
        char tempByte[3] = "ff";
        int byteToPrint;
        int count = 0;
        unsigned char ch[5] = "";
        int RawValue = 0;

        fread(ch, sizeof(char), INT_BYTES, pStream);
        for ( count = 0; count < INT_BYTES; count++ )
        {
             sprintf(tempByte, "%02x", ch[count]);
             byteToPrint = convReadIntHexToInt(count, tempByte);
             RawValue = RawValue + byteToPrint;
        }

        return(RawValue);
}


/*!
 *  NAME:
 *      convReadIntHexToInt() -  Converts Hex to Int for integer
 *
 *  DESCRIPTION:
 *         Converts Hex to Int for integer for unmarshalling of integer
 *         value
 *  
 *  ARGUMENTS:
 *	    - count {IN}
 *      - iHexa {IN/OUT}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int convReadIntHexToInt(int count, char iHexa[3])
{
	int MSB = 0;
	int LSB = 0;
	char chMSB[2];
	char chLSB[2];
	chMSB[0] = iHexa[0];
	chMSB[1] = '\0';
	chLSB[0] = iHexa[1];
	chLSB[1] = '\0';

	convReadIntCharHexToInt(chMSB, &MSB);
	convReadIntCharHexToInt(chLSB, &LSB);

	if(count == 0) return ( MSB * 268435456 + LSB * 16777216 );
	if(count == 1) return ( MSB * 1048576   + LSB * 65536);
	if(count == 2) return ( MSB * 4096      + LSB * 256);
	if(count == 3) return ( MSB * 16        + LSB );
	
	return(0);
}


/*!
 *  NAME:
 *      convReadIntCharHexToInt() -  Converts Hex array to Int for integer
 *
 *  DESCRIPTION:                                                            
 *        Converts Hex arrayto Int for integer for unmarshalling of integer 
 *         value
 *  
 *  ARGUMENTS:
 *	    - hex {IN}
 *      - integer {IN/OUT}
 *
 *  RETURNS 	
 *       void
 */

void convReadIntCharHexToInt(char *hex,int* SB)
{

     if (hex[0] <= '9' && hex[0] > '0')
                *SB = atoi(hex);

       if (hex[0] == 'a') *SB = 10 ;
        if (hex[0] == 'b') *SB = 11 ;
        if (hex[0] == 'c') *SB = 12 ;
        if (hex[0] == 'd') *SB = 13 ;
       if (hex[0] == 'e') *SB = 14 ;
        if (hex[0] == 'f') *SB = 15 ;
}


/*!
 *  NAME:
 *      read_double() -  Unmarshalling of double
 *
 *  DESCRIPTION:
 *        Unmarshalling of double
 *  
 *  ARGUMENTS:
 *	    - streamPtr {IN}
 *
 *  RETURNS 	
 *	     Returns the double value which has been read from input stream
 */

double read_double(void *streamPtr)
{
	int count = 0;
	char signDouble;
	int countNext = 0;
	int dExponent;
	double dMantissa;
	double thisDouble = 0;
	unsigned char HexaDouble[DOUBLE_HEXA_BYTES];
	unsigned char HexaTemp[2];
	unsigned char BinaryTemp[4];
	unsigned char HexaComplete[DOUBLE_HEXA_BITS];
	unsigned char Exponent[DOUBLE_EXPONENT_BITS     ];
	unsigned char Mantissa[DOUBLE_MANTISSA_BITS];
	unsigned char BinaryComplete[DOUBLE_HEXA_BITS * 4];

	fread(HexaDouble, sizeof(char), DOUBLE_HEXA_BYTES, streamPtr);
	
	for (count = 0; count < DOUBLE_HEXA_BYTES; count++) {
		sprintf((char *)HexaTemp, "%02x", HexaDouble[count]);
		HexaComplete[2 * count] = HexaTemp[0];
		HexaComplete[(2 * count) + 1] = HexaTemp[1];
	}

	for (count = 0; count < DOUBLE_HEXA_BITS; count++) {
		convReadDoubleHexToBinary(HexaComplete[count], (char *)BinaryTemp);
		for (countNext = 0; countNext < 4; countNext++) {
			BinaryComplete[countNext + (4 * count)] = BinaryTemp[countNext];
		}
	}

	for (count = 0; count < DOUBLE_EXPONENT_BITS     ; count++)
		Exponent[count] = BinaryComplete[count];

	for (count = 0; count < DOUBLE_MANTISSA_BITS; count++)
		Mantissa[count] = BinaryComplete[count + DOUBLE_EXPONENT_BITS     ];

	signDouble = BinaryComplete[0];

	dExponent = getReadDoubleExponent((char *)Exponent);
	dMantissa = getReadDoubleMantissa((char *)Mantissa);
	thisDouble = pow(2, dExponent) * dMantissa;
	if (signDouble == '1') 
		return -(thisDouble);
	else
		return thisDouble;
}



/*!
 *  NAME:
 *      getReadDoubleExponent() - Function for returning exponent value of 
 *                                 input double value.
 *                  
 *  DESCRIPTION: Function for returning exponent value of input double 
 *               value.
 *      
 *	ARGUMENTS:
 *	    - char array for double value {IN}
 *
 *  RETURNS 	
 *	     Returns Exponent (integer value) of input Double value.
 */

int getReadDoubleExponent(char Exponent[DOUBLE_EXPONENT_BITS])
{
	int count = 0;
	int reverseCount = 0;
	double tempDouble = 0;
	int dExponent = 0;

	for (count = 1, reverseCount = DOUBLE_EXPONENT_BITS      - 2; 
		 count < DOUBLE_EXPONENT_BITS     ; 
		 count++, reverseCount--) {
			 if (Exponent[count] == '1') 
				dExponent = dExponent + (int) pow(2, reverseCount);
	}
	dExponent = dExponent - DOUBLE_CONSTANT;
	return dExponent;
}

/*!
 *  NAME:
 *      getReadDoubleMantissa() - Function for returning mantissa value of 
 *                                 input double value.
 *                  
 *  DESCRIPTION: Function for returning mantissa value of input double 
 *               value.
 *      
 *	ARGUMENTS:
 *	    - char array for double value {IN}
 *
 *  RETURNS 	
 *	     Returns Exponent (integer value) of input Double value.
 */

double getReadDoubleMantissa(char Mantissa[DOUBLE_MANTISSA_BITS])
{
	int count = 0;
	int reverseCount = 0;
	double tempDouble = 0;
	double dMantissa = 0;

	for (count = 0; 
		 count < DOUBLE_MANTISSA_BITS; 
		 count++) {
			 if (Mantissa[count] == '1') 
				dMantissa = dMantissa + pow(2, - (count + 1));
	}
		 dMantissa = dMantissa + 1;
	return dMantissa;
}

/*!
 *  NAME:
 *      convReadDoubleHexToBinary() - Function for converting hex to binary
 *                                    for Reading a double value
 *                  
 *  DESCRIPTION: Function for converting hex to binary for unmarshalling a
 *               double value
 *      
 *	ARGUMENTS:
 *      - hexa {IN}
 *	    - char array for binary value {IN/OUT}
 *
 *   RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */



int convReadDoubleHexToBinary(char hexa, char binary[4])
{
	switch(hexa)
	{
	case '0':	strcpy(binary, "0000");
				break;
	case '1':	strcpy(binary, "0001");
				break;
	case '2':	strcpy(binary, "0010");
				break;
	case '3':	strcpy(binary, "0011");
				break;
	case '4':	strcpy(binary, "0100");
				break;
	case '5':	strcpy(binary, "0101");
				break;
	case '6':	strcpy(binary, "0110");
				break;
	case '7':	strcpy(binary, "0111");
				break;
	case '8':	strcpy(binary, "1000");
				break;
	case '9':	strcpy(binary, "1001");
				break;
	case 'a':	strcpy(binary, "1010");
				break;
	case 'A':	strcpy(binary, "1010");
				break;
	case 'b':	strcpy(binary, "1011");
				break;
	case 'B':	strcpy(binary, "1011");
				break;
	case 'c':	strcpy(binary, "1100");
				break;
	case 'C':	strcpy(binary, "1100");
				break;
	case 'd':	strcpy(binary, "1101");
				break;
	case 'D':	strcpy(binary, "1101");
				break;
	case 'e':	strcpy(binary, "1110");
				break;
	case 'E':	strcpy(binary, "1110");
				break;
	case 'f':	strcpy(binary, "1111");
				break;
	case 'F':	strcpy(binary, "1111");
				break;
	default:	break;
	}

	return 0;
}

/*!
 *  NAME:
 *      read_float() -  Unmarshalling of float value
 *
 *  DESCRIPTION:
 *         Unmarshalling of float value
 *  
 *  ARGUMENTS:
 *	    - streamPtr {IN}
 *
 *  RETURNS 	
 *	     Returns the float value which has been read from input stream
 */

float read_float(void *streamPtr)
{
	int count = 0;
	char signDouble;
	int countNext = 0;
	int dExponent = 0;
	float dMantissa = 0;
	float thisDouble = 0;
	unsigned char HexaDouble[FLOAT_HEXA_BYTES];
	unsigned char HexaTemp[2];
	unsigned char BinaryTemp[4];
	unsigned char HexaComplete[FLOAT_HEXA_BITS];
	unsigned char Exponent[FLOAT_EXPONENT_BITS];
	unsigned char Mantissa[FLOAT_MANTISSA_BITS];
	unsigned char BinaryComplete[FLOAT_HEXA_BITS * 4];

	fread(HexaDouble, sizeof(char), FLOAT_HEXA_BYTES, streamPtr);
	
	for (count = 0; count < FLOAT_HEXA_BYTES; count++) {
		sprintf((char *)HexaTemp, "%02x", HexaDouble[count]);
		HexaComplete[2 * count] = HexaTemp[0];
		HexaComplete[(2 * count) + 1] = HexaTemp[1];
	}

	for (count = 0; count < FLOAT_HEXA_BITS; count++) {
		convFloatReadHexToBinary(HexaComplete[count], (char *)BinaryTemp);
		for (countNext = 0; countNext < 4; countNext++) {
			BinaryComplete[countNext + (4 * count)] = BinaryTemp[countNext];
		}
	}

	for (count = 0; count < FLOAT_EXPONENT_BITS; count++)
		Exponent[count] = BinaryComplete[count];

	for (count = 0; count < FLOAT_MANTISSA_BITS; count++)
		Mantissa[count] = BinaryComplete[count + FLOAT_EXPONENT_BITS];

	signDouble = BinaryComplete[0];
	
	dExponent = getFloatReadExponent((char *)Exponent);
	dMantissa = getFloatReadMantissa((char *)Mantissa);
	thisDouble = (float) pow(2, dExponent) * dMantissa;
	
	if (signDouble == '1') 
		return -(thisDouble);
	else
		return thisDouble;
}


/*!
 *  NAME:
 *      getFloatReadExponent() - Function for returning exponent value of 
 *                                 input float value.
 *                  
 *  DESCRIPTION: Function for returning exponent value of input float 
 *               value.
 *      
 *	ARGUMENTS:
 *	    -char array for float value {IN}
 *
 *  RETURNS 	
 *	     Returns Exponent (integer value) of input float value.
 */

int getFloatReadExponent(char Exponent[FLOAT_EXPONENT_BITS])
{
	int count = 0;
	int reverseCount = 0;
	double tempDouble = 0;
	int dExponent = 0;

	for (count = 1, reverseCount = FLOAT_EXPONENT_BITS - 2; 
		 count < FLOAT_EXPONENT_BITS; 
		 count++, reverseCount--) {
			 if (Exponent[count] == '1') 
				dExponent = dExponent + (int) pow(2, reverseCount);
	}
	dExponent = dExponent - FLOAT_CONSTANT;
	return dExponent;
}

/*!
 *  NAME:
 *      getFloatReadMantissa() - Function for returning mantissa value of 
 *                                 input float value.
 *                  
 *  DESCRIPTION: Function for returning mantissa value of input float 
 *               value.
 *      
 *	ARGUMENTS:
 *	    - char array for float value {IN}
 *
 *  RETURNS 	
 *	     Returns Exponent (integer value) of input float value.
 */


float getFloatReadMantissa(char Mantissa[FLOAT_MANTISSA_BITS])
{
	int count = 0;
	int reverseCount = 0;
	float tempDouble = 0;
	float dMantissa = 0;

	for (count = 0; 
		 count < FLOAT_MANTISSA_BITS; 
		 count++) {
			 if (Mantissa[count] == '1') 
				dMantissa = dMantissa + (float) pow(2, - (count + 1));
	}
		 dMantissa = dMantissa + 1;
	return dMantissa;
}

/*!
 *  NAME:
 *      convFloatReadHexToBinary() - Function for converting hex to binary
 *                                    for Reading a double value
 *                  
 *  DESCRIPTION: Function for converting hex to binary for unmarshalling a
 *               float value
 *      
 *	ARGUMENTS:
 *      - hexa {IN}
 *	    - char array for binary value {IN/OUT}
 *
 *   RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int convFloatReadHexToBinary(char hexa, char binary[4])
{
	switch(hexa)
	{
	case '0':	strcpy(binary, "0000");
				break;
	case '1':	strcpy(binary, "0001");
				break;
	case '2':	strcpy(binary, "0010");
				break;
	case '3':	strcpy(binary, "0011");
				break;
	case '4':	strcpy(binary, "0100");
				break;
	case '5':	strcpy(binary, "0101");
				break;
	case '6':	strcpy(binary, "0110");
				break;
	case '7':	strcpy(binary, "0111");
				break;
	case '8':	strcpy(binary, "1000");
				break;
	case '9':	strcpy(binary, "1001");
				break;
	case 'a':	strcpy(binary, "1010");
				break;
	case 'A':	strcpy(binary, "1010");
				break;
	case 'b':	strcpy(binary, "1011");
				break;
	case 'B':	strcpy(binary, "1011");
				break;
	case 'c':	strcpy(binary, "1100");
				break;
	case 'C':	strcpy(binary, "1100");
				break;
	case 'd':	strcpy(binary, "1101");
				break;
	case 'D':	strcpy(binary, "1101");
				break;
	case 'e':	strcpy(binary, "1110");
				break;
	case 'E':	strcpy(binary, "1110");
				break;
	case 'f':	strcpy(binary, "1111");
				break;
	case 'F':	strcpy(binary, "1111");
				break;
	default:	break;
	}
	return 0;
}

/*!
 *  NAME:
 *      read_u_long() -  Unmarshalling of unsigned long value
 *
 *  DESCRIPTION:
 *         Unmarshalling of unsigned long value
 *  
 *  ARGUMENTS:
 *	    - pStream {IN}
 *
 *  RETURNS 	
 *	     Returns the unsigned long value which has been read from input 
 *       stream
 */

unsigned int read_u_long(void* pStream)
{
        char tempByte[3] = "ff";
        unsigned int byteToPrint;
        int count = 0;
        unsigned char ch[5];
		unsigned int RawValue = 0;  
        fread(ch, sizeof(char), 4, pStream);
       for (count = 0; count < U_INT_MAXCOUNT_4; count++)
       {
            sprintf(tempByte, "%02x", ch[count]);
            byteToPrint = convReadUIntHexToInt(count, tempByte);
            RawValue = RawValue + byteToPrint;
       }
       return(RawValue);
}

/*!
 *  NAME:
 *      convReadUIntHexToInt() - Function for converting hex to int
 *                                    for Reading a unsigned int value
 *                  
 *  DESCRIPTION: Function for converting hex to int for reading an
 *               unsigned int value
 *      
 *	ARGUMENTS:
 *      - hexa {IN}
 *	    - char array for binary value {IN/OUT}
 *
 *   RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

unsigned int convReadUIntHexToInt(int count, char iHexa[3])
{
        int MSB = 0;
        int LSB = 0;
        char chMSB[2];
        char chLSB[2];

        chMSB[0] = iHexa[0];
        chLSB[0] = iHexa[1];
        chMSB[1] = '\0';
        chLSB[1] = '\0';

        convReadUIntCharHexToInt(chMSB, &MSB);
        convReadUIntCharHexToInt(chLSB, &LSB);

        if(count == 0) return ( MSB * 268435456 + LSB * 16777216 );
        if(count == 1) return ( MSB * 1048576   + LSB * 65536);
        if(count == 2) return ( MSB * 4096      + LSB * 256);
        if(count == 3) return ( MSB * 16        + LSB );

		return (0);
}


/*!
 *  NAME:
 *      convReadUIntCharHexToInt() -  Converts Hex array to Int for 
 *                                     unsigned integer
 *
 *  DESCRIPTION:                                                            
 *        Converts Hex arrayto Int for undigned integer for unmarshalling 
 *         of integer value
 *  
 *  ARGUMENTS:
 *	    - hex {IN}
 *      - integer {IN/OUT}
 *
 *  RETURNS 	
 *       void
 */

void convReadUIntCharHexToInt(char* hex, int* SB)
{

     if (hex[0] <= '9' && hex[0] > '0')
                *SB = atoi(hex);

        if (hex[0] == 'a') *SB = 10 ;
        if (hex[0] == 'b') *SB = 11 ;
        if (hex[0] == 'c') *SB = 12 ;
        if (hex[0] == 'd') *SB = 13 ;
        if (hex[0] == 'e') *SB = 14 ;
        if (hex[0] == 'f') *SB = 15 ;
}

/*!
 *  NAME:
 *      read_string() -  Unmarshalling of string value
 *
 *  DESCRIPTION:
 *         Unmarshalling of string value
 *  
 *  ARGUMENTS:
 *	    - pStream {IN}
 *      - length {IN}
 *
 *  RETURNS 	
 *	     Returns the value which has been read from input stream
 */

char* read_string(int length, void* pStream)
{
    char* pRawValue;
    int quadLength = 0;

/*
if((strcmp(gVersion,IPDR_VERSION_3_1)) == 0) {
    if ( (length % 4) != 0)
          quadLength = length + (4 - (length % 4));
    else
          quadLength = length;
}
else {
	quadLength = length;
}
*/
    pRawValue = (char *) calloc(sizeof(char), (length + 1));
    fgets(pRawValue, quadLength + 1, pStream);
    pRawValue[length] = '\0';
    return (pRawValue);
}


/*
 *  NAME:
 *      read_utf8() - Unmarshalling of UTF8
 *                  
 *  DESCRIPTION:      Unmarshalling of UTF8 
 *      
 *	ARGUMENTS:
 *      - length {IN} 
 *	    - pStream {IN}
 *
 *  RETURNS 	
 *	      Returns the value which has been read
*/

wchar* read_wstring(int length, void* pStream, char* schemaVer)
{ 
 wchar* pRawValue;

 pRawValue = (wchar *) 
  calloc (sizeof(wchar), (length + 3));
 
/* Code Change to read the structure member for the schema version */
if((strcmp(schemaVer, IPDR_VERSION_3_1)) == 0) {
    if ((length % 4) != 0)
	length = length + (4 - (length % 4));
}

    fread((wchar *)pRawValue, sizeof(char), length, pStream);

 return (pRawValue);
}

/*
 *  NAME:
 *      read_octet_array() - Unmarshalling of Byte Array
 *                  
 *  DESCRIPTION:      Unmarshalling of Byte Array
 *      
 *	ARGUMENTS:
 *      - length {IN} 
 *	    - pStream {IN}
 *
 *  RETURNS 	
 *	      Returns the Byte Array which has been read
*/


byte* read_octet_array(int length, void* pStream, char* schemaVer)
{
    byte* pRawValue;

/* code change to read the structure member for the schema version */
if((strcmp(schemaVer,IPDR_VERSION_3_1)) == 0) {
	if ((length % 4) != 0)
		length = length + (4 - (length % 4));
}

  	pRawValue = (byte *) calloc (sizeof(byte), (length+1));

	fread(pRawValue, 1, length, pStream);

	pRawValue[length] = '\0';
    return (pRawValue);
}

/*
 *  NAME:
 *      read_longlong() - Unmarshalling of hyper
 *                  
 *  DESCRIPTION:     Unmarshalling of hyper 
 *      
 *	ARGUMENTS:
 *	    - pStream {IN}
 *
 *  RETURNS 	
 *	      Returns the hyper value which has been read
*/


hyper* read_longlong(void* pStream)
{
    char tempByte[3] = "ff";
    hyper byteToPrint;
	hyper* pRawValue;
	int count = 0;
    unsigned char ch[5];
	
	pRawValue = (hyper *) calloc(sizeof(hyper), 100);

	fread(ch, sizeof(char), 8, pStream);

    for(count = 0;count < HYPER_MAX_BYTES; count++) {
         sprintf(tempByte, "%02x", ch[count]);
         byteToPrint = hyperReadHexToInt(count, tempByte);
         (*pRawValue) = (*pRawValue) + byteToPrint;
	}
    return(pRawValue);
}

/*!
 *  NAME:
 *      hyperReadHexToInt() - Function for converting hex to int
 *                               for reading a hyper value
 *                  
 *  DESCRIPTION: Function for converting hex to int for reading a hyper
 *               value
 *                       
 *	ARGUMENTS:
 *      - count {IN}
 *	    - iHexa{IN}
 *
 *   RETURNS 	
 *	     returns the hyper
 */

hyper hyperReadHexToInt(int count, char iHexa[3])
{
        hyper MSB = 0;
        hyper LSB = 0;
        char chMSB[2];
        char chLSB[2];

        chMSB[0] = iHexa[0];
        chLSB[0] = iHexa[1];
        chMSB[1] = '\0';
        chLSB[1] = '\0';

        convReadHyperCharHexToInt(chMSB, &MSB);
        convReadHyperCharHexToInt(chLSB, &LSB);
		
		/* constant numbers in the below section are powers of 256 */
		switch(count){
		case 0: return ( MSB * 268435456 * 256 * 256 * 256 * 256 +
					     LSB * 6777216 * 256 * 256 * 256 * 256);
				break;
		case 1: return ( MSB * 268435456 * 256 * 256 * 256 +
					     LSB * 16777216 * 256 * 256 * 256);
				break;
		case 2: return ( MSB * 268435456 * 256 * 256 + 
					     LSB * 16777216 * 256 * 256);
				break;
		case 3: return ( MSB * 268435456 * 256 + 
					     LSB * 16777216 * 256);
				break;
		case 4: return ( MSB * 268435456 + 
					     LSB * 16777216 );
			    break;
		case 5: return ( MSB * 1048576 + 
					     LSB * 65536);
			    break;
		case 6: return ( MSB * 4096 + 
					     LSB * 256);
			    break;
		case 7: return ( MSB * 16 + 
					     LSB );
				break;
		default: return (0);
		}
}

/*!
 *  NAME:
 *      convReadHyperCharHexToInt() - Function for converting hex (in char 
 *                                    array)to int for reading a hyper 
 *                                    value
 *                  
 *  DESCRIPTION: Function for converting hex (in char array)to int for 
 *               reading a hyper value
 *                      
 *	ARGUMENTS:
 *      - hex {IN}
 *	    - integer{IN/OUT}
 *
 *   RETURNS 	
 *	    void
 */

void convReadHyperCharHexToInt(char *hex, hyper *SB)
{
     if (hex[0] <= '9' && hex[0] > '0')
                *SB = (hyper) atoi(hex);
        if (hex[0] == 'a') *SB = 10 ;
        if (hex[0] == 'b') *SB = 11 ;
        if (hex[0] == 'c') *SB = 12 ;
        if (hex[0] == 'd') *SB = 13 ;
        if (hex[0] == 'e') *SB = 14 ;
        if (hex[0] == 'f') *SB = 15 ;
}


/*
 *  NAME:
 *      read_u_longlong() - Unmarshalling of Unsigned Hyper
 *                  
 *  DESCRIPTION:     Unmarshalling of Unsigned Hyper 
 *      
 *	ARGUMENTS:
 *	    - pStream {IN}
 *
 *  RETURNS 	
 *	      Returns the value which has been read
*/


u_hyper* read_u_longlong(void* pStream)
{
    char tempByte[3] = "ff";
    u_hyper byteToPrint;
	u_hyper* pRawValue;
	int count = 0;
    unsigned char ch[5];
	
	pRawValue = (u_hyper *) calloc(sizeof(u_hyper), 100);

	fread(ch, sizeof(char), 8, pStream);

    for(count = 0;count < HYPER_MAX_BYTES; count++) {
         sprintf(tempByte, "%02x", ch[count]);
         byteToPrint = unhyperReadHexToInt(count, tempByte);
         (*pRawValue) = (*pRawValue) + byteToPrint;
	}
    return(pRawValue);
}

/*!
 *  NAME:
 *      unhyperReadHexToInt() - Function for converting hex to int
 *                               for Reading an unsigned hyper value
 *                  
 *  DESCRIPTION: Function for converting hex to int for reading an           
 *               unsigned hyper value
 *      
 *	ARGUMENTS:
 *      - count {IN}
 *	    - iHexa{IN}
 *
 *   RETURNS 	
 *	     returns the unsigned hyper
 */

u_hyper unhyperReadHexToInt(int count, char iHexa[3])
{
        u_hyper MSB = 0;
        u_hyper LSB = 0;
        char chMSB[2];
        char chLSB[2];

        chMSB[0] = iHexa[0];
        chLSB[0] = iHexa[1];
        chMSB[1] = '\0';
        chLSB[1] = '\0';

        convReadUnHyperCharHexToInt(chMSB, &MSB);
        convReadUnHyperCharHexToInt(chLSB, &LSB);
		
		/* constant numbers in the below section are powers of 256 */
		switch(count){
		case 0: return ( MSB * 268435456 * 256 * 256 * 256 * 256 +
					     LSB * 6777216 * 256 * 256 * 256 * 256);
				break;
		case 1: return ( MSB * 268435456 * 256 * 256 * 256 +
					     LSB * 16777216 * 256 * 256 * 256);
				break;
		case 2: return ( MSB * 268435456 * 256 * 256 + 
					     LSB * 16777216 * 256 * 256);
				break;
		case 3: return ( MSB * 268435456 * 256 + 
					     LSB * 16777216 * 256);
				break;
		case 4: return ( MSB * 268435456 + 
					     LSB * 16777216 );
			    break;
		case 5: return ( MSB * 1048576 + 
					     LSB * 65536);
			    break;
		case 6: return ( MSB * 4096 + 
					     LSB * 256);
			    break;
		case 7: return ( MSB * 16 + 
					     LSB );
				break;
		default: return (0);
		}
}


/*!
 *  NAME:
 *      convReadUnHyperCharHexToInt() - Function for converting hex 
 *                                 (in char array)to int for reading an 
 *                                 unsigned hyper value
 *                  
 *  DESCRIPTION: Function for converting hex (in char array)to int for 
 *               reading an unsigned hyper value
 *                  
 *      
 *	ARGUMENTS:
 *      - hex {IN}
 *	    - integer{IN/OUT}
 *
 *   RETURNS 	
 *	    void
 */


void convReadUnHyperCharHexToInt(char *hex, u_hyper *SB)
{
     if (hex[0] <= '9' && hex[0] > '0')
                *SB = (u_hyper) atoi(hex);
        if (hex[0] == 'a') *SB = 10 ;
        if (hex[0] == 'b') *SB = 11 ;
        if (hex[0] == 'c') *SB = 12 ;
        if (hex[0] == 'd') *SB = 13 ;
        if (hex[0] == 'e') *SB = 14 ;
        if (hex[0] == 'f') *SB = 15 ;
}

/*!
 *  NAME:
 *      read_short() -  Unmarshalling of short value
 *
 *  DESCRIPTION:
 *         Unmarshalling of short value
 *  
 *  ARGUMENTS:
 *	    - pStream {IN}
 *
 *  RETURNS 	
 *	     Returns the value which has been read from input stream
 */
short int read_short(void* pStream)
{
        char tempByte[3] = "ff";
        int byteToPrint;
        int count = 0;
        unsigned char ch[5] = "";
        short int RawValue = 0;

        fread(ch, sizeof(char), SHORT_INT_BYTES, pStream);
        for ( count = 0; count < SHORT_INT_BYTES; count++ )
        {
             sprintf(tempByte, "%02x", ch[count]);
             byteToPrint = convReadShortIntHexToShortInt(count, tempByte);
             RawValue = RawValue + byteToPrint;
        }

        return(RawValue);
}

/*!
 *  NAME:
 *      read_u_short() -  Unmarshalling of unsigned short value
 *
 *  DESCRIPTION:
 *         Unmarshalling of unsigned short value
 *  
 *  ARGUMENTS:
 *	    - pStream {IN}
 *
 *  RETURNS 	
 *	     Returns the value which has been read from input stream
 */
unsigned short int read_u_short(void* pStream)
{
        char tempByte[3] = "ff";
        unsigned int byteToPrint;
        int count = 0;
        unsigned char ch[5];
		unsigned short int RawValue = 0;  

        fread(ch, sizeof(char), SHORT_INT_BYTES, pStream);
       for (count = 0; count < U_INT_MAXCOUNT_2; count++)
       {
            sprintf(tempByte, "%02x", ch[count]);
            byteToPrint = convReadUShortIntHexToShortInt(count, tempByte);
            RawValue = RawValue + byteToPrint;
       }
       return(RawValue);
}

/*!
 *  NAME:
 *      read_byte() -  Unmarshalling of byte value
 *
 *  DESCRIPTION:
 *         Unmarshalling of byte value
 *  
 *  ARGUMENTS:
 *	    - pStream {IN}
 *
 *  RETURNS 	
 *	     Returns the value which has been read from input stream
 */
char read_byte(void* pStream)
{
        char tempByte[3] = "ff";
        char byteToPrint;
        int count = 0;
        unsigned char ch[5] = "";

        fread(ch, sizeof(char), BYTE_BYTES, pStream);
		  byteToPrint = ch;
        for ( count = 0; count < BYTE_BYTES; count++ )
        {
             sprintf(tempByte, "%02x", ch[count]);
             byteToPrint = convReadIntHexToByte(count, tempByte);
        }

        return(byteToPrint);
}

/*!
 *  NAME:
 *      read_boolean() -  Unmarshalling of boolean value
 *
 *  DESCRIPTION:
 *         Unmarshalling of boolean value
 *  
 *  ARGUMENTS:
 *	    - pStream {IN}
 *
 *  RETURNS 	
 *	     Returns the value which has been read from input stream
 */
int read_boolean(void* pStream)
{
        char tempByte[3] = "ff";
        int byteToPrint;
        int count = 0;
        unsigned char ch[5] = "";
        int RawValue = 0;

        fread(ch, sizeof(char), BYTE_BYTES, pStream);
        for ( count = 0; count < BYTE_BYTES; count++ )
        {
             sprintf(tempByte, "%02x", ch[count]);
             byteToPrint = convReadIntHexToByte(count, tempByte);
             RawValue = RawValue + byteToPrint;
        }

        return(RawValue);
}

/*!
 *  NAME:
 *      convReadIntHexToShortInt() -  Converts Hex to ShortInt for integer
 *
 *  DESCRIPTION:
 *         Converts Hex to ShortInt for integer for unmarshalling of integer
 *         value
 *  
 *  ARGUMENTS:
 *	    - count {IN}
 *      - iHexa {IN/OUT}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int convReadShortIntHexToShortInt(int count, char iHexa[3])
{
	int MSB = 0;
	int LSB = 0;
	char chMSB[2];
	char chLSB[2];
	chMSB[0] = iHexa[0];
	chMSB[1] = '\0';
	chLSB[0] = iHexa[1];
	chLSB[1] = '\0';

	convReadIntCharHexToInt(chMSB, &MSB);
	convReadIntCharHexToInt(chLSB, &LSB);

	if(count == 0) return ( MSB * 4096      + LSB * 256);
	if(count == 1) return ( MSB * 16        + LSB );
	
	return(0);
}

/*!
 *  NAME:
 *      convReadIntHexToShortInt() -  Converts Unsigned Short to Hex 
 *
 *  DESCRIPTION:
 *         Converts unsinged ShortInt to HEX for integer for unmarshalling of integer
 *         value
 *  
 *  ARGUMENTS:
 *	    - count {IN}
 *      - iHexa {IN/OUT}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
unsigned int convReadUShortIntHexToShortInt(int count, char iHexa[3])
{
        int MSB = 0;
        int LSB = 0;
        char chMSB[2];
        char chLSB[2];

        chMSB[0] = iHexa[0];
        chLSB[0] = iHexa[1];
        chMSB[1] = '\0';
        chLSB[1] = '\0';

        convReadUIntCharHexToInt(chMSB, &MSB);
        convReadUIntCharHexToInt(chLSB, &LSB);

        if(count == 0) return ( MSB * 4096      + LSB * 256);
        if(count == 1) return ( MSB * 16        + LSB );

		return (0);
}

/*!
 *  NAME:
 *      convReadIntHexToShortInt() -  Converts Hex to Byte for integer
 *
 *  DESCRIPTION:
 *         Converts Hex to Byte for integer for unmarshalling of integer
 *         value
 *  
 *  ARGUMENTS:
 *	    - count {IN}
 *      - iHexa {IN/OUT}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */
int convReadIntHexToByte(int count, char iHexa[3])
{
	int MSB = 0;
	int LSB = 0;
	char chMSB[2];
	char chLSB[2];
	chMSB[0] = iHexa[0];
	chMSB[1] = '\0';
	chLSB[0] = iHexa[1];
	chLSB[1] = '\0';

	convReadIntCharHexToInt(chMSB, &MSB);
	convReadIntCharHexToInt(chLSB, &LSB);

	if(count == 0) return ( MSB * 16        + LSB );
	
	return(0);
}
