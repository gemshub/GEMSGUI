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

int TProfil::Gordan( int N, double /*DG*/, double *A, double *X )
{
    double AMAX,DIA;      /*  A[N][N+1]  */
    int I,J,K,N1,II,IR,*INV,iRet=0;

    INV = new int[N];
    ErrorIf( !INV, "Gordan", "Error of memory allocation" );
    for(I=0;I<N;I++)
     INV[I] = 0.;	
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
// B - work matrix of  N(N+1) dimensions
// Return values: 0  - solved OK;
//                1  - no solution, degenerated or inconsistent system;
#define  r(i,j) (*(R+(j)+(i)*N1))
//
int TProfil::SquareRoots( int N, double *R, double *X, double *B )
{

//   fstream f_log("SquareRoots.txt", ios::out|ios::app );

    int I,J,K,P,Q,N1, iRet=0;
    double F,G, E;
    double *gg;
    N1 = N + 1;
    ErrorIf( !B, "SquareRoots", "Error param B." );
    for(I=0;I<N*n1;I++)
      B[I]= 0.;

    Q=-1;
    for(I=0;I<N;I++)
    {
        for(J=I;J<N;J++)
            B[Q+J-I+1]= r(I,J);  /*       *(R+I*N1+J); */
        Q=Q+N-I;
    }

    for(I=0;I<N;I++)
        X[I]=  r(I,N);      /*       *(R+I*N1+N); */

#ifndef IPMGEMPLUGIN
    gg = new double[N];
#endif
    Q=0;

    for(I=0;I<N;I++)
    {
//f_log << I << endl;
        F=B[Q];
        P=I;
//f_log << "F = " << F << " P = " << P << endl;
        for(J=0;J<=I-1;J++)
        {
            G=B[P];
#ifndef IPMGEMPLUGIN
            gg[J] = G;
#else
            F-=G*G;
#endif
            for(K=1;K<N-I;K++)
                B[Q+K]-=G*B[P+K];
            P+=N-J-1;
//f_log << "G = " << G << " F = " << F << " P=" << P << endl;
        }
#ifndef IPMGEMPLUGIN
        E = enorm( J, gg );
        F = ( F - (E*E));
#endif
        if( F <=  pmp->lowPosNum /* 2.22E-16 DBL_EPSILON*/ )
        {
            iRet=1;
            goto KN;
        }

//f_log  << " F = (F-E*E) " << F << " E=" << E << endl;
       F=1./sqrt(F);

       B[Q]=F;
//f_log << " F = " << F << " E = " << E << endl;

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
//      pmp->XFs[k] = pmp->Falps[k] = DOUBLE_EMPTY;
//      mem_set( pmp->SFs[k], ' ', MAXPHNAME+MAXSYMB );
//      pmp->SFs[k][0] = '`';
//    }
// }

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

