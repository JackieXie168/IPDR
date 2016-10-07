package org.ipdr;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.net.InetAddress;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.Timer;

import org.ipdr.api.ConnectionController;
import org.ipdr.api.ConnectionIOException;
import org.ipdr.api.ConnectionThread;
import org.ipdr.api.Outputter;
import org.ipdr.api.OutputterException;
import org.ipdr.api.OutputterFactory;
import org.ipdr.api.ProbingListener;
import org.ipdr.api.StateMachineListener;
import org.ipdr.api.TemplateNegotiator;
import org.ipdr.api.TemplateNegotiatorFactory;
import org.ipdr.api.configuration.CollectorConfigI;
import org.ipdr.api.configuration.ConfigException;
import org.ipdr.api.configuration.ConnectionConfigI;
import org.ipdr.api.configuration.SessionConfigI;
import org.ipdr.api.configuration.TemplateConfig;
import org.ipdr.api.log.LogManager;
import org.ipdr.protocol.ConnectionException;
import org.ipdr.protocol.InvalidCapabilities;
import org.ipdr.protocol.InvalidSessionStateException;
import org.ipdr.protocol.PeerSessionErrorException;
import org.ipdr.protocol.ProtocolMalformedMsgException;
import org.ipdr.protocol.ProtocolMsgCONNECT;
import org.ipdr.protocol.ProtocolMsgCONNECT_RESPONSE;
import org.ipdr.protocol.ProtocolMsgDATA;
import org.ipdr.protocol.ProtocolMsgDATA_ACK;
import org.ipdr.protocol.ProtocolMsgDISCONNECT;
import org.ipdr.protocol.ProtocolMsgERROR;
import org.ipdr.protocol.ProtocolMsgEmpty;
import org.ipdr.protocol.ProtocolMsgFINAL_TEMPLATE_DATA_ACK;
import org.ipdr.protocol.ProtocolMsgFLOW_START;
import org.ipdr.protocol.ProtocolMsgFLOW_STOP;
import org.ipdr.protocol.ProtocolMsgGET_SESSIONS;
import org.ipdr.protocol.ProtocolMsgGET_SESSIONS_RESPONSE;
import org.ipdr.protocol.ProtocolMsgGET_TEMPLATES;
import org.ipdr.protocol.ProtocolMsgGET_TEMPLATES_RESPONSE;
import org.ipdr.protocol.ProtocolMsgKEEPALIVE;
import org.ipdr.protocol.ProtocolMsgMODIFY_TEMPLATE;
import org.ipdr.protocol.ProtocolMsgMODIFY_TEMPLATE_RESPONSE;
import org.ipdr.protocol.ProtocolMsgSESSION_START;
import org.ipdr.protocol.ProtocolMsgSESSION_STOP;
import org.ipdr.protocol.ProtocolMsgSTART_NEGOTIATION;
import org.ipdr.protocol.ProtocolMsgSTART_NEGOTIATION_REJECT;
import org.ipdr.protocol.ProtocolMsgTEMPLATE_DATA;
import org.ipdr.protocol.ProtocolMsgTemplate;
import org.ipdr.protocol.ProtocolSessionInfo;
import org.ipdr.protocol.ProtocolStreamMsg;
import org.ipdr.protocol.SessionException;
import org.ipdr.utility.TimeoutTimerTask;

/**
 * Implements IPDR state machine
 */
public class IPDRStateMachine extends ProtocolStateMachine
{

    public static final short CONNECTION_ERROR_KEEPALIVE_EXPIRED = 0;

    public static final short CUSTOM_ERROR_BASE = 255;
    
    public static final short EXPORTER_ADDRESS_ERROR_UNKNOWN = CUSTOM_ERROR_BASE + 3;

    public static final short FLOW_ERROR = 1;

    public static final short FLOW_NO_ERROR = 0;

    public static final short NO_ERROR = -1;

    public static final short OUTPUTTER_ERROR = CUSTOM_ERROR_BASE + 1;

    public static final short SESSION_ERROR_MESSAGE_DECODE_ERROR = 3;

    public static final short SESSION_ERROR_MESSAGE_INVALID_CAPABILITIES = 1;

    public static final short SESSION_ERROR_MESSAGE_INVALID_STATE = 2;

    public static final short SESSION_ERROR_TERMINATING = 4;
    
    public static final short SESSION_ERROR_UNKNOWN = CUSTOM_ERROR_BASE + 2;

    /**
     * Indicates whether the state machine may quit
     */
    protected boolean canExit = false;
    /**
     * Holds connection configuration
     */
    protected ConnectionConfigI config = null;
    
    /** 
     * Stores an instance of template negotiator factory
     */
    protected IncomingConnectionHandler connectionHandler = null;
    
    /**
     * Holds state machine listener
     */
    protected ConnectionController controller = null;

    /**
     * Holds exporter keepalive timeout
     */
    protected long exporterKeepaliveTimeout;

    /**
     * Is true whenever the exporter supports multiple sessions
     */
    protected boolean exporterMultipleSessions = false;
    
    /** 
     * Stores the connection thread containing this IPDR state machine
     */
    protected boolean isPassiveConnection = false;

    /**
     * Holds state machine listener
     */
    protected StateMachineListener listener  = null;

    /**
     * Holds template negotiator instance
     */
    protected TemplateNegotiator negotiator = null;

    /** 
     * Stores an instance of template negotiator factory
     */
    protected TemplateNegotiatorFactory   negotiatorFactory;
    /** 
     * Holds outputter instance
     */
    protected Outputter outputter = null;
    
