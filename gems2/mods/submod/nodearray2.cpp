//-------------------------------------------------------------------
// $Id: nodearray.cpp 684 2005-11-23 13:17:15Z gems $
//
// C/C++ interface between GEM IPM and FMT node array
// Working whith DATACH and DATABR structures (i/o)
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

#include "nodearray.h"
#include "gdatastream.h"
#include <math.h>

#ifndef __unix
#include <io.h>
#endif

#ifndef IPMGEMPLUGIN
  #include "service.h"
  #include "visor.h"
#else
//  istream& f_getline(istream& is, gstring& str, char delim);
//  TNodeArray* TNodeArray::na;
#endif



// Data collection for monitoring differences
// Prints difference increments in a all nodes (cells) for time point t / at
void TNodeArray::logDiffsIC( FILE* diffile, int t, double at, int nx, int every_t )
{
  double dc;
  int i, ie;

  if( t % every_t )
    return;

  fprintf( diffile, "\nStep= %-8d  Time= %-12.4g\nNode#   ", t, at );
  for( ie=0; ie < int(CSD->nICb); ie++ )
    fprintf( diffile, "%-12.4s ", CSD->ICNL[ie] );
  for (i=0; i<nx; i++)    // node iteration
  {
     fprintf( diffile, "\n%5d   ", i );
     for( ie=0; ie < int(CSD->nICb); ie++ )
     {
        dc = NodT1[i]->bIC[ie] - NodT0[i]->bIC[ie];
        fprintf( diffile, "%-12.4g ", dc );
     }
  }
  fprintf( diffile, "\n" );
}

// Data collection for monitoring 1D profiles in debugging FMT models
// Prints dissolved elemental molarities in all cells for time point t / at
void TNodeArray::logProfileAqIC( FILE* logfile, int t, double at, int nx, int every_t )
{
  double pm;
  int i, ie;
  if( t % every_t )
    return;
  fprintf( logfile, "\nStep= %-8d  Time= %-12.4g     Dissolved IC total concentrations, M\n", t, at/(365*86400) );
  fprintf(logfile, "%s","Node#   ");
  for( ie=0; ie < int(CSD->nICb); ie++ )
    fprintf( logfile, "%-12.4s ", CSD->ICNL[ie] );
  for (i=0; i<nx; i++)    // node iteration
  {
     fprintf( logfile, "\n%5d   ", i );
     for( ie=0; ie < int(CSD->nICb); ie++ )
     {
       pm = NodT1[i]->bPS[ie]/NodT1[i]->vPS[0]*1000.;  // Assumes there is aq phase!
                 // total dissolved element molarity
       fprintf( logfile, "%-12.4g ", pm );
     }
  }
  fprintf( logfile, "\n" );
}

// Data collection for monitoring 1D profiles
// Prints total elemental amounts in all cells for time point t / at
void TNodeArray::logProfileTotIC( FILE* logfile, int t, double at, int nx, int every_t )
{
  double pm;
  int i, ie;
  if( t % every_t )
    return;
  fprintf( logfile, "\nStep= %-8d  Time= %-12.4g     Bulk IC amounts, moles\n", t, at/(365*86400) );
  fprintf(logfile, "%s","Node#   ");
  for( ie=0; ie < int(CSD->nICb); ie++ )
    fprintf( logfile, "%-12.4s ", CSD->ICNL[ie] );
  for (i=0; i<nx; i++)    // node iteration
  {
     fprintf( logfile, "\n%5d   ", i );
     for( ie=0; ie < int(CSD->nICb); ie++ )
     {
       pm = NodT1[i]->bIC[ie];
       fprintf( logfile, "%-12.4g ", pm );
     }
  }
  fprintf( logfile, "\n" );
}

// Prints amounts of reactive phases in all cells for time point t / at
void TNodeArray::logProfilePhMol( FILE* logfile, int t, double at, int nx, int every_t )
{
  double pm;
  int i, ip;
  if( t % every_t )
    return;
  fprintf( logfile, "\nStep= %-8d  Time= %-12.4g     Amounts of reactive phases, moles\n", t, at/(365*86400) );
  fprintf(logfile, "%s","Node#   ");
  for( ip=0; ip < int(CSD->nPHb); ip++ )
    fprintf( logfile, "%-12.12s ", CSD->PHNL[ip]+4 );
  for (i=0; i<nx; i++)    // node iteration
  {
     fprintf( logfile, "\n%5d   ", i );
     for( ip=0; ip < int(CSD->nPHb); ip++ )
     {
       pm = NodT1[i]->xPH[ip];
       fprintf( logfile, "%-12.4g ", pm );
     }
  }
  fprintf( logfile, "\n" );
}

