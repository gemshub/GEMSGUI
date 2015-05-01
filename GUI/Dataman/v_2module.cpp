//-------------------------------------------------------------------
// $Id: v_module.cpp 1391 2009-08-10 12:58:16Z gems $
//
// Implementation of TSubModule, TModule and TModList classes
//
// Copyright (C) 1996-2013  S.Dmytriyeva, A.Rysin
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
#include <unistd.h>
#ifndef __unix
#include <io.h>
#endif
#include <ctime>

#include "v_2module.h"
#include "v_ejdb_file.h"
#include "visor.h"
#include "service.h"
#include "t_print.h"
#include "t_read.h"
#include "NewSystemDialog.h"
#include "GemsMainWindow.h"
#include "m_param.h"
#include "tmltsystem.h"
#include "v_mod.h"

//---------------------------------------------------------

/// Select Open file mode
bool vfChooseFileOpenMode( QWidget* par,
             const char *filename, ios::openmode& mod )
{
   if( !access(filename, 0 ) ) //file exists
    switch( vfQuestion3( par, filename, "This file exists! What to do?",
                "&Append", "&Overwrite", "&Cancel") )
    {
    case VF3_2:
        mod = ios::out;
        break;
    case VF3_1:
        mod = ios::out|ios::app;
        break;
    case VF3_3:
        return false;
    }
   return true;
}

//---------------------------------------------------------

/// (supposed to be :) Called from TModuleImp right when
/// window is created
void TSubModule::Setup()
{
    //  SetCaption( (char*)GetName() );
}

/// Called from TModuleImp right before
/// window is closed
bool TSubModule::EvClose()
{
    return true;
}

/// Open help window for module
void TSubModule::CmHelp()
{
    if(pVisor->ProfileMode == true && nRT==RT_PARAM)
         pVisorImp->OpenHelp( GetHtml(), NUMSET );
    else
         pVisorImp->OpenHelp( GetHtml() );
}

/// Open help window for current page of module
void TSubModule::CmHelp2()
{
   pVisorImp->OpenHelp( GetHtml(), MWPAGE, pImp->curPage()+1 );
}

/// Get submodule/module widget
QWidget* TSubModule::window()
{
   if( nRT== RT_SYSEQ && pVisor->ProfileMode == true )
       return (QWidget*)(NewSystemDialog::pDia);
   else
       return (QWidget*)pImp;
}

// Callback for 'Help' command
const char* TSubModule::GetHtml()
{
   return GEMS_TOC_HTML;
}

// Open module window
void TSubModule::Show(QWidget* parent,  const char *str, bool viewmode )
{
    if (str) SetString(str);
    pVisorImp->OpenModule(parent, nRT, 0, !viewmode);
}

// Updates window contents
// see also TModuleImp::Update, TCWindow::Update(),
// TCPage::Update(), TField::Update()
void TSubModule::Update(bool force)
{
    if( !pImp )		// module not opened
        return;
    pImp->Update(force);
}

// Updates contents of all windows plus caption of the current one
void TSubModule::ModUpdate( const char *str )
{
    SetString(str);
    pVisor->Update();
}

//========================================
// TCModule class
//=======================================

void TCModule::Setup()
{
    ///  SetCaption( (char*)GetName() );
}

/// Returns true if we can close the window
bool TCModule::EvClose()
{
   try
    {
        return MessageToSave();
    }
    catch( TError& xcpt )
    {
        vfMessage(pImp, xcpt.title, xcpt.mess);
    }
    return false;	// can close on error ??
}

/// Opens window with 'Remake record' parameters
void TCModule::MakeQuery()
{
    pImp->MakeQuery();
}

int TCModule::keyEditField()
{
    if( nRT == RT_RTPARM || pVisor->ProfileMode == true )
        return startKeyEdit;
    else
        return 0;
}

bool  TCModule::isCalcModules()
{
  if( nRT == RT_PARAM || nRT >= RT_SYSEQ
      /* RT_PARAM == nRT || RT_SYSEQ == nRT ||
      RT_PROCES == nRT ||  RT_GTDEMO == nRT ||
      RT_UNSPACE == nRT || RT_DUALTH == nRT ||
      RT_GEM2MT == nRT */ )
    return true;
 else
    return false;
}

/// Make all keys filter
string  TCModule::makeKeyFilter()
{
    string strfilt;

    if( pVisor->ProfileMode == true &&  isCalcModules() )
      db->MakeKey( RT_PARAM, strfilt, RT_PARAM, 0, K_END );
    else
      db->MakeKey( RT_PARAM, strfilt, K_ANY, K_END );

    return strfilt;
}

/// Return true if Filter must be changed or setted
bool  TCModule::testKeyFilter()
{
  if( Filter.empty() || Filter == ALLKEY )
      return true;

  if( pVisor->ProfileMode == true &&  isCalcModules() )
  {
    string strfilt = rt[ RT_PARAM ].FldKey(0);
    if( Filter.find( strfilt ) == string::npos )
     return true;
  }

  return false;
}

/// Save solicitation
/// Returns true if user pressed 'save' or 'discard' and false on 'cancel'
bool TCModule::MessageToSave()
{
    string key_str = db->PackKey();
    if( contentsChanged && key_str.find_first_of("*?") == string::npos )
    {
        int res = vfQuestion3(window(), key_str.c_str(),
                       "Data record has been changed!",
               "Save changes", "Discard changes", "Cancel");
        if( res == VF3_3 )
           return false;

        if( res == VF3_1 )
            RecSave( key_str.c_str() );
    }
    contentsChanged = false;
    return true;
}

