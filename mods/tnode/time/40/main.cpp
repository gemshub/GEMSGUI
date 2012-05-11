//--------------------------------------------------------------------
// $Id: main.cpp 922 2007-09-14 08:06:21Z gems $
//
// Demo test of usage of the TNode class for implementing a simple
// direct coupling scheme between FMT and GEM in a single-GEM-call
// fashion, assuming that the chemical speciation and all dynamic
// parameter data are kept in the FMT part, which calls GEMIPM
// calculation once per node.

// TNode class implements a  simple C/C++ interface between GEM IPM
// and FMT codes. Works with DATACH and work DATABR structures
// without using the TNodearray class
//
// Copyright (C) 2006 S.Dmytriyeva, D.Kulik
//
// This file is part of GEMIPM2K code for thermodynamic modelling
// by Gibbs energy minimization

// This file may be distributed under the licence terms defined
// in GEMIPM2K.QAL
//
// See also http://gems.web.psi.ch/
// mailto://gems2.support@psi.ch
//-------------------------------------------------------------------

#include <time.h>
#include <math.h>
#include <string.h>

#include "node.h"

// #define nNodes  5   // set here how many nodes you need

int main( int argc, char* argv[] )
 {
   // Analyzing command line arguments
     // Default arguments
     char ipm_input_file_list_name[256] = "chemsys.lst";
     char dbr_input_file_name[256] = "chemsys-dbr.dat";
     char fmt_input_file_name[256] = "fmtparam.dat";

     if (argc >= 2 )
       strncpy( ipm_input_file_list_name, argv[1], 256);
         // list of files needed as input for initializing GEMIPM2K
     if (argc >= 3 )
           strncpy( dbr_input_file_name, argv[2], 256);
             // input file for boundary conditions
     if (argc >= 4 )
       strncpy( fmt_input_file_name, argv[3], 256);
         // your optional file with FMT input parameters

   // Creating TNode structure accessible trough node pointer
   TNode* node  = new TNode();

   // Here we read the files needed as input for initializing GEMIPM2K
   // The easiest way to prepare them is to use GEMS-PSI code (GEM2MT module)
   if( node->GEM_init( ipm_input_file_list_name ) )
       return 1;  // error occured during reading the files

   int nNodes = 5;     // number of local equilibrium nodes, 1 or more
   int nTimes = 100;   // Maximum number of time iteration steps
   unsigned int nTotIt;   // Number of GEM iterations per time step
   double t_start = 0., t_end = 10000., dt = 100., tc = 1.;
   double CalcTime = 0.0;
   double MeanIt;      // mean number of GEM iterations at time step

   cout << "Start of gemnode AIA test: " << ipm_input_file_list_name << " "
         << dbr_input_file_name << endl;
   cout << " nNodes = " << nNodes << "  nTimes = " << nTimes
         << "  t_start = " << t_start << " t_end = " << t_end
         << "  dt = " << dt << "  tc = " << tc << endl;

   // allocations and defaults for other FMT parameters can be added here

   // Here you can read your file with some FMT parameters and initial data
   // if( my_fmt_input(fmt_input_file_name) )
   //   return 2;

   // Number of ICs, DCs, Phases and Phases-solutions kept in the node
   // DATABR structure for exchange with GEMIPM - for your convenience
   int nIC, nDC, nPH, nPS;
//   int i,   j,   k,   ks;    // indices for direct access to components
                               // and phases data in the DataCH framework

   // Getting direct access to DataCH structure in GEMIPM2K memory
   DATACH* dCH = node->pCSD();
   if( !dCH  )
       return 3;

   // Getting direct access to work node DATABR structure which
   // exchanges data between GEMIPM and FMT parts
   DATABR* dBR = node->pCNode();
   if( !dBR  )
       return 4;

   // Extracting data bridge array sizes
   nIC = dCH->nICb;
   nDC = dCH->nDCb;
   nPH = dCH->nPHb;
   nPS = dCH->nPSb;

   // Allocating work memory for FMT part (here only chemical variables)
   // for nNodes (a few) nodes (real FMT problems consider many nodes).
   // Names of arrays are taken consistent with the DataBridge structure
   //  (see "databr.h") for better readability

   short m_NodeHandle[nNodes], m_NodeStatusCH[nNodes], m_IterDone[nNodes];

   double m_T[nNodes], m_P[nNodes], m_Vs[nNodes], m_Ms[nNodes],
          m_Gs[nNodes], m_Hs[nNodes], m_IC[nNodes], m_pH[nNodes], m_pe[nNodes],
          m_Eh[nNodes];

   double *m_xDC, *m_gam, *m_xPH, *m_aPH, *m_vPS, *m_mPS,*m_bPS,
         *m_xPA, *m_dul, *m_dll, *m_bIC, *m_rMB, *m_uIC;

   m_bIC = (double*)malloc( nNodes*nIC*sizeof(double) );
   m_rMB = (double*)malloc( nNodes*nIC*sizeof(double) );
   m_uIC = (double*)malloc( nNodes*nIC*sizeof(double) );
   m_xDC = (double*)malloc( nNodes*nDC*sizeof(double) );
   m_gam = (double*)malloc( nNodes*nDC*sizeof(double) );
   m_dul = (double*)malloc( nNodes*nDC*sizeof(double) );
   m_dll = (double*)malloc( nNodes*nDC*sizeof(double) );
   m_aPH = (double*)malloc( nNodes*nPH*sizeof(double) );
   m_xPH = (double*)malloc( nNodes*nPH*sizeof(double) );
   m_vPS = (double*)malloc( nNodes*nPS*sizeof(double) );
   m_mPS = (double*)malloc( nNodes*nPS*sizeof(double) );
   m_bPS = (double*)malloc( nNodes*nIC*nPS*sizeof(double) );
   m_xPA = (double*)malloc( nNodes*nPS*sizeof(double) );

   // (1) ---------------------------------------------
   // Initialization of chemical data kept in the FMT part.
   // Can be done in a loop over nodes, if there are many nodes
//   cout << "Begin Initialisation Part" << endl;
   int in;
   for(  in=1; in<nNodes; in++ )  // Node with in=0 will be initialized later
   {
     dBR->NodeStatusCH = NEED_GEM_AIA; // direct access to node DATABR structure

     // re-calculating equilibrium by calling GEMIPM2K, getting the status
     m_NodeStatusCH[in] = node->GEM_run();

     if( !( m_NodeStatusCH[in] == OK_GEM_AIA || m_NodeStatusCH[in] == OK_GEM_PIA ) )
        return 5; // GEM IPM did not converge properly

      // Extracting GEM IPM input chemical data into FMT part
     node->GEM_restore_MT( m_NodeHandle[in], m_NodeStatusCH[in],
       m_T[in], m_P[in], m_Vs[in], m_Ms[in],
       m_bIC+in*nIC, m_dul+in*nDC, m_dll+in*nDC, m_aPH+in*nPH );
       // Extracting GEM IPM output chemical data into FMT part
     node->GEM_to_MT( m_NodeHandle[in], m_NodeStatusCH[in], m_IterDone[in],
       m_Vs[in], m_Ms[in], m_Gs[in], m_Hs[in], m_IC[in], m_pH[in], m_pe[in],
       m_Eh[in], m_rMB+in*nIC, m_uIC+in*nIC, m_xDC+in*nDC, m_gam+in*nDC,
       m_xPH+in*nPH, m_vPS+in*nPS, m_mPS+in*nPS,
       m_bPS+in*nIC*nPS, m_xPA+in*nPS );

//  Uncomment this to test variable pressures and temperatures
//         m_T[in] += in*5;
//         m_P[in] += (in-1)*20;
//         m_T[in] += in*7;
//         m_P[in] += (in-1)*20;
     // Here the file output for the initial conditions can be implemented
   }

    // Read DATABR structure from text file separately (boundary condition)
      TNode::na->GEM_read_dbr( dbr_input_file_name );

  for(  in=0; in<1; in++ ) // Initialising boundary condition node(s)
  {
   dBR->NodeStatusCH = NEED_GEM_AIA; // activating GEM IPM for automatic initial
                                     // approximation
  // re-calculating equilibrium by calling GEMIPM
   m_NodeStatusCH[in] = node->GEM_run();

  if( !( m_NodeStatusCH[in] == OK_GEM_AIA || m_NodeStatusCH[in] == OK_GEM_PIA ) )
     return 5;
  // Extracting GEM IPM input chemical data into FMT part
   node->GEM_restore_MT( m_NodeHandle[in], m_NodeStatusCH[in], m_T[in],
    m_P[in], m_Vs[in], m_Ms[in],
    m_bIC+in*nIC, m_dul+in*nDC, m_dll+in*nDC, m_aPH+in*nPH );
     // Extracting GEM IPM output chemical data to FMT part
   node->GEM_to_MT( m_NodeHandle[in], m_NodeStatusCH[in], m_IterDone[in],
    m_Vs[in], m_Ms[in], m_Gs[in], m_Hs[in], m_IC[in], m_pH[in], m_pe[in],
    m_Eh[in], m_rMB+in*nIC, m_uIC+in*nIC, m_xDC+in*nDC, m_gam+in*nDC,
    m_xPH+in*nPH, m_vPS+in*nPS, m_mPS+in*nPS,
    m_bPS+in*nIC*nPS, m_xPA+in*nPS );

  // Here the file output for the initial boundary conditions can be implemented
 }

   cout << "End of gem_node test initialisation" << endl;
   clock_t t_start11, t_end11;
   t_start11 = clock();  // Setting time when the coupled modelling starts

   // (2) ----------------------------------------------
   // Work loop for the coupled FMT-GEM modelling

   cout << "Begin Coupled Modelling Part" << endl;
      // Getting DATABR indexes for chemical species to be monitored
   int xCa = node->DC_name_to_xDB("Ca+2");
   int xSr = node->DC_name_to_xDB("Sr+2");
   int xCl = node->DC_name_to_xDB("Cl-");
   int xiSr = node->IC_name_to_xDB("Sr");
   int xiCl = node->IC_name_to_xDB("Cl");
   int xCal = node->Ph_name_to_xDB("Calcite");
   int xStr = node->Ph_name_to_xDB("Strontianite");
   // Checking indexes
   cout << "xCa+2= " << xCa << " xSr+2=" << xSr << " xCl-=" << xCl << " xiSr=" << 
        xiSr  << " xCalcite=" << xCal << " xStrontianite=" << xStr << endl;
 nTotIt = 0;

   for( int it=0; it<nTimes; it++ )  // iterations over time
   {
     int in;
 //   cout << " FMT loop begins: " << endl;
     // Loop over nodes for calculating the mass transport step
         // ( this whole loop may be replaced by call(s) to FMT subroutines)
     for(  in=1; in<nNodes; in++ )
     {
       ; // Add here some operators as function of tc and dt
       // that transfer mass between nodes (i.e. change some elements of
       // m_bIC array). Take care about mass conservation and consistency
       // of chemical compositions of the nodes.
       // in this example, simply adding SrCl2 to m_bIC vector
       // in order to cause the conversion of calcite to strontianite
       if( it > 0 )
       {
         if( xiSr >= 0 )
           m_bIC[in*nIC+xiSr] += dt*in*5e-7;
         if( xiCl >= 0 )  
           m_bIC[in*nIC+xiCl] += dt*in*1e-6;
       }
       // Alternatively, the transport may be simulated using m_xDC arrays.
       // In this case, an alternative (overloaded) call to GEM_from_MT()
       // should be used (see below)
     }
//     cout << " FMT loop ends: ";
     cout << " it = " << it << "  dt = " << dt << "  tc = " << tc << endl;

//     cout << " Chemical loop begins: " << endl;
     // Loop over nodes for calculating the chemical equilibration step
     for( in=0; in<nNodes; in++ )
     {
        cout << "  in = " << in << "  T = " << m_T[in];

        m_NodeHandle[in] = in;
        m_NodeStatusCH[in] = NEED_GEM_AIA; 
//        m_NodeStatusCH[in] = NEED_GEM_PIA; 

        // Setting input data for GEM IPM
        node->GEM_from_MT( m_NodeHandle[in], m_NodeStatusCH[in],
             m_T[in], m_P[in], m_Vs[in], m_Ms[in],
             m_bIC+in*nIC, m_dul+in*nDC, m_dll+in*nDC, m_aPH+in*nPH );
//  Alternative call to correct bulk chemical composition using changed
//     m_xDC (chemical species amounts) data. Take care that this function
//     actually compresses the xDC values into mole amounts of elements
//     and adds them to the elements of bIC vector.
//   node->GEM_from_MT( m_NodeHandle[in], m_NodeStatusCH[in],
//     m_T[in], m_P[in], m_Vs[in], m_Ms[in],
//     m_bIC+in*nIC, m_dul+in*nDC, m_dll+in*nDC, m_aPH+in*nPH, m_xDC+in*nDC );

        // Calling GEM IPM calculation
        m_NodeStatusCH[in] = node->GEM_run( );
                if( !( m_NodeStatusCH[in] == OK_GEM_AIA ||
               m_NodeStatusCH[in] == OK_GEM_PIA ) )
            return 5;
CalcTime += node->GEM_CalcTime();  // Incrementing calculation time
        // Extracting GEM IPM output data to FMT part
        node->GEM_to_MT( m_NodeHandle[in], m_NodeStatusCH[in], m_IterDone[in],
          m_Vs[in], m_Ms[in], m_Gs[in], m_Hs[in], m_IC[in], m_pH[in], m_pe[in],
          m_Eh[in],m_rMB+in*nIC, m_uIC+in*nIC, m_xDC+in*nDC, m_gam+in*nDC,
          m_xPH+in*nPH, m_vPS+in*nPS, m_mPS+in*nPS,
          m_bPS+in*nIC*nPS, m_xPA+in*nPS  );

nTotIt += m_IterDone[in];
        // Here the debug print for each node can be implemented
//        cout << " Gem run ends: ";
        cout << " Cal= " << m_xPH[in*nPH+xCal] <<
                " Str= " << m_xPH[in*nPH+xStr];
        cout << " [Ca]= " << m_bPS[in*nIC*nPS+xCa] <<
                " [Sr]= " << m_bPS[in*nIC*nPS+xSr] <<
                " bSr= "  << m_bIC[in*nIC+xiSr] <<
                " pH= " << m_pH[in] << "  It= " << m_IterDone[in] << endl;
   }
//    cout << " Chemical loop ends: " << endl;
    // Here the output for the current transport state at tc can be implemented

    tc += dt;  // time increment
  }
  t_end11 = clock(); // getting end time of coupled calculations
  double dtime = ( t_end11- t_start11 );
  double clc_sec = CLOCKS_PER_SEC;
  MeanIt = double(nTotIt)/500.; // (double(nNodes*nTimes);

  cout <<  "Pure GEM IPM2 runtime , s: " <<  CalcTime << endl;
  cout <<  "Total time of calculation, s: " <<  (dtime)/clc_sec << endl;
  cout << "    Mean GEM IPM iterations: " << MeanIt << endl;

  cout << " This gem_node test ";

  // (3) ----------------------------------------------
  // Calculations finished - t_end reached

  // freeing dynamic arrays
  free( m_xDC );
  free( m_gam );
  free( m_xPH );
  free( m_vPS );
  free( m_mPS );
  free( m_bPS );
  free( m_xPA );
  free( m_dul );
  free( m_dll );
  free( m_bIC );
  free( m_rMB );
  free( m_uIC );

  // deleting GEMIPM and data exchange memory structures
  delete node;

  cout << "has finished Ok" << endl;

  return 0;
}

//---------------------------------------------------------------------------
// end of main.cpp for node class usage example
