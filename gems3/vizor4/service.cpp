//-------------------------------------------------------------------
// $Id: service.cpp 999 2008-01-17 13:34:35Z gems $
//
// Implementation of some GUI service functions
//
// Copyright (C) 1996-2008  A.Rysin, S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

using namespace std;

#ifndef __unix
#include <io.h>
#endif

#include <QFileDialog>
#include <QMutexLocker>
#include <QMessageBox>
#include <QInputDialog>

#include "service.h"

#include "SelectDialog.h"
#include "KeyFilter.h"
#include "ExcludeFillDialog.h"
#include "ProgressDialog.h"
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
#include "ListFilesDialog.h"
#include "ElementsDialog.h"
#include "KeyDialog.h"
#include "KeyProfile.h"
#include "ChangeKeyDialog.h"
#include "AutoPhaseDialog.h"
#include "LookupDialog.h"
#include "LoadMessage.h"
#include "InputSystemDialog.h"

QMutex mutex;

//----------------------------------------------------------------
// GUI Service functions for modules
//----------------------------------------------------------------

bool vfQuestion(QWidget* par, const gstring& title, const gstring& mess)
{
  if( pThread != QThread::currentThreadId() )
  {
     QMutexLocker  loker(&mutex);
     // (1)
     // int result = 3;
     // NewThread *thread = dynamic_cast<NewThread*>(QThread::currentThread());
     // if( thread )	
     // {  qRegisterMetaType<int>("int");
     //	   QMetaObject::invokeMethod(ProcessProgressDialog::pDia, "slQuestion", 
     //	   Q_ARG( void*, &result ),  Q_ARG( QWidget*, par ), Q_ARG( QString , QString(title.c_str()) ), 
     //	   Q_ARG( QString , QString(mess.c_str()) ));
     //	   ThreadControl::wait();
     //    return result;
     //	 }      
     // (2)	
       	 pVisorImp->thdata.setDThread( title, mess );
       	 QMetaObject::invokeMethod( pVisorImp, "theadService", 
       	        Q_ARG( int, thQuestion ),  Q_ARG( QWidget*, par ) );
         ThreadControl::wait();
         return pVisorImp->thdata.res;
     }
    int rest = (QMessageBox::question(par, title.c_str(), mess.c_str(),
                                     "&Yes", "&No") == 0);
    return rest;
}

int vfQuestYesNoCancel(QWidget* par, const gstring& title, const gstring& mess)
{
    int result = QMessageBox::question(
                par, title.c_str(), mess.c_str(), "&Yes", "&No", "&Cancel",
                0, 2);
    switch( result )
    {
    case 0:
        return VF_YES;
    case 1:
        return VF_NO;
        //   case 2:
        //     return VF_YES;
    }

    return VF_CANCEL;
}

void vfMessage(QWidget* par, const gstring& title, const gstring& mess, WarnType type)
{
	  if( pThread != QThread::currentThreadId() )
	  {
             QMutexLocker  loker(&mutex);
	     // (2)	
	       	 pVisorImp->thdata.setDThread( title, mess );
	       	 QMetaObject::invokeMethod( pVisorImp, "theadService", 
	       	        Q_ARG( int, thMessage ),  Q_ARG( QWidget*, par ) );
	         ThreadControl::wait();
	         return;
	     }
    switch( type )
    {
    case vfWarn:
        QMessageBox::warning(par, title.c_str(), mess.c_str() );
        break;
    case vfErr:
        QMessageBox::critical(par, title.c_str(), mess.c_str() );
        break;
    default:
        QMessageBox::information(par, title.c_str(), mess.c_str() );
    }
}

static int posx=0, posy=0;
// returns VF3_1, VF3_2 or VF3_3
int vfQuestion3(QWidget* par, const gstring& title, const gstring& mess, const gstring& s1,
            const gstring& s2,  const gstring& s3, bool i_mov )
{
  if( pThread != QThread::currentThreadId() )
  {
     QMutexLocker  loker(&mutex);
     // (2)	
   	 pVisorImp->thdata.setDThread( title, mess, s1, s2, s3 );
   	 QMetaObject::invokeMethod( pVisorImp, "theadService", 
  	        Q_ARG( int, thQuestion3 ),  Q_ARG( QWidget*, par ) );
    ThreadControl::wait();
    return pVisorImp->thdata.res;
  }
	QMessageBox qm( title.c_str(), mess.c_str(),
                    QMessageBox::Question,
                    QMessageBox::Yes | QMessageBox::Default,
                    (s3.empty()) ? (QMessageBox::No | QMessageBox::Escape) : QMessageBox::No,
                    (s3.empty()) ? QMessageBox::NoButton : (QMessageBox::Cancel | QMessageBox::Escape),
                    par);

    qm.setButtonText(QMessageBox::Yes, s1.c_str());
    qm.setButtonText(QMessageBox::No, s2.c_str());
    if( !s3.empty() )
	qm.setButtonText(QMessageBox::Cancel, s3.c_str());
    if( i_mov )
        qm.move(posx, posy);
    int res = qm.exec();
    if( i_mov )
    {
        posx = qm.x();
        posy = qm.y();
    }
    switch( res )
    {
    case QMessageBox::Yes :
        return VF3_1;
    case QMessageBox::No :
        return VF3_2;
    case QMessageBox::Cancel :
        return VF3_3;
    }
    return VF3_3;
}

