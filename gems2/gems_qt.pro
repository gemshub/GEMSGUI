#qmake -spec win32-borland gems_qt.pro > a

TEMPLATE	= app
#LANGUAGE        = C++
TARGET		= gems2
VERSION         = 2.1.0

DEFINES         += Use_mt_mode
CONFIG		+= qt thread warn_on debug 
#CONFIG		+= qmake_cache 



win32-borland {
	# Keep the size of the .tds file for the Qt library smaller than
	# 34 Mbytes to avoid linking problems
	QMAKE_CFLAGS_DEBUG += -vi -y-
	QMAKE_CXXFLAGS_DEBUG += -vi -y-
}

VIZOR_CPP      =  ./vizor
KERNEL_CPP     =  ./vizor/kernel
LIBS_CPP       =  ./vizor/libs
DLG_CPP        =  ./vizor/dlg
MODS_CPP       =  ./mods
SUBMOD_CPP     =  ./mods/submod

VIZOR_H      =  $$VIZOR_CPP
KERNEL_H     =  $$KERNEL_CPP  
LIBS_H       =  $$LIBS_CPP 
DLG_H        =  $$DLG_CPP 
MODS_H       =  $$MODS_CPP 
SUBMOD_H     =  $$SUBMOD_CPP  


DEPENDPATH   += ;$$VIZOR_H;$$KERNEL_H;$$LIBS_H;$$DLG_H;$$MODS_H;$$SUBMOD_H 
INCLUDEPATH   += ;$$VIZOR_H;$$KERNEL_H;$$LIBS_H;$$DLG_H;$$MODS_H;$$SUBMOD_H 

MOC_DIR = tmp
UI_DIR  = $$MOC_DIR
UI_SOURSEDIR  = $$MOC_DIR
UI_HEADERDIR  = $$MOC_DIR
OBJECTS_DIR       = obj


include($$VIZOR_CPP/vizor.pri)
include($$LIBS_CPP/libs.pri)
include($$KERNEL_CPP/kernel.pri)
include($$DLG_CPP/dlg.pri)
include($$MODS_CPP/mods.pri)
include($$SUBMOD_CPP/submod.pri)


