package org.ipdr.collector.configuration;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.io.File;
import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.List;

import org.ipdr.api.configuration.CollectorConfigBase;
import org.ipdr.api.configuration.ConfigException;
import org.ipdr.api.configuration.FieldConfig;
import org.ipdr.api.configuration.TemplateConfig;
import org.ipdr.utility.XMLUtility;
import org.jdom.DataConversionException;
import org.jdom.Document;
import org.jdom.Element;
import org.jdom.JDOMException;
import org.jdom.input.SAXBuilder;

/**
 * The class contains an implementation of <code>CollectorConfigI</code>,
 * which reads the configuration from an XML file. The xml file has the
 * following structure: 
 * <xmp>
 * 	<IPDRcollectorConfiguration xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"> 
 * 		<Collector IP="10.1.100.81" templateNegotiation="true" supportDiscovery="false" port="4737" startInterval="30" protocolVersion="2" vendor="IPDR" acceptTimeout="30" strictTemplates="false" udpDiscoveryTimeout="3" udpDiscoveryPort="4737"/> 
 * 			<Exporters>
 * 				<Exporter IP="10.1.100.14"> 
 * 					<Connection type="passive" count="1" connectTimeOut="100" keepAlive="20" port="15200" retryCount="1" retryInterval="1">
 * 						<Description>Connection's description </Description> 
 * 						<Sessions>
 * 							<Session id="1" name="mySession1"> 
 * 								<Description>Session's description1 </Description>
 * 								<TemplateRefs>
 * 									<TemplateRef ref="1">
 *										<TemplateConfig>
 *											<FieldConfig keyId="101" fieldUsed="false"/>
 *											<FieldConfig keyId="102" fieldUsed="true"/>
 *											<FieldConfig keyId="103" fieldUsed="true"/>
 *										</TemplateConfig>
 * 										
 * 									</TemplateRef> 
 * 								</TemplateRefs> 
 * 							</Session> 
 * 						</Sessions>
 * 					</Connection> 
 * 				</Exporter> 
 * 			</Exporters> 
 * 			<Templates>
 * 				<Template globalRef="111" id="1"> 
 * 					<Description>Template's description 1 </Description> 
 * 					<Fields>
 * 						<Field keyId="101" fieldUsed="true" keyName="myKey1" keyTypeId="33"></Field>
 *						<Field keyId="102" fieldUsed="true" keyName="myKey2" keyTypeId="33"></Field>
 *						<Field keyId="103" fieldUsed="true" keyName="myKey3" keyTypeId="40"></Field>
 *					</Fields> 
 * 				</Template> 
 * 			</Templates> 
 * 		</IPDRcollectorConfiguration> 
 * </xmp> 
 * The configuration file has three top-level tags: &lt;Collector&gt; for collector
 * configuration, &lt;Exporters&gt; for exporter configuration,
 * &lt;Templates&gt; for templates configuration. The collector configuration is
 * in the attributes of &lt;Collector&gt; element:
 * <dl>
 * <dt>IP</dt><dd>Collector IP address</dd>
 * <dt>port</dt><dd>Collector listening TCP port</dd>
 * <dt>templateNegotiation</dt><dd>Template negotiation capability</dd>
 * <dt>supportDiscovery</dt><dd>UDP discovery support</dd>
 * <dt>startInterval</dt><dd>Start interval for connection spawning</dd>
 * <dt>protocolVersion</dt><dd>Collector default protocol version</dd>
 * <dt>vendor</dt><dd>Vendor string</dd>
 * <dt>acceptTimeout</dt><dd>Accept timeout</dd>
 * <dt>strictTemplates</dt><dd>Strict templates flag</dd>
 * <dt>udpDiscoveryTimeout</dt><dd>Timeout for UDP discovery</dd>
 * <dt>udpDiscoveryPort</dt><dd>UDP port for incoming discovery requests</dd>
 * </dl>
 * 
 * Each exporter is represented as a &lt;Exporter&gt; element. It has a single attribute,
 * IP, which contains its IP address. Specific connections of an exporter are configured in child
 * &lt;Connection&gt; elements. Each &lt;Connection&gt; element has the following attributes:
 * <dl>
 * <dt>type</dt><dd>"active" or "passive", according to the connection type</dd>
 * <dt>count</dt><dd>Maximum count for the connection. Only meaningful for passive connections.</dd>
 * <dt>connectTimeOut</dt><dd>Connect timeout</dd>
 * <dt>retryCount</dt><dd>Number of connection attempt retries. Defaults to infinite</dd>
 * <dt>retryInterval</dt><dd>Interval between connection retries</dd>
 * <dt>keepAlive</dt><dd>Expected keepalive timeout</dd>
 * <dt>port</dt><dd>TCP port to connect to. Only meaningful for active connections.</dd>
 * </dl>
 * 
 * Under each &lt;Connection&gt; element, there is a &lt;Description&gt; child element which contains
 * verbal description of the connection.
 * There is also a &lt;Sessions&gt; tag, which contains a list of sessions on this connection. Each such session
 * is represented as &lt;Session&gt; element. The &lt;Session&gt; element has the following attributes:
 * <dl>
 * <dt>id</dt><dd>Numeric id of the session</dd>
 * <dt>name</dt><dd>Name of the session</dd>
 * </dl>
 * Each &lt;Session&gt; has a child &lt;Description&gt; element with verbal description of the session.
 * It also has a child &lt;TemplateRefs&gt; element, which contains template references for negotiation purposes.
 * Each template reference is represented by a &lt;TemplateRef&gt; element, which has the only attribute <code>ref</code>. The
 * attribute holds an id of the template.
 * Each &lt;TemplateRef&gt; has a child &lt;TemplateConfig&gt; element so that for Each field that is referenced the &lt;TemplateConfig&gt; contains a child &lt;FiledConfig&gt; element with the following attributes:
 * <dl>
 * <dt>keyId</dt><dd>Numeric id of the field</dd>
 * <dt>fieldUsed</dt><dd>When negotiated, the collectors decision whether to enabled the field for output</dd>
 * </dl>
 * 
 * The &lt;Templates&gt; top-level element holds description of known templates. Each template is represented by a &lt;Template&gt; element.
 * The tag must have an <code>id</code> attribute, which should hold a numeric value of the template id.
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
 * 
 */
