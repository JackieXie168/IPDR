package com.xacct.CRANE.collector;

// $Id: CollectorSetupManager.java,v 1.5 2005/04/08 12:48:10 guyg Exp $
// Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.net.UnknownHostException;
import java.util.ArrayList;

/** A special version of CollectorManager, used to get the session and
 *  template information from a transmitter.
 *
 *  Unlike its superclass, this class is designed to run one thread,
 *  outputting its progress (via a log callback), and it can be
 *  cancelled at any time.
 *
 *  Also unlike its superclass (CollectorManager), this does not run
 *  until stopped; it runs until it gets a response from the
 *  transmitter (or times out in getting a response). So, if you let
 *  it run to completion, there is no need to call
 *  waitForConnectionsToFinish, although that shouldn't hurt.
 *
 *  <pre>
 *  // in one thread
 *  CollectorSetupManager manager = new CollectorSetupManager(...);
 *  Thread managerThread = new Thread(manager, "Setup from ...");
 *  managerThread.start();
 *  managerThread.join();
 *  if (manager.finishedWithoutError()) {
 *      String vendorString = manager.getVendorString();
 *      ...
 *  } else {
 *      throw manager.getRunException();
 *  }
 *  </pre>
 *
 *  If you have another thread running, it can cancel the action by
 *  <code>manager.stopAndWaitForConnectionsToFinish()</code>, which
 *  should cause the an exception in the thread (that is,
 *  <code>manager.finishedWithoutError()</code> should return
 *  <code>false</code>).
 *
 *  XXX Need to test that this actually does stop things.
 *  @see ConnectionSetupThread
 *
 *  @version $Id: CollectorSetupManager.java,v 1.5 2005/04/08 12:48:10 guyg Exp $
 */
