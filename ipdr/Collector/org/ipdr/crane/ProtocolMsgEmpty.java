package org.ipdr.crane;

// $Id: ProtocolMsgEmpty.java,v 1.3 2005/01/30 15:13:46 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.net.UnknownHostException;

/** The Java encapsulation of an empty message (typically produced by a timeout
 *  event when reading).
 *
 *  @version $Id: ProtocolMsgEmpty.java,v 1.3 2005/01/30 15:13:46 ilyad Exp $
 */
public final class ProtocolMsgEmpty extends ProtocolMsg
{
    /** $Id: ProtocolMsgEmpty.java,v 1.3 2005/01/30 15:13:46 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: ProtocolMsgEmpty.java,v 1.3 2005/01/30 15:13:46 ilyad Exp $"; }

    private Exception exception;

    /** Create an empty message.
     *  @param exception The exception that caused this message
     *           to be created -- typically a timeout, but can also
     *           be null (to indicate no reason).
     */
    public ProtocolMsgEmpty(Exception exception) {
        this.exception = exception;
    }

    public ProtocolMsgEmpty() {
        this.exception = null;
    }

    public String msgName() { return "<empty>"; }
    public final byte msgId() { return -1; }

    public Exception getException() { return exception; }

    /** Create a binary message.
     *  @exception UnsupportedOperationException always
     */
    protected byte [] createRawMsg() throws UnknownHostException {
        byte [] result = new byte[HDR_LENGTH];
        setHeader(result);
        unimplemented(this, "createRawMsg"); // die if not over-ridden
        return result;
    }
}