public class CollectorConfigXml extends CollectorConfigBase
{

    /**
     * Holds XML document
     */
    Document document;

    /**
     * Points to the root element of the config file
     */
    protected Element root;

    /**
     * Points to the &lt;Collector&gt; element
     */
    protected Element collector;

    /**
     * Points to the &lt;Exporters&gt; element
     */
    public Element exporterConfigs;
    
    /**
     * Returns exporter Configuration element
     * @return Element
     */
    public Element getExporterConfigs()
    {
    	return exporterConfigs;
    }

    /**
     * Points to the &lt;Templates&gt; element
     */
    Element templateConfigs;

    /**
     * Default value for protocol version
     */
    private final int DEFAULT_PROTOCOL_VERSION = 2;

    /**
     * Default value for accept timeout
     */
    private final int DEFAULT_ACCEPT_TIMEOUT = 5;
    
    /**
     * Default value for controller query interval
     */
    private final int DEFAULT_CONTROLLER_QUERY_INTERVAL = 1000;
    
    
    

    protected static final String XML_COLLECTOR_ELEMENT = "Collector";
    protected static final String XML_EXPORTERS_ELEMENT = "Exporters";
    protected static final String XML_TEMPLATES_ELEMENT = "Templates";

    protected static final String XML_LISTENING_PORT_ATTRIBUTE = "port";
    protected static final String XML_COLLECTOR_IP_ATTRIBUTE = "IP";
    protected static final String XML_START_INTERVAL_ATTRIBUTE = "startInterval";
    protected static final String XML_ACCEPT_TIMEOUT_ATTRIBUTE = "acceptTimeout";
    protected static final String XML_UDP_DISCOVERY_PORT_ATTRIBUTE = "udpDiscoveryPort";
    protected static final String XML_SUPPORT_NEGOTIATION_ATTRIBUTE = "templateNegotiation";
    protected static final String XML_SUPPORT_DISCOVERY_ATTRIBUTE = "supportDiscovery";
    protected static final String XML_STRICT_TEMPLATES_ATTRIBUTE = "strictTemplates";


    /**
     * Initializes the configuration by reading and parsing the XML config file
     * @param fileName configuration file name
     * @throws ConfigException
     * @throws UnknownHostException
     * @throws DataConversionException
     * @throws JDOMException
     */
    public CollectorConfigXml(String fileName) throws ConfigException,
            UnknownHostException, DataConversionException, JDOMException
    {
        // Build the document with SAX and Xerces, no validation
        SAXBuilder builder = new SAXBuilder();
        // Create the document
        Document m_document = builder.build(new File(fileName));
        // get root element
        root = m_document.getRootElement();
        if (root == null) throw new ConfigException("Invalid config file");
        getConfigElements();
    }
    
    protected CollectorConfigXml(){}
    
    /**
     * Initializes this root parameter and fills configuration elements
     * @param root - the root element to set
     * @throws UnknownHostException
     * @throws DataConversionException
     * @throws ConfigException
     */
    public CollectorConfigXml(Element root) throws UnknownHostException, DataConversionException, ConfigException
    {
        this.root = root;
        getConfigElements();
    }

    /**
     * Fills configuration elements
     * @throws ConfigException
     * @throws UnknownHostException
     * @throws DataConversionException
     */
    protected void getConfigElements() throws ConfigException,
            UnknownHostException, DataConversionException
    {
        // get "Collector" element
        collector = root.getChild(XML_COLLECTOR_ELEMENT);
        if (collector == null)
                throw new ConfigException("Invalid config file");
        fillCollectorData();
        // get "Templates" elements
        templateConfigs = root.getChild(XML_TEMPLATES_ELEMENT);
        if (templateConfigs == null)
                throw new ConfigException("Invalid config file");
        fillTemplatesData();
        // get "Exporters" element
        exporterConfigs = root.getChild(XML_EXPORTERS_ELEMENT);
        if (exporterConfigs == null)
                throw new ConfigException("Invalid config file");
        fillExportersData();
    }

