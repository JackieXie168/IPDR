package org.ipdr.protocol;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
//ANY KIND either expresses or implied.

/**
 * The class represents a SESSION_STOP message.
 * The message structure is as follows:
 * <xmp>
 * struct SessionStop
 * {
 * 		struct IPDRStreamingHeader header;
 * 		short reasonCode; 		// values of 0-255 are reserved for standard
 * 								// reason codes. Values > 255 may be used for
 * 								// vendor specific codes
 * 								// 0 = end of data for Session
 * 								// 1 = handing off to higher priority collector
 * 								// 2 = exporter terminating
 * 								// 3 = congestion detected
 * 								// 4 = renegotiation is required
 * 								// 5 = start negotiation acknowledge
 * 								// 6 = close current document
 * 		UTF8String reasonInfo;
 * }
 * </xmp>
 *
 */
import org.ipdr.api.SessionStopInfo;
public class ProtocolMsgSESSION_STOP extends ProtocolStreamMsg implements SessionStopInfo
{
    /**
     * Holds the reason code
     */
    protected short reasonCode;

    /**
     * Holds the description of session stop reason
     */
    protected String reasonInfo;

    /**
     * @see org.ipdr.protocol.ProtocolMsgIdentification#msgName()
     */
    public String msgName()
    {
        return "SESSION_STOP";
    }

    public static final byte MSG_ID = 0x09;

    /**
     * @see org.ipdr.protocol.ProtocolStreamMsg#msgId()
     */
    public final byte msgId()
    {
        return MSG_ID;
    }

    /**
     * @see org.ipdr.protocol.ProtocolMsg#createRawMsg()
     */
    protected byte[] createRawMsg()
    {
        return null;
    }

    /**
     * Creates the message from raw bytes
     * @param message Raw bytes
     * @param length Buffer length
     * @throws ProtocolMalformedMsgException
     */
    public ProtocolMsgSESSION_STOP(byte[] message, int length)
            throws ProtocolMalformedMsgException
    {
        super(message, length, MSG_LENGTH);
        this.reasonCode = bigE.getShort(message, REASONCODE_OFFSET);
        this.reasonInfo = bigE.getStringLen(message, REASONINFO_OFFSET,
                REASONINFO_OFFSET + 4);
    }

    protected static final int REASONCODE_OFFSET = 8;

    protected static final int REASONINFO_OFFSET = 10;

    protected static final int MSG_LENGTH = 14;

    /**
     * @see java.lang.Object#toString()
     */
    public String toString()
    {
        return toStringBase() + "[" + "reason code:" + getReasonCode() + ","
                + "reason info:'" + getReasonInfo() + "']";
    }

    /**
     * Returns the reasonCode.
     *
     * @return short
     */
    public short getReasonCode()
    {
        return reasonCode;
    }

    /**
     * Returns the reasonInfo.
     *
     * @return String
     */
    public String getReasonInfo()
    {
        return reasonInfo;
    }
    
    /**
     * @see org.ipdr.protocol.ProtocolStreamMsg#getSessionId()
     */
    public byte getSessionId()
    {
    	return this.sessionId;
    }

}
