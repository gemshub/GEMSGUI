//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "main.h"
#include <stdio.h>


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender)
{
     double L,    // length of system [L]
            v,    // constant fluid velocity [L/T]
            tf,   // time step reduce control factor
            bC,   // initial background solute concentration [M/L**3]
            iCx,  // inital concentration @ node # x
            dx,   // node distance [L]
            dt,   // iterative time increment
            at;   // actual time step

     double **C;  // concentrations data arrays
//   TNodeArray

     int    nx,   // number of nodes
            mts,  // maximal time steps
            t,    // actual time iterator
            inx;  // initial node index

     double c0,
            c1,
            cm1,
            cm2,
            c12,
            cm12,
            sM0,
            sM1,
     dc,    // increment to concentration/amount
            cr;      // some help variables

            FILE *stream;

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
     L = 1.;      // length in m
     nx = 100;    // number of nodes (default 1500)
     v = 1e-8;    // fluid velocity constant m/sec
     tf = 1.;     // time step reduce factor
     mts = 10000; // max number of time steps
     bC = 1e-9;   // initial background concentration over all nodes  0
     iCx = 1.;     // initial concentration M/m3
     inx = 1;     // in the node index inx
// to define constant injection mode

     PB->Max=mts;

     C = new double*[nx*2];
     for (int i=0;i<=nx+1;i++)  {
          C[i] = new double[1];
          C[i] = new double[2];
     }

     stream=fopen("test.dat","w+");

     // init concentration arrays
     for (int i=0;i<=nx+1;i++) {
         C[i][0]=bC;
         C[i][1]=bC;
     }

// The NodeArray must be allocated here
    TNodeArray::na = new TNodeArray( nx+1 );

// Prepare the array for initial conditions allocation
     int* nodeType = new int(nx);
     for( ii =0; ii<nx+1; ii++ )
       nodeType[ii] = 1;
     nodeType[0] = 2;

//     C[inx][0]=iCx;   Initial condition for Dirak input
//     C[inx][1]=iCx;

 // Here we read MULTI structure, DATACH and DATABR files prepared from GEMS
TNodeArray::na->NewNodeArray( "MgWBoundC.ipm", "ipmfiles-dat.lst",
                   nodeType );

     DATACH* CH = TNodeArray::na->pCSD();
     DATABRPTR* C0 = TNodeArray::na->pNodT0();  // nodes at current time point
     DATABRPTR* C1 = TNodeArray::na->pNodT1();  // nodes at previous time point

     // start time iteration loop

     dx = L/nx;
     dt = 0.5*(dx/v)*1/tf;

     at = 0;
     t = 0;

     do {         // time iteration

        t+=1;
        at=at+dt;
        cr=v*dt/dx;

        PB->Position=t;

        for (int i=2;i<=nx;i++) {   // node iteration

// Concentrations in these nodes are fixed
           if (CB->State==cbChecked) {
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
           int Mode = NEED_GEM_FIA, RetCode;
           bool NeedGEM = false;

           // Here we compare this node for current time and for previous time
           for( ic=0; ic < CH->nICb; ic++)
           {                   // It has to be checked on minimal allowed c0 value
              el0 = C0[i]->bIC[ic];
              if( el0 <= 1e-16 )
              { // to stay on safe side
                el0 = 1e-16;
                C0[i]->bIC[ic] = el0;
              }
              el1 = C1[i]->bIC[ic];
              diff = fabs( el0 - el1 )/el1;
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

         sM0=0;sM1=0;
         for (int i=0;i<=nx;i++) {
             sM0+=C[i][0];
             sM1+=C[i][1];
         }

         SL1->Caption="Status ... time step " + IntToStr(t) + " time: " + FloatToStr(at/(365*86400));
         SL2->Caption="Status ... mass 1: " + FloatToStr(sM0);
         SL3->Caption="Status ... conc last node: " + FloatToStr(C[nx-1][0]);


         //fprintf(stream, "%d  %E  %E  %E \n", t, at, sM0, sM1);
         fprintf(stream, "%d  %E\n", t, at);

         Update();

     } while  ((t<mts) & (C[nx][0]<iCx*(1-1e-9)) & (sM0>1e-9)) ;


     fclose(stream);

      for (int i=0;i<=nx+1; i++)
          delete[] C[i];
      delete[] C;

}
//---------------------------------------------------------------------------

