//-------------------------------------------------------------------
// $Id$
//
// Copyright (C) 1992-2000  K.Chudnenko, D.Kulik, S.Dmitrieva
//
// Implementation of TFGLcalc class
//
// This file is part of a GEM-Selektor (GEMS) v.2.x.x program
// environment for thermodynamic modeling in geochemistry
// Uses: GEM-Vizor GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://les.web.psi.ch/Software/GEMS-PSI for more information
// E-mail: gems2.support@psi.ch; chud@igc.irk.ru
//-------------------------------------------------------------------

#include <math.h>

#include "s_tpwork.h"
#include "s_fgl.h"
#include "m_const.h"
//#include "service.h"
//#include "visor.h"

//--------------------------------------------------------------------//
int TCGFcalc::CGcalcFug( void )
{
    double T, P, Fugacity = 0.1, Volume = 0.0, DeltaH=0, DeltaS=0;
    float *Coeff, Eos4parPT[4] = { 0.0, 0.0, 0.0, 0.0 } ;
    int retCode = 0;

    ErrorIf( !aW.twp, "CG EoS", "Undefined twp");

    P = aW.twp->P;    /* P in 10^5 Pa  */
    T = aW.twp->TC+273.15;   /* T in K */

    Coeff = aW.twp->Cemp;     /* pointer to coeffs of CG EOS */

// Calling CG EoS functions here

    if( T >= aW.twp->TClow +273.15 && T < 1e4 && P >= 1. && P < 1e5 )
       retCode = CGFugacityPT( Coeff, Eos4parPT, Fugacity, Volume,
            DeltaH, DeltaS, P, T );
    else {
            Fugacity = P;
            Volume = 8.31451*T/P;
            aW.twp->V = Volume;
            aW.twp->Fug = Fugacity;
            return retCode;
          }

//    if( retCode < 0 )
//    {  //  error - too low pressure
//       Fugacity = P;
//      Volume = 8.31451*T;
//    }

    aW.twp->G += 8.31451 * T * log( Fugacity / P );
    /* add enthalpy and enthropy increments */
    aW.twp->H +=  DeltaH;   // in J/mol - to be completed
    aW.twp->S +=  DeltaS;   // to be completed
    aW.twp->V = Volume /* /10.  in J/bar */;
//    aW.twp->U = ((aW.twp->H/4.184)-RP*fg.VLK*fg.P2)*4.184;
    aW.twp->Fug = Fugacity;   /* fugacity at P */
// For passing corrected EoS coeffs to calculation of fluid
// mixtures
    aW.twp->wtW[6] = Eos4parPT[0];
    aW.twp->wtW[7] = Eos4parPT[1];
    aW.twp->wtW[8] = Eos4parPT[2];
    aW.twp->wtW[9] = Eos4parPT[3];
//
    return retCode;
}

double TCGFcalc::DIntegral(double T, double ro,unsigned IType)
{
  static double TOld,roOld;
  static double a,b,c,d,e;
   static double data[][6]=
      {{-0.257431, 0.439229,  0.414783,  -0.457019, -0.145520,  0.299666},
      {-0.396724, 0.690721,  0.628935,  -0.652622, -0.201462, -0.23163 },
      {-0.488498, 0.863195,  0.761344,  -0.750086, -0.218562, -0.538463},
      {-0.556600, 0.995172,  0.852903,  -0.804710, -0.214736, -0.761700},
      {-0.611295, 1.103390,  0.921359,  -0.838804, -0.197999, -0.940714},
      {-0.657866, 1.196189,  0.975721,  -0.862346, -0.172526, -1.091678},
      {-0.698790, 1.278054,  1.020604,  -0.880027, -0.140749, -1.222733},
      {-0.735855, 1.351533,  1.058986,  -0.894024, -0.104174, -1.338626},
      {-0.769504, 1.418223,  1.092052,  -0.905347, -0.063730, -1.442391},
      {-0.800934, 1.479538,  1.121453,  -0.914864, -0.020150, -1.536070},
      {-0.829779, 1.535822,  1.147161,  -0.922381, 0.026157 , -1.621183},
      {-0.856655, 1.587957,  1.169885,  -0.928269, 0.074849 , -1.698853},
      {-0.881757, 1.636402,  1.190082,  -0.932668, 0.125590 , -1.769898},
      {-0.904998, 1.681421,  1.207610,  -0.935419, 0.178283 , -1.835070},
      {-0.926828, 1.723393,  1.223088,  -0.936667, 0.232649 , -1.894899},
      {-0.946773, 1.762571,  1.236007,  -0.936403, 0.288687 , -1.949858},
      {-0.965248, 1.799170,  1.246887,  -0.934650, 0.346207 , -2.000344}};
  //static double dt12[]=
//      {-2.139734,1.971553, 0.945513, -1.901492,-0.588630,-5.390941};
//      {-0.637684, 0.708107,  0.222086,  -0.481116, -0.332141, -3.492213};

   unsigned n;
   double *dtmp,rez;

  if ( (T!=TOld) || (ro!=roOld) )
  {
    TOld=T;
    roOld=ro;
    e=log(T);
    b=ro*ro;
    d=ro;
    c=ro*e;
    a=b*e;
  }

  // special case
  /*
  if ( IType==12 )
  {
    rez=(dt12[0]*T + dt12[1])*b +
    (dt12[2]*T + dt12[3])*ro + dt12[4]*T + dt12[5];
    return exp(rez);
  }
    */

  n=IType-4;
  dtmp=data[n];
  rez=dtmp[0]*a + dtmp[1]*b + dtmp[2]*c + dtmp[3]*d + dtmp[4]*e + dtmp[5];
  return exp(rez);
}


double TCGFcalc::LIntegral(double T, double ro,unsigned IType)
{
  static double TOld,roOld;
  static double a,b,c,d,e;
  static double data[][6]=
  {{ -1.010391, 1.628552,  2.077476,  -2.30162 , -0.689931, -2.688117},
   { -1.228611, 2.060090,  2.463396,  -2.453303, -0.573894, -3.350638},
   { -1.354004, 2.402034,  2.718124,  -2.462814, -0.412252, -4.018632}};

   double *dtmp,rez;

  if ( (T!=TOld) || (ro!=roOld) )
  {
    TOld=T;
    roOld=ro;
    a=ro*ro*log(T);
    b=ro*ro;
    c=ro*log(T);
    d=ro;
    e=log(T);
  }

  switch ( IType )
  {
    case 662:
         dtmp=data[0];
         break;
    case 1262:
         dtmp=data[1];
         break;
    case 12122:
         dtmp=data[2];
         break;
    default:
         return 0;
  }

  rez=dtmp[0]*a + dtmp[1]*b + dtmp[2]*c + dtmp[3]*d + dtmp[4]*e + dtmp[5];
  return -exp(rez);

}

double TCGFcalc::KIntegral(double T, double ro,long unsigned IType)
{
  static double TOld,roOld;
  static double a,b,c,d,e;
  static double data[][6]=
  {{ -1.050534, 1.747476,  1.749366,  -1.999227, -0.661046, -3.028720},
   { -1.309550, 2.249120,  2.135877,  -2.278530, -0.773166, -3.704690},
   { -1.490116, 2.619997,  2.404319,  -2.420706, -0.829466, -3.930928},
   { -1.616385, 2.881007,  2.577600,  -2.484990, -0.828596, -4.175589},
   { -1.940503, 3.552034,  2.940925,  -2.593808, -0.724353, -4.899975}};

   double *dtmp,rez;

  if ( (T!=TOld) || (ro!=roOld) )
  {
    TOld=T;
    roOld=ro;
    a=ro*ro*log(T);
    b=ro*ro;
    c=ro*log(T);
    d=ro;
    e=log(T);
  }

  switch ( IType )
  {
    case 222333:
         dtmp=data[0];
         break;
    case 233344:
         dtmp=data[1];
         break;
    case 334445:
         dtmp=data[2];
   rez=dtmp[0]*a + dtmp[1]*b + dtmp[2]*c + dtmp[3]*d + dtmp[4]*e + dtmp[5];
         return -exp(rez);
    case 444555:
         dtmp=data[3];
         break;
    case 666777:
         dtmp=data[4];
         break;
    default:
         return 0;

  }

   rez=dtmp[0]*a + dtmp[1]*b + dtmp[2]*c + dtmp[3]*d + dtmp[4]*e + dtmp[5];
   return exp(rez);
}


double TCGFcalc::K23_13(double T, double ro)
{
  static double TOld,roOld,KOLD;
  static double a,b,c,d,e;
  static double dtmp[]=
  { -1.050534, 1.747476,  1.749366,  -1.999227, -0.661046, -3.028720};

  if ( (T!=TOld) || (ro!=roOld) )
  {
    TOld=T;
    roOld=ro;
    a=ro*ro*log(T);
    b=ro*ro;
    c=ro*log(T);
    d=ro;
    e=log(T);
  }
  else return KOLD;

   KOLD=dtmp[0]*a + dtmp[1]*b + dtmp[2]*c + dtmp[3]*d + dtmp[4]*e + dtmp[5];
   KOLD=exp(KOLD/3.);
   return KOLD;

  }

/////////////////////////////////////////////////////////////////////////////
// Implementation of TCGFcalc class

