//-------------------------------------------------------------------
// $Id: m_param.cpp 1406 2009-08-21 08:37:56Z gems $
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
// See http://gems.web.psi.ch/ for more information
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
//#include "m_dcomp.h"
//#include "m_icomp.h"
//#include "m_compos.h"
//#include "m_phase.h"
//#include "m_reacdc.h"
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
                            ln_to_lg = 0.434294481,
                              H2O_mol_to_kg = 55.50837344,
                                Min_phys_amount = 1.66e-24;

SPP_SETTING pa_ = {
    "GEM-Selektor v3.0.0: Numerical flags, controls and thresholds",
    {   // Typical default set (16.05.2010) new PhaseSelection( logSI )
        2,  /* PC */  2,     /* PD */   -4,   /* PRD */
        1,  /* PSM  */ 130,  /* DP */   1,   /* DW */
        1, /* DT */     10,   /* PLLG */   1,  /* PE */  7000, /* IIM */
        1000., /* DG */   1e-11,  /* DHB */  1e-20,  /* DS */
        1e-5,  /* DK */  0.01,  /* DF */  0.01,  /* DFM */
        1e-5,  /* DFYw */  1e-5,  /* DFYaq */    1e-5,  /* DFYid */
        1e-5,  /* DFYr,*/  1e-5,  /* DFYh,*/   1e-5,  /* DFYc,*/
        1e-6, /* DFYs, */  1e-17,  /* DB */   -1.,   /* AG */
        -0.98,   /* DGC */   1.0,   /* GAR */  1000., /* GAH */
        1e-3, /* GAS */   12.05,  /* DNS */   1e-11,  /* XwMin, */
        1e-11,  /* ScMin, */  1e-30, /* DcMin, */   1e-20, /* PhMin, */
        1e-5,  /* ICmin */   1e-10,  /* EPS */   1e-3,  /* IEPS */
        1e-8,  /* DKIN  */ 0,  /* tprn */
    },
    "CSC   ",   /* DCpct[6] */  "OjjbC+---",    /* DCpdc[9] */
    "+----M",   /* BCpc[6] */   "K3C   ",   /* REpct[6] */
    "SjjbC ",   /* REpdc[6]*/  "+-------", /* REpvc[8]  */
    "rjjbC-TCb-",   /* RPpdc[10]  */
    "++*-1-23----------4-------------", /* RPpvc[32] reserved free */
    "INNINN",   /* PHsol_t[6] */  "s-----",   /* PHpvc[6] */
    "++++-+-+++", /* MUpmv[10] */ "jjbC++0+", /* TPpdc[8] */
    "*-*-*-*-*-----------", /* TPpvc[20] */ "+-+-+-----", /* SYppc[10] */
    "*****-*---**-***-----------*", /* SYpvc[28]*/  "-+++----ME", /* UTppc[10] */
    "0*-------P--", /* PEpsc[12]  */  "------------", /* PEpvc[12] */
    { "GTDEMO task name   ", "Graphic screen # " } ,   /* GDcode[2][20] */
    "Plot ",                  /* GDpsc[7] */
    { "Abscissa","Ordinate"},    /* GDpcc[2][9] */
    "++++-+",   /* GDptc[6] */  "++++--",   /* GDpgw[6] */
    "*",   /* sdrefkey 32*/  "",   /* Reserv [50-32] */
    /* RTPARM */
    1, 15, 0,   /* P T V */  2,  /* Mode */
    { 1, 2, 3, 4, 5, },  /* ResShort */
    /* RTPARM */
    {1., 1., 0.}, /* Pi[3]*/  {0.0, 375., 25.}, /* Ti[3] */
    {0., 0., 0.},   /* Vi[3] */
    1., 25.,      /* Pr, Tr */
    /* MULTI */
    (float)1.66e-24, /* lowPosNum, */  (float)1e-16, /* logXw, */   (float)1e-9,  /* logYFk */
    { (float)0.064, (float)3.72, 0., 0., 0. },  // Defaults for aqueous auto-phase models
}; /* SPP_SETTING */

