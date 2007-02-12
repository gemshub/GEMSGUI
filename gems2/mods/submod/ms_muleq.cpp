//-------------------------------------------------------------------
// $Id$
//
// Implementation of TMulti  load functions
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
#include <stdio.h>
#include <math.h>

#include "m_param.h"
#include "m_syseq.h"
#include "service.h"
//#include "visor.h"

// set size of packed multi arrays
void TMulti::setSizes()
{
    short j, Lp;
    TSysEq* STat = (TSysEq*)(&aMod[RT_SYSEQ]);

    // short
    STat->stp->N = pm.N;
    STat->stp->Fi = pm.FI1;
    STat->stp->Fis = pm.FI1s;
    STat->stp->itIPM = pm.IT;
    STat->stp->itPar = pm.W1;  // Number of IPM-2 loops KD 29.11.01
    // float
    STat->stp->V = pm.VXc;
    STat->stp->T = pm.Tc;
    STat->stp->P = pm.Pc;
    STat->stp->H = pm.HXc;
    STat->stp->S = pm.SXc;
    STat->stp->UU = pm.FX;
    STat->stp->PCI = pm.PCI;
    STat->stp->ParE = pm.FitVar[3]; /* Tinkle supress factor */
    //STat->stp->Res1 = pm.IC;
    /* calc fakt count of DC */
    for( Lp=0,j=0; j<pm.L; j++ )
        if( pm.X[j] > pm.lowPosNum )
            Lp++;
    STat->stp->L = Lp;
}

// packed multi arrays to equstat
void TMulti::packData()
{
    short i,j;
    TSysEq* STat = (TSysEq*)(&aMod[RT_SYSEQ]);

    /* load data */
    for( i=0,j=0; j<pm.L; j++ )
        if( pm.X[j] > pm.lowPosNum )
        {
            STat->stp->llf[i] = pm.muj[j];
            STat->stp->Y[i] = pm.X[j];
            STat->stp->lnGam[i] = pm.lnGam[j];
            /* Removed by KD 13 May 02 */
//          if( pm.lnSAT && ( pm.SATT[j] == SAT_SITE || pm.SATT[j] == SAT_COMP
//                            || pm.SATT[j] == SAT_NCOMP ))
//          STat->stp->lnGam[i] += pm.lnSAT[j];  /* end insert */
            //     memcpy( STat->stp->SMp[i], pm.SM[j], MAXDCNAME );
            i++;
        }
    for( i=0; i<pm.N; i++ )
    {
        STat->stp->nnf[i] = pm.mui[i];
        STat->stp->B[i] = pm.B[i];
        STat->stp->U[i] = pm.U[i];
        //    memcpy( STat->stp->SBp[i], pm.SB[i], MAXICNAME+MAXSYMB );
    }

    for( i=0, j=0; j<pm.FIs; j++ )
        if( pm.YF[j] > 1e-18 )
        {
            STat->stp->phf[i] = pm.muk[j];
            for( int k=0; k<pm.N; k++ )
              *(STat->stp->Ba+i*pm.N+k) = *(pm.BF+j*pm.N+k);
            //       memcpy( STat->stp->SFp[i], pm.SF[i], MAXPHNAME );
            i++;
        }
}

// packed multi arrays to equstat (to z_sp_conf mode)
void TMulti::packData( TCIntArray PHon, TCIntArray DCon )
{
    short i,j;
    TSysEq* STat = (TSysEq*)(&aMod[RT_SYSEQ]);

    /* load data */
    for( i=0,j=0; j<pm.L; j++ )
        if( pm.X[j] > pm.lowPosNum )
        {
            STat->stp->llf[i] = (short)DCon[pm.muj[j]];
            STat->stp->Y[i] = pm.X[j];
            STat->stp->lnGam[i] = pm.lnGam[j];
            /* Removed by KD 13 May 02 */
//          if( pm.lnSAT && ( pm.SATT[j] == SAT_SITE || pm.SATT[j] == SAT_COMP
//                           || pm.SATT[j] == SAT_NCOMP ))
//             STat->stp->lnGam[i] += pm.lnSAT[j];  /* end insert */
            //     memcpy( STat->stp->SMp[i], pm.SM[j], MAXDCNAME );
            i++;
        }
    for( i=0; i<pm.N; i++ )
    {
        STat->stp->nnf[i] = pm.mui[i];
        STat->stp->B[i] = pm.B[i];
        STat->stp->U[i] = pm.U[i];
        //    memcpy( STat->stp->SBp[i], pm.SB[i], MAXICNAME+MAXSYMB );
    }

    for( i=0, j=0; j<pm.FIs; j++ )
        if( pm.YF[j] > 1e-18 )
        {
            STat->stp->phf[i] = (short)PHon[pm.muk[j]];
            for( int k=0; k<pm.N; k++ )
              *(STat->stp->Ba+i*pm.N+k) = *(pm.BF+j*pm.N+k);
            //       memcpy( STat->stp->SFp[i], pm.SF[i], MAXPHNAME );
            i++;
        }
}

