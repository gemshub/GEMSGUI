# BCB5 (bcc32 5.5.1 ) version
# Makefile	Makefile for GEM
#
# Version:	@(#)Makefile  28-Dec-1998
#

!include Makefile.qt

FLAGS	= $(CCFLAGS) $(DEBUG_FLAGS)
INC_L	= -I./core -I./libs -I../mods
INC	= $(INC_L) $(INCLUDES)
LIB	=   tlib /C /P512


OBJECTS = page_f.obj \
          visor.obj \
          main.obj \
          visor_w.obj \
          module_w.obj \
          page_w.obj \
          page_win_w.obj \
          pshape.obj \
          help.obj \
          units.obj \
          helpwindow.obj \
          graph.obj

SRC 	= page_f.cpp \
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

METASRC = moc_visor_w.cpp \
          moc_module_w.cpp  \
          moc_page_w.cpp \
          moc_query_w.cpp \
          moc_page_win_w.cpp  \
          moc_helpwindow.cpp \
          moc_graph.cpp

METAOBJ = tmp\moc_visor_w.obj \
          tmp\moc_module_w.obj  \
          tmp\moc_page_w.obj \
          tmp\moc_query_w.obj \
          tmp\moc_page_win_w.obj  \
          tmp\moc_helpwindow.obj



TARGET	= ..\lib\gemviz.lib

MAKE	= make

# rules

.cpp.obj:
	$(CC) $(FLAGS) $(INC) -c $<


all:  $(TARGET)


$(TARGET):   $(OBJECTS) $(METAOBJ)
        cd libs
	$(MAKE) all
	cd ..\core
	$(MAKE) all
	cd ..\dlg
	$(MAKE) all
	cd ..
        -del $(TARGET)
	$(LIB) $(TARGET) @&&|
+libs\config.obj \
+libs\gstring.obj\
+core\v_user.obj \
+core\v_vals.obj \
+core\v_object.obj \
+core\v_ipnc.obj \
+core\v_file.obj \
+core\v_dbm1.obj \
+core\v_dbm2.obj \
+core\t_print.obj \
+core\v_module.obj\
+page_f.obj\
+visor.obj\
+main.obj \
+visor_w.obj \
+module_w.obj \
+page_w.obj\
+pshape.obj \
+help.obj \
+units.obj \
+helpwindow.obj \
+tmp\moc_visor_w.obj \
+tmp\moc_module_w.obj +tmp\moc_page_w.obj +page_win_w.obj  +graph.obj \
+tmp\moc_query_w.obj +tmp\moc_page_win_w.obj \
+tmp\moc_helpwindow.obj \
+dlg\MainDialog.obj \
+dlg\MainDialogData.obj \
+dlg\DBDialog.obj \
+dlg\DBDialogData.obj \
+dlg\EQDialog.obj \
+dlg\EQDialogData.obj \
+dlg\SystemDialog.obj \
+dlg\SystemDialogData.obj \
+dlg\ProgressDialog.obj \
+dlg\ProgressDialogData.obj \
+dlg\KeyDialog.obj \
+dlg\KeyDialogData.obj \
+dlg\KeyProfile.obj \
+dlg\KeyProfileData.obj \
+dlg\KeyFilter.obj \
+dlg\SettingsDialog.obj \
+dlg\SettingsDialogData.obj \
+dlg\SelectDialog.obj \
+dlg\SelectDialogData.obj \
+dlg\CalcDialog.obj \
+dlg\CalcDialogData.obj \
+dlg\CalcCheckDialog.obj \
+dlg\CalcCheckDialogData.obj \
+dlg\DBAdvDialog.obj \
+dlg\DBAdvDialogData.obj \
+dlg\ColorDialog.obj  \
+dlg\ColorDialogData.obj \
+dlg\GraphDialog.obj  \
+dlg\GraphDialogData.obj \
+dlg\LegendDialog.obj  \
+dlg\LegendDialogData.obj \
+dlg\ListFilesDialog.obj \
+dlg\ListFilesDialogData.obj \
+dlg\ElementsDialog.obj \
+dlg\ElementsDialogData.obj \
+dlg\SetFiltersDialog.obj \
+dlg\SetFiltersDialogData.obj \
+dlg\HLresDialog.obj \
+dlg\HLresDialogData.obj \
+dlg\HLinpDialog.obj \
+dlg\HLinpDialogData.obj \
+dlg\LoadMessage.obj \
+tmp\moc_MainDialog.obj \
+tmp\moc_MainDialogData.obj \
+tmp\moc_DBDialog.obj \
+tmp\moc_DBDialogData.obj \
+tmp\moc_EQDialog.obj \
+tmp\moc_EQDialogData.obj \
+tmp\moc_SystemDialog.obj \
+tmp\moc_SystemDialogData.obj \
+tmp\moc_ProgressDialog.obj \
+tmp\moc_ProgressDialogData.obj \
+tmp\moc_KeyDialog.obj \
+tmp\moc_KeyDialogData.obj \
+tmp\moc_KeyProfile.obj \
+tmp\moc_KeyProfileData.obj \
+tmp\moc_KeyFilter.obj \
+tmp\moc_SettingsDialog.obj \
+tmp\moc_SettingsDialogData.obj \
+tmp\moc_SelectDialog.obj \
+tmp\moc_SelectDialogData.obj \
+tmp\moc_CalcDialog.obj \
+tmp\moc_CalcDialogData.obj \
+tmp\moc_CalcCheckDialog.obj \
+tmp\moc_CalcCheckDialogData.obj \
+tmp\moc_DBAdvDialog.obj \
+tmp\moc_DBAdvDialogData.obj \
+tmp\moc_ColorDialog.obj \
+tmp\moc_ColorDialogData.obj \
+tmp\moc_GraphDialog.obj \
+tmp\moc_GraphDialogData.obj \
+tmp\moc_LegendDialog.obj \
+tmp\moc_LegendDialogData.obj \
+tmp\moc_ListFilesDialog.obj \
+tmp\moc_ListFilesDialogData.obj \
+tmp\moc_ElementsDialog.obj \
+tmp\moc_ElementsDialogData.obj \
+tmp\moc_SetFiltersDialog.obj \
+tmp\moc_SetFiltersDialogData.obj \
+tmp\moc_HLresDialog.obj \
+tmp\moc_HLresDialogData.obj \
+tmp\moc_HLinpDialog.obj \
+tmp\moc_HLinpDialogData.obj \
+tmp\moc_LoadMessage.obj
|

