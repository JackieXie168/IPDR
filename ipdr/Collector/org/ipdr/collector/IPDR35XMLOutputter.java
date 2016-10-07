package org.ipdr.collector;

//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
//ANY KIND either expresses or implied.

import java.io.*;
import java.math.BigInteger;
import java.net.Inet4Address;
import java.net.Inet6Address;
import java.net.InetAddress;
import java.util.Date;

import org.ipdr.api.Outputter;
import org.ipdr.api.configuration.FieldConfigI;
import org.ipdr.api.log.LogManager;
import org.ipdr.protocol.ProtocolDataBigEndian;
import org.ipdr.utility.IPDRUtilities;
import java.util.*;
import java.text.SimpleDateFormat;
import org.ipdr.api.StateMachineListener;
import org.ipdr.protocol.*;
import org.ipdr.api.*;

/**
 * Outputs a record in IPDR-XML form.
 *
 */
public final class IPDR35XMLOutputter
    implements Outputter, StateMachineListener {
  private String mConfigFileName;
  private IPDR35XMLOutputterDataKey mCurrentDataKey;
  private String mOutputPrefix;
  private InetAddress mExporterAddress;
  private int mExporterPort;
  private IPDR35XMLDocDataHandler mDocDataHandler;
  private StateMachineListenerManager mStateMachineListenerManager;
  private IPDR35XMLOutputterTagsHandler mIPDR35XMLOutputterTagsHandler;


  /**
   * Constructor -
   * registers itself to StateMachineListenerManager
   * @param confiFileName
   *            The configuration file name
   * @param outputPrefix
   *            The prefix of output file name
   * @param transmitterAddress
   *            The ip address of Exporter
   * @param transmitterPort
   *            The port of Exporter
   * @param docDataHandler
   *            The handler of xml file data
   * @param stateMachineListenerManager
   *            The manager of StateMichineListeners
   */
  public IPDR35XMLOutputter(String confiFileName, String outputPrefix,
                            InetAddress transmitterAddress,
                            int transmitterPort,
                            IPDR35XMLDocDataHandler docDataHandler,
                            StateMachineListenerManager
                            stateMachineListenerManager) {
    LogManager.debug(this, "Constructor",
                     "REGISTERS itself with Exporter address " +
                    transmitterAddress.getHostName() + " Port " + transmitterPort);

    mConfigFileName = confiFileName;
    try
    {
      mIPDR35XMLOutputterTagsHandler = new IPDR35XMLOutputterTagsHandler(
          mConfigFileName);
    }
    catch(Exception e)
    {
      e.printStackTrace();
      LogManager.error("IPDR35XMLOutputter", "initiates IPDR35XMLOutputterTagsHandler", e.toString());
      System.exit(1);

    }
    mOutputPrefix = outputPrefix;
    mExporterPort = transmitterPort;
    mExporterAddress = transmitterAddress;
    mDocDataHandler = docDataHandler;
    mStateMachineListenerManager = stateMachineListenerManager;

    // registers itself to the StateMachineListenerManager
    mStateMachineListenerManager.registerListener(this,
                                                  mExporterAddress,
                                                  mExporterPort);
  }

  /**
   * Called from finalize method. Retrieves all opened XMl Output files from IPDR35XMLDocDataHandler,
   * Adds close tags and closes them.
   * @see org.ipdr.api.Outputter#close()
   */
  public final synchronized void close() {
    ArrayList documentsList = mDocDataHandler.
        getAllOpenDocuments();
    Iterator documentsIter = documentsList.iterator();
    while (documentsIter.hasNext()) {
      IPDR35XMLOutputterDataKey mCurrentDataKey = (IPDR35XMLOutputterDataKey)
          documentsIter.
          next();
      // Close end tags of previously opened XML output files
      endXMLFile(mCurrentDataKey);
      // Removes XML output files from hash of IPDR35XMLDocDataHandler
      mDocDataHandler.removeDocumentData(mCurrentDataKey);

    }
  }

  public final void endRecord() {
    StringBuffer recordBuffer = new StringBuffer();
    recordBuffer.append(getElementEndTagString(IPDR35XMLOutputterTagsHandler.
                                                IPDR));
    recordBuffer.append('\n');
    FileWriter XMLFileWriter = mDocDataHandler.getXMLFileWriter(mCurrentDataKey);
    if(XMLFileWriter == null)
    {
      return;
    }
    try {
      XMLFileWriter.write(recordBuffer.toString());
      XMLFileWriter.flush();
    }
    catch (IOException e) {
      LogManager.error("IPDR35XMLOutputter", "endRecord", e.toString());

    }


  }

  protected void finalize() throws Throwable {
    try {
      close();
      mStateMachineListenerManager.unRegisterListener(
          mExporterAddress, mExporterPort);

    }
    finally {
      super.finalize();
    }

  }

  /**
   * @see org.ipdr.api.Outputter#outputBase64Binary(int,
   *      org.ipdr.api.configuration.FieldConfigI, int, byte[])
   */
  public void outputBase64Binary(int keyI, FieldConfigI field, int length,
                                 byte[] value) {
    outputHexBinary(keyI, field, length, value);
  }

  /**
   * @see org.ipdr.api.Outputter#outputBoolean(int,
   *      org.ipdr.api.configuration.FieldConfigI, byte)
   */
  public final void outputBoolean(int keyI, FieldConfigI field, byte value) {
    StringBuffer buffer = new StringBuffer();
    buffer.append(value);
    appendRecordFieldValue(field.getName(), buffer);
  }

  /**
   * @see org.ipdr.api.Outputter#outputByte(int,
   *      org.ipdr.api.configuration.FieldConfigI, byte)
   */
  public final void outputByte(int keyI, FieldConfigI field, byte value) {
    StringBuffer buffer = new StringBuffer();
    buffer.append(value);
    appendRecordFieldValue(field.getName(), buffer);
  }

  /**
   * @see org.ipdr.api.Outputter#outputDateTimeMsec(int,
   *      org.ipdr.api.configuration.FieldConfigI, long)
   */
  public final void outputDateTimeMsec(int keyI, FieldConfigI field,
                                       long value) {
    StringBuffer buffer = new StringBuffer();
    buffer.append(IPDRUtilities.dateFormatMillisec(value));
    appendRecordFieldValue(field.getName(), buffer);

  }

  /**
   * @see org.ipdr.api.Outputter#outputDateTimeSec(int,
   *      org.ipdr.api.configuration.FieldConfigI, int)
   */
  public void outputDateTimeSec(int keyI, FieldConfigI field, int value) {
    StringBuffer buffer = new StringBuffer();
    buffer.append(IPDRUtilities.dateFormatSeconds(value));
    appendRecordFieldValue(field.getName(), buffer);

  }

  /**
   * @see org.ipdr.api.Outputter#outputDateTimeUsec(int,
   *      org.ipdr.api.configuration.FieldConfigI, long)
   */
  public final void outputDateTimeUsec(int keyI, FieldConfigI field,
                                       long value) {
    StringBuffer buffer = new StringBuffer();
    buffer.append(IPDRUtilities.dateFormatMicrosec(value));
    appendRecordFieldValue(field.getName(), buffer);

  }

  /**
   * @see org.ipdr.api.Outputter#outputDouble(int,
   *      org.ipdr.api.configuration.FieldConfigI, double)
   */
  public final void outputDouble(int keyI, FieldConfigI field, double value) {
    StringBuffer buffer = new StringBuffer();
    buffer.append(value);
    appendRecordFieldValue(field.getName(), buffer);
  }

  /**
   * @see org.ipdr.api.Outputter#outputFloat(int,
   *      org.ipdr.api.configuration.FieldConfigI, float)
   */
  public final void outputFloat(int keyI, FieldConfigI field, float value) {
    StringBuffer buffer = new StringBuffer();
    buffer.append(value);
    appendRecordFieldValue(field.getName(), buffer);
  }

  /**
   * @see org.ipdr.api.Outputter#outputHexBinary(int,
   *      org.ipdr.api.configuration.FieldConfigI, int, byte[])
   */
  public void outputHexBinary(int keyI, FieldConfigI field, int length,
                              byte[] value) {
    StringBuffer buffer = new StringBuffer();
    buffer.append("HexBinary: ");
    for (int i = 0; i < length; i++) {
      buffer.append(Integer.toHexString(value[i]));
    }

    appendRecordFieldValue(field.getName(), buffer);

  }

  /**
   * @see org.ipdr.api.Outputter#outputInt(int,
   *      org.ipdr.api.configuration.FieldConfigI, int)
   */
  public final void outputInt(int keyI, FieldConfigI field, int value) {
    StringBuffer buffer = new StringBuffer();
    buffer.append(value);
    appendRecordFieldValue(field.getName(), buffer);
  }

  /**
   * @see org.ipdr.api.Outputter#outputIPv4Addr(int,
   *      org.ipdr.api.configuration.FieldConfigI, java.net.InetAddress)
   */
  public final void outputIPv4Addr(int keyI, FieldConfigI field,
                                   Inet4Address value) {
    StringBuffer buffer = new StringBuffer();
    buffer.append("IP_V4:").append(value.getHostAddress()); ;
    appendRecordFieldValue(field.getName(), buffer);

  }

  /**
   * @see org.ipdr.api.Outputter#outputIPv6Addr(int,
   *      org.ipdr.api.configuration.FieldConfigI, java.net.Inet6Address)
   */
  public void outputIPv6Addr(int keyI, FieldConfigI field, Inet6Address value) {
    StringBuffer buffer = new StringBuffer();
    buffer.append("IP_V6:").append(value.getHostAddress());
    appendRecordFieldValue(field.getName(), buffer);

  }

  /**
   * @see org.ipdr.api.Outputter#outputLong(int,
   *      org.ipdr.api.configuration.FieldConfigI, long)
   */
  public final void outputLong(int keyI, FieldConfigI field, long value) {
    StringBuffer buffer = new StringBuffer();
    buffer.append(value);
    appendRecordFieldValue(field.getName(), buffer);
  }

  /**
   * @see org.ipdr.api.Outputter#outputMACAddress(int,
   *      org.ipdr.api.configuration.FieldConfigI, long)
   */
  public void outputMACAddress(int keyI, FieldConfigI field, long value) {
    int i;
    String rawValue = Long.toHexString(value);
    int valueLength = rawValue.length();
    String str = "";
    for (i = 0; i < (16 - valueLength); i++) {
      str = str + "0";
      if ( (i % 2) == 1) str = str + "-";
    }

    for (i = 0; i < valueLength; i++) {
      str = str + rawValue.charAt(i);
      if ( ( ( (16 - valueLength + i) % 2) == 1)
          && ( (16 - valueLength + i) < 15)) str = str + "-";
    }
    StringBuffer buffer = new StringBuffer();
    buffer.append("MAC-ADD:").append(str);
    appendRecordFieldValue(field.getName(), buffer);

  }

  public final void outputShort(int keyI, FieldConfigI field, short value) {
    StringBuffer buffer = new StringBuffer();
    buffer.append(value);
    appendRecordFieldValue(field.getName(), buffer);
  }

  /**
   * @see org.ipdr.api.Outputter#outputString(int,
   *      org.ipdr.api.configuration.FieldConfigI, java.lang.String)
   */
  public final void outputString(int keyI, FieldConfigI field, String str) {
    StringBuffer buffer = new StringBuffer();
    buffer.append("'").append(str).append("'");
    appendRecordFieldValue(field.getName(), buffer);

  }

  /**
   * @see org.ipdr.api.Outputter#outputUnsignedByte(int,
   *      org.ipdr.api.configuration.FieldConfigI, short)
   */
  public final void outputUnsignedByte(int keyI, FieldConfigI field,
                                       short value) {
    StringBuffer buffer = new StringBuffer();
    buffer.append(value);
    appendRecordFieldValue(field.getName(), buffer);
  }

  /**
   * @see org.ipdr.api.Outputter#outputUnsignedInt(int,
   *      org.ipdr.api.configuration.FieldConfigI, long)
   */
  public final void outputUnsignedInt(int keyI, FieldConfigI field, long value) {
    StringBuffer buffer = new StringBuffer();
    buffer.append(value);
    appendRecordFieldValue(field.getName(), buffer);
  }

  /**
   * @see org.ipdr.api.Outputter#outputUnsignedLong(int,
   *      org.ipdr.api.configuration.FieldConfigI, long)
   */
  public final void outputUnsignedLong(int keyI, FieldConfigI field,
                                       long value) {
    ProtocolDataBigEndian be = new ProtocolDataBigEndian();
    byte[] rawBytes = new byte[9];
    rawBytes[0] = 0;
    be.putLong(rawBytes, 1, value);
    BigInteger bd = new BigInteger(rawBytes);
    StringBuffer buffer = new StringBuffer();
    buffer.append(bd.toString());
    appendRecordFieldValue(field.getName(), buffer);

  }

  /**
   * @see org.ipdr.api.Outputter#outputUnsignedShort(int,
   *      org.ipdr.api.configuration.FieldConfigI, int)
   */
  public final void outputUnsignedShort(int keyI, FieldConfigI field,
                                        int value) {
    StringBuffer buffer = new StringBuffer();
    buffer.append(value);
    appendRecordFieldValue(field.getName(), buffer);
  }

  /**
   * @see org.ipdr.api.Outputter#outputUUID(int,
   *      org.ipdr.api.configuration.FieldConfigI, byte[])
   */
  public void outputUUID(int keyI, FieldConfigI field, byte[] value) {
    StringBuffer buffer = new StringBuffer();
    buffer.append("UUID:");
    for (int i = 4; i < 20; i++) {
      buffer.append( (char) value[i]);
    }
    appendRecordFieldValue(field.getName(), buffer);

  }

  /**
   * @see org.ipdr.api.Outputter#startRecord(java.net.InetAddress, int,
   *      java.util.Date, long, int, int, byte[], boolean)
   */
  public final synchronized void startRecord(InetAddress transmitterAddress,
                                             int transmitterPort,
                                             Date transmitterBootTime,
                                             long seqNo,
                                             int configId, int sessionId,
                                             int templateId, byte[] documentId,
                                             boolean isDuplicate) {
    mCurrentDataKey = new IPDR35XMLOutputterDataKey(
         transmitterAddress, transmitterPort,sessionId);
    StringBuffer recordBuffer = new StringBuffer();
    recordBuffer.append('<');
    recordBuffer.append(IPDR35XMLOutputterTagsHandler.IPDR);
    recordBuffer.append(getAttributeValue(IPDR35XMLOutputterTagsHandler.
                                           IPDR_RECORD_TYPE_ATTRIBUTE,
                                           mIPDR35XMLOutputterTagsHandler.
                                           getTypeName(templateId)));
    recordBuffer.append('>');
    recordBuffer.append('\n');
    recordBuffer.append(getElementTagString(IPDR35XMLOutputterTagsHandler.
                                             IPDR_CREATION_TIME));
    recordBuffer.append(getFormatedDate(new Date()));
    recordBuffer.append(getElementEndTagString(IPDR35XMLOutputterTagsHandler.
                                                IPDR_CREATION_TIME));
    recordBuffer.append('\n');
    recordBuffer.append(getElementTagString(IPDR35XMLOutputterTagsHandler.IPDR_SEQ_NUM_ATTRIBUTE));
    recordBuffer.append(seqNo);
    recordBuffer.append(getElementEndTagString(IPDR35XMLOutputterTagsHandler.IPDR_SEQ_NUM_ATTRIBUTE));
    recordBuffer.append('\n');


    mDocDataHandler.enlargeRecordsCounter(mCurrentDataKey);
    mDocDataHandler.setLastRecordDate(mCurrentDataKey,
                                      new Date());
    FileWriter XMLFileWriter = mDocDataHandler.getXMLFileWriter(mCurrentDataKey);
    if(XMLFileWriter==null)
    {
      return;
    }

    try {
      XMLFileWriter.write(recordBuffer.toString());
    }
    catch (IOException e) {
      LogManager.error("IPDR35XMLOutputter", "startRecord", e.toString());
    }


  }

  //Utility to close  XML file
  private void endXMLFile(IPDR35XMLOutputterDataKey currentDataKey) {
    if(mDocDataHandler.
       getRecordsCounter(currentDataKey)==0)
    {
      return;
    }
    FileWriter XMLFileWriter = mDocDataHandler.getXMLFileWriter(
        currentDataKey);
    if (XMLFileWriter == null) {
      return;
    }
    StringBuffer endBuffer = new StringBuffer();
    endBuffer.append("<IPDRDoc.End ");
    endBuffer.append(getAttributeValue(IPDR35XMLOutputterTagsHandler.
                                       IPDR_COUNT_ATTRIBUTE,
                                       Integer.toString(mDocDataHandler.
        getRecordsCounter(currentDataKey))));
    endBuffer.append(getAttributeValue(IPDR35XMLOutputterTagsHandler.
                                       IPDR_END_TIME_ATTRIBUTE,
                                       getFormatedDate(mDocDataHandler.
        getLastRecordDate(currentDataKey))));
    endBuffer.append(">");
    endBuffer.append("</IPDRDoc.End>");
    endBuffer.append('\n');
    endBuffer.append("</IPDRDoc>");
    try {
      XMLFileWriter.write(endBuffer.toString());
      XMLFileWriter.flush();
      XMLFileWriter.close();
    }
    catch (IOException e) {
      LogManager.error("IPDR35XMLOutputter", "endXMLFile", e.toString());
    }
    mDocDataHandler.removeDocumentData(currentDataKey);
  }

  //Utility to insert IPDRDoc Element with attributes  to XML file
  private StringBuffer getXMLFileHeader(byte[] documentId) {
    StringBuffer startDocBuffer = new StringBuffer();
    startDocBuffer.append(IPDR35XMLOutputterTagsHandler.StaticXMLHeader);
    startDocBuffer.append('\n');
    startDocBuffer.append("<IPDRDoc ");
    startDocBuffer.append(getAttributeValue(IPDR35XMLOutputterTagsHandler.
                                            IPDR_XMLNS,
                                            IPDR35XMLOutputterTagsHandler.
                                            NameSpaceValue));
    startDocBuffer.append(getAttributeValue(IPDR35XMLOutputterTagsHandler.
                                            IPDR_XMLNS_XSI_ATTRIBUTE,
                                            IPDR35XMLOutputterTagsHandler.
                                            SchemaInstanceValue));
    startDocBuffer.append(getAttributeValue(IPDR35XMLOutputterTagsHandler.
                                            IPDR_SCHEMA_LOCATION_ATTRIBUTE,
                                            mIPDR35XMLOutputterTagsHandler.
                                            getProperty(
        IPDR35XMLOutputterTagsHandler.SchemaLocation)));

    String docID = getStringDocumentID(documentId);
    startDocBuffer.append(getAttributeValue(IPDR35XMLOutputterTagsHandler.
                                            IPDR_DOC_ID_ATTRIBUTE, docID));
    startDocBuffer.append(getAttributeValue(IPDR35XMLOutputterTagsHandler.
                                            IPDR_CREATION_TIME_ATTRIBUTE,
                                            getFormatedDate(new Date())));
    startDocBuffer.append(getAttributeValue(IPDR35XMLOutputterTagsHandler.
                                            IPDR_RECORDER_INFO_ATTRIBUTE,
                                            mIPDR35XMLOutputterTagsHandler.
                                            getProperty(
        IPDR35XMLOutputterTagsHandler.IPDR_RECORDER_INFO)));
    startDocBuffer.append(getAttributeValue(IPDR35XMLOutputterTagsHandler.
                                            IPDR_VERSION_ATTRIBUTE,
                                            mIPDR35XMLOutputterTagsHandler.
                                            getProperty(
        IPDR35XMLOutputterTagsHandler.Version)));
    startDocBuffer.append('>');
    return startDocBuffer;

  }

  private final String getStringDocumentID(byte[] documentId) {
    StringBuffer docIDBuffer = new StringBuffer();
    for (int i = 0; i < documentId.length; i++) {
      short digit = (short) (0x00FF & documentId[i]);
      String rep = Long.toHexString(digit);
      if (rep.length() == 1) rep = "0" + rep;
      docIDBuffer.append(rep);
    }
    return docIDBuffer.toString();
  }

  //Utility to insert non Text Element without value to XML file
  private String getElementTagString(String elementName) {
    return new String("<" + elementName + ">");
  }

  //Utility to close  Element
  private String getElementEndTagString(String elementName) {
    return new String("</" + elementName + ">");
  }

  //Utility to insert Text Element with value to XML file
  private void appendRecordFieldValue(String fieldName, StringBuffer fieldValue) {
    StringBuffer recordBuffer = new StringBuffer();
    recordBuffer.append("<");
    recordBuffer.append(fieldName);
    recordBuffer.append(">");
    recordBuffer.append(fieldValue);
    recordBuffer.append("</");
    recordBuffer.append(fieldName);
    recordBuffer.append(">");
    recordBuffer.append('\n');
    FileWriter XMLFileWriter = mDocDataHandler.getXMLFileWriter(mCurrentDataKey);
    if(XMLFileWriter == null)
    {
      return;
    }
    try {
      XMLFileWriter.write(recordBuffer.toString());
      XMLFileWriter.flush();
    }
    catch (IOException e) {
      LogManager.error("IPDR35XMLOutputter", "appendRecordFieldValue", e.toString());

    }


  }

  //Utility to insert attribute with value to XML file
  private String getAttributeValue(String attributeName, String attributeValue) {
    StringBuffer attributeBuffer = new StringBuffer();
    attributeBuffer.append(attributeName);
    attributeBuffer.append("=");
    attributeBuffer.append('"');
    attributeBuffer.append(attributeValue);
    attributeBuffer.append('"');
    attributeBuffer.append('\n');
    return attributeBuffer.toString();
  }

  //Compose XML file name conserning Exporter's host,port, Session ID and document ID
  private String composeXMLFileName(IPDR35XMLOutputterDataKey dataKey,
                                    byte[] documentID) {
    StringBuffer fileName = new StringBuffer(mOutputPrefix);
    fileName.append("_");
    fileName.append(dataKey.getExporterAddress().getHostName());
    fileName.append("_");
    fileName.append(dataKey.getExporterPort());
    fileName.append("_");
    fileName.append(dataKey.getSessionID());
    fileName.append("_");
    fileName.append(getStringDocumentID(documentID));
    return getUniqueFileName(fileName.toString());

  }

  //Make new XML file , insert header
  private void startNewXMLFile(IPDR35XMLOutputterDataKey mCurrentDataKey,
                               byte[] documentID) {
    String fileName = composeXMLFileName(mCurrentDataKey, documentID);
    StringBuffer recordBuffer = new StringBuffer();
    FileWriter XMLFileWriter=null;
    try {
      recordBuffer = getXMLFileHeader(documentID);
      recordBuffer.append('\n');
      XMLFileWriter = new FileWriter(fileName, true);
      XMLFileWriter.write(recordBuffer.toString());
    }
    catch (IOException e) {
      LogManager.error("IPDR35XMLOutputter", "startNewXMLFile", e.toString());

    }
    mDocDataHandler.putDocumentData(mCurrentDataKey,
                                    documentID,
                                    XMLFileWriter);


  }

  //Return String representation of Date in format  "yyyy-dd-MM'TO'hh:mm:ssz"
  private String getFormatedDate(Date date) {
    Locale locale = Calendar.getInstance().getAvailableLocales()[0];
    SimpleDateFormat formatter = new SimpleDateFormat(mIPDR35XMLOutputterTagsHandler.
                                            getProperty(
      IPDR35XMLOutputterTagsHandler.DATE_FORMAT),locale);
    return formatter.format(date);
  }

  // Check if File with this name is already exists and changes its name
  private String getUniqueFileName(String fileName) {

    int postfix = 0;
    boolean fileExist=true;
    StringBuffer XMLNameBuffer=new StringBuffer();
    while(fileExist==true)
    {
      postfix++;
      XMLNameBuffer.delete(0,XMLNameBuffer.length());
      XMLNameBuffer.append(fileName);
      XMLNameBuffer.append('_');
      XMLNameBuffer.append(postfix);
      XMLNameBuffer.append(".xml");
      fileExist = new File(XMLNameBuffer.toString()).exists();
    }
    return XMLNameBuffer.toString();
  }

  /**
   * @see org.ipdr.api.StateMachineListener#notifyDisconnected(java.net.InetAddress, int)
   */
  public void notifyDisconnected(InetAddress address, int port) {
    try {
      close();
    }
    catch (Throwable e) {
      LogManager.error("IPDR35XMLOutputter", "notifyDisconnected", e.toString());

    }
  }

  /**
   * @see org.ipdr.api.StateMachineListener#notifyConnected(java.net.InetAddress, int, ConnectInfo)
   */
  public void notifyConnected(InetAddress address, int port, ConnectInfo info) {
  }

  /**
   * @see org.ipdr.api.StateMachineListener#notifyFlowStarted(java.net.InetAddress, int, FlowStartInfo)
   */
  public void notifyFlowStarted(InetAddress address, int port,
                                FlowStartInfo info) {
  }

  /**
   * Open new Output XML file
   * @see org.ipdr.api.StateMachineListener#notifySessionStarted(java.net.InetAddress, int, SessionStartInfo)
   */
  public void notifySessionStarted(InetAddress transmitterAddress,
                                   int transmitterPort, SessionStartInfo info) {
    byte[] documentId = info.getDocumentId();
    int sessionId = info.getSessionId();
    IPDR35XMLOutputterDataKey currentDataKey = new IPDR35XMLOutputterDataKey(
         transmitterAddress, transmitterPort,sessionId);
    startNewXMLFile(currentDataKey, documentId);

  }

  /**
   * Close Output XML file
   * @see org.ipdr.api.StateMachineListener#notifySessionStopped(java.net.InetAddress, int, SessionStopInfo)
   */
  public void notifySessionStopped(InetAddress transmitterAddress,
                                   int transmitterPort,
                                   SessionStopInfo info) {
    int sessionId = info.getSessionId();
    IPDR35XMLOutputterDataKey currentDataKey = new IPDR35XMLOutputterDataKey(
        transmitterAddress, transmitterPort,sessionId);
    endXMLFile(currentDataKey);

  }

  /**
   * Close Output XML file
   * @see org.ipdr.api.StateMachineListener#notifyFlowStopped(java.net.InetAddress, int, FlowStopInfo)
   */
  public void notifyFlowStopped(InetAddress transmitterAddress,
                                int transmitterPort,
                                FlowStopInfo info) {
    int sessionId = info.getSessionId();
    IPDR35XMLOutputterDataKey currentDataKey = new IPDR35XMLOutputterDataKey(
        transmitterAddress, transmitterPort, sessionId);
    endXMLFile(currentDataKey);

  }
  public void notifyTemplatesNegotiated(InetAddress address, int port,
				byte sessionId, ArrayList templates)
  {
  }


}
