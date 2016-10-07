package com.xacct.CRANE.collector;

// $Id: ConnectionThread.java,v 1.4 2005/01/30 15:13:39 ilyad Exp $
// Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.Iterator;
import java.util.ListIterator;
import java.util.Random;

/** The thread that processes a single CRANE connection.
 *  @see CollectorManager
 *
 *  @version $Id: ConnectionThread.java,v 1.4 2005/01/30 15:13:39 ilyad Exp $
 */
public class ConnectionThread extends Thread implements ProcessDataSeqNoAck, java.io.Serializable
{
    /** $Id: ConnectionThread.java,v 1.4 2005/01/30 15:13:39 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: ConnectionThread.java,v 1.4 2005/01/30 15:13:39 ilyad Exp $"; }

    /// How often to output data-ack message to log
    private static int dataAckLogInterval;
    public void setDataAckMsgInterval(int value) { this.dataAckLogInterval = value; }
    public int getDataAckMsgInterval() { return this.dataAckLogInterval; }

    /** When to output the next data-ack message to log
     *  @see ConnectionConfig#dataAckLogInterval
     */
    private static long nextDataAckLogMsgTime = System.currentTimeMillis();

    /// A flag for stopping the thread(s) (set by the stop() method)
    protected volatile boolean isRunning;

    /// The collector configuration
    protected CollectorConfig collectorConfig;

    /// The connection from configuration
    protected ConnectionConfig connectionConfig;

    /// The general outputting class
    private CollectorOutputter outputter;

    /// The "boot time" of the data exporter
    private Date exporterBootTime = null;

    /// The log
    protected LogHandler log;

    /// The connection
    protected ConnectionDynamic connection;

    /// Receiver buffer size for the connection
    private int receiveBufferSize;

    private ArrayList staticFields;

    /** A flag for turning of as much input processing as possible
     *  (useful for performance testing).
     */
    private boolean nullInput;
    public void setNullInput() { this.nullInput = true; }

    /** Map a session id to the data converter (big/little endian).
     *  The CRANE protocol specifies that the big/little endian
     *  decision is made on a per-session basis; but in practice it'll
     *  be on a per-connection basis. So, we keep a
     *  {@link ProtocolData connectionDataConverter} around; it'll be
     *  set to null if there are no data converters or if we need to
     *  actually map by session id.
     */
    private HashMap sessionIdToDataConverter;
    private ProtocolData connectionDataConverter;
    private final ProtocolData getSessionDataConverter(int id) throws ConfigException {
        ProtocolData result = connectionDataConverter;
        if (result == null) {
            result = (ProtocolData)sessionIdToDataConverter.get(new Integer(id));
            if (result == null) {
                throw new ConfigException(this, "sessionIdToDataConverter",
                                          "unknown session id: " + id);
            }
        }
        return result;
    }

    private final void setSessionBigEndian(int id, boolean bigEndian) {
        ProtocolData pd =
            bigEndian ?
            (ProtocolData)new ProtocolDataBigEndian() :
            (ProtocolData)new ProtocolDataLittleEndian();
        if (connectionDataConverter == null) {
            if (sessionIdToDataConverter.isEmpty()) {
                connectionDataConverter = pd;
            }
        } else if (connectionDataConverter.getClass() != pd.getClass()) {
            connectionDataConverter = null; // it'll be looked up on a per-session-id basis
        }
        sessionIdToDataConverter.put(new Integer(id), pd);
    }

    /** Create a connection thread.
     * @param collectorConfig The collector context.
     * @param connectionConfig The connection configuration.
     * @param outputterFactory The factory for creating outputter objects.
     * @param log The log for messages.
     * @param receiveBufferSize The input socket size (0 means default).
     */
    public ConnectionThread(CollectorConfig collectorConfig,
                            ConnectionConfig connectionConfig,
                            CollectorOutputterFactory outputterFactory,
                            LogHandler log,
                            int receiveBufferSize) {
        this.collectorConfig = collectorConfig;
        this.log   = log;
        this.connection = null;
        this.connectionConfig = connectionConfig;
        setName(connectionConfig.generateThreadName());
        this.isRunning = false;
        this.exporterBootTime  = null; // generate an error if it's used
        this.outputter = (outputterFactory == null) ? null : outputterFactory.create();
        this.sessionIdToDataConverter = new HashMap();
        this.connectionDataConverter = null;
        this.nullInput = false;
        this.receiveBufferSize = receiveBufferSize;
        this.dataAckLogInterval = connectionConfig.getDataAckLogInterval();
        prepareForRunning();
    }

    /** Process the configuration for more efficient access. This
     *  method <em>must</em> be called whenever any part of the
     *  configuration is changed.
     *
     *  XXX To be defined: how to handle a change as a result of
     *  template negotiation.
     *  XXX Note that we swap in new templates that we
     *  get from TMPL_DATA.
     */
    protected void prepareForRunning() {
        prepareSessionConfigForRunning();
        // XXX Neeed to fix stuff for lookupTemplate() to be more
        // efficient.
    }

    /** Minimum session ID seen so far. */
    private int minSessionId = Integer.MAX_VALUE;
    /** Maximum session ID seen so far. */
    private int maxSessionId = Integer.MIN_VALUE;
    /** Valid session IDs (offset by minSessionID). */
    private SessionConfig [] sessionLookup = null;

    /** Look up a session.
     * @param sessionId
     * @return session configuration corresponding to the session id
     * @throws ConfigException If invalid session id
     */
    private final SessionConfig lookupSession(int sessionId)
        throws ConfigException {
        if (sessionId != cachedLookupSessionId || cachedLookupSession == null) {
            try {
                // cachedLookupSession = connectionConfig.getSessionConfigById(sessionId);
                cachedLookupSession = sessionLookup[sessionId - minSessionId];
                cachedLookupSessionId = sessionId;
            } catch (IndexOutOfBoundsException e) {
                cachedLookupSession = null;
            }
        }
        if (cachedLookupSession == null) {
            throw new ConfigException(this, "lookupSession",
                                      "Unknown session: id=" + sessionId + " in range " +
                                      minSessionId + "..." + maxSessionId + " in " +
                                      connectionConfig +
                                      " ... looked in " + CollectorUtil.toStringLong(connectionConfig.getSessionConfigList()));
        }
        return cachedLookupSession;
    }

    /** It's not clear whether this caching helps or not but it's not
     *  a difficult thing to do, so leave it in for now. */
    protected SessionConfig cachedLookupSession = null;
    /** The session ID corresponding to the cached session.
     *  @see #cachedLookupSession
     */
    protected int cachedLookupSessionId = -1;

    /** Look up a template within a session
     *  XXX This code is inefficient if there are more than one template;
     *  the templates need to be in a vector for fast lookup.
     * @param sessionId
     * @param templateId
     * @return template configuration corresponding to the session id, template id
     * @throws ConfigException If invalid session id
     */
    private final TemplateConfig lookupTemplate(int sessionId, int templateId)
        throws ConfigException {
        if (sessionId != cachedLookupTemplateSessionId || cachedLookupTemplateSession == null) {
            cachedLookupTemplateSession = lookupSession(sessionId);
            cachedLookupTemplateSessionId = sessionId;
            cachedLookupTemplate = null; // force lookup in next step
            cachedLookupTemplateId = -1;
        }
        if (templateId != cachedLookupTemplateId || cachedLookupTemplate == null) {
            cachedLookupTemplate = cachedLookupTemplateSession.getTemplateConfigById(templateId);
            cachedLookupTemplateId = templateId;
        }
        return cachedLookupTemplate;
    }

