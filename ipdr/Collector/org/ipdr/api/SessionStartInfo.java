package org.ipdr.api;

//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
//ANY KIND either expresses or implied.
/**
 * The interface represents a SESSION_START message information.
 */

public interface SessionStartInfo {
  /**
   * Returns the ackSequenceInterval.
   *
   * @return int
   */
  public int getAckSequenceInterval();

  /**
   * Returns the ackTimeInterval.
   *
   * @return int
   */
  public int getAckTimeInterval();

  /**
   * Returns the documentId.
   *
   * @return byte[]
   */
  public byte[] getDocumentId();

  /**
   * Returns the droppedRecordCount.
   *
   * @return long
   */
  public long getDroppedRecordCount();

  /**
   * Returns the exporterBootTime.
   *
   * @return int
   */
  public int getExporterBootTime();

  /**
   * Returns the firstRecordSequenceNumber.
   *
   * @return long
   */
  public long getFirstRecordSequenceNumber();

  /**
   * Returns the primary collector flag.
   *
   * @return boolean
   */
  public boolean isPrimary();

  /**
   * Returns the sessionId.
   * @return byte
   */
  public byte getSessionId();

}
