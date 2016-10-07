/** @file Platforms/Generic/GenericPlatform.c
 *
 * The generic implementation of the platform abstraction layer. This
 * contains code for Solaris, Linux, and VxWorks. There is some code
 * for Windows, but it is obsolete and not to be trusted.
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: GenericPlatform.c,v 1.30 2005/05/11 12:38:34 dmitryk Exp $
 *
 * @note There is a dependency between this code and
 * Common/MemoryManagement.h (and Common/MemoryManagement.h), in the
 * layout of object headers.
 */

#include <IPDR/Config.h>
const char GenericPlatform_rcsid[] = "@(#) $Id: GenericPlatform.c,v 1.30 2005/05/11 12:38:34 dmitryk Exp $" IPDR_COPYRIGHT_STR;

#include <Platforms/Generic/GenericPlatform.h>

#include <Common/Platform.h>

const char GenericPlatform_h_rcsid[] = GENERIC_PLATFORM_H_RCSID;
const char Platform_h_rcsid[] = PLATFORM_H_RCSID;

/*#define TRACE_MAINFUNC*/


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include "Common/Transport.h" /* for PrintAddressAndPort */
#include <unistd.h> /* for gethostname */

#include "Common/Protocol.h" /* for the protocol field types whose sizes are checked here */

#define CAST_VOID(Type, P) ((Type)(P))

#if defined(SOLARIS)
/* Kludge: For GNU compiler, to get localtime_r */
/*         For some reason, these declarations are ifdef-ed out */
extern struct tm *gmtime_r(const time_t *, struct tm *);
extern struct tm *localtime_r(const time_t *, struct tm *);
#endif

/* A macro for a "panic" in a few places which indicate a severe logic
 * error. */

#if defined(VXWORKS) || defined(_VXWORKS)
#define ABORT_CALL taskSuspend(0)
#else
#define ABORT_CALL abort()
#endif

#if ! defined(STD_FILE_OPERATIONS_SUPPORT)
#error Need to modify LOG_WRITE macros if no std file support
#endif

/* string-ize macros */
#define Str(x) #x
#define Xstr(x) Str(x)

#define LOG_WRITE(e, lvl, msg) {                                                \
  if ((e) && (e)->ExporterHandleRegistered) {                                     \
    IPDR_LogWrite((e)->ExporterHandle, "Platform", lvl,                          \
                   __FILE__ ":" Xstr(__LINE__), msg);                           \
  } else {                                                                      \
    char b[IPDR_TIME_FORMAT_LENGTH+1];                                         \
    fprintf(stderr, "*** %.49s: " msg " (" __FILE__ ":" Xstr(__LINE__) ")\n",   \
            GenericPlatform_FormatTimeGMTNow((e), b));                          \
    fflush(stderr);                                                             \
  }                                                                             \
}
#define LOG_WRITE1(e, lvl, msg, a1) {                                           \
  if ((e) && (e)->ExporterHandleRegistered) {                                     \
    IPDR_LogWrite((e)->ExporterHandle, "Platform", lvl,                          \
                   __FILE__ ":" Xstr(__LINE__), msg, a1);                       \
  } else {                                                                      \
    char b[IPDR_TIME_FORMAT_LENGTH+1];                                         \
    fprintf(stderr, "*** %.49s: " msg " (" __FILE__ ":" Xstr(__LINE__) ")\n",   \
            GenericPlatform_FormatTimeGMTNow((e), b),                           \
            a1);                                                                \
    fflush(stderr);                                                             \
  }                                                                             \
}
#define LOG_WRITE2(e, lvl, msg, a1,a2) {                                        \
  if ((e) && (e)->ExporterHandleRegistered) {                                     \
    IPDR_LogWrite((e)->ExporterHandle, "Platform", lvl,                          \
                   __FILE__ ":" Xstr(__LINE__), msg, a1,a2);                    \
  } else {                                                                      \
    char b[IPDR_TIME_FORMAT_LENGTH+1];                                         \
    fprintf(stderr, "*** %.49s: " msg " (" __FILE__ ":" Xstr(__LINE__) ")\n",   \
            GenericPlatform_FormatTimeGMTNow((e), b),                           \
           a1,a2);                                                              \
    fflush(stderr);                                                             \
  }                                                                             \
}
#define LOG_WRITE3(e, lvl, msg, a1,a2,a3) {                                     \
  if ((e) && (e)->ExporterHandleRegistered) {                                     \
    IPDR_LogWrite((e)->ExporterHandle, "Platform", lvl,                          \
                   __FILE__ ":" Xstr(__LINE__), msg, a1,a2,a3);                 \
  } else {                                                                      \
    char b[IPDR_TIME_FORMAT_LENGTH+1];                                         \
    fprintf(stderr, "*** %.49s: " msg " (" __FILE__ ":" Xstr(__LINE__) ")\n",   \
            GenericPlatform_FormatTimeGMTNow((e), b),                           \
            a1,a2,a3);                                                          \
    fflush(stderr);                                                             \
  }                                                                             \
}
#define LOG_WRITE4(e, lvl, msg, a1,a2,a3,a4) {                                  \
  if ((e) && (e)->ExporterHandleRegistered) {                                     \
    IPDR_LogWrite((e)->ExporterHandle, "Platform", lvl,                          \
                   __FILE__ ":" Xstr(__LINE__), msg, a1,a2,a3,a4);              \
  } else {                                                                      \
    char b[IPDR_TIME_FORMAT_LENGTH+1];                                         \
    fprintf(stderr, "*** %.49s: " msg " (" __FILE__ ":" Xstr(__LINE__) ")\n",   \
            GenericPlatform_FormatTimeGMTNow((e), b),                           \
            a1,a2,a3,a4);                                                       \
    fflush(stderr);                                                             \
  }                                                                             \
}
#define LOG_WRITE5(e, lvl, msg, a1,a2,a3,a4,a5) {                               \
  if ((e) && (e)->ExporterHandleRegistered) {                                     \
    IPDR_LogWrite((e)->ExporterHandle, "Platform", lvl,                          \
                   __FILE__ ":" Xstr(__LINE__), msg, a1,a2,a3,a4,a5);           \
  } else {                                                                      \
    char b[IPDR_TIME_FORMAT_LENGTH+1];                                         \
    fprintf(stderr, "*** %.49s: " msg " (" __FILE__ ":" Xstr(__LINE__) ")\n",   \
            GenericPlatform_FormatTimeGMTNow((e), b),                           \
            a1,a2,a3,a4,a5);                                                    \
    fflush(stderr);                                                             \
  }                                                                             \
}
#define LOG_WRITE6(e, lvl, msg, a1,a2,a3,a4,a5,a6) {                            \
  if ((e) && (e)->ExporterHandleRegistered) {                                     \
    IPDR_LogWrite((e)->ExporterHandle, "Platform", lvl,                          \
                   __FILE__ ":" Xstr(__LINE__), msg, a1,a2,a3,a4,a5,a6);        \
  } else {                                                                      \
    char b[IPDR_TIME_FORMAT_LENGTH+1];                                         \
    fprintf(stderr, "*** %.49s: " msg " (" __FILE__ ":" Xstr(__LINE__) ")\n",   \
            GenericPlatform_FormatTimeGMTNow((e), b),                           \
            a1,a2,a3,a4,a5,a6);                                                 \
    fflush(stderr);                                                             \
  }                                                                             \
}

#if defined(VXWORKS) || defined(_VXWORKS)
#include <taskLib.h>
#include <sockLib.h>
#include <ioLib.h>
#include <inetLib.h>
#include <socket.h>
#include <netinet/tcp.h>
#include <errnoLib.h>
#include <signal.h>
#elif defined(_MSC_VER)
#error Microsoft C++ is not currently supported
/* There is some Microsoft C++ code in this file; but it is old and
 * probably doesn't work. */
#else
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#endif

/* Some systems have EWOULDBLOCK defined. Our code assumes that it's
 * the same as EAGAIN ... if not, give an error. */

#if ! defined(EWOULDBLOCK)
#define EWOULDBLOCK EAGAIN
#endif
#if defined(VXWORKS) || defined(_VXWORKS)
/* VxWorks: EAGAIN is for processes: use EWOULDBLOCK only */
#else
#if defined(EAGAIN) && defined(EWOULDBLOCK)
#if EAGAIN != EWOULDBLOCK
#error EAGAIN and EWOULDBLOCK not the same
#endif
#endif
#endif

#include <IPDR/IPDR.h>

#include <stdlib.h>

#ifdef DEBUG_MEMORY
#include "dmalloc.h"
#endif

#define MAX_SELECT_ITERATIONS    128 /* Up to this many calls to select() before checking for timer events. */
#define RECEIVE_TIMEOUT         5000
#define MAX_ERROR_STRING_LENGTH 1024 /* for Windows only */
#define MAX_TIMEWAIT            500000 /* 0.5 sec - see comments where it's used */
#define TIME_SKEW_DELTA       10000 /* 10 sec (in milliseconds) - see comments where it's used */

#if defined(VXWORKS) || defined(_VXWORKS)
#define CONST_CAST_CHARPTR(B) ((char*)(B)) /* for broken definition of sendto() etc. */
#define CAST_CHARVOIDPTR(B)   ((char*)(B)) /* for broken definition of getsockopt */
#else
#define CONST_CAST_CHARPTR(B) (B)
#define CAST_CHARVOIDPTR(B)   ((void*)(B))
#endif

static
struct timeval ZeroTV =
{
    0, 0
};

#if defined(PTHREADS_SUPPORT)
#define THREAD_SELF() pthread_self()
#define THREAD_EQ(a,b) pthread_equal(a, b)
#elif defined(_MSC_VER)
#define THREAD_SELF() GetCurrentThreadId()
#define THREAD_EQ(a,b) (a == b)
#elif defined(VXWORKS) || defined(_VXWORKS)
#define THREAD_SELF() taskIdSelf()
#define THREAD_EQ(a,b) (a == b)
#else
#error threads: not POSIX, MSC, or VxWorks
#endif

#if defined(_MSC_VER)
#define ERRNO_EWOULDBLOCK (WSAGetLastError() == WSAETIMEDOUT)
#else
#define ERRNO_EWOULDBLOCK (errno == EWOULDBLOCK)
#endif

#ifdef DEBUG_MODE
	/* memory collector */
static unsigned int GenericPlatform_MemoryCount(IPDR_PlatformArg E);
static unsigned int GenericPlatform_MemoryDump(IPDR_PlatformArg E, FILE* stream);
static unsigned int GenericPlatform_MemoryDeallocate(IPDR_PlatformArg E);
	/* memory collector */
#endif

static IPDR_Status GenericPlatform_InitializeInternal(IPDR_PlatformServices *PS);
static void *GenericPlatform_MainFunc(void *P);
static IPDR_Status GenericPlatform_CreateLock(IPDR_PlatformArg P, void **LockPtr);
static IPDR_Status GenericPlatform_DeleteLock(IPDR_PlatformArg P, void *LockPtr);
static IPDR_Status GenericPlatform_Lock(IPDR_PlatformArg P, void *LockPtr);
static IPDR_Status GenericPlatform_Unlock(IPDR_PlatformArg P, void *LockPtr);
static IPDR_Status GenericPlatform_LockPlatform(IPDR_PlatformArg P);
static IPDR_Status GenericPlatform_UnlockPlatform(IPDR_PlatformArg P);
static IPDR_Status GenericPlatform_SleepMS(IPDR_PlatformArg P, long interval);
static IPDR_Status GenericPlatform_MemoryFree(IPDR_PlatformArg E, void *Buf);
static char* GenericPlatform_FormatTimeSyslogNow(IPDR_PlatformArg P, char* buf);
static char* GenericPlatform_FormatTimeLocalNow(IPDR_PlatformArg P, char* buf);
static char* GenericPlatform_FormatTimeLocal(IPDR_PlatformArg P, IPDR_Time c, char* buf);
static char* GenericPlatform_FormatTimeGMTNow(IPDR_PlatformArg P, char* buf);
static char* GenericPlatform_FormatTimeGMT(IPDR_PlatformArg P, IPDR_Time c, char* buf);
static void GenericPlatform_GetTV(struct timeval *TV);
static void BlockSigPipe(GenericST *E);

#ifdef DEBUG_LOCKS
#define GenericPlatform_Lock(S, L) \
    GenericPlatform_db_Lock(S, L, __FILE__, __LINE__)
#define GenericPlatform_Unlock(S, L) \
    GenericPlatform_db_Unlock(S, L, __FILE__, __LINE__)

int GenericPlatform_db_LockPlatform(IPDR_PlatformArg S, const char*, int);
int GenericPlatform_db_UnlockPlatform(IPDR_PlatformArg S, const char*, int);
int GenericPlatform_db_Lock(IPDR_PlatformArg S, GenericLock *LockPtr, const char*, int);
int GenericPlatform_db_Unlock(IPDR_PlatformArg S, GenericLock *LockPtr, const char*, int);
#endif

#if defined(SNMP_SUPPORT)
extern void *ExporterMIB_MIBMainFunc(void *P);
IPDR_Status GenericPlatform_MIB_Start() ;
static void GenericPlatform_MIB_SetExporterHandle(IPDR_ExporterHandle EH);
static void GenericPlatform_MIB_UnSetExporterHandle();
static IPDR_BOOL GenericPlatform_MIB_IsRunning();
static void GenericPlatform_MIB_Shop() ;
static GenericMIB GenericPlatform_MIBHandle = {0, 0, NULL};

IPDR_Status
GenericPlatform_MIB_Start()
{
	if (GenericPlatform_MIBHandle.Running)
        return IPDR_OK;

    GenericPlatform_MIBHandle.ExporterHandle = NULL ;
#if defined(MT_SUPPORT)
#if defined(PTHREADS_SUPPORT)
	pthread_attr_t attr;
	/*size_t stackSize = 16*1024;*/
	/* Following commented out because not properly implemented on Solaris */
	/* if (stackSize < PTHREAD_STACK_MIN) { stackSize = PTHREAD_STACK_MIN; } */
	if (pthread_attr_init(&attr) != 0 ||
		/* stack size must be checked careful to ensure it's big enough to
			not cause problems with JNI's AttachCurrentThread */
		/* pthread_attr_setstacksize(&attr, stackSize) != 0 || */
		pthread_create(& GenericPlatform_MIBHandle.MainFuncThread, &attr, ExporterMIB_MIBMainFunc, (void *) &GenericPlatform_MIBHandle) != 0) {
		(void)pthread_attr_destroy(&attr);
		return IPDR_ERR;
	}
	(void)pthread_attr_destroy(&attr);
#elif defined(_MSC_VER)
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) ExporterMIB_MIBMainFunc,
                 &GenericPlatform_MIBHandle, 0, & GenericPlatform_MIBHandle.MainFuncThread);
