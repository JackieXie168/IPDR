noinst_PROGRAMS = IPDRWriteTool IPDRReadTool

AM_CFLAGS=-g -O2 -I$(top_srcdir)/include -Iinclude
AM_LDFLAGS=-lm -lexpat -lcurl -lm

IPDRWriteTool_SOURCES = IPDRWriteTool.c
IPDRReadTool_SOURCES = IPDRReadTool.c

IPDRWriteTool_LDADD = $(top_srcdir)/source/common/libIPDR.la $(top_srcdir)/source/xdr/libXDR.la $(top_srcdir)/source/utils/libUTILS.la $(top_srcdir)/source/xml/libXML.la
IPDRReadTool_LDADD = $(top_srcdir)/source/common/libIPDR.la $(top_srcdir)/source/xdr/libXDR.la $(top_srcdir)/source/utils/libUTILS.la $(top_srcdir)/source/xml/libXML.la

CLEANFILES = *~ *.log


#LIBDIR = ./../libs
#INCDIR = ./../include/

#CPPFLAGS =
#LDFLAGS =  -static
#LIBS = -L$(LIBDIR) -L./include/ -lIPDR -lUTILS -lXDR -lXML -lexpat -lcurl -lm
#CC = gcc
#CFLAGS = -g -O2 -I$(INCDIR)
#TARGET=IPDRLIBSTEST
#OBJECTS= IPDRWriteTool IPDRReadTool


#srcdir = .
#top_srcdir = ..
#VPATH =


#$(TARGET):$(OBJECTS)
#all: $(TARGET)


# Suffix rule for building testscript .c Sources
#.c:
#	@$(CC) -g -I$(INCDIR) -o $@ $@.c  $(LIBS)


check:	$(SUBDIRS)
	@echo
	@echo This package does not yet have a regression test.
	@echo

#clean:
#	rm -f IPDRWriteTool IPDRReadTool core *.o

#distclean: clean
#	rm -f Makefile

#maintainer-clean: distclean