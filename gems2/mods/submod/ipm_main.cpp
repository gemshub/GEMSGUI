//-------------------------------------------------------------------
// $Id: ipm_sel_bc.cpp 705 2006-04-28 19:39:01Z gems $
//
// Copyright (C) 1992-2000 K.Chudnenko, I.Karpov, D.Kulik, S.Dmitrieva
//
// Implementation of parts of the Interior Points Method (IPM) module
// for convex programming Gibbs energy minimization, described in:
// (Karpov, Chudnenko, Kulik (1997): American Journal of Science
//  v.297 p. 767-806)
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

#ifndef IPMGEMPLUGIN

#include "service.h"
#include "stepwise.h"

#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Main sequence of IPM calculations
//  Main place for implementation of diagnostics and setup
//  of IPM precision and convergence
//  Returns true   if IPM result is OK
//          false  if good result could not be obtained
//
void TMulti::MultiCalcMain()
{
    int i, j, RepeatSel=0, eRet;
    SPP_SETTING *pa = &TProfil::pm->pa;

    pmp->W1=0;
    if( pmp->pULR && pmp->PLIM )
        Set_DC_limits( DC_LIM_INIT );

    /* test insert in valid area */
mEFD:
     if(pmp->PZ && pmp->W1)
     { for( i=0; i<pmp->L; i++ )
        pmp->Y[i]=pmp->X[i];
      TotalPhases( pmp->Y, pmp->YF, pmp->YFA );
     }

    eRet = EnterFeasibleDomain( );

#ifndef IPMGEMPLUGIN
#ifndef Use_mt_mode
    pVisor->Update(false);
#endif
// STEPWISE (2)  - stop point to examine output from EFD()
STEP_POINT("After FIA");
#endif

    switch( eRet )
    {
     case 0:  // OK
              break;
     case 2:  // max iteration exceeded in EnterFeasibleDomain
                 Error("E04IPM EFD(): " ,
     "Prescribed precision of mass balance could not be reached because vector b or\n"
     "DC stoichiometries or standard-state thermodynamic data are inconsistent.\n");
              break;
     case 1: // degeneration in R matrix  for EnterFeasibleDomain
           if( pmp->DHBM<1e-5 )
            {
               pmp->DHBM *= 10.;
               goto mEFD;
            }
           else
                 Error("E05IPM EFD(): " ,
          "Degeneration in R matrix (fault in the linearized system solver).\n"
          "Invalid initial approximation - further IPM calculations are not possible");
           break;
    }

   // call of main minimization IPM
   eRet = InteriorPointsMethod( );

#ifndef IPMGEMPLUGIN
#ifndef Use_mt_mode
    pVisor->Update(false);
#endif
// STEPWISE (3)  - stop point to examine output from IPM()
   STEP_POINT("After IPM");
#endif

// Diagnostics of IPM results
   switch( eRet )
   {
     case 0:  // OK
              break;
     case 2:  // max iteration exceeded in InteriorPointsMethod
               if( pmp->DX<1e-3 )
               {
                   pmp->DX *= 10.;
                   goto mEFD;
                }
               else
                 Error("E06IPM IPM-main(): " ,
     "Given IPM convergence criterion could not be reached;\n Perhaps, vector b is not balanced,\n"
     "or DC stoichiometries or standard-state thermodynamic data are inconsistent. \n");
     case 1: // degeneration in R matrix  for InteriorPointsMethod
           if( pmp->DHBM<1e-5 )
            {
               pmp->DHBM *= 10.;
               goto mEFD;
            }
           else
               Error("E07IPM IPM-main(): ",
        "Degeneration in R matrix (fault in the linearized system solver).\n"
        "No valid IPM solution could be obtained. Probably, vector b is not balanced,\n"
        "or DC stoichiometries or standard-state thermodynamic data are inconsistent,\n"
        "or some relevant phases or DC are missing, or some kinetic constraints are too stiff.\n"
        );
          break;
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

    if( !pa->p.PC )    //  No Selector2 subroutine operation
    {  if( pmp->PD >= 2 )
           memcpy( pmp->G, pmp->G0, pmp->L*sizeof(double));
        return;  // solved
    }

//========= call Selekt2() =======
   PhaseSelect();
   if( pa->p.PC == 2 )
       XmaxSAT_IPM2();  // Install upper limits to xj of surface species

   if( !pmp->MK )
     if( RepeatSel<3 )
       { RepeatSel++;
         goto mEFD;
       }
     else
       Error( "E08IPM PhaseSelect(): "," Insertion of phases was incomplete!");

  MassBalanceDeviations( pmp->N, pmp->L, pmp->A, pmp->X, pmp->B, pmp->C);

#ifndef IPMGEMPLUGIN
// STEPWISE (4) Stop point after PhaseSelect()
STEP_POINT("PhaseSelect");
#ifndef Use_mt_mode
pVisor->Update( false );
#endif
#endif

   if(pmp->PZ )
   {
     if( !pmp->W1 )
     {
       pmp->W1++;           // IPM-2 precision algorithm - 1st run
       goto mEFD;
     }
     else
       if( pmp->W1 <  pa->p.DW )
       {
          for(i=0;i<pmp->N-pmp->E;i++)
          {
            if( fabs(pmp->C[i]) > pmp->DHBM // * pa->p.GAS
            || fabs(pmp->C[i]) > pmp->B[i] * pa->p.GAS )
            {
               if(pmp->W1 < pa->p.DW-1)
               {
                 pmp->W1++;          // IPM-2 precision algorithm - 2nd run
                 goto mEFD;
               }
               else
               {
                 gstring  buf,buf1;
                 vstr pl(5);
                 int jj=0;
                 for( j=0; j<pmp->N-pmp->E; j++ )
                    //  if( fabs(pmp->C[j]) > pmp->DHBM  || fabs(pmp->C[j]) > pmp->B[j] * pa->p.GAS )
                  if( fabs(pmp->C[j]) > pmp->B[j] * pa->p.GAS )
                  { sprintf( pl, " %-2.2s  ", pmp->SB[j] );
                    buf1 +=pl;
                    jj=1;
                  }
                 if( jj )
                 {
                    buf = "Prescribed balance precision cannot be reached\n";
                    buf += "for some trace independent components: ";
                    buf += buf1;
                    Error("E09IPM IPM-main(): ", buf.c_str() );
                 }
                else
                 Error("E10IPM IPM-main(): " ,
                 "Inconsistent GEM solution: imprecise mass balance\n for some major independent components: " );
              }
            }
          } // end of i loop
      }
   }
    memcpy( pmp->G, pmp->G0, pmp->L*sizeof(double));
   // appears to be normal return after successfull improvement of mass balance precision
}

//Calc equstat method IPM (iterations)
void TMulti::MultiCalcIterations()
{
    MultiCalcMain();

    //calc demo data
    for( int ii=0; ii<pmp->N; ii++ )
        pmp->U_r[ii] = pmp->U[ii]*pmp->RT;
    GasParcP();

    /* calc finished */
}

// Finding automatic initial approximation for the IPM algorithm
// using modified simplex method with two-side constraints
// Return code:
// false - OK for IPM
// true  - OK solved
//
bool TMulti::AutoInitialApprox( )
{
    int i, j, k, NN;
    double minB, sfactor;
    SPP_SETTING *pa = &TProfil::pm->pa;

// Kostya correct DK & DHB as automatic
    NN = pmp->N - pmp->E;
    minB=pmp->B[0];
    for(i=0;i<NN;i++)
      if( pmp->B[i] < minB )
          minB = pmp->B[i];
    if( minB < pa->p.DB )  // KD - foolproof
       minB = pa->p.DB;

//  check Ymin (cutoff)
   if(pmp->lowPosNum>minB*0.01)
      pmp->lowPosNum=minB*0.01;

   sfactor = calcSfactor();
   pmp->DHBM = sfactor * pa->p.DHB;  // 2.5 root
   pmp->DHBM *= (0.097+0.95/(1+exp(-(log10(minB)+11)/0.4)));

   pmp->DX = pa->p.DK;
   pmp->DX *= sfactor;
   pmp->DX *= (0.097+0.95/(1+exp(-(log10(minB)+6.1)/0.54)));
   if( pmp->DX < 0.01 * pa->p.DK )
       pmp->DX = 0.01 * pa->p.DK;
   pmp->DSM = pa->p.DS;  // Shall we add  * sfactor ?

#ifndef IPMGEMPLUGIN
#ifndef Use_mt_mode
   pVisor->Update(false);
#endif
#endif
    // Analyzing if Simplex approximation is needed
    if( !pmp->pNP  )
    {   // Preparing to call Simplex method
        pmp->FitVar[4] = pa->p.AG;  // smoothing parameter init
        pmp->pRR1 = 0;
        TotalPhases( pmp->X, pmp->XF, pmp->XFA );
//      pmp->IC = 0.0;  Important - for reproducibility of simplex FIA
        if( pa->p.PSM && pmp->FIs )
            GammaCalc(LINK_FIA_MODE);
        if( pa->p.PC == 2 )
           XmaxSAT_IPM2_reset();  // Reset upper limits for surface species
        pmp->IT = 0;
        pmp->pNP = 0;
        pmp->K2 = 0;
        pmp->PCI = 0;

  // simplex method is called here
        SimplexInitialApproximation( );

//  STEPWISE (0) - stop point for examining results from simplex IA
#ifndef IPMGEMPLUGIN
STEP_POINT( "End Simplex" );
#endif

        // no multi-component phases?
        if( !pmp->FIs )
            return true; // goto OVER; // solved !

      // Trace DC quantity into zeros
      TraceDCzeros( 0, pmp->L, sfactor );
    }
    else  // Taking previous result as initial approximation
    {
        int jb, je=0, jpb, jpe=0, jdb, jde=0;
        double LnGam, FitVar3;
        /*    pmp->IT *= pa->p.PLLG; */

        FitVar3 = pmp->FitVar[3];
        pmp->FitVar[3] = 1.0;
        TotalPhases( pmp->Y, pmp->YF, pmp->YFA );
        for( j=0; j< pmp->L; j++ )
            pmp->X[j] = pmp->Y[j];
        TotalPhases( pmp->X, pmp->XF, pmp->XFA );
        ConCalc( pmp->X, pmp->XF, pmp->XFA );
        if( pmp->E && pmp->LO )
            IS_EtaCalc();
        /*    if( pa->p.PSM )  corr. 13.4.96 */
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
           // Trace DC quantity into zeros
           TraceDCzeros( 0, pmp->L, 1/1000. );
        }
    }  // else
    pmp->MK = 1;
// STEPWISE (1) - stop point to see IA from old solution or raised simplex
#ifndef IPMGEMPLUGIN
STEP_POINT("Before FIA");
#endif
    return false;
}

