#include <iomanip>
#include <iostream>

#include "m_param.h"
#include "node.h"

#ifdef IPMGEMPLUGIN
  istream& f_getline(istream& is, gstring& str, char delim);
#endif

// skip  ' ',  '\n', '\t' and comments (from '#' to end of line)
void  skipSpace( fstream& ff )
{
  char input;
  ff.get( input );
  while( input == '#' || input == ' ' ||
        input == '\n' || input == '\t')
 {
   if( input == '#' )
    do{
         ff.get( input );
      }while( input != '\n' && input != '\0');
   if( input == '\0' )
     return;
   ff.get( input );
  }
 ff.putback(input);
}

void inArray( fstream& ff, char *name, short* arr, int size )
{
 char buf[200];
 skipSpace( ff );
 ff >> buf;
 if( memcmp( name, buf+1, strlen(name)))
    Error( buf, "DataBR text read 01: Invalid name of (int*2) array");
 for( int ii=0; ii<size; ii++  )
 {
   skipSpace( ff );
   ff >> arr[ii];
 }
}

void inArray( fstream& ff, char *name, float* arr, int size )
{
 char buf[200];
 skipSpace( ff );
 ff >> buf;
 if( memcmp( name, buf+1, strlen(name)))
    Error( buf, "DataBR text read 02: Invalid name of float (real*4) array");
 for( int ii=0; ii<size; ii++  )
 {
     skipSpace( ff );
     ff >> arr[ii];
 }
}

void inArray( fstream& ff, char *name, double* arr, int size )
{
 char buf[200];
 skipSpace( ff );
 ff >> buf;
 if( memcmp( name, buf+1, strlen(name)))
    Error( buf, "DataBR text read 03: Invalid name of double (real*8) array");
 for( int ii=0; ii<size; ii++  )
 {
     skipSpace( ff );
     ff >> arr[ii];
 }
}

void inArray( fstream& ff, char *name, char* arr,
                              int size, int el_size )
{
 char ch;
 char buf[200];
 skipSpace( ff );
 ff >> buf;
 if( memcmp( name, buf+1, strlen(name)))
    Error( buf, "DataBR text read 04: Invalid name of char[el_size] array");

 for( int ii=0; ii<size; ii++  )
 {
   skipSpace( ff );
   ff.get(ch);
   while( ff.good() && ch != '\'' )
       ff.get(ch);
   ff.getline( buf, el_size+1, '\'');
   strncpy( arr +(ii*el_size), buf, el_size );
 }

}

//---------------------------------------------------------//
// for test out data
void outArray( fstream& ff, char *name, char* arr,
                              int size, int arr_siz )
{
 ff << endl << "<" << name << ">" << endl;
 for( int ii=0, jj=0; ii<size; ii++, jj++  )
 {
    if(jj == 10)
    { jj=0;  ff << endl;}
    gstring str = gstring( arr +(ii*arr_siz), 0, arr_siz );
    str.strip();
    ff  << "\'" << str.c_str() << "\'" << " ";
 }
}

void outArray( fstream& ff, char *name, short* arr, int size )
{
 ff << endl << "<" << name << ">" << endl;
 for( int ii=0, jj=0; ii<size; ii++, jj++  )
 {
    if(jj == 10)
    { jj=0;  ff << endl;}
    ff << arr[ii] << " ";
 }
}

void outArray( fstream& ff, char *name,  float* arr,
            int size, int l_size )
{
 int sz = 10;
 if( l_size > 0 )
       sz = l_size;

 ff << endl << "<" << name << ">" << endl;
 for( int ii=0, jj=0; ii<size; ii++, jj++  )
 {
    if(jj == sz)
    { jj=0;  ff << endl;}
    ff << setprecision(10) << scientific << arr[ii] << " ";
 }
}

void outArray( fstream& ff, char *name,  double* arr,
            int size, int l_size )
{
 int sz = 10;
 if( l_size > 0 )
       sz = l_size;

 ff << endl << "<" << name << ">" << endl;
 for( int ii=0, jj=0; ii<size; ii++, jj++  )
 {
    if(jj == sz)
    { jj=0;  ff << endl;}
    ff << setprecision(18) << scientific << arr[ii] << " ";
 }
}

//===================================================================

