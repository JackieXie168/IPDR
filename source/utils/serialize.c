/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/



/**********************************************************************
* File                 : serialize.c                                  *
* Description          : The serialize				      *  
*				                                      *
* Author               : Infosys Tech Ltd.                            *   
* Modification History :                                              * 
* ----------------------------------------------------------------    *
*    Date      Name     Change/Description                            * 
*                                                                     * 
*                                                                     *
* ----------------------------------------------------------------    * 
**********************************************************************/


#include <stdio.h>
#include <string.h>
#include "utils/IPDRCommon.h"
#include "utils/IPDRMemory.h"

/*!
 *  NAME:
 *      serializeAttributeDescriptor() - converts AttributeDescriptor to string
 *                                                
 *  DESCRIPTION:
 *
 *
 *	ARGUMENTS:
 *      - AttributeDescriptor (IN/OUT)
 *	    - tempString (OUT)
 *
 *  RETURNS 	
 *	      Returns Zero or appropriate error code
*/
int serializeAttributeDescriptor(
				AttributeDescriptor* pAttributeDescriptor,
				char* tempString)
{
	int listLength = 0;
	strcpy(tempString , "");

	sprintf(tempString, "%s*%s*%s*%s$", 
	    pAttributeDescriptor->attributeName_,
            pAttributeDescriptor->attributeType_ ,
            pAttributeDescriptor->description_ ,
	    pAttributeDescriptor->derivedType_);
	return (IPDR_OK);
}




/*!
 *  NAME:
 *      deSerializeAttributeDescriptor() - converts string to AttributeDescriptor 
 *                                         structure
 *                                                
 *  DESCRIPTION:
 *
 *
 * ARGUMENTS:
 *      - AttributeDescriptor (IN/OUT)
 *     - tempString (OUT)
 *
 *  RETURNS  
 *       Returns Zero or appropriate error code
*/
/*int deSerializeAttributeDescriptor(
     char *inputString,
     AttributeDescriptor* pAttributeDescriptor)
{
   char* token;
   char seps[] = "*$";
   int tokenCount = 0;
   char *tmpString;
   unsigned int strLength = 0;

   strLength = strlen(inputString) + 1;
   tmpString = (char *) calloc (strLength, sizeof(char));
   strcpy(tmpString, inputString);
   
   token = strtok( tmpString, seps );
 
   while( token != NULL )
   {
      if (tokenCount == 2) strcpy(pAttributeDescriptor->attributeName_, token);
      if (tokenCount == 3) strcpy(pAttributeDescriptor->attributeType_, token);
      if (tokenCount == 4) strcpy(pAttributeDescriptor->description_, token);
      if (tokenCount == 5) strcpy(pAttributeDescriptor->derivedType_, token);
      token = strtok( NULL, seps );
      tokenCount++;
   }

   free(tmpString);
   free(token);
    
   return (IPDR_OK);
}
*/

int deSerializeAttributeDescriptor(
     char *inputString,
     AttributeDescriptor* pAttributeDescriptor)
{
  char* token;
   char seps[] = "*$";
   int tokenCount = 0;
   char *tmpString;
   unsigned int strLength = 0;
   int lengthDerivedType = 0;

   strLength = strlen(inputString) + 1;
   tmpString = (char *) calloc (strLength, sizeof(char));
   strcpy(tmpString, inputString);
   
   token = strtok( tmpString, "*");
 
   while( token != NULL )
   {
      if (tokenCount == 0) strcpy(pAttributeDescriptor->attributeName_, token);
      if (tokenCount == 1) strcpy(pAttributeDescriptor->attributeType_, token);
      if (tokenCount == 2) strcpy(pAttributeDescriptor->description_, token);
      if (tokenCount == 3) {
		  strcpy(pAttributeDescriptor->derivedType_, token);
		  token = strtok(pAttributeDescriptor->derivedType_, "$");
		  strcpy(pAttributeDescriptor->derivedType_, token);
	  }
      token = strtok( NULL, "*" );
      tokenCount++;
   }
      

   free(tmpString);
    
   return (IPDR_OK);
} 