BASE_PARAM dfBase[5] =
	{     // Added on 07.08.2009 to facilitate pre-setting in projects (SD,DK)
            { // Variant for aquatic systems with moderate non-ideality
                2,  /* PC */  2,     /* PD */   -4,   /* PRD */
                1,  /* PSM  */ 130,  /* DP */   1,   /* DW */
                1, /* DT */     10,   /* PLLG */   1,  /* PE */
                7000,   /* IIM */
                1000., /* DG */   1e-12,  /* DHB */  1e-20,  /* DS */
                1e-5,  /* DK */  0.01,  /* DF */  0.01,  /* DFM */
                1e-5,  /* DFYw */  1e-5,  /* DFYaq */    1e-5,  /* DFYid */
                1e-5,  /* DFYr,*/  1e-5,  /* DFYh,*/   1e-5,  /* DFYc,*/
                1e-6, /* DFYs, */  1e-17,  /* DB */   -1.,   /* AG */
                -0.98,   /* DGC */   1.0,   /* GAR */  1000., /* GAH */
                1e-3, /* GAS */   12.05,  /* DNS */   1e-11,  /* XwMin, */
                1e-11,  /* ScMin, */  1e-30, /* DcMin, */   1e-20, /* PhMin, */
                1e-5,  /* ICmin */   1e-10,  /* EPS */   1e-3,  /* IEPS */
                1e-8,  /* DKIN  */ 0,  /* tprn */
	    },
            { // Variant for aquatic systems with SCMs in sorption phases (16.05.2010)
                2,  /* PC */  2,     /* PD */   -4,   /* PRD */
                1,  /* PSM  */ 130,  /* DP */   1,   /* DW */
                1, /* DT */     10,   /* PLLG */   1,  /* PE */
                7000,   /* IIM */
                1000., /* DG */   1e-11,  /* DHB */  1e-20,  /* DS */
                1e-5,  /* DK */  0.01,  /* DF */  0.01,  /* DFM */
                1e-5,  /* DFYw */  1e-5,  /* DFYaq */    1e-5,  /* DFYid */
                1e-5,  /* DFYr,*/  1e-5,  /* DFYh,*/   1e-5,  /* DFYc,*/
                1e-6, /* DFYs, */  1e-17,  /* DB */   1.,   /* AG */
                -0.001,   /* DGC */   1.0,   /* GAR */  1000., /* GAH */
                1e-3, /* GAS */   12.05,  /* DNS */   1e-11,  /* XwMin, */
                1e-11,  /* ScMin, */  1e-30, /* DcMin, */   1e-20, /* PhMin, */
                1e-5,  /* ICmin */   1e-10,  /* EPS */   1e-3,  /* IEPS */
                1e-8,  /* DKIN  */ 0,  /* tprn */
	    },
            { // Variant for fluid-rock systems with highly non-ideal phases (16.05.2010)
                2,  /* PC */  2,     /* PD */   -4,   /* PRD */
                1,  /* PSM  */ 130,  /* DP */   1,   /* DW */
                1, /* DT */     10,   /* PLLG */   1,  /* PE */
                7000,   /* IIM */
                1000., /* DG */   1e-11,  /* DHB */  1e-20,  /* DS */
                1e-5,  /* DK */  0.01,  /* DF */  0.01,  /* DFM */
                1e-5,  /* DFYw */  1e-5,  /* DFYaq */    1e-5,  /* DFYid */
                1e-5,  /* DFYr,*/  1e-5,  /* DFYh,*/   1e-5,  /* DFYc,*/
                1e-6, /* DFYs, */  1e-17,  /* DB */   1.,   /* AG */
                0.01,   /* DGC */   1.0,   /* GAR */  1000., /* GAH */
                1e-4, /* GAS */   12.05,  /* DNS */   1e-11,  /* XwMin, */
                1e-11,  /* ScMin, */  1e-30, /* DcMin, */   1e-20, /* PhMin, */
                1e-5,  /* ICmin */   1e-10,  /* EPS */   1e-3,  /* IEPS */
                1e-8,  /* DKIN  */ 0,  /* tprn */
	    },
            { // Variant of strict settings for highest accuracy (16.05.2010)
                2,  /* PC */  2,     /* PD */   -4,   /* PRD */
                1,  /* PSM  */ 130,  /* DP */   1,   /* DW */
                1, /* DT */     10,   /* PLLG */   1,  /* PE */
                7000,   /* IIM */
                1000., /* DG */   1e-12,  /* DHB */  1e-20,  /* DS */
                3e-6,  /* DK */  0.01,  /* DF */  0.01,  /* DFM */
                1e-5,  /* DFYw */  1e-5,  /* DFYaq */    1e-5,  /* DFYid */
                1e-5,  /* DFYr,*/  1e-5,  /* DFYh,*/   1e-5,  /* DFYc,*/
                1e-6, /* DFYs, */  1e-17,  /* DB */   -1.,   /* AG */
                -0.98,   /* DGC */   1.0,   /* GAR */  1000., /* GAH */
                1e-4, /* GAS */   12.05,  /* DNS */   1e-11,  /* XwMin, */
                1e-11,  /* ScMin, */  1e-30, /* DcMin, */   1e-20, /* PhMin, */
                1e-5,  /* ICmin */   1e-10,  /* EPS */   1e-3,  /* IEPS */
                1e-8,  /* DKIN  */ 0,  /* tprn */
            },
            { // Variant of strict settings for reactive mass transport (interim, 08.12.2009)
                2,  /* PC */  2,     /* PD */   -4,   /* PRD */
                1,  /* PSM  */ 130,  /* DP */   1,   /* DW */
                1, /* DT */     10,   /* PLLG */   1,  /* PE */
                7000,   /* IIM */
                1000., /* DG */   1e-12,  /* DHB */  1e-20,  /* DS */
                1e-5,  /* DK */  0.01,  /* DF */  0.01,  /* DFM */
                1e-5,  /* DFYw */  1e-5,  /* DFYaq */    1e-5,  /* DFYid */
                1e-5,  /* DFYr,*/  1e-5,  /* DFYh,*/   1e-5,  /* DFYc,*/
                1e-6, /* DFYs, */  1e-17,  /* DB */   1.,   /* AG */
                -0.001,   /* DGC */   1.0,   /* GAR */  1000., /* GAH */
                1e-3, /* GAS */   12.05,  /* DNS */   1e-11,  /* XwMin, */
                1e-11,  /* ScMin, */  1e-30, /* DcMin, */   1e-20, /* PhMin, */
                1e-5,  /* ICmin */   1e-10,  /* EPS */   1e-3,  /* IEPS */
                1e-8,  /* DKIN  */ 0,  /* tprn */
            }
	    // More preset parameter structures can be added here
            // (if so, increase number of elements from 5 above and in dfBase() call below)
	};

