//#include <cmath>
//#include <cstdio>
#include "m_unspace.h"
#include "s_formula.h"
#include "visor.h"


//=========================================================================
// Generation task part
//=========================================================================

static short Prime[]=
    {
2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,
103,107,109,113,127,131,137,139,149,151,157,163,167,173,179,181,191,193,197,
199,211,223,227,229,233,239,241,251,257,263,269,271,277,281,283,293,307,311,
313,317,331,337,347,349,353,359,367,373,379,383,389,397,401,409,419,421,431,
433,439,443,449,457,461,463,467,479,487,491,499,503,509,521,523,541,547,557,
563,569,571,577,587,593,599,601,607,613,617,619,631,641,643,647,653,659,661,
673,677,683,691,701,709,719,727,733,739,743,751,757,761,769,773,787,797,809,
811,821,823,827,829,839,853,857,859,863,877,881,883,887,907,911,919,929,937,
941,947,953,967,971,977,983,991,997,1009,1013,1019,1021,1031,1033,1039,1049,
1051,1061,1063,1069,1087,1091,1093,1097,1103,1109,1117,1123,1129,1151,1153,
1163,1171,1181,1187,1193,1201,1213,1217,1223,1229,1231,1237,1249,1259,1277,
1279,1283,1289,1291,1297,1301,1303,1307,1319,1321,1327,1361,1367,1373,1381,
1399,1409,1423,1427,1429,1433,1439,1447,1451,1453,1459,1471,1481,1483,1487,
1489,1493,1499,1511,1523,1531,1543,1549,1553,1559,1567,1571,1579,1583,1597,
1601,1607,1609,1613,1619,1621,1627,1637,1657,1663,1667,1669,1693,1697,1699,
1709,1721,1723,1733,1741,1747,1753,1759,1777,1783,1787,1789,1801,1811,1823,
1831,1847,1861,1867,1871,1873,1877,1879,1889,1901,1907,1913,1931,1933,1949,
1951,1973,1979,1987,1993,1997,1999,2003,2011,2017,2027,2029,2039,2053,2063 };

short TUnSpace::near_prime_number( short num )
{
  int sizePrime = (sizeof(Prime)/sizeof(short));
  int i;
  for( i=0; i<sizePrime-1; i++)
   if( Prime[i] >= num )
           break;

  return Prime[i];
}

//set sizes of arrays
bool TUnSpace::test_sizes( )
{
    bool i=true;


   // the same sizes
   usp->N = TRMults::sm->GetMU()->N;
   usp->L = TRMults::sm->GetMU()->L;
   usp->Ls = TRMults::sm->GetMU()->Ls;
   usp->Fi = TRMults::sm->GetMU()->Fi;

   if( usp->Q<=0 )
   {
      i = false;
      usp->Q = 11;
   }
   usp->Q = near_prime_number( usp->Q );
   usp->qQ = (fabs(usp->quan_lev)*usp->Q);
   if(usp->qQ<1)
        usp->qQ=1;

   if( usp->nGB+usp->nGN+usp->nGR<=0 )
   {
      i = false;
      usp->nG = 2;
      usp->nGB =2;
   }

   if( usp->nGB+usp->nGN+usp->nGR != usp->nG )  // must be changed
   {
      usp->nG = usp->nGB+usp->nGN+usp->nGR;
   }
  // graphic
   if( usp->PsGraph != S_OFF )
   {
      usp->dimXY[0] = usp->Q;
      if( usp->dimXY[1] <= 0 )
      {
        i = false;
        usp->dimXY[1] = 3;
      }
      if( usp->dimEF[1] <= 0 || usp->dimEF[0] <= 0  )
      {
        i = false;
        usp->dimEF[0] = 10;
        usp->dimEF[1] = 5;
      }
   }

   usp->t = 0;
   usp->q = 0;
   usp->nPhA = 0;                                   // ????? nPhA test

   usp->Tc = usp->T[0];
   usp->Pc = usp->P[0];
   usp->Vc = usp->V[0];

   if( i==false )
        vfMessage(window(), GetName(),
   "E00UErem: Invalid dimensions in UnSpace data structure!", vfErr);

    return i;

}

