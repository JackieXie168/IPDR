package com.xacct.CRANE.collector;

// $Id: DedupSummaryOutputter.java,v 1.4 2005/01/30 15:13:40 ilyad Exp $
// Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.io.PrintStream;
import java.util.Date;
import java.util.TreeMap;

/** Counts output records, recording duplicates and missing items.
 *  Outputs a summary at the end.
 *
 *  @version $Id: DedupSummaryOutputter.java,v 1.4 2005/01/30 15:13:40 ilyad Exp $
 */
public final class DedupSummaryOutputter implements CollectorOutputter
{
    /** $Id: DedupSummaryOutputter.java,v 1.4 2005/01/30 15:13:40 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: DedupSummaryOutputter.java,v 1.4 2005/01/30 15:13:40 ilyad Exp $"; }

    private PrintStream out;
    private CollectorConfig collectorConfig;

    private boolean readyToAck;
    private long    seqNoToAck;

    private long                count;
    private ExtendedInetAddress lastTransmitterAddress;
    private int                 lastTransmitterPort;
    private long                lastSeqNo;
    private boolean             closeDone;

    /// Map seqNo -> number of records seen with this seqNo.
    /// XXX Should be seqNo + ":" + bootTime
    private TreeMap seqNoSeen; // tree map to keep things in sequence

    public final void commitTimerExpired() { }

    public final synchronized long    getLastSeqNoToAck()      { return seqNoToAck; }
    public final synchronized boolean isValidLastSeqNoToAck()  { return readyToAck; }

    public final void registerProcessDataSeqNoAck(ProcessDataSeqNoAck processDataSeqNoAck) { }

    /**
     * @param out The output stream (typically a PrintStream [System.out] or
     *            a FileOutputStream, or BufferedOutputStream)
     */
    public DedupSummaryOutputter(PrintStream out) {
        this.out             = out;
        this.collectorConfig = collectorConfig;
        this.readyToAck      = false;
        this.seqNoToAck      = 0; // meaningless until this.readyToAck is true
        this.count                  = 0;
        this.lastTransmitterAddress = null;
        this.lastTransmitterPort    = -1;
        this.lastSeqNo              = -1;
        this.closeDone              = false;
        this.seqNoSeen              = new TreeMap();
        line                        = new StringBuffer(1000);
    }

    private static final Integer One = new Integer(1);

    private StringBuffer line; // only used for debuggin

    public final synchronized void startRecord(ExtendedInetAddress transmitterAddress,
                                               int transmitterPort,
                                               String transmitterVendorString,
                                               Date transmitterBootTime,
                                               long seqNo,
                                               int sessionId,
                                               int templateId) {
        this.seqNoToAck = seqNo;

        this.count++;
        this.lastTransmitterAddress = transmitterAddress;
        this.lastTransmitterPort = transmitterPort;
        this.lastSeqNo = seqNo;
        Integer sn = new Integer((int)seqNo);
        Integer count = (Integer)this.seqNoSeen.get(sn);
        count = (count == null) ? One : new Integer(count.intValue() + 1);
        this.seqNoSeen.put(sn, count);

        this.line.setLength(0);
        this.line.append("[dedup] ").
            append(transmitterAddress).append(":").append(transmitterPort).
            append(" ").append("boot=").append(CollectorUtil.dateFormat(transmitterBootTime)).
            append(" ").append("#").append(sn).
            append(" ").append("sess=").append(sessionId).
            append(" ").append("tmpl=").append(templateId).
            append(" ").append("count=").append(count);
    }

    public final void endRecord() {
        System.out.println("in endRecord " + line);
        this.readyToAck = true;
        if (false) { // for debugging
            synchronized(this.out) {
                this.out.println(this.line.toString());
                this.out.flush();
            }
        }
    }

    public final synchronized void close() {
        synchronized(this.out) {
            this.out.println("");
            this.out.flush();
            if (this.count > 0) {
                if (lastTransmitterAddress == null) {
                    this.out.println("Closing dedup output count=" + this.count +
                                     " dedup count=" + this.seqNoSeen.size() +
                                     " last seq#=" + this.lastSeqNo);
                } else {
                    this.out.println("Closing dedup output count=" + this.count +
                                     " dedup count=" + this.seqNoSeen.size() +
                                     " last seq#=" + this.lastSeqNo +
                                     " transmitter: " +
                                     lastTransmitterAddress.andPortToString(lastTransmitterPort));
                }
                // TreeMap's keySet's iterator is ordered.
                int lastKey = ((Integer)this.seqNoSeen.lastKey()).intValue();
                for (int i = ((Integer)this.seqNoSeen.firstKey()).intValue(); i < lastKey; i++) {
                    Integer sn = new Integer(i);
                    Integer count = (Integer)this.seqNoSeen.get(sn);
                    if (count == null) {
                        this.out.println("   seq# " + sn + " *** missing");
                    } else if (count.intValue() != 1) {
                        this.out.println("   seq# " + sn + " output " + count + " times");
                    }
                }
            } else {
                this.out.println("Closing dedup output count=" + this.count);
            }
            this.out.flush();
        }
    }

    protected void finalize() throws Throwable {
        try {
            if (! closeDone) {
                close();
            }
        } finally {
            super.finalize();
        }
    }

    public final void outputBoolean(int keyI, FieldConfig field, FieldDescriptor fieldDesc, byte value) { }

    public final void outputUnsignedInt8(int keyI, FieldConfig field, FieldDescriptor fieldDesc, short value) { }
    public final void outputInt8(int keyI, FieldConfig field, FieldDescriptor fieldDesc, byte value) { }

    public final void outputUnsignedInt16(int keyI, FieldConfig field, FieldDescriptor fieldDesc, int value) { }
    public final void outputInt16(int keyI, FieldConfig field, FieldDescriptor fieldDesc, short value) { }

    public final void outputUnsignedInt32(int keyI, FieldConfig field, FieldDescriptor fieldDesc, long value) { }
    public final void outputInt32(int keyI, FieldConfig field, FieldDescriptor fieldDesc, int value) { }

    public final void outputUnsignedInt64(int keyI, FieldConfig field, FieldDescriptor fieldDesc, long value) { }
    public final void outputInt64(int keyI, FieldConfig field, FieldDescriptor fieldDesc, long value) { }

    public final void outputFloat(int keyI, FieldConfig field, FieldDescriptor fieldDesc, float value) { }
    public final void outputDouble(int keyI, FieldConfig field, FieldDescriptor fieldDesc, double value) { }

    public final void outputString(int keyI, FieldConfig field, FieldDescriptor fieldDesc, byte [] buf, int offset, int len) { }
    public final void outputString(int keyI, FieldConfig field, FieldDescriptor fieldDesc, String str) { }

    public final void outputIPv4Addr(int keyI, FieldConfig field, FieldDescriptor fieldDesc, int value) { }

    public final void outputTimeSec(int keyI, FieldConfig field, FieldDescriptor fieldDesc, long value) { }
    public final void outputTimeMillisec(int keyI, FieldConfig field, FieldDescriptor fieldDesc, long value) { }
    public final void outputTimeMicrosec(int keyI, FieldConfig field, FieldDescriptor fieldDesc, long value) { }

//    public final void outputUtf8() {
//    public final void outputUtf16() {
//    public final void outputBlob() {
//    public final void outputIpv6_ADDR() {
}
