package com.xacct.CRANE.collector;

// $Id: CollectorOutputter.java,v 1.5 2005/04/08 12:48:10 guyg Exp $
// Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.util.Date;

/** An interface for encapsulating the methods for outputting values
 *  for a record from the Collector.
 *
 *  No exceptions are defined here: if something goes wrong, the
 *  preferred exceptions are ClassCastException or TransformerException.
 *  (TransformerException is for Serializable, but makes a lot of
 *  sense in this case also).
 *  <p>
 *  This interface contains two capabilities:
 *  <ul>
 *  <li>Record outputting
 *    <ul>
 *      <li>{@link #startRecord startRecord}
 *      <li>{@link #outputString outputXxx} (repeated) for the fields
 *      <li>{@link #endRecord endRecord}
 *    </ul>
 *  <li>Acknowledgment synchronization
 *    <ul>
 *      <li>{@link #commitTimerExpired commitTimerExpired}
 *      <li>{@link #getLastSeqNoToAck getLastSeqNoToAck}
 *      <li>{@link ProcessDataSeqNoAck#processDataSeqNoAck processDataSeqNoAck}
 *    </ul>
 *  </ul>
 *  <p>
 *  Acknowledgment synchronization is to ensure that the
 *  {@link ConnectionThread} does not send back an ACK until a
 *  record is actually committed.
 *  Just because a record has been output doesn't mean
 *  that it has been properly persisted. So, there must be a
 *  communication between the outputter and the connection
 *  thread. This communication is done by {@link #getLastSeqNoToAck getLastSeqNoToAck}.
 *  The triggering of an ACk can be done either by the connection
 *  thread (calling {@link #commitTimerExpired commitTimerExpired})
 *  or by the outputter (calling back to the connection thread via
 *  {@link ProcessDataSeqNoAck#processDataSeqNoAck processDataSeqNoAck}
 *  <p>
 *  The caller must guarantee that the sequence will always be
 *    <ol>
 *      <li>{@link #startRecord startRecord}
 *      <li>{@link #outputString outputXxx} (repeated)
 *      <li>{@link #endRecord endRecord}
 *    </ol>
 *  <p>
 *  The calls to outputXxx can contain any template at all; if you
 *  need to set things up for multiple kinds of output, then you
 *  should do that in the constructor (that is, get the fields and
 *  template information from CollectorConfig).
 *  <p>
 *  Calls to {@link #isValidLastSeqNoToAck isValidLastSeqNoToAck}
 *  and {@link #getLastSeqNoToAck getLastSeqNoToAck} can
 *  happen at any time. It is the responsibility of the implementor to
 *  make these thread safe (this includes being thread safe in a call
 *  to startRecord or endRecord).
 *  <p>
 *  The sequence number to {@link #startRecord startRecord} is used to indicate
 *  which records have been committed.
 *  <p>
 *  There are two ways of communicating between the connection thread
 *  and the outputter (at least one must be provided; both can be if
 *  desired):
 *  <ol>
 *    <li>At regular intervals the connection thread can request
 *        the last persisted sequence number.
 *    <li>Whenever the outputter has reached a checkpoint, it can
 *        tell the connection thread. This is done with the the object
 *        passed to {@link #registerProcessDataSeqNoAck registerProcessDataSeqNoAck}.
 *  </ol>
 *  <p>
 *  If you wish to allow the connection thread to query the last
 *  record successfully committed, you must use the the
 *  {@link #isValidLastSeqNoToAck isValidLastSeqNoToAck}
 *  and {@link #getLastSeqNoToAck getLastSeqNoToAck} methods.
 *  The connection calls these like this:
 *  <code><br>if (outputter.isValidLastSeqNoToAck()) {</code>
 *  <code><br>&nbsp;&nbsp;lastSeqNo = outputter.getLastSeqNoToAck();</code>
 *  <code><br>&nbsp;&nbsp;</code><i>send ACK to transmitter</i>
 *  <code><br>}</code>
 *  <p>
 *  Whenever records are committed, you should have
 *  {@link #isValidLastSeqNoToAck isValidLastSeqNoToAck} return <code>true</code>, with the
 *  sequence number returned by {@link #getLastSeqNoToAck getLastSeqNoToAck}.
 *  <p>
 *  Before calling {@link #startRecord startRecord} for the first time, the
 *  connection thread will call {@link #registerProcessDataSeqNoAck registerProcessDataSeqNoAck}.
 *  This gives an object which implements a single method:
 *  {@link ProcessDataSeqNoAck#processDataSeqNoAck rocessDataSeqNoAck#processDataSeqNoAck}. Whenever the
 *  outputter reaches a commit point, it can call this method.
 *  <p>
 *  (Note that the connection thread may call
 *  {@link #registerProcessDataSeqNoAck registerProcessDataSeqNoAck} with a parameter of
 *  <code>null</code> ... this is used to deregister the callback.)
 *  <p>
 *  If you use this second method, you can either always have
 *  {@link #isValidLastSeqNoToAck isValidLastSeqNoToAck} always return <code>false</code>; or
 *  you can have it return <code>true</true> and give the committed
 *  seequence number as the result from {@link #getLastSeqNoToAck getLastSeqNoToAck}.
 *
 *  @version $Id: CollectorOutputter.java,v 1.5 2005/04/08 12:48:10 guyg Exp $
 */
