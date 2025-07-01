//-------------------------------------------------------------------
// $Id: s_supcrt2.cpp 1065 2008-05-16 12:46:59Z wagner $
//
// Copyright (C) 1992-2000  D.Kulik, S.Dmitrieva
//
// Implementation of TSupcrt class
//
// This file is part of a GEM-Selektor (GEMS) v.3.1.x program
// environment for thermodynamic modeling in geochemistry
// Uses: GEM-Selektor GUI GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the GPL v.3 license

//
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch;
//-------------------------------------------------------------------
//

#include <cmath>
#include "GEMS3K/verror.h"
#include "GEMS3K/v_detail.h"
#include "s_supcrt.h"

/* HGKcon - Constant parameters for the H2O equation of state given by
*           Haar,  Gallagher, & Kell (1984):  bp, bq = b(j), B(j) from
*           Table A.1, p.272; g1, g2, gf = alpha, beta, gamma  from eq
*           (A.2), p.272; g, ii, jj  = g(i), k(i), l(i) from eq (A.5),
*           p.272. Note that  tz < tcHGK.
*                 Tolerence limits required  in various real & inexact
*            comparisons are set and stored in COMMON /tolers/.     */


ACONST  ac_ = { /* wm, gascon, tz, aa, uref, sref*/
    0.1801520000e2, 0.46152200e0, 0.647073e3, 0.1e1,
    -0.4328455039e4, 0.76180802e1,  0., 0., 0.};

NCONST  nc_ ={ /* g(40), ii(40), jj(40), nc */
    {-0.53062968529023e3, 0.22744901424408e4, 0.78779333020687e3,
     -0.69830527374994e2, 0.17863832875422e5,-0.39514731563338e5,
     0.33803884280753e5,-0.13855050202703e5,-0.25637436613260e6,
     0.48212575981415e6,-0.34183016969660e6, 0.12223156417448e6,
     0.11797433655832e7,-0.21734810110373e7, 0.10829952168620e7,
     -0.25441998064049e6,-0.31377774947767e7, 0.52911910757704e7,
     -0.13802577177877e7,-0.25109914369001e6, 0.46561826115608e7,
     -0.72752773275387e7, 0.41774246148294e6, 0.14016358244614e7,
     -0.31555231392127e7, 0.47929666384584e7, 0.40912664781209e6,
     -0.13626369388386e7, 0.69625220862664e6,-0.10834900096447e7,
     -0.22722827401688e6, 0.38365486000660e6, 0.68833257944332e4,
     0.21757245522644e5,-0.26627944829770e4,-0.70730418082074e5,
     -0.22500000000000e0,-0.16800000000000e1, 0.5500000000000e-1,
     -.93000000000000e2},                             /* g[40]  */
    {0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3,
     4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 8, 8, 8, 8,
     2, 2, 0, 4, 2, 2, 2, 4},                        /* ii[40] */
    {2, 3, 5, 7, 2, 3, 5, 7, 2, 3, 5, 7, 2, 3, 5, 7,
     2, 3, 5, 7, 2, 3, 5, 7, 2, 3, 5, 7, 2, 3, 5, 7,
     1, 4, 4, 4, 0, 2, 0, 0},                        /* jj[40] */
    36};                                           /*  nc    */

ELLCON  el_ = { /* g1, g2, gf */
    0.11e2, 0.44333333333333e2, 0.35e1, 0., 0., 0., 0., 0., 0.};

BCONST  bc_ = { /* bp(10), bq(10) */
    {0.7478629e0,-0.3540782e0,  0.0e0, 0.0, 0.7159876e-2,
     0.0e0,      -0.3528426e-2, 0.0e0, 0.0, 0.0},    /* bp[10] */
    {0.11278334e1, 0.0e0, -0.5944001e0, -0.5010996e1, 0.0e0,
     0.63684256e0, 0.0e0, 0.0, 0.0, 0.0}};           /* bq[10] */

ADDCON  ad_ = { /* atz(4), adz(4), aat(4), aad(4) */
    {0.64e3,  0.64e3,  0.6416e3, 0.27e3},   /* atz[4]  */
    {0.319e0, 0.319e0, 0.319e0,  0.155e1},  /* adz[4]  */
    {0.2e5,   0.2e5,   0.4e5,    0.25e2},   /* aat[4]  */
    {0.34e2,  0.4e2,   0.3e2,    0.105e4} }; /* aad[4]  */

HGK_CRT hc_ = { /* tcHGK, dcHGK, pcHGK */
    0.647126e3, 0.322e3, 0.22055e2};

TOLERS  to_ = { /* TTOL, PTOL, DTOL, XTOL, EXPTOL, FPTOL */
    1.0e-6, 1.0e-6, 1.0e-9, 1.0e-5, -673.5e0, 1.0e-7};

HGK_BND hb_ = { /* Ttop, Tbtm, Ptop, Pbtm, Dtop, Dbtm */
    2.25e3,-2.0e1, 3.0e4, 1.0e-3,
    0.138074666423686955066817336896e1,
    0.858745555396173972667420987465e-7};

LIQICE  li_ = { /* sDli1, sPli1, sDli37, sPli37, sDIB30,
                       Tli13, Pli13, Dli13, TnIB30, DnIB30 */
    -0.584797401732178547634910059828e-2,
    -0.138180804975562958027981345769e3,
    0.183244000000000000000000000007e-2,
    0.174536874999999999999999999995e3,
    -0.168375439429928741092636579574e-3,
    -0.15e2,
    0.20741e4,
    0.108755631570602617113573577945e1,
    0.145e3,
    0.102631640581853166397515716306e1};

TPOINT  tt_ = {/* Utri,Stri,Htri,Atri,Gtri
                       Ttr, Ptripl, Dltrip, Dvtrip */
    0e0, 0e0, 0e0, 0.0e0, 0e0,
    2.7316e2,
    0.611731677193563186622762580414e-2,
    0.999778211030936587977889295063e0,
    0.485467583448287303988319166423e-5};

CRITS   cr_ = { /* Tc, rhoC, Pc, Pcon, Ucon, Scon, dPcon */
    647.067e0,  322.778e0,  22.046e0,
    0.034070660379837018423130834983e0,
    22046.0e0,
    0.034070660379837018423130834983e3,
    0.000000327018783663660700780197e0};

COEFS   co_ = { /* a(20), q(20), x(11) */
    {-0.017762e0, 5.238000e0, 0.000000e0,-2.549150e1,
     6.844500e0, 0.325000e0, 1.440300e0, 0.000000e0,
     1.375700e0, 2.366660e1, 4.820000e0, 0.294200e0,
     -1.123260e1,-2.265470e1,-1.788760e1,-4.933200e0,
     1.109430391161019373812391218008e0,
     -1.981395981400671095301629432211e0,
     0.246912528778663959151808173743e0,
     -0.843411332867484343974055795059e0},      /* a[20] */

    {-0.006000e0,-0.003000e0, 0.000000e0, 6.470670e2,
     3.227780e2, 2.204600e1, 0.267000e0,-1.600000e0,
     0.491775937675717720291497417773e0, 0.108500e0,
     0.586534703230779473334597524774e0,
     -1.026243389120214352553706598564e0,
     0.612903225806451612903225804745e0, 0.500000e0,
     -0.391500e0, 0.825000e0, 0.741500e0,
     0.103245882826119154987166286332e0,
     0.160322434159191991394857495360e0,
     -0.169859514687100893997445721324e0},      /* q[20] */

    {6.430000e2, 6.453000e2, 6.950000e2,
     1.997750e2, 4.200400e2,
     2.09945691135940719075293945960e1,
     2.15814057875264119875397458907e1,
     3.0135e1, 4.0484e1,
     0.175777517046267847932127026995e0,
     0.380293646126229135059562456934e0}};     /* x[11] */

