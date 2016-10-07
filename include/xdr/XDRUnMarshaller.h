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

#ifndef _XDRUNMARSHALLER_H
#define _XDRUNMARSHALLER_H

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "utils/IPDRCommon.h"

/*
 * Marsheller Helper Header Library. 
 * This will be created as a separate header file & 
 * the functions will be available to the above C Program.   
 * Header File Name: "XDRMarshaller.h"
 */

/* This definition is for integer */
#define INT_BYTES              	4	
#define SHORT_INT_BYTES          2
#define BYTE_BYTES               1

/* This definition is for unsigned integer */
#define U_INT_MAXCOUNT_4 	 	4		
#define U_INT_MAXCOUNT_2 	 	2		
#define U_INT_MAXCOUNT_1 	 	1		

/* This definition is for double */
#define DOUBLE_EXPONENT_BITS    	12
#define DOUBLE_MANTISSA_BITS		52
#define DOUBLE_HEXA_BYTES		8
#define DOUBLE_HEXA_BITS		16
#define DOUBLE_CONSTANT	        	1023

/* This definition is for float */
#define FLOAT_HEXA_BYTES		4
#define FLOAT_HEXA_BITS			8
#define FLOAT_EXPONENT_BITS		9
#define FLOAT_MANTISSA_BITS	        32
#define FLOAT_CONSTANT	        	127
#define HYPER_MAX_BYTES			8

/* This definition is for integer */
int convReadIntHexToInt(int count, char iHexa[3]);
void convReadIntCharHexToInt(char *phex, int *pSB);
int read_long(void* pStream);

/* This definition is for unsigned integer */
unsigned int convReadUIntHexToInt(int count, char iHexa[3]);
void convReadUIntCharHexToInt(char *hex, int *SB);
unsigned int read_u_long(void* pStream);

/* This definition is for double */
double getReadDoubleMantissa(char Mantissa[DOUBLE_MANTISSA_BITS]);
int getReadDoubleExponent(char Exponent[DOUBLE_EXPONENT_BITS     ]);
int convReadDoubleHexToBinary(char hexa, char binary[4]);
double read_double(void *streamPtr);

/* This definition is for float */
float getFloatReadMantissa(char Mantissa[FLOAT_MANTISSA_BITS]);
int getFloatReadExponent(char Exponent[FLOAT_EXPONENT_BITS]);
int convFloatReadHexToBinary(char hexa, char binary[4]);
float read_float(void *streamPtr);

/* This definition is for String */
char* read_string(int length, void* pStream);

/* This definition is for UTF8String */
/* Signature change to pass the schemaVersion parameter */
wchar* read_wstring(int length, void* pStream, char* schemaVer);

/* This definition is for Byte Array */
/* Signature change to pass the schemaVersion parameter */
byte* read_octet_array(int length, void* pStream, char* schemaVer);

/* This definition is for Hyper */
hyper hyperReadHexToInt(int count, char iHexa[3]);
void convReadHyperCharHexToInt(char *hex, hyper *SB);
hyper* read_longlong(void* pStream);

/* This definition is for unsigned Hyper */
u_hyper unhyperReadHexToInt(int count, char iHexa[3]);
void convReadUnHyperCharHexToInt(char *hex, u_hyper *SB);
u_hyper* read_u_longlong(void* pStream);

short int read_short(void* pStream);
unsigned short int read_u_short(void* pStream);
/*int read_byte(void* pStream);*/
char read_byte(void* pStream);
int read_boolean(void* pStream);

int convReadShortIntHexToShortInt(int count, char iHexa[3]);
unsigned int convReadUShortIntHexToShortInt(int count, char iHexa[3]);
int convReadIntHexToByte(int count, char iHexa[3]);


#endif


