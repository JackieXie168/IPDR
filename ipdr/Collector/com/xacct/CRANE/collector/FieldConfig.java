package com.xacct.CRANE.collector;

// $Id: FieldConfig.java,v 1.4 2005/01/30 15:13:41 ilyad Exp $
// Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.util.ArrayList;
import java.util.Iterator;

/** The field information for a CRANE server (collector).
 *
 *  @version $Id: FieldConfig.java,v 1.4 2005/01/30 15:13:41 ilyad Exp $
 */
public final class FieldConfig implements ConfigObject, Comparable, java.io.Serializable
{
    /** $Id: FieldConfig.java,v 1.4 2005/01/30 15:13:41 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: FieldConfig.java,v 1.4 2005/01/30 15:13:41 ilyad Exp $"; }

    /** The ISM SDK field descriptor
     */
    private FieldDescriptor fieldDesc;
    public final FieldDescriptor getFieldDesc() { return fieldDesc; }
    public final void setFieldDesc(FieldDescriptor fieldDesc)
    throws ConfigException {
        this.fieldDesc = fieldDesc;
        if (fieldDesc != null) {
            this.xacctKeyType = fieldDesc.getFieldType().getLabel();
            this.xacctKeyName = fieldDesc.getLabel();
            try {
                this.xacctFieldId = fieldDesc.getPrimitiveID();
            } catch (InvalidFieldDescriptorException ex) {
                throw new ConfigException(ex);
            }
        }
    }

    /** Automap this field, by creating a FieldDescriptor with the
     *  same name.
     *  @see TemplateConfig#automapAndEnableAllFields
     *
     *  @param collectorConfig The collector's configuration, in which
     *       this field configuration resides.
     *  @param fieldFactory The factory for creating fields.
     *  @throws ConfigException If the field information is invalid.
     */
    public final void automapAndEnable(CollectorConfig collectorConfig,
                                       FieldDescriptorFactory fieldFactory)
        throws ConfigException {
        try {
            setFieldDesc(fieldFactory.createFieldDescriptor(typeCRANE2Xacct(), getName()));
        } catch (InvalidFieldLabelException ex) {
            throw new ConfigException(ex);
        } catch (FieldRegistrationException ex) {
        }

        enabledForOutput = true;
    }


    /** The key's unique identifier (unique across templates!).
     *  &lt;KeyID&gt;...&lt;/KeyID&gt;
     */
    private int id;
    public final int getId() { return id; }
    public final void setFieldId(int id) { this.id = id; }

    /** The key's type id (see CRANE protocol doc for a list).
     *  &lt;KeyTypeId&gt;...&lt;/KeyTypeId&gt;
     */
    private int typeId;
    public final int getTypeId() { return typeId; }
    // public final void setTypeId(int typeId) { this.typeId = typeId; }

    /** The key's CRANELabel.
     *  &lt;KeyLabel&gt;...&lt;/KeyLabel&gt;
     *
     *  XXX get from FieldDescriptor?
     */
    private String CRANELabel;
    public final String getLabel() { return CRANELabel; }

    // public final void setLabel(String CRANELabel) { this.CRANELabel = CRANELabel; }

    /** The XACCT key ID.
     *  &lt;XACCTKeyID&gt;...&lt;/XACCTKeyID&gt;
     *
     *  XXX ??? get from fieldDesc.getPrimitiveID()
     */
    private int xacctFieldId;
    public final int getXacctFieldId() { return xacctFieldId; }
    /** You should <em>not</em> do this method because the information
     *  is already in the field descriptor -- set the field descriptor
     *  instead and the values will propagate.
     */
    private final void setXacctFieldId(int xacctFieldId) {
        throw new LogicException(this, "setXacctFieldId",
                                 "This operation is not supported: you should " +
                                 "use setFieldDesc instead. " +
                                 "new xacctFieldId=" + xacctFieldId);
        // this.xacctFieldId = xacctFieldId;
        // can't do this: this.fieldDesc.setPrimitiveID(xacctFieldId);
    }

    /** The XACCT key type
     *  derived from xacctFieldId.
     *
     *  XXX ??? get from fieldDesc.getFieldType().getLabel()
     */
    private String xacctKeyType;

    /** The XACCT key name.
     *
     *  XXX ??? get from fieldDesc ???
     */
    private String xacctKeyName;

    /** The CRANE name.
     *
     *  &lt;KeyName&gt;...&lt;/KeyName&gt; .
     *
     *  XXX ??? get from fieldDesc.getLabel()
     */
    private String CRANEName;
    public final String getName() { return CRANEName; }
    /** You should <em>not</em> do this method because the information
     *  is already in the field descriptor -- set the field descriptor
     *  instead and the values will propagate.
     */
    private final void setName(String name) {
        throw new LogicException(this, "setName",
                                 "This operation is not supported: you should " +
                                 "use setFieldDesc instead. " +
                                 "new name: " + name);
        // this.name = name;
        // can't do this: this.fieldDesc.setLabel(name);
    }

