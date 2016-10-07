/**
 * Copyright (C) 2002, The IPDR Organization, all rights reserved.
 * The use and distribution of this software is governed by the terms of
 * the license agreement which can be found in the file LICENSE.TXT at
 * the top of this source tree.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND, either express or implied.
 *
 *
 * File                 : IPDRCommon.h                
 * Description          : Contains All ControlFile    
 *                        function declarations       
 * Author               : Infosys Tech Ltd            
 * Modification History :                             
 *----------------------------------------------------
 * Date       Name        Change/Description          
 *----------------------------------------------------
 * 01/29/02   Infosys      Created                    
 *
 */

#ifndef _CONTROLFILE_H_
#define _CONTROLFILE_H_

#include "ftp/FtpStructures.H"
#include "ftp/GetFileFromURL.H"

ControlFile* controlFileFromURL(ListErrorStruct* pErrorList,
								char* controlFileURL);

ControlFile* controlFileFromLocation(ListErrorStruct* pErrorList,
									 char* controlFileLocation);
			   
int freeControlFile(ControlFile* pControlFile);

int writeStart(ListErrorStruct* pErrorList ,
			   ControlFile* pControlFile);

int writeEnd(ListErrorStruct* pErrorList ,
			 ControlFile* pControlFile);

int addDocName(ListErrorStruct* pErrorList ,
			   ControlFile* pControlFile,
			   char* docName);

int getDocName(ListErrorStruct* pErrorList ,
			   ControlFile* pControlFile, 
			   char* docName);  

int isEnd(ListErrorStruct* pErrorList ,
		  ControlFile* pControlFile);

int getDocCount(ListErrorStruct* pErrorList ,
				ControlFile* pControlFile, 
				int* docCount);

int refreshControlFile(ListErrorStruct* errorList ,
					   ControlFile* controlFile);  

#endif //_CONTROLFILE_H_
