#
# Makefile	Makefile for the systemV init suite.
#		Targets:   all      compiles everything
#		           install  installs the binaries (not the scripts)
#                          clean    cleans up
#
# Version:	@(#)Makefile  2.58  06-Sep-2001  MvS
#

include ../Makefile.qt

INC_LOCAL	= -I../kernel -I../../mods -I../libs  -I../
INCLUDES	= $(INC_LOCAL) $(INC_GLOBAL)
#FLAGS	= $(CCFLAGS) $(DEBUG_FLAGS)
# $(PROF_FLAGS)

SOURCES	= v_user.cpp v_vals.cpp v_object.cpp \
    v_file.cpp v_dbm1.cpp v_dbm2.cpp t_print.cpp t_read.cpp v_module.cpp

OBJECTS = $(subst .cpp,.o,$(SOURCES))

TARGET	= kernel.o


all:		$(TARGET)

$(TARGET):      $(OBJECTS) Makefile
	$(LD) -r $(OBJECTS) -o $@

clean:
	$(CLEAN_CMD)

dep:
	$(MAKEDEP_CMD) $(INC_LOCAL) $(SOURCES)

%.o:	%.cpp
	$(CC) $(FLAGS) $(INCLUDES) -c $<


# DO NOT DELETE

v_user.o: v_user.h ../libs/gstring.h ../libs/array.h ../libs/verror.h
v_vals.o: v_user.h ../libs/gstring.h ../libs/array.h ../libs/verror.h
v_vals.o: v_vals.h v_vals_impl.h
v_object.o: v_vals_impl.h v_vals.h ../libs/gstring.h ../libs/array.h v_user.h
v_object.o: ../libs/verror.h v_object.h ../../mods/m_param.h
v_object.o: ../../mods/v_mod.h ../../mods/submod/ms_rmults.h v_module.h
v_object.o: v_dbm.h v_file.h ../../mods/submod/ms_mtparm.h
v_object.o: ../../mods/submod/ms_system.h ../../mods/m_phase.h
v_object.o: ../../mods/submod/ms_multi.h ../../mods/v_ipnc.h
v_object.o: ../../mods/submod/ms_calc.h ../libs/config.h
v_file.o: v_file.h ../libs/gstring.h ../libs/array.h v_user.h
v_file.o: ../libs/verror.h
v_dbm1.o: v_dbm.h ../libs/gstring.h ../libs/array.h v_file.h v_user.h
v_dbm1.o: ../libs/verror.h v_object.h v_vals.h
v_dbm2.o: v_object.h ../libs/gstring.h ../libs/array.h v_user.h
v_dbm2.o: ../libs/verror.h v_vals.h v_dbm.h v_file.h ../../mods/v_mod.h
v_dbm2.o: ../visor.h v_module.h
t_print.o: t_print.h v_object.h ../libs/gstring.h ../libs/array.h v_user.h
t_print.o: ../libs/verror.h v_vals.h ../../mods/v_ipnc.h v_dbm.h v_file.h
t_print.o: ../service.h
t_read.o: t_read.h v_object.h ../libs/gstring.h ../libs/array.h v_user.h
t_read.o: ../libs/verror.h v_vals.h ../../mods/v_ipnc.h v_dbm.h v_file.h
t_read.o: ../service.h
v_module.o: v_module.h v_object.h ../libs/gstring.h ../libs/array.h v_user.h
v_module.o: ../libs/verror.h v_vals.h v_dbm.h v_file.h ../module_w.h
v_module.o: ../visor_w.h ../graph.h ../visor.h ../service.h
v_module.o: ../../mods/v_mod.h t_print.h ../../mods/v_ipnc.h t_read.h
v_module.o: ../dlg/NewSystemDialog.h ../dlg/NewSystemDialogData.h