// Prints dissolved species molarities in all cells for time point t / at
void TNodeArray::logProfileAqDC( FILE* logfile, int t, double at, int nx, int every_t )
{
  double pm;
  int i, is;
  if( t % every_t )
    return;
  fprintf( logfile, "\nStep= %-8d  Time= %-12.4g     Dissolved species concentrations, M\n", t, at/(365*86400) );
  fprintf(logfile, "%s","Node#   ");
  for( is=0; is < int(CSD->nDCb); is++ )
    fprintf( logfile, "%-12.4s ", CSD->DCNL[is] );
  for (i=0; i<nx; i++)    // node iteration
  {
     fprintf( logfile, "\n%5d   ", i );
     for( is=0; is < int(CSD->nDCinPH[0]); is++ )
     {
       pm = NodT1[i]->xDC[is]/NodT1[i]->vPS[0]*1000.;  // Assumes there is aq phase!
                 // dissolved species molarity
       fprintf( logfile, "%-12.4g ", pm );
     }
  }
  fprintf( logfile, "\n" );
}

//---------------------------------------------------------------

// new structures i/o

// Writting DataCH to binary file
void TNodeArray::datach_to_file( GemDataStream& ff )
{
// const data
   ff.writeArray( &CSD->nIC, 14 );
   ff.writeArray( &CSD->Ttol, 4 );

//dynamic data
   ff.writeArray( CSD->nDCinPH, CSD->nPH );
//   if( CSD->nICb >0 )
   ff.writeArray( CSD->xIC, CSD->nICb );
   ff.writeArray( CSD->xDC, CSD->nDCb );
   ff.writeArray( CSD->xPH, CSD->nPHb );

   ff.writeArray( CSD->A, CSD->nIC*CSD->nDC );
   ff.writeArray( CSD->ICmm, CSD->nIC );
   ff.writeArray( CSD->DCmm, CSD->nDC );
   ff.writeArray( CSD->DD, CSD->nDC );

   ff.writeArray( CSD->Tval,  CSD->nTp );
   ff.writeArray( CSD->Pval,  CSD->nPp );

   ff.writeArray( CSD->roW,  CSD->nPp*CSD->nTp );
   ff.writeArray( CSD->epsW, CSD->nPp*CSD->nTp );
   ff.writeArray( CSD->G0,  CSD->nDC*CSD->nPp*CSD->nTp );
   ff.writeArray( CSD->V0,  CSD->nDC*CSD->nPp*CSD->nTp );
   ff.writeArray( CSD->H0,  CSD->nDC*CSD->nPp*CSD->nTp );
   ff.writeArray( CSD->Cp0, CSD->nDC*CSD->nPp*CSD->nTp );

   if( CSD->nAalp >0 )
     ff.writeArray( CSD->Aalp, CSD->nPH );

   ff.writeArray( (char *)CSD->ICNL, MaxICN*CSD->nIC );
   ff.writeArray( (char *)CSD->DCNL, MaxDCN*CSD->nDC );
   ff.writeArray( (char *)CSD->PHNL, MaxPHN*CSD->nPH );

   ff.writeArray( CSD->ccIC, CSD->nIC );
   ff.writeArray( CSD->ccDC, CSD->nDC );
   ff.writeArray( CSD->ccDCW, CSD->nDC );
   ff.writeArray( CSD->ccPH, CSD->nPH );

}

