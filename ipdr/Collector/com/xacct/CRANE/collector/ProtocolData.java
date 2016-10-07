package com.xacct.CRANE.collector;

// $Id: ProtocolData.java,v 1.5 2005/04/08 12:48:10 guyg Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.Date;

/** Various methods for getting data from CRANE records and putting
 *  data into CRANE records. Most of these methods are abstract,
 *  allowing for big-endian and little-endian implementations.
 *
 *  @version $Id: ProtocolData.java,v 1.5 2005/04/08 12:48:10 guyg Exp $
 */
public abstract class ProtocolData
{
    /** $Id: ProtocolData.java,v 1.5 2005/04/08 12:48:10 guyg Exp $ */
    public static String versionStr() { return "@(#) $Id: ProtocolData.java,v 1.5 2005/04/08 12:48:10 guyg Exp $"; }

    public static final int byteSize          = 1;
    public static final int unsignedByteSize  = 1;
    public static final int shortSize         = 2;
    public static final int unsignedShortSize = 2;
    public static final int intSize           = 4;
    public static final int unsignedIntSize   = 4;
    public static final int addr4Size         = 4;
    public static final int longSize          = 8;

    /** Description for debugging.
     *  @return "bigEndian", "littleEndian".
     */
    abstract public String description();

    abstract public short getShort(byte [] record, int offset);
    abstract public int getUnsignedShort(byte [] record, int offset);
    abstract public int getInt(byte [] record, int offset);
    abstract public long getUnsignedInt(byte [] record, int offset);
    abstract public long getLong(byte [] record, int offset);
    abstract public String getAddr4AsStr(byte [] record, int offset);

    abstract public void putShort(byte [] record, int offset, short value);
    abstract public void putUnsignedShort(byte [] record, int offset, int value);
    abstract public void putInt(byte [] record, int offset, int value);
    abstract public void putUnsignedInt(byte [] record, int offset, long value);
    abstract public void putLong(byte [] record, int offset, long value);
    abstract public void putAddr4(byte [] record, int offset, InetAddress addr)
        throws UnknownHostException;

    protected static final String ISO_8859_1 = "ISO-8859-1";

    public final byte getByte(byte [] record, int offset) {
        return record[offset];
    }

    public final short getUnsignedByte(byte [] record, int offset) {
        return (short)(record[offset]&0xff);
    }

    public final InetAddress getAddr4(byte [] record, int offset)
        throws UnknownHostException {
        // XXX JDK 1.4: use InetAddress.getByAddress
        return InetAddress.getByName(getAddr4AsStr(record, offset));
    }

    public final Date getDate(byte [] record, int offset) {
        return CollectorUtil.dateFromSeconds(getUnsignedInt(record, offset));
    }

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

    public final void putDate(byte [] record, int offset, Date value) {
        putUnsignedInt(record, offset, CollectorUtil.dateInSeconds(value));
    }

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
