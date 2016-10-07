/** @file IPDRsimAux.cpp
 *
 * Auxilliary functions for IPDRsim.cpp
 *
 * Copyright (2005) © Amdocs, All Rights Reserved
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND either expresses or implied.
 *
 * $Id: IPDRsimAux.cpp,v 1.7 2005/01/30 15:41:26 guyg Exp $
 *
 * None of the code here is brilliantly efficient; just simple.
 *
 * @note XXX This file is assumed to be included by IPDRsim.cpp.
 */

const char IPDRsimAux_rcsid[] = "@(#) $Id: IPDRsimAux.cpp,v 1.7 2005/01/30 15:41:26 guyg Exp $";

#include <string>
#include <vector>

/** Trim leading and trailing blanks from a string
 *  @param in The string to be trimmed
 *  @return The string with leading and trailing blanks removed
 */
string trim(const string& in)
{
    string::size_type p0 = in.find_first_not_of(" ");
    if (p0 == string::npos) {
        return "";
    }
    string::size_type p2 = in.find_last_not_of(" ");
    return in.substr(p0, p2-p0+1);
}


/** Split a string using a delimiter and trim the resulting two strings
 *  @param in The string to be split
 *  @param match The delimiter (usually a single character, but not necessarily)
 *  @param left (updated) pointer to a string that gets the trimmed part to the left of (before) the delimiter
 *  @param right (updated) pointer to a string that gets the trimmed part to the right of (after) the delimiter
 */
void splitStr(const string& in,
              const string& match,
              string* left,
              string* right)
{
    string::size_type p = in.find(match);
    if (p == in.npos) {
        *left = trim(in);
        *right = "";
    } else {
        *left = trim(in.substr(0, p));
        *right = trim(in.substr(p+1));
    }
}


/** Tokenize a string into a vector of tokens, using blanks and tabs for delimiters
 *  @param in The string to tokenize
 *  @param tokens (updated) a vector of strings (the tokens)
 */
void tokenizeStr(const string& in, vector<string>& tokens)
{
    string str = in;
    const string splitters = " \t";
    tokens.clear();
    while (str.length() > 0) {
        string::size_type p = str.find_first_not_of(splitters);
        if (p == str.npos) {
            break;
        }
        string::size_type p2 = str.find_first_of(splitters, p);
        if (p2 == str.npos) {
            tokens.push_back(str.substr(p));
            break;
        }
        tokens.push_back(str.substr(p, p2-p));
        str = str.substr(p2);
    }
}

//  #include <sys/types.h>
//  #include <sys/socket.h>
//  #include <netinet/in.h>
#include <arpa/inet.h>


/** Encapsulate a host IP address and port with some convience methods
 */
class IPandPort
{
public:
    /** the host as a string (dotted-number notation) */
    string hostStr;

    /** the port as a string (number) */
    string portStr;

    /** Construct from a host and port as strings
     *  @param theHost the host as a string (dotted-number notation)
     *  @param thePort the port as a string (number)
     */
    IPandPort(const string& theHost, const string& thePort)
        : hostStr(theHost),
          portStr(thePort)
    { }

    /** Construct from a host and port as a single host:port string
     *  @param hostAndPort the host and port as string dotted-number:port
     */
    IPandPort(const string& hostAndPort)
    {
        splitStr(hostAndPort, ":", &hostStr, &portStr);
    }

    /** The type of a numeric IP address */
    typedef unsigned long IP_t;

    /** The type of a numeric port */
    typedef unsigned short Port_t;

    /** Turn the host string into the numeric form
     *  @return host as a number in network byte order
     */
    IP_t hostIPnum() const
    {
        return ntohl(inet_addr(hostStr.c_str()));
    }

    /** Turn the port string into the numeric form
     *  @return port as a number in internal order
     */
    Port_t portNum() const {
        return atoi(portStr.c_str()); // ought to check for validity also
    }

    /** Turn a numeric IP and port into a string
     *  @param addr the numeric form of the IP address
     *  @param port the port
     *  @return string in the form "xx.xx.xx.xx:yy"
     */
    static string asString(IP_t addr, Port_t port) {
        char addressStr[40];
        sprintf(addressStr, "%lu.%lu.%lu.%lu:%d",
                (addr >> 24) & 0xff,
                (addr >> 16) & 0xff,
                (addr >>  8) & 0xff,
                (addr >>  0) & 0xff,
                port);
        return string(addressStr);
    }
};


/** Convenience type: vector of IP:port values
 */
typedef vector<IPandPort> IPandPortList;

/** Given a list of host:port,host:port,... produce a vector of host/port objects
 *  @param in The input string as "host:port,host:port,..."
 *  @param list (updated) vector of host/port objects
 */
void processIPandPortStr(const string& in,
                         IPandPortList& list)
{
    string str = in;
    while (str.length() > 0) {
        string piece;
        splitStr(str, ",", &piece, &str);
        list.push_back(IPandPort(piece));
    }
}

/** Display an OID as a string
 *  @param oid The OID to display.
 *  @return A string with the OID in dotted notation.
 */
string OIDstr(const IPDR_OID& oid)
{
    string result = "";
    const char *dot = "";
    for (int i = 0; i < oid.Length; i++) {
        char buf[100];
        sprintf(buf, "%s%d", dot, oid.OIDArray[i]);
        result += buf;
        dot = ".";
    }
    return result;
}
