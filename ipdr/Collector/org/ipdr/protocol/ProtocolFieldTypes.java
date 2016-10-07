package org.ipdr.protocol;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import org.ipdr.api.configuration.FieldConfigI;

/**
 * The class concentrates XDR field types and lengths
 */
public final class ProtocolFieldTypes
{
    // length constants
    public static final int BOOLEAN_LEN = 1;
    public static final int BYTE_LEN = 1;
    public static final int DATETIME_LEN = 4;
    public static final int DATETIMEMSEC_LEN = 8;
    public static final int DATETIMEUSEC_LEN = 8;
    public static final int DOUBLE_LEN = 8;
    public static final int FLOAT_LEN = 4;
    public static final int INT_LEN = 4;
    public static final int IP4ADDR_LEN = 4;
    public static final int IP6ADDR_LEN = 16;
    public static final int LONG_LEN = 8;
    public static final int MACADDRESS_LEN = 8;
    public static final int SHORT_LEN = 2;
    public static final int UBYTE_LEN = 1;
    public static final int UINT_LEN = 4;
    public static final int ULONG_LEN = 8;
    public static final int USHORT_LEN = 2;
    public static final int UUID_LEN = 20; // 4 byte length (always 16) and 16

    // field types
    public static final int TYPE_BOOLEAN = 0x00000029;
    public static final int TYPE_BYTE = 0x0000002a;
    public static final int TYPE_DATETIME = 0x00000122;
    public static final int TYPE_DATETIMEMSEC = 0x00000224;
    public static final int TYPE_DATETIMEUSEC = 0x00000623;
    public static final int TYPE_DOUBLE = 0x00000026;
    public static final int TYPE_FLOAT = 0x00000025;
    public static final int TYPE_HEXBINARY = 0x00000027;
    public static final int TYPE_INT = 0x00000021;
    public static final int TYPE_IP4ADDR = 0x00000322;
    public static final int TYPE_IP6ADDR = 0x00000427;
    public static final int TYPE_LONG = 0x00000023;
    public static final int TYPE_MACADDRESS = 0x00000723;
    public static final int TYPE_SHORT = 0x0000002c;
    public static final int TYPE_STRING = 0x00000028;
    public static final int TYPE_UBYTE = 0x0000002b;
    public static final int TYPE_UINT = 0x00000022;
    public static final int TYPE_ULONG = 0x00000024;
    public static final int TYPE_USHORT = 0x0000002d;
    public static final int TYPE_UUID = 0x00000527;

    /**
     * Verifies field type id
     * @param field Field to check
     * @return true if the type id is OK
     */
    public static boolean validateField(FieldConfigI field)
    {
        int fieldTypeId = field.getTypeId();
        switch (fieldTypeId)
        {
        case TYPE_INT:
        case TYPE_UINT:
        case TYPE_LONG:
        case TYPE_ULONG:
        case TYPE_FLOAT:
        case TYPE_DOUBLE:
        case TYPE_STRING:
        case TYPE_HEXBINARY:
        case TYPE_BOOLEAN:
        case TYPE_BYTE:
        case TYPE_UBYTE:
        case TYPE_SHORT:
        case TYPE_USHORT:
        case TYPE_DATETIME:
        case TYPE_DATETIMEMSEC:
        case TYPE_IP4ADDR:
        case TYPE_IP6ADDR:
        case TYPE_UUID:
        case TYPE_DATETIMEUSEC:
        case TYPE_MACADDRESS:
            return true;
        default:
            return false;
        }
    }


    /**
     * Verifies field type id
     * @param field Field to check
     * @return true if the type id is OK
     */
    public static String getTypeName(int fieldTypeId)
    {
        switch (fieldTypeId)
        {
        case TYPE_INT:
            return "int";
        case TYPE_UINT:
            return "unsigned int";
        case TYPE_LONG:
            return "long";
        case TYPE_ULONG:
            return "unsigned long";
        case TYPE_FLOAT:
            return "float";
        case TYPE_DOUBLE:
            return "double";
        case TYPE_STRING:
            return "string";
        case TYPE_HEXBINARY:
            return "binary";
        case TYPE_BOOLEAN:
            return "boolean";
        case TYPE_BYTE:
            return "byte";
        case TYPE_UBYTE:
            return "unsigned byte";
        case TYPE_SHORT:
            return "short";
        case TYPE_USHORT:
            return "unsigned short";
        case TYPE_DATETIME:
            return "date/time (sec)";
        case TYPE_DATETIMEMSEC:
            return "date/time (msec)";
        case TYPE_IP4ADDR:
            return "IPv4 address";
        case TYPE_IP6ADDR:
            return "IPv6 address";
        case TYPE_UUID:
            return "UUID";
        case TYPE_DATETIMEUSEC:
            return "date/time (usec)";
        case TYPE_MACADDRESS:
            return "MAC address";
        default:
            return null;
        }
    }
}