// Setup one of 5 default IPM numerical settings
void TProfil::ChangeSettings(int nSettings)
{
	if( nSettings == 0  )
	 return;
	else if( nSettings == 1 )
		    pa.p = pa_.p;
	     else
                pa.p = dfBase[min(nSettings-2,5)];
}

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
pmulti = multi;
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
    if( pa.p.DG <1 )                     // SD 07/05/2010
        pa.p.DG = 1e4;
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

/* opens window with 'Remake record' parameters
*/
void TProfil::MakeQuery()
{
    const char * p_key;
    char flgs[38];
    int tasktype = 0;

    p_key  = db->PackKey();
    // flgs 0-9 MSpmv, 10-29 TPptv, 30-33  TPun, 34-37 TPsv
    memcpy( flgs, &mup->PmvSA, 10);
    memcpy( flgs+10, &tpp->PtvG, 20);
    memcpy( flgs+30, &tpp->PunE, 8);

    if( !vfProjectSet( window(), p_key, flgs, tasktype ))
         Error( p_key, "Project record configuration cancelled by the user!" );

    memcpy( &mup->PmvSA, flgs,    10);
    memcpy( &tpp->PtvG,  flgs+10, 20);
    memcpy( &tpp->PunE,  flgs+30, 8);
    // SD 07/08/2009 set BASE_PARAM from default
    ChangeSettings(tasktype);
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
    multi->to_file( ff/*, path*/ );
}

// outpu MULTI to txt format
// brief_mode - Do not write data items that contain only default values
// with_comments -Write files with comments for all data entries ( in text mode)
// addMui - Print internal indices in RMULTS to IPM file for reading into Gems back
void TProfil::outMulti( gstring& path, bool addMui, bool with_comments, bool brief_mode )
{
    multi->to_text_file_gemipm( path.c_str(), addMui, with_comments, brief_mode );
}

void TProfil::outMultiTxt( const char *path, bool append  )
{
    multi->to_text_file( path, append );
}

