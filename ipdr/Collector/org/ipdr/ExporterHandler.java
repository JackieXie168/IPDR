package org.ipdr;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
//ANY KIND either expresses or implied.

import java.net.InetAddress;
import java.util.ArrayList;

import org.ipdr.api.ConnectionController;
import org.ipdr.api.ConnectionControllerFactory;
import org.ipdr.api.ConnectionIOException;
import org.ipdr.api.ConnectionThread;
import org.ipdr.api.Outputter;
import org.ipdr.api.OutputterFactory;
import org.ipdr.api.ProbingListener;
import org.ipdr.api.StateMachineListener;
import org.ipdr.api.TemplateNegotiator;
import org.ipdr.api.TemplateNegotiatorFactory;
import org.ipdr.api.configuration.CollectorConfigI;
import org.ipdr.api.configuration.ConfigException;
import org.ipdr.api.configuration.ConnectionConfigI;
import org.ipdr.api.configuration.ExporterConfigI;
import org.ipdr.api.log.LogManager;
import org.ipdr.protocol.ProtocolMalformedMsgException;
import org.ipdr.protocol.ProtocolMsgFLOW_STOP;

/**
 * Handles single exporter object
 */
public class ExporterHandler
{

    /**
     * Holds exporter's connection threads
     */
    protected ArrayList                 connectionThreads = new ArrayList();
    /**
     * Holds exporter's configuration
     */
    protected ExporterConfigI           exporterConfig;
    /**
     * Holds factory for outputters to use in connection threads
     */
    protected OutputterFactory          outputterFactory;
    /**
     * Holds factory for template negotiators to use in connection threads
     */
    protected TemplateNegotiatorFactory templateNegotiatorFactory;
    /**
     * Holds factory for connection controller to use in connection threads
     */
    protected ConnectionControllerFactory connectionControllerFactory = null;
    /**
     * Holds instance of StateMachineListener
     */
    protected StateMachineListener stateMachineListener;
    /**
     * Flags the set of exporterProtocolVersion 
     */
    protected boolean hasVersionDiscovered = false;
    /**
     * Flags the disconnection of all related connection threads
     */
    protected volatile boolean disconnecting = false;    
    /**
     * Holds the exporter protocol version 
     */
    protected int exporterProtocolVersion;   
    /**
     * Holds instance of ProbingListener
     */
    private ProbingListener probingListener= null;
    
    /**
     * Initializes <code>ExporterHandler</code> object.
     * 
     * @param exporterConfig
     * @param outputterFactory
     * @param templateNegotiatorFactory
     * @param listener
     * @param connectionControllerFactory
     */
    public ExporterHandler(ExporterConfigI exporterConfig,
            OutputterFactory outputterFactory,
            TemplateNegotiatorFactory templateNegotiatorFactory,
            StateMachineListener listener,
            ConnectionControllerFactory connectionControllerFactory)
    {
        this.exporterConfig = exporterConfig;
        this.outputterFactory = outputterFactory;
        this.templateNegotiatorFactory = templateNegotiatorFactory;
        this.connectionControllerFactory = connectionControllerFactory;
        this.stateMachineListener = listener;
    }

    /**
     * Gets exporter address
     *
     * @return InetAddress of the exporter
     */
    public InetAddress getExporterAddress()
    {
        return exporterConfig.getExporterAddress();
    }

    /**
     * Initializes the exporter handler by creating a new connection thread
     */
    public void init()
    {
        for (int i = 0; i < exporterConfig.getConnectionConfigListLength(); i++)
        {
            ConnectionConfigI config = exporterConfig.getConnectionConfigAt(i);
            if (!config.isPassiveConnection())
            {
                Outputter outputter = null;
                TemplateNegotiator templateNegotiator = null;
                if (outputterFactory != null)
                    outputter = outputterFactory.create(config.getExporterAddress(),config.getExporterPort());

                if (templateNegotiatorFactory != null)
                    templateNegotiator = templateNegotiatorFactory
                            .getInstance(exporterConfig);
                ConnectionController connectionController = null;
                if (connectionControllerFactory != null)
                    connectionController = connectionControllerFactory.createInstance();
                synchronized(connectionThreads)
                {
                	connectionThreads.add(new ConnectionThread(this, config, outputter,
                        templateNegotiator, stateMachineListener, connectionController));
                }
            }
        }
    }

    /**
     * Joins all threads spawn by the exporter handler
     *
     * @throws InterruptedException
     */
    public void joinAll() throws InterruptedException
    {
        synchronized(connectionThreads)
        {
	        for (int i = 0; i < connectionThreads.size(); i++)
	        {
	            ConnectionThread thread = (ConnectionThread) connectionThreads
	                    .get(i);
	            thread.join();
	        }
        }
    }

    /**
     * Sets exporter's probingListener in each of his connection threads
     * 
     * @param probingListener
     */
    public void setProbingListener(ProbingListener probingListener)
    {
        this.probingListener = probingListener;
        for (int i = 0; i < connectionThreads.size(); i++)
        {
            ConnectionThread thread = (ConnectionThread) connectionThreads
                    .get(i);
            thread.setProbingListener(this.probingListener);
        }
    }

