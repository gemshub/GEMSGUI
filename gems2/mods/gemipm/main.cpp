//---------------------------------------------------------------------------

#include "tnodearray.h"
//#include <stdio.h>
int MassTransAdvec( double L,    // length of system [L]
               double v,    // constant fluid velocity [L/T]
               double tf,   // time step reduce control factor
               double bC,   // initial background solute concentration [M/L**3]
               double iCx,  // inital concentration @ node # x
               int    nx,   // number of nodes
               int    mts,  // maximal time steps
               int    inx  // initial node index
              );

int
 main( int argc, char* argv[] )
 {

     fstream cout( "log1.out", ios::out|ios::app );

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
            inx;  // initial node index


     L = 1.;      // length in m
     v = 1e-8;    // fluid velocity constant m/sec
     tf = 1.;     // time step reduce factor
     bC = 1e-9;   // initial background concentration over all nodes  0
     iCx = 1.;     // initial concentration M/m3

     nx = 100;    // number of nodes (default 1500)
     mts = 100; // max number of time steps   10000
     inx = 1;     // in the node index inx

     gstring multu_in1 = "MgWBoundC.ipm";
     gstring chbr_in1   = "ipmfiles-dat.lst";

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

 // Here we read MULTI structure, DATACH and DATABR files prepared from GEMS
    if( TNodeArray::na->NewNodeArray(
             multu_in1.c_str(), chbr_in1.c_str(), nodeType ) )
      return 0;  // error reading files

    MassTransAdvec( L, v, tf, bC, iCx, nx, mts, inx );

   delete[] nodeType;
   delete TNodeArray::na;

   return 0;
}

//---------------------------------------------------------------------------
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

     double **C;  // concentrations data arrays      // compare

     int   t,    // actual time iterator
           ic;

     double c0,
            c1,
            cm1,
            cm2,
            c12,
            cm12,
            sM0,
            sM1,
     dc,    // increment to concentration/amount
     el0,
     el1,
     diff,
            cr;      // some help variables

     FILE *stream;                                        // compare

     // test output file
     fstream log1( "Ca-profile.dat", ios::out );
     fstream log2( "Mg-profile.dat", ios::out );
//

     // read settings from formular ...
/*
     L = StrToFloat(Edit1->Text);
     nx = StrToInt(Edit2->Text);
     v = StrToFloat(Edit5->Text);
     tf = StrToFloat(Edit4->Text);
     mts = StrToInt(Edit3->Text);
     bC = StrToInt(Edit6->Text);
     iCx = StrToFloat(Edit7->Text);
     inx = StrToInt(Edit8->Text);
*/
// to define constant injection mode

     C = new double*[nx*2];                              // compare
     for (int i=0;i<=nx+1;i++)  {
          C[i] = new double[1];
          C[i] = new double[2];
     }

     stream=fopen("test.dat","w+");                      // compare

     // init concentration arrays                        // compare
     for (int i=0;i<=nx+1;i++) {
         C[i][0]=bC;
         C[i][1]=bC;
     }

     C[inx][0]=iCx;  // Initial condition for Dirak input // compare
     C[inx][1]=iCx;

// The NodeArray must be allocated before, set up work pointers

     DATACH* CH = TNodeArray::na->pCSD();
     DATABRPTR* C0 = TNodeArray::na->pNodT0();  // nodes at current time point
     DATABRPTR* C1 = TNodeArray::na->pNodT1();  // nodes at previous time point

     // start time iteration loop

     dx = L/nx;
     dt = 0.5*(dx/v)*1/tf;

     at = 0;
     t = 0;

// Data collection for monitoring - to extend here
log1 << endl << (at/(365*86400)) << "   ";
log2 << endl << (at/(365*86400)) << "   ";
         for (int i=0; i<nx+1; i++)    // node iteration
         {
             c0  = C0[i]->bPS[0*CH->nICb + 1];  // total dissolved Ca
             c1  = C0[i]->bPS[0*CH->nICb + 4];  // total dissolved Mg
             log1 << c0 << " ";
             log2 << c1 << " ";
         }
