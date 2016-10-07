/** @file IPDRsim.cpp
 *
 * IPDRsim sample "simulation" code for the IPDR client (transmitter)<br>
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: IPDRsim.cpp,v 1.57 2005/05/11 12:58:16 dmitryk Exp $
 *
 * <code>   IPDRsim [-f] recordFile configFile [param=value] [ - command ]</code><br>
 * Expects parameters of the form:
 * @htmlonly
 * <table border=0>
 * <tr valign=top><td><code>-f</code></td> <td>fast (all fields get same value)</td></tr>
 * <tr valign=top><td colspan=2>(defaults to: single-threaded platform)</td></tr>
 * <tr valign=top><td><code><i>recordFile</i></code></td>
 *     <td>the file to write a copy of the records to; if
 *         "" then no output.</td></tr>
 * <tr valign=top><td><code><i>configFile</i></code></td>
 *     <td>the name of a file that was saved using the "mibsave" command
 *         and which is loaded as part of the initialization process.
 *         This may be "" or "/dev/null" to indicate that there is no configuration file.
 * <tr valign=top><td><code>[param=value] ...</code></td>
 *     <td>Zero or more specifications of parameters, which override
 *         the values given in this program. You can also set
 *         values by using the "mibset" command. The difference is
 *         that parameters on the command line are set up at
 *         client (transmitter) initialization time, whereas "mibset" commands
 *         are executed after the client (transmitter) is initialized (the "mibset"
 *         commands can therefore be used simulate the loading of
 *         a previously saved MIB.
 * </td></tr>
 * <tr valign=top><td><code>[command]</code></td>
 *     <td>A string of commands, separated by ";"s. Typically, these are
 *         used for "mibset" commands, but you can also do "add_session" and
 *         similar commands. These commands are executed before any commands
 *         given by standard input. Be careful to quote things appropriately
 *         for your shell, so that you don't lose quotes when passed through
 *         to the IPDR client's (transmitter's) internal command processor and so that you
 *         hide the ";"s from the shell.
 * </td></tr>
 * </table>
 * @endhtmlonly
 * <p>
 *  After the command line is processed, commands are read from standard input
 *  (the <i>command</i> from the command line is executed before the commands from
 *  standard input).
 * <p>
 * The program outputs its progress and results to <code>stderr</code>.
 * <p>It also outputs what the Text File DXM will produce to the file specified
 * by the <code><i>recordFile</i></code> parameter.
 *
 * Example:
 * <code>
 * <br>&nbsp;&nbsp;IPDRsim IPDRsim-`date -u +%Y-%m-%dT%H-%M-%SZ`.csvx
 * <br>&nbsp;&nbsp;&nbsp;&nbsp;configuration.vendorString='IPDRsim Transmitter'
 * <br>&nbsp;&nbsp;&nbsp;&nbsp;-&nbsp;'add_session 101 session1 "The session";'
 * <br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;'add_server 101 server-1 10.0.0.213 15001 1;'
 * <br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;'add_server 101 server-2 10.0.0.214 15001 2'
 * </code>
 *
 *
 * Commands are (following the "-" in the command line, then read from
 * from <code>stdin</code>):
 *
 * <table>
 * <tr><td>help</td>
 *     <td>Print a brief help message about the commands.</td></tr>
 * <tr><td>template <i>templateId</i></td>
 *     <td>Call IPDR_CreateTemplate() for the record with this ID.</td></tr>
 * <tr><td>dsn <i>sessionId</i> <i>number</i></td>
 *     <td>Call IPDR_SetSessionDSN(..., <i>number</i>) to set the data sequence
 *         number for the session.
 *         This must be done before the <code>start</code> command.</td></tr>
 * <tr><td>start</td>
 *     <td>Call IPDR_StartExporter().</td></tr>
 * <tr><td>send <i>templateId</i> <i>numberOfRecords</i> <i>intervalBetweenRecords</i></td>
 *     <td>Using IPDR_SendData() for this template, send the specified number of records,
 *         with the interval between sending (zero means "as fast as possible").</td></tr>
 * <tr><td>sleep <i>milliSeconds</i></td>
 *     <td>Sleep the specified number of milli-seconds.</td></tr>
 * <tr><td>statistics</td>
 *     <td>Display session and server statistics.</td></tr>
 * <tr><td>mib</td>
 *     <td>Display the content of the MIBs.</td></tr>
 * <tr><td>mibsave [ <i>file</i> ]</td>
 *     <td>Save the persistent parts of the MIB to a file.
 *         If the file is omitted or is "-", output is to stderr.</td></tr>
 * <tr><td>mibload <i>file</i></td>
 *         Load MIB data that hsave been saved by "mibload".
 * <tr><td>drain <i>milliSeconds</i></td>
 *     <td>IPDR_DrainQueue() with a maximum wait specified (zero means "forever").</td></tr>
 * <tr><td>stop</td>
 *     <td>IPDR_DeleteExporter().</td></tr>
 *
 * <tr><td>console commands</td></tr>
 * <tr><td>help</td>
 *      <td>Print out brief help information for the commands.</td></tr>
 * <tr><td>helpm <i>module</i></td>
 *      <td>See IPDR_DoConsoleCommand().</td></tr>
 * <tr><td>helpc <i>command</i></td>
 *      <td>See IPDR_DoConsoleCommand().</td></tr>
 * <tr><td>exit</td>
 *      <td>See IPDR_DoConsoleCommand().</td></tr>
 * <tr><td>stop</td>
 *      <td>See IPDR_DoConsoleCommand().</td></tr>
 * <tr><td>log commands</td></tr>
 * <tr><td>get_debug_level [ <i>module</i> ]</td>
 *      <td>See IPDR_DoConsoleCommand().</td></tr>
 * <tr><td>set_debug_level <i>module</i>|* <i>level</i></td>
 *      <td>See IPDR_DoConsoleCommand().</td></tr>
 * <tr><td>get_logger_addr</td>
 *      <td>See IPDR_DoConsoleCommand().</td></tr>
 * <tr><td>set_logger_addr <address> <i>port</i></td>
 *      <td>See IPDR_DoConsoleCommand().</td></tr>
 * <!-- client (transmitter) commands: -->
 * <tr><td>add_session <i>id</i> <i>name</i> <i>description</i></td>
 *      <td>See IPDR_DoConsoleCommand().</td></tr>
 * <tr><td>delete_session <id></td>
 *      <td>See IPDR_DoConsoleCommand().</td></tr>
 * <tr><td>add_server <i>session_id</i> <i>name</i> <i>ip</i> <i>port</i> <i>priority</i></td>
 *      <td>See IPDR_DoConsoleCommand().</td></tr>
 * <tr><td>delete_server <i>session_id</i> <i>ip</i> <i>port</i></td>
 *      <td>See IPDR_DoConsoleCommand().</td></tr>
 * <tr><td>get_sessions</td>
 *      <td>See IPDR_DoConsoleCommand().</td></tr>
 * <tr><td>apply_templates</td>
 *      <td>See IPDR_DoConsoleCommand().</td></tr>
 * <tr><td>get_templates [ <i>session_id</i> ]</td>
 *      <td>See IPDR_DoConsoleCommand().</td></tr>
 * <tr><td>version</td>
 *      <td>See IPDR_DoConsoleCommand().</td></tr>
 * <tr><td>time</td>
 *      <td>See IPDR_DoConsoleCommand().</td></tr>
 * <tr><td>set_time <i>epoch_seconds</i></td>
 *      <td>See IPDR_DoConsoleCommand().</td></tr>
 * <tr><td>parameters</td>
 *      <td>See IPDR_DoConsoleCommand().</td></tr>
 * <tr><td>mib [ <i>node</i> ]</td>
 *      <td>See IPDR_DoConsoleCommand().</td></tr>
 * <tr><td>mibset <i>node</i> <i>value</i></td>
 *      <td>See IPDR_DoConsoleCommand().</td></tr>
 * <tr><td>log [ <i>print</i> | <i>callback</i> ]</td>
 *      <td>See IPDR_DoConsoleCommand().</td></tr>
 * </table>
 */

