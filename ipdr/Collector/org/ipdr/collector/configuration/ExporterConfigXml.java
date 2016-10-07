package org.ipdr.collector.configuration;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.net.Inet4Address;
import java.net.UnknownHostException;
import java.util.List;

import org.ipdr.api.configuration.CollectorConfigI;
import org.ipdr.api.configuration.ConfigException;
import org.ipdr.api.configuration.ExporterConfigBase;
import org.ipdr.utility.XMLUtility;
import org.jdom.DataConversionException;
import org.jdom.Element;

/**
 * Implements <code>ExporterConfigI</code>, reads the configuration from an XML config file
 */
public class ExporterConfigXml extends ExporterConfigBase
{

    /**
     * Holds list of connection configuration elements
     */
    List connections;

    /**
     * Corresponds to &lt;Exporter&gt; configuration element
     */
    Element exporter;

    /**
     * Constructs an <code>ExporterConfigXml</code> with the given collector configuration
     * from an XML element 
     * @param collectorConfig Collector configuration
     * @param exporter &lt;Exporter&gt; XML element
     * @throws ConfigException
     * @throws UnknownHostException
     * @throws DataConversionException
     */
    public ExporterConfigXml(CollectorConfigI collectorConfig, Element exporter)
            throws ConfigException, UnknownHostException,
            DataConversionException
    {
        super(collectorConfig);
        this.exporter = exporter;
        fillExporterData();
        connections = exporter.getChildren("Connection");
        if (connections.size() == 0)
                throw new ConfigException("Invalid config file");
        fillConnectionsData();
    }

    /**
     * Fills connections data from &lt;Connection&gt; elements
     * @throws DataConversionException
     * @throws ConfigException
     */
    private void fillConnectionsData() throws DataConversionException,
            ConfigException
    {
        for (int i = 0; i < connections.size(); i++)
        {
            this.connectionConfigList.add(new ConnectionConfigXml(this,
                    (Element) connections.get(i)));
        }
    }

    /**
     * Fills exporter data from &lt;Exporter&gt; element attributes
     * @throws DataConversionException
     * @throws UnknownHostException
     * @throws ConfigException
     */
    private void fillExporterData() throws DataConversionException,
            UnknownHostException, ConfigException
    {
        this.exporterAddress = (Inet4Address) Inet4Address.getByName(XMLUtility
                .getMandatoryStringAttribute(exporter, "IP"));
        this.udpDiscoveryPort = XMLUtility.getOptionalIntAttribute(exporter,
                "discoveryPort", 0);
    }
}