# Makefile for GEMS Vizor
# 
# 

include ./Makefile.qt

subdirs    =  libs kernel dlg


INC_LOCAL	= -I./kernel -I./libs -I../mods -I./
INCLUDES	= $(INC_LOCAL) $(INC_GLOBAL)
#FLAGS	= $(CCFLAGS) $(DEBUG_FLAGS)


SOURCES	= page_f.cpp \
          visor.cpp \
          main.cpp \
          visor_w.cpp \
          module_w.cpp \
          page_w.cpp \
          page_win_w.cpp \
          pshape.cpp \
          help.cpp \
          units.cpp \
          helpwindow.cpp \
          graph.cpp

OBJECTS = $(subst .cpp,.o,$(SOURCES))

METAOBJ = moc_visor_w.o \
          moc_module_w.o  \
          moc_page_w.o \
          moc_query_w.o \
          moc_page_win_w.o  \
          moc_helpwindow.o \
          moc_graph.o



OBJ_SUB   = ./kernel/kernel.o ./dlg/dlg.o ./libs/config.o  ./libs/gstring.o


TARGET	= ../lib/gemviz.o


.PHONY: subdirs_mk all

all:      $(TARGET)

$(TARGET): subdir_mk $(OBJECTS) $(METAOBJ) $(OBJ_SUB) Makefile

	$(LD) -r $(OBJECTS) $(METAOBJ) $(OBJ_SUB) -o $@   

subdir_mk:
	@for i in $(subdirs); do \
         echo Making all in $$i ;  \
	 $(MAKE) -C $$i all RELEASE=$(RELEASE) || exit 1; \
        done     

clean:  
	@for i in $(subdirs); do \
	$(MAKE) -C $$i clean || exit 1; \
        done     
	$(CLEAN_CMD)

dep:  
	@for i in $(subdirs); do \
	$(MAKE) -C $$i dep || exit 1; \
	done     
	$(MAKEDEP_CMD) $(INC_LOCAL) $(SOURCES)

# rules

%.o:	%.cpp
	$(CC) $(FLAGS) $(INCLUDES) -c $<

# DO NOT DELETE

