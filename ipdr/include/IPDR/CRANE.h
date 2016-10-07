/** @file CRANE.h
 *
 *  Public header file for the CRANE Exporter.
 *  This file is generally a wrapper for compadibility with the
 *  deprecated CRANE protocol
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 *  $Id: CRANE.h,v 1.18 2005/01/30 15:41:23 guyg Exp $
 *
*/

#ifndef DOXYGEN_PROCESSING

/* This file is not highly readable because it has had comments added
 * in a form suitable for processing by Doxygen
 * (http://www.doxygen.org).  The output of the Doxygen processing is
 * distributed with the sources; you are encouraged to not read this
 * file but instead use those files plus the other documentation
 * distributed with the API for understanding the API.
 */
#endif


/** CRANE transmitter (client) API
 *
 *  This document describes the CRANE transmitter (client) Application
 *  Programming Interface (API), a set of ANSI C declarations and
 *  prototypes enabling a network element's host application and local
 *  operating system properly to configure and control
 *  embedded CRANE transmitter. The CRANE Transmitter is
 *  implemented as a library of ANSI C functions and structures.
 *
 *  The "client" is also sometimes known as the "data transmitter".
 *  It is the code which is used by a network element to send data
 *  to a collector (server), which receives the data and processes it further.
 */

#ifndef _CRANE_H_


/** Macro to prevent double inclusion of this header.
 */
#define _CRANE_H_

#include <IPDR/Config.h>

/** Source code version identifier.
 */
#define CRANE_H_RCSID "@(#) $Id: CRANE.h,v 1.18 2005/01/30 15:41:23 guyg Exp $" CRANE_COPYRIGHT_STR

#if defined(GLOBAL_RENAME)


/** Renaming global version identifier (to avoid name clashes).
 */
#define CRANE_h_rcsid CRANE__CRANE_h_rcsid
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if 0
} /* put this here to un-confuse Emacs's automatic indentation */
#endif

#include <stddef.h>

/** @def CRANE_API_LINKAGE
 *  The linkage type to add to each external function declaration
 *  (mainly for Windows).
 */
#if defined(_MSC_VER)
#ifdef CRANE_STATIC


/** Linkage for Windows static.
 */
#define CRANE_API_LINKAGE
#elif defined(CRANE_EXPORTS)


/** Linkage for Windows dynamic (DLL) exports.
 */
#define CRANE_API_LINKAGE  __declspec(dllexport)
#else


/** Linkage for Windows dynamic (DLL) imports.
 */
#define CRANE_API_LINKAGE  __declspec(dllimport)
#endif
#else


/** Linkage for non-Windows.
 */
#define CRANE_API_LINKAGE
#endif


/** A convenience macro for defining the return value and linkage for
 *  an API function.
 */
#define CRANE_API CRANE_Status CRANE_API_LINKAGE


/** Value to use for "default" IP address (for type @ref CRANE_IPAddress).
 */
#define CRANE_DEFAULT_ADDRESS  0


/** @brief Type returned by a CRANE API function. Actual values are given by
 *  the enumeration #CRANE_STATUS_RETURN_CODE.
 */
typedef IPDR_Status CRANE_Status;


/** The type used as a context argument when calling platform function.
 */
typedef IPDR_PlatformArg CRANE_PlatformArg;


/** The information about a timer.
 */
typedef IPDR_TimerID CRANE_TimerID;


/** The information about an event that is triggered by a socket
 *  becoming available for I/O. In effect, this is a "callback". If
 *  the socket is handled by non-blocking I/O, the callback is called
 *  when the file descriptor (socket) becomes available for I/O (or
 *  has had something traumatic happen to it, such as being
 *  closed). In the case of blocking I/O, the callback is used by the
 *  generic thread which repeatedly does the I/O. (Note: for the
 *  blocking case - which has a thread - the event is not put into the
 *  list of non-blocking callbacks.)
 */
typedef IPDR_SocketEventID CRANE_SocketEventID;


/** The information for a transmitter (client), as created by CRANE_InitClient().
 */
typedef IPDR_ExporterHandle CRANE_ClientHandle;


/** The information for a session, as created by CRANE_CreateSession().
 */
typedef IPDR_SessionHandle CRANE_SessionHandle;


/** The information for a template, as created by
 *  CRANE_CreateTemplate().  This should be treated as an opaque type,
 *  with no reliance on the actual structure, which may change in the
 *  future. */
typedef IPDR_TemplateHandle CRANE_TemplateHandle;


/** An internal IP address (#unint32_t). Following ancient tradition,
 *  it is kept in host byte order, not network byte order.
 */
typedef IPDR_IPAddress CRANE_IPAddress;


/** A time value as seconds since "epoch" (1970-01-01T00:00:00 UTC
 *  (GMT)) (the number of bits in the type should be the same as or
 *  larger than <code>time_t</code>).
 */
typedef IPDR_Time CRANE_Time;

/** An opaque socket, whose contents may change in the future.
 *  @see Defs.h */
typedef IPDR_SocketHandle CRANE_SocketHandle;


/** Set a socket to an invalid value (that is, not open)
 *  @param S Pointer to the socket.
 */
#define CRANE_SetInvalidSocket(S) IPDR_SetInvalidSocket(S)

/** Check whether a socket is valid (opened).
 *  @param S The socket to check.
 *  @return 0 for not valid (not open); 1 for valid (opened).
 */
#define CRANE_IsValidSocket(S) IPDR_IsValidSocket(S)


/** Check whether two sockets are the same.
 *  @param s1 The first socket.
 *  @param s2 the second socket.
 *  @return 0 for different; 1 for the same.
 */
#define CRANE_SocketEq(s1,s2) IPDR_SocketEq(s1,s2)


/** Mask for disabling a key in a template (for CRANE_Key::KeyAttributes).
 */
#define CRANE_KEY_DISABLED IPDR_KEY_DISABLED


/** Convention with CRANE_KeyType values for varying length fields: this value
 *  is "or"ed with the key type (e.g., #CRANE_TYPE_STRING). */
#define CRANE_TYPE_VAR_LENGTH  IPDR_TYPE_VAR_LENGTH


/** Access a value from a record by a function callback (possible
 *  value for CRANE_Key::AccessType).
 */
#define CRANE_ACCESS_BY_FUNCTION IPDR_ACCESS_BY_FUNCTION


/** Access a value from a record by an offset (possible value for
 *  CRANE_Key::AccessType).
 */
#define CRANE_ACCESS_BY_OFFSET   IPDR_ACCESS_BY_OFFSET


/** Access data using a functional callback. For each field being
 *  output by CRANE, the data field needs to be accessed. For the
 *  #CRANE_ACCESS_BY_OFFSET case, this can be done by computing the
 *  pointer as <code>Data+(CRANE_Offset)key.KeyAccessor</code>.
 *  For the #CRANE_ACCESS_BY_FUNCTION case, the pointer is computed by this
 *  key access callback.
 *  The callback is called only if the key is enabled.
 *
 *  @param Data The data record being output.
 *  @param ExtraData The extra data recorded with CRANE_Key::ExtraData for this field.
 *  @param Value (updated) pointer to the data field to be transmitted.
 *  @see CRANE_Key
 */
typedef IPDR_KeyAccessor CRANE_KeyAccessor;


/** The type to use for accessing a field offset.
 */
typedef IPDR_Offset CRANE_Offset;

/** Key types identifiers (for CRANE_Key::KeyTypeID).  These are in
 *  @ref CRANE_RFC
 *  section 4.6 "Template Data (TMPL DATA)".
 */
typedef enum IPDR_KeyType CRANE_KeyType ;

/** The definition of a single key in a template, as passed to
 *  CRANE_CreateTemplate().
 *
 *  If these keys are used in <i>usage</i> to automatically
 *  generate field names, it is recommended to keep the key label
 *  length to less than 20 characters to avoid truncation in the
 *  database field name.
 */
typedef IPDR_Key CRANE_Key;


/** The definition of a template with its keys (the parameters to
 *  CRANE_CreateTemplate().
 */
typedef IPDR_Template CRANE_Template;

/** The definition of a collector (server) (the parameters to CRANE_CreateSession()).
 * @ingroup CRANE_ssessions
 */
typedef IPDR_CollectorHandle CRANE_Server;


/** Event types
 */
typedef enum IPDR_Event_t CRANE_Event_t;

/** Reason for a disconnection.
 */
typedef enum IPDR_EventDisconnected_t CRANE_EventDisconnected_t;

/** Type of error event.
 */
typedef enum IPDR_EventError_t CRANE_EventError_t ;

/** Callback for an event.
 *  @param EventID Identification of the type of event.
 *  @param Msg Message associated with the event.
 *             The string contains the collector (server) information in the form
 *             "<code>Server=<i>xx.xx.xx.xx</i>, Port=<i>xx</i>,</code>"
 *             (plus other text).
 *             [In the case of the active
 *             collector (server) changing, the information refers to the
 *             new collector (server).]
 *             However, you should not depend on this but instead
 *             should use ::CRANE_EventCallbackDetailed.
 *  @param Arg Context value given when callback was registered by
 *                     calling CRANE_SetClientEventHandler().
 *  @deprecated Use ::CRANE_EventCallbackDetailed instead.
 *  @see CRANE_SetClientEventHandler
 */
typedef IPDR_EventCallbackDetailed CRANE_EventCallback ;


/** Set a callback function which will be used by the
 *  transmitter (client) to notify on various events.
 *
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param Callback callback function.
 *  @param Arg argument passed to the @a Callback function.
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 *  @deprecated Use CRANE_SetClientEventHandlerDetailed() instead.
 */
#define CRANE_SetClientEventHandler(ClientHandle,Callback,Arg) IPDR_SetExporterEventHandlerDetailed(ClientHandle,Callback,Arg)


/** Extra information for a #CRANE_EVENT_SERVER_DISCONNECTED event
 *  (see @ref CRANE_EventData).
 */
typedef struct IPDR_EventDisconnectedData CRANE_EventDisconnectedData;

/** The type for data sequence numbers. Note that an older form of the
 *  protocol used signed numbers and reset the counter back to zero,
 *  reserving -1 for a kind of keep-alive. This has been changed to use
 *  a 32-bit unsigned number, with any keep-alive being done
 *  differently.
 */
typedef unsigned long CRANE_DSN_TYPE;


/** Extra information for a #CRANE_EVENT_RECORD_LOST event (see @ref CRANE_EventData).
 */

typedef struct IPDR_RecordLostData CRANE_RecordLostData ;

/** Extra information for a #CRANE_EVENT_ACTIVE_SERVER_CHANGED event
 *  (see @ref CRANE_EventData).
 */
typedef struct IPDR_ServerChangedData CRANE_ServerChangedData ;

/** Extra information for a #CRANE_EVENT_ERROR event
 *  (see @ref CRANE_EventData).
 */
typedef struct IPDR_ErrorData CRANE_ErrorData ;

/** The event information, containing what's in the message (see
 *  ::CRANE_EventCallback) but in binary form.
 */
typedef struct IPDR_EventData CRANE_EventData;

/** Explain an event in English, using the information in the
 *  event data (there will already be a string with the message;
 *  this just uses the \a EventID and sub-ID (if any) to give
 *  a brief string).
 *  @param Event The event, as sent to ::CRANE_EventCallbackDetailed.
 *  @return A C-string corresponding to the \a EventID and sub-ID
 *    (if any).
 */
#define CRANE_EventMsg(Event) IPDR_EventMsg(Event)

/** Callback for an event.
 *  @param Event The event.
 *  @param Msg Message associated with the event.
 *             The content of the message is subject to change.
 *             If you wish to get any additional information
 *             about the event, use the Event parameter; the
 *             message is simply a convenience for outputting
 *             som text about the error.
 *  @param Arg Context value given when callback was registered by
 *             calling CRANE_SetClientEventHandlerDetailed().
 */
typedef IPDR_EventCallbackDetailed CRANE_EventCallbackDetailed ;

