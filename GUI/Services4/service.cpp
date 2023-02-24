//-------------------------------------------------------------------
// $Id: service.cpp 999 2008-01-17 13:34:35Z gems $
//
// Implementation of some GUI service functions
//
// Copyright (C) 1996-2008  A.Rysin, S.Dmytriyeva
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


#ifndef _WIN32
#include <unistd.h>
#else
#include <io.h>
#endif

#include <QFileDialog>
#include <QDir>
#include <QMutexLocker>
#include <QMessageBox>
#include <QInputDialog>

#include "service.h"
#include "visor.h"

#include "SelectDialog.h"
#include "KeyFilter.h"
#include "ExcludeFillDialog.h"
#include "ProcessWizard.h"
#include "GEM2MTWizard.h"
#include "UnSpaceWizard.h"
#include "GtDemoWizard.h"
#include "ComposWizard.h"
#include "DCompWizard.h"
#include "ReacDCWizard.h"
#include "RTparmWizard.h"
#include "PhaseWizard.h"
#include "DualThWizard.h"
#include "ProjectWizard.h"
#include "SystemWizard.h"
#include "ListFilesDialog.h"
#include "ElementsDialog.h"
#include "KeyDialog.h"
#include "KeyProfile.h"
#include "ChangeKeyDialog.h"
#include "AutoPhaseDialog.h"
#include "LookupDialog.h"
#include "LoadMessage.h"
#include "InputSystemDialog.h"
#include "PhaseInfoDialog.h"
//using namespace std;


void deleteDirectory(QString dir);
void vfObjToFile(QWidget* par, TObject* obj);
QMutex qmutex;

//----------------------------------------------------------------
// GUI Service functions for modules
//----------------------------------------------------------------

bool vfQuestion(QWidget* par, const std::string& title, const std::string& mess)
{
    if( pThread != QThread::currentThreadId() )
    {
        QMutexLocker  loker(&qmutex);
        pVisorImp->thdata.setDThread( title, mess );
        QMetaObject::invokeMethod( pVisorImp, "theadService",
                                   Q_ARG( int, thQuestion ),  Q_ARG( QWidget*, par ) );
        ThreadControl::wait();
        return pVisorImp->thdata.res;
    }
    QString titl, spac, messag;
    titl = title.c_str(); spac = "\n\n"; messag = mess.c_str();

    int rest = (QMessageBox::question(par,
                                  #ifndef _WIN32
                                  #ifdef __APPLE__
                                      "Title", titl.append(spac+=messag),
                                  #else
                                      titl, messag,
                                  #endif
                                  #else
                                      titl, messag,
                                  #endif
                                      QMessageBox::Yes | QMessageBox::No));
    return rest==QMessageBox::Yes;
}

int vfQuestYesNoCancel(QWidget* par, const string& title, const string& mess)
{
    QString titl, spac, messag;
    titl = title.c_str(); spac = "\n\n"; messag = mess.c_str();

    int result = QMessageBox::question(par,
                                   #ifndef _WIN32
                                   #ifdef __APPLE__
                                       "Title", titl.append(spac+=messag),
                                   #else
                                       titl, messag,
                                   #endif
                                   #else
                                       titl, messag,
                                   #endif
                                       QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                                       QMessageBox::Yes); // default button

    switch( result )
    {
    default:
    case QMessageBox::Yes:      return VF_YES;
    case QMessageBox::No:       return VF_NO;
    case QMessageBox::Cancel:   return VF_CANCEL;
    }
    return VF_YES;
}

