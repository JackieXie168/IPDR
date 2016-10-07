#!/usr/local/bin/perl -w

# $Id: extractSnoopRecords.pl,v 1.1.1.1 2004/12/12 23:02:03 michaeld Exp $

# Read from a snoop file and produce output in chunks that
# interpretCRANE.pl can handle.
# (See RFC 1761 for the format of snoop files)
#
# This script reads from STDIN (*not* from a file name on the parameter line)
# and expects 2 parameters which give the sockets in the form IPaddr:port
# (you can specify an IPaddr of "*" and/or a port of "*"; for example:
#     extractSnoopRecords.pl 192.168.8.203:15000 '192.168.9.84:*'
#     extractSnoopRecords.pl 192.168.8.203:15000 '*:*'
# You should be careful with "*"s because if there are interleaved
# streams, you can confuse the downstream interpretation. "*"s are
# most useful for a series of sequential sessions.
#
# Typical use:
#   extractSnoopRecords.pl 192.168.8.203:15000 192.168.9.84:38750 <snoop.out | interpretCRANE.pl
# The main thing is that a buffer has to be kept of records, because
# it's possible to get something like this:
#    client -> server    (part of record)
#    server -> client    (record)
#    client -> server    (rest of record) (more records)
# and so we have to buffer up the record until it's complete.
# We use the message length field, which is bytes 4..7.
#
# This program is somewhat primitive ... amongst other things
# it doesn't take care of retransmitted or out of order packets.

use strict;
use Socket;

require 5.005;

my $socket1str = shift;
my $socket2str = shift;

die "Must give 2 sockets in form IPaddr:port" unless defined $socket1str && defined $socket2str;

($::socket1ip, $::socket1port) = &parseSocketStr($socket1str);
($::socket2ip, $::socket2port) = &parseSocketStr($socket2str);

die "Must give 2 sockets in form IPaddr:port (not $socket1str $socket2str)" unless defined $::socket1ip && defined $::socket1port && defined $::socket2ip && defined $::socket2port;

print STDERR "Sockets: ", &printableIP($::socket1ip,$::socket1port), ", ", &printableIP($::socket2ip,$::socket2port), "\n";

die "Input must be from stdin" unless $#ARGV < 0;

&readHeader;
&processRecords;

