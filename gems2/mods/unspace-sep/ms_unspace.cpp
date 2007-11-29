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

#include "ms_unspace.h"
#include "io_arrays.h"




TUnSpace* TUnSpace::pm;

TUnSpace::TUnSpace()
{
    usp=&us[0];

    // default data
    memset(usp, 0, sizeof(UNSPACE) );
  
    na = 0;
}  

TUnSpace::~TUnSpace()
{
  Free();
  if( na )
   delete na;
}

outField TUnSpace_static_fields[15] =  {
 { "Mode", 1,0 },
 { "Size" , 1,0 },
 { "MaxSteps", 1,0 },
 { "Tau", 1,0 },
 { "Grid", 1,0 },
 { "Types", 1,0 },
 { "Props", 1,0 },
 { "LSize", 1,0 },
 { "fVel", 1,0 },
 { "cLen", 1,0 },
 { "tf", 1,0 },
 { "cdv", 1,0 },
 { "cez", 1,0 },
 { "al_in", 1,0 },
 { "Dif_in", 1,0 }
 };

outField TUnSpace_dynamic_fields[3] =  {
 { "DiCp", 1, 0 },
 { "NPmean", 1, 0 },
 { "mGrid", 1, 0 }
 };

// The unspace start constant
int TUnSpace::Unspace_to_format_txt( const char *unspace_in1 )
{
 // read GEM2MT structure from file
  fstream f_log("ipmlog.txt", ios::out|ios::app );
  try
  {
   fstream ff(unspace_in1, ios::in );
   ErrorIf( !ff.good() , unspace_in1, "Fileopen error");

// static arrays
   TReadArrays  rdar( 15, TUnSpace_static_fields, ff);
   short nfild = rdar.findNext();
   while( nfild >=0 )
   {
     switch( nfild )
     {
 /*      case 0: rdar.readArray( "Mode", &mtp->PsMode, 1, 1);
            break;
       case 1: rdar.readArray( "Size", &mtp->xC, 3);
            mtp->nC = mtp->xC*mtp->yC*mtp->zC;
             break;
       case 2: rdar.readArray( "MaxSteps", &mtp->ntM, 1);
             break;
       case 3: rdar.readArray( "Tau", mtp->Tau, 3);
            break;
       case 4: rdar.readArray( "Grid", &mtp->PvGrid, 1, 1);
            break;
       case 5: rdar.readArray( "Types", &mtp->nPTypes, 1);
            break;
       case 6: rdar.readArray( "Props", &mtp->nProps, 1);
            break;
       case 7: rdar.readArray( "LSize", mtp->sizeLc, 3);
            break;
       case 8: rdar.readArray( "fVel", &mtp->fVel, 1);
            break;
       case 9: rdar.readArray( "cLen", &mtp->cLen, 1);
             break;
       case 10: rdar.readArray( "tf", &mtp->tf, 1);
            break;
       case 11: rdar.readArray( "cdv", &mtp->cdv, 1);
            break;
       case 12: rdar.readArray( "cez", &mtp->cez, 1);
            break;
       case 13: rdar.readArray( "al_in", &mtp->al_in, 1);
            break;
       case 14: rdar.readArray( "Dif_in", &mtp->Dif_in, 1);
            break;
     */}
     nfild = rdar.findNext();
  }

   // testing read
//   if( mtp->PsMode != GMT_MODE_W && mtp->PsMode != GMT_MODE_V )
//   {
//      rdar.setNoAlws( 4 /*"Grid"*/);
//      rdar.setNoAlws( 5 /*"Types"*/);
//      rdar.setNoAlws( 6 /*"Props"*/);
//      rdar.setNoAlws( 7 /*"LSize"*/);
//      mtp->PvGrid = '-';
//   }
   gstring ret = rdar.testRead();
   if( !ret.empty() )
   { ret += " - fields must be readed from TGEM2MT structure";
     Error( "Error", ret);
   }

 // realloc memory
   Alloc();
 // read arrays
   int ii, jj, indx;

   TReadArrays  rddar( 3, TUnSpace_dynamic_fields, ff);
  // Set up flags
 //  if( mtp->PsMode != GMT_MODE_W && mtp->PsMode != GMT_MODE_V )
 //     rddar.setNoAlws( 1 /*"NPmean"*/);
 //  if( mtp->PvGrid == S_OFF )
 //     rddar.setNoAlws( 2 /*"mGrid"*/);

   nfild = rddar.findNext();
   while( nfild >=0 )
   {
     switch( nfild )
     {
// line %5s "#", %6s "Init", %6s "Type",
//   %12s "Vt-m**3", %12s "vp-m/sec", %12s "porosity", %12s "Km-m**2",
//   %12s "al-m", %12s "hDl-m**2/s", %12s "nto"
// list #DiCp %5g index, %6g all #DiCp, %12.6g all #HydP
/*       case 0:  rddar.skipSpace();
               for( ii=0; ii<mtp->nC; ii++ )
               {
                 ff >> indx >> mtp->DiCp[ii][0] >> mtp->DiCp[ii][1];
                 for( jj=0; jj< SIZE_HYDP; jj++)
                    ff >> mtp->HydP[ii][jj];
               }
            break;
       case 1:  rddar.skipSpace();
           if( mtp->PsMode == GMT_MODE_W || mtp->PsMode == GMT_MODE_V )
// line ## prn=:mtPsfl[0] = "W" | mtPsfl[0] = "V"; ##
//    %5s "#", %6s "Mean", %6s "Min", %6s "Max",
//    %6s "ptype", %6s "mmode", %6s "tcode", %6s "ips", %6s "res", %6s "res"
// list #NPmean %5g index, %6g #NPmean, %6g #nPmin ,%6g #nPmax ,
//    %6g all #ParTD
              for( ii=0; ii<mtp->nPTypes; ii++ )
              {
                ff >> indx >> mtp->NPmean[ii];
                ff >> mtp->nPmin[ii] >> mtp->nPmax[ii];
                for( jj=0; jj< 6; jj++)
                   ff >> mtp->ParTD[ii][jj];
              }
            break;
       case 2:  rddar.skipSpace();
          if( mtp->PvGrid != S_OFF )
//     line ## prn=:mtPvfl[8] <> "-"; ##  %s "Grid"
//     list #mGrid  %12.6g all #mGrid
             for( ii=0; ii<mtp->nPTypes; ii++ )
               ff >> mtp->grid[ii][0]  >> mtp->grid[ii][1] >> mtp->grid[ii][2];
          break;
*/     }
     nfild = rddar.findNext();
   }
   ret = rddar.testRead();
   if( !ret.empty() )
   { ret += " - fields must be readed from TGEM2MT structure";
     Error( "Error", ret);
   }

   return 0;
  }
  catch(TError& err)
  {
      f_log << err.title.c_str() << "  : " << err.mess.c_str() << endl;
  }
  return 1;
}

