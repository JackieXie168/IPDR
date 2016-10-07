#!/usr/local/bin/perl -w

# $Id: interpretIPDR.pl,v 1.1.1.1 2004/12/12 23:02:03 michaeld Exp $

# Read stuff from output of extractSnoopRecords.pl

# (There's a flag here, whether the timestamp is on each record or not
# ... extractSnoopRecords.pl puts on a seconds/microseconds timestamp
# ... ethereal "save as" doesn't)

$::hasTimestamps = 1; # set to 0 if from ethereal

# IMPORTANT ... this expects input from STDIN, not from the file
# given on the command line (maybe I'll fix this in the future).

# To take a trace and interpret it, do this
#    (10.0.0.1 is ISM and 10.0.0.2 is CRANE client)
#   snoop -o snoop.out '(from 10.0.0.1 and to 10.0.0.2) or (to 10.0.0.1 and from 10.0.0.2)'
# which puts output in file snoop.out
#
# You can use extractSnoopRecords.pl for generating input to
# this program. This requires first looking at the snoop file with ethereal
# and figuring out what the IP addresses and ports are.
#
# You can then run this script on the saved files.
#
# If you want a more-or-less readable snoop output, do:
#     snoop -i snoop.out -x54 -V

# Implementation note: For historical reasons, this code converts the
# stream to a list of 2-digit hex nubers. This makes the code a little
# less understandable. Sorry, but it does make debugging a bit easier;
# and if you want to modify this to work with snoop's "-x" output, it
# should be pretty easy.

# There are a few omissions here; in particular, deleted fields aren't handled
# properly (it'll probably die on a wrong data record length).


# (Instructions for Ethereal)

# Start ethereal (you can download it for free from http://www.ethereal.com)
# Open the snoop file (snoop.out)
# Find a section of CRANE traffic (typically, there is a UDP exchange ahead of it)
# Click on one of the packets
# Menu: Tools -> Follow TCP Stream
# "Save As" doesn't work properly with this program [and probably never did because
#  of the problems with conversations overlapping].
# Reset (button is at bottom of Ethereal window) and repeat for other CRANE conversations.

use strict;
use Socket;

require 5.005;

die "Input must be from stdin" unless $#ARGV < 0;

&initMsgId();

# %::templateKey uses key "session:template:key" mapping to "type:attr:name:label:help"
#   (attr is as 4 hex digits, e.g. "00 00 00 00")
#   [actually, the mapped values are separated by "\t"].

%::templateKey = ();

%::templateName = (); # keeps names of templates
%::templateLength = (); # keeps lengths of templates

$::bytesAtOneTime = 10000; # needs to be longer than the longest possible message

my $bytesTotal = 0;

my $moreInput = 1;

my @bytes = (); # &readBytes;

my $recordNum = 0;

