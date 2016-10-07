package org.ipdr.protocol;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/**
 * Indicates invalid state transition or message for a session.
 */
public class InvalidSessionStateException extends SessionException
{
	/**
	 * Constructor for InvalidStateException.
	 * @param sessionId - session id where the trouble is
	 */
	public InvalidSessionStateException(byte sessionId)
	{
		super(sessionId);
	}

	/**
	 * Constructor for InvalidStateException.
	 * @param sessionId - session id of the session which caused troubles
	 * @param message  - descriptive message
	 */
	public InvalidSessionStateException(byte sessionId, String message)
	{
		super(sessionId, message);
	}

}
