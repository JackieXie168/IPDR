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

#include "xdr/XDRMarshaller.h"
#include "utils/errorCode.h"
#include "utils/IPDRCommon.h"


/*Marsheller Helper Header Library. This will be created as a separate 
header file & the functions will be available to the above C Program. 
Header File. Name: "XDRMarshaller.h"
*/


/*!
 *  NAME:
 *      write_long() -  Marshalling of Integer Value
 *
 *  DESCRIPTION:
 *         Marshalling of Integer Value 
 *  
 *  ARGUMENTS:
 *      - RawValue (IN)
 *	    - pStream {IN/OUT}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int write_long(int RawValue,void *pStream)
{
        int byteCount = 0;
        int count;
        char tempByte[3] = "ff";
        int byteToPrint;
        unsigned char bytes[INT_BITS];

        if(pStream == NULL)
                return(IPDR_NULL_INPUT_PARAMETER);

        sprintf((char *)bytes, "%08x", RawValue);
        for ( count = 0; count < INT_BITS; count = count + 2 )
        {
                tempByte[0] = bytes[count];
                tempByte[1] = bytes[count+1];
                byteToPrint = convWriteIntHexToInt(tempByte);
                fprintf(pStream, "%c", byteToPrint);
        }

        return(IPDR_OK);
}

/*!
 *  NAME:
 *      convWriteIntHexToInt() - Function for converting Hex to Int
 *                  
 *  DESCRIPTION:    Function for converting Hex to Int 
 *      
 *	ARGUMENTS:
 *      - iHexa (IN)
 *
 *  RETURNS 	
 *	     Returns integer equivalent of input Hexa decimal number.
 */

int convWriteIntHexToInt(char iHexa[3])
{
        int MSB = 0;
        int LSB = 0;
        char chMSB[2];
        char chLSB[2];

        chMSB[0] = iHexa[0];
        chLSB[0] = iHexa[1];
        chMSB[1] = '\0';
        chLSB[1] = '\0';

        if (chMSB[0] <= '9' && chMSB[0] >= '0') MSB = atoi(chMSB) * 16;

        if (chMSB[0] == 'a') MSB = 10 * 16;
        if (chMSB[0] == 'b') MSB = 11 * 16;
        if (chMSB[0] == 'c') MSB = 12 * 16;
        if (chMSB[0] == 'd') MSB = 13 * 16;
        if (chMSB[0] == 'e') MSB = 14 * 16;
        if (chMSB[0] == 'f') MSB = 15 * 16;

        if (chLSB[0] <= '9' && chLSB[0] >= '0') LSB = atoi(chLSB) ;

        if (chLSB[0] == 'a') LSB = 10 ;
        if (chLSB[0] == 'b') LSB = 11 ;
        if (chLSB[0] == 'c') LSB = 12 ;
        if (chLSB[0] == 'd') LSB = 13 ;
        if (chLSB[0] == 'e') LSB = 14 ;
        if (chLSB[0] == 'f') LSB = 15 ;

        return(MSB + LSB);
}

/*!
 *  NAME:
 *      write_double() - Marshalling of double Value
 *                  
 *  DESCRIPTION:     Marshalling of double Value 
 *      
 *	ARGUMENTS:
 *      - rawValue (IN)
 *	    - streamPointer {IN/OUT}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int write_double(double rawValue, void *streamPointer)
{
	char *cHex = NULL;
	int count = 0;
        char tempByte[3] = "ff";
        int byteToPrint;

	cHex = (char *)calloc(sizeof(char), DOUBLE_HEXA_BITS);

	if(streamPointer == NULL)
		return(IPDR_NULL_INPUT_PARAMETER);

	getWriteDoubleHexFromDouble(cHex, rawValue);
    
	/* Writing the values into file */
       	for (count = 0 ; count < DOUBLE_HEXA_BITS ; count = count + 2)
	{
		tempByte[0] = cHex[count];
		tempByte[1] = cHex[count + 1];
		byteToPrint = convWriteIntHexToInt(tempByte);
		fprintf(streamPointer, "%c", byteToPrint);
	}
	free(cHex);
    return(IPDR_OK);
}

