//-------------------------------------------------------------------
// Id: gems/mods/m_dcomp2.cpp  version 2.0.0   2001
//
// Implementation of TDComp class, calculation functions
//
// Rewritten from C to C++ by S.Dmytriyeva  970207 modified 010904
// Copyright (C) 1995-2001 S.Dmytriyeva, D.Kulik, V.Sinitsyn
//
// This file is part of a GEM-Selektor library for thermodynamic
// modelling by Gibbs energy minimization
// Uses: GEM-Vizor GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://les.web.psi.ch/Software/GEMS-PSI for more information
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
    double a, T, TC, Vst, Tst, T_Tst, Ts2, TT, dT, T2, T3, T4,
    T05, Tst2, Tst3, Tst4, Tst05, qQ, Tcr, Smax;
    int k, i, j, jf;

    // iRet = ZERO;
    // default values
    Vst = (double)dc[q].mVs[0];
    if( IsDoubleEmpty( Vst ))
        Vst = 0.;
    dT = TK_DELTA;
    TC = aW.twp->TC;
    aW.twp->T =    T = aW.twp->TC + dT;
    aW.twp->Tst =  Tst = (double)dc[q].TCst + dT;

    aW.twp->S = dc[q].Ss[0];
    aW.twp->G = dc[q].Gs[0]; /* - aW.twp->S * (T-Tst); */
    aW.twp->H = dc[q].Hs[0];
    aW.twp->V = Vst;
    if(( dc[q].pstate[0] == CP_GAS || dc[q].pstate[0] == CP_GASI )
            && aW.twp->P > 0.0 )
    { /* molar volume from the ideal gas law */
        aW.twp->V = T / aW.twp->P * R_CONSTANT;
    }
    aW.twp->devG = (double)dc[q].Gs[1];
