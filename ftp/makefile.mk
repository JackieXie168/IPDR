# *****************************************************
# * File                 : makefile.mk                *
# * Description          : Common Makefile for all    *
# *                        other makefiles            *
# * Author               : Infosys Tech Ltd           *
# * Modification History :                            *
# *---------------------------------------------------*
# * Date       Name             Change/Description    *
# *---------------------------------------------------*
# * 01/16/02  Infosys           Created               *         
# *****************************************************


#Uncomment this line to use GNU compiler
CC=gcc
#comment this line to use GNU compiler
#CC=cc

#Set IPDRLIBS to ipdrbase directory path
IPDRLIBSROOTPATH = /home/girishmd/c_ipdrbase_1.0.0

LIBPATH=$(IPDRROOTPATH)/libs
#used only for integration with conversion libraries
LIBPATH_IT=$(LIBPATH) $(IPDRLIBSROOTPATH)/libs

INCLUDEDIRS = -I$(IPDRROOTPATH)/include
#used only for integration with conversion libraries
INCLUDEDIRS_IT = $(INCLUDEDIRS) -I$(IPDRLIBSROOTPATH)/include 

# Uncomment this line to build the Application in Debug Mode
#DEBUGFLAG=DEBUG_IPDRFTP

# Uncomment this line to build the Application in Non Debug Mode
DEBUGFLAG=DEBUG_IPDRFTP_OFF

LDIPDRLIBS = -lIPDR -lXDR -lXML -lUTILS

MAKEOBJ= `if [ "$(CC)" = "gcc" ] ; then \
       echo $(CC) -g -D$(DEBUGFLAG) $(INCLUDEDIRS) -fPIC -c $(SOURCE) ; \
       elif [ "$(CC)" = "cc" -a "$(OS)" = "SunOS" ] ; then \
       echo $(CC) -g -xCC -D$(DEBUGFLAG) $(INCLUDEDIRS) -KPIC -c $(SOURCE) ; \
       elif [ "$(CC)" = "cc" -a "$(OS)" = "HP-UX" ] ; then \
       echo $(CC) -g -D$(DEBUGFLAG) $(INCLUDEDIRS) -Ae -c +z $(SOURCE) ; \
       fi`

MAKELIB = `if test $(OS) = "Linux"; \
           then echo $(CC) -g -shared -o $(LIBPATH)/$(TARGET) $(OBJECTS); \
           elif test $(OS) = "HP-UX" -a $(CC) = "gcc"; \
           then \
           echo $(CC) -g -fPIC -shared -o $(LIBPATH)/$(TARGET) $(OBJECTS); \
           elif test $(OS) = "HP-UX" -a $(CC) = "cc"; \
           then \
           echo ld -b -o $(LIBPATH)/$(TARGET) $(OBJECTS); \
           else \
           echo $(CC) -G -o -g $(LIBPATH)/$(TARGET) $(OBJECTS); \
           fi` 
