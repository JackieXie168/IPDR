package org.ipdr.collector;

import java.net.Inet4Address;
import java.net.Inet6Address;
import java.net.InetAddress;
import java.util.Date;

import org.ipdr.api.Outputter;
import org.ipdr.api.OutputterException;
import org.ipdr.api.configuration.FieldConfigI;

/**
 * The class provides a null outputter. It does nothing.
 *
 */
public class NullOutputter implements Outputter
{

    /**
     * @see org.ipdr.api.Outputter#startRecord(java.net.InetAddress, int, java.util.Date, long, int, int, int, byte[], boolean)
     */
    public void startRecord(InetAddress transmitterAddress,
            int transmitterPort, Date transmitterBootTime, long seqNo,
            int configId, int sessionId, int templateId, byte[] documentId,
            boolean isDuplicate)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#endRecord()
     */
    public void endRecord() throws OutputterException
    {
    }

    /**
     * @see org.ipdr.api.Outputter#close()
     */
    public void close()
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputBoolean(int, org.ipdr.api.configuration.FieldConfigI, byte)
     */
    public void outputBoolean(int keyI, FieldConfigI field, byte value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputUnsignedByte(int, org.ipdr.api.configuration.FieldConfigI, short)
     */
    public void outputUnsignedByte(int keyI, FieldConfigI field, short value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputByte(int, org.ipdr.api.configuration.FieldConfigI, byte)
     */
    public void outputByte(int keyI, FieldConfigI field, byte value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputUnsignedShort(int, org.ipdr.api.configuration.FieldConfigI, int)
     */
    public void outputUnsignedShort(int keyI, FieldConfigI field, int value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputShort(int, org.ipdr.api.configuration.FieldConfigI, short)
     */
    public void outputShort(int keyI, FieldConfigI field, short value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputUnsignedInt(int, org.ipdr.api.configuration.FieldConfigI, long)
     */
    public void outputUnsignedInt(int keyI, FieldConfigI field, long value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputInt(int, org.ipdr.api.configuration.FieldConfigI, int)
     */
    public void outputInt(int keyI, FieldConfigI field, int value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputUnsignedLong(int, org.ipdr.api.configuration.FieldConfigI, long)
     */
    public void outputUnsignedLong(int keyI, FieldConfigI field, long value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputLong(int, org.ipdr.api.configuration.FieldConfigI, long)
     */
    public void outputLong(int keyI, FieldConfigI field, long value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputFloat(int, org.ipdr.api.configuration.FieldConfigI, float)
     */
    public void outputFloat(int keyI, FieldConfigI field, float value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputDouble(int, org.ipdr.api.configuration.FieldConfigI, double)
     */
    public void outputDouble(int keyI, FieldConfigI field, double value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputString(int, org.ipdr.api.configuration.FieldConfigI, java.lang.String)
     */
    public void outputString(int keyI, FieldConfigI field, String value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputIPv4Addr(int, org.ipdr.api.configuration.FieldConfigI, java.net.Inet4Address)
     */
    public void outputIPv4Addr(int keyI, FieldConfigI field, Inet4Address value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputIPv6Addr(int, org.ipdr.api.configuration.FieldConfigI, java.net.Inet6Address)
     */
    public void outputIPv6Addr(int keyI, FieldConfigI field, Inet6Address value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputDateTimeSec(int, org.ipdr.api.configuration.FieldConfigI, int)
     */
    public void outputDateTimeSec(int keyI, FieldConfigI field, int value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputDateTimeMsec(int, org.ipdr.api.configuration.FieldConfigI, long)
     */
    public void outputDateTimeMsec(int keyI, FieldConfigI field, long value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputDateTimeUsec(int, org.ipdr.api.configuration.FieldConfigI, long)
     */
    public void outputDateTimeUsec(int keyI, FieldConfigI field, long value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputUUID(int, org.ipdr.api.configuration.FieldConfigI, byte[])
     */
    public void outputUUID(int keyI, FieldConfigI field, byte[] value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputMACAddress(int, org.ipdr.api.configuration.FieldConfigI, long)
     */
    public void outputMACAddress(int keyI, FieldConfigI field, long value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputBase64Binary(int, org.ipdr.api.configuration.FieldConfigI, int, byte[])
     */
    public void outputBase64Binary(int keyI, FieldConfigI field, int length,
            byte[] value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputHexBinary(int, org.ipdr.api.configuration.FieldConfigI, int, byte[])
     */
    public void outputHexBinary(int keyI, FieldConfigI field, int length,
            byte[] value)
    {
    }

}
