package org.ipdr.protocol;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.net.Inet4Address;

/**
 * The class represents a CONNECT message.
 * The message has the following structure:
 * <xmp>
 * struct Connect
 * { 
 * 		struct IPDRStreamingHeader header; 
 * 		int initiatorAddress;
 * 		short initiatorPort;
 * 		int capabilities; // an array of capability bits for capability negotiation 
 * 		int keepAliveInterval; // the maximum delay between some indication from remote, in seconds 
 * 		UTF8String vendorId; // the vendor ID of Exporter/Collector 
 * }
 * </xmp>
 */
public class ProtocolMsgCONNECT extends ProtocolStreamMsg
{
    /**
     * @see org.ipdr.protocol.ProtocolMsgIdentification#msgName()
     */
    public String msgName()
    {
        return "CONNECT";
    }

    public static final byte MSG_ID = 0x05;

    /**
     * @see org.ipdr.protocol.ProtocolStreamMsg#msgId()
     */
    public final byte msgId()
    {
        return MSG_ID;
    }

    /**
     * Holds connection initiatior address
     */
    private Inet4Address initiatorAddress;

    /**
     * Holds connection initiator port 
     */
    private int initiatorPort;

    /**
     * Holds capabilities bit vector
     */
    private long capabilities;

    /**
     * Holds keepalive interval
     */
    private long keepAliveInterval;

    /**
     * Holds vendor id
     */
    private String vendorId;

    /**
     * initiator address accessor
     */
    public Inet4Address getInitiatorAddress()
    {
    	return initiatorAddress;
    }
    /**
     * initiator port accessor
     */
    public int getInitiatorPort()
    {
    	return initiatorPort;
    }
    
    /**
     * Allows convenient setting of capabilities bits
     * @param capabilitiesStructures structured templates capability
     * @param capabilitiesMultiSession multisession capability
     * @param capabilitiesTemplateNegotiation template negotiation capability
     */
    protected void setCapabilities(boolean capabilitiesStructures,
            boolean capabilitiesMultiSession,
            boolean capabilitiesTemplateNegotiation)
    {
        this.capabilities = 0;
        if (capabilitiesStructures) capabilities |= 0x1;
        if (capabilitiesMultiSession) capabilities |= 0x2;
        if (capabilitiesTemplateNegotiation) capabilities |= 0x4;
    }

    /**
     * @see org.ipdr.protocol.ProtocolMsg#createRawMsg()
     */
    public final byte[] createRawMsg()
    {
        byte[] msgData = new byte[MSG_LENGTH + vendorId.length()];
        this.msgLength = MSG_LENGTH + vendorId.length();
        setHeader(msgData);
        bigE.putAddr4(msgData, SERVER_ADDRESS_OFFSET, this.initiatorAddress);
        bigE.putUnsignedShort(msgData, SERVER_PORT_OFFSET, this.initiatorPort);
        bigE.putUnsignedInt(msgData, CAPABILITIES_OFFSET, this.capabilities);
        bigE.putUnsignedInt(msgData, KEEPALIVE_OFFSET, this.keepAliveInterval);
        bigE.putStringLen(msgData, VENDOR_LEN_OFFSET, VENDOR_ID_OFFSET,
                vendorId);
        return msgData;
    }

    /**
     * Create message from raw bytes
     * @param msg bytes
     * @param length length
     * @throws ProtocolMalformedMsgException
     */
    public ProtocolMsgCONNECT(byte[] msg, int length)
            throws ProtocolMalformedMsgException
    {
        super(msg, length, MSG_LENGTH);
        try
        {
            this.initiatorAddress = bigE.getAddr4(msg, SERVER_ADDRESS_OFFSET);
            this.initiatorPort = bigE.getUnsignedShort(msg, SERVER_PORT_OFFSET);
            this.capabilities = bigE.getUnsignedInt(msg, CAPABILITIES_OFFSET);
            this.keepAliveInterval = bigE.getUnsignedInt(msg, KEEPALIVE_OFFSET);
            this.vendorId = bigE.getStringLen(msg, VENDOR_LEN_OFFSET,
                    VENDOR_ID_OFFSET);
        }
        catch (IndexOutOfBoundsException e)
        {
            handleIndexOutOfBounds(e, msg, length);
        }
    }

    /**
     * Creates message from values
     * @param initiatorAddress initiator address
     * @param initiatorPort initiator port
     * @param capabilitiesStructures structured templates capability flag
     * @param capabilitiesMultiSession multisession capability flag
     * @param capabilitiesTemplateNegotiation template negotiation flag
     * @param keepAliveInterval keepalive interval value
     * @param vendorId vendor id
     */
    public ProtocolMsgCONNECT(Inet4Address initiatorAddress, int initiatorPort,
            boolean capabilitiesStructures, boolean capabilitiesMultiSession,
            boolean capabilitiesTemplateNegotiation, int keepAliveInterval,
            String vendorId)
    {
        this.initiatorAddress = initiatorAddress;
        this.initiatorPort = initiatorPort;
        setCapabilities(capabilitiesStructures, capabilitiesMultiSession,
                capabilitiesTemplateNegotiation);
        this.keepAliveInterval = keepAliveInterval;
        this.vendorId = vendorId;
    }

    private final static int SERVER_ADDRESS_OFFSET = 8;

    private final static int SERVER_PORT_OFFSET = 12;

    private final static int CAPABILITIES_OFFSET = 14;

    private final static int KEEPALIVE_OFFSET = 18;

    private final static int VENDOR_LEN_OFFSET = 22;

    private final static int VENDOR_ID_OFFSET = 26;

    private final static int MSG_LENGTH = 26;

    /**
     * Returns the keepAliveInterval.
     * 
     * @return long
     */
    public long getKeepAliveInterval()
    {
        return keepAliveInterval;
    }

    /**
     * Returns structured templates capability
     * @return boolean
     */
    public boolean getCapabilitiesStructures()
    {
        return ((capabilities & 0x1) != 0);
    }

    /**
     * Returns multisession capability
     * @return boolean
     */
    public boolean getCapabilitiesMultiSession()
    {
        return ((capabilities & 0x2) != 0);
    }

    /**
     * Returns template negotiation capability
     * @return boolean
     */
    public boolean getCapabilitiesTemplateNegotiation()
    {
        return ((capabilities & 0x4) != 0);
    }

    /**
     * @see java.lang.Object#toString()
     */
    public String toString()
    {
        return toStringBase() + "[" + "structs:" + getCapabilitiesStructures()
                + "," + "multisession:" + getCapabilitiesMultiSession() + ","
                + "negotiation:" + getCapabilitiesTemplateNegotiation() + ","
                + "keepalive:" + getKeepAliveInterval() + "," + "vendor:'"
                + getVendorId() + "'" + "]";
    }

    /**
     * Returns the vendorId.
     * 
     * @return String
     */
    public String getVendorId()
    {
        return vendorId;
    }

}