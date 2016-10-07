package com.xacct.CRANE.collector;

//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/** Used when defining record formats and when manipulating records.
 *  FieldDescriptors are used when defining an input record format for
 *  an<code>InputPortDescriptor</code> object, when defining an output
 *  record formatfor an <code>OutputPortDescriptor</code>, and when
 *  defining both input andoutput record formats for an
 *  <code>EnhancementFunctionDescriptor</code>.
 *
 *  Every FieldDescriptor object has a type, chosen from several
 *  predefined field types. Every FieldDescriptor object also has a
 *  label, which must be unique.
 *
 * @see FieldDescriptorFactory
 */
public interface FieldDescriptor
{
    /** Returns the field data type of this FieldDescriptor object.
     *
     * @return the field data type of this FieldDescriptor object
     */
    public FieldType getFieldType();

    /** Returns the Fully Qualified Name of this FieldDescriptor
     *  object.  The field's FQN consists of the fields's namespace, a
     *  delimiter, and the field's label - all concatenated in this
     *  order.
     *
     * @return the Fully Qualified Name of this FieldDescriptor object
     */
    public String getFQN();

    /** Returns the local label of this FieldDescriptor object.
     *
     * @return the local label of this FieldDescriptor object
     */
    public String getLabel();

    /** Returns the namespace identifier of this FieldDescriptor object.
     *
     * @return the namespace identifier of this FieldDescriptor object
     */
    public String getNamespace();

    /** Returns the internal assigned identifier of this FieldDescriptor object.
     *
     * @return the internal assigned identifier of this FieldDescriptor object
     *
     * @throws InvalidFieldDescriptorException if the FieldDescriptor is invalid
     *         (has been released).
     */
    public Integer getID() throws InvalidFieldDescriptorException;

    /** Returns the primitive internal assigned identifier of this FieldDescriptor
     * object.
     *
     * @return the primitive internal assigned identifier of this FieldDescriptor
     * object
     *
     * @throws InvalidFieldDescriptorException if the FieldDescriptor is invalid
     *         (has been released).
     */
    public int getPrimitiveID() throws InvalidFieldDescriptorException;
}
