#qmake -spec win32-borland gemsipm2k.pro > a 
#qmake -spec win32-msvc.net gemsipm2k.pro > a

TEMPLATE	= app
LANGUAGE        = C++
TARGET		= gemipm2k
VERSION         = 2.1.1

CONFIG		-= qt 
CONFIG		+=  warn_on debug windows
CONFIG		+= console 

DEFINES         += IPMGEMPLUGIN

!win32-borland {  
DEFINES += __unix 
}

win32-borland {	
#  Debug, RTTI, exceptions, Visual C - compatible
        QMAKE_CFLAGS += -x -xd -xp -VM -RT
        QMAKE_CXXFLAGS += -x -xd -xp -VM -RT
}

LIBS_CPP       =  ../../vizor/libs
KERNEL_CPP     =  ../../vizor/kernel
MODS_CPP       =  ../
SUBMOD_CPP     =  ../submod

#LIBS_CPP       =  .
#KERNEL_CPP     =  .
#MODS_CPP       =  .
#SUBMOD_CPP     =  .

KERNEL_H     =  $$KERNEL_CPP  
LIBS_H       =  $$LIBS_CPP 
MODS_H       =  $$MODS_CPP 
SUBMOD_H     =  $$SUBMOD_CPP  



DEPENDPATH   += ;$$KERNEL_H;$$LIBS_H;$$MODS_H;$$SUBMOD_H
INCLUDEPATH   += ;$$KERNEL_H;$$LIBS_H;$$MODS_H;$$SUBMOD_H

OBJECTS_DIR       = obj

        HEADERS	 += $$LIBS_H/array.h \
                    $$LIBS_H/verror.h  \
                    $$LIBS_H/gstring.h  \
                    $$LIBS_H/gdatastream.h  \
                    $$KERNEL_H/v_user.h \
                    $$MODS_H/s_fgl.h \
                    $$SUBMOD_H/databr.h \
                    $$SUBMOD_H/datach.h \
                    $$SUBMOD_H/nodearray.h \
                    $$SUBMOD_H/particlearray.h \
                    $$SUBMOD_H/num_methods.h \
                    $$SUBMOD_H/ms_multi.h \
		    ms_gem2mt.h \
		    m_const.h \
		    m_param.h 
		   
        SOURCES	 += $$LIBS_CPP/gstring.cpp  \
                    $$LIBS_CPP/gdatastream.cpp  \
                    $$MODS_CPP/s_fgl.cpp \
                    $$SUBMOD_CPP/nodearray.cpp \
                    $$SUBMOD_CPP/nodearray2.cpp \
                    $$SUBMOD_CPP/particlearray.cpp \
                    $$SUBMOD_CPP/num_methods.cpp \
                    $$MODS_CPP/m_gem2mtt.cpp \
                    $$SUBMOD_CPP/ipm_fia_bc.cpp \
                    $$SUBMOD_CPP/ipm_gamma.cpp \
                    $$SUBMOD_CPP/ipm_sel_bc.cpp \
                    $$SUBMOD_CPP/ms_mueqexp.cpp \
                    $$SUBMOD_CPP/ms_multi_file.cpp \
		    main.cpp \
		    ms_param.cpp 