//    aW.twp->Cv = 0.;
    aW.twp->Cp = 0.;
    if( !dc[q].Cp )
    {
        aW.twp->Cp = dc[q].Cps[0];
        goto NEXT;
    }
    // get interval
    if( dc[q].TCint[0] > TC )
    {
        k=0;  /*iRet = 1;*/
    }
    else if( dc[q].TCint[2*dc[q].NeCp-1] <= TC )
    {
        k= dc[q].NeCp-1;   /*iRet = 1;*/
    }
    else for( j=0; j<dc[q].NeCp; j++ )
            if( dc[q].TCint[j] <= TC && dc[q].TCint[j+dc[q].NeCp] > TC )
            {
                k=j;
                break;
            }
    T2 = T * T;
    T3 = T2 * T;
    T4 = T3 * T;
    T05 = sqrt( T );
    for( i=0; i</*MAXCPCOEF*/aObj[o_dccp].GetN(); i++ )
    {  // Cp(t)
        float a1;
        a1 = /*(double)*/dc[q].Cp[i*dc[q].NeCp+k];
        if( IsFloatEmpty( a1 ) || !a1 ) continue;
        a = a1;
        switch( i )
        {
        case 0:
            aW.twp->Cp += a;
            break;
        case 1:
            aW.twp->Cp += a * T;
            break;
        case 2:
            aW.twp->Cp += a / T2;
            break;
        case 3:
            aW.twp->Cp += a / T05;
            break;
        case 4:
            aW.twp->Cp += a * T2;
            break;
        case 5:
            aW.twp->Cp += a * T3;
            break;
        case 6:
            aW.twp->Cp += a * T4;
            break;
        case 7:
            aW.twp->Cp += a / T3;
            break;
        case 8:
            aW.twp->Cp += a / T;
            break;
        case 9:
            aW.twp->Cp += a * T05;
            break;
            //        case 10: aW.twp->Cp += a * log( T ); break;
        }

    }
    if( fabs( T - Tst ) > TEMPER_PREC )
        for( j=0, jf=0; j<=k; j++ )
        {
            if( j == k )
                T = aW.twp->TC + dT;
            else T = dc[q].TCint[j+dc[q].NeCp] + dT;
            T2 = T * T;
            T3 = T2 * T;
            T4 = T3 * T;
            T05 = sqrt( T );
            if( !j )
                Tst = (double)dc[q].TCst + dT;
            else  Tst = dc[q].TCint[j] + dT;  // Bugfix DAK 8.09.00  + dT
            Tst2 = Tst * Tst;
            Tst3 = Tst2 * Tst;
            Tst4 = Tst3 * Tst;
            Tst05 = sqrt( Tst );
            T_Tst = T - Tst;
            Ts2 = T_Tst * T_Tst;
            TT = T / Tst;

            if( j && dc[q].Nft && dc[q].FtP[jf] <= Tst-dT )
            {
                if( !IsFloatEmpty( dc[q].FtP[dc[q].Nft+jf] ))  /* dS */
                    aW.twp->S += dc[q].FtP[dc[q].Nft+jf];
                if( !IsFloatEmpty( dc[q].FtP[dc[q].Nft*2+jf] ))  /* dH */
                    aW.twp->H += dc[q].FtP[dc[q].Nft*2+jf];
                if( !IsFloatEmpty( dc[q].FtP[dc[q].Nft*3+jf] ))  /* dV */
                    aW.twp->V += dc[q].FtP[dc[q].Nft*3+jf];
                /* Наклон фазового перехода */
                jf++;
            }
            /*     if(j) */
            aW.twp->G = aW.twp->G - aW.twp->S * T_Tst;
            /* aW.twp->Cp = 0.; */
            for( i=0; i< /*MAXCPCOEF*/aObj[o_dccp].GetN(); i++ ) // fix 08.09.00
            {
                a = (double)dc[q].Cp[i*dc[q].NeCp+j];
                if( IsDoubleEmpty( a ) || !a ) continue;
                switch( i )
                {
                case 0: /* aW.twp->Cp += a; */
                    aW.twp->S  += a * log( TT );
                    aW.twp->G -= a * ( T * log( TT ) - T_Tst );
                    aW.twp->H += a * T_Tst;
                    break;
                case 1: /* aW.twp->Cp += a * T; */
                    aW.twp->S  += a * T_Tst;
                    aW.twp->G -= a * Ts2 / 2.;
                    aW.twp->H += a * ( T2 - Tst2 ) / 2.;
                    break;
                case 2: /* aW.twp->Cp += a / T2; */
                    aW.twp->S  += a * ( 1./Tst2 - 1./T2 ) / 2.;
                    aW.twp->G -= a * Ts2 / T / Tst2 / 2.;
                    aW.twp->H += a * ( 1./Tst - 1./T );
                    break;
                case 3: /* aW.twp->Cp += a / T05; */
                    aW.twp->S  += a * 2. * ( 1./Tst05 - 1./T05 );
                    aW.twp->G -= a * 2. * (T05 - Tst05)*(T05 - Tst05) / Tst05;
                    aW.twp->H += a * 2. * ( T05 - Tst05 );
                    break;
                case 4: /* aW.twp->Cp += a * T2; */
                    aW.twp->S  += a * ( T2 - Tst2 ) / 2.;
                    aW.twp->G -= a * ( T3 + 2.*Tst3 - 3.* T * Tst2 ) / 6.;
                    aW.twp->H += a * ( T3 - Tst3 ) / 3.;
                    break;
                case 5: /* aW.twp->Cp += a * T3; */
                    aW.twp->S  += a * ( T3 - Tst3 ) / 3.;
                    aW.twp->G -= a * ( T4 + 3.*Tst4 - 4.*T * Tst3 ) / 12.;
                    aW.twp->H += a * ( T4 - Tst4 ) / 4.;
                    break;
                case 6: /* aW.twp->Cp += a * T4; */
                    aW.twp->S  += a * ( T4 - Tst4 ) / 4.;
                    aW.twp->G -= a * ( T4*T + 4.*Tst4*Tst - 5.*T*Tst4 ) / 20.;
                    aW.twp->H += a * ( T4 * T - Tst4 * Tst ) / 5;
                    break;
                case 7: /* aW.twp->Cp += a / T3; */
                    aW.twp->S  += a * ( 1./ Tst3 - 1./ T3 ) / 3.;
                    aW.twp->G -= a * ( Tst3 - 3.* T2 * Tst + 2.*T3 ) /
                                 6./ T2 / Tst3;
                    aW.twp->H += a * ( 1./ Tst2 - 1./ T2 ) / 2.;
                    break;
                case 8: /* aW.twp->Cp += a / T; */
                    aW.twp->S  += a * (1. / Tst - 1. / T );
                    aW.twp->G -= a * ( TT - 1. - log( TT ));
                    aW.twp->H += a * log( TT );
                    break;
                case 9: /* aW.twp->Cp += a * T05; */
                    aW.twp->S  += a * 2.* ( T05 - Tst05 );
                    aW.twp->G -= a * 2.* ( 2.* T * T05 - 3.* T * Tst05 +
                                           Tst * Tst05 ) / 3.;
                    aW.twp->H += a * 2.* ( T * T05 - Tst * Tst05 ) / 3.;
                    break;
                    //         case 10: break;  /*Do it! */
                }

            }
        }
    /*     if( k > 1 )
         {  */
    T=aW.twp->T; // Tst = aW.twp->Tst;
    //T2 = T * T;  //T3 = T2 * T;   //T4 = T3 * T;   //T05 = sqrt( T );
    // Tst2 = Tst * Tst;  //Tst3 = Tst2 * Tst;
    //Tst4 = Tst3 * Tst; // Tst05 = sqrt( Tst );
    // T_Tst = T - Tst;   //Ts2 = T_Tst * T_Tst;  // TT = T / Tst;
    /*     }    */
