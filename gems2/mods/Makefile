#
# Makefile	Makefile for the systemV init suite.
#		Targets:   all      compiles everything
#		           install  installs the binaries (not the scripts)
#
# Version:	@(#)Makefile  2.58  28-Dec-1995  MvS
#

include ./Makefile.mod_lin

subdirs = submod

INC_LOCAL	= -I../vizor/kernel -I../mods -I./submod -I../vizor/libs -I../vizor
INCLUDES	= $(INC_LOCAL) $(INC_GLOBAL)
#FLAGS	= $(CCFLAGS) $(DEBUG_FLAGS)


SOURCES 	= m_const.cpp m_sdata.cpp m_gem2mt.cpp  m_gem2mtc.cpp \
    m_param.cpp m_icomp.cpp v_ipnc.cpp \
    s_formula.cpp m_dcomp.cpp m_dcomp2.cpp \
    s_tpwork.cpp s_supcrt.cpp s_supcrt2.cpp \
    m_compos.cpp s_fgl.cpp s_fgl2.cpp s_fglbred.cpp \
    m_reacdc.cpp  m_phase.cpp \
    m_proces.cpp m_reac2.cpp  \
    m_prfget.cpp m_prfget2.cpp m_syseq.cpp \
    m_syst2.cpp m_unspace.cpp m_unsp_stat.cpp m_unsp_gen.cpp \
    m_dualth.cpp m_dualthc.cpp  \
    m_gtdemo.cpp m_rtparm.cpp

OBJECTS = $(subst .cpp,.o,$(SOURCES))

OBJ_SUB = ./submod/submods.o


TARGET	= ../lib/gemsel.o


.PHONY:	all subdirs_mk link

# rules

%.o:	%.cpp
	$(CC) $(FLAGS) $(INCLUDES) -c $<


all:	subdirs_mk $(TARGET)


subdirs_mk:
	for i in $(subdirs); do  \
	echo Making all in $$i ; \
	$(MAKE) -C $$i all RELEASE=$(RELEASE) || exit 1; \
	done


$(TARGET):    $(OBJECTS) $(OBJ_SUB)
	$(LD) $(LDFLAGS) $(OBJECTS) $(OBJ_SUB) -o $@


link:	$(TARGET)


clean:  
	@for i in $(subdirs); do  \
	 $(MAKE) -C $$i clean || exit 1; \
	done
	$(CLEAN_CMD)

dep:   
	for i in $(subdirs); do  \
	$(MAKE) -C $$i dep || exit 1; \
	done
	$(MAKEDEP_CMD) $(INC_LOCAL) $(SOURCES)

# DO NOT DELETE

