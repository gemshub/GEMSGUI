//-------------------------------------------------------------------
// $Id: main.cpp 717 2006-07-06 08:06:21Z gems $
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
#include "node.h"
#include "gstring.h"

istream&
f_getline(istream& is, gstring& str, char delim);

int main( int argc, char* argv[] )
 {

     gstring multu_in = "MgWBoundC.ipm";
     gstring chbr_in = "ipmfiles-dat.lst";

   // from argv
      if (argc >= 2 )
        multu_in = argv[1];
      if (argc >= 3 )
       chbr_in = argv[2];

  // The Node structure must be allocated here
   TNode* node = TNode::na = new TNode();

  // Here we read the MULTI structure, DATACH and DATABR files prepared from GEMS
  if( node->GEM_init( multu_in.c_str(), chbr_in.c_str() ) )
      return 1;  // error reading files


   // Allocate work memory for test

   int nNodes = 1; // number of local equilibrium compartments (nodes)
   int ntM = 100;  // Maximum allowed number of time iteration steps

   short m_NodeHandle, m_NodeStatusCH,  m_IterDone;
   double m_T, m_P, m_Vs, m_Ms,m_Gs, m_Hs, m_IC, m_pH, m_pe,
          m_Eh, m_denW, m_denWg, m_epsW, m_epsWg;
   double *m_xDC, *m_gam, *m_xPH, *m_vPS, *m_mPS,*m_bPS,
         *m_xPA, *m_dul, *m_dll, *m_bIC, *m_rMB, *m_uIC;

   int nIC, nDC, nPH, nPS;
   short *xDC, *xIC, *xPH;
   DATACH  *dCH = node->pCSD();

   if( !dCH  )
       return 1;

   // Extracting data bridge dimensionalities
   nIC = dCH->nICb;
   nDC = dCH->nDCb;
   nPH = dCH->nPHb;
   nPS = dCH->nPSb;

   m_xDC = (double*)malloc( nDC*sizeof(double) );
   m_gam = (double*)malloc( nDC*sizeof(double) );
   m_xPH = (double*)malloc( nPH*sizeof(double) );
   m_vPS = (double*)malloc( nPS*sizeof(double) );
   m_mPS = (double*)malloc( nPS*sizeof(double) );
   m_bPS = (double*)malloc( nIC*nPS*sizeof(double) );
   m_xPA = (double*)malloc( nPS*sizeof(double) );
   m_dul = (double*)malloc( nDC*sizeof(double) );
   m_dll = (double*)malloc( nDC*sizeof(double) );
   m_bIC = (double*)malloc( nIC*sizeof(double) );
   m_rMB = (double*)malloc( nIC*sizeof(double) );
   m_uIC = (double*)malloc( nIC*sizeof(double) );

  // set up start value for test arrays
   node->GEM_restore_MT( m_NodeHandle,m_NodeStatusCH,
          m_T,m_P, m_Vs, m_Ms, m_dul, m_dll, m_bIC );

  // work cycle
  for( int it=0; it<ntM; it++ )
  {

    for( int in=0; in<nNodes; in++ )
    {
      m_NodeHandle = in;
      m_NodeStatusCH = NEED_GEM_AIA; //NEED_GEM_PIA

      node->GEM_from_MT( m_NodeHandle,m_NodeStatusCH,
             m_T,m_P, m_Vs, m_Ms, m_dul, m_dll, m_bIC );

      node->GEM_run( m_NodeStatusCH );

      node->GEM_to_MT( m_NodeHandle, m_NodeStatusCH,m_IterDone,
                       m_Vs, m_Ms, m_Gs, m_Hs, m_IC, m_pH,
                       m_pe, m_Eh, m_denW,m_denWg,m_epsW, m_epsWg,
                       m_xDC, m_gam, m_xPH, m_vPS, m_mPS, m_bPS,
                       m_xPA, m_rMB, m_uIC );
    }
    m_T += 1;
    cout << "T = "<< m_T << " NodeStatuis = " << m_NodeStatusCH << endl;
//   m_bIC[] += .0001;


  }

  delete node;
  return 0;
}

//---------------------------------------------------------------------------

