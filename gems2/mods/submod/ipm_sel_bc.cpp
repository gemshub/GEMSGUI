//-------------------------------------------------------------------
// $Id$
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
#include <math.h>

#include "m_param.h"

#ifndef IPMGEMPLUGIN

#include "visor.h"
//#include "visor_w.h"
#include "service.h"
#include "stepwise.h"

#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Advanced solver of a system of linear equations
// using Jordan method
// N - dimension of the matrix A
// DG - balance precision ( 1e-14 )
// A - input matrix N(N+1) (destroyed during calculations);
//            Last column is a vector of independent variables.
// X - solution vector (N).
// Return values: 0  - solved OK;
//                1  - no solution, degenerated or inconsistent system;
//
#define  a(i,j) (*(A+(j)+(i)*N1))

int TProfil::Gordan( int N, double DG, double *A, double *X )
{
    double AMAX,DIA;      /*  A[N][N+1]  */
    int I,J,K,N1,II,IR,*INV,iRet=0;

    INV = new int[N];
    memset(INV, 0, sizeof(int)*N );
    ErrorIf( !INV, "Gordan", "Error of memory allocation" );
    N1 = N+1;
    for(I=0;I<N;I++)
    {
        AMAX=0.0;
        for(J=0;J<N;J++)
        {
            if( INV[J]!=0)
                continue;
            for(K=0;K<N;K++)
            {
                if( INV[K]!=0)
                    continue;
                if( AMAX<fabs( a(J,K) ))
                {
                    AMAX=fabs( a(J,K) );
                    IR=J;
                    II=K;
                }
            }
            if(AMAX<1e-39)
            {
                iRet=1;
                goto KN;
            }
        }
        if(IR!=II)
            for(J=0;J<=N;J++)
            {
                DIA= a(IR,J); /*  *(A+IR*N1+J); */
                a(IR,J) = a(II,J); /* *(A+IR*N1+J)=*(A+II*N1+J); */
                a(II,J) = DIA;     /* *(A+II*N1+J)=DIA; */
            }
        DIA= a(II,II); /* *(A+II*N1+II); */
        a(II,II) = 1.0;   /* *(A+II*N1+II)=1.0; */
        INV[II]=II+1;
        for(J=0;J<N;J++)
            if(INV[J]==0)
                a( II,J ) /= DIA;  /*   *(A+II*N1+J)/=DIA; */
        a(II,N) /= DIA;
        for(J=0;J<N;J++)
            if(J!=II)
            {
                DIA=a(J,II);
                a(J,II)=0.0;
                for(K=0;K<N;K++)
                    if(INV[K]==0)
                        a(J,K)-=a(II,K)*DIA;
                a(J,N)-=a(II,N)*DIA;
            }
    }
    for(I=0;I<N;I++)
        X[INV[I]-1]=a(INV[I]-1,N);  /*  *(A+(INV[I]-1)*N1+N); */

KN:
    delete[] INV;
    return(iRet);
}
#undef a

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Solver of a system of linear equations using method of square roots
// N - dimension of the matrix R (number of equations)
// DG - balance precision ( 1e-14 )
// R - input matrix of coeffs N(N+1);
//     Last column is a vector of independent variables.
// X - solution vector (N).
// B - work matrix of  N(N+1)/2 dimensions
// Return values: 0  - solved OK;
//                1  - no solution, degenerated or inconsistent system;
#define  r(i,j) (*(R+(j)+(i)*N1))
//
int TProfil::SquareRoots( int N, double *R, double *X, double *B )
{
    int I,J,K,P,Q,N1, iRet=0;
    double F,G;
    N1 = N + 1;
    // ErrorIf( !B, "SquareRoots", "Error param B." );
    memset( B, 0, N*N1*sizeof(double));
    Q=-1;
    for(I=0;I<N;I++)
    {
        for(J=I;J<N;J++)
            B[Q+J-I+1]= r(I,J);  /*       *(R+I*N1+J); */
        Q=Q+N-I;
    }
    for(I=0;I<N;I++)
        X[I]=  r(I,N);      /*       *(R+I*N1+N); */
    Q=0;
    for(I=0;I<N;I++)
    {
        F=B[Q];
        P=I;
        for(J=0;J<=I-1;J++)
        {
            G=B[P];
            F-=G*G;
            for(K=1;K<N-I;K++)
                B[Q+K]-=G*B[P+K];
            P+=N-J-1;
        }
        if( F<1E-39)
        {
            iRet=1;
            goto KN;
        }
        F=1/sqrt(F);
        B[Q]=F;
        for(K=1;K<N-I;K++)
            B[Q+K]*=F;
        Q+=N-I;
    }
    Q=0;
    for(I=0;I<N;I++)
    {
        P=I;
        F=X[I];
        for(J=0;J<=I-1;J++)
        {
            F-=X[J]*B[P];
            P+=N-J-1;
        }
        X[I]=B[Q]*F;
        Q+=N-I;
    }
    Q=N*N1/2-1;
    for(I=N-1;I>=0;I--)
    {
        F=X[I];
        for(J=I+1;J<N;J++)
            F-=X[J]*B[Q+J-I];
        X[I]=B[Q]*F;
        Q=Q-N+I-1;
    }
KN:
    return(iRet);
}
#undef r

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* Calculate mass-balance deviations
*  Params: N - number of IC in IPM problem
*          L -   number of DC in IPM problem
*          A - DC stoichiometry matrix (LxN)
*          Y - moles  DC quantities in IPM solution (L)
*          B - Input bulk chem. compos. (N)
*          C - deviations (N) */
// void TProfil::NBM( int N, int L, float *A, double *Y, double *B, double *C )
// {
//  short I,J;
//  for(J=0;J<N;J++)
//  {
//    C[J]=B[J];
//    for(I=0;I<L;I++)
//       C[J]-=*(A+I*pmp->N+J)*Y[I];
//  }
// }

