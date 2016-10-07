LIBDIR = ./../libs 
INCDIR = ./../include/

CPPFLAGS = 
LDFLAGS =  -static
LIBS = -L$(LIBDIR) -lIPDR -lUTILS -lXDR -lXML -lexpat -lcurl -lm
CC = gcc
CFLAGS = -g -O2 -I$(INCDIR)
TARGET=IPDRLIBSEXAMPLE
OBJECTS= testXDR testXML


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
	rm -f testXDR testXML core *.o

distclean: clean
	rm -f Makefile

maintainer-clean: distclean