    private SessionConfig cachedLookupTemplateSession = null;
    private int cachedLookupTemplateSessionId = -1;
    private TemplateConfig cachedLookupTemplate = null;
    private int cachedLookupTemplateId = -1;


    private final void prepareSessionConfigForRunning() {
        for (Iterator it = connectionConfig.getSessionConfigList().iterator(); it.hasNext(); ) {
            SessionConfig sess = (SessionConfig)it.next();
            log.debug(this, "prepareSessionConfigForRunning",
                      "Preparing session " + sess.getId());
            minSessionId = Math.min(sess.getId(), minSessionId);
            maxSessionId = Math.max(sess.getId(), maxSessionId);
        }
        if (maxSessionId >= minSessionId) {
            sessionLookup = new SessionConfig[maxSessionId - minSessionId + 1];
        }
        log.debug(this, "prepareSessionConfigForRunning",
                  "Session IDs: " + minSessionId + "..." + maxSessionId);
        for (int id = minSessionId; id <= maxSessionId; id++) {
            try {
                sessionLookup[id - minSessionId] = connectionConfig.getSessionConfigById(id);
            } catch (ConfigException e) {
                sessionLookup[id] = null;
                log.info(this, "prepareSessionConfigForRunning",
                         "Skipped non-used session id " + id);
            }
        }
    }


    /** The main processing (thread)
     *
     *  XXX The structure here is clumsy, with runLoop() not having
     *  clear semantics. Especially when this is subclassed (see
     *  ConnectionSetupThread).
     */
    public final void run() {
        isRunning = true;

        while (isRunning) {
            try {
                log.debug(this, "run", "Started CRANE collector");
                runLoop();
            } catch (ProtocolMalformedMsgException e) {
                log.fatal(this, "runLoop", "*** exception: " + e);
                CollectorLog.stackTrace(e);
            } catch (DataException e) {
                log.fatal(this, "runLoop", "*** exception: " + e);
                CollectorLog.stackTrace(e);
            } catch (Throwable e) {
                log.fatal(this, "runLoop", "*** exception: " + e);
                CollectorLog.stackTrace(e);
            } finally {
                if (this.connection != null) {
                    try {
                        this.connection.close(); // kill the connection
                    } catch (Throwable e) {
                        log.error(this, "runLoop", "Could not properly close connection: " + e);
                        CollectorLog.stackTrace(e);
                    }
                    this.connection = null;
                }
            }
            if (isRunning) {
                log.error(this, "runLoop", "Restarting after error");
            }
        }
        if (this.outputter != null) {
            this.outputter.close(); // give it a chance to shut down
            this.outputter = null;
        }
    }

    /// Keep track of the next request Id
    private int nextRequestId = (new Random().nextInt()) & 0x7fff;

    /// An object for synchronizing increment of request id
    private Object syncNextRequestId = new Object();

    /// Get a new request Id and increment the existing one
    protected final int getNextRequestId() {
        synchronized(syncNextRequestId) {
            return nextRequestId++;
        }
    }


    /** Main run loop.
     *
     * XXX The structure of this method isn't quite right, especially with the
     * sub-class ConnectionSetupThread ... need to refactor (outputter vs
     * connection etc).
     *
     * @throws ConfigException <<Description>>
     * @throws ConnectionIOException <<Description>>
     * @throws DataException <<Description>>
     * @throws OutputterException <<Description>>
     * @throws ProtocolMalformedMsgException <<Description>>
     */
    protected void runLoop()
        throws ProtocolMalformedMsgException, DataException, ConfigException,
        ConnectionIOException, OutputterException {

        this.exporterBootTime = null;  // generate an error if it's used

        while (isRunning) {
            try {
                // if (false) { // Used for testing UDP send/receive with "daytime" server
                //     log.print("daytime from " + connectionConfig.transmitterAddress + "(" +
                //               connectionConfig.getConnectTimeout() + "ms): " +
                //               testUDP(connectionConfig.transmitterAddress));
                // }

                runLoopConnection();

                if (isRunning) {
                    sendStatusReq("start-up", "runLoop", false);
                    receiveDataAndOutputIt();
                }

            } catch (ConnectionIOException e) {
                log.error(this, "runLoop",
                          "error with transmitter (client) " +
                          connectionConfig.transmitterAndPortString() +
                          ": " + e.getOriginalMsg() +
                          " ... caused by: " + e.getCause());
            }

            closeConnection();

            if (isRunning) {
                sleepReconnect();
            }
        }
    }

    // synchronized to block processDataSeqNoAck
    synchronized protected final void closeConnection()
    throws ConnectionIOException {
        if (this.connection != null) {
            this.connection.close();
            this.connection = null; // in case of "out of the blue" processDataSeqNoAck
        }
    }

    private final void sleepReconnect() {
        try {
            log.debug(this, "runLoop",
                      "Sleeping before reconnect attempt on " +
                      connectionConfig.transmitterAndPortString() +
                      " (" +
                      connectionConfig.getReconnectInterval() + "msec)");
            sleep(connectionConfig.getReconnectInterval());
        } catch (InterruptedException e) {
            // No need to do anything ... interruption is typically
            // from the stopProcessing() method, which also sets
            // isRunning to false, so the loop will end.
        }
    }

    // synchronized to block processDataSeqNoAck
    synchronized protected final void runLoopConnection()
        throws ConnectionIOException, ProtocolMalformedMsgException,
        ConfigException, DataException {

        makeConnection();

        if (isRunning) {
            for (int i = 0; isRunning && i < connectionConfig.getSessionConfigListLength(); i++) {
                sendStartAndValidateTemplates(connectionConfig.getSessionConfigAt(i));
                // collectorConfig.validateTemplatesFromTransmitter(templateConfigList);
            }
        }
    }

    // updates connectionConfig.transmitterPort
    protected final void makeConnection()
        throws ConnectionIOException, ProtocolMalformedMsgException, ConfigException {

        connectionConfig.setTransmitterPort(
            validateTransmitterProtocolVersion(ProtocolMsgInfoRequest.INFO_TRANSPORT_TCP,
                                               ProtocolMsg.PROTOCOL_VERSION));

        log.debug(this, "setTransmittedPort",
                  "Transmitter (client) port=" + connectionConfig.getTransmitterPort());

        this.connection =
            new ConnectionDynamic(collectorConfig, connectionConfig, log, receiveBufferSize);

        ProtocolMsgCONNECT msg =
            new ProtocolMsgCONNECT((byte)0,
                                   collectorConfig.getCollectorAddress().get(),
                                   collectorConfig.getCollectorConnectPortId());
        msg.sendMsg(connection);

        // There is no response in the CRANE protocol to a CONNECT
        // ... it's just used to inform the transmitter that we're here
        // ... next thing is ask for the templates or do a START.
    }


