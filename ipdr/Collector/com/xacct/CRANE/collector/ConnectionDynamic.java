package com.xacct.CRANE.collector;

// $Id: ConnectionDynamic.java,v 1.4 2005/01/30 15:13:41 ilyad Exp $
// Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.io.EOFException;
import java.io.IOException;
import java.io.InputStream;
import java.io.InterruptedIOException;
import java.io.OutputStream;
import java.io.PushbackInputStream;
import java.net.ConnectException;
import java.net.Socket;
import java.net.SocketException;
import java.net.UnknownHostException;

/** Encapsulation of a single connection from a session.
 *  Holds the socket, input and output streams, and any
 *  related information.
 *
 *  @version $Id: ConnectionDynamic.java,v 1.4 2005/01/30 15:13:41 ilyad Exp $
 */
public final class ConnectionDynamic implements java.io.Serializable
{
    /** $Id: ConnectionDynamic.java,v 1.4 2005/01/30 15:13:41 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: ConnectionDynamic.java,v 1.4 2005/01/30 15:13:41 ilyad Exp $"; }

    /** The connection socket. */
    private Socket socket;

    /** socket.getInputStream(). */
    private InputStream in;

    /** socket.getOutputStream(). */
    private OutputStream out;

    /** Where to write log messages. */
    public LogHandler getLog() { return log; }
    private LogHandler log;

    /** The last time-out value (0 means infinite timeout). This is
     *  kept as a class value, so that we can avoid unneeded calls to
     *  socket.setSoTimeout().
     * @see #setSoTimeout
     */
    private int timeout = 0;

    private CollectorConfig collectorConfig;
    /** The collector context. */
    public final CollectorConfig getCollectorConfig() { return collectorConfig; }

    private ConnectionConfig connectionConfig;
    public final ConnectionConfig getConnectionConfig() { return connectionConfig; }

    /** Create a connection and make the connection to the other end.
     * @param collectorConfig The collector context.
     * @param connectionConfig The connection's configuration.
     * @param log Where to write log messages.
     * @param receiveBufferSize The receive buffer size (0 means use default).
     * @throws ConnectionIOException If something goes wrong with the connection setup.
     */
    public ConnectionDynamic(CollectorConfig collectorConfig,
                             ConnectionConfig connectionConfig,
                             LogHandler log,
                             int receiveBufferSize)
        throws ConnectionIOException {
        this.connectionConfig = connectionConfig;
        this.collectorConfig = collectorConfig;
        this.log = log;

        this.connectTCP(log, receiveBufferSize);
    }