/*!
 *  NAME:
 *      getWriteDoubleHexFromDouble() - Function for converting hex to 
 *                                      double.
 *                  
 *  DESCRIPTION:     Function for converting hex to double.
 *      
 *	ARGUMENTS:
 *      - hexaDouble (IN)
 *	    - double value {IN/OUT}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int getWriteDoubleHexFromDouble(char hexaDouble[DOUBLE_HEXA_BITS], double n)
{
	int count = 0;
	double nPositive = 0;
	double mantissa;
	int exponent = 0;
	char c = 0;
	char tempChar[4];
	
	char *binaryA;
	char *binaryB;
    char *finalBinary;

	binaryA = (char *)calloc(sizeof(char), DOUBLE_MANTISSA_BITS);
	for (count = 0; count < DOUBLE_MANTISSA_BITS; count++) binaryA[count] = '0';
	binaryB = (char *)calloc(sizeof(char), DOUBLE_EXPONENT_BITS);
	for (count = 0; count < DOUBLE_EXPONENT_BITS; count++) binaryB[count] = '0';
	finalBinary = (char *)calloc(sizeof(char), DOUBLE_EXPONENT_BITS + DOUBLE_MANTISSA_BITS);
	for (count = 0; count < DOUBLE_EXPONENT_BITS + DOUBLE_MANTISSA_BITS; count++) finalBinary[count] = '0';


	if (n == 0) {
		exponent = -DOUBLE_CONSTANT;
	} else {
		if (n < 0) {
			nPositive = -n;
			exponent = getWriteDoubleExponent(nPositive);
		} else {
			nPositive = n;
			exponent = getWriteDoubleExponent(nPositive);
		}
	}
	
	mantissa = nPositive/(pow(2, exponent));

	getWriteDoubleBinaryBeforeDecimal(exponent + DOUBLE_CONSTANT, binaryB);
	if (n < 0) binaryB[0] = '1';

	getWriteDoubleBinaryAfterDecimal(mantissa - 1, binaryA);

        for (count = 0; count < DOUBLE_EXPONENT_BITS; count++) {
            finalBinary[count] = binaryB[count];
        }

        for (count = 0; count < DOUBLE_MANTISSA_BITS; count++) {
            finalBinary[DOUBLE_EXPONENT_BITS + count] = binaryA[count];
        }

	for (count = 0; count < DOUBLE_HEXA_BITS; count++) {
		tempChar[0] = finalBinary[0 + (4 * count)];
		tempChar[1] = finalBinary[1 + (4 * count)];
		tempChar[2] = finalBinary[2 + (4 * count)];
		tempChar[3] = finalBinary[3 + (4 * count)];
		hexaDouble[count] = getWriteDoubleHexFromBinary(tempChar);
	}


	free(binaryA);
	free(binaryB);
        free(finalBinary);

	return (IPDR_OK);
}

/*!
 *  NAME:
 *      getWriteDoubleExponent() - Function for returning exponent value of 
 *                                 input double value.
 *                  
 *  DESCRIPTION: Function for returning exponent value of input double 
 *               value.
 *      
 *	ARGUMENTS:
 *	    - double value {IN}
 *
 *  RETURNS 	
 *	     Returns Exponent (integer value) of input Double value.
 */

int getWriteDoubleExponent(double d)
{
	int count = 0;
	double temp = 0;

	if ((d > 0) && (d < 1)) {
		for (count = 0; count < DOUBLE_CONSTANT; count++) {
			temp = pow(2, count);
			if (d * temp >= 1) return (-count);
		}
	} else {
		for (count = 0; count < DOUBLE_CONSTANT; count++) {
			temp = pow(2, count);
			if (d/temp < 2) return (count);
		}
	}
	return (count);
}

/*!
 *  NAME:
 *      getWriteDoubleBinaryAfterDecimal() - Function for generating Binary 
 *                                         equivalent of mantissa of input 
 *                                         double value.
 *                  
 *  DESCRIPTION:     Function for generating Binary equivalent
 *                   of mantissa of input double value.
 *      
 *	ARGUMENTS:
 *	    - double value {IN}
 *		- char value {IN/OUT}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int getWriteDoubleBinaryAfterDecimal(double n, char *b)
{
	int count = 0;
	while (( n != 1) && (count < DOUBLE_MANTISSA_BITS)) {
		n = n * 2;
		if (n > 1) {
			n -= 1;
			b[count++] = '1';
		}
		else if (n == 1) b[count++] = '1';
		else b[count++] = '0';
	}
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      getWriteDoubleBinaryBeforeDecimal() - Function for generating Binary 
 *                                        equivalent of exponent of input 
 *                                       double value.
 *                  
 *  DESCRIPTION:     Function for generating Binary equivalent
 *                   of exponent of input double value.
 *      
 *	ARGUMENTS:
 *	    - double value {IN}
 *		- char value {IN/OUT}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int getWriteDoubleBinaryBeforeDecimal(double n, char *b)
{
	int count = DOUBLE_EXPONENT_BITS - 1;
	int itemp = 0;
	double dtemp = 0;

	if (n == 0) return 0;
	if (n == 1) {
		b[count--] = '1';
		return 0;
	}
	while ((n >= 1) && (count >= 0)) {
		itemp = (int)n % 2;
		n = (int)n / 2 ;
	
		if (itemp == 1) b[count--] = '1';
		if (itemp == 0) b[count--] = '0';
	}
	return (IPDR_OK);
}

/*!
 *  NAME:
 *      getWriteDoubleHexFromBinary() - Function for generating Hex 
 *                                     equivalent from Binary value.
 *                  
 *  DESCRIPTION:    Function for generating Hex equivalent from Binary 
 *                  value.
 *
 *	ARGUMENTS:
 *		- char array(binary) {IN}
 *
 *  RETURNS 	
 *	   returns the hexadecimal character value
 */

