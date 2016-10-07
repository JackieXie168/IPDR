package org.ipdr.utility;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.AbstractCollection;
import java.util.ArrayList;
import java.util.Date;
import java.util.ListIterator;

/**
 * Holds various miscellanous utilities
 *  
 */
public final class IPDRUtilities
{
    /**
     * Date format for generating ISO-format dates
     */
    private final static DateFormat dateISOformat = factoryDateFormat();

    /**
     * Compare two booleans, treating true as 1 and false as 0.
     * 
     * @param a
     *            First value to compare.
     * @param b
     *            Second value to compare.
     * @return Comparison value (-1, 0, 1).
     */
    static public final int compareToBool(boolean a, boolean b)
    {
        return (a && b) ? 0 : ((!a) && (!b)) ? 0 : ((!a) && b) ? -1 : 1;
    }

    /**
     * Convert a Date to an ISO-standard format. This is thread safe, even
     * though the underlying DateFormat.format() method is not thread safe.
     * 
     * @param date
     *            The Date to convert
     * @return string in the format yyyy-mm-ddThh:mm:ss
     * @see #factoryDateFormat
     */
    public final static String dateFormat(Date date)
    {
        synchronized (dateISOformat)
        {
            return dateISOformat.format(date);
        }
    }

    /**
     * Convert a value in microseconds to an ISO-standard format. This is thread
     * safe, even though the underlying DateFormat.format() method is not thread
     * safe.
     * 
     * @param milli
     *            The number of micro-seconds since 00:00:00 1 Jan 1970
     * @return string in the format yyyy-mm-ddThh:mm:ss.xxxxxx
     * @see #factoryDateFormat
     */
    public final static String dateFormatMicrosec(long micros)
    {
        String u = Long.toString(micros % 1000000);
        String d = dateFormat(dateFromSeconds(micros));
        StringBuffer result = new StringBuffer(d.length() + 7);
        result.append(d).append(".");
        for (int i = 6 - u.length(); i > 0; i--)
        {
            result.append("0");
        }
        result.append(u);
        return result.toString();
    }

    /**
     * Convert a value in milliseconds to an ISO-standard format. This is thread
     * safe, even though the underlying DateFormat.format() method is not thread
     * safe.
     * 
     * @param milli
     *            The number of milli-seconds since 00:00:00 1 Jan 1970
     * @return string in the format yyyy-mm-ddThh:mm:ss.xxx
     * @see #factoryDateFormat
     */
    public final static String dateFormatMillisec(long millis)
    {
        String u = Long.toString(millis % 1000);
        String d = dateFormat(dateFromSeconds(millis/1000));
        StringBuffer result = new StringBuffer(d.length() + 4);
        result.append(d).append(".");
        for (int i = 3 - u.length(); i > 0; i--)
        {
            result.append("0");
        }
        result.append(u);
        return result.toString();
    }

    /**
     * Convert a value in seconds to an ISO-standard format. This is thread
     * safe, even though the underlying DateFormat.format() method is not thread
     * safe.
     * 
     * @param seconds
     *            The number of seconds since 00:00:00 1 Jan 1970
     * @return string in the format yyyy-mm-ddThh:mm:ss
     * @see #factoryDateFormat
     */
    public final static String dateFormatSeconds(long seconds)
    {
        return dateFormat(dateFromSeconds(seconds));
    }

    /**
     * Convert seconds since Jan 1, 1970 to a Date
     * 
     * @param seconds
     *            since Jan 1, 1970
     * @return the date
     */
    public final static Date dateFromSeconds(long seconds)
    {
        return new Date(seconds * 1000);
    }

    /**
     * Convert a Date to seconds since Jan 1, 1970.
     * 
     * @param date
     *            The Date to convert
     * @return seconds since Jan 1, 1970
     */
    public final static long dateInSeconds(Date date)
    {
        return (date.getTime() + 500) / 1000;
    }

    /**
     * Create a data format for ISO dates. This might be needed if there is
     * deadlock with the dateFormat method.
     * 
     * @return a data format which will format a date into ISO format
     * @see #dateFormat
     */
    public final static DateFormat factoryDateFormat()
    {
        return new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss");
    }

    /**
     * Get an ArrayList, converting null to a zero-length array.
     * 
     * @param list
     *            The base list, may be null.
     * @return The base list if it's non-null; otherwise a zero-length array.
     */
    public final static ArrayList nonNull(ArrayList list)
    {
        return (list == null) ? new ArrayList(0) : list;
    }

    /**
     * Get a String, converting null to a zero-length string. Also intern's the
     * string for faster comparisons later.
     * 
     * @param str
     *            The base string, may be null.
     * @return The base list if it's non-null; otherwise a zero-length string.
     */
    public final static String nonNull(String str)
    {
        return (str == null) ? "" : str.trim().intern();
    }

    /**
     * From a list of objects, produce a string with all their elements. Space is
     * used as the separator
     * 
     * @param list
     *            The list of objects.
     */
    public final static String toString(AbstractCollection list)
    {
        return toString(list, " ");
    }

    /**
     * From a list of objects, produce a string with all their elements.
     * 
     * @param list
     *            The list of objects.
     * @param sep
     *            The separator between individual objects on output.
     * @return list[0].toString() + sep + list[1].toString() + sep + ... +
     *         list[list.length].toString()
     */
    public final static String toString(AbstractCollection list, String sep)
    {
        return toString(new ArrayList(list), sep);
    }

    /**
     * From a list of objects, produce a string with all their elements. Space is
     * used as the separator
     * 
     * @param list
     *            The list of objects.
     * @return list[0].toString() + " " + list[1].toString() + " " + ... +
     *         list[list.length].toString()
     */
    public final static String toString(ArrayList list)
    {
        return toString(list, " ");
    }

    /**
     * From a list of objects, produce a string with all their elements.
     * 
     * @param list
     *            The list of objects.
     * @param sep
     *            The separator between individual objects on output.
     * @return list[0].toString() + sep + list[1].toString() + sep + ... +
     *         list[list.length].toString()
     */
    public final static String toString(ArrayList list, String sep)
    {
        StringBuffer result = new StringBuffer(1000);
        String sepStr = "";
        for (ListIterator it = list.listIterator(); it.hasNext();)
        {
            result.append(sep).append(it.next().toString());
            sepStr = sep;
        }
        return result.toString();
    }
}