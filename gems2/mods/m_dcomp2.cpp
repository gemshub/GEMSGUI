//-------------------------------------------------------------------
// $Id$
//
// Implementation of TDComp class, calculation functions
//
// Rewritten from C to C++ by S.Dmytriyeva
// Copyright (C) 1995-2009  S.Dmytriyeva, D.Kulik, V.Sinitsyn, T. Wagner
//
// This file is part of a GEM-Selektor library for thermodynamic
// modelling by Gibbs energy minimization
// Uses: GEM-Vizor GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//
#include "m_dcomp.h"
#include "m_param.h"
#include "visor.h"
#include "s_tpwork.h"
#include "s_supcrt.h"
#include <math.h>

const double
ZPrTr = -0.1278034682e-1,
        YPrTr = -0.5798650444e-4,
                Pref = 0.1e1,
                       Tref = 0.29815e3,
                              eta = 0.166027e6,
                                    theta = 0.228e3,
                                            psi = 0.26e4,
                                                  gref = 0.0e0;

//--------------------------------------------------------------------//
void
TDComp::calc_tpcv( int q, int p, int CE, int CV )
{
    double a=0., T/*=0.*/, TC/*=0.*/, /*Vst=0.,*/ Tst/*=0.*/,
           T_Tst/*=0.*/, Ts2/*=0.*/, TT/*=0.*/, dT/*=0.*/,
           T2/*=0.*/, T3/*=0.*/, T4/*=0.*/, T05/*=0.*/,
           Tst2/*=0.*/, Tst3/*=0.*/, Tst4/*=0.*/, Tst05/*=0.*/;
    int k=0, i, j, jf;
    double ac[16];
    float a1;

// cout << endl << " q=" << q << " p=" << p << "  CE " << CE << "  CV " << CV << endl;
    // iRet = ZERO;
    // default values
//    Vst = (double)dc[q].mVs[0];
//    if( IsFloatEmpty( dc[q].mVs[0] ))
//        Vst = 0.;
    dT = TK_DELTA;
    TC = aW.twp->TC;
    aW.twp->T =    T = aW.twp->TC + dT;
    aW.twp->Tst =  Tst = (double)dc[q].TCst + dT;

    aW.twp->S = (double)dc[q].Ss[0];
    aW.twp->G = dc[q].Gs[0]; /* - aW.twp->S * (T-Tst); */
    aW.twp->H = dc[q].Hs[0];
//    aW.twp->V = Vst;
    aW.twp->V = 0.0;
// cout << " TC="<< TC << " T=" << aW.twp->T << " Tst=" << aW.twp->Tst << " Sst=" << aW.twp->S << " Gst="
//     << aW.twp->G << " Hst=" << aW.twp->H << " Cpst=" << aW.twp->Cp << " Vst=" << aW.twp->V << endl;
    if(( dc[q].pstate[0] == CP_GAS || dc[q].pstate[0] == CP_GASI )
            && aW.twp->P > 0.0 )
    { /* molar volume from the ideal gas law */
        aW.twp->V = T / aW.twp->P * R_CONSTANT;
    }
    aW.twp->devG = dc[q].Gs[1];
//    aW.twp->Cv = 0.;
    aW.twp->Cp = 0.;
    if( !dc[q].Cp )
    {
        aW.twp->Cp = (double)dc[q].Cps[0];
        goto NEXT;
    }
    // get interval
    if((double)dc[q].TCint[0] > TC )
    {
        k=0;  /*iRet = 1;*/
    }
    else if( (double)dc[q].TCint[2*dc[q].NeCp-1] <= TC )
    {
        k= dc[q].NeCp-1;   /*iRet = 1;*/
    }
    else for( j=0; j<dc[q].NeCp; j++ )
            if( (double)dc[q].TCint[j] <= TC && (double)dc[q].TCint[j+dc[q].NeCp] > TC )
            {
                k=j;
                break;
            }
    T2 = T * T;
    T3 = T2 * T;
    T4 = T3 * T;
    T05 = sqrt( T );
    for( i=0; i</*MAXCPCOEF*/aObj[o_dccp].GetN(); i++ )
    {  // Cp(t)  current temperature only
        a1 = dc[q].Cp[i*dc[q].NeCp+k];
        if( IsFloatEmpty( a1 ) || !a1 )
	    ac[i] = 0.0;
        else ac[i] = (double)a1;
     }
     aW.twp->Cp = ( ac[0] + ac[1]*T + ac[2]/T2 + ac[3]/T05 + ac[4]*T2
           + ac[5]*T3 + ac[6]*T4 + ac[7]/T3 + ac[8]/T + ac[9]*T05 /*+ ac[10]*log(T)*/);

    if( fabs( T - Tst ) > TEMPER_PREC )
        for( j=0, jf=0; j<=k; j++ )
        {
            if( j == k )
                T = aW.twp->TC + dT;
            else T = (double)dc[q].TCint[j+dc[q].NeCp] + dT;
            T2 = T * T;
            T3 = T2 * T;
            T4 = T3 * T;
            T05 = sqrt( T );
            if( !j )
                Tst = (double)dc[q].TCst + dT;
            else  Tst = (double)dc[q].TCint[j] + dT;  // Bugfix DAK 8.09.00  + dT
            Tst2 = Tst * Tst;
            Tst3 = Tst2 * Tst;
            Tst4 = Tst3 * Tst;
            Tst05 = sqrt( Tst );
            T_Tst = T - Tst;
            Ts2 = T_Tst * T_Tst;
            TT = T / Tst;

            if( j && dc[q].Nft && (double)dc[q].FtP[jf] <= Tst-dT )
            {   // Adding parameters of phase transition
                if( !IsFloatEmpty( dc[q].FtP[dc[q].Nft+jf] ))  /* dS */
                    aW.twp->S += dc[q].FtP[dc[q].Nft+jf];
                if( !IsFloatEmpty( dc[q].FtP[dc[q].Nft*2+jf] ))  /* dH */
                    aW.twp->H += (double)dc[q].FtP[dc[q].Nft*2+jf];
                if( !IsFloatEmpty( dc[q].FtP[dc[q].Nft*3+jf] ))  /* dV */
                    aW.twp->V += dc[q].FtP[dc[q].Nft*3+jf];
                /* More to be added ? */
                jf++;
            }
            /*     if(j) */
            aW.twp->G -= aW.twp->S * T_Tst;
            /* aW.twp->Cp = 0.; */
            for( i=0; i< /*MAXCPCOEF*/aObj[o_dccp].GetN(); i++ ) // fix 08.09.00
            {
	        a1 = dc[q].Cp[i*dc[q].NeCp+j];
                if( IsFloatEmpty( a1 ) || !a1 )
		    ac[i] = 0.0;
		else ac[i] = (double)a1;
            }
            aW.twp->S  += ( ac[0] * log( TT ) + ac[1] * T_Tst + ac[2] * ( 1./Tst2 - 1./T2 ) / 2.
	          + ac[3] * 2. * ( 1./Tst05 - 1./T05 ) + ac[4] * ( T2 - Tst2 ) / 2.
		  + ac[5] * ( T3 - Tst3 ) / 3. + ac[6] * ( T4 - Tst4 ) / 4.
		  + ac[7] * ( 1./ Tst3 - 1./ T3 ) / 3. + ac[8] * (1. / Tst - 1. / T )
		  + ac[9] * 2.* ( T05 - Tst05 ) );

	    aW.twp->G -= ( ac[0] * ( T * log( TT ) - T_Tst ) + ac[1] * Ts2 / 2. + ac[2] * Ts2 / T / Tst2 / 2.
	          + ac[3] * 2. * (T05 - Tst05)*(T05 - Tst05) / Tst05 + ac[4] * ( T3 + 2.*Tst3 - 3.* T * Tst2 ) / 6.
		  + ac[5] * ( T4 + 3.*Tst4 - 4.*T * Tst3 ) / 12. + ac[6] * ( T4*T + 4.*Tst4*Tst - 5.*T*Tst4 ) / 20.
		  + ac[7] * ( Tst3 - 3.* T2 * Tst + 2.*T3 ) / 6./ T2 / Tst3 + ac[8] * ( TT - 1. - log( TT ))
		  + ac[9] * 2.* ( 2.* T * T05 - 3.* T * Tst05 + Tst * Tst05 ) / 3. );

	    aW.twp->H += ( ac[0] * T_Tst + ac[1] * ( T2 - Tst2 ) / 2. + ac[2] * ( 1./Tst - 1./T )
	          + ac[3] * 2. * ( T05 - Tst05 ) + ac[4] * ( T3 - Tst3 ) / 3.
		  + ac[5] * ( T4 - Tst4 ) / 4. + ac[6] * ( T4 * T - Tst4 * Tst ) / 5
		  + ac[7] * ( 1./ Tst2 - 1./ T2 ) / 2. + ac[8] * log( TT )
		  + ac[9] * 2.* ( T * T05 - Tst * Tst05 ) / 3.  );

        }
    T=aW.twp->T;
NEXT:
	// Holland-Powell phases with Landau transition
    if( CE == CTM_CHP )  // Rewritten 11.11.04 using input by Th.Wagner & D.Dolejs
    {
        double Qq, Tcr, Tcr0, Smax, Vmax, k298, kT, a0, p, pp, ivdp, idvdtdp,
               Q298, v_bis, smq;
        // Parameters of lambda-transition
        Tcr0 =  (double)dc[q].TCr;   // given in centigrade
        Smax = (double)dc[q].Smax;
        k298 = (double)dc[q].Comp; // This is the bulk modulus k in kbar at 298 K!
        p = aW.twp->P;             // in bars
        if( IsFloatEmpty( dc[q].Comp ))
           k298 = 0.;
        a0 = (double)dc[q].Expa; // This is the a parameter (at one bar) in 1/K !
        if( IsFloatEmpty( dc[q].Expa ))
           a0 = 0.;

        if( !IsFloatEmpty( dc[q].TCr ) && !IsFloatEmpty( dc[q].Smax ))
        {
            Vmax = (double)dc[q].Der;
            if( IsFloatEmpty( dc[q].Der ))
            {
               Vmax = 0.0; Tcr = Tcr0;
            }
            else
               Tcr = Tcr0 + Vmax/Smax * p;
            Tcr += dT;
            Tcr0 += dT;  //  dT is a constant equal 273.15
            Q298 = pow( 1.- Tst/Tcr0, 0.25 );
            if( T<Tcr )
               Qq = pow( 1.- T/Tcr, 0.25 );
            else
               Qq = 0.;
            pp = p/1000.; // p is in bar, pp in kbar, kT and k298 in kbar
            kT = k298 * (1 - 1.5e-4 * ( T-Tst ));
            v_bis = Vmax * Q298*Q298 * ( 1.+a0*(T-Tst) -
                     20.*a0*( sqrt(T) - sqrt(Tst) ));
            ivdp = v_bis * kT / 3. * ( pow(1.+4.*pp/kT, 0.75) - 1. ) * 1000.;
            idvdtdp = Vmax*p*(a0-10.*a0/sqrt(T));
            smq = Smax * Tcr0 *( Q298*Q298 - pow(Q298,6.)/3. );

        // incrementing molar properties
            aW.twp->S += Smax * ( Q298*Q298 - Qq*Qq ) - idvdtdp;
            aW.twp->G += smq - T * Smax * Q298*Q298 + ivdp
                         + Smax * ((T-Tcr)*Qq*Qq + Tcr*pow(Qq,6.)/3.);
            aW.twp->H += smq - Smax*Tcr*( Qq*Qq - pow(Qq,6.)/3. )
                         + ivdp - T*idvdtdp;   // check this!
            aW.twp->V = v_bis;    // in J/bar

            if( T<Tcr )  // Cp is corrected at subcritical T only
                aW.twp->Cp += T * Smax / 2. / sqrt( Tcr ) / sqrt( Tcr-T );
        }
    }

    if( CV != CPM_AKI )
       calc_voldp( q, p, CE, CV );

}

