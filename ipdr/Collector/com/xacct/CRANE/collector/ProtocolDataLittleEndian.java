package com.xacct.CRANE.collector;

// $Id: ProtocolDataLittleEndian.java,v 1.5 2005/01/30 15:15:12 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.net.InetAddress;
import java.net.UnknownHostException;

/** Various methods for getting data from CRANE records and putting
 *  data into CRANE records, for little-endian data.
 *
 *  @version $Id: ProtocolDataLittleEndian.java,v 1.5 2005/01/30 15:15:12 ilyad Exp $
 */
public final class ProtocolDataLittleEndian extends ProtocolData
{
    /** $Id: ProtocolDataLittleEndian.java,v 1.5 2005/01/30 15:15:12 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: ProtocolDataLittleEndian.java,v 1.5 2005/01/30 15:15:12 ilyad Exp $"; }

    public ProtocolDataLittleEndian() { }

    public final String description() { return "littleEndian"; }

    public final short getShort(byte [] record, int offset) {
        return (short)
            (((record[offset+1]&0xff) << 8) +
             ((record[offset ]&0xff)));
    }

    public final int getUnsignedShort(byte [] record, int offset) {
        return 0xffff & (
            (int)((record[offset+1]) << 8) +
            (int)((record[offset+0]&0xff)));
    }

    public final int getInt(byte [] record, int offset) {
        return
            (int)((record[offset+3]     ) << 24) +
            (int)((record[offset+2]&0xff) << 16) +
            (int)((record[offset+1]&0xff) <<  8) +
            (int)((record[offset  ]&0xff));
    }

    public final long getUnsignedInt(byte [] record, int offset) {
        return 0xffffffffL & (
            (long)((record[offset+3]&0xff) << 24) +
            (long)((record[offset+2]&0xff) << 16) +
            (long)((record[offset+1]&0xff) <<  8) +
            (long)((record[offset  ]&0xff)));
    }

    public final long getLong(byte [] record, int offset) {
        return (((long)getInt(record, offset+4)) << 32) +
            getUnsignedInt(record, offset);
        // The following looks right, but it isn't (life's to short to figure
        // out what's wrong).
        // return
        // (long)((record[offset+7]&0xff) << 56) +
        // (long)((record[offset+6]&0xff) << 48) +
        // (long)((record[offset+5]&0xff) << 40) +
        // (long)((record[offset+4]&0xff) << 32) +
        // (long)((record[offset+3]&0xff) << 24) +
        // (long)((record[offset+2]&0xff) << 16) +
        // (long)((record[offset+1]&0xff) <<  8) +
        // (long)((record[offset  ]&0xff));
    }

    public final String getAddr4AsStr(byte [] record, int offset) {
        return "" +
            getUnsignedByte(record, offset+3) + "." +
            getUnsignedByte(record, offset+2) + "." +
            getUnsignedByte(record, offset+1) + "." +
            getUnsignedByte(record, offset  );
    }

    public final void putShort(byte [] record, int offset, short value) {
        record[offset+1] = (byte)((value >> 8) & 0xff);
        record[offset  ] = (byte)(value & 0xff);
    }

    public final void putUnsignedShort(byte [] record, int offset, int value) {
        record[offset+1] = (byte)((value >> 8) & 0xff);
        record[offset  ] = (byte)(value & 0xff);
    }

    public final void putInt(byte [] record, int offset, int value) {
        record[offset+3] = (byte)((value >> 24) & 0xff);
        record[offset+2] = (byte)((value >> 16) & 0xff);
        record[offset+1] = (byte)((value >>  8) & 0xff);
        record[offset  ] = (byte)((value      ) & 0xff);
    }

    public final void putUnsignedInt(byte [] record, int offset, long value) {
        record[offset+3] = (byte)((value >> 24) & 0xff);
        record[offset+2] = (byte)((value >> 16) & 0xff);
        record[offset+1] = (byte)((value >>  8) & 0xff);
        record[offset  ] = (byte)((value      ) & 0xff);
    }

    public final void putLong(byte [] record, int offset, long value) {
        record[offset+7] = (byte)((value >> 56) & 0xff);
        record[offset+6] = (byte)((value >> 48) & 0xff);
        record[offset+5] = (byte)((value >> 40) & 0xff);
        record[offset+4] = (byte)((value >> 32) & 0xff);
        record[offset+3] = (byte)((value >> 24) & 0xff);
        record[offset+2] = (byte)((value >> 16) & 0xff);
        record[offset+1] = (byte)((value >>  8) & 0xff);
        record[offset  ] = (byte)((value      ) & 0xff);
    }

    public final void putAddr4(byte [] record, int offset, InetAddress value)
        throws UnknownHostException {
        byte [] valueBytes = value.getAddress();
        if (valueBytes.length != 4) {
            throw new UnknownHostException(value.toString() + " not IPv4 - length=" +
                                                    valueBytes.length);
        }
        record[offset+3] = valueBytes[0];
        record[offset+2] = valueBytes[1];
        record[offset+1] = valueBytes[2];
        record[offset  ] = valueBytes[3];
    }
}
