package com.xacct.CRANE.collector;

// $Id: TestFieldDescriptor.java,v 1.4 2005/01/30 15:13:39 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.


/** Simple implementation of FieldDescriptor for standalong testing.
 *  @version $Id: TestFieldDescriptor.java,v 1.4 2005/01/30 15:13:39 ilyad Exp $
 *  @see Test
 */
public class TestFieldDescriptor implements FieldDescriptor
{
    /** $Id: TestFieldDescriptor.java,v 1.4 2005/01/30 15:13:39 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: TestFieldDescriptor.java,v 1.4 2005/01/30 15:13:39 ilyad Exp $"; }

    private FieldType fieldType;
    private String label;
    private Integer id;

    public TestFieldDescriptor(FieldType fieldType, String label, int id) {
        this.fieldType = fieldType;
        this.label = label;
        this.id = new Integer(id);
    }

    public FieldType getFieldType() { return fieldType; }
    public void release() { }
    public String getFQN() { return getNamespace() + "::" + getLabel(); }
    public String getLabel() { return label; }
    public String getNamespace() { return "TEST"; }
    public Integer getID() { return id; }
    public int getPrimitiveID() { return id.intValue(); }

    public String toString() {
        return "TestFieldDescriptor[" + getFQN() + " #" + id + " " +
            getFieldType().getLabel() + "]";
    }
    public int hashCode() { return getPrimitiveID(); }
    public boolean equals(Object other) {
        try {
            return equals((TestFieldDescriptor)other);
        } catch (ClassCastException e) {
            return false;
        }
    }
    private final boolean equals(TestFieldDescriptor other) {
        return other != null && this.id.equals(other.id);
    }
}