const char IPDRsim_rcsid[] = "@(#) $Id: IPDRsim.cpp,v 1.57 2005/05/11 12:58:16 dmitryk Exp $ "
" Copyright (2005) © Amdocs, All Rights Reserved"
"Compiled " __DATE__ " " __TIME__;

#define IPDR_utilities // Get the extra utility functions
#include <IPDR/Defs.h>
#include <IPDR/IPDR.h>
#include <Platforms/Generic/GenericPlatform.h>

#include <unistd.h>
#include <stdio.h>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cstddef>
#include <cstring>
#include <string>
#include <map>
#include <vector>
#include <time.h>
using namespace std;

//  #include <sys/types.h>
//  #include <sys/socket.h>
//  #include <netinet/in.h>
#include <arpa/inet.h>

// Using old style includes where needed, for some compilers
#include <stddef.h>
#include <signal.h>
#include <setjmp.h>

static string nowStrX();
static struct timespec getNowTime();

/** The "bail-out" jump buffer (set by <code>setjmP</code>) and used by handle_SIGUSR1().
 */
static jmp_buf stopJmpBuf; // for cleanup from interrupt


/** Handle USR1 signals by bailing out.
 * @param signum The UNIX signal number
 * @see stopJmpBuf
 */
static void handle_SIGUSR1(int signum)
{
    longjmp(stopJmpBuf, 1);
}

#include "IPDRsimAux.cpp"
#include "IPDRsimTemplates.cpp"


/** A string for prepending to output lines (to stderr), so they can be found easily
 *  amongst other outputs.
 */
static const string outputPrefix("*** ");

/** A global flag for whether we're trying to fill the output records
 *  fast (all the same) or slowly (more variation in the fields).
 */

bool fastFillFlag = false; // set by "-f" (true for "-f")


// ***********************************

/** The value of a parameter.
 */
class ParmValue_t
{
public:
    const char* value; /**< The actual value (can be NULL for default). */
    bool  setByCLI; /**< True if set from the command line. */
    ParmValue_t(const char* theValue, bool theSetByCLI)
        : value(theValue), setByCLI(theSetByCLI) { }
    ParmValue_t() : value(NULL), setByCLI(false) { }
};


/** A mapping from parameter name to parameter value. */
typedef map < string, ParmValue_t > ParmMap_t;


/** The argument to clientConfig(), containing a pointer to the
 *  parameters map and a pointer to the client (transmitter) handle.
 */
typedef struct
{
    ParmMap_t *parmMap; /**< Pointer to the parameters mapping */
    IPDR_ExporterHandle *clientH; /**< Pointer to the client (transmitter) handle (see IPDR_InitExporter() for explanation of why it's a pointer. */
} ExporterConfigContext_t;


/** Set the values of the parameters.
 *  @param parmMap (updated) parameer map
 */
static void initParmMap(ParmMap_t& parmMap)
{
    // Except as noted, the following parameters are all the default values
    parmMap["configuration.vendorString"] =           /*str*/
        ParmValue_t("IPDR.org - IPDR Exporter (simulator)", false); // not default value
    parmMap["configuration.exporterAddress"] =          /*IPstr*/ ParmValue_t(NULL, false);
    parmMap["configuration.exporterPort"] =             /*int*/ ParmValue_t("4737", false);
    parmMap["configuration.protocolInfoAddr"] =       /*IPstr*/ ParmValue_t(NULL, false);
    parmMap["configuration.protocolInfoPort"] =       /*int*/ ParmValue_t("4737", false);
    parmMap["configuration.windowSize"] =             /*int*/ ParmValue_t(NULL, false); // default: "1000000"
    parmMap["configuration.windowTimeout"] =          /*int*/ ParmValue_t("10000", false); // default: "10000", false);
    parmMap["configuration.tmplDataTimeout"] =        /*int*/ ParmValue_t("5000", false);
    parmMap["configuration.statisticsInterval"] =     /*int*/ ParmValue_t("60000", false); // default: "60000"
    parmMap["configuration.queueChunkSize"] =         /*int*/ ParmValue_t("16384", false);
    parmMap["configuration.queueInitialNumChunks"] =  /*int*/ ParmValue_t("4", false);
    parmMap["configuration.queueMaxMemory"] =         /*int*/ ParmValue_t("0", false);

	parmMap["configuration.waitBestCollectorTimeout"] =    /*int*/ ParmValue_t("60000", false);

	parmMap["configuration.dataAckTimeInterval"] =         /*int*/ ParmValue_t("1000", false);
	parmMap["configuration.dataAckSequenceInterval"] =         /*int*/ ParmValue_t("50", false);

	parmMap["configuration.keepAliveInterval"] =         /*int*/ ParmValue_t("20000", false);
	parmMap["configuration.autoconnectInterval"] =         /*int*/ ParmValue_t("5000", false);

    parmMap["configuration.transport.tcpTransport.flushInterval"] =                ParmValue_t(NULL, false); // default: "500";
    parmMap["configuration.transport.tcpTransport.dataRcvChunkSize"] =             /*int*/ ParmValue_t("16384", false);
    parmMap["configuration.transport.tcpTransport.dataRcvInitialNumChunks"] =      /*int*/ ParmValue_t("2", false);
    parmMap["configuration.transport.tcpTransport.dataRcvMaxMemory"] =             /*int*/ ParmValue_t("0", false);
    parmMap["configuration.transport.tcpTransport.dataSndChunkSize"] =             /*int*/ ParmValue_t("4096", false);
    parmMap["configuration.transport.tcpTransport.dataSndInitialNumChunks"] =      /*int*/ ParmValue_t("2", false);
    parmMap["configuration.transport.tcpTransport.dataSndMaxMemory"] =             /*int*/ ParmValue_t("0", false);
    parmMap["configuration.transport.tcpTransport.sendBuffersChunkSize"] =         /*int*/ ParmValue_t("4096", false);
    parmMap["configuration.transport.tcpTransport.sendBuffersInitialNumChunks"] =  /*int*/ ParmValue_t("2", false);
    parmMap["configuration.transport.tcpTransport.sendBuffersMaxMemory"] =         /*int*/ ParmValue_t("0", false);

    parmMap["configuration.transport.tcpTransport.minimumReconnectInterval"] =     /*int*/ ParmValue_t("1000", false);
    parmMap["configuration.transport.tcpTransport.maximumReconnectInterval"] =     /*int*/ ParmValue_t("10000", false);
    parmMap["configuration.transport.tcpTransport.connectionTimeout"] =            /*int*/ ParmValue_t("-1", false);

    parmMap["configuration.console.consolePort"] =           /*int*/ ParmValue_t("40200", false);
    parmMap["configuration.console.consolePassword"] =       /*str*/ ParmValue_t(NULL, false);

    /* let the following default: */
    parmMap["configuration.logging"] =  ParmValue_t(NULL, false);
    parmMap["configuration.logging.loggerAddress"] =  ParmValue_t(NULL, false);
    parmMap["configuration.logging.loggerPort"] =  ParmValue_t(NULL, false);
}


