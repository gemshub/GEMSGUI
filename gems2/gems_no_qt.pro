#qmake -spec win32-borland gems_qt.pro > a

TEMPLATE	= app
LANGUAGE        = C++
TARGET		= gems2
VERSION         = 2.1.1

CONFIG		-= qt
CONFIG		+= thread warn_on debug windows
#CONFIG		+= qmake_cache

DEFINES         += Use_mt_mode
LIBS	        += -L./lib -lgems_qt -lqtmt336 -lqtmain


!win32 {
  DEFINES += __unix
}

win32-borland {
   	DEFINES += __win32_borland
	# Keep the size of the .tds file for the Qt library smaller than
	# 34 Mbytes to avoid linking problems
	# QMAKE_CFLAGS_DEBUG += -vi -y-
	# QMAKE_CXXFLAGS_DEBUG += -vi -y-
        QMAKE_CFLAGS += -x -xd -xp -VM -RT
        QMAKE_CXXFLAGS += -x -xd -xp -VM -RT
}

VIZOR_CPP      =  ./vizor
KERNEL_CPP     =  ./vizor/kernel
LIBS_CPP       =  ./vizor/libs
DLG_CPP        =  ./vizor/dlg
MODS_CPP       =  ./mods
SUBMOD_CPP     =  ./mods/submod
NUMERICS_CPP     =  ./mods/numerics

VIZOR_H      =  $$VIZOR_CPP
KERNEL_H     =  $$KERNEL_CPP
LIBS_H       =  $$LIBS_CPP
DLG_H        =  $$DLG_CPP
MODS_H       =  $$MODS_CPP
SUBMOD_H     =  $$SUBMOD_CPP
NUMERICS_H     =  $$NUMERICS_CPP


DEPENDPATH   += ;$$VIZOR_H;$$KERNEL_H;$$LIBS_H;$$DLG_H;$$MODS_H;$$SUBMOD_H
DEPENDPATH   += ;$$NUMERICS_H
INCLUDEPATH   += ;$$VIZOR_H;$$KERNEL_H;$$LIBS_H;$$DLG_H;$$MODS_H;$$SUBMOD_H
INCLUDEPATH   += ;$$NUMERICS_H

OBJECTS_DIR       = obj

include($$MODS_CPP/mods.pri)
include($$NUMERICS_CPP/numerics.pri)
include($$SUBMOD_CPP/submod.pri)



