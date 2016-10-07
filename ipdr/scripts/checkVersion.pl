# $Id: checkVersion.pl,v 1.1.1.1 2004/12/12 23:02:03 michaeld Exp $

# Use this script as follows (see checkVersion.sh)

# ncftpget -u userid -px ftp://192.168.8.88/swver/ver11024/asmaind
# strings asmaind >asmaind.strings
# gunzip -c xxx.tgz | perl -w checkVersion.pl asmaind.strings

use strict;
use English;

my %srcID = ();

while (<STDIN>) {
    while ($_ =~ m/"@\(#\) ([^""]*)"/) {
        my $id = $1;
        $_ = $POSTMATCH;
        $id =~ s/(\$ )"/$1/; #" to make Perl emacs mode happy
        $id =~ m/Id: ([^\s]+)/;
        my $idKey = $1;
        $srcID{$idKey} = $id;
        if ($idKey =~ /,v$/) {
            $srcID{$PREMATCH} = $id;
        } else {
            $srcID{"$idKey,v"} = $id;
        }
        # print "($idKey) -> ($id)\n";
    }
}

die "Need the strings file" unless defined $ARGV[0];
open(STRINGS, "<$ARGV[0]") || die "Can't open $ARGV[0]";

while (<STRINGS>) {
    if (/@\(#\) /) {
        my $id = $POSTMATCH;
        $id =~ s/Copyright .*$//;
        chomp($id);
        $id =~ m/Id: ([^\s]+)/;
        my $idKey = $1;
        if ($srcID{$idKey} ne $id) {
            print "$idKey:\n";
            print "\t\tsource: $srcID{$idKey}\n";
            print "\t\texec:   $id\n";
        }
    }
}
