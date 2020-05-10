
#         CONFIG    -=qt

        HEADERS	  +=  $$SUBMODS_H/ms_calc.h \
#                    $$SUBMODS_H/databr.h \
#                    $$SUBMODS_H/datach.h  \
#                    $$SUBMODS_H/nodearray.h  \
#                     $$SUBMODS_H/node.h  \
    $$PWD/ms_multi_new.h \
    $$PWD/node_gui.h \
    $$PWD/nodearray_gui.h \
                      $$SUBMODS_H/particlearray.h  \
                      $$SUBMODS_H/ms_mtparm.h \
#		     $$SUBMODS_H/ms_multi.h \
		      $$SUBMODS_H/ms_rmults.h \
		      $$SUBMODS_H/ms_system.h \
                      $$SUBMODS_H/stepwise.h  \
#		     $$SUBMODS_H/io_arrays.h 

        SOURCES	  +=  $$SUBMODS_CPP/ms_rmults.cpp \
    $$PWD/ms_multi_new.cpp \
    $$PWD/node_gui.cpp \
    $$PWD/nodearray_gui.cpp \
                      $$SUBMODS_CPP/ms_mtparm.cpp \
                      $$SUBMODS_CPP/ms_multi.cpp  \
                      $$SUBMODS_CPP/ms_system.cpp \
                      $$SUBMODS_CPP/ms_sysbc.cpp  \
                      $$SUBMODS_CPP/ms_mupmph.cpp \
                      $$SUBMODS_CPP/ms_muload.cpp \
                      $$SUBMODS_CPP/ms_muleq.cpp   \
 #                   $$SUBMODS_CPP/ms_multi_file.cpp \
 #                   $$SUBMODS_CPP/ms_multi_format.cpp \
 #                   $$SUBMODS_CPP/ipm_chemical.cpp \
 #                   $$SUBMODS_CPP/ipm_chemical2.cpp \
 #                   $$SUBMODS_CPP/ipm_chemical3.cpp \
 #                   $$SUBMODS_CPP/ipm_main.cpp \
 #                   $$SUBMODS_CPP/ipm_simplex.cpp \
 #		     $$SUBMODS_CPP/io_arrays.cpp \
 #                   $$SUBMODS_CPP/nodearray.cpp  \
 #                   $$SUBMODS_CPP/node.cpp \
 #                   $$SUBMODS_CPP/node_format.cpp \
                     $$SUBMODS_CPP/particlearray.cpp
