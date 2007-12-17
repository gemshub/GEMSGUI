//-------------------------------------------------------------------
// $Id: m_integ.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of TInteg class, config and calculation functions
//
// Rewritten from C to C++ by S.Dmytriyeva  
// Copyright (C) 1995-2001 S.Dmytriyeva
//
// This file is part of a GEM-Selektor library for thermodynamic
// modelling by Gibbs energy minimization
// Uses: GEM-Vizor GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//
#include <math.h>
#include <stdio.h>

#ifndef IPMGEMPLUGIN

#include "m_gem2mt.h"
#include "nodearray.h"
#include "service.h"
#include "visor.h"

#else

#include <time.h>
#include "ms_gem2mt.h"
#include "nodearray.h"

#endif

#define dMB( q, i)  (dm[ (q)*mtp->Nb + (i)]) 

#define MB( q, i)  ( m[ (q)*mtp->Nb + (i)])

#define g(q,f,i)   ( mtp->gc[ (q)*(mtp->nC*mtp->Nb)+ (f)*mtp->Nb + (i)])

#define v(f)       ( (mtp->FDLf[f][1]) )

#define H(p, i)      (mtp->BSF[(abs(p)-1)* mtp->Nb + ( i )])

// calculate 1-step from system of equation 
void TGEM2MT::Solut( double *m, double *dm, double t )
{ 
  int kk, q, p, f, i;
  
  // set up 0.
  for( q=0; q <mtp->nC; q++ )
	for(i =0; i< mtp->Nb; i++ )
      dMB(q,i) = 0.;

  for(kk=0; kk<mtp->nFD; kk++ )
  {
    q = mtp->FDLi[kk][0];
	p = mtp->FDLi[kk][1];
	if( q >= 0 && p>=0 )
	{
	  for(f=0; f<mtp->nPG; f++ )
		for(i=0; i<mtp->Nb; i++ )
	    {	  
		  dMB(q,i) -=  v(f) * MB(q,i) * g(q,f,i);
		  dMB(p,i) +=  v(f) * MB(q,i) * g(q,f,i);
	   }	
	} else
		 if( q>= 0 )
		 {
		   for(f=0; f<mtp->nPG; f++ )
		     for(i=0; i<mtp->Nb; i++ )
			 {	  
				  dMB(q,i) -=  v(f) * MB(q,i) * g(q,f,i);
			 }	
		  } else
				 if( p>= 0 )
				 {
				   for(f=0; f<mtp->nPG; f++ )
				     for(i=0; i<mtp->Nb; i++ )
					 {	  
						  dMB(p,i) +=  H(q,i)*  v(f)  * g(p,f,i);
					 }	
				  }
	       
  }
 
}

#undef dMB
#undef MB

#define Mb( q, i)  ( mtp->MB[ (q)*mtp->Nb + (i)])

#define dMb( q, i)  (mtp->dMB[(q)*mtp->Nb + (i)])