//set default numbers to arrays ( PbD, NgLg .... )
void TUnSpace::set_def_data_to_arrays( bool mode )
{
  // must be done
    MTPARM* tpp = TMTparm::sm->GetTP();

    for(int i=0; i<usp->L; i++ )
    {
       if(usp->PsGen[0] == S_ON )
         usp->Gs[i][0] = usp->Gs[i][1] = tpp->G[i]+TSyst::sm->GetSY()->GEX[i];

       if(usp->PsGen[5] == S_ON )
         usp->Vs[i][0] = usp->Vs[i][1] = tpp->Vm[i];

       if(usp->PsGen[1] == S_ON && tpp->S )
         usp->Ss[i][0] = usp->Ss[i][1] = tpp->S[i];
     }

    if(usp->PsGen[2] == S_ON )
      for(int i=0; i<usp->N; i++ )
         usp->Bs[i][0] = usp->Bs[i][1] = TSyst::sm->GetSY()->B[i];

    usp->T[0] = usp->T[1]= tpp->T;
    usp->P[0] = usp->P[1]= tpp->P;
//      usp->V[0] = usp->V[1]= TProfil::pm->;

  // GAMs ??????

   if( usp->PsGraph != S_OFF )
    for(int j=0; j< usp->dimXY[1]+usp->dimEF[1]; j++ )
    {
        char tbuf[100];
        sprintf( tbuf, "%d", j+1 );
        if( !*usp->lNam[j]|| *usp->lNam[j] == ' ' )
            strncpy( usp->lNam[j], tbuf, MAXGRNAME );
    }

//  Temporary!
    strncpy( usp->UnICn[0], "Det.(m_t)",  NAME_SIZE );
    strncpy( usp->UnICn[1], "Filt<m_t>",  NAME_SIZE );
    strncpy( usp->UnICn[2], "2SigFilt",  NAME_SIZE );
    strncpy( usp->UnICn[3], "DomPA-m_t",  NAME_SIZE );
    strncpy( usp->UnICn[4], "2SigDomPA",  NAME_SIZE );
    strncpy( usp->UnICn[5], "qLapl-m_t",  NAME_SIZE );
    strncpy( usp->UnICn[6], "qHurv-m_t",  NAME_SIZE );
    strncpy( usp->UnICn[7], "qWald-m_t",  NAME_SIZE );
    strncpy( usp->UnICn[8], "qHome-m_t",  NAME_SIZE );
    strncpy( usp->UnICn[9], "reserved",  NAME_SIZE );

    strncpy( usp->UgDCn[0], "Det.(UIP)",  NAME_SIZE );
    strncpy( usp->UgDCn[1], "Filt<UIP>",  NAME_SIZE );
    strncpy( usp->UgDCn[2], "2SigFilt",  NAME_SIZE );
    strncpy( usp->UgDCn[3], "DomPA-UIP",  NAME_SIZE );
    strncpy( usp->UgDCn[4], "2SigDomPA",  NAME_SIZE );
    strncpy( usp->UgDCn[5], "qLapl-UIP",  NAME_SIZE );
    strncpy( usp->UgDCn[6], "qHurv-UIP",  NAME_SIZE );
    strncpy( usp->UgDCn[7], "qWald-UIP",  NAME_SIZE );
    strncpy( usp->UgDCn[8], "qHome-UIP",  NAME_SIZE );
    strncpy( usp->UgDCn[9], "reserved",  NAME_SIZE );

    strncpy( usp->UaDCn[0], "Det.(act)",  NAME_SIZE );
    strncpy( usp->UaDCn[1], "Filt<act>",  NAME_SIZE );
    strncpy( usp->UaDCn[2], "2SigFilt",  NAME_SIZE );
    strncpy( usp->UaDCn[3], "DomPA-act",  NAME_SIZE );
    strncpy( usp->UaDCn[4], "2SigDomPA",  NAME_SIZE );
    strncpy( usp->UaDCn[5], "qLapl-act",  NAME_SIZE );
    strncpy( usp->UaDCn[6], "qHurv-act",  NAME_SIZE );
    strncpy( usp->UaDCn[7], "qWald-act",  NAME_SIZE );
    strncpy( usp->UaDCn[8], "qHome-act",  NAME_SIZE );
    strncpy( usp->UaDCn[9], "reserved",  NAME_SIZE );

    strncpy( usp->UnDCAn[0], "Adap(UIP)",  NAME_SIZE );
    strncpy( usp->UnDCAn[1], "HintAdap",  NAME_SIZE );
    strncpy( usp->UnDCAn[2], "min(UIP)",  NAME_SIZE );
    strncpy( usp->UnDCAn[3], "max(UIP)",  NAME_SIZE );
    strncpy( usp->UnDCAn[4], "AdapF-UIP",  NAME_SIZE );
    strncpy( usp->UnDCAn[5], "2SigAdapF",  NAME_SIZE );
    strncpy( usp->UnDCAn[6], "reserved",  NAME_SIZE );
    strncpy( usp->UnDCAn[7], "reserved",  NAME_SIZE );

  if( mode  ) //default data
  {
    int ii;

    if(usp->PsGen[0] == S_ON  && usp->Pa_f_mol == S_ON)
    {
      for(ii=0; ii<usp->N; ii++ )
        if( usp->PsUnFltI == UNSP_UN_LOG )
        {   usp->m_t_lo[ii] = -20;
            usp->m_t_up[ii] = 1;
        }else
         {   usp->m_t_lo[ii] = 0;
             usp->m_t_up[ii] = 10;
         }
   }
   if( usp->PsGen[0] == S_ON  && usp->Ls && usp->Pa_f_fug== S_ON )
   {
     for(ii=0; ii<usp->Ls; ii++ )
       if( usp->PsUnFltD == UNSP_UN_LOG )
        {   usp->fug_lo[ii] = -80;
            usp->fug_up[ii] = 3;
        }else
         {   usp->fug_lo[ii] = 0;
             usp->fug_up[ii] = 1000;
         }
    }
/*    strncpy( usp->UnICn[0], "Det.(m_t)",  NAME_SIZE );
    strncpy( usp->UnICn[1], "Filt<m_t>",  NAME_SIZE );
    strncpy( usp->UnICn[2], "2SigFilt",  NAME_SIZE );
    strncpy( usp->UnICn[3], "DomPA<m_t>",  NAME_SIZE );
    strncpy( usp->UnICn[4], "2SigDomPA",  NAME_SIZE );
    strncpy( usp->UnICn[5], "qLapl<m_t>",  NAME_SIZE );
    strncpy( usp->UnICn[6], "qHurv<m_t>",  NAME_SIZE );
    strncpy( usp->UnICn[7], "qWald<m_t>",  NAME_SIZE );
    strncpy( usp->UnICn[8], "qHome<m_t>",  NAME_SIZE );
    strncpy( usp->UnICn[9], "reserved",  NAME_SIZE );

    strncpy( usp->UgDCn[0], "Det.(UIP)",  NAME_SIZE );
    strncpy( usp->UgDCn[1], "Filt<UIP>",  NAME_SIZE );
    strncpy( usp->UgDCn[2], "2SigFilt",  NAME_SIZE );
    strncpy( usp->UgDCn[3], "DomPA<UIP>",  NAME_SIZE );
    strncpy( usp->UgDCn[4], "2SigDomPA",  NAME_SIZE );
    strncpy( usp->UgDCn[5], "qLapl<UIP>",  NAME_SIZE );
    strncpy( usp->UgDCn[6], "qHurv<UIP>",  NAME_SIZE );
    strncpy( usp->UgDCn[7], "qWald<UIP>",  NAME_SIZE );
    strncpy( usp->UgDCn[8], "qHome<UIP>",  NAME_SIZE );
    strncpy( usp->UgDCn[9], "reserved",  NAME_SIZE );

    strncpy( usp->UaDCn[0], "Det.(con)",  NAME_SIZE );
    strncpy( usp->UaDCn[1], "Filt<con>",  NAME_SIZE );
    strncpy( usp->UaDCn[2], "2SigFilt",  NAME_SIZE );
    strncpy( usp->UaDCn[3], "DomPA<con>",  NAME_SIZE );
    strncpy( usp->UaDCn[4], "2SigDomPA",  NAME_SIZE );
    strncpy( usp->UaDCn[5], "qLapl<con>",  NAME_SIZE );
    strncpy( usp->UaDCn[6], "qHurv<con>",  NAME_SIZE );
    strncpy( usp->UaDCn[7], "qWald<con>",  NAME_SIZE );
    strncpy( usp->UaDCn[8], "qHome<con>",  NAME_SIZE );
    strncpy( usp->UaDCn[9], "reserved",  NAME_SIZE );

    strncpy( usp->UnDCAn[0], "Adap(UIP)",  NAME_SIZE );
    strncpy( usp->UnDCAn[1], "HintAdap",  NAME_SIZE );
    strncpy( usp->UnDCAn[2], "min(UIP)",  NAME_SIZE );
    strncpy( usp->UnDCAn[3], "max(UIP)",  NAME_SIZE );
    strncpy( usp->UnDCAn[4], "AdapF<UIP>",  NAME_SIZE );
    strncpy( usp->UnDCAn[5], "2SigAdapF",  NAME_SIZE );
    strncpy( usp->UnDCAn[6], "reserved",  NAME_SIZE );
    strncpy( usp->UnDCAn[7], "reserved",  NAME_SIZE );  */
  }
}

