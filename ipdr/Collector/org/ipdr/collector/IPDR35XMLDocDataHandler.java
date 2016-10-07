package org.ipdr.collector;

//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
//ANY KIND either expresses or implied.
import java.util.*;
import java.io.FileWriter;
import org.ipdr.api.log.LogManager;
/**
 * The class implements a handler of xml file data per Exporter and Session
 */
public class IPDR35XMLDocDataHandler {
  private Hashtable mExporterSessionXMLFileMap;
  private Hashtable mExporterSessionDocumentMap;

  /**
   * Constructor
   *
   */

  public IPDR35XMLDocDataHandler() {
    mExporterSessionDocumentMap = new Hashtable();
    mExporterSessionXMLFileMap = new Hashtable();
  }


  /**
   * put new XML file name for new Document ID per Exporter && Session
   * @param key IPDR35XMLOutputterDataKey
   * @param documentID new Document ID
   * @param xmlFileName new XML file name
   */

  public void putDocumentData(IPDR35XMLOutputterDataKey key, byte[] documentID,
                              FileWriter xmlFileWriter) {
    mExporterSessionDocumentMap.put(key, documentID);
    IPDR35XMLOutputterRecordData recordData = new IPDR35XMLOutputterRecordData();
    recordData.setXMLFileWriter(xmlFileWriter);
    mExporterSessionXMLFileMap.put(key, recordData);

  }

  /**
   * Remove data about for per Exporter && Session
   * @param key IPDR35XMLOutputterDataKey
   */

  public void removeDocumentData(IPDR35XMLOutputterDataKey key) {
    mExporterSessionDocumentMap.remove(key);
    mExporterSessionXMLFileMap.remove(key);

  }


  /**
   * Enlarge records counter of specified XML output file
   * @param key IPDR35XMLOutputterDataKey
   */

  public void enlargeRecordsCounter(IPDR35XMLOutputterDataKey key) {
    IPDR35XMLOutputterRecordData recordData = (IPDR35XMLOutputterRecordData)
        mExporterSessionXMLFileMap.get(key);
    if(recordData == null)
    {
      LogManager.error(this, "enlargeRecordsCounter",
                       getNotRegisteredSessionMessage(key)
                       );

      return;
    }
    recordData.enlargeRecordsCounter();
  }



  /**
   * Return records counter of specified XML output file
   * @param key IPDR35XMLOutputterDataKey
   * @return records counter int
   */

  public int getRecordsCounter(IPDR35XMLOutputterDataKey key) {
    IPDR35XMLOutputterRecordData recordData = (IPDR35XMLOutputterRecordData)
        mExporterSessionXMLFileMap.get(key);
    if(recordData == null)
    {
      LogManager.error(this, "getRecordsCounter",
                       getNotRegisteredSessionMessage(key)
                       );

      return 0;
    }

    return recordData.getRecordsCounter();
  }

  /**
   * Put last record date of specified XML output file
   * @param key IPDR35XMLOutputterDataKey
   * @param last record date  Date
   */

  public void setLastRecordDate(IPDR35XMLOutputterDataKey key,
                                Date lastRecordDate) {
    IPDR35XMLOutputterRecordData recordData = (IPDR35XMLOutputterRecordData)
        mExporterSessionXMLFileMap.get(key);
    if(recordData == null)
    {
      LogManager.error(this, "setLastRecordDate",
                       getNotRegisteredSessionMessage(key)
                       );

      return;
    }

    recordData.setRecordDate(lastRecordDate);
  }

  /**
   * Return last record date of specified XML output file
   * @param key IPDR35XMLOutputterDataKey
   * @return last record date Date
   */

  public Date getLastRecordDate(IPDR35XMLOutputterDataKey key) {
    IPDR35XMLOutputterRecordData recordData = (IPDR35XMLOutputterRecordData)
        mExporterSessionXMLFileMap.get(key);
    if(recordData == null)
    {
      LogManager.error(this, "getLastRecordDate",
                       getNotRegisteredSessionMessage(key)
                       );

      return new Date();
    }

    return recordData.getLastRecordDate();

  }

  /**
   * Get exisiting FileWriter of XML file name for Exporter & Session
   * @param key IPDR35XMLOutputterDataKey
   * @return FileWriter
   */

  public FileWriter getXMLFileWriter(IPDR35XMLOutputterDataKey key) {
    IPDR35XMLOutputterRecordData dataHandler = (IPDR35XMLOutputterRecordData)
        mExporterSessionXMLFileMap.get(key);
    if (dataHandler == null) {
      {
        LogManager.error(this, "getXMLFileWriter",
                         getNotRegisteredSessionMessage(key)
                         );

        return null;
      }
    }
    return dataHandler.getXMLFileWriter();
  }


  /**
   * Return a list of IPDR35XMLOutputterRecordData for each open document
   * @return list of IPDR35XMLOutputterRecordData ArrayList
   */

  public ArrayList getAllOpenDocuments() {
    ArrayList dataList = new ArrayList();
    Iterator filesIter = mExporterSessionDocumentMap.keySet().iterator();
    while (filesIter.hasNext()) {
      IPDR35XMLOutputterDataKey dataKey = (IPDR35XMLOutputterDataKey) filesIter.
          next();
      dataList.add(dataKey);
    }

    return dataList;
  }

  // Combine an error log message if IPDR35XMLOutputterDataKey was't found
  private String getNotRegisteredSessionMessage(IPDR35XMLOutputterDataKey key)
  {
    StringBuffer message = new StringBuffer();
    message.append("Session with parameters 'Exporter address' ");
    message.append(key.getExporterAddress().getHostAddress());
    message.append(" 'Exporter port' ");
    message.append(key.getExporterPort());
    message.append(" and 'Session id' ");
    message.append(key.getSessionID());
    message.append(" is not registered yet");
    return message.toString();

  }



}
