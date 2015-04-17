//-------------------------------------------------------------------
// $Id: m_param.cpp 1406 2009-08-21 08:37:56Z gems $
//
// Implementation of config and basic methods of TProfile class
//
// Rewritten from C to C++ by S.Dmytriyeva
// Copyright (C) 1995-2010 S.Dmytriyeva, D.Kulik
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

#ifdef __unix
#include <unistd.h>
#endif

#include "v_object.h"
#include "m_param.h"
#include "ms_rmults.h"
#include "ms_mtparm.h"
#include "ms_system.h"
#include "ms_calc.h"
#include "m_syseq.h"
#include "m_proces.h"
#include "visor.h"
#include "gdatastream.h"
#include "nodearray.h"
#include "service.h"

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
extern char *_GEMS_version_stamp;
extern char *_GEMIPM_version_stamp;
SPP_SETTING pa_ = {
    " GEMS-GUI v.3.1 r.2184 (rc) " " GEMS3K v.3.1 r.710 (rc) ",
    {   // Typical default set (28.08.2012) new PSSC( logSI ) & uDD()
        2,  /* PC */  2,     /* PD */   -5,   /* PRD */
        1,  /* PSM  */ 130,  /* DP */   1,   /* DW */
        0, /* DT */     30000,   /* PLLG */   1,  /* PE */  7000, /* IIM */
        1000., /* DG */   1e-13,  /* DHB */  1e-20,  /* DS */
        1e-6,  /* DK */  0.01,  /* DF */  0.01,  /* DFM */
        1e-5,  /* DFYw */  1e-5,  /* DFYaq */    1e-5,  /* DFYid */
        1e-5,  /* DFYr,*/  1e-5,  /* DFYh,*/   1e-5,  /* DFYc,*/
        1e-6, /* DFYs, */  1e-17,  /* DB */   1.,   /* AG */
        0.,   /* DGC */   1.0,   /* GAR */  1000., /* GAH */
        1e-3, /* GAS */   12.05,  /* DNS */   1e-13,  /* XwMin, */
        1e-13,  /* ScMin, */  1e-33, /* DcMin, */   1e-20, /* PhMin, */
        1e-5,  /* ICmin */   1e-10,  /* EPS */   1e-3,  /* IEPS */
        1e-10,  /* DKIN  */ 0,  /* tprn */
    },
    "CSC   ",   /* DCpct[6] */  "OjjbC+---",    /* DCpdc[9] */
    "+----M",   /* BCpc[6] */   "K3C   ",   /* REpct[6] */
    "SjjbC ",   /* REpdc[6]*/  "+-------", /* REpvc[8]  */
    "rjjbC-TCb-",   /* RPpdc[10]  */
    "++*-1-23----------4-------------", /* RPpvc[32] reserved free */
    "INNINN",   /* PHsol_t[6] */  "s-----",   /* PHpvc[6] */
    "++++-+-+++", /* MUpmv[10] */ "jjbC++0+", /* TPpdc[8] */
    "*-*-*-*-*-----------", /* TPpvc[20] */ "+-+-+-----", /* SYppc[10] */
    "*****-*-*-**-***-----------*", /* SYpvc[28]*/  "-+++----ME", /* UTppc[10] */
    "0K-------P--", /* PEpsc[12]  */  "------------", /* PEpvc[12] */
    { "GTDEMO task name   ", "Graphic screen # " } ,   /* GDcode[2][20] */
    "Plot ",                  /* GDpsc[7] */
    { "Abscissa","Ordinate"},    /* GDpcc[2][9] */
    "++++-+",   /* GDptc[6] */  "++++--",   /* GDpgw[6] */
    "*",   /* sdrefkey 32*/  "",   /* Reserv [50-32] */
    /* RTPARM */
    1, 15, 0,   /* P T V */  2,  /* Mode */
    { 1, 2, 3, 4, 5, },  /* ResShort */
    /* RTPARM */
    {0., 1., 0.}, /* Pi[3]*/  {0.0, 350., 25.}, /* Ti[3] */
    {0., 0., 0.},   /* Vi[3] */
    1., 25.,      /* Pr, Tr */
    /* MULTI */
    (float)1.66e-24, /* lowPosNum, */  (float)1e-16, /* logXw, */   (float)1e-9,  /* logYFk */
    { (float)0.064, (float)3.72, 0., 0., 0. },  // Defaults for aqueous auto-phase models
}; /* SPP_SETTING */

