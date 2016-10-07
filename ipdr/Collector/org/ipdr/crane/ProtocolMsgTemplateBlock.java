package org.ipdr.crane;

// $Id: ProtocolMsgTemplateBlock.java,v 1.4 2005/01/30 15:13:46 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.util.ArrayList;
import java.util.ListIterator;

import org.ipdr.api.configuration.FieldConfig;
import org.ipdr.api.configuration.TemplateConfig;
import org.ipdr.api.log.LogManager;
import org.ipdr.protocol.ProtocolMalformedMsgException;

/**
 * The Java encapsulation of the template block in such CRANE protocol messages
 * as "GET_TMPL_RSP".
 * 
 * <xmp>0 1 2 3 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ | Template
 * ID | Number of Keys |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ | Template
 * Flags |T| Description Length |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ | Template
 * Block Length |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ | | ~
 * Description ~ | |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ | | ~ Key
 * Block ~ | | +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ | | ~
 * ... ... ~ | |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ | | ~ Key
 * Block ~ | | +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * </xmp>
 * 
 * @version $Id: ProtocolMsgTemplateBlock.java,v 1.1 2005/01/21 15:58:57 ilyad
 *          Exp $
 */
public final class ProtocolMsgTemplateBlock
{
    /** $Id: ProtocolMsgTemplateBlock.java,v 1.4 2005/01/30 15:13:46 ilyad Exp $ */
    public static String versionStr()
    {
        return "@(#) $Id: ProtocolMsgTemplateBlock.java,v 1.4 2005/01/30 15:13:46 ilyad Exp $";
    }

    private ArrayList templateConfigList = new ArrayList();

    public ArrayList getTemplateConfigList()
    {
        return templateConfigList;
    }

    public final ListIterator getTemplateConfigIterator()
    {
        return  templateConfigList.listIterator();
    }

    public int getTemplateConfigListLength()
    {
        return (templateConfigList == null) ? 0 : templateConfigList.size();
    }

    public TemplateConfig getTemplateConfigAt(int i)
    {
        return (TemplateConfig) templateConfigList.get(i);
    }

    public ProtocolMsgTemplateBlock(byte[] msg, int offset, int templateCount,
            ProtocolMsgKeyBlockHandler keyBlockHandler)
            throws ProtocolMalformedMsgException
    {
        this.templateConfigList = new ArrayList(templateCount);
        for (int i = 0; i < templateCount; i++)
        {
            offset = getOneFromRaw(msg, offset, keyBlockHandler);
        }
    }

