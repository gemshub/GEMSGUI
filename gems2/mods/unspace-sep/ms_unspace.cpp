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

//realloc dynamic memory for work arrays (do not reading)
void TUnSpace::phase_lists_new()
{
// alloc memory for nPhA size
if( usp->nPhA > 0 )
  {
    usp->PhAndx = new short[usp->nPhA*usp->N];
    usp->PhNum = new short[usp->nPhA];
    usp->PhAID = new char[usp->nPhA][8];
    usp->PhAlst = new char[usp->nPhA][80];
    usp->PhAfreq = new float[ usp->nPhA];
  }
}

//realloc dynamic memory for work arrays (do not reading)
void TUnSpace::work_dyn_new()
{
//  work (not in record)
    usp->A = new float[ usp->L* usp->N];
    usp->sv = new short[ usp->Q];

    usp->Zcp = new double[ usp->Q];
    usp->Zmin = new double[ usp->Q];
    usp->Zmax = new double[ usp->Q];
    usp->ZmaxAbs = new double[ usp->Q];
    usp->Hom = new double[ usp->Q];
    usp->Prob = new double[ usp->Q];

    usp->pmr = 0;
    if( usp->PvPOR == S_ON )
    {  usp->POR = new float[ usp->Q];
       usp->pmr = usp->POR;
    }
    
    if( usp->PvPOM == S_ON )
    {
      usp->POM = new float[ usp->Q*usp->Q]; 
      usp->pmr = usp->POM;
    }

  usp->UnIC = new double[usp->N][UNSP_SIZE1];
  usp->UgDC = new double[usp->nPG][UNSP_SIZE1];
  usp->UaDC = new double[usp->Ls][UNSP_SIZE1];
  usp->UnDCA = new double[usp->nPG][UNSP_SIZE2];
}

//realloc dynamic memory for work arrays (reading)
void TUnSpace::nG_dyn_new()
{
	usp->nG = usp->nGB +usp->nGR+usp->nGN; 	
  if(usp->nG)
  {
    usp->PbD =  new short[ usp->nG];
    usp->SGp =  new char[ usp->nG][MAXPHNAME];
    usp->ncp =  new float [ usp->Q* usp->nG];
  }
  if(usp->nGB)
    usp->OVB = new float[ usp->nGB+1 ];
  if(usp->nGR)
    usp->OVR = new float[ usp->nGR+1];
  if(usp->nGN)
    usp->OVN = new float[ usp->nGN+1];

  if( usp->nPG )
    usp->ParNames = new char[usp->nPG][PARNAME_SIZE];
}

// realloc dynamic memory
void TUnSpace::Alloc()
{

    usp->UnICn = new char[UNSP_SIZE1][NAME_SIZE];
    usp->UgDCn = new char[UNSP_SIZE1][NAME_SIZE];
    usp->UaDCn = new char[UNSP_SIZE1][NAME_SIZE];
    usp->UnDCAn = new char[UNSP_SIZE2][NAME_SIZE];
  // allocation memory for arrays

  if(usp->PsGen[0] == S_ON )
  {
    usp->Gs = new float[usp->L][2];
    usp->NgLg = new short[usp->L];
    usp->IntLg = new float[usp->L][2];
    
    // do not read 
    usp->vG = new double[ usp->Q* usp->L];
    usp->vY = new double[ usp->Q* usp->L];
    usp->vYF = new double[ usp->Q* usp->Fi];
    usp->vGam = new double[ usp->Q* usp->L];
    usp->vMol = new double[ usp->Q* usp->N];
    usp->vU = new double[ usp->Q* usp->N];
    usp->vpH = new float[usp->Q][3];
    usp->vT = new float[usp->Q];
    usp->vP = new float[usp->Q];
    usp->vV = new float[usp->Q];

    usp->qQ = (short)(usp->quan_lev*usp->Q);
    if(usp->qQ<1)
        usp->qQ=1;
    usp->quanCx = new short[usp->Q][4];
    usp->quanCv = new float[usp->Q][4];
  }

  if(usp->PsGen[0] == S_ON  && usp->Pa_f_mol == S_ON)
  {
    usp->m_t_lo = new float[ usp->N];
    usp->m_t_up = new float[ usp->N];
  }

  // do not read 
  if( usp->PsGen[0] == S_ON  && usp->Ls )
       usp->vFug = new double[ usp->Q* usp->Ls];

  if( usp->PsGen[0] == S_ON  && usp->Ls && usp->Pa_f_fug== S_ON )
  {
    usp->fug_lo = new float[ usp->Ls];
    usp->fug_up = new float[ usp->Ls];
  }

  if(usp->PsGen[1]== S_ON)
  {
    usp->Ss = new float[usp->L][2];
    usp->NgLs = new short[usp->L];
    usp->IntLs = new float[usp->L][2];
    usp->vS = new double[ usp->Q* usp->L]; // do not read
  }
  
  if(usp->PsGen[5]== S_ON)
  {
    usp->Vs = new float[usp->L][2];
    usp->NgLv = new short[usp->L];
    usp->IntLv = new float[usp->L][2];
    usp->vmV = new double[ usp->Q* usp->L]; // do not read
  }

  if(usp->PsGen[2]== S_ON)
  {
    usp->NgNb = new short[usp->N];
    usp->IntNb = new float[usp->N][2];
    usp->Bs = new double[usp->N][2];
    usp->vB = new double[ usp->Q* usp->N];  // do not read
  } 

  if(usp->PsGen[6]== S_ON && usp->Ls )   
  {
    usp->NgGam = new short[usp->Ls];
    usp->IntGam = new float[usp->Ls][2];
    usp->GAMs = new float[usp->Ls][2];
    usp->vNidP = new double[ usp->Q* usp->Ls]; // do not read
  } 
  
  if(/*usp->PsGen[0??] == S_ON  &&*/ usp->Pa_f_pha == S_ON)
    usp->f_PhA = new short[ usp->N ];

   nG_dyn_new();               
   work_dyn_new();
   phase_lists_new();
}