sub processRecords
{
    my %nextTcpSeqNo = (); # indexed by fromIP:port
    my %buffer = (); # holds partially constructed message, indexed by fromIP:port
    my %packetFlags = (); # hold union of flags for the message

    my $lastMsg = undef; # keep last value of $msg, for debugging
    my %lastBuffer = (); # keep last value of %buffer, for debugging

    for (my $packetNum = 0; ; $packetNum ++) {
        my $bytesToRead = 6*4;
        my $hdr;
        my $bytes = read(STDIN, $hdr, $bytesToRead);
        die "$? on input" unless defined $bytes;

        return if $bytes == 0; # end of file

        die "Couldn't read file header" unless $bytes == $bytesToRead;

        my ($originalLength, $includedLength, $packetRecordLength,
            $cumulativeDrops, $timestampSeconds, $timestampMicroseconds) =
                unpack("NNNNNN", $hdr);

        die "Can't handle truncated packets" if $originalLength > $includedLength;

        $bytesToRead = $packetRecordLength - $bytesToRead;
        my $framedPacket;
        $bytes = read(STDIN, $framedPacket, $bytesToRead) || die $?;

        die "Couldn't read packet of $packetRecordLength (less header: $bytesToRead bytes)" unless $bytes == $bytesToRead;

        $framedPacket = substr($framedPacket, 0, $includedLength); # ignore any padding
        
        my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday) = gmtime($timestampSeconds);
        $year += 1900;

        my $etherHdr = substr($framedPacket, 0, 14);
        my $etherHdrLength = 14;
        my $ipHdr = substr($framedPacket, $etherHdrLength, 20);

        my ($toMAC, $fromMAC, $ethernetType) = unpack("a6a6n", $etherHdr);
        my ($ipVersionHdrLength, $ipTOS, $ipLength,
            $ipID, $ipFlagsAndFragment1, $ipFlagsAndFragment2,
            $ipTTL, $ipProtocol, $ipHdrChksum,
            $ipSourceIP, $ipDestIP) =
                unpack("CCnnCCCCnNN", $ipHdr);
        my $ipVersion = $ipVersionHdrLength >> 4;
        my $ipHdrLength = ($ipVersionHdrLength - ($ipVersion << 4)) * 4;
        if ($ipVersion != 4) {
            warn "Not IP V4 ($ipVersion)";
            next;
        }
        die "Not IP 20-byte header ($ipHdrLength)" unless $ipHdrLength == 20;
        die "Not IP under Ethernet ($ethernetType)" unless $ethernetType == 0x800;
        my $ipFlagsAndFragment = (($ipFlagsAndFragment1 << 8) & 0xffff) +
            ($ipFlagsAndFragment2 & 0xff);
        my $ipFlags = $ipFlagsAndFragment1 >> 13;
        my $ipFragment = $ipFlagsAndFragment & 0x1fff;
        # ip flags: 0: must be zero  [RFC 791]
        #           1: may fragment
        #           2: more fragments
        die "Can't handle fragmentation ($ipFlagsAndFragment)" unless ($ipFlags & 5) == 0;

        warn "Can only handle TCP, not protocol $ipProtocol" unless $ipProtocol == 0x06
            || $ipProtocol == 0x01 # ICMP
                || $ipProtocol == 0x11 # UDP
                    ;
        next unless $ipProtocol == 0x06;

        my $tcpHdr = substr($framedPacket, $etherHdrLength+$ipHdrLength, 20);

        my ($tcpSourcePort, $tcpDestPort, $tcpSeqNo, $tcpAckNo,
            $tcpHdrLengthAndFlags, $tcpWindowSize,
            $tcpChksum, $tcpUrgentPtr) =
            unpack("nnNNnnnn", $tcpHdr);

        if ((($::socket1ip eq '*' || $::socket1ip == $ipSourceIP) && ($::socket1port eq '*' || $::socket1port == $tcpSourcePort) &&
             ($::socket2ip eq '*' || $::socket2ip == $ipDestIP)   && ($::socket2port eq '*' || $::socket2port == $tcpDestPort)) ||

            (($::socket2ip eq '*' || $::socket2ip == $ipSourceIP) && ($::socket2port eq '*' || $::socket2port == $tcpSourcePort) &&
             ($::socket1ip eq '*' || $::socket1ip == $ipDestIP)   && ($::socket1port eq '*' || $::socket1port == $tcpDestPort))) {
            
            my $tcpHdrLength = ($tcpHdrLengthAndFlags >> 12) * 4;
            my $dataLength = $ipLength - $ipHdrLength- $tcpHdrLength;
            my $packetData = substr($framedPacket, $etherHdrLength + $ipHdrLength + $tcpHdrLength, $dataLength);
            my $sourceIpPort = "$ipSourceIP:$tcpSourcePort";
            my $expectedSeqNo = $nextTcpSeqNo{$sourceIpPort};

            $packetFlags{$sourceIpPort} |= ($tcpHdrLengthAndFlags & 0xff);
            my $FINflag = ($tcpHdrLengthAndFlags & 0x01) ? 1 : 0;
            my $SYNflag = ($tcpHdrLengthAndFlags & 0x02) ? 1 : 0;
            my $RSTflag = ($tcpHdrLengthAndFlags & 0x04) ? 1 : 0;
            my $PSHflag = ($tcpHdrLengthAndFlags & 0x08) ? 1 : 0;
            my $ACKflag = ($tcpHdrLengthAndFlags & 0x10) ? 1 : 0;
            my $URGflag = ($tcpHdrLengthAndFlags & 0x20) ? 1 : 0;

            if (1) {
                print STDERR $packetNum;
                print STDERR " from(", &printableIP($ipSourceIP, $tcpSourcePort), ")";
                print STDERR " to(", &printableIP($ipDestIP, $tcpDestPort), ")";
                print STDERR " seq#=$tcpSeqNo ack#=$tcpAckNo",
                print STDERR " dataLength=", sprintf("%4d", length($packetData));
                print STDERR " FIN" if $FINflag;
                print STDERR " SYN" if $SYNflag;
                print STDERR " RST" if $RSTflag;
                print STDERR " PSH" if $PSHflag;
                print STDERR " ACK" if $ACKflag;
                print STDERR " URG" if $URGflag;
                print STDERR "\n";
            }

            if (defined $expectedSeqNo && ! $SYNflag && $expectedSeqNo != $tcpSeqNo) {
                print STDERR "Bad TCP seq $tcpSeqNo, expected $expectedSeqNo between " .
                    &printableIP($ipSourceIP,$tcpSourcePort) . " and " .
                        &printableIP($ipDestIP,$tcpDestPort);
                if ($tcpSeqNo <= $expectedSeqNo) {
                    print STDERR " ... skipping initial " . ($expectedSeqNo - $tcpSeqNo) . " bytes out of " . length($packetData) . "\n";
                    if (length($packetData) > 0) {
                        $packetData = substr($packetData, $expectedSeqNo - $tcpSeqNo);
                        print STDERR "                         shortened to " . length($packetData) . "\n";
                    } else {
                        print STDERR "                         left at " . length($packetData) . "\n";
                    }
                    print STDERR "framedPacket: ", &hexedString($framedPacket), "\n";
                    print STDERR "packetData: ", &hexedString($packetData), "\n";
                    print STDERR "buffer: ", &hexedString($buffer{$sourceIpPort}), "\n";
                    print STDERR "lastMsg: ", &hexedString($lastMsg), "\n";
                    print STDERR "lastBuffer: ", &hexedString($lastBuffer{$sourceIpPort}), "\n";
                } else {
                    print STDERR " !!!! OUT OF ORDER !!!\n";
                    print STDERR " !!!! OUT OF ORDER !!!\n";
                    print STDERR " !!!! OUT OF ORDER !!!\n";
                }
            }

            $nextTcpSeqNo{$sourceIpPort} = $tcpSeqNo + $dataLength;
            if ($SYNflag || $FINflag) {
                $nextTcpSeqNo{$sourceIpPort} += 1; #SYN at beginning consumes 1 seq no
            }

            $buffer{$sourceIpPort} .= $packetData;
            $lastBuffer{$sourceIpPort} = $buffer{$sourceIpPort}; # for debugging

            for (;;) {
                # print STDERR "buffer (", length($buffer{$sourceIpPort}), " bytes): ", &hexedString($buffer{$sourceIpPort}), "\n";
                last unless length($buffer{$sourceIpPort}) >= 8;
                my ($msgVersion, $msgID, $msgSessionID, $msgFlag, $msgLength) = unpack("CCCCN", $buffer{$sourceIpPort});
                if ($msgVersion != 1) {
                    print STDERR "Packet length=$includedLength drop=$cumulativeDrops: ";
                    print STDERR "$year-$mon-${mday}T$hour:$min:$sec.";
                    printf STDERR "%06d", $timestampMicroseconds;
                    print STDERR " from(", &printableMAC($fromMAC), ")";
                    print STDERR " to(", &printableMAC($toMAC), ")";
                    printf STDERR " type=%04x", $ethernetType;
                    print STDERR " protocol=$ipProtocol ipLength=$ipLength";
                    print STDERR " from(", &printableIP($ipSourceIP, $tcpSourcePort), ")";
                    print STDERR " to(", &printableIP($ipDestIP, $tcpDestPort), ")";
                    print STDERR " seq#=$tcpSeqNo ack#=$tcpAckNo",
                    print STDERR " dataLength=", sprintf("%3d", length($packetData));
                    print STDERR " FIN" if $FINflag;
                    print STDERR " SYN" if $SYNflag;
                    print STDERR " RST" if $RSTflag;
                    print STDERR " PSH" if $PSHflag;
                    print STDERR " ACK" if $ACKflag;
                    print STDERR " URG" if $URGflag;
                    print STDERR "\n";
                    print STDERR "framedPacket: ", &hexedString($framedPacket), "\n";
                    print STDERR "packetData: ", &hexedString($packetData), "\n";
                    print STDERR "buffer: ", &hexedString($buffer{$sourceIpPort}), "\n";
                    print STDERR "lastMsg: ", &hexedString($lastMsg), "\n";
                    print STDERR "lastBuffer: ", &hexedString($lastBuffer{$sourceIpPort}), "\n";
                    die "Bad version ($msgVersion) [msgID=$msgID sessionID=$msgSessionID flag=$msgFlag] pktFlags=$packetFlags{$sourceIpPort}";
                }
                die "Bad length ($msgLength)" unless $msgLength > 0;
                last unless ($msgLength <= length($buffer{$sourceIpPort}));
                # print STDERR "Version=$msgVersion msgID=$msgID sessionID=$msgSessionID flag=$msgFlag length=$msgLength\n";
                # print STDERR "\t\t", &hexedString(substr($buffer{$sourceIpPort}, 0, $msgLength)), "\n";
                my $msg = substr($buffer{$sourceIpPort}, 0, $msgLength);
                $lastMsg = $msg; # for debugging
                my $pFlag = (defined $packetFlags{$sourceIpPort}) ? $packetFlags{$sourceIpPort} : 0;
                print STDOUT pack("NNNNNNNN", $timestampSeconds, $timestampMicroseconds, $ipSourceIP, $tcpSourcePort, $ipDestIP, $tcpDestPort, $pFlag, $packetNum) . $msg;
                if ($msgLength == length($buffer{$sourceIpPort})) {
                    $buffer{$sourceIpPort} = "";
                } else {
                    $buffer{$sourceIpPort} = substr($buffer{$sourceIpPort}, $msgLength);
                }
                if (length($packetData) > 0) {
                    $packetFlags{$sourceIpPort} = 0;
                }
            }
        } # It's a socket that we care about

    }
}

