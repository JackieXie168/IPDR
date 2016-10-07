package com.xacct.CRANE.collector;

// $Id: Test.java,v 1.4 2005/01/30 15:13:39 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.io.File;
import java.net.ConnectException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.Properties;
import java.util.TreeSet;

import org.jdom.Document;
import org.jdom.JDOMException;
import org.jdom.input.SAXBuilder;

/** Tester for the collector of data, using the CRANE protocol.
 *  This program takes arguments:
 *  @version $Id: Test.java,v 1.4 2005/01/30 15:13:39 ilyad Exp $
 *  @see #main
 */
public class Test implements Runnable
{
    /** $Id: Test.java,v 1.4 2005/01/30 15:13:39 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: Test.java,v 1.4 2005/01/30 15:13:39 ilyad Exp $"; }

    /// Hard-coded configuration: how long to wait for a response from UDP version negotiation request.
    private static final int defaultConnectTimeout = Constants.DEFAULT_CONNECT_TIMEOUT;

    /// Hard-coded configuration: how long to wait before trying to reconnect to a transmitter.
    private static final int defaultReconnectTimeout = Constants.DEFAULT_RECONNECT_INTERVAL;

    /** Entry point for testing
     *  @param arg The command-line arguments:
     *  <br><code>Test <i>configFile</i> <i>secondsToRun</i>|0 {csv|null|null0|dedup|config}</code>
     *  <dl>
     *  <dt><i>configFile</i><dd>The XML configuration file. If this is "", then a hard-coded
     *       configuration, using the transmitter address and port(s) from the arguments.
     *  <dt><i>secondsToRun</i><dd>Run the main thread for this long, then stop.
     *                   If this value is zero, run "forever".
     *  <dt><code>csv</code> | <code>null</code><dd>If "csv", output all records
     *        in a CSV-like format;
     *        <br>if "null", throw away all output (useful for performance testing);
     *        <br>if "null0", throw away all output and also do minimal processing
     *            of input (useful for performance testing);
     *        <br>if "dedup" count the occurrences of each sequence number and
     *            output a summary at the end.
     *        <br>if "config", don't process data but instead read the configuration
     *            information from the transmitter -- only a "skeleton" XML
     *            is needed (see below for this).
     *  </dl>
     *
     *  The minimal configuration file is something like this:
     *  <code>
     *  <br>&lt;?xml version="1.0" encoding="UTF-8"?&gt;
     *  <br>&lt;CRANEcollectorConfiguration xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="CRANEcollector.xsd"&gt;
     *  <br>&nbsp;&nbsp;&lt;Clients&gt;
     *  <br>&nbsp;&nbsp;&nbsp;&nbsp;&lt;Client&gt;
     *  <br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&lt;IP&gt;127.0.0.1&lt;/IP&gt;
     *  <br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&lt;Port&gt;15000&lt;/Port&gt;
     *  <br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&lt;VerPort&gt;15000&lt;/VerPort&gt;
     *  <br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&lt;VendorString/&gt;
     *  <br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&lt;CnctTO&gt;5000&lt;/CnctTO&gt;
     *  <br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&lt;CnctRetryTO&gt;2000&lt;/CnctRetryTO&gt;
     *  <br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&lt;RecnctItvl&gt;10000&lt;/RecnctItvl&gt;
     *  <br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&lt;CnctRndtrpTO&gt;2000&lt;/CnctRndtrpTO&gt;
     *  <br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&lt;AckLogItvl&gt;60000&lt;/AckLogItvl&gt;
     *  <br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&lt;Desc/&gt;
     *  <br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&lt;Sessions/&gt;
     *  <br>&nbsp;&nbsp;&nbsp;&nbsp;&lt;/Client&gt;
     *  <br>&nbsp;&nbsp;&lt;/Clients&gt;
     *  <br>&nbsp;&nbsp;&lt;Templates/&gt;
     *  <br>&nbsp;&nbsp;&lt;ConfigurationVersion&gt;1&lt;/ConfigurationVersion&gt;
     *  <br>&nbsp;&nbsp;&lt;ServerIPAddress/&gt;
     *  <br>&nbsp;&nbsp;&lt;ServerPort&gt;15001&lt;/ServerPort&gt;
     *  <br>&nbsp;&nbsp;&lt;ServerAckInterval&gt;1000&lt;/ServerAckInterval&gt;
     *  <br>&nbsp;&nbsp;&lt;ServerStartInterval&gt;10&lt;/ServerStartInterval&gt;
     *  <br>&nbsp;&nbsp;&lt;EnableDebugMsg&gt;false&lt;/EnableDebugMsg&gt;
     *  <br>&lt;/CRANEcollectorConfiguration&gt;
     *  </code>
     */
    static public void main(String [] arg) {
        Test test = new Test(arg);
        test.run();
    }

