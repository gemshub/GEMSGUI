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
            STat->stp->llf[i] = DCon[pm.muj[j]];
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
            STat->stp->phf[i] = PHon[pm.muk[j]];
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
        pm.U[ip] = STat->stp->U[is];
        pm.B[ip] = STat->stp->B[is];    // Added
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


// set old data dynamic memory
void TMulti::dyn_new_test(MULTI& tes)
{
  // Part 1
 if( flCopy == true )
   dyn_kill_test(tes);
 memset(&tes, 0, sizeof(MULTI));
 memcpy( &tes.N, &pm.N, 36*sizeof(short) );
 memcpy( &tes.TC, &pm.TC, 54*sizeof(double) );

 // tes.A = (float *)aObj[ o_wi_a].Alloc( tes.L, tes.N, F_ );
// tes.SB = (char (*)[MAXICNAME+MAXSYMB])aObj[ o_wd_sb].Alloc(
//    tes.N, 1, MAXICNAME+MAXSYMB );
// Error in free memory
//aObj[ o_w_sbh].SetPtr( tes.SB ); aObj[ o_w_sbh ].SetDim( tes.N, 1 );
// tes.SFs = (char (*)[MAXPHNAME+MAXSYMB])aObj[ o_wd_sfs].Alloc(
//    tes.FI, 1, MAXPHNAME+MAXSYMB );
// tes.SM = (char (*)[MAXDCNAME])aObj[ o_wd_sm].Alloc( tes.L, 1, MAXDCNAME );
// tes.SF = (char (*)[MAXPHNAME+MAXSYMB])aObj[ o_wd_sf].Alloc(
//     tes.FI, 1, MAXPHNAME+MAXSYMB );
// tes.SM2 = (char (*)[MAXDCNAME])aObj[ o_wd_sm2].Alloc( tes.Ls, 1, MAXDCNAME );
// tes.SF2 = (char (*)[MAXPHNAME+MAXSYMB])aObj[ o_wd_sf2].Alloc(
//     tes.FIs, 1, MAXPHNAME+MAXSYMB );

// tes.RLC = (char *)aObj[ o_wi_rlc].Alloc( tes.L, 1, A_ );
// tes.RSC = (char *)aObj[ o_wi_rsc].Alloc( tes.L, 1, A_ );
// tes.ICC = (char *)aObj[ o_wi_icc].Alloc( tes.N, 1, A_ );
// tes.DCC = (char *)aObj[ o_wi_dcc].Alloc( tes.L, 1, A_ );
// tes.PHC = (char *)aObj[ o_wi_phc].Alloc( tes.FI, 1, A_ );
// tes.DCCW = (char *)aObj[ o_wi_dccw].Alloc( tes.L, 1, A_ );

 // need  always to alloc vectors
 tes.L1 = new short[pm.FI];
 memcpy( tes.L1, pm.L1, pm.FI*sizeof(short) );
 tes.muk = new short[pm.FI];
 memcpy( tes.muk, pm.muk, pm.FI*sizeof(short) );
 tes.mui = new short[pm.N];
 memcpy( tes.mui, pm.mui, pm.N*sizeof(short) );
 tes.muj = new short[pm.L];
 memcpy( tes.muj, pm.muj, pm.L*sizeof(short) );

 tes.DUL = new double[pm.L];
 memcpy( tes.DUL, pm.DUL, pm.L*sizeof(double) );
 tes.DLL = new double[pm.L];
 memcpy( tes.DLL, pm.DLL, pm.L*sizeof(double) );
 tes.Vol = new double[pm.L];
 memcpy( tes.Vol, pm.Vol, pm.L*sizeof(double) );
 tes.Pparc = new double[pm.L];
 memcpy( tes.Pparc, pm.Pparc, pm.L*sizeof(double) );
 tes.MM = new double[pm.L];
 memcpy( tes.MM, pm.MM, pm.L*sizeof(double) );
 tes.Awt = new float[pm.N];
 memcpy( tes.Awt, pm.Awt, pm.N*sizeof(float) );
 tes.XFs = new float[pm.FI];
 memcpy( tes.XFs, pm.XFs, pm.FI*sizeof(float) );
 tes.Falps = new float[pm.FI];
 memcpy( tes.Falps, pm.Falps, pm.FI*sizeof(float) );
 tes.G = new double[pm.L];
 memcpy( tes.G, pm.G, pm.L*sizeof(double) );
 tes.G0 = new double[pm.L];
 memcpy( tes.G0, pm.G0, pm.L*sizeof(double) );
 tes.lnGam = new double[pm.L];
 memcpy( tes.lnGam, pm.lnGam, pm.L*sizeof(double) );
 tes.lnGmo = new double[pm.L];
 memcpy( tes.lnGmo, pm.lnGmo, pm.L*sizeof(double) );
 tes.B = new double[pm.N];
 memcpy( tes.B, pm.B, pm.N*sizeof(double) );
 tes.U = new double[pm.N];
 memcpy( tes.U, pm.U, pm.N*sizeof(double) );
 tes.U_r = new double[pm.N];
 memcpy( tes.U_r, pm.U_r, pm.N*sizeof(double) );
 tes.C = new double[pm.N];
 memcpy( tes.C, pm.C, pm.N*sizeof(double) );
 tes.XF = new double[pm.FI];
 memcpy( tes.XF, pm.XF, pm.FI*sizeof(double) );
 tes.YF = new double[pm.FI];
 memcpy( tes.YF, pm.YF, pm.FI*sizeof(double) );
 tes.Falp = new double[pm.FI];
 memcpy( tes.Falp, pm.Falp, pm.FI*sizeof(double) );
 tes.X = new double[pm.L];
 memcpy( tes.X, pm.X, pm.L*sizeof(double) );
 tes.Y = new double[pm.L];
 memcpy( tes.Y, pm.Y, pm.L*sizeof(double) );
 tes.XY = new double[pm.L];
 memcpy( tes.XY, pm.XY, pm.L*sizeof(double) );
 tes.MU = new double[pm.L];
 memcpy( tes.MU, pm.MU, pm.L*sizeof(double) );
 tes.EMU = new double[pm.L];
 memcpy( tes.EMU, pm.EMU, pm.L*sizeof(double) );
 tes.NMU = new double[pm.L];
 memcpy( tes.NMU, pm.NMU, pm.L*sizeof(double) );
 tes.W = new double[pm.L];
 memcpy( tes.W, pm.W, pm.L*sizeof(double) );
 tes.F = new double[pm.L];
 memcpy( tes.F, pm.F, pm.L*sizeof(double) );
 tes.F0 = new double[pm.L];
 memcpy( tes.F0, pm.F0, pm.L*sizeof(double) );
 tes.YOF = new double[pm.FI];
 memcpy( tes.YOF, pm.YOF, pm.FI*sizeof(double) );
 tes.lnGmM = new double[pm.L];
 memcpy( tes.lnGmM, pm.lnGmM, pm.L*sizeof(double) );
 tes.GEX = new double[pm.L];
 memcpy( tes.GEX, pm.GEX, pm.L*sizeof(double) );
 tes.FVOL = new double[pm.FI];
 memcpy( tes.FVOL, pm.FVOL, pm.FI*sizeof(double) );
 tes.FWGT = new double[pm.FI];
 memcpy( tes.FWGT, pm.FWGT, pm.FI*sizeof(double) );

 if( tes.L > 0 )
 {
   tes.Y_la = new double[pm.L];
   memcpy( tes.Y_la, pm.Y_la, pm.L*sizeof(double) );
   tes.Y_w = new double[pm.L];
   memcpy( tes.Y_w, pm.Y_w, pm.L*sizeof(double) );
   tes.Fx = new double[pm.L];
   memcpy( tes.Fx, pm.Fx, pm.L*sizeof(double) );
   tes.Wx = new double[pm.L];
   memcpy( tes.Wx, pm.Wx, pm.L*sizeof(double) );
   tes.VL = new float[pm.L];
   memcpy( tes.VL, pm.VL, pm.L*sizeof(float) );
   tes.Gamma = new double[pm.L];
   memcpy( tes.Gamma, pm.Gamma, pm.L*sizeof(double) );
   tes.lnGmf = new double[pm.L];
   memcpy( tes.lnGmf, pm.lnGmf, pm.L*sizeof(double) );
   tes.D = new double[pm.L];
   memcpy( tes.D, pm.D, pm.L*sizeof(double) );
 }

   // Part 2  not requited arrays

 if( tes.FIs > 0 && tes.Ls > 0 )
 {
   tes.BF = new double[pm.FIs*pm.N];
   memcpy( tes.BF, pm.BF, pm.FIs*pm.N*sizeof(double) );
   tes.XFA = new double[pm.FIs];
   memcpy( tes.XFA, pm.XFA, pm.FIs*sizeof(double) );
   tes.YFA = new double[pm.FIs];
   memcpy( tes.YFA, pm.YFA, pm.FIs*sizeof(double) );
//     tes.LsMod = (short *)aObj[ o_wi_lsmod].Alloc( tes.FIs, 1, I_ );
//     tes.LsMdc = (short *)aObj[ o_wi_lsmdc].Alloc( tes.FIs, 1, I_ );
//    PMs and DMS may be realloced after load arrays LsMod and LsMdc
//   tes.PMc = new float[pm.FIs];
//   memcpy( tes.PMc, pm.PMc, pm.FIs*sizeof(float) );
//   tes.DMc = new float[pm.Ls];
//   memcpy( tes.DMc, pm.DMc, pm.Ls*sizeof(float) );
   tes.PUL = new double[pm.FIs];
   memcpy( tes.PUL, pm.PUL, pm.FIs*sizeof(double) );
   tes.PLL = new double[pm.FIs];
   memcpy( tes.PLL, pm.PLL, pm.FIs*sizeof(double) );
//     tes.YOF = (float *)aObj[ o_wi_yof].Alloc( tes.FI, 1, F_ );
//     tes.sMod = (char (*)[6])aObj[ o_wi_smod].Alloc( tes.FIs, 1, 6 );
//     tes.RFLC = (char *)aObj[ o_wi_rflc].Alloc( tes.FIs, 1, A_ );
//     tes.RFSC = (char *)aObj[ o_wi_rfsc].Alloc( tes.FIs, 1, A_ );
 }

 if( tes.LO > 1 )
 {
   tes.Y_m = new double[pm.L];
   memcpy( tes.Y_m, pm.Y_m, pm.L*sizeof(double) );
   tes.IC_m = new double[pm.N];
   memcpy( tes.IC_m, pm.IC_m, pm.N*sizeof(double) );
   tes.IC_lm = new double[pm.N];
   memcpy( tes.IC_lm, pm.IC_lm, pm.N*sizeof(double) );
   tes.IC_wm = new double[pm.N];
   memcpy( tes.IC_wm, pm.IC_wm, pm.N*sizeof(double) );
 }
 // dispersion and sorbtion phases
 if( syp->PAalp != S_OFF )
 {
   tes.Aalp = new float[pm.FI];
   memcpy( tes.Aalp, pm.Aalp, pm.FI*sizeof(float) );
   tes.Xr0h0 = new float[pm.FI][2];
   memcpy( tes.Xr0h0, pm.Xr0h0, pm.FI*2*sizeof(float) );
 }
 if( syp->PSigm != S_OFF )
 {    tes.Sigw = new float[pm.FI];
     memcpy( tes.Sigw, pm.Sigw, pm.FI*sizeof(float) );
 }
 if( syp->PSigm != S_OFF )
 {    tes.Sigg = new float[pm.FI];
     memcpy( tes.Sigg, pm.Sigg, pm.FI*sizeof(float) );
 }
 if( tes.E )
 {
    tes.EZ = new double[pm.L];
     memcpy( tes.EZ, pm.EZ, pm.L*sizeof(double) );
    tes.Xcond = new float[pm.FI];
     memcpy( tes.Xcond, pm.Xcond, pm.FI*sizeof(float) );
    tes.Xeps = new float[pm.FI];
     memcpy( tes.Xeps, pm.Xeps, pm.FI*sizeof(float) );
 }

 if( tes.FIat > 0 && tes.Lads > 0 && tes.FIs > 0 )
 { // ADSORBTION AND ION EXCHANGE
//   tes.SATNdx = (short (*)[2])aObj[ o_wi_satndx].Alloc( tes.Ls, 2, I_ );
//   tes.SCM  = (char (*)[MST])aObj[ o_wi_scm].Alloc( tes.FIs, tes.FIat, A_ );
//   memset( tes.SCM, SC_NOT_USED, tes.FIs*tes.FIat );
    tes.Nfsp = new float[pm.FIs][6];
     memcpy( tes.Nfsp, pm.Nfsp, pm.FIs*6*sizeof(float) );
    tes.MASDT = new float[pm.FIs][6];
     memcpy( tes.MASDT, pm.MASDT, pm.FIs*6*sizeof(float) );
    tes.XcapA = new float[pm.FIs][6];
     memcpy( tes.XcapA, pm.XcapA, pm.FIs*6*sizeof(float) );
    tes.XcapB = new float[pm.FIs][6];
     memcpy( tes.XcapB, pm.XcapB, pm.FIs*6*sizeof(float) );
    tes.XcapD = new float[pm.FIs][6];
     memcpy( tes.XcapD, pm.XcapD, pm.FIs*6*sizeof(float) );
    tes.XcapF = new float[pm.FIs][6];
     memcpy( tes.XcapF, pm.XcapF, pm.FIs*6*sizeof(float) );
    tes.XdlA = new float[pm.FIs][6];
     memcpy( tes.XdlA, pm.XdlA, pm.FIs*6*sizeof(float) );
    tes.XdlB = new float[pm.FIs][6];
     memcpy( tes.XdlB, pm.XdlB, pm.FIs*6*sizeof(float) );
    tes.XdlD = new float[pm.FIs][6];
     memcpy( tes.XdlD, pm.XdlD, pm.FIs*6*sizeof(float) );
    tes.XpsiA = new double[pm.FIs][6];
     memcpy( tes.XpsiA, pm.XpsiA, pm.FIs*6*sizeof(double) );
    tes.XpsiB = new double[pm.FIs][6];
     memcpy( tes.XpsiB, pm.XpsiB, pm.FIs*6*sizeof(double) );
    tes.XpsiD = new double[pm.FIs][6];
     memcpy( tes.XpsiD, pm.XpsiD, pm.FIs*6*sizeof(double) );
    tes.XlamA = new float[pm.FIs][6];
     memcpy( tes.XlamA, pm.XlamA, pm.FIs*6*sizeof(float) );
    tes.Xetaf = new float[pm.FIs][6];
     memcpy( tes.Xetaf, pm.Xetaf, pm.FIs*6*sizeof(float) );
    tes.MASDJ = new float[pm.Ls];
     memcpy( tes.MASDJ, pm.MASDJ, pm.Ls*sizeof(float) );

    tes.XetaA = new double[pm.FIs][6];
     memcpy( tes.XetaA, pm.XetaA, pm.FIs*6*sizeof(double) );
    tes.XetaB = new double[pm.FIs][6];
     memcpy( tes.XetaB, pm.XetaB, pm.FIs*6*sizeof(double) );
    tes.XFTS = new double[pm.FIs][6];
     memcpy( tes.XFTS, pm.XFTS, pm.FIs*6*sizeof(double) );
    tes.lnSAT = new double[pm.Ls];
     memcpy( tes.lnSAT, pm.lnSAT, pm.Ls*sizeof(double) );
//   tes.SATT = (char *)aObj[ o_wi_satt].Alloc( tes.Ls, 1, A_ );
 }

 if( tes.PG > 0 )
 {
//   tes.Fug = (float *)aObj[ o_wd_fug].Alloc( tes.PG, 1, F_ );
//   tes.Fug_l = (float *)aObj[ o_wd_fugl].Alloc( tes.PG, 1, F_ );
//   tes.Ppg_l = (float *)aObj[ o_wd_ppgl].Alloc( tes.PG, 1, F_ );
 }

   // Part 3

 if( tes.Ls > 1 && tes.FIs > 0 )
 {
    tes.Wb = new float[pm.Ls];
     memcpy( tes.Wb, pm.Wb, pm.Ls*sizeof(float) );
    tes.Wabs = new float[pm.Ls];
     memcpy( tes.Wabs, pm.Wabs, pm.Ls*sizeof(float) );
    tes.Rion = new float[pm.Ls];
     memcpy( tes.Rion, pm.Rion, pm.Ls*sizeof(float) );
//   tes.Qp = (double *)aObj[ o_wi_qp].Alloc( tes.FIs, 20, D_ );
//     tes.Qd = (double *)aObj[ o_wi_qd].Alloc( 1, 20, D_ );

 }
  flCopy = true;
// tes.R = (float *)aObj[ o_w_r].Alloc( tes.N, tes.N+1, D_ );

}
// free dynamic memory in objects and values
void TMulti::dyn_kill_test(MULTI& tes)
{
// memcpy( &pm.PunE, "jjbC", 4 );
// memset( &pm.N, 0, 36*sizeof(short));
// memset( &pm.TC, 0, 54*sizeof(double));
 delete[] tes.L1; /* delete[] tes.LsMod;   delete[] tes.LsMdc; */
 delete[] tes.mui;    delete[] tes.muk  ;    delete[] tes.muj  ;
 delete[] tes.SATNdx;    delete[] tes.DUL  ;    delete[] tes.DLL  ;
 delete[] tes.GEX;    delete[] tes.PUL  ;    delete[] tes.PLL  ;
 delete[] tes.YOF; /*  delete[] tes.PMc  ;    delete[] tes.DMc  ; */
 delete[] tes.Vol;    delete[] tes.HYM  ;    delete[] tes.VL   ;
 delete[] tes.MM ;    delete[] tes.H0   ;    delete[] tes.A0   ;
 delete[] tes.U0;    delete[] tes.S0   ;    delete[] tes.Cp0  ;
 delete[] tes.Cv0;    delete[] tes.Pparc;    delete[] tes.Y_m  ;
 delete[] tes.Y_la;    delete[] tes.Y_w  ;    delete[] tes.Gamma;
 delete[] tes.lnGmf;    delete[] tes.lnGmM;    delete[] tes.EZ   ;
 delete[] tes.Wb;    delete[] tes.Wabs ;    delete[] tes.Rion ;
 delete[] tes.Aalp;    delete[] tes.Sigw ;    delete[] tes.Sigg ;
 delete[] tes.Nfsp;    delete[] tes.MASDT;    delete[] tes.FVOL ;
 delete[] tes.FWGT;    delete[] tes.XcapA;    delete[] tes.XcapB;
 delete[] tes.XcapD;    delete[] tes.XdlA ;    delete[] tes.XdlB ;
 delete[] tes.XdlD;    delete[] tes.XpsiA;    delete[] tes.XpsiB;
 delete[] tes.XpsiD;    delete[] tes.Xr0h0;    delete[] tes.XlamA;
 delete[] tes.Xetaf;    delete[] tes.Xcond;    delete[] tes.Xeps ;
 delete[] tes.Awt;    delete[] tes.A    ;    delete[] tes.XFs  ;
 delete[] tes.Falps;    delete[] tes.Fug  ;    delete[] tes.Fug_l;
 delete[] tes.Ppg_l;    delete[] tes.XFTS ;    delete[] tes.MASDJ;
 delete[] tes.G;    delete[] tes.G0   ;    delete[] tes.lnGam;
 delete[] tes.lnGmo;    delete[] tes.lnSAT;    delete[] tes.B    ;
 delete[] tes.U;    delete[] tes.U_r  ;    delete[] tes.C    ;
 delete[] tes.IC_m;    delete[] tes.IC_lm;    delete[] tes.IC_wm;
 delete[] tes.BF;    delete[] tes.XF   ;    delete[] tes.YF   ;
 delete[] tes.XFA;    delete[] tes.YFA  ;    delete[] tes.Falp ;
 delete[] tes.XetaA;    delete[] tes.XetaB;    delete[] tes.X    ;
 delete[] tes.Y;    delete[] tes.XY   ;    delete[] tes.Qp   ;
 delete[] tes.Qd;    delete[] tes.MU   ;    delete[] tes.EMU  ;
 delete[] tes.NMU;    delete[] tes.W    ;    delete[] tes.Fx   ;
 delete[] tes.Wx;    delete[] tes.F    ;    delete[] tes.F0   ;
 delete[] tes.D;    delete[] tes.R    ;    delete[] tes.R1   ;
 /* delete[] tes.sMod; */   delete[] tes.SB   ;    delete[] tes.SM   ;
 delete[] tes.SF;    delete[] tes.SFs  ;    delete[] tes.pbuf ; //
 delete[] tes.RLC;    delete[] tes.RSC  ;   /* delete[] tes.RFLC ;
 delete[] tes.RFSC;  */  delete[] tes.ICC  ;    delete[] tes.DCC  ;
 delete[] tes.PHC;    delete[] tes.SCM  ;  /*  delete[] tes.SATT ; */
 delete[] tes.DCCW;    delete[] tes.XcapF;    delete[] tes.SM2   ;
 delete[] tes.SF2;

}