moc:    $(SRCMOC)

clean:
     -del  libs\config.obj
     -del  libs\gstring.obj
     -del  core\v_user.obj
     -del  core\v_vals.obj
     -del  core\v_object.obj
     -del  core\v_ipnc.obj
     -del  core\v_file.obj
     -del  core\v_dbm1.obj
     -del  core\v_dbm2.obj
     -del  core\t_print.obj
     -del  core\v_module.obj
     -del  page_f.obj
     -del  visor.obj
     -del  main.obj
     -del  visor_w.obj
     -del  module_w.obj
     -del  page_w.obj
     -del  pshape.obj
     -del  help.obj
     -del  units.obj
     -del  helpwindow.obj
     -del  tmp\moc_visor_w.obj
     -del  tmp\moc_module_w.obj
     -del  tmp\moc_page_w.obj
     -del  page_win_w.obj
     -del  graph.obj
     -del  tmp\moc_query_w.obj
     -del  tmp\moc_page_win_w.obj
     -del  tmp\moc_helpwindow.obj
     -del  dlg\MainDialog.obj
     -del  dlg\MainDialogData.obj
     -del  dlg\DBDialog.obj
     -del  dlg\DBDialogData.obj
     -del  dlg\EQDialog.obj
     -del  dlg\EQDialogData.obj
     -del  dlg\SystemDialog.obj
     -del  dlg\SystemDialogData.obj
     -del  dlg\ProgressDialog.obj
     -del  dlg\ProgressDialogData.obj
     -del  dlg\KeyDialog.obj
     -del  dlg\KeyDialogData.obj
     -del  dlg\KeyProfile.obj
     -del  dlg\KeyProfileData.obj
     -del  dlg\KeyFilter.obj
     -del  dlg\SettingsDialog.obj
     -del  dlg\SettingsDialogData.obj
     -del  dlg\SelectDialog.obj
     -del  dlg\SelectDialogData.obj
     -del  dlg\CalcDialog.obj
     -del  dlg\CalcDialogData.obj
     -del  dlg\CalcCheckDialog.obj
     -del  dlg\CalcCheckDialogData.obj
     -del  dlg\DBAdvDialog.obj
     -del  dlg\DBAdvDialogData.obj
     -del  dlg\ColorDialog.obj
     -del  dlg\ColorDialogData.obj
     -del  dlg\GraphDialog.obj
     -del  dlg\GraphDialogData.obj
     -del  dlg\LegendDialog.obj
     -del  dlg\LegendDialogData.obj
     -del  dlg\ListFilesDialog.obj
     -del  dlg\ListFilesDialogData.obj
     -del  dlg\ElementsDialog.obj
     -del  dlg\ElementsDialogData.obj
     -del  dlg\SetFiltersDialog.obj
     -del  dlg\SetFiltersDialogData.obj
     -del  dlg\HLresDialog.obj
     -del  dlg\HLresDialogData.obj
     -del  dlg\HLinpDialog.obj
     -del  dlg\HLinpDialogData.obj
     -del  dlg\LoadMessage.obj
     -del  tmp\moc_MainDialog.obj
     -del  tmp\moc_MainDialogData.obj
     -del  tmp\moc_DBDialog.obj
     -del  tmp\moc_DBDialogData.obj
     -del  tmp\moc_EQDialog.obj
     -del  tmp\moc_EQDialogData.obj
     -del  tmp\moc_SystemDialog.obj
     -del  tmp\moc_SystemDialogData.obj
     -del  tmp\moc_ProgressDialog.obj
     -del  tmp\moc_ProgressDialogData.obj
     -del  tmp\moc_KeyDialog.obj
     -del  tmp\moc_KeyDialogData.obj
     -del  tmp\moc_KeyProfile.obj
     -del  tmp\moc_KeyProfileData.obj
     -del  tmp\moc_KeyFilter.obj
     -del  tmp\moc_SettingsDialog.obj
     -del  tmp\moc_SettingsDialogData.obj
     -del  tmp\moc_SelectDialog.obj
     -del  tmp\moc_SelectDialogData.obj
     -del  tmp\moc_CalcDialog.obj
     -del  tmp\moc_CalcDialogData.obj
     -del  tmp\moc_CalcCheckDialog.obj
     -del  tmp\moc_CalcCheckDialogData.obj
     -del  tmp\moc_DBAdvDialog.obj
     -del  tmp\moc_DBAdvDialogData.obj
     -del  tmp\moc_ColorDialog.obj
     -del  tmp\moc_ColorDialogData.obj
     -del  tmp\moc_GraphDialog.obj
     -del  tmp\moc_GraphDialogData.obj
     -del  tmp\moc_LegendDialog.obj
     -del  tmp\moc_LegendDialogData.obj
     -del  tmp\moc_ListFilesDialog.obj
     -del  tmp\moc_ListFilesDialogData.obj
     -del  tmp\moc_ElementsDialog.obj
     -del  tmp\moc_ElementsDialogData.obj
     -del  tmp\moc_SetFiltersDialog.obj
     -del  tmp\moc_SetFiltersDialogData.obj
     -del  tmp\moc_HLresDialog.obj
     -del  tmp\moc_HLresDialogData.obj
     -del  tmp\moc_HLinpDialog.obj
     -del  tmp\moc_HLinpDialogData.obj
     -del  tmp\moc_LoadMessage.obj

