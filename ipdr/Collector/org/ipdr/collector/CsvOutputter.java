package org.ipdr.collector;

//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.io.PrintStream;
import java.math.BigInteger;
import java.net.Inet4Address;
import java.net.Inet6Address;
import java.net.InetAddress;
import java.util.Date;

import org.ipdr.api.Outputter;
import org.ipdr.api.configuration.FieldConfigI;
import org.ipdr.api.log.LogManager;
import org.ipdr.protocol.ProtocolDataBigEndian;
import org.ipdr.utility.IPDRUtilities;

/**
 * Outputs a record in CSV form. Also runs dedup, for a nice summary.
 *  
 */
public final class CsvOutputter implements Outputter
{
    public static int styleHeader = 0x0001;

    public static int styleNameEqValue = 0x0002;

    public static int styleNone = 0x0000;

    private boolean closeDone;

    private long count;

    DedupSummaryOutputter dedupOutputter;

    private long lastSeqNo;

    private InetAddress lastTransmitterAddress;

    private int lastTransmitterPort;

    private StringBuffer line;

    private PrintStream out;
    
    ProtocolDataBigEndian be = new ProtocolDataBigEndian();
    // by endRecord
    private String sep = ",";

    String sepWithinLine;

    /**
     * Constructor
     * 
     * @param out
     *            The output stream (typically a PrintStream [System.out] or a
     *            FileOutputStream, or BufferedOutputStream)
     * @param sep
     *            The separator charcter
     */
    public CsvOutputter(PrintStream out, String sep)
    {
        this.out = out;
        this.sep = sep;
        this.sepWithinLine = ": ";
        this.line = new StringBuffer(1000);
        this.count = 0;
        this.lastTransmitterAddress = null;
        this.lastTransmitterPort = -1;
        this.lastSeqNo = -1;
        this.closeDone = false;

        this.dedupOutputter = new DedupSummaryOutputter(out);
    }

    /**
     * @see org.ipdr.api.Outputter#close()
     */
    public final synchronized void close()
    {
        if (this.count > 0)
        {
            if (lastTransmitterAddress == null)
            {
                LogManager.info(this, "close", "Closing Csv output count=" + this.count
                        + " last seq#=" + this.lastSeqNo);
            }
            else
            {
                LogManager.info(this, "close","Closing Csv output count=" + this.count
                        + " last seq#=" + this.lastSeqNo + " transmitter: "
                        + lastTransmitterAddress.toString() + ":"
                        + lastTransmitterPort);
            }
        }
        else
        {
            LogManager.info(this, "close", "Closing Csv output count=" + this.count);
        }
        this.dedupOutputter.close();
    }

    public final void endRecord()
    {
        this.out.println(this.line.toString());
        this.out.flush();
        this.dedupOutputter.endRecord();
    }

    protected void finalize() throws Throwable
    {
        try
        {
            if (!closeDone)
            {
                close();
            }
        }
        finally
        {
            super.finalize();
        }

        this.dedupOutputter.finalize();
    }

    /**
     * @see org.ipdr.api.Outputter#outputBase64Binary(int,
     *      org.ipdr.api.configuration.FieldConfigI, int, byte[])
     */
    public void outputBase64Binary(int keyI, FieldConfigI field, int length,
            byte[] value)
    {
        outputHexBinary(keyI, field, length, value);
    }

    /**
     * @see org.ipdr.api.Outputter#outputBoolean(int,
     *      org.ipdr.api.configuration.FieldConfigI, byte)
     */
    public final void outputBoolean(int keyI, FieldConfigI field, byte value)
    {
        outputSep(keyI, field);
        this.line.append(value);
        this.dedupOutputter.outputBoolean(keyI, field, value);
    }

    /**
     * @see org.ipdr.api.Outputter#outputByte(int,
     *      org.ipdr.api.configuration.FieldConfigI, byte)
     */
    public final void outputByte(int keyI, FieldConfigI field, byte value)
    {
        outputSep(keyI, field);
        this.line.append(value);
        this.dedupOutputter.outputByte(keyI, field, value);
    }

    /**
     * @see org.ipdr.api.Outputter#outputDateTimeMsec(int,
     *      org.ipdr.api.configuration.FieldConfigI, long)
     */
    public final void outputDateTimeMsec(int keyI, FieldConfigI field,
            long value)
    {
        outputSep(keyI, field);
        this.line.append(IPDRUtilities.dateFormatMillisec(value));
        this.dedupOutputter.outputDateTimeMsec(keyI, field, value);
    }