int TCGFcalc::CGFugacityPT( float *EoSparam, float *EoSparPT, double &Fugacity,
        double &Volume, double &DeltaH, double &DeltaS, double P, double T )
{
      int iRet; double ro;
      double X[1]={1};
      double FugPure[1];

      switch (int(EoSparam[4]))
      {
       case 0:
        EoSparPT[0]=EoSparam[0];
        EoSparPT[1]=EoSparam[1];
        EoSparPT[2]=EoSparam[2];
        EoSparPT[3]=EoSparam[3];
       break;
       case 1:  // H2O type
        EoSparPT[0]=EoSparam[0]+EoSparam[5]/(T+EoSparam[6]);
        EoSparPT[1]=EoSparam[1]+EoSparam[7]/(T+EoSparam[8]);
        EoSparPT[2]=EoSparam[2]+EoSparam[9]/(T+EoSparam[10]);
        EoSparPT[3]=EoSparam[3]+EoSparam[11]/(T+EoSparam[12]);
        break;
       case 2:  // CO2 type
        EoSparPT[0]=EoSparam[0]+EoSparam[5]*exp(T*EoSparam[6]);
        EoSparPT[1]=EoSparam[1]+EoSparam[7]*exp(T*EoSparam[8]);
        EoSparPT[2]=EoSparam[2]+EoSparam[9]*exp(T*EoSparam[10]);
        EoSparPT[3]=EoSparam[3]+EoSparam[11]*exp(T*EoSparam[12]);
        break;
        default:

        return 1;// Error: Wrong type of equation
      };

      iRet = CGActivCoefPT( X, EoSparPT, FugPure, 1, P, T );
      if( iRet  )
      {
          return iRet;
      };
      Fugacity= FugPure[0];
      ro = DENSITY( X, EoSparPT, 1, P, T );
      if( ro < 0 )
      {  // error - density could not be calculated
         iRet = -2; ro = 1.0;
      }
      Volume=0.1/ro;  // in J/bar
//
      DeltaH=0.;  // To be completed
      DeltaS=0.;  // To be completed
//
      return iRet;
  }


   int TCGFcalc::CGActivCoefPT(double *X,float *param,double *act, unsigned NN,
        double Pbar, double T )
   {
      //double act[MAXPARAM];
      //unsigned ncmp;
       double *xtmp,*Fx;
       double P=Pbar/10.;
      //ncmp=unsigned((nn-2)/5);
      try
      {
        xtmp=new double [NN];
          Fx=new double [NN];
      }
      catch(xalloc)
      {
        printf("Can't allocate memory\n");
        exit(1);
      }

      EOSPARAM paar(X,param,NN);
      double F0,Z,F1,fideal;
      //double e[4],s3[4],m,a,xnonp;
      double ro,delta=DELTA,ax,dx /*,tmp*/;
      /* unsigned */ int i;

       norm(paar.XX0,paar.NCmp());
       copy(paar.XX0,xtmp,paar.NCmp());

        paar.ParamMix(xtmp);

        ro=ROTOTALMIX(P,T,paar);
if( ro < 0.0 ) //  Too low pressure - no corrections will be done
  return ( -1 );
        Z=P/(R*T*ro);
        F0=FTOTALMIX(T,ro,paar);


  //       fideal=log(R*T*ro/BARMPA);
         fideal=log(R*T*ro/0.1);
         ax= Z - 1.+fideal;

       for ( i=0;i<paar.NCmp();i++)
       {
         if ( xtmp[i]>0. )
         {
          copy(paar.XX0,xtmp,paar.NCmp());
          dx=xtmp[i]*delta;
          xtmp[i]+=dx;
          norm(xtmp,paar.NCmp());

          paar.ParamMix(xtmp);
          F1=FTOTALMIX(T,ro,paar)*(1.+dx);

          Fx[i]=(F1-F0)/(dx);
         }
         else Fx[i]=0.;
       };

      // GMix=0.;
       for ( i=0;i<paar.NCmp();i++)
       {
         if ( xtmp[i]>0. && Fx[i]< 100. )
         {
   //       tmp=log(paar.XX0[i]);
      //    GMix+=tmp*paar.XX0[i];
          act[i] = exp(ax+Fx[i]);
         }
        else
         {
          act[i]=0.;
         }
       };
    //   GMix+=F0 + ax;

         //MLPutRealList(stdlink,act,paar.NCmp());
        delete [] xtmp;
        delete [] Fx;;

         return 0;
   };


   //void ACTDENS(double *data,long nn, double *act )
   int TCGFcalc::CGActivCoefRhoT(double *X,float *param,double *act, unsigned NN,
       double ro, double T )
   {

      //double  act[MAXPARAM];
     // unsigned ncmp;
      //ncmp=unsigned((nn-2)/5);

      //double  T = data[nn - 2];
      //double  ro = data[nn - 1];

       double *Fx,*xtmp;
      try
      {
        xtmp=new double [NN];
        Fx=new double [NN];
      }
      catch(xalloc)
      {
        printf("Cannot allocate memory\n");
        exit(1);
      }
      EOSPARAM paar(X,param,NN);

       double   F0,Z,F1,GMix,fideal;
      double delta=DELTA,ax,dx,tmp;
      int i;

       norm(paar.XX0,paar.NCmp());
       copy(paar.XX0,xtmp,paar.NCmp());

        paar.ParamMix(xtmp);
        Z=ZTOTALMIX(T,ro,paar);

        F0=FTOTALMIX(T,ro,paar);
         fideal=log(R*T*ro/0.1);
         ax= Z - 1.+fideal;

       for ( i=0;i<paar.NCmp();i++)
       {
         if ( xtmp[i]>0. )
         {
          copy(paar.XX0,xtmp,NN);
          if ( xtmp[i]>DELTAMOLLIM )
          {
            dx=xtmp[i]*delta;
          }
          else
          {
            dx=DELTAMOLLIM*delta;
          }

          xtmp[i]+=dx;
          norm(xtmp,paar.NCmp());

          paar.ParamMix(xtmp);
          F1=FTOTALMIX(T,ro,paar)*(1.+dx);

          Fx[i]=(F1-F0)/(dx);
         }
         else Fx[i]=0.;
       };

       GMix=0.;
       for ( i=0;i<paar.NCmp();i++)
       {
         if ( xtmp[i]>0. )
         {
          tmp=log(paar.XX0[i]);
          GMix+=tmp*paar.XX0[i];
          act[i] = exp(ax+Fx[i]);
         }
        else
         {
          act[i]=0.;
         }
       };

        delete [] xtmp;
        delete [] Fx;
        return 0;

    //   MLPutRealList(stdlink,act,paar.NCmp());
   };

   double TCGFcalc::DENSITY(double *X,float *param, unsigned NN ,double Pbar, double T )
   {
      double P = Pbar * 0.1;
      //unsigned ncmp;
      //ncmp=unsigned((nn-2)/5);

     // double  P = data[nn - 2];
     // double  T = data[nn - 1];

       double *xtmp;
      try
      {
        xtmp=new double [NN];
      }
      catch(xalloc)
      {
        printf("Can't allocate memory\n");
        exit(1);
      }
      EOSPARAM paar(X,param,NN);

      double ro;

       norm(paar.XX0,paar.NCmp());
       copy(paar.XX0,xtmp,paar.NCmp());

        paar.ParamMix(xtmp);
        ro=ROTOTALMIX(P,T,paar);
        delete [] xtmp;
    if( ro < 0. )
       printf(" Error - density cannot be found at this T,P" );
        return ro;
   };

   double TCGFcalc::PRESSURE(double *X,float *param,unsigned NN,double ro, double T)
   {
   //   unsigned ncmp;
   //   ncmp=unsigned((nn-2)/5);

   //   double  T = data[nn - 2];
   //   double ro = data[nn - 1];


      double *xtmp;
      try
      {
        xtmp=new double [NN];
      }
      catch(xalloc)
      {
        printf("Can't allocate memory\n");
        exit(1);
      }
      EOSPARAM paar(X,param,NN);

       norm(paar.XX0,paar.NCmp());
       copy(paar.XX0,xtmp,paar.NCmp());

        paar.ParamMix(xtmp);
        double P=PTOTALMIX(T,ro,paar);
        delete [] xtmp;
        return P*10.;
   };


//------------------------------------------------------------ private



void TCGFcalc::copy(double* sours,double *dest,unsigned num)
 {
  unsigned i;
       for ( i=0;i<num;i++)
       {
        dest[i]=sours[i];
       };
 }

void TCGFcalc::norm(double *X,unsigned mNum)
 {
  double tmp=0.;
  unsigned i;
  for ( i=0;i<mNum;i++ )
  {
    tmp+=X[i];
  }
  tmp=1./tmp;
  for ( i=0;i<mNum;i++ )
  {
    X[i]*=tmp;
  }
 }

double TCGFcalc::RPA(double beta,double nuw)
{
  double fi1,fi2;
 fi1=(1.20110+(0.064890+(-76.860+(562.686+(-2280.090+(6266.840+(-11753.40+(14053.8+(-9491.490 +2731.030*nuw)*nuw)*nuw)*nuw)*nuw)*nuw)*nuw)*nuw)*nuw)*nuw;
 fi2=(0.588890+(-7.455360+(40.57590+(-104.8970+(60.25470+(390.6310+(-1193.080+(1576.350+(-1045.910+283.7580*nuw)*nuw)*nuw)*nuw)*nuw)*nuw)*nuw)*nuw)*nuw)*nuw*nuw;
 return  (-12.*fi1 + 192.*fi2*beta)*beta*beta/PI;
}


double TCGFcalc::dHS(double beta,double ro )
{
/// service constants
   double DV112=1./12.;
   double DV712=7./12.;
/// local variables
   double T12,T112,T712,B13,
          dB,delta,d;
   double a0,a1,a6,a3,a4,a7,a9,a12;
   double p0,p2,p6,p3,p5,p8,p11;
   double dbdl,ri6ro,ri6ro2,d3,d2,dnew,F0,F1;
   unsigned i;

   T12=sqrt(beta);
   T112=exp(DV112*log(beta));
   T712=exp(DV712*log(beta));

   B13=(1+beta);
   B13=B13*B13*B13;

   dB= (P1*T112+PP2*T712+(P3+(P4+P5*beta)*beta)*beta)/B13;
   delta=(P6+P7*T12)/(1.+(P8+(P9+P10*T12)*T12)*T12);

   dbdl=dB*delta;
   ri6ro=PISIX*ro;
   ri6ro2=ri6ro*ri6ro;

   a0=dB+dbdl;
   a1=-1.;
   a3= (-1.5*dB -3.75*dbdl)*ri6ro;
   a4= (1.5*ri6ro);
   a6= (2.*dB + dbdl)*0.25*ri6ro2;
   a7= -0.5*ri6ro2;
   a9= -2.89325*ri6ro2*ri6ro*dbdl;
   a12= -0.755*ri6ro2*ri6ro2*dbdl;

   p0  = -1.;
   p2  = a3*3.;
   p3  = a4*4.;
   p5  = a6*6.;
   p6  = a7*7.;
   p8  = a9*9.;
   p11 = a12*12.;

   d=dB;
   i=0;
   while ( i++<21 )
   {
      d2=d*d;
      d3=d*d*d;

      F0= a0+(a1+(a3+(a4+(a6+(a7+(a9+a12*d3)*d2)*d)*d2)*d)*d2)*d;
      F1= p0+(p2+(p3+(p5+(p6+(p8+p11*d3)*d2)*d)*d2)*d)*d2;

      dnew=d-F0/F1;
      if ( fabs(dnew-d)<1.E-7 )
      {
        return dnew;
      }
      d=dnew;
   }

     if ( i>=20 )
     {
      return dB;
     }

   return dnew;

};


double TCGFcalc::FWCA(double T,double ro)
{
  static double TOld,roOld,F;
  double d,beta,nu,nuw;
  double nu1w1,nu1w2,nu1w3,nu1w4,nu1w5;
  double a0,a1,a2,a3;
  double I2;
  double I1_6,I1_12;
  double dW,dW12,dW6;
  double tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7;
  double F0,F1,FA;
  double rm,rmdw1,rmdw2,rmdw3,rmdw4,rmdw5;


  if ((T==TOld) && (ro==roOld))
  {
    return F;
  }
  else
  {
   TOld=T;
   roOld=ro;
  }

      rm=TWOPOW1SIX;

  beta=1./T;

  d=dHS( beta, ro );

  tmp2=PISIX*d*d*d;
  nu=tmp2*ro;
  tmp1=(1. - nu/16.);
  nuw=nu*tmp1;
  dW=d*exp(1./3.*log(tmp1));


  nu1w1=(1.-nuw);
  nu1w2=nu1w1*nu1w1;
  nu1w3=nu1w2*nu1w1;
  nu1w4=nu1w2*nu1w2;
  nu1w5=nu1w2*nu1w3;

  tmp1=(1-nu);
  tmp1=tmp1*tmp1;
  F0= ((4.-3.*nu)*nu)/tmp1;

  a0=fa0( nuw , nu1w2);
  a1=fa1( nuw , nu1w3);
  a2=fa2( nuw , nu1w4);
  a3=fa3( nuw , nu1w5);

  I1_6 =fI1_6( nuw );
  I1_12=fI1_12( nuw );

  rmdw1=rm/dW;
  rmdw2=rmdw1*rmdw1;
  rmdw3=rmdw1*rmdw2;
  rmdw4=rmdw2*rmdw2;
  rmdw5=rmdw3*rmdw2;

  dW6=dW*dW*dW;
  dW6=1./(dW6*dW6);
  dW12=dW6*dW6;

  tmp1=(a0/4.+ a1/12. + a2/24. + a3/24.)*dW6;
  tmp2=(a0/10.+ a1/90. + a2/720. + a3/5040.)*(-dW12);
  tmp3=(a0 - a1/3. + a2/12 - a3/60)/8.;
  tmp4=(a0 - a1 + a2/2. - a3/6.)*rmdw2*(-9.)/40.;
  tmp5=(a1 - a2 + a3/2)*rmdw3*(-2.)/9.;
  tmp6=(a2 - a3)*rmdw4*(-9.)/64.;
  tmp7=a3*(-3.)/35.*rmdw5;

  I2 = tmp1+tmp2+tmp3+tmp4+tmp5+tmp6+tmp7;

  F1=48.*nuw*(I1_12*dW12-I1_6*dW6 + I2)*beta;
  FA=RPA(beta,nuw);


  F=F0+F1+FA;

 return F;
}

 double TCGFcalc::ZWCANum(double T,double ro)
 {
  double delta=DELTA;
  double a0,a1;
  a1=FWCA(T,ro*(1.+delta));
  a0=FWCA(T,ro);
  return 1.+(a1-a0)/delta;
 }

 double TCGFcalc::UWCANum(double T,double ro)
 {
  double delta=DELTA;
  double a0,a1,beta0,beta1;
  beta0=1./T;
  beta1=beta0*(1.+delta);
  a1=FWCA(1./beta1,ro);
  a0=FWCA(T,ro);
  return (a1-a0)/(beta1-beta0);
 }

 double TCGFcalc::FDipPair(double T,double ro,double m2)
 {
  double kappa,Z,U,beta,F;
   kappa=m2*m2/(24.*T);
   beta=1./T;
   Z=ZWCANum(T,ro);
   U=UWCANum(T,ro);
   F=kappa*(4.*beta*U-Z+1.);
   return F;
 }

 double TCGFcalc::J6LJ(double T,double ro)
 {
  double kappa,Z,U,beta,F;
   beta=1./T;
   Z=ZWCANum(T,ro);
   kappa=-16.*PI*ro*beta;
   U=UWCANum(T,ro);
   F=(4.*beta*U-Z+1.)/kappa;
   return F;
 }

 double TCGFcalc::FTOTALMIX(double T_Real,double ro_Real,EOSPARAM& param)
  {
    double FF,A0,A2,A3,AP,A1;
    //unsigned iall,inopol;
    double emix,s3mix,rotmp,T2R;
    double Jind,Jdp;
    int /*itmp,jtmp,ktmp,*/ i,j,k;
    double s3tmp,mtmp,IK /*,atmp*/;
    double imtmp,jmtmp,iatmp,jatmp;
    double m2i,m2j,m2k;
    double s3tmpij,s3tmpik,s3tmpjk;
    double IKtmpij,IKtmpik,IKtmpjk;

    //iall=param.inonzero();
    //inopol=param.inonpolar();
    emix=param.EMIX();
    s3mix=param.S3MIX();

      rotmp=NA*ro_Real;
      T2R=T_Real*T_Real;

      A0=FWCA(T_Real/emix,s3mix*rotmp);


     // if ( inopol< iall )
      {
        /// dipole part
        A2=0.;
        for ( i=0;i<param.NCmp()-1;i++ )
        {
          //itmp=param.ind(i);
          for ( j=i+1; j<param.NCmp(); j++ )
          {
           // jtmp=param.ind(j);

            s3tmp=param.MIXS3(i,j);
            Jdp=J6LJ(T_Real*s3tmp/param.MIXES3(i,j) , s3tmp*rotmp);
            A2+=param.M2R(i)*param.M2R(j)*Jdp*
                           param.X(i)*param.X(j)/s3tmp;
          }
        }
          A2*=2.;
          for ( i=0; i<param.NCmp(); i++ )
          {
            //itmp=param.ind(i);
            mtmp=param.M2R(i);
            s3tmp=param.SIG3(i);
            Jdp=J6LJ(T_Real/param.EPS(i),s3tmp*rotmp);
            A2+=mtmp*mtmp*Jdp*param.X(i)*param.X(i)/s3tmp;
          }

         A2=-A2*TWOPI*rotmp/(3.*T2R);
         ///// A2 done ////

         if ( A2!=0. )
         {

          A3=0.;
          for ( i=0; i<param.NCmp(); i++ )
          {
            //itmp=param.ind(i);
            m2i=param.M2R(i);

            for ( j=0; j<param.NCmp(); j++  )
            {
             // jtmp=param.ind(j);
              m2j=param.M2R(j);

              s3tmpij=param.MIXS3(i,j);
              IKtmpij=K23_13(T_Real*s3tmpij/param.MIXES3(i,j),
                                                    s3tmpij*rotmp);
              for ( k=0; k<param.NCmp(); k++  )
              {
               //ktmp=param.ind(k);
               m2k=param.M2R(k);

               s3tmpik=param.MIXS3(i,k);
               s3tmpjk=param.MIXS3(j,k);

               IKtmpik=K23_13(T_Real*s3tmpik/param.MIXES3(i,k),
                                                    s3tmpik*rotmp);
               IKtmpjk=K23_13(T_Real*s3tmpjk/param.MIXES3(j,k),
                                                    s3tmpjk*rotmp);

               IK=IKtmpij*IKtmpik*IKtmpjk;
               A3+= m2i*m2j*m2k*IK*pow(s3tmpij*s3tmpik*s3tmpjk,-1./3.)*
               param.X(i)*param.X(j)*param.X(k);
              }
            }
          }
            A3=A3*32.*sqrt(14.*PI/5.)*
                  rotmp*rotmp*PI*PI*PI/(135.*T_Real*T2R);
            AP= A2/(1. - A3/A2);
         }
         else AP=0.;

        /// induced interaction
        A1=0.;
        for ( i=0;i<param.NCmp();i++ )
        {
         // itmp=param.ind(i);
          iatmp=param.A(i);
          imtmp=param.M2R(i);
          for ( j=0;j<param.NCmp();j++ )
          {
            //jtmp=param.ind(j);
            jatmp=param.A(j);
            jmtmp=param.M2R(j);

            s3tmp=param.MIXS3(i,j);
            Jind=J6LJ(T_Real*s3tmp/param.MIXES3(i,j),s3tmp*rotmp);

           A1+= (iatmp*jmtmp + jatmp*imtmp)
                  *Jind*param.X(i)*param.X(j)/s3tmp;
          }
        }
        A1=-A1*TWOPI*rotmp/T_Real;
//        A1=-A1*FOURPI*rotmp/T_Real;
      //  A1=0.;
        ///
      }/// end of polar contribution

     FF=A0 + A1 + AP;
     //printf("%g %g %g %g %g",A0,A1,A2,A3,AP);
     //exit(1) ;

    return FF;
  }


