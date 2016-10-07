package org.ipdr.protocol;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.net.UnknownHostException;

/**
 * The class represents a common parent for all protocol messages
 *  
 */
public abstract class ProtocolMsg implements ProtocolMsgIdentification
{

    /**
     * Big-endian data extractor
     */
    public static final ProtocolDataBigEndian bigE = new ProtocolDataBigEndian();

    /**
     * The message id (defines what type of record it is)
     */
    public short hdrMsgId = (short) 0xff;

    /**
     * Create a raw (output) message. This method is normally overridden by a
     * subclass. Do <em>not</em> call this by
     * <code>super.createRawMsg()</code> ... instead, just do
     * <code>setHeader(...</code> to set the result's header.
     * 
     * @return the raw message bytes, for sending
     */
    abstract protected byte[] createRawMsg() throws UnknownHostException;

    /**
     * Description of the message type (this is over-ridden by sub-classes)
     * 
     * @return The message type, as a string
     */
    public abstract String msgName();

    /**
     * @see java.lang.Object#toString()
     */
    public abstract String toString();

}