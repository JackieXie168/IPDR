package org.ipdr.protocol;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/**
 * Represents session-level exception for situations when exporter and/or collector capabilites are invalid
 *
 */
public class InvalidCapabilities extends SessionException
{

	/**
	 * Constructor for InvalidCapabilities.
	 * @param sessionId - the session ID
	 */
	public InvalidCapabilities(byte sessionId)
	{
		super(sessionId);
	}

	/**
	 * Constructor for InvalidCapabilities.
	 * @param sessionId - the session ID
	 * @param message - the detail message
	 */
	public InvalidCapabilities(byte sessionId, String message)
	{
		super(sessionId, message);
	}

}