double TCGFcalc::UTOTALMIX(double T_Real,double ro_Real,EOSPARAM& param)
{
  double T /*,ro,s3 */;
  double delta=DELTA;
  double a0,a1,beta0,beta1,eps;
  eps=param.EMIX();
  T=T_Real/eps;

  beta0=1./T;
  beta1=beta0*(1.+delta);
  a1=FTOTALMIX((1./beta1)*eps,ro_Real,param);
  a0=FTOTALMIX(T_Real,ro_Real,param);
  return (a1-a0)/(beta1-beta0);
 }


 double TCGFcalc::ZTOTALMIX(double T_Real,double ro_Real,EOSPARAM& param)
 {
  double delta=DELTA;
  double a0,a1;
  a1=FTOTALMIX(T_Real,ro_Real*(1.+delta),param);
  a0=FTOTALMIX(T_Real,ro_Real,param);

  return 1.+(a1-a0)/delta;
 }


 double TCGFcalc::PTOTALMIX(double T_Real,double ro_Real,EOSPARAM& param)
 {
  double Z;
    Z = ZTOTALMIX(T_Real,ro_Real,param);
    return Z*R*T_Real*ro_Real;
 }


 /*  melting density  */
 double Melt(double T)
 {

  return T*0.+.9;

 };

 double Melt2(double T)
 {
  return T*0.+3.;

 };

 #define FIRSTSEED (15)
 #define ROMIN (1.E-2)
 #define NPOINT (5)

 void  choose(double *pres, double P,unsigned &x1,unsigned &x2)
 {
  unsigned i;
  double deltam=-10000000.,tmp;
  double deltap=10000000.;

  for ( i=0; i<NPOINT;i++ )
  {
    tmp=P-pres[i];

    if ( tmp>0. )
    {
       if ( tmp<deltap )
       {
        deltap=tmp;
        x1=i;
       }
    }
    else
    {
       if ( tmp>deltam )
       {
        deltam=tmp;
        x2=i;
       }
    }
  }
     return ;

 }

/////////////////////////////////////////////////////////////////
 double TCGFcalc::ROTOTALMIX(double P,double TT,EOSPARAM& param)
 {
     unsigned i;
     double T /*,ro*/;
     double fact, fact0, romax, dro, roarr[FIRSTSEED];
     double Ptmp[FIRSTSEED], ro0, ro1, rotest, PP0, PP1 /* ,Ptest */;
     double a,b;
     double inttofloat;
     double f[4],x[4],ff,dens[5],pres[5];
     unsigned x1,x2;
//     double ptmp;

     T=TT/param.EMIX();
     fact0=1./(param.S3MIX()*NA);
     fact=R*TT*fact0;

     romax=Melt(T);
     inttofloat=FIRSTSEED-1;
     dro=(romax-ROMIN)/inttofloat;
     roarr[0]=ROMIN;
     roarr[1]=2.*ROMIN;

     for ( i=2;i<FIRSTSEED;i++)
     {
       inttofloat=i;
       roarr[i]=ROMIN+inttofloat*dro;
     }

     for ( i=0;i<FIRSTSEED;i++)
     {
      Ptmp[i]=ZTOTALMIX(TT,roarr[i]*fact0,param);
      Ptmp[i] *= roarr[i] * fact;
      if ( Ptmp[i] > P )
      {
        break;
      }
     }

     if ( i==0 )// Uses aproximation of ideal gas
     {
            return P/(R*TT);
     }


     // additional high pressure inteval
     if ( i==FIRSTSEED )
     {

     //roarr[0]=romax-0.0001;
     roarr[0]=roarr[FIRSTSEED-1];
     Ptmp[0]=Ptmp[FIRSTSEED-1];

     romax=Melt2(T);
     inttofloat=FIRSTSEED-1;
     dro=(romax-ROMIN)/inttofloat;
     for ( i=1;i<FIRSTSEED;i++)
     {
       inttofloat=i;
       roarr[i]=ROMIN+inttofloat*dro;
     }

     for ( i=1;i<FIRSTSEED;i++)
     {
      Ptmp[i]=ZTOTALMIX(TT,roarr[i]*fact0,param)*roarr[i]*fact;
      if ( Ptmp[i]>P )
      {
        break;
      }
     }

     if ( i==FIRSTSEED || i==0 )
     {
         printf("Input pressure is too high!\n");
//         exit(1);
         return (-1.0);
     }
     }

     ro0=roarr[i-1];
     ro1=roarr[i];
     PP0=Ptmp[i-1];
     PP1=Ptmp[i];
     i=0;

   while ( i++<20 )
   {
     //Start interp
     ff = ro0;
     dens[0]=ro0;
     dens[1]=ro1;
     pres[0]=PP0;
     pres[1]=PP1;
     //first order
     x[0]=P-pres[0];
     f[0]=(dens[1]-dens[0])/(pres[1]-pres[0]);
     ff+=f[0]*x[0];

     //second order
     dens[2]=ff;
     pres[2]=ZTOTALMIX(TT,ff*fact0,param)*ff*fact;

     if ( fabs(pres[2]-P)<1E-5 )
     {
       return ff*fact0;
     }

     x[1]=x[0]*(P-pres[1]);
     f[1]=(dens[2]-dens[1])/(pres[2]-pres[1]);

     f[0]=(f[1]-f[0])/(pres[2]-pres[0]);
     ff+=f[0]*x[1];

     //third order
     dens[3]=ff;
     pres[3]=ZTOTALMIX(TT,ff*fact0,param)*ff*fact;
     if ( fabs(pres[3]-P)<1E-6 )
     {
      return ff*fact0;
     }
     x[2]=x[1]*(P-pres[2]);
     f[2]=(dens[3]-dens[2])/(pres[3]-pres[2]);
     f[1]=(f[2]-f[1])/(pres[3]-pres[1]);
     f[0]=(f[1]-f[0])/(pres[3]-pres[0]);
     ff+=f[0]*x[2];
     dens[4]=ff;
     pres[4]=ZTOTALMIX(TT,ff*fact0,param)*ff*fact;
     if ( fabs(pres[4]-P)<1e-6 )
     {
      return ff*fact0;
     }

     choose(pres,P,x1,x2);

     ro0=dens[x1];
     ro1=dens[x2];
     PP0=pres[x1];
     PP1=pres[x2];

      if ( fabs((ro1-ro0))<0.001 )
      {
          a=(PP1-PP0)/(ro1-ro0);
          b=PP1-a*ro1;
          rotest=(P-b)/a;
          return rotest*(fact0);
      }
   }
        //return 10.;
         /// bad result

          a=(PP1-PP0)/(ro1-ro0);
          b=PP1-a*ro1;
          rotest=(P-b)/a;
          return rotest*(fact0);

 }


// Implementation of EOSPARAM class

EOSPARAM::~EOSPARAM()
{
  unsigned  i;
  if ( isize > 0)
  {
         for ( i=0;i<isize;i++ )
         {
           delete [] mixpar[i];
         }

   delete [] epspar;
   delete [] sig3par;
   delete [] XX;
   delete [] eps;
   delete [] eps05;
   delete [] sigpar;
   delete [] mpar;
   delete [] apar;
   delete [] aredpar;
   delete [] m2par;
   delete [] XX0;

   delete [] mixpar;

  }
}


void EOSPARAM::allocate(unsigned inew)
{
   unsigned i;
  if ( (isize > 0) && (inew > isize) )
  {
         for ( i=0;i<isize;i++ )
         {
           delete [] mixpar[i];
         }

   delete [] epspar;
   delete [] sig3par;
   delete [] XX;
   delete [] eps;
   delete [] eps05;
   delete [] sigpar;
   delete [] mpar;
   delete [] apar;
   delete [] aredpar;
   delete [] m2par;
   delete [] XX0;

   delete [] mixpar;

  }

  if ( (inew > isize) )
  {

    try{
           mixpar=new   double* [inew];
         for ( i=0;i<inew;i++ )
         {
           mixpar[i]=new   double [inew];
         }

   epspar =new double [inew];
   sig3par=new double [inew];
   XX     =new double [inew];
   eps    =new double [inew];
   eps05  =new double [inew];
   sigpar =new double [inew];
   mpar   =new double [inew];
   apar   =new double [inew];
   aredpar=new double [inew];
   m2par  =new double [inew];
   XX0    =new double [inew];
  }


  catch (xalloc)
  {
    printf("Can't allocate mamory\n");
    exit(1);
  }

  isize=inew;
  }
}