TSupcrt::TSupcrt()
{
    // set global constant
    //  npH2O++;
    ac = &ac_;
    nc = &nc_;
    el = &el_;
    bcn = &bc_;
    ad = &ad_;
    hc = &hc_;
    to = &to_;
    hb = &hb_;
    li = &li_;
    tt = &tt_;
    cr = &cr_;
    co = &co_;
}

//--------------------------------------------------------------------//
/* Pfind - Return P(T,D) if OK and -1 - if error
*          Calculated (dP/dD)T  when  invoked by Dfind
*          (isat=0)  and  (dP/dT)D  when  invoked  by  SUB  TsLVS
*          (isat=1).       */
double  TSupcrt::Pfind(int isat, double T, double DD)
{
    double err=0.;
    double  /* sd[2],*/ Pw0, Pw1, Pw2, Pw3,  am1, am2, am3,
    p00, p20, p40, p01, p21, p41, aa, xk0, xk1, pw11, alpha,
    alhi, besq, Tee, rho, rhodi=0., rho1=0., tt1, tt2, Pwmu,
                                         dPw0, dPw1, dPw, Pfind, Uw, dPdTcd, dPwdTw, Cvcoex=0.;
    /*  double amc */

    Pw1  = co->a[4];
    Pw2  = co->a[3];
    Pw3  = co->a[1];
    /* amc  = co->a[12]; */
    am1  = co->a[13];
    am2  = co->a[14];
    am3  = co->a[15];
    p00  = co->q[10];
    p20  = co->q[11];
    p40  = co->q[12];
    p01  = co->q[17];
    p21  = co->q[18];
    p41  = co->q[19];
    aa   = co->a[9];
    xk0  = co->a[6];
    xk1  = co->a[11];
    pw11 = co->q[8];
    alpha= co->q[9];
    alhi = co->q[14];
    besq = co->a[8];

    dv.xk[0] = xk0;
    dv.xk[1] = xk1;

    if (fabs(T - cr->Tc) < to->FPTOL)
        T = cr->Tc;
    Tee     = (T - cr->Tc) / cr->Tc;
    dv.Tw  = - cr->Tc / T;
    dv.dTw = 1.0 + dv.Tw;
    if ( !isat )
    {                           /* when isat == 0 */
        rho = DD / cr->rhoC;
        conver(&rho, Tee, rho1, &rhodi, err);
    }
    else
    {                           /* when isat == 1 */
        par.th1 = -1.0;
        a2.th  = par.th1;
        par.r1  = dv.dTw / (1.0 - besq);
        a2.r   = par.r1;

        ss(par.r1, par.th1, dv.s, dv.sd);
        rho = par.th1 * (xk0 * pow(par.r1, co->a[5]) +
                         xk1 * pow(par.r1, co->q[15])) + co->a[0] * (dv.s[0] + dv.s[1]);
        rho += 1.0 + pw11 * dv.dTw;
        dv.amu = 0.0;
        DD = rho * cr->rhoC;
    }
    tt1 = par.th1 * par.th1;
    tt2 = tt1*tt1;
    Pw0  = 1.0 + dv.dTw * (Pw1 + dv.dTw * (Pw2 + dv.dTw * Pw3));
    if ( !isat )
        Pwmu = dv.amu * rhodi;              /* when isat == 0 */
    else
        Pwmu = 0.0;                          /* when isat == 1 */
    dv.p0th = p00 + p20 * tt1 + p40 * tt2;
    dv.p1th = p01 + p21 * tt1 + p41 * tt2;
    dPw0 = xk0 * dv.p0th * pow(par.r1,(2.0 - alpha));
    dPw1 = xk1 * dv.p1th * pow(par.r1,(2.0 - alhi));
    dPw  = aa * (dPw0 + dPw1);
    dv.Pw   = Pw0 + Pwmu + dPw;
    Pfind = dv.Pw * cr->Pcon * T;
    if (fabs(par.th1) < 1.0)
        sa.iphase = 1;
    else
    {
        sa.iphase = 2;
        dv.dP0dT = Pw1 + dv.dTw * (2.0 * Pw2 + 3.0 * Pw3 * dv.dTw);
        dv.dM0dT = am1 + dv.dTw * (2.0 * am2 + 3.0 * am3 * dv.dTw);
        Uw = dv.dP0dT - rho * dv.dM0dT + pw11 * dv.amu + dv.s[0] + dv.s[1];
        dPdTcd = Uw + rho * dv.dM0dT;
        dPwdTw = dv.Pw - dv.Tw * dPdTcd;
        d2.dPdT   = cr->Pcon * dPwdTw;
    }

    aux(par.r1, par.th1, &dv.d2PdT2, &dv.d2PdMT, &dv.d2PdM2,
        aa, dv.xk, dv.sd, Cvcoex);
    if (sa.iphase == 1)
        d2.dPdD = cr->dPcon * DD * T / dv.d2PdM2;
    return(Pfind);
}

//--------------------------------------------------------------------//
/*   /coefs/ a[19], q[19], x[10]   */
void  TSupcrt::aux(double r1, double th1, double *d2PdT2, double *d2PdMT,
                   double *d2PdM2, double aa, double *xk, double *sd, double Cvcoex)
/* xk[2], sd[2] */
{
    int i= -1;
    double  sS[2], w[2], y[2], z[2], coex[2], cc, beta, besq, delta, alpha,
    s00, s20, s01, s21, deli, ww, yy, zz, gamma, tt1, ter, g, alhi,
    beti, gami, a11, a21, a4, f1;

    cc   = co->a[0];
    beta = co->a[5];
    besq = co->a[8];
    delta= co->a[10];
    alpha= co->q[9];
    s00  = co->a[16];
    s20  = co->a[17];
    s01  = co->a[18];
    s21  = co->a[19];

    deli  = 0.0;
    sS[0]   = s00 + s20 * th1 * th1;
    sS[1]   = s01 + s21 * th1 * th1;
    sd[0]  = 2.0 * th1 * s20;
    sd[1]  = 2.0 * th1 * s21;
    ww     = 0.0;
    yy     = 0.0;
    zz     = 0.0;
    gamma  = beta * (delta - 1.0);
    tt1    = th1 * th1;
    ter    = 2.0 * beta * delta - 1.0;
    g      = (1.0 + (besq * ter - 3.0) * tt1 - besq * (ter - 2.0) * tt1 * tt1);
    Cvcoex = 0.0;

    while ( ++i < 2 )
    {
        alhi    = alpha - deli;
        beti    = beta + deli;
        gami    = gamma - deli;
        if ( noZero(r1) )
        {
            w[i]  = (1.0 - alhi) * (1.0 - 3.0 * tt1) * sS[i] -
                    beta * delta * (1.0-tt1) * th1 * sd[i];
            w[i]  = (w[i] * pow(r1, -alhi)) / g;
            w[i]  = w[i] * xk[i];
            ww    = ww + w[i];
            y[i]  = beti * (1.0 - 3.0 * tt1) * th1 -
                    beta * delta * (1.0 - tt1) * th1;
            y[i]  = (y[i] * pow(r1, beti - 1.0)) * xk[i] / g;
            yy    = yy + y[i];
            z[i]  = 1.0-besq*(1.0-(2.0*beti))*tt1;
            z[i]  = (z[i]*pow(r1,-gami)) * xk[i] / g;
            zz    = zz + z[i];
            a11 = (beta * (delta - 3.0) - 3.0 * deli - besq * alhi * gami) /
                 (2.0 * besq * besq * (2.0 - alhi) * (1.0 - alhi) * alhi);
            a21 = 1 + ((beta * (delta - 3.0) - 3.0 * deli - besq * alhi
                       * ter) / (2.0 * besq * (1.0 - alhi) * alhi));
            a21 = -a21;
            a4 = 1.0 + ((ter - 2.0) / (2.0 * alhi));
            f1 = a11 + a21 + a4;
            coex[i] = ((2.0 - alhi) * (1.0 - alhi) * pow(r1, -alhi) * f1 * xk[i]);
            Cvcoex  += coex[i];
        }
        deli    = 0.5;
    }
    *d2PdT2 = aa * ww;
    *d2PdMT = yy + aa * cc * ww;
    *d2PdM2 = zz / aa + 2.0 * cc * yy + cc * cc * aa * ww;
}

