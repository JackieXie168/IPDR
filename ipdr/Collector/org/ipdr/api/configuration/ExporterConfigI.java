package org.ipdr.api.configuration;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.net.Inet4Address;
import java.util.ArrayList;

/**
 * This interface provides configuration of an Exporter
 */
public interface ExporterConfigI
{	
	/**
	 * Returns <code>CollectorConfigI</code> instance of collector configuration
	 * @return instance of collector configuration
	 */
	public CollectorConfigI getCollectorConfig();

    /**
     * Returns connection configuration by index value
     * @param i index value
     * @return <code>ConnectionConfigI</code> connection configuration at the given position 
     */
    public ConnectionConfigI getConnectionConfigAt(int i);
    
    /**
     * Returns <code>ArrayList</code> of connection configurations
     * @return list of connection configurations
     */
    public ArrayList getConnectionConfigList();

    /**
     * Returns number of existing connection configurations
     * @return length of internal connection list
     */
    public int getConnectionConfigListLength();
	
	/**
	 * Returns a sublist of passive connections from all defined connections
	 * @return <code>ArrayList</code> of <code>ConnectionConfigI</code>. Empty <code>ArrayList</code> is returned if there are no passive connections
	 */
	public ArrayList getPassiveConnectionList();

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
	 * @return exporter listening TCP port
	 */
	public int getExporterPort();
	/**
	 * Returns exporter UDP discovery port
	 * @return UDP discovery port
	 */
	public int getUDPDiscoveryPort();
}
