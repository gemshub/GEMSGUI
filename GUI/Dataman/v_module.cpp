//-------------------------------------------------------------------
// $Id: v_module.cpp 1391 2009-08-10 12:58:16Z gems $
//
// Implementation of TSubModule, TModule and TModList classes
//
// Copyright (C) 1996-2001 A.Rysin, S.Dmytriyeva
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

#include <ctime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include "v_module.h"
#include "visor.h"
#include "service.h"
#include "v_mod.h"
#include "t_print.h"
#include "t_read.h"
#include "NewSystemDialog.h"
#include "m_param.h"
#include <spdlog/sinks/stdout_color_sinks.h>

// Thread-safe logger to stdout with colors
std::shared_ptr<spdlog::logger> gui_logger = spdlog::stdout_color_mt("gems3gui");


// Default constructor and destructor

const std::string replace_question = "Data record with this key already exists! Replace?";

TSubModule::TSubModule( uint nrt ):
        nRT( nrt ),
        contentsChanged(false),
        //   sBw(600), sBh(460),
        pImp(nullptr)
{
    startKeyEdit = 0;
    iconFileName = "";
}


TSubModule::~TSubModule()
{}

void TSubModule::clearEditFocus()
{
    if( window() )  {
        auto* focus_w = window()->focusWidget();
        TCellInput* cell_w = dynamic_cast<TCellInput*>(focus_w);
        if( cell_w ) {
            cell_w->clearFocus();
        }
        TCellText* cell_t = dynamic_cast<TCellText*>(focus_w);
        if( cell_t ) {
            cell_t->clearFocus();
        }
    }
}

uint TSubModule::keyEditField()
{
    if( nRT == RT_RTPARM || pVisor->ProfileMode )
        return startKeyEdit;
    else return 0;
}

// (supposed to be :) Called from TModuleImp right when
// window is created

void
TSubModule::Setup()
{
    ///  SetCaption( (char*)GetName() );
}


// Called from TModuleImp right before
// window is closed

bool
TSubModule::EvClose()
{
    return true;
}


QWidget* TSubModule::window()
{
      if( nRT== RT_SYSEQ && pVisor->ProfileMode )
       return dynamic_cast<QWidget*>(NewSystemDialog::pDia);
      else
       return dynamic_cast<QWidget*>(pImp);
}

// Callback for 'close' command

void
TSubModule::CloseWin()
{
    pImp->CloseWin();
}

// Updates window contents
// see also TModuleImp::Update, TCWindow::Update(),
// TCPage::Update(), TField::Update()

void
TSubModule::Update(bool force)
{
    if( !pImp )		// module not opened
        return;
    pImp->Update(force);
}


// Updates contents of all windows plus caption of the current one

void
TSubModule::ModUpdate( const char *str )
{
    SetString(str);
    pVisor->Update();
}

// Open module window

void
TSubModule::Show(QWidget* parent,  const char *str, bool viewmode )
{
    if (str) SetString(str);
    pVisorImp->OpenModule(parent, nRT, 0, !viewmode);
}

// Callback for 'Help' command

const char* TSubModule::GetHtml()
{
   return GEMS_TOC_HTML;

}

void TSubModule::CmHelp()
{
    if(pVisor->ProfileMode && nRT==RT_PARAM)
         pVisorImp->OpenHelp( GetHtml(), NUMSET );
    else
         pVisorImp->OpenHelp( GetHtml() );
}

void TSubModule::CmHelp2()
{
   pVisorImp->OpenHelp( GetHtml(), MWPAGE, pImp->curPage()+1 );
}


//========================================
// TCModule class
//=======================================

// Default constructor

TCModule::TCModule( uint nrt ):
        TSubModule( nrt ),
        db(rt[nrt].get()),
        nQ(1), Filter(ALLKEY), start_title(" ")
{  }
// start_title = " Access to database record without remake/recalculation"


TCModule::~TCModule()
{}



void
TCModule::Setup()
{
    ///  SetCaption( (char*)GetName() );
}

/*! returns true if we can close the window
*/
bool
TCModule::EvClose()
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


/* opens window with 'Remake record' parameters
*/
void
TCModule::MakeQuery()
{
    pImp->MakeQuery();
}


