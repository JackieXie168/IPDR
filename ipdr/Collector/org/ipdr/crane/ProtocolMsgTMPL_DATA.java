package org.ipdr.crane;

// $Id: ProtocolMsgTMPL_DATA.java,v 1.4 2005/04/08 12:48:14 guyg Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.net.UnknownHostException;

import org.ipdr.protocol.ProtocolMalformedMsgException;

/** The Java encapsulation of a CRANE protocol messages: "TMPL_DATA".
 *
 *  <xmp>
 *        0                   1                   2                   3
 *        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |  Version      |  MID=0x10     | Session ID    | Message Flags |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                         Message Length                        |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |   Config ID   |  Flags      |E|       Number of Templates     |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                                                               |
 *       ~                       Template Block                          ~
 *       |                                                               |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                                                               |
 *       ~                       ...       ...                           ~
 *       |                                                               |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                                                               |
 *       ~                       Template Block                          ~
 *       |                                                               |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  </xmp>
 *
 *  @see ProtocolMsgTemplateBlock
 *  @see ProtocolMsgKeyBlock
 *
 *  @version $Id: ProtocolMsgTMPL_DATA.java,v 1.4 2005/04/08 12:48:14 guyg Exp $
 */
public class ProtocolMsgTMPL_DATA extends ProtocolMsg
{
    /** $Id: ProtocolMsgTMPL_DATA.java,v 1.4 2005/04/08 12:48:14 guyg Exp $ */
    public static String versionStr() { return "@(#) $Id: ProtocolMsgTMPL_DATA.java,v 1.4 2005/04/08 12:48:14 guyg Exp $"; }

    public String msgName() { return "TMPL_DATA"; }
    public static final byte MSG_ID = 0x10;
    public byte msgId() { return MSG_ID; }

    /// The templates
    public ProtocolMsgTemplateBlock templateBlock;

    /// The configuration id
    public short configId;

    /// Is the data big endian (network byte order) or little endian (DEC/Intel order?
    public boolean bigEndian;

    /** Construct from raw bytes
     *  @param msg The raw message bytes
     *  @param length The number of bytes in the message
     */
    public ProtocolMsgTMPL_DATA(byte [] msg, int length)
        throws ProtocolMalformedMsgException {
        super(msg, length, MIN_MSG_LENGTH);
        msg = trimToLength(msg, this.msgLength);
        try {
            this.configId = bigE.getUnsignedByte(msg, CONFIG_ID_OFFSET);
            byte flags    = bigE.getByte(        msg, FLAGS_OFFSET);
            this.bigEndian = (flags & FLAG_BIG_ENDIAN) != 0;
            this.templateBlock =
                new ProtocolMsgTemplateBlock(msg, TEMPLATES_OFFSET,
                                             bigE.getUnsignedShort(msg, NUMBER_TEMPLATES_OFFSET),
                                             new ProtocolMsgKeyBlock());
        } catch (IndexOutOfBoundsException e) { handleIndexOutOfBounds(e, msg, length); }
    }

    // XXX unimplemented
    protected byte [] createRawMsg() throws UnknownHostException {
        byte [] result = new byte[HDR_LENGTH];
        setHeader(result);
        unimplemented(this, "createRawMsg"); // die if not over-ridden
        return result;
    }

    public final String toString() {
        return super.toString() +
            "[configId=" + configId +
            " " + (bigEndian ? "bigEndian" : "littleEndian") +
            " " + templateBlock.toString() +
            "]";
    }

    public final String toStringLong() {
        return super.toStringLong() +
            "[configId=" + configId +
            " " + (bigEndian ? "bigEndian" : "littleEndian") +
            " " + templateBlock.toStringLong() +
            "]";
    }

    private static final int CONFIG_ID_OFFSET        =  8;
    private static final int FLAGS_OFFSET            =  9;
    private static final int NUMBER_TEMPLATES_OFFSET = 10;
    private static final int TEMPLATES_OFFSET        = 12;

    private static final int MIN_MSG_LENGTH          = 12;

    private static final int FLAG_BIG_ENDIAN         = 0x01;
}
