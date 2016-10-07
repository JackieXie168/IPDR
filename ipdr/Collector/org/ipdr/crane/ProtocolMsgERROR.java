package org.ipdr.crane;

// $Id: ProtocolMsgERROR.java,v 1.3 2005/01/30 15:13:46 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.util.Date;

import org.ipdr.protocol.ProtocolMalformedMsgException;
import org.ipdr.utility.IPDRUtilities;

/** The Java encapsulation of a CRANE protocol messages: "ERROR".
 *  <xmp>
 *        0                   1                   2                   3
 *        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |  Version      |  MID=0x23     | Session ID    | Message Flags |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                         Message Length                        |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                           Timestamp                           |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |         Error Code            |      Description Length       |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                                                               |
 *       ~                          Description                          ~
 *       |                                                               |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  </xmp>
 *
 *  @version $Id: ProtocolMsgERROR.java,v 1.3 2005/01/30 15:13:46 ilyad Exp $
 */
public final class ProtocolMsgERROR extends ProtocolMsg
{
    /** $Id: ProtocolMsgERROR.java,v 1.3 2005/01/30 15:13:46 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: ProtocolMsgERROR.java,v 1.3 2005/01/30 15:13:46 ilyad Exp $"; }

    public String msgName() { return "ERROR"; }
    public static final byte MSG_ID = 0x23;
    public final byte msgId() { return MSG_ID; }

    /// The timestamp
    public Date timestamp;

    /// The error code
    public int errorCode;

    /// The description string
    String description;

    /** Construct from raw bytes
     *  @param msg The raw message bytes
     *  @param length The number of bytes in the message
     */
    public ProtocolMsgERROR(byte [] msg, int length) throws ProtocolMalformedMsgException {
        super(msg, length, MIN_MSG_LENGTH);
        try {
            this.timestamp = bigE.getDate(         msg, TIMESTAMP_OFFSET);
            this.errorCode = bigE.getUnsignedShort(msg, ERROR_CODE_OFFSET);
            this.description = bigE.getStringShortLen(msg, DESCR_LENGTH_OFFSET, DESCR_OFFSET);
        } catch (IndexOutOfBoundsException e) { handleIndexOutOfBounds(e, msg, length); }
    }

    /** Construct from values.
     */
    public ProtocolMsgERROR(byte sessionId, Date timestamp,
                            int errorCode, String description) {
        super(MSG_ID, sessionId, MIN_MSG_LENGTH + description.length());
        this.timestamp   = timestamp;
        this.errorCode   = errorCode;
        this.description = description;
    }

    public final byte [] createRawMsg() {
        byte [] msgData = new byte[MIN_MSG_LENGTH + description.length()];
        setHeader(msgData);
        bigE.putDate(          msgData, TIMESTAMP_OFFSET, timestamp);
        bigE.putUnsignedShort( msgData, ERROR_CODE_OFFSET, errorCode);
        bigE.putStringShortLen(msgData, DESCR_LENGTH_OFFSET, DESCR_OFFSET, description);
        return msgData;
    }

    public String toString() {
        return super.toString() +
            "[timestamp=" + IPDRUtilities.dateFormat(timestamp) +
            " errCode=" + errorCode +
            " descr: " + description +
            "]";
    }

    public String toStringLong() {
        return super.toStringLong() +
            "[timestamp=" + IPDRUtilities.dateFormat(timestamp) +
            " errCode=" + errorCode +
            " descr: " + description +
            "]";
    }

    private final static int TIMESTAMP_OFFSET    =  8;
    private final static int ERROR_CODE_OFFSET   = 12;
    private final static int DESCR_LENGTH_OFFSET = 14;
    private final static int DESCR_OFFSET        = 16;

    private final static int MIN_MSG_LENGTH      = 16;
}