    /** 
     * Stores an instance of an Outputter factory
     */
    protected OutputterFactory            outputterFactory;
    
    /**
     * Indicates whether there is a pending flow stop request
     */
    protected volatile boolean pendingStopFlow = false;
 
    /**
     * Holds ProbingListener instance
     */
    private ProbingListener probingListener = null;
    
    /**
	 * The real IP address of the connected exporter
	 */
	protected InetAddress realExporterAddress;
	
	/**
	 * The real port of the connected exporter
	 */
	protected int realExporterPort;
    
    /**
     * Holds the probed exporter's sessions
     */
    private ArrayList sessionProbeResult = null;
    /**
     * Holds connection state
     */
    protected IPDRConnectionState state;
    /**
     * Contains list of pending flow stop requests
     */
    protected ArrayList stopFlows = new ArrayList();
    
    /** 
     * Stores the connection thread containing this IPDR state machine
     */
    protected ConnectionThread thread = null;

    /**
     * Facilitates all timers required by the state machine
     */
    protected Timer timer = new Timer(true);

    /**
     * Holds timer task for timers
     */
    protected TimeoutTimerTask timerTask = new TimeoutTimerTask(this);
    
    /**
     * Stores all sessions which were not yet probed for templates
     */
    private Hashtable unprobedSessions = new Hashtable();  

    /**
     * Initializes the state machine with given connection, configuration and
     * outputter for an active connection
     * 
     * @param connection
     * @param config
     * @param outputter
     */
    public IPDRStateMachine(ConnectionDynamic connection,
            ConnectionConfigI config, Outputter outputter)
    {
        this.connection = connection;
        this.config = config;
        this.state = new IPDRConnectionState(this, IPDRConnectionState.DISCONNECTED,
                outputter);
        this.timer.scheduleAtFixedRate(timerTask,
                TimeoutTimerTask.TIMEOUT_INTERVAL,
                TimeoutTimerTask.TIMEOUT_INTERVAL);
    }
    
    /**
     * Initializes the state machine with given connection, configuration,
     * outputter and template negotiator for an active connection
     * 
     * @param connection
     * @param config
     * @param outputter
     * @param negotiator
     */
    public IPDRStateMachine(ConnectionDynamic connection,
            ConnectionConfigI config, Outputter outputter,
            TemplateNegotiator negotiator)
    {
        this(connection, config, outputter);
        this.negotiator = negotiator;
    }
    
    /**
     * Initializes the state machine with given connection, configuration,
     * outputter, template negotiator and state machine listener for an active connection
     * 
     * @param connection
     * @param config
     * @param outputter
     * @param negotiator
     * @param listener
     */
    public IPDRStateMachine(ConnectionDynamic connection,
            ConnectionConfigI config, Outputter outputter,
            TemplateNegotiator negotiator, 
            StateMachineListener listener,
            ConnectionController controller)
    {
        this(connection, config, outputter, negotiator);
        this.listener = listener;
        this.controller = controller;
    }
    
    
    /** 
     * Initializes the state machine with given connection and
     * outputter factory for a passive connection
     * 
     * @param connection
     * @param outputterFactory
     */ 
    public IPDRStateMachine(ConnectionDynamic connection,
    		OutputterFactory outputterFactory)
    {
        this.connection = connection;
        this.outputterFactory = outputterFactory;
        this.state = new IPDRConnectionState(this,IPDRConnectionState.DISCONNECTED);
        this.timer.scheduleAtFixedRate(timerTask,
                TimeoutTimerTask.TIMEOUT_INTERVAL,
                TimeoutTimerTask.TIMEOUT_INTERVAL);
    }

    /**
     * Initializes the state machine with given connection,
     * outputter factory and template negotiator factory for a passive connection
     * 
     * @param connection
     * @param outputterFactory
     * @param negotiatorFactory
     */
    public IPDRStateMachine(ConnectionDynamic connection,
    		OutputterFactory outputterFactory,
            TemplateNegotiatorFactory negotiatorFactory)
    {
        this(connection,outputterFactory);
        this.negotiatorFactory = negotiatorFactory;
    }

    /**
     * Initializes the state machine with given connection,
     * outputter factory, template negotiator factory, state machine listener
     * connection controller, incoming connection handler and a connection thread
     * for a passive connection
     * 
     * @param connection
     * @param outputterFactory
     * @param negotiatorFactory
     * @param listener
     * @param connectionController
     * @param connectionHandler
     * @param thread
     */
    public IPDRStateMachine(ConnectionDynamic connection,
    		OutputterFactory outputterFactory,
            TemplateNegotiatorFactory negotiatorFactory, 
            StateMachineListener listener,
            ConnectionController controller,
			IncomingConnectionHandler connectionHandler,
			ConnectionThread thread)
    {
        this(connection, outputterFactory, negotiatorFactory);
        this.listener = listener;
        this.controller = controller;
        
        this.connectionHandler = connectionHandler;
        this.thread = thread;
    }
    
    /**
     * @see org.ipdr.ProtocolStateMachine#canExit()
     */
    public boolean canExit()
    {
        synchronized (this)
        {
            return this.canExit;
        }
    }

    /**
     * @see org.ipdr.ProtocolStateMachine#cleanup()
     */
    public void cleanup()
    {
        timer.cancel();
    }

    /**
     * The method is used by timer facilities to notify the state machine of
     * collector keepalive timeout. KEEPALIVE message is sent upon this method
     * invokation
     */
    public void collectorKeepAliveTimeout()
    {
        synchronized (this)
        {
            try
            {
                LogManager.debug("IPDRProtocolStateMachine",
                        "keepAliveTimeout",
                        "Keepalive timeout - sending keepalive");
                sendMessage(new ProtocolMsgKEEPALIVE(), connection);
            }
            catch (ConnectionIOException e)
            {
                LogManager.error(this, "keepAliveTimeout",
                        "exception while sending keepalive message:"
                                + e.getMessage());
            }
        }
    }
    