//--------------------------------------------------------------------//
// Calculation of V*dP correction to standard properties of
// condensed components
// Updated and bugfixed by DAK on 14 Dec 2000  PSI
void
TDComp::calc_voldp( int q, int /*p*/, int /*CE*/, int CV )
{
    double a/*=0.*/, T, Vst, Tst, Pst, P, P_Pst, T_Tst,
          Ts2, /*dT,*/ VP, VT,
          aC/*=0.*/, aE/*=0.*/, kap, T05, Tst05;
    int i;

    /* Init numbers - volume Vst must be in J/bar ! */
    Pst = (double)dc[q].Pst;
    Vst = (double)dc[q].mVs[0];
    if( IsFloatEmpty( dc[q].mVs[0] ))
        Vst = 0.;
//    dT = TK_DELTA; //TC = aW.twp->TC;
    T = aW.twp->T;
    Tst = aW.twp->Tst;
    P = aW.twp->P;
    P_Pst = aW.twp->P - Pst;
    T_Tst = T - Tst;
    Ts2 = T_Tst * T_Tst;
    T05 = sqrt(T);
    Tst05 = sqrt(Tst);

    if( CV == CPM_CON )
    { // Molar volume assumed independent of T and P
      aW.twp->V = Vst;
      aW.twp->G += Vst * P_Pst;
      aW.twp->H += P_Pst * Vst;
    }
    else if( CV == CPM_CEH )
    { /* Calculation over compressibility/expandability according to
         Holland & Powell, 1998 */
      // reading and checking the coeffs
      aC = (double)dc[q].Comp; // This is the bulk modulus k in kbar at 298 K!
      // aC *= 1000.;   Check! conversion from kbar to bar
      if( IsFloatEmpty( dc[q].Comp ))
          aC = 0.;
      aE = (double)dc[q].Expa; // This is the a parameter in 1/K !
      if( IsFloatEmpty( dc[q].Expa ))
          aE = 0.;
      if(( fabs( P_Pst ) > PRESSURE_PREC || fabs( T_Tst ) > TEMPER_PREC )
           && fabs(Vst) > 1e-9 && fabs(aC) > 1e-9 && fabs(aE) > 1e-9 )
                             // No zero molar volume !
      {
    	  // Inserted 01.07.03 - calculations acc. to Holland&Powell, 1998
         double VT, PP = P*0.001; // P seems to be in kbar in HP98 eqns!
         // Coeff. of thermal expansion at T
         aW.twp->Alp = aE * (1. - 10./T05 );
         // Bulk modulus at T
         kap = aC * ( 1. - 1.5e-4*T_Tst );
         // Compressibility at T  - check !
         aW.twp->Bet = 1./PP * (1. - pow( (1.- 4.*PP/(kap + 4.*PP )), 0.25 ));  // to check (compressibility)

         // Molar properties
         VT = Vst *(1.+ aE*T_Tst - 20.*aE*(T05 - Tst05));
         aW.twp->G += 1./3.* VT * kap * 1000. * (pow((1.+4.*PP/kap),0.75 )- 1.); // sign in pow((1-4... fixed 21.10.2004
         aW.twp->S -= Vst * P * ( aE - 10.*aE / T05 );
         aW.twp->H += -T * Vst * P * ( aE - 10.*aE / T05 )
              + 1./3. * VT * kap * 1000. * ( pow((1.+4.*PP/kap),0.75 ) - 1.);  // sign in pow((1-4... fixed 21.10.2004
         aW.twp->V += VT * pow( (1.- 4.*PP/(kap + 4.*PP )), 0.25 );
      }
      else {  // aE and aC are not provided
    	  // Warning here?
        // Molar volume assumed independent of T and P
        aW.twp->V = Vst;
        aW.twp->G += Vst * P_Pst;
        aW.twp->H += P_Pst * Vst;
      }
    }
    else if( (CV == CPM_VKE || CV == CPM_VBE ) && dc[q].Vt )
    {  /* Vm = f(T,P) equations */
        aC = 0.;
        aE = 0.;

//  Volume equation (Berman)
//  V(P,T)/V(1,298) = 1 + V1(T-298) + V2(T-298)**2 + V3(P-1) + V4(P-1)**2
// The same as usual one but a2=0 and 1 is added instead of a2: to Check!

        for( i=0; i<5; i++ )
        { // Finding compressibility and expandability from V=f(T,P) coeffs
            a = (double)dc[q].Vt[i];
            if( IsDoubleEmpty( a ) || !a ) continue;
            switch( i ) /* running over coeffs Vp(T,P) */
            {
            case 0:
                aE += a;
                break;
            case 1:
                aE += a * 2. * T_Tst;
                break;
            case 2:
                aE += a * 3. * T_Tst * T_Tst;
                break;
            case 3:
                aC -= a;
                break;
            case 4:
              aC -= a * 2. * P;
              // aC -= a * 2. * P_Pst;
                break;
            }
        }
        aW.twp->V = Vst;                 // added by KD 22.11.04
        if( fabs( P_Pst ) > PRESSURE_PREC || fabs( T_Tst ) > TEMPER_PREC )
        { /* can be calculated */
            VP = Vst * P_Pst;
            VT = Vst * T_Tst;
            aW.twp->G += VP;
            aW.twp->H += VP;
            for( i=0; i<MAXVTCOEF; i++ )
            { /* Over V(T,P) coeffs */
                a = (double)dc[q].Vt[i];
                if( IsDoubleEmpty( a ) || !a ) continue;
                switch( i ) /* coeffs of Vp(T) */
                {
                case 0:
                    aW.twp->V += a * VT;
                    aW.twp->S  -= a * VP;
                    aW.twp->G += a * T_Tst * VP;
                    aW.twp->H -= a * Tst * VP;
                    break;
                case 1:
                    aW.twp->V += a * VT * T_Tst;
                    aW.twp->S  -= a * VP * 2. * T_Tst;
                    aW.twp->G += a * VP * Ts2;
                    aW.twp->H -= a * VP * ( T + Tst ) * T_Tst;
                    break;
                case 2:
                    aW.twp->V += a * VT * Ts2;
                    aW.twp->S  -= a * VP * 3.* Ts2;
                    aW.twp->G += a * VP * Ts2 * T_Tst;
                    aW.twp->H += a * VP * Ts2 * ( 2.*T + Tst );
                    break;
                case 3:
                    aW.twp->V += a * VP;
                    aW.twp->G += a * VP * P_Pst / 2.;
                    aW.twp->H += a * VP * P_Pst / 2.;
                    break;
                case 4:
                    aW.twp->V += a * VP * P_Pst;
                    aW.twp->G += a * VP * ( P_Pst*P_Pst ) / 3.;
                    aW.twp->H += a * VP * ( P_Pst*P_Pst ) / 3.;
                    break;
                }
            }
        }
        aW.twp->Alp = aC;
        aW.twp->Bet = aE;
        /*       dc[q].Comp = (float)aW.twp->Alp;
                 dc[q].Expa = (float)aW.twp->Bet;  */
    }
    else if( CV == CPM_VBM && dc[q].ODc )  /* Code 'B' - Birch-Murnaghan */
    { // Inserted 04.04.2003 M.Gottschalk and KD
      if( fabs( P_Pst ) > PRESSURE_PREC || fabs( T_Tst ) > TEMPER_PREC )
      {
         double VV0=0.0, GG0=0.0, HH0=0.0, SS0=0.0, aC=0.0, aE=0.0;

         BirchMurnaghan( 0.1*Pst, 0.1*P, Tst, T, Vst*10., dc[q].ODc,
                         VV0, aC, aE, GG0, HH0, SS0 );
         // increments to V, G, H, S
         aW.twp->V += Vst+VV0/10.;    // fixed by KD on 22.11.04
//         aW.twp->V += VV0/10.;
         aW.twp->S += SS0;
         aW.twp->G += GG0;
         aW.twp->H += HH0;
         aW.twp->Alp = aC;
         aW.twp->Bet = aE;
      }
   }  // End of Birch-Murnaghan section
}


