package com.xacct.CRANE.collector;

// $Id: CsvOutputter.java,v 1.4 2005/01/30 15:13:42 ilyad Exp $
// Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.io.PrintStream;
import java.util.Date;

/** Outputs a record in CSV form.
 *  Also runs dedup, for a nice summary.
 *
 *  @version $Id: CsvOutputter.java,v 1.4 2005/01/30 15:13:42 ilyad Exp $
 */
public final class CsvOutputter implements CollectorOutputter
{
    /** $Id: CsvOutputter.java,v 1.4 2005/01/30 15:13:42 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: CsvOutputter.java,v 1.4 2005/01/30 15:13:42 ilyad Exp $"; }

    DedupSummaryOutputter dedupOutputter;

    private PrintStream out;
    private CollectorConfig collectorConfig;

    private String sep;

    private int style;

    public static int styleNone        = 0x0000;
    public static int styleHeader      = 0x0001;
    public static int styleNameEqValue = 0x0002;

    private long    lineSeqNo; // set by startRecord, used by endRecord
    private boolean readyToAck;
    private long    seqNoToAck;

    private long                count;
    private ExtendedInetAddress lastTransmitterAddress;
    private int                 lastTransmitterPort;
    private long                lastSeqNo;
    private boolean             closeDone;

    private StringBuffer line;

    public final void commitTimerExpired() { }

    public final synchronized long    getLastSeqNoToAck()      { return seqNoToAck; }
    public final synchronized boolean isValidLastSeqNoToAck()  { return readyToAck; }

    public final void registerProcessDataSeqNoAck(ProcessDataSeqNoAck processDataSeqNoAck) { }

    String sepWithinLine;

    /** Constructor
     * @param out The output stream (typically a PrintStream [System.out] or
     *            a FileOutputStream, or BufferedOutputStream)
     * @param style The various options ORed together
     * @param sep The separator charcter
     */
    public CsvOutputter(PrintStream out,
                        CollectorConfig collectorConfig,
                        int style, String sep) {
        this.out             = out;
        this.collectorConfig = collectorConfig;
        this.style           = style;
        this.sep             = sep;
        this.readyToAck      = false;
        this.lineSeqNo       = 0; // meaningless until this.readyToAck is true
        this.seqNoToAck      = 0; // meaningless until this.readyToAck is true
        this.sepWithinLine   = ": ";
        this.line            = new StringBuffer(1000);
        this.count           = 0;
        this.lastTransmitterAddress = null;
        this.lastTransmitterPort    = -1;
        this.lastSeqNo              = -1;
        this.closeDone              = false;

        this.dedupOutputter  = new DedupSummaryOutputter(out);
    }

    public final synchronized void startRecord(ExtendedInetAddress transmitterAddress,
                                               int transmitterPort,
                                               String transmitterVendorString,
                                               Date transmitterBootTime,
                                               long seqNo,
                                               int sessionId,
                                               int templateId) {
        this.line.setLength(0);
        this.line.append("[csv] ").
            append(transmitterAddress).append(":").append(transmitterPort).
            append(sep).append("boot=").append(CollectorUtil.dateFormat(transmitterBootTime)).
            append(sep).append("#").append(seqNo).
            append(sep).append("sess=").append(sessionId).
            append(sep).append("tmpl=").append(templateId);

        this.sepWithinLine = ": ";
        this.lineSeqNo = seqNo;

        this.count++;
        this.lastTransmitterAddress = transmitterAddress;
        this.lastTransmitterPort = transmitterPort;
        this.lastSeqNo = seqNo;

        this.dedupOutputter.startRecord(transmitterAddress, transmitterPort,
                                        transmitterVendorString, transmitterBootTime,
                                        seqNo, sessionId, templateId);
    }

    public final void endRecord() {
        this.out.println(this.line.toString());
        this.out.flush();
        this.seqNoToAck = lineSeqNo;
        this.readyToAck = true;
        this.dedupOutputter.endRecord();
    }

    public final synchronized void close() {
        if (this.count > 0) {
            if (lastTransmitterAddress == null) {
                this.out.println("Closing Csv output count=" + this.count +
                                 " last seq#=" + this.lastSeqNo);
            } else {
                this.out.println("Closing Csv output count=" + this.count +
                                 " last seq#=" + this.lastSeqNo +
                                 " transmitter: " +
                                 lastTransmitterAddress.andPortToString(lastTransmitterPort));
            }
        } else {
            this.out.println("Closing Csv output count=" + this.count);
        }
        this.dedupOutputter.close();
    }

    protected void finalize() throws Throwable {
        try {
            if (! closeDone) {
                close();
            }
        } finally {
            super.finalize();
        }

        this.dedupOutputter.finalize();
    }

    private final void outputSep(int keyI, FieldConfig field, FieldDescriptor fieldDesc) {
        this.line.append(this.sepWithinLine).append(field.getLabel()).
            // append("/").append(fieldDesc.getFQN()).
            append("{");
        try {
            this.line.append(fieldDesc.getPrimitiveID());
        } catch (InvalidFieldDescriptorException ex) {
            this.line.append(ex.toString());
        }
        this.line.append("}=");
        this.sepWithinLine = this.sep;
    }

    public final void outputBoolean(int keyI, FieldConfig field, FieldDescriptor fieldDesc, byte value) {
        outputSep(keyI, field, fieldDesc);
        this.line.append(value);
        this.dedupOutputter.outputBoolean(keyI, field, fieldDesc, value);
    }

