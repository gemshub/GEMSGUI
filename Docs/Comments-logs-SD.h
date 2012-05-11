
To do 08/12/2011

 1. Added records SDref to project
( build list of all sdref keys from other records (DComp. ReacDC ....) and copy
  this records to new project SDref file)

 2. rev 580 - standalone
    Copy internal structure of format write/read nodes structure from upgrade to trunk

 3 Bug: Lost changes in text field (editor), if after editing go to menu
     ( change equations and try calc process )

 4. GEM2MT after stop calculation point and reload point from files
    prodused values 10-20% different

  5. Add recording last open mode (D or E )
    and last open records to configurator files to be able to restore them upon next start of the program


To do

1. When build new system setup default values to Pmin, Pmax, Pinc, Tmin, Tmax, Tinc
   (May be change some key fields in other type of minimization )

2. Loading data from Equstat,> undefined F0, G => illegal values in some arrays
   Not all result arrays restored

3. Change install in Linux: not root, not rpm, rename ./gems3 to /libriaryGems3

4. Test help in MAC, use help from network

5. Learm broblem of minimization of convex one- and two- parameters functions
   (dvumernaya parametrizaciya vypuklyh funkciy) GoldSelection

6. In some profiles we have not the gem2mt files and so don not reload it.
   If load "Kaolinite" after "CalDolCol" all gem2mt records can be loaded.

7. there is a problem with correct recalculation of records in cases where phases are
   switched off. When a phase has been switched off and the SysEq? record was
   recalculated directly (even when the BCC has been recalculated before IPM minimization),
   the result is not correct. Only when the record is reloaded after a phase has been switched off
   and then recalculated, the correct result is obtained. This likely is related to the execution
   of GammaCalc?() in LINK_TP_MODE that is usually done when a record is reloaded

8. Change main Gems3 icon (may be color) (font "Nimbus Mono L")

9. In TUnSpase  tpp->G may not be at this T,P if use lookupArrays
   TUnspase must be rewriting

10 Not optimal model_w and model_tree for ubdating windows
   Errors in thread Process calculate mode ()
   May bee stack or updates in other thread;

   //==================================================================







http://icones.pro/en/?s=chemistry

//===================================================================

15/11/2011
1. Using SystemWizard only in "Remake" mode.
   For "New" mode we use default data ( changed: PbPH='+', PPHk='*', PhmKey = "---" )
   PPHk can not be changed (always *)
   Moved question "Would you like to activate (turn on ) all IComp, DComp/ReacDC and Phase entries?" to wizard

2.In mode Computation of Equilibria For Data base records (IComp,DComp,ReacDC, Phase) now we can
  "Save", "Display", "Calculate" records
  If record changed ( Saved ) program reload all thermodynamic data before calculation of equilibria

3. Built one list for selection DComp and ReacDC records ( remake Phase, ReacDC, Compos )

29/06/2011
Added new servise functions ( vfChooseDirectory, vfMakeDirectory)
Added new gstring function ( replaceall())
Changed internal VTK files names
Rewrited void TGEM2MT::InsertChanges function
Changes moiety element name format


25/01/2011
-Bugfix: Wrong error message when saving a cloned IComp record (BugIssue0042)
-Bugfix: Crash when pressing View Scripts on SubMod windows (BugIssue0039)
-Avoid prompt to select source record when renaming (SaveAs) RTparm, TDualTh, TGEM2MT,
 TProcess,TUnSpace  records(BugIssue0040)


17/11/2010
 Added  mLook flag to DATACH and mode of no interpolation, work on data for T,P pairs (for GEMSFIT)

15/11/2010
Added arrays of discrete values of T and P to LookupDialog


14/12/2010
Insert changes to trunk version: 1709,1714, 1727, 1729. 1730, 1731, 1732, 1734

14/12/2010
Inser allocators for SitFr
added question before "Switch on all components when "System/Remake" command"

13/12/2010
Was rewritten class Formuan - gems chemical formulas parser used only string functions

9/12/2010
Built new HelpWindow and HelpGenerator

29/11/2010
Added for DComp&ReacDCWizards "Codes for species-dependent EoS subroutines"

29/11/2010
Switch on all components when "System/Remake" command

29/11/2010
Bugfix: rtparm one line calculation
Set up zero indexes for all dynamic objects.

26/11/2010
Added objects to arrays DCMod and dcMod, setup TP_code filed;
For TCORKcalc model may be changed "EosCode", see constructors


