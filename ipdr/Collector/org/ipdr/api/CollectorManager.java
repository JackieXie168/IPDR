package org.ipdr.api;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.net.InetAddress;
import java.util.ArrayList;

import org.ipdr.ConnectionDynamic;
import org.ipdr.DiscoveryHandler;
import org.ipdr.ExporterHandler;
import org.ipdr.IncomingConnectionHandler;
import org.ipdr.ListeningSocketHandler;
import org.ipdr.api.configuration.CollectorConfigI;
import org.ipdr.api.configuration.ConfigException;
import org.ipdr.api.configuration.ConnectionConfigI;
import org.ipdr.api.log.LogManager;

/**
* The class provides access to IPDR protocol. The class allows initialization,
* execution and termination of IPDR collector. 
*/
public class CollectorManager implements IncomingConnectionHandler
{
  /**
   * Holds connection threads of all incoming/outgoing connections
   */
  protected ArrayList                 connectionThreads = new ArrayList();
  /**
   * Holds configuration for the collector
   */
  protected CollectorConfigI          configuration    = null;
  /**
   * Holds list of exporter handler objects for known exporters
   */
  protected ArrayList                 exporterHandlers = new ArrayList();
  /**
   * Holds listening socket handler thread
   */
  protected ListeningSocketHandler    listeningSocketHandler;
  /**
   * Holds discovery requests handler
   */
  protected DiscoveryHandler          discoveryHandler = null;
  /**
   * Holds instance of outputter factory
   */
  protected OutputterFactory          outputterFactory;
  /**
   * Holds instance of template negotiator factory
   */
  protected TemplateNegotiatorFactory templateNegotiatorFactory;

  /**
   * Holds instance of state machine listener
   */
  protected StateMachineListener stateMachineListener = null;
  
  /**
   * Holds instance of connection controller factory
   */
  protected ConnectionControllerFactory connectionControllerFactory = null;
  
  /**
   * Holds instance of probing listener
   */
  private ProbingListener probingListener = null;
  /**
   * @see org.ipdr.IncomingConnectionHandler#handleIncomingConnection(ConnectionDynamic)
   */
  public boolean handleIncomingConnection(ConnectionDynamic connection)
  {
      ConnectionController connectionController = null;
          	        
      if (connectionControllerFactory != null)
      	connectionController = connectionControllerFactory.createInstance();
      
      ConnectionThread thread = new ConnectionThread(
      		connection,	outputterFactory,
			templateNegotiatorFactory, stateMachineListener,
			connectionController, this);
      synchronized(connectionThreads)
		{
      	connectionThreads.add(thread);
      }
      if (probingListener != null)
      	thread.setProbingListener(probingListener);
      thread.start();
      return true;
  }
  
  /**
   * @see org.ipdr.IncomingConnectionHandler#exporterAddressKnown(InetAddress,ConnectionThread)
   */
  public ConnectionConfigI exporterAddressKnown(InetAddress exportersAddress, ConnectionThread thread)
  {
  	int j=0;
  	ConnectionThread connectionThread = null;
		for (; j < connectionThreads.size(); j++)
		{
			if (((ConnectionThread) connectionThreads.get(j)) == thread)
			{
				connectionThread = (ConnectionThread) connectionThreads.get(j);
				connectionThreads.remove(j);
				break;
			}
		}
  	for (int i=0; i < exporterHandlers.size(); i++)
  	{    		
  		if (((ExporterHandler) exporterHandlers.get(i)).getExporterAddress().equals(exportersAddress))
  		{
  			((ExporterHandler) exporterHandlers.get(i)).addConnection(thread);
  			ConnectionConfigI connectionConfig = ((ExporterHandler) exporterHandlers.get(i)).findFreePassiveConnection();
  	        if (connectionConfig == null)
  	            return null;
  			connectionThread.completeThread((ExporterHandler) exporterHandlers.get(i),connectionConfig);
  			return connectionConfig;
  		}
  	}
	return null;
  }

