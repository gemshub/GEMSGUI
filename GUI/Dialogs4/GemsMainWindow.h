//-------------------------------------------------------------------
// $Id: module_w.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of GemsMainWindow class
//
// Copyright (C) 2011  S.Dmytriyeva
// Uses  string class (C) A.Rysin 1999
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (http://qt.nokia.com)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the terms of GEMS4 Development
// Quality Assurance Licence (GEMS4.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------
#ifndef _GemsMainWindow_h_
#define _GemsMainWindow_h_

#include <QMainWindow>
#include <QProcess>
#include <QWaitCondition>
#include <QTimer>
#include <QPainter>
#include <QLineEdit>

QT_BEGIN_NAMESPACE
class QLabel;
class QAction;
class QToolButton;
class QMenu;
class QComboBox;
class QMdiArea;
class QMdiSubWindow;
class QSignalMapper;
class QSplitter;
QT_END_NAMESPACE

class TCModule;
class NewSystemDialog;
class GraphDialog;

#include <QTableWidget>

#include "ui_GemsMainWindow4.h"
#include "module_w.h"
#include "v_vals.h"
using namespace std;

#ifdef QT_NO_DEBUG
#define GEMS_RELEASE
#endif

struct ModuleListItem
 {
   int nRT;         // record/module num
   QString name;    // text of item
   //QString modName; // short name (aMod[nRT].GetName())
   QString toolTip;
};

// new enums
enum { MDD_DATABASE=0, MDD_SYSTEM=1 };

enum myThreadEvents { thMessage = 0, thQuestion, thQuestion3,
                      thChoice, thChoice2, thExcludeFillEdit  };
// data for thread
struct DThread
{
  int res;
  bool wait;
  //vfMessage & vfQuestion
  string title;     //prompt(vfChoice), caption
  string mess;
  //vfQuestion3
  string s1;
  string s2;
  string s3;
  //vfChoice (error, open only one SysEq)
  vector<string> list;
  int seli;
  //vfChoise3
  bool all; // only return
  //vfExcludeFillEdit
  vector<bool> sel;
  double fill_data;

  //Message&Quastion&Question3
  void setDThread( const string& title_, const string& mess_,
                   const string& s1_="", const string& s2_="",
                   const string& s3_="")
  {  res =0;
     wait = true;
     title = title_;
     mess =mess_;
     s1 =s1_;
     s2 =s2_;
     s3 =s3_;
  }
  //vfChoice&vfChoice2
  void setDThread( vector<string>& arr, const char* prompt,
                   int sel_, bool all_=false )
  {  res =0;
     wait = true;
     title = prompt;
     mess ="";
     all = all_;
     seli = sel_;
     list.clear();
     for( int ii=0; ii<arr.size(); ii++ )
       list.push_back( arr[ii]);
  }
  //vfExcludeFillEdit
  void setDThread( const char* caption, vector<string>& aICkeys,
             double fill_data_ )
  {  res = 0;
     wait = true;
     title = caption;
     fill_data = fill_data_;
     list.clear();
     for( int ii=0; ii<aICkeys.size(); ii++ )
       list.push_back( aICkeys[ii]);
  }
};

class TKeyTable: public QTableWidget
{
        Q_OBJECT

       void keyPressEvent(QKeyEvent* e);

  public:
       TKeyTable( QWidget * parent  ):
           QTableWidget(parent)
       {}
};


/*!
 \class TVisorImp
 \brief This class handles work of TVisor with real GUI classes
*/
class TVisorImp: public QMainWindow, public Ui::GemsMainWindowData
{
    Q_OBJECT

    int argc;
    char** argv;

    time_t last_update;
    size_t updateTime;

    //int ProfileMode;

    // Appearance settings
    QFont	CellFont;
    QFont	axisLabelFont;
    QFont	defaultFont;
    int charWidth;
    int charHeight;

    bool configAutosave;

    char TCpoint[32];  // Step point ID for stepwise mode

    void closeEvent(QCloseEvent*);
    void resizeEvent( QResizeEvent * event );
    void showEvent( QShowEvent * event );

public:
     DThread thdata;
     QProcess *proc;
     QMdiArea *mdiArea;

    TVisorImp(int c, char** v);
    ~TVisorImp();

    void openMdiChild( QWidget *p, bool showMaximized = false );
    void closeMdiChild( QWidget *p );

    // Sets the caption of the module
    void SetCaption(const char* mess)
         {  setWindowTitle(mess);   }
    
    // Sets the status of the module (in the statusbar)
    void SetStatus( const char* s )
         {  statusBar()->showMessage(s);   }

    void SetRecordKey( const char*s)
    { pLine->setText( tr(s)); }

    // must be done
    bool MakeQuery(TCModule& m);

    //--void SetDialog(QWidget* centralDialog);

    void OpenModule(QWidget* parent, int i, int page=0, int viewmode=0, bool select=false);
    void defineModuleKeysList( int nRT );
    void GetHelp();
    void OpenHelp(const char* file, const char* item=0, int page =-1);
    void OpenProgress(bool step=false);
    void CloseProgress();
    bool Message( QWidget* parent, const char* name,
             const char* msg, int prog, int total, bool move);
    void CloseMessage();
    void ProcessProgress( QWidget* parent, int nRT );

    // work with default values
    char GetNumericalMethod();
    unsigned short updateInterval() const
       {   return updateTime;   }
    void setUpdateInterval(unsigned short updInterval)
       {
          if( updInterval > 0 && updInterval < 60 )
                 updateTime = updInterval;
       }

    const char* version();
    const char* getGEMTitle();
    const QFont& getCellFont() const
         { return CellFont; }
    void setCellFont(const QFont& newCellFont);
    const QFont& getAxisLabelFont() const
        {  return axisLabelFont; }
    void setAxisLabelFont(const QFont& newAxisLabelFont)
        {  axisLabelFont = newAxisLabelFont;  }
    const QFont& getDefaultFont() const
        {  return defaultFont; }

    int getCharWidth() const
        {  return charWidth;  }
    int getCharHeight() const
        {  return charHeight; /* + 4;*/  }

    int getDoubleDigits() const
        {  return TValBase::doublePrecision;  }
    void setDoubleDigits(int newDoubleDigits)
        {  TValBase::doublePrecision = newDoubleDigits;   }

    void setConfigAutosave(bool autosave)
        {  configAutosave = autosave;  }
    bool getConfigAutosave() const
        {  return configAutosave;   }

    char* getTCpoint()
        {  return TCpoint;   }
    void setTCpoint( const char* str )
        {  memcpy( TCpoint, str, 15 );   }

    void setActionPrecise();

    // functions for threaded calculation
    QWaitCondition& getWaitProgress();
    QWaitCondition& getWaitCalc();
    QMutex& getMutexCalc();

public slots:
    void openRecordKey(  int row, int column  );
    void setMenuEnabled( bool menuEnabled = true );

    // select module
    void CmIComp();
    void CmDComp();
    void CmReacDC();
    void CmRTparm();
    void CmPhase();
    void CmCompos();
    void CmSysEq();
    void CmProcess();
    void CmGEM2MT();
    void CmGtDemo();
    void CmDualTh();
    void CmUnSpace();
    void CmProject();
    void CmSDref();
    void CmConst();

    void CmNext();
    void CmPrevious();
    void CmFilter();  // set Data Record filter

    //--- Manipulation current record

    void CmCreate();
    void CmNew();
    void CmShow( const char * key=0 );  //Demonstrate (show) Data Record
    void CmDerive();
    void CmCalc();  //Calculate or create Data Record
    void CmSave();
    void CmSaveAs();
    void CmDelete();
    void CmPrint();
    void CmPlot();
    void EvFileDown();
    //--void CmNewinProfile();
    //--void CmCreateinProfile();
    //--void CmLoadinProfile();

    //--- Manipulation files of Data Base   (Servis functions )
    void CmRebildFile();
    void CmAddFileToList();
    void CmAddOpenFile();
    void CmReOpenFileList();
    void CmAddEJDBToList();
    void CmAddOpenEJDB();
    void CmReOpenEJDB();

    //--- Manipulation list of records
    void CmCopyList();
    void CmRenameList();
    void CmTransferList();
    void CmDeleteList();
    void CmKeysToTXT();
    void CmExport();
    void CmImport();
    void CmBackupEJDB();
    void CmRestoreEJDB();
    void CmExportJson();
    void CmImportJson();

    // NewSystemDialog
    void CmOpen_MTPARAM();
    void CmOpen_SYSTEM_ICOMP();
    void CmInsert_SYSTEM();
    void CmOutMulti();
    void CmReadMulti();

    void CmRunBCC();
    void CmRunIPM();
    void CmSetKarpovAlgorithm();
    void CmSetIpoptAlgorithm();
    void CmSetIpnewtonAlgorithm();

    void CmOpen_EQSTAT_ICOMP();
    void CmOpen_MULTI();

    void CmPrintMtparam();
    void CmPrintSystem();
    void CmPrintEqstat();
    void CmPrInput();
    void CmPrResults();

    //Help menu
    void CmHelp();
    void CmHelp2();
    void CmScript();
    void CmSettingth();
    void CmTutorial();
    void CmMoreBCC();
    void CmMoreIPM();
    void CmHowto();

// Top level dialog

    bool SetProfileMode( const char * profileKey = 0);
    bool SetGeneralMode();
    void CmHelpAbout();
    void CmHelpAuthors();
    void CmHelpThanks();
    void CmHelpLicense();
    void CalcMulti();
//    void SaveSystem();
    void Update(bool force);
    void theadService( int nFunction, QWidget* par );


private slots:
    void updateMenus();
    void updateWindowMenu();
    void CmCalcMode();
    void CmDataBaseMode();
    void moveToolBar( int pos=10, int index=1 );
    void setActiveSubWindow(QWidget *window);
    void setActiveSubWindowIdex(int ndx);
    void changeKeyList();


 private:
    TCModuleImp *activeMdiChild(); //( MdiChild = ModuleWindow )
    NewSystemDialog *activeNewSystem();
    NewSystemDialog *activeNewSystemCommand();
    QMdiSubWindow *findMdiChild(const QString &moduleName);
    QMdiSubWindow *findMdiGraph(const QString &moduleName);
    QMdiSubWindow *findNewSystem();
    string nameMdiChild( QWidget *p );
    int indexMdiChild( QWidget *p );
    int nRTofActiveSubWindow();
    QIcon iconMdiChild( QWidget *p );

    void changeModulesKeys( int nRT );
    //void defineModuleKeysList( int nRT );
    void defineButtonsList();
    void setActions();

    void createStatusBar();

 // main window fields
    int currentNrt;
    bool settedCureentKeyIntotbKeys;
    //QIcon iconDatabase;
    //QIcon iconSystem;

    QWidget *itemWidget1;
    QLineEdit *pFilterKey;
    TKeyTable *tbKeys;

    QSplitter *splH;
    QWidget *itemWidget2;
    QHBoxLayout *layout2;
    QLabel *messageText;
    QSignalMapper *windowMapper;

 // menu collection
    QToolBar *toolDataBase;
    QToolBar *toolProject;
    QAction *separatorAct;
    QLineEdit *pLine;
    QComboBox *pModuleName;
    //QLabel *pModeName;

 };

extern TVisorImp* pVisorImp;
extern Qt::HANDLE pThread;

#endif   // _GemsMainWindow_h_
