package org.ipdr;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.util.ArrayList;

import org.ipdr.api.ConnectionIOException;
import org.ipdr.api.configuration.ConfigException;
import org.ipdr.protocol.ConnectionException;
import org.ipdr.protocol.InvalidSessionStateException;
import org.ipdr.protocol.ProtocolMalformedMsgException;

/**
 * This class defines an abstract state machine, suitable for both CRANE and
 * IPDR
 */
public abstract class ProtocolStateMachine
{
    /**
     * Starts the underlying protocol
     * 
     * @throws ConnectionException
     * @throws ConfigException
     * @throws ProtocolMalformedMsgException
     */
    public abstract void startProtocol() throws ConnectionException,
            ConfigException, ProtocolMalformedMsgException;

    /**
     * Notifies the state machine of connect timeout
     */
    public abstract void connectTimeout();

    /**
     * Notifies the state machine of data ack timeout
     * 
     * @param sessionId
     *            session id
     */
    public abstract void dataAckTimeout(byte sessionId);

    /**
     * Sets connection to use
     * 
     * @param connection
     */
    public void setConnection(ConnectionDynamic connection)
    {
        this.connection = connection;
    }

    /**
     * Holds the connection to use
     */
    protected ConnectionDynamic connection;

    /**
     * Indicates exit state for the main loop
     * 
     * @return true if the main loop may exit
     */
    public abstract boolean canExit();

    /**
     * Initiates disconnection of a session
     * 
     * @param sessionReasonCode
     *            reason code for session
     * @param flowReasonCode
     *            reason code for FLOW_STOP
     * @param reasonInfo
     *            verbal reason description
     * @throws ConfigException
     * @throws ProtocolMalformedMsgException
     * @throws ConnectionIOException
     */
    public abstract void initiateDisconnection(short sessionReasonCode,
            short flowReasonCode, String reasonInfo) throws ConfigException,
            ProtocolMalformedMsgException, ConnectionIOException;
    
    public abstract void stopFlow(byte sessionId);

    /**
     * Performs clean up of the state machine
     */
    public abstract void cleanup();

    /**
     * Runs the main loop, listening on the previously provided connection for
     * incoming messages
     * 
     * @throws InterruptedException
     * @throws ConnectionIOException
     * @throws ConfigException
     */
    public abstract void mainLoop() throws InterruptedException,
            ConnectionIOException, ConfigException;

    /**
     * Gets list of session ids
     * @return <code>ArrayList</code> of session ids
     */
    public abstract ArrayList getSessionList();

    /**
     * Returns session state of the specific session
     * @param sessionId session id to retrieve state for
     * @return session state
     * @throws InvalidSessionStateException
     */
    public abstract int getSessionState( byte sessionId ) throws InvalidSessionStateException;
}