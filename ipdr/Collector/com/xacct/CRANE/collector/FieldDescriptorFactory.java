package com.xacct.CRANE.collector;

//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/** Factory interface for creation of FieldDescriptor instances.
 *  @see FieldDescriptor
 */
public interface FieldDescriptorFactory
{
    /** Returns a FieldDescriptor with the specified field type and
     *  field label.  If such a field has already been registered in
     *  the system, the existing FieldDescriptor will be returned. If
     *  there is currently no such field, the method will attempt to
     *  create and register a new one.  Fields in the system are
     *  presumed to be registered in a central database, which is
     *  encapsulated within this factory.
     *
     * @param type the requested field type
     * @param label the requested field label
     *
     * @return a FieldDescriptor with the specified field type and field label
     *
     * @throws  InvalidFieldLabelException if the field has an invalid label.
     * @throws  FieldRegistrationException if an error occured while trying
     *          to register the field with the system. Commonly caused because
     *          a different field has already been registered with the same label.
     */
    public FieldDescriptor createFieldDescriptor(FieldType type, String label)
        throws InvalidFieldLabelException, FieldRegistrationException;

    /** Validates a field label, checking if it is of an allowed length and character set.
     *
     *  @param label the field label
     *
     *  @throws InvalidFieldLabelException if the specified label is not a valid field label.
     */
    public void validateFieldLabel(String label)
        throws InvalidFieldLabelException;
}
