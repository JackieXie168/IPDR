package org.ipdr;
//Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.io.EOFException;
import java.io.IOException;
import java.io.InputStream;
import java.io.InterruptedIOException;
import java.io.OutputStream;
import java.io.PushbackInputStream;
import java.net.ConnectException;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;
import java.net.SocketTimeoutException;
import java.net.UnknownHostException;

import org.ipdr.api.ConnectionIOException;
import org.ipdr.api.configuration.CollectorConfigI;
import org.ipdr.api.configuration.ConnectionConfigI;
import org.ipdr.api.log.LogManager;

/**
 * Encapsulation of a single connection from a session. Holds the socket, input
 * and output streams, and any related information.
 *  
 */
public final class ConnectionDynamic implements java.io.Serializable
{
    /** The connection socket. */
    private Socket       socket;

    /** socket.getInputStream(). */
    private InputStream  in;
    /** socket.getOutputStream(). */
    private OutputStream out;
    /**
     * The last time-out value (0 means infinite timeout). This is kept as a
     * class value, so that we can avoid unneeded calls to
     * socket.setSoTimeout().
     * 
     * @see #setSoTimeout
     */
    private int          timeout        = 0;

    /**
     * @return Collector configuration context
     */
    public final CollectorConfigI getCollectorConfig()
    {
        return collectorConfig;
    }
    /**
     * Holds configuration of the current connection
     */
    private ConnectionConfigI connectionConfig;
    /**
     * Holds collector's configuration
     */
    private CollectorConfigI  collectorConfig;

    /**
     * @return Connection configuration
     */
    public final ConnectionConfigI getConnectionConfig()
    {
        return connectionConfig;
    }

    /**
     * Create a connection and make the connection to the other end.
     * 
     * @param connectionConfig
     *            The connection's configuration.
     */
    public ConnectionDynamic(ConnectionConfigI connectionConfig)
            throws ConnectionIOException
    {
        this.connectionConfig = connectionConfig;
        this.collectorConfig = connectionConfig.getCollectorConfig();
        this.connectTCP();
    }

    /**
     * Make a ServerSocket on the port defined in the collector configuration
     * object
     * 
     * @param collectorConfig -
     *            collector configuration
     * @return listening (server) socket
     * @throws IOException
     */
    public static ServerSocket makeServerSocket(CollectorConfigI collectorConfig)
            throws IOException
    {
            LogManager.debug("ServerSocket", "listenTCP", "Listening on "
                    + collectorConfig.getLocalPort());
            ServerSocket serverSocket = new ServerSocket(collectorConfig
                    .getLocalPort(), collectorConfig.getBacklogSize(),
                    collectorConfig.getCollectorAddress());
            serverSocket
                    .setSoTimeout(collectorConfig.getAcceptTimeout() * 1000);

            return serverSocket;
    }

    /**
     * Creates a new instance of ConnectionDynamic, which listens on the given
     * server socket for incoming TCP connections The method throws an exception
     * if no connection was made during AcceptTimeout seconds
     * 
     * @param serverSocket -
     *            server socket to accept connections on
     * @param collectorConfig -
     *            collector configuration
     * @return ConnectionDynamic instance
     * @throws ConnectionIOException
     */
    public static ConnectionDynamic makeListeningInstance(
            ServerSocket serverSocket, CollectorConfigI collectorConfig)
            throws ConnectionIOException
    {
        try
        {
            return new ConnectionDynamic(serverSocket.accept(), collectorConfig);
        }
        catch (SocketTimeoutException ste)
        {
            LogManager.debug("ConnectionDynamic", "makeListeningInstance",
                    "Accept timeout fired");
            return null;
        }
        catch (IOException iex)
        {
            LogManager.error("ConnectionDynamic", "makeListeningInstance", iex
                    .getMessage());
            throw new ConnectionIOException(
                    "got I/O error while listening to TCP", iex);
        }
    }