// Reading dataCH structure from binary file
void TNodeArray::datach_from_file( GemDataStream& ff )
{
// const data
   ff.readArray( &CSD->nIC, 14 );
   ff.readArray( &CSD->Ttol, 4 );

  datach_realloc();
  databr_realloc();

//dynamic data
   ff.readArray( CSD->nDCinPH, CSD->nPH );
//   if( CSD->nICb >0 )
   ff.readArray( CSD->xIC, CSD->nICb );
   ff.readArray( CSD->xDC, CSD->nDCb );
   ff.readArray( CSD->xPH, CSD->nPHb );

   ff.readArray( CSD->A, CSD->nIC*CSD->nDC );
   ff.readArray( CSD->ICmm, CSD->nIC );
   ff.readArray( CSD->DCmm, CSD->nDC );
   ff.readArray( CSD->DD, CSD->nDC );

   ff.readArray( CSD->Tval,  CSD->nTp );
   ff.readArray( CSD->Pval,  CSD->nPp );

   ff.readArray( CSD->roW,  CSD->nPp*CSD->nTp );
   ff.readArray( CSD->epsW, CSD->nPp*CSD->nTp );
   ff.readArray( CSD->G0,  CSD->nDC*CSD->nPp*CSD->nTp );
   ff.readArray( CSD->V0,  CSD->nDC*CSD->nPp*CSD->nTp );
   ff.readArray( CSD->H0,  CSD->nDC*CSD->nPp*CSD->nTp );
   ff.readArray( CSD->Cp0, CSD->nDC*CSD->nPp*CSD->nTp );

   if( CSD->nAalp >0 )
     ff.readArray( CSD->Aalp, CSD->nPH );

   ff.readArray( (char *)CSD->ICNL, MaxICN*CSD->nIC );
   ff.readArray( (char *)CSD->DCNL, MaxDCN*CSD->nDC );
   ff.readArray( (char *)CSD->PHNL, MaxPHN*CSD->nPH );

   ff.readArray( CSD->ccIC, CSD->nIC );
   ff.readArray( CSD->ccDC, CSD->nDC );
   ff.readArray( CSD->ccDCW, CSD->nDC );
   ff.readArray( CSD->ccPH, CSD->nPH );

}

void TNodeArray::datach_to_text_file( fstream& ff )
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
void TNodeArray::datach_from_text_file(fstream& ff)
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

// allocate DataCH structure
void TNodeArray::datach_realloc()
{
 CSD->nDCinPH = new short[CSD->nPH];

 if( CSD->nICb >0 )
   CSD->xIC = new short[CSD->nICb];
 else  CSD->xIC = 0;
 if( CSD->nDCb >0 )
   CSD->xDC = new short[CSD->nDCb];
 else  CSD->xDC = 0;
 if( CSD->nPHb >0 )
   CSD->xPH = new short[CSD->nPHb];
 else  CSD->xPH = 0;

  CSD->A = new float[CSD->nIC*CSD->nDC];
  CSD->ICmm = new double[CSD->nIC];
  CSD->DCmm = new double[CSD->nDC];
  CSD->DD = new double[CSD->nDC];

  CSD->Tval = new float[CSD->nTp];
  CSD->Pval = new float[CSD->nPp];

  CSD->roW = new double[ CSD->nPp*CSD->nTp];
  CSD->epsW = new double[ CSD->nPp*CSD->nTp];
  CSD->G0 = new double[CSD->nDC*CSD->nPp*CSD->nTp];
  CSD->V0 = new double[CSD->nDC*CSD->nPp*CSD->nTp];
  CSD->H0 = new double[CSD->nDC*CSD->nPp*CSD->nTp];
  CSD->Cp0 = new double[CSD->nDC*CSD->nPp*CSD->nTp];

  if( CSD->nAalp >0 )
     CSD->Aalp = new double[CSD->nPH];
  else CSD->Aalp = 0;

  CSD->ICNL = new char[CSD->nIC][MaxICN];
  CSD->DCNL = new char[CSD->nDC][MaxDCN];
  CSD->PHNL = new char[CSD->nPH][MaxPHN];

  CSD->ccIC = new char[CSD->nIC];
  CSD->ccDC = new char[CSD->nDC];
  CSD->ccDCW = new char[CSD->nDC];
  CSD->ccPH = new char[CSD->nPH];
}

