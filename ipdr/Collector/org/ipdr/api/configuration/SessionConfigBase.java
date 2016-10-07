package org.ipdr.api.configuration;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.util.ArrayList;


/**
 * The class provides a convenient base for <code>SessionConfigI</code>
 * implementations
 */

public abstract class SessionConfigBase implements SessionConfigI
{
    /**
     * Holds parent connection
     */
    protected ConnectionConfigI connection;

    /**
     * Holds session description
     */
    protected String sessionDescription;

    /**
     * Holds session id
     */
    protected byte sessionId;

    /**
     * Holds session name
     */
    protected String sessionName;

    /**
     * Holds list of session templates
     */
    protected ArrayList sessionTemplatesList = new ArrayList();

    /**
     * Initializes the object with given parent connection
     * @param connection Parent <code>ConnectionConfigI</code> 
     */
    protected SessionConfigBase(ConnectionConfigI connection)
    {
        this.connection = connection;
    }

    /**
     * @see org.ipdr.api.configuration.SessionConfigI#getConnectionConfig()
     */
    public ConnectionConfigI getConnectionConfig()
    {
        return connection;
    }

    /**
     * @see org.ipdr.api.configuration.SessionConfigI#getSessionDescription()
     */
    public String getSessionDescription()
    {
        return sessionDescription;
    }

    /**
     * @see org.ipdr.api.configuration.SessionConfigI#getSessionId()
     */
    public byte getSessionId()
    {
        return sessionId;
    }

    /**
     * @see org.ipdr.api.configuration.SessionConfigI#getSessionName()
     */
    public String getSessionName()
    {
        return sessionName;
    }

    /**
     * @see org.ipdr.api.configuration.SessionConfigI#getSessionTemplatesList()
     */
    public ArrayList getSessionTemplatesList()
    {
        return sessionTemplatesList;
    }

    /**
     * Returns session id as hash code for the object
     * @see java.lang.Object#hashCode()
     */
    public int hashCode()
    {
        return (int) sessionId;
    }

}