    /**
     * @see org.ipdr.api.Outputter#outputDateTimeSec(int,
     *      org.ipdr.api.configuration.FieldConfigI, int)
     */
    public void outputDateTimeSec(int keyI, FieldConfigI field, int value)
    {
        outputSep(keyI, field);
        this.line.append(IPDRUtilities.dateFormatSeconds(value));
        this.dedupOutputter.outputDateTimeSec(keyI, field, value);
    }

    /**
     * @see org.ipdr.api.Outputter#outputDateTimeUsec(int,
     *      org.ipdr.api.configuration.FieldConfigI, long)
     */
    public final void outputDateTimeUsec(int keyI, FieldConfigI field,
            long value)
    {
        outputSep(keyI, field);
        this.line.append(IPDRUtilities.dateFormatMicrosec(value));
        this.dedupOutputter.outputDateTimeUsec(keyI, field, value);
    }

    /**
     * @see org.ipdr.api.Outputter#outputDouble(int,
     *      org.ipdr.api.configuration.FieldConfigI, double)
     */
    public final void outputDouble(int keyI, FieldConfigI field, double value)
    {
        outputSep(keyI, field);
        this.line.append(value);
        this.dedupOutputter.outputDouble(keyI, field, value);
    }

    /**
     * @see org.ipdr.api.Outputter#outputFloat(int,
     *      org.ipdr.api.configuration.FieldConfigI, float)
     */
    public final void outputFloat(int keyI, FieldConfigI field, float value)
    {
        outputSep(keyI, field);
        this.line.append(value);
        this.dedupOutputter.outputFloat(keyI, field, value);
    }

    /**
     * @see org.ipdr.api.Outputter#outputHexBinary(int,
     *      org.ipdr.api.configuration.FieldConfigI, int, byte[])
     */
    public void outputHexBinary(int keyI, FieldConfigI field, int length,
            byte[] value)
    {
        outputSep(keyI, field);
        this.line.append("HexBinary: "); // Shaig & Alexkis
        for (int i = 0; i < length; i++)
        {
            this.line.append(Integer.toHexString(value[i]));
        }
    }

    /**
     * @see org.ipdr.api.Outputter#outputInt(int,
     *      org.ipdr.api.configuration.FieldConfigI, int)
     */
    public final void outputInt(int keyI, FieldConfigI field, int value)
    {
        outputSep(keyI, field);
        this.line.append(value);
        this.dedupOutputter.outputInt(keyI, field, value);
    }

    /**
     * @see org.ipdr.api.Outputter#outputIPv4Addr(int,
     *      org.ipdr.api.configuration.FieldConfigI, java.net.Inet4Address)
     */
    /*
     * (non-Javadoc)
     * 
     * @see org.ipdr.api.Outputter#outputIPv4Addr(int,
     *      org.ipdr.api.configuration.FieldConfigI, java.net.Inet4Address)
     */
    public final void outputIPv4Addr(int keyI, FieldConfigI field,
            Inet4Address value)
    {
        outputSep(keyI, field);
        // Changed from value.getAddress() to value.getHostAddress()(shaig)
        this.line.append("IP_V4:").append(value.getHostAddress());
        this.dedupOutputter.outputIPv4Addr(keyI, field, value);

    }

    /**
     * @see org.ipdr.api.Outputter#outputIPv6Addr(int,
     *      org.ipdr.api.configuration.FieldConfigI, java.net.Inet6Address)
     */
    public void outputIPv6Addr(int keyI, FieldConfigI field, Inet6Address value)
    {
        //The whole function code added (shaig)
        outputSep(keyI, field);
        this.line.append("IP_V6:").append(value.getHostAddress());
        this.dedupOutputter.outputIPv6Addr(keyI, field, value);

    }

    /**
     * @see org.ipdr.api.Outputter#outputLong(int,
     *      org.ipdr.api.configuration.FieldConfigI, long)
     */
    public final void outputLong(int keyI, FieldConfigI field, long value)
    {
        outputSep(keyI, field);
        this.line.append(value);
        this.dedupOutputter.outputLong(keyI, field, value);
    }

    /**
     * @see org.ipdr.api.Outputter#outputMACAddress(int,
     *      org.ipdr.api.configuration.FieldConfigI, long)
     */
    public void outputMACAddress(int keyI, FieldConfigI field, long value)
    {
        // The whole function code (Alex)
        int i;
        String rawValue = Long.toHexString(value);
        int valueLength = rawValue.length();
        String str = "";
        for (i = 0; i < (16 - valueLength); i++)
        {
            str = str + "0";
            if ((i % 2) == 1) str = str + "-";
        }

        for (i = 0; i < valueLength; i++)
        {
            str = str + rawValue.charAt(i);
            if ((((16 - valueLength + i) % 2) == 1)
                    && ((16 - valueLength + i) < 15)) str = str + "-";
        }
        outputSep(keyI, field);
        this.line.append("MAC-ADD:").append(str);
        this.dedupOutputter.outputMACAddress(keyI, field, value);
    }