// Checks of Karpov phase stability criteria,
// Selekt2() procedure by Karpov & Chudnenko
// modified by DAK in 1995
// Returns 0, if new IPM loop is needed;
//         1, if the solution is final.
//
#define  a(j,i) (*(pm[q].A+(i)+(j)*pm[q].N))
//
void TProfil::PhaseSelect()
{
    short II,JJ,Z,I,J;//, iRet=0;
    double F1,F2,F3=0.0,*F0;

    f_alpha( );
    F0 = pmp->Falp;

    (pmp->K2)++;
    pmp->MK=0;
    JJ= -1;
    II= -1;
    F1= F2= pmp->lowPosNum*10000.;  // 1E-16;

    for(Z=0;Z<pmp->FI;Z++)
    {
        if( F0[Z]>F1 && pmp->YF[Z]<pmp->lowPosNum )
        {
            F1=F0[Z];  // selection of max Fa and phase index
            JJ=Z;
        }
        if( F0[Z]>F2 && pmp->YF[Z]>pmp->lowPosNum )
        {
            F2=F0[Z];
            II=Z;
        }
    }
    if( F1 > pa.p.DF && JJ >= 0 )
    {
        double sfactor, molB=0.0;
        int i, NN;
        F3=F1;
        // There is a phase for which DF (0.01) is exceeded
        //   S2:;
        NN = pmp->N - pmp->E;
        for(i=0;i<NN;i++)
           molB += pmp->B[i];
        sfactor = pow( molB, 0.4 )/7.7; // see ms_mueqexp.cpp(59)
        do
        {  // insert all phases with  F1 > DFM
            J=0; // insert this phase and set Y[j] for its components
            // with account for asymmetry and non-ideality
            for( I=0; I<JJ/*-1*/; I++ )   // A bug ?
                 J+=pmp->L1[I];
            pmp->YF[JJ] = 0.;
            for(I=J; I<J+pmp->L1[JJ]; I++)
            {
                switch( pmp->DCC[I] )
                {
                case DC_AQ_PROTON:
                case DC_AQ_ELECTRON:
                case DC_AQ_SPECIES:
                    pmp->Y[I] = pa.p.DFYaq * sfactor;
                    break;
                case DC_AQ_SOLVCOM:
                case DC_AQ_SOLVENT:
                    pmp->Y[I] = pa.p.DFYw * sfactor;
                    break;
                case DC_GAS_H2O:
                case DC_GAS_CO2:
                case DC_GAS_H2:
                case DC_GAS_N2:
                case DC_GAS_COMP:
                case DC_SOL_IDEAL:
                    pmp->Y[I] = pa.p.DFYid * sfactor;
                    break;
                case DC_SOL_MINOR:
                    pmp->Y[I] = pa.p.DFYh * sfactor;
                    break;
                case DC_SOL_MAJOR:
                    pmp->Y[I] = pa.p.DFYr * sfactor;
                    break;
                case DC_SCP_CONDEN:
                    pmp->Y[I] = pa.p.DFYc * sfactor;
                    break;
                    // implementation for adsorption?
                default:
                    pmp->Y[I] = pa.p.DFYaq * sfactor;
                    break;
                }
                pmp->YF[JJ] += pmp->Y[I];
            } /* I */
            pmp->FI1++;  // check phase rule
            if( pmp->FI1 >= pmp->NR+1 )
            { // No more phase can be inserted
                break; /* goto S5; */
            }
            // find a new phase to insert, if any
            F1= pmp->lowPosNum*10000.; // 1e-16
//          F1= pa.p.DFM-pa.p.DF; /* 1E-16; */
            JJ = -1;
            for( Z=0; Z<pmp->FI; Z++ )
                if( F0[Z] > F1 && pmp->YF[Z] < pmp->lowPosNum )
                {
                    F1=F0[Z];
                    JJ=Z;
                }
        }
        while( F1 > pa.p.DF && JJ >= 0/* 1E-15 */ );  /* goto S2; */
        // end of insertion cycle
        J=0;   /* S5: insert prime IPM solution */
        for(Z=0;Z<pmp->FIs;Z++)
        {
            if( pmp->YF[Z ] > pmp->lowPosNum /* 1E-18 */ )
            {
                pmp->YF[Z]=0.;
                for(I=J;I<J+pmp->L1[Z];I++)
                {
                   if( pmp->Y[I] < pmp->lowPosNum ) // Check what to insert !
                       pmp->Y[I] = pa.p.DFM; // lowPosNum?
                    pmp->YF[Z]+=pmp->Y[I]; // calc new  quantities of phases
                }
            }
            J+=pmp->L1[Z];
        }
        //  if( wn[W_WORK1].status )
#ifndef IPMGEMPLUGIN
        pVisor->Update(false);  // "PhaseSelection"
#endif
        if( pmp->K2>1 )
        { // more then first step - solution is not improved
            for(I=0;I<pmp->L;I++)
                if( fabs(pmp->Y[I]- pmp->XY[I]) > pa.p.DF*pmp->Y[I] ) // Check!
                    goto S6;
            pmp->PZ=2;
            F1=F3;
            goto S4;
        }
S6: // copy X changed by SELEKT2 algorithm
        for(I=0;I<pmp->L;I++)
            pmp->XY[I]=pmp->Y[I];
// Copy here also to pmp->X[I]?
        return;
        /*  else goto S5; */
    }
S4: // No phases to insert or no distortions
    if( F1<F2 )
    {
        F1=F2;
        JJ=II;
    }
    // end of iterations of SELEKT2
    pmp->MK=1;
    return;
}
#undef a

