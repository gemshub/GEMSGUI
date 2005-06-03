#include <math.h>
#include <stdio.h>

#include "m_unspace.h"
#include "visor.h"

//=========================================================================
// Analyze part
//=========================================================================

// calculated pay-off matrix  && analise data
void TUnSpace::analiseArrays( )
{
     int k, Ngr;

     usp->ob=0;

     setPhaseAssemb();   // set up phase assemblages data
     Ngr = sel_parg( usp->sv );

     for( k=0; k<usp->Q; k++ )
        if(  usp->sv[k] == Ngr /*&& !filters( k ) */ )
                // sv[i] < 0  if !filters
                // Ngr > 0  aqlways
            usp->ob++;
     switch( usp->Pa_OF )
     {
       case UNSP_OF_A:
       case UNSP_OF_B:
       default:   Un_criteria( );     // biuld payoff matrix and analise
                  out_QT( Ngr );  // put data for resalt arrays
                  break;

     }
}



short TUnSpace::kol_in_sol( int j )
//  return: kol-vo solutions s naborom phases kak v j-solution
{
return usp->PhNum[ abs(usp->sv[j])-1];
}

int TUnSpace::kolgrup()
// return: kol-vo phase groups
{
return usp->nPhA;
}


int TUnSpace::kvant_parag( int type /*short *mas*/ )
{   // sol[i] - kol-vo solutions c usp->quanCx[i][type] naborom phases
    // return:  kol-vo solutions v quantile

  int i,j,I=0,S=0,sum,kvant;
  short maxkol,*sol;

  kvant = usp->quan_lev*usp->Q;
  if(kvant<1)
       kvant=1;
  sol= new  short[kvant];

  for( i=0; i<kvant; i++)
   sol[i]= kol_in_sol( usp->quanCx[i][type] );
  for( i=0; i<kvant; i++)
  { sum=0;
    for( j=0; j<kvant ; j++)
     if( sol[j] == sol[i] )
       sum++;
    if( sum > S)
      { S=sum; I=i; }
  }
maxkol = sol[I];
delete[] sol;
return(I); //return(maxkol);
}



// indexes into array <mas> po quantile
void TUnSpace::kvant_index(float per,int N,double *mas, short type /**ind*/)
{
  double A0=-1,A1=fabs(mas[0]);
  short i,j,kvant;

  kvant=per*N;
  if(kvant<1)
      kvant=1;
  for(j=0; j<kvant; j++)
  { for(i=0 ;i<N; i++)
     if(fabs(mas[i])>A0 && fabs(mas[i])<=A1)
     { A1=fabs(mas[i]);
       usp->quanCx[j][type]=i;   //ind[j]=i;
       usp->quanCv[j][type]=mas[i];
     }
    A0=A1;
    for(i=0;i<N;i++)
     if(fabs(mas[i])>A1)
      { A1=fabs(mas[i]); break; }
   }
}


int TUnSpace::sel_parg( short *sv ) 
{
  int i, j,I,J, maxgr=1;

 if( usp->Pa_Crit == UNSP_CRIT_PA ) // select statistic
 {
  J = 0;
  for( j=0; j<usp->Q; j++)
     if(abs(sv[j])==1)
          J++;
  for(i=1; i<kolgrup(); i++ )
  { I=0;
    for( j=0; j<usp->Q; j++ )
     if(abs(sv[j])==i+1)
        I++;
    if(I>J)
    { maxgr = i+1 ; J=I; }
  }
 }
 else
  if(usp->Pa_Crit <= UNSP_CRIT_HOME_QAN)
  { maxgr = kvant_parag( usp->Pa_Crit - UNSP_CRIT_LAPL_QAN );
    maxgr++;
  }
return(maxgr);

}

int TUnSpace::filters( int k )
// return: 1-not, 0-yes (propuskaet)
{
  int Filtr=0,i;
  if( usp->Pa_f_pH == S_ON &&  ( usp->vpH[k] < usp->pH_lo ||
                         usp->vpH[k] > usp->pH_up ))
    Filtr=1;
  if( usp->Pa_f_fug == S_ON )
     for(i=0; i<usp->Ls; i++)
      if(  usp->fug_up[i] &&
          ( usp->vFug[k*usp->Ls+i] < usp->fug_lo[i] ||
            usp->vFug[k*usp->Ls+i] > usp->fug_up[i] ))
       Filtr=1;
  if( usp->Pa_f_mol == S_ON )
    for(i=0; i<usp->N;i++)
       if( usp->m_t_up[i] &&
        ( usp->vMol[k*usp->N+i] < usp->m_t_lo[i] ||
          usp->vMol[k*usp->N+i] > usp->m_t_up[i]))
       Filtr=1;
  // phases
  if( usp->Pa_f_pha == S_ON )
     for(i=0; i<usp->N; i++)
      if(  usp->PhAndx[(abs(usp->sv[k])-1)*usp->N+i] != usp->f_PhA[i] )
         Filtr=1;

return(Filtr);
}

