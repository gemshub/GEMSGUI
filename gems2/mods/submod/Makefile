#
# Makefile	Makefile for the systemV init suite.
#		Targets:   all      compiles everything
#		           install  installs the binaries (not the scripts)
#                          clean    cleans up
#
# Version:	@(#)Makefile  2.58  28-Dec-1995  MvS
#

include ../Makefile.mod_lin

INC_LOCAL	= -I../../vizor/kernel -I../ -I../submod -I../../vizor/libs -I../../vizor -I..
INCLUDES	= $(INC_LOCAL) $(INC_GLOBAL)
#FLAGS	= $(CCFLAGS) $(DEBUG_FLAGS)


SOURCES 	= ms_rmults.cpp ms_mtparm.cpp ms_multi.cpp ms_system.cpp \
         ms_sysbc.cpp ms_mupmph.cpp ms_muload.cpp ms_muleq.cpp ms_mueqexp.cpp \
         ipm_sel_bc.cpp ipm_gamma.cpp ipm_fia_bc.cpp ms_multi_file.cpp  \
         ms_multi_file.cpp

OBJECTS = $(subst .cpp,.o,$(SOURCES))

TARGET = submods.o

#

all:		$(TARGET)

$(TARGET):   $(OBJECTS)
	 $(LD)  -r $(OBJECTS) -o  $@

clean:
	$(CLEAN_CMD)

dep:
	$(MAKEDEP_CMD) $(INC_LOCAL) $(SOURCES)

%.o:	%.cpp
	 $(CC) $(FLAGS) $(INCLUDES) -c  $<

# DO NOT DELETE