while ($moreInput || $#bytes >= 0) {
    if ($#bytes < $::bytesAtOneTime && $moreInput) {
        my @bytes2 = &readNbytes();
        $moreInput = ($#bytes2 >= 0);
        # print "--- after read, moreInput=$moreInput, bytes/bytes2=", ($#bytes+1), "/", ($#bytes2+1), "\n";
        push @bytes, @bytes2;
        # print "--- after read, moreInput=$moreInput, bytes/bytes2=", ($#bytes+1), "/", ($#bytes2+1), "\n";
    }
    # print "--- bytes: ", ($#bytes+1), "\n";
    my ($timestampSeconds, $timestampMicroseconds, $ipSourceIP, $tcpSourcePort, $ipDestIP, $tcpDestPort, $packetFlags, $packetNum) =
        (0, 0, 0, 0, 0, 0, 0, 0);
    if ($::hasTimestamps) {
        $timestampSeconds = &longStrToInt(@bytes[0..3]);
        $timestampMicroseconds = &longStrToInt(@bytes[4..7]);
        $ipSourceIP = &longStrToInt(@bytes[8..11]);
        $tcpSourcePort = &longStrToInt(@bytes[12..15]);
        $ipDestIP = &longStrToInt(@bytes[16..19]);
        $tcpDestPort = &longStrToInt(@bytes[20..23]);
        $packetFlags = &longStrToInt(@bytes[24..27]);
        $packetNum = &longStrToInt(@bytes[28..31]);
        splice @bytes, 0, 32; # @bytes = @byte[32..$#bytes];
    }
    last if $#bytes < 0;
    my $msgID = &byteStrToInt($bytes[1]);
    my $length = &longStrToInt(@bytes[4..7]);
    my $session = &byteStrToInt($bytes[2]);
    die join(":", @bytes) if $length < 8;
    print "$recordNum\t"; # join(':', @bytes), "\n";
    print "version($bytes[0])";
    print " session($session)";
    printf " msgID(%d=0x%x)", $msgID, $msgID;
    print " msgFlags($bytes[3])";
    print " length($length) at offset $bytesTotal";
    print " ", &printableIP($ipSourceIP, $tcpSourcePort), " -> ", &printableIP($ipDestIP, $tcpDestPort);
    if ($packetFlags & 0x01) { print " FIN"; }
    if ($packetFlags & 0x02) { print " SYN"; }
    if ($packetFlags & 0x04) { print " RST"; }
    if ($packetFlags & 0x08) { print " PSH"; }
    if ($packetFlags & 0x10) { print " ACK"; }
    if ($packetFlags & 0x20) { print " URG"; }
    print " [pkt $packetNum]";
    $bytesTotal += $length;
    if ($timestampSeconds != 0 || $timestampMicroseconds != 0) {
        my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday) = gmtime($timestampSeconds);
        $year += 1900;
        $mon += 1;
        printf " %04d-%02d-%02dt%02d:%02d:%02d.%06d", $year, $mon, $mday, $hour, $min, $sec, $timestampMicroseconds;
    }
    print "\n";

    my $msgSubr = $::MsgID[$msgID];
    die "Unknown MsgID: $msgID" unless defined $msgSubr;
    &$msgSubr($length, $session, $timestampSeconds, $timestampMicroseconds, @bytes);
    print "\n\n";
    splice @bytes, 0, $length; # @bytes = @bytes[$length..$#bytes];
    $recordNum ++;
}

exit 0;

sub byteStrToInt
{
    my ($str) = @_;
    # my $b0 = substr($str, 0, 1);
    # my $b1 = substr($str, 1, 1);
    # return $b0 * 16 + $b1;
    die unless defined $str;
    die "byteStrToInt wrong length($str)" unless length($str) == 2;
    return eval("0x$str");
}

sub shortStrToInt
{
    my (@str) = @_;
    die ("shortStrToInt wrong size (" . join(":", @str) . ")") unless $#str == 1;
    return (&byteStrToInt($str[0])<<8) + &byteStrToInt($str[1]);
}

sub longStrToInt
{
    my (@str) = @_;
    die ("longStrToInt wrong size (" . join(":", @str) . ")") unless $#str == 3;
    return
        (&byteStrToInt($str[0])<<24) +
        (&byteStrToInt($str[1])<<16) +
        (&byteStrToInt($str[2])<<8) +
        &byteStrToInt($str[3]);
}

sub longLongStrToInt
{
    my (@str) = @_;
    die ("longStrToInt wrong size (" . join(":", @str) . ")") unless $#str == 7;
    return join(":", @str);

    # doesn't work ... need to make it 64-bit arithmetic:
    return
        (&byteStrToInt($str[0])<<56) +
        (&byteStrToInt($str[1])<<48) +
        (&byteStrToInt($str[2])<<40) +
        (&byteStrToInt($str[3])<<32);
        (&byteStrToInt($str[4])<<24) +
        (&byteStrToInt($str[5])<<16) +
        (&byteStrToInt($str[6])<<8) +
        &byteStrToInt($str[7]);
}

sub longStrToIntN1
{
    my (@str) = @_;
    my $result = &longStrToInt(@str);
    if ($result == 4294967295) {
        $result = -1;
    }
    return $result;
}

sub readBytes
{
    my $input = join('', <>);

    my @bytes = ();
    {
        for (my $i = 0; $i < length($input); $i++) {
            my $c = substr($input, $i, 1);
            push @bytes, sprintf('%02x', ord($c));
        }
    }
    return @bytes;
}

