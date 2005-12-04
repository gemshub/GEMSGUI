//-------------------------------------------------------------------
// $Id$
//
// Debugging version of a finite-difference 1D advection-diffusion
// mass transport model supplied by Dr. Frieder Enzmann (Uni Mainz)
// coupled with GEMIPM2K module for calculation of chemical equilibria
//
// Direct access to the TNodeArray class for storing all data for nodes
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

int
 main( int argc, char* argv[] )
 {

     double L,    // length of system [L]
            v,    // constant fluid velocity [L/T]
            tf,   // time step reduce control factor
            minel,  // minimal allowed amount of element (except charge) in bulk composition
            cutoff;  // cutoff value for applied differences

     int    nx,   // number of nodes
            mts,  // maximal time steps
            every,// output every time step (e.g. every 10-th step)
            inx,  // initial node index ????
            RetC = 0;

     L = 1.;      // length in m
     v = 1e-9;    // fluid velocity constant m/sec
     tf = 1.;     // time step reduce factor
     cutoff = 1e-7;   // cutoff value for delta_T corrections for bulk compositions)
     minel = 1e-12;   // minimal allowed amount of element (except charge) in bulk composition

nx = 100;    // number of nodes (default 1500)
mts = 300;   // max number of time steps   10000
every = 10;  // output on every 20-th time step
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
           i;

     double c0,
            c1,
            cm1,
            cm2,
            c12,
            cm12,
            dc,    // difference (decrement) to concentration/amount
            cr;      // some help variables

// Preparations: opening output files for monitoring 1D profiles
FILE* logfile;
logfile = fopen( "ICaq-log.dat", "w+" );    // Total dissolved element molarities
if( !logfile)
  return -1;
FILE* ph_file;
ph_file = fopen( "Ph-log.dat", "w+" );   // Mole amounts of phases
if( !logfile)
  return -1;
FILE* diffile;
diffile = fopen( "ICdif-log.dat", "w+" );   //  Element amount diffs for t and t-1
if( !logfile)
  return -1;

//  Getting direct access to TNodeArray class data
DATACH* CH = TNodeArray::na->pCSD();       // DataCH structure
DATABRPTR* C0 = TNodeArray::na->pNodT0();  // nodes at current time point
DATABRPTR* C1 = TNodeArray::na->pNodT1();  // nodes at previous time point

// time testing
  clock_t t_start, t_end, t_out, t_out2;
  clock_t outp_time = 0.;

// constant injection mode
//     C[inx][0]=iCx;  Initial condition for Dirak input
//     C[inx][1]=iCx;
// The NodeArray must have been allocated before, setting up work pointers
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
TNodeArray::na->logDiffsIC( diffile, t, at/(365*86400), nx+1, 1 );
TNodeArray::na->logProfileAqIC( logfile, t, at/(365*86400), nx+1, 1 );
TNodeArray::na->logProfilePhMol( ph_file, t, at/(365*86400), nx+1, 1 );
t_out2 = clock();
outp_time += ( t_out2 - t_out);
//  This loop contains the mass transport iteration time step
     do {   // time iteration step

        t+=1;
        at=at+dt;
        cr=v*dt/dx;

        for( i=2;i<nx;i++ ) {   // node iteration
           for( ic=0; ic < CH->nICb-1; ic++)  // splitting for independent components
           {                          // Charge (Zz) is not checked here!
                         // Chemical compositions may become inconsistent with time
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
// end of the mass transport iteration time step

//   Here we call a loop on GEM calculations over nodes
//   parallelization should affect this loop only
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
              // check RetCode from GEM IPM calculation
              if( !(RetCode==OK_GEM_AIA || RetCode == OK_GEM_PIA ))
              {
                gstring err_msg;

               fprintf( diffile, "\nError in GEMipm calculation part :" );
               fprintf( diffile, " Node= %-8d  Step= %-8d", i, t );
               switch( RetCode )
               {
                 case BAD_GEM_AIA:
                      err_msg = "Bad GEM result using simplex IA";
                      break;
                case  ERR_GEM_AIA:
                      err_msg = "GEM calculation error using simplex IA";
                      break;
                case  BAD_GEM_PIA:
                      err_msg = "Bad GEM result using previous solution IA";
                      break;
                case  ERR_GEM_PIA:
                      err_msg = "GEM calculation error using previous solution IA";
                      break;
               case  TERROR_GEM:  err_msg =  "Terminal error in GEMIPM2 module";
              }
              fprintf(diffile, "\n           %s", err_msg.c_str() );
//SD              break;
            }
         }
      }  // i   end of node iteration loop

//SD     if( RetCode==OK_GEM_AIA || RetCode == OK_GEM_PIA )
//     {
t_out = clock();
TNodeArray::na->logDiffsIC( diffile, t, at/(365*86400), nx+1, evrt );
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
          }  // i    end of node iteration loop

// Data collection for monitoring: Current state
t_out = clock();
TNodeArray::na->logProfileAqIC( logfile, t, at/(365*86400), nx+1, evrt );
TNodeArray::na->logProfilePhMol( ph_file, t, at/(365*86400), nx+1, evrt );
t_out2 = clock();
outp_time += ( t_out2 - t_out);
//   }

     } while ( t < mts );//SD && ( RetCode==OK_GEM_AIA || RetCode == OK_GEM_PIA ) ) ;
      // Other criteria to stop need to be implemented

t_end = clock();
double dtime = ( t_end- t_start );
fprintf( diffile,
  "\nTotal time of calculation %lg s;  Time of output %lg s;  Whole run time %lg s\n",
    (dtime-outp_time)/CLK_TCK,  outp_time/CLK_TCK, dtime/CLK_TCK );

fclose( logfile );
fclose( ph_file );
fclose( diffile );

   return RetCode;
}

//---------------------------------------------------------------------------

