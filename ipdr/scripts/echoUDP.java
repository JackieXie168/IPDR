// $Id: echoUDP.java,v 1.2 2004/12/13 19:57:51 dmitryk Exp $
// Copyright (c) 2002 XACCT Technologies Ltd., All Rights Reserved

import java.net.*;
import java.io.*;

/** A trivial program for echoing all the UDP packets that come to a
 *  port. To run it:
 *  <br>
 *  <xmp>
 *      javac echoUDP.java
 *      java echoUDP <portnum>
 *  </xmp>
 *  <br>where typically <portnum> will be 514 (syslog port).
 *
 *   This program is useful for getting the logging output from the
 *   IPDR client (trasnmitter) when you don't have a syslog facility
 *   available and you don't want to be bothered with installing
 *   something fancier on your machine.
 *
 *   The output is one line per UDP packet, with the line being
 *   prefixed by the machine that the packet came from.
 */

class echoUDP
{
    public static String versionStr() { return "@(#) $Id: echoUDP.java,v 1.2 2004/12/13 19:57:51 dmitryk Exp $"; }

    /// The main entry point
    public static void main(String arg[]) {

        if (arg.length != 1) {
            System.err.println("java echoUDP <port number>");
            System.exit(1);
        }
        
        int port = 0;

        try {
            port = Integer.parseInt(arg[0].trim());
        } catch(NumberFormatException e) {
            System.err.println("echoUDP: invalid port number(" + arg[0].trim() + "): " + e);
            System.exit(1);
        }

        listenOnPort(port);
    }

    /// Listen on a UDP port and echo everything that comes in.

    private static void listenOnPort(int port) {

        System.err.println("Listening on port " + port);

        try {
            DatagramSocket socket = new DatagramSocket(port);
            byte [] receiveBuf = new byte[0xffff+1];
            DatagramPacket receivePacket = new DatagramPacket(receiveBuf, receiveBuf.length);
            for (;;) {
                socket.receive(receivePacket);
                // amazingly inefficient code follows (would use StringBuffer,
                // but it doesn't have such a convenient constructor)
                String line = new String(receiveBuf, 0, receivePacket.getLength());
                // Trim whichspace from the right (also, amazingly inefficient code).
                // (String.trim() removes both left and right whitespace).
                while (line.length() > 0 &&
                       Character.isWhitespace(line.charAt(line.length()-1))) {
                    line = line.substring(0, line.length() - 1);
                }
                System.out.println(receivePacket.getAddress() + ": " + line);
            }
        } catch (IOException e) {
            System.err.println("Reading on port " + port + " got exception: " + e);
        }
    }
}
