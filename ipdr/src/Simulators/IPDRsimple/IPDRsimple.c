/** @file IPDRsimple.c
 *
 * IPDRsimple sample "simulation" code for the IPDR client (transmitter)<br>
 *
 * Copyright (2005)  Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 */


#include <IPDR/Defs.h>
#include <IPDR/IPDR.h>
#include <Platforms/Generic/GenericPlatform.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <arpa/inet.h>

// Using old style includes where needed, for some compilers
#include <stddef.h>
#include <signal.h>
#include <setjmp.h>
//
// Prototypes
//
static IPDR_IPAddress makeIP(int i1, int i2, int i3, int i4);
const char *clientConfig(const char *name, void *contextArg);
void logOutputCallback(IPDR_DebugLevel level,
                       const char *levelStr,
                       const char *clientName,
                       const char *module,
                       const char *func,
                       const char *msg,
                       void *context);

/** The data structure that is output.
 */
typedef struct {
    XDR_INT			counter;       /**< gets an over-all record count */
    XDR_INT			counter2;      /**< gets a counter within a group of records */
    DEFINE_STRING(name, 64);
    XDR_UINT		unsigned32;
    XDR_UINT		unsigned32_2;
    XDR_ULONG		unsigned64;
    XDR_IPV4ADDR	ipAddress;
    XDR_MACADDR		macAddress;
    XDR_LONG		ipdrLong;
    XDR_TIMEMSEC	IPDRCreationTime;
    XDR_FLOAT		ipFloat;
	XDR_DOUBLE		ipDouble;
	DEFINE_BASE64(base64, 64);
	DEFINE_HEXBINARY(hexBinary, 64);
	XDR_BOOLEAN		booleanValue;
	XDR_BYTE		byteValue;
	XDR_UBYTE		uByteValue;
	XDR_SHORT		shortValue;
	XDR_USHORT		uShortValue;
	XDR_DATETIME	dateTime;
    XDR_IPV6ADDR	xdrIPV6ADDR;
    XDR_UUID		xdrUUID;
    XDR_TIMEUSEC	timeUsec;
} OutputRecordStruct;

/** Definition of OutputRecordStruct, for a IPDR template.
 *  This definition uses field offsets everywhere.
 */
