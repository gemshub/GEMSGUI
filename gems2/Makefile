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
	- rm gems2


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
	mkdir -p /tmp/gems-2.0b
	tar c --exclude-from doc/tar_backup.exclude $(rpmfiles) | tar -xC /tmp/gems-2.0b
	cp -fr $(HOME)/GEMS2test/program /tmp/gems-2.0b
	tar Ccv /tmp --remove-files gems-2.0b | bzip2 > /tmp/gems-2.0b.tar.bz2
	cp gems2.spec /tmp
#	rm -fr ./program
#	rm -fr /tmp/gems-2.0b
	su -c "	mv /tmp/gems-2.0b.tar.bz2 /usr/src/RPM/SOURCES; \
		mv /tmp/gems2.spec /usr/src/RPM/SPECS; \
		cd /usr/src/RPM/SPECS; \
		rpm -bb gems2.spec"
	
