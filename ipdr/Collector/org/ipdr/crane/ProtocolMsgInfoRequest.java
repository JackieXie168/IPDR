package org.ipdr.crane;

// $Id: ProtocolMsgInfoRequest.java,v 1.4 2005/01/30 15:13:45 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.net.Inet4Address;
import java.net.UnknownHostException;
import java.util.Date;

import org.ipdr.api.log.LogManager;
import org.ipdr.protocol.ProtocolMalformedMsgException;

import com.xacct.CRANE.collector.CollectorUtil;

/** The Java encapsulation of a CRANE protocol info request message.
 *
 *  <xmp>
 *        0                   1                   2                   3
 *        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                         Server Address                        |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                        Server Boot Time                       |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |     'C'       |    'R'        |    'A'        |    'N'        |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  </xmp>
 *  <p>
 *  (Server = Collector)
 *  <p>
 *  This is <em>not</em> a subclass of ProtocolMsg because it
 *  it is laid out differently and is used as part of the initial
 *  UDP-based version negotiation.
 *  @see ProtocolMsgInfoResponse
 *
 *  @version $Id: ProtocolMsgInfoRequest.java,v 1.4 2005/01/30 15:13:45 ilyad Exp $
 */
public final class ProtocolMsgInfoRequest implements ProtocolMsgIdentification
{
    /** $Id: ProtocolMsgInfoRequest.java,v 1.4 2005/01/30 15:13:45 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: ProtocolMsgInfoRequest.java,v 1.4 2005/01/30 15:13:45 ilyad Exp $"; }

    /// The message type's name
    public String msgName() { return "ProtocolInfoRequest"; }

    /// Server (collector) address
    Inet4Address collector;

    /// Server (collector) boot time
    Date collectorBootTime;

    /// Request identifier: 'C','R','A','N'
    byte [] requestEyecatcher;

    /** Construct from raw bytes
     *  @param msg The raw message bytes
     *  @param length The number of bytes in the message
     */
    public ProtocolMsgInfoRequest(byte [] msg, int length)
        throws ProtocolMalformedMsgException, UnknownHostException {
        ProtocolMsg.verifyLength(this, msg, length, 12);
        try {
            this.collector = ProtocolMsg.bigE.getAddr4(msg, 0);
            this.collectorBootTime = ProtocolMsg.bigE.getDate(msg, 4);
            this.requestEyecatcher = new byte[4];
            this.requestEyecatcher[0] = ProtocolMsg.bigE.getByte(msg, 8+0);
            this.requestEyecatcher[1] = ProtocolMsg.bigE.getByte(msg, 8+1);
            this.requestEyecatcher[2] = ProtocolMsg.bigE.getByte(msg, 8+2);
            this.requestEyecatcher[3] = ProtocolMsg.bigE.getByte(msg, 8+3);
            if (this.requestEyecatcher[0] != 'C' ||
                this.requestEyecatcher[1] != 'R' ||
                this.requestEyecatcher[2] != 'A' ||
                this.requestEyecatcher[3] != 'N') {
                throw new ProtocolMalformedMsgException(
                    msgName() + " message eyecatcher not 'CRAN': '" +
                    (char)(this.requestEyecatcher[0]) +
                    (char)(this.requestEyecatcher[1]) +
                    (char)(this.requestEyecatcher[2]) +
                    (char)(this.requestEyecatcher[3]) + "'",
                    msg, length, 8);
            }
        } catch (IndexOutOfBoundsException e) {
            LogManager.stackTrace(e);
            String detail = msgName() + " message length of buffer (" + length + ") shorter than message contents";
            if (msg.length != length) {
                detail += " - with buffer containing " + msg.length + " bytes";
            }
            detail += " ... " + e;
            throw new ProtocolMalformedMsgException(detail, msg, length, 0);
        }
    }

    public String toString() {
        return "msg-" + msgName() + "['" +
            (char)(this.requestEyecatcher[0]) +
            (char)(this.requestEyecatcher[1]) +
            (char)(this.requestEyecatcher[2]) +
            (char)(this.requestEyecatcher[3]) +
            "' " +
            this.collector +
            " bootTime: " + CollectorUtil.dateFormat(this.collectorBootTime) +
            "]";
    }

    /** Create a ProtocolInfoRequestMsg
     *  @param localHost The local host (server: collector) -- typically <code>ExtendedInetAddress.getLocalHost().get()</code>
     *  @param bootTime The time when the server (collector) started
     */
    public ProtocolMsgInfoRequest(Inet4Address localHost, Date bootTime) {
        this.collector = localHost;
        this.collectorBootTime = bootTime;
        this.requestEyecatcher = new byte[4];
        this.requestEyecatcher[0] = (byte)'C';
        this.requestEyecatcher[1] = (byte)'R';
        this.requestEyecatcher[2] = (byte)'A';
        this.requestEyecatcher[3] = (byte)'N';
    }

    /** Create a ProtocolInfoRequestMsg for sending
     * @return <<Description>>
     * @throws UnknownHostException <<Description>>
     */
    public final byte [] createRawMsg() throws UnknownHostException {
        byte [] msgData = new byte[12];

        ProtocolMsg.bigE.putAddr4(msgData, 0, this.collector);

        ProtocolMsg.bigE.putUnsignedInt(msgData, 4, CollectorUtil.dateInSeconds(this.collectorBootTime));

        System.arraycopy(this.requestEyecatcher, 0, msgData, 8, requestEyecatcher.length);

        return msgData;
    }

    /// CRANE Protocol Info Block : Transport Type = TCP
    public final static int INFO_TRANSPORT_TCP = 1;

    /// CRANE Protocol Info Block : Transport Type = SCTP
    public final static int INFO_TRANSPORT_SCTP = 2;
}