void vfMessage(QWidget* par, const std::string& title, const std::string& mess, WarnType type)
{
    if( pThread != QThread::currentThreadId() )
    {
        QMutexLocker  loker(&qmutex);
        // (2)
        pVisorImp->thdata.setDThread( title, mess );
        QMetaObject::invokeMethod( pVisorImp, "theadService",
                                   Q_ARG( int, thMessage ),  Q_ARG( QWidget*, par ) );
        ThreadControl::wait();
        return;
    }
    QString titl, spac, messag;
    titl = title.c_str(); spac = "\n\n"; messag = mess.c_str();

    switch( type )
    {
    case vfWarn:
        QMessageBox::warning(par,
                     #ifndef _WIN32
                     #ifdef __APPLE__
                             "Title", titl.append(spac+=messag)
                     #else
                             titl, messag
                     #endif
                     #else
                             titl, messag
                     #endif
                             );
        break;
    case vfErr:
        QMessageBox::critical(par,
                      #ifndef _WIN32
                      #ifdef __APPLE__
                              "Title", titl.append(spac+=messag)
                      #else
                              titl, messag
                      #endif
                      #else
                              titl, messag
                      #endif
                              );
        break;
    default:
        QMessageBox::information(par,
                         #ifndef _WIN32
                         #ifdef __APPLE__
                                 "Title", titl.append(spac+=messag)
                         #else
                                 titl, messag
                         #endif
                         #else
                                 titl, messag
                         #endif
                                 );
    }
}

static int posx=0, posy=0;
// returns VF3_1, VF3_2 or VF3_3
int vfQuestion3(QWidget* par, const std::string& title, const std::string& mess, const std::string& s1,
                const std::string& s2,  const std::string& s3, bool i_mov )
{
    if( pThread != QThread::currentThreadId() )
    {
        QMutexLocker  loker(&qmutex);
        // (2)
        pVisorImp->thdata.setDThread( title, mess, s1, s2, s3 );
        QMetaObject::invokeMethod( pVisorImp, "theadService",
                                   Q_ARG( int, thQuestion3 ),  Q_ARG( QWidget*, par ) );
        ThreadControl::wait();
        return pVisorImp->thdata.res;
    }
    QString titl, spac, messag;
    titl = title.c_str(); spac = "\n\n"; messag = mess.c_str();

    QMessageBox qm( QMessageBox::Question,
                #ifdef __unix
                #ifdef __APPLE__
                    "Title", titl.append(spac+=messag),
                #else
                    titl, messag,
                #endif
                #else
                    titl, messag,
                #endif
                    QMessageBox::NoButton, par);

    QAbstractButton *yesButton = qm.addButton(s1.c_str(), QMessageBox::YesRole);
    QAbstractButton *noButton = qm.addButton(s2.c_str(), QMessageBox::NoRole);
    QAbstractButton *cancelButton = nullptr;
    if( !s3.empty() )
        cancelButton = qm.addButton(s3.c_str(), QMessageBox::RejectRole);
    if( i_mov )
        qm.move(posx, posy);
    qm.exec();
    if( i_mov )
    {
        posx = qm.x();
        posy = qm.y();
    }
    if (qm.clickedButton() == yesButton) {
        return VF3_1;
    }
    else if (qm.clickedButton() == noButton) {
        return VF3_2;
    }
    else if (qm.clickedButton() == cancelButton) {
        return VF3_3;
    }
    return VF3_3;
}


// Use only main thread
int vfQuestionYesNoAll(QWidget* par, const std::string& title, const std::string& mess, const std::string& s1 )
{
    if( pThread != QThread::currentThreadId() )
    {
        QMutexLocker  loker(&qmutex);
        pVisorImp->thdata.setDThread( title, mess, s1 );
        QMetaObject::invokeMethod( pVisorImp, "theadService",
                                   Q_ARG( int, thQuestionYesNoAll ),  Q_ARG( QWidget*, par ) );
        ThreadControl::wait();
        return pVisorImp->thdata.res;
    }

    QString titl, spac, messag;
    titl = title.c_str(); spac = "\n\n"; messag = mess.c_str();
    QMessageBox::StandardButton defbutton = QMessageBox::Yes;
    if( s1 == "No" )
        defbutton = QMessageBox::No;

    auto res = QMessageBox::question(par,
                                 #ifdef __unix
                                 #ifdef __APPLE__
                                     "Title", titl.append(spac+=messag),
                                 #else
                                     titl, messag,
                                 #endif
                                 #else
                                     titl, messag,
                                 #endif
                                     QMessageBox::Yes | QMessageBox::YesToAll
                                     | QMessageBox::No | QMessageBox::NoToAll | QMessageBox::Cancel,
                                     defbutton);
    switch( res )
    {
    default:
    case QMessageBox::Yes:      return VF_YES;
    case QMessageBox::No:       return VF_NO;
    case QMessageBox::YesToAll: return VF_YES_ALL;
    case QMessageBox::NoToAll:  return VF_NO_ALL;
    case QMessageBox::Cancel:   return VF_CANCEL;
    }
    return VF_YES;
}



