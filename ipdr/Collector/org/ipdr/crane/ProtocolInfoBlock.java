package org.ipdr.crane;

// $Id: ProtocolInfoBlock.java,v 1.3 2005/01/30 15:13:45 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/** The Java encapsulation of a single Protocol Info Block.
 *
 *  @see ProtocolMsgInfoResponse
 *
 *  @version $Id: ProtocolInfoBlock.java,v 1.3 2005/01/30 15:13:45 ilyad Exp $
 */
public final class ProtocolInfoBlock
{
    /** $Id: ProtocolInfoBlock.java,v 1.3 2005/01/30 15:13:45 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: ProtocolInfoBlock.java,v 1.3 2005/01/30 15:13:45 ilyad Exp $"; }

    /** Create a Protocol Info Block
     *
     * @param transportType Protocol.INFO_TRANSPORT_TCP or Protocol.INFO_TRANSPORT_SCTP
     * @param protocolVersion
     * @param portNumber
     */
    public ProtocolInfoBlock(int transportType, int protocolVersion, int portNumber) {
        this.transportType   = transportType;
        this.protocolVersion = protocolVersion;
        this.portNumber      = portNumber;
    }

    /// Transport type: Protocol.INFO_TRANSPORT_TCP or Protocol.INFO_TRANSPORT_SCTP
    public int transportType;

    /// Protocol version
    public int protocolVersion;

    /// Port number for receiving data
    public int portNumber;
}
