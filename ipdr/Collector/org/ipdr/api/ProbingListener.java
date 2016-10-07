package org.ipdr.api;

import java.util.ArrayList;

import org.ipdr.api.configuration.ExporterConfigI;

/**
 * 
 * The interface is used for probing sessions of a specified exporter
 */
public interface ProbingListener
{
    /**
     * Probing for specified sessions in a specified exporter
     * @param exporter The configuration of the exporter to be probed
     * @param sessions List of the sessions info block 
     */
    public void probedSessions(ExporterConfigI exporter, ArrayList sessions);
}