sub printableMAC
{
    my ($MAC) = @_;
    return join(":", map(sprintf("%02x", ord($_)), split('', $MAC)));
}

sub printableIP
{
    my ($IP, $port) = @_;
    return ($IP eq '*' ? '*' : inet_ntoa(pack("N",$IP))) . ":" . $port;
}

sub readHeader
{
    my $hdr;
    my $bytesToRead = 8+4+4;
    my $bytes = read(STDIN, $hdr, $bytesToRead) || die $?;

    die "Couldn't read file header" unless $bytes == $bytesToRead;
    my ($id, $version, $datalinkType) = unpack("Z8NN", $hdr);
    # print STDERR "$bytes bytes: id($id) version($version) datalink($datalinkType)\n";
    die "Not Snoop version 2 file" unless $id eq "snoop" && $version == 2;
    die "Can only handle Ethernet (datalink=$datalinkType)" unless $datalinkType == 4;
}

sub parseSocketStr
{
    my ($str) = @_;

    my ($ipStr, $portStr, @junk) = split(/:/, $str);

    return (undef,undef) unless defined $ipStr && defined $portStr && $#junk == -1;

    my $ip = ($ipStr eq '*') ? '*' : unpack("N",inet_aton($ipStr));

    return ($ip, '*') if $portStr eq '*';
    return ($ip, undef) unless $portStr =~ /^[0-9]+$/;
    return ($ip, $portStr+0);
}

sub hexedString
{
    my ($str) = @_;
    return join(" ", map(sprintf("%02x",ord($_)), split(//, $str)));
}
