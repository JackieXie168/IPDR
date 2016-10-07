/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/


/*****************************************************
 * File                 : XMLMarshaller              *
 * Description          :                            *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 * 24-Feb-02                                         *
 *****************************************************/


#include "xml/XMLMarshaller.h"
#include "utils/errorCode.h"
#include "utils/IPDRCommon.h"


/*
 *  NAME:
 *      write_xml() - Marshalling for XML Data
 *                  
 *  DESCRIPTION:     Marshalling for XML Data 
 *      
 *	ARGUMENTS:
 *      - pXMLString (IN/OUT)
 *	    - pXMLStream {IN/OUT}
 *
 *  RETURNS 	
 *	     Returns Zero if the operation is successful or appropriate
 * 		 error code in case of failure.
 */


int write_xml(
			  char *pXMLString, 
			  void *pXMLStream
			  )
{
	
	if( (pXMLString == NULL) || (pXMLStream == NULL) )
         return(IPDR_NULL_INPUT_PARAMETER);
	
	fprintf(pXMLStream, "%s", pXMLString);    

    return(IPDR_OK);
}

