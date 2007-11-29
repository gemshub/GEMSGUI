//-------------------------------------------------------------------
// $Id: main.cpp 792 2006-09-19 08:10:41Z gems $
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

#include <time.h>
#include <math.h>
#include "io_arrays.h"
#include "ms_unspace.h"
#include "gstring.h"

istream&
f_getline(istream& is, gstring& str, char delim);

//---------------------------------------------------------------------------
// Test of 1D advection (finite difference method provided by Dr. F.Enzmann,
// Uni Mainz) coupled with GEMIPM2K kernel (PSI) using the TNodeArray class.
// Finite difference calculations split over independent components
// (through bulk composition of aqueous phase).
// Experiments with smoothing terms on assigning differences to bulk composition
// of nodes

int main( int argc, char* argv[] )
 {
     int       RetC = 0;
     gstring unspace_in1 = "unspace_init.txt";
     gstring chbr_in1 = "ipmfiles-dat.lst";

// from argv
      //if (argc >= 2 )
      //  multu_in1 = argv[1];
      if (argc >= 2 )
       chbr_in1 = argv[1];
      if (argc >= 3 )
    	  unspace_in1 = argv[2];

   try{
/*
// The NodeArray must be allocated here
    TGEM2MT::pm = new TGEM2MT();

// Here we read the GEM2MT structure, prepared from GEMS or by hand
   if( TGEM2MT::pm->MassTransSetUp( gem2mt_in1.c_str() ))
     return 1;  // error reading files

// Here we read the MULTI structure, DATACH and DATABR files prepared from GEMS
    if( TGEM2MT::pm->MassTransInit( chbr_in1.c_str() ) )
      return 1;  // error reading files

// here we call the mass-transport finite-difference coupled routine
   RetC = TGEM2MT::pm->Trans1D( NEED_GEM_AIA );
*/   }
   catch(TError& err)
       {
        fstream f_log("ipmlog.txt", ios::out|ios::app );
        f_log << err.title.c_str() << ": " << err.mess.c_str() << endl;
        return 1;
       }

   return RetC;
}


//---------------------------------------------------------------------------

