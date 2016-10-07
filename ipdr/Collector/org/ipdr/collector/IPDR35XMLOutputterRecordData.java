package org.ipdr.collector;

//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
//ANY KIND either expresses or implied.
import java.util.Date;
import java.io.FileWriter;
/**
 * The class contains information about XML output file records
 */

public class IPDR35XMLOutputterRecordData {
  private int mRecordsCounter = 0;
  private Date mLastRecordDate;
  private FileWriter mXMLFileWriter;

  /**
   * Constructor
   */

  public IPDR35XMLOutputterRecordData() {
  }


  /**
   * Set last record date per XML file
   * @param recordDate last record date
   */

  public void setRecordDate(Date recordDate) {
    mLastRecordDate = recordDate;
  }


  /**
   * Enlarge records counter by 1 per XML file
   */

  public void enlargeRecordsCounter() {
    mRecordsCounter++;
  }

  /**
   * Return current counter of records per XML file
   * @return int current counter
   */

  public int getRecordsCounter() {
    return mRecordsCounter;
  }

  /**
   * Return last record date per XML file
   * @return Date last record date
   */

  public Date getLastRecordDate() {
    return mLastRecordDate;
  }

  /**
   * Set XML FileWriter
   * @param FileWriter
   */

  public void setXMLFileWriter(FileWriter aFileWriter) {
    mXMLFileWriter = aFileWriter;
  }

  /**
   * Return XML FileWriter
   * @return FileWriter
   */

  public FileWriter getXMLFileWriter() {
    return mXMLFileWriter;
  }


}