    /**
     * Completes the instantiation of the state machine for an active connection
     * by Creating an outputter and a template nagotiator from their factories
     * 
     * @param config
     * @param transmitterAddress
     * @param transmitterPort
     */
    public void completeMachine(ConnectionConfigI config,
    		InetAddress transmitterAddress,
			int transmitterPort)
    {
    	this.config = config;
        this.outputter = outputterFactory.create(transmitterAddress,transmitterPort);
        this.state.setOutputter(this.outputter);
        if (negotiatorFactory != null)
        {
        	this.negotiator = negotiatorFactory.getInstance(config.getExporterConfig());    
        }
    }

    /**
     * @see org.ipdr.ProtocolStateMachine#connectTimeout()
     */
    public void connectTimeout()
    {
        synchronized (this)
        {
            if (this.state.getState() == IPDRConnectionState.CONNECTING)
            {
                LogManager.error("IPDRProtocolStateMachine", "connectTimeout",
                        "Connection timeout fired");
                canExit = true;
            }
        }
    }

    /**
     * @see org.ipdr.ProtocolStateMachine#dataAckTimeout(byte)
     */
    public void dataAckTimeout(byte sessionId)
    {
        try
        {
            IPDRSessionState session = state.getSession(sessionId);
            synchronized (this)
            {
                if (session.getDataAckCount() != 0)
                        sendDataAcknowledge(sessionId, session.getConfigId(),
                                session.getCurrentRecordSequenceNumber());
            }
        }
        catch (ConnectionIOException e)
        {
        }
        catch (InvalidSessionStateException e)
        {
        }
    }

    /**
     * Sends appropriate FLOW_START messages
     * 
     * @throws ConnectionIOException
     * @throws ConfigException
     * @throws ProtocolMalformedMsgException
     */
    protected void doFlowStart() throws ConnectionIOException, ConfigException
    {
        boolean allStarted = true;
        // the multi-session logic is here.
        // 1. we already have an available connection. we need to see how many
        // sessions are there.
        if (!exporterMultipleSessions && config.getSessionListLength() > 1)
        {
            LogManager.error(this, "doFlowStart", "Multiple sessions ");
            //pick and handle a session
            SessionConfigI sessionConfig = config.getSessionConfigAt(0);
            if (controller != null && 
                    !controller.canStartFlow(config.getExporterAddress(), config, sessionConfig))
            {
                return;
            }
            if (this.state.hasSession(sessionConfig.getSessionId()))
                return;
            this.state.addSession(sessionConfig);
            ProtocolMsgFLOW_START message = new ProtocolMsgFLOW_START(
                    sessionConfig.getSessionId());
            
            sendMessage(message, connection);
            if (listener != null)
                listener.notifyFlowStarted(this.realExporterAddress, this.realExporterPort,
                		message);
        }
        else
        {
            for (int i = 0; i < config.getSessionListLength(); i++)
            {
                // start a single session on the connection
                SessionConfigI sessionConfig = config.getSessionConfigAt(i);
                if (this.state.hasSession(sessionConfig.getSessionId()))
                    continue;
                
                if (controller != null && 
                        !controller.canStartFlow(config.getExporterAddress(), config, sessionConfig))
                {
                    allStarted = false;
                    continue;
                }

                this.state.addSession(sessionConfig);
                ProtocolMsgFLOW_START message = new ProtocolMsgFLOW_START(
                        sessionConfig.getSessionId());

                sendMessage(message, connection);
                
                if (listener != null)
                    listener.notifyFlowStarted(this.realExporterAddress, this.realExporterPort,
                            message);
            }
        }
        
        if (allStarted)
            this.state.toState(IPDRConnectionState.SESSION_INITIATION);
    }
    

    /**
     * @param sessionId
     * @param reasonCode
     * @param reasonInfo
     * @throws ConnectionException
     */
    protected void doFlowStop(byte sessionId, short reasonCode, String reasonInfo) throws ConnectionException
    {
        ProtocolMsgFLOW_STOP messageFlowStop = new ProtocolMsgFLOW_STOP(
                sessionId, FLOW_ERROR, reasonInfo);

        try
        {
            messageFlowStop.sendMsg(connection);
            if (listener!= null)
                listener.notifyFlowStopped(this.realExporterAddress, this.realExporterPort, messageFlowStop);
        }
        catch (Exception e)
        {
            throw new ConnectionException("Failed to stop session "
                    + sessionId + ", Escalating the error");
        }
        state.removeSession(sessionId);
        state.toState(IPDRConnectionState.CONNECTED);
    }


    /**
     * start exporter probing procedure
     * 
     * @throws ConnectionIOException
     */
    protected void doInitiateProbe() throws ConnectionIOException
    {
        ProtocolMsgGET_SESSIONS msgGetSessions = new ProtocolMsgGET_SESSIONS((short)0);
        sendMessage(msgGetSessions, connection);
    }

    /**
     * Ends the connection
     * 
     * @param reasonCode
     *            reason code
     * @param reasonInfo
     *            verbal description of the reason
     */
    protected void endConnection(short reasonCode, String reasonInfo)
    {
        synchronized (this)
        {
            if (this.state.getState() == IPDRConnectionState.HANDLING_CONNECTION_ERROR)
            // already handling an error
                    return;

            initiateDisconnection(reasonCode, FLOW_ERROR, reasonInfo);
        }
    }

