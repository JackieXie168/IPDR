package org.ipdr;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.net.InetAddress;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.Hashtable;

import org.ipdr.api.Outputter;
import org.ipdr.api.OutputterException;
import org.ipdr.api.StateMachineListener;
import org.ipdr.api.configuration.ConfigException;
import org.ipdr.api.configuration.SessionConfigI;
import org.ipdr.protocol.DataException;
import org.ipdr.protocol.InvalidSessionStateException;
import org.ipdr.protocol.ProtocolMalformedMsgException;
import org.ipdr.protocol.ProtocolMsgDATA;
import org.ipdr.protocol.ProtocolMsgMODIFY_TEMPLATE_RESPONSE;
import org.ipdr.protocol.ProtocolMsgSESSION_START;
import org.ipdr.protocol.ProtocolMsgTEMPLATE_DATA;
import org.ipdr.protocol.SessionException;

/**
 * Represents connection state for a connection
 */
public class IPDRConnectionState
{
    public static final int      DISCONNECTED              = 0;
    public static final int      CONNECTING                = 1;
    public static final int      CONNECTED                 = 2;
    public static final int      SESSION_INITIATION        = 3;
    public static final int      READY                     = 4;
    public static final int      ACTIVE_SESSION            = 5;
    public static final int      HANDLING_CONNECTION_ERROR = 6;

    /**
     * Holds the state machine
     */
    private IPDRStateMachine parent;
    
    /**
     * Holds the actual state
     */
    private int                  state;
    /**
     * Holds state machine listener instance
     */
    private StateMachineListener listener;
    /**
     * Holds session id to session state map
     */
    private Hashtable            sessionMap                = new Hashtable();
    /**
     * Holds instance of the outputter
     */
    private Outputter            outputter = null;

    /**
     * Holds state of templates. If they are already known, TEMPLATE_DATA
     * needn't be expected before SESSION_START
     */
    private boolean              templatesKnown            = false;

    /**
     * Initializes the object with the given initial state and outputter object
     * 
     * @param state -
     *            initial state of the connection
     * @param outputter -
     *            instance of <code>Outputter</code>.
     */
    public IPDRConnectionState(IPDRStateMachine parent,int state, Outputter outputter)
    {
        this.parent = parent;
        this.state = state;
        this.outputter = outputter;
    }
    
    /**
     * Initializes the object with the given initial state, outputter and state
     * machine listener.
     * 
     * @param state -
     *            initial state
     * @param outputter -
     *            instance of <code>Outputter</code>
     * @param listener -
     *            instance of <code>StateMachineListener</code>
     */
    public IPDRConnectionState(IPDRStateMachine parent,int state, Outputter outputter,
            StateMachineListener listener)
    {
        this(parent, state, outputter);
        this.listener = listener;
    }

    /**
     * Initializes the object with the given initial state
     * 
     * @param state -
     *            initial state of the connection
     */
    public IPDRConnectionState(IPDRStateMachine parent, int state)
    {
        this.parent = parent;
        this.state = state;
    }
    
    /**
     * Initializes the object with the given initial state and state
     * machine listener.
     * 
     * @param state -
     *            initial state
     * @param listener -
     *            instance of <code>StateMachineListener</code>
     */
    public IPDRConnectionState(IPDRStateMachine parent, int state, StateMachineListener listener)
    {
        this(parent, state);
        this.listener = listener;
    }
    
    /**
     * Sets the outputter
     * 
     * @param outputter -
     *            instance of <code>Outputter</code>
     */
    public void setOutputter(Outputter outputter)
    {
        this.outputter = outputter;
    }
    
    /**
     * Returns the state.
     * 
     * @return int
     */
    public int getState()
    {
        return state;
    }

    /**
     * Performs state transition
     * 
     * @param state
     *            New target state
     */
    public void toState(int state)
    {
        this.state = state;
    }

    /**
     * Adds a new session
     * 
     * @param sessionConfig -
     *            configuration of the desired session
     * @throws ConfigException
     */
    public void addSession(SessionConfigI sessionConfig) throws ConfigException
    {
        Byte sessionId = new Byte(sessionConfig.getSessionId());
        if (sessionConfig == null)
            throw new ConfigException("Session configuration is null");
        if (sessionMap.get(sessionId) != null)
            throw new ConfigException("Session already started");
        sessionMap.put(sessionId, new IPDRSessionState(sessionConfig, this));
    }

