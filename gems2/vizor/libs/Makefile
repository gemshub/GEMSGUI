#
# Makefile	Makefile for the systemV init suite.
#		Targets:   all      compiles everything
#		           install  installs the binaries (not the scripts)
#                          clean    cleans up
#
# Version:	@(#)Makefile  2.58  06-Sep-2001  MvS
#

include ../Makefile.qt

INC_LOCAL	= -I../kernel -I../libs
INCLUDES	= $(INC_LOCAL) $(INC_GLOBAL)
#FLAGS		= $(CCFLAGS) $(DEBUG_FLAGS)

OBJECTS = config.o gstring.o  gdatastream.o
SOURCES	= config.cpp gstring.cpp gdatastream.cpp


all:		$(OBJECTS)

$(OBJECTS):	Makefile

clean:
	$(CLEAN_CMD)

dep:
	$(MAKEDEP_CMD) $(INC_LOCAL) $(SOURCES)


%.o:	%.cpp
	 $(CC) $(FLAGS) $(INCLUDES) -c $<

# DO NOT DELETE

config.o: config.h gstring.h array.h ../kernel/v_user.h verror.h
gstring.o: gstring.h array.h
gdatastream.o: gdatastream.h gstring.h array.h verror.h