//--------------------------------------------------------------------//
/* denLVS - Calculatin DH2O(T,P) or Dvap,Dliq(T,P) for equation
*           of state Levelt Sengers, et al (1983)             */
void  TSupcrt::denLVS(int isat, double T, double P)
{
    int i=-1;
    double  /*s[2], sd[2],*/ Pnext, Pdif, delD, Dmin, Dmax, pw11,
    xk0, xk1, Tw, dTw, rho1, rho2;

    /*  EQUIVALENCE (Dmin, x(4)), (Dmax, x(5)), (pw11, q(9)),
        1            (xk0,  a(7)), (xk1,  a(12))  */

    Dmin =  co->x[3];
    Dmax =  co->x[4];
    pw11 =  co->q[8];
    xk0  =  co->a[6];
    xk1  =  co->a[11];

    if (isat == 0)
    {
        sa.DH2O = cr->rhoC;
        while (++i < 20 )
        {
            Pnext = Pfind(isat, T, sa.DH2O);
            Pdif  = Pnext - P;
            if (sa.iphase == 2)
            {
                if (fabs(Pdif) <= 0.0e0)
                    return;

                if (Pdif < 0.0e0)
                    sa.DH2O = Dmax;
                else
                    sa.DH2O = Dmin;
            }
            else
            {
                delD = -Pdif / d2.dPdD;
                sa.DH2O +=  delD;
                if (sa.DH2O < Dmin)
                    sa.DH2O = Dmin;
                if (sa.DH2O > Dmax)
                    sa.DH2O = Dmax;
                if (fabs(delD / sa.DH2O) < 1.0e-6)
                    return;
            }
        }
    }
    else
    {
        Tw   = - cr->Tc / T;
        dTw  = 1.0e0 + Tw;

        ss(par.r1, par.th1, dv.s, dv.sd);
        rho1 = 1.0e0 + pw11 * dTw + co->a[0] * (dv.s[0] + dv.s[1]);
        rho2 = xk0 * pow(par.r1, co->a[5]) + xk1 * pow(par.r1, co->q[15]);
        sa.Dvap = cr->rhoC * (rho1 - rho2);
        sa.Dliq = cr->rhoC * (rho1 + rho2);
    }
}

//--------------------------------------------------------------------//
// Calculated equation P(T) see Washburn (1924): Monthly Weather Rev.,
// v.52,   pp.488-490.
double TSupcrt::Psublm(double Temp)
{
    double TT, PmmHg, Psublm;

    TT = Temp + 2.731e2;
    PmmHg = pow(1.0e1, (-2.4455646e3 / TT + 8.2312e0 * 0.4342945 * log(TT) -
                        1.677006e-2 * TT + 1.20514e-5 * TT * TT - 6.757169e0));
    /*** convert mmHg to bars ***/
    Psublm = PmmHg * 1.33322e-3;
    return(Psublm);
}

//--------------------------------------------------------------------//
int  TSupcrt::valTD(double T, double D, int /*isat*/, int epseqn)
{
    int istemp;
    double TmnLVS, Tcrit, Ttripl, Dlimit, Tk, /* Ps, Dl, Dv, */
    /*P,*/ PMPa, Dguess, Dsublm, dPdD;

    TmnLVS= co->x[0];
    if ((T - to->FPTOL > hb->Ttop) || (T + to->FPTOL < hb->Tbtm) ||
            (D - to->FPTOL > hb->Dtop) || (D + to->FPTOL < hb->Dbtm))
        return(0);
    Tcrit = cr->Tc - 273.15e0;
    Ttripl = tt->Ttr - 273.15e0;
    if ((T + to->FPTOL >= Tcrit) || ((T >= li->TnIB30) && (D >= tt->Dltrip)))
    {
        Dlimit = li->sDIB30 * (T - li->TnIB30) + hb->Dtop;
        if ( !(D - to->FPTOL <= Dlimit ))
            return(0);
    }
    else
    {
        if (D - to->FPTOL <= tt->Dltrip)
        {
            if (T >= Ttripl)
            {
                Tk = T + 273.15e0;
                if (Tk < TmnLVS)
                {
                    a1.rt = ac->gascon * Tk;
                    pcorr(0,Tk,&trp.Ps,&trp.Dl,&trp.Dv,epseqn);
                }
                else
                {
                    istemp = 1;
                    sa.DH2O = 0.0e0;
                    trp.P    = Pfind(istemp, Tk, sa.DH2O);
                    denLVS(istemp, Tk, trp.P);
                    trp.Dv = sa.Dvap / 1.0e3;
                    trp.Dl = sa.Dliq / 1.0e3;
                }
                if ((D >= trp.Dv) && (D <= trp.Dl))
                    aSpc.isat = 1; //isat = 1;
            }

            else
            {
                trp.P = Psublm(T);
                PMPa = trp.P / 1.0e1;
                Tk = T + 273.15e0;
                Dguess = PMPa / Tk / 0.4e0;
                a1.rt = ac->gascon * Tk;
                bb(Tk);
                denHGK(&Dsublm,&PMPa,Dguess,Tk,&dPdD);
                if ( !( D - to->FPTOL <= Dsublm ))
                    return(0);
            }
        }
        else
        {
            if (D <= li->Dli13)
            {
                Dlimit = li->sDli1 * (T - li->Tli13) + li->Dli13;
                if ( !( D + to->FPTOL >= Dlimit ))
                    return(0);
            }
            else
            {
                Dlimit = li->sDli37 * (T - li->Tli13) + li->Dli13;
                if ( !( D - to->FPTOL <= Dlimit ))
                    return(0);
            }
        }
    }
    return(1);
}

//--------------------------------------------------------------------//
// Return 1, if T-P define liquid or vapor H2O in the applicability range of HKF
// else 0.
int  TSupcrt::valTP(double T, double P)
{
    double  Plimit, Psubl;

    if ((T - to->FPTOL > hb->Ttop) || (T + to->FPTOL < hb->Tbtm) ||
            (P - to->FPTOL > hb->Ptop) || (P + to->FPTOL < hb->Pbtm))
        return(0);

    if (P >= li->Pli13)
    {
        Plimit = li->sPli37 * (T - li->Tli13) + li->Pli13;
        if ( !( P - to->FPTOL <= Plimit ))
            return(0);
    }
    else
    {
        if (P >= tt->Ptripl)
        {
            Plimit = li->sPli1 * (T - li->Tli13) + li->Pli13;
            if ( !( P +to->FPTOL >= Plimit ))
                return(0);
        }
        else
        {
            Psubl = Psublm(T);
            if ( !( P - to->FPTOL <= Psubl ))
                return(0);
        }
    }
    return(1);
}

