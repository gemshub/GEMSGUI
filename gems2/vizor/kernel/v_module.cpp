//-------------------------------------------------------------------
// Id: gems/vizor/kernel/v_module.cpp  version 2.0.0    2001
//
// Implementation of TSubModule, TModule and TModList classes
//
// Created : 970207    Modified: 010908
//
// Copyright (C) 1996-2001 A.Rysin, S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license #1435515
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://les.web.psi.ch/Software/GEMS-PSI/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

const char *GEMS_TOC_HTML = "gems_toc";

#include "v_module.h"
#include "module_w.h"
#include "visor_w.h"
#include "visor.h"
#include "service.h"
#include "v_mod.h"
#include "t_print.h"
#include "t_read.h"
#ifdef __unix
#include <unistd.h>
#endif

// Default constructor and destructor

TSubModule::TSubModule( int nrt ):
        nRT( nrt ),
        fEdit(false),
        //   sBw(600), sBh(460),
        pImp(0)
{}


TSubModule::~TSubModule()
{}


// (supposed to be :) Called from TModuleImp right when
// window is created

void
TSubModule::Setup()
{
    ///  SetCaption( (char*)GetName() );
}


// Called from TModuleImp right before
// window is closed

void
TSubModule::EvClose()
{
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
    pVisorImp->OpenModule(parent, nRT, 0, viewmode);
}

// Callback for 'Help' command

void
TSubModule::CmHelp()
{
//    pVisorImp->OpenHelp("GEMS-Linux-index");
    pVisorImp->OpenHelp( GEMS_TOC_HTML );    // 05.01.01

}


//========================================
// TCModule class
//=======================================

// Default constructor

TCModule::TCModule( int nrt ):
        TSubModule( nrt ),
        db(&rt[nrt]),
        nQ(1), Filter(ALLKEY), start_title(" ")
{}
// start_title = " Access to database record without remake/recalculation"


TCModule::~TCModule()
{}



void
TCModule::Setup()
{
    ///  SetCaption( (char*)GetName() );
}


// Called from TModuleImp right when
// window is created


void
TCModule::EvClose()
{
    try
    {
        MessageToSave();
    }
    catch( TError& xcpt )
    {
        vfMessage(pImp, xcpt.title, xcpt.mess);
    }
}


// opens window with 'Remake record' parameters

void
TCModule::MakeQuery()
{
    pImp->MakeQuery();
}


// save solicitation

void
TCModule::MessageToSave()
{
    if( nRT != RT_SDATA &&
        (pVisor->ProfileMode == true && nRT < RT_SYSEQ ) )
        return;

    const char* key= db->PackKey();
    gstring str=key;
    if( fEdit==true && str.find_first_of("*?" ) == gstring::npos )
    {
        if( vfQuestion(pImp, str.c_str(),
                       "Data record has been changed! Save changes?"))
            RecSave( str.c_str() );
    }
    fEdit = false;
}

// get key of record (existing key, new key or key temlate )

gstring
TCModule::GetKeyofRecord( const char *oldKey, const char *strTitle,
                          int keyType )
{
    gstring str = GetName();
    str += ": ";
    str += strTitle;

    gstring key;

    if( oldKey == 0 )
    {
        if(Filter.empty())
            key = ALLKEY;
        else key = Filter;
    }
    else key = oldKey;

    switch( keyType )
    {
    case KEY_OLD:
        return vfKeyEdit(pImp, str.c_str(), nRT, key.c_str() );
    case KEY_NEW:
        return vfKeyTemplEdit(pImp, str.c_str(), nRT, key.c_str(), false );
    case KEY_TEMP:
        Filter = vfKeyTemplEdit(pImp, str.c_str(), nRT, key.c_str() );
        return Filter;
    }
    Error( str.c_str(), "Illegal record key editing mode");
    return "";
}

// Tools for correcting IPN Math Scripts

bool
TCModule::CheckEqText( const char *erscan, const char *msg )
{
    gstring msger;
    if( msg==0 )
        msger = "Error in translation of Math Script equations:\n ";
    else
        msger = gstring(msg);
    msger += erscan;
    msger += "\n  Would you like to make corrections? ";
    if( !vfQuestion(pImp, GetName() , msger ) )
        return false;
    pVisorImp->OpenModule(window(), nRT);
    return true;
    // fEdit = true; "Replace record in database files?"
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
        AddRecord( key );
    else
        if( onOld == true || vfQuestion(pImp, key,
                 "This data record already exists! Replace?") )
            db->Rep( Rnum );
    fEdit = false;
}

// Save record to DB file

void
TCModule::CmSave()
{
    try
    {
        if(  pVisor->ProfileMode == true &&
            ( nRT < RT_SYSEQ &&  nRT != RT_PARAM && nRT != RT_SDATA  ))
            Error( GetName(), "Please, do it in Database mode!");

        gstring str=db->PackKey();
        if( str.find_first_of("*?" ) != gstring::npos ) // udef key
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
        vfMessage(pImp, xcpt.title, xcpt.mess);
    }
}


// Save record to DB file as new