#elif defined(VXWORKS) || defined(_VXWORKS)
    /* taskSpawn arguments:
       char * name,      // name of new task (stored at pStackBase)
       int priority,     // priority of new task
       int options,      // task option word
       int stackSize,    // size (bytes) of stack needed plus name
       FUNCPTR entryPt,  // entry point of new task
       int arg1,         // 1st of 10 req'd args to pass to func
       int iarg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10
    */
    GenericPlatform_MIBHandle.MainFuncThread =
        taskSpawn("tExporterMIB",97,0,16*1024,(FUNCPTR)ExporterMIB_MIBMainFunc,(int)&GenericPlatform_MIBHandle
		,0,0,0,0,0,0,0,0,0);
#else
#error MT_SUPPORT: did not start GenericPlatform_MainFunc
#endif
#else /* not MT_SUPPORT */
#error You really should set MT_SUPPORT, unless you are running on MS-DOG
    ExporterMIB_MIBMainFunc(&GenericPlatform_MIBHandle);
#endif
	return IPDR_OK;
}

void
GenericPlatform_MIB_Shop()
{
	if (! GenericPlatform_MIBHandle.Running )
		return ;

	GenericPlatform_MIBHandle.Running = 0 ;
	GenericPlatform_MIBHandle.ExporterHandle = NULL ;

#if defined(MT_SUPPORT)
    /* There's a question of whether we should do pthread_detach() or
       pthread_join(). The problem with pthread_join() is that it could
       block forever if there's anything inside MainFunc that can
       block. Also, the notion of join isn't supported on all platforms
       (e.g., VxWorks). So, let's do a detach. This means that
       GenericPlatform_Stop() will return almost immediately and that
       we have to be careful not to rip the memory away. So, we go into
       a little loop, waiting for the "Stopped" flag to be set. */

    errno = 0;
    if (! THREAD_EQ(THREAD_SELF(), GenericPlatform_MIBHandle.MainFuncThread)) {
#if defined(PTHREADS_SUPPORT)
        pthread_detach(GenericPlatform_MIBHandle.MainFuncThread);
#elif defined(VXWORKS) || defined(_VXWORKS)
        /* Not needed for VxWorks */
#else
#error thread detach/join not defined
#endif
    }
#endif
}

void
GenericPlatform_MIB_SetExporterHandle(IPDR_ExporterHandle EH)
{
	GenericPlatform_MIBHandle.ExporterHandle = EH ;
}

void
GenericPlatform_MIB_UnSetExporterHandle()
{
	GenericPlatform_MIBHandle.ExporterHandle = NULL ;
}

IPDR_BOOL
GenericPlatform_MIB_IsRunning()
{
	return GenericPlatform_MIBHandle.Running ? IPDR_TRUE : IPDR_FALSE;
}
#endif
/* ---------------------- separate platform functions ------------------------- */

static
IPDR_Status GenericPlatform_Start(IPDR_PlatformArg E)
{
    /* if the platform has already started, don't do anything */
    if (E->Enabled)
        return IPDR_OK;
    E->Enabled = IPDR_TRUE;

#if defined(MT_SUPPORT)
#if defined(PTHREADS_SUPPORT)
    {
        pthread_attr_t attr;
        /*size_t stackSize = 16*1024;*/
        /* Following commented out because not properly implemented on Solaris */
        /* if (stackSize < PTHREAD_STACK_MIN) { stackSize = PTHREAD_STACK_MIN; } */
        if (pthread_attr_init(&attr) != 0)
		{
			return IPDR_ERR;
		}
            /* stack size must be checked careful to ensure it's big enough to
               not cause problems with JNI's AttachCurrentThread */
            /* pthread_attr_setstacksize(&attr, stackSize) != 0 || */
        if (pthread_create(& E->MainFuncThread, &attr, GenericPlatform_MainFunc, (void *) E) != 0)
		{
            (void)pthread_attr_destroy(&attr);
            return IPDR_ERR;
        }
        (void)pthread_attr_destroy(&attr);
    }
#elif defined(_MSC_VER)
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) GenericPlatform_MainFunc,
                 E, 0, & E->MainFuncThread);
#elif defined(VXWORKS) || defined(_VXWORKS)
    /* taskSpawn arguments:
       char * name,      // name of new task (stored at pStackBase)
       int priority,     // priority of new task
       int options,      // task option word
       int stackSize,    // size (bytes) of stack needed plus name
       FUNCPTR entryPt,  // entry point of new task
       int arg1,         // 1st of 10 req'd args to pass to func
       int iarg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10
    */
    E->MainFuncThread =
        taskSpawn("tSocketListener",97,0,16*1024,(FUNCPTR)GenericPlatform_MainFunc,(int)E,0,0,0,0,0,0,0,0,0);
#else
#error MT_SUPPORT: did not start GenericPlatform_MainFunc
#endif
#else /* not MT_SUPPORT */
#error You really should set MT_SUPPORT, unless you are running on MS-DOG
    GenericPlatform_MainFunc(P);
#endif

    return IPDR_OK;
}

/* NOTE: _Stop must *not* be called with a lock held or else deadlock
 * will result. */

static
IPDR_Status GenericPlatform_Stop(IPDR_PlatformArg E)
{
    if (! E->Enabled)
        return IPDR_OK;

    E->Enabled = IPDR_FALSE;

#ifdef TRACE_MAINFUNC
    fprintf(stderr, "+++ finished setting MainFunc.Enabled = false\n"); fflush(stderr);
#endif

#if defined(MT_SUPPORT)
    /* There's a question of whether we should do pthread_detach() or
       pthread_join(). The problem with pthread_join() is that it could
       block forever if there's anything inside MainFunc that can
       block. Also, the notion of join isn't supported on all platforms
       (e.g., VxWorks). So, let's do a detach. This means that
       GenericPlatform_Stop() will return almost immediately and that
       we have to be careful not to rip the memory away. So, we go into
       a little loop, waiting for the "Stopped" flag to be set. */

    errno = 0;
    if (! THREAD_EQ(THREAD_SELF(), E->MainFuncThread)) {
#ifdef TRACE_MAINFUNC
        fprintf(stderr, "+++ joining MainFunc self=%p and main=%p\n", THREAD_SELF(), E->MainFuncThread); fflush(stderr);
#endif
#if defined(PTHREADS_SUPPORT)
        pthread_detach(E->MainFuncThread);
#elif defined(VXWORKS) || defined(_VXWORKS)
        /* Not needed for VxWorks */
#else
#error thread detach/join not defined
#endif
#ifdef TRACE_MAINFUNC
        fprintf(stderr, "+++ joined MainFunc\n"); fflush(stderr);
#endif

        while (E && (! E->Stopped)) {
#ifdef TRACE_MAINFUNC
            fprintf(stderr, "_Stop loop enabled=%d, stopped=%d\n",
                    E->Enabled, E->Stopped); fflush(stderr);
#endif
            GenericPlatform_SleepMS(E, 100);
        }
    }
#endif
#ifdef TRACE_MAINFUNC
	fprintf(stderr, "+++ out MainFunc\n"); fflush(stderr);
#endif
    return IPDR_OK;
}

static
IPDR_BOOL GenericPlatform_IsRunning(IPDR_PlatformArg E)
{
    return (E->Enabled && ! E->Stopped) ? IPDR_TRUE : IPDR_FALSE;
}

static
IPDR_Status GenericPlatform_Delete(IPDR_PlatformArg E)
{
    GenericFDEvent *UFE, *NextUFE;

    if (E == NULL) {
        return IPDR_OK;
    }

#ifdef TRACE_MAINFUNC
    fprintf(stderr, "*** GenericPlatformDelete\n");
#endif

    /* delete the FD events associated with dispatcher once we exit */

    UFE = E->FDReadEvents;
    while (UFE != NULL) {
        NextUFE = UFE->NextEvent;
        GenericPlatform_MemoryFree(E, UFE);
        UFE = NextUFE;
    }

    UFE = E->FDWriteEvents;
    while (UFE != NULL) {
        NextUFE = UFE->NextEvent;
        GenericPlatform_MemoryFree(E, UFE);
        UFE = NextUFE;
    }

    GenericPlatform_DeleteLock(E, E->PlatformLock);

	fprintf(stderr, "*** GenericPlatformDelete, Allocs: %d, Frees: %d, Allocated: %d.\n",
			E->numAllocs, E->numFrees, E->numAllocated); fflush(stderr);
#ifdef DEBUG_MODE
	/* memory collector */
	{
		FILE* f = fopen("dump.txt", "w");
		if(f != NULL)
		{
			char str[72];
			sprintf(str, "*** We Have %u Not Freed Memory Blocks ***\n", GenericPlatform_MemoryCount(E));
			fputs(str, f); fflush(f);
			GenericPlatform_MemoryDump(E, f);
			fclose(f);
		}
		GenericPlatform_MemoryDeallocate(E);
	}
	/* memory collector */
#endif

    free(E);

    return IPDR_OK;
}

static
IPDR_Status GenericPlatform_GetLastErrorMessage(IPDR_PlatformArg P,
                                                 char *Msg)
{
#ifdef _MSC_VER
    LPVOID Str;

    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
                  FORMAT_MESSAGE_IGNORE_INSERTS |
                  FORMAT_MESSAGE_ALLOCATE_BUFFER,
                  NULL,
                  GetLastError(),
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  (LPTSTR) & Str,
                  MAX_ERROR_STRING_LENGTH,
                  NULL);
    strcpy(Msg, (const char*)Str);
    LocalFree(Str);

#else
    /* We could use strerror_r on VxWorks (Solaris doesn't have
       it, presumably because it's not in POSIX yet). But then we need a
       buffer space in GenericST and the VxWorks documentation doesn't
       specify how big that needs to be. */

    const char* stre;

    if (errno == 0) {
        stre = IPDR_NO_ERROR_ERROR_MESSAGE; /* Must do this to avoid weird log messages. */
    } else {
        stre = strerror(errno);
    }

    if (stre) {
        strncpy(Msg, stre, IPDR_MAX_ERROR_LENGTH-1);
        Msg[IPDR_MAX_ERROR_LENGTH-1] = '\0'; /* ensure null-termination */
    } else {
        sprintf(Msg, "errno=%d", errno);
    }
#endif

    return IPDR_OK;
}

/* #define TEST_SET_EXPORTER_HANDLE */

static
IPDR_Status GenericPlatform_SetExporterHandle(IPDR_PlatformArg E, IPDR_ExporterHandle CH)
{
# if defined(TEST_SET_EXPORTER_HANDLE)
    /* will go to stderr */
    LOG_WRITE(E, IPDR_LEVEL_ERROR, "*** testing SetExporterHandle: before");
# endif
    E->ExporterHandle = CH;
    E->ExporterHandleRegistered = IPDR_TRUE;
# if defined(TEST_SET_EXPORTER_HANDLE)
    /* will go to log */
    LOG_WRITE(E, IPDR_LEVEL_ERROR, "*** testing SetClienHandle: after");
# endif
    return IPDR_OK;
}

static
IPDR_Status GenericPlatform_UnsetExporterHandle(IPDR_PlatformArg E)
{
# if defined(TEST_SET_EXPORTER_HANDLE)
    /* will go to log */
    LOG_WRITE(E, IPDR_LEVEL_ERROR, "*** testing UnsetExporterHandle: before");
# endif
    E->ExporterHandle = 0;
    E->ExporterHandleRegistered = IPDR_FALSE;
# if defined(TEST_SET_EXPORTER_HANDLE)
    /* will go to stderr */
    LOG_WRITE(E, IPDR_LEVEL_ERROR, "*** testing UnsetClienHandle: after");
# endif
    return IPDR_OK;
}

/* ------------------------------------------------------------- */

IPDR_Status GenericPlatform_Initialize(IPDR_PlatformServices *PS)
{
    return GenericPlatform_InitializeInternal(PS);
}

/* ---------------------- Main thread function ------------------------- */

