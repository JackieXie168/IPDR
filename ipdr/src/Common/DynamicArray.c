/** @file Common/DynamicArray.c
 *
 * DynamicArray implementation.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: DynamicArray.c,v 1.13 2005/03/29 09:46:06 dmitryk Exp $
 *
 * The dynamic array entity supports the dynamic adding, inserting,
 * removal operations and reallocates its internal data structure
 * accordingly.
 */

#include <IPDR/Config.h>
const char DynamicArray_rcsid[] = "@(#) $Id: DynamicArray.c,v 1.13 2005/03/29 09:46:06 dmitryk Exp $" IPDR_COPYRIGHT_STR;

#include <Common/DynamicArray.h>

const char DynamicArray_h_rcsid[] = DYNAMIC_ARRAY_H_RCSID;

#include <stdio.h>

#include <Common/MemoryManagement.h>
#include <Common/Platform.h>

/**
 * Initialize a new dynamic array.
 *
 * @param PS handle to a platform services structure
 * @param Array array to initialize
 * @param InitialCapacity initial size of array
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
#ifdef DEBUG_MEMORY
IPDR_Status DynamicArray_dm_Create(IPDR_PlatformServices *PS,
                                    DynamicArray *Array,
                                    int InitialCapacity,
                                    const char* FileName, int LineNum)
#else
    IPDR_Status DynamicArray_Create(IPDR_PlatformServices *PS,
                                     DynamicArray *Array,
                                     int InitialCapacity)
#endif
{
    int i;

    if (InitialCapacity == 0) {
        Array->Data = NULL;
        Array->Length = Array->Capacity = 0;
        return IPDR_OK;
    };

#ifdef DEBUG_MEMORY
    {
        IPDR_Status Status =
            Platform__dm_MEMORYALLOCATEVEC(PS, InitialCapacity,Array->Data,
                                           FileName, LineNum);
        Platform_Check_MEMORYALLOCATEVEC(PS, InitialCapacity, Array->Data, {});
        if (FAILURE(Status)) {
            X_MEMPANICVEC(InitialCapacity, Array->Data, FileName, LineNum);
            return Status;
        }
    }
#else
    Platform_Check_MEMORYALLOCATEVEC(PS, InitialCapacity, Array->Data, {});
#endif

    for (i = 0; i < InitialCapacity; i++)
        Array->Data[i] = NULL;

    Array->Capacity = InitialCapacity;
    Array->Length = 0;

    return IPDR_OK;
}

/**
 * Reset the capacity of an array to the
 *   requested number of entities.
 *
 * @param PS handle to a platform services structure
 * @param Array array to reset
 * @param Capacity new array initial capacity
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status DynamicArray_Reset(IPDR_PlatformServices *PS,
                                DynamicArray *Array, int Capacity)
{
    DynamicArray_Delete(PS, Array);
    return DynamicArray_Create(PS, Array, Capacity);
}

/**
 * Reset the capacity of an array to the
 *   requested number of entities.
 *
 * @param PS handle to a platform services structure
 * @param Array array to reset
 * @param Capacity new array initial capacity
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status DynamicArray_ResetNoFree(IPDR_PlatformServices *PS,
                                DynamicArray *Array, int Capacity)
{
    DynamicArray_DeleteNoFree(PS, Array);
    return DynamicArray_Create(PS, Array, Capacity);
}

/**
 * Delete the contents of the array
 *   (by decreasing the reference count of the contained
 *    objects)
 *
 * @param PS handle to a platform services structure
 * @param Array array to delete
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status DynamicArray_Delete(IPDR_PlatformServices *PS,
                                 DynamicArray *Array)
{
    int i;

    if (Array->Data == NULL)
        return IPDR_OK;

    /* We free the array from high to low because it is possible
       that another destructor will want to read through all these
       elements. (This shouldn't happen; but it doesn't hurt.) */

    for (i = Array->Length - 1; i >= 0; i--) {
        Array->Length--; /* in case anybody else tries to look at this array */
        if (Array->Data[i] != NULL) {
            Object_Release(PS, Array->Data[i]);
        }
    }

    Platform_MemoryFree(PS, Array->Data);

    Array->Length = 0;
    Array->Data = NULL;

    return IPDR_OK;
}

