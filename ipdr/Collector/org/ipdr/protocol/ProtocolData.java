package org.ipdr.protocol;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.net.Inet4Address;
import java.net.Inet6Address;
import java.util.ArrayList;
import java.util.Date;

import org.ipdr.api.configuration.FieldConfig;
import org.ipdr.api.configuration.FieldConfigI;
import org.ipdr.api.configuration.TemplateConfig;
import org.ipdr.utility.IPDRUtilities;

/**
 * Various methods for getting data from IPDR/CRANE records and putting data into
 * IPDR/CRANE records. Most of these methods are abstract, allowing for big-endian
 * and little-endian implementations.
 * 
 */
public abstract class ProtocolData
{
    // Constants for sizes
    public static final int byteSize = 1;

    public static final int unsignedByteSize = 1;

    public static final int shortSize = 2;

    public static final int unsignedShortSize = 2;

    public static final int intSize = 4;

    public static final int unsignedIntSize = 4;

    public static final int addr4Size = 4;

    public static final int longSize = 8;

    /**
     * Description for debugging.
     * 
     * @return "bigEndian", "littleEndian".
     */
    abstract public String description();

    /**
     * Extracts a short value from the record at the given offset
     * @param record - to extract the value from
     * @param offset - to start the extraction at
     * @return the extracted value
     */
    abstract public short getShort(byte[] record, int offset);

    /**
     * Extracts an unsigned short value from the record at the given offset
     * @param record - to extract the value from
     * @param offset - to start the extraction at
     * @return the extracted value
     */
    abstract public int getUnsignedShort(byte[] record, int offset);

    /**
     * Extracts an integer value from the record at the given offset
     * @param record to extract the value from
     * @param offset to start the extraction at
     * @return the extracted value
     */
    abstract public int getInt(byte[] record, int offset);

    /**
     * Extracts an unsigned int value from the record at the given offset
     * @param record to extract the value from
     * @param offset to start the extraction at
     * @return the extracted value
     */
    abstract public long getUnsignedInt(byte[] record, int offset);

    /**
     * Extracts a long value from the record at the given offset
     * @param record to extract the value from
     * @param offset to start the extraction at
     * @return the extracted value
     */
    abstract public long getLong(byte[] record, int offset);

    /**
     * Extracts a float value from the record at the given offset
     * @param record to extract the value from
     * @param offset to start the extraction at
     * @return the extracted value
     */
    abstract public float getFloat(byte[] record, int offset);

    /**
     * Extracts a double value from the record at the given offset
     * @param record to extract the value from
     * @param offset to start the extraction at
     * @return the extracted value
     */
    abstract public double getDouble(byte[] record, int offset);

    /**
     * Writes a short value into the record at the given offset
     * @param record Record to write the value to
     * @param offset Offset to place the value at
     * @param value Value to write
     */
    abstract public void putShort(byte[] record, int offset, short value);

    /**
     * Writes an unsigned short value into the record at the given offset
     * @param record Record to write the value to
     * @param offset Offset to place the value at
     * @param value Value to write
     */
    abstract public void putUnsignedShort(byte[] record, int offset, int value);

    /**
     * Writes an integer value into the record at the given offset
     * @param record Record to write the value to
     * @param offset Offset to place the value at
     * @param value Value to write
     */
    abstract public void putInt(byte[] record, int offset, int value);

    /**
     * Writes an unsigned integer value into the record at the given offset
     * @param record Record to write the value to
     * @param offset Offset to place the value at
     * @param value Value to write
     */
    abstract public void putUnsignedInt(byte[] record, int offset, long value);

    /**
     * Writes a long value into the record at the given offset
     * @param record Record to write the value to
     * @param offset Offset to place the value at
     * @param value Value to write
     */
    abstract public void putLong(byte[] record, int offset, long value);

    /**
     * Extracts a Inet4Address (IP) value from the record at the given offset
     * @param record to extract the value from
     * @param offset to start the extraction at
     * @return the extracted value
     */
    abstract public Inet4Address getAddr4(byte[] record, int offset);

    /**
     * Writes an Inet4Address (IP) value into the record at the given offset
     * @param record Record to write the value to
     * @param offset Offset to place the value at
     * @param value Value to write
     */
    abstract public void putAddr4(byte[] record, int offset, Inet4Address addr);