/// Get key of record (existing key, new key or key temlate )
string TCModule::GetKeyofRecord( const char *oldKey,
            const char *strTitle,  int keyType )
{
    // define title
    string str = GetName();
    str += ": ";
    str += strTitle;

    // define key
    string key;
    if( oldKey == 0 )
    {
        if( Filter.empty() )
            key = ALLKEY;
        else key = Filter;
    }
    else key = oldKey;

    // define widget
    QWidget* par = window();
    if( !par )
     par =  pVisor->window();

    switch( keyType )
    {
    case KEY_OLD:
        return vfKeyEdit(par, str.c_str(), nRT, key.c_str() );
    case KEY_NEW:
    case KEY_NEW_SAVEAS:
        return vfKeyTemplEdit(par, str.c_str(), nRT, key.c_str(), false );
    case KEY_TEMP:
         {
          string stt = vfKeyTemplEdit(par, str.c_str(), nRT, key.c_str() );
          if( !stt.empty())
            Filter = stt;
          return stt;
        }
    }
    Error( str, "Invalid record key editing mode");
    return "";
}

/// Tools for correcting IPN Math Scripts
bool TCModule::CheckEqText( const char *erscan, const char *msg )
{
    string msger;
    if( msg == 0 )
        msger = "E90MSTran: Error in translation of script:\n ";
    else
        msger = string(msg);
    msger += erscan;
    msger += "\n  Would you like to make corrections now? ";
    if( !vfQuestion( window(), GetName() , msger ) )
        return false;
    pVisorImp->OpenModule(window(), nRT, 0, true);
    return true;
}

//----------------------------------------------------------
//--- Manipulation of the current record
//----------------------------------------------------------

/// Save record structure to Data Base
void TCModule::RecSave( const char *key, bool onOld )
{
    int fnum = -1;
    bool yesToAll=false;

    if( onOld == false  )
        db->SaveRecordQuestion( key, fnum, yesToAll );
    else
        db->SaveRecord( key, fnum );
    // ?? if new
    pVisorImp->defineModuleKeysList( nRT );
    contentsChanged = false;

    if( pVisor->ProfileMode == true && !isCalcModules() && nRT != RT_SDATA )
      TMultiSystem::sm->pmp->pTPD = -1; // to reload thermodynamic data base
}

/// Read record into memory structure
void TCModule::RecInput( const char *key )
{
    db->Get( key );
    dyn_set();
    contentsChanged = false;
}

/// Load record from data base (show in project mode)
void TCModule::RecordLoadinProfile( const char *key )
{
    string str;

    if( key == 0 )
    {
        str = getFilter();
        str = GetKeyofRecord( str.c_str(), "Please, select a record key ", KEY_OLD );
    }
    else
       str=string(key);

    // get record
    if( str.empty() )
        return;
    RecInput( str.c_str() );
    check_input( str.c_str(), 0 );
}

/// Delete record with key
void TCModule::DeleteRecord( const char *key )
{
    db->Del( key );
}

/// Rebuild dialog for the record structure before calc
// To be converted into wizard form
// returns IDYES, IDNO or IDCANCEL
int TCModule::RecBuild( const char *key, int mode  )
{
    db->SetKey(key);
    int bldType = mode;
    if( bldType == VF_UNDEF )
       bldType = vfQuestion3(window(), "Reallocation of data arrays ",
                              GetName()+ string(" : ") + key ,
                              "&Bypass", "&Remake", "&Clear all");
    int retType = bldType;
    db->SetStatus(ONEF_);

    switch( bldType )
    {
      case VF3_3:    // =VF_CLEARALL
        retType = VF3_1;
        dyn_kill();
        set_def(); // set default data or zero if necessary
        if( nRT == RT_SYSEQ ) // no remake wizard if "Create"
         {
           contentsChanged = true;
           break;
         }
       case VF3_2:  // =VF_REMAKE
           MakeQuery();
           contentsChanged = true;
           break;
       case VF3_1:   // =VF_BYPASS   - do nothing
          retType = VF3_3;
          break;
    }
    return  retType;
}

/// Calculate record
void TCModule::RecCalc( const char* )
{
    contentsChanged = true;
}

/// Test record with key
void TCModule::TryRecInp( const char *_key, time_t& time_s, int q )
{
    string keyStr( _key, db->KeyLen() );
    if( !MessageToSave() )
       return;

    RecStatus iRet = db->Rtest( keyStr.c_str(), 1 );
    string msg;

    switch( iRet )
    {
    case MANY_:
            msg = "Record selection error! ";
            msg += keyStr;
            Error( GetName(), msg );
    case ONEF_:
            dyn_set(q);
            time_s = db->Rtime();
            return;
    case UNDF_:
    case NONE_:
    case EMPC_:
        {
            msg = "Database chain ";
            msg +=  GetName();
            msg += ": Data record not found, \n key  '";
            msg += keyStr;
            msg += "'.\n Maybe, database file is not linked to chain\n";
            if(pVisor->ProfileMode == true)
                Error( GetName(), msg );
            msg +=  "Create new record?";
            if( !vfQuestion(window(), GetName(), msg ))
                Error( GetName(), "Record creation rejected!");

            string str = keyStr;
            if( str.find_first_of("*?" ) != string::npos)  // pattern
                str = GetKeyofRecord( str.c_str(),
                       "Please, set a new record key ", KEY_NEW);
            if(  str.empty() )
                Error( GetName(), "Record creation rejected!");

            ErrorIf( db->Find( str.c_str() ),
                 GetName(), "A record with such key already exists!");
            db->SetKey( str.c_str() );
            pVisorImp->OpenModule(pVisorImp, nRT);

            check_input( str.c_str() );
            RecBuild( str.c_str() );
            SetString("Remake of the new record finished OK. "
                      " It is recommended to re-calculate the data");
            pVisor->Update();
            Error("Calculation failed!", "Check data fields and try calculation again!");
        } // break;
    case FAIL_:
        msg = "Failure!!! Database chain ";
        msg += GetName();
        msg += " is corrupt,\n"
               "Data Record key '";
        msg += keyStr;
        msg += "'\n Try to unload or re-index this database chain...";
        Error( GetName(),  msg );
    }
}

/// virtual callback for plotting the record
// should be redefined in subclasses (modules)
// to make some plotting
void TCModule::RecordPlot( const char* /*key*/ )
{
    vfMessage(window(), GetName(), "Plotting?\n\nNot here, yet... Sorry!");
}