/** Set a callback function which will be used by the
 *  transmitter (client) to notify on various events.
 *
 *  Usually the callback will be triggered from within an API
 *  function; but in some cases, it can occur asynchronously. The
 *  callback is multi-thread safe with all the other API functions;
 *  however, the caller may need to provide an additional mutex in the
 *  callback for protection from other threads outside the CRANE API.
 *
 *  Applications should not depend on the order of connect,
 *  disconnect, or changed events. In particular, it is possible for a
 *  single collector (server) to have multiple connect or disconnect events in a
 *  row because there can be multiple causes for a connection or
 *  disconnection (e.g., a TCP layer reconnection, a "START" message,
 *  another collector (server) disconnecting).
 *
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param Callback callback function.
 *  @param Arg argument passed to the @a Callback function.
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
#define CRANE_SetClientEventHandlerDetailed(ClientHandle,Callback,Arg) IPDR_SetClientEventHandlerDetailed(ClientHandle,Callback,Arg)

/** Callback for looking up a parameter value.
 *  @ingroup CRANE_initialization_starting_deletion
 *  The function receives a parameter name and
 *                the additional argument (\a ParamsHandlerArg) and returns the
 *                parameter value as a string (below is a list of available
 *                parameters).  If the function allocates memory for its return
 *                value, it must free the memory itself (e.g., on the next call to
 *                the parameters handler, or after it calls CRANE_DeleteClient().  If
 *                the callback returns NULL, the default value is used.  If this
 *                function pointer is NULL, the default values will be used.
 *  @param Name The parameter name.
 *  @param Arg  Context value given when callback was registered by calling CRANE_InitClient().
 *  @return C-string with the value; or NULL if the default is to be used.
 *  The names correspond to names in the MIB.
 *
 *  @see CRANE_MIBGet
 *  @see CRANE_MIB
 *  @see CRANE_MIBGetByName
 *  @see CRANE_MIB_related
 *  @see CRANE_transmitter_MIB
 *  @see CRANE_MIB
 *
 * This function is currently used only by CRANE_InitClient() to set
 * initial MIB values.  However, for future compatibility, it is
 * recommended that the \a Arg parameter contains a pointer to the
 * @ref CRANE_ClientHandle that is being set by
 * CRANE_InitClient(). The handle will be NULL if the MIB has not yet
 * been set up (that is, if CRANE_MIBGetByName() cannot be called).
 *
 * The following parameters are retrieved by the transmitter (client) using the
 * parameters handler callback. If the parameters handler callback is
 * not given, or if the callback returns NULL, the default values are
 * used.
 *
 * See @ref CRANE_configuration_values for details.
 *
 */
typedef IPDR_ParametersHandlerFunc CRANE_ParametersHandlerFunc;

/** Callback for loading the MIB.
 *  @ingroup CRANE_initialization_starting_deletion
 *  The function should limit its activity to calls to
 *  CRANE_MIBRestoreNodeByName().
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param MIBVersion The MIB version (for matching with the saved MIB).
 *  @param Arg  Context value given when callback was registered by calling CRANE_InitClient().
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 *    If this function fails, CRANE_InitClient() will fail with the
 *    same return value.
 */
typedef IPDR_LoadMIBFunc CRANE_LoadMIBFunc;


/** Callback for saving the MIB.
 *  @ingroup CRANE_initialization_starting_deletion
 *  The function will typically walk the MIB and write it to a file,
 *  so that it can be loaded by ::CRANE_LoadMIBFunc.
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param MIBVersion The MIB version (for matching with the load).
 *  @param Arg  Context value given when callback was registered by calling CRANE_InitClient().
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_SaveMIBFunc CRANE_SaveMIBFunc;

/** Levels for log messages; used both as a level for outputting
 *  (e.g., CRANE_LogWrite()) and for filtering (CRANE_SetModuleDebugLevel()).
 */
typedef enum IPDR_DebugLevel CRANE_DebugLevel ;

/** Set the debug level of a module within the CRANE transmitter (client)
 *
 * @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 * @param ModuleName name of module:
 *   <ul>
 *     <li>"<code>Client</code>"
 *     <li>"<code>Platform</code>"
 *     <li>"<code>TCP</code>"
 *     <li>"<code>Host</code>"
 *     <li>"<code>Queue</code>"
 *     <li>"<code>Memory</code>"
 *     <li>"<code>Protocol</code>"
 *    </ul>
 * @param  L debug level (messages up to this level will be
 *       output from the module).
 * @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 * @deprecated Use CRANE_SetModuleMoreDebugLevel() or set the
 *       value via the MIB or parameters.
 */
#define CRANE_SetModuleDebugLevel(ClientHandle,ModuleName,L) IPDR_SetModuleDebugLevel(ClientHandle,ModuleName,L)

/** Set the debug level for a transmitter (client), but don't decrease it below what
 *  it already is. Other than that, it's the same as CRANE_SetModuleDebugLevel().
 * @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 * @param ModuleName name of module:
 *   <ul>
 *     <li>"<code>Client</code>"
 *     <li>"<code>Platform</code>"
 *     <li>"<code>TCP</code>"
 *     <li>"<code>Host</code>"
 *     <li>"<code>Queue</code>"
 *     <li>"<code>Memory</code>"
 *     <li>"<code>Protocol</code>"
 *    </ul>
 * @param  L debug level (messages up to this level will be
 *       output from the module).
 * @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
#define CRANE_SetModuleMoreDebugLevel(ClientHandle,ModuleName,L) IPDR_SetModuleMoreDebugLevel(ClientHandle,ModuleName,L)


/** Check whether a specific module within the transmitter (client) outputs log
 *   messages at the given level.
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param ModuleName name of module:
 *   <ul>
 *     <li>"<code>Client</code>"
 *     <li>"<code>Platform</code>"
 *     <li>"<code>TCP</code>"
 *     <li>"<code>Host</code>"
 *     <li>"<code>Queue</code>"
 *     <li>"<code>Memory</code>"
 *     <li>"<code>Protocol</code>"
 *    </ul>
 *  @param Lvl the level to compare with.
 *  @return 1 if the module is outputing log messages at the
 *   requested level, 0 otherwise.
 */
#define CRANE_CheckLogLevel(ClientHandle,ModuleName,Lvl) IPDR_CheckLogLevel(ClientHandle,ModuleName,Lvl)

/** Output a log message to the CRANE client log.
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param ModuleName name of module:
 *   <ul>
 *     <li>"<code>Client</code>"
 *     <li>"<code>Platform</code>"
 *     <li>"<code>TCP</code>"
 *     <li>"<code>Host</code>"
 *     <li>"<code>Queue</code>"
 *     <li>"<code>Memory</code>"
 *     <li>"<code>Protocol</code>"
 *    </ul>
 *  @param Level message debug level.
 *  @param Func string describing the function which originated the message.
 *  @param Msg message (can be a printf style format string).
 *  @param ... additional arguments in case of a formatted msg.
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
/*PRINTFLIKE5*/
#define CRANE_LogWrite(ClientHandle,ModuleName,Level,Func,Msg,...) IPDR_LogWrite(ClientHandle,ModuleName,Level,Func,Msg,...)


/** Callback for outputting to a log.
 *  @param Msg The message that is to be written to the log.
 *  @param Arg The context that was passed to CRANE_SetLogOutputCallback().
 *  @see CRANE_SetLogOutputCallback
 *  @deprecated Use ::CRANE_LogCallbackDetailed instead.
 */
typedef IPDR_LogCallbackDetailed CRANE_LogCallback;


/** Set a callback function which will be used to output log messages
 *  (instead of sending them over a UDP socket to a syslog server
 *  or to stdout). See CRANE_SetLogOutputCallbackDetailed() for more
 *  information.
 *
 *  CRANE_SetLogOutputCallback() and
 *  CRANE_SetLogOutputCallabackDetailed() are cancel each other. If
 *  you first call CRANE_SetLogOutputCallbackDetailed() and then call
 *  CRANE_SetLogOutputCallback(), the function specified by
 *  CRANE_SetLogOutputCallbackDetailed() will be disabled. Similarly,
 *  if you call CRANE_SetLogOutputCallback() and then call
 *  CRANE_SetLogOutputputCallbackDetailed(), the function specified by
 *  CRANE_SetLogOutputCallback() will be disabled.
 *
 *  This callback is implemented as follows (or similar; but you should
 *  <em>not</em> rely on this format, which is subject to change):
 *  @verbatim
 *    char buf[...];
 *    sprintf("%.7s <%.40s> [%-8.8s] %.99s: %.199s\n",
 *            LevelStr[Level], ClientName, Module, Func, Msg)
 *    CB(Msg, Arg);
 *  @endverbatim
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param CB Callback.
 *  @param Arg additional argument attached to the callback.
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 *  @deprecated Use CRANE_SetLogOutputCallbackDetailed() if you
 *     want to get the logging details such as error level of the message.
 *  @see CRANE_SetLogOutputCallbackDetailed
 */
#define CRANE_SetLogOutputCallback(ClientHandle,CB,Arg) IPDR_SetLogOutputCallback(ClientHandle,CB,Arg)

/** Callback for outputting to a log (detailed).
 *
 *  This overrides the default behavior of writing to standard output
 *  (output in syslog format (CRANE_SetLogOutputAddress()) is in addition
 *  to any callback).
 *
 *  @param Level      the level of the message
 *  @param LevelStr   a string representing the message.
 *  @param ClientName the value given in CRANE_InitClient.
 *  @param Module     the module ("<code>Client</code>", "<code>Platform</code>",
 *                    etc. as used by CRANE_SetModuleDebugLevel()
 *                    or CRANE_LogWrite()).
 *  @param Func       a string representing where in the code the
 *                    message was generated.
 *  @param Msg        the actual log message.
 *  @param Arg The context that was passed to CRANE_SetLogOutputCallbackDetailed().
 *  @see CRANE_SetLogOutputCallback
 */
typedef IPDR_LogCallbackDetailed CRANE_LogCallbackDetailed;


/** Set a callback function which will be used to output
 *  log messages (instead of sending them over a UDP socket
 *  to a syslog server or to stdout).
 *
 *  CRANE_SetLogOutputCallback() and CRANE_SetLogOutputCallbackDetailed()
 *  can be called as many times as desired, in any order.
 *  The latest call to either of these overrides all other calls.
 *  The callback can be turned off by setting the callback function
 *  to NULL.
 *
 *  Output via callback is <em>in addition to</em> output in syslog
 *  format (see CRANE_SetLogOutputAddress()). If neither
 *  syslog output nor a callback are provided, log output is to
 *  standard error output (<code>stderr</code>) if
 *  #STD_FILE_OPERATIONS_SUPPORT is defined.
 *
 *  CRANE_SetLogOutputCallback() and
 *  CRANE_SetLogOutputCallabackDetailed() are cancel each other. If
 *  you first call CRANE_SetLogOutputCallbackDetailed() and then call
 *  CRANE_SetLogOutputCallback(), the function specified by
 *  CRANE_SetLogOutputCallbackDetailed() will be disabled. Similarly,
 *  if you call CRANE_SetLogOutputCallback() and then call
 *  CRANE_SetLogOutputputCallbackDetailed(), the function specified by
 *  CRANE_SetLogOutputCallback() will be disabled.
 *
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param CB Callback.
 *  @param Arg additional argument attached to the callback.
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
#define CRANE_SetLogOutputCallbackDetailed(ClientHandle,CB,Arg) IPDR_SetLogOutputCallbackDetailed(ClientHandle,CB,Arg)


/** Set the log output address and port.
 *  As mentioned in CRANE_logging, the log can be output
 *  by UDP in syslog format. To turn this on, you must
 *  set the log output address and port. Output in syslog format
 *  is <em>in addition to</em> output via callback. If neither
 *  syslog output nor a callback are provided, log output is to
 *  standard error output (<code>stderr</code>) if
 *  #STD_FILE_OPERATIONS_SUPPORT is defined.
 *
 *  You can turn off sending syslog datagrams by setting both
 *  \a Addr and \a Port to zero.
 *
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param Addr The IP address to send syslog UDP packets to.
 *  @param Port The IP port to send syslog UDP packets to (port 514 is
 *              the suggested port in RFC 3164).
 *  @param Facility The facility, as defined in RFC 3164. It should be one
 *         of the following (values 2 through 15 should not be used):
 *     <ul>
 *        <li>0: kernel messages
 *        <li>1: user-level messages
 *        <li>16: local use 0  (local0)
 *        <li>17: local use 1  (local1)
 *        <li>18: local use 2  (local2)
 *        <li>19: local use 3  (local3)
 *        <li>20: local use 4  (local4)
 *        <li>21: local use 5  (local5)
 *        <li>22: local use 6  (local6)
 *        <li>23: local use 7  (local7)
 *     </ul>
 *  @param MachineName The machine's name for the message, which
 *      must conform to RFC 1035 (an internal copy is made of this string).
 *      If this is  NULL or zero-length, an attempt will be made to generate
 *      the name (typically using the host name or IP address). If it cannot
 *      be determined, the value "<code>unknown-CRANE</code>"
 *      will be used.
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
#define CRANE_SetLogOutputAddress(ClientHandle,Addr,Port,Facility,MachineName)\
	IPDR_SetLogOutputAddress(ClientHandle,Addr,Port,Facility,MachineName)

/** Status / Error codes.
 *  @see CRANE_Status
 *  @ingroup CRANE_general_typedefs
 */
