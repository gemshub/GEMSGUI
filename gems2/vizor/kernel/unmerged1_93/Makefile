#
# Makefile	Makefile for the systemV init suite.
#		Targets:   all      compiles everything
#		           install  installs the binaries (not the scripts)
#                          clean    cleans up
#
# Version:	@(#)Makefile  2.58  28-Dec-1995  MvS
#

!include ../Makefile.qt

INC_L	= -I../core -I../../mods -I../libs  -I../
INC	= $(INC_L) $(INCLUDES)
FLAGS	= $(CCFLAGS) $(DEBUG_FLAGS)
# $(PROF_FLAGS)

OBJECTS = v_user.obj v_vals.obj v_object.obj v_ipnc.obj \
    v_file.obj v_dbm1.obj v_dbm2.obj t_print.obj v_module.obj
SRC	= v_user.cpp v_vals.cpp v_object.cpp v_ipnc.cpp \
    v_file.cpp v_dbm1.cpp v_dbm2.cpp t_print.cpp v_mpdule.cpp

TARGET	= core.lib


all:		$(OBJECTS)


.h.cpp:

.cpp.obj:
	$(CC) $(FLAGS) $(INC) -c $<


clean:
#	$(CLEAN_CMD)

