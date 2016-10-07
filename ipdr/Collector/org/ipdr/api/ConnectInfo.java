package org.ipdr.api;

//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
//ANY KIND either expresses or implied.
/**
 * The interface represents a Connect message information.
 */

public interface ConnectInfo {
	
  /**
   * Returns structured templates capability
   * @return boolean
   */
  public boolean getCapabilitiesStructures();

  /**
   * Returns multisession capability
   * @return boolean
   */
  public boolean getCapabilitiesMultiSession();

  /**
   * Returns template negotiation capability
   * @return boolean
   */
  public boolean getCapabilitiesTemplateNegotiation();
 
  /**
   * Returns the keepAliveInterval.
   * @return long
   */
  public long getKeepAliveInterval();

  /**
   * Returns the vendorId.
   * @return String
   */
  public String getVendorId();
  
}
