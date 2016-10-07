package com.xacct.CRANE.collector;

// $Id: SessionConfig.java,v 1.4 2005/01/30 15:13:39 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.util.ArrayList;
import java.util.Iterator;
import java.util.ListIterator;

/** The configuration information for a session within a CRANE server (collector).
 *
 *  @version $Id: SessionConfig.java,v 1.4 2005/01/30 15:13:39 ilyad Exp $
 */
public final class SessionConfig implements ConfigObject, Comparable, java.io.Serializable
{
    /** $Id: SessionConfig.java,v 1.4 2005/01/30 15:13:39 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: SessionConfig.java,v 1.4 2005/01/30 15:13:39 ilyad Exp $"; }

    /** Session id (unique for a transmitter (client)).
     *  &lt;ID&gt;...&lt;/ID&gt;
     */
    private int id;
    public final int getId() { return this.id; }
    public final void setId(int value) { this.id = value; }

    /** The name for this session.
     *  &lt;Name&gt;...&lt;/Name&gt;
     */
    private String name;
    public final String getName() { return this.name; }
    public final void setName(String value) { this.name = value; }

    /** A description of the session.
     *  &lt;Description&gt;...&lt;/Description&gt;
     */
    private String description;
    public final String getDescription() { return this.description; }
    public final void setDescription(String value) { this.description = value; }

    /** The templates.
     */
    private ArrayList templateConfigList;
    public final ArrayList getTemplateConfigList() {
        return (templateConfigList == null) ? emptyArrayList : templateConfigList;
    }
    public final ListIterator getTemplateConfigIterator() {
        return (templateConfigList == null) ? emptyListIterator
            : templateConfigList.listIterator();
    }
    public final int getTemplateConfigListLength() {
        return (templateConfigList == null) ? 0 : templateConfigList.size();
    }
    public final TemplateConfig getTemplateConfigAt(int i) {
        return (TemplateConfig)templateConfigList.get(i);
    }

    /** Get a template from the list of tamplates, using the template ID,
     *  throwing an exception if it's not there.
     *  @param id The template ID for matching.
     *  @return The template.
     *  @throws ConfigException If not found.
     */
    public final TemplateConfig getTemplateConfigById(int id) throws ConfigException {
        return TemplateConfig.getById(this, templateConfigList, id);
    }

    /** Look up a template from the list of templates, using the template ID,
     *  returning null if it's not there.
     * @param id The template ID for matching.
     * @return The template, or null if not found.
     */
    public final TemplateConfig lookupTemplateConfigById(int id) {
        return TemplateConfig.lookupById(this, templateConfigList, id);
    }

    /** The outstanding templates.
     *  These are the templates which appear at runtime but not in configuration.
     *  They need to be kept so their data could be ACKed
     */
    private ArrayList outstandingTemplateConfigList;
    public void setOutstandingTemplateConfigList(ArrayList value) { this.outstandingTemplateConfigList = value; }
    public final ArrayList getOutstandingTemplateConfigList() {
        return (outstandingTemplateConfigList == null) ? emptyArrayList : outstandingTemplateConfigList;
    }
    public final ListIterator getOutstandingTemplateConfigIterator() {
        return (outstandingTemplateConfigList == null) ? emptyListIterator
            : outstandingTemplateConfigList.listIterator();
    }
    public final int getOutstandingTemplateConfigListLength() {
        return (outstandingTemplateConfigList == null) ? 0 : outstandingTemplateConfigList.size();
    }
    public final TemplateConfig getOutstandingTemplateConfigAt(int i) { return (TemplateConfig)outstandingTemplateConfigList.get(i); }
    public final TemplateConfig getOutstandingTemplateConfigById(int id) throws ConfigException {
        return TemplateConfig.getOutstandingById(this, outstandingTemplateConfigList, id);
    }
    public final TemplateConfig lookupOutstandingTemplateConfigById(int id) {
        return TemplateConfig.lookupOutstandingById(this, outstandingTemplateConfigList, id);
    }

    /** Look up a session in a list of sessions, using the session id,
     *  throwing an exception if it's not there.
     * @param container The containing object (for error messages).
     * @param sessionConfigList The list of sessions.
     * @param matchId The (unique) session id.
     * @return The session whose id matches.
     * @throws ConfigException If matchId not found.
     * @see SessionConfig#lookupById
     */
    public final static SessionConfig getById(Object container,
                                              ArrayList sessionConfigList,
                                              int matchId)
        throws ConfigException {
        SessionConfig result = lookupById(container, sessionConfigList, matchId);
        if (result == null) {
            throw new ConfigException(container, "SessionConfig.getById",
                                      "Unknown session: id=" + matchId + " in " + container +
                                      " ... looked in " + CollectorUtil.toStringLong(sessionConfigList));
        }
        return result;
    }