typedef enum IPDR_STATUS_RETURN_CODE CRANE_STATUS_RETURN_CODE;

/** Convert a return code into an English string.
 *  @ingroup CRANE_general_typedefs
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *                This may be NULL, which might result in a slightly less
 *                informative message; it also might result in using
 *                a static area for creating the message.
 *  @param Status the result of a call a CRANE API call.
 *  @return A string with the English string, if the \a Status
 *  parameter is one of the Status / Error codes given by
 *  #CRANE_STATUS_RETURN_CODE. If the the status is #CRANE_OK, the
 *  return value is the empty string (""). If the status is not a
 *  valid code, a dynamically-created string is created. This string
 *  is valid only until another call of CRANE_StatusMsg() and is not
 *  thread-safe.
*/
#define CRANE_StatusMsg(ClientHandle,Status) IPDR_StatusMsg(ClientHandle,Status)

typedef IPDR_Start_t CRANE_Start_t ;


/** Stop any tasks / threads used for providing the timer / socket
 *  event services. After calling Stop, the transmitter (client) assumes that these
 *  services are not active but the abstraction layer can still be
 *  used.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Stop_t CRANE_Stop_t;


/** Test whether the transmitter (client) is running.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @return 0 for not running, 1 for running.
 */
typedef IPDR_IsRunning_t CRANE_IsRunning_t;


/** Free any resources internally allocated
 *  by the platform abstraction layer and stop execution of any code
 *  started (if any) by the Start operation.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Delete_t CRANE_Delete_t ;


/** Set the transmitter (client) handle in the platform abstraction (so that logging can be done from the platform abstraction).
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @param CH The transmitter (client) handle as returned by CRANE_InitClient().
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_SetExporterHandle_t CRANE_SetClientHandle_t ;


/** Called by CRANE_DeleteClient() to unset the transmitter (client) handle in the platform abstraction.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_UnsetExporterHandle_t CRANE_UnsetClientHandle_t ;

/** The longest error message that GetLastErrorMessage can return.
 */
#define CRANE_MAX_ERROR_LENGTH IPDR_MAX_ERROR_LENGTH


/** Retrieve the error message corresponding to the last error that
 *  occurred in the current thread/task during a platform abstraction
 *  layer call (for example, in case of Unix, this should correspond to
 *  the current <code>errno</code>).
 *  If there is no error (e.g., on Unix errno == 0), then this
 *  should set the string to the #CRANE_NO_ERROR_ERROR_MESSAGE
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @param Msg (output) A buffer of at least #CRANE_MAX_ERROR_LENGTH bytes, which
 *             receives a null-terminated C-string of the error message.
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_GetLastErrorMessage_t CRANE_GetLastErrorMessage_t;


/** Value returned by #CRANE_GetLastErrorMessage_t if there was no error. */
#define CRANE_NO_ERROR_ERROR_MESSAGE IPDR_NO_ERROR_ERROR_MESSAGE

/** The callback when a timer expires.
 *  @param Arg The context argument provided with the
 *             callback was registered with ::CRANE_RegisterTimer_t
 *             or ::CRANE_RegisterPeriodicTimer_t.
 *  @see CRANE_RegisterTimer_t
 *  @see CRANE_ReregisterTimer_t
 *  @see CRANE_RegisterPeriodicTimer_t
 */
typedef IPDR_TimerCallback_t CRANE_TimerCallback_t;


/** Register and start a new timer.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @param Callback Function to call when the timer expires.
 *  @param Arg Argument to the callback function.
 *  @param Timeout Timer period in milliseconds.
 *  @param ID (output) Reference to the timer for reregistering or
 *                  deleting the timer.
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 *  @see CRANE_ReregisterTimer_t
 *  @see CRANE_UnregisterTimer_t
 */
typedef IPDR_RegisterTimer_t CRANE_RegisterTimer_t;

/** Change the timeout for an existing timer.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @param Callback Function to call when the timer expires.
 *  @param Arg Argument to the callback function.
 *  @param Timeout Timer period in milliseconds.
 *  @param ID (updated) Reference to the timer from ::CRANE_RegisterTimer_t.
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_ReregisterTimer_t CRANE_ReregisterTimer_t;


/** Register and start a periodic timer.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @param Callback Function to call when the timer expires.
 *  @param Arg Argument to the callback function.
 *  @param Timeout Timer period in milliseconds.
 *  @param ID (output) Reference to the timer for deleting the timer.
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 *  @see CRANE_UnregisterTimer_t
 */
typedef IPDR_RegisterPeriodicTimer_t CRANE_RegisterPeriodicTimer_t;


/** Cancel and delete an existing timer.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @param ID The timer reference from ::CRANE_RegisterTImer_t
 *            or ::CRANE_RegisterPeriodictTimer_t.
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 *  @see CRANE_RegisterTimer_t
 *  @see CRANE_RegisterPeriodictTimer_t
 */
typedef IPDR_UnregisterTimer_t CRANE_UnregisterTimer_t;


/** The callback when a socket is ready to do I/O or
 *  an error has occurred on the socket.
 *  @param S The socket
 *  @param Arg The context argument provided with the
 *             callback was registered with ::CRANE_RegisterReadSocket_t
 *             or ::CRANE_Register_Write_Socket_t.
 *  @see CRANE_RegisterReadSocket_t
 *  @see CRANE_Register_Write_Socket_t
 */
typedef IPDR_SocketCallback_t CRANE_SocketCallback_t ;

/** Register a socket for reading. A callback is triggered when data
 *  arrives or a communication problem occurs on the socket.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_RegisterReadSocket_t CRANE_RegisterReadSocket_t ;

/** Register a socket for writing. A callback is triggered when data
 *  arrives or a communication problem occurs on the socket.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_RegisterWriteSocket_t CRANE_RegisterWriteSocket_t ;

/** Unregister a socket callback.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 *  @see CRANE_RegisterReadSocket_t
 *  @see CRANE_RegisterWRiteSocket_t
 */
typedef IPDR_UnregisterSocket_t CRANE_UnregisterSocket_t;


/** Pointers to functions implementing the @ref CRANE_event_services.
 */
typedef IPDR_EventServices CRANE_EventServices;

/** Function called by the transmitter (client) in order to allocate a block of
 *  memory. The function must allocate at least the amount of memory
 *  requested by the function. The memory must be aligned on the most
 *  restrictive boundary (as for C's <code>malloc()</code>).
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @param Size The number of bytes to allocate.
 *  @param Buf (output) Pointer to the allocated memory; 0 (NULL) if failure
 *                   (#CRANE_ERR_ALLOCATION_FAILED).
 *  @return #CRANE_OK on success;
 *                 CRANE_ERR_ALLOCATION_FAILED or #CRANE_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_MemoryAllocate_t CRANE_MemoryAllocate_t;


/** Allocate memory, given a chunk size and a header size.
 *  This is used to allocate "objects" in a safe way.
 *  Same as ::CRANE_MemoryAllocate_t(P, HdrSize+Size, Buf).
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @param HdrSize Size of a header that goes in front of the data;
 *                  must be sizeof ObjectHeader in MemoryManagement.h
 *  @param Size The number of bytes to allocate.
 *  @param Buf (output) Pointer to the allocated memory; 0 (NULL) if failure
 *                   (#CRANE_ERR_ALLOCATION_FAILED).
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 *  @see CRANE_MemoryAllocate_t
 */
typedef IPDR_MemoryAllocateWithHdr_t CRANE_MemoryAllocateWithHdr_t;


/** Free memory allocated by ::CRANE_MemoryAllocate.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @param Buf The pointer to the memory allocated by ::CRANE_MemoryAllocate.
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_MemoryFree_t CRANE_MemoryFree_t;


/** Free memory allocated by ::CRANE_MemoryAllocateWithHdr.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 *  @see CRANE_MemoryFree_t
 */
typedef IPDR_MemoryFreeWithHdr_t CRANE_MemoryFreeWithHdr_t;


/** Get statistics on memory allocation.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @param numAllocs The number of calls to the CRANE_MemoryAllocate_t or CRANE_MemoryAllocateWithHdr_t
 *                   services.
 *  @param numFrees The number of calls to the CRANE_MemoryFree_t or CRANE_MemoryFreeWithHdr_t
 *                   services.
 *  @param bytesAllocated (output) The total number of bytes currently allocated.
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_MemoryStatistics_t CRANE_MemoryStatistics_t ;


/** Pointers to functions implementing the @ref CRANE_memory_services.
 */
typedef IPDR_MemoryServices CRANE_MemoryServices;

/** Create a lock.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @param LockPtr (output) The lock (allocated and initialized as necessary).
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_CreateLock_t CRANE_CreateLock_t ;


/** Delete a lock.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @param LockPtr The lock (as allocated by ::CRANE_CreateLock_t).
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_DeleteLock_t CRANE_DeleteLock_t;


/** Obtain a lock. This function must be reentrant and allow the lock
 *  to be obtained recursively (that is, if the same thread tries to
 *  obtain the lock twice, it will not deadlock but will wait until
 *  the other lock is freed). These semantics are required if socket
 *  handling is done by "call backs" (e.g., using the Unix select() or
 *  poll() to wait until a socket has data, then doing a read).
 *  @param P The context argument when calling platform function, as
 *  passed to CRANE_InitClient().
 *  @param LockPtr The lock (as allocated by ::CRANE_CreateLock_t).
 *  @return #CRANE_OK on success;
 *  #CRANE_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Lock_t CRANE_Lock_t ;


/** Unlock a previously locked lock. Unlocking is recursive and symmetric
 *  with ::CRANE_Lock_t -- that is, if there are <i>n</i> calls to lock,
 *  then there must be <i>n</i> calls to unlock before the lock is released.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @param LockPtr The lock (as allocated by ::CRANE_CreateLock_t).
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 *  @see CRANE_Lock_t
 */
typedef IPDR_Unlock_t CRANE_Unlock_t ;


/** Lock the single platform lock. Usually this is implemented with a
 *  pre-allocated lock in the ::CRANE_LockPlatform_t structure. This
 *  lock must be recursive, as described with ::CRANE_Lockt_t.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @param LockPtr The lock (as allocated by ::CRANE_CreateLock_t).
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_LockPlatform_t CRANE_LockPlatform_t;


/** Unlock the single platform lock.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 *  @see CRANE_LockPlatform_t
 */
typedef IPDR_UnlockPlatform_t CRANE_UnlockPlatform_t ;


/** Pointers to functions implementing the @ref CRANE_lock_services.
 */
typedef IPDR_LockingServices CRANE_LockingServices;