    /**
     * Removes an existing session
     * 
     * @param sessionId -
     *            id of session to remove
     */
    public void removeSession(byte sessionId)
    {
        Byte sessionIdObj = new Byte(sessionId);
        if (sessionMap.get(sessionIdObj) != null)
            sessionMap.remove(sessionIdObj);
    }
    
    /**
     * Returns true if the session exists
     * @param sessionId
     * @return
     */
    public boolean hasSession(byte sessionId)
    {
        Byte sessionIdObj = new Byte(sessionId);
        return (sessionMap.get(sessionIdObj) != null);
    }

    /**
     * Handles template data by forwarding the incoming templates to the
     * appropriate <code>IPDRSessionState</code> instance
     * 
     * @param message -
     *            TEMPLATE_DATA message
     * @throws InvalidSessionStateException
     * @throws ConfigException
     */
    public void handleTemplateData(ProtocolMsgTEMPLATE_DATA message)
            throws InvalidSessionStateException, ConfigException
    {
        IPDRSessionState sessionState = getSession(message.getSessionId());
        sessionState.processTemplateData(message);
        this.templatesKnown = true;
    }

    /**
     * Handles modify template response by forwarding the incoming templates to
     * the appropriate <code>IPDRSessionState</code> instance
     * 
     * @param message -
     *            MODIFY_TEMPLATE_RESPONSE message
     * @throws InvalidSessionStateException
     * @throws ConfigException
     */
    public void handleModifyTemplateResponse(
            ProtocolMsgMODIFY_TEMPLATE_RESPONSE message)
            throws InvalidSessionStateException, ConfigException
    {
        IPDRSessionState sessionState = getSession(message.getSessionId());
        sessionState.processModifyTemplateResponse(message);
        this.templatesKnown = true;
    }

    /**
     * Handles session start message.
     * 
     * @param sessionId -
     *            id of session
     * @param message -
     *            SESSION_START message
     * @throws InvalidSessionStateException
     */
    public void handleSessionStart(byte sessionId,
            ProtocolMsgSESSION_START message)
            throws InvalidSessionStateException
    {
        IPDRSessionState sessionState = getSession(sessionId);
        sessionState.processSessionStart(message);
    }

    /**
     * Gets data acknowledge timeout for a given session
     * 
     * @param sessionId -
     *            session id
     * @return timeout in seconds
     * @throws InvalidSessionStateException
     */
    public int getSessionAckTimeInterval(byte sessionId)
            throws InvalidSessionStateException
    {
        IPDRSessionState sessionState = getSession(sessionId);
        return sessionState.getAckTimeInterval();
    }

    /**
     * Returns true if the data message must be acknowledged
     * 
     * @param sessionId
     * @return true if the data must be acknowledged
     * @throws InvalidSessionStateException
     */
    public boolean getSessionAcknowledge(byte sessionId)
            throws InvalidSessionStateException
    {
        IPDRSessionState sessionState = getSession(sessionId);
        return sessionState.acknowledge();
    }
    
    
    /**
     * Returns count of yet unacknowledged messages
     * @param sessionId
     * @return count
     * @throws InvalidSessionStateException
     */
    public int getDataAckCount(byte sessionId)
    throws InvalidSessionStateException
    {
        IPDRSessionState sessionState = getSession(sessionId);
        return sessionState.getDataAckCount();
    }

    /**
     * Resets session data acknowledge counter. This method is called when an
     * acknowledge message is sent as a result of a timeout
     * 
     * @param sessionId
     * @throws InvalidSessionStateException
     */
    public void resetSessionAcknowledge(byte sessionId)
            throws InvalidSessionStateException
    {
        IPDRSessionState sessionState = getSession(sessionId);
        sessionState.resetSessionAcknowledge();
    }

    /**
     * @see java.lang.Object#toString()
     */
    public String toString()
    {
        switch (state)
        {
            case DISCONNECTED:
                return "DISCONNECTED";
            case CONNECTING:
                return "CONNECTING";
            case CONNECTED:
                return "CONNECTED";
            case SESSION_INITIATION:
                return "SESSION_INITIATION";
            case READY:
                return "READY";
            case ACTIVE_SESSION:
                return "ACTIVE_SESSION";
            default:
                return "UNKNOWN";
        }
    }

