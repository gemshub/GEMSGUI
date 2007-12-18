//-------------------------------------------------------------------
// $Id$
//
// Declaration of TInteg class, config and calculation functions
//
// Rewritten from C to C++ by S.Dmytriyeva
// Copyright (C) 1995-2001 S.Dmytriyeva, D.Kulik
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

#ifndef _nteg_h_
#define _nteg_h_

#include "v_module.h"
#include "v_ipnc.h"

const int
MAXITER = 800, // max number steps
          MAXINTEGNAME = 70,
                         INT_BLOK =80;

// The task solution of system equations
class TInteg : public TCModule
{
    char keywd[24];             // task key

    // inital parameters of task
    char name[2][MAXINTEGNAME]; // task name
    int  Nequ;      //  number equations in system
    double x_bg,    //  span of integration
    x_end,
    step_bg, //  inital value of step
    Eps;     //  epsilon of solution
    double *param;  // work paramters of task
    double * y_bg;  // inital values
    char * TxtEqu;  // text of equations

    // working paraters
    double step;   // current step
    bool FlagGraf; // grafical or text mode
    int  MaxIter,  // max number of iterations
    nfcn,      // number of functional estimates
    nstep,     // number of steps
    naccept,   // number of permissible steps
    nrejct;    // number of unpermissible steps
    double arg_x,    // current value of argument
    * val_y,    // current value of functins
    * valdy;    // current value of differential
    int  Bsize;    // size of allx, ally, allst, allpr
    double  * allx,
    * ally,
    *allst;
    short   *allpr;

    //
    IPNCalc rpn;

protected:

    void Solut( int ix, int id, int it, double *t1, double *t2, double t3 );
    void MIDEX( int j, double t, double h, int ix, int id, int it );
    void PR( int Ni,int pr, double x, double step, double *y );
    void INTEG( int iy, int id, int ix, double eps,
                double& step, double t_begin, double t_end );

public:

    static TInteg* pm;

    TInteg( int nrt );
    const char* GetName() const
    {
        return "Integ";
    }

    void ods_link( int i=0);
    void dyn_set( int i=0);
    void dyn_kill( int i=0);
    void dyn_new( int i=0);
    void set_def( int i=0);

    void RecInput( const char *key );
    int RecBuild( const char *key, int mode = VF_UNDEF );
    void RecCalc( const char *key );
    void RecordPlot( const char */*key*/ );

    void CmHelp();
 //  virtual gstring  GetKeyofRecord( const char *oldKey, const char *strTitle,
 //      int keyType );

};

#endif  // _integ_h

outField TGEM2MT_static_fields[19] =  {
 { "Mode", 1,0 },
 { "PvFDL", 0,0 },  // PvPGD, PvFDL, PvSFL
 { "PvGrid", 0,0 },
 { "Size" , 1,0 },
 { "nFD" , 0,0 },
 { "nPG" , 0,0 },
 { "nSFD" , 0,0 },
 { "MaxSteps", 1,0 },
 { "nPTypes", 0,0 },
 { "nProps", 0,0 },
 { "Tau", 1,0 },
 { "LSize", 0,0 },
 { "fVel", 1,0 },
 { "cLen", 1,0 },
 { "tf", 1,0 },
 { "cdv", 1,0 },
 { "cez", 1,0 },
 { "al_in", 1,0 },
 { "Dif_in", 1,0 }
 };

outField TGEM2MT_dynamic_fields[16] =  {
 { "DiCp", 1, 0 },
 { "HydP", 1, 0 },
 { "NPmean", 0, 0 },
 { "NPmin", 0, 0 },
 { "NPmax", 0, 0 },
 { "ParTD", 0, 0 },
 { "mGrid", 0, 0 },
 { "FDLi", 0, 0 },
 { "FDLf", 0, 0 },
 { "BSF", 0, 0 },
 { "PGT", 0, 0 },
 { "UMPG", 0, 0 },
 { "FDLid", 0, 0 },
 { "FDLop", 0, 0 },
 { "FDLmp", 0, 0 },
 { "MPGid", 0, 0 }
 };

