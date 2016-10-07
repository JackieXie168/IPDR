package org.ipdr.protocol;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.util.ArrayList;

/**
 * The class represents the MODIFY_TEMPLATE_RESPONSE message
 *  
 */
public class ProtocolMsgMODIFY_TEMPLATE_RESPONSE extends ProtocolMsgTemplate
{
    /**
     * @see org.ipdr.protocol.ProtocolMsgIdentification#msgName()
     */
    public String msgName()
    {
        return "MODIFY_TEMPLATE_RESPONSE";
    }

    public static final byte MSG_ID = 0x1b;

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
     * @param configId
     * @param templates
     */
    public ProtocolMsgMODIFY_TEMPLATE_RESPONSE(byte sessionId, short configId,
            ArrayList templates)
    {
        super(sessionId, configId, (byte) 0, templates);
    }

    /**
     * Initialize class parameters
     * 
     * @param message
     * @param length
     * @throws ProtocolMalformedMsgException
     */
    public ProtocolMsgMODIFY_TEMPLATE_RESPONSE(byte[] message, int length)
            throws ProtocolMalformedMsgException
    {
        super(message, length);
    }
}