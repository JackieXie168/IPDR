package com.xacct.CRANE.collector;

// $Id: ConnectionSetupThread.java,v 1.5 2005/04/08 12:48:10 guyg Exp $
// Copyright (2005) (c) Amdocs, All Rights Reserved
// This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
// ANY KIND either expresses or implied.

import java.util.ArrayList;
import java.util.Iterator;

/** A special version of ConnectionSetup, used to get the session and
 *  template information from a single transmitter.
 *
 *  @see CollectorSetupManager
 *
 *  @version $Id: ConnectionSetupThread.java,v 1.5 2005/04/08 12:48:10 guyg Exp $
 */
public final class ConnectionSetupThread extends ConnectionThread implements java.io.Serializable
{
    /** $Id: ConnectionSetupThread.java,v 1.5 2005/04/08 12:48:10 guyg Exp $ */
    public static String versionStr() { return "@(#) $Id: ConnectionSetupThread.java,v 1.5 2005/04/08 12:48:10 guyg Exp $"; }

    /** Create a connection setup thread.
     *  @param collectorConfig The collector context.
     *  @param connectionConfig The connection configuration.
     *  @param log The log for messages. Often this will be
     *    different from the thread used for transferring data because we
     *    want to show progress separately (e.g., on a GUI during
     *    configuration).
     */
    public ConnectionSetupThread(CollectorConfig collectorConfig,
                                 ConnectionConfig connectionConfig,
                                 LogHandler log) {
        super(collectorConfig, connectionConfig, null, log, 0);
        this.runException      = new Exception("Not finished"); // in case somebody checks before it's finished.
        this.vendorString      = new StringBuffer();
        this.sessionConfigList = new ArrayList();
    }

    protected final void prepareForRunning() { /* Override: do nothing. */ }

    /// When the thread ends, any exception that occurred.
    private Exception runException;

    /** Get any exception that occurred when the thread was running.
     *  Undefined if the thread is still running.
     * @return Any exception that was thrown by the thread while running;
     *     null if no exception was thrown (i.e., it finished OK).
     */
    public synchronized Exception getRunException() { return this.runException; }

    /** When the thread ends, the vendor string that was received, if
     *  there was no exception.
     *  @see #getRunException
     */
    private StringBuffer vendorString;

    /** Get the vendor string.  Undefined if the thread is still
     *  running or if {@link #getRunException} returns non-null.
     * @return The vendor string from the transmitter.
     */
    public synchronized String getVendorString() { return this.vendorString.toString(); }

    /** When the thread ends, the sessions that were received, if
     *  there was no exception.
     *  @see #getRunException
     */
    private ArrayList sessionConfigList;

    /** Get the sessions.  Undefined if the thread is still
     *    running or if {@link #getRunException} returns non-null.
     *  @return The vendor string from the transmitter.
     */
    public synchronized ArrayList getSessionConfigList() { return this.sessionConfigList; }

    /** The overridden loop that the thread runs.
     *  @see ConnectionThread#run
     *  @see ConnectionThread#runLoop
     *  @throws ConfigException If there's something wrong with the configuration.
     *  @throws ConnectionIOException If something went wrong with the
     *                connection to the transmitter.
     *  @throws DataException If we didn't get what we expected from
     *                the transmitter.
     *  @throws ProtocolMalformedMsgException If there was something
     *                wrong in a message from the transmitter.
     */
    protected synchronized void runLoop()
        throws ProtocolMalformedMsgException, DataException, ConfigException, ConnectionIOException {
        try {
            log.info(this, "runLoop",
                     "Trying UDP connection to transmitter for version and port negotiation ...");
            makeConnection();

            if (isRunning) {
                requestSessionAndTemplateInfo();
            }
            log.info(this, "runLoop",
                     "Finished getting sessions and templates");
            this.runException = null; // Indicate that all is well
        } catch (ConnectionIOException e) {
            log.error(this, "runLoop",
                      "error with transmitter (client) " +
                      connectionConfig.transmitterAndPortString() +
                      ": " + e.getOriginalMsg() + " ... caused by: " + e.getCause());
            this.runException = e;
        } catch(Exception e) {
            e.printStackTrace();
            log.error(this, "runLoop",
                      "error with transmitter (client) " +
                      connectionConfig.transmitterAndPortString() +
                      ": " + e);
            this.runException = e;
        }

        if (this.connection != null) {
            this.connection.close();
            this.connection = null; // make sure the connection is gone
        }

        // XXX The following is a kludge because of bad structuring
        // of the run() and runLoop() methods.
        this.isRunning = false; // make sure upper level doesn't keep on trying
    }