// Calculate new reservuir states for tcur = x
void
TGEM2MT::CalcNewStates( int Ni,int pr, double tcur, double step, double *y )
{
  int q, i, f;
  double Mqfi = 0., Bqi = 0., Wqi = 0.;
  bool iRet = false;
  FILE* diffile = NULL;

 if( mtp->PvMO != S_OFF )
 {
   // Preparations: opening output files for monitoring 1D profiles
    diffile = fopen( "ICdif-log.dat", "w+" );   //  Element amount diffs for t and t-1
    if( !diffile)
    return;
 }
 clock_t t_start, t_end, t_out, t_out2;
 clock_t outp_time = (clock_t)0;
 t_start = clock();

// Set up new reservoir states at tcur
 for( q=0; q <mtp->nC; q++ )
	for(i =0; i< mtp->Nb; i++ )
	{
		node1_bIC(q, i) +=dMb( q, i);
	}	
 
// The analysis of GEM IA modes in nodes - optional
  int NodesSetToAIA = CheckPIAinNodes1D( NEED_GEM_AIA, 0, mtp->nC );

	
// Calculate new reservoir states at tcur	
// Calculation of chemical equilibria in all nodes at the beginning
// with the Simplex initial approximation
   CalcIPM( NEED_GEM_AIA, 0, mtp->nC, diffile );
	
 
  // Here one has to compare old and new equilibrium phase assemblage
  // and pH/pe in all nodes and decide if the time step was Ok or it
  // should be decreased. If so then the nodes from C0 should be
  // copied to C1 (to be implemented)

  // Output resalt if step accepted
   if( mtp->PvMO != S_OFF )
   {
    t_out = clock();
    na->logDiffsIC( diffile, mtp->ct, mtp->cTau/(365*86400), mtp->nC, 0 );
        // logging differences after the MT iteration loop
    t_out2 = clock();
    outp_time += ( t_out2 -  t_out);
   }

   
  #ifndef IPMGEMPLUGIN
             // time step accepted - Copying nodes from C1 to C0 row
             pVisor->Update();
             CalcGraph();
  #endif
  
  // copy node array for T0 into node array for T1
     copyNodeArrays();

  // Set parameters for mass transport
     for( q=0; q <mtp->nC; q++ )
		 for(i=0; i<mtp->Nb; i++ )
			 Mb( q, i) = 0.;

     for( q=0; q <mtp->nC; q++ )
	   for(f=0; f<mtp->nPG; f++ )
		 for(i=0; i<mtp->Nb; i++ )
	     {	  
			 // Mqfi = ???;
			 // Bqi =  ???;
			 // Wqi =  ???;
			 g(q,f,i) =  Mqfi/Bqi/Wqi;
			 Mb( q, i) += Mqfi;
	     }
     
}

#undef dMB
#undef MB

//Calculate record
bool TGEM2MT::CalcBoxModel( char mode )
{
    int nStart = 0, nEnd = mtp->nC;
    bool iRet = false;
    FILE* diffile = NULL;

    if( mtp->PvMO != S_OFF )
    {
    // Preparations: opening output files for monitoring 1D profiles
    diffile = fopen( "ICdif-log.dat", "w+" );   //  Element amount diffs for t and t-1
    if( !diffile)
      return iRet;
    }
    // Init part ?????  
    mtp->dx = mtp->cLen/mtp->nC;
    mtp->dTau = 0.5*(mtp->dx/mtp->fVel)*1/mtp->tf;
    mtp->oTau = 0;
    mtp->cTau = mtp->Tau[START_];
    // mtp->cTau = 0;
    mtp->ct = 0;

 // calculate inital states
 // Calculation of chemical equilibria in all nodes at the beginning
 // with the Simplex initial approximation
    CalcIPM( NEED_GEM_AIA, nStart, nEnd, diffile );
   
 // Set initial parameter values
 //   GetMBfromNodes(); 
    
#ifndef IPMGEMPLUGIN
      if(  mtp->PvMSg != S_OFF && vfQuestion(window(),
             GetName(), "Use graphic monitoring?") )
        {
            RecordPlot( 0 );
        }
#endif
     
  // calc part  
    int n = mtp->nC * mtp->Nb;  
    tt = new double[ n ][9];
    nfcn = nstep = naccept = nrejct = 0;
    INTEG( mtp->cdv, mtp->dTau, mtp->Tau[START_], mtp->Tau[STOP_] );
    delete[] tt;
    
    return iRet;
}

/*set default information
void TInteg::set_def(int)
{
    name[0][0] = '\0';
    name[1][0] = '\0';
    Nequ = 1;
    x_bg =0.;
    x_end = 1.;
    step_bg = 1.;
    Eps = 1e-10;
    step =0.;
    nfcn = nstep = naccept = nrejct = 0;
    arg_x = 0.;
    Bsize = 0;
    MaxIter = MAXITER;

    y_bg = 0;
    param = 0;
    TxtEqu = 0;
    val_y = 0;
    valdy = 0;
    allx = 0;
    ally = 0;
    allpr = 0;
    allst = 0;
    dyn_new();
}

*/


//--------------------------------------------------------------------
// Integration process