    // Side effect: sets exporterBootTime

    private final void sendStartAndValidateTemplates(SessionConfig sessionConfig)
        throws ConnectionIOException, ProtocolMalformedMsgException,
        DataException, ConfigException {
        ProtocolMsgSTART msgSTART = new ProtocolMsgSTART((byte)sessionConfig.getId());
        ProtocolMsg responseMsg = null;
        try {
            msgSTART.sendMsg(connection);
            // XXX Is indefinite wait the correct thing here? What if the connection dies ...
            // we should restart, I think. Need a separate timeout value from config, though.
            responseMsg = ProtocolMsg.readMsg(connection, "response to", msgSTART, 0);
        } catch (ConnectionIOException e) {
            // if (e.getCause() instanceof ConnectException) {
            throw new ConnectionIOException(e.getOriginalMsg() +
                                            " - possibly because not registered at transmitter?",
                                            connection,
                                            e.getCause());
        }

        if (responseMsg instanceof ProtocolMsgSTART_ACK) {
            this.exporterBootTime = ((ProtocolMsgSTART_ACK)responseMsg).bootTime;
            // Next should be the TMPL DATA message
            // XXX Is indefinite wait the correct thing here? What if the connection dies ...
            // we should restart, I think. Need a separate timeout value from config, though.
            responseMsg = ProtocolMsg.readMsg(connection, "(TEMPL DATA) response to", msgSTART, 0);
        } else {
            log.warn(this, "sendStartAndValidateTemplates",
                     "Did not get START ACK from transmitter (client)");
        }

        if (responseMsg instanceof ProtocolMsgTMPL_DATA) {
            log.info(this, "sendStartAndValidateTemplates",
                     "After START/ACK: " +
                     responseMsg.toStringLong());
            handleTMPL_DATA((ProtocolMsgTMPL_DATA)responseMsg);
        } else {
            throw new DataException(this, "sendStartAndValidateTemplates",
                                    "Expected TMPL DATA from request message " + msgSTART +
                                    " but received: " + responseMsg);
        }

    }

    // XXX A TMPL_DATA can arrive at any time. Because we're swapped
    // the template data (via setSessionTemplate), we
    // might have lost information from the configuration (e.g., for
    // fields that have been turned off at the transmitter). So, the
    // code here isn't right ... it ought to get the configuration
    // data directly (that is, retrieve the session information), then
    // process with it. An alternative would be to save the template
    // information before changing it; then restoring from the changes.

    private final void handleTMPL_DATA(ProtocolMsgTMPL_DATA tmplResponse)
        throws ConfigException, ProtocolMalformedMsgException, ConnectionIOException {

        //verifying that the requested session was retrieved
        SessionConfig sessionConfig = getRuntimeSession(connectionConfig, tmplResponse.sessionId);
        if(sessionConfig == null) { //this shouldn't happen
            log.error(this, "handleTMPL_DATA",
                      "configuration error, the retrieved session does not match the requested session");
            return;
        }
        setSessionBigEndian(sessionConfig.getId(), tmplResponse.bigEndian);

        // setting the session's runtime templates
        for (int i = 0; i < tmplResponse.templateBlock.getTemplateConfigListLength(); i++) {
            TemplateConfig tmplTransmitter = tmplResponse.templateBlock.getTemplateConfigAt(i);
            setSessionTemplate(connectionConfig,sessionConfig,tmplTransmitter);
        }

        prepareForRunning(); // XXX needed because a template might have changed

        // XXX Need to store "enabledForInput" (but see comments in outputDataRecord)

        // XXX need to do this properly ...
//      if (! tmpl.transmitsTheSameAs(tmplTransmitter)) {
//           // XXX We should enter template negotiation at this point
//           //     but for now, treat as an error
//           throw new ConfigException(this, "sendStartAndValidateTemplates",
//                                     "Mis-match of transmission form between transmitter (client) template: " +
//                                     tmplTransmitter.toStringLong() + " and configuration template: " +
//                                     tmpl.toStringLong());
//      }

        ProtocolMsgFINAL_TMPL_DATA_ACK msgFINAL_TMPL_DATA_ACK =
            new ProtocolMsgFINAL_TMPL_DATA_ACK((byte)sessionConfig.getId(),
                                               tmplResponse.configId);
        msgFINAL_TMPL_DATA_ACK.sendMsg(connection);
    }

    /** Basic validation method. If the session is not found in configuration, it is
     * an unexpected bug (a log message is outputted back in ConnectionThread).
     * @param connectionConfig
     * @param sessionId
     * @return session
     * @throws ConfigException
     * @throws ConnectionIOException
     * @throws ProtocolMalformedMsgException
     */
    private SessionConfig getRuntimeSession(ConnectionConfig connectionConfig,int sessionId)
        throws ConfigException, ProtocolMalformedMsgException, ConnectionIOException {
        return connectionConfig.getSessionConfigById(sessionId);
    }

