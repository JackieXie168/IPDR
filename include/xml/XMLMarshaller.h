/* Copyright (C) 2002, The IPDR Organization, all rights reserved.
* The use and distribution of this software is governed by the terms of
* the license agreement which can be found in the file LICENSE.TXT at
* the top of this source tree.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
* ANY KIND, either express or implied.
*/


/*****************************************************
 * File                 : XMLMarshaller.h            *
 * Description          :                            *
 * Author               : Infosys Tech Ltd           *
 * Modification History :                            *
 *---------------------------------------------------*
 * Date       Name        Change/Description         *
 *---------------------------------------------------*
 * 24-Feb-02                                         *
 *****************************************************/

#ifndef _XMLMARSHALLER_H
#define _XMLMARSHALLER_H

# define MAX_XML_STRING_LENGTH 300

int write_xml(
			  char *pXMLString, 
			  void *pXMLStream
			  );

#endif
