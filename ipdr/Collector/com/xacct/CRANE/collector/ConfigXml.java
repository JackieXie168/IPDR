package com.xacct.CRANE.collector;

// $Id: ConfigXml.java,v 1.4 2005/01/30 15:13:42 ilyad Exp $
// Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.ListIterator;

import org.jdom.Attribute;
import org.jdom.Document;
import org.jdom.Element;


/** Converts between XML and CollectorConfig. You can construct this
 *  with either the XML or a configuration, and then read off the other
 *  form.
 *  <p>
 *  The XML uses "references" to templates to reduce the size of the
 *  XML file. However, the configuration objects are kept separately,
 *  so that, for example, a field can be updated without affecting
 *  any other configuration objects.
 *
 *  @version $Id: ConfigXml.java,v 1.4 2005/01/30 15:13:42 ilyad Exp $
 */
public class ConfigXml
{
    /** $Id: ConfigXml.java,v 1.4 2005/01/30 15:13:42 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: ConfigXml.java,v 1.4 2005/01/30 15:13:42 ilyad Exp $"; }

    /** The XML */
    private Document xml = null;
    public final Document getXml() { return xml; }

    /** The configuration in internal form. */
    private CollectorConfig collectorConfig = null;
    public final CollectorConfig getCollectorConfig() { return collectorConfig; }

    private boolean m_isDebugLevelUsed = Constants.DEFAULT_IS_DEBUG_LEVEL_USED;
    public final boolean isDebugLevelUsed() { return m_isDebugLevelUsed; }
    public final void setDebugLevelused(boolean value) { m_isDebugLevelUsed = value; }

    /** Create an empty object.
     *  @see #setXml
     */
    public ConfigXml() { }

    /** Create an object from an XML document.
     *
     * @param xml The XML document.
     * @param fieldFactory For creating fields.
     * @throws ConfigException Bad configuration (shouldn't happen).
     */
    public ConfigXml(Document xml, FieldDescriptorFactory fieldFactory)
        throws ConfigException {
        setXml(xml, fieldFactory);
    }

    /** Set the XML document and create the configuration object from it.
     *
     * @param xml The XML document.
     * @param fieldFactory For creating fields.
     * @throws ConfigException Bad configuration (shouldn't happen).
     */
    public void setXml(Document xml, FieldDescriptorFactory fieldFactory)
        throws ConfigException {
        this.xml = xml;
        xmlToConfig(fieldFactory);
    }

    /** Create an object from the configuration object, creating the
     *  XML document.
     *
     * @param collectorConfig The configuration.
     * @param xsdFileName The XSD file name (goes into the XML document).
     * @param rootElementName The XML root element name.
     * @exception ConfigException if something is wrong the configuration (usually
     *         a program logic error).
     */
    public ConfigXml(CollectorConfig collectorConfig,
                     String xsdFileName,
                     String rootElementName)
    throws ConfigException {
        setCollectorConfig(collectorConfig, xsdFileName, rootElementName);
    }

    /** Create an object from the configuraiton object, filling in
     *  the XML document.
     *
     * @param collectorConfig The configuration.
     * @param xml The XML document with the top-level (root) element filled in.
     * @exception ConfigException if something is wrong the configuration (usually
     *         a program logic error).
     */
    public ConfigXml(CollectorConfig collectorConfig, Document xml) throws ConfigException {
        this.xml = xml;
        configToXml();
    }

    /** Set the configuration object, creating the XML document.
     *
     * @param collectorConfig The configuration.
     * @param xsdFileName The XSD file name (goes into the XML document).
     * @param rootElementName The XML root element name.
     * @exception ConfigException if something is wrong the configuration (usually
     *         a program logic error).
     */
    public void setCollectorConfig(CollectorConfig collectorConfig,
                                   String xsdFileName,
                                   String rootElementName)
    throws ConfigException {
        this.collectorConfig = collectorConfig;
        Element root = new Element(rootElementName);
        root.addAttribute(new Attribute("noNamespaceSchemaLocation", "xsi",
                                        "http://www.w3.org/2001/XMLSchema-instance",
                                        xsdFileName));
        xml = new Document(root);

        configToXml();
    }