//====================================================================
extern bool _comment;

void TGEM2MT::to_text_file( fstream& ff, bool with_comments )
{
  _comment = with_comments;
  
  TPrintArrays  prar(ff);

   if( _comment )
   {  ff << "# GEMIPM2K v. 2.2.0" << endl;
      ff << "# Prototype 04.12.2007" << endl;
      ff << "# Comments can be marked with # $ ;" << endl << endl;
      ff << "# Template for the Gem2mt data" << endl;
      ff << "# (should be read only after the DATACH, the IPM-DAT and DATABR files)" << endl << endl;
      ff << "#Section (scalar): Controls and dimensionalities of the Gem2mt operation" << endl;
   }
   if( _comment )
      ff << "# Code of GEM2MT mode of operation { S F A D T }" << endl;
   ff << left << setw(17) << "<Mode> " << "\'"<<  mtp->PsMode << "\'"<< endl;
   if( _comment )
        ff << "# numbers of nodes along x, y, z coordinates";
   ff << left << setw(7) << "<Size> " <<   mtp->nC << " 1" << " 1" << endl;
   if( _comment )
       ff << "# Maximum allowed number of time iteration steps (default 1000)" << endl;
   ff << left << setw(7) << "<MaxSteps> " <<   mtp->ntM << endl;
   if( _comment )
        ff << "# Physical time iterator";
   prar.writeArray(  "Tau", mtp->Tau, 3 );
   ff << endl;
   
   if( mtp->PsMode == GMT_MODE_F )
   {  if( _comment )
       ff << "# Use phase groups definitions, flux definition list & source fluxes and elemental stoichiometries";
      prar.writeArray(  "PvFDL", mtp->PvPGD, 3, 1 );
      if( _comment )
        ff << "#  number of MPG flux definitions (0 or >1)" <<  endl;
      ff << left << setw(7) << "<nFD> " <<   mtp->nFD << endl;
      if( _comment )
        ff << "# number of mobile phase groups (0 or >1)" << endl;
      ff << left << setw(7) << "<nPG> " << mtp->nPG <<  endl;
      if( _comment )
        ff << "# number of source flux definitions (0 or < nFD )" << endl;
      ff << left << setw(7) << "<nSFD> " << mtp->nSFD <<  endl;
   }     

   if( mtp->PsMode == GMT_MODE_W || mtp->PsMode == GMT_MODE_V )
   {  if( _comment )
       ff << "# Use array of grid point locations? (+ -)" << endl;
       ff << left << setw(17) << "<PvGrid> " << "\'"<<  mtp->PvGrid << "\'"<< endl;
       if( _comment )
        ff << "# res Number of allocated particle types (< 20 ? )" << endl;
       ff << left << setw(7) << "<nPTypes> " << mtp->nSFD <<  endl;
       if( _comment )
         ff << "# res Number of particle statistic properties (for monitoring) >= anPTypes" << endl;
       ff << left << setw(7) << "<nProps> " << mtp->nProps <<  endl;
       if( _comment )
         ff << "# spatial dimensions of the medium defines topology of nodes";
      prar.writeArray(  "LSize", mtp->sizeLc, 3 );
      ff << endl;
   }     

   if( _comment )
    ff << "# Advection/diffusion mass transport: initial fluid advection velocity (m/sec)" << endl;
   ff << left << setw(7) << "<fVel> " << mtp->fVel <<  endl;
   if( _comment )
    ff << "# column length (m)" << endl;
   ff << left << setw(7) << "<cLen> " << mtp->cLen <<  endl;
   if( _comment )
    ff << "# time step reduction factor" << endl;
   ff << left << setw(7) << "<tf> " << mtp->tf <<  endl;
   if( _comment )
    ff << "# cutoff factor for differences (1e-9)" << endl;
   ff << left << setw(7) << "<cdv> " << mtp->cdv <<  endl;
   if( _comment )
    ff << "# cutoff factor for minimal amounts of IC in node bulk compositions (1e-12)" << endl;
   ff << left << setw(7) << "<cez> " << mtp->cez <<  endl;
   if( _comment )
    ff << "# initial value of longitudinal dispersivity (m), usually 1e-3" << endl;
   ff << left << setw(7) << "<al_in> " << mtp->al_in <<  endl;
   if( _comment )
    ff << "# initial general diffusivity (m2/sec), usually 1e-9" << endl;
   ff << left << setw(7) << "<Dif_in> " << mtp->Dif_in <<  endl;
        
   ff<< "\n<END_DIM>\n";

 // dynamic arrays - must follow static data
   if( _comment )
   {   ff << "\n## Task configuration section ";
       ff << "\n#  Array of indexes of initial system variants for distributing to nodes";
   }
   prar.writeArray(  "DiCp", mtp->DiCp[0], mtp->nC*2, 2);
   if( _comment )
    ff << "\n# Hydraulic parameters for nodes in mass transport model";
   prar.writeArray(  "HydP", mtp->HydP[0], mtp->nC*SIZE_HYDP, SIZE_HYDP);
       
   if( mtp->PsMode == GMT_MODE_W || mtp->PsMode == GMT_MODE_V )
   {  if( _comment )
       ff << "\n# Array of initial mean particle type numbers per node";
       prar.writeArray(  "NPmean", mtp->NPmean, mtp->nPTypes );
       if( _comment )
        ff << "\n# Minimum average total number of particles of each type per one node";
       prar.writeArray(  "nPmin", mtp->nPmin, mtp->nPTypes );
        if( _comment )
         ff << "\n# Maximum average total number of particles of each type per one node";
       prar.writeArray(  "nPmax", mtp->nPmax, mtp->nPTypes );
       if( _comment )
         ff << "\n# Array of particle type definitions at t0 or after interruption";
       prar.writeArray(  "ParTD", mtp->ParTD[0], mtp->nPTypes*6, 6 );
      if( mtp->PvGrid != S_OFF )
      {
          if( _comment )
            ff << "\n# Array of grid point locations";
          prar.writeArray(  "grid", mtp->grid[0], mtp->nC*3, 3 );
      }
   }     
   if( mtp->PsMode == GMT_MODE_F )
   {  
    if( mtp->PvFDL != S_OFF )
    {
 	   if( _comment )
        ff << "\n# Indexes of nodes where this flux begins and ends";
        prar.writeArray(  "FDLi", mtp->FDLi[0], mtp->nFD*2,2 );
 	   if( _comment )
        ff << "\n# Part of the flux defnition list (flux order, flux rate, MPG quantities)";
        prar.writeArray(  "FDLf", mtp->FDLf[0], mtp->nFD*4, 4 );
 	   if( _comment )
        ff << "\n# ID of fluxes";
        prar.writeArray(  "FDLid", mtp->FDLid[0], mtp->nFD, MAXSYMB );
 	   if( _comment )
        ff << "\n# Operation codes (letters) flux type codes";
        prar.writeArray(  "FDLop", mtp->FDLop[0],  mtp->nFD, MAXSYMB  );
 	   if( _comment )
        ff << "\n# ID of MPG to move in this flux";
        prar.writeArray(  "FDLmp", mtp->FDLmp[0], mtp->nFD, MAXSYMB  );
    }
	if( mtp->PvPGD != S_OFF )
	{
	  if( _comment )
        ff << "\n# Units for setting phase quantities in MPG";
      prar.writeArray(  "UMPG", mtp->UMPG, mtp->FIf );
	  if( _comment )
         ff << "\n# Quantities of phases in MPG ";
	  prar.writeArray(  "PGT", mtp->PGT, mtp->FIf*mtp->nPG, mtp->nPG );
	  if( _comment )
	     ff << "\n# ID list of mobile phase groups";
	  prar.writeArray(  "MPGid", mtp->MPGid[0], mtp->nPG, MAXSYMB );
	}
   if( mtp->PvSFL != S_OFF )
   {
	  if( _comment )
	     ff << "\n# Table of bulk compositions of source fluxes";
      prar.writeArray(  "BSF", mtp->BSF, mtp->nSFD*mtp->Nf, mtp->Nf );
   }
 }     
   if( _comment )
     ff << "\n# End of file"<< endl;
 }


