package org.ipdr.collector;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.io.PrintStream;
import java.net.Inet4Address;
import java.net.Inet6Address;
import java.net.InetAddress;
import java.util.Date;
import java.util.TreeMap;

import org.ipdr.api.Outputter;
import org.ipdr.api.configuration.CollectorConfigI;
import org.ipdr.api.configuration.FieldConfigI;
import org.ipdr.api.log.LogManager;
import org.ipdr.utility.IPDRUtilities;

/**
 * Counts output records, recording duplicates and missing items. Outputs a
 * summary at the end.
 *  
 */
public final class DedupSummaryOutputter implements Outputter
{
    /**
     * Holds an instance of <code>PrintStream</code>
     */
    private PrintStream out;

    /**
     * Holds Collector configuration
     */
    private CollectorConfigI collectorConfig;

    private long count;

    private InetAddress lastTransmitterAddress;

    private int lastTransmitterPort;

    private long lastSeqNo;

    private boolean closeDone;

    /// Map seqNo -> number of records seen with this seqNo.
    private TreeMap seqNoSeen; // tree map to keep things in sequence

    /**
     * @param out
     *            The output stream (typically a PrintStream [System.out] or a
     *            FileOutputStream, or BufferedOutputStream)
     */
    public DedupSummaryOutputter(PrintStream out)
    {
        this.out = out;
        this.collectorConfig = null;
        this.count = 0;
        this.lastTransmitterAddress = null;
        this.lastTransmitterPort = -1;
        this.lastSeqNo = -1;
        this.closeDone = false;
        this.seqNoSeen = new TreeMap();
        line = new StringBuffer(1000);
    }

    private static final Integer One = new Integer(1);

    private StringBuffer line; // only used for debuggin

    /**
     * @see org.ipdr.api.Outputter#startRecord(java.net.InetAddress, int, java.util.Date, long, int, int, byte[], boolean)
     */
    public final synchronized void startRecord(InetAddress transmitterAddress,
            int transmitterPort,
            //		String transmitterVendorString,
            Date transmitterBootTime, long seqNo, int configId, int sessionId, int templateId, byte[] documentId, boolean isDuplicateRecord)
    {

        this.count++;
        this.lastTransmitterAddress = transmitterAddress;
        this.lastTransmitterPort = transmitterPort;
        this.lastSeqNo = seqNo;
        Integer sn = new Integer((int) seqNo);
        Integer count = (Integer) this.seqNoSeen.get(sn);
        count = (count == null) ? One : new Integer(count.intValue() + 1);
        this.seqNoSeen.put(sn, count);

        this.line.setLength(0);
        this.line.append("[dedup] ").append(transmitterAddress).append(":")
                .append(transmitterPort).append(" ").append("boot=").append(
                        IPDRUtilities.dateFormat(transmitterBootTime)).append(
                        " ").append("#").append(sn).append(" ").append("sess=")
                .append(sessionId).append(" ").append("tmpl=").append(
                        templateId).append(" ").append("count=").append(count);
    }

    /**
     * @see org.ipdr.api.Outputter#endRecord()
     */
    public final void endRecord()
    {
        LogManager.info(this, "endRecord", "DeDup - end Record");
    }

    /**
     * @see org.ipdr.api.Outputter#close()
     */
    public final synchronized void close()
    {
        synchronized (this.out)
        {
            this.out.flush();
            if (this.count > 0)
            {
                if (lastTransmitterAddress == null)
                {
                    LogManager.info(this, "close","Closing dedup output count=" + this.count
                            + " dedup count=" + this.seqNoSeen.size()
                            + " last seq#=" + this.lastSeqNo);
                } else
                {
                    LogManager.info(this, "close", "Closing dedup output count=" + this.count
                            + " dedup count=" + this.seqNoSeen.size()
                            + " last seq#=" + this.lastSeqNo + " transmitter: "
                            + lastTransmitterAddress.toString() + ":"
                            + lastTransmitterPort);
                }
                // TreeMap's keySet's iterator is ordered.
                int lastKey = ((Integer) this.seqNoSeen.lastKey()).intValue();
                for (int i = ((Integer) this.seqNoSeen.firstKey()).intValue(); i < lastKey; i++)
                {
                    Integer sn = new Integer(i);
                    Integer count = (Integer) this.seqNoSeen.get(sn);
                    if (count == null)
                    {
                        LogManager.info(this, "close","   seq# " + sn + " *** missing");
                    } else if (count.intValue() != 1)
                    {
                        LogManager.info(this, "close","   seq# " + sn + " output " + count
                                + " times");
                    }
                }
            } else
            {
                LogManager.info(this, "close","Closing dedup output count=" + this.count);
            }
            this.out.flush();
        }
    }