    /** Connect to transmitter via TCP
     *  (transmitterAddress:transmitterPort are "global" to this
     *  method), waiting up to connectionConfig.connectTimeout
     *  milliseconds
     *  @exception ConnectionIOException most I/O exceptions are
     *  caught with the reason preserved in the
     *  ConnectionIOException.getCause().
     */
    private final void connectTCP(LogHandler log, int receiveBufferSize)
        throws ConnectionIOException {

        // We might want to put a timeout on the connection.
        // XXX
        // With JDK 1.4, we could just do:
        // socket = new Socket();
        // socket.connect(new InetSocketAddress(connectionConfig.getTransmitterAddress(), connectionConfig.getTransmitterPort()),
        //                connectionConfig.connectTimeout);
        // With JDK 1.3, we would need to set a separate timer thread; when the sleep
        // sleep() times out, if no connection has been made, interrupt the main thread.

        log.debug(this, "connectTcp", "Trying TCP connection to " +
                  connectionConfig.transmitterAndPortString() +
                  " from " + collectorConfig.getCollectorAddress());

        try {
            // Normally collector = local host, so the following will
            // usually be the same as:
            // socket = new Socket(connectionConfig.getTransmitterAddress(),
            //                     connectionConfig.getTransmitterPort())
            // But we want to be sure that things happen the way we want on
            // multi-homed systems, if the collector address is given.
            //
            // Note that port == 0 lets the system pick a port (this
            // is semi-documented in JDK API doc).
            //
            // We cannot use the "create and connect" constructor because that would
            // cause a problem with receive buffers > 64K. Instead, we do things
            // in steps. Except that that requires 1.4.
            // XXX JDK 1.4: Use the connect-with-timeout version;

            // XXX When we get JDK 1.4, use newSocket();
            //      (setReuseAddress(true) not needed)
            //     setReceiveBufferSize
            //     connect, using port=0 for local

            this.socket = new Socket(connectionConfig.getTransmitterAddress().get(),
                   connectionConfig.getTransmitterPort(),
                   collectorConfig.getCollectorAddress().get(), 0);

            if (receiveBufferSize > 0) {
                this.socket.setReceiveBufferSize(receiveBufferSize);
            }

            try {
                socket.setSoLinger(true, 5); // XXX This delay should be a parameter
                // The SoLinger doesn't seem to have proper effect.
            } catch (SocketException e) {
                log.warn(this, "receiveConfig", "setSoLinger exception: " + e);
            }


            // socket.setSoTimeout(...); // if we care about blocking on input or output [we don't]

            this.socket.setTcpNoDelay(true);
            try {
                this.socket.setKeepAlive(true);
            } catch (SocketException ex) {
                log.warn(this, "receiveConfig", "Cannot set TCP keep-alive on " +
                         this.socket + ": " + ex);
            }

            // we have our own implementation of pushback, so don't use Sun's,
            // which would be called like this:
            // this.in = new PushbackInputStream(this.socket.getInputStream(), 1000);
            this.in = this.socket.getInputStream();
            this.out = this.socket.getOutputStream();
            log.info(this, "receiveConfig", "Successful TCP connection to " +
                     connectionConfig.transmitterAndPortString() +
                     " from " + collectorConfig.getCollectorAddress() +
                     " on socket " + socket);
        } catch (UnknownHostException e) {
            throw new ConnectionIOException("could not get a host name when connecting via TCP",
                                            connectionConfig, e);
        } catch (ConnectException e) {
            throw new ConnectionIOException("could not connect via TCP", connectionConfig, e);

        } catch (IOException e) {
            throw new ConnectionIOException("got I/O error while connecting via TCP", connectionConfig, e);

        }
    }

    /** Close the socket connection and ensure everything is sent.
     *  (With the usual caveat about solinger and friends.)
     * @throws ConnectionIOException If the close doesn't go properly.
     */
    public final void close() throws ConnectionIOException {
        try {
            out.flush();
            out.close();
            if (socket == null) {
                log.warn(this, "close", "socket is null");
            } else {
                // socket.shutdownOutput(); // already done by out.close(): should flush all data out.
                socket.close();
            }
            in.close();
        } catch (IOException e) {
            throw new ConnectionIOException("Error while closing TCP connection",
                                            connectionConfig, e);
        }
    }

    /** Set the socket timeout for read. This operates on a cached
     *  value, for efficiency (only calling socket.setSoTimeout when
     *  there's a changed).
     * @param timeout miliseconds to wait before interrupting the read.
     * @throws SocketException If it's not supported.
     * @see #getInputSynchronizer
     */
    protected final void setSoTimeout(int timeout) throws SocketException {
        if (this.timeout != timeout) {
            socket.setSoTimeout(timeout);
            this.timeout = timeout;
        }
    }

    /** Write an array of bytes to the socket.
     *
     * If you want to write in multiple chunks (e.g., a header, then
     * the body), you should use {@link #getInputSynchronizer} to
     * ensure that another thread doesn't interfere.
     * @param b The bytes.
     * @throws IOException If the write fails.
     * @see OutputStream#write
     * @see #getOutputSynchronizer
     */
    public void write(byte[] b) throws IOException {
        out.write(b);
    }