/** Callback for lookup of parameters.
 *  This does a lookup into the parmMap (passed in as the context).
 *  It also checks that a MIB value hasn't already been provided --- this isn't
 *  necessary currrently but is a "good idea" in case things need to change
 *  in the future.
 * @param name The parameter's name to look up
 * @param contextArg The context for the callback, as given to IPDR_InitExporter(): pointer to a ::ExporterConfigContext_t.
 * @return The value as a C-string pointer (NULL if unknown parameter)
 * @see initParmMap
 * @see ParmMap_t
 */
extern "C"
const char *clientConfig(const char *name,
                         void *contextArg)
{
    const ExporterConfigContext_t& context = *static_cast<ExporterConfigContext_t*>(contextArg);
    ParmMap_t::const_iterator lookup = context.parmMap->find(name);

    if (lookup == context.parmMap->end())
	{
        // cerr << outputPrefix << "Unknown parameter(" << name << ")" << endl;
        return NULL; // will be NULL if not in MIB, hence default value
	}
	else
	{
        if (lookup->second.setByCLI)
		{
            return lookup->second.value;
        }
		else
		{
            return lookup->second.value;
        }
    }
}


/** Look in a vector of records for one with a specific ID.
 *  @param rec A vector of records, terminated by an entry with NULL.
 *  @param templateId the template ID to look up.
 *  @return pointer to the record if found; NULL if not found
 *  @see Record::templateId
 */
static Record* lookupRecByTemplateId(Record* rec[], int templateId)
{
    for (int i = 0; rec[i]; i++) {
        if (rec[i]->templateId() == templateId) {
            return rec[i];
        }
    }
    return NULL;
}


/** The callback for outputting log messages (registered by IPDR_SetLogOutput()).
 * This simply outputs a message to stderr.
 * @param level The numeric value for the error level of this message
 * @param levelStr A C-string which relates to the error level
 * @param clientName The client's (transmitter's) name (C-string), as given to IPDR_InitExporter()
 * @param module The module that generated this message "(Exporter" (transmitter), "Platform", "TCP", etc.)
 * @param func The function within the module that generated this message
 * @param msg The message for outputting to the log
 * @param context The context for this callback, as passed to IPDR_SetLogOutput(): pointer to a IPDR_ExporterHandle
 * @see IPDR_SetLogOutput
 */
extern "C"
void logOutputCallback(IPDR_DebugLevel level,
                       const char *levelStr,
                       const char *clientName,
                       const char *module,
                       const char *func,
                       const char *msg,
                       void *context)
{
    IPDR_ExporterHandle *clientH = static_cast<IPDR_ExporterHandle *>(context);
        char timeStrBuf[IPDR_TIME_FORMAT_LENGTH + 1];
    char line[500];

    sprintf(line, "%.30sZ %.7s <%.40s> {%-8s} %.99s: %.299s",
            IPDR_FormatTimeGMTNow(*clientH, timeStrBuf),
            levelStr, clientName, module, func, msg);
    cerr << line << endl;
}

/** Data used for calculating data transfer rates (set by
 *  sendRecords() and retrieved by eventCallback()). It's a bit crude:
 *  it only really works if you do the "send" command immediately
 *  follwed by "drain 0".
 */
struct {
    long numRecords;          // set by sendRecords()
    timespec startTime;       // set by sendRecords()
    timespec endSendingTime;  // set by sendRecords() when finished
    timespec queueEmptyTime;  // set by eventCallback
} sendRateData;


/** The callback for handling an event (registered by IPDR_SetExporterEventHandler()).
 *  This simply outputs a message to stderr.
 *  @param event The event and its details
 *  @param msg A message associated with the event
 *  @param context The context for this callback, as passed to IPDR_SetExporterEventHandler(): a pointer to IPDR_ExporterHandle
 *  @see IPDR_SetExporterEventHandler
 *  @see IPDR_ExporterHandle
 */
extern "C"
void eventCallback(const IPDR_EventData* event,
                   const char *msg,
                   void *context)
{
    IPDR_ExporterHandle client = *(IPDR_ExporterHandle *)context;
    if (IPDR_CheckLogLevel(client, "Host", IPDR_LEVEL_DEBUG)) {
        cerr << endl << outputPrefix << nowStrX() << " Event: [" << // event->EventID << ": " <<
            IPDR_EventMsg(event) << "] " << msg << endl << endl;
    }
    IPDR_LogWrite(client, "Host",
                   (event->EventID == IPDR_EVENT_RECORD_LOST ||
                    event->EventID == IPDR_EVENT_ERROR) ? IPDR_LEVEL_ERROR : IPDR_LEVEL_INFO,
                   "EventCallback", "*** Event[%.99s] %s", IPDR_EventMsg(event), msg);
    switch (event->EventID) {
    case IPDR_EVENT_QUEUE_EMPTY:
        sendRateData.queueEmptyTime = getNowTime();
        {
            long sec0 = sendRateData.queueEmptyTime.tv_sec - sendRateData.startTime.tv_sec;
            long nsec_adj0 = sendRateData.queueEmptyTime.tv_nsec - sendRateData.startTime.tv_nsec;

            double sec = sec0;
            long nsec_adj = nsec_adj0;

            while (nsec_adj < 0) {
                sec -= 1;
                nsec_adj += (1000 * 1000 * 1000);
            }
            sec += ((double)nsec_adj) / (1000 * 1000 * 1000);
            if (sec != 0.0) {
                double rate = ((double)sendRateData.numRecords) / sec;
                IPDR_LogWrite(client, "Host", IPDR_LEVEL_INFO, "EventCallback",
                               "+++ Time to transmit %ld records: %g sec: %7g rec/sec",
                               sendRateData.numRecords,
                               sec, //  "(%ld.%10ld)"sec0, nsec_adj0,
                               rate, rate);
            }
        }
        break;
    default:
        break;
    }
}

/** The callback for handling a template start negotiation always enable negotiation
	@param SessionHandle
	@param CollectorHandle
	@param Negotiate
	@param Arg
	@see IPDR_SetStartNegotiationCallback
*/
void
enableStartNegotiationCB(	IPDR_SessionHandle SessionHandle,
					IPDR_CollectorHandle CollectorHandle,
					IPDR_BOOL *Negotiate,
					void *Arg)
{
	*Negotiate = IPDR_TRUE ;
}

/** The callback for handling a template start negotiation always disable negotiation
	@param SessionHandle
	@param CollectorHandle
	@param Negotiate
	@param Arg
	@see IPDR_SetStartNegotiationCallback
*/
void
disableStartNegotiationCB(	IPDR_SessionHandle SessionHandle,
					IPDR_CollectorHandle CollectorHandle,
					IPDR_BOOL *Negotiate,
					void *Arg)
{
	*Negotiate = IPDR_FALSE ;
}