m_const.o: m_const.h ../vizor/kernel/v_module.h ../vizor/kernel/v_object.h
m_const.o: ../vizor/libs/gstring.h ../vizor/libs/array.h
m_const.o: ../vizor/kernel/v_user.h ../vizor/libs/verror.h
m_const.o: ../vizor/kernel/v_vals.h ../vizor/kernel/v_dbm.h v_mod.h
m_sdata.o: m_sdata.h ../vizor/kernel/v_module.h ../vizor/kernel/v_object.h
m_sdata.o: ../vizor/libs/gstring.h ../vizor/libs/array.h
m_sdata.o: ../vizor/kernel/v_user.h ../vizor/libs/verror.h
m_sdata.o: ../vizor/kernel/v_vals.h ../vizor/kernel/v_dbm.h v_mod.h
m_sdata.o: ../vizor/service.h ../vizor/visor.h
m_integ.o: m_integ.h ../vizor/kernel/v_module.h ../vizor/kernel/v_object.h
m_integ.o: ../vizor/libs/gstring.h ../vizor/libs/array.h
m_integ.o: ../vizor/kernel/v_user.h ../vizor/libs/verror.h
m_integ.o: ../vizor/kernel/v_vals.h ../vizor/kernel/v_dbm.h v_ipnc.h
m_integ.o: ../vizor/visor.h ../vizor/service.h v_mod.h
m_param.o: m_unspace.h m_param.h v_mod.h ../vizor/kernel/v_vals.h
m_param.o: ../vizor/libs/gstring.h ../vizor/libs/array.h submod/ms_rmults.h
m_param.o: ../vizor/kernel/v_module.h ../vizor/kernel/v_object.h
m_param.o: ../vizor/kernel/v_user.h ../vizor/libs/verror.h
m_param.o: ../vizor/kernel/v_dbm.h submod/ms_mtparm.h submod/ms_system.h
m_param.o: m_phase.h submod/ms_multi.h submod/datach.h submod/databr.h
m_param.o: v_ipnc.h submod/ms_calc.h ../vizor/graph.h m_gtdemo.h m_syseq.h
m_param.o: m_dcomp.h s_tpwork.h m_icomp.h m_compos.h m_reacdc.h m_proces.h
m_param.o: m_dualth.h ../vizor/visor.h ../vizor/service.h
m_param.o: ../vizor/libs/gdatastream.h filters_data.h
m_icomp.o: m_icomp.h v_mod.h ../vizor/kernel/v_vals.h ../vizor/libs/gstring.h
m_icomp.o: ../vizor/libs/array.h ../vizor/kernel/v_module.h
m_icomp.o: ../vizor/kernel/v_object.h ../vizor/kernel/v_user.h
m_icomp.o: ../vizor/libs/verror.h ../vizor/kernel/v_dbm.h ../vizor/visor.h
m_icomp.o: ../vizor/service.h filters_data.h
v_ipnc.o: ../vizor/libs/gstring.h ../vizor/libs/array.h v_ipnc.h
v_ipnc.o: ../vizor/kernel/v_user.h ../vizor/libs/verror.h
v_ipnc.o: ../vizor/kernel/v_object.h ../vizor/kernel/v_vals.h
s_formula.o: s_formula.h ../vizor/libs/array.h ../vizor/kernel/v_user.h
s_formula.o: ../vizor/libs/gstring.h ../vizor/libs/verror.h v_mod.h
s_formula.o: ../vizor/kernel/v_vals.h m_icomp.h ../vizor/kernel/v_module.h
s_formula.o: ../vizor/kernel/v_object.h ../vizor/kernel/v_dbm.h
s_formula.o: ../vizor/service.h
m_dcomp.o: m_dcomp.h v_mod.h ../vizor/kernel/v_vals.h ../vizor/libs/gstring.h
m_dcomp.o: ../vizor/libs/array.h ../vizor/kernel/v_module.h
m_dcomp.o: ../vizor/kernel/v_object.h ../vizor/kernel/v_user.h
m_dcomp.o: ../vizor/libs/verror.h ../vizor/kernel/v_dbm.h s_tpwork.h
m_dcomp.o: m_param.h submod/ms_rmults.h submod/ms_mtparm.h submod/ms_system.h
m_dcomp.o: m_phase.h submod/ms_multi.h submod/datach.h submod/databr.h
m_dcomp.o: v_ipnc.h submod/ms_calc.h ../vizor/visor.h ../vizor/service.h
m_dcomp.o: s_formula.h s_supcrt.h s_fgl.h filters_data.h
m_dcomp2.o: m_dcomp.h v_mod.h ../vizor/kernel/v_vals.h
m_dcomp2.o: ../vizor/libs/gstring.h ../vizor/libs/array.h
m_dcomp2.o: ../vizor/kernel/v_module.h ../vizor/kernel/v_object.h
m_dcomp2.o: ../vizor/kernel/v_user.h ../vizor/libs/verror.h
m_dcomp2.o: ../vizor/kernel/v_dbm.h s_tpwork.h m_param.h submod/ms_rmults.h
m_dcomp2.o: submod/ms_mtparm.h submod/ms_system.h m_phase.h submod/ms_multi.h
m_dcomp2.o: submod/datach.h submod/databr.h v_ipnc.h submod/ms_calc.h
m_dcomp2.o: ../vizor/visor.h s_supcrt.h
s_tpwork.o: ../vizor/kernel/v_object.h ../vizor/libs/gstring.h
s_tpwork.o: ../vizor/libs/array.h ../vizor/kernel/v_user.h
s_tpwork.o: ../vizor/libs/verror.h ../vizor/kernel/v_vals.h s_tpwork.h
s_tpwork.o: v_mod.h
s_supcrt.o: s_supcrt.h ../vizor/libs/gstring.h ../vizor/libs/array.h v_mod.h
s_supcrt.o: ../vizor/kernel/v_vals.h s_tpwork.h ../vizor/kernel/v_object.h
s_supcrt.o: ../vizor/kernel/v_user.h ../vizor/libs/verror.h
s_supcrt2.o: ../vizor/kernel/v_user.h ../vizor/libs/gstring.h
s_supcrt2.o: ../vizor/libs/array.h ../vizor/libs/verror.h s_supcrt.h v_mod.h
s_supcrt2.o: ../vizor/kernel/v_vals.h s_tpwork.h ../vizor/kernel/v_object.h
m_compos.o: ../vizor/service.h ../vizor/libs/array.h ../vizor/libs/gstring.h
m_compos.o: ../vizor/kernel/v_user.h ../vizor/libs/verror.h
m_compos.o: ../vizor/kernel/v_object.h ../vizor/kernel/v_vals.h
m_compos.o: ../vizor/visor.h ../vizor/kernel/v_module.h
m_compos.o: ../vizor/kernel/v_dbm.h m_compos.h v_mod.h m_param.h
m_compos.o: submod/ms_rmults.h submod/ms_mtparm.h submod/ms_system.h
m_compos.o: m_phase.h submod/ms_multi.h submod/datach.h submod/databr.h
m_compos.o: v_ipnc.h submod/ms_calc.h m_icomp.h m_dcomp.h s_tpwork.h
m_compos.o: m_reacdc.h s_formula.h filters_data.h
s_fgl.o: s_fgl.h m_const.h ../vizor/kernel/v_module.h
s_fgl.o: ../vizor/kernel/v_object.h ../vizor/libs/gstring.h
s_fgl.o: ../vizor/libs/array.h ../vizor/kernel/v_user.h
s_fgl.o: ../vizor/libs/verror.h ../vizor/kernel/v_vals.h
s_fgl.o: ../vizor/kernel/v_dbm.h
s_fgl2.o: s_tpwork.h ../vizor/kernel/v_object.h ../vizor/libs/gstring.h
s_fgl2.o: ../vizor/libs/array.h ../vizor/kernel/v_user.h
s_fgl2.o: ../vizor/libs/verror.h ../vizor/kernel/v_vals.h v_mod.h s_fgl.h
s_fgl2.o: m_const.h ../vizor/kernel/v_module.h ../vizor/kernel/v_dbm.h
s_fglbred.o: m_const.h ../vizor/kernel/v_module.h ../vizor/kernel/v_object.h
s_fglbred.o: ../vizor/libs/gstring.h ../vizor/libs/array.h
s_fglbred.o: ../vizor/kernel/v_user.h ../vizor/libs/verror.h
s_fglbred.o: ../vizor/kernel/v_vals.h ../vizor/kernel/v_dbm.h s_fgl.h
m_reacdc.o: ../vizor/kernel/v_object.h ../vizor/libs/gstring.h
m_reacdc.o: ../vizor/libs/array.h ../vizor/kernel/v_user.h
m_reacdc.o: ../vizor/libs/verror.h ../vizor/kernel/v_vals.h m_reacdc.h
m_reacdc.o: v_mod.h ../vizor/kernel/v_module.h ../vizor/kernel/v_dbm.h
m_reacdc.o: m_dcomp.h s_tpwork.h m_param.h submod/ms_rmults.h
m_reacdc.o: submod/ms_mtparm.h submod/ms_system.h m_phase.h submod/ms_multi.h
m_reacdc.o: submod/datach.h submod/databr.h v_ipnc.h submod/ms_calc.h
m_reacdc.o: s_supcrt.h s_formula.h ../vizor/service.h ../vizor/visor.h
m_reacdc.o: filters_data.h
m_phase.o: ../vizor/kernel/v_object.h ../vizor/libs/gstring.h
m_phase.o: ../vizor/libs/array.h ../vizor/kernel/v_user.h
m_phase.o: ../vizor/libs/verror.h ../vizor/kernel/v_vals.h ../vizor/visor.h
m_phase.o: ../vizor/kernel/v_module.h ../vizor/kernel/v_dbm.h
m_phase.o: ../vizor/service.h m_dcomp.h v_mod.h s_tpwork.h m_reacdc.h
m_phase.o: m_phase.h m_param.h submod/ms_rmults.h submod/ms_mtparm.h
m_phase.o: submod/ms_system.h submod/ms_multi.h submod/datach.h
m_phase.o: submod/databr.h v_ipnc.h submod/ms_calc.h filters_data.h
m_proces.o: m_proces.h v_mod.h ../vizor/kernel/v_vals.h
m_proces.o: ../vizor/libs/gstring.h ../vizor/libs/array.h v_ipnc.h
m_proces.o: ../vizor/kernel/v_user.h ../vizor/libs/verror.h ../vizor/graph.h
m_proces.o: ../vizor/kernel/v_module.h ../vizor/kernel/v_object.h
m_proces.o: ../vizor/kernel/v_dbm.h m_syseq.h m_param.h submod/ms_rmults.h
m_proces.o: submod/ms_mtparm.h submod/ms_system.h m_phase.h submod/ms_multi.h
m_proces.o: submod/datach.h submod/databr.h submod/ms_calc.h
m_proces.o: ../vizor/service.h ../vizor/visor.h ../vizor/kernel/t_print.h
m_proces.o: ./submod/stepwise.h
m_reac2.o: m_param.h v_mod.h ../vizor/kernel/v_vals.h ../vizor/libs/gstring.h
m_reac2.o: ../vizor/libs/array.h submod/ms_rmults.h
m_reac2.o: ../vizor/kernel/v_module.h ../vizor/kernel/v_object.h
m_reac2.o: ../vizor/kernel/v_user.h ../vizor/libs/verror.h
m_reac2.o: ../vizor/kernel/v_dbm.h submod/ms_mtparm.h submod/ms_system.h
m_reac2.o: m_phase.h submod/ms_multi.h submod/datach.h submod/databr.h
m_reac2.o: v_ipnc.h submod/ms_calc.h m_reacdc.h m_dcomp.h s_tpwork.h
m_reac2.o: s_formula.h ../vizor/service.h ../vizor/visor.h
m_prfget.o: m_param.h v_mod.h ../vizor/kernel/v_vals.h
m_prfget.o: ../vizor/libs/gstring.h ../vizor/libs/array.h submod/ms_rmults.h
m_prfget.o: ../vizor/kernel/v_module.h ../vizor/kernel/v_object.h
m_prfget.o: ../vizor/kernel/v_user.h ../vizor/libs/verror.h
m_prfget.o: ../vizor/kernel/v_dbm.h submod/ms_mtparm.h submod/ms_system.h
m_prfget.o: m_phase.h submod/ms_multi.h submod/datach.h submod/databr.h
m_prfget.o: v_ipnc.h submod/ms_calc.h m_unspace.h ../vizor/graph.h m_syseq.h
m_prfget.o: ../vizor/service.h ../vizor/visor.h
m_prfget2.o: m_param.h v_mod.h ../vizor/kernel/v_vals.h
m_prfget2.o: ../vizor/libs/gstring.h ../vizor/libs/array.h submod/ms_rmults.h
m_prfget2.o: ../vizor/kernel/v_module.h ../vizor/kernel/v_object.h
m_prfget2.o: ../vizor/kernel/v_user.h ../vizor/libs/verror.h
m_prfget2.o: ../vizor/kernel/v_dbm.h submod/ms_mtparm.h submod/ms_system.h
m_prfget2.o: m_phase.h submod/ms_multi.h submod/datach.h submod/databr.h
m_prfget2.o: v_ipnc.h submod/ms_calc.h m_unspace.h ../vizor/graph.h m_syseq.h
m_prfget2.o: ../vizor/service.h
m_syseq.o: m_syseq.h m_param.h v_mod.h ../vizor/kernel/v_vals.h
m_syseq.o: ../vizor/libs/gstring.h ../vizor/libs/array.h submod/ms_rmults.h
m_syseq.o: ../vizor/kernel/v_module.h ../vizor/kernel/v_object.h
m_syseq.o: ../vizor/kernel/v_user.h ../vizor/libs/verror.h
m_syseq.o: ../vizor/kernel/v_dbm.h submod/ms_mtparm.h submod/ms_system.h
m_syseq.o: m_phase.h submod/ms_multi.h submod/datach.h submod/databr.h
m_syseq.o: v_ipnc.h submod/ms_calc.h ../vizor/service.h ../vizor/visor.h
m_syst2.o: m_syseq.h m_param.h v_mod.h ../vizor/kernel/v_vals.h
m_syst2.o: ../vizor/libs/gstring.h ../vizor/libs/array.h submod/ms_rmults.h
m_syst2.o: ../vizor/kernel/v_module.h ../vizor/kernel/v_object.h
m_syst2.o: ../vizor/kernel/v_user.h ../vizor/libs/verror.h
m_syst2.o: ../vizor/kernel/v_dbm.h submod/ms_mtparm.h submod/ms_system.h
m_syst2.o: m_phase.h submod/ms_multi.h submod/datach.h submod/databr.h
m_syst2.o: v_ipnc.h submod/ms_calc.h ../vizor/visor.h
m_unspace.o: m_unspace.h m_param.h v_mod.h ../vizor/kernel/v_vals.h
m_unspace.o: ../vizor/libs/gstring.h ../vizor/libs/array.h submod/ms_rmults.h
m_unspace.o: ../vizor/kernel/v_module.h ../vizor/kernel/v_object.h
m_unspace.o: ../vizor/kernel/v_user.h ../vizor/libs/verror.h
m_unspace.o: ../vizor/kernel/v_dbm.h submod/ms_mtparm.h submod/ms_system.h
m_unspace.o: m_phase.h submod/ms_multi.h submod/datach.h submod/databr.h
m_unspace.o: v_ipnc.h submod/ms_calc.h ../vizor/graph.h m_syseq.h
m_unspace.o: ../vizor/service.h ../vizor/visor.h
m_unsp_stat.o: m_unspace.h m_param.h v_mod.h ../vizor/kernel/v_vals.h
m_unsp_stat.o: ../vizor/libs/gstring.h ../vizor/libs/array.h
m_unsp_stat.o: submod/ms_rmults.h ../vizor/kernel/v_module.h
m_unsp_stat.o: ../vizor/kernel/v_object.h ../vizor/kernel/v_user.h
m_unsp_stat.o: ../vizor/libs/verror.h ../vizor/kernel/v_dbm.h
m_unsp_stat.o: submod/ms_mtparm.h submod/ms_system.h m_phase.h
m_unsp_stat.o: submod/ms_multi.h submod/datach.h submod/databr.h v_ipnc.h
m_unsp_stat.o: submod/ms_calc.h ../vizor/graph.h ../vizor/visor.h
m_unsp_gen.o: m_unspace.h m_param.h v_mod.h ../vizor/kernel/v_vals.h
m_unsp_gen.o: ../vizor/libs/gstring.h ../vizor/libs/array.h
m_unsp_gen.o: submod/ms_rmults.h ../vizor/kernel/v_module.h
m_unsp_gen.o: ../vizor/kernel/v_object.h ../vizor/kernel/v_user.h
m_unsp_gen.o: ../vizor/libs/verror.h ../vizor/kernel/v_dbm.h
m_unsp_gen.o: submod/ms_mtparm.h submod/ms_system.h m_phase.h
m_unsp_gen.o: submod/ms_multi.h submod/datach.h submod/databr.h v_ipnc.h
m_unsp_gen.o: submod/ms_calc.h ../vizor/graph.h s_formula.h m_syseq.h
m_unsp_gen.o: ../vizor/service.h ../vizor/visor.h
m_dualth.o: m_dualth.h m_param.h v_mod.h ../vizor/kernel/v_vals.h
m_dualth.o: ../vizor/libs/gstring.h ../vizor/libs/array.h submod/ms_rmults.h
m_dualth.o: ../vizor/kernel/v_module.h ../vizor/kernel/v_object.h
m_dualth.o: ../vizor/kernel/v_user.h ../vizor/libs/verror.h
m_dualth.o: ../vizor/kernel/v_dbm.h submod/ms_mtparm.h submod/ms_system.h
m_dualth.o: m_phase.h submod/ms_multi.h submod/datach.h submod/databr.h
m_dualth.o: v_ipnc.h submod/ms_calc.h m_syseq.h ../vizor/service.h
m_dualth.o: ../vizor/visor.h
m_dualthc.o: m_dualth.h m_param.h v_mod.h ../vizor/kernel/v_vals.h
m_dualthc.o: ../vizor/libs/gstring.h ../vizor/libs/array.h submod/ms_rmults.h
m_dualthc.o: ../vizor/kernel/v_module.h ../vizor/kernel/v_object.h
m_dualthc.o: ../vizor/kernel/v_user.h ../vizor/libs/verror.h
m_dualthc.o: ../vizor/kernel/v_dbm.h submod/ms_mtparm.h submod/ms_system.h
m_dualthc.o: m_phase.h submod/ms_multi.h submod/datach.h submod/databr.h
m_dualthc.o: v_ipnc.h submod/ms_calc.h m_syseq.h m_icomp.h m_compos.h
m_dualthc.o: s_formula.h ../vizor/service.h ../vizor/visor.h
m_gtdemo.o: m_proces.h v_mod.h ../vizor/kernel/v_vals.h
m_gtdemo.o: ../vizor/libs/gstring.h ../vizor/libs/array.h v_ipnc.h
m_gtdemo.o: ../vizor/kernel/v_user.h ../vizor/libs/verror.h ../vizor/graph.h
m_gtdemo.o: ../vizor/kernel/v_module.h ../vizor/kernel/v_object.h
m_gtdemo.o: ../vizor/kernel/v_dbm.h m_unspace.h m_param.h submod/ms_rmults.h
m_gtdemo.o: submod/ms_mtparm.h submod/ms_system.h m_phase.h submod/ms_multi.h
m_gtdemo.o: submod/datach.h submod/databr.h submod/ms_calc.h m_gtdemo.h
m_gtdemo.o: ../vizor/service.h ../vizor/visor.h
m_rtparm.o: m_rtparm.h v_mod.h ../vizor/kernel/v_vals.h
m_rtparm.o: ../vizor/libs/gstring.h ../vizor/libs/array.h
m_rtparm.o: ../vizor/kernel/v_module.h ../vizor/kernel/v_object.h
m_rtparm.o: ../vizor/kernel/v_user.h ../vizor/libs/verror.h
m_rtparm.o: ../vizor/kernel/v_dbm.h v_ipnc.h ../vizor/graph.h m_param.h
m_rtparm.o: submod/ms_rmults.h submod/ms_mtparm.h submod/ms_system.h
m_rtparm.o: m_phase.h submod/ms_multi.h submod/datach.h submod/databr.h
m_rtparm.o: submod/ms_calc.h m_dcomp.h s_tpwork.h m_reacdc.h m_sdata.h
m_rtparm.o: ../vizor/visor.h ../vizor/service.h
