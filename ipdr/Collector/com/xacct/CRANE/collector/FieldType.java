package com.xacct.CRANE.collector;

//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.
import java.io.ObjectStreamException;
import java.util.HashMap;
import java.util.Map;

/** Defines all valid FieldDescriptor field types.
 *
 *  There are three kinds of static members exposed by this class -
 *  FieldType instances, FieldType IDs and FieldType labels. The
 *  FieldType instances are the actual types, to be used for creating
 *  FieldDescriptors.
 *
 *  Each field type has an identifier and label, which can be accessed
 *  by using its <code>getID()</code> and <code>getLabel()</code>
 *  methods. To allow for greater code efficiency, the IDs and labels
 *  used by all the FieldType instances are also provided as static
 *  members (<code>FieldType.STRING</code>'s ID is
 *  <code>FieldType.STRING_ID</code> and its label is
 *  <code>FieldType.STRING_LABEL</code>).
 */
public final class FieldType implements java.io.Serializable
{
    /** $Id: FieldType.java,v 1.3 2005/01/30 15:13:42 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: FieldType.java,v 1.3 2005/01/30 15:13:42 ilyad Exp $"; }

    // IMPORTANT NOTE: These static initialized Map objects MUST be
    // initialized before the static FieldType objects. The static
    // FieldType construction depends on the previous initialization
    // of these Map objects.

    /** class static: lookup FieldType by ID */
    private static final Map idToFieldType    = new HashMap();
    /** class static: lookup FieldType by label */
    private static final Map labelToFieldType = new HashMap();

    /** FieldType.INT unique id */
    public static final int INT_ID        = ProtocolMsgKeyBlock.TYPE_INT_32;
    /** FieldType.STRING unique id */
    public static final int STRING_ID     = ProtocolMsgKeyBlock.TYPE_STRING;
    /** FieldType.BOOLEAN unique id */
    public static final int BOOLEAN_ID    = ProtocolMsgKeyBlock.TYPE_BOOLEAN;
    /** FieldType.SHORT unique id */
    public static final int SHORT_ID      = ProtocolMsgKeyBlock.TYPE_INT_16;
    /** FieldType.LONG unique id */
    public static final int LONG_ID       = ProtocolMsgKeyBlock.TYPE_INT_64;
    /** FieldType.DATE unique id */
    public static final int DATE_ID       = ProtocolMsgKeyBlock.TYPE_TIME_MSEC;
    /** FieldType.DOUBLE unique id */
    public static final int DOUBLE_ID     = ProtocolMsgKeyBlock.TYPE_DOUBLE;
    /** FieldType.FLOAT unique id */
    public static final int FLOAT_ID      = ProtocolMsgKeyBlock.TYPE_FLOAT;
    /** FieldType.IPADDRESS unique id */
    public static final int IPADDRESS_ID  = ProtocolMsgKeyBlock.TYPE_IPV4_ADDR;

    /** FieldType.INT unique label */
    public static final String INT_LABEL         = "Integer";
    /** FieldType.STRING unique label */
    public static final String STRING_LABEL      = "String";
    /** FieldType.BOOLEAN unique label */
    public static final String BOOLEAN_LABEL     = "Boolean";
    /** FieldType.SHORT unique label */
    public static final String SHORT_LABEL       = "Short";
    /** FieldType.LONG unique label */
    public static final String LONG_LABEL        = "Long";
    /** FieldType.DATE unique label */
    public static final String DATE_LABEL        = "Date";
    /** FieldType.DOUBLE unique label */
    public static final String DOUBLE_LABEL      = "Double";
    /** FieldType.FLOAT unique label */
    public static final String FLOAT_LABEL       = "Float";
    /** FieldType.IPADDRESS unique label */
    public static final String IPADDRESS_LABEL   = "IP Address";

    /** Integer data type */
    public static final FieldType INT         = new FieldType(INT_ID,       INT_LABEL);
    /** String data type */
    public static final FieldType STRING      = new FieldType(STRING_ID,    STRING_LABEL);
    /** Boolean data type */
    public static final FieldType BOOLEAN     = new FieldType(BOOLEAN_ID,   BOOLEAN_LABEL);
    /** Short data type */
    public static final FieldType SHORT       = new FieldType(SHORT_ID,     SHORT_LABEL);
    /** Long data type */
    public static final FieldType LONG        = new FieldType(LONG_ID,      LONG_LABEL);
    /** Date data type */
    public static final FieldType DATE        = new FieldType(DATE_ID,      DATE_LABEL);
    /** Double data type */
    public static final FieldType DOUBLE      = new FieldType(DOUBLE_ID,    DOUBLE_LABEL);
    /** Float data type */
    public static final FieldType FLOAT       = new FieldType(FLOAT_ID,     FLOAT_LABEL);
    /** IP address data type */
    public static final FieldType IPADDRESS   = new FieldType(IPADDRESS_ID, IPADDRESS_LABEL);