    /*** Help string.
     *
     *  &lt;KeyHelp&gt;...&lt;/KeyHelp&gt; .
     *
     *  XXX get from FieldDescriptor?
     */
    private String help;
    public final String getHelp() { return help; }
    // public final void setHelp(String help) { this.help = help; }

    private boolean enabledForOutput;
    public final boolean isEnabledForOutput() { return enabledForOutput; }
    public final void setEnabledForOutput(boolean value) { this.enabledForOutput = value; }


    // Not needed ... part of FieldDescriptor ...
//     String xacctFieldLabel;
//     public final String getXacctTypeId() { return xacctTypeId; }
//     /** You should <em>not</em> do this method because the information
//      *  is already in the field descriptor -- set the field descriptor
//      *  instead and the values will propagate.
//      */
//     private final void setXacctFieldLabel(String xacctFieldLabel) {
//         throw new LogicException(this, "setXacctFieldLabel" +
//                                  "This operation is not supported: you should " +
//                                  "use setFieldDesc instead. " +
//                                  "new xacctFieldLabel=" + xacctFIeldLabel);
//         this.xacctFieldLabel = xacctFieldLabel;
//         // can't do this: this.fieldDesc.getFieldType.setLabel(...)
//     }

    //String xacctType; // derived from xacctFieldId
    // public final String getXacctFieldLabel() { return xacctFieldLabel; }
    // public final void setXacctType(String xacctType) { this.xacctType = xacctType; }

    /** Null constructor: you must call set() to fill things in.
     */
    public FieldConfig() {
        this.setWithoutFieldDesc(-1, // id
                                 -1, // typeId
                                 null, // CRANELabel
                                 null, // help
                                 false); // enabledForOutput
    }


    /** Get a field in a list of fields, using the field id,
     *  throwing an exception if it's not there.
     * @param container the containing object (for error messages)
     * @param fieldConfigList the list of fields
     * @param matchId the (unique) field id
     * @return the field whose id matches
     * @throws ConfigException if matchId not found
     * @see FieldConfig#lookupById
     */
    public final static FieldConfig getById(Object container,
                                            ArrayList fieldConfigList,
                                            int matchId)
        throws ConfigException {
        FieldConfig result = lookupById(container, fieldConfigList, matchId);
        if (result == null) {
            throw new ConfigException(container, "FieldConfig.getById",
                                      "Unknown field: id=" + matchId + " in " + container +
                                      " ... looked in: " + CollectorUtil.toStringLong(fieldConfigList));
        }
        return result;
    }

    /** Look up a field in a list of fields, using the field id,
     *  returning null if it's not therre.
     * @param container the containing object (for error messages)
     * @param fieldConfigList the list of fields
     * @param matchId the (unique) field id
     * @return the field whose id matches, null if not found.
     * @see FieldConfig#getById
     */
    public final static FieldConfig lookupById(Object container,
                                               ArrayList fieldConfigList,
                                               int matchId) {
        FieldConfig result = null;
        for (Iterator it = fieldConfigList.iterator();
             result == null && it.hasNext();
            ) {
            FieldConfig elem = (FieldConfig)it.next();
            if (elem.getId() == matchId) {
                result = elem;
            }
        }
        return result;
    }

    public FieldConfig(int id,
                       int typeId,
                       String label,
                       String help,
                       boolean enabledForOutput) {
        this.setWithoutFieldDesc(id,
                                 typeId,
                                 label,
                                 help,
                                 enabledForOutput);
    }

    public FieldConfig(int id,
                       int typeId,
                       String label,
                       String help,
                       boolean enabledForOutput,
                       FieldDescriptor fd)
    throws ConfigException {
        this.setWithoutFieldDesc(id,
                                 typeId,
                                 label,
                                 help,
                                 enabledForOutput);
        setFieldDesc(fd);
    }

    /** Dangerous set-er because it doesn't setFieldDesc().
     */
    public final void setWithoutFieldDesc(int id,
                                          int typeId,
                                          String label,
                                          String help,
                                          boolean enabledForOutput){
        this.id                = id;
        this.typeId            = typeId;
        this.CRANELabel             = CollectorUtil.nonNull(label);
        this.help              = CollectorUtil.nonNull(help);
        this.CRANEName         = this.CRANELabel;
        this.enabledForOutput  = enabledForOutput;
//        this.xacctFieldId      = xacctFieldId;
//        this.xacctKeyType      = CollectorUtil.nonNull(xacctKeyType);
//        this.name              = CollectorUtil.nonNull(name);
//        this.help              = CollectorUtil.nonNull(help);
    }


    /** Get the FieldType of the field.
     */
    public final FieldType typeCRANE2Xacct() throws ConfigException {
        return typeCRANE2Xacct(getTypeId(), getLabel(), getName());
    }

