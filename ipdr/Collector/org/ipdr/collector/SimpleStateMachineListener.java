package org.ipdr.collector;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.net.InetAddress;
import java.util.ArrayList;

import org.ipdr.api.ConnectInfo;
import org.ipdr.api.DisconnectInfo;
import org.ipdr.api.FlowStartInfo;
import org.ipdr.api.FlowStopInfo;
import org.ipdr.api.SessionStartInfo;
import org.ipdr.api.SessionStopInfo;
import org.ipdr.api.StateMachineListener;
import org.ipdr.api.log.LogManager;

/**
 * The class provides simple logging implementation of state machine listener
 */
public class SimpleStateMachineListener implements StateMachineListener
{
    
    public void notifyDisconnected(InetAddress address, int port,
                                   DisconnectInfo info)
    {
    	 LogManager.debug(this, "notifyDisconnected", "LISTENER:disconnected " + String.valueOf(address) + ":" + port);
    }
    
    public void notifyDisconnected(InetAddress address, int port)
    {
    	LogManager.debug(this, "notifyDisconnected", "LISTENER:disconnected " + String.valueOf(address) + ":" + port);
    }

    public void notifyConnected(InetAddress address, int port, ConnectInfo info)
    {
    	LogManager.debug(this, "notifyConnected", "LISTENER:Connected " + address.toString() + ":" + port);
    }

    public void notifyFlowStarted(InetAddress address, int port,
                                  FlowStartInfo info)
    {
    	LogManager.debug(this, "notifyFlowStarted", "LISTENER:flow started " + address.toString() + ":" + port + " session:" + info.getSessionId());

    }
   
    public void notifySessionStarted(InetAddress address, int port,
                                     SessionStartInfo info)
    {
    	LogManager.debug(this, "notifySessionStarted", "LISTENER:session started " + address.toString() + ":" + port + " session:" + info.getSessionId());
    }

    public void notifySessionStopped(InetAddress address, int port,
                                     SessionStopInfo info)
    {
    	LogManager.debug(this, "notifySessionStopped", "LISTENER:session stopped " + address.toString() + ":" + port + " session:" + info.getSessionId());
    }

    public void notifyFlowStopped(InetAddress address, int port,
                                  FlowStopInfo info)
    {
    	LogManager.debug(this, "notifyFlowStopped", "LISTENER:flow stopped " + address.toString() + ":" + port + " session:" + info.getSessionId() + " reason:" + info.getReasonCode() + " msg:" + info.getReasonInfo());
    }
    
    public void notifyTemplatesNegotiated(InetAddress address, int port,
				byte sessionId, ArrayList templates)
    {
    	LogManager.debug(this, "notifyTemplatesNegotiated", "LISTENER:Templates negotiated " + address.toString() + ":" + port + " session:" + sessionId);
    }
}