// free dynamic memory
void TNodeArray::datach_free()
{
 if( CSD->nDCinPH )
  { delete[] CSD->nDCinPH;
    CSD->nDCinPH = 0;
  }
 if( CSD->xIC )
  { delete[] CSD->xIC;
    CSD->xIC = 0;
  }
 if( CSD->xDC )
  { delete[] CSD->xDC;
    CSD->xDC = 0;
  }
 if( CSD->xPH )
  { delete[] CSD->xPH;
    CSD->xPH = 0;
  }
 if( CSD->A )
  { delete[] CSD->A;
    CSD->A = 0;
  }
 if( CSD->ICmm )
  { delete[] CSD->ICmm;
    CSD->ICmm = 0;
  }
 if( CSD->DCmm )
  { delete[] CSD->DCmm;
    CSD->DCmm = 0;
  }
 if( CSD->DD )
  { delete[] CSD->DD;
    CSD->DD = 0;
  }

 if( CSD->Tval )
  { delete[] CSD->Tval;
    CSD->Tval = 0;
  }
 if( CSD->Pval )
  { delete[] CSD->Pval;
    CSD->Pval = 0;
  }

 if( CSD->roW )
  { delete[] CSD->roW;
    CSD->roW = 0;
  }
 if( CSD->epsW )
  { delete[] CSD->epsW;
    CSD->epsW = 0;
  }
 if( CSD->G0 )
  { delete[] CSD->G0;
    CSD->G0 = 0;
  }
 if( CSD->V0 )
  { delete[] CSD->V0;
    CSD->V0 = 0;
  }
 if( CSD->H0 )
  { delete[] CSD->H0;
    CSD->H0 = 0;
  }
 if( CSD->Cp0 )
  { delete[] CSD->Cp0;
    CSD->Cp0 = 0;
  }
 if( CSD->Aalp )
  { delete[] CSD->Aalp;
    CSD->Aalp = 0;
  }

 if( CSD->ICNL )
  { delete[] CSD->ICNL;
    CSD->ICNL = 0;
  }
 if( CSD->DCNL )
  { delete[] CSD->DCNL;
    CSD->DCNL = 0;
  }
 if( CSD->PHNL )
  { delete[] CSD->PHNL;
    CSD->PHNL = 0;
  }

 if( CSD->ccIC )
  { delete[] CSD->ccIC;
    CSD->ccIC = 0;
  }
 if( CSD->ccDC )
  { delete[] CSD->ccDC;
    CSD->ccDC = 0;
  }
 if( CSD->ccDCW )
  { delete[] CSD->ccDCW;
    CSD->ccDCW = 0;
  }
 if( CSD->ccPH )
  { delete[] CSD->ccPH;
    CSD->ccPH = 0;
  }
 // delete[] CSD;
}

// writing DataBR to binary file
void TNodeArray::databr_to_file( GemDataStream& ff )
{
// const data
   ff.writeArray( &CNode->NodeHandle, 6 );
   ff.writeArray( &CNode->T, 36 );

//dynamic data
   ff.writeArray( CNode->xDC, CSD->nDCb );
   ff.writeArray( CNode->gam, CSD->nDCb );
   ff.writeArray( CNode->xPH, CSD->nPHb );
   ff.writeArray( CNode->vPS, CSD->nPSb );
   ff.writeArray( CNode->mPS, CSD->nPSb );

   ff.writeArray( CNode->bPS, CSD->nPSb*CSD->nICb );
   ff.writeArray( CNode->xPA, CSD->nPSb );
   ff.writeArray( CNode->dul, CSD->nDCb );
   ff.writeArray( CNode->dll, CSD->nDCb );
   ff.writeArray( CNode->bIC, CSD->nICb );
   ff.writeArray( CNode->rMB, CSD->nICb );
   ff.writeArray( CNode->uIC, CSD->nICb );

   CNode->dRes1 = 0;
   CNode->dRes2 = 0;

//   datach_to_text_file();
//   databr_to_text_file();
}

// Reading work dataBR structure from binary file
void TNodeArray::databr_from_file( GemDataStream& ff )
{
// const data
   ff.readArray( &CNode->NodeHandle, 6 );
   ff.readArray( &CNode->T, 36 );

//dynamic data
   ff.readArray( CNode->xDC, CSD->nDCb );
   ff.readArray( CNode->gam, CSD->nDCb );
   ff.readArray( CNode->xPH, CSD->nPHb );
   ff.readArray( CNode->vPS, CSD->nPSb );
   ff.readArray( CNode->mPS, CSD->nPSb );

   ff.readArray( CNode->bPS, CSD->nPSb*CSD->nICb );
   ff.readArray( CNode->xPA, CSD->nPSb );
   ff.readArray( CNode->dul, CSD->nDCb );
   ff.readArray( CNode->dll, CSD->nDCb );
   ff.readArray( CNode->bIC, CSD->nICb );
   ff.readArray( CNode->rMB, CSD->nICb );
   ff.readArray( CNode->uIC, CSD->nICb );

   CNode->dRes1 = 0;
   CNode->dRes2 = 0;
}