BASE_PARAM dfBase[5] =
        {     // Added to facilitate pre-setting in projects (SD,DK), revised 07.10.2011 DK
            { // Variant for aquatic systems with moderate non-ideality
                2,  /* PC */  2,     /* PD */   -4,   /* PRD */
                1,  /* PSM  */ 130,  /* DP */   1,   /* DW */
                0, /* DT */     30000,   /* PLLG */   1,  /* PE */
                1000,   /* IIM */
                1000., /* DG */   1e-13,  /* DHB */  1e-20,  /* DS */
                1e-6,  /* DK */  0.01,  /* DF */  0.01,  /* DFM */
                1e-5,  /* DFYw */  1e-5,  /* DFYaq */    1e-5,  /* DFYid */
                1e-5,  /* DFYr,*/  1e-5,  /* DFYh,*/   1e-5,  /* DFYc,*/
                1e-6, /* DFYs, */  1e-17,  /* DB */   1.,   /* AG */
                -0.98,   /* DGC */   1.0,   /* GAR */  1000., /* GAH */
                1e-3, /* GAS */   12.05,  /* DNS */   1e-13,  /* XwMin, */
                1e-13,  /* ScMin, */  1e-33, /* DcMin, */   1e-20, /* PhMin, */
                1e-5,  /* ICmin */   1e-10,  /* EPS */   1e-3,  /* IEPS */
                1e-10,  /* DKIN  */ 0,  /* tprn */
	    },
            { // Variant for aquatic systems with SCMs in sorption phases
                2,  /* PC */  2,     /* PD */   -4,   /* PRD */
                1,  /* PSM  */ 130,  /* DP */   1,   /* DW */
                0, /* DT */     -30000,   /* PLLG */   1,  /* PE */
                7000,   /* IIM */
                1000., /* DG */   1e-13,  /* DHB */  1e-20,  /* DS */
                3e-6,  /* DK */  0.01,  /* DF */  0.01,  /* DFM */
                1e-5,  /* DFYw */  1e-5,  /* DFYaq */    1e-5,  /* DFYid */
                1e-5,  /* DFYr,*/  1e-5,  /* DFYh,*/   1e-5,  /* DFYc,*/
                1e-6, /* DFYs, */  1e-17,  /* DB */   1.0,   /* AG */
                -0.01,   /* DGC */   1.0,   /* GAR */  1000., /* GAH */
                1e-3, /* GAS */   12.05,  /* DNS */   1e-13,  /* XwMin, */
                1e-13,  /* ScMin, */  1e-33, /* DcMin, */   1e-20, /* PhMin, */
                1e-5,  /* ICmin */   1e-10,  /* EPS */   5e-3,  /* IEPS */
                1e-10,  /* DKIN  */ 0,  /* tprn */
	    },
            { // Variant for fluid-rock systems with highly non-ideal phases
                2,  /* PC */  2,     /* PD */   -4,   /* PRD */
                1,  /* PSM  */ 130,  /* DP */   1,   /* DW */
                1, /* DT */     30000,   /* PLLG */   1,  /* PE */
                7000,   /* IIM */
                1000., /* DG */   1e-11,  /* DHB */  1e-20,  /* DS */
                1e-5,  /* DK */  0.01,  /* DF */  0.001,  /* DFM */
                1e-5,  /* DFYw */  1e-5,  /* DFYaq */    1e-5,  /* DFYid */
                1e-5,  /* DFYr,*/  1e-5,  /* DFYh,*/   1e-5,  /* DFYc,*/
                1e-6, /* DFYs, */  1e-17,  /* DB */   1.,   /* AG */
                -0.001,   /* DGC */   1.0,   /* GAR */  1000., /* GAH */
                1e-3, /* GAS */   12.05,  /* DNS */   1e-11,  /* XwMin, */
                1e-11,  /* ScMin, */  1e-30, /* DcMin, */   1e-20, /* PhMin, */
                1e-5,  /* ICmin */   1e-10,  /* EPS */   1e-3,  /* IEPS */
                1e-10,  /* DKIN  */ 0,  /* tprn */
            },
            { // Variant of strict settings for highest accuracy
                2,  /* PC */  2,     /* PD */   -5,   /* PRD */
                1,  /* PSM  */ 130,  /* DP */   1,   /* DW */
                0, /* DT */     30000,   /* PLLG */   1,  /* PE */
                7000,   /* IIM */
                1000., /* DG */   1e-14,  /* DHB */  1e-20,  /* DS */
                1e-7,  /* DK */  0.01,  /* DF */  0.001,  /* DFM */
                1e-5,  /* DFYw */  1e-5,  /* DFYaq */    1e-5,  /* DFYid */
                1e-5,  /* DFYr,*/  1e-5,  /* DFYh,*/   1e-5,  /* DFYc,*/
                1e-6, /* DFYs, */  1e-17,  /* DB */   1.,   /* AG */
                0.,   /* DGC */   1.0,   /* GAR */  1000., /* GAH */
                1e-3, /* GAS */   12.05,  /* DNS */   1e-13,  /* XwMin, */
                1e-13,  /* ScMin, */  1e-40, /* DcMin, */   1e-30, /* PhMin, */
                1e-7,  /* ICmin */   1e-10,  /* EPS */   1e-3,  /* IEPS */
                1e-10,  /* DKIN  */ 0,  /* tprn */
            },
            { // Variant of strict settings for reactive mass transport (with sorption)
                2,  /* PC */  2,     /* PD */   -4,   /* PRD */
                1,  /* PSM  */ 130,  /* DP */   1,   /* DW */
                0, /* DT */     30000,   /* PLLG */   1,  /* PE */
                7000,   /* IIM */
                1000., /* DG */   1e-14,  /* DHB */  1e-20,  /* DS */
                1e-6,  /* DK */  0.01,  /* DF */  0.01,  /* DFM */
                1e-5,  /* DFYw */  1e-5,  /* DFYaq */    1e-5,  /* DFYid */
                1e-5,  /* DFYr,*/  1e-5,  /* DFYh,*/   1e-5,  /* DFYc,*/
                1e-6, /* DFYs, */  1e-17,  /* DB */   1.,   /* AG */
                -0.001,   /* DGC */   1.0,   /* GAR */  1000., /* GAH */
                1e-3, /* GAS */   12.05,  /* DNS */   1e-13,  /* XwMin, */
                1e-13,  /* ScMin, */  1e-33, /* DcMin, */   1e-20, /* PhMin, */
                1e-5,  /* ICmin */   1e-10,  /* EPS */   1e-3,  /* IEPS */
                1e-10,  /* DKIN  */ 0,  /* tprn */
            }
	    // More preset parameter structures can be added here
            // (if so, increase number of elements from 5 above and in dfBase() call below)
	};

// Setup one of 5 default IPM numerical settings
void TProfil::ChangeSettings(int nSettings)
{
	if( nSettings == 0  )
     return; // No change in controls set
	else if( nSettings == 1 )
		    pa.p = pa_.p;
	     else
            pa.p = dfBase[min(nSettings-2,5)];
    // Added by DK 16.02.2012
    string pa_ver = _GEMS_version_stamp;
    pa_ver += _GEMIPM_version_stamp;
    memcpy(pa.ver, pa_ver.c_str(), TDBVERSION-1);
    pa.ver[TDBVERSION-1]='\0';
}

