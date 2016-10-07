package org.ipdr.api;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.net.InetAddress;
import java.util.ArrayList;

import org.ipdr.ConnectionDynamic;
import org.ipdr.ExporterHandler;
import org.ipdr.IPDRStateMachine;
import org.ipdr.IncomingConnectionHandler;
import org.ipdr.ProtocolStateMachine;
import org.ipdr.api.configuration.ConfigException;
import org.ipdr.api.configuration.ConnectionConfigI;
import org.ipdr.api.log.LogManager;
import org.ipdr.protocol.ConnectionException;
import org.ipdr.protocol.InvalidSessionStateException;
import org.ipdr.protocol.ProtocolMalformedMsgException;

/**
 * The class represents a single connection thread
 */
public class ConnectionThread extends Thread
{
	/**
     * Stores interface for handling incoming connections 
     */
    protected IncomingConnectionHandler connectionHandler = null;
    
    /**
     * Stores parent <code>ExporterHandler</code> instance
     */
    protected ExporterHandler parent = null;
    /**
     * Stores connection configuration
     */
    protected ConnectionConfigI    config = null;
    /**
     * Stores underlying <code>ConnectionDynamic</code> instance
     */
    protected ConnectionDynamic    connection         = null;
    /**
     * Indicats that the connection is active
     */
    protected boolean              isActiveConnection = true;

    /**
     * Is true while attempts to connect should continue
     */
    protected volatile boolean keepConnecting = true;
    
    /**
     * Stores protocol version for the connection
     */
    protected int                  protocolVersion;
    /**
     * Stores an instance of either CRANE or IPDR state machine
     */
    protected ProtocolStateMachine stateMachine;
    /**
     * Stores an instance of an Outputter
     */
    protected Outputter            outputter = null;

    /**
     * Stores an instance of template negotiator
     */
    protected TemplateNegotiator   templateNegotiator = null;
    
    /**
     * Stores an instance of state machine listener
     */
    protected StateMachineListener stateMachineListener = null;
     
    /**
     * Stores an instance of connection controller
     */
    protected ConnectionController connectionController = null;
    
    /**
     * Stores an instance of probing listener
     */
    private ProbingListener probingListener = null;
    
    /**
     * Stores an instance of an Outputter factory
     */
    protected OutputterFactory            outputterFactory;

    /**
     * Stores an instance of template negotiator factory
     */
    protected TemplateNegotiatorFactory   negotiatorFactory;

    /**
     * Completes the work done in the passive connection connection thread constructor
     * 
     * @param parent -
     *            handle to the wanted exporter
     * @param config -
     *            connection configuration
     */
    public void completeThread(ExporterHandler parent, ConnectionConfigI config)
    {
    	this.parent = parent;
        this.config = config;
        config.addReference();
        this.setName(parent.getExporterAddress().getHostAddress() + ":active");
    }
    /**
     * Initializes connection thread on the given passive connection
     * 
     * @param connection - 
     * 				the related connection
     * @param outputterFactory - 
     * 				the outputter factory
     * @param negotiatorFactory - 
     * 				the template negotiator factory
     * @param listener - 
     * 				for examination of protocol state machine
     * @param connectionController - 
     * 				the connection controller
     * @param connectionHandler - 
     * 				handles incoming connections
     */
    public ConnectionThread(ConnectionDynamic connection,
            OutputterFactory outputterFactory,
            TemplateNegotiatorFactory negotiatorFactory,
			StateMachineListener listener, 
            ConnectionController connectionController,
			IncomingConnectionHandler connectionHandler)
    {
    	this.connectionHandler = connectionHandler;
    	
    	this.outputterFactory = outputterFactory;
    	this.negotiatorFactory = negotiatorFactory;
    	this.stateMachineListener = listener;
        this.connection = connection;
        this.isActiveConnection = false;
        this.connectionController = connectionController;       
    }

    /**
     * Instantiates the connection thread without an underlying connection
     * 
     * @param config -
     *            connection configuration
     * @param outputter -
     *            <CODE>Outputter</code> instance
     * @param templateNegotiator
     *            <code>TemplateNegotiator</code> instance
     */
    public ConnectionThread(ExporterHandler parent, ConnectionConfigI config, Outputter outputter,
            TemplateNegotiator templateNegotiator, StateMachineListener listener, 
            ConnectionController connectionController)
    {
        this.parent = parent;
        this.config = config;
        this.outputter = outputter;
        this.stateMachineListener = listener;
        this.templateNegotiator = templateNegotiator;
        this.connectionController = connectionController;
        config.addReference();
        this.setName(parent.getExporterAddress().getHostAddress() + ":active");
    }

    /**
     * Initiates an active connection to the exporter
     * 
     * @throws ConnectionIOException
     */
    public void connectToExporter() throws ConnectionIOException, InterruptedException
    {
        isActiveConnection = true;
        connection = new ConnectionDynamic(config);
    }

    /**
     * Loads state machine according to connection initiator
     */
    protected void loadStateMachine()
    {
    	IPDRStateMachine ipdrStateMachine;
    	if (isActiveConnection)
    	{
    		ipdrStateMachine =new IPDRStateMachine(this.connection, config,
    				outputter, templateNegotiator,
					stateMachineListener, connectionController);
    	}
    	else
    	{
    		ipdrStateMachine =new IPDRStateMachine(this.connection,
    				outputterFactory, negotiatorFactory,
					stateMachineListener, connectionController,
					connectionHandler, this);
    	}
    	if (this.probingListener!=null)
    		ipdrStateMachine.setProbingListener(this.probingListener);
    	stateMachine = ipdrStateMachine; 
    }
  