void TNodeArray::databr_to_text_file( fstream& ff )
{
// fstream ff("DataBR.out", ios::out );
// ErrorIf( !ff.good() , "DataCH.out", "Fileopen error");

  outArray( ff, "sCon",  &CNode->NodeHandle, 6 );
  outArray( ff, "dCon",  &CNode->T, 36 );

  outArray( ff, "xDC",  CNode->xDC, CSD->nDCb );
  outArray( ff, "gam",  CNode->gam, CSD->nDCb );
  outArray( ff, "xPH",  CNode->xPH, CSD->nPHb );
  outArray( ff, "vPS",  CNode->vPS, CSD->nPSb );
  outArray( ff, "mPS",  CNode->mPS, CSD->nPSb );


  outArray( ff, "bPS",  CNode->bPS, CSD->nPSb*CSD->nICb );
  outArray( ff, "xPA",  CNode->xPA, CSD->nPSb );
  outArray( ff, "dul",  CNode->dul, CSD->nDCb );
  outArray( ff, "dll",  CNode->dll, CSD->nDCb );
  outArray( ff, "bIC",  CNode->bIC, CSD->nICb );
  outArray( ff, "rMB",  CNode->rMB, CSD->nICb );
  outArray( ff, "uIC",  CNode->uIC, CSD->nICb );

}

// Reading work dataBR structure from text file
void TNodeArray::databr_from_text_file( fstream& ff )
{
// fstream ff("DataBR.out", ios::out );
// ErrorIf( !ff.good() , "DataCH.out", "Fileopen error");

  inArray( ff, "sCon",  &CNode->NodeHandle, 6 );
  inArray( ff, "dCon",  &CNode->T, 36 );

  inArray( ff, "xDC",  CNode->xDC, CSD->nDCb );
  inArray( ff, "gam",  CNode->gam, CSD->nDCb );
  inArray( ff, "xPH",  CNode->xPH, CSD->nPHb );
  inArray( ff, "vPS",  CNode->vPS, CSD->nPSb );
  inArray( ff, "mPS",  CNode->mPS, CSD->nPSb );

  inArray( ff, "bPS",  CNode->bPS, CSD->nPSb*CSD->nICb );
  inArray( ff, "xPA",  CNode->xPA, CSD->nPSb );
  inArray( ff, "dul",  CNode->dul, CSD->nDCb );
  inArray( ff, "dll",  CNode->dll, CSD->nDCb );
  inArray( ff, "bIC",  CNode->bIC, CSD->nICb );
  inArray( ff, "rMB",  CNode->rMB, CSD->nICb );
  inArray( ff, "uIC",  CNode->uIC, CSD->nICb );
}


// allocate DataBR structure
void TNodeArray::databr_realloc()
{
 CNode->xDC = new double[CSD->nDCb];
 CNode->gam = new double[CSD->nDCb];
 CNode->xPH = new double[CSD->nPHb];
 CNode->vPS = new double[CSD->nPSb];
 CNode->mPS = new double[CSD->nPSb];

 CNode->bPS = new double[CSD->nPSb*CSD->nICb];
 CNode->xPA = new double[CSD->nPSb];
 CNode->dul = new double[CSD->nDCb];
 CNode->dll = new double[CSD->nDCb];
 CNode->bIC = new double[CSD->nICb];
 CNode->rMB = new double[CSD->nICb];
 CNode->uIC = new double[CSD->nICb];

 CNode->dRes1 = 0;
 CNode->dRes2 = 0;
}

// free dynamic memory
DATABR * TNodeArray::databr_free( DATABR *CNode_ )
{

  if( CNode_ == 0)
    CNode_ = CNode;
  memset( &CNode_->NodeHandle, 0, 6*sizeof(short));
  memset( &CNode_->T, 0, 36*sizeof(double));

 if( CNode_->xDC )
  { delete[] CNode_->xDC;
    CNode_->xDC = 0;
  }
 if( CNode_->gam )
  { delete[] CNode_->gam;
    CNode_->gam = 0;
  }
 if( CNode_->xPH )
  { delete[] CNode_->xPH;
    CNode_->xPH = 0;
  }
 if( CNode_->vPS )
  { delete[] CNode_->vPS;
    CNode_->vPS = 0;
  }
 if( CNode_->mPS )
  { delete[] CNode_->mPS;
    CNode_->mPS = 0;
  }

 if( CNode_->bPS )
  { delete[] CNode_->bPS;
    CNode_->bPS = 0;
  }
 if( CNode_->xPA )
  { delete[] CNode_->xPA;
    CNode_->xPA = 0;
  }
 if( CNode_->dul )
  { delete[] CNode_->dul;
    CNode_->dul = 0;
  }
 if( CNode_->dll )
  { delete[] CNode_->dll;
    CNode_->dll = 0;
  }
 if( CNode_->bIC )
  { delete[] CNode_->bIC;
    CNode_->bIC = 0;
  }
 if( CNode_->rMB )
  { delete[] CNode_->rMB;
    CNode_->rMB = 0;
  }
 if( CNode_->uIC )
  { delete[] CNode_->uIC;
    CNode_->uIC = 0;
  }

 delete[] CNode_;
 return NULL;
}

