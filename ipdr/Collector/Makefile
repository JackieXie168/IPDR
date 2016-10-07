# IPDR Collector Make file
# $Log: Makefile,v $
# Revision 1.7  2005/01/26 17:30:40  ilyad
# *** empty log message ***
#
# Revision 1.6  2005/01/26 14:23:22  ilyad
# *** empty log message ***
#
# Revision 1.5  2005/01/26 11:39:35  ilyad
# *** empty log message ***
#
# Revision 1.4  2005/01/23 21:42:03  michaeld
# *** empty log message ***
#
# Revision 1.3  2005/01/11 12:26:37  michaeld
# Update make file
#
# Revision 1.2  2005/01/04 15:25:57  michaeld
# Fixed Solaris build - now successfully build jars
#
# Revision 1.1  2005/01/03 21:37:49  michaeld
# *** empty log message ***
#

JSDK_ROOT=/opt/j2sdk1.4.2_07
CVSROOT=/export/spare/cvs/ipdr

JRE_ROOT=$(JSDK_ROOT)/jre
CLASSPATH=.:jdom.jar:$(JSDK_ROOT)/jre/lib/rt.jar:log4j-1.2.9.jar
CLASSPATH_RUN=ipdrapi.jar:collector.jar:$(CLASSPATH)
JAVAC=$(JSDK_ROOT)/bin/javac
JAVA=$(JRE_ROOT)/bin/java

API_SUBDIRS=org/ipdr org/ipdr/utility org/ipdr/api org/ipdr/api/log org/ipdr/api/configuration org/ipdr/protocol org/ipdr/utility com/xacct/CRANE/collector
COLLECTOR_SUBDIRS=org/ipdr/collector org/ipdr/collector/configuration org/ipdr/collector/log
COLLECTOR_FILES=$(addsuffix .class,$(basename $(wildcard $(COLLECTOR_SUBDIRS:=/*.java))))
API_FILES=$(addsuffix .class,$(basename $(wildcard $(API_SUBDIRS:=/*.java))))

SUBDIRS=$(API_SUBDIRS) $(COLLECTOR_SUBDIRS)

all: api collector

api: ipdrapi.jar

collector: collector.jar api

ipdrapi.jar : $(API_FILES)
	@echo Building ipdrapi jar
	jar -cvf $@ $(API_SUBDIRS:=/*.class )

collector.jar: $(COLLECTOR_FILES)
	@echo Building collector jar
	jar -cvf $@ $(COLLECTOR_SUBDIRS:=/*.class )

%.class: %.java
	@echo Compiling $<
	@$(JAVAC) -classpath $(CLASSPATH) $<

run: collector api
	$(JAVA) -classpath $(CLASSPATH_RUN) org.ipdr.collector.Test

update:
	cd ..;cvs -d $(CVSROOT) update Collector; cd Collector

clean:
	@rm -f collector.jar ipdrapi.jar $(SUBDIRS:=/*.class )
