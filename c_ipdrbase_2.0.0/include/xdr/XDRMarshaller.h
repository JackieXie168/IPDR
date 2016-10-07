/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/


/*****************************************************
 * File                 : XDRMarshaller              *
 * Description          :                            *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 *                                                   *
 *****************************************************/

#ifndef _XDRMARSHALLER_H
#define _XDRMARSHALLER_H

/* 
 * Marsheller Helper Header Library. 
 * This will be created as a separate header file & 
 * the functions will be available to the above C Program.   
 * Header File Name: "XDRMarshaller.h"
 */

#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "utils/IPDRCommon.h" 

/* 
 * This flag needs to be set to following values  
 *   1: Display Debug messages, 
 *   0: No Display of Debug messages 
 */

/* This definition is for integer */
#define INT_BITS              		8
#define SHORT_INT_BITS              4
#define BYTE_BITS                   2

/* This definition is for double */
#define DOUBLE_EXPONENT_BITS		12
#define DOUBLE_MANTISSA_BITS		52
#define DOUBLE_BITS			64
#define DOUBLE_HEXA_BITS		16
#define DOUBLE_CONSTANT			1023

/* This definition is for float */
#define FLOAT_EXPONENT_BITS		9
#define FLOAT_MANTISSA_BITS		32
#define FLOAT_HEXA_BITS			8
#define FLOAT_CONSTANT			127

/* This definition is for unsigned Integer */
#define U_INT_MAXCOUNT_4 		4
#define U_SHORT_INT_MAXCOUNT_2 		2
#define U_SHORT_INT_MAXCOUNT_1 		1
#define HYPER_MAX_100 			100
#define HYPER_MAXCOUNT_16 		16
#define HYPER_BITS 			16

/* This function prototype is for integer */
int write_long(int RawValue,void *pStream);
int convWriteIntHexToInt(char iHexa[3]);

/* This function prototype is for double */
int getWriteDoubleExponent(double d);
int getWriteDoubleBinaryAfterDecimal(double n, char *b);
int getWriteDoubleBinaryBeforeDecimal(double n, char *b);
char getWriteDoubleHexFromBinary(char binary[4]);
int getWriteDoubleHexFromDouble(char hexa[DOUBLE_HEXA_BITS], double d);
int write_double(double rawValue, void *streamPointer);

/* This function prototype is for float */
int getFloatWriteExponent(double d);
int getFloatWriteBinaryAfterDecimal(double n, char *b);
int getFloatWriteBinaryBeforeDecimal(double n, char *b);
char getFloatWriteHexFromBinary(char binary[4]);
int getFloatWriteHexFromDouble(char hexa[FLOAT_HEXA_BITS], double d);
int hexToInt (char iHexa[3]);
int write_float(float rawValue, void *streamPointer);

/* This definition is for unsigned Integer */
int convWriteUInthexToInt(char iHexa[3]);
int write_u_long(unsigned int RawValue, void* pStream);

/* This definition is for Hyper Write */
int convWriteHyperHexToInt(char iHexa[3]);
int convWriteHyperToHex(hyper a, unsigned char* b);
int write_longlong(hyper RawValue,FILE *pStream);

/* This definition is for String Write */
int write_string(char* pRawValue, void* pStream); 

/* This definition is for UTF8String Write */
/*int write_wstring(wchar* pRawValue, void *pStream);*/
/* Signature changed in order to pass the schema version parameter now */
int write_wstring(wchar* pRawValue, void *pStream, char* schemaVer);
int getUTF8String(char* srcString, char* UTF8String);
int checkBigEndian();


/* This definition is for ByteArray Write */
int write_octet_array(byte* pRawValue, void *pStream);
/*int write_noctet_array(byte* pRawValue, void *pStream, int length);*/
/* code change to pass the schema version param */
int write_noctet_array(byte* pRawValue, void *pStream, int length, char* schemaVer);


/* This definition is for Unsigned Hyper Write */
int write_u_longlong(u_hyper RawValue,FILE *pStream);
int convWriteUnHyperToHex(u_hyper a, unsigned char* b);


int write_short(int RawValue,void *pStream);
int write_u_short( unsigned int RawValue, void* pStream);
int write_byte(int RawValue,void *pStream);
int write_boolean(char* RawValue,void *pStream);

#endif



