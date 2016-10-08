lib_LTLIBRARIES = libIPDRftp.la

AM_CFLAGS=-g -c -O2 -I$(top_srcdir)/include -Iinclude -I$(top_srcdir)/ftp/include

libIPDRftp_la_SOURCES = CapabilityFile.c ControlFile.c RangeFile.c SubscriptionGroup.c \
				GetFileFromURL.c ErrorHandling.c GetFileStream.c

libIPDRftp_la_LIBADD =$(top_srcdir)/source/common/libIPDR.la $(top_srcdir)/source/xdr/libXDR.la \
				$(top_srcdir)/source/utils/libUTILS.la $(top_srcdir)/source/xml/libXML.la

CLEANFILES = *~ *.log