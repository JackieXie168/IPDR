package org.ipdr.protocol;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/**
 * The class is a common ancestor for all IPDR protocol messages which contain only the header
 * Consult <code>ProtocolMsg</code> for exact header structure.
 * @see ProtocolMsg
 */
public abstract class ProtocolMsgHeaderOnly extends ProtocolStreamMsg
{
	/**
	 * Creates message from raw bytes
	 * @param msg Raw bytes
	 * @param length Length 
	 * @param requiredLength Required length
	 * @throws ProtocolMalformedMsgException
	 */
	protected ProtocolMsgHeaderOnly(byte[] msg, int length, int requiredLength)
		throws ProtocolMalformedMsgException
		{
			super( msg, length, requiredLength );
		}
		
	/**
	 * Creates message from values. Note that we only require message id and 
	 * session id to construct ANY of this class' children.
	 * @param msgId message id
	 * @param sessionId session id
	 * @param msgLength required length
	 */
	protected ProtocolMsgHeaderOnly(byte msgId, byte sessionId, int msgLength)
	{
		super( msgId, sessionId, msgLength);
	}

	
	/**
	 * @see org.ipdr.protocol.ProtocolMsg#createRawMsg()
	 */
	public final byte[] createRawMsg() 
	{
        byte [] msgData = new byte[ProtocolStreamMsg.HDR_LENGTH ];
        this.msgLength = ProtocolStreamMsg.HDR_LENGTH;
        setHeader(msgData);
        return msgData;	
	}
	
	/**
	 * @see java.lang.Object#toString()
	 */
	public String toString()
	{
		return toStringBase();
	}

}
