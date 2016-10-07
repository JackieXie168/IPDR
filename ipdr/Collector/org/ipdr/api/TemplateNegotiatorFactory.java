package org.ipdr.api;

import org.ipdr.api.configuration.ExporterConfigI;

//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/**
 * An interface to create instances of <code>TemplateNegotiator</code>
 */
public interface TemplateNegotiatorFactory
{
	/**
	 * Returns an instance of <code>TemplateNegotiator</code>
	 * @return An instance of <code>TemplateNegotiator</code>
	 */
	public TemplateNegotiator getInstance(ExporterConfigI exporteConfig);
}
