/** @file Common/MemoryManagement.c
 *
 * Implementation of the IPDR memory management mechanisms.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: MemoryManagement.c,v 1.13 2005/05/09 13:30:01 dmitryk Exp $
 *
 * Provides reference counted objects, somewhat in the style of
 * C++. Each such object class has its own destructors.
 *
 * @note There is a dependency between this code and
 * Platforms/Generic/GenericPlatform.c, in the layout of object
 * headers.
 */

#include <IPDR/Config.h>
const char MemoryManagement_rcsid[] = "@(#) $Id: MemoryManagement.c,v 1.13 2005/05/09 13:30:01 dmitryk Exp $" IPDR_COPYRIGHT_STR;

#include <Common/MemoryManagement.h>

const char MemoryManagement_h_rcsid[] = MEMORY_MANAGEMENT_H_RCSID;

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include <Common/Log.h>
#include <Common/CommonMsgs.h>

#include <Common/Platform.h>

#if defined(TRACE_OBJECTS) || defined(DEBUG_MODE)
static const char* Object_What(const ObjectHeader* OH);
#endif
struct XXhdr { ObjectHeader oh; int pastHdr; };



/**
 * Allocate a reference counted object with
 *   the requested size.
 *
 * @param PS handle to a platform services structure
 * @param Size size of object in bytes.
 * @param Destructor pointer to a function which will be called
 *                once the object's reference count reaches
 *                zero.
 * @param Object pointer to newly created object (output)
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 *
 */