// ------------------- ------------------ ----------------
// Calculation of feasible IPM initial approximation point
//
// Algorithm: see Karpov, Chudnenko, Kulik 1997 Amer.J.Sci. vol 297 p. 798-799
// (Appendix B)
//
// Returns: 0 - OK,
//          1 - no convergence at specified precision DHB
//
#define  r(i,j) (*(R+(j)+(i)*N1))
#define  a(j,i) (*(pmp->A+(i)+(j)*pmp->N))
//
int TProfil::EnterFeasibleDomain( )
{
    short IT1;
    int I, J, Z,  N, N1, sRet, iRet=0;
//    time_t old_time, new_time;
    double DHB, PCI, LM, LM1, E,
    *MU,*W,*R,*R1;

    MU=pmp->MU;
    W=pmp->W;
    ErrorIf( !MU || !W, "EnterFeasibleDomain()", "Error alloc pmp->MU or pmp->W." );
    memset( MU, 0, pmp->L*sizeof(double));
    memset( W, 0, pmp->L*sizeof(double));

    E = 1E-30;  // pa.p.DKIN? Default min value of Lagrange multiplier p
    DHB = pmp->DHBM;  // Convergence (balance precision) criterion
    pmp->Ec=0;  // Return code

    // calculation of total moles of phases
    TotalPhases( pmp->Y, pmp->YF, pmp->YFA );

    // if( wn[W_EQCALC].status )
    // {
    // load demo vector phases
    //    PhaseListPress();
    //    aSubMod[MD_EQCALC]->ModUpdate("Feasible Approx: iterations");
    //    new_time = time( &old_time );
    // }

    if( pmp->PLIM )
        if( Set_DC_limits(  DC_LIM_INIT ) )
        {
#ifndef IPMGEMPLUGIN
            if( !vfQuestion(window(), "EnterFeasibleDomain()",
                            "Inconsistent DC limits. Continue?" ))
                goto SINGULAR;
#else
        cout<< "EnterFeasibleDomain: Inconsistent DC limits"<< endl;
#endif
        }
    for(J=0;J<pmp->L;J++)
    {
        if( pmp->Y[J] < pmp->lowPosNum )
            continue;
        switch( pmp->RLC[J] )
        { // Adjustment of prime approximation according to kinetic constraints
        case NO_LIM:
        case LOWER_LIM:
            if( pmp->Y[J]<=pmp->DLL[J])
                pmp->Y[J]=pmp->DLL[J]+E;
            break;
        case BOTH_LIM:
            if( pmp->Y[J]<=pmp->DLL[J])
                pmp->Y[J]=pmp->DLL[J]+E;  /* 1e-10: pa.DKIN ? */
        case UPPER_LIM:
            if( pmp->Y[J]>=pmp->DUL[J])
                pmp->Y[J]=pmp->DUL[J]-E;
            break;
        }
    }
// BEGIN:
    for( IT1=0; IT1 < pa.p.DP; IT1++, pmp->IT++ )
    {  /* main cycle */
        pmp->NR=pmp->N;
        if( pmp->LO )
        {
            if( pmp->YF[0] < pmp->DSM && pmp->YFA[0] < pmp->lowPosNum*100.)
                pmp->NR= pmp->N-1;
        }
        N=pmp->NR;
        N1=N+1;
  MassBalanceDeviations( pmp->N, pmp->L, pmp->A, pmp->Y, pmp->B, pmp->C);

#ifndef IPMGEMPLUGIN
  pVisor->Update( false );
#endif
  Z=pmp->N - pmp->E;
  if(!pmp->W1)
   for(I=0;I<Z;I++)
   {
     if(fabs(pmp->C[I])>DHB)
     goto NEXT;
   }
   else
   for(I=0;I<Z;I++)
     if(fabs(pmp->C[I]) > DHB // * pa.p.GAS
        || fabs(pmp->C[I]) > pmp->B[I] * pa.p.GAS)
        goto NEXT;
     pmp->IT -= IT1;
//        for(I=0;I<pmp->NR;I++)
//            if(fabs(pmp->C[I]) > DHB )
//                goto NEXT;
        goto DONE;
NEXT:
        R=pmp->R;
        R1=pmp->R1;
        ErrorIf(  !R || !R1, "EnterFeasibleDomain()", "Error alloc R or R1" );
        memset( R, 0, N*N1*sizeof(double));

        for(J=0;J<pmp->L;J++)
        {
            switch( pmp->RLC[J] )
            { // Calculation of weight multipliers
            case NO_LIM:
            case LOWER_LIM:
                W[J]=(pmp->Y[J]-pmp->DLL[J])*(pmp->Y[J]-pmp->DLL[J]);
                break;
            case UPPER_LIM:
                W[J]=(pmp->DUL[J]-pmp->Y[J])*(pmp->DUL[J]-pmp->Y[J]);
                break;
            case BOTH_LIM:
                W[J]=((pmp->Y[J]-pmp->DLL[J])*(pmp->Y[J]-pmp->DLL[J])
                      < (pmp->DUL[J]-pmp->Y[J])*(pmp->DUL[J]-pmp->Y[J]))
                     ? (pmp->Y[J]-pmp->DLL[J])*(pmp->Y[J]-pmp->DLL[J]):
                     (pmp->DUL[J]-pmp->Y[J])*(pmp->DUL[J]-pmp->Y[J]);
            }
        }
        for(Z=0;Z<N;Z++) // Assembling system of linear equations
            for(I=0;I<N;I++)
            {
                r(Z,I) = 0.;
                for(J=0;J<pmp->L;J++)
                    if( pmp->Y[J]>pmp->lowPosNum /*1E-19 */ )
                        r(Z,I) += a(J,I) * a(J,Z) * W[J];
            }
        for(I=0;I<N;I++)
            r(I,N) = pmp->C[I];
        /* Solving system of linear equations */
        sRet = SquareRoots( N, R, pmp->U, R1 );
        if( sRet == 1 )
            goto TRY_GORDAN;
        goto SOLVED;
TRY_GORDAN:
        R1=pmp->R1;
        ErrorIf(  !R1, "EnterFeasibleDomain()", "Lost allocation of R1" );
        memcpy( R1, R, sizeof( double )*N*N1 );
        sRet = Gordan( N, pa.p.DG, R1, pmp->U );
        if( sRet == 1 )
            //       goto TRY_RISLUR;
            //   goto SOLVED;
            // TRY_RISLUR:       Removed 12.10.00 KVC & DAK
            goto SINGULAR;
SOLVED:  // solution of linear matrix obtained
        PCI=0.;  // calc of MU values and Dikin criterion
        for(J=0;J<pmp->L;J++)
        {
            MU[J]=0.; // initializing dual potentials
            if( pmp->Y[J] > pmp->lowPosNum /*1E-19 */)
            {
                for(I=0;I<N;I++)
                    MU[J] += a(J,I)*pmp->U[I];
                PCI+=MU[J]*W[J]*MU[J];
                MU[J]*=W[J];
            }
        }
        if( PCI > pmp->lowPosNum /* 1E-19*/ )
            PCI=1/sqrt(PCI);
        else PCI=1.; // zero Psi value ?

        for(J=0;J<pmp->L;J++)
        {  // Calculation of descent step size LM
            if( pmp->RLC[J]==NO_LIM ||
                    pmp->RLC[J]==LOWER_LIM || pmp->RLC[J]==BOTH_LIM )
            {
                if( MU[J]<0 && fabs(MU[J])>pmp->lowPosNum*100.)  //1E-16)
                {
                    Z=J;
                    LM=(-1)*(pmp->Y[Z]-pmp->DLL[Z])/MU[Z];
                    goto BB6;
                }
            }
            if( pmp->RLC[J]==UPPER_LIM || pmp->RLC[J]==BOTH_LIM )
            {
                if( MU[J]>pmp->lowPosNum*100.)  //1E-16)
                {
                    Z=J;
                    LM=(pmp->DUL[Z]-pmp->Y[Z])/MU[Z];
                    goto BB6;
                }
            }
        }
        LM=PCI;
        goto BB7;
BB6:
        for(J=Z+1;J<pmp->L;J++)
        {
            if( pmp->RLC[J]==NO_LIM ||
                    pmp->RLC[J]==LOWER_LIM || pmp->RLC[J]==BOTH_LIM )
            {
                if( MU[J]<0 && fabs(MU[J])>pmp->lowPosNum*100.)  //1E-16)
                {
                    LM1=(-1)*(pmp->Y[J]-pmp->DLL[J])/MU[J];
                    if( LM>LM1)
                        LM=LM1;
                }
            }
            if( pmp->RLC[J]==UPPER_LIM || pmp->RLC[J]==BOTH_LIM )
            {
                if( MU[J]>pmp->lowPosNum*100.)  //1E-16)
                {
                    LM1=(pmp->DUL[J]-pmp->Y[J])/MU[J];
                    if( LM>LM1)
                        LM=LM1;
                }
            }
        }
        LM *= .95;     // Smoothing of final lambda ????
BB7:
        if( LM > 1)
            LM = 1;
        //
        // calculation of new prime solution approximation
        for(J=0;J<pmp->L;J++)
            pmp->Y[J] += LM * MU[J];
// STEPWISE (5) Stop point at end of iteration of FIA()
#ifndef IPMGEMPLUGIN
STEP_POINT("FIA Iteration");
#endif
        // calculation of new total moles of phases
        // added by DAK in 1995
        // TotalPhases( pmp->Y, pmp->YF, pmp->YFA );
        // if( pmp->PLIM )
        //   if( Set_DC_limits(  DC_LIM_INIT ) )
        //   {
        //     if( !vfQuestion(window(), "EnterFeasibleDomain():",
        //        " Inconsistent DC limits. Continue?"  ))
        //       goto SINGULAR;
        //   }
        if( LM < 1E-5 )
        { // Too small step size - too slow convergence !
            pmp->Ec=1;
            goto KN;
        }

        //   if( wn[W_EQCALC].status )
        //   if( time( &new_time ) - old_time > 1L )
        //   {
        //  load demo vector phases
        //  PhaseListPress();
        //  aSubMod[MD_EQCALC]->ModUpdate("Feasible Approx: iterations");
        //  time( &old_time );
        //   }
        //
        //   if( pmp->pNP && IT1 >= pa.p.DP ) break;  // is this necessary ?
        //
    }  /* End loop on IT1 */
    //  Prescribed mass balance precision cannot be reached
    //  Take a look at vector b or values of DHB and DS

SINGULAR:
    // wn[W_EQCALC].status = 1;
#ifndef IPMGEMPLUGIN
#ifndef Use_mt_mode
    pVisor->Update(false);  // "Feasible Approx: error"
#endif
#endif
    pmp->Ec=1;
    iRet = 1;
    goto DONE;

KN:  /* if(pmp->DHBM<1e-6)
      {
        pmp->DHBM *= 10;
        pmp->Ec=iRet=0;
        goto BEGIN;
      }
      Error("EnterFeasibleDomain error: ",
            "Prescribed mass balance precision cannot be reached.");  */
DONE:
    return iRet;
}
#undef a
#undef r

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Interior Points Method:
// subroutine for unconditional minimization of descent step length
// on the interval 0 to LM
// uses the "Golden Section" procedure
//
// Returns: optimal value of LM which provides largest monotonous
// decrease in G(X)
//
double TProfil::LMD( double LM )
{
    double A,B,C,LM1,LM2,FX1,FX2;
    A=0.0;
    B=LM;
    if( LM<2.)
        C=.05*LM;
    else C=.1;
    if( B-A<C)
        goto OCT;
    LM1=A+.382*(B-A);
    LM2=A+.618*(B-A);
    FX1= GX( LM1 );
    FX2= GX( LM2 );
SH1:
    if( FX1>FX2)
        goto SH2;
    else goto SH3;
SH2:
    A=LM1;
    if( B-A<C)
        goto OCT;
    LM1=LM2;
    FX1=FX2;
    LM2=A+.618*(B-A);
    FX2=GX( LM2 );
    goto SH1;
SH3:
    B=LM2;
    if( B-A<C)
        goto OCT;
    LM2=LM1;
    FX2=FX1;
    LM1=A+.382*(B-A);
    FX1=GX( LM1 );
    goto SH1;
OCT:
    LM1=A+(B-A)/2;
    return(LM1);
}