log1 << endl;
log2 << endl;

     do {         // time iteration

        t+=1;
        at=at+dt;
        cr=v*dt/dx;

        for (int i=2;i<nx;i++) {   // node iteration

// Concentrations in these nodes are fixed
           if ( true ) {                             // compare
//           if (CB->State==cbChecked) {
              for (int j=0;j<=inx;j++) {
              C[j][0]=iCx;   // replace with copying these DataBR structures from line C1 to line C0
              C[j][1]=iCx;   // leave for comparison as "conventional tracer"
           }
           }

           for( ic=0; ic < CH->nICb; ic++)
           {
              // It has to be checked on minimal allowed c0 value
              c0  = C0[i]->bPS[0*CH->nICb + ic];    // Volume of aq solution in i-th node
              c1  = C0[i+1]->bPS[0*CH->nICb + ic];
              cm1 = C0[i-1]->bPS[0*CH->nICb + ic];
              cm2 = C0[i-2]->bPS[0*CH->nICb + ic];

              if (i==nx) c1=c0;    // that means: right boundary is open ....

              c12=((c1+c0)/2)-(cr*(c1-c0)/2)-((1-cr*cr)*(c1-2*c0+cm1)/6);
              cm12=((c0+cm1)/2)-(cr*(c0-cm1)/2)-((1-cr*cr)*(c0-2*cm1+cm2)/6);
              dc = cr*(c12-cm12);
// Here we move not just volume but also the bulk composition of aq phase

              C0[i]->bPS[0*CH->nICb + ic] = c0-dc;  // Correct for FD numerical scheme
              C0[i]->bIC[ic] -= dc; // correct for GEM calcuation
           }
// This part is left for comparison
           c0=C[i][0];
           c1=C[i+1][0];
           cm1=C[i-1][0];
           cm2=C[i-2][0];
           if (i==nx) c1=c0;    // that means: right boundary is open ....

           c12=((c1+c0)/2)-(cr*(c1-c0)/2)-((1-cr*cr)*(c1-2*c0+cm1)/6);
           cm12=((c0+cm1)/2)-(cr*(c0-cm1)/2)-((1-cr*cr)*(c0-2*cm1+cm2)/6);

           C[i][0]=c0-cr*(c12-cm12);
           C[i][1]=c0-cr*(c0-c1);  // left for comparison
         } // end of loop over nodes

//       Here we call a loop on GEM calculations over nodes
//       parallelization affects this loop
         for (int i=1; i<nx; i++)    // node iteration
         {
           int Mode = NEED_GEM_PIA, RetCode;
           bool NeedGEM = true;     // debugging

           // Here we compare this node for current time and for previous time
           for( ic=0; ic < CH->nICb; ic++)
           {                   // It has to be checked on minimal allowed c0 value
              el0 = C0[i]->bIC[ic];
              if( el0 <= 1e-12 )
              { // to stay on safe side
                el0 = 1e-12;
                C0[i]->bIC[ic] = el0;
              }
              el1 = C1[i]->bIC[ic];
              diff = fabs( el0 - el1 )/el0;
              if( diff > 1e-6 )
                  NeedGEM = true;  // we need to recalculate equilibrium
              if( diff > 1e-4 )
                  Mode = NEED_GEM_AIA;  // we even need to do it in auto Simplex mode
           }
           if( NeedGEM )
           {
              RetCode = TNodeArray::na->RunGEM( i, Mode );
              // check RetCode
           }
         }

         // Here one has to compare old and new equilibrium phase assemblage
         // and pH/pe in all nodes and decide if the time step was Ok or it
         // should be decreased. If so then the nodes from C1 should be
         // copied to C0

         // Copying nodes from C0 to C1
         for (int i=1; i<nx; i++)    // node iteration
           TNodeArray::na->CopyNodeFromTo( i, nx, C0, C1 );

// Data collection for monitoring - to extend here
log1 << endl << (at/(365*86400)) << "   ";
log2 << endl << (at/(365*86400)) << "   ";
         for (int i=0; i<nx+1; i++)    // node iteration
         {
             c0  = C0[i]->bPS[0*CH->nICb + 1];  // total dissolved Ca
             c1  = C0[i]->bPS[0*CH->nICb + 4];  // total dissolved Mg
             log1 << c0 << " ";
             log2 << c1 << " ";
         }

         sM0=0;sM1=0;
         for (int i=0;i<=nx;i++) {
             sM0+=C[i][0];
             sM1+=C[i][1];
         }

//cout << endl;
//cout << "Status ... time step " << t << " time: " << (at/(365*86400)) << endl;
//cout << "Status ... mass 1: " << sM0 << endl;
//cout << "Status ... conc last node: " << C[nx-1][0] << endl;


         //fprintf(stream, "%d  %E  %E  %E \n", t, at, sM0, sM1);
         fprintf(stream, "%d  %E\n", t, at);

     } while  ((t<mts) & (C[nx][0]<iCx*(1-1e-9)) & (sM0>1e-9)) ;


     fclose(stream);

    for (int i=0;i<=nx+1; i++)
          delete[] C[i];
     delete[] C;

   return 0;
}
//---------------------------------------------------------------------------