// unpacked equstat arrays to multi (Y to pm.Y)
void TMulti::unpackData()
{
    TSysEq* STat = (TSysEq*)(&aMod[RT_SYSEQ]);
    TProfil* Prf = (TProfil*)(&aMod[RT_PARAM]);

    int i, j, js, jp, is, ip;

   if( pm.pESU == 2 )   /*?multi?*/
   {     pm.IT = 0;
        return;
   }

    for( is=0; is<STat->stp->N; is++ )
    {
        i = STat->stp->nnf[is];
        /*search iComp in multi */
        for( j=0; j<pm.N; j++ )
            if( i != pm.mui[j] )
                continue;
            else
            {
                ip = j;
                goto FOUNDI;
            }
        pm.pNP = 1;
        //   gstring err = "No '";
        //   err += gstring(TProfil::pm->mup->SB[i], 0, IC_RKLEN);
        //   err += "' IComp in system.";
        Error( GetName(), "no such IComp in this system" );
FOUNDI:
        pm.U[ip] = STat->stp->U[is];
        if( pm.pESU != 2 ) pm.B[ip] = STat->stp->B[is];    // Added
    }

    /* Inserted by DAK 15.11.98 in Mainz */
    for( j=0; j<pm.L; j++ )
    { /* Cleaning  Y and lnGam vectors */
        pm.Y[j] = 0.0;
        pm.lnGam[j] = 0.0;
    }

    for( js=0; js<STat->stp->L; js++ )
    {
        j = Prf->indDC( STat->stp->llf[js] );
        /* search component in system */
        for( i=0; i<pm.L; i++ )
            if( j != pm.muj[i] )
                continue;
            else
            {
                jp = i;
                goto FOUND;
            }
        pm.pNP = 1;
        Error( GetName(), "No this DComp in system." );
FOUND:
        pm.Y[jp] = STat->stp->Y[js];
        pm.lnGam[jp] = STat->stp->lnGam[js];
        /* Inserted by DAK 08 Mar 98 to handle SAT at PIA */
// Removed by KD 13.05.02
//        if( pm.lnSAT && ( pm.SATT[jp] == SAT_SITE || pm.SATT[jp] == SAT_COMP
//                          || pm.SATT[jp] == SAT_NCOMP ))
//            pm.lnSAT[jp] = STat->stp->lnGam[js];  /* end insert */
    }

    // short
    pm.pRR1 = STat->stp->itPar;  /* Level of tinkle supressor */
    pm.FI1 = STat->stp->Fi;
    pm.FI1s = STat->stp->Fis;
    pm.IT = 0;
    // float
    pm.FitVar[4] = STat->stp->ParE;  /* Tinkle supress factor */
    pm.FX  = STat->stp->UU;  /* GX norm */
    pm.PCI = STat->stp->PCI;
    pm.VXc = STat->stp->V;
    pm.HXc = STat->stp->H;
    pm.SXc = STat->stp->S;
    // pm.IC = STat->stp->Res1;

    pm.pESU = 2;
    //   if( pm.pFAG == 2 )
    //     pm.pFAG = 1; /* we must do EqustatExpand after unpack, add Sveta */
}

// load data from TSysEq (EQstat => Multi)

void TMulti::loadData( bool newRec )
{
    // nesessary to realloc data after new system   (before function )
    // setDefData();
    if( newRec == false )
        unpackData();
}

