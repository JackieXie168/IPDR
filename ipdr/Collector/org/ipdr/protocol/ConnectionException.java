package org.ipdr.protocol;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/**
 * Represents a connection-level exception
 */
public class ConnectionException extends Exception
{

	/**
	 * Constructor for ConnectionException.
	 */
	public ConnectionException()
	{
		super();
	}

	/**
	 * Constructor for ConnectionException.
	 * @param message - the detail message.
	 */
	public ConnectionException(String message)
	{
		super(message);
	}

	/**
	 * Constructor for ConnectionException.
	 * @param message - the detail message. 
	 * @param cause - the cause. 
	 */
	public ConnectionException(String message, Throwable cause)
	{
		super(message, cause);
	}

	/**
	 * Constructor for ConnectionException.
	 * @param cause - the cause.
	 */
	public ConnectionException(Throwable cause)
	{
		super(cause);
	}

}
