package com.xacct.CRANE.collector;

// $Id: ProtocolMsgGET_SESS.java,v 1.4 2005/01/30 15:13:42 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/** The Java encapsulation of a CRANE protocol messages: "GET_SESS".
 *  <xmp>
 *        0                   1                   2                   3
//        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |  Version      |  MID=0x14     | Session ID    | Message Flags |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                         Message Length                        |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |           Request ID          |        Reserved               |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  </xmp>
 *
 *  @version $Id: ProtocolMsgGET_SESS.java,v 1.4 2005/01/30 15:13:42 ilyad Exp $
 */
public final class ProtocolMsgGET_SESS extends ProtocolMsg
{
    /** $Id: ProtocolMsgGET_SESS.java,v 1.4 2005/01/30 15:13:42 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: ProtocolMsgGET_SESS.java,v 1.4 2005/01/30 15:13:42 ilyad Exp $"; }

    public String msgName() { return "GET_SESS"; }
    public static final byte MSG_ID = 0x14;
    public final byte msgId() { return MSG_ID; }

    /// The request id
    public int requestId;

    /** Construct from raw bytes
     *  @param msg The raw message bytes
     *  @param length The number of bytes in the message
     */
    public ProtocolMsgGET_SESS(byte [] msg, int length) throws ProtocolMalformedMsgException {
        super(msg, length, MSG_LENGTH);
        try {
            this.requestId = bigE.getUnsignedShort(msg, REQUEST_ID_OFFSET);
        } catch (IndexOutOfBoundsException e) { handleIndexOutOfBounds(e, msg, length); }
    }

    /** Construct from values.
     */
    public ProtocolMsgGET_SESS(int requestId) {
        super(MSG_ID, (byte)0, MSG_LENGTH);
        this.requestId = requestId;
    }

    public final byte [] createRawMsg() {
        byte [] msgData = new byte[MSG_LENGTH];
        setHeader(msgData);
        bigE.putUnsignedShort(msgData, REQUEST_ID_OFFSET, requestId);
        bigE.putUnsignedShort(msgData, RESERVED1_OFFSET, 0);
        return msgData;
    }

    public String toString() {
        return super.toString() +
            "[reqId=" + this.requestId + "]";
    }

    private final static int REQUEST_ID_OFFSET =  8;
    private final static int RESERVED1_OFFSET  = 10;

    private final static int MSG_LENGTH        = 12;
}
