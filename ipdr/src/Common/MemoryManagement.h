/**
 * @file Common/MemoryManagement.h
 *
 * Header file for memory management oriented functionality, as
 * implemented in COmmon/MemoryManagement.c.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: MemoryManagement.h,v 1.10 2005/05/09 13:30:01 dmitryk Exp $
 *
 * The file contains definition for functions concerning:
 * <ul>
 *   <li>Reference counted objects handling
 *   <li>Object destruction on deallocation
 * </ul>
 *
 * @note There is a dependency between this code and
 * Platforms/Generic/GenericPlatform.c, in the layout of object
 * headers.
 */
#ifndef _MEMORY_MANAGEMENT_H_
#define _MEMORY_MANAGEMENT_H_

#include <IPDR/Config.h>
#define MEMORY_MANAGEMENT_H_RCSID "@(#) $Id: MemoryManagement.h,v 1.10 2005/05/09 13:30:01 dmitryk Exp $" IPDR_COPYRIGHT_STR

#if defined(GLOBAL_RENAME)
#define MemoryManagement_h_rcsid IPDR__MemoryManagement_h_rcsid
#define MemoryManagement_rcsid   IPDR__MemoryManagement_rcsid
#define Object_Allocate          IPDR__Object_Allocate
#define Object_Delete            IPDR__Object_Delete
#define Object_Duplicate         IPDR__Object_Duplicate
#define Object_GetReferenceCount IPDR__Object_GetReferenceCount
#define Object_Release           IPDR__Object_Release
#endif

#include <stddef.h>
#include <IPDR/Defs.h>
#include <IPDR/IPDR.h>
#include <Common/Platform.h>

#ifdef __cplusplus
extern "C" {
#endif

#if 0
} /* put this here to un-confuse Emacs automatic indentation */
#endif


/**
 * Header placed before a reference counted
 * object. Contains the object's reference count and a
 * pointer to the object's destructor.
 *
 * See also #GenericMemHdr in Platforms/Generic/GenericPlatform.h.
 */
typedef struct
{
    unsigned int size; /**< MUST be first to match with #Platform_MEMORYALLOCATESIZEWITHHDR */
    unsigned int eyecatcher; /**< "signature" for debugging: gets set to "a0a1a2a3" for validating pointers to ObjectHeader (set to something else when memory is freed): see same field in GenericMemHdr */
    int ReferenceCount;
    void (*Destructor)(IPDR_PlatformServices *PS, void *);
}
ObjectHeader;

#ifdef DEBUG_MEMORY
#define Object_Allocate(PS, Size, Destructor, Object) \
  Object_dm_Allocate(PS, Size, Destructor, Object, "Object_Allocate-" __FILE__, __LINE__)
#define Object_Delete(PS, Object) \
  Object_dm_Delete(PS, Object, "Object_Delete-" __FILE__, __LINE__)
#define Object_Duplicate(PS, Object) \
  Object_dm_Duplicate(PS, Object, "Object_Duplicate-" __FILE__, __LINE__)
#define Object_Release(PS, Object) \
  Object_dm_Release(PS, Object, "Object_Relese-" __FILE__, __LINE__)

IPDR_Status Object_dm_Allocate(IPDR_PlatformServices *PS,
                                size_t Size,
                                void (*Destructor)(IPDR_PlatformServices *PS, void *),
                                void **Object,
                                const char* FileName,
                                int lineNum);
IPDR_Status Object_dm_Delete(IPDR_PlatformServices *PS,
                              void *Object,
                              const char* FileName,
                              int lineNum);
IPDR_Status Object_dm_Duplicate(IPDR_PlatformServices *PS,
                                 void *Object,
                                 const char* FileName,
                                 int lineNum);
IPDR_Status Object_dm_Release(IPDR_PlatformServices *PS,
                               void *Object,
                               const char* FileName,
                               int lineNum);
#else
IPDR_Status Object_Allocate(IPDR_PlatformServices *PS,
                             size_t Size,
                             void (*Destructor)(IPDR_PlatformServices *PS, void *),
                             void **Object);
IPDR_Status Object_Delete(IPDR_PlatformServices *PS,
                           void *Object);
IPDR_Status Object_Duplicate(IPDR_PlatformServices *PS,
                              void *Object);
IPDR_Status Object_Release(IPDR_PlatformServices *PS,
                            void *Object);
#endif
int Object_GetReferenceCount(IPDR_PlatformServices *PS,
                             void *Object);

#ifdef __cplusplus
}
#endif

#endif