char getWriteDoubleHexFromBinary(char binary[4])
{
	char hex = 0;
	int count = 0;

	int result = 0;
	
	if (binary[3] == '1') result = 1;
	if (binary[2] == '1') result = result + 2;
	if (binary[1] == '1') result = result + 4;
	if (binary[0] == '1') result = result + 8;

	if (result < 10) hex = 48 + result;
	else if ((result >= 10) && (result < 16)) hex = 87 + result;
	
	return hex;

}

/*!
 *  NAME:
 *      write_float() - Marshalling of float Value
 *                  
 *  DESCRIPTION:     Marshalling of float Value 
 *      
 *	ARGUMENTS:
 *      - float value (IN/OUT)
 *	    - File pointer {IN/OUT}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int write_float(float rawValue, void *streamPointer)
{
	char *cHex = NULL;
	int count = 0;
    char tempByte[2] = "ff";
    int byteToPrint;
    if(streamPointer == NULL)
          return(IPDR_NULL_INPUT_PARAMETER);
	cHex = (char *)calloc(sizeof(char), FLOAT_HEXA_BITS);
	getFloatWriteHexFromDouble(cHex, rawValue);
	
    /* Writing the values into file */
    for (count = 0 ; count < FLOAT_HEXA_BITS ; count = count + 2) {
		tempByte[0] = cHex[count];
		tempByte[1] = cHex[count + 1];
		byteToPrint = hexToInt(tempByte);
		fprintf(streamPointer, "%c", byteToPrint);
	}

	free(cHex);
    return(IPDR_OK);
}

/*!
 *  NAME:
 *      hexToInt() - function for converting Hex to Int
 *                  
 *  DESCRIPTION:     function for converting Hex to Int 
 *      
 *	ARGUMENTS:
 *      - iHexa (IN)
 *
 *  RETURNS 	
 *	     Returns integer equivalent of input Hexa decimal number.
 */

int hexToInt (char iHexa[3])
{
        int MSB = 0;
        int LSB = 0;
        char chMSB[2];
        char chLSB[2];
        chMSB[0] = iHexa[0];
        chLSB[0] = iHexa[1];
        chMSB[1] = '\0';
        chLSB[1] = '\0';
        
		if (chMSB[0] <= '9' && chMSB[0] >= '0')
			MSB = atoi(chMSB) * 16;

        if (chMSB[0] == 'a') MSB = 10 * 16;

        if (chMSB[0] == 'b') MSB = 11 * 16;

        if (chMSB[0] == 'c') MSB = 12 * 16;

        if (chMSB[0] == 'd') MSB = 13 * 16;

        if (chMSB[0] == 'e') MSB = 14 * 16;

        if (chMSB[0] == 'f') MSB = 15 * 16;


        if (chLSB[0] <= '9' && chLSB[0] >= '0')
                LSB = atoi(chLSB) ;

        if (chLSB[0] == 'a') LSB = 10 ;

        if (chLSB[0] == 'b') LSB = 11 ;

        if (chLSB[0] == 'c') LSB = 12 ;

        if (chLSB[0] == 'd') LSB = 13 ;

        if (chLSB[0] == 'e') LSB = 14 ;

        if (chLSB[0] == 'f') LSB = 15 ;

        return(MSB + LSB);

}

