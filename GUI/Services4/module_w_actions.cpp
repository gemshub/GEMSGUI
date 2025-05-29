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


#include <ctime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

#include "model_w.h"

#include "v_module.h"
#include "visor.h"
#include "service.h"
#include "v_mod.h"

void TCModuleImp::clearFocus()
{
        auto* focus_w = focusWidget();
        TCellInput* cell_w = dynamic_cast<TCellInput*>(focus_w);
        if( cell_w ) {
            cell_w->clearFocus();
        }
        TCellText* cell_t = dynamic_cast<TCellText*>(focus_w);
        if( cell_t ) {
            cell_t->clearFocus();
        }
}

void TCModuleImp::CmHelp()
{
    if(pVisor->ProfileMode && rMod.rtNum() == RT_PARAM)
         pVisorImp->OpenHelp( rMod.GetHtml(), NUMSET );
    else
         pVisorImp->OpenHelp( rMod.GetHtml() );
}

void TCModuleImp::CmHelp2()
{
   pVisorImp->OpenHelp( rMod.GetHtml(), MWPAGE, curPage()+1 );
}

/*! returns true if we can close the window
*/
bool TCModuleImp::EvClose()
{
    try  {
        return rMod.MessageToSave();
    }
    catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
    return false;	// can close on error ??
}



//----------------------------------------------------------
//--- Manipulation of the current record
//----------------------------------------------------------


