/** @file Common/DynamicArray.h
 *
 * Header file for dynamically resizable array object, as implemented
 * in Common/DynamicArray.c.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: DynamicArray.h,v 1.10 2005/03/13 21:15:58 dmitryk Exp $
 *
 * The dynamic array entity supports the dynamic adding, inserting,
 * removal operations and reallocates its internal data structure
 * accordingly.  The array's contents are assumed to be reference
 * counted objects allocated using Object_Allocate. Deleting the array
 * decreases the reference count of the array contents.
 */

#ifndef _DYNAMIC_ARRAY_H_
#define _DYNAMIC_ARRAY_H_

#include <IPDR/Config.h>
#define DYNAMIC_ARRAY_H_RCSID "@(#) $Id: DynamicArray.h,v 1.10 2005/03/13 21:15:58 dmitryk Exp $" IPDR_COPYRIGHT_STR

#if defined(GLOBAL_RENAME)
#define DynamicArray_AddElement     IPDR__DynamicArray_AddElement
#define DynamicArray_AppendElement  IPDR__DynamicArray_AppendElement
#define DynamicArray_Copy           IPDR__DynamicArray_Copy
#define DynamicArray_CountObjects   IPDR__DynamicArray_CountObjects
#define DynamicArray_Create         IPDR__DynamicArray_Create
#define DynamicArray_Delete         IPDR__DynamicArray_Delete
#define DynamicArray_Remove         IPDR__DynamicArray_Remove
#define DynamicArray_RemoveAndShift IPDR__DynamicArray_RemoveAndShift
#define DynamicArray_Reset          IPDR__DynamicArray_Reset
#define DynamicArray_SetElementAt   IPDR__DynamicArray_SetElementAt
#define DynamicArray_h_rcsid        IPDR__DynamicArray_h_rcsid
#define DynamicArray_rcsid          IPDR__DynamicArray_rcsid
#endif

#include <IPDR/Defs.h>
#include <IPDR/IPDR.h>
#include <Common/Platform.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DEFAULT_CAPACITY_INCREMENT 32

typedef struct
{
  void **Data;
  int Length;
  int Capacity;
}
DynamicArray;

#ifdef DEBUG_MEMORY
#define DynamicArray_Create(PS, Array, InitialCapacity) \
    DynamicArray_dm_Create(PS, Array, InitialCapacity, "DynamicArray_Create-" __FILE__, __LINE__)
IPDR_Status DynamicArray_dm_Create(IPDR_PlatformServices *PS,
                                    DynamicArray *Array,
                                    int InitialCapacity,
                                    const char* FileName, int LineNum);
#else
IPDR_Status DynamicArray_Create(IPDR_PlatformServices *PS,
                                 DynamicArray *Array,
                                 int InitialCapacity);
#endif
IPDR_Status DynamicArray_Reset(IPDR_PlatformServices *PS,
                                DynamicArray *Array,
                                int Capacity);
IPDR_Status DynamicArray_ResetNoFree(IPDR_PlatformServices *PS,
                                DynamicArray *Array,
                                int Capacity);
IPDR_Status DynamicArray_Delete(IPDR_PlatformServices *PS,
                                 DynamicArray *Array);
IPDR_Status DynamicArray_DeleteNoFree(IPDR_PlatformServices *PS,
                                 DynamicArray *Array);
IPDR_Status DynamicArray_Remove(IPDR_PlatformServices *PS,
                                 DynamicArray *Array,
                                 int Index);

IPDR_Status DynamicArray_RemoveNoFree(IPDR_PlatformServices *PS,
                                 DynamicArray *Array,
                                 int Index);

IPDR_Status DynamicArray_RemoveAndShift(IPDR_PlatformServices *PS,
    DynamicArray *Array,
    int Index);

IPDR_Status DynamicArray_RemoveAndShiftNoFree(IPDR_PlatformServices *PS,
    DynamicArray *Array,
    int Index);

IPDR_Status DynamicArray_AddElement(IPDR_PlatformServices *PS,
                                     DynamicArray *Array,
                                     void *Element, int *Index);
#ifdef DEBUG_MEMORY
#define DynamicArray_AppendElement(CPS, Array, Element, Index) \
    DynamicArray_dm_AppendElement(CPS, Array, Element, Index, "DynamicArray_AppendElement-" __FILE__, __LINE__)
IPDR_Status DynamicArray_dm_AppendElement(IPDR_PlatformServices *PS,
                                           DynamicArray *Array,
                                           void *Element, int *Index,
                                           const char* FileName, int LineNum);
#else
IPDR_Status DynamicArray_AppendElement(IPDR_PlatformServices *PS,
                                        DynamicArray *Array,
                                        void *Element, int *Index);
#endif
IPDR_Status DynamicArray_SetElementAt(IPDR_PlatformServices *PS,
                                       DynamicArray *Array,
                                       int Index,
                                       void *Element);
IPDR_Status DynamicArray_Copy(IPDR_PlatformServices *PS,
                               DynamicArray *Source,
                               DynamicArray *Target);
IPDR_Status DynamicArray_CountObjects(IPDR_PlatformServices *PS,
                                       DynamicArray *Array,
                                       unsigned int *N);


#ifdef __cplusplus
}
#endif

#endif
