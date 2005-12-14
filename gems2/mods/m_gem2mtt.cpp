//-------------------------------------------------------------------
// $Id: m_gem2mtt.cpp 663 2005-12-13 16:27:14Z gems $
//
// Implementation of TGEM2MT class, mass transport functions
//
// Copyright (C) 2005 S.Dmytriyeva, D.Kulik
//
// This file is part of a GEM-Selektor library for thermodynamic
// modelling by Gibbs energy minimization
// Uses: GEM-Vizor GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://les.web.psi.ch/Software/GEMS-PSI for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//

#include <math.h>
#include <stdio.h>

#include "m_gem2mt.h"
#include "service.h"
#include "visor.h"


// ===========================================================

// Copying nodes from C0 to C1 row
void  TGEM2MT::copyNodeArrays()
{
  //  Getting direct access to TNodeArray class data
  DATACH* CH = na->pCSD();       // DataCH structure
  DATABRPTR* C0 = na->pNodT0();  // nodes at current time point
  DATABRPTR* C1 = na->pNodT1();  // nodes at previous time point
  double dc;

  for (int ii=1; ii<mtp->nC; ii++)    // node iteration
   {
     bool NeedCopy = false;
     for(int ic=0; ic < CH->nICb-1; ic++) // do not check charge
     {
        dc = C1[ii]->bIC[ic] - C0[ii]->bIC[ic];
        if( fabs( dc ) > min( mtp->cdv, (C0[ii]->bIC[ic] * 1e-3)))
                  NeedCopy = true;
     }
     if( NeedCopy )
       na->CopyNodeFromTo( ii, mtp->nC, C0, C1 );
   }  // ii    end of node iteration loop
}


//-------------------------------------------------------------------
// NewNodeArray()  make worked DATACH structure
// Allocates a new FMT node array of DATABR structures and
// reads in the data from MULTI (using nC, Bn and DiCp, DDc, HydP)
//-------------------------------------------------------------------
void  TGEM2MT::NewNodeArray()
{
 // generate Tval&Pval arrays
 if( mtp->PsTPai != S_OFF )
    gen_TPval();

 na->MakeNodeStructures( mtp->nICb, mtp->nDCb,  mtp->nPHb,
      mtp->xIC, mtp->xDC, mtp->xPH,
      mtp->Tval, mtp->Pval,
      mtp->nTai,  mtp->nPai, mtp->Tai[3], mtp->Pai[3]  );
 DATACH* data_CH = na->pCSD();

 // put DDc
 if( data_CH->DD && mtp->DDc )
  for( int jj=0; jj<data_CH->nDC; jj ++)
      data_CH->DD[jj] = mtp->DDc[jj];

 for( mtp->kv = 0; mtp->kv < mtp->nIV; mtp->kv++ )
 {
   pVisor->Message( window(), GetName(),
      "Generation of EqStat records\n"
           "Please, wait...", mtp->kv, mtp->nIV);

  // Make new Systat record & calculate it
     gen_task();
  // Save databr
     na->packDataBr();
  //
   for( int jj=0; jj<mtp->nC; jj ++)
    if( mtp->DiCp[jj] == mtp->kv )
     {    na->setNodeHandle( jj );
          na->MoveWorkNodeToArray( jj, mtp->nC,  na->pNodT0());
          na->CopyWorkNodeFromArray( jj, mtp->nC,  na->pNodT0() );
     }
    mt_next();      // Generate work values for the next EqStat rkey

 } // mtp->kv

 pVisor->CloseMessage();

 DATABRPTR* C0 = na->pNodT0();  // nodes at current time point
 for( int ii=0; ii<mtp->nC; ii++)
      if(  C0[ii] == 0 )
      Error( "NewNodeArray() error:" ," Undefined boundary condition!" );

 // put HydP
 for( int jj=0; jj<mtp->nC; jj ++)
   {    C0[jj]->Vt = mtp->HydP[jj][0];
        C0[jj]->eps = mtp->HydP[jj][1];
        C0[jj]->Km = mtp->HydP[jj][2];
        C0[jj]->al = mtp->HydP[jj][3];
        C0[jj]->hDl = mtp->HydP[jj][4];
        C0[jj]->nto = mtp->HydP[jj][5];
   }

  for (int ii=0; ii<mtp->nC; ii++)    // node iteration
   {
       na->CopyNodeFromTo( ii, mtp->nC, C0, na->pNodT1() );
   }  // ii    end of node iteration loop

}