public interface CollectorOutputter
{
    /** Prepare to output a record. The parameters given here
     *  will be active until endRecord is called.
     *
     *  See the note on thread safety for this interface.
     *
     *  @param transmitterAddress the transmitter's address (for this connection).
     *  @param transmitterPort the transmitter's data port (for this connection)
     *            as negotiated by the version negotiation messages
     *            or (if that failed) by the configuration.
     *  @param transmitterVendorString the transmitter's vendor string (for this connection),
     *            as returned by the GET SESS RSP message.
     *  @param transmitterBootTime the transmitter's boot time (for this connection),
     *            as returned by the START ACK message.
     *  @param seqNo the sequence number of the record. Usually, this
     *               increases each time, but there may be occasions
     *               when the sequence number starts back at a lower
     *               value or skips.
     *  @param sessionId
     *  @param templateId
     *  @see #endRecord
     */
    public void startRecord(ExtendedInetAddress transmitterAddress,
                            int transmitterPort,
                            String transmitterVendorString,
                            Date transmitterBootTime,
                            long seqNo,
                            int sessionId,
                            int templateId);

    /** Finish outputting a record. After this, the parameters
     *  given via startRecord will be invalid.
     *
     *  See the note on thread safety for this interface.
     *  @see #startRecord
     *
     *  @throws OutputterException if the record could not be output.
     */
    public void endRecord() throws OutputterException;

    /** Finish ouputting all records. You should also implement
     *  finalize(), which calls this.
     */
    public void close();

    /** The caller thread may have a timer for forcing a commit: it
     *  calls this method whenever the timer expires.  The behavior on
     *  this should be non-blocking. The outputter should attempt to
     *  commit as much as it can, in expectation of very soon being
     *  asked about the next commited sequence number. It can, if it
     *  wants, call the method given by
     *  {@link #registerProcessDataSeqNoAck registerProcessDataSeqNoAck}, but this is not necessary.
     */
    public void commitTimerExpired();

    /** Test whether a call to getLastSeqNoToAck will give a valid result.
     *  Typically, this will be false until the first record has been
     *  properly persisted, then true thereafter. It is the caller's
     *  responsibility to deal with multiple calls to getLastSeqNoToAck;
     *  the test is simply for validity, not whether there is a new
     *  value.
     *
     *  See the note on thread safety for this interface.
     *  @return true if the call to newSeqNoToAck will generate
     *               a new value from any previous call
     *  @see #getLastSeqNoToAck
     *  @see #endRecord
     *  @see #registerProcessDataSeqNoAck
     */
    public boolean isValidLastSeqNoToAck();

    /** Get the sequence number of the last record that has been
     *  properly persisted. If newSeqNoToAck would return false,
     *  the value from this call is undefined.
     *
     *  See the note on thread safety for this interface.
     *  @return The latest sequence number that can be acknowledged.
     *  @see #isValidLastSeqNoToAck
     */
    public long getLastSeqNoToAck();

    /** For cases where the outputter can tell when an acknowledgment
     *  should be done, a "callback". The outputter can completely ignore
     *  this. If it uses this, it may choose to always have
     *  isValidLastSeqNoToAck return false.
     *  This routine may only be called before a {@link #startRecord startRecord}
     *  or after a {@link #endRecord endRecord} (that is, when an output record is
     *  not being built up).
     *  @param processDataSeqNoAck the class for handling acknowledgments
     *            ... may be null, which causes "deregistration"
     *  @see #isValidLastSeqNoToAck
     */
    public void registerProcessDataSeqNoAck(ProcessDataSeqNoAck processDataSeqNoAck);

    /** Output a single boolean field.
     *  @param keyI This field is the keyI'th field in the record.
     *  @param field The field's configuration.
     *  @param fieldDesc The field's description.
     *  @param value The value to output.
     */
    public void outputBoolean(int keyI, FieldConfig field, FieldDescriptor fieldDesc, byte value);

    /** Output a single unsigned byte field.
     *  @param keyI This field is the keyI'th field in the record.
     *  @param field The field's configuration.
     *  @param fieldDesc The field's description.
     *  @param value The value to output.
     */
    public void outputUnsignedInt8(int keyI, FieldConfig field, FieldDescriptor fieldDesc, short value);

    /** Output a single signed byte field.
     *  @param keyI This field is the keyI'th field in the record.
     *  @param field The field's configuration.
     *  @param fieldDesc The field's description.
     *  @param value The value to output.
     */
    public void outputInt8(int keyI, FieldConfig field, FieldDescriptor fieldDesc, byte value);