  /**
   * CollectorManager constructor. The constructor receives the following
   * parameters:
   * 
   * @param config -
   *            an instance of collector configuration
   * @param outputterFactory -
   *            an instance of outputter factory
   * @param templateNegotiatorFactory -
   *            an instance of template negotiator factory
   * * @param listener -
   *            an instance of state machine listener
   * * @param connectionControllerFactory -
   *            an instance of connection controller factory
   */
  public CollectorManager(CollectorConfigI config,
          OutputterFactory outputterFactory,
          TemplateNegotiatorFactory templateNegotiatorFactory,
          StateMachineListener listener,
          ConnectionControllerFactory connectionControllerFactory)
  {
      this.configuration = config;
      this.outputterFactory = outputterFactory;
      this.templateNegotiatorFactory = templateNegotiatorFactory;
      this.stateMachineListener = listener;
      this.connectionControllerFactory = connectionControllerFactory;
      initialize();
  }

  /**
   * Sets the probing listener of the collector manager and 
   * all its registered exporter handlers
   * 
   * @param probingListener The probing listener to asign
   */
public void setProbingListener(ProbingListener probingListener)
  {
      this.probingListener = probingListener;
      for (int i = 0; i < exporterHandlers.size(); i++)
      {
          ExporterHandler exporter = (ExporterHandler) exporterHandlers
                  .get(i);
          exporter.setProbingListener(probingListener);
      }
  }
  /**
   * Searches exporter handler object by exporter IP address
   * 
   * @param address -
   *            address of exporter to look up
   * @return ExporterHandler object or null if not found.
   */
  protected ExporterHandler getExporterHandler(InetAddress address)
  {
      for (int i = 0; i < exporterHandlers.size(); i++)
      {
          ExporterHandler exporter = (ExporterHandler) exporterHandlers
                  .get(i);
          if (exporter.getExporterAddress().equals(address))
              return exporter;
      }
      return null;
  }

  /**
   * Performs initialization: instantiates exporter handlers, listening socket
   * handlers, discovery handler (if so configured)
   */
  protected void initialize()
  {
      int exportersCount = configuration.getExporterListLength();

      for (int i = 0; i < exportersCount; i++)
      {
          ExporterHandler exporter = new ExporterHandler(configuration
                  .getExporterAt(i), outputterFactory,
                  templateNegotiatorFactory, stateMachineListener, connectionControllerFactory);
          exporterHandlers.add(exporter);
          exporter.init();
      }

      // check for passive connections
      this.listeningSocketHandler = new ListeningSocketHandler(this,
              configuration);

      if (configuration.getCollectorSupportDiscovery())
          this.discoveryHandler = new DiscoveryHandler(configuration);
  }

  /**
   * Executes the collector. Spawns exporter handlers, listening socket,
   * discovery handler (if required)
   */
  public void run()
  {
      for (int i = 0; i < exporterHandlers.size(); i++)
      {
          ExporterHandler exporter = (ExporterHandler) exporterHandlers
                  .get(i);
          try
          {
              exporter.run();
          }
          catch (ConfigException e)
          {
              LogManager.error(this, "run", "Failed to start exporter handler:" + e.getMessage());
          }
      }
      this.listeningSocketHandler.start();
      if (this.discoveryHandler != null)
          this.discoveryHandler.start();
  }

  /**
   * Disconnects all existing connections, shuts down the listening socket
   */
  public void disconnectAll()
  {
      listeningSocketHandler.close();
      if (this.discoveryHandler != null)
          discoveryHandler.close();
      for (int i = 0; i < exporterHandlers.size(); i++)
      {
          ExporterHandler exporter = (ExporterHandler) exporterHandlers
                  .get(i);
          exporter.disconnectAll();
      }
  }

  /**
   * Waits for all the threads to finish
   * 
   * @throws InterruptedException
   */
  public void joinAll() throws InterruptedException
  {
      for (int i = 0; i < exporterHandlers.size(); i++)
      {
          ExporterHandler exporter = (ExporterHandler) exporterHandlers
                  .get(i);
          exporter.joinAll();
      }
      listeningSocketHandler.join();
      if (this.discoveryHandler != null)
          discoveryHandler.join();
  }
  
  /**
   * Locates connection threads which handle the given session id.
   * 
   * @param sessionId session id to locate
   * @param exporterAddress exporter to locate the session at.
   * @return Suitable connection thread. null if none are found
   */
  public ConnectionThread locateConnectionBySession(byte sessionId, InetAddress exporterAddress)
  {
      for(int i = 0; i < exporterHandlers.size(); i++)
      {
          ExporterHandler exporterHandler = (ExporterHandler)exporterHandlers.get(i);
          if (exporterHandler.getExporterAddress().getHostAddress().equals(exporterAddress.getHostAddress()))
              return (exporterHandler.locateBySession(sessionId));
      }
      return null;
  }
}