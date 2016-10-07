package org.ipdr.protocol;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/**
 * The class represents a FINAL_TEMPLATE_DATA_ACK message
 */
public class ProtocolMsgFINAL_TEMPLATE_DATA_ACK extends ProtocolMsgHeaderOnly
{
    /**
     * @see org.ipdr.protocol.ProtocolMsgIdentification#msgName()
     */
    public String msgName()
    {
        return "FINAL_TEMPLATE_DATA_ACK";
    }

    public static final byte MSG_ID = 0x13;

    /**
     * @see org.ipdr.protocol.ProtocolStreamMsg#msgId()
     */
    public final byte msgId()
    {
        return MSG_ID;
    }

    /**
     * Initialize class parameters
     * 
     * @param sessionId
     */
    public ProtocolMsgFINAL_TEMPLATE_DATA_ACK(byte sessionId)
    {
        super(MSG_ID, sessionId, HDR_LENGTH);
        this.sessionId = sessionId;
        this.msgLength = HDR_LENGTH;
    }

}