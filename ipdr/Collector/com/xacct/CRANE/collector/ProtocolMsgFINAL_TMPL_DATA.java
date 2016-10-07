package com.xacct.CRANE.collector;

// $Id: ProtocolMsgFINAL_TMPL_DATA.java,v 1.4 2005/01/30 15:13:42 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/** The Java encapsulation of a CRANE protocol messages: "FINAL_TMPL_DATA".
 *  It is identical to TMPL_DATA excet for the message Id.
 *  <xmp>
 *        0                   1                   2                   3
 *        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |  Version      |  MID=0x12     | Session ID    | Message Flags |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                         Message Length                        |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |    Config. ID |    Reserved   |   Number of Template Changes  |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                                                               |
 *       ~                    Template Change Block                      ~
 *       |                                                               |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                                                               |
 *       ~                       ...       ...                           ~
 *       |                                                               |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       |                                                               |
 *       ~                    Template Change Block                      ~
 *       |                                                               |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  </xmp>
 *
 *  @version $Id: ProtocolMsgFINAL_TMPL_DATA.java,v 1.4 2005/01/30 15:13:42 ilyad Exp $
 */
public final class ProtocolMsgFINAL_TMPL_DATA extends ProtocolMsgTMPL_DATA
{
    /** $Id: ProtocolMsgFINAL_TMPL_DATA.java,v 1.4 2005/01/30 15:13:42 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: ProtocolMsgFINAL_TMPL_DATA.java,v 1.4 2005/01/30 15:13:42 ilyad Exp $"; }

    public String msgName() { return "FINAL_TMPL_DATA"; }
    public static final byte MSG_ID = 0x12;
    public final byte msgId() { return MSG_ID; }

    /** Construct from raw bytes
     *  @param msg The raw message bytes
     *  @param length The number of bytes in the message
     */
    public ProtocolMsgFINAL_TMPL_DATA(byte [] msg, int length, LogHandler log)
        throws ProtocolMalformedMsgException {
        super(msg, length, log);
    }
}