// ------------------- ------------------ ----------------
// Calculation of feasible IPM initial approximation point
//
// Algorithm: see Karpov, Chudnenko, Kulik 1997 Amer.J.Sci. vol 297 p. 798-799
// (Appendix B)
//
// Returns: 0 - OK,
//          1 - no convergence at specified precision DHB
//          2  - used more than pa.p.DP iterations
//
int TMulti::EnterFeasibleDomain()
{
    short IT1;
    int I, J, Z,  N, sRet, iRet=0;
    double DHB, LM;
    SPP_SETTING *pa = &TProfil::pm->pa;

    ErrorIf( !pmp->MU || !pmp->W, "EnterFeasibleDomain()",
                              "Error alloc pmp->MU or pmp->W." );
    DHB = pmp->DHBM;  // Convergence (balance precision) criterion
    pmp->Ec=0;  // Return code

    // calculation of total moles of phases
    TotalPhases( pmp->Y, pmp->YF, pmp->YFA );

    if( pmp->PLIM )
        Set_DC_limits(  DC_LIM_INIT );

    // Adjustment of prime approximation according to kinetic constraints
    LagrangeMultiplier();

//----------------------------------------------------------------------------
// BEGIN:  /* main cycle */
    for( IT1=0; IT1 < pa->p.DP; IT1++, pmp->IT++ )
    {
        // get size of task
        pmp->NR=pmp->N;
        if( pmp->LO )
        {   if( pmp->YF[0] < pmp->DSM && pmp->YFA[0] < pmp->lowPosNum*100.)
                pmp->NR= (short)(pmp->N-1);
        }
        N=pmp->NR;

       // Calculation of mass-balance deviations in IPM
       MassBalanceDeviations( pmp->N, pmp->L, pmp->A, pmp->Y, pmp->B, pmp->C);

      // test solution
       Z = pmp->N - pmp->E;
       for(I=0;I<Z;I++)
         if( fabs(pmp->C[I]) > DHB  ||
             ( pmp->W1 && fabs(pmp->C[I]) > pmp->B[I] * pa->p.GAS ) )
           break;
       if( I == Z ) // OK
       { pmp->IT -= IT1;
         return iRet;       // OK
       }

       WeightMultipliers( true );
       // Assembling and Solving system of linear equations
       sRet = SolverLinearEquations( N, true );
       if( sRet == 1 )  // error no solution
          break;

// SOLVED: solution of linear matrix obtained
      pmp->PCI = calcDikin( N, true);  // calc of MU values and Dikin criterion

      LM = calcLM( true ); // Calculation of descent step size LM
      if( LM < 1E-5 )
         Error( "E03IPM FIA-iteration:",
            "Too small step size - too slow convergence");
      if( LM > 1.)
         LM = 1.;

        // calculation of new prime solution approximation
      for(J=0;J<pmp->L;J++)
            pmp->Y[J] += LM * pmp->MU[J];

#ifndef IPMGEMPLUGIN
#ifndef Use_mt_mode
  pVisor->Update( false );
#endif
// STEPWISE (5) Stop point at end of iteration of FIA()
STEP_POINT("FIA Iteration");
#endif
}  /* End loop on IT1 */
//----------------------------------------------------------------------------
    //  Prescribed mass balance precision cannot be reached
    //  Take a look at vector b or values of DHB and DS

   pmp->Ec=1;
   if( IT1 == pa->p.DP )
      iRet = 2;
   else
      iRet = 1;
   return iRet;   // no solution
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Calculation of chemical equilibrium using Interior Points
//  Method algorithm (see Karpov et al., 1997, p. 785-786)
//  GEM IPM
// Returns: 0, if converged;
//          1, in case of degeneration
//          2, (more than max iteration)  divergence or
//             user interruption
//
int TMulti::InteriorPointsMethod( )
{
    int N, IT1,J,Z,iRet;
    double LM=0., LM1, FX1;
    SPP_SETTING *pa = &TProfil::pm->pa;

    if( pmp->FIs )
      for( J=0; J<pmp->Ls; J++ )
            pmp->lnGmo[J] = pmp->lnGam[J];

    for(J=0;J<pmp->N;J++)
    {   pmp->U[J]=0.;
        pmp->C[J]=0.;
    }

    pmp->Ec=0;
    pmp->FX=GX( LM  );  // calculation of G(x)

    if( pmp->FIs ) // multicomponent phases present
      for(Z=0; Z<pmp->FIs; Z++)
        pmp->YFA[Z]=pmp->XFA[Z];

//----------------------------------------------------------------------------
//  main cycle of IPM iterations
    for( IT1 = 0; IT1 < pa->p.IIM; IT1++, pmp->IT++ )
    {
      // get size of task
        pmp->NR=pmp->N;
        if( pmp->LO ) // water-solvent is present
        {
            if( pmp->YF[0]<pmp->DSM && pmp->Y[pmp->LO]< pmp->lowPosNum *100.)
                pmp->NR=(short)(pmp->N-1);
        }
        N = pmp->NR;
        memset( pmp->F, 0, pmp->L*sizeof(double));

        PrimeChemicalPotentials( pmp->F, pmp->Y, pmp->YF, pmp->YFA );

        // Set weight multipliers for DC
        WeightMultipliers( false );
        // making and solve  matrix of IPM linear equations
        iRet = SolverLinearEquations( N, false );
        if( iRet == 1 )
        { pmp->Ec=1;
          return 1;
        }

//SOLVED:  // got U vector - calculating criteria Karpov and Dikin
       f_alpha( );
       pmp->PCI=calcDikin( N, false );  // calc of MU values and Dikin criterion;

       // Determination of descent step size  LM
       LM = calcLM( false );

       LM1=LMD( LM ); // Finding optimal value of descent step
       FX1=GX( LM1 ); // New G(X) after the descent step
       pmp->PCI = sqrt(pmp->PCI); // Dikin criterion

       if( (IT1 > 3) && (FX1 - pmp->FX > 10)  &&
          ( IT1 > pa->p.IIM/2 || pmp->PD==3 ) )  // ????????
        {
            if( pmp->LO && pmp->X[pmp->LO]>pmp->lowPosNum && pmp->PD==3 )
                pmp->PD=2;     //  what does this mean?
            else  pmp->DX= 0.5 * pmp->PCI;
        }

       MassBalanceDeviations( pmp->N, pmp->L, pmp->A, pmp->Y, pmp->B, pmp->C );

       pmp->FX=FX1;
       // main iteration done 
       // Calculation of activity coefficients on IPM iteration
        if( pmp->PD==3 )
            GammaCalc( LINK_UX_MODE );

        if( pmp->PHC[0] == PH_AQUEL && pmp->XF[0] <= pa->p.XwMin &&
             pmp->X[pmp->LO] <= pmp->lowPosNum*1e3 )    // bugfix 29.11.05 KD
        {
            pmp->XF[0]=0.;  // elimination of aqueous phase
            pmp->XFA[0] = 0.;
        }
        // Restoring vectors Y and YF
        Restoring_Y_YF();

#ifndef IPMGEMPLUGIN
#ifndef Use_mt_mode
  pVisor->Update( false );
#endif
// STEPWISE (6)  Stop point at IPM() main iteration
STEP_POINT( "IPM Iteration" );
#endif
        if( pmp->PCI < pmp->DX )  // Dikin criterion satisfied
            break;
    } // end of main IPM cycle
//----------------------------------------------------------------------------

  if( IT1 >= pa->p.IIM)
   return 2; // IT>500

 // Final calculation of activity coefficients
  TotalPhases( pmp->X, pmp->XF, pmp->XFA );

  if(pmp->PZ && pmp->W1)
      Mol_u( pmp->Y, pmp->X, pmp->XF, pmp->XFA );

  if( pmp->PD==1 || pmp->PD == 2  /*|| pmp->PD == 3*/  )
        GammaCalc( LINK_UX_MODE);
   else
        ConCalc( pmp->X, pmp->XF, pmp->XFA );

   MassBalanceDeviations( pmp->N, pmp->L, pmp->A, pmp->X, pmp->B, pmp->C);
   return 0;
}

/* Calculation of Simplex initial approximation of prime vector x
*  using modified simplex method with two-side constraints on x
*/
void TMulti::SimplexInitialApproximation( )
{
    int T,Q,*STR=0,*NMB=0;
    register int i,j,k;
    double GZ,EPS,*DN=0,*DU=0,*AA=0,*B1=0;
    /*  char DCC, ICC, PHC; */
    try
    {  // Allocation of work arrays
        pmp->Ec=0;
        Q=pmp->L;
        DN= new double[Q];
        DU= new double[Q+pmp->N];
        B1= new double[pmp->N];
        ErrorIf( !DN || !DU || !B1, "SimplexInitialApproximation()", "Memory alloc erorr." );
        memset(DN, 0, sizeof(double)*Q );
        memset(DU, 0, sizeof(double)*(Q+pmp->N) );
        memset(B1, 0, sizeof(double)*pmp->N );

        // GZ = 1e+9; /* 1e6 */
        // EPS = 1e-9;    pmp->EPS; 1e-6;  Precision ?
        EPS = TProfil::pm->pa.p.EPS; //  added 13.10.00  KVC  DAK
        GZ = 1./EPS;    //  added 13.10.00

        T=0; // Calcuation of all non-zero values in A and G arrays
        for(i=0;i<pmp->L;i++)
            if(fabs(pmp->G[i])>1E-19)
                T++;
        for(j=0;j<pmp->N;j++)
            for(i=0;i<pmp->L;i++)
                if(fabs(*(pmp->A+i*pmp->N+j))>1E-19)
                    T++;
        if( pmp->PLIM ) // Setting constraints on x elements
            Set_DC_limits(  DC_LIM_INIT );

        for(i=0;i<Q;i++)
        {
            DN[i]=pmp->DLL[i];
            DU[i]=pmp->DUL[i];
        }

        /* for(i=Q;i<Q+pmp->N;i++) DU[i]=0.; */
        // Allocation of arrays on T
        AA= new double[T];
        STR= new int[T];
        NMB= new int[Q+1];
        ErrorIf( !AA || !STR || !NMB, "SimplexInitialApproximation", "Memory alloc error #2");
        memset(AA, 0, sizeof(double)*T );
        memset(STR, 0, sizeof(int)*T );
        memset(NMB, 0, sizeof(int)*(Q+1) );

        // if( wn[W_EQCALC].status )
        //   aSubMod[MD_EQCALC]->ModUpdate(" SIMPLEX Approximation ");

        // Copying vector b
        for(j=0;j<pmp->N;j++)
            B1[j]=pmp->B[j];
        k=-1;
        for(i=0;i<pmp->L;i++)
        {  // Loading non-zero values
            if(fabs(pmp->G[i])>1E-19)
            {
                k++;
                AA[k]=-pmp->G[i];
                NMB[i]=k+1;
            }
            else NMB[i]=k+2;
            for(j=0;j<pmp->N;j++)
                if(fabs(*(pmp->A+i*pmp->N+j))>1E-19)
                {
                    k++;
                    AA[k]=*(pmp->A+i*pmp->N+j);
                    STR[k]=j+1;
                }
        }
        NMB[Q]=T+1;
        // Calling generic simplex solver
        Simplex(pmp->N,Q,T,GZ,EPS,DN,DU,B1,pmp->U,AA,STR,NMB);

        // unloading simplex solution into a copy of x vector
        for(i=0;i<pmp->L;i++)
            pmp->Y[i]=DU[i];

        // calculating initial quantities of phases
        TotalPhases( pmp->Y, pmp->YF, pmp->YFA );

        pmp->FX = GX( 0.0 ); // calculation of initial G(X) value
        MassBalanceDeviations( pmp->N, pmp->L, pmp->A, pmp->Y, pmp->B, pmp->C );
        // Freeing work arrays
        if( DN) delete[]DN;
        if( DU) delete[]DU;
        if( AA) delete[]AA;
        if( B1) delete[]B1;
        if( STR) delete[]STR;
        if( NMB) delete[]NMB;
    }
    catch( TError& xcpt )
    {
        if( DN) delete[]DN;
        if( DU) delete[]DU;
        if( AA) delete[]AA;
        if( B1) delete[]B1;
        if( STR) delete[]STR;
        if( NMB) delete[]NMB;
        Error( xcpt.title.c_str(), xcpt.mess.c_str());
    }

}
//--------------------- End of ipm_main.cpp ---------------------------
