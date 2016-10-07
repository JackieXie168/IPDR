package org.ipdr.collector;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.io.PrintStream;
import java.net.InetAddress;

import org.ipdr.api.Outputter;
import org.ipdr.api.OutputterFactory;

/**
 * Factory for DedupOutputter objects
 * 
 */
public final class DedupSummaryOutputterFactory implements OutputterFactory
{
    /**
     * Constructs DedupSummaryOutputterFactory with given <code>PrintStream</code>
     * @param out the PrintStream
     */
    public DedupSummaryOutputterFactory(PrintStream out)
    {
        this.out = out;
    }

    /**
     * Holds the <code>PrintStream</code> for the produced outputters
     */
    private PrintStream out;

    /**
     * @see org.ipdr.api.OutputterFactory#create()
     */
    public Outputter create()
    {
        return new DedupSummaryOutputter(out);
    }
    
	/* (non-Javadoc)
	 * @see org.ipdr.api.OutputterFactory#create(java.net.InetAddress, int)
	 */
	public Outputter create(InetAddress transmitterAddress, int transmitterPort) {
		// TODO Auto-generated method stub
		return new DedupSummaryOutputter(out);
	}
}