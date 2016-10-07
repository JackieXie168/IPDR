package com.xacct.CRANE.collector;

// $Id: ProtocolMsgDATA.java,v 1.4 2005/01/30 15:13:41 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/** The Java encapsulation of a CRANE protocol messages: "DATA".
 *  <xmp>
 *        0                   1                   2                   3
 *        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |  Version      |  MID=0x20     | Session ID    | Message Flags |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                         Message Length                        |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |        Template ID            |    Config. ID |  Flags    |D|S|
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                   Data Sequence Number (DSN)                  |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                                                               |
 *       ~                          Record Data                          ~
 *       |                                                               |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  </xmp>
 *
 *  @version $Id: ProtocolMsgDATA.java,v 1.4 2005/01/30 15:13:41 ilyad Exp $
 */
public final class ProtocolMsgDATA extends ProtocolMsg
{
    /** $Id: ProtocolMsgDATA.java,v 1.4 2005/01/30 15:13:41 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: ProtocolMsgDATA.java,v 1.4 2005/01/30 15:13:41 ilyad Exp $"; }

    public String msgName() { return "DATA"; }
    public static final byte MSG_ID = 0x20;
    public final byte msgId() { return MSG_ID; }
    public final boolean shouldOutputToLog(LogHandler log) { return log.debug(); }

    /// The template id
    public int templateId;

    /// The configuration id
    public short configId;

    /// The "duplicate" flag
    public boolean duplicateFlag;

    /// The "start DataSequenceNumber sequence" flag
    public boolean startDSNflag;

    /// The Data Sequence Number ("DSN")
    public long dataSeqNo;

    /// The raw data
    public byte [] rawData;

    /// Where the actual data starts in rawData (i.e., initial offset
    /// to use when pulling dataout of rawData).
    public int rawDataOffset;

    /** Construct a basic object, for reuse (with the <code>set</code> method).
     *  @param msg The raw message: must be at least 16 bytes but otherwise
     *             not used.
     */
    public ProtocolMsgDATA(byte [] msg) throws ProtocolMalformedMsgException {
        super(msg, MIN_MSG_LENGTH, MIN_MSG_LENGTH);
    }

    /** Construct from raw bytes. Code has been removed that copies
     *  the message buffer, so beware: if you re-use "msg", you'll
     *  corrupt what's here. You can be safe by doing:
     *      byte [] newRawData = new byte[rawData.length];
     *      System.arraycopy(rawData, 0, newRawData, 0, rawData.length);
     *      rawData = newRawData;
     *  @param msg The raw message bytes
     *  @param length The number of bytes in the message
     */
    public ProtocolMsgDATA(byte [] msg, int length) throws ProtocolMalformedMsgException {
        super(msg, length, MIN_MSG_LENGTH);
        set2(msg, length);
    }

    /**
     */
    public final ProtocolMsgDATA set(byte [] msg, int length)
        throws ProtocolMalformedMsgException {
        super.set(msg, length, MIN_MSG_LENGTH);
        return set2(msg, length);
    }

    private final ProtocolMsgDATA set2(byte [] msg, int length)
        throws ProtocolMalformedMsgException {
        // msg = trimToLength(msg, this.msgLength);
        try {
            this.templateId    = bigE.getUnsignedShort(msg, TEMPLATE_ID_OFFSET);
            this.configId      = bigE.getUnsignedByte( msg, CONFIG_ID_OFFSET);
            byte flags         = bigE.getByte(         msg, FLAGS_OFFSET);
            this.dataSeqNo     = bigE.getUnsignedInt(  msg, DSN_OFFSET);
            this.duplicateFlag = (flags & DUPLICATE_FLAG) != 0;
            this.startDSNflag  = (flags & START_DSN_FLAG) != 0;
            if (true) {
                this.rawData   = msg;
                this.rawDataOffset = DATA_OFFSET;
            } else {
                this.rawData   = new byte[this.msgLength - DATA_OFFSET];
                System.arraycopy(msg, DATA_OFFSET, this.rawData, 0, this.rawData.length);
                this.rawDataOffset = 0;
            }
        } catch (IndexOutOfBoundsException e) { handleIndexOutOfBounds(e, msg, length); }
        return this;
    }

    /** Construct from values.
     *  Makes a copy of the data (inefficient).
     */
    public ProtocolMsgDATA(byte sessionId,
                           byte templateId,
                           byte configId,
                           boolean duplicateFlag,
                           boolean startDSNflag,
                           long dataSeqNo,
                           byte [] rawData) {
        super(MSG_ID, sessionId, rawData.length + MIN_MSG_LENGTH);
        this.templateId    = templateId;
        this.configId      = configId;
        this.duplicateFlag = duplicateFlag;
        this.startDSNflag  = startDSNflag;
        this.dataSeqNo     = dataSeqNo;
        this.rawData       = new byte[rawData.length - MIN_MSG_LENGTH];
        System.arraycopy(rawData, DATA_OFFSET, rawData, 0, this.rawData.length);
    }

    public final byte [] createRawMsg() {
        byte [] msgData = new byte[MIN_MSG_LENGTH + this.rawData.length];
        setHeader(msgData);
        byte flag = 0;
        if (duplicateFlag) { flag |= DUPLICATE_FLAG; }
        if (startDSNflag)  { flag |= START_DSN_FLAG; }
        bigE.putUnsignedShort(msgData, TEMPLATE_ID_OFFSET, this.templateId);
        bigE.putUnsignedByte( msgData, CONFIG_ID_OFFSET,   this.configId);
        bigE.putUnsignedByte( msgData, FLAGS_OFFSET,       flag);
        bigE.putUnsignedInt(  msgData, DSN_OFFSET,         dataSeqNo);
        System.arraycopy(this.rawData, 0, msgData, DATA_OFFSET, this.rawData.length);
        return msgData;
    }

    public final long processDataSeqNo(long lastDataSeqNo, boolean anyDataRecordSeen)
    throws DataException {
        if (! anyDataRecordSeen && ! this.startDSNflag) {
            throw new DataException(this, "receiveData",
                                    "First data record does not have 'start DSN' flag: " +
                                    this.toStringLong());
        }
        if (this.startDSNflag) {
            lastDataSeqNo = this.dataSeqNo;
        } else {
            lastDataSeqNo ++;
            if (lastDataSeqNo > 0xffffffffL) {
                lastDataSeqNo = 0;
            }
            if (this.dataSeqNo != lastDataSeqNo) {
                throw new DataException(this, "receiveData",
                                        "Expected data seq #" + lastDataSeqNo + " but got #" +
                                        this.dataSeqNo + ": " + this.toStringLong());
            }
            lastDataSeqNo = this.dataSeqNo;
        }
        return lastDataSeqNo;
    }

    public String toString() {
        return super.toString() +
            "[tmplId=" + templateId +
            " configId=" + configId +
            (duplicateFlag ? " dup" : "") +
            (startDSNflag ? " startDSN" : "") +
            " DSN=" + dataSeqNo +
            "]";
    }

    public String toStringLong() {
        return toString();
    }

    private final static int TEMPLATE_ID_OFFSET =  8;
    private final static int CONFIG_ID_OFFSET   = 10;
    private final static int FLAGS_OFFSET       = 11;
    private final static int DSN_OFFSET         = 12;
    private final static int DATA_OFFSET        = 16;

    private final static int MIN_MSG_LENGTH     = 16;

    private final static byte START_DSN_FLAG    = 0x01;
    private final static byte DUPLICATE_FLAG    = 0x02;
}
