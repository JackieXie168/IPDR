package org.ipdr.api.configuration;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.net.Inet4Address;
import java.util.ArrayList;

/**
 * The class provides a convenient base for <code>ExporterConfigI</code> implementations
 */
public abstract class ExporterConfigBase implements ExporterConfigI
{
	/**
	 * Holds collector configuration
	 */
	protected CollectorConfigI collectorConfig;
	/**
	 * Holds list of connection configurations
	 */
	protected ArrayList connectionConfigList = new ArrayList();
	/**
	 * Holds exporter's IP address
	 */
	protected Inet4Address exporterAddress;
	/**
	 * Holds exporters port
	 */
	protected int exporterPort = 15000;
	/**
	 * Holds exporters udp discovery port
	 */
	protected int udpDiscoveryPort;
	
	/**
	 * Constructs the class with given collector configuration
	 * @param collectorConfig Collector configuration
	 */
	protected ExporterConfigBase( CollectorConfigI collectorConfig )
	{
		this.collectorConfig = collectorConfig;
	}
	
	/**
	 * @see org.ipdr.api.configuration.ExporterConfigI#getCollectorConfig()
	 */
	public CollectorConfigI getCollectorConfig()
	{
		return collectorConfig;
	}
	/**
	 * @see org.ipdr.api.configuration.ExporterConfigI#getConnectionConfigAt(int)
	 */
	public ConnectionConfigI getConnectionConfigAt(int i)
	{
		return (ConnectionConfigI)connectionConfigList.get(i);
	}
	/**
	 * @see org.ipdr.api.configuration.ExporterConfigI#getConnectionConfigList()
	 */
	public ArrayList getConnectionConfigList()
	{
		return connectionConfigList;
	}
	/**
	 * @see org.ipdr.api.configuration.ExporterConfigI#getConnectionConfigListLength()
	 */
	public int getConnectionConfigListLength()
	{
		return connectionConfigList.size();
	}
	/**
	 * @see org.ipdr.api.configuration.ExporterConfigI#getExporterAddress()
	 */
	public Inet4Address getExporterAddress()
	{
		return exporterAddress;
	}
	/**
	 * @see org.ipdr.api.configuration.ExporterConfigI#getExporterAndPortString()
	 */
	public String getExporterAndPortString()
	{
		if ( exporterAddress == null )
			return "";
		return exporterAddress.toString() + ":" + String.valueOf(exporterPort);
	}
	/**
	 * @see org.ipdr.api.configuration.ExporterConfigI#getExporterPort()
	 */
	public int getExporterPort()
	{
		return exporterPort;
	}

	/**
	 * @see org.ipdr.api.configuration.ExporterConfigI#getPassiveConnectionList()
	 */
	public ArrayList getPassiveConnectionList()
	{
		ArrayList list = new ArrayList();
		for ( int i = 0; i < connectionConfigList.size(); i ++ )
		{
			ConnectionConfigI connectionConfig = (ConnectionConfigI) connectionConfigList.get(i);
			if (connectionConfig.isPassiveConnection() )
				list.add(connectionConfig);	
		}
		return list;
	}
    /**
     * @see org.ipdr.api.configuration.ExporterConfigI#getUDPDiscoveryPort()
     */
    public int getUDPDiscoveryPort()
    {
        return udpDiscoveryPort;
    }

}
