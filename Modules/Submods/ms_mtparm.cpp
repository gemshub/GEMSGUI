//-------------------------------------------------------------------
// $Id: ms_mtparm.cpp 1377 2009-07-24 13:12:02Z gems $
//
// Implementation of TMTparm class, config and  load functions
//
// Rewritten from C to C++ by S.Dmytriyeva
// Copyright (C) 1995-2012 S.Dmytriyeva, D.Kulik
//
// This file is part of a GEM-Selektor library for thermodynamic
// modelling by Gibbs energy minimization
// Uses: GEM-Selektor GUI GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the GPL v.3 license

//
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//

#include "ms_mtparm.h"
#include "m_param.h"
#include "m_reacdc.h"
#include "m_dcomp.h"
#include "visor.h"
// added 09.06.2016 for calculating Psat when exporting GEMS3K files and P=0 is not specified; DM
#include "s_supcrt.h"

TMTparm* TMTparm::sm;

TMTparm::TMTparm( int nrt ):
        TSubModule( nrt )
{
    set_def();
}

const char* TMTparm::GetHtml()
{
   return GSM_MTPARM_HTML;

}

// link values to objects
void TMTparm::ods_link( int /*q*/)
{
    //aObj[ o_tpsymb]->SetPtr( tp.symb );
    //aObj[ o_tpp]->SetPtr( tp.P_ );
    //aObj[ o_tptc]->SetPtr( tp.TC_ );
    //aObj[ o_tpnv]->SetPtr( tp.NV_ );
    aObj[ o_tpunits]->SetPtr(&tp.PunE );   /*a 4*/
    aObj[ o_tpsv]->SetPtr(   &tp.PeosW );  /*a 4*/
    aObj[ o_tpptv]->SetPtr(  &tp.PtvG );   /*a 20*/
    aObj[ o_tp_T]->SetPtr(   &tp.T );
    aObj[ o_tp_P]->SetPtr(   &tp.P );
    aObj[ o_tp_RT]->SetPtr(  &tp.RT );    /*f 2*/
    aObj[ o_tprow]->SetPtr(  &tp.RoW );   /*f 2*/
    aObj[ o_tpepsw]->SetPtr( &tp.EpsW );  /*f 2*/
    aObj[ o_tpvisw]->SetPtr( &tp.VisW );  /*f 2*/
    aObj[ o_tpstr]->SetPtr(  &tp.PunE );
    aObj[ o_tp_L]->SetPtr(   &tp.L );    /*i 4*/

    aObj[ o_tpmark]->SetPtr( tp.mark );
    aObj[ o_tpmark]->SetDim( tp.L, 1 );
    aObj[ o_tpg]->SetPtr( tp.G );
    aObj[ o_tpg]->SetDim( tp.L, 1 );
    aObj[ o_tpdevg]->SetPtr( tp.devG );
    aObj[ o_tpdevg]->SetDim( tp.L, 1 );
    aObj[ o_tph]->SetPtr( tp.H );
    aObj[ o_tph]->SetDim( tp.L, 1 );
    aObj[ o_tpdevh]->SetPtr( tp.devH );
    aObj[ o_tpdevh]->SetDim( tp.L, 1 );
    aObj[ o_tps]->SetPtr( tp.S );
    aObj[ o_tps]->SetDim( tp.L, 1 );
    aObj[ o_tpdevs]->SetPtr( tp.devS );
    aObj[ o_tpdevs]->SetDim( tp.L, 1 );
    aObj[ o_tpcp]->SetPtr( tp.Cp );
    aObj[ o_tpcp]->SetDim( tp.L, 1 );
    aObj[ o_tpdevcp]->SetPtr( tp.devC );
    aObj[ o_tpdevcp]->SetDim( tp.L, 1 );
    aObj[ o_tpvm]->SetPtr( tp.Vm );
    aObj[ o_tpvm]->SetDim( tp.L, 1 );
    aObj[ o_tpdevv]->SetPtr( tp.devV );
    aObj[ o_tpdevv]->SetDim( tp.L, 1 );
    aObj[ o_tpf]->SetPtr( tp.F );
    aObj[ o_tpf]->SetDim( tp.L, 1 );
    aObj[ o_tpu]->SetPtr( tp.U );
    aObj[ o_tpu]->SetDim( tp.L, 1 );
    aObj[ o_tpcv]->SetPtr( tp.Cv );
    aObj[ o_tpcv]->SetDim( tp.L, 1 );
    aObj[ o_tpalp]->SetPtr( tp.Alp );
    aObj[ o_tpalp]->SetDim( tp.L, 1 );
    aObj[ o_tpbet]->SetPtr( tp.Bet );
    aObj[ o_tpbet]->SetDim( tp.L, 1 );

    aObj[ o_tpwbor]->SetPtr( tp.Wbor );
    aObj[ o_tpwbor]->SetDim( tp.Ls, 1 );
    aObj[ o_tpwrad]->SetPtr( tp.Wrad );
    aObj[ o_tpwrad]->SetDim( tp.Ls, 1 );

    aObj[ o_tpfug]->SetPtr( tp.Fug );
    aObj[ o_tpfug]->SetDim( tp.Lg, 1 );
    aObj[ o_tpdvg]->SetPtr( tp.dVg );
    aObj[ o_tpdvg]->SetDim( tp.Lg, 4 );
    aObj[ o_tpdcmod]->SetPtr( tp.DCMod );
    aObj[ o_tpdcmod]->SetDim( tp.L, 1 );
}

