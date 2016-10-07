package com.xacct.CRANE.collector;

// $Id: ProtocolMsgFINAL_TMPL_DATA_ACK.java,v 1.4 2005/01/30 15:13:41 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/** The Java encapsulation of a CRANE protocol messages: "FINAL_TMPL_DATA_ACK".
 *  <xmp>
 *        0                   1                   2                   3
 *        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |  Version      |  MID=0x13     | Session ID    | Message Flags |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                         Message Length                        |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |   Config. ID  |                     Reserved                  |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  </xmp>
 *
 *  @version $Id: ProtocolMsgFINAL_TMPL_DATA_ACK.java,v 1.4 2005/01/30 15:13:41 ilyad Exp $
 */
public final class ProtocolMsgFINAL_TMPL_DATA_ACK extends ProtocolMsg
{
    /** $Id: ProtocolMsgFINAL_TMPL_DATA_ACK.java,v 1.4 2005/01/30 15:13:41 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: ProtocolMsgFINAL_TMPL_DATA_ACK.java,v 1.4 2005/01/30 15:13:41 ilyad Exp $"; }

    public String msgName() { return "FINAL_TMPL_DATA_ACK"; }
    public static final byte MSG_ID = 0x13;
    public final byte msgId() { return MSG_ID; }

    /// The configuration identifier
    public short configId;

    /** Construct from raw bytes
     *  @param msg The raw message bytes
     *  @param length The number of bytes in the message
     */
    public ProtocolMsgFINAL_TMPL_DATA_ACK(byte [] msg, int length) throws ProtocolMalformedMsgException {
        super(msg, length, MSG_LENGTH);
        try {
            this.configId = bigE.getUnsignedByte(msg, CONFIG_ID_OFFSET);
        } catch (IndexOutOfBoundsException e) { handleIndexOutOfBounds(e, msg, length); }
    }

    /** Construct from values.
     */
    public ProtocolMsgFINAL_TMPL_DATA_ACK(byte sessionId, short configId) {
        super(MSG_ID, sessionId, MSG_LENGTH);
        this.configId = configId;
    }

    public final byte [] createRawMsg() {
        byte [] msgData = new byte[MSG_LENGTH];
        setHeader(msgData);
        bigE.putByte(msgData,  CONFIG_ID_OFFSET, (byte)configId);
        bigE.putByte(msgData,  RESERVED1_OFFSET, (byte)0);
        bigE.putShort(msgData, RESERVED2_OFFSET, (short)0);
        return msgData;
    }

    public String toString() {
        return super.toString() +
            "[configId=" + this.configId + "]";
    }

    private final static int CONFIG_ID_OFFSET  =  8;
    private final static int RESERVED1_OFFSET  =  9;
    private final static int RESERVED2_OFFSET  = 10;

    private final static int MSG_LENGTH        = 12;
}