unsigned EOSPARAM::ParamMix(double *Xin)
  {
    /* unsigned */ int j,i;
    double tmp,tmp1,tmp2;
    for ( i=0; i<NComp; i++ ) XX[i]=Xin[i];

    emix=0.;
    s3mix=0.;

    for ( i=0;i<NComp-1;i++ )
    {
      for ( j=i+1;j<NComp;j++ )
      {
          tmp=XX[i]*XX[j];
          tmp2=mixpar[j][i]; //eps
          tmp1=mixpar[i][j]; //signa
          s3mix+= tmp1*tmp;
          emix+= tmp2*tmp;
      }
    }

    s3mix*=2.;
    emix*=2.;

    for ( i=0;i<NComp;i++ )
    {
          tmp=XX[i]*XX[i];

          s3mix+= sig3par[i]*tmp;
          emix +=  epspar[i]*tmp;
    }
    emix=emix/s3mix;
    return NComp;
  }

  void EOSPARAM::PureParam(double* e,double* s,double* m,double* a)
  {
    /* unsigned */ int i;

    for ( i=0;i<NComp;i++ )
    {
      e[i]=eps[i];
      s[i]=sigpar[i];
      m[i]=mpar[i];
      a[i]=aredpar[i];
    }

  }

 ////////////////////////////////////////////////////////////////////
void EOSPARAM::init(double *Xinp, float * data,unsigned ncmp)
{
  int i,j;
  double tmp;

  //if ( ncmp>MAXPARAM ) NComp=MAXPARAM;
   allocate(ncmp);
   NComp=(int)ncmp;

  for ( i=0;i<NComp;i++ )
  {
       XX0[i] = Xinp[i];

    sigpar[i] = data[i*4    ];
       eps[i] = data[i*4 + 1];
      mpar[i] = data[i*4 + 2];
      apar[i] = data[i*4 + 3];

  }

    for ( i=0;i<NComp;i++ )
    {
      tmp=sigpar[i];
      tmp=tmp*tmp*tmp;
      sig3par[i]=tmp;
      eps05[i]=sqrt(eps[i]);
      epspar[i]=tmp*eps[i];
      m2par[i]=mpar[i]*mpar[i]/(1.38048E-4);
      aredpar[i]=apar[i]/tmp;
    }

  /// calculation of mixing properties ///
    for ( i=0;i<NComp-1;i++ )
    {
     for ( j=i+1;j<NComp;j++ )
     {
       tmp=(sigpar[i]+sigpar[j])*0.5;
       tmp=tmp*tmp*tmp;
       mixpar[i][j]=tmp;
       mixpar[j][i]=tmp*eps05[i]*eps05[j];
     }
    }
};

/////////////////////////////////////////////////////////////////////

void EOSPARAM::copy(double* sours,double *dest,unsigned num)
 {
  unsigned i;
       for ( i=0;i<num;i++)
       {
        dest[i]=sours[i];
       };
 }

void EOSPARAM::norm(double *X,unsigned mNum)
 {
  double tmp=0.;
  unsigned i;
  for ( i=0;i<mNum;i++ )
  {
    tmp+=X[i];
  }
  tmp=1./tmp;
  for ( i=0;i<mNum;i++ )
  {
    X[i]*=tmp;
  }
 }


/////////////////////////////////////////////////////////////////////////////


const float RG = 83.144;

//--------------------------------------------------------------------//
void TFGLcalc::calc_FGL( void )
{
    float /*RP=.023901,*/ /*RT=1.98719,*/ PH,PB;
    int PGL=0;

    memset( &fg, 0, sizeof(FGWORK));
    memset( &fc, 0, sizeof(FGCONST));
    ErrorIf( !aW.twp, "FGL", "Undefined twp");

    fg.Z = 1;    /* calc all */
    fg.P2 = aW.twp->P;    /* P 10^5 Pa  i */
    PB = aW.twp->P;
    fg.MM = aW.twp->mwt;        /* molar massa  */
    fg.TK = aW.twp->TC+273.15;   /* T  K i */
    /* Critical parametres of gas i */
    fg.KT = aW.twp->CPg[0];     /* T  K */
    fg.KP = aW.twp->CPg[1];     /* P bar */
    fg.KV = aW.twp->CPg[2]*10;  /* V cm3 */
    fg.KZ = aW.twp->CPg[3];     /* compressions */
    fg.WW = aW.twp->CPg[4];     /* factor acentrichnosti Pitcera  */
    if( IsFloatEmpty( fg.KT )||IsFloatEmpty( fg.KP )||IsFloatEmpty( fg.KZ )
            || IsFloatEmpty( fg.KV ) || IsFloatEmpty( fg.WW ) )
    {
// #ifndef IPMGEMPLUGIN
//        if( !vfQuestion( 0, aW.twp->DRkey /*"FGL"*/,
//                 "FGL: some critical parameters are missing! Proceed?" ))
//            return; //Error("FGL","FGL: some critical parameters are missing!);
//        else
// #endif
//      {   // potentially error!
            fg.VLK=RG*fg.TK/fg.P2;
            goto NEXT;
//        }
    }
    fg.TR=fg.TK/fg.KT;
    fg.PR=fg.P2/fg.KP;
    fg.IH=0;
    if( aW.twp->PdcC == DC_GAS_H2 )
        fg.IH=1;
    if( aW.twp->PdcC == DC_GAS_H2O ) fg.IH=2;
    if( fg.TK > fg.KT )
        ZAK(PGL);
    else DOK(&PH,&PB,PGL);
    /*   if( fg.TK<=fg.KT && fg.P2 > PH && PGL )
           twp->G += RT*fg.TK *log(PB);
         else  bm->G[I]+=RT*fg.TK*log(fg.P2); */
    /* add  ln fugitivn */
    aW.twp->G += 8.3144*fg.TK*log(fg.FUG);
    /*   if( bp->PGT==1||bp->PGT==3 ) {  */
    /* add entolp and entrop */
    aW.twp->H += fg.DH*4.184;
    aW.twp->S += fg.DS*4.184;
NEXT:
    /*  Helmholtz */
//    if( fg.TK<=fg.KT && fg.P2>PH && PGL )
//        aW.twp->F = aW.twp->G - (RP*fg.VLK*PB)*4.184;
//    else aW.twp->F = aW.twp->G - (RP*fg.VLK*fg.P2)*4.184;
    aW.twp->V = fg.VLK /* /10. */;
//    aW.twp->U = ((aW.twp->H/4.184)-RP*fg.VLK*fg.P2)*4.184;
    aW.twp->Fug = fg.FUG;   /* fugitivnost o */
}

//--------------------------------------------------------------------//
void TFGLcalc::DOK(float *PH,float *PB,int PGL)
{
    float  PK,Pr;
    double CHI;
    tw.T=fg.TK;
    if(fg.IH==2)
        *PH=SAT(fg.TK);
    else *PH=PY()*1.0132;
    if( *PH<=fg.P2&&PGL==0 ) *PB=*PH-.00001;
    if( *PH==fg.P2&&PGL==1 ) *PB=*PH+.00001;
    if( *PH>fg.P2&&PGL==1 )
    {
        *PB=*PH-.00001;
        PK=*PH+.00001;
    }
    if( *PH>fg.P2&&PGL==0||*PH<fg.P2&&PGL==1 ) *PB=fg.P2;
    /*   if( IJ==0&&PGL==0 )  */
    { fg.Z=1;
        Pr=fg.P2;
        fg.P2=*PB;
        fg.PR=*PB/fg.KP;
        fgl();
        CHI=fg.FUG;
        fg.P2=Pr;
        fg.PR=Pr/fg.KP;
    }
    /*     if( IJ==0&&PGL==1 )
         {  if( *PB<.01*fg.KP ) *PB=.01*fg.KP;
            if( *PB>10*fg.KP ) *PB=10*fg.KP;
            fg.PB=*PB; fg.PR=*PB/fg.KP;
            fg.Z=1;
            fgl(fg,fc,bp,&tw); bm->GAMMA[I]=fg.FUG; CHI=fg.FUG;
         }
         if(fg.IH==2)
       { fg.PB=*PB; fg.PR=*PB/fg.KP;
         fg.Z=1;
         fgl(fg,fc,bp,&tw); bm->GAMMA[I]=fg.FUG;
         fg.VLK=RG*fg.TK/fg.PB;
       }   */
    if( PGL==1&&*PH>fg.P2 )
    {
        if(fg.IH==2)
        {
            Pr=fg.P2;
            fg.P2=PK;
            fg.PR=PK/fg.KP;
            fg.Z=1;
            fgl();
            CHI=fg.FUG;
            fg.P2=Pr;
            fg.PR=Pr/fg.KP;
        }
        else
            fg.VLK=NG();
        /* ! */
        fg.FUG=exp(log(*PH*CHI)-
                   fg.VLK*(*PH-fg.P2)/(RG*fg.TK)-log(fg.P2));
    }
    if( *PH<=fg.P2&&PGL==0 )  fg.VLK=RG*fg.TK/fg.P2;
}

//--------------------------------------------------------------------//
void TFGLcalc::ZAK(int PGL)
{
    float LLG,PK=0.,PB,PR;
    int PLLG = -3;  /* By default !!! */
    double CHI;
    if( PLLG>=0 )
    {
        if( PLLG<1e-10 ) LLG=fg.KV;
        if( PLLG>1e-10 ) LLG=PLLG*fg.KV;
        fg.VLK=LLG;
        PK=PKDM( fg.P2,1);
    }
    if( PLLG<0 && PLLG>-1.1 )
    {
        PK=KNIGHT(fg.TK);
        PR=fg.P2;
        fg.P2=PK;
        fg.PR=PK/fg.KP;
        fgl(); //CHI=fg.FUG;
        LLG=fg.VLK;
        fg.P2=PR;
        fg.PR=PR/fg.KP;
    }
    if( PLLG>-2.1&&(PGL==0&&PK<=fg.P2||PGL==1&&PK>fg.P2) ) PB=PK;
    else PB=fg.P2;
    fg.Z=1;
    PR=fg.P2;
    fg.P2=PB;
    fg.PR=PB/fg.KP;
    fgl();
    fg.P2=PR;
    fg.PR=PR/fg.KP;
    CHI=fg.FUG;
    /*   bm->GAMMA[I]=CHI; */
    if( PB==PK )
        /*   bm->GAMMA[I]  */
        fg.FUG = exp(log(PK*CHI)+LLG*(fg.P2-PK)/(RG*fg.TK)-log(fg.P2));
}

//--------------------------------------------------------------------//
float TFGLcalc::SAT( float T )
{ /*float a[8] = { -7.85823,1.83991,-11.7811,22.6705,-15.9393,1.77516,
                      PC=220.64, TC= 647.14}; */
    float *A, KK;
    double  TAU;
    TConst* aCon= TConst::pm;
    aCon->RecInput( "FGCONST:Sat_H2O:" );
    aCon->dyn_set();
    A = (float *)aCon->CA;
    /* { strcpy( sd->ConLab, "Sat : A1-A6,PC,TC");
       sd->DimK  = 1; sd->DimI  = 1;  sd->DimJ  = 8; sd->Type  = F_;
       if( !(A = (float *)OBJalloc( O_CONSTDATA, F_, 1, 8 )))
         return F_EMPTY;
       memset( A, 0, 8*sizeof(float));
       memcpy( A, a, 8*sizeof(float));
       if( DBadd( RT_CONST, "FGCONST:Sat_H2O:" ) < ZERO )
        return F_EMPTY;
      iRet = DBfind( RT_CONST, "FGCONST:Sat_H2O:" );
     }*/
    TAU=1-T/A[7];
    KK=A[7]/T*(A[0]*TAU+A[1]*pow(TAU,1.5)+A[2]*pow(TAU,3)+A[3]*pow(TAU,3.5)+
               A[4]*pow(TAU,4)+A[5]*pow(TAU,7.5));
    KK=A[6]*exp(KK);
    return KK;
}

//--------------------------------------------------------------------//
float TFGLcalc::KNIGHT( float T )
{ /*float a[8] = { 273.15, 2094, 20.56, .06896, 8.903, 1e-5, 4.214, 1e-8 };*/
    float *A, TAU, KK;
    TConst* aCon = TConst::pm;
    aCon->RecInput( "FGCONST:KNIGHT:" );
    aCon->dyn_set();
    A = (float *)aCon->CA;
    /*{ strcpy( sd->ConLab, "KNIGHT : Š®íää¨æ¨¥­âë");
       sd->DimK  = 1; sd->DimI  = 1;  sd->DimJ  = 8; sd->Type  = F_;
       if( !(A = (float *)OBJalloc( O_CONSTDATA, F_, 1, 8 )))
         return F_EMPTY;
       memset( A, 0, 8*sizeof(float));
       memcpy( A, a, 8*sizeof(float));
       if( DBadd( RT_CONST, "FGCONST:KNIGHT:" ) < ZERO )
        return F_EMPTY;
      iRet = DBfind( RT_CONST, "FGCONST:KNIGHT:" );
     }*/
    TAU = T-A[0];
    KK= A[1]-A[2]*TAU+A[3]*TAU*TAU-A[4]*A[5]*TAU*TAU*TAU
        +A[6]*A[7]*TAU*TAU*TAU*TAU;
    return KK;
}

