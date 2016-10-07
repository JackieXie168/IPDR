package org.ipdr.api.configuration;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/**
 * This interface provides configuration of a field in a template
 *  
 */
public interface FieldConfigI
{
	/**
	 * Returns field id
	 * @return field id
	 */
	public int getId();
	/**
	 * Returns field type id
	 * @return field type id
	 */
	public int getTypeId();
	/**
	 * Returns field name
	 * @return field name
	 */
	public String getName();
	/**
	 * Indicates if the field should be outputted
	 * @return true if the field is enabled for output
	 */
	public boolean isEnabledForOutput();
	/**
	 * Sets the field for output
	 * @param enabled value to set
	 */
	public void setEnabledForOutput( boolean enabled );
}