// =================================================================
// FROM MS_MULOAD

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Load Thermodynamic Data from MTPARM to MULTI
void TMulti::CompG0Load()
{
    int j, jj, k, jb, je=0;
    float Gg = 0., Vv = 0.;

    /* pTPD state of reload t/d data 0-all, 1 G0, 2 do not*/
    if( pmp->pTPD < 1 )
    {
        pmp->T = pmp->Tc = tpp->T + C_to_K;
        pmp->TC = pmp->TCc = tpp->T;
        if( tpp->P > 1e-9 )
            pmp->P = pmp->Pc = tpp->P;
        else pmp->P = pmp->Pc = 1e-9;
// Added 07.06.05 for T,P dependent b_gamma   KD
        pmp->FitVar[0] = TProfil::pm->pa.aqPar[0];
        pmp->denW = tpp->RoW;
        pmp->denWg = tpp->RoV;
        pmp->epsW = tpp->EpsW;
        pmp->epsWg = tpp->EpsV;
        pmp->RT = tpp->RT; // R_CONSTANT * pm->Tc
        pmp->FRT = F_CONSTANT/pmp->RT;
        pmp->lnP = log( pmp->P );
    }
    if( pmp->pTPD <= 1 )
    {
        for( k=0; k<pmp->FI; k++ )
        {
            jb = je;
            je += pmp->L1[k];
            /*load t/d data from DC */
            for( j=jb; j<je; j++ )
            {
                jj = pmp->muj[j];
                if( syp->Guns )
                    Gg = syp->Guns[jj];
                pmp->G0[j] = Cj_init_calc( tpp->G[jj]+Gg, j, k );
            }
        }
    }
    if( !pmp->pTPD )
    {
        int xVol = 0;   // SD 09/02/2007
        if( pmp->PV == VOL_CONSTR )
          xVol = getXvolume();

        for( j=0; j<pmp->L; j++ )
        {
            jj = pmp->muj[j];

            if( tpp->PtvVm == S_ON )
                switch( pmp->PV )
                { /* make mol volumes of components */
                case VOL_CONSTR:
                    if( syp->Vuns )
                       Vv = syp->Vuns[jj];
                    pmp->A[j*pmp->N+xVol] = tpp->Vm[jj]+Vv;
                case VOL_CALC:
                case VOL_UNDEF:
                    if( syp->Vuns )
                       Vv = syp->Vuns[jj];
                    pmp->Vol[j] = (tpp->Vm[jj]+Vv ) * 10.;
                    break;
                }
            else pmp->Vol[j] = 0.0;

            /* load other t/d parametres - do it! */
        }
    }
    Alloc_internal(); // optimization 08/02/2007

  pmp->pTPD = 2;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Calculation of full structure MULTI by expanding
// SysEq main vectors read from the database
//
void TMulti::EqstatExpand( const char *key )
{
    int i, j, k, jb, je/*=0*/, jpb, jpe=0, jdb, jde=0;
    double FitVar3;

//    if( !pmp->NR )       Sveta 30/08/01
        pmp->NR = pmp->N;

    /* Load thermodynamic data for DC, if necessary */
    if(  pmp->pTPD < 2)
    {
        CompG0Load();
        memcpy( pmp->stkey, key, EQ_RKLEN );
        pmp->stkey[EQ_RKLEN]='\0';
    }
    /* Load activity coeffs for phases-solutions */
    if( pmp->FIs )
    {
        for( j=0; j< pmp->Ls; j++ )
        {
            pmp->lnGmo[j] = pmp->lnGam[j];
            if( fabs( pmp->lnGam[j] ) <= 84. )
                pmp->Gamma[j] = exp( pmp->lnGam[j] );
            else pmp->Gamma[j] = 1;
        }
    }
    /* recalc kinetic restrictions for DC */
    if( pmp->pULR && pmp->PLIM )
         Set_DC_limits( DC_LIM_INIT );

    TotalPhases( pmp->X, pmp->XF, pmp->XFA );
    for( j=0; j<pmp->L; j++ )
        pmp->Y[j] = pmp->X[j];

    for( k=0; k<pmp->FI; k++ )
    {
        pmp->YF[k] = pmp->XF[k];
        if( k<pmp->FIs )
            pmp->YFA[k] = pmp->XFA[k];
    }
    /* set IPM weight multipliers for DC*/
    WeightMultipliers( false );

    /* Calculate elemental chemical potentials in J/mole */
    for( i=0; i<pmp->N; i++ )
        pmp->U_r[i] = pmp->U[i]*pmp->RT;

    ConCalc( pmp->X, pmp->XF, pmp->XFA);
    /* Calculate mass-balance deviations (moles) */
    MassBalanceDeviations( pmp->N, pmp->L, pmp->A, pmp->X, pmp->B, pmp->C );
    /* Calc Eh, pe, pH,and other stuff */
    if( pmp->E && pmp->LO )
        IS_EtaCalc();

    FitVar3 = pmp->FitVar[3];   /* Switch off smoothing factor */
    pmp->FitVar[3] = 1.0;
    /* Scan phases to retrieve concentrations and activities */
    je = 0;
    for( k=0; k<pmp->FIs; k++ )
    {
        jb = je;
        je = jb+pmp->L1[k];


        jpb = jpe;
jpe += pmp->LsMod[k*3]*pmp->LsMod[k*3+2];
// ipe ????
        jdb = jde;
        jde += pmp->LsMdc[k]*pmp->L1[k];

        if( pmp->PHC[k] == PH_SORPTION )
        {
            if( pmp->E && pmp->LO )
                GouyChapman( jb, je, k );
            /* calculation of surface activity terms */
            SurfaceActivityCoeff( jb, je, jpb, jdb, k );
//            SurfaceActivityTerm(  jb, je, k );
        }
        for( j=jb; j<je; j++ )
        {
            /* calc Excess Gibbs energies F0 and values c_j */
            pmp->F0[j] = Ej_init_calc( 0, j, k );
//            pmp->G[j] = pmp->G0[j] + pmp->F0[j];
            pmp->G[j] = pmp->G0[j] + pmp->GEX[j] + pmp->F0[j];
            /* pmp->Gamma[j] = exp( pmp->lnGam[j] ); */
        }
    }
    pmp->FitVar[3]=FitVar3;
    pmp->GX_ = pmp->FX * pmp->RT;
    /* calc Prime DC chemical potentials defined via g0_j, Wx_j and lnGam_j */
    PrimeChemicalPotentials( pmp->F, pmp->X, pmp->XF, pmp->XFA );
    /*calc Karpov phase criteria */
    f_alpha();
    /*calc gas partial pressures  -- obsolete? */
    GasParcP();
    pmp->pFAG = 2;
}

//Calculation by IPM (preparing for calculation, unpack data)
// key contains SysEq record key
//
void TMulti::MultiCalcInit( const char *key )
{
    int j, k;

   // Bulk composition and/or dimensions changed ?
    if( pmp->pBAL < 2 || pmp->pTPD < 2)
        MultiRemake( key );

        /* calc mass of system */
        pmp->MBX = 0.0;
        for(int i=0; i<pmp->N; i++ )
        {
            pmp->MBX += pmp->B[i] * (double)pmp->Awt[i];
        }
        pmp->MBX /= 1000.;

    // unpackSysEq record
    if( pmp->pESU /*== 1*/ && pmp->pNP )     // problematic statement !!!!!!!!!
    {
        unpackData(); // loading data from EqstatUnpack( key );
        pmp->IC = 0.;
        for( j=0; j< pmp->L; j++ )
            pmp->X[j] = pmp->Y[j];
        TotalPhases( pmp->X, pmp->XF, pmp->XFA );
    }
    else // Simplex initial approximation to be done
        for( j=0; j<pmp->L; j++ )
            pmp->X[j] = pmp->Y[j] = 0.0;  // 01/12/2006 SD

    //  if( wn[W_EQCALC].status )
    //    aSubMod[MD_EQCALC]->ModUpdate("PM_asm4   Assembling IPM arrays (4)");

    // loading thermodynamic data, if neccessary
    if( pmp->pTPD < 2)
        CompG0Load( );


    for( j=0; j< pmp->L; j++ )
    {
        /* pmp->Y[j] = pmp->X[j]; */
//        pmp->G[j] = pmp->G0[j]; /* pmp->GEX[j]; */
        pmp->G[j] = pmp->G0[j] + pmp->GEX[j];    // changed 5.12.2006
    }
    // test phases - solutions and load models
    if( pmp->FIs )
    {
        for( j=0; j< pmp->Ls; j++ )
        {
            pmp->lnGmo[j] = pmp->lnGam[j];
            pmp->Gamma[j] = 1.0;
        }
    }
    if( pmp->FIs /*&& pmp->pIPN <=0*/ )  // SD 29/11/2006
    { // not done if already present in MULTI !
        pmp->PD = TProfil::pm->pa.p.PD;
        SolModLoad();
        /*   pmp->pIPN = 1; */
         GammaCalc( LINK_TP_MODE);
    }
    else
    {
        if( !pmp->FIs )
        { /* no multi-component phases */
            pmp->PD = 0;
            pmp->pIPN = 1;
        }
    }
    // recalc restrictions for DC quantities
    if( pmp->pULR && pmp->PLIM )
         Set_DC_limits(  DC_LIM_INIT );

    //   aObj[o_w_sbh].SetPtr( pmp->SB[0]  ); aObj[ o_w_sbh].SetDim( 1, pmp->N );
    if( !pmp->SFs )
        pmp->SFs = (char (*)[MAXPHNAME+MAXSYMB])aObj[ o_wd_sfs].Alloc(
                       pmp->FI, 1, MAXPHNAME+MAXSYMB );

    // dynamic arrays - begin load
    for( k=0; k<pmp->FI; k++ )
    {
        pmp->XFs[k] = pmp->YF[k];
        pmp->Falps[k] = pmp->Falp[k];
        memcpy( pmp->SFs[k], pmp->SF[k], MAXPHNAME+MAXSYMB );
    }
}



//--------------------- End of ms_muleq.cpp ---------------------------

