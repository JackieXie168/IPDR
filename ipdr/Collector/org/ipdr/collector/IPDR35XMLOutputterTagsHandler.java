package org.ipdr.collector;

//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
//ANY KIND either expresses or implied.
import java.util.*;
import org.jdom.Document;
import org.jdom.Element;
import org.jdom.JDOMException;
import org.jdom.input.SAXBuilder;
import java.io.File;

/**
 * The class to loads information from parsed configuration xml file
 */

public class IPDR35XMLOutputterTagsHandler {

  private HashMap configMap = new HashMap();
  private Hashtable attributesMap = new Hashtable();

  public final static String ROOT = "IPDR35XMLOutputConfig";

  public final static String SchemaLocation = "SchemaLocation";
  public final static String Template = "Template";
  public final static String templateID = "templateID";
  public final static String typeName = "typeName";
  public final static String Templates = "Templates";
  public final static String Version = "version";
  public final static String DATE_FORMAT = "dateFormat";

  public final static String IPDR_IPDRDoc = "IPDRDoc";
  public final static String IPDR_XMLNS = "xmlns";
  public final static String IPDR_XMLNS_XSI_ATTRIBUTE = "xmlns:xsi";
  public final static String IPDR_SCHEMA_LOCATION_ATTRIBUTE =
      "xsi:schemaLocation";
  public final static String IPDR_DOC_ID_ATTRIBUTE = "docId";
  public final static String IPDR_CREATION_TIME_ATTRIBUTE = "creationTime";
  public final static String IPDR_RECORDER_INFO_ATTRIBUTE = "IPDRRecorderInfo";
  public final static String IPDR_SEQ_NUM_ATTRIBUTE = "seqNum";
  public final static String IPDR_RECORDER_INFO = "IPDRRecorderInfo";
  public final static String IPDR_VERSION_ATTRIBUTE = "version";
  public final static String IPDR_RECORD_TYPE_ATTRIBUTE = "xsi:type";
  public final static String IPDR = "IPDR ";
  public final static String IPDR_CREATION_TIME = "IPDRCreationTime";
  public final static String IPDR_DOC_END = "IPDRDoc.End";
  public final static String IPDR_COUNT_ATTRIBUTE = "count";
  public final static String IPDR_END_TIME_ATTRIBUTE = "endTime";
  public final static String SchemaInstanceValue =
      "http://www.w3.org/2001/XMLSchema";
  public final static String NameSpaceValue =
      "http://www.ipdr.org/namespaces/ipdr";
  public final static String StaticXMLHeader =
      "<?xml version = \"1.0\" encoding = \"UTF-8\"?>";

  /**
   * Holds XML document
   */
  private Document document;

  /**
   * Points to the root element of the config file
   */
  private Element root;

  /**
   * Constructor
   *
   * @param fileName
   *            Outputter configuration file name
   */


  public IPDR35XMLOutputterTagsHandler(String fileName) throws JDOMException,
      Exception {
    // Build the document with SAX and Xerces, no validation
    SAXBuilder builder = new SAXBuilder();
    // Create the document
    Document m_document = builder.build(new File(fileName));
    // get root element
    root = m_document.getRootElement();
    if (root == null)throw new Exception("Invalid config file");
    fillConfigData();

  }

  public String getTypeName(int templateID) {
    String templateName =  (String) attributesMap.get(Integer.toString(templateID));
    if(templateName!=null)
    {
      return templateName;
    }
    return "Unknown template";
  }

  public String getProperty(String propertyName) {
    String property = (String) configMap.get(propertyName);
    if(property!=null)
    {
      return property;
    }
    return "Unknown property";
  }
  /**
   * Return singleton instance of IPDR35XMLOutputterTagsHandler
   * @return IPDR35XMLOutputterTagsHandler
   */


  /**
   * Fills config parameters
   * @throws Exception
   */
  private void fillConfigData() throws Exception {

    Element recordInfoElement = root.getChild(this.IPDR_RECORDER_INFO);
    if (recordInfoElement == null)
      throw new Exception("No Record Info");
    String recordInfo = recordInfoElement.getText();
    configMap.put(IPDR_RECORDER_INFO, recordInfo);
    Element versionElement = root.getChild(this.Version);
    if (versionElement == null)
      throw new Exception("No Version");
    String version = versionElement.getText();
    configMap.put(Version, version);

    Element schemaLocationElement = root.getChild(this.SchemaLocation);
    if (schemaLocationElement == null)
      throw new Exception("No Schema Locatione");
    String schemaLocation = schemaLocationElement.getText();
    configMap.put(SchemaLocation, schemaLocation);

    Element dateFormatElement = root.getChild(DATE_FORMAT);
    if (dateFormatElement == null)
      configMap.put(DATE_FORMAT, "yyyy-dd-MM'TO'hh:mm:ssz");
    String dateFormat = dateFormatElement.getText();
    configMap.put(DATE_FORMAT, dateFormat);


    Element templatesElement = root.getChild(this.Templates);
    List templates = templatesElement.getChildren();
    Iterator templatesIter = templates.iterator();
    String id;
    String typeName;
    while (templatesIter.hasNext()) {
      Element template = (Element) templatesIter.next();
      id = template.getAttributeValue(this.templateID);
      typeName = template.getAttributeValue(this.typeName);
      attributesMap.put(id, typeName);
    }

  }

}