//--------------------------------------------------------------------//
void TFGLcalc::fgl()
{
    float KT,KP;
    KT=fg.KT;
    KP=fg.KP;
    if(KT>=5.1&&KT<=5.28 || KT>=33.&&KT<=33.4 || KT>=44.&&KT<=44.8)
    {
        if(KT>=5.1&&KT<=5.28)
        {
            KT=10.47;
            KP=6.67;
        }
        if(KT>=33.&&KT<=33.4)
        {
            KT=43.6;
            KP=20.2;
        }
        if(KT>=44.&&KT<=44.8)
        {
            KT=45.5;
            KP=26.9;
        }
        KT=KT/(1.+21.8/(fg.MM*fg.TK));
        KP=KP/(1.+44.2/(fg.MM*fg.TK));
    }
    if(KT>=647.&&KT<=647.6)
    {
        KT=480.81+157863.*(1./fg.TK);
        KP=251.95+53171.78*(1./fg.TK);
    }
    if(KT>=405.&&KT<=406.2)
    {
        KT=389.64+16326.99*(1./fg.TK);
        KP=118.13+16406.4*(1./fg.TK);
    }
    fg.P1=1.;
    fg.TR=fg.TK/fg.KT;
    fg.PR=fg.P2/fg.KP;
    fc.Tmin[0]=0.3*fg.KT;
    fc.Tmax[0]=4.*fg.KT;
    fc.Pmin[0]=0.01*fg.KP;
    fc.Pmax[0]=10.*fg.KP;
    fc.Tmin[1]=KT;
    fc.Tmax[1]=50.*KT;
    fc.Pmin[1]=KP;
    fc.Pmax[1]=2000.*KP;
    /*  fc.Tmin[2]=10.;
      fc.Tmax[2]=15000.;
      fc.Pmin[2]=0.;
      fc.Pmax[2]=1000.;
      fc.Tmin[3]=400.;
      fc.Tmax[3]=3273.;
      fc.Pmin[3]=1.;
      fc.Pmax[3]=1000.;
      fc.Tmin[4]=400.;
      fc.Tmax[4]=3273.;
      fc.Pmin[4]=1000.;
      fc.Pmax[4]=5000.;
      fc.Tmin[5]=400.;
      fc.Tmax[5]=15000.;
      fc.Pmin[5]=5000.;
      fc.Pmax[5]=1000000.;
      fc.Tmin[6]=149.;
      fc.Tmax[6]=8195.;
      fc.Pmin[6]=55.;
      fc.Pmax[6]=170000.;
      fc.Tmin[7]=273.15;
      fc.Tmax[7]=2273.15;
      fc.Pmin[7]=1.;
      fc.Pmax[7]=500000.;
      fc.Tmin[8]=150.;
      fc.Tmax[8]=7500.;
      fc.Pmin[8]=1.;
      fc.Pmax[8]=1000000.;*/
    fugaz();
}

//--------------------------------------------------------------------//
void TFGLcalc::fugaz()
{
    int i,pr,I=0,nPr[7];
    float gPmin[7],gPmax[7],P,GTP,DH;
    float RR=1.98719;
    for(i=0;i<7;i++) nPr[i]=0;
    P=fg.P2;
    if(fg.P1>fg.P2)
    {
        BERTOLE(fg.P1,fg.P2,&GTP,&DH);
        fg.DH=DH;
        fg.GTP=GTP;
        fg.FUG=exp(fg.GTP/(RR*fg.TK)-log(fg.P2));
        fg.DS=(fg.DH-fg.GTP)/fg.TK;
        fg.dU=fg.DH-(RR*fg.TK-.023901*fg.P2*fg.VLK);
        return;
    }
    while(1)
    {
        pr=OPTION(P);
        nPr[I]=pr;
        gPmax[I]=P;
        if(!I&&!fg.Z)
        {
            gPmin[0]=fg.P1;
            fg.Z=1;
            break;
        }
        if(fc.Pmin[pr-1]>fg.P1)
        {
            gPmin[I]=fc.Pmin[pr-1];
            P=gPmin[I]-0.001;
            I++;
            continue;
        }
        else
        {
            gPmin[I]=fg.P1;
            break;
        }
    }
    fg.DS=0.;
    fg.DH=0;
    fg.GTP=0.;
    for(i=0;i<=I;i++)
    {
        if(nPr[i]==1)
            LEEK( gPmin[i],gPmax[i],&GTP,&DH);
        if(nPr[i]==2)
            BREED( gPmin[i],gPmax[i],&GTP,&DH);
        if(nPr[i]==3)
            BERTOLE( gPmin[i],gPmax[i],&GTP,&DH);
        if(nPr[i]==4)
            FESAX(1, gPmin[i],gPmax[i],&GTP,&DH);
        if(nPr[i]==5)
            FESAX(2, gPmin[i],gPmax[i],&GTP,&DH);
        if(nPr[i]==6)
            FESAX(3,  gPmin[i],gPmax[i],&GTP,&DH);
        if(nPr[i]==7)
            FESAX(4, gPmin[i],gPmax[i],&GTP,&DH);
        if(nPr[i]==9)
            EXTRA( gPmin[i],gPmax[i],&GTP,&DH);
        if(nPr[i]==8)
            water_g( gPmin[i],gPmax[i],&GTP,&DH);
        fg.DH+=DH;
        fg.GTP+=GTP;
    }
    fg.FUG=exp(fg.GTP/(RR*fg.TK)-log(fg.P2));
    fg.DS=(fg.DH-fg.GTP)/fg.TK;
    fg.dU=fg.DH-(RR*fg.TK-.023901*fg.P2*fg.VLK);
}

typedef struct
{  /* prioritet equations of statr */
    int gas[6],
    h2[4],
    h2o[6];
}
B_PRIORITY;
B_PRIORITY bpp = {
    { 1,2,3,4,6,9 },
    { 1,2,3,9 },
    { 8,1,2,3,4,9 }
};
B_PRIORITY *bp;

//--------------------------------------------------------------------//
int TFGLcalc::OPTION(float P)
{
    int i,pr,kp=6;
    bp = &bpp;
    if(fg.IH==1) kp=4;
    for(i=0;i<kp; i++)
    {
        if(!fg.IH)   pr=bp->gas[i];
        if(fg.IH==1) pr=bp->h2[i];
        if(fg.IH==2) pr=bp->h2o[i];
        if(fg.TK>=fc.Tmin[pr-1] && fg.TK<=fc.Tmax[pr-1] &&
                P>=fc.Pmin[pr-1] && P<=fc.Pmax[pr-1])
            return(pr);
    }
    return(9);
}

//--------------------------------------------------------------------//
float TFGLcalc::PY()
{
    float Z1,Z2,Z3,Z4,Z5,Z;
    Z1=5.92714-6.09648/fg.TR-1.28862*log(fg.TR)+.169347*pow(double(fg.TR),6.);
    Z2=15.2518-15.6875/fg.TR-13.4721*log(fg.TR)+.43577*pow(double(fg.TR),6.);
    Z3=fg.WW*Z2;
    Z4=Z1+Z3;
    Z5=exp(Z4);
    Z=Z5*fg.KP;
    return(Z);
}

//--------------------------------------------------------------------//
float TFGLcalc::PKDM(float P,float EPS)
{
    float DP,PC;
    int I;
    DP=500.;
    I=0;
    PC=0;
M:
    fg.PR=fg.P2/fg.KP;
    fgl();
    if( fabs(fg.VLK-fg.KV)<=EPS || fabs(fg.P2-PC)<=1)
        goto ME;
    PC=fg.P2;
    if( fg.VLK>fg.KV)
    {
        if( I==-1)
            DP=DP/2;
        if( P>=fg.KP*10)
        {
            fg.P2=fg.KP*10;
            goto ME;
        }
        I=1;
        P=P+DP;
        goto M;
    }
    else if(!I)
    {
        DP=(P-1)/2;
        P=1;
        I=-1;
        goto M;
    }
    else
    {
        if( P-DP<=0)
            goto ME;
        if( I==1)
            DP=DP/2;
        I=-1;
        P=P-DP;
        goto M;
    }
ME:
    return(P);
}

//--------------------------------------------------------------------//
float TFGLcalc::NG()
{
    float K1,K2,K4,D,X,KZ;
    double TR;
    TR=fg.TR;
    KZ=fg.KZ;
    if( KZ>0.3)
        KZ=0.3;
    K1=17.4425-(214.578*KZ)+(989.625*(KZ*KZ))-(1552.06*(KZ*KZ*KZ));
    if( fg.KZ<=.26)
        K2=-3.28257+(13.4777*KZ)+(107.4844*(KZ*KZ))-(384.211*(KZ*KZ*KZ));
    else if( KZ>.26)
        K2=60.2091-(402.063*KZ)+(501.0*(KZ*KZ))+(641.0*(KZ*KZ*KZ));
    K4=.93-K2;
    D=1+(K1*pow(1.0-TR,.333333))+(K2*pow(1.0-TR,0.666667))+
      (K4*pow(1.0-TR,1.333333));
    X=fg.KV/D;
    return(X);
}

//--------------------------------------------------------------------//
// calc delta G,H,KZ if  P>=1 and 0.1<PR<10  and  .3<TR<4
void TFGLcalc::FESAX(int TT,float P1,float P2, float *GTP,float *DH )
{
    float A,B,C,D,RR,ZZ; double TR,KT,TK;  /*,PR;*/
    float KP,R;
    TK=fg.TK;
    KT=fg.KT;
    KP=fg.KP;
    RR=1.98719;
    TR=TK/KT;
    TT--;
    A=fc.COF[0][TT]+fc.COF[1][TT]/TR+fc.COF[2][TT]/(TR*TR)+
      fc.COF[3][TT]*log(TR);
    B=fc.COF[4][TT]+fc.COF[5][TT]/TR+fc.COF[6][TT]/TR/TR+
      fc.COF[7][TT]/TR/TR/TR+fc.COF[8][TT]*log(TR);
    C=fc.COF[9][TT]+fc.COF[10][TT]/TR+fc.COF[11][TT]/TR/TR;
    C+=fc.COF[12][TT]/TR/TR/TR+fc.COF[13][TT]/TR/TR/TR/TR+
       fc.COF[14][TT]/pow(TR,1.5)+fc.COF[15][TT]*log(TR);
    D=fc.COF[16][TT]+fc.COF[17][TT]/TR+fc.COF[18][TT]/TR/TR;
    D+=fc.COF[19][TT]/(TR*TR*TR);
    R=A*log(P2/P1)+B*(P2-P1)/KP+C*(P2*P2-P1*P1)/KP/KP/2.;
    *GTP=(R+D*(P2*P2*P2-P1*P1*P1)/3./KP/KP/KP)*RR*TK;
    ZZ=A/P2+B/KP+C*P2/KP/KP+D*P2*P2/KP/KP/KP;
    if(fg.Z)
    {
        fg.VLK=83.144*TK*ZZ;
        fg.Z=0;
    }
    A=-(fc.COF[1][TT]*KT)/(TK*TK)-(2.*fc.COF[2][TT]*KT*KT)/(TK*TK*TK)
      +fc.COF[3][TT]/TK;
    B=-(fc.COF[5][TT]*KT)/(TK*TK)-2.*fc.COF[6][TT]*KT*KT/(TK*TK*TK)-
      3.*fc.COF[7][TT]*KT*KT*KT/(TK*TK*TK*TK)+fc.COF[8][TT]/TK;
    C=-fc.COF[10][TT]*KT/(TK*TK)-2.*fc.COF[11][TT]*KT*KT/(TK*TK*TK)-
      3.*fc.COF[12][TT]*KT*KT*KT/pow(TK,4.)-4.*fc.COF[13][TT]*pow(KT,4.)/
      pow(TK,5.)-1.5*fc.COF[14][TT]*pow(KT,1.5)/pow(TK,2.5)
      +fc.COF[15][TT]/TK;
    D=-fc.COF[17][TT]*KT/(TK*TK)-2.*fc.COF[18][TT]*KT*KT/(TK*TK*TK)
      -3.*fc.COF[19][TT]*(KT*KT*KT)/(TK*TK*TK*TK);
    R=A*log(P2/P1)+B*(P2-P1)/KP+C*(P2*P2-P1*P1)/(2.*KP*KP);
    *DH=-RR*TK*TK*(R+D*(P2*P2*P2-P1*P1*P1)/(3.*KP*KP*KP));
    /*  fg.DS=(fg.DH-fg.GTP)/TK;
        fg.dU=fg.DH-(RR*TK-.023901*P2*fg.VLK); */
}