// set dynamic Objects ptr to values
void TMTparm::dyn_set(int /*q*/)
{
    tp.mark = (char *)aObj[ o_tpmark ]->GetPtr();
    tp.G =    (double *)aObj[ o_tpg ]->GetPtr();
    tp.devG = (double *)aObj[ o_tpdevg ]->GetPtr();
    tp.H =    (double *)aObj[ o_tph ]->GetPtr();
    tp.devH = (double *)aObj[ o_tpdevh ]->GetPtr();
    tp.S =    (double *)aObj[ o_tps ]->GetPtr();
    tp.devS = (double *)aObj[ o_tpdevs ]->GetPtr();
    tp.Cp =   (double *)aObj[ o_tpcp ]->GetPtr();
    tp.devC = (double *)aObj[ o_tpdevcp ]->GetPtr();
    tp.Vm =   (double *)aObj[ o_tpvm ]->GetPtr();
    tp.devV = (double *)aObj[ o_tpdevv ]->GetPtr();
    tp.F =    (double *)aObj[ o_tpf ]->GetPtr();
    tp.U =    (double *)aObj[ o_tpu ]->GetPtr();
    tp.Cv =   (double *)aObj[ o_tpcv ]->GetPtr();
    tp.Alp =  (double *)aObj[ o_tpalp ]->GetPtr();
    tp.Bet =  (double *)aObj[ o_tpbet ]->GetPtr();
    tp.Wbor = (double *)aObj[ o_tpwbor ]->GetPtr();
    tp.Wrad = (double *)aObj[ o_tpwrad ]->GetPtr();
    tp.Fug =  (double *)aObj[ o_tpfug ]->GetPtr();
    tp.dVg =  (double *)aObj[ o_tpdvg ]->GetPtr();
    tp.DCMod  = (char (*)[6])aObj[ o_tpdcmod ]->GetPtr();
}

// free dynamic memory in objects and values
void TMTparm::dyn_kill(int /*q*/)
{
    tp.mark = (char *)aObj[ o_tpmark ]->Free();
    tp.G =    (double *)aObj[ o_tpg ]->Free();
    tp.devG = (double *)aObj[ o_tpdevg ]->Free();
    tp.H =    (double *)aObj[ o_tph ]->Free();
    tp.devH = (double *)aObj[ o_tpdevh ]->Free();
    tp.S =    (double *)aObj[ o_tps ]->Free();
    tp.devS = (double *)aObj[ o_tpdevs ]->Free();
    tp.Cp =   (double *)aObj[ o_tpcp ]->Free();
    tp.devC = (double *)aObj[ o_tpdevcp ]->Free();
    tp.Vm =   (double *)aObj[ o_tpvm ]->Free();
    tp.devV = (double *)aObj[ o_tpdevv ]->Free();
    tp.F =    (double *)aObj[ o_tpf ]->Free();
    tp.U =    (double *)aObj[ o_tpu ]->Free();
    tp.Cv =   (double *)aObj[ o_tpcv ]->Free();
    tp.Alp =  (double *)aObj[ o_tpalp ]->Free();
    tp.Bet =  (double *)aObj[ o_tpbet ]->Free();
    tp.Wbor = (double *)aObj[ o_tpwbor ]->Free();
    tp.Wrad = (double *)aObj[ o_tpwrad ]->Free();
    tp.Fug =  (double *)aObj[ o_tpfug ]->Free();
    tp.dVg =  (double *)aObj[ o_tpdvg ]->Free();
    tp.DCMod  = (char (*)[6])aObj[ o_tpdcmod ]->Free();
}

// realloc dynamic memory
void TMTparm::dyn_new(int /*q*/)
{
    ErrorIf( tp.L < 2, GetName(), "tp.L < 2");
    if( tp.Pres1 != S_OFF )
        tp.mark = (char *)aObj[ o_tpmark]->Alloc( tp.L, 1, A_ );
    else  tp.mark = (char *)aObj[ o_tpmark ]->Free();

    if( tp.PtvG != S_OFF )
        tp.G = (double *)aObj[ o_tpg]->Alloc(tp.L, 1, D_ );
    else tp.G =    (double *)aObj[ o_tpg ]->Free();

    if( tp.PtvdG != S_OFF )
        tp.devG = (double *)aObj[ o_tpdevg]->Alloc( tp.L, 1, D_ );
    else tp.devG = (double *)aObj[ o_tpdevg ]->Free();

    if( tp.PtvH != S_OFF )
        tp.H = (double *)aObj[ o_tph]->Alloc( tp.L, 1, D_ );
    else  tp.H =    (double *)aObj[ o_tph ]->Free();

    if( tp.PtvdH != S_OFF )
        tp.devH = (double *)aObj[ o_tpdevh]->Alloc( tp.L, 1, D_ );
    else tp.devH = (double *)aObj[ o_tpdevh ]->Free();

    if( tp.PtvS != S_OFF )
        tp.S = (double *)aObj[ o_tps]->Alloc( tp.L, 1, D_ );
    else  tp.S =    (double *)aObj[ o_tps ]->Free();

    if( tp.PtvdS != S_OFF )
        tp.devS = (double *)aObj[ o_tpdevs]->Alloc( tp.L, 1, D_ );
    else tp.devS = (double *)aObj[ o_tpdevs ]->Free();

    if( tp.PtvCp != S_OFF )
        tp.Cp = (double *)aObj[ o_tpcp]->Alloc( tp.L, 1, D_ );
    else   tp.Cp =   (double *)aObj[ o_tpcp ]->Free();

    if( tp.PtvdCp != S_OFF )
        tp.devC = (double *)aObj[ o_tpdevcp]->Alloc( tp.L, 1, D_ );
    else  tp.devC = (double *)aObj[ o_tpdevcp ]->Free();

    if( tp.PtvVm != S_OFF )
        tp.Vm = (double *)aObj[ o_tpvm]->Alloc( tp.L, 1, D_ );
    else   tp.Vm =   (double *)aObj[ o_tpvm ]->Free();

    if( tp.PtvdVm != S_OFF )
        tp.devV = (double *)aObj[ o_tpdevv]->Alloc( tp.L, 1, D_ );
    else  tp.devV = (double *)aObj[ o_tpdevv ]->Free();

    if( tp.PtvF != S_OFF )
        tp.F = (double *)aObj[ o_tpf]->Alloc( tp.L, 1, D_ );
    else  tp.F =    (double *)aObj[ o_tpf ]->Free();

    if( tp.PtvU != S_OFF )
        tp.U = (double *)aObj[ o_tpu]->Alloc( tp.L, 1, D_ );
    else  tp.U =    (double *)aObj[ o_tpu ]->Free();

    if( tp.PtCv != S_OFF )
        tp.Cv = (double *)aObj[ o_tpcv]->Alloc( tp.L, 1, D_ );
    else  tp.Cv =   (double *)aObj[ o_tpcv ]->Free();

    if( tp.PtvA != S_OFF )
        tp.Alp = (double *)aObj[ o_tpalp]->Alloc( tp.L, 1, D_ );
    else  tp.Alp =  (double *)aObj[ o_tpalp ]->Free();

    if( tp.PtvB != S_OFF )
        tp.Bet = (double *)aObj[ o_tpbet]->Alloc( tp.L, 1, D_ );
    else   tp.Bet =  (double *)aObj[ o_tpbet ]->Free();

    if( tp.PtvWb != S_OFF )
        tp.Wbor = (double *)aObj[ o_tpwbor]->Alloc( tp.Ls, 1, D_ );
    else tp.Wbor = (double *)aObj[ o_tpwbor ]->Free();

    if( tp.PtvWr != S_OFF )
        tp.Wrad = (double *)aObj[ o_tpwrad]->Alloc( tp.Ls, 1, D_ );
    else   tp.Wrad = (double *)aObj[ o_tpwrad ]->Free();

    if( tp.PtvFg != S_OFF )
        tp.Fug = (double *)aObj[ o_tpfug]->Alloc( tp.Lg, 1, D_ );
    else tp.Fug =  (double *)aObj[ o_tpfug ]->Free();

    if( tp.PtvdVg != S_OFF )
        tp.dVg = (double *)aObj[ o_tpdvg]->Alloc( tp.Lg, 4, D_ ); // 5 for PRSV  4 for CG EoS
    else tp.dVg =  (double *)aObj[ o_tpdvg ]->Free();

    tp.DCMod  = (char (*)[6])aObj[ o_tpdcmod ]->Alloc( tp.L, 1, 6 );
}


