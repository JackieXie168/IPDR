package com.xacct.CRANE.collector;

// $Id: ConnectionIOException.java,v 1.4 2005/01/30 15:13:39 ilyad Exp $
// Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/** Exception thrown when an I/O error happens during reading or writing
 *  on the TCP connection. Contains extra context information about
 *  what was going on, which helps in understanding the problem.
 *  @see java.net.UnknownHostException
 *  @see java.net.ConnectException
 *  @see java.io.IOException
 *  @see java.net.SocketException
 *
 *  @version $Id: ConnectionIOException.java,v 1.4 2005/01/30 15:13:39 ilyad Exp $
 */
public final class ConnectionIOException
    extends java.io.IOException
    implements java.io.Serializable {

    /** $Id: ConnectionIOException.java,v 1.4 2005/01/30 15:13:39 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: ConnectionIOException.java,v 1.4 2005/01/30 15:13:39 ilyad Exp $"; }

    /// What went wrong.
    private Throwable cause;

    /// The message about what went wrong.
    private String originalMsg;

    /** Create an exception, with a message, the connnection
     *  configuration, and the underlying error.
     * @param msg What went wrong.
     * @param connectionConfig The configuration for the configuration
     *         (used to get the transmitter's IP address and other such stuff).
     * @param cause The I/O exception.
     */
    public ConnectionIOException(String msg, ConnectionConfig connectionConfig, Throwable cause) {
        super(msg + " on connection " + connectionConfig + ": " + cause);
        this.cause = cause;
        this.originalMsg = msg;
    }

    /** Create an exception, with a message, the connnection
     *  configuration, and the underlying error.
     * @param msg What went wrong.
     * @param connection The connnection (used to get the transmitter's IP
     * address and other such stuff).
     * @param cause What went wrong.
     */
    public ConnectionIOException(String msg, ConnectionDynamic connection, Throwable cause) {
        this(msg, connection.getConnectionConfig(), cause);
    }

    /** Get the underlying cause.
     * @return The cause.
     */
    public Throwable getCause() { return cause; }

    /** Get the original message (without the extra stuff from the connection et al).
     * @return What was given to the constructor.
     */
    public String getOriginalMsg() { return originalMsg; }


    // override
    public void printStackTrace(java.io.PrintWriter s) {
        if (cause == null) {
            super.printStackTrace(s);
        } else {
            cause.printStackTrace(s);
        }
    }
}
