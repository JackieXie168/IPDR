package org.ipdr.protocol;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
//ANY KIND either expresses or implied.


/**
 * The class represents a SESSION_START message.
 * The message has the following structure:
 * <xmp>
 * struct SessionStart
 * {
 * 		struct IPDRStreamingHeader header;
 * 		int exporterBootTime; // boot time of Exporter(in seconds from epoch time)
 * 		long firstRecordSequenceNumber; // first sequence number to be expected
 * 		long droppedRecordCount // number of records dropped in gap situations
 * 		boolean primary; // indication that the Collector is the primary
 * 		int ackTimeInterval; // the maximum time between acks from Collector
 * 							 // (in second units)
 * 		int ackSequenceInterval; // the maximum number of unacknowledged records
 * 		char documentId[16]; // the UUID associated with the info being sent
 * 							 // in this Session
 * }
 * </xmp>
 */
import org.ipdr.api.SessionStartInfo;
public class ProtocolMsgSESSION_START extends ProtocolStreamMsg implements SessionStartInfo
{
    /**
     * Holds exporter boot time
     */
    protected int     exporterBootTime;
    /**
     * Holds first record sequence number
     */
    protected long    firstRecordSequenceNumber;
    /**
     * Holds dropped record count
     */
    protected long    droppedRecordCount;
    /**
     * Holds primary flag
     */
    protected boolean primary;
    /**
     * Holds data acknowledge timeout
     */
    protected int     ackTimeInterval;
    /**
     * Holds data acknowledge message window size
     */
    protected int     ackSequenceInterval;
    /**
     * Holds document id as reported by Exporter
     */
    byte[]            documentId;

    /**
     * @see org.ipdr.protocol.ProtocolMsgIdentification#msgName()
     */
    public String msgName()
    {
        return "SESSION_START";
    }
    public static final byte MSG_ID = 0x08;

    /**
     * @see org.ipdr.protocol.ProtocolStreamMsg#msgId()
     */
    public final byte msgId()
    {
        return MSG_ID;
    }

    /**
     * Creates the message from raw bytes
     * @param message Raw bytes
     * @param length Buffer size 
     * @throws ProtocolMalformedMsgException
     */
    public ProtocolMsgSESSION_START(byte[] message, int length)
            throws ProtocolMalformedMsgException
    {
        super(message, length, MSG_LENGTH);
        this.exporterBootTime = bigE.getInt(message, EXPORTER_BOOT_TIME_OFFSET);
        this.firstRecordSequenceNumber = bigE.getLong(message,
                FIRST_RECORD_SEQ_NUMBER_OFFSET);
        this.droppedRecordCount = bigE.getLong(message,
                DROPPED_RECORD_COUNT_OFFSET);
        this.primary = bigE.getBoolean(message, PRIMARY_OFFSET);
        this.ackTimeInterval = bigE.getInt(message, ACK_TIME_INTERVAL_OFFSET);
        this.ackSequenceInterval = bigE.getInt(message,
                ACK_SEQUENCE_INTERVAL_OFFSET);
        this.documentId = bigE.getFixedByteArray(message, DOCUMENT_ID_OFFSET,
                DOCUMENT_ID_LEN);
    }

    /**
     * @see org.ipdr.protocol.ProtocolMsg#createRawMsg()
     */
    protected byte[] createRawMsg()
    {
        return null;
    }

    private final static int EXPORTER_BOOT_TIME_OFFSET      = HDR_LENGTH;
    private final static int FIRST_RECORD_SEQ_NUMBER_OFFSET = EXPORTER_BOOT_TIME_OFFSET + 4;
    private final static int DROPPED_RECORD_COUNT_OFFSET    = FIRST_RECORD_SEQ_NUMBER_OFFSET + 8;
    private final static int PRIMARY_OFFSET                 = DROPPED_RECORD_COUNT_OFFSET + 8;
    private final static int ACK_TIME_INTERVAL_OFFSET       = PRIMARY_OFFSET + 1;
    private final static int ACK_SEQUENCE_INTERVAL_OFFSET   = ACK_TIME_INTERVAL_OFFSET + 4;
    private final static int DOCUMENT_ID_OFFSET             = ACK_SEQUENCE_INTERVAL_OFFSET + 4;

    private final static int DOCUMENT_ID_LEN                = 16;
    private final static int MSG_LENGTH                     = DOCUMENT_ID_OFFSET + 16;

    /**
     * Returns the ackSequenceInterval.
     *
     * @return int
     */
    public int getAckSequenceInterval()
    {
        return ackSequenceInterval;
    }

    /**
     * Returns the ackTimeInterval.
     *
     * @return int
     */
    public int getAckTimeInterval()
    {
        return ackTimeInterval;
    }

    /**
     * Returns the documentId.
     *
     * @return byte[]
     */
    public byte[] getDocumentId()
    {
        return this.documentId;
    }

    /**
     * Returns the droppedRecordCount.
     *
     * @return long
     */
    public long getDroppedRecordCount()
    {
        return droppedRecordCount;
    }

    /**
     * Returns the exporterBootTime.
     *
     * @return int
     */
    public int getExporterBootTime()
    {
        return exporterBootTime;
    }

    /**
     * Returns the firstRecordSequenceNumber.
     *
     * @return long
     */
    public long getFirstRecordSequenceNumber()
    {
        return firstRecordSequenceNumber;
    }

    /**
     * Returns the primary collector flag.
     *
     * @return boolean
     */
    public boolean isPrimary()
    {
        return primary;
    }

    /**
     * @see java.lang.Object#toString()
     */
    public String toString()
    {
        char[] charDocumentId = new char[DOCUMENT_ID_LEN];
        for (int i = 0; i < DOCUMENT_ID_LEN; i ++)
            charDocumentId[i]=(char)this.documentId[i];


        return toStringBase() + "[" + "exporterBootTime:"
                + getExporterBootTime() + "," + "firstSN:"
                + getFirstRecordSequenceNumber() + "," + "droppedCount:"
                + getDroppedRecordCount() + "," + "primary:" + isPrimary()
                + "," + "acktimeinterval:" + getAckTimeInterval() + ","
                + "ackseqinterval:" + getAckSequenceInterval() + ","
                + "documentId:" + String.valueOf(charDocumentId) + "]";
    }

}
