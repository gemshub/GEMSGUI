Porting .ui files to Qt4

1. uic3 SettingsDialogData.ui > SettingsDialogData.h
2. uic3 -impl SettingsDialogData.h SettingsDialogData.ui > SettingsDialogData.cpp

do it for compare (use end of SettingsDialogData.h and
SettingsDialogData.cpp in SettingsDialog.cpp )

3. uic3 -convert SettingsDialogData.ui > SettingsDialog4.ui

Converts a Qt Designer 3 .ui file to the QtDesigner 4 format.

4. Open SettingsDialog4.ui file and change
  - Q3ButtonGroup to ( QGroupBox + QButtonGroup in *.cpp if necessary )
  - Q3GroupBox   to QGroupBox
  - add ful Layout for all window
  - add icons for img.qrc file
  - "Form Settings" uncheck "Pixmap Function"
  - Q3ListView to QTreeWidget or QListWidget
  - Q3ListViewItem to QTreeWidgetItem
  - Q3ListBox to QListWidget
  - Q3Frame to QFrame
  - Q3ProgressBar to QProgressBar
  - setCaption to setWindowTitle   
  - .toLatin1().data()

5. uic SettingsDialog4.ui > SettingsDialog4.h

Builds source file for Qt4 version


6.qt3to4 SettingsDialog.h, SettingsDialog4.cpp

7. Edit resulting files (see 1. 2. )

8. For QWizard:
	do as ProcessWizard



Done!!!!!!

Smoll dialogs

1. MainDialog4.ui (no layout )
2. SettingsDialog4.ui
3. DBDialog4.ui
4. EQDialog4.ui
5. DBAdvDialog4.ui (no layout )
6. ListFilesDialog4.ui ( need test for selection QTreeWidget )
7. SelectDialog4.ui ( need test for selection QListWidget )
8. ProgressDialog4.ui ( need test  )
9. CalcDialog4.ui     ( test ? )
10.CalcCheckDialog4.ui ( test ? )
11.KeyDialogData4.ui ( need test for selection QListWidget )
12.ChangeKeyDialogData4.ui 
13.KeyProfile4.ui ( need test for selection QListWidget )
14. AutoPhaseDialog4.ui
15. ColorDialog4.ui
16. LegendDialog4.ui ( must be tested ) 
   (CmChangeGraphType( int new_type ) - test clearing old componetts ) !!!!
17. GraphDialog4.ui  ( must be tested )
18. ElementsDialog4.ui
19. SetFiltersDialog4.ui
20. NewSystemDialog4.ui
    done 
        NewSystemDialog.h
        NewSystemMenu.cpp
    must be done !!!  
        NewSystemDialog.cpp for editin mode (as in other forms)
	         QTreeWidget and (QTreeWidgetItem + MyCellEdit(nO, nMi, nNi ) )
21. Added ModuleDialog4.ui for modules menu

	
Wizards

1. ProcessWizard4.ui
2. GEM2MTWizard4.ui
3. UnSpaceWizard4.ui
4. ComposWizard4.ui
5. DualThWizard4.ui
6. DCompWizard4.ui
7. ReacDCWizard4.ui
8. RTparmWizard4.ui
9. GtDemoWizard4.ui
10. PhaseWizard4.ui

Dialogs without *.ui

1. KeyFilter.cpp/.h   
2. ExcludeFillDialog.cpp/.h
3. LoadMessage.cpp/.h


Vizor
  1. module_w.cpp/.h
  2. visor.cpp       (must be tested files to load system )
  3. service.cpp     ( part of wisor_w.cpp )
  4. visor_w.cpp
      - OpenHelp - must be changed for new help structure 
      - SetDialog - commented part must be changed
      - bool ThreadControl::wait() and :wait(unsigned long time) - must be changed   
  5. pshape.cpp/.h    (test drag/drop events) 
  6. page_f.cpp/.h    ( changes in structures )
  7. model_w.cpp/.h   ( description of QTableView and editors for all objects in window  ) 
  8. page_w.cpp/.h          
      
      
Comments

1. All Dialogs must be tested yet in full
2. Test multithread mode in LoadMessage.cpp, visor_w.cpp (part is commented)
3. NewSystemDialog.cpp rewrite use new objEditor 
   rewrite QItemDelegate and QTreeWidgetItem(nO, ii, unitslist string) and insert it to tree 
4. OpenHelp - must be changed for new help structure
5.Did  Not implemented selections for elements of Module windows     only test
6.Did  Not implemented context menu for elements of Module windows  Did
7. Service.cpp !!!
     commented all for threads

     
//================================================================================     
april 2008     
     
1. Did

Tested and fixed errors for TObjectTable class and Module windows 
Implemented selections for elements of Module windows    
Implemented context menu for elements of Module windows  

Module Window  (left )
- No scroll TextEdit fields for TObjectTable

2. Did

Tested and fixed errors for Graph windows.


jan 2009

Did 

NewSystemDialog.cpp changed for editing mode (as in other forms),
  Writed TTreeView, TTreeModel and TTreeDelegate
  
- must be testet  
- must be implemented context menu for elements of NewSystemDialog windows
  and selection
- must be writed PrintList1 
- must be writed slot objectChanged for System (set flag of changing record)


To harmonize files with the gems2 files for catalogs

1. data - all files are equal in gems2 and gems3
          (in viz_cn.ini can be used check files for lines
           as for column  (see XAun_ == mAUcln) )

2. numerics - all files are equal in gems2 and gems3

3. submod - all files are equal in gems2 and gems3
            ( stepwise.h can be changed in future)

4. mods - all files are equal in gems2 and gems3
            (m_param.cpp can be changed version in future )

5. kernel - all files are equal in gems2 and gems3

6. libs - all files are equal in gems2 and gems3


7. dlg4 - all files are new in  gems3
          changes must be did in the same time

8. vizor4 - all files are new in  gems3
          changes must be did in the same time
  (exeption graph.cpp(.h), units.cpp(.h), servise.h, vizor.h )


27/01/2009 All Dialogs in dlg and dlg4 are equal
           we need add only later changes 
      
feb 2009

NewSystemDialog add "What this" command or not? Ask DK
		
23/03/2009

NewSystemDialog.cpp changed for editing mode (as in other forms),
  Writed TTreeView, TTreeModel and TTreeDelegate
Use this classes to window "Results: Equilibrium State"
Added  PrintList function

25/03/2009

must be implemented context menu for elements of NewSystemDialog windows
  and selection
- must be writed PrintList1 
- must be writed slot objectChanged for System (set flag of changing record)