//--------------------------------------------------------------------//
// valid insert data and scales for HKF
void
TSupcrt::valid(int it, int id1, int ip, int ih, int itripl, int isat,
               int iopt, int useLVS, int epseqn, double Temp, double *Pres,
               double *Dens0, int *eR)    /* double Temp, Pres, Dens; */
{
    double Ttripl, Tcrit, Pcrit; /*T, D, P, */
    // valid specification
    *eR = valspc(it, id1, ip, ih, itripl, isat, iopt, useLVS, epseqn);
    ErrorIf( !*eR, "Tsupcrt","HKF H2O: Specification error!");
    // translate to  degC, bars, g/cm3
    trp.T   = TdegK(it, Temp) - 273.15e0;
    trp.D   = *Dens0 * un.fd;
    trp.P   = *Pres / un.fp * 1.0e1;
    Ttripl  = tt->Ttr - 273.15e0;
    Tcrit   = cr->Tc - 273.15e0;
    Pcrit   = cr->Pc * 1.0e1;
    if (isat == 0)
    {
        if (iopt == 1)
        {
            *eR = valTD(trp.T, trp.D, isat, epseqn);
            ErrorIf( !*eR, "Tsupcrt","Error 1 HKF H2O: at isat=0 & iopt=1,\n"
                     " entered values of T & D fall outside validity region of HGK EOS!");
        }
        else
        {
            *eR = valTP(trp.T, trp.P);
            ErrorIf( !*eR, "Tsupcrt","Error 2 HKF H2O: at isat=0 & iopt=2, \n"
                     "entered values of T & P fall outside validity region of HGK EOS!");
        }
    }
    else
    {
        if (iopt == 1)
        {
            if ( !((trp.T + to->FPTOL  >= Ttripl) && (trp.T- to->FPTOL <= Tcrit)))
            {
                *eR = 0;
                Error( "Tsupcrt", "Error 3 HKF H2O: at isat=1 & iopt=1, input T falls\n"
                       " outside the region between triple and critical points of H2O!\n"
                       " Please try with pressure P > 0.006 bar...");
            }
        }
        else
        {
            if ( !((trp.P+ to->FPTOL >= tt->Ptripl) && (trp.P - to->FPTOL <= Pcrit)))
            {
                *eR = 0;
                Error( "Tsupcrt","Error 4 HKF H2O: at isat=1 & iopt=2, input P falls\n"
                       " outside the region between triple and critical points of H2O!");
            }
        }
    }
}

//--------------------------------------------------------------------//
// Check parametres of state H2O in critical region *eR=1 if yes, else *eR=0
// T,P, D insert in users units, returned in degK, MPa, kg/m3.
void TSupcrt::crtreg(int isat, int iopt, int it, double *T, double *P,
                     double *D, int *eR)
{
    int isat1;
    double Tmin1, Tmin2, Tmax, Dmin, Dmax, Pbase1,
    Pbase2, PTmins, PTmaxs, Pstest, ddummy,/* crtreg,*/
    Pmin, Pmax;
    /*    EQUIVALENCE (Tmin1,  x(1)),  (Tmin2, x(2)),  (Tmax,  x(3)),
       1            (Dmin,   x(4)),  (Dmax,  x(5)),
       2            (Pbase1, x(6)),  (Pbase2,x(7)),
       3            (PTmins, x(10)), (PTmaxs,x(11)) */
    Tmin1=  co->x[0];
    Tmin2= co->x[1];
    Tmax=  co->x[2];
    Dmin=  co->x[3];
    Dmax=  co->x[4];
    Pbase1= co->x[5];
    Pbase2= co->x[6];
    PTmins= co->x[9];
    PTmaxs= co->x[10];

    *T = TdegK(it, *T);     /* � ��ନ�㥬 T � degK */

    if (isat == 0)
    {
        if (iopt == 1)
        {
            *D *= un.fd * 1.0e3;
            if ( !( (*T >= Tmin1) && (*T <= Tmax) && (*D >= Dmin) && (*D <= Dmax) ))
            {
                *eR = 0;
                return;
            }
        }
        else
        {
            *P /= un.fp;
            if ((*T < Tmin1) || (*T > Tmax))
            {
                *eR = 0;
                return;
            }
            else
            {
                Pmin = Pbase1 + PTmins * (*T - Tmin1);
                Pmax = Pbase2 + PTmaxs * (*T - Tmin2);
                if ((*P >= Pmin) && (*P <= Pmax))
                {
                    *eR = 1;
                    return;
                }
                else
                {
                    if ((*P >= Pmin) && (*T <= Tmin2))
                    {
                        isat1 = 1;
                        ddummy = 0.0e0;
                        Pstest = Pfind(isat1, *T, ddummy);
                        if ( !(*P <= Pstest))
                        {
                            *eR = 0;
                            return;
                        }
                    }
                    else
                    {
                        *eR = 0;
                        return;
                    }
                }
            }
        }
    }
    else
    {
        if (iopt == 1)
        {
            if ( !(*T >= Tmin1))
            {
                *eR = 0;
                return;
            }
        }
        else
        {
            *P /= un.fp;
            if ( !(*P >= Pbase1))
            {
                *eR = 0;
                return;
            }
        }
    }
    *eR = 1;
}

//--------------------------------------------------------------------//
// T - ���������
double TSupcrt::TsLVS(int isat, double Pres)
{
    double TsLVS2, TsLVS, Dn, Pnext, dT;
    int i = 0;
    TsLVS2 = cr->Tc - 1.0e0;
    Dn     = cr->rhoC;
    do
    {
        Pnext = Pfind(isat, TsLVS2, Dn);
        dT = (Pnext - Pres) / d2.dPdT;
        TsLVS2 -= dT;
        if (TsLVS2 > cr->Tc)
            TsLVS2 = cr->Tc;
    }
    while( (++i > 20) || ( fabs(dT / TsLVS2) > 1.0e-8 ));
    TsLVS = TsLVS2;
    return(TsLVS);
}

//--------------------------------------------------------------------//
// backup - save COMMON values Pfind during saturation check).
void TSupcrt::backup( struct STORE &sto )
{
    sto.isav1 = sa.iphase;
    sto.sav2  = par.r1;
    sto.sav3  = par.th1;
    sto.sav4  = dv.amu;
    sto.sav5  = dv.s[0];
    sto.sav6  = dv.s[1];
    sto.sav7  = dv.Pw;
    sto.sav8  = dv.Tw;
    sto.sav9  = dv.dTw;
    sto.sav10 = dv.dM0dT;
    sto.sav11 = dv.dP0dT;
    sto.sav12 = dv.d2PdM2;
    sto.sav13 = dv.d2PdMT;
    sto.sav14 = dv.d2PdT2;
    sto.sav15 = dv.p0th;
    sto.sav16 = dv.p1th;
    sto.sav17 = dv.xk[0];
    sto.sav18 = dv.xk[1];
    sto.sav19 = d2.dPdD;
}

//--------------------------------------------------------------------//
// restor - Restore COMMON  values Pfind after saturation check.
void TSupcrt::restor( struct STORE sto )
{
    sa.iphase = sto.isav1;
    par.r1     = sto.sav2;
    par.th1    = sto.sav3;
    dv.amu    = sto.sav4;
    dv.s[0]   = sto.sav5;
    dv.s[1]   = sto.sav6;
    dv.Pw     = sto.sav7;
    dv.Tw     = sto.sav8;
    dv.dTw    = sto.sav9;
    dv.dM0dT  = sto.sav10;
    dv.dP0dT  = sto.sav11;
    dv.d2PdM2 = sto.sav12;
    dv.d2PdMT = sto.sav13;
    dv.d2PdT2 = sto.sav14;
    dv.p0th   = sto.sav15;
    dv.p1th   = sto.sav16;
    dv.xk[0]  = sto.sav17;
    dv.xk[1]  = sto.sav18;
    d2.dPdD   = sto.sav19;
}

//--------------------------------------------------------------------//
/* LVSsat - if isat=1, calculated Psat(T) or Tsat(P) (iopt=1,2).
*           if isat=0, check T-D or T-P (iopt=1,2) into TOL
*           if yes    isat <- 1  and  T <- Tsat.                  */
void TSupcrt::LVSsat(int iopt, int isat, double *T, double *P, double *D)
{
    double   ERRTOL= 1.0e-12, TCTOL = 1.0e-2, Tsat;
    struct STORE sto; /*  backup() or restor() */
    if (isat == 1)
    {
        if (iopt == 1)
            *P = Pfind(isat, *T, *D);
        *T = TsLVS(isat, *P);
    }
    else
    {
        if (iopt == 1)
            *P = Pfind(isat, *T, *D);
        if ( !(*P-ERRTOL > cr->Pc))
        {
            backup( sto );
            Tsat = TsLVS(isat, *P);
            if (fabs(Tsat - *T) < TCTOL)
            {
                *T = Tsat;
                aSpc.isat = isat = 1;
            }
            else
                restor( sto );
        }
    }
}

