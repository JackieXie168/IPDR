package org.ipdr.protocol;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.io.EOFException;
import java.net.UnknownHostException;

import org.ipdr.ConnectionDynamic;
import org.ipdr.api.ConnectionIOException;
import org.ipdr.api.log.LogManager;

/** 
 * The Java encapsulation of a IPDR protocol message over TCP
 * The message has the common header:
 *  <xmp>
 * struct IPDRStreamingHeader 
 * { 
 * 		char version; // version of protocol, for this version, set to 2 
 * 		char messageId; // the ID of this Message (see MessageIds)  
 * 		char sessionId; // the ID of this Session or 0 if not Session-specific 
 * 		char messageFlags; // reserved/unused and set to 0 
 * 		int messageLen; // length in bytes of Message including header  
 * }
 * </xmp>
 */
public abstract class ProtocolStreamMsg extends ProtocolMsg 
{
	/** Construct from raw bytes
	 *  @param msg The raw message bytes
	 *  @param length The number of bytes in the message
	 *  @param requiredLength The required number of bytes in the message
	 */
	public ProtocolStreamMsg(byte[] msg, int length, int requiredLength)
		throws ProtocolMalformedMsgException
	{
		set(msg, length, requiredLength);
	}

	/**
	 * Create the message from raw bytes
	 * @param msg The raw message bytes
	 * @param length The number of bytes in the message
	 * @param requiredLength The required number of bytes in the message
	 * @throws ProtocolMalformedMsgException
	 */
	public final void set(byte[] msg, int length, int requiredLength)
		throws ProtocolMalformedMsgException
	{
		initZero();
		verifyLength(this, msg, length, requiredLength);

		try
		{
			this.protocolVersion = bigE.getByte(msg, PROTOCOL_VERSION_OFFSET);
			this.hdrMsgId = bigE.getByte(msg, MSGID_OFFSET);
			this.sessionId = bigE.getByte(msg, SESSION_ID_OFFSET);
			this.hdrFlags = bigE.getByte(msg, HDR_FLAGS_OFFSET);
			this.msgLength = (int) bigE.getUnsignedInt(msg, MSG_LENGTH_OFFSET);
		}
		catch (IndexOutOfBoundsException e)
		{
			handleIndexOutOfBounds(e, msg, length);
			LogManager.stackTrace(e);
		}

		if (this.protocolVersion != PROTOCOL_VERSION)
		{
			throw new ProtocolMalformedMsgException(
				msgName()
					+ " protocol version("
					+ this.protocolVersion
					+ ") not "
					+ PROTOCOL_VERSION,
				msg,
				length,
				PROTOCOL_VERSION_OFFSET);
		}

		if (length > msg.length)
		{
			throw new ProtocolMalformedMsgException(
				msgName()
					+ " message length ("
					+ length
					+ ") greater than bytes.length ("
					+ msg.length
					+ ")",
				msg,
				length,
				MSG_LENGTH_OFFSET);
		}
	}

	/** 
	 * Constructor only for use by "pseudo" messages
	 */
	protected ProtocolStreamMsg()
	{
		initZero();
	}

	/** 
	 * Set values to zero 
	*/
	private final void initZero()
	{
		this.protocolVersion = 0;
		this.hdrMsgId = 0;
		this.sessionId = 0;
		this.hdrFlags = 0;
		this.msgLength = 0;
	}

	/** Construct from values
	 *  @param msgId
	 *  @param sessionId
	 *  @param msgLength The number of bytes in the message (including header)
	 */
	public ProtocolStreamMsg(byte msgId, byte sessionId, int msgLength)
	{
		this.protocolVersion = PROTOCOL_VERSION;
		this.hdrMsgId = msgId;
		this.sessionId = sessionId;
		this.hdrFlags = 0;
		this.msgLength = msgLength;
	}

	/** 
	 * Helper method for dealing with out-of-bounds when processing raw data
	 *  @param e The exception
	 *  @param buf the buffer
	 *  @param length the length of message as given to the constructor
	 *  @exception Turns this into a ProtocolMalformedMsgException
	 */

	protected final void handleIndexOutOfBounds(
		IndexOutOfBoundsException e,
		byte[] buf,
		int length)
		throws ProtocolMalformedMsgException
	{
		LogManager.stackTrace(e);
		String msg =
			msgName()
				+ " message length of buffer ("
				+ length
				+ ") shorter than message contents";
		if (buf.length != length)
		{
			msg += " - with buffer containing " + buf.length + " bytes";
		}
		msg += " ... " + e;
		throw new ProtocolMalformedMsgException(msg, buf, length, 0);
	}

