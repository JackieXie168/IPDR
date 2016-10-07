/** @file Common/CommonUtil.h
 *
 * Header file for misc utility functions, as implemented by
 * Common/CommonUtil.c.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: CommonUtil.h,v 1.9 2005/01/30 15:41:24 guyg Exp $
 * 
 */
#ifndef _UTIL_H_
#define _UTIL_H_

#include <IPDR/Config.h>
#define COMMON_UTIL_H_RCSID "@(#) $Id: CommonUtil.h,v 1.9 2005/01/30 15:41:24 guyg Exp $" IPDR_COPYRIGHT_STR

#if defined(GLOBAL_RENAME)
#define CommonUtil_h_rcsid   IPDR__CommonUtil_h_rcsid
#define CommonUtil_rcsid     IPDR__CommonUtil_rcsid
#define CRCposix             IPDR__CRCposix
#define CRCposixInit         IPDR__CRCposixInit
#define CRCposixPostprocess  IPDR__CRCposixPostprocess
#define Util_Pad32           IPDR__Util_Pad32
#define Util_ReadChar        IPDR__Util_ReadChar
#define Util_ReadInt         IPDR__Util_ReadInt
#define Util_ReadLong        IPDR__Util_ReadLong
#define Util_ReadShort       IPDR__Util_ReadShort
#define Util_ReadString      IPDR__Util_ReadString
#define Util_ReadUnsignedInt IPDR__Util_ReadUnsignedInt
#define Util_StrDup          IPDR__Util_StrDup
#define Util_WriteChar       IPDR__Util_WriteChar
#define Util_WriteInt        IPDR__Util_WriteInt
#define Util_WriteLong       IPDR__Util_WriteLong
#define Util_WriteShort      IPDR__Util_WriteShort
#define Util_WriteString     IPDR__Util_WriteString
#endif

#include <IPDR/Defs.h>
#include <IPDR/IPDR.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DEBUG_MEMORY
#define Util_StrDup(PS, Source, Target) \
    Util_dm_StrDup(PS, Source, Target, "StrDup-" __FILE__, __LINE__)
IPDR_Status Util_dm_StrDup(IPDR_PlatformServices *PS,
                            const char *Source, char **Target,
                            const char* FileName, int LineNum);
#else
IPDR_Status Util_StrDup(IPDR_PlatformServices *PS,
                         const char *Source, char **Target);
#endif

#ifdef DEBUG_MEMORY
#define Util_StrDupAndReplace(PS, Source, Target) \
    Util_dm_StrDup(PS, Source, Target, "StrDup-" __FILE__, __LINE__)
IPDR_Status Util_dm_StrDupAndReplace(IPDR_PlatformServices *PS,
                                      const char *Source, char **Target,
                                      const char* FileName, int LineNum);
#else
IPDR_Status Util_StrDupAndReplace(IPDR_PlatformServices *PS,
                                   const char *Source, char **Target);
#endif

int Util_Pad32(unsigned int Size);

#ifdef PERSISTENT_CONFIGURATION

int Util_WriteInt(char *Buf, int Pos, int Value);
int Util_WriteShort(char *Buf, int Pos, short Value);
int Util_WriteLong(char *Buf, int Pos, long Value);
int Util_WriteString(char *Buf, int Pos, const char *Value);
int Util_WriteChar(char *Buf, int Pos, char Value);
int Util_ReadChar(const char *Buf, int Pos, char *Value);
int Util_ReadInt(const char *Buf, int Pos, int *Value);
int Util_ReadUnsignedInt(const char *Buf, int Pos, unsigned int *Value);
int Util_ReadShort(const char *Buf, int Pos, short *Value);
int Util_ReadLong(const char *Buf, int Pos, long *Value);
int Util_ReadString(const char *Buf, int Pos, const char **Value);

#endif

#if 0 /* || defined(DOXYGEN_PROCESSING) */

/** @defgroup CRC_calculation CRC calculation
 *
 * Posix CRC calculation. This gives the
 * same result as the Unix cksum(1) command. To use (where byteCount
 * bytes have been read into buf and totalByteCount is the total of
 * all byteCount's for all calls to CRCposix()):
 *
 * <code>
 * CRC_t crc = CRCposixInit();
 * <br>crc = CRCposix(crc, buf, byteCount);
 * <br>&nbsp;&nbsp;... <i>repeat CRCposix() with additional buffers as needed</i> ...
 * <br>crc = CRCposixPostprocess(crc, totalByteCount);
 * </code>
 *
 * @note  Not used right now ... intended for future protocol extension
 *       to contain checksum.
 *
 * @note It is not clear what the Copyright status is of this code.
*/

/** Type returned from CRC calculation - at least 32 bits.
 */
typedef unsigned long CRC_t;

CRC_t CRCposixInit(void);
CRC_t CRCposix(CRC_t crc, const unsigned char *buf, unsigned long byteCount);
CRC_t CRCposixPostprocessy(CRC_t crc, unsigned long totalByteCount);

#endif

#ifdef __cplusplus
}
#endif

#endif