//=========================================================================
// Calculated part (not for change)

// calculate element pay-off matrix e(t,q) function (1)
// i=t, j=q
double TUnSpace::ePO( int i, int j )
{
  double PM,rab,RG;
  int k,ii,z,GF=-1,WF=-1,i1,j1;

  RG = R_CONSTANT;
  if(usp->PvSi == S_ON) 
    { j1=j; i1=i;}
  else
    { j1=i; i1=j;} 


  if( TProfil::pm->mup->Laq )
     WF=0;

  if( TProfil::pm->mup->Pg )
  { if(!TProfil::pm->mup->Laq)
         GF=0;
    else GF=1;
   }
   PM=0.;
   ii=0;
   for( z=0; z<usp->Fi; z++)
   {
     for( k=ii; k<ii+(TProfil::pm->mup->Ll[z]); k++)        
      if (TProfil::pm->syp->Dcl[k]!=S_OFF ) 
       { if( z==WF )
         { if( k < (TProfil::pm->mup->Laq-1) &&
               usp->vYF[i*usp->Fi] >1e-19 &&
               usp->vY[i*usp->L+(TProfil::pm->mup->Laq)-1] >1e-19 &&
               usp->vY[i*usp->L+ k ] >1e-19 )
           
             PM += (usp->vG[j1*usp->L+k]/(RG*(usp->vT[j1]+273.15)) + log(55.51) - usp->vY[i*usp->L+(TProfil::pm->mup->Laq)-1]/
                    usp->vYF[i*usp->Fi] + log(usp->vY[i*usp->L+k]) - log(usp->vY[i*usp->L+(TProfil::pm->mup->Laq)-1]) +
                    1. + usp->vGam[i*usp->L+k]) * usp->vY[i*usp->L+k];
           
           if( k == (TProfil::pm->mup->Laq-1) &&
               usp->vY[i*usp->L+(TProfil::pm->mup->Laq-1)] > 1e-19 &&
               usp->vYF[i*usp->Fi+z] > 1e-19 )
            
              PM += (usp->vG[j1*usp->L+k]/(RG*(usp->vT[j1]+273.15)) + log( usp->vY[i*usp->L+k] ) - log( usp->vYF[i*usp->Fi+z] ) -
                  ( usp->vY[i*usp->L+k] / usp->vYF[i*usp->Fi+z]) - ( usp->vYF[i*usp->Fi+z] / usp->vY[i*usp->L+k]) +
                  2.+ usp->vGam[i*usp->L+k]) * usp->vY[i*usp->L+k];
            
          continue;
         }
        if ( z==GF)
        { if( usp->vY[i*usp->L+k] > 1e-19 &&  usp->vYF[i*usp->Fi+z] >1e-19 )
            PM += ( usp->vG[j1*usp->L+k] / (RG*(usp->vT[j1]+273.15)) +
                    log(usp->vY[i*usp->L+k]) - log(usp->vYF[i*usp->Fi+z]) +
                    usp->vGam[i*usp->L+k] + log(usp->vP[i]) ) * (usp->vY[i*usp->L+k]);
         continue;
        }
        if( z>WF && z>GF &&
            usp->vY[i*usp->L+k] > 1e-19 &&
            usp->vYF[i*usp->Fi+z] > 1e-19 )
           PM += ( usp->vG[j1*usp->L+k] / (RG*(usp->vT[j1]+273.15)) + log(usp->vY[i*usp->L+k])
                   - log(usp->vYF[i*usp->Fi+z])  + usp->vGam[i*usp->L+k] ) * (usp->vY[i*usp->L+k] );
       }
      ii += TProfil::pm->mup->Ll[z];
     }

     for( z=0; z<usp->N; z++)
       PM -= TProfil::pm->syp->B[z] * (usp->vU[i1*usp->N+z]);

   return(PM);
}