static
void *GenericPlatform_MainFunc(void *P)
{
    struct timeval TV, ETV, prevTV;
    GenericTimer *T, *ET;
    IPDR_PlatformArg E = CAST_VOID(IPDR_PlatformArg, P);

    GenericFDEvent *UFE;
    GenericFDEvent *NextUFE, *PrevUFE;
    IPDR_BOOL FDEventTriggered;
    IPDR_BOOL FDUnregistered;
    int i;

    GenericPlatform_Lock(P, E->PlatformLock);

    BlockSigPipe(E);

    E->ScanningFDs = IPDR_FALSE;
    E->Stopped = IPDR_FALSE;
    GenericPlatform_GetTV(&prevTV);

    for (;;) {
        int selectRC;
        int selectErrno;
        long timeDelta;
        struct timeval *ST = NULL;

#ifdef TRACE_MAINFUNC
        fprintf(stderr, "MainFunc loop (timers) enabled=%d, stopped=%d\n", E->Enabled, E->Stopped); fflush(stderr);
#endif

        if (! E->Enabled) {
#ifdef TRACE_MAINFUNC
            fprintf(stderr, "MainFunc quit[1]\n"); fflush(stderr);
#endif
            goto endMainFunc; /* need goto because we can't do 2-level break */
        }

        GenericPlatform_GetTV(&TV);

        timeDelta = (TV.tv_sec - prevTV.tv_sec) * 1000 + (TV.tv_usec - prevTV.tv_usec) / 1000;

        if (timeDelta < 0 || timeDelta > TIME_SKEW_DELTA) {
            /* Somebody has changed the time, do our best to readjust all
               the timer events */
            char t1buf[IPDR_TIME_FORMAT_LENGTH + 1];
            char t2buf[IPDR_TIME_FORMAT_LENGTH + 1];
            LOG_WRITE3(E, IPDR_LEVEL_ERROR, "Clock has changed by %ld millisec from %.99s to %.99s",
                       timeDelta,
                       GenericPlatform_FormatTimeGMT(P, prevTV.tv_sec, t1buf),
                       GenericPlatform_FormatTimeGMT(P, TV.tv_sec, t2buf));
            for (T = E->Timers; T != NULL; T = T->NextTimer) {
                T->ExpirationTime = TV;
                T->ExpirationTime.tv_sec += (T->Remaining / 1000);
                T->ExpirationTime.tv_usec += (T->Remaining % 1000) * 1000;
                if (T->ExpirationTime.tv_usec > 1000000) {
                    T->ExpirationTime.tv_usec -= 1000000;
                    T->ExpirationTime.tv_sec += 1;
                }
            }
            timeDelta = 0; /* So it doesn't affect calculation of remaining times below */
        }

        prevTV = TV;

        /* find if there is a timer event */

        if (E->Timers != NULL) {

            T = E->Timers;
            ET = NULL; /* gets the next expiry time */

            while (T != NULL) {

                if (! E->Enabled) {
#ifdef TRACE_MAINFUNC
                    fprintf(stderr, "MainFunc quit[2]\n"); fflush(stderr);
#endif
                    goto endMainFunc; /* need goto because we can't do 2-level break */
                }

                /* check for expired timers */
                if (T->ExpirationTime.tv_sec < TV.tv_sec ||
                    (T->ExpirationTime.tv_sec == TV.tv_sec
                     && T->ExpirationTime.tv_usec <= TV.tv_usec)) {

                    T->Remaining = 0;

                    if (! T->Periodic) {
                        /* unlink but don't delete because an unregister call
                           will do that. */
                        if (T->PrevTimer == NULL)
                            E->Timers = T->NextTimer;
                        else
                            T->PrevTimer->NextTimer = T->NextTimer;

                        if (T->NextTimer != NULL)
                            T->NextTimer->PrevTimer = T->PrevTimer;

                        T->Expired = IPDR_TRUE;
#ifdef TRACE_MAINFUNC
                        fprintf(stderr, "MainFunc timer callback\n"); fflush(stderr);
#endif
                        T->Callback(T->Arg);

                        T = E->Timers; /* repeat from beginning ... probably not needed but best to be safe. */
                        continue;
                    } else { /* periodic */
                        /* recalculate the expiration time of the timer */
                        GenericPlatform_GetTV((& (T->ExpirationTime)));
                        T->Remaining = T->Timeout;
                        T->ExpirationTime.tv_sec += (T->Timeout / 1000);
                        T->ExpirationTime.tv_usec += (T->Timeout % 1000) * 1000;
                        if (T->ExpirationTime.tv_usec > 1000000) {
                            T->ExpirationTime.tv_usec -= 1000000;
                            T->ExpirationTime.tv_sec += 1;
                        }

#ifdef TRACE_MAINFUNC
                        fprintf(stderr, "MainFunc timer callback\n"); fflush(stderr);
#endif
                        T->Callback(T->Arg);

                        T = E->Timers; /* repeat from beginning because this one might need firing again */
                        continue;
                    }
                } else { /* not yet expired */
                    if (ET == NULL ||
                        T->ExpirationTime.tv_sec < ET->ExpirationTime.tv_sec ||
                        (T->ExpirationTime.tv_sec == ET->ExpirationTime.tv_sec &&
                         T->ExpirationTime.tv_usec < ET->ExpirationTime.tv_usec)) {
                        T->Remaining -= timeDelta;
                        ET = T;
                    }
                }

                T = T->NextTimer;
            } /* while (T != NULL) */

            if (ET != NULL) {
                ETV.tv_sec = ET->ExpirationTime.tv_sec - TV.tv_sec;
                ETV.tv_usec = ET->ExpirationTime.tv_usec - TV.tv_usec;

                while (ETV.tv_usec < 0) {
                    ETV.tv_sec -= 1;
                    ETV.tv_usec += 1000000;
                }
                if (ETV.tv_sec < 0) { /* shouldn't happen, but let's be safe */
                    ETV.tv_sec = 0;
                    ETV.tv_usec = 1;
                }

                ST = & ETV;
            }
        } /* if (E->Timers != NULL) */

        if (ST == NULL) { /* Don't wait too long, just in case */
            ETV.tv_sec = 1;
            ETV.tv_usec = 0;
            ST = & ETV;
        }

        /* We've finished looking for timer events. Now, let's try for an
           I/O event. */

#ifdef TRACE_MAINFUNC
        fprintf(stderr, "MainFunc select delay %ldsec %ldusec\n", ST->tv_sec, ST->tv_usec); fflush(stderr);
#endif

#ifdef _MSC_VER
#error Need to verify that select with empty list of FDs works for _MSC_VER
#endif

        /* The following is a rather crude way of ensuring that we don't
         * wait too long (e.g., a register timer event happens in another
         * thread or --- more important --- the main loop has been
         * stopped). A better way would be if we could wake up the select
         * ... that could be done with pipes but wouldn't be very
         * platform-indepedent. As things are, we wake up every 0.5
         * second, find that there are no timer events, and go back to
         * sleep.
         *
         * If you change this value, be sure to modify the time-out loop in
         * GenericPlatform_Stop!!!
         */

        if (ST->tv_sec >= 0 || ST->tv_usec >= MAX_TIMEWAIT) {
            ST->tv_sec = 0;
            ST->tv_usec = 500000;
#ifdef TRACE_MAINFUNC
            fprintf(stderr, "MainFunc select delay reset to %ldsec %ldusec\n", ST->tv_sec, ST->tv_usec); fflush(stderr);
#endif
        }

        if (! E->Enabled) {
#ifdef TRACE_MAINFUNC
            fprintf(stderr, "MainFunc quit[3]\n"); fflush(stderr);
#endif
            goto endMainFunc; /* need goto because we can't do 2-level break */
        }

        GenericPlatform_Unlock(P, E->PlatformLock);

        selectErrno = 0;
        errno = 0;
        selectRC = select(E->MaxFD.socketFd + 1, & (E->ReadFDs), & (E->WriteFDs), NULL, ST);
        /* The only errno that should be OK is EINTR ... this just means
           to re-do the select.*/
        if (selectRC != 0) {
            selectErrno = errno;
        }

        GenericPlatform_Lock(E, E->PlatformLock);

#ifdef TRACE_MAINFUNC
        fprintf(stderr, "MainFunc loop (FDs) enabled=%d, stopped=%d, selectRC=%d\n", E->Enabled, E->Stopped, selectRC); fflush(stderr);
#endif

        /* Do *NOT* check for selectRC here ... the code depends on
         * going through the FDs, which resets the bits ... if no
         * callback was triggered, then there's a break from the
         * loop. */

        for (i = 0; ; i++) { /* Do at most MAX_SELECT_ITERATIONS times
                                ... break is just before select() at
                                end of this loop. We have the loop so
                                that we can clean up a bunch of
                                pending reads before looking for timer
                                events. */

            if (! E->Enabled) {
#ifdef TRACE_MAINFUNC
                fprintf(stderr, "MainFunc quit[4]\n"); fflush(stderr);
#endif
                goto endMainFunc; /* need goto because we can't do 2-level break */
            }

            if (selectRC < 0) {
				switch(selectErrno){
	                case EINTR:
						LOG_WRITE3(E, IPDR_LEVEL_WARNING, "selectRC[%d]==EINTR (%d) (%.50s)", i, selectErrno, strerror(selectErrno));
						break;
					case EBADF:
						LOG_WRITE3(E, IPDR_LEVEL_WARNING, "selectRC[%d]==EBADF (%d) (%.50s)", i, selectErrno, strerror(selectErrno));
						break;
					case EINVAL:
						LOG_WRITE3(E, IPDR_LEVEL_WARNING, "selectRC[%d]==EINVAL (%d) (%.50s)", i, selectErrno, strerror(selectErrno));
						break;
					case ENOMEM:
						LOG_WRITE3(E, IPDR_LEVEL_WARNING, "selectRC[%d]==ENOMEM (%d) (%.50s)", i, selectErrno, strerror(selectErrno));
						goto endMainFunc; /* need goto because we can't do 2-level break */
						break;
				}
                break; /* get out of select() ... do timeouts again */
            }
            /* It's hard to say what we should do if we got a bad return
               code from select() and errno != EINTR. For now, let's just
               keep on doing the code and hope for the best. For future, we
               should probably have a "deep bail-out" function from
               MainFunc, which can do something desperate, such as
               System.exit(). */

            /* invoke callbacks associated with file descriptors */

            E->ScanningFDs = IPDR_TRUE;

            FDEventTriggered = IPDR_FALSE;
            FDUnregistered = IPDR_FALSE;

            for (UFE = E->FDReadEvents; UFE != NULL; UFE = UFE->NextEvent) {
#ifdef TRACE_MAINFUNC
                fprintf(stderr, "*** MainFunc: Read FD " Platform_SOCKET_FMT " for run MT\n", Platform_SOCKET_DISPLAY(UFE->FD)); fflush(stderr);
#endif
                if (! E->Enabled) {
#ifdef TRACE_MAINFUNC
                    fprintf(stderr, "MainFunc quit[5]\n"); fflush(stderr);
#endif
                    goto endMainFunc; /* need goto because we can't do 2-level break */
                }

                if (UFE->Unregistered) {
                    FDUnregistered = IPDR_TRUE;
#ifdef TRACE_MAINFUNC
                    fprintf(stderr, "MainFunc callback unregistered fd " Platform_SOCKET_FMT "\n", Platform_SOCKET_DISPLAY(UFE->FD)); fflush(stderr);
#endif
                } else {
                    /* Note that the Callback might call UnregisterSocket, which
                       would clear the UFE->FD entry */
                    if (FD_ISSET(UFE->FD.socketFd, & (E->ReadFDs)) && selectRC >= 0) {
#ifdef TRACE_MAINFUNC
                        fprintf(stderr, "MainFunc callback read fd " Platform_SOCKET_FMT "\n", Platform_SOCKET_DISPLAY(UFE->FD)); fflush(stderr);
#endif
                        UFE->Callback(UFE->FD, UFE->Arg);
                        FDEventTriggered = IPDR_TRUE;
                    } else {
                        FD_SET(UFE->FD.socketFd, (& E->ReadFDs)); /* set it on for next select */
                    }
                }
            } /* loop over read events */

            for (UFE = E->FDWriteEvents; UFE != NULL; UFE = UFE->NextEvent) {
#ifdef TRACE_MAINFUNC
                fprintf(stderr, "*** MainFunc: Write FD " Platform_SOCKET_FMT " for run MT", Platform_SOCKET_DISPLAY(UFE->FD)); fflush(stderr);
#endif
                if (! E->Enabled) {
#ifdef TRACE_MAINFUNC
                    fprintf(stderr, "MainFunc quit[6]\n"); fflush(stderr);
#endif
                    goto endMainFunc; /* need goto because we can't do 2-level break */
                }
                if (! UFE->Unregistered) {
                    /* Note that the Callback might call UnregisterSocket, which
                       would clear the UFE->FD entry */
                    if (FD_ISSET(UFE->FD.socketFd, & (E->WriteFDs)) && selectRC >= 0) {
#ifdef TRACE_MAINFUNC
                        fprintf(stderr, "MainFunc callback write fd " Platform_SOCKET_FMT "\n", Platform_SOCKET_DISPLAY(UFE->FD)); fflush(stderr);
#endif
                        UFE->Callback(UFE->FD, UFE->Arg);
                        FDEventTriggered = IPDR_TRUE;
                    } else {
                        FD_SET(UFE->FD.socketFd, (& E->WriteFDs)); /* for next select */
                    }
                }
            } /* loop over write events */

            E->ScanningFDs = IPDR_FALSE;

            /* If a file descriptor was unregistered, scan the file
             * descriptors lists for unregistered FDs */

            if (FDUnregistered) {
                E->MaxFD.socketFd = -1;
                UFE = E->FDReadEvents;
                PrevUFE = NULL;
                while (UFE != NULL) {
                    NextUFE = UFE->NextEvent;
                    if (UFE->Unregistered) {
                        FD_CLR(UFE->FD.socketFd, (& E->ReadFDs));
                        if (PrevUFE == NULL)
                            E->FDReadEvents = NextUFE;
                        else
                            PrevUFE->NextEvent = NextUFE;
                        GenericPlatform_MemoryFree(E, UFE);
                    } else {
                        if (UFE->FD.socketFd > E->MaxFD.socketFd)
                            E->MaxFD = UFE->FD;
                        PrevUFE = UFE;
                    }
                    UFE = NextUFE;
                }

                UFE = E->FDWriteEvents;
                PrevUFE = NULL;
                while (UFE != NULL) {
                    NextUFE = UFE->NextEvent;
                    if (UFE->Unregistered) {
                        FD_CLR(UFE->FD.socketFd, (& E->WriteFDs));
                        if (PrevUFE == NULL)
                            E->FDWriteEvents = NextUFE;
                        else
                            PrevUFE->NextEvent = NextUFE;
                        GenericPlatform_MemoryFree(E, UFE);
                    } else {
                        if (UFE->FD.socketFd > E->MaxFD.socketFd)
                            E->MaxFD = UFE->FD;
                        PrevUFE = UFE;
                    }
                    UFE = NextUFE;
                }
            } /* if FDUnregistered */

            if (! E->Enabled) {
#ifdef TRACE_MAINFUNC
                fprintf(stderr, "MainFunc quit[7]\n"); fflush(stderr);
#endif
                goto endMainFunc; /* need goto because we can't do 2-level break */
            }

            /* If no I/O event happened, we're finished. Otherwise,
             * continue on doing select()s but don't do the timer
             * loop. The select()s are with no timeout ... we're just
             * trying to process a bunch of pending I/Os as fast as we
             * can without any relocking/unlocking of the mutexes. */

            if (! FDEventTriggered || E->MaxFD.socketFd == -1)
                break ;

#ifdef TRACE_MAINFUNC
            fprintf(stderr, "MainFunc loop (FD iteration) enabled=%d, stopped=%d\n", E->Enabled, E->Stopped); fflush(stderr);
#endif

            if (! E->Enabled) {
#ifdef TRACE_MAINFUNC
                fprintf(stderr, "MainFunc quit[8]\n"); fflush(stderr);
#endif
                goto endMainFunc; /* need goto because we can't do 2-level break */
            }

            if (i >= MAX_SELECT_ITERATIONS) { /* but don't do it too many times */
                break;
            }

            selectErrno = 0;
            errno = 0;
            selectRC = select(E->MaxFD.socketFd + 1, & E->ReadFDs, & E->WriteFDs, NULL, & ZeroTV);
            if (selectRC < 0) {
                selectErrno = errno;
            }

        } /* for (i = 0; i < MAX_SELECT_ITERATIONS && E->Enabled; i++) */
#ifdef TRACE_MAINFUNC
        fprintf(stderr, "MainFunc loop (end FD iterations) enabled=%d, stopped=%d\n", E->Enabled, E->Stopped); fflush(stderr);
#endif
    } /* while (E->Enabled) */

endMainFunc:

#ifdef TRACE_MAINFUNC
    fprintf(stderr, "MainFunc finished enabled=%d, stopped=%d\n", E->Enabled, E->Stopped); fflush(stderr);
#endif

    E->Stopped = IPDR_TRUE; /* Signal to GeneralPlatform_Stop that we've left the loop */

    GenericPlatform_Unlock(P, E->PlatformLock);

#ifdef TRACE_MAINFUNC
    fprintf(stderr, "MainFunc finished[2] enabled=%d, stopped=%d\n", E->Enabled, E->Stopped); fflush(stderr);
#endif

    return NULL;
}

/* ---------------------- MemoryServices ------------------------- */

#if defined(DEBUG_MEMORY) || defined(DEBUG_MODE)
#include <Common/MemoryManagement.h> /* For debugging in MemoryFree */
#endif

#ifdef DEBUG_MODE
	/* memory collector */
