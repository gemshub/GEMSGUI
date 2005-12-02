//-------------------------------------------------------------------
// $Id$
//
// Debugging version of coupled finite-difference
// 1D advection-diffusion mass transport model
// supplied by F.Enzmann (Uni Mainz) with GEMIPM2K
// module for calculation of chemical equilibria
//
// uses TNodeArray class
//
// Copyright (C) 2005 S.Dmytriyeva, F.Enzmann, D.Kulik
//
//-------------------------------------------------------------------

#include "nodearray.h"
//#include <stdio.h>
int MassTransAdvec( double L,    // length of system [L]
               double v,    // constant fluid velocity [L/T]
               double tf,   // time step reduce control factor
               double cdv,  // cutoff value of differences to be applied to bulk compositions
               double cez,  // minimal allowed amount of element (except charge) in bulk composition
               int    nx,   // number of nodes
               int    mts,  // maximal time steps
               int    evrt, // output on every evrt time step (step 0 always) 
               int    inx   // initial node index
              );

void logProfile( FILE* logfile, int t, double at, int nx, int every );

int
 main( int argc, char* argv[] )
 {

     double L,    // length of system [L]
            v,    // constant fluid velocity [L/T]
            tf,   // time step reduce control factor
            bC,   // initial background solute concentration [M/L**3]
            minel,  // minimal allowed amount of element (except charge) in bulk composition
            dx,   // node distance [L]
            dt,   // iterative time increment
            at,   // actual time step
            cutoff;  // cutoff value for applied differences    

     int    nx,   // number of nodes
            mts,  // maximal time steps
            every,// output every time step (e.g. every 10-th step)
            inx,  // initial node index
            RetC = 0;

     L = 1.;      // length in m
     v = 3e-9;    // fluid velocity constant m/sec
     tf = 1.;     // time step reduce factor
     cutoff = 1e-7;   // cutoff value for differences (to use for correcting bulk compositions)
     minel = 1e-10;   // minimal allowed amount of element (except charge) in bulk composition
      
nx = 100;    // number of nodes (default 1500)
mts = 500;   // max number of time steps   10000
every = 20;  // output on every 20-th time step
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
       nodeType[ii] = 2;
     nodeType[0] = 1;
     nodeType[1] = 1;
// This is constant injection mode - both nodes 0 and 1 should be set the same!

 // Here we read the MULTI structure, DATACH and DATABR files prepared from GEMS
    if( TNodeArray::na->NewNodeArray(
             multu_in1.c_str(), chbr_in1.c_str(), nodeType ) )
      return 1;  // error reading files

// here we call the mass-transport finite-difference coupled routine
    RetC = MassTransAdvec( L, v, tf, cutoff, minel, nx, mts, every, inx );

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
  fprintf( logfile, "\nStep= %-8d  Time= %-12.4g     Dissolved total concentrations, M\n", t, at/(365*86400) );
  fprintf(logfile, "%s","Node#    Calcite     Dolomite     ");
  for( ie=0; ie<min( 14,int(CH->nICb) ); ie++ )
    fprintf( logfile, "%-12.4s ", CH->ICNL[ie] );
  for (i=0; i<nx+1; i++)    // node iteration
  {
     fprintf( logfile, "\n%5d   ", i );
     pm[0]  = C0[i]->xPH[3];   // amount of calcite
     pm[1]  = C0[i]->xPH[4];   // amount of dolomite
     for( ie=0; ie < min( 14, int(CH->nICb) ); ie++ )
       pm[ie+2]  = C0[i]->bPS[0*CH->nICb + ie]/C0[i]->vPS[0]*1000.;
                 // total dissolved element molarity
     for( ip = 0; ip < min( 16, 2+int(CH->nICb) ); ip++ )
        fprintf( logfile, "%-12.4g ", pm[ip] );
   }
   fprintf( logfile, "\n" );
}