#ifdef IPMGEMPLUGIN

// calculation mode: passing input GEM data changed on previous FMT iteration
//                   into work DATABR structure
void TNodeArray::GEM_input_from_MT(
   short p_NodeHandle,    // Node identification handle
   short p_NodeStatusCH,  // Node status code CH;  see typedef NODECODECH
   double p_T,     // Temperature T, K                        +      +      -     -
   double p_P,     // Pressure P, bar                         +      +      -     -
   double p_Ms,    // Mass of reactive subsystem, kg          +      +      -     -
   double p_dt,    // actual time step
   double p_dt1,   // priveous time step
   double  *p_dul,  // upper kinetic restrictions [nDCb]            +      +      -     -
   double  *p_dll,  // lower kinetic restrictions [nDCb]            +      +      -     -
   double  *p_bIC  // bulk mole amounts of IC[nICb]                +      +      -     -
   )
{
  int ii;
  bool useSimplex = false;

  CNode->NodeHandle = p_NodeHandle;
  CNode->NodeStatusCH = p_NodeStatusCH;
  CNode->T = p_T;
  CNode->P = p_P;
  CNode->Ms = p_Ms;
  CNode->dt = p_dt;
  CNode->dt1 = p_dt1;
// Checking if no-simplex IA is Ok
   for( ii=0; ii<CSD->nICb; ii++ )
   {  //  Sveta 11/02/05 for test
      //if( fabs(CNode->bIC[ii] - p_bIC[ii] ) > CNode->bIC[ii]*1e-4 ) // bugfix KD 21.11.04
       //     useSimplex = true;
     CNode->bIC[ii] = p_bIC[ii];
   }
   for( ii=0; ii<CSD->nDCb; ii++ )
   {
     CNode->dul[ii] = p_dul[ii];
     CNode->dll[ii] = p_dll[ii];
   }
   if( useSimplex && CNode->NodeStatusCH == NEED_GEM_PIA )
     CNode->NodeStatusCH = NEED_GEM_AIA;
   // Switch only if PIA is ordered, leave if simplex is ordered (KD)
}

// readonly mode: passing input GEM data to FMT
void TNodeArray::GEM_input_back_to_MT(
   short &p_NodeHandle,    // Node identification handle
   short &p_NodeStatusCH,  // Node status code CH;  see typedef NODECODECH
   double &p_T,     // Temperature T, K                        +      +      -     -
   double &p_P,     // Pressure P, bar                         +      +      -     -
   double &p_Ms,    // Mass of reactive subsystem, kg          +      +      -     -
   double &p_dt,    // actual time step
   double &p_dt1,   // priveous time step
   double  *p_dul,  // upper kinetic restrictions [nDCb]            +      +      -     -
   double  *p_dll,  // lower kinetic restrictions [nDCb]            +      +      -     -
   double  *p_bIC  // bulk mole amounts of IC[nICb]                +      +      -     -
   )
{
 int ii;
  p_NodeHandle = CNode->NodeHandle;
  p_NodeStatusCH = CNode->NodeStatusCH;
  p_T = CNode->T;
  p_P = CNode->P;
  p_Ms = CNode->Ms;
  p_dt = CNode->dt;
  p_dt1 = CNode->dt1;
// Checking if no-simplex IA is Ok
   for( ii=0; ii<CSD->nICb; ii++ )
     p_bIC[ii] = CNode->bIC[ii];
   for( ii=0; ii<CSD->nDCb; ii++ )
   {  p_dul[ii] = CNode->dul[ii];
      p_dll[ii] = CNode->dll[ii];
   }
}