static
unsigned int
GenericPlatform_MemoryCount(IPDR_PlatformArg E)
{
	unsigned int count = 0;
	GenericMemHdr* mh = E->memCollectorHead;
	while(mh != NULL)
	{
		count++;
		mh = mh->next;
	}
	return count;
}
static
unsigned int
GenericPlatform_MemoryDeallocate(IPDR_PlatformArg E)
{
	unsigned int count = 0;
	GenericMemHdr* mh = E->memCollectorHead;
	while(mh != NULL)
	{
		count++;
		mh = (GenericMemHdr *)mh->next;
		free((void*)mh);
	}
	E->memCollectorHead = NULL;
	E->memCollectorTail = NULL;
	return count;
}
static
unsigned int
GenericPlatform_MemoryDump(IPDR_PlatformArg E, FILE* stream)
{
	unsigned int count = 0;
	GenericMemHdr* mh = E->memCollectorHead;
	while(mh != NULL)
	{
		unsigned i , j ;
		char *p1 = (char *)mh;
		char *p2 = (char *)mh + mh->size;
		char str[72];
		char space = ' ';
		sprintf(str, "*** Memory Block, Size: %10u, address: %p ***\n", mh->size, mh);
		fputs(str, stream); fflush(stream);
		for(; p1 < p2 ;)
		{
			char *p3 = p1;
			char *b1 = str;
			char *b2 = str;
			memset(str, space, 72);
			for( i = 0 ; i < 4 && p1 < p2 ; i++ )
			{
				for( j = 0 ; j < 4 && p1 < p2 ; j++, b1+=3 , p1++)
				{
					unsigned char ch = *p1;
					sprintf(b1, "%02X ", ch);
				}
				sprintf(b1++, " ");
			}
			sprintf(b1++, "  ");
			b1++;
			p1 = p3;
			memset(b1, space, 54-(b1-b2));
			b1 += 54-(b1-b2);
			for( i = 0 ; i < 16 && p1 < p2 ; i++, b1++, p1++)
			{
				char ch = *p1;
				ch = isprint((int)ch)?ch:'.';
				sprintf(b1, "%c", ch);
			}
			sprintf(b1++, "\n");
			fputs(str, stream); fflush(stream);
		}
		count++;
		mh = mh->next;
	}
	return count;
}
	/* memory collector */
#endif

#if defined(DEBUG_MEMORY)

IPDR_Status GenericPlatform_dm_MemoryAllocate(IPDR_PlatformArg E, int Size, void **Buf,
                                               const char* FileName, int LineNum)
{
    GenericMemHdr *mh;

    Size += sizeof (GenericMemHdr);

    E->numAllocs++;
    E->bytesAllocated += Size;

    mh = _malloc_leap(FileName, LineNum, Size);

    if (mh == NULL)
        return IPDR_ERR_ALLOCATION_FAILED;

    mh->size = Size;
    mh->ReferenceCount_unused = -999;
    mh->Destructor_unused = 0;
/*#ifdef DEBUG_MODE*/
    mh->eyecatcher = 0xa0a1a2a3;
/*#endif*/
    *Buf = (char *)mh + sizeof (GenericMemHdr);

    return IPDR_OK;
}

IPDR_Status GenericPlatform_dm_MemoryFree(IPDR_PlatformArg E, void *Buf,
                                           const char* FileName, int LineNum)
{
    if (Buf != NULL) {
        GenericMemHdr* mh = (GenericMemHdr*) ((char*)Buf - sizeof (GenericMemHdr));
        E->numFrees++;
        E->bytesAllocated -= mh->size;

/*#ifdef DEBUG_MODE*/
        mh->eyecatcher = 0xa3a2a1a0;
/*#endif*/
        _free_leap(FileName, LineNum, mh);
    }
	else
	{
		/* This is a serious logic error and indicates that somebody
           has done an extra MemoryFree. */
       LOG_WRITE1(E, IPDR_LEVEL_FATAL, "free Bufer is NULL\n");
		return IPDR_ERR_MEMORY_FREE;
	}
    return IPDR_OK;
}

#else

static
IPDR_Status GenericPlatform_MemoryAllocate(IPDR_PlatformArg E, size_t Size, void **Buf)
{
    GenericMemHdr *mh;

    Size += sizeof (GenericMemHdr);

    E->numAllocs++;
	E->numAllocated++;
    E->bytesAllocated += Size;
/*
	printf("%s allocates %d bytes, %d allocs, total %lu\n", E->eyecatcher, Size, E->numAllocs, E->bytesAllocated );
*/
    mh = malloc(Size);

    if (mh == NULL)
        return IPDR_ERR_ALLOCATION_FAILED;

    mh->size = Size;
    /*mh->ReferenceCount_unused = -999;
    mh->Destructor_unused = 0;*/
#ifdef DEBUG_MODE
    mh->eyecatcher = 0xcefacefa;
#endif
#ifdef DEBUG_MODE
	/* memory collector */
	/*switch((unsigned)mh & 0x00000FFF){
		case 0xfe8:
			{
				int a = 5;
				int b = a;
			}
			break;
	};*/
	if(E->memCollectorHead == NULL && E->memCollectorTail == NULL)
	{
		mh->next = NULL;
		mh->prev = NULL;
		E->memCollectorHead = mh;
		E->memCollectorTail = mh;
	}
	else
	{
		mh->next = NULL;
		mh->prev = E->memCollectorTail;
		E->memCollectorTail->next = mh;
		E->memCollectorTail = mh;
	}
	/* memory collector */
#endif
    *Buf = (char *)mh + sizeof (GenericMemHdr);

    return IPDR_OK;
}

static
IPDR_Status GenericPlatform_MemoryAllocateWithHdr(IPDR_PlatformArg E, size_t HdrSize, size_t Size, void **Buf)
{
    /* This initial test will be optimized away */
    /*if (HdrSize != sizeof (GenericMemHdr)) {
        LOG_WRITE2(E, IPDR_LEVEL_FATAL, "MemoryAllocateWithHdr hdr size invalid: %u should be %u ***",
                   HdrSize, sizeof (GenericMemHdr));
        ABORT_CALL;
    }*/

    return GenericPlatform_MemoryAllocate(E, HdrSize + Size, Buf);
}

static
IPDR_Status GenericPlatform_MemoryFree(IPDR_PlatformArg E, void *Buf)
{
    if (Buf != NULL) {
        GenericMemHdr* mh = (GenericMemHdr*) ((char*)Buf - sizeof (GenericMemHdr));
        E->numFrees++;
		E->numAllocated--;
        E->bytesAllocated -= mh->size;

#ifdef DEBUG_MODE
        mh->eyecatcher = 0xefbeadde;
#endif
#ifdef DEBUG_MODE
	/* memory collector */
	if(mh->next == NULL && mh->prev == NULL)
	{
		E->memCollectorHead = NULL;
		E->memCollectorTail = NULL;
	}
	else if(mh->prev != NULL && mh->next == NULL)
	{
		E->memCollectorTail = mh->prev;
		((GenericMemHdr*)mh->prev)->next = NULL;
	}
	else if(mh->prev == NULL && mh->next != NULL)
	{
		E->memCollectorHead = mh->next;
		((GenericMemHdr*)mh->next)->prev = NULL;
	}
	else if(mh->prev != NULL && mh->next != NULL)
	{
		((GenericMemHdr*)mh->next)->prev = mh->prev;
		((GenericMemHdr*)mh->prev)->next = mh->next;
	}
	/* memory collector */
#endif
        free(mh);
    }
	else
	{
		/* This is a serious logic error and indicates that somebody
           has done an extra MemoryFree. */
		return IPDR_ERR_MEMORY_FREE;
	}
    return IPDR_OK;
}

static
IPDR_Status GenericPlatform_MemoryFreeWithHdr(IPDR_PlatformArg E, size_t HdrSize, void *Buf)
{
    if (Buf != NULL) {
        /* This test will be optimized away */
        /*if (HdrSize != sizeof (GenericMemHdr)) {
            LOG_WRITE2(E, IPDR_LEVEL_FATAL, "MemoryFreeWitHHdr hdr size invalid: %u should be %u ***\n",
                       HdrSize, sizeof (GenericMemHdr));
            ABORT_CALL;
        }*/
        return GenericPlatform_MemoryFree(E, (char*)Buf /* Dmitryk fixing memory bug !!! +HdrSize */);
    }
	else
	{
		/* This is a serious logic error and indicates that somebody
           has done an extra MemoryFree. */
		return IPDR_ERR_MEMORY_FREE;
	}
    return IPDR_OK;
}

#endif

static
IPDR_Status GenericPlatform_MemoryStatistics(IPDR_PlatformArg E,
                                              unsigned int *numAllocs,
                                              unsigned int *numFrees,
                                              size_t *bytesAllocated)
{
	if(!numAllocs || !numFrees || !bytesAllocated)
		return IPDR_ERR_INVALID_VALUE;

    GenericPlatform_Lock(E, E->PlatformLock);
    *numAllocs = E->numAllocs;
    *numFrees = E->numFrees;
    *bytesAllocated = E->bytesAllocated;
    GenericPlatform_Unlock(E, E->PlatformLock);
    return IPDR_OK;
}


/* ---------------------- EventServices ------------------------- */

static
IPDR_Status GenericPlatform_RegisterTimer(IPDR_PlatformArg E,
                                           IPDR_TimerCallback_t Callback,
                                           void *Arg,
                                           int Timeout,
                                           IPDR_TimerID *T)
{
    GenericTimer *UT;

    GenericPlatform_Lock(E, E->PlatformLock);
    {
        IPDR_Status Status = GenericPlatform_MemoryAllocate(E, sizeof *UT, (void**)&UT);
        if (FAILURE(Status)) {
            GenericPlatform_Unlock(E, E->PlatformLock);
            return Status;
        }
    }

    if (E->Timers != NULL)
        E->Timers->PrevTimer = UT;

    UT->Callback = Callback;
    UT->Arg = Arg;
    UT->PrevTimer = NULL;
    UT->NextTimer = E->Timers;
    UT->Periodic = IPDR_FALSE;
    UT->Timeout = Timeout;
    UT->Remaining = Timeout;
    UT->Expired = IPDR_FALSE;

    E->Timers = UT;

    GenericPlatform_GetTV((& (UT->ExpirationTime)));

    UT->ExpirationTime.tv_sec += (Timeout / 1000);
    UT->ExpirationTime.tv_usec += (Timeout % 1000) * 1000;
    if (UT->ExpirationTime.tv_usec > 1000000) {
        UT->ExpirationTime.tv_usec -= 1000000;
        UT->ExpirationTime.tv_sec += 1;
    }

    *T = (IPDR_TimerID) UT;

    GenericPlatform_Unlock(E, E->PlatformLock);
    return IPDR_OK;
}

static
IPDR_Status GenericPlatform_ReregisterTimer(IPDR_PlatformArg E,
                                             IPDR_TimerCallback_t Callback,
                                             void *Arg,
                                             int Timeout,
                                             IPDR_TimerID* T)
{
    GenericTimer *UT = *T;

    if (*T == NULL) {
        return GenericPlatform_RegisterTimer(E, Callback, Arg, Timeout, T);
    }

    GenericPlatform_Lock(E, E->PlatformLock);

    UT->Callback = Callback;
    UT->Arg = Arg;

    GenericPlatform_GetTV((& (UT->ExpirationTime)));

    UT->ExpirationTime.tv_sec += (Timeout / 1000);
    UT->ExpirationTime.tv_usec += (Timeout % 1000) * 1000;
    if (UT->ExpirationTime.tv_usec > 1000000) {
        UT->ExpirationTime.tv_usec -= 1000000;
        UT->ExpirationTime.tv_sec += 1;
    }

    GenericPlatform_Unlock(E, E->PlatformLock);
    return IPDR_OK;
}

static
IPDR_Status GenericPlatform_RegisterPeriodicTimer(IPDR_PlatformArg E,
                                                   IPDR_TimerCallback_t Callback,
                                                   void *Arg,
                                                   int Timeout,
                                                   IPDR_TimerID *T)
{
    GenericTimer *UT;

    GenericPlatform_Lock(E, E->PlatformLock);
    {
        IPDR_Status Status = GenericPlatform_MemoryAllocate(E, sizeof *UT, (void**)&UT);
        if (FAILURE(Status)) {
            GenericPlatform_Unlock(E, E->PlatformLock);
            return Status;
        }
    }

    if (E->Timers != NULL)
        E->Timers->PrevTimer = UT;

    UT->Callback = Callback;
    UT->Arg = Arg;
    UT->PrevTimer = NULL;
    UT->NextTimer = E->Timers;
    UT->Periodic = IPDR_TRUE;
    UT->Timeout = Timeout;
    UT->Remaining = Timeout;
    UT->Expired = IPDR_FALSE;

    E->Timers = UT;

    GenericPlatform_GetTV((& (UT->ExpirationTime)));

    UT->ExpirationTime.tv_sec += (Timeout / 1000);
    UT->ExpirationTime.tv_usec += (Timeout % 1000) * 1000;
    if (UT->ExpirationTime.tv_usec > 1000000) {
        UT->ExpirationTime.tv_usec -= 1000000;
        UT->ExpirationTime.tv_sec += 1;
    }

    *T = (IPDR_TimerID) UT;

    GenericPlatform_Unlock(E, E->PlatformLock);
    return IPDR_OK;
}

static
IPDR_Status GenericPlatform_RegisterReadSocket(IPDR_PlatformArg E,
                                                IPDR_SocketHandle FD,
                                                IPDR_SocketCallback_t Callback,
                                                void *Arg,
                                                IPDR_SocketEventID *ID)
{
    IPDR_Status Status;
    GenericFDEvent *UFE;

    Status = GenericPlatform_MemoryAllocate(E, sizeof(GenericFDEvent), (void**)&UFE);
    if (FAILURE(Status)) { return Status; }

#ifdef TRACE_MAINFUNC
    fprintf(stderr, "RegisterReadSocket fd " Platform_SOCKET_FMT "\n", Platform_SOCKET_DISPLAY(FD)); fflush(stderr);
#endif

    *ID = NULL;

    GenericPlatform_Lock(E, E->PlatformLock);

    UFE->FD = FD;
    UFE->Callback = Callback;
    UFE->Arg = Arg;
    UFE->Unregistered = IPDR_FALSE;

    /* If we were running in multi-threaded mode (that is, one thread
     * for each socket), we would start a thread to handle the reads.
     * However, we're in single-threaded mode with non-blocking I/O,
     * so we add the FD to the list of FDs to be checked by
     * select(). [Yes, the Start function starts a thread; but that
     * thread is the only one.]  */

    UFE->NextEvent = E->FDReadEvents;
    E->FDReadEvents = UFE;

    FD_SET(UFE->FD.socketFd, & (E->ReadFDs));
    if (FD.socketFd > E->MaxFD.socketFd) {
        E->MaxFD = FD;
    }

    GenericPlatform_Unlock(E, E->PlatformLock);
    *ID = UFE;
    return IPDR_OK;
}

