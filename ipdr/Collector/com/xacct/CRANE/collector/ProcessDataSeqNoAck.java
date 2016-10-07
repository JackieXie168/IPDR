package com.xacct.CRANE.collector;

// $Id: ProcessDataSeqNoAck.java,v 1.4 2005/01/30 15:13:39 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/** The "callback" from a CollectorOutput, which gives a new data
 *  sequence number that can be acknowledged back to the transmitter.
 *
 *  @version $Id: ProcessDataSeqNoAck.java,v 1.4 2005/01/30 15:13:39 ilyad Exp $
 */
public interface ProcessDataSeqNoAck
{
    /** This method is called when the outputter decides that an
     *  acknowledgment should be sent (e.g., it has committed some
     *  records to persistent storage). The connection thread can send
     *  an ACK back to the transmitter at this point, instead of
     *  waiting for a timer.
     *  @param newDataSeqNoToAck The last data sequence number that
     *             should be ACKed.
     *  @exception ConnectionIOException
     *  @exception ProtocolMalformedMsgException
     *  @exception ConfigException
     */
    public void processDataSeqNoAck(long newDataSeqNoToAck)
        throws ConnectionIOException, ProtocolMalformedMsgException, ConfigException;
}
