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

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include "v_module.h"
#include "visor.h"
#include "service.h"
#include "ms_multi_new.h"
#include "t_print.h"
#include "t_read.h"
#include "NewSystemDialog.h"
#include <spdlog/sinks/stdout_color_sinks.h>

// Thread-safe logger to stdout with colors
std::shared_ptr<spdlog::logger> gui_logger = spdlog::stdout_color_mt("gems3gui");

const std::string replace_question = "Data record with this key already exists! Replace?";


// Default constructor and destructor
TCModule::TCModule(size_t nrt):
        nRT(nrt),
        start_key_field_edit(0),
        contents_changed(false),
        state_line(),
        icon_file_path(),
        pImp(nullptr),
        nQ(1),
        Filter(ALLKEY),
        start_title(" "),
        db( nrt>MD_RMULTS? nullptr: rt[nrt].get())
{}

TCModule::~TCModule()
{}

void TCModule::clearEditFocus()
{
    if( pImp )  {
        pImp->clearFocus();
    }
}

size_t TCModule::keyEditField()
{
    if(nRT == RT_RTPARM || pVisor->ProfileMode)
        return start_key_field_edit;
    else return 0;
}

QWidget* TCModule::window()
{
      if( nRT== RT_SYSEQ && pVisor->ProfileMode )
       return dynamic_cast<QWidget*>(NewSystemDialog::pDia);
      else
       return dynamic_cast<QWidget*>(pImp);
}

// Updates window contents
// see also TModuleImp::Update, TCWindow::Update(),
// TCPage::Update(), TField::Update()
void TCModule::Update(bool force)
{
    if(pImp) {
       pImp->Update(force);
    }
}

// Updates contents of all windows plus caption of the current one
void TCModule::ModUpdate(const std::string& str)
{
    set_string(str);
    pVisor->Update();
}

// Open module window
void TCModule::Show(QWidget* parent, const char *str, bool viewmode)
{
    if(str) set_string(str);
    pVisor->OpenModule(parent, nRT, 0, !viewmode);
}

// Callback for 'Help' command
const char* TCModule::GetHtml()
{
   return GEMS_TOC_HTML;
}

/* opens window with 'Remake record' parameters
*/
void TCModule::MakeQuery()
{
    if(pImp) {
        pImp->MakeQuery();
    }
}

/*! returns true if user pressed 'save' or 'discard' and false on 'cancel' */
bool TCModule::MessageToSave()
{
    if(db==nullptr) {
        return true;
    }

    clearEditFocus();

    //--if( nRT != RT_SDATA &&
    //-- (pVisor->ProfileMode == true && nRT < RT_SYSEQ) )
    //--  return true;

    std::string key_str = db->PackKey();
    if( contents_changed && key_str.find_first_of("*?") == std::string::npos )
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
    contents_changed = false;

    return true;
}

// get key of record (existing key, new key or key temlate )
std::string TCModule::GetKeyofRecord(const char *oldKey, const char *strTitle, int keyType)
{
    if(db==nullptr) {
        return "";
    }

    std::string str = GetName();
    str += ": ";
    str += strTitle;

    std::string key;

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
          std::string stt = vfKeyTemplEdit(par/*window()*/, str.c_str(), nRT, key.c_str() );
          if( !stt.empty())
            Filter = stt;
          return Filter;
        }
    }
    Error( str, "Invalid record key editing mode");
    return "";
}

// make all keys filter
std::string  TCModule::makeKeyFilter()
{
    std::string strfilt;
    if(db==nullptr) {
        return strfilt;
    }

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
    std::string strfilt = char_array_to_string( rt[RT_PARAM]->FldKey(0), rt[RT_PARAM]->FldLen(0) );
    StripLine(strfilt);
    strfilt += ":";
    if(Filter.rfind(strfilt, 0) != 0)
     return true;
  }
  return false;
}

// Tools for correcting IPN Math Scripts
bool TCModule::CheckEqText(const char *erscan, const char *msg)
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
    pVisor->OpenModule(window(), nRT,0,true);

    return true;
    // contentsChanged = true; "Replace record in database files?"
}

//----------------------------------------------------------
//--- Manipulation of the current record
//----------------------------------------------------------