void TProfil::makeGEM2MTFiles(QWidget* par )
{
    TNodeArray* na = 0;
    float* Tval = 0;
    float* Pval = 0;

    try
	 {
      // set default data and realloc arrays
      char flags[4];
      int ii, nT, nP;
      float Tai[4], Pai[4];

      if( !vfLookupDialogSet(par, flags, nT, nP, Tai, Pai ) )// ask constants
        	return;
      //  define setup arrays
 	  float cT = Tai[START_];
 	  float cP = Pai[START_];
       Tval = new float[nT];
       Pval = new float[nP];

 	 for( ii=0; ii<nT; ii++ )
 	 {
 	     Tval[ii] = cT;
 	     cT += Tai[STEP_];
 	 }

 	 for( ii=0; ii<nP; ii++ )
 	 {
 	     Pval[ii] = cP;
 	     cP += Pai[STEP_];
 	 }

     na = new TNodeArray( 1, TProfil::pm->pmp/*multi->GetPM()*/ );

    // realloc and setup data for dataCH and DataBr structures
    na->MakeNodeStructures( par, ( flags[0] == S_OFF ) ,
		    Tval, Pval, nT, nP, Tai[3], Pai[3]  );

    // setup dataBR and NodeT0 data
   //na->packDataBr();
   na->MoveWorkNodeToArray( 0, 1, na->pNodT0() );
   // make  all files
   na->PutGEM2MTFiles( par, 1,  ( flags[1] == S_ON ), ( flags[2] == S_ON ),
		   ( flags[3] == S_ON ), false, true );// addMui, to txt
   }
    catch( TError& xcpt )
    {
      if( na )
       delete na;
      na = 0;
      if( Tval )
       delete[] Tval;
      Tval = 0;
      if( Pval )
       delete[] Pval;
      Pval = 0;
       Error(  xcpt.title.c_str(), xcpt.mess.c_str() );
    }
    if( na )
     delete na;
    na = 0;
    if( Tval )
     delete[] Tval;
    Tval = 0;
    if( Pval )
     delete[] Pval;
    Pval = 0;
}

// Reading structure MULTI (GEM IPM work structure)
void TProfil::readMulti( GemDataStream& ff )
{

      ff.readArray( &pa.p.PC, 10 );
      ff.readArray( &pa.p.DG, 28 );
      multi->from_file( ff );
}

// Reading structure MULTI (GEM IPM work structure)
void TProfil::readMulti( const char* path )
{
      multi->from_text_file_gemipm( path);
}