    /** Read a specified number of bytes.
     *
     * See the general contract of the <code>readFully</code>
     * method of <code>DataInput</code>.
     *
     * Bytes for this operation are read from the contained input
     * stream.
     *
     * If you want to read in multiple chunks (e.g., a header, then
     * the body), you should use {@link #getInputSynchronizer} to
     * ensure that another thread doesn't interfere.
     *
     * @param      b     the buffer into which the data is read.
     * @param      off   the start offset of the data.
     * @param      len   the number of bytes to read.
     * @param      timeout timeout in milliseconds.
     * @exception  EOFException  if this input stream reaches the end before
     *               reading all the bytes.
     * @exception  IOException   if an I/O error occurs.
     * @exception  InterruptedIOException if the timeout is exceeded; in this
     *               case the bytesTransferred vaule will have the number
     *               of bytes that were read into the buffer (b) before
     *               the exception. A value of zero should not be interpreted
     *               as meaning that nothing was read; the implementation of
     *               this method may choose to save the bytes in a separate
     *               (invisible) buffer.
     * @see        java.io.FilterInputStream#in
     * @see        java.io.DataInput#readFully
     * @see        #getInputSynchronizer
     */

    public final void readFully(byte[] b,
                                int off,
                                int len, // max # of bytes to read
                                int timeout)
        throws IOException {

        // Here is the JRE code for readFully. Note that it can throw
        // an exception before it returns:

        // public final void readFully(byte b[], int off, int len) throws IOException {
        //      if (len < 0)
        //          throw new IndexOutOfBoundsException();
        //      InputStream in = this.in;
        //      int n = 0;
        //      while (n < len) {
        //          int count = in.read(b, off + n, len - n);
        //          if (count < 0)
        //              throw new EOFException();
        //          n += count;
        //      }
        // }

        // So, it's important to keep the following logic because we
        // don't want to time out on a socket while reading.  If we
        // get nothing on it, that's ok, but if there's SOMETHING, we
        // have to make sure it is of size len

        int rb = 0; // # of bytes that were read
        long t1 = System.currentTimeMillis();

        while (rb < len) {
            try {
                setSoTimeout(timeout);
                int rbOnce = read(b, off + rb, len - rb);
                if (rbOnce < 0) {
                    unread(b, off, rb);
                    // Not sure that we should push back the bytes
                    // read so far or no; but there shouldn't be
                    // another read anyway. Unfortunately, there's no
                    // way to report the number of bytes read so far
                    // (although we could subclass EOFException to
                    // provide this).
                    throw new EOFException("Unexpected end-of-stream on " + in);
                } else {
                    rb += rbOnce;
                }
            } catch (InterruptedIOException ex) {
                // There are two choices of how to proceed: just
                // report the bytes that weren't transferred, or push
                // them all back and report that nothing was
                // transferred. We choose to report all the bytes that
                // were transferred because that doesn't require
                // extending our pushback buffer, although chances are
                // the caller wil call unread() to do this anyway.
                if (true) {
                    ex.bytesTransferred += rb;
                } else {
                    unread(b, off, rb + ex.bytesTransferred);
                    ex.bytesTransferred = 0;
                }
                throw ex;
            }
            if (timeout > 0 && rb < len) {
                long t2 = System.currentTimeMillis();
                timeout -= (int)(t2 - t1);
                t1 = t2;
                if (timeout <= 0) {
                    timeout = 1;
                }
            }
        }
    }

    /** The pushback buffer (we can't use PushbackStream because it
     *  requires a single fixed allocation and we don't know in general
     *  how much pushback will be needed).
     *  @see #unreadPos
     */
    protected byte [] unreadBuf = null;

    /** The position within the pushback buffer from which the next byte will
     *  be read.  When the buffer is empty, <code>pos</code> is equal to
     *  <code>buf.length</code>; when the buffer is full, <code>pos</code> is
     *  equal to zero.
     *  @see #unreadBuf
     */
    protected int unreadPos = 0;

