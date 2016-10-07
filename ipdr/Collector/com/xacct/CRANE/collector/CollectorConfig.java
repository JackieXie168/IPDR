package com.xacct.CRANE.collector;

// $Id: CollectorConfig.java,v 1.5 2005/01/30 15:13:39 ilyad Exp $
// Copyright (2005) (c) Amdocs, All Rights Reserved
// This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
// ANY KIND either expresses or implied.
import java.util.ArrayList;
import java.util.Iterator;
import java.util.ListIterator;

/** The top-level configuration information for a CRANE server (collector).
 *
 *  The logical structure (as reflected in the XML) is as
 *  follows. Note the templateId, fieldId, which reference to the
 *  global templates and fields. Also note that the templates show a
 *  list of fieldId:enabled for whether the field is to be output (for
 *  input, that is determined dynamically by template negotiation).
 *
 *  <xmp>
 *   collector
 *     [] connection
 *       [] session
 *         [] template (key = templateId)
 *           [] field (key = fieldId) [includes FieldDescriptor, enabledForOutput]
 *  </xmp>
 *
 *
 *  @version $Id: CollectorConfig.java,v 1.5 2005/01/30 15:13:39 ilyad Exp $
 */
public final class CollectorConfig implements ConfigObject, Comparable, java.io.Serializable
{
    /** $Id: CollectorConfig.java,v 1.5 2005/01/30 15:13:39 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: CollectorConfig.java,v 1.5 2005/01/30 15:13:39 ilyad Exp $"; }

    /** Configuration version code (currently "1").
     *
     *  &lt;ConfigurationVersion&gt;...&lt;/ConfigurationVersion&gt;
     */
    public final String getConfigurationVersion() { return this.configurationVersion; }
    public final void  setConfigurationVersion(String value) { this.configurationVersion = value; }
    private String configurationVersion = Constants.DEFAULT_CONFIG_VERSION;

    /** The protocol version (currently "1"). */
    public final static String VERSION_1 = "1";

    /** The collector's (server's) IP address (trying to avoid 127.0.0.1 if possible).
     *
     *  &lt;ServerIPAddress&gt;...&lt;/ServerIPAddress&gt;
     *  @see #getCollectorConnectPortId
     *  @see #setCollectorAddress
     */
    public final ExtendedInetAddress getCollectorAddress() { return this.collectorAddress; }
    public final void setCollectorAddress(ExtendedInetAddress value) { this.collectorAddress = value; }
    private ExtendedInetAddress collectorAddress;

    /** The collector's (server's) "port". This is not really a port
     *  in the TCP/IP sense; but is instead used with the CONNECT
     *  message as the "Server Port" field. Typically, this is used by
     *  the transmitter (client) to distinguish between multiple
     *  connections from the same IP address (that is, we can run
     *  multiple collectors on the same machine without multi-homing
     *  the IP address).
     *
     *  &lt;ServerPort&gt;...&lt;/ServerPort&gt;
     *  @see #getCollectorAddress
     *  @see #setCollectorConnectPortId
     */
    public final int getCollectorConnectPortId() { return this.collectorConnectPortId; }
    public final void setCollectorConnectPortId(int value) { this.collectorConnectPortId = value; }
    private int collectorConnectPortId = Constants.DEFAULT_SERVER_PORT;

    /** How often to send an ack to a transmitter (client).
     *  &lt;AckInterval&gt;...&lt;/AckInterval&gt;
     */
    public final int getAckInterval() { return this.ackInterval; }
    public final void setAckInterval(int value) { this.ackInterval = value; }
    private int ackInterval = Constants.DEFAULT_SERVER_ACK_INTERVAL;

    // XXX hard-wired for now: 30 minutes (in milliseconds)
    public final int getNoDataKeepAliveInterval() { return this.noDataKeepAliveInterval; }
    public final void setNoDataKeepAliveInterval(int value) { this.noDataKeepAliveInterval = value; }
    private int noDataKeepAliveInterval = Constants.DEFAULT_NO_DATA_KEEP_ALIVE_INTERVAL;