sub readNbytes
{
    my $input = '';
    my $byteCount = read(STDIN, $input, $::bytesAtOneTime * 2);
    die "Read error: $!" unless defined $byteCount;
    # print "--- read $byteCount bytes ---\n";
    my @bytes = ();
    {
        for (my $i = 0; $i < $byteCount; $i++) {
            my $c = substr($input, $i, 1);
            push @bytes, sprintf('%02x', ord($c));
        }
    }
    return @bytes;
}

sub initMsgId
{
    @::MsgID = ();
    $::MsgID[0x0000] = \&MsgID0000;
    $::MsgID[0x0001] = \&MsgID0001;
    $::MsgID[0x0002] = \&MsgID0002;
    $::MsgID[0x0003] = \&MsgID0003;
    $::MsgID[0x0004] = \&MsgID0004;
    $::MsgID[0x0005] = \&MsgID0005;

    $::MsgID[0x0010] = \&MsgID0010;
    $::MsgID[0x0011] = \&MsgID0011;
    $::MsgID[0x0012] = \&MsgID0012;
    $::MsgID[0x0013] = \&MsgID0013;
    $::MsgID[0x0014] = \&MsgID0014;
    $::MsgID[0x0015] = \&MsgID0015;
    $::MsgID[0x0016] = \&MsgID0016;
    $::MsgID[0x0017] = \&MsgID0017;
    $::MsgID[0x0018] = \&MsgID0018;
    $::MsgID[0x0019] = \&MsgID0019;

    $::MsgID[0x0020] = \&MsgID0020;
    $::MsgID[0x0021] = \&MsgID0021;
    $::MsgID[0x0023] = \&MsgID0023;
    $::MsgID[0x0024] = \&MsgID0024;

    $::MsgID[0x0030] = \&MsgID0030;
    $::MsgID[0x0031] = \&MsgID0031;
}

sub MsgHdrXXX
{
    my ($timestampSeconds, $timestampMicroseconds) = @_;
    my $result = "\t\t*-* ";
    return $result;
}

sub MsgID0000
{
    my ($length, $session, $timestampSeconds, $timestampMicroseconds, @bytes) = @_;
    print &MsgHdrXXX($timestampSeconds, $timestampMicroseconds), "(reserved)";
}

sub MsgID0001
{
    my ($length, $session, $timestampSeconds, $timestampMicroseconds, @bytes) = @_;
    print &MsgHdrXXX($timestampSeconds, $timestampMicroseconds), "START";
}

sub MsgID0002
{
    my ($length, $session, $timestampSeconds, $timestampMicroseconds, @bytes) = @_;
    print &MsgHdrXXX($timestampSeconds, $timestampMicroseconds), "START ACK";
    my $bootTimeRaw = &longStrToInt(@bytes[8..11]);
    my $bootTime = gmtime($bootTimeRaw);
    printf " bootTime (%d : 0x%x) ... %s", $bootTimeRaw, $bootTimeRaw, $bootTime;
}

sub MsgID0003
{
    my ($length, $session, $timestampSeconds, $timestampMicroseconds, @bytes) = @_;
    print &MsgHdrXXX($timestampSeconds, $timestampMicroseconds), "STOP";
}

sub MsgID0004
{
    my ($length, $session, $timestampSeconds, $timestampMicroseconds, @bytes) = @_;
    print &MsgHdrXXX($timestampSeconds, $timestampMicroseconds), "STOP ACK";
}

sub MsgID0005
{
    my ($length, $session, $timestampSeconds, $timestampMicroseconds, @bytes) = @_;
    print &MsgHdrXXX($timestampSeconds, $timestampMicroseconds), "CONNECT server: ", &byteStrToInt($bytes[8]), ".",
    &byteStrToInt($bytes[9]), ".",
    &byteStrToInt($bytes[10]), ".",
    &byteStrToInt($bytes[11]), " port ", &shortStrToInt(@bytes[12..13]);
}


