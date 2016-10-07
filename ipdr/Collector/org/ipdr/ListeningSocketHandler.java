package org.ipdr;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.io.IOException;
import java.net.ServerSocket;

import org.ipdr.api.ConnectionIOException;
import org.ipdr.api.configuration.CollectorConfigI;
import org.ipdr.api.log.LogManager;

/**
 * Implements listening socket for incoming connections from exporters
 */
public class ListeningSocketHandler extends Thread
{
    /**
     * Incoming connection handler for the connections
     */
    protected IncomingConnectionHandler handler;
    /**
     * Collector configuration instance
     */
    protected CollectorConfigI          config;
    /**
     * Server socket
     */
    protected ServerSocket              socket;
    /**
     * Exit flag
     */
    protected boolean                   canExit = false;

    /**
     * Initiates the listening socket handler
     * 
     * @param handler
     *            handler of incoming connections
     * @param config
     *            configuration
     */
    public ListeningSocketHandler(IncomingConnectionHandler handler,
            CollectorConfigI config)
    {
        this.handler = handler;
        this.config = config;
    }

    /**
     * @see java.lang.Runnable#run()
     */
    public void run()
    {
        setName("ListeningSocketHandler");

        try
        {
        	socket = null;
        	while (socket == null && !canExit())
        	{
        		try
				{
        			socket = ConnectionDynamic.makeServerSocket(config);
        			break;
				}
        		catch (IOException iex)
				{
        			LogManager.warn("ConnectionDynamic", "makeServerSocket", iex.getMessage());
            		try
					{
            			sleep(5);
					}
            		catch (InterruptedException e)
					{
            		}
				}
        	}
            while (!canExit())
            {
                ConnectionDynamic connection;

                connection = ConnectionDynamic.makeListeningInstance(socket,
                        config);

                if (connection == null)
                    continue;
                if (!handler.handleIncomingConnection(connection))
                    connection.close();
            }
        }
        catch (ConnectionIOException cie)
        {
            LogManager.error(this, "run", cie.getMessage());
        }
        finally
        {
            try
            {
            	if (socket!=null)
            		socket.close();
            }
            catch (IOException e)
            {
                // TODO Auto-generated catch block
            }
        }
    }

    /**
     * returns canExit value
     * 
     * @return canExit value
     */
    private boolean canExit()
    {
        synchronized (this)
        {
            return canExit;
        }
    }

    /**
     * Closes the socked handler
     */
    public void close()
    {
        synchronized (this)
        {
            canExit = true;
        }
    }

}