    /**
     * This constructor is used by makeListeningInstance to instantiate
     * ConnectionDynamic with an incoming socket
     * 
     * @param socket -
     *            socket to use for the data connection
     * @param collectorConfig -
     *            collector configuration
     * @throws ConnectionIOException
     */
    protected ConnectionDynamic(Socket socket, CollectorConfigI collectorConfig)
            throws ConnectionIOException
    {
        this.socket = socket;
        this.collectorConfig = collectorConfig;
        try
        {
            prepareSocket();
        }
        catch (UnknownHostException e)
        {
            throw new ConnectionIOException(
                    "could not get a host name when connecting via TCP",
                    connectionConfig, e);
        }
        catch (ConnectException e)
        {
            throw new ConnectionIOException("could not connect via TCP",
                    connectionConfig, e);
        }
        catch (IOException e)
        {
            throw new ConnectionIOException(
                    "got I/O error while connecting via TCP", connectionConfig,
                    e);
        }
    }

    /**
     * Connect to transmitter via TCP (transmitterAddress:transmitterPort are
     * "global" to this method), waiting up to
     * connectionConfig.getConnectTimeout seconds
     * 
     * @exception ConnectionIOException
     *                most I/O exceptions are caught with the reason preserved
     *                in the ConnectionIOException.getCause().
     */
    private final void connectTCP() throws ConnectionIOException
    {
        LogManager.debug(this, "connectTcp", "Trying TCP connection to "
                + connectionConfig.getExporterAndPortString() + " from "
                + getCollectorConfig().getCollectorAddress());
        try
        {
            this.socket = new Socket(connectionConfig.getExporterAddress(),
                    connectionConfig.getExporterPort());
            prepareSocket();
        }
        catch (UnknownHostException e)
        {
            throw new ConnectionIOException(
                    "could not get a host name when connecting via TCP",
                    connectionConfig, e);
        }
        catch (ConnectException e)
        {
            throw new ConnectionIOException("could not connect via TCP",
                    connectionConfig, e);
        }
        catch (IOException e)
        {
            throw new ConnectionIOException(
                    "got I/O error while connecting via TCP", connectionConfig,
                    e);
        }
    }

    /**
     * Sets various socket parameters for the current socket
     * 
     * @throws SocketException
     * @throws IOException
     */
    private void prepareSocket() throws SocketException, IOException
    {
        if (getCollectorConfig().getReceiveBufferSize() > 0)
        {
            this.socket.setReceiveBufferSize(getCollectorConfig()
                    .getReceiveBufferSize());
        }
        try
        {
            socket.setSoLinger(true, 5);
        }
        catch (SocketException e)
        {
            LogManager.warn(this, "receiveConfig", "setSoLinger exception: "
                    + e);
        }
        this.socket.setTcpNoDelay(true);
        try
        {
            this.socket.setKeepAlive(true);
        }
        catch (SocketException ex)
        {
            LogManager.warn(this, "receiveConfig",
                    "Cannot set TCP keep-alive on " + this.socket + ": " + ex);
        }
        this.in = this.socket.getInputStream();
        this.out = this.socket.getOutputStream();
        LogManager.info(this, "receiveConfig", "Successful TCP connection to "
                + socket.getInetAddress() + " from " + socket.getPort());
    }

    /**
     * Close the socket connection and ensure everything is sent. (With the
     * usual caveat about solinger and friends.)
     * 
     * @throws ConnectionIOException
     *             If the close doesn't go properly.
     */
    public final void close() throws ConnectionIOException
    {
        try
        {
            out.flush();
            out.close();
            if (socket == null)
            {
                LogManager.warn(this, "close", "socket is null");
            }
            else
            {
                socket.close();
            }
            in.close();
            LogManager.info(this, "close", "socket is closed");
        }
        catch (IOException e)
        {
            throw new ConnectionIOException(
                    "Error while closing TCP connection", connectionConfig, e);
        }
    }

    /**
     * Set the socket timeout for read. This operates on a cached value, for
     * efficiency (only calling socket.setSoTimeout when there's a changed).
     * 
     * @param timeout
     *            miliseconds to wait before interrupting the read.
     * @throws SocketException
     *             If it's not supported.
     * @see #getInputSynchronizer
     */
    protected final void setSoTimeout(int timeout) throws SocketException
    {
        if (this.timeout != timeout)
        {
            socket.setSoTimeout(timeout);
            this.timeout = timeout;
        }
    }