// calculate element pay-off matrix e(t,q) function (2)
// i=t, j=q
double TUnSpace::ePO1( int i,int j )
{
  double PM,rab,RG;
  int k,ii,z,GF=-1,WF=-1,i1,j1;

  RG = R_CONSTANT;
  if(usp->PvSi == S_ON) 
    { j1=j; i1=i;}
  else
    { j1=i; i1=j;} 
  if( TProfil::pm->mup->Laq )
     WF=0;

  if( TProfil::pm->mup->Pg )
  { if(!TProfil::pm->mup->Laq)
         GF=0;
    else GF=1;
   }

  PM=0.;
  ii=0;
  for( z=0; z<usp->Fi; z++)
  {
   for( k=ii; k<ii+(TProfil::pm->mup->Ll[z]); k++ )
    if (TProfil::pm->syp->Dcl[k]!=S_OFF ) 
    {
      if( z==WF )
      {
         if( k < (TProfil::pm->mup->Laq-1) &&
               usp->vYF[i*usp->Fi] >1e-19 &&
               usp->vY[i*usp->L+(TProfil::pm->mup->Laq)-1] >1e-19 &&
               usp->vY[i*usp->L+ k ] >1e-19  )
           
             PM += usp->vG[j1*usp->L+k]/(RG*(usp->vT[j1]+273.15)) + log(55.51) - usp->vY[i*usp->L+(TProfil::pm->mup->Laq-1)]/
                    usp->vYF[i*usp->Fi] + log(usp->vY[i*usp->L+k]) - log(usp->vY[i*usp->L+(TProfil::pm->mup->Laq-1)]) +
                   1. + usp->vGam[i*usp->L+k];
             
           if( k == (TProfil::pm->mup->Laq-1) && usp->vY[i*usp->L+(TProfil::pm->mup->Laq-1)] > 1e-19 &&
               usp->vYF[i*usp->Fi+z] > 1e-19 )

              PM += usp->vG[j1*usp->L+k]/(RG*(usp->vT[j1]+273.15)) + log( usp->vY[i*usp->L+k] ) - log( usp->vYF[i*usp->Fi+z] ) -
                 ( usp->vY[i*usp->L+k] / usp->vYF[i*usp->Fi+z]) - ( usp->vYF[i*usp->Fi+z] / usp->vY[i*usp->L+k]) +
                 2.+ usp->vGam[i*usp->L+k];
              
            continue;
         }
        if ( z==GF)
        { if( (usp->vY[i*usp->L+k] > 1e-19) && usp->vYF[i*usp->Fi+z] >1e-19 )
            PM += ( usp->vG[j1*usp->L+k] / (RG*(usp->vT[j1]+273.15)) + log(usp->vY[i*usp->L+k]) - log(usp->vYF[i*usp->Fi+z]) +
                    usp->vGam[i*usp->L+k] + log(usp->vP[i]) );
            continue;
        }
        if( z>WF && z>GF &&
            usp->vY[i*usp->L+k] > 1e-19 &&  usp->vYF[i*usp->Fi+z] > 1e-19 )
           PM += ( usp->vG[j1*usp->L+k] / (RG*(usp->vT[j1]+273.15)) + log(usp->vY[i*usp->L+k])
                   - log(usp->vYF[i*usp->Fi+z]) + usp->vGam[i*usp->L+k] );
        }
      ii += TProfil::pm->mup->Ll[z];
     }

     double R=0.;
     for( k=0; k< usp->L; k++)
      if (TProfil::pm->syp->Dcl[k]!=S_OFF && usp->vY[i1*usp->L+k] > 1e-19 )
        for( z=0; z<usp->N; z++)
           R += usp->A[k*usp->N+z]* (usp->vU[i1*usp->N+z]);

  PM-=R;
  return(PM);
}