//set default information
void TMTparm::set_def( int /*q*/)
{
    TProfil *aPa=dynamic_cast<TProfil *>(aMod[RT_PARAM].get());
    memcpy( &tp.PunE, aPa->pa.TPpdc, 8 );
    memcpy( &tp.PtvG, aPa->pa.TPpvc, 20 );
    /*  tp.L = mup->L;
      tp.Ls = mup->Ls;
      tp.Lg = mup->Pg;
      tp.La = mup->Laq;
     */
    tp.L = tp.Ls =  tp.Lg =  tp.La = 0;
    memset( &tp.T, 0, 10*sizeof(double));
    tp.mark = 0;
    tp.G =    0;
    tp.devG = 0;
    tp.H =    0;
    tp.devH = 0;
    tp.S =    0;
    tp.devS = 0;
    tp.Cp =   0;
    tp.devC = 0;
    tp.Vm =   0;
    tp.devV = 0;
    tp.F =    0;
    tp.U =    0;
    tp.Cv =   0;
    tp.Alp =  0;
    tp.Bet =  0;
    tp.Wbor = 0;
    tp.Wrad = 0;
    tp.Fug = 0;
    tp.dVg = 0;
    tp.DCMod = 0;
}

// realoc memory to MTPARM structure
// cfg vectors and size in Project is const
void TMTparm::MTparmAlloc()
{
    RMULTS* mup = TRMults::sm->GetMU();
    tp.L = mup->L;
    tp.Ls = mup->Ls;
    tp.Lg = mup->Pg;
    tp.La = mup->Laq;
    char *PtvM_ = &tp.PtvG; /* set recalc */
    for(int i=0; i<20; i++ )
        if( PtvM_[i] == S_ON )
            PtvM_[i] = S_REM;
    if( tp.PtvG == S_OFF )
        tp.PtvG = S_REM;  /* default array */
    if( tp.PtvVm == S_OFF )
        tp.PtvVm = S_REM;  /* default array */
  if( tp.Lg )  // Added on 13.06.03 by KD
  {
    // SD 17/07/2009 tp.PtvdVg = S_REM; // Arrays used by CG-EoS and PRSV-EoS for fluids !
     tp.PtvFg = S_REM;
  }
    dyn_new();
    aSta.setdef();  //Added by Sveta 15/06/2002
}

#define  Fill_zero( type, val )   ( ((type) == 'T') ? (0) : (val) )

