# $Id: Makefile,v 1.4 2005/04/21 11:53:40 dmitryk Exp $

include $(IPDR_ROOT)/build/IPDR.defs

SRC = DynamicArray.c \
      MemoryManagement.c \
      CommonUtil.c \
      Log.c \
      Console.c \
      ParametersHandler.c \
      ChunksManager.c

TARGET_TYPE = archive
TARGET_NAME = IPDRCommon

ifdef ENABLE_HOST_SIM
CFLAGS += -DENABLE_HOST_SIM
endif

include $(IPDR_ROOT)/build/IPDR.rules