/*! returns true if user pressed 'save' or 'discard' and false on 'cancel'
*/
bool
TCModule::MessageToSave()
{
    clearEditFocus();

    //--if( nRT != RT_SDATA &&
    //-- (pVisor->ProfileMode == true && nRT < RT_SYSEQ) )
    //--  return true;

    string key_str = db->PackKey();
    if( contentsChanged && key_str.find_first_of("*?") == string::npos )
//        && ( db->GetStatus()!= UNDF_ ) )   // 09/11/2004 Sveta
    {
        int res = vfQuestion3(window(), key_str,
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

// get key of record (existing key, new key or key temlate )
string
TCModule::GetKeyofRecord( const char *oldKey, const char *strTitle,
                          int keyType )
{
    string str = GetName();
    str += ": ";
    str += strTitle;

    string key;

    if( oldKey == nullptr )
    {
        if(Filter.empty())
            key = ALLKEY;
        else key = Filter;
    }
    else key = oldKey;

    QWidget* par = window();
    if(!par)
     par =   pVisor->window();

    switch( keyType )
    {
    case KEY_OLD:
        return vfKeyEdit(par/*window()*/, str.c_str(), nRT, key.c_str() );
    case KEY_NEW:
    case KEY_NEW_SAVEAS:
        return vfKeyTemplEdit(par/*window()*/, str.c_str(), nRT, key.c_str(), false );
    case KEY_TEMP:
         {
          string stt = vfKeyTemplEdit(par/*window()*/, str.c_str(), nRT, key.c_str() );
          if( !stt.empty())
            Filter = stt;
          return Filter;
        }
    }
    Error( str, "Invalid record key editing mode");
    return "";
}

// make all keys filter
string  TCModule::makeKeyFilter()
{
    string strfilt;
    if( pVisor->ProfileMode &&
         ( RT_PARAM==nRT || RT_SYSEQ==nRT || RT_PROCES==nRT ||
           RT_GTDEMO==nRT || RT_UNSPACE==nRT || RT_DUALTH==nRT || RT_GEM2MT==nRT ) )
    {
      strfilt = char_array_to_string( rt[RT_PARAM]->FldKey(0), rt[RT_PARAM]->FldLen(0) );
      StripLine(strfilt);
      strfilt += ":";
    }
    else
       strfilt = "*:";

    for(int jj=1; jj<db->KeyNumFlds(); jj++)
    {
        strfilt += "*:";
    }
    return strfilt;
}

// return true if Filter must be changed or setted
bool  TCModule::testKeyFilter()
{
  if( Filter.empty() || Filter== ALLKEY)
      return true;
  if( pVisor->ProfileMode &&
       ( RT_PARAM==nRT || RT_SYSEQ==nRT || RT_PROCES==nRT ||
         RT_GTDEMO==nRT || RT_UNSPACE==nRT || RT_DUALTH==nRT || RT_GEM2MT==nRT ) )
  {
    string strfilt = char_array_to_string( rt[RT_PARAM]->FldKey(0), rt[RT_PARAM]->FldLen(0) );
    StripLine(strfilt);
    strfilt += ":";
    if(Filter.rfind(strfilt, 0) != 0)
     return true;
  }
  return false;
}


// Tools for correcting IPN Math Scripts
bool
TCModule::CheckEqText( const char *erscan, const char *msg )
{
    std::string msger;
    if( msg==nullptr )
        msger = "E90MSTran: Error in translation of script:\n ";
    else
        msger = std::string(msg);
    msger += erscan;
    msger += "\n  Would you like to make corrections now? ";
    if( !vfQuestion(window(), GetName() , msger ) )
        return false;
    pVisorImp->OpenModule(window(), nRT,0,true);

    return true;
    // contentsChanged = true; "Replace record in database files?"
}

//----------------------------------------------------------
//--- Manipulation of the current record
//----------------------------------------------------------


// Save record structure

void
TCModule::RecSave( const char *key, bool onOld )
{
    int  Rnum = db->Find( key );
    if( Rnum<0 )
    {
       AddRecord( key );
       pVisorImp->defineModuleKeysList( nRT );
    }
    else
        if( onOld == true || vfQuestion(window(), key, replace_question) )
            db->Rep( Rnum );
    contentsChanged = false;

    if( pVisor->ProfileMode && ( nRT < RT_SYSEQ &&  nRT != RT_PARAM && nRT != RT_SDATA ) )
      TMulti::sm->GetPM()->pTPD = -1; // to reload thermodynamic data base
}

// Save record to DB file
void TCModule::CmSaveM()
{
  try{
       string str=db->PackKey();
       if( str.find_first_of("*?" ) != string::npos
           || ( db->GetStatus() == UNDF_  && db->RecCount() &&  // 09/11/2004 Sveta
           nRT != RT_SDATA ) )   // oct 2005  Sveta
             Error( GetName(), "E1 Cannot save under record key template, or record contents are not yet loaded!");
       CmSave();
     }
   catch( TError& xcpt )
    {
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

void
TCModule::CmSave()
{
    try
    {
        clearEditFocus();
        //--if(  pVisor->ProfileMode == true &&
           //--( nRT < RT_SYSEQ &&  nRT != RT_PARAM && nRT != RT_SDATA  ))
           //--Error( GetName(), "Please, do it in Database mode!");

        string str=db->PackKey();
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


// Save record to DB file as new

void
TCModule::CmSaveAs()
{
    try
    {
        clearEditFocus();
        if( pVisor->ProfileMode &&
           ( nRT < RT_SYSEQ &&  nRT != RT_PARAM && nRT != RT_SDATA && nRT != RT_PHASE) )
            Error( GetName(), "Please, do it in Database mode!");

        string str=db->PackKey();
        int  Rnum;

    AGN:
        str = GetKeyofRecord( str.c_str(),"Insert new record key ", KEY_NEW_SAVEAS );
        if(  str.empty() )
             return;

        Rnum = db->Find( str.c_str() );
        if( Rnum>=0 ) // name of exist record
        {

           switch( vfQuestion3( window(), str,
                "This record already exists! What to do?",
                "&Replace", "Re&name", "&Cancel") )
           {
            case VF3_2: goto AGN;
            case VF3_1: break;
            case VF3_3: return;
           }
       }

        RecSave( str.c_str(), false );
        pVisor->Update( true );
    }
    catch( TError& xcpt )
    {
        pVisor->Update( true );
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}


// Delete record with key

void
TCModule::DeleteRecord( const char *key, bool errifNo )
{
    int  Rnum = db->Find( key );
    if( Rnum < 0 )
    {
        if( !errifNo )
            return;
        string str = " Record ";
        str += key;
        str += "\n not found to delete!";
        Error( GetName(), str );
    }
    db->Del( Rnum );
}


// Delete current record

void
TCModule::CmDelete()
{
    try
    {
        if( nRT == RT_PARAM )
            Error( GetName(), "This record cannot be deleted!");

        if( pVisor->ProfileMode && nRT < RT_SYSEQ )
            Error( GetName(), "Please, do it in Database mode!");

        std::string str=db->PackKey();
        if( str.find_first_of("*?" ) != std::string::npos )
            Error( GetName(), "Current record key not defined!");
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

// Read record into memory structure

void
TCModule::RecInput( const char *key )
{
    int Rnum = db->Find( key );
    if( Rnum<0 )
    {
        string msg = "Record ";
        msg += string(key, 0, db->KeyLen());
        msg += " not found!" ;
        Error( GetName(), msg );
    }
    db->Get( Rnum );
    dyn_set();
    contentsChanged = false;
    //  pVisor->Update();
}

// Show another record from DB (Without check)
void
TCModule::CmShow( const char *key )
{
    try
    {
        if( pVisor->ProfileMode &&
                ( nRT >= RT_SYSEQ || nRT == RT_PARAM )  )
            Error( GetName(), "Invalid command in Project mode!");

        if( !MessageToSave() )
	    return;

    	// get key of record
        string str;
        if( key == nullptr )
        { str = GetKeyofRecord(
          /*db->PackKey()*/nullptr, "Select data record key ", KEY_OLD );
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


// Set new Filter and select first records from list (Sveta 12/06/01)
void
TCModule::CmFilter()
{
    try
    {
        if( ! MessageToSave() )
	    return;

        string str = Filter;
        str = GetKeyofRecord( str.c_str(),
                         "Please, give a record key template", KEY_TEMP );
        if(  str.empty() )
            return;

       TCStringArray aKey;
       TCIntArray anR;
       auto Nrec = db->GetKeyList( Filter.c_str(), aKey, anR );
       if( Nrec >= 1 )
       {
        if( pVisor->ProfileMode )
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

// Show next record from template list (Sveta 14/06/01)
void
TCModule::CmNext()
{
    try
    {
       size_t i_next = 0;
       if( ! MessageToSave() )
	    return;

       // get current record key
       string str=db->UnpackKey();
       // select scroll list
       TCStringArray aKey;
       TCIntArray anR;
       auto Nrec = db->GetKeyList( Filter.c_str(), aKey, anR );
       if( Nrec <= 0 )
           return; // no records to scroll
       // get current record key
       if( !(str.find_first_of("*?" ) != string::npos) )
          //Current record key is defined!
       {
         for(size_t i=0; i<aKey.size(); i++ )
          if( str == aKey[i])
            {
              i_next = i+1;
              if( i_next >=  Nrec ) i_next--;
              break;
            }
        }
       if( pVisor->ProfileMode )
          RecordLoadinProfile( aKey[i_next].c_str() );
       else
          RecInput( aKey[i_next].c_str() );
       pVisor->Update( true );
    }
    catch( TError& xcpt )
    {
        pVisor->Update( true );
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

// Show previous record from template list (Sveta 14/06/01)
void
TCModule::CmPrevious()
{
    try
    {
       size_t i_next = 0;
       if( ! MessageToSave() )
    	return;

       // get current record key
       string str=db->UnpackKey();
       // select scroll list
       TCStringArray aKey;
       TCIntArray anR;
       auto Nrec = db->GetKeyList( Filter.c_str(), aKey, anR );
       if( Nrec <= 0 )
           return; // no records to scroll
       if( !(str.find_first_of("*?" ) != string::npos) )
          //Current record key is defined!
       {
         for(size_t i=0; i<aKey.size(); i++ )
          if( str == aKey[i])
            {
              if( i==0 )
                i_next = i;
              else
               i_next = i-1;
              break;
            }
        }
       if( pVisor->ProfileMode )
          RecordLoadinProfile( aKey[i_next].c_str() );
       else
          RecInput( aKey[i_next].c_str() );
       pVisor->Update( true );
    }
    catch( TError& xcpt )
    {
        pVisor->Update( true );
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}



// Rebuild dialog for the record structure before calc
// To be converted into wizard form
int
TCModule::RecBuild( const char *key, int mode  )
{
    // returns IDYES, IDNO or IDCANCEL

    int bldType = mode;
    if( bldType == VF_UNDEF )
       bldType = vfQuestion3(window(), "Reallocation of data arrays ",
                              GetName()+ std::string(" : ") + key ,
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
        [[fallthrough]];
    case VF3_2:  // =VF_REMAKE
        //if( !pImp )
        //{    Show(pVisorImp, "Remaking...");

        //}
//        pImp->MakeQuery();
        MakeQuery();

       //if( pVisor->ProfileMode == true && nRT == RT_SYSEQ )
       //  pImp->CloseWin();

        contentsChanged = true;
        break;
    case VF3_1:   // =VF_BYPASS   - do nothing
        retType = VF3_3;
        break;
    }
    return  retType;
    // rebuild text
}

// Rebuild loading before record (error if current record undefined)
void
TCModule::CmDerive()
{
    try
    {
        if( !pVisor->ProfileMode && ( nRT == RT_PARAM || nRT >= RT_SYSEQ ) )
            Error( GetName(), "Please, do it in Project mode!");
        if( pVisor->ProfileMode &&
             ( nRT < RT_SYSEQ && nRT != RT_SDATA ) )
            Error( GetName(), "Please, do it in Database mode!");

        if( ! MessageToSave() )
	    return;

        string str = char_array_to_string( db->UnpackKey(), db->KeyLen() );
                    //db->PackKey();
        if( str.find_first_of("*?" ) != string::npos
            || ( db->GetStatus() == UNDF_ && nRT != RT_SDATA) )   // 09/11/2004 Sveta
            Error( GetName(), "E2 Cannot save under record key template, or record contents are not yet loaded!");

        check_input( db->UnpackKey(), 0 );  // SD 18/11/2008
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


// Calculate record

void
TCModule::RecCalc( const char* )
{
    contentsChanged = true;
}

// Calc loading before record (error if current record undefined)

void
TCModule::CmCalc()
{
    try
    {
        clearEditFocus();
        if( !pVisor->ProfileMode && ( nRT == RT_PARAM || nRT >= RT_SYSEQ ) )
            Error( GetName(), "Please, do it in Project mode!");
        //--if( pVisor->ProfileMode == true &&
        //--     ( nRT < RT_SYSEQ && nRT != RT_SDATA ) )
        //--    Error( GetName(), "Please, do it in Database mode!");

      //  if( ! MessageToSave() ) //27/08/2010
      //    return;

        string str=db->PackKey();
        if( str.find_first_of("*?" ) != string::npos
               || ( db->GetStatus()== UNDF_ && db->RecCount()
                 && nRT != RT_SDATA ))  // 09/11/2004 Sveta
            Error( GetName(), "E3 Cannot save under record key template, or record contents are not yet loaded!");
        //int  Rnum = db->Find( str.c_str() );
        //ErrorIf( Rnum<0, GetKeywd(),
        // "Record to calculate not found!");
        // db->Get( Rnum ); must be done before
        // dyn_set(); must be done before
        str = char_array_to_string( db->UnpackKey(), db->KeyLen() );
        check_input( str.c_str() );
        SetString("Calculation... ");
        clock_t t_start11, t_end11;
        t_start11 = clock();

        RecCalc( str.c_str() );

        // 19/02/2007 Sveta
        t_end11 = clock();
        clock_t dtime = ( t_end11- t_start11 );
        char  buf[200];
        sprintf(buf, "Calculation finished OK (elapsed time: %lg s).",
                static_cast<double>(dtime)/CLOCKS_PER_SEC);
        SetString(buf);

        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        pVisor->Update( true );
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

// Add new record to DB

void
TCModule::CmNew()
{
    try
    {
        if(  nRT == RT_PARAM || nRT >= RT_SYSEQ )
            Error( GetName(), "Please, do it in Project mode!");

        if( ! MessageToSave() )
	    return;

       string dlgName = "Please, set a new record key ";
       string str = db->PackKey();
       int  Rnum;

   AGN:
       str = GetKeyofRecord( str.c_str(), dlgName.c_str(), KEY_NEW);
       if(  str.empty() )
            return;

       Rnum = db->Find( str.c_str() );
       if( Rnum>=0 ) // name of exist record
       {
          dlgName = "This record already exists! Please, enter another name.";
          goto AGN;
       } // ErrorIf( Rnum>=0, GetName(), "This record alredy exist!");

        str = char_array_to_string( db->UnpackKey(), db->KeyLen() );
        check_input( str.c_str(), 0 ); // SD 18/11/2008
        RecBuild( str. c_str(), VF_REMAKE );
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

void
TCModule::CmCreate()
{
    try
    {
        if(  nRT == RT_PARAM || nRT >= RT_SYSEQ )
            Error( GetName(), "Please, do it in Project mode!");

        if( ! MessageToSave() )
	    return;

        string dlgName = "Please, set a new record key ";
        string str = db->PackKey();
        int  Rnum;

    AGN:
        str = GetKeyofRecord( str.c_str(), dlgName.c_str(), KEY_NEW);
        if(  str.empty() )
             return;

        Rnum = db->Find( str.c_str() );
        if( Rnum>=0 ) // name of exist record
        {
           dlgName = "This record already exists! Please, enter another name.";
           goto AGN;
        } // ErrorIf( Rnum>=0, GetName(), "This record alredy exist!");


        str = char_array_to_string( db->UnpackKey(), db->KeyLen() );
        check_input( str.c_str() , 0 ); // SD 18/11/2008
        RecBuild( str.c_str(), VF_CLEARALL );
        SetString("Remake of the new record finished OK. "
                  " It is recommended to re-calculate the data");
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        pVisor->Update( true );
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

/*
//Show next record from DB (and check relationship)
void
TCModule::CmFind() // ???? error in smShow
{
   try{
       if(  nRT == RT_PARAM || nRT >= RT_SYSEQ )
          Error( GetName(), "Do it in Project mode!");
       CmShow();
       if( check_input( db->UnpackKey() ) )
       {   CmCalc();
           SetString("Calculate finished OK.");
       }
       pVisor->Update();
     }
   catch( TError& xcpt )
     {  vfMessage( xcpt.title, xcpt.mess);
     }
}
*/


// Test record with key

void
TCModule::TryRecInp( const char *_key, time_t& time_s, int q )
{
    string  key( _key, 0, db->KeyLen() );

    if( ! MessageToSave() )
	return;

    RecStatus iRet = db->Rtest( key.c_str(), 1 );
    std::string msg;

    switch( iRet )
    {
    case MANY_:
        msg = "Record selection error! ";
        msg += key;
// Sveta 14/06/01        if( RecChoise( key ) == false )
            Error( GetName(), msg );
        break;
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
            msg += ": Data record not found, \n"
                   " key  '";
            msg += key;
            msg += "'.\n Maybe, database file is not linked to chain\n";
            if(pVisor->ProfileMode)
                Error( GetName(), msg );
            msg +=  "Create new record?";
            if( !vfQuestion(window(), GetName(), msg ))
                Error( GetName(), "Record creation rejected!");
            string str = key;

            if( str.find_first_of("*?" ) != string::npos)  // pattern
                str = GetKeyofRecord( str.c_str(),
                       "Please, set a new record key ", KEY_NEW);
            if(  str.empty() )
                Error( GetName(), "Record creation rejected!");
            int  Rnum = db->Find( str.c_str() );
            ErrorIf( Rnum>=0, GetName(), "A record with such key already exists!");
            pVisorImp->OpenModule(pVisorImp, nRT);
            string str1 = char_array_to_string( db->UnpackKey(), db->KeyLen() );
            check_input( str1.c_str() );
            RecBuild( str.c_str() );
            SetString("Remake of the new record finished OK. "
                      " It is recommended to re-calculate the data");
            pVisor->Update();
            Error("Calculation failed!", "Check data fields and try calculation again!");
        }
        break;
    case FAIL_:
        msg = "Failure!!! Database chain ";
        msg += GetName();
        msg += " is corrupt,\n"
               "Data Record key '";
        msg += key;
        msg += "'\n Try to unload or re-index this database chain...";
        Error( GetName(),  msg );
    }
}

//----------------------------------------------------------
//--- Manipulation of the current record
//--- Process, UnSpace, GTdemo, Duterm in project mode
//----------------------------------------------------------

// load record from data base (show in project mode)

void TCModule::RecordLoadinProfile( const char *key )
{
    string str;

    if( key==nullptr )
    {
        str = getFilter();
        str = GetKeyofRecord( str.c_str(), "Please, select a record key ", KEY_OLD );
    }
    else str=string(key);
    // get record
    if( str.empty() )
            return;
    RecInput( str.c_str() );
    check_input( str.c_str(), 0 );
}

// loads the project


void TCModule::CmLoadinProfile(const char *key)
{
    try
    {
        if( nRT < RT_SYSEQ && nRT != RT_SDATA)
        {
          //  Error( GetName(),  "Please, do it in Database mode!");
          CmShow(key);
          return;
        }

        if( ! MessageToSave() )
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


// Adds new record to DB in Project mode

void
TCModule::CmNewinProfile()
{
    try
    {
        if( nRT < RT_SYSEQ  && nRT != RT_SDATA )
            Error( GetName(),  "Please, do it in Database mode!");
        if( ! MessageToSave() )
	    return;

        // Get record key
        string str = makeKeyFilter();
        string dlgName = "Please, set a new record key ";
        int  Rnum;

    AGN:
        str = GetKeyofRecord( str.c_str(), dlgName.c_str(), KEY_NEW);
        if(  str.empty() )
             return;

        Rnum = db->Find( str.c_str() );
        if( Rnum>=0 ) // name of exist record
        {
           dlgName = "This record already exists! Please, enter another name.";
           goto AGN;
        } // ErrorIf( Rnum>=0, GetName(), "This record alredy exist!");

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

void
TCModule::CmCreateinProfile()
{
    try
    {
        if( nRT < RT_SYSEQ && nRT != RT_SDATA )
            Error( GetName(),  "Please, do it in Database mode!");

        if( ! MessageToSave() )
	    return;

        // Get record key
        string str = makeKeyFilter();
        string dlgName = "Please, set a new record key ";
        int  Rnum;

    AGN:
        str = GetKeyofRecord( str.c_str(), dlgName.c_str(), KEY_NEW);
        if(  str.empty() )
             return;

        Rnum = db->Find( str.c_str() );
        if( Rnum>=0 ) // name of exist record
        {
           dlgName = "This record already exists! Please, enter another name.";
           goto AGN;
        } // ErrorIf( Rnum>=0, GetName(), "This record alredy exist!");

        check_input( str.c_str(), 0 );
        RecBuild( str.c_str(), VF_CLEARALL );
        SetString("Remake of the new record finished OK. "
                  " It is recommended to re-calculate data");
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        pVisor->Update( true );
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}


// virtual callback for plotting the record
// should be redefined in subclasses (modules)
// to make some plotting

void
TCModule::RecordPlot( const char* /*key*/ )
{
    vfMessage(window(), GetName(), "Plotting?\n\nNot here, yet... Sorry!");
}


// callback for 'Plot' command

void
TCModule::CmPlot()
{
    try
    {
        if( ! MessageToSave() )
	    return;

        string str=db->PackKey();
        if( str.find_first_of("*?" ) != string::npos
        || ( db->GetStatus()== UNDF_ && db->RecCount() &&
           nRT != RT_SDATA ))  // 09/11/2004 Sveta
            Error( GetName(), "E4 Cannot save under record key template, or record contents are not yet loaded!");
        //int  Rnum = db->Find( str.c_str() );
        //ErrorIf( Rnum<0, GetName(), "Record to demonstrate not found!");
        RecordPlot( str.c_str() );
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

void
TCModule::PrintSDref( const char* sd_key, const char* text_fmt )
{
 // read sdref record with format prn
/* TSData::pm->RecInput( sd_key );
 char * text_fmt = TSData::pm->getAbstr();
 if( !text_fmt )
   Error( sd_key, "No format text in this record.");  */
 // open file to output

    std::string filename="";
    if( vfChooseFileSave(window(), filename, "Please, provide name of TXT-file") )
    {
        ios::openmode mod = ios::out;

        if( !access(filename.c_str(), 0 ) ) //file exists
            switch( vfQuestion3( window(), filename,
                             "This file exists! What to do?",
                                 "&Append", "&Overwrite", "&Cancel") )
            {
            case VF3_2:
                mod = ios::out;
                break;
            case VF3_1:
                mod = ios::out|ios::app;
                break;
            case VF3_3:
                return;
            }
       fstream f( filename, mod );
       ErrorIf( !f.good() , filename, "Fileopen error");
      // scan and print format
      TPrintData dat( sd_key, nRT, f, text_fmt );
    }

}



// virtual callback for printing the record
// should be redefined in subclasses (modules)
// to make some plotting

void
TCModule::RecordPrint( const char* key )
{
  // select  SDref key
 string sd_key;
 if( key )
  sd_key=key;
 else
 {
    // read sdref record with format prn
    sd_key = "pscript*:*:";
    sd_key += db->GetKeywd();
    sd_key += "*";
    sd_key += ":";
 }
 if( sd_key.find_first_of("*?" ) != string::npos )
 {
     sd_key = dynamic_cast<TCModule *>(aMod[RT_SDATA].get())->GetKeyofRecord(
          sd_key.c_str(), "Please, select a print script", KEY_OLD);
 }

 if( sd_key.empty() )
     return;

  dynamic_cast<TCModule *>(aMod[RT_SDATA].get())->RecInput( sd_key.c_str() );
  const char * text_fmt = static_cast<const char *>(aObj[o_sdabstr]->GetPtr());
  if( !text_fmt )
       Error( sd_key, "No print script in this record.");

  PrintSDref( sd_key.c_str(), text_fmt );
}

// callback for 'print' command

void
TCModule::CmPrint()
{
    try
    {
        if( ! MessageToSave() )
	    return;

        /*string str=db->PackKey();
        if( str.find_first_of("*?" ) != string::npos )
            Error( GetName(), "Current record is not defined!");*/
        RecordPrint();
        SetString("Printing of a record finished OK. ");
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

void
TCModule::CmScript()
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

      sd_key += "*";
      sd_key += ":";
      sd_key =dynamic_cast<TCModule *>(aMod[RT_SDATA].get())->GetKeyofRecord(
          sd_key.c_str(), "Please, select an appropriate script", KEY_OLD);
      if( sd_key.empty() )
      return;
      dynamic_cast<TCModule *>(aMod[RT_SDATA].get())->RecInput( sd_key.c_str() );
       /*if( pImp )
           pVisorImp->OpenModule(pImp->topLevelWidget(), RT_SDATA);
       else*/ pVisor->OpenModule(nullptr, RT_SDATA,0,true);  // KD: workaround for NewSystemDialog
      dynamic_cast<TCModule *>(aMod[RT_SDATA].get())->Update();

    }
    catch( TError& xcpt )
    {
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}


//----------------------------------------------------------
//--- Manipulation with database files   (Service functions )
//----------------------------------------------------------

// Index files of data base

void
TCModule::CmRebildFile()
{
    try
    {
        if( ! MessageToSave() )
	    return;

        pVisor->Message( window(), GetName(), "Compressing database file(s). "
         "Please, wait...", 0, 100 );

        db->RebildFile(SelectFileList(closef|openf));
        // pVisorImp->CloseMessage();
        dyn_set();
        //        SetString("Compression finished OK");
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        pVisor->Update();
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

// Rename files from opend files list of data base

void
TCModule::CmAddFileToList()
{
    try
    {
        if( ! MessageToSave() )
	    return;

        std::string filename = db->GetKeywd();
                filename += ".newname.";
                filename +=  PDB_EXT;
        std::string filter = "*.";
                filter +=  PDB_EXT;

        if( vfChooseFileSave(window(), filename,
       "Enter a new database file name", filter.c_str() ) == false )
            return;
        // test Path Added Sveta 5/03/02
        // pdb extension, name must started db->GetKeywd()
        std::string dir;
        std::string name;
        std::string ext;
        u_splitpath( filename, dir, name, ext);
        ext = PDB_EXT;
        size_t pos = name.find( db->GetKeywd() );
        if( pos != 0 )
        {  name = "." + name;
           name = db->GetKeywd()+ name;
        }
        filename = u_makepath( dir, name, ext );

        TDBFile* file = new TDBFile(filename.c_str());
        /////////
        db->AddFileToList(file);
        db->SetKey( ALLKEY );
        //        SetString("Command finished OK");
        pVisor->Update(); // no objecs change, only title
    }

    catch( TError& xcpt )
    {
        pVisor->Update();
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

// Open new files from the list of closed files in the data base

void
TCModule::CmAddOpenFile()
{
    try
    {
        if( ! MessageToSave() )
	    return;

        db->AddOpenFile(SelectFileList(closef));
        db->SetKey( ALLKEY );
        dyn_set();
        //        SetString("Command finished OK");
        pVisor->Update(); // no objecs change, only title
    }

    catch( TError& xcpt )
    {
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

// Rebuild the list of opened files for the data base

void
TCModule::CmReOpenFileList()
{
    try
    {
        if( ! MessageToSave() )
	    return;

        TCIntArray arr = SelectFileList(closef|openf|oldself);

        if( arr.size() < 1 )
            if( !vfQuestion( window(), GetName(),
                  "No database files selected to open! Continue?" ))
                return;

        db->Close();
        db->Open( true, UPDATE_DBV, arr );
        db->SetKey( ALLKEY );
        //       SetString("Command finished OK");
        dyn_set();
        pVisor->Update(); // no objecs change, only title
    }

    catch( TError& xcpt )
    {
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

//----------------------------------------------------------
//--- Manipulation with list of records
//----------------------------------------------------------

// Export list of keys to TXT file

void
TCModule::CmKeysToTXT()
{
    try
    {
        if( ! MessageToSave() )
	    return;

        KeysToTXT( Filter.c_str() );
        db->SetKey( ALLKEY );
        //        SetString("Command finished OK");
        pVisor->Update(); // no objecs change, only title
    }

    catch( TError& xcpt )
    {
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

// Delete the list of records

void
TCModule::CmDeleteList()
{
    try
    {
        if( ! MessageToSave() )
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


// Copy list command (Sveta 14/06/01)

void
TCModule::CmCopyList( )
{
    try
    {
        if( ! MessageToSave() )
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

void
TCModule::CmRenameList( )
{
    try
    {
        if( ! MessageToSave() )
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

// Transfer the list of records

void
TCModule::CmTransferList()
{
    try
    {
        if( ! MessageToSave() )
	    return;

        Transfer( Filter.c_str() );
        dyn_set();
        //       SetString("Command finished OK");
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        pVisor->Update();
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

// Export data to text file

void
TCModule::CmExport()
{
    try
    {
        if( ! MessageToSave() )
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

void
TCModule::CmBackup()
{
    try
    {
        if( ! MessageToSave() )
	    return;

        RecToTXT( Filter.c_str() );
        //  SetString("Command finished OK");
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        pVisor->Update();
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

// Import data from text file

void
TCModule::CmImport()
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

void
TCModule::CmRestore()
{
    try
    {
        if( ! MessageToSave() )
	    return;

        RecOfTXT( );
        //     SetString("Command finished OK");
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        pVisor->Update();
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

void TCModule::CmBackuptoJson()
{
    try
    {
        if( !MessageToSave() )
            return;

        RecListToJSON( Filter.c_str() );
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        pVisor->Update();
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

void TCModule::CmRestorefromJson()
{
    try
    {
        if( !MessageToSave() )
            return;

        RecListFromJSON();
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        pVisor->Update();
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

// Adds the record

void
TCModule::AddRecord(const char* key )
{
    int file = db->fNum;

    ErrorIf(!key, "TCModule::AddRecord()", "empty record key!");

    if( strpbrk(key,"*?/")!=nullptr )
        Error( GetName(), "Attempt to insert record with template key!");

    ErrorIf( db->fOpenNameBuf.size()<1, GetName(), "No database file choosen");
    if( db->fOpenNameBuf.size()>1 )
    {
        string s="Choose a database file to put a record: "+ string(key);
        file = db->fOpenNameBuf.size() - 1;  // 04.04.01 KD
        file = vfChoice(window(), db->fOpenNameBuf, s.c_str(), file );
    }
    else
        file = 0;
    if( file >= 0)
        db->AddRecordToFile(key, file);
}

// Adds the record, or all records to file Sveta 15/06/01

void TCModule::AddRecord(const char* key, int& fnum )
{
    int file = db->fNum;

    ErrorIf(!key, "TCModule::AddRecord()", "empty record key!");

    if( strpbrk(key,"*?/") != nullptr )
        Error( GetName(), "Attempt to insert record with template key!");
    if( fnum >= 0 )
      file = fnum;
    else
    {
        ErrorIf( db->fOpenNameBuf.size()<1, GetName(),
              "No database file choosen");
        if( db->fOpenNameBuf.size()>1 )
        {
           bool ok_to_all = false;
           string s="Choose a database file to put a record: "
                      + string(key);
           file = db->fOpenNameBuf.size() - 1;  // 04.04.01 KD
           file = vfChoice2(window(), db->fOpenNameBuf, s.c_str(), file, ok_to_all );
           if( ok_to_all == true && file >= 0 )
              fnum = file;
       }
       else
         file = 0;
     }
    if( file >= 0 )
      db->AddRecordToFile(key, file);
    else
        fnum = - 2;
}

void TCModule::ReplaceRecordwithQuestion(int Rnum, const char *key, int &quest_reply)
{
    if(quest_reply == VF_YES_ALL)
    {
        db->Rep(Rnum);
    }
    else if(quest_reply != VF_NO_ALL)
    {
        quest_reply = vfQuestionYesNoAll(window(), key, replace_question);
        switch(quest_reply)
        {
        case VF_YES_ALL:
        case VF_YES:
            db->Rep(Rnum);
            break;
        }
    }
}

// Test unique keys name before add the record(s)
int TCModule::AddRecordTest(const char* key, int& fnum )
{
    int  Rnum;
    string str = key;

AGN: Rnum = db->Find( str.c_str() );
    if( Rnum>=0 ) // name of exist record
    {
        str=db->PackKey();
        str = GetKeyofRecord( str.c_str(),
                              "This key record already exists! Replace please?", KEY_NEW );
        if(str.empty())
            return 0;
        goto AGN;
    }
    AddRecord( str.c_str(), fnum );
    return 1;
}

// Unloades Data Record keys to txt-file

void TCModule::KeysToTXT( const char *pattern )
{
    TCStringArray aKey = vfMultiKeys( window(),
       "Please, mark record keys to be listed in txt-file",
       nRT, pattern );
    if( aKey.size() <1 )
        return;

    string s = GetName();
    std::string filename;
    s += " : Please, select file to write record keys";
    if( !vfChooseFileSave(window(), filename, s.c_str()) )
        return;
    fstream f(filename, ios::out);
    ErrorIf( !f.good() , GetName(), "Fileopen error");

    // check for errors
    f << " " << GetName() << " \'" << pattern << "\' Nrec="  << aKey.size() << "\n";
    for(size_t i=0; i<aKey.size(); i++ )
    {
        f << aKey[i].c_str() << "\n";
    }
    ErrorIf( !f.good() , GetName(), "Writefile error");
}

// Unloads Data Record to txt-file
void TCModule::RecToTXT( const char *pattern )
{
    TCStringArray aKey = vfMultiKeys( window(),
       "Please, mark records to be unloaded into txt-file",
       nRT, pattern );
    if( aKey.size() <1 )
        return;

    std::string s = GetName();
    std::string filename;
    s += " : Please, give a file name for unloading records";
    if( vfChooseFileSave( window(), filename, s.c_str() ) == false )
        return;
    fstream f(filename.c_str(), ios::out);
    ErrorIf( !f.good() , GetName(), "File write error");

    for(size_t i=0; i<aKey.size(); i++ )
    {
       int Rnum = db->Find( aKey[i].c_str() );
       db->Get( Rnum );

       aObj[o_reckey]->SetPtr( const_cast<void*>(static_cast<const void *>(aKey[i].c_str())) );
       aObj[o_reckey]->toTXT(f);
        for(int no=db->GetObjFirst(); no<db->GetObjFirst()+db->GetObjCount();  no++)
            aObj[no]->toTXT(f);
    }

    ErrorIf( !f.good() , GetName(), "Filewrite error");
    dyn_set();
}

// Loads Data Records from txt-file

void TCModule::RecOfTXT()
{
    char buf[150];
    int Rnum;
    int fnum= -1 ;// FileSelection dialog: implement "Ok to All"

    std::string s = std::string( GetName() )+" : Please, select file with unloaded records";
    std::string filename;
    if( vfChooseFileOpen( window(), filename, s.c_str() ) == false )
        return;
    fstream f(filename.c_str(), ios::in);
    ErrorIf( !f.good() , GetName(), "Fileread error...");

    int quest_reply = VF_UNDEF;
    while( !f.eof() )
    {
        aObj[o_reckey]->SetPtr(buf);
        aObj[o_reckey]->ofTXT(f);
        for(int no=db->GetObjFirst(); no<db->GetObjFirst()+db->GetObjCount(); no++)
            aObj[no]->ofTXT(f);
        buf[db->KeyLen()] = '\0';

        Rnum = db->Find( buf );
        if( Rnum >= 0 )
        {
           ReplaceRecordwithQuestion(Rnum, buf, quest_reply);
           if( quest_reply == VF_CANCEL )
               break;
        }
        else
        {
            AddRecord( buf, fnum );
            if( fnum == -2 )
                break;
        }
        s = std::string( buf );
        do
        {
            f.get(buf[0]);
            if( !f.good() )
                break;
        } while( buf[0] == ' ' || buf[0]=='\n' );
        if( f.eof() )
            break;
        f.putback( buf[0] );
    }
    if( f.bad() )
    {
        std::string str = "File read error! \n";
        str += "Last good record :";
        str += s;
        Error( GetName(), str );
    }
    dyn_set();
}

// Unloads Data Record to user format
void TCModule::RecExport( const char *pattern )
{

    // read sdref record with format prn
    string sd_key = "escript*:*:";
            sd_key += db->GetKeywd();
            sd_key += "*";
            sd_key += ":";
    sd_key = dynamic_cast<TCModule *>(aMod[RT_SDATA].get())->GetKeyofRecord(
          sd_key.c_str(), "Select key of escript format", KEY_OLD);
    if( sd_key.empty() )
     return;
    dynamic_cast<TCModule *>(aMod[RT_SDATA].get())->RecInput( sd_key.c_str() );
    char * text_fmt = static_cast<char *>(aObj[o_sdabstr]->GetPtr());
    if( !text_fmt )
       Error( sd_key, "No format text in this record.");

    TCStringArray aKey = vfMultiKeys( window(),
       "Please, mark records to be unloaded into txt-file",
       nRT, pattern );
    if( aKey.size() <1 )
        return;

    std::string s = GetName();
    std::string filename;
    s += " : Please, give a file name for unloading records";
    if( vfChooseFileSave( window(), filename, s.c_str() ) == false )
        return;
   ios::openmode mod = ios::out;
   if( !(::access(filename.c_str(), 0 )) ) //file exists
     switch( vfQuestion3( window(), filename,
                   "This file exists! What to do?",
                  "&Append", "&Overwrite", "&Cancel") )
     {
       case VF3_2:
                mod = ios::out;
                break;
      case VF3_1:
                mod = ios::out|ios::app;
                break;
      case VF3_3:
                return;
     }
    fstream f(filename.c_str(), mod);
    ErrorIf( !f.good() , GetName(), "File write error");

    for(size_t i=0; i<aKey.size(); i++ )
    {
       int Rnum = db->Find( aKey[i].c_str() );
       db->Get( Rnum );
       TPrintData dat( sd_key.c_str(), nRT, f, text_fmt );
    }

    ErrorIf( !f.good() , GetName(), "Filewrite error");
    dyn_set();
}

// Loads Data Records from user format
void TCModule::RecImport()
{
    //char buf[150];
    int Rnum;
    int fnum= -1 ;// FileSelection dialog: implement "Ok to All"
    char ch;

    // read sdref record with format read
    string sd_key = "iscript*:*:";
            sd_key += db->GetKeywd();
            sd_key += "*";
            sd_key += ":";
    sd_key = dynamic_cast<TCModule *>(aMod[RT_SDATA].get())->GetKeyofRecord(
          sd_key.c_str(), "Select key of iscript format", KEY_OLD);
    if( sd_key.empty() )
     return;
    dynamic_cast<TCModule *>(aMod[RT_SDATA].get())->RecInput( sd_key.c_str() );
    char * text_fmt = static_cast<char *>(aObj[o_sdabstr]->GetPtr());
    if( !text_fmt )
       Error( sd_key, "No format text in this record.");

    // translate scripts
    TReadData dat( sd_key.c_str(), nRT, text_fmt );

    std::string s =std::string( GetName() )+" : Please, select file with imported records";
    std::string filename;
    if( vfChooseFileOpen( window(), filename, s.c_str() ) == false )
        return;
    fstream f(filename.c_str(), ios::in);
    ErrorIf( !f.good() , GetName(), "Fileread error...");

    int iter = 0;
    int quest_reply = VF_UNDEF;
    while( !f.eof() )
    {
        dat.readRecord( iter, f );
        std::string keyp = db->UnpackKey();
        Rnum = db->Find( keyp.c_str() );

        if( Rnum >= 0 )
        {
            ReplaceRecordwithQuestion(Rnum, keyp.c_str(), quest_reply);
            if( quest_reply == VF_CANCEL )
                break;
        }
        else
        {
            AddRecord( keyp.c_str(), fnum );
            if( fnum == -2 )
                break;
        }
        s = keyp;
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
      std::string str = "File read error! \n";
              str += "Last good record :";
              str += s;
      Error( GetName(), str );
     }
    dyn_set();
}

void TCModule::RecListToJSON(const char *pattern)
{
    TCStringArray aKey = vfMultiKeys( window(),
                                      "Please, mark records to be unloaded to JSON",
                                      nRT, pattern );
    if( aKey.size() <1 )
        return;

    string s = GetName();
    string filename = GetName();
           filename += "_backup.json";
    s += " : Please, give a file name for unloading records";
    if( vfChooseFileSave( window(), filename, s.c_str(), "*.json" ) == false )
        return;

    QJsonArray allArray;
    for( size_t i=0; i<aKey.size(); i++ )
    {
        int Rnum = db->Find( aKey[i].c_str() );
        db->Get( Rnum );
        db->SetKey( aKey[i].c_str() );
        QJsonObject recObject;
        db->toJsonObject( recObject );
        allArray.append(recObject);
    }
    QJsonDocument saveDoc(allArray);
    fstream f_out( filename, ios::out );
    if( f_out.good() )
        f_out << saveDoc.toJson().data() << std::endl;
    dyn_set();
}

void TCModule::CurrentToJSON(const std::string& filename)
{
    QJsonObject recObject;
    db->toJsonObject( recObject );
    QJsonDocument saveDoc(recObject);
    fstream f_out( filename, ios::out );
    if( f_out.good() )
        f_out << saveDoc.toJson().data() << std::endl;
}

void TCModule::RecListFromJSON()
{
    int fnum= -1 ;// FileSelection dialog: implement "Ok to All"

    // Choose file name
    string s =string( GetName() )+" : Please, select file with unloaded records";
    string filename;
    if( vfChooseFileOpen( window(), filename, s.c_str(), "*.json" ) == false )
        return;

    QFile CurrentFile(filename.c_str());
    if(!CurrentFile.open(QIODevice::ReadOnly)) return;
    QByteArray json_data = CurrentFile.readAll();

    QJsonDocument readDoc = QJsonDocument::fromJson(json_data);
    QJsonArray allArray = readDoc.array();
    int quest_reply = VF_UNDEF;
    for( const auto& val : allArray)
    {
        std::string keyp = db->fromJsonObject( val.toObject() );
        auto Rnum = db->Find( keyp.c_str() );
        if( Rnum >= 0 )
        {
            ReplaceRecordwithQuestion(Rnum, keyp.c_str(), quest_reply);
            if( quest_reply == VF_CANCEL )
                break;
        }
        else
        {
            AddRecord( keyp.c_str(), fnum );
            if( fnum == -2 )
                break;
        }
    }
    dyn_set();
}


// delete list of records from Data Base
void TCModule::DelList( const char *pattern )
{
    TCStringArray aKey = vfMultiKeys( window(),
       "Please, mark record keys to be deleted from database",
       nRT, pattern );
    int ichs = 1;

    for(size_t i=0; i<aKey.size(); i++ )
    {
        string str = "Please, confirm deleting record \nwith key: ";
        str += aKey[i];
        if( ichs )
        {
         switch( vfQuestion3(window(), GetName(), str,
                                "&Yes", "&No", "&Delete All" ))
            {
            case VF3_3:
                ichs=0;
                break;
            case VF3_1:
                break;
            case VF3_2:
                continue;
            }
        }
        DeleteRecord( aKey[i].c_str(), false );
    }
}

// transfer list of records in Data Base to another file
// (ever used???)  Has to be re-implemented, indeed !

void TCModule::Transfer( const char *pattern )
{
    int nrec = 0;
    int fnum= -1 ;// FileSelection dialog: implement "Ok to All"

    TCStringArray aKey = vfMultiKeys( window(),
       "Please, mark record keys to be moved",
       nRT, pattern );


    for(size_t i=0; i<aKey.size(); i++ )
    {
        nrec = db->Find( aKey[i].c_str() );
        db->Get( nrec );
        /// !!!
        auto oldfile = db->fNum;
        db->Del( nrec );
        AddRecord( aKey[i].c_str(), fnum );
        if( fnum == -2 )
        {
          db->AddRecordToFile( aKey[i].c_str(), oldfile );
          break;
       }
    }
}

// copy list of records in Data Base to another file
// (Sveta 15/06/01)

void
TCModule::CopyRecordsList( const char *pattern, bool if_rename )
{
    int nrec = 0;
    int fnum= -1 ;// FileSelection dialog: implement "Ok to All"
    string str;

    if( if_rename )
     str = "Please, mark record keys to be renamed";
    else
     str = "Please, mark record keys to be copied";

    TCStringArray aKey = vfMultiKeys( window(),
       str.c_str(), nRT, pattern );

    int rn_type = 0;

    if( aKey.size() < 2)
      rn_type = 1;
    else
     {  switch (vfQuestYesNoCancel(window(),
        "How to rename records",
        "Each key separately (Yes) \n"
        "All records using template (No)" ))
       {
        case VF_YES:
            rn_type = 1;
            break;
        case VF_NO:
            break;
        case VF_CANCEL:
        return;
       }
    }
    uint fld = db->KeyNumFlds()-1;
    string from_t;
    string to_t;
    if( rn_type == 0 )
    {
     if( !vfKeyCanged(window(), "", from_t,  to_t, db->FldLen( fld ) ))
      return;
    }

    for(size_t i=0; i<aKey.size(); i++ )
    {
        nrec = db->Find( aKey[i].c_str() );
        db->Get( nrec );
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
          //int fld = db->KeyNumFlds()-1;
          string str1;
          str = "";
          for(uint ii=0; ii<fld; ii++)
          {
             str1 = string(db->FldKey( ii ), 0, db->FldLen( ii ));
             strip( str1 );
             str += str1 + ":";
           }
          str1= string(db->FldKey( fld ), 0, db->FldLen( fld ));
          ChangeforTempl( str1, from_t, to_t, db->FldLen( fld ));
          str += str1 + ":";

        }
       if( if_rename )
       {
         fnum = db->fNum;
         db->Del( nrec );
         //Point SaveRecord
         if( !AddRecordTest( str.c_str(), fnum ))
          db->AddRecordToFile( aKey[i].c_str(), fnum );
         //db->AddRecordToFile( str.c_str(), fnum );
       }
       else
       {
         //Point SaveRecord
         AddRecordTest( str.c_str(), fnum );
         if( fnum == -2 )
             break;
       }
    }
}

TCIntArray
TCModule::SelectFileList(int mode)
{
    TCStringArray names;
    TCIntArray indx;
    TCIntArray sel;

    db->GetFileList(mode, names, indx, sel);

    TCIntArray aSel = vfMultiChoiceSet(window(), names,
         "Selection of files", sel);

    TCIntArray arr;
    for( size_t i=0; i<aSel.size(); i++ )
        arr.push_back( indx[aSel[i]] );

    return arr;
}

//========================================================

// descructor for TModList for cleaning up Modules from memory

TModuleList::~TModuleList()
{
//    for( unsigned ii=0; ii<GetCount(); ii++ )
//        delete elem(ii);
}

// public list of modules
TModuleList aMod;

//--------------------- End of v_module.cpp ---------------------------