void
TCModule::CmSaveAs()
{
    try
    {
        if( pVisor->ProfileMode == true &&
           ( nRT < RT_SYSEQ &&  nRT != RT_PARAM && nRT != RT_SDATA ) )
            Error( GetName(), "Please, do it in Database mode!");

        gstring str=db->PackKey();
        str = GetKeyofRecord( str.c_str(),
                 "Insert new record keyed ", KEY_NEW );
        if(  str.empty() )
            return ;
        RecSave( str.c_str(), false );
        pVisor->Update( true );
    }
    catch( TError& xcpt )
    {
        vfMessage(pImp, xcpt.title, xcpt.mess);
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
        gstring str = " Record ";
        str += key;
        str += "\n not found to delete!";
        Error( GetName(), str.c_str() );
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

        if( pVisor->ProfileMode == true && nRT < RT_SYSEQ )
            Error( GetName(), "Please, do it in Database mode!");

        gstring str=db->PackKey();
        if( str.find_first_of("*?" ) != gstring::npos )
            Error( GetName(), "Current record key not defined!");
        if( !vfQuestion(pImp, GetName(),
                   "Confirm deletion of data record keyed "+str ))
            return;
        DeleteRecord( str.c_str() );
        fEdit = false;
    }
    catch( TError& xcpt )
    {
        vfMessage(pImp, xcpt.title, xcpt.mess);
    }
}

// Read record into memory structure

void
TCModule::RecInput( const char *key )
{
    int Rnum = db->Find( key );
    if( Rnum<0 )
    {
        gstring msg = "Record ";
        msg += gstring(key, 0, db->KeyLen());
        msg += " not found!" ;
        Error( GetName(), msg.c_str());
    }
    db->Get( Rnum );
    dyn_set();
    fEdit = false;
    //  pVisor->Update();
}

// Show another record from DB (Without check)
void
TCModule::CmShow()
{
    try
    {
        if( pVisor->ProfileMode == true &&
                ( nRT >= RT_SYSEQ  || nRT == RT_PARAM )  )
            Error( GetName(), "Illegal command in Profile mode!");

        MessageToSave();
        // get key of record
        gstring str = GetKeyofRecord(
      /*db->PackKey()*/0, "Select data record key ", KEY_OLD );
        if(  str.empty() )
            return ;
        RecInput( str.c_str() );
        SetTitle();
        pVisor->Update( true );
    }
    catch( TError& xcpt )
    {
        vfMessage(pImp, xcpt.title, xcpt.mess);
    }
}


// Set new Filter and select first records from list (Sveta 12/06/01)
void
TCModule::CmFilter()
{
    try
    {
        MessageToSave();
        gstring str = Filter;
        str = GetKeyofRecord( str.c_str(),
                         "Please, give a key template", KEY_TEMP );
        if(  str.empty() )
            return;

       TCStringArray aKey;
       TCIntArray anR;
       int Nrec = db->GetKeyList( Filter.c_str(), aKey, anR );
       if( Nrec >= 1 )
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
        vfMessage(pImp, xcpt.title, xcpt.mess);
    }
}

// Show next record from template list (Sveta 14/06/01)
void
TCModule::CmNext()
{
    try
    {
       int i_next = 0;
       MessageToSave();
       // get current record key
       gstring str=db->UnpackKey();
       // select scroll list
       TCStringArray aKey;
       TCIntArray anR;
       int Nrec = db->GetKeyList( Filter.c_str(), aKey, anR );
       if( Nrec <= 0 )
           return; // no records to scroll
       // get current record key
       if( !(str.find_first_of("*?" ) != gstring::npos) )
          //Current record key is defined!
       {
         for(uint i=0; i<aKey.GetCount(); i++ )
          if( str == aKey[i])
            {
              i_next = i+1;
              if( i_next >=  Nrec ) i_next--;
              break;
            }
        }
       if( pVisor->ProfileMode == true )
          RecordLoadinProfile( aKey[i_next].c_str() );
       else
          RecInput( aKey[i_next].c_str() );
       pVisor->Update( true );
    }
    catch( TError& xcpt )
    {
        vfMessage(pImp, xcpt.title, xcpt.mess);
    }
}