//=============================================
// Choice dialogs
//=============================================

int vfChoice(QWidget* par, TCStringArray& arr, const char* prompt, int sel)
{
  if( pThread != QThread::currentThreadId() )
  {
     QMutexLocker  loker(&qmutex);
     // (2)	
     pVisorImp->thdata.setDThread( arr, prompt, sel );
   	 QMetaObject::invokeMethod( pVisorImp, "theadService", 
  	        Q_ARG( int, thChoice ),  Q_ARG( QWidget*, par ) );
    ThreadControl::wait();
    return pVisorImp->thdata.res;
  }
    SelectDialog cw(par, prompt, arr, sel);
    cw.exec();
    return cw.selected();
}

int vfChoice(QWidget* par, const char* title, const char* prompt,
         int nVal, int *arr, int sel)
{
  QStringList items;
  bool ok;

  for( int ii=0; ii<nVal; ii++)
   items << QString("%1").arg(arr[ii]);

  QString res =  QInputDialog::getItem ( par, title, prompt,
    items, sel, false,  &ok  );

  if (ok && !res.isEmpty())
      return res.toInt(&ok);

  return -1;
}


int vfChoice2(QWidget* par, TCStringArray& arr, const char* prompt,
              int sel, bool& all_)
{
    if( pThread != QThread::currentThreadId() )
    {
        QMutexLocker  loker(&qmutex);
        // (2)
        pVisorImp->thdata.setDThread( arr, prompt, sel, all_ );
        QMetaObject::invokeMethod( pVisorImp, "theadService",
                                   Q_ARG( int, thChoice2 ),  Q_ARG( QWidget*, par ) );
        ThreadControl::wait();
        all_ = pVisorImp->thdata.all_no;
        return pVisorImp->thdata.res;
    }
    SelectDialog cw(par, prompt, arr, sel, all_);
    cw.exec();
    return cw.selected(all_);
}

TCIntArray 
vfMultiChoice(QWidget* par, TCStringArray& arr, const char* prompt)
{
    TCIntArray sel;
    SelectDialog cw(par, prompt, arr, sel);
    cw.exec();
    return cw.allSelected();
}

TCIntArray 
vfMultiChoiceSet(QWidget* par, TCStringArray& arr, const char* prompt, TCIntArray& sel )
{
    SelectDialog cw(par, prompt, arr, sel);
    cw.exec();
    return cw.allSelected();
}

TCStringArray vfMultiKeys(QWidget* par, const char* caption,
                            uint iRt, const char* key )
{
    TCStringArray sel;
    KeyDialog dbk(par, iRt, sel, key, caption);
    dbk.exec();
    return dbk.allSelectedKeys();
}

TCStringArray vfMultiKeysSet(QWidget* par, const char* caption,
                    uint iRt, const char* key, TCStringArray& sel )
{
    KeyDialog dbk(par, iRt, sel, key, caption);
    dbk.exec();
    return dbk.allSelectedKeys();
}

TCStringArray vfRDMultiKeysSet(QWidget* par, const char* caption,
                    const char* key, TCStringArray& sel, short NsuT )
{
    RDKeyDialog dbk(par, sel, key, caption, NsuT);
    dbk.exec();
    return dbk.allSelectedKeys();
}

//----------------------------------------------------------------
// System remake dialog
bool
vfExcludeFillEdit(QWidget* par, const char* caption,
   TCStringArray& aICkeys, std::vector<bool>& sel, double& fill_data )
{
  if( pThread != QThread::currentThreadId() )
  {
     QMutexLocker  loker(&qmutex);
     // (2)	
     pVisorImp->thdata.setDThread( caption, aICkeys, fill_data );
   	 QMetaObject::invokeMethod( pVisorImp, "theadService", 
  	        Q_ARG( int, thExcludeFillEdit ),  Q_ARG( QWidget*, par ) );
    ThreadControl::wait();
    if( !pVisorImp->thdata.res )
      return false;

    sel = pVisorImp->thdata.sel;              
    fill_data = pVisorImp->thdata.fill_data;  

    return true;
  }

    ExcludeFillDialog dbk(par, caption, aICkeys, fill_data );
    if( !dbk.exec() )
        return false;

    sel = dbk.getFillType();
    fill_data = dbk.getFillValue();

    return true;
}

