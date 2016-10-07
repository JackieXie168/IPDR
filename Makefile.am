ACLOCAL_AMFLAGS = -I m4
AM_LDFLAGS = -lm
AM_CFLAGS=-c -I$(top_srcdir)/include

SUBDIRS = source example test ftp/source/ftp ftp/test

#AUTOMAKE_OPTIONS = foreign no-dependencies
#INCLUDES = -I./../../include/

#lib_LIBRARIES=libIPDR.a
#lib_LTLIBRARIES = libIPDR.la

#libIPDR_a_SOURCES = descriptor.c  FNFData.c  FNFType.c  getFile.c  IPDRDocReader.c  IPDRDocWriter.c  IPDRHeader.c  schema.c  
#libIPDR_la_SOURCES = descriptor.c  FNFData.c  FNFType.c  getFile.c  IPDRDocReader.c  IPDRDocWriter.c  IPDRHeader.c  schema.c

EXTRA_DIST = docs/*.pdf docs/doxygen docs/doxygen-private ftp/docs/doxygen ftp/docs/doxygen ftp/docs/doxygen-private

docs/doxygen:
	@mkdir -p $(builddir)/$@
	pushd $(abs_srcdir) && sed -e "s|^OUTPUT_DIRECTORY.*$$|OUTPUT_DIRECTORY = $(abs_builddir)/$@|;" < Doxyfile | doxygen - && popd

docs/doxygen-private:
	@mkdir -p $(builddir)/$@
	pushd $(abs_srcdir)/include && sed -e "s|^OUTPUT_DIRECTORY.*$$|OUTPUT_DIRECTORY = $(abs_builddir)/$@|;" < Doxyfile | doxygen - && popd

ftp/docs/doxygen:
	@mkdir -p $(builddir)/$@
	pushd $(abs_srcdir) && sed -e "s|^OUTPUT_DIRECTORY.*$$|OUTPUT_DIRECTORY = $(abs_builddir)/$@|;" < Doxyfile | doxygen - && popd

ftp/docs/doxygen-private:
	@mkdir -p $(builddir)/$@
	pushd $(abs_srcdir)/include && sed -e "s|^OUTPUT_DIRECTORY.*$$|OUTPUT_DIRECTORY = $(abs_builddir)/$@|;" < Doxyfile | doxygen - && popd

clean-local:
	rm -rf docs/doxygen
	rm -rf docs/doxygen-private


CLEANFILES = *~ *.log
