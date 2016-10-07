package com.xacct.CRANE.collector;

// $Id: LogicException.java,v 1.4 2005/01/30 15:13:42 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/** Exception thrown when there's some kind of logic inconsistency in
 *  the program. Should never happen, of course ... it's a kind of
 *  assertion. This is a subclass of RuntimeException, so that it
 *  doesn't need to be specified everywhere.
 *
 *  @version $Id: LogicException.java,v 1.4 2005/01/30 15:13:42 ilyad Exp $
 */
public final class LogicException extends RuntimeException implements java.io.Serializable
{
    /** $Id: LogicException.java,v 1.4 2005/01/30 15:13:42 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: LogicException.java,v 1.4 2005/01/30 15:13:42 ilyad Exp $"; }

    private Throwable cause;

    /** Create the exception, from a single string.
     *  @param msg What went wrong.
     */
    public LogicException(String msg) { super(msg); }

    /** Create the exception, form another exception.
     *  @param cause The unexpected exception.
     */
    public LogicException(Throwable cause) {
        // XXX JDK 1.4, we can do super(cause);
        super(cause.toString());
        this.cause = cause;
        // XXX needs JDK 1.4: this.setStackTrace(cause.getStackTrace());
    }

    /** Create the exception, from the object (e.g., the running thread),
     *  the function (or method) in the object, and the message.
     * @param o The object (e.g., the running thread).
     * @param function The function (or method) in the object.
     * @param msg The message about what went wrong.
     */
    public LogicException(Object o, String function, String msg) {
        super(msg + " (in " + o.getClass().getName() + "." + function + ")");
    }

    // override
    public void printStackTrace(java.io.PrintWriter s) {
        if (cause == null) {
            super.printStackTrace(s);
        } else {
            cause.printStackTrace(s);
        }
    }
}
