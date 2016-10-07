package org.ipdr.crane;

import org.ipdr.protocol.ProtocolMalformedMsgException;

// $Id: ProtocolMsgSTART_NEGOTIATE_ACK.java,v 1.3 2005/01/30 15:13:46 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/** The Java encapsulation of a CRANE protocol message: "START_NEGOTIATE_ACK".
 *  <xmp>
 *        0                   1                   2                   3
 *        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |  Version      |  MID=0x19     | Session ID    | Message Flags |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                         Message Length                        |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  </xmp>
 *
 *  @version $Id: ProtocolMsgSTART_NEGOTIATE_ACK.java,v 1.3 2005/01/30 15:13:46 ilyad Exp $
 */
public final class ProtocolMsgSTART_NEGOTIATE_ACK extends ProtocolMsg
{
    /** $Id: ProtocolMsgSTART_NEGOTIATE_ACK.java,v 1.3 2005/01/30 15:13:46 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: ProtocolMsgSTART_NEGOTIATE_ACK.java,v 1.3 2005/01/30 15:13:46 ilyad Exp $"; }

    public String msgName() { return "START_NEGOTIATE_ACK"; }
    public static final byte MSG_ID = 0x19;
    public final byte msgId() { return MSG_ID; }

    /** Construct from raw bytes
     *  @param msg The raw message bytes
     *  @param length The number of bytes in the message
     */
    public ProtocolMsgSTART_NEGOTIATE_ACK(byte [] msg, int length) throws ProtocolMalformedMsgException {
        super(msg, length, MSG_LENGTH);
    }

    /** Construct from values
     *  @param sessionId The session identifier
     */
    public ProtocolMsgSTART_NEGOTIATE_ACK(byte sessionId) {
        super(MSG_ID, sessionId, MSG_LENGTH);
    }

    public final byte [] createRawMsg() {
        byte [] msgData = new byte[MSG_LENGTH];
        setHeader(msgData);
        return msgData;
    }

    private final static int MSG_LENGTH = 8;
}
