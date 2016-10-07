
package org.ipdr.protocol;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/**
 * The class represents a START_NEGOTIATION_REJECT message
 */
public class ProtocolMsgSTART_NEGOTIATION_REJECT extends ProtocolMsgHeaderOnly
{

    /**
     * Initialize class parameters
     * 
     * @param msg
     * @param length
     * @throws ProtocolMalformedMsgException
     */
    public ProtocolMsgSTART_NEGOTIATION_REJECT(byte[] msg, int length) throws ProtocolMalformedMsgException
    {
        super(msg, length, ProtocolStreamMsg.HDR_LENGTH);
    }

    /**
     * Initialize class parameters
     * 
     * @param sessionId
     */
    public ProtocolMsgSTART_NEGOTIATION_REJECT(byte sessionId)
    {
        super(MSG_ID, sessionId, ProtocolStreamMsg.HDR_LENGTH);
    }

    public static final byte MSG_ID = 0x1E;
    /**
     * @see org.ipdr.protocol.ProtocolStreamMsg#msgId()
     */
    public byte msgId()
    {
        return MSG_ID;
    }

    /**
     * @see org.ipdr.protocol.ProtocolMsgIdentification#msgName()
     */
    public String msgName()
    {
        return "START_NEGOTIATION_REJECT";
    }

}
