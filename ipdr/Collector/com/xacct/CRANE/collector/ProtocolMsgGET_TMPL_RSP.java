package com.xacct.CRANE.collector;

// $Id: ProtocolMsgGET_TMPL_RSP.java,v 1.4 2005/01/30 15:13:39 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.net.UnknownHostException;

/** The Java encapsulation of a CRANE protocol messages: "GET_TMPL_RSP".
 *  <pre>
 *        0                   1                   2                   3
 *        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |  Version      |  MID=0x17     | Session ID    | Message Flags |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                         Message Length                        |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |           Request ID          |       Number of Templates     |
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
 *  </pre>
 *
 *  @see ProtocolMsgTemplateBlock
 *  @see ProtocolMsgExtendedKeyBlock
 *
 *  @version $Id: ProtocolMsgGET_TMPL_RSP.java,v 1.4 2005/01/30 15:13:39 ilyad Exp $
 */
public final class ProtocolMsgGET_TMPL_RSP extends ProtocolMsg
{
    /** $Id: ProtocolMsgGET_TMPL_RSP.java,v 1.4 2005/01/30 15:13:39 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: ProtocolMsgGET_TMPL_RSP.java,v 1.4 2005/01/30 15:13:39 ilyad Exp $"; }

    public String msgName() { return "GET_TMPL_RSP"; }
    public static final byte MSG_ID = 0x17;
    public final byte msgId() { return MSG_ID; }

    /// The request id
    public int requestId;

    /// The templates
    public ProtocolMsgTemplateBlock templateBlock;

    /** Construct from raw bytes
     *  @param msg The raw message bytes
     *  @param length The number of bytes in the message
     */
   public ProtocolMsgGET_TMPL_RSP(byte [] msg, int length, LogHandler log)
        throws ProtocolMalformedMsgException {
        super(msg, length, MIN_MSG_LENGTH);
        msg = trimToLength(msg, this.msgLength);
        try {
            this.requestId  = bigE.getUnsignedShort(msg, REQUEST_ID_OFFSET);
            this.templateBlock =
                new ProtocolMsgTemplateBlock(msg, TEMPLATES_OFFSET,
                                             bigE.getUnsignedShort(msg, NUMBER_TEMPLATES_OFFSET),
                                                        new ProtocolMsgExtendedKeyBlock(), log);
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
            "[reqId=" + requestId +
            " tmpl#=" + templateBlock.toString() +
            "]";
    }

    public final String toStringLong() {
        return super.toStringLong() +
            "[reqId=" + requestId +
            " tmpl#=" + templateBlock.toStringLong() +
            "]";
    }

    private static final int REQUEST_ID_OFFSET       =  8;
    private static final int NUMBER_TEMPLATES_OFFSET = 10;
    private static final int TEMPLATES_OFFSET        = 12;

    private static final int MIN_MSG_LENGTH          = 12;
}