//=============================================
// KeyEdit dialogs
//=============================================

string
vfKeyEdit(QWidget* par, const char* caption, uint iRt, const char* key)
{
    KeyDialog dbk(par, iRt, key, caption);
    if( !dbk.exec() )
        return "";

    return dbk.getKey();
}


// Return
// int genGEMS3k > 0 generate GEMS IPN files for all systems and processes;  genGEMS3k == 2 brife mode
// int recalc_all > 0 recalculate all systems before open project;  recalc_all == 2 generate dump file with result
// makeCalc: 0 - no recalculation; 2- NEED_GEM_SIA; 1-NEED_GEM_AIA
string vfKeyProfile( QWidget* par, const char* caption, int iRt,
                     bool& chAqGas, bool& addFiles, bool& remake,
                     string& key_templ,
                     int& recalc_all, int& genGEMS3k, int& makeCalc )
{
    KeyProfile dbk(par, iRt, caption);
    if( !dbk.exec() )
        return "";

    chAqGas = dbk.getAqGasState();
    addFiles = dbk.getFilesState();
    key_templ = dbk.getTemplateKey();
    remake = dbk.getRemakeState();

    makeCalc = dbk.getCalcMode();
    genGEMS3k =  dbk.getGEMSExportMode();
    recalc_all = dbk.getDumpMode();

    return dbk.getKey();
}

string
vfKeyTemplEdit(QWidget* par, const char* caption, uint iRt, const char* key,
               bool allowTemplate)
{
  if( pThread != QThread::currentThreadId() )
       Error( key, "Must be no template key");

    KeyFilter dbk(par, iRt, key, caption, allowTemplate);
    if( !dbk.exec() )
        return "";

    return dbk.getFilter();
}


bool
vfKeyCanged(QWidget* par, const char* caption,
            string& from_str, string& to_str, int fldLen )
{
    ChangeKeyDialog dbk(par, fldLen, caption );
    if( !dbk.exec() )
        return false;

    from_str = dbk.getTemplFrom();
    to_str =  dbk.getTemplTo();

    return true;
}

double vfGetDouble( QWidget * par, const char* title, const char* label,
  double value, double min, double max, int decimals )
{
    bool ok;
    double d = QInputDialog::getDouble(par, title,
    label, value, min, max, decimals, &ok);
    if(!ok)
      d = value; // or Error("Cancel");
    return d;
 }

//=============================================
// Open - Save dialogs
// @param par - parent widget
// @param path - source directory
// @param title - dialog title
//=============================================

bool vfChooseFileOpen(QWidget* par, std::string& path_,
          const char* title , const char* filter )
{
    std::string path;
    if( path_.find('/') == std::string::npos )
    {
        path = pVisor->localDir();//userGEMDir();
        path  +=  "/";
        path += path_;
    }
    else   path = path_;

    QString filt;
    if( filter )
        filt = QString("Text files (%1);;All files (*)").arg(filter);
    else
        filt = "Text files (*.txt);;All files (*)";

    QString fn = QFileDialog::getOpenFileName(  par,title,
          path.c_str(), filt, 0,
          QFileDialog::DontConfirmOverwrite );
   if ( !fn.isEmpty() )
    {
        path_ = fn.toStdString();
        std::string dir;
        std::string name;
        std::string newname;
        u_splitpath( path_, dir, name, newname );
        pVisor->setLocalDir( dir );
        return true;
    }
    else
    {
        path = "";
        return false;
    }
}

bool vfChooseFileSave(QWidget* par, std::string& path_,
       const char* title, const char *filter)
{
      std::string path;
      if( path_.find('/') == std::string::npos )
      {
          path = pVisor->localDir();//userGEMDir();
          path +=  "/";
          path += path_;
      }
      else   path = path_;

    QString filt;
    if( filter )
        filt = QString("Files (%1);;All files (*)").arg(filter);
    else
        filt = "Text files (*.txt);;All files (*)";

    replace(path, "\\", "/");

    QString fn = QFileDialog::getSaveFileName( par, title,
         path.c_str(), filt, 0,
         QFileDialog::DontConfirmOverwrite);
    //QFileDialog flDialog( par, title, path.c_str(), filt.c_str() );
    //QStringList fn;
    //if (flDialog.exec())
    // fn = flDialog.selectedFiles();

    if ( !fn.isEmpty() )
    {
        path_ = fn.toStdString();
        std::string dir;
        std::string name;
        std::string newname;
        u_splitpath( path_, dir, name, newname );
        pVisor->setLocalDir( dir );
        return true;
    }
    else
    {
        path = "";
        return false;
    }
}