// Show previous record from template list (Sveta 14/06/01)
void
TCModule::CmPrevious()
{
    try
    {
       int i_next = 0;
       MessageToSave();
       // get current record key
       gstring str=db->UnpackKey();
       // select scroll list
       TCStringArray aKey;
       TCIntArray anR;
       int Nrec = db->GetKeyList( Filter.c_str(), aKey, anR );
       if( Nrec <= 0 )
           return; // no records to scroll
       if( !(str.find_first_of("*?" ) != gstring::npos) )
          //Current record key is defined!
       {
         for(uint i=0; i<aKey.GetCount(); i++ )
          if( str == aKey[i])
            {
              i_next = i-1;
              if( i_next <  0 ) i_next++;
              break;
            }
        }
       if( pVisor->ProfileMode == true )
          RecordLoadinProfile( aKey[i_next].c_str() );
       else
          RecInput( aKey[i_next].c_str() );
       pVisor->Update( true );
    }
    catch( TError& xcpt )
    {
        vfMessage(pImp, xcpt.title, xcpt.mess);
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
       bldType = vfQuestion3(pImp, "Reallocation of data arrays ",
                              GetName()+ gstring(" : ") + key ,
                              "&Bypass", "&Remake", "&Clear all");
    int retType = bldType;

    switch( bldType )
    {
    case VF3_3:    // VF3_1
        retType = VF3_1;
        dyn_kill();
        set_def(); // set default data or zero if necessary
    case VF3_2:
        if( !pImp )
        {    Show(pVisorImp, "Remaking...");

        }
        pImp->MakeQuery();

        if( pVisor->ProfileMode == true && nRT == RT_SYSEQ )
            pImp->CloseWin();

        fEdit = true;
        break;
    case VF3_1:   // VF3_3   - do nothing
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
        if( pVisor->ProfileMode != true && (nRT == RT_SYSEQ || nRT == RT_PROCES
                 || nRT == RT_PROBE  || nRT == RT_PARAM || nRT > RT_GTDEMO ) )
            Error( GetName(), "Please, do it in Profile mode!");
        if( pVisor->ProfileMode == true &&
             ( nRT < RT_SYSEQ && nRT != RT_SDATA ) )
            Error( GetName(), "Please, do it in Database mode!");

        MessageToSave();
        gstring str= gstring( db->UnpackKey(), 0, db->KeyLen() );
                    //db->PackKey();
        if( str.find_first_of("*?" ) != gstring::npos )
            Error( GetName(), "Current record key is not defined!");

        //check_input( db->UnpackKey() );
        RecBuild( str.c_str(), VF_REMAKE );
        SetString("Remake finished OK"
                  "It is recommended to re-calculate data");
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        vfMessage(pImp, xcpt.title, xcpt.mess);
    }
}


// Calculate record

void
TCModule::RecCalc( const char* )
{
    fEdit = true;
}

// Calc loading before record (error if current record undefined)

void
TCModule::CmCalc()
{
    try
    {
        if( pVisor->ProfileMode != true && (nRT == RT_SYSEQ || nRT == RT_PROCES
                                            || nRT == RT_PROBE  || nRT == RT_PARAM || nRT > RT_GTDEMO ) )
            Error( GetName(), "Please, do it in Profile mode!");
        if( pVisor->ProfileMode == true &&
             ( nRT < RT_SYSEQ && nRT != RT_SDATA ) )
            Error( GetName(), "Please, do it in Database mode!");

        MessageToSave();
        gstring str=db->PackKey();
        if( str.find_first_of("*?" ) != gstring::npos )
            Error( GetName(), "Current record key is not defined!");
        //int  Rnum = db->Find( str.c_str() );
        //ErrorIf( Rnum<0, GetKeywd(),
        // "Record to calculate not found!");
        // db->Get( Rnum ); must be done before
        // dyn_set(); must be done before
        str = gstring( db->UnpackKey(), 0, db->KeyLen() );
        check_input( str.c_str() );
        RecCalc( str.c_str() );
        SetString("Calculation finished OK");
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        vfMessage(pImp, xcpt.title, xcpt.mess);
    }
}

// Add new record to DB

void
TCModule::CmNew()
{
    try
    {
        if( nRT == RT_SYSEQ || nRT == RT_PROCES ||
                nRT == RT_PROBE  || nRT == RT_PARAM || nRT > RT_GTDEMO )
            Error( GetName(), "Please, do it in Profile mode!");

        MessageToSave();
        gstring str = GetKeyofRecord( db->PackKey(),
                             "Insert new key of data record", KEY_NEW);
        if(  str.empty() )
            return;
        int  Rnum = db->Find( str.c_str() );
        ErrorIf( Rnum>=0, GetName(), "This record alredy exist!");
        str = gstring( db->UnpackKey(), 0, db->KeyLen() );
        check_input( str.c_str() );
        RecBuild( str.c_str(), VF_REMAKE );
        SetString("Remake of the new record finished OK"
                  "It is recommended to re-calculate data");
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        vfMessage(pImp, xcpt.title, xcpt.mess);
    }
}

void
TCModule::CmCreate()
{
    try
    {
        if( nRT == RT_SYSEQ || nRT == RT_PROCES ||
                nRT == RT_PROBE  || nRT == RT_PARAM || nRT > RT_GTDEMO )
            Error( GetName(), "Please, do it in Profile mode!");

        MessageToSave();
        gstring str = GetKeyofRecord( db->PackKey(),
                             "Insert new key of data record", KEY_NEW);
        if(  str.empty() )
            return;
        int  Rnum = db->Find( str.c_str() );
        ErrorIf( Rnum>=0, GetName(), "This record alredy exist!");
        str = gstring( db->UnpackKey(), 0, db->KeyLen() );
        check_input( str.c_str() );
        RecBuild( str.c_str(), VF_CLEARALL );
        SetString("Remake of the new record finished OK"
                  "It is recommended to re-calculate data");
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        vfMessage(pImp, xcpt.title, xcpt.mess);
    }
}

/*
//Show next record from DB (and check relationship)
void
TCModule::CmFind() // ???? error in smShow
{
   try{
       if( nRT == RT_SYSEQ || nRT == RT_PROCES ||
            nRT == RT_PROBE  || nRT == RT_PARAM || nRT > RT_GTDEMO )
          Error( GetName(), "Do it in Profile mode!");
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
    vstr key( db->KeyLen(), _key);
    // gstring  key( _key, 0, db->KeyLen());

    MessageToSave();
    RecStatus iRet = db->Rtest( key, 1 );
    gstring msg;

    switch( iRet )
    {
    case MANY_:
        msg = "Record selection error! ";
        msg += key;
// Sveta 14/06/01        if( RecChoise( key ) == false )
            Error( GetName(), msg.c_str() );
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
            if(pVisor->ProfileMode == true)
                Error( GetName(), msg.c_str() );
            msg +=  "Create new record?";
            if( !vfQuestion(pImp, GetName(), msg ))
                Error( GetName(), "Record creation rejected!");
            gstring str = key.p;

            if( str.find_first_of("*?" ) != gstring::npos)  // pattern
                str = GetKeyofRecord( str.c_str(),
                       "Insert new key of data record", KEY_NEW);
            if(  str.empty() )
                Error( GetName(), "Record creation rejected!");
            int  Rnum = db->Find( str.c_str() );
            ErrorIf( Rnum>=0, GetName(), "A record with such key already exists!");
            pVisorImp->OpenModule(pVisorImp, nRT);
            gstring str1 = gstring( db->UnpackKey(), 0, db->KeyLen() );
            check_input( str1.c_str() );
            RecBuild( str.c_str() );
            SetString("Remake of the new record finished OK"
                      "It is recommended to re-calculate data");
            pVisor->Update();
            Error("Calculation failed!", "Check data fields and try calculation again!");
        } // break;
    case FAIL_:
        msg = "Failure!!! Database chain ";
        msg += GetName();
        msg += " is corrupt,\n"
               "Data Record key '";
        msg += key;
        msg += "'\n Try to unload or re-index this database chain...";
        Error( GetName(),  msg.c_str() );
    }
}

//----------------------------------------------------------
//--- Manipulation of the current record
//--- Process, Probe, GTdemo, Duterm in profile mode
//----------------------------------------------------------

// load record from data base (show in profile mode)

void TCModule::RecordLoadinProfile( const char *key )
{
    gstring str;

    if( key==0 )
    {
        str = gstring( rt[RT_PARAM].FldKey(0), 0, rt[RT_PARAM].FldLen(0) );
        str += ":"; //04/09/01 ????
        for( int i=1; i<db->KeyNumFlds(); i++)
            str += "*:";
        //      db->MakeKey( RT_PARAM, pkey, RT_PARAM, 0,
        // K_ANY, K_ANY, K_ANY, K_ANY,K_ANY, K_ANY, K_ANY, K_ANY, K_ANY,  K_END);
        str = GetKeyofRecord( str.c_str(), "Get a record key: ", KEY_OLD );
        // ErrorIf( str.empty(), GetName(), "Operation cancelled!");
    }
    else str=gstring(key);
    // get record
    if(  str.empty() )
            return ;
    RecInput( str.c_str() );
    if( check_input( str.c_str(), 0 ) ) // read and unpack base SyStat
        vfMessage(pImp, GetName(),
      "Warning: Startpoint SysEq record has been modified!" );
    //   pVisor->Update();
}


// loads the profile


void TCModule::CmLoadinProfile()
{
    try
    {
        if( nRT < RT_SYSEQ  )
            Error( GetName(),  "Please, do it in Database mode!");

        MessageToSave();
        RecordLoadinProfile();
        pVisor->Update(true);
    }
    catch( TError& xcpt )
    {
        vfMessage(pImp, xcpt.title, xcpt.mess);
    }
}


// Adds new record to DB in Profile mode

void
TCModule::CmNewinProfile()
{
    try
    {
        if( nRT < RT_SYSEQ  )
            Error( GetName(),  "Please, do it in Database mode!");
        MessageToSave();
        // Get record key
        gstring str = gstring( rt[RT_PARAM].FldKey(0), 0, rt[RT_PARAM].FldLen(0) );
        str += ":"; //04/09/01 ????
        for( int i=1; i<db->KeyNumFlds(); i++)
            str += "*:";
        str = GetKeyofRecord( str.c_str(), "Please, enter a new key", KEY_NEW );
        if(  str.empty() )
            return;
        int  Rnum = db->Find( str.c_str() );
        ErrorIf( Rnum>=0, GetName(), "This record alredy exists!");
        check_input( str.c_str(), 0 );
        RecBuild( str.c_str(), VF_REMAKE );
        SetString("Remake of the new record finished OK"
                  "It is recommended to re-calculate data");
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        vfMessage(pImp, xcpt.title, xcpt.mess);
    }
}

void
TCModule::CmCreateinProfile()
{
    try
    {
        if( nRT < RT_SYSEQ  )
            Error( GetName(),  "Please, do it in Database mode!");
        MessageToSave();
        // Get record key
        gstring str = gstring( rt[RT_PARAM].FldKey(0), 0, rt[RT_PARAM].FldLen(0) );
        str += ":"; //04/09/01 ????
        for( int i=1; i<db->KeyNumFlds(); i++)
            str += "*:";
        str = GetKeyofRecord( str.c_str(), "Please, enter a new key", KEY_NEW );
        if(  str.empty() )
            return;
        int  Rnum = db->Find( str.c_str() );
        ErrorIf( Rnum>=0, GetName(), "This record alredy exists!");
        check_input( str.c_str(), 0 );
        RecBuild( str.c_str(), VF_CLEARALL );
        SetString("Remake of the new record finished OK"
                  "It is recommended to re-calculate data");
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        vfMessage(pImp, xcpt.title, xcpt.mess);
    }
}


// virtual callback for plotting the record
// should be redefined in subclasses (modules)
// to make some plotting

void
TCModule::RecordPlot( const char* /*key*/ )
{
    vfMessage(pImp, GetName(), "Plotting?\n\nNot here, yet... Sorry!");
}


