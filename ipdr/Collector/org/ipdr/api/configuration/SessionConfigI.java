package org.ipdr.api.configuration;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.util.ArrayList;

/**
 * This interface provides configuration of a Session
 *  
 */
public interface SessionConfigI
{

    /**
     * Returns parent connection config
     * @return <code>ConnectionConfigI</code> of parent connection
     */
    public ConnectionConfigI getConnectionConfig();

    /**
     * Returns verbal description of the session
     * @return The description
     */
    public String getSessionDescription();
    /**
     * Returns session id
     * @return session id
     */
    public byte getSessionId();

    /**
     * Returns session name
     * @return session name
     */
    public String getSessionName();

    /**
     * Returns list of templates configured for the session 
     * @return <code>ArrayList</code> of templates
     */
    public ArrayList getSessionTemplatesList();
}