page_f.o: page_s.h ./kernel/v_object.h ./libs/gstring.h ./libs/array.h
page_f.o: ./kernel/v_user.h ./libs/verror.h ./kernel/v_vals.h
page_f.o: ./kernel/v_module.h ./kernel/v_dbm.h ./kernel/v_file.h
page_f.o: ./libs/config.h units.h
visor.o: ./libs/config.h ./libs/gstring.h ./libs/array.h service.h
visor.o: ./kernel/v_user.h ./libs/verror.h visor.h ./kernel/v_module.h
visor.o: ./kernel/v_object.h ./kernel/v_vals.h ./kernel/v_dbm.h
visor.o: ./kernel/v_file.h units.h visor_w.h ../mods/setthread.h graph.h
visor.o: page_win_w.h page_s.h ../mods/m_const.h ../mods/m_sdata.h
visor.o: ../mods/v_mod.h ../mods/m_integ.h ./kernel/v_ipnc.h
visor.o: ../mods/m_param.h ../mods/submod/ms_rmults.h
visor.o: ../mods/submod/ms_mtparm.h ../mods/submod/ms_system.h
visor.o: ../mods/m_phase.h ../mods/submod/ms_multi.h ../mods/submod/ms_calc.h
visor.o: ../mods/m_icomp.h ../mods/m_dcomp.h ../mods/s_tpwork.h
visor.o: ../mods/m_compos.h ../mods/m_reacdc.h ../mods/m_rtparm.h
visor.o: ../mods/m_syseq.h ../mods/m_proces.h ../mods/m_probe.h
visor.o: ../mods/m_gtdemo.h ../mods/m_duterm.h
main.o: visor.h ./libs/gstring.h ./libs/array.h ./kernel/v_module.h
main.o: ./kernel/v_object.h ./kernel/v_user.h ./libs/verror.h
main.o: ./kernel/v_vals.h ./kernel/v_dbm.h ./kernel/v_file.h visor_w.h
main.o: ../mods/setthread.h graph.h dlg/MainDialog.h
visor_w.o: helpwindow.h service.h ./libs/array.h ./libs/gstring.h
visor_w.o: ./kernel/v_user.h ./libs/verror.h visor_w.h ../mods/setthread.h
visor_w.o: ./kernel/v_vals.h graph.h ./kernel/v_module.h ./kernel/v_object.h
visor_w.o: ./kernel/v_dbm.h ./kernel/v_file.h visor.h module_w.h
visor_w.o: ../mods/m_param.h ../mods/v_mod.h ../mods/submod/ms_rmults.h
visor_w.o: ../mods/submod/ms_mtparm.h ../mods/submod/ms_system.h
visor_w.o: ../mods/m_phase.h ../mods/submod/ms_multi.h ./kernel/v_ipnc.h
visor_w.o: ../mods/submod/ms_calc.h ../mods/m_syseq.h page_win_w.h page_s.h
visor_w.o: ./libs/config.h dlg/MainDialog.h dlg/SelectDialog.h
visor_w.o: dlg/KeyFilter.h dlg/KeyFilter.h dlg/ProgressDialog.h
visor_w.o: dlg/SystemDialog.h dlg/ListFilesDialog.h dlg/LoadMessage.h
visor_w.o: dlg/KeyDialog.h dlg/KeyProfile.h
module_w.o: visor.h ./libs/gstring.h ./libs/array.h ./kernel/v_module.h
module_w.o: ./kernel/v_object.h ./kernel/v_user.h ./libs/verror.h
module_w.o: ./kernel/v_vals.h ./kernel/v_dbm.h ./kernel/v_file.h visor_w.h
module_w.o: ../mods/setthread.h graph.h module_w.h page_win_w.h page_s.h
module_w.o: ./libs/config.h query_w.h
page_w.o: units.h ./libs/array.h ./libs/gstring.h page_i.h page_s.h
page_w.o: ./kernel/v_object.h ./kernel/v_user.h ./libs/verror.h
page_w.o: ./kernel/v_vals.h ./kernel/v_module.h ./kernel/v_dbm.h
page_w.o: ./kernel/v_file.h ./libs/config.h visor.h ../mods/m_param.h
page_w.o: ../mods/v_mod.h ../mods/submod/ms_rmults.h
page_w.o: ../mods/submod/ms_mtparm.h ../mods/submod/ms_system.h
page_w.o: ../mods/m_phase.h ../mods/submod/ms_multi.h ./kernel/v_ipnc.h
page_w.o: ../mods/submod/ms_calc.h ../mods/setthread.h ../mods/m_sdata.h
page_w.o: module_w.h page_w.h page_win_w.h visor_w.h graph.h service.h
page_w.o: query_w.h help.h dlg/CalcDialog.h dlg/CalcCheckDialog.h
page_win_w.o: units.h ./libs/array.h ./libs/gstring.h page_i.h page_s.h
page_win_w.o: ./kernel/v_object.h ./kernel/v_user.h ./libs/verror.h
page_win_w.o: ./kernel/v_vals.h ./kernel/v_module.h ./kernel/v_dbm.h
page_win_w.o: ./kernel/v_file.h ./libs/config.h visor.h ../mods/m_param.h
page_win_w.o: ../mods/v_mod.h ../mods/submod/ms_rmults.h
page_win_w.o: ../mods/submod/ms_mtparm.h ../mods/submod/ms_system.h
page_win_w.o: ../mods/m_phase.h ../mods/submod/ms_multi.h ./kernel/v_ipnc.h
page_win_w.o: ../mods/submod/ms_calc.h ../mods/setthread.h ../mods/m_sdata.h
page_win_w.o: module_w.h page_w.h page_win_w.h visor_w.h graph.h service.h
page_win_w.o: query_w.h help.h dlg/CalcDialog.h dlg/CalcCheckDialog.h
pshape.o: pshape.h ./kernel/v_user.h ./libs/gstring.h ./libs/array.h
pshape.o: ./libs/verror.h
help.o: help.h ./libs/gstring.h ./libs/array.h ./kernel/v_user.h
help.o: ./libs/verror.h
units.o: ./kernel/v_user.h ./libs/gstring.h ./libs/array.h ./libs/verror.h
units.o: units.h ./libs/config.h
helpwindow.o: helpwindow.h visor.h ./libs/gstring.h ./libs/array.h
helpwindow.o: ./kernel/v_module.h ./kernel/v_object.h ./kernel/v_user.h
helpwindow.o: ./libs/verror.h ./kernel/v_vals.h ./kernel/v_dbm.h
helpwindow.o: ./kernel/v_file.h
graph.o: graph.h ./libs/array.h ./kernel/v_module.h ./kernel/v_object.h
graph.o: ./libs/gstring.h ./kernel/v_user.h ./libs/verror.h ./kernel/v_vals.h
graph.o: ./kernel/v_dbm.h ./kernel/v_file.h visor_w.h ../mods/setthread.h
graph.o: dlg/GraphDialog.h pshape.h
