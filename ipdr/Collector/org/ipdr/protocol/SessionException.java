package org.ipdr.protocol;

//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

public abstract class SessionException extends Exception
{
	protected byte sessionId;

	/**
	 * Constructor for SessionException
	 * 
	 * @param sessionId
	 */
	protected SessionException( byte sessionId )
	{
		super();
		this.sessionId = sessionId;
	}
	
	/**
	 * Constructor for SessionException
	 * 
	 * @param sessionId
	 * @param message
	 */
	protected SessionException( byte sessionId, String message )
	{
		super(message);
		this.sessionId = sessionId;
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