std::string TCModule::CurrentKey()
{
  std::string str=db->PackKey();
        if( str.find_first_of("*?" ) != std::string::npos
            || ( db->GetStatus()== UNDF_ && db->RecCount() &&
                nRT != RT_SDATA ))  // 09/11/2004 Sveta
            Error( GetName(), "E4 Cannot save under record key template, or record contents are not yet loaded!");
        return str;
}

// Save record structure

void TCModule::RecSave( const char *key, bool onOld )
{
    if(db==nullptr) {
        return;
    }

    int  Rnum = db->Find( key );
    if( Rnum<0 )
    {
       AddRecord( key );
       pVisorImp->defineModuleKeysList(nRT);
    }
    else
        if( onOld == true || vfQuestion(window(), key, replace_question) )
            db->Rep( Rnum );
    contents_changed = false;

    if( pVisor->ProfileMode && ( nRT < RT_SYSEQ &&  nRT != RT_PARAM && nRT != RT_SDATA ) )
      TMulti::sm->GetPM()->pTPD = -1; // to reload thermodynamic data base
}

// Save record to DB file
void TCModule::SaveM()
{
    if(db==nullptr) {
        return;
    }

       std::string str=db->PackKey();
       if( str.find_first_of("*?" ) != std::string::npos
           || ( db->GetStatus() == UNDF_  && db->RecCount() &&  // 09/11/2004 Sveta
           nRT != RT_SDATA ) )   // oct 2005  Sveta
             Error( GetName(), "E1 Cannot save under record key template, or record contents are not yet loaded!");
       SaveCurrentKey();
}

void TCModule::SaveCurrentKey()
{
    if(db==nullptr) {
        return;
    }
        std::string str = db->PackKey();
        if( str.find_first_of("*?" ) != std::string::npos )
        {
            str = GetKeyofRecord(str.c_str(), "Insert new record keyed ", KEY_NEW);
            if(  str.empty() )
                return ;
        }
        RecSave( str.c_str() );
}

void TCModule::SaveAs()
{
        if( pVisor->ProfileMode &&
           ( nRT < RT_SYSEQ &&  nRT != RT_PARAM && nRT != RT_SDATA && nRT != RT_PHASE) )
            Error( GetName(), "Please, do it in Database mode!");

        std::string str=db->PackKey();
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
}

// Delete record with key
void TCModule::DeleteRecord(const char *key, bool errifNo)
{
    if(db==nullptr) {
        return;
    }

    int  Rnum = db->Find( key );
    if( Rnum < 0 )
    {
        if( !errifNo )
            return;
        std::string str = " Record ";
        str += key;
        str += "\n not found to delete!";
        Error( GetName(), str );
    }
    db->Del( Rnum );
    contents_changed = false;
}

void TCModule::DeleteCurrent()
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
        contents_changed = false;
}

// Read record into memory structure
void TCModule::RecInput(const char *key)
{
    if(db==nullptr) {
        return;
    }

    int Rnum = db->Find( key );
    if( Rnum<0 )
    {
        std::string msg = "Record ";
        msg += std::string(key, 0, db->KeyLen());
        msg += " not found!" ;
        Error( GetName(), msg );
    }
    db->Get( Rnum );
    dyn_set();
    contents_changed = false;
    //  pVisor->Update();
}

// Set new Filter and select first records from list (Sveta 12/06/01)
void TCModule::RunFilter()
{
        std::string str = Filter;
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
}

// Show next record from template list (Sveta 14/06/01)
void TCModule::RunNext()
{
       size_t i_next = 0;

       // get current record key
       std::string str=db->UnpackKey();
       // select scroll list
       TCStringArray aKey;
       TCIntArray anR;
       auto Nrec = db->GetKeyList( Filter.c_str(), aKey, anR );
       if( Nrec <= 0 )
           return; // no records to scroll
       // get current record key
       if( !(str.find_first_of("*?" ) != std::string::npos) )
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
}

// Show previous record from template list (Sveta 14/06/01)
void TCModule::RunPrevious()
{
       size_t i_next = 0;

       // get current record key
       std::string str=db->UnpackKey();
       // select scroll list
       TCStringArray aKey;
       TCIntArray anR;
       auto Nrec = db->GetKeyList( Filter.c_str(), aKey, anR );
       if( Nrec <= 0 )
           return; // no records to scroll
       if( !(str.find_first_of("*?" ) != std::string::npos) )
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
}