// Reading structure MULTI (GEM IPM work structure)
void TProfil::CmReadMulti( QWidget* par, const char* path )
{
    TNodeArray* na = new TNodeArray( 1, TProfil::pm->pmp/*multi->GetPM()*/ );
    if( na->GEM_init( path ) ) // mui, muj and muk arrays not cleared!!!!
    {
      Error( path , "Chemical system definition files listed in this file \n"
   		                     " cannot be found or are corrupt");
    }
   // Test sizes and flags
   // my be diffirent with internal (do not test):
   // syp->PAalp, syp->PSigm, pmp->FIat, pmp->E
    int iRet = BAL_compare();
    switch(iRet)
    {
      case 0: // Bad base task for load - different sizes
          vfMessage(par, "Changes in the GEMIPM2K system setup relative to GEMS",
                    "Some vectors were allocated or some dimensions changed.\n"
                    "The equilibrium cannot be recalculated recalculate");
          break;
       case 1: // different B
          //vfMessage(par, "Changes in the GEMIPM2K system setup relative to GEMS",
          //          "Some constraints are not the same.\n"
          //          "Different results may be obtained upon recalculation");
       default:
          break;
    }

    // get scaling to constant mass of internal system
    //float inMass = 1., scFactor =1.;
    //inMass = vfGetDouble( par, "Scaling to constant internal mass of the system",
    //  "Internal mass (kg)", inMass, 1e-5, 1e5, 4 );
    //if( na->cMs() > 1e-20 )
    //    scFactor = inMass / na->cMs();
    // setup from dataBR to Multi
    na->unpackDataBr( true );
    delete na;

    // We can get different results in GEMS than in GEMIPM2K
    // because of slightly different values into G, V ...
    // (interpolation of thermodynamic data or precision )
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


long int showMss = 1L;
// test result GEM IPM calculation of equilibrium state in MULTI
long int TProfil::testMulti()
{
  if( pmp->MK || pmp->PZ )
  {
   if( pa.p.PSM == 2 )
   {
     fstream f_log("ipmlog.txt", ios::out|ios::app );
     f_log << "Warning " << pmp->stkey << ": " <<  pmp->errorCode << ":" << endl;
     f_log << pmp->errorBuf << endl;
   }
   if( showMss )
   {
	   multi->addErrorMessage(" \nContinue?");
      switch( vfQuestion3(0, pmp->errorCode, pmp->errorBuf,
                           "&Yes", "&No", "&Yes to All" ))
       {
       case VF3_3:
    	   showMss=0l;
       case VF3_1:
           break;
       case VF3_2:
    	   Error(pmp->errorCode, pmp->errorBuf);
       }
   }

   return 1L;
  }

  return 0L	;
}

// Load Thermodynamic data from Database
void TProfil::CheckMtparam()
{

  if( fabs( tpp->T - pmp->TCc ) > 1.e-10 ||
         fabs( tpp->P - pmp->Pc ) > 1.e-9 )
   { // load new MTPARM on T or P
      mtparm->LoadMtparm( pmp->TCc, pmp->Pc );
      pmp->pTPD = 0;
   }

}


/*// Load Thermodynamic data from Lookup Arrays
void TProfil::CheckMtparamFromLookup()
{

  if( fabs( tpp->T - pmp->TCc ) > 1.e-10 ||
      fabs( tpp->P - pmp->Pc ) > 1.e-9 )
   { // load new MTPARM on T or P
//      mtparm->LoadMtparmLookup( pmp->TCc, pmp->Pc );
      pmp->pTPD = 0;
   }

}

// Build Lookup Arrays internal data
void TProfil::BuildLookupArrays( )
{

//  mtparm->LoadLookup( pmp->Tai, pmp->Pai );

}
*/


// GEM IPM calculation of equilibrium state in MULTI
// without test changings in system
//
double TProfil::calcMulti( long int& NumPrecLoops, long int& NumIterFIA, long int& NumIterIPM )
{
  TSysEq* STat = (TSysEq*)(&aMod[RT_SYSEQ]);
  calcFinished = false;

  /*if( fabs( tpp->curT - pmp->TCc ) > 1.e-10 ||
         fabs( tpp->curP - pmp->Pc ) > 1.e-10 )
   { // load new MTPARM on T or P
      mtparm->LoadMtparm( pmp->TCc, pmp->Pc );
      pmp->pTPD = 0;
   }
  */

  multi->calcEqustat( 0, NumIterFIA, NumIterIPM );

  calcFinished = true;
  STat->setCalcFlag( true );
  STat->CellChanged();

  return pmp->t_elap_sec;
}

// Setup of flags for MULTY remake
// pNP,  //Mode of FIA selection: 0-auto-SIMPLEX,1-old eqstate,-1-user's choice
// pESU, // Unpack old eqstate from EQSTAT record?  0-no 1-yes
// pIPN, // State of IPN-arrays:  0-create; 1-available; -1 remake
// pBAL, // State of reloading CSD:  1- BAL only; 0-whole CSD
// pTPD, // State of reloading thermod data: 0- all    2 - no
void TProfil::PMtest( const char *key )
{
    //double V, T, P;
    TSysEq* STat = (TSysEq*)(&aMod[RT_SYSEQ]);
    TProcess* Proc = (TProcess*)(&aMod[RT_PROCES]);

    // test for available old solution
    if( STat->ifCalcFlag() )
    { if( !pmp->pESU )      // if pESU == 2 (task loaded before), left value
          pmp->pESU = 1;
    }
    else pmp->pESU = 0;

   // no old solution => must be simplex
   if( pmp->pESU == 0 )
        pmp->pNP = 0;

   // test changes in the modified system relative to MULTI
   pmp->pBAL =  BAL_compare();
   if( !pmp->pBAL ) // if some vectors were allocated or some dimensions changed
   {
       pmp->pIPN = 0;
       pmp->pTPD = 1; // reload Go, Vol
   }

    // special setup from process simulator
    if( Proc->pep->Istat == P_EXECUTE ||
        Proc->pep->Istat == P_MT_EXECUTE )
    {
        if(Proc->pep->PvR1 == S_OFF )
            pmp->pNP = 0;
        else
            pmp->pNP = 1;
    }

    multi->MultiKeyInit( key );
    /* Get V, P and T from SysEq record key
    gstring s = gstring( key,MAXMUNAME+MAXTDPCODE+MAXSYSNAME+MAXTIME,MAXPTN);
    V = atof(s.c_str());
    s = gstring( key,MAXMUNAME+MAXTDPCODE+MAXSYSNAME+MAXTIME+MAXPTN,MAXPTN);
    P = atof(s.c_str());
    s = gstring( key,MAXMUNAME+MAXTDPCODE+MAXSYSNAME+MAXTIME+MAXPTN+MAXPTN,MAXPTN);
    T = atof(s.c_str());

    //if( fabs ( pmp->VE - V ) > 1.e-10 )
    //    pmp->VE = V;
    if( fabs( tpp->curT - T ) > 1.e-10 ||
            fabs( tpp->curP - P ) > 1.e-10 )
    { // load new MTPARM on T or P
        mtparm->LoadMtparm( T, P );
        pmp->pTPD = 0;
    }
    */
}


// -------------------------------------------------------------------
// Compare changes in the modified system relative to MULTI
// if some vectors were allocated or some dimensions changed - return 0;
// else if bulk composition or some constraints has changed - return 1;
// else if nothing has changed - return 2.
short TProfil::BAL_compare()
{
    int i,j,k, jj, jb, je=0;
    double Go, Gg, Ge, pGo;

// Test A - sizes and selectors
    if( pmp->N != syp->N || pmp->L != syp->L || pmp->Ls != syp->Ls || pmp->LO != syp->Lw
            || pmp->PG != syp->Lg // || pmp->E != syp->PE  // check of E added 08.02.2010 DK
            || pmp->PSOL != syp->Lhc || pmp->Lads != syp->Lsor
            || pmp->FI != syp->Fi || pmp->FIs != syp->Fis )
        return 0;
    if( !((syp->PE != S_OFF && pmp->E == 1) || (syp->PE == S_OFF && pmp->E == 0))  )
        return 0;

    if(( syp->DLLim == S_ON || syp->DULim == S_ON ) && pmp->PLIM != 1 )
        return 0;
    else if( syp->DLLim == S_OFF && syp->DULim == S_OFF && pmp->PLIM == 1 )
        return 0;
// test selectors
    for( i=0; i<pmp->N; i++ )
        if( syp->Icl[pmp->mui[i]] == S_OFF )
            return 0;
    for( j=0; j<pmp->L; j++ )
        if( syp->Dcl[pmp->muj[j]] == S_OFF )
            return 0;
    for( k=0; k<pmp->FI; k++ )
        if( syp->Pcl[pmp->muk[k]] == S_OFF )
            return 0;
// lists of components didn't change
// test B - recipes and constraints
    for( i=0; i<pmp->N; i++ )
        if( fabs( syp->B[pmp->mui[i]] - pmp->B[i] ) >= pa.p.DB )
            return 1;
    // test other settings for DCs
  for( k = 0; k < pmp->FI; k++ )
  {
	jb = je;
	je += pmp->L1[k];
	for( j=jb; j<je; j++ )
    {
       Gg = Ge = 0.0;    //   This part had to be changed after integrating Ge into pmp->G0
       jj = pmp->muj[j]; //        DK    07.03.2008,  16.05.2008
       Go = tpp->G[jj]; //  G0(T,P) value taken from MTPARM
       if( syp->Guns )  // This is used mainly in UnSpace calculations
           Gg = syp->Guns[jj];    // User-set increment to G0 from project system
       if( syp->GEX && syp->PGEX != S_OFF )   // User-set increment to G0 from project system
           Ge = syp->GEX[jj];     //now Ge is integrated into pmp->G0 (since 07.03.2008) DK
       pGo = multi->Cj_init_calc( Go+Gg+Ge, j, k );
       if( fabs( pGo - pmp->G0[j] )* pmp->RT >= 0.001 )
       {
           pmp->pTPD = 1;   // Fixed here to invoke CompG0Load() DK 16.05.2008
    	   break;   // GEX or Guns has changed for this DC in the system definition
       }
 //      if( syp->PGEX != S_OFF )
 //           if( fabs( syp->GEX[pmp->muj[j]] - pmp->GEX[j]*pmp->RT ) >= 0.001 )
 //               break;
        if(( syp->DLLim != S_OFF ) && pmp->PLIM == 1 )
//            if( fabs( (double)syp->DLL[jj] - pmp->DLL[j] ) >= 1e-19 )
              if( syp->DLL[jj] != (float)pmp->DLL[j]  )   //SD 22/01/2009
                break;
        if(( syp->DULim != S_OFF ) && pmp->PLIM == 1 )
//            if( fabs( (double)syp->DUL[jj] - pmp->DUL[j] ) >= 1e-19 )
              if( syp->DUL[jj] != (float)pmp->DUL[j] )   //SD 22/01/2009
              break;
        if( syp->DULim != S_OFF || syp->DLLim != S_OFF )
        {  if( pmp->RLC[j] != syp->RLC[jj] )
             break;
           if( pmp->RSC[j] != syp->RSC[jj] )
             break;
        }
    }  // j
    if( j < je )
       return 1;
  }  // k
 //   if( pmp->FIat > 0 )  //      Adsorption models - always
 //      return 1;

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

//// Clear syp->XeD, syp->XeA, syp->Phm and copy pmp->B to syp->BI
void TProfil::Clear_XeA_XeD_Phm_BIun()
{
    int i;
     // put Bb data to system  ( into B and bi_)
    if( syp->PbIC != S_OFF )
     for( i=0; i<mup->N; i++)
      {
        syp->BI[i] =  0; //pmp->B[];
        syp->BIun[i] =  QUAN_MOL;
      }
// set zeros to xd_ xp_ and xa_
    if( syp->PbDC != S_OFF )
        for( i=0; i < mup->L; i++)
              syp->XeD[i] = 0.;
    if( syp->PbAC != S_OFF && mup->La > 0 )
        for( i=0; i < mup->La; i++)
               syp->XeA[i] = 0.;
    if( syp->PbPH != S_OFF )
        for( i=0; i < mup->Fi; i++)
               syp->Phm[i] = 0.;
}

void TProfil::LoadFromMtparm(double T, double P,double *G0,  double *V0,
                double *H0, double *S0, double *Cp0, double *A0, double *U0,
                double denW[5], double epsW[5], double denWg[5], double epsWg[5], int* tp_mark_ )
{
    if( fabs( tpp->curT - T ) > 1.e-10 ||
            fabs( tpp->curP - P ) > 1.e-10 )
    { // load new MTPARM on T or P
        mtparm->LoadMtparm( T, P );
        pmp->pTPD = 0;
    }
    denW[0] = tpp->RoW;
    denW[1] = tpp->dRdTW;
    denW[2] = tpp->d2RdT2W;
    denW[3] = tpp->dRdPW;
    denW[4] = tpp->d2RdP2W;
    denWg[0] = tpp->RoV;
    denWg[1] = tpp->dRdTV;
    denWg[2] = tpp->d2RdT2V;
    denWg[3] = tpp->dRdPV;
    denWg[4] = tpp->d2RdP2V;
    epsW[0] = tpp->EpsW;
    epsW[1] = tpp->dEdTW;
    epsW[2] = tpp->d2EdT2W;
    epsW[3] = tpp->dEdPW;
    epsW[4] = tpp->d2EdP2W;
    epsWg[0] = tpp->EpsV;
    epsWg[1] = tpp->dEdTV;
    epsWg[2] = tpp->d2EdT2V;
    epsWg[3] = tpp->dEdPV;
    epsWg[4] = tpp->d2EdP2V;
    for( int jj=0; jj<mup->L; jj++ )
    {
      if( tpp->mark[jj] == 'e'/*CP_NOT_VALID*/ )
          tp_mark_[jj] = 1;
      G0[jj] =  tpp->G[jj]+syp->GEX[jj];
      V0[jj] =  tpp->Vm[jj];
      if( H0 )
      { if( tpp->H )
           H0[jj] =  tpp->H[jj];
        else
           H0[jj] = 0.;
      }
      if( S0 )
      {	 if( tpp->S )
            S0[jj] =  tpp->S[jj];
         else
                S0[jj] = 0.;
      }
      if( Cp0 )
      {	  if( tpp->Cp )
             Cp0[jj] =  tpp->Cp[jj];
          else
             Cp0[jj] =  0.;
      }
      if( A0 )
      {	 if( tpp->F )
            A0[jj] =  tpp->F[jj];
         else
                A0[jj] = 0.;
      }
      if( U0 )
      {	 if( tpp->U )
            U0[jj] =  tpp->U[jj];
         else
            U0[jj] = 0.;
      }
    }
}

// ------------------ End of m_param.cpp -----------------------




