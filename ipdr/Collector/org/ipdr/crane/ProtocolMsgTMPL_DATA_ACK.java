package org.ipdr.crane;

// $Id: ProtocolMsgTMPL_DATA_ACK.java,v 1.3 2005/01/30 15:13:45 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.net.UnknownHostException;

import org.ipdr.protocol.ProtocolMalformedMsgException;

/** The Java encapsulation of a CRANE protocol messages: "TMPL_DATA_ACK".
 *  <xmp>
 *        0                   1                   2                   3
 *        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |  Version      |  MID=0x11     | Session ID    | Message Flags |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                         Message Length                        |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |    Config. ID |    Reserved   |   Number of Template Changes  |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                                                               |
 *       ~                    Template Change Block                      ~
 *       |                                                               |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                                                               |
 *       ~                       ...       ...                           ~
 *       |                                                               |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                                                               |
 *       ~                    Template Change Block                      ~
 *       |                                                               |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  </xmp>
 *
 *  @version $Id: ProtocolMsgTMPL_DATA_ACK.java,v 1.3 2005/01/30 15:13:45 ilyad Exp $
 */
public final class ProtocolMsgTMPL_DATA_ACK extends ProtocolMsg
{
    /** $Id: ProtocolMsgTMPL_DATA_ACK.java,v 1.3 2005/01/30 15:13:45 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: ProtocolMsgTMPL_DATA_ACK.java,v 1.3 2005/01/30 15:13:45 ilyad Exp $"; }

    public String msgName() { return "TMPL_DATA_ACK"; }
    public static final byte MSG_ID = 0x11;
    public final byte msgId() { return MSG_ID; }

    /** Construct from raw bytes
     *  XXX unimplemented
     *  @param msg The raw message bytes
     *  @param length The number of bytes in the message
     */
    public ProtocolMsgTMPL_DATA_ACK(byte [] msg, int length) throws ProtocolMalformedMsgException {
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

//     private static final String msgToStringTMPL_DATA_ACK(ProtocolDataBigEndian bigE, byte [] msg) {
//         return "configId=" + bigE.getUnsignedByte(msg, 8) +
//             "numberOfTemplateChanges=" + bigE.getUnsignedShort(msg, 10);
//     }

    private static final int MIN_MSG_LENGTH = 12;
}
