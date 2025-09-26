//-------------------------------------------------------------------
// $Id: module_w.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of GemsMainWindow class
//
// Copyright (C) 2011  S.Dmytriyeva
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (https://qt.io/download-open-source)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the GPL v.3 license

//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------
#ifndef GemsMainWindow_h_
#define GemsMainWindow_h_

#include <QMainWindow>
#include <QProcess>
#include <QWaitCondition>
#include <QTimer>
#include <QPainter>
#include <QLineEdit>
#include <QThread>
#include <QHBoxLayout>
#include <QTableWidget>
#include "module_w.h"
#include "v_vals.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QAction;
class QToolButton;
class QMenu;
class QComboBox;
class QMdiArea;
class QMdiSubWindow;
class QSplitter;
QT_END_NAMESPACE

class IPNCalcObject;
class TCModule;
class NewSystemDialog;

namespace jsonui {
class GraphDialog;
}
using namespace jsonui;
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
                      thChoice, thChoice2, thExcludeFillEdit, thQuestionYesNoAll  };
// data for thread
struct DThread
{
    int res;
    bool wait;
    //vfMessage & vfQuestion
    std::string title;     //prompt(vfChoice), caption
    std::string mess;
    //vfQuestion3
    std::string s1;
    std::string s2;
    std::string s3;
    //vfChoice (error, open only one SysEq)
    TCStringArray list;
    int seli;
    //vfChoise3
    bool all_no; // only return
    //vfExcludeFillEdit
    std::vector<bool> sel;
    double fill_data;

    //Message&Quastion&Question3
    void setDThread( const std::string& title_, const std::string& mess_,
                     const std::string& s1_="", const std::string& s2_="", const std::string& s3_="")
    {
        res =0;
        wait = true;
        title = title_;
        mess =mess_;
        s1 =s1_;
        s2 =s2_;
        s3 =s3_;
    }
    //vfChoice&vfChoice2
    void setDThread( TCStringArray& arr, const char* prompt, int sel_, bool all_=false )
    {
        res =0;
        wait = true;
        title = prompt;
        mess ="";
        all_no = all_;
        seli = sel_;
        list.clear();
        for( size_t ii=0; ii<arr.size(); ii++ )
            list.push_back( arr[ii]);
    }
    //vfExcludeFillEdit
    void setDThread( const char* caption, TCStringArray& aICkeys,
                     double fill_data_ )
    {
        res = 0;
        wait = true;
        title = caption;
        fill_data = fill_data_;
        list.clear();
        for( size_t ii=0; ii<aICkeys.size(); ii++ )
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

namespace Ui {
class GemsMainWindowData;
}

/*!
 \class TVisorImp
 \brief This class handles work of TVisor with real GUI classes
*/
class TVisorImp: public QMainWindow
{
    Q_OBJECT

    Ui::GemsMainWindowData *ui;
    int argc;
    char** argv;

    time_t last_update;
    int updateTime;

    //int ProfileMode;

    // Appearance settings
    QFont	CellFont;
    QFont	axisLabelFont;
    QFont	defaultFont;
    int charWidth;
    int charHeight;
    int colorScheme=0;

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

    void OpenModule(QWidget* parent, uint i, int page=0, int viewmode=0, bool select=false);
    void defineModuleKeysList( size_t nRT );
    void GetHelp();
    void OpenHelp(const char* file, const char* item=nullptr, int page =-1);
    bool Message( QWidget* parent, const char* name,
                  const char* msg, int prog, int total, bool move);
    void CloseMessage();
    void ProcessProgress( QWidget* parent, int nRT );

    // work with default values
    int updateInterval() const
    {   return updateTime;   }
    void setUpdateInterval(int updInterval)
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

    int getColorScheme() const
    {  return colorScheme;  }
    void setColorScheme(int newColorScheme)
    {  colorScheme = newColorScheme;   }

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

Q_SIGNALS:
    void run_IPM();

public slots:
    void finish_IPN();
    void error_IPN(std::string err_mess);

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
    void CmShow( const char * key=nullptr );  //Demonstrate (show) Data Record
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

    //--- Manipulation list of records
    void CmCopyList();
    void CmRenameList();
    void CmTransferList();
    void CmDeleteList();
    void CmKeysToTXT();
    void CmBackup();
    void CmRestore();
    void CmExport();
    void CmImport();
    void CmBackuptoJson();
    void CmRestorefromJson();


    // NewSystemDialog
    void CmOpen_MTPARAM();
    void CmOpen_SYSTEM_ICOMP();
    void CmInsert_SYSTEM();
    void CmOutMulti();
    void CmReadMulti();
    void CmProjectExportJson();
    void CmProjectImportJson();

    void CmRunBCC();
    void CmRunIPM();

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

    bool SetProfileMode( const char * profileKey = nullptr);
    bool SetGeneralMode();
    void CmHelpAbout();
    void CmHelpAuthors();
    void CmHelpThanks();
    void CmHelpLicense();
    void Update(bool force);
    void theadService( int nFunction, QWidget* par );


private slots:
    void updateMenus();
    void updateWindowMenu();
    void CmCalcMode();
    void CmDataBaseMode();
    void moveToolBar( int pos=10, int index=1 );
    void setActiveSubWindow(QWidget *window);
    void setActiveSubWindowName(const QString& name);
    void changeKeyList();

    // Run extern GEMS3 server
    //void startGEMServer();

    // Kills the GEMS3 server, causing it to exit immediately.
    // Kill when error occurred or program exit
    //void killGEMServer();

    // This slot is executed when an error occurs with the GEMS3 process.
    // The specified error describes the type of error that occurred.
    //void GEMServerErrorOccurred(QProcess::ProcessError error);

    //void readOutput();

private:
    // GEMS3 server application run
    //QProcess *GEMS3_proc = nullptr;
    /// The thread that provides GEM IPN calculation
    QThread calc_thread;
    /// GEM IPN run object
    IPNCalcObject* calc_model=nullptr;

    void setCalcClient();

    TCModuleImp *activeMdiChild(); //( MdiChild = ModuleWindow )
    NewSystemDialog *activeNewSystem();
    NewSystemDialog *activeNewSystemCommand();
    QMdiSubWindow *findMdiChild(const QString &moduleName);
    QMdiSubWindow *findMdiGraph(const QString &moduleName);
    QMdiSubWindow *findNewSystem();
    string nameMdiChild( QWidget *p );
    int nRTofActiveSubWindow();
    QIcon iconMdiChild( QWidget *p );

    void changeModulesKeys( int nRT );
    void defineButtonsList();
    void setActions();

    //void createStatusBar();

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

    // menu collection
    QToolBar *toolDataBase;
    QToolBar *toolProject;
    QAction *separatorAct;
    QLineEdit *pLine;
    QComboBox *pModuleName;
};

extern TVisorImp* pVisorImp;
extern Qt::HANDLE pThread;

#endif   // _GemsMainWindow_h_