//---------------------------------------------------------------------------
// Calculations related to Burch-Murnaghan (1947) equation (12 coeffs)
// Written in fortran by M.Gottschalk, GFZ Potsdam
// Translated to C/C++ by D.Kulik, PSI LES, 08 April 2003
//
// calculate the volume integral
double TDComp::BM_IntVol(double P, double Pref, double vt, double vpt,
                         double kt0, double kp, double kpp)
{
   double vt23, pint;

   vt23 = pow( vt/vpt, 2./3. );

   pint = (-3.*kt0*vt*(6.*(199. - 75.*kp + 9.*kp*kp + 9.*kpp*kt0)*vpt*vt*
          pow( vt/vpt, 1./3. ) +
          vt*vt*(-668. + kp*(276. - 63.*vt23) + 9*kp*kp*(-4. + vt23) +
          9.*kpp*kt0*(-4. + vt23) + 143.*vt23) +
          vpt*vpt*(287. + kp*kp*(9. - 36.*vt23) +
          9.*kpp*kt0*(1. - 4.*vt23) - 956.*vt23 + kp*(-87. + 324.*vt23))
          ))/(128.*vpt*vpt);

// return vdP
   return -Pref*vt+P*vpt-pint;
}


//-----------------------------------------------------------------------
// calculate the volume at P and T
//
double TDComp::BM_Volume( double P, double vt, double kt0, double kp,
                         double kpp, double vstart)
{
      double  veq, vv, vvnew, vvold, vt23, dveq;
      int i=0;

      vv    = vstart;
      vvold = vv;
      vvnew = vvold + 1.;

// Newton iteration (max. 50 iterations)
  while ( fabs(vvnew-vvold) > 1e-10 && i++ < 50 )
  {
        vt23 = pow( vt/vv, 2./3.);
        veq =  3./2.*kt0*(1.+3./4.*(kp-4.)*(vt23-1.)
             +3./8.*(143./9.-7.*kp+kp*kp+kpp*kt0)*(vt23-1.)*(vt23-1.))
             *(vt23-1.)*pow( vt/vv, 5./3. ) - P;
        dveq = (kt0*vt*(vt*vt*(4509. + kp*kp*(243. - 99.*vt23)
             + 9.*kpp*kt0*(27. - 11.*vt23) + 9.*kp*(-207. + 77.*vt23)
             - 1573.*vt23) - 21.*(199. - 75.*kp + 9.*kp*kp + 9.*kpp*kt0)
             *vt*pow( vt/vv, 1./3. ) *vv
             + 5.*(239. - 81.*kp + 9.*kp*kp + 9.*kpp*kt0)
             *vt23*vv*vv))/(48.*vv*vv*vv*vv);
        vvold = vv;
        vvnew = vv - veq/dveq/2.;
        vv    = vvnew;
   }
// returns volume
      return vv;
}