//--------------------------------------------------------------------//
//thmLVS - calculation thermodinamic and transport fitches in critical
//       region H2O on equations of state Levelt Sengers, et al (1983).
void TSupcrt::thmLVS(int isat, double T, double r1, double th1)
{
    /* double  sd[2];*/
    double pw1, pw11, pw2, pw3, amc, am1, am2, am3, aa, Hw, dPwdTw,
    dPdTal,/* xk0, xk1,*/ d2P0dT, d2M0dT, dPdT2, rho, Uw, Cvcoex=0.,
            CviTw2, Cvw, Cpw, Sw, Scond;
    /* double alhi, besq, alpha; */

    pw2  = co->a[3];
    pw3  = co->a[1];
    /*  besq = co->a[8]; */
    amc  = co->a[12];
    am1  = co->a[13];
    am2  = co->a[14];
    aa   = co->a[9];
    dv.xk[0]  = co->a[6];
    am3  = co->a[15];
    dv.xk[1]  = co->a[11];
    pw11 = co->q[8];
    /*  alpha= co->q[9]; */
    /* alhi = co->q[14]; */
    pw1  = co->a[4];

    d2P0dT = 2.0 * pw2 + 6.0 * pw3 * dv.dTw;
    d2M0dT = 2.0 * am2 + 6.0 * am3 * dv.dTw;
    dv.dP0dT  = pw1 + dv.dTw * (2.0 * pw2 + 3.0 * pw3 * dv.dTw);
    dv.dM0dT  = am1 + dv.dTw * (2.0 * am2 + 3.0 * am3 * dv.dTw);
    if (isat == 0)
    {
        rho = sa.DH2O / cr->rhoC;
        Uw  = dv.dP0dT - rho * dv.dM0dT + pw11 * dv.amu +
              dv.s[0] + dv.s[1];
    }
    else
    {
        rho  = th1 * ( dv.xk[0] * pow(r1,co->a[5]) + dv.xk[1] *
                       pow(r1,co->q[15])) + co->a[0] * (dv.s[0] + dv.s[1]);
        rho += 1.0 + pw11 * dv.dTw;
        Uw   = dv.dP0dT - rho * dv.dM0dT + pw11 * dv.amu +
               dv.s[0] + dv.s[1];
        sa.DH2O = rho * cr->rhoC;
        dPdT2    = dv.Pw - dv.Tw * (Uw + rho * dv.dM0dT);
        th.heat = 1.0e3 * T * (cr->Pcon * dPdT2) * (1.0 / sa.Dvap -
                  1.0 / sa.Dliq);

        ss(r1, th1, dv.s, dv.sd);
        aux(r1,th1, &dv.d2PdT2, &dv.d2PdMT, &dv.d2PdM2, aa,
            dv.xk, dv.sd, Cvcoex);
        if ( noZero(r1)  )
            d2.dPdD = cr->dPcon * sa.DH2O * T / dv.d2PdM2;
    }
    if ( noZero(r1) )
    {
        a1.dPdTcd = dv.dP0dT + pw11 * (dv.amu -rho / dv.d2PdM2) +
                    dv.s[0] + dv.s[1] - dv.d2PdMT * rho / dv.d2PdM2;
        dPwdTw = dv.Pw - dv.Tw * a1.dPdTcd;
        dPdTal = cr->Pcon * dPwdTw;
        CviTw2 = d2P0dT - rho * d2M0dT + dv.d2PdT2 -
                 (pw11 + dv.d2PdMT) * (pw11 + dv.d2PdMT) / dv.d2PdM2;
        Cvw    = CviTw2 * dv.Tw * dv.Tw;
        Cpw    = Cvw + dv.d2PdM2 * dPwdTw * dPwdTw / (rho * rho);
        th.betaw  = 1.0 / (sa.DH2O * d2.dPdD);
        th.alphw  = th.betaw * dPdTal;
        th.Speed   = 1.0e3 * sqrt(Cpw / Cvw * d2.dPdD);
    }
    else
    {
        Cvw   = 1.0;
        Cpw   = 1.0;
        th.betaw = 1.0;
        th.alphw = 1.0;
        th.Speed = 0.0;
    }
    Hw = dv.Pw - dv.Tw * Uw;
    Sw = Hw - rho * (dv.amu + amc + dv.dTw *
                     (am1 + dv.dTw * (am2 + dv.dTw * am3)));
    Scond  = cr->Scon / sa.DH2O;
    th.U      = Uw * cr->Ucon / sa.DH2O;
    th.H      = Hw * Scond * T;
    th.Entrop = Sw * Scond;
    th.AE     = th.U - T * th.Entrop;
    th.GE     = th.H - T * th.Entrop;
    th.Cv     = Cvw * Scond;
    th.Cp     = Cpw * Scond;
}