    /** Setting the runtime template, as retrived by handleTMPL_DATA.
     * The template would be compared against the session in
     * configuration and modified according to the runtime
     * information. If a mismatch is found, a matching log message is
     * output.
     *
     * @param connectionConfig <<Description>>
     * @param transmitterSession <<Description>>
     * @param tmplTransmitter <<Description>>
     * @throws ConfigException <<Description>>
     */
    public void setSessionTemplate(ConnectionConfig connectionConfig,
                                   SessionConfig transmitterSession,
                                   TemplateConfig tmplTransmitter) throws ConfigException {
        //getting the SessionConfig from configuration
        ConnectionConfig savedConnection = null;
        SessionConfig savedSession = null;
        TemplateConfig savedTemplate = null;
        try {
            savedConnection =
                collectorConfig.getConnectionConfigById(connectionConfig.getTransmitterAddress(),
                                                        connectionConfig.getTransmitterVersionPort());
        } catch (ConfigException ce) {
            throw new ConfigException("ConnectionThread.setSessionTemplate: Could not find the matching connection config on retrieval of GET_TEMPLATE: " + ce);
        }
        if (savedConnection != null) {
            savedSession = savedConnection.lookupSessionConfigById(transmitterSession.getId());
            if(savedSession == null) {
                throw new ConfigException("ConnectionThread.setSessionTemplate: Could not find the matching session config on retrieval of GET_TEMPLATE");
            }
        }
        savedTemplate = savedSession.lookupTemplateConfigById(tmplTransmitter.getId());
        if (savedTemplate == null) {
            // this is an outstanding template, we'll set it into the
            // session we'll check whether it already exists in the
            // session's outstanding-templates list

            log.warn(this, "setSessionTemplate",
                     "The retrieved template (id: " + tmplTransmitter.getId() +
                     ", description: " + tmplTransmitter.getDescription() +
                     ") from session "+ savedSession.getId() +
                     " and client " + connectionConfig.getTransmitterAddress() +
                     ":" + connectionConfig.getTransmitterVersionPort() +
                     " was not found in configuration." +
                     " All incoming data from this template will be acknowledged but not processed." +
                     " If you wish to collect that data into the system, please reconfigure that session.");

           // setting this template to be part of the configuration
           // object but with no fields (a new array-list)
            tmplTransmitter.set(tmplTransmitter.getId(),
                                new ArrayList(0),
                                tmplTransmitter.getDescription(),
                                tmplTransmitter.isForStatusResponse());

           ArrayList allSessionTemplates = savedSession.getTemplateConfigList();

           boolean isTmplTransmitterFound = false;
           for (ListIterator iter = allSessionTemplates.listIterator(); iter.hasNext(); ) {
               TemplateConfig tmpl = (TemplateConfig)iter.next();
               if(tmpl != null && tmpl.getId() == tmplTransmitter.getId()) {
                   isTmplTransmitterFound = true;
               }
           }
           if (! isTmplTransmitterFound) {
               allSessionTemplates.add(tmplTransmitter);
           }
           savedSession.setTemplates(allSessionTemplates);

           // done with this template...
           return;
        }

        // now we know that the recieved template from the trasmitter
        // exists in configuration, it's time to verify that the
        // fields match.

        ArrayList transmitterTemplateFields = tmplTransmitter.getFieldList();
        ArrayList savedTemplateFields = savedTemplate.getFieldList();
        ArrayList removedTransmitterFields = new ArrayList();
        // iterating through the transmitter fields to detect unusable fields (fields that have
        // no match in configutation... no mapping)
        ArrayList orderedList = new ArrayList();
        for (ListIterator transmitterFieldsIter = transmitterTemplateFields.listIterator();
             transmitterFieldsIter.hasNext(); ) {
            FieldConfig transmitterField = (FieldConfig)transmitterFieldsIter.next();
            FieldConfig savedField = transmitterField.lookupById(null,savedTemplateFields,transmitterField.getId());

            // the cases where the transmitter field is unusable
            // (there is no matching field in the configuraion)
            if(savedField == null ||
               transmitterField.getTypeId() != savedField.getTypeId()) {
                log.warn(this, "setSessionTemplate",
                         "The retrieved field (id: " + transmitterField.getId() +
                         ") from template id " + tmplTransmitter.getId() +
                         " ,session id " + savedSession.getId() +
                         " and client "+ connectionConfig.getTransmitterAddress() +
                         ":" + connectionConfig.getTransmitterVersionPort() +
                         " was not found in configuration." +
                         " All incoming data on this field will be discarded."+
                         " If you wish to collect that data into the system, please reconfigure that template/field.");
            } else {
                if (! savedField.isEnabledForOutput()) {
                    log.warn(this, "setSessionTemplate",
                             "The retrieved field (id: " + savedField.getId() +
                             " label: " + savedField.getLabel() +
                             ") from template id " + tmplTransmitter.getId() +
                             ", session id " + savedSession.getId() +
                             " and client " + connectionConfig.getTransmitterAddress() +
                             ":" + connectionConfig.getTransmitterVersionPort() +
                             " was found in configuration and is disabled for output." +
                             " All incoming data on this field will be discarded." +
                             " If you wish to collect that data into the system, please reconfigure that template/field.");
                }
                if (savedField.getFieldDesc() == null) { // the field is unmapped
                    log.warn(this, "setSessionTemplate",
                             "The retrieved field (id: " + savedField.getId() +
                             " label: " + savedField.getLabel() +
                             ") from template id " + tmplTransmitter.getId() +
                             ", session id " + savedSession.getId() +
                             " and client " + connectionConfig.getTransmitterAddress() +
                             ":" + connectionConfig.getTransmitterVersionPort() +
                             " was found in configuration and is unmapped for output." +
                             " All incoming data on this field will be discarded."+
                             " If you wish to collect that data into the system, please reconfigure that template/field.");
                }

                orderedList.add(savedField);
            }
        }

        // now we can swap the saved template with the transmitter template
        synchronized (savedTemplate) {
            savedTemplate.set(savedTemplate.getId(),
                              orderedList,
                              savedTemplate.getDescription(),
                              savedTemplate.isForStatusResponse());
        }

        TemplateConfig sessionTemplate = savedSession.lookupTemplateConfigById(savedTemplate.getId());
        ArrayList allSessionTemplates = savedSession.getTemplateConfigList();

        for (ListIterator templatesIter = allSessionTemplates.listIterator();
             templatesIter.hasNext(); ) {
            TemplateConfig tc = (TemplateConfig)templatesIter.next();
            if(tc.getId() == sessionTemplate.getId()) {
                int index = allSessionTemplates.indexOf(tc);
                allSessionTemplates.set(index,sessionTemplate);
                break;
            }
        }
        savedSession.setTemplates(allSessionTemplates);
    }

    /** Receive data and output it. This also starts the acknowledgment timer thread.
     *  @param connection
     */
    private final void receiveDataAndOutputIt()
        throws ConnectionIOException, ProtocolMalformedMsgException,
        DataException, ConfigException, OutputterException {

        outputter.registerProcessDataSeqNoAck(this);

        initSeqNoData();

        try {
            // By defining the buffer outside readMsg, we get about 2x
            // speed-up ... but we must be sure to not do another
            // readMsg with the same buffer while we're still using
            // the "msg" that we read (things might get corrupted, at
            // least with ProtocolMsgDATA).

            byte [] readMsgBuf = new byte[0x8000]; // should be big enough
            while (isRunning) {
                long now = System.currentTimeMillis();
                int timeToWait = (int)(nextAckTime - now);
                // XXX Need to avoid multiple zero-wait responses in
                // the case of falling severely behind in processing.
                // see below where nextAckTime is set ... it might not be sufficient.
                // log.debug(this, "receiveDataAndOutputIt", "timeToWait="+timeToWait);

                ProtocolMsg msg;
                if (timeToWait > 0 && timeToWait > collectorConfig.getAckIntervalEpsilon()) {
                    msg = ProtocolMsg.readMsg(readMsgBuf, connection,
                                              "data", null, timeToWait);
                } else {
                    msg = new ProtocolMsgEmpty();
                }
                if (log.debug()) {
                    log.debug(this, "receiveDataAndOutputIt",
                              "data record: " + msg.toStringLong());
                }

                now = System.currentTimeMillis();
                if (msg instanceof ProtocolMsgDATA) {
                    handleSingleDATA((ProtocolMsgDATA)msg);
                    nextNoDataKeepAliveTime = now + collectorConfig.getNoDataKeepAliveInterval();
                } else if (msg instanceof ProtocolMsgEmpty) {
                    // Nothing was read: a timeout or other interruption.

                    outputter.commitTimerExpired();

                    ProtocolMsgEmpty msgEmpty = (ProtocolMsgEmpty)msg;
                    Exception e = msgEmpty.getException();

                    // XXX For JDK 1.4, we can check that we have an
                    // instance of InterruptedIOException ...
                    // for 1.3, suppress unnecessary messages by doing
                    // a comparison with a magic string
                    boolean showWhatHappened = false;
                    if (e == null) { // we generated this ourselves
                        showWhatHappened = false;
                    } else {
                        showWhatHappened = true;
                    }
                    if (log.debug()) { showWhatHappened = true; }

                    if (showWhatHappened) {
                        log.debug(this, "receiveDataAndOutputItLoop",
                                  "Interrupt (timeout?) on input socket: " +
                                  msgEmpty.getException());
                    }

                    // note that processDataSeqNoAck() will set the
                    // next timers (in case it's called from Outputter).

                    if (this.anyDataRecordSeen && outputter.isValidLastSeqNoToAck()) {
                        processDataSeqNoAck(outputter.getLastSeqNoToAck());
                    }
                    if (nextAckTime <= (now + collectorConfig.getAckIntervalEpsilon())) {
                        // try to avoid many zero-time acks if we fall far behind.
                        nextAckTime = now + collectorConfig.getAckInterval() / 2;
                    }

                    if (nextNoDataKeepAliveTime <= now) {
                        // XXX send a "keep-alive" status query
                        sendStatusReq("no data from transmitter", "receiveDataAndOutputIt", true);
                        nextNoDataKeepAliveTime = now + collectorConfig.getNoDataKeepAliveInterval();
                        log.debug(this, "processDataSeqNoAck",
                                  "Next keep-alive: " +
                                  CollectorUtil.dateFormatMillisec(nextNoDataKeepAliveTime));
                    }
                } else if (msg instanceof ProtocolMsgTMPL_DATA) {
                    // NB: ProtcolMsgFINAL_TMPL_DATA is subclass of
                    // ProtocolMsgTMPL_DATA, so it is also handled
                    // here.
                    handleTMPL_DATA((ProtocolMsgTMPL_DATA)msg);
                } else if (msg instanceof ProtocolMsgERROR) {
                    handleERROR((ProtocolMsgERROR)msg);
                } else {
                    throw new DataException(this, "receiveDataAndOutputIt",
                                            "Unexpected message: " + msg.toStringLong());
                }
            }
        }
        //Alex
        catch(Exception e)
        {
          e.printStackTrace();
        }
        //Alex
        finally {
            outputter.registerProcessDataSeqNoAck(null);
        }
    }