    /**
     * Fills collector parameters from &lt;Collector&gt; element
     * @throws DataConversionException
     * @throws UnknownHostException
     * @throws ConfigException
     */
    private void fillCollectorData() throws DataConversionException,
            UnknownHostException, ConfigException
    {
        this.collectorAddress = (Inet4Address) InetAddress.getByName(XMLUtility
                .getMandatoryStringAttribute(collector, XML_COLLECTOR_IP_ATTRIBUTE));

        this.localPort = XMLUtility.getMandatoryIntAttribute(collector, XML_LISTENING_PORT_ATTRIBUTE);

        this.collectorProtocolVersion = XMLUtility.getOptionalIntAttribute(
                collector, "protocolVersion", DEFAULT_PROTOCOL_VERSION);
        
        this.controllerQueryInterval = XMLUtility.getOptionalIntAttribute(collector, "controllerQueryInterval", DEFAULT_CONTROLLER_QUERY_INTERVAL);

        this.collectorStartInterval = XMLUtility.getMandatoryIntAttribute(
                collector, XML_START_INTERVAL_ATTRIBUTE);

        this.collectorSupportDiscovery = XMLUtility
                .getOptionalBooleanAttribute(collector, XML_SUPPORT_DISCOVERY_ATTRIBUTE,
                        false);

        this.collectorTemplateNegotiation = XMLUtility
                .getOptionalBooleanAttribute(collector, XML_SUPPORT_NEGOTIATION_ATTRIBUTE,
                        false);

        this.vendorId = XMLUtility.getMandatoryStringAttribute(collector,
                "vendor");

        this.acceptTimeout = XMLUtility.getOptionalIntAttribute(collector,
                XML_ACCEPT_TIMEOUT_ATTRIBUTE, DEFAULT_ACCEPT_TIMEOUT);

        this.strictTemplates = XMLUtility.getOptionalBooleanAttribute(
                collector, XML_STRICT_TEMPLATES_ATTRIBUTE, false);

        this.udpDiscoveryTimeout = XMLUtility.getMandatoryIntAttribute(
                collector, "udpDiscoveryTimeout");

        this.udpDiscoveryPort = XMLUtility.getMandatoryIntAttribute(collector,
                XML_UDP_DISCOVERY_PORT_ATTRIBUTE);
        
        this.supportCRANE = XMLUtility.getOptionalBooleanAttribute(collector, "supportCRANE", true);
    }

    /**
     * Fills exporter data from &lt;Exporters&gt; element
     * @throws ConfigException
     * @throws UnknownHostException
     * @throws DataConversionException
     */
    private void fillExportersData() throws ConfigException,
            UnknownHostException, DataConversionException
    {
        //exporterConfigList
        java.util.List exporters = exporterConfigs.getChildren();
        for (int i = 0; i < exporters.size(); i++)
        {
            ExporterConfigXml exporter = new ExporterConfigXml(this,
                    (Element) exporters.get(i));
            this.exporterConfigList.add(exporter);
        }
    }

    /**
     * Fills templates from &lt;Templates&gt;
     * @throws DataConversionException
     * @throws ConfigException
     */
    private void fillTemplatesData() throws DataConversionException,
            ConfigException
    {
        java.util.List templates = templateConfigs.getChildren();
        for (int i = 0; i < templates.size(); i++)
        {
            TemplateConfig template = new TemplateConfig();
            // get "Template" element
            Element templateElt = (Element) templates.get(i);
            // fill templateConfig data
            template.setId((short)XMLUtility.getMandatoryIntAttribute(
                    templateElt, "id"));
            template.setTemplateGlobalRef(XMLUtility.getMandatoryIntAttribute(
                    templateElt, "globalRef"));
            template.setTemplateDescription( templateElt
                    .getChildText("Description") );
            // fill "Fields" data
            List fields = templateElt.getChild("Fields").getChildren("Field");
            for (int j = 0; j < fields.size(); j++)
            {
                // get "Field" element
                Element fieldElt = (Element) fields.get(j);
                FieldConfig field = new FieldConfig(XMLUtility
                        .getMandatoryIntAttribute(fieldElt, "keyId"),
                        XMLUtility.getMandatoryIntAttribute(fieldElt,
                                "keyTypeId"), XMLUtility
                                .getMandatoryStringAttribute(fieldElt,
                                        "keyName"), XMLUtility
                                .getMandatoryBooleanAttribute(fieldElt,
                                        "fieldUsed"));
                // add fieldConfig element to template data
                template.addField(field);
            }
            this.templatesConfigList.add(template);
        }
    }

    /**
     * @see java.lang.Object#toString()
     */
    public String toString()
    {
        return collector.toString();
    }


}