IPDR_Key outputRecordTemplate[] =
{
    { 1, XDR_TYPE_INT, "counter", "counter", "[explanation of 'counter']",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)offsetof(OutputRecordStruct, counter) },
    { 2, XDR_TYPE_INT, "counter2", "counter2", "[explanation of 'counter2']",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET,
NULL,(IPDR_KeyAccessor)offsetof(OutputRecordStruct, counter2) },
    { 3, XDR_TYPE_STRING, "name", "name", "[explanation of 'name']",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET,
NULL,(IPDR_KeyAccessor)offsetof(OutputRecordStruct, ACCESS_STRING(name)) },
    { 4, XDR_TYPE_UINT, "unsigned32", "unsigned32", "[explanation of 'unsigned32']",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET,
NULL,(IPDR_KeyAccessor)offsetof(OutputRecordStruct, unsigned32) },
    { 5, XDR_TYPE_UINT, "unsigned32_2", "unsigned32_2", "[explanation of 'unsigned32_2']",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET,
NULL,(IPDR_KeyAccessor)offsetof(OutputRecordStruct, unsigned32_2) },
    { 6, XDR_TYPE_ULONG, "unsigned64", "unsigned64", "[explanation of 'unsigned64']",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET,
NULL,(IPDR_KeyAccessor)offsetof(OutputRecordStruct, unsigned64) },
    { 7, XDR_TYPE_IPV4ADDR, "ipAddress", "ipAddress", "[explanation of 'ipAddress']",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET,
NULL,(IPDR_KeyAccessor)offsetof(OutputRecordStruct, ipAddress) },
    { 8, XDR_TYPE_MACADDR, "macAddress", "macAddress", "[explanation of 'macAddress']",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET,
NULL,(IPDR_KeyAccessor)offsetof(OutputRecordStruct, macAddress) },

	{ 9, XDR_TYPE_LONG, "IPDRCLong", "", "Long.",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)offsetof(OutputRecordStruct, ipdrLong) },
	{ 10, XDR_TYPE_TIMEMSEC, "IPDRCreationTime", "", "Record creation time.",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)offsetof(OutputRecordStruct, IPDRCreationTime) },
	{ 11, XDR_TYPE_FLOAT, "IPDR_FLOAT", "", "FLOAT.",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)offsetof(OutputRecordStruct, ipFloat) },
	{ 12, XDR_TYPE_DOUBLE, "IPDR_DOUBLE", "", "DOUBLE.",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)offsetof(OutputRecordStruct, ipDouble) },
	{ 13, XDR_TYPE_BASE64, "IPDR_BASE64", "", "BASE64.",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)offsetof(OutputRecordStruct, ACCESS_BASE64(base64)) },

	{ 14, XDR_TYPE_HEXBINARY, "IPDR_HEXBINARY", "HEXBINARY", "HEXBINARY.",

      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)offsetof(OutputRecordStruct, ACCESS_HEXBINARY(hexBinary)) },
	{ 15, XDR_TYPE_BOOLEAN, "IPDR_BOOLEAN", "", "BOOLEAN.",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)offsetof(OutputRecordStruct, booleanValue) },
	{ 17, XDR_TYPE_BYTE, "IPDR_BYTE", "", "BYTE.",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)offsetof(OutputRecordStruct, byteValue) },
	{ 18, XDR_TYPE_UBYTE, "IPDR_UBYTE", "", "UBYTE.",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)offsetof(OutputRecordStruct, uByteValue) },
	{ 19, XDR_TYPE_SHORT, "IPDR_SHORT", "", "SHORT.",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)offsetof(OutputRecordStruct, shortValue) },
	{ 20, XDR_TYPE_USHORT, "IPDR_USHORT", "", "USHORT.",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)offsetof(OutputRecordStruct, uShortValue) },
	{ 21, XDR_TYPE_DATETIME, "IPDR_DATETIME", "", "DATETIME.",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET, NULL,
      (IPDR_KeyAccessor)offsetof(OutputRecordStruct, dateTime) },
    { 22, XDR_TYPE_IPV6ADDR, "ip6Address", "ip6Address", "[explanation of 'ip6Address']",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET,NULL,(IPDR_KeyAccessor)offsetof(OutputRecordStruct, xdrIPV6ADDR) },
    { 23, XDR_TYPE_UUID, "uuid", "uuid", "[explanation of 'uuid']",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET,NULL,(IPDR_KeyAccessor)offsetof(OutputRecordStruct, xdrUUID) },
    { 24, XDR_TYPE_TIMEUSEC, "timeUsec", "timeUsec", "[explanation of 'timeUsec']",
      IPDR_TRUE, IPDR_ACCESS_BY_OFFSET,NULL,(IPDR_KeyAccessor)offsetof(OutputRecordStruct, timeUsec) }

};

/** Utility function: check a return code, printing a nasty
 *  error message and explanation, then exiting if the result isn't OK.
 *  @param rc The return code to be tested.
 *  @param msg The message to print if the return code isn't IPDR_OK.
 */
static void checkRc(IPDR_Status rc, const char* msg)
{
    if (FAILURE(rc)) {
        fprintf(stdout, "Failed %s: %s\n", msg, IPDR_StatusMsg(NULL, rc));
        exit(1);
    }
}

/** Trim trailing whitespace and newlines from a string.
 *  @param buf The string to be trimmed (it gets shortened as necessary).
 */
static void trimBuf(char* buf)
{
    int len = strlen(buf);
    while (len > 0 && isspace(buf[len - 1] & 0xff)) {
        len--;
        buf[len] = '\0';
    }
}

/** Output a single record.
 *  @param counter The overall counter.
 *  @param i The counter within this group of records.
 *  @param clientH from IPDR_InitExporter
 *  @param templateH from IPDR_CreateTemplate
 */
