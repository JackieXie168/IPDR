package com.xacct.CRANE.collector;

// $Id: ProtocolMsgSTOP_ACK.java,v 1.4 2005/01/30 15:13:41 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/** The Java encapsulation of a CRANE protocol messages: "STOP_ACK".
 *  <xmp>
 *        0                   1                   2                   3
 *        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |  Version      |  MID=0x04     | Session ID    | Message Flags |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                         Message Length                        |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  </xmp>
 *
 *  @version $Id: ProtocolMsgSTOP_ACK.java,v 1.4 2005/01/30 15:13:41 ilyad Exp $
 */
public final class ProtocolMsgSTOP_ACK extends ProtocolMsg
{
    /** $Id: ProtocolMsgSTOP_ACK.java,v 1.4 2005/01/30 15:13:41 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: ProtocolMsgSTOP_ACK.java,v 1.4 2005/01/30 15:13:41 ilyad Exp $"; }

    public String msgName() { return "STOP_ACK"; }
    public static final byte MSG_ID = 0x04;
    public final byte msgId() { return MSG_ID; }

    /** Construct from raw bytes
     *  @param msg The raw message bytes
     *  @param length The number of bytes in the message
     */
    public ProtocolMsgSTOP_ACK(byte [] msg, int length) throws ProtocolMalformedMsgException {
        super(msg, length, MSG_LENGTH);
    }

    /** Construct from values
     *  @param sessionId The session identifier
     */
    public ProtocolMsgSTOP_ACK(byte sessionId) {
        super(MSG_ID, sessionId, MSG_LENGTH);
    }

    public final byte [] createRawMsg() {
        byte [] msgData = new byte[MSG_LENGTH];
        setHeader(msgData);
        return msgData;
    }

    private final static int MSG_LENGTH = 8;
}
