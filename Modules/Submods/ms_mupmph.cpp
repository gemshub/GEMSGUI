//-------------------------------------------------------------------
// $Id: ms_mupmph.cpp 1379 2009-07-29 14:53:22Z gems $
//
// (c) 1992-2007 S.Dmitrieva, D.Kulik, K.Chudnenko, I.Karpov
//
// Implementation of parts of the Interior Points Method (IPM) module
// for convex programming Gibbs energy minimization, described in:
// (Karpov, Chudnenko, Kulik (1997): American Journal of Science
//  v.297 p. 767-806)
//
// This file is part of a GEM-Selektor (GEMS) v.3.1.x program
// environment for thermodynamic modeling in geochemistry
// Uses: GEM-Selektor GUI GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the GPL v.3 license

//
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//
#include <cmath>
#include <iomanip>

#include "m_param.h"
#include "s_formula.h"

// Aseembling base GEM-IPM structure to calculate equilibrium states
//
// 1) Makes full  stoichiometry matrix A using the sy.Dcl list
// 2) Looks through Phase records and loads data into mixing model arrays
//    (optionally translating non-ideality Phase scripts)
// 3) Continues looking through one-component Phases, to seek for
//    specific surfaces and related surface parameters
// 4) Performs necessary re-scaling and conversions of data
// 5) Tests whether it is possible to take old primal (x, gamma) and dual
//   (u) solutions as an initial approximation
//

// Load System data
// Aseembling base GEM-IPM structure to calculate equilibrium states
//
// 1) Makes full  stoichiometry matrix A using the sy.Dcl list
// 2) Looks through Phase records and loads data into mixing model arrays
//    (optionally translating non-ideality Phase scripts)
// 3) Continues looking through one-component Phases, to seek for
//    specific surfaces and related surface parameters
void TMulti::MultiSystemInit( )
{
  int i, N;
  short ii;
  RMULTS* mup = TRMults::sm->GetMU();
  SYSTEM *syp = TSyst::sm->GetSY();

  pm.N =     syp->N;
  pm.NR =    syp->N;
  pm.L =     syp->L;
  pm.Ls =   syp->Ls;
  pm.LO =   syp->Lw/*-1*/;
  pm.PG =   syp->Lg;
  pm.PSOL = syp->Lhc;
  pm.Lads = syp->Lsor;
  pm.FI = syp->Fi;
  pm.FIs = syp->Fis;
//   pm.FIa = syp->N;

   if( syp->DLLim == S_ON || syp->DULim == S_ON )
        pm.PLIM = 1;
   else pm.PLIM = 0;

   if( syp->PE != S_OFF )
       pm.E = 1;
   else pm.E = 0;

   if( syp->NsTm > 0 && syp->PNfsp != S_OFF )
       pm.FIat = MST;
   else pm.FIat = 0;

   pm.MBX = syp->MBX;
   pm.GWAT = syp->Mwat;
   if( syp->Mbel > 1e-6 )    // workaround 21.06.13 DK - needs more investigation!
        pm.IC =  syp->Mbel;
   pm.GWAT = syp->Mwat * H2O_mol_to_kg;   // constant corrected 30.08.2008

    // Reallocating memory, if necessary
    if( !pm.pBAL )
{
    pmp->pKMM = -1;
    pmp->ITau = -1;
    pmp->kTau = 0.;
    pmp->kdT = 0.;
    dyn_new();
}
   if( pm.pBAL == 2 )
         goto NEXT2;

   // loading parameters for ICs (independent components)
   for( N=0, i=-1, ii=0; ii< mup->N; ii++ )
    {
        if( syp->Icl[ii] == S_OFF )
            continue;
        i++;
        N++;
        pm.mui[i] = ii;
        pm.ICC[i] = mup->ICC[ii];
        pm.Awt[i] = mup->BC[ii];
        pm.B[i] = syp->B[ii];
        pm.U[i] = pm.U_r[i] = pm.C[i] = 0.0;
        memcpy( pm.SB[i], mup->SB[ii], MAXICNAME+MAXSYMB );
        memcpy( pm.SB1[i], mup->SB[ii], MAXICNAME  );
    }

    if( N != pm.N )
        vfMessage( window(),
                   "Multi make error: N != pm.N",
                   "Please, press the BCC button first!" );
NEXT2:
    // loading data for DCs (dependent components)
    multi_sys_dc();

    // loading data for phases, incl. sorption and solution models
    multi_sys_ph();

    if( pm.pBAL < 2 )
        ConvertDCC(); // Loading generic species codes

   if(  !pm.pBAL && pm.pTPD > 1 )
       pm.pTPD = 1; // DC_LoadThermodynamicData();

    // Tests on integrity of CSD can be added here
     pm.pBAL = 2;
}

