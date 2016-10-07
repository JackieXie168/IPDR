package org.ipdr.collector;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.io.File;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;

import org.ipdr.api.TemplateNegotiator;
import org.ipdr.api.TemplateNegotiatorFactory;
import org.ipdr.api.configuration.ConfigException;
import org.ipdr.api.configuration.ExporterConfigI;
import org.ipdr.api.configuration.FieldConfig;
import org.ipdr.api.configuration.TemplateConfig;
import org.jdom.DataConversionException;
import org.jdom.Document;
import org.jdom.Element;
import org.jdom.JDOMException;
import org.jdom.input.SAXBuilder;

/**
 * Factory for <code>SimpleTemplateNegotiator</code>. The object reads template configuration from an XML
 * file of the following format:
 * <xmp>
 * 		<Templates>
 *		<Template id="TEMPLATE ID">
 *			<Description>DESCRIPTION</Description>
 *			<Fields>
 *				<Field keyId="135" fieldUsed="true" keyName="myKey1" keyTypeId="3"></Field>
 *				<Field keyId="138" fieldUsed="false" keyName="myKey4" keyTypeId="4"></Field>
 *			</Fields>
 *		</Template>
 * </xmp>
 * Each &lt;Template&gt; tag corresponds to a single template. The tag must have an <code>id</code> attribute,
 * which should hold numeric value of the template id.
 * Under the &lt;Template&gt;, tags &lt;Description&gt; and &lt;Fields&gt; are expected to be found.
 * The &lt;Description&gt; holds the verbal description of the template.
 * The &lt;Fields&gt; tag holds enumeration of template fields.
 * Each field is represented by a &lt;Field&gt; tag. Each such tag must have the following attributes:
 * <dl>
 * <dt>keyId</dt><dd>Numeric id of the field</dd>
 * <dt>fieldUsed</dt><dd>Is the field enabled for output by default</dd>
 * <dt>keyName</dt><dd>Name of the field</dd>
 * <dt>keyTypeId</dt><dd>Type id of the field</dd>
 * </dl>  
 * */
public class SimpleTemplateNegotiatorFactory
	implements TemplateNegotiatorFactory
{
    /**
     * The <code>Hashtable</code> maps template id's to <code>TemplateConfig</code> objects
     */
    protected Hashtable templateMap = new Hashtable();
    
    /**
     * The contstructor attempts load of templates from XML file with the given file name.
     * @param fileName Name of the file to load templates from
     * @throws JDOMException
     * @throws ConfigException
     */
    public SimpleTemplateNegotiatorFactory( String fileName ) throws JDOMException, ConfigException
    {
		// Build the document with SAX and Xerces, no validation
		SAXBuilder builder = new SAXBuilder();
		// Create the document
		Document document = builder.build(new File(fileName));
		// get root element
		Element root = document.getRootElement();
		if (root == null)
			throw new ConfigException("Invalid config file");
		// get <Template> elements
		buildTemplateHash(root);
    }
    
    public SimpleTemplateNegotiatorFactory(){}
    
    
	/**
     * @param root
     * @throws DataConversionException
     * @throws ConfigException
     */
    protected void buildTemplateHash(Element root) throws DataConversionException, ConfigException
    {
        List templateConfigs = root.getChildren();
		
		for (Iterator tIt = templateConfigs.iterator(); tIt.hasNext(); )
		{
		    Element templateConfig = (Element) tIt.next();
		    TemplateConfig template = new TemplateConfig();
		    template.setId((short)templateConfig.getAttribute("id").getIntValue());
            List fieldConfigs = templateConfig.getChild("Fields").getChildren(
            "Field");
            
            ArrayList fields = new ArrayList();
		    for (int j = 0; j < fieldConfigs.size(); j++)
		    {
		        // get "Field" element
		        Element fieldElt = (Element) fieldConfigs.get(j);
		        FieldConfig field = new FieldConfig(fieldElt.getAttribute(
		                "keyId").getIntValue(), fieldElt.getAttribute(
		                "keyTypeId").getIntValue(), fieldElt
		                .getAttributeValue("keyName"), fieldElt.getAttribute(
		                "fieldUsed").getBooleanValue());
		        // add fieldConfig element to template data
		        fields.add(field);
		    }
		    template.setFields(fields);
		    template.validateSelf();
		    templateMap.put(new Short(template.getId()), template);
		}
    }


    /**
	 * @see org.ipdr.api.TemplateNegotiatorFactory#getInstance()
	 */
	public TemplateNegotiator getInstance()
	{
		return new SimpleTemplateNegotiator(templateMap);
	}

	
	/* (non-Javadoc)
	 * @see org.ipdr.api.TemplateNegotiatorFactory#getInstance(org.ipdr.api.configuration.ExporterConfigI)
	 */
	public TemplateNegotiator getInstance(ExporterConfigI exporterConfig) {
		return new SimpleTemplateNegotiator(exporterConfig);
	}
}
