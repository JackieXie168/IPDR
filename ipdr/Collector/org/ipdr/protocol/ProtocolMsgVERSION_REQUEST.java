package org.ipdr.protocol;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.net.Inet4Address;
import java.net.InetAddress;

/**
 * The class represents IPDR VERSION_REQUEST message.
 * The message has the following structure:
 * <xmp>
 * struct VersionRequest 
 * { 
 * 		int requesterAddress; // IPv4 address of the version request initiator 
 * 		int requesterBootTime; // boot time of the version request initiator (in seconds from epoch time) 
 * 		char [4] msg; // MUST be 'CRAN' for version 1 and 'IPDR' for version 2 
 * }
 * </xmp>
 */
public class ProtocolMsgVERSION_REQUEST extends ProtocolMsg
{
	/**
	 * Holds requester address
	 */
	protected Inet4Address requesterAddress;
	/**
	 * Holds requester boot time
	 */
	protected int requesterBootTime;
	/**
	 * Holds protocol string
	 */
	protected byte [] protocolString = {'I', 'P', 'D', 'R'};
	/**
	 * @see org.ipdr.protocol.ProtocolMsg#createRawMsg()
	 */
	public byte[] createRawMsg() 
	{
		byte [] message = new byte[MSG_LENGTH];
		bigE.putAddr4(message, REQUESTER_ADDRESS_OFFSET, requesterAddress);
		bigE.putInt(message, REQUESTER_BOOT_TIME_OFFSET, requesterBootTime);
		bigE.putFixedByteArray(message, protocolString, PROTOCOL_STRING_OFFSET);
		return message;
	}
	
	/**
	 * Creates a VERSION_REQUEST message from values
	 * @param address requester address value
	 * @param bootTime boot time value
	 */
	public ProtocolMsgVERSION_REQUEST( Inet4Address address, int bootTime )
	{
		this.requesterAddress = address;
		this.requesterBootTime = bootTime;
	}
	
	/**
	 * Creates a VERSION_REQUEST from raw bytes
	 * @param buffer bytes
	 * @throws ProtocolMalformedMsgException
	 */
	public ProtocolMsgVERSION_REQUEST( byte[] buffer )
	throws ProtocolMalformedMsgException
	{
	    if ( buffer.length < MSG_LENGTH )
	        throw new ProtocolMalformedMsgException("Message too short", buffer, MSG_LENGTH, 0);
	    this.requesterAddress = bigE.getAddr4(buffer, REQUESTER_ADDRESS_OFFSET);
	    this.requesterBootTime = bigE.getInt(buffer, REQUESTER_BOOT_TIME_OFFSET);
	    this.protocolString = bigE.getFixedByteArray(buffer, PROTOCOL_STRING_OFFSET, 4);
	}
	
	/**
	 * @see java.lang.Object#toString()
	 */
	public String toString()
	{
		return "msg-" + msgName() + "["
			+ "requester:" + requesterAddress.toString() + "," 
			+ "req. boot:" + requesterBootTime + "," 
			+ "proto:" + String.valueOf(protocolString) + "]";	
	}
	
	protected static final int REQUESTER_ADDRESS_OFFSET = 0;
	protected static final int REQUESTER_BOOT_TIME_OFFSET = 4;
	protected static final int PROTOCOL_STRING_OFFSET = 8;
	public static final int MSG_LENGTH = 12;	
	/**
	 * @see org.ipdr.protocol.ProtocolMsgIdentification#msgName()
	 */
	public String msgName()
	{
		return "VERSION_REQUEST";
	}

    /**
     * @return Returns the requesterAddress.
     */
    public InetAddress getRequesterAddress()
    {
        return this.requesterAddress;
    }
}
