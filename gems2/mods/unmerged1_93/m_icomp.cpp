//-------------------------------------------------------------------
// Id: gems/mods/m_icomp.cpp  version 2.0.0   2001
//
// Implementation of TIComp class, config and calculation functions
//
// Rewritten from C to C++ by S.Dmytriyeva  970207 modified 010904
// Copyright (C) 1995-2001 S.Dmytriyeva, D.Kulik
//
// This file is part of a GEM-Selektor library for thermodynamic
// modelling by Gibbs energy minimization
// Uses: GEM-Vizor GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://les.web.psi.ch/Software/GEMS-PSI for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//
const char *GEMS_IC_HTML = "gm_icomp";

#include "m_icomp.h"
#include "v_object.h"
#include "visor.h"
#include "service.h"

TIComp* TIComp::pm;

TIComp::TIComp( int nrt ):
        TCModule( nrt )
{
    aFldKeysHelp.Add(
"l<6  Symbol of Independent Component (IC) { U Mg Hum Lnta Vol Zz ... }_");
    aFldKeysHelp.Add(
        "l<4  Class of IC { e o h a i z v <integer> }_");
    aFldKeysHelp.Add(
        "l<22 Record Key Comment to IC definition_");
    icp=&ic[0];
    set_def();
    start_title = " Data for Independent Components ";
}

// link values to objects
void TIComp::ods_link( int q)
{
    // aObj[ o_icsymb].SetPtr( ic[q].symb );
    // aObj[ o_ictype].SetPtr( ic[q].type );
    // aObj[ o_icgrp].SetPtr( ic[q].grp );
    aObj[ o_icawt].SetPtr( &ic[q].awt );
    aObj[ o_icass].SetPtr( &ic[q].aSs );
    aObj[ o_icacp].SetPtr( &ic[q].aCp );
    aObj[ o_icavs].SetPtr( &ic[q].aVs );
    aObj[ o_icarad].SetPtr( &ic[q].arad );
    aObj[ o_icidis].SetPtr( &ic[q].idis );
    aObj[ o_icval].SetPtr( &ic[q].val );
    aObj[ o_icnum].SetPtr( &ic[q].num );
    aObj[ o_iccoor].SetPtr( &ic[q].coor );
    aObj[ o_icsst].SetPtr( ic[q].sst );
    aObj[ o_icname].SetPtr( ic[q].name );
    aObj[ o_icform].SetPtr( ic[q].form );
    aObj[ o_icdc].SetPtr(  ic[q].dc_ref );
    aObj[ o_icfloat].SetPtr( &ic[q].awt );
    aObj[ o_icint].SetPtr( &ic[q].val );
    aObj[ o_ictprn].SetPtr( ic[q].tprn );
    icp=&ic[q];
}

// set dynamic Objects ptr to values
void TIComp::dyn_set(int q)
{
    ErrorIf( icp!=&ic[q], GetName(),
             "Illegal access to ic in dyn_set.");
    ic[q].tprn= (char *)aObj[o_ictprn].GetPtr();
}

// free dynamic memory in objects and values
void TIComp::dyn_kill(int q)
{
    ErrorIf( icp!=&ic[q], GetName(),
             "Illegal access to ic in dyn_kill.");
    ic[q].tprn = (char *)aObj[o_ictprn].Free();
}

// realloc dynamic memory
void TIComp::dyn_new(int /*q*/)
{
    // if( ic[q].tprn == 0 )
    //   ic[q].tprn = (char *)aObj[o_ictprn].Alloc( 1, 256, S_ );
    // icp=&ic[q];
}


//set default information

