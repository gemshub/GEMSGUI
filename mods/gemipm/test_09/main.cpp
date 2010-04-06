//--------------------------------------------------------------------
// $Id: main.cpp 1384 2009-08-06 08:39:43Z gems $
//
// Demo test of usage of the TNode class for implementing a simple
// batch-like calculation of equilibria using text file input and
// GEMIPM2 numerical kernel

// TNode class implements a  simple C/C++ interface of GEMIPM2K.
// It works with DATACH and work DATABR structures and respective
// DCH (chemical system definition) and DBR (recipe or data bridge)
// data files. In addition, the program reads an IPM inlut file which 
// can be used for tuning up numerical controls of GEM IPM2 algorithm 
// and for setting up the parameters of non-ideal mixing models.
//
// Copyright (C) 2007,2008 D.Kulik, S.Dmitrieva
//
// This file is part of GEMIPM2K code for thermodynamic modelling
// by Gibbs energy minimization

// This file may be distributed under the licence terms defined
// in GEMIPM2K.QAL
//
// See also http://gems.web.psi.ch
// mailto://gems2.support@psi.ch
//-------------------------------------------------------------------

#include <time.h>
#include <math.h>
#include <string.h>

#include "node.h"
#include <iomanip>


//The simplest case: data exchange using disk files only
int main( int argc, char* argv[] )
 {
   long nRecipes = 0;
   char (*recipes)[fileNameLength] = 0;
   
   // Analyzing command line arguments
   // Default arguments
   char input_system_file_list_name[256] = "system-dat.lst";
   char input_recipes_file_list_name[256] = "more_recipes.lst";
   
   if (argc >= 2 )
       strncpy( input_system_file_list_name, argv[1], 256);
   // list of DCH, IPM and DBR input files for initializing GEMIPM2K
   
   // Creates TNode structure instance accessible trough the "node" pointer
   TNode* node  = new TNode();
  
   // (1) Initialization of GEMIPM2K internal data by reading  files
   //     whose names are given in the input_system_file_list_name
  if( node->GEM_init( input_system_file_list_name ) )
  {
      // error occured during reading the files
      return 1;
  }

   // Getting direct access to work node DATABR structure which exchanges the
   // data with GEM IPM2 (already filled out by reading the DBR input file)
   DATABR* dBR = node->pCNode(); 

  // test internal functions
   long int xCa = node->IC_name_to_xCH("Ca");
   long int xCa_ion = node->DC_name_to_xCH("Ca+2");
   long int xCal = node->DC_name_to_xCH("Cal");
   long int xaq = node->Ph_name_to_xCH("aq_gen");
   long int xCalcite = node->Ph_name_to_xCH("Calcite");
   long int xbCa = node->IC_xCH_to_xDB(xCa);
   long int xbCa_ion = node->DC_xCH_to_xDB(xCa_ion);
   long int xbCal = node->DC_xCH_to_xDB(xCal);
   long int xbaq = node->Ph_xCH_to_xDB(xaq);
   long int xbCalcite = node->Ph_xCH_to_xDB(xCalcite);
   
   cout << "          CH  BR" << endl;
   cout << " Ca       " << xCa << "   " << xbCa << endl;
   cout << " Ca+2     " << xCa_ion << "   " << xbCa_ion << endl;
   cout << " Cal     " << xCal << "  " << xbCal << endl;
   cout << " aq_gen   " << xaq << "   " << xbaq << endl;
   cout << " Calcite  " << xCalcite << "   " << xbCalcite << endl;
   cout << setprecision(7) << setw(10) << endl;
   
   // Asking GEM to run with automatic initial approximation 
   dBR->NodeStatusCH = NEED_GEM_AIA;

   // (2) re-calculating equilibrium by calling GEMIPM2K, getting the status back
   int NodeStatusCH = node->GEM_run( 3., false );

   if( NodeStatusCH == OK_GEM_AIA || NodeStatusCH == OK_GEM_SIA  )
   {    // (3) Writing results in default DBR file
       node->GEM_write_dbr( NULL, false, true );
       node->GEM_print_ipm( NULL );   // possible debugging printout
   }
   else {
      // (4) possible return status analysis, error message
       node->GEM_print_ipm( NULL );   // possible debugging printout
       return 5; // GEM IPM did not converge properly      //?????
        }

   // test internal functions
  cout << "Ph_Volume   Aq: " << node->Ph_Volume(xbaq) <<  " Calcite: " << node->Ph_Volume(xbCalcite) << endl;   
  cout << "Ph_Mass     Aq: " << node->Ph_Mass(xbaq) <<  " Calcite: " << node->Ph_Mass(xbCalcite) << endl;   
  cout << "Ph_SatInd   Aq: " << node->Ph_SatInd(xbaq) <<  " Calcite: " << node->Ph_SatInd(xbCalcite) << endl;   
   
  cout << endl;
  cout << "Ca+2    Get_nDC  " << node->Get_nDC(xbCa_ion) <<  " DC_n  " << node->DC_n(xCa_ion) << endl;   
  cout << "Cal     Get_nDC  " << node->Get_nDC(xbCal) <<  " DC_n  " << node->DC_n(xCal) << endl;   
  cout << "Ca+2    Get_muDC " << node->Get_muDC(xbCa_ion) <<  " DC_mu " << node->DC_mu(xCa_ion) << endl;   
  cout << "Cal     Get_muDC " << node->Get_muDC(xbCal) <<  " DC_mu " << node->DC_mu(xCal) << endl;   
  cout << "Ca+2    Get_aDC  " << node->Get_aDC(xbCa_ion) <<  " DC_a  " << node->DC_a(xCa_ion) << endl;   
  cout << "Cal     Get_aDC  " << node->Get_aDC(xbCal) <<  " DC_a  " << node->DC_a(xCal) << endl;   
  cout << "Ca+2    Get_cDC  " << node->Get_cDC(xbCa_ion) <<  " DC_c  " << node->DC_c(xCa_ion) << endl;   
  cout << "Cal     Get_cDC  " << node->Get_cDC(xbCal) <<  " DC_c  " << node->DC_c(xCal) << endl;   
  cout << "Ca+2    Get_gDC  " << node->Get_gDC(xbCa_ion) <<  " DC_g  " << node->DC_g(xCa_ion) << endl;   
  cout << "Cal     Get_gDC  " << node->Get_gDC(xbCal) <<  " DC_g  " << node->DC_g(xCal) << endl;   
  cout << endl;
  cout << "G0   Ca+2: " << node->DC_G0( xCa_ion, node->cP(), node->cTK(), false ) <<  " Cal: " << node->DC_G0( xCal, node->cP(), node->cTK(), false ) << endl;   
  cout << "V0   Ca+2: " << node->DC_V0( xCa_ion, node->cP(), node->cTK() ) <<  " Cal: " << node->DC_V0( xCal, node->cP(), node->cTK() ) << endl;   
  cout << "H0   Ca+2: " << node->DC_H0( xCa_ion, node->cP(), node->cTK() ) <<  " Cal: " << node->DC_H0( xCal, node->cP(), node->cTK() ) << endl;   
  cout << "S0   Ca+2: " << node->DC_S0( xCa_ion, node->cP(), node->cTK() ) <<  " Cal: " << node->DC_S0( xCal, node->cP(), node->cTK() ) << endl;   
  cout << "Cp0  Ca+2: " << node->DC_Cp0( xCa_ion, node->cP(), node->cTK() ) <<  " Cal: " << node->DC_Cp0( xCal, node->cP(), node->cTK() ) << endl;   
  cout << "A0   Ca+2: " << node->DC_A0( xCa_ion, node->cP(), node->cTK() ) <<  " Cal: " << node->DC_A0( xCal, node->cP(), node->cTK() ) << endl;   
  cout << "U0   Ca+2: " << node->DC_U0( xCa_ion, node->cP(), node->cTK() ) <<  " Cal: " << node->DC_U0( xCal, node->cP(), node->cTK() ) << endl;   
   
   // Here a possible loop on input recipes begins
   if (argc >= 3 )
   {  
 	  char NextRecipeFileName[256];
 	  char NextRecipeOutFileName[300];
 	  char input_recipes_file_list_path[256-fileNameLength] = "";
     
 	  strncpy( input_recipes_file_list_name, argv[2], 256);
      // list of additional recipes (dbr.dat files) e.g. for simulation
      // of a titration or another irreversible process
  	  // Reading list of recipes names from file 
  	  recipes = f_getfiles(  input_recipes_file_list_name, 
  	       		 input_recipes_file_list_path, nRecipes, ',');			 
  	  
  	 for(int cRecipe=0; cRecipe < nRecipes; cRecipe++ )
      { 
         // Trying to read the next file name 
  	    sprintf(NextRecipeFileName , "%s%s", input_recipes_file_list_path, recipes[cRecipe] );
  
        // (5) Reading the next DBR file with different input composition or temperature
        node->GEM_read_dbr( NextRecipeFileName );

        // Asking GEM IPM2 to run (faster) with smart initial approximation 
        dBR->NodeStatusCH = NEED_GEM_SIA;       

        NodeStatusCH = node->GEM_run( 3., false );

        if( NodeStatusCH == OK_GEM_AIA || NodeStatusCH == OK_GEM_SIA  )
        {    sprintf(NextRecipeOutFileName , "%s.out", NextRecipeFileName );
        	 node->GEM_write_dbr( NextRecipeOutFileName, false, true );
sprintf(NextRecipeOutFileName , "%s.Dump.out", NextRecipeFileName );
node->GEM_print_ipm( NextRecipeOutFileName );
        }
        else {
               // error message, debugging printout
     	      sprintf(NextRecipeOutFileName , "%s.Dump.out", NextRecipeFileName );
              node->GEM_print_ipm( NextRecipeOutFileName );
//??              return 5; // GEM IPM did not converge properly
              }

      }
   }	 
  	 // end of possible loop on input recipes
   delete node;
   if( recipes ) delete recipes;

 // End of example  
   return 0; 
 }  
   

//---------------------------------------------------------------------------
// end of main.cpp for TNode class usage - GEM single calculation example
