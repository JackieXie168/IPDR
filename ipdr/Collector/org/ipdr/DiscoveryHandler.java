package org.ipdr;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.Inet4Address;
import java.net.SocketException;
import java.net.SocketTimeoutException;
import java.net.UnknownHostException;
import java.util.ArrayList;

import org.ipdr.api.configuration.CollectorConfigI;
import org.ipdr.api.log.LogManager;
import org.ipdr.protocol.ProtocolInfo;
import org.ipdr.protocol.ProtocolMalformedMsgException;
import org.ipdr.protocol.ProtocolMsgVERSION_REQUEST;
import org.ipdr.protocol.ProtocolMsgVERSION_RESPONSE;

/**
 * Handles incoming discovery requests.
 */
public class DiscoveryHandler extends Thread
{

    /**
     * Indicates that the thread may exit
     */
    protected volatile boolean          canExit = false;
    /**
     * Holds collector configuration
     */
    protected CollectorConfigI collectorConfig;
    /**
     * Holds datagram listening socket
     */
    protected DatagramSocket   socket  = null;

    /**
     * Initializes the handler with given collector configuration
     * 
     * @param collectorConfig
     */
    public DiscoveryHandler(CollectorConfigI collectorConfig)
    {
        this.collectorConfig = collectorConfig;
    }

    /**
     * @return Returns the canExit.
     */
    public boolean isCanExit()
    {
        return canExit;
    }

    /**
     * Sets canExit when it is time to quit.
     * 
     * @param canExit
     *            The canExit to set.
     */
    protected void setCanExit(boolean canExit)
    {
        this.canExit = canExit;
    }

    /**
     * Shuts the discovery handler down
     */
    public void close()
    {
        setCanExit(true);
    }

    /**
     * @see java.lang.Runnable#run()
     */
    public void run()
    {
        setName("Discovery handler");
        try
        {
            socket = new DatagramSocket(collectorConfig.getUDPDiscoveryPort(),
                    collectorConfig.getCollectorAddress());
            LogManager.info(this, "run", "Listening for incoming UDP requests on " + collectorConfig.getUDPDiscoveryPort());
            socket.setSoTimeout(collectorConfig.getAcceptTimeout());
            byte[] buffer = new byte[ProtocolMsgVERSION_REQUEST.MSG_LENGTH];

            DatagramPacket packet = new DatagramPacket(buffer,
                    ProtocolMsgVERSION_REQUEST.MSG_LENGTH);

            while (!isCanExit())
            {
                try
                {
                    socket.receive(packet);
                    // packet arrived. handle it.
                    handleUDPDatagram(packet);
                }
                catch (SocketTimeoutException e)
                {
                }
                catch (ProtocolMalformedMsgException e)
                {
                    LogManager.error(this, "run", e.getMessage());
                }
                catch (IOException e)
                {
                    LogManager.error(this, "run", e.getMessage());
                }
            }
        }
        catch (SocketException e)
        {
            LogManager.error(this, "run", e.getMessage());
        }
        finally
        {
            try
            {
                socket.close();
            }
            catch (Exception e)
            {
            }
        }
    }

    public static final int PROTOCOL_VERSION_UNKNOWN = -1;
    public static final int PROTOCOL_VERSION_UNSUPPORTED = -2;
    
    /**
     * Filters the TCP protocols from the result of a UDPDiscovery query
     * @param remoteAddress - exporter's IP
     * @param remotePort - UDP port
     * @param discoveryTimeout - UDP discovery timeout
     * @return ArrayList
     */
    public static synchronized ArrayList getTcpProtocols (Inet4Address localAddress, Inet4Address remoteAddress, int remotePort, int discoveryTimeout )
    {
        ArrayList allProtocols;
        allProtocols = DiscoveryHandler.queryAllProtocols((Inet4Address) localAddress, 0, remoteAddress, remotePort, discoveryTimeout);
        if (allProtocols == null)
            return null;
        
        ArrayList result = new ArrayList();
        for (int i = 0; i < allProtocols.size(); i++)
        {
            ProtocolInfo info = (ProtocolInfo) allProtocols.get(i);
            if (info.getTransportType() == ProtocolInfo.TCP_TRANSPORT)
                result.add(info);
        }
        return result;
    }
    