void TMulti::to_text_file_gemipm( const char *path )
{
  SPP_SETTING *pa = &TProfil::pm->pa;

   //static values
   char PAalp;
   char PSigm;
   float EpsW;
   float RoW;

#ifndef IPMGEMPLUGIN
   PAalp = syp->PAalp;
   PSigm = syp->PSigm;
   EpsW = TProfil::pm->tpp->EpsW;
   RoW = TProfil::pm->tpp->RoW;
#else
   PAalp = PAalp_;
   PSigm = PSigm_;
   EpsW = EpsW_;
   RoW = RoW_;
#endif
  fstream ff( path, ios::out );
  ErrorIf( !ff.good() , path, "Fileopen error");

   ff << "\"" << pmp->stkey << "\"" << endl;
// short from pa
   ff << left << setw(12) << "<pa_DP> " << right << setw(8) << pa->p.DP << endl;
   ff << left << setw(12) << "<pa_DW> " << right << setw(8) << pa->p.DW  << endl;
   ff << left << setw(12) << "<pa_DT> " << right << setw(8) << pa->p.DT  << endl;
   ff << left << setw(12) << "<pa_IIM> " << right << setw(8) <<  pa->p.IIM << endl;
   ff << left << setw(12) << "<pa_DG> " <<  right << setw(8) << pa->p.DG << endl;
   ff << left << setw(12) << "<pa_DHB> " << right << setw(8) <<  pa->p.DHB << endl;
   ff << left << setw(12) << "<pa_DS> " << right << setw(8) <<  pa->p.DS << endl;
   ff << left << setw(12) << "<pa_DK> " <<  right << setw(8) << pa->p.DK << endl;
   ff << left << setw(12) << "<pa_DF> " <<  right << setw(8) << pa->p.DF << endl;
   ff << left << setw(12) << "<pa_DB> " <<  right << setw(8) << pa->p.DB << endl;
   ff << left << setw(12) << "<pa_GAS> " << right << setw(8) <<  pa->p.GAS << endl;
   ff << left << setw(12) << "<pa_EPS> " <<  right << setw(8) << pa->p.EPS << endl;
   ff << left << setw(12) << "<pa_PC> " <<  right << setw(8) << pa->p.PC << endl;
   ff << left << setw(12) << "<pa_DFM> " <<  right << setw(8) << pa->p.DFM << endl;
   ff << left << setw(12) << "<pa_DFYw> " <<  right << setw(8) << pa->p.DFYw << endl;
   ff << left << setw(12) << "<pa_DFYaq> " <<  right << setw(8) << pa->p.DFYaq << endl;
   ff << left << setw(12) << "<pa_DFYid> " <<  right << setw(8) << pa->p.DFYid << endl;
   ff << left << setw(12) << "<pa_DFYr> " <<  right << setw(8) << pa->p.DFYr << endl;
   ff << left << setw(12) << "<pa_DFYh> " <<  right << setw(8) << pa->p.DFYh << endl;
   ff << left << setw(12) << "<pa_DFYc> " <<  right << setw(8) << pa->p.DFYc << endl;
   ff << left << setw(12) << "<pa_DFYs> " << right << setw(8) <<  pa->p.DFYs << endl;
   ff << left << setw(12) << "<pa_XwMin> " <<  right << setw(8) << pa->p.XwMin << endl;
   ff << left << setw(12) << "<pa_ScMin> " <<  right << setw(8) << pa->p.ScMin << endl;
   ff << left << setw(12) << "<pa_DcMin> " <<  right << setw(8) << pa->p.DcMin << endl;
   ff << left << setw(12) << "<pa_PhMin> " <<  right << setw(8) << pa->p.PhMin << endl;
   ff << left << setw(12) << "<pa_ICmin> " <<  right << setw(8) << pa->p.ICmin << endl;
   ff << left << setw(12) << "<pa_PD> " <<  right << setw(8) << pa->p.PD << endl;
   ff << left << setw(12) << "<pa_PRD> " <<  right << setw(8) << pa->p.PRD << endl;
   ff << left << setw(12) << "<pa_PSM> " <<  right << setw(8) << pa->p.PSM << endl;
   ff << left << setw(12) << "<pa_AG> " <<  right << setw(8) << pa->p.AG << endl;
   ff << left << setw(12) << "<pa_DGC> " <<  right << setw(8) << pa->p.DGC << endl;
   ff << left << setw(12) << "<pa_GAR> " <<  right << setw(8) << pa->p.GAR << endl;
   ff << left << setw(12) << "<pa_GAH> " <<  right << setw(8) << pa->p.GAH << endl;
   ff << left << setw(12) << "<pa_IEPS> " <<  right << setw(8) << pa->p.IEPS << endl;
   ff << left << setw(12) << "<pa_DKIN> " <<  right << setw(8) << pa->p.DKIN << endl;
   ff << left << setw(12) << "<pa_PLLG> " <<  right << setw(8) << pa->p.PLLG << endl;
   ff << left << setw(12) << "<pa_PE> " <<  right << setw(8) << pa->p.PE << endl;
   ff << left << setw(12) << "<pa_DNS> " <<  right << setw(8) << pa->p.DNS << endl;

// const from multi
   ff << left << setw(12) << "<PAalp> " <<  right << setw(6) <<
      "\'" << PAalp << "\'" << endl;
   ff << left << setw(12) << "<PSigm> " <<  right << setw(6) <<
      "\'" << PSigm << "\'" << endl;
   ff << left << setw(12) << "<Lads> " <<  right << setw(8) << pmp->Lads << endl;
   ff << left << setw(12) << "<FIa> " <<  right << setw(8) << pmp->FIa << endl;
   ff << left << setw(12) << "<FIat> " <<  right << setw(8) << pmp->FIat << endl;
   ff << left << setw(12) << "<sitNc> " <<  right << setw(8) << pmp->sitNcat << endl;
   ff << left << setw(12) << "<sitNa> " <<  right << setw(8) << pmp->sitNan << endl;
   ff << left << setw(12) << "<E> " <<  right << setw(8) << pmp->E << endl;
   ff << left << setw(12) << "<PV> " <<  right << setw(8) << pmp->PV << endl;
   ff << left << setw(12) << "<pKin> " <<  right << setw(8) << pmp->PLIM << endl;
//   ff << left << setw(12) << "<Ls> " <<  right << setw(8) << pmp->Ls << endl;
//   ff << left << setw(12) << "<LO> " <<  right << setw(8) << pmp->LO << endl;
//   ff << left << setw(12) << "<PG> " <<  right << setw(8) << pmp->PG << endl;
   ff << left << setw(12) << "<PSOL> " <<  right << setw(8) << pmp->PSOL << endl;
// double from Multi
   ff << left << setw(12) << "<GWAT> " <<  right << setw(8) << pmp->GWAT << endl;
   ff << left << setw(12) << "<EpsW> " <<  right << setw(8) << EpsW << endl;
   ff << left << setw(12) << "<RoW\'  " <<  right << setw(8) << RoW << endl;
// outArray( ff, "FitVar",  pmp->FitVar, 5 );

//dynamic values
    // Part 1
   outArray( ff, "B", pmp->B,  pmp->N);
   outArray( ff, "DUL", pmp->DUL,  pmp->L);
   outArray( ff, "DLL", pmp->DLL,  pmp->L);
    /* need  always to alloc vectors */
   outArray( ff, "Pparc", pmp->Pparc,  pmp->L);
   outArray( ff, "G", pmp->G,  pmp->L);
   outArray( ff, "GEX", pmp->GEX,  pmp->L);
   outArray( ff, "lnGmf", pmp->lnGmf,  pmp->L);
   outArray( ff, "YOF", pmp->YOF,  pmp->FI);

   // Part 2  not always required arrays
    if( pmp->FIs > 0 && pmp->Ls > 0 )
    {
      outArray( ff, "LsMod", pmp->LsMod, pmp->FIs);
      outArray( ff, "LsMdc", pmp->LsMdc, pmp->FIs);
      int LsModSum = 0;
      int LsMdcSum = 0;
      for(int i=0; i<pmp->FIs; i++)
      {
        LsModSum += pmp->LsMod[i];
        LsMdcSum += (pmp->LsMdc[i]*pmp->L1[i]);
      }
      if(LsModSum )
       outArray( ff, "PMc", pmp->PMc,  LsModSum);
      if(LsMdcSum )
       outArray( ff, "DMc", pmp->DMc,  LsMdcSum);
    }

    /* dispersed and sorption phases */
    if( PSigm != S_OFF )
    {
      outArray( ff, "Sigw", pmp->Sigw,  pmp->FI);
      outArray( ff, "Sigg", pmp->Sigg,  pmp->FI);
    }
    if( pmp->E )
      outArray( ff, "EZ", pmp->EZ,  pmp->L);

    if( pm.FIat > 0 && /*pm.Lads > 0 &&Sveta 12/09/99*/ pm.FIs > 0 )
    { /* ADSORPTION AND ION EXCHANGE */
      outArray( ff, "Nfsp", &pmp->Nfsp[0][0], pmp->FIs*pmp->FIat);
      outArray( ff, "MASDT", &pmp->MASDT[0][0], pmp->FIs*pmp->FIat);
      outArray( ff, "C1", &pmp->XcapA[0][0], pmp->FIs*pmp->FIat);
      outArray( ff, "C2", &pmp->XcapB[0][0], pmp->FIs*pmp->FIat);
      outArray( ff, "C3", &pmp->XcapF[0][0], pmp->FIs*pmp->FIat);
      outArray( ff, "pCh", &pmp->Xetaf[0][0], pmp->FIs*pmp->FIat);

      outArray( ff, "SATX", &pmp->SATX[0][0], pmp->Lads*4);
//      outArray( ff, "MASDJ", pm.MASDJ, pm.Ls);
      outArray( ff, "MASDJ", &pmp->MASDJ[0][0], pmp->Lads*DFCN);
    }

   if( pm.sitNcat*pm.sitNcat )
     outArray( ff, "sitE", pmp->sitE, pmp->sitNcat*pmp->sitNan );
   if( pm.sitNcat )
     outArray( ff, "sitXc", pmp->sitXcat, pmp->sitNcat );
   if( pm.sitNan )
      outArray( ff, "sitXa", pmp->sitXan, pmp->sitNan );

// character
   outArray( ff, "RLC", pmp->RLC, pmp->L, 1 );
   outArray( ff, "RSC", pmp->RSC, pmp->L, 1 );
   if( pm.FIs > 0 && pm.Ls > 0 )
     outArray( ff, "sMod", pmp->sMod[0], pmp->FIs, 6 );
 if( pm.FIat > 0 && /*pm.Lads > 0 &&Sveta 12/09/99*/ pm.FIs > 0 )
 {
   outArray( ff, "SCM", pmp->SCM[0], pmp->FIs, pmp->FIat );
   outArray( ff, "SACT", pmp->SATT, pmp->Lads, 1 );
   outArray( ff, "DCads", pmp->DCC3, pmp->Lads, 1 );
 }
}

