package com.xacct.CRANE.collector;

// $Id: DedupSummaryOutputterFactory.java,v 1.4 2005/01/30 15:13:41 ilyad Exp $
// Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.io.PrintStream;

/** Factory for DedupOutputter objects
 *
 *  @version $Id: DedupSummaryOutputterFactory.java,v 1.4 2005/01/30 15:13:41 ilyad Exp $
 */
public final class DedupSummaryOutputterFactory implements CollectorOutputterFactory
{
    /** $Id: DedupSummaryOutputterFactory.java,v 1.4 2005/01/30 15:13:41 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: DedupSummaryOutputterFactory.java,v 1.4 2005/01/30 15:13:41 ilyad Exp $"; }

    public DedupSummaryOutputterFactory(PrintStream out) {
        this.out = out;
    }

    private PrintStream out;

    public CollectorOutputter create() {
        return new DedupSummaryOutputter(out);
    }
}
