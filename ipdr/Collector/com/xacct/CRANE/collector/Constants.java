package com.xacct.CRANE.collector;

// $Id: Constants.java,v 1.4 2005/01/30 15:13:41 ilyad Exp $
// Copyright (2005) (c) Amdocs, All Rights Reserved
// This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
// ANY KIND either expresses or implied.

/** Various default values.
 *
 *  @version $Id: Constants.java,v 1.4 2005/01/30 15:13:41 ilyad Exp $
 */
public class Constants
{
    /** $Id: Constants.java,v 1.4 2005/01/30 15:13:41 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: Constants.java,v 1.4 2005/01/30 15:13:41 ilyad Exp $"; }

    // defaults:
    public static final String DEFAULT_IPADDRESS = ""; // should *not* be 127.0.0.1
    public static final String DEFAULT_CONFIG_VERSION = "1";

    public static final int     DEFAULT_CLIENT_VERSION_PORT = 15000;
    public static final int     DEFAULT_SERVER_PORT = 15001;
    public static final int     DEFAULT_SERVER_ACK_INTERVAL = 1000;
    public static final int     DEFAULT_SERVER_START_INTERVAL = 10;
    public static final boolean DEFAULT_IS_DEBUG_LEVEL_USED = true;
    public static final int     DEFAULT_ACK_INTERVAL_EPSILON = 10;
    public static final int     DEFAULT_NO_DATA_KEEP_ALIVE_INTERVAL = 30 * 60 * 1000;

    public static final String  DEFAULT_LOG_IPADDRESS = ""; // should *not* be 127.0.0.1
    public static final boolean DEFAULT_TEMPLATE_STATUS_RESPONSE = false;

    /// @see ConnectionConfig#connectTimeout
    public static final int DEFAULT_CONNECT_TIMEOUT = 5000;

    /// @see ConnectionConfig#connectRetryTimeout
    public static final int DEFAULT_RETRY_TIMEOUT = 2000;

    /// @see ConnectionConfig#connectReconnectInterval
    public static final int DEFAULT_RECONNECT_INTERVAL = 10000;

    /// @see ConnectionConfig#connectRoundtripTimeout
    public static final int DEFAULT_CONNECT_ROUNDTRIP_TIMEOUT = 500;

    /// @see ConnectionConfig.dataAckLogInterval
    public static final int DEFAULT_DATA_ACK_LOG_INTERVAL = 1 * 60 * 1000; // 1 minute: time is in milliseconds
}
