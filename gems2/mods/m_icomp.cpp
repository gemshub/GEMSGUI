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
#include "filters_data.h"

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

int TIComp::RecBuild( const char *key, int mode  )
{
    int bldType = mode;
    if( bldType == VF_UNDEF )
       bldType = vfQuestion3(window(), "Reallocation of data arrays ",
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
        fEdit = true;
        break;
    case VF3_1:   // VF3_3   - do nothing
        retType = VF3_3;
        break;
    }
    if( retType == VF3_1 )
    {
        strncpy( icp->name, db->FldKey(2), db->FldLen(2));
        icp->name[db->FldLen(2)] = '\0';
    }
    return  retType;
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
TIComp::GetElements( bool isotopes, TCStringArray& aIC, TCIntArray& aIndMT )
{

 TCIntArray anR;
 TCStringArray aIC1;

 //open selected files in kernel database
 //db->OpenOnlyFromList(names);
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
TIComp::CopyElements( const char * prfName,
         elmWindowData el_data, icSetupData st_data )
{
    // open selected kernel files
   //db->OpenOnlyFromList(el_data.flNames);
    int fnum_ = db->GetOpenFileNum( prfName );

    //  copy to it selected records
    // ( add to last key field first symbol from prfname )
    int nrec;
    for(uint j, i=0; i<el_data.ICrds.GetCount(); i++ )
    {
       for( j=0; j<el_data.oldIComps.GetCount(); j++ )
        if( !memcmp( el_data.ICrds[i].c_str(),
            el_data.oldIComps[j].c_str(), MAXICNAME+MAXSYMB ))
         break;
       if( j<el_data.oldIComps.GetCount() )
         continue;

       nrec = db->Find( el_data.ICrds[i].c_str() );
       db->Get( nrec );
        /// !!! changing record key
       gstring str= gstring(db->FldKey( 2 ), 0, db->FldLen( 2 ));
       ChangeforTempl( str, st_data.from_templ,
                       st_data.to_templ, db->FldLen( 2 ));
        str += ":";
        gstring str1 = gstring(db->FldKey( 1 ), 0, db->FldLen( 1 ));
        str1.strip();
        str = str1 + ":" + str;
        str1 = gstring(db->FldKey( 0 ), 0, db->FldLen( 0 ));
        str1.strip();
        str = str1 + ":" + str;
        AddRecord( str.c_str(), fnum_ );
    }

    // close all no project files
    TCStringArray names1;
    names1.Add(prfName);
    db->OpenOnlyFromList(names1);

}




// ------------- End of file  m_icomp.cpp -------------------