/* *************************************************************** */
/*  dalLVS - Calc and return  (d(alpha)/dt)p(D,T,alpha) for equation
*            of state Levelt Sengers et al. (1983).
*            Comments:  D (kg/m**3),  T (degK),  P (MPa),
*                         alpha (degK**-1).                        */
double TSupcrt::dalLVS(double D, double T, double P, double alpha)
{
    int  i=-1;
    double s[2], dsdT[2], sp[2], dspdT[2], k[2], calpha[2], cbeta[2],
    cgamma[2], u[2], v[2], w[2], dudT[2], dvdT[2], dwdT[2];
    double dalLVS, a, c, delta, bsq, P11, Delta1, P2, P3, s00, s01, s20,
    s21, b1, b2, ar1, a01, ar2, a02, delT, q, amult, d0dT, drdT,
    dqdT, dP0dTT, ddelMT, dPdTT, dPdMMT, dPdMTT, dPPTT, pterm;
    /* double P1; */

    a     = co->a[9];
    c     = co->a[0];
    delta = co->a[10];
    bsq   = co->a[8];
    P11   = co->q[8];
    Delta1= co->q[13];
    /* P1    = co->a[4]; */
    P2    = co->a[3];
    P3    = co->a[1];
    s00   = co->a[16];
    s01   = co->a[18];
    s20   = co->a[17];
    s21   = co->a[19];

    if ( approximatelyZero(a2.r) )
    {
        dalLVS = 1.0e6;
        return(dalLVS);
    }

    k[0]      = co->a[6];
    k[1]      = co->a[11];
    calpha[0] = co->q[9];
    calpha[1] = co->q[14];
    cbeta[0]  = co->a[5];
    cbeta[1]  = co->q[15];
    cgamma[0] = cbeta[0] * (delta - 1.0);
    cgamma[1] = cgamma[0] - Delta1;
    delT      = (T - cr->Tc) / T;
    s[0]      = s00 + s20 * pow(a2.th,2.);
    s[1]      = s01 + s21 * pow(a2.th,2.);
    sp[0]     = 2.0 * s20 * a2.th;
    sp[1]     = 2.0 * s21 * a2.th;
    /* ********************************************************************* */
    /* Compute drdT and d0dT from solution of the linear system              */
    /*                      ax = b                                           */
    /* d(dPdM)/dT = -D/Dc*alpha - P11*Tc/pow(T,2) = ar1*drdT + a01*d0dT = b1 */
    /* d(delT)/dT =           Tc/pow(T,2)         = ar2*drdT + a02*d0dT = b2 */
    /* ********************************************************************* */

    b1 = -D / cr->rhoC * alpha - P11 * cr->Tc / T / T;
    b2 =  cr->Tc / pow(T,2.);

    ar1 = 0.0;
    a01 = 0.0;

    while(++i < 2 )
    {
        ar1 = ar1 + k[i] * (cbeta[i] * a2.th * pow(a2.r,(cbeta[i]-1.0)) +
                            a * c * (1.0 - calpha[i]) * pow(a2.r,-calpha[i]) * s[i]);
        a01 = a01 + k[i] * (pow(a2.r,cbeta[i]) + a * c * sp[i] *
                            pow(a2.r,(1.0-calpha[i])));
    }
    ar2 = 1.0 - bsq * pow(a2.th,2.) - a * c * cbeta[0] * delta *
          (1.0 - pow(a2.th,2.)) * a2.th * pow(a2.r,(cbeta[0] * delta - 1.0));
    a02 = 3.0 * a * c * pow(a2.th,2.) * pow(a2.r,cbeta[0] * delta) -
          2.0 * bsq * a2.r * a2.th - a * c * pow(a2.r,cbeta[0] * delta);
    /********************************************************************/
    /* solve the linear system with simplistic GE w/ partial pivoting  */
    /********************************************************************/
    if (fabs(ar1) > fabs(ar2))
    {
        amult = -ar2 / ar1;
        d0dT  = (b2 + amult * b1) / (a02 + amult * a01);
        drdT  = (b1 - a01 * d0dT) / ar1;
    }
    else
    {
        amult = -ar1 / ar2;
        d0dT  = (b1 + amult * b2) / (a01 + amult * a02);
        drdT  = (b2 - a02 * d0dT) / ar2;
    }
    /********************************************************************/
    /*   Compute theta polynomials and their tempertaure derivatives  */
    dsdT[0]   = 2.0 * s20 * a2.th * d0dT;
    dsdT[1]   = 2.0 * s21 * a2.th * d0dT;
    dspdT[0]  = 2.0 * s20 * d0dT;
    dspdT[1]  = 2.0 * s21 * d0dT;
    q    = 1.0 + (bsq * (2.0 * cbeta[0] * delta - 1.0) - 3.0) *
           pow(a2.th,2.) - bsq * (2.0 * cbeta[0] * delta - 3.0)
           * pow(a2.th,4.);
    dqdT = 2.0 * (bsq * (2.0 * cbeta[0] * delta - 1.0) - 3.0) *
           a2.th * d0dT - 4.0 * bsq * (2.0 * cbeta[0] * delta -
                                       3.0) * pow(a2.th,3.) * d0dT;
    i=-1;
    while(++i < 2)
    {
        u[i]    = (1.0 - bsq * (1.0 - 2.0 * cbeta[i]) * pow(a2.th,2.)) / q;
        dudT[i] = (-2.0 * bsq * (1.0 - 2.0 * cbeta[i]) * a2.th * d0dT -
                   u[i] * dqdT) / q;
        v[i]    = ((cbeta[i] - cbeta[0] * delta) * a2.th +
                   (cbeta[0] * delta - 3.0 * cbeta[i]) * pow(a2.th,3.)) / q;
        dvdT[i] = ((cbeta[i] - cbeta[0] * delta) * d0dT +
                   3.0 * (cbeta[0] * delta - 3.0 * cbeta[i]) *
                   pow(a2.th,2.) * d0dT - v[i] * dqdT) / q;
        w[i]    = ((1.0 - calpha[i]) * (1.0 - 3.0 * pow(a2.th,2.)) *
                   s[i] - cbeta[0] * delta * (a2.th - pow(a2.th,3.)) *
                   sp[i]) / q;
        dwdT[i] = ((1.0 - calpha[i]) * ((1.0 - 3.0 * pow(a2.th,2.)) *
                                        dsdT[i] - 6.0 * a2.th * s[i] * d0dT) - cbeta[0] *
                   delta * ((a2.th - pow(a2.th,3.)) * dspdT[i] + sp[i] *
                            (d0dT - 3.0 * pow(a2.th,2.) * d0dT)) - w[i] * dqdT) / q;
    }
    /***************************************************************/
    /*   Compute dP0dTT, ddelMT, dPdTT, dPdMMT, dPdMTT, dPPTT      */

    dP0dTT = cr->Tc / pow(T,2.) * (2.0* P2 + 6.0 * P3 * delT);
    ddelMT = a * pow(a2.r,cbeta[0]*delta) * (cbeta[0]* delta *
             a2.th / a2.r * (1.0 - pow(a2.th,2.)) * drdT +
             (1.0 - 3.0 * pow(a2.th,2.)) * d0dT);
    dPdTT  = 0.0;
    dPdMMT = 0.0;
    dPdMTT = 0.0;
    i=-1;
    while (++i < 2)
    {
        dPdTT  = dPdTT + a * k[i] * (pow(a2.r,(1.0 - calpha[i])) *
                                     dsdT[i] + s[i] * (1.0 - calpha[i]) *
                                     pow(a2.r,-calpha[i]) * drdT);
        dPdMMT = dPdMMT + k[i] * ((pow(a2.r,(-cgamma[i])) * dudT[i] -
                                   u[i] * cgamma[i] * pow(a2.r,(-1.0 - cgamma[i]) * drdT)) /
                                  a + 2.0 * c * (pow(a2.r,(cbeta[i] - 1.0)) * dvdT[i] +
                                                 v[i] * (cbeta[i] - 1.0) * pow(a2.r,(cbeta[i] - 2.0)) * drdT)
                                  + a * pow(c,2.) * (pow(a2.r,(-calpha[i])) * dwdT[i] -
                                                     calpha[i] * w[i] * pow(a2.r,(-1.0 - calpha[i]) * drdT)));
        dPdMTT = dPdMTT + k[i] * (pow(a2.r,(cbeta[i] - 1.0)) * dvdT[i] +
                                  v[i] * (cbeta[i] - 1.0) * pow(a2.r,(cbeta[i] - 2.0)) *
                                  drdT + a * c * (pow(a2.r,(-calpha[i])) * dwdT[i] -
                                                  calpha[i] * pow(a2.r,(-1.0 - calpha[i])) * drdT * w[i]));
    }
    dPPTT = dP0dTT + dPdTT + P11 * ddelMT - D / cr->rhoC * dPdMTT /
            dv.d2PdM2 + (P11 + dv.d2PdMT) * (D / cr->rhoC * alpha /
                                             dv.d2PdM2 + D / cr->rhoC * dPdMMT / pow(dv.d2PdM2,2.));
    pterm = P / cr->Pc + a1.dPdTcd;

    /*          compute (d(alpha)/dT)P              */
    dalLVS  = cr->Tc * pow(cr->rhoC,2.) / pow(D,2.) / pow(T,2.) *
              (-2.0 / T * dv.d2PdM2 * pterm + 2.0 * alpha *
               dv.d2PdM2 * pterm + pterm * dPdMMT + dv.d2PdM2 * dPPTT);
    return(dalLVS);
}

//--------------------------------------------------------------------//
// translate parametrs into users units and load them into tprops.
void TSupcrt::dimLVS(int isat, int itripl, double theta, double T, double *Pbars,
                     double *dL, double *dV, WPROPS *www, int epseqn)
{
    double CpJKkg, betaPa, betab, dkgm3, pbars;
    pbars = *Pbars * 1.0e1;
    if (isat == 1)
    {
        *dV   = sa.Dvap;
        *dL   = sa.Dliq;
    }
    www->Aw = th.AE * un.fh;
    www->Gw  = th.GE * un.fh;
    www->Sw  = th.Entrop * un.fh * un.ft;
    www->Uw  = th.U * un.fh;
    www->Hw  = th.H * un.fh;
    www->Cvw = th.Cv * un.fh * un.ft;
    www->Cpw = th.Cp * un.fh * un.ft;
    www->Speedw = th.Speed * un.fs;
    www->Betaw = th.betaw / un.fp;
    www->Alphaw = th.alphw;
    /**************************************************************/
    a2.th = theta;
    www->dAldT = dalLVS(sa.DH2O,T, pbars/1.0e1,www->Alphaw); /*!!!!!*/
    /**************************************************************/
    CpJKkg  = th.Cp * 1.0e3;
    betaPa  = th.betaw / 1.0e6;
    betab   = th.betaw / 1.0e1;

    if ( approximatelyEqual( fabs(theta), 1.0e0) )
    {
        dkgm3 = sa.DH2O;
        www->Surtenw = 0.0e0;
    } else
    if (T > cr->Tc ) // only below critical T we have two phases DM 23.07.2024
    {
        dkgm3 = sa.DH2O;
        www->Surtenw = 0.0e0;
    }
    else
    {
        if (theta < 0.0e0)
        {
            dkgm3 = sa.Dvap;
            www->Surtenw = 0.0e0;
        }
        else
        {
            dkgm3 = sa.Dliq;
            www->Surtenw = surten(T) * un.fst;
        }
    }
    Born92(T, pbars, dkgm3/1.0e3, betab, &www->Alphaw,
           &www->dAldT, &www->Dielw, &www->ZBorn,
           &www->QBorn, &www->YBorn, &www->XBorn, epseqn);
    www->Viscw  = viscos(T, pbars, dkgm3, betaPa) * un.fvd;
    www->Tcondw = thcond(T, pbars, dkgm3, www->Alphaw, betaPa) *
                  un.fc * un.ft;
    www->Tdiffw = www->Tcondw / un.fc / un.ft / (dkgm3 * CpJKkg) *
                  un.fvk;
    www->Prndtlw= www->Viscw / un.fvd * CpJKkg /
                  (www->Tcondw / un.fc / un.ft);
    www->Visckw = www->Viscw / un.fvd / dkgm3 * un.fvk;
    www->Albew = www->Alphaw / www->Betaw;

    if (itripl == 1)
        triple(T, www);
}

