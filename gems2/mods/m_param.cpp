//-------------------------------------------------------------------
// $Id$
//
// Implementation of config and basic methods of TProfile class
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
// See http://les.web.psi.ch/Software/GEMS-PSI for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//
const char *GEMS_SP_HTML = "gm_project";

#ifdef __unix
#include <unistd.h>
#endif

#include <math.h>
#include "m_unspace.h"
#include "m_gtdemo.h"
#include "m_syseq.h"
#include "m_dcomp.h"
#include "m_icomp.h"
#include "m_compos.h"
#include "m_phase.h"
#include "m_reacdc.h"
#include "m_proces.h"
#include "m_dualth.h"
#include "m_param.h"
#include "v_object.h"
#include "visor.h"
#include "v_mod.h"
#include "service.h"
#include "gdatastream.h"

#include "nodearray.h"

TProfil* TProfil::pm;

const double R_CONSTANT = 8.31451,
              NA_CONSTANT = 6.0221367e23,
                F_CONSTANT = 96485.309,
                  e_CONSTANT = 1.60217733e-19,
                    k_CONSTANT = 1.380658e-23,
// Conversion factors
                      cal_to_J = 4.184,
                        C_to_K = 273.15,
                          lg_to_ln = 2.302585093,
                            ln_to_lg = 0.434294481;

SPP_SETTING pa_ = {
    "GEMS-PSI v2.x: Controls and defaults for numeric modules",
    {
        1,  /* PC */  3,     /* PD */   3,   /* PRD */
        1,  /* PSM  */ 144,  /* DP */   15,   /* DW */
        -3, /* DT */  0,     /* PLLG */   1,   /* PE */
        500,   /* IIM */
        1e-30, /* DG */   1e-8,  /* DHB */  1e-12,  /* DS */
        1e-4,  /* DK */  0.01,  /* DF */  -0.1,  /* DFM */
        1e-6,  /* DFYw */  1e-6,  /* DFYaq */    1e-6,  /* DFYid */
        1e-6,  /* DFYr,*/  1e-6,  /* DFYh,*/   1e-6,  /* DFYc,*/
        1e-12, /* DFYs, */  1e-17,  /* DB */   0.7,   /* AG */
        0.07,   /* DGC */   1.0,   /* GAR */  1000., /* GAH */
        0.01, /* GAS */  12.05,  /* DNS */  1e-8,  /* XwMin, */
        1e-8,  /* ScMin, */  1e-20, /* DcMin, */   1e-10, /* PhMin, */
        3e-5,  /* ICmin */   1e-7,  /* EPS */   1e-3,  /* IEPS */
        1e-4,  /* DKIN  */ 0,  /* tprn */
    },
    "CSC   ",   /* DCpct[6] */  "OjjbC+---",    /* DCpdc[9] */
    "+----M",   /* BCpc[6] */   "K3C   ",   /* REpct[6] */
    "SjjbC ",   /* REpdc[6]*/  "+-------", /* REpvc[8]  */
    "rjjbC-TK--",   /* RPpdc[10]  */
    "++*-1-23----------4-------------", /* RPpvc[32] reserved free */
    "INNINN",   /* PHsol_t[6] */  "s-----",   /* PHpvc[6] */
    "++++-+-+++", /* MUpmv[10] */ "jjbC++0+", /* TPpdc[8] */
    "*-------*-----------", /* TPpvc[20] */ "+-+-+-----", /* SYppc[10] */
    "***-*-*---**-***-----------*", /* SYpvc[28]*/  "-+++----ME", /* UTppc[10] */
    "0*----------", /* PEpsc[12]  */  "------------", /* PEpvc[12] */
    { "GTDEMO task name   ", "Graphic screen # " } ,   /* GDcode[2][20] */
    "Plot ",                  /* GDpsc[7] */
    { "Abscissa","Ordinate"},    /* GDpcc[2][9] */
    "++++-+",   /* GDptc[6] */  "++++--",   /* GDpgw[6] */
    "*",   /* sdrefkey 32*/  "",   /* Reserv [50-32] */
    /* RTPARM */
    1, 15, 0,   /* P T V */  2,  /* Mode */
    { 1, 2, 3, 4, 5, },  /* ResShort */
    /* RTPARM */
    {0., 10000., 1000.}, /* Pi[3]*/  {0.0, 374., 25.}, /* Ti[3] */
    {0., 0., 0.},   /* Vi[3] */
    1., 25.,      /* Pr, Tr */
    /* MULTI */
    (float)1e-19, /* lowPosNum, */  (float)1e-16, /* logXw, */   (float)1e-9,  /* logYFk */
    { (float)0.064, (float)3.72, 0., 0.5 }, 0.   // Reserv
}; /* SPP_SETTING */