static void outputRecord(int counter, int i,
                         IPDR_ExporterHandle clientH,
						 IPDR_SessionHandle sessionH,
                         IPDR_TemplateHandle templateH)
{
	/*IPDR_TemplateHandle TH;*/
	OutputRecordStruct rec;
	IPDR_Status rc;
	rec.counter = counter;
	rec.counter2 = i;
	FILL_STRING(rec.name, "xdrBASE64")
	rec.unsigned32 = 1;
	rec.unsigned32_2 = 2;
	rec.unsigned64 = 3;
	rec.ipAddress = makeIP(192, 168, 1, 255);
	rec.macAddress = 0x1122334455667788llu;
	rec.ipdrLong=38675690;
	rec.IPDRCreationTime=123456;
	rec.ipFloat=175.576f;
	rec.ipDouble=12345.45;
	FILL_BASE64(rec.base64, "alex")
	FILL_HEXBINARY(rec.hexBinary, "123456789123456", strlen("123456789123456"));
	rec.booleanValue=1;
	rec.byteValue=5;
	rec.uByteValue=3;
	rec.shortValue=99;
	rec.uShortValue=111;
	rec.dateTime=0;
	memcpy(rec.xdrIPV6ADDR, "1234567812345678", 16);
	long helper = htonl(16);
	memcpy(rec.xdrUUID, &helper, 4);
	memcpy(&(rec.xdrUUID[4]), "1234567812345678", 16);
	rec.timeUsec=0xAAA;

	/*rc = IPDR_GetTemplate(sessionH, 1, NULL, NULL, NULL, &TH);
	rc = IPDR_DeleteTemplate(sessionH, TH);*/

	rc = IPDR_SendData(clientH, sessionH,templateH, &rec);
	checkRc(rc, "IPDR_SendData");
}



/** Output batches of records. This reads numbers from stdin (one per
 *  line) and outputs that many records in each batch.
 *  @param clientH from IPDR_InitClient
 *  @param templateH from IPDR_CreateTemplate
 */

