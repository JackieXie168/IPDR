# $Id: Makefile,v 1.4 2005/04/21 11:30:40 dmitryk Exp $

USE_GCC := 1 # Sun's CC (5) doesn't have STL

include $(IPDR_ROOT)/build/IPDR.defs

# comment out files that are #include-d from IPDRsim.cpp
SRC = IPDRsim.cpp # IPDRsimTemplates.cpp IPDRsimAux.cpp

LDFLAGS += -L$(IPDR_ROOT)/tools/cmu_snmp/$(IPDR_PLATFORM)/lib 
CFLAGS += -I$(IPDR_ROOT)/tools/cmu_snmp/$(IPDR_PLATFORM)/include

TARGET_TYPE = bin
TARGET_NAME = IPDRsim

ifeq ($(IPDR_PLATFORM), vxworks)
DEPENDENCIES = Platforms/VxWorks Exporter 
LIBS = IPDRExporter IPDRVxWorks 
else
DEPENDENCIES = Exporter Platforms/Generic 
LIBS = IPDRExporter IPDRGeneric # agentx
endif

include $(IPDR_ROOT)/build/IPDR.rules

doc: FORCE
	$(RM) -r IPDRsim-doc
	mkdir -p IPDRsim-doc/html
	cp -pi ../../../../../Doc/rfc*.txt IPDRsim-doc/html
	@# in case the doxygen fails
	@# touch IPDRsim-doc/html/index.html
	doxygen IPDRsim.doxygen-cfg

FORCE: ;
