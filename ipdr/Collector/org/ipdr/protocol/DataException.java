package org.ipdr.protocol;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/**
 * Exception thrown when there's an error in the protocol data. Not to be
 * confused with ProtocolMalformedMsgException, which is used when the message
 * itself is messed up.
 * 
 * @see ProtocolMalformedMsgException
 *  
 */
public final class DataException extends Exception implements
        java.io.Serializable
{
    /**
     * Create the exception, from a single string.
     * @param msg - what went wrong.
     */
    public DataException(String msg)
    {
        super(msg);
    }

    /**
     * Create the exception from the object (e.g., the running thread), the
     * function (or method) in the object, and the message. 
     * @param o - the object (e.g., the running thread).
     * @param function - the function (or method) in the object.
     * @param msg - the message about what went wrong.
     */
    public DataException(Object o, String function, String msg)
    {
        super(msg + " (in " + o.getClass().getName() + "." + function + ")");
    }
}