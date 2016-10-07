package org.ipdr.crane;

import org.ipdr.api.configuration.FieldConfig;
import org.ipdr.protocol.ProtocolData;

// $Id: ProtocolMsgKeyBlock.java,v 1.4 2005/01/30 15:13:45 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/** The Java encapsulation of the key block in such CRANE protocol
 *  messages as "TMPL_DATA".
 *
 *  Key Block Format:
 *  <xmp>
 *       0                   1                   2                   3
 *       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *      |                            Key ID                             |
 *      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *      |          Key Type ID          |            Reserved           |
 *      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *      |                      Key Attribute Vector                   |K|
 *      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  </xmp>
 *
 *  @version $Id: ProtocolMsgKeyBlock.java,v 1.4 2005/01/30 15:13:45 ilyad Exp $
 */
public final class ProtocolMsgKeyBlock implements ProtocolMsgKeyBlockHandler
{
    /** $Id: ProtocolMsgKeyBlock.java,v 1.4 2005/01/30 15:13:45 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: ProtocolMsgKeyBlock.java,v 1.4 2005/01/30 15:13:45 ilyad Exp $"; }

    public ProtocolMsgKeyBlock() { }

    public int getFromRaw(byte [] msg, int offset, FieldConfig field) {
        // CollectorLog.info(this, "getFromRaw", "offset=" + offset + " i=" + fieldI);
        // dumpHex(msg, offset, 100);
        int keyId         = (int)ProtocolMsg.bigE.getUnsignedInt(  msg, offset + KEY_ID_OFFSET);
        int keyTypeId          = ProtocolMsg.bigE.getUnsignedShort(msg, offset + KEY_TYPE_ID_OFFSET);
        int keyAttributeVector = ProtocolMsg.bigE.getInt(msg, offset + KEY_ATTRIBUTES_OFFSET);

        boolean enabled = ((keyAttributeVector & KEY_ATTRIBUTE_DISABLED) == 0);

        // CollectorLog.println("ProtocolMsgKeyBlock.getFromRaw" +
        //                      " keyId=" + keyId +
        //                      " keyTypeId=" + ProtocolMsgKeyBlock.interpretTypeId(keyTypeId) +
        //                      " keyAttributeVector=" + keyAttributeVector);

        field.setId(keyId);
        field.setTypeId(keyTypeId);
        field.setEnabledForOutput(enabled);

        return ProtocolMsg.round32(offset + KEY_ATTRIBUTES_OFFSET + ProtocolData.unsignedIntSize);
    }

    // The following are relative offsets within an key block

    private static final int KEY_ID_OFFSET                =  0;
    private static final int KEY_TYPE_ID_OFFSET           =  4;
    private static final int KEY_RESERVED_OFFSET          =  6;
    private static final int KEY_ATTRIBUTES_OFFSET        =  8;

    public  static final int KEY_ATTRIBUTE_DISABLED       = ProtocolMsgExtendedKeyBlock.KEY_ATTRIBUTE_DISABLED;

    public static final String interpretTypeId(int id) {
        switch (id) {
        case TYPE_BOOLEAN:      return "Boolean";
        case TYPE_UINT_8:       return "Unsigned_Integer_8";
        case TYPE_INT_8:        return "Signed_Integer_8";
        case TYPE_UINT_16:      return "Unsigned_Integer_16";
        case TYPE_INT_16:       return "Signed_Integer_16";
        case TYPE_UINT_32:      return "Unsigned_Integer_32";
        case TYPE_INT_32:       return "Signed_Integer_32";
        case TYPE_UINT_64:      return "Unsigned_Integer_64";
        case TYPE_INT_64:       return "Signed_Integer_64";

        case TYPE_FLOAT:        return "Float";
        case TYPE_DOUBLE:       return "Double";

        case TYPE_IPV4_ADDR:    return "IP_address_(Ipv4)";
        case TYPE_IPV6_ADDR:    return "IP_address_(Ipv6)";
        case TYPE_TIME_SEC:     return "Time_SEC";
        case TYPE_TIME_MSEC:    return "Time_MSEC_64";
        case TYPE_TIME_USEC:    return "Time_USEC_64";
        case TYPE_TIME_MSEC_32: return "Time_MSEC_32";
        case TYPE_TIME_USEC_32: return "Time_USEC_32";

        case TYPE_STRING:       return "String";
        case TYPE_NT_STRING:    return "Null_Terminated_String";
        case TYPE_UTF8:         return "UTF-8_String";
        case TYPE_UTF16:        return "UTF-16_String";
        case TYPE_BLOB:         return "Arbitrary_Data_(BLOB)";

        default: return "Unknown type(" + id + ")";
        }
    }

    static public final int TYPE_UNKNOWN       = 0x0000;
    static public final int TYPE_BOOLEAN       = 0x0001;
    static public final int TYPE_UINT_8        = 0x0002;
    static public final int TYPE_INT_8         = 0x0003;
    static public final int TYPE_UINT_16       = 0x0004;
    static public final int TYPE_INT_16        = 0x0005;
    static public final int TYPE_UINT_32       = 0x0006;
    static public final int TYPE_INT_32        = 0x0007;
    static public final int TYPE_UINT_64       = 0x0008;
    static public final int TYPE_INT_64        = 0x0009;
    static public final int TYPE_FLOAT         = 0x000a;
    static public final int TYPE_DOUBLE        = 0x000b;
    static public final int TYPE_STRING        = 0x400c;
    static public final int TYPE_NT_STRING     = 0x400d;
    static public final int TYPE_UTF8          = 0x400e;
    static public final int TYPE_UTF16         = 0x400f;
    static public final int TYPE_IPV4_ADDR     = 0x0010;
    static public final int TYPE_IPV6_ADDR     = 0x0011;
    static public final int TYPE_TIME_SEC      = 0x0012;
    static public final int TYPE_TIME_MSEC     = 0x0013;
    static public final int TYPE_TIME_USEC     = 0x0014;
    static public final int TYPE_TIME_MSEC_32  = 0x0015;
    static public final int TYPE_TIME_USEC_32  = 0x0016;
    static public final int TYPE_BLOB          = 0x4017; // XXX Does not match doc

}
