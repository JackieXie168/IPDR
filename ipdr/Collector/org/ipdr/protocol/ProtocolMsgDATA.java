package org.ipdr.protocol;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.


/**
 * The class represents a DATA message.
 * The message has the following structure:
 * <xmp>
 * struct Data 
 * { 
 * 		struct IPDRStreamingHeader header; 
 * 		short templateId; // a Template ID relative to the Session 
 * 						  // defined  in the header. The Fields in 
 * 						  // this Template were reported at the 
 * 						  // beginning of the Session via 
 * 						  //TEMPLATE DATA Messages 
 * 		short configId;   // (see above) 
 * 		char flags;
 * 		long sequenceNum; // Session relative sequence number of this record 
 * 		opaque dataRecord<>; // XDR encoded Fields based defined by templateId // 
 * }
 */
public final class ProtocolMsgDATA extends ProtocolStreamMsg
{
    /**
     * @see org.ipdr.protocol.ProtocolMsgIdentification#msgName()
     */
    public String msgName()
    {
        return "DATA";
    }

    public static final byte MSG_ID = 0x20;

    /**
     * @see org.ipdr.protocol.ProtocolStreamMsg#msgId()
     */
    public final byte msgId()
    {
        return MSG_ID;
    }

    /**
    * The template id
    */
    protected short templateId;

    /**
    * The configuration id
    */
    protected short configId;

    /**
     * The "duplicate" flag
     */
    protected boolean duplicateFlag;

    /**
     * The Data Sequence Number ("DSN")
     */
    protected long dataSeqNo;

    /**
     * The raw data
     */
    protected byte[] rawData;

    /**
     * Where the actual data starts in rawData (i.e., initial offset
     * to use when pulling dataout of rawData)
     */
    protected int rawDataOffset;

    /**
     * Construct a basic object, for reuse (with the <code>set</code> method).
     * 
     * @param msg - The raw message: must be at least 16 bytes but otherwise not used.
     */
    public ProtocolMsgDATA(byte[] msg) throws ProtocolMalformedMsgException
    {
        super(msg, MIN_MSG_LENGTH, MIN_MSG_LENGTH);
    }

    /**
     * Construct from raw bytes. Code has been removed that copies the message
     * buffer, so beware: if you re-use "msg", you'll corrupt what's here. You
     * can be safe by doing: byte [] newRawData = new byte[rawData.length];
     * System.arraycopy(rawData, 0, newRawData, 0, rawData.length); rawData =
     * newRawData;
     * 
     * @param msg - The raw message bytes
     * @param length -  The number of bytes in the message
     */
    public ProtocolMsgDATA(byte[] msg, int length)
            throws ProtocolMalformedMsgException
    {
        super(msg, length, MIN_MSG_LENGTH);
        set2(msg, length);
    }

    /**
     * Varify the length of the message and initializes class parameters
     * 
     * @param msg - The raw message bytes
     * @param length - The number of bytes in the message
     * @return this
     * @throws ProtocolMalformedMsgException
     */
    public final ProtocolMsgDATA set(byte[] msg, int length)
            throws ProtocolMalformedMsgException
    {
        super.set(msg, length, MIN_MSG_LENGTH);
        return set2(msg, length);
    }

    /**
     * Initialize class parameters
     * 
     * @param msg -	The raw message bytes
     * @param length - The number of bytes in the message
     * @return this
     * @throws ProtocolMalformedMsgException
     */
    private final ProtocolMsgDATA set2(byte[] msg, int length)
            throws ProtocolMalformedMsgException
    {
        // msg = trimToLength(msg, this.msgLength);
        try
        {
            this.templateId = bigE.getShort(msg, TEMPLATE_ID_OFFSET);
            this.configId = bigE.getShort(msg, CONFIG_ID_OFFSET);
            byte flags = bigE.getByte(msg, FLAGS_OFFSET);
            this.dataSeqNo = bigE.getLong(msg, DSN_OFFSET);
            this.duplicateFlag = (flags & DUPLICATE_FLAG) != 0;
            this.rawData = msg;
            this.rawDataOffset = DATA_OFFSET;
        }
        catch (IndexOutOfBoundsException e)
        {
            handleIndexOutOfBounds(e, msg, length);
        }
        return this;
    }

    /**
     * Construct from values. Makes a copy of the data (inefficient)
     * 
     * @param sessionId
     * @param templateId
     * @param configId
     * @param duplicateFlag
     * @param startDSNflag
     * @param dataSeqNo
     * @param rawData
     */
    public ProtocolMsgDATA(byte sessionId, byte templateId, byte configId,
            boolean duplicateFlag, boolean startDSNflag, long dataSeqNo,
            byte[] rawData)
    {
        super(MSG_ID, sessionId, rawData.length + MIN_MSG_LENGTH);
        this.templateId = templateId;
        this.configId = configId;
        this.duplicateFlag = duplicateFlag;
        this.dataSeqNo = dataSeqNo;
        this.rawData = new byte[rawData.length - MIN_MSG_LENGTH];
        System.arraycopy(rawData, DATA_OFFSET, rawData, 0, this.rawData.length);
    }

    /**
     * @see org.ipdr.protocol.ProtocolMsg#createRawMsg()
     */
    public final byte[] createRawMsg()
    {
        byte[] msgData = new byte[MIN_MSG_LENGTH + this.rawData.length];
        setHeader(msgData);
        byte flag = 0;
        if (duplicateFlag)
        {
            flag |= DUPLICATE_FLAG;
        }
        bigE.putUnsignedShort(msgData, TEMPLATE_ID_OFFSET, this.templateId);
        bigE.putUnsignedShort(msgData, CONFIG_ID_OFFSET, this.configId);
        bigE.putUnsignedByte(msgData, FLAGS_OFFSET, flag);
        bigE.putLong(msgData, DSN_OFFSET, dataSeqNo);
        System.arraycopy(this.rawData, 0, msgData, DATA_OFFSET,
                this.rawData.length);
        return msgData;
    }

    /**
     * @see java.lang.Object#toString()
     */
    public String toString()
    {
        return toStringBase() + "[tmplId=" + templateId + " configId="
                + configId + (duplicateFlag ? " dup" : "")
                + " DSN=" + dataSeqNo + "]";
    }


    private final static int TEMPLATE_ID_OFFSET = 8;

    private final static int CONFIG_ID_OFFSET = 10;

    private final static int FLAGS_OFFSET = 12;

    private final static int DSN_OFFSET = 13;

    private final static int DATA_LEN_OFFSET = 21;
    
    private final static int DATA_OFFSET = 25;

    private final static int MIN_MSG_LENGTH = 25;

    private final static byte DUPLICATE_FLAG = 0x01;

    /**
     * Returns the configId.
     * 
     * @return short
     */
    public short getConfigId()
    {
        return configId;
    }

    /**
     * Returns the templateId.
     * 
     * @return int
     */
    public short getTemplateId()
    {
        return templateId;
    }

    /**
     * Returns the dataSeqNo.
     * 
     * @return long
     */
    public long getDataSeqNo()
    {
        return dataSeqNo;
    }

    /**
     * Returns the rawDataOffset.
     * 
     * @return int
     */
    public int getRawDataOffset()
    {
        return rawDataOffset;
    }

    /**
     * Returns the rawData.
     * 
     * @return byte[]
     */
    public byte[] getRawData()
    {
        return rawData;
    }

    /**
     * @return Returns the duplicateFlag.
     */
    public boolean isDuplicateFlag()
    {
        return duplicateFlag;
    }
}