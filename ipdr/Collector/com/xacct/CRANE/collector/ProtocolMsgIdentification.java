package com.xacct.CRANE.collector;

// $Id: ProtocolMsgIdentification.java,v 1.4 2005/01/30 15:13:43 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/** Some base methods for all protocol messages: used to dispaly
 *  information about them.
 *
 *  @version $Id: ProtocolMsgIdentification.java,v 1.4 2005/01/30 15:13:43 ilyad Exp $
 */
public interface ProtocolMsgIdentification
{
    /** The message type's name
     *  @return The message type, as a string
     */
    public String msgName();
}
