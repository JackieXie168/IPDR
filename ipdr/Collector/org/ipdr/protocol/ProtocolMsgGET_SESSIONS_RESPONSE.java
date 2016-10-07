package org.ipdr.protocol;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.util.ArrayList;

/**
 * The class represents a GET_SESSIONS_RESPONSE message.
 * The message structure is as follows:
 * <xmp>
 * struct GetSessionsResponse 
 * { 
 * 		struct IPDRStreamingHeader header; 
 * 		short requestId; // allows match up of response to request 
 *		struct SessionBlock sessionBlocks <>; // description of supported Sessions 
 * }
 * </xmp>
 * A session block has the following structure:
 * <xmp>
 * struct SessionBlock 
 * {
 * 		char sessionId; 
 * 		char reserved; 
 * 		UTF8String sessionName; // Optional names and description for the Session 
 * 		UTF8String sessionDescription; 
 * 		int ackTimeInterval; // the maximum time between acks for this Session in sec. 
 * 		int ackSequenceInterval; // the maximum number of unacknowledged data items  
 * }
 * </xmp>
 */
public class ProtocolMsgGET_SESSIONS_RESPONSE extends ProtocolStreamMsg
{
    /**
     * @see org.ipdr.protocol.ProtocolMsgIdentification#msgName()
     */
    public String msgName()
    {
        return "GET_SESSIONS_RESPONSE";
    }

    public static final byte MSG_ID = 0x15;

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
     * Holds session infos
     */
    protected ArrayList sessions;

    /**
     * Creates message from values
     * @param requestId request id
     * @param sessions <code>ArrayList</code> of session info blocks
     */
    protected ProtocolMsgGET_SESSIONS_RESPONSE(short requestId,
            ArrayList sessions)
    {
        this.requestId = requestId;
        this.sessions = sessions;
    }

    /**
     * Creates message from raw bytes
     * @param message raw message
     * @param length buffer length
     * @throws ProtocolMalformedMsgException
     */
    protected ProtocolMsgGET_SESSIONS_RESPONSE(byte[] message, int length)
            throws ProtocolMalformedMsgException
    {
        super(message, length, MSG_LENGTH);
        this.requestId = bigE.getShort(message, REQUEST_ID_OFFSET);
        this.sessions = bigE.getSessionInfoBlock(message, SESSIONS_OFFSET);
    }

    protected static final int REQUEST_ID_OFFSET = 8;

    protected static final int SESSIONS_OFFSET = 10;

    protected static final int MSG_LENGTH = 14;

    /**
     * @see org.ipdr.protocol.ProtocolMsg#createRawMsg()
     */
    protected byte[] createRawMsg()
    {
        byte[] message = new byte[this.msgLength];
        setHeader(message);
        bigE.putShort(message, REQUEST_ID_OFFSET, requestId);
        unimplemented(this, "createRawMsg");
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
     * Returns sessions parameters
     * 
     * @return ArrayList
     */
    public ArrayList getSessions()
    {
        return sessions;
    }
    /**
     * @see java.lang.Object#toString()
     */
    public String toString()
    {
        return toStringBase() + "[request:" + getRequestId() + ","
                + "sessions count:" + sessions.size() + "]";
    }

}