    private String configFileName;
    private int milliSecToRun;
    private String typeOfOutput;
    private LogHandler log = new CollectorLog();
    private FieldDescriptorFactory fieldFactory = new TestFieldDescriptorFactory();

    private CollectorManager collectorManager = null;
    private CollectorOutputterFactory outputterFactory = null;
    private CollectorConfig collectorConfig = null;

    private ConfigXml configXml = null;

    /** Create the test run object.
     *  Calls System.exit if things go horribly wrong.
     *  @param arg The arguments from the command line.
     */
    public Test(String [] arg) {
        if (arg.length != 3) {
            CollectorLog.printerrln("Test <configFile> <secondsToRun>|0 {csv|null|null0|dedup|config}");
            System.exit(1);
        }

        // Get the command-line parameters

        configFileName     = arg[0];
        milliSecToRun      = parseInt(arg[1], "secondsToRun") * 1000;
        typeOfOutput       = arg[2].trim();
    }

    public final void run() {
        // log.setDebug();
        log.setInfo();

        if (false) { // for testing ... dump all system properties
            showSystemProperties();
        }

        if (false) { // for testing ... check that little- and big-endian data are transformed properly.
            log.info(this, "main", "--- big endian data test ---");
            testData(new ProtocolDataBigEndian());
            log.info(this, "main", "--- little endian data test ---");
            testData(new ProtocolDataLittleEndian());
        }

        if (false) { // for testing ... check that IP addresses are converted properly.
            log.info(this, "main", "--- IP address conversion test ---");
            testIpAddress();
        }

        readConfigurationXML();

        if (log.debug()) {
            log.debug(this, "main", "--- base configuration ---");
            ConfigXml.outputXml(configXml.getXml(), System.err);
        }

        boolean getConfigurationFromTransmitter = false;

        // Set up the outputter ...
        if (typeOfOutput.equals("csv")) {
            outputterFactory = new CsvOutputterFactory(System.out,
                                                       collectorConfig,
                                                       CsvOutputter.styleNameEqValue,
                                         ", ");
        } else if (typeOfOutput.equals("null")) {
            outputterFactory = new NullOutputterFactory(System.out);
        } else if (typeOfOutput.equals("null0")) {
            outputterFactory = new NullOutputterFactory(System.out);
        } else if (typeOfOutput.equals("dedup")) {
            outputterFactory = new DedupSummaryOutputterFactory(System.out);
        } else if (typeOfOutput.equals("config")) {
            outputterFactory = new NullOutputterFactory(System.out);
            getConfigurationFromTransmitter = true;
        } else {
            CollectorLog.printerrln("CollectorManager ... type of output must be \"csv\", \"null\", or \"dedup\"");
            System.exit(1);
        }

        if (getConfigurationFromTransmitter) { // This would normally be done by configuration GUI
            log.info(this, "main", "Getting configuration from " +
                     collectorConfig.getConnectionConfigListLength() + " exporter(s)");
            for (Iterator it = collectorConfig.getConnectionConfigList().iterator(); it.hasNext(); ) {
                ConnectionConfig connection = (ConnectionConfig)it.next();
                log.info(this, "main", "Getting configuration from " + connection);
                getConfigurationInfoFromTransmitter(collectorConfig, connection);
            }
            return;
        }

        log.debug(this, "main", "Collector configuration: " + collectorConfig.toStringLong());

        log.info(this, "main", "Collector will run for " + (milliSecToRun/1000) + " seconds");

        // Finally, we've processed the command arguments, so let's
        // start connecting to the transmitters and receiving data.
        try {
            collectorManager = new CollectorManager(collectorConfig,
                                                    log,
                                                    outputterFactory);
            if (typeOfOutput.equals("null0")) {
                collectorManager.setNullInput();
            }
            if (milliSecToRun == 0) {
                Thread.sleep(Integer.MAX_VALUE);
                log.info(this, "main", "Letting collector run for maximum time");
            } else {
                log.info(this, "main", "Letting collector run for " + (milliSecToRun/1000) + " seconds");
                Thread.sleep(milliSecToRun);
            }
        } catch (InterruptedException e) {
            log.info(collectorManager, "main", "finished: " + e);
        } catch (Exception e) {
            log.fatal(this, "main",
                      "CollectorManager error (file " + configFileName +  "): " + e);
            CollectorLog.stackTrace(e);
            showStatus();
            System.exit(1);
        } finally {
            showStatus();
            log.info(this, "main", "Stopping the collectors");
            collectorManager.stopAndWaitForConnectionsToFinish();
            log.info(this, "main", "Finished");
        }
    }

