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
 * Description          : Contains All Range File     
 *                        function declarations.       
 * Author               : Infosys Tech Ltd            
 * Modification History :                             
 *----------------------------------------------------
 * Date       Name        Change/Description          
 *----------------------------------------------------
 * 01/29/02   Infosys      Created                    
 *
 *
 *
 */

#ifndef _RANGEFILE_H_
#define _RANGEFILE_H_

#include "ftp/FtpStructures.H"
#include "ftp/GetFileFromURL.H"

int getFirstSequenceNbr(ListErrorStruct* pErrorList ,
						  char* rangeFileURL,
						  char* firstSeqNbr);

int getLastSequenceNbr(ListErrorStruct* pErrorList ,
						  char* rangeFileURL, 
						  char* lastSeqNbr);

int setFirstSequenceNbr(ListErrorStruct* pErrorList ,
						  char* rangeFilePath, 
						  char* firstSeqNbr);

int setLastSequenceNbr(ListErrorStruct* pErrorList ,
						  char* rangeFilePath, 
						  char* lastSeqNbr);

#endif //_RANGEFILE_H_
