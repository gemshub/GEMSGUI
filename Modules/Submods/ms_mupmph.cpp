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
// This file may be distributed under the terms of GEMS4 Development
// Quality Assurance Licence (GEMS4.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//
#include <cmath>
#include <iomanip>

#include "tmltsystem.h"
//#include "m_param.h"
#include "s_formula.h"
#include "v_user.h"
#include "service.h"
#include "ms_mtparm.h"
#include "ms_rmults.h"
#include "ms_system.h"

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
void TMultiSystem::MultiSystemInit( )
{
  int i, N;
  short ii;
  RMULTS* mup = TRMults::sm->GetMU();
  SYSTEM *syp = TSyst::sm->GetSY();

  pmp->N =     syp->N;
  pmp->NR =    syp->N;
  pmp->L =     syp->L;
  pmp->Ls =   syp->Ls;
  pmp->LO =   syp->Lw/*-1*/;
  pmp->PG =   syp->Lg;
  pmp->PSOL = syp->Lhc;
  pmp->Lads = syp->Lsor;
  pmp->FI = syp->Fi;
  pmp->FIs = syp->Fis;
//   pmp->FIa = syp->N;

   if( syp->DLLim == S_ON || syp->DULim == S_ON )
        pmp->PLIM = 1;
   else pmp->PLIM = 0;

   if( syp->PE != S_OFF )
       pmp->E = 1;
   else pmp->E = 0;

   if( syp->NsTm > 0 && syp->PNfsp != S_OFF )
       pmp->FIat = MST;
   else pmp->FIat = 0;

   pmp->MBX = syp->MBX;
   pmp->GWAT = syp->Mwat;
   if( syp->Mbel > 1e-6 )    // workaround 21.06.13 DK - needs more investigation!
        pmp->IC =  syp->Mbel;
   pmp->GWAT = syp->Mwat * H2O_mol_to_kg;   // constant corrected 30.08.2008

    // Reallocating memory, if necessary
    if( !pmp->pBAL )
{
    pmp->pKMM = -1;
    pmp->ITau = -1;
    pmp->kTau = 0.;
    pmp->kdT = 0.;
        dyn_new();
}
   if( pmp->pBAL == 2 )
         goto NEXT2;

   // loading parameters for ICs (independent components)
   for( N=0, i=-1, ii=0; ii< mup->N; ii++ )
    {
        if( syp->Icl[ii] == S_OFF )
            continue;
        i++;
        N++;
        pmp->mui[i] = ii;
        pmp->ICC[i] = mup->ICC[ii];
        pmp->Awt[i] = mup->BC[ii];
        pmp->B[i] = syp->B[ii];
        pmp->U[i] = pmp->U_r[i] = pmp->C[i] = 0.0;
        memcpy( pmp->SB[i], mup->SB[ii], MAXICNAME+MAXSYMB );
        memcpy( pmp->SB1[i], mup->SB[ii], MAXICNAME  );
    }

    if( N != pmp->N )
        vfMessage( window(),
                   "Multi make error: N != pmp->N",
                   "Please, press the BCC button first!" );
NEXT2:

    // loading data for DCs (dependent components)
    multi_sys_dc();

    // Resetting vector of TSolMod pointers to avoid problems with activity coeffs
    //     in TSolMod calculations after switching phases on/off (DK 25.05.2009)
    if(  pmp->pIPN <=0 ) //   !pmp->pNP || !pmp->pBAL || !pmp->pKMM )
    {
       pActivity->Reset_TSolMod();
       pActivity->Reset_TSorpMod();
    }
    // loading data for phases, incl. sorption and solution models
    multi_sys_ph();

    if( pmp->pBAL < 2 )
        pBaseSyst->ConvertDCC(); // Loading generic species codes

   if(  !pmp->pBAL && pmp->pTPD > 1 )
       pmp->pTPD = 1; // DC_LoadThermodynamicData();

    // Tests on integrity of CSD can be added here
     pmp->pBAL = 2;
}

// Load System data to define lookup arrays
void TMultiSystem::SystemToLookup( )
{
   SYSTEM *syp = TSyst::sm->GetSY();

   // copy intervals for minimizatiom
   pmp->Pai[0] = syp->Pmin;
   pmp->Pai[1] = syp->Pmax;
   pmp->Pai[2] = syp->Pinc;
   pmp->Pai[3] = 0.1;
   pmp->Tai[0] = syp->Tmin;
   pmp->Tai[1] = syp->Tmax;
   pmp->Tai[2] = syp->Tinc;
   pmp->Tai[3] = 0.1;
   pmp->Fdev1[0] = 0.;
   pmp->Fdev1[1] = syp->Tdev1;
   pmp->Fdev2[0] = 0.;
   pmp->Fdev2[1] = syp->Tdev2;

   if( pmp->tMin == G_TP_ ) // set default intervals
   {
     if( pmp->Pai[0] > pmp->P || pmp->Pai[1] < pmp->P )
     {
       pmp->Pai[0] = pmp->Pai[1] = pmp->P;
       pmp->Pai[2] = 0.;
     }
     if( pmp->Tai[0] > pmp->TC || pmp->Tai[1] < pmp->TC )
     {
       pmp->Tai[0] = pmp->Tai[1] = pmp->TC;
       pmp->Tai[2] = 0.;
     }
   }

}