    protected void sendStatusReq(String msg, String function, boolean warning)
        throws ConnectionIOException, ProtocolMalformedMsgException, ConfigException {
        ProtocolMsgSTATUS_REQ req = new ProtocolMsgSTATUS_REQ(this.sessionId);
        String fullMsg = "Sending status request [" + msg +
            "] (msg.Id=" + req.msgId() + ") to transmitter on " + connection;
        if (warning) {
            log.warn(this, function, fullMsg);
        } else {
            log.info(this, function, fullMsg);
        }
        req.sendMsg(this.connection);
    }

    // synchronized block processDataSeqNoAck callback
    synchronized protected final void handleSingleDATA(ProtocolMsgDATA msgDATA)
        throws ConnectionIOException, ProtocolMalformedMsgException,
        ConfigException, DataException, OutputterException {

        try {
            this.lastDataSeqNo = msgDATA.processDataSeqNo(lastDataSeqNo, this.anyDataRecordSeen);
        } catch (DataException e) {
            // try to force things out as much as possible before quitting
            log.info(this, "receiveDataAndOutputIt",
                     "Got exception, will try to DATA ACK before restarting. Exception: " + e);
            log.debug(this, "receiveDataAndOutputIt",
                      "anyDataRecordSeen=" + this.anyDataRecordSeen +
                      ", isRunning=" + this.isRunning +
                      ", lastDataSeqNoAcked=" + this.lastDataSeqNoAcked +
                      (outputter.isValidLastSeqNoToAck() ?
                       ", outputter.seqNoToAck=" + outputter.getLastSeqNoToAck() :
                       ", outputter.notValidSeqNoToAck") +
                      ", connection=" + this.connection);
            this.nextDataAckLogMsgTime = System.currentTimeMillis();
            outputter.commitTimerExpired();
            if (outputter.isValidLastSeqNoToAck()) {
                processDataSeqNoAck(outputter.getLastSeqNoToAck());
            }
            // and give up
            try {
                log.info(this, "receiveDataAndOutputIt", "sleeping before closing connection");
                // SoLinger in ConnectionDynamic doesn't seem to work properly.
                sleep(collectorConfig.getAckInterval()); // XXX should be a separate parameter
            } catch (InterruptedException e2) { }
            throw e;
        }

        // XXX Must confirm that seq # is on a
        // connection basis, not a session basis.

        if (this.anyDataRecordSeen) {
            if (this.sessionId != (byte)msgDATA.sessionId ||
                this.configId != (byte)msgDATA.configId) {
                throw new DataException(this, "receiveDataAndOutputIt",
                                        "Unknown sessionId=" + msgDATA.sessionId +
                                        " configId=" + msgDATA.configId +
                                        " (expecting sessionId=" + this.sessionId +
                                        " configId=" + this.configId + ")");
            }
        }
        outputDataRecord(msgDATA);
        this.sessionId = (byte)msgDATA.sessionId;
        this.configId  = (byte)msgDATA.configId;
        this.anyDataRecordSeen = true;
    }

    private final void handleERROR(ProtocolMsgERROR msg) {
        log.error(this, "handleERROR",
                  "Received ERROR message from transmitter (client): " +
                  msg.toStringLong());
    }

    // XXX This information is for the data seq no processing
    // ... probably should be encapsulated a bit better (it's too
    // global to ConnectionThread).

    private boolean anyDataRecordSeen = false;
    long lastDataSeqNo = -1; // meaningless until this.anyDataRecordSeen

    private long nextAckTime = 0; // set in receiveDataAndOutputIt and reset int processDataSeqNoAck

    private long nextNoDataKeepAliveTime = 0; // set similar to nextAckTime

    private long lastDataSeqNoAcked = -1; // meaningless unless anyDataRecordSeen
    // XXX need to decide whether DSN is per sessionID or global
    // XXX if global, then must be set by processDataSeqNoAck()
    // XXX it's suitable garbage here for now ...
    private byte sessionId = -1;          //    ditto
    private byte configId = -1;           //    ditto

    private void initSeqNoData() {
        anyDataRecordSeen = false;
        lastDataSeqNo = -1;
        lastDataSeqNoAcked = -1;
        sessionId = -1;
        configId = -1;
        nextAckTime = System.currentTimeMillis() + collectorConfig.getAckInterval();
        nextNoDataKeepAliveTime = System.currentTimeMillis() + collectorConfig.getNoDataKeepAliveInterval();
    }