/// Scan and print into format
void TCModule::PrintSDref( const char* sd_key, const char* text_fmt )
{
    // open file to output
    string filename="";
    if( vfChooseFileSave(window(), filename,
        "Please, provide name of TXT-file") == false )
      return;

    ios::openmode mod = ios::out;
    if( vfChooseFileOpenMode( window(), filename.c_str(), mod ))
    {
       fstream f( filename.c_str(), mod );
        ErrorIf( !f.good() , filename, "Fileopen error");
        // scan and print format
      TPrintData dat( sd_key, nRT, f, text_fmt );
    }
}

/// Printing the record
void TCModule::RecordPrint( const char* sdkey )
{
    // select  SDref key
    string sd_key;
    if( sdkey )
       sd_key=sdkey;
    else
    {
      // read sdref record with format prn
      sd_key = "pscript*:*:";
      sd_key += db->GetKeywd();
      sd_key += "*:";
    }
    if( sd_key.find_first_of("*?" ) != string::npos )
    {
       sd_key = (dynamic_cast<TCModule *>(aMod[RT_SDATA]))->GetKeyofRecord(
          sd_key.c_str(), "Please, select a print script", KEY_OLD );
    }
    if( sd_key.empty() )
      return;

    // Read sdref record
   ((TCModule *)aMod[RT_SDATA])->RecInput( sd_key.c_str() );
    const char * text_fmt = (char *)aObj[o_sdabstr].GetPtr();
    if( !text_fmt )
       Error( sd_key, "No print script in this record.");

    PrintSDref( sd_key.c_str(), text_fmt );
}

//----------------------------------------------------------
//--- Command with the current one record
//----------------------------------------------------------