/** The callback for handling a template changed (registered by IPDR_SetTemplateChangedCallback()).
 *  This simply outputs a message to stderr and continues to send the same information.
 *  @param Session
 *  @param templates Vector of changed templates.
 *  @param numTemplates The number of templates that were changed.
 *  @param context (unused) The context for this callback, as passed to IPDR_SetTemplateChangedCallback(): a pointer to #IPDR_ExporterHandle.
 *  @see IPDR_SetTemplateChangedCallback
 *  @see IPDR_ExporterHandle
 */
extern "C"
void templatesChangedCB(IPDR_SessionHandle Session,
						IPDR_Template *templates,
						int numTemplates,
                        void *context)
{
    bool SwitchStatus = true;
    Record** rec = static_cast<Record**>(context);
    cerr << outputPrefix << "Entered templates changed callback" << endl;
    for (int i = 0; i < numTemplates; i++) {
        const IPDR_Template &tmpl = templates[i];
        Record* theRec = lookupRecByTemplateId(rec, tmpl.TemplateID);
        cerr << "  Template ID = " << tmpl.TemplateID <<
            ", Schema Name = " << tmpl.SchemaName <<
			", Type Name = " << tmpl.TypeName << endl;
		for (int  j = 0; j < tmpl.NumKeys; j++)
		{
			IPDR_Key &ky = tmpl.Keys[j];
			for (int k = 0; k < theRec->numKeys(); k++)
			{
				if (ky.KeyID == theRec->keys()[k].KeyID)
				{
					if(ky.Enabled != theRec->keys()[k].Enabled)
					{
						if(SwitchStatus)
						{
							ky.Enabled = !ky.Enabled;
							cerr << "     changed to " <<
										((ky.Enabled) ? "disabled" : "enabled ") <<
										" Key ID = " <<  ky.KeyID <<
										", KeyName = " << ky.KeyName <<
										", KeyLabel = " << ky.KeyLabel <<
										", KeyHelp = " << ky.KeyHelp <<
										endl;
							SwitchStatus = false;
						}
						else
						{
							SwitchStatus = true ;
						}
						break;
					}
				}
			}
        }
    }
}


/** Generate a C-string for the current time in ISO standard form.
 *  @param buf The buffer that will be filled with the date/time string (null-terminated)
 *  @param bufSize The buffer size
 */
static void nowStr(char* buf, size_t bufSize)
{
    time_t c = time(0);
    struct tm tmMem;
    struct tm* tmPtr = localtime_r(&c, &tmMem);
    strftime(buf, bufSize, "%Y-%m-%dT%H:%M:%S%Z", tmPtr);
}


/** Generate a string with the current time in ISO standard form.  Uses
 *  an internal static value, so multiple calls to this function must
 *  <em>not</em> be done in a single statement.
 *  @return The ISO standard form string with the current date/time
 *  @see nowStr
 */
static string nowStrX()
{
    static char buf[100];
    nowStr(buf, sizeof buf);
    return string(buf);
}


/** Get the current time.
 *  @return The current time as a Unix-standard "timespec"
 */
static struct timespec getNowTime()
{
    struct timespec result;
#if defined(CYGWIN)
    struct timeval tv;
    gettimeofday(&tv, NULL);
    result.tv_sec  = tv.tv_sec;
    result.tv_nsec = tv.tv_usec * 1000;
#else
    clock_gettime(CLOCK_REALTIME, &result);
#endif
    return result;
}


/** Send a batch of records, at a specified rate.
 *  @param record (updated) The record, which supports a "fill" method for putting data into it.
 *  @param recNum (updated) The record number
 *  @param clientH The client handle as set by IPDR_InitExporter()
 *  @param sessionH The session handle
 *  @param times The number of records to send (number of times to call IPDR_SendData())
 *  @param intvl The interval between sends, in milliseconds: 0 means "as fast as possible"
 *  @param ostr Pointer to output stream for writing a CSV-form of the
 *        records that were sent via IPDR; can be NULL to prevent output.
 */
static void sendRecords(Record& record,
                        unsigned long& recNum,
                        IPDR_ExporterHandle clientH,
						IPDR_SessionHandle sessionH,
						IPDR_TemplateHandle templateH,
                        unsigned int times,
                        unsigned int intvl,
                        ostream *ostr)
{
    timespec nextSend;
    unsigned int timesOrig = times;
    char nowStrValue[40];

    static int thousand = 1000; // for millisec
    static int million = thousand * thousand; // for microsecond or milliseconds in nanoseconds
    static int billion = thousand * million; // for nanoseconds

    nowStr(nowStrValue, sizeof nowStrValue);

    cerr << outputPrefix << "+++ Sending " << times << " records interval=" << intvl <<
        " at " << nowStrValue << " ..." << endl;

    sendRateData.numRecords = times;
    sendRateData.startTime = getNowTime();
    nextSend = sendRateData.startTime;

    for (; times > 0; times--) {
        timespec now = getNowTime();

        // calculate how many milliseconds to sleep.

        if (intvl > 0) {
            timespec sleepTime;
            sleepTime.tv_sec  = nextSend.tv_sec  - now.tv_sec;
            sleepTime.tv_nsec = nextSend.tv_nsec - now.tv_nsec;
            while (sleepTime.tv_nsec < 0)        { sleepTime.tv_nsec += billion; sleepTime.tv_sec -= 1; }
            while (sleepTime.tv_nsec >= billion) { sleepTime.tv_nsec -= billion; sleepTime.tv_sec += 1; }

            int sleepDuration = sleepTime.tv_sec * thousand + sleepTime.tv_nsec / million;

            nextSend.tv_sec  += (intvl / thousand);
            nextSend.tv_nsec += ((intvl % thousand) * million);
            while (nextSend.tv_nsec < 0)        { nextSend.tv_nsec += billion; nextSend.tv_sec -= 1; }
            while (nextSend.tv_nsec >= billion) { nextSend.tv_nsec -= billion; nextSend.tv_sec += 1; }

            IPDR_SleepMS(clientH, sleepDuration);
            nowStr(nowStrValue, sizeof nowStrValue);
        }

        record.fill(nowStrValue, 30 - (recNum % 30), recNum, now);
        recNum++;

        IPDR_Status rc = IPDR_SendData(clientH, sessionH, templateH, &record);
        if (FAILURE(rc)) {
            cerr << outputPrefix << "SendData failed: " << IPDR_StatusMsg(clientH, rc) << endl;
            cerr << "    (Stopping with " << times << " iterations left." << endl;
            break;
        }
        if (ostr) {
            record.printRec(*ostr) << endl;
        }
    }

    sendRateData.endSendingTime = getNowTime();

    if (times != 0) {
        cerr << outputPrefix << "finished sending " << (timesOrig - times) <<
            " records (didn't send " << times << " of " << timesOrig << ")";
        sendRateData.numRecords -= times;
    } else {
        cerr << outputPrefix << "finished sending " << timesOrig << " records";
    }
    cerr << " records intvl=" << intvl << " at " << nowStrX() << endl;
}

/** Execute the "template" command.
 *  @param sessionID session id to assign the template
 *  @param templateId The identifier for the template.
 *  @param exporterH Exporter handle from IPDR_InitExporter().
 *  @param rec Vector of records that can be output.
 */