//---------------------------------------------------------------------------
// Test of 1D advection (finite difference method provided by Dr. F.Enzmann,
// Uni Mainz) coupled with GEMIPM2K kernel (PSI) using the TNodeArray class.
// Finite difference calculations split over independent components
// (through bulk composition of aqueous phase).
// Experiments with smoothing terms on assigning differences to bulk composition
// of nodes
int MassTransAdvec( double L,    // length of system [L]
               double v,     // constant fluid velocity [L/T]
               double tf,    // time step reduce control factor
               double cdv,   // cutoff value of differences to be applied to bulk compositions
               double cez,   // minimal allowed amount of element (except charge) in bulk composition
               int    nx,    // number of nodes
               int    mts,   // maximal time steps
               int    evrt,  // output calls on every evrt time step
               int    inx    // initial node index
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

// time testing
  clock_t t_start, t_end, t_out, t_out2;
  clock_t outp_time = 0.;

// constant injection mode
//     C[inx][0]=iCx;  Initial condition for Dirak input
//     C[inx][1]=iCx;
// The NodeArray must have been allocated before, setting up work pointers

     DATACH* CH = TNodeArray::na->pCSD();       // access to DataCH structure
     DATABRPTR* C0 = TNodeArray::na->pNodT0();  // nodes at current time point
     DATABRPTR* C1 = TNodeArray::na->pNodT1();  // nodes at previous time point

// time( NULL);
// starting the time iteration loop
     t_start = clock();

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
t_out = clock();
logProfile( logfile, t, at, nx, 1 );
TNodeArray::na->logDiffs( diffile, t, at/(365*86400), nx, 1 );
t_out2 = clock();
outp_time += ( t_out2 - t_out);

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
// if( fabs(dc) > min( cdv, C0[i]->bIC[ic] * 1e-4 ))
              C0[i]->bPS[0*CH->nICb + ic] = c0-dc;  // Correction for FD numerical scheme
if( fabs(dc) > min( cdv, C0[i]->bIC[ic] * 1e-3 ))
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
              if( C0[i]->bIC[ic] < cez )
              { // to stay on safe side
                 C0[i]->bIC[ic] = cez;
              }
              dc = C1[i]->bIC[ic] - C0[i]->bIC[ic];
if( fabs( dc ) > min( cdv, (C0[i]->bIC[ic] * 1e-3 )))
                  NeedGEM = true;  // we need to recalculate equilibrium in this node
// if( fabs( dc ) > min( cdv*100., C0[i]->bIC[ic] * 1e-2 ))
                  Mode = NEED_GEM_AIA;  // we even need to do it in auto Simplex mode
// this has to be done in an intelligent way as a separate subroutine
           }

           if( NeedGEM )
           {
              RetCode = TNodeArray::na->RunGEM( i, Mode );
              // check RetCode
              if( !(RetCode==OK_GEM_AIA || RetCode == OK_GEM_PIA ))
              {
                gstring err_msg;

               fprintf( diffile, "\nError in GEMipm calculation part :" );
               fprintf( diffile, " Node= %-8d  Step= %-8d", i, t );
               switch( RetCode )
               {
                 case BAD_GEM_AIA:
                      err_msg = "Bad result from simplex IA";
                      break;
                case  ERR_GEM_AIA:
                      err_msg = "Failed to calculated from simplex IA";
                      break;
                case  BAD_GEM_PIA:
                      err_msg = "Bad result without simplex from previous solution";
                      break;
                case  ERR_GEM_PIA:
                      err_msg = "Failed to calculated without simplex";
                      break;
               case  TERROR_GEM:  err_msg =  "Terminal error GemIPM";
              }
              fprintf(diffile, "\n           %s", err_msg.c_str() );
              break;
            }
         }
      }  // end of node iteration loop

     if( RetCode==OK_GEM_AIA || RetCode == OK_GEM_PIA )
     {
t_out = clock();
TNodeArray::na->logDiffs( diffile, t, at/(365*86400), nx, evrt );
    // logging differences after the MT iteration loop
t_out2 = clock();
outp_time += ( t_out2 -  t_out);

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
if( fabs( dc ) > min( cdv, (C0[i]->bIC[ic] * 1e-3)))
                  NeedCopy = true;
             }
             if( NeedCopy )
                 TNodeArray::na->CopyNodeFromTo( i, nx, C0, C1 );
           }

// Data collection for monitoring: Current state
t_out = clock();
logProfile( logfile, t, at, nx, evrt );
t_out2 = clock();
outp_time += ( t_out2 - t_out);

  }

     } while ( t < mts  && ( RetCode==OK_GEM_AIA || RetCode == OK_GEM_PIA ) ) ;
      // Other criteria to stop need to be implemented

t_end = clock();
double dtime = ( t_end- t_start );
fprintf( diffile,
  "\nTotal time of calculation %lg, Time of output %lg, Whole run time %lg\n",
    (dtime-outp_time)/CLK_TCK,  outp_time/CLK_TCK, dtime/CLK_TCK );

fclose( logfile );
fclose( diffile );

   return RetCode;
}

//---------------------------------------------------------------------------