    public final void outputUnsignedInt8(int keyI, FieldConfig field, FieldDescriptor fieldDesc, short value) {
        outputSep(keyI, field, fieldDesc);
        this.line.append(value);
        this.dedupOutputter.outputUnsignedInt8(keyI, field, fieldDesc, value);
    }

    public final void outputInt8(int keyI, FieldConfig field, FieldDescriptor fieldDesc, byte value) {
        outputSep(keyI, field, fieldDesc);
        this.line.append(value);
        this.dedupOutputter.outputInt8(keyI, field, fieldDesc, value);
    }

    public final void outputUnsignedInt16(int keyI, FieldConfig field, FieldDescriptor fieldDesc, int value) {
        outputSep(keyI, field, fieldDesc);
        this.line.append(value);
        this.dedupOutputter.outputUnsignedInt16(keyI, field, fieldDesc, value);
    }

    public final void outputInt16(int keyI, FieldConfig field, FieldDescriptor fieldDesc, short value) {
        outputSep(keyI, field, fieldDesc);
        this.line.append(value);
        this.dedupOutputter.outputInt16(keyI, field, fieldDesc, value);
    }

    public final void outputUnsignedInt32(int keyI, FieldConfig field, FieldDescriptor fieldDesc, long value) {
        outputSep(keyI, field, fieldDesc);
        this.line.append(value);
        this.dedupOutputter.outputUnsignedInt32(keyI, field, fieldDesc, value);
    }

    public final void outputInt32(int keyI, FieldConfig field, FieldDescriptor fieldDesc, int value) {
        outputSep(keyI, field, fieldDesc);
        this.line.append(value);
        this.dedupOutputter.outputInt32(keyI, field, fieldDesc, value);
    }

    public final void outputUnsignedInt64(int keyI, FieldConfig field, FieldDescriptor fieldDesc, long value) {
        // XXX Unsigned long doesn't exist in Java
        outputSep(keyI, field, fieldDesc);
        this.line.append(value);
        this.dedupOutputter.outputUnsignedInt64(keyI, field, fieldDesc, value);
    }

    public final void outputInt64(int keyI, FieldConfig field, FieldDescriptor fieldDesc, long value) {
        outputSep(keyI, field, fieldDesc);
        this.line.append(value);
        this.dedupOutputter.outputInt64(keyI, field, fieldDesc, value);
    }

    public final void outputFloat(int keyI, FieldConfig field, FieldDescriptor fieldDesc, float value) {
        outputSep(keyI, field, fieldDesc);
        this.line.append(value);
        this.dedupOutputter.outputFloat(keyI, field, fieldDesc, value);
    }

    public final void outputDouble(int keyI, FieldConfig field, FieldDescriptor fieldDesc, double value) {
        outputSep(keyI, field, fieldDesc);
        this.line.append(value);
        this.dedupOutputter.outputDouble(keyI, field, fieldDesc, value);
    }

    public final void outputString(int keyI, FieldConfig field, FieldDescriptor fieldDesc, byte [] buf, int offset, int len) {
        outputSep(keyI, field, fieldDesc);
        this.line.append("'");
        // why isn't Stringbuf.append(byte[],int,int) defined? it is for String ...
        for (int i = 0; i < len; i++) {
            this.line.append((char)buf[offset + i]);
        }
        this.line.append("'");
        this.dedupOutputter.outputString(keyI, field, fieldDesc, buf, offset, len);
    }

    public final void outputString(int keyI, FieldConfig field, FieldDescriptor fieldDesc, String str) {
        outputSep(keyI, field, fieldDesc);
        this.line.append("'").append(str).append("'");
        this.dedupOutputter.outputString(keyI, field, fieldDesc, str);
    }

    public final void outputIPv4Addr(int keyI, FieldConfig field, FieldDescriptor fieldDesc, int value) {
        String str = "" + (value >> 24) + "." +
            (value >> 16 & 0xff) + "." +
            (value >> 8 & 0xff) + "." +
            (value & 0xff);
        outputSep(keyI, field, fieldDesc);
        this.line.append("IP_V4:").append(str);
        this.dedupOutputter.outputIPv4Addr(keyI, field, fieldDesc, value);
    }

    public final void outputTimeSec(int keyI, FieldConfig field, FieldDescriptor fieldDesc, long value) {
        outputSep(keyI, field, fieldDesc);
        this.line.append(CollectorUtil.dateFormatSeconds(value));
        this.dedupOutputter.outputTimeSec(keyI, field, fieldDesc, value);
    }

    public final void outputTimeMillisec(int keyI, FieldConfig field, FieldDescriptor fieldDesc, long value) {
        outputSep(keyI, field, fieldDesc);
        this.line.append(CollectorUtil.dateFormatMillisec(value));
        this.dedupOutputter.outputTimeMillisec(keyI, field, fieldDesc, value);
    }

    public final void outputTimeMicrosec(int keyI, FieldConfig field, FieldDescriptor fieldDesc, long value) {
        outputSep(keyI, field, fieldDesc);
        this.line.append(CollectorUtil.dateFormatMicrosec(value));
        this.dedupOutputter.outputTimeMicrosec(keyI, field, fieldDesc, value);
    }

//    public final void outputUtf8() {
//    public final void outputUtf16() {
//    public final void outputBlob() {
//    public final void outputIpv6_ADDR() {
}
