package com.xacct.CRANE.collector;

// $Id: ConnectionConfig.java,v 1.4 2005/01/30 15:13:42 ilyad Exp $
// Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.util.ArrayList;
import java.util.Iterator;
import java.util.ListIterator;

/** The configuration information for a connection with a CRANE server (collector).
 *
 *  @version $Id: ConnectionConfig.java,v 1.4 2005/01/30 15:13:42 ilyad Exp $
 */
public final class ConnectionConfig implements ConfigObject, Comparable, java.io.Serializable
{
    /** $Id: ConnectionConfig.java,v 1.4 2005/01/30 15:13:42 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: ConnectionConfig.java,v 1.4 2005/01/30 15:13:42 ilyad Exp $"; }

    /** The transmitter (client) address, from which we get data.
     *  &lt;IPAddress&gt;...&lt;/IPAddress&gt;
     *  @see #getTransmitterVersionPort
     *  @see #getTransmitterPort
     */
    public final ExtendedInetAddress getTransmitterAddress() { return this.transmitterAddress; }
    public final void setTransmitterAddress(ExtendedInetAddress value) { this.transmitterAddress= value; }
    private ExtendedInetAddress transmitterAddress;

    /** The transmitter (client) port, from which we get protocol version information
     *  Although this can be specified in the XML, it is usually got from the
     *  ProtocolInfoResponse message, whose value overrides the XML value.
     *  &lt;Port&gt;...&lt;/Port&gt;
     *  @see #getTransmitterVersionPort
     *  @see #getTransmitterAddress
     */
    public final int getTransmitterPort() { return this.transmitterPort; }
    public final void setTransmitterPort(int value) { this.transmitterPort = value; }
    private int transmitterPort;

    /** The transmitter (client) port, from which we get the initial protocol information.
     *  &lt;VersionPort&gt;...&lt;/VersionPort&gt;
     *  @see #getTransmitterPort
     *  @see #getTransmitterAddress
     *  @see ProtocolMsgInfoRequest
     *  @see ProtocolMsgInfoResponse
     */
    public final int getTransmitterVersionPort() { return this.transmitterVersionPort; }
    public final void setTransmitterVersionPort(int value) { this.transmitterVersionPort = value; }
    private int transmitterVersionPort;

    /** The transmitter's "vendor" identification string.
     *  &lt;VendorString&gt;...&lt;/VendorString&gt;
     */
    public final String getTransmitterVendorString() { return this.transmitterVendorString; }
    public final void setTransmitterVendorString(String value) { this.transmitterVendorString = value; }
    private String transmitterVendorString;

    /** How long to wait for a ProtocolInfoRequest to time out in
     *  milliseconds -- also used for timeouts when getting session
     *  and template information from the transmitter.  This is the
     *  overall time; the individual retries may be sent more often.
     *  &lt;ConnectTimeout&gt;...&lt;/ConnectTimeout&gt;
     *  @see #getConnectRetryTimeout
     */
    public final int getConnectTimeout() { return this.connectTimeout; }
    private int connectTimeout = Constants.DEFAULT_CONNECT_TIMEOUT;

    /** How long to wait for between individual attempts to get
     *  ProtocolInfo in milliseconds. If no response is received
     *  within this time, another attempt will be made. The total
     *  number of attempts is controlled by connectTimeout.
     *  @see #getConnectTimeout
     *  @see #getConnectRoundtripTimeout
     */
    public final int getConnectRetryTimeout() { return this.connectRetryTimeout; }
    private int connectRetryTimeout = Constants.DEFAULT_RETRY_TIMEOUT;

    /** How long to wait on an individual UDP packet before deciding
     *  that it has been lost and should be re-sent.
     *  @see #getConnectRetryTimeout
     */
    public final int getConnectRoundtripTimeout() { return this.connectRoundtripTimeout; }
    private int connectRoundtripTimeout = Constants.DEFAULT_CONNECT_ROUNDTRIP_TIMEOUT;

    /** How long to wait before trying to connect again to the transmitter
     *  in milliseconds.
     *  &lt;ReconnectInterval&gt;...&lt;/ReconnectInterval&gt;
     */
    public final int getReconnectInterval() { return this.reconnectInterval; }
    private int reconnectInterval = Constants.DEFAULT_RECONNECT_INTERVAL;

    /** Interval between outputting messages to the log about ACKs, in milliseconds.
     */
    public final int getDataAckLogInterval() { return this.dataAckLogInterval; }
    private int dataAckLogInterval = Constants.DEFAULT_DATA_ACK_LOG_INTERVAL;

    /** The sessions within this connection. */
    public final ArrayList getSessionConfigList() {
        return (sessionConfigList == null) ? emptyArrayList : sessionConfigList;
    }
    public final void setSessionConfigList(ArrayList value) { this.sessionConfigList = value; }
    private ArrayList sessionConfigList;
    public final ListIterator getSessionConfigIterator() {
        return (sessionConfigList == null) ? emptyListIterator
            : sessionConfigList.listIterator();
    }
    public final int getSessionConfigListLength() {
        return (sessionConfigList == null) ? 0 : sessionConfigList.size();
    }
    public final SessionConfig getSessionConfigAt(int i) {
        return (SessionConfig)sessionConfigList.get(i);
    }