// Copying results that must be returned into the FMT part into MAIF_CALC parameters
void TNodeArray::GEM_output_to_MT(
   short &p_NodeHandle,    // Node identification handle
   short &p_NodeStatusCH,  // Node status code CH;  see typedef NODECODECH
   short &p_IterDone,      // Number of iterations performed by IPM (if not need GEM)
// Chemical scalar variables
   double &p_Vs,    // Volume V of reactive subsystem, cm3     -      -      +     +
   double &p_Gs,    // Gibbs energy of reactive subsystem (J)  -      -      +     +
   double &p_Hs,    // Enthalpy of reactive subsystem (J)      -      -      +     +
   double &p_IC,    // Effective molal aq ionic strength           -      -      +     +
   double &p_pH,    // pH of aqueous solution                      -      -      +     +
   double &p_pe,    // pe of aqueous solution                      -      -      +     +
   double &p_Eh,    // Eh of aqueous solution, V                   -      -      +     +
   double &p_denW,
   double &p_denWg, // Density of H2O(l) and steam at T,P      -      -      +     +
   double &p_epsW,
   double &p_epsWg, // Diel.const. of H2O(l) and steam at T,P  -      -      +     +
// Dynamic data - dimensions see in DATACH.H and DATAMT.H structures
// exchange of values occurs through lists of indices, e.g. xDC, xPH
   double  *p_xDC,    // DC mole amounts at equilibrium [nDCb]      -      -      +     +
   double  *p_gam,    // activity coeffs of DC [nDCb]               -      -      +     +
   double  *p_xPH,  // total mole amounts of phases [nPHb]          -      -      +     +
   double  *p_vPS,  // phase volume, cm3/mol        [nPSb]          -      -      +     +
   double  *p_mPS,  // phase (carrier) mass, g      [nPSb]          -      -      +     +
   double  *p_bPS,  // bulk compositions of phases  [nPSb][nICb]    -      -      +     +
   double  *p_xPA,  // amount of carrier in phases  [nPSb] ??       -      -      +     +
   double  *p_dul,  // upper kinetic restrictions [nDCb]            +      +      -     -
   double  *p_dll,  // lower kinetic restrictions [nDCb]            +      +      -     -
   double  *p_bIC,  // bulk mole amounts of IC[nICb]                +      +      -     -
   double  *p_rMB,  // MB Residuals from GEM IPM [nICb]             -      -      +     +
   double  *p_uIC  // IC chemical potentials (mol/mol)[nICb]       -      -      +     +
   )
{

   p_NodeHandle = CNode->NodeHandle;
   p_NodeStatusCH = CNode->NodeStatusCH;
   p_IterDone = CNode->IterDone;

   p_Vs = CNode->Vs;
   p_Gs = CNode->Gs;
   p_Hs = CNode->Hs;
   p_IC = CNode->IC;
   p_pH = CNode->pH;
   p_pe = CNode->pe;
   p_Eh = CNode->Eh;
   p_denW = CNode->denW;
   p_denWg = CNode->denWg;
   p_epsW = CNode->epsW;
   p_epsWg = CNode->epsWg;

  memcpy( p_xDC, CNode->xDC, CSD->nDCb*sizeof(double) );
  memcpy( p_gam, CNode->gam, CSD->nDCb*sizeof(double) );
  memcpy( p_xPH, CNode->xPH, CSD->nPHb*sizeof(double) );
  memcpy( p_vPS, CNode->vPS, CSD->nPSb*sizeof(double) );
  memcpy( p_mPS, CNode->mPS, CSD->nPSb*sizeof(double) );
  memcpy( p_bPS, CNode->bPS, CSD->nPSb*CSD->nICb*sizeof(double) );
  memcpy( p_xPA, CNode->xPA, CSD->nPSb*sizeof(double) );
  memcpy( p_dul, CNode->dul, CSD->nDCb*sizeof(double) );
  memcpy( p_dll, CNode->dll, CSD->nDCb*sizeof(double) );
  memcpy( p_bIC, CNode->bIC, CSD->nICb*sizeof(double) );
  memcpy( p_rMB, CNode->rMB, CSD->nICb*sizeof(double) );
  memcpy( p_uIC, CNode->uIC, CSD->nICb*sizeof(double) );
}

#endif

//-----------------------End of tnodearray2.cpp--------------------------

