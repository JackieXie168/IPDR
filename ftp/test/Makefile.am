noinst_PROGRAMS = ProducerIT ConsumerIT UnitTest ProducerTest1 ProducerTest3 ProducerTest4a ProducerTest4b ProducerTest6 \
     ConsumerTest1 ConsumerTest2 ConsumerTest3 ConsumerTest5a ConsumerTest5b

AM_CFLAGS=-g -O2 -I$(top_srcdir)/ftp/include -Iinclude
AM_LDFLAGS=-lm -lexpat -lcurl -lm


SOURCE = ipdrInterface/IPDRReadUtils.c ipdrInterface/IPDRReadWriteTool.c \
         ipdrInterface/IPDRWriteUtils.c

IPDRLIBS = $(top_srcdir)/ftp/source/ftp/libIPDRftp.la $(top_srcdir)/source/common/libIPDR.la $(top_srcdir)/source/xdr/libXDR.la $(top_srcdir)/source/utils/libUTILS.la $(top_srcdir)/source/xml/libXML.la

UnitTest_SOURCES = UnitTest.c
ProducerIT_SOURCES = ProducerIT.c $(SOURCE)
ConsumerIT_SOURCES = ConsumerIT.c $(SOURCE)
ProducerTest1_SOURCES = ProducerTest1.c
ProducerTest3_SOURCES = ProducerTest3.c
ProducerTest4a_SOURCES = ProducerTest4a.c
ProducerTest4b_SOURCES = ProducerTest4b.c
ProducerTest6_SOURCES = ProducerTest6.c
ConsumerTest1_SOURCES = ConsumerTest1.c
ConsumerTest2_SOURCES = ConsumerTest2.c
ConsumerTest3_SOURCES = ConsumerTest3.c
ConsumerTest5a_SOURCES = ConsumerTest5a.c
ConsumerTest5b_SOURCES = ConsumerTest5b.c

UnitTest_LDADD = $(top_srcdir)/ftp/source/ftp/libIPDRftp.la
ProducerTest1_LDADD = $(top_srcdir)/ftp/source/ftp/libIPDRftp.la
ProducerTest3_LDADD = $(top_srcdir)/ftp/source/ftp/libIPDRftp.la
ProducerTest4a_LDADD = $(top_srcdir)/ftp/source/ftp/libIPDRftp.la
ProducerTest4b_LDADD = $(top_srcdir)/ftp/source/ftp/libIPDRftp.la
ProducerTest6_LDADD = $(top_srcdir)/ftp/source/ftp/libIPDRftp.la
ConsumerTest1_LDADD = $(top_srcdir)/ftp/source/ftp/libIPDRftp.la
ConsumerTest2_LDADD = $(top_srcdir)/ftp/source/ftp/libIPDRftp.la
ConsumerTest3_LDADD = $(top_srcdir)/ftp/source/ftp/libIPDRftp.la
ConsumerTest5a_LDADD = $(top_srcdir)/ftp/source/ftp/libIPDRftp.la
ConsumerTest5b_LDADD = $(top_srcdir)/ftp/source/ftp/libIPDRftp.la
ProducerIT_LDADD = $(top_srcdir)/ftp/source/ftp/libIPDRftp.la $(IPDRLIBS)
ConsumerIT_LDADD = $(top_srcdir)/ftp/source/ftp/libIPDRftp.la $(IPDRLIBS)

CLEANFILES = *~ *.log
