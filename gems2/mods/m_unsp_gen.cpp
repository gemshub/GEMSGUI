#include <math.h>
#include <stdio.h>

#include "m_unspace.h"
#include "s_formula.h"
#include "m_syseq.h"
#include "service.h"
#include "visor.h"


//=========================================================================
// Generation part
//=========================================================================

short Prime[]=
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
  int i;
  for( i=0; i<sizeof(Prime)/sizeof(short)-1; i++)
   if( Prime[i] >= num )
           break;

  return Prime[i];
}

//set sizes of arrays
bool TUnSpace::test_sizes( )
{
    bool i=true;


   // the same sizes
   usp->N = TProfil::pm->mup->N;
   usp->L = TProfil::pm->mup->L;
   usp->Ls = TProfil::pm->mup->Ls;
   usp->Fi = TProfil::pm->mup->Fi;
   usp->Fis = TProfil::pm->mup->Fis;

   if( usp->Q<=0 )
   {
      i = false;
      usp->Q = 11;
   }
   usp->Q = near_prime_number( usp->Q );
   usp->qQ = (int)(fabs(usp->quan_lev)*usp->Q);
   if(usp->qQ<1)
        usp->qQ=1;

   if( usp->nG<=0 )
   {
      i = false;
      usp->nG = 2;
      usp->nGB =2;
   }

   if( usp->nGB+usp->nGN+usp->nGR != usp->nG )  // ???? must be changed
   {
      i = false;
      usp->nGB = usp->nG;
      usp->nGN = usp->nGR = 0;
   }
   usp->t = 0;
   usp->q = 0;
   usp->nPhA = 0;                                   // ????? nPhA test

   usp->Tc = usp->T;
   usp->Pc = usp->P;
   usp->Vc = usp->V;

   if( i==false )
        vfMessage(window(), GetName(),
   "E00UErem: Invalid dimensions in UnSpace data structure!", vfErr);

    return i;

}