    /**
     * Ends the given session
     * 
     * @param sessionId -
     *            session id of session to end
     * @param reasonCode -
     *            reason code
     * @param reasonInfo -
     *            verbal reason description
     * @throws ConnectionException
     */
    protected void endSession(byte sessionId, short reasonCode,
            String reasonInfo) throws ConnectionException
    {
        synchronized (this)
        {
            IPDRSessionState session;
            try
            {
                session = state.getSession(sessionId);
            }
            catch (InvalidSessionStateException e)
            {
                // no such session
                return;
            }

            if (session.getState() == IPDRSessionState.ERROR)
            // already handling disconnection
                    return;

            ProtocolMsgERROR messageError = new ProtocolMsgERROR(sessionId,
                    reasonCode, reasonInfo);

            try
            {
                messageError.sendMsg(connection);
                LogManager.error(this, "endSession",
                        "Session error encountered. code:" + reasonCode
                                + " reason:" + reasonInfo);
            }
            catch (Exception e)
            {
                throw new ConnectionException("Failed to stop session "
                        + sessionId + ", Escalating the error");
            }

            doFlowStop(sessionId, reasonCode, reasonInfo);
        }
    }

    /**
     * The method is used by timer facilities to notify the state machine of
     * exporter keepalive timeout.
     */
    public void exporterKeepAliveTimeout()
    {
        synchronized (this)
        {

            LogManager.error("IPDRProtocolStateMachine",
                    "exporterKeepAliveTimeout",
                    "Exporter keepalive timeout missed");
            endConnection(CONNECTION_ERROR_KEEPALIVE_EXPIRED,
                    "Exporter keepalive timeout expired");
        }
    }

    /**
     * Returns the exporterKeepaliveTimeout.
     * 
     * @return long
     */
    public long getExporterKeepaliveTimeout()
    {
        return exporterKeepaliveTimeout;
    }
   
    /**
     * Returns exporter address as reported by the Exporter
     * @return
     */
    public InetAddress getRealExporterAddress()
    {
        return realExporterAddress;
    }

    /**
     * @see org.ipdr.ProtocolStateMachine#getSessionList()
     */
    public ArrayList getSessionList()
    {
        return this.state.getSessionIdList();
    }


    /**
     * @see org.ipdr.ProtocolStateMachine#getSessionState(byte)
     */
    public int getSessionState(byte sessionId) throws InvalidSessionStateException
    {
        return this.state.getSessionState(sessionId);
    }
    