// Load System data to define lookup arrays
void TMulti::SystemToLookup( )
{
   SYSTEM *syp = TSyst::sm->GetSY();

   // copy intervals for minimizatiom
   pm.Pai[0] = syp->Pmin;
   pm.Pai[1] = syp->Pmax;
   pm.Pai[2] = syp->Pinc;
   pm.Pai[3] = 0.1;
   pm.Tai[0] = syp->Tmin;
   pm.Tai[1] = syp->Tmax;
   pm.Tai[2] = syp->Tinc;
   pm.Tai[3] = 0.1;
   pm.Fdev1[0] = 0.;
   pm.Fdev1[1] = syp->Tdev1;
   pm.Fdev2[0] = 0.;
   pm.Fdev2[1] = syp->Tdev2;

   if( pm.tMin == G_TP_ ) // set default intervals
   {
     if( pm.Pai[0] > pm.P || pm.Pai[1] < pm.P )
     {
       pm.Pai[0] = pm.Pai[1] = pm.P;
       pm.Pai[2] = 0.;
     }
     if( pm.Tai[0] > pm.TC || pm.Tai[1] < pm.TC )
     {
       pm.Tai[0] = pm.Tai[1] = pm.TC;
       pm.Tai[2] = 0.;
     }
   }

}


//Load data for DC from Modelling Project definition to MULTI structure
//
void TMulti::multi_sys_dc()
{
    int j, ii, L, iZ=0;
    short jj, jja, ja, kk;
    double a, *A; // , Vv =0.;
    double mm;
    TIArray<TFormula> aFo;
    gstring form;
    RMULTS* mup = TRMults::sm->GetMU();
    SYSTEM *syp = TSyst::sm->GetSY();

    ErrorIf( !TMTparm::sm->GetTP()->G, "Multi", "Multi make error: !tpp->G" );
///    int xVol = 0;   // SD 09/02/2007
///    if( pm.PV == VOL_CONSTR )
///      xVol = getXvolume();

    if( !pm.pBAL )
    {  // making full stoichiometry matrix from DC formula list
        // load formulae
        for( ii=0; ii<mup->L; ii++ )
        {
            aFo.Add( new TFormula() );
            form = aFo[ii].form_extr( ii, mup->L, mup->DCF );
            aFo[ii].SetFormula( form.c_str() ); // and ce_fscan
        }

        A = new double[mup->N*mup->L];
        fillValue(A, 0., (mup->N*mup->L) );
        for( ii=0; ii<mup->L; ii++ )
            aFo[ii].Stm_line( mup->N, A+ii*mup->N, (char *)mup->SB, mup->Val );
        aFo.Clear();
        // loading data for dependent components
        for( L=0, j=-1, jj=0; jj<mup->L; jj++ )
        {
            if( syp->Dcl[jj] == S_OFF )
                continue;
            j++;
            L++;
            pm.DCC[j] = mup->DCC[jj];
            pm.muj[j] = jj;
            pm.lnGam[j] = 0.0;
            pm.lnGmo[j] = 0.0;
            pm.X[j] = 0.0;
            pm.Y[j] = 0.0;
            pm.XY[j] = 0.0;
            memcpy( pm.SM[j], mup->SM[jj]+MAXSYMB+MAXDRGROUP, MAXDCNAME );
            if( j < syp->Ls )
                memcpy( pm.SM2[j], mup->SM[jj]+MAXSYMB+MAXDRGROUP, MAXDCNAME );
            if( j < syp->Ls && j >= syp->Ls - syp->Lsor)
            {   // assembling DC name list for sorption surface species
                ja = (short)(j-(syp->Ls-syp->Lsor));
                if(!(pm.SM3 && pm.DCC3))
                {  // Here a workaround of crash detected on 7.02.2011
                   char buf[32];
                   memcpy( buf, mup->SM[jj], MAXSYMB+MAXDRGROUP+MAXDCNAME ); buf[MAXSYMB+MAXDRGROUP+MAXDCNAME] = 0;
                   vfMessage( window(),
                              "Multi make warning: Lsor != pm.Lads, DC:", buf );
                }
                else {
                   memcpy( pm.SM3[ja], mup->SM[jj]+MAXSYMB+MAXDRGROUP, MAXDCNAME );
                   // surface species DC class code (usage will be obsolete)
                   pm.DCC3[ja] = mup->DCC[jj];
                }
            }
            copyValues( pm.dcMod[j], TMTparm::sm->GetTP()->DCMod[jj], 6);
            mm = 0.0;
            for( ii=0; ii<pm.N; ii++ ) // compressing the stoichiometry matrix
            {
                a = A[jj*mup->N + pm.mui[ii]];
                pm.A[j*pm.N+ii] = a;
                mm += a * pm.Awt[ii];   // calculating molar mass of DC
            }
            pm.MM[j] = mm;
        }
        delete[] A;
        // A = 0;
//     ErrorIf( L != pm.L, "Multi", "Multi make error: L != pm.L" );
        if( L != pm.L )
            vfMessage( window(),
                        "Multi make error: L != pm.L", "Please, press BCC!" );

    }

    if( pm.E )
    {   // index of charge
        for(  ii=0; ii<pm.N; ii++ )
            if( pm.ICC[ii] == IC_CHARGE )
                goto CH_FOUND;
        // error no charge
        if( vfQuestion( window(), "Multi", "No DC with formula charge are included:\n"
                       "Proceed (Y), Cancel (N)?"  ))
        {    pm.E = 0;
             syp->PE=S_OFF;  // Change Sveta 14/10/2002
        }
        else
            Error( "Multi","No DC with formula charge are included" );
CH_FOUND:
        iZ = ii;
    }

    for( j=-1, jj=0; jj<mup->L; jj++ ) // Main loop for data loading
    {
        if( syp->Dcl[jj] == S_OFF )
            continue;
        j++;
        pm.DCC[j] = mup->DCC[jj];
        if( pm.E )
            pm.EZ[j] = pm.A[pm.N*j+iZ];
        pm.Pparc[j] = pm.Pc;
        pm.F0[j] = 0.0;
        pm.XY[j] = 0.0;

///        if( tpp->PtvVm == S_ON )
///            switch( pm.PV )
///            { // calculating actual molar volume of DC
///            case VOL_CONSTR:
///              if( syp->Vuns )
///                       Vv = syp->Vuns[jj];
///                pm.A[j*pm.N+xVol] = tpp->Vm[jj] + Vv;
///            case VOL_CALC:
///            case VOL_UNDEF:
///                if( syp->Vuns )
///                       Vv = syp->Vuns[jj];
///                pm.Vol[j] = ( tpp->Vm[jj] + Vv )* 10.; // Check!
///               break;
///            }
///        else pm.Vol[j] = 0.0;

// testing kinetic/metastability constraints
        if( pm.PLIM && jj < mup->L )
        { // KM constraints necessary!
            pm.RLC[j] = syp->RLC[jj];
            pm.RSC[j] = syp->RSC[jj];
            if( syp->DUL )
                pm.DUL[j] = syp->DUL[jj];
            else pm.DUL[j] = 1e6;
            ja = (short)(j-(pm.Ls-pm.Lads));
            if( pm.lnSAC && ja < pm.Lads && ja >= 0
                  && pm.DUL[j] < 1e6 - TProfil::pm->pa.p.DKIN )
               pm.lnSAC[ja][3] = pm.DUL[j]; // Copy of DUL for SACT refining
            if( syp->DLL )
                pm.DLL[j] = syp->DLL[jj];
            else pm.DLL[j] = 0.0;
        }
        else
        {
            pm.RLC[j] = NO_LIM;
            pm.RSC[j] = QUAN_MOL;
            pm.DUL[j] = 1e6;  // tc_DCmaxm trivial restrictions!
            pm.DLL[j] = 0.0;
        }
        // additional data
        pm.fDQF[j] = 0.0;  // DQF parameters or pure-gas fugacities zeroed off

        if( syp->PParc != S_OFF )
            pm.Pparc[j] = syp->Pparc[jj];
        pm.lnGmM[j] = 0.0;
        if( jj < mup->L && syp->PlnGf != S_OFF )  // was jj < mup->Ls
        {
            pm.lnGmf[j] = syp->lnGmf[jj];
        }
        else pm.lnGmf[j] = 0.0;

        if( !( j < pm.Ls && j >= pm.Ls - pm.Lads ) )
            continue;   // the following is not done for non-surface species

        ja = (short)(j-(pm.Ls-pm.Lads));
        jja = (short)(jj-(mup->Ls-mup->Lads));
// Loading MaSdj - max.sur.densities for non-competitive sorbates */
        if( syp->PMaSdj != S_OFF )
        {
            for( kk=0; kk<DFCN; kk++ )
               if( !IsFloatEmpty( syp->MaSdj[jja][kk] ) )
                   pm.MASDJ[ja][kk] = syp->MaSdj[jja][kk];
               else pm.MASDJ[ja][kk]= 0.0;
        }
        if( syp->PSATT != S_OFF )
        { // Loading SATC - codes of methods for SAT calculation
            pm.SATT[ja] = syp->SATC[jja][SA_MCA];

         // Loading allocation to surface types
            switch( syp->SATC[jja][SA_STX] )
            {
              default:
              case CST_0:
                pm.SATX[ja][XL_ST] = 0;
                break;
              case CST_1:
                pm.SATX[ja][XL_ST] = 1;
                break;
              case CST_2:
                pm.SATX[ja][XL_ST] = 2;
                break;
              case CST_3:
                pm.SATX[ja][XL_ST] = 3;
                break;
              case CST_4:
                pm.SATX[ja][XL_ST] = 4;
                break;
              case CST_5:
                pm.SATX[ja][XL_ST] = 5;
                break;
            }

            // Loading allocation to carrier end-members
            switch( syp->SATC[jja][SA_EMX] )
            {
              case CCA_0:
                pm.SATX[ja][XL_EM] = 0;
                break;
              case CCA_1:
                pm.SATX[ja][XL_EM] = 1;
                break;
              case CCA_2:
                pm.SATX[ja][XL_EM] = 2;
                break;
              case CCA_3:
                pm.SATX[ja][XL_EM] = 3;
                break;
              case CCA_4:
                pm.SATX[ja][XL_EM] = 4;
                break;
              case CCA_5:
                pm.SATX[ja][XL_EM] = 5;
                break;
//              case CCA_6:
//                pm.SATX[ja][XL_EM] = 6;
//                break;
//              case CCA_7:
//                pm.SATX[ja][XL_EM] = 7;
//                break;
//              case CCA_8:
//                pm.SATX[ja][XL_EM] = 8;
//                break;
//              case CCA_9:
//                pm.SATX[ja][XL_EM] = 9;
//                break;
              case CCA_VOL:
                pm.SATX[ja][XL_EM] = -1; // whole sorbent
                break;
              default:
                pm.SATX[ja][XL_EM] = -2; // main DC of the sorbent
                break;
            }

            // Loading allocation to surface sites
            switch( syp->SATC[jja][SA_SITX] )
            {
              default:
              case CSI_0:
                pm.SATX[ja][XL_SI] = 0;
                break;
              case CSI_1:
                pm.SATX[ja][XL_SI] = 1;
                break;
              case CSI_2:
                pm.SATX[ja][XL_SI] = 2;
                break;
              case CSI_3:
                pm.SATX[ja][XL_SI] = 3;
                break;
              case CSI_4:
                pm.SATX[ja][XL_SI] = 4;
                break;
              case CSI_5:
                pm.SATX[ja][XL_SI] = 5;
                break;
            }
            // Loading charge allocation to surface planes
            switch( syp->SATC[jja][SA_PLAX] )
            {
              default:
              case SPL_0:   // only to 0 plane (sorbent surface plane)
                   pm.SATX[ja][XL_SP] = 0;
                   break;
              case SPL_1:
              case SPL_B:   // to beta plane or plane 1 in CD 3-layer
                   pm.SATX[ja][XL_SP] = 1;
                   break;
              case SPL_2:
              case SPL_D:   // to DL plane or plane 2 in CD 3-layer
                   pm.SATX[ja][XL_SP] = 2;
                   break;
              case SPL_3:
              case SPL_C:   // to OS anion beta plane (4-layer model, reserved)
                   pm.SATX[ja][XL_SP] = 3;
                   break;
            }
        }
    }
}