    /**
     * Write an array of bytes to the socket.
     * 
     * If you want to write in multiple chunks (e.g., a header, then the body),
     * you should use {@link #getInputSynchronizer}to ensure that another
     * thread doesn't interfere.
     * 
     * @param b
     *            The bytes.
     * @throws IOException
     *             If the write fails.
     * @see OutputStream#write
     * @see #getOutputSynchronizer
     */
    public void write(byte[] b) throws IOException
    {
        out.write(b);
    }

    /**
     * Read a specified number of bytes.
     * 
     * See the general contract of the <code>readFully</code> method of
     * <code>DataInput</code>.
     * 
     * Bytes for this operation are read from the contained input stream.
     * 
     * If you want to read in multiple chunks (e.g., a header, then the body),
     * you should use {@link #getInputSynchronizer}to ensure that another
     * thread doesn't interfere.
     * 
     * @param b
     *            the buffer into which the data is read.
     * @param off
     *            the start offset of the data.
     * @param len
     *            the number of bytes to read.
     * @param timeout
     *            timeout in milliseconds.
     * @exception EOFException
     *                if this input stream reaches the end before reading all
     *                the bytes.
     * @exception IOException
     *                if an I/O error occurs.
     * @exception InterruptedIOException
     *                if the timeout is exceeded; in this case the
     *                bytesTransferred vaule will have the number of bytes that
     *                were read into the buffer (b) before the exception. A
     *                value of zero should not be interpreted as meaning that
     *                nothing was read; the implementation of this method may
     *                choose to save the bytes in a separate (invisible) buffer.
     * @see java.io.FilterInputStream#in
     * @see java.io.DataInput#readFully
     * @see #getInputSynchronizer
     */
    public final void readFully(byte[] b, int off, int len,
    // max # of bytes to read
            int timeout) throws IOException
    {
        int rb = 0; // # of bytes that were read
        long t1 = System.currentTimeMillis();
        while (rb < len)
        {
            try
            {
                setSoTimeout(timeout);
                int rbOnce = read(b, off + rb, len - rb);
                if (rbOnce < 0)
                {
                    unread(b, off, rb);
                    // Not sure that we should push back the bytes
                    // read so far or no; but there shouldn't be
                    // another read anyway. Unfortunately, there's no
                    // way to report the number of bytes read so far
                    // (although we could subclass EOFException to
                    // provide this).
                    throw new EOFException("Unexpected end-of-stream on " + in);
                }
                else
                {
                    rb += rbOnce;
                }
            }
            catch (InterruptedIOException ex)
            {
                // There are two choices of how to proceed: just
                // report the bytes that weren't transferred, or push
                // them all back and report that nothing was
                // transferred. We choose to report all the bytes that
                // were transferred because that doesn't require
                // extending our pushback buffer, although chances are
                // the caller wil call unread() to do this anyway.
                if (true)
                {
                    ex.bytesTransferred += rb;
                }
                else
                {
                    unread(b, off, rb + ex.bytesTransferred);
                    ex.bytesTransferred = 0;
                }
                throw ex;
            }
            if (timeout > 0 && rb < len)
            {
                long t2 = System.currentTimeMillis();
                timeout -= (int) (t2 - t1);
                t1 = t2;
                if (timeout <= 0)
                {
                    timeout = 1;
                }
            }
            
        }
    }
    /**
     * The pushback buffer (we can't use PushbackStream because it requires a
     * single fixed allocation and we don't know in general how much pushback
     * will be needed).
     * 
     * @see #unreadPos
     */
    protected byte[]     unreadBuf  = null;
    /**
     * The position within the pushback buffer from which the next byte will be
     * read. When the buffer is empty, <code>pos</code> is equal to
     * <code>buf.length</code>; when the buffer is full, <code>pos</code>
     * is equal to zero.
     * 
     * @see #unreadBuf
     */
    protected int        unreadPos  = 0;
    /**
     * Amount to increase the pushback buffer beyond what is needed (to avoid
     * repetitive small re-allocations).
     * 
     * @see #unreadBuf
     */
    protected static int extraAlloc = 1000;