ms_rmults.o: ../../vizor/kernel/v_object.h ../../vizor/libs/gstring.h
ms_rmults.o: ../../vizor/libs/array.h ../../vizor/kernel/v_user.h
ms_rmults.o: ../../vizor/libs/verror.h ../../vizor/kernel/v_vals.h
ms_rmults.o: ../m_param.h ../v_mod.h ../submod/ms_rmults.h
ms_rmults.o: ../../vizor/kernel/v_module.h ../../vizor/kernel/v_dbm.h
ms_rmults.o: ../../vizor/kernel/v_file.h ../submod/ms_mtparm.h
ms_rmults.o: ../submod/ms_system.h ../m_phase.h ../submod/ms_multi.h
ms_rmults.o: ../v_ipnc.h ../submod/ms_calc.h ../m_icomp.h ../m_dcomp.h
ms_rmults.o: ../s_tpwork.h ../m_compos.h ../m_reacdc.h ../s_formula.h
ms_rmults.o: ../../vizor/service.h ../../vizor/visor.h
ms_mtparm.o: ../../vizor/kernel/v_object.h ../../vizor/libs/gstring.h
ms_mtparm.o: ../../vizor/libs/array.h ../../vizor/kernel/v_user.h
ms_mtparm.o: ../../vizor/libs/verror.h ../../vizor/kernel/v_vals.h
ms_mtparm.o: ../m_param.h ../v_mod.h ../submod/ms_rmults.h
ms_mtparm.o: ../../vizor/kernel/v_module.h ../../vizor/kernel/v_dbm.h
ms_mtparm.o: ../../vizor/kernel/v_file.h ../submod/ms_mtparm.h
ms_mtparm.o: ../submod/ms_system.h ../m_phase.h ../submod/ms_multi.h
ms_mtparm.o: ../v_ipnc.h ../submod/ms_calc.h ../m_reacdc.h ../m_dcomp.h
ms_mtparm.o: ../s_tpwork.h ../../vizor/service.h ../../vizor/visor.h
ms_multi.o: ../m_param.h ../v_mod.h ../../vizor/kernel/v_vals.h
ms_multi.o: ../../vizor/libs/gstring.h ../../vizor/libs/array.h
ms_multi.o: ../submod/ms_rmults.h ../../vizor/kernel/v_module.h
ms_multi.o: ../../vizor/kernel/v_object.h ../../vizor/kernel/v_user.h
ms_multi.o: ../../vizor/libs/verror.h ../../vizor/kernel/v_dbm.h
ms_multi.o: ../../vizor/kernel/v_file.h ../submod/ms_mtparm.h
ms_multi.o: ../submod/ms_system.h ../m_phase.h ../submod/ms_multi.h
ms_multi.o: ../v_ipnc.h ../submod/ms_calc.h ../../vizor/visor.h
ms_system.o: ../m_param.h ../v_mod.h ../../vizor/kernel/v_vals.h
ms_system.o: ../../vizor/libs/gstring.h ../../vizor/libs/array.h
ms_system.o: ../submod/ms_rmults.h ../../vizor/kernel/v_module.h
ms_system.o: ../../vizor/kernel/v_object.h ../../vizor/kernel/v_user.h
ms_system.o: ../../vizor/libs/verror.h ../../vizor/kernel/v_dbm.h
ms_system.o: ../../vizor/kernel/v_file.h ../submod/ms_mtparm.h
ms_system.o: ../submod/ms_system.h ../m_phase.h ../submod/ms_multi.h
ms_system.o: ../v_ipnc.h ../submod/ms_calc.h ../m_syseq.h
ms_system.o: ../../vizor/service.h ../../vizor/visor.h
ms_sysbc.o: ../../vizor/kernel/v_object.h ../../vizor/libs/gstring.h
ms_sysbc.o: ../../vizor/libs/array.h ../../vizor/kernel/v_user.h
ms_sysbc.o: ../../vizor/libs/verror.h ../../vizor/kernel/v_vals.h
ms_sysbc.o: ../m_param.h ../v_mod.h ../submod/ms_rmults.h
ms_sysbc.o: ../../vizor/kernel/v_module.h ../../vizor/kernel/v_dbm.h
ms_sysbc.o: ../../vizor/kernel/v_file.h ../submod/ms_mtparm.h
ms_sysbc.o: ../submod/ms_system.h ../m_phase.h ../submod/ms_multi.h
ms_sysbc.o: ../v_ipnc.h ../submod/ms_calc.h ../m_compos.h ../m_syseq.h
ms_sysbc.o: ../s_formula.h ../../vizor/service.h ../../vizor/visor.h
ms_mupmph.o: ../m_param.h ../v_mod.h ../../vizor/kernel/v_vals.h
ms_mupmph.o: ../../vizor/libs/gstring.h ../../vizor/libs/array.h
ms_mupmph.o: ../submod/ms_rmults.h ../../vizor/kernel/v_module.h
ms_mupmph.o: ../../vizor/kernel/v_object.h ../../vizor/kernel/v_user.h
ms_mupmph.o: ../../vizor/libs/verror.h ../../vizor/kernel/v_dbm.h
ms_mupmph.o: ../../vizor/kernel/v_file.h ../submod/ms_mtparm.h
ms_mupmph.o: ../submod/ms_system.h ../m_phase.h ../submod/ms_multi.h
ms_mupmph.o: ../v_ipnc.h ../submod/ms_calc.h ../m_proces.h
ms_mupmph.o: ../../vizor/graph.h ../m_probe.h ../m_gtdemo.h ../s_formula.h
ms_mupmph.o: ../m_syseq.h ../../vizor/service.h ../../vizor/visor.h
ms_muload.o: ../m_param.h ../v_mod.h ../../vizor/kernel/v_vals.h
ms_muload.o: ../../vizor/libs/gstring.h ../../vizor/libs/array.h
ms_muload.o: ../submod/ms_rmults.h ../../vizor/kernel/v_module.h
ms_muload.o: ../../vizor/kernel/v_object.h ../../vizor/kernel/v_user.h
ms_muload.o: ../../vizor/libs/verror.h ../../vizor/kernel/v_dbm.h
ms_muload.o: ../../vizor/kernel/v_file.h ../submod/ms_mtparm.h
ms_muload.o: ../submod/ms_system.h ../m_phase.h ../submod/ms_multi.h
ms_muload.o: ../v_ipnc.h ../submod/ms_calc.h ../s_formula.h
ms_muload.o: ../../vizor/service.h ../../vizor/visor.h stepwise.h
ms_muleq.o: ../m_param.h ../v_mod.h ../../vizor/kernel/v_vals.h
ms_muleq.o: ../../vizor/libs/gstring.h ../../vizor/libs/array.h
ms_muleq.o: ../submod/ms_rmults.h ../../vizor/kernel/v_module.h
ms_muleq.o: ../../vizor/kernel/v_object.h ../../vizor/kernel/v_user.h
ms_muleq.o: ../../vizor/libs/verror.h ../../vizor/kernel/v_dbm.h
ms_muleq.o: ../../vizor/kernel/v_file.h ../submod/ms_mtparm.h
ms_muleq.o: ../submod/ms_system.h ../m_phase.h ../submod/ms_multi.h
ms_muleq.o: ../v_ipnc.h ../submod/ms_calc.h ../m_syseq.h
ms_muleq.o: ../../vizor/service.h ../../vizor/visor.h
ms_mueqexp.o: ../m_param.h ../v_mod.h ../../vizor/kernel/v_vals.h
ms_mueqexp.o: ../../vizor/libs/gstring.h ../../vizor/libs/array.h
ms_mueqexp.o: ../submod/ms_rmults.h ../../vizor/kernel/v_module.h
ms_mueqexp.o: ../../vizor/kernel/v_object.h ../../vizor/kernel/v_user.h
ms_mueqexp.o: ../../vizor/libs/verror.h ../../vizor/kernel/v_dbm.h
ms_mueqexp.o: ../../vizor/kernel/v_file.h ../submod/ms_mtparm.h
ms_mueqexp.o: ../submod/ms_system.h ../m_phase.h ../submod/ms_multi.h
ms_mueqexp.o: ../v_ipnc.h ../submod/ms_calc.h ../../vizor/service.h
ms_mueqexp.o: ../../vizor/visor.h stepwise.h
ipm_sel_bc.o: ../m_param.h ../v_mod.h ../../vizor/kernel/v_vals.h
ipm_sel_bc.o: ../../vizor/libs/gstring.h ../../vizor/libs/array.h
ipm_sel_bc.o: ../submod/ms_rmults.h ../../vizor/kernel/v_module.h
ipm_sel_bc.o: ../../vizor/kernel/v_object.h ../../vizor/kernel/v_user.h
ipm_sel_bc.o: ../../vizor/libs/verror.h ../../vizor/kernel/v_dbm.h
ipm_sel_bc.o: ../../vizor/kernel/v_file.h ../submod/ms_mtparm.h
ipm_sel_bc.o: ../submod/ms_system.h ../m_phase.h ../submod/ms_multi.h
ipm_sel_bc.o: ../v_ipnc.h ../submod/ms_calc.h ../../vizor/visor.h
ipm_sel_bc.o: ../../vizor/service.h stepwise.h
ipm_gamma.o: ../m_param.h ../v_mod.h ../../vizor/kernel/v_vals.h
ipm_gamma.o: ../../vizor/libs/gstring.h ../../vizor/libs/array.h
ipm_gamma.o: ../submod/ms_rmults.h ../../vizor/kernel/v_module.h
ipm_gamma.o: ../../vizor/kernel/v_object.h ../../vizor/kernel/v_user.h
ipm_gamma.o: ../../vizor/libs/verror.h ../../vizor/kernel/v_dbm.h
ipm_gamma.o: ../../vizor/kernel/v_file.h ../submod/ms_mtparm.h
ipm_gamma.o: ../submod/ms_system.h ../m_phase.h ../submod/ms_multi.h
ipm_gamma.o: ../v_ipnc.h ../submod/ms_calc.h
ipm_fia_bc.o: ../m_param.h ../v_mod.h ../../vizor/kernel/v_vals.h
ipm_fia_bc.o: ../../vizor/libs/gstring.h ../../vizor/libs/array.h
ipm_fia_bc.o: ../submod/ms_rmults.h ../../vizor/kernel/v_module.h
ipm_fia_bc.o: ../../vizor/kernel/v_object.h ../../vizor/kernel/v_user.h
ipm_fia_bc.o: ../../vizor/libs/verror.h ../../vizor/kernel/v_dbm.h
ipm_fia_bc.o: ../../vizor/kernel/v_file.h ../submod/ms_mtparm.h
ipm_fia_bc.o: ../submod/ms_system.h ../m_phase.h ../submod/ms_multi.h
ipm_fia_bc.o: ../v_ipnc.h ../submod/ms_calc.h ../../vizor/service.h
ms_multi_file.o: ../m_param.h ../v_mod.h ../../vizor/kernel/v_vals.h
ms_multi_file.o: ../../vizor/libs/gstring.h ../../vizor/libs/array.h
ms_multi_file.o: ../submod/ms_rmults.h ../../vizor/kernel/v_module.h
ms_multi_file.o: ../../vizor/kernel/v_object.h ../../vizor/kernel/v_user.h
ms_multi_file.o: ../../vizor/libs/verror.h ../../vizor/kernel/v_dbm.h
ms_multi_file.o: ../../vizor/kernel/v_file.h ../submod/ms_mtparm.h
ms_multi_file.o: ../submod/ms_system.h ../m_phase.h ../submod/ms_multi.h
ms_multi_file.o: ../v_ipnc.h ../submod/ms_calc.h