double TUnSpace::ePO2( int i,int j )
{
  double PM,R,RG;
  int k,ii,z,GF=-1,WF=-1,i1,j1;

  RG = R_CONSTANT;
  if(usp->PvSi == S_ON) 
    { j1=j; i1=i;}
  else
    { j1=i; i1=j;} 
  if( TProfil::pm->mup->Laq )
     WF=0;

  if( TProfil::pm->mup->Pg )
  { if(!TProfil::pm->mup->Laq)
         GF=0;
    else GF=1;
   }

  PM=R=0.;
  ii=0;
  for( z=0; z<usp->Fi; z++)
  {
    for( k=ii; k<ii+(TProfil::pm->mup->Ll[z]); k++ )
     if (TProfil::pm->syp->Dcl[k]!=S_OFF ) 
    {
      if( z==WF )
      {
         if( k < (TProfil::pm->mup->Laq-1) &&
               usp->vYF[i*usp->Fi] >1e-19 &&
               usp->vY[i*usp->L+(TProfil::pm->mup->Laq)-1] >1e-19 &&
               usp->vY[i*usp->L+ k ] >1e-19 )
           PM += usp->vG[j1*usp->L+k]/(RG*(usp->vT[j1]+273.15)) + log(55.51) -
             usp->vY[i*usp->L+(TProfil::pm->mup->Laq-1)]/usp->vYF[i*usp->Fi] +
             log(usp->vY[i*usp->L+k]) - log(usp->vY[i*usp->L+(TProfil::pm->mup->Laq-1)]) +
             1. + usp->vGam[i*usp->L+k];
          if( k == (TProfil::pm->mup->Laq-1) &&
               usp->vY[i*usp->L+(TProfil::pm->mup->Laq-1)] > 1e-19 &&
               usp->vYF[i*usp->Fi+z] > 1e-19 )
            PM += usp->vG[j1*usp->L+k]/(RG*(usp->vT[j1]+273.15)) + log( usp->vY[i*usp->L+k] ) -
              log( usp->vYF[i*usp->Fi+z] ) - ( usp->vY[i*usp->L+k] / usp->vYF[i*usp->Fi+z]) -
              ( usp->vYF[i*usp->Fi+z] / usp->vY[i*usp->L+k]) + 2.+ usp->vGam[i*usp->L+k];
           continue;
       }
        if( ( z==GF) && (usp->vY[i*usp->L+k] > 1e-19) &&
             usp->vYF[i*usp->Fi+z] >1e-19 )
        {
             PM += ( usp->vG[j1*usp->L+k] / (RG*(usp->vT[j1]+273.15)) +
                    log(usp->vY[i*usp->L+k]) - log(usp->vYF[i*usp->Fi+z]) +
                    usp->vGam[i*usp->L+k] + log(usp->vP[i]) );
            continue;
        }
        if( z>WF && z>GF &&
            usp->vY[i*usp->L+k] > 1e-19 && usp->vYF[i*usp->Fi+z] > 1e-19 )
          PM += ( usp->vG[j1*usp->L+k] / (RG*(usp->vT[j1]+273.15))+ log(usp->vY[i*usp->L+k])
                   - log(usp->vYF[i*usp->Fi+z]) + usp->vGam[i*usp->L+k] );
      }
      ii += TProfil::pm->mup->Ll[z];
     }
  ii=0;
  for( z=0; z<usp->Fi; z++)
  {
    for( k=ii; k<ii+(TProfil::pm->mup->Ll[z]); k++ )
     if (TProfil::pm->syp->Dcl[k]!=S_OFF ) 
    {
      if( z==WF )
      {
         if( k < (TProfil::pm->mup->Laq-1) &&
               usp->vYF[j*usp->Fi] >1e-19 &&
               usp->vY[j*usp->L+(TProfil::pm->mup->Laq)-1] >1e-19 &&
               usp->vY[j*usp->L+ k ] >1e-19 )
           R += usp->vG[i1*usp->L+k]/(RG*(usp->vT[i1]+273.15)) + log(55.51) -
             usp->vY[j*usp->L+(TProfil::pm->mup->Laq-1)]/usp->vYF[j*usp->Fi] +
             log(usp->vY[j*usp->L+k]) - log(usp->vY[j*usp->L+(TProfil::pm->mup->Laq-1)]) +
             1. + usp->vGam[j*usp->L+k];
          if( k == (TProfil::pm->mup->Laq-1) &&
               usp->vY[j*usp->L+(TProfil::pm->mup->Laq-1)] > 1e-19 &&
               usp->vYF[j*usp->Fi+z] > 1e-19 )
            R += usp->vG[i1*usp->L+k]/(RG*(usp->vT[i1]+273.15)) + log( usp->vY[j*usp->L+k] ) -
              log( usp->vYF[j*usp->Fi+z] ) - ( usp->vY[j*usp->L+k] / usp->vYF[j*usp->Fi+z]) -
              ( usp->vYF[j*usp->Fi+z] / usp->vY[j*usp->L+k]) + 2.+ usp->vGam[j*usp->L+k];
           continue;
       }
        if( ( z==GF) && (usp->vY[j*usp->L+k] > 1e-19) &&
             usp->vYF[j*usp->Fi+z] >1e-19 )
        {
             R += ( usp->vG[i1*usp->L+k] / (RG*(usp->vT[i1]+273.15)) +
                    log(usp->vY[j*usp->L+k]) - log(usp->vYF[j*usp->Fi+z]) +
                    usp->vGam[j*usp->L+k] + log(usp->vP[j]) );
            continue;
        }
        if( z>WF && z>GF &&
            usp->vY[j*usp->L+k] > 1e-19 && usp->vYF[j*usp->Fi+z] > 1e-19 )
          R += ( usp->vG[i1*usp->L+k] / (RG*(usp->vT[i1]+273.15))+ log(usp->vY[j*usp->L+k])
                   - log(usp->vYF[j*usp->Fi+z]) + usp->vGam[j*usp->L+k] );
      }
      ii += TProfil::pm->mup->Ll[z];
     }
  PM-=R;
  return(PM);
}