//------------------------------------------------------------------------
// calculate the integral vdP using the Birch-Murnaghan EOS
// this function will be incorporated into GEM-Selektor v.2.1.0 code
//
void
TDComp::BirchMurnaghan( double Pref, double P, double Tref, double T, double v0,
          const float *BMConst, double &vv, double &alpha, double &beta,
          double &dG, double &dH, double &dS )
{
   double vt, /*vpt,*/ a1, a2, a3,    //  a4, a5,
          kt00, kt0, dkdt, kp, kpp, vstart,
          /*Volume, IntVol, */ Pincr=0.01, Tincr=0.1,
          Pplus, Pminus, Tplus, Tminus,
          vPplus, vPminus, vTplus, vTminus,
          kt0Tplus, kt0Tminus, kppTplus, kppTminus,
          vtTplus, vtTminus, dGTplus, dGTminus;

   // v0   = BMConst(1) - in GEMS passed as a separate function parameter
    a1   = BMConst[0];
    a2   = BMConst[1];
    a3   = BMConst[2];
    // a4   = BMConst[3];  for future extensions
    // a5   = BMConst[4];
    kt00 = BMConst[5];
    dkdt = BMConst[6];
    kp   = BMConst[7];
    kpp  = BMConst[8];

    Pplus  = P + Pincr;
    Pminus = P - Pincr;
    Tplus  = T + Tincr;
    Tminus = T - Tincr;

    // calculate bulk modulus at T and its T increments
    kt0        = kt00 + dkdt*(T - Tref);
    kt0Tplus   = kt00 + dkdt*(Tplus - Tref);
    kt0Tminus  = kt00 + dkdt*(Tminus - Tref);

    // set kpp if not already defined and its T increments
    if ( fabs(kpp) < 1e-20 )
    {
      kpp       = -((35./9.+(3.-kp)*(4.-kp))/kt0);
      kppTplus  = -((35./9.+(3.-kp)*(4.-kp))/kt0Tplus);
      kppTminus = -((35./9.+(3.-kp)*(4.-kp))/kt0Tminus);
    }

    // calculate volume at T and Pref and its T increments
    vt = v0* exp( a1*(T-Tref)
         + a2/2.*(T*T-Tref*Tref)
         + a3*(-1./T+1./Tref) );
    vtTplus  =  v0* exp( a1*(Tplus-Tref)
         + a2/2.*(Tplus*Tplus-Tref*Tref)
         + a3*(-1./Tplus+1./Tref) );
    vtTminus =  v0* exp( a1*(Tminus-Tref)
         + a2/2.*(Tminus*Tminus-Tref*Tref)
         + a3*(-1./Tminus+1./Tref) );

    // calculate volume to start iterations
    vstart = vt* exp( -1./kt0*(P-Pref) );

    // calculate volumes at P and T and its increments
    vv      = BM_Volume(P, vt, kt0, kp, kpp, vstart);
    vPplus  = BM_Volume(Pplus, vt, kt0, kp, kpp, vv);
    vPminus = BM_Volume(Pminus, vt, kt0, kp, kpp, vv);
    vTplus  = BM_Volume(P, vtTplus, kt0Tplus, kp, kppTplus, vv);
    vTminus = BM_Volume(P, vtTminus,kt0Tminus,kp,kppTminus, vv);

    // calculate aplha and beta at P and T
    alpha =  1./vv*((vTplus-vTminus)/(2.*Tincr));
    beta  = -1./vv*((vPplus-vPminus)/(2.*Pincr));

    // calculate vdP (P-T correction of G ->  dG)
    dG = BM_IntVol(P, Pref, vt, vv, kt0, kp, kpp);

    // calculate d(vdP)/dT (dS)
    dGTplus  = BM_IntVol(P,Pref,vtTplus,vTplus,kt0Tplus,kp,kppTplus);
    dGTminus = BM_IntVol(P,Pref,vtTminus,vTminus,kt0Tminus,kp,kppTminus);
    dS = (dGTplus-dGTminus)/(2.*Tincr);

    // calculate dH
    dH = dG + T*dS;

}  // End of section for Birch-Murnaghan calculations


