//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

package org.ipdr.protocol;

/**
 * The class represents a START_NEGOTIATION message
 */
public class ProtocolMsgSTART_NEGOTIATION extends ProtocolMsgHeaderOnly
{
    public static final byte MSG_ID = 0x1d;

    /**
     * @see org.ipdr.protocol.ProtocolStreamMsg#msgId()
     */
    public byte msgId()
    {
        return (byte) MSG_ID;
    }

    /**
     * Initialize class parameters
     * 
     * @param msg
     * @param length
     * @param requiredLength
     * @throws ProtocolMalformedMsgException
     */
    public ProtocolMsgSTART_NEGOTIATION(byte[] msg, int length,
            int requiredLength) throws ProtocolMalformedMsgException
    {
        super(msg, length, requiredLength);
    }

    /**
     * Initialize class parameters
     * 
     * @param sessionId
     */
    public ProtocolMsgSTART_NEGOTIATION(byte sessionId)
    {
        super(MSG_ID, sessionId, ProtocolStreamMsg.HDR_LENGTH);
    }

    /**
     * 
     * @see org.ipdr.protocol.ProtocolMsgIdentification#msgName()
     */
    public String msgName()
    {
        return "START_NEGOTIATION";
    }
}