    private final void showStatus() {
        if (collectorManager != null) {
            collectorManager.logConnectionStatus(log);
        }
    }

    private final void readConfigurationXML() {
        log.debug(this, "main", "Configuration file: " + configFileName);

        try {
            configFromFile();
        } catch (Exception e) {
            log.fatal(this, "main",
                      "configuration error (file " + configFileName +  "): " + e);
            CollectorLog.stackTrace(e);
            showStatus();
            System.exit(1);
        }
    }

    private final void configFromFile() throws JDOMException, ConfigException {
        log.debug(this, "main", "Configuration from file " + configFileName);

        File configFile = new File(configFileName);
        Document configurationDoc = (new SAXBuilder(false/*true*/)).build(configFile);
        log.debug(this, "setConfigurationFromXML",
                  "doc from file " + configFile +  ": " + configurationDoc);
        if (log.debug()) {
            ConfigXml.outputXml(configurationDoc, System.err);
        }

        configXml = new ConfigXml(configurationDoc, fieldFactory);

        this.collectorConfig = configXml.getCollectorConfig();
        if (log.debug()) {
            log.debug(this, "main", "--- configuration from " +
                      configFileName + " ---");
            ConfigXml.outputXml(configXml.getXml(), System.err);
            log.debug(this, "main", "collectorConfig: " +
                      this.collectorConfig.toStringLong());
        }
    }

    /** Get the configuration information from the transmitter.  This
     *  would be normally used by the ISM's configuration GUI, to set
     *  up the output fields. It can use a skeleton configuration,
     *  with only the "top level" configuration and conection stuff,
     *  ignoring the session, template, and field stuff.
     */
    private final void getConfigurationInfoFromTransmitter(CollectorConfig setupConfig,
                                                           ConnectionConfig connectionConfig) {
        CollectorLog progressLog = new CollectorLog("**setup** ");
        ExtendedInetAddress transmitterAddress = connectionConfig.getTransmitterAddress();
        int transmitterPort = connectionConfig.getTransmitterPort();
        int transmitterVersionPort = connectionConfig.getTransmitterVersionPort();
        ExtendedInetAddress collectorAddress = setupConfig.getCollectorAddress();
        int collectorConnectPortId = setupConfig.getCollectorConnectPortId();
        int connectTimeout = connectionConfig.getConnectTimeout();
        int reconnectInterval = connectionConfig.getReconnectInterval();
        ConfigXml setupConfigXml = new ConfigXml();

        progressLog.info(this, "getConfigurationInfoFromTransmitter",
                         // " Transmitter: " + ExtendedInetAddress.andPortToString(transmitterAddrStr, transmitterPort) +
                         "Collector: " + collectorAddress +
                         " portID: " + collectorConnectPortId +
                         " using connection: " + connectionConfig);

        getConfigurationInfoFromTransmitter(progressLog,
                                            setupConfigXml,
                                            transmitterAddress,
                                            transmitterPort,
                                            transmitterVersionPort,
                                            collectorAddress,
                                            collectorConnectPortId,
                                            connectTimeout,
                                            reconnectInterval);

        if (log.info()) {
            log.info(this, "main",
                     "--- Configuration read from exporter " +
                     transmitterAddress.andPortToString(transmitterPort) +
                     " ---");
            ConfigXml.outputXml(setupConfigXml.getXml(), System.err);
        }
    }