// ???  Save record to DB file
void TCModule::CmSaveM()
{
  try{
       if(  db->GetStatus() == UNDF_  && db->RecCount() &&
              nRT != RT_SDATA  )
          Error( GetName(), "E1 record contents are not yet loaded!");
        CmSave();
     }
   catch( TError& xcpt )
    {
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

/// Add new record to DB ( remake current )
void TCModule::CmNew()
{
    try
    {
        if( pVisor->ProfileMode != true && isCalcModules() )
            Error( GetName(), "Please, do it in Project mode!");
        if( pVisor->ProfileMode == true &&
             ( nRT < RT_SYSEQ && nRT != RT_SDATA ) )
            Error( GetName(), "Please, do it in Database mode!");

       if( !MessageToSave() )
         return;

       string dlgName = "Please, set a new record key ";
       string str = db->PackKey();
       do
       {
          str = GetKeyofRecord( str.c_str(), dlgName.c_str(), KEY_NEW );
          if(  str.empty() ) // cancel command
             return;
          dlgName = "This record already exists! Please, enter another name.";
       }  while( db->Find( str.c_str() ) != false );

       db->SetKey( str.c_str() );
       str = db->UnpackKey();
       check_input( str.c_str(), 0 );
       RecBuild( str.c_str(), VF_REMAKE );
       SetString("Remake of the new record finished OK. "
                  " It is recommended to re-calculate the data.");
       pVisor->Update();
    }
    catch( TError& xcpt )
    {
        pVisor->Update( true );
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

/// Add new record to DB ( clear all )
void TCModule::CmCreate()
{
   try
    {
        if( pVisor->ProfileMode != true && isCalcModules() )
            Error( GetName(), "Please, do it in Project mode!");
        if( pVisor->ProfileMode == true &&
             ( nRT < RT_SYSEQ && nRT != RT_SDATA ) )
            Error( GetName(), "Please, do it in Database mode!");

        if( !MessageToSave() )
             return;

    string dlgName = "Please, set a new record key ";
    string str = db->PackKey();
    do
    {
       str = GetKeyofRecord( str.c_str(), dlgName.c_str(), KEY_NEW );
       if(  str.empty() ) // cancel command
          return;
       dlgName = "This record already exists! Please, enter another name.";
    }  while( db->Find( str.c_str() ) != false );

    db->SetKey( str.c_str() );
    str = db->UnpackKey();
    check_input( str.c_str(), 0 );
    RecBuild( str.c_str(), VF_CLEARALL );
    SetString("Remake of the new record finished OK. "
               " It is recommended to re-calculate the data.");
    pVisor->Update();

    }
    catch( TError& xcpt )
    {
        pVisor->Update( true );
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

/// Set new Filter and select first records from list
void TCModule::CmFilter()
{
    try
    {
        if( !MessageToSave() )
          return;

        string str = Filter;
        str = GetKeyofRecord( str.c_str(),
                         "Please, give a record key template", KEY_TEMP );
        if(  str.empty() )
            return;

       vector<string> aKey;
       db->GetKeyList( Filter.c_str(), aKey );
       if( aKey.size() > 0 )
       {
         if( pVisor->ProfileMode == true )
          RecordLoadinProfile( aKey[0].c_str() );
         else
          RecInput( aKey[0].c_str() );
       }
       pVisor->Update( true );
    }
    catch( TError& xcpt )
    {
        pVisor->Update( true );
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

/// Rebuild loading before record (error if current record undefined)
void TCModule::CmDerive()
{
  try
    {
        if( pVisor->ProfileMode != true && isCalcModules() )
            Error( GetName(), "Please, do it in Project mode!");
        if( pVisor->ProfileMode == true &&
             ( nRT < RT_SYSEQ && nRT != RT_SDATA ) )
            Error( GetName(), "Please, do it in Database mode!");

        if( !MessageToSave() )
            return;

        string str = db->UnpackKey();
        if( str.find_first_of("*?" ) != string::npos
           || ( db->GetStatus() == UNDF_ && nRT != RT_SDATA ) )
            Error( GetName(), "E2 Cannot save under record key template, or record contents are not yet loaded!");

        check_input( str.c_str(), 0 );
        RecBuild( str.c_str(), VF_REMAKE );
        SetString("Remake finished OK. "
                  " It is recommended to re-calculate the data.");
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        pVisor->Update( true );
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

/// Show another record from DB (Without check)
void TCModule::CmShow( const char *key )
{
   try
   {
        if( pVisor->ProfileMode == true && isCalcModules() )
            Error( GetName(), "Invalid command in Project mode!");
        if( !MessageToSave() )
            return;

        // get key of record
        string str;
        if( key == 0)
        { str = GetKeyofRecord( 0, "Select data record key ", KEY_OLD );
          if( str.empty() )
            return;
        }
        else  str = string(key);

        RecInput( str.c_str() );
        SetTitle();
        pVisor->Update( true );
    }
    catch( TError& xcpt )
    {
        pVisor->Update( true );
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

/// Show another record from DB in project mode ( With check)
void TCModule::CmLoadinProfile(const char *key)
{
    try
    {
        if( nRT < RT_SYSEQ && nRT != RT_SDATA)
        {
          CmShow(key);
          return;
        }

        if( !MessageToSave() )
           return;
        RecordLoadinProfile(key);
        pVisor->Update(true);
    }
    catch( TError& xcpt )
    {
        pVisor->Update( true );
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

/// Calc loading before record (error if current record undefined)
void TCModule::CmCalc()
{
    try
    {
        if( pVisor->ProfileMode != true && isCalcModules() )
            Error( GetName(), "Please, do it in Project mode!");

        string str = db->PackKey();
        if( str.find_first_of("*?" ) != string::npos
            || ( db->GetStatus()== UNDF_ && db->RecCount()
                 && nRT != RT_SDATA ))
            Error( GetName(), "E3 Cannot save under record key template, or record contents are not yet loaded!");

        check_input( str.c_str() );
        SetString("Calculation... ");
        clock_t t_start11, t_end11;
        t_start11 = clock();

        RecCalc( str.c_str() );

        t_end11 = clock();
        clock_t dtime = ( t_end11- t_start11 );

        string ttlstr = "Calculation finished OK (elapsed time: ";
        char sbuf[100];
        sprintf( sbuf, "%lg", (double)dtime/(double)CLOCKS_PER_SEC);
        ttlstr += sbuf;//to_string((double)dtime/(double)CLOCKS_PER_SEC);
        ttlstr += " s).";
        SetString(ttlstr.c_str());
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        pVisor->Update( true );
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

/// Save current record
void TCModule::CmSave()
{
    try
    {
        string str = db->PackKey();
        if( str.find_first_of("*?" ) != string::npos )
        {
            str = GetKeyofRecord( str.c_str(),
                     "Insert new record keyed ", KEY_NEW );
            if(  str.empty() )
                return ;
        }
        RecSave( str.c_str() );
    }
    catch( TError& xcpt )
    {
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

/// Save current record to DB file as new
void TCModule::CmSaveAs()
{
    try
    {
        if( pVisor->ProfileMode == true && ( nRT < RT_SYSEQ
            &&  nRT != RT_PARAM && nRT != RT_SDATA && nRT != RT_PHASE ) )
            Error( GetName(), "Please, do it in Database mode!");

        string dlgName = "Please, set a new record key ";
        string str=db->PackKey();
    AGN:
        str = GetKeyofRecord( str.c_str(), dlgName.c_str(), KEY_NEW_SAVEAS );
        if(  str.empty() )
             return;

        if( db->Find( str.c_str() ) ) // name of exist record
        {
          switch( vfQuestion3( window(), str.c_str(),
                "This record already exists! What to do?",
                "&Replace", "Re&name", "&Cancel") )
           {
            case VF3_2: goto AGN;
            case VF3_1: break;
            case VF3_3: return;
           }
       }

       RecSave( str.c_str()/*, false*/ );
       pVisor->Update( true );
    }
    catch( TError& xcpt )
    {
        pVisor->Update( true );
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

/// Delete current record
void TCModule::CmDelete()
{
    try
    {
        if( nRT == RT_PARAM )
            Error( GetName(), "This record cannot be deleted!");

        if( pVisor->ProfileMode == true && nRT < RT_SYSEQ )
            Error( GetName(), "Please, do it in Database mode!");

        string str=db->PackKey();
        //if( str.find_first_of("*?" ) != string::npos )
        //    Error( GetName(), "Current record key not defined!");
        if( !vfQuestion(window(), GetName(),
               "Confirm deletion of data record keyed "+str ))
            return;
        DeleteRecord( str.c_str() );
        contentsChanged = false;
    }
    catch( TError& xcpt )
    {
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

/// Callback for 'Plot' command
void TCModule::CmPlot()
{
    try
    {
        if( !MessageToSave() )
          return;

        string str=db->PackKey();
        if( str.find_first_of("*?" ) != string::npos
           || ( db->GetStatus()== UNDF_ && db->RecCount() &&
               nRT != RT_SDATA ))
            Error( GetName(), "E4 Cannot save under record key template, or record contents are not yet loaded!");
        RecordPlot( str.c_str() );
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

/// Callback for 'print' command
void TCModule::CmPrint()
{
    try
    {
        if( !MessageToSave() )
          return;

        RecordPrint();
        SetString("Printing of a record finished OK. ");
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

/// Open SDref window with script
void TCModule::CmScript()
{
    try
    {
      // read sdref record with format prn
      string sd_key = "?script*:*:";

      if( nRT < MD_RMULTS )
        sd_key += db->GetKeywd();
      else
      {
        sd_key += GetName();
      }
      sd_key += "*:";

      sd_key = (dynamic_cast<TCModule *>(aMod[RT_SDATA]))->GetKeyofRecord(
          sd_key.c_str(), "Please, select an appropriate script", KEY_OLD);
      if( sd_key.empty() )
         return;

      (dynamic_cast<TCModule *>(aMod[RT_SDATA]))->RecInput( sd_key.c_str() );
       pVisorImp->OpenModule( 0, RT_SDATA, 0, true );
      (dynamic_cast<TCModule *>(aMod[RT_SDATA]))->Update();
    }
    catch( TError& xcpt )
    {
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

//----------------------------------------------------------
//--- Manipulation with list of records
//----------------------------------------------------------

/// Delete the list of records
void TCModule::CmDeleteList()
{
    try
    {
        if( !MessageToSave() )
            return;

        DelList( Filter.c_str() );
        db->SetKey( ALLKEY );
    }
    catch( TError& xcpt )
    {
        pVisor->Update();
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

/// Copy list og records
void TCModule::CmCopyList( )
{
    try
    {
        if( !MessageToSave() )
            return;

        CopyRecordsList( Filter.c_str(), false );
        dyn_set();
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        pVisor->Update();
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

/// Rename list of records
void TCModule::CmRenameList( )
{
    try
    {
        if( !MessageToSave() )
            return;

        CopyRecordsList( Filter.c_str(), true );
        dyn_set();
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        pVisor->Update();
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

/// Transfer the list of records
void TCModule::CmTransferList()
{
    try
    {
        if( !MessageToSave() )
          return;

        Transfer( Filter.c_str() );
        dyn_set();
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        pVisor->Update();
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

/// Export list of keys to TXT file
void TCModule::CmKeysToTXT()
{
    try
    {
        if( !MessageToSave() )
            return;

        vector<string>  aKey = vfMultiKeys( window(),
               "Please, mark record keys to be listed in txt-file",
               nRT, Filter.c_str() );
        if( aKey.size() <1 )
                return;

        string s = GetName();
        string filename;
            s += " : Please, select file to write record keys";
        if( !vfChooseFileSave(window(), filename, s.c_str()) )
                return;
        ios::openmode mod = ios::out;
        if( !vfChooseFileOpenMode( window(), filename.c_str(), mod ))
                return;
        fstream f(filename.c_str(), mod );
        ErrorIf( !f.good() , GetName(), "Fileopen error");

        f << " " << GetName() << " \'" << Filter.c_str()
          << "\' Nrec= "  << aKey.size() << "\n";
        for(int i=0; i<aKey.size(); i++ )
                f << aKey[i].c_str() << "\n";

        ErrorIf( !f.good() , GetName(), "Writefile error");

        pVisor->Update(); // no objecs change, only title
    }
    catch( TError& xcpt )
    {
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

/// Export data to format text file
void TCModule::CmExport()
{
    try
    {
        if( !MessageToSave() )
           return;

        RecExport( Filter.c_str() );
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        pVisor->Update();
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}


/// Import data from format text file
void TCModule::CmImport()
{
    try
    {
        if( ! MessageToSave() )
        return;

        RecImport();
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        pVisor->Update();
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

/// Export Data Records to json txt-file
void TCModule::CmExportJson()
{
    try
    {
        if( !MessageToSave() )
           return;
        RecListToBsonJSON( Filter.c_str() );
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        pVisor->Update();
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

/// Import Data Records from json txt-file
void TCModule::CmImportJson()
{
    try
    {
        if( !MessageToSave() )
          return;
        RecListFromBsonJSON();
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        pVisor->Update();
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

/// Export Data Records to EJDB
void TCModule::CmBackupEJDB()
{
    try
    {
        if( !MessageToSave() )
           return;
        RecListToEJDB( Filter.c_str() );
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        pVisor->Update();
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

/// Loads Data Records from EJDB
void TCModule::CmRestoreEJDB()
{
    try
    {
        if( !MessageToSave() )
          return;
        RecListFromEJDB(Filter.c_str());
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        pVisor->Update();
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

// internal functions ----------------------------------------

/// delete list of records from Data Base
void TCModule::DelList( const char *pattern )
{
    vector<string> aKey = vfMultiKeys( window(),
       "Please, mark record keys to be deleted from database",
       nRT, pattern );
    int ichs = 1;

    for(int i=0; i<aKey.size(); i++ )
    {
        string str = "Please, confirm deleting record \nwith key: ";
        str += aKey[i];
        if( ichs )
        {
            switch( vfQuestion3(window(), GetName(), str.c_str(),
                                "&Yes", "&No", "&Delete All" ))
            {
            case VF3_3:
                ichs=0;
            case VF3_1:
                break;
            case VF3_2:
                continue;
            }
        }
        DeleteRecord( aKey[i].c_str() );
    }
}

/// transfer list of records in Data Base to another file
// (ever used???)  Has to be re-implemented, indeed !
void TCModule::Transfer( const char *pattern )
{
    int fnum = -1 ;// FileSelection dialog: implement "Ok to All"

    vector<string> aKey = vfMultiKeys( window(),
       "Please, mark record keys to be moved",
       nRT, pattern );

    for(int i=0; i<aKey.size(); i++ )
    {
        db->Get( aKey[i].c_str() );
        // !!!
        int oldfile = db->GetFnum();
        db->Del( aKey[i].c_str() );
        db->AddRecord( aKey[i].c_str(), fnum );
        if( fnum == -2 )
        { db->AddRecord( aKey[i].c_str(), oldfile );
          break;
        }
    }
}

/// copy list of records in Data Base to another file
void TCModule::CopyRecordsList( const char *pattern, bool if_rename )
{
    int fnum = -1 ;// FileSelection dialog: implement "Ok to All"
    int oldfnum = 0;
    string str;

    if( if_rename )
     str = "Please, mark record keys to be renamed";
    else
     str = "Please, mark record keys to be copied";

    vector<string> aKey = vfMultiKeys( window(),
       str.c_str(), nRT, pattern );

    // define rename procedure
    int rn_type = 0;
    if( aKey.size() < 2)
      rn_type = 1;
    else
     {  switch (vfQuestYesNoCancel(  window(),
        "How to rename records",
        "Each key separately (Yes) \n"
        "All records using template (No)" ))
       {
        case VF_YES:
            rn_type = 1;
        case VF_NO:
            break;
        case VF_CANCEL:
        return;
       }
    }
    int fld = db->KeyNumFlds()-1;
    string from_t;
    string to_t;
    if( rn_type == 0 )
    {
     if( !vfKeyCanged(window(), "", from_t,  to_t, db->FldLen( fld ) ))
      return;
    }

    for(int i=0; i<aKey.size(); i++ )
    {
        db->Get( aKey[i].c_str() );
        /// !!! changing record key
        if( rn_type ==  1)
        {
          str= aKey[i];
          str = GetKeyofRecord( str.c_str(),
                 "Insert new record key", KEY_NEW_SAVEAS );
          if(  str.empty() )
            return ;
        }
        else
        {
          str = db->FldKey( fld );
          ChangeforTempl( str, from_t, to_t, db->FldLen( fld ));
          db->SetFldKey( fld, str.c_str() );
          str = db->PackKey();
        }

       if( if_rename )
       {
         oldfnum = db->GetFnum();
         db->Del( aKey[i].c_str() );
       }

       //Point SaveRecord
       try {
               db->AddRecord( str.c_str(), fnum );
               if( fnum == -2 )
                  break;
              }
       catch( TError& xcpt )
       {
           if( if_rename )
               db->AddRecord( aKey[i].c_str(), oldfnum );
           break;
           // may be message to continue next record
       }
    }
}

/// Unloads Data Record to json txt format
void TCModule::RecListToBsonJSON( const char *pattern )
{
    vector<string> aKey = vfMultiKeys( window(),
       "Please, mark records to be unloaded to JSON",
       nRT, pattern );
    if( aKey.size() <1 )
        return;

    string s = GetName();
    string filename;
    s += " : Please, give a file name for unloading records";
    if( vfChooseFileSave( window(), filename, s.c_str(), "*.json" ) == false )
        return;

    FILE* logfile = NULL;
    logfile = fopen( filename.c_str(), "w+" );
    ErrorIf( !logfile , filename, "File write error");

    fprintf(logfile, "[\n");
    for(int i=0; i<aKey.size(); i++ )
    {
        db->Get( aKey[i].c_str() );
        bson obj;
        dbEJ->SetKey(aKey[i].c_str());
        dbEJ->RecToBson( &obj, db->Rtime() );
        print_bson_object_to_json( logfile, &obj );
        bson_destroy(&obj);
        if( i<aKey.size()-1)
            fprintf(logfile, ",");
         fprintf(logfile, "\n");
    }
    fprintf(logfile, "]");

    fclose( logfile );
    dyn_set();
}

/// Loads Data Record from json txt format
void TCModule::RecListFromBsonJSON()
{
    int fnum= -1 ;// FileSelection dialog: implement "Ok to All"
    bool yesToAll=false;

    // Choose file name
    string s =string( GetName() )+" : Please, select file with unloaded records";
    string filename;
    if( vfChooseFileOpen( window(), filename, s.c_str(), "*.json" ) == false )
        return;
    fstream f(filename.c_str(), ios::in);
    ErrorIf( !f.good() , filename, "Fileread error...");

    //filename +=".out";
    //FILE* logfile = NULL;
    //logfile = fopen( filename.c_str(), "w+" );
    //ErrorIf( !logfile , filename, "File write error");

    // read bson records array from file
    ParserJson parserJson;
    char b;
    string objStr;
    // int ii=0;         //test

    // bson_fprintf(logfile, "[");  //test
    while( !f.eof() )
    {
      f.get(b);
      if( b == jsBeginObject )
      {
        //if(ii)                        //test
        //  bson_fprintf(logfile, ","); //test
        //bson_fprintf(logfile, "\n");  //test
        //ii++;                         //test

        objStr =  parserJson.readObjectText(f);
        bson obj;
        bson_init(&obj);
        parserJson.parseObject( &obj );
        bson_finish(&obj);
        string key = dbEJ->RecFromBson( &obj );
        //print_bson_object_to_json( logfile, &obj ); //test
        bson_destroy(&obj);
        db->SaveRecordQuestion( key.c_str(), fnum, yesToAll );
        if( fnum == -2 )
           break;
      }
    }
    //bson_fprintf(logfile, "\n]");   //test

    //fclose( logfile );   //test
    dyn_set();
}

/// Unloads Data Record to EJDB
void TCModule::RecListToEJDB( const char *pattern )
{
    int fnum = -1;
    bool yesToAll=false;

    // Reopen files on oldDB
    vector<int> arrO = SelectFileList(closef|openf|oldself, 2 );
    if( arrO.size() < 1 )
        if( !vfQuestion( window(), GetName(),
              "No Old database files selected to open! Continue?" ))
              return;

    dbOld->Close();
    dbOld->Open( arrO );
    dbOld->SetKey( ALLKEY );

#ifdef oldDBusing
    vector<string> aKey = vfMultiKeys( window(),
       "Please, mark records to be unloaded to EJDB",
       nRT, pattern );
    if( aKey.size() <1 )
        return;
#else
    // Select keys to load
    vector<string> aKeys;
    dbOld->GetKeyList( ALLKEY, aKeys, true );
    //if( aKeys.size() <= 0 )
    //  return;
    vector<int> aSel = vfMultiChoice(window(), aKeys,
        "Selection of records");
#endif

    // Reopen files on EJDB
    vector<int> arrEJ = SelectFileList(closef|openf|oldself, 1 );
    if( arrEJ.size() < 1 )
        if( !vfQuestion( window(), GetName(),
              "No EJDB database files selected to open! Continue?" ))
            return;
    dbEJ->Close();
    dbEJ->Open(arrEJ );
    dbEJ->SetKey( ALLKEY );


#ifdef oldDBusing
    // Save records
    for(int i=0; i<aKey.size(); i++ )
    {
        dbOld->Get( aKey[i].c_str() );
        stringToUtf8( aKey[i] );
        dbEJ->SaveRecordQuestion( aKey[i].c_str(), fnum, yesToAll );
        if( fnum == -2 )
           break;
    }
#else
    // Save records
    for(int i=0; i<aSel.size(); i++ )
    {
        dbOld->Get( aKeys[aSel[i]].c_str() );
        stringToUtf8( aKeys[aSel[i]] );
        dbEJ->SaveRecordQuestion( aKeys[aSel[i]].c_str(), fnum, yesToAll );
        if( fnum == -2 )
           break;
    }
#endif

    dyn_set();
}

/// Copy all of records in Data Base to EJDataBase Project
/// If fromOld true, from old Project file
///            else, from EJDB project template
/// fnum - selected file to save
/// from_t, to_t - template to change record key field fld
void TCModule::CopyAllToEJDB( bool fromOld, int& fnum,
      int fld, const string& from_t,  const string& to_t, const char *pattern )
{
    string skey;
    bool yesToAll=false;

    // select all records
    vector<string> aKey;
    if(fromOld)
      dbOld->GetKeyList( pattern, aKey );
    else
       dbEJ->GetKeyList( pattern, aKey );

    // Save records
    for(int i=0; i<aKey.size(); i++ )
    {
        skey = aKey[i];
        if(fromOld)
        {  dbOld->Get( skey.c_str() );
           if( fld >=0 )
           { skey = dbOld->FldKey( fld );
             ChangeforTempl( skey, from_t, to_t, dbOld->FldLen( fld ));
             dbOld->SetFldKey( fld, skey.c_str() );
             skey = dbOld->PackKey();
           }
         } else
          {  dbEJ->Get( skey.c_str() );
             skey = dbEJ->FldKey( fld );
             ChangeforTempl( skey, from_t, to_t, dbEJ->FldLen( fld ));
             dbEJ->SetFldKey( fld, skey.c_str() );
             skey = dbEJ->PackKey();
           }
        dbEJ->SaveRecordQuestion( skey.c_str(), fnum, yesToAll );
        if( fnum == -2 )
           break;
    }
    dyn_set();
}

/// Loads Data Record from EJDB
void TCModule::RecListFromEJDB( const char *pattern )
{
    int fnum = -1;
    bool yesToAll=false;

    // Reopen files on EJDB
    vector<int> arrEJ = SelectFileList(closef|openf|oldself, 1 );
    if( arrEJ.size() < 1 )
        if( !vfQuestion( window(), GetName(),
              "No EJDB database files selected to open! Continue?" ))
              return;

    dbEJ->Close();
    dbEJ->Open( arrEJ );
    dbEJ->SetKey( ALLKEY );

#ifdef oldDBusing

    // Select keys to load
    vector<string> aKeys;
    dbEJ->GetKeyList( ALLKEY, aKeys, true );
    //if( aKeys.size() <= 0 )
    //  return;
    vector<int> aSel = vfMultiChoice(window(), aKeys,
         "Selection of records");
#else
    vector<string> aKey = vfMultiKeys( window(),
       "Please, mark records to be unloaded to Old Database",
       nRT, pattern );
    if( aKey.size() <1 )
        return;
#endif

    // Reopen files on Old DB
    vector<int> arrO = SelectFileList(closef|openf|oldself, 2 );
    if( arrO.size() < 1 )
        if( !vfQuestion( window(), GetName(),
              "No old database files selected to open! Continue?" ))
            return;
    dbOld->Close();
    dbOld->Open(arrO );
    dbOld->SetKey( ALLKEY );

#ifdef oldDBusing
    // Save records
    for(int i=0; i<aSel.size(); i++ )
    {
        dbEJ->Get( aKeys[aSel[i]].c_str() );
        dbOld->SaveRecordQuestion( aKeys[aSel[i]].c_str(), fnum, yesToAll );
        if( fnum == -2 )
           break;
    }
#else
    // Save records
    for(int i=0; i<aKey.size(); i++ )
    {
        dbEJ->Get( aKey[i].c_str() );
        dbOld->SaveRecordQuestion( aKey[i].c_str(), fnum, yesToAll );
        if( fnum == -2 )
           break;
    }
#endif

    dyn_set();
}

/// Unloads Data Record to user format
void TCModule::RecExport( const char *pattern )
{
    // read sdref record with format prn
    string sd_key = "escript*:*:";
           sd_key += db->GetKeywd();
           sd_key += "*:";
    sd_key = (dynamic_cast<TCModule *>(aMod[RT_SDATA]))->GetKeyofRecord(
          sd_key.c_str(), "Select key of escript format", KEY_OLD);
    if( sd_key.empty() )
       return;
    (dynamic_cast<TCModule *>(aMod[RT_SDATA]))->RecInput( sd_key.c_str() );
    char * text_fmt = (char *)aObj[o_sdabstr].GetPtr();
    if( !text_fmt )
       Error( sd_key, "No format text in this record.");

    vector<string> aKey = vfMultiKeys( window(),
       "Please, mark records to be unloaded into txt-file",
       nRT, pattern );
    if( aKey.size() <1 )
        return;

   string s = GetName();
   string filename;
       s += " : Please, give a file name for unloading records";
   if( !vfChooseFileSave(window(), filename, s.c_str()) )
           return;
   ios::openmode mod = ios::out;
   if( !vfChooseFileOpenMode( window(), filename.c_str(), mod ))
           return;
   fstream f(filename.c_str(), mod);
   ErrorIf( !f.good() , GetName(), "File write error");

   for(int i=0; i<aKey.size(); i++ )
    {
       db->Get( aKey[i].c_str() );
       TPrintData dat( sd_key.c_str(), nRT, f, text_fmt );
    }

    ErrorIf( !f.good() , GetName(), "Filewrite error");
    dyn_set();
}

/// Loads Data Records from user format
void TCModule::RecImport()
{
    int fnum= -1 ;// FileSelection dialog: implement "Ok to All"
    bool yesToAll=false;
    char ch;

    // read sdref record with format read
    string sd_key = "iscript*:*:";
           sd_key += db->GetKeywd();
           sd_key += "*:";
    sd_key = (dynamic_cast<TCModule *>(aMod[RT_SDATA]))->GetKeyofRecord(
          sd_key.c_str(), "Select key of iscript format", KEY_OLD);
    if( sd_key.empty() )
     return;
    (dynamic_cast<TCModule *>(aMod[RT_SDATA]))->RecInput( sd_key.c_str() );
    char * text_fmt = (char *)aObj[o_sdabstr].GetPtr();
    if( !text_fmt )
       Error( sd_key, "No format text in this record.");

    // translate scripts
    TReadData dat( sd_key.c_str(), nRT, text_fmt );

    string s =  GetName();
           s += " : Please, select file with imported records";
    string filename;
    if( vfChooseFileOpen( window(), filename, s.c_str(), "*.fmt" ) == false )
        return;
    fstream f(filename.c_str(), ios::in);
    ErrorIf( !f.good() , GetName(), "Fileread error...");
    string keyp;
    int iter = 0;
    while( !f.eof() )
    {
        dat.readRecord( iter, f );
        keyp = db->UnpackKey();
        db->SaveRecordQuestion( keyp.c_str(), fnum, yesToAll );
        if( fnum == -2)
          break;
        do
         {
            f.get(ch);
            if( !f.good() )
                break;
         } while( ch == ' ' || ch =='\n' );
        if( f.eof() )
            break;
        f.putback( ch );
        iter++;
    }
    if( f.bad() )
    {
      string str = "File read error! \n";
             str += "Last good record :";
             str += keyp;
      Error( GetName(), str );
     }
     dyn_set();
}

//----------------------------------------------------------
//--- Manipulation with database files   (Service functions )
// must be changed foe EJDB
//----------------------------------------------------------

/// Index files of data base
/// Only for old DataBase format
void TCModule::CmRebildFile()
{
    try
    {
        if( !MessageToSave() )
           return;

        pVisor->Message( window(), GetName(), "Compressing database file(s). "
         "Please, wait...", 0, 100 );
        dbOld->RebildFile(SelectFileList(closef|openf, 2));
        dyn_set();
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        pVisor->Update();
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

/// Rename files from opend files list of data base
/// Only for old DataBase format
void TCModule::CmAddFileToList()
{
    try
    {
        if( !MessageToSave() )
          return;

        string filename = dbOld->GetKeywd();
               filename += ".newname.";
               filename +=  PDB_EXT;
        string filter = "*.";
               filter +=  PDB_EXT;

        if( vfChooseFileSave(window(), filename,
       "Enter a new database file name", filter.c_str() ) == false )
            return;
        // test Path -- pdb extension, name must started db->GetKeywd()
        string dir;
        string name;
        string ext;
        u_splitpath( filename, dir, name, ext);
        ext = PDB_EXT;
        size_t pos = name.find( dbOld->GetKeywd() );
        if( pos != 0 )
        {  name = "." + name;
           name = dbOld->GetKeywd()+ name;
        }
        filename = u_makepath( dir, name, ext );

        TDBFile* file = new TDBFile(filename.c_str());
        /////////
        dbOld->AddFileToList(file);
        db->SetKey( ALLKEY );
        pVisor->Update(); // no objecs change, only title
    }
    catch( TError& xcpt )
    {
        pVisor->Update();
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

/// Open new files from the list of closed files in the data base
/// Only for old DataBase format
void TCModule::CmAddOpenFile()
{
    try
    {
        if( !MessageToSave() )
           return;

        dbOld->AddOpenFile(SelectFileList(closef, 2));
        db->SetKey( ALLKEY );
        dyn_set();
        pVisor->Update(); // no objecs change, only title
    }
    catch( TError& xcpt )
    {
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

/// Rebuild the list of opened files for the data base
/// Only for old DataBase format
void TCModule::CmReOpenFileList()
{
    try
    {
        if( !MessageToSave() )
          return;

        vector<int> arr = SelectFileList(closef|openf|oldself, 2);

        if( arr.size() < 1 )
            if( !vfQuestion( window(), GetName(),
                  "No database files selected to open! Continue?" ))
                return;

        dbOld->Close();
        dbOld->Open( arr );
        db->SetKey( ALLKEY );
        dyn_set();
        pVisor->Update(); // no objecs change, only title
    }
    catch( TError& xcpt )
    {
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

vector<int> TCModule::SelectFileList(int mode, int dbType )
{
    vector<string> names;
    vector<int> indx;
    vector<int> sel;
    string title = "Selection of DB files";

    if( dbType == 0)
      db->GetFileList(mode, names, indx, sel);
    else
        if( dbType == 1)
        {
          dbEJ->GetFileList(mode, names, indx, sel);
          title = "Selection of EJDB files";
        }
         else {
            dbOld->GetFileList(mode, names, indx, sel);
            title = "Selection of old-format DB files";
        }

    vector<int> aSel = vfMultiChoiceSet(window(), names,
         title.c_str(), sel);

    vector<int> arr;
    for( int i=0; i<aSel.size(); i++ )
        arr.push_back( indx[aSel[i]] );

    return arr;
}

/// Make new EJDataBase
/// Only for EJ DataBase format
void TCModule::CmAddEJDBToList()
{
    try
    {
        if( !MessageToSave() )
          return;

        string filename = pVisor->sysDBDir()+"/NewEJDBname";
        string filter = "*";

        if( vfChooseFileSave(window(), filename,
       "Enter a new EJ DataBase name", filter.c_str() ) == false )
            return;

        // added new or link existing EJ DataBase
        flsEJ.addNew( filename );
        db->SetKey( ALLKEY );
        pVisor->Update(); // no objecs change, only title
    }
    catch( TError& xcpt )
    {
        pVisor->Update();
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

/// Open new database from the list of closed databases for the module
/// Only for EJ DataBase format
void TCModule::CmAddOpenEJDB()
{
    try
    {
        if( !MessageToSave() )
           return;

        dbEJ->AddOpenFile(SelectFileList(closef, 1));
        db->SetKey( ALLKEY );
        dyn_set();
        pVisor->Update(); // no objecs change, only title
    }
    catch( TError& xcpt )
    {
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

/// Rebuild the list of opened databases for the module
/// Only for EJ DataBase format
void TCModule::CmReOpenEJDB()
{
    try
    {
        if( !MessageToSave() )
          return;

        vector<int> arr = SelectFileList(closef|openf|oldself, 1);

        if( arr.size() < 1 )
            if( !vfQuestion( window(), GetName(),
                  "No database files selected to open! Continue?" ))
                return;

        dbEJ->Close();
        dbEJ->Open( arr );
        db->SetKey( ALLKEY );
        dyn_set();
        pVisor->Update(); // no objecs change, only title
    }
    catch( TError& xcpt )
    {
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

//========================================================
// descructor for TModList for cleaning up Modules from memory

TModuleList::~TModuleList()
{
    for( int ii=0; ii<size(); ii++ )
        delete at(ii);
}

// public list of modules
TModuleList aMod;

//--------------------- End of v_2module.cpp ---------------------------