	/** Verify the length and throw an exception if wrong
	 *  @param id the object that provides identification methods (may be null)
	 *  @param buf the raw message buffer
	 *  @param bufLength the message buffer length
	 *  @param requiredLength the minimum size of the message
	 *  @exception ProtocolMalformedMsgException if bufLength < requiredLength
	 */
	protected static final void verifyLength(
		ProtocolMsgIdentification id,
		byte[] buf,
		int bufLength,
		int requiredLength)
		throws ProtocolMalformedMsgException
	{
		requiredLength = round32(requiredLength);
		if (bufLength < requiredLength)
		{
			throw new ProtocolMalformedMsgException(
				((id == null) ? "???" : id.msgName())
					+ " message too short (must be at least "
					+ requiredLength
					+ ")",
				buf,
				bufLength,
				MSG_LENGTH_OFFSET + ProtocolData.unsignedIntSize);
		}
	}

	/**
	 * Round the length to 4-byte alignment
	 * @param value length
	 * @return rounded length
	 */
	public static final int round32(int value)
	{
		return (value + 3) >> 2 << 2;
	}

	/** Trim a byte vector to a particular length.
	 *  This is a slightly inefficient routine, mainly for catching errors:
	 *  it may reallocate the vector in the course of trimming.
	 *  @param buf The input vector
	 *  @param length The length to trim it to
	 *  @return The original vector, if it was already of that length;
	 *          an exception if the length is too long; otherwise
	 *          a new vector of the exact length (with the contents
	 *          of the input vector).
	 */

	protected final byte[] trimToLength(byte[] buf, int length)
	{
		byte[] result;
		if (length == buf.length)
		{
			result = buf;
		}
		else
			if (length > buf.length)
			{
				throw new IndexOutOfBoundsException(
					"Specified length "
						+ length
						+ " to a byte vector of length "
						+ buf.length);
			}
			else
			{
				result = new byte[length];
				System.arraycopy(buf, 0, result, 0, length);
			}
		return result;
	}

	/** Get the message id value. This is overridden by
	 *  a subclass.
	 */
	public abstract byte msgId();

	/** Flag for whether we should we output a log message when
	 *  sending or receiving.  Subclasses may override this (i.e.,
	 *  DATA and DATA_ACK) will to check for "debug" level output.
	 */
	public boolean shouldOutputToLog()
	{
		return LogManager.info();
	}

	/** Read an entire raw message, reallocating a buffer if needed.
	 *  This routine is provided for efficiency; normally, you would use
	 *  @link readMsg.
	 *  This function is thread-safe on "in", which must not be null
	 *  XXX It is not clear to what degree DataInputStream is buffered.
	 *      If performance is inadequate, consider using BufferedReader
	 *      or some other buffered input class.
	 *  @param in The input socket (e.g., new DataInputStream(socket.getInputStream()))
	 *  @param buf The buffer into which the data will be read. This can be zero length
	 *      initially, if desired; but it is recommended to be larger.
	 *      It is not guaranteed that the message will be put into this
	 *      buffer; a separate buffer might be allocated and returned.
	 *  @return The buffer, reallocated larger if necessary.
	 *  @exception EOFException is thrown if end-of-file is reached before
	 *             reading an entire message - if this happens, the contents
	 *             of the buffer are unpredictable.
	 *             IOException is thrown for all other I/O errors.
	 *  @see ConnectionDynamic#readFully
	 */
	private static final byte[] readRawMsg(
		ConnectionDynamic connection,
		byte[] buf,
		int timeoutMillisec)
		throws java.io.IOException
	{
		if (buf.length < HDR_LENGTH)
		{
			buf = new byte[HDR_LENGTH];
		}

		// The following code assumes that readFully will block
		// until all the message is read and that if it throws an
		// exception, on re-read will get everything again (keeps
		// things in a buffer - see {@link ConnectionDynamic#unread}).
		synchronized (connection.getInputSynchronizer())
		{
			long t1 = System.currentTimeMillis();
			try
			{
				connection.readFully(buf, 0, HDR_LENGTH, timeoutMillisec);
//				LogManager.info(ProtocolStreamMsg.class, "readRawMsg",
//					"In readRawMsg : after connection.readFully 1");

			}
			catch (java.io.InterruptedIOException ex)
			{
				connection.unread(buf, 0, ex.bytesTransferred);
				ex.bytesTransferred = 0;
				throw ex;
			}

			int msgLength = (int) (bigE.getUnsignedInt(buf, MSG_LENGTH_OFFSET));

			if (buf.length < msgLength)
			{
				byte[] newbuf = new byte[msgLength];
				System.arraycopy(buf, 0, newbuf, 0, HDR_LENGTH);
				buf = newbuf;
			}

			int timeout2;
			if (timeoutMillisec == 0)
			{
				timeout2 = 0;
			}
			else
			{
				timeout2 =
					timeoutMillisec - (int) (System.currentTimeMillis() - t1);
				if (timeout2 <= 0)
				{
					timeout2 = 1; // a very short timeout
				}
			}
			try
			{
				connection.readFully(
					buf,
					HDR_LENGTH,
					msgLength - HDR_LENGTH,
					timeout2);

			}
			catch (java.io.InterruptedIOException ex)
			{
				connection.unread(buf, 0, HDR_LENGTH + ex.bytesTransferred);
				ex.bytesTransferred = 0;
				throw ex;
			}
		}
		return buf;
	}

