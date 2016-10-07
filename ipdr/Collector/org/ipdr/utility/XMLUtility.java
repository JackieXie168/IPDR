package org.ipdr.utility;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import org.ipdr.api.configuration.ConfigException;
import org.jdom.Attribute;
import org.jdom.DataConversionException;
import org.jdom.Element;

/**
 * The class encapsulates various useful XML utilities
 */
public class XMLUtility
{
    /**
     * Returns an instance of <code>Attribute</code> of attribute under given element. If none exist, raises an exception
     * @param element The element to retrieve the attribute
     * @param attributeName The name of the attribute to retrieve
     * @return The instance of <code>Attribute</code>
     * @throws ConfigException
     */
    protected static Attribute getMandatoryAttribute(Element element,
            String attributeName) throws ConfigException
    {
        Attribute result = element.getAttribute(attributeName);
        if (result == null)
            throw new ConfigException("Mandatory attribute '" + attributeName + "' does not exist for element " + element.getQualifiedName());
        return result;
    }

    /**
     * Returns integer value of attribute with given name of the given element. Raises exception if such attribute does not exist
     * @param element Element to get the value from
     * @param attributeName Name of the attribute
     * @return value of the attribute
     * @throws ConfigException if the attribute does not exist
     * @throws DataConversionException if the value is not a valid integer
     */
    public static int getMandatoryIntAttribute(Element element,
            String attributeName) throws ConfigException, DataConversionException
    {
        return getMandatoryAttribute(element, attributeName).getIntValue();
    }
    
    /**
     * Returns integer value of attribute with given name of the given element. Returns default value if such attribute does not exist
     * @param element Element to get the value from
     * @param attributeName Name of the attribute
     * @param defaultValue Value to use if there is no such attribute
     * @return value of the attribute or defaultValue if the attribute does not exist
     * @throws DataConversionException
     */
    public static int getOptionalIntAttribute(Element element, String attributeName, int defaultValue) throws DataConversionException
    {
        Attribute attribute = element.getAttribute(attributeName);
        if (attribute != null)
            return attribute.getIntValue();
        return defaultValue;
    }

    /**
     * Returns boolean value of attribute with given name of the given element. Raises exception if such attribute does not exist
     * @param element Element to get the value from
     * @param attributeName Name of the attribute
     * @return value of the attribute
     * @throws ConfigException if the attribute does not exist
     * @throws DataConversionException if the value is not a valid boolean
     */
    public static boolean getMandatoryBooleanAttribute(Element element,
            String attributeName) throws ConfigException, DataConversionException
    {
        return getMandatoryAttribute(element, attributeName).getBooleanValue();
    }
    /**
     * Returns boolean value of attribute with given name of the given element. Returns default value if such attribute does not exist
     * @param element Element to get the value from
     * @param attributeName Name of the attribute
     * @param defaultValue Value to use if there is no such attribute
     * @return value of the attribute or defaultValue if the attribute does not exist
     * @throws DataConversionException
     */
    public static boolean getOptionalBooleanAttribute(Element element, String attributeName, boolean defaultValue) throws DataConversionException
    {
        Attribute attribute = element.getAttribute(attributeName);
        if (attribute != null)
            return attribute.getBooleanValue();
        return defaultValue;
    }
    /**
     * Returns string value of attribute with given name of the given element. Raises exception if such attribute does not exist
     * @param element Element to get the value from
     * @param attributeName Name of the attribute
     * @return value of the attribute
     * @throws ConfigException if the attribute does not exist
     */
    public static String getMandatoryStringAttribute(Element element,
            String attributeName) throws ConfigException
    {
        return getMandatoryAttribute(element, attributeName).getValue();
    }
}