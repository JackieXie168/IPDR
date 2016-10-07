package com.xacct.CRANE.collector;

// $Id: ProtocolMsgInfoResponse.java,v 1.4 2005/01/30 15:13:40 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.


/** The Java encapsulation of a CRANE protocol info response message.
 *
 *  <xmp>
 *        0                   1                   2                   3
 *        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                      Default Protocol Info                    |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                    Additional Protocols Count                 |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                    Additional Protocols Info                  |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |              ...   Additional Protocols Info  ...             |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                    Additional Protocols Info                  |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  </xmp>
 *  Protocol Info Block
 *  <xmp>
 *        0                   1                   2                   3
 *        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                         Transport Type                        |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                        Protocol Version                       |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |         Port Number           |            Reserved           |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   </xmp>
 *  This is <em>not</em> a subclass of ProtocolMsg because it
 *  it is laid out differently and is used as part of the initial
 *  UDP-based version negotiation.
 *
 *  @see ProtocolMsgInfoRequest
 *
 *  @version $Id: ProtocolMsgInfoResponse.java,v 1.4 2005/01/30 15:13:40 ilyad Exp $
 */
public final class ProtocolMsgInfoResponse implements ProtocolMsgIdentification
{
    /** $Id: ProtocolMsgInfoResponse.java,v 1.4 2005/01/30 15:13:40 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: ProtocolMsgInfoResponse.java,v 1.4 2005/01/30 15:13:40 ilyad Exp $"; }

    /// The message type's name
    public String msgName() { return "ProtocolInfoResponse"; }

    /// The information block(s)
    ProtocolInfoBlock [] info;

    /** Construct from raw bytes
     *  @param msg The raw message bytes
     *  @param length The number of bytes in the message
     */
    public ProtocolMsgInfoResponse(byte [] msg, int length) throws ProtocolMalformedMsgException {
        ProtocolMsg.verifyLength(this, msg, length, 16);
        try {
            long additionalCount = ProtocolMsg.bigE.getUnsignedInt(msg, 12);
            info = new ProtocolInfoBlock[(int)additionalCount+1];
            int offset = 0;
            for (int i = 0; i < info.length; i++) {
                info[i] = new ProtocolInfoBlock(
                    ProtocolMsg.bigE.getInt(msg, offset),      // transportType
                    ProtocolMsg.bigE.getInt(msg, offset+4),    // protocolVersion
                    ProtocolMsg.bigE.getShort(msg, offset+8)); // portNumber
                offset += (offset == 0) ? 12 : 16;
            }
        } catch (IndexOutOfBoundsException e) {
            CollectorLog.stackTrace(e);
            String detail = "Protocol Info Response message length of buffer (" + length + ") shorter than message contents";
            if (msg.length != length) {
                detail += " - with buffer containing " + msg.length + " bytes";
            }
            detail += " ... " + e;
            throw new ProtocolMalformedMsgException(detail, msg, length, 0);
        }
    }

    /** Create a ProtocolInfoResponseMsg for sending
     *  XXX unimplemented
     *  @param info a vector of information blocks
     *  @return the raw message bytes, for sending
     */
    public static final byte [] createRawMsg(ProtocolInfoBlock [] info) throws UnsupportedOperationException {
        ProtocolMsg.unimplemented("ProtocolMsgInfoResponse", "createRawMsg");
        return new byte[0];
    }

    public String toString() {
        String result = "msg-" + msgName() + "[";
        String sep = "";
        for (int i = 0; i < info.length; i++) {
            result += sep + "type=" + info[i].transportType +
                " version=" + info[i].protocolVersion +
                " port=" + info[i].portNumber;
            sep = "; ";
        }
        return result + "]";
    }

    /// CRANE Protocol Info Block : Transport Type = TCP
    public final static int INFO_TRANSPORT_TCP = 1;

    /// CRANE Protocol Info Block : Transport Type = SCTP
    public final static int INFO_TRANSPORT_SCTP = 2;
}
