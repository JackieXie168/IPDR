# *****************************************************
# * File                 : makefile                   *
# * Description          : Common Makefile for all    *
# *                        test scripts               *
# * Author               : Infosys Tech Ltd           *
# * Modification History :                            *
# *---------------------------------------------------*
# * Date       Name             Change/Description    *
# *---------------------------------------------------*
# * 03/01/02  Infosys           Created               *
# *****************************************************

include $(IPDRROOTPATH)/makefile.mk

SOURCE = ipdrInterface/IPDRReadUtils.c ipdrInterface/IPDRReadWriteTool.c \
         ipdrInterface/IPDRWriteUtils.c 

LDFLAGS = -lexpat -lcurl -lFTP -lm 
LDFLAGS_IT = -lexpat -lcurl -lFTP $(LDIPDRLIBS) -lm 

LDINCLUDE = -L$(IPDRLIBSROOTPATH)/libs -L$(IPDRROOTPATH)/libs -L/usr/local/lib

test:UnitTest ProducerTest1 ProducerTest3 ProducerTest4a ProducerTest4b ProducerTest6 \
     ConsumerTest1 ConsumerTest2 ConsumerTest3 ConsumerTest5a ConsumerTest5b
test_IT:ProducerIT ConsumerIT

MAKETEST = `if [ "$(CC)" = "cc" -a "$(OS)" = "SunOS" ] ; then \
            echo $(CC) -g -xCC -D$(DEBUGFLAG) $(INCLUDEDIRS) -o $@ $@.c $(LDINCLUDE) $(LDFLAGS) ; \
            else \
            echo $(CC) -g -D$(DEBUGFLAG) $(INCLUDEDIRS) -o $@ $@.c $(LDINCLUDE) $(LDFLAGS) ; \
            fi`

MAKETEST_IT = `if [ "$(CC)" = "cc" -a "$(OS)" = "SunOS" ] ; then \
               echo $(CC) -g -xCC $@.c $(SOURCE) $(INCLUDEDIRS_IT) -I./include -o $@ $(LDINCLUDE) $(LDFLAGS_IT); \
               else \
               echo $(CC)  -g $@.c $(SOURCE) $(INCLUDEDIRS_IT) -I./include -o $@ $(LDINCLUDE) $(LDFLAGS_IT) ; \
               fi`

all:test test_IT

.c:
	@$(MAKETEST)

ProducerIT:ProducerIT.c $(SOURCE)
	@$(MAKETEST_IT)

ConsumerIT:ConsumerIT.c $(SOURCE)
	@$(MAKETEST_IT)

clean:
	rm -f *.o \
        UnitTest \
        ProducerTest1 \
        ProducerTest3 \
        ProducerTest4a \
        ProducerTest4b \
        ProducerTest6 \
        ConsumerTest1 \
        ConsumerTest2 \
        ConsumerTest3 \
        ConsumerTest5a \
        ConsumerTest5b \
        ProducerIT \
        ConsumerIT \
        x*.* core
