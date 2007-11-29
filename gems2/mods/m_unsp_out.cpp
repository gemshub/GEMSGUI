#include <math.h>
#include <stdio.h>

#ifndef IPMGEMPLUGIN

#include "m_unspace.h"

#else

#include "ms_unspace.h"

#endif
// !!! internal using  syp->GEX, mup->Laq, pm->tpp->G


//=========================================================================
// Output part (must be changed all )

// out information: criteria and statistics
void TUnSpace::out_QT( int Ngr  )
{
  double Ca,Ca1,sCa,sCa1, quanLapl,quanHur,quanWald,quanHom;
  float st=2.,st1=2.; // coeff. Studenta
  int i, k,ii,l,/*ca=0,*/kf, kf1;

 //  paragen( ); // put data to nPhA, PhAndx, PhAfreq ....


//==============================================
// UiDC

  ii = usp->N;
  if( TProfil::pm->mup->Laq )
      ii--;

  for( l=0; l<ii ; l++ )
  {
   Ca = sCa = Ca1 = sCa1= 0.;
   kf=0; kf1=0;
   for( i=0; i<usp->Q; i++ )
   { if(usp->sv[i]>=0)
     { Ca += usp->vMol[i*usp->N+l];
       kf++;
     }
     if( abs(usp->sv[i]) == Ngr )
     {  Ca1 += usp->vMol[i*usp->N+l];
        kf1++;
     }
   }
   if(kf)
    Ca /= kf;
   if(kf1)
    Ca1 /= kf1;

   for( i=0; i<usp->Q; i++ )
   {
     if(usp->sv[i]>=0)
      sCa += (usp->vMol[i*usp->N+l]-Ca) * (usp->vMol[i*usp->N+l]-Ca);
     if( abs(usp->sv[i]) == Ngr )
      sCa1 += (usp->vMol[i*usp->N+l]-Ca1) * (usp->vMol[i*usp->N+l]-Ca1);
   }
   if(kf>1)
     sCa /= (kf-1);
   if(kf1>1)
     sCa1 /= (kf1-1);
   if(sCa>0)
       sCa = sqrt(sCa);
   if(sCa1>0)
       sCa1 = sqrt(sCa1);

   quanLapl=quanHur=quanWald=quanHom=0.;
   for(k=0;k<usp->qQ;k++)
   { quanLapl += usp->vMol[usp->quanCx[k][0]*usp->N+l];
     quanHur +=  usp->vMol[usp->quanCx[k][1]*usp->N+l];
     quanWald += usp->vMol[usp->quanCx[k][2]*usp->N+l];
     quanHom  += usp->vMol[usp->quanCx[k][3]*usp->N+l];
   }
   usp->UnIC[l][0] = usp->vMol[l];
   usp->UnIC[l][1] = Ca;
   if(kf>0)
    usp->UnIC[l][2] = st*sCa / sqrt(double(kf));
   usp->UnIC[l][3] = Ca1;
   if(kf1>0)
    usp->UnIC[l][4] = st1*sCa1 / sqrt(double(kf1));
   usp->UnIC[l][5] = quanLapl/usp->qQ;
   usp->UnIC[l][6] = quanHur/usp->qQ;
   usp->UnIC[l][7] = quanWald/usp->qQ;
   usp->UnIC[l][8] = quanHom/usp->qQ;
   usp->UnIC[l][9] = 0.;

 }

//==============================================
// UaDC

  for( l=0; l<usp->Ls ; l++ )
  {
   Ca = sCa = Ca1 = sCa1= 0.;
   kf=0; kf1=0;

   for( i=0; i<usp->Q; i++ )
   { if(usp->sv[i]>=0)
     { Ca += pow( 10., usp->vFug[i*usp->Ls+l]);
       kf++;
     }
     if( abs(usp->sv[i]) == Ngr )
     {  Ca1 += pow( 10., usp->vFug[i*usp->Ls+l]);
        kf1++;
     }
   }
   if(kf)
    Ca /= kf;
   if(kf1)
    Ca1 /= kf1;

   for( i=0; i<usp->Q; i++ )
   {
     if(usp->sv[i]>=0)
      sCa += (pow( 10., usp->vFug[i*usp->Ls+l]-Ca)) *
                            pow( 10., (usp->vFug[i*usp->Ls+l]-Ca));
     if( abs(usp->sv[i]) == Ngr )
      sCa1 += pow( 10., (usp->vFug[i*usp->Ls+l]-Ca1)) *
                           pow( 10., (usp->vFug[i*usp->Ls+l]-Ca1));
   }
   if(kf>1)
     sCa /= (kf-1);
   if(kf1>1)
     sCa1 /= (kf1-1);
   if(sCa>0)
       sCa = sqrt(sCa);
   if(sCa1>0)
       sCa1 = sqrt(sCa1);

   quanLapl=quanHur=quanWald=quanHom=0.;
   for(k=0;k<usp->qQ;k++)
   { quanLapl += pow( 10., usp->vFug[usp->quanCx[k][0]*usp->Ls+l]);
     quanHur +=  pow( 10., usp->vFug[usp->quanCx[k][1]*usp->Ls+l]);
     quanWald += pow( 10., usp->vFug[usp->quanCx[k][2]*usp->Ls+l]);
     quanHom  += pow( 10., usp->vFug[usp->quanCx[k][3]*usp->Ls+l]);
   }
   usp->UaDC[l][0] = pow( 10., usp->vFug[l]);
   usp->UaDC[l][1] = Ca;
   if(kf>0)
    usp->UaDC[l][2] = st*sCa / sqrt(double(kf));
   usp->UaDC[l][3] = Ca1;
   if(kf1>0)
    usp->UaDC[l][4] = st1*sCa1 / sqrt(double(kf1));
   usp->UaDC[l][5] = quanLapl/usp->qQ;
   usp->UaDC[l][6] = quanHur/usp->qQ;
   usp->UaDC[l][7] = quanWald/usp->qQ;
   usp->UaDC[l][8] = quanHom/usp->qQ;
   usp->UaDC[l][9] = 0.;
 }


//==============================================
// UgDC
for( int ii=0; ii<usp->nPG; ii++ )
{
  double var1=0, var3=0, var_L=0, var_H=0, var_W=0, var_Hom=0;
  int sz1 = 0, sz3 = 0;

  usp->UgDC[ii][0] = value_nPG( ii, 0 );

  for(int q=0; q<usp->Q; q++)
  {
    if( usp->sv[q] >0 )
    { var1 += value_nPG( ii, q );
      sz1++;
    }
    if( abs(usp->sv[q]) == Ngr )
    { var3 += value_nPG( ii, q );
      sz3++;
    }
  }

   if( sz1)
     usp->UgDC[ii][1] = var1/sz1;
   else
     usp->UgDC[ii][1] = 0;

   if( sz3>0 )
     usp->UgDC[ii][3] = var3/sz3;
   else
     usp->UgDC[ii][3] = 0;

  var1 = 0;
  var3 = 0;
  for(int q=0; q<usp->Q; q++)
  {
    if( usp->sv[q] >0 )
     var1 += pow( usp->UgDC[ii][1]- value_nPG( ii, q ), 2);
    if( abs(usp->sv[q]) == Ngr )
     var3 += pow( usp->UgDC[ii][3]- value_nPG( ii, q ), 2);
  }

   if( sz1>1 )
     usp->UgDC[ii][2] = st*sqrt(var1/(sz1-1)) / sqrt(double(sz1));
   else
     usp->UgDC[ii][2] = 0;

   if( sz3>1 )
     usp->UgDC[ii][4] = st*sqrt(var3/(sz3-1)) / sqrt(double(sz3));
   else
     usp->UgDC[ii][4] = 0;

  for(int t=0; t<usp->qQ; t++)
  {
    var_L += value_nPG( ii, usp->quanCx[t][0] );
    var_H += value_nPG( ii, usp->quanCx[t][1] );
    var_W += value_nPG( ii, usp->quanCx[t][2] );
    var_Hom += value_nPG( ii, usp->quanCx[t][3] );
    // calc statistics
  }

  usp->UgDC[ii][5] = var_L/usp->qQ;
  usp->UgDC[ii][6] = var_H/usp->qQ;
  usp->UgDC[ii][7] = var_W/usp->qQ;
  usp->UgDC[ii][8] = var_Hom/usp->qQ;

  var1 = 0;
  for(int t=0; t<usp->qQ; t++)
  {
    switch( usp->Pa_Crit )
    {
     case UNSP_CRIT_LAPL_QAN:
        var1 += pow( usp->UgDC[ii][5]- value_nPG( ii, usp->quanCx[t][0] ), 2);
        break;
     case UNSP_CRIT_HUR_QAN:
        var1 += pow( usp->UgDC[ii][6]- value_nPG( ii, usp->quanCx[t][1] ), 2);
        break;
     case UNSP_CRIT_WALD_QAN:
        var1 += pow( usp->UgDC[ii][7]- value_nPG( ii, usp->quanCx[t][2] ), 2);
        break;
     case UNSP_CRIT_HOME_QAN:
        var1 += pow( usp->UgDC[ii][8]- value_nPG( ii, usp->quanCx[t][3] ), 2);
        break;
   }
  }
   if( usp->qQ >1 )
     usp->UgDC[ii][9] = st*sqrt(var1/(usp->qQ-1)) / sqrt(double(usp->qQ));
   else
     usp->UgDC[ii][9] = 0;
}
//=================================================

}