/**
 * Delete the contents of the array
 *   (by decreasing the reference count of the contained
 *    objects)
 *
 * @param PS handle to a platform services structure
 * @param Array array to delete
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status DynamicArray_DeleteNoFree(IPDR_PlatformServices *PS,
                                 DynamicArray *Array)
{
    if (Array->Data == NULL)
        return IPDR_OK;

    Platform_MemoryFree(PS, Array->Data);

    Array->Length = 0;
    Array->Data = NULL;

    return IPDR_OK;
}

/**
 * Remove an entity from the array.
 *   The function deletes the contents of the array
 *   (by decreasing the reference count of the contained
 *    objects and placing NULL in the given position)
 *
 * @param PS handle to a platform services structure
 * @param Array array to delete element from
 * @param Index index of an element to remove
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status DynamicArray_Remove(IPDR_PlatformServices *PS,
                                 DynamicArray *Array, int Index)
{
	void *pObj = NULL;

	if (Index >= Array->Length || Index < 0)
		return IPDR_OK;

    if (Array->Data[Index] != NULL) {
		pObj = Array->Data[Index];
        Array->Data[Index] = NULL;
        Object_Release(PS, pObj);
    }
    return IPDR_OK;
}

/**
 * Remove an entity from the array.
 *   The function deletes the contents of the array
 *   (by decreasing the reference count of the contained
 *    objects and placing NULL in the given position)
 *
 * @param PS handle to a platform services structure
 * @param Array array to delete element from
 * @param Index index of an element to remove
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status DynamicArray_RemoveNoFree(IPDR_PlatformServices *PS,
                                 DynamicArray *Array, int Index)
{
    if (Array->Data[Index] != NULL) {
        Array->Data[Index] = NULL;
    }
    return IPDR_OK;
}

/**
 * Remove an entity from the array.
 *   The function deletes the contents of the array
 *   (by decreasing the reference count of the contained
 *    objects and shifting the array backwards).
 *
 * @param PS handle to a platform services structure
 * @param Array array to delete element from
 * @param Index index of an element to remove
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status DynamicArray_RemoveAndShift(IPDR_PlatformServices *PS,
                                         DynamicArray *Array,
                                         int Index)
{
    int i;
	void *pObj = NULL;
	if (Index >= Array->Length || Index < 0)
		return IPDR_OK;

    if (Array->Data[Index] != NULL) {
		pObj = Array->Data[Index];
    	Array->Data[Index] = NULL;
		Object_Release(PS, pObj);
	}

    for (i = Index; i < Array->Length-1; i++)
        Array->Data[i] = Array->Data[i+1];

    Array->Data[i] = NULL;
    (Array->Length)--;

    return IPDR_OK;
}

/**
 * Remove an entity from the array.
 *   The function deletes the contents of the array
 *   (by decreasing the reference count of the contained
 *    objects and shifting the array backwards).
 *
 * @param PS handle to a platform services structure
 * @param Array array to delete element from
 * @param Index index of an element to remove
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status DynamicArray_RemoveAndShiftNoFree(IPDR_PlatformServices *PS,
                                         DynamicArray *Array,
                                         int Index)
{
    int i;
	if (Index >= Array->Length || Index < 0)
		return IPDR_OK;

    for (i = Index; i < Array->Length-1; i++)
        Array->Data[i] = Array->Data[i+1];

    Array->Data[i] = NULL;
    (Array->Length)--;

    return IPDR_OK;
}

/**
 * Append an object to the end of an array
 *   (the array will be resized if needed).
 *
 * @param PS handle to a platform services structure
 * @param Array array to delete element from
 * @param Object object to add to array
 * @param Index index of append object (output)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
#ifdef DEBUG_MEMORY
IPDR_Status DynamicArray_dm_AppendElement(IPDR_PlatformServices *PS,
                                           DynamicArray *Array,
                                           void *Object, int *Index,
                                           const char* FileName, int LineNum)
#else
    IPDR_Status DynamicArray_AppendElement(IPDR_PlatformServices *PS,
                                            DynamicArray *Array,
                                            void *Object, int *Index)
#endif
{
    int i;
    void **NewData;

    if (Array->Length == Array->Capacity) {
#ifdef DEBUG_MEMORY
        IPDR_Status Status =
            Platform_dm_MEMORYALLOCATEVEC(PS, Array->Capacity + DEFAULT_CAPACITY_INCREMENT, NewData,
                                          FileName, LineNum);
        if (FAILURE(Status)) {
            X_MEM_PANICVEC(Array->Capacity + DEFAULT_CAPACITY_INCREMENT, NewData, FileName, LineNum);
            return Status;
        }
#else
        Platform_Check_MEMORYALLOCATEVEC(PS,
                                         Array->Capacity + DEFAULT_CAPACITY_INCREMENT,
                                         NewData, {});
#endif

        for (i = 0; i < Array->Capacity; i++)
            NewData[i] = Array->Data[i];

        Platform_MemoryFree(PS, Array->Data);

        Array->Data = NewData;
        Array->Capacity += DEFAULT_CAPACITY_INCREMENT;
    }

    Array->Data[Array->Length] = Object;
    *Index = Array->Length;
    (Array->Length)++;

    return IPDR_OK;
}

/**
 * Add an object to an array (if a free
 *   location is found in the middle of the array it will be
 *   used).
 *
 * @param PS handle to a platform services structure
 * @param Array array to delete element from
 * @param Object object to add to array
 * @param Index index of append object (output)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status DynamicArray_AddElement(IPDR_PlatformServices *PS,
                                     DynamicArray *Array,
                                     void *Object, int *Index)
{
    int i;

    /* attempt to place in the middle the object in a free position */

    for (i = 0; i < Array->Length; i++)
        if (Array->Data[i] == NULL) {
            Array->Data[i] = Object;
            *Index = i;
            return IPDR_OK;
        }

    /* append to the end of the array */

    return DynamicArray_AppendElement(PS, Array, Object, Index);
}

