#

include Makefile.common

subdirs		= mods vizor
backupfiles	= doc data vizor mods  Makefile* *.spec
rpmfiles	= lib doc vizor mods Makefile* *.spec

OBJS		= ./lib/gemsel.o ./lib/gemviz.o
#OBJS		= ./mods/*.o ./mods/submod/*.o ./vizor/*.o ./vizor/dlg/*.o ./vizor/kernel/*.o ./vizor/libs/*.o
		    

TARGET		= gems2

all:	subdirs_mk link


subdirs_mk:
	@for i in $(subdirs); do \
          echo Making all in $$i ; \
	  $(MAKE) -C $$i all RELEASE=$(RELEASE) || exit 1; \
        done

link:
	$(LINK_CMD) $(OBJS) $(LIBS) $(LIBDIRS) -o $(TARGET)


dep:
	@for i in $(subdirs); do \
	  $(MAKE) -C $$i dep || exit 1;\
        done


clean:
	@for i in $(subdirs); do \
	  $(MAKE) -C $$i clean || exit 1;\
        done
	- rm gems


install:	release
	


EXCLUDE_LIST=--exclude @* --exclude *.o --exclude *.bak --exclude *~ --exclude *.out \
--exclude *.rpo  --exclude tmp.moc.cpp --exclude CVS

backup: 
	 tar cv $(EXCLUDE_LIST) \
	    $(backupfiles) | bzip2 > gems-`date +%Y%m%d`.tar.bz2

backupd: 
	 tar cv ~/GEMsetup | bzip2 > gems_data-`date +%Y%m%d`.tar.bz2

# builds bianary package (rpm -bb)
rpm:
	mkdir /tmp/gems-1.93b
	tar c --exclude-from doc/tar_backup.exclude\
	GEMS.sys $(rpmfiles) | tar -xC /tmp/gems-1.93b
	cp gems-1.93b-6.spec /tmp
	tar Ccv /tmp gems-1.93b | bzip2 > /tmp/gems-1.93b.tar.bz2
	rm -fr /tmp/gems-1.93b
	su -c "mv /tmp/gems-1.93b.tar.bz2 /usr/src/RPM/SOURCES; \
		cp /tmp/gems-1.93b-6.spec /usr/src/RPM/SPECS; \
		cd /usr/src/RPM/SPECS; \
	    rpm -bb  gems-1.93b-6.spec"
	