    /**
     * Writes an Inet6Address (IPv6) value into the record at the given offset
     * @param record Record to write the value to
     * @param offset Offset to place the value at
     * @param value Value to write
     */
    abstract public void putAddr6(byte[] record, int offset, Inet6Address addr);

    /**
     * Extracts an Inet6Address (IPv6) value from the record at the given offset
     * @param record to extract the value from
     * @param offset to start the extraction at
     * @return the extracted value
     */
    abstract public Inet6Address getAddr6(byte[] record, int offset);

    /**
     * Extracts a boolean value from the record at the given offset
     * @param record to extract the value from
     * @param offset to start the extraction at
     * @return the extracted value
     */
    abstract public boolean getBoolean(byte[] record, int offset);

    /**
     * Writes a boolean value into the record at the given offset
     * @param record Record to write the value to
     * @param offset Offset to place the value at
     * @param value Value to write
     */
    abstract public void putBoolean(byte[] record, int offset, boolean value);

    protected static final String ISO_8859_1 = "ISO-8859-1";

    /**
     * Extracts a single byte from the record at the given offset
     * @param record to extract the value from
     * @param offset to start the extraction at
     * @return the extracted value
     */
    public final byte getByte(byte[] record, int offset)
    {
        return record[offset];
    }

    /**
     * Extracts an unsigned byte value from the record at the given offset
     * @param record to extract the value from
     * @param offset to start the extraction at
     * @return the extracted value
     */
    public final short getUnsignedByte(byte[] record, int offset)
    {
        return (short) (record[offset] & 0xff);
    }

    /**
     * Extracts a date value from the record at the given offset
     * @param record to extract the value from
     * @param offset to start the extraction at
     * @return the extracted value
     */
    public final Date getDate(byte[] record, int offset)
    {
        return IPDRUtilities.dateFromSeconds(getUnsignedInt(record, offset));
    }

    /**
     * Extracts a string from the record, starting at the given offset. The string length is
     * extracted at a given offset as well
     * @param record Record to extract the string from
     * @param offsetLength Offset for string length 
     * @param offsetString Offset for the string itself
     * @return The extracted string
     */
    public final String getStringShortLen(byte[] record, int offsetLength,
            int offsetString)
    {
        return getStringWithLen(record, getUnsignedShort(record, offsetLength),
                offsetString);
    }

    /**
     * Extracts a string from the record, starting at the given offset. The string length is
     * extracted at a given offset as well
     * @param record Record to extract the string from
     * @param offsetLength Offset for string length 
     * @param offsetString Offset for the string itself
     * @return The extracted string
     */
    public final String getStringLen(byte[] record, int offsetLength,
            int offsetString)
    {
        return getStringWithLen(record, (int) getUnsignedInt(record,
                offsetLength), offsetString);
    }

    /**
     * Extracts a string of the given length from the record, starting at the given offset. 
     * @param record Record to extract the string from
     * @param length String length 
     * @param offsetString Offset for the string itself
     * @return The extracted string
     */
    public final String getStringWithLen(byte[] record, int length,
            int offsetString)
    {
        try
        {
            return new String(record, offsetString, length, ISO_8859_1);
        }
        catch (java.io.UnsupportedEncodingException e)
        {
            // This is not supposed to happen because ISO-8859-1 is
            // required for all conforming Java
            // implementations. Anyway, let's do things the hard way:
            StringBuffer result = new StringBuffer(length);
            for (int i = 0; i < length; i++)
            {
                result.append((char) record[offsetString + i]);
            }
            return result.toString();
        }
    }

    /**
     * Writes a byte value into the record at the given offset
     * @param record Record to write the value to
     * @param offset Offset to place the value at
     * @param value Value to write
     */    
    public final void putByte(byte[] record, int offset, byte value)
    {
        record[offset] = (byte) (value & 0xff);
    }

    /**
     * Writes an unsigned byte value into the record at the given offset
     * @param record Record to write the value to
     * @param offset Offset to place the value at
     * @param value Value to write
     */    
    public final void putUnsignedByte(byte[] record, int offset, short value)
    {
        record[offset] = (byte) (value & 0xff);
    }

