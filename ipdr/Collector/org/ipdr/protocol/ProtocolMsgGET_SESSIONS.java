package org.ipdr.protocol;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/**
 * The class represents a GET_SESSIONS message.
 * The message has the following structure:
 * <xmp>
 * struct GetSessions 
 * { 
 * 		struct IPDRStreamingHeader header; 
 * 		short requestId; // numeric ID of request for Sessions  
 * }
 * </xmp>
 */

public class ProtocolMsgGET_SESSIONS extends ProtocolStreamMsg
{
    /**
     * @see org.ipdr.protocol.ProtocolMsgIdentification#msgName()
     */
    public String msgName()
    {
        return "GET_SESSIONS";
    }

    public static final byte MSG_ID = 0x14;

    /**
     * @see org.ipdr.protocol.ProtocolStreamMsg#msgId()
     */
    public final byte msgId()
    {
        return MSG_ID;
    }

    /**
     * Holds the request id
     */
    protected short requestId;

    /**
     * Creates the message from values
     * @param requestId request id
     */
    public ProtocolMsgGET_SESSIONS(short requestId)
    {
        this.requestId = requestId;
    }

    /**
     * Creates the message from raw bytes
     * 
     * @param message raw bytes
     * @param length buffer length
     * @throws ProtocolMalformedMsgException
     */
    protected ProtocolMsgGET_SESSIONS(byte[] message, int length)
            throws ProtocolMalformedMsgException
    {
        super(message, length, MSG_LENGTH);
        this.requestId = bigE.getShort(message, REQUEST_ID_OFFSET);
    }

    protected static final int REQUEST_ID_OFFSET = 8;

    protected static final int MSG_LENGTH = 10;

    /**
     * @see org.ipdr.protocol.ProtocolMsg#createRawMsg()
     */
    protected byte[] createRawMsg()
    {
        byte[] message = new byte[MSG_LENGTH];
        this.msgLength = MSG_LENGTH;
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
        return toStringBase() + "[request:" + requestId + "]";
    }

}