    private final void xmlToConfig(FieldDescriptorFactory fieldFactory)
        throws ConfigException {
        Element root = xml.getRootElement();

        String configVersion     = root.getChildText(XML_CONFIGURATION_VERSION).trim();
        String serverIPAddress   = root.getChildText(XML_SERVER_IPADDRESS).trim();
        ExtendedInetAddress serverExtendedAddress = null;
        try {
            serverExtendedAddress = new ExtendedInetAddress(serverIPAddress);
        } catch(UnknownHostException uhe) {
            throw new ConfigException(uhe);
        }
        int serverPort           =  extractInt(root, XML_SERVER_PORT);

        // If the server (collector) IP address isn't given, let's try
        // to give it a real value. This is a bit of a kludge; but we
        // want to ensure that the XML has something in it, for
        // debugging purposes (otherwise, we aren't 100% sure how the
        // address is resolved on multi-homed machines).

        int serverAckInterval    =  extractInt(root, XML_SERVER_ACK_INTERVAL);
        int serverStartInterval  =  extractInt(root, XML_SERVER_START_INTERVAL);
        m_isDebugLevelUsed       = extractBool(root, XML_IS_DEBUG_LEVEL_USED);

        ArrayList connectionConfigList = extractClients(root, fieldFactory);

        collectorConfig = new CollectorConfig(
            configVersion,
            serverExtendedAddress,
            serverPort,
            serverAckInterval,
            serverStartInterval,
            connectionConfigList);

        collectorConfig.setNoDataKeepAliveInterval(extractInt(root, XML_NO_DATA_KEEP_ALIVE_INTERVAL,
                                                              Constants.DEFAULT_NO_DATA_KEEP_ALIVE_INTERVAL));
    }

    /** Extract the clients (transmitters/connections) from the XML
     *  @param root XML element above the clients (transmitters/connections)
     *  @param fieldFactory constructor for fields
     *  @return ConnectionConfigList
     *  @throws ConfigException Something wrong in the XML
     */
    private ArrayList extractClients(Element root,
                                     FieldDescriptorFactory fieldFactory)
        throws ConfigException {
        Element clients = root.getChild(XML_CLIENTS);
        List clientList = (List)clients.getChildren(XML_CLIENT);
        ArrayList connectionConfigList = new ArrayList(clientList.size());

        extractClientList(root, clientList.iterator(),
                          connectionConfigList, fieldFactory);

        return connectionConfigList;
    }

    /** Get the clients (connections/transmitters) into a list.
     *  @param root XML element above the clients (transmitters/transmitters)
     *  @param i clientList.iterator()
     *  @param connectionConfigList output
     *  @param fieldFactory constructor for new fields
     *  @throws ConfigException Something wrong in the XML
     */
    private void extractClientList(Element root, Iterator i,
                                   ArrayList connectionConfigList,
                                   FieldDescriptorFactory fieldFactory)
        throws ConfigException {

        while (i.hasNext()) {
            Element client = (Element)i.next();

            String clientIP                   = extractStr(client, XML_CLIENT_IPADDRESS);
            int clientPort                    = extractInt(client, XML_CLIENT_PORT);
            int clientVersionPort             = extractInt(client, XML_CLIENT_VERSION_PORT);
            String clientVendorString         = extractStr(client, XML_CLIENT_VENDOR_STRING);
            int clientConnectTimeout          = extractInt(client, XML_CLIENT_CONNECT_TIMEOUT);
            int clientReconnectInterval       = extractInt(client, XML_CLIENT_RECONNECT_INTERVAL);
            int clientConnectRetryTimeout     = extractInt(client, XML_CLIENT_CONNECT_RETRY_TIMEOUT);
            int clientConnectRoundtripTimeout = extractInt(client, XML_CLIENT_CONNECT_ROUNDTRIP_TIMEOUT);
            int clientConnectDataLogInterval  = extractInt(client, XML_CLIENT_ACK_LOG_INTERVAL);
            String clientDescription          = extractStr(client, XML_CLIENT_DESCRIPTION);

            Element sessions= client.getChild(XML_CLIENT_SESSIONS);
            List sessionList = (List)sessions.getChildren(XML_CLIENT_SESSION);
            ArrayList sessionConfigList = new ArrayList(sessionList.size());

            extractSessionList(root, sessionList.iterator(), sessionConfigList, fieldFactory,
                               ExtendedInetAddress.andPortToString(clientIP, clientPort));

            try {
                // Can throw ConfigException:
                connectionConfigList.add(
                    new ConnectionConfig(
                        new ExtendedInetAddress(clientIP), // transmitter (client) address
                        clientPort,
                        clientVersionPort,
                        clientVendorString,
                        clientConnectTimeout,
                        clientReconnectInterval,
                        clientConnectRetryTimeout,
                        clientConnectRoundtripTimeout,
                        clientConnectDataLogInterval,
                        sessionConfigList));
            } catch (UnknownHostException uhe) {
                throw new ConfigException(uhe);
            }
        }
    }