    /** Get a session from the list of sessions, using the session ID
     *  as key, throwing an exception if it's not there.
     * @param id Session ID.
     * @return The session configuration.
     * @throws ConfigException If the session ID not found.
     * @see #lookupSessionConfigById
     */
    public final SessionConfig getSessionConfigById(int id) throws ConfigException {
        return SessionConfig.getById(this, sessionConfigList, id);
    }
    /** Get a session from the list of sessions, using the session ID
     *  as key, return null if it's not there.
     * @param id Session ID.
     * @return The session configuration; null if not found.
     * @see #getSessionConfigById
     */
    public final SessionConfig lookupSessionConfigById(int id) {
        return SessionConfig.lookupById(this, sessionConfigList, id);
    }

    /** Get a connection in a list of connections, using the addr:versionPort as a key,
     *  throwing an exception if it's not there.
     * @see ConnectionConfig#lookupById
     * @param container The containing object (for error messages).
     * @param connectionConfigList The list of connections.
     * @param addr The transmitter's address.
     * @param versionPort The transmitter's version port.
     * @return The connection whose address:versionPort matches
     * @throws ConfigException If address:versionPort not found.
     */
    public final static ConnectionConfig getById(Object container,
                                                 ArrayList connectionConfigList,
                                                 ExtendedInetAddress addr,
                                                 int versionPort)
        throws ConfigException {
        ConnectionConfig result =
            lookupById(container, connectionConfigList, addr, versionPort);
        if (result == null) {
            throw new ConfigException(container, "ConnectionConfig.getById",
                                      "Unknown connection: " + addr.andPortToString(versionPort) +
                                      " in " + container);
        }
        return result;
    }

    /** Look up a connection in a list of connections, using the addr:versionPort as a key,
     *  returning null if it's not there.
     * @see ConnectionConfig#getById
     * @param container The containing object (for error messages).
     * @param connectionConfigList The list of connections.
     * @param addr The transmitter's address.
     * @param versionPort The transmitter's version port.
     * @return the connection whose address:versionPort matches, null if not found.
     */
    public final static ConnectionConfig lookupById(Object container,
                                                    ArrayList connectionConfigList,
                                                    ExtendedInetAddress addr,
                                                    int versionPort) {
        ConnectionConfig result = null;
        for (Iterator it = connectionConfigList.iterator();
             result == null && it.hasNext();
            ) {
            ConnectionConfig elem = (ConnectionConfig)it.next();
            if (elem.getTransmitterAddress().equals(addr) &&
                elem.getTransmitterVersionPort() == versionPort) {
                result = elem;
            }
        }
        return result;
    }

    public final void setById(ArrayList connectionConfigList, ConnectionConfig elem) {
        for (int i = 0; i < connectionConfigList.size(); i++) {
            ConnectionConfig c = (ConnectionConfig)connectionConfigList.get(i);
            if (c.getTransmitterAddress().equals(elem.getTransmitterAddress()) &&
                c.getTransmitterVersionPort() == elem.getTransmitterVersionPort()) {
                connectionConfigList.set(i, elem);
                return;
            }
        }
        connectionConfigList.add(elem);
    }

    /// Create the object and ensure all fields are non-null

    public ConnectionConfig(ExtendedInetAddress transmitterAddress,
                            int transmitterPort,
                            int transmitterVersionPort,
                            String vendorString,
                            int connectTimeout,
                            int reconnectInterval,
                            ArrayList sessionConfigList) throws ConfigException {
        set(transmitterAddress,
            transmitterPort,
            transmitterVersionPort,
            vendorString,
            connectTimeout,
            reconnectInterval,
            -1,
            -1,
            -1,
            sessionConfigList);
    }

    public ConnectionConfig(ExtendedInetAddress transmitterAddress,
                            int transmitterPort,
                            int transmitterVersionPort,
                            String vendorString,
                            int connectTimeout,
                            int reconnectInterval,
                            int connectRetryTimeout,
                            int connectRoundtripTimeout,
                            int dataAckLogInterval,
                            ArrayList sessionConfigList) throws ConfigException {
        set(transmitterAddress,
            transmitterPort,
            transmitterVersionPort,
            vendorString,
            connectTimeout,
            reconnectInterval,
            connectRetryTimeout,
            connectRoundtripTimeout,
            dataAckLogInterval,
            sessionConfigList);
    }

