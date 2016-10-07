package org.ipdr.protocol;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/**
 * The exception for giving information about a mal-formed message. This is used
 * for reporting cases where the received message is in a garbled form.
 */
public final class ProtocolMalformedMsgException extends Exception
{
    /**
     * The message raw bytes (including header).
     */
    public byte[] rawMsg;

    /**
     * The message raw bytes length.
     */
    public int rawMsgLength;

    /**
     * The offset within the message bytes where the problem was detected.
     */
    public int offset;

    /**
     * Create an exception (should never happen if the transmitter is behaving
     * properly).
     * 
     * @param msg - What went wrong.
     * @param rawMsg - The CRANE protocol message from the transmitter.
     * @param length - The length of the CRANE protocol message.
     * @param offset - Offset of where the problem was discovered inside the CRANE protocol message.
     */
    public ProtocolMalformedMsgException(String msg, byte[] rawMsg, int length,
            int offset)
    {
        super(msg);
        this.rawMsg = rawMsg;
        this.rawMsgLength = length;
        this.offset = offset;
    }

    /**
     * Get the detail messages about this exception.
     * 
     * @return The detail message.
     */
    public String getMessage()
    {
        if (this.rawMsg.length == 0 && this.rawMsgLength == 0
                && this.offset == 0)
        {
            return super.getMessage();
        }
        else
        {
            return super.getMessage()
                    + " msg.length="
                    + this.rawMsg.length
                    + ((this.rawMsg.length == this.rawMsgLength) ? ""
                            : " length=" + this.rawMsgLength)
                    + ((this.offset == 0) ? "" : " at offset " + this.offset);
        }
    }
}