26/11/2010
added for Wizards E,C,V,R,A,B "Dc code"

26/11/2010
BugFix: Indexation into DMc  array

26/11/2010
Added to DCompWizard and ReacDCWizart codes
 DC_SOL_MINDEP='F' and DC_SOL_MAJDEP='D'
Added to PhaseWizard SB_BERMAN='B'

26/11/2010  !! to trunk
   BugFix: Wrong number of  neutral species except H2O in Phase

25/11/2010
  Changed list of DC class code in user.ini file

TESWGVCHNIJM<digit>     XYZABPQRO       // coments to DCC arrays
TESW   GVCHNI JM   PQR <digit>AB  XYZ O // coments to DCC arrays

TESKWL GVCHNI JMFD QPR <0-9>  AB  XYZ O // list from v_mod.h

TESKWL GVCHNI JM QPR 0123456789 AB XYZO  //lists from units.ini
TESKWL GVCHNI JM 0123456789 XYZ AB PQRO
TESWL  GVCHNI JM 0123456789 XYZ AB PQRO
TESKW  GVCHNI JM 0123456789 XYZ AB PQRO

//New list of DC class code will be copied to Phase definition (can be changed there too):
TESKWLGVCHNIJMFDQPR0123456789ABXYZO  35

DCompWizard
OTSKWLGVCIMJXYZ0246813579QP          27
No in DCompWizard "EHNFDRAB"         8

ReacDCWizard
OTSKWLGIJMXYZ0246813579QP            25
No in ReacDCWizard "EVCHNFDRAB"      10


24/11/2010
Send impotant changes to trunk version, exept 09/11/2010

23/11/2010
Unable to create second instance of gems3

22/11/2010
Install QtSingleApplication (obsolute)
- Unpacking the arhive qtsingleapplication-2.6_1-opensource.tar.gz

- Building the component: Open with QtCreator ../qtsingleapplication-2.6_1-opensource/buildlib/buildlib.pro
      and exec "build all" command

- Copy library from "..//qtsingleapplication-2.6_1-opensource/lib" to "$(QTDIR)/lib"  directory
 the "../qtsingleapplication-2.6_1-opensource/lib" file(s) are copied into the "lib" directory of
 your Qt installation. The Qt installation will already have set up that directory to be searched
 by the dynamic linker.

- Create new folder $(QTDIR)/include/QtSingleApplication

- Copy headers from  "..//qtsingleapplication-2.6_1-opensource/src" to "$(QTDIR)/include/QtSingleApplication"


19/11/2010
Added to visor_cn.ini format  'l' - under the first ( begin + tied with previous cell for scrolling)

18/11/2010
Rewritten  "End member moiety- site multiplicity number tables” structure

17/11/2010
Added for Multi class arrays for Collect occupancy numbers for the multi-site mixing models

15/11/2010
BugFix: Retain setup of aqueous and gas phases if use template project

12/11/2010
Added Multi-site (sublattice) SS models to phase calculation

11/11/2010
Was rewritten class Formuan - gems chemical formulas parser
and extended "Chemical formula syntax in GEM-Selektor" to site terms.

09/11/2010
BugFix: Sd_ref cannot be printed (BugIssue0034)
We use only default script for sdref records

09/11/2010
In case two records with the same record key are inserted, the program offers the option to change the name of
the second of them to avoid deletion and loss of data ( "SaveAs", "New", "Create" commands for DataBase).
For "Rename" and "Move" commands, we added a function AddRecordTest() - for testing unique key name before
adding the record(s).
If two database files are loaded which contain two or more entries having the same record key we close second file
and stop open function. In this case we cannot rename records without loss of consistency.

08/11/2010
Changed KeyFilter dialog

19/10/2010
Added function AddRecordTest - for testing unique keys name before add the record(s)
(important for transfer/rename commands, but likelihood of this mistake very little)

12/10/2010
Writing/reading arrays to files: using CHAR_EMPTY as representations
EMPTY values in files (SD)

01/10/2010
Deleted gstring.cpp, gstring.h, array.h, min(), max() from GEMIPM2k

27/05/2010
Wrote classes GoldenSelection and GoldenSelectionTwo
for minimization of convex one- and two- parameters functions.
Method of Gold Selection

Added start patterns of  Helmholtz Energy and Internal Energy minimization
into Gems3