//--------------------------------------------------------------------//
// calc delta G,H,KZ if  P<1 and PR<.01 and PR>10  and TR<.3 and TR>4
void TFGLcalc::BERTOLE(float P1,float P2,float *GTP,float *DH)
{
    float A1,A2,A4,RR,ZZ,TK,KT,KP;
    TK=fg.TK;
    KT=fg.KT;
    KP=fg.KP;
    RR =1.98719;
    A1=9.*KT*(P2-P1)/(128.*KP);
    A4=9.*KT/(128.*KT*TK);
    A2=27.*KT*KT*KT*(P2-P1)/(64.*KP);
    ZZ=1./P2+A4*(1.-6.*KT*KT/(TK*TK));
    if(fg.Z)
    {
        fg.VLK=83.144*TK*ZZ;
        fg.Z=0;
    }
    *GTP=RR*TK*(log(P2/P1)+A1/TK-A2/(TK*TK*TK));
    *DH=-RR*TK*TK*(-A1/(TK*TK)+3.*A2/(TK*TK*TK*TK));
}

//--------------------------------------------------------------------//
// calc delta G,H,KZ if PR>.01 and PR<=10  and TR>.3 and TR<=4
void TFGLcalc::LEEK( float P1, float P2, float *GTP, float *DH)
{
    float TK,KT,KP,RR,PC,TR,PR,g1,g2,h1,h2;
    RR =1.98719;
    KT=fg.KT;
    KP=fg.KP;
    TK=fg.TK;
    PC=fg.P2;
    TR=fg.TR;
    PR=fg.PR;
    fg.P2=P2;
    fg.TR=TK/KT;
    fg.PR=P2/KP;
    LIKES();
    g2=RR*TK*(log(FD())+log(P2));
    h2=-RR*KT*ENTLG();
    if(P1>1.000000000001)
    {
        fg.P2=P1;
        fg.PR=P1/KP;
        LIKES( );
        g1=RR*TK*(log(FD())+log(P1));
        h1=-RR*KT*ENTLG();
    }
    else
    {
        g1=0;
        h1=0.;
    }
    *GTP=g2-g1;
    *DH=h2-h1;
    fg.P2=PC;
    fg.TR=TR;
    fg.PR=PR;
}

//--------------------------------------------------------------------//
void TFGLcalc::water_g( float P1, float P2,float *GTP,float *DH)
{
    float g1,g2,h1,h2;
    tw.T=fg.TK;
    tw.P=P2;
    TERH2O(  tw.R);
    if(fg.Z)
    {
        fg.VLK=1./tw.R*18.0153;
        fg.Z=0;
    }
    {
        tw.GT/=4.184;
        tw.HT/=4.184;
    }
    h2=tw.HT;
    g2=tw.GT;
    tw.P=P1;
    TERH2O(  tw.R);
    {
        tw.GT/=4.184;
        tw.HT/=4.184;
    }
    h1=tw.HT;
    g1=tw.GT;
    *GTP=g2-g1;
    *DH=h2-h1;
}

/*float a[9][7]={ 0.,-5306.2968529023,22744.901424408,
     -26627.94482977,7877.9333020687,0.,-698.30527374994,
      0.,178638.32875422,-395147.31563338,0.,338038.84280753,
      0.,-138550.50202703,68833.257944332,-2563743.661326,4821257.5981415,
      217572.45522644,-3418301.696966,0.,1222315.6417448,
      0.,11797433.655832,-21734810.110373,0.,10829952.16862,0.,-2544199.8064049,
      0.,-31377774.947767,52911910.757704,-707304.18082074,
     -13802577.177877,0,-2510991.4369001,0,46561826.115608,-72752773.275387,
      0.,4177424.6148294,0.,14016358.244614,0.,-31555231.392127,47929666.384584,
      0.,4091266.4781209,0.,-13626369.388386,0.,0.,0.,0.,0.,0.,0.,0.,
      6962522.0862664,-10834900.096447,0.,-2272282.7401688,0.,3836548.600066};
float b[18] = { B0=.747862916,C=-.354078223,D=.007159876,
               E=-.003528426,BB0=1.127833441,B1=-.594400123,
               B2=-5.010995915,B4=.636842555,RG=4.615233,
               AL=11,BET=44.3333333333,GAM=3.5,
               UT=-2.4801167e5,ST=.1083,HT=251728.46,
               FT=178499.,GT1=1.80153,GT2=3716.64};
float aa[18]=
    { 20.9662681977,1973.0271018,-.483429455355,6.05743189245,
     -68.3010170092,-106.4285576,-4.3135538513,.458155781,
     -.047754901883,.0041238460633,-.00027929052852,1.4481695261E-5,
     -5.6473658748E-7,1.6200446E-8,-3.303822796E-10,4.51916067368E-12,
     -3.70734122708E-14,1.37546068238E-16}; */

#define A(i,j) (*(A1+i*7+j))
//--------------------------------------------------------------------//
// 19/10/1999: ATTENTION! variable 'HZ' was changed to 'HZ1'
void TFGLcalc::TERH2O(float RH)
{
    float TAU,BT,BTT,B,BB,BBT,BBTT,Y,ER,PR,DPR,S,
    YT,YTT,YR,V,VT,VP,VTT,VPP,VTP,DCHI,XLNCHI,
    DF,DU,DCV,SS2,S3,SS1,RS,TT,LNT,GZ,HZ1,CPZ,F0,G0,U0,H0,S0,
    CV0,CP0,FZ,UZ,CVZ,F,U,CV,G,H,CP,
    DPT,DPRR,DPTT,DPRT,S1,S2,KK,NN,KKK,AB1,AB2,AB3,
    B0,C,D,E,BB0,B1,B2,B4, RG1,AL,BET,GAM,UT,ST,HT,FT,GT1,GT2;
    int I,N,J;
    float *A1, *AA;

    TABLE1();
    TConst* aCon = TConst::pm;
    aCon->RecInput( "FGCONST:TERH2O:" );
    aCon->dyn_set();
    A1 = (float *)aCon->CA;
    /* { strcpy( sd->ConLab, "TERH2O: A[9][7], AA[18]  ¨ â.¤. ");
       sd->DimK  = 1; sd->DimI  = 6;  sd->DimJ  = 18; sd->Type  = F_;
       if( !(A1 = (float *)OBJalloc( O_CONSTDATA, F_, 6, 18 )))
         return;
       memset( A1, 0, 108*sizeof(float));
       memcpy( A1, a, 63*sizeof(float));
       memcpy( A1+72, b, 18*sizeof(float));
       memcpy( A1+90, aa, 18*sizeof(float));
       if( DBadd( RT_CONST, "FGCONST:TERH2O:" ) < ZERO )
        return;
      iRet = DBfind( RT_CONST, "FGCONST:TERH2O:" );
     }*/
    AA = A1+72;
    B0=AA[0];
    C=AA[1];
    D=AA[2];
    E=AA[3];
    BB0=AA[4];
    B1=AA[5];
    B2=AA[6];
    B4=AA[7];
    RG1=AA[8];
    AL=AA[9];
    BET=AA[10];
    GAM=AA[11];
    UT=AA[12];
    ST=AA[13];
    HT=AA[14];
    FT=AA[15];
    GT1=AA[16];
    GT2=AA[17];
    AA = A1+90;

    TAU=tw.T/647.073;
    B=B0+C*log(TAU)+D/(TAU*TAU*TAU)+E/(TAU*TAU*TAU*TAU*TAU);
    BT=(C-3*D/(TAU*TAU*TAU)-5*E/(TAU*TAU*TAU*TAU*TAU))/tw.T;
    BTT=-(C-12*D/(TAU*TAU*TAU)-30*E/(TAU*TAU*TAU*TAU*TAU))/(tw.T*tw.T);
    BB=BB0+B1/TAU+B2/(TAU*TAU)+B4/(TAU*TAU*TAU*TAU);
    BBT=-(B1/TAU+2*B2/(TAU*TAU)+4*B4/(TAU*TAU*TAU*TAU))/tw.T;
    BBTT=(2*B1/TAU+6*B2/(TAU*TAU)+20*B4/(TAU*TAU*TAU*TAU))/(tw.T*tw.T);
    if(RH<1e-18) RH=HAAR(tw.T,tw.P);
    tw.R=RH;
    Y=RH*B/4.;
    YT=RH*BT/4.;
    YTT=RH*BTT/4.;
    YR=B/4.;
    ER=exp(-RH);
    PR=0.;
    DPR=0.;
    DPT=0.;
    DPRR=0.;
    DPTT=0.;
    DPRT=0.;
    DCHI=0.;
    DF=0.;
    DU=0.;
    DCV=0.;
    for(N=0;N<9;N++)
    {
        S=A(N,0);
        S1=0.;
        S2=0.;
        SS2=A(N,0);
        S3=0.;
        for(J=1;J<7;J++)
        {
            KK=A(N,J)/pow(TAU,J);
            S+=KK;
            S1+=(-J)*KK;
            S2+=J*(J+1)*KK;
            SS1=S;
            SS2+=(J+1)*KK;
            S3+=(J+1)*(-J)*KK;
        }
        KKK=RH*((N+1)*ER-1)/(1-ER);
        NN=pow((1-ER),N);
        PR+=S*NN;
        DPR+=S*NN*(2+KKK);
        DPT+=S1*NN;
        DCHI+=(S*pow((1-ER),(N+1)))/(N+1);
        DPRR+=S*((1+KKK)*(2+KKK)*NN+RH*NN/(1-ER)*((N+1)*ER*(1-RH)-1-ER*KKK));
        DPTT+=S2*NN;
        DPRT+=S1*(2+KKK)*NN;
        DF+=SS1*NN*(1-ER)/(N+1);
        DU+=SS2*NN*(1-ER)/(N+1);
        DCV+=S3*NN*(1-ER)/(N+1);
    }
    AB1=(1+AL*Y+BET*Y*Y)/((1-Y)*(1-Y)*(1-Y));
    AB2=(1+2*AL*Y+3*BET*Y*Y)/((1-Y)*(1-Y)*(1-Y));
    AB3=(AL+2*BET*Y)/((1-Y)*(1-Y)*(1-Y));
    PR=RH*RH*ER*PR+RH*RG1*tw.T*(AB1+RH*(BB-B*GAM));
    DPR=RH*ER*DPR+RG1*tw.T*(AB2+3*Y*AB1/(1-Y)+2*RH*(BB-B*GAM));
    DPT=RH*RH*ER*DPT/tw.T+RH*RG1*(AB1+RH*(BB-GAM*B))+RH*RG1*tw.T*(AB3
            +3*AB1/(1-Y))*YT+RH*RH*RG1*tw.T*(BBT-GAM*BT);
    DPTT=RH*RH*ER*DPTT/(tw.T*tw.T)+2*RH*RG1*(AB3+3*AB1/(1-Y))*YT+2*RH*RH*RG1*
         (BBT-GAM*BT)+RH*RG1*tw.T*(2*BET/((1-Y)*(1-Y)*(1-Y))+6*AB3/(1-Y)
                                   +12*AB1/((1-Y)*(1-Y)))*YT*YT+RH*RG1*tw.T*(AB3+3*AB1/(1-Y))*YTT
         +RH*RH*RG1*tw.T*(BBTT-GAM*BTT);
    DPRR=ER*DPRR+2*RG1*tw.T*((AL+3*BET*Y)/((1-Y)*(1-Y)*(1-Y))+3*AB2/(1-Y)
                             +6*Y*AB1/((1-Y)*(1-Y)))*YR+2*RG1*tw.T*(BB-GAM*B);
    DPRT=RH*ER*DPRT/tw.T+2*RH*RG1*tw.T*(BBT-GAM*BT)+RG1*(AB2
            +3*Y*AB1/(1-Y)+2*RH*(BB-GAM*B))+2*RG1*tw.T*((AL+3*BET*Y)/
                    ((1-Y)*(1-Y)*(1-Y))+3*AB2/(1-Y)+6*Y*AB1/((1-Y)*(1-Y)))*YT;
    XLNCHI=(1+AL+BET)/(2*(1-Y)*(1-Y))-log(1-Y)-(BET-1)/(1-Y)
           -(3+AL-BET)/2+RH*(BB-GAM*B)-log(tw.P/(RH*RG1*tw.T))+tw.P/(RH*RG1*tw.T)
           -1+DCHI/RG1/tw.T;
    tw.CHI=exp(XLNCHI);
    tw.FUG=tw.CHI*tw.P;
    V=1/RH;
    VT=DPT/(RH*RH*DPR);
    VP=-1/(RH*RH*DPR);
    VTT=2*(DPT/DPR)*(DPT/DPR)/(RH*RH*RH)+DPTT/(DPR*RH*RH)-2*DPRT*DPT/
        ((RH*DPR)*(RH*DPR))+DPRR*DPT*DPT/(RH*RH*DPR*DPR*DPR);
    VPP=(2*DPR+RH*DPRR)/(RH*DPR*RH*DPR*RH*DPR);
    VTP=-2*DPT/(RH*RH*RH*DPR*DPR)-DPRR*DPT/(RH*RH*DPR*DPR*DPR)+
        DPRT/(RH*DPR*RH*DPR);
    tw.DAT=-(VT/V)*(VT/V)+VTT/V;
    tw.DBT=VT*VP/(V*V)-VTP/V;
    tw.DAP=-VP*VT/(V*V)+VTP/V;
    tw.DBP=(VP/V)*(VP/V)-VPP/V;
    tw.ALP=VT/V;
    tw.BE=-VP/V;
    TT=tw.T/100;
    LNT=log(tw.T);
    GZ=-(AA[0]+AA[1]/tw.T)*LNT;
    HZ1=AA[0]+AA[1]*(1-LNT)/tw.T;
    CPZ=AA[0]-AA[1]/tw.T;
    for(I=2;I<18;I++)
    {
        GZ-=AA[I]*pow(TT,(I-5));
        HZ1+=AA[I]*(I-5)*pow(TT,(I-5));
        CPZ+=AA[I]*(I-5)*(I-4)*pow(TT,(I-5));
    }
    F0=GZ-1;
    G0=GZ;
    U0=HZ1-1;
    H0=HZ1;
    S0=H0-G0;
    CV0=CPZ-1;
    CP0=CPZ;
    RS=8.31448;
    tw.G1=G0*RS*tw.T-182217.57;
    tw.H1=H0*RS*tw.T-251730.09;
    tw.S01=S0*RS+.1089;
    tw.CP1=CP0*RS;
    FZ=F0-log(1-Y)-(BET-1)/(1-Y)+(1+AL+BET)/(2*(1-Y)*(1-Y))
       +RH*(BB-GAM*B)-(AL-BET+3)/2+log(RG1*RH*tw.T/1  );
    UZ=U0+tw.T*((1/(Y-1)+(BET-1)/((1-Y)*(1-Y))-(1+AL+BET)/((1-Y)*(1-Y)*(1-Y)))*
                YT-RH*(BBT-GAM*BT));
    CVZ=CV0+2*tw.T*((1/(Y-1)+(BET-1)/((1-Y)*(1-Y))-(1+AL+BET)/((1-Y)*(1-Y)*(1-Y)))
                    *YT-RH*(BBT-GAM*BT));
    CVZ+=tw.T*tw.T*((2*(BET-1)/((1-Y)*(1-Y)*(1-Y))-
                     1/((1-Y)*(1-Y))-3*(1+AL+BET)/((1-Y)*(1-Y)*(1-Y)*(1-Y)))*YT*YT+
                    (1/(Y-1)+(BET-1)/((1-Y)*(1-Y))
                     -(1+AL+BET)/((1-Y)*(1-Y)*(1-Y)))*YTT-RH*(BBTT-GAM*BTT));
    F=FZ+DF/RG1/tw.T;
    U=UZ+DU/RG1/tw.T;
    CV=CVZ+DCV/RG1/tw.T;
    G=F+tw.P*V/RG1/tw.T;
    H=U+tw.P*V/RG1/tw.T;
    CP=CV-tw.T*VT*VT/VP/RG1;
    tw.UT=U*tw.T*RS+UT;
    tw.ST=(H-G)*RS+ST;
    tw.HT=H*tw.T*RS-HT;
    tw.CVT=CV*RS;
    tw.CPT=CP*RS;
    tw.FT=F*tw.T*RS-FT;
    tw.GT=tw.FT+tw.P*V*GT1-GT2;
    tw.GPG0=tw.GT-tw.G1;
}