    private final void getConfigurationInfoFromTransmitter(CollectorLog progressLog,
                                                           ConfigXml setupConfigXml,
                                                           ExtendedInetAddress transmitterAddress,
                                                           int transmitterPort,
                                                           int transmitterVersionPort,
                                                           ExtendedInetAddress collectorAddress,
                                                           int collectorConnectPortId,
                                                           int connectTimeout,
                                                           int reconnectInterval) {
        try {
            CollectorSetupManager manager =
                new CollectorSetupManager(collectorAddress,
                                          collectorConnectPortId,
                                          progressLog,
                                          transmitterAddress,
                                          transmitterPort,
                                          transmitterVersionPort,
                                          connectTimeout,
                                          reconnectInterval);
            Thread managerThread =
                new Thread(manager, "Setup from " +
                           transmitterAddress.andPortToString(transmitterPort));
            managerThread.start();

            progressLog.info(this, "main", "Setup thread started and waiting for result ...");
            managerThread.join();
            progressLog.info(this, "main", "Finished waiting.");

            if (manager.finishedWithoutError()) {
                String vendorString = manager.getVendorString();
                ArrayList sessionConfigList = manager.getSessionConfigList();
                // Just output the values that we received.
                log.info(this, "main", "Results from getting transmitter's session/template/field info ...");
                log.info(this, "main", "Vendor string:\n    " + vendorString);
                log.info(this, "main", "Sessions: " +
                             CollectorUtil.toStringLong(sessionConfigList, "    ", "\n"));
                log.info(this, "main", "--- configuration from transmitter ---");

                ConnectionConfig setupConnection =
                    new ConnectionConfig(transmitterAddress,
                                         manager.getTransmitterPort(), // updated by negotiation
                                         transmitterVersionPort,
                                         manager.getVendorString(),
                                         defaultConnectTimeout,
                                         defaultReconnectTimeout,
                                         sessionConfigList);
                ArrayList setupConnectionList = new ArrayList(1);
                setupConnectionList.add(setupConnection);

                CollectorConfig setupCollector =
                    new CollectorConfig(CollectorConfig.VERSION_1,
                                        collectorAddress,
                                        collectorConnectPortId,
                                        1000, // setupConfig.getAckInterval(),
                                        10,   // setupConfig.getCollectorStartInterval(),
                                        setupConnectionList);

                setupCollector.automapAndEnableAllFields(fieldFactory);

                setupConfigXml.setCollectorConfig(setupCollector,
                                                  "CRANEcollector.xsd", // XXX hard-coded XSD name
                                                  "CRANEcollectorConfiguration"); // XXX hard-coded
            } else { // failed receiving: output the exception info.
                Exception e = manager.getRunException();
                boolean doStackTrace = true;
                if (e instanceof ConnectionIOException &&
                    (((ConnectionIOException)e).getCause() instanceof ConnectException)) {
                    doStackTrace = false;
                }
                handleGetConfigurationException(progressLog, e, doStackTrace,
                                                transmitterAddress, transmitterPort,
                                                "Error reported from connnection thread when getting setup from transmitter");
                return; // System.exit(1);
            }
        } catch (ConfigException e) {
            handleGetConfigurationException(progressLog, e, true,
                                            transmitterAddress, transmitterPort,
                                            "Configuration error getting setup from transmitter");
            return; // System.exit(1);
//      // following commented out because it can't be thrown inside try
//      } catch (UnknownHostException e) {
//          handleGetConfigurationException(progressLog, e, true,
//                                          transmitterAddress, transmitterPort,
//                                          "Could not resolve address for transmitter");
//          return; // System.exit(1);
        } catch (InterruptedException e) {
            handleGetConfigurationException(progressLog, e, true,
                                            transmitterAddress, transmitterPort,
                                            "Interrupted getting setup from transmiter");
            return; // System.exit(1);
        } catch (Exception e) {
            // Other failures are possible but shouldn't happen (not a
            // failure of receiving information from the transmitter,
            // but in the configuration, etc.
            handleGetConfigurationException(progressLog, e, true,
                                            transmitterAddress, transmitterPort,
                                            "Unexpected error when trying to setup from transmitter");
            return; // System.exit(1);
        }
    }