// realoc memory to MTPARM structure and load data to arrays
void TMTparm::LoadMtparm( double cT, double cP )
{
    int j, jf;
    double P_old, TC, TK, P;
    time_t tim;
    TDComp* aDC=dynamic_cast<TDComp *>(aMod[RT_DCOMP].get());
    aDC->ods_link(0);
    TReacDC* aRC=dynamic_cast<TReacDC *>(aMod[RT_REACDC].get());
    aRC->ods_link(0);
    TProfil *aPa=dynamic_cast<TProfil *>(aMod[RT_PARAM].get());    // added 07.06.05 by KD
    RMULTS* mup = TRMults::sm->GetMU();

    if( tp.L != mup->L ||  tp.Ls != mup->Ls ||
            tp.Lg != mup->Pg ||  tp.La != mup->Laq )
        Error( "MTparm", "Modelling project dimension error!");

    tp.curT=cT;
    tp.curP=cP;
    TC = cT;
    TK = TC + C_to_K;
    P_old = P = cP;
    tp.T = TC; tp.TK = TK; /* scales !!! */
if( P < 1e-5 )  // trial check  5.12.2006
  P = 1e-5;
    tp.P = P;
    tp.RT = R_CONSTANT * TK;

    if( tp.La && TC < 120. && TC >= 0.0 )
    { /* calc approximation of water properties Nordstrom ea, 1990 */
        if( TC < 1.75 )
        {
            TC = 1.75;
            TK = 273.16;
        }
        tp.RoW = 1.-pow(TC-3.9863,2)*(TC+288.9414)/508929.2/(TC+68.12963)+
                 0.011445*exp(-374.3/TC);
        tp.EpsW = 2727.428+0.6224107*TK -466.9151*log(TK) -52000.87/TK;
        /* These approximations are used only if SUPCRT92 submodule*/
        /* is not activated, e.g., no HKF data for aqueous species */
    }
    if( tp.Pres1 != S_OFF && tp.mark )
    	fillValue( tp.mark, '+', tp.L );

    aW.ods_link(0);

    aSpc.on_sat_curve = false; // 01.06.2016

    for( j=0; j < tp.L; j++ )
    {
        aW.ods_link(0);
        if( !aDC->firstDc() )
            aDC->ods_link(0);
        if( !aRC->firstRc() )
            aRC->ods_link(0);
        if( !aW.firstAW() )
            aW.ods_link(0);
        // show curent key ?? mup->DCS[j]+mup->SM[j]
        if( mup->DCS[j] == SRC_DCOMP )
            aDC->TryRecInp( mup->SM[j], tim, 0 );
        if( mup->DCS[j] == SRC_REACDC )
            aRC->TryRecInp( mup->SM[j], tim, 0 );


        /*calc thermodynamic data of Dependent Component*/
        aW.set_zero(0);
        aW.twp->P = tp.P;
        aW.twp->TC = TC;
        aW.twp->T = TK; // TC + C_to_K;
        aW.twp->unE = tp.PunE;
        aW.twp->unV = tp.PunV;
        if( mup->DCS[j] == SRC_DCOMP )
        {
            aDC->DCthermo( 0, 0 );
        }
        else aRC->RCthermo( 0, 0 );

        if( aWp.init && P_old < 1.00001e-5 && P < 1.00001e-5 )  //  Doubtful ??
        {  /* insert precision? calculated by Psat */
            P = tp.P = aW.twp->P;
            tp.PeosW = S_ON;
            tp.P_HKF = S_ON;
        }

        if( mup->DCS[j] == SRC_DCOMP )
          copyValues( tp.DCMod[j], aDC->dcp->pct, 6);
        else
          copyValues( tp.DCMod[j], aRC->rcp->pct, 6);

        /* load work structure to MTPARM */
        if( tp.PtvG != S_OFF )    tp.G[j] = Fill_zero( mup->DCC[j], aW.twp->G );
        if( tp.PtvdG != S_OFF )    tp.devG[j] = Fill_zero( mup->DCC[j], aW.twp->devG);
        if( tp.PtvH != S_OFF )    tp.H[j] = Fill_zero( mup->DCC[j], aW.twp->H);
        if( tp.PtvdH != S_OFF )   tp.devH[j] = Fill_zero( mup->DCC[j], aW.twp->devH);
        if( tp.PtvS != S_OFF )    tp.S[j] = Fill_zero( mup->DCC[j], aW.twp->S);
        if( tp.PtvdS != S_OFF )   tp.devS[j] = Fill_zero( mup->DCC[j], aW.twp->devS);
        if( tp.PtvCp != S_OFF )   tp.Cp[j] = Fill_zero( mup->DCC[j], aW.twp->Cp );
        if( tp.PtvdCp != S_OFF )  tp.devC[j] = Fill_zero( mup->DCC[j],aW.twp->devCp);
        if( tp.PtvVm != S_OFF )   tp.Vm[j] = Fill_zero( mup->DCC[j],aW.twp->V);
        if( tp.PtvdVm != S_OFF )  tp.devV[j] = Fill_zero( mup->DCC[j],aW.twp->devV);
//        if( tp.PtvF != S_OFF )    tp.F[j] = Fill_zero( mup->DCC[j],aW.twp->F); //SD 14/07/2009
//        if( tp.PtvU != S_OFF )    tp.U[j] = Fill_zero( mup->DCC[j],aW.twp->U);
//        if( tp.PtCv != S_OFF )    tp.Cv[j] = Fill_zero( mup->DCC[j],aW.twp->Cv);
        if( tp.PtvA != S_OFF )    tp.Alp[j] = Fill_zero( mup->DCC[j], aW.twp->Alp);
        if( tp.PtvB != S_OFF )    tp.Bet[j] = Fill_zero( mup->DCC[j],aW.twp->Bet);
//        if( tp.PtvWb != S_OFF && j< tp.Ls )   tp.Wbor[j] = aW.twp->Wbor;
//        if( tp.PtvWr != S_OFF && j< tp.Ls )   tp.Wrad[j] = aW.twp->Wrad;

        jf = j - tp.La; // gas/fluid phase(s) must immediately follow aqueous phase

//if( jf < 0 ) jf = j;  // provisional fix! DK 20.07.2006

        if( jf >= 0 && jf < tp.Lg )
        {
        	// TP corrected fugacity and EoS coeffs for gases/fluids
        	if( tp.PtvFg != S_OFF )
        	{
        		tp.Fug[jf] = aW.twp->Fug;

                        // back correction for PRSV EoS model
        		if( mup->DCS[j] == SRC_DCOMP && aDC->dcp->pct[2] == CPM_PRSV )  // since 15.02.2007 (DK)
        			tp.G[j] -= R_CONSTANT * tp.TK * log( aW.twp->Fug/tp.P );
                        // back correction for CG EoS model
        		if( mup->DCS[j] == SRC_DCOMP && aDC->dcp->pct[2] == CPM_EMP )  // since 15.02.2007 (DK)
        			tp.G[j] -= R_CONSTANT * tp.TK * log( aW.twp->Fug/tp.P );
                        // back correction for SRK EoS model
        		if( mup->DCS[j] == SRC_DCOMP && aDC->dcp->pct[2] == CPM_SRK )  // added 17.12.2008 (TW)
        			tp.G[j] -= R_CONSTANT * tp.TK * log( aW.twp->Fug/tp.P );
                        // back correction for PR78 EoS model
        		if( mup->DCS[j] == SRC_DCOMP && aDC->dcp->pct[2] == CPM_PR78 )  // added 18.07.2009 (TW)
        			tp.G[j] -= R_CONSTANT * tp.TK * log( aW.twp->Fug/tp.P );
                        // back correction for CORK EoS model
                        if( mup->DCS[j] == SRC_DCOMP && aDC->dcp->pct[2] == CPM_CORK )  // added 02.05.2010 (TW)
                                tp.G[j] -= R_CONSTANT * tp.TK * log( aW.twp->Fug/tp.P );
                        // back correction for STP EoS model
                        if( mup->DCS[j] == SRC_DCOMP && aDC->dcp->pct[2] == CPM_STP )  // added 05.12.2010 (TW)
                                tp.G[j] -= R_CONSTANT * tp.TK * log( aW.twp->Fug/tp.P );

        	}
        }

        /* set scales - not done yet?
        switch( tp.PunT )
    {
          case PVT_CELS:   tp.T = TC; tp.RT = ; break;
          case PVT_FAREN:
          default:
          case PVT_KELVIN: tp.T = TC+TK_DELTA; tp.RT = ;
    }
        switch( tp.PunE )
    {
          default:
          case TDAT_KJOUL: tp.RT /= 1000.;
          case TDAT_JOUL: tp.RT *= R_U; break;
          case TDAT_CAL:   tp.RT /= 1000.;
          case TDAT_KCAL: tp.RT *= R_U/4.184; break;
          case TDAT_NORM:  * no did! *;
    }
        switch( tp.PunP )
    {
          default:
          case PVT_KBAR:  tp.P = P/1000.; break;
          case PVT_BAR:   tp.P = P; break;
          case PVT_KPASC: tp.P = P*1e2; break;
          case PVT_PASC:  tp.P = P*1e5; break;
          case PVT_ATM:   tp.P = P/1.013; break;
    }
        */
        /* test T limit */

        if( tp.mark && tp.Pres1 != S_OFF )
        {
            if( mup->DCS[j] == SRC_DCOMP )
            {
                if( aDC->dcp->pct[0] != CTPM_HKF && aDC->dcp->TCint )
                {
                    if( aW.twp->TC < aDC->dcp->TCint[0] ||
                            aW.twp->TC > aDC->dcp->TCint[ aDC->dcp->NeCp*2 - 1 ] )
                        tp.mark[j] = CP_NOT_VALID;
                }
                else
                { /* HKF EOS */
                    if ( aW.twp->TC < 0.001 || aW.twp->TC > 1000. || aW.twp->P > 5000. )
                        tp.mark[j] = CP_NOT_VALID;
                }
            }
            if( mup->DCS[j] == SRC_REACDC && aRC->rcp->TCint )
            {
                short nTp, nPp;
                nTp = max( aRC->rcp->nTp, (short)2 );
                nPp = max( (short)2, aRC->rcp->nPp );
                if( aW.twp->TC < aRC->rcp->TCint[0] ||
                        aW.twp->TC > aRC->rcp->TCint[nTp-1] ||
                        aW.twp->P > aRC->rcp->Pint[nPp-1] )
                    tp.mark[j] = CP_NOT_VALID;
            }
       }

       //  01.06.2016
       int CV = toupper( aDC->dcp->pct[2] );
       if (CV == CPM_GAS)
           aWp.init = false;
       //

       if( aWp.init== true )
       { /* load water properties from HGK/HKF*/
         float b_gamma;
         double gfun;
         double rhow, epsw, alpw, dalw, betw, xbornw, ybornw, /*zbornw,*/ qbornw;
         double rhov, epsv, alpv, dalv, betv, xbornv, ybornv, /*zbornv,*/ qbornv;

         if (aSpc.metastable) // 01.06.2016
             aSpc.isat = 1;

         // pull water parameters from WATERPARAM
         rhow = aSta.Dens[aSpc.isat];  // water liquid
         alpw = aWp.Alphaw[aSpc.isat];
         dalw = aWp.dAldT[aSpc.isat];
         betw = aWp.Betaw[aSpc.isat];
         epsw = aWp.Dielw[aSpc.isat];
         xbornw = aWp.XBorn[aSpc.isat];
         ybornw = aWp.YBorn[aSpc.isat];
         //zbornw = aWp.ZBorn[aSpc.isat];
         qbornw = aWp.QBorn[aSpc.isat];
         rhov = aSta.Dens[!aSpc.isat];  // water vapor
         alpv = aWp.Alphaw[!aSpc.isat];
         dalv = aWp.dAldT[!aSpc.isat];
         betv = aWp.Betaw[!aSpc.isat];
         epsv = aWp.Dielw[!aSpc.isat];
         xbornv = aWp.XBorn[!aSpc.isat];
         ybornv = aWp.YBorn[!aSpc.isat];
         //zbornv = aWp.ZBorn[!aSpc.isat];
         qbornv = aWp.QBorn[!aSpc.isat];

         // recalculate and assign water properties
         tp.RoW  = aSta.Dens[aSpc.isat];  // density (g cm-3)
         tp.EpsW = aWp.Dielw[aSpc.isat];  // dielectric constant
         tp.VisW = aWp.Viscw[aSpc.isat];  //dynamic viscosity
         tp.dRdTW = - alpw * rhow;
         tp.d2RdT2W = rhow * ( pow(alpw,2.) - dalw );
         tp.dRdPW = betw * rhow;
         tp.dEdTW = ybornw * pow(epsw,2.);
         tp.d2EdT2W = (xbornw + 2.*epsw*pow(ybornw,2.)) * pow(epsw,2.);
         tp.dEdPW = qbornw * pow(epsw,2.);
         tp.RoV  = aSta.Dens[!aSpc.isat];  // density (g cm-3)
         tp.EpsV = aWp.Dielw[!aSpc.isat];  // dielectric constant
         tp.VisV = aWp.Viscw[!aSpc.isat];  //dynamic viscosity
         tp.dRdTV = - alpv * rhov;
         tp.d2RdT2V = rhov * ( pow(alpv,2.) - dalv );
         tp.dRdPV = betv * rhov;
         tp.dEdTV = ybornv * pow(epsv,2.);
         tp.d2EdT2V = (xbornv + 2.*epsv*pow(ybornv,2.)) * pow(epsv,2.);
         tp.dEdPV = qbornv * pow(epsv,2.);

         tp.d2RdP2W = 0.;
         tp.d2RdP2V = 0.;
         tp.d2EdP2W = 0.;
         tp.d2EdP2V = 0.;

         tp.PeosW = S_ON;
         tp.P_HKF = S_ON;
         if( tp.P < 1e-6 )     // added 25.11.05
             tp.P = aW.twp->P;
         // Added 07.06.05 for b_gamma=f(T,P) calculations
         gfun = aW.twp->gfun;
         b_gamma = aPa->pa.aqPar[0];
         switch( tp.Pbg )
         {
           default:
           case BG_CONST:  // constant b_gamma as specified in the dialog
               break;
           case BG_TP_NACL:
               aPa->pa.aqPar[0] = b_gamma_TP( TK, P, tp.EpsW, gfun, 1 );
               break;
           case BG_TP_KCL:
               aPa->pa.aqPar[0] = b_gamma_TP( TK, P, tp.EpsW, gfun, 2 );
               break;
           case BG_TP_NAOH:
               aPa->pa.aqPar[0] = b_gamma_TP( TK, P, tp.EpsW, gfun, 3 );
               break;
           case BG_TP_KOH:
               aPa->pa.aqPar[0] = b_gamma_TP( TK, P, tp.EpsW, gfun, 4 );
               break;
         }
         if( aPa->pa.aqPar[0] < -3.0 || aPa->pa.aqPar[0] > 1.0 )
           aPa->pa.aqPar[0] = b_gamma; // error - restoring old constant value
         aWp.init = false;
      }
    } /*j*/
    if( tp.PtvG == S_REM )  tp.PtvG = S_ON;
    if( tp.PtvdG== S_REM )  tp.PtvdG = S_ON;
    if( tp.PtvH == S_REM )  tp.PtvH = S_ON;
    if( tp.PtvdH == S_REM ) tp.PtvdH = S_ON;
    if( tp.PtvS == S_REM )  tp.PtvS = S_ON;
    if( tp.PtvdS == S_REM ) tp.PtvdS = S_ON;
    if( tp.PtvCp == S_REM ) tp.PtvCp = S_ON;
    if( tp.PtvdCp == S_REM )tp.PtvdCp = S_ON;
    if( tp.PtvVm == S_REM ) tp.PtvVm = S_ON;
    if( tp.PtvdVm == S_REM )tp.PtvdVm = S_ON;
    if( tp.PtvF == S_REM )  tp.PtvF = S_ON;
    if( tp.PtvU == S_REM )  tp.PtvU = S_ON;
    if( tp.PtCv == S_REM )  tp.PtCv = S_ON;
    if( tp.PtvA == S_REM )  tp.PtvA = S_ON;
    if( tp.PtvB == S_REM )  tp.PtvB = S_ON;
    if( tp.PtvWb == S_REM ) tp.PtvWb = S_ON;
    if( tp.PtvWr == S_REM ) tp.PtvWr = S_ON;
    if( tp.PtvFg == S_REM ) tp.PtvFg = S_ON;
    if( tp.PtvdVg == S_REM )tp.PtvdVg = S_ON;


    if( tp.mark && tp.G )
          polmod_test();
    
	NormDoubleRound(tp.G, tp.L, 13 ); // SD 22/07/2009
	NormDoubleRound(tp.RoW, 13 ); 
	NormDoubleRound(tp.EpsW, 13 ); 
//cout << "T = " << tp.T << " P= " << tp.P << " G[0] " << setprecision(18) << scientific << tp.G[0]<< endl;
}

