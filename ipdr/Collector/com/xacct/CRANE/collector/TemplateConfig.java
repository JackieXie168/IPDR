package com.xacct.CRANE.collector;

// $Id: TemplateConfig.java,v 1.4 2005/01/30 15:13:41 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.util.ArrayList;
import java.util.Iterator;
import java.util.ListIterator;

/** The template information for a CRANE server (collector).
 *
 *  @version $Id: TemplateConfig.java,v 1.4 2005/01/30 15:13:41 ilyad Exp $
 */
public final class TemplateConfig implements ConfigObject, Comparable, java.io.Serializable
{
    /** $Id: TemplateConfig.java,v 1.4 2005/01/30 15:13:41 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: TemplateConfig.java,v 1.4 2005/01/30 15:13:41 ilyad Exp $"; }

    /** Template id (unique within a session).
     *  &lt;ID&gt;...&lt;/ID&gt;
     */
    private int id;
    public final int getId() { return id; }

    /** The keys.
     */
    private ArrayList fieldList;
    public final ArrayList getFieldList() {
        return (fieldList == null) ? emptyArrayList : fieldList;
    }
    public final ListIterator getFieldIterator() {
        return (fieldList == null) ? emptyListIterator
            : fieldList.listIterator();
    }
    public final int getFieldListLength() {
        return (fieldList == null) ? 0 : fieldList.size();
    }
    public final FieldConfig getFieldAt(int i) {
        return ((FieldConfig)fieldList.get(i));
    }

    /** The template's description.
     *  &lt;Description&gt;...&lt;/Description&gt;
     */
    private String description;
    public final String getDescription() { return description; }

    /** Derived from 'T' flag in Template Block. See protocol spec:
     *  4.19 STATUS REQ, 4.20 STATUS_RSP, 'T' bit in 4.6 TMPL_DATA.
     */
    boolean forStatusResponse;
    public final boolean isForStatusResponse() { return forStatusResponse; }

    public final String getReferenceString() {
        return buildReferenceString();
    }

    private final String buildReferenceString() {
        String result =
            "id= " + id +
            " desc= "+ description +
            " keyString= " + getFieldsString();
        return result;
    }

    // XXX Can be replaced by Util.toString(getFieldList()), I think [pgl]
    private final String getFieldsString() {
        String result = "";
        String sep = "";
        for (ListIterator it = getFieldList().listIterator(); it.hasNext(); ) {
            FieldConfig fc = (FieldConfig)it.next();
            result += sep + fc.toString();
            sep = ",";
        }
        return result;
    }


    /** Look up a template in a list of templates, using the template id,
     *  throwing an exception if it's not there.
     * @param container the containing object (for error messages)
     * @param templateConfigList the list of templates
     * @param matchId the (unique) template id
     * @return the template whose id matches
     * @throws ConfigException if matchId not found
     * @see TemplateConfig#lookupById
     */
    public final static TemplateConfig getById(Object container,
                                               ArrayList templateConfigList,
                                               int matchId)
        throws ConfigException {
        TemplateConfig result = lookupById(container, templateConfigList, matchId);
        if (result == null) {
            throw new ConfigException(container, "TemplateConfig.getById",
                                      "Unknown template: id=" + matchId + " in " + container +
                                      " ... looked in: " + CollectorUtil.toStringLong(templateConfigList));
        }
        return result;
    }

    /** Look up a template in a list of outstanding templates, using the template id,
     *  throwing an exception if it's not there.
     * @param container the containing object (for error messages)
     * @param templateConfigList the list of templates
     * @param matchId the (unique) template id
     * @return the template whose id matches
     * @throws ConfigException if matchId not found
     * @see TemplateConfig#lookupById
     */
    public final static TemplateConfig getOutstandingById(Object container,
                                               ArrayList outstandingTemplateConfigList,
                                               int matchId)
        throws ConfigException {
        TemplateConfig result = lookupById(container, outstandingTemplateConfigList, matchId);
        if (result == null) {
            throw new ConfigException(container, "TemplateConfig.getById",
                                      "Unknown template: id=" + matchId + " in " + container +
                                      " ... looked in: " + CollectorUtil.toStringLong(outstandingTemplateConfigList));
        }
        return result;
    }

    /** Look up a template in a list of templates, using the template id,
     *  returning null if it's not there.
     * @param container the containing object (for error messages)
     * @param templateConfigList the list of templates
     * @param matchId the (unique) template id
     * @return the template whose id matches, null if not found.
     * @see TemplateConfig#getById
     */
    public final static TemplateConfig lookupById(Object container,
                                                  ArrayList templateConfigList,
                                                  int matchId) {
        TemplateConfig result = null;
        for (Iterator it = templateConfigList.iterator(); result == null && it.hasNext(); ) {
            TemplateConfig elem = (TemplateConfig)it.next();
            if (elem == null) {
                return null;
            }
            else if (elem.getId() == matchId) {
                result = elem;
            }
        }
        return result;
    }