    /** Get the sessions into a list.
     *  @param root XML element above the clients (transmitters/transmitters)
     *  @param i sessionList.iterator()
     *  @param connectionConfigList output
     *  @param fieldFactory constructor for fields
     *  @param clientDescr string describing client (IP:port)
     *  @throws ConfigException Something wrong in the XML
     */
    private void extractSessionList(Element root, Iterator j,
                                    ArrayList sessionConfigList,
                                    FieldDescriptorFactory fieldFactory,
                                    String clientDescr)
        throws ConfigException {
        while (j.hasNext()) {
            Element session = (Element)j.next();

            int sessionId      = extractInt(session, XML_CLIENT_SESSION_ID);
            String sessionName = extractStr(session, XML_CLIENT_SESSION_NAME);
            String sessionDesc = extractStr(session, XML_CLIENT_SESSION_DESCRIPTION);

            Element sessionTemplates = session.getChild(XML_CLIENT_SESSION_TEMPLATES);
            List sessionTemplate = (List)sessionTemplates.getChildren(XML_CLIENT_SESSION_TEMPLATE);

            ArrayList templateConfigList = new ArrayList(sessionTemplate.size());

            extractTemplates(root, sessionId, clientDescr, sessionTemplate.iterator(),
                             templateConfigList, fieldFactory);

            // Can throw ConfigException:
            sessionConfigList.add(
                new SessionConfig(sessionId, sessionName, sessionDesc, templateConfigList));
        }
    }

    /** Get the templates into a list.
     *  @param root XML element above the clients (transmitters/transmitters)
     *  @param sessionId the sesion Id that contains the templates
     *  @param clientDescr string describing client (IP:port)
     *  @param k sessionTemplate.iterator()
     *  @param connectionConfigList output
     *  @param fieldFactory constructor for fields
     *  @throws ConfigException Something wrong in the XML
     */
    private void extractTemplates(Element root, int sessionId, String clientDescr,
                                  Iterator k, ArrayList templateConfigList,
                                  FieldDescriptorFactory fieldFactory)
        throws ConfigException {
        while (k.hasNext()) {
            Element template = (Element)k.next();

            String templateRef = extractStr(template, XML_CLIENT_SESSION_TEMPLATE_REF);
            TemplateConfig extractedTemplate =
                extractSessionTemplate(root, fieldFactory, templateRef);
            if (extractedTemplate == null) {
                throw new ConfigException(
                    "Configuration XML error: template reference '" + templateRef + "'" +
                    " is defined in session id " + sessionId + " of client " +
                    clientDescr +
                    " but not defined in the Templates section of the configuration");
            }
            templateConfigList.add(extractedTemplate);
        }
    }

    /** Extract a template for a session, given the ref
     *  @param root XML element above the clients (transmitters/transmitters)
     *  @param fieldFactory constructor for fields
     *  @param templateRef the template ref value
     *  @return TemplateConfig if found; null if not found
     *  @throws ConfigException Something wrong in the XML
     */
    private TemplateConfig extractSessionTemplate(Element root,
                                                  FieldDescriptorFactory fieldFactory,
                                                  String templateRef)
        throws ConfigException {
        Element templates = root.getChild(XML_TEMPLATES);
        List templateList = (List)templates.getChildren(XML_TEMPLATE);
        ArrayList templateConfigList = new ArrayList(templateList.size());

        for (Iterator i = templateList.iterator(); i.hasNext(); ) {
            Element template = (Element)i.next();

            String globalTemplateRef = template.getChildText(XML_GLOBAL_TEMPLATE_REF);
            if (! globalTemplateRef.equals(templateRef)) {
                continue;
            }

            int templateId = extractInt(template, XML_TEMPLATE_ID);

            String templateDescription     =  extractStr(template, XML_TEMPLATE_DESC);
            boolean templateStatusResponse = extractBool(template, XML_TEMPLATE_STATUS_RESPONSE);

            Element templatesFields = template.getChild(XML_CLIENT_SESSION_TEMPLATE_FIELDS);
            List templatesField = (List)templatesFields.getChildren(XML_CLIENT_SESSION_TEMPLATE_FIELD);

            ArrayList fieldList = new ArrayList(templatesField.size());

            extractFields(templatesField.iterator(), fieldList, fieldFactory);

            return new TemplateConfig(templateId, fieldList,
                                      templateDescription, templateStatusResponse);
        }

        return null;
    }

