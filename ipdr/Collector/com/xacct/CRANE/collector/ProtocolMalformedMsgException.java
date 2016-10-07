package com.xacct.CRANE.collector;

// $Id: ProtocolMalformedMsgException.java,v 1.4 2005/01/30 15:13:40 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/** The exception for giving information about a mal-formed
 *  message. This is used for reporting cases where the received
 *  message is in a garbled form. This exception should be used when
 *  the actual bytes in the message will be useful to figure out what
 *  went wrong; if the problem can be expressed by "I expected field
 *  xx to have the value 0x01 but instead it had 0x02", then use Data
 *  Exception. (However, there are exceptions: if the protocol version
 *  field is wrong, then a ProtocolMalformedMsgException is more
 *  appropriate, because we'll probably need a dump of the message to
 *  figure out what's wrong.)
 *  @see DataException
 *
 *  @version $Id: ProtocolMalformedMsgException.java,v 1.4 2005/01/30 15:13:40 ilyad Exp $
 */
public final class ProtocolMalformedMsgException extends Exception
{
    /** $Id: ProtocolMalformedMsgException.java,v 1.4 2005/01/30 15:13:40 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: ProtocolMalformedMsgException.java,v 1.4 2005/01/30 15:13:40 ilyad Exp $"; }

    /// The message raw bytes (including header).
    public byte [] rawMsg;

    /// The message raw bytes length.
    public int rawMsgLength;

    /// The offset within the message bytes where the problem was detected.
    public int offset;

    /** Create an exception (should never happen if the transmitter is behaving properly).
     * @param msg What went wrong.
     * @param rawMsg The CRANE protocol message from the transmitter.
     * @param length The length of the CRANE protocol message.
     * @param offset Offset of where the problem was discovered inside the CRANE protocol message.
     */
    public ProtocolMalformedMsgException(String msg, byte [] rawMsg, int length, int offset) {
        super(msg);
        this.rawMsg       = rawMsg;
        this.rawMsgLength = length;
        this.offset       = offset;
    }

    /** Get the detail messages about this exception.
     * @return The detail message.
     */
    public String getMessage() {
        if (this.rawMsg.length == 0 &&
            this.rawMsgLength == 0 &&
            this.offset == 0) {
            return super.getMessage();
        } else {
            return super.getMessage() + " msg.length=" + this.rawMsg.length +
                ((this.rawMsg.length == this.rawMsgLength) ? "" : " length=" + this.rawMsgLength) +
                ((this.offset == 0) ? "" : " at offset " + this.offset);
        }
    }
}
