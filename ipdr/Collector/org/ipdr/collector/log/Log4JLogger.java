/*
 * Created on 29/03/2005
 *
 * TODO To change the template for this generated file go to
 * Window - Preferences - Java - Code Style - Code Templates
 */
package org.ipdr.collector.log;

import java.text.DateFormat;
import java.util.Date;

import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import org.ipdr.api.log.LogHandler;
import org.ipdr.utility.IPDRUtilities;
//import org.apache.log4j.Appender;
//import org.apache.log4j.FileAppender;
//import org.apache.log4j.SimpleLayout;

/**
 */
public class Log4JLogger extends LogHandler {

	private final String DEBUG_TAG = "debug";
    private final String INFO_TAG  = " info";
    private final String WARN_TAG  = " warn";
    private final String ERROR_TAG = "error";
    private final String FATAL_TAG = "fatal";
    
	static Logger logger = Logger.getLogger(Log4JLogger.class);
	
	private String linePrefix;
	
	private String pkgPrefix;
	
    private final static DateFormat dateISOformat = IPDRUtilities.factoryDateFormat();

	
	public Log4JLogger()
	{
		this("");
	}
	
	public Log4JLogger(String linePrefix)
	{
		String configurationFileName = "LoggerConfigurationFile.txt";
		
		this.linePrefix = linePrefix;
        Package pkg = this.getClass().getPackage();
        if (pkg == null)
        {
            pkgPrefix = "";
        }
        else
        {
            pkgPrefix = pkg.getName() + ".";
        }
        // applying logger configuration from file
        PropertyConfigurator.configure(configurationFileName);
	}
	/**
	 * @see org.ipdr.api.log.LogHandler#debug()
	 */
	public boolean debug() {
		// TODO Auto-generated method stub
		return true;
		//return false;
	}

	/**
	 * @see org.ipdr.api.log.LogHandler#debug(java.lang.String, java.lang.String, java.lang.String)
	 */
	public void debug(String className, String function, String msg) {
		// TODO Auto-generated method stub
		logWriteX(DEBUG_TAG, className, function, msg);

	}

	/**
	 * @see org.ipdr.api.log.LogHandler#error()
	 */
	public boolean error() {
		// TODO Auto-generated method stub
		return true;
		//return false;
	}

	/**
	 * @see org.ipdr.api.log.LogHandler#error(java.lang.String, java.lang.String, java.lang.String)
	 */
	public void error(String className, String function, String msg) {
		// TODO Auto-generated method stub
		logWriteX(ERROR_TAG, className, function, msg);

	}

	/**
	 * @see org.ipdr.api.log.LogHandler#fatal()
	 */
	public boolean fatal() {
		// TODO Auto-generated method stub
		return true;
		//return false;
	}

	/**
	 * @see org.ipdr.api.log.LogHandler#fatal(java.lang.String, java.lang.String, java.lang.String)
	 */
	public void fatal(String className, String function, String msg) {
		// TODO Auto-generated method stub
		logWriteX(FATAL_TAG, className, function, msg);

	}

	/**
	 * @see org.ipdr.api.log.LogHandler#info()
	 */
	public boolean info() {
		// TODO Auto-generated method stub
		return true;
		//return false;
	}

	/**
	 * @see org.ipdr.api.log.LogHandler#info(java.lang.String, java.lang.String, java.lang.String)
	 */
	public void info(String className, String function, String msg) {
		// TODO Auto-generated method stub
		logWriteX(INFO_TAG, className, function, msg);

	}

	/**
	 * @see org.ipdr.api.log.LogHandler#setDebug()
	 */
	public void setDebug() {
		// TODO Auto-generated method stub

	}

	/**
	 * @see org.ipdr.api.log.LogHandler#setError()
	 */
	public void setError() {
		// TODO Auto-generated method stub

	}

	/**
	 * @see org.ipdr.api.log.LogHandler#setFatal()
	 */
	public void setFatal() {
		// TODO Auto-generated method stub

	}

	/**
	 * @see org.ipdr.api.log.LogHandler#setInfo()
	 */
	public void setInfo() {
		// TODO Auto-generated method stub

	}

	/**
	 * @see org.ipdr.api.log.LogHandler#setWarn()
	 */
	public void setWarn() {
		// TODO Auto-generated method stub

	}

	/**
	 * @see org.ipdr.api.log.LogHandler#warn()
	 */
	public boolean warn() {
		// TODO Auto-generated method stub
		return true;
		//return false;
	}

	/**
	 * @see org.ipdr.api.log.LogHandler#warn(java.lang.String, java.lang.String, java.lang.String)
	 */
	public void warn(String className, String function, String msg) {
		// TODO Auto-generated method stub
		logWriteX(WARN_TAG, className, function, msg);

	}	
	
	/**
     * The utility for outputting a single message.
     * 
     * @param level
     *            The level ("debug", " info", etc.).
     * @param className
     *            The class name.
     * @param function
     *            The function (method).
     * @param msg
     *            The message.
     */
	protected synchronized void logWriteX(String level, String className, String function, String msg)
    {
        synchronized (dateISOformat)
        {
            String classNameFull = className;
            if (msg == null)
            {
                msg = "";
            }
            if (function == null)
            {
                function = "";
            }
            if (className == null)
            {
                className = "";
            }

            String date = dateISOformat.format(new Date());
            //if (true)
            {
                if (className.startsWith(pkgPrefix))
                {
                    className = className.substring(pkgPrefix.length());
                }
                // Upper-case first character of message, for consistency.
                if (msg.length() > 0 && Character.isLowerCase(msg.charAt(0)))
                {
                    msg = Character.toUpperCase(msg.charAt(0)) + msg.substring(1);
                }
                
                String finalMsg = this.linePrefix + ":" + date + " " + // hh:mm:ss --
                // leave date
                // off for
                // compactness
                ((level != INFO_TAG && level != DEBUG_TAG) ? ("[" + level + "] ") : "") + msg + " :: " + function + "[" + className + "] ";
                if (level.equals(DEBUG_TAG))
                {
                	logger.debug(finalMsg);
                }
                else if (level.equals(INFO_TAG))
                {
                	logger.debug(finalMsg);
                }
                else if (level.equals(WARN_TAG))
                {
                	logger.warn(finalMsg);
                }
                else if (level.equals(ERROR_TAG))
                {
                	logger.error(finalMsg);
                }
                else if (level.equals(FATAL_TAG))
                {
                	logger.fatal(finalMsg);
                }
            }
        }
    }
}