static
void cmdTemplate(int sessionID,
				 int templateId,
                 IPDR_ExporterHandle exporterH,
                 Record* rec[])
{
	IPDR_SessionHandle sessionH	;
	const char* sessionName ;

	if( FAILURE(IPDR_GetSession(exporterH,sessionID, &sessionH, &sessionName, NULL)))
	{
		cerr << "Invalid session ID: " << sessionID << endl ;
		return ;
	}

    Record* theRec = lookupRecByTemplateId(rec, templateId);
    if (rec && theRec) {
        IPDR_Status rc;
        cerr << outputPrefix << "Creating Template #" << theRec->templateId() <<
            " (" << theRec->templateDescr() << ")" <<
            " for Session #" << sessionID << " (" << sessionName << ")"	<< endl;
        theRec->printKeys(cerr) << endl;

		if(IPDR_IsRunning(exporterH))
			IPDR_StopSession(sessionH, IPDR_STOP_REASON_TEMPLATEUPDATED, 60*1000);

        rc = IPDR_CreateTemplate(exporterH,
								  sessionH,
                                  theRec->templateId(),
                                  theRec->templateDescr(),
				  				  "",
                                  theRec->numKeys(),
                                  theRec->keys(),
                                  & theRec->templateH);
        if (FAILURE(rc))
		{
            cerr << outputPrefix << "IPDR_CreateTemplate failed: " << IPDR_StatusMsg(exporterH, rc) << endl;
            //exit(1);
        }
		else
		{
			if(IPDR_IsRunning(exporterH))
				IPDR_ApplyTemplates(sessionH);
		}

		if(IPDR_IsRunning(exporterH))
			IPDR_StartSession (sessionH,1);

    } else {
        cerr << outputPrefix << "Invalid templateId: " << templateId << endl;
    }
}

/** Execute the "dsn" command.
 *  @param sessionId the session ID
 *  @param dsn The new data sequence number.
 *  @param clientH Exporter handle from IPDR_InitExporter().
 */

static
void cmdSetSessionDSN(unsigned char sessionId,
                      IPDR_DSN_TYPE dsn,
                      IPDR_ExporterHandle clientH)
{
#ifdef DEPRECATED
    IPDR_SessionHandle sessionH;
    const char* sessionName;
    const char* sessionDescr;
    IPDR_Status rc;

    rc = IPDR_GetSession(clientH, sessionId, &sessionH, &sessionName, &sessionDescr);
    if (FAILURE(rc)) {
        cerr << outputPrefix << "setSessionDSN: GetSession(..., " <<
            (unsigned int)sessionId << ", ...) failed: " <<
            IPDR_StatusMsg(clientH, rc) << endl;
        //exit(1);
    }
    rc = IPDR_SetSessionDSN(clientH, sessionH, dsn);
    if (FAILURE(rc)) {
        cerr << outputPrefix << "setSessionDSN(" << dsn <<
            " failed for session(" << sessionName << ") [" <<
            sessionDescr << "]: " << IPDR_StatusMsg(clientH, rc) << endl;
        //exit(1);
    }
    cerr << outputPrefix << "setSessionDSN(" << dsn <<
            ") for session(" << sessionName << ") [" <<
            sessionDescr << "]" << endl;
#endif

}

/** Output statistics.
 *  @param clientH Exporter handle from IPDR_InitExporter().
 *  @param sessionH Session handle from IPDR_CreateSession().
 */

static
void cmdStatistics(IPDR_ExporterHandle clientH,
                   IPDR_SessionHandle sessionH)
{
    IPDR_Statistics stats;
    unsigned long outstanding, unACKed;
    IPDR_Status rc =
        IPDR_GetSessionStatistics(clientH, sessionH, NULL, &stats, &outstanding, &unACKed);
    if (SUCCESS(rc)) {
        cerr << outputPrefix << nowStrX() << " session: " <<
            stats.TransmittedRecords  << " TX, " <<
            stats.AcknowledgedRecords << " ACK, " <<
            stats.EnqueuedRecords     << " enq, " <<
            stats.LostRecords         << " lost, " <<
            outstanding               << " outstanding, " <<
            unACKed                   << " unAcKed" << endl;
    } else {
        cerr << outputPrefix << "IPDR_GetSessionStatistics failed: " << IPDR_StatusMsg(clientH, rc) << endl;
    }
	unsigned int NumCollectors;
	IPDR_Collector *Collectors;
    rc = IPDR_GetCollectorInSessionList(clientH, sessionH, &NumCollectors, &Collectors);
    if (FAILURE(rc)) {
        cerr << outputPrefix << "IPDR_GetServersList failed: " << IPDR_StatusMsg(clientH, rc) << endl;
        NumCollectors = 0; // so the following loop won't be executed
    }
    for (unsigned int i = 0; i < NumCollectors; i++) {
        IPDR_EventData lastEvent;
        rc = IPDR_GetServerStatisticsAndState(clientH, sessionH, Collectors[i].Addr, Collectors[i].Port,
                                               NULL, &stats, &lastEvent);
        if (SUCCESS(rc)) {
            cerr << outputPrefix <<
                IPandPort::asString(Collectors[i].Addr, Collectors[i].Port) <<
                " (" << IPDR_EventMsg(&lastEvent) << "): " <<
                stats.TransmittedRecords  << " TX, " <<
                stats.AcknowledgedRecords << " ACK, " <<
                stats.EnqueuedRecords     << " enq, " <<
                stats.LostRecords        << " lost" << endl;
        } else {
            cerr << outputPrefix << "IPDR_GetServersStatistics(" <<
                IPandPort::asString(Collectors[i].Addr, Collectors[i].Port) <<
                ") failed: " << IPDR_StatusMsg(clientH, rc) << endl;
        }
    }
}


/** Output usage information to stderr.
 *  @param name A C-string with the program's name (<code>argv[0]</code> to main()).
 *  @return A non-zero value, so that you can write "<code>return usage(...)</code>"
 */
int usage(const char* name)
{
    cerr << "Usage: " << name <<
        " [-f] recordFile configFile [param=value] ... [ - command ]" << endl;
    return 1;
}

/** Get a vector of sessions.
 */

struct SessionInfo
{
    IPDR_SessionHandle handle;
    unsigned int id;
    const char* name;
    const char* descr;
};

vector<SessionInfo> getSessions(IPDR_ExporterHandle clientH)
{
    vector<SessionInfo> result;
    int numSessions;
    unsigned char *sessions;

    IPDR_Status rc = IPDR_GetSessionsList(clientH, &numSessions, &sessions);
    if (SUCCESS(rc)) {
        for (int i = 0; i < numSessions; i++) {
            SessionInfo si;
            si.id = sessions[i];
            rc = IPDR_GetSession(clientH, sessions[i], &si.handle, &si.name, &si.descr);
            if (SUCCESS(rc)) {
                result.push_back(si);
            } else {
                cerr << outputPrefix << "IPDR_GetSession(" << static_cast<unsigned int>(sessions[i]) << "): " << IPDR_StatusMsg(clientH, rc) << endl;
            }
        }
    } else {
        cerr << outputPrefix << "IPDR_GetSessionsList: " << IPDR_StatusMsg(clientH, rc) << endl;
    }

    return result;
}

/** Callback for saving the MIB.
 *  @param clientH Exporter handle.
 *  @param MIBVersion The MIB's version.
 *  @param contextArg The context argument as provided to
 *     IPDR_InitiExporter(): name of file to write to -- if "" or NULL,
 *         nothing is done.
 */