//set default numbers to arrays ( PbD, NgLg .... )
void TUnSpace::set_def_data_to_arrays( bool mode )
{
  // must be done

    for(int i=0; i<usp->L; i++ )
    {
       if(usp->PsGen[0] == S_ON )
         usp->Gs[i] = TProfil::pm->tpp->G[i]+TProfil::pm->syp->GEX[i];

       if(usp->PsGen[5] == S_ON )
         usp->Vs[i] = TProfil::pm->tpp->Vm[i];

       if(usp->PsGen[1] == S_ON && TProfil::pm->tpp->S )
         usp->Ss[i] = TProfil::pm->tpp->S[i];
     }

    if(usp->PsGen[2] == S_ON )
      for(int i=0; i<usp->N; i++ )
         usp->Bs[i] = TProfil::pm->syp->B[i];

  if( mode  )
  {    //default data
    strncpy( usp->UnICn[0], "group",  NAME_SIZE );
    strncpy( usp->UnICn[1], "2_sgm",  NAME_SIZE );
    strncpy( usp->UnICn[2], "total",  NAME_SIZE );
    strncpy( usp->UnICn[3], "2_sgm",  NAME_SIZE );
    strncpy( usp->UnICn[4], "determin",  NAME_SIZE );
    strncpy( usp->UnICn[5], "Laplace",  NAME_SIZE );
    strncpy( usp->UnICn[6], "Homenuk",  NAME_SIZE );
    strncpy( usp->UnICn[7], "perc.deter.",  NAME_SIZE );
    strncpy( usp->UnICn[8], "N phas.grup.",  NAME_SIZE );
    strncpy( usp->UnICn[9], "reserved",  NAME_SIZE );

    strncpy( usp->UgDCn[0], "group",  NAME_SIZE );
    strncpy( usp->UgDCn[1], "2_sgm",  NAME_SIZE );
    strncpy( usp->UgDCn[2], "determin",  NAME_SIZE );
    strncpy( usp->UgDCn[3], "Laplace",  NAME_SIZE );
    strncpy( usp->UgDCn[4], "Homenuk",  NAME_SIZE );
    strncpy( usp->UgDCn[5], "mean",  NAME_SIZE );
    strncpy( usp->UgDCn[6], "quan_Lap",  NAME_SIZE );
    strncpy( usp->UgDCn[7], "quan_Hom",  NAME_SIZE );
    strncpy( usp->UgDCn[8], "dual",  NAME_SIZE );
    strncpy( usp->UgDCn[9], "reserved",  NAME_SIZE );

    strncpy( usp->UnDCAn[0], "G dat",  NAME_SIZE );
    strncpy( usp->UnDCAn[1], "interval",  NAME_SIZE );
    strncpy( usp->UnDCAn[2], "min",  NAME_SIZE );
    strncpy( usp->UnDCAn[3], "max",  NAME_SIZE );
    strncpy( usp->UnDCAn[4], "mean",  NAME_SIZE );
    strncpy( usp->UnDCAn[5], "int.filters",  NAME_SIZE );
    strncpy( usp->UnDCAn[6], "reserved",  NAME_SIZE );
    strncpy( usp->UnDCAn[7], "reserved",  NAME_SIZE );
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
       if(  usp->PsGen[0]== S_ON && ( i+1 == usp->NgLg[j] ) ||
            usp->PsGen[1]== S_ON && ( i+1 == usp->NgLs[j] ) ||
            usp->PsGen[5]== S_ON && ( i+1 == usp->NgLv[j] ) )
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

  // realloc internal arrays
  nG_dyn_new();

  BELOV( usp->Q, usp->nGB, usp->OVB);    
  UNIFORM0( 0 );
  UNIFORM0( 1 );

  if( TProfil::pm->syp->Guns )
    memset( TProfil::pm->syp->Guns, 0, TProfil::pm->mup->L*sizeof(float) );
  if( TProfil::pm->syp->Vuns )
    memset( TProfil::pm->syp->Vuns, 0, TProfil::pm->mup->L*sizeof(float) );

  if( usp->PsGen[0]== S_ON )
    for( j=0; j<usp->L; j++)
        usp->IntLg[j] = usp->IntLg0[j];

}

//realloc & setup data before analyse part
void TUnSpace::init_analyse( )
{

  // realloc internal arrays
  work_dyn_new();

  // Get full matrix A
  // load formulae
  TIArray<TFormula> aFo;
  gstring form;
  int ii;

  for( ii=0; ii<TProfil::pm->mup->L; ii++ )
  {
     aFo.Add( new TFormula() );
     form = aFo[ii].form_extr( ii, TProfil::pm->mup->L, TProfil::pm->mup->DCF );
     aFo[ii].SetFormula( form.c_str() ); // and ce_fscan
  }

  memset(usp->A, 0, sizeof(float)*(TProfil::pm->mup->N*TProfil::pm->mup->L) );
  for( ii=0; ii<TProfil::pm->mup->L; ii++ )
     aFo[ii].Stm_line( TProfil::pm->mup->N, usp->A+ii*TProfil::pm->mup->N,
           (char *)TProfil::pm->mup->SB, TProfil::pm->mup->Val );
  aFo.Clear();
}


// make EqStat key  && calculate records
void TUnSpace::unsp_eqkey()
{
    vstr buf(40);

    sprintf(buf, "%.4d", usp->q);
    memset(usp->timep, 0, 5 );
    strncpy(usp->timep, buf, 4 );
    gcvt( usp->Tc, 6,  usp->TCp );
    gcvt( usp->Pc, 6,  usp->Pp );
    gcvt( usp->Vc, 6,  usp->Bnamep );

   rt[RT_SYSEQ].MakeKey( RT_UNSPACE,  usp->stkey, RT_UNSPACE, 0, RT_UNSPACE,1,
        RT_UNSPACE, 2, K_IMM, usp->timep, K_IMM, usp->Bnamep,
        K_IMM, usp->Pp, K_IMM, usp->TCp, RT_UNSPACE, 7, K_END );
   rt[RT_SYSEQ].Find(usp->stkey);

// calc current SyStat
    TProfil::pm->CalcEqstat( false );
    TSysEq::pm->CmSave();           // save results to DB
    if( usp->stl )
       memcpy( usp->stl+usp->q, usp->stkey, EQ_RKLEN );
}


// building arrays for make pay off matrix
void TUnSpace::buildTestedArrays()
{
 int i, Ip;

 for( Ip=0; Ip<usp->Q; Ip++)
 {
    usp->q = Ip;

   pVisor->Message( window(), GetName(),
             "Generation of EqStat records\n"
                 "Please, wait...", usp->q, usp->Q);

 // setup uncertainty point data
     NexT( Ip );

 // calculate EqStat record (Thermodynamic&Equlibria)
     TProfil::pm->pmp->pTPD = 0;
     unsp_eqkey();

 // set zeros for tested arrays
 if(usp->PsGen[0]== S_ON  )
 {   for( i=0; i<usp->L; i++)
    {
       usp->vY[Ip*usp->L+i]  = 0.;
       usp->vGam[Ip*usp->L+i]= 0.;
       usp->vG[Ip*usp->L+i]= 0.;
    }
    for( i=0; i<usp->Fi; i++)
         usp->vYF[Ip*usp->Fi+i] = 0.;
    for( i=0; i<usp->N; i++)
    {
       usp->vU[Ip*usp->N+i] = 0.;
       usp->vMol[Ip*usp->N+i] = 0.;
    }
    if(TProfil::pm->mup->Pg)
       for( i=0; i<usp->Ls; i++)
            usp->vFug[Ip*usp->Ls+i] = 0.;

 // copy data from multy
    for( i=0; i<TProfil::pm->pmp->L; i++)
    {
      usp->vY[Ip*usp->L+(TProfil::pm->pmp->muj[i])]  =
                                         TProfil::pm->pmp->X[i];
      usp->vGam[Ip*usp->L+(TProfil::pm->pmp->muj[i])]=
                                         TProfil::pm->pmp->lnGam[i];

//      usp->vG[Ip*usp->L+(TProfil::pm->pmp->muj[i])]=
//                                     TProfil::pm->pmp->G[i];
     }

    for( i=0; i<usp->L; i++)
    {
      double xx = TProfil::pm->syp->Guns[i];
             xx += TProfil::pm->syp->GEX[i];
             xx += TProfil::pm->tpp->G[i];
      usp->vG[Ip*usp->L+i]= xx;
     }

    for( i=0; i<TProfil::pm->pmp->FI; i++)
       usp->vYF[Ip*usp->Fi+(TProfil::pm->pmp->muk[i])] =
                                           TProfil::pm->pmp->XF[i];

    for( i=0; i<TProfil::pm->pmp->N; i++)
    {
       usp->vU[Ip*usp->N+(TProfil::pm->pmp->mui[i])] = TProfil::pm->pmp->U[i];
       usp->vMol[Ip*usp->N+(TProfil::pm->pmp->mui[i])] =
                                                    TProfil::pm->pmp->IC_m[i];
    }
    usp->vpH[Ip] = TProfil::pm->pmp->pH;
    if(TProfil::pm->mup->Pg)
       for( i=0; i<TProfil::pm->pmp->Ls; i++)
          usp->vFug[Ip*usp->Ls+(TProfil::pm->pmp->muj[i])] =
                TProfil::pm->pmp->Y_la[i];

    usp->vT[Ip]= TProfil::pm->pmp->TC;
    usp->vP[Ip]= TProfil::pm->pmp->P;
  }
 }
}



//===============================================
// From Kostya  (not for changed)

/*  select initial values x0 */
void TUnSpace::UNIFORM0( int reg )
// reg: 0 - uniform, 1 - normal
// usp->OVR - ravnomernye sluchainye chusla
// usp->OVN - normal sluchainye chusla
{
  int i,j,k;
  double R;

  if(!reg)
    k=usp->nGR;
  else
    k=usp->nGN;

  for(i=0;i<k;i++)
  {   j=rand();
      R=ceil(24359738368.*j/RAND_MAX + 10000000000.);
      if(!fmod(R,2))
         R=R+1.;
       if(!reg)
         usp->OVR[i+1]=R;
       else
         usp->OVN[i+1]=R;
    }
}

void TUnSpace::BELOV(int QT, int NG, float *OVB)
// QT - kol-vo points
// NG - kol-vo groups
// OVB - chisla po zakonu Belova
{
   int QT2,i,j,k,D,B,W,MC,m2,*LI,*LI1;

   QT2=(QT-1)/2;

   LI = new int[QT2+1];
   LI1 = new int[QT2+1];

   W=floor(float(NG)/float(QT2));
   if(W)
   {  k=1;
      for(i=1;i<=QT2;i++)
         for(j=0;j<=W-1;j++)
            OVB[i+j*QT2]=i;
   }
   else
      k=0;
   W*=QT2;
   if(W>=NG)
    {
        delete[] LI;
        delete[] LI1;
        return;
    }

  OVB[W+1]=k*QT2+1;
  for(i=1;i<=QT2;i++)
   LI[i]=WL(i,OVB[W+1],QT);
  for(D=W+2;D<=NG;D++)
  { MC=0;
    for(j=1;j<=QT;j++)
    { for(i=1;i<=QT2;i++)
         LI1[i]=WL(i,j,QT)+LI[i];
      B=LI1[1];
      for(i=2;i<=QT2;i++)
        if(LI1[i]<B)
           B=LI1[i];
      m2=0;
      if(B==MC)
       for(i=1;i<=D-1;i++)
         if(OVB[i]==OVB[D])
         { OVB[D]=j;
            m2=1;
            break;
         }
       if(!m2&&B>MC)
         { MC=B;
           OVB[D]=j;
         }
      }
      for(i=1;i<=QT2;i++)
        LI[i]+=WL(i,OVB[D],QT);
   }
  delete[] LI;
  delete[] LI1;
}

int TUnSpace::WL(int i,int j,int QT)
{ int W;
  W=i*j-floor(float(i*j)/float(QT))*QT;  //  ?????????? 
  if( QT-W < W )
      W=QT-W;
  return(W);
}

// Belov point
float TUnSpace::PNTBEL(int J,int QT,int OV)
// J - number point
// QT - numbers points
// OV -  number of Belov for group
{ float TB;
  TB=fabs(float(J*OV)-floor(float(J*OV)/float(QT))*float(QT));
  TB+=(QT-1)/2;
  while(1)
   if(TB>=QT) TB-=QT;
    else break;
  return(TB);
}

// uniform point
double TUnSpace::ravrand(double *x)
{ double m35=34359738368., m36=68719476736., m37=137438953472.;
  float a=0.,b=1.;
  *x=*x*5.;
  if(*x>=m37) *x=*x-m37;
  if(*x>=m36) *x=*x-m36;
  if(*x>=m35) *x=*x-m35;
 return(*x/m35*(b-a)+a);
}

// normal point
double TUnSpace::norrand(double *x)
{ double R1=0.;
  int j;
  for(j=0;j<101;j++)
    R1+=ravrand(&*x);
      R1=(R1-101./2.)/pow(101./12.,0.5);
           R1=1./6.*(R1-(-3.0));
           if(R1<0.) R1=0.;
           if(R1>1.) R1=1.;
           return(R1);
/*return(1./9.*(R1-(-4.5)));*/
}

// Next point space uncertainty
void  TUnSpace::NexT(int J )
// J - point of sample
{
  int i,j,k1,k2,k3;
  float R;
  double x, xx;

  k1 = k2 = k3 = 1;
  for(i=1; i<=usp->nG; i++)
  { if(!usp->PbD[i-1])
    {  R = PNTBEL( J, usp->Q, usp->OVB[k1]);
      k1++;
      if(usp->Q > 1)
         R/=(usp->Q-1);
    }
   if(usp->PbD[i-1]==1)
     {  x = usp->OVR[k2];
        R = ravrand(&x);
        usp->OVR[k2] = x;
        k2++;
     }
   if(usp->PbD[i-1]==2)
     {  x = usp->OVN[k3];
        R = norrand(&x);
        usp->OVN[k3] = x;
        k3++;
     }
    usp->ncp[J*usp->nG+(i-1)]=R;

   if( usp->PsGen[0]== S_ON || usp->PsGen[1]== S_ON || usp->PsGen[5]== S_ON )
    for( j=0; j<usp->L; j++)
     if(usp->PsGen[0]== S_ON && usp->NgLg[j]==i||
        usp->PsGen[1]== S_ON && usp->NgLs[j]==i||
        usp->PsGen[5]== S_ON && usp->NgLv[j]==i)
     {

      if(usp->PsGen[0]== S_ON && usp->NgLg[j]==i)
      {
        xx = 2*usp->IntLg[j]*R-usp->IntLg[j];
        TProfil::pm->syp->Guns[j] = xx;
      }
      if(usp->PsGen[1]== S_ON&& usp->NgLs[j]==i)
      {  xx = usp->Ss[j] - usp->IntLs[j]+2*usp->IntLs[j]*R;
//         usp->R1[ii*17+3]= xx;
      }
      if(usp->PsGen[5]== S_ON&& usp->NgLv[j]==i)
      {
        xx = 2*usp->IntLv[j]*R-usp->IntLv[j];
        TProfil::pm->syp->Vuns[j] = xx;
      }
    }
    if( usp->PsGen[2]== S_ON )
     for(j=0; j<usp->N; j++)
     {  if( usp->NgNb[j] == i)
        { xx = usp->Bs[j] - usp->IntNb[j] + 2*usp->IntNb[j]*R;
          if( xx < 0 )
            xx = 0.;
          TProfil::pm->syp->B[j]= xx;
        }
     }
    if( usp->PsGen[3]== S_ON && usp->NgT==i )
     { xx = usp->T - usp->IntT + 2* usp->IntT*R;
       usp->Tc = xx;
       if(usp->Tc<0.)
          usp->Tc=0.;
     }
   if( usp->PsGen[4]== S_ON && usp->NgP==i )
     {
       xx = usp->P- usp->IntP + 2*usp->IntP*R;
       usp->Pc = xx;
       if(usp->Pc<0.) usp->Pc = 1e-5;
     }
   }
}

//=================================================================