    /** "callback" for when a new data sequence number can be sent to the transmitter.
     *   XXX synchronized ... is it correct? see other synchronized's and runLoop
     */
    synchronized public final void processDataSeqNoAck(long newDataSeqNoToAck)
    throws ConnectionIOException, ProtocolMalformedMsgException, ConfigException {
        // XXX

        // It is not clear that we have sufficient conditions here to
        // prevent a problem when a callback to here happens after
        // "isRunning" is set false (that is, race conditions.
        // Probably we should have a "mutex" which guarantees
        // behaviour in such cases (e.g., put a mutex on this callback
        // and also on the main read-process loop).

        // log.info(this, "processDataSeqNoAck",
        //          "isRunning=" + this.isRunning +
        //          ", anyDataSeen=" + this.anyDataRecordSeen +
        //          ", lastDSNAcked=" + this.lastDataSeqNoAcked +
        //          ", newDSNToAck=" + newDataSeqNoToAck +
        //          ", connection=" + this.connection);

        long now = System.currentTimeMillis();
        boolean doAckLogMsg = (now >= this.nextDataAckLogMsgTime);

        if (doAckLogMsg) {
            sendStatusReq("periodic update", "processDataSeqNoAck", false);
            this.nextDataAckLogMsgTime += this.dataAckLogInterval;
        }
        if (this.isRunning && this.connection != null && this.anyDataRecordSeen &&
            newDataSeqNoToAck != this.lastDataSeqNoAcked) {
            ProtocolMsgDATA_ACK msgACK =
                new ProtocolMsgDATA_ACK(this.sessionId, this.configId, newDataSeqNoToAck);
            if (doAckLogMsg || log.debug()) {
                log.info(this, "ProcessDataSeqNoAck", "Sending to " +
                         connection.addrAndPortToString() + ": " + msgACK);
            }
            msgACK.sendMsg(connection);
            this.lastDataSeqNoAcked = newDataSeqNoToAck;
            this.nextNoDataKeepAliveTime = now + collectorConfig.getNoDataKeepAliveInterval();
        }
        this.nextAckTime = now + collectorConfig.getAckInterval();
    }

    /** Output a single data record.
     * @param msgDATA The message with the data.
     * @throws ConfigException If there's something wrong with the configuration (e.g.,
     *                  the message specifies a template that isn't in the configuration).
     * @throws DataException If there's something wrong with the data.
     * @throws OutputterException Generalized exception from outputting.
     */
    private final void outputDataRecord(ProtocolMsgDATA msgDATA)
        throws DataException, ConfigException, OutputterException {

        // XXX Note that lookupTemplate gets a modified template (from
        // TMPL_DATA) that has the fields in the expected order, and
        // that can handle unexpected fields. It also doesn't have any
        // fields that have been disabled.

        SessionConfig sessionConfig = lookupSession(msgDATA.sessionId);
        TemplateConfig templateConfig = lookupTemplate(msgDATA.sessionId, msgDATA.templateId);
        ProtocolData converter = getSessionDataConverter(msgDATA.sessionId);

        // XXX The following test should be done outside the loop,
        // along with a few other tests here.

        if (outputter instanceof CollectorFastOutputter) {
            boolean isBigEndian;
            if (converter instanceof ProtocolDataBigEndian) {
                isBigEndian = true;
            } else if (converter instanceof ProtocolDataLittleEndian) {
                isBigEndian = false;
            } else {
                throw new LogicException(this, "outputDataReocrd",
                                         "converter neither big nor little endian: " +
                                         converter.getClass().getName());
            }
            ((CollectorFastOutputter)outputter).
                outputRecord(connectionConfig.getTransmitterAddress(),
                             connectionConfig.getTransmitterPort(),
                             connectionConfig.getTransmitterVendorString(),
                             this.exporterBootTime,
                             msgDATA,
                             isBigEndian,
                             sessionConfig,
                             templateConfig);
            return;
        }


        // XXX The following doesn't take into account configId and configuration changes

        outputter.startRecord(connectionConfig.getTransmitterAddress(),
                              connectionConfig.getTransmitterPort(),
                              connectionConfig.getTransmitterVendorString(),
                              this.exporterBootTime,
                              msgDATA.dataSeqNo,
                              msgDATA.sessionId,
                              msgDATA.templateId);

//        if(((RecordOutputter)outputter).isStreamPaused())
//        {
//          // paying for not having an ISM context on this object... the
//          // RecordOutputter handles the paused/resumed flag for me
//          return;
//        }

        int fieldIdListLength = templateConfig.getFieldListLength();
        int offset = msgDATA.rawDataOffset;

        // In the following, this.nullInput is for performance testing.
        for (int i = 0; ! this.nullInput && i < fieldIdListLength; i++) {
            // XXX setSessionTemplate has already swapped in updated templates
            FieldConfig field = templateConfig.getFieldAt(i);
            boolean isEnabledForOutput = field.isEnabledForOutput();
            FieldDescriptor fd = field.getFieldDesc();

            if (log.debug()) {
                log.debug(this, "outputDataRecord",
                          "field #" + i + " offset=" + offset +
                          ": " + fd);
            }

            switch (field.getTypeId()) {
            case ProtocolMsgKeyBlock.TYPE_BOOLEAN:
                if (isEnabledForOutput && fd != null) {
                    outputter.outputBoolean(i, field, fd, converter.getByte(msgDATA.rawData, offset));
                }
                offset += ProtocolData.byteSize;
                break;
            case ProtocolMsgKeyBlock.TYPE_UINT_8:
                if (isEnabledForOutput && fd != null) {
                    outputter.outputUnsignedInt8(i, field, fd, converter.getUnsignedByte(msgDATA.rawData, offset));
                }
                offset += ProtocolData.unsignedByteSize;
                break;
            case ProtocolMsgKeyBlock.TYPE_INT_8:
                if (isEnabledForOutput && fd != null) {
                    outputter.outputInt8(i, field, fd, converter.getByte(msgDATA.rawData, offset));
                }
                offset += ProtocolData.byteSize;
                break;
            case ProtocolMsgKeyBlock.TYPE_UINT_16:
                if (isEnabledForOutput && fd != null) {
                    outputter.outputUnsignedInt16(i, field, fd, converter.getUnsignedShort(msgDATA.rawData, offset));
                }
                offset += ProtocolData.unsignedShortSize;
                break;
            case ProtocolMsgKeyBlock.TYPE_INT_16:
                if (isEnabledForOutput && fd != null) {
                    outputter.outputInt16(i, field, fd, converter.getShort(msgDATA.rawData, offset));
                }
                offset += ProtocolData.shortSize;
                break;
            case ProtocolMsgKeyBlock.TYPE_UINT_32:
                if (isEnabledForOutput && fd != null) {
                    outputter.outputUnsignedInt32(i, field, fd, converter.getUnsignedInt(msgDATA.rawData, offset));
                }
                offset += ProtocolData.unsignedIntSize;
                break;
            case ProtocolMsgKeyBlock.TYPE_INT_32:
                if (isEnabledForOutput && fd != null) {
                    outputter.outputInt32(i, field, fd, converter.getInt(msgDATA.rawData, offset));
                }
                offset += ProtocolData.intSize;
                break;
            case ProtocolMsgKeyBlock.TYPE_UINT_64:
                // XXX Unsigned long doesn't exist in Java
                if (isEnabledForOutput && fd != null) {
                    outputter.outputUnsignedInt64(i, field, fd, converter.getLong(msgDATA.rawData, offset));
                }
                offset += ProtocolData.longSize;
                break;
            case ProtocolMsgKeyBlock.TYPE_INT_64:
                if (isEnabledForOutput && fd != null) {
                    outputter.outputInt64(i, field, fd, converter.getLong(msgDATA.rawData, offset));
                }
                offset += ProtocolData.longSize;
                break;
            case ProtocolMsgKeyBlock.TYPE_STRING: {
                int len = converter.getInt(msgDATA.rawData, offset);
                offset += ProtocolData.intSize;
                if (isEnabledForOutput && fd != null) {
                    outputter.outputString(i, field, fd, converter.getStringWithLen(msgDATA.rawData, len, offset));
                }
                offset += len;
            }
                break;
            case ProtocolMsgKeyBlock.TYPE_NT_STRING: {
                int strEnd = offset;
                while (msgDATA.rawData[strEnd++] != 0) { }
                int len = strEnd - offset - 1;
                if (isEnabledForOutput && fd != null) {
                    outputter.outputString(i, field, fd, msgDATA.rawData, offset, len);
                }
                offset = strEnd;
            }
                break;
            case ProtocolMsgKeyBlock.TYPE_IPV4_ADDR:
                if (isEnabledForOutput && fd != null) {
                    outputter.outputIPv4Addr(i, field, fd, (int)converter.getUnsignedInt(msgDATA.rawData, offset));
                }
                offset += ProtocolData.unsignedIntSize;
                break;
            case ProtocolMsgKeyBlock.TYPE_TIME_SEC:
                // XXX Check this conversion (should use date methods in ProtocolData?)
                if (isEnabledForOutput && fd != null) {
                    outputter.outputTimeSec(i, field, fd, converter.getUnsignedInt(msgDATA.rawData, offset));
                }
                offset += ProtocolData.unsignedIntSize;
                break;
            case ProtocolMsgKeyBlock.TYPE_TIME_MSEC:
                // XXX Unsigned long doesn't exist in Java
                // XXX Check this conversion (should use date methods in ProtocolData?)
                if (isEnabledForOutput && fd != null) {
                    outputter.outputTimeMillisec(i, field, fd, converter.getLong(msgDATA.rawData, offset));
                }
                offset += ProtocolData.longSize;
                break;
            case ProtocolMsgKeyBlock.TYPE_TIME_USEC:
                // XXX Unsigned long doesn't exist in Java
                // XXX Check this conversion (should use date methods in ProtocolData?)
                if (isEnabledForOutput && fd != null) {
                    outputter.outputTimeMicrosec(i, field, fd, converter.getLong(msgDATA.rawData, offset));
                }
                offset += ProtocolData.longSize;
                break;
            case ProtocolMsgKeyBlock.TYPE_TIME_MSEC_32: {
                // XXX Unsigned long doesn't exist in Java
                long value = ((long)converter.getUnsignedInt(msgDATA.rawData, offset)) * 1000 +
                    converter.getUnsignedInt(msgDATA.rawData, offset + ProtocolData.unsignedIntSize);
                if (isEnabledForOutput && fd != null) {
                    outputter.outputTimeMillisec(i, field, fd, converter.getLong(msgDATA.rawData, offset));
                }
                offset += (ProtocolData.unsignedIntSize + ProtocolData.unsignedIntSize);
            }
                break;
            case ProtocolMsgKeyBlock.TYPE_TIME_USEC_32: {
                // XXX Unsigned long doesn't exist in Java
                long value = ((long)converter.getUnsignedInt(msgDATA.rawData, offset)) * 1000000 +
                    converter.getUnsignedInt(msgDATA.rawData, offset + ProtocolData.unsignedIntSize);
                if (isEnabledForOutput && fd != null) {
                    outputter.outputTimeMicrosec(i, field, fd, converter.getLong(msgDATA.rawData, offset));
                }
                offset += (ProtocolData.unsignedIntSize + ProtocolData.unsignedIntSize);
            }
                break;
            case ProtocolMsgKeyBlock.TYPE_FLOAT: {
                int asBits = converter.getInt(msgDATA.rawData, offset);
                float value = Float.intBitsToFloat(asBits);
                if (isEnabledForOutput && fd != null) {
                    outputter.outputFloat(i, field, fd, value);
                }
                offset += ProtocolData.intSize;
            }
                break;
            case ProtocolMsgKeyBlock.TYPE_DOUBLE: {
                long asBits = converter.getLong(msgDATA.rawData, offset);
                double value = Double.longBitsToDouble(asBits);
                if (isEnabledForOutput && fd != null) {
                    outputter.outputDouble(i, field, fd, value);
                }
                offset += ProtocolData.longSize;
            }
              break;
//              case ProtocolMsgKeyBlock.TYPE_UTF8:
//                  break;
//              case ProtocolMsgKeyBlock.TYPE_UTF16:
//                  break;
//              case ProtocolMsgKeyBlock.TYPE_BLOB:
//                  break;
//              case ProtocolMsgKeyBlock.TYPE_IPV6_ADDR:
//                  break;
//              case ProtocolMsgKeyBlock.TYPE_TIME_MSEC_32:
//                  break;
//              case ProtocolMsgKeyBlock.TYPE_TIME_USSEC_32:
//                  break;
            default:
                throw new DataException(this, "outputDataRecord",
                                        "Unknown type: " + field.getTypeId());
            }
        }
        if(fieldIdListLength > 0) { // to avoid sending data on a template with no fields...
                                    // (which is an 'unexpected' template)
           outputter.endRecord();
        }
    }