    /**
     * @see java.lang.Object#finalize()
     */
    protected void finalize() throws Throwable
    {
        try
        {
            if (!closeDone)
            {
                close();
            }
        } finally
        {
            super.finalize();
        }
    }

    /**
     * @see org.ipdr.api.Outputter#outputBoolean(int, org.ipdr.api.configuration.FieldConfigI, byte)
     */
    public final void outputBoolean(int keyI, FieldConfigI field, byte value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputUnsignedByte(int, org.ipdr.api.configuration.FieldConfigI, short)
     */
    public final void outputUnsignedByte(int keyI, FieldConfigI field,
            short value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputByte(int, org.ipdr.api.configuration.FieldConfigI, byte)
     */
    public final void outputByte(int keyI, FieldConfigI field, byte value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputUnsignedShort(int, org.ipdr.api.configuration.FieldConfigI, int)
     */
    public final void outputUnsignedShort(int keyI, FieldConfigI field,
            int value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputShort(int, org.ipdr.api.configuration.FieldConfigI, short)
     */
    public final void outputShort(int keyI, FieldConfigI field, short value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputUnsignedInt(int, org.ipdr.api.configuration.FieldConfigI, long)
     */
    public final void outputUnsignedInt(int keyI, FieldConfigI field, long value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputInt(int, org.ipdr.api.configuration.FieldConfigI, int)
     */
    public final void outputInt(int keyI, FieldConfigI field, int value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputUnsignedLong(int, org.ipdr.api.configuration.FieldConfigI, long)
     */
    public final void outputUnsignedLong(int keyI, FieldConfigI field,
            long value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputLong(int, org.ipdr.api.configuration.FieldConfigI, long)
     */
    public final void outputLong(int keyI, FieldConfigI field, long value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputFloat(int, org.ipdr.api.configuration.FieldConfigI, float)
     */
    public final void outputFloat(int keyI, FieldConfigI field, float value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputDouble(int, org.ipdr.api.configuration.FieldConfigI, double)
     */
    public final void outputDouble(int keyI, FieldConfigI field, double value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputString(int, org.ipdr.api.configuration.FieldConfigI, java.lang.String)
     */
    public final void outputString(int keyI, FieldConfigI field, String str)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputDateTimeMsec(int, org.ipdr.api.configuration.FieldConfigI, long)
     */
    public final void outputDateTimeMsec(int keyI, FieldConfigI field,
            long value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputDateTimeUsec(int, org.ipdr.api.configuration.FieldConfigI, long)
     */
    public final void outputDateTimeUsec(int keyI, FieldConfigI field,
            long value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputIPv4Addr(int, org.ipdr.api.configuration.FieldConfigI, java.net.Inet4Address)
     */
    public void outputIPv4Addr(int keyI, FieldConfigI field, Inet4Address value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputIPv6Addr(int, org.ipdr.api.configuration.FieldConfigI, java.net.Inet6Address)
     */
    public void outputIPv6Addr(int keyI, FieldConfigI field, Inet6Address value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputDateTimeSec(int, org.ipdr.api.configuration.FieldConfigI, int)
     */
    public void outputDateTimeSec(int keyI, FieldConfigI field, int value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputUUID(int, org.ipdr.api.configuration.FieldConfigI, byte[])
     */
    public void outputUUID(int keyI, FieldConfigI field, byte[] value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputMACAddress(int, org.ipdr.api.configuration.FieldConfigI, long)
     */
    public void outputMACAddress(int keyI, FieldConfigI field, long value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputBase64Binary(int, org.ipdr.api.configuration.FieldConfigI, int, byte[])
     */
    public void outputBase64Binary(int keyI, FieldConfigI field, int length,
            byte[] value)
    {
    }

    /**
     * @see org.ipdr.api.Outputter#outputHexBinary(int, org.ipdr.api.configuration.FieldConfigI, int, byte[])
     */
    public void outputHexBinary(int keyI, FieldConfigI field, int length,
            byte[] value)
    {
    }

}