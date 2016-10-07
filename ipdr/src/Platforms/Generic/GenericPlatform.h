/** @file Platforms/Generic/GenericPlatform.h
 *
 * Implementation of platform-specific services (see @ref
 * IPDR_platform), as defined in Platforms/Generic/GenericPlatform.c.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: GenericPlatform.h,v 1.12 2005/05/09 13:30:02 dmitryk Exp $
 *
 * @note There is a dependency between this code and
 * Common/MemoryManagement.h, in the layout of object headers.
 */

#ifndef _GENERIC_PLATFORM_H_

/** Macro to prevent double inclusion of this header.
 */
#define _GENERIC_PLATFORM_H_

#include <IPDR/Config.h>

/** Source code version identifier.
 */
#define GENERIC_PLATFORM_H_RCSID "@(#) $Id: GenericPlatform.h,v 1.12 2005/05/09 13:30:02 dmitryk Exp $" IPDR_COPYRIGHT_STR

#if defined(GLOBAL_RENAME)
#define GenericPlatform_h_rcsid            IPDR__GenericPlatform_h_rcsid
#define GenericPlatform_rcsid              IPDR__GenericPlatform_rcsid
#define GenericPlatform_Initialize         IPDR__GenericPlatform_Initialize
#define GenericPlatform_FormatTimeGMTNow   IPDR__GenericPlatform_FormatTimeGMTNow
#define GenericPlatform_FormatTimeLocalNow IPDR__GenericPlatform_FormatTimeLocalNow
#endif

#include <stddef.h>
#if defined(_MSC_VER)
#error Native Microsoft compiler is not supported
#include <winsock2.h>
#elif defined(VXWORKS) || defined(_VXWORKS)
#include <semLib.h>
#include <time.h>
#include <taskLib.h>
#else
/* Unix */
#include <sys/time.h>
#include <unistd.h>
#if defined(PTHREADS_SUPPORT) /* && defined(MT_SUPPORT) */
#include <pthread.h>
#endif
#endif

#include <sys/types.h>

#include <IPDR/Defs.h>
#include <IPDR/IPDR.h>

#ifdef __cplusplus
extern "C" {
#endif

#if 0
} /* put this here to un-confuse Emacs's automatic indentation */
#endif

/** A socket callback event.
 *  @see ::IPDR_RegisterReadSocket_t
 *  @see ::IPDR_RegisterWriteSocket_t
 */
typedef struct IPDR_tag_SocketEvent
{
    IPDR_SocketHandle FD; /**< The file (socket) descriptor. */
    IPDR_SocketCallback_t Callback; /**< The callback as given to ::IPDR_RegisterReadSocket_t or ::IPDR_RegisterWriteSocket_t. */
    void *Arg; /**< The context argument to ::IPDR_RegisterReadSocket_t or ::IPDR_RegisterWriteSocket_t. */

    /** The Unregistered flag is used to allow freeing "FDEvent" when it
        is no longer needed. To synchronize everything, the freeing is
        done only when the events are processed. This is marked as
        "volatile" because one thread can set it and another thread looks
        at its value.  */

    volatile IPDR_BOOL Unregistered;

    struct IPDR_tag_SocketEvent *NextEvent; /**< Next in the chain of events. */

#if defined(PTHREADS_SUPPORT)
    pthread_t SocketThread;
#elif defined(_MSC_VER)
    DWORD SocketThread;
#elif defined(VXWORKS) || defined(_VXWORKS)
    signed int SocketThread;
#else
#error Threads: not one of PTHREADS/VXWORKS/_MSC_VER
#endif
}
GenericFDEvent;

/** A structure for keeping track of timer events. The "Expired" flag
 *  is used to allow safe deletion -- when the event is fired, it is
 *  removed from the list of timer events but it is not deleted;
 *  instead, an "unregisterTimer" does the removal. Typically, a timer
 *  will be part of a structure, so the timer is finally removed when
 *  the structure is freed.
 *  @see IPDR_RegisterTimer_t
 *  @see IPDR_RegisterPeriodictTimer_t
 */
typedef struct IPDR_tag_Timer
{
    struct timeval ExpirationTime;
    int Timeout; /**<  milliseconds. */
    int Remaining; /**< milliseconds recalculated each time round in case clock is reset */
    IPDR_TimerCallback_t Callback;
    void *Arg;
    IPDR_BOOL Periodic;
    IPDR_BOOL Expired;
    struct IPDR_tag_Timer *PrevTimer;
    struct IPDR_tag_Timer *NextTimer;
}
GenericTimer;

