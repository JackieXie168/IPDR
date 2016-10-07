package org.ipdr.collector;

//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
//ANY KIND either expresses or implied.

import java.net.InetAddress;

import org.ipdr.api.DisconnectInfo;
import org.ipdr.api.StateMachineListener;
import org.ipdr.api.log.LogManager;

import java.util.ArrayList;
import java.util.Hashtable;
import org.ipdr.api.*;

/**
 * State Machine Listener implements StateMachineListener, that manages all registered instances of StateMachineListener.
*  StateMachineListenerManager behaviour reminnds the Observer design pattern. When an interface method is called,
*  StateMachineListenerManager finds the StateMachineListener instance by unique ExporterRegistrationKey among  previously registered StateMachineListeners and calls the same
*  interface method on it.
 */

public class StateMachineListenerManager
    implements StateMachineListener {
  private Hashtable mRegisteredListeners;

  /**
   * Constructor
   */

  public StateMachineListenerManager() {
    mRegisteredListeners = new Hashtable();
  }

  /**
   * @see org.ipdr.api.StateMachineListener#notifyDisconnected(java.net.InetAddress, int)
   */
  public void notifyDisconnected(InetAddress address, int port) {
    LogManager.debug(this, "notifyDisconnected", "LISTENER:disconnected " +
                     String.valueOf(address) + ":" + port);
    if (null == address) {
      return;
    }
    ExporterRegistrationKey registrationKey = new ExporterRegistrationKey(
        address, port);
    StateMachineListener listener = (StateMachineListener) mRegisteredListeners.
        get(registrationKey);
    if (listener != null) {
      listener.notifyDisconnected(address, port);
    }

  }

  /**
   * @see org.ipdr.api.StateMachineListener#notifyConnected(java.net.InetAddress, int, ConnectInfo)
   */
  public void notifyConnected(InetAddress address, int port, ConnectInfo info) {
    LogManager.debug(this, "notifyConnected",
                     "LISTENER:Connected " + address.toString() + ":" + port);
    ExporterRegistrationKey registrationKey = new ExporterRegistrationKey(
        address, port);
    StateMachineListener listener = (StateMachineListener) mRegisteredListeners.
        get(registrationKey);
    if (listener != null) {
      listener.notifyConnected(address, port, info);
    }

  }

  /**
   * @see org.ipdr.api.StateMachineListener#notifyFlowStarted(java.net.InetAddress, int, FlowStartInfo)
   */
  public void notifyFlowStarted(InetAddress address, int port,
                                FlowStartInfo info) {
    LogManager.debug(this, "notifyFlowStarted",
                     "LISTENER:flow started " + address.toString() + ":" + port +
                     " session:" + info.getSessionId());
    ExporterRegistrationKey registrationKey = new ExporterRegistrationKey(
        address, port);
    StateMachineListener listener = (StateMachineListener) mRegisteredListeners.
        get(registrationKey);
    if (listener != null) {
      listener.notifyFlowStarted(address, port, info);
    }

  }

  /**
   * Call to "notifySessionStarted" of registered StateMachineListener by its Exporter address and port
   * @see org.ipdr.api.StateMachineListener#notifySessionStarted(java.net.InetAddress, int, SessionStartInfo)
   */
  public void notifySessionStarted(InetAddress address, int port,
                                   SessionStartInfo info) {
    LogManager.debug(this, "notifySessionStarted",
                     "LISTENER:session started " + address.toString() + ":" +
                     port + " session:" + info.getSessionId());
    ExporterRegistrationKey registrationKey = new ExporterRegistrationKey(
        address, port);
    StateMachineListener listener = (StateMachineListener) mRegisteredListeners.
        get(registrationKey);
    if (listener != null) {
      listener.notifySessionStarted(address, port, info);
    }

  }

  /**
   * Call to "notifySessionStopped" of registered StateMachineListener by its Exporter address and port
   * @see org.ipdr.api.StateMachineListener#notifySessionStopped(java.net.InetAddress, int, SessionStopInfo)
   */
  public void notifySessionStopped(InetAddress address, int port,
                                   SessionStopInfo info) {
    LogManager.debug(this, "notifySessionStopped",
                     "LISTENER:session stopped " + address.toString() + ":" +
                     port + " session:" + info.getSessionId());
    ExporterRegistrationKey registrationKey = new ExporterRegistrationKey(
        address, port);
    StateMachineListener listener = (StateMachineListener) mRegisteredListeners.
        get(registrationKey);
    if (listener != null) {
      listener.notifySessionStopped(address, port, info);
    }

  }

  /**
   * @see org.ipdr.api.StateMachineListener#notifyFlowStopped(java.net.InetAddress, int, FlowStopInfo)
   */
  public void notifyFlowStopped(InetAddress address, int port,
                                FlowStopInfo info) {
    LogManager.debug(this, "notifyFlowStopped",
                     "LISTENER:flow stopped " + address.toString() + ":" + port +
                     " session:" + info.getSessionId() + " reason:" +
                     info.getReasonCode() + " msg:" + info.getReasonInfo());
    ExporterRegistrationKey registrationKey = new ExporterRegistrationKey(
        address, port);
    StateMachineListener listener = (StateMachineListener) mRegisteredListeners.
        get(registrationKey);
    if (listener != null) {
      listener.notifyFlowStopped(address, port, info);
    }

  }

  /**
   * Register  instances of StateMachineListener by unique ExporterRegistrationKey.
   * If ExporterRegistrationKey is previously registered, StateMachineListenerManager overrides it.
   * @param listener instance of StateMachineListener
   * @param aExporterAddress IP address of Exporter
   * @param aExporterPort Exporter's port
   */

  public void registerListener(StateMachineListener listener,
                               InetAddress aExporterAddress, int aExporterPort) {
    LogManager.debug(this, "registerListener",
                     "REGISTER LISTENER " + aExporterAddress.toString() + ":" + aExporterPort);

    ExporterRegistrationKey registrationKey = new ExporterRegistrationKey(
        aExporterAddress, aExporterPort);
    mRegisteredListeners.put(registrationKey, listener);
  }

  /**
   * Unegister instances of StateMachineListener by Exporter
   * @param aExporterAddress IP address of Exporter
   * @param aExporterPort Exporter's port
   */

  public void unRegisterListener(InetAddress aExporterAddress,
                                 int aExporterPort) {
    ExporterRegistrationKey registrationKey = new ExporterRegistrationKey(
        aExporterAddress, aExporterPort);
    mRegisteredListeners.remove(registrationKey);
  }

  /**
   * Inner class of StateMachineListenerManager for managing registrated state mashine listeners
   */

  private class ExporterRegistrationKey {
    private InetAddress exporterAddress;
    private int exporterPort;

    /**
     * Constructor
     *
     * @param aExporterAddress
     *            Exporter's IP address
     * @param aPort
     *            Exporter's port
     */

    public ExporterRegistrationKey(InetAddress aExporterAddress,
                                   int aPort) {
      exporterAddress = aExporterAddress;
      exporterPort = aPort;

    }

    public int hashCode() {


      return (Integer.toString(exporterPort) +
              exporterAddress.getHostAddress()).hashCode();
          }

    /**
     * Return Exporter's port
     * @return exporterPort int
     */

    public int getExporterPort() {
      return exporterPort;
    }

    /**
     * Return Exporter's IP address
     * @return exporterAddress InetAddress
     */

    public InetAddress getExporterAddress() {
      return exporterAddress;
    }

    public boolean equals(Object obj) {
      if (obj instanceof ExporterRegistrationKey) {
        if (
            ( (ExporterRegistrationKey) obj).getExporterAddress() ==
            exporterAddress &&
            ( (ExporterRegistrationKey) obj).getExporterPort() ==
            exporterPort) {
          return true;
        }
      }
      return false;

    }
  }

/* (non-Javadoc)
 * @see org.ipdr.api.StateMachineListener#notifyTemplatesNegotiated(java.net.InetAddress, int, byte, java.util.ArrayList)
 */
public void notifyTemplatesNegotiated(InetAddress address, int port, byte sessionId, ArrayList templates) {
	// TODO Auto-generated method stub

}



}
