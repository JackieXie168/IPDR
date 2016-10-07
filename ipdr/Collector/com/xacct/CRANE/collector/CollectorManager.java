package com.xacct.CRANE.collector;

// $Id: CollectorManager.java,v 1.4 2005/01/30 15:13:42 ilyad Exp $
// Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.


/** A collector of data, using the CRANE protocol. This object simply
 *  holds all the data for a collector, plus methods for starting and
 *  stopping the various collection threads. The actual running
 *  threads are elsewhere.
 *  @see ConnectionThread
 *
 *  @version $Id: CollectorManager.java,v 1.4 2005/01/30 15:13:42 ilyad Exp $
 */
public class CollectorManager implements java.io.Serializable
{
    /** $Id: CollectorManager.java,v 1.4 2005/01/30 15:13:42 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: CollectorManager.java,v 1.4 2005/01/30 15:13:42 ilyad Exp $"; }

    /// The configuration (from XML).
    protected CollectorConfig collectorConfig;

    /// Get the configuration.
    public final CollectorConfig getCollectorConfig() { return collectorConfig; }

    /// The connection threads.
    private ConnectionThread [] connectionThreadList;

    /// The log.
    protected LogHandler log;
    public final LogHandler getLog() { return this.log; }

    /// The outputter factory.
    private CollectorOutputterFactory outputterFactory;

    /// Constructor, for use by sub-classes.
    protected CollectorManager(LogHandler log) {
        this.collectorConfig      = null;
        this.connectionThreadList = new ConnectionThread[0]; // none
        this.log                  = log;
        this.outputterFactory     = null;
    }

    /** Constructor which starts all the (per-connection) thread.  If
     *  you change the configuration, you must create a new
     *  CollectorManager.
     *
     *  XXX This should change in the future, to allow "on the fly" reconfiguration.
     * @param collectorConfig The configuration (will not be changed).
     * @param log The output log.
     * @param outputterFactory For outputting the records received from the transmitter(s).
     * @throws InterruptedException  If the start-up is interrupted.
     * @see Thread#start
     */
    public CollectorManager(CollectorConfig collectorConfig,
                            LogHandler log,
                            CollectorOutputterFactory outputterFactory)
        throws InterruptedException {
        initAndStartConnections(collectorConfig, log, outputterFactory);
    }

    /** Start all the collector (per-connection) threads.
     * @param outputterFactory The class which handles per-record output.
     * @throws InterruptedException  If the start-up is interrupted (it can
     *         take a while if there are a lot of connection threads and/or
     *         if there's a long sleep interval between startups).
     * @see Thread#start
     */
    protected void initAndStartConnections(CollectorConfig collectorConfig,
                                           LogHandler log,
                                           CollectorOutputterFactory outputterFactory)
    throws InterruptedException {
        this.collectorConfig  = collectorConfig;
        this.log              = log;
        this.outputterFactory = outputterFactory;

        this.connectionThreadList =
            new ConnectionThread[collectorConfig.getConnectionConfigListLength()];
        int interval = 0; // how long to wait between starting threads.
        int receiveBufferSize = 0;

        // XXX Sort-of hard-coded ...
        // If we have a relatively small number of connections, give them
        // big read sockets.
        if (collectorConfig.getConnectionConfigListLength() < 10) {
            receiveBufferSize = 256*1024;
        }

        for (int i = 0; i < collectorConfig.getConnectionConfigListLength(); i++) {
            try { // wait a bit before starting the next one
                Thread.sleep(interval);
            } catch (InterruptedException e) {
            log.info(this, "initAndStartConnections",
                "Interrupted start-up at #" + i +
                " of " + collectorConfig.getConnectionConfigListLength());
                throw e;
            }
            log.info(this, "initAndStartConnections", "Starting connection #" +
                     i + ": " + collectorConfig.getConnectionConfigAt(i));
            // XXX Note that makeConnectionThread might be over-ridden.
            connectionThreadList[i] =
                makeConnectionThread(collectorConfig.getConnectionConfigAt(i),
                                     outputterFactory, log, receiveBufferSize);
            connectionThreadList[i].start();
            interval = collectorConfig.getCollectorStartInterval();
        }
    }

    /** Set the thread(s) to do minimal input process (for performance testing).
     */
    public void setNullInput() {
        for (int i = 0; i < this.connectionThreadList.length; i++) {
            if (connectionThreadList[i] != null) {
                this.connectionThreadList[i].setNullInput();
            }
        }
    }

