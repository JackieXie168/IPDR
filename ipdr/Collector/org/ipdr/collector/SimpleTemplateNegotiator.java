package org.ipdr.collector;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.util.ArrayList;
import java.util.Hashtable;
import java.util.List;

import org.ipdr.api.TemplateNegotiator;
import org.ipdr.api.configuration.ExporterConfigI;
import org.ipdr.api.configuration.FieldConfig;
import org.ipdr.api.configuration.TemplateConfig;
import org.ipdr.collector.configuration.CollectorConfigXml;
import org.jdom.DataConversionException;
import org.jdom.Element;

/**
 * Simple template negotiator implementation.
 *
 */
public class SimpleTemplateNegotiator implements TemplateNegotiator
{
    /**
     * Holds array list of fixed templates to be returned as required template set
     */
    protected Hashtable fixedTemplateSet = new Hashtable();

    protected ExporterConfigI exporterConfig;
    /**
     * Instantiates <code>SimpleTemplateNegotiator</code> with a predefined set of templates
     * which will be always sent on negotiation request.
     * @param templateMap the <code>Hashtable</code> of templates to return on every request
     */
    public SimpleTemplateNegotiator(Hashtable templateMap)
    {
        this.fixedTemplateSet = templateMap;
    }
    
    public SimpleTemplateNegotiator(ExporterConfigI exporterConfig)
    {
    	this.exporterConfig = exporterConfig;
    }

	/**
	 * Always agrees to a non-negotiable set of templates
	 * @see org.ipdr.api.TemplateNegotiator#isTemplateDataAcceptable(ArrayList)
	 */
	public boolean isTemplateDataAcceptable(ArrayList templates)
	{
		return true;
	}

	/**
	 * Returns pre-defined set of templates (provided by the factory)
	 * @see org.ipdr.api.TemplateNegotiator#negotiateTemplate(byte, ArrayList)
	 */
	public ArrayList negotiateTemplate(
			byte sessionId,
			ArrayList incomingTemplates)
	{
		ArrayList result = new ArrayList();
		// Get Exporter's IP to compare to all exporters in the configuration
		String exporterIP = exporterConfig.getExporterAddress().toString();
		exporterIP = exporterIP.substring(1);
		// Get Collector XML representation class to extract Elements
		CollectorConfigXml collectorXml = (CollectorConfigXml) exporterConfig.getCollectorConfig();
		Element exportersElement = collectorXml.getExporterConfigs();
		List exporterList = exportersElement.getChildren("Exporter");
		for (int i=0; i< exporterList.size(); i++)
		{
			// find the correct exporter that contains the needed session
			String exporterXmlIP = ((Element) exporterList.get(i)).getAttribute("IP").getValue();
			if (exporterIP.equals(exporterXmlIP))
			{
				List connectionList = ((Element) exporterList.get(i)).getChildren("Connection");
				for (int j=0; j<connectionList.size(); j++)
				{
					Element sessionsElement = ((Element) connectionList.get(j)).getChild("Sessions");
					if (sessionsElement != null)
					{
						List sessionList = sessionsElement.getChildren("Session");
						for (int k=0; k<sessionList.size(); k++)
						{
							// compare wanted sessionId to checked session
							if (Byte.parseByte(((Element) sessionList.get(k)).getAttribute("id").getValue()) == sessionId)
							{
								// We found the correct session, now lets get the templates
								Element templateRefsElement = ((Element) sessionList.get(k)).getChild("TemplateRefs");
								if (templateRefsElement != null)
								{
									List templateRefList = templateRefsElement.getChildren("TemplateRef");
									for (int l=0; l<templateRefList.size(); l++)
									{
										try
										{
											buildTemplateHash((Element) templateRefList.get(l));
										}
										/*
										catch (ConfigException e1)
										{
											
										}
										*/
										catch (DataConversionException e2)
										{
											
										}
									}
								}
							}
						}
					}
				}
			}
		}
		return negotiateTemplateHelper(incomingTemplates);
	}
	
	protected void buildTemplateHash(Element templateConfig) throws DataConversionException/*, ConfigException*/
    {
		TemplateConfig template = new TemplateConfig();
		Element templateConfigElement = templateConfig.getChild("TemplateConfig");
		template.setId((short)templateConfig.getAttribute("ref").getIntValue());
		List fieldConfigs = templateConfigElement.getChildren("FieldConfig");

		ArrayList fields = new ArrayList();
		for (int j = 0; j < fieldConfigs.size(); j++)
		{
			// get "FieldConfig" element
			Element fieldElt = (Element) fieldConfigs.get(j);
			FieldConfig field = new FieldConfig(fieldElt.getAttribute(
					"keyId").getIntValue(), 0, "", fieldElt.getAttribute(
									"fieldUsed").getBooleanValue());
			// add fieldConfig element to template data
			fields.add(field);
		}
		template.setFields(fields);
		//template.validateSelf();
		fixedTemplateSet.put(new Short(template.getId()), template);
    }
	
	protected ArrayList negotiateTemplateHelper(ArrayList incomingTemplates)
	{
		ArrayList result = new ArrayList();
		for(int i = 0; i < incomingTemplates.size(); i ++)
		{
			TemplateConfig template = (TemplateConfig)incomingTemplates.get(i);
			TemplateConfig storedTemplate;
			Short templateId = new Short(template.getId());

			storedTemplate = (TemplateConfig)fixedTemplateSet.get(templateId);
			if (storedTemplate != null)
			{
				for (int j=0; j<storedTemplate.getFieldListLength(); j++)
				{
					ArrayList storedTemplateFieldList = storedTemplate.getFieldList();
					FieldConfig storedTemplateField = (FieldConfig) storedTemplateFieldList.get(j); 
					boolean storedTemplateFieldEnable = storedTemplateField.isEnabledForOutput();
					int storedTemplateFieldId = storedTemplateField.getId();
					FieldConfig templateField = (FieldConfig) template.getFieldById(storedTemplateFieldId);
					templateField.setEnabledForOutput(storedTemplateFieldEnable);				
				}
			}			
			result.add(template);
		}
		if (result.size()== 0)
			return null;
		return result;
	}
/* OLD implementation */
/*	
	public ArrayList negotiateTemplate(
		byte sessionId,
		ArrayList incomingTemplates)
	{
	    ArrayList result = new ArrayList();
	    for(int i = 0; i < incomingTemplates.size(); i ++)
	    {
	        TemplateConfig template = (TemplateConfig)incomingTemplates.get(i);
	        TemplateConfig storedTemplate;
	        Short templateId = new Short(template.getId());
	        
	        storedTemplate = (TemplateConfig)fixedTemplateSet.get(templateId);
	        if (storedTemplate != null)
	            result.add(storedTemplate);
	    }
	    if (result.size()== 0)
	        return null;
	    return result;
	}
*/
}