//==========================================
bool TMulti::CompressPhaseIpxt( int kPH )
{
  int jj, jb, cnt=0;
  TCIntArray  aDCused;
  TPhase* aPH=(TPhase *)(&aMod[RT_PHASE]);
  RMULTS* mup = TRMults::sm->GetMU();

  for( jj=0, jb = 0; jj<kPH; jj++ )
        jb += mup->Ll[jj];

  for( jj=0, cnt = 0; jj<mup->Ll[kPH]; jj++ )
  {
     if( TSyst::sm->GetSY()->Dcl[jj+jb] == S_OFF )
          aDCused.Add(-1);
     else
     { aDCused.Add(cnt); cnt++; }
  }

  if( cnt < mup->Ll[kPH] )
   return aPH->CompressRecord( cnt, aDCused, true );
  else return true;
}

//============================================

//Loading data for phases and mixing models into the structure MULTI
//
void TMulti::multi_sys_ph()
{
    int k, i;
    bool non_sorption_phase, is_ss;
    short kk, j, je, jb, ja=0;
    //vstr pkey(MAXRKEYLEN);
    time_t crt;
//    double G;
    double PMM;  // Phase mean mol. mass
    int Cjs, car_l[32], car_c=0; // current index carrier sorbent
    TPhase* aPH=(TPhase *)(&aMod[RT_PHASE]);
    RMULTS* mup = TRMults::sm->GetMU();
    SYSTEM *syp = TSyst::sm->GetSY();

    aPH->ods_link(0);

    pm.Ls = 0;
    pm.PG = 0;
    pm.FIs = syp->Fis;
    pm.FIa = 0;
    pm.Lads = 0;   // Is this really needed?
//    ja=0;
    for( kk=0, /*FI=0,*/ k=-1, /*jb=0,*/ je=0; kk<mup->Fi; kk++ )
    {
        if( TSyst::sm->GetSY()->Pcl[kk] == S_OFF )
            continue;
        k++; // FI++;  //PMM = 0.0;
        pm.L1[k] = syp->Ll[kk];
        jb = je;
        je += pm.L1[k];
        pm.muk[k] = kk;
        pm.PHC[k] = mup->PHC[kk];
        memcpy( pm.SF[k], mup->SF[kk], MAXSYMB );
        memcpy( pm.SF[k]+MAXSYMB, mup->SF[kk]+MAXSYMB+MAXPHSYMB, MAXPHNAME );
        if( kk < mup->Fis ) // bugfix 4.04.14 DK was k < syp.Fis
        {
            memcpy( pm.SF2[k], mup->SF[kk], MAXSYMB );
            memcpy( pm.SF2[k]+MAXSYMB, mup->SF[kk]+MAXSYMB+MAXPHSYMB, MAXPHNAME );
        }
        if( pm.pNP && pm.pBAL && pm.pKMM>0  )
        goto PARLOAD;

        aPH->TryRecInp( mup->SF[kk], crt, 0 );  // Now reading all phase records!

        // New stuff for TKinMet
        // Resetting vector of TSolMod pointers to avoid problems with activity coeffs
        //     in TSolMod calculations after switching phases on/off
        if(pm.ITau < 0 )
        {
            if(phKinMet[k])
                delete phKinMet[k];
            phKinMet[k] = NULL;
            pm.kMod[k][0] = aPH->php->kin_t[2];
            pm.kMod[k][1] = aPH->php->kin_t[3];
            pm.kMod[k][2] = aPH->php->kin_t[4];
            pm.kMod[k][3] = aPH->php->kin_t[5];
            pm.kMod[k][4] = aPH->php->kin_t[6];
            pm.kMod[k][5] = aPH->php->kin_t[7];
        }

        if( aPH->php->PrpCon == S_ON )
        {
            pm.LsKin[k*6] = aPH->php->nPRk;
            pm.LsKin[k*6+1] = aPH->php->nSkr;
            pm.LsKin[k*6+2] = aPH->php->nrpC;
            pm.LsKin[k*6+3] = aPH->php->naptC;
            pm.LsKin[k*6+4] = aPH->php->nAscC;
            pm.LsKin[k*6+5] = aPH->php->nFaces;
        }
        else {
            pm.LsKin[k*6] = pm.LsKin[k*6+1] = pm.LsKin[k*6+2] =0;
            pm.LsKin[k*6+3] = pm.LsKin[k*6+4] = pm.LsKin[k*6+5] =0;
        }
/*
long int
*xSKrC,  ///< new: Collected array of aq/gas/sorption species indexes used in activity products (-> += LsKin[k][1])
*ocPRkC; ///< new: Collected array of operation codes for kinetic parallel reaction terms (-> += LsKin[k][0])
*/

        if( kk < mup->Fis && mup->Ll[kk] > 1 )
        { // getting data and parameters for a multicomponent phase
//           if( pm.pNP && pm.pBAL )
//                goto PARLOAD;
            // Resetting vector of TSolMod pointers to avoid problems with activity coeffs
            //     in TSolMod calculations after switching phases on/off (DK 25.05.2009)
            if(phSolMod[k])
                delete phSolMod[k];
            phSolMod[k] = NULL;
//          aPH->TryRecInp( mup->SF[kk], crt, 0 );
            // read informations from phase-solution
            memcpy( pm.sMod[k], aPH->php->sol_t, 6 );
            pm.sMod[k][6] = aPH->php->kin_t[0];
            pm.sMod[k][7] = aPH->php->kin_t[1];
            // Added SD 20/01/2010
            if( aPH->php->Ppnc == S_ON && aPH->php->npxM > 0 )
                CompressPhaseIpxt( kk );
            if( aPH->php->Ppnc == S_ON )
            {
               pm.LsMod[k*3] = aPH->php->ncpN;
               pm.LsMod[k*3+2] = aPH->php->ncpM;
               pm.LsMod[k*3+1] = aPH->php->npxM;
            }
            else pm.LsMod[k*3] = pm.LsMod[k*3+1] = pm.LsMod[k*3+2] =0;
            if( aPH->php->Psco == S_ON )
                pm.LsMdc[k*3] = aPH->php->nscM;
            else pm.LsMdc[k*3] = 0;

            pm.LsPhl[k*2] = aPH->php->nlPh;
            pm.LsPhl[k*2+1] = aPH->php->nlPc;
            pm.LsMdc2[k*3] = aPH->php->ndqf;
            pm.LsMdc2[k*3+1] = aPH->php->nrcp;

            pm.LsUpt[k*2] = aPH->php->numpC;
            if( aPH->php->PumpCon && aPH->php->lICu && ( aPH->php->kin_t[4] == KM_UPT_ENTRAP
                 || aPH->php->kin_t[4] == KM_IEX_FAST || aPH->php->kin_t[4] == KM_IEX_SLOW ) )
            pm.LsUpt[k*2+1] = pm.L1[k]; // bugfix 04.03.2015 DK
                  //  aPH->php->nFaces;     // provisional 8.04.2013 DK

// New stuff for TSorpMod
if(phSorpMod[k])
   delete phSorpMod[k];
phSorpMod[k] = NULL;
/*
long int
*LsESmo, ///< new: number of EIL model layers; EIL params per layer; CD coefs per DC; reserved  [Fis][4]
*LsISmo, ///< new: number of surface sites; isotherm coeffs per site; isotherm coeffs per DC; max.denticity of DC [Fis][4]
*xSMd;   ///< new: denticity of surface species per surface site (site allocation) (-> L1[k]*LsISmo[k][3]] )
*/
        }
        else  // nothing to read in the Phase record
            if( k<pm.FIs )
            {
                pm.LsMod[k*3] = 0;
                pm.LsMod[k*3+1] = 0;
                pm.LsMod[k*3+2] = 0;
                pm.LsMdc[k*3] = 0;
                memset( pm.sMod[k], ' ', 8 );
                pm.LsPhl[k*2] = 0;
                pm.LsPhl[k*2+1] = 0;
                pm.LsMdc2[k*3] = 0;
                pm.LsMdc2[k*3+1] = 0;
                pm.LsUpt[k*2] = 0;
                pm.LsUpt[k*2+1] = 0;
// New stuff for TSorpMod
            }


PARLOAD: if( k < syp->Fis )
             pm.Ls += pm.L1[k];
        non_sorption_phase = true;
        switch( pm.PHC[k] )
        { // loading necessary parameters
        case PH_AQUEL:
             break;
        case PH_GASMIX:
        case PH_PLASMA:
        case PH_FLUID:
//            pm.PG = pm.L1[k];
              pm.PG += pm.L1[k];
            break;
        case PH_HCARBL:
            pm.PSOL = pm.L1[k];
            break;
        case PH_LIQUID:
        case PH_SINCOND:
        case PH_SIMELT:
        case PH_IONEX:
            break;
        case PH_ADSORPT:
            break;
        case PH_POLYEL:
        case PH_SORPTION:
            pm.FIa++; non_sorption_phase = false;
            ph_sur_param( k, kk );
            for(i=0; i<32; i++ )
                car_l[i]=-1;
            car_c=0; // cQ=0;
        case PH_SINDIS:
            break;
        default:
            ; // Error message may be needed here!
        }
        // Moved by DK on 25.07.2006
        if( syp->PAalp != S_OFF )
           pm.Aalp[k] = syp->Aalp[kk];
        if( syp->PSigm != S_OFF )
        {
           pm.Sigw[k] = syp->Sigm[kk][0];
           pm.Sigg[k] = syp->Sigm[kk][1];
        }
        if( syp->PXepsC != S_OFF )
        {
           pm.Xcond[k] = syp->XEpsC[kk][1];
           pm.Xeps[k] = syp->XEpsC[kk][0];
        }
        if( syp->PXr0h0 != S_OFF )
        {
           pm.Xr0h0[k][0] = syp->Xr0h0[kk][0];
           pm.Xr0h0[k][1] = syp->Xr0h0[kk][1];
        }
        PMM = 0.0;
        Cjs = -1;
        // Loop over DCs in Phase, calculation of phase mean mol. mass
        for( j=jb; j<je; j++ )
        {
            is_ss = true;
            switch( pm.DCC[j] )
            {
            case DC_AQ_SOLVENT:
                pm.LO = j; is_ss = false;
                PMM = pm.MM[j];
                break;
            case DC_PEL_CARRIER:
            case DC_SUR_CARRIER:
                Cjs = j;
                PMM = pm.MM[j];
                car_l[car_c++]=j;  pm.Lads++; ja++;
                break;
            case DC_SUR_MINAL:
                car_l[car_c++]=j;  pm.Lads++; ja++;
                break;
  /* Remapping DC codes to indices in pm->SATX - compat with old-style SCMs *
   * even - inner-sphere complex on surface type 0,1,2,3,4 - A plane */
            case DC_SSC_A0:
//                pm.SATX[ja][XL_ST] = AT_SA0;
                pm.Lads++; ja++;
                break;
            case DC_SSC_A1:
//                pm.SATX[ja][XL_ST] = AT_SA1;
                pm.Lads++; ja++;
                break;
            case DC_SSC_A2:
//                pm.SATX[ja][XL_ST] = AT_SA2;
                pm.Lads++; ja++;
                break;
            case DC_SSC_A3:
//                pm.SATX[ja][XL_ST] = AT_SA3;
                pm.Lads++; ja++;
                break;
            case DC_SSC_A4:
//                pm.SATX[ja][XL_ST] = AT_SA4;
                pm.Lads++; ja++;
                break;
  /* odd - outer-sphere complex on surface type 0,1,2,3,4 - beta plane */
            case DC_WSC_A0:
//                pm.SATX[ja][XL_ST] = AT_SB0;
                pm.Lads++; ja++;
                break;
            case DC_WSC_A1:
//                pm.SATX[ja][XL_ST] = AT_SB1;
                pm.Lads++; ja++;
                break;
            case DC_WSC_A2:
//                pm.SATX[ja][XL_ST] = AT_SB2;
                pm.Lads++; ja++;
                break;
            case DC_WSC_A3:
//                pm.SATX[ja][XL_ST] = AT_SB3;
                pm.Lads++; ja++;
                break;
            case DC_WSC_A4:
//                pm.SATX[ja][XL_ST] = AT_SB4;
                pm.Lads++; ja++;
                break;
  /* Inner-sphere exchange ion at const-charge plane */
            case DC_IESC_A:
//                pm.SATX[ja][XL_ST] = AT_SA5;
                pm.Lads++; ja++;
                break;
  /* Outer-sphere exchange ion at const-charge plane */
            case DC_IEWC_B:
//                pm.SATX[ja][XL_ST] = AT_SB5;
                pm.Lads++; ja++;
                break;
  /* Aliaces for single-site model *
        * Surface site A plane -> '0' */
            case DC_SUR_GROUP:
//                pm.SATX[ja][XL_ST] = AT_SA0;
                pm.Lads++; ja++;
                break;
        /* Strong sur. complex A plane -> '0' */
            case DC_SUR_COMPLEX:
//                pm.SATX[ja][XL_ST] = AT_SA0;
                pm.Lads++; ja++;
                break;
       /* Weak sur complex B plane -> '1' */
            case DC_SUR_IPAIR:
//                pm.SATX[ja][XL_ST] = AT_SB0;
                pm.Lads++; ja++;
                break;
/* End extension */
            case DC_SOL_MAJOR: case DC_SOL_MAJDEP:
                Cjs = j;
            case DC_SOL_MINOR: case DC_SOL_MINDEP: case DC_SOL_IDEAL:
                PMM += pm.MM[j]; is_ss = false;
                break;
            default: /* if( isdigit( pm.DCC[j] ))
                                    pm.Lads++; else */
                is_ss = false;
                PMM += pm.MM[j]; // /pm.L1[k]; bugfix DK 28.02.2012
                break;
            }
 // Inserted 08.02.2011 by DK to prevent crashes on inconsistent phases
            if( non_sorption_phase && is_ss )
            {
                char buf[128];
                sprintf(buf, ": %d %2.16s %c  ( DC: %d  %2.16s %c ) ",
                        k, pm.SF[k],  pm.PHC[k], j, pm.SM[j], pm.DCC[j] );
                vfMessage( window(),
                           "Inconsistent DC in non-sorption phase", buf );
                pm.Lads--;
            }
            if( !non_sorption_phase && !is_ss )
            {
                char buf[128];
                sprintf(buf, ": %d %2.16s %c  ( DC: %d  %2.16s %c ) ",
                        k, pm.SF[k],  pm.PHC[k], j, pm.SM[j], pm.DCC[j] );
                vfMessage( window(),
                           "Inconsistent DC in sorption phase", buf );
            }
        }  /* j */
        if( PMM > 0. )  // cannot calculate better before the composition of the phase is known
            PMM /= pm.L1[k]; // DK 28.02.2012

        if( pm.PHC[k] == PH_POLYEL || pm.PHC[k] == PH_SORPTION )
            /* Analyzing assignment of surtypes */
            ph_surtype_assign( k, kk, jb, je, car_l, car_c, Cjs );

        if( syp->PYOF == S_OFF )
            pm.YOF[k] = 0.0;
        else
            switch( pm.PHC[k] )
            { // convert free energy of metastable phase from J/g to J/mol */
            case PH_POLYEL:
            case PH_SORPTION:
                if( Cjs >= 0 )
                {
                    PMM = pm.MM[Cjs];
                    pm.FWGT[k] = PMM;
                }
                /*  leave normalized to 1 gram pm.ln1Na[k] *= PMM; */
            case PH_AQUEL:
                if( pm.FWGT[k] < 1e-33 )   // bugfix 26.04.13 DK
                {
                    pm.YOF[k] = syp->YOF[kk] * PMM / pm.RT;
                    pm.FWGT[k] = PMM;
                }
                break;
            default:
                if( Cjs >=0 ) // if there is a MAJOR component in solution phase
                    PMM = pm.MM[Cjs];
                if( pm.FWGT[k] < 1e-33 )  // bugfix 26.04.13 DK
                {
                   pm.YOF[k] = syp->YOF[kk] * PMM / pm.RT;
                   pm.FWGT[k] = PMM;
                }
                break;
            }

        // test of kinetic constraints (units of measurement!)
        if( pm.PLIM && k >= pm.FIs )
        { /* restrictions set! */
            /* one-component phases */
            if( syp->PLLim != S_OFF || syp->PULim != S_OFF )
            {
                pm.RLC[pm.Ls+k-pm.FIs] = syp->RFLC[kk];
                pm.RSC[pm.Ls+k-pm.FIs] = syp->RFSC[kk];
            }
            if( syp->PUL )
                pm.DUL[pm.Ls+k-pm.FIs] = syp->PUL[kk];
            if( syp->PLL )
                pm.DLL[pm.Ls+k-pm.FIs] = syp->PLL[kk];
        }
        if( pm.PLIM && k < pm.FIs )
            /*    ( syp->PLLim != S_OFF || syp->PULim != S_OFF ) )  */
        { /* solutions */
            if( syp->RFLC )
                pm.RFLC[k] = syp->RFLC[kk];
            else pm.RFLC[k] = NO_LIM;
            if( syp->RFSC )
                pm.RFSC[k] = syp->RFSC[kk];
            else pm.RFSC[k] = QUAN_MOL;
            if( syp->PUL )
                pm.PUL[k] = syp->PUL[kk];
            else pm.PUL[k] = 1e6;
            if( syp->PLL )
                pm.PLL[k] = syp->PLL[kk];
            else pm.PLL[k] = 0.0;
        }
        if( !pm.PLIM && k<pm.FIs )
        { /* no set restrictions */
            pm.RFLC[k] = NO_LIM;
            pm.RFSC[k] = QUAN_MOL;
            pm.PUL[k] = 1e6;
            pm.PLL[k] = 0.0;
        }

    }  /* k */
}

