//-------------------------------------------------------------------
// $Id$
//
// Debugging version of coupled finite-difference
// 1D advection-diffusion mass transport model
// supplied by F.Enzmann (Uni Mainz) with GEMIPM2K
// module for calculation of chemical equilibria
//
// uses tnodearray class
//
// Copyright (C) 2005 S.Dmytriyeva, F.Enzmann, D.Kulik
//
//-------------------------------------------------------------------

#include "tnodearray.h"
//#include <stdio.h>
int MassTransAdvec( double L,    // length of system [L]
               double v,    // constant fluid velocity [L/T]
               double tf,   // time step reduce control factor
               double bC,   // initial background solute concentration [M/L**3]
               double iCx,  // inital concentration in the node inx
               int    nx,   // number of nodes
               int    mts,  // maximal time steps
               int    inx  // initial node index
              );

void logProfile( FILE* logfile, int t, double at, int nx, int every );
void logDiffs( FILE* diffile, int t, double at, int nx, int every );

int
 main( int argc, char* argv[] )
 {

     double L,    // length of system [L]
            v,    // constant fluid velocity [L/T]
            tf,   // time step reduce control factor
            bC,   // initial background solute concentration [M/L**3]
            iCx,  // inital concentration @ node # x
            dx,   // node distance [L]
            dt,   // iterative time increment
            at;   // actual time step

     int    nx,   // number of nodes
            mts,  // maximal time steps
            inx,  // initial node index
            RetC = 0;

     L = 1.;      // length in m
     v = 1e-8;    // fluid velocity constant m/sec
     tf = 1.;     // time step reduce factor
     bC = 1e-9;   // initial background concentration over all nodes  0
     iCx = 1.;     // initial concentration M/m3

nx = 100;    // number of nodes (default 1500)
mts = 200; // max number of time steps   10000
     inx = 1;     // in the node index inx

     gstring multu_in1 = "MgWBoundC.ipm";
     gstring chbr_in1 = "ipmfiles-dat.lst";

// from argv
      if (argc >= 2 )
        multu_in1 = argv[1];
      if (argc >= 3 )
        chbr_in1 = argv[2];

// The NodeArray must be allocated here
    TNodeArray::na = new TNodeArray( nx+1 );

// Prepare the array for initial conditions allocation
     int* nodeType = new int[nx+1];
     for(int ii =0; ii<nx+1; ii++ )
       nodeType[ii] = 1;
     nodeType[0] = 2;
     nodeType[1] = 2;
// This is constant injection mode - both nodes 0 and 1 should be set the same!

 // Here we read the MULTI structure, DATACH and DATABR files prepared from GEMS
    if( TNodeArray::na->NewNodeArray(
             multu_in1.c_str(), chbr_in1.c_str(), nodeType ) )
      return 1;  // error reading files

// here we call the mass-transport finite-difference coupled routine
    RetC = MassTransAdvec( L, v, tf, bC, iCx, nx, mts, inx );

   delete[] nodeType;
   delete TNodeArray::na;

   return RetC;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Data collection for monitoring 1D profiles - to extend here, if needed
// Prints phases and element concentrations in all cells for time point t / at
void logProfile( FILE* logfile, int t, double at, int nx, int every_t )
{
  double pm[16];
  int i, ie, ip;
  DATACH* CH = TNodeArray::na->pCSD();       // access to DataCH structure
  DATABRPTR* C0 = TNodeArray::na->pNodT0();  // nodes at current time point
//  DATABRPTR* C1 = TNodeArray::na->pNodT1();  // nodes at previous time point
  if( t % every_t )
    return;
  fprintf( logfile, "\nStep= %-8d  Time= %-12.4g\n", t, at/(365*86400) );
  fprintf(logfile, "%s","Node#   Calcite     Dolomite     ");
  for( ie=0; ie<min( 14,int(CH->nICb) ); ie++ )
    fprintf( logfile, "%-12.4s ", CH->ICNL[ie] );
  for (i=0; i<nx+1; i++)    // node iteration
  {
     fprintf( logfile, "\n%5d   ", i );
     pm[0]  = C0[i]->xPH[2];   // amount of calcite
     pm[1]  = C0[i]->xPH[3];   // amount of dolomite
     for( ie=0; ie < min( 14, int(CH->nICb) ); ie++ )
       pm[ie+2]  = C0[i]->bPS[0*CH->nICb + ie]/C0[i]->vPS[0]*1000.;
                 // total dissolved element molarity
     for( ip = 0; ip < min( 16, 2+int(CH->nICb) ); ip++ )
        fprintf( logfile, "%-12.4g ", pm[ip] );
   }
   fprintf( logfile, "\n" );
}

// Data collection for monitoring differences
// Prints difference increments in a all nodes (cells) for time point t / at
void logDiffs( FILE* diffile, int t, double at, int nx, int every_t )
{
  double dc;
  int i, ie;
  DATACH* CH = TNodeArray::na->pCSD();       // access to DataCH structure
  DATABRPTR* C0 = TNodeArray::na->pNodT0();  // nodes at current time point
  DATABRPTR* C1 = TNodeArray::na->pNodT1();  // nodes at previous time point

  if( t % every_t )
    return;

  fprintf( diffile, "\nStep= %-8d  Time= %-12.4g\nNode#   ", t, at/(365*86400) );
  for( ie=0; ie < int(CH->nICb); ie++ )
    fprintf( diffile, "%-12.4s ", CH->ICNL[ie] );
  for (i=0; i<nx+1; i++)    // node iteration
  {
     fprintf( diffile, "\n%5d   ", i );
     for( ie=0; ie < int(CH->nICb); ie++ )
     {
        dc = C1[i]->bIC[ie] - C0[i]->bIC[ie];
        fprintf( diffile, "%-12.4g ", dc );
     }
  }
  fprintf( diffile, "\n" );
}

//---------------------------------------------------------------------------
// Test of 1D advection (finite difference method provided by Dr. F.Enzmann,
// Uni Mainz) coupled with GEMIPM2K kernel (PSI) using the tnodearray class.
// Finite difference calculations split over independent components
// (through bulk composition of aqueous phase).
// Experiments with smoothing terms on assigning differences to bulk composition
// of nodes
int MassTransAdvec( double L,    // length of system [L]
               double v,    // constant fluid velocity [L/T]
               double tf,   // time step reduce control factor
               double bC,   // initial background solute concentration [M/L**3]
               double iCx,  // inital concentration @ node # x
               int    nx,   // number of nodes
               int    mts,  // maximal time steps
               int    inx  // initial node index
              )
{
     double dx,   // node distance [L]
            dt,   // iterative time increment
            at;   // actual time step

     int   t,    // actual time iterator
           ic,
           RetCode = OK_GEM_AIA,
           i,
           ie,
           ip;

     double c0,
            c1,
            cm1,
            cm2,
            c12,
            cm12,
            sM0,
            sM1,
            dc,    // difference (decrement) to concentration/amount
            cr,      // some help variables
     pm[16];  // up to 16 variables for plotting/monitoring

     // test output files
FILE* logfile;
FILE* diffile;
logfile = fopen( "Profiles.dat", "w+" );
if( !logfile)
  return -1;
diffile = fopen( "Differences.dat", "w+" );
if( !logfile)
  return -1;

// constant injection mode
//     C[inx][0]=iCx;  Initial condition for Dirak input
//     C[inx][1]=iCx;
// The NodeArray must have been allocated before, setting up work pointers

     DATACH* CH = TNodeArray::na->pCSD();       // access to DataCH structure
     DATABRPTR* C0 = TNodeArray::na->pNodT0();  // nodes at current time point
     DATABRPTR* C1 = TNodeArray::na->pNodT1();  // nodes at previous time point

// starting the time iteration loop

     dx = L/nx;
     dt = 0.5*(dx/v)*1/tf;

     at = 0;
     t = 0;

     for (i=1; i<nx; i++)    // node iteration
     {
       int Mode = NEED_GEM_AIA; // debugging
       C0[i]->bIC[CH->nICb-1] = 0.;   // zeroing charge off
//       RetCode = TNodeArray::na->RunGEM( 0, Mode );
       RetCode = TNodeArray::na->RunGEM( i, Mode );
     }  // end of node iteration loop
// Data collection for monitoring: Initial state (at t=0)
logProfile( logfile, t, at, nx, 1 );
logDiffs( diffile, t, at, nx, 1 );

     do {   // time iteration step

        t+=1;
        at=at+dt;
        cr=v*dt/dx;

        for( i=2;i<nx;i++ ) {   // node iteration
           for( ic=0; ic < CH->nICb-1; ic++)  // splitting for independent components
           {                          // Charge (Zz) is not checked here!
              // It has to be checked on minimal allowed c0 value
              c0  = C0[i]->bPS[0*CH->nICb + ic];
              c1  = C0[i+1]->bPS[0*CH->nICb + ic];
              cm1 = C0[i-1]->bPS[0*CH->nICb + ic];
              cm2 = C0[i-2]->bPS[0*CH->nICb + ic];

              if (i==nx) c1=c0;    // the right boundary is open ....

              c12=((c1+c0)/2)-(cr*(c1-c0)/2)-((1-cr*cr)*(c1-2*c0+cm1)/6);
              cm12=((c0+cm1)/2)-(cr*(c0-cm1)/2)-((1-cr*cr)*(c0-2*cm1+cm2)/6);
              dc = cr*(c12-cm12);

// Checking the difference to assign
// if( fabs(dc) > min( 1e-7, C0[i]->bIC[ic] * 1e-4 ))
              C0[i]->bPS[0*CH->nICb + ic] = c0-dc;  // Correction for FD numerical scheme
if( fabs(dc) > min( 1e-7, C0[i]->bIC[ic] * 1e-3 ))
              C0[i]->bIC[ic] -= dc; // correction for GEM calcuation
           }
         } // end of loop over nodes

//       Here we call a loop on GEM calculations over nodes
//       parallelization will affect this loop
         for (i=1; i<nx; i++)    // node iteration
         {
           int Mode = NEED_GEM_PIA;  // debugging
           bool NeedGEM = false;     // debugging

C0[i]->bIC[CH->nICb-1] = 0.;   // zeroing charge off in bulk composition

           // Here we compare this node for current time and for previous time
           for( ic=0; ic < CH->nICb-1; ic++)    // we do not check charge here!
           {     // It has to be checked on minimal allowed c0 value
              if( C0[i]->bIC[ic] < 1e-12 )
              { // to stay on safe side
                 C0[i]->bIC[ic] = 1e-12;
              }
              dc = C1[i]->bIC[ic] - C0[i]->bIC[ic];
if( fabs( dc ) > min( 1e-7, (C0[i]->bIC[ic] * 1e-3 )))
                  NeedGEM = true;  // we need to recalculate equilibrium in this node
if( fabs( dc ) > min( 1e-5, C0[i]->bIC[ic] * 1e-2 ))
                  Mode = NEED_GEM_AIA;  // we even need to do it in auto Simplex mode
// this has to be done in an intelligent way as a separate subroutine
           }

           if( NeedGEM )
           {
              RetCode = TNodeArray::na->RunGEM( i, Mode );
              // check RetCode
//             if( !(RetCode==OK_GEM_AIA || RetCode == OK_GEM_PIA ))
//                 break;
           }
         }  // end of node iteration loop

logDiffs( diffile, t, at, nx, 10 );  // logging differences after the MT iteration loop

//         if( RetCode==OK_GEM_AIA || RetCode == OK_GEM_PIA )
//         {
          // Here one has to compare old and new equilibrium phase assemblage
          // and pH/pe in all nodes and decide if the time step was Ok or it
          // should be decreased. If so then the nodes from C1 should be
          // copied to C0 (to be implemented)

          // time step accepted - Copying nodes from C0 to C1 row
          for (int i=1; i<nx; i++)    // node iteration
          {
             bool NeedCopy = false;
             for( ic=0; ic < CH->nICb-1; ic++) // do not check charge
             {
                dc = C1[i]->bIC[ic] - C0[i]->bIC[ic];
if( fabs( dc ) > min( 1e-7, (C0[i]->bIC[ic] * 1e-3)))
                  NeedCopy = true;
             }
             if( NeedCopy )
                 TNodeArray::na->CopyNodeFromTo( i, nx, C0, C1 );
           }

// Data collection for monitoring: Current state
logProfile( logfile, t, at, nx, 10 );

     } while ( t < mts );
     // && ( RetCode==OK_GEM_AIA || RetCode == OK_GEM_PIA ) ) ;
      // Other criteria to stop need to be implemented

fclose( logfile );
fclose( diffile );

   return RetCode;
}

//---------------------------------------------------------------------------