	/** Read a message from input.
	 *  This allocates a buffer each time, so it is less efficient than using
	 *  @link readRawMsg and remembering the buffer between calls.
	 *  @param in The input socket (e.g., new DataInputStream(socket.getInputStream()))
	 *  @param whatDoing A brief description of what this read is for; used to
	 *                   construct a meaningful exception (may be NULL)
	 *  @param responseTo The message to which this is a response (may be NULL)
	 *  @param timeoutMillisec zero for infinite wait
	 *  @return The internal form of the message
	 */
	public static final ProtocolStreamMsg readMsg(
		byte[] buf,
		ConnectionDynamic connection,
		String whatDoing,
		ProtocolStreamMsg responseTo,
		int timeoutMillisec)
		throws ConnectionIOException, ProtocolMalformedMsgException
	{

		ProtocolStreamMsg result;

		try
		{
			buf = readRawMsg(connection, buf, timeoutMillisec);

			result = construct(buf);

			if (result.shouldOutputToLog())
			{
				LogManager.debug(
					"ProtocolMsg",
					"readMsg",
					"received from "
						+ connection.toString()
						+ ": "
						+ result.toStringLong());
			}
		}
		catch (java.io.IOException e)
		{
			if (e instanceof java.io.InterruptedIOException)
			{
				result = new ProtocolMsgEmpty();
			}
			else
			{
				throw new ConnectionIOException(
					"I/O error while reading "
						+ makeErrMsg(whatDoing, responseTo),
					e);
			}
		}
		return result;
	}

	public static final ProtocolStreamMsg readMsg(
		ConnectionDynamic connection,
		String whatDoing,
		ProtocolStreamMsg responseTo,
		int timeoutMillisec)
		throws ConnectionIOException, ProtocolMalformedMsgException
	{
		return readMsg(new byte[0x100], // should be big enough
		connection, whatDoing, responseTo, timeoutMillisec);
	}

	private static final String makeErrMsg(
		String whatDoing,
		ProtocolStreamMsg responseTo)
	{
		String errMsg = "";
		if (whatDoing != null)
		{
			errMsg += whatDoing;
		}
		if (responseTo != null)
		{
			errMsg += ((errMsg.length() > 0) ? " - " : "") + responseTo;
		}
		return errMsg;
	}

	/** Write a message to the socket. This method is thread-safe on "out", which
	 *  must not be null.
	 *  @param connection the connection
	 */
	public final void sendMsg(ConnectionDynamic connection)
		throws ConnectionIOException
	{
		byte[] msg = null;
		try
		{
			msg = createRawMsg();
		}
		catch (UnknownHostException e)
		{
			throw new ConnectionIOException("Error while sending " + this, e);
		}

		if (this.shouldOutputToLog())
		{
			LogManager.debug(
				this,
				"send",
				"Sending to " + connection.addrAndPortToString() + ": " + this);
		}
		try
		{
			synchronized (connection.getOutputSynchronizer())
			{
				connection.write(msg);
			}
		}
		catch (java.io.IOException e)
		{
			throw new ConnectionIOException(
				"I/O error while sending " + this,
				e);
		}
	}

	public static final ProtocolStreamMsg construct(byte[] msg, int length)
		throws ProtocolMalformedMsgException
	{
		return construct(msg, length, null);
	}