/*!
 *  NAME:
 *      getFloatWriteHexFromDouble() - Function for converting hex to double.
 *                  
 *  DESCRIPTION:     Function for converting hex to double.
 *      
 *	ARGUMENTS:
 *      - hexaDouble (IN/OUT)
 *	    - double value {IN/OUT}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int getFloatWriteHexFromDouble(char hexaDouble[FLOAT_HEXA_BITS], double n)
{

	int count = 0;
	double nPositive = 0;
	double mantissa;
	int exponent = 0;
	char c = 0;
	char tempChar[4];
	char *binaryA;
	char *binaryB;
    char *finalBinary;
	
	binaryA = (char *)calloc(sizeof(char), FLOAT_MANTISSA_BITS);

	for (count = 0; count < FLOAT_MANTISSA_BITS; count++) binaryA[count] = '0';
		binaryB = (char *)calloc(sizeof(char), FLOAT_EXPONENT_BITS);

	for (count = 0; count < FLOAT_EXPONENT_BITS; count++) binaryB[count] = '0';
		finalBinary = (char *)calloc(sizeof(char), FLOAT_EXPONENT_BITS + FLOAT_MANTISSA_BITS);

	for (count = 0; count < FLOAT_EXPONENT_BITS + FLOAT_MANTISSA_BITS; count++) finalBinary[count] = '0';

	if (n == 0) {
		exponent = -FLOAT_CONSTANT;
	} else {
		if (n < 0) {
			nPositive = -n;
			exponent = getFloatWriteExponent(nPositive);
		} else {
			nPositive = n;
			exponent = getFloatWriteExponent(nPositive);
		}
	}

	mantissa = nPositive/(pow(2, exponent));

	getFloatWriteBinaryBeforeDecimal(exponent + FLOAT_CONSTANT, binaryB);

	if (n < 0) binaryB[0] = '1';
		getFloatWriteBinaryAfterDecimal(mantissa - 1, binaryA);

	for (count = 0; count < FLOAT_EXPONENT_BITS; count++) {
		finalBinary[count] = binaryB[count];
    }

    for (count = 0; count < FLOAT_MANTISSA_BITS; count++) {
        finalBinary[FLOAT_EXPONENT_BITS + count] = binaryA[count];
    }

	for (count = 0; count < FLOAT_HEXA_BITS; count++) {
		tempChar[0] = finalBinary[0 + (4 * count)];
		tempChar[1] = finalBinary[1 + (4 * count)];
		tempChar[2] = finalBinary[2 + (4 * count)];
		tempChar[3] = finalBinary[3 + (4 * count)];
		hexaDouble[count] = getFloatWriteHexFromBinary(tempChar);
	}

	free(binaryA);
	free(binaryB);
	free(finalBinary);
	return(IPDR_OK);

}

/*!
 *  NAME:
 *      getFloatWriteExponent() - Function for returning exponent value of 
 *                             input double value.
 *                  
 *  DESCRIPTION: Function for returning exponent value of input double 
 *               value.
 *      
 *	ARGUMENTS:
 *	    - double value {IN/OUT}
 *
 *  RETURNS 	
 *	     Returns Exponent (integer value) of input Double value.
 */

int getFloatWriteExponent(double d)

{

	int count = 0;

	double temp = 0;



	if ((d > 0) && (d < 1)) {

		for (count = 0; count < FLOAT_CONSTANT; count++) {

			temp = pow(2, count);

			if (d * temp >= 1) return -count;

		};

	} else {

		for (count = 0; count < FLOAT_CONSTANT; count++) {

			temp = pow(2, count);

			if (d/temp < 2) return count;

		};

	}

	return count;

}



/*!
 *  NAME:
 *      getFloatWriteBinaryAfterDecimal() - This function calculates the 
 *                                   binary for the portion after the 
 *                                   decimnal point
 *                  
 *  DESCRIPTION: This function calculates the binary for the portion 
 *               after the decimnal point
 *      
 *	ARGUMENTS:
 *	    - double value {IN}
 *      - char array(binary value) {IN/OUT}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int getFloatWriteBinaryAfterDecimal(double n, char *b)

{

	int count = 0;

	while (( n != 1) && (count < FLOAT_MANTISSA_BITS)) {

		n = n * 2;

		if (n > 1) {

			n -= 1;

			b[count++] = '1';

		}

		else if (n == 1) b[count++] = '1';

		else b[count++] = '0';

	}

	return 0;

}



/*!
 *  NAME:
 *      getFloatWriteBinaryBeforeDecimal() -  This function calculates the 
 *                                 binary for the portion before the 
 *                                 decimnal point
 *
 *                  
 *  DESCRIPTION: This function calculates the binary for the portion 
 *               before the decimnal point
 *      
 *	ARGUMENTS:
 *	    - double value {IN}
 *      - char array (binary value) {IN/OUT}
 *
 *  RETURNS 	
 *	      Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */

int getFloatWriteBinaryBeforeDecimal(double n, char *b)

{

	int count = FLOAT_EXPONENT_BITS - 1;

	int itemp = 0;

	double dtemp = 0;

	

	if (n == 0) return 0;

	if (n == 1) {

		b[count--] = '1';

		return 0;

	}

	while ((n >= 1) && (count >= 0)) {

		itemp = (int)n % 2;

		n = (int)n / 2 ;

	

		if (itemp == 1) b[count--] = '1';

		if (itemp == 0) b[count--] = '0';

	}

	return 0;

}

/*!
 *  NAME:
 *      getFloatWriteHexFromBinary() -  This function calculates the 
 *                                hexadecimal value for the binary 
 *                                equivalent of the float value
 *
 *  DESCRIPTION: hexadecimal value for the binary equivalent of the float 
 *               value
 *      
 *	ARGUMENTS:
 *	    - binary {IN}
 *
 *  RETURNS 	
 *	       Returns the hexadecimal character value
 */



char getFloatWriteHexFromBinary(char binary[4])

{

	char hex = 0;

	int count = 0;



	int result = 0;

	

	if (binary[3] == '1') result = 1;

	if (binary[2] == '1') result = result + 2;

	if (binary[1] == '1') result = result + 4;

	if (binary[0] == '1') result = result + 8;



	if (result < 10) hex = 48 + result;

	else if ((result >= 10) && (result < 16)) hex = 87 + result;

	return hex;
}