    /** Stop this thread, as soon as possible. Interrupts the thread,
     *  so that if it's in the middle of an I/O you can get an
     *  exception (but this is necessary; otherwise we might wait
     *  forever).
     *
     */
    public void stopProcessing() {
        this.isRunning = false;
        this.interrupt(); // if we're in the middle of something, stop doing it!
    }

    /** Display the thread's status to the log.
     * @param log
     */
    synchronized public void logThreadStatus(LogHandler log) {
        log.info(this, "logThreadStatus",
                 "Connection thread " + this.toString() +
                 " " +
                 ((connection == null) ? "no connection" : connection.toString()) +
                 " anyData=" + anyDataRecordSeen +
                 " lastDSNacked=" + lastDataSeqNoAcked +
                 " lastDSN=" + lastDataSeqNo +
                 " next ACK in " + (nextAckTime - System.currentTimeMillis())/1000.0 + " sec");
    }


    /** Get the version information from the transmitter (waiting up to
     *  <code>connectionConfig.getConnectTimeout()</code> milliseconds for a response
     *  and resending every <code>connectionConfig.getConnectRetryTimeout())
     *
     *  XXX This does slightly wrong things with timeouts on UDP and
     *  TCP connections. For UDP, it ought to not try just once
     *  (current design), but instead try every 1000 msec until
     *  getConnectTimeout is reached (that is, if getConnectTimeout =
     *  5000, it will send 5 UPD packets before giving up). Note that
     *  it doesn't hurt to send extra UDP packets -- we're just
     *  looking for one response.
     *
     *  @param desiredTransportType The desired transport type (PROTOCOL_INFO_TRANSPORT_TCP
     *                       or PROTOCOL_INFO_TRANSPORT_SCTP)
     *  @param desiredProtocolVersion The protocol version that we can handle
     *  @return Port to be used for getting data from the client;
     *          connectionConfig.getTransmitterVersionPort() if
     *          nothing matched the transportType/protocolVersion.
     *  @exception traps all I/O exceptions and simply returns versionPort
     */

