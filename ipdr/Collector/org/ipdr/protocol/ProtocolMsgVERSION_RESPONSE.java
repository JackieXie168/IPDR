package org.ipdr.protocol;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.util.ArrayList;

/**
 * The class represents IPDR VERSION_RESPONSE message
 * The message has the following structure:
 * <xmp>
 * struct VersionResponse 
 * { 
 * 		ProtocolInfo defaultProtocol; 
 * 		ProtocolInfo [] additionalProtocols; 
 * } 
 * struct ProtocolInfo 
 * { 
 * 		int transportType; 
 * 		int protocolVersion; 
 * 		short portNumber; 
 * 		short reserved; 
 * }
 * </xmp>
 */
public class ProtocolMsgVERSION_RESPONSE extends ProtocolMsg
{
	/**
	 * Holds default protocol
	 */
	ProtocolInfo defaultProtocol;
	/**
	 * Holds additional protocols
	 */
	ArrayList additionalProtocols = new ArrayList();
	/**
	 * @see org.ipdr.protocol.ProtocolMsg#createRawMsg()
	 */
	public byte[] createRawMsg() 
	{
	    byte [] message = new byte[ ProtocolInfo.INFO_SIZE * (additionalProtocols.size()+1) +4];
	    int offset = 0;
	    bigE.putProtocolInfo(message, offset, defaultProtocol);
	    offset += ProtocolInfo.INFO_SIZE;
	    bigE.putProtocolInfoBlock(message, offset, additionalProtocols);
		return message;
	}
	
	/**
	 * Creates VERSION_RESPONSE message from raw bytes
	 * @param message buffer to parse
	 * @throws ProtocolMalformedMsgException
	 */
	public ProtocolMsgVERSION_RESPONSE( byte [] message )
	throws ProtocolMalformedMsgException
	{
	    try
	    {
	        defaultProtocol = bigE.getProtocolInfo(message, 0);
	        additionalProtocols = bigE.getProtocolInfoBlock(message, ProtocolInfo.INFO_SIZE);
	    }
	    catch (IndexOutOfBoundsException e)
	    {
	        throw new ProtocolMalformedMsgException("Index out of bounds", message, message.length, 0);
	    }
	}
	
	/**
	 * Creates a VERSION_RESPONSE from values
	 * @param defaultProtocol default protocol
	 * @param additionalProtocols extra protocols
	 */
	public ProtocolMsgVERSION_RESPONSE( ProtocolInfo defaultProtocol, ArrayList additionalProtocols)
	{
	    this.defaultProtocol = defaultProtocol;
	    this.additionalProtocols = additionalProtocols;
	}

	/**
	 * @see java.lang.Object#toString()
	 */
	public String toString()
	{
		return "msg" + msgName() + "[" +
			defaultProtocol.toString() + "]";
	}

	/**
	 * Returns the additionalProtocols.
	 * @return ArrayList
	 */
	public ArrayList getAdditionalProtocols()
	{
		return additionalProtocols;
	}

	/**
	 * Returns the defaultProtocol.
	 * @return ProtocolInfo
	 */
	public ProtocolInfo getDefaultProtocol()
	{
		return defaultProtocol;
	}

	/**
	 * @see org.ipdr.protocol.ProtocolMsgIdentification#msgName()
	 */
	public String msgName()
	{
		return "VERSION_RESPONSE";
	}

}