/// Set the numerical optimization algorithm used for the chemical equilibrium calculations
void TProfil::SetNumericalMethodWrk( const char codAlgorithm)
{
   wrknode->SetNumericalMethod(codAlgorithm);
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
    aFldKeysHelp.push_back("Name of the modeling project");
    aFldKeysHelp.push_back("Comment to the project definition");
    //aFldShot.Add("PRname");
    //aFldShot.Add("PRcom");

    //startKeyEdit = 0;
    start_title = " Numerical and Configuration Settings ";
    pa= pa_;
    pa.p.tprn=0;

    rmults = 0;
    mtparm =  0;
    syst = 0;
    multi =0;
    wrknode = 0;

    //mup = 0;
    //tpp = 0;
    //syp = 0;
    //pmp = 0;

    SFold = 0;
    SMold = 0;
    SAold = 0;
    SBold = 0;
    Llold =0;

    userCancel = false;
    stepWise = false;
    calcFinished = false;
//    fStopCalc = false;
    comp_change_all = false;
    internalBufer = 0;
}

// init submodules to calc module
void TProfil::InitSubModules()
{
        aMod.push_back( rmults = new TRMults( MD_RMULTS ) );
        TRMults::sm = rmults;
        rmults->ods_link();
        aMod.push_back( mtparm = new TMTparm( MD_MTPARM ) );
        TMTparm::sm = mtparm;
        mtparm->ods_link();
        aMod.push_back( syst = new TSyst( MD_SYSTEM ) );
        TSyst::sm = syst;
        syst->ods_link();
        aMod.push_back( multi = new TMultiSystem( MD_MULTI ) );
        TMultiSystem::sm = multi;
        multi->ods_link();
        aMod.push_back( new TEQCalc( MD_EQCALC ) );
        aMod.push_back( new TEQDemo( MD_EQDEMO ) );

        // Creates TNode structure instance accessible trough the "node" pointer
        wrknode = new TNode( multi );
}

// link values to objects
void TProfil::ods_link( int )
{
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
    aObj[ o_paaqpar].SetPtr( pa.aqPar );

    aObj[ o_pabcpc].SetPtr(  pa.BCpc );
    aObj[ o_pagdpgw].SetPtr(  pa.GDpgw );
    aObj[ o_pasdref].SetPtr(  pa.SDrefKey );

    aObj[ o_spppar].SetPtr(  (void *)&pa );
    aObj[ o_spppar].SetM( sizeof( SPP_SETTING ) );
    aObj[ o_sptext].SetPtr(  internalBufer );
}

// set dynamic Objects ptr to values
void TProfil::dyn_set(int )
{
    pa.p.tprn= (char *)aObj[o_patprn].GetPtr();
    internalBufer = (char *)aObj[ o_sptext].GetPtr();

    if( rmults ) rmults->dyn_set();
    if( mtparm ) mtparm->dyn_set();
    if( syst ) syst->dyn_set();
    if( multi ) multi->dyn_set();
}

// free dynamic memory in objects and values
void TProfil::dyn_kill(int )
{
    pa.p.tprn = (char *)aObj[o_patprn].Free();
internalBufer = (char *)aObj[ o_sptext].Free();
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
    internalBufer = 0;
    if( rmults ) rmults->set_def();
    if( mtparm ) mtparm->set_def();
    if( syst ) syst->set_def();
    if( multi ) multi->set_def();
}

// opens window with 'Remake record' parameters
void TProfil::MakeQuery()
{
    const char * p_key;
    char flgs[38];
    int tasktype = 0;

    p_key  = db->PackKey();
    // flgs 0-9 MSpmv, 10-29 TPptv, 30-33  TPun, 34-37 TPsv
    memcpy( flgs, &rmults->GetMU()->PmvSA, 10);
    memcpy( flgs+10, &mtparm->GetTP()->PtvG, 20);
    memcpy( flgs+30, &mtparm->GetTP()->PunE, 8);

    if( !vfProjectSet( pVisor->window()/*window()*/, p_key, flgs, tasktype ))
         Error( p_key, "Project record configuration cancelled by the user!" );

    memcpy( &rmults->GetMU()->PmvSA, flgs,    10);
    memcpy( &mtparm->GetTP()->PtvG,  flgs+10, 20);
    memcpy( &mtparm->GetTP()->PunE,  flgs+30, 8);
    // Set BASE_PARAM from default
    ChangeSettings(tasktype);
}

// Help on Modelling Project module ( ? button )
const char* TProfil::GetHtml()
{
   if(pVisor->ProfileMode == true )
     return GEMS_NUMSET_HTML;
   else
    return GM_PROJECT_HTML;
}


void TProfil::makeGEM2MTFiles(QWidget* par )
{
    TNode* na = 0;
    MULTIBASE *pmp = multi->pmp;

    try
	 {
      // set default data and realloc arrays
      char flags[6];
      double Tai[4], Pai[4];
      long int nTp_, nPp_;
      double *arT, *arP;

      Tai[0] = pmp->TCc; Tai[1] = pmp->TCc;
      Pai[0] = pmp->Pc; Pai[1] = pmp->Pc;

      if( !vfLookupDialogSet(par, flags, Tai, Pai ) )// ask constants
        	return;

      //internal objects for lookup arrays data
      arT = (double *) aObj[ o_w_tval].GetPtr();
      nTp_ = aObj[ o_w_tval].GetN();
      arP = (double *) aObj[ o_w_pval].GetPtr();
      nPp_ = aObj[ o_w_pval].GetN();


      na = new TNode( multi );

      // realloc and setup data for dataCH and DataBr structures
      //na->MakeNodeStructures( par, ( flags[0] == S_OFF ) , Tai, Pai  );
      na->MakeNodeStructures( par, ( flags[0] == S_OFF ),( flags[4] == S_ON ),
          arT, arP, nTp_, nPp_, Tai[3], Pai[3]  );

   na->SetNumericalMethod( pVisor->GetNumericalMethod() );
    // setup dataBR and NodeT0 data
   na->packDataBr();
   //na->MoveWorkNodeToArray( 0, 1, na->pNodT0() );
   // make  all files
   if(flags[5] == S_OFF )
      na->pCNode()->NodeStatusFMT = No_nodearray;


   na->PutGEM2MTFiles( par, ( flags[1] == S_ON ? ft_json : ft_binary ), ( flags[2] == S_ON ),
           ( flags[3] == S_ON ),  true );// addMui, to txt
//Test to compare standalone  na->GEM_print_ipm( "GemsCalcPhase.txt" );
   }
    catch( TError& xcpt )
    {
      if( na )
       delete na;
      na = 0;
       throw; //Error(  xcpt.title, xcpt.mess );
    }
    if( na )
     delete na;
    na = 0;

}