// Save record to DB file
void TCModuleImp::CmSaveM()
{
  try{
      if(rMod.IsSubModule()) {
          return;
        }
       rMod.SaveM();
     }
   catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void TCModuleImp::CmSave()
{
    try{
        if(rMod.IsSubModule()) {
            return;
        }
        rMod.clearEditFocus();
        rMod.SaveCurrentKey();
    }
    catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

// Save record to DB file as new

void TCModuleImp::CmSaveAs()
{
    try
    {
        if(rMod.IsSubModule()) {
            return;
        }

        rMod.clearEditFocus();
        rMod.SaveAs();
        pVisor->Update( true );
    }
    catch( TError& xcpt )
    {
        pVisor->Update( true );
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}


// Delete current record

void TCModuleImp::CmDelete()
{
    try
    {
        if(rMod.IsSubModule()) {
            return;
        }
        rMod.DeleteCurrent();
    }
    catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

// Show another record from DB (Without check)
void TCModuleImp::CmShow( const char *key )
{
    try
    {
        if(rMod.IsSubModule()) {
            return;
        }

        if( pVisor->ProfileMode &&
                ( rMod.rtNum() >= RT_SYSEQ || rMod.rtNum() == RT_PARAM )  )
            Error( rMod.GetName(), "Invalid command in Project mode!");

        if( !rMod.MessageToSave() )
	    return;

    	// get key of record
        string str;
        if( key == nullptr )
        { str = rMod.GetKeyofRecord(
          /*db->PackKey()*/nullptr, "Select data record key ", KEY_OLD );
          if( str.empty() )
    	    return;
        }
        else  str = string(key);

        rMod.RecInput( str.c_str() );
    	pVisor->Update( true );
    }
    catch( TError& xcpt )
    {
        pVisor->Update( true );
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

// Set new Filter and select first records from list (Sveta 12/06/01)
void TCModuleImp::CmFilter()
{
    try
    {
        if(rMod.IsSubModule()) {
            return;
        }

        if( !rMod.MessageToSave() )
	    return;

        rMod.RunFilter();
       pVisor->Update( true );
    }
    catch( TError& xcpt )
    {
        pVisor->Update( true );
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

// Show next record from template list (Sveta 14/06/01)
void
TCModuleImp::CmNext()
{
    try
    {
        if(rMod.IsSubModule()) {
            return;
        }

        if( rMod.MessageToSave() ) {
         rMod.RunNext();
         pVisor->Update( true );
        }
    }
    catch( TError& xcpt )
    {
        pVisor->Update( true );
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

// Show previous record from template list (Sveta 14/06/01)
void
TCModuleImp::CmPrevious()
{
    try
    {
        if(rMod.IsSubModule()) {
            return;
        }

        if( rMod.MessageToSave() ) {
            rMod.RunPrevious();
            pVisor->Update( true );
        }
    }
    catch( TError& xcpt )
    {
        pVisor->Update( true );
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}


// Rebuild loading before record (error if current record undefined)
void
TCModuleImp::CmDerive()
{
    try
    {
        if(rMod.IsSubModule()) {
            return;
        }
        if( rMod.MessageToSave() ) {
            rMod.RunDerive();
            pVisor->Update();
        }
    }
    catch( TError& xcpt )
    {
        pVisor->Update( true );
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}


// Calc loading before record (error if current record undefined)
void TCModuleImp::CmCalc()
{
    try
    {
        if(rMod.IsSubModule()) {
            return;
        }

        rMod.clearEditFocus();
        rMod.RunCalc();
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        pVisor->Update( true );
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

// Add new record to DB
void TCModuleImp::CmNew()
{
    try
    {
        if(rMod.IsSubModule()) {
            return;
        }

        if( rMod.MessageToSave() ) {
            rMod.RunNew();
            pVisor->Update();
        }
    }
    catch( TError& xcpt )
    {
        pVisor->Update( true );
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void TCModuleImp::CmCreate()
{
    try
    {
        if(rMod.IsSubModule()) {
            return;
        }


        if( rMod.MessageToSave() ) {
            rMod.RunCreate();
            pVisor->Update();
        }
    }
    catch( TError& xcpt )
    {
        pVisor->Update( true );
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}


//----------------------------------------------------------
//--- Manipulation of the current record
//--- Process, UnSpace, GTdemo, Duterm in project mode
//----------------------------------------------------------


// loads the project
void TCModuleImp::CmLoadinProfile(const char *key)
{
    try
    {
        if(rMod.IsSubModule()) {
            return;
        }

        if( rMod.rtNum() < RT_SYSEQ && rMod.rtNum() != RT_SDATA)
        {
          //  Error( GetName(),  "Please, do it in Database mode!");
          CmShow(key);
          return;
        }

        if( !rMod.MessageToSave() )
	    return;
        rMod.RecordLoadinProfile(key);
        pVisor->Update(true);
    }
    catch( TError& xcpt )
    {
        pVisor->Update( true );
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

// Adds new record to DB in Project mode
void TCModuleImp::CmNewinProfile()
{
    try
    {
        if(rMod.IsSubModule()) {
            return;
        }

        if( rMod.MessageToSave() ) {
            rMod.NewinProfile();
            pVisor->Update();
        }
    }
    catch( TError& xcpt )
    {
        pVisor->Update( true );
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void TCModuleImp::CmCreateinProfile()
{
    try
    {
        if(rMod.IsSubModule()) {
            return;
        }

        if( rMod.MessageToSave() ) {
            rMod.CreateinProfile();
            pVisor->Update();
        }
    }
    catch( TError& xcpt )
    {
        pVisor->Update( true );
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}


void TCModuleImp::CmPlot()
{
    try
    {
        if(rMod.IsSubModule()) {
            return;
        }

        if( !rMod.MessageToSave() )
	    return;

        string str=rMod.CurrentKey();
        rMod.RecordPlot( str.c_str() );
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void TCModuleImp::CmPrint()
{
    try
    {
        if(rMod.IsSubModule()) {
            return;
        }

        if( !rMod.MessageToSave() )
	    return;

        /*string str=db->PackKey();
        if( str.find_first_of("*?" ) != string::npos )
            Error( GetName(), "Current record is not defined!");*/
        rMod.RecordPrint();
        rMod.SetString("Printing of a record finished OK. ");
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void TCModuleImp::CmScript()
{
    try
    {
        // read sdref record with format prn
      string sd_key = "?script*:*:";

      if( rMod.rtNum() < MD_RMULTS )
        sd_key += rMod.DBKeywd();
      else
      {
        sd_key += rMod.GetName();
      }

      sd_key += "*";
      sd_key += ":";
      sd_key =aMod[RT_SDATA]->GetKeyofRecord(
          sd_key.c_str(), "Please, select an appropriate script", KEY_OLD);
      if( sd_key.empty() )
           return;
      aMod[RT_SDATA]->RecInput( sd_key.c_str() );
       /*if( pImp )
           pVisorImp->OpenModule(pImp->topLevelWidget(), RT_SDATA);
       else*/ pVisor->OpenModule(nullptr, RT_SDATA,0,true);  // KD: workaround for NewSystemDialog
      aMod[RT_SDATA]->Update();
    }
    catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

//----------------------------------------------------------
//--- Manipulation with database files   (Service functions )
//----------------------------------------------------------

// Index files of data base
void TCModuleImp::CmRebildFile()
{
    try
    {
        if(rMod.IsSubModule()) {
            return;
        }

        if( !rMod.MessageToSave() )
	    return;

        rMod.RebildFile();
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        pVisor->Update();
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

// Rename files from opend files list of data base
void TCModuleImp::CmAddFileToList()
{
    try
    {
        if( !rMod.MessageToSave() )
	    return;

        rMod.AddFileToList();
        pVisor->Update(); // no objecs change, only title
    }

    catch( TError& xcpt )
    {
        pVisor->Update();
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

// Open new files from the list of closed files in the data base
void TCModuleImp::CmAddOpenFile()
{
    try
    {
        if( !rMod.MessageToSave() )
	    return;
        rMod.AddOpenFile();
        pVisor->Update(); // no objecs change, only title
    }
    catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

// Rebuild the list of opened files for the data base
void TCModuleImp::CmReOpenFileList()
{
    try
    {
        if( !rMod.MessageToSave() )
	    return;

        rMod.ReOpenFileList();
        pVisor->Update(); // no objecs change, only title
    }
    catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

//----------------------------------------------------------
//--- Manipulation with list of records
//----------------------------------------------------------

// Export list of keys to TXT file
void TCModuleImp::CmKeysToTXT()
{
    try
    {
        if( !rMod.MessageToSave() )
	    return;
        rMod.KeysToTXT();
        pVisor->Update(); // no objecs change, only title
    }
    catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

// Delete the list of records
void TCModuleImp::CmDeleteList()
{
    try
    {
        if( !rMod.MessageToSave() )
	    return;
        rMod.DelList();
    }
    catch( TError& xcpt )
    {
        pVisor->Update();
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}


// Copy list command (Sveta 14/06/01)
void TCModuleImp::CmCopyList( )
{
    try
    {
        if( !rMod.MessageToSave() )
	    return;

        rMod.CopyRecordsList( false );
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        pVisor->Update();
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void TCModuleImp::CmRenameList( )
{
    try
    {
        if( !rMod.MessageToSave() )
	    return;

        rMod.CopyRecordsList(true);
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        pVisor->Update();
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

// Transfer the list of records
void TCModuleImp::CmTransferList()
{
    try
    {
        if( !rMod.MessageToSave() )
	    return;
        rMod.Transfer();
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        pVisor->Update();
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

// Export data to text file
void TCModuleImp::CmExport()
{
    try
    {
        if( !rMod.MessageToSave() )
	    return;

        rMod.RecExport();
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        pVisor->Update();
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void TCModuleImp::CmBackup()
{
    try
    {
        if( !rMod.MessageToSave() )
	    return;

        rMod.RecToTXT();
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        pVisor->Update();
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

// Import data from text file
void TCModuleImp::CmImport()
{
    try
    {
        if( !rMod.MessageToSave() )
	    return;

        rMod.RecImport();
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        pVisor->Update();
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void TCModuleImp::CmRestore()
{
    try
    {
        if( !rMod.MessageToSave() )
	    return;

        rMod.RecOfTXT();
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        pVisor->Update();
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void TCModuleImp::CmBackuptoJson()
{
    try
    {
        if( !rMod.MessageToSave() )
            return;

        rMod.RecListToJSON();
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        pVisor->Update();
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void TCModuleImp::CmRestorefromJson()
{
    try
    {
        if( !rMod.MessageToSave() )
            return;

        rMod.RecListFromJSON();
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        pVisor->Update();
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

//--------------------- End of module_w_actions.cpp ---------------------------


