package com.xacct.CRANE.collector;

// $Id: CollectorUtil.java,v 1.4 2005/01/30 15:13:43 ilyad Exp $
// Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.AbstractCollection;
import java.util.ArrayList;
import java.util.Date;
import java.util.ListIterator;

/** Various handy utilities - most are static.
 *
 *  @version $Id: CollectorUtil.java,v 1.4 2005/01/30 15:13:43 ilyad Exp $
 */
public final class CollectorUtil implements java.io.Serializable
{
    /** $Id: CollectorUtil.java,v 1.4 2005/01/30 15:13:43 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: CollectorUtil.java,v 1.4 2005/01/30 15:13:43 ilyad Exp $"; }

    /** Convert a Date to seconds since Jan 1, 1970.
     *  @param date The Date to convert
     *  @return seconds since Jan 1, 1970
     */
    public final static long dateInSeconds(Date date) {
        return (date.getTime() + 500) / 1000;
    }

    /** Convert seconds since Jan 1, 1970 to a Date
     *  @param seconds since Jan 1, 1970
     *  @return the date
     */
    public final static Date dateFromSeconds(long seconds) {
        return new Date(seconds * 1000);
    }

    /** Convert a Date to an ISO-standard format.
     *  This is thread safe, even though the underlying
     *  DateFormat.format() method is not thread safe.
     *  @param date The Date to convert
     *  @return string in the format yyyy-mm-ddThh:mm:ss
     *  @see #factoryDateFormat
     */
    public final static String dateFormat(Date date) {
        synchronized(dateISOformat) {
            return dateISOformat.format(date);
        }
    }

    /** Convert a value in seconds to an ISO-standard format.
     *  This is thread safe, even though the underlying
     *  DateFormat.format() method is not thread safe.
     *  @param seconds The number of seconds since 00:00:00 1 Jan 1970
     *  @return string in the format yyyy-mm-ddThh:mm:ss
     *  @see #factoryDateFormat
     */
    public final static String dateFormatSeconds(long seconds) {
        return dateFormat(dateFromSeconds(seconds));
    }

    /** Convert a value in milliseconds to an ISO-standard format.
     *  This is thread safe, even though the underlying
     *  DateFormat.format() method is not thread safe.
     *  @param milli The number of milli-seconds since 00:00:00 1 Jan 1970
     *  @return string in the format yyyy-mm-ddThh:mm:ss.xxx
     *  @see #factoryDateFormat
     */
    public final static String dateFormatMillisec(long millis) {
        String u = Long.toString(millis % 1000);
        String d = dateFormat(dateFromSeconds(millis));
        StringBuffer result = new StringBuffer(d.length() + 4);
        result.append(d).append(".");
        for (int i = 3 - u.length(); i > 0; i--) {
            result.append("0");
        }
        result.append(u);
        return result.toString();
    }

    /** Convert a value in microseconds to an ISO-standard format.
     *  This is thread safe, even though the underlying
     *  DateFormat.format() method is not thread safe.
     *  @param milli The number of micro-seconds since 00:00:00 1 Jan 1970
     *  @return string in the format yyyy-mm-ddThh:mm:ss.xxxxxx
     *  @see #factoryDateFormat
     */
    public final static String dateFormatMicrosec(long micros) {
        String u = Long.toString(micros % 1000000);
        String d = dateFormat(dateFromSeconds(micros));
        StringBuffer result = new StringBuffer(d.length() + 7);
        result.append(d).append(".");
        for (int i = 6 - u.length(); i > 0; i--) {
            result.append("0");
        }
        result.append(u);
        return result.toString();
    }

    /** Create a data format for ISO dates. This might be needed
     *  if there is deadlock with the dateFormat method.
     *  @return a data format which will format a date into ISO format
     *  @see #dateFormat
     */
    public final static DateFormat factoryDateFormat() {
        return new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss");
    }

    /// Date format for generating ISO-format dates
    private final static DateFormat dateISOformat = factoryDateFormat();

    /** Put a Java array into an ArrayList, updating the ArrayList.
     * @param result The ArrayList that gets updated with the elements in the list.
     * @param list The list whose elements are put into the result.
     * @see #toArrayList(ConfigObject [])
     */
    public final static void toArrayList(ArrayList result, ConfigObject [] list) {
        result.clear();
        if (list != null) {
            result.ensureCapacity(list.length);
            for (int i = 0; i < list.length; i++) {
                result.add(list[i]);
            }
        }
    }

    /** Convert a Java array to an Array List.
     * @param list The array whose elements are put into the result.
     * @return A new ArrayList, with the elements in the list.
     * @see #toArrayList(ArrayList, ConfigObject [])
     */
    public final static ArrayList toArrayList(ConfigObject [] list) {
        if (list == null) {
            return new ArrayList(0);
        } else {
            ArrayList result = new ArrayList(list.length);
            for (int i = 0; i < list.length; i++) {
                result.add(list[i]);
            }
            return result;
        }
    }

//     /** Convert an ArrayList to a Java array.
//      *  To use:
//      *  <br><code>private static final SessionConfig [] arrayPrototype = new SessionConfig[0];</code>
//      *  <br><code>public static final SessionConfig [] toArray(ArrayList list) {</code>
//      *  <br><code>&nbsp;&nbspreturn (SessionConfig []) CollectorUtil.toArray(list, arrayPrototype);</code>
//      *  <br><code>}</code>
//      *
//      * @param list The array, or null.
//      * @param prototype A zero-length prototype array (new ConfigObject[0]).
//      * @return The Java array, with all the elements of the list. You must
//      *    cast this to Xxx[] where Xxx is the type of the elements in the
//      *    list.
//      */
//     public final static Object toArray(ArrayList list, ConfigObject [] prototype) {
//         if (list == null) {
//             return prototype.clone();
//         } else {
//             return list.toArray(prototype);
//         }
//     }

