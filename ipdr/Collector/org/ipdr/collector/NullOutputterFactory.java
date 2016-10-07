package org.ipdr.collector;

import java.net.InetAddress;

import org.ipdr.api.Outputter;
import org.ipdr.api.OutputterFactory;

/**
 * The class produces instances of NullOutputter
 *
 */
public class NullOutputterFactory implements OutputterFactory
{

    /**
     * @see org.ipdr.api.OutputterFactory#create()
     */
    public Outputter create()
    {
        return new NullOutputter();
    }    

	/* (non-Javadoc)
	 * @see org.ipdr.api.OutputterFactory#create(java.net.InetAddress, int)
	 */
	public Outputter create(InetAddress transmitterAddress, int transmitterPort) {
		// TODO Auto-generated method stub
		return new NullOutputter();
	}
}
