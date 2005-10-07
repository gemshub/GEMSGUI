//-------------------------------------------------------------------
// $Id$
//
// Copyright (C) 1992-2000  D.Kulik, S.Dmitrieva, K.Chudnenko, I.Karpov
//
// Implementation of chemistry-specific functions
// for convex programming Gibbs energy minimization
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
#include <math.h>
//#include <stdio.h>
#include <iostream>

#include "m_param.h"

#ifndef IPMGEMPLUGIN

#include "service.h"
#include "visor.h"
//#include "visor_w.h"
#include "stepwise.h"

#endif


// -----------------------------------------------------------------
//const double tc_ISdelta = 1e-5;
//const double LOWESTDC_ = 1e-6;

// Finding automatic initial approximation for the IPM algorithm
// using modified simplex method with two-side constraints
//
bool TProfil::AutoInitialApprox( )
{
    int i, j, k, NN;
    double minB/*=0.0*/, molB=0.0, sfactor/*=1.0*/;

// Kostya correct DK & DHB as automatic
    NN = pmp->N - pmp->E;
    minB=pmp->B[0];
    for(i=0;i<NN;i++)
    {
      if( pmp->B[i] < minB )
          minB = pmp->B[i];
      molB += pmp->B[i];
    }
    if( minB < pa.p.DB )  // KD - foolproof
       minB = pa.p.DB;

//  check Ymin (cutoff)
   if(pmp->lowPosNum>minB*0.01)
      pmp->lowPosNum=minB*0.01;

   sfactor = pow( molB, 0.4 )/7.7;
   pmp->DHBM = sfactor * pa.p.DHB;  // 2.5 root
   pmp->DHBM *= (0.097+0.95/(1+exp(-(log10(minB)+11)/0.4)));

   pmp->DX = pa.p.DK;
   pmp->DX *= sfactor;
   pmp->DX *= (0.097+0.95/(1+exp(-(log10(minB)+6.1)/0.54)));
   if( pmp->DX < 0.01 * pa.p.DK )
       pmp->DX = 0.01 * pa.p.DK;
   pmp->DSM = pa.p.DS;  // Shall we add  * sfactor ?

#ifndef IPMGEMPLUGIN
   pVisor->Update(false);
#endif
    // Analyzing if Simplex approximation is needed
    if( !pmp->pNP  )  //  || ( pmp->pNP == -1 &&
                      //  !vfQuestion(window(),  "IPM (initial approximation):",
                      //  "Previous minimization result (Y)\n Automatic SIMPLEX approximation (N)? " )))
    {   // Preparing to call Simplex method
        pmp->FitVar[4] = pa.p.AG;  // smoothing parameter init
        pmp->pRR1 = 0;
        TotalPhases( pmp->X, pmp->XF, pmp->XFA );
//      pmp->IC = 0.0;  Important - for reproducibility of simplex FIA
        if( pa.p.PSM && pmp->FIs )
            GammaCalc(LINK_FIA_MODE);
        if( pa.p.PC == 2 )
           XmaxSAT_IPM2_reset();  // Reset upper limits for surface species
        pmp->IT = 0;
        pmp->pNP = 0;
        pmp->K2 = 0;
        pmp->PCI = 0;

/*
if( pa.p.PRD >= 7 )
{                      // Dima 18/05/2002 test init load before simplex
  if( multi->flCopy == true )
  {
//    std::cout << " P. 7 with simplex: ";
//    multi->dyn__test( multi->GetPMcopy1() );
  }
  multi->dyn_new_test( multi->GetPMcopy1() );
}
*/
  // simplex method is called here
        SimplexInitialApproximation( );
//  STEPWISE (0) - stop point for examining results from simplex IA
#ifndef IPMGEMPLUGIN
STEP_POINT( "End Simplex" );
#endif
        // no multi-component phases?
        if( !pmp->FIs )
            return true; // goto OVER; // solved !
        for( i=0; i<pmp->L; i++ )
        {
//            if( pmp->Y[i] <= /* LOWESTDC_ */ pmp->lowPosNum )
                //	       pmp->Y[i] = LOWESTDC_ / 10.;
                //         pmp->Y[i] = pa.p.DFYaq;
//            if( pmp->Y[i] < pa.p.DFYc * sfactor ) // 24.02.03
//            {
                // Trace DC quantity into zeros!
                switch( pmp->DCC[i] )
                {
                case DC_AQ_PROTON:
                case DC_AQ_ELECTRON:
                case DC_AQ_SPECIES:
                    if( pmp->Y[i] < pa.p.DFYaq * sfactor /*= pmp->lowPosNum */ )
                       pmp->Y[i] =  pa.p.DFYaq * sfactor;
                    break;
                case DC_AQ_SOLVCOM:
                case DC_AQ_SOLVENT:
                    if( pmp->Y[i] < pa.p.DFYw * sfactor )
                       pmp->Y[i] =  pa.p.DFYw * sfactor;
                    break;
                case DC_GAS_H2O:
                case DC_GAS_CO2:
                case DC_GAS_H2:
                case DC_GAS_N2:
                case DC_GAS_COMP:
                case DC_SOL_IDEAL:
                    if( pmp->Y[i] < /* = pmp->lowPosNum */ pa.p.DFYid*sfactor )
                       pmp->Y[i] = pa.p.DFYid * sfactor;
                    break;
                case DC_SOL_MINOR:
                    if( pmp->Y[i] < /* = pmp->lowPosNum */ pa.p.DFYh*sfactor )
                       pmp->Y[i] = pa.p.DFYh * sfactor;
                    break;
                case DC_SOL_MAJOR:
                    if( pmp->Y[i] < pa.p.DFYr * sfactor )
                        pmp->Y[i] =  pa.p.DFYr * sfactor;
                    break;

                case DC_SCP_CONDEN:
                    if( pmp->Y[i] < /* pmp->lowPosNum */ pa.p.DFYc * sfactor )
                        pmp->Y[i] =  pa.p.DFYc * sfactor;
                    break;
                    // implementation for adsorption?
                default:
                    if( pmp->Y[i] < pa.p.DFYaq *sfactor /* pmp->lowPosNum */ )
                        pmp->Y[i] =  pa.p.DFYaq * sfactor;
                    break;
                }
//              pmp->Y[i] *= sfactor;  // 2.5 root
//             }
           } // i
    }
    else  // Taking previous result as initial approximation
    {
        int jb, je=0, jpb, jpe=0, jdb, jde=0;
        double LnGam, FitVar3;
        /*    pmp->IT *= pa.p.PLLG; */

/*if( pa.p.PRD >= 8 )
{                      // Dima 18/05/2002 test init load before simplex
  if( multi->flCopy == true )
  {
//    std::cout << " Point 8: ";
    multi->dyn__test( multi->GetPMcopy1() );
  }
  multi->dyn_new_test( multi->GetPMcopy1() );
}
*/        FitVar3 = pmp->FitVar[3];
        pmp->FitVar[3] = 1.0;
        TotalPhases( pmp->Y, pmp->YF, pmp->YFA );
        for( j=0; j< pmp->L; j++ )
            pmp->X[j] = pmp->Y[j];
        TotalPhases( pmp->X, pmp->XF, pmp->XFA );
        ConCalc( pmp->X, pmp->XF, pmp->XFA );
        if( pmp->E && pmp->LO )
            IS_EtaCalc();
        /*    if( pa.p.PSM )  corr. 13.4.96 */
        for( k=0; k<pmp->FI; k++ )
        {
            jb = je;
            je += pmp->L1[k];
            jpb = jpe;
            jpe += pmp->LsMod[k];
            jdb = jde;
            jde += pmp->LsMdc[k]*pmp->L1[k];
            if( pmp->PHC[k] == PH_SORPTION || pmp->PHC[k] == PH_POLYEL )
            {
               if( pmp->E && pmp->LO )
                   GouyChapman( jb, je, k );
//               SurfaceActivityTerm( jb, je, k );
                 SurfaceActivityCoeff(  jb, je, jpb, jdb, k );
            }
            for( j=jb; j<je; j++ )
            {
                LnGam = pmp->lnGam[j];
                if( fabs( LnGam ) > 1e-9 )
                    pmp->Gamma[j] = exp( LnGam );
                else pmp->Gamma[j] = 1.0;
                pmp->F0[j] = Ej_init_calc( 0.0, j, k  );
                pmp->G[j] = pmp->G0[j] + pmp->F0[j];
                pmp->lnGmo[j] = LnGam;
            }  // j
        } // k
        pmp->FitVar[3] = FitVar3; // Restore smoothing parameter

        if( pmp->pNP <= -1 )
        {  // With raising zeroed species and phases
           for( i=0; i<pmp->L; i++ )
           {
//            if( pmp->Y[i] <= pmp->lowPosNum )
//              { // Put trace DC quantity instead of zeros!
                 switch( pmp->DCC[i] )
                 {
                   case DC_AQ_PROTON:
                   case DC_AQ_ELECTRON:
                   case DC_AQ_SPECIES:
                      if( pmp->Y[i] <= pmp->lowPosNum )
                           pmp->Y[i] = pa.p.DFYaq/1000.;
                      break;
                   case DC_AQ_SOLVCOM:
                   case DC_AQ_SOLVENT:
                        if( pmp->Y[i] < pa.p.DFYw )
                           pmp->Y[i] = pa.p.DFYw;
                      break;
                   case DC_GAS_H2O: case DC_GAS_CO2: case DC_GAS_H2:
                   case DC_GAS_N2: case DC_GAS_COMP:
                   case DC_SOL_IDEAL:
                        if( pmp->Y[i] <= pmp->lowPosNum )
                          pmp->Y[i] = pa.p.DFYid/1000.;
                      break;
                   case DC_SOL_MINOR:
                        if( pmp->Y[i] <= pmp->lowPosNum )
                          pmp->Y[i] = pa.p.DFYh/1000.; break;
                   case DC_SOL_MAJOR:
                        if( pmp->Y[i] < pa.p.DFYr )
                          pmp->Y[i] = pa.p.DFYr; break;
                   case DC_SCP_CONDEN:
                        if( pmp->Y[i] < pa.p.DFYc )
                        pmp->Y[i] = pa.p.DFYc; break;
                   // implementation for adsorption?
                   default:
                        if( pmp->Y[i] <= pmp->lowPosNum )
                        pmp->Y[i] = pa.p.DFYaq/1000.; break;
                 }
              } // i
//           }
        }
/*if( pa.p.PRD >= 7 )
{                      // Dima 18/05/2002 test init load before simplex
  if( multi->flCopy == true )
  {
//    std::cout << " P.7  no simplex: ";
    multi->dyn__test( multi->GetPMcopy1() );
  }
  multi->dyn_new_test( multi->GetPMcopy1() );
}
*/    }
    pmp->MK = 1;
    // Sveta 18/01/1999 test init load
    /*   if( ((TMulti *)aSubMod[MD_MULTI])->flCopy == true )
         ((TMulti *)aSubMod[MD_MULTI])->dyn__test(
                ((TMulti *)aSubMod[MD_MULTI])->copy1);
      ((TMulti *)aSubMod[MD_MULTI])->dyn_new_test(
                ((TMulti *)aSubMod[MD_MULTI])->copy1);
    */
// STEPWISE (1) - stop point to see IA from old solution or raised simplex
#ifndef IPMGEMPLUGIN
STEP_POINT("Before FIA");
#endif
    return false;
    //   OVER: /* calc finished */
    //   if( wn[W_EQCALC].status )
    //     aSubMod[MD_EQCALC]->ModUpdate("EQ_done  EQilibrium STATe: computed OK");
    //   return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Main sequence of IPM calculations
//  Main place for implementation of diagnostics and setup
//  of IPM precision and convergence
//  Returns true   if IPM result is OK
//          false  if good result could not be obtained
//

bool TProfil::MultiCalcMain( int& /*pll*/, double& /*FXold*/ )
{
    int i, j, RepeatSel=0, eRet;
    pmp->W1=0;

    if( pmp->pULR && pmp->PLIM )
        if( Set_DC_limits( DC_LIM_INIT ))
#ifndef IPMGEMPLUGIN
            if( !vfQuestion(window(), "IPM: ",
                            "Inconsistent metastability restrictions to DC or phases.\n"
                            "Continue calculation (take those restrictions as trivial)?" ))
#endif
                Error("IPM error: " ,
                      "Inconsistent metastability restrictions to DC or phases.");

    /* test insert in valid area */
    mEFD:
     if(pmp->PZ && pmp->W1)
     { for( i=0; i<pmp->L; i++ )
        pmp->Y[i]=pmp->X[i];
      TotalPhases( pmp->Y, pmp->YF, pmp->YFA );
     }

    eRet = EnterFeasibleDomain( );
    if( eRet )
        goto ERET_THINK;

// STEPWISE (2)  - stop point to examine output from EFD()
#ifndef IPMGEMPLUGIN
#ifdef Use_mt_mode

STEP_POINT("After FIA");

#endif
#endif


   /* minimization  IPM */
    eRet = InteriorPointsMethod( );


// STEPWISE (3)  - stop point to examine output from IPM()
#ifndef IPMGEMPLUGIN
STEP_POINT("After IPM");
#endif
ERET_THINK:  // Diagnostics of IPM results !!!!!!!!!!!!!!!!!!!!!!!!!!!!
    if( eRet )
    {   if(eRet==2 )
        { if( pmp->DX<1e-3 )
            {
               pmp->DX *= 10.;
               goto mEFD;
            }
          else
#ifndef IPMGEMPLUGIN
          if( vfQuestion(window(), "IPM:",
            "For a given IPM convergence criterion, vector b is not balanced,\n"
            "or DC standard-state thermodynamic data inconsistent. \n"
            "Browse debug data screen forms (Y) Skip to abnormal exit from IPM (N)?" ))
            {  pVisor->Update( true );
                return true;
            }
#else
          std::cout << "For a given IPM convergence criterion, vector b is not balanced,\n"
           << "or DC standard-state thermodynamic data inconsistent. \n"
           <<  "Browse debug data screen forms (Y) Skip to abnormal exit from IPM (N)?"
           << endl;

#endif

        }
     /*   if( !pmp->MK  )
            return true; //goto OVER;
        if( pmp->pNP > 0 ) // error of initial approximation
        {
            pmp->pNP = 0;
            pmp->PD = pa.p.PD;
            goto AGAIN;
        }  */
        else
        { if( pmp->DHBM<1e-5 )
            {
               pmp->DHBM *= 10.;
               goto mEFD;
            }
          else
#ifndef IPMGEMPLUGIN
            if( !vfQuestion(window(),  "Invalid initial approximation for IPM:",
                             "Proceed with automatic SIMPLEX approximation ?" ))
#endif
            Error("IPM error: ", "Invalid initial approximation for IPM.");
        }

      /*  else
        {
            pmp->DX *= 10.;
            pmp->pNP = 0;
            pmp->PD = pa.p.PD;
            goto AGAIN;
        }  */
    }
    pmp->FI1 = 0;
    pmp->FI1s = 0;
    for( i=0; i<pmp->FI; i++ )
        if( pmp->YF[i] > 1e-18 )
        {
            pmp->FI1++;
            if( i < pmp->FIs )
                pmp->FI1s++;
        }
    if( !pa.p.PC )    //  ???
        if( pmp->PD < 2 )
            return true; //break;
        else
        {
           // pmp->MK = 1;
            goto ITDTEST;
        }
    /* call Selekt2() */
   PhaseSelect();
   if( pa.p.PC == 2 )
       XmaxSAT_IPM2();  // Install upper limits to xj of surface species

   if( !pmp->MK )
     if( RepeatSel<3 )
       { RepeatSel++;
         goto mEFD;
       }
     else
         std::cout<< "Warning PhaseSelect: Insertion of phases was incomplete!"<< endl;
    //   if( !vfQuestion(window(), "PhaseSelect : warning",
    //        "Insert phase cannot be reached. Continue?" ))
    //         return false;

   MassBalanceDeviations( pmp->N, pmp->L, pmp->A, pmp->X, pmp->B, pmp->C);
// STEPWISE (4) Stop point after PhaseSelect()
#ifndef IPMGEMPLUGIN
#ifdef Use_mt_mode

STEP_POINT("PhaseSelect");

#endif
   pVisor->Update( false );
#endif
   if(pmp->PZ && !pmp->W1)
    { pmp->W1++;           // IPM-2 precision algorithm - 1st run
      goto mEFD;
    }
   if(pmp->PZ && pmp->W1 && pmp->W1 <  pa.p.DW )
    for(i=0;i<pmp->N-pmp->E;i++)
     if( fabs(pmp->C[i]) > pmp->DHBM // * pa.p.GAS
         || fabs(pmp->C[i]) > pmp->B[i] * pa.p.GAS )
      if(pmp->W1 < pa.p.DW-1)
        {
            pmp->W1++;          // IPM-2 precision algorithm - 2nd run
            goto mEFD;
        }
       else
       { gstring  buf,buf1;
         vstr pl(5);
         int jj=0;
          for( j=0; j<pmp->N-pmp->E; j++ )
          //  if( fabs(pmp->C[j]) > pmp->DHBM  || fabs(pmp->C[j]) > pmp->B[j] * pa.p.GAS )
            if( fabs(pmp->C[j]) > pmp->B[j] * pa.p.GAS )
            { sprintf( pl, " %-2.2s  ", pmp->SB[j] );
              buf1 +=pl;
              jj=1;
            }
           if(!jj)
               goto ITDTEST;
           buf = "Prescribed balance precision cannot be reached\n for independent components: ";
           buf += buf1;
        //   buf += "Continue?";
        //  if( !vfQuestion(window(), "IPM : warning", buf.c_str() ))
        //      break;
          std::cout<< buf.c_str()<< endl;
          goto ITDTEST;
       }

ITDTEST: /* test flag modes */
//   if( pa.p.PC == 2 )
//       XmaxSAT_IPM2_Reset();  // Reset upper limits for surface species
 /*   if( !pmp->MK )
    {  // insert or deleted phase
        FXold = pmp->FX;
        goto RESET_GT;
    }
    if( pa.p.DK != pmp->DX && !pll )
    { // bad convergence - how to improve ?  ???????????????????????
        if( pmp->PD <= 1 )
        {
            pmp->MK = 0;
            pll=1;
            FXold = pmp->FX;
            goto RESET_GT;
        }
    }
    if( pmp->PD == 2 )  // test of total G(X) value
        if( fabs( FXold - pmp->FX ) > tc_ISdelta )
        {
            pmp->MK=0;
            FXold = pmp->FX;
            goto RESET_GT;
        }  */
    /* if( pmp->PD == 3 ) */
    /* pmp->MK = 0 */;
// RESET_GT:  ??????????????????????????????????
    memcpy( pmp->G, pmp->G0, pmp->L*sizeof(double));
    return false;

// AGAIN: Call simplex if necessary
//   return AutoInitialApprox();
}

//Calc equstat method IPM (iterations)
void TProfil::MultiCalcIterations()
{
    pll=0;
    FXold=0.0;
 /*   do
    { // cycle of iterations Selekt
        // Stop calculations here Sveta
        if( MultiCalcMain( pll, FXold ) == true )
            break;
        if( fStopCalc == true )	// 'Cancel' button
            return;	// should we do some cleanup?
    } while( !pmp->MK );  */
    MultiCalcMain( pll, FXold );

    //calc demo data
    for( int ii=0; ii<pmp->N; ii++ )
        pmp->U_r[ii] = pmp->U[ii]*pmp->RT;
    GasParcP();

    /* calc finished */
    //   if( wn[W_EQCALC].status )
    //  aMod[MD_EQCALC]->ModUpdate("EQ_done  EQilibrium STATe: computed OK");
}

// End of file ms_muload.cpp
// ----------------------------------------------------------------

//#define mp(i,j) pmp->MASDJ[(j)+(i)*D_F_CD_NP]
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* Calculation of max.moles of surface species for SAT stabilization
*  to improve IPM-2 convergence at high SAT values  KD 08.03.02
*  xj0 values are placed as upper kunetic constraints
*/
void TProfil::XmaxSAT_IPM2( void )
{
    int i, j, ja/*=0*/, k, jb, je=0, ist=0, Cj/*=0*/, iSite[6];
    double XS0/*=0.*/, xj0/*=0.*/, XVk, XSk/*=0.*/, XSkC/*=0.*/, xj,
           Mm, rIEPS, /*oDUL,*/ xjn/*=0.*/;

  if(!pmp->DUL )   // not possible to install upper kinetic constraint!
      return;

  for( k=0; k<pmp->FIs; k++ )
  { /* loop on phases */
     jb = je;
     je += pmp->L1[k];
     if( pmp->PHC[k] != PH_SORPTION )
          continue;

    if( pmp->XFA[k] < pmp->DSM ) // No sorbent retained by the IPM
        continue;
    if( pmp->XF[k]-pmp->XFA[k] < pmp->lowPosNum /* *10. */ )
        continue;  /* No surface species */

    for(i=0; i<6; i++)
        iSite[i] = -1;

    /* Extraction of site indices */
    for( j=jb; j<je; j++ )
    {
        ja = j - ( pmp->Ls - pmp->Lads );
        if( pmp->SATT[ja] != SAT_SOLV )
        {
            if( pmp->DCC[j] == DC_PEL_CARRIER || pmp->DCC[j] == DC_SUR_MINAL ||
                    pmp->DCC[j] == DC_SUR_CARRIER ) continue;
            ist = pmp->SATX[ja][XL_ST] / MSPN; // MSPN = 2 - number of EDL planes
            continue;
        }
        ist = pmp->SATX[ja][XL_ST] / MSPN;
        iSite[ist] = j;     // To be checked !!!
    }

    for( j=jb; j<je; j++ )
    { /* Loop for DC */
        if( pmp->X[j] <= pmp->lowPosNum /* *10. */ )
            continue;  /* This surface DC has been killed by IPM */
        rIEPS = pa.p.IEPS;
//        oDUL = pmp->DUL[j];
        ja = j - ( pmp->Ls - pmp->Lads );

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
            XSk = pmp->XFTS[k][ist]; /* Tot.moles of sorbates on surf.type */
            xj = pmp->X[j];  /* Current moles of this surf.species */
//            a=1.0;  Frumkin factor - reserved for extension to FFG isotherm
            switch( pmp->SATT[ja] )
            {
            case SAT_COMP: /* Competitive surface species on a surface type */
                /* a = fabs(pmp->MASDJ[j]); */
// rIEPS = pa.p.IEPS * 0.1;
                if( iSite[ist] < 0 )
                    xjn = 0.0;
                else xjn = pmp->X[iSite[ist]]; // neutral site does not compete!
                XS0 = max(pmp->MASDT[k][ist], pmp->MASDJ[ja][PI_DEN]);
                XS0 = XS0 * XVk * Mm / 1e6 * pmp->Nfsp[k][ist]; /* expected total in moles */
// Experimental  rIEPS = pa.p.IEPS * XS0;
                XSkC = XSk - xjn - xj; /* occupied by the competing species */
                if( XSkC < 0.0 )
                    XSkC = rIEPS; // 0.0;   ??????????
//                if( XSkC > XS0 )
//                    XSkC = XS0 - 2.0 * rIEPS;
                xj0 = XS0 - XSkC;    /* expected moles of this sorbate */
                if( xj0 > pmp->lnSAC[ja][3] )
                    xj0 = pmp->lnSAC[ja][3];
                if( xj0 < rIEPS )
                   xj0 = rIEPS;  /* ensuring that it will not zero off */
                pmp->DUL[j] = xj0; // XS0*(1.0-pa.p.IEPS);  //pa.p.IEPS;
// pmp->DUL[j] = XS0 - rIEPS;
                // Compare with old DUL from previous iteration!
/*                if( pmp->W1 != 1 && pmp->IT > 0 && fabs( (pmp->DUL[j] - oDUL)/pmp->DUL[j] ) > 0.1 )
                {
cout << "XmaxSAT_IPM2 Comp. IT= " << pmp->IT << " j= " << j << " oDUL=" << oDUL << " DUL=" << pmp->DUL[j] << endl;
                }
*/                break;
            case SAT_L_COMP:
            case SAT_QCA_NCOMP:
            case SAT_QCA1_NCOMP:
            case SAT_QCA2_NCOMP:
            case SAT_QCA3_NCOMP:
            case SAT_QCA4_NCOMP:
            case SAT_BET_NCOMP:
            case SAT_FRUM_COMP:
            case SAT_FRUM_NCOMP:
            case SAT_PIVO_NCOMP:
            case SAT_VIR_NCOMP:
            case SAT_NCOMP: /* Non-competitive surface species */
// rIEPS = pa.p.IEPS * 2;
//                xj0 = fabs(pmp->MASDJ[j]) * XVk * Mm / 1e6
//                      * pmp->Nfsp[k][ist];  in moles
                 xj0 = fabs( pmp->MASDJ[ja][PI_DEN] ) * XVk * Mm / 1e6
                      * pmp->Nfsp[k][ist]; /* in moles */
                 pmp->DUL[j] = xj0 - rIEPS;
// Experimental  rIEPS = pa.p.IEPS * xj0;
//                 if( xj0 > 2.0*rIEPS )
//                   pmp->DUL[j] = xj0 - rIEPS; // xj0*(1.0-pa.p.IEPS); //pa.p.IEPS;
//                 else pmp->DUL[j] = rIEPS;
// Compare with old DUL from previous iteration!
/*                if( pmp->W1 != 1 && pmp->IT > 0 && fabs( (pmp->DUL[j] - oDUL)/pmp->DUL[j] ) > 0.1 )
                {
cout << "XmaxSAT_IPM2 Ncomp IT= " << pmp->IT << " j= " << j << " oDUL=" << oDUL << " DUL=" << pmp->DUL[j] << endl;
                }
*/                break;

            case SAT_SOLV:  /* Neutral surface site (e.g. >O0.5H@ group) */
rIEPS = pa.p.IEPS;
                XS0 = max( pmp->MASDT[k][ist], pmp->MASDJ[ja][PI_DEN] );
                XS0 = XS0 * XVk * Mm / 1e6 * pmp->Nfsp[k][ist]; /* in moles */

// Experimental   rIEPS = pa.p.IEPS * XS0;
                pmp->DUL[j] =  XS0 - rIEPS; // xj0*(1.0-pa.p.IEPS);  //pa.p.IEPS;
                if( pmp->DUL[j] <= rIEPS )
                   pmp->DUL[j] = rIEPS;
                break;
// New methods added by KD 13.04.04
            case SAT_INDEF: /* No SAT calculation */
            default:        /* pmp->lnGam[j] = 0.0; */
                break;
            }
        }
     }  /* j */
  } /* k */
}