    /**
     * Handle a protocol message.
     * @param message
     * @throws ConnectionIOException
     * @throws ConfigException
     * @throws ConnectionException
     * @throws SessionException
     * @throws ProtocolMalformedMsgException
     * @throws OutputterException
     */
    protected void handleMessage(ProtocolStreamMsg message)
            throws ConnectionIOException, ConfigException, ConnectionException,
            SessionException, ProtocolMalformedMsgException, OutputterException
    {
        synchronized (this)
        {
            switch (message.msgId())
            {
            case ProtocolMsgCONNECT.MSG_ID:
            {
                if (this.state.getState() != IPDRConnectionState.DISCONNECTED)
                        throw new ConnectionException();
                ProtocolMsgCONNECT connectMessage = (ProtocolMsgCONNECT) message;
                this.exporterMultipleSessions = connectMessage
                        .getCapabilitiesMultiSession();
                this.exporterKeepaliveTimeout = connectMessage
                        .getKeepAliveInterval();
                LogManager.info("IPDRStateMachine", "handleCONNECT",
                        "CONNECT received and parsed");
                
                InetAddress exporterAddress = connectMessage.getInitiatorAddress();
                int exporterPort = connectMessage.getInitiatorPort();
                
                this.realExporterAddress = exporterAddress;
                this.realExporterPort = exporterPort;
                ConnectionConfigI connectionConfig = connectionHandler.exporterAddressKnown(exporterAddress,thread);  
                if (connectionConfig != null)
                {
                	completeMachine(connectionConfig,exporterAddress,exporterPort);
                }
                else
                {
                	endConnection(EXPORTER_ADDRESS_ERROR_UNKNOWN,"Collector is not configured for the exporter specified in address");
                	break;
                }
                
                // now send response
                CollectorConfigI collectorConfig = config.getCollectorConfig();

                ProtocolMsgCONNECT_RESPONSE msg = new ProtocolMsgCONNECT_RESPONSE(
                        collectorConfig.getCapabilitiesStructures(),
                        collectorConfig.getCapabilitiesMultiSession(),
                        collectorConfig.getCapabilitesTemplateNegotiation(),
                        config.getKeepAliveInterval(), collectorConfig
                                .getVendorId());

                sendMessage(msg, connection);
                
                if (listener != null)
                    listener.notifyConnected(this.realExporterAddress, this.realExporterPort, msg);
                
                this.state.toState(IPDRConnectionState.CONNECTED);
                timerTask.setCollectorKeepaliveTimeout(exporterKeepaliveTimeout);
                
                break;
            }
            case ProtocolMsgCONNECT_RESPONSE.MSG_ID:
            {
                timerTask.resetConnectTimeout();
                this.exporterMultipleSessions = ((ProtocolMsgCONNECT_RESPONSE) message)
                        .getCapabilitiesMultiSession();
                this.exporterKeepaliveTimeout  = ((ProtocolMsgCONNECT_RESPONSE) message).getKeepAliveInterval();
                if (this.state.getState() != IPDRConnectionState.CONNECTING)
                        throw new ConnectionException();
                LogManager.info("IPDRStateMachine", "handleMessage",
                        "CONNECT received and parsed");

               	this.realExporterAddress = this.connection.getAddress();
               	this.realExporterPort = this.connection.getPort();
                
                if (listener != null)
                    listener.notifyConnected(this.realExporterAddress, this.realExporterPort, (ProtocolMsgCONNECT_RESPONSE) message);

                this.state.toState(IPDRConnectionState.CONNECTED);

                timerTask.setCollectorKeepaliveTimeout(exporterKeepaliveTimeout);

                break;
            }
            case ProtocolMsgMODIFY_TEMPLATE_RESPONSE.MSG_ID:
            {
                timerTask.resetExporterKeepaliveTimeout();
                validateSessionState(message);
                validateTemplates(message);
                state
                        .handleModifyTemplateResponse((ProtocolMsgMODIFY_TEMPLATE_RESPONSE) message);

                primitiveAcknowledgeTemplate(message);
                state.setSessionConfigId(message.getSessionId(),
                        ((ProtocolMsgMODIFY_TEMPLATE_RESPONSE) message)
                                .getConfigId());
                
                if (listener != null)
                    listener.notifyTemplatesNegotiated(this.realExporterAddress, this.realExporterPort, ((ProtocolMsgMODIFY_TEMPLATE_RESPONSE) message).getSessionId(), ((ProtocolMsgMODIFY_TEMPLATE_RESPONSE) message).getTemplates());

                break;
            }
            case ProtocolMsgDATA.MSG_ID:
            {
                timerTask.resetExporterKeepaliveTimeout();
                validateSessionState(message);
                if (state.getDataAckCount(message.getSessionId())== 0)
                    timerTask.setDataAckTimeout(message.getSessionId(), state.getSessionAckTimeInterval(message.getSessionId()));

                state.outputDataMessage((ProtocolMsgDATA) message);
                if (state.getSessionAcknowledge(message.getSessionId()))
                        sendDataAcknowledge((ProtocolMsgDATA) message);// send ack
                break;
            }

            case ProtocolMsgTEMPLATE_DATA.MSG_ID:
            {
                short configID = ((ProtocolMsgTEMPLATE_DATA) message)
                        .getConfigId();

                timerTask.resetExporterKeepaliveTimeout();
                validateSessionState(message);
                validateTemplates(message);
                if (config.getCollectorConfig()
                        .getCapabilitesTemplateNegotiation()
                        && ((ProtocolMsgTEMPLATE_DATA) message).isNegotiable()
                        && this.negotiator != null)
                {
                    // initiate template negotiation

                    ArrayList newTemplates = negotiator
                            .negotiateTemplate(
                                    ((ProtocolMsgTEMPLATE_DATA) message)
                                            .getSessionId(),
                                    ((ProtocolMsgTEMPLATE_DATA) message)
                                            .getTemplates());

                    if (newTemplates != null)
                    {
                        state.setSessionConfigId(message.getSessionId(),
                                configID);
                        ProtocolMsgMODIFY_TEMPLATE outgoingMessage = new ProtocolMsgMODIFY_TEMPLATE(
                                ((ProtocolMsgTEMPLATE_DATA) message)
                                        .getSessionId(),
                                ((ProtocolMsgTEMPLATE_DATA) message)
                                        .getConfigId(), newTemplates);
                        outgoingMessage.sendMsg(connection);

                        state.setSessionState(message.getSessionId(),
                                IPDRSessionState.NEGOTIATE_START);
                        // now we wait for the MODIFY_TEMPLATE_RESPONSE to
                        // arrive
                    }
                    else
                    {
                        state.setSessionConfigId(message.getSessionId(),
                                configID);
                        state
                                .handleTemplateData((ProtocolMsgTEMPLATE_DATA) message);
                        primitiveAcknowledgeTemplate(message);
                    }
                }
                else
                {
                    // no template negotiation
                    state.setSessionConfigId(message.getSessionId(), configID);
                    state
                            .handleTemplateData((ProtocolMsgTEMPLATE_DATA) message);
                    primitiveAcknowledgeTemplate(message);
                }
                break;
            }

            case ProtocolMsgSESSION_START.MSG_ID:
            {
                validateSessionState(message);
                state.handleSessionStart(message.getSessionId(),
                        (ProtocolMsgSESSION_START) message);
                state.setSessionState(message.getSessionId(),
                        IPDRSessionState.DATA);
                
                
                if (listener != null)
                    listener.notifySessionStarted(this.realExporterAddress, this.realExporterPort, (ProtocolMsgSESSION_START) message);
                break;
            }
            case ProtocolMsgSESSION_STOP.MSG_ID:
            {
                validateSessionState(message);
                state.setSessionState(message.getSessionId(),
                        IPDRSessionState.FLOW_STARTED);
                
                this.state.toState(IPDRConnectionState.CONNECTED);
                if (listener != null)
                    listener.notifySessionStopped(this.realExporterAddress, this.realExporterPort, (ProtocolMsgSESSION_STOP) message);
                break;
            }
            case ProtocolMsgSTART_NEGOTIATION_REJECT.MSG_ID:
            {
                validateSessionState(message);
                state.setSessionState(message.getSessionId(),
                        IPDRSessionState.DATA);
                break;
            }
            case ProtocolMsgERROR.MSG_ID:
            {
                timerTask.resetExporterKeepaliveTimeout();
                ProtocolMsgERROR errorMessage = (ProtocolMsgERROR) message;
                //this.state.toState(IPDRConnectionState.HANDLING_CONNECTION_ERROR);
                LogManager
                        .error(this, "handleMessage", errorMessage.toString());
                if (errorMessage.getSessionSpecific() && errorMessage.getErrorCode() == (short)0x04)
                    throw new PeerSessionErrorException(errorMessage.getSessionId(), errorMessage.getErrorCode(), errorMessage.getDescription());
                break;
            }
            case ProtocolMsgGET_SESSIONS_RESPONSE.MSG_ID:
            {
                timerTask.resetExporterKeepaliveTimeout();
                ProtocolMsgGET_SESSIONS_RESPONSE sessMsg = (ProtocolMsgGET_SESSIONS_RESPONSE)message;
                sessionProbeResult = sessMsg.getSessions();
                
                for (int i = 0; i < sessionProbeResult.size(); i ++)
                {
                    ProtocolSessionInfo info = (ProtocolSessionInfo) sessionProbeResult.get(i);
                    unprobedSessions.put(new Byte(info.getSessionId()), info);
                    ProtocolMsgGET_TEMPLATES getTmpl = new ProtocolMsgGET_TEMPLATES(info.getSessionId(),(short)0);
                    sendMessage(getTmpl, connection);
                }
                
                break;
            }
            case ProtocolMsgGET_TEMPLATES_RESPONSE.MSG_ID:
            {
                timerTask.resetExporterKeepaliveTimeout();
                ProtocolMsgGET_TEMPLATES_RESPONSE tplMsg = (ProtocolMsgGET_TEMPLATES_RESPONSE)message;
                
                ArrayList templates = tplMsg.getTemplates();
                Byte sessionId = new Byte(tplMsg.getSessionId());
                ProtocolSessionInfo info = (ProtocolSessionInfo)unprobedSessions.get(sessionId);
                if (info == null)
                    throw new ConnectionException("Unexpected or duplicate GET_TEMPLATES response");
                info.setTemplates(templates);
                unprobedSessions.remove(sessionId);
                if (unprobedSessions.size() == 0)
                    probingListener.probedSessions(config.getExporterConfig(), sessionProbeResult);
                break;
            }
            case ProtocolMsgEmpty.MSG_ID:
            {
            }
            }
        }
    }