    /**
     * @see java.lang.Runnable#run()
     */
    public void run()
    {
    	
        boolean reloadStateMachine = false;
     
        loadStateMachine();
                
        try
        {
            int attempt = 0;
            boolean success = false;
            if (isActiveConnection)
            while ( keepConnecting && (attempt < config.getRetryCount() || config.getRetryCount() == ConnectionConfigI.INFINITE) )
            {
                try
                {
                    if (reloadStateMachine)
                    {
                        this.stateMachine.cleanup();
                        loadStateMachine();
                        reloadStateMachine = false;
                    }
                    
                    while (connectionController!= null && !connectionController.canConnect(config.getExporterAddress(), config))
                        sleep(config.getCollectorConfig().getControllerQueryInterval());
                    
	                connectToExporter();
	                if (!keepConnecting)
	                    throw new ConnectionIOException("Failed to connect to the exporter after " + attempt + " tries. Giving up on disconnection request", config, null); 
	                stateMachine.setConnection(this.connection);
	                reloadStateMachine = true;
	                stateMachine.mainLoop();
	                success = true;
                }
                catch (ConnectionIOException e)
                {
                    LogManager.warn(this,"connectToExporter","Connection to " + config.getExporterAndPortString() + " failed, attempt " + (attempt+1) + ". Sleeping for " + config.getRetryInterval() + " seconds" );
                    attempt++;
                    success = false;
                    sleep(config.getRetryInterval()*1000);
                    continue;
                }
                if (!success)
                    throw new ConnectionIOException("Failed to connect to the exporter after " + attempt + " tries. Giving up", config, null); 
            }
            if (!isActiveConnection)
            {
                ((IPDRStateMachine) stateMachine).startListeningState();
                stateMachine.mainLoop();
            }

        }
        catch (ConnectionIOException e)
        {
            LogManager.error(this, "run", e.getMessage());
        }
        catch (InterruptedException e)
        {
            LogManager.error(this, "run", e.getMessage());
        }
        catch (ConfigException e)
        {
            LogManager.error(this, "run", e.getMessage());
        }
        finally
        {
            if (stateMachine != null)
                stateMachine.cleanup();
            if (config != null)
                config.removeReference();
        }
    }

    /**
     * Forces disconnection of the connection.
     * 
     * @param reasonCode -
     *            reason code of the disconnection
     * @param reasonInfo -
     *            verbal description of disconnection reason
     * @throws ConfigException
     * @throws ProtocolMalformedMsgException
     * @throws ConnectionIOException
     */
    public void disconnect(short reasonCode, String reasonInfo)
            throws ConfigException, ProtocolMalformedMsgException,
            ConnectionIOException
    {
        keepConnecting = false;
        stateMachine.initiateDisconnection(IPDRStateMachine.NO_ERROR,
                reasonCode, reasonInfo);
        if (config != null)
        {
        	config.removeReference();
        }
        if (connection != null)
            connection.close();
    }
    
    /**
     * stop flow with specified session
     * @param sessionId
     */
    public void stopFlow(byte sessionId)
    {
        this.stateMachine.stopFlow(sessionId);
    }
    
    /**
     * Sends a "start negotiation" message. The message can only be sent when data is being sent in the specific session.
     * @param sessionId Session id to send the message in
     * @throws InvalidSessionStateException
     * @throws ConnectionException
     */
    public void sendStartNegotiation(byte sessionId) throws InvalidSessionStateException, ConnectionException
    {
        if ( this.protocolVersion == 2)
            ((IPDRStateMachine)this.stateMachine).sendStartNegotiation(sessionId);
    }
    
    /**
     * Retrieves list of existing sessions
     * @return <code>ArrayList</code> of session ids
     */
    public ArrayList getSessionList()
    {
        return this.stateMachine.getSessionList();
    }
    
    /**
     * Returns version of currently active protocol
     * @return Version of the protocol in use
     */
    public int getProtocolVersion()
    {
        return this.protocolVersion; 
    }
    
    /**
     * Returns state of a session
     * @param sessionId
     * @return session state
     * @throws InvalidSessionStateException
     */
    public int getSessionState( byte sessionId ) throws InvalidSessionStateException
    {
        return this.stateMachine.getSessionState(sessionId);
    }
    
    /**
     * Retrieves IP address of either active or configured connection
     * @return IP address of either connection or connection configuration
     */
    public InetAddress getInetAddress()
    {
        if (this.connection!= null)
            return connection.getAddress();
        return config.getExporterAddress();
    }
    
    /**
     * Checks if the connection thread has a currently active session with the given id
     * @param sessionId session id to check
     * @return <code>true</code> if the session exists
     */
    public boolean hasSession(byte sessionId)
    {
        try
        {
            this.stateMachine.getSessionState(sessionId);
            return true; 
        }
        catch (InvalidSessionStateException e)
        {
            return false;
        }
    }

    /**
     * Set class parameter probingListener
     * @param listener The probing listener to asign
     */
    public void setProbingListener(ProbingListener probingListener)
    {
        this.probingListener = probingListener;
    }
}