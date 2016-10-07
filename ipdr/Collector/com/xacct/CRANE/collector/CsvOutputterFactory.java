package com.xacct.CRANE.collector;

// $Id: CsvOutputterFactory.java,v 1.4 2005/01/30 15:13:42 ilyad Exp $
// Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.io.PrintStream;

/** Factory for CsvOutputter objects
 *
 *  @version $Id: CsvOutputterFactory.java,v 1.4 2005/01/30 15:13:42 ilyad Exp $
 */
public final class CsvOutputterFactory implements CollectorOutputterFactory
{
    /** $Id: CsvOutputterFactory.java,v 1.4 2005/01/30 15:13:42 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: CsvOutputterFactory.java,v 1.4 2005/01/30 15:13:42 ilyad Exp $"; }

    /** Constructor
     * @param out The output stream (typically a PrintStream [System.out] or
     *            a FileOutputStream, or BufferedOutputStream)
     * @param style The various options ORed together
     * @param sep The separator character
     */
    public CsvOutputterFactory(PrintStream out,
                               CollectorConfig collectorConfig,
                               int style, String sep) {
        this.out             = out;
        this.collectorConfig = collectorConfig;
        this.style           = style;
        this.sep             = sep;
    }

    private PrintStream out;
    private CollectorConfig collectorConfig;
    private String sep;
    private int style;

    public CollectorOutputter create() {
        return new CsvOutputter(out, collectorConfig, style, sep);
    }
}