static void outputRecords(IPDR_ExporterHandle clientH,
				  IPDR_SessionHandle sessionH,
                          IPDR_TemplateHandle templateH)
{

    int counter = 0;
    int value;
    int i;

    char buf[100];

    fprintf(stdout, "Please insert number of record to send and press enter\n");
    while (0 != fgets(buf, sizeof buf, stdin)) {
        trimBuf(buf);
        value = atoi(buf);
        if (value < 0) {
	        fprintf(stdout, "ERROR - Outputting %d less then zero\n", value);
            break;
        }

        for (i = 0; i < value; i++) {
            outputRecord(counter, i, clientH, sessionH,templateH);
            counter++;
        }
	    fprintf(stdout, "Please insert number of record to send and press enter\n");
		IPDR_SleepMS(clientH, 5*1000);
	}
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



const char *clientConfig(const char *name,
                         void *contextArg)
{
    if (0 == strcmp(name, "configuration.console.consolePort")) {
        return "40000";
    }

    if (0 == strcmp(name, "configuration.vendorString")) {
        return "Amdocs Technologies Ltd. - IPDR simple client simulator";
    }

    if (0 == strcmp(name, "configuration.statisticsInterval")) {
        return "30000"; /* 30 seconds */
    }

    return NULL; /* default */
}



/** Utility for making a IPDR_IPAddress from 4 integers.
 */

static IPDR_IPAddress makeIP(int i1, int i2, int i3, int i4)
{
    return ((i1&0xff) << 24) +
        ((i2&0xff) << 16) +
        ((i3&0xff) << 8) +
        ((i4&0xff) << 0);
}


/** Callback for displaying a IPDR event.
 */

static void eventCallback(const IPDR_EventData* event,
                          const char *msg,
                          void *context)
{
    fprintf(stdout, "*** event [%.99s] %s\n", IPDR_EventMsg(event), msg);
    fflush(stdout);
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
/*extern "C" */

void logOutputCallback(IPDR_DebugLevel level,
                       const char *levelStr,
                       const char *clientName,
                       const char *module,
                       const char *func,
                       const char *msg,
                       void *context)
{

    IPDR_ExporterHandle *clientH = (IPDR_ExporterHandle *)context;

    char timeStrBuf[IPDR_TIME_FORMAT_LENGTH + 1];

    fprintf(stdout, "%.30sZ %.7s <%.40s> {%-8s} %.99s: %.299s\n",
            IPDR_FormatTimeGMTNow(*clientH, timeStrBuf),
            levelStr, clientName, module, func, msg);
    fflush(stdout);
}

/** The main program.
 *  @param argc The number of arguments + 1 for the program name
 *  @param argv The arguments (<code>argv[0]</code> is the program name)
 *  @return Zero for success; non-zero for error.
 */

int main(int argc, const char * const argv[])

{

    IPDR_Status rc = 0;
    IPDR_PlatformServices services;
    IPDR_ExporterHandle clientH;
    IPDR_CollectorHandle serverH;
    IPDR_SessionHandle sessionH;
    IPDR_TemplateHandle templateH;
    long drainMillisec = 60000; /* 60 seconds */



    fprintf(stdout, "GenericPlatform_Initialize ...\n"); fflush(stdout);
    rc = GenericPlatform_Initialize(&services);
    checkRc(rc, "GenericPlatform_Initialize");



    rc = IPDR_InitExporter(&services,
                          "*", // "IPDRsim",
                          clientConfig,
                          0, /* &clientConfigContext */
                          0, /* loadMIBcallback */
                          0, /* configFileName */
                          0, /* saveMIBcallback */
                          0, /* configFileName */
                          &clientH);
    checkRc(rc, "IPDR_InitExporter");

    IPDR_SetModuleMoreDebugLevel(clientH, "Exporter", IPDR_LEVEL_DEBUG);
    IPDR_SetModuleMoreDebugLevel(clientH, "Platform", IPDR_LEVEL_INFO);
    IPDR_SetModuleMoreDebugLevel(clientH, "TCP",      IPDR_LEVEL_INFO);
    IPDR_SetModuleMoreDebugLevel(clientH, "Host",     IPDR_LEVEL_INFO);
    IPDR_SetModuleMoreDebugLevel(clientH, "Queue",    IPDR_LEVEL_INFO);
    IPDR_SetModuleMoreDebugLevel(clientH, "Memory",   IPDR_LEVEL_INFO);
    IPDR_SetModuleMoreDebugLevel(clientH, "Protocol", IPDR_LEVEL_INFO); // IPDR_LEVEL_WARNING); // INFO shows things like template change messages.





    fprintf(stdout, "IPDR_SetLogOutputCallbackDetailed...\n"); fflush(stdout);

    rc = IPDR_CreateSession(clientH, "session1", 1, "NE/one", &sessionH);
    rc = IPDR_AddCollector(clientH, "server1", makeIP(10,17,38,244), 0, 0);

    fprintf(stdout, "IPDR_AddCollector...\n"); fflush(stdout);

    rc = IPDR_GetCollector(clientH,"server1",&serverH, NULL, NULL, NULL);
    rc = IPDR_AssocCollectorSession(clientH,sessionH,serverH,10);

    fprintf(stdout, "IPDR_AssocCollectorSession...\n"); fflush(stdout);

    rc = IPDR_CreateTemplate(
		  clientH,sessionH, 1, "OutputRecord","",
	      sizeof outputRecordTemplate / sizeof outputRecordTemplate[0],
		  outputRecordTemplate,
		 &templateH);


    fprintf(stdout, "IPDR_CreateTemplate...\n"); fflush(stdout);
    fprintf(stdout, "IPDR_SetExporterEventHandlerDetailed...\n"); fflush(stdout);

    IPDR_SetExporterEventHandlerDetailed(clientH, eventCallback, &clientH);
    checkRc(rc, "IPDR_SetExporterEventHandlerDetailed");

    fprintf(stdout, "IPDR_StartExporter...\n"); fflush(stdout);

    rc = IPDR_StartExporter(clientH);
    checkRc(rc, "IPDR_StartExporter");

    outputRecords(clientH, sessionH,templateH);

    fprintf(stdout, "Waiting up to %g seconds to drain the output ...\n", drainMillisec / 1000.0); fflush(stdout);

    rc = IPDR_DrainQueue(clientH, sessionH, drainMillisec);
    checkRc(rc, "IPDR_DrainQueue");

    fprintf(stdout, "IPDR_DeleteExporter...\n"); fflush(stdout);

    rc = IPDR_DeleteExporter(clientH);
    checkRc(rc, "IPDR_DeleteExporter");

    fprintf(stdout, "services.Delete ...\n"); fflush(stdout);

    rc = services.Delete(services.PlatformArg);

    checkRc(rc, "services.Delete");

    fprintf(stdout, "Processing finished.\n"); fflush(stdout);


    return 0;
}