    private final int validateTransmitterProtocolVersion(int desiredTransportType,
                                                         int desiredProtocolVersion)
        throws ProtocolMalformedMsgException {
        int overrideTransmitterPort = connectionConfig.getTransmitterPort(); // set to default value
        ProtocolMsgInfoRequest msgInfoRequest = null;
        byte [] msgInfoRequestRaw = null;
        Exception exception = null;

        try {
            msgInfoRequest =
                new ProtocolMsgInfoRequest(connectionConfig.getTransmitterAddress().get(),
                                           new Date());
            msgInfoRequestRaw = msgInfoRequest.createRawMsg();
        } catch (UnknownHostException e) {
            exception = e;
        }

        boolean finished = (exception != null);
        int numberOfRetries =
            Math.max(1,connectionConfig.getConnectTimeout() /
                       connectionConfig.getConnectRetryTimeout());

        for (; numberOfRetries > 0 && ! finished; numberOfRetries--) {
            exception = null;
            try {
                log.debug(this, "validateTransmitterProtocolVersion",
                          "Sending UDP to " +
                          connectionConfig.transmitterAndVersionPortString() +
                          " (with timeout " + connectionConfig.getConnectRetryTimeout() +
                          "msec): " + msgInfoRequest);
                byte [] protocolInfo =
                    sendAndReceiveVersionDatagrams(msgInfoRequestRaw,
                                                   msgInfoRequestRaw.length);
                ProtocolMsgInfoResponse infoMsg =
                    new ProtocolMsgInfoResponse(protocolInfo, protocolInfo.length);
                log.info(this, "validateTransmitterProtocolVersion",
                         "Received from " +
                         connectionConfig.getTransmitterAddress().andPortToString(connectionConfig.getTransmitterVersionPort()) +
                         ": " + infoMsg);

                for (int i = 0; i < infoMsg.info.length; i++) {
                    if (infoMsg.info[i].transportType == desiredTransportType &&
                        infoMsg.info[i].protocolVersion == desiredProtocolVersion) {
                        overrideTransmitterPort = infoMsg.info[i].portNumber;
                        finished = true;
                    }
                }
                if (! finished) {
                    log.warn(this, "validateTransmitterProtocolVersion",
                             "Desired transport type(" + desiredTransportType + ")" +
                             " and protocol version(" + desiredProtocolVersion + ")" +
                             " not found in protocolInfo message from transmitter (client)" +
                             " ... using " + overrideTransmitterPort + " ... received message: " +
                             infoMsg);
                    finished = true; // No point in trying again: will get same answer.
                }
            } catch (java.io.InterruptedIOException e) {
                // XXX For JDK 1.4, use SocketTimeoutException
                exception = e;
                log.debug(this, "validateTransmitterProtocolVersion",
                          "I/O interrupted with timeout " +
                          connectionConfig.getConnectRetryTimeout() + "msec: " + e +
                          " after reading " + e.bytesTransferred + " bytes");
            } catch (java.io.IOException e) {
                exception = e;
                log.warn(this, "validateTransmitterProtocolVersion",
                         "I/O error " +
                         connectionConfig.getConnectTimeout() + "msec: " + e);
            }
        }
        if (exception != null) {
            log.warn(this, "validateTransmitterProtocolVersion",
                     "failed with retry timeout=" +
                     connectionConfig.getConnectRetryTimeout() + "msec, " +
                     "roundtrip timeout=" +
                     connectionConfig.getConnectRoundtripTimeout() + ", and " +
                     "overall timeout=" +
                     connectionConfig.getConnectTimeout() + "msec: " + exception +
                     " ... will use port " + overrideTransmitterPort);
        }
        return overrideTransmitterPort;
    }


    /** Send a datagram and wait for response, resending the datagram
     *  every so often.
     *  @param sendData Bytes to send.
     *  @param sendLength How many bytes to send.
     */
    private final byte [] sendAndReceiveVersionDatagrams(byte [] sendData,
                                                         int sendLength)
        throws java.io.IOException {
        int roundtripTimeout =
            Math.max(0,
                     Math.min(connectionConfig.getConnectRoundtripTimeout(),
                              connectionConfig.getConnectRetryTimeout()));

        int timesToTry = Math.max(1, connectionConfig.getConnectRetryTimeout() / roundtripTimeout);
        for (int i = 1; ;i++) {
            try {
                return sendAndReceiveVersionDatagram(roundtripTimeout,
                                                     sendData, sendLength);
            // XXX For JDK 1.4:
            // } catch (SocketTimeoutException exc) {
            } catch (java.io.InterruptedIOException exc) {
                if (i >= timesToTry) {
                    throw exc;
                }
            }
        }
    }

    /** Send a single datagram and wait for response.
     *  @param timeout How long to wait for a response: 0 or negative means forever.
     *  @param sendData Bytes to send.
     *  @param sendLength How many bytes to send.
     */
    private final byte [] sendAndReceiveVersionDatagram(int timeout,
                                                        byte [] sendData,
                                                        int sendLength)
        throws java.io.IOException {
        DatagramSocket socket = new DatagramSocket();
        try {
            DatagramPacket sendPacket, receivePacket;
            byte [] receiveBuf = new byte[0xffff+1]; // big enough for all possibilities

            timeout = Math.max(0, timeout);
            socket.setSoTimeout(timeout);
            sendPacket = new DatagramPacket(sendData, sendData.length,
                                            connectionConfig.getTransmitterAddress().get(),
                                            connectionConfig.getTransmitterVersionPort());
            receivePacket = new DatagramPacket(receiveBuf, receiveBuf.length);

            socket.send(sendPacket);
            socket.receive(receivePacket);

            // If we get here, we've received a packet: a timeout would
            // throw IOException.

            int receivedLength = receivePacket.getLength();

            byte [] result = new byte[receivedLength];
            System.arraycopy(receivePacket.getData(), receivePacket.getOffset(),
                             result, 0, receivedLength);

            socket.close();
            socket = null; // make really sure it's gone (probably not needed)

            return result;
        } finally {
            if (socket != null) {
                socket.close();
                socket = null; // and make sure
            }
        }
    }

//    public class MyTimer extends Thread
//  {
//      RecordOutputter m_ro;
//      public MyTimer(RecordOutputter ro)
//      {
//          m_ro = ro;
//      }
//      public void run()
//      {
//          try
//          {
//              while(true)
//              {
//                  Thread.sleep(4000);
//                  m_ro.processDataSeqNoAck(lastSeqNo);
//              }
//          }
//          catch(Exception e)
//          {
//
//          }
//      }
//
//  }

//     private final String testUDP(ExtendedInetAddress transmitter) {
//         // Call DAYTIME
//         final int DAYTIME_PORT = 13;

//         String dummyMsg = "Requesting date/time at " +
//             CollectorUtil.dateFormat(new Date());

//         byte [] dummyData = dummyMsg.getBytes();
//         try {
//             return new String(sendAndReceiveDatagram(transmitter, DAYTIME_PORT,
//                                                      connectionConfig.getConnectTimeout(),
//                                                      dummyData, dummyData.length));
//         } catch (IOException e) {
//             return "Error from daytime: " + e;
//         }
//     }
}
