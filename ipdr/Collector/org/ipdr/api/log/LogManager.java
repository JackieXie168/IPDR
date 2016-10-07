package org.ipdr.api.log;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/**
 * The class manages logging for the collector. It uses a LogHandler for the
 * actual logging.
 */
public class LogManager
{
    /**
     * Holds the current log handler
     */
    private static LogHandler handler = null;

    /**
     * Test whether the log is outputting "debug" (and more severe) messages.
     * 
     * @return true if outputting "debug".
     */
    public static boolean debug()
    {
        if (handler != null) return handler.debug();
        return false;
    }

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
    public static void debug(Object o, String function, String msg)
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
    public static void debug(String className, String function, String msg)
    {
        if (handler != null) handler.debug(className, function, msg);
    }

    /**
     * Test whether the log is outputting "error" (and more severe) messages.
     * 
     * @return true if outputting "error".
     */
    public static boolean error()
    {
        if (handler != null) return handler.error();
        return false;
    }

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
    public static void error(Object o, String function, String msg)
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
    public static void error(String className, String function, String msg)
    {
        if (handler != null) handler.error(className, function, msg);
    }

    /**
     * Test whether the log is outputting "fatal" (and more severe) messages.
     * 
     * @return true if outputting "fatal".
     */
    public static boolean fatal()
    {
        if (handler != null) return handler.fatal();
        return false;
    }

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
    public static void fatal(Object o, String function, String msg)
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
    public static void fatal(String className, String function, String msg)
    {
        if (handler != null) handler.fatal(className, function, msg);
    }

    /**
     * Test whether the log is outputting "info" (and more severe) messages.
     * 
     * @return true if outputting "info".
     */
    public static boolean info()
    {
        if (handler != null) return handler.info();
        return false;
    }

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
    public static void info(Object o, String function, String msg)
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
    public static void info(String className, String function, String msg)
    {
        if (handler != null) handler.info(className, function, msg);
    }

    /**
     * Print a line and flush
     * 
     * @param line
     *            The line to print
     */
    public static void println(String msg)
    {
        if (handler != null) LogHandler.println(msg);
    }

    public static void registerLogHandler(LogHandler handler)
    {
        LogManager.handler = handler;
    }

    /**
     * Set the logger to ouput "debug" (and more severe) messages.
     */
    public static void setDebug()
    {
        if (handler != null) handler.setDebug();
    }

    /**
     * Set the logger to ouput "error" (and more severe) messages.
     */
    public static void setError()
    {
        if (handler != null) handler.setError();
    }

    /**
     * Set the logger to ouput "fatal" (and more severe) messages.
     */
    public static void setFatal()
    {
        if (handler != null) handler.setFatal();
    }

    /**
     * Set the logger to ouput "info" (and more severe) messages.
     */
    public static void setInfo()
    {
        if (handler != null) handler.setInfo();
    }

    /**
     * Set the logger to ouput "warning" (and more severe) messages.
     */
    public static void setWarn()
    {
        if (handler != null) handler.setWarn();
    }

    /**
     * For debugging when we don't have an interactive debugger; this can be
     * called anywhere to give information about where we are.
     * 
     * @param msg
     *            Message to output with the stack trace.
     */
    public static void stackTrace(String msg)
    {
        Exception e = new Exception(msg);
        System.err.println("Forcing stack trace: " + msg);
        stackTrace(e);
    }

    /**
     * Print a stack trace on stderr.
     * 
     * @param e
     *            The exceptions, whose stack trace is to be output.
     */
    public static void stackTrace(Throwable e)
    {
        e.printStackTrace(System.err);
        System.err.flush();
    }

    /**
     * Test whether the log is outputting "warning" (and more severe) messages.
     * 
     * @return true if outputting "warning".
     */
    public static boolean warn()
    {
        if (handler != null) return handler.warn();
        return false;
    }

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
    public static void warn(Object o, String function, String msg)
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
    public static void warn(String className, String function, String msg)
    {
        if (handler != null) handler.warn(className, function, msg);
    }
}