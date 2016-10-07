package org.ipdr.protocol;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
//ANY KIND either expresses or implied.

/**
 * The class represents a FLOW_STOP message.
 * The message has the following structure:
 * <xmp>
 * struct FlowStop
 * {
 * 		struct IPDRStreamingHeader header;
 * 		short reasonCode; // values of 0-255 are reserved for standard
 * 						  // reason codes. Values &gt; 255 may be used for
 * 						  // vendor specific codes.
 * 						  // 0 = normal process termination
 * 						  // 1 = termination due to process error
 *		UTF8String reasonInfo;
 * }
 * </xmp>
 */
import org.ipdr.api.FlowStopInfo;
public class ProtocolMsgFLOW_STOP extends ProtocolStreamMsg implements FlowStopInfo
{
    /**
     * @see org.ipdr.protocol.ProtocolMsgIdentification#msgName()
     */
    public String msgName()
    {
        return "FLOW_STOP";
    }

    public static final byte MSG_ID = 0x03;

    /**
     * @see org.ipdr.protocol.ProtocolStreamMsg#msgId()
     */
    public final byte msgId()
    {
        return MSG_ID;
    }

    /**
     * Holds message reason code
     */
    private short reasonCode;

    /**
     * Holds message reason info
     */
    private String reasonInfo;

    /**
     * @see org.ipdr.protocol.ProtocolMsg#createRawMsg()
     */
    public final byte[] createRawMsg()
    {
        byte[] msgData = new byte[MSG_LENGTH + reasonInfo.length()];
        this.msgLength = MSG_LENGTH + reasonInfo.length();
        setHeader(msgData);
        bigE.putShort(msgData, REASONCODE_OFFSET, reasonCode);
        bigE.putStringLen(msgData, REASONINFO_LEN_OFFSET, REASONINFO_OFFSET,
                reasonInfo);
        return msgData;
    }

    /**
     * Initialize class parameters
     * 
     * @param sessionId
     * @param reasonCode
     * @param reasonInfo
     */
    public ProtocolMsgFLOW_STOP(byte sessionId, short reasonCode,
            String reasonInfo)
    {
        super(MSG_ID, sessionId, ProtocolStreamMsg.HDR_LENGTH);
        this.reasonCode = reasonCode;
        this.reasonInfo = reasonInfo;
    }

    /**
     * Sets sessionId parameter
     * @param sessionId
     */
    public void setSessionId(byte sessionId)
    {
        this.sessionId = sessionId;
    }

    private static final int MSG_LENGTH = HDR_LENGTH + 4;

    private static final int REASONCODE_OFFSET = HDR_LENGTH;

    private static final int REASONINFO_LEN_OFFSET = REASONCODE_OFFSET + 2;

    private static final int REASONINFO_OFFSET = REASONCODE_OFFSET + 4;

    public final static short FLOW_STOP_REASON_NORMAL = 0;

    public final static short FLOW_STOP_REASON_ERROR = 1;

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
     * @see java.lang.Object#toString()
     */
    public String toString()
    {
        return toStringBase() + "[reasoncode:" + getReasonCode()
                + ",reasoninfo:'" + getReasonInfo() + "']";
    }

}