    /** Look up a session in a list of sessions, using the session id,
     *  returning null if it's not there.
     * @param container The containing object (for error messages).
     * @param sessionConfigList The list of sessions.
     * @param matchId The (unique) session id.
     * @return The session whose id matches, null if not found.
     * @see SessionConfig#getById
     */
    public final static SessionConfig lookupById(Object container,
                                                 ArrayList sessionConfigList,
                                                 int matchId) {
        SessionConfig result = null;
        for (Iterator it = sessionConfigList.iterator();
             result == null && it.hasNext();
            ) {
            SessionConfig elem = (SessionConfig)it.next();
            if (elem.getId() == matchId) {
                result = elem;
            }
        }
        return result;
    }

    /** Create the object and ensure all fields are non-null.
     * @param id Session id.
     * @param name Session name.
     * @param description session description.
     * @param templateConfigList List of templates with field ids.
     */
    public SessionConfig(int id,
                         String name,
                         String description,
                         ArrayList templateConfigList) {
        set(id, name, description, templateConfigList);
    }

    /** Helper method for filling in the object (used by constructors).
     * @param id Session id.
     * @param name Session name.
     * @param description session description.
     * @param templateConfigList List of templates with field ids.
     */
    private final void set(int id,
                            String name,
                            String description,
                            ArrayList templateConfigList) {
        this.id          = id;
        this.name        = CollectorUtil.nonNull(name);
        this.description = CollectorUtil.nonNull(description);
        this.templateConfigList  = CollectorUtil.nonNull(templateConfigList);
    }

    public final void setTemplates(ArrayList templateConfigList) { this.templateConfigList = templateConfigList; }

    /** Automap all the fields in the templates in this session.
     *  @see ConnectionConfig#automapAndEnableAllFields
     *
     *  @param collectorConfig The collector's configuration, in which
     *       this session configuration resides.
     *  @param fieldFactory The factory for creating fields.
     *  @throws ConfigException If the field information is invalid.
     */
   public final void automapAndEnableAllFields(CollectorConfig collectorConfig,
                                               FieldDescriptorFactory fieldFactory)
       throws ConfigException {

       for (Iterator it = getTemplateConfigList().iterator(); it.hasNext(); ) {
           TemplateConfig tf = (TemplateConfig)it.next();
           tf.automapAndEnableAllFields(collectorConfig, fieldFactory);
       }
   }

    public final boolean equals(Object other) {
        try {
            return equals((SessionConfig)other);
        } catch (ClassCastException e) {
            return false;
        }
    }

    public final boolean equals(SessionConfig other) {
        return other != null && (0 == this.compareTo(other));
    }

    public final int hashCode() { return id ^ getTemplateConfigListLength(); }

    public final int compareTo(Object other) { return compareTo((SessionConfig)other); }

    /** Compare two sessions. We do <em>not</em> use the following
     *  fields, which have no "semantic" meaning:
     *  collectorConfig
     *  @param other The other session to compare with
     *  @return same as Comparable.compareTo
     *  @see Comparable#compareTo
     */
    public final int compareTo(SessionConfig other) {
        if (this == other) { return 0; }
        int result = 0;
        if (result == 0) { result = this.id - other.id; }
        if (result == 0) { result = this.name.compareTo(other.name); }
        if (result == 0) { result = this.description.compareTo(other.description); }
        if (result == 0) {
            for (int i = 0; result == 0 && i < this.getTemplateConfigListLength() && i < other.getTemplateConfigListLength(); i++) {
                result = getTemplateConfigAt(i).compareTo(other.getTemplateConfigAt(i));
            }
        }
        if (result == 0) { result = this.getTemplateConfigListLength() - other.getTemplateConfigListLength(); }
        return result;
    }

    public final String toString() {
        return "Session[id=" + id +
            " template#=" + getTemplateConfigListLength() +
            " name(" + name + ")" +
            " descr(" + description + ")" +
            "]";
    }

    public final String toStringLong() {
        return toStringLong("");
    }

    public final String toStringLong(String indent) {
        return this.toString() +
            ((getTemplateConfigListLength() > 0) ? "\n": "") +
            CollectorUtil.toStringLong(templateConfigList, indent);
    }
}