static
IPDR_Status GenericPlatform_RegisterWriteSocket(IPDR_PlatformArg E,
                                                 IPDR_SocketHandle FD,
                                                 IPDR_SocketCallback_t Callback,
                                                 void *Arg,
                                                 IPDR_SocketEventID *ID)
{
    GenericFDEvent *UFE;
    GenericPlatform_Lock(E, E->PlatformLock);
#ifdef TRACE_MAINFUNC
    fprintf(stderr, "RegisterWriteSocket fd " Platform_SOCKET_FMT "\n", Platform_SOCKET_DISPLAY(FD)); fflush(stderr);
#endif
    *ID = NULL;

    GenericPlatform_MemoryAllocate(E, sizeof(GenericFDEvent), (void**)&UFE);
    if (UFE == NULL) {
        GenericPlatform_Unlock(E, E->PlatformLock);
        return IPDR_ERR_ALLOCATION_FAILED;
    }

    UFE->FD = FD;
    UFE->Callback = Callback;
    UFE->Arg = Arg;
    UFE->NextEvent = E->FDWriteEvents;
    UFE->Unregistered = IPDR_FALSE;

    E->FDWriteEvents = UFE;

    FD_SET(UFE->FD.socketFd, & (E->WriteFDs));
    if (FD.socketFd > E->MaxFD.socketFd) {
        E->MaxFD = FD;
    }

    *ID = UFE;

    GenericPlatform_Unlock(E, E->PlatformLock);
    return IPDR_OK;
}

static
IPDR_Status GenericPlatform_UnregisterTimer(IPDR_PlatformArg E,
                                             IPDR_TimerID T)
{
    GenericTimer *UT = T;

    if (UT == NULL) {
        return IPDR_OK;
    }

    GenericPlatform_Lock(E, E->PlatformLock);

    if (! UT->Expired) {
        /* unlink only if unexpired: if it was expired, it will
           already be unlinked */
        if (UT->PrevTimer == NULL)
            E->Timers = UT->NextTimer;
        else
            UT->PrevTimer->NextTimer = UT->NextTimer;

        if (UT->NextTimer != NULL)
            UT->NextTimer->PrevTimer = UT->PrevTimer;
    }

    /* now it's safe to free it */
    GenericPlatform_MemoryFree(E, T);

    GenericPlatform_Unlock(E, E->PlatformLock);

    return IPDR_OK;
}

static
IPDR_Status GenericPlatform_UnregisterSocket(IPDR_PlatformArg E, IPDR_SocketEventID ID)
{
    GenericFDEvent *NextUFE, *PrevUFE = NULL, *UFE;

    if (ID == NULL) {
        return IPDR_OK;
    }

    GenericPlatform_Lock(E, E->PlatformLock);

    /* In case we're currently iterating over the file descriptors,
       mark the file descriptor for deletion. Also clear the select
       bits because otherwise select() will give us a EBADF error. */
    if (E->ScanningFDs) {
        UFE = ID;
#ifdef TRACE_MAINFUNC
        fprintf(stderr, "UnregisterSocket (while scanning FDs): " Platform_SOCKET_FMT "\n", Platform_SOCKET_DISPLAY(UFE->FD)); fflush(stderr);
#endif
        UFE->Unregistered = IPDR_TRUE;
        FD_CLR(UFE->FD.socketFd, &E->ReadFDs);
        FD_CLR(UFE->FD.socketFd, &E->WriteFDs);
        GenericPlatform_Unlock(E, E->PlatformLock);
        return IPDR_OK;
    }

#ifdef TRACE_MAINFUNC
    fprintf(stderr, "UnregisterSocket: " Platform_SOCKET_FMT "\n", Platform_SOCKET_DISPLAY(ID->FD)); fflush(stderr);
#endif

    UFE = E->FDReadEvents;
    E->MaxFD.socketFd = -1;

    while (UFE != NULL) {
        NextUFE = UFE->NextEvent;
        if (UFE == ID) {
#ifdef TRACE_MAINFUNC
            fprintf(stderr, "\tUnregisterSocket (read): " Platform_SOCKET_FMT "\n", Platform_SOCKET_DISPLAY(UFE->FD)); fflush(stderr);
#endif
            if (PrevUFE == NULL) {
                E->FDReadEvents = UFE->NextEvent;
            } else {
                PrevUFE->NextEvent = UFE->NextEvent;
            }

            FD_CLR(UFE->FD.socketFd, & (E->ReadFDs));
            GenericPlatform_MemoryFree(E, UFE);
        } else {
            if (UFE->FD.socketFd > E->MaxFD.socketFd) {
                E->MaxFD = UFE->FD;
            }
            PrevUFE = UFE;
        }

        UFE = NextUFE;
    }

    UFE = E->FDWriteEvents;
    PrevUFE = NULL;

    while (UFE != NULL) {
        NextUFE = UFE->NextEvent;
        if (UFE == ID) {
#ifdef TRACE_MAINFUNC
            fprintf(stderr, "\tUnregisterSocket (write): " Platform_SOCKET_FMT "\n", Platform_SOCKET_DISPLAY(UFE->FD)); fflush(stderr);
#endif
            if (PrevUFE == NULL) {
                E->FDWriteEvents = UFE->NextEvent;
            } else {
                PrevUFE->NextEvent = UFE->NextEvent;
            }
            FD_CLR(UFE->FD.socketFd, & (E->WriteFDs));
            GenericPlatform_MemoryFree(E, UFE);
        } else {
            if (UFE->FD.socketFd > E->MaxFD.socketFd) {
                E->MaxFD = UFE->FD;
            }
            PrevUFE = UFE;
        }
        UFE = NextUFE;
    }

    GenericPlatform_Unlock(E, E->PlatformLock);

    return IPDR_OK;
}

#if 0
/* This is no longer used (it's for debugging) */

static
char* GenericPlatform_DescribeTimer(IPDR_PlatformArg E,
                                    const char* descr,
                                    IPDR_TimerID T,
                                    char* buf)
{
    GenericTimer *UT = T;

    if (UT == NULL) {
        sprintf(buf, "%.50s(null)", descr);
    } else {
        char timeStrBuf[IPDR_TIME_FORMAT_LENGTH];
        FormatTimeGMT(timeStrBuf, UT->ExpirationTime.tv_sec);
        sprintf(buf, "%.50s(%ld%.15s%.15s:%.49s.%06ldZ)",
                descr,
                (long)UT->Timeout,
                UT->Periodic ? "rep" : "",
                UT->Expired ? "expired" : "",
                timeStrBuf,
                (long)UT->ExpirationTime.tv_usec);
    }
    return buf;
}

#endif

/* ---------------------- PersistenceServices ------------------------- */

/* These aren't currently used and can be replaced by NULL in the
 * PersistencedServices structure. */

static
IPDR_Status GenericPlatform_PersistentRetrieve(IPDR_PlatformArg E, const char *UnitName,
                                                char **Buffer, unsigned int *BufferLength)
{
    FILE *FP;
    struct stat StatBuf;

    if (stat(CONST_CAST_CHARPTR(UnitName), & StatBuf) < 0)
        return IPDR_ERR_PERSISTENCE_ERROR;

    *BufferLength = StatBuf.st_size;

#ifdef DEBUG_MEMORY
    if (FAILURE(GenericPlatform_dm_MemoryAllocate(E, *BufferLength, (void**)Buffer,
                                                      "PersistentRetrieve-" __FILE__, __LINE__))) {
        return IPDR_ERR_ALLOCATION_FAILED;
    }
#else
    if (FAILURE(GenericPlatform_MemoryAllocate(E, *BufferLength, (void**)Buffer))) {
        return IPDR_ERR_ALLOCATION_FAILED;
    }
#endif

    FP = fopen(UnitName, "r");
    if (FP == NULL)
        return IPDR_ERR_PERSISTENCE_ERROR;

    if (fread(*Buffer, 1, *BufferLength, FP) < (size_t) *BufferLength) {
        GenericPlatform_MemoryFree(E, *Buffer);
        *Buffer = 0;
        fclose(FP);
        return IPDR_ERR_PERSISTENCE_ERROR;
    }

    fclose(FP);
    return IPDR_OK;
}

static
IPDR_Status GenericPlatform_PersistentStore(IPDR_PlatformArg P, const char *UnitName,
                                             const char *Buffer, unsigned int BufferLength)
{
    FILE *FP = fopen(UnitName, "w");

    if (FP == NULL)
        return IPDR_ERR_PERSISTENCE_ERROR;

    if (fwrite(Buffer, 1, BufferLength, FP) < (size_t) BufferLength) {
        fclose(FP);
        return IPDR_ERR_PERSISTENCE_ERROR;
    }

    fclose(FP);
    return IPDR_OK;
}

/* ---------------------- NetworkServices ------------------------- */

static
IPDR_Status GenericPlatform_CreateTCPSocket(IPDR_PlatformArg E,
                                             IPDR_SocketHandle *S)
{
    int On = 1; /* this must be an int and SO_REUSEADDR must take an int (not unsigned char) */
	int KeepAlive = -1; /* same as 1: but this sets all the bits in case arg isn't an int */

    S->socketFd = socket(PF_INET, SOCK_STREAM, 0);
    if (! IPDR_IsValidSocket(*S)) {
        return IPDR_ERR_COMMUNICATION_ERROR;
    }

#if defined(VXWORKS) || defined(_VXWORKS)
#else
    if (0) { /* Assume that the socket is only for accepting connections. */
        unsigned long buflen = 256*1024; /* max on Solaris */
        if (setsockopt(S->socketFd, SOL_SOCKET, SO_RCVBUF, (const void*)&buflen, sizeof buflen) < 0) {
            LOG_WRITE2(E, IPDR_LEVEL_WARNING, "Couldn't set receive buflen(%lu) for socket " Platform_SOCKET_FMT "\n", buflen, Platform_SOCKET_DISPLAY(*S));
        }
    }
    if (1) { /* Don't need a big buffer for receiving ACKs */
        unsigned long buflen = 256*1024; /* max on Solaris */
        if (setsockopt(S->socketFd, SOL_SOCKET, SO_SNDBUF, (const void*)&buflen, sizeof buflen) < 0) {
            LOG_WRITE2(E, IPDR_LEVEL_WARNING, "Couldn't set send buflen(%lu) for socket " Platform_SOCKET_FMT "\n", buflen, Platform_SOCKET_DISPLAY(*S));
        }
    }
#endif

    if (setsockopt(S->socketFd, SOL_SOCKET, SO_KEEPALIVE, (const void*)&KeepAlive, sizeof KeepAlive) < 0) {
        LOG_WRITE2(E, IPDR_LEVEL_WARNING, "Couldn't set accept receive keepalive(%d) for socket " Platform_SOCKET_FMT "\n", KeepAlive, Platform_SOCKET_DISPLAY(*S));
    }

    if (setsockopt(S->socketFd, SOL_SOCKET, SO_REUSEADDR,
                   (char *) & On, sizeof (On)) < 0) {
#ifndef _MSC_VER
        close(S->socketFd);
#else
        closesocket(S->socketFd);
#endif
        return IPDR_ERR_COMMUNICATION_ERROR;
    }

    return IPDR_OK;
}

static
IPDR_Status GenericPlatform_CreateUDPSocket(IPDR_PlatformArg P,
                                             IPDR_SocketHandle *S)
{
    S->socketFd = socket(PF_INET, SOCK_DGRAM, 0);

    if (! IPDR_IsValidSocket(*S)) {
        return IPDR_ERR_COMMUNICATION_ERROR;
    }

    return IPDR_OK;
}

static
IPDR_Status GenericPlatform_Connect(IPDR_PlatformArg P,
                                  IPDR_SocketHandle S, IPDR_IPAddress LocalAddr,
                                  unsigned short LocalPort)
{
    struct sockaddr_in Addr;

    memset(& Addr, sizeof Addr, 0);
    Addr.sin_family = AF_INET;
    Addr.sin_port = htons(LocalPort);
    Addr.sin_addr.s_addr = htonl(LocalAddr);

	errno = 0;
	if (connect(S.socketFd, (struct sockaddr *) & Addr, sizeof Addr) < 0)
	{
        TL_PABuf paBuf;
        LOG_WRITE3(P, IPDR_LEVEL_ERROR, "Connect %.99s errno=%d: %.99s\n",
                   TL_PrintAddressAndPort(LocalAddr, LocalPort, paBuf), errno, strerror(errno));
        switch (errno) {
        default:
            return IPDR_ERR_COMMUNICATION_ERROR;
        }
	}

	return IPDR_OK;
}

static
IPDR_Status GenericPlatform_GetSockName(IPDR_PlatformArg P,
                                  IPDR_SocketHandle S, IPDR_IPAddress *LocalAddr,
                                  unsigned short *LocalPort)
{
    struct sockaddr_in Addr;
	struct sockaddr * pAddr = (struct sockaddr *)&Addr;
	socklen_t addrLen = sizeof( Addr );

    memset(pAddr, addrLen, 0);

	*LocalAddr = 0;
	*LocalPort = 0;

	errno = 0;
	if (getsockname(S.socketFd, pAddr, &addrLen) < 0)
	{
        LOG_WRITE2(P, IPDR_LEVEL_ERROR, "getsockname, errno=%d: %.99s\n",
                   errno, strerror(errno));
        switch (errno) {
        default:
            return IPDR_ERR_COMMUNICATION_ERROR;
        }
	}

	*LocalAddr = ntohl(Addr.sin_addr.s_addr);
	*LocalPort = ntohs(Addr.sin_port);

	return IPDR_OK;
}

static
IPDR_Status GenericPlatform_Bind(IPDR_PlatformArg P,
                                  IPDR_SocketHandle S, IPDR_IPAddress LocalAddr,
                                  unsigned short LocalPort)
{
    struct sockaddr_in Addr;

    memset(& Addr, sizeof Addr, 0);
    Addr.sin_family = AF_INET;
    Addr.sin_port = htons(LocalPort);
    Addr.sin_addr.s_addr = htonl(LocalAddr);

    errno = 0;
    if (bind(S.socketFd, (struct sockaddr *) & Addr, sizeof Addr) < 0) {
        TL_PABuf paBuf;
        LOG_WRITE3(P, IPDR_LEVEL_ERROR, "Bind %.99s errno=%d: %.99s\n",
                   TL_PrintAddressAndPort(LocalAddr, LocalPort, paBuf), errno, strerror(errno));
        switch (errno) {
        case EADDRINUSE:
        case EADDRNOTAVAIL:
        case EACCES:
        case EINVAL:
            return (LocalAddr == 0) ? IPDR_ERR_PORT_ALREADY_IN_USE : IPDR_ERR_ADDRESS_ALREADY_IN_USE;
        default:
            return IPDR_ERR_COMMUNICATION_ERROR;
        }
    }

    return IPDR_OK;
}