/** Store data into non-volatile (persistent) memory.
 *  The function must allow a named unit of memory to be replaced by
 *  another unit with the same \a UnitName but with a different
 *  \a BufferLength. This can happen during re-configuration.
 *  @param UnitName The name of the unit of memory to store,
 *            typically a file name.
 *  @param Buffer Pointer to the data to be stored.
 *  @param BufferLength Number of bytes to store; if 0 then delete
 *          the data from non-volatile memory.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_PersistentStore_t CRANE_PersistentStore_t;


/** Retrieve data from non-volatile (persistent) memory.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @param UnitName The name of the unit of memory to retrieve,
 *            typically a file name.
 *  @param Buffer (output) A pointer to a buffer
 *      that can be freed using the ::CRANE_MemoryFree_t.
 *      This functionstores the retrieved information in this buffer.
 *      The CRANE transmitter (client) will use ::CRANE_MemoryFree_t to free this.
 *  @param BufferLength (output) The number of bytes retrieved.
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_PersistentRetrieve_t CRANE_PersistentRetrieve_t;

/** Pointers to functions implementing the @ref CRANE_non_volatile_memory.
 */
typedef IPDR_PersistenceServices CRANE_PersistenceServices;

/** Create a socket that can be used for TCP communication.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @param S (output) Identifier of the newly created socket.
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_CreateTCPSocket_t CRANE_CreateTCPSocket_t;

/** Create a socket that can be used for UDP communication.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @param S (output) Identifier of the newly created socket.
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_CreateUDPSocket_t CRANE_CreateUDPSocket_t;

/** Bind a socket to a local address.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @param S Socket (created with ::CRANE_CreateTCPSocket_t or ::CRANE_CreateUDPSocket_t).
 *  @param LocalAddr The local address in host byte order.  If this
 *      value is #CRANE_LOCAL_ADDRESS, use the address of the transmitter (client) machine
 *      (i.e., equivalent to 127.0.0.1)
 *  @param LocalPort the local port number in host byte order.
 *  @return #CRANE_OK on success; #CRANE_ERR_ADDRESS_ALREADY_IN_USE,
 *     #CRANE_ERR_COMMUNICATION_ERROR, or #CRANE_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Bind_t CRANE_Bind_t;

/** Close a socket and deallocate any resources associated with
 *  the socket. If you need to leave the socket allocated, use
 *  ::CRANE_Shutdown_t.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @param S Socket (created with ::CRANE_CreateTCPSocket_t or ::CRANE_CreateUDPSocket_t).
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Close_t CRANE_Close_t ;


/** Shutdown a socket for both reading and writing, leaving the socket allocated.
 *  The socket is still allocated, but any operation other than
 *  ::CRANE_Close_t will get an error status.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @param S Socket (created with ::CRANE_CreateTCPSocket_t or ::CRANE_CreateUDPSocket_t).
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Shutdown_t CRANE_Shutdown_t;

/** Modify a socket to accept connections.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @param S Socket (created with ::CRANE_CreateTCPSocket_t.
 *  @param Backlog Maximum number of queued connections.
 *  @return #CRANE_OK on success; #CRANE_ERR_COMMUNICATION_ERROR, or #CRANE_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Listen_t CRANE_Listen_t;


/** Create a socket for the first pending connection.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @param S Socket (created with ::CRANE_CreateTCPSocket_t.
 *          prepared by ::CRANE_Listen_T).
 *  @param Addr (output) The address of the new connection in host byte order.
 *  @param Port (output) The port of the new connection in host byte order.
 *  @param AcceptSocket (output) The newly created socket.
 *  @return #CRANE_OK on success; #CRANE_ERR_COMMUNICATION_ERROR, or #CRANE_STATUS_RETURN_CODE otherwise.
 *  @see CRANE_Listen_t
 */
typedef IPDR_Accept_t CRANE_Accept_t;


/** Set any platform specific options of a TCP socket opened at the
 *  transmitter (client) side (for example it may change the send and receive buffer
 *  size, or change the socket to non blocking). The function receives
 *  a ::CRANE_ParametersHandlerFunc callback function (the same one
 *  passed to CRANE_InitClient()) which can be used to retrieve socket
 *  parameters from an external configuration mechanism.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @param S Socket (created with ::CRANE_CreateTCPSocket_t).
 *  @param ParamFunc Function for retrieving parameter values.
 *  @param ParamArg Context argument as passed to CRANE_InitClient().
 *  @return #CRANE_OK on success; #CRANE_ERR_COMMUNICATION_ERROR, or #CRANE_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_SetExporterSocketParams_t CRANE_SetClientSocketParams_t;


/** Send the contents of a buffer onto a TCP socket.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @param S Socket (created with ::CRANE_CreateTCPSocket_t).
 *  @param Buf Pointer to the bytes to write.
 *  @param BufLength The number of bytes to be written from the buffer.
 *  @return #CRANE_OK on success; #CRANE_ERR_COMMUNICATION_ERROR, or #CRANE_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Send_t CRANE_Send_t;


/** Read into a buffer from a TCP socket.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @param S Socket (created with ::CRANE_CreateTCPSocket_t).
 *  @param Buf Pointer to the buffer where the bytes will be read into.
 *  @param BufLength Maximum number of bytes to read into the buffer.
 *  @param Timeout Maximum time in milliseconds to wait for data (0 means wait forever);
 *          the function returns #CRANE_ERR_OPERATION_WOULD_BLOCK if the timeout expires.
 *  @param ReadBytes (output) How many bytes were actually read.
 *         This will be greater than zero if the return code is
 *         #CRANE_OK; it will be zero for any other return code.
 *         End-of-input is treated as #CRANE_ERR_COMMUNICATION_ERROR.
 *  @return #CRANE_OK on success; #CRANE_ERR_COMMUNICATION_ERROR,
 *        #CRANE_ERR_OPERATION_WOULD_BLOCK, or #CRANE_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_Recv_t CRANE_Recv_t;

/** Read into a buffer from a UDP socket.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @param S Socket (created with ::CRANE_CreateUDPSocket_t).
 *  @param Buf Pointer to the buffer where the bytes will be read into.
 *  @param BufLength Maximum number of bytes to read into the buffer.
 *  @param Timeout Maximum time in milliseconds to wait for data (0 means wait forever);
 *          the function returns #CRANE_ERR_OPERATION_WOULD_BLOCK if the timeout expires.
 *  @param ReadBytes (output) How many bytes were actually read.
 *         This will be greater than zero if the return code is
 *         #CRANE_OK; it will be zero for any other return code.
 *         End-of-input is treated as #CRANE_ERR_COMMUNICATION_ERROR.
 *  @return #CRANE_OK on success; #CRANE_ERR_COMMUNICATION_ERROR, or #CRANE_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_RecvFrom_t CRANE_RecvFrom_t;

/** Send the contents of a buffer onto a TCP socket.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @param S Socket (created with ::CRANE_CreateUDPSocket_t).
 *  @param Buf Pointer to the bytes to write.
 *  @param BufLength The number of bytes to be written from the buffer.
 *  @return #CRANE_OK on success; #CRANE_ERR_COMMUNICATION_ERROR, or #CRANE_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_SendTo_t CRANE_SendTo_t;


/** Get the hostname, in the form needed for syslog. The result is always
 *  null-terminated; if insufficient space is provided, the result will be truncated.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @param HostName The returned host name. On Unix systems, this should be
 *                  at least MAXHOSTNAMELEN long. If the call fails, this
 *                  will be set to the empty string ("").
 *  @param NameLength The size of the \a HostName array.
 *  @return #CRANE_OK on success; #CRANE_ERR otherwise (GetLastErrorMessage ought
 *        to have an indication of the problem).
 *  @see CRANE_SetLogOutputAddress
 */
typedef CRANE_Status (*CRANE_GetHostName_t)(CRANE_PlatformArg P,
                                            char* HostName,
                                            int NameLength);

/** Pointers to functions implementing the @ref CRANE_network_services.
 */
typedef IPDR_NetworkServices CRANE_NetworkServices;

/** The maximum length of the buffer in the time string output functions.
 */
#define CRANE_TIME_FORMAT_LENGTH IPDR_TIME_FORMAT_LENGTH

/** Get the current time of day.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @param Time (output) seconds since "epoch" (1970-01-01T00:00:00 UTC (GMT))
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_GetTime_t CRANE_GetTime_t;

/** Set the current time of day. It is allowed that this function
 *  fails, for example by not having sufficient permissions to set
 *  the time of day (the function is only used by some debug
 *  and testing functions).
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @param Time seconds since "epoch" (1970-01-01T00:00:00 UTC (GMT))
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_SetTime_t CRANE_SetTime_t ;

/** Sleep for a given number of milliseconds.
 *  If the system clock is not sufficiently precise, the time
 *  interval is rounded up to the precision of the clock.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @param Interval The number of milliseconds to sleep.
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
typedef IPDR_SleepMS_t CRANE_SleepMS_t;

/** Format the current time into ISO format in the local time zone.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @param Buf The buffer to get the date/time C-string (must be #CRANE_TIME_FORMAT_LENGTH
 *             bytes long or more).
 *  @return The formatted value (using Buf's space).
 *  @see CRANE_FormatTimeLocal_t
 */
typedef IPDR_FormatTimeLocalNow_t CRANE_FormatTimeLocalNow_t;

/** Format the current time into ISO format in the GMT (UTC) time zone.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @param Buf The buffer to get the date/time C-string (must be #CRANE_TIME_FORMAT_LENGTH
 *             bytes long or more).
 *  @return The formatted value (using Buf's space).
 *  @see CRANE_FormatTimeGMT_t
 */
typedef IPDR_FormatTimeGMTNow_t CRANE_FormatTimeGMTNow_t;

/** Format a time into ISO format in the local time zone.
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @param Time seconds since "epoch" (1970-01-01T00:00:00 UTC (GMT))
 *  @param Buf The buffer to get the date/time C-string (must be #CRANE_TIME_FORMAT_LENGTH
 *             bytes long or more).
 *  @return The formatted value (using Buf's space).
 */
typedef IPDR_FormatTimeLocal_t CRANE_FormatTimeLocal_t;


/** Format a time into ISO format in the GMT (UTC) time zone.
 *  @param Time seconds since "epoch" (1970-01-01T00:00:00 UTC (GMT))
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @param Buf The buffer to get the date/time C-string (must be #CRANE_TIME_FORMAT_LENGTH
 *             bytes long or more).
 *  @return The formatted value (using Buf's space).
 */
typedef IPDR_FormatTimeGMT_t CRANE_FormatTimeGMT_t;

/** Format the current time into syslog format.
 *  This formats the time according to RFC 3164, e.g.:
 *  "<code>Jul&nbsp;&nbsp;1&nbsp;17:23:47</code>".
 *  @param P The context argument when calling platform function, as passed to CRANE_InitClient().
 *  @param Buf The buffer to get the date/time C-string (must be #CRANE_TIME_FORMAT_LENGTH
 *             bytes long or more).
 *  @return The formatted value (using Buf's space).
 */
typedef IPDR_FormatTimeSyslogNow_t CRANE_FormatTimeSyslogNow_t;

/** Pointers to functions implementing the @ref CRANE_time_services.
 */
typedef IPDR_TimeServices CRANE_TimeServices ;

/** The platform services structure contains elements specifying the
 *  various platform specific services as well as Start, Stop and
 *  Delete functions called during transmitter (client) start / stop / delete
 *  operations respectively.
 */
typedef IPDR_PlatformServices CRANE_PlatformServices;

/** Allocate a chunk of memory.
 *  See ::CRANE_MemoryAllocate_t
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param Size The number of bytes to allocate.
 *  @param Buf (output) Pointer to the allocated memory; 0 (NULL) if failure
 *                   (#CRANE_ERR_ALLOCATION_FAILED).
 *  @return #CRANE_OK on success;
 *                 CRANE_ERR_ALLOCATION_FAILED or #CRANE_STATUS_RETURN_CODE otherwise.
 */