//   Here we call a loop on GEM calculations over nodes
//   parallelization should affect this loop only
//   return code   true   Ok
//                 false  Error in GEMipm calculation part
//
bool TGEM2MT::CalcIPM( char mode, int start_node, int end_node )
{
   int Mode, ic, RetCode=OK_GEM_AIA;
   bool NeedGEM;
   bool iRet = true;
   double dc; // difference (decrement) to concentration/amount
   //  Getting direct access to TNodeArray class data
   DATACH* CH = na->pCSD();       // DataCH structure
   DATABRPTR* C0 = na->pNodT0();  // nodes at current time point
   DATABRPTR* C1 = na->pNodT1();  // nodes at previous time point

   end_node = min( end_node, (int)mtp->nC );

   for( int ii = start_node; ii< end_node; ii++) // node iteration
   {
     mtp->qc = ii;
     Mode = mode;  // debugging  NEED_GEM_PIA;
     NeedGEM = false;     // debugging

     C0[ii]->bIC[CH->nICb-1] = 0.;   // zeroing charge off in bulk composition

     // Here we compare this node for current time and for previous time
      for( ic=0; ic < CH->nICb-1; ic++)    // we do not check charge here!
       {     // It has to be checked on minimal allowed c0 value
         if( C0[ii]->bIC[ic] < mtp->cez )
         { // to stay on safe side
            C0[ii]->bIC[ic] = mtp->cez;
         }
         dc = C1[ii]->bIC[ic] - C0[ii]->bIC[ic];

        if( fabs( dc ) > min( mtp->cdv, (C0[ii]->bIC[ic] * 1e-3 )))
            NeedGEM = true;  // we need to recalculate equilibrium in this node
        if( fabs( dc ) > min( mtp->cdv*100., C0[ii]->bIC[ic] * 1e-2 ))
                  Mode = NEED_GEM_AIA;  // we even need to do it in auto Simplex mode
// this has to be done in an intelligent way as a separate subroutine
       }

     if( NeedGEM )
     {
        RetCode = na->RunGEM( ii, Mode );
        // check RetCode from GEM IPM calculation
        if( !(RetCode==OK_GEM_AIA || RetCode == OK_GEM_PIA ))
        {
          char buf[200];
          gstring err_msg;
          iRet = false;

          sprintf( buf, " Node= %-8d  Step= %-8d\n", ii, mtp->ct );
          err_msg = buf;
          switch( RetCode )
          {
                case BAD_GEM_AIA:
                      err_msg += "Bad GEM result using simplex IA";
                      break;
                case  ERR_GEM_AIA:
                      err_msg += "GEM calculation error using simplex IA";
                      break;
                case  BAD_GEM_PIA:
                      err_msg += "Bad GEM result using previous solution IA";
                      break;
                case  ERR_GEM_PIA:
                      err_msg += "GEM calculation error using previous solution IA";
                      break;
               case  TERROR_GEM:  err_msg +=  "Terminal error in GEMIPM2 module";
              }
              err_msg += "\n Continue?";
              if( !vfQuestion( window(),
                 "Error in GEMipm calculation part",err_msg.c_str() ))
                 Error("Error in GEMipm calculation part", "Process stoped");
// output multi with error
//              gstring mul_name = "multi_";
//              gstring br_name = "db_";
//              sprintf( buf, "%d_%d.err", i, t);
//              mul_name+=buf;
//              br_name+=buf;
//              na->printfGEM( mul_name.c_str(), br_name.c_str(),0 );
        }
     }
   }  // ii   end of node iteration loop
   return iRet;
}


// The mass transport iteration time step
void TGEM2MT::MassTransAdvecStart()
{
    mtp->dx = mtp->cLen/mtp->nC;
    mtp->dTau = 0.5*(mtp->dx/mtp->fVel)*1/mtp->tf;
    mtp->oTau = 0;
    mtp->cTau = mtp->Tau[START_];
    // mtp->cTau = 0;
    mtp->ct = 0;
}


// The mass transport iteration time step
void TGEM2MT::MassTransAdvecStep()
{
 double c0, c1, cm1,  cm2,
        c12, cm12, dc, cr;      // some help variables
 int ii, ic;

 //  Getting direct access to TNodeArray class data
 DATACH* CH = na->pCSD();       // DataCH structure
 DATABRPTR* C0 = na->pNodT0();  // nodes at current time point
 DATABRPTR* C1 = na->pNodT1();  // nodes at previous time point

   mtp->ct += 1;
   mtp->cTau += mtp->dTau;
   cr = mtp->fVel * mtp->dTau/mtp->dx;

   for( ii = 2; ii< mtp->nC-1; ii++) // node iteration, -1 the right boundary is open ....
   {
     mtp->qc = ii;
     for( ic=0; ic < CH->nICb-1; ic++)  // splitting for independent components
     {                          // Charge (Zz) is not checked here!
       // Chemical compositions may become inconsistent with time
       // It has to be checked on minimal allowed c0 value
        c0  = C0[ii]->bPS[0*CH->nICb + ic];
        c1  = C0[ii+1]->bPS[0*CH->nICb + ic];
        cm1 = C0[ii-1]->bPS[0*CH->nICb + ic];
        cm2 = C0[ii-2]->bPS[0*CH->nICb + ic];

        c12=((c1+c0)/2)-(cr*(c1-c0)/2)-((1-cr*cr)*(c1-2*c0+cm1)/6);
        cm12=((c0+cm1)/2)-(cr*(c0-cm1)/2)-((1-cr*cr)*(c0-2*cm1+cm2)/6);
        dc = cr*(c12-cm12);

// Checking the difference to assign
// if( fabs(dc) > min( cdv, C0[i]->bIC[ic] * 1e-4 ))
       C0[ii]->bPS[0*CH->nICb + ic] = c0-dc;  // Correction for FD numerical scheme
/*if( dc >= C0[i]->bIC[ic] )
 {
    fprintf( diffile, "\nError in Mass Transport calculation part :" );
    fprintf( diffile, " Node= %-8d  Step= %-8d  IC= %s ", i, t, CH->ICNL[ic] );
    fprintf(diffile, "\n Attempt to set new amount to %lg (old: %lg  Diff: = %lg ) ",
         C0[i]->bIC[ic]-dc, C0[i]->bIC[ic], dc);
    BC_error = true;
 } */
       if( fabs(dc) > min( mtp->cdv, C0[ii]->bIC[ic] * 1e-3 ))
              C0[ii]->bIC[ic] -= dc; // correction for GEM calcuation
   } // loop over IC
  } // end of loop over nodes
}


