# GNU standard requirements
# SHELL=/bin/sh
# compile variables

NAME		= gems
VERSION		= 2.1.0

ifeq ("x$(QTDIR)","x")
# QTDIR           = /home/gems/qt3
QTDIR		= /usr/lib/qt3
# QTDIR		= /usr/local/qt
endif

DEBUG_FLAGS	= -ggdb
PROF_FLAGS	= 

LIBDIRS		= -L$(QTDIR)/lib -L/usr/X11R6/lib
LDFLAGS		= -m elf_i386

ifeq ($(RELEASE), YES)
QTLIBS		= $(QTDIR)/lib/libqt-mt.a
else
QTLIBS		= -lqt-mt
endif

ifeq ("$(shell uname)", "Darwin")
LIBS		= $(QTLIBS) -lz -framework Carbon
else
LIBS		= $(QTLIBS) -lz -lX11 -lXext -lXt -lm -lstdc++ -lpthread 
ifeq ($(RELEASE), YES)
LIBS		+= -lXft -lXcursor -lXinerama -lXrandr
endif
endif

ifeq ("$(shell uname)", "Darwin")
SHARED_SRC_DIR	= gems2.app/Contents/Resources
else
SHARED_SRC_DIR	= shared
endif

# commands
CC		= c++ -m32 -Wl,-melf_i386
LD		= ld

ifeq ($(RELEASE),YES)
 LINK_CMD	= $(CC)
else
ifeq ($(PROFILE),YES)
 #flag for making profile version of GEMS
 LINK_CMD	= $(CC) $(DEBUG_FLAGS) $(PROF_FLAGS)
else
 #flags for making usual debug version
 LINK_CMD	= $(CC) $(DEBUG_FLAGS)
endif
endif


subdirs		= mods vizor
backupfiles	= doc data vizor mods  Makefile* *.spec
rpmfiles	= lib doc vizor mods shared img projects Makefile* *.spec

ifeq ("$(shell ls /usr/src/packages -d)", "/usr/src/packages")
RPMDIR=/usr/src/packages
else
RPMDIR=/usr/src/RPM
endif



OBJS		= ./lib/gemsel.o ./lib/gemviz.o
		    
ifeq ("$(shell uname)", "Darwin")
TARGET		= gems2.app/Contents/MacOS/gems2
else
TARGET		= gems2
endif

all:	subdirs_mk $(TARGET)


subdirs_mk:
	@for i in $(subdirs); do \
          echo Making all in $$i ; \
	  $(MAKE) -C $$i all RELEASE=$(RELEASE) || exit 1; \
        done

$(TARGET):	$(OBJS)
	@if [ "`uname`" == "Darwin" ]; then \
	  install -d gems2.app/Contents/MacOS; \
	fi
	$(LINK_CMD) $(OBJS) $(LIBS) $(LIBDIRS) -o $(TARGET) 


link:	$(TARGET)


dep:
	@for i in $(subdirs); do \
	  $(MAKE) -C $$i dep || exit 1;\
        done


clean:
	@for i in $(subdirs); do \
	  $(MAKE) -C $$i clean || exit 1;\
        done
	- rm $(TARGET)


install:	release
	


EXCLUDE_LIST=--exclude *Data.* --exclude .* --exclude @* --exclude *.o --exclude *.bak --exclude *~ --exclude *.out \
--exclude *.rpo  --exclude tmp.moc.cpp --exclude CVS

backup: 
	 tar cv $(EXCLUDE_LIST) \
	    $(backupfiles) | bzip2 > gems-`date +%Y%m%d`.tar.bz2

backupd: 
	 tar cv ~/GEMS2test | bzip2 > gems_data-`date +%Y%m%d`.tar.bz2

# builds binary package (rpm -bb)
rpm:	check_shared
	mkdir -p /tmp/$(NAME)-$(VERSION)
	tar c --exclude-from doc/tar_backup.exclude $(rpmfiles) | tar -xC /tmp/$(NAME)-$(VERSION)
#	cp -fr /usr/share/gems2 /tmp/$(NAME)-$(VERSION)/shared
	tar Ccv /tmp --remove-files $(NAME)-$(VERSION) | bzip2 > /tmp/$(NAME)-$(VERSION).tar.bz2
	cp gems2.spec /tmp
#	rm -fr ./program
#	rm -fr /tmp/gems-2.0
	su -c "	mv /tmp/$(NAME)-$(VERSION).tar.bz2 $(RPMDIR)/SOURCES; \
		mv /tmp/gems2.spec $(RPMDIR)/SPECS; \
		cd $(RPMDIR)/SPECS; \
		rpmbuild -bb gems2.spec"
	
ifeq ("$(shell uname)", "Darwin")

copy_data:
		cp -R ./data ./gems2.app/Contents/Resources/
else

copy_data:	
		cp -R ./data ./shared/ 
endif	

zip:
# check_shared
	-mv ~/Desktop/gems2-mac-`date +%Y%m%d`.zip ~/Desktop/gems2-mac-`date +%Y%m%d`.zip.bak
	zip -r ~/Desktop/gems2-mac-`date +%Y%m%d`.zip gems2.app/Contents/MacOS gems2.app/Contents/Resources gems2.app/Contents/Info.plist gems2.app/Contents/PkgInfo -x gems2.app/Contents/Resources/CVS/*


check_shared:
	if [ -d $(SHARED_SRC_DIR)/DB.default ]; then /bin/true; else /bin/false; fi
	if [ -d $(SHARED_SRC_DIR)/projects/TryNPTDB ]; then /bin/true; else /bin/false; fi
	if [ -d $(SHARED_SRC_DIR)/data ]; then /bin/true; else /bin/false; fi
	if [ -d $(SHARED_SRC_DIR)/visor.data ]; then /bin/true; else /bin/false; fi
	if [ -d $(SHARED_SRC_DIR)/doc/html ]; then /bin/true; else /bin/false; fi
	if [ -d $(SHARED_SRC_DIR)/doc/pdf ]; then /bin/true; else /bin/false; fi
	if [ -d $(SHARED_SRC_DIR)/doc/txt ]; then /bin/true; else /bin/false; fi
	if [ -d $(SHARED_SRC_DIR)/img ]; then /bin/true; else /bin/false; fi
	if [ -d $(SHARED_SRC_DIR)/data ]; then /bin/true; else /bin/false; fi
	if [ -f $(SHARED_SRC_DIR)/img/gems32.xpm ]; then /bin/true; else /bin/false; fi
	if [ -f doc/gems2.desktop ]; then /bin/true; else /bin/false; fi
