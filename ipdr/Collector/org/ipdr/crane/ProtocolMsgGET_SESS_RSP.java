package org.ipdr.crane;

// $Id: ProtocolMsgGET_SESS_RSP.java,v 1.3 2005/01/30 15:13:45 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.net.UnknownHostException;
import java.util.ArrayList;

import org.ipdr.protocol.ProtocolMalformedMsgException;

import com.xacct.CRANE.collector.SessionConfig;

/** The Java encapsulation of a CRANE protocol messages: "GET_SESS_RSP".
 *  <xmp>
 *        0                   1                   2                   3
 *        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |  Version      |  MID=0x15     | Session ID    | Message Flags |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                         Message Length                        |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |           Request ID          |       Number of Sessions      |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |    Vendor String Length       |           Reserved            |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-|
 *       |                                                               |
 *       ~                       Vendor String                           ~
 *       |                                                               |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                                                               |
 *       ~                         Session Block                         ~
 *       |                                                               |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                                                               |
 *       ~                       ...       ...                           ~
 *       |                                                               |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                                                               |
 *       ~                         Session Block                         ~
 *       |                                                               |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *      Session Block
 *
 *       0                   1                   2                   3
 *       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *      | Session ID    |   Reserved    |      Session Name Length      |
 *      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *      |  Session Description Length   |             Reserved          |
 *      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *      |                                                               |
 *      ~                          Session Name                         ~
 *      |                                                               |
 *      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *      |                                                               |
 *      ~                       Session Description                     ~
 *      |                                                               |
 *      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  </xmp>
 *
 *  @version $Id: ProtocolMsgGET_SESS_RSP.java,v 1.3 2005/01/30 15:13:45 ilyad Exp $
 */
public final class ProtocolMsgGET_SESS_RSP extends ProtocolMsg
{
    /** $Id: ProtocolMsgGET_SESS_RSP.java,v 1.3 2005/01/30 15:13:45 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: ProtocolMsgGET_SESS_RSP.java,v 1.3 2005/01/30 15:13:45 ilyad Exp $"; }

    public String msgName() { return "GET_SESS_RSP"; }
    public static final byte MSG_ID = 0x15;
    public final byte msgId() { return MSG_ID; }

    /// The request id
    public int requestId;

    /// The vendor string
    public String vendorString;

    /// The sessions
    ArrayList sessionConfigList;

    /** Construct from raw bytes
     *  @param msg The raw message bytes
     *  @param length The number of bytes in the message
     */
    public ProtocolMsgGET_SESS_RSP(byte [] msg, int length)
        throws ProtocolMalformedMsgException {
        super(msg, length, MIN_MSG_LENGTH);
        msg = trimToLength(msg, this.msgLength);
        try {
            int numSessions = bigE.getUnsignedShort(msg, NUMBER_SESSIONS_OFFSET);
            this.requestId = bigE.getUnsignedShort(msg, REQUEST_ID_OFFSET);
            this.sessionConfigList = new ArrayList(numSessions);
            int vendorStringLength = bigE.getUnsignedShort(msg, VENDOR_STRING_LENGTH_OFFSET);
            this.vendorString = bigE.getStringWithLen(msg, vendorStringLength, VENDOR_STRING_OFFSET);

            int offset = ProtocolMsg.round32(VENDOR_STRING_OFFSET + vendorStringLength);
            for (int i = 0; i < numSessions; i++) {
                offset = getSessionBlock(msg, offset, i);
            }
        } catch (IndexOutOfBoundsException e) { handleIndexOutOfBounds(e, msg, length); }
    }

    private final int getSessionBlock(byte [] msg, int offset, int i) {
        short sessionId = bigE.getUnsignedByte(msg, offset + SESSION_ID_OFFSET);
        int sessionNameLength = bigE.getUnsignedShort(msg, offset + SESSION_NAME_LENGTH_OFFSET);
        int sessionDescrLength = bigE.getUnsignedShort(msg, offset + SESSION_DESCR_LENGTH_OFFSET);
        String sessionName = bigE.getStringWithLen(msg, sessionNameLength, offset + SESSION_NAME_OFFSET);
        offset = round32(offset + SESSION_NAME_OFFSET + sessionNameLength);
        String sessionDescr = bigE.getStringWithLen(msg, sessionDescrLength, offset);
        offset = round32(offset + sessionDescrLength);
        this.sessionConfigList.add(new SessionConfig(sessionId, sessionName, sessionDescr, null));
        return offset;
    }

    // XXX missing construct from values, createRawMsg

    // XXX unimplemented
    protected byte [] createRawMsg() throws UnknownHostException {
        byte [] result = new byte[HDR_LENGTH];
        setHeader(result);
        unimplemented(this, "createRawMsg"); // die if not over-ridden
        return result;
    }

    public String toString() {
        return super.toString() +
            "[reqId=" + this.requestId +
            " vendor(" + vendorString + ")" +
            " session#=" + sessionConfigList.size() + "]";
    }

    public String toStringLong() {
        return this.toString() +
            "\n";
//        + IPDRUtilities.toStringLong(sessionConfigList, "    ");
    }

    private static final int MIN_MSG_LENGTH = 12;

    private final static int REQUEST_ID_OFFSET           =  8;
    private final static int NUMBER_SESSIONS_OFFSET      = 10;
    private final static int VENDOR_STRING_LENGTH_OFFSET = 12;
    private final static int RESERVED1_OFFSET            = 14;
    private final static int VENDOR_STRING_OFFSET        = 16;

    // The following are relative to a session block:

    private final static int SESSION_ID_OFFSET           =  0;
    private final static int SESSION_RESERVED1_OFFSET    =  1;
    private final static int SESSION_NAME_LENGTH_OFFSET  =  2;
    private final static int SESSION_DESCR_LENGTH_OFFSET =  4;
    private final static int SESSION_RESERVED2_OFFSET    =  6;
    private final static int SESSION_NAME_OFFSET         =  8;
}
