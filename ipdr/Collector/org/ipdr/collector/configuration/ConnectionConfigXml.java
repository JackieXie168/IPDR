package org.ipdr.collector.configuration;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import org.ipdr.api.configuration.ConfigException;
import org.ipdr.api.configuration.ConnectionConfigBase;
import org.ipdr.api.configuration.ConnectionConfigI;
import org.ipdr.api.configuration.ExporterConfigI;
import org.ipdr.utility.XMLUtility;
import org.jdom.DataConversionException;
import org.jdom.Element;
/**
 * Implements <code>ConnectionConfigI</code>, reads the configuration from an XML config file
 */
public class ConnectionConfigXml extends ConnectionConfigBase
{

	/**
	 * Holds &lt;Connection&gt; element
	 */
	Element connection;
	
	/**
	 * Holds list of &lt;Session&gt; elements
	 */
	Element sessions;

	/**
	 * Holds default value for retry interval
	 */
	private final int DEFAULT_RETRY_INTERVAL = 5;
	
	/**
	 * Initializes the class with given exporter configuration and &lt;Connection&gt; element 
	 * @param exporter Exporter configuration
	 * @param connection &lt;Connection&gt; element
	 * @throws DataConversionException
	 * @throws ConfigException
	 */
	public ConnectionConfigXml(ExporterConfigI exporter, Element connection)
		throws DataConversionException, ConfigException
	{
		super(exporter);
		this.connection = connection;
		fillConnectionData();
		sessions = this.connection.getChild("Sessions");
		if (sessions != null)
//			throw new ConfigException("Invalid config file: there is no 'Sessions' element");
		    fillSessionsData();
	}

	/**
	 * Fills connection data from the &lt;Connection&gt; element
	 * @throws DataConversionException
	 * @throws ConfigException
	 */
	private void fillConnectionData() throws DataConversionException, ConfigException
	{
		this.connectionPort = XMLUtility.getMandatoryIntAttribute(connection,"port");
		
		this.connectionCount = XMLUtility.getOptionalIntAttribute(connection,"count", 0);
		
		this.connectTimeout = XMLUtility.getMandatoryIntAttribute(connection, "connectTimeOut");
		
		this.keepAliveInterval = XMLUtility.getMandatoryIntAttribute(connection, "keepAlive");
		
		this.passiveConnection = XMLUtility.getMandatoryStringAttribute(connection, "type").equals("passive");
		
		this.retryCount = XMLUtility.getOptionalIntAttribute(connection, "retryCount", ConnectionConfigI.INFINITE);
		
		this.retryInterval = XMLUtility.getOptionalIntAttribute(connection, "retryInterval", DEFAULT_RETRY_INTERVAL);
		
		this.connectionDescription = connection.getChildText("Description");
	}

	/**
	 * Fills session data from &lt;Sessions&gt; element
	 * @throws DataConversionException
	 * @throws ConfigException
	 */
	private void fillSessionsData()
		throws DataConversionException, ConfigException
	{
		java.util.List sessions = this.sessions.getChildren();
		for (int i = 0; i < sessions.size(); i++)
		{
			SessionConfigXml session =
				new SessionConfigXml(this, (Element) sessions.get(i));
			this.sessionList.add(session);
		}
	}

}
