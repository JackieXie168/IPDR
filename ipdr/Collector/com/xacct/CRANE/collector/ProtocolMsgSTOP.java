package com.xacct.CRANE.collector;

// $Id: ProtocolMsgSTOP.java,v 1.4 2005/01/30 15:13:40 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/** The Java encapsulation of a CRANE protocol message: "STOP".
 *  <xmp>
 *        0                   1                   2                   3
 *        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |  Version      |  MID=0x03     | Session ID    | Message Flags |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                         Message Length                        |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  </xmp>
 *
 *  @version $Id: ProtocolMsgSTOP.java,v 1.4 2005/01/30 15:13:40 ilyad Exp $
 */
public final class ProtocolMsgSTOP extends ProtocolMsg
{
    /** $Id: ProtocolMsgSTOP.java,v 1.4 2005/01/30 15:13:40 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: ProtocolMsgSTOP.java,v 1.4 2005/01/30 15:13:40 ilyad Exp $"; }

    public String msgName() { return "STOP"; }
    public static final byte MSG_ID = 0x03;
    public final byte msgId() { return MSG_ID; }

    /** Construct from raw bytes
     *  @param msg The raw message bytes
     *  @param length The number of bytes in the message
     */
    public ProtocolMsgSTOP(byte [] msg, int length) throws ProtocolMalformedMsgException {
        super(msg, length, MSG_LENGTH);
    }

    /** Construct from values
     *  @param sessionId The session identifier
     */
    public ProtocolMsgSTOP(byte sessionId) {
        super(MSG_ID, sessionId, MSG_LENGTH);
    }

    public final byte [] createRawMsg() {
        byte [] msgData = new byte[MSG_LENGTH];
        setHeader(msgData);
        return msgData;
    }

    private final static int MSG_LENGTH = 8;
}