extern "C"
IPDR_Status saveMIBcallback(IPDR_ExporterHandle clientH, int MIBVersion, void* contextArg)
{
    return IPDR_OK;
}


/** Load the MIB from a file.
 *  @param clientH Exporter handle.
 *  @param MIBVersion The MIB version that we're using, for verifying a match
 *          or doing necessary conversions.
 *  @param fileName Name of file to read from: empty or "" for do-nothing.
 */
static
IPDR_Status loadMIB(IPDR_ExporterHandle clientH, int MIBVersion, const char* fileName)
{
    if (fileName && 0 != strcmp(fileName, "")) {
        ifstream in(fileName);
        if (! in) {
            cerr << "*** loadMIB(" << fileName << "): could not open" << endl;
            return IPDR_ERR;
        }

        {
            string versionLine;
            getline(in, versionLine);
            // We should check that MIBversion matches ...
        }

        while (! in.eof()) {
            string line;
            getline(in, line);
            if (! line.empty()) {
                // The line is: OID name type value
                // separated by tabs
                string::size_type t1pos = line.find('\t', 0);
                string::size_type t2pos = line.find('\t', t1pos+1);
                string::size_type t3pos = line.find('\t', t2pos+1);

                string oidStr = line.substr(0, t1pos);
                string name = line.substr(t1pos+1, t2pos-t1pos-1);
                string type = line.substr(t2pos+1, t3pos-t2pos-1);
                string value = line.substr(t3pos+1);

                // We could use either oidStr or name: we use name, in
                // case OID numbers have changed.

                /*IPDR_Status rc = IPDR_MIBRestoreNodeByName(clientH, name.c_str(),
                                                             atoi(type.c_str()),
                                                             value.c_str(), NULL);
                if (FAILURE(rc)) {
                    cerr << "FAILED load: " <<
                        " OID(" << oidStr << ")" <<
                        " name(" << name << ")" <<
                        " type(" << type << ")" <<
                        " value(" << value << "): " <<
                        IPDR_StatusMsg(clientH, rc) << endl;
                    return rc;
                }*/
            }
        }
    }
    return IPDR_OK;
}


/** Callback for loading the MIB during initialization.
 *  @param clientH Exporter handle.
 *  @param MIBVersion The MIB's version.
 *  @param contextArg The context argument as provided to
 *     IPDR_InitiExporter(): name of file to read from: empty or "" for do-nothing.
 */
extern "C"
IPDR_Status loadMIBcallback(IPDR_ExporterHandle clientH, int MIBVersion, void* contextArg)
{
    const char* fileName = reinterpret_cast<const char*>(contextArg);
    return loadMIB(clientH, MIBVersion, fileName);
}


/** Execute a command.
 *  @param clientH client Handle.
 *  @param rec Vector of Records.
 *  @param recNum Current record number (initially 0).
 *  @param recordFile File which gets CVS form of the records sent by IPDR.
 *  @param cmd The command line.
 *  @param end (output) set to true if the command was one that stops execution;
 *         otherwise set to false.
 */

void executeCommand(IPDR_ExporterHandle clientH,
                    Record* rec[],
                    unsigned long& recNum,
                    ofstream *recordFile,
                    const string& cmd,
                    bool *end)
{
    *end = false;
    IPDR_Status rc;

    if (! cmd.empty()) {
        const char *cmdPart;
        int cmdArgC;
        const char **cmdArgs;
		// The following line is bad style, but it's more convenient
        // than allocating a C-string, just so it can be modified.
        char *line = const_cast<char*>(cmd.c_str());
        while (line && line[0] != '\0') {
			IPDR_Status cmdRc = IPDR_DoConsoleCommand(clientH, stderr,
                                                        line,
                                                        &cmdPart, &cmdArgC, &cmdArgs,
                                                        &line,
														NULL,
														0);
			string cmdStr = cmdPart;
            switch (cmdRc) {
            case IPDR_CMD_EXIT:
                *end = true;
				//IPDR_SleepMS(clientH, 10*1000);
                return;
            case IPDR_CMD_HELP:
                cerr << "Testing commands:" << endl <<
                    "  template <sessionID> <templateId>" << endl <<
                    "  dsn <sessionId> <dsn>" << endl <<
                    "  start" << endl <<
                    "  send <templateId> <numberOfRecords> <intervalBetweenRecords>" << endl <<
                    "  sleep <milliSeconds>" << endl <<
                    "  statistics" << endl <<
                    "  drain <milliSeconds>" << endl <<
                    "  mibsave [ <filename> ]" << endl <<
                    "  mibload <filename>" << endl;
                break;
            case IPDR_OK:
                break;
            case IPDR_ERR_SESSION_ALREADY_EXISTS:
            case IPDR_ERR_COLLECTOR_ALREADY_EXISTS:
            case IPDR_ERR_ALREADY_RUNNING:
                /* No harm done: just ignore the command and keep going. */
                break;
            /* Errors that might be OK but probably aren't: */
            case IPDR_ERR_KEY_ALREADY_EXISTS:
            case IPDR_ERR_TEMPLATE_ALREADY_EXISTS:
            default:
                cerr << outputPrefix << "Command (" << cmd <<
                    ") error: " << IPDR_StatusMsg(clientH, cmdRc) << endl;
                break;
            case IPDR_CMD_NOT_FOUND:
                if (cmdStr == "template") {
                    if (cmdArgC == 2) {
                        cmdTemplate(atoi(cmdArgs[0]), atoi(cmdArgs[1]), clientH, rec);
                    } else {
                        cerr << "'template' command must have 2 arguments (sessionID, templateID)" << endl;
                    }
                } else if (cmdStr == "statistics") {
                    vector<SessionInfo> sessions = getSessions(clientH);
                    if (sessions.size() <= 0) {
                        cerr << outputPrefix << "No sessions for statistics." << endl;
                    }
                    for (unsigned int i = 0; i < sessions.size(); i++) {
                        cerr << outputPrefix << "Statistics for session " << sessions[i].id <<
                            " (" << sessions[i].name << ": " << sessions[i].descr << ")" << endl;
                        cmdStatistics(clientH, sessions[i].handle);
                    }
                } else if (cmdStr == "dsn" || cmdStr == "DSN") {
                    if (cmdArgC == 2) {
                        cmdSetSessionDSN(atoi(cmdArgs[0]), strtoul(cmdArgs[1], NULL, 10), clientH);
                    } else {
                        cerr << outputPrefix << "'dsn' command must have 2 argument (session Id, new sequence number)" << endl;
                    }
                } else if (cmdStr == "start") {
                    if (cmdArgC == 0) {
                        cerr << outputPrefix << "Starting Exporter ..." << endl;

                        rc = IPDR_StartExporter(clientH);
                        if (FAILURE(rc)) {
                            cerr << outputPrefix << "IPDR_StartExporter failed: " << IPDR_StatusMsg(clientH, rc) << endl;
                            //exit(1);
                        }
                    } else {
                        cerr << "'start' command doesn't take an argument" << endl;
                    }
                } else if (cmdStr == "send") {
                    if (cmdArgC == 4) {
						int sessionId = atoi(cmdArgs[0]);
                        int templateId = atoi(cmdArgs[1]);
                        long times = atoi(cmdArgs[2]);
                        long intvl = atoi(cmdArgs[3]);

						IPDR_SessionHandle sessionH;

						if( FAILURE(IPDR_GetSession(clientH,sessionId, &sessionH, NULL, NULL)))
						{
							cerr << "Invalid session ID: " << sessionId << endl ;
						}
						else
						{
							IPDR_TemplateHandle templateH;
							if(SUCCESS(IPDR_GetTemplate(sessionH,templateId,NULL,NULL,NULL,&templateH)))
							{
								Record* theRec = lookupRecByTemplateId(rec, templateId);
								if (theRec) {
									sendRecords(*theRec, recNum, clientH, sessionH, templateH, times, intvl, recordFile);
								} else {
									cerr << outputPrefix << "Invalid templateId: " << templateId << endl;
								}
							}
							else
							{
								cerr << outputPrefix << "Invalid templateId: " << templateId << ", For Session: " << sessionId << endl;
							}
						}
                    } else {
                        cerr << "'send' command must have 4 arguments (sessionID, templateID, times, interval)" << endl;
                    }
                } /*else if (cmdStr == "mibsave") {
                    switch (cmdArgC) {
                    case 0:
                        saveMIB(clientH, IPDR_MIBVersion(clientH), NULL);
                        break;
                    case 1:
                        saveMIB(clientH, IPDR_MIBVersion(clientH), cmdArgs[0]);
                        break;
                    default:
                        cerr << outputPrefix << "'mibsave' command must have 0 or 1 argument (file name)" << endl;
                    }
                } else if (cmdStr == "mibload") {
                    switch (cmdArgC) {
                    case 1:
                        loadMIB(clientH, IPDR_MIBVersion(clientH), cmdArgs[0]);
                        break;
                    default:
                        cerr << outputPrefix << "'mibload' command must have 0 or 1 argument (file name)" << endl;
                    }
                }*/ else if (cmdStr == "sleep") {
                    if (cmdArgC == 1) {
                        unsigned int millisec = atoi(cmdArgs[0]);
                        cerr << outputPrefix << "Sleeping " << ((double)millisec)/1000.0 << " seconds ..." << endl;
                        IPDR_SleepMS(clientH, millisec);
                        cerr << outputPrefix << "Awake! " << nowStrX() << endl;
                    } else {
                        cerr << "sleep' command must have 1 argument (millisec)" << endl;
                    }
                } else if (cmdStr == "drain") {
                    if (cmdArgC == 1) {
                        unsigned int millisec = atoi(cmdArgs[0]);
                        vector<SessionInfo> sessions = getSessions(clientH);
                        if (sessions.size() <= 0) {
                            cerr << outputPrefix << "No sessions to drain." << endl;
                        }
                        for (unsigned int i = 0; i < sessions.size(); i++) {
                            cerr << outputPrefix << "Draining session " << sessions[i].id <<
                                " (" << sessions[i].name << ": " << sessions[i].descr << ")";
                            if (millisec > 0) {
                                cerr << " for " << millisec << " milli-seconds ..." << endl;
                            } else {
                                cerr << " (indefinite wait)" << endl;
                            }
                            rc = IPDR_DrainQueue(clientH, sessions[i].handle, millisec);
                            if (SUCCESS(rc)) {
                                cerr << outputPrefix << "Finished draining at " << nowStrX() << endl;
                            } else {
                                cerr << outputPrefix << "IPDR_DrainQueue at " << nowStrX() << ": " << IPDR_StatusMsg(clientH, rc) << endl;
                            }
                        }
                    } else {
                        cerr << "'drain' command must have 1 argument (millisec: 0 for \"indefinite\")" << endl;
                    }
                } else {
                    cerr << "Command '" << cmdStr <<
                        "' with " << cmdArgC << " arguments (";
                    for (int i = 0; i < cmdArgC; i++) {
                        cerr << ((i == 0) ? "" : " ") << cmdArgs[i];
                    }
                    cerr << ") not found ... 'help' will give you a list." << endl;
                    break;
                }
            }
        }
    }
}