    /**
     * Writes a date value into the record at the given offset
     * @param record Record to write the value to
     * @param offset Offset to place the value at
     * @param value Value to write
     */    
    public final void putDate(byte[] record, int offset, Date value)
    {
        putUnsignedInt(record, offset, IPDRUtilities.dateInSeconds(value));
    }

    /**
     * Writes a string value into the record at the given offset
     * @param record Record to write the value to
     * @param offsetLength Offset to place the length at. The string is written immediately after the length
     * @param value Value to write
     */    
    public final void putStringLen(byte[] record, int offsetLength, String value)
    {
        putStringLen(record, offsetLength, offsetLength + 4, value);
    }

    /**
     * Writes a string value into the record at the given offset, writes its length at the given offset
     * @param record Record to write the value to
     * @param offsetLength Offset to place the length at
     * @param offsetString Offset to place the string at
     * @param value Value to write
     */
    public final void putStringLen(byte[] record, int offsetLength,
            int offsetString, String value)
    {
        try
        {
            if (value != null)
            {
                byte[] b = value.getBytes(ISO_8859_1);
                putUnsignedInt(record, offsetLength, b.length);
                System.arraycopy(b, 0, record, offsetString, b.length);
            }
            else
                putUnsignedInt(record, offsetLength, 0);
        }
        catch (java.io.UnsupportedEncodingException e)
        {
            // Not supposed to happen; let's do it another way:
            int length = value.length();
            putUnsignedShort(record, offsetLength, length);
            for (int i = 0; i < length; i++)
            {
                record[offsetString + i] = (byte) value.charAt(i);
            }
        }
    }

    /**
     * Writes a string value into the record at the given offset, writes its length at the given offset
     * @param record Record to write the value to
     * @param offsetLength Offset to place the length at
     * @param offsetString Offset to place the string at
     * @param value Value to write
     */
    public final void putStringShortLen(byte[] record, int offsetLength,
            int offsetString, String value)
    {
        try
        {
            byte[] b = value.getBytes(ISO_8859_1);
            putUnsignedShort(record, offsetLength, b.length);
            System.arraycopy(b, 0, record, offsetString, b.length);
        }
        catch (java.io.UnsupportedEncodingException e)
        {
            // Not supposed to happen; let's do it another way:
            int length = value.length();
            putUnsignedShort(record, offsetLength, length);
            for (int i = 0; i < length; i++)
            {
                record[offsetString + i] = (byte) value.charAt(i);
            }
        }
    }

    /**
     * Extracts a fixed-length byte array value from the record at the given offset
     * @param record to extract the value from
     * @param offset to start the extraction at
     * @param length of the array to extract
     * @return the extracted value
     */    
    public final byte[] getFixedByteArray(byte[] record, int offset, int length)
    {
        byte[] value = new byte[length];

        for (int i = 0; i < length; i++)
            value[i] = record[offset + i];
        return value;
    }

    /**
     * Writes a fixed-length byte array into the record
     * @param record record to write the value 
     * @param array array to write
     * @param offset offset to start from
     */
    public final void putFixedByteArray(byte[] record, byte[] array, int offset)
    {
        for (int i = 0; i < array.length; i++)
            record[offset + i] = array[i];
    }

    /**
     * Writes a field in XDR format into the record
     * @param record record to write the value
     * @param offset offset to start at
     * @param value value to write
     */
    public final void putFieldConfig(byte[] record, int offset,
            FieldConfigI value)
    {
        putInt(record, offset, value.getTypeId());
        putInt(record, offset + 4, value.getId());
        putStringLen(record, offset + 8, offset + 12, value.getName());
        putBoolean(record, offset + 12 + value.getName().length(), value
                .isEnabledForOutput());
    }

    /**
     * Writes a template block in XDR format into the record
     * @param record record to write the value
     * @param offset offset to start at
     * @param templates array of templates to write
     */
    public final void putTemplateBlock(byte[] record, int offset,
            ArrayList templates)
    {
        putInt(record, offset, templates.size());
        offset += 4;
        for (int i = 0; i < templates.size(); i++)
        {
            TemplateConfig templateConfig = (TemplateConfig) templates.get(i);
            putTemplate(record, offset, templateConfig);
            offset += calculateTemplateLength(templateConfig);
        }
    }