#define CRANE_MemoryAllocate(ClientHandle,Size,Buf) IPDR_MemoryAllocate(ClientHandle,Size,Buf)


/** Free a chunk of memory.
 *  See ::CRANE_MemoryFree_t
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param Buf The pointer to the memory allocated by ::CRANE_MemoryAllocate.
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
#define CRANE_MemoryFree(ClientHandle,Buf) IDPR_MemoryFree(ClientHandle,Buf)


/** Get memory usage statistics.
 *  See ::CRANE_MemoryStatistics_t
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param numAllocs The number of calls to the CRANE_MemoryAllocate_t or CRANE_MemoryAllocateWithHdr_t
 *                   services.
 *  @param numFrees The number of calls to the CRANE_MemoryFree_t or CRANE_MemoryFreeWithHdr_t
 *                   services.
 *  @param bytesAllocated (output) The total number of bytes currently allocated.
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
#define CRANE_MemoryStatistics(ClientHandle,numAllocs,numFrees,bytesAllocated) IPDR_MemoryStatistics(ClientHandle,numAllocs,numFrees,bytesAllocated)


/** Register and start a new timer.
 *  See ::CRANE_RegisterTimer_t
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param Callback Function to call when the timer expires.
 *  @param Arg Argument to the callback function.
 *  @param Timeout Timer period in milliseconds.
 *  @param ID (output) Reference to the timer for reregistering or
 *                  deleting the timer.
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 *  @see CRANE_ReregisterTimer
 *  @see CRANE_UnregisterTimer
 */
#define CRANE_RegisterTimer(ClientHandle,Callback,Arg,Timeout,ID) IPDR_RegisterTimer(ClientHandle,Callback,Arg,Timeout,ID)

/** Change the timeout for an existing timer.
 *  See ::CRANE_ReregisterTimer_t
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param Callback Function to call when the timer expires.
 *  @param Arg Argument to the callback function.
 *  @param Timeout Timer period in milliseconds.
 *  @param ID (updated) Reference to the timer from ::CRANE_RegisterTimer_t.
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
#define CRANE_ReregisterTimer(ClientHandle,Callback,Arg,Timeout,ID) IPDR_ReregisterTimer(ClientHandle,Callback,Arg,Timeout,ID)


/** Register and start a periodic timer.
 *  See ::CRANE_RegisterPeriodicTimer_t
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param Callback Function to call when the timer expires.
 *  @param Arg Argument to the callback function.
 *  @param Timeout Timer period in milliseconds.
 *  @param ID (output) Reference to the timer for deleting the timer.
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 *  @see CRANE_UnregisterTimer_t
 */
#define CRANE_RegisterPeriodicTimer(ClientHandle,Callback,Arg,Timeout,ID) IPDR_RegisterPeriodicTimer(ClientHandle,Callback,Arg,Timeout,ID)


/** Cancel and delete an existing timer.
 *  See ::CRANE_UnregisterTimer_t
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param ID The timer reference from ::CRANE_RegisterTImer_t
 *            or ::CRANE_RegisterPeriodictTimer_t.
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 *  @see CRANE_RegisterTimer_t
 *  @see CRANE_RegisterPeriodictTimer_t
 */
#define CRANE_UnregisterTimer(ClientHandle,ID) IPDR_UnregisterTimer(ClientHandle,ID)


/** Get the current time of day.
*   See ::CRANE_GetTime_t
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param Time (output) seconds since "epoch" (1970-01-01T00:00:00 UTC (GMT))
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
#define CRANE_GetTime(ClientHandle,Time) IPDR_GetTime(ClientHandle,Time)


/** Sleep for a given number of milliseconds.
 *  See ::CRANE_SleepMS_t
 *  If the system clock is not sufficiently precise, the time
 *  interval is rounded up to the precision of the clock.
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param Interval The number of milliseconds to sleep.
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
#define CRANE_SleepMS(ClientHandle,Interval) IPDR_SleepMS(ClientHandle,Interval)


/** Format the current time into ISO format in the local time zone.
 *  See ::CRANE_FormatTimeLocalNow_t
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param Buf The buffer to get the date/time C-string (must be #CRANE_TIME_FORMAT_LENGTH
 *             bytes long or more).
 *  @return The formatted value (using Buf's space).
 *  @see CRANE_FormatTimeLocal_t
 */
#define CRANE_FormatTimeLocalNow(ClientHandle,Buf) IPDR_FormatTimeLocalNow(ClientHandle,Buf)


/** Format the current time into ISO format in the GMT (UTC) time zone.
 *  See ::CRANE_FormatTimeLocalGMTNow_t
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param Buf The buffer to get the date/time C-string (must be #CRANE_TIME_FORMAT_LENGTH
 *             bytes long or more).
 *  @return The formatted value (using Buf's space).
 *  @see CRANE_FormatTimeGMT_t
 */
#define CRANE_FormatTimeGMTNow(ClientHandle,Buf) IPDR_FormatTimeGMTNow(ClientHandle,Buf)

/** Format a time into ISO format in the local time zone.
 *  See ::CRANE_FormatTimeLocal_t
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param Time seconds since "epoch" (1970-01-01T00:00:00 UTC (GMT))
 *  @param Buf The buffer to get the date/time C-string (must be #CRANE_TIME_FORMAT_LENGTH
 *             bytes long or more).
 *  @return The formatted value (using Buf's space).
 */
#define CRANE_FormatTimeLocal(ClientHandle,Time,Buf) IPDR_FormatTimeLocal(ClientHandle,Time,Buf)

/** Format a time into ISO format in the GMT (UTC) time zone.
 *  See ::CRANE_FormatTimeGMT_t
 *  @param Time seconds since "epoch" (1970-01-01T00:00:00 UTC (GMT))
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param Buf The buffer to get the date/time C-string (must be #CRANE_TIME_FORMAT_LENGTH
 *             bytes long or more).
 *  @return The formatted value (using Buf's space).
 */
#define CRANE_FormatTimeGMT(ClientHandle,Time,Buf) IPDR_FormatTimeGMT(ClientHandle,Time,Buf)

/** Initialize a new transmitter (client) given the platform services and parameters.
 *
 *  The CRANE transmitter (client) initialization process involves passing a
 *  function pointer which will be used by the transmitter (client) to retrieve its
 *  parameters and a structure that contains function pointer to
 *  platform specific handlers. These implement socket, timer, and
 *  memory allocation operations. Initial parameters should be set
 *  before the transmitter (client) is activated or may be modified by setting
 *  different values to the MIB's relevant OIDs. Changing the
 *  parameters at run time may cause the connections to be restarted.
 *
 *  The implementation is free to use any method it wishes to store
 *  the parameter values. Examples are:
 *  <ul>
 *  <li>As a pre-defined list (the sample code uses this method).
 *  <li>As an external file which is scanned for values.
 *  <li>As internal values which are set via a separate command line
 *  interface or via SNMP (as part of a MIB).
 *  </ul>
 *
 *  The \a ParamsHandlerArg parameter to the parameters handler
 *  function contains the necessary context for the implementation to
 *  fetch the values.
 *
 *  @note
 *
 *  If any subsequent function returns #CRANE_ERR_NOT_RUNNING,
 *  you must call CRANE_DeleteClient() to clean things up before
 *  calling this function again.
 *
 *  It is undefined when the implementation calls the \a ParamsHandler.
 *  It may choose to collect all parameter the values during
 *  initialization; if your \a ParamsHandler changes its values after
 *  initialization, you must stop and start the transmitter (client) to make them
 *  take effect.
 *
 *  If you wish to give precedence to the values in the MIB, you can
 *  get at them with CRANE_MIBGetByName(). You must create the
 *  \a ParamsHandlerArg to contain a pointer to the \a transmitter (client) handle
 *  and test to ensure that the handle is non-NULL before using it
 *  (CRANE_InitClient() will sufficiently instantiate the transmitter (client) to
 *  allow MIB lookups.
 *
 *  @param PS handle to a platform services structure supplied by the
 *                host application, containing pointers to service functions. The
 *                contents of this structure will depend on platform specifics.
 *  @param ClientName name of the transmitter (client) (used for logging
 *                purposes). A copy is made of the string. If this
 *                parameter is NULL or zero-length, a default name is supplied.
 *  @param ParamsHandler callback function for initial values of parameters
 *                needed by the transmitter (client). Any values set by this callback
 *                can be changed before CRANE_StartClient() by using
 *                CRANE_MIBSet().
 *                See additional comments with
 *                @ref CRANE_ParametersHandlerFunc.
 *                If this parameter is NULL, default values are used.
 *  @param ParamsHandlerArg argument to the \a ParamsHandler callback function.
 *  @param LoadMIB callback function for initializing the values in the MIB.
 *                 The function should limit its activity to calls to
 *                 CRANE_MIBRestoreNodeByName().
 *                 If this function fails (returns anything other than #CRANE_OK),
 *                 CRANE_InitClient() will fail with the
 *                 same return value.
 *                 If this parameter is NULL, nothing is done for loading the MIB.
 *  @param LoadMIBArg passed to \a LoadMIB.
 *  @param SaveMIB callback function for saving the values in the MIB.
 *                 If this parameter is NULL, nothing is done for saving the MIB.
 *  @param SaveMIBArg passed to \a SaveMIB.
 *  @param CH (output) handle to the newly created transmitter (client); NULL on failure.
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 *  @see CRANE_PersistenceServices
 *  @see CRANE_MIB_related
 *  @see CRANE_ParametersHandlerFunc
 *  @see CRANE_StartClient
 *  @see CRANE_IsRunning
 */
#define CRANE_InitClient(PS,ClientName,ParamsHandler,ParamsHandlerArg,LoadMIB,LoadMIBArg,SaveMIB,SaveMIBArg,CH)\
			IPDR_InitExporter(PS,ClientName,ParamsHandler,ParamsHandlerArg,LoadMIB,LoadMIBArg,SaveMIB,SaveMIBArg,CH)


/** Start a previously created transmitter (client).
 *  After the transmitter (client) has been initialized and sessions and templates
 *  have been created, the transmitter (client) can be started. For <em>some</em>
 *  implementations of the platform services, this will result in
 *  starting a new thread.
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 *  @see CRANE_IsRunning()
 *  @see CRANE_InitClient()
 *  @see CRANE_DeleteClient()
 */
#define CRANE_StartClient(ClientHandle) IPDR_StartExporter(ClientHandle)


/** Test whether the transmitter (client) is running.
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @return 0 for not running, 1 for running.
 *  @see CRANE_ERR_NOT_RUNNING
 */
#define CRANE_IsRunning(ClientHandle) IPDR_IsRunning(ClientHandle)


/** Statistics for a session / collector (server).
 *  These can be used for a session or a collector (server): for a session, they
 *  sum all the collectors (servers); for a collector (server), the give the values when the
 *  collector was active. For example, if 100 records wer enqueued when
 *  "collector-1" was active and 200 records were enqueued when
 *  "collector-2" was active (and both are part of "session-1") and 50
 *  records were enqueued when no collector (server) was active, then the session
 *  will show a total of 350.
 */
typedef IPDR_Statistics CRANE_Statistics;


