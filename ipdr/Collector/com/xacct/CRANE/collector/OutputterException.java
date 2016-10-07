package com.xacct.CRANE.collector;

// $Id: OutputterException.java,v 1.4 2005/01/30 15:13:43 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/** Exception thrown when there's an error in outputting data.
 *  @see CollectorOutputter
 *
 *  @version $Id: OutputterException.java,v 1.4 2005/01/30 15:13:43 ilyad Exp $
 */
public final class OutputterException extends Exception implements java.io.Serializable
{
    /** $Id: OutputterException.java,v 1.4 2005/01/30 15:13:43 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: OutputterException.java,v 1.4 2005/01/30 15:13:43 ilyad Exp $"; }

    /** Create the exception, from a single string.
     * @param msg What went wrong.
     */
    public OutputterException(String msg) { super(msg); }

    /** Create the exception, from another exception.
     * @param Exception what went wrong.
     */
    public OutputterException(Throwable cause) {
        // XXX JDK 1.4, we can do super(cause);
        super(cause == null ? (String)null : cause.toString());
    }

    /** Create the exception from the object (e.g., the running thread),
     *  the function (or method) in the object, and the message.
     * @param o The object (e.g., the running thread).
     * @param function The function (or method) in the object.
     * @param msg The message about what went wrong.
     */
    public OutputterException(Object o, String function, String msg) {
        super(msg + " (in " + o.getClass().getName() + "." + function + ")");
    }
}