//realloc & setup data before generation part
void TUnSpace::init_generation( )
{
 int i,j;
 int JJ=0;

 usp->nG = 0;
 usp->nGB = 0;
 usp->nGN = 0;
 usp->nGR = 0;
 usp->nPG = 0;

 if(usp->PsGen[0]== S_ON)
      JJ++;
 if(usp->PsGen[1]== S_ON)
       JJ++;
 if(usp->PsGen[5]== S_ON)
       JJ++;
 for( i=0; i< usp->N+(usp->L*JJ)+2;i++)
  {
     if(usp->PsGen[0]== S_ON||usp->PsGen[1]== S_ON||usp->PsGen[5]== S_ON)
     {
       for( j=0;j< usp->L; j++)
       if(  ((usp->PsGen[0]== S_ON) && ( i+1 == usp->NgLg[j] )) ||
            ((usp->PsGen[1]== S_ON) && ( i+1 == usp->NgLs[j] )) ||
            ((usp->PsGen[5]== S_ON) && ( i+1 == usp->NgLv[j] )) )
        { usp->nG++;
          break;
        }
     if(j < usp->L)
       continue;
    }
    if( usp->PsGen[2]== S_ON )
    { for( j=0; j<usp->N; j++)
       if( i+1 == usp->NgNb[j] )
         { usp->nG++;
           break;
         }
      if(j<usp->N)
        continue;
    }
    if( usp->PsGen[3]== S_ON && i+1 == usp->NgT )
       { usp->nG++; continue; }
    if( usp->PsGen[4]== S_ON && i+1 == usp->NgP)
       { usp->nG++; continue; }
  }
  // start nG must be more than new nG and array PbD must be inserted after
  // remake ?????????? /
  // calculate kGB, kGR, kGN
  for( i=0; i< usp->nG; i++)
  {  if( !usp->PbD[i])
           usp->nGB++;
     if( usp->PbD[i]==1 )
           usp->nGR++;
     if(  usp->PbD[i]==2 )
           usp->nGN++;
  }

  usp->nPG = calc_nPG();

  // realloc internal arrays
  nG_dyn_new();

  build_nPG_list();
  BELOV( usp->Q, usp->nGB, usp->OVB);
  UNIFORM0( 0 );
  UNIFORM0( 1 );

  if( syu->Guns )
          fillValue( syu->Guns, 0., TRMults::sm->GetMU()->L );
  if( syu->Vuns )
          fillValue( syu->Vuns, 0., TRMults::sm->GetMU()->L );

// copy data from second column

  if( usp->PsGen[0]== S_ON )
    for( j=0; j<usp->L; j++)
    {    usp->IntLg[j][0] = usp->IntLg[j][1];
         usp->Gs[j][0] = usp->Gs[j][1];
    }

  if( usp->PsGen[1]== S_ON )
    for( j=0; j<usp->L; j++)
    {    usp->IntLs[j][0] = usp->IntLs[j][1];
         usp->Ss[j][0] = usp->Ss[j][1];
    }

  if( usp->PsGen[5]== S_ON )
    for( j=0; j<usp->L; j++)
    {    usp->IntLv[j][0] = usp->IntLv[j][1];
         usp->Vs[j][0] = usp->Vs[j][1];
    }

  if( usp->PsGen[2]== S_ON )
    for( j=0; j<usp->N; j++)
    {    usp->IntNb[j][0] = usp->IntNb[j][1];
         usp->Bs[j][0] = usp->Bs[j][1];
    }

  if(usp->PsGen[6]== S_ON && usp->Ls )   // new by DK
    for( j=0; j<usp->Ls; j++)
    {    usp->IntGam[j][0] = usp->IntGam[j][1];
         usp->GAMs[j][0] = usp->GAMs[j][1];
    }

  usp->T[0] = usp->T[1];
  usp->P[0] = usp->P[1];
  usp->V[0] = usp->V[1];
  usp->Tc = usp->T[0];
  usp->Pc = usp->P[0];
  usp->Vc = usp->V[0];

}


