//-------------------------------------------------------------------
// $Id: m_sdata.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of TSData class, config and calculation functions
//
// Rewritten from C to C++ by S.Dmytriyeva
// Copyright (C) 1995-2001 S.Dmytriyeva, D.Kulik
//
// This file is part of a GEM-Selektor library for thermodynamic
// modelling by Gibbs energy minimization
// Uses: GEM-Selektor GUI GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the terms of GEMS3 Development
// Quality Assurance Licence (GEMS3.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//

#include "m_sdata.h"
#include "service.h"
#include "visor.h"
#include "v_object.h"

TSData* TSData::pm;

TSData::TSData( int nrt ):
        TCModule( nrt )
{
    refs = 0;
    title = abstr = 0;
    aFldKeysHelp.push_back("Script name or author's name for data source");
    aFldKeysHelp.push_back("Script number or year of publication { 1990 }");
    aFldKeysHelp.push_back("Script module name { DComp ReacDC } or data source type");
    //startKeyEdit = 0;
    //aFldShot.Add("SDname");
    //aFldShot.Add("SDyear");
    //aFldShot.Add("SDtype");
    set_def();
    start_title = " Scripts or Bibliographic References ";
}

// link values to objects
void TSData::ods_link( int )
{
    aObj[o_sdnref].SetPtr( &nREf );
    aObj[o_sdauth].SetPtr( auth );
    aObj[o_sdyear].SetPtr( year );
    aObj[o_sdclass].SetPtr( type );
    aObj[o_sdvoly].SetPtr( volyr );
    aObj[o_sdpage].SetPtr( pages );
    aObj[o_sdauthr].SetPtr( authors );
    aObj[o_sdedit].SetPtr( editn );
    aObj[o_sdnote].SetPtr( notes );
}

// set default data
void TSData::set_def(int)
{
    nREf = 0;
    dyn_new();
    strcpy( auth, "Einstein_ea" );
    strcpy( year, "2003" );
    strcpy( type, "wow" );
    strcpy( authors, "Einstein, A. and others" );
    strcpy( title, "Forgotten discovery of greatest importance" );
    strcpy( volyr, "2003, v.33" );
    strcpy( pages, "222-333" );
    strcpy( editn, "Journal of Systematic Research" );
    strcpy( notes, "Something wonderful" );
    strcpy( abstr, "Precise description of Universe" );
}

// reallocate dynamic memory
void TSData::dyn_new(int)
{
    if( title==0 )
        title = (char *)aObj[o_sdtitle].Alloc( 1, 128, S_ );
    if( abstr==0 )
        abstr = (char *)aObj[o_sdabstr].Alloc( 1, 128, S_ );
    if( nREf != 0 )
        refs = (char *)aObj[o_sdrefs].Alloc( nREf, 1, 32 );
    else  if( refs )
        refs = (char *)aObj[o_sdrefs].Free();
}

// set dynamic objects ptr to values
void TSData::dyn_set(int)
{
    title = (char *)aObj[o_sdtitle].GetPtr();
    abstr = (char *)aObj[o_sdabstr].GetPtr();
    refs = (char *)aObj[o_sdrefs].GetPtr();
    nREf = (short)aObj[o_sdrefs].GetN();
}

// free dynamic memory in objects and values
void TSData::dyn_kill(int)
{
    title = (char *)aObj[o_sdtitle].Free();
    abstr = (char *)aObj[o_sdabstr].Free();
    refs = (char *)aObj[o_sdrefs].Free();
}

//Rebuild record structure before calc
int TSData::RecBuild( const char *key, int mode  )
{
AGAIN:
    int ret = TCModule::RecBuild( key, mode );
    if( ret == VF_CANCEL )
        return ret;
    if( nREf < 0 || nREf > 7 )
        goto AGAIN;
    dyn_new();
    pVisor->Update();
    return ret;
}

//Save record structure - added 09.04.03 KD
void TSData::RecSave( const char *key, bool onOld )
{
    TCModule::RecSave( key, onOld );
}