    /** Get the fields into a list.
     *  @param i templatesField.iterator()
     *  @param fieldList output
     *  @param fieldFactory constructor for fields
     *  @throws ConfigException Something wrong in the XML
     */
    private void extractFields(Iterator n, ArrayList fieldList,
                               FieldDescriptorFactory fieldFactory)
        throws ConfigException {
        while (n.hasNext()) {
            Element tmplField = (Element)n.next();
            int fieldId       = extractInt( tmplField, XML_CRANE_KEY_ID);
            String CRANEName  = extractStr( tmplField, XML_CRANE_KEY_NAME);
            String CRANELabel = extractStr( tmplField, XML_CRANE_KEY_LABEL);
            String CRANEHelp  = extractStr( tmplField, XML_CRANE_KEY_HELP);
            int CRANETypeId   = extractInt( tmplField, XML_CRANE_KEY_TYPE);
            boolean isUsed    = extractBool(tmplField, XML_FIELD_USED);

            String xacctLabel = null;
            String xacctType  = null;

            try {
                xacctLabel = extractStr(tmplField, XML_XACCT_FIELD_LABEL);
                xacctType  = extractStr(tmplField, XML_XACCT_FIELD_TYPE);
            } catch(NullPointerException NPE) {
                fieldList.add(new FieldConfig(fieldId, CRANETypeId, CRANELabel,
                                              CRANEHelp, isUsed));
                continue;
            } catch (NumberFormatException NFE) {
                fieldList.add(new FieldConfig(fieldId, CRANETypeId, CRANELabel,
                                              CRANEHelp, isUsed));
                continue;
            }

            FieldDescriptor fd = null;
            try {
                fd = fieldFactory.createFieldDescriptor(
                    FieldType.getFieldType(xacctType),
                    stripFieldNamespace(xacctLabel));

                if (fd == null) {
                    throw new ConfigException("Unable to create field descriptor, the retrieved field was null, "+
                                              "unable to continue extracting fields; field: " +
                                              xacctLabel);
                }
            } catch (InvalidFieldLabelException e) {
                throw new ConfigException("Unable to create field descriptor (invalid field label) for field: " +
                                          xacctLabel + ": " + e);
            } catch (FieldRegistrationException e) {
                throw new ConfigException("Unable to create field descriptor (registration error) for field: " +
                                          xacctLabel + ": " + e);
            }

            fieldList.add(new FieldConfig(fieldId, CRANETypeId, CRANELabel,
                                          CRANEHelp, isUsed, fd));
        }
    }

    /** Get the name of the field without the label.
     * @param label The field label, possibly with leading ":"s
     * @return label with leading "xxx:" stuff stripped off.
     */
    public static final String stripFieldNamespace(String label) {
        int delimIndex = label.lastIndexOf(":");
        return (delimIndex < 0) ? label : label.substring(delimIndex + 1);
    }

    protected static int extractInt(Element root, String elementName)
        throws ConfigException {
        String value = root.getChildText(elementName);
        try {
            return Integer.parseInt(value);
        } catch (NumberFormatException e) {
            throw new ConfigException("Invalid element number for " + elementName +
                                      ": " + value + ": " + e);
        }
    }

    protected static int extractInt(Element root, String elementName, int defaultValue)
        throws ConfigException {
        String value = root.getChildText(elementName);
        try {
            return (value == null) ? defaultValue : Integer.parseInt(value);
        } catch (NumberFormatException e) {
            throw new ConfigException("Invalid element number for " + elementName +
                                      ": " + value + ": " + e);
        }
    }

    protected static String extractStr(Element root, String elementName) {
        return root.getChildText(elementName).trim();
    }

    protected static boolean extractBool(Element root, String elementName) {
        String value = root.getChildText(elementName);
        return Boolean.valueOf(value).booleanValue();
    }

    protected static void addElement(Element root, String elementName, int value) {
        Element element = new Element(elementName);
        element.addContent(String.valueOf(value));
        root.addContent(element);
    }