    /**
     * Outputs a separator
     * 
     * @param keyI
     * @param field
     */
    private final void outputSep(int keyI, FieldConfigI field)
    {
        this.sepWithinLine = this.sep;
        this.line.append(this.sep);
    }

    public final void outputShort(int keyI, FieldConfigI field, short value)
    {
        outputSep(keyI, field);
        this.line.append(value);
        this.dedupOutputter.outputShort(keyI, field, value);
    }

    /**
     * @see org.ipdr.api.Outputter#outputString(int,
     *      org.ipdr.api.configuration.FieldConfigI, java.lang.String)
     */
    public final void outputString(int keyI, FieldConfigI field, String str)
    {
        outputSep(keyI, field);
        this.line.append("'").append(str).append("'");
        this.dedupOutputter.outputString(keyI, field, str);
    }

    /**
     * @see org.ipdr.api.Outputter#outputUnsignedByte(int,
     *      org.ipdr.api.configuration.FieldConfigI, short)
     */
    public final void outputUnsignedByte(int keyI, FieldConfigI field,
            short value)
    {
        outputSep(keyI, field);
        this.line.append(value);
        this.dedupOutputter.outputUnsignedByte(keyI, field, value);
    }

    /**
     * @see org.ipdr.api.Outputter#outputUnsignedInt(int,
     *      org.ipdr.api.configuration.FieldConfigI, long)
     */
    public final void outputUnsignedInt(int keyI, FieldConfigI field, long value)
    {
        outputSep(keyI, field);
        this.line.append(value);
        this.dedupOutputter.outputUnsignedInt(keyI, field, value);
    }

    /**
     * @see org.ipdr.api.Outputter#outputUnsignedLong(int,
     *      org.ipdr.api.configuration.FieldConfigI, long)
     */
    public final void outputUnsignedLong(int keyI, FieldConfigI field,
            long value)
    {
        outputSep(keyI, field);
        
        byte[] rawBytes = {0,0,0,0,0,0,0,0,0};
        rawBytes[0] = 0;
        be.putLong(rawBytes, 1, value);
        BigInteger bd = new BigInteger(rawBytes);
        this.line.append(bd.toString());
        this.dedupOutputter.outputUnsignedLong(keyI, field, value);
    }

    /**
     * @see org.ipdr.api.Outputter#outputUnsignedShort(int,
     *      org.ipdr.api.configuration.FieldConfigI, int)
     */
    public final void outputUnsignedShort(int keyI, FieldConfigI field,
            int value)
    {
        outputSep(keyI, field);
        this.line.append(value);
        this.dedupOutputter.outputUnsignedShort(keyI, field, value);
    }

    /**
     * @see org.ipdr.api.Outputter#outputUUID(int,
     *      org.ipdr.api.configuration.FieldConfigI, byte[])
     */
    public void outputUUID(int keyI, FieldConfigI field, byte[] value)
    {
        outputSep(keyI, field); // Shaig
        this.line.append("UUID:");
        for (int i = 4; i < 20; i++)
        {
            this.line.append((char) value[i]);
        }
    }

    /**
     * @see org.ipdr.api.Outputter#startRecord(java.net.InetAddress, int,
     *      java.util.Date, long, int, int, byte[], boolean)
     */
    public final synchronized void startRecord(InetAddress transmitterAddress,
            int transmitterPort, Date transmitterBootTime, long seqNo,
            int configId, int sessionId, int templateId, byte[] documentId,
            boolean isDuplicate)
    {
        this.line.setLength(0);
        this.line.append("[csv] ").append(transmitterAddress).append(":")
                .append(transmitterPort).append(sep).append("boot=").append(
                        IPDRUtilities.dateFormat(transmitterBootTime)).append(isDuplicate? ":DUP:" : "").append(
                        sep).append("#").append(seqNo).append(sep).append(
                        "configid=").append(configId).append(sep).append(
                        "sess=").append(sessionId).append(sep).append("tmpl=")
                .append(templateId).append(";documentid=");
        for (int i = 0; i < documentId.length; i++)
        {
            short digit = (short)(0x00FF & documentId[i] );
            String rep = Long.toHexString(digit);
            if (rep.length() == 1) rep = "0" + rep;
            this.line.append(rep);
        }

        this.sepWithinLine = ": ";

        this.count++;
        this.lastTransmitterAddress = transmitterAddress;
        this.lastTransmitterPort = transmitterPort;
        this.lastSeqNo = seqNo;

        this.dedupOutputter.startRecord(transmitterAddress, transmitterPort,
                transmitterBootTime, seqNo, configId, sessionId, templateId,
                documentId, isDuplicate);
    }
   
}