    private final void set(ExtendedInetAddress transmitterAddress,
                           int transmitterPort,
                           int transmitterVersionPort,
                           String transmitterVendorString,
                           int connectTimeout,
                           int reconnectInterval,
                           int connectRetryTimeout,
                           int connectRoundtripTimeout,
                           int dataAckLogInterval,
                           ArrayList sessionConfigList) throws ConfigException {
        this.transmitterAddress      = transmitterAddress;
        this.transmitterPort         = transmitterPort;
        this.transmitterVersionPort  = transmitterVersionPort;
        this.transmitterVendorString = CollectorUtil.nonNull(transmitterVendorString);
        this.connectTimeout          = connectTimeout;
        this.reconnectInterval       = reconnectInterval;
        this.connectRetryTimeout     = connectRetryTimeout;
        this.connectRoundtripTimeout = connectRoundtripTimeout;
        this.dataAckLogInterval      = dataAckLogInterval;

        if (this.connectTimeout <= 0) {
            this.connectTimeout      = Constants.DEFAULT_CONNECT_TIMEOUT;
        }
        if (this.reconnectInterval <= 0) {
            this.reconnectInterval   = Constants.DEFAULT_RECONNECT_INTERVAL;
        }
        if (this.connectRetryTimeout <= 0) {
            this.connectRetryTimeout = Constants.DEFAULT_RETRY_TIMEOUT;
        }
        if (this.connectRoundtripTimeout <= 0) {
            this.connectRoundtripTimeout = Constants.DEFAULT_CONNECT_ROUNDTRIP_TIMEOUT;
        }
        if (this.dataAckLogInterval <= 0) { // set the connection thread with this +
                                            // use the full c-tor from Configuration +
                                            // change the constants to smaller names
            this.dataAckLogInterval = Constants.DEFAULT_DATA_ACK_LOG_INTERVAL;
        }
        this.sessionConfigList       = CollectorUtil.nonNull(sessionConfigList);
    }

    /** Automap all the fields in the templates in the sessions in this connection.
     *  @see CollectorConfig#automapAndEnableAllFields
     *
     *  @param collectorConfig The collector's configuration, in which
     *       this connection configuration resides.
     *  @param fieldFactory The factory for creating fields.
     *  @throws ConfigException If the field information is invalid.
     */
   public final void automapAndEnableAllFields(CollectorConfig collectorConfig,
                                               FieldDescriptorFactory fieldFactory)
       throws ConfigException {

       for (Iterator it = sessionConfigList.iterator(); it.hasNext(); ) {
           SessionConfig session = (SessionConfig)it.next();
           session.automapAndEnableAllFields(collectorConfig, fieldFactory);
       }
   }

    public final String generateThreadName() {
        return "" + transmitterAddress.andPortToString(transmitterPort) +
            "(" + transmitterVendorString + ")";
    }

    public final boolean equals(Object other) {
        try {
            return equals((ConnectionConfig)other);
        } catch (ClassCastException e) {
            return false;
        }
    }

    public final boolean equals(ConnectionConfig other) {
        return other != null && (0 == this.compareTo(other));
    }

    public final int hashCode() {
        return transmitterAddress.hashCode() ^ transmitterVersionPort ^
            transmitterPort ^ sessionConfigList.size();
    }

    public final int compareTo(Object other) {
        return compareTo((ConnectionConfig)other);
    }


    /** Compare two connections. We do <em>not</em> use fields that
     *   have no "semantic" meaning, such as "up-pointers".
     *  @param other The other connection to compare with.
     *  @return Same as Comparable#compareTo
     *  @see Comparable#compareTo
     */
    public final int compareTo(ConnectionConfig other) {
        if (this == other) { return 0; }
        int result = 0;
        if (result == 0) { result = this.transmitterAddress.compareTo(other.transmitterAddress); }
        if (result == 0) { result = this.transmitterVersionPort - other.transmitterVersionPort; }
        if (result == 0) { result = this.transmitterPort - other.transmitterPort; }
        if (result == 0) { result = this.transmitterVendorString.compareTo(other.transmitterVendorString); }
        if (result == 0) { result = this.connectTimeout - other.connectTimeout; }
        if (result == 0) { result = this.reconnectInterval - other.reconnectInterval; }
        if (result == 0) {
            for (int i = 0; result == 0 && i < this.getSessionConfigListLength() && i < other.getSessionConfigListLength(); i++) {
                result = this.getSessionConfigAt(i).compareTo(other.getSessionConfigAt(i));
            }
        }
        if (result == 0) { result = this.getSessionConfigListLength() - other.getSessionConfigListLength(); }
        return result;
    }

    public final String transmitterAndPortString() {
        return transmitterAddress.andPortToString(transmitterPort);
    }

    public final String transmitterAndVersionPortString() {
        return transmitterAddress.andPortToString(transmitterVersionPort);
    }

    public final String toString() {
        return "Connection[transmitter=" + transmitterAndPortString() +
            " [host=" + transmitterAddress.getHostName() +
            // XXX needs JDK 1.4: " canonicalHost=" + transmitterAddress.getCanonicalHostName() +
            " addr=" + transmitterAddress.getHostAddress() + "] " +
            "(versionPort=" + transmitterVersionPort +
            ") connTimeout=" + connectTimeout +
            " reconnectIntvl=" + reconnectInterval +
            " vendor(" + transmitterVendorString + ")" +
            "]";
    }

    public final String toStringLong() { return toStringLong(""); }

    public final String toStringLong(String indent) {
        String result = this.toString();
        for (int i = 0; i < getSessionConfigListLength(); i++) {
            result += "\n" + indent + "session #" + i + ": " +
                getSessionConfigAt(i).toStringLong("    " + indent);
        }
        return result;
    }
}