static
IPDR_Status GenericPlatform_Close(IPDR_PlatformArg P, IPDR_SocketHandle S)
{
    /* From the Solaris manual (your mileage might vary on other platforms, even BSD):
     *
     * SO_LINGER controls the action taken when unsent messages are queued
     * on a socket and a close(2) is performed. If the socket promises
     * reliable delivery of data and SO_LINGER is set, the system will
     * block the process on the close() attempt until it is able to
     * transmit the data or until it decides it is unable to deliver the
     * information (a timeout period, termed the linger interval, is
     * specified in the set- sockopt() call when SO_LINGER is
     * requested). If SO_LINGER is disabled and a close() is issued, the
     * system will process the close() in a manner that allows the process
     * to continue as quickly as possible.  */

#if 0
    /************* Don't do "linger" at all *******************/

    struct linger lngr;

#if defined(_MSC_VER) || defined(VXWORKS) || defined(_VXWORKS)
#error Verify value of linger parameter
#else
    lngr.l_linger = 1; /* one second on Solaris; might be different on other platforms */
    lngr.l_onoff = 1;
#endif

    if (! IPDR_IsValidSocket(S)) {
        return IPDR_OK;
    }

    setsockopt(S.socketFd, SOL_SOCKET, SO_LINGER, (void *) &lngr, sizeof(struct linger));

#else /* if 0 for "linger" */
	if (! IPDR_IsValidSocket(S))
	{
        return IPDR_OK;
    }
#endif /* if 0 for "linger" */

#ifdef TRACE_MAINFUNC
    fprintf(stderr, "*** GenericMPlatform_Close " Platform_SOCKET_FMT " close %.150s:%d\n",
            Platform_SOCKET_DISPLAY(S), __FILE__, __LINE__); fflush(stderr);
#endif

#ifndef _MSC_VER
    close(S.socketFd);
#else
    closesocket(S.socketFd);
#endif

    return IPDR_OK;
}

static
IPDR_Status GenericPlatform_Listen(IPDR_PlatformArg P,
                                    IPDR_SocketHandle S, int Backlog)
{
    if (listen(S.socketFd, Backlog) < 0) {
        return IPDR_ERR_COMMUNICATION_ERROR;
    }

    return IPDR_OK;
}

static
IPDR_Status GenericPlatform_Accept(IPDR_PlatformArg E,
                                    IPDR_SocketHandle S, IPDR_IPAddress *Addr,
                                    unsigned short *Port, IPDR_SocketHandle *AcceptSocket)
{
    struct sockaddr_in SockAddr;
    int Len;
    int KeepAlive = -1; /* same as 1: but this sets all the bits in case arg isn't an int */

    Len = sizeof(SockAddr);

    AcceptSocket->socketFd = accept(S.socketFd, (struct sockaddr *) & SockAddr, & Len);
    if (! IPDR_IsValidSocket(*AcceptSocket)) {
        return IPDR_ERR_COMMUNICATION_ERROR;
    }

#if defined(VXWORKS) || defined(_VXWORKS)
#else
    if (1) {
        unsigned long buflen = 256*1024; /* max on Solaris */
        if (setsockopt(AcceptSocket->socketFd, SOL_SOCKET, SO_SNDBUF, (const void*)&buflen, sizeof buflen) < 0) {
            LOG_WRITE2(E, IPDR_LEVEL_WARNING, "Couldn't set accept send buflen(%lu) for socket " Platform_SOCKET_FMT "\n", buflen, Platform_SOCKET_DISPLAY(*AcceptSocket));
        }
    }
    if (0) { /* Don't need a big buffer for receiving ACKs */
        unsigned long buflen = 256*1024; /* max on Solaris */
        if (setsockopt(AcceptSocket->socketFd, SOL_SOCKET, SO_RCVBUF, (const void*)&buflen, sizeof buflen) < 0) {
            LOG_WRITE2(E, IPDR_LEVEL_WARNING, "Couldn't set accept receive buflen(%lu) for socket " Platform_SOCKET_FMT "\n", buflen, Platform_SOCKET_DISPLAY(*AcceptSocket));
        }
    }
#endif
    if (setsockopt(AcceptSocket->socketFd, SOL_SOCKET, SO_KEEPALIVE, (const void*)&KeepAlive, sizeof KeepAlive) < 0) {
        LOG_WRITE2(E, IPDR_LEVEL_WARNING, "Couldn't set accept receive keepalive(%d) for socket " Platform_SOCKET_FMT "\n", KeepAlive, Platform_SOCKET_DISPLAY(*AcceptSocket));
    }


    *Port = ntohs(SockAddr.sin_port);
    *Addr = ntohl(SockAddr.sin_addr.s_addr);

    return IPDR_OK;
}

static
IPDR_Status GenericPlatform_SetExporterSocketParams(IPDR_PlatformArg E,
                                                   IPDR_SocketHandle S,
                                                   IPDR_ParametersHandlerFunc ParamFunc,
                                                   void *ParamArg)
{
    /* ****** WARNING WARNING WARNING WARNING WARNING WARNING **********
     *                                                                 *
     * You *must* set the socket to non-blocking mode for this         *
     * implementation of the platform code. If you do not, chances are *
     * good that the system will eventually "hang" on a read.          *
     *                                                                 *
     * ****** WARNING WARNING WARNING WARNING WARNING WARNING **********/

#ifndef _MSC_VER
#if defined(VXWORKS) || defined(_VXWORKS)
    int nonBlockingMode = -1; /* same as 1: but this sets all the bits in case arg isn't an int */
    if (ioctl(S.socketFd, FIONBIO, (int)&nonBlockingMode) < 0) { /* The (int) cast is for VxWork's broken header */
        return IPDR_ERR_COMMUNICATION_ERROR;
    }
#else
    int Flags = fcntl(S.socketFd, F_GETFL, 0);
#ifdef TRACE_MAINFUNC
    fprintf(stderr, "SetSocketParams: non-blocking " Platform_SOCKET_FMT "\n", Platform_SOCKET_DISPLAY(S)); fflush(stderr);
#endif
    if (Flags < 0) {
        return IPDR_ERR_COMMUNICATION_ERROR;
    }
    if (fcntl(S.socketFd, F_SETFL, Flags | O_NONBLOCK) < 0) {
        return IPDR_ERR_COMMUNICATION_ERROR;
    }
#endif
#endif

    return IPDR_OK;
}

static
IPDR_Status GenericPlatform_Send(IPDR_PlatformArg E, IPDR_SocketHandle S,
        const char *Buf, unsigned int BufLength,
        unsigned int *WrittenBytes)
{
    int RC = 0;
    *WrittenBytes = 0;

/*#define TESTING_SLOW_SEND 1*/
#ifdef TESTING_SLOW_SEND
    /* Slow send is for testing that the collector can handle the
     * message arriving in tiny bits with long timeouts. We send a
     * random amount at a time, then sleep for a period that is
     * hopefully greater than the Nagle algorithm's wait on the
     * buffer. */
    static int haveSetRand = 0;
    if (! haveSetRand) { srand(time(0)); }

    while (*WrittenBytes < BufLength) {
        int amountToWrite;
        amountToWrite = (int)((BufLength - *WrittenBytes) * (double)rand() / (double)RAND_MAX);
        if (amountToWrite < 5) { amountToWrite = 5; }
        if (amountToWrite > 100) { amountToWrite = 100; }
        if (amountToWrite > (BufLength - *WrittenBytes)) { amountToWrite = (BufLength - *WrittenBytes); }
        if (amountToWrite <= 0) { amountToWrite = 1; }
        errno = 0;
        fprintf(stderr, "... sending %d bytes of %d\n", amountToWrite, BufLength);
        RC = send(S.socketFd, Buf + *WrittenBytes, amountToWrite, 0);
        if (RC < 0 && errno == EINTR) {
            LOG_WRITE(E, IPDR_LEVEL_WARNING, "send() got EINTR");
            continue;
        }
        if (RC < 0) {
            return ERRNO_EWOULDBLOCK ? IPDR_ERR_OPERATION_WOULD_BLOCK : IPDR_ERR_COMMUNICATION_ERROR;     }
        *WrittenBytes += RC;
        if (RC < amountToWrite) {
            return IPDR_ERR_OPERATION_WOULD_BLOCK;
        }
        GenericPlatform_SleepMS(E, 100);
    }
    fprintf(stderr, "... finished sending %d bytes\n", BufLength);
    return IPDR_OK;
#else
    for (;;) { /* send() until we don't get EINTR result */
        errno = 0;
        RC = send(S.socketFd, Buf, BufLength, 0);
        if (RC < 0 && errno == EINTR) {
            LOG_WRITE(E, IPDR_LEVEL_WARNING, "send() got EINTR");
            continue;
        }
        break; /* Succeeded or got non-EINTR error */
    }

    if (RC < 0) {
        return ERRNO_EWOULDBLOCK ? IPDR_ERR_OPERATION_WOULD_BLOCK : IPDR_ERR_COMMUNICATION_ERROR;
    } else {
        *WrittenBytes = RC;
        return (RC < BufLength) ? IPDR_ERR_OPERATION_WOULD_BLOCK : IPDR_OK;
    }
#endif
}

static
IPDR_Status GenericPlatform_Recv(IPDR_PlatformArg E, IPDR_SocketHandle S,
                                  char *Buf, unsigned int BufLength, int Timeout,
                                  unsigned int *ReadBytes)
{
    int recvRC;

    *ReadBytes = 0;

#ifdef TRACE_MAINFUNC
    LOG_WRITE3(E, IPDR_LEVEL_DEBUG,
               "---GenericPlatform_Recv(bufLen=%ud timeout=%d socket=" Platform_SOCKET_FMT,
               BufLength, Timeout, Platform_SOCKET_DISPLAY(S));
#endif

    /* if a timeout is specified, do a poll or select until the socket is readable */
    if (Timeout > 0) {
        /* The following code assumes that we're in non-blocking
           mode ... if not, strange things could happen. */
#if 1 || defined(VXWORKS) /* implement this with select() for all platforms, to make sure it works ...
                             but poll() is better for non-VxWorks */
struct timeval timeout;
int selectRC;
fd_set FDSet;
FD_ZERO(& FDSet);
FD_SET(S.socketFd, & FDSet);
timeout.tv_sec = Timeout / 1000;
timeout.tv_usec = (Timeout % 1000) * 1000;
for (;;) { /* select() until we don't get EINTR result */
    errno = 0;
    selectRC = select(S.socketFd + 1, & FDSet, NULL, NULL, & timeout);
    if (selectRC < 0 && errno == EINTR) {
        LOG_WRITE(E, IPDR_LEVEL_WARNING, "select(Recv) got EINTR");
        continue;
    }
    break; /* Succeeded or got non-EINTR error */
}
if (selectRC == 0 || ! FD_ISSET(S.socketFd, & FDSet)) {
#ifdef TRACE_MAINFUNC
    LOG_WRITE6(E, IPDR_LEVEL_DEBUG,
               "---block: GenericPlatform_Recv(bufLen=%u timeout=%d socket=" Platform_SOCKET_FMT " rc=%d set=%ld errno=%d",
               BufLength, Timeout, Platform_SOCKET_DISPLAY(S),
               selectRC, (long)FD_ISSET(S.socketFd, & FDSet), errno);
#endif
    return IPDR_ERR_OPERATION_WOULD_BLOCK;
}
if (selectRC < 0) {
    LOG_WRITE6(E, IPDR_LEVEL_WARNING,
               "...error: GenericPlatform_Recv(bufLen=%u timeout=%d socket=" Platform_SOCKET_FMT " rc=%d set=%ld errno=%d",
               BufLength, Timeout, Platform_SOCKET_DISPLAY(S),
               selectRC, (long)FD_ISSET(S.socketFd, & FDSet), errno);
    return ERRNO_EWOULDBLOCK ? IPDR_ERR_OPERATION_WOULD_BLOCK : IPDR_ERR_COMMUNICATION_ERROR;
}

#ifdef TRACE_MAINFUNC
LOG_WRITE6(E, IPDR_LEVEL_DEBUG,
           "...recv:  GenericPlatform_Recv(bufLen=%u timeout=%d socket=" Platform_SOCKET_FMT " rc=%d set=%ld errno=%d",
           BufLength, Timeout, Platform_SOCKET_DISPLAY(S),
           selectRC, (long)FD_ISSET(S.socketFd, & FDSet), errno);
#endif
#else
{
    struct pollfd PFD;
    int pollRC;
    PFD.fd = S;
    PFD.events = POLLIN;
    for (;;) { /* poll() until we don't get EINTR resut */
        errno = 0;
        PFD.revents = 0; /* probably not needed, but let's be safe */
        pollRC = poll(& PFD, 1, Timeout);
        if (pollRC < 0 && errno == EINTR) {
            LOG_WRITE(E, IPDR_LEVEL_WARNING, "poll(Recv) got EINTR");
            continue;
        }
        break; /* Succeeded or got non-EINTR error */
    }
    if (pollRC == 0) {
        return IPDR_ERR_OPERATION_WOULD_BLOCK;
    }
    if (pollRC < 0) {
        return ERRNO_EWOULDBLOCK ? IPDR_ERR_OPERATION_WOULD_BLOCK : IPDR_ERR_COMMUNICATION_ERROR;
    }
    if (PFD.revents & (POLLERR|POLLHUP|POLLNVAL)) {
        return IPDR_ERR_COMMUNICATION_ERROR;
    }
}
#endif
    } /* end of timeout code */

    for (;;) { /* recv() until we don't get EINTR result */
        errno = 0;
        recvRC = recv(S.socketFd, Buf, BufLength, 0);
        if (recvRC < 0 && errno == EINTR) {
            LOG_WRITE(E, IPDR_LEVEL_WARNING, "recv() got EINTR");
            continue;
        }
        break; /* Succeeded or got non-EINTR error */
    }

#ifdef TRACE_MAINFUNC
    LOG_WRITE5(E, IPDR_LEVEL_DEBUG,
               "+++recv:  GenericPlatform_Recv(bufLen=%u timeout=%d socket=" Platform_SOCKET_FMT " rc=%d errno=%d",
               BufLength, Timeout, Platform_SOCKET_DISPLAY(S),
               recvRC, errno);
#endif

    if (recvRC == 0) { /* end-of-file */
        return IPDR_ERR_COMMUNICATION_ERROR;
    }

    if (recvRC < 0) {
        *ReadBytes = 0;
        if (ERRNO_EWOULDBLOCK) {
            return IPDR_ERR_OPERATION_WOULD_BLOCK;
        } else {
            return IPDR_ERR_COMMUNICATION_ERROR;
        }
    }

    *ReadBytes = recvRC;
    return IPDR_OK;
}