    /**
     * Gets number of active sessions
     * 
     * @return number of active sessions
     */
    public int getFlowCount()
    {
        return sessionMap.size();
    }

    /**
     * Gets list of session ids
     * 
     * @return ArrayList of session ids
     */
    public ArrayList getSessionIdList()
    {
        ArrayList list = new ArrayList(sessionMap.size());
        Enumeration keys = sessionMap.keys();
        while (keys.hasMoreElements())
        {
            list.add(keys.nextElement());
        }
        return list;
    }

    /**
     * Gets session state
     * 
     * @param id -
     *            session id
     * @return state of the specific session
     * @throws InvalidSessionStateException
     */
    public int getSessionState(byte id) throws InvalidSessionStateException
    {
        IPDRSessionState session = getSession(id);
        return session.getState();

    }
    
    /**
     * Gets first record sequence number for a session
     * @param id - session id
     * @return first record sequence number of the specific session
     * @throws InvalidSessionStateException
     */
    public long getFirstRecordSequenceNumber(byte id) throws InvalidSessionStateException
    {
        IPDRSessionState session = getSession(id);
        return session.getFirstRecordSequenceNumber();
    }
    
    /**
     * Gets current record sequence number for a session
     * @param id - session id
     * @return first record sequence number of the specific session
     * @throws InvalidSessionStateException
     */
    public long getCurrentRecordSequenceNumber(byte id) throws InvalidSessionStateException
    {
        IPDRSessionState session = getSession(id);
        return session.getCurrentRecordSequenceNumber();
    }

    /**
     * Retrieves <code>IPDRSessionState</code> object for the given session
     * 
     * @param id
     * @return IPDRSessionState object
     * @throws InvalidSessionStateException
     */
    protected IPDRSessionState getSession(byte id)
            throws InvalidSessionStateException
    {
        Byte sessionId = new Byte(id);
        IPDRSessionState session = (IPDRSessionState) sessionMap.get(sessionId);
        if (session == null)
            throw new InvalidSessionStateException(id, "Session not found");
        return session;
    }

    /**
     * Sets a new state for the session.
     * 
     * @param id -
     *            session id
     * @param state -
     *            new state
     * @throws InvalidSessionStateException
     */
    public void setSessionState(byte id, int state)
            throws InvalidSessionStateException
    {
        IPDRSessionState session = getSession(id);
        session.setState(state);
    }

    /**
     * Sets configId as provided by SESSION_START message
     * 
     * @param id
     *            session id
     * @param configId
     *            new config Id
     * @throws InvalidSessionStateException
     */
    public void setSessionConfigId(byte id, short configId)
            throws InvalidSessionStateException
    {
        IPDRSessionState session = getSession(id);
        session.setConfigId(configId);
    }

    /**
     * Outputs the data message
     * 
     * @param message -
     *            DATA message to output
     * @throws SessionException
     * @throws ProtocolMalformedMsgException
     * @throws OutputterException
     */
    public void outputDataMessage(ProtocolMsgDATA message)
            throws SessionException, ProtocolMalformedMsgException,
            OutputterException
    {
        try
        {
            IPDRSessionState session = getSession(message.getSessionId());
            session.outputDataMessage(message);
        }
        catch (DataException e)
        {
            throw new ProtocolMalformedMsgException("Data error", message
                    .getRawData(), 0, 0);
        }
    }

    /**
     * Returns the outputter.
     * 
     * @return Outputter
     */
    public Outputter getOutputter()
    {
        return outputter;
    }

    /**
     * Returns the templatesKnown.
     * 
     * @return boolean
     */
    public boolean isTemplatesKnown()
    {
        return templatesKnown;
    }

    /**
     * @return Returns real exporter address
     */
    public InetAddress getRealExporterAddress()
    {
        return parent.getRealExporterAddress();
    }

    /**
     * @return Returns real exporter port
     */
    public int getRealExporterPort()
    {
        return parent.getRealExporterPort();
    }

}