/*!
 *  NAME:
 *      write_u_long() - Marshalling of unsigned integer
 *                  
 *  DESCRIPTION:     Marshalling of unsigned integer 
 *      
 *	ARGUMENTS:
 *      - pRawValue (IN/OUT)
 *	    - pStream {IN/OUT}
 *
 *  RETURNS 	
 *	      Returns Zero if the operation is successful or appropriate error 
 *        code in case of failure
*/

int write_u_long(
				unsigned int RawValue, 
			    void* pStream
			  )

{
        
        int byteCount = 0;
        char tempByte[2] = "ff";
        int byteToPrint=0;
		unsigned char bytes[INT_BITS];
        int count=0;
        int counter=0;


		if(pStream==NULL)
			return(18);


         sprintf((char *)bytes, "%08x", RawValue);
         for(count=0;count<U_INT_MAXCOUNT_4; count++)
         {
             counter=2*count;
             tempByte[0] = bytes[counter];
             tempByte[1] = bytes[counter+1];
             byteToPrint = convWriteUInthexToInt(tempByte);
             fprintf(pStream, "%c", byteToPrint);
            
		 }

    return(IPDR_OK);
}

/*!
 *  NAME:
 *      convWriteUInthexToInt() - Converts hex to integer value
 *                  
 *  DESCRIPTION:    Converts hex to integer value for 
 *                                Unsigned Int
 *
 *	ARGUMENTS:
 *      - iHexa (IN)
 *
 *  RETURNS 	
 *	      Returns the equivalent hex value
*/


int convWriteUInthexToInt(char iHexa[3])
{
        int MSB = 0;
        int LSB = 0;
        char chMSB[2];
        char chLSB[2];
   
        chMSB[0] = iHexa[0];
        chLSB[0] = iHexa[1];
        chMSB[1] = '\0';
        chLSB[1] = '\0';
        if (chMSB[0] <= '9' && chMSB[0] >= '0')
                MSB = atoi(chMSB) * 16;

        if (chMSB[0] == 'a') MSB = 10 * 16;
        if (chMSB[0] == 'b') MSB = 11 * 16;
        if (chMSB[0] == 'c') MSB = 12 * 16;
        if (chMSB[0] == 'd') MSB = 13 * 16;
        if (chMSB[0] == 'e') MSB = 14 * 16;
        if (chMSB[0] == 'f') MSB = 15 * 16;

        if (chLSB[0] <= '9' && chLSB[0] >= '0')
                LSB = atoi(chLSB) ;

        if (chLSB[0] == 'a') LSB = 10 ;
        if (chLSB[0] == 'b') LSB = 11 ;
        if (chLSB[0] == 'c') LSB = 12 ;
        if (chLSB[0] == 'd') LSB = 13 ;
        if (chLSB[0] == 'e') LSB = 14 ;
        if (chLSB[0] == 'f') LSB = 15 ;

        return (MSB + LSB);
}


/*!
 *  NAME:
 *      write_longlong() - Marshalling of Hyper Value
 *                  
 *  DESCRIPTION:    Marshalling of Hyper Value
 *
 *	ARGUMENTS:
 *      - RawValue (IN)
 *      - pStream (IN/OUT)
 *
 *  RETURNS 	
 *  Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
*/


int write_longlong(hyper RawValue,FILE *pStream)
{
        int byteCount = 0;
        int count;
        char tempByte[3] = "ff";
        int byteToPrint;
        int convHex;
        unsigned char bytes[HYPER_BITS];
        char tempBytes[HYPER_BITS];

        if(pStream==NULL)
                return(18);
        convHex = convWriteHyperToHex(RawValue, bytes);
        
		sprintf(tempBytes, "%016s", bytes);
		for(count = 0;count < HYPER_MAXCOUNT_16;count = count + 2)
        {
                tempByte[0] = (unsigned char)tempBytes[count];
                tempByte[1] = (unsigned char)tempBytes[count + 1];
                byteToPrint = convWriteHyperHexToInt(tempByte);
                fprintf(pStream, "%c", byteToPrint);
        }
        return(IPDR_OK);
}

/*!
 *  NAME:
 *      convWriteHyperHexToInt() - Convert hex to int value for hyper
 *
 *  DESCRIPTION:    Convert hex to int value for hyper
 *
 *	ARGUMENTS:
 *       - iHexa
 *
 *  RETURNS :
 *  Returns the integer value
*/