// Reading TGEM2MT structure from text file
void TGEM2MT::from_text_file(fstream& ff)
{
// static arrays
 TReadArrays  rdar( 19, TGEM2MT_static_fields, ff);
 short nfild = rdar.findNext();
 while( nfild >=0 )
 {
   switch( nfild )
   {
   case 0: rdar.readArray( "Mode", &mtp->PsMode, 1, 1);
           break;
   case 1: rdar.readArray( "PvFDL", &mtp->PvPGD, 3, 1);
           break;
   case 2: rdar.readArray( "PvGrid", &mtp->PvGrid, 1, 1);
           break;
   case 3: rdar.readArray( "Size", &mtp->xC, 3);
            mtp->nC = mtp->xC*mtp->yC*mtp->zC;
           break;
   case 4: rdar.readArray( "nFD", &mtp->nFD, 1);
           break;
   case 5: rdar.readArray( "nPG", &mtp->nPG, 1);
           break;
   case 6: rdar.readArray( "nSFD", &mtp->nSFD,  1);
           break;
   case 7: rdar.readArray( "MaxSteps", &mtp->ntM, 1);
           break;
   case 8: rdar.readArray( "nPTypes", &mtp->nPTypes,  1);
           break;
   case 9: rdar.readArray( "nProps", &mtp->nProps,  1);
           break;
   case 10: rdar.readArray( "Tau", mtp->Tau, 3);
           break;
   case 11: rdar.readArray( "LSize", mtp->sizeLc, 3);
           break;
   case 12: rdar.readArray( "fVel", &mtp->fVel, 1);
        break;
   case 13: rdar.readArray( "cLen", &mtp->cLen, 1);
         break;
   case 14: rdar.readArray( "tf", &mtp->tf, 1);
        break;
   case 15: rdar.readArray( "cdv", &mtp->cdv, 1);
        break;
   case 16: rdar.readArray( "cez", &mtp->cez, 1);
        break;
   case 17: rdar.readArray( "al_in", &mtp->al_in, 1);
        break;
   case 18: rdar.readArray( "Dif_in", &mtp->Dif_in, 1);
        break;
 }
   nfild = rdar.findNext();
 }

 if( mtp->PsMode == GMT_MODE_F )
 { 
    rdar.setAlws( 1 /*"PvFDL"*/);
    rdar.setAlws( 4 /*"nFD"*/);
    rdar.setAlws( 5 /*"nPG"*/);
    rdar.setAlws( 6 /*"nSFD"*/);
 }     

 if( mtp->PsMode == GMT_MODE_W || mtp->PsMode == GMT_MODE_V )
 {  
    rdar.setAlws( 2 /*"PvGrid"*/);
    rdar.setAlws( 8 /*"nPTypes"*/);
    rdar.setAlws( 9 /*"nProps"*/);
    rdar.setAlws( 11 /*"LSize"*/);
 }     
 // setup default
 mtp->Nf =  mtp->nICb = TNodeArray::na->pCSD()->nICb;
 mtp->FIf = mtp->nPHb = TNodeArray::na->pCSD()->nPHb;
 mtp->nDCb =TNodeArray::na->pCSD()->nDCb;
 
 // testing read
 gstring ret = rdar.testRead();
 if( !ret.empty() )
  { ret += " - fields must be readed from TGEM2MT structure";
    Error( "Error", ret);
  }

  Alloc();

//dynamic data
 TReadArrays  rddar( 16, TGEM2MT_dynamic_fields, ff);

// Set up flags
 if( mtp->PsMode == GMT_MODE_W || mtp->PsMode == GMT_MODE_V )
 { 
   rddar.setAlws( 2 /*"NPmean"*/);
   rddar.setAlws( 3 /*"nPmin"*/);
   rddar.setAlws( 4 /*"nPmax"*/);
   rddar.setAlws( 5 /*"ParTD"*/);
   if( mtp->PvGrid != S_OFF )
	    rddar.setAlws( 6 /*"grid"*/);
 }     
 if( mtp->PsMode == GMT_MODE_F )
 {  
   if( mtp->PvFDL != S_OFF )
   {
	   rddar.setAlws( 7 /*"FDLi"*/);
	   rddar.setAlws( 8 /*"FDLf"*/);
	   rddar.setAlws( 12 /*"FDLid"*/);
	   rddar.setAlws( 13 /*"FDLop"*/);
	   rddar.setAlws( 14 /*"FDLmp"*/);
   }
  if( mtp->PvPGD != S_OFF )
  {
		   rddar.setAlws( 11 /*"UMPG"*/);
		   rddar.setAlws( 10 /*"PGT"*/);
		   rddar.setAlws( 15 /*"MPGid"*/);
  }
  if( mtp->PvSFL != S_OFF )
	   rddar.setAlws( 9 /*"BSF"*/);
 }     

 nfild = rddar.findNext();
 while( nfild >=0 )
 {
   switch( nfild )
   {
   case 0: rddar.readArray( "DiCp", mtp->DiCp[0], mtp->nC*2 );
            break;
   case 1: rddar.readArray( "HydP", mtp->HydP[0], mtp->nC*SIZE_HYDP );
            break;
   case 2: rddar.readArray( "NPmean", mtp->NPmean, mtp->nPTypes );
            break;
   case 3: rddar.readArray( "nPmin", mtp->nPmin, mtp->nPTypes );
            break;
   case 4: rddar.readArray( "nPmax", mtp->nPmax, mtp->nPTypes );
            break;
   case 5: rddar.readArray( "ParTD", mtp->ParTD[0], mtp->nPTypes*6 );
             break;
   case 6: rddar.readArray(  "grid", mtp->grid[0], mtp->nC*3 );
             break;
   case 7: rddar.readArray( "FDLi", mtp->FDLi[0], mtp->nFD*2 );
             break;
   case 8: rddar.readArray( "FDLf", mtp->FDLf[0], mtp->nFD*4 );
             break;
   case 9: rddar.readArray(  "BSF", mtp->BSF, mtp->nSFD*mtp->Nf );
             break;
   case 10: rddar.readArray(  "PGT", mtp->PGT, mtp->FIf*mtp->nPG );
             break;
   case 11: rddar.readArray(  "UMPG", mtp->UMPG, mtp->FIf );
            break;
   case 12: rddar.readArray(  "FDLid", mtp->FDLid[0], mtp->nFD, MAXSYMB );
            break;
   case 13: rddar.readArray(  "FDLop", mtp->FDLop[0],  mtp->nFD, MAXSYMB  );
            break;
   case 14:rddar.readArray(  "FDLmp", mtp->FDLmp[0], mtp->nFD, MAXSYMB  );
            break;
   case 15:rddar.readArray(  "MPGid", mtp->MPGid[0], mtp->nPG, MAXSYMB );
            break;
  }
     nfild = rddar.findNext();
 }
 
 // testing read
 ret = rddar.testRead();
 if( !ret.empty() )
  { ret += " - fields must be read from TUnSpace structure";
    Error( "Error", ret);
  }

}
