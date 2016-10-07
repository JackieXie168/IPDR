package org.ipdr.utility;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.util.ArrayList;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.TimerTask;

import org.ipdr.IPDRStateMachine;
import org.ipdr.ProtocolStateMachine;

/**
 * The class facilitates all timers as used by protocol state machines. The
 * timers include: connect timeout, keepalive timeouts, data acknowledge
 * timeouts
 */
public class TimeoutTimerTask extends TimerTask
{
    /**
     * Value for timeout value check interval, in milliseconds
     */
    public static final int TIMEOUT_INTERVAL = 500;

    /**
     * Special value for a timeout which is turned off
     */
    private final static int TIMEOUT_RESET = Integer.MAX_VALUE;

    /**
     * Collector keepalive timeout
     */
    protected volatile long collectorKATimeout = TIMEOUT_RESET;

    /**
     * Connect timeout
     */
    protected volatile long connectTimeout = TIMEOUT_RESET;

    /**
     * <code>Hashtable</code> which maps session ids to data acknowledge
     * timeouts
     */
    protected Hashtable dataAckTimeoutMap = new Hashtable();

    /**
     * Exporter keepalive timeout
     */
    protected volatile long exporterKATimeout = TIMEOUT_RESET;

    /**
     * Instance of protocol state machine to communicate with
     */
    protected ProtocolStateMachine stateMachine;

    /**
     * Constructs the timer task, initializing the pointer to protocol state
     * machine instance
     * 
     * @param stateMachine
     *            state machine instance to communicate with
     */
    public TimeoutTimerTask(ProtocolStateMachine stateMachine)
    {
        this.stateMachine = stateMachine;
    }

    /**
     * Resets collector keepalive timeout
     */
    public void resetCollectorKeepaliveTimeout()
    {
        collectorKATimeout = TIMEOUT_RESET;
    }

    /**
     * Resets connect timeout
     */
    public void resetConnectTimeout()
    {
        connectTimeout = TIMEOUT_RESET;
    }

    /**
     * Resets data acknowledge timeout for a given session
     * 
     * @param sessionId
     *            session id to reset the timeout for.
     */
    public void resetDataAckTimeout(byte sessionId)
    {
        synchronized (this)
        {
            Byte id = new Byte(sessionId);
            if (dataAckTimeoutMap.containsKey(id))
                    dataAckTimeoutMap.remove(id);
        }
    }

    /**
     * Resets exporter keepalive timeout
     */
    public void resetExporterKeepaliveTimeout()
    {
        exporterKATimeout = TIMEOUT_RESET;
    }

    /**
     * This method is invoked by the platform. The method checks and decreases
     * all latches, notifying the state machine if a timeout fires.
     * 
     * @see java.lang.Runnable#run()
     */
    public void run()
    {
        if (collectorKATimeout != TIMEOUT_RESET)
                collectorKATimeout -= TIMEOUT_INTERVAL;

        if (collectorKATimeout == 0)
                ((IPDRStateMachine) stateMachine).collectorKeepAliveTimeout();

        if (exporterKATimeout != TIMEOUT_RESET)
                exporterKATimeout -= TIMEOUT_INTERVAL;

        if (exporterKATimeout == 0)
                ((IPDRStateMachine) stateMachine).exporterKeepAliveTimeout();
        if (connectTimeout != TIMEOUT_RESET)
                connectTimeout -= TIMEOUT_INTERVAL;
        if (connectTimeout == 0) stateMachine.connectTimeout();

        ArrayList fireTimeouts = new ArrayList();

        synchronized (this)
        {
            Enumeration enkeys = dataAckTimeoutMap.keys();
            while (enkeys.hasMoreElements())
            {
                Byte key = (Byte) enkeys.nextElement();
                long timeout = ((Long) dataAckTimeoutMap.get(key)).longValue();
                dataAckTimeoutMap.remove(key);
                timeout -= TIMEOUT_INTERVAL;
                if (timeout == 0)
                {
                    fireTimeouts.add(key);
                }
                else
                {
                    dataAckTimeoutMap.put(key, new Long(timeout));
                }
            }
        }
        if (fireTimeouts.size() != 0)
                for (int i = 0; i < fireTimeouts.size(); i++)
                {
                    stateMachine.dataAckTimeout(((Byte) fireTimeouts.get(i))
                            .byteValue());
                }
    }

    /**
     * Sets a new value for collector keepalive timeout
     * 
     * @param timeout
     *            new timeout value in seconds
     */
    public void setCollectorKeepaliveTimeout(long timeout)
    {
        collectorKATimeout = timeout * 1000;
    }

    /**
     * Sets a new value for connect timeout
     * 
     * @param timeout
     *            new timeout value in seconds
     */
    public void setConnectTimeout(int timeout)
    {
        connectTimeout = timeout * 1000;
    }

    /**
     * Sets a new value for data acknowledge timeout for a given session
     * 
     * @param sessionId
     *            session id to set the timeout for
     * @param timeout
     *            new timeout value in seconds
     */
    public void setDataAckTimeout(byte sessionId, long timeout)
    {
        synchronized (this)
        {
            Byte id = new Byte(sessionId);
            if (dataAckTimeoutMap.containsKey(id))
                    dataAckTimeoutMap.remove(id);
            dataAckTimeoutMap.put(id, new Long(timeout * 1000));
        }
    }

    /**
     * Sets a new value for exporter keepalive timeout
     * 
     * @param timeout
     *            new timeout value in seconds
     */
    public void setExporterKeepaliveTimeout(int timeout)
    {
        exporterKATimeout = timeout * 1000;
    }
}