/** Abstraction of a recursive lock.
 */
typedef struct
{
    char eyecatcher[8]; /* "LOCKRCR" */
#ifdef MT_SUPPORT
#if defined(PTHREADS_SUPPORT)
    pthread_mutex_t CountLock;
    pthread_mutex_t ActualLock;
    pthread_t Owner;
    int RecursionCount;
#elif defined(_MSC_VER)
    CRITICAL_SECTION CountLock;
    CRITICAL_SECTION ActualLock;
    DWORD Owner;
    int RecursionCount;
#elif defined(VXWORKS) || defined(_VXWORKS)
    SEM_ID ActualLock; /* This is a recursive thread: doesn't need CountLock */
#else
#error Threads: not one of PTHREADS/VXWORKS/_MSC_VER
#endif
#endif
} GenericLock;

/** A "header" that gets put ahead of stuff we allocate. This allows
 *  auditing memory allocation and also allows Purify to work, because
 *  it's the same layout as ObjectHeader (in MemoryManagement.h). So,
 *  if ObjectHeader changes, this will need to change also (you'll get
 *  a run-time error from GenerCMPlatform_MemoryAllocateWithHdr()).
 */

typedef struct
{ /* See also ObjectHeader in MemoryManagement.h */
    unsigned int size; /**< MUST be first to match layout of ObjectHeader in MemoryManagement.h */
    unsigned int eyecatcher; /**< "signature" for debugging: gets set to "a0a1a2a3" for validating pointers to GenericMemHdr (set to something else when memory is freed): see same field in ObjectHeader */
#ifdef DEBUG_MODE
	/* memory collector */
	void *next;
	void *prev;
	/* memory collector */
#endif
}
GenericMemHdr;

/** Information needed by the platform.
 */
typedef struct IPDR_tag_PlatformArg
{
    char eyecatcher[8]; /**< "IPDRPS" */
    GenericTimer *Timers; /**< list of currently active timers */
    volatile IPDR_BOOL Enabled; /**< true while the MainFunc is running */
    volatile IPDR_BOOL Stopped; /**< set by MainFunc when it finishes (it has seen that ! Enabled) */

    GenericLock *PlatformLock;

#ifdef MT_SUPPORT
#if defined(PTHREADS_SUPPORT)
    pthread_t MainFuncThread;
#elif defined(_MSC_VER)
    DWORD MainFuncThread;
#elif defined(VXWORKS)
    int MainFuncThread;
#else
#error Threads: not one of PTHREADS/VXWORKS/_MSC_VER
#endif
#endif

    GenericFDEvent *FDReadEvents;  /**< linked list of read FD events */
    GenericFDEvent *FDWriteEvents; /**< linked list of write FD events */
    volatile IPDR_BOOL ScanningFDs; /**< communicate between UnregisterSocket and main FD event loop */

    fd_set ReadFDs;
    fd_set WriteFDs;
    IPDR_SocketHandle MaxFD;

    IPDR_BOOL ExporterHandleRegistered; /**< true if \a ExporterHandle has a meaningful value. */
    IPDR_ExporterHandle ExporterHandle; /**< Exporter handle, so we can output messages */

    unsigned numAllocs; /**< Number of memory allocations. */
    unsigned numFrees; /**< Number of memory frees. */
	unsigned numAllocated; /**< Number of memory frees. */
    size_t   bytesAllocated; /**< Incremented/decremented with alloc/free, not includeing this structure */
#ifdef DEBUG_MODE
	/* memory collector */
	GenericMemHdr *memCollectorHead;
	GenericMemHdr *memCollectorTail;
	/* memory collector */
#endif
}
GenericST;


/* -------- functions --------- */

/** non-multitasking version of initialize
 *  which creates a thread if MT_SUPPORT */
IPDR_Status GenericPlatform_Initialize(IPDR_PlatformServices *PS);

#if defined(SNMP_SUPPORT)
/*
*	XX    XX	XXXX	XXXXXX
*	XXXXXXXX	 XX 	XX   XX
*	XX XX XX	 XX 	XXXXXXX
*	XX    XX	 XX 	XX    XX
*	XX    XX	XXXX	XXXXXXX
*/

typedef struct IPDR_tag_ExporterMIB{
	pthread_t				MainFuncThread;

	volatile	IPDR_BOOL	Running; /**< true while the MainFunc is running */

	IPDR_ExporterHandle		ExporterHandle;
}GenericMIB;
#endif

#ifdef __cplusplus
}
#endif

#endif