// test polimorf modifications
void TMTparm::polmod_test()
{
    int i, j, k, jv;
    char savespn[MAXRKEYLEN];
    RMULTS* mup = TRMults::sm->GetMU();

    if( tp.Pres1 != S_REM )
        return;
    i= mup->Ls;
    while( i < mup->L-1 )
    {
        j=i+1;
        memcpy( savespn, mup->SM[i]+MAXSYMB, MAXDRGROUP );
        for( k=1; k<=16; k++, j++ )
        {
            if( memcmp( savespn, mup->SM[j]+MAXSYMB, MAXDRGROUP ))
                break;
        }
        if( k == 1 )
        {
            i++;
            continue;
        }
        /* have modifications */
        for( k=i+1, jv=i; k<j; k++ )
            if( tp.G[k] < tp.G[jv] )
                jv=k;
        for( k=i; k<j; k++ )
            if( k != jv )
                tp.mark[k] = PM_NOT_EXIST;
        i=j;
    }
}

//-----------------------------------------------------------------------------
/* Subroutine provided by D.Dolejs and Th.Wagner in May 2005
bgamma coefficient for electrolytes at P & T
external conditions:
tk - temperature (K)
pb - pressure (bar)
eps - dielectric constant
gsf(pb,tc,dh2o) is the g-function of solvent at p(bar), t(C) and water density,
obtained from HKF-EOS
returns b_gamma value or -1 in the case of error */
double
TMTparm::b_gamma_TP( double tk, double pb, double eps, double gsf, int mode )
{
   double bgm_result;
   double ni, a1, a2, a3, a4, a5, c1, c2, omg, bg, bs, rc, ra;
   double omgpt, nbg;
/* ni = stoichiometric number of moles of ions in one mole of electrolyte */
/* rc, ra - radius of cation and anion, respectively at 298 K/1 bar */
/* units are cal, kg, K, mol, bar */

   switch( mode )
   {
      case 1:  // NaCl
              ni=2.; a1=0.030056; a2=-202.55; a3=-2.9092; a4=20302;
              a5=-0.206; c1=-1.50; c2=53300.; omg=178650.;
              bg=-174.623; bs=2.164; rc=0.97; ra=1.81;
              break;
      case 2:  // KCl
              ni=2.; a1=0.0172; a2=-115.36; a3=-1.1857; a4=13854.2;
              a5=-0.262; c1=-2.53; c2=38628.4; omg=164870.;
              bg=-70.0; bs=1.727; rc=1.33; ra=1.81;
              break;
      case 3: // NaOH
              ni=2.; a1=0.030056; a2=-202.55; a3=-2.9092; a4=20302;
              a5=-0.206; c1=-1.50; c2=53300.; omg=205520.;
              bg=-267.4; bs=1.836; rc=0.97; ra=1.40;
              break;
      case 4: // KOH
              ni=2.; a1=0.0172; a2=-115.36; a3=-1.1857; a4=13854.2;
              a5=-0.262; c1=-2.53; c2=38628.4; omg=191730.;
              bg=-335.7; bs=1.26; rc=1.33; ra=1.40;
              break;
      default: // wrong mode
              return -1.;
   }
   // calculation part
   omgpt=1.66027e5*(1./(0.94+rc+gsf)+1./(ra+gsf));
   nbg=-ni*bg/2.+ni*bs*(tk-298.15)/2.-c1*(tk*log(tk/298.15)-tk+298.15)
      +a1*(pb-1.)+a2*log((2600.+pb)/(2600.+1.))
      -c2*((1./(tk-228.)-1./(298.15-228.))*(228.-tk)/228.-tk/(228.*228.)
      *log((298.15*(tk-228.))/(tk*(298.15-228.))))
      +1./(tk-228.)*(a3*(pb-1.)+a4*log((2600.+pb)/(2600.+1.)))
      +a5*(omgpt*(1./eps-1.)-omg*(1./78.24513-1.)-5.80e-5*omg*(tk-298.15));
   bgm_result=nbg/(2.*log(10.)*1.98721*tk);
   return bgm_result;
}