void
BASE_PARAM::write(GemDataStream& oss)
{
    oss.writeArray( &PC, 10 );
    oss.writeArray( &DG, 28 );
    oss.writeArray( "0000", 4 );
}

void
BASE_PARAM::read(GemDataStream& iss)
{
    char tmp[4];
    iss.readArray( &PC, 10 );
    iss.readArray( &DG, 28 );
    iss.readArray( tmp, 4 );
}


void
SPP_SETTING::write(GemDataStream& oss)
{
    oss.writeArray( ver, TDBVERSION );
    p.write( oss );
    oss.writeArray( DCpct, 352 );
    oss.writeArray( &NP, 9 );
    oss.writeArray( Pi, 19 );
}

void
SPP_SETTING::read(GemDataStream& iss)
{
    iss.readArray( ver, TDBVERSION );
    p.read( iss );
    iss.readArray( DCpct, 352 );
    iss.readArray( &NP, 9 );
    iss.readArray( Pi, 19 );
}


TProfil::TProfil( int nrt ):
        TCModule( nrt )
{
    aFldKeysHelp.Add(
        "l<10 Identifier of root modelling project definition");
    aFldKeysHelp.Add(
        "l<38 Record key comment to project definition");
    start_title = " Numerical and Configuration Settings ";
    pa= pa_;
    pa.p.tprn=0;

    rmults = 0;
    mtparm =  0;
    syst = 0;
    multi =0;

    mup = 0;
    tpp = 0;
    syp = 0;
    pmp = 0;

    SFold = 0;
    SMold = 0;
    SAold = 0;
    SBold = 0;
    Llold =0;

    userCancel = false;
    stepWise = false;
    calcFinished = false;
    fStopCalc = false;
    comp_change_all = false;

}

// init submodules to calc module
void TProfil::InitSubModules()
{
    if( !rmults /*aMod.GetCount() < MD_RMULTS*/ )
    {
        aMod.Add( rmults = new TRMults( MD_RMULTS ) );
        rmults->ods_link();
        mup = rmults->GetMU();
        aMod.Add( mtparm = new TMTparm( MD_MTPARM, mup ) );
        mtparm->ods_link();
        tpp = mtparm->GetTP();
        aMod.Add( syst = new TSyst( MD_SYSTEM, mup ) );
        syst->ods_link();
        syp = syst->GetSY();
        aMod.Add( multi = new TMulti( MD_MULTI, syp, tpp, mup ) );
        multi->ods_link();
        pmp = multi->GetPM();
        aMod.Add( new TEQCalc( MD_EQCALC ) );
        aMod.Add( new TEQDemo( MD_EQDEMO ) );
    }
}