/** Get statistics for a session.
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param SessionHandle Session handle returned from CRANE_CreateSession().
 *  @param Now (output) the current time (seconds since 1970-01-01T00:00:00Z).
 *            Use NULL if you don't want this value.
 *  @param Statistics (output) the statistics for the session.
 *  @param QueuedOutstandingRecords (output) the number of records
 *            currently on the output queue (unsent or unacknowledged).
 *            Use NULL if you don't want this value.
 *  @param QueuedUnacknowledgedRecords (output) the number of records
 *            that currently have not been acknowledged. Use NULL
 *            if you don't want this value.
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
#define CRANE_GetSessionStatistics(ClientHandle,SessionHandle,Now,Statistics,QueuedOutstandingRecords,QueuedUnacknowledgedRecords)\
			IPDR_GetSessionStatistics(ClientHandle,SessionHandle,Now,Statistics,QueuedOutstandingRecords,QueuedUnacknowledgedRecords)


/** Get statistics for a collector (server) within a session and get information
 *  about its state.
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param SessionHandle Session handle returned from CRANE_CreateSession().
 *  @param Addr The collector's (server's) IP address.
 *  @param Port The collector's (server's) port.
 *  @param Now (output) the current time (seconds since 1970-01-01T00:00:00Z).
 *            Use NULL if you don't want this value.
 *  @param Statistics (output) the statistics for the collector (server).
 *            Use NULL if you don't want this value.
 *  @param LastEvent (output) the last state-change "event" that was
 *            triggered;
 *            This can be used to get the state and other current
 *            information about the collector (server). [Non-state events,
 *            such as #CRANE_EVENT_RECORD_LOST, are ignored.]
 *            Use NULL if you don't want this value.
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
#define CRANE_GetServerStatisticsAndState(ClientHandle,SessionHandle,Addr,Port,Now,Statistics,LastEvent)\
			IPDR_GetServerStatisticsAndState(ClientHandle,SessionHandle,Addr,Port,Now,Statistics,LastEvent)

/** Stop and delete a CRANE transmitter (client), and release all resources.
 *  Any connections to a collector (server) are broken.
 *
 *  If you get an unexpected response of #CRANE_ERR_NOT_RUNNING,
 *  you must call this function before calling CRANE_InitClient().
 *
 *  This does <em>not</em> verify that all the data waiting in the
 *  output queue has been written or acknowledged.
 *  To ensure that all data has been acknowledged, use
 *  CRANE_DrainQueue() or use CRANE_GetSessionStatistics() and
 *  ::CRANE_EventCallbackDetailed with the #CRANE_EVENT_QUEUE_EMPTY
 *  event.
 *
 *  This function is <em>not</em> thread-safe; you must ensure that
 *  no other CRANE_<i>xxx</i> function is active or called before
 *  this function returns.
 *
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 *  @see CRANE_InitClient
 */
#define CRANE_DeleteClient(ClientHandle) IPDR_DeleteExporter(ClientHandle)


/** Wait until all the records in the queue have been sent and acknowledged.
 *  Because there is no guarantee that the queue will ever drain (all the
 *  collectors (servers) could be disconnected and they won't reconnect), a time limit
 *  can also be specified. If the queue did not drain in time, this function
 *  returns #CRANE_ERR_OPERATION_WOULD_BLOCK -- it is always possible that
 *  the queue has drained asynchronously so you should also check the
 *  \a QueuedUnacknowledgedRecords value from CRANE_GetSessionStatistics().
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param SessionHandle Session Handle as returned by CRANE_CreateSession().
 *  @param Timeout Wait at most this long (in milliseconds) for the queue to
 *                 drain; a value of zero means wait forever.
 *  @return #CRANE_OK on success; #CRANE_ERR_OPERATION_WOULD_BLOCK
 *            #CRANE_STATUS_RETURN_CODE otherwise.
 *  @see CRANE_DeleteClient
 */
#define CRANE_DrainQueue(ClientHandle,SessionHandle,Timeout) IPDR_DrainQueue(ClientHandle,SessionHandle,Timeout)

/** Create a new session on the transmitter (client).
 *  @ingroup CRANE_sessions
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param Name session name (an internal copy is made of this string).
 *  @param ID Session ID.
 *  @param Description session description (an internal copy is made of this string).
 *  @param SH (output) handle to the newly created session.
 *  @return #CRANE_OK on success; #CRANE_ERR_SESSION_ALREADY_EXISTS,
 *          or
 *          #CRANE_STATUS_RETURN_CODE otherwise.
 */
#define CRANE_CreateSession(ClientHandle,Name,ID,Description,SH) IPDR_CreateSession(ClientHandle,Name,ID,Description,SH)


/** Delete a previously created session and release
 *  all resources associated with it.
 *  @ingroup CRANE_sessions
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param SessionHandle Handle to session as returned by CRANE_CreateSession().
 *  @return #CRANE_OK on success; #CRANE_ERR_UNABLE_TO_FIND_SESSION,
 *   or #CRANE_STATUS_RETURN_CODE otherwise.
 */
#define CRANE_DeleteSession(ClientHandle,SessionHandle) IPDR_DeleteSession(ClientHandle,SessionHandle)


/** Get the handle to a session given the session's ID.
 *  @ingroup CRANE_sessions
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param ID Session ID to find.
 *  @param SH (output) Handle to the session as returned by CRANE_CreateSession().
 *  @param Name (output) Name of the session as provided to CRANE_CreateSession().
 *               This parameter can be NULL if you don't want the value.
 *  @param Description (output) Description of the session as provided to CRANE_CreateSession().
 *               This parameter can be NULL if you don't want the value.
 *  @return #CRANE_OK on success; #CRANE_ERR_UNABLE_TO_FIND_SESSION or
 *           #CRANE_STATUS_RETURN_CODE otherwise.
 */
#define CRANE_GetSession(ClientHandle,ID,SH,Name,Description) IPDR_GetSession(ClientHandle,ID,SH,Name,Description)


/** Get a list of all the session IDs.
 *  @ingroup CRANE_sessions
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param NumSessions (output) number of sessions.
 *  @param Sessions (output) array of collector (server) IDs, which is allocated internally
 *           and remains valid until the transmitter (client) is deleted by a call to
 *           CRANE_DeleteClient(); or until another call to CRANE_GetSessionsList().
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
#define CRANE_GetSessionsList(ClientHandle,NumSessions,Sessions) IPDR_GetSessionsList(ClientHandle,NumSessions,Sessions)


/** Add a collector (server) to a session's collectors (servers) list, so that data can be sent to it.
 *  @ingroup CRANE_sessions
 *
 *  collectors (servers) are assigned a priority level. Data is always sent to the
 *  active collector (server) with the highest priority (if two collectors (servers) have the
 *  same priority, it is undefined which will be chosen to be active).
 *  Zero is highest priority.
 *
 *  collectors (servers) can be added dynamically. Data will be sent to a collector (server) as
 *  soon as a (transport layer) connection has been established and
 *  the collector (server) has indicated that it is ready to receive data by
 *  sending a START message and there is no higher priority collector (server)
 *  active.
 *
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param SessionHandle Handle to session to add collector (server) to.
 *  @param Name Name of collector (server) (an internal copy is made of this string).
 *  @param Addr IP address of the collector (server).
 *  @param Port Port number of the collector (server).
 *  @param Priority collector's (server's) priority (0 - highest priority; behavior
 *            undefined for negative values).
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 *
 * IPDR Compatibility:
 * This function implemented as calling
 *
*/
#define CRANE_AddServer(ClientHandle,SessionHandle,Name,Addr,Port,Priority) \
		IPDR_AddCollectorCompat(ClientHandle,SessionHandle,Name,Addr,Port,Priority)


/** Remove a previously added collector (server).
 *
 *  collectors (servers) can be deleted dynamically, causing lower priority collectors (servers)
 *  to be activated. Removal of a collector (server) causes immediate
 *  disconnection, potentially losing acknowledgments from the collector (server)
 *  and duplicate transmission to the alternate collector (server).
 *  @ingroup CRANE_sessions
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param SessionHandle Handle to session to remove collector (server) from.
 *  @param Addr address of collector (server).
 *  @param Port port number of the collector (server).
 *  @return #CRANE_OK on success; #CRANE_ERR_SESSION_ALREADY_EXISTS or
 *     #CRANE_STATUS_RETURN_CODE otherwise.
 */
#define CRANE_RemoveServer(ClientHandle,SessionHandle,Addr,Port)


/** Retrieve the current list of collectors (servers) configured for a given session.
 *  @ingroup CRANE_sessions
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param SessionHandle Handle to session to retrieve collector(s) (server(s)) from.
 *  @param NumServers (output) number of collectors (servers).
 *  @param Servers (output) array of collector (server) structures, which is allocated internally
 *           and remains valid until the session is deleted by a call to
 *           CRANE_DeleteSession() or CRANE_DeleteClient(); or until
 *           another call to CRANE_GetServersList() is done.
 *  @return #CRANE_OK on success; #CRANE_ERR_UNABLE_TO_FIND_SESSION or
 *       #CRANE_STATUS_RETURN_CODE otherwise.
 */
#define CRANE_GetServersList(ClientHandle,SessionHandle,NumServers,Servers)\
			IPDR_GetCollectorInSessionList(ClientHandle,SessionHandle,NumServers,Servers)


/** Create a new template within the transmitter (client).
 *
 *  This function is used to create a new template, with or without
 *  keys. Additional keys can be added to a template after its
 *  creation.
 *
 *  A template applies to all sessions. The collector may declare that
 *  it is not interested in receiving a template by disabling all the
 *  keys, in which case any ouptut of the template via
 *  CRANE_SendData() will do nothing. (For efficiency, the host
 *  application might decide not to even call CRANE_SendData() -- it
 *  will know that all the keys have been disabled by the
 *  ::CRANE_TemplatesChangeCallback being called with all keys having
 *  #CRANE_KEY_DISABLED set in the CRANE_Key.KeyAttributes field.
 *
 *  During the transmitter (client) initialization process, all key data structures
 *  passed as arguments to this function are defined and
 *  activated. Therefore, a platform's core templates should be
 *  created in this way.
 *
 *  Templates can also be created, and keys may be added to templates,
 *  dynamically (in run time). Templates and keys so created will
 *  become active only when CRANE_ApplyTemplates() is called. They
 *  will not persist after power-down and will not be re-created in
 *  the next initialization.
 *
 *  When all keys in a template are accessed using offsets and all
 *  have a fixed length, it is recommended to group all mandatory keys
 *  at the beginning for efficiency purposes.
 *
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param TemplateID ID of template.
 *  @param Description Template description (an internal copy is made of this string).
 *  @param NumKeys Number of keys within the template (may be zero for an empty template).
 *  @param Keys Array of keys associated with the template (an internal
 *                copy is made of this array and all its contents).
 *  @param TH (output) Handle to the newly created template.
 *  @return #CRANE_OK on success; #CRANE_ERR_TEMPLATE_ALREADY_EXISTS or
 *             #CRANE_STATUS_RETURN_CODE otherwise.
 *  @see CRANE_ApplyTemplates
 *  @see CRANE_AddKey
 *  @see CRANE_DeleteKey
 */

#define CRANE_CreateTemplate(ClientHandle, TemplateID, Description, NumKeys, Keys, TH) \
	IPDR_CreateTemplate_All(ClientHandle, TemplateID, Description, NULL, NumKeys, Keys)

/** Create a new template within the transmitter (client), for status responses.
 *  See CRANE_CreateTemplate() for more details. The only difference
 *  between this and CRANE_CreateTemplate() is that templates created
 *  by this call are used for STATUS RSP messages.
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param TemplateID ID of template.
 *  @param Description Template description (an internal copy is made of this string).
 *  @param NumKeys Number of keys within the template (may be zero for an empty template).
 *  @param Keys Array of keys associated with the template (an internal
 *                copy is made of this array and all its contents).
 *  @param TH (output) Handle to the newly created template.
 *  @return #CRANE_OK on success; #CRANE_ERR_TEMPLATE_ALREADY_EXISTS or
 *             #CRANE_STATUS_RETURN_CODE otherwise.
 *  @see CRANE_ApplyTemplates
 *  @see CRANE_AddKey
 *  @see CRANE_DeleteKey
 */
CRANE_Status CRANE_CreateStatusTemplate(CRANE_ClientHandle ClientHandle,
		                        int TemplateID,
				        const char *Description,
				        int NumKeys,
				        const CRANE_Key *Keys,
					CRANE_TemplateHandle *TH);

					
/** Retrieve a template handle given a template ID.
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param TemplateID ID of template.
 *  @param TH (output) Handle to the found template.
 *  @return #CRANE_OK on success; #CRANE_ERR_TEMPLATE_NOT_FOUND or
 *               #CRANE_STATUS_RETURN_CODE otherwise.
 *  @see CRANE_CreateTemplate
 */