static
IPDR_Status GenericPlatform_RecvFrom(IPDR_PlatformArg E, IPDR_SocketHandle S,
                                      char *Buf, unsigned int BufferLength,
                                      IPDR_IPAddress *RemoteAddr, unsigned short *RemotePort,
                                      unsigned int *ReadBytes)
{
    struct sockaddr_in Addr;
    int AddrLen = sizeof(Addr);
    int rc;

    *ReadBytes = 0;

    rc = recvfrom(S.socketFd,  Buf, BufferLength, 0,
                  (struct sockaddr *) & Addr, & AddrLen);

#ifdef TRACE_MAINFUNC
    fprintf(stderr, "*** GenericPlatformRecvfrom(timeout=%d) rc=%d\n", RECEIVE_TIMEOUT, rc);
#endif
    if (rc == 0) {
        return IPDR_ERR_OPERATION_WOULD_BLOCK;
    }
    if (rc < 0) {
        return IPDR_ERR_COMMUNICATION_ERROR;
    }

    *ReadBytes = rc;
    *RemotePort = ntohs(Addr.sin_port);
    *RemoteAddr = ntohl(Addr.sin_addr.s_addr);

    return IPDR_OK;
}

static
IPDR_Status GenericPlatform_SendTo(IPDR_PlatformArg P, IPDR_SocketHandle S,
                                    const char *Buf, unsigned int BufferLength,
                                    IPDR_IPAddress RemoteAddr, unsigned short RemotePort)
{
    struct sockaddr_in Addr;

    memset(& Addr, sizeof(Addr), 0);
    Addr.sin_family = AF_INET;
    Addr.sin_port = htons(RemotePort);
    Addr.sin_addr.s_addr = htonl(RemoteAddr);

    if (sendto(S.socketFd, CONST_CAST_CHARPTR(Buf), BufferLength, 0,
               (struct sockaddr *) & Addr, sizeof(Addr)) < 0)
        return IPDR_ERR_COMMUNICATION_ERROR;

    return IPDR_OK;
}

static
IPDR_Status GenericPlatform_Shutdown(IPDR_PlatformArg P, IPDR_SocketHandle S)
{
    /* If it turns out that linger is needed, then need to add it here
       (I think). See code in GenericPlatform_Close. */

    int rc = shutdown(S.socketFd, 2); /* shutdown both read and write */

    return (rc == 0) ? IPDR_OK : IPDR_ERR_COMMUNICATION_ERROR;
}

static
IPDR_Status GenericPlatform_GetHostName(IPDR_PlatformArg P, char* HostName, int NameLength)
{
    errno = 0;
    /* Implementation note: if you're on a platform that doesn't have
     * gethostname() or similar, try to get an IP address (convert to
     * character string with inet_ntoa()), otherwise just hard-code a
     * result. */
    if (NameLength <= 0) {
        return IPDR_ERR_INVALID_VALUE;
    } else if (0 == gethostname(HostName, NameLength)) {
        HostName[NameLength - 1] = '\0'; /* ensure it's null terminated */
        return IPDR_OK;
    } else {
        /* errno will have been set */
        HostName[0] = '\0'; /* empty string */
        return IPDR_ERR;
    }

    /* If we need it, here is generic code for resolving a name to an
     * IP address (for Unix): */
#if 0
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

    if ((int)(addr = inet_addr(hostname)) == -1) {
        hp = gethostbyname(hostname);
    } else {
        hp = gethostbyaddr((char *)&addr, sizeof (addr), AF_INET);
    }
    if (hp == NULL) {
        (void) printf("host information for %s not found\n", hostname);
        return 3;
    }
    for (p = hp->h_addr_list; *p != 0; p++) {
        struct in_addr in;
        char **q;
        (void) memcpy(&in.s_addr, *p, sizeof (in.s_addr));
        (void) printf("%s\t%s", inet_ntoa(in), hp->h_name);
        for (q = hp->h_aliases; *q != 0; q++)
            (void) printf(" %s", *q);
        (void) putchar('\n');
    }
#endif
}

/* ---------------------- LockingServices ------------------------- */

static
IPDR_Status GenericPlatform_CreateLock(IPDR_PlatformArg E, void **LockPtr)
{
    GenericLock* lock;
    {
        IPDR_Status Status = GenericPlatform_MemoryAllocate(E, sizeof(GenericLock), (void**)&lock);
        *LockPtr = lock;
        if (FAILURE(Status)) {
            return Status;
        }
    }

    strncpy(lock->eyecatcher, "LOCKRCR", sizeof lock->eyecatcher);

#if defined(MT_SUPPORT)
#if defined(PTHREADS_SUPPORT)
    pthread_mutex_init(& lock->ActualLock, NULL);
    pthread_mutex_init(& lock->CountLock, NULL);
    lock->RecursionCount = 0;
#elif defined(_MSC_VER)
    InitializeCriticalSection(& lock->ActualLock);
    InitializeCriticalSection(& lock->CountLock);
    lock->RecursionCount = 0;
#elif defined(VXWORKS) || defined(_VXWORKS)
    lock->ActualLock = semMCreate(SEM_Q_PRIORITY | SEM_DELETE_SAFE);
    if (lock->ActualLock == NULL) {
        GenericPlatform_MemoryFree(E, lock);
        return IPDR_ERR;
    }
    /* Recursive mutex, so no need for CountLock */
#else
#error Define CreateLock
#endif
#endif

    return IPDR_OK;
}

static
IPDR_Status GenericPlatform_DeleteLock(IPDR_PlatformArg E, void *LockPtr)
{
    GenericLock* lock = (GenericLock*)LockPtr;
    strncpy(lock->eyecatcher, "Xlockrc", sizeof lock->eyecatcher);

#if defined(MT_SUPPORT)
#if defined(PTHREADS_SUPPORT)
    pthread_mutex_destroy(& lock->ActualLock);
    pthread_mutex_destroy(& lock->CountLock);
#elif defined( _MSC_VER)
    DeleteCriticalSection(& lock->ActualLock);
    DeleteCriticalSection(& lock->CountLock);
#elif defined(VXWORKS) || defined(_VXWORKS)
    semDelete(lock->ActualLock);
    /* No countLock */
#else
#error Define DeleteLock
#endif
#endif

    GenericPlatform_MemoryFree(E, lock);

    return IPDR_OK;
}

#ifdef DEBUG_LOCKS

IPDR_Status GenericPlatform_db_LockPlatform(IPDR_PlatformArg E, const char* FileName, int LineNum)
{
    IPDR_Status Status = GenericPlatform_db_Lock(E, E->PlatformLock, FileName, LineNum);
    if (FAILURE(Status)) {
        LOG_WRITE(E, IPDR_LEVEL_FATAL, "Platform Lock failed (%.150s:%d): rc=%d", rc, FileName, LineNum);
    }
    return rc;
}

IPDR_Status GenericPlatform_db_UnlockPlatform(IPDR_PlatformArg E, const char* FileName, int LineNum)
{
    IPDR_Status Status = GenericPlatform_db_Unlock(E, E->PlatformLock, FileName, LineNum);
    if (FAILURE(Status)) {
        LOG_WRITE(E, IPDR_LEVEL_FATAL, "Platform Lock failed (%.150s:%d): rc=%d\n", rc, FileName, LineNum);
    }
    return rc;
}

#else

static
IPDR_Status GenericPlatform_LockPlatform(IPDR_PlatformArg E)
{
    IPDR_Status Status = GenericPlatform_Lock(E, E->PlatformLock);
    if (FAILURE(Status)) {
        LOG_WRITE1(E, IPDR_LEVEL_FATAL, "Platform Lock failed: rc=%d\n", Status);
    }
    return Status;
}

static
IPDR_Status GenericPlatform_UnlockPlatform(IPDR_PlatformArg E)
{
    IPDR_Status Status = GenericPlatform_Unlock(E, E->PlatformLock);
    if (FAILURE(Status)) {
        LOG_WRITE1(E, IPDR_LEVEL_FATAL, "Platform Unlock failed: rc=%d\n", Status);
    }
    return Status;
}

#endif

#ifdef DEBUG_LOCKS
static
IPDR_Status GenericPlatform_db_Lock(IPDR_PlatformArg P, GenericLock *LockPtr,
                                     const char* FileName, int LineNum)
#else
    static
IPDR_Status GenericPlatform_Lock(IPDR_PlatformArg E, void *LockPtr)
#endif
{
#if defined(MT_SUPPORT)
    GenericLock* lock = (GenericLock*)LockPtr;
#if defined(PTHREADS_SUPPORT)
    /* The following implements a "recursive" lock, which doesn't
       deadlock if a thread tries to get the lock when the same thread
       has already acquired the lock (it'll block if another thread has
       the lock). This is accomplished by having a "use count" protected
       by its mutex in addition to the locking mutex. */
    /* To do: the non-recursive POSIX code and the Windows code are almost
       identical ... combine them into one. */
    /* To do: convert the following to use POSIX threads recursive option:
       ( see http://www.as400.ibm.com/developer/threads/uguide/users-55.htm#307637 )
       pthread_mutex_t mutex;
       pthread_mutexattr_t mta;
       pthread_mutexattr_init(&mta);
       pthread_mutexattr_setkind_np(&mta, PTHREAD_MUTEX_RECURSIVE_NP);
       pthread_mutex_init(&mutex, &mta);
    */
    int AcquiredLock = 0; /* flag is set to true when the lock is obtained */
#ifdef DEBUG_LOCKS
    fprintf(stderr, "Lock try [%.150s:%d](0x%p) count=%d owner=%d self=%d\n",
            FileName, LineNum, lock, lock->RecursionCount, lock->Owner, THREAD_SELF()); fflush(stderr);
#endif
    while (! AcquiredLock) {
        pthread_mutex_lock(& lock->CountLock); /* lock the counter */
#ifdef DEBUG_LOCKS
        fprintf(stderr, "Lock on  [%.150s:%d](0x%p) count=%d owner=%d self=%d\n",
                FileName, LineNum, lock, lock->RecursionCount, lock->Owner, THREAD_SELF()); fflush(stderr);
#endif
        /* RecursionCount is the number of times the platform mutex has
           been acquired by a single thread. If RecursionCount is zero,
           the mutex hasn't been acquired by anybody. The CountLock mutex
           protects RecursionCount. */
        /* Getting a mutex is:
           pthread_mutex_lock(mutex)   for POSIX threads
           EnterCriticalSection(lock)  for Windows
           semTake(lock, WAIT_FOREVER) for VxWorks
           Getting the current thread ID is:
           pthread_self()              for POSIX threads
           GetCurrentThreadId()        for Windows
           ???                         for VxWorks
        */
        if (lock->RecursionCount <= 0) {            /* Nobody else has this lock */
            if (lock->RecursionCount < 0) {
                /* This is a serious logic error and indicates that somebody
                   has done an extra Unlock(). */
                LOG_WRITE1(E, IPDR_LEVEL_FATAL, "lock RecursionCount=%d\n", lock->RecursionCount);
            }
            lock->RecursionCount = 1;                 /* We have it now */
            lock->Owner = pthread_self();             /* And remember our thread */
            AcquiredLock = 1;                         /* We'll exit the loop */
            pthread_mutex_lock(& lock->ActualLock);   /* Get the critical section mutex */
        } else if (pthread_equal(lock->Owner, pthread_self())) { /* We already have this lock */
            lock->RecursionCount++;                   /* Increment the counter */
            AcquiredLock = 1;                         /* We'll exit the loop */
        }
        pthread_mutex_unlock(& lock->CountLock);
        if (! AcquiredLock) {
            /* Arrive here only if somebody else has the lock */
            /* Wait until the other person has released the platform lock,
               so that we don't go into a tight loop looking at the count */
            pthread_mutex_lock(& lock->ActualLock);
            /* And unlock the platform lock, and loop. Somebody else might
               get in here in between, but that'll just be an extra unneeded
               loop, which we can't prevent anyway.  */
            pthread_mutex_unlock(& lock->ActualLock);
        }
    }
#elif defined(_MSC_VER)
#error _MSC_VER Lock should be integrated with VxWorks, pthreads versions
    int AcquiredLock = 0;
    while (! AcquiredLock) {
        EnterCriticalSection(& (U->CountLock));
        if (! U->RecursionCount) {
            U->RecursionCount = 1;
            U->Owner = GetCurrentThreadId();
            AcquiredLock = 1;
            EnterCriticalSection(& (U->ActualLock));
        } else if (U->Owner == GetCurrentThreadId()) {
            U->RecursionCount++;
            AcquiredLock = 1;
        }
        LeaveCriticalSection(&(U->CountLock));
        if (! AcquiredLock) {
            EnterCriticalSection(& (U->ActualLock));
            LeaveCriticalSection(& (U->ActualLock));
        }
    }
#elif defined(VXWORKS) || defined(_VXWORKS)
    if (ERROR == semTake(lock->ActualLock, WAIT_FOREVER)) {
        /* will get ERROR only if task timed out, which won't happen with WAIT_FOREVER */
        taskSuspend(0);
    }
#else
#error Defined Lock()
#endif
#endif /* MT_SUPPORT */

    return IPDR_OK;
}

#ifdef DEBUG_LOCKS
static
IPDR_Status GenericPlatform_db_Unlock(IPDR_PlatformArg E, GenericLock *LockPtr,
                                       const char* FileName, int LineNum)
#else
    static
IPDR_Status GenericPlatform_Unlock(IPDR_PlatformArg E, void *LockPtr)
#endif
{
#if defined(MT_SUPPORT)
    GenericLock* lock = (GenericLock*)LockPtr;
#if defined(PTHREADS_SUPPORT)
    /* See the commentary for GenericPlatform_Lock */
    /* We obtain the mutex on the count and decrement it.
       If the count goes down to zero, we no longer need
       the main lock, so we release it. */
    pthread_mutex_lock(& lock->CountLock);
#ifdef DEBUG_LOCKS
    fprintf(stderr, "Lock off [%.150s:%d](0x%p) count=%d owner=%d self=%d\n",
            FileName, LineNum, lock, lock->RecursionCount, lock->Owner, pthread_self()); fflush(stderr);
#endif
    --(lock->RecursionCount);
    if (! lock->RecursionCount) {
        lock->Owner = pthread_self();
        pthread_mutex_unlock(& lock->ActualLock);
    }
    pthread_mutex_unlock(& lock->CountLock);
#elif defined(_MSC_VER)
#error _MSC_VER _Unlock should be integrated with VxWorks, pthreads versions
    EnterCriticalSection(&(U->CountLock));
    --(U->RecursionCount);
    if (! U->RecursionCount) {
        U->Owner = GetCurrentThreadId();
        LeaveCriticalSection(&(U->ActualLock));
    }
    LeaveCriticalSection(&(U->CountLock));
#elif defined(VXWORKS) || defined(_VXWORKS)
    if (ERROR == semGive(lock->ActualLock)) { /* Or: OK!=... */
        taskSuspend(0);
    }
#else
#error Defined Unlock()
#endif
#endif /* MT_SUPPORT */

    return IPDR_OK;
}

/* ---------------------- TimeServices ------------------------- */