25/05/2010
Used in GEMS3 kernel interpolation for T and P for the lookup arrays of thermodynamic data
and  use CompG0Load from ms_param.cpp
Added to DATACH structure vector Psat - Pressure Pa at saturated H2O vapour at given temperature [nTp]


20/05/09
 Added static objects list to InputSystemDialog
 Implemmnt P_REACTORS (seq.reactor) into Process Wizard


19/05/2010
QD calls is better isolated
Removed TNode::na static pointer


//==================================================================================================

// old version before 19/05/2010

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
   done 
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
      - SetDialog - commented part must be changed  (done 14-08-2009)
      - bool ThreadControl::wait() and :wait(unsigned long time) - must be changed  (done 14-08-2009) 
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
5.Did  Not implemented selections for elements of Module windows    done
6.Did  Not implemented context menu for elements of Module windows  Done
7. Service.cpp !!!
     commented all for threads  (done 14-08-2009)

     
//================================================================================     
\section2 Editing Key Bindings

    The list of key bindings which are implemented for editing:
    \table
    \header \i Keypresses \i Action
    \row \i Backspace \i Deletes the character to the left of the cursor.
    \row \i Delete \i Deletes the character to the right of the cursor.
    \row \i Ctrl+C \i Copy the selected text to the clipboard.
    \row \i Ctrl+Insert \i Copy the selected text to the clipboard.
    \row \i Ctrl+K \i Deletes to the end of the line.
    \row \i Ctrl+V \i Pastes the clipboard text into text edit.
    \row \i Shift+Insert \i Pastes the clipboard text into text edit.
    \row \i Ctrl+X \i Deletes the selected text and copies it to the clipboard.
    \row \i Shift+Delete \i Deletes the selected text and copies it to the clipboard.
    \row \i Ctrl+Z \i Undoes the last operation.
    \row \i Ctrl+Y \i Redoes the last operation.
    \row \i Left \i Moves the cursor one character to the left.
    \row \i Ctrl+Left \i Moves the cursor one word to the left.
    \row \i Right \i Moves the cursor one character to the right.
    \row \i Ctrl+Right \i Moves the cursor one word to the right.
    \row \i Up \i Moves the cursor one line up.
    \row \i Down \i Moves the cursor one line down.
    \row \i PageUp \i Moves the cursor one page up.
    \row \i PageDown \i Moves the cursor one page down.
    \row \i Home \i Moves the cursor to the beginning of the line.
    \row \i Ctrl+Home \i Moves the cursor to the beginning of the text.
    \row \i End \i Moves the cursor to the end of the line.
    \row \i Ctrl+End \i Moves the cursor to the end of the text.
    \row \i Alt+Wheel \i Scrolls the page horizontally (the Wheel is the mouse wheel).
    \endtable


//===========================================================================
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

11/08/2009 

Harmonized changed from gems2 since 10/02/2009
(include all dialogs and wizards)

14/08/2009
Finished and tested thread support in Process and System step calculations
Clearing all files from vizor4




17/08/2009
Harmonized files with the gems2 files for catalogs

1. data - all files are equal in gems2 and gems3
          (in viz_cn.ini can be used check files for lines
           as for column  (see XAun_ == mAUcln) )
2. numerics - all files are equal in gems2 and gems3
3. submod - all files are equal in gems2 and gems3
4. mods - all files are equal in gems2 and gems3
            (m_param.cpp can be changed version in future )
5. kernel - all files are equal in gems2 and gems3
6. libs - all files are equal in gems2 and gems3

7. dlg4 - all files are new in  gems3
          changes must be did in the same time
8. vizor4 - all files are new in  gems3
          changes must be did in the same time
  (exeption graph.cpp(.h), units.cpp(.h) )

Added command "Copy with names" (Added to selections labels of Table columns)
      
      
18/08/2009
Working of Help system
All number of html files are 56  
// using in gems "+/-", need changing "*" 
// 11
(+) <section title="General documentation files" ref="g_about.html">
(+)    <section title="More About, history" ref="gm_about.html"> </section>
(-)    <section title="Terms of Use and Disclaimer" ref="terms-of-use.html"></section>
(-)    <section title="Users Reference" ref="g_useref.html"></section>
(+)    <section title="Database record keys" ref="gdb_rkey.html"></section>
(-)    <section title="Math Scripts" ref="gmscript.html">
(-)       <section title="DOD lists (for Math Scripts)" ref="mscr_dod.html"></section>
       </section>
