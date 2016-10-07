package com.xacct.CRANE.collector;

// $Id: DataException.java,v 1.4 2005/01/30 15:13:39 ilyad Exp $
// Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/** Exception thrown when there's an error in the protocol data, such
 *  as didn't get an expected message or got an invalid session or
 *  template id. Maybe this should have been called
 *  ProtocolException. Not to be confused with
 *  ProtocolMalformedMsgException, which is used when the message
 *  itself is messed up and it would be useful to have a dump of the
 *  message bytes to figure out what's wrong.
 *  @see ProtocolMalformedMsgException
 *
 *  @version $Id: DataException.java,v 1.4 2005/01/30 15:13:39 ilyad Exp $
 */
public final class DataException extends Exception implements java.io.Serializable
{
    /** $Id: DataException.java,v 1.4 2005/01/30 15:13:39 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: DataException.java,v 1.4 2005/01/30 15:13:39 ilyad Exp $"; }

    /** Create the exception, from a single string.
     * @param msg What went wrong.
     */
    public DataException(String msg) { super(msg); }

    /** Create the exception from the object (e.g., the running thread),
     *  the function (or method) in the object, and the message.
     * @param o The object (e.g., the running thread).
     * @param function The function (or method) in the object.
     * @param msg The message about what went wrong.
     */
    public DataException(Object o, String function, String msg) {
        super(msg + " (in " + o.getClass().getName() + "." + function + ")");
    }
}