/** The main program.
 * @param argc The number of arguments + 1 for the program name
 * @param argv The arguments (<code>argv[0]</code> is the program name)
 * @return Zero for success; non-zero for error.
 */
int main(int argc, const char * const argv[])
{
    XyzFwdRecord XyzFwdRec;
    XyzAckRecord XyzAckRec;
    SimpleRecord simpleRec;
	DOCSISRecord docsisRec;
	VoIPRecord	voipRecord;
	int newArgc = argc;
	char** newArgv = const_cast<char**>(argv);
	char* newArgvStrings[50];
	int newArgvCount = 0;

    Record* rec[] = { &XyzFwdRec, &XyzAckRec, &simpleRec, &docsisRec, &voipRecord, 0 };
    IPDR_Status rc;

    int argvI = 1;
    ParmMap_t parmMap; // The parameter values go here.

    initParmMap(parmMap);

    if (argvI < newArgc && newArgv[argvI][0] == '-') {
        if (0 == strcmp(newArgv[argvI], "-f")) { fastFillFlag = true; }
        /*else { return usage(newArgv[0]); }
        argvI++;*/
    }

	if(newArgc == 1)
	{
		newArgvCount = 1;
		newArgvStrings[0] = (char*)malloc(strlen(newArgv[0])+1);
		strcpy(newArgvStrings[0], newArgv[0]);
		string cfn = newArgv[0] + string(".conf");
		FILE* f = fopen(cfn.c_str(), "r");
		if(f != NULL)
		{
			char buff[1024];
			for(newArgvCount = 1 ; newArgvCount < 50 && fgets(buff, 1024, f) != NULL ; newArgvCount++)
			{
				newArgvStrings[newArgvCount] = (char*)malloc(strlen(buff));
				buff[strlen(buff)-1] = '\0';
				strcpy(newArgvStrings[newArgvCount], buff);
				memset(buff, 0, 1024);
			}
			fclose(f);
			if(newArgvCount > 1)
			{
				newArgc = newArgvCount;
				newArgv = newArgvStrings;
			}
		}
	}

    const string recordFileName = /*(argvI < newArgc) ? newArgv[argvI++] :*/ "";
    const string configFileName = /*(argvI < newArgc) ? newArgv[argvI++] :*/ "";

    for (; argvI < newArgc; argvI++) { // get the over-riding parameters
        const char* a = newArgv[argvI];
		if (a[0] == '-') {
            if (a[1] == '\0') {
                argvI++;
                break;
            }
			else {
                cerr << "Invalid option: \"" << a << "\"" << endl;
            }
        } else {
            const char* eqPtr = strchr(a, '=');
            if (eqPtr) {
                string key(a, eqPtr - a);
                cerr << "  parameter(" << key << ") = \"" << &eqPtr[1] << "\"" << endl;
                // Depend on the args having been allocated already and
                // not going away ... if of form name=value, then eqPtr
                // points to "value", which is nicely null-terminated.
                parmMap[key] = ParmValue_t(&eqPtr[1], true);
            } else {
                cerr << "Invalid override argument: \"" << a << "\"" << endl;
                return usage(newArgv[0]);
            }
        }
    }

    string cliCmdStr = "";

    for (; argvI < newArgc; argvI++) { // get the command
        if (cliCmdStr.empty()) {
            cliCmdStr = newArgv[argvI];
        } else {
            cliCmdStr += string(" ") + newArgv[argvI];
        }
    }

    ofstream *recordFile;
    if (recordFileName.empty() || recordFileName.length() == 0) {
        recordFile = NULL;
    } else {
        recordFile = new ofstream(recordFileName.c_str());
        if (! recordFile) {
            cerr << "Cannot open file \"" << recordFileName << "\" for output" << endl;
            return 1;
        }
    }

    cerr << outputPrefix << "Platform Initialization (output to \"" << recordFileName << "\")..." <<
        (fastFillFlag ? " [fast fill]" : "") <<
        endl;

    // intialize the platform services structure

    IPDR_PlatformServices services;

    cerr << outputPrefix << "Single-threaded (non-blocking I/O)" << endl;
    rc = GenericPlatform_Initialize(&services);
    if (FAILURE(rc)) {
        cerr << outputPrefix << "Platform initialization failed: " << IPDR_StatusMsg(NULL, rc) << endl;
        //exit(1);
    }

	rc = IPDR_MIB_Start(&services);
	if (FAILURE(rc)) {
        cerr << outputPrefix << "MIB initialization failed: " << IPDR_StatusMsg(NULL, rc) << endl;
        //exit(1);
    }

    // end of platform services initialization

    // initialize the IPDR client

    IPDR_ExporterHandle clientH = 0;

    ExporterConfigContext_t clientConfigContext = { &parmMap, &clientH };

    rc = IPDR_InitExporter(&services,
                          "*", // "IPDRsim",
                          clientConfig,
                          &clientConfigContext,
                          loadMIBcallback,
                          const_cast<char*>(configFileName.c_str()),
                          saveMIBcallback,
                          const_cast<char*>(configFileName.c_str()),
                          &clientH);
    if (FAILURE(rc)) {
        cerr << outputPrefix << "IPDR_InitExporter failed: " << IPDR_StatusMsg(clientH, rc) << endl;
        //exit(1);
    }

	//IPDR_MIB_SetExporterHandle(clientH);

    rc = IPDR_SetLogOutputCallbackDetailed(clientH,
                                            logOutputCallback,
                                            &clientH);
    if (FAILURE(rc)) {
        cerr << outputPrefix << "IPDR_SetLogOuptutCallback2 failed: " << IPDR_StatusMsg(clientH, rc) << endl;
        //exit(1);
    }

    // The following logging values would be saved as part of
    // "mibsave", so if "mibload" is done, it will override any values
    // set here.

    /*IPDR_SetModuleMoreDebugLevel(clientH, "Exporter",   IPDR_LEVEL_INFO);
    IPDR_SetModuleMoreDebugLevel(clientH, "Platform", IPDR_LEVEL_INFO);
    IPDR_SetModuleMoreDebugLevel(clientH, "TCP",      IPDR_LEVEL_INFO);
    IPDR_SetModuleMoreDebugLevel(clientH, "Host",     IPDR_LEVEL_INFO);
    IPDR_SetModuleMoreDebugLevel(clientH, "Queue",    IPDR_LEVEL_INFO);
    IPDR_SetModuleMoreDebugLevel(clientH, "Memory",   IPDR_LEVEL_INFO);
    IPDR_SetModuleMoreDebugLevel(clientH, "Protocol", IPDR_LEVEL_INFO); // IPDR_LEVEL_WARNING); // INFO shows things like template change messages.
    IPDR_SetModuleMoreDebugLevel(clientH, "JNI",      IPDR_LEVEL_MUTE); // only on server
    IPDR_SetModuleMoreDebugLevel(clientH, "SERVER",   IPDR_LEVEL_MUTE); // only on server*/

    IPDR_SetTemplateChangedCallback(clientH, templatesChangedCB, rec);
	IPDR_SetStartNegotiationCallback(clientH, enableStartNegotiationCB, &clientH);
    IPDR_SetExporterEventHandlerDetailed(clientH, eventCallback, &clientH);

    if (setjmp(stopJmpBuf) == 0) {
        // normal execution
        if (signal(SIGUSR1, handle_SIGUSR1) == SIG_IGN) {
            cerr << outputPrefix << "SIGUSR1 was masked off ... leave it" << endl;
            signal(SIGUSR1, SIG_IGN); // leave alone if already ignored
        }

        // We could create one or more sessions by the following, but instead we
        // depend on commands ("add_session") to do this for us:
        //   IPDR_SessionHandle sessionH;
        //   rc = IPDR_CreateSession(clientH, "session1", sessionID, "NE/one", &sessionH);

        // We could add servers (collectors) to each session by the following, but
        // instead we depend on commands ("add_server") to do this for
        // us:
        //   rc = IPDR_AddCollector(clientH, serverName, serverIPnum, serverPort, numOfConnections);

        // Normally, we'd add templates here to the session but
        // instead they're done by commands. See the cmdTemplate()
        // function.
        //   rc = IPDR_CreateTemplate(clientH, templateId, templateDesc, numKeys, keys, &templateH);

        // And then we'd start the client; but again, that's done by a
        // command:
        //   rc = IPDR_StartExporter(clientH);

        // And then we'd send data; but that's also done by a command:
        //   rc = IPDR_SendData(clientH, record.templateH, &record);

        // And finally we'd drain; but that's also done by a command:
        //   rc = IPDR_DrainQueue(clientH, sessionH, millisec);


        // ---------------------------------------------
        // Main loop, executing commands from command line or standard input.

        unsigned long recNum = 0;
        bool end = false;

        if (!cliCmdStr.empty()) {
            cerr << outputPrefix << "command from CLI: " << cliCmdStr << endl;
            executeCommand(clientH, rec, recNum, recordFile, cliCmdStr, &end);
        }

		newArgc = argc;
		newArgv = const_cast<char**>(argv);

		for(int newArgvCountIndex = 0 ; newArgvCountIndex < newArgvCount ; newArgvCountIndex++)
		{
			free(newArgvStrings[newArgvCountIndex]);
		}
		newArgvCount = 0;

        while (! end /*&& ! cin.eof()*/) {
            string cmd;
            getline(cin, cmd);
            executeCommand(clientH, rec, recNum, recordFile, cmd, &end);
        }

        cerr << outputPrefix << "Shutting down." << endl;
    } else {
        // longjmp here from signal handler
        cerr << outputPrefix << "Caught interrupt, cleaning up." << endl;
    }

    cerr << outputPrefix << "..." << endl;

    // Delete the client and remove the platform services.

	//IPDR_MIB_UnSetExporterHandle();

    rc = IPDR_DeleteExporter(clientH);
    if (FAILURE(rc)) {
        cerr << outputPrefix << "IPDR_DeleteExporter failed: " << IPDR_StatusMsg(clientH, rc) << endl;
        //exit(1);
    }

	IPDR_MIB_Stop(&services);

    rc = services.Delete(services.PlatformArg);
    if (FAILURE(rc)) {
        cerr << outputPrefix << "Platform delete  failed: " << IPDR_StatusMsg(clientH, rc) << endl;
        //exit(1);
    }

	for(int newArgvCountIndex = 0 ; newArgvCountIndex < newArgvCount ; newArgvCountIndex++)
	{
		free(newArgvStrings[newArgvCountIndex]);
	}
	newArgvCount = 0;

    delete recordFile;

    cerr << outputPrefix << "Finished at " << nowStrX() << endl;

    return 0;
}
