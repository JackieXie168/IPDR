package org.ipdr.collector.configuration;

import java.util.Iterator;
import java.util.List;

import org.ipdr.api.configuration.ConfigException;
import org.ipdr.api.configuration.ConnectionConfigI;
import org.ipdr.api.configuration.FieldConfigI;
import org.ipdr.api.configuration.SessionConfigBase;
import org.ipdr.api.configuration.TemplateConfig;
import org.ipdr.utility.XMLUtility;
import org.jdom.DataConversionException;
import org.jdom.Element;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/**
 * Implements <code>SessionConfigI</code>, reads the configuration from an XML config file
 */
public class SessionConfigXml extends SessionConfigBase
{
    /**
     * Holds &lt;Session&gt; element
     */
    Element session;

    /**
     * Holds &lt;TemplateRefs&gt; element
     */
    Element templates;

    /**
     * Constructs an <code>SessionConfigXml</code> with the given collector configuration
     * from an XML element 
     * @param connection Connection configuration
     * @param sessionElt &lt;Session&gt; XML element
     * @throws DataConversionException
     * @throws ConfigException
     */
    public SessionConfigXml(ConnectionConfigI connection, Element sessionElt)
            throws DataConversionException, ConfigException
    {
        super(connection);
        this.session = sessionElt;
        fillSessionData();
        // get "Templates" elements
        templates = this.session.getChild("TemplateRefs");
        if (templates == null)
                throw new ConfigException("Invalid config file");
        fillTemplatesData();
    }

    /**
     * Fills session data from &lt;Session&gt; element
     */
    private void fillSessionData()
    {
        this.sessionId = Byte.parseByte(session.getAttributeValue("id"));
        this.sessionName = session.getAttributeValue("name");
        this.sessionDescription = session.getChildText("Description");
    }

    /**
     * Fills templates from &lt;Templates&gt;, looking them up in the global
     * template list
     * @throws DataConversionException
     * @throws ConfigException
     */
    private void fillTemplatesData() throws DataConversionException,
            ConfigException
    {
        // get list of config templates
        CollectorConfigXml config = (CollectorConfigXml) (this.connection.getCollectorConfig());
        java.util.List templates = this.templates.getChildren("TemplateRef");
        for (int i = 0; i < templates.size(); i++)
        {
            Element templateElt = (Element) templates.get(i);
            int ref = templateElt.getAttribute("ref").getIntValue();
            // fint template with id = ref in configTemplates
            TemplateConfig template = null;
            try
            {
                template = (TemplateConfig) config.findTemplate(ref).clone();
            }
            catch (CloneNotSupportedException e)
            {
            }
            // find template negotiation mask - if exists 
            if (template == null)
                throw new ConfigException("Invalid template reference");
            Element templateConfiguration = templateElt.getChild("TemplateConfig");
            if (templateConfiguration != null)
            {
                // apply the unique session fields setup to the template clone
                List fieldSetup = templateConfiguration.getChildren("FieldConfig");
                Iterator itFieldSetup = fieldSetup.iterator();
                while(itFieldSetup.hasNext())
                {
                    Element fieldSetupElt = (Element)itFieldSetup.next();
                    int fieldId = XMLUtility.getMandatoryIntAttribute(fieldSetupElt, "keyId");
                    boolean fieldState = XMLUtility.getMandatoryBooleanAttribute(fieldSetupElt, "fieldUsed");
                    FieldConfigI fieldConfig = template.getFieldById(fieldId);
                    fieldConfig.setEnabledForOutput(fieldState);
                }
            }   
            this.sessionTemplatesList.add(template);
        }
    }
}