//calculation number of unspace components
double TUnSpace::value_nPG( int line, int q )
{
  int j, count=0;

  if( usp->PsGen[3]== S_ON &&  usp->NgT > 0 )
  {   if( line == count )
       return usp->vT[q];
      count++;
  }
  if( usp->PsGen[4]== S_ON &&  usp->NgP > 0 )
  {   if( line == count )
       return usp->vP[q];
      count++;
  }
  if( /*usp->PsGen[]== S_ON && */ usp->NgV > 0 )
  {   if( line == count )
       return usp->vV[q];
      count++;
  }

  if( usp->PsGen[2]== S_ON )
     for( j=0; j<usp->N; j++)
       if( usp->NgNb[j] > 0  )
       {   if( line == count )
          return usp->vB[q*usp->N+j];
         count++;
        }

  if( usp->PsGen[0]== S_ON )
     for( j=0; j<usp->L; j++)
       if( usp->NgLg[j] > 0  )
       {   if( line == count )
          return usp->vG[q*usp->L+j];
         count++;
        }

  if( usp->PsGen[1]== S_ON )
     for( j=0; j<usp->L; j++)
       if( usp->NgLs[j] > 0  )
       {   if( line == count )
          return usp->vS[q*usp->L+j];
         count++;
        }

  if( usp->PsGen[5]== S_ON )
     for( j=0; j<usp->L; j++)
       if( usp->NgLv[j] > 0  )
       {   if( line == count )
          return usp->vmV[q*usp->L+j];
         count++;
        }

  if( usp->PsGen[6]== S_ON )
     for( j=0; j<usp->Ls; j++)
       if( usp->NgGam[j] > 0  )
       {   if( line == count )
          return usp->vNidP[q*usp->Ls+j];
          count++;
        }

  return 0;
}