    /** convert from a CRANE typeID to a FieldType.
     *
     * @param typeId the CRANE typeID.
     * @param label the field label (used for exception message).
     * @param name the field name (used for exception message).
     * @return the corresponding field type.
     * @throws ConfigException If the typeID wasn't convertable.
     */
    public static final FieldType typeCRANE2Xacct(int typeId, String label, String name) throws ConfigException {
        FieldType type;
        switch (typeId) {
        case ProtocolMsgKeyBlock.TYPE_BOOLEAN:
            type = FieldType.BOOLEAN;
            break;
        case ProtocolMsgKeyBlock.TYPE_INT_8:
        case ProtocolMsgKeyBlock.TYPE_UINT_8:
        case ProtocolMsgKeyBlock.TYPE_INT_16:
            type = FieldType.SHORT;
            break;
        case ProtocolMsgKeyBlock.TYPE_UINT_16:
        case ProtocolMsgKeyBlock.TYPE_INT_32:
            type = FieldType.INT;
            break;
        case ProtocolMsgKeyBlock.TYPE_UINT_32:
        case ProtocolMsgKeyBlock.TYPE_INT_64:
        // XXX need to warn the user
        case ProtocolMsgKeyBlock.TYPE_UINT_64:
            type = FieldType.LONG;
            break;
        case ProtocolMsgKeyBlock.TYPE_FLOAT:
            type = FieldType.FLOAT;
            break;
        case ProtocolMsgKeyBlock.TYPE_DOUBLE:
            type = FieldType.DOUBLE;
            break;
        case ProtocolMsgKeyBlock.TYPE_STRING:
        case ProtocolMsgKeyBlock.TYPE_NT_STRING:
            type = FieldType.STRING;
            break;
        case ProtocolMsgKeyBlock.TYPE_IPV4_ADDR:
            type = FieldType.IPADDRESS;
            break;
        case ProtocolMsgKeyBlock.TYPE_TIME_SEC:
        case ProtocolMsgKeyBlock.TYPE_TIME_MSEC:
        // XXX need to warn the user
        case ProtocolMsgKeyBlock.TYPE_TIME_USEC:
            type = FieldType.DATE;
            break;
        default:
            throw new ConfigException("Test", "addNewField", "Unknown field typeID: " + typeId +
                                      " label(" + label + " name(" + name + ")");
        }
        return type;
    }

    /** Check whether two keys are "similar" in the sense that all the
     *  important fields match. This is used to check that the key from
     *  a transmitter matches a key in the configuration.
     */
    public final boolean similarTo(FieldConfig other) {
        return this.id == other.id &&
            this.typeId == other.typeId &&
            // names must be equal, see protocol for GET TMPL RSP
            (this.CRANELabel.equals("") || other.CRANELabel.equals("") || this.CRANELabel.equals(other.CRANELabel));
    }


    // stuff for Comparable, Cloneable

    public final boolean equals(Object other) {
        try {
            return equals((FieldConfig)other);
        } catch (ClassCastException e) {
            return false;
        }
    }

    public final boolean equals(FieldConfig other) {
        return other != null && (0 == this.compareTo(other));
    }

    public final int hashCode() { return id; }

    public final int compareTo(Object other) {
        return compareTo((FieldConfig)other);
    }

    public final int compareTo(FieldConfig other) {
        if (this == other) { return 0; }
        int result = 0;
        if (result == 0) { result = this.id - other.id; }
        if (result == 0) { result = this.typeId - other.typeId; }
        if (result == 0) { result = this.CRANELabel.compareTo(other.CRANELabel); }
        if (result == 0) {
            if (this.fieldDesc == null && other.fieldDesc != null) { result = 1;
            } else if (this.fieldDesc != null && other.fieldDesc == null) { result = -1;
            }
        }
        if (result == 0 && this.fieldDesc != null && other.fieldDesc != null) {
                           result = (this.fieldDesc.equals(other.fieldDesc))? 0 : 1;
        }
//        if (result == 0) { result = this.xacctKeyType.compareTo(other.xacctKeyType); }
//        if (result == 0) { result = this.name.compareTo(other.name); }
        if (result == 0) { result = this.help.compareTo(other.help); }
        return result;
    }

    public final String toString() {
        return "Field[" + id + ":" + CRANELabel +
            (isEnabledForOutput() ? "" : "/disabled") +
            ((fieldDesc == null) ? "" : (" " + fieldDesc)) +
            "]";
    }

    /// A possibly longer description of the key

    public final String toStringLong() {
        return toStringLong("");
    }

    /** A possibly longer description of the key
     *  @param indent A string to put at the beginning of each line
     *  @return A (possibly multi-line) description
     */
    public final String toStringLong(String indent) {
        return "Field[" +
            "id=" + id +
            " CRANELabel \"" + CRANELabel  +
            (isEnabledForOutput() ? "" : "/disabled") +
            ((fieldDesc == null) ? "" : (" " + fieldDesc)) +
            " " + ProtocolMsgKeyBlock.interpretTypeId(typeId) +
            " help \"" + help + "\"" +
            "]";
    }
}