int convWriteHyperHexToInt(char iHexa[3])
{
        int MSB = 0;
        int LSB = 0;
        char chMSB[2];
        char chLSB[2];

        chMSB[0] = iHexa[0];
        chLSB[0] = iHexa[1];
        chMSB[1] = '\0';
        chLSB[1] = '\0';

        if (chMSB[0] <= '9' && chMSB[0] >= '0')
                MSB = atoi(chMSB) * 16;

        if (chMSB[0] == 'A') MSB = 10 * 16;
        if (chMSB[0] == 'B') MSB = 11 * 16;
        if (chMSB[0] == 'C') MSB = 12 * 16;
        if (chMSB[0] == 'D') MSB = 13 * 16;
        if (chMSB[0] == 'E') MSB = 14 * 16;
        if (chMSB[0] == 'F') MSB = 15 * 16;

        if (chLSB[0] <= '9' && chLSB[0] >= '0')
                LSB = atoi(chLSB) ;

        if (chLSB[0] == 'A') LSB = 10 ;
        if (chLSB[0] == 'B') LSB = 11 ;
        if (chLSB[0] == 'C') LSB = 12 ;
        if (chLSB[0] == 'D') LSB = 13 ;
        if (chLSB[0] == 'E') LSB = 14 ;
        if (chLSB[0] == 'F') LSB = 15 ;

        return(MSB + LSB);
}

/*!
 *  NAME:
 *      convWriteHyperToHex() - Convert hyper to hex value
 *
 *  DESCRIPTION:   Convert hyper to hex value
 *
 *	ARGUMENTS:
 *      - hyper (IN)
 *      - unsigned char array(hex value) (IN/OUT)
 *
 *  RETURNS :
 *  Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
*/

int convWriteHyperToHex(hyper a, unsigned char* b)
{
        hyper qut;
        hyper rem;
        int len = 0;
        int i;
        int count = 0;
        int count1 = 0;
        char* temp = NULL;
        temp = (char*)calloc (HYPER_MAX_100, sizeof(char));
        do {
                qut = a / 16;
                rem = a % 16;
                if (qut < 16 && rem < 16) {
                        if(qut < 10)
                                *(b + count) = (char)(qut + 48);
                        else
                                *(b + count) = (char)(qut + 55);
                        count++;
                        if(rem < 10)
                                *(b + count) = (char)(rem + 48);
                        else
                                *(b + count) = (char)(rem + 55);
                        count++;
                        a = qut;
                }
                else {
                        a = qut;
                        if(rem < 10) {
							*(temp + count1) = (char)(rem + 48);
                        }
                        else {
							*(temp + count1) = (char)(rem + 55);
                        }
                        count1++;
                *(temp + count1) = '\0';
                }
        } while (a > 15);
        len = strlen(temp);
        for(i = len-1; i >= 0; i--) {
			*(b + count) = *(temp + i);
            count++;
        }
        *(b + count) = '\0';
        free(temp);
		return (0);
}


/*!
 *  NAME:
 *      write_string() - Marshalling of string
 *                  
 *  DESCRIPTION:     Marshalling of string 
 *      
 *	ARGUMENTS:
 *      - pRawValue (IN)
 *	    - pStream {IN/OUT}
 *
 *  RETURNS 	
 *	      Returns Zero or appropriate error code
 */

int write_string(char* pRawValue, void* pStream) 
{
    int count = 0;
    for(count = 0; count < (int)strlen(pRawValue); count++){
       fprintf(pStream, "%c",pRawValue[count]);
     }
/*
	if((strcmp(gVersion,IPDR_VERSION_3_1)) == 0) {
    if((strlen(pRawValue)%4)!=0) {
       for(count=(strlen(pRawValue)%4);count<4;count++)
         fprintf(pStream, "%c",0);
     }
   } 
*/
	 return (0);
}

/*!
 *  NAME:
 *      write_utf8() - Marshalling of UTF8 string
 *                  
 *  DESCRIPTION:     Marshalling of UTF8 string 
 *      
 *	ARGUMENTS:
 *      - pRawValue (IN)
 *	    - pStream {IN/OUT}
 *
 *  RETURNS 	
 *	      Returns Zero or appropriate error code
*/

int write_wstring(wchar* pRawValue, void *pStream, char* schemaVer)
{
    int count = 0;
 int length = 0;
 wchar* UTF8String;
 
 UTF8String = (wchar *) calloc (sizeof(char), MAX_IPDR_STRING * 2);
 
 getUTF8String((char *) pRawValue, (char *) UTF8String);
 
 while(UTF8String[count] != '\0') {
  if ((int)UTF8String[count] <= 255)
   UTF8String[count] = 19456 + (int)UTF8String[count];
  count++;
 }
 
 length = count;
 for(count = 0; count < length; count++) {
       fprintf(pStream, "%c", UTF8String[count]);
 }
         
/* Code change to check the strcuture member for schema version */
if((strcmp(schemaVer, IPDR_VERSION_3_1)) == 0) {
	length = strlen((char *) pRawValue);

	if ((length % 4) != 0) {
    	for (count = (length % 4); count < 4; count++)
    	fprintf(pStream, "%c",0);
   }
}
	
  free (UTF8String);
  return (0);
}


int getUTF8String(char* srcString, char* UTF8String)
{
 int count = 0;
 int strLength = 0;
 
 for (count = 0; srcString[count] != '\0'; count++) {
	 if (!checkBigEndian()) {
  UTF8String[2 * count] = srcString[count];
  UTF8String[2 * count + 1] = 'L';
	 } else {
    UTF8String[2 * count ] = 'L';
	UTF8String[2 * count + 1] = srcString[count];
	 }
 }
 
 UTF8String[2 * count] = '\0';
 return 0;
}