// Rebuild dialog for the record structure before calc
// To be converted into wizard form
int TCModule::RecBuild(const char *key, int mode)
{
    if(db==nullptr) {
        return VF3_3;
    }

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
           contents_changed = true;
           break;
         }
        [[fallthrough]];
    case VF3_2:  // =VF_REMAKE
        MakeQuery();
        contents_changed = true;
        break;
    case VF3_1:   // =VF_BYPASS   - do nothing
        retType = VF3_3;
        break;
    }
    return  retType;
    // rebuild text
}

// Rebuild loading before record (error if current record undefined)
void TCModule::RunDerive()
{
        if( !pVisor->ProfileMode && ( nRT == RT_PARAM || nRT >= RT_SYSEQ ) )
            Error( GetName(), "Please, do it in Project mode!");
        if( pVisor->ProfileMode &&
             ( nRT < RT_SYSEQ && nRT != RT_SDATA ) )
            Error( GetName(), "Please, do it in Database mode!");

        std::string str = char_array_to_string( db->UnpackKey(), db->KeyLen() );
                    //db->PackKey();
        if( str.find_first_of("*?" ) != std::string::npos
            || ( db->GetStatus() == UNDF_ && nRT != RT_SDATA) )   // 09/11/2004 Sveta
            Error( GetName(), "E2 Cannot save under record key template, or record contents are not yet loaded!");

        check_input( db->UnpackKey(), 0 );  // SD 18/11/2008
        RecBuild( str.c_str(), VF_REMAKE );
        set_string("Remake finished OK. "
                  " It is recommended to re-calculate the data.");
}


// Calculate record
void TCModule::RecCalc(const char*)
{
    contents_changed = true;
}

// Calc loading before record (error if current record undefined)
void  TCModule::RunCalc()
{
        if( !pVisor->ProfileMode && ( nRT == RT_PARAM || nRT >= RT_SYSEQ ) )
            Error( GetName(), "Please, do it in Project mode!");

        std::string str=db->PackKey();
        if( str.find_first_of("*?" ) != std::string::npos
               || ( db->GetStatus()== UNDF_ && db->RecCount()
                 && nRT != RT_SDATA ))  // 09/11/2004 Sveta
            Error( GetName(), "E3 Cannot save under record key template, or record contents are not yet loaded!");
        str = char_array_to_string( db->UnpackKey(), db->KeyLen() );
        check_input( str.c_str() );
        set_string("Calculation... ");
        clock_t t_start11, t_end11;
        t_start11 = clock();

        RecCalc( str.c_str() );

        // 19/02/2007 Sveta
        t_end11 = clock();
        clock_t dtime = ( t_end11- t_start11 );
        char  buf[200];
        sprintf(buf, "Calculation finished OK (elapsed time: %lg s).",
                static_cast<double>(dtime)/CLOCKS_PER_SEC);
        set_string(buf);
}

// Add new record to DB

void TCModule::RunNew()
{
        if(  nRT == RT_PARAM || nRT >= RT_SYSEQ )
            Error( GetName(), "Please, do it in Project mode!");

       std::string dlgName = "Please, set a new record key ";
       std::string str = db->PackKey();
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
        set_string("Remake of the new record finished OK. "
                  " It is recommended to re-calculate the data.");
}

void TCModule::RunCreate()
{
        if(  nRT == RT_PARAM || nRT >= RT_SYSEQ )
            Error( GetName(), "Please, do it in Project mode!");

        std::string dlgName = "Please, set a new record key ";
        std::string str = db->PackKey();
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
        set_string("Remake of the new record finished OK. "
                  " It is recommended to re-calculate the data");
}