void TMulti::Test_Eq( int size, double *p1, double *p2, const char *key )
{
  vstr buf(150);
  for( int i=0; i<size; i++ )
    if( fabs(p1[i]-p2[i]) > fabs(p1[i])*1e-7 )
    {
       sprintf(buf, "%7s[%3d]:  %12.8g  %12.8g", key, i, p1[i], p2[i]);
#ifndef IPMGEMPLUGIN
//       vfMessage( window(), "Double ", buf.p );
#endif
       cout << "Double " << buf.p << endl;
//       break;
    }
}

void TMulti::Test_Eq( int size, float *p1, float *p2, const char *key )
{
  vstr buf(150);
  for( int i=0; i<size; i++ )
    if( p1[i] != p2[i] )
    {
       sprintf(buf, "%7s[%3d]:  %12.8g  %12.8g", key, i, p1[i], p2[i]);
#ifndef IPMGEMPLUGIN
//       vfMessage(window(), "Float ", buf.p);
#endif
       cout << "Float  " << buf.p << endl;
//       break;
    }
}

void TMulti::Test_Eq( int size, short *p1, short *p2, const char *key )
{
  vstr buf(150);
  for( int i=0; i<size; i++ )
    if( p1[i] != p2[i] )
    {
       sprintf(buf, "%7s[%3d]:  %12d  %12d", key, i, p1[i], p2[i]);
#ifndef IPMGEMPLUGIN
//       vfMessage(window(), "Float ", buf.p);
#endif
       cout << "Short  " << buf.p << endl;
//       break;
    }
}