// link values to objects
void TProfil::ods_link( int )
{
    /* rmults->ods_link();
     mtparm->ods_link();
     syst->ods_link();
     multi->ods_link();*/
    aObj[o_paver].SetPtr( pa.ver );
    aObj[o_papc].SetPtr( &(pa.p.PC) );
    aObj[o_paprd].SetPtr(&(pa.p.PD) );
    aObj[o_padpwt].SetPtr( &(pa.p.DP));
    aObj[o_papllg].SetPtr( &(pa.p.PLLG));
    aObj[o_pape].SetPtr( &(pa.p.PE) );
    aObj[o_paiim].SetPtr( &(pa.p.IIM) );
    aObj[o_padg].SetPtr( &(pa.p.DG) );
    aObj[o_padhb].SetPtr( &(pa.p.DHB) );
    aObj[o_pads].SetPtr( &(pa.p.DS) );
    aObj[o_padk].SetPtr( &(pa.p.DK) );
    aObj[ o_padf].SetPtr(   &(pa.p.DF));
    aObj[ o_padfy].SetPtr(  &(pa.p.DFYw));
    aObj[ o_padb].SetPtr(   &(pa.p.DB));
    aObj[ o_paag].SetPtr(   &(pa.p.AG));
    aObj[ o_padgc].SetPtr(  &(pa.p.DGC));
    aObj[ o_pagan].SetPtr(  &(pa.p.GAR));
    aObj[ o_padns].SetPtr(  &(pa.p.DNS));
    aObj[ o_paxmin].SetPtr( &(pa.p.XwMin));
    aObj[ o_paeps].SetPtr(  &(pa.p.EPS));
    aObj[ o_padkin].SetPtr( &(pa.p.DKIN));
    aObj[ o_patprn].SetPtr( pa.p.tprn );
    aObj[ o_padcpct].SetPtr(pa.DCpct );
    aObj[ o_padcpdc].SetPtr(pa.DCpdc );
    aObj[ o_parepct].SetPtr(pa.REpct );
    aObj[ o_parepdc].SetPtr(pa.REpdc );
    aObj[ o_parepvc].SetPtr(pa.REpvc );
    aObj[ o_parppdc].SetPtr(pa.RPpdc );
    aObj[ o_parppvc].SetPtr(pa.RPpvc );
    aObj[ o_paphsol_t].SetPtr(pa.PHsol_t );
    aObj[ o_paphpvc].SetPtr(pa.PHpvc );
    aObj[ o_pamupmv].SetPtr(pa.MUpmv );
    aObj[ o_patppdc].SetPtr(pa.TPpdc );
    aObj[ o_patppvc].SetPtr(pa.TPpvc );
    aObj[ o_pasyppc].SetPtr(pa.SYppc );
    aObj[ o_pasypvc].SetPtr(pa.SYpvc );
    aObj[ o_pautppc].SetPtr(pa.UTppc );
    aObj[ o_papepsc].SetPtr(pa.PEpsc );
    aObj[ o_papepvc].SetPtr(pa.PEpvc );
    aObj[ o_pagdcode].SetPtr(&pa.GDcode[0][0] );
    aObj[ o_pagdpsc].SetPtr(pa.GDpsc );
    aObj[ o_pagdpcc].SetPtr(&pa.GDpcc[0][0] );
    aObj[ o_pagdptc].SetPtr(pa.GDptc );
    aObj[ o_parpnptv].SetPtr(&pa.NP );
    aObj[ o_parpmode].SetPtr(&pa.Mode );
    aObj[ o_partpi].SetPtr(   pa.Pi );
    aObj[ o_partti].SetPtr(   pa.Ti );
    aObj[ o_partvi].SetPtr(   pa.Vi );
    aObj[ o_padrprtr].SetPtr(&pa.DRpst );
    aObj[ o_papmnum].SetPtr( &pa.lowPosNum );

    aObj[ o_pabcpc].SetPtr(  pa.BCpc );
    aObj[ o_pagdpgw].SetPtr(  pa.GDpgw );
    aObj[ o_pasdref].SetPtr(  pa.SDrefKey );

    aObj[ o_spppar].SetPtr(  (void *)&pa );
    aObj[ o_spppar].SetM( sizeof( SPP_SETTING ) );
    //   aObj[ o_sppconst].SetPtr( sc_ );
    //   ob[o_sppconst].dim_M = sizeof( SPP_CONST );
    //   aObj[ o_sppdatac].SetPtr( &nQ );
    //   ob[o_sppdatac].dim_M = sizeof( DATACOUNT );
}

// set dynamic Objects ptr to values

void TProfil::dyn_set(int )
{
    pa.p.tprn= (char *)aObj[o_patprn].GetPtr();
    if( rmults ) rmults->dyn_set();
    if( mtparm ) mtparm->dyn_set();
    if( syst ) syst->dyn_set();
    if( multi ) multi->dyn_set();
}