//  correction values G298 and intervals
void TUnSpace::AdapG()
// Iter- iteration of adaptation
{
  int Ngr;
 // Ngr - number set phases from criteria
 Ngr = sel_parg( usp->sv );


//===============================================
// UnDCA
for( int ii=0; ii<usp->nPG; ii++ )
{
  double val, sg_val, sr=0, min=0, max=0, sg_sr=0;
  int i1 = 0, sr_sz = 0;


  for(int q=0; q<usp->Q; q++)
  {
    if( usp->sv[q] == Ngr )      // filters and phase selections
    {
      val = value_nPG( ii, q );
      sr += val;
      sr_sz++;
      if(!i1)  // first
      {  min = max = val; i1++; }
      else
      {
        if( val < min ) min = val;
        if( val > max ) max = val;
      }
    }
  }

  if( sr_sz )
     sr = sr/sr_sz;
  else
     sr = 0;

  for(int q=0; q<usp->Q; q++)
  {
    if( usp->sv[q] == Ngr )
     sg_sr += pow( sr - value_nPG( ii, q ), 2);
  }

  if( sr_sz >1 )
     sg_sr = 3*sqrt(sg_sr/(sr_sz-1)) / sqrt(double(sr_sz));
  else
     sg_sr = 0;

  switch( usp->Pa_Crit )
   {
    case UNSP_CRIT_PA:         val = sr;   sg_val = sg_sr;  break;
    case UNSP_CRIT_LAPL_QAN:   val = usp->UgDC[ii][5];
                               sg_val = usp->UgDC[ii][9];
                               break;
    case UNSP_CRIT_HUR_QAN:    val = usp->UgDC[ii][6];
                               sg_val = usp->UgDC[ii][9];
                               break;
    case UNSP_CRIT_WALD_QAN:   val = usp->UgDC[ii][7];
                               sg_val = usp->UgDC[ii][9];
                               break;
    case UNSP_CRIT_HOME_QAN:   val = usp->UgDC[ii][8];
                               sg_val = usp->UgDC[ii][9];
    default:                   break;
   }
   adapt_nPG( ii, val, sg_val );

   usp->UnDCA[ii][0] = val;
   usp->UnDCA[ii][1] = sg_val;
   usp->UnDCA[ii][2] = min;
   usp->UnDCA[ii][3] = max;
   usp->UnDCA[ii][4] = sr;
   usp->UnDCA[ii][5] = sg_sr;
   usp->UnDCA[ii][6] = 0.;
   usp->UnDCA[ii][7] = 0.;
 }
}

