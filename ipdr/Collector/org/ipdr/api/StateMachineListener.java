package org.ipdr.api;

//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
//ANY KIND either expresses or implied.

import java.net.InetAddress;
import java.util.ArrayList;

/**
 * The interface provides a convenient way to peek into protocol state machine
 */
public interface StateMachineListener {
  /**
   * Receives notification on disconnection
   * @param address IP address of the disconnecting exporter
   * @param port TCP port of the disconnecting exporter
   * @param DISCONNECT message information
   */
  public void notifyDisconnected(InetAddress address, int port);

  /**
   * Receives notification on connection
   * @param address IP address of the connected exporter
   * @param port TCP port of the connected exporter
   * @param CONNECT message information
   */
  public void notifyConnected(InetAddress address, int port, ConnectInfo info);

  /**
   * Receives notifications on flow start
   * @param address IP address of the exporter
   * @param port TCP port of the exporter
   * @param FLOW START message information
   */
  public void notifyFlowStarted(InetAddress address, int port,
                                FlowStartInfo info);

  /**
   * Receives notifications on session start
   * @param address IP address of the exporter
   * @param port TCP port of the exporter
   * @param SESSION_START message information
   */
  public void notifySessionStarted(InetAddress address, int port,
                                   SessionStartInfo info);

  /**
   * Receives notifications on session stop
   * @param address IP address of the exporter
   * @param port TCP port of the exporter
   * @param SESSION_STOP message information
   */
  public void notifySessionStopped(InetAddress address, int port,
                                   SessionStopInfo info);

  /**
   * Receives notifications on flow stop
   * @param address IP address of the exporter
   * @param port TCP port of the exporter
   * @param FLOW STOP message information
   */
  public void notifyFlowStopped(InetAddress address, int port,
                                FlowStopInfo info);
  
  /**
   * Receives notifications on template negotiation
   * @param address IP address of the exporter
   * @param port TCP port of the exporter
   * @param sessionId The session where the negotiated templates exists 
   * @param templates The templates to be negotiated
 */
public void notifyTemplatesNegotiated(InetAddress address, int port,
  								byte sessionId, ArrayList templates);
}