/* access to element of R matrix */
#define  r(i,j) (*(R+(j)+(i)*N1))
/* access to element of A matrix */
#define  a(j,i) (*(pmp->A+(i)+(j)*pmp->N))

// One iteration of Interior Points Method algorithm
// (see Karpov et al., 1997, p. 785-786 )
// Returns 0 if solved OK;
//         1 in case of degeneration of linearized matrix R
//
int TProfil::InteriorPointsIteration( )
{
    int N, N1, i,j, z, sRet;
    double *F,*W,*R, *R1,  Mu;
    gstring Cpoint;

    N = pmp->NR;
    N1=N+1;
    R=pmp->R;
    R1=pmp->R1;
    F=pmp->F;
    W=pmp->W;
    // ErrorIf( !R || !R1 || !F || !W, "InteriorPointsIteration",
    //  "Error alloc R or R1 or F or W" );
    memset( F, 0, pmp->L*sizeof(double));
    memset( W, 0, pmp->L*sizeof(double));

    PrimeChemicalPotentials( pmp->F, pmp->Y, pmp->YF, pmp->YFA );

    // Set weight multipliers for DC
    for(j=0;j<pmp->L;j++)
    {
        switch( pmp->RLC[j] )
        {
        case NO_LIM:
        case LOWER_LIM:
            W[j] = pmp->Y[j]-pmp->DLL[j];
            break;
        case UPPER_LIM:
            W[j] = pmp->DUL[j]-pmp->Y[j];
            break;
        case BOTH_LIM:
            W[j]= ((pmp->Y[j]-pmp->DLL[j])<(pmp->DUL[j]-pmp->Y[j]))?
                  (pmp->Y[j]-pmp->DLL[j]): (pmp->DUL[j]-pmp->Y[j]);
            break;    // added to bugfix by DAK 22.08.01 PSI
        default: // big error
            Error( "InteriorPointsIteration", "Error in code pmp->RLC[j]!!!!" );
        }
        if( W[j] < 0. ) W[j]=0.;  // Weight multipliers cannot be negative
    }

    // making R matrix of IPM linear equations

    memset( R, 0, N*N1*sizeof(double));

    for( z=0; z<N; z++)
        for( i=0; i<N; i++ )
        { // r(z,i) = 0.;
            for( j=0; j<pmp->L; j++ )
                if( pmp->Y[j] > pmp->lowPosNum /* 1E-19 */ )
                    r(z,i) += a(j,i) * a(j,z) * W[j];
        }
    for( i=0; i<N; i++ )
    {  // r(i,N) = 0.;
        for( j=0; j<pmp->L; j++)
            if( pmp->Y[j] > pmp->lowPosNum /* 1E-19 */ )
                r(i,N) += F[j] * a(j,i) * W[j];
    }
    // if( wn[W_WORK1].status )
    // pVisor->Update(false); "InPoint: R matrix"  ???????

    // Solving matrix R of linear equations
    sRet = SquareRoots( N, R, pmp->U, R1 );
    if( sRet == 1 )
        goto TRY_GORDAN;
    Cpoint = "SquareRoots(): U vector";
    goto SOLVED;
TRY_GORDAN:
    R1 = pmp->R1;
    ErrorIf( !R1 , "InteriorPointsIteration","Error alloc R1." );
    memcpy( R1, R, sizeof(double)*N*N1 );

    // if( wn[W_WORK1].status )
    //  pVisor->Update(false);  "Jordan(): U vector"

    sRet = Gordan( N, pa.p.DG, R1, pmp->U );
    if( sRet == 1 )
    {
#ifndef IPMGEMPLUGIN
        pVisor->Update(false); // "Degeneration in InPoint()"
#endif
        pmp->Ec=1;
        return 1;
    }
    Cpoint = "Jordan(): U vector";

SOLVED:  // got U vector - calculating criteria Karpov and Dikin
    f_alpha( );
    // calculation of vector MU - direction of descent
    pmp->PCI=0.;
    for( j=0; j<pmp->L; j++)
        if( pmp->Y[j] > pmp->lowPosNum /* 1E-19 */ )
        {
            Mu = DualChemPot( pmp->U, pmp->A+j*pmp->N, N );
            Mu -= F[j];
            pmp->MU[j] = Mu * W[j];
            pmp->PCI += Mu * W[j] * Mu; // Dikin criterion
        }

        else pmp->MU[j] = 0.0;
    return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Calculation of chemical equilibrium using Interior Points
//  Method algorithm (see Karpov et al., 1997, p. 785-786)
//  GEM IPM
// Returns: 0, if converged;
//          1, in case of degeneration, divergence or
//             user interruption
//
int TProfil::InteriorPointsMethod( )
{
    int /*INO,*/IT1,I,J,JJ,Z,iRet;
    double LM=0.,LM1,FX1;
//    time_t old_time, new_time;

    if( pmp->FIs )
        for( J=0; J<pmp->Ls; J++ )
            pmp->lnGmo[J] = pmp->lnGam[J];

   //  pmp->W1=0;  pmp->IC=0.; INO=0;

    for(J=0;J<pmp->N;J++)
    {
        pmp->U[J]=0.;
        pmp->C[J]=0.;
    }
    pmp->Ec=0;

    pmp->FX=GX( LM  );  // calculation of G(x)

//    for(Z=0; Z<pmp->FI; Z++)
//        pmp->YF[Z]=pmp->XF[Z];
    if( pmp->FIs ) // multicomponent phases present
        for(Z=0; Z<pmp->FIs; Z++)
            pmp->YFA[Z]=pmp->XFA[Z];

    //  if( wn[W_EQCALC].status )
    //  {
    //     aSubMod[MD_EQCALC]->ModUpdate("Entering IPM EqCalc()");
    //     new_time = time( &old_time );
    // IFTshow( IFV_HELPMSG,
    // "Start of iterations of Gibbs energy minimization by IPM algorithm" );
    //  }

    for( IT1 = 0; IT1 < pa.p.IIM; IT1++, pmp->IT++ )
    {  /* main cycle of IPM iterations */
       // if(pmp->W1)
  //  if( IT1 > 22 )
  //   multi->to_text_file();  //test for calck 17/06/02

        pmp->NR=pmp->N;
        if( pmp->LO ) // water-solvent is present
        {
            if( pmp->YF[0]<pmp->DSM && pmp->Y[pmp->LO]< pmp->lowPosNum *100.)
                pmp->NR=pmp->N-1;
        }

        iRet = InteriorPointsIteration();
        if( iRet ) // degeneration !!!
            return iRet;

       // Determination of descent step size  LM
        for(J=0;J<pmp->L;J++)
        {
            if( pmp->RLC[J]==NO_LIM ||
                    pmp->RLC[J]==LOWER_LIM || pmp->RLC[J]==BOTH_LIM )
            {
                if( pmp->MU[J]<0 && fabs(pmp->MU[J])>pmp->lowPosNum*100.)  //1E-16 )
                {
                    Z=J;
                    LM=(-1)*(pmp->Y[Z]-pmp->DLL[Z])/pmp->MU[Z];
                    goto IN5;
                }
            }
            if( pmp->RLC[J]==UPPER_LIM || pmp->RLC[J]==BOTH_LIM )
            {
                if( pmp->MU[J]>pmp->lowPosNum*100.) //1E-16 )
                {
                    Z=J;
                    LM=(pmp->DUL[Z]-pmp->Y[Z])/pmp->MU[Z];
                    goto IN5;
                }
            }
        }
        LM=1./sqrt(pmp->PCI);
        goto IN6;

IN5:
        for(J=Z+1;J<pmp->L;J++)
        {
            if( pmp->RLC[J]==NO_LIM ||
                    pmp->RLC[J]==LOWER_LIM || pmp->RLC[J]==BOTH_LIM )
            {
                if( pmp->MU[J]<0 && fabs(pmp->MU[J])>pmp->lowPosNum*100.)  //1E-16 )
                {
                    LM1=(-1)*(pmp->Y[J]-pmp->DLL[J])/pmp->MU[J];
                    if( LM>LM1 )
                        LM=LM1;
                }
            }
            if( pmp->RLC[J]==UPPER_LIM || pmp->RLC[J]==BOTH_LIM )
            {
                if( pmp->MU[J]>pmp->lowPosNum*100.) //1E-16 )
                {
                    LM1= (pmp->DUL[J]-pmp->Y[J])/pmp->MU[J];
                    if( LM>LM1 )
                        LM=LM1;
                }
            }
        }
IN6:
        LM1=LMD( LM ); // Finding optimal value of descent step
        FX1=GX( LM1 ); // New G(X) after the descent step
        pmp->PCI=sqrt(pmp->PCI); // Dikin criterion

        //  recalc min and max restrictions to DC */
        //  if( pmp->PLIM )
        //    if( Set_DC_limits(  DC_LIM_CURRENT ))
        //    {
        //      if( !vfQuestion(window(), "IPM:",
        //      " Inconsistent metastability restrictions to DC or phases.\n"
        //      "Continue calculation (take those restrictions as trivial)?" ))
        //        goto FAIL_EQ;
        //    }

        if( IT1 > 3 && FX1 - pmp->FX > 10  && ( IT1 > pa.p.IIM/2 ||
                                                pmp->PD==3 ) )  // ????????
        {
            if( pmp->LO && pmp->X[pmp->LO]>pmp->lowPosNum && pmp->PD==3 )
                pmp->PD=2;
            else  pmp->DX= 0.5 * pmp->PCI;
        }
        MassBalanceDeviations( pmp->N, pmp->L, pmp->A, pmp->Y, pmp->B, pmp->C );

        //    if( wn[W_EQCALC].status )
        //    {
#ifndef IPMGEMPLUGIN

#ifndef Use_mt_mode
    pVisor->Update(false);
#endif

#endif
        //      if( time( &new_time ) - old_time >= (long)pa.p.DT )
        //      {
        /* load demo vector phases */
        //    PhaseListPress();
        //    aSubMod[MD_EQCALC]->ModUpdate("PM_ipmi   Iteration of IPM EqCalc()");
        //    time( &old_time );
        //      }
        //    }
        pmp->FX=FX1;

        // main iteration done 
        JJ=0;
        // Calculation of activity coefficients on IPM iteration
        if( pmp->PD==3 )
            GammaCalc( LINK_UX_MODE );

        if( pmp->PHC[0] == PH_AQUEL && pmp->X[pmp->LO] < pa.p.XwMin )
        {
            pmp->XF[0]=0.;  // elimination of aqueous phase
            pmp->XFA[0] = 0.;
        }
        // Restoring vectors Y and YF
        for(Z=0;Z<pmp->FI;Z++)
        {
            if( pmp->XF[Z] <= pmp->DSM ||
                    pmp->PHC[Z] == PH_SORPTION && (pmp->XFA[Z] < pa.p.ScMin) )
            {
                pmp->YF[Z]= 0.;
                if( pmp->FIs && Z<pmp->FIs )
                    pmp->YFA[Z] = 0.;
                for(I=JJ; I<JJ+pmp->L1[Z]; I++)
                {
                    pmp->Y[I]=0.;
                    pmp->lnGam[I] = 0.;
                }
            }
            else            {
                pmp->YF[Z]=pmp->XF[Z];
                if( pmp->FIs && Z<pmp->FIs )
                    pmp->YFA[Z]=pmp->XFA[Z];
                for(I=JJ;I<JJ+pmp->L1[Z];I++)
                    pmp->Y[I]=pmp->X[I];
            }
            JJ+=pmp->L1[Z];
        };
// STEPWISE (6)  Stop point at IPM() main iteration
#ifndef IPMGEMPLUGIN
STEP_POINT( "IPM Iteration" );
#endif
        if( pmp->PCI < pmp->DX )  // Dikin criterion satisfied
            goto IN7;
    } // end of main IPM cycle
// FAIL_EQ:
/*    if( vfQuestion(window(), "IPM:",
        "For a given IPM convergence criterion, vector b is not balanced,\n"
        "or DC standard-state thermodynamic data inconsistent. \n"
        "Browse debug data screen forms (Y) Skip to abnormal exit from IPM (N)?" ))
    {
        //    wn[W_EQCALC].status = 1;
        ///   aSubMod[MD_EQCALC]->Show("Imprecise result");
        pVisor->Update( true );
    }  */

    return 2; // IT>500

IN7: // PhaseListPress();
    // Final calculation of activity coefficients
 //   for( I=0; I<pmp->L; I++ )
 //       pmp->X[I]=pmp->Y[I];
     TotalPhases( pmp->X, pmp->XF, pmp->XFA );
 //   if( pmp->PD==3 )
 //           GammaCalc( LINK_UX_MODE );
    if(pmp->PZ && pmp->W1)
      Mol_u( pmp->Y, pmp->X, pmp->XF, pmp->XFA );
    for( I=0; I<pmp->L; I++ )
       pmp->D[I]=pmp->Y[I]-pmp->X[I];
    if( pmp->PD==1 || pmp->PD == 2  /*|| pmp->PD == 3*/  )
        GammaCalc( LINK_UX_MODE);
    else ConCalc( pmp->X, pmp->XF, pmp->XFA );
    MassBalanceDeviations( pmp->N, pmp->L, pmp->A, pmp->X, pmp->B, pmp->C);
    //  if( wn[W_EQCALC].status )
    //    aSubMod[MD_EQCALC]->ModUpdate("PM_ipms   EqCalc() converged");
#ifndef IPMGEMPLUGIN
 #ifndef Use_mt_mode
    pVisor->Update( false );
 #endif
#endif
    return 0;
}

// Compression of demo phase list ????
//
// void TProfil::PhaseListPress()
// { int k, kd;
//  for( kd=0, k=0; k<pmp->FI; k++ )
//  {
//    if( pmp->YF[k] <= 1e-13 )
//      continue;
//    pmp->XFs[kd] = pmp->YF[k];
//    pmp->Falps[kd] = pmp->Falp[k];
//    memcpy( pmp->SFs[kd], pmp->SF[k], MAXPHNAME+MAXSYMB );
//    kd++;
//  }
//  if( kd < pmp->FI )
//    for( k=kd; k<pmp->FI; k++ )
//    {
//      pmp->XFs[k] = pmp->Falps[k] = FLOAT_EMPTY;
//      memset( pmp->SFs[k], ' ', MAXPHNAME+MAXSYMB );
//      pmp->SFs[k][0] = '`';
//    }
// }

// End of file ipm_sel_bc.cpp
//----------------------------------------------------------------
// Kostya: debug of calculation X(j) from Au

void TProfil::Mol_u( double Y[], double X[], double XF[], double XFA[] )
{
    int i,j,jj,ii,jb,je,k;
    int isp, ist;  double Ez, Psi;   // added  KD 23.11.01
//    double SPmol,SPmol1;
    double  Dsur, DsurT, MMC, *XU;
    XU = new double[pmp->L];

 //   ofstream ofs("c:/gems_b/x_u.txt",ios::out | ios::app);

  jb=0;
  for( k=0; k<pmp->FI; k++ )
  { /* cycle by phases */
      je=jb+pmp->L1[k];
      Dsur=0.0; DsurT=0.0;
      if( pmp->PHC[k] == PH_AQUEL && XF[k] > pmp->lowPosNum )
        Dsur = XFA[k]/XF[k] - 1.0; // Asymm.corr.
      if( (pmp->PHC[k] == PH_SORPTION || pmp->PHC[k] == PH_POLYEL)
            && XFA[k] > pmp->lowPosNum )
      {
         MMC = 0.0; // calculation of molar mass of the sorbent
         for( jj=jb; jj<je; jj++ )
         {
            if( pmp->DCC[jj] == DC_SUR_CARRIER ||
                pmp->DCC[jj] == DC_SUR_MINAL ||
                pmp->DCC[jj] == DC_PEL_CARRIER )
                    MMC += pmp->MM[jj]*X[jj]/XFA[k];
         }
         Dsur = XFA[k]/XF[k] - 1.0;
         DsurT = MMC * pmp->Aalp[k] * pa.p.DNS*1.66054e-6;
      }

    for(j=jb;j<je;j++)
    {
      if( XF[k] >= pmp->DSM ) // pmp->lowPosNum ) fixed by KD 23.11.01
      {
         XU[j] = -pmp->G0[j] -pmp->lnGam[j]
                 + DualChemPot( pmp->U, pmp->A+j*pmp->N, pmp->NR );
         if( pmp->PHC[k] == PH_AQUEL ) // pmp->LO && k==0)
         {
            if(j == pmp->LO)
                ;     //     disabled by KD 23.11.01
//                XU[j] += Dsur - 1. + 1. / ( 1.+ Dsur ) + log(XF[k]);
            else
                XU[j] += Dsur + log(XFA[k]);
         }
         else if( pmp->PHC[k] == PH_POLYEL || pmp->PHC[k] == PH_SORPTION )
         {
            if( pmp->DCC[j] == DC_PEL_CARRIER ||
                 pmp->DCC[j] == DC_SUR_CARRIER ||
                 pmp->DCC[j] == DC_SUR_MINAL )
                ;    //     disabled by KD 23.11.01
//                XU[j] += Dsur - 1.0 + 1.0 / ( 1.0 + Dsur )
//                      - DsurT + DsurT / ( 1.0 + DsurT ) + log(XF[k]);
            else  // rewritten by KD  23.11.01
               Psi = 0.0;
               Ez = pmp->EZ[j];
               /* Get ist - index of surface type */
               ist = pmp->SATNdx[j][0] / MSPN;
               /* and isp - index of surface plane  */
               isp = pmp->SATNdx[j][0] % MSPN;
               if( !isp )
                   /* This is A (0) plane */
                   Psi = pmp->XpsiA[k][ist];
               else /* This is B plane */
                  Psi = pmp->XpsiB[k][ist];
               XU[j] += Dsur + DsurT/( 1.0 + DsurT ) + log(XFA[k])+
               log( DsurT * pmp->Nfsp[k][ist] ) - pmp->FRT * Ez * Psi;
         }
         else
           XU[j] += log(XF[k]);

         if( XU[j] > -42. && XU[j] < 42. )
             XU[j] = exp( XU[j] );
         else
             XU[j] = /* F_EMPTY*/ 0.0;
         if( XU[j] <= pmp->lowPosNum )
             XU[j]=0.;
      }
      else
          XU[j]=0.;
    }
    jb = je;
  }  /* k */
 /*   if(pmp->LO&& k==0)
     for( j=jb; j<je; j++ )
    {
            if(j==pmp->LO)
                   continue;
            SPmol = X[j]*Factor;  // molality
            SPmol1 = XU[j]*Factor;

            for( ii=0; ii<pmp->N; ii++ )
            {
                pmp->IC_m[ii] += SPmol* a(j,ii);
                MU[ii] += SPmol1* a(j,ii);
            }
     }
  */

 /*   ofs<<"---------------------"<<endl;
     ofs<<pmp->stkey<<endl;
    char elemb[20];
   if(pmp->LO)
    for( ii=0; ii<pmp->N; ii++ )
    {
        strncpy(elemb, pmp->SB[ii], 4);
        elemb[4]=0;
        ofs << elemb << pmp->IC_m[ii];
        ofs << "\t  "<< MU[ii];
        if(pmp->IC_m[ii]>0) ofs<<" \t "<<log10(pmp->IC_m[ii]);
        if(MU[ii]>0)  ofs<<" \t "<< log10(MU[ii])<<endl;
    }
    ofs<<endl;
    for( j=0; j<pmp->L; j++ )
    {   strncpy(elemb, pmp->SM[j], 16);
        elemb[16]=0;
        ofs << elemb  <<" \t"<<X[j]<<" \t "<< XU[j]<<endl;
    }
   }    */


    for( j=0; j<pmp->L; j++ )
    { ii=0;
      if(pa.p.PLLG)
      { for( i=0; i<pmp->N-pmp->E; i++ )
        if(a(i,j) && pmp->B[i] < pmp->DHBM*pow(10.,pa.p.DT))
        { ii=1; break; }
      }
      else
        if(Y[j]<pmp->DHBM*pow(10.,pa.p.DT))
          ii=1;
      if (ii)
        X[j]=XU[j];
      else
        X[j]=Y[j];
    }
    TotalPhases( X, XF, XFA );

    delete[] XU;
//   ofs.close();
}

// End of file ipm_sel_bc.cpp
//-----------------------------------------------------------

