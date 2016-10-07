package org.ipdr.api.configuration;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.net.Inet4Address;
import java.net.InetAddress;
import java.util.ArrayList;

/**
 * The class provides a convenient base for <code>CollectorConfigI</code> implementations
 */
public abstract class CollectorConfigBase implements CollectorConfigI
{
    /**
     * @see org.ipdr.api.configuration.CollectorConfigI#getControllerQueryInterval()
     */
    public long getControllerQueryInterval()
    {
        return this.controllerQueryInterval;
    }
	/**
	 * Holds accept timeout value
	 */
	protected  int acceptTimeout;
	/**
	 * Holds backlog size value
	 */
	protected int backLogSize;
	/**
	 * Holds collector IP address value
	 */
	protected Inet4Address collectorAddress;
	/**
	 * Holds collector protocol version
	 */
	protected int collectorProtocolVersion;
	/**
	 * Holds start interval value
	 */
	
	
	protected int controllerQueryInterval;
	
	protected int collectorStartInterval;
	/**
	 * Holds discovery support value
	 */
	protected boolean collectorSupportDiscovery;
	/**
	 * Holds template negotiation capability value
	 */
	protected boolean collectorTemplateNegotiation;
	/**
	 * Holds default protocol version value
	 */
	protected int defaultProtocolVersion;
	// Exporters
	/**
	 * Holds list of exporter configurations
	 */
	protected ArrayList exporterConfigList = new ArrayList();
	/**
	 * Holds local port value
	 */
	protected int localPort;
	/**
	 * Holds receive buffer size value
	 */
	protected final int receiveBufferSize = 1024;
	
	/**
	 * Holds strict templates flag value
	 */
	protected boolean strictTemplates;
	//Templates
	/**
	 * Holds list of templates as <code>ArrayList</code>
	 */
	protected ArrayList templatesConfigList = new ArrayList();
	/**
	 * Holds UDP discovery port value
	 */
	protected int udpDiscoveryPort = 10000;
	/**
	 * Holds UDP discovery timeout value
	 */
	protected int udpDiscoveryTimeout;
	/**
	 * Holds vendor id value
	 */
	protected String vendorId;
	
	/**
	 * Holds crane support flag
	 */
	protected boolean supportCRANE;

	/**
	 * @see org.ipdr.api.configuration.CollectorConfigI#findExporter(InetAddress)
	 */
	public ExporterConfigI findExporter(InetAddress address)
	{
		for ( int i = 0; i < exporterConfigList.size(); i ++ )
		{
			ExporterConfigI exporter = (ExporterConfigI) exporterConfigList.get(i);
			if ( exporter.getExporterAddress().equals(address) )
				return exporter;
		}
		return null;
	}
    /**
     * @see org.ipdr.api.configuration.CollectorConfigI#findTemplate(int)
     */
    public TemplateConfig findTemplate( int id )
    {
		for ( int i = 0; i < templatesConfigList.size(); i ++ )
		{
			TemplateConfig template = (TemplateConfig) templatesConfigList.get(i);
			if ( template.getTemplateId() == id )
				return template;
		}
		return null;
    }

	/**
	 * @see org.ipdr.api.configuration.CollectorConfigI#getAcceptTimeout()
	 */
	public int getAcceptTimeout()
	{
		return acceptTimeout;
	}

	/**
	 * @see org.ipdr.api.configuration.CollectorConfigI#getBacklogSize()
	 */
	public int getBacklogSize()
	{
		return backLogSize;
	}

	/**
	 * @see org.ipdr.api.configuration.CollectorConfigI#getCapabilitesTemplateNegotiation()
	 */
	public boolean getCapabilitesTemplateNegotiation()
	{
		return this.collectorTemplateNegotiation;
	}

	/**
	 * @see org.ipdr.api.configuration.CollectorConfigI#getCapabilitiesMultiSession()
	 */
	public boolean getCapabilitiesMultiSession()
	{
		return true;
	}

