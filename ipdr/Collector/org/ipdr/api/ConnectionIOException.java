package org.ipdr.api;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import org.ipdr.api.configuration.ConnectionConfigI;

/**
 * Exception thrown when an I/O error happens during reading or writing on the
 * TCP connection. Contains extra context information about what was going on,
 * which helps in understanding the problem.
 * 
 * @see java.net.UnknownHostException
 * @see java.net.ConnectException
 * @see java.io.IOException
 * @see java.net.SocketException
 *  
 */
public final class ConnectionIOException extends java.io.IOException implements
        java.io.Serializable
{

    /// What went wrong.
    private Throwable cause;

    /// The message about what went wrong.
    private String    originalMsg;

    /**
     * Create an exception, with a message, the connnection configuration, and
     * the underlying error.
     * 
     * @param msg
     *            What went wrong.
     * @param connectionConfig
     *            The configuration for the configuration (used to get the
     *            transmitter's IP address and other such stuff).
     * @param cause
     *            The I/O exception.
     */
    public ConnectionIOException(String msg,
            ConnectionConfigI connectionConfig, Throwable cause)
    {
        super(msg + " on connection " + connectionConfig + ": " + cause);
        this.cause = cause;
        this.originalMsg = msg;
    }

    /**
     * Create an exception, with a message and the underlying error.
     * 
     * @param msg
     *            What went wrong
     * @param cause
     *            The I/O Exception
     */
    public ConnectionIOException(String msg, Throwable cause)
    {
        super(msg + " on connection " + cause);
        this.cause = cause;
        this.originalMsg = msg;
    }

    /**
     * Get the underlying cause.
     * 
     * @return The cause.
     */
    public Throwable getCause()
    {
        return cause;
    }

    /**
     * Get the original message (without the extra stuff from the connection et
     * al).
     * 
     * @return What was given to the constructor.
     */
    public String getOriginalMsg()
    {
        return originalMsg;
    }

    /**
     * @see java.lang.Throwable#printStackTrace(java.io.PrintWriter)
     */
    public void printStackTrace(java.io.PrintWriter s)
    {
        if (cause == null)
        {
            super.printStackTrace(s);
        }
        else
        {
            cause.printStackTrace(s);
        }
    }
}