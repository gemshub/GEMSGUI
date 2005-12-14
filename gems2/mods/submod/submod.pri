
#        CONFIG    -=qt

        HEADERS	  += $$SUBMOD_H/databr.h \
                     $$SUBMOD_H/datach.h  \
                     $$SUBMOD_H/nodearray.h  \
                     $$SUBMOD_H/ms_calc.h \
                     $$SUBMOD_H/ms_mtparm.h \
		     $$SUBMOD_H/ms_multi.h \
		     $$SUBMOD_H/ms_rmults.h \
		     $$SUBMOD_H/ms_system.h \
                     $$SUBMOD_H/stepwise.h  \
                     $$SUBMOD_H/v_smodhand.h  

        SOURCES	  +=  $$SUBMOD_CPP/ms_rmults.cpp \
                      $$SUBMOD_CPP/ms_mtparm.cpp \
                      $$SUBMOD_CPP/ms_multi.cpp  \
                      $$SUBMOD_CPP/ms_system.cpp \
                      $$SUBMOD_CPP/ms_sysbc.cpp  \
                      $$SUBMOD_CPP/ms_mupmph.cpp \
                      $$SUBMOD_CPP/ms_muload.cpp \
                      $$SUBMOD_CPP/ms_muleq.cpp   \
                      $$SUBMOD_CPP/ms_mueqexp.cpp \
                      $$SUBMOD_CPP/ipm_sel_bc.cpp \
                      $$SUBMOD_CPP/ipm_gamma.cpp  \
                      $$SUBMOD_CPP/ipm_fia_bc.cpp \
                      $$SUBMOD_CPP/ms_multi_file.cpp  \
                      $$SUBMOD_CPP/nodearray.cpp