// free dynamic memory in objects and values
void TProfil::dyn_kill(int )
{
    pa.p.tprn = (char *)aObj[o_patprn].Free();
    if( rmults ) rmults->dyn_kill();
    if( mtparm ) mtparm->dyn_kill();
    if( syst ) syst->dyn_kill();
    if( multi ) multi->dyn_kill();
}

// realloc dynamic memory
void TProfil::dyn_new(int )
{
    /*  rmults->dyn_new();
      mtparm->dyn_new();
      syst->dyn_new();
      multi->dyn_new();
    */
    // if( pa.p.tprn == 0 )
    //   pa.p.tprn = (char *)aObj[o_patprn].Alloc( 1, 256, S_ );
}


//set default information

void TProfil::set_def( int )
{
    pa = pa_;
    if( rmults ) rmults->set_def();
    if( mtparm ) mtparm->set_def();
    if( syst ) syst->set_def();
    if( multi ) multi->set_def();
}

// Help on Modelling Project module ( ? button )
void
TProfil::CmHelp()
{
    pVisor->OpenHelp( GEMS_SP_HTML );  //  05.01.01
}

void TProfil::outMulti( GemDataStream& ff, gstring& path  )
{
    ff.writeArray( &pa.p.PC, 10 );
    ff.writeArray( &pa.p.DG, 28 );
    multi->to_file( ff, path );
}

// outpu MULTI to txt format
void TProfil::outMulti( gstring& path  )
{
    multi->to_text_file_gemipm( path.c_str() );
}

void TProfil::outMultiTxt( const char *path  )
{
    multi->to_text_file( path );
}

void TProfil::outMulti( )
{
   TNodeArray* na = new TNodeArray( 1, TProfil::pm->pmp/*multi->GetPM()*/ );

// set default data and realloc arrays
   float Tai[3], Pai[3];
   Tai[0] = Tai[1] = 25.;
   Pai[0] = Pai[1] = 1.;
   Tai[2] = Pai[2] = 0.;

// realloc and setup data for dataCH and DataBr structures
   na->MakeNodeStructures( window(), false, Tai, Pai );

// setup dataBR and NodeT0 data
   na->packDataBr();
   na->MoveWorkNodeToArray( 0, 1, na->pNodT0() );
// make  all files
//   na->PutGEM2MTFiles( window(), 1, true, true );
na->PutGEM2MTFiles( window(), 1, true, false );
   delete na;
}

// Reading structure MULTI (GEM IPM work structure)
void TProfil::readMulti( GemDataStream& ff )
{

      ff.readArray( &pa.p.PC, 10 );
      ff.readArray( &pa.p.DG, 28 );
      multi->from_file( ff );
}

