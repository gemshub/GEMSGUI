#
# Makefile	Makefile for the systemV init suite.
#		Targets:   all      compiles everything
#		           install  installs the binaries (not the scripts)
#                          clean    cleans up
#
# Version:	@(#)Makefile  2.58  28-Dec-1995  MvS
#

include ./Makefile.mod_lin

INC_LOCAL	= -I. -I../../vizor/kernel -I../ -I../submod -I../../vizor/libs -I../../vizor -I..
INCLUDES	= $(INC_LOCAL) $(INC_GLOBAL)
#FLAGS	= $(CCFLAGS) $(DEBUG_FLAGS)


SOURCES         = ../../vizor/libs/gstring.cpp ../../vizor/libs/gdatastream.cpp \
	main.cpp ms_param.cpp \
	../submod/ms_mueqexp.cpp \
	../submod/ipm_sel_bc.cpp ../submod/ipm_gamma.cpp \
	../submod/ipm_fia_bc.cpp ../submod/ms_multi_file.cpp \
	../s_fgl.cpp ../submod/ms_datach_br.cpp

SOURCES         = ../../vizor/libs/gstring.o ../../vizor/libs/gdatastream.o \
	main.o ms_param.o \
	ms_mueqexp.o \
	ipm_sel_bc.o ipm_gamma.o \
	ipm_fia_bc.o ms_multi_file.o \
	s_fgl.o ms_datach_br.o

OBJECTS = $(subst .cpp,.o,$(SOURCES))


TARGET = gemipm2

#

all:		$(TARGET)

$(TARGET):   $(OBJECTS)
	g++ $(OBJECTS) -o $@

clean:
	$(CLEAN_CMD)

dep:
	$(MAKEDEP_CMD) $(INC_LOCAL) $(SOURCES)


ms_mueqexp.o:		../submod/ms_mueqexp.cpp
	$(CC) $(FLAGS) $(INCLUDES) -c  $<

ipm_sel_bc.o:	../submod/ipm_sel_bc.cpp
	$(CC) $(FLAGS) $(INCLUDES) -c  $<

ipm_gamma.o:		../submod/ipm_gamma.cpp
	$(CC) $(FLAGS) $(INCLUDES) -c  $<

ipm_fia_bc.o:		../submod/ipm_fia_bc.cpp
	$(CC) $(FLAGS) $(INCLUDES) -c  $<

ms_multi_file.o:	../submod/ms_multi_file.cpp
	$(CC) $(FLAGS) $(INCLUDES) -c  $<

s_fgl.o:	../s_fgl.cpp
	$(CC) $(FLAGS) $(INCLUDES) -c  $<

ms_datach_br.o:	../submod/ms_datach_br.cpp
	$(CC) $(FLAGS) $(INCLUDES) -c  $<


%.o:	%.cpp
	$(CC) $(FLAGS) $(INCLUDES) -c $< -o $@

# DO NOT DELETE

main.o: ../../vizor/libs/verror.h ../../vizor/libs/gstring.h
main.o: ../../vizor/libs/array.h m_param.h ../../vizor/libs/gdatastream.h
main.o: ../submod/ms_multi.h ../submod/datach.h ../submod/databr.h m_const.h
ms_param.o: m_param.h ../../vizor/libs/gdatastream.h
ms_param.o: ../../vizor/libs/gstring.h ../../vizor/libs/array.h
ms_param.o: ../submod/ms_multi.h ../submod/datach.h ../submod/databr.h
ms_param.o: m_const.h ../../vizor/libs/verror.h