//calculation number of unspace components
int TUnSpace::calc_nPG()
{
  int j, count=0;

  if( usp->PsGen[3]== S_ON &&  usp->NgT > 0 )
       count++;
  if( usp->PsGen[4]== S_ON &&  usp->NgP > 0 )
       count++;
  if( /*usp->PsGen[]== S_ON && */ usp->NgV > 0 )
       count++;

  if( usp->PsGen[2]== S_ON )
     for( j=0; j<usp->N; j++)
       if( usp->NgNb[j] > 0  )
           count++;

  if( usp->PsGen[0]== S_ON )
     for( j=0; j<usp->L; j++)
       if( usp->NgLg[j] > 0  )
           count++;

  if( usp->PsGen[1]== S_ON )
     for( j=0; j<usp->L; j++)
       if( usp->NgLs[j] > 0  )
           count++;

  if( usp->PsGen[5]== S_ON )
     for( j=0; j<usp->L; j++)
       if( usp->NgLv[j] > 0  )
           count++;

  if( usp->PsGen[6]== S_ON )
     for( j=0; j<usp->Ls; j++)
       if( usp->NgGam[j] > 0  )
           count++;
  return count;
}

//build list of unspace components
void TUnSpace::build_nPG_list()
{
  char tbuf[100];
  int j, count=0;

  if( !usp->ParNames )
    return;

  if( usp->PsGen[3]== S_ON &&  usp->NgT > 0 )
  {
     sprintf( tbuf, "%02d T ", usp->NgT );
     strncpy( usp->ParNames[count], tbuf, 5);
     count++;
  }
  if( usp->PsGen[4]== S_ON &&  usp->NgP > 0 )
  {
     sprintf( tbuf, "%02d P ", usp->NgP );
     strncpy( usp->ParNames[count], tbuf, 5);
     count++;
  }
  if( /*usp->PsGen[]== S_ON && */ usp->NgV > 0 )
  {
     sprintf( tbuf, "%02d V ", usp->NgV );
     strncpy( usp->ParNames[count], tbuf, 5);
     count++;
  }

  if( usp->PsGen[2]== S_ON )
     for( j=0; j<usp->N; j++)
       if( usp->NgNb[j] > 0  )
       {
          sprintf( tbuf, "%02d B ",  usp->NgNb[j] );
          strncpy( usp->ParNames[count], tbuf, 5);
          strncpy( usp->ParNames[count]+5,
             TRMults::sm->GetMU()->SB[j], MAXICNAME+MAXSYMB); //10
          count++;
       }

  if( usp->PsGen[0]== S_ON )
     for( j=0; j<usp->L; j++)
       if( usp->NgLg[j] > 0  )
       {
          sprintf( tbuf, "%02dG0 ",  usp->NgLg[j] );
          strncpy( usp->ParNames[count], tbuf, 5);
          strncpy( usp->ParNames[count]+5,
            TRMults::sm->GetMU()->SM[j]+MAXSYMB+MAXDRGROUP, PARNAME_SIZE-5);
          count++;
       }

  if( usp->PsGen[1]== S_ON )
     for( j=0; j<usp->L; j++)
       if( usp->NgLs[j] > 0  )
       {
          sprintf( tbuf, "%02dS0 ",  usp->NgLs[j] );
          strncpy( usp->ParNames[count], tbuf, 5);
          strncpy( usp->ParNames[count]+5,
             TRMults::sm->GetMU()->SM[j]+MAXSYMB+MAXDRGROUP, PARNAME_SIZE-5);
          count++;
       }

  if( usp->PsGen[5]== S_ON )
     for( j=0; j<usp->L; j++)
       if( usp->NgLv[j] > 0  )
       {
          sprintf( tbuf, "%02dV0 ",  usp->NgLv[j] );
          strncpy( usp->ParNames[count], tbuf, 5);
          strncpy( usp->ParNames[count]+5,
             TRMults::sm->GetMU()->SM[j]+MAXSYMB+MAXDRGROUP, PARNAME_SIZE-5);
          count++;
       }

  if( usp->PsGen[6]== S_ON )
     for( j=0; j<usp->Ls; j++)
       if( usp->NgGam[j] > 0  )
       {
          sprintf( tbuf, "%02dGa ",  usp->NgGam[j] );
          strncpy( usp->ParNames[count], tbuf, 5);
          strncpy( usp->ParNames[count]+5,
             TRMults::sm->GetMU()->SM[j]+MAXSYMB+MAXDRGROUP, PARNAME_SIZE-5);
          count++;
       }
}

