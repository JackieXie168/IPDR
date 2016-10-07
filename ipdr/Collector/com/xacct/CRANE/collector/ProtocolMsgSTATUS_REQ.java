package com.xacct.CRANE.collector;

// $Id: ProtocolMsgSTATUS_REQ.java,v 1.4 2005/01/30 15:13:40 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/** The Java encapsulation of a CRANE protocol message: "STATUS_REQ".
 *  <xmp>
 *        0                   1                   2                   3
 *        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |  Version      |  MID=0x30     | Session ID    | Message Flags |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                         Message Length                        |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  </xmp>
 *
 *  @version $Id: ProtocolMsgSTATUS_REQ.java,v 1.4 2005/01/30 15:13:40 ilyad Exp $
 */
public final class ProtocolMsgSTATUS_REQ extends ProtocolMsg
{
    /** $Id: ProtocolMsgSTATUS_REQ.java,v 1.4 2005/01/30 15:13:40 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: ProtocolMsgSTATUS_REQ.java,v 1.4 2005/01/30 15:13:40 ilyad Exp $"; }

    public String msgName() { return "STATUS_REQ"; }
    public static final byte MSG_ID = 0x30;
    public final byte msgId() { return MSG_ID; }

    /** Construct from raw bytes
     *  @param msg The raw message bytes
     *  @param length The number of bytes in the message
     */
    public ProtocolMsgSTATUS_REQ(byte [] msg, int length) throws ProtocolMalformedMsgException {
        super(msg, length, MSG_LENGTH);
    }

    /** Construct from values
     *  @param sessionId The session identifier
     */
    public ProtocolMsgSTATUS_REQ(byte sessionId) {
        super(MSG_ID, sessionId, MSG_LENGTH);
    }

    public final byte [] createRawMsg() {
        byte [] msgData = new byte[MSG_LENGTH];
        setHeader(msgData);
        return msgData;
    }

    private final static int MSG_LENGTH = 8;
}
