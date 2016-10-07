package org.ipdr.api;

// $Id: Outputter.java,v 1.8 2005/02/16 10:45:03 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.net.Inet4Address;
import java.net.Inet6Address;
import java.net.InetAddress;
import java.util.Date;

import org.ipdr.api.configuration.FieldConfigI;

/**
 * An interface for encapsulating the methods for outputting values for a record
 * from the Collector.
 * 
 * No exceptions are defined here: if something goes wrong, the preferred
 * exceptions are ClassCastException or OutputterException.
 * <p>
 * This interface contains:
 * <ul>
 * <li>{@link #startRecord startRecord}
 * <li>{@link #outputString outputXxx}(repeated) for the fields
 * <li>{@link #endRecord endRecord}
 * </ul>
 * <p>
 *  
 */
public interface Outputter
{
    /**
     * Prepare to output a record. The parameters given here will be active
     * until endRecord is called.
     * 
     * See the note on thread safety for this interface.
     * 
     * @param transmitterAddress
     *            the transmitter's address (for this connection).
     * @param transmitterPort
     *            the transmitter's data port (for this connection) as
     *            negotiated by the version negotiation messages or (if that
     *            failed) by the configuration.
     * @param transmitterBootTime
     *            the transmitter's boot time (for this connection).
     * @param seqNo
     *            the sequence number of the record. Usually, this increases
     *            each time, but there may be occasions when the sequence number
     *            starts back at a lower value or skips.
     * @param sessionId
     * @param templateId
     * @see #endRecord
     */
    public void startRecord(InetAddress transmitterAddress,
            int transmitterPort, Date transmitterBootTime, long seqNo,
            int configId, int sessionId, int templateId, 
            byte[] documentId, boolean isDuplicate);

    /**
     * Finish outputting a record. After this, the parameters given via
     * startRecord will be invalid.
     * 
     * See the note on thread safety for this interface.
     * 
     * @see #startRecord
     * 
     * @throws OutputterException
     *             if the record could not be output.
     */
    public void endRecord() throws OutputterException;

    /**
     * Finish ouputting all records. You should also implement finalize(), which
     * calls this.
     */
    public void close();

    /**
     * Output a single boolean field
     * 
     * @param keyI
     *            This field is the keyI'th field in the record.
     * @param field
     *            The field's configuration.
     * @param value
     *            The value to output.
     */
    public void outputBoolean(int keyI, FieldConfigI field, byte value);

    /**
     * Output a single unsigned byte field.
     * 
     * @param keyI
     *            This field is the keyI'th field in the record.
     * @param field
     *            The field's configuration.
     * @param value
     *            The value to output.
     */
    public void outputUnsignedByte(int keyI, FieldConfigI field, short value);

    /**
     * Output a single signed byte field.
     * 
     * @param keyI
     *            This field is the keyI'th field in the record.
     * @param field
     *            The field's configuration.
     * @param value
     *            The value to output.
     */
    public void outputByte(int keyI, FieldConfigI field, byte value);

    /**
     * Output a single unsigned short field.
     * 
     * @param keyI
     *            This field is the keyI'th field in the record.
     * @param field
     *            The field's configuration.
     * @param value
     *            The value to output.
     */
    public void outputUnsignedShort(int keyI, FieldConfigI field, int value);

    /**
     * Output a single signed short field.
     * 
     * @param keyI
     *            This field is the keyI'th field in the record.
     * @param field
     *            The field's configuration.
     * @param value
     *            The value to output.
     */
    public void outputShort(int keyI, FieldConfigI field, short value);

    /**
     * Output a single unsigned integer field.
     * 
     * @param keyI
     *            This field is the keyI'th field in the record.
     * @param field
     *            The field's configuration.
     * @param value
     *            The value to output.
     */
    public void outputUnsignedInt(int keyI, FieldConfigI field, long value);

    /**
     * Output a single signed integer field.
     * 
     * @param keyI
     *            This field is the keyI'th field in the record.
     * @param field
     *            The field's configuration.
     * @param value
     *            The value to output.
     */
    public void outputInt(int keyI, FieldConfigI field, int value);