void TUnSpace::Alloc()
{
/*  mtp->DiCp = new short[mtp->nC][2];
  mtp->HydP = new double[mtp->nC][SIZE_HYDP];
  if( mtp->PsMode == GMT_MODE_W || mtp->PsMode == GMT_MODE_V )
  {
    mtp->NPmean = new short[ mtp->nPTypes ];
    mtp->nPmin =  new short[ mtp->nPTypes ];
    mtp->nPmax =  new short[ mtp->nPTypes ];
    mtp->ParTD = new short[ mtp->nPTypes][6];
    if( mtp->PvGrid != S_OFF )
      mtp->grid = new float[mtp->nC][3];
  }
*/}

void TUnSpace::Free()
{
/*  if( mtp->DiCp  )
    delete[] mtp->DiCp;
  if( mtp->HydP  )
     delete[] mtp->HydP;
  if( mtp->NPmean  )
    delete[] mtp->NPmean;
  if( mtp->nPmin  )
    delete[] mtp->nPmin;
  if( mtp->nPmax  )
    delete[] mtp->nPmax;
  if( mtp->ParTD  )
    delete[] mtp->ParTD;
  if( mtp->grid  )
    delete[] mtp->grid;
*/    
}

//realloc dynamic memory for work arrays
void TUnSpace::phase_lists_new()
{

// alloc memory for nPhA size
/*
if( usp->nPhA > 0 )
  {
    usp->PhAndx = (short *)aObj[ o_unphndx].Alloc( usp->nPhA, usp->N, I_);
    usp->PhNum = (short *)aObj[ o_unphnum].Alloc( usp->nPhA, 1 , I_ );
    usp->PhAID = (char (*)[8])aObj[ o_unphaid].Alloc( usp->nPhA, 1, 8 );
    usp->PhAlst = (char (*)[80])aObj[ o_unphalst].Alloc( usp->nPhA, 1, 80 );
    usp->PhAfreq = (float *)aObj[ o_unafreg].Alloc( usp->nPhA, 1, F_ );
  }
else
  {  usp->PhAndx = (short *)aObj[ o_unphndx].Free();
     usp->PhNum = (short *)aObj[ o_unphnum].Free();
     usp->PhAID = (char (*)[8])aObj[ o_unphaid].Free();
     usp->PhAlst = (char (*)[80])aObj[ o_unphalst].Free();
     usp->PhAfreq = (float *)aObj[ o_unafreg].Free();
  }
*/
}

//---------------------------------------------------------------------------