double TUnSpace::ePO3( int i,int j )
{  double R=0.,PM=0.;
   int k,z;
   for(k=0;k<usp->L;k++)
    for(z=0;z<usp->N;z++)
      R+=usp->A[k*usp->N+z]*(*(usp->vU+j*usp->N+z));
   PM+=R;
   R=0;
   for(k=0;k<usp->L;k++)
    for(z=0;z<usp->N;z++)
      R+=usp->A[k*usp->N+z]*(*(usp->vU+i*usp->N+z));
    PM-=R;
 return(PM);
}

double TUnSpace::ePO4( int i,int j )
{  double PM=0.;
   int z;
   for(z=0;z<usp->N;z++)
     PM+= (usp->vU[j*usp->N+z]) - (usp->vU[i*usp->N+z]);
 return(PM);
}


double TUnSpace::ePO5( int i,int j )
{  double PM=0.;
   int z;
   for(z=0;z<usp->N;z++)
     PM+= TProfil::pm->syp->B[z]*(usp->vU[j*usp->N+z]) -
          TProfil::pm->syp->B[z]*(usp->vU[i*usp->N+z]);
 return(PM);
}

double TUnSpace::g_uw( int j, double *U, float *A,
       double lgGAM,double x,double xjw, double Xw)
// dual solution water
{ double R,gk;
  int i;
  R=0.;
  for( i=0; i<usp->N; i++)
    R+= A[j*usp->N+i]*( U[i] * R_CONSTANT * 298.15);
  gk = R + R_CONSTANT * 298.15 *
       (-lgGAM - log(55.51) - log(x/Xw) + log(xjw/Xw) + xjw/Xw-1 );
return(gk);
}



double TUnSpace::g_u( int j, double *U,float *A,
                      double lgGAM,double x,double Xw)
// dual solution others
{ double R,gk;
  int i;
  R=0.;
  for(i=0;i<usp->N;i++)
   R+= A[j*usp->N+i] * (U[i] * R_CONSTANT * 298.15);
  gk= R + R_CONSTANT * 298.15 * (-lgGAM + log(x/Xw));
return(gk);
}


