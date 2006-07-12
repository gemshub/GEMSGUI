//-------------------------------------------------------------------
// $Id: node.cpp 684 2005-11-23 13:17:15Z gems $
//
// C/C++ interface between GEM IPM and FMT node array
// Working whith DATACH and DATABR structures
//
// Copyright (C) 2004-2005 S.Dmytriyeva, D.Kulik
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

#include "node.h"

#ifndef __unix
#include <io.h>
#endif

void TNode::databr_to_text_file( fstream& ff )
{
// fstream ff("DataBR.out", ios::out );
// ErrorIf( !ff.good() , "DataCH.out", "Fileopen error");

  outArray( ff, "sCon",  &CNode->NodeHandle, 6 );
  outArray( ff, "dCon",  &CNode->T, 36 );

  outArray( ff, "bIC",  CNode->bIC, CSD->nICb );
  outArray( ff, "rMB",  CNode->rMB, CSD->nICb );
  outArray( ff, "uIC",  CNode->uIC, CSD->nICb );

  outArray( ff, "xDC",  CNode->xDC, CSD->nDCb );
  outArray( ff, "gam",  CNode->gam, CSD->nDCb );
  outArray( ff, "dul",  CNode->dul, CSD->nDCb );
  outArray( ff, "dll",  CNode->dll, CSD->nDCb );

  outArray( ff, "xPH",  CNode->xPH, CSD->nPHb );
  outArray( ff, "vPS",  CNode->vPS, CSD->nPSb );
  outArray( ff, "mPS",  CNode->mPS, CSD->nPSb );
  outArray( ff, "bPS",  CNode->bPS, CSD->nPSb*CSD->nICb );
  outArray( ff, "xPA",  CNode->xPA, CSD->nPSb );
}

// Reading work dataBR structure from text file
void TNode::databr_from_text_file( fstream& ff )
{
// fstream ff("DataBR.out", ios::out );
// ErrorIf( !ff.good() , "DataCH.out", "Fileopen error");

  inArray( ff, "sCon",  &CNode->NodeHandle, 6 );
  inArray( ff, "dCon",  &CNode->T, 36 );

  inArray( ff, "bIC",  CNode->bIC, CSD->nICb );
  inArray( ff, "rMB",  CNode->rMB, CSD->nICb );
  inArray( ff, "uIC",  CNode->uIC, CSD->nICb );

  inArray( ff, "xDC",  CNode->xDC, CSD->nDCb );
  inArray( ff, "gam",  CNode->gam, CSD->nDCb );
  inArray( ff, "dul",  CNode->dul, CSD->nDCb );
  inArray( ff, "dll",  CNode->dll, CSD->nDCb );

  inArray( ff, "xPH",  CNode->xPH, CSD->nPHb );
  inArray( ff, "vPS",  CNode->vPS, CSD->nPSb );
  inArray( ff, "mPS",  CNode->mPS, CSD->nPSb );
  inArray( ff, "bPS",  CNode->bPS, CSD->nPSb*CSD->nICb );
  inArray( ff, "xPA",  CNode->xPA, CSD->nPSb );
}