(-)    <section title="Print, Export and Import Scripts" ref="pe_script.html"></section>
(-*)   <section title="Glossary of notions and symbols" ref="glossary.html"></section>
(-)    <section title="List of bibliographic references" ref="g_litref.html"></section>
(+)   <section title="Installation, Configuration and Setup" ref="gemsetup.html"></section>
    </section>
// 4(5) (? gm_syseq as main window from list)
(+) <section title="Single Thermodynamic System" ref="gm_syseq.html">
(+)    <section title="CSD Setup Submodule" ref="gems_bcc.html">
(-*)      <section title="Extensions for GEM sorption modelling" ref="gems_csm.html"></section>
       </section>
(+)    <section title="GEM IPM-2 Controls" ref="gems_ipm.html"> </section>
(+)    <section title="Results of GEM IPM" ref="gems_sys.html"> </section>
    </section>
// 15
(+) <section title="Module manuals (database record types)" ref="gt_howto.html">
(+)    <section title="Scripts- and Data References (SDref)" ref="gm_sdref.html"> </section>
(+)    <section title="Independent  Components  (IComp)" ref="gm_icomp.html"></section>
(2+)   <section title="Dependent Components (DComp)" ref="gm_dcomp.html"></section>
(2+)   <section title="Reaction-Defined Format of Thermodynamic Data (ReacDC)" ref="gm_reacdc.html"></section>
(2+)   <section title="Predefined Composition Objects (PCO) (Compos)" ref="gm_compos.html"></section>
(2+)   <section title="Phase Definitions (Phase)" ref="gm_phase.html"></section>
(2+)   <section title="Temperature-Pressure Tabulation (RTParm)" ref="gm_rtparm.html"></section>
(+)    <section title="Modelling Project" ref="gm_project.html"></section>
(+)    <section title="Packed CSD and GEM results for equilibrium state" ref="gm_syseq.html">
(2+)   <section title="Process Simulation (Proces)" ref="gm_proces.html"></section>
(2+)   <section title="Data sampler operation (GtDemo)" ref="gm_gtdemo.html"></section>
(2+)   <section title="Dual Thermodynamics (DualTh)" ref="gm_dualth.html"></section>
(+)    <section title="Sensitivity Analysis (UnSpace)" ref="gm_unspace.html"></section>
(2+)   <section title="Coupled Mass Transport (GEM2MT)" ref="gm_gem2mt.html"></section>
    </section>
// 3 
(-) <section title="Graphical User's Interface (GUI)" ref="gems_gui.html">
(+)    <section title="System Stoichiometry Configuration (ElementsDialog)" ref="elements.html"> </section>
(+)    <section title="AutoPhaseSetup dialog to make/open modelling projects" ref="autophase_setup.html"></section>
(+)    <section title=" Preferences SettingsDialog" ref="gemsetup.html#PREFERENCES_DIALOG"></section>
    </section>
//4 
     <section title="Theory and methods of calculations implemented in GEMS code" ref="theorgem.html">
(-*)   <section title="Theoretical issues of GEM equilibria calculations" ref="theorgem.html"> </section>
(-*)   <section title="Thermodynamic Data Calculations" ref="theortdb.html"></section>
(-)    <section title="Default (Kernel) Database" ref="kernel_db.html"></section>
(-*)   <section title="Theoretical Extensions for Surface Complexation" ref="theorscm.html"></section>
     </section>
//5 (gt_howto.html used before) 
(-*) <section title="The HowTo documentation files" ref="g_tutor.html">
(+)    <section title="How to Manage Modelling Projects" ref="gp_howto.html"> </section>
(+)    <section title="How To Calculate Equilibria in Geochemical Systems" ref="ge_howto.html"></section>
(+)    <section title="How to maintain and extend thermodynamic database" ref="gt_howto.html"></section>
(4+)   <section title="Miscellaneous HOWTO" ref="g_miscel.html"></section>
(+)    <section title="Getting Started Manual" ref="gemstart-body.html"></section>
    </section>