//set up new data and inervals for  unspace components
void TUnSpace::adapt_nPG( int line, double new_val, double new_int )
{
  int j, count=0;

  if( usp->PsGen[3]== S_ON &&  usp->NgT > 0 )
  {   if( line == count )
      {
       usp->T[0] = new_val;
       usp->IntT[0] = new_int;
       return;
      }
      count++;
  }
  if( usp->PsGen[4]== S_ON &&  usp->NgP > 0 )
  {   if( line == count )
      {
       usp->P[0] = new_val;
       usp->IntP[0] = new_int;
       return;
      }
      count++;
  }
  if( /*usp->PsGen[]== S_ON && */ usp->NgV > 0 )
  {   if( line == count )
      {
       usp->V[0] = new_val;
       usp->IntV[0] = new_int;
       return;
      }
      count++;
  }

  if( usp->PsGen[2]== S_ON )
     for( j=0; j<usp->N; j++)
       if( usp->NgNb[j] > 0  )
       {  if( line == count )
          {
            usp->Bs[j][0] = new_val;
            usp->IntNb[j][0] = new_int;
            return;
          }
         count++;
     }

  if( usp->PsGen[0]== S_ON )
    for( j=0; j<usp->L; j++)
       if( usp->NgLg[j] > 0  )
       {   if( line == count )
           {
            usp->Gs[j][0] = (float)new_val;
            usp->IntLg[j][0] = new_int;
            //syu->GEX[j] = // 16/02/2007
            // usp->Gs[j][0]-float(TProfil::pm->tpp->G[j]);
            for( int jj=0; jj<pmu->L; jj++)
             if( pmu->muj[jj] == j )
             {
               pmu->GEX[jj] = (usp->Gs[j][0]-
                float(TProfil::pm->tpp->G[j]))/pmu->RT;
               break;
             }
            return;
           }
         count++;
        }

  if( usp->PsGen[1]== S_ON )
     for( j=0; j<usp->L; j++)
       if( usp->NgLs[j] > 0  )
       {   if( line == count )
           {
             usp->Ss[j][0] = new_val;
             usp->IntLs[j][0] = new_int;
             return;
           }
         count++;
        }

  if( usp->PsGen[5]== S_ON )
     for( j=0; j<usp->L; j++)
       if( usp->NgLv[j] > 0  )
       {   if( line == count )
           {
             usp->Vs[j][0] = new_val;
             usp->IntLv[j][0] = new_int;
             return;
           }
         count++;
        }

  if( usp->PsGen[6]== S_ON )
     for( j=0; j<usp->Ls; j++)
       if( usp->NgGam[j] > 0  )
       {   if( line == count )
           {
             usp->GAMs[j][0] = new_val;
             usp->IntGam[j][0] = new_int;
             return;
           }
          count++;
        }
}

//====================================================================


