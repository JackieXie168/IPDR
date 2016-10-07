/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/


/*****************************************************
 * File                 : errorHandler.h			 *
 * Description          : Error Handling functions   *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 * 15/01/02   Infosys      Created                   *
 *****************************************************/

#ifndef _ERRORHANDLER_H
#define _ERRORHANDLER_H

#include <ctype.h>
#include <sys/malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <sys/timeb.h>

#include "utils/IPDRCommon.h"

void freeListErrorStructure(ListErrorStructure** pHeadRef);

int addlistErrorStructure(ListErrorStructure** pHeadRef, 
			 int errorCode, 
			 hyper offsetValue, 
			 char *functionName, 
			 char *errorMessage, 
			 int recordCount, 
			 int errorSeverity);

int getLengthListErrorStructure(ListErrorStructure* pListErrorStructure);

int printListErrorStructure(ListErrorStructure *pListErrorStructure);

int appendListErrorStructure
                (
				 struct ListErrorStructure** pHeadRef, 
	 			 int errorCode, 
				 hyper offsetValue, 
				 char *functionName, 
				 char *errorMessage, 
				 int recordCount, 
				 int errorSeverity
				 ) ;

int getErrorMessage (char* errorMessage, int* errorCode);

int errorHandler(
				 IPDRCommonParameters* pIPDRCommonParameters, 
				 int* errorCode, 
				 char* functionName 
				 );

ErrorStructure* newExceptionStructure(void);

int freeExceptionStructure(ErrorStructure* pExceptionStructure);

int printExceptionStructure(ErrorStructure* pExceptionStructure);

int exceptionHandler(
				 IPDRCommonParameters* pIPDRCommonParameters, 
				 int* pErrorCode, 
				 char* functionName
				 );
#endif
