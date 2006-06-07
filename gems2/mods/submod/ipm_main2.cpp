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
#include <math.h>
#include "jama_cholesky.h"
#include "jama_lu.h"
using namespace TNT;
using namespace JAMA;
#include "m_param.h"

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* Calculation of mass-balance deviations in IPM
*  Params: N - number of IC in IPM problem
*          L -   number of DC in IPM problem
*          A - DC stoichiometry matrix (LxN)
*          Y - moles  DC quantities in IPM solution (L)
*          B - Input bulk chem. compos. (N)
*          C - deviations (N)*/
void
TMulti::MassBalanceDeviations( int N, int L, float *A, double *Y, double *B, double *C )
{
    int I,J;
    for(J=0;J<N;J++)
    {
        C[J]=B[J];
        for(I=0;I<L;I++)
            C[J]-=(double)(*(A+I*N+J))*Y[I];
    }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Interior Points Method:
// subroutine for unconditional minimization of descent step length
// on the interval 0 to LM
// uses the "Golden Section" procedure
//
// Returns: optimal value of LM which provides largest monotonous
// decrease in G(X)
//
double TMulti::LMD( double LM )
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

//Added Sveta
//===================================================================

// Trace DC quantity into zeros
void TMulti::TraceDCzeros( int iStart, int iEnd, double sfactor, int JJ )
{
  SPP_SETTING *pa = &TProfil::pm->pa;

  for(int i=iStart; i<iEnd; i++ )
  {
    if( JJ >=0 )
      pmp->YF[JJ] = 0.;

     switch( pmp->DCC[i] )
     {
       case DC_AQ_PROTON:
       case DC_AQ_ELECTRON:
       case DC_AQ_SPECIES:
          if( JJ>=0 || pmp->Y[i] < pa->p.DFYaq * sfactor )
               pmp->Y[i] =  pa->p.DFYaq * sfactor;
           break;
       case DC_AQ_SOLVCOM:
       case DC_AQ_SOLVENT:
            if( JJ>=0 || pmp->Y[i] < pa->p.DFYw * sfactor )
                pmp->Y[i] =  pa->p.DFYw * sfactor;
            break;
       case DC_GAS_H2O:
       case DC_GAS_CO2:
       case DC_GAS_H2:
       case DC_GAS_N2:
       case DC_GAS_COMP:
       case DC_SOL_IDEAL:
            if( JJ>=0 || pmp->Y[i] < pa->p.DFYid*sfactor )
                  pmp->Y[i] = pa->p.DFYid * sfactor;
             break;
       case DC_SOL_MINOR:
            if( JJ>=0 || pmp->Y[i] < pa->p.DFYh*sfactor )
                   pmp->Y[i] = pa->p.DFYh * sfactor;
             break;
       case DC_SOL_MAJOR:
            if( JJ>=0 || pmp->Y[i] < pa->p.DFYr * sfactor )
                  pmp->Y[i] =  pa->p.DFYr * sfactor;
             break;
       case DC_SCP_CONDEN:
             if( JJ>=0 || pmp->Y[i] < pa->p.DFYc * sfactor )
                  pmp->Y[i] =  pa->p.DFYc * sfactor;
              break;
                    // implementation for adsorption?
       default:
             if( JJ>=0 || pmp->Y[i] < pa->p.DFYaq *sfactor )
                   pmp->Y[i] =  pa->p.DFYaq * sfactor;
             break;
     }
    if( JJ >=0 )
     pmp->YF[JJ] += pmp->Y[i];
   } // i
}

// Adjustment of prime approximation according to kinetic constraints
void TMulti::LagrangeMultiplier()
{
    double E = 1E-8;  // pa.p.DKIN? Default min value of Lagrange multiplier p
//    E = 1E-30;  // pa.p.DKIN? Default min value of Lagrange multiplier p

    for(int J=0;J<pmp->L;J++)
    {
        if( pmp->Y[J] < pmp->lowPosNum )
            continue;

        switch( pmp->RLC[J] )
        {
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
    }   // J
}

// Calculation of weight multipliers
void TMulti::WeightMultipliers( bool square )
{
  double  W1, W2;//, *W = pmp->W;
  memset( pmp->W, 0, pmp->L*sizeof(double));

  for(int J=0; J<pmp->L; J++)
  {
    switch( pmp->RLC[J] )
    {
      case NO_LIM:
      case LOWER_LIM:
           W1=(pmp->Y[J]-pmp->DLL[J]);
           if( square )
             pmp->W[J]= W1 * W1;
             else
             pmp->W[J] = max( W1, 0. );
           break;
      case UPPER_LIM:
           W1=(pmp->DUL[J]-pmp->Y[J]);
           if( square )
             pmp->W[J]= W1 * W1;
           else
             pmp->W[J] = max( W1, 0.);
           break;
      case BOTH_LIM:
           W1=(pmp->Y[J]-pmp->DLL[J]);
           W2=(pmp->DUL[J]-pmp->Y[J]);
           if( square )
           {
             W1 = W1*W1;
             W2 = W2*W2;
           }
           pmp->W[J]=( W1 < W2 ) ? W1 : W2 ;
           if( !square && pmp->W[J] < 0. ) pmp->W[J]=0.;
           break;
      default: // big error
            Error( "E04IPM IPM-Iteration:",
                 "Error in codes of metastability constraints" );
    }
  } // J
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Make and Solve a system of linear equations using method of
// Cholesky Decomposition (  if a square matrix R happens to
// be symmetric and positive defined)
// If Cholesky Decomposition has no solution,
// Solve of a system of linear set of equations using method of
// LU Decomposition (   A = L*U , L - lower triangular ( has elements only
// on the diagonal and below )  U - is upper triangular ( has elements only
// on the diagonal and above))
// Parameters:
// bool initAppr - Inital approximation point(true) or iteration of IPM (false)
// int N - dimension of the matrix R (number of equations)
// pmp->U - solution vector (N).
// Return values: 0  - solved OK;
//                1  - no solution, degenerated or inconsistent system;
#define  a(j,i) ((double)(*(pmp->A+(i)+(j)*N)))
//
int TMulti::SolverLinearEquations( int N, bool initAppr )
{
  int ii, jj, kk;
  double aa;
  Array2D<double> A(N,N);
  Array1D<double> B(N);

  // making  matrix of IPM linear equations
  for( kk=0; kk<N; kk++)
    for( ii=0; ii<N; ii++ )
    {  aa = 0.;
       for( jj=0; jj<pmp->L; jj++ )
          if( pmp->Y[jj] > pmp->lowPosNum /* 1E-19 */ )
            aa += a(jj,ii) * a(jj,kk) * pmp->W[jj];
      A[kk][ii] = aa;
    }

 for( ii=0; ii<N; ii++ )
   if( initAppr )
       B[ii] = pmp->C[ii];
   else
      {  aa = 0.;
          for( jj=0; jj<pmp->L; jj++)
           if( pmp->Y[jj] > pmp->lowPosNum /* 1E-19 */ )
                aa += pmp->F[jj] * a(jj,ii) * pmp->W[jj];
          B[ii] = aa;
      }

// this routine constructs its Cholesky decomposition, A = L · LT .
  Cholesky<double>  chol(A);

  if( chol.is_spd() )  // is positive definite A.
  {
    B = chol.solve( B );
  }
  else
  {
// no solution by Cholesky decomposition Try LU Decompositon
// The LU decompostion with pivoting always exists, even if the matrix is
// singular, so the constructor will never fail.

   LU<double>  lu(A);

// The primary use of the LU decomposition is in the solution
// of square systems of simultaneous linear equations.
// This will fail if isNonsingular() returns false.
   if( !lu.isNonsingular() )
     return 1; // Singular matrix

  B = lu.solve( B );
  }

  for( ii=0; ii<N; ii++ )
   pmp->U[ii] = B[ii];
  return 0;
}
#undef a

// calc of MU values and Dikin criterion
// Parameters:
// bool initAppr - Inital approximation point(true) or iteration of IPM (false)
// int N - dimension of the matrix R (number of equations)
double TMulti::calcDikin(  int N, bool initAppr )
{
  int  J;
  double Mu, PCI=0.;

  for(J=0;J<pmp->L;J++)
  {
    if( pmp->Y[J] > pmp->lowPosNum /*1E-19 */)
    {
      Mu = DualChemPot( pmp->U, pmp->A+J*pmp->N, N );
      if( !initAppr )
        Mu -= pmp->F[J];
      PCI += Mu*pmp->W[J]*Mu;
      pmp->MU[J] = Mu*pmp->W[J];
    }
    else
      pmp->MU[J]=0.; // initializing dual potentials
  }
  if( initAppr )
  {
     if( PCI > pmp->lowPosNum /* 1E-19*/ )
          PCI=1/sqrt(PCI);
     else PCI=1.; // zero Psi value ?
  }

  return PCI;
}

// Calculation of descent step size LM
// Parameters:
// bool initAppr - Inital approximation point(true) or iteration of IPM (false)
double TMulti::calcLM(  bool initAppr )
{
   int J;
   int Z = -1;
   double LM, LM1, Mu;

   for(J=0;J<pmp->L;J++)
   {
     Mu = pmp->MU[J];
    if( pmp->RLC[J]==NO_LIM ||
        pmp->RLC[J]==LOWER_LIM || pmp->RLC[J]==BOTH_LIM )
    {
       if( Mu < 0 && fabs(Mu)>pmp->lowPosNum*100.)  //1E-16)
       {
         if( Z == -1 )
         { Z = J;
           LM = (-1)*(pmp->Y[Z]-pmp->DLL[Z])/Mu;
         }
         else
         {
           LM1 = (-1)*(pmp->Y[J]-pmp->DLL[J])/Mu;
           if( LM > LM1)
             LM = LM1;
         }
       }
    }
    if( pmp->RLC[J]==UPPER_LIM || pmp->RLC[J]==BOTH_LIM )
    {
       if( Mu > pmp->lowPosNum*100.)  //1E-16)
       {
         if( Z == -1 )
         { Z = J;
           LM = (pmp->DUL[Z]-pmp->Y[Z])/Mu;
         }
         else
         {
           LM1=(pmp->DUL[J]-pmp->Y[J])/Mu;
           if( LM>LM1)
               LM=LM1;
         }
      }
    }
   }

  if( initAppr )
  { if( Z == -1 )
     LM = pmp->PCI;
    else
     LM *= .95;     // Smoothing of final lambda ????
  }
  else
  {  if( Z == -1 )
       LM = 1./sqrt(pmp->PCI);
  }
  return LM;
}

// Restoring vectors Y and YF
void TMulti::Restoring_Y_YF()
{
 int Z, I, JJ = 0;

 for( Z=0; Z<pmp->FI ; Z++ )
 {
   if( pmp->XF[Z] <= pmp->DSM ||
       pmp->PHC[Z] == PH_SORPTION &&
       ( pmp->XFA[Z] < TProfil::pm->pa.p.ScMin) )
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
   else
   {
     pmp->YF[Z] = pmp->XF[Z];
     if( pmp->FIs && Z < pmp->FIs )
        pmp->YFA[Z] = pmp->XFA[Z];
     for(I = JJ; I < JJ+pmp->L1[Z]; I++)
        pmp->Y[I]=pmp->X[I];
   }
   JJ += pmp->L1[Z];
 } // Z

}

// Calculation of sfactor
double TMulti::calcSfactor()
{
    double molB=0.0, sfactor;
    int i, NN = pmp->N - pmp->E;

    for(i=0;i<NN;i++)
      molB += pmp->B[i];

   sfactor = pow( molB, 0.4 )/7.7;
   return sfactor;
}

//===================================================================

// Checks of Karpov phase stability criteria,
// Selekt2() procedure by Karpov & Chudnenko
// modified by DAK in 1995
// Returns 0, if new IPM loop is needed;
//         1, if the solution is final.
//
//#define  a(j,i) ((double *)(*(pm[q].A+(i)+(j)*pm[q].N)))
//
void TMulti::PhaseSelect()
{
    short /*II,*/JJ,Z,I,J;//, iRet=0;
    double F1,F2/*,F3=0.0*/,*F0;
    SPP_SETTING *pa = &TProfil::pm->pa;


    f_alpha( );
    F0 = pmp->Falp;

    (pmp->K2)++;
    pmp->MK=0;
    JJ= -1;
 //   II= -1;
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
           // II=Z;
        }
    }
    if( F1 > pa->p.DF && JJ >= 0 )
    {
        double sfactor;
        // There is a phase for which DF (0.01) is exceeded
        sfactor = calcSfactor();
        do
        {  // insert all phases with  F1 > DFM
            // insert this phase and set Y[j] for its components
            // with account for asymmetry and non-ideality
            for( J=0, I=0; I<JJ/*-1*/; I++ )   // A bug ?
                 J+=pmp->L1[I];

            TraceDCzeros( J, J+pmp->L1[JJ], sfactor, JJ );

            pmp->FI1++;  // check phase rule
            if( pmp->FI1 >= pmp->NR+1 )
            { // No more phase can be inserted
                break;
            }
            // find a new phase to insert, if any
            F1= pmp->lowPosNum*10000.; // 1e-16
            JJ = -1;
            for( Z=0; Z<pmp->FI; Z++ )
                if( F0[Z] > F1 && pmp->YF[Z] < pmp->lowPosNum )
                {
                    F1=F0[Z];
                    JJ=Z;
                }
        }
        while( F1 > pa->p.DF && JJ >= 0/* 1E-15 */ );
        // end of insertion cycle
        J=0;   /*  insert prime IPM solution */
        for(Z=0;Z<pmp->FIs;Z++)
        {
            if( pmp->YF[Z ] > pmp->lowPosNum /* 1E-18 */ )
            {
                pmp->YF[Z]=0.;
                for(I=J;I<J+pmp->L1[Z];I++)
                {
                   if( pmp->Y[I] < pmp->lowPosNum ) // Check what to insert !
                       pmp->Y[I] = pa->p.DFM; // lowPosNum?
                    pmp->YF[Z]+=pmp->Y[I]; // calc new  quantities of phases
                }
            }
            J+=pmp->L1[Z];
        }
#ifndef IPMGEMPLUGIN
#ifndef Use_mt_mode
        pVisor->Update(false);  // "PhaseSelection"
#endif
#endif
        if( pmp->K2>1 )
        { // more then first step - solution is not improved
            for(I=0;I<pmp->L;I++)
                if( fabs(pmp->Y[I]- pmp->XY[I]) > pa->p.DF*pmp->Y[I] ) // Check!
                    goto S6;
            pmp->PZ=2;
            goto S4;
        }
S6: // copy X changed by SELEKT2 algorithm
        for(I=0;I<pmp->L;I++)
            pmp->XY[I]=pmp->Y[I];
// Copy here also to pmp->X[I]?
        return;
    }
S4: // No phases to insert or no distortions
    // end of iterations of SELEKT2
    pmp->MK=1;
}

// Generic simplex method with two sided constraints
// (c) K.Chudnenko 1992
//  SPOS function
// P -
// STR -
// NMB -
// J -
// M -
// AA -
//
void TMulti::SPOS( double *P, int STR[],int NMB[],int J,int M,double AA[])
{
    int I,K;
    K=0;
    for(I=0; I<=M; I++)
    {
        if( I==STR[NMB[J]+K-1])
        {
            *(P+I)=AA[NMB[J]+K-1];
            if( NMB[J]+K+1!=NMB[J+1])
                K++;
        }
        else *(P+I)=0.;
    }
}

// Generic simplex method with two sided constraints
// (c) K.Chudnenko 1992
//  START function
// T -
// ITER -
// M
// N
// STR -
// NMB -
// J -
// M -
// AA -
// ......
//
void TMulti::START( int T,int *ITER,int M,int N,int NMB[],
                     double GZ,double EPS,int STR[],int *BASE,
                     double B[],double UND[],double UP[],double AA[],double *A,
                     double *Q )
{
    int I,J;

    for( I=0;I<M;I++)
        UP[N+I]=0.;
    for( J=0;J<N;J++)
    {
        if(fabs(UP[J])<EPS)
            UP[J]=0.;
        else
        {
            UP[J]-=UND[J];
            if( fabs(UP[J])<EPS)
                UP[J]=EPS;
            else if( UP[J]<0.)
                Error("E00IPM: Simplex", "Negative UP[J] value(s) in START() ");
        }
        SPOS(Q, STR, NMB, J, M, AA);
        for( I=0;I<M;I++)
            B[I]-=Q[I+1]*UND[J];
    }
    for( I=0;I<M;I++)
    {
        if( B[I]<0.)
        {
            B[I]=fabs(B[I]);
            for( J=0;J<T;J++)
                if(STR[J]==I)
                    AA[J]=-AA[J];
        }
    }
    *A=0.;
    *ITER=0;
    for( I=0;I<M;I++)
    {
        *A-=GZ*B[I];
        *(A+(I+1)*(M+1))=B[I];
        for( J=0;J<M;J++)
            *(A+(I+1)*(M+1)+J+1)=0.;
        *(A+(I+1)*(M+1)+I+1)=1.;
        BASE[I]=N+I;
        *(A+I+1)=-GZ;
    }
}

// Generic simplex method with two sided constraints
// (c) K.Chudnenko 1992
//  NEW function   parameters
//  OPT -
// .....
//
void TMulti::NEW(int *OPT,int N,int M,double EPS,double *LEVEL,int *J0,
                  int *Z,int STR[], int NMB[], double UP[],
                  double AA[], double *A)
{
    int I,J,J1;
    double MAX,A1;
    double *P;
    P= new double[M+1];
    ErrorIf( !P, "Simplex", "At NEW memory alloc error. ");
    memset(P, 0, sizeof(double)*(M+1) );
    J1=*J0;
    MAX=0.;
    for( J=J1+1;J<=N;J++)
    {
        SPOS( P, STR, NMB, J-1, M, AA);
        A1=-P[0];
        for( I=1;I<=M;I++)
            A1+=P[I]*(*(A+I));
        if(fabs(A1)>MAX)
        {
            if(UP[J-1]>=-EPS && A1<-EPS)
            {
                *Z=1;
                goto MK3;
            }
            else if(UP[J-1]<-EPS && A1>EPS)
            {
                *Z=0;
                goto MK3;
            }
            else continue;
MK3:
            MAX=fabs(A1);
            *J0=J;
            if( MAX>=*LEVEL)
                goto MK4;
        }
    }
    for( J=1;J<J1;J++)
    {
        SPOS(P, STR, NMB, J-1, M, AA);
        A1=-P[0];
        for( I=1;I<=M;I++)
            A1+=P[I]*(*(A+I));
        if(fabs(A1)>MAX)
        {
            if(UP[J-1]>=-EPS && A1<-EPS)
            {
                *Z=1;
                goto MK3A;
            }
            else if(UP[J-1]<-EPS && A1>EPS)
            {
                *Z=0;
                goto MK3A;
            }
            else continue;
MK3A:
            MAX=fabs(A1);
            *J0=J;
            if( MAX>=*LEVEL)
                goto MK4;
        }
    }
    *LEVEL=MAX/2;
    if( *LEVEL<EPS)
        *LEVEL=EPS;
MK4:
    if( MAX<EPS)
        *OPT=1;
    else *OPT=0;
    delete[] P;
}


// Generic simplex method with two sided constraints
// (c) K.Chudnenko 1992
//  WORK function
// GZ -
//.....
//
void TMulti::WORK(double GZ,double EPS,int *I0, int *J0,int *Z,int *ITER,
                   int M, int STR[],int NMB[],double AA[],
                   int BASE[],int *UNO,double UP[],double *A,double Q[])
{
    double MIM,A1;
    int UN,J,I;
    double *P;
    P=  new double[M+1];
    ErrorIf( !P, "Simplex", "At WORK memory alloc error. ");
    memset(P, 0, sizeof(double)*(M+1) );
    *UNO=0;
    *ITER=*ITER+1;
    J=*J0-1;
    SPOS(P, STR, NMB, J, M, AA);
    for( I=0;I<=M;I++)
    {
        Q[I]=0.;
        for( J=1;J<=M;J++)
            Q[I]+=*(A+I*(M+1)+J)*P[J];
    }
    Q[0]-=P[0];
    UN=1;
    MIM=0.;
    for( I=1;I<=M;I++)
    {
        if(*Z==1)
            A1=GZ;
        else A1=-GZ;
        if( (*Z==1 && Q[I]>EPS)||(*Z==0 && Q[I]<-EPS))
        {
            UN=0;
            A1=*(A+I*(M+1))/Q[I];
        }
        else if((*Z==1 && Q[I]<-EPS)||(*Z==0 &&Q[I]>EPS))
        {
            J=BASE[I-1];
            if(fabs(UP[J])>EPS)
            {
                UN=0;
                A1=((*(A+I*(M+1)))-fabs(UP[J]))/Q[I];
            }
        }
        if(I==1||((*Z==1 && A1<MIM)||(*Z==0 && A1>MIM)))
        {
            MIM=A1;
            *I0=I;
        }
    }
    if( UN==1 && fabs(UP[*J0-1])<EPS)
    {
        *UNO=1;
        delete[] P;
        return;
    }
    if(fabs(MIM)<fabs(UP[*J0-1])||fabs(UP[*J0-1])<EPS)
    {
        J=BASE[*I0-1];
        if((*Z==1&&Q[*I0]>0.)||(*Z==0&&Q[*I0]<0.))
            UP[J]=fabs(UP[J]);
        else UP[J]=-fabs(UP[J]);
        if(*Z==1)
            *(A+(*I0)*(M+1))=MIM;
        else *(A+(*I0)*(M+1))=MIM+fabs(UP[*J0-1]);

        for( I=0;I<=M;I++)
            if( I!=*I0)
                *(A+I*(M+1))-=Q[I]*MIM;
        BASE[*I0-1]=*J0-1;
        A1=1.E0/Q[*I0];
        for( J=1;J<=M;J++)
            *(A+(*I0)*(M+1)+J)*=A1;
        for( I=0;I<*I0;I++)
            for( J=1;J<=M;J++)
                *(A+I*(M+1)+J)-= Q[I]*(*(A+(*I0)*(M+1)+J));
        for( I=*I0+1;I<=M;I++)
            for( J=1;J<=M;J++)
                *(A+I*(M+1)+J)-=Q[I]*(*(A+(*I0)*(M+1)+J));
    }
    else
    {
        for( I=0;I<=M;I++)
        {
            *(A+I*(M+1))-=UP[*J0-1]*Q[I];
            UP[*J0-1]=-UP[*J0-1];
        }
    }
    delete[] P;
}

// Generic simplex method with two sided constraints
// (c) K.Chudnenko 1992
//  FIN function
//
//  EPS -
// ....
//
void TMulti::FIN(double EPS,int M,int N,int STR[],int NMB[],
                  int BASE[],double UND[],double UP[],double U[],
                  double AA[],double *A,double Q[],int */*ITER*/)
{
    int /* K,*/I,J;
    double *P;
    P=  new double[M+1];
    ErrorIf( !P, "Simplex", "At FIN memory alloc error. ");
    memset(P, 0, sizeof(double)*(M+1) );
    for( J=0;J<N;J++)
    {
        if( UP[J]>-EPS)
            UP[J]=0.;
        else UP[J]=fabs(UP[J]);
    }
    for( I=1;I<=M;I++)
    {
        UP[BASE[I-1]]=*(A+I*(M+1));
        Q[I]=0.;
    }
    Q[0]=0.;
    for( J=1;J<=N;J++)
    {
        SPOS( P, STR, NMB, J-1, M, AA);
        UP[J-1]+=UND[J-1];
        for( I=0;I<=M;I++)
            Q[I]+=UP[J-1]*P[I];
    }
    for( I=1;I<=M;I++)
        U[I-1]=-*(A+I);
    delete[] P;
}

// Generic simplex method with two sided constraints
// (c) K.Chudnenko 1992
//  Main function
//
//  M  - number of independent components
//  N  - number of unknowns
//  T  - dimension of a work vector AA[] containing all non-zero
//        values of vector GT[] and A[][] matrix (over lines)
//  GZ - Limiting value of the unknown
//  EPS - precision (convergence) criterion (default 1e-9)
//  UND - vector of lower constraints to unknowns
//  UP - input vector of upper constraints to unknowns;
//        output vector of unknowns (simplex solution) (N+M)
//  B -  M input values of independent components (bulk composition)
//  U -  output vector of the dual solution (M)
//  AA - work array (T)
//  STR - markup vector of values in AA array (T)
//  NMB - indices of values in AA
// returns 0 if OK;
//         1 if inconsistent input constraints;
//        -1 if memory allocation error;
//
void TMulti::Simplex(int M, int N, int T, double GZ, double EPS,
                      double *UND, double *UP, double *B, double *U,
                      double *AA, int *STR, int *NMB )
{
    int IT=200,I0=0,J0=0,Z,UNO,OPT=0,ITER, i;
    double LEVEL;
    int *BASE=0;
    double *A=0,*Q=0;

    try
    {
        A=  new double[(M+1)*(M+1)];   /* A[pm->N][pm->N],Q[pm->N]; */
        Q=  new double[M+1];
        BASE=  new int[M];          /* BASE[pm->N-1]  */
        ErrorIf( !A || !Q || !BASE, "Simplex", "Memory alloc error ");
        memset(A, 0, sizeof(double)*(M+1)*(M+1) );
        memset(Q, 0, sizeof(double)*(M+1) );
        memset(BASE, 0, sizeof(int)*(M) );

        LEVEL=GZ;
        START( T, &ITER, M, N, NMB, GZ, EPS, STR, BASE, B,  UND, UP, AA, A, Q );

        for( i=0; i<IT; i++ )   // while(1) fixed  03.11.00
        {
            NEW( &OPT, N, M,EPS, &LEVEL, &J0, &Z, STR, NMB, UP, AA, A);
            if( OPT)
                goto FINISH;  // Converged
            WORK( GZ, EPS, &I0, &J0, &Z, &ITER, M, STR, NMB, AA, BASE, &UNO, UP, A, Q);
            if( UNO)
                goto FINISH; // Solution at boundary of the constraints polyhedron
        }
        if( EPS > 1.0e-6 )
        {
         Error( "E01IPM: Simplex",
             "LP solution cannot be obtained with sufficient precision" );
        }
FINISH: FIN( EPS, M, N, STR, NMB, BASE, UND, UP, U, AA, A, Q, &ITER);
        delete[] A;
        delete[] Q;
        delete[] BASE;
    }
    catch( TError& xcpt )
    {
        if( A) delete[]A;
        if( Q) delete[]Q;
        if( BASE) delete[]BASE;
        Error( xcpt.title.c_str(), xcpt.mess.c_str());
    }
    // Done
}

//--------------------- End of ipm_main2.cpp ---------------------------
