package org.ipdr.api;

/**
 * This interface relates to a connection controller factory
 */
public interface ConnectionControllerFactory
{
    /**
     * Create an instance of ConnectionController from this factory
     * @return ConnectionController
     */
    public ConnectionController createInstance();
}