sub MsgID0010
{
    my ($length, $session, $timestampSeconds, $timestampMicroseconds, @bytes) = @_;
    my $endianFlag = &extractBit(&byteStrToInt($bytes[9]), 0);
    my $endianMsg = ($endianFlag ? "big" : "little");
    my $numTemplates = &shortStrToInt(@bytes[10..11]);
    print &MsgHdrXXX($timestampSeconds, $timestampMicroseconds), "TMPL DATA config(", &byteStrToInt($bytes[8]),
    ") flags($bytes[9]) E($endianFlag:$endianMsg) #templates($numTemplates)";
    &msgIDtemplates(0, $session, $numTemplates, $length-12, @bytes[12..$#bytes]);
    die "Can't handle little-endian" unless $endianFlag == 1;
}

sub MsgID0011
{
    my ($length, $session, $timestampSeconds, $timestampMicroseconds, @bytes) = @_;
    my $numTemplates = &shortStrToInt(@bytes[10..11]);
    print &MsgHdrXXX($timestampSeconds, $timestampMicroseconds), "TMPL DATA ACK config(", &byteStrToInt($bytes[8]),
    ") #templateChanges($numTemplates)";

    my $pos = 12;
    for (; $numTemplates > 0; $numTemplates--) {
        $pos = &roundTo(4, $pos);
        my $templateID = &shortStrToInt(@bytes[$pos..$pos+1]);
        my $numberKeys = &shortStrToInt(@bytes[$pos+2..$pos+3]);
        print "\n\t\ttemplate($templateID) ($::templateName{$templateID})";
        $pos += 4;
        for (my $keyNum = 0 ; $keyNum < $numberKeys; $keyNum++) {
            my $keyNumPadded = sprintf("%04d", $keyNum);
            my $keyID = &longStrToInt(@bytes[$pos..$pos+3]);
            my $type = &typeName(&shortStrToInt(@bytes[$pos+4..$pos+5]));
            print "\n\t\t\t#$keyNum key $keyID type($type)";
            print " attr($bytes[$pos+8] $bytes[$pos+9] $bytes[$pos+10] $bytes[$pos+11])";
            # do the best we can without the extended stuff
            if (! defined $::templateKey{"$session:$templateID:$keyID"}) {
                $::templateKey{"$session:$templateID:$keyNumPadded"} = "$type\t\t<$keyID>\t\t";
            }
            $pos += 12;
        }
    }
}

sub MsgID0012
{
    my ($length, $session, $timestampSeconds, $timestampMicroseconds, @bytes) = @_;
    my $endianFlag = &extractBit(&byteStrToInt($bytes[9]), 0);
    my $endianMsg = ($endianFlag ? "big" : "little");
    my $numTemplates = &shortStrToInt(@bytes[10..11]);
    print &MsgHdrXXX($timestampSeconds, $timestampMicroseconds), "FINAL TMPL DATA config(", &byteStrToInt($bytes[8]),
    ") flags($bytes[9]) E($endianFlag $endianMsg) #templates($numTemplates)";
     &msgIDtemplates(0, $session, $numTemplates, $length-12, @bytes[12..$#bytes]);
    die "Can't handle little-endian" unless $endianFlag == 1;
}

sub MsgID0013
{
    my ($length, $session, $timestampSeconds, $timestampMicroseconds, @bytes) = @_;
    print &MsgHdrXXX($timestampSeconds, $timestampMicroseconds), "FINAL TMPL DATA ACK config(", &byteStrToInt($bytes[8]), ")";
}

sub MsgID0014
{
    my ($length, $session, $timestampSeconds, $timestampMicroseconds, @bytes) = @_;
    print &MsgHdrXXX($timestampSeconds, $timestampMicroseconds), "GET SESS req(", &shortStrToInt(@bytes[8..9]), ")";
}

sub MsgID0015
{
    my ($length, $session, $timestampSeconds, $timestampMicroseconds, @bytes) = @_;
    my $sessions = &shortStrToInt(@bytes[10..11]);
    print &MsgHdrXXX($timestampSeconds, $timestampMicroseconds), "GET SESS RSP",
    " req(", &shortStrToInt(@bytes[8..9]),
    ") #sess($sessions)";
    my $pos = 16;
    my $vendorStr = "";
    for (my $vendorStrLength = &shortStrToInt(@bytes[12..13]);
         $vendorStrLength > 0;
         $vendorStrLength--) {
        $vendorStr .= chr(&byteStrToInt($bytes[$pos++]));
    }
    print " vendorStr($vendorStr)";
    for (my $i = 0; $i < $sessions; $i++) {
        $pos = &roundTo(4, $pos);
        my $sessionID = &byteStrToInt($bytes[$pos]);
        my $sessionNameLength = &shortStrToInt(@bytes[$pos+2..$pos+3]);
        my $sessionDescrLength = &shortStrToInt(@bytes[$pos+4..$pos+5]);
        $pos += 8;
        my $sessionName = "";
        for (; $sessionNameLength > 0; $sessionNameLength--) {
            $sessionName .= chr(&byteStrToInt($bytes[$pos++]));
        }
        $pos = &roundTo(4, $pos);
        my $descrName = "";
        for (; $sessionDescrLength > 0; $sessionDescrLength--) {
            $descrName .= chr(&byteStrToInt($bytes[$pos++]));
        }
        print "\n\t\tsession $i: ($sessionName) ($descrName)";
    }
}

sub MsgID0016
{
    my ($length, $session, $timestampSeconds, $timestampMicroseconds, @bytes) = @_;
    print &MsgHdrXXX($timestampSeconds, $timestampMicroseconds), "GET TMPL req(", &shortStrToInt(@bytes[8..9]), ")";
}

sub MsgID0017
{
    my ($length, $session, $timestampSeconds, $timestampMicroseconds, @bytes) = @_;
    my $numTemplates = &shortStrToInt(@bytes[10..11]);
    print &MsgHdrXXX($timestampSeconds, $timestampMicroseconds), "GET TMPL RSP req(", &shortStrToInt(@bytes[8..9]), ") #templates($numTemplates)";
    &msgIDtemplates(1, $session, $numTemplates, $length-12, @bytes[12..$#bytes]);
}

sub MsgID0018
{
    my ($length, $session, $timestampSeconds, $timestampMicroseconds, @bytes) = @_;
    print &MsgHdrXXX($timestampSeconds, $timestampMicroseconds), "START NEGOTIATE";
}

sub MsgID0019
{
    my ($length, $session, $timestampSeconds, $timestampMicroseconds, @bytes) = @_;
    print &MsgHdrXXX($timestampSeconds, $timestampMicroseconds), "START NEGOTIATE ACK";
}

sub MsgID0020
{
    my ($length, $session, $timestampSeconds, $timestampMicroseconds, @bytes) = @_;
    my $syncFlag = &extractBit(&byteStrToInt($bytes[11]), 0);
    my $dupFlag = &extractBit(&byteStrToInt($bytes[11]), 1);
    my $template =  &shortStrToInt(@bytes[8..9]);
    my $dataLength = $length - 16;
    my $thisTmplName = $::templateName{$template};
    if (! defined $thisTmplName) { $thisTmplName = "???"; }
    print &MsgHdrXXX($timestampSeconds, $timestampMicroseconds), "DATA template($template) $thisTmplName config(", &byteStrToInt($bytes[10]),
    ") flags($bytes[11]) sync($syncFlag) dup($dupFlag) DSN(",
    &longStrToIntN1(@bytes[12..15]), ")",
    " [bytes=$dataLength]"; # " ", join(":", @bytes[16..16+$dataLength]);
    my @fields = ();
    foreach my $f (sort keys %::templateKey) {
        if ($f =~ m/^$session:$template:([0-9]+)$/) {
            $fields[$1] = $::templateKey{$f};
            #### NEED TO REMOVE DELETED KEYS #####
        }
    }
    my $pos = 16;
    my $lastPos = 16 + $dataLength - 1;
    if ($#fields >= 0) {
        for (my $fi = 0; $fi <= $#fields; $fi++) {
            while ($fi <= $#fields && ! defined $fields[$fi]) { ++ $fi; }
            if ($fi <= $#fields) {
                my ($type, $attr, $name, $label, $help) = split(m/\t/, $fields[$fi]);
                print "\n\t\t\t$fi\@$pos: $name ($type) = ";
                if ($type eq "BOOLEAN") {
                    print &byteStrToInt($bytes[$pos]); $pos += 1;
                } elsif ($type eq "UINT_8") {
                    print &byteStrToInt($bytes[$pos]); $pos += 1;
                } elsif ($type eq "INT_8") {
                    print &byteStrToInt($bytes[$pos]); $pos += 1;
                } elsif ($type eq "UINT_16") {
                    # $pos = &roundTo(2, $pos);
                    print &shortStrToInt(@bytes[$pos..$pos+1]); $pos += 2;
                } elsif ($type eq "INT_16") {
                    # $pos = &roundTo(2, $pos);
                    print &shortStrToInt(@bytes[$pos..$pos+1]); $pos += 2;
                } elsif ($type eq "UINT_32") {
                    # $pos = &roundTo(4, $pos);
                    print &longStrToInt(@bytes[$pos..$pos+3]); $pos += 4;
                } elsif ($type eq "INT_32") {
                    # $pos = &roundTo(4, $pos);
                    print &longStrToInt(@bytes[$pos..$pos+3]); $pos += 4;
                } elsif ($type eq "UINT_64") {
                    # $pos = &roundTo(8, $pos);
                    print &longLongStrToInt(@bytes[$pos..$pos+7]); $pos += 8;
                } elsif ($type eq "INT_64") {
                    # $pos = &roundTo(8, $pos);
                    print &longLongStrToInt(@bytes[$pos..$pos+7]); $pos += 8;
                } elsif ($type eq "FLOAT") {
                    # $pos = &roundTo(4, $pos);
                    print join(":", @bytes[$pos..$pos+4]); $pos += 4;
                } elsif ($type eq "DOUBLE") {
                    # $pos = &roundTo(8, $pos);
                    print join(":", @bytes[$pos..$pos+7]); $pos += 8;
                } elsif ($type eq "STRING") {
                    # $pos = &roundTo(4, $pos);
                    my $length = &longStrToInt(@bytes[$pos..$pos+3]); $pos += 4;
                    my $str = '';
                    for (; $length > 0; $length--) { $str .= chr(&byteStrToInt($bytes[$pos++])); }
                    print "\"$str\"";
                } elsif ($type eq "NT_STRING") {
                    my $str = '';
                    while ($bytes[$pos] ne "00") {
                        $str .= chr(&byteStrToInt($bytes[$pos++]));
                    }
                    print "\"$str\"";
                    $pos++; # over the "00"
                } elsif ($type eq "UTF8") {
                    die "*** UTF8 ***";
                } elsif ($type eq "UTF16") {
                    die "*** UTF16 ***";
                } elsif ($type eq "IPV4_ADDR") {
                    # $pos = &roundTo(4, $pos);
                    print &byteStrToInt($bytes[$pos]), ".",
                    &byteStrToInt($bytes[$pos+1]), ".",
                    &byteStrToInt($bytes[$pos+2]), ".",
                    &byteStrToInt($bytes[$pos+3]);
                    $pos += 4;
                } elsif ($type eq "IPV6_ADDR") {
                    print &byteStrToInt($bytes[$pos]), ".",
                    &byteStrToInt($bytes[$pos+1]), ".",
                    &byteStrToInt($bytes[$pos+2]), ".",
                    &byteStrToInt($bytes[$pos+3]), ".",
                    &byteStrToInt($bytes[$pos+4]), ".",
                    &byteStrToInt($bytes[$pos+5]);
                    $pos += 4;
                } elsif ($type eq "TIME_SEC") {
                    # $pos = &roundTo(4, $pos);
                    my $time_s = &longStrToInt(@bytes[$pos..$pos+3]);
                    $pos += 4;
                    my $time = gmtime($time_s);
                    print "$time_s: $time";
                } elsif ($type eq "TIME_MSEC") {
                    # $pos = &roundTo(8, $pos);
                    my $time_m = join(":", @bytes[$pos..$pos+7]);
                    $pos += 8;
                    my $time = gmtime($time_m / 1000);
                    print "$time_m: $time", sprintf(".%03d", $time_m % 1000);
                } elsif ($type eq "TIME_USEC") {
                    # $pos = &roundTo(8, $pos);
                    my $time_u = join(":", @bytes[$pos..$pos+7]);
                    $pos += 8;
                    my $time = gmtime($time_u / 1000000);
                    print "$time_u: $time", sprintf(".%06d", $time_u % 1000000);
                } elsif ($type eq "BLOB") {
                    # $pos = &roundTo(4, $pos);
                    my $length = &longStrToInt(@bytes[$pos..$pos+3]); $pos += 4;
                    print "[$length bytes]";
                    $pos += $length;
                } else {
                    die "Unknown type: $type for session($session) template($template) name($name)";
                }
            }
        }
        print "\n\t\tActual length = $pos";
    } else { # no field information, so just skip 
        $pos = $lastPos - 1;
    }
    # messages are word (4-byte) aligned, so skip over trailing stuff
    while ($pos % 4 != 0) { $pos++; }
    die "fields(", ($#fields+1), ")=", join(":",@fields), " pos=$pos, dataLength=$dataLength, lastPos=$lastPos ... " . join(":", @bytes[0..15]) . "|" . join(":", @bytes[16..$pos-1]) . "|" . join(":", @bytes[$pos..$lastPos]) . "|" . join(":", @bytes[$lastPos+1..$lastPos+20]) unless $pos == $lastPos + 1;
}

sub MsgID0021
{
    my ($length, $session, $timestampSeconds, $timestampMicroseconds, @bytes) = @_;
    print &MsgHdrXXX($timestampSeconds, $timestampMicroseconds), "DATA ACK config(", &byteStrToInt($bytes[12]),
    ") DSN(", &longStrToIntN1(@bytes[8..11]), ")";
}

sub MsgID0023
{
    my ($length, $session, $timestampSeconds, $timestampMicroseconds, @bytes) = @_;
    print &MsgHdrXXX($timestampSeconds, $timestampMicroseconds), "DATA NACK config(", &byteStrToInt($bytes[12]),
    ") DSN(", &longStrToIntN1(@bytes[8..11]), ")";
}

sub MsgID0024
{
    my ($length, $session, $timestampSeconds, $timestampMicroseconds, @bytes) = @_;
    my $errorTime = gmtime(&longStrToInt(@bytes[8..11]));
    print &MsgHdrXXX($timestampSeconds, $timestampMicroseconds), "ERROR at: $errorTime code(", &shortStrToInt(@bytes[12..13]), ")";
    my $errorLength = &shortStrToInt(@bytes[14..15]);
    my $errorStr = '';
    for (my $pos = 16; $errorLength > 0; $errorLength--) {
        $errorStr .= chr(&byteStrToInt($bytes[$pos++]));
    }
    if (length($errorStr) > 0) {
        print ": $errorStr";
    } else {
        print " (no description)";
    }
}


sub MsgID0030
{
    my ($length, $session, $timestampSeconds, $timestampMicroseconds, @bytes) = @_;
    print &MsgHdrXXX($timestampSeconds, $timestampMicroseconds), "STATUS REQ";
}

sub MsgID0031
{
    my ($length, $session, $timestampSeconds, $timestampMicroseconds, @bytes) = @_;
    my $recordLength = &longStrToInt(@bytes[12..15]);
    print &MsgHdrXXX($timestampSeconds, $timestampMicroseconds), "STATUS RSP template(", &shortStrToInt(@bytes[8..9]),
    ") config(", &shortStrToInt(@bytes[10..11]), ") length($recordLength)";
}

sub msgIDtemplates
{
    my ($extended, $session, $numTemplates, $length, @bytes) = @_;
    my $pos = 0;
    for (; $numTemplates > 0; $numTemplates--) {
        $pos = &roundTo(4, $pos);
        my $templateID = &shortStrToInt(@bytes[$pos..$pos+1]);
        my $numberKeys = &shortStrToInt(@bytes[$pos+2..$pos+3]);
        my $templateFlags = "$bytes[$pos+4] $bytes[$pos+5]";
        my $descrLength = &shortStrToInt(@bytes[$pos+6..$pos+7]);
        my $templateLength = &longStrToInt(@bytes[$pos+8..$pos+11]);
        my $descr = "";
        $pos += 12;
        for (; $descrLength > 0; $descrLength--) {
            $descr .= chr(&byteStrToInt($bytes[$pos++]));
        }
        print "\n\t\ttemplate($templateID) ($descr) flags($templateFlags) length($templateLength)";
        $::templateName{$templateID} = $descr;
        $::templateLength{$templateID} = $templateLength;
        $pos = &roundTo(4, $pos);
        for (my $keyNum = 0 ; $keyNum < $numberKeys; $keyNum++) {
            my $keyNumPadded = sprintf("%04d", $keyNum);
            my $keyID = &longStrToInt(@bytes[$pos..$pos+3]);
            my $type = &typeName(&shortStrToInt(@bytes[$pos+4..$pos+5]));
            print "\n\t\t\t#$keyNum key $keyID type($type)";
            if ($extended) {
                my $keyNameLength  = &shortStrToInt(@bytes[$pos+6..$pos+7]);
                my $keyLabelLength = &shortStrToInt(@bytes[$pos+8..$pos+9]);
                my $keyHelpLength  = &shortStrToInt(@bytes[$pos+10..$pos+11]);
                my $attr = "$bytes[$pos+12] $bytes[$pos+13] $bytes[$pos+14] $bytes[$pos+15]";
                $pos += 16;
                my $keyName = '';
                for (; $keyNameLength > 0; $keyNameLength--) {
                    $keyName .= chr(&byteStrToInt($bytes[$pos++]));
                }
                $pos = &roundTo(4, $pos);
                my $keyLabel = '';
                for (; $keyLabelLength > 0; $keyLabelLength--) {
                    $keyLabel .= chr(&byteStrToInt($bytes[$pos++]));
                }
                $pos = &roundTo(4, $pos);
                my $keyHelp = '';
                for (; $keyHelpLength > 0; $keyHelpLength--) {
                    $keyHelp .= chr(&byteStrToInt($bytes[$pos++]));
                }
                $pos = &roundTo(4, $pos);
                print " attr($attr) name($keyName) label($keyLabel) help($keyHelp)";
                $::templateKey{"$session:$templateID:$keyNumPadded"} = "$type\t$attr\t$keyName<$keyID>\t$keyLabel\t$keyHelp";
            } else {
                print " attr($bytes[$pos+8] $bytes[$pos+9] $bytes[$pos+10] $bytes[$pos+11])";
                # do the best we can without the extended stuff
                if (! defined $::templateKey{"$session:$templateID:$keyID"}) {
                    $::templateKey{"$session:$templateID:$keyNumPadded"} = "$type\t\t<$keyID>\t\t";
                }
                $pos += 12;
            }
        }
    }
}

sub typeName
{
    my ($typeID) = @_;
    if ($typeID == 0x0001) { return "BOOLEAN"; }
    if ($typeID == 0x0002) { return "UINT_8"; }
    if ($typeID == 0x0003) { return "INT_8"; }
    if ($typeID == 0x0004) { return "UINT_16"; }
    if ($typeID == 0x0005) { return "INT_16"; }
    if ($typeID == 0x0006) { return "UINT_32"; }
    if ($typeID == 0x0007) { return "INT_32"; }
    if ($typeID == 0x0008) { return "UINT_64"; }
    if ($typeID == 0x0009) { return "INT_64"; }
    if ($typeID == 0x000a) { return "FLOAT"; }
    if ($typeID == 0x000b) { return "DOUBLE"; }
    if ($typeID == 0x400c) { return "STRING"; }
    if ($typeID == 0x400d) { return "NT_STRING"; }
    if ($typeID == 0x400e) { return "UTF8"; }
    if ($typeID == 0x400f) { return "UTF16"; }
    if ($typeID == 0x0010) { return "IPV4_ADDR"; }
    if ($typeID == 0x0011) { return "IPV6_ADDR"; }
    if ($typeID == 0x0012) { return "TIME_SEC"; }
    if ($typeID == 0x0013) { return "TIME_MSEC"; }
    if ($typeID == 0x0014) { return "TIME_USEC"; }
    if ($typeID == 0x4015) { return "BLOB"; }
    return "Type($typeID)"
}

sub roundTo
{
    my ($N, $i) = @_;
    while ($i % $N != 0) { $i++; }
    return $i;
}

# extractBit ... bitFromRight starts at 0 (e.g. 0 is rightmost, 31 is leftmost)

sub extractBit
{
    my ($int, $bitFromRight) = @_;
    for (; $bitFromRight > 0; $bitFromRight--) { $int = $int >> 1; }
    return $int & 0x01;
}

sub printableIP
{
    my ($IP, $port) = @_;
    return inet_ntoa(pack("N",$IP)) . ":" . $port;
}
