//-------------------------------------------------------------------
// $Id$
//
// Copyright (C) 1992-2000  D.Kulik, S.Dmitrieva, K.Chudnenko, I.Karpov
//
// Implementation of chemistry-specific functions (concentrations,
// activity coefficients, adsorption models etc.)
// for convex programming Gibbs energy minimization, described in:
// (Karpov, Chudnenko, Kulik (1997): American Journal of Science
//  v.297 p. 767-806); Kulik (2000), Geoch.Cosmoch.Acta 64,3161-3179
//
// This file is part of a GEM-Selektor (GEMS) v.2.x.x program
// environment for thermodynamic modeling in geochemistry
// Uses: GEM-Vizor GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://les.web.psi.ch/Software/GEMS-PSI for more information
// E-mail: gems2.support@psi.ch; chud@igc.irk.ru
//-------------------------------------------------------------------
//
#include "m_param.h"

//#define _no_fgl_const_
#include "s_fgl.h"

#include <math.h>
#include <iostream>
#include <fstream>

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Calc  partial pressures of gases - to remake using dual potentials
void TProfil::GasParcP()
{
    short k,  i,  jj;
    int jb, je, j;

    if( !pmp->PG )
        return;
    for( k=0, je=0; k<pmp->FIs; k++ ) // phase
    {
        jb = je;
        je = jb+pmp->L1[k];
        if( pmp->PHC[k] == PH_GASMIX || pmp->PHC[k] == PH_PLASMA
           || pmp->PHC[k] == PH_FLUID )
        {
#ifndef IPMGEMPLUGIN
            char (*SMbuf)[MAXDCNAME] =
                (char (*)[MAXDCNAME])aObj[ o_w_tprn].Alloc( pmp->PG, 1, MAXDCNAME );
            memcpy(SMbuf,pmp->SM[jb], pmp->PG*MAXDCNAME*sizeof(char) );
            //     aObj[ o_w_tprn].SetPtr( pmp->SM[jb] );
            //     aObj[ o_w_tprn].SetDim( pmp->PG, 1 );
#endif
            for( j=jb,jj=0; j<je; j++,jj++ )
            {  /* fixed 02.03.98 DAK */
                pmp->Fug_l[jj] = -pmp->G0[j] /* + pmp->lnGam[j] */;
                if( pmp->Pc > 1e-9 )
                    pmp->Fug_l[jj] += log(pmp->Pc); /* fixed 02.03.98 DAK */
                for( i=0; i<pmp->N; i++ )
                    pmp->Fug_l[jj] += *(pmp->A+j*pmp->N+i) * pmp->U[i];
                if( pmp->Fug_l[jj] > -37. && pmp->Fug_l[jj] < 16. )
                    pmp->Fug[jj] = exp( (double)pmp->Fug_l[jj] );
                else  pmp->Fug[jj] = /* F_EMPTY*/ 0.0;
                /* Partial pressure ! */
                pmp->Ppg_l[jj] = pmp->Fug_l[jj] - pmp->lnGam[j]/* + log(pmp->Pc)*/;
                pmp->Fug_l[jj] *= .43429448;
                pmp->Ppg_l[jj] *= .43429448;
            }
            break;
        }
    }
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* Calculation of pH via activities of H2O and OH- */
/* Suggested by V.A.Sinitsyn, Apr 7, 1997 */
double TProfil::pH_via_hydroxyl( double x[], double Factor, int j)
{
    double lnaH;
    int jwa, jhy;
    jwa = j+1;
    jhy = j-1;                      /* Dangerous ! */
    lnaH = - pmp->G0[jhy] + 4.016534 + pmp->G0[jwa] + pmp->lnGam[jwa]
           - pmp->lnGam[jhy] - log( x[jhy]*Factor );
    x[j] = exp( lnaH - pmp->lnGam[j] ) / Factor;
    return (-lnaH *ln_to_lg);
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#define  a(j,i) (*(pmp->A+(i)+(j)*pmp->N))
/* Calculation of dual chemical potentials and concentrations
* for species (indexed jb to je) in a k-th phase.
* Input arrays X, XF, XFA,  input factors: Factor, MMC
* This function has to be rewritten using new set of built-in
* chemical functions.
*/
void TProfil::ConCalcDC( double X[], double XF[], double XFA[],
                         double Factor, double MMC, double Dsur, int jb, int je, int k)
{
    int j, ii;
    double Muj, DsurT, SPmol, lnFmol=4.016535;

    if( pmp->PHC[0] == PH_AQUEL )
    {  /* mole fraction to molality conversion */
        if( !k ) lnFmol = log(1000./MMC);  // aq species
        else lnFmol = 4.016535; 	   // other species
    }

    for( j=jb; j<je; j++ )
    { /* cycle by DC - important bugfixes 02.04.2003 */
        Muj = DualChemPot( pmp->U, pmp->A+j*pmp->N, pmp->NR );
        pmp->Fx[j] = Muj * pmp->RT;     /* el-chem potential in J/mole */

        if( X[j] <= pmp->lowPosNum )
        { /* zeroing off */
            pmp->Wx[j] = 0.0;
            pmp->VL[j] = 0.0;
            pmp->Y_w[j] = 0.0;
            pmp->lnGam[j] = 0.0;
            if( pmp->PHC[0] == PH_AQUEL )
               pmp->Y_m[j] = 0.0;
            switch( pmp->DCC[j] ) /* choice of expressions */
            {
               case DC_SCP_CONDEN:
                    pmp->Y_la[j] = ln_to_lg * ( Muj - pmp->G0[j] );
                    break;
               case DC_AQ_ELECTRON: case DC_AQ_PROTON:  case DC_AQ_SPECIES:
                    pmp->Y_la[j] = ln_to_lg*(Muj - pmp->G0[j]
                                        + Dsur + lnFmol);
                    break;
               case DC_AQ_SOLVENT: case DC_AQ_SOLVCOM:
                    pmp->Y_la[j] = ln_to_lg* (Muj - pmp->G0[j]
                                        + Dsur - 1. + 1. / ( 1.+Dsur ) );
                    break;
               case DC_GAS_COMP: case DC_GAS_H2O:  case DC_GAS_CO2:   /* gases */
               case DC_GAS_H2: case DC_GAS_N2:
                    pmp->Y_la[j] = ln_to_lg * ( Muj - pmp->G0[j] );
                    if( pmp->Pc > 1e-9 )
                        pmp->Y_la[j] += log10( pmp->Pc );
                    break;
               case DC_SOL_IDEAL: case DC_SOL_MINOR: case DC_SOL_MAJOR:
                    pmp->Y_la[j] = ln_to_lg * ( Muj - pmp->G0[j] );
                    break;
               case DC_SUR_GROUP:
                    DsurT = MMC * pmp->Aalp[k] * pa.p.DNS*1.66054e-6;
                    pmp->Y_la[j] = ln_to_lg * ( Muj - pmp->G0[j]
                                         + Dsur + DsurT/( 1.0+DsurT ) + lnFmol );
                    break;
               case DC_SSC_A0: case DC_SSC_A1: case DC_SSC_A2: case DC_SSC_A3:
               case DC_SSC_A4: case DC_WSC_A0: case DC_WSC_A1: case DC_WSC_A2:
               case DC_WSC_A3: case DC_WSC_A4: case DC_SUR_COMPLEX:
               case DC_SUR_IPAIR: case DC_IESC_A: case DC_IEWC_B:
                    DsurT = MMC * pmp->Aalp[k] * pa.p.DNS*1.66054e-6;
                    pmp->Y_la[j] = ln_to_lg * ( Muj - pmp->G0[j]
                                         + Dsur + DsurT/( 1.0+DsurT ) + lnFmol );
                    break; // Coulombic term to be considered !!!!!!!!!!
               case DC_PEL_CARRIER: case DC_SUR_MINAL: case DC_SUR_CARRIER: /* sorbent */
                    DsurT = MMC * pmp->Aalp[k] * pa.p.DNS*1.66054e-6;
                    pmp->Y_la[j] = ln_to_lg * ( Muj - pmp->G0[j]
                       + Dsur - 1. + 1./(1.+Dsur) - DsurT + DsurT/(1+DsurT) );
                    break;
               default:
                    break; /* error in DC class code */
            }
            continue;
        }
        /* calculation of mole fraction */
        pmp->Wx[j] = X[j]/XF[k];
        if( pmp->Wx[j] > pmp->lowPosNum )
            pmp->VL[j] = log( pmp->Wx[j] );
        pmp->Y_la[j] = 0.0;
        switch( pmp->DCC[j] ) /* choice of expressions */
        {
        case DC_SCP_CONDEN:
            pmp->Wx[j] = 1;
            pmp->VL[j] = 0.0;
            if( pmp->LO )
                pmp->Y_m[j] = X[j]*Factor; /* molality */
            else pmp->Y_m[j] = 0.0;
            pmp->Y_w[j] = /* mass % of the system */
                1e2 * X[j] * pmp->MM[j] / pmp->MBX;
            pmp->Y_la[j] = ln_to_lg * ( Muj - pmp->G0[j] );
            //  obsolete                pmp->Y_la[j] = ln_to_lg* pmp->lnGam[j];
            pmp->FVOL[k] += pmp->Vol[j]*X[j];
            break;
        case DC_AQ_ELECTRON: /* pE  SPmol = X[j]*Factor; */
            //                          pe_ = -ln_to_lg*( log( SPmol ) + pmp->lnGam[j]);
            pmp->Y_m[j] = 0.0;
            pmp->Y_la[j] = 0.0 - pmp->pe;  /* ln_a = -pe_ / .43429448; */
            pmp->Y_w[j] = 0.0;
            //                          pmp->IC_m[pmp->N-1] += SPmol;
            break;
        case DC_AQ_PROTON:
            pmp->pH = -ln_to_lg*(Muj-pmp->G0[j]+Dsur+lnFmol);
            /* obsolete DAK 11.01.00    if( X[j] < 3e-8 )
                                           pmp->pH = pH_via_hydroxyl( X, Factor, j );
                                        else pmp->pH = -ln_to_lg*(log( X[j]*Factor ) +
                                                     pmp->lnGam[j] );    */
        case DC_AQ_SPECIES:
            SPmol = X[j]*Factor;  /* molality */
            pmp->IC +=  /* calc effective molal ionic strength */
                0.5* SPmol *(pmp->EZ[j]*pmp->EZ[j]);

//    pmp->FVOL[k] += pmp->Vol[j]*SPmol;  Error - found by B.Lothenbach 03.02.03
          pmp->FVOL[k] += pmp->Vol[j]*X[j]; // fixed 04.02.03 KD
            pmp->Y_m[j] = SPmol;
            pmp->Y_la[j] = ln_to_lg*(Muj - pmp->G0[j]
                                        + Dsur + lnFmol); //    Variant: Without Dsur?
            // obsolete        pmp->Y_la[j] = ln_to_lg* (log( SPmol ) + pmp->lnGam[j] );
            pmp->Y_w[j] = 1e6 * X[j] * pmp->MM[j] / pmp->FWGT[k];
            for( ii=0; ii<pmp->NR; ii++ )
            {
                pmp->IC_m[ii] += SPmol* a(j,ii);
                pmp->IC_wm[ii] += X[j]* a(j,ii);  // moles of element in aq spec
            }
            break;
        case DC_AQ_SOLVENT: /* mol fractions in solvent */
        case DC_AQ_SOLVCOM:
            pmp->Y_m[j] = X[j]/XFA[k];
            pmp->Y_w[j] = 1e3*X[j]*pmp->MM[j]/pmp->FWGT[k];
            pmp->FVOL[k] += pmp->Vol[j]*X[j];
            pmp->Y_la[j] = ln_to_lg* (Muj - pmp->G0[j]
                                        + Dsur - 1. + 1. / ( 1.+Dsur ) );
            // Obsolete DAK 11.01.00   pmp->Y_la[j] = ln_to_lg* (log(pmp->Wx[j]) + pmp->lnGam[j] );
            break;
        case DC_GAS_COMP:
        case DC_GAS_H2O:
        case DC_GAS_CO2:   /* gases */
        case DC_GAS_H2:    /* volume */
        case DC_GAS_N2:
            pmp->FVOL[k] += pmp->Vol[j]*X[j];
            pmp->Y_la[j] = ln_to_lg * ( Muj - pmp->G0[j] );
            if( pmp->Pc > 1e-9 )
                pmp->Y_la[j] += log10( pmp->Pc );
            // Obsolete               pmp->Y_la[j] = ln_to_lg * (log(pmp->Wx[j]) + pmp->lnGam[j] );
            break;
        case DC_SOL_IDEAL:
        case DC_SOL_MINOR:   /* volume */
        case DC_SOL_MAJOR:
            pmp->FVOL[k] += pmp->Vol[j]*X[j];
            pmp->Y_la[j] = ln_to_lg * ( Muj - pmp->G0[j] );
            // Obsolete               pmp->Y_la[j] = ln_to_lg * (log(pmp->Wx[j]) + pmp->lnGam[j] );
            break;
            /* adsorption: Simplified by DAK 11.01.00 */
        case DC_SUR_GROUP:
            pmp->Y_m[j] = X[j]*Factor; /* molality */
            pmp->Y_w[j] =  /* mg/g sorbent */
                1e3 * X[j] * pmp->MM[j] / (MMC*XFA[k]);
            DsurT = MMC * pmp->Aalp[k] * pa.p.DNS*1.66054e-6;
            pmp->Y_la[j] = ln_to_lg * ( Muj - pmp->G0[j]
                                         + Dsur + DsurT/( 1.0+DsurT ) + lnFmol );
            break;
        case DC_SSC_A0:
        case DC_SSC_A1:
        case DC_SSC_A2:
        case DC_SSC_A3:
        case DC_SSC_A4:
        case DC_WSC_A0:
        case DC_WSC_A1:
        case DC_WSC_A2:
        case DC_WSC_A3:
        case DC_WSC_A4:  /* case DC_SUR_GROUP: */
        case DC_SUR_COMPLEX:
        case DC_SUR_IPAIR:
        case DC_IESC_A:
        case DC_IEWC_B:
            pmp->Y_m[j] = X[j]*Factor; /* molality */
            pmp->Y_w[j] =  /* mg/g sorbent */
                1e3 * X[j] * pmp->MM[j] / (MMC*XFA[k]);
            DsurT = MMC * pmp->Aalp[k] * pa.p.DNS*1.66054e-6;
            pmp->Y_la[j] = ln_to_lg * ( Muj - pmp->G0[j]
                                         + Dsur + DsurT/( 1.0+DsurT ) + lnFmol );
            break;
        case DC_PEL_CARRIER:
        case DC_SUR_MINAL:
        case DC_SUR_CARRIER: /* sorbent */
            pmp->Y_m[j] = X[j]*Factor; /* molality */
            pmp->Y_w[j] = 0.0;
            if(pmp->FWGT[0]>pmp->lowPosNum)
              pmp->Y_w[j] = /* mg of sorbent per kg aq solution */
                1e6 * X[j] * pmp->MM[j] / pmp->FWGT[0];
            DsurT = MMC * pmp->Aalp[k] * pa.p.DNS*1.66054e-6;
            pmp->Y_la[j] = ln_to_lg * ( Muj - pmp->G0[j]
                           + Dsur - 1. + 1./(1.+Dsur) - DsurT + DsurT/(1+DsurT) );
            // Obsolete         pmp->Y_la[j] = ln_to_lg * (log(pmp->Wx[j]) + pmp->lnGam[j] );
            pmp->FVOL[k] += pmp->Vol[j]*X[j];
            break;
        default:
            break; /* error in DC class code */
        }
        ;
    }   /* j */
}
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* Calculation of derived values (concentrations etc.) on IPM iteration
*  from X,XF, and XFA vectors. Also calculates pH, pe, Eh
* This function has to be rewritten using new set of built-in
* chemical functions.
*/
void TProfil::ConCalc( double X[], double XF[], double XFA[])
{
    int k, ii;
    int i, j, ist/*=0*/, jj/*=0*/, jja/*=0*/;
    double Factor=0.0, Dsur=0.0, MMC=0.0;
   // Kostya: debug calculating x from dual solution
      if( pmp->Ls < 2 || !pmp->FIs )
        return;


    for( j=0; j<pmp->Ls; j++ )
    {
        pmp->Wx[j] = 0.;
        pmp->VL[j] = 0.;
    }
    j=0;
    pmp->VXc = 0.0;
    for( k=0; k<pmp->FI; k++ )
    { /* cycle by phases */
        i=j+pmp->L1[k];
        pmp->FWGT[k] = 0.0;
        pmp->FVOL[k] = 0.0;
        //   Dsur = 0.0;

        if( k >= pmp->FIs || pmp->L1[k] == 1 )
        { /* this is a single- component phase */
            if( XF[k] < pmp->DSM )
            {
                //     pmp->Y_la[j] = 0.0;
                if( pmp->LO )
                    pmp->Y_m[j] = 0.0;
                pmp->Y_w[j] = 0.0;
                pmp->Fx[j] = DualChemPot( pmp->U, pmp->A+j*pmp->N, pmp->NR );
                pmp->Y_la[j] = ln_to_lg * ( pmp->Fx[j] - pmp->G0[j] /* + Dsur */ );
                pmp->Fx[j] *= pmp->RT;     /* el-chem potential */
                goto NEXT_PHASE;
            }
            pmp->Wx[j] = 1.0;
            pmp->VL[j] = 0.0;
            if( pmp->LO && XFA[0] > 0 )
                pmp->Y_m[j] = X[j] * 1000./18.01528/XFA[0]; /* molality */
            pmp->Y_w[j] = /* mass % in the system */
                1e2 * X[j] * pmp->MM[j] / pmp->MBX;
            pmp->Fx[j] = DualChemPot( pmp->U, pmp->A+j*pmp->N, pmp->NR );
            pmp->Y_la[j] = ln_to_lg * ( pmp->Fx[j] - pmp->G0[j] /* + Dsur */ );
            pmp->Fx[j] *= pmp->RT;     /* el-chem potential */
            //     pmp->Y_la[j] = ln_to_lg * pmp->lnGam[j];
            pmp->FWGT[k] += X[j] * pmp->MM[j];
            pmp->FVOL[k] += X[j] * pmp->Vol[j];
            goto NEXT_PHASE;
        }
        if( pmp->LO && !k )
            for( ii=0; ii<pmp->N; ii++ )
            {
                pmp->IC_m[ii] = 0.0;
                pmp->IC_lm[ii] = 0.0;
                pmp->IC_wm[ii] = 0.0;
            }

        if( XF[k] <= pmp->DSM ||
                (pmp->PHC[k] == PH_AQUEL && XFA[k] <= pa.p.XwMin )
                || ( pmp->PHC[k] == PH_SORPTION && XFA[k] <= pa.p.ScMin ))
        {
            memset( pmp->BF+k*pmp->N, 0, sizeof(double)*pmp->N );
            for(jj=j; jj<i; jj++)   // Loop added 10.03.01  KD (GTDEMO)
            {
                pmp->Wx[j] = 0.0;
                if( pmp->LO )
                    pmp->Y_m[jj] = 0.0;
                pmp->Y_w[jj] = 0.0;
                pmp->Fx[jj] = DualChemPot( pmp->U, pmp->A+jj*pmp->N, pmp->NR );
                pmp->Y_la[jj] = ln_to_lg * ( pmp->Fx[jj] - pmp->G0[jj] /* + Dsur */ );
                if(pmp->PHC[k] == PH_AQUEL || pmp->PHC[k] == PH_SORPTION )
                   pmp->Y_la[jj] += 1.74438;
                if(pmp->PHC[k] == PH_GASMIX || pmp->PHC[k] == PH_PLASMA )
                   pmp->Y_la[jj] += log10( pmp->Pc );
                pmp->Fx[jj] *= pmp->RT;     /* el-chem potential */
                pmp->lnGam[jj] = 0.0;
            }
            goto NEXT_PHASE;
        }
        /* calculate bulk stoichiometry of a multicomponent phase */
        phase_bcs( pmp->N, pmp->L1[k], pmp->A+j*pmp->N, pmp->X+j,
                   pmp->BF+k*pmp->N );

        switch( pmp->PHC[k] )
        {
        case PH_AQUEL:
            MMC = 0.0; /* mol mass of carrier */
            //                     Dsur = XF[k] - XFA[k];
            Dsur = XFA[k]/XF[k] - 1.0; // Asymm.corr. - aq only!
            if( XFA[k] > pmp->lowPosNum )
            {
                for(jj=j; jj<i; jj++)
                    if( pmp->DCC[jj] == DC_AQ_SOLVENT ||
                            pmp->DCC[jj] == DC_AQ_SOLVCOM )
                        MMC += pmp->MM[jj]*X[jj]/XFA[k];
            }
            else MMC=18.01528; // Assuming water-solvent
            if( (XFA[k] > pmp->lowPosNum) && (MMC > pmp->lowPosNum) )
                Factor = 1000./MMC/XFA[k]; /* molality */
            else Factor = 0.0;
            pmp->IC=0.; /* Factor = 0.5*55.508373/pmp->Yw; */
            pmp->pe = ln_to_lg* pmp->U[pmp->N-1];
            pmp->Eh = 0.000086 * pmp->U[pmp->N-1] * pmp->T;
        case PH_GASMIX:
        case PH_FLUID:
        case PH_PLASMA:
        case PH_SIMELT:
        case PH_HCARBL:
        case PH_SINCOND:
        case PH_SINDIS:
        case PH_LIQUID:
            pmp->YFk = XF[k];
            for(jj=j; jj<i; jj++)
            {
                if( X[jj] > pmp->lowPosNum)    // 1e-12 )
                    pmp->FWGT[k] += X[jj]*pmp->MM[jj];
            }
            break;
        case PH_POLYEL:
        case PH_SORPTION: /* only sorbent end-members! */
            pmp->YFk = XFA[k];
            MMC=0.0;

            for( ist=0; ist<pmp->FIat; ist++ )
                pmp->XFTS[k][ist] = 0.0;
            if( XFA[k] < pmp->lowPosNum ) XFA[k] = pmp->lowPosNum;
            for( jj=j; jj<i; jj++ )
            {
               jja = jj - ( pmp->Ls - pmp->Lads );
                if( pmp->DCC[jj] == DC_SUR_CARRIER ||
                        pmp->DCC[jj] == DC_SUR_MINAL ||
                        pmp->DCC[jj] == DC_PEL_CARRIER )
                {
                    MMC += pmp->MM[jj]*X[jj]/XFA[k];
                    /* Only sorbent mass */
                    pmp->FWGT[k] += X[jj]*pmp->MM[jj];
                }
                else
                {
                    ist = pmp->SATX[jja][0] / MSPN;
                    pmp->XFTS[k][ist] += X[jj];
                }
            }
            pmp->logYFk = log(pmp->YFk);
            Dsur = XFA[k]/XF[k] - 1.0;  // Also for sorption phases
            if( Dsur <= -1.0 ) Dsur = -0.999999; // Debugging!!!!!
// double DsurTT=0.0;  //  Temporarily!
// DsurTT = XFA[k] * MMC * pmp->Aalp[k] * pa.p.DNS*1.66054e-6;
// Dsur = DsurTT / XF[k] - 1.0;
            break;
        default:

            return; /* Phase class code error! */
        }
        /* cycle for species concentrations in a phase */
        ConCalcDC( X, XF, XFA, Factor, MMC, Dsur, j, i, k );

NEXT_PHASE:
        pmp->VXc += pmp->FVOL[k];
        if( pmp->PHC[k] == PH_AQUEL && XFA[k] > pa.p.XwMin )
            for( ii=0; ii<pmp->NR; ii++ )
            {
               if( pmp->LO  ) // Sveta 30/08/01
               { if( pmp->IC_m[ii] >= pa.p.DB )
                    pmp->IC_lm[ii] = ln_to_lg*log( pmp->IC_m[ii] );
                else
                    pmp->IC_lm[ii] = 0;
                if( pmp->FWGT[k] >= pa.p.DB )
                    pmp->IC_wm[ii] *= pmp->Awt[ii]*1000./pmp->FWGT[k];
                else
                    pmp->IC_wm[ii] = 0;
               }
            }
        j = i;
    }  /* k */

}

//#define mp(i,j) (float)pmp->MASDJ[(j)+(i)*D_F_CD_NP]
// Added by KD on 24.10.2004 for implementation of CD-MUSIC, Frumkin etc.

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* Multi-surface (ad)sorption phases   Brooklyn 5/15/97 DAK */
/* Updated Mainz 5 Jan 2000 Mainz by DAK */
/* Calculation of surface charge */
void TProfil::IS_EtaCalc()
{
    int k, i, ist/*=0*/, isp/*=0*/, j=0, ja/*=0*/;
    double XetaS=0., XetaW=0.,  Ez, CD0, CDb, ObS;
    for( k=0; k<pmp->FIs; k++ )
    { /*cycle over phases */
        i=j+pmp->L1[k];
        if( pmp->FIat > 0 )
            for( ist=0; ist<pmp->FIat; ist++ )
            {
                pmp->XetaA[k][ist] = 0.0;
                pmp->XetaB[k][ist] = 0.0;
            }

        if( pmp->XF[k] <= pmp->DSM ||
                pmp->PHC[k] == PH_AQUEL && pmp->X[pmp->LO] <= pa.p.XwMin
                || pmp->PHC[k] == PH_SORPTION && pmp->XF[k] <= pa.p.ScMin )
            goto NEXT_PHASE;

        switch( pmp->PHC[k] )
        {  /* initialization according to phase class */
        case PH_AQUEL:  /* aqueous solution - sum charch */
            pmp->Yw = pmp->XFA[k];
            XetaW = 0.0;
            break;
        case PH_PLASMA:
        case PH_SIMELT:
            XetaS = 0.0;
            break;
        case PH_POLYEL:
        case PH_SORPTION: /*calc charge density later on */
            break;
        default:
            break;
        }
        for( ; j<i; j++ )
        { /* cycle by DC for calculating total phase charge */
            if( pmp->X[j] <= pmp->lowPosNum*100. )
                continue; /* Skipping too low concentrations */
            ja = j - ( pmp->Ls - pmp->Lads );

            switch( pmp->DCC[j] ) /* select expressions for species classes */
            {
            case DC_AQ_ELECTRON:
            case DC_AQ_PROTON:
            case DC_AQ_SPECIES:
                XetaW += pmp->X[j]*pmp->EZ[j];
            case DC_AQ_SOLVCOM:
            case DC_AQ_SOLVENT:
                break;
            case DC_PEL_CARRIER:
            case DC_SUR_MINAL:
            case DC_SUR_CARRIER: /*charge of carrier: ???????
                                             pmp->XetaA[k] += pmp->X[j]*pmp->EZ[j]; */
                break;
                /* surface species */
            case DC_SSC_A0:
            case DC_SSC_A1:
            case DC_SSC_A2:
            case DC_SSC_A3:
            case DC_SSC_A4:
            case DC_WSC_A0:
            case DC_WSC_A1:
            case DC_WSC_A2:
            case DC_WSC_A3:
            case DC_WSC_A4:
            case DC_SUR_GROUP:
            case DC_SUR_COMPLEX:
            case DC_SUR_IPAIR:
            case DC_IESC_A:
            case DC_IEWC_B: // Get ist - index of surface type
                            // and isp - index of surface plane
                ist = pmp->SATX[ja][XL_ST] / MSPN;
                isp = pmp->SATX[ja][XL_ST] % MSPN;
                            // Get charge distribution information
// From now on, the mp(j,PI_DEN) macro replaces pmp->MASDJ[j]
                CD0 = pmp->MASDJ[ja][PI_CD0];  // species charge that goes into 0 plane
                CDb = pmp->MASDJ[ja][PI_CDB];  // species charge that goes into B plane
                ObS = pmp->MASDJ[ja][PI_DEN];  // obsolete - the sign for outer-sphere charge
                if( ObS >= 0.0 )
                   ObS = 1.0;
                else ObS = -1.0;
                Ez = pmp->EZ[j];

                if( !isp )
                { // This is the 0 (A) plane
                    if( fabs( CD0 ) > 1e-20 )  // Doubtful...
                       Ez = CD0;
                    pmp->XetaA[k][ist] += pmp->X[j]*Ez;
                }
                else
                { /* This is the B plane */
//                    Ez = pmp->EZ[j];
                    if( pmp->SCM[k][ist] == SC_MTL )
                    { /* Modified TL: Robertson, 1997; also XTLM Kulik 2002 */
                        if( fabs( CDb ) > 1e-20 )  // Doubtful...
                           Ez = CDb;
                        pmp->XetaB[k][ist] += pmp->X[j]*Ez;
                    }
                    else if( pmp->SCM[k][ist] == SC_TLM )
                    {
// New CD version of TLM  added 25.10.2004
                        if( fabs( CD0 ) > 1e-20 && fabs( CDb ) > 1e-20 )
                        {
                           pmp->XetaB[k][ist] += pmp->X[j] * CDb;
                           pmp->XetaA[k][ist] += pmp->X[j] * CD0;
                        }
// Old version:  TLM Hayes & Leckie, 1987 uses the sign indicator at density
                        else {
                          if( ObS < 0.0 )
                          {
                            Ez -= 1.0;
                            pmp->XetaB[k][ist] += pmp->X[j] * Ez;
                            pmp->XetaA[k][ist] += pmp->X[j];
                          }
                          else
                          {
                            Ez += 1.0;
                            pmp->XetaB[k][ist] += pmp->X[j] * Ez;
                            pmp->XetaA[k][ist] -= pmp->X[j];
                          }
                        }
                    }
                    else if( pmp->SCM[k][ist] == SC_BSM )
                    { // Basic Stern model Christl & Kretzschmar, 1999
// New CD version of BSM  added 25.10.2004
                        if( fabs( CD0 ) > 1e-20 && fabs( CDb ) > 1e-20 )
                        {
                           pmp->XetaB[k][ist] += pmp->X[j] * CDb;
                           pmp->XetaA[k][ist] += pmp->X[j] * CD0;
                        }
// Old version: uses the sign indicator at density
                        else {
                          if( ObS < 0.0 )
                          {
                            Ez -= 1.0;
                            pmp->XetaB[k][ist] += pmp->X[j] * Ez;
                            pmp->XetaA[k][ist] += pmp->X[j];
                          }
                          else
                          {
                            Ez += 1.0;
                            pmp->XetaB[k][ist] += pmp->X[j] * Ez;
                            pmp->XetaA[k][ist] -= pmp->X[j];
                          }
                        }
                    }
                    else if( pmp->SCM[k][ist] == SC_MXC )
                    { /* BSM for ion exchange on perm.charge surface */
                        if( fabs( CDb ) > 1e-20 )  // Doubtful...
                           Ez = CDb;
                        pmp->XetaB[k][ist] += pmp->X[j]*Ez;
                        if( fabs( CD0 ) > 1e-20 )
                            pmp->XetaA[k][ist] += pmp->X[j]*CD0;  // experimental
                    }
                    else if( pmp->SCM[k][ist] == SC_CCM )
                    { // Added 25.07.03 to implement the extended CCM Nilsson ea 1996
// New CD version of BSM  added 25.10.2004
                        if( fabs( CD0 ) > 1e-20 && fabs( CDb ) > 1e-20 )
                        {
                           pmp->XetaB[k][ist] += pmp->X[j] * CDb;
                           pmp->XetaA[k][ist] += pmp->X[j] * CD0;
                        }
// Old version: uses the sign indicator at density
                        else {
                          if( ObS < 0.0 )
                          {
                            Ez -= 1.0;
                            pmp->XetaB[k][ist] += pmp->X[j] * Ez;
                            pmp->XetaA[k][ist] += pmp->X[j];
                          }
                          else
                          {
                            Ez += 1.0;
                            pmp->XetaB[k][ist] += pmp->X[j] * Ez;
                            pmp->XetaA[k][ist] -= pmp->X[j];
                          }
                        }
                    }
                 /*    case DC_SUR_DL_ION:  XetaS += pmp->X[j]*pmp->EZ[j];  */
                }
                break;
            default:
                XetaS += pmp->X[j]*pmp->EZ[j];
                break;
            }
        }   /* j */
        /* compare pmp->Xetaf[k]+pmp->XetaA[k]+pmp->XetaB[k] and XetaS */
        /* Test XetaW */
NEXT_PHASE:
        j = i;
        if( pmp->LO && !k && pmp->FIat > 0 )
        {
            pmp->XetaA[k][0] = XetaW;
            pmp->XetaB[k][0] = XetaW;
        }
        if( (pmp->PHC[k] == PH_PLASMA || pmp->PHC[k] == PH_SIMELT)
                && pmp->FIat)
            pmp->XetaA[k][0] = XetaS;
    }  /* k */
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Calculation of the surface potential pm[q].XpsiD[k] on diffuse
*  layer plane on k-th sorption phase from total charge pmp->Xeta[k]
* ( in moles ) using Gouy-Chapman equation.
* Strictly valid at PSI < 30 mV. Modified by DAK 5 Jan 2000
* to add a Basic Stern EDL model.
*/
void TProfil::GouyChapman(  int /*jb*/, int /*je*/, int k )
{
    int ist;
    double SigA=0., SigD=0., SigB=0., XetaA[MST], XetaB[MST], f1, f3;
    double A/*=1e-9*/, Sig, F2RT, I, Cap;
    /* Del, F=F_CONSTANT, Cap0; */
    if( pmp->XF[k] < pa.p.ScMin )
        return; /* no sorbent */

    /* sorbent mass in grams */
    pmp->YFk = pmp->FWGT[k];
    if(pmp->YFk < pmp->lowPosNum*100.)
       pmp->YFk = pmp->lowPosNum*100.;

    for( ist=0; ist<pmp->FIat; ist++ ) /* cycle by surface types */
    {
        double PsiD, PSIo, PsiA, PsiB;

        if( pmp->SCM[k][ist] == SC_NOT_USED || pmp->Nfsp[k][ist] < 1e-9  )
            continue;
        /* Calculation of charge densities */
        if( fabs( pmp->XetaA[k][ist]) > pmp->lowPosNum*100. )
            XetaA[ist] = pmp->XetaA[k][ist]*F_CONSTANT/pmp->YFk/pmp->Aalp[k]
                         /pmp->Nfsp[k][ist]; /* C/m2 */
        else XetaA[ist] = 0.0;
        if( fabs( pmp->XetaB[k][ist]) > pmp->lowPosNum*100. )/* moles */
            XetaB[ist] = pmp->XetaB[k][ist] *F_CONSTANT/pmp->YFk/pmp->Aalp[k]
                         /pmp->Nfsp[k][ist]; /* C/m2 */
        else XetaB[ist] = 0.0;
        /* Limit charge densities to 0.7 C/m2 */
        if( fabs(XetaA[ist]) > 1.4 )
        {
// cout << "EDL charge density A " << XetaA[ist] << " truncated to +- 0.7 C/m2" <<
//        "  IT= " << pmp->IT << " k= " << k << " ist= " << ist << endl;
            XetaA[ist] = XetaA[ist] < 0.0 ? -1.4: 1.4;
        }
        if( fabs(XetaB[ist]) > 2.0 )
        {
// cout << "EDL charge density B " << XetaB[ist] << " truncated to +- 1.7 C/m2" <<
//        "  IT= " << pmp->IT << " k= " << k << " ist= " << ist << endl;
            XetaB[ist] = XetaB[ist] < 0.0 ? -2.0: 2.0;
        }
        if( fabs( XetaA[ist] ) < pmp->lowPosNum*1e6 )
            goto GEMU_CALC;  /* skipping at near-zero charge */

        /* calculating charge density at diffuse layer */
        switch( pmp->SCM[k][ist] )
        {
        case SC_CCM:  /* Constant-Capacitance Model Schindler ext. Nilsson */
            SigA = pmp->Xetaf[k][ist] + XetaA[ist];
//            SigD = -SigA;
            SigD = -SigA - XetaB[ist];
            SigB = XetaB[ist];
            break;
        case SC_DDLM: /* Diff. Double Layer Model Dzombak and Morel, 1990 */
            SigA = pmp->Xetaf[k][ist] + XetaA[ist];
            SigD = -SigA;
            break;
        case SC_TLM:  /* Triple-Layer Model Hayes and Leckie, 1987 */
            SigA = pmp->Xetaf[k][ist] + XetaA[ist];
            SigD = -SigA - XetaB[ist];
            break;
        case SC_MTL:  /* Modified Triple-Layer Model Robertson, 1997 */
            SigA = pmp->Xetaf[k][ist] + XetaA[ist];
            SigD = -SigA - XetaB[ist];
            break;
        case SC_BSM: /* Basic Stern model: Christl and Kretzschmar, 1999 */
            SigA = pmp->Xetaf[k][ist] + XetaA[ist];
            SigD = -SigA - XetaB[ist];
            break;
        case SC_MXC:  /* BSM for Ion-Exchange on perm.charge surface */
            SigA = pmp->Xetaf[k][ist] + XetaA[ist];
            SigD = -SigA - XetaB[ist];
            break;
        case SC_NNE:  /* Non-Electrostatic Sorption */
            SigA = 0;
            SigD = 0;
            break;
        default:
            continue;
        }
//        if( fabs( SigD ) > 1 )
//        {
//cout << "EDL charge density D " << SigD << " truncated to +- 1 C/m2" <<
//        "  IT= " << pmp->IT << " k= " << k << " ist= " << ist << endl;
//            SigD = SigD < 0.0 ? -1.0: 1.0;
//        }
        /* Gouy-Chapman equation */
        /* params of diffuse layer using Damaskin, 1987,p.192-195 */
        A = 1e-9;
        F2RT = pmp->FRT / 2.;
        Sig = SigD;
        I=pmp->IC;
        if( I > 1e-7 )
            /* Aq solution density Ro included acc. to Machesky ea., 1999 */
            /* Only for the basic Stern model for now (Jan 5, 2000 DAK)! */
        {
            double Ro = 1.0;
            if( pmp->SCM[k][ist] == SC_BSM && pmp->FVOL[0] > 1e-16 )
                Ro = pmp->FWGT[0] / pmp->FVOL[0];
            A = sqrt( 2000. * 8.854e-12 * pmp->epsW * pmp->RT * I * Ro );
        }
        Cap = F2RT * sqrt( 4.*A*A + Sig*Sig );

        /* Sveta: workaround because of problems with log argument */
        f3 =  sqrt( 1.+Sig*Sig/(4.*A*A) ) - Sig/(2.*A);
//cout<< f1 << ' '<< f2 << ' ' << f3 << endl;
        if( f3 < 1 )
        {
            f1 = exp( -3. * F2RT );
            if( f3<f1) f3 = f1;
        }
        else
        {
            f1 = exp( 3. * F2RT );
            if( f3>f1 ) f3 = f1;
        }
        PSIo = log(f3)/F2RT;
//          PSIo = log( sqrt( 1.+Sig*Sig/(4.*A*A) ) - Sig/(2.*A) ) / F2RT;
//          Cap0 = fabs(Sig/PSIo);
//          Del = A*1e9/(2.*I*F)/cosh(PSIo*F2RT);
//          pmp->XdlD[k] = Del;
        pmp->XcapD[k][ist] = Cap;
        pmp->XpsiD[k][ist] = PSIo;
        PsiD = PSIo; /* PsiA = PsiB = 0; */
        if( fabs( PsiD ) > 0.4 )
        {
// cout << "All EDL models: PsiD = " << PsiD << " truncated to +- 0.4 V" <<
//      "  IT= " << pmp->IT << " k= " << k << " ist= " << ist << endl;
                PsiD = PsiD<0? -0.4: 0.4;
        }
GEMU_CALC:
        /* calculating potentials at EDL planes */
        switch( pmp->SCM[k][ist] )
        {
        case SC_DDLM: /* Diffuse Double Layer Model  Dzombak & Morel 1990 */
            pmp->XpsiA[k][ist] = PsiD;
            pmp->XpsiB[k][ist] = PsiD;
            break;
        case SC_CCM:  /* Constant-Capacitance Model Schindler, ext. Nilsson */
            if( pmp->XcapB[k][ist] > 0.001 )
            {  // Classic CCM Schindler with inner-sphere species only
               PsiA = SigA / pmp->XcapA[k][ist];
               if( fabs( PsiA ) > 0.7 )
               {
                   PsiA = PsiA<0? -0.7: 0.7;
               }
               pmp->XpsiA[k][ist] = PsiA;
            }
            else { // Extended CCM model Nilsson ea 1996 as TLM with PsiD = 0
               PsiB = - SigB / pmp->XcapB[k][ist];
               if( fabs( PsiB ) > 0.3 )  /* Cutoff potential */
               {
                   PsiB = PsiB<0? -0.3: 0.3;
               }
               PsiA = PsiB + SigA / pmp->XcapA[k][ist];
               if( fabs( PsiA ) > 0.7 )
               {
                  PsiA = PsiA<0? -0.7: 0.7;
               }
               pmp->XpsiA[k][ist] = PsiA;
               pmp->XpsiB[k][ist] = PsiB;
            }
            break;
        case SC_MTL:  /* Modified Triple Layer Model for X- Robertson | Kulik */
// PsiD = 0.0; // test
            PsiB = PsiD - SigD / pmp->XcapB[k][ist];
            if( fabs( PsiB ) > 0.6)  /* Cutoff potential */
            {
// cout << "EDL (MTL) PsiB = " << PsiB << " truncated to +- 0.3 V" <<
//      "  IT= " << pmp->IT << " k= " << k << " ist= " << ist << endl;
                PsiB = PsiB<0? -0.6: 0.6;
            }
            PsiA = PsiB + SigA / pmp->XcapA[k][ist];
            if( fabs( PsiA ) > 1.1 )
            {
// cout << "EDL (MTL) PsiA = " << PsiA << " truncated to +- 1.0 V" <<
//      "  IT= " << pmp->IT << " k= " << k << " ist= " << ist << endl;
                PsiA = PsiA<0? -1.1: 1.1;
            }
            pmp->XpsiA[k][ist] = PsiA;
            pmp->XpsiB[k][ist] = PsiB;
            break;
        case SC_TLM:  /* Triple-Layer Model   Hayes 1987 */
            PsiB = PsiD - SigD / pmp->XcapB[k][ist];
            if( fabs( PsiB ) > 0.6 )  /* Cutoff potential */
            {
// cout << "EDL (TLM) PsiB = " << PsiB << " truncated to +- 0.3 V" <<
//      "  IT= " << pmp->IT << " k= " << k << " ist= " << ist << endl;
                PsiB = PsiB<0? -0.6: 0.6;
            }
            PsiA = PsiB + SigA / pmp->XcapA[k][ist];
            if( fabs( PsiA ) > 1.1 )
            {
// cout << "EDL (TLM) PsiA = " << PsiA << " truncated to +- 0.7 V" <<
//      "  IT= " << pmp->IT << " k= " << k << " ist= " << ist << endl;
                PsiA = PsiA<0? -1.1: 1.1;
            }
            pmp->XpsiA[k][ist] = PsiA;
            pmp->XpsiB[k][ist] = PsiB;
            break;
        case SC_BSM: /* Basic Stern model, Christl & Kretzschmar, 1999 */
            PsiB = PsiD;
            if( fabs( PsiB ) > 0.6 )  /* Cutoff potential */
            {
//cout << "EDL (BSM) PsiB = " << PsiB << " truncated to +- 0.6 V" <<
//      "  IT= " << pmp->IT << " k= " << k << " ist= " << ist << endl;
                PsiB = PsiB<0? -0.6: 0.6;
            }
            PsiA = PsiB + SigA / pmp->XcapA[k][ist];
            if( fabs( PsiA ) > 1.1 )
            {
//cout << "EDL (BSM) PsiA = " << PsiA << " truncated to +- 1.0 V" <<
//      "  IT= " << pmp->IT << " k= " << k << " ist= " << ist << endl;
                PsiA = PsiA<0? -1.1: 1.1;
            }
            pmp->XpsiA[k][ist] = PsiA;
            pmp->XpsiB[k][ist] = PsiB;
            break;
        case SC_MXC:  /* BSM for const.charge surfaces */
            PsiB = PsiD;
            if( fabs( PsiB ) > 0.6 )  /* Cutoff potential */
            {
// cout << "EDL (MXC) PsiB = " << PsiB << " truncated to +- 0.6 V" <<
//      "  IT= " << pmp->IT << " k= " << k << " ist= " << ist << endl;
                PsiB = PsiB<0? -0.6: 0.6;
            }
            PsiA = PsiB + SigA / pmp->XcapA[k][ist];
            if( fabs( PsiA ) > 1.1 )
            {
// cout << "EDL (MXC) PsiA = " << PsiA << " truncated to +- 1.0 V" <<
//      "  IT= " << pmp->IT << " k= " << k << " ist= " << ist << endl;
                PsiA = PsiA<0? -1.1: 1.1;
            }
            pmp->XpsiA[k][ist] = PsiA;
            pmp->XpsiB[k][ist] = PsiB;
            break;
        case SC_NNE:  /* Non-Electrostatic Sorption */
            pmp->XpsiA[k][ist] = 0.0;
            pmp->XpsiB[k][ist] = 0.0;
            break;
        default:
            break;
        }
    }  // ist    surf. types
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* With new calculation of surface activity coeff terms (2004)
*  Retains old (obsolete) SAT calculations (Kulik 2000, 2002)
*  Revised by KD in April 2004 (PSI) to introduce new activity
*  coefficient terms SACT rigorously derived from Langmuir and QCA
*  isotherms (Kulik 2004, to be submitted).
*  SACT are placed into pmp->lnGam[j], as any activity coeffs.
*  pmp->lnSAC[*][3] vector is now used to keep original DUL[j] to restore
*  them after IPM-2 refinements for surface complexes.
*/
void TProfil::SurfaceActivityCoeff( int jb, int je, int /*jpb*/, int /*jdb*/, int k )
{
    int i, ii, j, ja/*=0*/, ist/*=0*/, iss/*=0*/, dent/*=1*/, Cj/*=0*/, iSite[6];
    double XS0,  xj0, XVk, XSk, XSkC/*=0.*/, xj/*=0.*/, Mm/*=0.*/,
             rIEPS, ISAT, /*SAT,*/ XSs/*=0.*/,
             /*bet,*/ SATst/*=1.0*/, xjn/*=0.*/, q1, q2, Fi/*=0.*/, cN/*=1.*/, eF/*=0.*/;

    if( pmp->XFA[k] <= pmp->DSM ) /* No sorbent retained by the IPM */
        return;
    if( pmp->XF[k]-pmp->XFA[k] < pmp->lowPosNum /* *10. */ )
        return;  /* No surface species */

    for(i=0; i<MST; i++)
    {
        iSite[i] = -1;
        for( ii=0; ii<MST; ii++ )
           pmp->D[i][ii] = 0.0;  // cleaning the matrix for sites totals
    }
    /* Extraction of site indices for the neutral >OH group */
    for( j=jb; j<je; j++ )
    {
        ja = j - ( pmp->Ls - pmp->Lads );
        if( pmp->SATT[ja] == SAT_SOLV )
        {
           ist = pmp->SATX[ja][XL_ST] / MSPN;
           iSite[ist] = j;
        }
        // Counting current sites totals
        if( pmp->DCC[j] == DC_SUR_CARRIER ||
            pmp->DCC[j] == DC_SUR_MINAL ||
            pmp->DCC[j] == DC_PEL_CARRIER ||
            pmp->SATT[ja] == SAT_SOLV )
            continue;
        // Calculate ist - index of surface type
        ist = pmp->SATX[ja][XL_ST] / MSPN;
        if( ist < 0 || ist >= MST )
            ist = 0;  // default: zero surface type
        // Calculate iss - index of site on surf.type
        iss = pmp->SATX[ja][XL_SI];
        if( iss < 0 || iss >= MST )
            iss = 0;  // default: zero site is the weekest and the most abundant one
        pmp->D[iss][ist] += pmp->X[j]; // adding to total amount on a site
    }

    for( j=jb; j<je; j++ )
    { // Main loop for DC surface complexes
        if( pmp->X[j] <= pmp->lowPosNum /* *10. */ )
            continue;  /* This surface DC has been killed by IPM */
//        OSAT = pmp->lnGmo[j]; // added 6.07.01 by KDA
        ja = j - ( pmp->Ls - pmp->Lads );
        rIEPS = pa.p.IEPS;   // between 1e-8 and 1e-10 default 1e-9
//        dent = 1;  // default - monodentate
        switch( pmp->DCC[j] )  /* code of species class */
        {
        default: /* pmp->lnGam[j] = 0.0; */
            continue;
        case DC_SSC_A0:
        case DC_SSC_A1:
        case DC_SSC_A2:
        case DC_SSC_A3:
        case DC_SSC_A4:
        case DC_WSC_A0:
        case DC_WSC_A1:
        case DC_WSC_A2:
        case DC_WSC_A3:
        case DC_WSC_A4:
        case DC_SUR_GROUP:
        case DC_IEWC_B:
        case DC_SUR_COMPLEX:
        case DC_SUR_IPAIR:
        case DC_IESC_A:
            /* Calculate ist - index of surface type */
            ist = pmp->SATX[ja][XL_ST] / MSPN;
            /* Calculate iss - index of site on surf.type */
            iss = pmp->SATX[ja][XL_SI];
            if( iss < 0 || iss >= MST )
              iss = 0;  // zero site is the weekest and the most abundant one
            /* Cj - index of carrier DC */
            Cj = pmp->SATX[ja][XL_EM];
            if( Cj < 0 )
            {  /* Assigned to the whole sorbent */
                XVk = pmp->XFA[k];
                Mm = pmp->FWGT[k] / XVk;
            }
            else
            { /* Assigned to one of the sorbent end-members */
                XVk = pmp->X[Cj];
                if( XVk < pmp->DSM/10.0 )
                    continue; /* This end-member is zeroed off by IPM */
                Mm = pmp->MM[Cj] * XVk/pmp->XFA[k];  // mol.mass
            }
            XSk = pmp->XFTS[k][ist]; // Tot.moles of sorbates on surf.type
  XSs = pmp->D[iss][ist]; // tot.moles of SC on site type
            xj = pmp->X[j];  // Current moles of this surf.species

// isotherm parameters
            if( pmp->MASDJ )
            {
               cN = pmp->MASDJ[ja][PI_P2];  // Frumkin/Pivovarov water coord. number
               dent = (int)cN;              // dentateness for L and QCA isoterms
               Fi = pmp->MASDJ[ja][PI_P1];  // Frumkin lateral interaction energy term
            //   bet = pmp->MASDJ[ja][PI_P3]; // BET beta parameter
            }
            else {
               cN = 0.0; Fi = 0.0; /*bet = 1.0;*/ dent = 1;
            }
            switch( pmp->SATT[ja] ) // selection of the activity coeff. model
            {
            case SAT_L_COMP: // Competitive monodent. Langmuir on a surface and site type
                XSkC = XSs / XVk / Mm * 1e6
                   /pmp->Nfsp[k][ist]/ pmp->Aalp[k]/1.66054;  // per nm2        check this!!!!!!!
                XS0 = fabs(pmp->MASDJ[ja][PI_DEN])/pmp->Aalp[k]/1.66054; // max.dens.per nm2
/*            XS0 = pmp->MASDT[k][ist] * XVk * Mm / 1e6
                      * pmp->Nfsp[k][ist]; expected total sites in moles */
                if( pa.p.PC == 1 )
                    rIEPS = pa.p.IEPS * XS0;  // relative IEPS
                if( XSkC < 0.0 )
                    XSkC = 0.0;
                if( XSkC >= XS0 )  // Limits
                    XSkC = XS0 - 2.0 * rIEPS;
                q1 = XS0 - XSkC;
                if( pa.p.PC == 2 && !pmp->W1 || pa.p.PC != 2 )
                {
                  q2 = rIEPS * XS0;
                  if( q1 > q2 )
                      q2 = q1;
                }
                else {
                   q2 = q1;
                   if( q2 <= 1e-22 )
                       q2 = 1e-22;
                }
                ISAT = log( XS0 ) - log( q2 );
                pmp->lnGam[j] = ISAT;
pmp->lnSAC[ja][0] = ISAT;
                break;
    // (Non)competitive QCA-L for 1 to 4 dentate species
//            case SAT_QCA4_NCOMP: dent++;     /* code '4' */
//            case SAT_QCA3_NCOMP: dent++;     /* code '3' */
//            case SAT_QCA2_NCOMP:             /* code '2' */
            case SAT_QCA_NCOMP:  // dent++;   bidentate is default for QCA
//            case SAT_QCA1_NCOMP:             /* code '1' */
                xj0 = fabs(pmp->MASDJ[ja][PI_DEN])/pmp->Aalp[k]/1.66054;
                                             // Max site density per nm2
/*              xj0 = fabs(pmp->MASDJ[j]) * XVk * Mm / 1e6
                      * pmp->Nfsp[k][ist];     in moles */
                xj = XSs / XVk / Mm / pmp->Nfsp[k][ist] * 1e6     // xj
                     /pmp->Aalp[k]/1.66054; // Density per nm2 on site type iss
                if( pa.p.PC == 1 )
                    rIEPS = pa.p.IEPS * xj0; // relative IEPS
                if(xj >= xj0/(double)dent)
                     xj = xj0/(double)dent - rIEPS;  // upper limit
//                ISAT = 0.0;
                q2 = xj0 - xj*dent;  // Computing differences in QCA gamma
                q1 = xj0 - xj;
                if( q1 < 1e-22 )
                    q1 = 1e-22;
                if( q2 < 1e-22 )
                    q2 = 1e-22;
                ISAT = log(xj0) + log(q1)*(dent-1) - log(q2)*dent;
//                if( pa.p.PC == 2 && pmp->W1 )
//                      ISAT = log( xj ) - log( q1 );
                pmp->lnGam[j] = ISAT;
pmp->lnSAC[ja][0] = ISAT;
                break;
            case SAT_FRUM_COMP:
                dent = 1; // monodentate for now
                xj0 = fabs(pmp->MASDJ[ja][PI_DEN])/pmp->Aalp[k]/1.66054;
                                             // Max site density per nm2
                xj = XSs / XVk / Mm / pmp->Nfsp[k][ist] * 1e6  //  xj
                     /pmp->Aalp[k]/1.66054; // Current density per nm2
                if( pa.p.PC == 1 )
                    rIEPS = pa.p.IEPS * xj0; // relative IEPS
                if(xj >= xj0/dent)
                     xj = xj0/dent - rIEPS;  // upper limit
                q2 = xj0 - xj*dent;  // Computing differences in QCA gamma
                q1 = xj0 - xj;
                if( q1 < 1e-22 )
                    q1 = 1e-22;
                if( q2 < 1e-22 )
                    q2 = 1e-22;
                ISAT = log(xj0) + log(q1)*(dent-1) - log(q2)*dent;
            // Calculation of the Frumkin exponential term
                if( fabs (Fi) < 1e-9 || fabs (cN) < 1e-9 )
                   eF = 0.0;
                else {
                   eF = cN * Fi * xj*dent / xj0 ;  // Fi = Fi'/(kT) Bockris p.938
                }
                pmp->lnGam[j] = ISAT + eF;
pmp->lnSAC[ja][0] = ISAT;
pmp->lnSAC[ja][1] = eF;
                break;
            case SAT_FRUM_NCOMP: /* (Non)competitive Frumkin isotherm for perm.charge surfaces */
                XSkC = xj / XVk / Mm / pmp->Nfsp[k][ist] * 1e6
                       / pmp->Aalp[k]/1.66054;  // per nm2
                XS0 = pmp->MASDJ[ja][PI_DEN]/pmp->Aalp[k]/1.66054; /* max.dens.per nm2 */
/*            XS0 = pmp->MASDT[k][ist] * XVk * Mm / 1e6
                      * pmp->Nfsp[k][ist]; expected total sites in moles */
                if( pa.p.PC == 1 )
                    rIEPS = pa.p.IEPS * XS0;  // relative IEPS
                if( XSkC < 0.0 )
                    XSkC = 0.0;
                if( XSkC >= XS0 )  // Limits
                    XSkC = XS0 - 2.0 * rIEPS;
                q1 = XS0 - XSkC;
                if( pa.p.PC == 2 && !pmp->W1 || pa.p.PC != 2 )
                {
                  q2 = rIEPS * XS0;
                  if( q1 > q2 )
                      q2 = q1;
                }
                else {
                   q2 = q1;
                   if( q2 <= 1e-22 )
                       q2 = 1e-22;
                }
                ISAT = log( XS0 ) - log( q2 );
            // Calculation of the Frumkin exponential term (competitive)
                if( fabs (Fi) < 1e-9 || fabs (cN) < 1e-9 )
                   eF = 0.0;
                else {
                   eF = cN * Fi * XSkC / XS0;  // Fi = Fi'/(kT) Bockris p.938
                }
                pmp->lnGam[j] = ISAT + eF;
pmp->lnSAC[ja][0] = ISAT;
pmp->lnSAC[ja][1] = eF;
                break;
            case SAT_PIVO_NCOMP: /* (Non)competitive Pivovarov isotherm */
                dent = 1; // monodentate for now
                xj0 = fabs(pmp->MASDJ[ja][PI_DEN])/pmp->Aalp[k]/1.66054;
                                             // Max site density per nm2
                xj = XSs / XVk / Mm / pmp->Nfsp[k][ist] * 1e6
                     /pmp->Aalp[k]/1.66054; // Current density per nm2
                if( pa.p.PC == 1 )
                    rIEPS = pa.p.IEPS * xj0; // relative IEPS
                if(xj >= xj0/dent)
                     xj = xj0/dent - rIEPS;  // upper limit
                q2 = xj0 - xj*dent;  // Computing differences in QCA gamma
                q1 = xj0 - xj;
                if( q1 < 1e-22 )
                    q1 = 1e-22;
                if( q2 < 1e-22 )
                    q2 = 1e-22;
                ISAT = log(xj0) + log(q1)*(dent-1) - log(q2)*dent;
            // Calculation of the Frumkin exponential term
                if( fabs (Fi) < 1e-9 || fabs (cN) < 1e-9 )
                   eF = 0.0;
                else {
                   double pivovar/* = 1.0*/;
                   eF = cN * Fi * xj / xj0 ;  // Fi = Fi'/(kT) Bockris p.938
            // Calculation of the Pivovarov 98 exponential correction term
                   pivovar = xj0 / ( xj0 + xj * ( cN -1 ));
                   eF *= pivovar;
                }
                pmp->lnGam[j] = ISAT + eF;
pmp->lnSAC[ja][0] = ISAT;
pmp->lnSAC[ja][1] = eF;
                break;
            case SAT_VIR_NCOMP: /* Non-Competitive virial isotherm */
                dent = 1; // monodentate for now
                xj0 = fabs(pmp->MASDJ[ja][PI_DEN])/pmp->Aalp[k]/1.66054;
                                             // Max site density per nm2
                xj = XSs / XVk / Mm / pmp->Nfsp[k][ist] * 1e6  // xj / XVk ...
                     /pmp->Aalp[k]/1.66054; // Current density per nm2
                if( pa.p.PC == 1 )
                    rIEPS = pa.p.IEPS * xj0; // relative IEPS
                if(xj >= xj0/dent)
                     xj = xj0/dent - rIEPS;  // upper limit
                ISAT = 0.0;
                if( fabs (Fi) < 1e-9 || fabs (cN) < 1e-9 )
                   eF = 0.0;
                else {         // Calculation of the Frumkin exponential term
                   eF = cN * Fi * xj / xj0 ;  // Fi = Fi'/(kT) Bockris p.938
                }
                pmp->lnGam[j] = ISAT + eF;
pmp->lnSAC[ja][0] = ISAT;
pmp->lnSAC[ja][1] = eF;
                break;
            case SAT_BET_NCOMP: /* Non-competitive BET for surf.precipitation */
                ISAT = 0.0;
// To be completed
//
//
                pmp->lnGam[j] = ISAT;
pmp->lnSAC[ja][0] = ISAT;
                break;
            case SAT_INDEF: /* No SAT calculation */
                pmp->lnGam[j] = 0.0;
pmp->lnSAC[ja][0] = 0;
                break;
            default:        /* pmp->lnGam[j] = 0.0; */
                break;
//  Old obsolete SAT calculations
            case SAT_COMP: // Competitive SAT (old, obsolete) on a surface type
                if( iSite[ist] < 0 )
                    xjn = 0.0;
                else xjn = pmp->X[iSite[ist]]; // neutral site does not compete!
                XS0 = pmp->MASDT[k][ist] * XVk * Mm / 1e6
                      * pmp->Nfsp[k][ist]; // expected total in moles
                if( pa.p.PC == 1 )
                    rIEPS = pa.p.IEPS * XS0;  // relative IEPS
                XSkC = XSk - xjn - xj; // occupied by the competing species;
	                             // this sorbate cannot compete to itself
                if( XSkC < 0.0 )
                    XSkC = 0.0;
                if( XSkC >= XS0 )  // Limits
                    XSkC = XS0 - 2.0 * rIEPS;
                xj0 = XS0 - XSkC;    // expected moles of this sorbate
                if(xj >= xj0)
                       xj = xj0 - rIEPS; // limits: 2 * rIEPS to XS0 - rIEPS
                if( xj * 2 <= xj0 )
                    ISAT = 0.0;      // ideal case
                else
                {
                   q1 = xj0 - xj;
                   q2 = rIEPS * XS0;
                   if( pa.p.PC == 2 && !pmp->W1 || pa.p.PC != 2 )
                   {
                      if( q1 > q2 )
                        q2 = q1;
                   }
                   else {
                      q2 = q1;
                      if( q2 <= 1e-33 )
                         q2 = 1e-33;
                   }
                   ISAT = log( xj ) - log( q2 );
                }
                pmp->lnGam[j] = ISAT;
pmp->lnSAC[ja][0] = ISAT;
                break;
            case SAT_NCOMP: /* Non-competitive truncated L SAT (old, obsolete) */
// rIEPS = pa.p.IEPS * 2;
                xj0 = fabs( pmp->MASDJ[ja][PI_DEN] ) * XVk * Mm / 1e6
                      * pmp->Nfsp[k][ist]; /* in moles */
                if( pa.p.PC == 1 )
                    rIEPS = pa.p.IEPS * xj0;  // relative IEPS
                if(xj >= xj0)
                     xj = xj0 - rIEPS;  // upper limit
                if( xj * 2.0 <= xj0 )  // Linear adsorption - to improve !
                    ISAT = 0.0;
                else
                {
                    q1 = xj0 - xj;  // limits: rIEPS to 0.5*xj0
                    q2 = xj0 * rIEPS;
                    if( pa.p.PC == 2 && pmp->W1 )
                       ISAT = log( xj ) - log( q1 );
                    else {
                       if( q1 > q2 )
                          ISAT = log( xj ) - log( q1 );
                       else
                          ISAT = log( xj ) - log( q2 );
                    }
                 }
                pmp->lnGam[j] = ISAT;
pmp->lnSAC[ja][0] = ISAT;
                break;
            case SAT_SOLV:  // Neutral surface site (e.g. >O0.5H@ group)
                            // applies to the whole surface type!
                XSs = 0.0;  // calc total moles on all sites on surface type
                for( i=0; i<MST; i++ )
                   XSs += pmp->D[i][ist];
                XSkC = XSs / XVk / Mm * 1e6  // total non-solvent surf.species
                   /pmp->Nfsp[k][ist]/ pmp->Aalp[k]/1.66054;  // per nm2
                XS0 = max( pmp->MASDT[k][ist], pmp->MASDJ[ja][PI_DEN] );
                SATst = pa.p.DNS*1.66054*pmp->Aalp[k]/XS0;
                XS0 = XS0 / pmp->Aalp[k]/1.66054;
                if( pa.p.PC == 1 )
                    rIEPS = pa.p.IEPS * XS0;  // relative IEPS
                if( XSkC < 0.0 )
                    XSkC = 0.0;
                if( XSkC >= XS0 )  // Limits
                    XSkC = XS0 - 2.0 * rIEPS;
                q1 = XS0 - XSkC;
                if( pa.p.PC == 2 && !pmp->W1 || pa.p.PC != 2 )
                {
                  q2 = rIEPS * XS0;
                  if( q1 > q2 )
                      q2 = q1;
                }
                else {
                   q2 = q1;
                   if( q2 <= 1e-22 )
                       q2 = 1e-22;
                }
                ISAT = log( XS0 ) - log( q2 );
                pmp->lnGam[j] = ISAT + log( SATst );
pmp->lnSAC[ja][0] = ISAT;
pmp->lnSAC[ja][1] = log(SATst);
/*                if( pmp->IT > 0 &&
                  ( SAT < 1e-2 || SAT > 1.0/rIEPS || fabs(log(SAT)+log(SATst)-OSAT) > 3 ) )
                {
cout << "IT=" << pmp->IT << ": NSite SAT for j= " << j << " old lnSAT= " << OSAT << " new lnSAT= " << ISAT << endl;
cout << "     x[jn]= " << pmp->X[j] << " XSk= " << XSk << " XSkC=" << XSkC << " XS0=" << XS0 << " SATst=" << SATst << endl;
//  ISAT = OSAT + 1;           // experiment
*/                break;
            }
        }
    }  /* j */
}


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* Corrections to prime chemical potentials F0[j]
*  of j-th species in k-th phase among IPM main iterations
*  Returns double - value of corrected chem. potential.
*  If error, returns +7777777 J/mole.
*  Last modif. 05 Jan 2000 by DAK to include BSM EDL model.
*/
double TProfil::Ej_init_calc( double, int j, int k)
{
    int ja=0, ist/*=0*/, isp/*=0*/, jc=-1;
    double F0=0.0, Fold, dF0, Mk=0.0, Ez, psiA, psiB, CD0, CDb, ObS;

    Fold = pmp->F0[j];
    if( pmp->FIat > 0 && j < pmp->Ls && j >= pmp->Ls - pmp->Lads )
    {
        ja = j - ( pmp->Ls - pmp->Lads );
        jc = pmp->SATX[ja][XL_EM];
    }
    if( k < pmp->FIs && pmp->XFA[k] > 1e-12)
    {
           if( jc < 0 ) /* phase (carrier) molar mass g/mkmol */
              Mk = pmp->FWGT[k]/pmp->XFA[k]*1e-6;
           else Mk = pmp->MM[jc]*(pmp->X[jc]/pmp->XFA[k])*1e-6;
        /* DC carrier molar mass g/mkmol */
    }
    switch( pmp->DCC[j] )
    { /* analyse species class code */
    case DC_SCP_CONDEN:
    case DC_AQ_PROTON:
    case DC_AQ_ELECTRON:
    case DC_AQ_SPECIES:
    case DC_GAS_COMP:
    case DC_GAS_H2O:
    case DC_GAS_CO2:
    case DC_GAS_H2:
    case DC_GAS_N2:
    case DC_AQ_SOLVENT:
    case DC_AQ_SOLVCOM:
    case DC_SOL_IDEAL:
    case DC_SOL_MINOR:
    case DC_SOL_MAJOR:
        F0 = pmp->lnGmM[j];
        break;
        /*adsorption */
    case DC_SSC_A0:
    case DC_SSC_A1:
    case DC_SSC_A2:
    case DC_SSC_A3:
    case DC_SSC_A4:
    case DC_WSC_A0:
    case DC_WSC_A1:
    case DC_WSC_A2:
    case DC_WSC_A3:
    case DC_WSC_A4:
    case DC_SUR_GROUP:
    case DC_SUR_COMPLEX:
    case DC_SUR_IPAIR:
    case DC_IESC_A:
    case DC_IEWC_B:
        F0 = pmp->lnGmM[j]; /* + pmp->lnGam[j]; */
        /* get ist - index of surface type and isp - index of surface plane  */
        ist = pmp->SATX[ja][0] / MSPN;
        isp = pmp->SATX[ja][0] % MSPN;
        CD0 = pmp->MASDJ[ja][PI_CD0];  // species charge that goes into 0 plane
        CDb = pmp->MASDJ[ja][PI_CDB];  // species charge that goes into B plane
        ObS = pmp->MASDJ[ja][PI_DEN];  // obsolete - the sign for outer-sphere charge
        if( ObS >= 0.0 )
            ObS = 1.0;
        else ObS = -1.0;
        psiA = pmp->XpsiA[k][ist];
        psiB = pmp->XpsiB[k][ist];
        Ez = double(pmp->EZ[j]);
        if( !isp )  // This is the 0 (A) plane species
        {
            if( fabs( CD0 ) > 1e-20 )  // Doubtful...
                Ez = CD0;
            F0 += psiA * Ez * pmp->FRT;
        }
        else  /* This is B plane */
        {
            if( pmp->SCM[k][ist] == SC_MTL || pmp->SCM[k][ist] == SC_MXC )
            { /* Modified TL: Robertson, 1997; also XTLM Kulik 2002 */
                  if( fabs( CDb ) > 1e-20 )  // Doubtful...
                      Ez = CDb;
                  F0 += psiB * Ez * pmp->FRT;
            }
            if( pmp->SCM[k][ist] == SC_TLM )
            {
// New CD version of TLM  added 25.10.2004
               if( fabs( CD0 ) > 1e-20 && fabs( CDb ) > 1e-20 )
                  F0 += ( psiA*CD0 + psiB*CDb )* pmp->FRT;
// see also Table 4 in Zachara & Westall, 1999
// Old version:  TLM Hayes & Leckie, 1987 uses the sign indicator at density
                else {
                  if( ObS < 0 )
                  {
                      Ez -= 1.0;
                      F0 += ( psiA + Ez * psiB )* pmp->FRT;
                  }
                  else
                  {
                      Ez += 1.0;
                      F0 += ( Ez * psiB - psiA )* pmp->FRT;
                  }
               }
            }
            else if( pmp->SCM[k][ist] == SC_BSM || pmp->SCM[k][ist] == SC_CCM )
            { /* Basic Stern model Christl & Kretzschmar, 1999 */
// New CD version of TLM  added 25.10.2004
               if( fabs( CD0 ) > 1e-20 && fabs( CDb ) > 1e-20 )
                  F0 += ( psiA*CD0 + psiB*CDb )* pmp->FRT;
                else {
                  if( ObS < 0 )
                  {
                      Ez -= 1.0;
                      F0 += ( psiA + Ez * psiB )* pmp->FRT;
                  }
                  else
                  {
                      Ez += 1.0;
                      F0 += ( Ez * psiB - psiA )* pmp->FRT;
                  }
               }
            }
        }
        if( Mk > 1e-9 )
        {
            if( pmp->SCM[k][ist] == SC_MXC || pmp->SCM[k][ist] == SC_NNE ||
                    pmp->SCM[k][ist] == SC_IEV )
                F0 -= log( Mk * pmp->Nfsp[k][ist] * pmp->Aalp[k]
                           * pa.p.DNS*1.66054 );
            else F0 -= log( Mk * pmp->Nfsp[k][ist] * pmp->Aalp[k]
                                * pa.p.DNS*1.66054 );
            F0 -= pmp->Aalp[k]*Mk*pa.p.DNS*1.66054 /
                  ( 1.0 + pmp->Aalp[k]*Mk*pa.p.DNS*1.66054 );
        }
        break;
    case DC_PEL_CARRIER:
    case DC_SUR_MINAL:  // const charge of carrier - not yet done !!!!!!!!!!
    case DC_SUR_CARRIER:
        F0 -= pmp->Aalp[k]*Mk*pa.p.DNS*1.66054 /
              ( 1.0 + pmp->Aalp[k]*Mk*pa.p.DNS*1.66054 );
        F0 += pmp->Aalp[k]*Mk*pa.p.DNS*1.66054;
        break;
    }
    F0 += pmp->lnGam[j];

    if( k >= pmp->FIs )   //Sveta ?????????????
        return F0;
    // Smoothing procedure for highly non-ideal phases
    if( pmp->sMod[k][SGM_MODE] != SM_IDEAL
            || pmp->sMod[k][SCM_TYPE] != SC_NNE )
    {
        dF0 = F0 - Fold;
        if( pmp->X[j]>pmp->lowPosNum && fabs( dF0 ) >= 1e-5 ) /* to be checked !!! */
            F0 = Fold + dF0 * pmp->FitVar[3];
    }  // FitVar[3] = TinkleSuppressFactor(); see GammaCalc()
    return F0;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* Link DOD for Non-ideality Equations in GammaCalc() */
void TProfil::pm_GC_ods_link( int k, int jb, int jpb, int jdb )
{
//Ask Dima!!! 20/04/2002
#ifndef IPMGEMPLUGIN
    ErrorIf( k < 0 || k >= pmp->FIs , "GammaCalc", "Illegal link: k=0||>FIs" );
    aObj[ o_nsmod].SetPtr( pmp->sMod[k] );
    aObj[ o_nncp].SetPtr( pmp->LsMod+k );
    aObj[ o_nncd].SetPtr( pmp->LsMdc+k );
    aObj[ o_ndc].SetPtr(  pmp->L1+k );
    aObj[ o_nez].SetPtr( pmp->EZ+jb );
    aObj[o_nez].SetN(  pmp->L1[k]);
    aObj[ o_npcv].SetPtr( pmp->PMc+jpb );
    aObj[o_npcv].SetDim( 1, pmp->LsMod[k]);
    aObj[ o_ndcm].SetPtr( pmp->DMc+jdb );
    aObj[o_ndcm].SetDim( pmp->L1[k], pmp->LsMdc[k] );
    aObj[ o_nmvol].SetPtr( pmp->Vol+jb );
    aObj[o_nmvol].SetN( pmp->L1[k]);
    aObj[ o_nppar].SetPtr(pmp->VL+jb );
    aObj[o_nppar].SetN(  pmp->L1[k]);
    aObj[ o_ngtn].SetPtr( pmp->G0+jb );
    aObj[o_ngtn].SetN( pmp->L1[k] );
    aObj[ o_ngam].SetPtr( pmp->Gamma+jb ); /* Gamma calculated */
    aObj[o_ngam].SetN( pmp->L1[k] );
    aObj[ o_nlngam].SetPtr( pmp->lnGam+jb ); /* ln Gamma calculated*/
    aObj[o_nlngam].SetN( pmp->L1[k]);
    aObj[ o_nu].SetPtr(  pmp->U );
    aObj[o_nu].SetM( pmp->N );
    aObj[ o_nas].SetPtr(  pmp->A+pmp->N*jb );
    aObj[o_nas].SetDim(  pmp->L1[k], pmp->N );
    aObj[ o_nxa].SetPtr(  pmp->XF+k );
    aObj[ o_nxaa].SetPtr(  pmp->XFA+k );
    if( pmp->FIat > 0 )
    {
        aObj[ o_nxast].SetPtr( pmp->XFTS[k] );
        aObj[ o_nxcec].SetPtr( pmp->MASDT[k] );
    }
    else
    {
        aObj[ o_nxast].SetPtr( 0 );
        aObj[ o_nxcec].SetPtr( 0 );
    }
    /* */
    aObj[ o_nbmol].SetPtr( pmp->FVOL+k );  /* phase volume  */
    aObj[ o_nxx].SetPtr(  pmp->X+jb );
    aObj[o_nxx].SetN( pmp->L1[k]);
    aObj[ o_nwx].SetPtr(  pmp->Wx+jb );
    aObj[o_nwx].SetN( pmp->L1[k]);
    aObj[ o_nmju].SetPtr( pmp->Fx+jb );
    aObj[o_nmju].SetN( pmp->L1[k]);
    aObj[ o_nqp].SetPtr( pmp->Qp+k*QPSIZE );
    aObj[ o_nqd].SetPtr( pmp->Qd+k*QDSIZE );   // Fixed 7.12.04 by KD
#endif
}

// Calculation of smoothing factor for high non-ideality
// Returns current value of this factor
double TProfil::TinkleSupressFactor( double ag, int ir)
{
    double TF, dg, rg=0.0, irf;
    int Level, itqF, itq;

    Level = pmp->pRR1;
    irf = (double)ir;
    dg = pa.p.DGC;
    if( dg > -0.1 )
    { /* New smoothing suitable also for adsorption
        if( ir <= 50 ) rg = 0.0;
        else if( ir > 50 && ir <= 80 )  rg = ( irf-50.)/30.;
        else if( ir > 80 && ir <= 100 ) rg = ( irf-80.)/20.+1.0;
        else if( ir > 100 ) rg = ( irf-100.)/10.+2.0;
        TF = ag * pow( dg, rg );  */
        if(ag>1) ag=1;
        if(ag<0.1) ag=0.1;
        if(dg>0.15) dg=0.15;
        if( dg <= 0.0 )
          TF = ag;
        else
          TF = ag * ( 1 - pow(1-exp(-dg*irf),60.));
        Level =  (int)rg;
    }
    else
    {  /* Old smoothing for solid solutions: -1.0 < pa.p.DGC < 0 */
        itq = ir/60;
        dg = fabs( dg );
        itqF = ir/(60/(itq+1))-itq;  /* 0,1,2,4,5,6... */
        if( itqF < Level )
            itqF = Level;
        TF = ag * pow( dg, itqF );
        Level = itqF;
    }
    pmp->pRR1 = (short)Level;
    return TF;
}

static double ICold=0.;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* Calculation of the activity coefficient vector (lnGam[])
*  LinkMode - parameter indicating call status of GammaCalc():
*   LINK_TP_MODE  - calculation of equations depending on TP only;
*   LINK_UX_MODE  - calculation of equations depending on current
*                   IPM approximation of the equilibrium state;
*   LINK_FIA_MODE - calculation on initial approximation (FIA).
*/
void TProfil::GammaCalc( int LinkMode  )
{
    int k, j, jb, je=0, jpb, jpe=0, jdb, jde=0;
    char *sMod;
    double LnGam, pmpXFk;
    //  high-precision IPM-2
 //   if(pmp->PZ && pmp->W1 > 1 )
 //     goto END_LOOP;

   pmp->FitVar[3] = TinkleSupressFactor( pmp->FitVar[4], pmp->IT );

    /* calculating concentrations of species in multi-component phases */
    switch( LinkMode )
    {
    case LINK_FIA_MODE: /* Initial approximation mode */
        if( pmp->LO )
        {
            pmp->GWAT = 55.51;
            pmp->XF[0] = pmp->GWAT;
            for( j=0; j<pmp->L; j++ )
                if( pmp->X[j] < pmp->lowPosNum )
                    pmp->X[j] = pa.p.DFYaq;
            ConCalc( pmp->X, pmp->XF, pmp->XFA );
//            if( pmp->E )
//                IS_EtaCalc( );
//          pmp->IC = max( pmp->MOL, pmp->IC );
            pmp->IC = 0.0;  // Important for simplex FIA reproducibility
            if( pmp->E && pmp->FIat > 0 )
            {
               for( k=0; k<pmp->FIs; k++ )
               {
                 int ist;
                 if( pmp->PHC[k] == PH_POLYEL || pmp->PHC[k] == PH_SORPTION )
                   for( ist=0; ist<pmp->FIat; ist++ ) /* cycle by surface types */
                   {
                     pmp->XetaA[k][ist] = 0.0;
                     pmp->XetaB[k][ist] = 0.0;
                     pmp->XpsiA[k][ist] = 0.0;
                     pmp->XpsiB[k][ist] = 0.0;
                     pmp->XpsiD[k][ist] = 0.0;
                     pmp->XcapD[k][ist] = 0.0;
                   }  // ist
               }  // k
            } // FIat
        }   // LO
        /* clearing vectors of  activity coefficients */
        for( j=0; j<pmp->L; j++ )
        {
            if( pmp->lnGmf[j] )
                pmp->lnGam[j] = pmp->lnGmf[j];
            /* !!!!!!!!!!!!!!!!!*/
            else pmp->lnGam[j] = 0.;/* pmp->lnGmm[j]; */
            pmp->Gamma[j] = 1.;
        }
    case LINK_TP_MODE:  /* Equations depending on T,P only */
/*      if( pmp->LO && pmp->Lads )           Debugging!
        {
            ConCalc( pmp->X, pmp->XF, pmp->XFA  );
            if( pmp->E )
                IS_EtaCalc();
        }
*/      pmp->FitVar[3] = 1.0;  // is it really necessary ?
        break;
    case LINK_UX_MODE: /* calc concentrations of DC on IPM iteration */
        ConCalc( pmp->X, pmp->XF, pmp->XFA );
        /* clearing vectors of  activity coefficients */
        for( j=0; j<pmp->L; j++ )
        {
            pmp->lnGam[j] = 0.;
            pmp->Gamma[j] = 1.;
        }
        if( pmp->E ) /* calc charge in the phase */
        {
//          IS_EtaCalc();
          if( pmp->FIat > 0 )
             for( k=0; k<pmp->FIs; k++ )
             {
               if( pmp->PHC[k] == PH_POLYEL || pmp->PHC[k] == PH_SORPTION )
               {  int ist;
                  for( ist=0; ist<pmp->FIat; ist++ ) /* cycle by surface types */
                  {  // Clear electrostatic potentials
                     pmp->XpsiA[k][ist] = 0.0;
                     pmp->XpsiB[k][ist] = 0.0;
                     pmp->XpsiD[k][ist] = 0.0;
                  }  // ist
                }
             }  // k
         } // pmp->E
        break;
    default:
        Error("GammaCalc","Illegal Link Mode.");
    }

    for( k=0; k<pmp->FI; k++ )
    { /* loop on phases */
        jb = je;
        je += pmp->L1[k];
        if( pmp->L1[k] == 1 )
            goto END_LOOP;
        sMod = pmp->sMod[k];
        if( sMod[SGM_MODE] == SM_IDEAL )
            goto END_LOOP;
pmpXFk = 0.;  // Added 07.01.05 by KD
for( j = jb; j < je; j++ )
   pmpXFk += pmp->X[j];
        jpb = jpe;
        jpe += pmp->LsMod[k];
        jdb = jde;
        jde += pmp->LsMdc[k]*pmp->L1[k];
//  memset( pmp->Qd, 0, sizeof(double)*QDSIZE );  Dubious line! KD 03.07.02

        switch( pmp->PHC[k] )
        {   /* calculate activity coefficients using built-in functions */
          case PH_AQUEL:   /*calc by DH III appr. HKF */
             if( sMod[SGM_MODE] == SM_STNGAM && /* pmp->XF[k] */ pmpXFk > pa.p.XwMin
                  && pmp->IC > pa.p.ICmin )
             {
                switch( sMod[SPHAS_TYP] )
                {
                  case SM_AQDH3:
                       DebyeHueckel3Karp( jb, je, jpb, jdb, k );
                          break;
                  case SM_AQDH2:
                       DebyeHueckel2Kjel( jb, je, jpb, jdb, k );
                          break;
                  case SM_AQDH1:
                       DebyeHueckel1LL( jb, je, /* jpb, jdb, */ k );
                          break;
                  case SM_AQDHH:
                       DebyeHueckel3Hel( jb, je, jpb, jdb, k );
                          break;
                  case SM_AQDAV:
                       Davies03temp( jb, je, /* jpb, jdb, */ k );
                          break;
                  case SM_AQSIT:  // SIT - under construction
                       SIT_aqac_PSI( jb, je, jpb, jdb, k );
                          break;
                  default:
                          break;
                }
                goto END_LOOP; /* break; */
             }
             else if( sMod[SGM_MODE] == SM_IDEAL )
                goto END_LOOP;
             break;
          case PH_GASMIX:
          case PH_PLASMA:
          case PH_FLUID:
            if( sMod[SGM_MODE] == SM_STNGAM )
            {
                if( /* pmp->XF[k] */ pmpXFk > pmp->DSM )
                {
                    if( sMod[SPHAS_TYP] == SM_FLUID && pmp->XF[k] > pa.p.PhMin )
                       ChurakovFluid( jb, je, jpb, jdb, k );
                }
                goto END_LOOP; /* break; */
            }
            else if( sMod[SGM_MODE] == SM_IDEAL )
                goto END_LOOP;
            break;
         case PH_LIQUID:
         case PH_SIMELT:
         case PH_SINCOND:
         case PH_SINDIS:
         case PH_HCARBL:  // solid and liquid nonel solutions
             if( sMod[SGM_MODE] == SM_STNGAM && /* pmp->XF[k] */ pmpXFk > pmp->DSM )
             {
                switch( sMod[SPHAS_TYP] )
                {
                  case SM_REDKIS:
                       RedlichKister( jb, je, jpb, jdb, k );
                          break;
                  case SM_MARGB:
                       MargulesBinary( jb, je, jpb, jdb, k );
                          break;
                  case SM_MARGT:
                       MargulesTernary( jb, je, jpb, jdb, k );
                          break;
                  case SM_RECIP: // under construction
                  default:
                          break;
                }
                goto END_LOOP; /* break; */
             }
             else if( sMod[SGM_MODE] == SM_IDEAL )
                goto END_LOOP;
             break;
        case PH_POLYEL:  /* PoissonBoltzmann( q, jb, je, k ) break; */
        case PH_SORPTION: /* calc elstatic potenials from Gouy-Chapman eqn */
            if( pmp->PHC[0] == PH_AQUEL && /* pmp->XF[k] */ pmpXFk > pmp->DSM
                   && pmp->XF[0] > pa.p.XwMin )
            {
//              ConCalc( pmp->X, pmp->XF, pmp->XFA  );  Debugging
                if( LinkMode == LINK_UX_MODE )
                {
                    if( pmp->E )
                    {
                       IS_EtaCalc();
                       GouyChapman( jb, je, k );
                    /* PoissonBoltzmann( q, jb, je, k ) */
                    }
                    /* Calculate surface activity coeffs */
//                  SurfaceActivityTerm(  jb, je, k );
                    SurfaceActivityCoeff(  jb, je, jpb, jdb, k );
                }
            }
            if( sMod[SGM_MODE] == SM_IDEAL )
                goto END_LOOP;
            else break;
        default:
            if( sMod[SGM_MODE] == SM_STNGAM )
            {  /* RegSolMod(  jb, je, k ) */;
                goto END_LOOP; /* break; */
            }
            if( sMod[SGM_MODE] == SM_IDEAL )
                goto END_LOOP;
        }
        if( pmp->XF[k]<=pmp->DSM && LinkMode == LINK_UX_MODE )
            goto END_LOOP; /* phase is zeroed off */
        if( pmp->LO && pmp->XF[0] <= pa.p.XwMin && LinkMode == LINK_UX_MODE )
            goto END_LOOP; /* aqueous phase is disappearing */
        if(pmp->PHC[k] == PH_AQUEL && (( pmp->IC < pa.p.ICmin))
                && LinkMode == LINK_UX_MODE )
            goto END_LOOP; /* Ionic strength is too low for aqueous solution */
        else ICold = pmp->IC;
//Ask Dima!!! 20/04/2002
#ifndef IPMGEMPLUGIN
        /* Link DOD and set sizes of work arrays */
        pm_GC_ods_link( k, jb, jpb, jdb );
        pmp->is=0;
        pmp->js=0;
        pmp->next=1;

        switch( LinkMode )
        { /* check mode of calc */
        case LINK_TP_MODE: /* before calc TP-models */
            if( sMod[SPHAS_DEP] == SM_TPDEP && multi->qEp[k].nEquat() )
                multi->qEp[k].CalcEquat();
            if( sMod[DCOMP_DEP] == SM_TPDEP && multi->qEd[k].nEquat() )
            {  /* calc scripted models */
                switch( sMod[DCE_LINK] )
                {
                case SM_PUBLIC:  /* one script to all species */
                    for( pmp->js=0, pmp->is=0; pmp->js<pmp->L1[k]; pmp->js++ )
                        multi->qEd[k].CalcEquat();
                    break;
                case SM_PRIVATE_: /* separate equations for species */
                    multi->qEd[k].CalcEquat();
                    break;
                }
            }
            break;
        case LINK_FIA_MODE: /* start approximation */
            break;
        case LINK_UX_MODE:  /* model dependent on current approximation */
            if( sMod[SPHAS_DEP] == SM_UXDEP && multi->qEp[k].nEquat() )
                // Equations for the whole phase
                multi->qEp[k].CalcEquat();
            if( sMod[DCOMP_DEP] == SM_UXDEP && multi->qEd[k].nEquat() )
            {  /* Equations for species */
                switch( sMod[DCE_LINK] )
                {
                case SM_PUBLIC:  /* one script to all species */
                    for( pmp->js=0, pmp->is=0; pmp->js<pmp->L1[k]; pmp->js++ )
                        multi->qEd[k].CalcEquat();
                    break;
                case SM_PRIVATE_:  /* separate group of equations for each species */
                    multi->qEd[k].CalcEquat();
                    break;
                }
            }
            break;
        default:
            Error("GammaCalc","Illegal LinkMode 2");
        }
#endif
END_LOOP: /* if( LinkMode == LINK_TP_MODE ) */
        /* make lnGam with priority*/
        for( j=jb; j<je; j++ )
        {
            if( fabs( 1.0-pmp->Gamma[j] ) > 1e-9 && pmp->Gamma[j] > 1e-35 )
                pmp->lnGam[j] += log( pmp->Gamma[j] );
            LnGam = pmp->lnGam[j];
            pmp->lnGmo[j] = LnGam;
            if( fabs( LnGam ) > 1e-9 && fabs( LnGam )<42. )
                pmp->Gamma[j] = exp( LnGam );
            else pmp->Gamma[j] = 1.0;
            pmp->F0[j] = Ej_init_calc( 0.0, j, k );
            pmp->G[j] = pmp->G0[j] + pmp->F0[j];
        }
    }  /* k */
    //  if( wn[W_EQCALC].status )
    //  aSubMod[MD_EQCALC]->ModUpdate("PM_ipms   EqCalc() converged");
}

// ----------------------------------------------------------------------------
// Built-in functions for activity coefficients
//
// aqueous electrolyte
// SIT NEA PSI

void
TProfil::SIT_aqac_PSI( int jb, int je, int /*jpb*/, int /*jdb*/, int /*k*/ )
{

    int j/*=0*/, icat/*=0*/, ian/*=0*/, ic/*=0*/, ia/*=0*/;
    double T, A, B, /*a0=3.72, *//*a0c,*/
           I, sqI, /*bg,*/ bgi=0, Z2, lgGam, /*molt,*/ SumSIT;
//    float nPolicy;

    I= pmp->IC;
    if( I < pa.p.ICmin )
        return;
    T = pmp->Tc;
    A = 1.82483e6 * sqrt( pmp->denW ) / pow( T*pmp->epsW, 1.5 );
    B = 50.2916 * sqrt( pmp->denW ) / sqrt( T*pmp->epsW );

//    molt = ( pmp->XF[0]-pmp->XFA[0] )*1000./18.01528/pmp->XFA[0]; /* tot.molality */
    sqI = sqrt( I );

    ErrorIf( fabs(A) < 1e-9 || fabs(B) < 1e-9, "SIT",
        "Error: A,B were not calculated - no values of RoW and EpsW !" );

    /* Calculation of EDH equation */
//  bgi = bg;
    ian= -1;
    icat = -1;
    for( j=jb; j<je; j++ )
    {
// Determining index of cation or anion
      if( pmp->EZ[j] < 0 )
          ian++;
      else if( pmp->EZ[j] > 0 )
          icat++;
      else ;

      if( pmp->EZ[j] )
      {       // Charged species : calculation of the DH part
           Z2 = pmp->EZ[j]*pmp->EZ[j];
           lgGam = ( -A * sqI * Z2 ) / ( 1. + 1.5 * sqI );  // B * 4.562 = 1.5 at 25 C
              // Calculation of SIT sums
           SumSIT = 0.;
           if( pmp->EZ[j] > 0 )
           {       // this is a cation
              for( ia=0; ia<pmp->sitNan; ia++ )
                 SumSIT += pmp->sitE[ icat*pmp->sitNan + ia ]
                        * I * pmp->Y_m[pmp->sitXan[ia]];
              lgGam += SumSIT;
           }
           else {  // this is an anion
              for( ic=0; ic<pmp->sitNcat; ic++ )
                 SumSIT += pmp->sitE[ ic*pmp->sitNan + ian ]
                        * I * pmp->Y_m[pmp->sitXcat[ic]];
              lgGam += SumSIT;
           }
      }
      else
      { // Neutral species */
         if( pmp->DCC[j] != DC_AQ_SOLVENT ) // common salt-out coefficient ??
               lgGam = bgi * I;
            else /* water-solvent - a0 - osmotic coefficient */
               lgGam = 0.;
      }
      pmp->lnGam[j] = lgGam * lg_to_ln;
    } /* j */
    if( ++icat != pmp->sitNcat || ++ian != pmp->sitNan )
       Error( "SITgamma",
          "Inconsistent numbers of cations and anions in gamma calculation" );

}
//------------------------------------
//  EDH with common ion-size parameter
//
void
TProfil::DebyeHueckel3Hel( int jb, int je, int jpb, int /*jdb*/, int /* k */ )
{
    int j;
    double T, A, B, a0, a0c, I, sqI, bg, bgi, Z2, lgGam/*, molt*/;
    float nPolicy;

    I= pmp->IC;
    if( I < pa.p.ICmin )
        return;
    T = pmp->Tc;
    A = pmp->PMc[jpb+0];
    B = pmp->PMc[jpb+1];
    bg = pmp->PMc[jpb+5];
    a0c = pmp->PMc[jpb+6];
    nPolicy = pmp->PMc[jpb+7];

//    molt = ( pmp->XF[0]-pmp->XFA[0] )*1000./18.01528/pmp->XFA[0]; /* tot.molality */
    sqI = sqrt( I );

//Ask Dima!!! 20/04/2002
#ifndef IPMGEMPLUGIN
    if( fabs(A) < 1e-9 )
    {
        A = 1.82483e6 * sqrt( tpp->RoW ) / pow( T*tpp->EpsW, 1.5 );
//        pmp->PMc[jpb+0] = A;
    }
    if( fabs(B) < 1e-9 )
    {
        B = 50.2916 * sqrt( tpp->RoW ) / sqrt( T*tpp->EpsW );
//        pmp->PMc[jpb+1] = B;
    }
#else
    if( fabs(A) < 1e-9 )
        A = 1.82483e6 * sqrt( multi->RoW_ ) / pow( T*multi->EpsW_, 1.5 );
    if( fabs(B) < 1e-9 )
        B = 50.2916 * sqrt( multi->RoW_ ) / sqrt( T*multi->EpsW_ );
#endif
    ErrorIf( fabs(A) < 1e-9 || fabs(B) < 1e-9, "DebyeHueckel3Hel",
        "Error: A,B were not calculated - no values of RoW and EpsW !" );
    /* Calculation of EDH equation */
//  bgi = bg;
    for( j=jb; j<je; j++ )
    {
        bgi = bg; // Common third parameter
        a0 = a0c; // Common ion-size parameter
        if( pmp->EZ[j] )
        { /* Charged species */
            Z2 = pmp->EZ[j]*pmp->EZ[j];
            lgGam = ( -A * sqI * Z2 ) / ( 1. + B * a0 * sqI ) + bgi * I ;
        }
        else
        { /* Neutral species */
            if( nPolicy >= 0.0 )
            {
               if( pmp->DCC[j] != DC_AQ_SOLVENT ) /* salt-out coefficient */
                   lgGam = bgi * I;
               else /* water-solvent - a0 - osmotic coefficient */
                   lgGam = 0.;
//                 lgGam = a0 * molt; /* corrected: instead of I - tot.molality */
            }
            else { // nPolicy < 0 - all gamma = 1 for neutral species
               lgGam = 0.;
            }
        }
        pmp->lnGam[j] = lgGam * lg_to_ln;
    } /* j */
}

// Debye-Hueckel with Kielland ion-size
// and optional salt-out corr. for neutral species
//
void
TProfil::DebyeHueckel2Kjel( int jb, int je, int jpb, int jdb, int k )
{
    int j;
    double T, A, B, a0, /*a0c,*/ I, sqI, bg, bgi, Z2, lgGam, molt;
    float nPolicy;

    I= pmp->IC;
    if( I < pa.p.ICmin )
        return;
    T = pmp->Tc;
    A = pmp->PMc[jpb+0];
    B = pmp->PMc[jpb+1];
    bg = pmp->PMc[jpb+5];
//    a0c = pmp->PMc[jpb+6];
    nPolicy = pmp->PMc[jpb+7];

    molt = ( pmp->XF[0]-pmp->XFA[0] )*1000./18.01528/pmp->XFA[0]; /* tot.molality */
    sqI = sqrt( I );

//Ask Dima!!! 20/04/2002
#ifndef IPMGEMPLUGIN
    if( fabs(A) < 1e-9 )
    {
        A = 1.82483e6 * sqrt( tpp->RoW ) / pow( T*tpp->EpsW, 1.5 );
//        pmp->PMc[jpb+0] = A;
    }
    if( fabs(B) < 1e-9 )
    {
        B = 50.2916 * sqrt( tpp->RoW ) / sqrt( T*tpp->EpsW );
//        pmp->PMc[jpb+1] = B;
    }
#else
    if( fabs(A) < 1e-9 )
        A = 1.82483e6 * sqrt( multi->RoW_ ) / pow( T*multi->EpsW_, 1.5 );
    if( fabs(B) < 1e-9 )
        B = 50.2916 * sqrt( multi->RoW_ ) / sqrt( T*multi->EpsW_ );
#endif
    ErrorIf( fabs(A) < 1e-9 || fabs(B) < 1e-9, "DebyeHueckel2Kjel",
        "Error: A,B were not calculated - no values of RoW and EpsW !" );
    /* Calculation of DH equation */
    bgi = bg;
    for( j=jb; j<je; j++ )
    {
        a0 = pmp->DMc[jdb+j*pmp->LsMdc[k]];
        if( pmp->EZ[j] )
        { /* Charged species */
            Z2 = pmp->EZ[j]*pmp->EZ[j];
            lgGam = ( -A * sqI * Z2 ) / ( 1. + B * a0 * sqI ); // + bgi * I ;
        }
        else
        { /* Neutral species */
            if( nPolicy >= 0.0 )
            {
               if( a0 > 0.0 )
               {
                  if( pmp->DCC[j] != DC_AQ_SOLVENT ) /* salt-out coefficient */
                     lgGam = a0 * I;
                  else /* water-solvent - a0 - rational osmotic coefficient */
                     lgGam = a0 * molt; /* corrected: instead of I - sum.molality */
               }
               else {
                  if( a0 < -0.99 )
                      lgGam = 0.;
                  else if( fabs( a0 ) < 1e-9 )
                      lgGam = bgi * I;  /* Average salt-out coeff. */
                  else lgGam = a0 * I;  /* Busenberg & Plummer */
               }
            }
            else { // nPolicy < 0 - all gamma = 1 for neutral species
               lgGam = 0.;
            }
        }
        pmp->lnGam[j] = lgGam * lg_to_ln;
    } /* j */
}

void
TProfil::DebyeHueckel1LL( int jb, int je, /* int jpb, int jdb, */ int /* k */ )
{
    int j;
    double T, A, /* a0, */ I, sqI, Z2, lgGam;
//    float nPolicy;

    I= pmp->IC;
    if( I < pa.p.ICmin )
        return;
    T = pmp->Tc;
//    A = pmp->PMc[jpb+0];
    sqI = sqrt( I );

//Ask Dima!!! 20/04/2002
#ifndef IPMGEMPLUGIN
//    if( fabs(A) < 1e-9 )
        A = 1.82483e6 * sqrt( tpp->RoW ) / pow( T*tpp->EpsW, 1.5 );
#else
//    if( fabs(A) < 1e-9 )
        A = 1.82483e6 * sqrt( multi->RoW_ ) / pow( T*multi->EpsW_, 1.5 );
#endif
    ErrorIf( fabs(A) < 1e-9 /* || fabs(B) < 1e-9 */, "DebyeHueckel1LL",
        "Error: A was not calculated - no values of RoW and EpsW !" );
    /* Calculation of DH equation */
    for( j=jb; j<je; j++ )
    {
        if( pmp->EZ[j] )
        { /* Charged species */
            Z2 = pmp->EZ[j]*pmp->EZ[j];
            lgGam = ( -A * sqI * Z2 ); // / ( 1 + B * a0 * sqI ) + bgi * I ;
        }
        else  { /* Neutral species */
            lgGam = 0.;
        }
        pmp->lnGam[j] = lgGam * lg_to_ln;
    } /* j */
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* Aqueous electrolyte                        Brooklyn 5/15/97 DAK */
// Calculation of individual activity coefficients using extended
// Debye-Hueckel equation with common 3rd parameter (HKF81)
//  and individual (Kielland) ion size parameters
//
void TProfil::DebyeHueckel3Karp( int jb, int je, int jpb, int jdb, int k )
{
    int j;
    double T, A, B, a0, /*a0c,*/ I, sqI, bg, bgi, Z2, lgGam, molt;
    float nPolicy;

    I= pmp->IC;
    if( I < pa.p.ICmin )
        return;
    T = pmp->Tc;
    A = pmp->PMc[jpb+0];
    B = pmp->PMc[jpb+1];
    bg = pmp->PMc[jpb+5];
//    a0c = pmp->PMc[jpb+6];
    nPolicy = pmp->PMc[jpb+7];

    molt = ( pmp->XF[0]-pmp->XFA[0] )*1000./18.01528/pmp->XFA[0]; /* tot.molality */
    sqI = sqrt( I );

//Ask Dima!!! 20/04/2002
#ifndef IPMGEMPLUGIN
    if( fabs(A) < 1e-9 )
    {
       A = 1.82483e6 * sqrt( tpp->RoW ) / pow( T*tpp->EpsW, 1.5 );
//       pmp->PMc[jpb+0] = A;
    }
    if( fabs(B) < 1e-9 )
    {
       B = 50.2916 * sqrt( tpp->RoW ) / sqrt( T*tpp->EpsW );
//       pmp->PMc[jpb+1] = B;
    }
#else
    if( fabs(A) < 1e-9 )
        A = 1.82483e6 * sqrt( multi->RoW_ ) / pow( T*multi->EpsW_, 1.5 );
    if( fabs(B) < 1e-9 )
        B = 50.2916 * sqrt( multi->RoW_ ) / sqrt( T*multi->EpsW_ );
#endif
    ErrorIf( fabs(A) < 1e-9 || fabs(B) < 1e-9, "DebyeHueckel3Karp",
        "Error: A,B were not calculated - no values of RoW and EpsW !" );
    /* Calculation of EDH equation */
//  bgi = bg;
    for( j=jb; j<je; j++ )
    {
        bgi = bg;
        a0 = pmp->DMc[jdb+j*pmp->LsMdc[k]];
//        if( pmp->LsMdc[k] > 1 )
//        { /* Individual bg coeff Truesdell-Jones (Parkhurst,1990) */
//            bgi = pmp->DMc[jdb+j*pmp->LsMdc[k]+1];
//            if( !bgi )
//                bgi = bg;
//        }
        if( pmp->EZ[j] )
        { /* Charged species */
            Z2 = pmp->EZ[j]*pmp->EZ[j];
            lgGam = ( -A * sqI * Z2 ) / ( 1. + B * a0 * sqI ) + bgi * I ;
        }
        else
        { /* Neutral species */
            if( nPolicy >= 0.0 )
            {
               if( a0 > 0.0 )
               {
                  if( pmp->DCC[j] != DC_AQ_SOLVENT ) /* Setchenow coefficient */
                     lgGam = a0 * I;
                  else /* water-solvent - a0 - rational osmotic coefficient */
                     lgGam = a0 * molt; /* corrected: instead of I - sum.molality */
               }
               else {
                  if( a0 < -0.99 )
                      lgGam = 0.;
                  else if( fabs( a0 ) < 1e-9 )
                      lgGam = bgi * I;  /* Average Setchenow coeff. */
                  else lgGam = a0 * I;  /* Busenberg & Plummer */
               }
            }
            else { // nPolicy < 0 - all gamma = 1 for neutral species
               lgGam = 0.;
            }
        }
        pmp->lnGam[j] = lgGam * lg_to_ln;
    } /* j */
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* Aqueous electrolyte                      KD 25.01.02 */
/* Calculation of individual activity coefficients
* using Davies equation with common 0.3 parameter and
 temperature-dependent A parameter */
void TProfil::Davies03temp( int jb, int je, /* int jpb, int jdb, */ int /* k */ )
{
    int j;
    double T, A, I, sqI, Z2, lgGam;

    I= pmp->IC;
    if( I < pa.p.ICmin )
        return;  // too low ionic strength

    T=pmp->Tc;
    sqI = sqrt( I );
//    if( fabs(A) < 1e-9 )
//Ask Dima!!! 20/04/2002
#ifndef IPMGEMPLUGIN
    A = 1.82483e6 * sqrt( tpp->RoW ) / pow( T*tpp->EpsW, 1.5 );
#else
    A = 1.82483e6 * sqrt( multi->RoW_ ) / pow( T*multi->EpsW_, 1.5 );
#endif
//  at 25 C 1 bar: A = 0.5114
    ErrorIf( fabs(A) < 1e-9, "Davies03temp",
       "Error: A is not calculated - check values of RoW and EpsW !" );
    /* Calculation of Davies equation: Langmuir 1997 p. 133 */
    for( j=jb; j<je; j++ )
    {
        if( pmp->EZ[j] )
        {   /* Charged species */
            Z2 = pmp->EZ[j]*pmp->EZ[j];
            lgGam = ( -A * Z2 ) * ( sqI/( 1. + sqI ) - 0.3 * I );
        }
        else
        { /* Neutral species */
          lgGam = 0;
        }
        pmp->lnGam[j] = lgGam * lg_to_ln;
    } /* j */
}

// fluid mixtures
// ---------------------------------------------------------------------
void
TProfil::ChurakovFluid( int jb, int je, int /* jpb */, int jdb, int k )
{
    double *FugCoefs; float *EoSparam;
    int j, jj;
    double ro;
    TCGFcalc aCGF;

    FugCoefs = (double*)malloc( pmp->L1[k]*sizeof(double) );
    EoSparam = pmp->DMc+jdb;

    ro = aCGF.CGActivCoefPT( pmp->X+jb, EoSparam, FugCoefs, pmp->L1[k],
        pmp->Pc, pmp->Tc );
    if (ro <= 0. )
    {
//       cout << "\nCGFluid() error: ro= " << ro << endl;  // error message!
       return;
    }
    // Phase volume of the fluid in cm3
    pmp->FVOL[k] = pmp->FWGT[k] / ro;

    for( jj=0, j=jb; j<je; j++, jj++ )
    {
        if( FugCoefs[jj] > 1e-23 /* && pmp->Pparc[j] > 1e-23 */ )
             pmp->lnGam[j] = log(FugCoefs[ jj ]/pmp->Pparc[j]);
        else
             pmp->lnGam[j] = 0;
    } /* j */
    free( FugCoefs );

}

// ------------------ condensed mixtures --------------------------
// Binary Redlich-Kister model - parameters (dimensionless)
// in ph_cf Phase opt.array 2x3, see also Phase module
// Implemented by KD on 31 July 2003
//
void
TProfil::RedlichKister( int jb, int /* je */, int jpb, int /*jdb */, int k )
{
  double /* T, P, */ a0, a1, a2, lnGam1, lnGam2, X1, X2;

//  if( je != jb+1 )
//    ; // Wrong dimensions - error message?
//  T = pmp->Tc;
//  P = pmp->Pc;
// parameters
  a0 = pmp->PMc[jpb+0];
  a1 = pmp->PMc[jpb+1];  // regular model - should be 0
  a2 = pmp->PMc[jpb+2];  // regular model - should be 0
// mole fractions
  X1 = pmp->X[jb] / pmp->XF[k];
  X2 = pmp->X[jb+1] / pmp->XF[k];
// activity coeffs
  lnGam1 = X2*X2 *(a0 + a1*( 3.*X1 - X2 ) + a2 *( X1 - X2 )*( 5.*X1 - X2 ) );
  lnGam2 = X1*X1 *(a0 - a1*( 3.*X2 - X1 ) + a2 *( X2 - X1 )*( 5.*X2 - X1 ) );
// assignment
  pmp->lnGam[jb] = lnGam1;
  pmp->lnGam[jb+1] = lnGam2;
}

// Binary Margules model - parameters (in J/mol) in ph_cf Phase opt.array 2x3
// See also Phase module
// Implemented by KD on 31 July 2003
//
void
TProfil::MargulesBinary( int jb, int /*je*/, int jpb, int /* jdb */, int k )
{
  double T, P, WU1, WS1, WV1, WU2, WS2, WV2, WG1, WG2,
         a1, a2, lnGam1, lnGam2, X1, X2;
  double Vex/*, Sex, Hex, Uex*/;

//  if( je != jb+1 )
//    ; // Wrong dimensions - error message?
  T = pmp->Tc;
  P = pmp->Pc;
  WU1 = pmp->PMc[jpb+0];
  WS1 = pmp->PMc[jpb+1];  // in J/K/mol, if unknown should be 0
  WV1 = pmp->PMc[jpb+2];  // in J/bar if unknown should be 0
  WU2 = pmp->PMc[jpb+3];
  WS2 = pmp->PMc[jpb+4];  // if unknown should be 0
  WV2 = pmp->PMc[jpb+5];  // if unknown should be 0
// parameters
  WG1 = WU1 - T * WS1 + P * WV1;
  WG2 = WU2 - T * WS2 + P * WV2;
// if regular, WG1 should be equal WG2
// if ideal, WG1 = WG2 = 0
  a1 = WG1 / pmp->RT;
  a2 = WG2 / pmp->RT;
// mole fractions
  X1 = pmp->X[jb] / pmp->XF[k];
  X2 = pmp->X[jb+1] / pmp->XF[k];
// activity coeffs
  lnGam1 = ( 2.*a2 - a1 )* X2*X2 + 2.*( a1 - a2 )* X2*X2*X2;
  lnGam2 = ( 2.*a1 - a2 )* X1*X1 + 2.*( a2 - a1 )* X1*X1*X1;
// assignment
  pmp->lnGam[jb] = lnGam1;
  pmp->lnGam[jb+1] = lnGam2;
  // Calculate excess volume, entropy and enthalpy !
  // To be used in total phase property calculations
  Vex = ( WV1*X1 + WV2*X2 ) * X1*X2;
pmp->FVOL[k] += Vex*10.;
//  Sex = ( WS1*X1 + WS2*X2 ) * X1*X2;
//  Hex = ( (WU1+P*WV1)*X1 + (WU2+P*WV2)*X2 ) * X1*X2;
//  Uex = ( WU1*X1 + WU2*X2 ) * X1*X2;
 }

// Ternary regular Margules model - parameters (in J/mol)
// in ph_cf Phase opt.array 4x3; see also Phase module
// Implemented by KD on 31 July 2003
//
void
TProfil::MargulesTernary( int jb, int /*je*/, int jpb, int /*jdb*/, int k )
{
  double T, P, WU12, WS12, WV12, WU23, WS23, WV23, WU13, WS13, WV13,
         WU123, WS123, WV123, WG12, WG13, WG23, WG123,
         a12, a13, a23, a123, lnGam1, lnGam2, lnGam3, X1, X2, X3;
  double Vex/*, Sex, Hex, Uex*/;

//  if( je != jb+2 )
//    ; // Wrong dimensions - error message?
  T = pmp->Tc;
  P = pmp->Pc;
  WU12 = pmp->PMc[jpb+0];
  WS12 = pmp->PMc[jpb+1];  // if unknown should be 0
  WV12 = pmp->PMc[jpb+2];  // if unknown should be 0
  WU13 = pmp->PMc[jpb+3];
  WS13 = pmp->PMc[jpb+4];  // if unknown should be 0
  WV13 = pmp->PMc[jpb+5];  // if unknown should be 0
  WU23 = pmp->PMc[jpb+6];
  WS23 = pmp->PMc[jpb+7];  // if unknown should be 0
  WV23 = pmp->PMc[jpb+8];  // if unknown should be 0
  WU123 = pmp->PMc[jpb+9];
  WS123 = pmp->PMc[jpb+10];  // if unknown should be 0
  WV123 = pmp->PMc[jpb+11];  // if unknown should be 0

  // parameters
  WG12 = WU12 - T * WS12 + P * WV12;
  WG13 = WU13 - T * WS13 + P * WV13;
  WG23 = WU23 - T * WS23 + P * WV23;
  WG123 = WU123 - T * WS123 + P * WV123;
  a12 = WG12 / pmp->RT;
  a13 = WG13 / pmp->RT;
  a23 = WG23 / pmp->RT;
  a123 = WG123 / pmp->RT;

  // mole fractions
  X1 = pmp->X[jb] / pmp->XF[k];
  X2 = pmp->X[jb+1] / pmp->XF[k];
  X3 = pmp->X[jb+2] / pmp->XF[k];  // activity coeffs
  lnGam1 = a12 * X2 *( 1-X1 ) + a13 * X3 * ( 1-X1 ) - a23 * X2 * X3
           + a123 * X2 * X3 * ( 1 - 2.*X1 );
  lnGam2 = a23 * X3 *( 1-X2 ) + a12 * X1 * ( 1-X2 ) - a13 * X1 * X3
           + a123 * X1 * X3 * ( 1 - 2.*X2 );
  lnGam3 = a13 * X1 *( 1-X3 ) + a23 * X2 * ( 1-X3 ) - a12 * X1 * X2
           + a123 * X1 * X2 * ( 1 - 2.*X3 );
  // assignment
  pmp->lnGam[jb] = lnGam1;
  pmp->lnGam[jb+1] = lnGam2;
  pmp->lnGam[jb+2] = lnGam3;
  // Calculate excess volume, entropy and enthalpy !
  // To be done!
  Vex = WV12*X1*X2 + WV13*X1*X3 + WV23*X2*X3 + WV123*X1*X2*X3;
pmp->FVOL[k] += Vex*10.;
//  Sex = WS12*X1*X2 + WS13*X1*X3 + WS23*X2*X3 + WS123*X1*X2*X3;
//  Uex = WU12*X1*X2 + WU13*X1*X3 + WU23*X2*X3 + WU123*X1*X2*X3;
//  Hex = (WU12+P*WV12)*X1*X2 + (WU13+P*WV13)*X1*X3
//         + (WU23+P*WV23)*X2*X3 + (WU123+P*WV123)*X1*X2*X3;
}

//

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* Obsolete calculation of surface activity terms (Kulik, 1998, 2000, 2002)
* Revised by DAK on 03.Jan.2000 in Mainz ( cf. Kulik, 1999a,b )
*  to improve on IPM convergence at high SAT values.
* SAT are now placed into pmp->lnGam[j], as any activity coeffs
*  pmp->lnSAT vector is now used to keep original DUL[j] to restore
*  them after IPM-2 refinements for surface complexes.
*
void TProfil::SurfaceActivityTerm( int jb, int je, int k )
{
    int i, j, ist, Cj, iSite[6];
    double XS0,  xj0, XVk, XSk, XSkC, xj, Mm, rIEPS, ISAT, SAT,
           /* OSAT, * SATst, xjn, q1, q2;

    if( pmp->XFA[k] < pmp->DSM ) /* No sorbent retained by the IPM *
        return;
    if( pmp->XF[k]-pmp->XFA[k] < pmp->lowPosNum /* *10. * )
        return;  /* No surface species *

    for(i=0; i<6; i++)
        iSite[i] = -1;

    /* Extraction of site indices for neutral >OH groups *
    for( j=jb; j<je; j++ )
    {
        if( pmp->SATT[j] != SAT_SITE )
//        {
//            if( pmp->DCC[j] == DC_PEL_CARRIER || pmp->DCC[j] == DC_SUR_MINAL ||
//                    pmp->DCC[j] == DC_SUR_CARRIER ) continue;
//          ist = pmp->SATNdx[j][0] / MSPN; // MSPN = 2 - number of EDL planes
            continue;
//        }
        ist = pmp->SATNdx[j][0] / MSPN;
        iSite[ist] = j;
    }

    for( j=jb; j<je; j++ )
    { /* Loop for DC *
        if( pmp->X[j] <= pmp->lowPosNum /* *10. * )
            continue;  /* This surface DC has been killed by IPM *
//        OSAT = pmp->lnGmo[j]; // added 6.07.01 by KDA
        rIEPS = pa.p.IEPS;   // between 1e-8 and 1e-10 default 1e-9
        switch( pmp->DCC[j] )  /* code of species class *
        {
        default: /* pmp->lnGam[j] = 0.0; *
            continue;
        case DC_SSC_A0:
        case DC_SSC_A1:
        case DC_SSC_A2:
        case DC_SSC_A3:
        case DC_SSC_A4:
        case DC_WSC_A0:
        case DC_WSC_A1:
        case DC_WSC_A2:
        case DC_WSC_A3:
        case DC_WSC_A4:
        case DC_SUR_GROUP:
        case DC_IEWC_B:
        case DC_SUR_COMPLEX:
        case DC_SUR_IPAIR:
        case DC_IESC_A:
            /* Calculate ist - index of surface type *
            ist = pmp->SATNdx[j][0] / MSPN;
            /* Cj - index of carrier DC *
            Cj = pmp->SATNdx[j][1];
            if( Cj < 0 )
            {  /* Assigned to the whole sorbent *
                XVk = pmp->XFA[k];
                Mm = pmp->FWGT[k] / XVk;
            }
            else
            { /* Assigned to one of the sorbent end-members *
                XVk = pmp->X[Cj];
                if( XVk < pmp->DSM/10.0 )
                    continue; /* This end-member is zeroed off by IPM *
                Mm = pmp->MM[Cj] * XVk/pmp->XFA[k];  // mol.mass
            }
            XSk = pmp->XFTS[k][ist]; /* Tot.moles of sorbates on surf.type *
            xj = pmp->X[j];  /* Current moles of this surf.species *
//          a=1.0;    Frumkin factor - reserved for extension to FFG isotherm
            switch( pmp->SATT[j] )
            {
            case SAT_COMP: // Competitive surface species on a surface type
                /* a = fabs(pmp->MASDJ[j]); *
// rIEPS = pa.p.IEPS * 0.8;
                if( iSite[ist] < 0 )
                    xjn = 0.0;
                else xjn = pmp->X[iSite[ist]]; // neutral site does not compete!
                XS0 = pmp->MASDT[k][ist] * XVk * Mm / 1e6
                      * pmp->Nfsp[k][ist]; // expected total in moles
                if( pa.p.PC == 1 )
                    rIEPS = pa.p.IEPS * XS0;  // relative IEPS
                XSkC = XSk - xjn - xj; // occupied by the competing species;
	                             // this sorbate cannot compete to itself
                if( XSkC < 0.0 )
                    XSkC = 0.0;
                if( XSkC >= XS0 )  // Limits
                    XSkC = XS0 - 2.0 * rIEPS;
                xj0 = XS0 - XSkC;    // expected moles of this sorbate
                if(xj >= xj0)
                       xj = xj0 - rIEPS; // limits: 2 * rIEPS to XS0 - rIEPS
                if( xj * 2 <= xj0 )
                    ISAT = 0.0;      // ideal case
                else
                {
                   q1 = xj0 - xj;
                   q2 = rIEPS * XS0;
                   if( pa.p.PC == 2 && !pmp->W1 || pa.p.PC != 2 )
                   {
                      if( q1 > q2 )
                        q2 = q1;
                   }
                   else {
                      q2 = q1;
                      if( q2 <= 1e-33 )
                         q2 = 1e-33;
                   }
                   ISAT = log( xj ) - log( q2 );
                }
                // Test and compare with OSAT
/*                if( pmp->IT > 0 &&
                   ( ISAT < 0 || ISAT > 0-log(rIEPS) || fabs(ISAT-OSAT) > 3 ))
                {
cout << "IT=" << pmp->IT << ": Compt SAT for j= " << j << " old lnSAT= " << OSAT << " new lnSAT= " << ISAT << endl;
cout << "     x[j]= " << pmp->X[j] << " xj= " << xj << " xj0= " << xj0 << " XSkC= " << XSkC << " XSk= " << XSk << endl;
// ISAT = OSAT + 1;           // experiment
                }
*              pmp->lnGam[j] = ISAT;
                break;
            case SAT_NCOMP: /* Non-competitive surface species *
// rIEPS = pa.p.IEPS * 2;
                xj0 = fabs(pmp->MASDJ[j][PI_DEN]) * XVk * Mm / 1e6
                      * pmp->Nfsp[k][ist]; /* in moles *
                if( pa.p.PC == 1 )
                    rIEPS = pa.p.IEPS * xj0;  // relative IEPS
                if(xj >= xj0)
                     xj = xj0 - rIEPS;  // upper limit
                if( xj * 2.0 <= xj0 )  // Linear adsorption - to improve !
                    ISAT = 0.0;
                else
                {
                    q1 = xj0 - xj;  // limits: rIEPS to 0.5*xj0
                    q2 = xj0 * rIEPS;
                    if( pa.p.PC == 2 && pmp->W1 )
                       ISAT = log( xj ) - log( q1 );
                    else {
                       if( q1 > q2 )
                          ISAT = log( xj ) - log( q1 );
                       else
                          ISAT = log( xj ) - log( q2 );
                    }
                 }
//                    if( pa.p.PC == 2 && !pmp->W1 || pa.p.PC != 2 )
/*                  if( pa.p.PC == 2 && pmp->W1 )
                    {
                         q2 = q1;
                    }
                    else {
                       if( q1 > q2 )
                           q2 = q1;
                    }
                    if( q2 <= 1e-33 )
                        q2 = 1e-33;
                    ISAT = log( xj ) - log( q2 );
                }
*/                // Test and compare with OSAT
/*                if( pmp->IT > 0 &&
                  ( ISAT < 0 || ISAT > 0-log(rIEPS) || fabs(ISAT-OSAT) > 3 ))
                {
cout << "IT=" << pmp->IT << ": Ncomp SAT for j= " << j << " old lnSAT= " << OSAT << " new lnSAT= " << ISAT << endl;
cout << "     x[j]= " << pmp->X[j] << " xj= " << xj << " xj0= " << xj0 << " XSk= " << XSk << endl;
// ISAT = OSAT + 1;           // experiment
                }
*              pmp->lnGam[j] = ISAT;
                break;
            case SAT_SITE:  /* Neutral surface site (e.g. >O0.5H@ group) *
// rIEPS = pa.p.IEPS;
                XS0 = pmp->MASDT[k][ist] * XVk * Mm / 1e6
                      * pmp->Nfsp[k][ist]; /* in moles *
                if( pa.p.PC == 1 )
                    rIEPS = pa.p.IEPS * XS0;  // relative IEPS
                XSkC = XSk - xj;
                if( XSkC < 0.0 ) // potentially a serious error !
                    XSkC = 0.0;
//                if( XSkC >= XS0 )  // Limits
//                    XSkC = XS0 - 2.0 * rIEPS;
                if( pmp->MASDJ[j][PI_DEN] <= 0.0 )
                    SATst = pa.p.DNS*1.66054*pmp->Aalp[k]/
                            pmp->MASDT[k][ist];
                else SATst = pa.p.DNS*1.66054*pmp->Aalp[k]/pmp->MASDJ[j][PI_DEN];
//                                 pmp->MASDJ[j];
                if( fabs(XS0-XSkC) > XS0*rIEPS )
                {
                    if( XSkC > XS0 )  // case 1
                        SAT = 1.0;
                    else
                    {
                        SAT = xj/(XS0-XSkC);      // case 2
                        if( XSk > XS0 )
                            SAT *= XSk/(XS0-XSkC);    // case 3
                    }
                }
                else SAT = xj * XS0*rIEPS;     // boost ?????
/*                if( pmp->IT > 0 &&
                  ( SAT < 1e-2 || SAT > 1.0/rIEPS || fabs(log(SAT)+log(SATst)-OSAT) > 3 ) )
                {
cout << "IT=" << pmp->IT << ": NSite SAT for j= " << j << " old lnSAT= " << OSAT << " new lnSAT= " << ISAT << endl;
cout << "     x[jn]= " << pmp->X[j] << " XSk= " << XSk << " XSkC=" << XSkC << " XS0=" << XS0 << " SATst=" << SATst << endl;
//  ISAT = OSAT + 1;           // experiment
                }
*              if( SAT < 1e-2 )  // limits
                    SAT = 1e-2;  // to limit boosting
                if( SAT > 1.0/rIEPS )
                    SAT = 1.0/rIEPS;  // to limit from above
                pmp->lnGam[j] = log( SAT );
                pmp->lnGam[j] += log( SATst );
                break;
            case SAT_INDEF: /* No SAT calculation *
            default:        /* pmp->lnGam[j] = 0.0; *
                break;
            }
        }
    }  /* j *
}  */

// End of file ipm_gamma.cpp
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

