package org.ipdr.api;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
//ANY KIND either expresses or implied.




//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
//ANY KIND either expresses or implied.

/**
 * An interface for creating new CollectorOutputter objects.
 */
import java.net.InetAddress;

public interface OutputterFactory {
  /**
   * Returns a new instance of an <code>Outputter</code>
   * @param transmitterAddress Exporter's ip address
   * @param transmitterPort Exporter's port
   * @return An instance of <code>Outputter</code>
   */
  public Outputter create(InetAddress transmitterAddress,
                          int transmitterPort);
}