//--------------------------------------------------------------------//
//cpswap - load critical parametres A, G, U, H, S, Vs, Di,ZB
//from wpliq to wprops and aprocsimaze values - Cv, Cp, alpha, beta, visc,
//tcond, Prndtl, tdiff, visck, YB, QB, XB, daldT, st �� wprops � wpliq.
void TSupcrt::cpswap()
{
    wr.Aw     = wl.Aw;
    wr.Gw     = wl.Gw;
    wr.Sw     = wl.Sw;
    wr.Uw     = wl.Uw;
    wr.Hw     = wl.Hw;
    wr.Dielw  = wl.Dielw;
    wr.ZBorn  = wl.ZBorn;
    wr.Surtenw= wl.Surtenw;
    wl.Cvw    = wr.Cvw;
    wl.Cpw    = wr.Cpw;
    wl.Alphaw = wr.Alphaw;
    wl.Betaw  = wr.Betaw;
    wl.YBorn  = wr.YBorn;
    wl.QBorn  = wr.QBorn;
    wl.XBorn  = wr.XBorn;
    wl.Tcondw = wr.Tcondw;
    wl.Tdiffw = wr.Tdiffw;
    wl.Prndtlw= wr.Prndtlw;
    wl.dAldT  = wr.dAldT;
    wl.Albew  = wr.Albew;
    wr.Speedw = 0.429352766443498e2 * un.fs;
    wr.Viscw  = 1.0e6;
    wr.Visckw = 1.0e6;
    wl.Speedw = wr.Speedw;
    wl.Viscw  = wr.Viscw;
    wl.Visckw = wr.Visckw;
}

//--------------------------------------------------------------------//
// Calculation thermodinamic and transport water properties in H2O critical region
// ( 369.85 - 419.85  degC, 0.20-0.42 gm/cm3) see equat Levelt Sengers, et al
// (1983): J.Phys. Chem. Ref. Data, V.12, No.1, pp.1-28.
void TSupcrt::LVSeqn(int isat, int iopt, int itripl, double TC,
                     double *P, double *Dens0, int /*epseqn*/)
{
    int  cpoint=0, ioptsv=0, sW=0;
    double dL=0., dV=0., cdens=0.;

    /* cpoint = 0; */
    sa.DH2O = *Dens0;
    do
    {
        LVSsat(iopt, isat, &TC, P, &sa.DH2O);
        if ((isat != 0) || (iopt != 1))
            denLVS(isat, TC, *P);
        if ( !isat )
            *Dens0 = sa.DH2O;
        else
        {
            *Dens0 = sa.Dliq;
            aSta.Dens[1] = sa.Dvap;
        }
        if ( !isat )
        {
            thmLVS(isat, TC, par.r1, par.th1);
            dimLVS(isat, itripl, par.th1, TC, P, &dL, &dV, &wr, aSpc.epseqn);
            /*   iRET = change(wpr, wr); */
            if ( cpoint )
            {
                cpswap();
                *Dens0 = cdens;
                aSta.Dens[1] = cdens;
                aSpc.isat = isat = 1;
                aSpc.iopt = iopt = ioptsv;
            }
        }
        else
        {
            par.th1 = -1.0e0;
            thmLVS(isat, TC, par.r1, par.th1);
            dimLVS(isat, itripl, par.th1, TC, P, &dL, &dV, &wr, aSpc.epseqn);
            /*    iRET = change(wpr, wr);   */
            par.th1 =  1.0e0;
            thmLVS(isat, TC, par.r1, par.th1);
            dimLVS(isat, itripl, par.th1, TC, P, &dL, &dV, &wl, aSpc.epseqn);
            /*   iRET = change(wpl, wl); */
            if (  approximatelyEqual(dL, dV) )
            {
                cpoint = 1;
                cdens  = dL;
                TC =  647.067000003e0;
                *P =  22.0460000008e0;
                ioptsv   = iopt;
                aSpc.iopt = iopt = 2;
                aSpc.isat = isat = 0;
                sW=1;
            }
        }
    }
    while ( sW != 0 );
}

//--------------------------------------------------------------------//
/* HGKsat - if isat=1, calculated Psat(T) or Tsat(P) (iopt=1,2).
*           if isat=0, check T-D or T-P (iopt=1,2) into TOL
*           if yes    isat <- 1  and  calc fitches                  */
void TSupcrt::HGKsat(int& isat, int iopt, int itripl, double Temp,
                     double *Pres, double *Dens, int epseqn)
{
    double  Ptemp=0., dltemp=0., dvtemp=0.;

    if ( isat == 1 )
    {
        if (iopt == 1)
            pcorr(itripl, Temp, Pres, Dens, &aSta.Dens[1], epseqn);
        else
            tcorr(itripl, &Temp, Pres, Dens, &aSta.Dens[1], epseqn);
    }
    else
    {
        if ((Temp > cr->Tc) || (Temp < tt->Ttr) ||
                ((iopt == 2) && (*Pres > cr->Pc)))
            return;                             /* ????????? */
        else
        {
            pcorr(itripl, Temp, &Ptemp, &dltemp, &dvtemp, epseqn);
            if (((iopt == 2) && (fabs(*Pres-Ptemp) <= to->PTOL)) ||
                    ((iopt == 1) && ((fabs(aSta.Dens[0]-dltemp) <= to->DTOL) ||
                                     (fabs(aSta.Dens[0] - dvtemp) <= to->DTOL))))
            {
                isat = 1;
                *Pres = Ptemp;
                aSta.Dens[0] = dltemp;
                aSta.Dens[1] = dvtemp;
            }
        }
    }
}

//--------------------------------------------------------------------//
// Calc dependent state values
void TSupcrt::calcv3(int iopt, int itripl, double Temp, double *Pres,
                     double *Dens, int epseqn)
{
    double ps=0., dll=0., dvv=0., dguess;

    if (iopt == 1)
    {
        resid(Temp, Dens);
        base(Dens, Temp);
        ideal(Temp);
        *Pres  = a1.rt * *Dens * ac->zb + qq.q0;
    }
    else
    {
        if (Temp < ac->tz)
        {
            pcorr(itripl, Temp, &ps, &dll, &dvv, epseqn);
        }
        else
        {
            ps   = 2.0e4;
            dll  = 0.0e0;
        }
        if (*Pres >  ps)
            dguess = dll;
        else
        {
            dguess = *Pres / Temp / 0.4e0;
        }
        denHGK(&aSta.Dens[0], Pres, dguess, Temp, &fct.dpdd);   /* Dens ???*/
        ideal(Temp);
    }
}