    /**
     * Calculates the length of the given template block
     * @param templates templates 
     * @return length of template block
     */
    public final int getTemplateBlockLength(ArrayList templates)
    {
        int result = 4;
        if (templates == null) return result;

        for (int i = 0; i < templates.size(); i++)
        {
            TemplateConfig templateConfig = (TemplateConfig) templates.get(i);
            result += calculateTemplateLength(templateConfig);
        }
        return result;
    }

    /**
     * Writes a template in XDR format into the record
     * @param record record to write the value
     * @param offset offset to start at
     * @param value value to write
     */
    public final void putTemplate(byte[] record, int offset,
            TemplateConfig value)
    {
        putShort(record, offset, value.getId());
        offset += 2;

        putStringLen(record, offset, value.getSchemaName());
        offset += 4 + (value.getSchemaName() == null ? 0 : value
                .getSchemaName().length());

        putStringLen(record, offset, value.getTypeName());
        offset += 4 + (value.getTypeName() == null ? 0 : value.getTypeName()
                .length());

        putInt(record, offset, value.getFieldListLength());
        offset += 4;
        for (int i = 0; i < value.getFieldListLength(); i++)
        {
            FieldConfigI fieldConfig = value.getFieldConfigAt(i);
            putFieldConfig(record, offset, fieldConfig);
            offset += calculateFieldLength(fieldConfig);
        }
    }

    /**
     * Extracts a template block from the message.
     * @param message Message to extract the templates from
     * @param offset offset to start at
     * @return extracted templates
     */
    public final ArrayList getTemplateBlock(byte[] message, int offset)
    {
        ArrayList result = new ArrayList();
        // starting from length
        int templateBlockLength = getInt(message, offset);
        if (templateBlockLength == 0) return result;

        offset += 4;

        for (int i = 0; i < templateBlockLength; i++)
        {
            TemplateConfig template = getTemplateConfig(message, offset);
            result.add(template);
            offset += calculateTemplateLength(template);
        }
        return result;
    }

    /**
     * Extracts a single template from the message
     * @param message Message to extract the template from
     * @param offset offset to start at
     * @return extracted template
     */
    public final TemplateConfig getTemplateConfig(byte[] message, int offset)
    {
        TemplateConfig template = new TemplateConfig();
        short id = getShort(message, offset);
        offset += 2;

        template.setId(id);

        String schemaName = getStringLen(message, offset, offset + 4);
        template.setSchemaName(schemaName);
        offset += 4 + schemaName.length();

        String typeName = getStringLen(message, offset, offset + 4);
        offset += 4 + typeName.length();
        template.setTypeName(typeName);

        template.setFields(getFields(message, offset));
        return template;
    }

    /**
     * Extracts field block in XDR format from the message
     * @param message Message to extract from
     * @param offset offset to start at
     * @return extracted fields
     */
    public final ArrayList getFields(byte[] message, int offset)
    {
        ArrayList result = new ArrayList();
        int fieldBlockLength = getInt(message, offset);
        if (fieldBlockLength == 0) return result;
        offset += 4;
        for (int i = 0; i < fieldBlockLength; i++)
        {
            int typeId = getInt(message, offset);
            int fieldId = getInt(message, offset + 4);
            offset += 8;
            String fieldName = getStringLen(message, offset, offset + 4);
            offset += 4 + fieldName.length();
            boolean enabled = getBoolean(message, offset);
            offset += 1;

            result.add(new FieldConfig(fieldId, typeId, fieldName, enabled));
        }
        return result;
    }

    /**
     * Calculates length of a single template
     * @param template template to calculate the length
     * @return length of the template
     */
    public final int calculateTemplateLength(TemplateConfig template)
    {
        int result = 2
                + // template id
                4
                + (template.getSchemaName() == null ? 0 : template
                        .getSchemaName().length())
                + 4
                + (template.getTypeName() == null ? 0 : template.getTypeName()
                        .length()) + 4; // field list length
        for (int i = 0; i < template.getFieldListLength(); i++)
        {
            result += calculateFieldLength(template.getFieldConfigAt(i));
        }
        return result;
    }

    /**
     * Calculates length of a single field
     * @param field Field to calculate the length
     * @return length of the field
     */
    public final int calculateFieldLength(FieldConfigI field)
    {
        return 4 /* type id */+ 4 /* field id */+ 4 /* string length */
                + field.getName().length() + 1 /* is enabled */;
    }