static
IPDR_Status GenericPlatform_GetTime(IPDR_PlatformArg P, IPDR_Time *T) {
#ifdef _MSC_VER
    struct _timeb tb;
    _ftime(& tb);
    *T = tb.time;
    return IPDR_OK;
/* note: cygwin doesn't have clock_gettime() ... you can
   check for defined(__unix) and do something with it ...
   for example gettimeofday() or ftime()
*/
#elif defined(SOLARIS) || defined(VXWORKS) || defined(_VXWORKS)
    struct timespec ts;
    int rc = clock_gettime(CLOCK_REALTIME, &ts);
    *T = ts.tv_sec;
    return (rc == 0) ? IPDR_OK : IPDR_ERR;
#else
/* this also works on Solaris, but we'll use the same as VxWorks: */
    struct timeval tv;
    int rc;
    rc = gettimeofday(& tv, NULL);
    *T = tv.tv_sec;
    return (rc == 0) ? IPDR_OK : IPDR_ERR;
/* or this (generic Unix): */
/*    time_t tb = time(0); */
/*    *T = tb; */
/*    return (tb == (time_t)-1) ? IPDR_ERR : IPDR_OK; */
#endif
}

static
IPDR_Status GenericPlatform_SetTime(IPDR_PlatformArg P, IPDR_Time T) {
#ifdef _MSC_VER
    errno = ENOTSUP;
    return IPDR_ERR;
#elif defined(SOLARIS) || defined(VXWORKS) || defined(_VXWORKS)
    struct timespec ts;
    int rc;
    ts.tv_sec = T;
    ts.tv_nsec = 0;
    errno = 0;
    rc = clock_settime(CLOCK_REALTIME, &ts);
    return (rc == 0) ? IPDR_OK : IPDR_ERR;
#else
/* this also works on Solaris, but we'll use the same as VxWorks: */
    struct timeval ts;
    int rc;
    ts.tv_sec = T;
    ts.tv_usec = 0;
    errno = 0;
    rc = settimeofday(&ts, NULL);
    return (rc == 0) ? IPDR_OK : IPDR_ERR;
#endif
}

static
IPDR_Status GenericPlatform_SleepMS(IPDR_PlatformArg P, long interval)
{
    if (interval > 0) {
#ifdef _MSC_VER
        Sleep(interval);
#else
/*#eif	SOLARIS*/
        struct timeval TV;
        TV.tv_sec = interval / 1000;
        TV.tv_usec = 1000 * (interval % 1000);
		/*fprintf(stderr, "############# GenericPlatform_SleepMS (select) 1\n"); fflush(stderr);*/
        select(0, NULL, NULL, NULL, & TV);
		/*fprintf(stderr, "############# GenericPlatform_SleepMS (select) 2\n"); fflush(stderr);*/
/*#else
		fprintf(stderr, "############# GenericPlatform_SleepMS (usleep) 1\n"); fflush(stderr);
		usleep(1000 * interval);
		fprintf(stderr, "############# GenericPlatform_SleepMS (usleep) 2\n"); fflush(stderr);*/
#endif
    }
    return IPDR_OK;
}

/* The following time formatting routines do the equivalent of:
   struct tm* tmPtr = localtime(c);
   strftime(timeStrBuf, sizeof timeStrBuf, "%Y-%m-%dT%H:%M:%S", tmPtr);
   typical use:
   char timeStrBuf[IPDR_TIME_FORMAT_LENGTH + 1]; !* +1 for "Z" *!
   Platform_FormatTimeGMTNow(EXPORTER_PLATFORM, timeStrBuf);
   strcat(timeStrBuf, "Z"); !* mark it as "GMT" *!
*/

static
char* FormatTime(char* buf, struct tm* tmPtr)
{
    /* It would be nice to use strftime; but it appears to cause problems
       on some systems, so use sprintf */
    sprintf(buf, "%04d-%02d-%02dT%02d:%02d:%02d",
            1900 + tmPtr->tm_year, 1 + tmPtr->tm_mon, tmPtr->tm_mday,
            tmPtr->tm_hour, tmPtr->tm_min, tmPtr->tm_sec);
    return buf;
}

static
char* GenericPlatform_FormatTimeLocal(IPDR_PlatformArg P, IPDR_Time c, char* buf)
{
    /* POSIX and VxWorks differ in their definition of localtime_r:
       POSIX returns struct tm* pointer but VxWorks returns an int. We
       depend on this function simply filling the buffer and ignore the
       return value.  */
    struct tm tmMem;
    /* On VxWorks, time_t is not the same as IPDR_Time, but this seems to work: */
    time_t cx = c;
    localtime_r(&cx, &tmMem);
    FormatTime(buf, &tmMem);
    return buf;
}

static
char* GenericPlatform_FormatTimeGMT(IPDR_PlatformArg P, IPDR_Time c, char* buf)
{
    /* POSIX and VxWorks differ in their definition of gmtime_r:
       POSIX returns struct tm* pointer but VxWorks returns an int. We
       depend on this function simply filling the buffer and ignore the
       return value.  */
    struct tm tmMem;
    time_t cx = c;
    gmtime_r(&cx, &tmMem);
    FormatTime(buf, &tmMem);
    return buf;
}

static
char* GenericPlatform_FormatTimeLocalNow(IPDR_PlatformArg P, char* buf)
{
    IPDR_Time t;
    GenericPlatform_GetTime(P, &t); /* or: t = time(0); */
    GenericPlatform_FormatTimeLocal(P, t, buf);
    return buf;
}

static
char* GenericPlatform_FormatTimeGMTNow(IPDR_PlatformArg P, char* buf)
{
    IPDR_Time t;
    GenericPlatform_GetTime(P, &t); /* or: t = time(0); */
    GenericPlatform_FormatTimeGMT(P, t, buf);
    return buf;
}

static
char* GenericPlatform_FormatTimeSyslogNow(IPDR_PlatformArg P, char* buf)
{
    IPDR_Time t;
    /* See comments with GenericPlatform_FormatTimeLocal */
    struct tm tmMem;
    time_t cx;
    static const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    GenericPlatform_GetTime(P, &t); /* or: t = time(0); */
    cx = t;
    localtime_r(&cx, &tmMem);
    sprintf(buf, "%.3s %2d %02d:%02d:%02d",
            months[tmMem.tm_mon], tmMem.tm_mday,
            tmMem.tm_hour, tmMem.tm_min, tmMem.tm_sec);
    return buf;
}
/* ---------------------- Utility functions ------------------------- */

static void BlockSigPipe(GenericST *E)
{
#ifndef _MSC_VER
    sigset_t SigSet;

    sigemptyset(& SigSet);
    sigaddset(& SigSet, SIGPIPE);

#if defined(MT_SUPPORT)
#if defined(PTHREADS_SUPPORT)
    pthread_sigmask(SIG_BLOCK, & SigSet, NULL);
#else
    /* This might not be what you want: it does all threads in a
       task. But it seems that VxWorks only offers this possibility;
       it has no equivalent of pthread_sigmask. */
    sigprocmask(SIG_BLOCK, & SigSet, NULL);
#endif
#else /* not MT_SUPPORT */
    sigprocmask(SIG_BLOCK, & SigSet, NULL);
#endif

#endif
}

#if defined(_MSC_VER)
static void GenericPlatform_GetTV(struct timeval *TV)
{
    struct _timeb tb;
    _ftime(& tb);

    TV->tv_sec = tb.time;
    TV->tv_usec = tb.millitm * 1000;
}
#else
/* VXWORKS, POSIX */
static void GenericPlatform_GetTV(struct timeval *TV)
{
#if defined(SOLARIS) || defined(VXWORKS) || defined(_VXWORKS)
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts); /* Should verify return code == 0 */
    TV->tv_sec = ts.tv_sec;
    TV->tv_usec = ts.tv_nsec / 1000;
#else
    gettimeofday(TV, NULL); /* is Solaris-only? also cygwin ... */
    /* see also comment under GenericPlatform_GetTime */
#endif
}
#endif

/* ------------------- IPDR API functions for sockets -----------------*/

/* These don't really belong here ... there ought to be a proper
 * "wrapper" and "handle" for sockets but their use is somewhat
 * pervasive (with the the more "Berkeley" flavor, namely an int file
 * descriptor), so for now, put them here and figure things out
 * properly in the future, if we have to port to a platform that
 * doesn't use the Berkeley or Winsock model. */


/* --------------- IPDR API function --------------- */

void IPDR_SetInvalidSocket(IPDR_SocketHandle* S)
{
    S->socketFd = -1;
}


/* --------------- IPDR API function --------------- */

int IPDR_IsValidSocket(const IPDR_SocketHandle S)
{
	/* 0, 1, 2 - stdin, stdout, stderr !!!!! */
    return (S.socketFd > 2);
}


/* --------------- IPDR API function --------------- */

IPDR_BOOL IPDR_SocketEq(const IPDR_SocketHandle S1, const IPDR_SocketHandle S2)
{
    return (S1.socketFd == S2.socketFd) ? IPDR_TRUE : IPDR_FALSE;
}


/* --------------- Platform Initialization --------------- */
#if defined(SNMP_SUPPORT)
static
IPDR_MIBServices GenericPlatform_MIBServices = {
	GenericPlatform_MIB_Start,
	GenericPlatform_MIB_SetExporterHandle,
	GenericPlatform_MIB_UnSetExporterHandle,
	GenericPlatform_MIB_IsRunning,
	GenericPlatform_MIB_Shop
};
#endif

static
IPDR_MemoryServices GenericPlatform_MemoryServices = {
#ifdef DEBUG_MEMORY
    0, 0, 0, 0, 0 /* make sure we trap out if we accidentally call these */
#else
    GenericPlatform_MemoryAllocate,
    GenericPlatform_MemoryAllocateWithHdr,
    GenericPlatform_MemoryFree,
    GenericPlatform_MemoryFreeWithHdr,
    GenericPlatform_MemoryStatistics
#endif
};

static
IPDR_EventServices GenericPlatform_EventServices = {
    GenericPlatform_RegisterTimer,
    GenericPlatform_ReregisterTimer,
    GenericPlatform_RegisterPeriodicTimer,
    GenericPlatform_UnregisterTimer,
    GenericPlatform_RegisterReadSocket,
    GenericPlatform_RegisterWriteSocket,
    GenericPlatform_UnregisterSocket,
    /*GenericPlatform_DescribeTimer*/
};

static
IPDR_PersistenceServices GenericPlatform_PersistenceServices = {
    GenericPlatform_PersistentStore,
    GenericPlatform_PersistentRetrieve
};

static
IPDR_NetworkServices GenericPlatform_NetworkServices = {
    GenericPlatform_CreateTCPSocket,
    GenericPlatform_CreateUDPSocket,
    GenericPlatform_Bind,
    GenericPlatform_Connect,
	GenericPlatform_GetSockName,
    GenericPlatform_Close,
    GenericPlatform_Listen,
    GenericPlatform_Accept,
    GenericPlatform_SetExporterSocketParams,
    GenericPlatform_Send,
    GenericPlatform_Recv,
    GenericPlatform_RecvFrom,
    GenericPlatform_SendTo,
    GenericPlatform_Shutdown,
    GenericPlatform_GetHostName
};

static
IPDR_LockingServices GenericPlatform_LockingServices = {
    GenericPlatform_CreateLock,
    GenericPlatform_DeleteLock,
#ifdef DEBUG_LOCKS
    0, 0, 0, 0 /* make sure we trap out if we accidentally call these */
#else
    GenericPlatform_Lock,
    GenericPlatform_Unlock,
    GenericPlatform_LockPlatform,
    GenericPlatform_UnlockPlatform
#endif
};

static
IPDR_TimeServices GenericPlatform_TimeServices = {
    GenericPlatform_GetTime,
    GenericPlatform_SetTime,
    GenericPlatform_SleepMS,
    GenericPlatform_FormatTimeLocalNow,
    GenericPlatform_FormatTimeGMTNow,
    GenericPlatform_FormatTimeLocal,
    GenericPlatform_FormatTimeGMT,
    GenericPlatform_FormatTimeSyslogNow
};

IPDR_Status GenericPlatform_InitializeInternal(IPDR_PlatformServices *PS)
{
    IPDR_Status Status = IPDR_OK;

#ifdef _MSC_VER
    WSADATA wsaData;
#endif

    GenericST *UST = (GenericST *) malloc(sizeof(GenericST));
    if (UST == NULL) {
        return IPDR_ERR_ALLOCATION_FAILED;
    }

#ifdef _MSC_VER
    WSAStartup(MAKEWORD(2, 2), & wsaData);
#endif

    strncpy(PS->eyecatcher, "IPDRPS", sizeof PS->eyecatcher);
    PS->PlatformArg = (IPDR_PlatformArg) UST;
#ifdef DEBUG_MODE
	/* memory collector */
	UST->memCollectorHead = NULL;
	UST->memCollectorTail = NULL;
	/* memory collector */
#endif
    strncpy(UST->eyecatcher, "GnrcST ", sizeof UST->eyecatcher);

    UST->Timers = NULL;
    UST->Enabled = IPDR_FALSE;
    UST->Stopped = IPDR_FALSE;
    UST->PlatformLock = NULL;
    UST->FDReadEvents = NULL;
    UST->FDWriteEvents = NULL;
    UST->ScanningFDs = IPDR_FALSE;

    FD_ZERO(& (UST->ReadFDs));
    FD_ZERO(& (UST->WriteFDs));
    UST->MaxFD.socketFd = -1;

    UST->ExporterHandleRegistered = IPDR_FALSE;
    UST->ExporterHandle = NULL;

    UST->numAllocs = 0;
    UST->numFrees = 0;
    UST->bytesAllocated = 0;

    PS->Start = GenericPlatform_Start;
    PS->Stop = GenericPlatform_Stop;
    PS->IsRunning = GenericPlatform_IsRunning;
    PS->Delete = GenericPlatform_Delete;
    PS->GetLastErrorMessage = GenericPlatform_GetLastErrorMessage;
    PS->SetExporterHandle = GenericPlatform_SetExporterHandle;
    PS->UnsetExporterHandle = GenericPlatform_UnsetExporterHandle;

    PS->MemoryServices = GenericPlatform_MemoryServices;
    PS->EventServices = GenericPlatform_EventServices;
    PS->PersistenceServices = GenericPlatform_PersistenceServices;
    PS->NetworkServices = GenericPlatform_NetworkServices;
    PS->LockingServices = GenericPlatform_LockingServices;
    PS->TimeServices = GenericPlatform_TimeServices;
#if defined(SNMP_SUPPORT)
	PS->MIBServices = GenericPlatform_MIBServices;
#endif
#ifdef _MSC_VER
    if (WSAStartup (MAKEWORD(1,1), &wsaData) != 0)
        return IPDR_ERR;
#endif

    Status = GenericPlatform_CreateLock(UST, (void**)&UST->PlatformLock);
    if (FAILURE(Status)) {
        free(UST);
        PS->PlatformArg = NULL;
        return Status;
    }

    BlockSigPipe(UST);

/* #warning Initialize the log Platform here ... */

    return IPDR_OK;
}
