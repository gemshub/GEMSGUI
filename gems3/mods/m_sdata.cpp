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
// Uses: GEM-Vizor GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//

#include "m_sdata.h"
#include "v_object.h"
#include "v_mod.h"
#include "service.h"
#include "visor.h"

TSData* TSData::pm;

TSData::TSData( int nrt ):
        TCModule( nrt )
{
    refs = 0;
    title = abstr = 0;
    aFldKeysHelp.Add("Script name or author's name for data source");
    aFldKeysHelp.Add("Script number or year of publication { 1990 }");
    aFldKeysHelp.Add("Script module name { DComp ReacDC } or data source type");
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
   gstring text_fmt = "line %s \"SDref record: \", %s rkey, %11s date, %6s time\n"
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

TSData* pmSData;

// --------------------- End of m_sdata.cpp --------------------------