// clearing pmp->DUL constraints!
void TProfil::XmaxSAT_IPM2_reset( void )
{
    int j, ja/*=0*/, k, jb, je=0;

  if(!pmp->DUL )   // no upper kinetic constraints!
      return;

  for( k=0; k<pmp->FIs; k++ )
  { /* loop on phases */
     jb = je;
     je += pmp->L1[k];
     if( pmp->PHC[k] != PH_SORPTION )
          continue;

    for( j=jb; j<je; j++ )
    { /* Loop for DC */
      ja = j - ( pmp->Ls - pmp->Lads );
      if( pmp->lnSAC && ja >= 0 && ja < pmp->Lads )
          pmp->DUL[j] = pmp->lnSAC[ja][3];  // temp. storing initial DUL constr.
    }  /* j */
  } /* k */
}
//----------------------------------------------------------------------------


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// calc value of dual chemical potencial
double TProfil::DualChemPot( double U[], float AL[], int N )
{
    double Nu = 0.0;
    for(int i=0; i<N; i++ )
        Nu += AL[i]? U[i]*AL[i]: 0.0;
    return Nu;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* �� �娮� 䠧�- �⢮�             17.03.96
*  N - - ᨬ�, M - ᨬ� ⮢.
*  X - � ��- ᨬ� ⮢.
*  BF -  �⠢ 䠧�
*/
void TProfil::phase_bcs( int N, int M, float *A, double X[], double BF[] )
{
    int i, j;
    double Xx;

    if( !A || !X || !BF )
        return;
    memset( BF, 0, N*sizeof( double ));
    for( j=0; j<M; j++ )
    {
        Xx = X[j];
        if( fabs( Xx ) < 1e-12 )
            continue;
        for( i=0; i<N; i++ )
            BF[i] += A[i+j*N] * Xx;
    }
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
//  This procedure sets kinetic limits according to a given
//  concentration units
//  Needs much more work and elaboration!
//
int TProfil::Set_DC_limits( int Mode )
{
    double XFL, XFU, XFS=0., XFM, MWXW, MXV, XL, XU;
    int jb, je, j,k,JJ,KK, MpL, iRet=0;
    vstr tbuf(80);

    if( !pmp->PLIM )
        return iRet;  /* no limits */
// ???????????????????????????????????????
    ConCalc( pmp->X, pmp->XF, pmp->XFA );

    for(k=0; k<pmp->FI; k++)
        XFS+=pmp->XF[k];  /* calc sum of mol all phases */

    jb=0;
    for( k=0; k<pmp->FI; k++ )
    { /* cycle by phases */
        je=jb+pmp->L1[k];
        /*XFM=0.;*/
        MWXW =0.;
        MXV = 0.;
        XFL = 0.;
        XFU = 1e6;
        if( Mode && pmp->XF[k] < pmp->DSM )
            goto NEXT_PHASE;
        XFM = pmp->FWGT[k]; /* Mass of a phase */
        if( Mode )
        {
            MWXW = XFM/pmp->XF[k]; /* current mol weight of phase */
            MXV = pmp->FVOL[k]/pmp->XF[k]; /*current mol volume of phase  */
        }
        /* Check codes for phase DC */
        MpL=0;
        for( j=jb; j<je; j++ )
            if( pmp->RLC[j] != NO_LIM )
                MpL = 1;
        if( pmp->RFLC[k] == NO_LIM && !MpL )
        { /* check type restrictions on phase */
            goto NEXT_PHASE;
        }
        switch( pmp->RFSC[k] )
        { /* check scale restrictions on phase in all system */
        case QUAN_MOL:
            XFL = Mode? pmp->XF[k]: pmp->PLL[k];
            XFU = Mode? pmp->XF[k]: pmp->PUL[k];
            break;
        case CON_MOLAL:
            XFL = Mode? pmp->XF[k] /* *pmp->X[pmp->LO]/55.508373 */:
                  pmp->PLL[k]*pmp->GWAT/55.508373;
            XFU = Mode? pmp->XF[k] /* *pmp->X[pmp->LO]/55.508373 */:
                  pmp->PUL[k]*pmp->GWAT/55.508373;
            break;
        case CON_MOLFR:
            XFL = Mode? pmp->XF[k]: pmp->PLL[k]*XFS;
            XFU = Mode? pmp->XF[k]: pmp->PUL[k]*XFS;
            break;
        case CON_WTFR:   if(MWXW < 1e-15) break;  // Temp.fix
            XFL = Mode? pmp->XF[k]: pmp->PLL[k]*pmp->MBX/MWXW;
            XFU = Mode? pmp->XF[k]: pmp->PUL[k]*pmp->MBX/MWXW;
            break;
        case CON_VOLFR:   if(MXV < 1e-15) break; // Temp.fix
            XFL = Mode? pmp->XF[k]: pmp->PLL[k]*pmp->VXc/MXV;
            XFU = Mode? pmp->XF[k]: pmp->PUL[k]*pmp->VXc/MXV;
            break;
        default:
            ; /* do more? */
        }
//        if( pmp->RFLC[k] == NO_LIM )
//        {                            Temporary!
            XFL = 0.0;
            XFU = 1e6;
//        }
        for( j=jb; j<je; j++ )
        { /* cycle by DC. */
            if( pmp->RLC[j] == NO_LIM )
                continue;

            if( Mode )
            {
                XU = pmp->DUL[j];
                XL = pmp->DLL[j];
            }
            else
                switch( pmp->RSC[j] ) /* get initial limits */
                {
                case QUAN_MOL:
                    XU = pmp->DUL[j];
                    XL = pmp->DLL[j];
                    break;
                case CON_MOLAL:
                    XU = pmp->DUL[j]*pmp->GWAT/55.508373;
                    XL = pmp->DLL[j]*pmp->GWAT/55.508373;
                    break;
                case CON_MOLFR:
                    XU = pmp->DUL[j]*XFU;
                    XL = pmp->DLL[j]*XFL;
                    break;
                case CON_WTFR:
//Ask Dima!!! 20/04/2002
#ifndef IPMGEMPLUGIN
                    XU = pmp->DUL[j]*XFU*MWXW /
                         syst->MolWeight(pmp->N, pmp->Awt, pmp->A+j*pmp->N );
                    XL = pmp->DLL[j]*XFL*MWXW /
                         syst->MolWeight(pmp->N, pmp->Awt, pmp->A+j*pmp->N );

#endif
                    break;
                case CON_VOLFR:
                    XU = pmp->DUL[j]*XFU*MXV/ pmp->Vol[j];
                    XL = pmp->DLL[j]*XFL*MXV/ pmp->Vol[j];
                    break;
                default:
                    ; /* do more */
                }
            /* check combine */
            if( XU < 0.0 ) XU = 0.0;
            if( XU > 1e6 ) XU = 1e6;
            if( XL < 0.0 ) XL = 0.0;
            if( XL > 1e6 ) XL = 1e6;
            if( XU > XFU )
            {
                iRet = 1;
                JJ = j;
                KK = k;
                sprintf( tbuf, "Inconsistent upper limits j=%d k=%d XU=%g XFU=%g",
                         JJ, KK, XU, XFU );
#ifndef IPMGEMPLUGIN
                vfMessage(window(), "Set_DC_limits",tbuf.p );
#else
                cout << tbuf.p << endl;
#endif

                XU = XFU; // - pmp->lowPosNum;
            }
            if( XL < XFL )
            {
                iRet = 1;
                JJ = j;
                KK = k;
                sprintf( tbuf, "Inconsistent lower limits j=%d k=%d XL=%g XFL=%g",
                         JJ, KK, XL, XFL );
#ifndef IPMGEMPLUGIN
                vfMessage(window(), "Set_DC_limits",tbuf.p );
#else
                cout << tbuf.p << endl;
#endif
                XL = XFL; // - pmp->lowPosNum;
            }
            pmp->DUL[j]=XU;
            pmp->DLL[j]=XL;
        }   /* j */
NEXT_PHASE:
        jb = je;

    }  /* k */
    return iRet;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Calc sum count of phases
void TProfil::TotalPhases( double X[], double XF[], double XFA[] )
{
    int jj, j, i, k;
    double XFw, XFs, x;

    j=0;
    for( k=0; k< pmp->FI; k++ )
    { /* cycle by phases */
        i=j+pmp->L1[k];
        XFw = 0.0;
        XFs=0.0; /* calc  count mol of carrier */
        for(jj=j; jj<i; jj++)
        {
            x = X[jj];
            if( pmp->DCCW[jj] == DC_ASYM_CARRIER && pmp->FIs )
                XFw += x;
            else XFs += x;
        }
        XF[k] = XFw + XFs;
        if( k < pmp->FIs )
            XFA[k] = XFw;
        j=i;
    }
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Calculate mass-balance deviations (moles)
// void TProfil::eDmb( int N, int L, float *A, double *Y, double *B, double *C )
// {
//  short I,J;
//  for(J=0;J<N;J++)
//  {
//    C[J]=B[J];
//    for(I=0;I<L;I++)
//       if( Y[I] && *(A+I*N+J) )
//           C[J]-= *(A+I*N+J) *Y[I];
//  }
// }


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* Calculation of prime chemical potential F (returned)
* from moles of DC Y[], total moles of phase YF[] and standard 
* Gibbs energy gT (obtained from pmp->G[])
* On error returns +7777777.
*/
double TProfil::PrimeChemPot(
    double G,      /* gT0+gEx */
    double logY,   /* ln x */
    double logYF,  /* ln Xa */
    double asTail, /* asymmetry non-log term or 0 for symmetric phases */
    double logYw,  /* ln Xv */
    char DCCW      /* generalized species class code */
)
{
    double F;
    switch( DCCW )
    {
    case DC_SINGLE:
        F = G;
        break;
    case DC_ASYM_SPECIES:
        F = G + logY - logYw + asTail;
        break;
    case DC_ASYM_CARRIER:
        F = G + logY - logYF + asTail + 1.0 -
            1.0/(1.0 - asTail);
        break;
    case DC_SYMMETRIC:
        F = G + logY - logYF;
        break;
    default:
        F = 7777777.;
    }
    return F;
}


// Kernel functions of IPM - rewritten by DAK for adsorption

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// VJ - Update of prime chemical potentials
void TProfil::PrimeChemicalPotentials( double F[], double Y[], double YF[], double YFA[] )
{
    int i,j,k;
    double v, Yf; // v is debug variable

    j=0;
    for( k=0; k<pmp->FI; k++ )
    { /* cycle by phase */
        i=j+pmp->L1[k];
        if( YF[k] <= pmp->lowPosNum*100. ||
                pmp->PHC[k] == PH_AQUEL && Y[pmp->LO] <= pa.p.XwMin )
            goto NEXT_PHASE;

        pmp->YFk = 0.0;
        Yf= YF[k]; /* calculate number of moles of carrier */
        if( pmp->FIs && k<pmp->FIs )
            pmp->YFk = YFA[k];
        if( Yf >= 1e6 )
        {                 // error - will result in zerodivide!
           char pbuf[80];
           sprintf( pbuf, "%20s", pmp->SF[k] ); pbuf[20] = 0;
//cout << "Error in IPM PrimeChemicalPotentials(): IT = " << pmp->IT << endl;
//cout << "  Phase " << pbuf << " k= " << k << " Yf= " << Yf;
//cout << " YFa= " << pmp->YFk << " Yf fixed to " << pmp->YFk << endl;
//           for( ; j<i; j++ )
//           {
//             if( Y[j] > pmp->YFk )
//             {
//               sprintf( pbuf, "%16s", pmp->SM[j] ); pbuf[MAXDCNAME] = 0;
// cout << "    j= " << j << " " << pbuf << "  Y= " << Y[j] << endl;
//             }
//           }
           Yf = pmp->YFk;
        }
        if( pmp->YFk > pmp->lowPosNum*10. )
        {
            pmp->logXw = log(pmp->YFk);
            pmp->aqsTail = 1.- pmp->YFk / Yf;
        }
        if( pmp->L1[k] > 1 )
            pmp->logYFk = log( Yf );
        if( pmp->PHC[k] == PH_AQUEL)
            /* ln moles of solvent in aqueous phase */
            pmp->Yw = pmp->YFk;
        for( ; j<i; j++ )
        { /*cycle by DC. */
            if( Y[j] < pmp->lowPosNum )
                continue;  /* exception by minimum DC quantity */
            /* calculate chemical potential of j-th DC */
            v = PrimeChemPot( pmp->G[j], log(Y[j]), pmp->logYFk,
                              pmp->aqsTail, pmp->logXw, pmp->DCCW[j] );
            F[j] = v;
        }   /* j */
NEXT_PHASE:
        j = i;
    }  /* k */
    if( pmp->Yw > pa.p.XwMin )
        pmp->logXw = log(pmp->Yw);
}

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* Calculation of Karpov stability criteria for a DC*/
double TProfil::KarpovCriterionDC(
    double *dNuG,  /* Nu[j]-c[j] difference - is modified here */
    double logYF,  /* ln Xa */
    double asTail, /* asymmetry correction (0 for symmetric phases) */
    double logYw,  /* ln Xv */
    double Wx,     /* mole fraction */
    char DCCW      /* generalized DC class code */
)
{
    double Fj;     /* output phase stability criterion */

    if( logYF > -35. && Wx > 1e-18 )
        switch( DCCW ) /* expressions for fj */
        {
        default: /* error code !!! */
            *dNuG = 36.;
        case DC_SINGLE:
            Wx = 1.0;
        case DC_SYMMETRIC:
            break;
        case DC_ASYM_SPECIES:
            *dNuG += logYw - logYF - asTail;
            break;
        case DC_ASYM_CARRIER:
            *dNuG += 1.0/(1.0 - asTail) - asTail - 1.0;
        }
    if( fabs( *dNuG ) > 35.)
        Fj = ( *dNuG > 0 )? 1.5860135e15: 6.305117e-16;
    else Fj = exp( *dNuG );
    Fj -= Wx;
    return Fj;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// calculation of Karpov stability criteria for all phases
void TProfil::f_alpha()
{
    short k;
    int j, ii;
    double *EMU,*NMU, YF, Nu, dNuG, Wx, Yj, Fj;

    EMU = pmp->EMU;
    NMU = pmp->NMU;
    memset( EMU, 0, pmp->L*sizeof(double));
    memset( NMU, 0, pmp->L*sizeof(double));

    j=0;
    pmp->YMET = 0.0;
    for( k=0; k<pmp->FI; k++ )
    { /*phases */
        ii=j+pmp->L1[k];
        pmp->Falp[k] = pmp->YMET; /* metastability parameter */
        pmp->logXw = -35.;
        pmp->logYFk = -35.;

        pmp->YFk = 0.0;
        YF= pmp->YF[k]; /* moles of carrier */
        if( pmp->FIs && k<pmp->FIs )
            pmp->YFk = pmp->YFA[k];
        if( pmp->YFk > 6.305117e-16 )
        {
            pmp->logXw = log(pmp->YFk);
            pmp->aqsTail = 1.- pmp->YFk / YF;
        }
        else
        {
            pmp->logXw = -35.;
            pmp->aqsTail = 0.0;
        }

        if( pmp->L1[k] > 1 && YF > 6.305117e-16 )
            pmp->logYFk = log( YF );
        else pmp->logYFk = -35.;
        if( pmp->PHC[k] == PH_AQUEL) /* number of moles of solvent */
            pmp->Yw = pmp->YFk;

        for( ; j<ii; j++ )
        { /* DC */
            Nu = DualChemPot( pmp->U, pmp->A+j*pmp->N, pmp->NR );
            dNuG = Nu - pmp->G[j]; /* this is -s_j (6pot paper 1) */
            Wx = 0.0;
            Yj = pmp->Y[j];
            if( YF > 1e-12 && Yj > pmp->lowPosNum*10. )
                Wx = Yj / YF; /*calc mol fraction of DC */
            /* calc Karpov criteria of DC */
            Fj = KarpovCriterionDC( &dNuG, pmp->logYFk, pmp->aqsTail,
                                    pmp->logXw, Wx, pmp->DCCW[j] );
            NMU[j] = dNuG;
            EMU[j] = Fj;
            if( YF > 1e-12 && Yj > pmp->lowPosNum*10. )
                pmp->Falp[k] += EMU[j]; /* calc Karpov criteria of phase */
        }   /* j */
        j = ii;
    }  /* k */
}

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* Calculation of a species increment to total Gibbs energy G(X)
*  of the system (returned). On error returns +7777777.
*/
double TProfil::FreeEnergyIncr(
    double G,      /* gT0+gEx */
    double x,      /* x - moles of species */
    double logXF,  /* ln Xa - moles of phase */
    double logXw,  /* ln Xv - moles of the solvent/sorbent */
    char DCCW      /* generalized species class code */
)
{
    double Gi;

    switch( DCCW )
    {
    case DC_ASYM_SPECIES:
        Gi = x * ( G + log(x) - logXw );
        break;
    case DC_ASYM_CARRIER:
    case DC_SYMMETRIC:
        Gi = x * ( G + log(x) - logXF );
        break;
    case DC_SINGLE:
        Gi = G * x;
        break;
    default:
        Gi = 7777777.;
    }
    return Gi;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* Calculation of the total Gibbs energy of the system G(X).
*  Parameter LM is IPM step size for calculation of new 
*  quantities of all species (vector X[]) using direction 
*  of descent (MU[] vector). If LM==0, this function 
*  just copies vector Y[] into X[].
*  Returns value of G(X) in moles.
*/
double TProfil::GX( double LM  )
{
    int i, j, k;
    double x, XF, XFw, FX, Gi /* debug variable */;

    if( LM<pmp->lowPosNum)     /* copy vector Y into X */
        for(i=0;i<pmp->L;i++)
            pmp->X[i]=pmp->Y[i];
    else  /*calculate new values of X */
        for(i=0;i<pmp->L;i++)
        {  /* vector pmp->MU - the direction of descent! */
            pmp->X[i]=pmp->Y[i]+LM*pmp->MU[i];
            if( pmp->X[i] <  pmp->lowPosNum )
                pmp->X[i]=0.;
        }
    /*calculate new total quantities of phases */
    TotalPhases( pmp->X, pmp->XF, pmp->XFA );

    /* calculate G(X) */
    FX=0.;
    j=0;
    for( k=0; k<pmp->FI; k++ )
    { /* loop for phases */
        i=j+pmp->L1[k];
        XFw = 0.0;  /* calc moles of solvent/sorbent */
        if( pmp->FIs && k<pmp->FIs )
            XFw = pmp->XFA[k];
        if( XFw > pmp->lowPosNum*10. )
            pmp->logXw = log( XFw );
        /*   */
        XF = pmp->XF[k];
        if( XF <= pmp->lowPosNum*1000. ||
                (pmp->PHC[k] == PH_AQUEL && XFw <= pa.p.XwMin )
                || ( pmp->PHC[k] == PH_SORPTION && XFw <= pa.p.ScMin ))
            goto NEXT_PHASE;
        pmp->logYFk = log( XF );

        for( ; j<i; j++ )
        { /* Species */
            x = pmp->X[j];
            if( x < pmp->lowPosNum*10. )
                continue;
            /* calc increment of G(x) */
            Gi = FreeEnergyIncr( pmp->G[j], x, pmp->logYFk, pmp->logXw,
                                 pmp->DCCW[j] );
            FX += Gi;
        }   /* j */
NEXT_PHASE:
        j = i;
    }  /* k */
    return(FX);
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// GX for ProbeUncert in statistic
double TProfil::pb_GX( double *Gxx  )
{
    int i, j;
    short k;
    double Gi, x, XF, XFw, FX;

    /* calc G(X) */
    FX=0.;
    j=0;
    for( k=0; k<pmp->FI; k++ )
    { /*phases*/
        i=j+pmp->L1[k];
        XFw = 0.0;  /* calc number of moles of carrier*/
        if( pmp->FIs && k<pmp->FIs )
            XFw = pmp->XFA[k];
        if( XFw > pmp->lowPosNum*10. )
            pmp->logXw = log( XFw );
        /* calc new quantitys of phase  */
        XF = pmp->XF[k];
        if( XF <= pmp->lowPosNum*1000. ||
                (pmp->PHC[k] == PH_AQUEL && XFw <= pa.p.XwMin )
                || ( pmp->PHC[k] == PH_SORPTION && XFw <= pa.p.ScMin ))
            goto NEXT_PHASE;
        pmp->logYFk = log( XF );

        for( ; j<i; j++ )
        { /*DC */
            x = pmp->X[j];
            if( x < pmp->lowPosNum*10. )
                continue;
            /* calc increment of G(x) */
            Gi = FreeEnergyIncr( Gxx[j], x, pmp->logYFk, pmp->logXw,
                                 pmp->DCCW[j] );
            FX += Gi;
        }   /* j */
NEXT_PHASE:
        j = i;
    }  /* k */
    return(FX);
}

// Calc value cj (G0) by type of DC and standart value g(T,P)
// k - index of phase, j - index DC in phase
// if error code return 777777777.
double TProfil::Cj_init_calc( double g0, int j, int k )
{
    double G, YOF;

    G = g0/pmp->RT;
    /*  if( k < pmp->FIs )  */
    YOF = pmp->YOF[k];
    /* ��� ��� ����!!!!! */
    switch( pmp->DCC[j] )
    { /* Aqueous electrolyte */
    case DC_AQ_PROTON:
    case DC_AQ_ELECTRON:
    case DC_AQ_SPECIES:
        G += pmp->ln5551;
        /* calc mol weight of solvent !!!!!!!!!!!!!!!!!!!!!!!!!!! */
    case DC_AQ_SOLVCOM:
    case DC_AQ_SOLVENT:
#ifndef IPMGEMPLUGIN
        if( syp->PYOF != S_OFF )
            pmp->GEX[j] += YOF;
#endif
        break;
        /* as phase- test add ln P general !!!!!!!!!!!!!!!!!!!!!! */
    case DC_GAS_COMP: /* gases exept H2O and CO2 */
    case DC_GAS_H2O: /* index to switch off? */
    case DC_GAS_CO2:
    case DC_GAS_H2:
    case DC_GAS_N2:
        if( pmp->Pparc[j] != 1.0 && pmp->Pparc[j] > 1e-30 )
            G += log( pmp->Pparc[j] );
        /* Solution non-electrolyte */
    case DC_SCP_CONDEN: /*a single-component phase */
    case DC_SOL_IDEAL:
    case DC_SOL_MINOR:
    case DC_SOL_MAJOR:
    case DC_SUR_MINAL:
    case DC_SUR_CARRIER:
    case DC_PEL_CARRIER:
#ifndef IPMGEMPLUGIN
        if( syp->PYOF != S_OFF )
            pmp->GEX[j] += YOF;
#endif
        break;
        /* Sorption phases */
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
        G += pmp->ln5551;
        break;
    default: /* error code */
        return 7777777.;
    }
    return G += pmp->GEX[j];
}

//--------------------- End of ms_muexp.cpp ---------------------------