void TIComp::set_def( int q)
{
    ErrorIf( icp!=&ic[q], GetName(), "Illegal access to IC in set_def()");
    strcpy( ic[q].sst, "s" );
    strcpy( ic[q].name, "Independent Component");
    strncpy( ic[q].form, rt[rtNum()].FldKey(0), MAXICNAME );
    strcpy( ic[q].dc_ref, "`" );
    ic[q].awt   =  FLOAT_EMPTY;
    ic[q].aSs   =  FLOAT_EMPTY;
    ic[q].aCp   =  0;
    ic[q].aVs   =  0;
    ic[q].arad  =  0;
    ic[q].idis  =  1;
    ic[q].val   =  1;
    ic[q].num   =  0;
    ic[q].coor  =  0;
    ic[q].tprn  =  0;
    dyn_new();
}

// Input necessary data and link DOD
void
TIComp::RecInput( const char *key )
{
    TCModule::RecInput( key );
}

void
TIComp::CmHelp()
{
    pVisor->OpenHelp( GEMS_IC_HTML );
}

void
TIComp::GetElements( bool isotopes, TCStringArray& names,
                      TCStringArray& aIC, TCIntArray& aIndMT )
{

 TCIntArray anR;
 TCStringArray aIC1;

 //open selected files in kernel database
 db->OpenOnlyFromList(names);
 //db->OpenAllFiles(true);
 db->GetKeyList( "*:*:*:", aIC1, anR );

 for( uint ii=0; ii<aIC1.GetCount(); ii++ )
 {
    RecInput( aIC1[ii].c_str() );
    if( *db->FldKey( 1 ) == 'a' || *db->FldKey( 1 ) == 'v' ) // addition
      aIndMT.Add( -1 );
    else
      if( isotopes || *db->FldKey( 1 ) == 'e' || *db->FldKey( 1 ) == 'z' ||
          *db->FldKey( 1 ) == 'h' || *db->FldKey( 1 ) == 'o' )
        aIndMT.Add( icp->num );
      else
        continue;
   aIC.Add(aIC1[ii]);
 }
}



#include "m_sdata.h"
void
TIComp::RecordPrint( const char *key )
{

 gstring sd_key;
  if( key )
  sd_key=key;
 else
  sd_key = "pscript:0000:icomp:";
 // read sdref record with format prn
 TSData::pm->RecInput( sd_key.c_str() );
 char * text_fmt = TSData::pm->getAbstr();
 if( !text_fmt )
   Error( sd_key.c_str(), "No format text in this record.");
 PrintSDref( sd_key.c_str(), text_fmt );
}

void
TIComp::CopyElements( const char * prfName, TCStringArray& aKeys )
{
    // added to profile file icomp.copy.prfname
    gstring Path = pVisor->userProfDir();
    Path += prfName;
    Path += "/";
    Path += db->GetKeywd();
    Path += ".";
    Path += "copy";
    Path += ".";
    Path += prfName;
    Path += ".";
    Path += PDB_EXT;
    TDBFile *aFl= new TDBFile( Path );
    int fnum_ = db->AddFileToList( aFl );
    //  copy to it selected records
    // ( add to last key field first symbol from prfname )
    int nrec, j;
    for(uint i=0; i<aKeys.GetCount(); i++ )
    {
        nrec = db->Find( aKeys[i].c_str() );
        db->Get( nrec );
        /// !!! changing record key
        gstring str= gstring(db->FldKey( 2 ), 0, db->FldLen( 2 ));
        for( j=0; j<db->FldLen( 2 ); j++ )
         if( str[j] == ' ' )
          break;
        if( j == db->FldLen( 2 ) )
          j--;
        if( str[j] == *prfName )
            str[j] = ' ';
        else
            str[j] = *prfName;
        str = gstring(db->FldKey( 1 ), 0, db->FldLen( 1 )) + str;
        str = gstring(db->FldKey( 0 ), 0, db->FldLen( 0 )) + str;
        AddRecord( str.c_str(), fnum_ );
    }
    // close all no profile files
    TCStringArray names;
    names.Add(prfName);
    db->OpenOnlyFromList(names);
}




// ------------- End of file  m_icomp.cpp -------------------


