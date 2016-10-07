package com.xacct.CRANE.collector;

// $Id: ExtendedInetAddress.java,v 1.4 2005/01/30 15:13:43 ilyad Exp $
// Copyright (2005) (c) Amdocs, All Rights Reserved
//This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND either expresses or implied.

import java.net.InetAddress;
import java.net.UnknownHostException;

/** Extension of InetAddress, with different conventions for name
 *  lookup. This makes handling null values in the configuration file
 *  easier. We need this different convention because we sometimes
 *  send an IP address to the other side, and "127.0.0.1" needs to
 *  be converted to the actual IP address of the machine.
 *  <p>
 *  XXX
 *  This ought to be subclass of InetAddress, but unfortunately that is
 *  a "final" class in JDKs before 1.4, so we do it as a delegation class
 *  (and a bit of other pain).
 *
 *  @version $Id: ExtendedInetAddress.java,v 1.4 2005/01/30 15:13:43 ilyad Exp $
 */
public final class ExtendedInetAddress /* extends InetAddress */
    implements Comparable, java.io.Serializable
{
    /** $Id: ExtendedInetAddress.java,v 1.4 2005/01/30 15:13:43 ilyad Exp $ */
    public static String versionStr() { return "@(#) $Id: ExtendedInetAddress.java,v 1.4 2005/01/30 15:13:43 ilyad Exp $"; }

    /// The object used for delegation (can be null, from the constructor!)
    protected InetAddress self;

    /// Get the InetAddress
    public InetAddress get() { return self; }

    /** Create from an InetAddress.  This is not normally used: better
     *  to use the form that takes a string, and which does proper
     *  things with null.
     *  @see #ExtendedInetAddress(String)
     * @param self The IP address (may be null)
     */
    protected ExtendedInetAddress(InetAddress self) throws UnknownHostException {
        this.self = self;
        fixAddress();
    }

    /** Create from a host-name string.
     *  For defaulting: use localHost rather than 127.0.0.1 wherever possible
     *  (null, "", 0.0.0.0, 127.0.0.1)
     *  @param string host The specified host with null, "", or "0.0.0.0", or "127.0.0.1" being
     *                used for the local host. The string is trimmed of blanks
     *                before being used.
     * @throws UnknownHostException If a local address is passed in
     * and local host lookup fails.
     */
    public ExtendedInetAddress(String host) throws UnknownHostException {
        String trimmedHost = CollectorUtil.nonNull(host);
        if (trimmedHost.length() == 0) { host = null; } // getByName wants this
        this.self = InetAddress.getByName(trimmedHost); // local host if trimmedHost == null
        fixAddress();
    }

    /** If the address refers to something local (e.g., 127.0.0.1),
     *  try to change it to the host address.  This should not be
     *  relied on 100% because of indeterminism with multi-homed
     *  machines.
     */
    protected void fixAddress() throws UnknownHostException {
        byte [] addr = this.self.getAddress();
        // Check for magic addresses: 0.0.0.0, 127.0.0.1
        // XXX With JDK 1.4, use isLoopbackAddress()
        //   ... also look at isAnyLocalAddress(), isLinkLocalAddress(),
        //       isSiteLocalAddress().
        if (addr.length == 4 &&
            (addrEq(addr, 0, 0, 0, 0) ||
             addrEq(addr, 127, 0, 0, 1))) {
            this.self = InetAddress.getLocalHost();
        }
    }

    // for debugging, isn't fully generalhttp://www.greggman.com/restaurants/ja/2002-12-01-jangara.htm
    private static String addrToString(InetAddress iAddr) {
        byte [] addr = iAddr.getAddress();
        return "InetAddress[bytes(" + (addr[0]&0xff)  + "." + (addr[1]&0xff) + "." +
            (addr[2]&0xff) + "." + (addr[3]&0xff) +
            "): " + iAddr.toString() +
            " hostName(" + iAddr.getHostName() + ")" +
            " hostAddr " + iAddr.getHostAddress() + "]";
    }

/** Check that a 4-byte address is equal to a list of 4 integers (a convenience function).
 * @param addr The address to be compared
 * @param a0 1st byte
 * @param a1 2nd byte
 * @param a2 3rd byte
 * @param a3 4th byte
 * @return Whether they address's 4 bytes match the 4 numbers given to
 * this function.
 */
    private static final boolean addrEq(byte [] addr, int a0, int a1, int a2, int a3) {
        return (addr[0]&0xff) == a0 && (addr[1]&0xff) == a1 &&
            (addr[2]&0xff) == a2 && (addr[3]&0xff) == a3;
    }

/** @see #compareTo(InetAddress)
 */
    public final int compareTo(Object o) {
        return compareTo((InetAddress)o);
    }

/** @see #compareTo(InetAddress)
 */
    public final int compareTo(ExtendedInetAddress o) {
        return compareTo(self);
    }

    /** Compare two InetAddress's for sorting purposes.  Just like first.compareTo(second), if InetAddress supported compareTo()
     * @param other The other address.
     * @return Negative, zero, positive integer according to wither this < = > other.
     * @see Comparable#compareTo
     */
    public int compareTo(InetAddress other) {
        if (self == other) { return 0; }
        if (self == null && other == null) { return 0; }
        if (self == null) { return -1; }
        if (other == null) { return 1; }
        // from here, we're guaranteed that self != null && other != null
        byte [] sbThis = self.getAddress();
        byte [] sbOther = other.getAddress();
        int result = 0;
        for (int i = 0; i < sbThis.length && i < sbOther.length && result == 0; i++) {
            result = sbThis[i] - sbOther[i];
        }
        if (result == 0) {
            result = sbThis.length - sbOther.length;
        }
        return result;
    }

    public String getHostName() { return self.getHostName(); }
    // XXX Needs JDK 1.4: public String getCanonicalHostName() { return self.getCanonicalHostName(); }
    public String getHostAddress() { return self.getHostAddress(); }

    // Do <em>not</em> use toString if you want just a host name or IP
    // address ...  use getHostName(), getHostAddress(), or
    // getCanonicalHostName() instead.
    public String toString() {
        String result = self.toString();
        if (result.startsWith("/")) {
            result = result.substring(1);
        }
        // You should probably use getHostAddress() if you want the following result:
        //         int index = result.indexOf("/");
        //         if (index >= 0) {
        //             result = result.substring(index+1);
        //         }
        return result;
    }

    public int hashCode() { return self.hashCode(); }

    public boolean equals(Object other) {
        try {
            return equals((ExtendedInetAddress)other);
        } catch (ClassCastException e) {
            try {
                return equals((InetAddress)other);
            } catch (ClassCastException e2) {
                return false;
            }
        }
    }

    public boolean equals(ExtendedInetAddress other) {
        return this == other || (self != null && other != null && self.equals(other.self)); }
    public boolean equals(InetAddress other) {
        return self != null && other != null && self.equals(other);
    }

    public String andPortToString(int port) {
        return andPortToString(toString(), port);
    }

    public static String andPortToString(String addr, int port) {
        return addr + ":" + port;
    }

    public static String andPortToString(InetAddress addr, int port) {
        return addr + ":" + port;
    }
}