//--------------------------------------------------------------------//
float TFGLcalc::HAAR(float T,float P)
{
    /*float B0=.747862916,C=-.354078223,D=.007159876,E=-.003528426,
          BB0=1.127833441,B1=-.594400123,B2=-5.010995915,
          B4=.636842555,RG=4.615233,AL=11.,BET=44.3333333333,GAM=3.5;*/
    float B0,C,D,E,BB0,B1,B2,B4,RG1,AL,BET,GAM;
    float TAU,PS=0.,B,BB,RH,Y,ER,PR,DPR,S,RHN;
    int I,N,J;
    float *A1, *AA;

    TConst* aCon = TConst::pm;
    aCon->RecInput( "FGCONST:TERH2O:" );
    aCon->dyn_set();
    A1 = (float *)aCon->CA;

    AA = A1+72;
    B0=AA[0];
    C=AA[1];
    D=AA[2];
    E=AA[3];
    BB0=AA[4];
    B1=AA[5];
    B2=AA[6];
    B4=AA[7];
    RG1=AA[8];
    AL=AA[9];
    BET=AA[10];
    GAM=AA[11];
    //AA = A1+90;

    TAU=T/647.073;
    if( T<=647.25)  PS= SAT(T);
    if( P<=1&&T<372.7814&&P!=PS ) P=PS;
    B=B0+C*log(TAU)+D/(TAU*TAU*TAU)+E/(TAU*TAU*TAU*TAU*TAU);
    /* BT=(C-3*D/(TAU*TAU*TAU)-5*E/(TAU*TAU*TAU*TAU*TAU))/T;
    BTT=-(C-12*D/(TAU*TAU*TAU)-30*E/(TAU*TAU*TAU*TAU*TAU))/(T*T); */
    BB=BB0+B1/TAU+B2/(TAU*TAU)+B4/(TAU*TAU*TAU*TAU);
    /* BBT=-(B1/TAU+2*B2/(TAU*TAU)+4*B4/(TAU*TAU*TAU*TAU))/T;
    BBTT=(2*B1/TAU+6*B2/(TAU*TAU)+20*B4/(TAU*TAU*TAU*TAU))/(T*T); */
    if( P<235&&T>647.25&&T<873 ) RH=.01;
    else RH=1.;
    if( P<PS ) RH=.00001;
    if( P<=1&&T>647.25 ) RH=.000001;
    for(I=0;I<100;I++)
    {
        Y=RH*B/4.;
        ER=exp(-RH);
        PR=0.;
        DPR=0.;
        for(N=0;N<9;N++)
        {
            S=A(N,0);
            for(J=1;J<7;J++)
                S+=A(N,J)/pow(TAU,J);
            PR+=S*pow((1.-ER),N);
            DPR+=S*pow((1.-ER),N)*(2.+RH*((N+1)*ER-1.)/(1.-ER));
        }
        PR=RH*RH*ER*PR+RH*RG1*T*((1.+AL*Y+BET*Y*Y)/pow(1.-Y,3.)+4.*Y*(BB/B-GAM));
        DPR=RH*ER*DPR+RG1*T*((1.+2.*AL*Y+3.*BET*Y*Y)/pow(1.-Y,3.)+
                             3.*Y*(1.+AL*Y+BET*Y*Y)/pow(1.-Y,4.)+8.*Y*(BB/B-GAM));
        RHN=RH+(P-PR)/DPR;
        if( fabs(RHN-RH)<fabs(RHN/1e6)&&fabs(P-PR)<fabs(P/1e5)) break;
        RH=RHN;
        if(RH<0)
        { /*odz();*/ break;
        }
        /*  if( RH<0 ) printf("\nT=%f P=%f RHN=%f RH=%f PR=%f DPR=%f",
                      T,P,RHN,RH,PR,DPR); */
    }
    return(RH);
}

//--------------------------------------------------------------------//
void TFGLcalc::EXTRA( float P1, float P2,float *GTP,float *DH)
{
    float TK,KT,KP,TR,PR; /*PC,RR;*/
    KT=fg.KT;
    KP=fg.KP;
    TK=fg.TK;
    TR=fg.TR;
    PR=fg.PR;
    if(TR<0.3 && PR<=10. || TR>=0.3&&TR<=4.&&PR<=0.01)
    {
        if(TR<0.3 && PR>=0.01&&PR<=10.)
        {
            fg.KT=TK/0.31;
            fg.TR=TK/fg.KT;
        }
        if(TR<0.3 && PR<0.01)
        {
            fg.KT=TK/0.31;
            fg.KP=fg.P2/0.011;
            fg.TR=TK/fg.KT;
            fg.PR=P2/fg.KP;
        }
        if(TR>=0.3 && TR<=4. && PR<=0.01)
        {
            fg.KP=fg.P2/0.011;
            fg.PR=P2/fg.KP;
        }
        LEEK(P1,P2,&*GTP,&*DH);
    }
    if(TR<1.&&PR>=10. || TR>=1.&&TR<=50.&&PR>2000. || TR>50.&&PR>=1.)
    {
        if(TR<1. && PR>=10.&&PR<=2000.)
        {
            fg.KT=TK/1.1;
            fg.TR=TK/fg.KT;
        }
        if(TR<1. && PR>2000.)
        {
            fg.KT=TK/1.1;
            fg.KP=fg.P2/1999.9;
            fg.TR=TK/fg.KT;
            fg.PR=P2/fg.KP;
        }
        if(TR>=1. && TR<=50. && PR>2000.)
        {
            fg.KP=fg.P2/1999.9;
            fg.PR=P2/fg.KP;
        }
        if(TR>50. && PR>2000.)
        {
            fg.KT=TK/49.9;
            fg.KP=fg.P2/1999.9;
            fg.TR=TK/fg.KT;
            fg.PR=P2/fg.KP;
        }
        if(TR>50. && PR>=1. && PR<=2000.)
        {
            fg.KT=TK/49.9;
            fg.TR=TK/fg.KT;
        }
        BREED(P1,P2,&*GTP,&*DH);
    }
    fg.KT=KT;
    fg.KP=KP;
    fg.TR=TR;
    fg.PR=PR;
}
//--------------------------------------------------------------------//
//calc delta entalpie gas
float TFGLcalc::ENTLG()
{
    float H0,H1,L,L1,L2,E,P,P1,P2,R6,P3,R1,R2,TR,VR0,Z0,
    VRR,ZR,W,  L3,L4,L5,E1,P4,P5,P6,R3,P7,R4,R5,HR,H;
    TR=fg.TR;
    VR0=fg.VR0;
    Z0=fg.Z0;
    VRR=fg.VRR;
    ZR=fg.ZR;
    W=fg.WW;
    L=.042724/(2.*(TR*TR*TR)*.060167);
    L1=.65392+1.+(.060167/(VR0*VR0));
    L2=exp((-.060167)/(VR0*VR0));
    E=L*(.65392+1.-(L1*L2));
    P=(2.*.15479)/TR;
    P1=(3.*.030323)/(TR*TR);
    P2=.265728+P+P1;
    R6=P2/(TR*VR0);
    P3=.0186984-((3.*0.0)/(TR*TR));
    R1=P3/(2.*TR*(VR0*VR0));
    R2=.0000623689/(5.*TR*(VR0*VR0*VR0));
    R2=R2/(VR0*VR0);
    H0=-TR*(Z0-1.-R6-R1+R2+(3.*E));
    L3=.041577/(2.*(TR*TR*TR)*.03754);
    L4=1.226+1+(.03754/(VRR*VRR));
    L5=exp((-.03754)/(VRR*VRR));
    E1=L3*(1.226+1.-(L4*L5));
    P4=(2.*.027655)/TR;
    P5=(3.*.203488)/(TR*TR);
    P6=.331511+P4+P5;
    R3=P6/(TR*VRR);
    P7=.0503618-((3.*.016901)/(TR*TR));
    R4=P7/(2.*TR*(VRR*VRR));
    R5=.00000740336/(5.*TR*(VRR*VRR*VRR));
    R5=R5/(VRR*VRR);
    HR=-TR*(ZR-1.-R3-R4+R5+(3.*E1));
    H1=(1./.3978)*(HR-H0);
    /*  printf(" %7.3f",H0);  */
    H=H0+(W*H1);
    return(H);
}