//=============================================
// Choice dialogs
//=============================================

int vfChoice(QWidget* par, TCStringArray& arr, const char* prompt, int sel)
{
  if( pThread != QThread::currentThreadId() )
  {
     QMutexLocker  loker(&mutex);
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
     QMutexLocker  loker(&mutex);
     // (2)	
     pVisorImp->thdata.setDThread( arr, prompt, sel, all_ );
   	 QMetaObject::invokeMethod( pVisorImp, "theadService", 
  	        Q_ARG( int, thChoice2 ),  Q_ARG( QWidget*, par ) );
    ThreadControl::wait();
    all_ = pVisorImp->thdata.all;
    return pVisorImp->thdata.res;
  }
    SelectDialog cw(par, prompt, arr, sel, all_);
    cw.exec();
    return cw.selected( all_ );
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
                            int iRt, const char* key )
{
    TCStringArray sel;
    KeyDialog dbk(par, iRt, sel, key, caption);
    dbk.exec();
    return dbk.allSelectedKeys();
}

TCStringArray vfMultiKeysSet(QWidget* par, const char* caption,
                    int iRt, const char* key, TCStringArray& sel )
{
    KeyDialog dbk(par, iRt, sel, key, caption);
    dbk.exec();
    return dbk.allSelectedKeys();
}