// Loading surface type parameters
void TMulti::ph_sur_param( int k, int kk )
{
    int i;
    SYSTEM *syp = TSyst::sm->GetSY();

    for( i=0; i< syp->NsTm; i++ )
    {
        if( syp->SCMT[kk][i] == SC_NOT_USED || fabs(syp->Nfsp[kk][i])<1e-9 )
        { /*this type surface of dont used*/
            pm.Nfsp[k][i] =  DOUBLE_EMPTY;
            pm.MASDT[k][i] = DOUBLE_EMPTY;
            //    pm.MISDT[k][i] = FLOAT_EMPTY;
            pm.XcapA[k][i] = DOUBLE_EMPTY;
            pm.XcapB[k][i] = DOUBLE_EMPTY;
            pm.Xetaf[k][i] = DOUBLE_EMPTY;
            pm.SCM[k][i] = SC_NOT_USED;
            /*  pm.CSTS[k][i] = CCA_VOL; */
            continue;
        }
        pm.SCM[k][i] = syp->SCMT[kk][i];
        /*  pm.CSTS[k][i] = syp->CSTS[kk][i]; */
        pm.Nfsp[k][i] = syp->Nfsp[kk][i];  /* fraction! */

        /* (1e18/m2 m2/g)/(6.022e23 1/mol)=1.66054 mkmol/g */

        if( !syp->MaSdt || syp->MaSdt[kk][i] < 1e-9 )
            pm.MASDT[k][i] = TProfil::pm->pa.p.DNS * 1.66054 * syp->Aalp[kk];
        /* default, mkmol/g */
        else pm.MASDT[k][i] = syp->MaSdt[kk][i]*syp->Aalp[kk]*1.66054;
        /* max. in mkmol/g */

        // if( !syp->MiSdt || syp->MiSdt[kk][i] < 1e-9 )
        //     pm.MISDT[k][i] = 1.0; /* default, mkmol/g */
        //  else pm.MISDT[k][i] = syp->MiSdt[kk][i]*syp->Aalp[kk]*1.66054;
        /* min. in mkmol/g */
        /*
          pm.XCEC[k][i] = syp->Nsph[kk][i] * syp->Aalp[kk]*
                               syp->Nfsp[kk][i]*1.6606;        mkmol/g */

        if( syp->PXfIEC != S_OFF )
        {  /* Permanent surface charge density Ion-exchange capacity */
           // XfIEC[] in mkeq/m2; Xetaf[] in C/m2
            if( fabs(syp->XfIEC[kk][i])>1e-9 )
                pm.Xetaf[k][i] = syp->XfIEC[kk][i]*.09648531;
            else pm.Xetaf[k][i] = 0.0;
        }
        if( syp->PXcapA != S_OFF )
            pm.XcapA[k][i] = syp->XcapA[kk][i];
        else pm.XcapA[k][i] = 0.;
        if( syp->PXcapB != S_OFF )
            pm.XcapB[k][i] = syp->XcapB[kk][i];
        else pm.XcapB[k][i] = 0.;

        pm.XdlA[k][i] = 0.0;
        pm.XdlB[k][i] = 0.0;
        pm.XdlD[k][i] = 0.0;
        pm.XcapD[k][i] = 0.;
        /*
          if( syp->PXlam != S_OFF )
            pm.XlamA[k][i] = syp->Xlam[kk][i];
          else pm.XlamA[k][i] = 0.;
          if( syp->PXetf != S_OFF )
              pm.Xetaf[k][i] = syp->Xetaf[kk][i];
          else pm.Xetaf[k][i] = 0.;
        */
    } /* end for i */
}