//realloc & setup data before analyse part
void TUnSpace::init_analyse( )
{

    // realloc internal arrays
    work_dyn_new();

    // Get full matrix A
    // load formulae
    std::vector<TFormula> aFo;
    std::string form;
    int ii;

    for( ii=0; ii<TRMults::sm->GetMU()->L; ii++ )
    {
        aFo.push_back( TFormula() );
        form = aFo[ii].form_extr( ii, TRMults::sm->GetMU()->L,
                                  TRMults::sm->GetMU()->DCF );
        aFo[ii].SetFormula( form.c_str() ); // and ce_fscan
    }

    fillValue(usp->A, 0., (TRMults::sm->GetMU()->N*TRMults::sm->GetMU()->L) );
    for( ii=0; ii<TRMults::sm->GetMU()->L; ii++ )
        aFo[ii].Stm_line(TRMults::sm->GetMU()->N, usp->A+ii*TRMults::sm->GetMU()->N,
                         (char *)TRMults::sm->GetMU()->SB, TRMults::sm->GetMU()->Val );
    aFo.clear();
}


//=================================================================
// Graphics and math scripts

// Translate, analyze and unpack equations of TUnSpace
void TUnSpace::text_analyze( int nObj)
{
    try
    {
        TProfil* PRof = TProfil::pm;
        int mupL=0, pmpL =0;

        if( pVisor->ProfileMode )
        {
            mupL = TRMults::sm->GetMU()->L;
            pmpL = TMulti::sm->GetPM()->L;
        }

        switch(nObj)
        {
        case o_ungexpr:
            PRof->ET_translate( o_untprn, o_ungexpr, 0, mupL, 0, pmpL );
            rpn[1].GetEquat( static_cast<char *>(aObj[o_untprn]->GetPtr()) );
            break;
        case o_unexpr:
            PRof->ET_translate( o_untprn, o_unexpr, 0, mupL, 0, pmpL );
            rpn[0].GetEquat( static_cast<char *>(aObj[o_untprn]->GetPtr()) );
            break;
        }

    }
    catch( TError& xcpt )
    {
        char *erscan = static_cast<char *>(aObj[nObj]->GetPtr());
        vfMessage(window(), xcpt.title, xcpt.mess);
        /*bool   iRet = */
        CheckEqText(  erscan,
                      "E91MSTran: Error in translation of TUnSpace math script: " );
        /*  if( iRet )
               goto AGAIN;  */
        Error(  GetName() , xcpt.mess );
    }
}

// calculate equations of graphic data
void TUnSpace::calc_graph()
{
    //     if( usp->PsGraph == S_OFF )
    //      return;

    // calc equations
    for( usp->q = 0; usp->q < usp->dimXY[0]/*usp->Q*/; usp->q++ )
    {
        pVisor->Message( window(), GetName(),
                         "Calculation graphic\n"
                         "Please, wait...", usp->q, usp->Q);
        rpn[1].CalcEquat();
    }
}


void
TUnSpace::RecordPlot( const char* /*key*/ )
{
    if( usp->PsGraph == S_OFF )
        return;

    std::vector<TPlot> plt;

    plt.push_back( TPlot(o_unxa, o_unyc ));
    int  nLn = plt[ 0 ].getLinesNumber();
    plt.push_back( TPlot(o_unxs, o_unys ));
    nLn += plt[1].getLinesNumber();
    if( plot )
    {
        int oldN = aObj[o_unplline]->GetN();
        TPlotLine defpl("", 3, 6, 0);

        plot = static_cast<TPlotLine * >(aObj[ o_unplline ]->Alloc( nLn, sizeof(TPlotLine) ));
        for(int ii=0; ii<nLn; ii++ )
        {
            if( ii >= oldN )
                plot[ii] = defpl;
            plot[ii].setName( usp->lNam[ii]);
            //strncpy( plot[ii].name, usp->lNam[ii], MAXGRNAME-1 );
            //plot[ii].name[MAXGRNAME-1] = '\0';
        }
        gd_gr = updateGraphWindow( gd_gr, this, plt, usp->name,
                                   usp->size[0], usp->size[1], plot,
                usp->axisType, usp->xNames, usp->yNames );
    }
    else
    {
        std::vector<TPlotLine> def_plt_lines;
        def_plt_lines.push_back(TPlotLine( "",3,6,0));
        //def_plt_lines.push_back(TPlotLine( "",7,20,0));
        TCStringArray lnames;
        for(int  ii=0; ii<usp->dimXY[1]+usp->dimEF[1]; ii++ )
            lnames.push_back( std::string(usp->lNam[ii], 0, MAXGRNAME ));
        gd_gr = updateGraphWindow( gd_gr, this, plt, usp->name,
                                   usp->xNames, usp->yNames, lnames, def_plt_lines, ISOLINES );
    }
}