// callback for 'Plot' command

void
TCModule::CmPlot()
{
    try
    {
        MessageToSave();
        gstring str=db->PackKey();
        if( str.find_first_of("*?" ) != gstring::npos )
            Error( GetName(), "Current record key is not provided!");
        //int  Rnum = db->Find( str.c_str() );
        //ErrorIf( Rnum<0, GetName(), "Record to demonstrate not found!");
        RecordPlot( str.c_str() );
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        vfMessage(pImp, xcpt.title, xcpt.mess);
    }
}

void
TCModule::PrintSDref( const char* sd_key, char* text_fmt )
{
 // read sdref record with format prn
/* TSData::pm->RecInput( sd_key );
 char * text_fmt = TSData::pm->getAbstr();
 if( !text_fmt )
   Error( sd_key, "No format text in this record.");  */
 // open file to output
 gstring filename;
    if( vfChooseFileSave(pImp, filename, "Please, provide name of TXT-file") )
    {
        ios::openmode mod = ios::out;

        if( !(::access(filename.c_str(), 0 )) ) //file exists
            switch( vfQuestion3( pImp, filename.c_str(),
                             "This file exists! What to do?",
                                 "&Append", "&Overwrite", "&Cancel") )
            {
            case VF3_2:
                mod = ios::out;
                break;
            case VF3_1:
                mod = ios::app;
                break;
            case VF3_3:
                return;
            }
       fstream f( filename.c_str(), mod );
       ErrorIf( !f.good() , filename.c_str(), "Fileopen error");
      // scan and print format
      TPrintData dat( sd_key, nRT, f, text_fmt );
    }

}



