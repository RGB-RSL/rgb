TOPDIR = .

CXX=g++

PWD = $(shell pwd)
BASEPWD = $(shell basename $(shell pwd))
#AGENTVERSION = $(shell awk 'BEGIN  { "basename $(PWD)" | getline $$Y;gsub("Persia-","",$$Y);printf"%s",$$Y}')
#VERNAME = Eskilas-$(AGENTVERSION)
AGENTVERSION = ${BASEPWD}
VERNAME = ${BASEPWD}
OWNER = $(USER)
ARCHIVEDIR = ../Archive
#ARCHIVEDIR = ~/Archive

include $(TOPDIR)/Makefile.common

LDFLAGS += -L$(LIBDIR) -l$(MAINLIB)
LIBS += $(foreach DEPLIB,$(DEPLIBS),-l$(DEPLIB))
MAINLIB = Main

BINS = EskilasAgent
DEPS = $(foreach DEPLIB,$(DEPLIBS),$(LIBDIR)/lib$(DEPLIB).a)

clean tidy::
	rm -f .lib/lib$(MAINLIB).a

all clean tidy::
	@for I in $(DEPLIBS); do $(MAKE) -C $$I $@ || exit 1; done

all::
	$(CXX) -o $(BINS) $(CXXFLAGS) $(DEFS) Main.cpp $(LDFLAGS);
#	@if newer $(LIBDIR)/lib$(MAINLIB).a EskilasAgent || newer Main.cpp PersiaAgent; then \
#	fi

bz2:
	rm -rf /tmp/$(BASEPWD)
	cp -r $(PWD) /tmp/$(BASEPWD)
	cp -r /tmp/$(BASEPWD) $(PWD)
	rm -rf /tmp/$(BASEPWD)
	make -C ./$(BASEPWD) clean
	-tar -jcf ../$(VERNAME).tar.bz2 $(BASEPWD)/*
	rm -rf /tmp/$(BASEPWD)
	rm -rf $(BASEPWD)

archive:
	@mkdir -p $(ARCHIVEDIR)/$(OWNER)
	@if [ -e COMMENT ] ; then rm -f COMMENT ; fi
	@echo "Dist File :         Archive/$(OWNER)/Eskilas-$(AGENTVERSION)-`date +%Y_%m_%d-%H_%M`.tar.bz2" > COMMENT
	@echo "Code Description :  " >> COMMENT
	@kdialog --inputbox "Please enter a description for this release of Eskilas source code :" "<General Version>" >> COMMENT
	@echo -n "Package Host Name : " >> COMMENT
	@hostname >> COMMENT
	@echo -n "Release Date :      " >> COMMENT
	@date +%Y/%m/%d >> COMMENT
	@echo -n "Release Time :      " >> COMMENT
	@date +%H:%M\ \ %Z >> COMMENT
	@echo "Code Version :      $(AGENTVERSION)" >> COMMENT
	@echo "Code Owner :        $(OWNER)" >> COMMENT
	@cat COMMENT
	rm -r -f $(ARCHIVEDIR)/$(OWNER)/$(VERNAME)
	cp -r $(PWD) $(ARCHIVEDIR)/$(OWNER)/
	make -C $(ARCHIVEDIR)/$(OWNER)/$(VERNAME) clean
	rm -rf $(ARCHIVEDIR)/$(OWNER)/$(VERNAME)/Scripts
	@{ cd $(ARCHIVEDIR)/$(OWNER)/ ; tar -czf Eskilas-$(AGENTVERSION)-`date +%Y_%m_%d-%H_%M`.tar.gz $(VERNAME) ; cd $(PWD) ; }
	rm -rf $(ARCHIVEDIR)/$(OWNER)/$(VERNAME)
	@echo " Archive Created ============== \n"

pack: archive
	echo "Package Created !"

version:
	@echo $(AGENTVERSION)

doc:
	doxygen ./Documents/Doxyfile

touch:
	touch * */* */*/*
