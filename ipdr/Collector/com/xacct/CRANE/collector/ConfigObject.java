package com.xacct.CRANE.collector;

// $Id: ConfigObject.java,v 1.4 2005/01/30 15:13:40 ilyad Exp $
// Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.util.ArrayList;
import java.util.ListIterator;

/** Encapsulation of the various methods that all xxxConfig objects
 *  must implement.
 *
 *  For accessing the individual items of a list within a
 *  configuration object, we use the following conventions (e.g., for
 *  accessing the TemplateConfig list within a CollectorConfig, "Aaa"
 *  would be "Template"):
 *  <dl>
 *  <dt>getAaaConfigList()<dd>gets the list as an ArrayList.
 *  <dt>getAaaConfigIterator()<dd>gets the list as a ListIterator.
 *  <dt>getAaaConfigListLength()<dd>gets the length of the list.
 *  <dt>getAaaConfigAt(int i)<dd>gets the <i>i</i>th element
 *  <dt>getAaaConfigById(...)<dd>gets the element identified by the
 *          "id" (usually, but not always, an int).
 *  </dl>
 *  <p>
 *  These methods are safe in the sense that they will work if the
 *  contained list is <code>null</code>.
 *
 *  @version $Id: ConfigObject.java,v 1.4 2005/01/30 15:13:40 ilyad Exp $
 */
public interface ConfigObject extends Comparable
{
    /** Returns a string representation of the object. In general, the
     *  toString method returns a string that "textually represents"
     *  this object. The result should be a concise but informative
     *  representation that is easy for a person to read. It is
     *  recommended that all subclasses override this method.
     *  @return a string representation of the object.
     *  {@inheritDoc}
     */
    public String toString();

    /** Returns a longer string representation of the object.
     *  @return a "full" string representation of the object.
     *  @see #toString
     */
    public String toStringLong();
    /** Returns a longer string representation of the object,
     *  plus a certain indent. This is used for multi-line sub-objects,
     *  where some level of indentation is desired.
     *  @param indent a string to prepend to the result, usually
     *                a string of blanks (indent).
     *  @return a "full" string representation of the object.
     *  @see #toString()
     */
    public String toStringLong(String indent);

    /** Convenience value for a zero-length array when a list is null */
    public final ArrayList emptyArrayList = new ArrayList(0);
    /** Convenience value for a zero-lenght iterator when a list is null */
    public final ListIterator emptyListIterator = emptyArrayList.listIterator();

    /*  ----
     *  Here is the "canonical form" from Venners' book:
     *
     *  public boolean equals(Object other) {
     *    if (other == null) { return false; }
     *    try {
     *       return equals(ConfigObject)other);
     *    } catch ClassCastException e) { return false; }

     *  }
     *a negative integer, zero, or a positive integer as this object is less than, equal to, or greater than the specified object.
     *  And here is the "canonical" form from Larman&Guthrie ... but I
     *  think that this is wrong because getClass() doesn't handle
     *  instances ... Larman&Guthrie claim that "other instanceof
     *  ConfigObject" is wrong because that could lead to a.equals(b)
     *  but !b.equals(a). Anyway:
     *
     *  public boolean equals(Object other) {
     *     if (this == other) {
     *         return true;
     *     } else if (other == null || getClass() != other.getClass()) {
     *         return true;
     *     } else {
     *         return equals((ConfigObject)other);
     *     }
     *  }
     *
     *  In addition, we define a version that works on our own obja negative integer, zero, or a positive integer as this object is less than, equal to, or greater than the specified object.ect:
     *  public boolean equals(ConfigObject other) {
     *     return other != null && (0 == compareTo(other));
     *  }
     */

    /** Indicates whether some other object is "equal to" this one.
     *  @param other the reference object with which to compare.
     *  @return true if this object is the same as the argument; false otherwise.
     *  @see Object#equals
     *  @see #compareTo
     */
    public boolean equals(Object other);

    /** Compare two objects.  See the contract for
     *  {@link Comparable#compareTo}: it should return NullPointerException if
     *  the other object is null (equals() returns false).
     *  @param other the reference object with which to compare.
     *  @return a negative integer, zero, or a positive integer as
     *  this object is less than, equal to, or greater than the
     *  specified object.
     *  @see Comparable#compareTo
     *  @see #equals
     *  {@inheritDoc}
     */
    public int compareTo(Object other);

    /*  Compute a hash code. See the contract for Object.hashCode:
     *  if a.equals(b), then a.hashCode() == b.hashCode().
     *  @return hash code
     *  {@inheritDoc}
     */
    public int hashCode();
}