bool TUnSpace::SaveChartData( jsonui::ChartData* gr )
{

    // We can only have one Plot dialog (modal one) so condition should be omitted!!
    if( !gd_gr )
        return false;

    strncpy(  usp->name, gr->title.c_str(), MAXFORMULA );
    usp->axisType[0] = static_cast<short>(gr->axisTypeX);
    usp->axisType[5] = static_cast<short>(gr->axisTypeY);
    usp->axisType[4] = static_cast<short>(gr->getGraphType());
    usp->axisType[1] = static_cast<short>(gr->b_color[0]);
    usp->axisType[2] = static_cast<short>(gr->b_color[1]);
    usp->axisType[3] = static_cast<short>(gr->b_color[2]);
    strncpy( usp->xNames, gr->xName.c_str(), MAXAXISNAME);
    strncpy( usp->yNames, gr->yName.c_str(), MAXAXISNAME);
    for(int ii=0; ii<4; ii++ )
    {
        usp->size[0][ii] =  static_cast<float>(gr->region[ii]);
        usp->size[1][ii] =  static_cast<float>(gr->part[ii]);
    }

    plot = static_cast<TPlotLine *>(aObj[ o_unplline]->Alloc( gr->getSeriesNumber(), sizeof(TPlotLine)));
    for(int ii=0; ii<gr->getSeriesNumber(); ii++ )
    {
        plot[ii] = convertor( gr->lineData( ii ) );
        strncpy(  usp->lNam[ii], plot[ii].getName().c_str(), MAXGRNAME );
    }

    //if( gr->graphType == ISOLINES )
    //   gr->getColorList();

    pVisor->Update();
    contentsChanged = true;

    return true;
}

//=====================================================