    /**
     * @see org.ipdr.ProtocolStateMachine#initiateDisconnection(short, short,
     *      java.lang.String)
     */
    public void initiateDisconnection(short sessionReasonCode,
            short flowReasonCode, String reasonInfo)
            
    {
        synchronized (this)
        {
            this.canExit = true;

            ProtocolMsgFLOW_STOP flowStopMsg = new ProtocolMsgFLOW_STOP(
                    (byte) 0, flowReasonCode, reasonInfo);
            

            // send FLOW_STOP to all active sessions
            ArrayList sessionIds = this.state.getSessionIdList();
            for (int i = 0; i < sessionIds.size(); i++)
            {
                flowStopMsg
                        .setSessionId(((Byte) sessionIds.get(i)).byteValue());

                try
                {
                    sendMessage(flowStopMsg, connection);
                }
                catch (ConnectionIOException e)
                {
                }
                finally
                {
                    if (listener!=null)
                        listener.notifyFlowStopped(this.realExporterAddress, this.realExporterPort, flowStopMsg);
                }
            }

            if (sessionReasonCode != NO_ERROR)
            {
                ProtocolMsgERROR messageError = new ProtocolMsgERROR((byte) 0,
                        sessionReasonCode, reasonInfo);
                try
                {
                    sendMessage(messageError, connection);
                }
                catch (ConnectionIOException e)
                {
                }
            }

            ProtocolMsgDISCONNECT disconnectMsg = new ProtocolMsgDISCONNECT();
            try
            {
                sendMessage(disconnectMsg, connection);
            }
            catch (ConnectionIOException e)
            {
            }
            finally
            {
                if (listener!=null)
                    listener.notifyDisconnected(this.realExporterAddress, this.realExporterPort);
            }
        }

    }

    /**
     * @see org.ipdr.ProtocolStateMachine#mainLoop()
     */
    public void mainLoop() throws ConfigException, ConnectionIOException
    {
        try
        {
            byte[] buffer = new byte[0x8000];
            if (this.isPassiveConnection != true) startProtocol();
            while (!canExit())
            {
                if (this.state.getState() == IPDRConnectionState.CONNECTED )
                {
                    if (this.probingListener != null && this.sessionProbeResult == null )
                    {
                        doInitiateProbe();
                    }
                    doFlowStart();
                }
                
                if (pendingStopFlow)
                {
                    synchronized(stopFlows)
                    {
	                    Iterator it = stopFlows.iterator();
	                    while (it.hasNext())
	                    {
	                        Byte sessionId = (Byte)it.next();
	                        this.doFlowStop( sessionId.byteValue(), SESSION_ERROR_TERMINATING, "Flow stop requested by user");
	                    }
	                    stopFlows.clear();
	                    pendingStopFlow = false;
                    }
                }
                
                try
                {
                    ProtocolStreamMsg message = ProtocolStreamMsg.readMsg(
                            this.connection, null, null, 100);
                    handleMessage(message);
                }
                catch (PeerSessionErrorException e)
                {
                    	state.removeSession(e.getSessionId());
                    	state.toState(IPDRConnectionState.CONNECTED);
                        if (listener!= null)
                        {
                            ProtocolMsgFLOW_STOP bogusFlowStop = new ProtocolMsgFLOW_STOP(e.getSessionId(), FLOW_ERROR, e.getMessage());
                            listener.notifyFlowStopped(this.realExporterAddress, this.realExporterPort, bogusFlowStop);
                        }
                }
                catch (InvalidSessionStateException e)
                {
                    endSession(e.getSessionId(),
                            SESSION_ERROR_MESSAGE_INVALID_STATE,
                            "Message received is invalid for session state");
                }
                catch (InvalidCapabilities e)
                {
                    endSession(e.getSessionId(),
                            SESSION_ERROR_MESSAGE_INVALID_CAPABILITIES,
                            "Message is unsuitable for peer capabilities");
                }
                catch (SessionException e)
                {
                    endSession(e.getSessionId(),
                            SESSION_ERROR_UNKNOWN,
                            e.getMessage());
                }
            }
        }
        catch (ProtocolMalformedMsgException e)
        {
            // invalid message - interrupt the connection
            endConnection(SESSION_ERROR_MESSAGE_DECODE_ERROR,
                    "Corrupted message received");
        }
        catch (ConnectionIOException e)
        {
            if (listener!=null)
                listener.notifyDisconnected(this.realExporterAddress, this.realExporterPort);
            throw e;
        }
        catch (ConnectionException e)
        {
            if (listener!=null)
                listener.notifyDisconnected(this.realExporterAddress, this.realExporterPort);
            this.canExit = true;
//            endConnection((short) 0xFF, e.getMessage());
        }
        catch (OutputterException e)
        {
            endConnection(OUTPUTTER_ERROR, "Outputter exception encountered:"
                    + e.getMessage());
        }
    }