    /** Request the sessions and the templates for each session from
     *  the transmitter.
     *
     * @throws ConfigException
     * @throws ConnectionIOException
     * @throws DataException
     * @throws ProtocolMalformedMsgException
     */
    private synchronized void requestSessionAndTemplateInfo()
        throws ConnectionIOException, ProtocolMalformedMsgException, DataException, ConfigException {
        requestSessions();
        log.info(this, "requestSessionAndTemplateInfo", "VendorString(" + vendorString + ")");
        log.info(this, "requestSessionAndTemplateInfo", "Processing the " + sessionConfigList.size() + " sessions");
        if (sessionConfigList.size() == 0) {
            log.warn(this, "requestSessionAndTemplateInfo",
                     "No sessions: check for being properly registered with transmitter " + connection);
        }
        for (Iterator it = sessionConfigList.iterator(); it.hasNext(); ) {
            SessionConfig session= (SessionConfig)it.next();
            ArrayList sessionTemplateConfigList =
                requestTemplatesAndFields((byte)session.getId());
            session.setTemplates(sessionTemplateConfigList);
        }
        log.info(this, "requestSessionAndTemplateInfo",
                 "End processing the " + sessionConfigList.size() + " sessions");
    }

    /** Get the sessions from the transmitter.
     * @throws ConfigException
     * @throws ConnectionIOException
     * @throws DataException
     * @throws ProtocolMalformedMsgException
     */
    private final void requestSessions()
        throws ConnectionIOException, ProtocolMalformedMsgException,
        DataException, ConfigException {

        // XXX This code only works if next response from transmitter
        // is GET_SESS_RSP ...  ought to be done with callback on
        // request id

        vendorString.setLength(0);
        sessionConfigList.clear();

        ProtocolMsgGET_SESS msgGET_SESS =
            new ProtocolMsgGET_SESS(getNextRequestId());
        msgGET_SESS.sendMsg(connection);

        // It's not a good idea to wait forever here, just in case the
        // TCP connection doesn't respond. We'll use the
        // XXX ProtocolInfoRequest timeout, even though it's not really
        // intended for this.
        ProtocolMsg responseMsg =
            ProtocolMsg.readMsg(connection, "response to", msgGET_SESS,
                                connectionConfig.getConnectTimeout());
        log.info(this, "requestSessions",
                 "GET_SESS_RSP response: " + responseMsg.toStringLong());

        if (! (responseMsg instanceof ProtocolMsgGET_SESS_RSP)) {
            throw new DataException(this, "requestSessions",
                                    "Expected GET_SESS_RSP from request message " + msgGET_SESS +
                                    " but received: " + responseMsg);
        }

        ProtocolMsgGET_SESS_RSP response = (ProtocolMsgGET_SESS_RSP)responseMsg;
        if (msgGET_SESS.requestId != response.requestId) {
            throw new DataException(this, "requestSessions",
                                    "Unexpected request ID (" + response.requestId +
                                    " when expecting " + msgGET_SESS.requestId + " from request message " +
                                    msgGET_SESS + " ... whose response was " + responseMsg);
        }

        vendorString.append(response.vendorString);
        this.sessionConfigList = response.sessionConfigList;
    }

    /** Get the templates and fields for a session from the transmitter.
     *  Side effect: updates fieldConfigSet
     * @param sessionId
     * @return The templates (fieldConfigSet is modified also).
     * @throws ConfigException
     * @throws ConnectionIOException
     * @throws DataException
     * @throws ProtocolMalformedMsgException
     */
    private final ArrayList requestTemplatesAndFields(byte sessionId)
        throws ConnectionIOException, ProtocolMalformedMsgException, DataException, ConfigException {

        // XXX This code only works if next response from transmitter is GET_TMPL_RSP ...
        // ought to be done with callback on request id

        ProtocolMsgGET_TMPL msgGET_TMPL =
            new ProtocolMsgGET_TMPL((byte)sessionId, getNextRequestId());
        msgGET_TMPL.sendMsg(connection);

        // It's not a good idea to wait forever here, just in case the
        // TCP connection doesn't respond. We'll use the
        // XXX ProtocolInfoRequest timeout, even though it's not really
        // intended for this.
        ProtocolMsg responseMsg =
            ProtocolMsg.readMsg(connection, "response to", msgGET_TMPL,
                                connectionConfig.getConnectTimeout());

        if (! (responseMsg instanceof ProtocolMsgGET_TMPL_RSP)) {
            throw new DataException(this, "requestTemplates",
                                    "Expected GET_TMPL_RSP from request message " + msgGET_TMPL +
                                    " but received: " + responseMsg);
        }

        ProtocolMsgGET_TMPL_RSP response = (ProtocolMsgGET_TMPL_RSP)responseMsg;
        if (msgGET_TMPL.requestId != response.requestId) {
            throw new DataException(this, "requestTemplates",
                                    "Unexpected request ID (" + response.requestId +
                                    " when expecting " + msgGET_TMPL.requestId + " from request message " +
                                    msgGET_TMPL + " ... whose response was " + responseMsg);
        }

        ArrayList result = response.templateBlock.getTemplateConfigList();
        return result;
    }

    /** Stop the thread as soon as possible.
     */
    public void stopProcessing() {
        log.info(this, "stopProcessing", "Stopping the setup.");
        super.stopProcessing();
    }

    /** Display the thread's status to the log.
     * @param log
     */
    public synchronized void logThreadStatus(LogHandler log) {
        log.info(this, "logThreadStatus",
                 "Connection thread " + this.toString() +
                 " " +
                 ((connection == null) ? "no connection" : connection.toString()));
    }
}
