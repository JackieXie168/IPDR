package org.ipdr.api.configuration;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.net.Inet4Address;
import java.util.ArrayList;

/**
 * The class provides a convenient base for <code>ConnectionConfigI</code> implementations
 */
public abstract class ConnectionConfigBase implements ConnectionConfigI
{
	/**
	 * Holds connection maximum allowed count
	 */
    protected int connectionCount;
	/**
	 * Holds connection description
	 */
	protected String connectionDescription;
	/**
	 * Holds connection port
	 */
	protected int connectionPort;
	/**
	 * Holds CONNECT timeout
	 */
	protected int connectTimeout;
	
	/**
	 * Holds exporter configuration
	 */
	protected ExporterConfigI exporter;
	/**
	 * Holds keepalive interval
	 */
	protected int keepAliveInterval;
	/**
	 * Holds connection type (true if passive)
	 */
	protected boolean passiveConnection;
	/**
	 * Holds connection reference count
	 */
	protected int referenceCount = 0;
	/**
	 * Holds number of connect retries for the connection
	 */
	protected int retryCount;
	/**
	 * Holds interval between connection retries
	 */
	protected int retryInterval;
	
	/**
	 * Holds list of sessions
	 */
	protected ArrayList sessionList = new ArrayList();
	
	/**
	 * Initializes the class with given exporter configuration
	 * @param exporter <code>ExporterConfigI</code> instance to use
	 */
	protected ConnectionConfigBase( ExporterConfigI exporter )
	{
		this.exporter = exporter;
	}

	/**
	 * @see org.ipdr.api.configuration.ConnectionConfigI#addReference()
	 */
	public void addReference()
	{
		referenceCount++;
	}
	/**
	 * @see org.ipdr.api.configuration.ConnectionConfigI#getCollectorConfig()
	 */
	public CollectorConfigI getCollectorConfig()
	{
		return exporter.getCollectorConfig();
	}


	/**
	 * @see org.ipdr.api.configuration.ConnectionConfigI#getConnectionCount()
	 */
	public int getConnectionCount()
	{
		return connectionCount;
	}
	
	/**
	 * @see org.ipdr.api.configuration.ConnectionConfigI#getConnectionDescription()
	 */
	public String getConnectionDescription()
	{
		return connectionDescription;
	}
	/**
	 * @see org.ipdr.api.configuration.ConnectionConfigI#getConnectTimeout()
	 */
	public int getConnectTimeout()
	{
		return connectTimeout;
	}
	/**
	 * @see org.ipdr.api.configuration.ConnectionConfigI#getExporterAddress()
	 */
	public Inet4Address getExporterAddress()
	{
		return exporter.getExporterAddress();
	}
	/**
	 * @see org.ipdr.api.configuration.ConnectionConfigI#getExporterAndPortString()
	 */
	public String getExporterAndPortString()
	{
		return exporter.getExporterAddress().toString() + ":" + connectionPort;
	}
	/**
	 * @see org.ipdr.api.configuration.ConnectionConfigI#getExporterPort()
	 */
	public int getExporterPort()
	{
		return connectionPort;
	}
	/**
	 * @see org.ipdr.api.configuration.ConnectionConfigI#getKeepAliveInterval()
	 */
	public int getKeepAliveInterval()
	{
		return keepAliveInterval;
	}

	/**
	 * @see org.ipdr.api.configuration.ConnectionConfigI#getReferenceCount()
	 */
	public int getReferenceCount()
	{
		return referenceCount;
	}

    /**
     * @see org.ipdr.api.configuration.ConnectionConfigI#getRetryCount()
     */
    public int getRetryCount()
    {
        return retryCount;
    }
    /**
     * @see org.ipdr.api.configuration.ConnectionConfigI#getRetryInterval()
     */
    public int getRetryInterval()
    {
        return retryInterval;
    }

	/**
	 * @see org.ipdr.api.configuration.ConnectionConfigI#getSessionConfigAt(int)
	 */
	public SessionConfigI getSessionConfigAt(int index)
	{
		return (SessionConfigI)sessionList.get(index);
	}

	/**
	 * @see org.ipdr.api.configuration.ConnectionConfigI#getSessionListLength()
	 */
	public int getSessionListLength()
	{
		return sessionList.size();
	}

	/**
	 * @see org.ipdr.api.configuration.ConnectionConfigI#isPassiveConnection()
	 */
	public boolean isPassiveConnection()
	{
		return passiveConnection;
	}

	/**
	 * @see org.ipdr.api.configuration.ConnectionConfigI#removeReference()
	 */
	public void removeReference()
	{
		referenceCount--;
	}
    public ExporterConfigI getExporterConfig()
    {
        return this.exporter;
    }
}