void TUnSpace::Free()
{
  if( usp->PhAndx  )
    delete[] usp->PhAndx;
  if( usp->PhNum  )
    delete[] usp->PhNum;
  if( usp->PhAID  )
     delete[] usp->PhAID;
  if(usp->PhAlst  )
      delete[] usp->PhAlst;
  if( usp->PhAfreq  )
      delete[] usp->PhAfreq;

  //  work (not in record)
  if( usp->A  )
     delete[] usp->A;
  if( usp->sv  )
     delete[] usp->sv;
  if( usp->Zcp  )
    delete[] usp->Zcp;
  if( usp->Zmin  )
      delete[] usp->Zmin;
  if( usp->Zmax  )
    delete[] usp->Zmax;
  if( usp->ZmaxAbs  )
     delete[] usp->ZmaxAbs;
  if(usp->Hom  )
      delete[] usp->Hom;
  if( usp->Prob  )
      delete[] usp->Prob;
   if( usp->POR  )
      delete[] usp->POR;
   if( usp->POM  )
      delete[] usp->POM;
  if( usp->UnIC  )
    delete[] usp->UnIC;
  if( usp->UgDC  )
      delete[] usp->UgDC;
  if( usp->UaDC  )
    delete[] usp->UaDC;
  if( usp->UnDCA  )
     delete[] usp->UnDCA;

//realloc dynamic memory for work arrays (reading)
   if( usp->PbD )
      delete[] usp->PbD;
   if( usp->SGp  )
      delete[] usp->SGp;
   if( usp->ncp  )
     delete[] usp->ncp;
   if( usp->OVB  )
       delete[] usp->OVB;
   if( usp->OVR  )
     delete[] usp->OVR;
   if( usp->OVN  )
      delete[] usp->OVN;
   if(usp->ParNames  )
       delete[] usp->ParNames;

// allocation memory for arrays
   if( usp->UnICn )
      delete[] usp->UnICn;
   if( usp->UgDCn  )
      delete[] usp->UgDCn;
   if( usp->UaDCn  )
     delete[] usp->UaDCn;
   if( usp->UnDCAn  )
       delete[] usp->UnDCAn;
   
   if( usp->Gs )
      delete[] usp->Gs;
   if( usp->NgLg  )
      delete[] usp->NgLg;
   if( usp->IntLg  )
     delete[] usp->IntLg;
   if( usp->vG  )
       delete[] usp->vG;
  if( usp->vY)
     delete[] usp->vY;
  if( usp->vYF  )
     delete[] usp->vYF;
  if( usp->vGam  )
    delete[] usp->vGam;
  if( usp->vMol  )
      delete[] usp->vMol;
   if( usp->vU )
      delete[] usp->vU;
   if( usp->vpH  )
      delete[] usp->vpH;
   if( usp->vT  )
     delete[] usp->vT;
   if( usp->vP  )
       delete[] usp->vP;
   if( usp->vV )
      delete[] usp->vV;
   if( usp->quanCx  )
      delete[] usp->quanCx;
   if( usp->quanCv  )
     delete[] usp->quanCv;
   if( usp->m_t_lo  )
       delete[] usp->m_t_lo;
  if( usp->m_t_up)
     delete[] usp->m_t_up;
  if( usp->vFug  )
     delete[] usp->vFug;
  if( usp->fug_lo  )
    delete[] usp->fug_lo;
  if( usp->fug_up  )
      delete[] usp->fug_up;
   if( usp->Ss )
      delete[] usp->Ss;
   if( usp->NgLs  )
      delete[] usp->NgLs;
   if( usp->IntLs  )
     delete[] usp->IntLs;
   if( usp->vS  )
       delete[] usp->vS;
    if( usp->Vs  )
       delete[] usp->Vs;
    if( usp->NgLv  )
      delete[] usp->NgLv;
    if( usp->IntLv  )
        delete[] usp->IntLv;
   if( usp->vmV)
      delete[] usp->vmV;
   if( usp->NgNb  )
      delete[] usp->NgNb;
   if( usp->IntNb  )
     delete[] usp->IntNb;
   if( usp->Bs  )
       delete[] usp->Bs;
   if( usp->vB )
       delete[] usp->vB;
   if( usp->NgGam  )
       delete[] usp->NgGam;
   if( usp->IntGam  )
      delete[] usp->IntGam;
   if( usp->GAMs  )
        delete[] usp->GAMs;
   if( usp->vNidP  )
        delete[] usp->vNidP;
   if( usp->f_PhA  )
       delete[] usp->f_PhA;
}

//---------------------------------------------------------------------------