	/** Factory method for constructing from raw bytes.
	 *  This looks into the header and calls the appropriate sub-class
	 *  @param msg the raw bytes
	 *  @param length the number of bytes in the message
	 *  @param msgDATA a pre-allocated DATA object, which can be used if
	 *                 if this is of that type -- null if you want one
	 *                 constructed.
	 *  @return the Java-ized form of the message
	 */
	public static final ProtocolStreamMsg construct(
		byte[] msg,
		int length,
		ProtocolMsgDATA msgDATA)
		throws ProtocolMalformedMsgException
	{
		verifyLength(null, msg, length, HDR_LENGTH);
		byte protocolVersion = bigE.getByte(msg, 0);
		if (protocolVersion != PROTOCOL_VERSION)
		{
			throw new ProtocolMalformedMsgException(
				"Wrong protocol version: " + protocolVersion,
				msg,
				length,
				0);
		}
		byte msgId = bigE.getByte(msg, 1);
		try
		{
			switch (msgId)
			{
				case ProtocolMsgCONNECT.MSG_ID :
					return new ProtocolMsgCONNECT(msg, length);
				case ProtocolMsgCONNECT_RESPONSE.MSG_ID :
					return new ProtocolMsgCONNECT_RESPONSE(msg, length);
				case ProtocolMsgDISCONNECT.MSG_ID :
					return new ProtocolMsgDISCONNECT(msg, length);
				case ProtocolMsgKEEPALIVE.MSG_ID :
					return new ProtocolMsgKEEPALIVE(msg, length);
				case ProtocolMsgTEMPLATE_DATA.MSG_ID :
					return new ProtocolMsgTEMPLATE_DATA(msg, length);
				case ProtocolMsgMODIFY_TEMPLATE_RESPONSE.MSG_ID :
					return new ProtocolMsgMODIFY_TEMPLATE_RESPONSE(msg, length);
				case ProtocolMsgSESSION_START.MSG_ID :
					return new ProtocolMsgSESSION_START(msg, length);
				case ProtocolMsgSESSION_STOP.MSG_ID:
					return new ProtocolMsgSESSION_STOP(msg, length);
				case ProtocolMsgDATA.MSG_ID :
					return new ProtocolMsgDATA(msg, length);
				case ProtocolMsgSTART_NEGOTIATION_REJECT.MSG_ID:
						return new ProtocolMsgSTART_NEGOTIATION_REJECT(msg, length);
				case ProtocolMsgGET_SESSIONS_RESPONSE.MSG_ID:
				    return new ProtocolMsgGET_SESSIONS_RESPONSE(msg, length);
				case ProtocolMsgGET_TEMPLATES_RESPONSE.MSG_ID:
				    return new ProtocolMsgGET_TEMPLATES_RESPONSE(msg, length);
				case ProtocolMsgERROR.MSG_ID:
				    return new ProtocolMsgERROR(msg, length);
				default :
					throw new ProtocolMalformedMsgException(
						"Message header invalid msgId=" + msgId,
						msg,
						length,
						1);
			}
		}
		catch (IndexOutOfBoundsException e)
		{
			throw new ProtocolMalformedMsgException(
				"Error parsing message",
				msg,
				length,
				1);
		}
	}

	/** Factory method for constructing from raw bytes.
	 *  This looks into the header and calls the appropriate sub-class
	 *  @param msg the raw bytes
	 *  @return the Java-ized form of the message
	 */
	public static final ProtocolStreamMsg construct(byte[] msg)
		throws ProtocolMalformedMsgException
	{
		return construct(msg, msg.length);
	}

	/** A human-readable form of the message. This is over-ridden by
	 *  sub-classes, which typically use this (super.toString()) to
	 *  get the header part.
	 *  @return human-readable display
	 */
	protected String toStringBase()
	{
		return "msg-" + msgName() +
		// will get sub-class method for name, if it exists
		"["
			+ ((protocolVersion == PROTOCOL_VERSION)
				? ""
				: "version=" + protocolVersion + " ")
			+ "sess="
			+ sessionId
			+ ((hdrFlags != 0) ? " flags=" + hdrFlags : "")
			+ "]";
	}

	/** A longer human-readable message. Overridden by some sub-classes.
	 *  @return human-readable string (long)
	 */
	public String toStringLong()
	{
		return "msg-" + msgName() +
		// will get sub-class method for name, if it exists
		"[version="
			+ protocolVersion
			+ " sess="
			+ sessionId
			+ ((hdrFlags != 0) ? " flags=" + hdrFlags : "")
			+ " len="
			+ msgLength
			+ "]";
	}