    /** Amount to increase the pushback buffer beyond what is needed
     *  (to avoid repetitive small re-allocations).
     *  @see #unreadBuf
     */
    protected static int extraAlloc = 1000;

    /** Push back bytes to be read again.
     *
     * If you are reading in multiple chunks (e.g., a header, then
     * the body), you should use {@link #getInputSynchronizer} to
     * ensure that another thread doesn't interfere.
     * @param b Bytes to push back.
     * @param off Offset with b of the bytes to be pushed back.
     * @param len Number of bytes to be pushed back.
     * @see PushbackInputStream#unread
     * @see #unreadBuf
     * @see #unreadPos
     * @see #extraAlloc
     * @see #getInputSynchronizer
     */
    public final void unread(byte[] b, int off, int len) {
        if (len == 0) {
            return;
        }
        if (unreadBuf == null) {
            unreadBuf = new byte[len + extraAlloc];
            unreadPos = unreadBuf.length;
        } else if (len > unreadPos) {
            int add = len - unreadPos + extraAlloc;
            byte [] newUnreadBuf = new byte[unreadBuf.length + add];
            System.arraycopy(unreadBuf, unreadPos,
                             newUnreadBuf, unreadPos + add,
                             unreadBuf.length - unreadPos);
            unreadBuf = newUnreadBuf;
            unreadPos += add;
        }
        unreadPos -= len;
        System.arraycopy(b, off, unreadBuf, unreadPos, len);
    }

    /** Read bytes from the input stream.
     *
     * If you want to read in multiple chunks (e.g., a header, then
     * the body), you should use {@link #getInputSynchronizer} to
     * ensure that another thread doesn't interfere.
     * @param b array of bytes to be filled.
     * @param off offset within the array to start filling.
     * @param len maximum number of bytes to read.
     * @return number of bytes actually read.
     * @throws IOException if something goes wrong (or a timeout).
     * @see PushbackInputStream#read
     * @see #getInputSynchronizer
     */
    protected int read(byte[] b, int off, int len) throws IOException {
        // The following code is stolen shamelessly from
        // PushbackInputStream.read().

        if (len == 0) {
            return 0;
        }

        int avail = (unreadBuf == null) ? 0 : unreadBuf.length - unreadPos;
        if (avail > 0) {
            if (len < avail) {
                avail = len;
            }
            System.arraycopy(unreadBuf, unreadPos, b, off, avail);
            unreadPos += avail;
            off += avail;
            len -= avail;
        }
        if (len > 0) {
            len = in.read(b, off, len);
            if (len == -1) {
                return avail == 0 ? -1 : avail;
            }
            return avail + len;
        }
        return avail;
    }

    /** An object to use for synchronizing on inputs.  This might be
     *  needed if you do an intial {@link #readFully} to get a header,
     *  followed by another {@link #readFully} to get the rest of the
     *  message.
     *
     * @return The synchronization object.
     */
    public Object getInputSynchronizer() { return in; }

    /** An object to use for synchronizing on outputs.
     *
     * @return The synchronization object.
     */
    public Object getOutputSynchronizer() { return out; }

    public final String toString() {
        String extra = "";
        if (socket == null) {
            extra = ", noSocket";
        } else {
            try {
                extra = ", linger=" + socket.getSoLinger() +
                    ", noDelay=" + socket.getTcpNoDelay() +
                    ", keepalive=" + socket.getKeepAlive();
            } catch (SocketException e) {
                extra = ", socket=" + socket + ", exception: " + e;
            }
        }
        return connectionConfig.toString() + extra;
    }

    /** Get the address and port as a printable string.
     *  @return A string in the form "IPAddress:port"
     */
    public final String addrAndPortToString() {
        return connectionConfig.getTransmitterAddress().andPortToString(connectionConfig.getTransmitterPort());
    }
}
