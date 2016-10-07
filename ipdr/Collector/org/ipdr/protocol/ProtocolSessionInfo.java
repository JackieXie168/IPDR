package org.ipdr.protocol;

import java.util.ArrayList;


//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/**
 * Represents a session info block for GetSessions message
 *
 */
public class ProtocolSessionInfo
{
	/**
	 * Holds session id
	 */
	protected byte sessionId;
	/**
	 * Holds the reserved byte field
	 */
	protected byte reserved;
	/**
	 * Holds session name
	 */
	protected String sessionName;
	/**
	 * Holds session description
	 */
	protected String sessionDescription;
	/**
	 * Holds acknowledge time interval
	 */
	protected int ackTimeInterval;
	/**
	 * Holds acknowledge sequence interval (number of data messages to send the acknowledge after)
	 */
	protected int ackSequenceInterval;
	
	ArrayList templates;
	
	public ProtocolSessionInfo(){}
	
	/**
	 * Returns the ackSequenceInterval.
	 * @return int
	 */
	public int getAckSequenceInterval()
	{
		return ackSequenceInterval;
	}

	/**
	 * Returns the ackTimeInterval.
	 * @return int
	 */
	public int getAckTimeInterval()
	{
		return ackTimeInterval;
	}

	/**
	 * Returns the reserved.
	 * @return byte
	 */
	public byte getReserved()
	{
		return reserved;
	}

	/**
	 * Returns the sessionDescription.
	 * @return String
	 */
	public String getSessionDescription()
	{
		return (sessionDescription == null ? "" : sessionDescription);
	}

	/**
	 * Returns the sessionId.
	 * @return byte
	 */
	public byte getSessionId()
	{
		return sessionId;
	}

	/**
	 * Returns the sessionName.
	 * @return String
	 */
	public String getSessionName()
	{
		return sessionName;
	}

	/**
	 * Sets the ackSequenceInterval.
	 * @param ackSequenceInterval The ackSequenceInterval to set
	 */
	public void setAckSequenceInterval(int ackSequenceInterval)
	{
		this.ackSequenceInterval = ackSequenceInterval;
	}

	/**
	 * Sets the ackTimeInterval.
	 * @param ackTimeInterval The ackTimeInterval to set
	 */
	public void setAckTimeInterval(int ackTimeInterval)
	{
		this.ackTimeInterval = ackTimeInterval;
	}

	/**
	 * Sets the reserved.
	 * @param reserved The reserved to set
	 */
	public void setReserved(byte reserved)
	{
		this.reserved = reserved;
	}

	/**
	 * Sets the sessionDescription.
	 * @param sessionDescription The sessionDescription to set
	 */
	public void setSessionDescription(String sessionDescription)
	{
		this.sessionDescription = sessionDescription;
	}

	/**
	 * Sets the sessionId.
	 * @param sessionId The sessionId to set
	 */
	public void setSessionId(byte sessionId)
	{
		this.sessionId = sessionId;
	}

	/**
	 * Sets the sessionName.
	 * @param sessionName The sessionName to set
	 */
	public void setSessionName(String sessionName)
	{
		this.sessionName = sessionName;
	}
    public ArrayList getTemplates()
    {
        return templates;
    }
    public void setTemplates(ArrayList templates)
    {
        this.templates = templates;
    }
}