NEXT:
    if( CE == CTM_CHP )
    {
        Tcr =  (double)dc[q].TCr;
        Smax = (double)dc[q].Smax;
        if( !IsDoubleEmpty( Tcr ) && !IsDoubleEmpty( Smax ))
        {
            Tcr += dT;
            if( T<Tcr )
            {
                qQ = 1. - T / Tcr;
                a = 2.* Tcr * Smax;
                aW.twp->Cp += T * Smax / 2. / sqrt( Tcr ) / sqrt( Tcr-T );
                aW.twp->S  += Smax * ( 1.- sqrt( qQ ));
                aW.twp->G -= Smax *( Tcr-T )*sqrt( qQ ) - a * qQ * sqrt( qQ ) / 6.;
                aW.twp->H += a * ( qQ* sqrt( qQ ) /6. - sqrt( qQ ) / 2. + 1./3. );
            }
            else
            {
                aW.twp->S  += Smax;
                aW.twp->G -= Smax*(T-Tcr);
                aW.twp->H += 2.* Smax * Tcr / 3.;
            }
        }
    }

    calc_voldp( q, p, CE, CV );
}

//--------------------------------------------------------------------//
// Calculation of V*dP correction to standard properties of
// condensed components
// Updated and bugfixed by DAK on 14 Dec 2000  PSI
void
TDComp::calc_voldp( int q, int /*p*/, int /*CE*/, int CV )
{
    double a, T, Vst, Tst, Pst, P_Pst, T_Tst, Ts2, dT, VP, VT, aC, aE;
    int i;

    /* Init numbers - volume Vst must be in J/bar ! */
    Pst = (double)dc[q].Pst;
    Vst = (double)dc[q].mVs[0];
    if( IsDoubleEmpty( Vst ))
        Vst = 0.;
    dT = TK_DELTA; //TC = aW.twp->TC;
    aW.twp->T =    T = aW.twp->TC + dT;
    aW.twp->Tst =  Tst = (double)dc[q].TCst + dT;
    aC = 0.; aE = 0.;
    P_Pst = aW.twp->P - Pst;
    T_Tst = T - Tst;
    Ts2 = T_Tst * T_Tst;

    if( CV == CPM_CEH || CV == CPM_CON )
    { /* Calculation over compressibility/expandability according to
         Holland & Powell, 1990 */
        if( CV == CPM_CEH )
        {  // reading and checking the coeffs
           aC = (double)dc[q].Comp;
           if( IsDoubleEmpty( aC ))
              aC = 0.;
           aE = (double)dc[q].Expa;
           if( IsDoubleEmpty( aE ))
              aE = 0.;
        }
        if(( fabs( P_Pst ) > PRESSURE_PREC || fabs( T_Tst ) > TEMPER_PREC )
             && fabs(Vst) > 1e-10 )  // No zero molar volume !
        {
            aE /= Vst;  // ?  check
            aC /= Vst;  // ?  check
//            aW.twp->V  = Vst * ( 1.+ aE * T_Tst ) * ( 1.- aC * aW.twp->P );
//            aW.twp->S  -= Vst * aW.twp->P * aE * (1. - aC * aW.twp->P / 2.);
//            aW.twp->G+=Vst*(1.+ aE * T_Tst)*( aW.twp->P-aC*aW.twp->P*aW.twp->P/2.);
//            aW.twp->H += aW.twp->P*Vst*(1.-aC*aW.twp->P/2.)*(1.+aE*T_Tst - aE );
            aW.twp->V = Vst * ( 1. + aE * T_Tst ) * ( 1.- aC * P_Pst );
            aW.twp->S -= Vst * P_Pst * aE * (1. - aC * P_Pst / 2.);
            aW.twp->G += Vst * (1. + aE * T_Tst)*( P_Pst - aC * P_Pst * P_Pst/2.);
            aW.twp->H += P_Pst * Vst * (1.-aC * P_Pst / 2.)*(1.+aE * T_Tst - aE );
        }
    }
    else if( (CV == CPM_VKE || CV == CPM_VBE || CV == CPM_VBL) && dc[q].Vt )
    {  /* Vm = f(T,P) equations */
        aC = 0.;
        aE = 0.;
//
//  VOLUME EQUATION (BERMAN):
//  V(P,T)/V(1,298) = 1 + V1(T-298) + V2(T-298)**2 + V3(P-1) + V4(P-1)**2
// The same as usual one but a2=0 and 1 is added instead of a2: to Check!
//
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
//              aC -= a * 2. * aW.twp->P;
                aC -= a * 2. * P_Pst;
                break;
            }
        }
        if( fabs( P_Pst ) > PRESSURE_PREC || fabs( T_Tst ) > TEMPER_PREC )
        { /* can calculate */
//          P_Pst = aW.twp->P - Pst;
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
                    aW.twp->G += a * VP * ( aW.twp->P*aW.twp->P - Pst*Pst ) / 3.;
                    aW.twp->H += a * VP * ( aW.twp->P*aW.twp->P - Pst*Pst ) / 3.;
                    break;                      // Check why not P_Pst*P_Pst
                }
            }
        }
        aW.twp->Alp = aC;
        aW.twp->Bet = aE;
        /*       dc[q].Comp = (float)aW.twp->Alp;
                 dc[q].Expa = (float)aW.twp->Bet;  */
    }