void TMTparm::LoadDataToLookup( QWidget* par, DATACH* CSD )
{

    int kk, ip, it, ll, jj;
    double cT, cP;
    int *tp_mark;
    tp_mark = new int[tp.L];
    fillValue( tp_mark, 0, tp.L);

    if( tp.L != TRMults::sm->GetMU()->L  )
        Error( "MTparm", "Modelling project dimension error!");
    tp_mark = new int[tp.L];
    fillValue( tp_mark, 0, tp.L);

    for(  it=0; it<CSD->nTp; it++)
    {
      cT = CSD->TKval[it]-C_to_K;
      for(  ip=0; ip<CSD->nPp; ip++)
      {
       if( par )
          pVisor->Message( par, "Building lookup arrays",
              "Loading thermodynamic data", it*CSD->nPp+ip, CSD->nTp*CSD->nPp );

       cP = CSD->Pval[ip]/1e5; //bar_to_Pa;
       // calculates new G0, V0, H0, Cp0, S0
       if( fabs( tp.curT - cT ) > 1.e-10 ||
              fabs( tp.curP - cP ) > 1.e-10 || TMulti::sm->GetPM()->pTPD == -1 )
       { // load new MTPARM on T or P
           LoadMtparm( cT, cP );
           TMulti::sm->GetPM()->pTPD = 0;
       }

      if( cP < 1e-5 )
       CSD->Psat[it ] = tp.P*1e5;
      else // DM added 09.06.2016
      {

//          double Psat = 0.0;
          TSupcrt supCrt;
//          supCrt.Supcrt_H2O(cT, &Psat);
//          CSD->Psat[it] = Psat * 100000; // in Pa
          if (cT+273.15 < 647.067e0)
            CSD->Psat[it] = supCrt.getPsatHGK(cT + 273.15)*10.0*100000; // in Pa
      }

//      cout << "tp.RoW = " << tp.RoW << " tp.EpsW = " << tp.EpsW << endl;

      CSD->denW[( 0 * CSD->nPp + ip) * CSD->nTp + it] = tp.RoW    *1e3;
      CSD->denW[( 1 * CSD->nPp + ip) * CSD->nTp + it] = tp.dRdTW  *1e3;
      CSD->denW[( 2 * CSD->nPp + ip) * CSD->nTp + it] = tp.d2RdT2W*1e3;
      CSD->denW[( 3 * CSD->nPp + ip) * CSD->nTp + it] = tp.dRdPW  *1e3;
      CSD->denW[( 4 * CSD->nPp + ip) * CSD->nTp + it] = tp.d2RdP2W*1e3;
      CSD->denWg[( 0 * CSD->nPp + ip) * CSD->nTp + it] = tp.RoV   *1e3;
      CSD->denWg[( 1 * CSD->nPp + ip) * CSD->nTp + it] = tp.dRdTV *1e3;
      CSD->denWg[( 2 * CSD->nPp + ip) * CSD->nTp + it] = tp.d2RdT2V*1e3;
      CSD->denWg[( 3 * CSD->nPp + ip) * CSD->nTp + it] = tp.dRdPV  *1e3;
      CSD->denWg[( 4 * CSD->nPp + ip) * CSD->nTp + it] = tp.d2RdP2V*1e3;
      CSD->epsW[( 0 * CSD->nPp +  ip) * CSD->nTp + it] = tp.EpsW;
      CSD->epsW[( 1 * CSD->nPp + ip) * CSD->nTp + it] = tp.dEdTW;
      CSD->epsW[( 2 * CSD->nPp + ip) * CSD->nTp + it] = tp.d2EdT2W;
      CSD->epsW[( 3 * CSD->nPp + ip) * CSD->nTp + it] = tp.dEdPW;
      CSD->epsW[( 4 * CSD->nPp + ip) * CSD->nTp + it] = tp.d2EdP2W;
      CSD->epsWg[( 0 * CSD->nPp + ip) * CSD->nTp + it] = tp.EpsV;
      CSD->epsWg[( 1 * CSD->nPp + ip) * CSD->nTp + it] = tp.dEdTV;
      CSD->epsWg[( 2 * CSD->nPp + ip) * CSD->nTp + it] = tp.d2EdT2V;
      CSD->epsWg[( 3 * CSD->nPp + ip) * CSD->nTp + it] = tp.dEdPV;
      CSD->epsWg[( 4 * CSD->nPp + ip) * CSD->nTp + it] = tp.d2EdP2V;

      // copy to arrays
     for( kk=0; kk<CSD->nDC; kk++)
      {
         ll = ( kk * CSD->nPp + ip) * CSD->nTp + it;
         jj= TMulti::sm->GetPM()->muj[kk];

         if( tp.mark[jj] == 'e'/*CP_NOT_VALID*/ )
             tp_mark[jj] = 1;

         CSD->G0[ll] =  tp.G[jj]+ TSyst::sm->GetSY()->GEX[jj]; //
         CSD->V0[ll] =  tp.Vm[jj]*1e-5;
         if( tp.H )
           CSD->H0[ll] = tp.H[jj];
         else
           CSD->H0[ll] = 0;
         if( tp.S )
           CSD->S0[ll] = tp.S[jj];
         else
           CSD->S0[ll] = 0;
         if( tp.Cp )
           CSD->Cp0[ll] =  tp.Cp[jj];
         else
           CSD->Cp0[ll] =  0.;
         if( tp.F )
           CSD->A0[ll] =  tp.F[jj];
         else
           CSD->A0[ll] = 0.;
         if( tp.U )
           CSD->U0[ll] =  tp.U[jj];
         else
           CSD->U0[ll] = 0.;
       }
     } //ip
  } //it
  if( par )
     pVisor->CloseMessage();

  string err = "";
  for( jj=0, kk=0; kk<CSD->nDC; kk++)
  {
     if( tp_mark[TMulti::sm->GetPM()->muj[kk]]==1 )
     {	  err +=" ";
          err += string(CSD->DCNL[kk],0, MaxDCN);
          if(!((++jj)%5)) err += "\n";
     }
  }
  //if( !err.empty() )
  //           vfMessage(par,"Not quality for TP dependencies of DC", err.c_str());
  //cout <<  "Not quality for TP dependencies of DC" << err.c_str() << endl;
  // free memory
  delete[] tp_mark;
}