void TGEM2MT::Trans1D( char mode )
{
  int evrt =10;

// Preparations: opening output files for monitoring 1D profiles
FILE* logfile;
logfile = fopen( "ICaq-log.dat", "w+" );    // Total dissolved element molarities
if( !logfile)
  return;
FILE* ph_file;
ph_file = fopen( "Ph-log.dat", "w+" );   // Mole amounts of phases
if( !logfile)
  return;
FILE* diffile;
diffile = fopen( "ICdif-log.dat", "w+" );   //  Element amount diffs for t and t-1
if( !logfile)
  return;


// time testing
clock_t t_start, t_end, t_out, t_out2;
clock_t outp_time = 0.;
t_start = clock();


     switch( mtp->PsMode )
     {
      case GMT_MODE_A: MassTransAdvecStart();
                break;
      default: // more mass transport models here
               break;
     }

    CalcIPM( mode, 1, mtp->nC-1 );


// Data collection for monitoring: Initial state (at t=0)
t_out = clock();
na->logDiffsIC( diffile, mtp->ct, mtp->cTau/(365*86400), mtp->nC, 1 );
na->logProfileAqIC( logfile, mtp->ct, mtp->cTau/(365*86400), mtp->nC, 1 );
na->logProfilePhMol( ph_file, mtp->ct, mtp->cTau/(365*86400), mtp->nC, 1 );
t_out2 = clock();
outp_time += ( t_out2 - t_out);

        if(  mtp->PvMSg != S_OFF && vfQuestion(window(),
             GetName(), "Use graphic window?") )
        {
            RecordPlot( 0 );
        }

       pVisor->Message( window(), GetName(),
                 "Calculating transport; \n"
                 "Please, wait...", mtp->ct, mtp->ntM);
       setStop( false );


//  This loop contains the mass transport iteration time step
     do {   // time iteration step

        //  the mass transport iteration time step
         switch( mtp->PsMode )
         {
          case GMT_MODE_A: MassTransAdvecStep();
                  break;
          default: // more mass transport models here
                  break;
        }

       pVisor->Message( window(), GetName(),
                 "Calculating transport; \n"
                 "Please, wait...", mtp->ct, mtp->ntM);

       //   Here we call a loop on GEM calculations over nodes
       //   parallelization should affect this loop only
       CalcIPM( mode, 1, mtp->nC-1 );

t_out = clock();
na->logDiffsIC( diffile, mtp->ct, mtp->cTau/(365*86400), mtp->nC, evrt );
    // logging differences after the MT iteration loop
t_out2 = clock();
outp_time += ( t_out2 -  t_out);

          // Here one has to compare old and new equilibrium phase assemblage
          // and pH/pe in all nodes and decide if the time step was Ok or it
          // should be decreased. If so then the nodes from C1 should be
          // copied to C0 (to be implemented)

          // time step accepted - Copying nodes from C0 to C1 row
          CalcGraph();
          copyNodeArrays();


// Data collection for monitoring: Current state
t_out = clock();
na->logProfileAqIC( logfile, mtp->ct, mtp->cTau/(365*86400), mtp->nC, evrt );
na->logProfilePhMol( ph_file, mtp->ct, mtp->cTau/(365*86400), mtp->nC, evrt );
t_out2 = clock();
outp_time += ( t_out2 - t_out);
         // if( mtp->rei5 )
          // break;


     } while ( mtp->cTau < mtp->Tau[STOP_] || mtp->ct < mtp->ntM );

pVisor->CloseMessage();

t_end = clock();
double dtime = ( t_end- t_start );
double clc_sec = CLOCKS_PER_SEC;
fprintf( diffile,
  "\nTotal time of calculation %lg s;  Time of output %lg s;  Whole run time %lg s\n",
    (dtime-outp_time)/clc_sec,  outp_time/clc_sec, dtime/clc_sec );

fclose( logfile );
fclose( ph_file );
fclose( diffile );

}



// --------------------- end of m_gem2mtt.cpp ---------------------------