    /**
     * Output a single unsigned long integer field.
     * 
     * @param keyI
     *            This field is the keyI'th field in the record.
     * @param field
     *            The field's configuration.
     * @param value
     *            The value to output.
     */
    public void outputUnsignedLong(int keyI, FieldConfigI field, long value);

    /**
     * Output a single signed long integer field.
     * 
     * @param keyI
     *            This field is the keyI'th field in the record.
     * @param field
     *            The field's configuration.
     * @param value
     *            The value to output.
     */
    public void outputLong(int keyI, FieldConfigI field, long value);

    /**
     * Output a single float field.
     * 
     * @param keyI
     *            This field is the keyI'th field in the record.
     * @param field
     *            The field's configuration.
     * @param value
     *            The value to output.
     */
    public void outputFloat(int keyI, FieldConfigI field, float value);

    /**
     * Output a single double field.
     * 
     * @param keyI
     *            This field is the keyI'th field in the record.
     * @param field
     *            The field's configuration.
     * @param value
     *            The value to output.
     */
    public void outputDouble(int keyI, FieldConfigI field, double value);

    /**
     * Output a single string field (from a string).
     * 
     * @param keyI
     *            This field is the keyI'th field in the record.
     * @param field
     *            The field's configuration.
     * @param value
     *            The value to output.
     */
    public void outputString(int keyI, FieldConfigI field, String value);

    /**
     * Output a single IPV4 address field.
     * 
     * @param keyI
     *            This field is the keyI'th field in the record.
     * @param field
     *            The field's configuration.
     * @param value
     *            The value to output.
     */
    public void outputIPv4Addr(int keyI, FieldConfigI field, Inet4Address value);

    /**
     * Output a single IPV6 address field
     * 
     * @param keyI
     *            This field is the keyI'th field in the record.
     * @param field
     *            The field's configuration.
     * @param value
     *            The value to output.
     */
    public void outputIPv6Addr(int keyI, FieldConfigI field, Inet6Address value);

    /**
     * Output a single time field (seconds since epoch).
     * 
     * @param keyI
     *            This field is the keyI'th field in the record.
     * @param field
     *            The field's configuration.
     * @param value
     *            The value to output.
     */
    public void outputDateTimeSec(int keyI, FieldConfigI field, int value);

    /**
     * Output a single time field (milliseconds since epoch).
     * 
     * @param keyI
     *            This field is the keyI'th field in the record.
     * @param field
     *            The field's configuration.
     * @param value
     *            The value to output.
     */
    public void outputDateTimeMsec(int keyI, FieldConfigI field, long value);

    /**
     * Output a single time field (microseconds since epoch).
     * 
     * @param keyI
     *            This field is the keyI'th field in the record.
     * @param field
     *            The field's configuration.
     * @param value
     *            The value to output.
     */
    public void outputDateTimeUsec(int keyI, FieldConfigI field, long value);

    /**
     * Output a single UUID field
     * 
     * @param keyI
     *            This field is the keyI'th field in the record.
     * @param field
     *            The field's configuration.
     * @param value
     *            The value to output.
     */
    public void outputUUID(int keyI, FieldConfigI field, byte[] value);

    /**
     * Output a single MAC address field
     * 
     * @param keyI
     *            This field is the keyI'th field in the record.
     * @param field
     *            The field's configuration.
     * @param value
     *            The value to output.
     */
    public void outputMACAddress(int keyI, FieldConfigI field, long value);

    /**
     * Output a base-64 encoded binary data
     * 
     * @param keyI
     *            This field is the keyI'th field in the record.
     * @param field
     *            The field's configuration.
     * @param value
     *            The value to output.
     */
    public void outputBase64Binary(int keyI, FieldConfigI field, int length,
            byte[] value);

    /**
     * Output raw hex data
     * 
     * @param keyI
     *            This field is the keyI'th field in the record.
     * @param field
     *            The field's configuration.
     * @param value
     *            The value to output.
     */
    public void outputHexBinary(int keyI, FieldConfigI field, int length,
            byte[] value);
}