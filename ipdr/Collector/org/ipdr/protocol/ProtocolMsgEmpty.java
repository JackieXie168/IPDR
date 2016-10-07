package org.ipdr.protocol;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/**
 * The Java encapsulation of an empty message (typically produced by a timeout
 * event when reading).
 * 
 */
public final class ProtocolMsgEmpty extends ProtocolStreamMsg
{
    private Exception exception;

    /**
     * Create an empty message.
     * 
     * @param exception
     *            The exception that caused this message to be created --
     *            typically a timeout, but can also be null (to indicate no
     *            reason).
     */
    public ProtocolMsgEmpty(Exception exception)
    {
        this.exception = exception;
    }

    /**
     * Sets exception parameter to null 
     */
    public ProtocolMsgEmpty()
    {
        this.exception = null;
    }

    public String msgName()
    {
        return "<empty>";
    }

    public static final byte MSG_ID = -1;

    public final byte msgId()
    {
        return -1;
    }

    /**
     * Returns the exception
     * 
     * @return Exception
     */
    public Exception getException()
    {
        return exception;
    }

    /**
     * Create a binary message.
     * 
     * @exception UnsupportedOperationException
     *                always
     */
    protected byte[] createRawMsg()
    {
        byte[] result = new byte[HDR_LENGTH];
        setHeader(result);
        unimplemented(this, "createRawMsg"); // die if not over-ridden
        return result;
    }

    /**
     * @see java.lang.Object#toString()
     */
    public String toString()
    {
        return "EMPTY MESSAGE";
    }

}