    /**
     * Process the raw bytes of the message, to extract one template and field
     * information. You can put this function inside a loop, each time around
     * getting one more template. It updates this.templateConfigList.
     * 
     * @param msg
     *            Raw bytes
     * @param offset
     *            Current offset within msg
     * @param keyBlockHandler
     *            The handler for keys: it will be either for basic keys or
     *            extended keys.
     * @param log
     *            Logger.
     * @return Offset of next data.
     * @throws ProtocolMalformedMsgException
     *             If the raw bytes don't conform to the protocol.
     */
    private int getOneFromRaw(byte[] msg, int offset,
            ProtocolMsgKeyBlockHandler keyBlockHandler)
            throws ProtocolMalformedMsgException
    {
        // log.debug("ISM.ISM_187_1_0.ProtocolMsgTemplateBlock",
        // "getOneFromRaw", "offset=" + offset);
        // ProtocolMsg.dumpHex(msg, offset, 100);
        int blockLength = (int) ProtocolMsg.bigE.getUnsignedInt(msg, offset
                + TEMPLATE_BLOCK_LENGTH_OFFSET);
        int templateId = ProtocolMsg.bigE.getUnsignedShort(msg, offset
                + TEMPLATE_ID_OFFSET);
        int numKeys = ProtocolMsg.bigE.getUnsignedShort(msg, offset
                + NUMBER_KEYS_OFFSET);
        int templateFlags = ProtocolMsg.bigE.getUnsignedShort(msg, offset
                + TEMPLATE_FLAGS_OFFSET);
        int descrLength = ProtocolMsg.bigE.getUnsignedShort(msg, offset
                + TEMPLATE_DESCR_LENGTH_OFFSET);

        String description = ProtocolMsg.bigE.getStringWithLen(msg,
                descrLength, offset + TEMPLATE_DESCRIPTION_OFFSET);

        boolean forStatusResponse = ((ProtocolMsg.bigE.getShort(msg, offset
                + TEMPLATE_FLAGS_OFFSET) & TEMPLATE_STATUS_FLAG) != 0);

        // log.debug("ISM.ISM_187_1_0.ProtocolMsgTemplateBlock",
        // "getOneFromRaw",
        //           "blockLength=" + blockLength +
        //           " templateId=" + templateId +
        //           " numKeys=" + numKeys +
        //           " templateFlags=" + templateFlags +
        //           " descrLength=" + descrLength +
        //           " description(" + description + ")");

        // log.debug("ISM.ISM_187_1_0.ProtocolMsgTemplateBlock",
        // "getOneFromRaw",
        //           "keyOffset=" + (offset + TEMPLATE_DESCRIPTION_OFFSET + descrLength) +
        //           " -> " + ProtocolMsg.round32(offset + TEMPLATE_DESCRIPTION_OFFSET +
        // descrLength));

        // do *not* use description.length() because it's in characters, not
        // bytes!
        int keyOffset = ProtocolMsg.round32(offset
                + TEMPLATE_DESCRIPTION_OFFSET + descrLength);

        ArrayList fieldList = new ArrayList(numKeys); // used when creating
                                                      // TemplateConfig)

        synchronized (fieldList)
        {
            for (int i = 0; i < numKeys; i++)
            {
                FieldConfig fi = new FieldConfig(); // initialized in next line
                                                    // by getOneFromRaw():
                keyOffset = keyBlockHandler.getFromRaw(msg, keyOffset, fi); // updates
                                                                            // fi
                fieldList.add(fi);
            }
        }

        int actualBlockLength = ProtocolMsg.round32(keyOffset - offset);
        if (actualBlockLength != ProtocolMsg.round32(blockLength))
        {
            // XXX this situation is an error; but some old clients
            // (transmitters)
            // have the value wrong, so just output a nasty message
            // instead of throwing a
            // ProtocolMalformedMsgException(... msg, msg.length, offset)
            // or DataException.
            LogManager.warn("ISM.ISM_187_1_0.ProtocolMsgTemplateBlock",
                    "getOneFromRaw", "template block length ("
                            + ProtocolMsg.round32(blockLength)
                            + ") does not match actual length ("
                            + actualBlockLength + ")");
        }

        templateConfigList.add(new TemplateConfig(templateId, fieldList,
                description));
        return keyOffset; // ProtocolMsg.round32(offset + blockLength);
    }

    public final String toString()
    {
        return "{tmpl#=" + getTemplateConfigListLength() + " "
//                + CollectorUtil.toString(getTemplateConfigList()) 
                + "}";
    }

    public final String toStringLong()
    {
        return "{tmpl#=" + getTemplateConfigListLength() + " "
//                + CollectorUtil.toStringLong(getTemplateConfigList()) 
                + "}";
    }

    // The following are relative offsets within a template block

    private static final int TEMPLATE_ID_OFFSET = 0;

    private static final int NUMBER_KEYS_OFFSET = 2;

    private static final int TEMPLATE_FLAGS_OFFSET = 4;

    private static final int TEMPLATE_DESCR_LENGTH_OFFSET = 6;

    private static final int TEMPLATE_BLOCK_LENGTH_OFFSET = 8;

    private static final int TEMPLATE_DESCRIPTION_OFFSET = 12;

    private static final short TEMPLATE_STATUS_FLAG = (short) 0x01;
}