// Checks if the modelling project name is the same as read from GEMS3K I/O files
bool TProfil::CompareProjectName( const char* SysKey )
{
    int len = rt[RT_PARAM].FldLen(0);
    const char* proj_key = db->UnpackKey();
    if( memcmp( SysKey, proj_key, len ) )
        return true;
    else
      return false;
}

// Change the key read from GEMS3K I/O files
// Copy T and P from DATABR
void TProfil::ChangeTPinKey( double T, double P )
{
    char bT[40];
    char bP[40];

    Gcvt( T, 6, bT );
    Gcvt( P, 6, bP );
    rt[RT_SYSEQ].SetKey( multi->pmp->stkey );
    string str = multi->pmp->stkey;
    rt[RT_SYSEQ].MakeKey( RT_SYSEQ, str, RT_SYSEQ, 0, RT_SYSEQ, 1,
                           RT_SYSEQ, 2, RT_SYSEQ, 3, RT_SYSEQ, 4,
                           K_IMM, bP, K_IMM, bT, RT_SYSEQ, 7, K_END);
    strncpy(multi->pmp->stkey, str.c_str(), EQ_RKLEN );

    string capName = "Change the key read from GEMS3K I/O files";
AGAIN:
    str = TSysEq::pm->GetKeyofRecord( str.c_str(),capName.c_str(), KEY_NEW );
    if( str.empty() )
         Error( GetName(), "Record creation rejected!");
    if( rt[RT_SYSEQ].Find( str.c_str() )  )
    {
        capName = "This record already exists! Please, enter another name.";
        goto AGAIN;
    }
    rt[RT_SYSEQ].SetKey( str.c_str() );
}


//  This function sets the system/SysEq switches for components and phases
//       according to mui, muj, mup index lists in MULTI that were read in.
void TProfil::SetSysSwitchesFromMulti( )
{
     MULTIBASE* pmp = multi->pmp;
     RMULTS* mup = rmults->GetMU();
     SYSTEM* syp = syst->GetSY();
     int i, ii, j, jj, k, kk;


     // Set default informations to arrays
     syst->setDefData();

     // ICs
     for( ii=0; ii < mup->N; ii++ )
        syp->Icl[ii] = S_OFF;
     for( i=0; i < pmp->N; i++ )
     {
        ii = pmp->mui[i];
        syp->Icl[ii] = S_ON;
     }
     syp->N = pmp->N;
     // DCs
     for( jj=0; jj < mup->L; jj++ )
     {
         syp->Dcl[jj] = S_OFF;
     //    syp->DLL[jj] = 0.;
     //    syp->DUL[jj] = 1e6;
     }
     for( j=0; j < pmp->L; j++ )
     {
        jj = pmp->muj[j];
        syp->Dcl[jj] = S_ON;
     //   syp->DLL[jj] = pmp->DLL[j];
     //   syp->DUL[jj] = pmp->DUL[j];
     }
     syp->L = pmp->L;
     syp->Ls = pmp->Ls;
     // Phases
     for( kk=0; kk < mup->Fi; kk++ )
        syp->Pcl[kk] = S_OFF;
     for( k=0; k < pmp->FI; k++ )
     {
        kk = pmp->muk[k];
        syp->Pcl[kk] = S_ON;
        syp->Ll[kk] = pmp->L1[k];
     }
     syp->Fi = pmp->FI;
     syp->Fis = pmp->FIs;
}