// calculate pay-off matrix with different functions
void TUnSpace::Un_criteria()
{
   short  t, q, jj;
   double R;
   double Kr=0.;

   for( t=0; t<usp->Q; t++ )
   {
   pVisor->Message( window(), GetName(),
             "Generation of pay off matrix\n"
                 "Please, wait...", t, usp->Q);


    for( q=0; q<usp->Q; q++ )
    {
      usp->t = t;
      usp->q = q;
      switch( usp->Pa_OF )
     {
       case UNSP_OF_A:  R = ePO( t,q ); break;
       case UNSP_OF_B:  R = ePO1( t,q ); break;
       case UNSP_OF_C:  R = ePO2( t,q ); break;
       case UNSP_OF_D:  R = ePO3( t,q ); break;
       case UNSP_OF_E:  R = ePO4( t,q ); break;
       case UNSP_OF_F:  R = ePO5( t,q ); break;
     }
        if(!q)
         {
            // save  pay-off matrix
            if( usp->PvPOM == S_ON )
            {
             usp->pmr = usp->POM + t*usp->Q ;
             aObj[ o_unpmr].SetPtr( usp->pmr );
             usp->POM[ t*usp->Q+q ] = R;
            }
            if( usp->PvPOR == S_ON )
             usp->POR[ q ] = R;
           usp->Zmin[t] = usp->Zmax[t]= R;
         if( usp->Pa_Zcp == S_OFF )
           usp->Zcp[t] = R;
          else
           usp->Zcp[t] = fabs(R);
           usp->ZmaxAbs[t] = fabs(R);
         }
        else
        {
            // save  pay-off matrix
            if( usp->PvPOM == S_ON )
             usp->POM[ t*usp->Q+q ] = R;
            if( usp->PvPOR == S_ON )
             usp->POR[ q ] = R;

             if( R >usp->Zmax[t] )
                 usp->Zmax[t] = R;
             if( R < usp->Zmin[t] )
                 usp->Zmin[t] = R;
             if( fabs(R) > usp->ZmaxAbs[t] )
                 usp->ZmaxAbs[t] = fabs(R);
         if( usp->Pa_Zcp == S_OFF )
             usp->Zcp[t] += R;
          else
             usp->Zcp[t] += fabs(R);
         }
        Kr += fabs(R);
        if(!t )
         usp->Prob[q] = fabs(R);
        else
         usp->Prob[q] += fabs(R);
    }

    if( usp->Q)
      usp->Zcp[t] /= (double)usp->Q;
   }

// posibility for Homenuk

   for( q=0; q<usp->Q;q++)
       usp->Prob[q] /= Kr;

//  Wald criteria
   R = usp->ZmaxAbs[0];
   jj=0;
   for( t=1; t<usp->Q; t++ )
    if( usp->ZmaxAbs[t] < R )
    { jj=t;
      R= usp->ZmaxAbs[t];
    }
   usp->Wald = jj;
   usp->CrW = R;
   usp->nw = kol_in_sol(jj );
   kvant_index( usp->quan_lev, usp->Q, usp->ZmaxAbs, 2 );


//  Hurwitz criteria (a=0.5)
    R = fabs( usp->Zmin[0] *.5 + usp->Zmax[0]* .5 );
    jj=0;
    for( t=1; t<usp->Q; t++ )
      if( fabs( usp->Zmin[t]*.5 + usp->Zmax[t]*.5 ) < R )
        { jj = t;
          R = fabs( usp->Zmin[t]*.5 + usp->Zmax[t]*.5 );
        }
   usp->Hurw = jj;
   usp->CrH = R;
   usp->nh = kol_in_sol( jj );
   for( t=0; t<usp->Q; t++)
    usp->Hom[t]= fabs( usp->Zmin[t]*.5 + usp->Zmax[t]*.5 ); // usp->Hom - working array
   kvant_index( usp->quan_lev, usp->Q, usp->Hom, 1 );

//  Laplace criteria
if( usp->nGB > 0)
  R = 1e6;
else
  R= fabs( usp->Zcp[0]);
   jj=0;
  for( t=0; t<usp->Q; t++ )
     if(fabs( usp->Zcp[t] ) < R )
      { jj = t;
        R = fabs(usp->Zcp[t]);
      }
   usp->Lapl = jj;
   usp->CrL = R;
   usp->nl = kol_in_sol( jj );


   for( t=0; t<usp->Q; t++)
    usp->Zcp[t]= fabs( usp->Zcp[t]);
   kvant_index( usp->quan_lev, usp->Q, usp->Zcp, 0/*usp->quanLap*/ );

// 2 prohod for Homenuk

   for( t=0; t<usp->Q; t++ )
   {
    for( q=0; q<usp->Q; q++ )
    {   
       switch( usp->Pa_OF )
       {
         case UNSP_OF_A:  R = ePO( t,q ); break;
         case UNSP_OF_B:  R = ePO1( t,q ); break;
         case UNSP_OF_C:  R = ePO2( t,q ); break;
         case UNSP_OF_D:  R = ePO3( t,q ); break;
         case UNSP_OF_E:  R = ePO4( t,q ); break;
         case UNSP_OF_F:  R = ePO5( t,q ); break;
       }
         
        if(!q)
          usp->Hom[t] = R*usp->Prob[q];
        else
          usp->Hom[t] += R*usp->Prob[q];
    }
     usp->Hom[t] = fabs( usp->Hom[t] );
   }

//  Homenuk criteria
   R = usp->Hom[0];
   jj=0;
   for( t=1; t<usp->Q; t++ )
     if( usp->Hom[t] < R )
      { jj = t;
        R = usp->Hom[t];
      }
   usp->Homen = jj;
   usp->CrHom = R;
   usp->nHom = kol_in_sol( jj );
   kvant_index( usp->quan_lev, usp->Q, usp->Hom, 3/*usp->quanHom*/ );

}


