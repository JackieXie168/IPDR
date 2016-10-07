package org.ipdr.protocol;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.net.Inet4Address;
import java.net.Inet6Address;
import java.net.InetAddress;
import java.net.UnknownHostException;

/**
 * Implementation of <code>ProtocolData</code> for big endian byte order
 */
public final class ProtocolDataBigEndian extends ProtocolData
{
    public ProtocolDataBigEndian()
    {
    }

    /**
     * @see org.ipdr.protocol.ProtocolData#description()
     */
    public final String description()
    {
        return "bigEndian";
    }

    /**
     * @see org.ipdr.protocol.ProtocolData#getAddr4(byte[], int)
     */
    public final Inet4Address getAddr4(byte[] record, int offset)
    {
        Inet4Address result = null;
        try
        {
            result = (Inet4Address) InetAddress.getByAddress(getFixedByteArray(
                    record, offset, 4));
        }
        catch (UnknownHostException e)
        {
            // this place is unreachable!
        }
        return result;
    }

    /**
     * @see org.ipdr.protocol.ProtocolData#getAddr6(byte[], int)
     */
    public Inet6Address getAddr6(byte[] record, int offset)
    {
        Inet6Address result = null;
        try
        {
            result = (Inet6Address) Inet6Address
                    .getByAddress(getFixedByteArray(record, offset, 16));
        }
        catch (UnknownHostException e)
        {
            // this place is unreachable
        }
        return result;
    }

    /**
     * @see org.ipdr.protocol.ProtocolData#getBoolean(byte[], int)
     */
    public final boolean getBoolean(byte[] record, int offset)
    {
        /* boolean is 1 byte in IPDR */
        /* return (getInt( record, offset) == 1); */
        return record[offset] == 1;
    }

    /**
     * @see org.ipdr.protocol.ProtocolData#getDouble(byte[], int)
     */
    public final double getDouble(byte[] record, int offset)
    {
        double value = Double.longBitsToDouble(getLong(record, offset));
        return value;
    }

    /**
     * @see org.ipdr.protocol.ProtocolData#getFloat(byte[], int)
     */
    public final float getFloat(byte[] record, int offset)
    {
        float value = Float.intBitsToFloat(getInt(record, offset));
        return value;
    }

    /**
     * @see org.ipdr.protocol.ProtocolData#getInt(byte[], int)
     */
    public final int getInt(byte[] record, int offset)
    {
        return (int) ((record[offset]) << 24)
                + (int) ((record[offset + 1] & 0xff) << 16)
                + (int) ((record[offset + 2] & 0xff) << 8)
                + (int) ((record[offset + 3] & 0xff));
    }

    /**
     * @see org.ipdr.protocol.ProtocolData#getLong(byte[], int)
     */
    public final long getLong(byte[] record, int offset)
    {
        return (((long) getInt(record, offset)) << 32)
                + getUnsignedInt(record, offset + 4);
    }

    /**
     * @see org.ipdr.protocol.ProtocolData#getShort(byte[], int)
     */
    public final short getShort(byte[] record, int offset)
    {
        return (short) (((record[offset] & 0xff) << 8) + ((record[offset + 1] & 0xff)));
    }

    /**
     * @see org.ipdr.protocol.ProtocolData#getUnsignedInt(byte[], int)
     */
    public final long getUnsignedInt(byte[] record, int offset)
    {
        return 0xffffffffL & ((long) ((record[offset] & 0xff) << 24)
                + (long) ((record[offset + 1] & 0xff) << 16)
                + (long) ((record[offset + 2] & 0xff) << 8) + (long) ((record[offset + 3] & 0xff)));
    }

    /**
     * @see org.ipdr.protocol.ProtocolData#getUnsignedShort(byte[], int)
     */
    public final int getUnsignedShort(byte[] record, int offset)
    {
        return 0xffff & ((int) ((record[offset]) << 8) + (int) ((record[offset + 1] & 0xff)));
    }

    /**
     * 
     * @see org.ipdr.protocol.ProtocolData#putAddr4(byte[], int,
     *      java.net.Inet4Address)
     */
    public void putAddr4(byte[] record, int offset, Inet4Address addr)
    {
        putAddress(record, offset, addr);
    }

    /**
     * 
     * @see org.ipdr.protocol.ProtocolData#putAddr6(byte[], int,
     *      java.net.Inet6Address)
     */
    public void putAddr6(byte[] record, int offset, Inet6Address addr)
    {
        putAddress(record, offset, addr);
    }

    /**
     * puts a generic inet address (either IPv4 or IPv6) into the message
     * @param record message to write to
     * @param offset offset to start at
     * @param addr address to write
     */
    protected void putAddress(byte[] record, int offset, InetAddress addr)
    {
        byte[] rawAddress = addr.getAddress();

        for (int i = 0; i < rawAddress.length; i++)
        {
            record[offset + i] = rawAddress[i];
        }
    }

    /**
     * @see org.ipdr.protocol.ProtocolData#putBoolean(byte[], int, boolean)
     */
    public final void putBoolean(byte[] record, int offset, boolean value)
    {
        /* boolean is 1 byte in IPDR */
        /* putInt(record, offset, value ? 1:0); */
        record[offset] = (byte) (value ? 1 : 0);
    }

    /**
     * @see org.ipdr.protocol.ProtocolData#putInt(byte[], int, int)
     */
    public final void putInt(byte[] record, int offset, int value)
    {
        record[offset] = (byte) ((value >> 24) & 0xff);
        record[offset + 1] = (byte) ((value >> 16) & 0xff);
        record[offset + 2] = (byte) ((value >> 8) & 0xff);
        record[offset + 3] = (byte) ((value) & 0xff);
    }

    /**
     * @see org.ipdr.protocol.ProtocolData#putLong(byte[], int, long)
     */
    public final void putLong(byte[] record, int offset, long value)
    {
        record[offset] = (byte) ((value >> 56) & 0xff);
        record[offset + 1] = (byte) ((value >> 48) & 0xff);
        record[offset + 2] = (byte) ((value >> 40) & 0xff);
        record[offset + 3] = (byte) ((value >> 32) & 0xff);
        record[offset + 4] = (byte) ((value >> 24) & 0xff);
        record[offset + 5] = (byte) ((value >> 16) & 0xff);
        record[offset + 6] = (byte) ((value >> 8) & 0xff);
        record[offset + 7] = (byte) ((value) & 0xff);
    }

    /**
     * @see org.ipdr.protocol.ProtocolData#putShort(byte[], int, short)
     */
    public final void putShort(byte[] record, int offset, short value)
    {
        record[offset] = (byte) ((value >> 8) & 0xff);
        record[offset + 1] = (byte) (value & 0xff);
    }

    /**
     * @see org.ipdr.protocol.ProtocolData#putUnsignedInt(byte[], int, long)
     */
    public final void putUnsignedInt(byte[] record, int offset, long value)
    {
        record[offset] = (byte) ((value >> 24) & 0xff);
        record[offset + 1] = (byte) ((value >> 16) & 0xff);
        record[offset + 2] = (byte) ((value >> 8) & 0xff);
        record[offset + 3] = (byte) ((value) & 0xff);
    }

    /**
     * @see org.ipdr.protocol.ProtocolData#putUnsignedShort(byte[], int, int)
     */
    public final void putUnsignedShort(byte[] record, int offset, int value)
    {
        record[offset] = (byte) ((value >> 8) & 0xff);
        record[offset + 1] = (byte) (value & 0xff);
    }
}