// Reading structure MULTI (GEM IPM work structure)
void TProfil::CmReadMulti( QWidget* par, const char* path )
{
    TNodeArray* na = new TNodeArray( 1, multi );
    MULTIBASE* pmp = multi->pmp;
    SYSTEM* syp = syst->GetSY();
    //string key = pmp->stkey;

    if( na->GEM_init( path ) )
    {
      Error( path, "GEMS3K Init() error: \n"
             "Some GEMS3K input files are corrupt or cannot be found.");
    }
    multi->dyn_set();

    // Here to compare the modelling project name; error when from a different project.
    if( CompareProjectName( pmp->stkey ) )
    {
        delete na;
        Error( pmp->stkey, "E15IPM: Wrong project name by reading GEMS3K I/O files ");
    }

     // Unpacking the actual contents of DBR file including speciation
    na->unpackDataBr( true );
    for( int j=0; j < pmp->L; j++ )
        pmp->X[j] = pmp->Y[j];
    pmp->TC = pmp->TCc;
    pmp->T =  pmp->Tc;
    pmp->P =  pmp->Pc;
    pmp->VX_ = pmp->VXc; // from cm3 to m3

    // Set T and P  for key from DataBr
    ChangeTPinKey( pmp->TC, pmp->P );

    pmp->pESU = 2;  // SysEq unpack flag set

    // Test sizes and flags
    // my be diffirent compared to internal system (do not test):
    // syp->PAalp, syp->PSigm, pmp->FIat, pmp->E
    int iRet = BAL_compare();
    switch(iRet)
    {
      case 0: // Some lists have changed in the loaded GEM task relative to base system
           pmp->pIPN = 0;
           if( pmp->pTPD > 1)
               pmp->pTPD = 1; // reload Go, Vol
           pmp->pKMM = 0;
           // sets the system/SysEq switches for
//             components and phases according to mui, muj, mup that were read in.
           SetSysSwitchesFromMulti( );
//           vfMessage(par, "Changes in the GEMS3K system setup relative to GEMS",
//                    "Some vectors were allocated or some dimensions changed.\n"
//                    "The equilibrium cannot be recalculated");
          break;
       case 1: // Bulk composition has changed, but lists are the same
          //vfMessage(par, "Changes in the GEMS3K system setup relative to GEMS",
          //          "Some constraints are not the same.\n"
          //          "Different results may be obtained upon recalculation");
       default:
          break;
    }

    // for loading GEX to System
    CheckMtparam();
    multi->DC_LoadThermodynamicData( na );

    // Unpack the pmp->B vector (b) into syp->BI and syp->BI (BI_ vector).
    for( int i=0; i < pmp->N; i++ )
        syp->BI[pmp->mui[i]] = syp->B[pmp->mui[i]] = pmp->B[i];

    for( int jj, j=0; j < pmp->L; j++ )
    {
       jj = pmp->muj[j];
       syp->DLL[jj] = pmp->DLL[j];
       syp->DUL[jj] = pmp->DUL[j];
       syp->GEX[jj] = na->DC_G0(j, pmp->Pc*bar_to_Pa, pmp->Tc, false) - mtparm->GetTP()->G[jj];
    }

// !!! We cannot restore to System  for adsorbtion must be done

    TSysEq::pm->CellChanged();

    // calculate mass of the system
     pmp->MBX = 0.0;
    for(int i=0; i<pmp->N; i++ )
     pmp->MBX += pmp->B[i] * pmp->Awt[i];
     pmp->MBX /= 1000.;

    // Restoring the rest of MULTI contents from primal and dual solution
    pmp->pIPN =0;
    multi->Alloc_internal();
    multi->EqstatExpand( pmp->stkey, false, false );
//    outMultiTxt( "IPM_EqstatExpand.txt"  );
    //    multi->Free_internal();
    //    na->unpackDataBr( true );
    delete na;
    // We can get different results in GEMS than in GEMS3K
    // because of slightly different values into G, V ...
    // (interpolation of thermodynamic data or precision )
}

// Load Thermodynamic data from Database
void TProfil::CheckMtparam()
{

  if( fabs( mtparm->GetTP()->T - multi->pmp->TCc ) > 1.e-10 ||
         fabs( mtparm->GetTP()->P - multi->pmp->Pc ) > 1.e-9 )
   { // load new MTPARM on T or P
      mtparm->LoadMtparm( multi->pmp->TCc, multi->pmp->Pc );
      multi->pmp->pTPD = 0;
   }
 }