tmp\moc_visor_w.obj: visor_w.h
      	$(MOC) visor_w.h -o moc_visor_w.cpp

        $(CC) $(CCFLAGS) $(INC) -o$@ -c moc_visor_w.cpp
	-del moc_visor_w.cpp


tmp\moc_module_w.obj: module_w.h
      	$(MOC) module_w.h -o moc_module_w.cpp

        $(CC) $(CCFLAGS) $(INC) -o$@ -c moc_module_w.cpp
	-del moc_module_w.cpp

tmp\moc_page_w.obj: page_w.h
      	$(MOC) page_w.h -o moc_page_w.cpp

        $(CC) $(CCFLAGS) $(INC) -o$@ -c moc_page_w.cpp
	-del moc_page_w.cpp

tmp\moc_query_w.obj: query_w.h
      	$(MOC) query_w.h -o moc_query_w.cpp

        $(CC) $(CCFLAGS) $(INC) -o$@ -c moc_query_w.cpp
	-del moc_query_w.cpp

tmp\moc_page_win_w.obj: page_win_w.h
      	$(MOC) page_win_w.h -o moc_page_win_w.cpp

        $(CC) $(CCFLAGS) $(INC) -o$@ -c moc_page_win_w.cpp
	-del moc_page_win_w.cpp

tmp\moc_helpwindow.obj: helpwindow.h
      	$(MOC) helpwindow.h -o moc_helpwindow.cpp

        $(CC) $(CCFLAGS) $(INC) -o$@ -c moc_helpwindow.cpp
	-del moc_helpwindow.cpp


