# GNU standard requirements
# SHELL=/bin/sh
# compile variables

NAME		= gems2
VERSION		= 2.0

ifeq ("x$(QTDIR)","x")
QTDIR		= /usr/lib/qt3
#QTDIR		= /usr/local/qt
endif

DEBUG_FLAGS	= -ggdb
PROF_FLAGS	= 

LIBDIRS		= -L$(QTDIR)/lib -L/usr/X11R6/lib
LDFLAGS		=

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


# commands
CC		= c++
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
rpmfiles	= lib doc vizor mods shared Makefile* *.spec

OBJS		= ./lib/gemsel.o ./lib/gemviz.o
		    
ifeq ("$(shell uname)", "Darwin")
TARGET		= gems2.app/Contents/MacOS/gems2
else
TARGET		= gems2
endif

all:	subdirs_mk link


subdirs_mk:
	@for i in $(subdirs); do \
          echo Making all in $$i ; \
	  $(MAKE) -C $$i all RELEASE=$(RELEASE) || exit 1; \
        done

link:
	@if [ "`uname`" == "Darwin" ]; then \
	  install -d gems2.app/Contents/MacOS; \
	fi
	$(LINK_CMD) $(OBJS) $(LIBS) $(LIBDIRS) -o $(TARGET)


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

# builds bianary package (rpm -bb)
rpm:
	mkdir -p /tmp/$(NAME)-$(VERSION)
	tar c --exclude-from doc/tar_backup.exclude $(rpmfiles) | tar -xC /tmp/$(NAME)-$(VERSION)
#	cp -fr /usr/share/gems2 /tmp/$(NAME)-$(VERSION)/shared
	tar Ccv /tmp --remove-files $(NAME)-$(VERSION) | bzip2 > /tmp/$(NAME)-$(VERSION).tar.bz2
	cp gems2.spec /tmp
#	rm -fr ./program
#	rm -fr /tmp/gems-2.0
	su -c "	mv /tmp/$(NAME)-$(VERSION).tar.bz2 /usr/src/RPM/SOURCES; \
		mv /tmp/gems2.spec /usr/src/RPM/SPECS; \
		cd /usr/src/RPM/SPECS; \
		rpm -bb gems2.spec"
	
zip:
	-mv ~/Desktop/gems2-mac-`date +%Y%m%d`.zip ~/Desktop/gems2-mac-`date +%Y%m%d`.zip.bak
	zip -r ~/Desktop/gems2-mac-`date +%Y%m%d`.zip gems2.app/Contents/MacOS gems2.app/Contents/Resources gems2.app/Contents/Info.plist gems2.app/Contents/PkgInfo -x gems2.app/Contents/Resources/CVS/*

