package org.ipdr.protocol;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.util.ArrayList;

/**
 * The class represents the TEMPLATE_DATA message
 *  
 */
public class ProtocolMsgTEMPLATE_DATA extends ProtocolMsgTemplate
{

    /**
     * @see org.ipdr.protocol.ProtocolMsgIdentification#msgName()
     */
    public String msgName()
    {
        return "TEMPLATE_DATA";
    }

    public static final byte MSG_ID = 0x10;

    public final byte msgId()
    {
        return MSG_ID;
    }

    public ProtocolMsgTEMPLATE_DATA(byte sessionId, short configId,
            boolean isNegotiable, ArrayList templates)
    {
        super(sessionId, configId, (byte) (isNegotiable ? 1 : 0), templates);
    }

    public ProtocolMsgTEMPLATE_DATA(byte[] message, int length)
            throws ProtocolMalformedMsgException
    {
        super(message, length);
    }

    /**
     * Returns the isNegotiable.
     * 
     * @return boolean
     */
    public boolean isNegotiable()
    {
        return (flags & 0x1) == 1;
    }

    /**
     * @see java.lang.Object#toString()
     */
    public String toString()
    {
        return super.toString() + "[negotiable:" + isNegotiable() + "]";
    }

}