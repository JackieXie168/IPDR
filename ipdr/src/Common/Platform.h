/** @file Common/Platform.h
 *
 * Macros for perform platform specific services.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: Platform.h,v 1.13 2005/04/21 11:31:22 dmitryk Exp $
 *
 */
#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include <IPDR/Config.h>
#define PLATFORM_H_RCSID "@(#) $Id: Platform.h,v 1.13 2005/04/21 11:31:22 dmitryk Exp $" IPDR_COPYRIGHT_STR

#define Platform_h_rcsid IPDR__Platform_h_rcsid

#ifdef __cplusplus
extern "C" {
#endif

#if defined(SNMP_SUPPORT)	
#define Platform_MIB_Start(S) (S)->MIBServices.MIB_Start()
#define Platform_MIB_SetExporterHandle(S, E) (S)->MIBServices.MIB_SetExporterHandle(E)
#define Platform_MIB_UnSetExporterHandle(S) (S)->MIBServices.MIB_UnSetExporterHandle()
#define Platform_MIB_IsRunning(S) (S)->MIBServices.MIB_IsRunning()
#define Platform_MIB_Shop(S) (S)->MIBServices.MIB_Shop()
#endif

#define Platform_Start(S) (S)->Start((S)->PlatformArg)
#define Platform_Stop(S) (S)->Stop((S)->PlatformArg)
#define Platform_IsRunning(S) (S)->IsRunning((S)->PlatformArg)
#define Platform_Delete(S) (S)->Delete((S)->PlatformArg)
#define Platform_GetLastErrorMessage(S, Msg) (S)->GetLastErrorMessage((S)->PlatformArg, Msg)
#define Platform_SetExporterHandle(S, CH) (S)->SetExporterHandle((S)->PlatformArg, CH)
#define Platform_UnsetExporterHandle(S) (S)->UnsetExporterHandle((S)->PlatformArg)
#define Platform_RegisterTimer(S, Callback, Arg, Timeout, ID) \
(S)->EventServices.RegisterTimer((S)->PlatformArg, \
Callback, Arg, Timeout, ID)
#define Platform_ReregisterTimer(S, Callback, Arg, Timeout, ID) \
(S)->EventServices.ReregisterTimer((S)->PlatformArg, Callback, Arg, Timeout, ID)
#define Platform_RegisterPeriodicTimer(S, Callback, Arg, Timeout, ID) \
(S)->EventServices.RegisterPeriodicTimer((S)->PlatformArg, \
Callback, Arg, Timeout, ID)
#define Platform_UnregisterTimer(S, ID) \
(S)->EventServices.UnregisterTimer((S)->PlatformArg, ID)
#define Platform_UnregisterTimerAndClear(S, ID) \
{ Platform_UnregisterTimer(S, *(ID)); *(ID) = NULL; }
#if 0
/* Not used (for debugging) */
#define Platform_DescribeTimer(S, Descr, ID) \
(S)->EventServices.DescribeTimer((S)->PlatformArg, Descr, ID)
#endif

#define Platform_RegisterReadSocket(S, SK, CB ,Arg, ID) \
(S)->EventServices.RegisterReadSocket((S)->PlatformArg, \
SK, CB, Arg, ID)
#define Platform_RegisterWriteSocket(S, SK, CB ,Arg, ID) \
(S)->EventServices.RegisterWriteSocket((S)->PlatformArg, \
SK, CB, Arg, ID)
#define Platform_UnregisterSocket(S, ID) \
(S)->EventServices.UnregisterSocket((S)->PlatformArg, ID)
#define Platform_UnregisterSocketAndClear(S, ID) \
{ Platform_UnregisterSocket(S, *(ID)); *(ID) = NULL; }

#ifdef DEBUG_MEMORY
/* #include <Platforms/Generic/Generic.h> */
extern int GenericPlatform_dm_MemoryAllocate(IPDR_PlatformArg P, int Size, void **Buf,
                                             const char* FileName, int LineNum);
extern int GenericPlatform_dm_MemoryFree(IPDR_PlatformArg P, void *Buf,
                                         const char* FileName, int LineNum);
#define X_Platform_MemoryAllocate(S, Size, Buf) \
    GenericPlatform_dm_MemoryAllocate(S, Size, Buf, __FILE__, __LINE__)
#define X_Platform_MemoryAllocateWithHdr(S, Size, Buf) \
    GenericPlatform_dm_MemoryAllocate(S, HdrSize, Size, Buf, __FILE__, __LINE__)
#define Platform_MemoryFree(S, Buf) \
    GenericPlatform_dm_MemoryFree(S, Buf, __FILE__, __LINE__)
#else
#define X_Platform_MemoryAllocate(S, Size, Buf) \
(S)->MemoryServices.MemoryAllocate((S)->PlatformArg, Size, Buf)
#define X_Platform_MemoryAllocateWithHdr(S, HdrSize, Size, Buf) \
(S)->MemoryServices.MemoryAllocateWithHdr((S)->PlatformArg, HdrSize, Size, Buf)
#define Platform_MemoryFree(S, Buf) \
(S)->MemoryServices.MemoryFree((S)->PlatformArg, Buf)
#define Platform_MemoryFreeWithHdr(S, HdrSize, Buf) \
(S)->MemoryServices.MemoryFreeWithHdr((S)->PlatformArg, HdrSize, Buf)
#endif

/* For memory allocation, we don't use the above macros but instead
   use these helpers. They all expect the "Buf" parameter to be the
   pointer, so sizeof *Buf will be the size of the
   individual element. In effect, these do:
       Buf = malloc(sizeof *Buf)
   or
       Buf = malloc(Num * sizeof *Buf)   (for vectors)

   For slightly obscure reasons, these are prefixed with "X_"
   ... that's because they shouldn't be used outside GenericPlatform
   implementation without good cause (objects ought to be allocated by
   using ChunksManager.c, DynamicArray.c or the object handler in
   MemoryManagement.c.
*/

#ifdef DEBUG_MEMORY
#define Platform_dm_MEMORYALLOCATE(P, Buf, File, Line) \
    X_Platform_dm_MemoryAllocate(P, sizeof *(Buf), (void**)&(Buf, File, Line))
#define Platform_dm_MEMORYALLOCATEVEC(P, Num, Buf, File, Line) \
    X_Platform_dm_MemoryAllocate(P, (Num) * (sizeof *(Buf)), (void**)&(Buf, File, Line))
#define Platform_dm_MEMORYALLOCATESIZE(P, Size, Buf, File, Line) \
    X_Platform_dm_MemoryAllocate(P, Size, (void**)&(Buf, File, Line))
#define Platform_dm_MEMORYALLOCATESIZEWITHHDR(P, HdrSize, Size, Buf, File, Line) \
    X_Platform_dm_MemoryAllocateWithHdr(P, HdrSize, Size, (void**)&(Buf, File, Line))
#endif

#define Platform_MEMORYALLOCATE(P, Buf) \
    X_Platform_MemoryAllocate(P, sizeof *(Buf), (void**)&(Buf))
#define Platform_MEMORYALLOCATEVEC(P, Num, Buf) \
    X_Platform_MemoryAllocate(P, (Num) * (sizeof *(Buf)), (void**)&(Buf))
#define Platform_MEMORYALLOCATESIZE(P, Size, Buf) \
    X_Platform_MemoryAllocate(P, Size, (void**)&(Buf))
#define Platform_MEMORYALLOCATESIZEWITHHDR(P, HdrSize, Size, Buf) \
    X_Platform_MemoryAllocateWithHdr(P, HdrSize, Size, (void**)&(Buf))

#ifdef STD_FILE_OPERATIONS_SUPPORT
#define X_MEMPANIC(Buf, File, Line) \
    fprintf(stderr, "*\n***MEMORY ALLOCATION failed for %u bytes at %.99s:%d ***\n\n", sizeof *(Buf), File, Line)
#define X_MEMPANICVEC(Num, Buf, File, Line) \
    fprintf(stderr, "*\n***MEMORY ALLOCATION failed for %u elements of %ud bytes = %u bytes at %.99s:%d ***\n\n", \
           (Num), sizeof *(Buf),  (Num) * (sizeof *(Buf)), File, Line)
#define X_MEMPANICSIZE(Size, File, Line) \
    fprintf(stderr, "*\n***MEMORY ALLOCATION failed for %u bytes at %.99s:%d ***\n\n", Size, File, Line)
#else
#define X_MEMPANIC(Buf, File, Line)
#define X_MEMPANICVEC(Num, Buf, File, Line)
#define X_MEMPANICSIZE(Size, File, Line)
#endif

#define Platform_Check_MEMORYALLOCATE(P, Buf, CleanupAction)                    \
{ IPDR_Status x_rc = Platform_MEMORYALLOCATE(P, Buf);                          \
  if (FAILURE(x_rc)) {                                                       \
    X_MEMPANIC(Buf, __FILE__, __LINE__);                                        \
    {CleanupAction}                                                             \
    return x_rc;                                                                \
  }                                                                             \
}
#define Platform_Check_MEMORYALLOCATEVEC(P, Num, Buf, CleanupAction)            \
{ IPDR_Status x_rc = Platform_MEMORYALLOCATEVEC(P, Num, Buf);                  \
  if (FAILURE(x_rc)) {                                                       \
    X_MEMPANICVEC(Num, Buf, __FILE__, __LINE__);                                \
    {CleanupAction}                                                             \
    return x_rc;                                                                \
  }                                                                             \
}
#define Platform_Check_MEMORYALLOCATESIZE(P, Size, Buf, CleanupAction)          \
{ IPDR_Status x_rc = Platform_MEMORYALLOCATESIZE(P, Size, Buf);                \
  if (FAILURE(x_rc)) {                                                       \
    X_MEMPANICSIZE(Size, __FILE__, __LINE__);                                   \
    {CleanupAction}                                                             \
    return x_rc;                                                                \
  }                                                                             \
}
#define Platform_Check_MEMORYALLOCATESIZEWITHHDR(P, HdrSize, Size, Buf, CleanupAction) \
{ IPDR_Status x_rc = Platform_MEMORYALLOCATESIZEWITHHDR(P, HdrSize, Size, Buf); \
  if (FAILURE(x_rc)) {                                                       \
    X_MEMPANICSIZE((HdrSize) + (Size), __FILE__, __LINE__);                     \
    {CleanupAction}                                                             \
    return x_rc;                                                                \
  }                                                                             \
}
#define Platform_MemoryStatistics(S, numAllocs, numFrees, bytesAllocated) \
(S)->MemoryServices.MemoryStatistics((S)->PlatformArg, numAllocs, numFrees, bytesAllocated)

#ifdef DEBUG_LOCKS
extern int GenericPlatform_db_LockPlatform(IPDR_PlatformArg S, const char*, int);
extern int GenericPlatform_db_UnlockPlatform(IPDR_PlatformArg S, const char*, int);
#define Platform_Lock(S) \
    GenericPlatform_db_LockPlatform((S)->PlatformArg, __FILE__, __LINE__)
#define Platform_Unlock(S) \
    GenericPlatform_db_UnlockPlatform((S)->PlatformArg, __FILE__, __LINE__)
#else
#define Platform_Lock(S) \
if ((S)->LockingServices.Lock) (S)->LockingServices.LockPlatform((S)->PlatformArg)
#define Platform_Unlock(S) \
if ((S)->LockingServices.Unlock) (S)->LockingServices.UnlockPlatform((S)->PlatformArg)
#endif

#define Platform_PersistenceAvailable(S) \
((S)->PersistenceServices.PersistentStore != NULL)

#define Platform_PersistentStore(S, U, Buf, Len) \
(S)->PersistenceServices.PersistentStore((S)->PlatformArg, \
U, Buf, Len)

#define Platform_PersistentRetrieve(S, U, Buf, Len) \
(S)->PersistenceServices.PersistentRetrieve((S)->PlatformArg, \
U, Buf, Len)

#define Platform_CreateTCPSocket(S, SK) \
(S)->NetworkServices.CreateTCPSocket((S)->PlatformArg, SK)
#define Platform_CreateUDPSocket(S, SK) \
(S)->NetworkServices.CreateUDPSocket((S)->PlatformArg, SK)
#define Platform_Bind(S, SK, Addr, Port) \
(S)->NetworkServices.Bind((S)->PlatformArg, SK, Addr, Port)
#define Platform_Connect(S, SK, Addr, Port) \
(S)->NetworkServices.Connect((S)->PlatformArg, SK, Addr, Port)
#define Platform_GetSockName(S, SK, Addr, Port) \
(S)->NetworkServices.GetSockName((S)->PlatformArg, SK, Addr, Port)
/* no Platform_CloseAndInvalidate(S, SP) ... we always close+invalidate socket descriptor */
#define Platform_CloseAndInvalidate(S, SP) \
{ (S)->NetworkServices.Close((S)->PlatformArg, *(SP)); IPDR_SetInvalidSocket(SP); }
#define Platform_Listen(S, SK, BL) \
(S)->NetworkServices.Listen((S)->PlatformArg, SK, BL)
#define Platform_Accept(S, SK, Addr, Port, ASK) \
(S)->NetworkServices.Accept((S)->PlatformArg, SK, Addr, Port, ASK)
#define Platform_SetExporterSocketParams(S, SK, F, A) \
(S)->NetworkServices.SetExporterSocketParams((S)->PlatformArg, SK, F, A)
#define Platform_Send(S, SK, Buf, Len, B) \
(S)->NetworkServices.Send((S)->PlatformArg, SK, Buf, Len, B)
#define Platform_Recv(S, SK, Buf, Len, Timeout, RB) \
(S)->NetworkServices.Recv((S)->PlatformArg, SK, Buf, Len, \
Timeout, RB)
#define Platform_RecvFrom(S, SK, Buf, Len, Addr, Port, RB) \
(S)->NetworkServices.RecvFrom((S)->PlatformArg, \
SK, Buf, Len, Addr, Port, RB)
#define Platform_SendTo(S, SK, Buf, Len, Addr, Port) \
(S)->NetworkServices.SendTo((S)->PlatformArg, SK, Buf, Len, \
Addr, Port)
#define Platform_Shutdown(S, SK) \
(S)->NetworkServices.Shutdown((S)->PlatformArg, SK)
#define Platform_GetHostName(S, N, L) ((S)->NetworkServices.GetHostName((S)->PlatformArg, N, L))
#define Platform_GetTime(S, T) (S)->TimeServices.GetTime((S)->PlatformArg, T)
#define Platform_SetTime(S, T) (S)->TimeServices.SetTime((S)->PlatformArg, T)
#define Platform_SleepMS(S, T) (S)->TimeServices.SleepMS((S)->PlatformArg, T)
#define Platform_FormatTimeLocalNow(S, B) ((S)->TimeServices.FormatTimeLocalNow((S)->PlatformArg, B))
#define Platform_FormatTimeGMTNow(S, B) ((S)->TimeServices.FormatTimeGMTNow((S)->PlatformArg, B))
#define Platform_FormatTimeLocal(S, T, B) ((S)->TimeServices.FormatTimeLocal((S)->PlatformArg, T, B))
#define Platform_FormatTimeGMT(S, T, B) ((S)->TimeServices.FormatTimeGMT((S)->PlatformArg, T, B))
#define Platform_FormatTimeSyslogNow(S, B) ((S)->TimeServices.FormatTimeSyslogNow((S)->PlatformArg, B))

#ifdef __cplusplus
}
#endif

#endif
