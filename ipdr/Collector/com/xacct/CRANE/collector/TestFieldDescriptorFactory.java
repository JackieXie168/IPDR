package com.xacct.CRANE.collector;

// $Id: TestFieldDescriptorFactory.java,v 1.4 2005/01/30 15:13:42 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.util.Iterator;
import java.util.TreeMap;

/** Simple implementation of FieldDescriptorFactory for standalone testing.
 *  @version $Id: TestFieldDescriptorFactory.java,v 1.4 2005/01/30 15:13:42 ilyad Exp $
 *  @see Test
 */
public class TestFieldDescriptorFactory implements FieldDescriptorFactory
{
    public static String versionStr() { return "@(#) $Id: TestFieldDescriptorFactory.java,v 1.4 2005/01/30 15:13:42 ilyad Exp $"; }

    public TestFieldDescriptorFactory() { this(100); }

    /** Constructor.
     *  @param nextId The identifier number to assign to the next
     *                FieldDescriptor that is created.
     *  @see TestFieldDescriptorFactory#createFieldDescriptor
     */
    public TestFieldDescriptorFactory(int nextId) {
        this.nextId = nextId;
    }

    public FieldDescriptor createFieldDescriptor(FieldType type, String label)
        throws InvalidFieldLabelException, FieldRegistrationException {
        validateFieldLabel(label);
        FieldDescriptor result = (FieldDescriptor)labelToFieldDescriptor.get(label);
        if (result == null) {
            result = new TestFieldDescriptor(type, label, this.nextId);
            labelToFieldDescriptor.put(label, result);
            this.nextId++;
        } else if (result.getFieldType() != type) {
            throw new FieldRegistrationException("field with label(" + label + ") of type " +
                                                 type + " trying to replace existing field of type " +
                                                 result.getFieldType());
        }
        return result;
    }

    public void validateFieldLabel(String label) throws InvalidFieldLabelException {
        // we allow everything.
    }

    /// Map label (string) to FieldDescriptor: it's a TreeMap so that
    /// it prints in alphabetic order with toString().
    private TreeMap labelToFieldDescriptor = new TreeMap();

    /// Keep track of the next identifier to assign to a
    /// FieldDescriptor.
    private int nextId;

    public String toString() {
        StringBuffer result = new StringBuffer(500);
        result.append("TestFieldDescriptorFactory[");
        String sep = "";
        for (Iterator it = labelToFieldDescriptor.keySet().iterator(); it.hasNext(); ) {
            result.append(sep).append(it.next());
            sep = " ";
        }
        result.append("]");
        return result.toString();
    }
}