    /**
     * Push back bytes to be read again.
     * 
     * If you are reading in multiple chunks (e.g., a header, then the body),
     * you should use {@link #getInputSynchronizer}to ensure that another
     * thread doesn't interfere.
     * 
     * @param b
     *            Bytes to push back.
     * @param off
     *            Offset with b of the bytes to be pushed back.
     * @param len
     *            Number of bytes to be pushed back.
     * @see PushbackInputStream#unread
     * @see #unreadBuf
     * @see #unreadPos
     * @see #extraAlloc
     * @see #getInputSynchronizer
     */
    public final void unread(byte[] b, int off, int len)
    {
        if (len == 0)
        {
            return;
        }
        if (unreadBuf == null)
        {
            unreadBuf = new byte[len + extraAlloc];
            unreadPos = unreadBuf.length;
        }
        else
            if (len > unreadPos)
            {
                int add = len - unreadPos + extraAlloc;
                byte[] newUnreadBuf = new byte[unreadBuf.length + add];
                System.arraycopy(unreadBuf, unreadPos, newUnreadBuf, unreadPos
                        + add, unreadBuf.length - unreadPos);
                unreadBuf = newUnreadBuf;
                unreadPos += add;
            }
        unreadPos -= len;
        System.arraycopy(b, off, unreadBuf, unreadPos, len);
    }

    /**
     * Read bytes from the input stream.
     * 
     * If you want to read in multiple chunks (e.g., a header, then the body),
     * you should use {@link #getInputSynchronizer}to ensure that another
     * thread doesn't interfere.
     * 
     * @param b
     *            array of bytes to be filled.
     * @param off
     *            offset within the array to start filling.
     * @param len
     *            maximum number of bytes to read.
     * @return number of bytes actually read.
     * @throws IOException
     *             if something goes wrong (or a timeout).
     * @see PushbackInputStream#read
     * @see #getInputSynchronizer
     */
    protected int read(byte[] b, int off, int len) throws IOException
    {
        // The following code is stolen shamelessly from
        // PushbackInputStream.read().
        if (len == 0)
        {
            return 0;
        }
        int avail = (unreadBuf == null) ? 0 : unreadBuf.length - unreadPos;
        if (avail > 0)
        {
            if (len < avail)
            {
                avail = len;
            }
            System.arraycopy(unreadBuf, unreadPos, b, off, avail);
            unreadPos += avail;
            off += avail;
            len -= avail;
        }
        if (len > 0)
        {
            try
            {
                len = in.read(b, off, len);
            }
            catch (InterruptedIOException e)
            {
                e.bytesTransferred += avail;
                throw e;
            }
            if (len == -1)
            {
                return avail == 0 ? -1 : avail;
            }
            return avail + len;
        }
        return avail;
    }

    /**
     * An object to use for synchronizing on inputs. This might be needed if you
     * do an intial {@link #readFully}to get a header, followed by another
     * {@link #readFully}to get the rest of the message.
     * 
     * @return The synchronization object.
     */
    public Object getInputSynchronizer()
    {
        return in;
    }

    /**
     * An object to use for synchronizing on outputs.
     * 
     * @return The synchronization object.
     */
    public Object getOutputSynchronizer()
    {
        return out;
    }

    /**
     * @see java.lang.Object#toString()
     */
    public final String toString()
    {
        String extra = "";
        if (socket == null)
        {
            extra = ", noSocket";
        }
        else
        {
            try
            {
                extra = ", linger=" + socket.getSoLinger() + ", noDelay="
                        + socket.getTcpNoDelay() + ", keepalive="
                        + socket.getKeepAlive();
            }
            catch (SocketException e)
            {
                extra = ", socket=" + socket + ", exception: " + e;
            }
        }
        String configStr = "";
        if (connectionConfig != null)
            configStr = connectionConfig.toString();
        return configStr + extra;
    }

    /**
     * Get the address and port as a printable string.
     * 
     * @return A string in the form "IPAddress:port"
     */
    public final String addrAndPortToString()
    {
        return getAddress().toString() + ":" + getPort();
    }

    /**
     * Get the ip address as an InetAddress object
     * 
     * @return InetAddress or null if the socket is null
     */
    public InetAddress getAddress()
    {
        if (socket != null)
            return socket.getInetAddress();
        return null;
    }

    /**
     * Get the remote port
     * 
     * @return remote port or 0 if the socket is null
     */
    public int getPort()
    {
        if (socket != null)
            return socket.getPort();
        return 0;
    }
}