    /**
     * The method sends out final template data acknowledge
     * 
     * @param message
     *            incoming template message
     * @throws InvalidSessionStateException
     * @throws ConnectionIOException
     * @throws ProtocolMalformedMsgException
     * @throws ConfigException
     */
    public void primitiveAcknowledgeTemplate(ProtocolStreamMsg message)
            throws InvalidSessionStateException, ConnectionIOException
    {
        state.setSessionState(message.getSessionId(),
                IPDRSessionState.NEGOTIATE_FINISH);

        ProtocolMsgFINAL_TEMPLATE_DATA_ACK outgoingMessage = new ProtocolMsgFINAL_TEMPLATE_DATA_ACK(
                message.getSessionId());
        sendMessage(outgoingMessage, connection);
    }

    /**
     * Acknowledges data message of a given session with given config id and
     * serial number
     * 
     * @param sessionId -
     *            session id
     * @param configId -
     *            configuration id
     * @param dataSeqNo -
     *            sequence number of the message to acknowledge
     * @throws ConnectionIOException
     * @throws ProtocolMalformedMsgException
     * @throws ConfigException
     * @throws InvalidSessionStateException
     */
    protected void sendDataAcknowledge(byte sessionId, short configId,
            long dataSeqNo) throws ConnectionIOException,
            InvalidSessionStateException
    {
        ProtocolMsgDATA_ACK outgoingMessage = new ProtocolMsgDATA_ACK(
                sessionId, configId, dataSeqNo);
        sendMessage(outgoingMessage, connection);
        state.resetSessionAcknowledge(sessionId);
        timerTask.setDataAckTimeout(sessionId, state
                .getSessionAckTimeInterval(sessionId));
    }

    /**
     * Acknowledges the provided data message
     * 
     * @param message -
     *            message to acknowledge
     * @throws ConnectionIOException
     * @throws ProtocolMalformedMsgException
     * @throws ConfigException
     * @throws InvalidSessionStateException
     */
    protected void sendDataAcknowledge(ProtocolMsgDATA message)
            throws ConnectionIOException, InvalidSessionStateException
    {
        sendDataAcknowledge(message.getSessionId(), message.getConfigId(),
                message.getDataSeqNo());
    }

    /**
     * Sends the message over the connection, resetting the outgoing keepalive
     * timeout
     * 
     * @param message
     *            message to send
     * @param connection
     *            connection to use
     * @throws ConnectionIOException
     * @throws ProtocolMalformedMsgException
     * @throws ConfigException
     */
    protected void sendMessage(ProtocolStreamMsg message,
            ConnectionDynamic connection) throws ConnectionIOException
    {
        timerTask.resetCollectorKeepaliveTimeout();
        if (connection != null) message.sendMsg(connection);
        timerTask.setCollectorKeepaliveTimeout(exporterKeepaliveTimeout);
    }

    /**
     * This method can be used by collector implementations to start negotiation
     * in the middle of an active session
     * 
     * @param sessionId -
     *            session id
     * @throws InvalidSessionStateException
     * @throws ConnectionException
     */
    public void sendStartNegotiation(byte sessionId)
            throws InvalidSessionStateException, ConnectionException
    {
        synchronized (this)
        {
            IPDRSessionState sessionState = this.state.getSession(sessionId);
            if (sessionState.getState() != IPDRSessionState.DATA)
                    throw new InvalidSessionStateException(sessionId,
                            "Session state is invalid for re-negotiation request");
            ProtocolMsgSTART_NEGOTIATION request = new ProtocolMsgSTART_NEGOTIATION(
                    sessionId);
            try
            {
                sendMessage(request, this.connection);
//                sessionState.setState(IPDRSessionState.RE_NEGOTIATING);
            }
            catch (ConnectionIOException e)
            {
                LogManager.error(this, "sendStartNegotiation", e.getMessage());
                throw new ConnectionException(e);
            }
        }
    }

    /**
     * Sets the exporterKeepaliveTimeout.
     * 
     * @param exporterKeepaliveTimeout
     *            The exporterKeepaliveTimeout to set
     */
    public void setExporterKeepaliveTimeout(long exporterKeepaliveTimeout)
    {
        this.exporterKeepaliveTimeout = exporterKeepaliveTimeout;
    }
    