//    if( fabs( aC ) > 1e-18 )
//        aW.twp->Cv = aW.twp->Cp - T* aW.twp->V * aE * aE / aC;
// Finished 
}

//--------------------------------------------------------------------//
void
TDComp::calc_tpH2O( int pst )
{
    double CaltoJ = cal_to_J;

    ErrorIf( !aW.twp, GetName(), "Udefined twp!" );
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
*           Shock et al.,1991) and it  ўрёЄэ√х яЁюшчтюфэ√х (dgdP, dgdT,
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
        Error( GetName(), "(TdegC > TMAX+TOL) || (Pbars > PMAX+TOL)!" );
    /* use Shock et al.(1991) equations with f(P,T) difference function */
    if (geqn == 3)
    {
        gShok2(TdegC,Pbars,Dgcm3,betab,alphaK,daldT,
               g,dgdP,dgdT,d2gdT2);
    }
    else
        Error( GetName(), "gfun92 - geqn!=3 no cod" );
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
}

//--------------------------------------------------------------------//
// Calculation  t/d parametres for water solution (Res to TPWORK)
void TDComp::calc_tphkf( int q, int /*p*/ )
{
    int /*iRet= 0,*/ i;
    double TK, JtoCal = 1/cal_to_J;

    AQSREF arf;
    ErrorIf(!dc[q].HKFc, GetName(),
            "Record dc dosn`t have HKF coefficients!");
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
        i=1;        /* обеспечивает выбор св-в воды-жидкости  */
    else             /* при расчетах по кнп                    */
        i=0;        /* выбор св-в воды при расчетах в однофаз. обл. */
    TK = TdegK(aSpc.it, aSta.Temp);     /* transform T to degK */
    /*Calc t/d param water solut for Temp (K), Pres (bar) */
    calc_thkf( arf, aSta.Pres, TK, aSta.Dens[i],
               aWp.Betaw[i], aWp.Alphaw[i], aWp.dAldT[i],
               aWp.ZBorn[i], aWp.QBorn[i], aWp.YBorn[i],
               aWp.XBorn[i], 3);
}
// ------------------ End of m_dcomp2.cpp ----------------------------