// Test by comparing old and new dynamic memory of the MULTI structure
void TMulti::dyn__test(MULTI& tes)
{
 cout << "Test  begin:     previous     current" << endl;
 Test_Eq( 36, &tes.N, &pm.N, "IFlags" );
 Test_Eq( 54, &tes.TC, &pm.TC, "Scalars" );
 Test_Eq( pm.L, tes.DUL, pm.DUL, "DUL" );
 Test_Eq( pm.L, tes.DLL, pm.DLL, "DLL" );
 Test_Eq( pm.L, tes.Vol, pm.Vol, "Vol" );
 Test_Eq( pm.L, tes.Pparc, pm.Pparc, "Pparc" );
 Test_Eq( pm.L, tes.MM, pm.MM, "MM" );
 Test_Eq( pm.N, tes.Awt, pm.Awt, "Awt" );
// Test_Eq( pm.FI, tes.XFs, pm.XFs, "XFs" );
// Test_Eq( pm.FI, tes.Falps, pm.Falps, "Falps" );
 Test_Eq( pm.L, tes.G, pm.G, "G" );
 Test_Eq( pm.L, tes.G0, pm.G0, "G0" );
 Test_Eq( pm.L, tes.lnGam, pm.lnGam, "lnGam" );
 Test_Eq( pm.L, tes.lnGmo, pm.lnGmo, "lnGmo" );
 Test_Eq( pm.N, tes.B, pm.B, "B" );
 Test_Eq( pm.N, tes.U, pm.U, "U" );
 Test_Eq( pm.N, tes.U_r, pm.U_r, "U_r" );
 Test_Eq( pm.N, tes.C, pm.C, "C" );
 Test_Eq( pm.FI, tes.XF, pm.XF, "XF" );
 Test_Eq( pm.FI, tes.YF, pm.YF, "YF" );
 Test_Eq( pm.FI, tes.Falp, pm.Falp, "Falp" );
 Test_Eq( pm.L, tes.X, pm.X, "X" );
 Test_Eq( pm.L, tes.Y, pm.Y, "Y" );
 Test_Eq( pm.L, tes.XY, pm.XY, "XY" );
 Test_Eq( pm.L, tes.MU, pm.MU, "MU" );
 Test_Eq( pm.L, tes.EMU, pm.EMU, "EMU" );
 Test_Eq( pm.L, tes.NMU, pm.NMU, "NMU" );
 Test_Eq( pm.L, tes.W, pm.W, "W" );
 Test_Eq( pm.L, tes.F, pm.F, "F" );
 Test_Eq( pm.L, tes.F0, pm.F0, "F0" );
 Test_Eq( pm.FI, tes.YOF, pm.YOF, "YOF" );
 Test_Eq( pm.FI, tes.FVOL, pm.FVOL, "FVOL" );
 Test_Eq( pm.FI, tes.FWGT, pm.FWGT, "FWGT" );
 Test_Eq( pm.L, tes.lnGmM, pm.lnGmM, "lnGmM" );
 Test_Eq( pm.L, tes.GEX, pm.GEX, "GEX" );

 if( tes.L > 0 )
 {
 Test_Eq( pm.L, tes.Y_la, pm.Y_la, "Y_la" );
 Test_Eq( pm.L, tes.Y_w, pm.Y_w, "Y_w" );
 Test_Eq( pm.L, tes.Fx, pm.Fx, "Fx" );
 Test_Eq( pm.L, tes.Wx, pm.Wx, "Wx" );
 Test_Eq( pm.L, tes.VL, pm.VL, "VL" );
 Test_Eq( pm.L, tes.Gamma, pm.Gamma, "Gamma" );
 Test_Eq( pm.L, tes.lnGmf, pm.lnGmf, "lnGmf" );
 Test_Eq( pm.L, tes.D, pm.D, "D" );
 }

   // Part 2  not required arrays

 if( tes.FIs > 0 && tes.Ls > 0 )
 {
 Test_Eq( pm.FIs*pm.N, tes.BF, pm.BF, "BF" );
 Test_Eq( pm.FIs, tes.XFA, pm.XFA, "XFA" );
 Test_Eq( pm.FIs, tes.YFA, pm.YFA, "YFA" );
// Test_Eq( pm.FIs, tes.PMc, pm.PMc, "PMc" ); /* not allocated ! */
// Test_Eq( pm.FIs, tes.DMc, pm.DMc, "DMc" ); /* not allocated ! */
// Test_Eq( pm.FIs, tes.PUL, pm.PUL, "PUL" );
// Test_Eq( pm.FIs, tes.PLL, pm.PLL, "PLL" );
}

 if( tes.LO > 1 )
 {
 Test_Eq( pm.L, tes.Y_m, pm.Y_m, "Y_m" );
 Test_Eq( pm.N, tes.IC_m, pm.IC_m, "IC_m" );
 Test_Eq( pm.N, tes.IC_lm, pm.IC_lm, "IC_lm" );
 Test_Eq( pm.N, tes.IC_wm, pm.IC_wm, "IC_wm" );
 }
 // sorption phases
 if( syp->PAalp != S_OFF )
 {
 Test_Eq( pm.FI, tes.Aalp, pm.Aalp, "Aalp" );
// Test_Eq( pm.FI*2, &tes.Xr0h0[0][0], &pm.Xr0h0[0][0], "Xr0h0" );
 }
 if( syp->PSigm != S_OFF )
   Test_Eq( pm.FI, tes.Sigw, pm.Sigw, "Sigw" );
 if( syp->PSigm != S_OFF )
   Test_Eq( pm.FI, tes.Sigg, pm.Sigg, "Sigg" );
 if( tes.E )
 {
 Test_Eq( pm.L, tes.EZ, pm.EZ, "EZ" );
 Test_Eq( pm.FI, tes.Xcond, pm.Xcond, "Xcond" );
 Test_Eq( pm.FI, tes.Xeps, pm.Xeps, "Xeps" );
 }

 if( tes.FIat > 0 && tes.Lads > 0 && tes.FIs > 0 )
 {
 Test_Eq( pm.FIs*6, &tes.Nfsp[0][0], &pm.Nfsp[0][0], "Nfsp" );
 Test_Eq( pm.FIs*6, &tes.MASDT[0][0], &pm.MASDT[0][0], "MASDT" );
 Test_Eq( pm.FIs*6, &tes.XcapA[0][0], &pm.XcapA[0][0], "XcapA" );
 Test_Eq( pm.FIs*6, &tes.XcapB[0][0], &pm.XcapB[0][0], "XcapB" );
 Test_Eq( pm.FIs*6, &tes.XcapD[0][0], &pm.XcapD[0][0], "XcapD" );
 Test_Eq( pm.FIs*6, &tes.XcapF[0][0], &pm.XcapF[0][0], "XcapF" );
 Test_Eq( pm.FIs*6, &tes.XdlA[0][0], &pm.XdlA[0][0], "XdlA" );
 Test_Eq( pm.FIs*6, &tes.XdlB[0][0], &pm.XdlB[0][0], "XdlB" );
 Test_Eq( pm.FIs*6, &tes.XdlD[0][0], &pm.XdlD[0][0], "XdlD" );
 Test_Eq( pm.FIs*6, &tes.XpsiA[0][0], &pm.XpsiA[0][0], "XpsiA" );
 Test_Eq( pm.FIs*6, &tes.XpsiB[0][0], &pm.XpsiB[0][0], "XpsiB" );
 Test_Eq( pm.FIs*6, &tes.XpsiD[0][0], &pm.XpsiD[0][0], "XpsiD" );
 Test_Eq( pm.FIs*6, &tes.XlamA[0][0], &pm.XlamA[0][0], "XlamA" );
 Test_Eq( pm.FIs*6, &tes.Xetaf[0][0], &pm.Xetaf[0][0], "Xetaf" );
 Test_Eq( pm.FIs*6, &tes.XpsiA[0][0], &pm.XpsiA[0][0], "XpsiA" );
 Test_Eq( pm.Ls, tes.MASDJ, pm.MASDJ, "MASDJ" );
 Test_Eq( pm.FIs*6, &tes.XetaA[0][0], &pm.XetaA[0][0], "XetaA" );
 Test_Eq( pm.FIs*6, &tes.XetaB[0][0], &pm.XetaB[0][0], "XetaB" );
 Test_Eq( pm.FIs*6, &tes.XFTS[0][0], &pm.XFTS[0][0], "XFTS" );
// Test_Eq( pm.Ls, tes.lnSAT, pm.lnSAT, "lnSAT" );
 }

 if( tes.PG > 0 )
 {
//   tes.Fug = (float *)aObj[ o_wd_fug].Alloc( tes.PG, 1, F_ );
//   tes.Fug_l = (float *)aObj[ o_wd_fugl].Alloc( tes.PG, 1, F_ );
//   tes.Ppg_l = (float *)aObj[ o_wd_ppgl].Alloc( tes.PG, 1, F_ );
 }

   // Part 3

// if( tes.Ls > 1 && tes.FIs > 0 )
// {
// Test_Eq( pm.Ls, tes.Wb, pm.Wb, "Wb" );
// Test_Eq( pm.Ls, tes.Wabs, pm.Wabs, "Wabs" );
// Test_Eq( pm.Ls, tes.Rion, pm.Rion, "Rion" );
// }
  cout << "Test sequence end: " << endl;
}

//--------------------- End of ms_muleq.cpp ---------------------------