//Delete record with key
void
TProfil::DeleteRecord( const char *key, bool /*errifNo*/ )
{
    TCStringArray aList;
    TCIntArray anR;
    vstr pkey(81);
    uint i;

    int  Rnum = rt[nRT].Find( key );
    ErrorIf( Rnum<0, GetName(), "Record to delete not found!");
    rt[RT_PARAM].Get( Rnum ); // read record
    dyn_set();
    SetFN();                  // reopen files of data base
    rt[nRT].SetKey( key);

    // Delete all records connected to project
    aList.Clear();    //SYSEQ
    anR.Clear();
    rt[RT_SYSEQ].MakeKey( RT_PARAM, pkey, RT_PARAM, 0,
                           K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_END);
    rt[RT_SYSEQ].GetKeyList( pkey, aList, anR );
    for( i=0; i< aList.GetCount(); i++)
        TSysEq::pm->DeleteRecord(aList[i].c_str());

    aList.Clear();    //PROCES
    anR.Clear();
    rt[RT_PROCES].MakeKey( RT_PARAM, pkey, RT_PARAM, 0,
                            K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_END);
    rt[RT_PROCES].GetKeyList( pkey, aList, anR );
    for( i=0; i< aList.GetCount(); i++)
        TProcess::pm->DeleteRecord(aList[i].c_str());

    aList.Clear();    //UNSPACE
    anR.Clear();
    rt[RT_UNSPACE].MakeKey( RT_PARAM, pkey, RT_PARAM, 0,
      K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_END);
    rt[RT_UNSPACE].GetKeyList( pkey, aList, anR );
    for( i=0; i< aList.GetCount(); i++)
        TUnSpace::pm->DeleteRecord(aList[i].c_str());

    aList.Clear();    //GTDEMO
    anR.Clear();
    rt[RT_GTDEMO].MakeKey( RT_PARAM, pkey, RT_PARAM, 0,
                            K_ANY, K_ANY, K_ANY, K_ANY, K_END);
    rt[RT_GTDEMO].GetKeyList( pkey, aList, anR );
    for( i=0; i< aList.GetCount(); i++)
        TGtDemo::pm->DeleteRecord(aList[i].c_str());

    aList.Clear();    //DUALTH
    anR.Clear();
    rt[RT_DUALTH].MakeKey( RT_PARAM, pkey, RT_PARAM, 0,
                            K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_END);
    rt[RT_DUALTH].GetKeyList( pkey, aList, anR );
    for( i=0; i< aList.GetCount(); i++)
        TDualTh::pm->DeleteRecord(aList[i].c_str());

    /*   aList.Clear();    //MASTRANSP
       anR.Clear();
       rt[RT_].MakeKey( RT_PARAM, pkey, RT_PARAM, 0,
               K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_END);
       rt[RT_].GetKeyList( pkey, aList, anR );
       for( i=0; i< aList.GetCount(); i++)
        aMod[RT_].DeleteRecord(aList[i].c_str());
    */
    rt[nRT].Del( Rnum );
}

//Show IComp, DComp or other reactions from list
void TProfil::ShowDBWindow( const char *objName, int nLine )
{
    gstring s;
    time_t tr;
    const char* title = "Demonstrate in calculate Mode.";

    switch( *objName )
    {
    case 'I': // Icomp : IC_v__ or ICnam
        if( strncmp(objName, aObj[o_musb].GetKeywd(), MAXKEYWD)==0)
            s = gstring( mup->SB[nLine], 0, IC_RKLEN );
        else  if( strncmp(objName, aObj[o_wd_sb].GetKeywd(), MAXKEYWD)==0 ||
                  strncmp(objName, aObj[o_w_sbh].GetKeywd(), MAXKEYWD)==0 )
            s = gstring( mup->SB[pmp->mui[nLine]], 0, IC_RKLEN );
        else break;
        TIComp::pm->RecInput( s.c_str() );
        TIComp::pm->Show(window(), title, true);
        break;
    case 'C': // Compod : CC_v__
        if( strncmp(objName, aObj[o_musa].GetKeywd(), MAXKEYWD)==0)
            s = gstring( mup->SA[nLine], 0, BC_RKLEN );
        else break;
        TCompos::pm->RecInput( s.c_str() );
        TCompos::pm->Show(window(), title, true);
        break;
    case 'P': // Phase : Ph_v__ or Ph_v2 or Phnam or Phnam2
        if( strncmp(objName, aObj[o_musf].GetKeywd(), MAXKEYWD)==0 ||
                strncmp(objName, aObj[o_musf2].GetKeywd(), MAXKEYWD)==0 )
            s = gstring( mup->SF[nLine], 0, PH_RKLEN );
        else  if( strncmp(objName, aObj[o_wd_sf].GetKeywd(), MAXKEYWD)==0 ||
                  strncmp(objName, aObj[o_wd_sf2].GetKeywd(), MAXKEYWD)==0 )
            s = gstring( mup->SF[pmp->muk[nLine]], 0, PH_RKLEN );
        else break;
        TPhase::pm->RecInput( s.c_str() );
        TPhase::pm->Show(window(), title, true);
        break;
    case 'D': // Phase : DC_v__ or DC_v2 or DCnam or DCnam2
        if( strncmp(objName, aObj[o_musm].GetKeywd(), MAXKEYWD)==0 ||
                strncmp(objName, aObj[o_musm2].GetKeywd(), MAXKEYWD)==0 )
            s = gstring( mup->SM[nLine], 0, DC_RKLEN );
        else  if( strncmp(objName, aObj[o_wd_sm].GetKeywd(), MAXKEYWD)==0 ||
                  strncmp(objName, aObj[o_wd_sm2].GetKeywd(), MAXKEYWD)==0 )
        {
            nLine = pmp->muj[nLine];
            s = gstring( mup->SM[nLine], 0, DC_RKLEN );
        }
        else break;
        if( mup->DCS[nLine] == SRC_DCOMP )
        {
            //        TDComp::pm->RecInput( s.c_str() );
            TDComp::pm->TryRecInp( s.c_str(), tr, 0 );
            TDComp::pm->Show(window(), title, true);
        }
        else
        {
            //              TReacDC::pm->RecInput( s.c_str() );
            TReacDC::pm->TryRecInp( s.c_str(), tr, 0 );
            TReacDC::pm->Show(window(), title, true);
        }
        break;
    }
}