bool vfChooseDirectory(QWidget* par, std::string& path_,
       const char* title )
{
      std::string path;
      if( path_.find('/') == std::string::npos )
      {      
            path = pVisor->localDir();//userGEMDir();
            path +=  "/";
            path+= path_;
      }
      else   path = path_;

      replace( path, "\\", "/");
      QString dir = QFileDialog::getExistingDirectory(par, title,  path.c_str(),
                                                     QFileDialog::ShowDirsOnly
                                                     | QFileDialog::DontResolveSymlinks);

    if ( !dir.isEmpty() )
    {
        path_ = dir.toStdString();
        pVisor->setLocalDir( path_ );
        return true;
    }
    else
    {
        path_ = pVisor->localDir();
        return false;
    }
}

void deleteDirectory(QString dir)
{
    QDir data_dir(dir);

    //First delete any files in the current directory
    QFileInfoList files = data_dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
    for(int file = 0; file < files.count(); file++)
    {
        data_dir.remove(files.at(file).fileName());
    }

    //Now recursively delete any child directories
    QFileInfoList dirs = data_dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs);
    for(int dr = 0; dr < dirs.count(); dr++)
    {
        deleteDirectory(dirs.at(dr).absoluteFilePath());
        data_dir.rmdir(dirs.at(dr).absoluteFilePath());
    }
}


void vfMakeDirectory(QWidget* par, const char *dir, int askOverwrite )
{
    // make directory dir (find system function)
    QDir d(dir);
    if ( d.exists() )
    {
        if( askOverwrite )
        {
            std::string mess = dir;
            mess += "\n";
            mess+=  "This directory exists! Overwrite?";
            if( !vfQuestion( par, "Create new directory",mess) )
                Error( dir, "Error creating directory!");

            if( askOverwrite == 2)
            {
              deleteDirectory(dir);
            }
        }
        return;
    }
    if( !d.mkdir( dir ))
        Error( dir, "Error creating directory!");
}

// exports obect to TXT file
//
// @param par - parent widget
// @param obj - object to export
void vfObjToFile(QWidget* par, TObject* obj)
{
    std::string filename;
AGAIN:
    if( vfChooseFileSave(par, filename, "Please, provide name of TXT-file") )
    {
        ios::openmode mod = ios::out;
        if( !access(filename.c_str(), 0 ) ) //file exists
            switch( vfQuestion3( par, filename.c_str(), "This file exists! What to do?",
                                 "&Append", "&Overwrite", "&Cancel") )
            {
            case VF3_2:
                mod = ios::out;
                break;
            case VF3_1:
                mod = ios::out|ios::app;
                break;
            case VF3_3:
                goto AGAIN;
            }
        fstream f( filename.c_str(), mod );
        /// errors
        obj->toTXT( f );
    }
}

//=============================================
// Ones used Dialogs
//=============================================

bool vfListFiles(QWidget* par, bool show_dlg, const char * prfName,
                 TCStringArray& fls, TCIntArray& cnt )
{
    ListFilesDialog cw( par, prfName );
    if( show_dlg )
    {
BACK:
        if( !cw.exec() )
            return false;

        switch( vfQuestion3(par, prfName,
                            "Did you really completed  selection of \n"
                            " database files to be linked to the project?",
                            "&Yes/Proceed", "&No/Go back", "&Help" ))
        {
        case VF3_1:
            break;
        case VF3_3:
        case VF3_2:
            goto BACK;
        }
    }
    cw.allSelected( fls, cnt );
    return true;
}

