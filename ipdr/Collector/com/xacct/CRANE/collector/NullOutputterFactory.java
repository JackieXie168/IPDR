package com.xacct.CRANE.collector;

// $Id: NullOutputterFactory.java,v 1.5 2005/04/08 12:48:10 guyg Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.io.PrintStream;

/** Factory for NullOutputter objects
 *
 *  @version $Id: NullOutputterFactory.java,v 1.5 2005/04/08 12:48:10 guyg Exp $
 */
public final class NullOutputterFactory implements CollectorOutputterFactory
{
    /** $Id: NullOutputterFactory.java,v 1.5 2005/04/08 12:48:10 guyg Exp $ */
    public static String versionStr() { return "@(#) $Id: NullOutputterFactory.java,v 1.5 2005/04/08 12:48:10 guyg Exp $"; }

    public NullOutputterFactory(PrintStream out) {
        this.out = out;
    }

    private PrintStream out;

    public CollectorOutputter create() {
        return new NullOutputter(out);
    }
}
