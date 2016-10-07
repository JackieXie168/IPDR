package org.ipdr.crane;

// $Id: ProtocolMsgCONNECT.java,v 1.3 2005/01/30 15:13:45 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.net.Inet4Address;
import java.net.UnknownHostException;

import org.ipdr.protocol.ProtocolMalformedMsgException;

/** The Java encapsulation of a CRANE protocol message: "CONNECT".
 *  <xmp>
 *        0                   1                   2                   3
 *        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |  Version      |  MID=0x05     | Session ID    | Message Flags |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                         Message Length                        |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                         Server Address                        |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |          Server Port          |           Reserved            |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  </xmp>
 *  <p>
 *  (Server = Collector)
 *
 *  @version $Id: ProtocolMsgCONNECT.java,v 1.3 2005/01/30 15:13:45 ilyad Exp $
 */
public final class ProtocolMsgCONNECT extends ProtocolMsg
{
    /** $Id: ProtocolMsgCONNECT.java,v 1.3 2005/01/30 15:13:45 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: ProtocolMsgCONNECT.java,v 1.3 2005/01/30 15:13:45 ilyad Exp $"; }

    public String msgName() { return "CONNECT"; }
    public static final byte MSG_ID = 0x05;
    public final byte msgId() { return MSG_ID; }

    /// The server (collector)
    public Inet4Address collectorAddress;

    /// The server (collector) port
    public int collectorConnectPortId;

    /** Construct from raw bytes
     *  @param msg The raw message bytes
     *  @param length The number of bytes in the message
     */
    public ProtocolMsgCONNECT(byte [] msg, int length)
        throws ProtocolMalformedMsgException, UnknownHostException {
        super(msg, length, MSG_LENGTH);
        try {
            this.collectorAddress       = bigE.getAddr4(        msg, SERVER_ADDRESS_OFFSET);
            this.collectorConnectPortId = bigE.getUnsignedShort(msg, SERVER_PORT_OFFSET);
        } catch (IndexOutOfBoundsException e) { handleIndexOutOfBounds(e, msg, length); }
    }

    /** Construct from values
     *  @param sessionId The session identifier
     *  @param collectorAddress The local host (server: collector) -- typically <code>ExtendedInetAddress.getLocalHost().get()</code>
     *  @param collectorConnectPortId The "port" that identifies this collector (no relation to TCP/UDP port)
     */
    public ProtocolMsgCONNECT(byte sessionId,
                              Inet4Address collectorAddress,
                              int collectorConnectPortId) {
        super(MSG_ID, sessionId, MSG_LENGTH);
        this.collectorAddress       = collectorAddress;
        this.collectorConnectPortId = collectorConnectPortId;
    }

    public final byte [] createRawMsg() throws UnknownHostException {
        byte [] msgData = new byte[MSG_LENGTH];
        setHeader(msgData);
        bigE.putAddr4(        msgData, SERVER_ADDRESS_OFFSET, this.collectorAddress);
        bigE.putUnsignedShort(msgData, SERVER_PORT_OFFSET,    this.collectorConnectPortId);
        bigE.putUnsignedShort(msgData, RESERVED1_OFFSET,      0);
        return msgData;
    }

    public String toString() {
        return super.toString() +
            "[collector=" + this.collectorAddress + " id=" + this.collectorConnectPortId + "]";
    }

    private final static int SERVER_ADDRESS_OFFSET =  8;
    private final static int SERVER_PORT_OFFSET    = 12;
    private final static int RESERVED1_OFFSET      = 14;

    private final static int MSG_LENGTH            = 16;
}
