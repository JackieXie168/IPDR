package org.ipdr.crane;

import org.ipdr.api.configuration.FieldConfig;

// $Id: ProtocolMsgExtendedKeyBlock.java,v 1.4 2005/01/30 15:13:46 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/** The Java encapsulation of the extended key block in such CRANE
 *  protocol messages as "GET_TMPL_RSP".
 *
 *  Extended Key Block Format:
 *
 *  XXX The protocol document is wrong XXX
 *
 *  <xmp>
 *       0                   1                   2                   3
 *       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *      |                            Key ID                             |
 *      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *      |          Key Type ID          |        Key Name Length        |
 *      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *      |          Key Label Length     |        Key Help Length        |
 *      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *      |                      Key Attribute Vector                   |K|
 *      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *      |                                                               |
 *      ~                            Key Name                           ~
 *      |                                                               |
 *      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *      |                                                               |
 *      ~                            Key Label                          ~
 *      |                                                               |
 *      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *      |                                                               |
 *      ~                            Key Help                           ~
 *      |                                                               |
 *      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  </xmp>
 *
 *  @version $Id: ProtocolMsgExtendedKeyBlock.java,v 1.4 2005/01/30 15:13:46 ilyad Exp $
 */
public final class ProtocolMsgExtendedKeyBlock implements ProtocolMsgKeyBlockHandler
{
    /** $Id: ProtocolMsgExtendedKeyBlock.java,v 1.4 2005/01/30 15:13:46 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: ProtocolMsgExtendedKeyBlock.java,v 1.4 2005/01/30 15:13:46 ilyad Exp $"; }

    public ProtocolMsgExtendedKeyBlock() { }

    public int getFromRaw(byte [] msg, int offset, FieldConfig field) {
        // CollectorLog.debug(this, "getFromRawKey", "offset=" + offset + " i=" + fieldI);
        // dumpHex(msg, offset, 100);
        int keyId     = (int)ProtocolMsg.bigE.getUnsignedInt(  msg, offset + KEY_ID_OFFSET);
        int keyTypeId      = ProtocolMsg.bigE.getUnsignedShort(msg, offset + KEY_TYPE_ID_OFFSET);
        int keyNameLength  = ProtocolMsg.bigE.getUnsignedShort(msg, offset + KEY_NAME_LENGTH_OFFSET);
        int keyLabelLength = ProtocolMsg.bigE.getUnsignedShort(msg, offset + KEY_LABEL_LENGTH_OFFSET);
        int keyHelpLength  = ProtocolMsg.bigE.getUnsignedShort(msg, offset + KEY_HELP_LENGTH_OFFSET);
        int keyAttrOffset  = KEY_ATTRIBUTES_OFFSET;
        int keyNameOffset  = KEY_NAME_OFFSET;
        int keyLabelOffset = keyNameOffset  + ProtocolMsg.round32(keyNameLength);
        int keyHelpOffset  = keyLabelOffset + ProtocolMsg.round32(keyLabelLength);
        int endBlockOffset = keyHelpOffset + ProtocolMsg.round32(keyHelpLength);

        int keyAttributeVector = ProtocolMsg.bigE.getInt(msg, offset + keyAttrOffset);

        // CollectorLog.debug(this, "getFromRaw",
        //                   "keyId=" + keyId +
        //                   " keyTypeId=" + FieldConfig.interpretTypeId(keyTypeId) +
        //                   " keyLabelLength=" + keyLabelLength +
        //                   " keyNameLength=" + keyNameLength +
        //                   " keyHelpOffset=" + keyHelpOffset +
        //                   " keyLabelOffset=" + keyLabelOffset +
        //                   " keyHelpLength=" + keyHelpLength +
        //                   " keyNameOffset=" + keyNameOffset +
        //                   " keyAttrOffset=" + keyAttrOffset +
        //                   " keyAttributeVector=" + keyAttributeVector);

        String keyName     = ProtocolMsg.bigE.getStringWithLen(msg, keyNameLength,  offset + keyNameOffset);
        String keyLabel    = ProtocolMsg.bigE.getStringWithLen(msg, keyLabelLength, offset + keyLabelOffset);
        String keyHelp     = ProtocolMsg.bigE.getStringWithLen(msg, keyHelpLength,  offset + keyHelpOffset);

        boolean enabled = ((keyAttributeVector & KEY_ATTRIBUTE_DISABLED) == 0);

        // CollectorLog.debug(this, "getFromRaw",
        //                   "label(" + keyLabel +
        //                   ") name(" + keyName +
        //                   ") help(" + keyHelp);

        field.setId(keyId);
        field.setTypeId(keyTypeId);
        field.setName(keyLabel);
        field.setEnabledForOutput(enabled);

        return ProtocolMsg.round32(offset + endBlockOffset);
    }

    // The following are relative offsets within an extended key block

    private static final int KEY_ID_OFFSET                =  0;
    private static final int KEY_TYPE_ID_OFFSET           =  4;
    private static final int KEY_NAME_LENGTH_OFFSET       =  6;
    private static final int KEY_LABEL_LENGTH_OFFSET      =  8;
    private static final int KEY_HELP_LENGTH_OFFSET       = 10;
    private static final int KEY_ATTRIBUTES_OFFSET        = 12;
    private static final int KEY_NAME_OFFSET              = 16;

    public  static final int KEY_ATTRIBUTE_DISABLED       = 0x01;
}