public final class CollectorSetupManager
    extends CollectorManager
    implements Runnable, java.io.Serializable
{
    /** $Id: CollectorSetupManager.java,v 1.5 2005/04/08 12:48:10 guyg Exp $ */
    public static String versionStr() { return "@(#) $Id: CollectorSetupManager.java,v 1.5 2005/04/08 12:48:10 guyg Exp $"; }

    /// The constructed configuration.
    private CollectorConfig collectorSetupConfig;

    /** Get the session/template information from an transmitter.
     *  This creates a connection, starts a thread, sends a "get sessions" request.
     *  You can monitor its progress with the log callback.
     *  You do <em>not</em> need to do stopAndWaitForConnectionsToFinish().
     *
     *  XXX This code is not fully baked: in particular, it does horrible
     *  things if there is already an existing connection. What it should
     *  do is something different if there's alread a connection.
     * @param collectorAddress This collector's address (may be null or "", in which case defaults to localhost).
     * @param collectorConnectPortId The connection port Id for identifying this collector to the transmitter.
     * @param log Gets progress messages.
     * @param transmitterAddress The remote transmitter's address.
     * @param transmitterPort Where to get data from at the transmitter if a connectino to the version port fails.
     * @param transmitterVersionPort The version negotiation port on the transmitter.
     * @param connectTimeout How long to wait for a UDP respone during version negotiation.
     * @param reconnectInterval How long to wait before trying to reconnect.
     * @throws ConfigException If there's something wrong with the configuration (shouldn't happen).
     * @throws UnknownHostException If a local or remote address is invalid.
     */
    public CollectorSetupManager(ExtendedInetAddress collectorAddress,
                                 int collectorConnectPortId,
                                 LogHandler log,
                                 ExtendedInetAddress transmitterAddress,
                                 int transmitterPort,
                                 int transmitterVersionPort,
                                 int connectTimeout,
                                 int reconnectInterval)
        throws ConfigException {
        super(log);
        set(collectorAddress,
            collectorConnectPortId,
            transmitterAddress,
            transmitterPort,
            transmitterVersionPort,
            connectTimeout,
            reconnectInterval);
    }

    public CollectorSetupManager(String collectorAddress,
                                 int collectorConnectPortId,
                                 LogHandler log,
                                 String transmitterAddress,
                                 int transmitterPort,
                                 int transmitterVersionPort,
                                 int connectTimeout,
                                 int reconnectInterval)
        throws ConfigException, UnknownHostException {
        super(log);
        set(new ExtendedInetAddress(collectorAddress),
            collectorConnectPortId,
            new ExtendedInetAddress(transmitterAddress),
            transmitterPort,
            transmitterVersionPort,
            connectTimeout,
            reconnectInterval);
    }

    private void set(ExtendedInetAddress collectorAddress,
                     int collectorConnectPortId,
                     ExtendedInetAddress transmitterAddress,
                     int transmitterPort,
                     int transmitterVersionPort,
                     int connectTimeout,
                     int reconnectInterval)
        throws ConfigException {
        connectionList = new ArrayList(1);
        connectionList.add(new ConnectionConfig(transmitterAddress,
                                                transmitterPort,
                                                transmitterVersionPort,
                                                "", // vendor string
                                                connectTimeout,
                                                reconnectInterval,
                                                (ArrayList)null));

        this.collectorSetupConfig =
            new CollectorConfig(CollectorConfig.VERSION_1,
                                collectorAddress,
                                collectorConnectPortId,
                                1000, // ack Interval - unused
                                0, // collector start interval
                                connectionList);
    }

    // global so that we can get the transmitter port from it.
    private ArrayList connectionList;

    // This value is updated during the negotiation process
    public int getTransmitterPort() {
        return ((ConnectionConfig)connectionList.get(0)).getTransmitterPort();
    }

    /** Do the actual processing.
     */
    public void run() {
        try {
            initAndStartConnections(collectorSetupConfig, log, null);
        } catch(InterruptedException e) {
            // This situation shouldn't normally happen: it's only
            // there because there's a "sleep" between starting
            // threads and a sleep can be interrupted. So, this
            // situation shouldn't happen.
            throw new LogicException(this, "run",
                                     "Interrupted initAndStartConnections: " + e);
        }
        waitForConnectionsToFinish();
    }

    /** Get the exception (if any) from receiving the session/template
     *  information.  Only meaningful if super.waitForConnectionsToFinish
     *  has finished (which should be the case if the constructor has done
     *  its job).
     * @return The exception: null if everything went fine.
     */
    public Exception getRunException() { return getThread().getRunException(); }

    /** Check whether the thread produced any errors.  Only meaningful
     *  if super.waitForConnectionsToFinish has finished (which should
     *  be the case if the constructor has done its job).
     *  @see #getRunException
     *  @return True if finished successfully, false if there was a problem.
     */
    public boolean finishedWithoutError() { return getRunException() == null; }

    /** Get the vendor string (if any) from receiving the session/template
     *  information.  Only meaningful if super.waitForConnectionsToFinish
     *  has finished (which should be the case of the constructor has done
     *  its job and getRunException has returned null.
     * @return The exception: null if everything went fine.
     */
    public String getVendorString() { return getThread().getVendorString(); }

    /** Get the session configurationst (if any) from receiving the
     *  session/template information.  Only meaningful if
     *  super.waitForConnectionsToFinish has finished (which should be the
     *  case of the constructor has done its job and getRunException has
     *  returned null.
     * @return The exception: null if everything went fine.
     */
    public ArrayList getSessionConfigList() { return getThread().getSessionConfigList(); }

    /** Override the super-classe's creation of a thread to connect to the
     * transmitter, for getting session and template info.
     * @param collectorConfig The overall configuration information.
     * @param connectionConfig The single connection information, used to
     *            get from the transmitter.
     * @param outputterFactory (unused).
     * @param log For reporting what happens.
     * @param receiveBufferSize (unused).
     * @return The thread.
     */
    protected ConnectionThread makeConnectionThread(ConnectionConfig connectionConfig,
                                                    CollectorOutputterFactory outputterFactory,
                                                    LogHandler log,
                                                    int receiveBufferSize) {
        return new ConnectionSetupThread(collectorConfig,
                                         connectionConfig,
                                         log);
    }

    /** Helper function: get the (only) thread.
     * @return The thread.
     */
    private final ConnectionSetupThread getThread() {
        return (ConnectionSetupThread)super.getTheOnlyThread();
    }
}