//Load data for DC from Modelling Project definition to MULTI structure
//
void TMultiSystem::multi_sys_dc()
{
    int j, ii, L, iZ=0;
    short jj, jja, ja, kk;
    double a, *A; // , Vv =0.;
    double mm;
    vector<TFormula> aFo;
    string form;
    RMULTS* mup = TRMults::sm->GetMU();
    SYSTEM *syp = TSyst::sm->GetSY();

    ErrorIf( !TMTparm::sm->GetTP()->G, "Multi", "Multi make error: !tpp->G" );
///    int xVol = 0;   // SD 09/02/2007
///    if( pmp->PV == VOL_CONSTR )
///      xVol = getXvolume();

    if( !pmp->pBAL )
    {  // making full stoichiometry matrix from DC formula list
        // load formulae
        for( ii=0; ii<mup->L; ii++ )
        {
            aFo.push_back( TFormula() );
            form = aFo[ii].form_extr( ii, mup->L, mup->DCF );
            aFo[ii].SetFormula( form.c_str() ); // and ce_fscan
        }

        A = new double[mup->N*mup->L];
        fillValue(A, 0., (mup->N*mup->L) );
        for( ii=0; ii<mup->L; ii++ )
            aFo[ii].Stm_line( mup->N, A+ii*mup->N, (char *)mup->SB, mup->Val );
        aFo.clear();
        // loading data for dependent components
        for( L=0, j=-1, jj=0; jj<mup->L; jj++ )
        {
            if( syp->Dcl[jj] == S_OFF )
                continue;
            j++;
            L++;
            pmp->DCC[j] = mup->DCC[jj];
            pmp->muj[j] = jj;
            pmp->lnGam[j] = 0.0;
            pmp->lnGmo[j] = 0.0;
            pmp->X[j] = 0.0;
            pmp->Y[j] = 0.0;
            pmp->XY[j] = 0.0;
            memcpy( pmp->SM[j], mup->SM[jj]+MAXSYMB+MAXDRGROUP, MAXDCNAME );
            if( j < syp->Ls )
                memcpy( pmp->SM2[j], mup->SM[jj]+MAXSYMB+MAXDRGROUP, MAXDCNAME );
            if( j < syp->Ls && j >= syp->Ls - syp->Lsor)
            {   // assembling DC name list for sorption surface species
                ja = (short)(j-(syp->Ls-syp->Lsor));
                if(!(pmp->SM3 && pmp->DCC3))
                {  // Here a workaround of crash detected on 7.02.2011
                   char buf[32];
                   memcpy( buf, mup->SM[jj], MAXSYMB+MAXDRGROUP+MAXDCNAME ); buf[MAXSYMB+MAXDRGROUP+MAXDCNAME] = 0;
                   vfMessage( window(),
                              "Multi make warning: Lsor != pmp->Lads, DC:", buf );
                }
                else {
                   memcpy( pmp->SM3[ja], mup->SM[jj]+MAXSYMB+MAXDRGROUP, MAXDCNAME );
                   // surface species DC class code (usage will be obsolete)
                   pmp->DCC3[ja] = mup->DCC[jj];
                }
            }
            copyValues( acp->dcMod[j], TMTparm::sm->GetTP()->DCMod[jj], 6);
            mm = 0.0;
            for( ii=0; ii<pmp->N; ii++ ) // compressing the stoichiometry matrix
            {
                a = A[jj*mup->N + pmp->mui[ii]];
                pmp->A[j*pmp->N+ii] = a;
                mm += a * pmp->Awt[ii];   // calculating molar mass of DC
            }
            pmp->MM[j] = mm;
        }
        delete[] A;
        // A = 0;
//     ErrorIf( L != pmp->L, "Multi", "Multi make error: L != pmp->L" );
        if( L != pmp->L )
            vfMessage( window(),
                        "Multi make error: L != pmp->L", "Please, press BCC!" );

    }

    if( pmp->E )
    {   // index of charge
        for(  ii=0; ii<pmp->N; ii++ )
            if( pmp->ICC[ii] == IC_CHARGE )
                goto CH_FOUND;
        // error no charge
        if( vfQuestion( window(), "Multi", "No DC with formula charge are included:\n"
                       "Proceed (Y), Cancel (N)?"  ))
        {    pmp->E = 0;
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
        pmp->DCC[j] = mup->DCC[jj];
        if( pmp->E )
            pmp->EZ[j] = pmp->A[pmp->N*j+iZ];
        acp->Pparc_[j] = pmp->Pc;
        pmp->F0[j] = 0.0;
        pmp->XY[j] = 0.0;

///        if( tpp->PtvVm == S_ON )
///            switch( pmp->PV )
///            { // calculating actual molar volume of DC
///            case VOL_CONSTR:
///              if( syp->Vuns )
///                       Vv = syp->Vuns[jj];
///                pmp->A[j*pmp->N+xVol] = tpp->Vm[jj] + Vv;
///            case VOL_CALC:
///            case VOL_UNDEF:
///                if( syp->Vuns )
///                       Vv = syp->Vuns[jj];
///                pmp->Vol[j] = ( tpp->Vm[jj] + Vv )* 10.; // Check!
///               break;
///            }
///        else pmp->Vol[j] = 0.0;

// testing kinetic/metastability constraints
        if( pmp->PLIM && jj < mup->L )
        { // KM constraints necessary!
            pmp->RLC[j] = syp->RLC[jj];
            pmp->RSC[j] = syp->RSC[jj];
            if( syp->DUL )
                pmp->DUL[j] = syp->DUL[jj];
            else pmp->DUL[j] = 1e6;
            ja = (short)(j-(pmp->Ls-pmp->Lads));
            if( pmp->lnSAC && ja < pmp->Lads && ja >= 0
                  && pmp->DUL[j] < 1e6 - TProfil::pm->pa.p.DKIN )
               pmp->lnSAC[ja][3] = pmp->DUL[j]; // Copy of DUL for SACT refining
            if( syp->DLL )
                pmp->DLL[j] = syp->DLL[jj];
            else pmp->DLL[j] = 0.0;
        }
        else
        {
            pmp->RLC[j] = NO_LIM;
            pmp->RSC[j] = QUAN_MOL;
            pmp->DUL[j] = 1e6;  // tc_DCmaxm trivial restrictions!
            pmp->DLL[j] = 0.0;
        }
        // additional data
        pmp->fDQF[j] = 0.0;  // DQF parameters or pure-gas fugacities zeroed off

        if( syp->PParc != S_OFF )
            acp->Pparc_[j] = syp->Pparc[jj];
        pmp->lnGmM[j] = 0.0;
        if( jj < mup->L && syp->PlnGf != S_OFF )  // was jj < mup->Ls
        {
            pmp->lnGmf[j] = syp->lnGmf[jj];
        }
        else pmp->lnGmf[j] = 0.0;

        if( !( j < pmp->Ls && j >= pmp->Ls - pmp->Lads ) )
            continue;   // the following is not done for non-surface species

        ja = (short)(j-(pmp->Ls-pmp->Lads));
        jja = (short)(jj-(mup->Ls-mup->Lads));
// Loading MaSdj - max.sur.densities for non-competitive sorbates */
        if( syp->PMaSdj != S_OFF )
        {
            for( kk=0; kk<DFCN; kk++ )
               if( !IsFloatEmpty( syp->MaSdj[jja][kk] ) )
                   pmp->MASDJ[ja][kk] = syp->MaSdj[jja][kk];
               else pmp->MASDJ[ja][kk]= 0.0;
        }
        if( syp->PSATT != S_OFF )
        { // Loading SATC - codes of methods for SAT calculation
            pmp->SATT[ja] = syp->SATC[jja][SA_MCA];

         // Loading allocation to surface types
            switch( syp->SATC[jja][SA_STX] )
            {
              default:
              case CST_0:
                pmp->SATX[ja][XL_ST] = 0;
                break;
              case CST_1:
                pmp->SATX[ja][XL_ST] = 1;
                break;
              case CST_2:
                pmp->SATX[ja][XL_ST] = 2;
                break;
              case CST_3:
                pmp->SATX[ja][XL_ST] = 3;
                break;
              case CST_4:
                pmp->SATX[ja][XL_ST] = 4;
                break;
              case CST_5:
                pmp->SATX[ja][XL_ST] = 5;
                break;
            }

            // Loading allocation to carrier end-members
            switch( syp->SATC[jja][SA_EMX] )
            {
              case CCA_0:
                pmp->SATX[ja][XL_EM] = 0;
                break;
              case CCA_1:
                pmp->SATX[ja][XL_EM] = 1;
                break;
              case CCA_2:
                pmp->SATX[ja][XL_EM] = 2;
                break;
              case CCA_3:
                pmp->SATX[ja][XL_EM] = 3;
                break;
              case CCA_4:
                pmp->SATX[ja][XL_EM] = 4;
                break;
              case CCA_5:
                pmp->SATX[ja][XL_EM] = 5;
                break;
//              case CCA_6:
//                pmp->SATX[ja][XL_EM] = 6;
//                break;
//              case CCA_7:
//                pmp->SATX[ja][XL_EM] = 7;
//                break;
//              case CCA_8:
//                pmp->SATX[ja][XL_EM] = 8;
//                break;
//              case CCA_9:
//                pmp->SATX[ja][XL_EM] = 9;
//                break;
              case CCA_VOL:
                pmp->SATX[ja][XL_EM] = -1; // whole sorbent
                break;
              default:
                pmp->SATX[ja][XL_EM] = -2; // main DC of the sorbent
                break;
            }

            // Loading allocation to surface sites
            switch( syp->SATC[jja][SA_SITX] )
            {
              default:
              case CSI_0:
                pmp->SATX[ja][XL_SI] = 0;
                break;
              case CSI_1:
                pmp->SATX[ja][XL_SI] = 1;
                break;
              case CSI_2:
                pmp->SATX[ja][XL_SI] = 2;
                break;
              case CSI_3:
                pmp->SATX[ja][XL_SI] = 3;
                break;
              case CSI_4:
                pmp->SATX[ja][XL_SI] = 4;
                break;
              case CSI_5:
                pmp->SATX[ja][XL_SI] = 5;
                break;
            }
            // Loading charge allocation to surface planes
            switch( syp->SATC[jja][SA_PLAX] )
            {
              default:
              case SPL_0:   // only to 0 plane (sorbent surface plane)
                   pmp->SATX[ja][XL_SP] = 0;
                   break;
              case SPL_1:
              case SPL_B:   // to beta plane or plane 1 in CD 3-layer
                   pmp->SATX[ja][XL_SP] = 1;
                   break;
              case SPL_2:
              case SPL_D:   // to DL plane or plane 2 in CD 3-layer
                   pmp->SATX[ja][XL_SP] = 2;
                   break;
              case SPL_3:
              case SPL_C:   // to OS anion beta plane (4-layer model, reserved)
                   pmp->SATX[ja][XL_SP] = 3;
                   break;
            }
        }
    }
}

//==========================================
bool TMultiSystem::CompressPhaseIpxt( int kPH )
{
  int jj, jb, cnt=0;
  vector<int>  aDCused;
  TPhase* aPH=(TPhase *)(aMod[RT_PHASE]);
  RMULTS* mup = TRMults::sm->GetMU();

  for( jj=0, jb = 0; jj<kPH; jj++ )
        jb += mup->Ll[jj];

  for( jj=0, cnt = 0; jj<mup->Ll[kPH]; jj++ )
  {
     if( TSyst::sm->GetSY()->Dcl[jj+jb] == S_OFF )
          aDCused.push_back(-1);
     else
     { aDCused.push_back(cnt); cnt++; }
  }

  if( cnt < mup->Ll[kPH] )
   return aPH->CompressRecord( cnt, aDCused, true );
  else return true;
}

//============================================

//Loading data for phases and mixing models into the structure MULTI
//
void TMultiSystem::multi_sys_ph()
{
    int k, i;
    bool non_sorption_phase, is_ss;
    short kk, j, je, jb, ja=0;
    time_t crt;
//    double G;
    double PMM;  // Phase mean mol. mass
    int Cjs, car_l[32], car_c=0; // current index carrier sorbent
    TPhase* aPH=(TPhase *)(aMod[RT_PHASE]);
    RMULTS* mup = TRMults::sm->GetMU();
    SYSTEM *syp = TSyst::sm->GetSY();

    aPH->ods_link(0);

    pmp->Ls = 0;
    pmp->PG = 0;
    pmp->FIs = syp->Fis;
    pmp->FIa = 0;
    pmp->Lads = 0;   // Is this really needed?
//    ja=0;
    for( kk=0, /*FI=0,*/ k=-1, /*jb=0,*/ je=0; kk<mup->Fi; kk++ )
    {
        if( TSyst::sm->GetSY()->Pcl[kk] == S_OFF )
            continue;
        k++; // FI++;  //PMM = 0.0;
        pmp->L1[k] = syp->Ll[kk];
        jb = je;
        je += pmp->L1[k];
        pmp->muk[k] = kk;
        pmp->PHC[k] = mup->PHC[kk];
        memcpy( pmp->SF[k], mup->SF[kk], MAXSYMB );
        memcpy( pmp->SF[k]+MAXSYMB, mup->SF[kk]+MAXSYMB+MAXPHSYMB, MAXPHNAME );
        if( kk < mup->Fis ) // bugfix 4.04.14 DK was k < syp.Fis
        {
            memcpy( pmp->SF2[k], mup->SF[kk], MAXSYMB );
            memcpy( pmp->SF2[k]+MAXSYMB, mup->SF[kk]+MAXSYMB+MAXPHSYMB, MAXPHNAME );
        }
        if( pmp->pNP && pmp->pBAL && pmp->pKMM )
                goto PARLOAD;

        aPH->TryRecInp( mup->SF[kk], crt, 0 );  // Now reading all phase records!

        // New stuff for TKinMet
        // Resetting vector of TSolMod pointers to avoid problems with activity coeffs
        //     in TSolMod calculations after switching phases on/off
        if(pmp->ITau < 0 )
        {
            if(phKinMet[k])
                delete phKinMet[k];
            phKinMet[k] = NULL;
            pmp->kMod[k][0] = aPH->php->kin_t[2];
            pmp->kMod[k][1] = aPH->php->kin_t[3];
            pmp->kMod[k][2] = aPH->php->kin_t[4];
            pmp->kMod[k][3] = aPH->php->kin_t[5];
            pmp->kMod[k][4] = aPH->php->kin_t[6];
            pmp->kMod[k][5] = aPH->php->kin_t[7];
        }

        if( aPH->php->PrpCon == S_ON )
        {
            pmp->LsKin[k*6] = aPH->php->nPRk;
            pmp->LsKin[k*6+1] = aPH->php->nSkr;
            pmp->LsKin[k*6+2] = aPH->php->nrpC;
            pmp->LsKin[k*6+3] = aPH->php->naptC;
            pmp->LsKin[k*6+4] = aPH->php->nAscC;
            pmp->LsKin[k*6+5] = aPH->php->nFaces;
        }
        else {
            pmp->LsKin[k*6] = pmp->LsKin[k*6+1] = pmp->LsKin[k*6+2] =0;
            pmp->LsKin[k*6+3] = pmp->LsKin[k*6+4] = pmp->LsKin[k*6+5] =0;
        }
/*
long int
*xSKrC,  ///< new: Collected array of aq/gas/sorption species indexes used in activity products (-> += LsKin[k][1])
*ocPRkC; ///< new: Collected array of operation codes for kinetic parallel reaction terms (-> += LsKin[k][0])
*/

        if( kk < mup->Fis && mup->Ll[kk] > 1 )
        { // getting data and parameters for a multicomponent phase
//           if( pmp->pNP && pmp->pBAL )
//                goto PARLOAD;
// Resetting vector of TSolMod pointers to avoid problems with activity coeffs
//     in TSolMod calculations after switching phases on/off (DK 25.05.2009)
//  if(phSolMod[k])
//     delete phSolMod[k];
//  phSolMod[k] = NULL;
//          aPH->TryRecInp( mup->SF[kk], crt, 0 );
            // read informations from phase-solution
            memcpy( pmp->sMod[k], aPH->php->sol_t, 6 );
            pmp->sMod[k][6] = aPH->php->kin_t[0];
            pmp->sMod[k][7] = aPH->php->kin_t[1];
            // Added SD 20/01/2010
            if( aPH->php->Ppnc == S_ON && aPH->php->npxM > 0 )
                CompressPhaseIpxt( kk );
            if( aPH->php->Ppnc == S_ON )
            {
               acp->LsMod_[k*3] = aPH->php->ncpN;
               acp->LsMod_[k*3+2] = aPH->php->ncpM;
               acp->LsMod_[k*3+1] = aPH->php->npxM;
            }
            else acp->LsMod_[k*3] = acp->LsMod_[k*3+1] = acp->LsMod_[k*3+2] =0;
            if( aPH->php->Psco == S_ON )
                acp->LsMdc_[k*3] = aPH->php->nscM;
            else acp->LsMdc_[k*3] = 0;

            pmp->LsPhl[k*2] = aPH->php->nlPh;
            pmp->LsPhl[k*2+1] = aPH->php->nlPc;
            acp->LsMdc2_[k*3] = aPH->php->ndqf;
            acp->LsMdc2_[k*3+1] = aPH->php->nrcp;

            pmp->LsUpt[k*2] = aPH->php->numpC;
            if( aPH->php->PumpCon && aPH->php->lICu && ( aPH->php->kin_t[4] == KM_UPT_ENTRAP
                 || aPH->php->kin_t[4] == KM_IEX_FAST || aPH->php->kin_t[4] == KM_IEX_SLOW ) )
            pmp->LsUpt[k*2+1] = pmp->L1[k]; // bugfix 04.03.2015 DK
                  //  aPH->php->nFaces;     // provisional 8.04.2013 DK

// New stuff for TSorpMod
//if(phSorpMod[k])
//   delete phSorpMod[k];
//phSorpMod[k] = NULL;
/*
long int
*LsESmo, ///< new: number of EIL model layers; EIL params per layer; CD coefs per DC; reserved  [Fis][4]
*LsISmo, ///< new: number of surface sites; isotherm coeffs per site; isotherm coeffs per DC; max.denticity of DC [Fis][4]
*xSMd;   ///< new: denticity of surface species per surface site (site allocation) (-> L1[k]*LsISmo[k][3]] )
*/
        }
        else  // nothing to read in the Phase record
            if( k<pmp->FIs )
            {
                acp->LsMod_[k*3] = 0;
                acp->LsMod_[k*3+1] = 0;
                acp->LsMod_[k*3+2] = 0;
                acp->LsMdc_[k*3] = 0;
                memset( pmp->sMod[k], ' ', 8 );
                pmp->LsPhl[k*2] = 0;
                pmp->LsPhl[k*2+1] = 0;
                acp->LsMdc2_[k*3] = 0;
                acp->LsMdc2_[k*3+1] = 0;
                pmp->LsUpt[k*2] = 0;
                pmp->LsUpt[k*2+1] = 0;
// New stuff for TSorpMod

            }


PARLOAD: if( k < syp->Fis )
             pmp->Ls += pmp->L1[k];
        non_sorption_phase = true;
        switch( pmp->PHC[k] )
        { // loading necessary parameters
        case PH_AQUEL:
             break;
        case PH_GASMIX:
        case PH_PLASMA:
        case PH_FLUID:
//            pmp->PG = pmp->L1[k];
              pmp->PG += pmp->L1[k];
            break;
        case PH_HCARBL:
            pmp->PSOL = pmp->L1[k];
            break;
        case PH_LIQUID:
        case PH_SINCOND:
        case PH_SIMELT:
            break;
        case PH_POLYEL:
        case PH_SORPTION:
            pmp->FIa++; non_sorption_phase = false;
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
           pmp->Aalp[k] = syp->Aalp[kk];
        if( syp->PSigm != S_OFF )
        {
           pmp->Sigw[k] = syp->Sigm[kk][0];
           pmp->Sigg[k] = syp->Sigm[kk][1];
        }
        if( syp->PXepsC != S_OFF )
        {
           pmp->Xcond[k] = syp->XEpsC[kk][1];
           pmp->Xeps[k] = syp->XEpsC[kk][0];
        }
        if( syp->PXr0h0 != S_OFF )
        {
           pmp->Xr0h0[k][0] = syp->Xr0h0[kk][0];
           pmp->Xr0h0[k][1] = syp->Xr0h0[kk][1];
        }
        PMM = 0.0;
        Cjs = -1;
        // Loop over DCs in Phase, calculation of phase mean mol. mass
        for( j=jb; j<je; j++ )
        {
            is_ss = true;
            switch( pmp->DCC[j] )
            {
            case DC_AQ_SOLVENT:
                pmp->LO = j; is_ss = false;
                PMM = pmp->MM[j];
                break;
            case DC_PEL_CARRIER:
            case DC_SUR_CARRIER:
                Cjs = j;
                PMM = pmp->MM[j];
                car_l[car_c++]=j;  pmp->Lads++; ja++;
                break;
            case DC_SUR_MINAL:
                car_l[car_c++]=j;  pmp->Lads++; ja++;
                break;
  /* Remapping DC codes to indices in pm->SATX - compat with old-style SCMs *
   * even - inner-sphere complex on surface type 0,1,2,3,4 - A plane */
            case DC_SSC_A0:
//                pmp->SATX[ja][XL_ST] = AT_SA0;
                pmp->Lads++; ja++;
                break;
            case DC_SSC_A1:
//                pmp->SATX[ja][XL_ST] = AT_SA1;
                pmp->Lads++; ja++;
                break;
            case DC_SSC_A2:
//                pmp->SATX[ja][XL_ST] = AT_SA2;
                pmp->Lads++; ja++;
                break;
            case DC_SSC_A3:
//                pmp->SATX[ja][XL_ST] = AT_SA3;
                pmp->Lads++; ja++;
                break;
            case DC_SSC_A4:
//                pmp->SATX[ja][XL_ST] = AT_SA4;
                pmp->Lads++; ja++;
                break;
  /* odd - outer-sphere complex on surface type 0,1,2,3,4 - beta plane */
            case DC_WSC_A0:
//                pmp->SATX[ja][XL_ST] = AT_SB0;
                pmp->Lads++; ja++;
                break;
            case DC_WSC_A1:
//                pmp->SATX[ja][XL_ST] = AT_SB1;
                pmp->Lads++; ja++;
                break;
            case DC_WSC_A2:
//                pmp->SATX[ja][XL_ST] = AT_SB2;
                pmp->Lads++; ja++;
                break;
            case DC_WSC_A3:
//                pmp->SATX[ja][XL_ST] = AT_SB3;
                pmp->Lads++; ja++;
                break;
            case DC_WSC_A4:
//                pmp->SATX[ja][XL_ST] = AT_SB4;
                pmp->Lads++; ja++;
                break;
  /* Inner-sphere exchange ion at const-charge plane */
            case DC_IESC_A:
//                pmp->SATX[ja][XL_ST] = AT_SA5;
                pmp->Lads++; ja++;
                break;
  /* Outer-sphere exchange ion at const-charge plane */
            case DC_IEWC_B:
//                pmp->SATX[ja][XL_ST] = AT_SB5;
                pmp->Lads++; ja++;
                break;
  /* Aliaces for single-site model *
        * Surface site A plane -> '0' */
            case DC_SUR_GROUP:
//                pmp->SATX[ja][XL_ST] = AT_SA0;
                pmp->Lads++; ja++;
                break;
        /* Strong sur. complex A plane -> '0' */
            case DC_SUR_COMPLEX:
//                pmp->SATX[ja][XL_ST] = AT_SA0;
                pmp->Lads++; ja++;
                break;
       /* Weak sur complex B plane -> '1' */
            case DC_SUR_IPAIR:
//                pmp->SATX[ja][XL_ST] = AT_SB0;
                pmp->Lads++; ja++;
                break;
/* End extension */
            case DC_SOL_MAJOR: case DC_SOL_MAJDEP:
                Cjs = j;
            case DC_SOL_MINOR: case DC_SOL_MINDEP: case DC_SOL_IDEAL:
                PMM += pmp->MM[j]; is_ss = false;
                break;
            default: /* if( isdigit( pmp->DCC[j] ))
                                    pmp->Lads++; else */
                is_ss = false;
                PMM += pmp->MM[j]; // /pmp->L1[k]; bugfix DK 28.02.2012
                break;
            }
 // Inserted 08.02.2011 by DK to prevent crashes on inconsistent phases
            if( non_sorption_phase && is_ss )
            {
                char buf[128];
                sprintf(buf, ": %d %2.16s %c  ( DC: %d  %2.16s %c ) ",
                        k, pmp->SF[k],  pmp->PHC[k], j, pmp->SM[j], pmp->DCC[j] );
                vfMessage( window(),
                           "Inconsistent DC in non-sorption phase", buf );
                pmp->Lads--;
            }
            if( !non_sorption_phase && !is_ss )
            {
                char buf[128];
                sprintf(buf, ": %d %2.16s %c  ( DC: %d  %2.16s %c ) ",
                        k, pmp->SF[k],  pmp->PHC[k], j, pmp->SM[j], pmp->DCC[j] );
                vfMessage( window(),
                           "Inconsistent DC in sorption phase", buf );
            }
        }  /* j */
        if( PMM > 0. )  // cannot calculate better before the composition of the phase is known
            PMM /= pmp->L1[k]; // DK 28.02.2012

        if( pmp->PHC[k] == PH_POLYEL || pmp->PHC[k] == PH_SORPTION )
            /* Analyzing assignment of surtypes */
            ph_surtype_assign( k, kk, jb, je, car_l, car_c, Cjs );

        if( syp->PYOF == S_OFF )
            pmp->YOF[k] = 0.0;
        else
            switch( pmp->PHC[k] )
            { // convert free energy of metastable phase from J/g to J/mol */
            case PH_POLYEL:
            case PH_SORPTION:
                if( Cjs >= 0 )
                {
                    PMM = pmp->MM[Cjs];
                    pmp->FWGT[k] = PMM;
                }
                /*  leave normalized to 1 gram pmp->ln1Na[k] *= PMM; */
            case PH_AQUEL:
                if( pmp->FWGT[k] < 1e-33 )   // bugfix 26.04.13 DK
                {
                    pmp->YOF[k] = syp->YOF[kk] * PMM / pmp->RT;
                    pmp->FWGT[k] = PMM;
                }
                break;
            default:
                if( Cjs >=0 ) // if there is a MAJOR component in solution phase
                    PMM = pmp->MM[Cjs];
                if( pmp->FWGT[k] < 1e-33 )  // bugfix 26.04.13 DK
                {
                   pmp->YOF[k] = syp->YOF[kk] * PMM / pmp->RT;
                   pmp->FWGT[k] = PMM;
                }
                break;
            }

        // test of kinetic constraints (units of measurement!)
        if( pmp->PLIM && k >= pmp->FIs )
        { /* restrictions set! */
            /* one-component phases */
            if( syp->PLLim != S_OFF || syp->PULim != S_OFF )
            {
                pmp->RLC[pmp->Ls+k-pmp->FIs] = syp->RFLC[kk];
                pmp->RSC[pmp->Ls+k-pmp->FIs] = syp->RFSC[kk];
            }
            if( syp->PUL )
                pmp->DUL[pmp->Ls+k-pmp->FIs] = syp->PUL[kk];
            if( syp->PLL )
                pmp->DLL[pmp->Ls+k-pmp->FIs] = syp->PLL[kk];
        }
        if( pmp->PLIM && k < pmp->FIs )
            /*    ( syp->PLLim != S_OFF || syp->PULim != S_OFF ) )  */
        { /* solutions */
            if( syp->RFLC )
                pmp->RFLC[k] = syp->RFLC[kk];
            else pmp->RFLC[k] = NO_LIM;
            if( syp->RFSC )
                pmp->RFSC[k] = syp->RFSC[kk];
            else pmp->RFSC[k] = QUAN_MOL;
            if( syp->PUL )
                pmp->PUL[k] = syp->PUL[kk];
            else pmp->PUL[k] = 1e6;
            if( syp->PLL )
                pmp->PLL[k] = syp->PLL[kk];
            else pmp->PLL[k] = 0.0;
        }
        if( !pmp->PLIM && k<pmp->FIs )
        { /* no set restrictions */
            pmp->RFLC[k] = NO_LIM;
            pmp->RFSC[k] = QUAN_MOL;
            pmp->PUL[k] = 1e6;
            pmp->PLL[k] = 0.0;
        }

    }  /* k */
}

// Loading surface type parameters
void TMultiSystem::ph_sur_param( int k, int kk )
{
    int i;
    SYSTEM *syp = TSyst::sm->GetSY();

    for( i=0; i< syp->NsTm; i++ )
    {
        if( syp->SCMT[kk][i] == SC_NOT_USED || fabs(syp->Nfsp[kk][i])<1e-9 )
        { /*this type surface of dont used*/
            pmp->Nfsp[k][i] =  DOUBLE_EMPTY;
            pmp->MASDT[k][i] = DOUBLE_EMPTY;
            //    pmp->MISDT[k][i] = FLOAT_EMPTY;
            pmp->XcapA[k][i] = DOUBLE_EMPTY;
            pmp->XcapB[k][i] = DOUBLE_EMPTY;
            pmp->Xetaf[k][i] = DOUBLE_EMPTY;
            pmp->SCM[k][i] = SC_NOT_USED;
            /*  pmp->CSTS[k][i] = CCA_VOL; */
            continue;
        }
        pmp->SCM[k][i] = syp->SCMT[kk][i];
        /*  pmp->CSTS[k][i] = syp->CSTS[kk][i]; */
        pmp->Nfsp[k][i] = syp->Nfsp[kk][i];  /* fraction! */

        /* (1e18/m2 m2/g)/(6.022e23 1/mol)=1.66054 mkmol/g */

        if( !syp->MaSdt || syp->MaSdt[kk][i] < 1e-9 )
            pmp->MASDT[k][i] = TProfil::pm->pa.p.DNS * 1.66054 * syp->Aalp[kk];
        /* default, mkmol/g */
        else pmp->MASDT[k][i] = syp->MaSdt[kk][i]*syp->Aalp[kk]*1.66054;
        /* max. in mkmol/g */

        // if( !syp->MiSdt || syp->MiSdt[kk][i] < 1e-9 )
        //     pmp->MISDT[k][i] = 1.0; /* default, mkmol/g */
        //  else pmp->MISDT[k][i] = syp->MiSdt[kk][i]*syp->Aalp[kk]*1.66054;
        /* min. in mkmol/g */
        /*
          pmp->XCEC[k][i] = syp->Nsph[kk][i] * syp->Aalp[kk]*
                               syp->Nfsp[kk][i]*1.6606;        mkmol/g */

        if( syp->PXfIEC != S_OFF )
        {  /* Permanent surface charge density Ion-exchange capacity */
           // XfIEC[] in mkeq/m2; Xetaf[] in C/m2
            if( fabs(syp->XfIEC[kk][i])>1e-9 )
                pmp->Xetaf[k][i] = syp->XfIEC[kk][i]*.09648531;
            else pmp->Xetaf[k][i] = 0.0;
        }
        if( syp->PXcapA != S_OFF )
            pmp->XcapA[k][i] = syp->XcapA[kk][i];
        else pmp->XcapA[k][i] = 0.;
        if( syp->PXcapB != S_OFF )
            pmp->XcapB[k][i] = syp->XcapB[kk][i];
        else pmp->XcapB[k][i] = 0.;

        pmp->XdlA[k][i] = 0.0;
        pmp->XdlB[k][i] = 0.0;
        pmp->XdlD[k][i] = 0.0;
        pmp->XcapD[k][i] = 0.;
        /*
          if( syp->PXlam != S_OFF )
            pmp->XlamA[k][i] = syp->Xlam[kk][i];
          else pmp->XlamA[k][i] = 0.;
          if( syp->PXetf != S_OFF )
              pmp->Xetaf[k][i] = syp->Xetaf[kk][i];
          else pmp->Xetaf[k][i] = 0.;
        */
    } /* end for i */
}

// Assigning surface types to carrier (DC) - added 5/13/97 DAK
// Extended for CD-MUSIC by KD 31.10.2004
void TMultiSystem::ph_surtype_assign( int k, int kk, int jb, int je,
                                 int car_l[], int car_c, int Cjs )
{
    int j, jcl, ja/*=0*/;
    long int  ist/*=0*/, isi/*=0*/;

    /*  double SATst; */

    for( j=jb; j<je; j++ )
    {
        if( pmp->DCC[j] == DC_PEL_CARRIER || pmp->DCC[j] == DC_SUR_CARRIER
                || pmp->DCC[j] == DC_SUR_MINAL )
            continue;
        ja = j - ( pmp->Ls - pmp->Lads );
ist = pmp->SATX[ja][XL_ST]; // / MSPN;   index of surface type
isi = pmp->SATX[ja][XL_SI];                 // index of site on this surface type
pmp->D[ist][isi] = 0.0;                    // cleanining the totals for sites
        jcl = pmp->SATX[ja][XL_EM];
        if( jcl < car_c && jcl >= 0 )
            pmp->SATX[ja][XL_EM] = car_l[jcl];
        else if( jcl >= car_c || jcl == -1 )
            pmp->SATX[ja][XL_EM] = -1;   /* whole carrier */
        else pmp->SATX[ja][XL_EM] = Cjs; /* main carrier DC */

        if( pmp->SATT[ja] != SAT_COMP )
// To be fixed !!!!!!!!!!!
            pmp->MASDJ[ja][PI_DEN] *= TSyst::sm->GetSY()->Aalp[kk]*1.66054;  /* 1/nm2 to mkmol/g */
//            pmp->MASDJ[j] *= syp->Aalp[kk]*1.66054;  /* mkmol/g */
        if( !pmp->MASDJ[ja] )
        {
            //      if( pmp->SATT[j] == SAT_SITE )
            //         pmp->MASDJ[j] = pmp->MASDT[k][ist] - pmp->MISDT[k][ist] ;
            if( pmp->SATT[ja] == SAT_NCOMP )
                pmp->MASDJ[ja][PI_DEN] = pmp->MASDT[k][ist]; // Do we really need this?
//              pmp->MASDJ[j] = pmp->MASDT[k][ist];
            if( pmp->SATT[ja] == SAT_COMP )
                pmp->MASDJ[ja][PI_DEN] = 1.0;
//                pmp->MASDJ[j] = 1.0;
        }
    } /* for j */
}


// End of file ms_mupmph.cpp
// --------------------------------------------------------------------

