
#        CONFIG    -=qt

        HEADERS	  += $$SUBMOD_H/databr.h \
                     $$SUBMOD_H/datach.h  \
                     $$SUBMOD_H/nodearray.h  \
                     $$SUBMOD_H/node.h  \
                     $$SUBMOD_H/particlearray.h  \
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
                    $$SUBMOD_CPP/ms_multi_file.cpp \
                    $$SUBMOD_CPP/ms_multi_format.cpp \
                    $$SUBMOD_CPP/ipm_chemical.cpp \
                    $$SUBMOD_CPP/ipm_chemical2.cpp \
                    $$SUBMOD_CPP/ipm_chemical3.cpp \
                    $$SUBMOD_CPP/ipm_main.cpp \
                    $$SUBMOD_CPP/ipm_simplex.cpp \
                      $$SUBMOD_CPP/nodearray.cpp  \
                      $$SUBMOD_CPP/node.cpp \
                      $$SUBMOD_CPP/node_format.cpp \
                      $$SUBMOD_CPP/particlearray.cpp
