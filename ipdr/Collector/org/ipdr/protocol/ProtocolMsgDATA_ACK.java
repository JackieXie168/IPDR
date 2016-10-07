package org.ipdr.protocol;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/**
 * The class represents a DATA_ACK message.
 * The message has the following structure:
 * <xmp>
 * struct DataAcknowledge 
 * { 
 * 		struct IPDRStreamingHeader header; 
 * 		short configId; 
 *		long sequenceNum; // Session relative sequence number of last record 
 * 						  // received in a series.  
 * }
 * </xmp>
*/
public final class ProtocolMsgDATA_ACK extends ProtocolStreamMsg
{
	/**
	 * @see org.ipdr.protocol.ProtocolMsgIdentification#msgName()
	 */
	public String msgName()
	{
		return "DATA_ACK";
	}
	public static final byte MSG_ID = 0x21;
	/**
	 * @see org.ipdr.protocol.ProtocolStreamMsg#msgId()
	 */
	public final byte msgId()
	{
		return MSG_ID;
	}

	/**
	 * The Data Sequence Number
	 */
	public long dataSeqNo;

	/**
	 * The configuration id
	 */
	public short configId;

	/** 
	 * Construct from raw bytes
	 * @param msg The raw message bytes
	 * @param length The number of bytes in the message
	 */
	public ProtocolMsgDATA_ACK(byte[] msg, int length)
		throws ProtocolMalformedMsgException
	{
		super(msg, length, MSG_LENGTH);

		try
		{
			this.dataSeqNo = bigE.getLong(msg, DSN_OFFSET);
			this.configId = bigE.getShort(msg, CONFIG_ID_OFFSET);
		}
		catch (IndexOutOfBoundsException e)
		{
			handleIndexOutOfBounds(e, msg, length);
		}
	}

	/** 
	 * Construct from values.
	 * @param sessionId session id
	 * @param configId configuration id
	 * @param dataSeqNo data sequence number to acknowledge
	 */
	public ProtocolMsgDATA_ACK(byte sessionId, short configId, long dataSeqNo)
	{
		super(MSG_ID, sessionId, MSG_LENGTH);
		this.configId = configId;
		this.dataSeqNo = dataSeqNo;
	}

	/**
	 * @see org.ipdr.protocol.ProtocolMsg#createRawMsg()
	 */
	public final byte[] createRawMsg()
	{
		byte[] msgData = new byte[MSG_LENGTH];
		setHeader(msgData);
		bigE.putShort(msgData, CONFIG_ID_OFFSET, this.configId);
		bigE.putLong(msgData, DSN_OFFSET, this.dataSeqNo);
		return msgData;
	}

	/**
	 * @see java.lang.Object#toString()
	 */
	public String toString()
	{
		return toStringBase()
			+ "[configID="
			+ configId
			+ " DSN="
			+ dataSeqNo
			+ "]";
	}

	private final static int DSN_OFFSET = 10;
	private final static int CONFIG_ID_OFFSET = 8;

	private final static int MSG_LENGTH = 18;
}