    /** Output a single unsigned short integer field.
     *  @param keyI This field is the keyI'th field in the record.
     *  @param field The field's configuration.
     *  @param fieldDesc The field's description.
     *  @param value The value to output.
     */
    public void outputUnsignedInt16(int keyI, FieldConfig field, FieldDescriptor fieldDesc, int value);

    /** Output a single signed short integer field.
     *  @param keyI This field is the keyI'th field in the record.
     *  @param field The field's configuration.
     *  @param fieldDesc The field's description.
     *  @param value The value to output.
     */
    public void outputInt16(int keyI, FieldConfig field, FieldDescriptor fieldDesc, short value);

    /** Output a single unsigned integer field.
     *  @param keyI This field is the keyI'th field in the record.
     *  @param field The field's configuration.
     *  @param fieldDesc The field's description.
     *  @param value The value to output.
     */
    public void outputUnsignedInt32(int keyI, FieldConfig field, FieldDescriptor fieldDesc, long value);

    /** Output a single signed integer field.
     *  @param keyI This field is the keyI'th field in the record.
     *  @param field The field's configuration.
     *  @param fieldDesc The field's description.
     *  @param value The value to output.
     */
    public void outputInt32(int keyI, FieldConfig field, FieldDescriptor fieldDesc, int value);

    /** Output a single unsigned long integer field.
     *  @param keyI This field is the keyI'th field in the record.
     *  @param field The field's configuration.
     *  @param fieldDesc The field's description.
     *  @param value The value to output.
     */
    public void outputUnsignedInt64(int keyI, FieldConfig field, FieldDescriptor fieldDesc, long value);

    /** Output a single signed long integer field.
     *  @param keyI This field is the keyI'th field in the record.
     *  @param field The field's configuration.
     *  @param fieldDesc The field's description.
     *  @param value The value to output.
     */
    public void outputInt64(int keyI, FieldConfig field, FieldDescriptor fieldDesc, long value);

    /** Output a single float field.
     *  @param keyI This field is the keyI'th field in the record.
     *  @param field The field's configuration.
     *  @param fieldDesc The field's description.
     *  @param value The value to output.
     */
    public void outputFloat(int keyI, FieldConfig field, FieldDescriptor fieldDesc, float value);

    /** Output a single double float field.
     *  @param keyI This field is the keyI'th field in the record.
     *  @param field The field's configuration.
     *  @param fieldDesc The field's description.
     *  @param value The value to output.
     */
    public void outputDouble(int keyI, FieldConfig field, FieldDescriptor fieldDesc, double value);

    /** Output a single string field (from a string).
     *  @param keyI This field is the keyI'th field in the record.
     *  @param field The field's configuration.
     *  @param fieldDesc The field's description.
     *  @param value The value to output.
     */
    public void outputString(int keyI, FieldConfig field, FieldDescriptor fieldDesc, String value);

    /** Output a single string field (from an array of bytes).
     *  @param keyI This field is the keyI'th field in the record.
     *  @param field The field's configuration.
     *  @param fieldDesc The field's description.
     *  @param value The value to output.
     */
    public void outputString(int keyI, FieldConfig field, FieldDescriptor fieldDesc, byte [] buf, int offset, int len);

    /** Output a single IPV4 address field.
     *  @param keyI This field is the keyI'th field in the record.
     *  @param field The field's configuration.
     *  @param fieldDesc The field's description.
     *  @param value The value to output.
     */
    public void outputIPv4Addr(int keyI, FieldConfig field, FieldDescriptor fieldDesc, int value);

    /** Output a single time field (seconds since epoch).
     *  @param keyI This field is the keyI'th field in the record.
     *  @param field The field's configuration.
     *  @param fieldDesc The field's description.
     *  @param value The value to output.
     */
    public void outputTimeSec(int keyI, FieldConfig field, FieldDescriptor fieldDesc, long value);

    /** Output a single time field (milliseconds since epoch).
     *  @param keyI This field is the keyI'th field in the record.
     *  @param field The field's configuration.
     *  @param fieldDesc The field's description.
     *  @param value The value to output.
     */
    public void outputTimeMillisec(int keyI, FieldConfig field, FieldDescriptor fieldDesc, long value);

    /** Output a single time field (microseconds since epoch).
     *  @param keyI This field is the keyI'th field in the record.
     *  @param field The field's configuration.
     *  @param fieldDesc The field's description.
     *  @param value The value to output.
     */
    public void outputTimeMicrosec(int keyI, FieldConfig field, FieldDescriptor fieldDesc, long value);


    // XXX More to be added ...
    //    FieldConfig.TYPE_UTF8:
    //    FieldConfig.TYPE_UTF16:
    //    FieldConfig.TYPE_BLOB:
    //    FieldConfig.TYPE_IPV6_ADDR:
}
