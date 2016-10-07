package org.ipdr;

import java.net.InetAddress;

import org.ipdr.api.ConnectionThread;
import org.ipdr.api.configuration.ConnectionConfigI;

//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/**
 * Provides interface for handling of incoming connections. Used by
 * <code>ListeningSocketHandler</code>.
 */
public interface IncomingConnectionHandler
{
    /**
     * Attempts handling of an incoming connection
     * 
     * @param connection -
     *            connection to handle
     * @return true if the connection is handled, false if it is rejected.
     */
    public boolean handleIncomingConnection(ConnectionDynamic connection);
    
    /**
     * Tries to relate the exporter's IP to an exsiting exporterHandler 
     * 
     * @param exportersAddress -
     *            address of the exporter
     * @return true if the exporter's address has a related handler, false otherwise.
     */
    public ConnectionConfigI exporterAddressKnown(InetAddress exportersAddress,
    		ConnectionThread thread);
}