int checkBigEndian()
{
	union {
		short s;
		char c[sizeof(short)];

	} un;

	un.s = 0x0102;
	if (sizeof(short) == 2) {
		if (un.c[0] == 1 && un.c[1] == 2)
			return 1;
		else if (un.c[0] == 2 && un.c[1] == 1)
			return 0;
		else
			return -1;
	}
	return -1;
}



/*!
 *  NAME:
 *      write_octet_array() - Marshalling of Byte Array
 *                  
 *  DESCRIPTION:     Marshalling of Byte Array 
 *      
 *	ARGUMENTS:
 *      - pRawValue (IN)
 *	    - pStream {IN/OUT}
 *
 *  RETURNS 	
 *	      Returns Zero or appropriate error code
*/

int write_octet_array(byte* pRawValue, void *pStream)
{
    int count = 0;

    for(count = 0; count < (int) strlen((char *)pRawValue); count++){
       fprintf(pStream, "%c", pRawValue[count] );
    }
/*
	if((strcmp(gVersion,IPDR_VERSION_3_1)) == 0) {
    	if( (strlen((char *)pRawValue) % 4) != 0 ) {
       	for( count = (strlen((char *)pRawValue) % 4); count < 4 ; count++)
         	fprintf(pStream, "%c", 0);
    	}
	}
*/
	return (0);
}


/*!
 *  NAME:
 *      write_noctet_array() - Marshalling of Byte Array
 *                  
 *  DESCRIPTION:     Marshalling of Byte Array 
 *      
 *	ARGUMENTS:
 *      - pRawValue (IN)
 *      - length (IN)
 *	    - pStream {IN/OUT}
 *
 *  RETURNS 	
 *	      Returns Zero or appropriate error code
*/

int write_noctet_array(byte* pRawValue, void *pStream, int length, char* schemaVer)
{
    int count = 0;

    for(count = 0; count < length; count++){
       fprintf(pStream, "%c", pRawValue[count] );
    }

/* Code change to read the structure member for the schema version */
if((strcmp(schemaVer, IPDR_VERSION_3_1)) == 0) {
    if( (length % 4) != 0 ) {
       for( count = (length % 4); count < 4 ; count++)
         fprintf(pStream, "%c", 0);
    }
}
	return (0);
}



/*!
 *  NAME:
 *      write_u_longlong() - Marshalling of Unsigned hyper
 *                  
 *  DESCRIPTION:      Marshalling of Unsigned hyper
 *      
 *	ARGUMENTS:
 *      - pRawValue (IN)
 *	    - pStream {IN/OUT}
 *
 *  RETURNS 	
 *	      Returns Zero or appropriate error code
*/

int write_u_longlong(u_hyper RawValue,FILE *pStream)
{
        int byteCount = 0;
        int count;
        char tempByte[3] = "ff";
        int byteToPrint;
        int convHex;
        unsigned char bytes[HYPER_BITS];
        char tempBytes[HYPER_BITS];

        if(pStream==NULL)
                return(18);
        convHex = convWriteUnHyperToHex(RawValue, bytes);
		sprintf(tempBytes, "%016s", bytes);
		for(count = 0;count < HYPER_MAXCOUNT_16;count = count + 2) {
                tempByte[0] = (unsigned char)tempBytes[count];
                tempByte[1] = (unsigned char)tempBytes[count + 1];
                byteToPrint = convWriteHyperHexToInt(tempByte);
                fprintf(pStream, "%c", byteToPrint);
        }
        return(IPDR_OK);
}


/*!
 *  NAME:
 *      convWriteUnHyperToHex() - Convert unsigned hyper to hex
 *                  
 *  DESCRIPTION:     Convert unsigned hyper to hex
 *      
 *	ARGUMENTS:
 *      - unsigned hyper value (IN)
 *	    - unsigned char array(hex value) {IN/OUT}
 *
 *  RETURNS 	
 *	      Returns Zero or appropriate error code
*/

int convWriteUnHyperToHex(u_hyper a, unsigned char* b)
{
        u_hyper qut;
        u_hyper rem;
        int len = 0;
        int i;
        int count = 0;
        int count1 = 0;
        char* temp = NULL;
        temp = (char*)calloc (sizeof(char), HYPER_MAX_100);
        do {
                qut = a / 16;
                rem = a % 16;
                if (qut < 16 && rem < 16) {
                        if(qut < 10)
                                *(b + count) = (char)(qut + 48);
                        else
                                *(b + count) = (char)(qut + 55);
                        count++;
                        if(rem < 10)
                                *(b + count) = (char)(rem + 48);
                        else
                                *(b + count) = (char)(rem + 55);
                        count++;
                        a = qut;
                }
                else {
                        a = qut;
                        if(rem < 10) {
                                *(temp + count1) = (char)(rem + 48);
						}
                        else {
                                *(temp + count1) = (char)(rem + 55);
                        }
                        count1++;
                *(temp + count1) = '\0';
                }
        } while (a > 15);
        len = strlen(temp);
        for(i = len-1; i >= 0; i--) {
                *(b + count) = *(temp + i);
                count++;
        }
        *(b + count) = '\0';
        return (0);
}