    /** type label */
    private String m_label;
    /** type ID */
    private int m_ID;

    /** Creates a new FieldType. This constructor is private access,
     *  because the only valid FieldType instances are the static
     *  FieldType members in this * class.  Therefore the constructor
     *  is only accessible internally.
     *
     * @param ID unique ID
     * @param label unique label
     */
    private FieldType(int ID, String label) {
        m_ID = ID;
        m_label = label;

        // add the new FieldType to the static FieldType lookup tables, which allow
        // lookup of a FieldType by ID and by label
        if (FieldType.idToFieldType.put(new Integer(ID), this) != null) {
            throw new LogicException("Internal error: FieldType with ID=" + ID + " cannot be created more than once");
        }
        if (FieldType.labelToFieldType.put(label, this) != null) {
            throw new LogicException("Internal error: FieldType with label=" + label + " cannot be created more than once");
        }
    }

    /** java.io.Serializable method implementation. When deserializing
     *  a FieldType, a new instance is created. This is not desirable,
     *  because that would potentially mean that there are multiple
     *  instances of FieldType with the same unique ID and label.
     *  While this is not disastrous, it means that it is not possible
     *  to compare two FieldType instances with the <code>==</code>
     *  operator, as they may be different instances of the same type.
     *
     *  The <code>readResolve()</code> method replaces the deserialized
     *  instance with the correct static final instance of FieldType --
     *  deserializing a FieldType will return a reference to the
     *  <B>static</B> FieldType with the same ID.
     *
     * @throws ObjectStreamException if the method failed
     * @return one of the static FieldType instances
     */
    private Object readResolve() throws ObjectStreamException  {
        return FieldType.getFieldType(new Integer(this.getID()));
    }

    /** Returns the unique identifier for this FieldType.
     *
     * @return the unique identifier for this FieldType
     */
    public int getID() {
        return m_ID;
    }

    /** Returns the unique label for this FieldType.
     *
     * @return the unique label for this FieldType
     */
    public String getLabel()
    {
        return m_label;
    }

    public int hashCode() {
        return m_ID;
    }

    /** Checks if two FieldTypes are equal. Done by comparing their unique IDs.
     *
     * @param other the FieldType to compare this FieldType with.
     *
     * @return true if the FieldTypes have the same unique ID.
     */
    public boolean equals(Object other) {
        if ((other != null) && (other instanceof FieldType)) {
            return m_ID == ((FieldType)other).m_ID;
        }
        return false;
    }

    /** Returns this FieldType's string representation.
     *
     * @return this FieldType's string representation
     */
    public String toString() {
        return getLabel();
    }

    /** Returns the static FieldType object that has the specified unique ID,
     *  or <code>null</code> if no such object exists.
     *
     * @param typeID a FieldType ID
     *
     * @return the static FieldType object that has the specified unique ID,
     *         or <code>null</code> if no such object exists.
     */
    public static FieldType getFieldType(Integer typeID) {
        return (FieldType)idToFieldType.get(typeID);
    }

    /** Returns the static FieldType object that has the specified label,
     *  or <code>null</code> if no such object exists.
     *
     * @param label a FieldType label
     *
     * @return the static FieldType object that has the specified label,
     *         or <code>null</code> if no such object exists.
     */
    public static FieldType getFieldType(String label) {
        return (FieldType)labelToFieldType.get(label);
    }

    /** Returns an array containing all the FieldType instances.
     *
     * @return an array containing all the FieldType instances.
     */
    public static FieldType[] getAllFieldTypes() {
        return (FieldType[])idToFieldType.values().toArray(new FieldType[idToFieldType.size()]);
    }

    /** Returns an array containing all the FieldType labels.
     *
     * @return an array containing all the FieldType labels.
     */
    public static String[] getAllFieldTypeLabels() {
        return (String[])labelToFieldType.keySet().toArray(new String[labelToFieldType.size()]);
    }

    /** Returns an array containing all the FieldType IDs.
     *
     * @return an array containing all the FieldType IDs.
     */
    public static Integer[] getAllFieldTypeIDs() {
        return (Integer[])idToFieldType.keySet().toArray(new Integer[idToFieldType.size()]);
    }
}