const char* TSData::GetHtml()
{
   return GM_SDREF_HTML;
}

// virtual callback for printing the record
// should be redefined in subclasses (modules)
// to make some plotting

void TSData::RecordPrint( const char* key )
{
   string text_fmt = "line %s \"SDref record: \", %s rkey, %11s date, %6s time\n"
                      "line %s \"\"\n"
                      "line %s \"Authors: \", %s #SDauth\n"
                      "line %s \"Title: \", %s #SDtitl\n"
                      "line %s \"Edition: \", %s #SDedit\n"
                      "line %s \"Year: \", %s #SDvoly, %s \"Pages: \", %s #SDpage \n"
                      "line %s \"Comment\"\n"
                      "line %s #SDnote\n"
                      "line %s \"Abstract\"\n"
                      "line %s #SDabst\n"
                      "line %s \"------\"\n";
    PrintSDref( "pscript-script:0000:sdref:", text_fmt.c_str() );
}


void TSData::CopyRecords( const char *prfName, vector<string>& SDlist )
{
   int ii;

    // open selected kernel files
    int fnum_ = db->GetOpenFileNum( prfName );

    // add scripts for list to copy
    vector<string> aScripts;
    db->GetKeyList( "?script*:*:*:", aScripts );
    for(ii=0; ii<aScripts.size(); ii++ )
       SDlist.push_back(aScripts[ii]);

    // rename records from list
    for(ii=0; ii<SDlist.size(); ii++ )
    {
    try{
        if(SDlist[ii].empty())
          continue;

        if( !db->Find(SDlist[ii].c_str()))
          continue;
        RecInput( SDlist[ii].c_str() );

      // changing record key
      string str= db->FldKey( 2 );
      // we can change/add only one last (7) symbol
      // first (6) critical for scripts
      ChangeforTempl( str, "*", "*_", db->FldLen( 2 ));
      db->SetFldKey(2,str.c_str());
      str = db->PackKey();
      //Point SaveRecord
      if( !db->Find(str.c_str()))
        db->AddRecord( str.c_str(), fnum_ );
     }catch( TError& xcpt)
      {
         cout << xcpt.mess << "   " << SDlist[ii].c_str() << endl;
      }
    }

    // close all no project files
    vector<string> names1;
    names1.push_back(prfName);
    db->OpenOnlyFromList(names1);
}

// Test record with key
void TSData::TryRecInp( const char *key_, time_t& time_s, int q )
{
    vector<string> aSDlist;

    if( ! MessageToSave() )
        return;

    TEJDBKey dbKey(db->GetDBKey());
    dbKey.SetKey(key_);
    string fld2 = dbKey.FldKey(2);
    fld2 += "*";
//    cout << fld2.c_str() << endl;
    dbKey.SetFldKey(2,fld2.c_str());
    RecStatus iRet = db->Rtest( dbKey.UnpackKey(), 1 );
    string msg;

    switch( iRet )
    {
    case MANY_:    // Get Key list
        db->GetKeyList(  dbKey.UnpackKey(), aSDlist );
        db->Get(aSDlist[0].c_str());
    case ONEF_:
        dyn_set(q);
        time_s = db->Rtime();
        return;
    case UNDF_:
    case NONE_:
    case EMPC_:
        {
            msg = "E16DCrun: In database chain  ";
            msg +=  GetName();
            msg += ": Data record not found, \n"
                   " key  '";
            msg += key_;
            msg += "'.\n Maybe, a database file is not linked.\n";
            Error( GetName(), msg );
        } // break;
    case FAIL_:
        msg = "E22DCrun: Failure!!! Database chain ";
        msg += GetName();
        msg += " is corrupt,\n"
               "Data record key '";
        msg += key_;
        msg += "'\n Try to backup/restore or compress files in this database chain!";
        Error( GetName(),  msg );
    }
}

TSData* pmSData;

// --------------------- End of m_sdata.cpp --------------------------