// insert changes in Project to TUnSpace
void TUnSpace::InsertChanges( std::vector<CompItem>& aIComp,
    std::vector<CompItem>& aPhase,  std::vector<CompItem>&aDComp )
{
// make copy of UNSPACE  structure (only for changed arrays )
   ods_link(-2);  //1
   memcpy( &us[1].N, &us[0].N, 32 );
   memcpy( &us[1].PunE, &us[0].PunE, 38 );
   dyn_new(1);

//********************************************************
   // copy arrays
  if(usp->PsGen[0] == S_ON )
  {
    memcpy( us[1].Gs, us[0].Gs, us[1].L*2*sizeof(float) );
    memcpy( us[1].NgLg, us[0].NgLg, us[1].L*sizeof(short) );
    memcpy( us[1].IntLg, us[0].IntLg, us[1].L*2*sizeof(float) );

    memcpy( us[1].vG, us[0].vG, us[1].Q*us[1].L*sizeof(double) );
    memcpy( us[1].vY, us[0].vY, us[1].Q*us[1].L*sizeof(double) );
    memcpy( us[1].vYF, us[0].vYF, us[1].Q*us[1].Fi*sizeof(double) );
    memcpy( us[1].vGam, us[0].vGam, us[1].Q*us[1].L*sizeof(double) );
    memcpy( us[1].vMol, us[0].vMol, us[1].Q*us[1].N*sizeof(double) );
    memcpy( us[1].vU, us[0].vU,     us[1].Q*us[1].N*sizeof(double) );
  }

  if(usp->PsGen[0] == S_ON  && usp->Pa_f_mol == S_ON)
  {
    memcpy( us[1].m_t_lo, us[0].m_t_lo, us[1].N*sizeof(float) );
    memcpy( us[1].m_t_up, us[0].m_t_up, us[1].N*sizeof(float) );
  }

  if( usp->PsGen[0] == S_ON  && usp->Ls )
    memcpy( us[1].vFug, us[0].vFug, us[1].Q*us[1].Ls*sizeof(double) );

  if( usp->PsGen[0] == S_ON  && usp->Ls && usp->Pa_f_fug== S_ON )
  {
    memcpy( us[1].fug_lo, us[0].fug_lo, us[1].Ls*sizeof(float) );
    memcpy( us[1].fug_up, us[0].fug_up, us[1].Ls*sizeof(float) );
  }

  if(usp->PsGen[1]== S_ON)
  {
    memcpy( us[1].Ss, us[0].Ss, us[1].L*2*sizeof(float) );
    memcpy( us[1].NgLs, us[0].NgLs, us[1].L*sizeof(short) );
    memcpy( us[1].IntLs, us[0].IntLs, us[1].L*2*sizeof(float) );
    memcpy( us[1].vS, us[0].vS, us[1].L*us[1].Q*sizeof(double) );
  }

  if(usp->PsGen[5]== S_ON)
  {
    memcpy( us[1].Vs, us[0].Vs, us[1].L*2*sizeof(float) );
    memcpy( us[1].NgLv, us[0].NgLv, us[1].L*sizeof(short) );
    memcpy( us[1].IntLv, us[0].IntLv, us[1].L*2*sizeof(float) );
    memcpy( us[1].vmV, us[0].vmV, us[1].L*us[1].Q*sizeof(double) );
  }

  if(usp->PsGen[2]== S_ON)
  {
    memcpy( us[1].NgNb, us[0].NgNb, us[1].N*sizeof(short) );
    memcpy( us[1].IntNb, us[0].IntNb, us[1].N*2*sizeof(float) );
    memcpy( us[1].Bs, us[0].Bs, us[1].N*2*sizeof(double) );
    memcpy( us[1].vB, us[0].vB, us[1].N*us[1].Q*sizeof(double) );
  }

  if(usp->PsGen[6]== S_ON)   // new by DK
  {
    memcpy( us[1].NgGam, us[0].NgGam, us[1].Ls*sizeof(short) );
    memcpy( us[1].IntGam, us[0].IntGam, us[1].Ls*2*sizeof(float) );
    memcpy( us[1].GAMs, us[0].GAMs, us[1].Ls*2*sizeof(float) );
    memcpy( us[1].vNidP, us[0].vNidP, us[1].Ls*us[1].Q*sizeof(double) );
  }

//  not for icomps, free array
//  if(  usp->Pa_f_pha == S_ON)
//    memcpy( us[1].f_PhA, us[0].f_PhA, us[1].N*sizeof(short) );

//*********************************************************/
//  resize us[0]

   ods_link(-1);
   usp->N = TRMults::sm->GetMU()->N;
   usp->L = TRMults::sm->GetMU()->L;
   usp->Fi = TRMults::sm->GetMU()->Fi;
   usp->Ls = TRMults::sm->GetMU()->Ls;
   dyn_new(0);

   // recorb must be remaked and recalculated
   usp->Gstat = GS_INDEF;
   usp->Astat = AS_INDEF;

// pack and copy data from 1 to 0 (using deleting lists)
//***************************************************

    uint i=0;
    int j, ii=0, jj =0;

if( aIComp.size() < 1)
  goto DCOMPS;

// IComps  ( size  N )
    while( jj < usp->N )
    {
      if( i < aIComp.size() &&  aIComp[i].line == ii )
      {
        if( aIComp[i].delta == 1 )
        { // add line
          if(usp->PsGen[0] == S_ON )
              for( j =0; j<usp->Q; j++ )
              {
                usp->vMol[j*usp->N+jj] = 0.;
                usp->vU[j*usp->N+jj] = 0.;
              }
          if(usp->PsGen[0] == S_ON  && usp->Pa_f_mol == S_ON)
          {
             usp->m_t_lo[jj] = 0.;
             usp->m_t_up[jj] = 0.;
          }
          if(usp->PsGen[2]== S_ON)
          {
             for( j =0; j<usp->Q; j++ )
                usp->vB[j*usp->N+jj] = 0.;
             usp->NgNb[jj] = 0;
             usp->IntNb[jj][0] = 0.;
             usp->IntNb[jj][1] = 0.;
             usp->Bs[jj][0] = 0.;
             usp->Bs[jj][1] = 0.;
          }
          jj++;
         }
          else
           { // delete line
             ii++;
           }
        i++;
       }
       else
       {  // copy line
         if( ii < us[1].N )
         {
          if(usp->PsGen[0] == S_ON )
              for( j =0; j<usp->Q; j++ )
              {
                usp->vMol[j*usp->N+jj] = us[1].vMol[j*us[1].N+ii];
                usp->vU[j*usp->N+jj] = us[1].vU[j*us[1].N+ii];
              }
          if(usp->PsGen[0] == S_ON  && usp->Pa_f_mol == S_ON)
          {
             usp->m_t_lo[jj] = us[1].m_t_lo[ii];
             usp->m_t_up[jj] = us[1].m_t_up[ii];
          }
          if(usp->PsGen[2]== S_ON)
          {
             usp->NgNb[jj] = us[1].NgNb[ii];
             usp->IntNb[jj][0] = us[1].IntNb[ii][0];
             usp->IntNb[jj][1] = us[1].IntNb[ii][1];
             usp->Bs[jj][0] = us[1].Bs[ii][0];
             usp->Bs[jj][1] = us[1].Bs[ii][1];
             for( j =0; j<usp->Q; j++ )
                usp->vB[j*usp->N+jj] = us[1].vB[j*us[1].N+ii];
          }
          }
        jj++;
        ii++;
       }
    }

DCOMPS:
if( aDComp.size() < 1)
  goto PHASES;

// DComps  ( size  L )
    ii=0; jj =0; i=0;

    while( jj < usp->L )
    {
      if( i < aDComp.size() &&  aDComp[i].line == ii )
      {
        if( aDComp[i].delta == 1 )
        { // add line
          if(usp->PsGen[0] == S_ON )
          {   for( j =0; j<usp->Q; j++ )
              {
                usp->vG[j*usp->L+jj] = 0.;
                usp->vY[j*usp->L+jj] = 0.;
                usp->vGam[j*usp->L+jj] = 0.;
              }
                usp->Gs[jj][0] = 0.;
                usp->Gs[jj][1] = 0.;
                usp->NgLg[jj] = 0;
                usp->IntLg[jj][0] = 0.;
                usp->IntLg[jj][1] = 0.;
            }
            if(usp->PsGen[1]== S_ON)
            {
                for( j =0; j<usp->Q; j++ )
                     usp->vS[j*usp->L+jj] = 0.;
                usp->Ss[jj][0] = 0.;
                usp->Ss[jj][1] = 0.;
                usp->NgLs[jj] = 0;
                usp->IntLs[jj][0] = 0.;
                usp->IntLs[jj][1] = 0.;
            }
            if(usp->PsGen[5]== S_ON)
            {
                for( j =0; j<usp->Q; j++ )
                     usp->vmV[j*usp->L+jj] = 0.;
                usp->Vs[jj][0] = 0.;
                usp->Vs[jj][1] = 0.;
                usp->NgLv[jj] = 0;
                usp->IntLv[jj][0] = 0.;
                usp->IntLv[jj][1] = 0.;
            }
          jj++;
         }
          else
           { // delete line
             ii++;
           }
        i++;
       }
       else
       {  // copy line
         if( ii < us[1].L )
         {
          if(usp->PsGen[0] == S_ON )
          {   for( j =0; j<usp->Q; j++ )
              {
                usp->vG[j*usp->L+jj] = us[1].vG[j*us[1].L+ii];
                usp->vY[j*usp->L+jj] = us[1].vY[j*us[1].L+ii];
                usp->vGam[j*usp->L+jj] = us[1].vGam[j*us[1].L+ii];
              }
                usp->Gs[jj][0] = us[1].Gs[ii][0];
                usp->Gs[jj][1] = us[1].Gs[ii][1];
                usp->NgLg[jj] = us[1].NgLg[ii];
                usp->IntLg[jj][0] = us[1].IntLg[ii][0];
                usp->IntLg[jj][1] = us[1].IntLg[ii][1];
            }
            if(usp->PsGen[1]== S_ON)
            {
                for( j =0; j<usp->Q; j++ )
                    usp->vS[j*usp->L+jj] = us[1].vS[j*us[1].L+ii];
                usp->Ss[jj][0] = us[1].Ss[ii][0];
                usp->Ss[jj][1] = us[1].Ss[ii][1];
                usp->NgLs[jj] = us[1].NgLs[ii];
                usp->IntLs[jj][0] = us[1].IntLs[ii][0];
                usp->IntLs[jj][1] = us[1].IntLs[ii][1];
            }
            if(usp->PsGen[5]== S_ON)
            {
                for( j =0; j<usp->Q; j++ )
                    usp->vmV[j*usp->L+jj] = us[1].vmV[j*us[1].L+ii];
                usp->Vs[jj][0] = us[1].Vs[ii][0];
                usp->Vs[jj][1] = us[1].Vs[ii][1];
                usp->NgLv[jj] = us[1].NgLv[ii];
                usp->IntLv[jj][0] = us[1].IntLv[ii][0];
                usp->IntLv[jj][1] = us[1].IntLv[ii][1];
            }
          }
        jj++;
        ii++;
       }
    }

 // DComps  ( size  Ls )
    ii=0; jj =0;  i=0;

    while( jj < usp->Ls )
    {
      if( i < aDComp.size() &&  aDComp[i].line == ii )
      {
        if( aDComp[i].delta == 1 )
        { // add line
           if( usp->PsGen[0] == S_ON  && usp->Ls )
                usp->vFug[jj] = 0.;
           if( usp->PsGen[0] == S_ON  && usp->Ls && usp->Pa_f_fug== S_ON )
            {
                usp->fug_lo[jj] = 0.;
                usp->fug_up[jj] = 0.;
             }
           if(usp->PsGen[6]== S_ON)   // new by DK
            {
                usp->NgGam[jj] = 0;
                usp->IntGam[jj][0] = 0.;
                usp->IntGam[jj][1] = 0.;
                usp->GAMs[jj][0] = 0.;
                usp->GAMs[jj][1] = 0.;
                for( j =0; j<usp->Q; j++ )
                    usp->vNidP[j*usp->Ls+jj] = 0.;

             }
          jj++;
         }
          else
           { // delete line
             ii++;
           }
        i++;
       }
       else
       {  // copy line
         if( ii < us[1].Ls )
         {
           if( usp->PsGen[0] == S_ON  && usp->Ls )
                usp->vFug[jj] = us[1].vFug[ii];
           if( usp->PsGen[0] == S_ON  && usp->Ls && usp->Pa_f_fug== S_ON )
            {
                usp->fug_lo[jj] = us[1].fug_lo[ii];
                usp->fug_up[jj] = us[1].fug_up[ii];
             }
           if(usp->PsGen[6]== S_ON)   // new by DK
            {
                usp->NgGam[jj] = us[1].NgGam[ii];
                usp->IntGam[jj][0] = us[1].IntGam[ii][0];
                usp->IntGam[jj][1] = us[1].IntGam[ii][1];
                usp->GAMs[jj][0] = us[1].GAMs[ii][0];
                usp->GAMs[jj][1] = us[1].GAMs[ii][1];
                for( j =0; j<usp->Q; j++ )
                    usp->vNidP[j*usp->Ls+jj] = us[1].vNidP[j*us[1].Ls+ii];

             }
          }
        jj++;
        ii++;
       }
    }


//*************************************************************
PHASES:

if( aPhase.size() < 1)
  return;

    ii=0; jj =0; i=0;
// PHases  ( size  Fi )
    while( jj < usp->Fi )
    {
      if( i < aPhase.size() &&  aPhase[i].line == ii )
      {
        if( aPhase[i].delta == 1 )
        { // add line
          if(usp->PsGen[0] == S_ON )
              for( j =0; j<usp->Q; j++ )
                usp->vYF[j*usp->Fi+jj] = 0.;
          jj++;
         }
          else
           { // delete line
             ii++;
           }
        i++;
       }
       else
       {  // copy line
         if( ii < us[1].Fi )
         {
          if(usp->PsGen[0] == S_ON )
              for( j =0; j<usp->Q; j++ )
                usp->vYF[j*usp->Fi+jj] = us[1].vYF[j*us[1].Fi+ii];
          }
        jj++;
        ii++;
       }
    }
}   // end of   InsertChanges

//--------------------- End of m_unsp_gen.cpp ---------------------------
