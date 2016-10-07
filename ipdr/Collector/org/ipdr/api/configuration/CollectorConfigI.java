package org.ipdr.api.configuration;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.net.Inet4Address;
import java.net.InetAddress;


/**
 * This interface provides configuration of the collector.
 */
public interface CollectorConfigI 
{
    /** 
     * Returns IP address of the collector. 
     */
    public Inet4Address getCollectorAddress();

    
    /** Interval to wait between starting threads, to avoid
     *  a burst of activity at the beginning.
     */
    public int getCollectorStartInterval();
    
    /**
     * Indicates whether the collector should support UDP-based discovery
     * @return true if discovery is supported
     */
    public boolean getCollectorSupportDiscovery();
    
    /**
     * Returns collector's supported protocol version
     * @return 1 for CRANE, 2 for IPDR
     */
    public int getCollectorProtocolVersion();

	/**
	 * Returns default receive buffer size
	 * @return Buffer size
	 */
	public int getReceiveBufferSize();
	
	/**
	 * Returns accept timeout. The accept timeout is used by <code>ListeningSocketHandler</code>
	 * to check for shutdown requests periodically rather than hang on the server socket indefinitely. 
	 * @return value of the accept timeout
	 */
	public int getAcceptTimeout();

	/**
	 * Returns strict templates configuration value. If this is true, each template received from exporter
	 * is validated against the existing configuration.  
	 * @return true if strict templates are on
	 */
	public boolean isStrictTemplates(); 
    
	
	/**
	 * Indicates whether structured templates are supported
	 * @return false always
	 */
	public boolean getCapabilitiesStructures();
	/**
	 * Indicates whether multiple sessions over same connection are supported
	 * @return true if multiple sessions are supported
	 */
	public boolean getCapabilitiesMultiSession();
    /**
     * Indicates whether the collector should support template negotiation
     * @return true if template negotiaton is supported
     */
	public boolean getCapabilitesTemplateNegotiation();
	
	/**
	 * Returns vendor id
	 * @return vendor id
	 */
	public String getVendorId();

    /**
     * Allows random access to the list of exporter configurations. Returns exporter configuration by index value
     * @param position index value of the exporter
     * @return <code>ExporterConfigI</code> of the appropriate exporter
     */
    public ExporterConfigI getExporterAt(int position);
    /**
     * Returns number of existing exporter configurations
     * @return length of internal exporter list
     */
    public int getExporterListLength();
    /**
     * Returns exporter configuration by address
     * @param address <code>InetAddress</code> which is looked up in the list of exporters
     * @return Exporter configuration for the given address
     */
    public ExporterConfigI findExporter( InetAddress address );
    /**
     * Returns local port to listen for incoming connections on
     * @return local TCP port for inbound connections
     */
    public int getLocalPort();
    
    /**
     * Locates template configuration by given template id
     * @param id id to find template by it
     * @return <code>TemplateConfig</code> with the given id
     */
    public TemplateConfig findTemplate( int id );
    /**
     * Locates template by its ordinal number rather than by id
     * @param position index value of template
     * @return <code>TemplateConfig</code> at the given position
     */
    public TemplateConfig getTemplateAt(int position);
    
    /**
     * Returns number of configured templates
     * @return length of template list
     */
    public int getTemplateListLength();
    
    /**
     * Returns backlog size for incoming connections.
     * @return backlog size
     */
    public int getBacklogSize();
    
    /**
     * Returns UDP port to listen for incoming discovery requests on.
     * @return UDP discovery port
     */
    public int getUDPDiscoveryPort();
    
    /**
     * Returns UDP discovery timeout value. After discovery timeout, the collector
     * concludes the exporter does not support UDP-based discovery 
     * @return UDP discovery timeout value in seconds
     */
    public int getUDPDiscoveryTimeout();
    
    /**
     * Returns true if the template passed as the parameter is configured. 
     * @param template to check if it was configured
     * @return true if such template is configured
     */
    public boolean isTemplateDefined(TemplateConfig template);
    
    /**
     * Returns true if legacy CRANE protocol should be supported by the collector
     * @return true if CRANE must be supported
     */
    public boolean getSupportCRANE();


    /**
     * @return
     */
    public long getControllerQueryInterval();
}