// virtual callback for printing the record
// should be redefined in subclasses (modules)
// to make some plotting

void
TCModule::RecordPrint( const char* /*key*/ )
{
    vfMessage(pImp, GetName(), "Printing?\n\nNot here, yet... Sorry!");
}

// callback for 'print' command

void
TCModule::CmPrint()
{
    try
    {
        MessageToSave();
        /*gstring str=db->PackKey();
        if( str.find_first_of("*?" ) != gstring::npos )
            Error( GetName(), "Current record is not defined!");*/
        RecordPrint();
        SetString("Printing record finished OK");
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        vfMessage(pImp, xcpt.title, xcpt.mess);
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
        MessageToSave();
        pVisor->Message( pImp, GetName(), "Compressing database file \n"
         "Please, wait...", 0, 100 );

        db->RebildFile(SelectFileList(closef|openf));
        pVisorImp->CloseMessage();
        dyn_set();
        //        SetString("Compression finished OK");
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        vfMessage(pImp, xcpt.title, xcpt.mess);
    }
}

// Rename files from opend files list of data base

void
TCModule::CmAddFileToList()
{
    try
    {
        MessageToSave();

        gstring filename;
        if( vfChooseFileSave(pImp, filename,
                             "Put new Data Base file name" ) == false )
            return;
        // test Path Added Sveta 5/03/02
        // pdb extension, name must started db->GetKeywd()
        gstring dir;
        gstring name;
        gstring ext;
        u_splitpath( filename, dir, name, ext);
        ext = PDB_EXT;
        size_t pos = name.find( db->GetKeywd() );
        if( pos != 0 )
        {  name = "." + name;
           name = db->GetKeywd()+ name;
        }
        u_makepath( filename, dir, name, ext );

        TDBFile* file = new TDBFile(filename);
        /////////
        db->AddFileToList(file);
        db->SetKey( ALLKEY );
        //        SetString("Command finished OK");
        pVisor->Update(); // no objecs change, only title
    }

    catch( TError& xcpt )
    {
        vfMessage(pImp, xcpt.title, xcpt.mess);
    }
}

// Open new files from the list of closed files in the data base

void
TCModule::CmAddOpenFile()
{
    try
    {
        MessageToSave();
        db->AddOpenFile(SelectFileList(closef));
        db->SetKey( ALLKEY );
        //        SetString("Command finished OK");
        pVisor->Update(); // no objecs change, only title
    }

    catch( TError& xcpt )
    {
        vfMessage(pImp, xcpt.title, xcpt.mess);
    }
}

// Rebild the list of opened files for the data base

void
TCModule::CmReOpenFileList()
{
    try
    {
        MessageToSave();
        TCIntArray arr = SelectFileList(closef|openf|oldself);

        if( arr.GetCount() < 1 )
            if( !vfQuestion( pImp, GetName(),
                  "No database files selected to open! Continue?" ))
                return;

        db->Close();
        db->Open( true, UPDATE_DBV, arr );
        db->SetKey( ALLKEY );
        //       SetString("Command finished OK");
        pVisor->Update(); // no objecs change, only title
    }

    catch( TError& xcpt )
    {
        vfMessage(pImp, xcpt.title, xcpt.mess);
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
        MessageToSave();

        KeysToTXT( Filter.c_str() );
        db->SetKey( ALLKEY );
        //        SetString("Command finished OK");
        pVisor->Update(); // no objecs change, only title
    }

    catch( TError& xcpt )
    {
        vfMessage(pImp, xcpt.title, xcpt.mess);
    }
}

// Delete the list of records

void
TCModule::CmDeleteList()
{
    try
    {
        MessageToSave();
        DelList( Filter.c_str() );
        db->SetKey( ALLKEY );
    }
    catch( TError& xcpt )
    {
        vfMessage(pImp, xcpt.title, xcpt.mess);
    }
}


// Copy list command (Sveta 14/06/01)

void
TCModule::CmCopyList( )
{
    try
    {
        MessageToSave();
        CopyRecordsList( Filter.c_str(), false );

         dyn_set();
         pVisor->Update();
    }
    catch( TError& xcpt )
    {
        vfMessage(pImp, xcpt.title, xcpt.mess);
    }
}

void
TCModule::CmRenameList( )
{
    try
    {
        MessageToSave();
        CopyRecordsList( Filter.c_str(), true );

         dyn_set();
         pVisor->Update();
    }
    catch( TError& xcpt )
    {
        vfMessage(pImp, xcpt.title, xcpt.mess);
    }
}

// Transfer the list of records

void
TCModule::CmTransferList()
{
    try
    {
        MessageToSave();
        Transfer( Filter.c_str() );
        dyn_set();
        //       SetString("Command finished OK");
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        vfMessage(pImp, xcpt.title, xcpt.mess);
    }
}

// Export data to text file

void
TCModule::CmExport()
{
    try
    {
        MessageToSave();
        RecExport( Filter.c_str() );
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        vfMessage(pImp, xcpt.title, xcpt.mess);
    }
}

void
TCModule::CmBackup()
{
    try
    {
        MessageToSave();
        RecToTXT( Filter.c_str() );
        //  SetString("Command finished OK");
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        vfMessage(pImp, xcpt.title, xcpt.mess);
    }
}

// Import data from text file

