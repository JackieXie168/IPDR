package org.ipdr.crane;

// $Id: ProtocolMsgSTART_ACK.java,v 1.4 2005/01/30 15:13:46 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.util.Date;

import org.ipdr.protocol.ProtocolMalformedMsgException;

import com.xacct.CRANE.collector.CollectorUtil;

/** The Java encapsulation of a CRANE protocol messages: "START_ACK".
 *  <xmp>
 *        0                   1                   2                   3
 *        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |  Version      |  MID=0x02     | Session ID    | Message Flags |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                         Message Length                        |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                        Client Boot Time                       |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  </xmp>
 *
 *  @version $Id: ProtocolMsgSTART_ACK.java,v 1.4 2005/01/30 15:13:46 ilyad Exp $
 */
public final class ProtocolMsgSTART_ACK extends ProtocolMsg
{
    /** $Id: ProtocolMsgSTART_ACK.java,v 1.4 2005/01/30 15:13:46 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: ProtocolMsgSTART_ACK.java,v 1.4 2005/01/30 15:13:46 ilyad Exp $"; }

    public String msgName() { return "START_ACK"; }
    public static final byte MSG_ID = 0x02;
    public final byte msgId() { return MSG_ID; }

    /// The transmitter's (client's) boot time
    Date bootTime;

    /** Construct from raw bytes
     *  @param msg The raw message bytes
     *  @param length The number of bytes in the message
     */
    public ProtocolMsgSTART_ACK(byte [] msg, int length) throws ProtocolMalformedMsgException {
        super(msg, length, MSG_LENGTH);
        try {
            this.bootTime = bigE.getDate(msg, BOOT_TIME_OFFSET);
        } catch (IndexOutOfBoundsException e) { handleIndexOutOfBounds(e, msg, length); }
    }

    /** Construct from values.
     */
    public ProtocolMsgSTART_ACK(byte sessionId, Date bootTime) {
        super(MSG_ID, sessionId, MSG_LENGTH);
        this.bootTime = bootTime;
    }

    public final byte [] createRawMsg() {
        byte [] msgData = new byte[MSG_LENGTH];
        setHeader(msgData);
        bigE.putDate(msgData, BOOT_TIME_OFFSET, bootTime);
        return msgData;
    }

    public String toString() {
        return super.toString() +
            "[bootTime=" + CollectorUtil.dateFormat(bootTime) + "]";
    }

    public final static int BOOT_TIME_OFFSET =  8;

    private final static int MSG_LENGTH      = 12;
    /**
     * @return Returns the bootTime.
     */
    public Date getBootTime()
    {
        return bootTime;
    }
}