#ifdef DEBUG_MEMORY
IPDR_Status Object_dm_Allocate(
#else
    IPDR_Status Object_Allocate(
#endif
        IPDR_PlatformServices *PS, size_t Size,
        void (*Destructor)(IPDR_PlatformServices *PS, void *),
        void **Object
#ifdef DEBUG_MEMORY
        , const char* FileName, int LineNum
#endif
        )
{
    ObjectHeader *OH;
#ifdef DEBUG_MEMORY
    IPDR_Status Status =
        Platform_dm_MEMORYALLOCATESIZEWITHHDR(PS, sizeof (ObjectHeader), Size, OH, FileName, LineNum);
    if (FAILURE(Status)) {
        X_MEMPANICSIZE(sizeof (ObjectHeader) + Size, FileName, LineNum);
        return Status;
    }
#else
    Platform_Check_MEMORYALLOCATESIZEWITHHDR(PS,
                                             sizeof (ObjectHeader), Size,
                                             OH, {});
#endif

    { /* The following ought to be optimized away by the compiler,
         unless the check fails. */
        if (offsetof(struct XXhdr, pastHdr) != sizeof (ObjectHeader)) {
#ifdef STD_FILE_OPERATIONS_SUPPORT
            fprintf(stderr, "\n*** ObjectHeader size invalid: %u should be %u ***\n",
                    sizeof (ObjectHeader), offsetof(struct XXhdr, pastHdr));
#endif
            abort();
        }
    }
	/* Dmitryk fixing memory bug !!!
    OH = (ObjectHeader*)((char*)OH - sizeof (ObjectHeader));
	*/
    OH->ReferenceCount = 1;
    OH->Destructor = Destructor;
    /* It already has the size filled in, so we don't neeed to do this:*/
	/* Dmitryk fixing memory bug !!! */
    OH->size = Size + sizeof (ObjectHeader);
    /**/
#ifdef DEBUG_MODE
    OH->eyecatcher = 0xcefacefa;
#endif

    *Object = (void *) ((char *) OH + sizeof(ObjectHeader));
    memset(*Object, 0, Size);

#ifdef TRACE_OBJECTS
    fprintf(stderr, "Object_Allocate %p (%.99s)\n", ((char*)OH)-8, Object_What(OH));
#endif

    return IPDR_OK;
}

/* Debug function: figure out what kind of object we have */

#if defined(TRACE_OBJECTS) || defined(DEBUG_MODE)

    static
    const char* Object_What(const ObjectHeader* OH)
{
    const char* result;

#if 1 && defined(DEBUG_MODE)
    /* These are in separate libraries, so you might need to leave them out here */
    extern void Exporter_Destructor(IPDR_PlatformServices *PS, void *Obj);
    extern void Key_Destructor(IPDR_PlatformServices *PS, void *Obj);
    extern void Message_Destructor(IPDR_PlatformServices *PS, void *Obj);
    extern void Collector_Destructor(IPDR_PlatformServices *PS, void *Obj);
	extern void CollectorSession_Destructor(IPDR_PlatformServices *PS, void *Obj);
    extern void Session_Destructor(IPDR_PlatformServices *PS, void *Obj);
    extern void Template_Destructor(IPDR_PlatformServices *PS, void *Obj);
    extern void TransmissionContext_Destructor(IPDR_PlatformServices *PS, void *Obj);
#endif

    extern void ConsoleModule_Destructor(IPDR_PlatformServices *PS, void *Obj);
    extern void ConsoleSession_Destructor(IPDR_PlatformServices *PS, void *Obj);
    extern void Module_Destructor(IPDR_PlatformServices *PS, void *Obj);
    extern void NameDef_Destructor(IPDR_PlatformServices *PS, void *Obj);

    if (! OH) {
        result = "<null>";
    } else if (! OH->Destructor) {
        result = "<unknown>";

#if 1 && defined(DEBUG_MODE)
        /* These are in separate libraries, so you might need to leave them out here */
    } else if (OH->Destructor == Exporter_Destructor) {
        result = "Exporter";
    } else if (OH->Destructor == Key_Destructor) {
        result = "Key";
    } else if (OH->Destructor == Message_Destructor) {
        result = "Message";
	} else if (OH->Destructor == Message_Destructor) {
		result = "CollectorSession";
    } else if (OH->Destructor == Collector_Destructor) {
        result = "Collector";
    } else if (OH->Destructor == Session_Destructor) {
        result = "Session";
    } else if (OH->Destructor == Template_Destructor) {
        result = "Template";
    } else if (OH->Destructor == TransmissionContext_Destructor) {
        result = "TransmissionContext";
#endif

    } else if (OH->Destructor == ConsoleModule_Destructor) {
        result = "ConsoleModule";
    } else if (OH->Destructor == ConsoleSession_Destructor) {
        result = "ConsoleSession";
    } else if (OH->Destructor == Module_Destructor) {
        result = "Module";
    } else {
        static char r[40];
        sprintf(r, "<unk:%p>", OH->Destructor);
        result = r;
    }
    return result;
}

#endif

/**
 * Delete a reference counted object
 *   (ignoring the current reference count)
 *
 * @param PS handle to a platform services structure
 * @param Object object to delete
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
#ifdef DEBUG_MEMORY
IPDR_Status Object_dm_Delete(IPDR_PlatformServices *PS, void *Object, const char* FileName, int LineNum)
#else
    IPDR_Status Object_Delete(IPDR_PlatformServices *PS, void *Object)
#endif
{
    ObjectHeader *OH;

    if (Object == NULL)
        return IPDR_OK;

    OH = (ObjectHeader *) ((char *) Object - sizeof(ObjectHeader));

#ifdef TRACE_OBJECTS
    fprintf(stderr, "Object_Delete %p (%.99s) referenceCount=%d\n", ((char*)OH)-8, Object_What(OH), OH->ReferenceCount);
#endif

#ifdef DEBUG_MODE
    if (OH->eyecatcher != 0xcefacefa) {
        printf("Object_Delete: %.99s Bad ObjectHeader(%p): %x\n",
               Object_What(Object), Object, OH->eyecatcher);
        abort();
    }
    OH->eyecatcher = 0xefbeadde;
#endif

#ifdef DEBUG_MEMORY
    return GenericPlatform_dm_MemoryFreeWithHdr(PS, sizeof (ObjectHeader), OH, FileName, LineNum);
#else
    return Platform_MemoryFreeWithHdr(PS, sizeof (ObjectHeader), OH);
#endif
}

/**
 * Increase the reference count of an
 *   object by one.
 *
 * @param PS handle to a platform services structure
 * @param Object object to increase reference count of.
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
#ifdef DEBUG_MEMORY
IPDR_Status Object_dm_Duplicate(IPDR_PlatformServices *PS, void *Object, const char* FileName, int LineNum)
#else
    IPDR_Status Object_Duplicate(IPDR_PlatformServices *PS, void *Object)
#endif
{
    ObjectHeader *OH;

    if (Object == NULL)
        return IPDR_OK;

    OH = (ObjectHeader *) ((char *) Object - sizeof(ObjectHeader));

#ifdef TRACE_OBJECTS
    fprintf(stderr, "Object_Duplicate %p (%.99s) referenceCount=%d + 1\n", ((char*)OH-8), Object_What(OH), OH->ReferenceCount);
#endif

#ifdef DEBUG_MODE
    if (OH->eyecatcher != 0xcefacefa) {
        printf("Object_Duplicate: %.99s Bad ObjectHeader(%p): %x\n",
               Object_What(Object), Object, OH->eyecatcher);
        abort();
    }
#endif

    ++(OH->ReferenceCount);

    return IPDR_OK;
}

/**
 * Decrease the reference count of an object.
 *   If the reference count reaches zero the destructor
 *   will be called and the memory allocated for the
 *   object will be freed.
 *
 * @param PS handle to a platform services structure
 * @param Object object to release
 *
 * @return #IPDR_OK on success; #IPDR_STATUS_RETURN_CODE otherwise.
 */
#ifdef DEBUG_MEMORY
IPDR_Status Object_dm_Release(IPDR_PlatformServices *PS, void *Object, const char* FileName, int LineNum)
#else
    IPDR_Status Object_Release(IPDR_PlatformServices *PS, void *Object)
#endif
{

    ObjectHeader *OH;

    if (Object == NULL)
        return IPDR_OK;

    OH = (ObjectHeader *) ((char *) Object - sizeof(ObjectHeader));

#ifdef TRACE_OBJECTS
    fprintf(stderr, "Object_Release %p (%.99s) referenceCount=%d - 1\n", ((char*)OH-8), Object_What(OH), OH->ReferenceCount);
#endif

#ifdef DEBUG_MODE
    if (OH->eyecatcher != 0xcefacefa) {
        printf("Object_Release: %.99s Bad ObjectHeader(%p): %x\n",
               Object_What(Object), Object, OH->eyecatcher);
    }
#endif

    --(OH->ReferenceCount);
    if (OH->ReferenceCount <= 0) {
        if (OH->Destructor != NULL)
            OH->Destructor(PS, Object);
#ifdef DEBUG_MEMORY
        GenericPlatform_dm_MemoryFreeWithHdr(PS, sizeof (ObjectHeader), OH, FileName, LineNum);
#else
        Platform_MemoryFreeWithHdr(PS, sizeof (ObjectHeader), OH);
#endif
    }

    return IPDR_OK;
}

/**
 *  Get the reference count of an object.
 *
 * @param PS handle to a platform services structure
 * @param Object object to get reference count of
 *
 * @return Reference count of the object
 */
int Object_GetReferenceCount(IPDR_PlatformServices *PS, void *Object)
{
    ObjectHeader *OH;

    if (Object == NULL)
        return 0;

    OH = (ObjectHeader *) ((char *) Object - sizeof(ObjectHeader));

#ifdef DEBUG_MODE
    if (OH->eyecatcher != 0xcefacefa) {
        printf("Object_GetReferenceCount: %.99s Bad ObjectHeader(%p): %x\n",
               Object_What(Object), Object, OH->eyecatcher);
    }
#endif

    return OH->ReferenceCount;
}