CRANE_Status CRANE_GetTemplate(CRANE_ClientHandle ClientHandle,
			       int TemplateID,
			       CRANE_TemplateHandle *TH);

			       
/** Delete a previously created template.
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param TH Handle of template (created by CRANE_CreateTemplate()) to remove.
 *  @return #CRANE_OK on success; #CRANE_ERR_TEMPLATE_NOT_FOUND or
 *                         #CRANE_STATUS_RETURN_CODE otherwise.
 */
CRANE_Status CRANE_DeleteTemplate(CRANE_ClientHandle ClientHandle,
				  CRANE_TemplateHandle TH);


/** Add a key to a previously created template.
 *
 *  If this function is called after the transmitter (client) has already been
 *  started, the key will be added only after CRANE_ApplyTemplates()
 *  is called.
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param TH Template handle previously created by CRANE_CreateTemplate().
 *  @param CKey Key to add (an internal copy is made of this structure and
 *              all its contents).
 *  @return #CRANE_OK on success; #CRANE_ERR_KEY_ALREADY_EXISTS,
 *       #CRANE_ERR_TEMPLATE_NOT_FOUND, or #CRANE_STATUS_RETURN_CODE
 *       otherwise.
 *  @see CRANE_ApplyTemplates
 */
//CRANE_Status CRANE_AddKey(CRANE_ClientHandle ClientHandle,
//			  CRANE_TemplateHandle TH,
//			  const CRANE_Key * CKey);


/** Remove a key from a template.
 *
 *  If this function is called after the transmitter (client) has already been
 *  started, the key will be added only after CRANE_ApplyTemplates()
 *  is called.
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param TH Template handle previously created by CRANE_CreateTemplate().
 *  @param KeyID ID of key to remove from the template.
 *  @return #CRANE_OK on success; #CRANE_ERR_KEY_ALREADY_EXISTS,
 *       #CRANE_ERR_TEMPLATE_NOT_FOUND, or #CRANE_STATUS_RETURN_CODE
 *       otherwise.
 */
//CRANE_Status CRANE_RemoveKey(CRANE_ClientHandle ClientHandle,
//			     CRANE_TemplateHandle TH,
//			     unsigned int KeyID);

/** Apply a new template configuration during transmitter (client) runtime.
 *
 *  Changes to templates, including template creation and add/remove
 *  keys, take effect only after this function runs to successful
 *  completion.
 *
 *  This function causes a flush of the record queue and will block
 *  until the records have been flushed.
 *
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 *  @see CRANE_AddKey
 *  @see CRANE_RemoveKey
 */
#define CRANE_ApplyTemplates(ClientHandle) IPDR_ApplyTemplates_All(ClientHandle)


/** Callback for when a template is changed by the collector (server).
 *   During the template negotiation, the collector (server) can request that
 *  certain fields be not transmitted. The CRANE transmitter (client) software will
 *  agree to this and not transmit these fields; this callback is
 *  called to inform the host application that certain fields are not
 *  needed (possibly improving its performance). The unneeded fields
 *  can safely be left unitialized when a call to CRANE_SendData() is
 *  done.
 *
 *  There is no record of before/after for fields; processing should be
 *  something like this:
 *  @verbatim
 *     for (i = 0; i < NumTemplates; i++) {
 *       const CRANE_Template *T = Templates[i];
 *       for (j = 0; j < T->NumKeys; j++) {
 *         const CRANE_Key *K = T->Keys[j];
 *         if (K->KeyAttributes & CRANE_KEY_DISABLED) {
 *           // handle: field K->KeyID is disabled
 *         } else {
 *           // handle: field K->KeyID is enabled (if it was previously disabled)
 *         }
 *       }
 *     }
 *  @endverbatim
 *
 *  @param NumTemplates The number of templates that have been changed.
 *  @param Templates The templates that have changed.
 *  @param Arg The \a Arg parameter set by CRANE_SetTemplatesChangeCallback().
 *  @see CRANE_SetTemplatesChangeCallback
 */
typedef IPDR_TemplateChangedCallback CRANE_TemplatesChangeCallback;


/** Set a callback which will be called whenever the templates configuration changes.
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param Callback Callback function.
 *  @param Arg - extra parameter passed to the callback function
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
#define CRANE_SetTemplatesChangeCallback(ClientHandle,Callback,Arg) IPDR_SetTemplatesChangeCallback(ClientHandle,Callback,Arg)

/** Send data using the CRANE protocol.
 *
 *  This function is called by the host application when the data to
 *  be delivered is ready to be transmitted. It copies data suitable to
 *  populate a specific template, to all currently active sessions
 *  that were configured to receive this template. (If all keys in the
 *  given template were disabled for a specific session, the record
 *  will not be sent in that session.) Once the data is enqueued it is
 *  sent over to the transport layer to each active collector (server) in each
 *  applicable session (and with appropriate fail-over processing
 *  within sessions).
 *
 *  The #CRANE_EVENT_RECORD_LOST event will be triggered for each
 *  session for which there is no space in the output queue (see
 *  CRANE_SetClientEventHandler()). The record-lost event can occur
 *  even if a #CRANE_OK return code is returned from this function. The
 *  record-lost event may also occur asynchornously from the
 *  CRANE_SendData() function.
 *
 *  As with all other API functions, it is safe for multiple threads to
 *  call this function simultaneously; mutexes will ensure
 *  multi-thread safety.  If the underlying platform supports
 *  non-blocking I/O, this function will not block on I/O: if the data
 *  cannot be sent immediately, it will be sent asynchronously later.
 *
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param TemplateHandle Handle of template to which the data comform.
 *  @param Data Pointer to the actual data (which serves as the
 *          base address when using offsets or passed to
 *          accessor functions
 *  @return #CRANE_OK on success; #CRANE_ERR_ALLOCATION_FAILED
 *     (sometimes, when the queue is full), #CRANE_ERR_QUEUE_FULL, or
 *     #CRANE_STATUS_RETURN_CODE otherwise.
 */
#define CRANE_SendData(ClientHandle,TemplateHandle,Data) \
			IPDR_SendDataCompat(ClientHandle,TemplateHandle,Data)

/** The root OID for all CRANE MIB items; a @ref CRANE_OID is taken
 *  relative to this value
 *  (<code>iso.org.dod.internet.private.crane.client</code> or <code>1.3.6.1.4.4316.2</code>).
 *  @see CRANE_MIB_ROOT_OID_INTS
 */
#define CRANE_MIB_ROOT_OID IPDR_MIB_ROOT_OID


/** The root node name for all CRANE MIB items; it has the
 *  same meaning as #CRANE_MIB_ROOT_OID.
 */
#define CRANE_MIB_ROOT_NODE_NAME IPDR_MIB_ROOT_NODE_NAME


/** The root OID as a list of integers (see @ref CRANE_MIB_ROOT_OID_LENGTH).
 */
#define CRANE_MIB_ROOT_OID_INTS IPDR_MIB_ROOT_OID_INTS


/** The root OID's number of dotted items (see @ref CRANE_MIB_ROOT_OID_INTS).
 */
#define CRANE_MIB_ROOT_OID_LENGTH IPDR_MIB_ROOT_OID_LENGTH


/** The maximum number of integers in a @ref CRANE_OID.
 */
#define CRANE_MAX_OID_SIZE IPDR_MAX_OID_SIZE


/** The MIB-related functions use Object Identifiers (OIDs) in order
 *  to specify an object within the CRANE transmitter (client) MIB; all object
 *  identifiers are relative to the CRANE transmitter (client) MIB root OID
 *  #CRANE_MIB_ROOT_OID.
 */
typedef IPDR_OID CRANE_OID;

/** For translating from an OID to a name, there is a structure
 *  similar to a @ref CRANE_OID, except it has names instead of OIDs.
 */
typedef IPDR_OIDname CRANE_OIDname;


/** Get the MIB version identifier (for synchronizing store and load).
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @return Version (as passed to ::CRANE_LoadMIBFunc and ::CRANE_SaveMIBFunc);
 *          -1 is returned if there was an error (e.g., invalid \a Client).
 */
#define CRANE_MIBVersion(ClientHandle) IPDR_MIBVersion(ClientHandle)


/** Perform a get operation from the CRANE transmitter (client) MIB, given an OID.
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param O OID to get value of (relative to #CRANE_MIB_ROOT_OID). This is an input
 *           value but any trailing zeros are removed.
 *  @param Value (output) pointer to string representation of the OID's value.
 *               This parameter can be NULL if you don't want the value.
 *  @param Type (output) OID's value type, can be one of<br>
 *                                   #CRANE_TYPE_IPV4_ADDR,<br>
 *                                   #CRANE_TYPE_BOOLEAN,<br>
 *                                   #CRANE_TYPE_UINT_32,<br>
 *                                   #CRANE_TYPE_INT_32,<br>
 *                                   #CRANE_TYPE_UINT_64,<br>
 *                                   #CRANE_TYPE_INT_64,<br>
 *                                   #CRANE_TYPE_STRING (which gives a C-string).
 *               This parameter can be NULL if you don't want the type.
 * @param Persistent (output) 1 if the value is one that should
 *               be preserved across stopping and starting the transmitter (client); 0 otherwise.
 *               This parameter can be NULL if you don't want the persistency value.
 * @param Writable (output) 1 if the value can be modified by
 *               CRANE_MIBSet(); 0 otherwise.
 *               This parameter can be NULL if you don't want to know about writability.
 * @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 * @see CRANE_MIBGetByName
 * @see CRANE_ParametersHandlerFunc
 */
#define CRANE_MIBGet(ClientHandle,O,Value,Type,Persistent,Writable) IPDR_MIBGet(ClientHandle,O,Value,Type,Persistent,Writable)


/** Perform a get operation from the CRANE transmitter (client) MIB, given a name.
 *  For example, a lookup with "<code>configuration.protocolInfoPort</code>"
 *  will yield the OID 6.17 and the value (if any) that is already in the
 *  MIB. This can be useful with #CRANE_ParametersHandlerFunc, to allow
 *  keeping a current value.
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param Name The name to look up. For example: "<code>configuration.protocolInfoPort</code>".
 *              The name may contain a mixture of names and numbers, so that
 *              "<code>6.17</code>", "<code>6.protocolInfoPort</code>" and
 *              "<code>configuration.protocolInfoPort</code>"
 *              will all give the same result (this capability is mainly
 *              to allow indexes at the ends of OID names.
 *  @param O (output) OID corresponding to the name (relative to #CRANE_MIB_ROOT_OID).
 *               For example: 6.17.
 *               This parameter can be NULL if you don't want the OID.
 *               The value returned is undefined if the lookup was not successful.
 *  @param Value (output) pointer to string representation of the OID's value.
 *               This parameter can be NULL if you don't want the value.
 *  @param Type (output) OID's value type, can be one of<br>
 *                                   #CRANE_TYPE_IPV4_ADDR,<br>
 *                                   #CRANE_TYPE_BOOLEAN,<br>
 *                                   #CRANE_TYPE_UINT_32,<br>
 *                                   #CRANE_TYPE_INT_32,<br>
 *                                   #CRANE_TYPE_UINT_64,<br>
 *                                   #CRANE_TYPE_INT_64,<br>
 *                                   #CRANE_TYPE_STRING (which gives a C-string).
 *               This parameter can be NULL if you don't want the type.
 * @param Persistent (output) 1 if the value is one that should
 *               be preserved across stopping and starting the transmitter (client); 0 otherwise.
 *               This parameter can be NULL if you don't want the persistency value.
 * @param Writable (output) 1 if the value can be modified by
 *               CRANE_MIBSet(); 0 otherwise.
 *               This parameter can be NULL if you don't want to know about writability.
 * @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 * @see CRANE_MIBGet
 * @see CRANE_ParametersHandlerFunc
 */
#define CRANE_MIBGetByName(ClientHandle, Name,O,Value,Type,Persistent,Writable)\
			IPDR_MIBGetByName(ClientHandle, Name,O,Value,Type,Persistent,Writable)