    protected static void addElement(Element root, String elementName, boolean value) {
        Element element = new Element(elementName);
        element.addContent(String.valueOf(value));
        root.addContent(element);
    }

    protected static void addElement(Element root, String elementName, String value) {
        Element element = new Element(elementName);
        element.addContent(value);
        root.addContent(element);
    }

    protected static Element addNewElement(Element root, String elementName) {
        Element element = new Element(elementName);
        root.addContent(element);
        return element;
    }



    private final void configToXml()
    throws ConfigException {
        Element root = xml.getRootElement();

        Element clientsElement = addNewElement(root, XML_CLIENTS);

        Element templatesElement = addNewElement(root, XML_TEMPLATES);

        addElement(root, XML_CONFIGURATION_VERSION, collectorConfig.getConfigurationVersion());
        addElement(root, XML_SERVER_IPADDRESS,      collectorConfig.getCollectorAddress().getHostAddress());
        addElement(root, XML_SERVER_PORT,           collectorConfig.getCollectorConnectPortId());
        addElement(root, XML_SERVER_ACK_INTERVAL,   collectorConfig.getAckInterval());
        addElement(root, XML_SERVER_START_INTERVAL, collectorConfig.getCollectorStartInterval());
        addElement(root, XML_IS_DEBUG_LEVEL_USED,   m_isDebugLevelUsed);
        addElement(root, XML_NO_DATA_KEEP_ALIVE_INTERVAL, collectorConfig.getNoDataKeepAliveInterval());

        addClientsElement(clientsElement, templatesElement);
    }


    /** Add the clients (transmitters/transmitters) to the XML document, adding templates as needed.
     *  The <Client> elements refer to templates via <TmplRef> elements.
     *  As needed new <Template> elements (with appropriate <GlobalRef>
     *  elements) are added to the <Templates>; and an appropraite
     *  reference is created.
     *
     * @param clientsElement The <Clients> element
     * @param templatesElement The <Templates> element
     */
    private void addClientsElement(Element clientsElement, Element templatesElement)
    throws ConfigException {
        int refCounter = 0;
        // templateToRefId maps TemplateConfig.getReferenceString() to reference id (from refCounter)
        HashMap templateToRefId = new HashMap();

        for (ListIterator connectionIter = collectorConfig.getConnectionConfigIterator();
             connectionIter.hasNext(); ) {
            ConnectionConfig cc = (ConnectionConfig)connectionIter.next();
            Element clientElement = addNewElement(clientsElement, XML_CLIENT);

            addElement(clientElement, XML_CLIENT_IPADDRESS,                 cc.getTransmitterAddress().getHostAddress());
            addElement(clientElement, XML_CLIENT_PORT,                      cc.getTransmitterPort());
            addElement(clientElement, XML_CLIENT_VERSION_PORT,              cc.getTransmitterVersionPort());
            addElement(clientElement, XML_CLIENT_VENDOR_STRING,             cc.getTransmitterVendorString());
            addElement(clientElement, XML_CLIENT_CONNECT_TIMEOUT,           cc.getConnectTimeout());
            addElement(clientElement, XML_CLIENT_CONNECT_RETRY_TIMEOUT,     cc.getConnectRetryTimeout());
            addElement(clientElement, XML_CLIENT_RECONNECT_INTERVAL,        cc.getReconnectInterval());
            addElement(clientElement, XML_CLIENT_CONNECT_ROUNDTRIP_TIMEOUT, cc.getConnectRoundtripTimeout());
            addElement(clientElement, XML_CLIENT_ACK_LOG_INTERVAL,          cc.getDataAckLogInterval());
            addElement(clientElement, XML_CLIENT_DESCRIPTION,               "" /*no method yet*/);

            Element sessionsElement = addNewElement(clientElement, XML_CLIENT_SESSIONS);

            for (ListIterator j = cc.getSessionConfigIterator(); j.hasNext(); ) {
                SessionConfig sc = (SessionConfig)j.next();

                Element sessionElement = addNewElement(sessionsElement, XML_CLIENT_SESSION);

                addElement(sessionElement, XML_CLIENT_SESSION_ID,           sc.getId());
                addElement(sessionElement, XML_CLIENT_SESSION_NAME,         sc.getName()) ;
                addElement(sessionElement, XML_CLIENT_SESSION_DESCRIPTION,  sc.getDescription());
                Element sessionTemplates = addNewElement(sessionElement, XML_CLIENT_SESSION_TEMPLATES);

                // adding this template's template information (template id and 'used fields' info)
                for (ListIterator k = sc.getTemplateConfigIterator(); k.hasNext(); ) {
                    TemplateConfig tc =  (TemplateConfig)k.next();

                    String templateRefString = tc.getReferenceString();
                    String templateRefId = (String)templateToRefId.get(templateRefString);

                    if (templateRefId == null) {
                        addTemplate(++refCounter, templateRefString, tc, templateToRefId,
                                    templatesElement);
                    }
                    Element sessionTemplate = addNewElement(sessionTemplates, XML_CLIENT_SESSION_TEMPLATE);
                    addElement(sessionTemplate, XML_CLIENT_SESSION_TEMPLATE_REF, refCounter);
                }
            }
        }

        refCounter = addUnusedTemplates(refCounter, collectorConfig.getConnectionConfigIterator(),
                                        templateToRefId, templatesElement);
    }

