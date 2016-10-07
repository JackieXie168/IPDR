package org.ipdr.protocol;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/**
 * Represents session-level exception upon error message sent by peer
 *
 */
public class PeerSessionErrorException extends SessionException
{
	/**
	 * Holds error reason code as sent by peer
	 */
	protected short peerReasonCode;
	/**
	 * Holds textual description as sent by peer
	 */
	protected String peerMessage;
	
	/**
	 * Initializes the object
	 * @param sessionId - session id with the exception
	 * @param peerReasonCode - peer reason code
	 * @param peerMessage - peer error message
	 */
	public PeerSessionErrorException( byte sessionId, short peerReasonCode, String peerMessage )
	{
		super( sessionId );
		this.peerReasonCode = peerReasonCode;
		this.peerMessage = peerMessage;
	}

}