void TMulti::from_text_file_gemipm( const char *path )
{
  SPP_SETTING *pa = &TProfil::pm->pa;
  DATACH  *dCH = TNode::na->pCSD();
  int ii;

   //static values
   char PAalp;
   char PSigm;
   float EpsW;
   float RoW;

  // get sizes from DATACH
  pmp->N = pmp->NR = dCH->nIC;
  pmp->L = dCH->nDC;
  pmp->FI = dCH->nPH;
  pmp->FIs = dCH->nPS;
  pmp->Ls = 0;
  for( ii=0; ii<dCH->nPS; ii++)
  {
    pmp->Ls += dCH->nDCinPH[ii];
    if( dCH->ccPH[ii] == 'a' )
     pmp->LO = pmp->Ls-1;
    if( dCH->ccPH[ii] == 'g' || dCH->ccPH[ii] == 'p' || dCH->ccPH[ii] == 'f')
      pmp->PG = dCH->nDCinPH[ii];
  }
  // read sizes and constants from txt file
  fstream ff( path, ios::in );
  ErrorIf( !ff.good() , path, "Fileopen error");

   gstring str;
   skipSpace( ff );
   f_getline( ff, str, '\n');
   memcpy( pmp->stkey, str.c_str(), EQ_RKLEN );
// read to pa
   inArray( ff,"pa_DP", &pa->p.DP, 1);
   inArray( ff,"pa_DW", &pa->p.DW , 1);
   inArray( ff,"pa_DT", &pa->p.DT , 1);
   inArray( ff,"pa_IIM", &pa->p.IIM, 1);
   inArray( ff,"pa_DG" , &pa->p.DG, 1);
   inArray( ff,"pa_DHB", &pa->p.DHB, 1);
   inArray( ff,"pa_DS", &pa->p.DS, 1);
   inArray( ff,"pa_DK" , &pa->p.DK, 1);
   inArray( ff,"pa_DF" , &pa->p.DF, 1);
   inArray( ff,"pa_DB" , &pa->p.DB, 1);
   inArray( ff,"pa_GAS", &pa->p.GAS, 1);
   inArray( ff,"pa_EPS" , &pa->p.EPS, 1);
   inArray( ff,"pa_PC" , &pa->p.PC, 1);
   inArray( ff,"pa_DFM" , &pa->p.DFM, 1);
   inArray( ff,"pa_DFYw" , &pa->p.DFYw, 1);
   inArray( ff,"pa_DFYaq" , &pa->p.DFYaq, 1);
   inArray( ff,"pa_DFYid" , &pa->p.DFYid, 1);
   inArray( ff,"pa_DFYr" , &pa->p.DFYr, 1);
   inArray( ff,"pa_DFYh" , &pa->p.DFYh, 1);
   inArray( ff,"pa_DFYc" , &pa->p.DFYc, 1);
   inArray( ff,"pa_DFYs", &pa->p.DFYs, 1);
   inArray( ff,"pa_XwMin" , &pa->p.XwMin, 1);
   inArray( ff,"pa_ScMin" , &pa->p.ScMin, 1);
   inArray( ff,"pa_DcMin" , &pa->p.DcMin, 1);
   inArray( ff,"pa_PhMin" , &pa->p.PhMin, 1);
   inArray( ff,"pa_ICmin" , &pa->p.ICmin, 1);
   inArray( ff,"pa_PD" , &pa->p.PD, 1);
   inArray( ff,"pa_PRD" , &pa->p.PRD, 1);
   inArray( ff,"pa_PSM" , &pa->p.PSM, 1);
   inArray( ff,"pa_AG" , &pa->p.AG, 1);
   inArray( ff,"pa_DGC" , &pa->p.DGC, 1);
   inArray( ff,"pa_GAR" , &pa->p.GAR, 1);
   inArray( ff,"pa_GAH" , &pa->p.GAH, 1);
   inArray( ff,"pa_IEPS" , &pa->p.IEPS, 1);
   inArray( ff,"pa_DKIN" , &pa->p.DKIN, 1);
   inArray( ff,"pa_PLLG" , &pa->p.PLLG, 1);
   inArray( ff,"pa_PE" , &pa->p.PE, 1);
   inArray( ff,"pa_DNS" , &pa->p.DNS, 1);
// to multi
   inArray( ff,"PAalp" , &PAalp, 1, 1);
   inArray( ff,"PSigm" , &PSigm, 1, 1);
   inArray( ff,"Lads" , &pmp->Lads, 1);
   inArray( ff,"FIa" , &pmp->FIa, 1);
   inArray( ff,"FIat" , &pmp->FIat, 1);
   inArray( ff,"sitNc" , &pmp->sitNcat, 1);
   inArray( ff,"sitNa" , &pmp->sitNan, 1);
   inArray( ff,"E" , &pmp->E, 1);
   inArray( ff,"PV" , &pmp->PV, 1);
   inArray( ff,"pKin" , &pmp->PLIM, 1);
//   inArray( ff,"Ls" , &pmp->Ls, 1);
//   inArray( ff,"LO" , &pmp->LO, 1);
//   inArray( ff,"PG" , &pmp->PG, 1);
   inArray( ff,"PSOL" , &pmp->PSOL, 1);
// double from Multi
   inArray( ff,"GWAT" , &pmp->GWAT, 1);
   inArray( ff,"EpsW" , &EpsW, 1);
   inArray( ff,"RoW" , &RoW, 1);
// outArray( ff,"FitVar",  pmp->FitVar, 5 );

#ifndef IPMGEMPLUGIN
//   syp->PAalp = PAalp;
//   syp->PSigm = PSigm;
#else
   PAalp_ = PAalp;
   PSigm_ = PSigm;
   EpsW_ = EpsW;
   RoW_ =  RoW;
#endif

   //realloc memory
#ifdef IPMGEMPLUGIN
   multi_realloc( PAalp, PSigm );
#endif

// get dynamic data from DATACH file
  for( ii=0; ii<dCH->nPH; ii++)
    pmp->L1[ii] = dCH->nDCinPH[ii];
  memcpy( pmp->A, dCH->A, dCH->nIC*dCH->nDC*sizeof(float));
  for( ii=0; ii< dCH->nIC; ii++ )
   pmp->Awt[ii]  = dCH->ICmm[ii];
  if( dCH->nAalp >0 )
    for( ii=0; ii< dCH->nPH; ii++ )
      pmp->Aalp[ii] = dCH->Aalp[ii];
  memcpy( pmp->MM, dCH->DCmm, dCH->nDC*sizeof(double));

  memcpy( pmp->SB, dCH->ICNL, MaxICN*dCH->nIC*sizeof(char));
  memcpy( pmp->SM, dCH->DCNL, MaxDCN*dCH->nDC*sizeof(char));
  memcpy( pmp->SF, dCH->PHNL, MaxPHN*dCH->nPH*sizeof(char));

  memcpy( pmp->ICC, dCH->ccIC, dCH->nIC*sizeof(char));
  memcpy( pmp->DCC, dCH->ccDC, dCH->nDC*sizeof(char));
  memcpy( pmp->DCCW, dCH->ccDCW, dCH->nDC*sizeof(char));
  memcpy( pmp->PHC, dCH->ccPH, dCH->nPH*sizeof(char));

//read dynamic values from txt file
   inArray( ff, "B", pmp->B,  pmp->N);
   inArray( ff, "DUL", pmp->DUL,  pmp->L);
   inArray( ff, "DLL", pmp->DLL,  pmp->L);
    /* Part 1 need  always to alloc vectors */
   inArray( ff, "Pparc", pmp->Pparc,  pmp->L);
   inArray( ff, "G", pmp->G,  pmp->L);
   inArray( ff, "GEX", pmp->GEX,  pmp->L);
   inArray( ff, "lnGmf", pmp->lnGmf,  pmp->L);
   inArray( ff, "YOF", pmp->YOF,  pmp->FI);

   // Part 2  not always required arrays
    if( pmp->FIs > 0 && pmp->Ls > 0 )
    {
      inArray( ff, "LsMod", pmp->LsMod, pmp->FIs);
      inArray( ff, "LsMdc", pmp->LsMdc, pmp->FIs);
      int LsModSum = 0;
      int LsMdcSum = 0;
      for(int i=0; i<pmp->FIs; i++)
      {
        LsModSum += pmp->LsMod[i];
        LsMdcSum += (pmp->LsMdc[i]*pmp->L1[i]);
      }
      if(LsModSum )
       inArray( ff, "PMc", pmp->PMc,  LsModSum);
      if(LsMdcSum )
       inArray( ff, "DMc", pmp->DMc,  LsMdcSum);
    }
    /* dispersed and sorption phases */
    if( PSigm != S_OFF )
    {
      inArray( ff, "Sigw", pmp->Sigw,  pmp->FI);
      inArray( ff, "Sigg", pmp->Sigg,  pmp->FI);
    }
    if( pmp->E )
      inArray( ff, "EZ", pmp->EZ,  pmp->L);

    if( pm.FIat > 0 && /*pm.Lads > 0 &&Sveta 12/09/99*/ pm.FIs > 0 )
    { /* ADSORPTION AND ION EXCHANGE */
      inArray( ff, "Nfsp", &pmp->Nfsp[0][0], pmp->FIs*pmp->FIat);
      inArray( ff, "MASDT", &pmp->MASDT[0][0], pmp->FIs*pmp->FIat);
      inArray( ff, "C1", &pmp->XcapA[0][0], pmp->FIs*pmp->FIat);
      inArray( ff, "C2", &pmp->XcapB[0][0], pmp->FIs*pmp->FIat);
      inArray( ff, "C3", &pmp->XcapF[0][0], pmp->FIs*pmp->FIat);
      inArray( ff, "pCh", &pmp->Xetaf[0][0], pmp->FIs*pmp->FIat);

      inArray( ff, "SATX", &pmp->SATX[0][0], pmp->Lads*4);
      inArray( ff, "MASDJ", &pmp->MASDJ[0][0], pmp->Lads*DFCN);
    }

   if( pm.sitNcat*pm.sitNcat )
     inArray( ff, "sitE", pmp->sitE, pmp->sitNcat*pmp->sitNan );
   if( pm.sitNcat )
     inArray( ff, "sitXc", pmp->sitXcat, pmp->sitNcat );
   if( pm.sitNan )
      inArray( ff, "sitXa", pmp->sitXan, pmp->sitNan );

// character
 inArray( ff, "RLC", pmp->RLC, pmp->L, 1 );
 inArray( ff, "RSC", pmp->RSC, pmp->L, 1 );
 if( pm.FIs > 0 && pm.Ls > 0 )
   inArray( ff, "sMod", pmp->sMod[0], pmp->FIs, 6 );
 if( pm.FIat > 0 && /*pm.Lads > 0 &&Sveta 12/09/99*/ pm.FIs > 0 )
 {
   inArray( ff, "SCM", pmp->SCM[0], pmp->FIs, pmp->FIat );
   inArray( ff, "SACT", pmp->SATT, pmp->Lads, 1 );
   inArray( ff, "DCads", pmp->DCC3, pmp->Lads, 1 );
 }


}

//=============================================================================