    /**
     * Extracts session info from a  message
     * @param message message to extract from
     * @param offset offset to start at
     * @return Extracted session info
     */
    public final ProtocolSessionInfo getSessionInfo(byte[] message, int offset)
    {
        ProtocolSessionInfo sessionInfo = new ProtocolSessionInfo();
        sessionInfo.setSessionId(getByte(message, offset));
        offset += 1;
        sessionInfo.setReserved(getByte(message, offset));
        offset += 1;
        sessionInfo.setSessionName(getStringLen(message, offset, offset + 4));
        offset += 4 + sessionInfo.getSessionName().length();
        sessionInfo.setSessionDescription(getStringLen(message, offset,
                offset + 4));
        offset += 4 + sessionInfo.getSessionDescription().length();
        sessionInfo.setAckTimeInterval(getInt(message, offset));
        sessionInfo.setAckSequenceInterval(getInt(message, offset + 4));
        return sessionInfo;
    }

    /**
     * Extracts session info block from a  message
     * @param message message to extract from
     * @param offset offset to start at
     * @return Extracted session info items as an <code>ArrayList</code>
     */
    public final ArrayList getSessionInfoBlock(byte[] message, int offset)
    {
        ArrayList result = new ArrayList();
        ;
        int count = getInt(message, offset);

        offset += 4;
        for (int i = 0; i < count; i++)
        {
            ProtocolSessionInfo sessionInfo = getSessionInfo(message, offset);
            offset += calculateSessionInfoLength(sessionInfo);
            result.add(sessionInfo);
        }
        return result;
    }

    /**
     * Extracts protocol info from a  message
     * @param message message to extract from
     * @param offset offset to start at
     * @return Extracted protocol info
     */
    public final ArrayList getProtocolInfoBlock(byte[] message, int offset)
    {
        ArrayList result = new ArrayList();
        int count = getInt(message, offset);
        offset += 4;
        for (int i = 0; i < count; i++)
        {
            ProtocolInfo info = getProtocolInfo(message, offset);
            offset += ProtocolInfo.INFO_SIZE;
            result.add(info);
        }
        return result;
    }

    /**
     * Extracts protocol info from a message
     * @param message message to extract from
     * @param offset offset to start at
     * @return Extracted protocol info
     */
    public final ProtocolInfo getProtocolInfo(byte[] message, int offset)
    {
        int transportType = getInt(message, offset);
        int protocolVersion = getInt(message, offset + 4);
        short portNumber = getShort(message, offset + 8);
        short reserved = getShort(message, offset + 10);
        return new ProtocolInfo(transportType, protocolVersion, portNumber,
                reserved);
    }

    /**
     * Writes protocol info block to the message
     * @param message message to write to
     * @param offset offset to start at
     * @param block protocol info block
     */
    public final void putProtocolInfoBlock(byte[] message, int offset,
            ArrayList block)
    {
        putInt(message, offset, block.size());
        offset += 4;
        for (int i = 0; i < block.size(); i++)
        {
            putProtocolInfo(message, offset, (ProtocolInfo) block.get(i));
            offset += ProtocolInfo.INFO_SIZE;
        }
    }

    /**
     * Writes protocol info to the message
     * @param message message to write to
     * @param offset offset to start at
     * @param value protocol info to write
     */
    public final void putProtocolInfo(byte[] message, int offset,
            ProtocolInfo value)
    {
        putInt(message, offset, value.getTransportType());
        putInt(message, offset + 4, value.getProtocolVersion());
        putShort(message, offset + 8, value.getPortNumber());
        putShort(message, offset + 10, value.getReserved());
    }

    /**
     * Calculates length of a session info     
     * @param sessionInfo session info to calculate the length
     * @return calculated length
     */
    public final int calculateSessionInfoLength(ProtocolSessionInfo sessionInfo)
    {
        return 1 /* session id */+ 1 /* reserved */+ 4 /* string length */
                + sessionInfo.getSessionName().length() + 4 /* string length */
                + sessionInfo.getSessionDescription().length() + 4 /* ackTimeInterval */
                + 4 /* ackSequenceInterval */;

    }
}