    private final void handleGetConfigurationException(LogHandler progressLog,
                                                       Exception ex,
                                                       boolean doStackTrace,
                                                       ExtendedInetAddress transmitterAddress,
                                                       int transmitterPort,
                                                       String msg) {
        progressLog.error(this, "getConfigurationInfoFromTransmitter",
                          msg + " " +
                          transmitterAddress.andPortToString(transmitterPort) +
                          ": " + ex);
        if (doStackTrace) {
            CollectorLog.stackTrace(ex);
        }
        showStatus();
    }

    /** Utility function for parsing a command-line integer: exit on
     * failure.
     * @param str The command-line integer (string).
     * @param descr Description for an error message.
     * @return The value if successful
     * @throws exit(1) on failure.
     */
    private final int parseInt(String str, String descr) {
        int result = 0;
        str = str.trim();
        try {
            result = Integer.parseInt(str);
        } catch (NumberFormatException e) {
            CollectorLog.printerrln("Test: invalid " + descr + " (" + str + "): " + e);
            System.exit(1);
        }
        return result;
    }

    /** Testing function: see whether we handled addresses such as
     * 0.0.0.0, null, 127.0.0.1 properly (you need to inspect the results,
     * because the actual result will depend on things like local host IP
     * address).
     * @param log Report the results here.
     */
    private final void testIpAddress() {
        String [] addr = { null, "",  " ", "10.0.0.40", "ent450", "1.2.3.4", "www.yahoo.com",
                           "0.0.0.0", "127.0.0.1", "1.2.3.4.5", "foo-bar",
                           "192.168.254.132", "64.169.42.200",
                           "192.168.126.9", "192.168.120.16"};
        for (int i = 0; i < addr.length; i++) {
            String addrDescr = (addr[i] == null) ? "(null)" : "'" + addr[i] + "'";
            try {
                ExtendedInetAddress eAddr = new ExtendedInetAddress(addr[i]);
                log.info(this, "testIpAddress",
                             addrDescr + " ->\t" + eAddr +
                             "\t... name(" + eAddr.getHostName() +
                             ") addr(" + eAddr.getHostAddress() + ")");
            } catch (Exception e) {
                log.info(this, "testIpAddress", addrDescr + " got exception: " + e);
            }
        }
    }


    /** For testing, display the Java Virtual Machine's properties.
     */
    private final void showSystemProperties() {
        Properties properties = System.getProperties();
        System.err.println("Properites:");
        if (false) { // dump in system-defined order and truncating long strings
            properties.list(System.err);
        } else { // dump in alphabetical order and showing long strings
            for (Iterator it = new TreeSet(properties.keySet()).iterator(); it.hasNext(); ) {
                Object key = it.next();
                System.err.println(key + "=" + properties.get(key));
            }
        }
        System.err.println("end of Properties");
    }