//bool
//vfElements(QWidget* par, const char * prfName,
//            TCStringArray& rds, TCStringArray& names,
//            bool& aAqueous, bool& aGaseous, bool& aSorption)
bool
vfElements(QWidget* par, const char * prfName,
           elmWindowData& elm_data, setFiltersData& sf_data )
{
    ElementsDialog eldlg( par, prfName, elm_data );
    if( !eldlg.exec() )
        return false;

    sf_data =  eldlg.getFilters();
    elm_data = eldlg.getData();

    return true;
}

bool
vfSystemInput(QWidget* par, const char * p_key,
              std::vector<windowSetupData>& wnData, std::vector<tableSetupData>& tbData,
              std::vector<pagesSetupData>& scalarsList )
{
    InputSystemDialog pdlg( par, p_key, wnData, tbData, scalarsList );
    if( !pdlg.exec() )
        return false;

    pdlg.getTable(tbData);
    return true;
}


bool
vfProcessSet(QWidget* par, const char * p_key,
             char flgs[24], int size[8], short tabInt[6], double tabDoubl[24],
string& calcScript, string& outScript, TCStringArray& names,
string& xName, string& yName )
{
    ProcessWizard pdlg( p_key, flgs, size,  tabInt, tabDoubl,
                        calcScript.c_str(), outScript.c_str(),
                        xName.c_str(), yName.c_str(), par );
    if( !pdlg.exec() )
        return false;

    pdlg.getFlags( flgs );
    pdlg.getSizes( size );
    pdlg.getTable( tabInt, tabDoubl );

    outScript = pdlg.getOutScript();
    calcScript = pdlg.getCalcScript();
    names = pdlg.getNames( xName, yName );

    return true;
}

bool
vfGEM2MTSet(QWidget* par, const char * p_key,
            char flgs[32], int size[20],  double Tai[4], double Pai[4], double Tau[3],
string& calcScript, string& outScript, TCStringArray& names,
string& xName, string& yName, TCIntArray& vtk1, TCIntArray& vtk2 )
{
    GEM2MTWizard pdlg( p_key, flgs, size, Tai, Pai, Tau,
                       calcScript.c_str(), outScript.c_str(),
                       xName.c_str(), yName.c_str(), vtk1, vtk2, par );
    if( !pdlg.exec() )
        return false;

    pdlg.getFlags( flgs );
    pdlg.getSizes( size );
    pdlg.getTdata(  Tai );
    pdlg.getPdata(  Pai );
    pdlg.getTaudata(  Tau );

    outScript = pdlg.getOutScript();
    calcScript = pdlg.getCalcScript();
    names = pdlg.getNames( xName, yName );
    pdlg.getVTK( vtk1, vtk2 );

    return true;
}

bool
vfUnSpaceSet(QWidget* par, const char * p_key,
             char flgs[38], int size[10] )
{
    UnSpaceWizard pdlg( p_key, flgs, size, par );
    if( !pdlg.exec() )
        return false;

    pdlg.getFlags( flgs );
    pdlg.getSizes( size );

    return true;
}

bool vfGtDemoSet( QWidget* par, const char * p_key, char flgs[16], int size[8],
                  string& prkey, string& script, TCStringArray& names,
                  string& xName, string& yName, TCStringArray& keys )
{
    GtDemoWizard cdlg( p_key, flgs, size, script, prkey,
                       xName, yName, keys, par );
    if( !cdlg.exec() )
        return false;
    cdlg.getSizes( size, prkey, keys );
    cdlg.getFlags( flgs );
    script = cdlg.getScript();
    names = cdlg.getNames( xName, yName );

    return true;
}

bool
vfComposSet(QWidget* par, const char * p_key,
            char flgs[6], int size[2], double& r2  )
{
     ComposWizard cdlg( p_key, flgs, size, r2, par );
     if( !cdlg.exec() )
       return false;
    cdlg.getFlags( flgs );
    cdlg.getSizes( size );
    r2 = cdlg.getR2();

    return true;
}

bool
vfDCompSet(QWidget* par, const char * p_key,
            char flgs[15], int size[4]  )
{
     DCompWizard cdlg( p_key, flgs, size, par );
     if( !cdlg.exec() )
       return false;
    cdlg.getFlags( flgs );
    cdlg.getSizes( size );

    return true;
}

