package com.xacct.CRANE.collector;

// $Id: CollectorOutputterFactory.java,v 1.4 2005/01/30 15:13:41 ilyad Exp $
// Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

/** An interface for creating new CollectorOutputter objects.
 *
 *  @version $Id: CollectorOutputterFactory.java,v 1.4 2005/01/30 15:13:41 ilyad Exp $
 */
public interface CollectorOutputterFactory
{
    public CollectorOutputter create();
}