    /** Create a new connection thread. This can be over-ridden by a sub-class.
     *  The thread is simply created; it is not started.
     * @param connectionConfig The connection's configuration.
     * @param outputterFactory For outputting records.
     * @param log The logger.
     * @return A thread for processing the connection.
     */
    protected ConnectionThread makeConnectionThread(ConnectionConfig connectionConfig,
                                                    CollectorOutputterFactory outputterFactory,
                                                    LogHandler log,
                                                    int receiveBufferSize) {
        if (! (this instanceof CollectorManager)) { // make sure sub-class over-rides this
            throw new LogicException(this, "makeConnectionThread",
                                     "Sub-class of CollectorManager used without its own makeConnectionThread()." +
                                     " class=" + this.getClass().getName());
        }
        return new ConnectionThread(collectorConfig,
                                    connectionConfig,
                                    outputterFactory,
                                    log,
                                    receiveBufferSize);
    }

    /** Stop all the collector threads. This does not interrupt the
     *  threads; it depends on ConnectionThread.stopProcessing() to do
     *  that, if desirable. Also, does not send a STOP message to the
     *  transmitter; again, that's the job of
     *  ConnectionThread.stopProcessing().
     *
     *  This does not wait for the threads to finish.
     *
     * @see ConnectionThread#stopProcessing
     * @see #waitForConnectionsToFinish
     */
    public void stopConnections() {
        for (int i = 0; i < connectionThreadList.length; i++) {
            if (connectionThreadList[i] != null) {
                log.info(this, "stopConnections", "Stopping connection #" + i +
                         ": " + connectionThreadList[i]);
                if (connectionThreadList[i] != null) {
                    connectionThreadList[i].stopProcessing();
                }
            }
        }
    }

    /** Wait for all the collector threads to finish.
     *
     * @see Thread#join
     */
    public void waitForConnectionsToFinish() {
        for (int i = 0; i < connectionThreadList.length; i++) {
            if (connectionThreadList[i] != null) {
                try {
                    if (connectionThreadList[i] != null) {
                        connectionThreadList[i].join();
                    }
                } catch (InterruptedException e) { }
                // don't: might need data from threads later connectionThreadList[i] = null;
            }
        }
    }

    /** Stop all connections and wait for them to gracefully finish.
     *  @see #stopConnections
     *  @see #waitForConnectionsToFinish
     */
    public void stopAndWaitForConnectionsToFinish() {
        stopConnections();
        waitForConnectionsToFinish();
    }

    /** Helper function: get the (only) thread. This is used by subclasses that
     *  start a single thread for one specific request to the transmitter.
     * @return The thread.
     */
    protected ConnectionThread getTheOnlyThread() {
        if (connectionThreadList.length != 1) {
            throw new LogicException(this, "getTheOnlyThread",
                                     "number of threads not 1: " + connectionThreadList.length);
        }
        return connectionThreadList[0];
    }

    /** Display all the connections' statuses to the log.
     *  @param log The log for outputting.
     */
    public void logConnectionStatus(LogHandler log) {
        log.info(this, "logThreadStatus",
                 "Displaying thread status for " + connectionThreadList.length + " connections" +
                 " and " + Thread.activeCount() + " threads in the thread group for " +
                 Thread.currentThread().toString());
        Runtime runtime = Runtime.getRuntime();
        log.info(this, "logThreadStatus",
// XXX                 "Processors=" + runtime.availableProcessors() +
                 " freeMem=" + runtime.freeMemory() +
// XXX                " maxMem=" + runtime.maxMemory() +
                 " totalMem=" + runtime.totalMemory()
            );

        // XXX dump JVM's properties?

        for (int i = 0; i < connectionThreadList.length; i++) {
            if (connectionThreadList[i] != null) {
                connectionThreadList[i].logThreadStatus(log);
            }
        }

        log.info(this, "logThreadStatus", "End of thread statuses " +
                 "(" + connectionThreadList.length + " threads)");
    }

    /** Set the data ACK interval for all connections.
     * @param valueMillis The interval in milliseconds.
     */
    public void setDataAckMsgInterval(int valueMillis) {
        for (int i = 0; i < connectionThreadList.length; i++) {
            if (connectionThreadList[i] != null) {
                connectionThreadList[i].setDataAckMsgInterval(valueMillis);
            }
        }
        log.info(this, "setDataAckMsgInterval",
                 "data-ACK message will be output every " + valueMillis/1000.0 + " seconds");
    }
}
