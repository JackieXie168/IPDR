# *****************************************************
# * File                 : Makefile                   *
# * Description          : Makefile for C files       *
# * Author               : Infosys Tech Ltd           *
# * Modification History :                            *
# *---------------------------------------------------*
# * Date       Name             Change/Description    *
# *---------------------------------------------------*
# * 01/16/02  Infosys           Created               *         
# *****************************************************

include $(IPDRROOTPATH)/makefile.mk

PROGRAM=IPDRFTPLIB

SOURCE = CapabilityFile.c ControlFile.c RangeFile.c SubscriptionGroup.c \
         GetFileFromURL.c ErrorHandling.c GetFileStream.c
OBJECTS = CapabilityFile.o ControlFile.o RangeFile.o \
          SubscriptionGroup.o  GetFileFromURL.o ErrorHandling.o GetFileStream.o

$(PROGRAM): $(TARGET) 
all: $(TARGET)

#Suffix rule for .o files
.c.o:
	@$(MAKEOBJ)

$(TARGET): $(OBJECTS)
	@$(MAKELIB)
clean:
	rm -f $(OBJECTS) $(LIBPATH)/$(TARGET)

