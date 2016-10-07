package org.ipdr.protocol;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/**
 * Represents a protocol info record of the discovery message
 */
public class ProtocolInfo
{
    /**
     * Holds transport type
     */
    protected int           transportType;
    /**
     * Holds protocol version
     */
    protected int           protocolVersion;
    /**
     * Holds port number
     */
    short                   portNumber;
    /**
     * Holds value for the reserved field
     */
    short                   reserved;

    /**
     * Constant for TCP transport type
     */
    public final static int TCP_TRANSPORT  = 1;
    /**
     * Constant for SCTP transport type
     */
    public final static int SCTP_TRANSPORT = 2;

    /**
     * Constant for CRANE protocol
     */
    public final static int CRANE_VERSION  = 1;
    /**
     * Constant for IPDR protocol
     */
    public final static int IPDR_VERSION   = 2;

    /**
     * Constructor for ProtocolInfo.
     */
    public ProtocolInfo(int transportType, int protocolVersion,
            short portNumber, short reserved)
    {
        this.transportType = transportType;
        this.protocolVersion = protocolVersion;
        this.portNumber = portNumber;
        this.reserved = reserved;
    }

    /**
     * Returns the portNumber.
     * 
     * @return short
     */
    public short getPortNumber()
    {
        return portNumber;
    }

    /**
     * Returns the protocolVersion.
     * 
     * @return int
     */
    public int getProtocolVersion()
    {
        return protocolVersion;
    }

    /**
     * Returns the reserved.
     * 
     * @return short
     */
    public short getReserved()
    {
        return reserved;
    }

    /**
     * Returns the transportType.
     * 
     * @return int
     */
    public int getTransportType()
    {
        return transportType;
    }

    public final static int INFO_SIZE = 12;

    /**
     * @see java.lang.Object#toString()
     */
    public String toString()
    {
        return "transport:"
                + (transportType == TCP_TRANSPORT ? "TCP" : "other") + ","
                + "protocol:" + protocolVersion + "," + "port:" + portNumber;
    }

}