    /** For testing, check that big/little-endian extraction functions
     * work properly. This ought to be more automated but isn't yet.
     * @param data The conversion class.
     * @param log Use this for outputting results.
     */
    private final void testData(ProtocolData data) {
        byte [][] bytev = { { (byte)0xff },
                            { (byte)0x01 } };
        byte [][] shortv = { { (byte)0xff, (byte)0xff },
                             { (byte)0x01, (byte)0x02 },
                             { (byte)0x80, 0 } };
        byte [][] intv = { { (byte)0xff, (byte)0xff, (byte)0xff, (byte)0xff },
                           { (byte)0x00, (byte)0x00, (byte)0x01, (byte)0x02 },
                           { (byte)0x80, 0, 0, 0 }  };
        byte [][] longv = { { (byte)0xff, (byte)0xff, (byte)0xff, (byte)0xff,
                              (byte)0xff, (byte)0xff, (byte)0xff, (byte)0xff },
                            { (byte)0xff, (byte)0xff, (byte)0xff, (byte)0xff,
                              (byte)0xff, (byte)0xff, (byte)0xff, (byte)0xfe },
                            { (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00,
                              (byte)0x00, (byte)0x00, (byte)0x00, (byte)0xff },
                            { (byte)0x7f, (byte)0xff, (byte)0xff, (byte)0xff,
                              (byte)0xff, (byte)0xff, (byte)0xff, (byte)0xff },
                            { (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00,
                              (byte)0xff, (byte)0xff, (byte)0xff, (byte)0xff },
                            { (byte)0x80, 0, 0, 0, 0, 0, 0, 0 } };

        for (int i = 0; i < bytev.length; i++) {
            byte [] z = new byte[ProtocolData.byteSize];
            byte v = data.getByte(bytev[i], 0);
            data.putByte(z, 0, v);
            testDataShow(bytev[i],  "  byte", v, z);
        }
        for (int i = 0; i < bytev.length; i++) {
            byte [] z = new byte[ProtocolData.unsignedByteSize];
            short v = data.getUnsignedByte(bytev[i], 0);
            data.putUnsignedByte(z, 0, v);
            testDataShow(bytev[i],  " uByte", v, z);
        }
        for (int i = 0; i < shortv.length; i++) {
            byte [] z = new byte[ProtocolData.shortSize];
            short v = data.getShort(shortv[i], 0);
            data.putShort(z, 0, v);
            testDataShow(shortv[i], " short", v, z);
        }
        for (int i = 0; i < shortv.length; i++) {
            byte [] z = new byte[ProtocolData.unsignedShortSize];
            int v = data.getUnsignedShort(shortv[i], 0);
            data.putUnsignedShort(z, 0, v);
            testDataShow(shortv[i], "uShort", v, z);
        }
        for (int i = 0; i < intv.length; i++) {
            byte [] z = new byte[ProtocolData.intSize];
            int v = data.getInt(intv[i], 0);
            data.putInt(z, 0, v);
            testDataShow(intv[i],   "   int", v, z);
        }
        for (int i = 0; i < intv.length; i++) {
            byte [] z = new byte[ProtocolData.unsignedIntSize];
            long v = data.getUnsignedInt(intv[i], 0);
            data.putUnsignedInt(z, 0, v);
            testDataShow(intv[i],   "  uInt", v, z);
        }
        for (int i = 0; i < longv.length; i++) {
            byte [] z = new byte[ProtocolData.longSize];
            long v = data.getLong(longv[i], 0);
            data.putLong(z, 0, v);
            testDataShow(longv[i],  "  long", v, z);
        }
    }

    /** Helper function: display results of tesing data conversion.
     * @param in The input bytes.
     * @param type Textual description of the data type.
     * @param value The result of converting "in".
     * @param out The byte array result of outputting the value.
     * @param log For reporting the results.
     */
    private final void testDataShow(byte [] in, String type, long value,
                                    byte [] out) {
        log.info(this, "testDataShow", type + ": " + hexByteStr(in, " ") + " -> " + value);
        for (int i = 0; i < out.length; i++) {
            if (in[i] != out[i]) {
                log.info(this, "testDataShow", "*** in(" + hexByteStr(in, " ") + ")" +
                             "!=out(" + hexByteStr(out, " ") + ")");
            }
        }
    }

    /** Display an array of byetes as a string.
     * @param in The value (array of bytes).
     * @param byteSep What goes between the individual bytes (typically " ").
     * @return The resulting string.
     */
    private final String hexByteStr(byte [] in, String byteSep) {
        String sep = "";
        String result = "";
        for (int i = 0; i < in.length; i++) {
            result += sep + hexByteStr(in[i]);
            sep = byteSep;
        }
        return result;
    }

    /** Display a byte as a string.
     * @param i The value.
     * @return As a hex string.
     */
    private final String hexByteStr(byte i) {
        int j = i & 0xff;
        if (j < 16) {
            return "0" + Integer.toHexString(j);
        } else {
            return Integer.toHexString(j);
        }
    }
}
