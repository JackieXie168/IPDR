package com.xacct.CRANE.collector;

// $Id: CollectorFastOutputter.java,v 1.4 2005/01/30 15:13:39 ilyad Exp $
// Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/** A class for encapsulating the methods for outputting values for a
 *  record from the Collector; but more optimized than {@link
 *  CollectorOutputter}.  If an outputter implements this interface,
 *  {@link CollectorOutputter#startRecord startRecord},
 *  {@link CollectorOutputter#endRecord endRecord},
 *  and the various {@link CollectorOutputter#outputString outputXxx}
 *  methods won't be used. The intent is that an outputter object can
 *  choose to also support faster output; the connection handler may
 *  decide to use the fast outputter if it is supported.
 *  @see CollectorOutputter
 *
 *  @version $Id: CollectorFastOutputter.java,v 1.4 2005/01/30 15:13:39 ilyad Exp $
 */
public interface CollectorFastOutputter extends CollectorOutputter
{
    /** Output a record.
     *  @param transmitterAddress the transmitter's address (for this connection).
     *  @param transmitterPort the transmitter's data port (for this connection)
     *            as negotiated by the version negotiation messages
     *            or (if that failed) by the configuration.
     *  @param transmitterVendorString the transmitter's vendor string (for this connection),
     *            as returned by the GET SESS RSP message.
     *  @param transmitterBootTime the transmitter's boot time (for this connection),
     *            as returned by the START ACK message.
     *  @param seqNo the sequence number of the record. Usually, this
     *               increases each time, but there may be occasions
     *               when the sequence number starts back at a lower
     *               value or skips.
     *  @param sessionId the message's session ID
     *  @param templateId
     *  @param msgData the record, as received by the collector thread
     *  @param isBigEndian if true, big-endian; if false, little endian
     */
    public void outputRecord(ExtendedInetAddress transmitterAddress,
                             int transmitterPort,
                             String transmitterVendorString,
                             java.util.Date transmitterBootTime,
                             ProtocolMsgDATA msgData,
                             boolean isBigEndian,
                             SessionConfig sessionConfig,
                             TemplateConfig templateConfig);
}
