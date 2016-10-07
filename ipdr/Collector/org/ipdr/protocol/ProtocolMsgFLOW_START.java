package org.ipdr.protocol;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
//ANY KIND either expresses or implied.

/**
 * The class represents a FLOW_START message
 *
 */
import org.ipdr.api.FlowStartInfo;
public class ProtocolMsgFLOW_START extends ProtocolMsgHeaderOnly implements FlowStartInfo
{
    /**
     * @see org.ipdr.protocol.ProtocolMsgIdentification#msgName()
     */
    public String msgName()
    {
        return "FLOW_START";
    }

    public static final byte MSG_ID = 0x01;

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
    public ProtocolMsgFLOW_START(byte sessionId)
    {
        super(MSG_ID, sessionId, ProtocolStreamMsg.HDR_LENGTH);
    }
    
    /**
     * @see org.ipdr.protocol.ProtocolStreamMsg#getSessionId()
     */
    public byte getSessionId()
    {
    	return this.sessionId;
    }

}
