package org.ipdr.protocol;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/**
 * The class represents a GET_TEMPLATES message.
 * The message has the following structure:
 * <xmp>
 * struct GetTemplates 
 * { 
 * 		struct IPDRStreamingHeader header; 
 * 		short requestId; // numeric ID of request for Templates  
 * }
 * </xmp>
 * 
 */
public class ProtocolMsgGET_TEMPLATES extends ProtocolStreamMsg
{
    /**
     * @see org.ipdr.protocol.ProtocolMsgIdentification#msgName()
     */
    public String msgName()
    {
        return "GET_TEMPLATES";
    }

    public static final byte MSG_ID = 0x16;

    /**
     * @see org.ipdr.protocol.ProtocolStreamMsg#msgId()
     */
    public final byte msgId()
    {
        return MSG_ID;
    }

    /**
     * Holds request id
     */
    protected short requestId;

    /**
     * Creates message from values
     * @param requestId request id
     */
    public ProtocolMsgGET_TEMPLATES(byte sessionId, short requestId)
    {
        this.msgLength = MSG_LENGTH;
        this.requestId = requestId;
        this.sessionId = sessionId;
    }

    /**
     * Creates message from raw bytes
     * @param message raw bytes
     * @param length buffer length
     * @throws ProtocolMalformedMsgException
     */
    protected ProtocolMsgGET_TEMPLATES(byte[] message, int length)
            throws ProtocolMalformedMsgException
    {
        super(message, length, MSG_LENGTH);
        this.requestId = bigE.getShort(message, REQUEST_ID_OFFSET);
        // load and parse templates
    }

    protected static final int REQUEST_ID_OFFSET = 8;

    protected static final int MSG_LENGTH = 10;

    /**
     * @see org.ipdr.protocol.ProtocolMsg#createRawMsg()
     */
    protected byte[] createRawMsg()
    {
        byte[] message = new byte[this.msgLength];
        setHeader(message);
        bigE.putShort(message, REQUEST_ID_OFFSET, requestId);
        return message;
    }

    /**
     * Returns the requestId.
     * 
     * @return short
     */
    public short getRequestId()
    {
        return requestId;
    }

    /**
     * @see java.lang.Object#toString()
     */
    public String toString()
    {
        return toStringBase() + "[request:" + getRequestId() + "]";
    }

}