package org.ipdr.collector;

//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
//ANY KIND either expresses or implied.

import java.net.InetAddress;

/**
 * This class is a unique key for documents management in IPDR35XMLDocDataHandler
 */

class IPDR35XMLOutputterDataKey {
  private int sessionID;
  private InetAddress exporterAddress;
  private int exporterPort;

  /**
   * Constructor
   *
   * @param aExporterAddress
   *            Exporter's IP address
   * @param aPort
   *            Exporter's port
   * @param aSessionID
   *            Session id
   */

  public IPDR35XMLOutputterDataKey(InetAddress aExporterAddress,
                                   int aPort, int aSessionID) {
    sessionID = aSessionID;
    exporterAddress = aExporterAddress;
    exporterPort = aPort;

  }

  public int hashCode() {
    return (Integer.toString(sessionID) + Integer.toString(exporterPort) +
            exporterAddress.getHostAddress()).hashCode();
  }

  public int getSessionID() {
    return sessionID;
  }

  public int getExporterPort() {
    return exporterPort;
  }

  public InetAddress getExporterAddress() {
    return exporterAddress;
  }

  public boolean equals(Object obj) {
    if (obj instanceof IPDR35XMLOutputterDataKey) {
      if ( ( (IPDR35XMLOutputterDataKey) obj).getSessionID() == sessionID
          &&
          ( (IPDR35XMLOutputterDataKey) obj).getExporterAddress() ==
          exporterAddress &&
          ( (IPDR35XMLOutputterDataKey) obj).getExporterPort() == exporterPort) {
        return true;
      }
    }
    return false;

  }
}
