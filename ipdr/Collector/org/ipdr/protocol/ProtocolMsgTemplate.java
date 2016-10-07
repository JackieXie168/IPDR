package org.ipdr.protocol;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.util.ArrayList;

/**
 * The class is a common ancestor of all classes which contain a template block.
 * All such messages share the following structure:
 * <xmp>
 * struct ModifyTemplate 
 * { 
 * 		struct IPDRStreamingHeader header; 
 * 		short configId; 		// Identifies context of Template definitions.
 * 								// Changes in Template MUST use a different 
 * 								// configId (0 if unused) 
 *  	char flags; 			// 
 * 		TemplateBlock changeTemplates<>; // Definitions of Templates  
 * }
 * </xmp>
 * TemplateBlock has the following structure:
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
 * FieldDescriptor has the following structure:
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
public abstract class ProtocolMsgTemplate extends ProtocolStreamMsg
{
	/**
	 * configuration id
	 */
	protected short configId;
	/**
	 * Flags field
	 */
	protected byte flags;
	/**
	 * Template block
	 */
	protected ArrayList templates;

	/**
	 * Constructs the message from values
	 * @param sessionId session id
	 * @param configId configuration id
	 * @param flags flags byte
	 * @param templates template block
	 */
	protected ProtocolMsgTemplate( 
	byte sessionId,
	short configId,
	byte flags,
	ArrayList templates)
	{
		this.sessionId = sessionId;
		this.configId = configId;
		this.flags = flags;
		this.templates = templates;
	}

	/**
	 * Constructs the message from raw bytes
	 * @param message raw bytes
	 * @param length buffer length
	 * @throws ProtocolMalformedMsgException
	 */
	protected ProtocolMsgTemplate(
	byte [] message, int length) throws ProtocolMalformedMsgException
	{
		super (message, length, MSG_LENGTH);
		this.configId = bigE.getShort( message, CONFIG_ID_OFFSET);
		this.flags= bigE.getByte( message, FLAGS_OFFSET);
		// load and parse templates
		this.templates = bigE.getTemplateBlock( message, TEMPLATES_OFFSET);
	}

	protected static final int CONFIG_ID_OFFSET 	= 8;
	protected static final int FLAGS_OFFSET 		= 10;
	protected static final int TEMPLATES_OFFSET	= 11;
	protected static final int MSG_LENGTH			= 15;
	
	/**
	 * @see org.ipdr.protocol.ProtocolMsg#createRawMsg()
	 */
	protected byte[] createRawMsg() 
	{
		this.msgLength = TEMPLATES_OFFSET + bigE.getTemplateBlockLength(templates) ;
		byte [] message = new byte[this.msgLength];
		setHeader(message);
		bigE.putShort( message, CONFIG_ID_OFFSET, configId);
		bigE.putByte( message, FLAGS_OFFSET, flags );
		bigE.putTemplateBlock( message, TEMPLATES_OFFSET, templates);
		return message;
	}
	/**
	 * Returns the configId.
	 * @return short
	 */
	public short getConfigId()
	{
		return configId;
	}

	/**
	 * Returns the templates.
	 * @return ArrayList
	 */
	public ArrayList getTemplates()
	{
		return templates;
	}

	/**
	 * @see java.lang.Object#toString()
	 */
	public String toString()
	{
		return toStringBase() + "[config:" + getConfigId() + ","
								+ "templates count:" + getTemplates().size() + "]";
	}

}