    /** If time left to ACK is less than this, don't bother trying a
     *  read, just do the ACK. This value must be non-negative.
     *
     *  &lt;ServerAckInterval&gt;...&lt;/ServerAckInterval&gt;
     */
    public final int getAckIntervalEpsilon() { return this.ackIntervalEpsilon; }
    public final void setAckIntervalEpsilon(int value) { this.ackIntervalEpsilon = value; }
    private int ackIntervalEpsilon = Constants.DEFAULT_ACK_INTERVAL_EPSILON;

    /** Interval to wait between starting threads, to avoid
     *  a burst of activity at the beginning.
     *  &lt;CollectorStartInterval&gt;...&lt;/CollectorStartInterval&gt;
     */
    public final int getCollectorStartInterval() { return this.collectorStartInterval; }
    public final void setCollectorStartInterval(int value) { this.collectorStartInterval = value; }
    private int collectorStartInterval = Constants.DEFAULT_SERVER_START_INTERVAL;

    /** The connections' configurations. Each connection contains
     *  sessions.
     */
    private ArrayList /*ConnectionConfigList*/ connectionConfigList;
    public final void setConnectionConfigList(ArrayList value) { this.connectionConfigList = value; }
    public final ArrayList getConnectionConfigList() {
        return (connectionConfigList == null) ? emptyArrayList : connectionConfigList;
    }
    public final ListIterator getConnectionConfigIterator() {
        return (connectionConfigList == null) ? emptyListIterator
            : connectionConfigList.listIterator();
    }
    public final int getConnectionConfigListLength() {
        return (connectionConfigList == null) ? 0 : connectionConfigList.size();
    }
    public final ConnectionConfig getConnectionConfigAt(int i) {
        return (ConnectionConfig)connectionConfigList.get(i);
    }

    /** Get a connection from the list of connections, using the
     *  addr:versionPort as a key, throwing an exception if it's not
     *  there.
     *  @see CollectorConfig#lookupConnectionConfigById
     *  @param addr The transmitter's address.
     *  @param versionPort The port used to negotiate version information.
     *  @return the connection whose address:versionPort matches
     *  @throws ConfigException If address:versionPort not found.
     */
    public final ConnectionConfig getConnectionConfigById(ExtendedInetAddress addr, int versionPort)
        throws ConfigException {
        return ConnectionConfig.getById(this, connectionConfigList, addr, versionPort);
    }

    /** Look up a connection in the list of connections, using the
     *  addr:versionPort as a key, returning null if it's not there.
     * @see CollectorConfig#getConnectionConfigById
     * @param addr The transmitter's address.
     * @param versionPort The transmitter's version port.
     * @return the connection whose address:versionPort matches, null if not found.
     */
    public final ConnectionConfig lookupConnectionConfigById(ExtendedInetAddress addr, int versionPort) {
        return ConnectionConfig.lookupById(this, connectionConfigList, addr, versionPort);
    }

    /**
     * Construct a collector's configuration
     *
     * @param configurationVersion @see configurationVersion
     * @param collectorAddress @see collectorAddress
     * @param collectorConnectPortId @see collectorConnectPortId
     * @param ackInterval @see ackInterval
     * @param collectorStartInterval @see collectorStartInterval
     * @param connectionConfigList @see connectionConfigList
     * @throws ConfigException on an invalid value
     */
    public CollectorConfig(String configurationVersion,
                           ExtendedInetAddress collectorAddress,
                           int collectorConnectPortId,
                           int ackInterval,
                           int collectorStartInterval,
                           ArrayList connectionConfigList) throws ConfigException {
        set(configurationVersion,
            collectorAddress,
            collectorConnectPortId,
            ackInterval,
            collectorStartInterval,
            connectionConfigList);
    }

    /**
     * Set the values in a collector's configuration
     *
     * @param configurationVersion @see configurationVersion
     * @param collectorAddress @see collectorAddress
     * @param collectorConnectPortId @see collectorConnectPortId
     * @param ackInterval @see ackInterval
     * @param collectorStartInterval @see collectorStartInterval
     * @param connectionConfigList @see connectionConfigList
     * @throws ConfigException on an invalid value
     */
    private final void set(String configurationVersion,
                           ExtendedInetAddress collectorAddress,
                           int collectorConnectPortId,
                           int ackInterval,
                           int collectorStartInterval,
                           ArrayList connectionConfigList) throws ConfigException {
        this.configurationVersion   = CollectorUtil.nonNull(configurationVersion);
        this.collectorAddress       = collectorAddress;
        this.collectorConnectPortId = collectorConnectPortId;
        this.ackInterval            = ackInterval;
        this.collectorStartInterval = collectorStartInterval;
        this.connectionConfigList   = CollectorUtil.nonNull(connectionConfigList);

        validate();
    }