// Test record with key
void TCModule::TryRecInp(const char *_key, time_t& time_s, int q, bool save)
{
    if(db==nullptr) {
        return;
    }

    std::string  key( _key, 0, db->KeyLen() );

    if(save) {
      if( !MessageToSave() )
         return;
    }

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
            std::string str = key;

            if( str.find_first_of("*?" ) != std::string::npos)  // pattern
                str = GetKeyofRecord( str.c_str(),
                       "Please, set a new record key ", KEY_NEW);
            if(  str.empty() )
                Error( GetName(), "Record creation rejected!");
            int  Rnum = db->Find( str.c_str() );
            ErrorIf( Rnum>=0, GetName(), "A record with such key already exists!");
            pVisor->OpenModule(pVisorImp, nRT);
            std::string str1 = char_array_to_string( db->UnpackKey(), db->KeyLen() );
            check_input( str1.c_str() );
            RecBuild( str.c_str() );
            set_string("Remake of the new record finished OK. "
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
    std::string str;

    if( key==nullptr )
    {
        str = getFilter();
        str = GetKeyofRecord( str.c_str(), "Please, select a record key ", KEY_OLD );
    }
    else str=std::string(key);
    // get record
    if( str.empty() )
            return;
    RecInput( str.c_str() );
    check_input( str.c_str(), 0 );
}

void TCModule::NewinProfile()
{
        if( nRT < RT_SYSEQ  && nRT != RT_SDATA )
            Error( GetName(),  "Please, do it in Database mode!");

        // Get record key
        std::string str = makeKeyFilter();
        std::string dlgName = "Please, set a new record key ";
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
        set_string("Remake of the new record finished OK. "
                  " It is recommended to re-calculate the data.");
}

void TCModule::CreateinProfile()
{
        if( nRT < RT_SYSEQ && nRT != RT_SDATA )
            Error( GetName(),  "Please, do it in Database mode!");

        // Get record key
        std::string str = makeKeyFilter();
        std::string dlgName = "Please, set a new record key ";
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
        set_string("Remake of the new record finished OK. "
                  " It is recommended to re-calculate data");
}

// virtual callback for plotting the record
// should be redefined in subclasses (modules)
// to make some plotting
void TCModule::RecordPlot( const char* /*key*/ )
{
    vfMessage(window(), GetName(), "Plotting?\n\nNot here, yet... Sorry!");
}

void TCModule::PrintSDref( const char* sd_key, const char* text_fmt )
{
    if(db==nullptr) {
        return;
    }

 // read sdref record with format prn
/* TSData::pm->RecInput( sd_key );
 char * text_fmt = TSData::pm->getAbstr();
 if( !text_fmt )
   Error( sd_key, "No format text in this record.");  */
 // open file to output

    std::string filename="";
    if( vfChooseFileSave(window(), filename, "Please, provide name of TXT-file") )
    {
        std::ios::openmode mod = std::ios::out;

        if(vfExist(filename))
            switch( vfQuestion3( window(), filename,
                             "This file exists! What to do?",
                                 "&Append", "&Overwrite", "&Cancel") )
            {
            case VF3_2:
                mod = std::ios::out;
                break;
            case VF3_1:
                mod = std::ios::out|std::ios::app;
                break;
            case VF3_3:
                return;
            }
       std::fstream f( filename, mod );
       ErrorIf( !f.good() , filename, "Fileopen error");
      // scan and print format
      TPrintData dat( sd_key, nRT, f, text_fmt );
    }

}


// virtual callback for printing the record
// should be redefined in subclasses (modules)
// to make some plotting
void TCModule::RecordPrint(const char* key)
{
    if(db==nullptr) {
        return;
    }

  // select  SDref key
 std::string sd_key;
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
 if( sd_key.find_first_of("*?" ) != std::string::npos )
 {
     sd_key = aMod[RT_SDATA]->GetKeyofRecord(
          sd_key.c_str(), "Please, select a print script", KEY_OLD);
 }

 if( sd_key.empty() )
     return;

  aMod[RT_SDATA]->RecInput( sd_key.c_str() );
  const char * text_fmt = static_cast<const char *>(aObj[o_sdabstr]->GetPtr());
  if( !text_fmt )
       Error( sd_key, "No print script in this record.");

  PrintSDref( sd_key.c_str(), text_fmt );
}

//----------------------------------------------------------
//--- Manipulation with database files   (Service functions )
//----------------------------------------------------------

// Index files of data base
void TCModule::RebildFile()
{
        pVisor->Message( window(), GetName(), "Compressing database file(s). "
         "Please, wait...", 0, 100 );

        db->RebildFile(SelectFileList(closef|openf));
        // pVisorImp->CloseMessage();
        dyn_set();
        //        SetString("Compression finished OK");
}

// Rename files from opend files list of data base
void TCModule::AddFileToList()
{

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
}

// Open new files from the list of closed files in the data base
void TCModule::AddOpenFile()
{
        db->AddOpenFile(SelectFileList(closef));
        db->SetKey( ALLKEY );
        dyn_set();
        //        SetString("Command finished OK");
}

// Rebuild the list of opened files for the data base
void TCModule::ReOpenFileList()
{

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
}

// Adds the record
void TCModule::AddRecord(const char* key )
{
    if(db==nullptr) {
        return;
    }

    int file = db->fNum;

    ErrorIf(!key, "TCModule::AddRecord()", "empty record key!");

    if( strpbrk(key,"*?/")!=nullptr )
        Error( GetName(), "Attempt to insert record with template key!");

    ErrorIf( db->fOpenNameBuf.size()<1, GetName(), "No database file choosen");
    if( db->fOpenNameBuf.size()>1 )
    {
        std::string s="Choose a database file to put a record: "+ std::string(key);
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
    if(db==nullptr) {
        return;
    }

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
           std::string s="Choose a database file to put a record: "
                      + std::string(key);
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
    if(db==nullptr) {
        return 0;
    }

    int  Rnum;
    std::string str = key;

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

void TCModule::KeysToTXT()
{
    TCStringArray aKey = vfMultiKeys( window(),
       "Please, mark record keys to be listed in txt-file",
       nRT, Filter.c_str() );
    if( aKey.size() <1 )
        return;

    std::string s = GetName();
    std::string filename;
    s += " : Please, select file to write record keys";
    if( !vfChooseFileSave(window(), filename, s.c_str()) )
        return;
    std::fstream f(filename, std::ios::out);
    ErrorIf( !f.good() , GetName(), "Fileopen error");

    // check for errors
    f << " " << GetName() << " \'" << Filter << "\' Nrec="  << aKey.size() << "\n";
    for(size_t i=0; i<aKey.size(); i++ )
    {
        f << aKey[i].c_str() << "\n";
    }
    ErrorIf( !f.good() , GetName(), "Writefile error");
}

// Unloads Data Record to txt-file
void TCModule::RecToTXT()
{
    TCStringArray aKey = vfMultiKeys( window(),
       "Please, mark records to be unloaded into txt-file",
       nRT, Filter.c_str() );
    if( aKey.size() <1 )
        return;

    std::string s = GetName();
    std::string filename;
    s += " : Please, give a file name for unloading records";
    if( vfChooseFileSave( window(), filename, s.c_str() ) == false )
        return;
    std::fstream f(filename.c_str(), std::ios::out);
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
    db->SetKey( ALLKEY );
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
    std::fstream f(filename.c_str(), std::ios::in);
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
void TCModule::RecExport()
{
    // read sdref record with format prn
    std::string sd_key = "escript*:*:";
            sd_key += db->GetKeywd();
            sd_key += "*";
            sd_key += ":";
    sd_key = aMod[RT_SDATA]->GetKeyofRecord(
          sd_key.c_str(), "Select key of escript format", KEY_OLD);
    if( sd_key.empty() )
     return;
    aMod[RT_SDATA]->RecInput( sd_key.c_str() );
    char * text_fmt = static_cast<char *>(aObj[o_sdabstr]->GetPtr());
    if( !text_fmt )
       Error( sd_key, "No format text in this record.");

    TCStringArray aKey = vfMultiKeys( window(),
       "Please, mark records to be unloaded into txt-file",
       nRT, Filter.c_str() );
    if( aKey.size() <1 )
        return;

    std::string s = GetName();
    std::string filename;
    s += " : Please, give a file name for unloading records";
    if( vfChooseFileSave( window(), filename, s.c_str() ) == false )
        return;
   std::ios::openmode mod = std::ios::out;
   if(vfExist(filename))
     switch( vfQuestion3( window(), filename,
                   "This file exists! What to do?",
                  "&Append", "&Overwrite", "&Cancel") )
     {
       case VF3_2:
                mod = std::ios::out;
                break;
      case VF3_1:
                mod = std::ios::out|std::ios::app;
                break;
      case VF3_3:
                return;
     }
    std::fstream f(filename.c_str(), mod);
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
    std::string sd_key = "iscript*:*:";
            sd_key += db->GetKeywd();
            sd_key += "*";
            sd_key += ":";
    sd_key = aMod[RT_SDATA]->GetKeyofRecord(
          sd_key.c_str(), "Select key of iscript format", KEY_OLD);
    if( sd_key.empty() )
     return;
    aMod[RT_SDATA]->RecInput( sd_key.c_str() );
    char * text_fmt = static_cast<char *>(aObj[o_sdabstr]->GetPtr());
    if( !text_fmt )
       Error( sd_key, "No format text in this record.");

    // translate scripts
    TReadData dat( sd_key.c_str(), nRT, text_fmt );

    std::string s =std::string( GetName() )+" : Please, select file with imported records";
    std::string filename;
    if( vfChooseFileOpen( window(), filename, s.c_str() ) == false )
        return;
    std::fstream f(filename.c_str(), std::ios::in);
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

void TCModule::RecListToJSON()
{
    TCStringArray aKey = vfMultiKeys( window(),
                                      "Please, mark records to be unloaded to JSON",
                                      nRT, Filter.c_str() );
    if( aKey.size() <1 )
        return;

    std::string s = GetName();
    std::string filename = GetName();
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
    std::fstream f_out( filename, std::ios::out );
    if( f_out.good() )
        f_out << saveDoc.toJson().data() << std::endl;
    dyn_set();
}

void TCModule::CurrentToJSON(const std::string& filename)
{
    if(db==nullptr) {
        return;
    }

    QJsonObject recObject;
    db->toJsonObject( recObject );
    QJsonDocument saveDoc(recObject);
    std::fstream f_out( filename, std::ios::out );
    if( f_out.good() )
        f_out << saveDoc.toJson().data() << std::endl;
}

void TCModule::RecListFromJSON()
{
    int fnum= -1 ;// FileSelection dialog: implement "Ok to All"

    // Choose file name
    std::string s =std::string( GetName() )+" : Please, select file with unloaded records";
    std::string filename;
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
void TCModule::DelList()
{
    TCStringArray aKey = vfMultiKeys( window(),
       "Please, mark record keys to be deleted from database",
       nRT, Filter.c_str() );
    int ichs = 1;

    for(size_t i=0; i<aKey.size(); i++ )
    {
        std::string str = "Please, confirm deleting record \nwith key: ";
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
    db->SetKey( ALLKEY );
}

// transfer list of records in Data Base to another file
// (ever used???)  Has to be re-implemented, indeed !
void TCModule::Transfer()
{
    int nrec = 0;
    int fnum= -1 ;// FileSelection dialog: implement "Ok to All"

    TCStringArray aKey = vfMultiKeys( window(),
       "Please, mark record keys to be moved",
       nRT, Filter.c_str() );


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

    dyn_set();
}

// copy list of records in Data Base to another file
// (Sveta 15/06/01)
void TCModule::CopyRecordsList(bool if_rename)
{
    int nrec = 0;
    int fnum= -1 ;// FileSelection dialog: implement "Ok to All"
    std::string str;

    if( if_rename )
     str = "Please, mark record keys to be renamed";
    else
     str = "Please, mark record keys to be copied";

    TCStringArray aKey = vfMultiKeys( window(),
       str.c_str(), nRT, Filter.c_str() );

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
    std::string from_t;
    std::string to_t;
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
          std::string str1;
          str = "";
          for(uint ii=0; ii<fld; ii++)
          {
             str1 = std::string(db->FldKey( ii ), 0, db->FldLen( ii ));
             strip( str1 );
             str += str1 + ":";
           }
          str1= std::string(db->FldKey( fld ), 0, db->FldLen( fld ));
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
    dyn_set();
}

TCIntArray TCModule::SelectFileList(int mode)
{
    TCStringArray names;
    TCIntArray indx;
    TCIntArray sel;

    if(db==nullptr) {
        return sel;
    }

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
{}

// public list of modules
TModuleList aMod;

//--------------------- End of v_module.cpp ---------------------------