// Setup of flags for MULTY remake
// pNP,  //Mode of FIA selection: 0-LPP AIA, 1-old eqstate, -1-user's choice
// pESU, // Unpack old eqstate from EQSTAT record?  0-no 1-yes
// pIPN, // State of IPN-arrays:  0-create; 1-available; -1 remake
// pBAL, // State of reloading CSD:  1- BAL only; 0-whole CSD
// pTPD, // State of reloading thermod data: 0- all    2 - no
void TProfil::PMtest( const char *key )
{
    //double V, T, P;
    TSysEq* STat = (TSysEq*)(aMod[RT_SYSEQ]);
    TProcess* Proc = (TProcess*)(aMod[RT_PROCES]);
    MULTIBASE *pmp = multi->pmp;

    // test for available old solution
    if( STat->ifCalcFlag() )
    { if( !pmp->pESU )      // if pESU == 2 (task loaded before), left value
          pmp->pESU = 1;
    }
    else pmp->pESU = 0;

   // no old solution => must be LPP AIA
   if( pmp->pESU == 0 )
        pmp->pNP = 0;

   // test changes in the modified system relative to MULTI
   pmp->pBAL =  BAL_compare();
//   cout << "pmp->pBAL " << pmp->pBAL << "  pmp->pTPD " << pmp->pTPD<< endl;

   if( !pmp->pBAL ) // if some vectors were allocated or some dimensions changed
   {
       pmp->pIPN = 0;
       if( pmp->pTPD > 1)
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
    SYSTEM *syp = TSyst::sm->GetSY();
    MULTIBASE *pmp = multi->pmp;

    // Changes in thermdynamic (Saved DComp, ReacDC or Phase records )
    if( pmp->pTPD == -1 )  // 16/11/2011 SD
      return 0;

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


  // test other settings for G0
  for( k = 0; k < pmp->FI; k++ )
  {
	jb = je;
	je += pmp->L1[k];
    for( j=jb; j<je; j++ )
    {
       Gg = Ge = 0.0;    //   This part had to be changed after integrating Ge into pmp->G0
       jj = pmp->muj[j]; //        DK    07.03.2008,  16.05.2008

       Go = mtparm->GetTP()->G[jj]; //  G0(T,P) value taken from MTPARM
       if( syp->Guns )  // This is used mainly in UnSpace calculations
           Gg = syp->Guns[jj];    // User-set increment to G0 from project system
       if( syp->GEX && syp->PGEX != S_OFF )   // User-set increment to G0 from project system
           Ge = syp->GEX[jj];     //now Ge is integrated into pmp->G0 (since 07.03.2008) DK
       pGo = multi->ConvertGj_toUniformStandardState( Go+Gg+Ge, j, k );

       if( fabs( pGo - pmp->G0[j] )* pmp->RT >= 0.001 )
       {
           break;   // GEX or Guns has changed for this DC in the system definition
       }
    }  // j
    if( j < je )
       return 0;
  }  // k


  // lists of components didn't change
  // test B - recipes and constraints
      for( i=0; i<pmp->N; i++ )
          if( pmp->B[i] != 0. )
          {
//              if( fabs( syp->B[pmp->mui[i]] /pmp->B[i] - 1. ) > pa.p.DHB * 0.001   )
              if( fabs( log10( syp->B[pmp->mui[i]] /pmp->B[i] )) > 1e-13  ) // log10( pa.p.DB ) ) )
                  return 1;
          }
          else
          { if( syp->B[pmp->mui[i]]  !=  0.   )
              return 1;
          }

    je = 0;
      // test other settings for DCs
    for( k = 0; k < pmp->FI; k++ )
    {
          jb = je;
          je += pmp->L1[k];
      for( j=jb; j<je; j++ )
      {
         jj = pmp->muj[j]; //        DK    07.03.2008,  16.05.2008

          if(( syp->DLLim != S_OFF ) && pmp->PLIM == 1 )
  //            if( fabs( syp->DLL[jj] - pmp->DLL[j] ) >= 1e-19 )
                if( syp->DLL[jj] != pmp->DLL[j]  )   //SD 22/01/2009
                  break;
          if(( syp->DULim != S_OFF ) && pmp->PLIM == 1 )
  //            if( fabs( syp->DUL[jj] - pmp->DUL[j] ) >= 1e-19 )
                if( syp->DUL[jj] != pmp->DUL[j] )   //SD 22/01/2009
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
    RMULTS* mup = rmults->GetMU();
    SYSTEM *syp = syst->GetSY();

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
    if( syp->PbAC != S_OFF && rmults->GetMU()->La > 0 )
        for( i=0; i < mup->La; i++)
               syp->XeA[i] = 0.;
    if( syp->PbPH != S_OFF )
        for( i=0; i < mup->Fi; i++)
               syp->Phm[i] = 0.;
}

void TProfil::LoadFromMtparm( QWidget* par, DATACH *CSD , bool no_interpolat)
{
    if( no_interpolat )
       mtparm->LoadDataToPair( par, CSD );
    else
       mtparm->LoadDataToLookup( par, CSD );
}

//=========================================================================================
// the same functions in ms_param.cpp

//#include "equlibrate.h"
#include "node.h"

/// GEM IPM calculation of equilibrium state in MULTI
/// without testing changes in the system
//
double TProfil::ComputeEquilibriumState( long int& NumPrecLoops, long int& NumIterFIA, long int& NumIterIPM )
{
  TSysEq* STat = (TSysEq*)(aMod[RT_SYSEQ]);
  calcFinished = false;


  double CalcTime = wrknode->ComputeEquilibriumState( NumIterFIA, NumIterIPM );
  //profil->ComputeEquilibriumState( PrecLoops, NumIterFIA, NumIterIPM );

  ////wrknode->GEM_run( false );
  //multi->CalculateEquilibriumState( 0, NumIterFIA, NumIterIPM );

  calcFinished = true;
  STat->setCalcFlag( true );
  // STat->CellChanged(); // SD 28/11/2011 to protect MessageToSave()

  return CalcTime;
}

/*
void TProfil::outMulti( GemDataStream& ff, string& path  )
{
    ff.writeArray( &pa.p.PC, 10 );
    ff.writeArray( &pa.p.DG, 28 );
    multi->to_file( ff/*, path* );
}*/


void TProfil::outMultiTxt( const char *path, bool append  )
{
    multi->to_text_file( path, append );
}

/*
// Reading structure MULTI (GEM IPM work structure)
void TProfil::readMulti( GemDataStream& ff )
{

      ff.readArray( &pa.p.PC, 10 );
      ff.readArray( &pa.p.DG, 28 );
      multi->from_file( ff );
}

// Reading structure MULTI (GEM IPM work structure)
void TProfil::readMulti( const char* path,  DATACH  *dCH )
{
      multi->from_text_file_gemipm( path, dCH);
}
*/

double TProfil::MolWeight( int N, double *ICaw, double *Smline )
 { 	return syst->MolWeight( N, ICaw, Smline ); }

void TProfil::SyTestSizes()
 { 	syst->SyTestSizes(); }

void TProfil::ET_translate( int nOet, int nOpex, int JB, int JE, int jb, int je,
 tget_ndx *get_ndx  )
{
  multi->ET_translate( nOet, nOpex, JB, JE, jb, je, get_ndx);
}

// Build list names of index for object
void TProfil::getNamesList( int nO, vector<string>& lst )
{
   lst.clear();

   int i, iCode = aObj[nO].GetIndexationCode();
   RMULTS* mup = TRMults::sm->GetMU();
   MULTIBASE *pmp = multi->pmp;

   long int nOsz = aObj[nO].GetN();

    // Getting new indexation code
   switch( iCode )
   {  // analyze indexation code
       default: //break;
          Error( "E01MSWin: Unknown indexation base for data object",aObj[nO].GetKeywd()  );
       case A_icx:
                  for( i=0; i<pmp->N; i++ )
                     lst.push_back( string( pmp->SB[i], 0, MAXICNAME ));
                  break;
       case A_dcx:
       case A_dcxicx:
                   for( i=0; i<min(pmp->L,nOsz); i++ )
                       lst.push_back( string( pmp->SM[i],0, MAXDCNAME) );
                   break;
       case A_dcsx:
                   for( i=pmp->Ls + pmp->Lads; i<pmp->L; i++ )
                       lst.push_back( string( pmp->SM[i], 0, MAXDCNAME) );
                   break;
       case A_phx:
                   for( i=0; i<min(pmp->FI,nOsz); i++ )
                     lst.push_back( string( pmp->SF[i]+MAXSYMB, 0, MAXPHNAME));
                   break;
       case A_phxicx:
                   for( i=0; i<pmp->N; i++ )
                      lst.push_back( string( pmp->SB[i], 0, MAXICNAME ));
                   break;
                   //for( i=0; i<pmp->FIs; i++ )
                   //  lst.Add( string( pmp->SF[i]+MAXSYMB, 0, MAXPHNAME));
                   //break;
       case A_ICx:
                  for( i=0; i<mup->N; i++ )
                       lst.push_back( string( mup->SB[i], 0, MAXICNAME ));
                   break;
       case A_ACx:
                  for( i=0; i<mup->La; i++ )
                      lst.push_back( string( mup->SA[i], 0, MAXCMPNAME ));
                   break;
       case A_DCx:
                   for( i=0; i<mup->L; i++ )
                      lst.push_back( string( mup->SM[i]+MAXSYMB+MAXDRGROUP ,
                                             0, MAXDCNAME ));
                   break;
       case A_DCSx:
                  for( i=mup->Ls+mup->Lads; i<mup->L; i++ )
                    lst.push_back( string( mup->SM[i]+MAXSYMB+MAXDRGROUP ,
                                            0, MAXDCNAME ));
                  break;
       case A_PHx:
                   for( i=0; i<mup->Fi; i++ )
                       lst.push_back( string( mup->SF[i]+MAXSYMB+MAXPHSYMB,
                                              0, MAXPHNAME));
                   break;
   }
}


// save old lists of keys to compare
void TProfil::SaveOldList()
{
    RMULTS* mup = rmults->GetMU();
    Nold = mup->N;
    Lold = mup->L;
    Fiold = mup->Fi;
    Fisold = mup->Fis;
    Laold = mup->La;
    Lsold = mup->Ls;
    SBold = (char (*)[IC_RKLEN]) new char[mup->N*IC_RKLEN];
    memcpy( SBold, mup->SB, mup->N*IC_RKLEN*sizeof(char));
    SAold = (char (*)[BC_RKLEN]) new char[mup->La*BC_RKLEN];
    memcpy( SAold, mup->SA, mup->La*BC_RKLEN*sizeof(char));
    Llold =   new short[mup->Fi];
    memcpy( Llold, mup->Ll, mup->Fi*sizeof(short));
    SFold = (char (*)[PH_RKLEN]) new char[mup->Fi*PH_RKLEN];
    memcpy( SFold, mup->SF, mup->Fi*sizeof(char)*PH_RKLEN );
    SMold = (char (*)[DC_RKLEN]) new char[mup->L*DC_RKLEN];
    memcpy( SMold, mup->SM, mup->L*sizeof(char)*DC_RKLEN );
}

// delete old lists of keys to compare
void TProfil::DeleteOldList()
{
    if( SBold )  delete[] SBold;
    SBold= 0;
    if( SAold )  delete[] SAold;
    SAold = 0;
    if(  Llold ) delete[]  Llold;
    Llold = 0;
    if( SFold ) delete[] SFold;
    SFold = 0;
    if( SMold ) delete[] SMold;
    SMold = 0;
}

// push element to the list - refurbished by DK on 15.02.2012
//
void TProfil::Push( vector<CompItem>& aList, int aLine,
                    short aDelta, const char* dbKeywd, string aKey )
{
   if( comp_change_all == false )
   {
       string stt = aKey;
       if( aDelta < 0 )
          stt += " record to be deleted from the project database. Action?";
       else stt += " record to be inserted into project database. Action?";

       switch( vfQuestion3(window(), dbKeywd, stt.c_str(),
              "&Do it", "Do it for &All", "&Cancel" ))
       {
       case VF3_3:   // Skip: now skipping, as the user wants
                    Error( dbKeywd, "Comparison error!" );
                    break;
       case VF3_2:  // Do it for all
                    comp_change_all = true;
       case VF3_1:  // Do it for this item
                    aList.push_back( CompItem( aLine, aDelta));
       }
   }
   else

       aList.push_back( CompItem( aLine, aDelta));
}


// Compare IComp keys lists
void TProfil::ICcompare( vector<CompItem>& aIComp)
{
    int i, j, l;
    RMULTS* mup = rmults->GetMU();

    i = 0;
    j = 0;
    while( i<Nold && j<mup->N )
    {
        l = memcmp( SBold[i], mup->SB[j], IC_RKLEN-MAXICGROUP ); // 14/11/12 SD
        if( l==0 )
        {
            i++;
            j++;
        }
        else
            if( l<0 )
            {
                Push( aIComp, i, -1, "IComp", string(SBold[i], 0, IC_RKLEN) );
                i++;
            }
            else
            {
                Push( aIComp, i, 1, "IComp", string(mup->SB[j], 0, IC_RKLEN) );
                j++;
            }
    }
    while( i<Nold  )
    {
        Push( aIComp, i, -1, "IComp", string(SBold[i], 0, IC_RKLEN) );
        i++;
    }
    while( j<mup->N )
    {
        Push( aIComp, i, 1, "IComp", string(mup->SB[j], 0, IC_RKLEN) );
        j++;
    }
}

// Compare Compos keys lists
void TProfil::COMPcompare( vector<CompItem>& aCompos)
{
    int i, j, l;
    RMULTS* mup = rmults->GetMU();

    i = 0;
    j = 0;
    while( i<Laold && j<mup->La )
    {
        l = memcmp( SAold[i], mup->SA[j], BC_RKLEN-MAXCMPGROUP ); // 14/11/12 SD
        if( l==0 )
        {
            i++;
            j++;
        }
        else
            if( l<0 )
            {
                Push( aCompos, i, -1, "Compos", string(SAold[i], 0, BC_RKLEN) );
                i++;
            }
            else
            {
                Push( aCompos, i, 1, "Compos", string(mup->SA[j], 0, BC_RKLEN) );
                j++;
            }
    }
    while( i<Laold  )
    {
        Push( aCompos, i, -1, "Compos", string(SAold[i], 0, BC_RKLEN) );
        i++;
    }
    while( j<mup->La )
    {
        Push( aCompos, i, 1, "Compos", string(mup->SA[j], 0, BC_RKLEN) );
        j++;
    }
}

// compare DCOMP&REACT keys lists to one phase
void TProfil::DCcompare( vector<CompItem>& aList,
                         int& i,int& j, int nI, int nJ)
{
    int l;
    RMULTS* mup = rmults->GetMU();

    while( i<nI && j<nJ )
    {
        l = memcmp( SMold[i], mup->SM[j], DC_RKLEN-MAXSYMB );
        if( l==0 )
        {
            i++;
            j++;
        }
        else
            if( l<0 )
            {
                Push( aList, i, -1, "DComp/ReacDC", string(SMold[i], 0, DC_RKLEN) );
                i++;
            }
            else
            {
                Push( aList, i, 1, "DComp/ReacDC", string(mup->SM[j], 0, DC_RKLEN) );
                j++;
            }
    }
    while( i<nI  )
    {
        Push( aList, i, -1, "DComp/ReacDC", string(SMold[i], 0, DC_RKLEN) );
        i++;
    }
    while( j<nJ )
    {
        Push( aList, i, 1, "DComp/ReacDC", string(mup->SM[j], 0, DC_RKLEN) );
        j++;
    }
}

// Compare Phase and DComp&React keys lists
void TProfil::PHcompare( vector<CompItem>& aPhase, vector<CompItem>& aDComp)
{
    int i, j, l;
    int id =0, jd=0;
    RMULTS* mup = rmults->GetMU();

    i = 0;
    j = 0;
    while( i<Fisold && j<mup->Fis )
    {
        l = memcmp( SFold[i], mup->SF[j], PH_RKLEN-MAXPHGROUP ); // fix KD 24.06.03
        if( l==0 )
        {
            DCcompare( aDComp, id, jd, id+Llold[i], jd+mup->Ll[j]);
            i++;
            j++;
        }
        else
            if( l<0 )
            {
                Push( aPhase, i, -1, "Phase", string(SFold[i], 0, PH_RKLEN) );
                for( int ii=id; ii<id+Llold[i]; ii++)
                    Push( aDComp, ii, -1, "DComp/ReacDC", string(SMold[ii], 0, DC_RKLEN) );
                id += Llold[i];
                i++;
            }
            else
            {
                Push( aPhase, i, 1, "Phase", string(mup->SF[j], 0, PH_RKLEN) );
                for( int jj=jd; jj<jd+mup->Ll[j]; jj++)
                    Push( aDComp, id, 1, "DComp/ReacDC", string(mup->SM[jj], 0, DC_RKLEN) );
                jd += mup->Ll[j];
                j++;
            }
    }
    while( i<Fisold  )
    {
        Push( aPhase, i, -1, "Phase", string(SFold[i], 0, PH_RKLEN) );
        for( int ii=id; ii<id+Llold[i]; ii++)
            Push( aDComp, ii, -1, "DComp/ReacDC", string(SMold[ii], 0, DC_RKLEN) );
        id += Llold[i];
        i++;
    }
    while( j<mup->Fis )
    {
        Push( aPhase, i, 1, "Phase", string(mup->SF[j], 0, PH_RKLEN) );
        for( int jj=jd; jj<jd+mup->Ll[j]; jj++)
            Push( aDComp, id, 1, "DComp/ReacDC", string(mup->SM[jj], 0, DC_RKLEN) );
        jd += mup->Ll[j];
        j++;
    }

    // compare one-component phase
    i = Fisold;
    j = mup->Fis;
    id =Lsold, jd=mup->Ls;
    while( i<Fiold && j<mup->Fi )
    {
        l = memcmp( SFold[i], mup->SF[j], PH_RKLEN );
        if( l==0 )
        {
            DCcompare( aDComp, id, jd, id+Llold[i], jd+mup->Ll[j] );
            i++;
            j++;
        }
        else
            if( l<0 )
            {
                Push( aPhase, i, -1, "Phase", string(SFold[i], 0, PH_RKLEN) );
                for( int ii=id; ii<id+Llold[i]; ii++)
                    Push( aDComp, ii, -1, "DComp/ReacDC", string(SMold[ii], 0, DC_RKLEN) );
                id += Llold[i];
                i++;
            }
            else
            {
                Push( aPhase, i, 1, "Phase", string(mup->SF[j], 0, PH_RKLEN) );
                for( int jj=jd; jj<jd+mup->Ll[j]; jj++)
                    Push( aDComp, id, 1, "DComp/ReacDC", string(mup->SM[jj], 0, DC_RKLEN) );
                jd += mup->Ll[j];
                j++;
            }
    }
    while( i<Fiold  )
    {
        Push( aPhase, i, -1, "Phase", string(SFold[i], 0, PH_RKLEN) );
        for( int ii=id; ii<id+Llold[i]; ii++)
            Push( aDComp, ii, -1, "DComp/ReacDC", string(SMold[ii], 0, DC_RKLEN) );
        id += Llold[i];
        i++;
    }
    while( j<mup->Fi )
    {
        Push( aPhase, i, 1, "Phase", string(mup->SF[j], 0, PH_RKLEN) );
        for( int jj=jd; jj<jd+mup->Ll[j]; jj++)
            Push( aDComp, id, 1, "DComp/ReacDC", string(mup->SM[jj], 0, DC_RKLEN) );
        jd += mup->Ll[j];
        j++;
    }
}


int TProfil::indPH( int i )
{
    if( isSysEq == false )
        return i;

    for( int ii=0; ii<PHon.size(); ii++)
        if( i == PHon[ii])
            return ii;

    return -1;
}

int TProfil::indDC( int i )
{
    if( isSysEq == false )
        return i;
    for( int ii=0; ii<DCon.size(); ii++)
        if( i == DCon[ii])
            return ii;
    return -1;
}


// ------------------ End of m_param.cpp -----------------------




