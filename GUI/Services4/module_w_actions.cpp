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

// returns true if we can close the window
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
    try {
        rMod.clearEditFocus();

        auto nrt_db = rMod.rtNum();
        if(rMod.IsSubModule()) {
            nrt_db = RT_SYSEQ;
        }
        aMod[nrt_db]->SaveM();
    }
    catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

// Save record to DB file as new
void TCModuleImp::CmSaveAs()
{
    try {
        rMod.clearEditFocus();

        auto nrt_db = rMod.rtNum();
        if(rMod.IsSubModule()) {
            nrt_db = RT_SYSEQ;
        }
        aMod[nrt_db]->SaveAs();
        pVisor->Update( true );
    }
    catch( TError& xcpt )  {
        pVisor->Update( true );
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}


// Delete current record
void TCModuleImp::CmDelete()
{
    try  {
        auto nrt_db = rMod.rtNum();
        if(rMod.IsSubModule()) {
            nrt_db = RT_SYSEQ;
        }
        aMod[nrt_db]->DeleteCurrent();
    }
    catch( TError& xcpt )  {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

// Show another record from DB (Without check)
void TCModuleImp::CmShow( const char *key )
{
    try  {
        auto nrt_db = rMod.rtNum();
        if(rMod.IsSubModule()) {
            nrt_db = RT_SYSEQ;
        }

        if( pVisor->ProfileMode &&
            ( aMod[nrt_db]->rtNum() >= RT_SYSEQ || aMod[nrt_db]->rtNum() == RT_PARAM )  )
            Error( aMod[nrt_db]->GetName(), "Invalid command in Project mode!");

        if( !aMod[nrt_db]->MessageToSave() )
            return;

        // get key of record
        std::string str;
        if( key == nullptr )    {
            str = aMod[nrt_db]->GetKeyofRecord( /*db->PackKey()*/nullptr, "Select data record key ", KEY_OLD );
            if(str.empty() )
                return;
        }
        else  str = std::string(key);

        aMod[nrt_db]->RecInput(str.c_str());
        aMod[nrt_db]->SetTitle();
        pVisor->Update( true );
    }
    catch( TError& xcpt )  {
        pVisor->Update( true );
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

// Rebuild loading before record (error if current record undefined)
void TCModuleImp::CmDerive()
{
    try {
        auto nrt_db = rMod.rtNum();
        if(rMod.IsSubModule()) {
            nrt_db = RT_SYSEQ;
        }
        if( aMod[nrt_db]->MessageToSave() ) {
            aMod[nrt_db]->RunDerive();
            pVisor->Update();
        }
    }
    catch( TError& xcpt ) {
        pVisor->Update( true );
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}


// Calc loading before record (error if current record undefined)
void TCModuleImp::CmCalc()
{
    try  {
        rMod.clearEditFocus();

        auto nrt_db = rMod.rtNum();
        if(rMod.IsSubModule()) {
            nrt_db = RT_SYSEQ;
        }
        aMod[nrt_db]->RunCalc();
        pVisor->Update();
    }
    catch( TError& xcpt ) {
        pVisor->Update( true );
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

// Add new record to DB
void TCModuleImp::CmNew()
{
    try {
        auto nrt_db = rMod.rtNum();
        if(rMod.IsSubModule()) {
            nrt_db = RT_SYSEQ;
        }
        if( aMod[nrt_db]->MessageToSave() ) {
            aMod[nrt_db]->RunNew();
            pVisor->Update();
        }
    }
    catch( TError& xcpt ) {
        pVisor->Update( true );
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void TCModuleImp::CmCreate()
{
    try {
        auto nrt_db = rMod.rtNum();
        if(rMod.IsSubModule()) {
            nrt_db = RT_SYSEQ;
        }
        if( aMod[nrt_db]->MessageToSave() ) {
            aMod[nrt_db]->RunCreate();
            pVisor->Update();
        }
    }
    catch( TError& xcpt ) {
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
    try {
        if(rMod.IsSubModule()) {
            if( aMod[RT_SYSEQ]->MessageToSave() ) {
                aMod[RT_SYSEQ]->RecordLoadinProfile(key);
                pVisor->Update();
            }
            return;
        }

        if( rMod.rtNum() < RT_SYSEQ && rMod.rtNum() != RT_SDATA) {
            CmShow(key);
            return;
        }

        if( rMod.MessageToSave() ) {
            rMod.RecordLoadinProfile(key);
            pVisor->Update(true);
        }
    }
    catch( TError& xcpt )  {
        pVisor->Update( true );
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

// Adds new record to DB in Project mode
void TCModuleImp::CmNewinProfile()
{
    try {
        auto nrt_db = rMod.rtNum();
        if(rMod.IsSubModule()) {
            nrt_db = RT_SYSEQ;
        }
        if( aMod[nrt_db]->MessageToSave() ) {
            aMod[nrt_db]->NewinProfile();
            pVisor->Update();
        }
    }
    catch( TError& xcpt )   {
        pVisor->Update( true );
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void TCModuleImp::CmCreateinProfile()
{
    try  {
        auto nrt_db = rMod.rtNum();
        if(rMod.IsSubModule()) {
            nrt_db = RT_SYSEQ;
        }
        if( aMod[nrt_db]->MessageToSave() ) {
            aMod[nrt_db]->CreateinProfile();
            pVisor->Update();
        }
    }
    catch( TError& xcpt ) {
        pVisor->Update( true );
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void TCModuleImp::CmPlot()
{
    try {
        auto nrt_db = rMod.rtNum();
        if(rMod.IsSubModule()) {
            nrt_db = RT_SYSEQ;
        }

        if( !aMod[nrt_db]->MessageToSave() )
            return;

        std::string str=aMod[nrt_db]->CurrentKey();
        if( str.find_first_of("*?" ) != std::string::npos )
           // || ( db->GetStatus()== UNDF_ && db->RecCount() && nrt_db != RT_SDATA ))
            Error( aMod[nrt_db]->GetName(), "E4 Cannot save under record key template, or record contents are not yet loaded!");

        aMod[nrt_db]->RecordPlot( str.c_str() );
        pVisor->Update();
    }
    catch( TError& xcpt ) {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void TCModuleImp::CmPrint()
{
    try  {
        auto nrt_db = rMod.rtNum();
        if(rMod.IsSubModule()) {
            nrt_db = RT_SYSEQ;
        }

        if( !aMod[nrt_db]->MessageToSave() )
            return;

        aMod[nrt_db]->RecordPrint();
        aMod[nrt_db]->SetString("Printing of a record finished OK. ");
        pVisor->Update();
    }
    catch( TError& xcpt ) {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

//----------------------------------------------------------
//--- Manipulation with database files   (Service functions )
//----------------------------------------------------------

// Index files of data base
void TCModuleImp::CmRebildFile()
{
    try  {
        if(rMod.IsSubModule()) {
            return;
        }

        if(rMod.MessageToSave()) {
            rMod.RebildFile();
            pVisor->Update();
        }
    }
    catch( TError& xcpt ) {
        pVisor->Update();
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

// Rename files from opend files list of data base
void TCModuleImp::CmAddFileToList()
{
    try {
        if( !rMod.MessageToSave() )
            return;

        rMod.AddFileToList();
        pVisor->Update(); // no objecs change, only title
    }
    catch( TError& xcpt )  {
        pVisor->Update();
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

// Open new files from the list of closed files in the data base
void TCModuleImp::CmAddOpenFile()
{
    try {
        if( !rMod.MessageToSave() )
            return;
        rMod.AddOpenFile();
        pVisor->Update(); // no objecs change, only title
    }
    catch( TError& xcpt ) {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

// Rebuild the list of opened files for the data base
void TCModuleImp::CmReOpenFileList()
{
    try {
        if( !rMod.MessageToSave() )
            return;

        rMod.ReOpenFileList();
        pVisor->Update(); // no objecs change, only title
    }
    catch( TError& xcpt ) {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

//----------------------------------------------------------
//--- Manipulation with list of records
//----------------------------------------------------------

// Export list of keys to TXT file
void TCModuleImp::CmKeysToTXT()
{
    try {
        if( rMod.MessageToSave() ) {
            rMod.KeysToTXT();
            pVisor->Update(); // no objecs change, only title
        }
    }
    catch( TError& xcpt )  {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

// Delete the list of records
void TCModuleImp::CmDeleteList()
{
    try {
        if( rMod.MessageToSave() ) {
            rMod.DelList();
        }
    }
    catch( TError& xcpt ) {
        pVisor->Update();
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

// Copy list command (Sveta 14/06/01)
void TCModuleImp::CmCopyList( )
{
    try {
        if( !rMod.MessageToSave() )
            return;

        rMod.CopyRecordsList( false );
        pVisor->Update();
    }
    catch( TError& xcpt ) {
        pVisor->Update();
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void TCModuleImp::CmRenameList( )
{
    try  {
        if( !rMod.MessageToSave() )
            return;

        rMod.CopyRecordsList(true);
        pVisor->Update();
    }
    catch( TError& xcpt ) {
        pVisor->Update();
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

// Transfer the list of records
void TCModuleImp::CmTransferList()
{
    try {
        if( !rMod.MessageToSave() )
            return;
        rMod.Transfer();
        pVisor->Update();
    }
    catch( TError& xcpt ) {
        pVisor->Update();
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

// Export data to text file
void TCModuleImp::CmExport()
{
    try {
        if( !rMod.MessageToSave() )
            return;

        rMod.RecExport();
        pVisor->Update();
    }
    catch( TError& xcpt ) {
        pVisor->Update();
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void TCModuleImp::CmBackup()
{
    try {
        if( !rMod.MessageToSave() )
            return;

        rMod.RecToTXT();
        pVisor->Update();
    }
    catch( TError& xcpt ) {
        pVisor->Update();
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

// Import data from text file
void TCModuleImp::CmImport()
{
    try {
        if( !rMod.MessageToSave() )
            return;

        rMod.RecImport();
        pVisor->Update();
    }
    catch( TError& xcpt ) {
        pVisor->Update();
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void TCModuleImp::CmRestore()
{
    try {
        if( !rMod.MessageToSave() )
            return;

        rMod.RecOfTXT();
        pVisor->Update();
    }
    catch( TError& xcpt ) {
        pVisor->Update();
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void TCModuleImp::CmBackuptoJson()
{
    try {
        if( !rMod.MessageToSave() )
            return;

        std::string s= rMod.GetName();
        std::string filename = s + ".backup.json";
        s += " : Please, give a file name for unloading records";
        if( vfChooseFileSave( window(), filename, s.c_str(), "*.json" ) == false )
            return;

        rMod.RecListToJSON(rMod.Filter.c_str(), filename);
        pVisor->Update();
    }
    catch( TError& xcpt )  {
        pVisor->Update();
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

void TCModuleImp::CmRestorefromJson()
{
    try {
        if( !rMod.MessageToSave() )
            return;
        // Choose file name
        std::string s = std::string( rMod.GetName() )+" : Please, select file with unloaded records";
        std::string filename;
        if( vfChooseFileOpen( window(), filename, s.c_str(), "*.json" ) == false )
            return;
        rMod.RecListFromJSON(filename);
        pVisor->Update();
    }
    catch( TError& xcpt ) {
        pVisor->Update();
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

//--------------------- End of module_w_actions.cpp ---------------------------