#include "filters_data.h"
// Save file configuration to Project structure
bool TProfil::rCopyFilterProfile( const char * prfName )
{

    uint ii;
    setFiltersData sf_data;
    elmWindowData  elm_data;

//    Save list of phases from template profile:
    TCStringArray names1;
    names1.Add(prfName);
    rt[RT_ICOMP].OpenOnlyFromList(names1);
    rt[RT_PHASE].OpenOnlyFromList(names1);

   TCStringArray PHkeys;
   TCIntArray    ICcnt;
   rt[RT_PHASE].GetKeyList( "*:*:*:*:*:", PHkeys, ICcnt );

//    TCStringArray dbNames;
//    bool aAqueous, aGaseous, aSorption;
//    if( !vfElements(window(), prfName, ICkeys,
//            dbNames, aAqueous, aGaseous, aSorption ))
    if( !vfElements(window(), prfName, elm_data, sf_data ))
      return false;

//    elm_data.flNames.Add(prfName);
    pVisor->Message( 0, "Loading Modelling Project",
"Copying Kernel database records to Modelling Project;\n Please, wait...", 10  );

    // added to project file icomp.kernel.prfname
    // and copy to it selected records
    // add to last key field first symbol from prfname
    // close all kernel files
    TIComp* aICdata=(TIComp *)(&aMod[RT_ICOMP]);
    aICdata->CopyElements( prfName, elm_data, sf_data.ic_d );
    ICcnt.Clear();
    for( ii=0; ii<elm_data.ICrds.GetCount(); ii++ )
       ICcnt.Add(0);

    //compos
    TCompos* aCOdata=(TCompos *)(&aMod[RT_COMPOS]);
    TCStringArray aCMnoused;
    aCOdata->CopyRecords( prfName, aCMnoused, elm_data, sf_data.cm_d );

    //dcomp
    TDComp* aDCdata=(TDComp *)(&aMod[RT_DCOMP]);
    aDCdata->CopyRecords( prfName, ICcnt, elm_data, sf_data.dc_d );

    //reacds
    TReacDC* aRDdata=(TReacDC *)(&aMod[RT_REACDC]);
    aRDdata->CopyRecords( prfName, ICcnt, elm_data, sf_data.rd_d );

    //phase
    TPhase* aPHdata=(TPhase *)(&aMod[RT_PHASE]);
    TCStringArray aPHnoused;
    aPHdata->CopyRecords( prfName, aPHnoused, PHkeys, elm_data, sf_data.ph_d );

    //show errors
    TCStringArray aICnoused;
    for( ii=0; ii<elm_data.ICrds.GetCount(); ii++ )
       if( ICcnt[ii] == 0 )
         aICnoused.Add(elm_data.ICrds[ii]);

    if( aICnoused.GetCount() > 0 )
      vfChoice(  window(), aICnoused, "List of unused Independent Components" );

    if( aPHnoused.GetCount() > 0 || aCMnoused.GetCount() > 0)
    {  // List of Phases or Compos with some species discarded
        ios::openmode mod = ios::out;
        const char *filename = "DiscardedRecords.txt";
// This question is not needed anymore  DK 27.10.2005
/*      if( !(::access( filename, 0 )) ) //file exists
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
                return true;
            }
*/
        fstream f( filename, mod );
        ErrorIf( !f.good() , filename, "Fileopen error");
        f <<   "Discarded Phase records\n";
        for( ii=0; ii<aPHnoused.GetCount(); ii++ )
             f << aPHnoused[ii].c_str() <<  "\n";
        f <<   "\n\nDiscarded Compos records\n";
        for( ii=0; ii<aCMnoused.GetCount(); ii++ )
             f << aCMnoused[ii].c_str() <<  "\n";
        f <<   "\n";
        ErrorIf( !f.good() , filename, "Writefile error");
   }

    useAqPhase = elm_data.flags[cbAqueous_];
    useGasPhase = elm_data.flags[cbGaseous_];
    return true;
}