	/**
	 * @see org.ipdr.api.configuration.CollectorConfigI#getCapabilitiesStructures()
	 */
	public boolean getCapabilitiesStructures()
	{
		return false;
	}
	/**
	 * @see org.ipdr.api.configuration.CollectorConfigI#getCollectorAddress()
	 */
	public Inet4Address getCollectorAddress()
	{
		return this.collectorAddress;
	}
    /**
     * @see org.ipdr.api.configuration.CollectorConfigI#getCollectorProtocolVersion()
     */
    public int getCollectorProtocolVersion()
    {
    	return this.collectorProtocolVersion;
    }
	/**
	 * @see org.ipdr.api.configuration.CollectorConfigI#getCollectorStartInterval()
	 */
	public int getCollectorStartInterval()
	{
		return this.collectorStartInterval;
	}
    /**
     * @see org.ipdr.api.configuration.CollectorConfigI#getCollectorSupportDiscovery()
     */
    public boolean getCollectorSupportDiscovery()
    {
    	return this.collectorSupportDiscovery;
    }
	
	/**
	 * @see org.ipdr.api.configuration.CollectorConfigI#getExporterAt(int)
	 */
	public ExporterConfigI getExporterAt(int position)
	{
		if ( position >= 0 && position < exporterConfigList.size())
			return (ExporterConfigI)exporterConfigList.get(position);
		return null;
	}

	/**
	 * @see org.ipdr.api.configuration.CollectorConfigI#getExporterListLength()
	 */
	public int getExporterListLength()
	{
		return exporterConfigList.size();
	}

	/**
	 * @see org.ipdr.api.configuration.CollectorConfigI#getLocalPort()
	 */
	public int getLocalPort()
	{
		return localPort;
	}

	/**
	 * @see org.ipdr.api.configuration.CollectorConfigI#getReceiveBufferSize()
	 */
	public int getReceiveBufferSize()
	{
		return receiveBufferSize;
	}
	
    /**
     * @see org.ipdr.api.configuration.CollectorConfigI#getTemplateAt(int)
     */
    public TemplateConfig getTemplateAt(int position)
    {
		return (TemplateConfig) templatesConfigList.get(position);
	}
    
    /**
     * @see org.ipdr.api.configuration.CollectorConfigI#getTemplateListLength()
     */
    public int getTemplateListLength()
    {
    	return templatesConfigList.size();
    }

    /**
     * @see org.ipdr.api.configuration.CollectorConfigI#getUDPDiscoveryPort()
     */
    public int getUDPDiscoveryPort()
    {
        return this.udpDiscoveryPort;
    }

    /**
     * @see org.ipdr.api.configuration.CollectorConfigI#getUDPDiscoveryTimeout()
     */
    public int getUDPDiscoveryTimeout()
    {
        return udpDiscoveryTimeout;
    }

	/**
	 * @see org.ipdr.api.configuration.CollectorConfigI#getVendorId()
	 */
	public String getVendorId()
	{
		return vendorId;
	}
	
    /**
     * @see org.ipdr.api.configuration.CollectorConfigI#isStrictTemplates()
     */
    public boolean isStrictTemplates()
    {
        return this.strictTemplates;
    }
    /**
     * The template is looked up by id.
     * <code>equals()</code> method of <code>TemplateConfig</code> is then used to check if the templates are the same
     * @see org.ipdr.api.configuration.CollectorConfigI#isTemplateDefined(org.ipdr.api.configuration.TemplateConfig)
     */
    public boolean isTemplateDefined(TemplateConfig template)
    {
        TemplateConfig configuredTemplate = findTemplate(template.getId());
        if ( configuredTemplate == null )
            return false;
        return (configuredTemplate.equals(template));
    }
    /**
     * @see org.ipdr.api.configuration.CollectorConfigI#getSupportCRANE()
     */
    public boolean getSupportCRANE()
    {
        return supportCRANE;
    }
}