void TMTparm::LoadDataToPair( QWidget* par, DATACH* CSD )
{

    int kk, ip, it, ll, jj;
    double cT, cP;
    int *tp_mark;
    tp_mark = new int[tp.L];
    fillValue( tp_mark, 0, tp.L);

    if( tp.L != TRMults::sm->GetMU()->L  )
        Error( "MTparm", "Modelling project dimension error!");
    tp_mark = new int[tp.L];
    fillValue( tp_mark, 0, tp.L);

    for(  ip=0; ip<CSD->nPp; ip++)
      {

       if( par )
          pVisor->Message( par, "Building lookup arrays",
              "Loading thermodynamic data", ip, CSD->nPp );

       it = min<int>( ip, CSD->nTp-1 );
       cT = CSD->TKval[it]-C_to_K;
       cP = CSD->Pval[ip]/1e5; //bar_to_Pa;
       // calculates new G0, V0, H0, Cp0, S0
       if( fabs( tp.curT - cT ) > 1.e-10 ||
              fabs( tp.curP - cP ) > 1.e-10 || TMulti::sm->GetPM()->pTPD == -1)
       { // load new MTPARM on T or P
           LoadMtparm( cT, cP );
           TMulti::sm->GetPM()->pTPD = 0;
       }

      if( cP < 1e-5 )
       CSD->Psat[it] = tp.P*1e5;
      else // DM added 09.06.2016
      {
//          double Psat = 0.0;
          TSupcrt supCrt;
//          supCrt.Supcrt_H2O(cT, &Psat);
//          CSD->Psat[it] = Psat*100000;
          if (cT+273.15 < 647.067e0)
            CSD->Psat[it] = supCrt.getPsatHGK(cT + 273.15)*10.0*100000; // in Pa
      }

      CSD->denW[( 0 * CSD->nPp + ip) ] = tp.RoW    *1e3;
      CSD->denW[( 1 * CSD->nPp + ip) ] = tp.dRdTW  *1e3;
      CSD->denW[( 2 * CSD->nPp + ip) ] = tp.d2RdT2W*1e3;
      CSD->denW[( 3 * CSD->nPp + ip) ] = tp.dRdPW  *1e3;
      CSD->denW[( 4 * CSD->nPp + ip) ] = tp.d2RdP2W*1e3;
      CSD->denWg[( 0 * CSD->nPp + ip) ] = tp.RoV   *1e3;
      CSD->denWg[( 1 * CSD->nPp + ip) ] = tp.dRdTV *1e3;
      CSD->denWg[( 2 * CSD->nPp + ip) ] = tp.d2RdT2V*1e3;
      CSD->denWg[( 3 * CSD->nPp + ip) ] = tp.dRdPV  *1e3;
      CSD->denWg[( 4 * CSD->nPp + ip) ] = tp.d2RdP2V*1e3;
      CSD->epsW[( 0 * CSD->nPp +  ip) ] = tp.EpsW;
      CSD->epsW[( 1 * CSD->nPp + ip) ] = tp.dEdTW;
      CSD->epsW[( 2 * CSD->nPp + ip) ] = tp.d2EdT2W;
      CSD->epsW[( 3 * CSD->nPp + ip) ] = tp.dEdPW;
      CSD->epsW[( 4 * CSD->nPp + ip) ] = tp.d2EdP2W;
      CSD->epsWg[( 0 * CSD->nPp + ip)] = tp.EpsV;
      CSD->epsWg[( 1 * CSD->nPp + ip)] = tp.dEdTV;
      CSD->epsWg[( 2 * CSD->nPp + ip)] = tp.d2EdT2V;
      CSD->epsWg[( 3 * CSD->nPp + ip)] = tp.dEdPV;
      CSD->epsWg[( 4 * CSD->nPp + ip)] = tp.d2EdP2V;


      // copy to arrays
     for( kk=0; kk<CSD->nDC; kk++)
      {
         ll = ( kk * CSD->nPp + ip);
         jj= TMulti::sm->GetPM()->muj[kk];

         if( tp.mark[jj] == 'e'/*CP_NOT_VALID*/ )
             tp_mark[jj] = 1;

         CSD->G0[ll] =  tp.G[jj]+ TSyst::sm->GetSY()->GEX[jj]; //
         CSD->V0[ll] =  tp.Vm[jj]*1e-5;
         if( tp.H )
           CSD->H0[ll] = tp.H[jj];
         else
           CSD->H0[ll] = 0;
         if( tp.S )
           CSD->S0[ll] = tp.S[jj];
         else
           CSD->S0[ll] = 0;
         if( tp.Cp )
           CSD->Cp0[ll] =  tp.Cp[jj];
         else
           CSD->Cp0[ll] =  0.;
         if( tp.F )
           CSD->A0[ll] =  tp.F[jj];
         else
           CSD->A0[ll] = 0.;
         if( tp.U )
           CSD->U0[ll] =  tp.U[jj];
         else
           CSD->U0[ll] = 0.;
       }
     } //ip
  if( par )
     pVisor->CloseMessage();

  string err = "";
  for( jj=0, kk=0; kk<CSD->nDC; kk++)
  {
     if( tp_mark[TMulti::sm->GetPM()->muj[kk]]==1 )
     {	  err +=" ";
          err += string(CSD->DCNL[kk],0, MaxDCN);
          if(!((++jj)%5)) err += "\n";
     }
  }
  delete[] tp_mark;
}

//--------------------- End of ms_mtparm.cpp ---------------------------