    /** Look up a template in a list of outstanding templates, using the template id,
     *  returning null if it's not there.
     * @param container the containing object (for error messages)
     * @param outstandingTemplateConfigList the list of templates
     * @param matchId the (unique) template id
     * @return the template whose id matches, null if not found.
     * @see TemplateConfig#getById
     */
    public final static TemplateConfig lookupOutstandingById(Object container,
                                                  ArrayList outstandingTemplateConfigList,
                                                  int matchId) {
        if (outstandingTemplateConfigList == null) {
            return null;
        }
        TemplateConfig result = null;
        for (Iterator it = outstandingTemplateConfigList.iterator(); result == null && it.hasNext(); ) {
            TemplateConfig elem = (TemplateConfig)it.next();
            if (elem.getId() == matchId) {
                result = elem;
            }
        }
        return result;
    }

    /// Create the object and ensure all fields are non-null

    public TemplateConfig(int id,
                          ArrayList fieldList, // ordered
                          String description,
                          boolean forStatusResponse)
    throws ConfigException {
        set(id, fieldList, description, forStatusResponse);
    }

    public final void set(int id,
                          ArrayList fieldList, // ordered
                          String description,
                          boolean forStatusResponse)
    throws ConfigException {
        fieldList = CollectorUtil.nonNull(fieldList);
        this.id = id;
        this.description = CollectorUtil.nonNull(description);
        this.forStatusResponse = forStatusResponse;
        this.fieldList = new ArrayList(fieldList.size());
        FieldConfig fc = null;
        synchronized(fieldList) {
            for (int i = 0; i < fieldList.size(); i++) {
                Object e = fieldList.get(i);
                if (e instanceof FieldConfig) {
                    fc = (FieldConfig)e;
                }
                if (fc.getFieldDesc() != null) {
                    this.fieldList.add(
                        new FieldConfig(fc.getId(), fc.getTypeId(), fc.getLabel(),
                                        fc.getHelp(), fc.isEnabledForOutput(), fc.getFieldDesc()));
                } else {
                    this.fieldList.add(
                        new FieldConfig(fc.getId(), fc.getTypeId(), fc.getLabel(),
                                        fc.getHelp(), fc.isEnabledForOutput()));
                }
            }
        }
    }

    /** Check whether two templates are "similar" in the sense that
     *  all the important fields match. This is used to check that the
     *  template from a transmitter matches a template in the
     *  configuration.
     */
    public final boolean similarTo(TemplateConfig other) {
        boolean result = this.id == other.id &&
            this.getFieldListLength() == other.getFieldListLength();
        for (int i = 0; i < getFieldListLength() && result; i++) {
            result = this.getFieldAt(i) == other.getFieldAt(i);
        }
        return result;
    }

    /** Search through a list of templates for one that is similar to this one.
     *  @param templateConfigList The list to search
     *  @return A match, if found; otherwise null.
     *  @see #similarTo
     */
    public final TemplateConfig findSimilarTo(ArrayList templateConfigList) {
        for (Iterator it = templateConfigList.iterator(); it.hasNext(); ) {
            TemplateConfig elem = (TemplateConfig)it.next();
            if (this.similarTo(elem)) {
                return elem;
            }
        }
        return null;
    }


    /** Automap all the fields in the template.
     *  @see SessionConfig#automapAndEnableAllFields
     *
     *  @param collectorConfig The collector's configuration, in which
     *       this template configuration resides.
     *  @param fieldFactory The factory for creating fields.
     *  @throws ConfigException If the field information is invalid.
     */
   public final void automapAndEnableAllFields(CollectorConfig collectorConfig,
                                               FieldDescriptorFactory fieldFactory)
    throws ConfigException {

       for (Iterator it = getFieldList().iterator(); it.hasNext(); ) {
           FieldConfig f = (FieldConfig)it.next();
           f.automapAndEnable(collectorConfig, fieldFactory);
       }
   }

    // stuff for Comparable, Cloneable

    public final boolean equals(Object other) {
        try {
            return equals((TemplateConfig)other);
        } catch (ClassCastException e) {
            return false;
        }
    }

    public final boolean equals(TemplateConfig other) {
        return other != null && (0 == this.compareTo(other));
    }

    public final int hashCode() { return id; }

    public final int compareTo(Object other) { return compareTo((TemplateConfig)other); }

    public final int compareTo(TemplateConfig other) {
        if (this == other) { return 0; }
        int result = 0;
        if (result == 0) { result = this.id - other.id; }
        if (result == 0) { result = this.description.compareTo(other.description); }
        return result;
    }

    public final String toString() {
        return "Template[" +
            "id=" + id +
            " key#=" + getFieldListLength() +
            " descr=\"" + description + "\"" +
            "]";
    }

    /// A possibly longer description of the template
    public final String toStringLong() {
        return toStringLong("");
    }

    /** A possibly longer description of the template
     *  @param indent A string to put at the beginning of each line
     *  @return A (possibly multi-line) description
     */
    public final String toStringLong(String indent) {
        StringBuffer result = new StringBuffer(1000);
        result.append(this.toString()).append("{");
        String sep = "";
        for (int i = 0; i < this.getFieldListLength(); i++) {
            result.append(sep).append(this.getFieldAt(i));
            sep = " ";
        }
        result.append("}");
        return result.toString();
    }

}