// Assigning surface types to carrier (DC) - added 5/13/97 DAK
// Extended for CD-MUSIC by KD 31.10.2004
void TMulti::ph_surtype_assign( int k, int kk, int jb, int je,
                                 int car_l[], int car_c, int Cjs )
{
    int j, jcl, ja/*=0*/;
    long int  ist/*=0*/, isi/*=0*/;

    /*  double SATst; */

    for( j=jb; j<je; j++ )
    {
        if( pm.DCC[j] == DC_PEL_CARRIER || pm.DCC[j] == DC_SUR_CARRIER
                || pm.DCC[j] == DC_SUR_MINAL )
            continue;
        ja = j - ( pm.Ls - pm.Lads );
ist = pm.SATX[ja][XL_ST]; // / MSPN;   index of surface type
isi = pm.SATX[ja][XL_SI];                 // index of site on this surface type
pm.D[ist][isi] = 0.0;                    // cleanining the totals for sites
        jcl = pm.SATX[ja][XL_EM];
        if( jcl < car_c && jcl >= 0 )
            pm.SATX[ja][XL_EM] = car_l[jcl];
        else if( jcl >= car_c || jcl == -1 )
            pm.SATX[ja][XL_EM] = -1;   /* whole carrier */
        else pm.SATX[ja][XL_EM] = Cjs; /* main carrier DC */

        if( pm.SATT[ja] != SAT_COMP )
// To be fixed !!!!!!!!!!!
            pm.MASDJ[ja][PI_DEN] *= TSyst::sm->GetSY()->Aalp[kk]*1.66054;  /* 1/nm2 to mkmol/g */
//            pm.MASDJ[j] *= syp->Aalp[kk]*1.66054;  /* mkmol/g */
        if( !pm.MASDJ[ja] )
        {
            //      if( pm.SATT[j] == SAT_SITE )
            //         pm.MASDJ[j] = pm.MASDT[k][ist] - pm.MISDT[k][ist] ;
            if( pm.SATT[ja] == SAT_NCOMP )
                pm.MASDJ[ja][PI_DEN] = pm.MASDT[k][ist]; // Do we really need this?
//              pm.MASDJ[j] = pm.MASDT[k][ist];
            if( pm.SATT[ja] == SAT_COMP )
                pm.MASDJ[ja][PI_DEN] = 1.0;
//                pm.MASDJ[j] = 1.0;
        }
    } /* for j */
}


// End of file ms_mupmph.cpp
// --------------------------------------------------------------------

