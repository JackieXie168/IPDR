package org.ipdr.collector;

// $Id: CsvOutputterFactory.java 2.100 2003/03/29 19:04:30 [release 2.1.2 2003-04-15T21-36-38Z] $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.io.PrintStream;
import java.net.InetAddress;

import org.ipdr.api.Outputter;
import org.ipdr.api.OutputterFactory;

/**
 * Factory for CsvOutputter objects
 */
public final class CsvOutputterFactory implements OutputterFactory
{
	/* (non-Javadoc)
	 * @see org.ipdr.api.OutputterFactory#create(java.net.InetAddress, int)
	 */
	public Outputter create(InetAddress transmitterAddress, int transmitterPort)
	{
		return new CsvOutputter(out, sep);
	}
    /**
     * Constructor
     * 
     * @param out
     *            The output stream (typically a PrintStream [System.out] or a
     *            FileOutputStream, or BufferedOutputStream)
     * @param sep
     *            The separator character
     */
    public CsvOutputterFactory(PrintStream out, String sep)
    {
        this.out = out;
        this.sep = sep;
    }

    /**
     * Holds <code>PrintStream</code> instance for future Csv outputters
     */
    private PrintStream out;

    /**
     * Separator symbols for the records
     */
    private String      sep;

    /**
     * @see org.ipdr.api.OutputterFactory#create()
     */
    public Outputter create()
    {
        return new CsvOutputter(out, sep);
    }
}