    /** Automap all the fields in the templates in the sessions in the
     *  connections.  The fields typically get names that are derived
     *  from the field names that were received from the transmitter.
     *
     *  @param collectorConfig The collector's configuration, in which
     *       this connection configuration resides.
     *  @param fieldFactory The factory for creating fields.
     *  @throws ConfigException If the field information is invalid.
     */
   public final void automapAndEnableAllFields(FieldDescriptorFactory fieldFactory)
       throws ConfigException {

       for (Iterator i = connectionConfigList.iterator(); i.hasNext(); ) {
           ConnectionConfig connectionConfig = (ConnectionConfig)i.next();
           connectionConfig.automapAndEnableAllFields(this, fieldFactory);
       }
   }

    /** Validate the data, throwing an exception if there's something
     *  wrong.  This is called by the initial CRANE collectorConfig
     *  object creation; it should be called whenever the
     *  CollectorConfig object is updated, or when anything inside it
     *  is updated (that is, connections, sessions, templates, keys).
     *
     *  The main thing to validate is that all the keys in all the
     *  templates are unique. The XML does not guarantee this: it puts
     *  keys under templates. We do a similar test for templates even
     *  though the XML ought to ensure that the template keys are
     *  unique ... after all, we might change the XML in the future.
     *
     *  All templates must also be essentially unique (that is, if
     *  they have the same key, they must be equal in all important
     *  items (name, key id/types).
     *
     *  XXX should template uniqueness be global or just per session?
     *
     *  XXX should also test for only one copy of a template per session
     *
     *  Also ensure that collector start interval is at least 1 millisecond
     */
    private final void validate() throws ConfigException {
        collectorStartInterval = Math.max(1, collectorStartInterval);
        ackIntervalEpsilon = Math.max(0, ackIntervalEpsilon);

        // validateTemplates();

        // validateKeys();
    }

    /** Check that the templates contain stuff that we expect. If
     *  there's something unexpected, we try to handle it but if we
     *  can't, come to a grinding halt (requiring the configuration
     *  files to be updated). This is typically used at a session
     *  level.
     *  @param transmitterTemplateConfigList
     */
    public final void validateTemplatesFromTransmitter(ArrayList transmitterTemplateConfigList)
        throws ConfigException {
        // XXX To be written
    }

    public final int hashCode() {
        return collectorAddress.hashCode() ^
            collectorConnectPortId ^
            getConnectionConfigListLength();
    }

    public final int compareTo(Object o) {
        return compareTo((CollectorConfig)o);
    }

    public final int compareTo(CollectorConfig other) {
        if (this == other) { return 0; }
        int result = 0;
        if (result == 0) { result = this.collectorAddress.compareTo(other.collectorAddress); }
        if (result == 0) { result = this.collectorConnectPortId - other.collectorConnectPortId; }
        for (int i = 0; result == 0 && i < this.getConnectionConfigListLength() && i < other.getConnectionConfigListLength(); i++) {
            result = this.getConnectionConfigAt(i).compareTo(other.getConnectionConfigAt(i));
        }
        if (result == 0) { result = this.getConnectionConfigListLength() - other.getConnectionConfigListLength(); }
        if (result == 0) { result = this.ackInterval            - other.ackInterval; }
        if (result == 0) { result = this.collectorStartInterval - other.collectorStartInterval; }
        return result;
    }

    public final String toString() {
        return "Collector[" + "configVer=" + configurationVersion +
            " collector=" + collectorAddress.andPortToString(collectorConnectPortId) +
            " ackIntvl=" + ackInterval +
            " startIntvl=" + collectorStartInterval +
            " connection#=" + getConnectionConfigListLength() +
            " connections: " + CollectorUtil.toString(connectionConfigList) +
            "]";
    }

    public final String toStringLong() { return toStringLong(""); }

    public final String toStringLong(String indent) {
        String result = this.toString();
        for (int i = 0; i < getConnectionConfigListLength(); i++) {
            result += "\n  connection #" + i + ": " +
                getConnectionConfigAt(i).toStringLong("    ");
        }
        return result;
    }
}