//-----------------------------------------------------------------
// calculation of partial molal volumes for aqueous nonelectrolyte species
// using EOS (Akinfiev and Diamond, 2003) provided by TW 30.01.2008
//
void TDComp::calc_akinf( int q, int p )
{
	double CaltoJ = cal_to_J;
	// calculate infinite dilution properties of aqueous species at T and P of interest
	double Pbar, Tk, rho, alp, bet, dalpT, dH0k;
	double Tr = 298.15, Pr = 1.0, R_CONST = 8.31451;
	double Gig, Sig, CPig, Gw, Sw, CPw;
	double Geos, Veos, Seos, CPeos, Heos;
	double Gids, Vids, Sids, CPids, Hids;
	double Geos298, Veos298, Seos298, CPeos298, Heos298;

	dH0k = (-182161.88);  // enthapy of ideal gas water at 0 K

	// Properties of water at Tr,Pr (25 deg C, 1 bar) from SUPCRT92 routines
	// adopted H2O ideal gas data from NIST-TRC database
	Gig = -228526.66;
	Sig = 188.72683;
	CPig = 33.58743;
	Gw = -237181.38;
	Sw = 69.92418;
	CPw = 75.36053;
	rho = 0.99706136;
	alp = 2.59426542e-4;
	bet = 4.52187717e-5;
	dalpT = 9.56485765e-6;

	Akinfiev_EOS_increments(Tr, Pr, Gig, Sig, CPig, Gw, Sw, CPw, rho, alp, bet, dalpT, q,
			           Geos298, Veos298, Seos298, CPeos298, Heos298 );

	// Getting back ideal gas properties corrected for T of interest
	// by substracting properties of hydration at Tr, Pr
    Gids = aW.twp->G -= Geos298;
    // Vids = aW.twp->V -= Veos298;
    Sids = aW.twp->S -= Seos298;
    // CPids = aW.twp->Cp -= CPeos298;
    CPids = aW.twp->Cp;
    Hids = aW.twp->H -= Heos298;

    // Properties of water at T,P of interest, modified 06.02.2008 (TW)
	Tk = aW.twp->T;
	Pbar = aW.twp->P;
	Gig = aWp.Gigw[aSpc.isat]*R_CONST*Tk + dH0k;  // converting normalized ideal gas values
	Sig = aWp.Sigw[aSpc.isat]*R_CONST;
	CPig = aWp.Cpigw[aSpc.isat]*R_CONST;
	Gw = aWp.Gw[aSpc.isat]*CaltoJ;
	Sw = aWp.Sw[aSpc.isat]*CaltoJ;
	CPw = aWp.Cpw[aSpc.isat]*CaltoJ;
	rho = aSta.Dens[aSpc.isat];
	alp = aWp.Alphaw[aSpc.isat];
	bet = aWp.Betaw[aSpc.isat];
	dalpT = aWp.dAldT[aSpc.isat];

	Akinfiev_EOS_increments(Tk, Pbar, Gig, Sig, CPig, Gw, Sw, CPw, rho, alp, bet, dalpT, q,
			           Geos, Veos, Seos, CPeos, Heos );

	// Getting dissolved gas properties corrected for T,P of interest
	// by adding properties of hydration at T,P
	aW.twp->G = Gids + Geos + Seos298*(Tk-298.15);  // S(T-Tr) corrected for dSh at Tr,Pr
	// aW.twp->V = Vids + Veos;
	aW.twp->V = Veos;
	aW.twp->S = Sids + Seos;
    aW.twp->Cp = CPids + CPeos;
    aW.twp->H = Hids + Heos;
}


