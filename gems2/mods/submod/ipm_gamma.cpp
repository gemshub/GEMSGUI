//-------------------------------------------------------------------
// Id: gems/mods/submod/ipm_gamma.cpp
//      version 2.0.0                           edited 2001-09-08
//  Created: 1992-1997
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
#include "service.h"
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
        if( pmp->PHC[k] == PH_GASMIX || pmp->PHC[k] == PH_PLASMA )
        {
            char (*SMbuf)[MAXDCNAME] =
                (char (*)[MAXDCNAME])aObj[ o_w_tprn].Alloc( pmp->PG, 1, MAXDCNAME );
            memcpy(SMbuf,pmp->SM[jb], pmp->PG*MAXDCNAME*sizeof(char) );
            //     aObj[ o_w_tprn].SetPtr( pmp->SM[jb] );
            //     aObj[ o_w_tprn].SetDim( pmp->PG, 1 );
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

    for( j=jb; j<je; j++ )
    { /* cycle by DC. */
        Muj = DualChemPot( pmp->U, pmp->A+j*pmp->N, pmp->NR );
        pmp->Fx[j] = Muj * pmp->RT;     /* el-chem potential in J/mole */

        if( X[j] <= pmp->lowPosNum )
        { /* zeroing off */
            pmp->Wx[j] = 0.0;
            pmp->VL[j] = 0.0;
            pmp->Y_la[j] = ln_to_lg * ( Muj - pmp->G0[j] /* + Dsur */ );
            //      pmp->Y_la[j] = 0.0;  Log activity or fugacity
            pmp->Y_w[j] = 0.0;
            if( pmp->PHC[0] == PH_AQUEL )
                pmp->Y_m[j] = 0.0;
            continue;
        }
        if( pmp->PHC[0] == PH_AQUEL )
        {  /* mole fraction to molality conversion */
            if( !k ) lnFmol = log(1000./MMC);  // aq species
            else lnFmol = 4.016535; 	   // other species
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
            pmp->FVOL[k] += pmp->Vol[j]*SPmol;
            pmp->Y_m[j] = SPmol;
            pmp->Y_la[j] = ln_to_lg*(Muj - pmp->G0[j]
                                        + Dsur + lnFmol); //    Variant: Without Dsur?
            // obsolete        pmp->Y_la[j] = ln_to_lg* (log( SPmol ) + pmp->lnGam[j] );
            pmp->Y_w[j] =
                1e6 * X[j] * pmp->MM[j] / pmp->FWGT[k];
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
        case DC_SUR_SITE:
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
        case DC_WSC_A4:  /* case DC_SUR_SITE: */
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
            break; // Coulombic term to be considered !!!!!!!!!!
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
    int i, j, ist, jj;
    short k,ii;
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
                pmp->Fx[jj] *= pmp->RT;     /* el-chem potential */
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
        case PH_PLASMA:
        case PH_SIMELT:
        case PH_HCARBL:
        case PH_SINCOND:
        case PH_SINDIS:
        case PH_SOLUTION:
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
                    ist = pmp->SATNdx[jj][0] / MSPN;
                    pmp->XFTS[k][ist] += X[jj];
                }
            }
            pmp->logYFk = log(pmp->YFk);
            Dsur = XFA[k]/XF[k] - 1.0;  // Also for sorption phases
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

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* Multi-surface (ad)sorption phases   Brooklyn 5/15/97 DAK */
/* Updated Mainz 5 Jan 2000 Mainz by DAK */
/* Calculation of surface charge */
void TProfil::IS_EtaCalc()
{
    int k, i, ist, isp, j=0;
    double XetaS=0., XetaW=0.,  Ez;
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
            case DC_SUR_SITE:
            case DC_SUR_COMPLEX:
            case DC_SUR_IPAIR:
            case DC_IESC_A:
            case DC_IEWC_B: /* Get ist - index of surface type
                                      and isp - index of surface plane  */
                ist = pmp->SATNdx[j][0] / MSPN;
                isp = pmp->SATNdx[j][0] % MSPN;
                if( !isp )
                { /* This is A (0) plane */
                    pmp->XetaA[k][ist] += pmp->X[j]*pmp->EZ[j];
                }
                else
                { /* This is B plane */
                    Ez = pmp->EZ[j];
                    if( pmp->SCM[k][ist] == SC_MTL )
                    { /* Modified TL: Robertson, 1997 */
                        pmp->XetaB[k][ist] += pmp->X[j]*Ez;
                    }
                    else if( pmp->SCM[k][ist] == SC_TLM )
                    { /* TLM Hayes & Leckie, 1987 */
                        if( pmp->MASDJ[j] < 0.0 )
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
                    else if( pmp->SCM[k][ist] == SC_BSM )
                    { /* Basic Stern model Christl & Kretzschmar, 1999 */
                        if( pmp->MASDJ[j] < 0.0 )
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
    double SigA=0., SigD=0., PSIo, XetaA[MST], XetaB[MST], f1, f3;
    double A=1e-9, Sig, F2RT, I, Cap, PsiD, PsiB, PsiA, Ro = 1.0;
    /* Del, F=F_CONSTANT, Cap0; */
    if( pmp->XF[k] < pa.p.ScMin )
        return; /* no sorbent */

    /* sorbent mass in grams */
    pmp->YFk = pmp->FWGT[k];

    for( ist=0; ist<pmp->FIat; ist++ ) /* cycle by surface types */
    {
        if( pmp->SCM[k][ist] == SC_NOT_USED || pmp->Nfsp[k][ist] < 1e-9  )
            continue;
        /* Calculation of charge densities */
        if( fabs( pmp->XetaA[k][ist]) >1e-9 )
            XetaA[ist] = pmp->XetaA[k][ist]*F_CONSTANT/pmp->YFk/pmp->Aalp[k]
                         /pmp->Nfsp[k][ist]; /* C/m2 */
        else XetaA[ist] = 0.0;
        if( fabs( pmp->XetaB[k][ist]) >1e-9 )/* moles */
            XetaB[ist] = pmp->XetaB[k][ist] *F_CONSTANT/pmp->YFk/pmp->Aalp[k]
                         /pmp->Nfsp[k][ist]; /* C/m2 */
        else XetaB[ist] = 0.0;
        /* Limiting charge densities to 2 C/m2 */
        if( fabs(XetaA[ist]) > 2.0 )
            XetaA[ist] = XetaA[ist]<0.0?-2.0:2.0;
        if( fabs(XetaB[ist]) > 2.0 )
            XetaB[ist] = XetaB[ist]<0.0?-2.0:2.0;

        PsiD=0.0;
        PSIo=0.0;
        PsiA=0.0;
        PsiB=0.0;
        if( fabs( XetaA[ist] ) < 1e-7 )
            goto GEMU_CALC;  /* skipping at near-zero charge */

        /* calculating charge density at diffuse layer */
        switch( pmp->SCM[k][ist] )
        {
        case SC_CCM:  /* Constant-Capacitance Model Schindler */
            SigA = pmp->Xetaf[k][ist] + XetaA[ist];
            SigD = -SigA;
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
        case SC_MXC:  /* Ion-Exchange without SC on const.charge surface */
            SigA = pmp->Xetaf[k][ist];
            SigD = -SigA;
            break;
        case SC_NNE:  /* Non-Electrostatic Sorption */
            SigA = 0;
            SigD = 0;
            break;
        default:
            continue;
        }
        /* Gouy-Chapman equation */
        /* params of diffuse layer using Damaskin, 1987,p.192-195 */
        F2RT = pmp->FRT / 2.;
        Sig = SigD;
        I=pmp->IC;
        if( I > 1e-7 )
            /* Aq solution density Ro included acc. to Machesky ea., 1999 */
            /* Only for the basic Stern model for now (Jan 5, 2000 DAK)! */
            { if( pmp->SCM[k][ist] == SC_BSM && pmp->FVOL[0] > 1e-16 )
                Ro = pmp->FWGT[0] / pmp->FVOL[0];
            else Ro = 1.0;
            A = sqrt( 2000. * 8.854e-12 * pmp->epsW * pmp->RT * I * Ro );
        }
        Cap = F2RT * sqrt( 4.*A*A + Sig*Sig );

        /* Sveta: workaround because of problems with log argument */
        f3 =  sqrt( 1.+Sig*Sig/(4.*A*A) ) - Sig/(2.*A);
        //cout<< f1 << ' '<< f2 << ' ' << f3 << endl;
        if( f3 < 1 )
        {
            f1 = exp( -3 * F2RT);
            if( f3<f1) f3 = f1;
        }
        else
        {
            f1 = exp( 3*F2RT );
            if( f3>f1 ) f3 = f1;
        }
        PSIo = log(f3)/F2RT;
        //  PSIo = log( sqrt( 1.+Sig*Sig/(4.*A*A) ) - Sig/(2.*A) ) / F2RT;

        /*  Cap0 = fabs(Sig/PSIo);
            Del = A*1e9/(2.*I*F)/cosh(PSIo*F2RT);
            pmp->XdlD[k] = Del;
        */
        pmp->XcapD[k][ist] = Cap;
        pmp->XpsiD[k][ist] = PSIo;
        PsiD = PSIo; /* PsiA = PsiB = 0; */

GEMU_CALC:
        /* calculating potentials at EDL planes */
        switch( pmp->SCM[k][ist] )
        {
        case SC_CCM:  /* Constant-Capacitance Model   Schindler 1973 */
            PsiA = SigA / pmp->XcapA[k][ist];
            pmp->XpsiA[k][ist] = PsiA;
            break;
        case SC_MTL:  /* Modified Triple Layer Model  Robertson 1996 */
            PsiB = PsiD - SigD / pmp->XcapB[k][ist];
            if( fabs( PsiB ) > 1.0 )  /* Cutoff potential */
                PsiB = PsiB<0? -1.0: 1.0;
            PsiA = PsiB + SigA / pmp->XcapA[k][ist];
            if( fabs( PsiA ) > 1.0 )
                PsiA = PsiA<0? -1.0: 1.0;
            pmp->XpsiA[k][ist] = PsiA;
            pmp->XpsiB[k][ist] = PsiB;
            break;
        case SC_TLM:  /* Triple-Layer Model   Hayes 1987 */
            PsiB = PsiD - SigD / pmp->XcapB[k][ist];
            if( fabs( PsiB ) > 1.0 ) /* Cutoff potential */
                PsiB = PsiB<0? -1.0: 1.0;
            PsiA = PsiB + SigA / pmp->XcapA[k][ist];
            if( fabs( PsiA ) > 1.0 )
                PsiA = PsiA<0? -1.0: 1.0;
            pmp->XpsiA[k][ist] = PsiA;
            pmp->XpsiB[k][ist] = PsiB;
            break;
        case SC_BSM: /* Basic Stern model, Christl & Kretzschmar, 1999 */
            PsiB = PsiD;
            if( fabs( PsiB ) > 1.0 ) /* Cutoff potential */
                PsiB = PsiB<0? -1.0: 1.0;
            PsiA = PsiB + SigA / pmp->XcapA[k][ist];
            if( fabs( PsiA ) > 1.0 )
                PsiA = PsiA<0? -1.0: 1.0;
            pmp->XpsiA[k][ist] = PsiA;
            pmp->XpsiB[k][ist] = PsiB;
            break;
        case SC_DDLM: /* Diffuse Double Layer Model  Dzombak & Morel 1990 */
            pmp->XpsiA[k][ist] = PsiD;
            pmp->XpsiB[k][ist] = PsiD;
            break;
        case SC_MXC:  /* Ion-Exchange on const.charge surface */
            //PsiA = PsiD;
            pmp->XpsiA[k][ist] = PsiD;
            break;
        case SC_NNE:  /* Non-Electrostatic Sorption */
        default:
            break;
        }
        /* phases */
    }
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* Calculation of surface activity terms (Kulik & Aja, 1997)
* Revised by DAK on 03.Jan.2000 in Mainz ( cf. Kulik, 1999a,b )
*   to improve on IPM convergence at high SAT values.
* SAT are now placed into pmp->lnGam[j], as any activity coeffs
*   pmp->lnSAT vector is now obsolete */
void TProfil::SurfaceActivityTerm( int jb, int je, int k )
{
    int i, j, ist, Cj, iSite[6];
    double XS0,  xj0, XVk, XSk, XSkC, xj, Mm, ISAT=0.0, SAT=1.0,
            OSAT=0.0, SATst, a, xjn, q1, q2;

    if( pmp->XFA[k] < pmp->DSM ) /* No sorbent retained by the IPM */
        return;
    if( pmp->XF[k]-pmp->XFA[k] < pmp->lowPosNum /* *10. */ )
        return;  /* No surface species */

    for(i=0; i<6; i++)
        iSite[i] = -1;

    /* Extraction of site indices */
    for( j=jb; j<je; j++ )
    {
        if( pmp->SATT[j] != SAT_SITE )
        {
            if( pmp->DCC[j] == DC_PEL_CARRIER || pmp->DCC[j] == DC_SUR_MINAL ||
                    pmp->DCC[j] == DC_SUR_CARRIER ) continue;
            ist = pmp->SATNdx[j][0] / MSPN; // MSPN = 2 - number of EDL planes
            continue;
        }
        ist = pmp->SATNdx[j][0] / MSPN;
        iSite[ist] = j;
    }

    for( j=jb; j<je; j++ )
    { /* Loop for DC */
        if( pmp->X[j] <= pmp->lowPosNum /* *10. */ )
            continue;  /* This surface DC has been killed by IPM */
        OSAT = pmp->lnGmo[j]; // added 6.07.01 by KDA
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
        case DC_SUR_SITE:
        case DC_IEWC_B:
        case DC_SUR_COMPLEX:
        case DC_SUR_IPAIR:
        case DC_IESC_A:
            /* Calculate ist - index of surface type */
            ist = pmp->SATNdx[j][0] / MSPN;
            /* Cj - index of carrier DC */
            Cj = pmp->SATNdx[j][1];
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
            XSk = pmp->XFTS[k][ist]; /* Tot.moles of sorbates on surf.type */
            xj = pmp->X[j];  /* Current moles of this surf.species */
            a=1.0; /* Frumkin factor - reserved for extension to FFG isotherm */
            switch( pmp->SATT[j] )
            {
            case SAT_COMP: /* Competitive surface species on a surface type */
                /* a = fabs(pmp->MASDJ[j]); */
                if( iSite[ist] < 0 )
                    xjn = 0.0;
                else xjn = pmp->X[iSite[ist]]; // neutral site does not compete!
                XS0 = pmp->MASDT[k][ist] * XVk * Mm / 1e6
                      * pmp->Nfsp[k][ist]; /* expected total in moles */
                XSkC = XSk - xjn - xj; /* occupied by the competing species;
                		                            this sorbate cannot compete to itself */
                /* New variant */
                if( XSkC < pa.p.IEPS )
                    XSkC = pa.p.IEPS;
                xj0 = XS0 - XSkC;    /* expected moles of this sorbate */
                if( xj0 < pa.p.IEPS )
                    xj0 = pa.p.IEPS;  /* ensuring that it is non-negative */
                if( xj * 2 <= xj0 )
                    ISAT = 0.0;
                else
                {
                    q1 = xj0 - xj;
                    if( q1 > pa.p.IEPS )
                        q2 = log( q1 );
                    else q2 = log( pa.p.IEPS );
                    ISAT = log( xj ) - q2;
                    /*                     q1 = ( XS0 - XSkC + xj ) / xj;
                                           if( q1 > 1.0 )
                                           {  q2 = q1;               // How did it work?
                                              ISAT = q2<=1.0? q2-1.0: 1.0;   }
                                           else ISAT = pa.p.IEPS;
                                           pmp->lnGam[j] = -log( ISAT );
                    */
                }
                pmp->lnGam[j] = ISAT;
                break;
            case SAT_NCOMP: /* Non-competitive surface species */
                xj0 = fabs(pmp->MASDJ[j]) * XVk * Mm / 1e6
                      * pmp->Nfsp[k][ist]; /* in moles */
                if( xj * 2 <= xj0 )    // Linear adsorption
                    ISAT = 0.0;
                else
                {
                    q1 = xj0 - xj;
                    q2 = xj0 * pa.p.IEPS; // eps6
                    if( q1 > q2 )
                       ISAT = log( xj ) - log( q1 );
                    else
                    {             // What to do ?
                       ISAT = log( xj ) - log( q2 );
                       if( ISAT < OSAT )
                           ISAT -= log(pa.p.IEPS);
                    }   // 10 lines above fixed by KDA 6.07.01
                }   
                pmp->lnGam[j] = ISAT;
                break;
            case SAT_SITE:  /* Neutral surface site (e.g. >O0.5H@ group) */
                XSkC = XSk - xj;
                XS0 = pmp->MASDT[k][ist] * XVk * Mm / 1e6
                      * pmp->Nfsp[k][ist]; /* in moles */
                if( pmp->MASDJ[j] <= 0.0 )
                    SATst = pa.p.DNS*1.66054*pmp->Aalp[k]/
                            pmp->MASDT[k][ist];
                else SATst = pa.p.DNS*1.66054*pmp->Aalp[k]/
                                 pmp->MASDJ[j];
                if( fabs(XS0-XSkC) > pa.p.IEPS /* 1e-14 */ )
                {
                    if( XSkC > XS0 )
                        SAT = 1.0;
                    else
                    {
                        SAT = xj/(XS0-XSkC);
                        if( XSk > XS0 )
                            SAT *= XSk/(XS0-XSkC);
                    }
                }
                else SAT = xj * pa.p.IEPS /* 1e14 */ ;
                if( SAT < 1e-2 /* pa.p.IEPS */ )
                    SAT = 1e-2; /* pa.p.IEPS; to allow killing */
                if( SAT > 1.0/pa.p.IEPS )
                    SAT = 1.0/pa.p.IEPS;
                pmp->lnGam[j] = log( SAT );
                pmp->lnGam[j] += log( SATst );
                break;
            case SAT_INDEF: /* No SAT calculation */
            default:        /* pmp->lnGam[j] = 0.0; */
                break;
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
    int ist, isp, jc=0;
    double F0=0.0, Fold, dF0, Mk=0.0, Ez;

    Fold = pmp->F0[j];
    if( pmp->FIat > 0 )
        jc = pmp->SATNdx[j][1];
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
    case DC_SUR_SITE:
    case DC_SUR_COMPLEX:
    case DC_SUR_IPAIR:
    case DC_IESC_A:
    case DC_IEWC_B:
        F0 = pmp->lnGmM[j]; /* + pmp->lnGam[j]; */
        /* get ist - index of surface type and isp - index of surface plane  */
        ist = pmp->SATNdx[j][0] / MSPN;
        isp = pmp->SATNdx[j][0] % MSPN;
        if( !isp )  /* This is  A (0) plane */
            F0 += pmp->XpsiA[k][ist]*pmp->EZ[j]*pmp->FRT;
        else  /* This is B plane */
        {
            Ez = pmp->EZ[j];
            if( pmp->SCM[k][ist] == SC_TLM )
            { /* TLM model after Hayes, 1987,
                		           see also Table 4 in Zachara & Westall, 1999 */
                if( pmp->MASDJ[j] < 0 )
                {
                    Ez -= 1.0;
                    F0 += (pmp->XpsiA[k][ist]+Ez*pmp->XpsiB[k][ist])
                          *pmp->FRT;
                }
                else
                {
                    Ez += 1.0;
                    F0 += (Ez*pmp->XpsiB[k][ist]-pmp->XpsiA[k][ist])
                          *pmp->FRT;
                }
            }
            else if( pmp->SCM[k][ist] == SC_BSM )
            { /* Basic Stern model Christl & Kretzschmar, 1999 */
                if( pmp->MASDJ[j] < 0 )
                {
                    Ez -= 1.0;
                    if( pmp->DCC[j] == DC_SUR_IPAIR ) // Doubtful!
                        F0 += Ez*pmp->XpsiB[k][ist] * pmp->FRT;
                    else
                        F0 += (pmp->XpsiA[k][ist]+Ez*pmp->XpsiB[k][ist])
                              *pmp->FRT;
                }
                else
                {
                    Ez += 1.0;
                    if( pmp->DCC[j] == DC_SUR_IPAIR ) // Doubtful!
                        F0 += Ez*pmp->XpsiB[k][ist] * pmp->FRT;
                    else
                        F0 += (Ez*pmp->XpsiB[k][ist]-pmp->XpsiA[k][ist])
                              *pmp->FRT;
                }		             ;
            }
            else if( pmp->SCM[k][ist] == SC_MTL )
            { /* Modified TLM after Robertson, 1996 */
                F0 += pmp->XpsiB[k][ist] * Ez * pmp->FRT;
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
    aObj[ o_nqp].SetPtr( pmp->Qp+k*20 );
    aObj[ o_nqd].SetPtr( pmp->Qd );      /* 20 cells */
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

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* Aqueous electrolyte                        Brooklyn 5/15/97 DAK */
/* Calculation of individual activity coefficients as proposed by
* I.K.Karpov using DH(III) with Kielland' a0 parameters and common
* 3rd parameter (HKF81) */
void TProfil::DebyeHueckel3HelKarp( int jb, int je, int jpb, int jdb, int k )
{
    int j;
    double T, A, B, a0, I, sqI, bg, bgi, Z2, lgGam;

    I= pmp->IC;
    if( I < pa.p.ICmin )
        return;
    T=pmp->Tc;
    A= pmp->PMc[jpb+0];
    B= pmp->PMc[jpb+1];
    bg=pmp->PMc[jpb+5];
    sqI = sqrt( I );
    if( fabs(A) < 1e-9 )
        A = 1.82483e6 * sqrt( tpp->RoW ) / pow( T*tpp->EpsW, 1.5 );
    if( fabs(B) < 1e-9 )
        B = 50.2916 * sqrt( tpp->RoW ) / sqrt( T*tpp->EpsW );
    ErrorIf( fabs(A) < 1e-9 || fabs(B) < 1e-9, "DebyeHueckel3HelKarp",
             "Error - No values of RoW and EpsW !" );
    /* Calculation of DH equation */
    bgi = bg;
    for( j=jb; j<je; j++ )
    {
        a0 = pmp->DMc[jdb+j*pmp->LsMdc[k]];
        if( pmp->LsMdc[k] > 1 )
        { /* Individual bg coeff Truesdell-Jones (Parkhurst,1990) */
            bgi = pmp->DMc[jdb+j*pmp->LsMdc[k]+1];
            if( !bgi )
                bgi = bg;
        }
        if( pmp->EZ[j] )
        { /* Charged species */
            Z2 = pmp->EZ[j]*pmp->EZ[j];
            lgGam = ( -A * sqI * Z2 ) / ( 1 + B * a0 * sqI ) + bgi * I ;
        }
        else
        { /* Neutral species */
            if( a0 > 0 )
            {
                if( pmp->DCC[j] != DC_AQ_SOLVENT ) /* Setchenow coefficient */
                    lgGam = a0 * I;
                else /* water-solvent - a0 - osmotic coefficient */
                    lgGam = a0 * I; /* correct: instead of I - sum.molality */
            }
            else
            {
                if( a0 < -0.99 )
                    lgGam = 0;
                else if( fabs( a0 ) < 1e-9 )
                    lgGam = bgi * I;  /* Average Setchenow coeff. */
                else lgGam = a0 * I;  /* Busenberg & Plummer */
            }
        }
        pmp->lnGam[j] = lgGam * lg_to_ln;
    } /* j */
}

static double ICold=0.;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* Calculation of activity coefficient vector (lnGam[])
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
    double LnGam;
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
            if( pmp->E )
                IS_EtaCalc( );
//          pmp->IC = max( pmp->MOL, pmp->IC );
            pmp->IC = 0.0;  // Important for simplex FIA reproducibility
            if( pmp->FIat > 0 )
            {
                pmp->XetaA[0][0] = 0.0;
                pmp->XetaB[0][0] = 0.0;
            }
        }
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
        if( pmp->LO && pmp->Lads )
        {
            ConCalc( pmp->X, pmp->XF, pmp->XFA  );
            if( pmp->E )
                IS_EtaCalc();
        }
        pmp->FitVar[3] = 1.0;
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
            IS_EtaCalc();
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

        jpb = jpe;
        jpe += pmp->LsMod[k];
        jdb = jde;
        jde += pmp->LsMdc[k]*pmp->L1[k];
        memset( pmp->Qd, 0, sizeof(double)*20 );

        switch( pmp->PHC[k] )
        {   /* calculate activity coefficients by built-in functions */
        case PH_AQUEL:   /*calc by DH III appr. HKF */
            if( sMod[SGM_MODE] == SM_STNGAM )
            {
                if( pmp->XF[k] > pa.p.XwMin &&
                        pmp->IC > pa.p.ICmin )
                {
                    DebyeHueckel3HelKarp(  jb, je, jpb, jdb, k );
                    /* Pitzer( q, jb, je, k ); */
                }
                goto END_LOOP; /* break; */
            }
            else if( sMod[SGM_MODE] == SM_IDEAL )
                goto END_LOOP;
            break;
        case PH_POLYEL:  /* PoissonBoltzmann( q, jb, je, k ) break; */
        case PH_SORPTION: /* calc elstatic potenials from Gouy-Chapman eqn */
            if( pmp->PHC[0] == PH_AQUEL &&
                    pmp->XF[k] > pmp->DSM &&
                    pmp->XF[0] > pa.p.XwMin )
            {
                GouyChapman(   jb, je, k );
                /* PoissonBoltzmann( q, jb, je, k ) */
                /* Calculate surface activity coeffs */
                SurfaceActivityTerm(  jb, je, k );
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
                case SM_PRIVATE: /* separate equations for species */
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
                case SM_PRIVATE:  /* separate group of equations for each species */
                    multi->qEd[k].CalcEquat();
                    break;
                }
            }
            break;
        default:
            Error("GammaCalc","Illegal LinkMode 2");
        }
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

// End of file ipm_gamma.cpp
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