/**
 * Set a specific element in an array to
 *   the requested object (the array is resized if needed).
 *
 * @param PS handle to a platform services structure
 * @param Array array to delete element from
 * @param Index index of append object
 * @param Object object to add to array
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status DynamicArray_SetElementAt(IPDR_PlatformServices *PS,
                                       DynamicArray *Array,
                                       int Index,
                                       void *Object)
{
    int i;
    void **NewData;

    if (Index >= Array->Capacity) {
        Platform_Check_MEMORYALLOCATEVEC(PS,
                                         Index + DEFAULT_CAPACITY_INCREMENT,
                                         NewData, {});
        for (i = 0; i < Array->Capacity; i++)
            NewData[i] = Array->Data[i];
        for ( ; i < Index + DEFAULT_CAPACITY_INCREMENT; i++)
            NewData[i] = NULL;

        Platform_MemoryFree(PS, Array->Data);

        Array->Data = NewData;
        Array->Capacity = Index + DEFAULT_CAPACITY_INCREMENT;
    }

    Array->Data[Index] = Object;

    if (Index >= Array->Length)
        Array->Length = Index + 1;

    return IPDR_OK;
}

/**
 * Copy the contents of an array to another
 *   array (the reference count of the array contents is
 *   increased).
 *
 * @param PS handle to a platform services structure
 * @param Source source array
 * @param Target target array
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status DynamicArray_Copy(IPDR_PlatformServices *PS,
                               DynamicArray *Source,
                               DynamicArray *Target)
{
    int i;
    IPDR_Status Status = DynamicArray_Reset(PS, Target, Source->Length);

    if (FAILURE(Status))
        return Status;

    for (i = 0; i < Source->Length; i++) {
        Target->Data[i] = Source->Data[i];
        Object_Duplicate(PS, Target->Data[i]);
    }

    Target->Length = Source->Length;

    return IPDR_OK;
}

/**
 * Count the number of non empty entries
 *   in the array.
 *
 * @param PS handle to a platform services structure
 * @param Array array to count objects of
 * @param N number of objects (output)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
IPDR_Status DynamicArray_CountObjects(IPDR_PlatformServices *PS,
                                       DynamicArray *Array,
                                       unsigned int *N)
{
    int i;

    *N = 0;
    for (i = 0; i < Array->Length; i++)
        if (Array->Data[i] != NULL)
            (*N)++;

    return IPDR_OK;
}