// empty files
"templ_key.html"  (General documentation files)
"setfilters.html" (General documentation files)
"gems_toc.html"   (General documentation files)
"process_setup.html" (Graphical User's Interface)
"theorkin.html"     (Theory and methods of calculations implemented in GEMS code)
"theoruns.html"     (Theory and methods of calculations implemented in GEMS code)
"g_tests.html"      (The HowTo documentation)
"g_formats.html"

// internal files
"GEMS-techdata.html"
"index.html"
"g_objndx.html"		
"gemstar2.html"
"gemstdbm.html"

//old files
"gm_integ.html"
"gemstart.html" (use "gemstart-body")

All inserted in gems3.qhp
Use gems3.qhcp

19/08/2009
Added all objects from g_objndx.html as keywords in Help system gems3

24/08/2009
Added ProjectWizard to gems3

26/08/2009
Default window sizes
ICOMP (450, 340); DCOMP (620,500); REACDC (520,600); RTPARM (695, 480);
Phase (630,440); Compos(620,440); Sdref(630,520); const_(590, 420); SysEq(640, 500)
MtParm (770, 660); System (640,600); EqDemo (690, 570);  Multi(800, 620); 
Process (800, 700); GTDemo (660,620); GEM2MT (800, 630); Project (800, 660) 
  
04/09/2009
Fixed bug - When you open a project and invoke the option "change file configuration
for the selected project", the hierarchy of database files in the dialog is screwed up.

07/09/2009
Fixed bug - I changed the status of gas phase to "-" but when pressing BCC button it would go
back to "+". So effectively the program would not let me switch off the gas phase manually.
If changed switch for Phase, then will change all switch for dcomp/reacts for this phase.

09/09/2009
Fixed bugs
1. It appears that it is not possible to enter the indicator for unknown value "---"
   into any cells that should be recalculated manually. But better use command "Clear".
2. A rubbish phase with some odd values (mole amounts, activity etc.) was displayed
   on Single Thermodynamic Window. (Problem in diferent threads for calculation and showing )

21/09/2009
1. Fixed files selections error in SetFiltersDialog
2. fixed problem "The Phase wizard looks not nice"
3. Unchecked volume constraint on ElementsDialog


9/10/2009
1. Selected first field of tables, when Tab navigations
2. Added scrolling for text fields without editing field (warning from 04-Sep-2009)
3. For Help-system  (add assistant.exe, QtCLucene4.dll, QtCore4.dll, QtGui4.dll,
                     QtHelp4.dll, QtNetwork4.dll, QtSql4.dll, QtWebKit4.dll to
                     gems3.exe directory in Windows )

 12/10/2009
 1.  Changed left mergin of the window for system and equlibrium trees.


 16/10/2009
 Fixed some errors when displaing tables:
 1. Show table view, if first object is empty
 2. Hide Text edit table, if object is empty

 02/11/2009
 Fixed errors
 1. Problems in copy/paste multiselections
 2. Seted not exclusive property for the button group in ExcludeFillDialog
 3. Changed fill_data to aPa->pa.p.DHB*10. from aPa->pa.p.DB*10.

 03/11/2009
 Fixed some errors for Paste/PasteTransposed commands

 04/11/2009
 Fixed error if "Clear"/"Paste"/"PasteTransposed" commands used for no editable fielda
 Hide up level scrolling for text fields when editing field

 09/11/2009
 Live text editor without mouse ( "Esc" - not save; "F4" - save changes )

10/11/2009
 Fixed error - the wizards doesnot read the configuration of a record when you do the remake.
 Added icon "New(Clone)" and changed order of commands on module and Newsystem windows.
 Setup right-aligment for the numerical cells (module and system windows)
 Labels for the text fielda aligned at top
 Spell out "Files" for button for file configuration
 Extended fields Lsmod and Lsmdc on Details for expert window


 11/11/2009
 Changed window hight for dialogs (KeyDialog4, KeyProfile4, NewsystemDialog4, SelectDialog4)
 Changed "inactiv" selection color for lists in dialogs (KeyDialog4, SelectDialog4)
 Changed size of bar the proportion in ProgressDialogData
 Removed outer frame line for RTparm and Project wizards
 Added possibility to copy work record key for all modules (not only system)
 Fixed selection error when command  "Ctr+L" and "Ctr+R" for modules

 12/11/2009
  Changed functionality "Select record before open of module window".
  Do not request for "Save" after cell editing mode without changin anything (bug from 04-Sep-2009 ).
  Bugfix: We cannot "Cancel" module window closing when "Data record has been changed"

 13/11/2009
  Bugfix: Clear edit focus and save changing before all actions for modules

 16/11/2009
   Bugfix: Returned New(clone) to menu
   Bugfix: Hide selection first line in SelectDialog
   Remove outer frame about data objects

18/11/2009
  Bugfix: When loaded a SysEq/Project record and then remake this record,
      also the list of records pops up and prompts me to select a record.
  Bugfix: Make GTDemo record from Process
  Added HOWTO-Build-help.txt

19/11/2009
 Changed configuration of DComp, Phase, GEM2MT and DualTh windows in viz_cn.ini
 Bugfix: error in calculation size for complex tables

20/11/2009
 The width of the record key field must be that the whole record key is always visible

25/11/2009
Fixed Unreadable characters in Const records (see const_names.doc)
On window <Gam &C> added header for tabl qd
Bugfix: execute command "&Show record"/F7 if record is not exist in opened DB files

30/11/2009
Bugfix: Clicking on some empty cells in tables results in crash

11/12/2009
Inserted Changes to pscripts
1. If error in condition, we ignore this condition, after question
2. If error when calculate condition, we set result to false (do not print this line), after question
3. Print "NULL" value, if array is not allocated

Problems in scripts:
1.   "pscript-sorption:0004:system:"
     Table 3A - OK, but error data in SCMT_, header must be changed (change columns count)
2.   "pscript-seqreac:0001:system:"
     Table 3 - xp_ array is not allocated in test Projects
3.   "pscript-eqst-aqsorp:0002:multi:"
     Table 2,  error in condition for sati => script must be changed

14/12/2009
Inserted changes into "The read from GEMIPM2K files menu item"
I. Before executing this commant you must read and recalculate start record
   (from produce a set of DCH, IPM and DBR files )
II. Executing command
1.Clone the current SysEq? record (change the record key);
2.Clear the xd_, xa_, xp_ arrays to zeros and copy pmp->B (B_) vector to syp->BI (BI_ vector);
3.Offer the user to select the *.lst file name containing a set of DCH, IPM and DBR files to read in;
4.Read the contents of files into the MULTI (GEM IPM work structure);
5.Test changes in the modified system relative to MULTI (only for information);
6.Get internal mass of the system to scale down/up (to reproduce a GEMIPM2K calculation);
7.Unpack the DBR file content into the MULTI data structure (SD)
8.Unpack the pmp->B vector (b) into syp->BI (BI_ vector).

III. We can get different results into GEMS and into GEMIPM2K
  as different values into G, V ...
   (interpolation of thermodynamic data or precision )


23/12/2009
BugFix: Changes in font and font size not saved
BugFix: Error when run without "-c" or "-d"

20/01/2010
Added function for compress phases records
bool TPhase::CompressRecord( int nDCused, TCIntArray& DCused, bool onlyIPX )


21/01/2010
Added function for compress arrays of phase-related coefficients of non-ideality model
before copy to Multi structure

26/01/2010
Added InputSystemDialog for Input Recipe of Single Thermodynamic System
and command/button "Press to set up bulk chemical composition (BCC) of the system"

29/01/2010
Changed CalcDialog - did no selections into dialog
Added EquatSetup class for defining/building Output plots

02/02/2010
Finished EquatSetup class for defining/building Output plots
Added Output Dialog for GtDemo

03/02/2010
Added Output Dialog for Process
Changed files configurations (deleted all files from standalone/GEMIPM2K)


04/02/2010
Set up different default colors for default plotlines

05/02/2010
Added Output Dialog for RTparam and did some changes in calculation rtparm module
BugFix: In mathscripts preprocessin select DC index  for the DC name <name> in statement,
    if more then one  DCs with this name switched on
Added Dialog for select one number from integer list

10/02/2010
Added window for setup default scripts for Process calculation (types P, G, S, L)
on the ProcessWizard screen
BugFix: ExcludeFillDialog window was high

11/02/2010
Added SystemWizard
BugFix: cut titles on GraphWindow
Changed Remake command for System

12/02/2010
Changed MAXGRNAME form 7 to 16 (did names of graphic lines longer)


T. Wagner
1. Module windows - remove outer frame about data objects.
    We use outer frame around work&scroll area; menus and buttons are command area,
2. We have anreadable headings - We must change column size for object in viz_cn.ini file
   full name we can see into tooltip and on statusbar line
3. We can move between different tables using "Tab" key, We can move between cells in one table
   using arrow keys
4. Unreadable characters in Const records (We do not use this records in GEMS3 )