const int NMAX = 800;
const int KM = 8;
const double UROUND = 1.73e-18;
const double FAC1 = 2.e-2;
const double FAC2 = 4.0;
const double FAC3 = .9;
const double FAC4 = .8;
const double SAFE1 = .65;
const double SAFE2 = .94;
const double MAXSTEP = 1.7;
//const int MAXINTEGEXPR = 200;

//double *x;
//double *dx;
//double *tv;

//static double tt[9][ MAXINTEGEXPR ]; change 14/12/2007 [ MAXINTEGEXPR ][9]
static double hh[9];
static double w[9];
static double err;
static double epsd4;
static int    nj[9]={2,4,6,8,10,12,14,16,18};
static double a1[9]={3.0,7.0,13.0,21.0,31.0,43.0,57.0,73.0,91.0};


// internal point j calculation
void TGEM2MT::MIDEX( int j, double t, double h )
{
    double *z1=0, *z2=0, *dz=0;
    double hj, scal, fac, facmin, expo, ys, v1, v2;
    int i,m,mm,l,n;
    try
    {
        n = mtp->nC*mtp->Nb;
        z1 = new double[n];
        z2 = new double[n];
        dz = new double[n];
        memset( z1, 0, sizeof(double)*n );
        memset( z2, 0, sizeof(double)*n );
        memset( dz, 0, sizeof(double)*n );

        hj = h / (double)nj[ j ];
        // 
        for( i=0; i < n; i++ )
        {
            z1[ i ] = x[ i ];
            z2[ i ] = x[ i ] + hj * dx[ i ];
        }
        // 
        m = nj[ j ] - 1;
        for( mm=0; mm < m; mm++ )
        {
            Solut(  z2, dz, t+hj*(double)(mm+1) );
            for( i=0; i < n; i++ )
            {
                ys = z1[ i ];
                z1[ i ] = z2[ i ];
                z2[ i ] = ys + hj * dz[ i ] * 2.;
            }
        }
        // 
        Solut(  z2, dz, t+h );
        for( i=0; i < n; i++ )
            tt[ i ][ j ] = ( z1[ i ] + z2[ i ] + hj * dz[ i ] ) / 2.;
        nfcn += nj[ j ];
        //
        if(j == 0)
            goto VEL;
        for( l=j; l >= 1; l-- )
        {
            fac = pow( (double)nj[ j ] / (double)nj[ l-1 ], 2.) - 1.;
            for( i=0; i < n; i++ )
                tt[ i ][ l-1 ] = tt[ i ][ l ] +
                                 ( tt[ i ][ l ] - tt[ i ][ l-1 ] ) / fac;
        }
        err = 0e0;
        for( i=0; i < n; i++ )
        { // 
            v2 = fabs( tt[ i ][ 0 ] );
            v1 = fabs( x[ i ] );
            v1 = max( v1, v2 );
            v2 = max( 1.e-6, UROUND / epsd4 );
            scal = max( v1, v2 );
            err += pow( (tt[ i ][ 0 ] - tt[ i ][ 1 ] ) / scal, 2. );
        }
        err = pow( err / (double)n, .5 );
        //
        expo = 1.e0 / (double)( 2 * ( j + 1 )-1 );
        facmin = pow( FAC1, expo );
        v1 = max( facmin, pow( err/epsd4, expo ) / SAFE2 );
        fac = min( FAC2/facmin, v1 );
        fac= 1.e0 / fac;
        hh[ j ] = min( h * fac, MAXSTEP );
        w[ j ] = a1[ j ] / hh[ j ];

VEL:
        delete[] z1;
        delete[] z2;
        delete[] dz;
    }
    catch( ... )
    {
        if( z1 ) delete[] z1;
        if( z2 ) delete[] z2;
        if( dz ) delete[] dz;
        Error( "INTEG ", "Error in MIXED!");
    }
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  ������� INTEG ������ ������ ���� ����
//		 dyi/dx = f( y1,y2, ... ,yn,x )  i=1,...,n
//               yi( x0 )=yi0
//  ������� ������������� � ��������� �������� � ����������
//  ������ ����
void
TGEM2MT::INTEG( double eps, double& step, double t_begin, double t_end )
{
    double  t, h1, h, v1;
    int     j,i,reject,last,kc,kopt,k;

    x = mtp->MB;
    dx = mtp->dMB;
    tv = &mtp->cTau;

    h = step;
    epsd4 = eps * SAFE1;
    v1 = -log10(eps)*.6+1.5;
    v1 = min( 8.0, v1 );
    k = (int)max( 3.0, v1 ) - 1;
    t = t_begin;
    h1 = t_end-t;
    v1 = min( MAXSTEP, h1/2. );
    h = min( h, v1 );
    CalcNewStates( 0, k, t, h, x ); // 14/12/2007 ????? may be did before in calc
    err = w[ 0 ] = 0.0;
    reject = last = 0;   /*false*/

    //
    while( fabs( h1 ) >= UROUND )
    {
        v1 = min( h1, MAXSTEP);
        h = min( h, v1 );
        if( h >= ( h1 - UROUND ) )  last = 1;      /*true*/
        Solut(  x, dx, t );
        nfcn++;
        if (( nstep == 0 )||( last ))     //1
        {
            nstep++;
            for( j=0; j <= k; j++ )
            {
                kc=j;
                MIDEX( j, t, h );
                if( ( j > 0 ) && ( err <= eps ) ) goto l60;
            }
            goto l55;
        }
        //
l30:
        nstep++;
        ErrorIf( nstep >= MaxIter, "INTEG", "Excess iteration number" ); // 14/12/2007 !!!!
        kc = k-1;
        for( j=0; j <= kc; j++ )
            MIDEX( j, t, h);
        // 
        if( !( k == 1 || reject ) )
        {
            if( err <= eps ) goto l60;
            if( ( err/eps ) >
                    ( pow( (double)(nj[k+1]*nj[k])/4., 2. ) ) )  goto l100;
        }
        MIDEX( k, t, h );
        kc = k;
        if( err <= eps ) goto l60;
        // 
l55:
        if( err/eps > pow( (double)(nj[k])/2.0, 2.0 ) ) goto l100;
        kc = k + 1;
        MIDEX( kc, t, h );
        if( err > eps ) goto l100;
        // 
l60:
        t += h;
        step = h;
        mtp->cTau = t;
        for( i=0; i < mtp->nC*mtp->Nb; i++ )
            x[i] = tt[i][0];
        Solut(  x, dx, t );
        naccept++;
        CalcNewStates( naccept, kc, t, h, x );
        //
        if( kc == 1 )
        {
            kopt = 2;
            if( reject ) kopt = 1;
        }
        else if( kc <= k )
        {
            kopt = kc;
            if( w[kc-1] < w[kc]*FAC3 ) kopt = kc - 1;
            if( w[kc] < w[kc-1]*FAC3 )
                kopt = min( (kc+1) , (KM-1) );
        }
        else
        {
            kopt = kc-1;
            if( kc > 2 && w[kc-2] < w[kc-1]*FAC3 )
                kopt = kc - 2;
            if( w[kc] < w[kopt]*FAC3 )
                kopt = min( kc, (KM-1) );
        }
        // 
        if( reject )
        {
            k = min( kopt, kc );
            h = min( h, hh[ k ] );
            reject = 0;           /*false*/
        }
        else
        {  // 
            if( kopt <= kc )
                h = hh[ kopt ];
            else if( kc < k && ( w[kc] < ( w[kc-1] * FAC4 ) ))
                h = hh[kc] * a1[kopt+1] / a1[kc];
            else h = hh[kc] * a1[kopt] / a1[kc];
            k = kopt;
        }
        h1=t_end-t;
    }     /*while*/
    *tv = t;
    step = h;
    return;
l100:
    k = min( k, kc );
    if( k > 1 && ( w[k-1] < w[k] * FAC3 ) )
        k--;
    nrejct++;
    h = hh[k];
    reject = 1;
    goto l30;
}


// --------------------- end of m_gem2mtbox.cpp ---------------------------


