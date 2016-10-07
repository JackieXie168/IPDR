package com.xacct.CRANE.collector;

// $Id: ProtocolMsgSTATUS_RSP.java,v 1.4 2005/01/30 15:13:43 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.net.UnknownHostException;

/** The Java encapsulation of a CRANE protocol messages: "STATUS_RSP".
 *  <xmp>
 *        0                   1                   2                   3
 *        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |  Version      |  MID=0x31     | Session ID    | Message Flags |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                         Message Length                        |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |        Template ID            |  Reserved     |Config. ID     |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                         Record Length                         |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                                                               |
 *       ~                         Record Data                           ~
 *       |                                                               |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  </xmp>
 *
 *  @version $Id: ProtocolMsgSTATUS_RSP.java,v 1.4 2005/01/30 15:13:43 ilyad Exp $
 */
public final class ProtocolMsgSTATUS_RSP extends ProtocolMsg
{
    /** $Id: ProtocolMsgSTATUS_RSP.java,v 1.4 2005/01/30 15:13:43 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: ProtocolMsgSTATUS_RSP.java,v 1.4 2005/01/30 15:13:43 ilyad Exp $"; }

    public String msgName() { return "STATUS_RSP"; }
    public static final byte MSG_ID = 0x31;
    public final byte msgId() { return MSG_ID; }

    /** Construct from raw bytes
     *  XXX unimplemented
     *  @param msg The raw message bytes
     *  @param length The number of bytes in the message
     */
    public ProtocolMsgSTATUS_RSP(byte [] msg, int length) throws ProtocolMalformedMsgException {
        super(msg, length, MIN_MSG_LENGTH);

        try {
            unimplemented(this, "<init>");
        } catch (IndexOutOfBoundsException e) { handleIndexOutOfBounds(e, msg, length); }
    }

    // XXX unimplemented
    protected byte [] createRawMsg() throws UnknownHostException {
        byte [] result = new byte[HDR_LENGTH];
        setHeader(result);
        unimplemented(this, "createRawMsg"); // die if not over-ridden
        return result;
    }

    private static final int MIN_MSG_LENGTH = 16;
}
