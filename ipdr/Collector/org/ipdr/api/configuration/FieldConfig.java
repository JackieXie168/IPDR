package org.ipdr.api.configuration;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.


/**
 * This class provides a basic implementation of <code>FieldConfigI</code>
 */
public class FieldConfig implements FieldConfigI, Cloneable
{

    /**
     * Holds field id
     */
    private int id;

    /**
     * Holds field output flag
     */
    private boolean isEnabledForOutput;

    /**
     * Holds field name
     */
    private String name;

    /**
     * Holds field type id
     */
    private int typeId;

    public FieldConfig()
    {
    }

    /**
     * Creates a field with given id, type id, name and state
     * @param id field id
     * @param typeId field type id
     * @param name field name
     * @param enabled field <code>enabled</code> flag
     */
    public FieldConfig(int id, int typeId, String name, boolean enabled)
    {
        this.id = id;
        this.typeId = typeId;
        this.name = name;
        this.isEnabledForOutput = enabled;
    }

    /**
     * A field is considered equal to another field if its id, name and type id are identical
     * @see java.lang.Object#equals(java.lang.Object)
     */
    public boolean equals(Object obj)
    {
        try
        {
            FieldConfigI peer = (FieldConfigI) obj;

            return (peer.getId() == this.getId()
                    && peer.getTypeId() == this.getTypeId() && peer.getName()
                    .equals(this.getName()));
        }
        catch (ClassCastException e)
        {
            return false;
        }
    }

    /**
     * @see org.ipdr.api.configuration.FieldConfigI#getId()
     */
    public int getId()
    {
        return id;
    }

    /**
     * @see org.ipdr.api.configuration.FieldConfigI#getName()
     */
    public String getName()
    {
        return name;
    }

    /**
     * @see org.ipdr.api.configuration.FieldConfigI#getTypeId()
     */
    public int getTypeId()
    {
        return typeId;
    }

    /**
     * @see org.ipdr.api.configuration.FieldConfigI#isEnabledForOutput()
     */
    public boolean isEnabledForOutput()
    {
        return this.isEnabledForOutput;
    }

    /**
     * @see org.ipdr.api.configuration.FieldConfigI#setEnabledForOutput(boolean)
     */
    public void setEnabledForOutput(boolean enabled)
    {
        this.isEnabledForOutput = enabled;
    }

    /**
     * @param id
     *            The id to set.
     */
    public void setId(int id)
    {
        this.id = id;
    }

    /**
     * @param name
     *            The name to set.
     */
    public void setName(String name)
    {
        this.name = name;
    }

    /**
     * @param typeId
     *            The typeId to set.
     */
    public void setTypeId(int typeId)
    {
        this.typeId = typeId;
    }

    public Object clone() throws CloneNotSupportedException
    {
        return super.clone();
    }
}