void
TCModule::CmImport()
{
    try
    {
        MessageToSave();
        RecImport();
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        vfMessage(pImp, xcpt.title, xcpt.mess);
    }
}

void
TCModule::CmRestore()
{
    try
    {
        MessageToSave();
        RecOfTXT( );
        //     SetString("Command finished OK");
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        vfMessage(pImp, xcpt.title, xcpt.mess);
    }
}

// Adds the record

void
TCModule::AddRecord(const char* key )
{
    int file = db->fNum;

    ErrorIf(!key, "TCModule::AddRecord()", "empty record key!");

    if( strpbrk(key,"*?/")!=0 )
        Error( GetName(), "Attempt to insert record with template key!");

    ErrorIf( db->fOpenNameBuf.GetCount()<1, GetName(), "No database file choosen");
    if( db->fOpenNameBuf.GetCount()>1 )
    {
        gstring s="Choose a database file to put a record: "+ gstring(key);
        file = db->fOpenNameBuf.GetCount() - 1;  // 04.04.01 KD
        file = vfChoice(pImp, db->fOpenNameBuf, s.c_str(), file );
    }
    else
        file = 0;
    if( file >= 0)
        db->AddRecordToFile(key, file);
}

// Adds the record, or all records to file Sveta 15/06/01

void
TCModule::AddRecord(const char* key, int& fnum )
{
    int file = db->fNum;

    ErrorIf(!key, "TCModule::AddRecord()", "empty record key!");

    if( strpbrk(key,"*?/")!=0 )
        Error( GetName(), "Attempt to insert record with template key!");
    if( fnum >= 0 )
      file = fnum;
    else
    {
        ErrorIf( db->fOpenNameBuf.GetCount()<1, GetName(),
              "No database file choosen");
        if( db->fOpenNameBuf.GetCount()>1 )
        {
           bool ok_to_all = false;
           gstring s="Choose a database file to put a record: "
                      + gstring(key);
           file = db->fOpenNameBuf.GetCount() - 1;  // 04.04.01 KD
           file = vfChoice2(pImp, db->fOpenNameBuf, s.c_str(), file, ok_to_all );
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

// Unloades Data Record keys to txt-file

void
TCModule::KeysToTXT( const char *pattern )
{
    TCStringArray aKey = vfMultiKeys( pImp,
       "Please, mark record keys to be listed in txt-file",
       nRT, pattern );
    if( aKey.GetCount() <1 )
        return;

    gstring s = GetName();
    gstring filename;
    s += " : Please, select file to write record keys";
    if( !vfChooseFileSave(pImp, filename, s.c_str()) )
        return;
    fstream f(filename.c_str(), ios::out);
    ErrorIf( !f.good() , GetName(), "Fileopen error");

    // check for errors
    f << " " << GetName() << " \'" << pattern << "\' Nrec="
    << aKey.GetCount() << "\n";
    for(uint i=0; i<aKey.GetCount(); i++ )
    {
        f << aKey[i].c_str() << "\n";
    }
    ErrorIf( !f.good() , GetName(), "Writefile error");
}

// Unloads Data Record to txt-file
void
TCModule::RecToTXT( const char *pattern )
{
    TCStringArray aKey = vfMultiKeys( pImp,
       "Please, mark records to be unloaded into txt-file",
       nRT, pattern );
    if( aKey.GetCount() <1 )
        return;

    gstring s = GetName();
    gstring filename;
    s += " : Please, give a file name for unloading records";
    if( vfChooseFileSave( pImp, filename, s.c_str() ) == false )
        return;
    fstream f(filename.c_str(), ios::out);
    ErrorIf( !f.good() , GetName(), "File write error");

    for(uint i=0; i<aKey.GetCount(); i++ )
    {
       int Rnum = db->Find( aKey[i].c_str() );
       db->Get( Rnum );
       aObj[o_reckey].SetPtr( (void *)aKey[i].c_str());
       aObj[o_reckey].toTXT(f);
       for(int no=db->GetObjFirst(); no<db->GetObjFirst()+db->GetObjCount(); no++)
            aObj[no].toTXT(f);
    }

    ErrorIf( !f.good() , GetName(), "Filewrite error");
    dyn_set();
}

// Loads Data Records from txt-file

void
TCModule::RecOfTXT()
{
    vstr buf(150);
    int Rnum;
    int fnum= -1 ;// FileSelection dialog: implement "Ok to All"

    gstring s =gstring( GetName() )+" : Please, select file with unloaded records";
    gstring filename;
    if( vfChooseFileOpen( pImp, filename, s.c_str() ) == false )
        return;
    fstream f(filename.c_str(), ios::in);
    ErrorIf( !f.good() , GetName(), "Fileread error...");

    while( !f.eof() )
    {
        aObj[o_reckey].SetPtr(buf);
        aObj[o_reckey].ofTXT(f);
        for(int no=db->GetObjFirst(); no<db->GetObjFirst()+db->GetObjCount(); no++)
            aObj[no].ofTXT(f);
        buf[db->KeyLen()] = '\0';
        Rnum = db->Find( buf );
        if( Rnum >= 0 )
        {
           if( vfQuestion(pImp, buf.p,
               "Data record with this key already exists! Replace?"))
              db->Rep( Rnum);
        }
        else {  AddRecord( buf, fnum );
                if( fnum == -2 )
                  break;
             }
        s = gstring( buf );
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
      gstring str = "File read error! \n";
              str += "Last good record :";
              str += s;
      Error( GetName(), str.c_str() );
     }
     dyn_set();
}

// Unloads Data Record to user format
void
TCModule::RecExport( const char *pattern )
{

    // read sdref record with format prn
    gstring sd_key = "escript*:*:";
            sd_key += db->GetKeywd();
            sd_key += ":";
    sd_key = ((TCModule *)&aMod[RT_SDATA])->GetKeyofRecord(
          sd_key.c_str(), "Select key of escript format", KEY_OLD);
    if( sd_key.empty() )
     return;
    ((TCModule *)&aMod[RT_SDATA])->RecInput( sd_key.c_str() );
    char * text_fmt = (char *)aObj[o_sdabstr].GetPtr();
    if( !text_fmt )
       Error( sd_key.c_str(), "No format text in this record.");

    TCStringArray aKey = vfMultiKeys( pImp,
       "Please, mark records to be unloaded into txt-file",
       nRT, pattern );
    if( aKey.GetCount() <1 )
        return;

    gstring s = GetName();
    gstring filename;
    s += " : Please, give a file name for unloading records";
    if( vfChooseFileSave( pImp, filename, s.c_str() ) == false )
        return;
   ios::openmode mod = ios::out;
   if( !(::access(filename.c_str(), 0 )) ) //file exists
     switch( vfQuestion3( pImp, filename.c_str(),
                   "This file exists! What to do?",
                  "&Append", "&Overwrite", "&Cancel") )
     {
       case VF3_2:
                mod = ios::out;
                break;
      case VF3_1:
                mod = ios::app;
                break;
      case VF3_3:
                return;
     }
    fstream f(filename.c_str(), ios::out);
    ErrorIf( !f.good() , GetName(), "File write error");

    for(uint i=0; i<aKey.GetCount(); i++ )
    {
       int Rnum = db->Find( aKey[i].c_str() );
       db->Get( Rnum );
       TPrintData dat( sd_key.c_str(), nRT, f, text_fmt );
    }

    ErrorIf( !f.good() , GetName(), "Filewrite error");
    dyn_set();
}

// Loads Data Records from user format
void
TCModule::RecImport()
{
    vstr buf(150);
    int Rnum;
    int fnum= -1 ;// FileSelection dialog: implement "Ok to All"
    char ch;

    // read sdref record with format read
    gstring sd_key = "iscript*:*:";
            sd_key += db->GetKeywd();
            sd_key += ":";
    sd_key = ((TCModule *)&aMod[RT_SDATA])->GetKeyofRecord(
          sd_key.c_str(), "Select key of iscript format", KEY_OLD);
    if( sd_key.empty() )
     return;
    ((TCModule *)&aMod[RT_SDATA])->RecInput( sd_key.c_str() );
    char * text_fmt = (char *)aObj[o_sdabstr].GetPtr();
    if( !text_fmt )
       Error( sd_key.c_str(), "No format text in this record.");

    // translate scripts
    TReadData dat( sd_key.c_str(), nRT, text_fmt );

    gstring s =gstring( GetName() )+" : Please, select file with imported records";
    gstring filename;
    if( vfChooseFileOpen( pImp, filename, s.c_str() ) == false )
        return;
    fstream f(filename.c_str(), ios::in);
    ErrorIf( !f.good() , GetName(), "Fileread error...");

    int iter = 0;
    while( !f.eof() )
    {
        dat.readRecord( iter, f );
        gstring keyp = db->UnpackKey();
        Rnum = db->Find( keyp.c_str() );
        if( Rnum >= 0 )
        {
           if( vfQuestion(pImp, keyp.c_str(),
               "Data record with this key already exists! Replace?"))
              db->Rep( Rnum );
        }
        else {  AddRecord( keyp.c_str(), fnum );
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
      gstring str = "File read error! \n";
              str += "Last good record :";
              str += s;
      Error( GetName(), str.c_str() );
     }
     dyn_set();
}


// delete list of records from Data Base
void
TCModule::DelList( const char *pattern )
{
    TCStringArray aKey = vfMultiKeys( pImp,
       "Please, mark record keys to be deleted from database",
       nRT, pattern );
    int ichs = 1;

    for(uint i=0; i<aKey.GetCount(); i++ )
    {
        gstring str = "Please, confirm deleting record \nwith key: ";
        str += aKey[i];
        if( ichs )
        {
            switch( vfQuestion3(pImp, GetName(), str.c_str(),
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
        DeleteRecord( aKey[i].c_str(), false );
    }
}

// transfer list of records in Data Base to another file
// (ever used???)  Has to be re-implemented, indeed !

void
TCModule::Transfer( const char *pattern )
{
    int nrec = 0;
    int fnum= -1 ;// FileSelection dialog: implement "Ok to All"

    TCStringArray aKey = vfMultiKeys( pImp,
       "Please, mark record keys to be moved",
       nRT, pattern );


    for(uint i=0; i<aKey.GetCount(); i++ )
    {
        nrec = db->Find( aKey[i].c_str() );
        db->Get( nrec );
        /// !!!
        int oldfile = db->fNum;
        db->Del( nrec );
        AddRecord( aKey[i].c_str(), fnum );
        if( fnum == -2 )
        { db->AddRecordToFile( aKey[i].c_str(), oldfile );
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
    gstring str;

    if( if_rename )
     str = "Please, mark record keys to be renamed";
    else
     str = "Please, mark record keys to be copied";

    TCStringArray aKey = vfMultiKeys( pImp,
       str.c_str(), nRT, pattern );

    int rn_type = 0;
    switch (vfQuestYesNoCancel(pImp,
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

    gstring from_t;
    gstring to_t;
    if( rn_type == 0 )
    {
     if( !vfKeyCanged(pImp, "", from_t,  to_t ))
      return;
    }

    for(uint i=0; i<aKey.GetCount(); i++ )
    {
        nrec = db->Find( aKey[i].c_str() );
        db->Get( nrec );
        /// !!! changing record key
        if( rn_type ==  1)
        {
          str= aKey[i];
          str = GetKeyofRecord( str.c_str(),
                 "Insert new record key", KEY_NEW );
          if(  str.empty() )
            return ;
        }
        else
        {
          int fld = db->KeyNumFlds()-1;
          str= gstring(db->FldKey( fld ), 0, db->FldLen( fld ));
          ChangeforTempl( str, from_t, to_t, db->FldLen( fld ));
          str += ":";
          for(int ii=fld-1; ii>=0; ii--)
          {  gstring str1 =
               gstring(db->FldKey( ii ), 0, db->FldLen( ii ));
             str1.strip();
             str = str1 + ":" + str;
           }
        }
       if( if_rename )
       {
         fnum = db->fNum;
         db->Del( nrec );
         db->AddRecordToFile( str.c_str(), fnum );
       }
       else
       {
         AddRecord( str.c_str(), fnum );
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

    TCIntArray aSel = vfMultiChoiceSet(pImp, names, "Selection of files", sel);

    TCIntArray arr;
    for( uint i=0; i<aSel.GetCount(); i++ )
        arr.Add( indx[aSel[i]] );

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


// out     ========================
/*
// Unloads Data Record to txt-file

void
TCModule::RecToTXT( const char *pattern )
{
    int ichs;
    uint i;

    TCStringArray aKey = vfMultiKeys( pImp,
       "Please, mark records to be unloaded into txt-file",
       nRT, pattern );
    if( aKey.GetCount() <1 )
        return;

    gstring s = GetName();
    gstring filename;
    s += " : Please, give a file name for unloading records";
    if( vfChooseFileSave( pImp, filename, s.c_str() ) == false )
        return;
    fstream f(filename.c_str(), ios::out);
    ErrorIf( !f.good() , GetName(), "File write error");
    switch( vfQuestion3(pImp, GetName(), "Unload all marked records (Y) "
                        "or show/confirm each one (N)?","&All", "&Each one", "&Skip" ))
    {
    case VF3_2:
        ichs = 1;
        break;
    case VF3_1:
        ichs = 0;
        break;
    default:
        return;
    }

    i=0;
    while( i<aKey.GetCount() )
    {
       int Rnum = db->Find( aKey[i].c_str() );
       db->Get( Rnum );
        if( ichs )
        {
            pVisor->Update(); // no objecs change, only title
            switch( vfQuestion3(pImp, aKey[i], "Unload record?",
                                "&Yes", "&No", "&Unload All" ))
            {
            case VF3_3:
                ichs=0;
            case VF3_1:
                break;
            case VF3_2:
                i++;
                continue;
            }
        }
        aObj[o_reckey].SetPtr( (void *)aKey[i].c_str());
        aObj[o_reckey].toTXT(f);
        for(int no=db->GetObjFirst(); no<db->GetObjFirst()+db->GetObjCount(); no++)
            aObj[no].toTXT(f);
        i++;
    }
    ErrorIf( !f.good() , GetName(), "Filewrite error");

    dyn_set();
}

// Loads Data Records from txt-file

void
TCModule::RecOfTXT()
{
    vstr buf(150);
    int ichs, Rnum;
    int fnum= -1 ;// FileSelection dialog: implement "Ok to All"

    gstring s =gstring( GetName() )+" : Please, select file with unloaded records";
    gstring filename;
    if( vfChooseFileOpen( pImp, filename, s.c_str() ) == false )
        return;
    fstream f(filename.c_str(), ios::in);
    ErrorIf( !f.good() , GetName(), "Fileread error...");
    switch( vfQuestion3(pImp, GetName(), "Load all records (Y) "
                        "or show/confirm each one (N)?","&All", "&Each one", "&Skip" ))
    {
    case VF3_2:
        ichs = 1;
        break;
    case VF3_1:
        ichs = 0;
        break;
    default:
        return;
    }
    while( !f.eof() )
    {
        aObj[o_reckey].SetPtr(buf);
        aObj[o_reckey].ofTXT(f);
        for(int no=db->GetObjFirst(); no<db->GetObjFirst()+db->GetObjCount(); no++)
            aObj[no].ofTXT(f);
        buf[db->KeyLen()] = '\0';
        if( ichs )
        {
            db->SetKey( buf );
            pVisor->Update(); // no objecs change, only title
            switch( vfQuestion3(pImp, buf.p, "Load record?" ,
                                "&Yes", "&No", "&Load All" ))
            {
            case VF3_3:
                ichs=0;
            case VF3_1:
                Rnum = db->Find( buf );
                if( Rnum >= 0 )
                {
                    if( vfQuestion(pImp, buf.p,
                       "Data record with this key already exists! Replace?"))
                        db->Rep( Rnum);
                }
                else
                {  AddRecord( buf, fnum );
                   if( fnum == -2 )
                   break;
                 }
                break;
            case VF3_2: // db->SetKey( buf );
                break;
            }
        }
        else
        {
            Rnum = db->Find( buf );
            if( Rnum >= 0 )
            {
                if( vfQuestion(pImp, buf.p,
                   "Data record with this key already exists! Replace?"))
                    db->Rep( Rnum);
            }
            else {  AddRecord( buf, fnum );
                   if( fnum == -2 )
                    break;
                 }
        }
        do
        {
            f.get(buf[0]);
            if( !f.good() )
                break;
        }
        while( buf[0] == ' ' || buf[0]=='\n' );
        if( f.eof() )
            break;
        f.putback( buf[0] );
    }
    ErrorIf( f.bad() , GetName(), "File read error");

    dyn_set();
}

*/

//--------------------- End of v_module.cpp ---------------------------


