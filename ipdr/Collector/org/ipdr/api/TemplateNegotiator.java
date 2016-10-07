package org.ipdr.api;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.util.ArrayList;

/**
 * Provides interface for custom template negotiators
 */
public interface TemplateNegotiator
{
	/**
	 * Allows rejection of the entire set of templates
	 * @param templates <code>ArrayList</code> of templates to accept/reject
	 * @return true if the templates are acceptable 
	 */
	public boolean isTemplateDataAcceptable( ArrayList templates );
	/**
	 * Allows negotiation of templates
	 * @param sessionId session id
	 * @param incomingTemplates incoming templates
	 * @return outgoing templates (will be sent to exporter as is)
	 */
	public ArrayList negotiateTemplate( byte sessionId, ArrayList incomingTemplates );
}
