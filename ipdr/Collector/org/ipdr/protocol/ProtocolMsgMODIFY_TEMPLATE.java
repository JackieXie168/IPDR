package org.ipdr.protocol;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.util.ArrayList;

/**
 * The class represents the MODIFY_TEMPLATE message
 *  
 */
public class ProtocolMsgMODIFY_TEMPLATE extends ProtocolMsgTemplate
{

    /**
     * @see org.ipdr.protocol.ProtocolMsgIdentification#msgName()
     */
    public String msgName()
    {
        return "MODIFY_TEMPLATE";
    }

    public static final byte MSG_ID = 0x1a;

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
    public ProtocolMsgMODIFY_TEMPLATE(byte sessionId, short configId,
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
    public ProtocolMsgMODIFY_TEMPLATE(byte[] message, int length)
            throws ProtocolMalformedMsgException
    {
        super(message, length);
    }

    /**
     * @see java.lang.Object#toString()
     */
    public String toString()
    {
        return null;
    }

}