    /**
     * Sets the probingListener parameter
     * @param probingListener
     */
    public void setProbingListener(ProbingListener probingListener)
    {
        this.probingListener = probingListener;
    }

    /**
     * The method starts the protocol in listening state
     * 
     * @throws InvalidSessionStateException
     * @throws ConnectionIOException
     */
    public void startListeningState()
    {
    	this.isPassiveConnection = true;
        this.state.toState(IPDRConnectionState.DISCONNECTED);
    }

    /**
     * @see org.ipdr.ProtocolStateMachine#startProtocol()
     */
    public void startProtocol() throws ConnectionException
    {
        synchronized (this)
        {
            if (this.state.getState() != IPDRConnectionState.DISCONNECTED)
                    throw new ConnectionException();
 			
            CollectorConfigI collectorConfig = config.getCollectorConfig();

            ProtocolMsgCONNECT message = new ProtocolMsgCONNECT(collectorConfig
                    .getCollectorAddress(), collectorConfig.getLocalPort(),
                    collectorConfig.getCapabilitiesStructures(),
                    collectorConfig.getCapabilitiesMultiSession(),
                    collectorConfig.getCapabilitesTemplateNegotiation(), config
                            .getKeepAliveInterval(), collectorConfig
                            .getVendorId());

            try
            {
                sendMessage(message, connection);
                this.state.toState(IPDRConnectionState.CONNECTING);
            }
            catch (ConnectionIOException e)
            {
                throw new ConnectionException(e);
            }
            timerTask.setConnectTimeout(config.getConnectTimeout());
        }
    }

    /**
     * The method starts a new session
     * 
     * @param sessionId
     *            session id to start
     * @throws InvalidSessionStateException
     */
    protected void startSession(byte sessionId)
            throws InvalidSessionStateException
    {
        state.setSessionState(sessionId, IPDRSessionState.DATA);
    }

    /**
     * @see org.ipdr.ProtocolStateMachine#stopFlow(byte)
     */
    public void stopFlow(byte sessionId)
    {
        synchronized(stopFlows)
        {
            stopFlows.add(new Byte(sessionId));
            this.pendingStopFlow = true;
        }
    }

    /**
     * @see java.lang.Object#toString()
     */
    public String toString()
    {
        return "IPDR state machine state:" + state.toString();
    }

    /**
     * The method encapsulates the logic for session state validation for
     * incoming messages
     * 
     * @param message
     *            message to validate
     * @throws InvalidSessionStateException
     */
    protected void validateSessionState(ProtocolStreamMsg message)
            throws InvalidSessionStateException
    {
        switch (message.msgId())
        {
        case ProtocolMsgTEMPLATE_DATA.MSG_ID:
            if (
                    state.getSessionState(message.getSessionId()) != IPDRSessionState.FLOW_STARTED
                    &&
                    state.getSessionState(message.getSessionId()) != IPDRSessionState.NEGOTIATE_FINISH
                    )
                    throw new InvalidSessionStateException(message
                            .getSessionId(),
                            "Session state is invalid for template negotiation");
            break;
        case ProtocolMsgMODIFY_TEMPLATE_RESPONSE.MSG_ID:
            if (state.getSessionState(message.getSessionId()) != IPDRSessionState.NEGOTIATE_START)
                    throw new InvalidSessionStateException(message
                            .getSessionId(),
                            "Session state is invalid for template negotiation");
            break;
        case ProtocolMsgDATA.MSG_ID:
            if (state.getSessionState(message.getSessionId()) != IPDRSessionState.DATA )
                    throw new InvalidSessionStateException(message
                            .getSessionId(),
                            "Session state is invalid for data collection");
            break;
        case ProtocolMsgSESSION_START.MSG_ID:
            if (!((state.getSessionState(message.getSessionId()) == IPDRSessionState.NEGOTIATE_FINISH) || (state
                    .getSessionState(message.getSessionId()) == IPDRSessionState.FLOW_STARTED && state
                    .isTemplatesKnown())))
                    throw new InvalidSessionStateException(message
                            .getSessionId(),
                            "Session state is invalid for session to start");
            break;
        case ProtocolMsgSESSION_STOP.MSG_ID:
            if (state.getSessionState(message.getSessionId()) != IPDRSessionState.DATA  
                    )
                    throw new InvalidSessionStateException(message
                            .getSessionId(),
                            "Session state is invalid for session stop message");
            break;
        case ProtocolMsgSTART_NEGOTIATION_REJECT.MSG_ID:
            break;
        case ProtocolMsgERROR.MSG_ID:
        case ProtocolMsgEmpty.MSG_ID:
        }
    }

    /**
     * The method validates templates by checking whether they are defined in
     * the configuration file. Unknown templates raise an exception
     * 
     * @param message -
     *            protocol template message to be validated
     * @throws ConfigException
     */
    protected void validateTemplates(ProtocolStreamMsg message)
            throws ConfigException
    {
        // obtain template block
        if (!config.getCollectorConfig().isStrictTemplates()) return;
        ProtocolMsgTemplate msg = (ProtocolMsgTemplate) message;
        ArrayList templates = msg.getTemplates();
        for (int i = 0; i < templates.size(); i++)
        {
            TemplateConfig template = (TemplateConfig) templates.get(i);
            if (!this.config.getCollectorConfig().isTemplateDefined(template))
                    throw new ConfigException(
                            "Template received was not configured properly");
        }
    }
    /**
     * @return Returns the realExporterPort.
     */
    public int getRealExporterPort()
    {
        return realExporterPort;
    }
}