//----------------------------------------------------------------
// System remake dialog
bool
vfExcludeFillEdit(QWidget* par, const char* caption,
   TCStringArray& aICkeys, TOArray<bool>& sel, double& fill_data )
{
  if( pThread != QThread::currentThreadId() )
  {
     QMutexLocker  loker(&mutex);
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

gstring
vfKeyEdit(QWidget* par, const char* caption, int iRt, const char* key)
{
    KeyDialog dbk(par, iRt, key, caption);
    if( !dbk.exec() )
        return "";

    return dbk.getKey();
}

gstring
vfKeyProfile(QWidget* par, const char* caption, int iRt,
    bool& chAqGas, bool& addFiles,
    bool& remake,  int& makeDump, gstring& key_templ )
{
    KeyProfile dbk(par, iRt, caption);
    if( !dbk.exec() )
        return "";

    chAqGas = dbk.getAqGasState();
    addFiles = dbk.getFilesState();
    key_templ = dbk.getTemplateKey();
    remake = dbk.getRemakeState();
    makeDump = dbk.getMakeDump();

    return dbk.getKey();
}

gstring
vfKeyTemplEdit(QWidget* par, const char* caption, int iRt, const char* key,
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
            gstring& from_str, gstring& to_str )
{
    ChangeKeyDialog dbk(par, caption );
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

bool vfChooseFileOpen(QWidget* par, gstring& path_,
          const char* title , const char* filter )
{
    gstring path;
    if( path_.find('/') == gstring::npos )
    {      path = pVisor->localDir();//userGEMDir();
             path+= path_;
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
        path_ = fn.toLatin1().data();
        gstring dir;
        gstring name;
        gstring newname;
        u_splitpath( path_, dir, name, newname );
        dir  += "/";
        pVisor->setLocalDir( dir );
        return true;
    }
    else
    {
        path = "";
        return false;
    }
}

bool vfChooseFileSave(QWidget* par, gstring& path_,
       const char* title, const char *filter)
{
      gstring path;
      if( path_.find('/') == gstring::npos )
      {      path = pVisor->localDir();//userGEMDir();
             path+= path_;
      }
      else   path = path_;

    QString filt;
    if( filter )
        filt = QString("Files (%1);;All files (*)").arg(filter);
    else
        filt = "Text files (*.txt);;All files (*)";

    path.replace("\\", "/");

    QString fn = QFileDialog::getSaveFileName( par, title,
         path.c_str(), filt, 0,
         QFileDialog::DontConfirmOverwrite);
    //QFileDialog flDialog( par, title, path.c_str(), filt.c_str() );
    //QStringList fn;
    //if (flDialog.exec())
    // fn = flDialog.selectedFiles();

    if ( !fn.isEmpty() )
    {
        path_ = fn.toLatin1().data();
        gstring dir;
        gstring name;
        gstring newname;
        u_splitpath( path_, dir, name, newname );
        dir  += "/";
        pVisor->setLocalDir( dir );
        return true;
    }
    else
    {
        path = "";
        return false;
    }
}

// exports obect to TXT file
//
// @param par - parent widget
// @param obj - object to export
void vfObjToFile(QWidget* par, TObject* obj)
{
    gstring filename;
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
     ElementsDialog eldlg( par, prfName );
     if( !eldlg.exec() )
      return false;

    sf_data =  eldlg.getFilters();
    elm_data = eldlg.getData();

    return true;
}

bool
vfSystemInput(QWidget* par, const char * p_key,
  TIArray<windowSetupData>& wnData, TIArray<tableSetupData>& tbData )
{
     InputSystemDialog pdlg( par, p_key, wnData, tbData );
     if( !pdlg.exec() )
      return false;

    pdlg.getTable(tbData);
    return true;
}


bool
vfProcessSet(QWidget* par, const char * p_key,
              char flgs[24], int size[6], short tabInt[6], double tabDoubl[24],
              gstring& calcScript, gstring& outScript, TCStringArray& names )
{
     ProcessWizard pdlg( p_key, flgs, size,  tabInt, tabDoubl,
                 calcScript.c_str(), outScript.c_str(), par );
     if( !pdlg.exec() )
      return false;

    pdlg.getFlags( flgs );
    pdlg.getSizes( size );
    pdlg.getTable( tabInt, tabDoubl );

    outScript = pdlg.getOutScript();
    calcScript = pdlg.getCalcScript();
    names = pdlg.getNames();

    return true;
}

bool
vfGEM2MTSet(QWidget* par, const char * p_key,
              char flgs[22], int size[17] )
{
     GEM2MTWizard pdlg( p_key, flgs, size, par );
     if( !pdlg.exec() )
      return false;

    pdlg.getFlags( flgs );
    pdlg.getSizes( size );

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

bool vfGtDemoSet(QWidget* par, const char * p_key, int size[7],
            gstring& prkey, gstring& script, TCStringArray& names )
{
     GtDemoWizard cdlg( p_key, size, script.c_str(), prkey.c_str(),  par );
     if( !cdlg.exec() )
       return false;
    cdlg.getSizes( size );
    prkey = cdlg.getPrKey();
    script = cdlg.getScript();
    names = cdlg.getNames();

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

bool
vfReacDCSet(QWidget* par, const char * p_key,
            char flgs[12], int size[4]  )
{
     ReacDCWizard cdlg( p_key, flgs, size, par );
     if( !cdlg.exec() )
       return false;
    cdlg.getFlags( flgs );
    cdlg.getSizes( size );

    return true;
}


bool
vfRTparmSet(QWidget* par, const char * p_key,
            char flgs[10], int size[7], float val[6],
            gstring& script, gstring& xName, TCStringArray& names   )
{
     RTparmWizard cdlg( p_key, flgs, size, val, script.c_str(), par );
     if( !cdlg.exec() )
       return false;
    cdlg.getFlags( flgs, xName  );
    cdlg.getSizes( size );
    cdlg.getFloat( val );

    script = cdlg.getScript();
    names = cdlg.getNames();

    return true;
}

bool
vfPhaseSet(QWidget* par, const char * p_key,
            char flgs[12], int size[6], double& r2  )
{
     PhaseWizard cdlg( p_key, flgs, size, r2, par );
     if( !cdlg.exec() )
      return false;

    cdlg.getFlags( flgs );
    cdlg.getSizes( size );
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

// call to AutoPhaseDialog  added 18.07.03
bool
vfAutoPhaseSet(QWidget* wpar, const char *pr_key, gstring& a_key, gstring& g_key,
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
bool vfLookupDialogSet(QWidget* wpar, char flags[4],
		int& nT, int& nP, float Tai[4], float Pai[4] )
{
	LookupDialog apdlg( wpar );

     if( !apdlg.exec() )
      return false;

     apdlg.getTdata( nT, Tai );
     apdlg.getPdata( nP, Pai );
     apdlg.getFlags( flags );
    return true;
}

//--------------------- End of service.cpp ---------------------------