// Implementation of calculation of hydration properties of nonelectrolytes (Akinfiev and Diamond, 2003)
void
TDComp::Akinfiev_EOS_increments(double Tk, double P, double Gig, double Sig, double CPig,
		double Gw, double Sw, double CPw, double rho, double alp, double bet, double dalpT, int q,
		double& Geos, double& Veos, double& Seos, double& CPeos, double& Heos )
{
	double derP, derT, der2T;
	double deltaB, lnKH, Nw, xi, aa, bb, RT;
	double fug, vol, drhoT, drhoP, d2rhoT, lnfug, Gres, Sres, CPres;
	const double RR = 83.1451, R_CONST = 8.31451;
	const double MW = 18.01528;

	RT = Tk*R_CONST;
	// EOS coefficients
	xi = (double)dc[q].CpFS[0];
	aa = (double)dc[q].CpFS[1];
	bb = (double)dc[q].CpFS[2];

	Gres = Gw-Gig;
	Sres = Sw-Sig;
	CPres = CPw - CPig;
	lnfug = Gres/RT;
	fug = exp(lnfug);
	vol = MW/rho;
	drhoT = - alp*rho;
	drhoP = bet*rho;
	d2rhoT = rho*(pow(alp,2.)-dalpT);

	// calculation of infinite dilution properties
	Nw = 1000./MW;
	deltaB = 0.5*(aa + bb*pow((1000./Tk),0.5));
	lnKH = (1.-xi)*log(fug) + xi*log(RR*Tk/MW*rho) + 2.*rho*deltaB;

	Geos = - R_CONST*Tk*log(Nw) + (1.-xi)*R_CONST*Tk*log(fug) + R_CONST*Tk*xi*log(RR*Tk/MW*rho)
		+ R_CONST*Tk*rho*(aa+bb*pow((1000./Tk),0.5));
	derP = aa*Tk*drhoP + bb*pow(10.,1.5)*pow(Tk,0.5)*drhoP;
	derT = aa*(rho+Tk*drhoT) + bb*(0.5*pow(10.,1.5)*pow(Tk,-0.5)*rho + pow(10.,1.5)*pow(Tk,0.5)*drhoT);
	der2T = aa*(2.*drhoT+Tk*d2rhoT) + bb*((-0.25)*pow(10.,1.5)*pow(Tk,-1.5)*rho
		+ pow(10.,1.5)*pow(Tk,-0.5)*drhoT + pow(10.,1.5)*pow(Tk,0.5)*d2rhoT);
	Veos = (vol*(1.-xi) + xi*RR*Tk*(1./rho)*drhoP + RR*derP)/10.;
	Seos = (1.-xi)*(Sres) + R_CONST*log(Nw) - R_CONST*(xi + xi*log(RR*Tk/MW) + xi*log(rho)
		+ xi*Tk*(1./rho)*drhoT) - R_CONST*derT;
	CPeos = (1.-xi)*(CPres) - R_CONST*(xi + 2.*xi*Tk*(1./rho)*drhoT
		- xi*pow(Tk,2.)/pow(rho,2.)*pow(drhoT,2.) + xi*pow(Tk,2.)/rho*d2rhoT) - R_CONST*Tk*der2T;
	Heos = Geos + Tk*Seos;
}


//--------------------------------------------------------------------
// Begin section converted from SUPCRT92
//
void
TDComp::calc_tpH2O( int pst )
{
    double CaltoJ = cal_to_J;

    ErrorIf( !aW.twp, GetName(), "E23DCrun: Undefined twp!" );
    aW.twp->T = aSta.Temp;
    aW.twp->P = aSta.Pres;
    aW.twp->V = dcp->mwt / aSta.Dens[pst] / 10.;  /* j/bar */
//    aW.twp->F = aWp.Aw[pst] * CaltoJ;
//    aW.twp->U = aWp.Uw[pst] * CaltoJ;
    aW.twp->H = aWp.Hw[pst] * CaltoJ;
    aW.twp->G = aWp.Gw[pst] * CaltoJ;
    aW.twp->S = aWp.Sw[pst] * CaltoJ;
    aW.twp->Cp = aWp.Cpw[pst] * CaltoJ;
//    aW.twp->Cv = aWp.Cvw[pst] * CaltoJ;
    aW.twp->Bet = aWp.Betaw[pst];
    aW.twp->Alp = aWp.Alphaw[pst];
}


//--------------------------------------------------------------------//
/* gShok2- Calc  g, dgdP, dgdT, d2gdT2 use equations in Shock et al. (1991)
* units:   T ................. C
*          D ................. g/cm**3
*          beta, dgdP ........ bars**(-1)
*          alpha, dgdT ....... K**(-1)
*          daldT, d2gdT2 ..... K**(-2)                            */
void TDComp::gShok2(double T, double P, double D, double beta, double alpha,
                    double daldT, double *g, double *dgdP, double *dgdT,
                    double *d2gdT2)
{
    double a, b, dgdD, /*dgdD2,*/ dadT, dadTT, dbdT, dbdTT, dDdT, dDdP,
    dDdTT, Db, dDbdT, dDbdTT, ft, dftdT, dftdTT, fp, dfpdP,
    f, dfdP, dfdT, d2fdT2, tempy;
    double C[6]  = {-0.2037662e+01,  0.5747000e-02, -0.6557892e-05,
                    0.6107361e+01, -0.1074377e-01,  0.1268348e-04 };
    double cC[3] = { 0.3666666e+02, -0.1504956e-09,  0.5017997e-13 };
    /*  SUBROUTINE gShok2(T,P,D,beta,alpha,daldT,g,dgdP,dgdT,d2gdT2) */
    //Sveta 19/02/2000 1-D < 0 pri D>1 => pow(-number, b) = -NaN0000 error
    ErrorIf( D >= 1.3, GetName(), "gShok2: D >= 1.3!" );

    double pw = fabs(1.0e0 - D); // insert Sveta 19/02/2000

    a  = C[0] + C[1] * T + C[2] * pow(T,2.);
    b  = C[3] + C[4] * T + C[5] * pow(T,2.);
    *g = a * pow( pw, b );

    dgdD   = - a * b * pow(pw,(b - 1.0e0));
    // dgdD2  =   a * b * (b - 1.0e0) * pow((1.0e0 - D),(b - 2.0e0));

    dadT   =   C[1] + 2.0e0 * C[2] * T;
    dadTT  =   2.0e0 * C[2];
    dbdT   =   C[4] + 2.0e0 * C[5] * T;
    dbdTT  =   2.0e0 * C[5];

    dDdT   = - D * alpha;
    dDdP   =   D * beta;
    dDdTT  = - D * (daldT - pow(alpha,2.));

//    Db     = pow((1.0e0 - D),b);  Fixed by DAK 01.11.00
    Db     = pow( pw , b );
    dDbdT  = -b * pow(pw,(b - 1.0e0)) * dDdT +
             log(pw) * Db  * dbdT;

    dDbdTT = -(b * pow(pw,(b-1.0e0)) * dDdTT +
               pow(pw,(b - 1.0e0)) * dDdT * dbdT + b * dDdT *
               ( -(b - 1.0e0) * pow(pw,(b - 2.0e0)) * dDdT +
                 log(pw) * pow(pw,(b - 1.0e0)) * dbdT)) +
             log(pw) * pow(pw,b) * dbdTT -
             pow(pw,b) * dbdT * dDdT / (1.0e0 - D) +
             log(pw) * dbdT * dDbdT;

    *dgdP   = dgdD * dDdP;
    *dgdT   = a * dDbdT + Db * dadT;
    *d2gdT2 = a * dDbdTT + 2.0e0 * dDbdT * dadT + Db * dadTT;

    if((T < 155.0) || (P > 1000.0) || (T > 355.0))
        return;

    tempy  = ((T - 155.0e0) / 300.0e0);
    ft     = pow(tempy,4.8) + cC[0] * pow(tempy,16.);

    dftdT  = 4.8e0 / 300.0e0 * pow(tempy,3.8) + 16.0e0 / 300.0e0 *
             cC[0] * pow(tempy,15.);

    dftdTT = 3.8e0 * 4.8e0 / (300.0e0 * 300.0e0) * pow(tempy,2.8) +
             15.0e0 * 16.0e0 / (300.0e0*300.0e0) * cC[0] * pow(tempy,14.);

    fp     = cC[1] * pow((1000.0e0 - P),3.) + cC[2] * pow((1000.0e0 - P),4.);

    dfpdP  = -3.0e0 * cC[1] * pow((1000.0e0 - P),2.) -
             4.0e0 * cC[2] * pow((1000.0e0 - P),3.);

    f      = ft * fp;
    dfdP   = ft * dfpdP;
    dfdT   = fp * dftdT;
    d2fdT2 = fp * dftdTT;

    *g      -= f;
    *dgdP   -= dfdP;
    *dgdT   -= dfdT;
    *d2gdT2 -= d2fdT2;

}


