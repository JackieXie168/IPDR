package org.ipdr.protocol;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.util.ArrayList;

/**
 * The class represents a GET_TEMPLATES_RESPONSE message
 * The message has the following structure:
 * <xmp>
 * struct GetTemplatesResponse 
 * { 
 * 		struct IPDRStreamingHeader header; 
 * 		short requestId; 
 * 		short configId; 
 * 		TemplateBlock currentTemplates <>; 
 * }
 * </xmp>
 * A TemplateBlock has the following structure:
 * <xmp>
 * struct TemplateBlock
 * { 
 * 		short templateId; // ID of Template - context within configId 
 * 						  // Provides numeric Identifier to 
 * 						  // IPDR service specification for context of 
 * 						  // Session/config 
 * 		UTF8String schemaName; // Reference to IPDR service specification 
 * 		UTF8String typeName; // Reference to IPDR service specification 
 * 		FieldDescriptor fields<>; // Fields in this Template 
 * }
 * </xmp>
 * 
 *A FieldDescriptor has the following structure:
 *<xmp>
 * struct FieldDescriptor
 * { 
 * 		int typeId; // ID of Field Type 
 * 		int fieldId; // unqualified Field code that can be used 
 * 					 // as alternative accessor handles to Fields 
 * 		UTF8String fieldName; // Note that Field names are to be qualified 
 * 							  // with the Namespace name, as an example:  
 * 							  // http://www.ipdr.org/namespace:movieId  
 * 							  // The namespace MUST match one of those 
 * 							  // targeted by the schema or schema imports 
 * 		boolean isEnabled // true=enabled Field false=disabled Field 
 * }
 *</xmp>
 */
public class ProtocolMsgGET_TEMPLATES_RESPONSE extends ProtocolStreamMsg
{
    /**
     * @see org.ipdr.protocol.ProtocolMsgIdentification#msgName()
     */
    public String msgName()
    {
        return "GET_TEMPLATES_RESPONSE";
    }

    public static final byte MSG_ID = 0x17;

    /**
     * @see org.ipdr.protocol.ProtocolStreamMsg#msgId()
     */
    public final byte msgId()
    {
        return MSG_ID;
    }

    /**
     * Holds config id	
     */
    protected short configId;

    /**
     * Holds request id
     */
    protected short requestId;

    /**
     * Holds <code>ArrayList</code> of template info blocks
     */
    protected ArrayList templates;

    /**
     * Creates message from values
     * @param requestId request id
     * @param configId config id
     * @param templates list of templates
     */
    protected ProtocolMsgGET_TEMPLATES_RESPONSE(short requestId,
            short configId, ArrayList templates)
    {
        this.requestId = requestId;
        this.configId = configId;
        this.templates = templates;
    }

    /**
     * Creates message from raw bytes
     * @param message raw bytes
     * @param length buffer length
     * @throws ProtocolMalformedMsgException
     */
    protected ProtocolMsgGET_TEMPLATES_RESPONSE(byte[] message, int length)
            throws ProtocolMalformedMsgException
    {
        super(message, length, MSG_LENGTH);
        this.configId = bigE.getShort(message, CONFIG_ID_OFFSET);
        this.requestId = bigE.getShort(message, REQUEST_ID_OFFSET);
        // load and parse templates
        this.templates = bigE.getTemplateBlock(message, TEMPLATES_OFFSET);
    }

    protected static final int REQUEST_ID_OFFSET = 8;

    protected static final int CONFIG_ID_OFFSET = 10;

    protected static final int TEMPLATES_OFFSET = 12;

    protected static final int MSG_LENGTH = 12;

    /**
     * @see org.ipdr.protocol.ProtocolMsg#createRawMsg()
     */
    protected byte[] createRawMsg()
    {
        this.msgLength = TEMPLATES_OFFSET
                + bigE.getTemplateBlockLength(templates);
        byte[] message = new byte[this.msgLength];
        setHeader(message);
        bigE.putShort(message, REQUEST_ID_OFFSET, requestId);
        bigE.putShort(message, CONFIG_ID_OFFSET, configId);
        bigE.putTemplateBlock(message, TEMPLATES_OFFSET, templates);
        return message;
    }

    /**
     * Returns the configId.
     * 
     * @return short
     */
    public short getConfigId()
    {
        return configId;
    }

    /**
     * Returns the templates.
     * 
     * @return ArrayList
     */
    public ArrayList getTemplates()
    {
        return templates;
    }

    /**
     * Returns the requestId.
     * 
     * @return short
     */
    public short getRequestId()
    {
        return requestId;
    }

    /**
     * @see java.lang.Object#toString()
     */
    public String toString()
    {
        return toStringBase() + "[" + "request:" + getRequestId() + ","
                + "config:" + getConfigId() + "," + "template count:"
                + getTemplates().size() + "]";
    }

}