/*!
 *  NAME:
 *      write_short() - Marshalling of short
 *                  
 *  DESCRIPTION:      Marshalling of short
 *      
 *	ARGUMENTS:
 *      - pRawValue (IN)
 *	    - pStream {IN/OUT}
 *
 *  RETURNS 	
 *	      Returns Zero or appropriate error code
*/
int write_short(int RawValue,void *pStream)
{
        int byteCount = 0;
        int count;
        char tempByte[3] = "ff";
        int byteToPrint;
        unsigned char bytes[INT_BITS];

        if(pStream == NULL)
                return(IPDR_NULL_INPUT_PARAMETER);

        sprintf((char *)bytes, "%04x", RawValue);
        for ( count = 0; count < SHORT_INT_BITS; count = count + 2 )
        {
                tempByte[0] = bytes[count];
                tempByte[1] = bytes[count+1];
                byteToPrint = convWriteIntHexToInt(tempByte);
                fprintf(pStream, "%c", byteToPrint);
        }

        return(IPDR_OK);
}

/*!
 *  NAME:
 *      write_u_short() - Marshalling of Unsigned short
 *                  
 *  DESCRIPTION:      Marshalling of Unsigned short
 *      
 *	ARGUMENTS:
 *      - pRawValue (IN)
 *	    - pStream {IN/OUT}
 *
 *  RETURNS 	
 *	      Returns Zero or appropriate error code
*/
int write_u_short(
				unsigned int RawValue, 
			    void* pStream
			  )

{
        
      int byteCount = 0;
      char tempByte[2] = "ff";
      int byteToPrint=0;
		unsigned char bytes[INT_BITS];
      int count=0;
      int counter=0;


		if(pStream==NULL)
			return(18);


       sprintf((char *)bytes, "%04x", RawValue);
       for(count=0;count<U_SHORT_INT_MAXCOUNT_2; count++)
       {
          counter=2*count;
          tempByte[0] = bytes[counter];
          tempByte[1] = bytes[counter+1];
          byteToPrint = convWriteUInthexToInt(tempByte);
          fprintf(pStream, "%c", byteToPrint);
		 }

    return(IPDR_OK);
}

/*!
 *  NAME:
 *      write_byte() - Marshalling of Byte
 *                  
 *  DESCRIPTION:      Marshalling of Byte
 *      
 *	ARGUMENTS:
 *      - pRawValue (IN)
 *	    - pStream {IN/OUT}
 *
 *  RETURNS 	
 *	      Returns Zero or appropriate error code
*/
int write_byte(int RawValue,void *pStream)
{
/*
        int byteCount = 0;
        int count;
        char tempByte[3] = "ff";
        int byteToPrint;
        unsigned char bytes[INT_BITS];

        if(pStream == NULL)
                return(IPDR_NULL_INPUT_PARAMETER);

        sprintf((char *)bytes, "%02x", RawValue);
        for ( count = 0; count < BYTE_BITS; count = count + 2 )
        {
                tempByte[0] = bytes[count];
                tempByte[1] = bytes[count+1];
                byteToPrint = convWriteIntHexToInt(tempByte);
                fprintf(pStream, "%c", byteToPrint);
        }
*/
       fprintf(pStream, "%c",RawValue);

        return(IPDR_OK);
}

/*!
 *  NAME:
 *      write_boolean() - Marshalling of Boolean
 *                  
 *  DESCRIPTION:      Marshalling of Boolean
 *      
 *	ARGUMENTS:
 *      - pRawValue (IN)
 *	    - pStream {IN/OUT}
 *
 *  RETURNS 	
 *	      Returns Zero or appropriate error code
*/
int write_boolean(char* RawValue,void *pStream)
{
        int byteCount = 0;
        int count;
        char tempByte[3] = "ff";
        int byteToPrint;
        unsigned char bytes[INT_BITS];

        if(pStream == NULL)
                return(IPDR_NULL_INPUT_PARAMETER);

		  if(strcmp(RawValue, "true") == 0) {
        	sprintf((char *)bytes, "%02x", 1);
		  } else if(strcmp(RawValue, "false") == 0) {
        	sprintf((char *)bytes, "%02x", 0);
		  }

        for ( count = 0; count < BYTE_BITS; count = count + 2 )
        {
                tempByte[0] = bytes[count];
                tempByte[1] = bytes[count+1];
                byteToPrint = convWriteIntHexToInt(tempByte);
                fprintf(pStream, "%c", byteToPrint);
        }

        return(IPDR_OK);
}
