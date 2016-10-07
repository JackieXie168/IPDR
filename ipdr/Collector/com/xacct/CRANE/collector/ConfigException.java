package com.xacct.CRANE.collector;

// $Id: ConfigException.java,v 1.4 2005/01/30 15:13:43 ilyad Exp $
// Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/** Exception thrown when there's an error in the configuration data.
 *
 *  @version $Id: ConfigException.java,v 1.4 2005/01/30 15:13:43 ilyad Exp $
 */
public final class ConfigException extends Exception implements java.io.Serializable
{
    /** $Id: ConfigException.java,v 1.4 2005/01/30 15:13:43 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: ConfigException.java,v 1.4 2005/01/30 15:13:43 ilyad Exp $"; }

    /** Create the exception, from a single string.
     *  @param msg The detail message.
     */
    public ConfigException(String msg) { super(msg); }

    /** Create the exception, from another exception (wrapper).
     *  @param ex The other exception.
     */
    public ConfigException(Throwable cause) {
        // XXX JDK 1.4, we can do super(cause);
        super(cause == null ? (String)null : cause.toString());
    }

    /** Create the exception, from the object (e.g., the running thread),
     * the function (or method) in the object, and the message.
     * @param o The object (e.g., the running thread).
     * @param function The function (or method) in the object.
     * @param msg The detail message about what went wrong.
     */
    public ConfigException(Object o, String function, String msg) {
        super(msg + " (in " + o.getClass().getName() + "." + function + ")");
    }
}