// GEM IPM calculation of equilibrium state in MULTI
void TProfil::calcMulti()
{
    // MultiCalcInit( keyp.c_str() );
    multi->CompG0Load();
    if( multi->AutoInitialApprox() == false )
        multi->MultiCalcIterations();


}




// Setup of flags for MULTY remake
// pNP,  //Mode of FIA selection: 0-auto-SIMPLEX,1-old eqstate,-1-user's choice
// pESU, // Unpack old eqstate from EQSTAT record?  0-no 1-yes
// pIPN, // State of IPN-arrays:  0-create; 1-available; -1 remake
// pBAL, // State of reloading CSD:  1- BAL only; 0-whole CSD
// pFAG, //State of initial lnGam load: 0-no, 1-on Mbel, 2-lnGmf, -1-SurEta
// pTPD, // State of reloading thermod data: 0- all    2 - no
// pULR, // reserved
void TProfil::PMtest( const char *key )
{
    double T, P;
    TSysEq* STat = (TSysEq*)(&aMod[RT_SYSEQ]);
    TProcess* Proc = (TProcess*)(&aMod[RT_PROCES]);
//    TUnSpace* Prob = (TUnSpace*)(&aMod[RT_UNSPACE]);

    ///  pmp->pNP = -1;
    if( STat->ifCalcFlag())
    { if( !pmp->pESU )
          pmp->pESU = 1;
    }
    else pmp->pESU = 0;

    if( pmp->pESU == 0 ) // no old solution
        pmp->pNP = 0;

    if( Proc->pep->Istat == P_EXECUTE ||
        Proc->pep->Istat == P_MT_EXECUTE )
    {
        if(Proc->pep->PvR1 == S_OFF )
            pmp->pNP = 0;
        else
            pmp->pNP = 1;
    }
/*    if( Prob->usp->pbcalc == P_EXECUTE )    ???????
    {
        if(Prob->usp->zond[11] == S_OFF )
            pmp->pNP = 0;
        else
            pmp->pNP = 1;
    }
*/    pmp->pBAL =  BAL_compare();
    if( !pmp->pBAL )
        pmp->pIPN = 0;
//    if( qEp.GetCount()<1 || qEd.GetCount()<1 )  Caution!
    if( multi->qEp.GetCount()<1 && multi->qEd.GetCount()<1 && !pmp->sitE )
        pmp->pIPN = 0;
    // Get P and T from key
    gstring s = gstring( key,MAXMUNAME+MAXTDPCODE+MAXSYSNAME+MAXTIME+MAXPTN,MAXPTN);
    P = atof(s.c_str());
    s = gstring( key,MAXMUNAME+MAXTDPCODE+MAXSYSNAME+MAXTIME+MAXPTN+MAXPTN,MAXPTN);
    T = atof(s.c_str());

    if( fabs( tpp->curT - T ) > 1.e-10 ||
            fabs( tpp->curP - P ) > 1.e-10 )
    { // load new MTPARM on T or P
        mtparm->LoadMtparm( T, P );
        pmp->pTPD = 0;
    }
}