//=========================================================================
// Output part (must be changed all )

// out information: criteria and statistics
void TUnSpace::out_QT( int Ngr  )
{
  double Ca,Ca1,sCa,sCa1, quanLapl,quanHur,quanWald,quanHom;
  float st=2.,st1=2.; // coeff. Studenta
  int i, k,ii,l,ca=0,kf, kf1;

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
     { Ca += pow( 10, usp->vFug[i*usp->Ls+l]);
       kf++;
     }
     if( abs(usp->sv[i]) == Ngr )
     {  Ca1 += pow( 10, usp->vFug[i*usp->Ls+l]);
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
      sCa += (pow( 10, usp->vFug[i*usp->Ls+l]-Ca)) * pow( 10, (usp->vFug[i*usp->Ls+l]-Ca));
     if( abs(usp->sv[i]) == Ngr )
      sCa1 += pow( 10, (usp->vFug[i*usp->Ls+l]-Ca1)) * pow( 10, (usp->vFug[i*usp->Ls+l]-Ca1));
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
   { quanLapl += pow( 10, usp->vFug[usp->quanCx[k][0]*usp->Ls+l]);
     quanHur +=  pow( 10, usp->vFug[usp->quanCx[k][1]*usp->Ls+l]);
     quanWald += pow( 10, usp->vFug[usp->quanCx[k][2]*usp->Ls+l]);
     quanHom  += pow( 10, usp->vFug[usp->quanCx[k][3]*usp->Ls+l]);
   }
   usp->UaDC[l][0] = pow( 10, usp->vFug[l]);
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
        min = max = val;
      else
      {
        if( val < min ) min = val;
        if( val < min ) min = val;
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

// output table with set phases: numbers and %
void TUnSpace::setPhaseAssemb( )
{ int i, j, fl, np, k, numPH;

// set array usp->sv Indices of PhA ids in sampled variants
// and nPhA Number of different phase assemblages in all sampled variants
  for(i=0; i<usp->Q; i++ )
    usp->sv[i] = -1;

  for( i=0, usp->nPhA=0; i<usp->Q; i++)
   {
     if( usp->sv[i] < 0 )
     { usp->sv[i] = usp->nPhA;
       for( j=i+1; j<usp->Q; j++)
       { fl=0;
         for( k=0; k<usp->Fi; k++ )
          if( usp->vYF[i*usp->Fi+k] && !usp->vYF[j*usp->Fi+k]  ||
             !usp->vYF[i*usp->Fi+k] && usp->vYF[j*usp->Fi+k]   )
          { fl=1; break; }
         if(!fl)
           usp->sv[j] = usp->nPhA;
       }
       usp->nPhA++;
     }
   }

// alloc memory for nPhA size
  phase_lists_new();

for(np=0; np<usp->nPhA; np++ )
    for( i=0; i<usp->N; i++ )
       usp->PhAndx[np*usp->N+i] = -1;

// set data to arrays
for(np=0; np<usp->nPhA; np++ )
{
   usp->PhNum[np] = 0;
   for( i=0; i<usp->Q; i++)
     if( np == usp->sv[i])
     {
       if( !usp->PhNum[np] )
       {
         gstring lst = "";
         numPH=0;

         for( k=0; k<usp->Fi; k++ )
          if( usp->vYF[i*usp->Fi+k] > 1e-19 )
          {
            usp->PhAndx[np*usp->N+numPH ] = (short)k;
            numPH++;
            lst += gstring(
               TProfil::pm->mup->SF[k]+MAXSYMB+MAXPHSYMB, 0, MAXPHNAME);
            lst.strip();
            lst += ";";
          }
         sprintf( usp->PhAID[np], "GR%2d", np );
         strncpy( usp->PhAlst[np], lst.c_str(), 80 );
       }
       usp->PhNum[np]++;
     }
   usp->PhAfreq[np] = (float)usp->PhNum[np]/usp->Q*100;
  }

// not from 0
   for( k=0; k<usp->Q; k++ )
   {   usp->sv[k]++;
       if( filters( k ) )
          usp->sv[k] *= -1;
   }

}
//====================================================================