    private int addUnusedTemplates(int refCounter, ListIterator connectionIter,
                                   HashMap templateToRefId, Element templatesElement)
    throws ConfigException {
        while (connectionIter.hasNext()) {
            ConnectionConfig cc = (ConnectionConfig)connectionIter.next();
            for (ListIterator sessionIter = cc.getSessionConfigIterator();
                 sessionIter.hasNext(); ) {
                SessionConfig sc = (SessionConfig)sessionIter.next();
                for (ListIterator tmplIter = sc.getTemplateConfigIterator();
                     tmplIter.hasNext(); ) {
                    TemplateConfig tmpl = (TemplateConfig)tmplIter.next();
                    if (tmpl != null) {
                        String templateRefString = tmpl.getReferenceString();
                        if (! templateToRefId.containsKey(templateRefString)) {
                            addTemplate(++refCounter, templateRefString,
                                        tmpl, templateToRefId, templatesElement);
                        }
                    }
                }
            }
        }

        return refCounter;
    }

    private void addTemplate(int refCounter, String templateRefString,
                             TemplateConfig tc,
                             HashMap templateToRefId,
                             Element templatesElement)
    throws ConfigException {
        templateToRefId.put(templateRefString, String.valueOf(refCounter));

        Element templateElement = addNewElement(templatesElement, XML_TEMPLATE);

        addElement(templateElement, XML_GLOBAL_TEMPLATE_REF,      refCounter);
        addElement(templateElement, XML_TEMPLATE_ID,              tc.getId());
        addElement(templateElement, XML_TEMPLATE_DESC ,           tc.getDescription());
        addElement(templateElement, XML_TEMPLATE_STATUS_RESPONSE, tc.isForStatusResponse());

        Element templateFields = addNewElement(templateElement, XML_CLIENT_SESSION_TEMPLATE_FIELDS);

        // adding this templates 'template information' (template id and 'used fields' info)
        for (ListIterator n = tc.getFieldIterator(); n.hasNext(); ) {
            FieldConfig fc = (FieldConfig)n.next();

            Element templateField = addNewElement(templateFields, XML_CLIENT_SESSION_TEMPLATE_FIELD);

            addElement(templateField, XML_CRANE_KEY_ID,    fc.getId());
            addElement(templateField, XML_FIELD_USED,      fc.isEnabledForOutput());
            addElement(templateField, XML_CRANE_KEY_NAME,  fc.getName());
            addElement(templateField, XML_CRANE_KEY_LABEL, fc.getLabel());
            addElement(templateField, XML_CRANE_KEY_HELP,  fc.getHelp());
            addElement(templateField, XML_CRANE_KEY_TYPE,  fc.getTypeId());

            if (fc.getFieldDesc() != null) {
                addElement(templateField, XML_XACCT_FIELD_LABEL, fc.getFieldDesc().getLabel());
                addElement(templateField, XML_XACCT_FIELD_TYPE,  fc.getFieldDesc().getFieldType().getLabel());
                try {
                    addElement(templateField, XML_XACCT_FIELD_ID,    fc.getFieldDesc().getPrimitiveID());
                } catch (InvalidFieldDescriptorException ex) {
                    throw new ConfigException(ex);
                }
            }
        }
    }


