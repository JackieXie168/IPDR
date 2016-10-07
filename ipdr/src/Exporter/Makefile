# $Id: Makefile,v 1.4 2005/04/21 11:30:21 dmitryk Exp $

include $(IPDR_ROOT)/build/IPDR.defs

ENABLE_HOST_SIM :=
IPDR_VER := \"IPDR Exporter V$(shell cat $(IPDR_ROOT)/IPDR/Doc/VERSION) - $(PLATFORM), Built $(shell date -u +%Y-%m-%dT%k:%M:%SZ)\"

SRC = Exporter.c \
      ExporterProtocol.c \
      Templates.c \
      VolatileQueue.c \
      ExporterConsole.c \
      ProtocolDebug.c \
      ExporterMIB.c \
      VQHandler.c
#ExporterMIBConstants.c \
 
#  ExtQueueHandler.c

ifdef ENABLE_HOST_SIM
SRC += HostApp.c
CFLAGS += -DENABLE_HOST_SIM
endif

CFLAGS += -DIPDR_VER="$(IPDR_VER)"

TARGET_TYPE = archive
TARGET_NAME = IPDRExporter

SUB_ARCHIVES = IPDRCommon IPDRTCP
DEPENDENCIES = Common Transports/TCP

include $(IPDR_ROOT)/build/IPDR.rules
