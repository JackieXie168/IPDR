package org.ipdr.api;

import java.net.InetAddress;

import org.ipdr.api.configuration.ConnectionConfigI;
import org.ipdr.api.configuration.SessionConfigI;

/**
 * The interface allows to control the ability of establishing flow start state in a connection
 *
 */
public interface ConnectionController
{
    /**
     * Allows control on initialization of a connection
     * @param exporterAddress address of the exporter 
     * @param connectionConfig
     * @return boolean
     */
    public boolean canConnect(InetAddress exporterAddress, ConnectionConfigI connectionConfig);
    /**
     * Allows control on initialization of a flow start state in an established connection
     * @param exporterAddress
     * @param connectionConfig
     * @param sessionConfig
     * @return boolean
     */
    public boolean canStartFlow(InetAddress exporterAddress, ConnectionConfigI connectionConfig, SessionConfigI sessionConfig);
}