    private static final String XML_FIELDS                           = "Fields";
    private static final String XML_FIELD                            = "Field";
    private static final String XML_CRANE_KEY_ID                     = "CrnKeyId";
    private static final String XML_CRANE_KEY_NAME                   = "CrnKeyName";
    private static final String XML_CRANE_KEY_LABEL                  = "CrnKeyLabel";
    private static final String XML_CRANE_KEY_HELP                   = "CrnKeyHelp";
    private static final String XML_CRANE_KEY_TYPE                   = "CrnKeyTypeId";
    private static final String XML_FIELD_USED                       = "FieldUsed";
    private static final String XML_XACCT_FIELD_ID                   = "XFieldId";
    private static final String XML_XACCT_FIELD_LABEL                = "XFieldLabel";
    private static final String XML_XACCT_FIELD_TYPE                 = "XFieldType";

    private static final String XML_CLIENTS                          = "Clients";
    private static final String XML_CLIENT                           = "Client";
    private static final String XML_CLIENT_IPADDRESS                 = "IP";
    private static final String XML_CLIENT_PORT                      = "Port";
    private static final String XML_CLIENT_VERSION_PORT              = "VerPort";
    private static final String XML_CLIENT_VENDOR_STRING             = "VendorString";
    private static final String XML_CLIENT_CONNECT_TIMEOUT           = "CnctTO";
    private static final String XML_CLIENT_CONNECT_RETRY_TIMEOUT     = "CnctRetryTO";
    private static final String XML_CLIENT_RECONNECT_INTERVAL        = "RecnctItvl";
    private static final String XML_CLIENT_CONNECT_ROUNDTRIP_TIMEOUT = "CnctRndtrpTO";
    private static final String XML_CLIENT_ACK_LOG_INTERVAL          = "AckLogItvl";
    private static final String XML_CLIENT_DESCRIPTION               = "Desc";

    private static final String XML_CLIENT_SESSIONS                  = "Sessions";
    private static final String XML_CLIENT_SESSION                   = "Session";
    private static final String XML_CLIENT_SESSION_ID                = "Id";
    private static final String XML_CLIENT_SESSION_NAME              = "Name";
    private static final String XML_CLIENT_SESSION_DESCRIPTION       = "Desc";
    private static final String XML_CLIENT_SESSION_TEMPLATES         = "Tmplts";
    private static final String XML_CLIENT_SESSION_TEMPLATE          = "Tmpl";
    private static final String XML_CLIENT_SESSION_TEMPLATE_REF      = "TmplRef";
    private static final String XML_CLIENT_SESSION_TEMPLATE_FIELDS   = "Fields";
    private static final String XML_CLIENT_SESSION_TEMPLATE_FIELD    = "Field";

    private static final String XML_TEMPLATES                        = "Templates";
    private static final String XML_TEMPLATE                         = "Template";
    private static final String XML_GLOBAL_TEMPLATE_REF              = "GlobalRef";
    private static final String XML_TEMPLATE_ID                      = "Id";
    private static final String XML_TEMPLATE_DESC                    = "Desc";
    private static final String XML_TEMPLATE_STATUS_RESPONSE         = "StatRsp";
    private static final String XML_FIELD_IDS                        = "FieldIds";

    private static final String XML_CONFIGURATION_VERSION            = "ConfigurationVersion";
    private static final String XML_SERVER_IPADDRESS                 = "ServerIPAddress";
    private static final String XML_SERVER_PORT                      = "ServerPort";
    private static final String XML_SERVER_ACK_INTERVAL              = "ServerAckInterval";
    private static final String XML_SERVER_START_INTERVAL            = "ServerStartInterval";
    private static final String XML_IS_DEBUG_LEVEL_USED              = "EnableDebugMsg";
    private static final String XML_NO_DATA_KEEP_ALIVE_INTERVAL      = "NoDataKeepAliveInterval";

    /** Print the XML document
     *
     * @param doc The JDOM XML
     * @param out The output print stream (e.g., <code>System.out</code>);
     */
    public static void outputXml(org.jdom.Document doc, java.io.PrintStream out) {
        try {
            if (doc == null) {
                out.println("*** null XML document ***");
            } else {
                (new org.jdom.output.XMLOutputter("  ", true)).output(doc, out);
            }
            out.flush();
        } catch (java.io.IOException e) {
            System.err.println("Error while outputting XML " + doc + ": " + e);
        }
    }

    public static void outputXml(org.jdom.Element element, java.io.PrintStream out) {
        outputXml(new org.jdom.Document(element), out);
    }
}