void TProfil::LoadFromMtparm(double T, double P,double *G0,
        double *V0, double *H0, double *S0, double *Cp0, double &roW, double &epsW )
{
    if( fabs( tpp->curT - T ) > 1.e-10 ||
            fabs( tpp->curP - P ) > 1.e-10 )
    { // load new MTPARM on T or P
        mtparm->LoadMtparm( T, P );
        pmp->pTPD = 0;
    }

    roW = tpp->RoW;
    epsW = tpp->EpsW;
    for( int jj=0; jj<mup->L; jj++ )
    {
      G0[jj] =  tpp->G[jj];
      V0[jj] =  tpp->Vm[jj];
      if( H0 && tpp->H )
        H0[jj] =  tpp->H[jj];
      if( S0 && tpp->S )
        S0[jj] =  tpp->S[jj];
      if( Cp0 && tpp->Cp )
         Cp0[jj] =  tpp->Cp[jj];
    }
}

// -------------------------------------------------------------------
// Compare changes in the modified system relative to MULTI
// if some vectors were allocated or some dimensions changed - return 0;
// else if bulk composition or some constraints has changed - return 1;
// else if nothing has changed - return 2.
short TProfil::BAL_compare()
{
    int i,j,k;
    /* test sizes */
    if( pmp->N != syp->N || pmp->L != syp->L || pmp->Ls != syp->Ls
            || pmp->LO != syp->Lw || pmp->PG != syp->Lg
            || pmp->PSOL != syp->Lhc || pmp->Lads != syp->Lsor
            || pmp->FI != syp->Fi || pmp->FIs != syp->Fis )
        return 0;
    if(( syp->DLLim == S_ON || syp->DULim == S_ON ) && pmp->PLIM != 1 )
        return 0;
    else if( syp->DLLim == S_OFF && syp->DULim == S_OFF && pmp->PLIM == 1 )
        return 0;

    /* test selectors */
    for( i=0; i<pmp->N; i++ )
        if( syp->Icl[pmp->mui[i]] == S_OFF )
            return 0;
    for( j=0; j<pmp->L; j++ )
        if( syp->Dcl[pmp->muj[j]] == S_OFF )
            return 0;
    for( k=0; k<pmp->FI; k++ )
        if( syp->Pcl[pmp->muk[k]] == S_OFF )
            return 0;
    /* lists of components didn't change */
    /* test B */
    for( i=0; i<pmp->N; i++ )
        if( fabs( syp->B[pmp->mui[i]] - pmp->B[i] ) >= pa.p.DB )
            return 1;
    /* test other restrictions */
    for( j=0; j<pmp->L; j++ )
    {
        if( syp->PGEX != S_OFF )
            if( fabs( syp->GEX[pmp->muj[j]] - pmp->GEX[j]*pmp->RT ) >= 0.001 )
                break;
        if(( syp->DLLim != S_OFF ) && pmp->PLIM == 1 )
            if( fabs( syp->DLL[pmp->muj[j]] - pmp->DLL[j] ) >= 1e-19 )
                break;
        if(( syp->DULim != S_OFF ) && pmp->PLIM == 1 )
            if( fabs( syp->DUL[pmp->muj[j]] - pmp->DUL[j] ) >= 1e-19 )
              break;
        if( syp->DULim != S_OFF || syp->DLLim != S_OFF )
        {  if( pmp->RLC[j] != syp->RLC[pmp->muj[j]] )
             break;
           if( pmp->RSC[j] != syp->RSC[pmp->muj[j]] )
             break;
        }
    }
    if( j < pmp->L )
      return 1;

    if( pmp->FIat > 0 )  //      Adsorption models - always
       return 1;

    for( k=0; k<pmp->FI; k++ )
    {
      int kk = pmp->muk[k];
      if( syp->PSigm != S_OFF )
      {
         if( fabs( pmp->Sigw[k] - syp->Sigm[kk][0]) > 1e-19 )
           break;
         if( fabs( pmp->Sigg[k] - syp->Sigm[kk][1]) > 1e-19 )
           break;
      }
      if( syp->PAalp != S_OFF )
        if( fabs( pmp->Aalp[k] - syp->Aalp[kk]) > 1e-19 )
         break;
    }

    if( k < pmp->FI )
        return 1;

    // bulk chem. compos. and constraints unchanged
    return 2;
}

// ------------------ End of m_param.cpp -----------------------