//--------------------------------------------------------------------//
//Calc model Li-Kesslera
void TFGLcalc::LIKES()
{
    if( fg.TR>1.4 || fg.TR>1.1&&fg.PR>=1&&fg.PR<=5. ||
            fg.TR>1.&&fg.PR>=1&&fg.PR<=1.5 )
    {
        fg.HV=(82.04*fg.TK)/fg.P2;
        fg.VRK = ROOT(1,fg.HV,1e-5,1000);
        fg.HV=fg.VRK*fg.KP/(82.04*fg.KT);
        goto M4;
    }
    if( fg.TR<=1 && fg.TR>0 && fg.PR<1 &&  fg.P2<1.0132*PY( ))
    {
        fg.HV=(82.04*fg.TK)/fg.P2;
        fg.VRK= ROOT(1,fg.HV,1e-5,1000);
        fg.HV=fg.VRK*fg.KP/(82.04*fg.KT);
        goto M4;
    }
    if( fg.TR==1 && fg.PR==1 )
    {
        fg.VR0=.2901;
        goto M7;
    }
    if( fg.TR==1 && fg.PR>1)
    {
        fg.HV=NG();
        fg.HV=fg.HV*fg.KP/(82.04*fg.KT);
        fg.VR0=fg.HV;
        fg.VR0=BISEC(fg.HV/2.5,fg.HV*1.1,1e-7,1E-7,1);
        goto M7;
    }
    if( fg.PR<1 && fg.P2>=1.0132*PY())
    {
        fg.HV=NG();
        fg.HV=fg.HV*fg.KP/(82.04*fg.KT);
        fg.VR0=fg.HV;
        fg.VR0=BISEC(fg.HV/2.5,fg.HV*1.2,1e-7,1e-7,1);
        goto M7;
    }
    if(fg.PR>=1)
    {
        fg.HV=NG();
        fg.HV=fg.HV*fg.KP/(82.04*fg.KT);
        fg.VR0=fg.HV;
        fg.VR0=BISEC(fg.HV/2.5,fg.HV*1.2,1e-7,1e-7,1);
        goto M7;
    }
M4:
    fg.VR0= ROOT(2,fg.HV,1e-7,1000);
M7:
    fg.Z0=(fg.PR*fg.VR0)/fg.TR;
    if( fg.TR>1.4 || fg.TR>1.1&&fg.PR>=1&&fg.PR<=5. ||
            fg.TR>1.&&fg.PR>=1&&fg.PR<=1.5 )
    {
        fg.HV=fg.TK*24.5/(fg.P2*568.8);
        fg.VRK=ROOT(1,fg.HV,1e-5,1000);
        fg.HV=fg.VRK*fg.KP/(82.04*fg.KT);
        goto M5;
    }
    if( fg.TR<=1 && fg.PR<1 && fg.P2<1.0132*PY())
    {
        fg.HV=fg.TK*24.5/(fg.P2*568.8);
        fg.VRK=ROOT(1,fg.HV,1e-5,1000);
        fg.HV=fg.VRK*fg.KP/(82.04*fg.KT);
        goto M5;
    }
    if (fg.TR==1 && fg.PR==1)
    {
        fg.VRR=.25513338;
        goto M8;
    }
    if ( fg.TR==1 && fg.PR>1)
    {
        fg.HV=NG();
        fg.HV=fg.HV*fg.KP/(82.04*fg.KT);
        fg.VRR=fg.HV;
        fg.VRR=BISEC(fg.HV/2.5,fg.HV*1.1,1e-7,1e-7,2);
        goto M8;
    }
    if (fg.PR<1 && fg.P2>=1.0132*PY())
    {
        fg.HV=NG();
        fg.HV=fg.HV*fg.KP/(82.04*fg.KT);
        fg.VRR=fg.HV;
        fg.VRR= BISEC(fg.HV/2.5,fg.HV*1.1,1e-7,1e-7,2);
        goto M8;
    }
    if (fg.PR>=1)
    {
        fg.HV=NG();
        fg.HV=fg.HV*fg.KP/(82.04*fg.KT);
        fg.VRR=fg.HV;
        fg.VRR=BISEC(fg.HV/2.5,fg.HV*1.1,1e-7,1e-7,2);
        goto M8;
    }
M5:
    fg.VRR=ROOT(3,fg.HV,1e-7,1000);
M8:
    fg.ZR=(fg.PR*fg.VRR)/fg.TR;
    fg.Z1=(1/.3978)*(fg.ZR-fg.Z0);
    fg.ZLK=fg.Z0+(fg.WW*fg.Z1);
    if(fg.Z)
    {
        fg.VLK=fg.ZLK*82.04*fg.TK/fg.P2;
        fg.Z=0;
    }
    return;
}       /*     END LIKES;    */

//--------------------------------------------------------------------//
float TFGLcalc::BISEC(float A,float B,float EPS,float EPS1, short K )
{
    float Y,Z,X;
    X=A;
    fg.HV=X;
    if (K==1)
        Y=LB1();
    if (K==2)
        Y=LB2();
    if( fabs(Y)<=EPS)
        goto FIN;
    X=B;
    fg.HV=X;
    if (K==1)
        Z=LB1();
    if (K==2)
        Z=LB2();
    if( fabs(Z)<=EPS)
        goto FIN;
    if( (Y>=0 && Z>=0)||(Y<0 &&Z<0) )
        goto FIN;
ITER:
    X=A/2+B/2;
    fg.HV=X;
    if (K==1)
        Y=LB1();
    if (K==2)
        Y=LB2();
    if( (Y>=0 && Z>=0)||(Y<0 &&Z<0) )
        B=X;
    else A=X;
    if( fabs(A-B)>=EPS1)
        goto ITER;
FIN:
    return(X);
}

//--------------------------------------------------------------------//
float TFGLcalc::LB1()
{
    float B,C,D,T1,T2,T3,TR,HV,PR ;
    TR=fg.TR;
    HV=fg.HV;
    PR=fg.PR;
    B=.1181193-(.265728/TR)-(.15479/(TR*TR))-(.030323/(TR*TR*TR));
    C=.0236744-(.0186984/TR)+(0/(TR*TR*TR));
    D=.0000155488+(.0000623689/TR);
    T1=.042724/((TR*TR*TR)*(HV*HV));
    T2=(.65392+(.060167/(HV*HV)));
    T3=exp((-.060167)/(HV*HV));
    B=(HV-(TR/PR)*(1+B/HV+C/(HV*HV)+((D/(HV*HV*HV))/(HV*HV))+(T1*T2*T3)));
    return(B);
}

//--------------------------------------------------------------------//
float TFGLcalc::LB2()
{
    float B,C,D,T4,T5,T6,TR,HV,PR;
    TR=fg.TR;
    HV=fg.HV;
    PR=fg.PR;
    B=.2026579-(.331511/TR)-(.027655/(TR*TR))-(.203488/(TR*TR*TR));
    C=.0313385-(.0503618/TR)+(.016901/(TR*TR*TR));
    D=.000048736+(.00000740336/TR);
    D=(D/(HV*HV))/(HV*HV);
    T4=.041577/((TR*TR*TR)*(HV*HV));
    T5=(1.226+(.03754/(HV*HV)));
    T6=exp((-.03754)/(HV*HV));
    B=(HV-(TR/PR)*(1+B/HV+C/(HV*HV)+(D/HV)+(T4*T5*T6)));
    return(B);
}


//--------------------------------------------------------------------//
float TFGLcalc::FD()
{
    float F0,F1,B,C,D,L,L1,L2,E,G,G1,G2,L3,L4,L5,
    E1,B1,C1,D1,G3,G4,G5,FR,F,VR0,TR,VRR;
    TR=fg.TR;
    VR0=fg.VR0;
    VRR=fg.VRR;
    L=.042724/(2*(TR*TR*TR)*.060167);
    L1=.65392+1+(.060167/(VR0*VR0));
    L2=exp((-.060167)/(VR0*VR0));
    E=L*(.65392+1-(L1*L2));
    B=.1181193-(.265728/TR)-(.15479/(TR*TR))-(.030323/(TR*TR*TR));
    C=.0236744-(.0186984/TR)+(0/(TR*TR*TR));
    D=.0000155488+(.0000623689/TR);
    G=B/VR0;
    G1=C/(2*(VR0*VR0));
    G2=D/(5*(VR0*VR0*VR0));
    G2=G2/(VR0*VR0);
    F0=fg.Z0-1-log(fg.Z0)+G+G1+G2+E;
    F0=F0*.4342945;
    L3=.041577/(2*(TR*TR*TR)*.03754);
    L4=1.226+1+(.03754/(VRR*VRR));
    L5=exp((-.03754)/(VRR*VRR));
    E1=L3*(1.226+1-(L4*L5));
    B1=.2026579-(.331511/TR)-(.027655/(TR*TR))-(.203488/(TR*TR*TR));
    C1=.0313385-(.0503618/TR)+(.016901/(TR*TR*TR));
    D1=.000048736+(.00000740336/TR);
    G3=B1/VRR;
    G4=C1/(2*(VRR*VRR));
    G5=D1/(5*(VRR*VRR*VRR));
    G5=G5/(VRR*VRR);
    FR=fg.ZR-1-log(fg.ZR)+G3+G4+G5+E1;
    FR=FR*.4342945;
    F1=(1/.3978)*(FR-F0);
    /*    printf(" %7.3f",F0);  */
    F=F0+(fg.WW*F1);
    F=pow(10.,double(F));
    return(F);
}

//--------------------------------------------------------------------//
float TFGLcalc::RK()
{
    float B, F, BX;
    B=(.086640350*82.04*fg.KT)/fg.KP;
    F=(1/fg.TR)*(1+(.48+1.574*fg.WW-.176*
                    (fg.WW*fg.WW))*(1-(pow(double(fg.TR),.5))))*2;
    B=(fg.HV/(fg.HV-B)-(.4274802327*B*F/(.08664035*(fg.HV+B))));
    BX = (82.04*fg.TK/fg.P2)*B;
    return( BX );
}

//--------------------------------------------------------------------//
float TFGLcalc::LK1()
{
    float B,C,D,T1,T2,T3,HV2;
    HV2= fg.HV*fg.HV;
    B= (.030323/(fg.TR*fg.TR*fg.TR));
    B=.1181193-(.265728/fg.TR)-(.15479/(fg.TR*fg.TR))-B;
    C=.0236744-(.0186984/fg.TR)+(0./(fg.TR*fg.TR*fg.TR));
    D=.0000155488+(.0000623689/fg.TR);
    T1=.042724/((fg.TR*fg.TR*fg.TR)*HV2);
    T2=(.65392+(.060167/HV2));
    T3=exp((-.060167)/HV2);
    B=(1.+B/fg.HV+C/(HV2)+((D/(HV2*fg.HV))/HV2)+(T1*T2*T3));
    B=(fg.TR/fg.PR)*B;
    return(B);
}

//--------------------------------------------------------------------//
float TFGLcalc::LK2()
{
    float B,C,D,T4,T5,T6,TR,HV,PR;
    TR=fg.TR;
    HV=fg.HV;
    PR=fg.PR;
    B=.2026579-(.331511/TR)-(.027655/(TR*TR))-(.203488/(TR*TR*TR));
    C=.0313385-(.0503618/TR)+(.016901/(TR*TR*TR));
    D=.000048736+(.00000740336/TR);
    D=(D/(HV*HV))/(HV*HV);
    T4=.041577/((TR*TR*TR)*(HV*HV));
    T5=(1.226+(.03754/(HV*HV)));
    T6=exp((-.03754)/(HV*HV));
    B=((TR/PR)*(1+B/HV+C/(HV*HV)+(D/HV)+(T4*T5*T6)));
    return(B);
}

//--------------------------------------------------------------------//
float TFGLcalc::ROOT(short K,float A,float EPS,short N)
{
    float B,D,H,C,G;
    short J;//,M;

    //M=1;
    fg.HV=A;
    if(K==1)
        G=RK();
    if(K==2)
        G=LK1();
    if(K==3)
        G=LK2();
    B=G-A;
    D=B;
    C=B;
    if( fabs(C)<=EPS )
        goto FIN;
    for (J=0; J<N; J++)
    {
        fg.HV=G;
        if(K==1)
            C=RK()-G;
        if(K==2)
            C=LK1()-G;
        if(K==3)
            C=LK2()-G;
        if( fabs(C)<=EPS)
            goto FIN;
        H=B/C;
        B=C;
        if( H<=1.00001 && H>=.999999)
            goto FIN;
        //if( H>0 && H<2) M++;
        D/=(H-1);
        G+=D;
    }
    //M=-M;
FIN:
    return(G);
}

//--------------------- End of s_fgl.cpp ---------------------------