    /** From a list of objects, produce a string with all their elements.
     * @param list The list of objects.
     * @param sep The separator between individual objects on output.
     * @return list[0].toString() + sep + list[1].toString() + sep + ... + list[list.length].toString()
     */
    public final static String toString(ConfigObject [] list, String sep) {
        StringBuffer result = new StringBuffer(1000);
        String sepStr = "";
        for (int i = 0; i < list.length; i++) {
            result.append(sepStr).append(list[i].toString());
            sepStr = sep;
        }
        return result.toString();
    }

    public final static String toString(ConfigObject [] list) { return toString(list, " "); }

    public final static String toString(ArrayList list, String sep) {
        StringBuffer result = new StringBuffer(1000);
        String sepStr = "";
        for (ListIterator it = list.listIterator(); it.hasNext(); ) {
            result.append(sep).append(it.next().toString());
            sepStr = sep;
        }
        return result.toString();
    }

    public final static String toString(AbstractCollection list) { return toString(list, " "); }
    public final static String toString(AbstractCollection list, String sep) {
        return toString(new ArrayList(list), sep);
    }

    public final static String toString(ArrayList list) { return toString(list, " "); }

    public final static String toStringLong(ConfigObject [] list) {
        return toStringLong(list, "");
    }

    public final static String toStringLong(ConfigObject [] list, String indent) {
        return toStringLong(list, indent, "");
    }

    public final static String toStringLong(ConfigObject [] list, String indent, String sep) {
        StringBuffer result = new StringBuffer(1000);
        String indent2 = indent + "    ";
        for (int i = 0; i < list.length; i++) {
            result.append(sep).append(indent).append(list[i].toStringLong(indent2));
            sep = "\n";
        }
        if (list.length > 0) {
            result.append(sep).append(indent).append("(end ").append(list[0].getClass().getName()).append(" list)");
        }
        return result.toString();
    }

    public final static String toStringLong(ArrayList list) {
        return toStringLong(list, "");
    }

    public final static String toStringLong(AbstractCollection list) { return toString(list, " "); }
    public final static String toStringLong(AbstractCollection list, String indent) {
        return toStringLong(new ArrayList(list), indent);
    }

    public final static String toStringLong(ArrayList list, String indent) {
        return toStringLong(list, indent, "");
    }

    public final static String toStringLong(ArrayList list, String indent, String sep) {
        StringBuffer result = new StringBuffer(1000);
        String indent2 = indent + "    ";
        for (ListIterator it = list.listIterator(); it.hasNext(); ) {
            Object elem = it.next();
            result.append(sep).append(indent);
            if (elem instanceof Integer) {
                // result.append("Integer(").append(((Integer)elem).intValue()).append(")");
                result.append(((Integer)elem).intValue());
            } else {
                result.append(((ConfigObject)elem).toStringLong(indent2));
            }
            sep = "\n";
        }
        if (! list.isEmpty()) {
            result.append(sep).append(indent).append("(end ").append(list.get(0).getClass().getName()).append(" list)");
        }
        return result.toString();
    }

    /** Compare two booleans, treating true as 1 and false as 0.
     * @param a First value to compare.
     * @param b Second value to compare.
     * @return Comparison value (-1, 0, 1).
     */
    static public final int compareToBool(boolean a, boolean b) {
        return
            (a && b) ? 0 :
            ((!a) && (!b)) ? 0 :
            ((!a) && b) ? -1 : 1;
    }

    /** Get an ArrayList, converting null to a zero-length array.
     *  @param list The base list, may be null.
     *  @return The base list if it's non-null; otherwise a zero-length array.
     */
    public final static ArrayList nonNull(ArrayList list) {
        return (list == null) ? new ArrayList(0) : list;
    }

    /** Get a String, converting null to a zero-length string.
     *  Also intern's the string for faster comparisons later.
     *  @param str The base string, may be null.
     *  @return The base list if it's non-null; otherwise a zero-length string.
     */
    public final static String nonNull(String str) {
        return (str == null) ? "" : str.trim().intern();
    }

//     /** Ensure that a Java array is non-null, if necessary creating a zero-length array.
//      * @param array The array, or null.
//      * @param prototype A zero-length prototype array.
//      * @return The array, if no null; otherwise a clone of the zero-length prototype array.
//      *    You must cast it to the appropriate xxx [] type.
//      */
//     public final static Object nonNull(ConfigObject array, ConfigObject [] prototype) {
//         return (array == null) ? prototype.clone() : array;
//     }
}
