package com.xacct.CRANE.collector;

// $Id: CollectorLog.java,v 1.5 2005/04/08 12:48:10 guyg Exp $
// Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.text.DateFormat;
import java.util.Date;
// JDK 1.4: import java.util.regex.*;

/** A simple implementation of logging, that outputs to stderr
 *  with time stamps.
 *
 *  @version $Id: CollectorLog.java,v 1.5 2005/04/08 12:48:10 guyg Exp $
 */
public class CollectorLog extends LogHandler implements java.io.Serializable
{
    /** $Id: CollectorLog.java,v 1.5 2005/04/08 12:48:10 guyg Exp $ */
    public static String versionStr() { return "@(#) $Id: CollectorLog.java,v 1.5 2005/04/08 12:48:10 guyg Exp $"; }

    /** Line prefix (handy for things such as the "progress" callback
     *  for CollectorSetupManager.
     */
    private String linePrefix;

    /** The constructor.
     *  @param pkgPrefix
     */
    public CollectorLog() {
        this("");
    }

    /** The constructor.
     * @param linePrefix A string to prefix on each output line (handy
     for things such as the "progress" callback for
     CollectorSetupManager, to allow distinguishing a set of log
     outputs).
     */
    public CollectorLog(String linePrefix) {
        this.linePrefix = linePrefix;
        Package pkg = this.getClass().getPackage();
        if (pkg == null) {
            pkgPrefix = "";
        } else {
            pkgPrefix = pkg.getName() + ".";
            // JDK 1.4: replace all "." is pkg name by "\\.",
            //   and Pattern.compile() the result.
        }
    }

    /** The output level (default: info). Values are:
     *  <br>0: debug
     *  <br>1: info
     *  <br>2: warning
     *  <br>3: error
     *  <br>4: fatal
     a*/
    private int level = 1; // info

    public boolean debug() { return (level <= 0); }
    public boolean info()  { return (level <= 1); }
    public boolean warn()  { return (level <= 2); }
    public boolean error() { return (level <= 3); }
    public boolean fatal() { return (level <= 4); }

    public void setDebug() { level = 0; }
    public void setInfo() { level = 1; }
    public void setWarn() { level = 2; }
    public void setError() { level = 3; }
    public void setFatal() { level = 4; }

    private final String DEBUG_TAG = "debug";
    private final String INFO_TAG  = " info";
    private final String WARN_TAG  = " warn";
    private final String ERROR_TAG = "error";
    private final String FATAL_TAG = "fatal";

    public void debug(String className, String function, String msg) {
        if (debug()) { logWriteX(DEBUG_TAG, className, function, msg); }
    }

    public void info(String className, String function, String msg) {
        if (info()) { logWriteX(INFO_TAG, className, function, msg); }
    }

    public void warn(String className, String function, String msg) {
        if (warn()) { logWriteX(WARN_TAG, className, function, msg); }
    }

    public void error(String className, String function, String msg) {
        if (error()) { logWriteX(ERROR_TAG, className, function, msg); }
    }

    public void fatal(String className, String function, String msg) {
        if (fatal()) { logWriteX(FATAL_TAG, className, function, msg); }
    }


    /** The package prefix to remove from messages.
     *  This is set by the constructor to this package's
     *  name, followed by "." (e.g., "<code>com.xacct.CRANE.collector.</code>").
     *
     *  XXX With JDK 1.4, this should be replaced by a Pattern, and
     *  the pattern should be set by something like
     *  Pattern.compile("<code>^com\\.xacct\\.CRANE\\.collector\\.</code>").
     */
    private String pkgPrefix = "";

    /** The utility for outputting a single message.
     *
     * @param level The level ("debug", " info", etc.).
     * @param className The class name.
     * @param function The function (method).
     * @param msg The message.
     */
    protected synchronized void logWriteX(String level, String className, String function, String msg) {
        synchronized(dateISOformat) {
            String classNameFull = className;
            if (msg == null) { msg = ""; }
            if (function == null) { function = ""; }
            if (className == null) { className = ""; }

            // Not much point in the following commented out stuff
            // because Class.forName() requires a fully-qualified name
            // (e.g., "com.xacct.CRANE.receiver.CollectorThread")
            // try {
            //     classNameFull = Class.forName(className).getName();
            // } catch (ClassNotFoundException e) {
            //     classNameFull = "**(" + e + ")**" + className;
            // }

            String date = dateISOformat.format(new Date());
            if (true) {
                // XXX JDK 1.4: className = pkgPrefixPattern.matcher(className).replaceFirst("");
                if (className.startsWith(pkgPrefix)) {
                    className = className.substring(pkgPrefix.length());
                }
                // Upper-case first character of message, for consistency.
                if (msg.length() > 0 && Character.isLowerCase(msg.charAt(0))) {
                    msg = Character.toUpperCase(msg.charAt(0)) + msg.substring(1);
                }
                printerrln(this.linePrefix +
                           ":" + date.substring(date.length() - 2) + " " + // hh:mm:ss -- leave date off for compactness
                           ((level != INFO_TAG && level != DEBUG_TAG) ? ("[" + level + "] ") : "") +
                           msg + " :: " + function + "[" + className + "] " + date);
            } else {
                boolean firstLine = true;
                int offset = 0;
                while (offset < msg.length()) {
                    if (firstLine) {
                        printerr(linePrefix +
                            date +
                            " " + level +
                            "[" + classNameFull + "." + function + "] thread " +
                            Thread.currentThread().getName());
                    } else {
                        printerr(linePrefix +
                            blanksOfLength(date) +
                            " " + blanksOfLength(level) +
                            " " + blanksOfLength(classNameFull) +
                            " " + blanksOfLength(function) + "  thread " +
                            Thread.currentThread().getName());
                    }

                    printerr("\t... ");
                    int nl = msg.indexOf('\n', offset);
                    if (nl < 0) {
                        printerrln(msg.substring(offset));
                        offset = msg.length();
                    } else {
                        printerrln(msg.substring(offset, nl - 1));
                        offset = nl + 1;
                    }
                    firstLine = false;
                }
            }
        }
    }

    /** Generate a string of blanks.
     *  @param length The number of blanks.
     *  @return A string of exactly "length" blanks.
     */
    private static final String blanksOfLength(int length) {
        StringBuffer blanks = new StringBuffer(length);
        for (; length > 0; length--) {
            blanks.append(' ');
        }
        return blanks.toString();
    }

    /** Generate a string of blanks.
     *  @param str A pattern string
     *  @return A string of blanks, the same length as "st".
     */
    private static final String blanksOfLength(String str) {
        return blanksOfLength(str.length());
    }

    /** Date format for generating ISO-format dates (for suitable
     *  synchronization)
     */
    private final static DateFormat dateISOformat =
        CollectorUtil.factoryDateFormat();
}
