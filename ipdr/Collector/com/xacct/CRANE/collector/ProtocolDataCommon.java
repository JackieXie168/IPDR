package com.xacct.CRANE.collector;

// $Id: ProtocolDataCommon.java,v 1.4 2005/01/30 15:13:43 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/** Various common methods for getting data from CRANE records and
 *  putting data into CRANE records, for both big-endian and
 *  little-endian data.
 *
 *  @version $Id: ProtocolDataCommon.java,v 1.4 2005/01/30 15:13:43 ilyad Exp $
 */
public class ProtocolDataCommon
{
    /** $Id: ProtocolDataCommon.java,v 1.4 2005/01/30 15:13:43 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: ProtocolDataCommon.java,v 1.4 2005/01/30 15:13:43 ilyad Exp $"; }

    protected static final String ISO_8859_1 = "ISO-8859-1";

    /// This class must be over-ridden in all subclasses
    protected String getAddr4AsStr(byte [] record, int offset) { return ""; }

    /// This class must be over-ridden in all subclasses
    protected void putUnsignedInt(byte [] record, int offset, long value) { }

    /// This class must be over-ridden in all subclasses
    protected void putUnsignedShort(byte [] record, int offset, int value) { }

    /// This class must be over-ridden in all subclasses
    protected int getUnsignedShort(byte [] record, int offset) { return 0; }

    /// This class must be over-ridden in all subclasses
    protected long getUnsignedInt(byte [] record, int offset) { return 0; }

    public final byte getByte(byte [] record, int offset) {
        return record[offset];
    }

    public final short getUnsignedByte(byte [] record, int offset) {
        return (short)(record[offset]&0xff);
    }

    public final java.net.InetAddress getAddr4(byte [] record, int offset)
        throws java.net.UnknownHostException {
        // XXX JDK 1.4: use InetAddress.getByAddress
        return java.net.InetAddress.getByName(getAddr4AsStr(record, offset));
    }

//    public final java.util.Date getDate(byte [] record, int offset) {
//        return ProtocolMsg.dateFromSeconds(getUnsignedInt(record, offset));
//    }

    public final String getStringShortLen(byte [] record, int offsetLength, int offsetString) {
        return getStringWithLen(record, getUnsignedShort(record, offsetLength), offsetString);
    }

    public final String getStringWithLen(byte [] record, int length, int offsetString) {
        try {
            return new String(record, offsetString, length, ISO_8859_1);
        } catch (java.io.UnsupportedEncodingException e) {
            // This is not supposed to happen because ISO-8859-1 is
            // required for all conforming Java
            // implementations. Anyway, let's do things the hard way:
            StringBuffer result = new StringBuffer(length);
            for (int i = 0; i < length; i++) {
                result.append((char)record[offsetString + i]);
            }
            return result.toString();
        }
    }

    public final void putByte(byte [] record, int offset, byte value) {
        record[offset] = (byte)(value & 0xff);
    }

    public final void putUnsignedByte(byte [] record, int offset, short value) {
        record[offset] = (byte)(value & 0xff);
    }

//    public final void putDate(byte [] record, int offset, java.util.Date value) {
//        putUnsignedInt(record, offset, ProtocolMsg.dateInSeconds(value));
//    }

    public final void putStringShortLen(byte [] record, int offsetLength, int offsetString, String value) {
        try {
            byte [] b = value.getBytes(ISO_8859_1);
            putUnsignedShort(record, offsetLength, b.length);
            System.arraycopy(b, 0, record, offsetString, b.length);
        } catch (java.io.UnsupportedEncodingException e) {
            // Not supposed to happen; let's do it another way:
            int length = value.length();
            putUnsignedShort(record, offsetLength, length);
            for (int i = 0; i < length; i++) {
                record[offsetString + i] = (byte)value.charAt(i);
            }
        }
    }
}