/** Perform a get next operation on a given OID, to get the
 *  next OID in the MIB in "SNMP walk" order.
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param CurrentOID (input) The current OID  (relative to #CRANE_MIB_ROOT_OID).
 *         If this is zero length (CurrentOID.Length == 0) [#Crane_ZeroOID], the
 *         first object will be retrieved.
 *  @param NextOID (output) Next OID in the MIB (relative to #CRANE_MIB_ROOT_OID).
 *         This may be the same as \a CurrentOID.
 *  @return #CRANE_OK on success; #CRANE_ERR if no more OIDs;
 *     #CRANE_STATUS_RETURN_CODE otherwise.
*/
#define CRANE_MIBGetNext(ClientHandle,CurrentOID,NextOID)\
			IPDR_MIBGetNext(ClientHandle,CurrentOID,NextOID)


/** Set a value of an OID in the MIB, if the node already exists.
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param O OID to set (relative to #CRANE_MIB_ROOT_OID).
 *  @param Value OID's value as a null-terminated C-string.
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
#define CRANE_MIBSet(ClientHandle,O,Value)\
			IPDR_MIBSet(ClientHandle,O,Value)


/** Set a value of an OID in the MIB, by name. This differs from
 *  CRANE_MIBSet() in that the node need not already exists --
 *  typically, this is used for re-loading a MIB that has been saved
 *  (by saving the values for all the persistent entries: see using
 *  CRANE_MIBGetNext()).  This function should only be called after
 *  CRANE_InitClient() [which sets up the MIB] and before
 *  CRANE_StartClient() which will consume various values in the MIB].
 *  If called after CRANE_StartClient(), its behavior will be the same
 *  as CRANE_MibSet().
 *
 *  If called before CRANE_StartClient() has been called, no internal
 *  callbacks are envoked for the value (for example, changing the
 *  console port would normally cause the console to be restarted; but this
 *  isn't done because the transmitter (client) isn't yet started).
 *
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param Name The node name, as a C-string --- can be either
 *    in name form (e.g., <code>"sessionsTable.sessionsEntry.sessionName.1"</code>)
 *    or in numeric form (e.g., <code>"1.1.2.1"</code>).
 *  @param Type Object's type, can be one of<br>
 *                                   #CRANE_TYPE_IPV4_ADDR,<br>
 *                                   #CRANE_TYPE_BOOLEAN,<br>
 *                                   #CRANE_TYPE_UINT_32,<br>
 *                                   #CRANE_TYPE_INT_32,<br>
 *                                   #CRANE_TYPE_UINT_64,<br>
 *                                   #CRANE_TYPE_INT_64,<br>
 *                                   #CRANE_TYPE_STRING (which must be a C-string).
 *  @param Value Object's value as a null-terminated C-string.
 *  @param O (output) OID that was set (relative to #CRANE_MIB_ROOT_OID).
 *     This parameter can be NULL if you don't want the value.
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 */
#define CRANE_MIBRestoreNodeByName(ClientHandle, Name,Type,Value,O)\
			IPDR_MIBRestoreNodeByName(ClientHandle, Name,Type,Value,O)


/** Look up an OID and return as a string.
 *  This always succeeds because any lookup that fails
 *  simply returns numbers (as strings) -- this is because
 *  an OID can have trailing nodes which are actually
 *  array indexes (e.g., "<code>sessionsTable.sessionsEntry.sessionID.1</code>").
 *  Names are given with the same root point as OIDs (that is, the initial
 *  "<code>iso.org.dod.iternet.private.crane.client</code>" is omitted).
 *
 *  Setting a value can cause parts of the transmitter (client) to restart. The following
 *  MIB settings cause restarts:
 *  <br>Any sub-node of 6 (<code>configuration</code>).
 *  <br>commands.addServer.addServerExecute
 *  <br>commands.removeServer.removeServerExecute
 *
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param OID The OID to set.
 *  @param NodeName (output) The resulting node name.
 *  @param ExtraBuf (updated) A string buffer, for constructing any integers that
 *                  are needed. This should be of size:
 *                  <code>CRANE_MAX_OID_SIZE * sizeof "-2147483647"</code>
 *  @return #CRANE_OK on success; #CRANE_STATUS_RETURN_CODE otherwise.
 *  @see CRANE_MIB_ROOT_OID
 *  @see CRANE_MIB_ROOT_NODE_NAME
 *  @see CRANE_MIBGet
 *  @see CRANE_ParametersHandlerFunc
 */
#define CRANE_OIDtoName(ClientHandle,OID,NodeName,ExtraBuf)\
			IPDR_OIDtoName(ClientHandle,OID,NodeName,ExtraBuf)

#if defined(CRANE_UTILITIES) || defined(DOXYGEN_PROCESSING)

#include <stdio.h>


/** Execute a console command and output the result to a file.
 *  Typically, the caller will read a line, then call this
 *  function to evaluate it and output its results. This is the
 *  same function as is used by the console, so the command
 *  "<code>help</code>" will generate a list of commands.
 *  Comments can be added to a line by preceding them with
 *  a "<code>;</code>". Blank lines and comment lines generate
 *  no output. It is undefined what happens if this function is
 *  simultaneously by multiple threads (e.g., by a debugging
 *  main thread and the console reader).
 *
 *  "#" is the comment symbol: anything after "#" is ignored.
 *
 *  Multiple commands can be separated by ";".
 *
 *  Parameters with embedded white space can have be quoted with
 *  either the single or the double quote symbol.
 *
 *  If there are multiple commands in a line (separated
 *  by ";"), \a ParsedCommand, \a ParsedArgc, and \a ParsedArgs
 *  will be for the last command that was executed.
 *
 *  If a command fails, further commands in the line are not executed.
 *
 * <table>
 * <tr><td><b>console commands</b></td></tr>
 * <tr><td>help</td>
 *      <td>Print out brief help information for the commands.</td></tr>
 * <tr><td>helpm <i>module</i></td>
 *      <td>Print out brief help information for a module's commands.</td></tr>
 * <tr><td>helpc <i>command</i></td>
 *      <td>Print out brief help information for a command.</td></tr>
 * <tr><td>exit</td>
 *      <td>Exit from the console.</td></tr>
 * <tr><td>stop</td>
 *      <td>Exit from the console.</td></tr>
 * <tr><td><b>log commands</b></td></tr>
 * <tr><td>get_debug_level [ <i>module</i> ]</td>
 *      <td>Display the logging level for one or all the modules
 *          (see CRANE_SetModuleDebugLevel()).</td></tr>
 * <tr><td>set_debug_level <i>module</i>|* <i>level</i></td>
 *      <td>Set the logging level for one or all modules (the "*"
 *          argument means "all").
 *          See CRANE_SetModuleDebugLevel().</td></tr>
 * <tr><td>get_logger_addr</td>
 *      <td>Display the UDP (syslog) address and port.
 *          See CRANE_SetLogOutputAddress().</td></tr>
 * <tr><td>set_logger_addr <address> <i>port</i></td>
 *      <td>Set the UDP (syslog) address and port.
 *          See CRANE_SetLogOutputAddress().</td></tr>
 * <tr><td><b>transmitter (client) commands:</b></td></tr>
 * <tr><td>add_session <i>id</i> <i>name</i> <i>description</i></td>
 *      <td>See CRANE_CreateSession().</td></tr>
 * <tr><td>delete_session <id></td>
 *      <td>See CRANE_DeleteSession().</td></tr>
 * <tr><td>add_server <i>session_id</i> <i>name</i> <i>ip</i> <i>port</i> <i>priority</i></td>
 *      <td>See CRANE_AddServer().</td></tr>
 * <tr><td>delete_server <i>session_id</i> <i>ip</i> <i>port</i></td>
 *      <td>See CRANE_RemoveServer().</td></tr>
 * <tr><td>get_sessions</td>
 *      <td>Displays all the sessions, their collectors (servers), and statistics for
            them.</td></tr>
 * <tr><td>apply_templates</td>
 *      <td>See CRANE_ApplyTemplates().</td></tr>
 * <tr><td>get_templates [ <i>session_id</i> ]</td>
 *      <td>Display all the tmplates for all sessions, or for
 *           a single session.</td></tr>
 * <tr><td>version</td>
 *      <td>Show version information.</td></tr>
 * <tr><td>time</td>
 *      <td>Show the current time, when the transmitter (client) was started (the
 *          "boot" time in the protocol START ACK record) and how
 *          long it has been running.</td></tr>
 * <tr><td>set_time <i>epoch_seconds</i></td>
 *      <td>Set the time using a value from 1970-01-01 in seconds.
 *          This is intended for hosts which do not have a mechanism
 *          for setting the clock; and would be envoked by something
 *          like this (assuming that the host is on 192.168.8.88 and
 *          port 40000 is the console port):
 *  <xmp>
 *   perl -e '$|=1; sleep 1;
 *         print "time\nset_time ", time(), "\n";
 *         sleep 1' |
 *     telnet 192.168.8.88 40000
 *  </xmp>
 *      </td></tr>
 * <tr><td>parameters</td>
 *      <td>Show the parameter values as returned by
 *          CRANE_ParametersHandlerFunc().</td></tr>
 * <tr><td>mib [ <i>node</i> ]</td>
 *      <td>Display the MIB below a particular node (or all the
 *          MIB if the node is omitted).</td></tr>
 * <tr><td>mibset <i>node</i> <i>value</i></td>
 *      <td>Set one node in the MIB (see CRANE_MIBSet()).</td></tr>
 * <tr><td>log [ <i>print</i> | <i>callback</i> ]</td>
 *      <td>With no arguments, show the output status of the log; otherwise
 *          set the status (see CRANE_SetLogOutputCallbackDetailed()):
 *          <dl><dt>print</dt><dd>Override the callback
 *              and instead output to <code>stderr</code> (this
 *              is maingly for debugging).</dd>
 *              <dt>callback</dt><dd>Normal callback behavior.</dd>
 *          </dl>
 *      </td></tr>
 *  </table>
 *
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param Output The output file, which must be already opened.
 *  @param Cmd The command line (null-terminated C string). This is
 *     modified as a by-product of parsing, so if you should make a copy
 *     of it first if you wish to use it afterwards.
 *  @param ParsedCommand (output) Gets the command (if any)
 *     This parameter can be NULL if you don't want the value.
 *  @param ParsedArgc (output) Gets the number of arguments (zero if none).
 *     This parameter can be NULL if you don't want the value.
 *  @param ParsedArgs (output) Gets the arguments as a vector of C-strings.
 *     This parameter can be NULL if you don't want the value.
 *  @param RemainingCmd (output) If there is more in the input to process,
 *     then this will point to the unknown command; otherwise it will be NULL
 *     or an empty string.
 *     This parameter can be NULL if you don't want the value.
 *  @return #CRANE_OK on success; #CRANE_CMD_EXIT if an "exit" command was found;
 *     #CRANE_CMD_HELP if a "help" command was found (its output will have already been done);
 *     #CRANE_STATUS_RETURN_CODE otherwise.
 */
#define CRANE_DoConsoleCommand(ClientHandle,Output,Cmd,ParsedCommand,ParsedArgc,ParsedArgs,RemainingCmd)\
			IPDR_DoConsoleCommand(ClientHandle,Output,Cmd,ParsedCommand,ParsedArgc,ParsedArgs,RemainingCmd)


/** For testing: set the data sequence number for a session.
 *  This must be called before CRANE_StartClient();
 *  @ingroup CRANE_sessions
 *  @param ClientHandle Handle to a transmitter (client) previously created by CRANE_InitClient().
 *  @param SessionHandle Handle to the session as returned by CRANE_CreateSession().
 *  @param DSN The next sequence number to use.
 *  @return #CRANE_OK on success; #CRANE_ERR_ALREADY_RUNNING if
 *      called after CRANE_StartClient() has been called.
 */
#define CRANE_SetSessionDSN(ClientHandle,SessionHandle,DSN)
#endif

#ifdef __cplusplus
}
#endif

#endif