    /**
     * Query for all protocols used by this connection
     * @param localAddress - collector's IP address
     * @param bootTime - requester boot time
     * @param remoteAddress - exporter's IP address
     * @param remotePort - exporter's port
     * @param discoveryTimeout - UDP discovery timeout
     * @return ArrayList
     */
    public static synchronized ArrayList queryAllProtocols( Inet4Address localAddress, int bootTime, Inet4Address remoteAddress, int remotePort, int discoveryTimeout )
    {
    try
    {
        ProtocolMsgVERSION_REQUEST reqMessage = new ProtocolMsgVERSION_REQUEST( localAddress, bootTime);
        DatagramSocket discoverySocket;
        discoverySocket = new DatagramSocket();
        byte[] reqRawMessage = reqMessage.createRawMsg();
    	DatagramPacket reqPacket = new DatagramPacket(reqRawMessage, reqRawMessage.length, remoteAddress, remotePort);
    	discoverySocket.send(reqPacket);
    
        byte [] buffer = new byte[32000];
        DatagramPacket replyPacket = new DatagramPacket(buffer, 32000);
        discoverySocket.setSoTimeout(discoveryTimeout);
        discoverySocket.receive(replyPacket);

        // parse the packet
        ProtocolMsgVERSION_RESPONSE response = new ProtocolMsgVERSION_RESPONSE(replyPacket.getData());
        ArrayList result = new ArrayList();
        result.add(response.getDefaultProtocol());
        result.addAll(response.getAdditionalProtocols());
        
        return result;
    }
    catch(SocketTimeoutException e)
    {
        return null;
    }
    catch (SocketException e)
    {
        LogManager.error("DiscoveryHandler", "queryProtocolVersion", e.toString());
        return null;
    }
    catch (IOException e)
    {
        LogManager.error("DiscoveryHandler", "queryProtocolVersion", e.toString());
        return null;
    }
    catch (ProtocolMalformedMsgException e)
    {
        LogManager.error("DiscoveryHandler", "queryProtocolVersion", e.toString());
        return null;
    }   
    }
    
    /**
     * Return most latest version of IPDR type protocol which uses a TCP protocol
     * @param localAddress - collector IP address
     * @param bootTime - requester boot time
     * @param remoteAddress - exporter's IP address
     * @param remotePort - exporter's port
     * @param discoveryTimeout - UDP discovery time out
     * @param defaultVersion - defualt protocol version to query for
     * @return int
     */
    public static synchronized int queryProtocolVersionOnly( Inet4Address localAddress, int bootTime, Inet4Address remoteAddress, int remotePort, int discoveryTimeout, int defaultVersion)
    {
            ArrayList protos = queryAllProtocols(localAddress, bootTime, remoteAddress, remotePort, discoveryTimeout );
            if (protos == null)
                return PROTOCOL_VERSION_UNSUPPORTED;
            // check the default proto:
            ProtocolInfo defaultProtocol = (ProtocolInfo)protos.get(0);
            // if it is TCP, accept it.
            if (defaultProtocol.getTransportType() == ProtocolInfo.TCP_TRANSPORT)
                return defaultProtocol.getProtocolVersion();
            
            int supportedAlternative = PROTOCOL_VERSION_UNSUPPORTED;
            // locate tcp transport and default version
            for (int i = 1 ; i < protos.size(); i ++)
            {
                ProtocolInfo proto = (ProtocolInfo)protos.get(i);
                if (proto.getTransportType() != ProtocolInfo.TCP_TRANSPORT)
                    continue;
                if (proto.getProtocolVersion() == defaultVersion )
                    return defaultVersion;
                if (proto.getProtocolVersion() == ProtocolInfo.IPDR_VERSION || proto.getProtocolVersion() == ProtocolInfo.CRANE_VERSION)
                    supportedAlternative =proto.getProtocolVersion(); 
            }
            return supportedAlternative;

   }
    /**
     * Encapsulates handling of an incoming VERSION_REQUEST. The method replies
     * with VERSION_RESPONSE message, offering IPDR and CRANE protocols over TCP
     * as available options, IPDR the default.
     * 
     * @param packet
     *            The incoming datagram
     * @throws ProtocolMalformedMsgException
     * @throws IOException
     */
    protected void handleUDPDatagram(DatagramPacket packet)
            throws ProtocolMalformedMsgException, IOException
    {
        try
        {
            ProtocolMsgVERSION_REQUEST message = new ProtocolMsgVERSION_REQUEST(
                    packet.getData());

            ProtocolInfo defaultProtocol = new ProtocolInfo(
                    ProtocolInfo.TCP_TRANSPORT, ProtocolInfo.IPDR_VERSION,
                    (short) collectorConfig.getLocalPort(), (short) 0);
            ProtocolInfo extraProtocol = new ProtocolInfo(
                    ProtocolInfo.TCP_TRANSPORT, ProtocolInfo.CRANE_VERSION,
                    (short) collectorConfig.getLocalPort(), (short) 0);
            ArrayList extra = new ArrayList();
            extra.add(extraProtocol);

            ProtocolMsgVERSION_RESPONSE response = new ProtocolMsgVERSION_RESPONSE(
                    defaultProtocol, extra);
            byte[] responseRaw = response.createRawMsg();
            DatagramPacket outboundPacket = new DatagramPacket(responseRaw,
                    responseRaw.length, message.getRequesterAddress(),
                    packet.getPort());
            socket.send(outboundPacket);
        }
        catch (UnknownHostException e)
        {
            throw new ProtocolMalformedMsgException("Bad requester address",
                    packet.getData(), ProtocolMsgVERSION_REQUEST.MSG_LENGTH, 0);
        }
    }
}