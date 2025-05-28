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
#include "v_module.h"
#include "visor.h"
#include "service.h"
#include "ms_multi_new.h"
#include "t_print.h"
//#include "t_read.h"
#include "NewSystemDialog.h"
#include <spdlog/sinks/stdout_color_sinks.h>

// Thread-safe logger to stdout with colors
std::shared_ptr<spdlog::logger> gui_logger = spdlog::stdout_color_mt("gems3gui");


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
        return false;
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
        if( onOld == true || vfQuestion(window(), key,
                                    "Data record with this key already exists! Replace?") )
            db->Rep( Rnum );
    contents_changed = false;

    if( pVisor->ProfileMode && ( nRT < RT_SYSEQ &&  nRT != RT_PARAM && nRT != RT_SDATA ) )
      TMulti::sm->GetPM()->pTPD = -1; // to reload thermodynamic data base
}

void TCModule::CmSave()
{
    if(db==nullptr) {
        return;
    }

    try
    {
        clearEditFocus();
        //--if(  pVisor->ProfileMode == true &&
           //--( nRT < RT_SYSEQ &&  nRT != RT_PARAM && nRT != RT_SDATA  ))
           //--Error( GetName(), "Please, do it in Database mode!");

        std::string str = db->PackKey();
        if( str.find_first_of("*?" ) != std::string::npos )
        {
            str = GetKeyofRecord(str.c_str(), "Insert new record keyed ", KEY_NEW);
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

// Calculate record
void TCModule::RecCalc(const char*)
{
    contents_changed = true;
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


