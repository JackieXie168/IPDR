# $Id: Makefile,v 1.10 2005/05/11 12:25:17 dmitryk Exp $


USE_GCC := 1 # Sun's CC (5) doesn't have STL

include $(IPDR_ROOT)/build/IPDR.defs

SRC = IPDRsimple.c

LDFLAGS += -L$(IPDR_ROOT)/tools/cmu_snmp/$(IPDR_PLATFORM)/lib
CFLAGS += -I$(IPDR_ROOT)/tools/cmu_snmp/$(IPDR_PLATFORM)/include

TARGET_TYPE = bin
TARGET_NAME = IPDRsimple

ifeq ($(IPDR_PLATFORM), vxworks)
DEPENDENCIES = Platforms/VxWorks Exporter
LIBS = IPDRExporter IPDRVxWorks
else
DEPENDENCIES = Platforms/Generic Exporter
LIBS = IPDRExporter IPDRGeneric # agentx
endif

include $(IPDR_ROOT)/build/IPDR.rules

doc: FORCE
	$(RM) -r IPDRsimple-doc
	mkdir -p IPDRsimple-doc/html
	cp -pi ../../../../../Doc/rfc*.txt IPDRsimple-doc/html
	@# in case the doxygen fails
	@# touch IPDRsimple-doc/html/index.html
	doxygen IPDRsimple.doxygen-cfg

FORCE: ;
