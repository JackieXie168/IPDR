package org.ipdr.crane;

import org.ipdr.protocol.ProtocolMalformedMsgException;

// $Id: ProtocolMsgGET_TMPL.java,v 1.3 2005/01/30 15:13:45 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/**
 * The Java encapsulation of a CRANE protocol messages: "GET_TMPL". <xmp>0 1 2
 * 3 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ | Version |
 * MID=0x16 | Session ID | Message Flags |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ | Message
 * Length | +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
 * Request ID | Reserved |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ </xmp>
 * 
 * @version $Id: ProtocolMsgGET_TMPL.java,v 1.3 2005/01/30 15:13:45 ilyad Exp $
 */
public final class ProtocolMsgGET_TMPL extends ProtocolMsg
{
    /** $Id: ProtocolMsgGET_TMPL.java,v 1.3 2005/01/30 15:13:45 ilyad Exp $ */
    public static String versionStr()
    {
        return "@(#) $Id: ProtocolMsgGET_TMPL.java,v 1.3 2005/01/30 15:13:45 ilyad Exp $";
    }

    public String msgName()
    {
        return "GET_TMPL";
    }

    public static final byte MSG_ID = 0x16;

    public final byte msgId()
    {
        return MSG_ID;
    }

    /// The request id
    public int requestId;

    /**
     * Construct from raw bytes
     * 
     * @param msg
     *            The raw message bytes
     * @param length
     *            The number of bytes in the message
     */
    public ProtocolMsgGET_TMPL(byte[] msg, int length)
            throws ProtocolMalformedMsgException
    {
        super(msg, length, MSG_LENGTH);
        try
        {
            this.requestId = bigE.getUnsignedShort(msg, REQUEST_ID_OFFSET);
        }
        catch (IndexOutOfBoundsException e)
        {
            handleIndexOutOfBounds(e, msg, length);
        }
    }

    /**
     * Construct from values.
     */
    public ProtocolMsgGET_TMPL(byte sessionId, int requestId)
    {
        super(MSG_ID, sessionId, MSG_LENGTH);
        this.requestId = requestId;
    }

    public final byte[] createRawMsg()
    {
        byte[] msgData = new byte[MSG_LENGTH];
        setHeader(msgData);
        bigE.putUnsignedShort(msgData, REQUEST_ID_OFFSET, requestId);
        bigE.putUnsignedShort(msgData, RESERVED1_OFFSET, 0);
        return msgData;
    }

    public String toString()
    {
        return super.toString() + "[reqId=" + this.requestId + "]";
    }

    private final static int REQUEST_ID_OFFSET = 8;

    private final static int RESERVED1_OFFSET = 10;

    private final static int MSG_LENGTH = 12;
}