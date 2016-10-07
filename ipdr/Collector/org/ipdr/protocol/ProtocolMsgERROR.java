package org.ipdr.protocol;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.util.Date;

/**
 * The class represents an ERROR message. The message has the following
 * structure:
 * <xmp>
 * struct Error 
 * { 
 * 		struct IPDRStreamingHeader header; 
 * 		int timeStamp; // time of error (in seconds from epoch time)  
 * 		short errorCode; // The error code field consists of two parts: 
 * 						 // Session oriented flag: it is a one bit flag.
 *  					 // It is the MSB of the errorCode field. It 
 * 						 // indicates whether the error is specific for 
 * 						 // the session (=1) or it is a general error and
 * 						 // thus it is not specific to the session (=0). 
 * 						 // The code ID: The rest 15 LSBs of the 
 * 						 // errorCode field, specifies the error code ID 
 * 						 //(0 - 32767). Values of 0-255 are reserved for
 * 						 // standard error codes. 
 * 						 // 0 = keepalive expired 
 * 						 // 1 = Message invalid for capabilities 
 * 						 // 2 = Message invalid for state 
 * 						 // 3 = Message decode error 
 * 						 // 4 = process terminating 
 *						 // Values > 255 may be used for vendor specific 
 *						 // error codes  
 *  	UTF8String description; 
 * }
 * </xmp>
 */

public class ProtocolMsgERROR extends ProtocolStreamMsg
{
    /**
     * @see org.ipdr.protocol.ProtocolMsgIdentification#msgName()
     */
    public String msgName()
    {
        return "ERROR";
    }

    public static final byte MSG_ID = 0x23;

    /**
     * @see org.ipdr.protocol.ProtocolStreamMsg#msgId()
     */
    public final byte msgId()
    {
        return MSG_ID;
    }

    /**
     * Holds error time stamp
     */
    protected int timeStamp;

    
    /**
     * Holds error code
     */
    protected short errorCode;

    /**
     * Holds error description
     */
    protected String description;

    /**
     * @see org.ipdr.protocol.ProtocolMsg#createRawMsg()
     */
    public final byte[] createRawMsg()
    {
        this.msgLength = ProtocolMsgERROR.MSG_LENGTH + (description == null? 0: description.length());
        byte[] msgData = new byte[msgLength];
        setHeader(msgData);
        bigE.putInt(msgData, TIMESTAMP_OFFSET, timeStamp);
        bigE.putShort(msgData, ERROR_CODE_OFFSET, errorCode);
        bigE.putStringLen(msgData, DESCRIPTION_LEN_OFFSET, description);
        return msgData;
    }

    /**
     * Constructs message from values
     * 
     * @param sessionId session id for session-specific error
     * @param errorCode full error code
     * @param description textual description
     */
    public ProtocolMsgERROR(byte sessionId, short errorCode, String description)
    {
        super(MSG_ID, sessionId, ProtocolMsgERROR.MSG_LENGTH);
        this.timeStamp = (int) ((new Date()).getTime() / 1000);
        this.errorCode = errorCode;
        this.description = description;
    }

    /**
     * Constructs message from raw bytes
     * @param msg Raw bytes
     * @param length Buffer length
     * @throws ProtocolMalformedMsgException
     */
    public ProtocolMsgERROR(byte[] msg, int length)
            throws ProtocolMalformedMsgException
    {
        super(msg, length, MSG_LENGTH);
        this.timeStamp = bigE.getInt(msg, TIMESTAMP_OFFSET);
        this.errorCode = bigE.getShort(msg, ERROR_CODE_OFFSET);
        this.description = bigE.getStringLen(msg, DESCRIPTION_LEN_OFFSET,
                DESCRIPTION_OFFSET);
    }

    private final static int TIMESTAMP_OFFSET = 8;

    private final static int ERROR_CODE_OFFSET = 12;

    private final static int DESCRIPTION_LEN_OFFSET = 14;

    private final static int DESCRIPTION_OFFSET = 18;

    private final static int MSG_LENGTH = 18;

    protected final short ERROR_LEVEL_MASK = (short) (0x1 << 15);

    /**
     * Returns true if the error is session-specific
     * @return boolean
     */
    public boolean getSessionSpecific()
    {
        return (errorCode & ERROR_LEVEL_MASK) != 0;
    }

    /**
     * Returns the errorCode.
     * 
     * @return short
     */
    public short getErrorCode()
    {
        return (short) (errorCode & ~ERROR_LEVEL_MASK);
    }

    /**
     * Sets the errorCode.
     * 
     * @param errorCode
     *            The errorCode to set
     */
    public void setErrorCode(short errorCode, boolean sessionSpecific)
    {
        this.errorCode = errorCode;
        if (sessionSpecific) this.errorCode |= ERROR_LEVEL_MASK;
    }

    /**
     * @see java.lang.Object#toString()
     */
    public String toString()
    {
        return toStringBase() + "[" + "timestamp:" + getTimeStamp() + ","
                + "is session:" + getSessionSpecific() + "," + "code:"
                + getErrorCode() + "," + "description:'" + getDescription()
                + "']";
    }

    /**
     * Returns the timeStamp.
     * 
     * @return int
     */
    public int getTimeStamp()
    {
        return timeStamp;
    }

    /**
     * Returns the description.
     * 
     * @return String
     */
    public String getDescription()
    {
        return description;
    }

}