//--------------------------------------------------------------------//
// Calc metastable 01.06.2016
void TSupcrt::calcv2(int iopt, int itripl, double Temp, double *Pres,
                     double *Dens, int epseqn)
{
    double ps=0., dll=0., dvv=0., dguess;

    if (iopt == 1)
    {
        resid(Temp, Dens);
        base(Dens, Temp);
        ideal(Temp);
        *Pres  = a1.rt * *Dens * ac->zb + qq.q0;
    }
    else
    {
        if (Temp < ac->tz)
        {
            pcorr(itripl, Temp, &ps, &dll, &dvv, epseqn);
        }
        else
        {
            ps   = 2.0e4;
            dll  = 0.0e0;
        }
        if (*Pres <  ps)
        {
            dguess = dll;
            if (aSpc.CV == CPM_HKF)
                aSpc.metastable = true;
        }
        else
        {
            dguess = *Pres / Temp / 0.4e0;
            if (aSpc.CV == CPM_GAS)
                aSpc.metastable = true;
        }
        denHGK(&aSta.Dens[1], Pres, dguess, Temp, &fct.dpdd);   /* Dens ???*/
        ideal(Temp);
    }
}

//--------------------------------------------------------------------//
// Calculation thermodynamic and transport H2O properties for equation of
// state Haar, Gallagher, & Kell (1984).
void TSupcrt::HGKeqn(int isat, int iopt, int itripl, double Temp,
                     double *Pres, double *Dens0, int epseqn)
{
    a1.rt = ac->gascon * Temp;
    HGKsat(isat, iopt, itripl, Temp, Pres, Dens0, epseqn);
    if (isat == 0)
    {
        // metastable phase propperties 01.06.2016
        bb(Temp);
        calcv2(iopt, itripl, Temp, Pres, &aSta.Dens[1], epseqn);
        thmHGK(&aSta.Dens[1], Temp);
        dimHGK(isat, itripl, Temp, Pres, &aSta.Dens[1], epseqn);

        memcpy(&wl, &wr, sizeof(WPROPS)); // 01.06.2016

        // stable phase properties
        bb(Temp);
        calcv3(iopt, itripl, Temp, Pres, Dens0, epseqn);
        thmHGK(Dens0, Temp);
        dimHGK(isat, itripl, Temp, Pres, Dens0, epseqn);
    }
    else
    {
        memcpy(&wl, &wr, sizeof(WPROPS));             /* from wr to wl */
        dimHGK(2, itripl, Temp, Pres, &aSta.Dens[1], epseqn);
    }
}

//--------------------------------------------------------------------//
// translate t from deg to users units
double TSupcrt::TdegUS(int it, double t)
{
    double TdegUS=t;
    switch(it)
    {
    case 0:
        TdegUS = t;
        break;
    case 1:
        TdegUS = t - 273.15e0;
        break;
    case 2:
        TdegUS = t * 1.8e0;
        break;
    case 3:
        TdegUS = t * 1.8e0 - 459.67e0;
        break;
    }
    return(TdegUS);
}

//--------------------------------------------------------------------//
void TSupcrt::Supcrt_H2O( double TC, double *P )
{
    int eR, CV;
    char PdcC;
    bool on_sat_curve;
    double tempy;

    // 01.06.2016
    CV = aSpc.CV;
    PdcC = aSpc.PdcC;
    on_sat_curve = aSpc.on_sat_curve;
    //

    //memset( &aSpc, '\0', sizeof(SPECS));
    aSpc = SPECS{};
    // in SUPCRT92 set type calculation of parametres
    // 01.06.2016
    aSpc.CV = CV;
    aSpc.PdcC = PdcC;
    aSpc.on_sat_curve = on_sat_curve;
    //
    aSpc.it=1;
    aSpc.id=1;
    aSpc.ip=1;
    aSpc.ih=4;
    aSpc.itripl=1;
    // 01.06.2016
    double psat = PsHGK(TC + 273.15)*10.0;
    if (( approximatelyZero( fabs( *P ), 1e-5 ) || aSpc.on_sat_curve || fabs(*P -  psat) <  1.e-4*psat) && TC <= cr->Tc - 273.15e0) // || aSpc.on_sat_curve || fabs(*P -  psat) < 1.e-9* psat added 01.06.2016, fix 06.11.2025
    { // set only T
        aSpc.isat=1;
        aSpc.iopt=1;
        // 01.06.2016
        aSpc.on_sat_curve = true;
    }
    else
    { //set T and P
        aSpc.isat = 0;
        aSpc.iopt = 2;
    }
    aSpc.useLVS=1;
    aSpc.epseqn=4;
    aSta.Dens[0]=0.0;

    memset( &wl, 0, sizeof(WPROPS));
    memset( &wr, 0, sizeof(WPROPS));
    memset( &a2, 0, sizeof(ABC2));
    memset( &a1, 0, sizeof(ABC1));
    memset( &trp, 0, sizeof(TERM_PR));
    memset( &un, 0, sizeof(UNITS));
    memset( &sa, 0, sizeof(SATUR));
    memset( &qq, 0, sizeof(QQQQ));
    memset( &fct, 0, sizeof(FCTS));
    memset( &ba, 0, sizeof(BASEF));
    memset( &res, 0, sizeof(RESF));
    memset( &id, 0, sizeof(IDF));
    memset( &th, 0, sizeof(THERM));
    memset( &par, 0, sizeof(PARAM));
    memset( &d2, 0, sizeof(DERI2));
    memset( &dv, 0, sizeof(DERIV));
    memset( &io, 0, sizeof(IO_Y));
    //set local parameters with to fact scales
    unit(aSpc.it, aSpc.id, aSpc.ip, aSpc.ih, aSpc.itripl);
    // check insert data if Errir that trow
    valid(aSpc.it, aSpc.id, aSpc.ip, aSpc.ih, aSpc.itripl,
          aSpc.isat, aSpc.iopt, aSpc.useLVS, aSpc.epseqn,
          aSta.Temp, &aSta.Pres, &aSta.Dens[0], &eR);
    // chack parametres in critical region eR=1
    crtreg(aSpc.isat, aSpc.iopt, aSpc.it, &aSta.Temp, &aSta.Pres,
           &aSta.Dens[0], &eR);
    if( eR )
    {
        aSpc.icrit  = 1;
        aSpc.useLVS = 1;
    }
    else
    {
        aSpc.icrit  = 0;
        aSpc.useLVS = 0;
    }

    if ( aSpc.useLVS )
    { // calculation of thermodynamic and transport water properties
        // in the near critical region
        ideal(aSta.Temp);   // bugfix for Akinfiev nonelectrolyte model (TW, 16.05.2008)
    	LVSeqn(aSpc.isat, aSpc.iopt, aSpc.itripl,
               aSta.Temp, &aSta.Pres, &aSta.Dens[0], aSpc.epseqn);
        aSta.Dens[0] /= 1.0e3;
        if ( aSpc.isat == 1 )
            aSta.Dens[1] /= 1.0e3;
    }
    else
    {
        aSta.Dens[0] /= 1.0e3;
        HGKeqn(aSpc.isat, aSpc.iopt, aSpc.itripl,
               aSta.Temp, &aSta.Pres, &aSta.Dens[0], aSpc.epseqn);

        // if metastable added 01.06.2016
        if (aSpc.metastable)
            aSpc.isat = 1;
    }
    load(0, aWp);
    if (aSpc.isat == 1)
    {
        if (!aSpc.metastable) // 01.06.2016
        {
            tempy = aSta.Dens[0];
            aSta.Dens[0] = aSta.Dens[1];
            aSta.Dens[1] = tempy;
        }
        load(aSpc.isat, aWp);
    }

    // translate T - to users units
    aSta.Temp   = TdegUS(aSpc.it, aSta.Temp);
    aSta.Pres  *= un.fp;
    aSta.Dens[0] /= un.fd;

    if (aSpc.isat == 1)
        aSta.Dens[1] /= un.fd;
    *P = aSta.Pres;
    aWp.init = true;
}

//--------------------- End of s_subcrt2.cpp ---------------------------
