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
#include "visor.h"


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
        return;
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
     //   pm.U[ip] = STat->stp->U[is];
     //   pm.B[ip] = STat->stp->B[is];    // Added     //Deleted Sveta 10/02/05
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

//--------------------- End of ms_muleq.cpp ---------------------------

