# $Id: checkVersion.sh,v 1.1.1.1 2004/12/12 23:02:03 michaeld Exp $
# set -x
#
# This trivial script gets images from a number of machiens and
# checks their version information against the distribution.

/bin/rm -f asmaind asmaind.strings

VER=ver11023
VER=ver11025

for arg in 192.168.8.85 192.168.8.88 192.168.9.80 192.168.9.87 192.168.9.84 192.168.9.80
do
  echo =================
  echo $arg
  echo =================
  /usr/local/bin/ncftpget -u userid -px ftp://$arg/swver/$VER/asmaind
  strings asmaind >asmaind.strings
  gunzip -c $(ls -t CRANE_distribution_* | head -1) | perl -w checkVersion.pl asmaind.strings
done

/bin/rm -f asmaind asmaind.strings