bool vfReacDCSet(QWidget* par, const char * p_key,
                 char flgs[12], int size[4], TCStringArray& sel  )
{
    ReacDCWizard cdlg( p_key, flgs, size, sel, par );
    if( !cdlg.exec() )
        return false;
    cdlg.getFlags( flgs );
    cdlg.getSizes( size );
    sel = cdlg.allSelectedKeys();
    return true;
}


bool
vfRTparmSet(QWidget* par, const char * p_key,
            char flgs[10], int size[7], double val[6],
            string& script, string& xName,  string& yName,
            TCStringArray& names )
{
    string axName;
    RTparmWizard cdlg( p_key, flgs, size, val, script.c_str(),
                       xName.c_str(), yName.c_str(), par );
     if( !cdlg.exec() )
       return false;
    cdlg.getFlags( flgs, xName  );
    cdlg.getSizes( size );
    cdlg.getFloat( val );

    script = cdlg.getScript();
    names = cdlg.getNames( axName, yName );

    return true;
}

bool vfPhaseSet( QWidget* par, const char * p_key,
                 char flgs[37], int size[30], double& r2, TCStringArray& rd_sel,
                 TCStringArray& phase_sel, TCStringArray& lDCr_sel  )
{
    PhaseWizard cdlg( p_key, flgs, size, r2, rd_sel, phase_sel, lDCr_sel, par );
    if( !cdlg.exec() )
        return false;

    cdlg.getFlags( flgs );
    cdlg.getSizes( size, rd_sel, phase_sel, lDCr_sel);
    r2 = cdlg.getR2();
    return true;
}

bool
vfDualThSet(QWidget* par, const char * p_key,
            char flgs[20], int size[8]  )
{
     DualThWizard cdlg( p_key, flgs, size, par );
     if( !cdlg.exec() )
       return false;
    cdlg.getFlags( flgs );
    cdlg.getSizes( size );

    return true;
}

bool
vfProjectSet(QWidget* par, const char * p_key,
              char flgs[38], int&  taskset )
{
     ProjectWizard pdlg( p_key, flgs, par );
     if( !pdlg.exec() )
      return false;

    pdlg.getFlags( flgs );
    taskset = pdlg.get_Settings();

    return true;
}

bool
vfSystemSet(QWidget* par, const char * p_key,
            char flgs[40],  string& name, string& comment, string& EQkey )
{
     SystemWizard pdlg( p_key, flgs, name, comment,  EQkey, par );
     if( !pdlg.exec() )
      return false;

    pdlg.getFlags( flgs );
    name = pdlg.getName();
    comment = pdlg.getComment();
    EQkey = pdlg.getEQkey();

    return true;
}

// call to AutoPhaseDialog  added 18.07.03
bool
vfAutoPhaseSet(QWidget* wpar, const char *pr_key, string& a_key, string& g_key,
         char& acode, char& gcode, float apar[8]  )
{
     AutoPhaseDialog apdlg( pr_key, acode, gcode, wpar );

     apdlg.set_apar( apar );
     apdlg.set_akey( a_key );
     apdlg.set_gkey( g_key );

     if( !apdlg.exec() )
      return false;

    acode = apdlg.get_acode();
    gcode = apdlg.get_gcode();
    apdlg.get_apar( apar );
    apdlg.get_akey( a_key );
    apdlg.get_gkey( g_key );
//    nSettings = apdlg.get_Settings();

if( acode == 'S' )
	acode = 'U';     // Blocking unavailable automatic SIT mode ?????
    return true;
}

// call to LookupDialog  added 2009
bool vfLookupDialogSet(QWidget* wpar, char flags[6],
                double Tai[4], double Pai[4] )
{
	LookupDialog apdlg( wpar );

     apdlg.setTdata(  Tai );
     apdlg.setPdata(  Pai );

     if( !apdlg.exec() )
      return false;

     apdlg.getTdata(  Tai );
     apdlg.getPdata(  Pai );
     apdlg.getFlags( flags );
    return true;
}

void vfPhaseInfo(QWidget* wpar, bool system, int xph, string phname,
                 vector<int>& xdclist, vector<string>& dcnames, int xdc )
{
    PhaseInfoDialog dlg( wpar,  system, xph, phname, xdclist, dcnames, xdc );
    dlg.exec();
}

//--------------------- End of service.cpp ---------------------------