//--------------------------------------------------------------------//
/* gfun92 - Calculation: function g for (Tanger and Helgeson, 1988;
*           Shock et al.,1991) and it  частные производные (dgdP, dgdT,
*           d2gdT2)  for  TdegC,  Pbars  for algoritm :
*        geqn = 1 ...... use Tanger-Helgeson (1988) equations
*        geqn = 2 ...... use Shock et al. (1991) equations
*                        without the f(P,T) difference function
*        geqn = 3 ...... use Shock et al. (1991) equations
*                        with the f(P,T) difference function       */
void TDComp::gfun92(double TdegC, double Pbars, double Dgcm3, double betab,
                    double alphaK, double daldT, double *g, double *dgdP,
                    double *dgdT, double *d2gdT2, int geqn)
{
    double TMAX = 1000.0e0, PMAX = 5000.0e0, TOL=1.0e-4;
    /*  SUBROUTINE gfun92(TdegC,Pbars,Dgcm3,betab,alphaK,daldT,
                          g,dgdP,dgdT,d2gdT2,geqn)                 */
    /* initialize g and derivatives to zero */
    *g      = 0.0e0;
    *dgdP   = 0.0e0;
    *dgdT   = 0.0e0;
    *d2gdT2 = 0.0e0;
    if ((TdegC > TMAX+TOL) || (Pbars > PMAX+TOL))
        Error( GetName(), "E24DCrun: (TdegC > TMAX+TOL) || (Pbars > PMAX+TOL)!" );
    /* use Shock et al.(1991) equations with f(P,T) difference function */
    if (geqn == 3)
    {
        gShok2(TdegC,Pbars,Dgcm3,betab,alphaK,daldT,
               g,dgdP,dgdT,d2gdT2);
    }
    else
        Error( GetName(), "E25DCrun: gfun92()- error in HGK calculations" );
}


//--------------------------------------------------------------------//
/* omeg92 - calc the conventinal born coef(W)current aqueous species
*  and dwdP, dwdP, as functions  g, dgdP, dgdT, d2gdT2, wref and Z
*           see equat Johnson et al. (1991).  */
/* SUBROUTINE omeg92(g,dgdP,dgdT,d2gdT2,wref,Z,w,dwdP,dwdT,d2wdT2) */
/* COMMON /aqscon/ eta, theta, psi, anion, cation, gref */
void TDComp::omeg92(double g, double dgdP, double dgdT, double d2gdT2,
                    double wref, double ZZ, double *W, double *dwdP,
                    double *dwdT, double *d2wdT2)
{
    double reref, re, Z3, Z4;
    if( ZZ == 0.0e0 ) /* neutral aqueous species */
    {
        *W      = wref;
        *dwdP   = 0.0e0;
        *dwdT   = 0.0e0;
        *d2wdT2 = 0.0e0;
    }
    else             /* charged aqueous species */
    {
        reref = pow(ZZ,2.) / (wref / eta + ZZ / (3.082e0 + gref));
        re = reref + fabs(ZZ) * g;
        *W = eta * (pow(ZZ,2.) / re - ZZ / (3.082e0 + g));
        Z3 = fabs(pow(ZZ,3.)) / pow(re,2.) - ZZ / pow((3.082e0 + g),2.);
        Z4 = fabs(pow(ZZ,4.)) / pow(re,3.) - ZZ / pow((3.082e0 + g),3.);
        *dwdP   = -eta * Z3 * dgdP;
        *dwdT   = -eta * Z3 * dgdT;
        *d2wdT2 = 2.0e0 * eta * Z4 * pow(dgdT,2.) - eta * Z3 * d2gdT2;
    }
}


