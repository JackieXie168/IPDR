package org.ipdr.api.log;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/**
 * An abstraction of logging. This is provided as an abstract class rather than
 * an interface because a number of useful methods are provided here; and life
 * would be more complicated with pure interfaces.
 */
public abstract class LogHandler 
{

    /**
     * Print to stdout and flush
     * 
     * @param str
     *            The string to print
     */
    public static void print(String str)
    {
        synchronized (System.out)
        {
            System.out.print(str);
            System.out.flush();
        }
    }

    /**
     * Print to stderr and flush
     * 
     * @param str
     *            The string to print
     */
    public static void printerr(String str)
    {
        synchronized (System.err)
        {
            System.err.print(str);
            System.err.flush();
        }
    }

    /**
     * Print a line to stderr and flush
     * 
     * @param line
     *            The line to print
     */
    public static void printerrln(String line)
    {
        synchronized (System.err)
        {
            System.err.println(line);
            System.err.flush();
        }
    }
    /**
     * Print a line to stdout and flush
     * 
     * @param line
     *            The line to print
     */
    public static void println(String line)
    {
        synchronized (System.out)
        {
            System.out.println(line);
            System.out.flush();
        }
    }

    /**
     * Test whether the log is outputting "debug" (and more severe) messages.
     * 
     * @return true if outputting "debug".
     */
    abstract public boolean debug();

    /**
     * Output a debug-level message.
     * 
     * @param o
     *            The object in whose context this is being output (may be
     *            null).
     * @param function
     *            The function (method) that is being executed.
     * @param msg
     *            The specific message.
     */
    public void debug(Object o, String function, String msg)
    {
        debug((o == null) ? "" : o.getClass().getName(), function, msg);
    }

    /**
     * Output a debug-level message.
     * 
     * @param className
     *            The name of the class where the message is being output from.
     * @param function
     *            The function (method) that is being executd.
     * @param The
     *            specific message.
     */
    abstract public void debug(String className, String function, String msg);

    /**
     * Test whether the log is outputting "error" (and more severe) messages.
     * 
     * @return true if outputting "error".
     */
    abstract public boolean error();

    /**
     * Output a error-level message.
     * 
     * @param o
     *            The object in whose context this is being output (may be
     *            null).
     * @param function
     *            The function (method) that is being executed.
     * @param msg
     *            The specific message.
     */
    public void error(Object o, String function, String msg)
    {
        error((o == null) ? "" : o.getClass().getName(), function, msg);
    }

    /**
     * Output a error-level message.
     * 
     * @param className
     *            The name of the class where the message is being output from.
     * @param function
     *            The function (method) that is being executd.
     * @param The
     *            specific message.
     */
    abstract public void error(String className, String function, String msg);

    /**
     * Test whether the log is outputting "fatal" (and more severe) messages.
     * 
     * @return true if outputting "fatal".
     */
    abstract public boolean fatal();

    /**
     * Output a fatal-level message.
     * 
     * @param o
     *            The object in whose context this is being output (may be
     *            null).
     * @param function
     *            The function (method) that is being executed.
     * @param msg
     *            The specific message.
     */
    public void fatal(Object o, String function, String msg)
    {
        fatal((o == null) ? "" : o.getClass().getName(), function, msg);
    }

    /**
     * Output a fatal-level message.
     * 
     * @param className
     *            The name of the class where the message is being output from.
     * @param function
     *            The function (method) that is being executd.
     * @param The
     *            specific message.
     */
    abstract public void fatal(String className, String function, String msg);

    /**
     * Test whether the log is outputting "info" (and more severe) messages.
     * 
     * @return true if outputting "info".
     */
    abstract public boolean info();

    /**
     * Output a info-level message.
     * 
     * @param o
     *            The object in whose context this is being output (may be
     *            null).
     * @param function
     *            The function (method) that is being executed.
     * @param msg
     *            The specific message.
     */
    public void info(Object o, String function, String msg)
    {
        info((o == null) ? "" : o.getClass().getName(), function, msg);
    }

    /**
     * Output a info-level message.
     * 
     * @param className
     *            The name of the class where the message is being output from.
     * @param function
     *            The function (method) that is being executd.
     * @param The
     *            specific message.
     */
    abstract public void info(String className, String function, String msg);

    /**
     * Set the logger to ouput "debug" (and more severe) messages.
     */
    abstract public void setDebug();

    /**
     * Set the logger to ouput "error" (and more severe) messages.
     */
    abstract public void setError();

    /**
     * Set the logger to ouput "fatal" (and more severe) messages.
     */
    abstract public void setFatal();

    /**
     * Set the logger to ouput "info" (and more severe) messages.
     */
    abstract public void setInfo();

    /**
     * Set the logger to ouput "warning" (and more severe) messages.
     */
    abstract public void setWarn();

    /**
     * Test whether the log is outputting "warning" (and more severe) messages.
     * 
     * @return true if outputting "warning".
     */
    abstract public boolean warn();

    /**
     * Output a warning-level message.
     * 
     * @param o
     *            The object in whose context this is being output (may be
     *            null).
     * @param function
     *            The function (method) that is being executed.
     * @param msg
     *            The specific message.
     */
    public void warn(Object o, String function, String msg)
    {
        warn((o == null) ? "" : o.getClass().getName(), function, msg);
    }

    /**
     * Output a warning-level message.
     * 
     * @param className
     *            The name of the class where the message is being output from.
     * @param function
     *            The function (method) that is being executd.
     * @param The
     *            specific message.
     */
    abstract public void warn(String className, String function, String msg);

}