	/** Fill in the header of the message. Typically this is not
	 *  used directly but is used by sub-classes.
	 *  @param msgData the message bytes to be filled
	 *  @param msgId CRANE protocol header message id
	 *  @param sessionId CRANE protocol header session id
	 *  @param msgFlags CRANE protocol header flags (normally zero)
	 *  @param msgLength CRANE protocol header length (normally msgData.length)
	 */
	protected final void setHeader(byte[] msgData)
	{
		bigE.putByte(msgData, PROTOCOL_VERSION_OFFSET, PROTOCOL_VERSION);
		bigE.putByte(msgData, MSGID_OFFSET, msgId());
		bigE.putByte(msgData, SESSION_ID_OFFSET, (byte) this.sessionId);
		bigE.putByte(msgData, HDR_FLAGS_OFFSET, (byte) 0);
		bigE.putUnsignedInt(msgData, MSG_LENGTH_OFFSET, this.msgLength);
	}


	/// Utility: dump a raw message in hex
	public static final void dumpHex(byte[] raw, int offset, int length)
	
	{
		LogManager.println("dumping ...");
		final int stride = 8;
		for (int i = 0; i < length && i + offset < raw.length; i += stride)
		{
			String line = "";
			int pos = offset + i;
			if (i < 10)
			{
				line += "...   " + i;
			}
			else
				if (i < 100)
				{
					line += "...  " + i;
				}
				else
				{
					line += "... " + i;
				}
			if (pos < 10)
			{
				line += " _   " + pos;
			}
			else
				if (pos < 100)
				{
					line += " _  " + pos;
				}
				else
				{
					line += " _ " + pos;
				}
			line += ": ";
			String chars = "";
			for (int j = 0; j < 8 && i + j + offset < raw.length; j++)
			{
				int v = raw[offset + i + j] & 0xff;
				line += " " + ((v < 0x10) ? " " : "") + Integer.toHexString(v);
				if (v >= 0x20 && v <= 0xfe)
				{
					chars += (char) v;
				}
				else
				{
					chars += '?';
				}
			}
			LogManager.println(line + " (" + chars + ")");
		}
	}


	/// 
	/**
	 * The protocol Version
	 */
	public short protocolVersion;

	/// 
	/**
	 * The session identifier
	 */
	public byte sessionId;

	/// 
	/**
	 * Message header flags (ignored in version 1 and 2 of the protocol)
	 */
	public byte hdrFlags;

	/** Message length, from the header. The header shows an unsigned
	 *  32 but Java won't let us construct a byte array with a "long",
	 *  so let's leave this as an "int" (2^31 is a pretty big message
	 *  in CRANE anyway!).
	 */
	public int msgLength;

	/// The minimum length of a message (header only)
	public static final int HDR_LENGTH = 8;

	private static final int PROTOCOL_VERSION_OFFSET = 0;
	private static final int MSGID_OFFSET = 1;
	private static final int SESSION_ID_OFFSET = 2;
	///// This is 2 in the protocol doc !!!!!!!!!!!!
	private static final int HDR_FLAGS_OFFSET = 3;
	///// This is 3 in the protocol doc !!!!!!!!!!!!
	private static final int MSG_LENGTH_OFFSET = 4;

	/// The version of the CRANE protocol that we support
	public static final byte PROTOCOL_VERSION = 2;

	/** Output a message about an unimplemented method and throw
	 *  an appropriate execption.
	 * @param className The name of the class.
	 * @param function The name of the function (method).
	 * @throws UnsupportedOperationException Always thrown.
	 */
	public static void unimplemented(String className, String function)
		throws UnsupportedOperationException
	{
		throw new UnsupportedOperationException(
			"Not implemented (" + className + "." + function + ")");
	}

	/** Output a message about an unimplemented method and throw
	 *  an appropriate execption.
	 * @param o The object whose method is unimplemented.
	 * @param function The name of the function (method).
	 * @throws UnsupportedOperationException Always thrown.
	 */
	public static void unimplemented(Object o, String function)
		throws UnsupportedOperationException
	{
		unimplemented(o.getClass().getName(), function);
	}
	/**
	 * Returns the sessionId.
	 * @return byte
	 */
	public byte getSessionId()
	{
		return sessionId;
	}

}