//--------------------------------------------------------------------//
// Calculation  t/d parametres for water solution (Res to TPWORK)
// (Using equations given  by  Tanger  and  Helgeson (1988), Shock et al.(1991),
// and  Johnson et al. (1991)).
void TDComp::calc_thkf( AQSREF& arf, double P, double T, double Dw, double betaw,
                        double alphaw, double daldTw, double Z, double Q,
                        double Y, double X, int geqn)
{
    double Vaqs,Saqs,Cpaqs,Haqs,Gaqs,VQterm,SYterm,CpXtrm,HYterm,GZterm,
    /*Gfaqs,Hfaqs,SPrTra,a[4],c[2],wref,chg,*/
    g,dgdP,dgdT,d2gdT2,W,dwdP,dwdT,d2wdT2, CaltoJ = cal_to_J;
    // Calc g, dgdP, dgdT, d2gdT2
    gfun92(T-273.15, P, Dw, betaw, alphaw, daldTw,&g, &dgdP, &dgdT,&d2gdT2, geqn);
    //Calc  W, dwdP, dwdT, d2wdT2
    omeg92(g, dgdP, dgdT, d2gdT2, arf.wref, arf.chg, &W, &dwdP, &dwdT, &d2wdT2);

    VQterm = 0.4184004e2 * (-W * Q + (-Z - 1.0e0) * dwdP);

    /* the leading constant converts cal/(mol*bar) -> cm3/mol */

    Vaqs   = 0.4184004e2 * (arf.A[0] + arf.A[1] / (psi + P) + arf.A[2] /
                            ( T - theta) + arf.A[3] / (psi + P) / (T - theta)) + VQterm;

    SYterm = W * Y - (-Z - 1.0e0) * dwdT - arf.wref * YPrTr;
    Saqs   = arf.SPrTra + arf.C[0] * log(T / Tref) - arf.C[1] / theta *
             (1.0e0 / (T - theta) - 1.0e0 / (Tref - theta) + (1.0e0 / theta) *
              log(Tref * (T - theta) / T / (Tref - theta))) + (arf.A[2] *
                      (P - Pref) + arf.A[3] * log((psi + P) / (psi+Pref))) *
             pow((1.0e0 / (T-theta)),2.) + SYterm;

    CpXtrm = W * T * X + 2.0e0 * T * Y * dwdT + T * (Z + 1.0e0) * d2wdT2;
    Cpaqs  = arf.C[0] + arf.C[1] / pow((T - theta),2.) - (2.0e0 * T /
             pow((T-theta),3.)) * (arf.A[2] * (P - Pref) + arf.A[3] *
                                   log((psi + P) / (psi + Pref))) + CpXtrm;

    HYterm = W * (-Z - 1.0e0) + W * T * Y - T * (-Z - 1.0e0) *dwdT -
             arf.wref * (-ZPrTr - 1.0e0) - arf.wref * Tref * YPrTr;
    Haqs   = arf.Hfaqs + arf.C[0] * (T - Tref) - arf.C[1] * (1.0e0 /
             (T - theta) - 1.0e0 / (Tref - theta)) + arf.A[0] * (P -
                     Pref) + arf.A[1] * log((psi + P) / (psi + Pref)) +
             (arf.A[2] * (P - Pref) + arf.A[3] * log((psi + P) /
                                                     (psi + Pref))) * ((2.0e0 * T - theta) / pow((T - theta),2.))
             + HYterm;

    GZterm = W * (-Z - 1.0e0) - arf.wref * (-ZPrTr - 1.0e0) + arf.wref *
             YPrTr * (T - Tref);
    Gaqs   = arf.Gfaqs - arf.SPrTra * (T - Tref) - arf.C[0] * (T * log(T /
             Tref) - T + Tref) + arf.A[0] * (P - Pref) + arf.A[1] *
             log((psi + P) / (psi + Pref)) - arf.C[1] * ((1.0e0 / (T - theta) -
                     1.0e0 / (Tref - theta)) * ((theta - T) / theta) - T / pow(theta,2.)
                     * log((Tref * (T - theta)) / (T * (Tref - theta)))) + (1.0e0 /
                             (T - theta)) * (arf.A[2] * (P - Pref) + arf.A[3] * log((psi + P) /
                                             (psi + Pref))) + GZterm;
    // GZterm = W * (-Z - 1.0e0);

    aW.twp->P   = P;
    aW.twp->TC  = T-C_to_K;
    aW.twp->V  = Vaqs / 10.;       /* J/bar */
    aW.twp->S   = Saqs * CaltoJ;
    aW.twp->G   = Gaqs * CaltoJ;
    aW.twp->H   = Haqs * CaltoJ;
    /*
      aW.twp->F   =
      aW.twp->U   =
    */
    aW.twp->Cp  = Cpaqs * CaltoJ;
    /*
      aW.twp->Cv  =
      aW.twp->Bet =
      aW.twp->Alp =
    */
    aW.twp->gfun = g;  // solvent g-function - passed for b_gamma=f(T,P) 07.06.05
}


//--------------------------------------------------------------------//
// Calculation  t/d parametres for water solution (Res to TPWORK)
void TDComp::calc_tphkf( int q, int /*p*/ )
{
    int /*iRet= 0,*/ i;
    double TK, JtoCal = 1/cal_to_J;

    AQSREF arf;
    ErrorIf(!dc[q].HKFc, GetName(),
       "E26DCrun: This DComp record does not have HKF EoS coefficients!");
    arf.Gfaqs  = dc[q].Gs[0] * JtoCal;
    arf.Hfaqs  = dc[q].Hs[0] * JtoCal;
    arf.SPrTra = dc[q].Ss[0] * JtoCal;
    /* Adjust magnitude for e-o-s coef. & omega */
    arf.A[0] = dc[q].HKFc[0];
    arf.A[1] = dc[q].HKFc[1];
    arf.A[2] = dc[q].HKFc[2];
    arf.A[3] = dc[q].HKFc[3];
    arf.C[0] = dc[q].HKFc[4];
    arf.C[1] = dc[q].HKFc[5];
    arf.wref = dc[q].HKFc[6];
    arf.chg = (int)dc[q].Zz;
    if ( aSpc.isat )
        i=1;        // below Psat curve (vapour field)?
    else
        i=0;        // Above Psat curve (liquid field)
    TK = TdegK(aSpc.it, aSta.Temp);     /* transform T to degK */
    /*Calc t/d param water solut for Temp (K), Pres (bar) */
    calc_thkf( arf, aSta.Pres, TK, aSta.Dens[i],
               aWp.Betaw[i], aWp.Alphaw[i], aWp.dAldT[i],
               aWp.ZBorn[i], aWp.QBorn[i], aWp.YBorn[i],
               aWp.XBorn[i], 3);
}
// ------------------ End of m_dcomp2.cpp ----------------------------
