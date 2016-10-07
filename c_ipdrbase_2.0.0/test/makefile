LIBDIR = ./../libs 
INCDIR = ./../include/

CPPFLAGS = 
LDFLAGS =  -static
LIBS = -L$(LIBDIR) -L./include/ -lIPDR -lUTILS -lXDR -lXML -lexpat -lcurl -lm
CC = gcc
CFLAGS = -g -O2 -I$(INCDIR)
TARGET=IPDRLIBSTEST
OBJECTS= IPDRWriteTool IPDRReadTool


srcdir = .
top_srcdir = ..
VPATH = 


$(TARGET):$(OBJECTS)
all: $(TARGET)


# Suffix rule for building testscript .c Sources
.c:
	@$(CC) -g -I$(INCDIR) -o $@ $@.c  $(LIBS)


check:	$(SUBDIRS)
	@echo
	@echo This package does not yet have a regression test.
	@echo

clean:
	rm -f IPDRWriteTool IPDRReadTool core *.o

distclean: clean
	rm -f Makefile

maintainer-clean: distclean
