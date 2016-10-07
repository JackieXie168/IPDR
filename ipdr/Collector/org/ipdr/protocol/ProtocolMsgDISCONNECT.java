package org.ipdr.protocol;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
//ANY KIND either expresses or implied.

/**
 * The class represents a DISCONNECT message
 *
 */
import org.ipdr.api.DisconnectInfo;
public class ProtocolMsgDISCONNECT extends ProtocolMsgHeaderOnly implements DisconnectInfo
{
    /**
     * @see org.ipdr.protocol.ProtocolMsgIdentification#msgName()
     */
    public String msgName()
    {
        return "DISCONNECT";
    }

    public static final byte MSG_ID = 0x07;

    /**
     * @see org.ipdr.protocol.ProtocolStreamMsg#msgId()
     */
    public final byte msgId()
    {
        return MSG_ID;
    }

    /**
     * Initialize class parameters with default values
     */
    public ProtocolMsgDISCONNECT()
    {
        super(MSG_ID, (byte) 0, ProtocolStreamMsg.HDR_LENGTH);
    }

    /**
     * Initialize class parameters
     * 
     * @param msg
     * @param length
     * @throws ProtocolMalformedMsgException
     */
    public ProtocolMsgDISCONNECT(byte[] msg, int length)
            throws ProtocolMalformedMsgException
    {
        super(msg, length, HDR_LENGTH);
    }

}
