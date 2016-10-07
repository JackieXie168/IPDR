package com.xacct.CRANE.collector;

// $Id: NullOutputter.java,v 1.4 2005/01/30 15:13:40 ilyad Exp $
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.io.PrintStream;
import java.util.Date;

/** Outputs records to the "bit bucket", typically used for
 *  performance testing.
 *
 *  @version $Id: NullOutputter.java,v 1.4 2005/01/30 15:13:40 ilyad Exp $
 */
public final class NullOutputter implements CollectorOutputter
{
    /** $Id: NullOutputter.java,v 1.4 2005/01/30 15:13:40 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: NullOutputter.java,v 1.4 2005/01/30 15:13:40 ilyad Exp $"; }

    private PrintStream out;

    private boolean readyToAck;
    private long    seqNoToAck;

    private long                count;
    private ExtendedInetAddress lastTransmitterAddress;
    private int                 lastTransmitterPort;
    private long                lastSeqNo;
    private boolean             closeDone;

    public final void commitTimerExpired() { }

    public final synchronized long    getLastSeqNoToAck()      { return seqNoToAck; }
    public final synchronized boolean isValidLastSeqNoToAck()  { return readyToAck; }

    public final void registerProcessDataSeqNoAck(ProcessDataSeqNoAck processDataSeqNoAck) { }

    public NullOutputter(PrintStream out) {
        this.out         = out;
        this.readyToAck  = false;
        this.seqNoToAck  = 0; // meaningless until this.readyToAck is true
        this.count                  = 0;
        this.lastTransmitterAddress = null;
        this.lastTransmitterPort    = -1;
        this.lastSeqNo              = -1;
        this.closeDone              = false;
    }

    public synchronized void startRecord(ExtendedInetAddress transmitterAddress,
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
    }

    public final void endRecord() {
        this.readyToAck = true;
    }

    public final void close() {
        if (this.count > 0) {
            if (lastTransmitterAddress == null) {
                this.out.println("Closing null output count=" + this.count +
                                 " last seq#=" + this.lastSeqNo);
            } else {
                this.out.println("Closing null output count=" + this.count +
                                 " last seq#=" + this.lastSeqNo +
                                 " transmitter: " +
                                 lastTransmitterAddress.andPortToString(lastTransmitterPort));
            }
        } else {
            this.out.println("Closing null output count=" + this.count);
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
