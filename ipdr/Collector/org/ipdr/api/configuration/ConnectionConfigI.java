package org.ipdr.api.configuration;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.net.Inet4Address;

/**
 * This interface provides configuration of a Connection
 *
 */
public interface ConnectionConfigI
{
	/**
	 * Indicates that connection attempts should be retried indefinitely
	 */
	public final int INFINITE = -1;
	/**
	 * Adds a reference to this connection configuration
	 */
	public void addReference();
	/**
	 * Returns <code>CollectorConfigI</code> instance of collector configuration
	 * @return instance of collector configuration
	 */
	public CollectorConfigI getCollectorConfig();
	/**
	 * Returns number of allowed connections 
	 * @return number of connections
	 */
	public int getConnectionCount();
	/**
	 * Returns verbal description of the connection
	 * @return description of the connection
	 */
	public String getConnectionDescription();
	/**
	 * Returns connect timeout
	 * @return connect timeout value
	 */
	public int getConnectTimeout();
	/**
	 * Returns exporter IP address
	 * @return exporter address
	 */
	public Inet4Address getExporterAddress();
	/**
	 * Returns exporter address and port as a string
	 * @return &lt;ip&gt;:&lt;port&gt;
	 */
	public String getExporterAndPortString();
	/**
	 * Returns exporter TCP port
	 * @return exporter TCP port
	 */
	public int getExporterPort();
	
	public ExporterConfigI getExporterConfig();
	/**
	 * Returns collector's keep alive interval
	 * @return keepalive interval that will be sent to exporter
	 */
	public int getKeepAliveInterval();
	/**
	 * Returns current reference count (number of existing connections that use this configuration)
	 * @return number of references
	 */
	public int getReferenceCount();
	
	/**
	 * Returns number of retries for connection attempts
	 * @return number of retries
	 */
	public int getRetryCount();
	/**
	 * Returns interval between connection attempts, in seconds
	 * @return Interval between connection attempts in seconds
	 */
	public int getRetryInterval();
    /**
     * Returns session configuration by index value
     * @param index index value
     * @return <code>SessionConfigI</code> session configuration at the given position 
     */
	public SessionConfigI getSessionConfigAt(int index);
	
    /**
     * Returns number of existing session configurations
     * @return length of internal session list
     */	
	public int getSessionListLength();
	/**
	 * Indicates that the connection is passive
	 * @return true if the connection is passive
	 */
	public boolean isPassiveConnection();
	/**
	 * Removes a reference to this connection configuration
	 */
	public void removeReference();
}