void TNode::datach_to_text_file( fstream& ff )
{
// fstream ff("DataCH.out", ios::out );
// ErrorIf( !ff.good() , "DataCH.out", "Fileopen error");

  outArray( ff, "sCon",  &CSD->nIC, 14 );
  outArray( ff, "dCon",  &CSD->Ttol, 4 );

//dynamic data
   outArray( ff, "nDCinPH", CSD->nDCinPH, CSD->nPH);
//   if( CSD->nICb >0 )
   outArray( ff, "xIC", CSD->xIC, CSD->nICb);
   outArray( ff, "xDC", CSD->xDC, CSD->nDCb);
   outArray( ff, "xPH", CSD->xPH, CSD->nPHb);

   outArray( ff, "A", CSD->A, CSD->nDC*CSD->nIC, CSD->nIC );
   outArray( ff, "ICmm", CSD->ICmm, CSD->nIC);
   outArray( ff, "DCmm", CSD->DCmm, CSD->nDC);
   outArray( ff, "DD", CSD->DD, CSD->nDC);

   outArray( ff, "Tval", CSD->Tval, CSD->nTp );
   outArray( ff, "Pval", CSD->Pval, CSD->nPp );

   outArray( ff, "roW", CSD->roW, CSD->nPp*CSD->nTp );
   outArray( ff, "epsW", CSD->epsW,  CSD->nPp*CSD->nTp );
   outArray( ff, "G0", CSD->G0, CSD->nDC*CSD->nPp*CSD->nTp,
                                    CSD->nPp*CSD->nTp );
   outArray( ff, "V0", CSD->V0,  CSD->nDC*CSD->nPp*CSD->nTp,
                                    CSD->nPp*CSD->nTp );
   outArray( ff, "H0", CSD->H0,  CSD->nDC*CSD->nPp*CSD->nTp,
                                     CSD->nPp*CSD->nTp );
   outArray( ff, "Cp0", CSD->Cp0,CSD->nDC*CSD->nPp*CSD->nTp,
                                     CSD->nPp*CSD->nTp  );

   if( CSD->nAalp >0 )
      outArray( ff, "Aalp", CSD->Aalp, CSD->nPH);

   outArray( ff, "ICNL", CSD->ICNL[0], CSD->nIC, MaxICN );
   outArray( ff, "DCNL", CSD->DCNL[0], CSD->nDC, MaxDCN );
   outArray( ff, "PHNL", CSD->PHNL[0], CSD->nPH, MaxPHN );

   outArray( ff, "ccIC", CSD->ccIC, CSD->nIC, 1 );
   outArray( ff, "ccDC", CSD->ccDC, CSD->nDC, 1 );
   outArray( ff, "ccDCW", CSD->ccDCW, CSD->nDC, 1 );
   outArray( ff, "ccPH", CSD->ccPH, CSD->nPH, 1 );

}

// Reading dataCH structure from text file
void TNode::datach_from_text_file(fstream& ff)
{
// fstream ff("DataCH.out", ios::in );
// ErrorIf( !ff.good() , "DataCH.out", "Fileopen error");

  inArray( ff, "sCon",  &CSD->nIC, 14 );
  inArray( ff, "dCon",  &CSD->Ttol, 4 );

  datach_realloc();
  databr_realloc();

//dynamic data
   inArray( ff, "nDCinPH", CSD->nDCinPH, CSD->nPH);
//   if( CSD->nICb >0 )
   inArray( ff, "xIC", CSD->xIC, CSD->nICb);
   inArray( ff, "xDC", CSD->xDC, CSD->nDCb);
   inArray( ff, "xPH", CSD->xPH, CSD->nPHb);

   inArray( ff, "A", CSD->A, CSD->nDC*CSD->nIC );
   inArray( ff, "ICmm", CSD->ICmm, CSD->nIC);
   inArray( ff, "DCmm", CSD->DCmm, CSD->nDC);
   inArray( ff, "DD", CSD->DD, CSD->nDC);

   inArray( ff, "Tval", CSD->Tval, CSD->nTp );
   inArray( ff, "Pval", CSD->Pval, CSD->nPp );

   inArray( ff, "roW", CSD->roW,   CSD->nPp*CSD->nTp);
   inArray( ff, "epsW", CSD->epsW, CSD->nPp*CSD->nTp);
   inArray( ff, "G0", CSD->G0,  CSD->nDC*CSD->nPp*CSD->nTp);
   inArray( ff, "V0", CSD->V0,  CSD->nDC*CSD->nPp*CSD->nTp);
   inArray( ff, "H0", CSD->H0,  CSD->nDC*CSD->nPp*CSD->nTp);
   inArray( ff, "Cp0", CSD->Cp0,CSD->nDC*CSD->nPp*CSD->nTp);

   if( CSD->nAalp >0 )
      inArray( ff, "Aalp", CSD->Aalp, CSD->nPH);

   inArray( ff, "ICNL", CSD->ICNL[0], CSD->nIC, MaxICN );
   inArray( ff, "DCNL", CSD->DCNL[0], CSD->nDC, MaxDCN );
   inArray( ff, "PHNL", CSD->PHNL[0], CSD->nPH, MaxPHN );

   inArray( ff, "ccIC", CSD->ccIC, CSD->nIC, 1 );
   inArray( ff, "ccDC", CSD->ccDC, CSD->nDC, 1 );
   inArray( ff, "ccDCW", CSD->ccDCW, CSD->nDC, 1 );
   inArray( ff, "ccPH", CSD->ccPH, CSD->nPH, 1 );
}

//-----------------------End of node_format.cpp--------------------------
