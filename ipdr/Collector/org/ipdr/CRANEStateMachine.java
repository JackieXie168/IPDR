package org.ipdr;



//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/**
 */
public class CRANEStateMachine //extends ProtocolStateMachine
{
//    protected volatile boolean isRunning;
//
//    /// The collector configuration
//    protected CollectorConfigI collectorConfig;
//
//    /// The connection from configuration
//    protected ConnectionConfigI connectionConfig;
//
//    /// The "boot time" of the data exporter
//    private Date exporterBootTime = null;
//
//    /// The general outputting class
//    private Outputter outputter;
//
//    private byte sessionId = -1; //    ditto
//
//    /** A flag for turning of as much input processing as possible
//     *  (useful for performance testing).
//     */
//    private boolean nullInput;
//    public void setNullInput() { this.nullInput = true; }
//    private long nextAckTime = 0; // set in receiveDataAndOutputIt and reset int
//    /** When to output the next data-ack message to log
//     *  @see ConnectionConfig#dataAckLogInterval
//     */
//    private static long nextDataAckLogMsgTime = System.currentTimeMillis();
//
//    // processDataSeqNoAck
//
//    private long nextNoDataKeepAliveTime = 0; // set similar to nextAckTime
//
//    private boolean anyDataRecordSeen = false;
//
//    long lastDataSeqNo = -1; // meaningless until this.anyDataRecordSeen
//
//    private long lastDataSeqNoAcked = -1; // meaningless unless
//
//    // anyDataRecordSeen
//
//    private byte configId = -1; //    ditto
//
//    /**
//     * @see org.ipdr.ProtocolStateMachine#startProtocol()
//     */
//    public void startProtocol() throws ConnectionException, ConfigException,
//            ProtocolMalformedMsgException
//    {
//        // TODO Auto-generated method stub
//
//    }
//
//    /**
//     * @see org.ipdr.ProtocolStateMachine#connectTimeout()
//     */
//    public void connectTimeout()
//    {
//        // TODO Auto-generated method stub
//
//    }
//
//    /**
//     * @see org.ipdr.ProtocolStateMachine#dataAckTimeout(byte)
//     */
//    public void dataAckTimeout(byte sessionId)
//    {
//        // TODO Auto-generated method stub
//
//    }
//
//    /**
//     * @see org.ipdr.ProtocolStateMachine#canExit()
//     */
//    public boolean canExit()
//    {
//        // TODO Auto-generated method stub
//        return false;
//    }
//
//    /**
//     * @see org.ipdr.ProtocolStateMachine#initiateDisconnection(short, short,
//     *      java.lang.String)
//     */
//    public void initiateDisconnection(short sessionReasonCode,
//            short flowReasonCode, String reasonInfo) throws ConfigException,
//            ProtocolMalformedMsgException
//    {
//        // TODO Auto-generated method stub
//
//    }
//
//    /**
//     * @see org.ipdr.ProtocolStateMachine#cleanup()
//     */
//    public void cleanup()
//    {
//        // TODO Auto-generated method stub
//
//    }
//
//    /*
//     * (non-Javadoc)
//     * 
//     * @see org.ipdr.ProtocolStateMachine#mainLoop()
//     */
//    public void mainLoop() throws InterruptedException, ConfigException
//    {
//        this.exporterBootTime = null; // generate an error if it's used
//
//        while (isRunning)
//        {
//            try
//            {
//                // if (false) { // Used for testing UDP send/receive with
//                // "daytime" server
//                //     log.print("daytime from " +
//                // connectionConfig.transmitterAddress + "(" +
//                //               connectionConfig.getConnectTimeout() + "ms): " +
//                //               testUDP(connectionConfig.transmitterAddress));
//                // }
//
//                runLoopConnection();
//
//                if (isRunning)
//                {
//                    sendStatusReq("start-up", "runLoop", false);
//                    receiveDataAndOutputIt();
//                }
//
//            }
//            catch (ConnectionIOException e)
//            {
//                LogManager.error(this, "runLoop",
//                        "error with transmitter (client) "
//                                + connectionConfig.getExporterAndPortString()
//                                + ": " + e.getOriginalMsg()
//                                + " ... caused by: " + e.getCause());
//            }
//
//            closeConnection();
//
//            if (isRunning)
//            {
//                sleepReconnect();
//            }
//        }
//    }
//
//    protected void sendStatusReq(String msg, String function, boolean warning)
//            throws ConnectionIOException, ProtocolMalformedMsgException,
//            ConfigException
//    {
//        ProtocolMsgSTATUS_REQ req = new ProtocolMsgSTATUS_REQ(this.sessionId);
//        String fullMsg = "Sending status request [" + msg + "] (msg.Id="
//                + req.msgId() + ") to transmitter on " + connection;
//        if (warning)
//        {
//            LogManager.warn(this, function, fullMsg);
//        }
//        else
//        {
//            LogManager.info(this, function, fullMsg);
//        }
//        req.sendMsg(this.connection);
//    }
//
//    synchronized protected final void runLoopConnection()
//            throws ConnectionIOException, ProtocolMalformedMsgException,
//            ConfigException, DataException
//    {
//
//        makeConnection();
//
//        if (isRunning)
//        {
//            for (int i = 0; isRunning
//                    && i < connectionConfig.getSessionListLength(); i++)
//            {
//                sendStartAndValidateTemplates(connectionConfig
//                        .getSessionConfigAt(i));
//                // collectorConfig.validateTemplatesFromTransmitter(templateConfigList);
//            }
//        }
//    }
//
//    protected final void makeConnection() throws ConnectionIOException,
//            ProtocolMalformedMsgException, ConfigException
//    {
//
//        connectionConfig.setTransmitterPort(validateTransmitterProtocolVersion(
//                ProtocolMsgInfoRequest.INFO_TRANSPORT_TCP,
//                ProtocolMsg.PROTOCOL_VERSION));
//
//        LogManager.debug(this, "setTransmittedPort",
//                "Transmitter (client) port="
//                        + connectionConfig.getExporterPort());
//
//        this.connection = new ConnectionDynamic(connectionConfig);
//
//        ProtocolMsgCONNECT msg = new ProtocolMsgCONNECT((byte) 0,
//                collectorConfig.getCollectorAddress(), collectorConfig
//                        .getLocalPort());
//        msg.sendMsg(connection);
//
//        // There is no response in the CRANE protocol to a CONNECT
//        // ... it's just used to inform the transmitter that we're here
//        // ... next thing is ask for the templates or do a START.
//    }
//
//    private final int validateTransmitterProtocolVersion(
//            int desiredTransportType, int desiredProtocolVersion)
//            throws ProtocolMalformedMsgException
//    {
//        int overrideTransmitterPort = connectionConfig.getExporterPort(); // set
//        // to
//        // default
//        // value
//        ProtocolMsgInfoRequest msgInfoRequest = null;
//        byte[] msgInfoRequestRaw = null;
//        Exception exception = null;
//
//        try
//        {
//            msgInfoRequest = new ProtocolMsgInfoRequest(connectionConfig
//                    .getExporterAddress(), new Date());
//            msgInfoRequestRaw = msgInfoRequest.createRawMsg();
//        }
//        catch (UnknownHostException e)
//        {
//            exception = e;
//        }
//
//        boolean finished = (exception != null);
//        int numberOfRetries = (connectionConfig.getRetryCount() == ConnectionConfigI.INFINITE ? Integer.MAX_VALUE
//                : connectionConfig.getRetryCount());
//
//        for (; numberOfRetries > 0 && !finished; numberOfRetries--)
//        {
//            exception = null;
//            try
//            {
//                LogManager.debug(this, "validateTransmitterProtocolVersion",
//                        "Sending UDP to "
//                                + connectionConfig.getExporterAddress()
//                                        .toString() + " (with timeout "
//                                + connectionConfig.getRetryInterval()
//                                + "msec): " + msgInfoRequest);
//                byte[] protocolInfo = sendAndReceiveVersionDatagrams(
//                        msgInfoRequestRaw, msgInfoRequestRaw.length);
//                ProtocolMsgInfoResponse infoMsg = new ProtocolMsgInfoResponse(
//                        protocolInfo, protocolInfo.length);
//                LogManager
//                        .info(this, "validateTransmitterProtocolVersion",
//                                "Received from "
//                                        + connectionConfig.getExporterAddress()
//                                        + ":"
//                                        + connectionConfig.getCollectorConfig()
//                                                .getUDPDiscoveryPort() + ": "
//                                        + infoMsg);
//
//                for (int i = 0; i < infoMsg.info.length; i++)
//                {
//                    if (infoMsg.info[i].transportType == desiredTransportType
//                            && infoMsg.info[i].protocolVersion == desiredProtocolVersion)
//                    {
//                        overrideTransmitterPort = infoMsg.info[i].portNumber;
//                        finished = true;
//                    }
//                }
//                if (!finished)
//                {
//                    LogManager
//                            .warn(
//                                    this,
//                                    "validateTransmitterProtocolVersion",
//                                    "Desired transport type("
//                                            + desiredTransportType
//                                            + ")"
//                                            + " and protocol version("
//                                            + desiredProtocolVersion
//                                            + ")"
//                                            + " not found in protocolInfo message from transmitter (client)"
//                                            + " ... using "
//                                            + overrideTransmitterPort
//                                            + " ... received message: "
//                                            + infoMsg);
//                    finished = true; // No point in trying again: will get same
//                    // answer.
//                }
//            }
//            catch (java.io.InterruptedIOException e)
//            {
//                // XXX For JDK 1.4, use SocketTimeoutException
//                exception = e;
//                LogManager.debug(this, "validateTransmitterProtocolVersion",
//                        "I/O interrupted with timeout "
//                                + connectionConfig.getRetryInterval()
//                                + "msec: " + e + " after reading "
//                                + e.bytesTransferred + " bytes");
//            }
//            catch (java.io.IOException e)
//            {
//                exception = e;
//                LogManager.warn(this, "validateTransmitterProtocolVersion",
//                        "I/O error " + connectionConfig.getConnectTimeout()
//                                + "msec: " + e);
//            }
//        }
//        if (exception != null)
//        {
//            LogManager.warn(this, "validateTransmitterProtocolVersion",
//                    "failed with retry timeout="
//                            + connectionConfig.getRetryInterval() + "msec, "
//                            + ", and " + "overall timeout="
//                            + connectionConfig.getConnectTimeout() + "msec: "
//                            + exception + " ... will use port "
//                            + overrideTransmitterPort);
//        }
//        return overrideTransmitterPort;
//    }
//
//    /**
//     * Receive data and output it. This also starts the acknowledgment timer
//     * thread.
//     * 
//     * @param connection
//     */
//    private final void receiveDataAndOutputIt() throws ConnectionIOException,
//            ProtocolMalformedMsgException, DataException, ConfigException,
//            OutputterException
//    {
//
//        //        outputter.registerProcessDataSeqNoAck(this);
//
//        initSeqNoData();
//
//        try
//        {
//            // By defining the buffer outside readMsg, we get about 2x
//            // speed-up ... but we must be sure to not do another
//            // readMsg with the same buffer while we're still using
//            // the "msg" that we read (things might get corrupted, at
//            // least with ProtocolMsgDATA).
//
//            byte[] readMsgBuf = new byte[0x8000]; // should be big enough
//            while (isRunning)
//            {
//                long now = System.currentTimeMillis();
//                int timeToWait = (int) (nextAckTime - now);
//                // XXX Need to avoid multiple zero-wait responses in
//                // the case of falling severely behind in processing.
//                // see below where nextAckTime is set ... it might not be
//                // sufficient.
//                // log.debug(this, "receiveDataAndOutputIt",
//                // "timeToWait="+timeToWait);
//
//                ProtocolMsg msg;
//                if (timeToWait > 0
//                        && timeToWait > collectorConfig.getAckIntervalEpsilon())
//                {
//                    msg = ProtocolMsg.readMsg(readMsgBuf, connection, "data",
//                            null, timeToWait);
//                }
//                else
//                {
//                    msg = new ProtocolMsgEmpty();
//                }
//                if (LogManager.debug())
//                {
//                    LogManager.debug(this, "receiveDataAndOutputIt",
//                            "data record: " + msg.toStringLong());
//                }
//
//                now = System.currentTimeMillis();
//                if (msg instanceof ProtocolMsgDATA)
//                {
//                    handleSingleDATA((ProtocolMsgDATA) msg);
//                    nextNoDataKeepAliveTime = now
//                            + collectorConfig.getNoDataKeepAliveInterval();
//                }
//                else
//                    if (msg instanceof ProtocolMsgEmpty)
//                    {
//                        // Nothing was read: a timeout or other interruption.
//
//                        outputter.commitTimerExpired();
//
//                        ProtocolMsgEmpty msgEmpty = (ProtocolMsgEmpty) msg;
//                        Exception e = msgEmpty.getException();
//
//                        // XXX For JDK 1.4, we can check that we have an
//                        // instance of InterruptedIOException ...
//                        // for 1.3, suppress unnecessary messages by doing
//                        // a comparison with a magic string
//                        boolean showWhatHappened = false;
//                        if (e == null)
//                        { // we generated this ourselves
//                            showWhatHappened = false;
//                        }
//                        else
//                        {
//                            showWhatHappened = true;
//                        }
//                        if (LogManager.debug())
//                        {
//                            showWhatHappened = true;
//                        }
//
//                        if (showWhatHappened)
//                        {
//                            LogManager.debug(this,
//                                    "receiveDataAndOutputItLoop",
//                                    "Interrupt (timeout?) on input socket: "
//                                            + msgEmpty.getException());
//                        }
//
//                        // note that processDataSeqNoAck() will set the
//                        // next timers (in case it's called from Outputter).
//
//                        if (this.anyDataRecordSeen
//                                && outputter.isValidLastSeqNoToAck())
//                        {
//                            processDataSeqNoAck(outputter.getLastSeqNoToAck());
//                        }
//                        if (nextAckTime <= (now + collectorConfig
//                                .getAckIntervalEpsilon()))
//                        {
//                            // try to avoid many zero-time acks if we fall far
//                            // behind.
//                            nextAckTime = now
//                                    + collectorConfig.getAckInterval() / 2;
//                        }
//
//                        if (nextNoDataKeepAliveTime <= now)
//                        {
//                            // XXX send a "keep-alive" status query
//                            sendStatusReq("no data from transmitter",
//                                    "receiveDataAndOutputIt", true);
//                            nextNoDataKeepAliveTime = now
//                                    + collectorConfig
//                                            .getNoDataKeepAliveInterval();
//                            LogManager
//                                    .debug(
//                                            this,
//                                            "processDataSeqNoAck",
//                                            "Next keep-alive: "
//                                                    + IPDRUtilities
//                                                            .dateFormatMillisec(nextNoDataKeepAliveTime));
//                        }
//                    }
//                    else
//                        if (msg instanceof ProtocolMsgTMPL_DATA)
//                        {
//                            // NB: ProtcolMsgFINAL_TMPL_DATA is subclass of
//                            // ProtocolMsgTMPL_DATA, so it is also handled
//                            // here.
//                            handleTMPL_DATA((ProtocolMsgTMPL_DATA) msg);
//                        }
//                        else
//                            if (msg instanceof ProtocolMsgERROR)
//                            {
//                                handleERROR((ProtocolMsgERROR) msg);
//                            }
//                            else
//                            {
//                                throw new DataException(this,
//                                        "receiveDataAndOutputIt",
//                                        "Unexpected message: "
//                                                + msg.toStringLong());
//                            }
//            }
//        }
//        //Alex
//        catch (Exception e)
//        {
//            e.printStackTrace();
//        }
//        //Alex
//        finally
//        {
//            outputter.registerProcessDataSeqNoAck(null);
//        }
//    }
//
//    /*
//     * (non-Javadoc)
//     * 
//     * @see org.ipdr.ProtocolStateMachine#getSessionList()
//     */
//    public ArrayList getSessionList()
//    {
//        // TODO Auto-generated method stub
//    }
//
//    /*
//     * (non-Javadoc)
//     * 
//     * @see org.ipdr.ProtocolStateMachine#getSessionState(byte)
//     */
//    public int getSessionState(byte sessionId)
//            throws InvalidSessionStateException
//    {
//        // TODO Auto-generated method stub
//    }
//
//    private void initSeqNoData()
//    {
//        anyDataRecordSeen = false;
//        lastDataSeqNo = -1;
//        lastDataSeqNoAcked = -1;
//        sessionId = -1;
//        configId = -1;
//        nextAckTime = System.currentTimeMillis()
//                + collectorConfig.getAckInterval();
//        nextNoDataKeepAliveTime = System.currentTimeMillis()
//                + collectorConfig.getNoDataKeepAliveInterval();
//    }
//
//    synchronized protected final void handleSingleDATA(ProtocolMsgDATA msgDATA)
//            throws ConnectionIOException, ProtocolMalformedMsgException,
//            ConfigException, DataException, OutputterException
//    {
//
//        try
//        {
//            this.lastDataSeqNo = msgDATA.processDataSeqNo(lastDataSeqNo,
//                    this.anyDataRecordSeen);
//        }
//        catch (DataException e)
//        {
//            // try to force things out as much as possible before quitting
//            LogManager.info(this, "receiveDataAndOutputIt",
//                    "Got exception, will try to DATA ACK before restarting. Exception: "
//                            + e);
//            LogManager
//                    .debug(
//                            this,
//                            "receiveDataAndOutputIt",
//                            "anyDataRecordSeen="
//                                    + this.anyDataRecordSeen
//                                    + ", isRunning="
//                                    + this.isRunning
//                                    + ", lastDataSeqNoAcked="
//                                    + this.lastDataSeqNoAcked
//                                    + (outputter.isValidLastSeqNoToAck() ? ", outputter.seqNoToAck="
//                                            + outputter.getLastSeqNoToAck()
//                                            : ", outputter.notValidSeqNoToAck")
//                                    + ", connection=" + this.connection);
//            this.nextDataAckLogMsgTime = System.currentTimeMillis();
//            outputter.commitTimerExpired();
//            if (outputter.isValidLastSeqNoToAck())
//            {
//                processDataSeqNoAck(outputter.getLastSeqNoToAck());
//            }
//            // and give up
//            try
//            {
//                LogManager.info(this, "receiveDataAndOutputIt",
//                        "sleeping before closing connection");
//                // SoLinger in ConnectionDynamic doesn't seem to work properly.
//                sleep(collectorConfig.getAckInterval()); // XXX should be a
//                // separate parameter
//            }
//            catch (InterruptedException e2)
//            {
//            }
//            throw e;
//        }
//
//        // XXX Must confirm that seq # is on a
//        // connection basis, not a session basis.
//
//        if (this.anyDataRecordSeen)
//        {
//            if (this.sessionId != (byte) msgDATA.sessionId
//                    || this.configId != (byte) msgDATA.configId)
//            {
//                throw new DataException(this, "receiveDataAndOutputIt",
//                        "Unknown sessionId=" + msgDATA.sessionId + " configId="
//                                + msgDATA.configId + " (expecting sessionId="
//                                + this.sessionId + " configId=" + this.configId
//                                + ")");
//            }
//        }
//        outputDataRecord(msgDATA);
//        this.sessionId = (byte) msgDATA.sessionId;
//        this.configId = (byte) msgDATA.configId;
//        this.anyDataRecordSeen = true;
//    }
//
//    synchronized protected final void closeConnection()
//            throws ConnectionIOException
//    {
//        if (this.connection != null)
//        {
//            this.connection.close();
//            this.connection = null; // in case of "out of the blue"
//                                    // processDataSeqNoAck
//        }
//    }
//
//    private final void sleepReconnect()
//    {
//        try
//        {
//            LogManager.debug(this, "runLoop",
//                    "Sleeping before reconnect attempt on "
//                            + connectionConfig.getExporterAndPortString()
//                            + " (" + connectionConfig.getRetryInterval()
//                            + "msec)");
//            Thread.sleep(connectionConfig.getRetryInterval());
//        }
//        catch (InterruptedException e)
//        {
//            // No need to do anything ... interruption is typically
//            // from the stopProcessing() method, which also sets
//            // isRunning to false, so the loop will end.
//        }
//    }
//
//    // Side effect: sets exporterBootTime
//
//    private final void sendStartAndValidateTemplates(
//            SessionConfigI sessionConfig) throws ConnectionIOException,
//            ProtocolMalformedMsgException, DataException, ConfigException
//    {
//        ProtocolMsgSTART msgSTART = new ProtocolMsgSTART((byte) sessionConfig
//                .getSessionId());
//        ProtocolMsg responseMsg = null;
//        try
//        {
//            msgSTART.sendMsg(connection);
//            // XXX Is indefinite wait the correct thing here? What if the
//            // connection dies ...
//            // we should restart, I think. Need a separate timeout value from
//            // config, though.
//            responseMsg = ProtocolMsg.readMsg(connection, "response to",
//                    msgSTART, 0);
//        }
//        catch (ConnectionIOException e)
//        {
//            // if (e.getCause() instanceof ConnectException) {
//            throw new ConnectionIOException(e.getOriginalMsg()
//                    + " - possibly because not registered at transmitter?", e
//                    .getCause());
//        }
//
//        if (responseMsg instanceof ProtocolMsgSTART_ACK)
//        {
//            this.exporterBootTime = ((ProtocolMsgSTART_ACK) responseMsg)
//                    .getBootTime();
//            // Next should be the TMPL DATA message
//            // XXX Is indefinite wait the correct thing here? What if the
//            // connection dies ...
//            // we should restart, I think. Need a separate timeout value from
//            // config, though.
//            responseMsg = ProtocolMsg.readMsg(connection,
//                    "(TEMPL DATA) response to", msgSTART, 0);
//        }
//        else
//        {
//            LogManager.warn(this, "sendStartAndValidateTemplates",
//                    "Did not get START ACK from transmitter (client)");
//        }
//
//        if (responseMsg instanceof ProtocolMsgTMPL_DATA)
//        {
//            LogManager.info(this, "sendStartAndValidateTemplates",
//                    "After START/ACK: " + responseMsg.toStringLong());
//            handleTMPL_DATA((ProtocolMsgTMPL_DATA) responseMsg);
//        }
//        else
//        {
//            throw new DataException(this, "sendStartAndValidateTemplates",
//                    "Expected TMPL DATA from request message " + msgSTART
//                            + " but received: " + responseMsg);
//        }
//
//    }
//
//    /**
//     * Send a datagram and wait for response, resending the datagram every so
//     * often.
//     * 
//     * @param sendData
//     *            Bytes to send.
//     * @param sendLength
//     *            How many bytes to send.
//     */
//    private final byte[] sendAndReceiveVersionDatagrams(byte[] sendData,
//            int sendLength) throws java.io.IOException
//    {
//        int roundtripTimeout = Math.max(0, Math.min(connectionConfig
//                .getConnectTimeout(), connectionConfig
//                .getRetryInterval()));
//
//        int timesToTry = Math.max(1, connectionConfig.getRetryInterval()
//                / roundtripTimeout);
//        for (int i = 1;; i++)
//        {
//            try
//            {
//                return sendAndReceiveVersionDatagram(roundtripTimeout,
//                        sendData, sendLength);
//                // XXX For JDK 1.4:
//                // } catch (SocketTimeoutException exc) {
//            }
//            catch (java.io.InterruptedIOException exc)
//            {
//                if (i >= timesToTry)
//                {
//                    throw exc;
//                }
//            }
//        }
//    }
//
//    private final void handleTMPL_DATA(ProtocolMsgTMPL_DATA tmplResponse)
//            throws ConfigException, ProtocolMalformedMsgException,
//            ConnectionIOException
//    {
//
//        //verifying that the requested session was retrieved
//        SessionConfig sessionConfig = getRuntimeSession(connectionConfig,
//                tmplResponse.sessionId);
//        if (sessionConfig == null)
//        { //this shouldn't happen
//            LogManager
//                    .error(
//                            this,
//                            "handleTMPL_DATA",
//                            "configuration error, the retrieved session does not match the requested session");
//            return;
//        }
//        setSessionBigEndian(sessionConfig.getId(), tmplResponse.bigEndian);
//
//        // setting the session's runtime templates
//        for (int i = 0; i < tmplResponse.templateBlock
//                .getTemplateConfigListLength(); i++)
//        {
//            TemplateConfig tmplTransmitter = tmplResponse.templateBlock
//                    .getTemplateConfigAt(i);
//            setSessionTemplate(connectionConfig, sessionConfig, tmplTransmitter);
//        }
//
//        prepareForRunning(); // XXX needed because a template might have changed
//
//        // XXX Need to store "enabledForInput" (but see comments in
//        // outputDataRecord)
//
//        // XXX need to do this properly ...
//        //  if (! tmpl.transmitsTheSameAs(tmplTransmitter)) {
//        //       // XXX We should enter template negotiation at this point
//        //       // but for now, treat as an error
//        //       throw new ConfigException(this, "sendStartAndValidateTemplates",
//        //                                 "Mis-match of transmission form between transmitter (client)
//        // template: " +
//        //                                 tmplTransmitter.toStringLong() + " and configuration template: " +
//        //                                 tmpl.toStringLong());
//        //  }
//
//        ProtocolMsgFINAL_TMPL_DATA_ACK msgFINAL_TMPL_DATA_ACK = new ProtocolMsgFINAL_TMPL_DATA_ACK(
//                (byte) sessionConfig.getId(), tmplResponse.configId);
//        msgFINAL_TMPL_DATA_ACK.sendMsg(connection);
//    }
//    private final void handleERROR(ProtocolMsgERROR msg) {
//        LogManager.error(this, "handleERROR",
//                  "Received ERROR message from transmitter (client): " +
//                  msg.toStringLong());
//    }
//
//    /** Output a single data record.
//     * @param msgDATA The message with the data.
//     * @throws ConfigException If there's something wrong with the configuration (e.g.,
//     *                  the message specifies a template that isn't in the configuration).
//     * @throws DataException If there's something wrong with the data.
//     * @throws OutputterException Generalized exception from outputting.
//     */
//    private final void outputDataRecord(ProtocolMsgDATA msgDATA)
//        throws DataException, ConfigException, OutputterException {
//
//        // XXX Note that lookupTemplate gets a modified template (from
//        // TMPL_DATA) that has the fields in the expected order, and
//        // that can handle unexpected fields. It also doesn't have any
//        // fields that have been disabled.
//
//        SessionConfig sessionConfig = lookupSession(msgDATA.sessionId);
//        TemplateConfig templateConfig = lookupTemplate(msgDATA.sessionId, msgDATA.templateId);
//        ProtocolData converter = getSessionDataConverter(msgDATA.sessionId);
//
//        // XXX The following doesn't take into account configId and configuration changes
//
//        outputter.startRecord(connectionConfig.getExporterAddress(),
//                              connectionConfig.getExporterPort(),
////                              connectionConfig(),
//                              this.exporterBootTime,
//                              msgDATA.dataSeqNo,
//                              msgDATA.sessionId,
//                              msgDATA.templateId);
//
//
//        int fieldIdListLength = templateConfig.getFieldListLength();
//        int offset = msgDATA.rawDataOffset;
//
//        // In the following, this.nullInput is for performance testing.
//        for (int i = 0; ! this.nullInput && i < fieldIdListLength; i++) {
//            // XXX setSessionTemplate has already swapped in updated templates
//            FieldConfigI field = templateConfig.getFieldConfigAt(i);
//            boolean isEnabledForOutput = field.isEnabledForOutput();
//
//            if (LogManager.debug()) {
//                LogManager.debug(this, "outputDataRecord",
//                          "field #" + i + " offset=" + offset +
//                          ": ");
//            }
//
//            switch (field.getTypeId()) {
//            case ProtocolMsgKeyBlock.TYPE_BOOLEAN:
//                if (isEnabledForOutput ) {
//                    outputter.outputBoolean(i, field, converter.getByte(msgDATA.rawData, offset));
//                }
//                offset += ProtocolData.byteSize;
//                break;
//            case ProtocolMsgKeyBlock.TYPE_UINT_8:
//                if (isEnabledForOutput ) {
//                    outputter.outputUnsignedInt8(i, field, converter.getUnsignedByte(msgDATA.rawData, offset));
//                }
//                offset += ProtocolData.unsignedByteSize;
//                break;
//            case ProtocolMsgKeyBlock.TYPE_INT_8:
//                if (isEnabledForOutput) {
//                    outputter.outputInt8(i, field, converter.getByte(msgDATA.rawData, offset));
//                }
//                offset += ProtocolData.byteSize;
//                break;
//            case ProtocolMsgKeyBlock.TYPE_UINT_16:
//                if (isEnabledForOutput) {
//                    outputter.outputUnsignedInt16(i, field, converter.getUnsignedShort(msgDATA.rawData, offset));
//                }
//                offset += ProtocolData.unsignedShortSize;
//                break;
//            case ProtocolMsgKeyBlock.TYPE_INT_16:
//                if (isEnabledForOutput) {
//                    outputter.outputInt16(i, field, converter.getShort(msgDATA.rawData, offset));
//                }
//                offset += ProtocolData.shortSize;
//                break;
//            case ProtocolMsgKeyBlock.TYPE_UINT_32:
//                if (isEnabledForOutput && fd != null) {
//                    outputter.outputUnsignedInt32(i, field, fd, converter.getUnsignedInt(msgDATA.rawData, offset));
//                }
//                offset += ProtocolData.unsignedIntSize;
//                break;
//            case ProtocolMsgKeyBlock.TYPE_INT_32:
//                if (isEnabledForOutput && fd != null) {
//                    outputter.outputInt32(i, field, fd, converter.getInt(msgDATA.rawData, offset));
//                }
//                offset += ProtocolData.intSize;
//                break;
//            case ProtocolMsgKeyBlock.TYPE_UINT_64:
//                // XXX Unsigned long doesn't exist in Java
//                if (isEnabledForOutput && fd != null) {
//                    outputter.outputUnsignedInt64(i, field, fd, converter.getLong(msgDATA.rawData, offset));
//                }
//                offset += ProtocolData.longSize;
//                break;
//            case ProtocolMsgKeyBlock.TYPE_INT_64:
//                if (isEnabledForOutput && fd != null) {
//                    outputter.outputInt64(i, field, fd, converter.getLong(msgDATA.rawData, offset));
//                }
//                offset += ProtocolData.longSize;
//                break;
//            case ProtocolMsgKeyBlock.TYPE_STRING: {
//                int len = converter.getInt(msgDATA.rawData, offset);
//                offset += ProtocolData.intSize;
//                if (isEnabledForOutput && fd != null) {
//                    outputter.outputString(i, field, fd, converter.getStringWithLen(msgDATA.rawData, len, offset));
//                }
//                offset += len;
//            }
//                break;
//            case ProtocolMsgKeyBlock.TYPE_NT_STRING: {
//                int strEnd = offset;
//                while (msgDATA.rawData[strEnd++] != 0) { }
//                int len = strEnd - offset - 1;
//                if (isEnabledForOutput && fd != null) {
//                    outputter.outputString(i, field, fd, msgDATA.rawData, offset, len);
//                }
//                offset = strEnd;
//            }
//                break;
//            case ProtocolMsgKeyBlock.TYPE_IPV4_ADDR:
//                if (isEnabledForOutput && fd != null) {
//                    outputter.outputIPv4Addr(i, field, fd, (int)converter.getUnsignedInt(msgDATA.rawData, offset));
//                }
//                offset += ProtocolData.unsignedIntSize;
//                break;
//            case ProtocolMsgKeyBlock.TYPE_TIME_SEC:
//                // XXX Check this conversion (should use date methods in ProtocolData?)
//                if (isEnabledForOutput && fd != null) {
//                    outputter.outputTimeSec(i, field, fd, converter.getUnsignedInt(msgDATA.rawData, offset));
//                }
//                offset += ProtocolData.unsignedIntSize;
//                break;
//            case ProtocolMsgKeyBlock.TYPE_TIME_MSEC:
//                // XXX Unsigned long doesn't exist in Java
//                // XXX Check this conversion (should use date methods in ProtocolData?)
//                if (isEnabledForOutput && fd != null) {
//                    outputter.outputTimeMillisec(i, field, fd, converter.getLong(msgDATA.rawData, offset));
//                }
//                offset += ProtocolData.longSize;
//                break;
//            case ProtocolMsgKeyBlock.TYPE_TIME_USEC:
//                // XXX Unsigned long doesn't exist in Java
//                // XXX Check this conversion (should use date methods in ProtocolData?)
//                if (isEnabledForOutput && fd != null) {
//                    outputter.outputTimeMicrosec(i, field, fd, converter.getLong(msgDATA.rawData, offset));
//                }
//                offset += ProtocolData.longSize;
//                break;
//            case ProtocolMsgKeyBlock.TYPE_TIME_MSEC_32: {
//                // XXX Unsigned long doesn't exist in Java
//                long value = ((long)converter.getUnsignedInt(msgDATA.rawData, offset)) * 1000 +
//                    converter.getUnsignedInt(msgDATA.rawData, offset + ProtocolData.unsignedIntSize);
//                if (isEnabledForOutput ) {
//                    outputter.outputTimeMillisec(i, field, converter.getLong(msgDATA.rawData, offset));
//                }
//                offset += (ProtocolData.unsignedIntSize + ProtocolData.unsignedIntSize);
//            }
//                break;
//            case ProtocolMsgKeyBlock.TYPE_TIME_USEC_32: {
//                // XXX Unsigned long doesn't exist in Java
//                long value = ((long)converter.getUnsignedInt(msgDATA.rawData, offset)) * 1000000 +
//                    converter.getUnsignedInt(msgDATA.rawData, offset + ProtocolData.unsignedIntSize);
//                if (isEnabledForOutput ) {
//                    outputter.outputTimeMicrosec(i, field, converter.getLong(msgDATA.rawData, offset));
//                }
//                offset += (ProtocolData.unsignedIntSize + ProtocolData.unsignedIntSize);
//            }
//                break;
//            case ProtocolMsgKeyBlock.TYPE_FLOAT: {
//                int asBits = converter.getInt(msgDATA.rawData, offset);
//                float value = Float.intBitsToFloat(asBits);
//                if (isEnabledForOutput ) {
//                    outputter.outputFloat(i, field, value);
//                }
//                offset += ProtocolData.intSize;
//            }
//                break;
//            case ProtocolMsgKeyBlock.TYPE_DOUBLE: {
//                long asBits = converter.getLong(msgDATA.rawData, offset);
//                double value = Double.longBitsToDouble(asBits);
//                if (isEnabledForOutput ) {
//                    outputter.outputDouble(i, field, value);
//                }
//                offset += ProtocolData.longSize;
//            }
//              break;
////              case ProtocolMsgKeyBlock.TYPE_UTF8:
////                  break;
////              case ProtocolMsgKeyBlock.TYPE_UTF16:
////                  break;
////              case ProtocolMsgKeyBlock.TYPE_BLOB:
////                  break;
////              case ProtocolMsgKeyBlock.TYPE_IPV6_ADDR:
////                  break;
////              case ProtocolMsgKeyBlock.TYPE_TIME_MSEC_32:
////                  break;
////              case ProtocolMsgKeyBlock.TYPE_TIME_USSEC_32:
////                  break;
//            default:
//                throw new DataException(this, "outputDataRecord",
//                                        "Unknown type: " + field.getTypeId());
//            }
//        }
//        if(fieldIdListLength > 0) { // to avoid sending data on a template with no fields...
//                                    // (which is an 'unexpected' template)
//           outputter.endRecord();
//        }
//    }

}