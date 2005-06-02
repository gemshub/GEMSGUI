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
     // set up filters
     for( k=0; k<usp->Q; k++ )
        if( filters( k ) )
            usp->sv[k] *= -1;


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
/*   short i,k,fl,Kgr=1;
   for( i=0; i<usp->Q; i++)
      { fl=0;
        for( k=0; k<usp->Fi; k++)
          if(  usp->vYF[i*usp->Fi+k] &&
              !usp->vYF[j*usp->Fi+k] ||
              !usp->vYF[i*usp->Fi+k] &&
              usp->vYF[j*usp->Fi+k] )
          { fl=1; break; }
         if(!fl) Kgr++;
       }
  Kgr--;
return(Kgr); */
}

int TUnSpace::kolgrup()
// return: kol-vo phase groups
{

return usp->nPhA;
/*
  int i,j,k,Kgr=1,fl,*sv0;

  sv0= new int[usp->Q];

  for( i=0; i<usp->Q; i++)
    sv0[i]=0;

  for(i=0; i< usp->Q; i++ )
  {
    if(!sv0[i])
    {
      sv0[i]=Kgr;
      for( j=i+1; j<usp->Q; j++)
      {
        fl=0;
        for( k=0; k<usp->Fi; k++)
        if( usp->vYF[i*usp->Fi+k] && !usp->vYF[j*usp->Fi+k] ||
           !usp->vYF[i*usp->Fi+k] && usp->vYF[j*usp->Fi+k] )
        { fl=1; break; }
        if(!fl)
           sv0[j]=Kgr;
     }
     Kgr++;
   }
 }
Kgr--;

delete[] sv0;
return(Kgr);  */
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
return(maxkol);
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


int TUnSpace::sel_parg( short *sv ) \
// ???????? must be changed using int TUnSpace::setPhaseAssemb() arrays
{
  int i, j,k,fl,I,J, maxgr=1;

  short Ngr=1;

  for( i=0; i<usp->Q; i++ )
    sv[i] = 0;

   // sv - nomera of paragenesis
   // Ngr - kol-vo paragenesis
   // return:  nomer nabora phases po criteriju

   for( i=0; i<usp->Q; i++)
   {
     if(!sv[i])
     { sv[i] = Ngr;
       for( j=i+1; j<usp->Q; j++)
       { fl=0;
         for( k=0; k<usp->Fi; k++ )
          if( usp->vYF[i*usp->Fi+k] && !usp->vYF[j*usp->Fi+k]  ||
             !usp->vYF[i*usp->Fi+k] && usp->vYF[j*usp->Fi+k]   )
          { fl=1; break; }
         if(!fl) sv[j]=Ngr;
       }
       Ngr++;
     }
   }
 Ngr--;
 // maxgr - max number of paragen. (size of  sv array)

 if( usp->Pa_Crit == UNSP_CRIT_PA ) // select statistic
 {
  J = 0;
  for( j=0; j<usp->Q; j++)
     if(sv[j]==1)
          J++;
  for(i=1; i<Ngr; i++ )
  { I=0;
    for( j=0; j<usp->Q; j++ )
     if(sv[j]==i+1)
        I++;
    if(I>J)
    { maxgr = i+1 ; J=I; }
  }
 }
 if( usp->Pa_Crit == UNSP_CRIT_LAPL_QAN )          // select Laplace_quantile
 {
   J=kvant_parag( 0 /*usp->quanLap*/ );
   for(i=0; i<Ngr; i++)
   { I=0;
     for(j=0; j<usp->Q; j++ )
      if(sv[j]==i+1)
        I++;
     if(I==J)
     { maxgr = i+1; break; }
   }
 }
 if( usp->Pa_Crit ==  UNSP_CRIT_HOME_QAN )       // select Homenuk_quantile
 {
   J=kvant_parag( 3/*usp->quanHom*/ );
   for(i=0; i<Ngr; i++)
   { I=0;
     for(j=0; j<usp->Q; j++)
      if(sv[j]==i+1)
        I++;
     if(I==J)
     { maxgr= i+1; break; }
   }
 }
 if( usp->Pa_Crit == UNSP_CRIT_LAPL_P )               // select Laplace
 {
   maxgr=sv[usp->Lapl];
 }
 if( usp->Pa_Crit == UNSP_CRIT_HOME_P)               // select Homenuk
  {
    maxgr=sv[usp->Homen];
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
/* vstavka for mr.Kulik: proverka  normirovka values pay-off matrix
       if(bp->OF==1)
        R=ePO(usp,bp,t,t);
       if(bp->OF==2)
        R=ePO1(usp,bp,t,t,A);
       fprintf(fcr,"  * %8.5f ** %8.5f ** %8.5f",
           Zcp[t]-R, fabs(Zcp[t])-R, fabs(Zcp[t])-fabs(R));
//  print G(2),G(21) - test
       fprintf(fcr,"  %8.f %8.f",*(usp->vG+t*usp->L+2),*(usp->vG+t*usp->L+21));
//  print N set phases & N p/P
//       fprintf(fcr," %3.d %4.d",pech_parg(usp,t),t+1);
*/
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
{ int i,j;

  double Ca,Ca1,sCa,sCa1,gg1,sg1,quanLapl,quanHur,quanWald,quanHom;
  float st=2.,st1=2.; // coeff. Studenta
  int k,ii,fl,l,ca=0;
  int kvant = usp->quan_lev*usp->Q;

 //  paragen( ); // put data to nPhA, PhAndx, PhAfreq ....

  ii = usp->N;
  if( TProfil::pm->mup->Laq )
      ii--;
  for( l=0; l<ii ; l++ )
  {
   Ca = sCa = Ca1 = sCa1= 0.;
   for( i=0; i<usp->Q; i++ )
     Ca += usp->vMol[i*usp->N+l];
   Ca /= usp->Q;
   for( i=0; i<usp->Q; i++ )
   {

     sCa += (usp->vMol[i*usp->N+l]-Ca) * (usp->vMol[i*usp->N+l]-Ca);
     fl=0;
      for(k=0;k<usp->Fi; k++)                           //  ???????
          if(  usp->vYF[i*usp->Fi+k] && !usp->vYF[k] ||
             !usp->vYF[i*usp->Fi+k]  && usp->vYF[k]   )
          { fl=1; break; }
      if(!fl)
           Ca1 += usp->vMol[i*usp->N+l];
      if(!fl && !l)
            ca++;
   }
    sCa /= (usp->Q-1);
    if(sCa>0)
       sCa = sqrt(sCa);
   if(ca)
     Ca1 /= ca;
   for( i=0; i<usp->Q; i++ )
      { fl=0;
        for( k=0; k<usp->Fi; k++ )
          if( usp->vYF[i*usp->Fi+k] && !(usp->vYF[k]) ||
             !usp->vYF[i*usp->Fi+k] && usp->vYF[k]  )
          { fl=1; break; }
         if(!fl)
            sCa1+=(usp->vMol[i*usp->N+l]-Ca1)*(usp->vMol[i*usp->N+l]-Ca1);
       }
   if( ca > 1 )
     sCa1 /= (ca-1);
   if( sCa1 > 0 )
      sCa1 = sqrt(sCa1);

   j   = kol_in_sol(0);

/*   usp->UnIC[l][0] = Ca1;
   usp->UnIC[l][1] = st1*sCa1 / sqrt(double(ca));
   usp->UnIC[l][2] = Ca;
   usp->UnIC[l][3] = st*sCa / sqrt(double(usp->Q));
   usp->UnIC[l][4] = usp->vMol[l];
   usp->UnIC[l][5] = usp->vMol[usp->Lapl*usp->N+l];
   usp->UnIC[l][6] = usp->vMol[usp->Homen*usp->N+l];
   usp->UnIC[l][7] = (float)j/usp->Q*100.;
   usp->UnIC[l][8] = kolgrup();
   usp->UnIC[l][9] = 0.;

*/ 
   quanLapl=quanHur=quanWald=quanHom=0.;
       for(k=0;k<kvant;k++)
        { quanLapl += usp->vMol[usp->quanCx[k][0]*usp->N+l];
          quanHur +=  usp->vMol[usp->quanCx[k][1]*usp->N+l];
          quanWald += usp->vMol[usp->quanCx[k][2]*usp->N+l];
          quanHom  += usp->vMol[usp->quanCx[k][3]*usp->N+l];
        }
          quanLapl/=kvant;
	  quanHur/=kvant;
          quanWald/=kvant;
          quanHom/=kvant;  
   usp->UnIC[l][0] = usp->vMol[l];
   usp->UnIC[l][1] = Ca;
   usp->UnIC[l][2] = st*sCa / sqrt(double(usp->Q));
   usp->UnIC[l][3] = Ca1;
   usp->UnIC[l][4] = st1*sCa1 / sqrt(double(ca));
   usp->UnIC[l][5] = quanLapl;
   usp->UnIC[l][6] = quanHur;
   usp->UnIC[l][7] = quanWald;
   usp->UnIC[l][8] = quanHom;
   usp->UnIC[l][9] = 0.;

 }


 for( l=0; l<usp->L; l++ )
 {
    gg1=0.; sg1=0.;
    for( i=0; i<usp->Q; i++ )
    { fl=0;
      for( k=0; k<usp->Fi; k++ )
          if( usp->vYF[i*usp->Fi+k] && !usp->vYF[k] ||
             !usp->vYF[i*usp->Fi+k] && usp->vYF[k]  )
          { fl=1; break; }
         if(!fl)
           gg1 += usp->vG[i*usp->L+l];
      }
   gg1 /= ca;
   for(i=0; i<usp->Q; i++)
    { fl=0;
      for(k=0;k<usp->Fi;k++)
          if( usp->vYF[i*usp->Fi+k] && !(usp->vYF[k])||
             !usp->vYF[i*usp->Fi+k] && usp->vYF[k] )
          { fl=1; break; }
         if(!fl)
            sg1 += (usp->vG[i*usp->L+l]-gg1)*(usp->vG[i*usp->L+l]-gg1);
      }
  if(ca>1)
     sg1 /= (ca-1);
  if(sg1>0)
    sg1=sqrt(sg1);

   usp->UgDC[l][0] = gg1;
   if( usp->NgLg[l] > 0 )
      usp->UgDC[l][1] = st1*sg1/sqrt(double(ca));
   else
      usp->UgDC[l][1] = 0.;
   usp->UgDC[l][2] = usp->Gs[l][0]; //usp->vG[l];
   usp->UgDC[l][3] = usp->vG[usp->Lapl*usp->L+l];
   usp->UgDC[l][4] = usp->vG[usp->Homen*usp->L+l];

  }

 for( l=0; l<usp->Ls; l++ )             // UaDC
 {
    gg1=0.; sg1=0.;
    for( i=0; i<usp->Q; i++ )
    { fl=0;
      for( k=0; k<usp->Fi; k++ )
          if(usp->vYF[i*usp->Fi+k] && !usp->vYF[k] ||
             !usp->vYF[i*usp->Fi+k] && usp->vYF[k]  )
          { fl=1; break; }
         if(!fl)
           gg1 += usp->vFug[i*usp->Ls+l];
      }
   gg1 /= ca;
   for(i=0; i<usp->Q; i++)
    { fl=0;
      for(k=0;k<usp->Fi;k++)
          if( usp->vYF[i*usp->Fi+k] && !(usp->vYF[k])||
             !usp->vYF[i*usp->Fi+k] && usp->vYF[k] )
          { fl=1; break; }
         if(!fl)
            sg1 += (usp->vFug[i*usp->Ls+l]-gg1)*(usp->vFug[i*usp->Ls+l]-gg1);
      }
  if(ca>1)
     sg1 /= (ca-1);
  if(sg1>0)
    sg1=sqrt(sg1);

   usp->UaDC[l][0] = gg1;
   usp->UaDC[l][1] = st1*sg1/sqrt(double(ca));
   usp->UaDC[l][2] = usp->vFug[l];
   usp->UaDC[l][3] = usp->vFug[usp->Lapl*usp->Ls+l];
   usp->UaDC[l][4] = usp->vFug[usp->Homen*usp->Ls+l];

   usp->UaDC[l][5] = 0.;
   usp->UaDC[l][6] = 0.;
   usp->UaDC[l][7] = 0.;
   usp->UaDC[l][8] = 0.;
   usp->UaDC[l][9] = 0.;

  }


//=======================================================
     double sr,srx,gu,srGAM,*srU,sRXjw=0.,srXF,/*sto,*/quanLapl,quanHur,quanWald,quanHom;
     int kvant,z;

     kvant = usp->quan_lev*usp->Q;
     if(kvant<1)
         kvant=1;
     srU= new  double[usp->N];

     if(TProfil::pm->mup->Laq )
     {
        for(k=0; k<usp->Q; k++)
          if( usp->sv[k] == Ngr  /* && !filters( k )*/)
           sRXjw += usp->vY[k*usp->L+TProfil::pm->mup->Laq-1];
        if( usp->ob)
         sRXjw/=usp->ob;
     }
     for(j=0; j<usp->N; j++)
     { srU[j]=0.;
       for(k=0;k<usp->Q;k++)
        if( usp->sv[k] == Ngr /*&& !filters( k )*/)
          srU[j] += usp->vU[k*usp->N+j];
       if(usp->ob)
        srU[j] /= usp->ob;
     }

    ii=0;
    for( z=0; z<usp->Fi; z++)
    { srXF=0.;
      for( k=0; k<usp->Q; k++)
        if(usp->sv[k]==Ngr /* && !filters( k )*/)
           srXF += usp->vYF[k*usp->Fi+z];
      if(usp->ob)
          srXF/=usp->ob;
      for( j=ii; j<ii+TProfil::pm->mup->Ll[z]; j++)
      {
        sr=srx=srGAM=0.;
       for(k=0; k<usp->Q; k++ )
        if( usp->sv[k]==Ngr /*&& !filters( k )*/)
        {  sr += usp->vG[k*usp->L+j];
           srx+= usp->vY[k*usp->L+j];
           srGAM += usp->vGam[k*usp->L+j];
        }
       if(usp->ob)
        { sr/=usp->ob;  srx/=usp->ob; srGAM/=usp->ob; }
/*       for( k=0; k<usp->Q; k++)
        if( us->sv[k]==Ngr && !filters(k) )
          sto += (usp->vG[k*usp->L+j]-sr)*(usp->vG[k*usp->L+j]-sr);
       if(usp->ob && sto>0)
       { sto/=usp->ob;
         sto = sqrt(sto);
       }   */
       quanLapl=quanHur=quanWald=quanHom=0.;
       for(k=0;k<kvant;k++)
        { quanLapl +=usp->vG[usp->quanCx[k][0]*usp->L+j];
          quanHur +=usp->vG[usp->quanCx[k][1]*usp->L+j];
          quanWald +=usp->vG[usp->quanCx[k][2]*usp->L+j];
          quanHom  +=usp->vG[usp->quanCx[k][3]*usp->L+j];
        }
          quanLapl/=kvant;
	  quanHur/=kvant;
          quanWald/=kvant;
          quanHom/=kvant;
      if(srx)
       if(j<TProfil::pm->mup->Laq-1)
         gu=g_uw(j,srU,usp->A,srGAM,srx,sRXjw,srXF);
       else
         gu=g_u( j,srU,usp->A,srGAM,srx,srXF);
      else
       gu=0.;

   usp->UgDC[j][5] = sr;
   usp->UgDC[j][6] = quanLapl;
   usp->UgDC[j][7] = quanHom;
   usp->UgDC[j][8] = gu;
   usp->UgDC[j][9] = 0.;
      }
    ii += TProfil::pm->mup->Ll[z];
   }

  delete[] srU;
}


//  correction values G298 and intervals
void TUnSpace::AdapG()
// Iter- iteration of adaptation
{  int i,k, i1;
   double quanLapl,quanHur,quanWald,quanHom;
   int kvant;
   double sr,sto,min,max;
   int Ngr;

 // Ngr - number set phases from criteria
 // sv - numbers set phases in solutions
 // ob - chislo solutions c dannym naborom phases & udovl. filters
     usp->ob=0;
     Ngr = sel_parg( usp->sv );
    // set up filters
     for( k=0; k<usp->Q; k++ )
        if( filters( k ) )
            usp->sv[k] *= -1;

     for(k=0;k<usp->Q;k++)
        if( usp->sv[k] == Ngr /*&& !filters(k)*/)
           usp->ob++;

    for(i=0;i<usp->L;i++)
    {
      i1=0; sto=0.;

      for( k=0; k<usp->Q; k++)
        if( usp->sv[k]==Ngr /*&& !filters(k) */)
          if(!i1)
           {  min = max = sr = usp->vG[k*usp->L+i]; i1++; }
          else
           { if( usp->vG[k*usp->L+i] < min )
              min = usp->vG[k*usp->L+i];
             if( usp->vG[k*usp->L+i] > max )
              max = usp->vG[k*usp->L+i];
             sr += usp->vG[k*usp->L+i];
           }
       if(usp->ob)
        sr/=usp->ob;
       for(k=0; k<usp->Q; k++)
        if( usp->sv[k] == Ngr /*&& !filters(k)*/)
           sto += (usp->vG[k*usp->L+i]-sr)*(usp->vG[k*usp->L+i]-sr);
       if(usp->ob > 1 && sto>0)
       { sto/=(usp->ob);
         sto=sqrt(sto);
       }
       kvant = usp->quan_lev*usp->Q;
       if(kvant<1)
           kvant=1;
       quanLapl=quanHur=quanWald=quanHom=0.;

       for(k=0; k<kvant; k++)
       { quanLapl += usp->vG[usp->quanCx[k][0]*usp->L+i];
	 quanHur += usp->vG[usp->quanCx[k][1]*usp->L+i];
	 quanWald += usp->vG[usp->quanCx[k][2]*usp->L+i];
         quanHom += usp->vG[usp->quanCx[k][3]*usp->L+i];
       }
        quanLapl/=kvant;
	quanHur/=kvant;
	quanWald/=kvant;
        quanHom/=kvant;
 
   switch( usp->Pa_Crit )
   {
    case UNSP_CRIT_PA:         usp->Gs[i][0] = sr;   break;
    case UNSP_CRIT_LAPL_QAN:   usp->Gs[i][0] = quanLapl;   break;
    case UNSP_CRIT_HOME_QAN:   usp->Gs[i][0] = quanHom;     break;
    case UNSP_CRIT_LAPL_P:     usp->Gs[i][0] = usp->vG[usp->Lapl*usp->L+i];
                               break;
    case UNSP_CRIT_HOME_P:     usp->Gs[i][0] = usp->vG[usp->Homen*usp->L+i];
    default:                   break;
   }


      usp->UnDCA[i][0] = usp->vG[i]; 
      usp->UnDCA[i][1] = fabs(usp->IntLg[i][0]);
      usp->UnDCA[i][2] = min;
      usp->UnDCA[i][3] = max;
      usp->UnDCA[i][4] = sr;
      usp->UnDCA[i][5] = 3.*sto/sqrt(double(usp->ob));
      usp->UnDCA[i][6] = 0.;
      usp->UnDCA[i][7] = 0.;

//   ??????????????????
//   adatpter new data to BD
    if( usp->NgLg[i] > 0 )
    {  usp->IntLg[i][0]=3.*sto/sqrt(double(usp->ob));
       TProfil::pm->syp->GEX[i] =
            usp->Gs[i][0]-TProfil::pm->tpp->G[i];
    }
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

}
//====================================================================


