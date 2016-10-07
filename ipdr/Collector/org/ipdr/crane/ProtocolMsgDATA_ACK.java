package org.ipdr.crane;

import org.ipdr.protocol.ProtocolMalformedMsgException;

// $Id: ProtocolMsgDATA_ACK.java,v 1.3 2005/01/30 15:13:45 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/** The Java encapsulation of a CRANE protocol messages: "DATA_ACK".
 *  <xmp>
 *        0                   1                   2                   3
 *        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |  Version      |  MID=0x21     | Session ID    | Message Flags |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                         Message Length                        |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                      Data Sequence Number                     |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |   Config. ID  |                  Reserved                     |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  </xmp>
 *
 *  @version $Id: ProtocolMsgDATA_ACK.java,v 1.3 2005/01/30 15:13:45 ilyad Exp $
 */
public final class ProtocolMsgDATA_ACK extends ProtocolMsg
{
    /** $Id: ProtocolMsgDATA_ACK.java,v 1.3 2005/01/30 15:13:45 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: ProtocolMsgDATA_ACK.java,v 1.3 2005/01/30 15:13:45 ilyad Exp $"; }

    public String msgName() { return "DATA_ACK"; }
    public static final byte MSG_ID = 0x21;
    public final byte msgId() { return MSG_ID; }

    // The Data Sequence Number
    public long dataSeqNo;

    /// The configuration id
    public short configId;

    /** Construct from raw bytes
     *  @param msg The raw message bytes
     *  @param length The number of bytes in the message
     */
    public ProtocolMsgDATA_ACK(byte [] msg, int length) throws ProtocolMalformedMsgException {
        super(msg, length, MSG_LENGTH);

        try {
            this.dataSeqNo = bigE.getUnsignedInt( msg, DSN_OFFSET);
            this.configId  = bigE.getUnsignedByte(msg, CONFIG_ID_OFFSET);
        } catch (IndexOutOfBoundsException e) { handleIndexOutOfBounds(e, msg, length); }
    }

    /** Construct from values.
     */
    public ProtocolMsgDATA_ACK(byte sessionId, byte configId, long dataSeqNo) {
        super(MSG_ID, sessionId, MSG_LENGTH);
        this.configId  = configId;
        this.dataSeqNo = dataSeqNo;
    }

    public final byte [] createRawMsg() {
        byte [] msgData = new byte[MSG_LENGTH];
        setHeader(msgData);
        bigE.putUnsignedInt( msgData, DSN_OFFSET,       this.dataSeqNo);
        bigE.putUnsignedByte(msgData, CONFIG_ID_OFFSET, this.configId);
        bigE.putByte(        msgData, RESERVED1_OFFSET, (byte)0);
        bigE.putShort(       msgData, RESERVED2_OFFSET, (short)0);
        return msgData;
    }

    public String toString() {
        return super.toString() +
            "[configID=" + configId +
            " DSN=" + dataSeqNo + "]";
    }

    private final static int DSN_OFFSET         =  8;
    private final static int CONFIG_ID_OFFSET   = 12;
    private final static int RESERVED1_OFFSET   = 13;
    private final static int RESERVED2_OFFSET   = 14;

    private final static int MSG_LENGTH         = 16;
}
