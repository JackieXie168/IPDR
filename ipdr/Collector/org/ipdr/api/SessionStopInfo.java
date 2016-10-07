package org.ipdr.api;







//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
//ANY KIND either expresses or implied.
/**
 * The interface represents a SESSION_STOP message information.
 */
public interface SessionStopInfo {

  /**
   * Returns the sessionId.
   * @return byte
   */
  public byte getSessionId();

  /**
   * Returns the reasonCode.
   *
   * @return short
   */

  public short getReasonCode();

  /**
   * Returns the reasonInfo.
   *
   * @return String
   */
  public String getReasonInfo();

}