    /**
     * Disconnects all active connection threads
     */
    public void disconnectAll()
    {
    	
        disconnecting = true;
        synchronized(connectionThreads)
        {
	        for (int i = 0; i < connectionThreads.size(); i++)
	        {
	            ConnectionThread thread = (ConnectionThread) connectionThreads
	                    .get(i);
	            try
	            {
	                thread.disconnect(ProtocolMsgFLOW_STOP.FLOW_STOP_REASON_NORMAL,
	                        "Exporter handler shutdown");
	            }
	            catch (ConnectionIOException e)
	            {
	                LogManager.error(this, "disconnectAll", e.getMessage());
	            }
	            catch (ConfigException e)
	            {
	                LogManager.error(this, "disconnectAll", e.getMessage());
	            }
	            catch (ProtocolMalformedMsgException e)
	            {
	                LogManager.error(this, "disconnectAll", e.getMessage());
	            }
	        }
        }
    }

    /**
     * Probes the exporter
     * 
     * @throws ConfigException
     */
    public void probeExporter()
    throws ConfigException
    {
        CollectorConfigI config = this.exporterConfig.getCollectorConfig();
        if (this.exporterConfig.getUDPDiscoveryPort() == 0)
            return;
        int newVersion = DiscoveryHandler.queryProtocolVersionOnly( config.getCollectorAddress(),  0,
                exporterConfig.getExporterAddress(), exporterConfig.getUDPDiscoveryPort(), config.getUDPDiscoveryTimeout(), config.getCollectorProtocolVersion()) ;
        if (newVersion == DiscoveryHandler.PROTOCOL_VERSION_UNSUPPORTED)
            throw new ConfigException("No suitable version is supported by the Exporter");
        setExporterProtocolVersion(newVersion);
    }

    /**
     * Locates free passive connection in the requested exporter configuration
     *
     * @return <code>ConnectionConfig</code> of the located connection or null
     *         if none found
     */
    public ConnectionConfigI findFreePassiveConnection()
    {
        // find appropriate used and passive configuration
        ArrayList passiveConnections = exporterConfig
                .getPassiveConnectionList();
        for (int i = 0; i < passiveConnections.size(); i++)
        {
            ConnectionConfigI connectionConfig = (ConnectionConfigI) passiveConnections
                    .get(i);
            if (connectionConfig.getReferenceCount() < connectionConfig
                    .getConnectionCount())
                return connectionConfig;
        }
        return null;
    }

    /**
     * Registers the incoming connection
     *
     * @param thread -
     *            the incoming connection thread
     */
    public void addConnection(ConnectionThread thread)
    {
    	synchronized(connectionThreads)
        {
            connectionThreads.add(thread);
        }
    }
    /**
     * Runs the handler, spawning all connection threads.
     */
    public void run() throws ConfigException
    {
        probeExporter();
        for (int i = 0; i < connectionThreads.size(); i++)
        {
            ConnectionThread thread = (ConnectionThread) connectionThreads
                    .get(i);
            thread.start();
        }
    }
    /**
     * @return Returns the exporterProtocolVersion.
     */
    public int getExporterProtocolVersion()
    {
        synchronized(this)
        {
            return this.exporterProtocolVersion;
        }
    }
    /**
     * @param exporterProtocolVersion The exporterProtocolVersion to set.
     */
    public void setExporterProtocolVersion(int exporterProtocolVersion)
    {
        synchronized(this)
        {
            this.exporterProtocolVersion = exporterProtocolVersion;
            this.hasVersionDiscovered = true;
        }
    }
    /**
     * @return Returns the hasVersionDiscovered.
     */
    public boolean isVersionDiscovered()
    {
        return this.hasVersionDiscovered;
    }

    /**
     * Looks up connection threads by IP address
     * @param address of the <code>ConnectionThread</code> instance to locate
     * @return an <code>ArrayList</code> of instances of <code>ConnectionThread</code> which share the address provided. Empty <code>ArrayList</code> is returned if none are found
     */
    public ArrayList locateByAddress(InetAddress address)
    {
        ArrayList result = new ArrayList();
        for(int i = 0; i < this.connectionThreads.size(); i ++)
        {
            ConnectionThread thread=(ConnectionThread)connectionThreads.get(i);
            if (thread.getInetAddress().equals(address))
                result.add(thread);
        }
        return result;
    }

    /**
     * Looks up connection thread by session Id
     * @param sessionId of session to look up
     * @return an instance of <code>ConnectionThread</code>, or <code>null</code> if none are found.
     */
    public ConnectionThread locateBySession(byte sessionId)
    {
        for(int i = 0; i < this.connectionThreads.size(); i ++)
        {
            ConnectionThread thread=(ConnectionThread)connectionThreads.get(i);
            if (thread.hasSession(sessionId))
                return thread;
        }
        return null;
    }

}
