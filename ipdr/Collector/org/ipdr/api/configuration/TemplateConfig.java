package org.ipdr.api.configuration;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Iterator;

import org.ipdr.protocol.ProtocolFieldTypes;

/**
 * Represents a template in the system
 */
public class TemplateConfig
{

    /**
     * Holds fields of the template
     */
    protected ArrayList fields = new ArrayList();

    /**
     * Holds map of field ids to fields 
     */
    protected Hashtable fieldsMap = new Hashtable();
    /**
     * Holds template id
     */
    private short id;

    /**
     * Holds template schema name
     */
    private String schemaName;

    /**
     * Holds template description
     */
    protected String templateDescription;

    /**
     * Holds template global ref id
     */
    protected int templateGlobalRef;

    /**
     * Holds template name
     */
    private String typeName;

    public TemplateConfig()
    {
    }

    public TemplateConfig(int templateId, ArrayList fields, String description)
    {
        setId((short) templateId);
        setFields(fields);
        this.templateDescription = description;
    }

    /**
     * Compares two templates. The templates are considered equal, if they have same id and fields
     * 
     * @see java.lang.Object#equals(java.lang.Object)
     */
    public boolean equals(Object obj)
    {
        if (obj.getClass() != TemplateConfig.class) return false;
        TemplateConfig template = (TemplateConfig) obj;

        /*
         * templates are considered equal if: 1. they have the same ID 2. they
         * contain exactly the same fields
         */
        if (this.id != template.getTemplateId()) return false;
        // compare fields
        ArrayList templateFields = template.getFieldList();
        for (int i = 0; i < templateFields.size(); i++)
        {
            FieldConfigI peerField = (FieldConfigI) templateFields.get(i);
            Integer fieldKey = new Integer(peerField.getId());
            FieldConfigI myField = (FieldConfigI) fieldsMap.get(fieldKey);
            if (myField == null) return false;
            if (!myField.equals(peerField)) return false;
        }
        return true;
    }

    /**
     * Returns field by its index value 
     * @param position index of field
     * @return <code>FieldConfigI</code> of field at a given position
     */
    public FieldConfigI getFieldConfigAt(int position)
    {
        return (FieldConfigI) fields.get(position);
    }

    /**
     * Returns list of fields in the templates
     * @return <code>ArrayList</code> of fields
     */
    public ArrayList getFieldList()
    {
        return this.fields;
    }

    /**
     * Returns number of fields in the template
     * @return number of fields
     */
    public int getFieldListLength()
    {
        return fields.size();
    }

    /**
     * Returns the id
     * @return id
     */
    public short getId()
    {
        return this.id;
    }

    /**
     * Returns schema name
     * @return schema name
     */
    public String getSchemaName()
    {
        return this.schemaName;
    }

    /**
     * Returns template description 
     * @return template description
     */
    public String getTemplateDescription()
    {
        return this.templateDescription == null ? "" : this.templateDescription;
    }

    /**
     * Returns template global ref id
     * @return ref id
     */
    public int getTemplateGlobalRef()
    {
        return this.templateGlobalRef;
    }

    /**
     * Synonim for getId()
     * @return id
     */
    public int getTemplateId()
    {
        return this.id;
    }

    /**
     * Returns type name
     * @return type name
     */
    public String getTypeName()
    {
        return this.typeName;
    }

    /**
     * Sets the fields.
     * 
     * @param fields
     *            The fields to set
     */
    public void setFields(ArrayList fields)
    {
        this.fields = fields;
        for (int i = 0; i < fields.size(); i++)
        {
            FieldConfigI field = (FieldConfigI) fields.get(i);
            this.fieldsMap.put(new Integer(field.getId()), field);
        }
    }
    
    /**
     * Adds a field to the template
     * @param field The field to add
     */
    public void addField(FieldConfigI field)
    {
        this.fieldsMap.put(new Integer(field.getId()), field);
        this.fields.add(field);
	}
    
    public FieldConfigI getFieldById(int id)
    {
        Integer iid = new Integer(id);
        return (FieldConfigI)fieldsMap.get(iid);
    }

    /**
     * Sets the id.
     * 
     * @param id
     *            The id to set
     */
    public void setId(short id)
    {
        this.id = id;
    }

    /**
     * Sets the schemaName.
     * 
     * @param schemaName
     *            The schemaName to set
     */
    public void setSchemaName(String schemaName)
    {
        this.schemaName = schemaName;
    }

    /**
     * Sets the typeName.
     * 
     * @param typeName
     *            The typeName to set
     */
    public void setTypeName(String typeName)
    {
        this.typeName = typeName;
    }

    /**
     * Validates fields of the template. Raises an exception if there is an invalid field
     * @throws ConfigException
     */
    public void validateSelf() throws ConfigException
    {
        for (int i = 0; i < fields.size(); i++)
        {
            if (!ProtocolFieldTypes.validateField((FieldConfigI) fields.get(i)))
                    throw new ConfigException(
                            "Invalid field type in the template:" + ((FieldConfigI) fields.get(i)).getName() );
        }
    }
    /**
     * @param templateDescription The templateDescription to set.
     */
    public void setTemplateDescription(String templateDescription)
    {
        this.templateDescription = templateDescription;
    }
    /**
     * @param templateGlobalRef The templateGlobalRef to set.
     */
    public void setTemplateGlobalRef(int templateGlobalRef)
    {
        this.templateGlobalRef = templateGlobalRef;
    }

    public Object clone() throws CloneNotSupportedException
    {
        TemplateConfig cloneObj = new TemplateConfig();
        cloneObj.id = this.id;
        cloneObj.schemaName = this.schemaName;
        cloneObj.templateDescription = this.templateDescription;
        cloneObj.templateGlobalRef = this.templateGlobalRef;
        ArrayList cloneFields = new ArrayList(fields.size());
        Iterator it = fields.iterator();
        while(it.hasNext())
        {
            FieldConfig original = (FieldConfig) ((FieldConfig)it.next()).clone();
            cloneFields.add(original);
        }
        
        cloneObj.setFields(cloneFields);
        return cloneObj;
    }
}