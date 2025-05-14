//-------------------------------------------------------------------
// $Id: s_supcrt.cpp 1252 2009-02-23 11:03:11Z wagner $
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
#include "s_supcrt.h"
#include "GEMS3K/v_detail.h"

//--------------------------------------------------------------------//
double  TdegK(int it, double t)
{
    double TdegK=t;
    switch(it)
    {
    case 0:
        TdegK = t;
        break;
    case 1:
        TdegK = t + 273.15e0;
        break;
    case 2:
        TdegK = t / 1.8e0;
        break;
    case 3:
        TdegK = (t + 459.67e0) / 1.8e0;
        break;
    }
    return(TdegK);
}

//--------------------------------------------------------------------//
void
TSupcrt::unit(int it, int id1, int ip, int ih, int itripl)
{
    double fft[4] =  {1.e0, 1.e0, 0.555555556e0, 0.555555556e0 };
    double ffd[4] = {1.e-3, 1.e0, 1.80152e-2, 1.6018e-2};
    double ffvd[4]= {1.e0, 1.e1, 0.555086816e0, 0.671968969e0 };
    double ffvk[4]= {1.e0, 1.e4, 1.0e4, 1.076391042e1 };
    double ffs[4] = {1.e0, 1.e2, 1.0e2, 3.280833e0 };
    double ffp[5] = {1.e0, 1.e1, 9.869232667e0, 1.45038e2, 1.01971e1};
    double ffh[6] = {1.e0, 1.e0, 1.80152e1, 2.3901e-1, 4.305816e0, 4.299226e-1};
    double ffst[4]= {1.e0, 1.e3, 0.555086816e2, 0.2205061e1 };
    double ffcd[4]= {1.e0, 1.e-2, 1.0e-2, 0.3048e0 };
    double ffch[6]= {1.e-3, 1.e0, 1.0e0, 0.23901e0, 0.23901e0, 0.947244e-3 };

    un.ft  = fft[it];
    un.fd  = ffd[id1];
    un.fvd = ffvd[id1];
    un.fvk = ffvk[id1];
    un.fs  = ffs[id1];
    un.fp  = ffp[ip];
    un.fh  = ffh[ih];
    un.fst = ffst[id1];
    un.fc  = ffcd[id1] * ffch[ih];
    if ( itripl == 1 )
        tpset();
}

//--------------------------------------------------------------------//
// tpset - calculate values U, S, H, A, G in 3-y point (in J/g from
// Table 2,  Helgeson & Kirkham,  1974a)
void
TSupcrt::tpset()
{
    double Utr, Str, Htr, Atr, Gtr;
    // 29.10.2025 fix to CODATA/NEA slected standard water properties H -285830 J/mol and S 69.95 J/mol
    // Using the values at 298.15 K for H 1888.803 J/mol and S 6.6102 J/(mol*K ) calculated from the HGK84 (IAPWS84) EoS

    Utr = -15766.55e0; //-15766.0e0;
    Str =  3.5158e0;//3.5144e0;
    Htr = -15970.6e0; //-15971.0e0;
    Atr = -12870.11e0;//-12870.0e0;
    Gtr = -13070.67e0;//-13073.0e0;

    tt->Utri = Utr * un.fh;
    tt->Stri = Str * un.fh;
    tt->Htri = Htr * un.fh;
    tt->Atri = Atr * un.fh;
    tt->Gtri = Gtr * un.fh;

}

//--------------------------------------------------------------------//
int TSupcrt::valspc(int it, int id1, int ip, int ih, int itripl,
                    int isat, int iopt, int useLVS, int epseqn)
{
    if ( !((0 <= it)     && (it     <= 3) &&
            (0 <= id1)     && (id1     <= 3) &&
            (0 <= ip)     && (ip     <= 4) &&
            (0 <= ih)     && (ih     <= 5) &&
            (0 <= itripl) && (itripl <= 1) &&
            (0 <= isat)   && (isat   <= 1) &&
            (1 <= iopt)   && (iopt   <= 2) &&
            (0 <= useLVS) && (useLVS <= 1) &&
            (1 <= epseqn) && (epseqn <= 5)))
        return(0);
    return(1);
}

//--------------------------------------------------------------------//
double  TSupcrt::PsHGK(double t)
{
    double  pl, psHGK, v, w, b, q, z;
    int i=-1;
    double a[8] ={ -.78889166e1,  .25514255e1, -.6716169e1,  .33239495e2,
                   -.10538479e3,  .17435319e3, -.14839348e3, .48631602e2};
    if (t <= 314.0e0)
    {
        pl    = 6.3573118e0 - 8858.843e0 / t + 607.56335e0 * pow(t,-0.6e0);
        psHGK = 0.1e0 * exp(pl);
    }
    else
    {
        v = t / 647.25e0;
        w = fabs(1.0e0 - v);
        b = 0.0e0;
        while (++i <= 7)
        {
            z  = i + 1;
            b += a[i] * pow(w,((z + 1.0e0) / 2.0e0));
        }
        q = b / v;
        psHGK = 22.093e0 * exp(q);
    }
    return(psHGK);
}

// added 09.06.2016 DM
double TSupcrt::getPsatHGK (double TK)
{
//    double dp;
//    double p  = PsHGK(TK);
//    double dL= 0.0e0;
//    double dV = 0.0e0;

//    do
//    {
//        corr(aSpc.itripl, TK, &p, &dL, &dV, &trp.delg, aSpc.epseqn);
//        dp   = trp.delg * ac->gascon * TK / (1.0e0 / dV - 1.0e0 / dL);
//        p  += dp;
//    }
//    while ( fabs(trp.delg) > 1.0e-4 );
    return PsHGK(TK);
}

//--------------------------------------------------------------------//
//the "excluded volume" (eq A.3)and virial coefficient (eq A.4), in cm3/g (b1,b2)
//(b1t,b1tt,b2t,b2tt)./* /ellcon/  /aconst/  /bconst/ bp(10), bq(10) */
void TSupcrt::bb(double t)
{
    int i=0;
    double v[10];
    v[0] = 1.0e0;
    while(++i <= 9)
    {
        v[i] = v[i-1] * ac->tz / t;
    }
    el->b1   = bcn->bp[0] + bcn->bp[1] * log(1.0 / v[1]);
    el->b2   = bcn->bq[0];
    el->b1t  = bcn->bp[1] * v[1] / ac->tz;
    el->b2t  = 0.0e0;
    el->b1tt = 0.0e0;
    el->b2tt = 0.0e0;
    i=1;
    while ( ++i <= 9 )
    {
        el->b1   += bcn->bp[i] * v[i-1];
        el->b2   += bcn->bq[i] * v[i-1];
        el->b1t  -= (i-1) * bcn->bp[i] * v[i-1] / t;
        el->b2t  -= (i-1) * bcn->bq[i] * v[i-1] / t;
        el->b1tt += bcn->bp[i] *(i-1)*(i-1) * v[i-1] / t / t;
        el->b2tt += bcn->bq[i] * (i-1)*(i-1) * v[i-1] / t / t;
    }
    el->b1tt -= el->b1t / t;
    el->b2tt -= el->b2t / t;
}

//--------------------------------------------------------------------//
/*   /resf/  /qqqq/   /nconst/ /aconst/  /addcon/ /RTcurr/ /tolers/ */
/*  atz[3], adz[3], aat[3], aad[3], g[39], ii[39], jj[39]  */
void TSupcrt::resid(double t, double *d)
{
    int i=0, l, j, k, km;
    double  qr[11], qt[10], qzr[9], qzt[9];
    double  e, /* q10,*/ q20, v, dfdt,/* q2a, q5t,*/ d2f, dpt, tex, /* fct,*/
    /*dadt,*/ ddz, zz, del, dex,/* qp,*/ qm, att, tx, tau, ex1, ex2;
    long double q10, fct1, q5t, qp, dadt, q2a;
    qr[0]  = 0.0e0;
    qq.q5 = 0.0e0;
    qq.q0 = 0.0e0;
    res.ar = 0.0e0;
    dadt   = 0.0e0;
    res.cvr= 0.0e0;
    res.dpdtr = 0.0e0;
    e     = exp(-ac->aa * *d);
    q10   = (*d) * (*d) * e;
    q20   = 1.0e0 - e;
    qr[1] = q10;
    v     = ac->tz / t;
    qt[0] = t / ac->tz;

    while(++i < 10)
    {
        qr[i+1] = qr[i] * q20;
        qt[i]   = qt[i-1] * v;
    }
    for (i=0; i<=8; i++) /* EQUIVALENCE (qr(3), qzr(1)), (qt(2), qzt(1)) */
    {
        qzr[i] = qr[i+2];
        qzt[i]=qt[i+1];
    }                  /*  as resalt: qzr[0]=qr[2], ... qzr[8]=qr[10];
                                         qzt[0]=qt[1], ... qzt[8]=qt[9]; */
    i=-1;
    while(++i <  nc->nc)
    {
        k  = nc->ii[i] + 1;
        l  = nc->jj[i];
        zz = k;
        if (k == 1)
            qp = nc->g[i] * ac->aa * qr[1] * qzt[l-1];
        else
            qp = nc->g[i] * ac->aa * qzr[k-2] * qzt[l-1];

        qq.q0 += qp;
        qq.q5 += ac->aa * (2.0 / *d - ac->aa * (1.0 - e * (k-1) / q20)) * qp;
        res.ar += nc->g[i] * qzr[k-1] * qzt[l-1] / q10 / zz / a1.rt;
        dfdt    = pow(q20,((double)k)) * (1-l) * qzt[l] / ac->tz / k;
        d2f     = l * dfdt;
        dpt     = dfdt*q10 * ac->aa * k / q20;
        dadt   += nc->g[i] * dfdt;
        res.dpdtr += nc->g[i] * dpt;
        res.cvr   += nc->g[i] * d2f / ac->gascon;
    }
    qp  = 0.0e0;
    q2a = 0.0e0;
    j=35;
    while (++j <= 39 )
    {
        if ( noZero(nc->g[j]) )
        {
            k   = nc->ii[j];
            km  = nc->jj[j];
            ddz = ad->adz[j-36];
            del = *d / ddz - 1.0e0;
            if (fabs(del) < 1.0e-10)
                del = 1.0e-10;
            ex1   = -ad->aad[j-36] * pow(del,(double)k);

              if (ex1 < -215)   // if (ex1 < to->EXPTOL)   exp(-215) produces nan
                dex = 0.0e0;
            else
                dex = exp(ex1)  * pow(del,(double)km);

            att   = ad->aat[j-36];
            tx    = ad->atz[j-36];
            tau   = t / tx - 1.0e0;
            ex2   = -att * tau * tau;

            if (ex2 <= -215)  // if (ex2 <=  to->EXPTOL)
                tex = 0.0e0;
            else
                tex = exp(ex2);
            q10   = dex * tex;
            qm    = km / del - k * ad->aad[j-36] * pow(del,((double)k-1));
            fct1   = qm * *d * *d * q10 / ddz;
            q5t   = fct1 * (2.0e0 / *d + qm / ddz) - (*d / ddz) * (*d / ddz) *
                    q10 * (km / del / del + k * (k-1) * ad->aad[j-36] *
                           pow(del,((double)k-2)));
            qq.q5    += q5t * nc->g[j];
            qp        += nc->g[j] * fct1;
            dadt      -= 2.0e0 * nc->g[j] * att * tau * q10 / tx;
            res.dpdtr -= 2.0e0 * nc->g[j] * att * tau * fct1 / tx;
            q2a       += t * nc->g[j] * (4.0e0 * att * ex2 + 2.0e0 *
                                         att) * q10 / tx / tx;
            res.ar    += q10 * nc->g[j] / a1.rt;
        }
    }
    res.sr   = -dadt / ac->gascon;
    res.ur   = res.ar + res.sr;
    res.cvr += q2a / ac->gascon;
    qq.q0  += qp;
}

//--------------------------------------------------------------------//
// calc  Abase, Gbase, Sbase, Ubase, Hbase, Cvbase,
// see  Haar , Gallagher & Kell (1979), eq(1)
/*  /ellcon/  /basef/   /aconst/  */
void TSupcrt::base(double *d, double t)
{
    double x, z0, dz0, bb2tt;

    ac->yb = .25e0 * el->b1 * *d;
    x      = 1.0e0 - ac->yb;
    z0     = (1.0e0 + el->g1 * ac->yb + el->g2 * ac->yb * ac->yb) / (x*x*x);
    ac->zb = z0 + 4.0e0 * ac->yb * (el->b2 / el->b1 - el->gf);
    dz0    = (el->g1 + 2.0e0 * el->g2 * ac->yb) / (x * x * x) + 3.0e0 *
             (1.0e0 + el->g1 * ac->yb + el->g2 * ac->yb * ac->yb) / (x*x*x*x);
    ac->dzb= dz0 + 4.0e0 * (el->b2 / el->b1 - el->gf);
    ba.pb = ac->zb;
    ba.ab = -log(x) - (el->g2 - 1.0e0)/x + 28.16666667e0 / x / x +
            4.0e0* ac->yb *(el->b2 / el->b1 - el->gf) + 15.166666667e0 +
            log( *d * t * ac->gascon / .101325e0);
    ba.gb = ba.ab + ac->zb;
    ba.ub = -t * el->b1t * (ac->zb - 1.0e0 - *d * el->b2) / el->b1
            - *d * t * el->b2t;
    ba.sb = ba.ub - ba.ab;
    ba.hb = ac->zb + ba.ub;
    bb2tt  = t * t * el->b2tt;
    ba.cvb= 2.0e0 * ba.ub + (z0 - 1.0e0) * (((t * el->b1t / el->b1)
                                            * (t * el->b1t / el->b1)) - t * t * el->b1tt / el->b1)
            - *d * (bb2tt - el->gf * el->b1tt * t * t) -
            (t * el->b1t / el->b1) * (t * el->b1t / el->b1) * ac->yb * dz0;
    ba.dpdtb = ba.pb / t + *d * (ac->dzb * el->b1t / 4.0e0 + el->b2t -
                                 el->b2 / el->b1 * el->b1t);
}

//--------------------------------------------------------------------//
//calc density (d in g/cm3) and dP/dD (dPdd) as f(p(MPa), t(degK))
// from an initial density guess (dguess).
void TSupcrt::denHGK(double *d, double *p, double dguess, double t, double *dpdd)
{
    double pp, dp, dpdx, x;
    int  i= 0;
    *d  = dguess;
FIRST:
    do
    {
        i++;
        if (*d <= 0.0e0) *d = 1.0e-8;
        if (*d > 1.9e0)  *d = 1.9e0;
        resid(t,d);
        base(d,t);
        pp    = a1.rt * *d * ba.pb + qq.q0;
        *dpdd = a1.rt * (ac->zb + ac->yb * ac->dzb) + qq.q5;
        /*  if  dpdd < 0  assume d in 2-phase region and adjust accordingly */
        if (*dpdd > 0.0) goto OTHER;
        if (dguess >= 0.2967e0)   *d *= 1.02e0;
        if (dguess < 0.2967e0)    *d *= 0.98e0;
    }
    while (i <= 10);
    return;
OTHER:
    {
        dpdx = *dpdd * 1.1e0;
        if (dpdx < 0.1e0)
            dpdx = 0.1e0;
        dp   = fabs(1.0e0 - pp / *p);
        if ((dp < 1.0e-8) || ((dguess > 0.3e0) && (dp < 1.0e-7)) ||
                ((dguess > 0.7e0) && (dp < 1.0e-6)))
            return;
        x = (*p - pp) / dpdx;
        if (fabs(x)  >  0.1e0)
            x *= 0.1e0 / fabs(x);
        *d += x;
        if (*d <= 0.0e0)
            *d = 1.0e-8;
        if ( i <= 30 ) goto FIRST;
    }
}

//--------------------------------------------------------------------//
// calc t/d properties H2O as ideal gas: equat see Woolley (1979).
void TSupcrt::ideal(double t)
{
    int i=1;
    double  tt1, tl, emult;
    double c[18]={ .19730271018e2,    .209662681977e2,  -.483429455355e0,
                   .605743189245e1,   .2256023885e2,    -.987532442e1,
                   -.43135538513e1,    .458155781e0,     -.47754901883e-1,
                   .41238460633e-2,  -.27929052852e-3,   .14481695261e-4,
                   -.56473658748e-6,   .16200446e-7,     -.3303822796e-9,
                   .451916067368e-11,-.370734122708e-13, .137546068238e-15 };

    tt1  = t / 1.0e2;
    tl  = log(tt1);
    id.gi  = - (c[0] / tt1 + c[1]) * tl;
    id.hi  = (c[1] + c[0] * (1.0e0 - tl) / tt1);
    id.cpi = c[1] - c[0] / tt1;
    while ( ++i <= 17 )
    {
        emult = pow(tt1,((double)i - 5.));
        id.gi  -= c[i] * emult;
        id.hi  += c[i] * (i-5) * emult;
        id.cpi += c[i] * (i-5) * (i-4) * emult;
    }
    id.ai  = id.gi - 1.0e0;
    id.ui  = id.hi - 1.0e0;
    id.cvi = id.cpi - 1.0e0;
    id.si  = id.ui - id.ai;
}

//--------------------------------------------------------------------//
// calc t/d functions for HGK ( Helmholtz, Gibbs, et all)
void TSupcrt::thmHGK(double *d, double t)
{
    double  z;

    z = ac->zb + qq.q0 / a1.rt/ *d;
    fct.dpdd = a1.rt * (ac->zb + ac->yb * ac->dzb) + qq.q5;
    fct.ad   = ba.ab + res.ar + id.ai - ac->uref / t + ac->sref;
    fct.gd   = fct.ad + z;
    fct.ud   = ba.ub + res.ur + id.ui - ac->uref / t;
    fct.dpdt = a1.rt * *d * ba.dpdtb + res.dpdtr;
    fct.cvd  = ba.cvb + res.cvr + id.cvi;
    fct.cpd  = fct.cvd + t * fct.dpdt * fct.dpdt /
               (*d * *d * fct.dpdd * ac->gascon);
    fct.hd   = fct.ud + z;
    fct.sd   = ba.sb + res.sr + id.si - ac->sref;
    fct.dvdt = fct.dpdt / fct.dpdd / *d / *d;
    fct.cjtt = 1.0e0 / *d - t * fct.dvdt;
    fct.cjth = -fct.cjtt / fct.cpd / ac->gascon;
}

//--------------------------------------------------------------------//
// calc   (d(alpha)/dt)p(d,t,alpha) on equat Haar et al. (1983).
double TSupcrt::dalHGK(double *d, double t, double alpha)
{
    int i=-1;
    double  k, l, km, lm, /* kp,*/ lp, x, xtzt, dydtp, dbdd,
    db2dd, db2ddt, db2dtp, db3ddt, db3dtt, drdd, dr2dd, dr2ddt,
    dr2dtp, /* dr3ddt, dr3dtt,*/ e1, e2, tzt, ai, bi, di, ti, tau, del,
    /* ex1, ex2, ex12, */ qm, xdell, xdelk, dalHGK/*, dex, tex*/,
    term1, term2, term3, term4;
    long double /* db2dd, db2ddt, db2dtp, db3ddt, db3dtt,
                 drdd, dr2dd, dr2ddt,dr2dtp,*/ dr3ddt, dr3dtt,
    ex1, ex2, ex12, dex, tex, term5;
    /*   evaluate derivatives for the base function */
    ac->yb= .25e0 * el->b1 * *d;
    x     = 1.0e0 - ac->yb;
    dydtp = (*d / 4.0e0) * (el->b1t - el->b1 * alpha);
    dbdd  = ac->gascon * t * ((el->b1 / 4.0e0 / x) * (1.0e0 -
                              (el->g2 - 1.0e0) / x + (el->g1 + el->g2 + 1.0e0) /
                              x / x) + el->b2 - el->b1 * el->gf + 1.0e0 / *d);
    db2dd = ac->gascon * t * ((el->b1 * el->b1 / 16.0e0 / x / x) *
                              (1.0e0 - 2.0e0 * (el->g2 - 1.0e0) / x + 3.0e0 *
                               (el->g1 + el->g2 + 1.0e0) / x / x) - 1.0e0 / *d / *d);
    db2ddt = ac->gascon * t * ((el->b1t / 4.0e0 / x / x) *
                               (1.0e0 - (el->g2 - 1.0e0) * (1.0e0 + ac->yb) / x +
                                (el->g1 + el->g2 + 1.0e0) * (1.0e0 + 2.0e0 * ac->yb) /
                                x / x) + el->b2t - el->gf * el->b1t) + dbdd / t;
    db2dtp = dbdd / t + ac->gascon * t * ((el->b1 * dydtp / 4.0e0 /
                                           x / x / x) * (1.0e0 - el->g2 + 2.0e0 * (el->g1 +
                                                                                   el->g2 + 1.0e0) / x) + ((x * el->b1t + el->b1 *
                                                                                                            dydtp) / 4.0e0 / x / x) * (1.0e0 - (el->g2 - 1.0e0) /
                                                                                                                                       x + (el->g1 + el->g2 + 1.0e0) / x / x) + el->b2t -
                                          el->gf * el->b1t + alpha / *d );
    db3ddt = db2dd / t + ac->gascon * t * ((el->b1 * el->b1 *
                                            dydtp / 8.0e0 / x / x / x / x) * (1.0e0 - el->g2 +
                                                                              3.0e0 * (el->g1 + el->g2 + 1.0e0) / x) + (el->b1 *
                                                                                      (x * el->b1t + el->b1 * dydtp) / 8.0e0 / x / x / x) *
                                           (1.0e0 - 2.0e0 * (el->g2 - 1.0e0) / x + 3.0e0 *
                                            (el->g1 + el->g2 + 1.0e0) / x / x) - 2.0e0 * alpha /
                                           *d / *d );
    db3dtt = (db2ddt - dbdd / t) / t + ac->gascon * t * ((el->b1t *
             dydtp / 2.0e0 / x / x / x / x) * (1.0e0 - el->g2 +
                                               (el->g1 + el->g2 + 1.0e0) * (2.0e0 + ac->yb) / x) +
             ((x * el->b1tt + 2.0e0 * el->b1t * dydtp) / 4.0e0 /
              x / x / x) * (1.0e0 - (el->g2 - 1.0e0) * (1+ ac->yb) /
                            x + (el->g1 + el->g2 + 1.0e0) * (1.0e0 + 2.0e0 * ac->yb) /
                            x / x) + el->b2tt - el->gf * el->b1tt ) + (t * db2dtp -
                                    dbdd) / t / t;
    /***********************************************************/

    /*   evaluate derivatives for the residual function  */

    /*      drdd   = q/d/d                  */
    /*      dr2dd  = (q5 - 2.0e0/d*q0)/d/d   */
    /*      dr2ddt = dpdtr/d/d              */

    e1  = exp(-ac->aa * *d);
    e2  = 1.0e0 - e1;
    tzt = ac->tz / t;
    drdd   = 0.0e0;
    dr2dd  = 0.0e0;
    dr2ddt = 0.0e0;
    dr2dtp = 0.0e0;
    dr3ddt = 0.0e0;
    dr3dtt = 0.0e0;
    /*    evaluate terms 1-36      */
    while (++i < nc->nc)
    {
        k = nc->ii[i] + 1.0e0;
        l = nc->jj[i] - 1.0e0;
        km = k - 1.0e0;
        //   lm = l - 1.0e0;
        /*  kp = k + 1.0e0; */
        lp = l + 1.0e0;
        xtzt = pow(tzt,l);
        drdd   = drdd + nc->g[i] * xtzt * pow(e2,km) * e1;
        dr2dd  = dr2dd + nc->g[i] * e1 * xtzt * pow(e2,km) *
                 (km * e1 / e2 - 1.0e0);
        dr2ddt = dr2ddt - nc->g[i] * e1 * l * pow(e2,km) * pow(tzt,lp) /
                 ac->tz;
        dr2dtp = dr2dtp + nc->g[i] * e1 * pow(e2,km) * xtzt *
                 ( *d * alpha - l / t - km * e1 * *d * alpha / e2 );
        dr3ddt = dr3ddt + nc->g[i] * (km * *d * alpha * e1 * e1 * xtzt *
                                      pow(e2,(k-3.0e0)) + e1 * xtzt * pow(e2,km) * (km*e1/e2 -
                                              1.0e0) * (*d * alpha - l / t - km * *d * alpha * e1 / e2) );
        dr3dtt = dr3dtt + nc->g[i] * l * e1 * pow(e2,km) * pow(tzt,lp) /
                 ac->tz * (lp / t + *d * alpha * km * e1 / e2 - *d * alpha );
    }

    /*   evaluate terms 37-40   */

    i=35;
    while (++i <= 39)
    {
        k  = nc->ii[i];
        l  = nc->jj[i];
        km = k - 1.0e0;
        lm = l - 1.0e0;
        /*   kp = k + 1.0e0; */
        //  lp = l + 1.0e0;
        ai = ad->aad[i-36];
        bi = ad->aat[i-36];
        di = ad->adz[i-36];
        ti = ad->atz[i-36];
        tau = t / ti - 1.0e0;
        del = *d / di - 1.0e0;

        if (fabs(del) < 1.0e-10)
            del = 1.0e-10;
        ex1 = -ai * pow(del,k);
        if (ex1 < to->EXPTOL)
            dex = 0.0e0;
        else
            dex = exp(ex1);
        ex2  = -bi * tau * tau;
        if (ex2 <= to->EXPTOL)
            tex = 0.0e0;
        else
            tex = exp(ex2);
        ex12    = dex * tex;
        qm      = l / del - k * ai * pow(del,km);
        xdell   = pow(del,l);
        xdelk   = pow(del,k);
        drdd   += nc->g[i] * xdell * ex12 / di * qm;

        dr2dd  += nc->g[i] * xdell * ex12 / di / di * (qm * qm -
                  l / di / di - ai * k * km * pow(del,k-2.0e0));

        dr2ddt -= nc->g[i] * 2.0e0 * bi * tau * ex12 * xdell / ti / di * qm;

        dr2dtp += nc->g[i] / di * (*d * alpha * xdell * ex12 / di / del/del *
                                   (l + ai * k * km * xdelk) + qm * ( ex12 * (xdell* (k * ai *
                                                                      *d * alpha * pow(del,km) / di - 2.0e0 * bi * tau / ti) - l *
                                                                      *d * alpha * pow(del,lm)/di)));

        term1 = l * *d * alpha / di / del / del + ai * k * km * *d * alpha *
                pow(del,k-2.) / di;
        term2 = 2.* qm * term1 - 2.0e0 * l * *d * alpha / di / del / del /
                del + ai * k * km * (k-2.) * pow(del,k-3.) * *d * alpha / di;
        term3 = qm * qm - l / del / del - ai * k * km * pow(del,k-2.);
        term4 = ai * k * pow(del,k-1.) * *d * alpha / di - 2.0e0 * bi * tau / ti;
        term5 = ex12 * xdell * term4 - ex12 * l * pow(del,l-1.) * *d * alpha / di;

        dr3ddt += nc->g[i] / di / di * (xdell * ex12 * term2 + term3 * term5);

        dr3dtt -= 2.0e0 * nc->g[i] * bi / ti / di * (tau * xdell * ex12 * *d *
                  alpha / del / del / di * (l + ai * k * km * pow(del,k)) + qm *
                  (xdell * ex12 * (ai * k * *d * alpha * tau * pow(del,km) / di +
                                   (1.0e0 - 2.0e0 * bi * tau * tau) / ti - tau * l * *d * alpha /
                                   di / del )));
    }

    /*   compute (d(alpha)/dT)P   */

    dalHGK = ((db3dtt + dr3dtt) * (2.0e0 * (dbdd + drdd) + *d * (db2dd +
                                   dr2dd)) - (db2ddt + dr2ddt) * (2.0e0 * (db2dtp + dr2dtp) +
                                                                  *d * (db3ddt + dr3ddt) - *d * alpha * (db2dd + dr2dd))) /
             (2.0e0 * (dbdd + drdd) + *d * (db2dd + dr2dd)) / (2.0e0 *
                     (dbdd + drdd) + *d * (db2dd + dr2dd));

    return(dalHGK);
}

//--------------------------------------------------------------------//
// din. viscosity H2O in kg/m*s, if Pbars in validity region
// specified  by the  initial if  statement Watson et al. (1980);
//See  statement 3.1-2 and 4.1-5 in Tabl. 1,6,8 in Sengers and Kamgar-Parsi (1984).
double TSupcrt::viscos(double Tk, double Pbars, double Dkgm3, double betaPa)
{
    double  a[4] = { 0.0181583e0,  0.0177624e0,
                     0.0105287e0, -0.0036744e0  };
    double b[7][6] =
        {
            { 0.5132047e0, 0.3205656e0, 0.0e0, 0.0e0,-0.7782567e0, 0.1885447e0 },
            { 0.2151778e0, 0.7317883e0, 1.2410440e0, 1.4767830e0, 0.0e0, 0.0e0 },
            {-0.2818107e0,-1.0707860e0,-1.2631840e0, 0.0e0, 0.0e0, 0.0e0 },
            { 0.1778064e0, 0.4605040e0, 0.2340379e0,-0.4924179e0, 0.0e0, 0.0e0 },
            {-0.0417661e0, 0.0e0, 0.0e0, 0.1600435e0, 0.0e0, 0.0e0 },
            { 0.0e0,-0.01578386e0, 0.0e0, 0.0e0, 0.0e0, 0.0e0  },
            { 0.0e0, 0.0e0, 0.0e0,-0.003629481e0, 0.0e0, 0.0e0 }
        };
    double Tstar, Dstar, Pstar, ustar, TOL, vIscos, TdegC, sum,
    T, D, u0, u1, u2, xt;
    int i=-1, j=-1;

    Tstar  = 647.270e0;
    Dstar  = 317.763e0;
    Pstar  = 22.1150e6;
    ustar  = 1.0e-6;
    TOL    = 1.0e-2;
    vIscos = 0.0e0;
    TdegC  = Tk - 273.15e0;

    if ((Pbars > 5000.0e0 + TOL) ||
            ((Pbars > 3500.0e0 + TOL) && (TdegC > 150.0e0 + TOL)) ||
            ((Pbars > 3000.0e0 + TOL) && (TdegC > 600.0e0 + TOL)) ||
            (TdegC  >  900.0e0 + TOL))
        return(vIscos);
    T = Tk / Tstar;
    D = Dkgm3 / Dstar;
    sum = 0.0e0;
    while(++i <= 3 )
    {
        sum += a[i] / pow(T,i);
    }
    u0 = ustar * sqrt(T) / sum;
    sum = 0.0e0;
    i=-1;
    while (++i <= 5)
    {
        while (++j <= 6)
        {
            sum += b[j][i] * pow((1.0e0/T-1),i) * pow((D-1),j);
        }
    }
    u1 = exp(D * sum);
    if( (0.997e0 <= T) && (T <= 1.0082e0) &&
            (0.755e0 <= D) && (D <= 1.2900e0) )
    {
        xt = Pstar / pow(Dstar,2.) * betaPa * pow(Dkgm3,2.);
        if (xt < 22.0e0)
            u2 = 1.0e0;
        else
            u2 = 0.922 * pow(xt,0.0263e0);
    }
    else
        u2 = 1.0e0;
    vIscos = u0 * u1 * u2;
    return(vIscos);
}

//--------------------------------------------------------------------//
// thermal conductivity H2O in J/m*deg*s (=W/m*deg),
// specified by the initial IF statement equat Sengers et al. (1984);
//See  statement 3.2.14 in Tabl.2-5 and I.5-6 from the above reference.
double TSupcrt::thcond(double Tk, double Pbars, double Dkgm3, double alph,
                       double betaPa)
{
    int i=-1, j=-1;
    double TOL, Tstar, Dstar, Pstar, ustar, C, TdegC;
    double thCOND, T, D, sum, L0, L1, L2, u0, u1, xt, dPdT;
    double aL[4] = { 0.2022230e1, 0.1411166e2, 0.5255970e1,-0.2018700e1 };
    double au[4] = { 0.0181583e0, 0.0177624e0, 0.0105287e0,-0.0036744e0 };
    double bL[5][6] =
        {
            { 1.329304600e0,-0.404524370e0, 0.244094900e0, 0.018660751e0,
              -0.129610680e0, 0.044809953e0                                 },
            { 1.701836300e0,-2.215684500e0, 1.651105700e0,-0.767360020e0,
              0.372833440e0,-0.112031600e0                                 },
            { 5.224615800e0,-1.012411100e1, 4.987468700e0,-0.272976940e0,
              -0.430833930e0, 0.133338490e0                                 },
            { 8.712767500e0,-9.500061100e0, 4.378660600e0,-0.917837820e0,
              0.0e0, 0.0e0                                                 },
            {-1.852599900e0, 0.934046900e0, 0.0e0, 0.0e0, 0.0e0, 0.0e0     }
        };
    double bu[6][5] =
        {
            { 0.5019380e0, 0.2356220e0,-0.2746370e0, 0.1458310e0,-0.0270448e0 },
            { 0.1628880e0, 0.7893930e0,-0.7435390e0, 0.2631290e0,-0.0253093e0 },
            {-0.1303560e0, 0.6736650e0,-0.9594560e0, 0.3472470e0,-0.0267758e0 },
            { 0.9079190e0, 1.2075520e0,-0.6873430e0, 0.2134860e0,-0.0822904e0 },
            {-0.5511190e0, 0.0670665e0,-0.4970890e0, 0.1007540e0, 0.0602253e0 },
            { 0.1465430e0,-0.0843370e0, 0.1952860e0,-0.0329320e0,-0.0202595e0 }
        };
    TOL    = 1.0e-2;
    Tstar  = 647.270e0;
    Dstar  = 317.763e0;
    Pstar  = 22.1150e6;
    ustar  = 1.0e-6;
    C      = 3.7711e-8;
    thCOND = 0.0e0;
    TdegC  = Tk - 273.15e0;

    if ((Pbars > 4000.0e0+TOL) ||
            ((Pbars > 2000.0e0+TOL) && (TdegC > 125.0e0+TOL)) ||
            ((Pbars > 1500.0e0+TOL) && (TdegC > 400.0e0+TOL)) ||
            (TdegC  > 800.0e0+TOL))
        return(thCOND);
    T = Tk / Tstar;
    D = Dkgm3 / Dstar;
    sum = 0.0e0;
    while (++i <= 3)
    {
        sum += aL[i] / pow(T,(double)i);
    }
    L0 = sqrt(T) / sum;
    sum = 0.0e0;
    i=-1;
    while(++i <= 4)
    {
        while (++j <= 5)
        {
            sum += bL[i][j] * pow((1.0e0 / T-1),(double)i) * pow((D-1),(double)j);
        }
    }
    L1 = exp(D*sum);
    sum = 0.0e0;
    i=-1;
    while (++i <= 3)
    {
        sum += au[i] / pow(T,(double)i);
    }
    u0 = ustar * pow(T,0.5) / sum;
    sum = 0.0e0;
    i=-1;
    j=-1;
    while (++i <= 5)
    {
        while (++j <= 4)
        {
            sum += bu[i][j] * pow((1.0e0 / T-1),(double)i) *
                   pow( (D - 1), (double)j );
        }
    }
    u1   = exp(D * sum);
    xt   = Pstar / pow(Dstar,2.) * betaPa * pow(Dkgm3,2.);
    dPdT = Tstar / Pstar * alph/betaPa;

    L2   = C / (u0 * u1) * pow((T / D),2.) * pow(dPdT,2.) *
           pow( std::abs(xt),0.4678e0) * pow(D,0.5) * exp(-18.66e0 *
                                               pow((T - 1),2.) - pow((D - 1),4.));
    thCOND = L0 * L1 + L2;
    return(thCOND);
}

//--------------------------------------------------------------------//
// Vargaftik et al. (1983). See  statement 10.1-2, in Kestin et al. (1984);
// and C.5 in Tabl. 11, Haar et al. (1984).
double TSupcrt::surten(double Tsatur)
{
    double Ttripl, Tcrit, Tstar, Tcstar, v, B, stref, FPTOL;
    double sUrten, Tnorm;

    Ttripl = 273.16e0;
    Tcrit  = 647.067e0;
    Tstar  = 647.27e0;
    Tcstar = 0.999686e0;
    v      =  1.256e0;
    B      = -0.625e0;
    stref  = 0.2358e0;
    FPTOL  = 1.0e-10;

    if ((Tsatur < Ttripl) || (Tsatur > Tcrit))
    {
        sUrten = 0.0e0;
        return(sUrten);
    }

    if (Tsatur >= Tcrit - FPTOL)
        Tnorm = 0.0e0;
    else
        Tnorm = (Tcstar - Tsatur / Tstar) / Tcstar;

    sUrten = stref * pow(Tnorm,v) * (1.0e0 + B * Tnorm);
    return(sUrten);
}

//--------------------------------------------------------------------//
/* Born92 - calculated Z, Q, Y, X .
*           epseqn = 1 statement Helgeson-Kirkham (1974)
*           epseqn = 2 statement Pitzer (1983)
*           epseqn = 3 statement Uematsu-Franck (1980)
*           epseqn = 4 statement Johnson-Norton (1991) !!!
*           epseqn = 5 statement Archer-Wang (1990)      */
void TSupcrt::Born92(double TK, double Pbars, double Dgcm3, double betab,
                     double *alphaK, double *daldT, double *eps, double *Z,
                     double *Q, double *Y, double *X, int epseqn)
{
    //int iRET = 0;
    double TMAX, PMAX, TOL, TdegC, dedP, dedT, d2edT2;

    TMAX = 1000.0e0;
    PMAX = 5000.0;
    TOL  = 1.0e-3;

    *eps = 0.0e0;
    *Z   = 0.0e0;
    *Y   = 0.0e0;
    *Q   = 0.0e0;
    *X   = 0.0e0;
    TdegC = TK - 273.15e0;
    /*  The following line can be commented out to facilitate probably
    *   unreliable, yet potentially useful, predictive calculations
    *   at state conditions beyond the validity limits of the aqueous
    *   species equation of state.                                   */

    if ( (TdegC > TMAX + TOL) || (Pbars > PMAX + TOL) )
        return;
    if (epseqn == 4)
    {
        /* d2edT2=0.; */
        JN91(TK, Dgcm3, betab, alphaK, daldT, eps, &dedP, &dedT,
             &d2edT2);
        epsBrn(eps, dedP, dedT, d2edT2, Z, Q, Y, X);
    }
}

//--------------------------------------------------------------------//
/* JN91 -Calculated (eps, dedP, dedT, d2edT2)(T,D) on statements
*         Johnson and Norton (1991);   fit regressed  from least
*         squares (HK74 and Pitz83 )
*           DATA type: T ............... K
*                    D ............... g/cm**3
*                    beta, dedP ...... bar**(-1)
*                    alpha, dedT ..... K**(-1)
*                    daldT, d2edT2 ... K**(-2)                         */
void TSupcrt::JN91(double T, double D, double beta, double *alpha, double *daldT,
                   double *eps, double *dedP, double *dedT, double *d2edT2)
{
    int j=-1, k=0;
    double c[5], dcdT[5], dc2dTT[5];
    double Tref = 298.15e0, Tn;

    double a[10] = { 0.1470333593e+02, 0.2128462733e+03,
                     -0.1154445173e+03, 0.1955210915e+02,
                     -0.8330347980e+02, 0.3213240048e+02,
                     -0.6694098645e+01,-0.3786202045e+02,
                     0.6887359646e+02,-0.2729401652e+02 };

    Tn        = T / Tref;
    c[0]      = 1.0e0;
    dcdT[0]   = 0.0e0;
    dc2dTT[0] = 0.0e0;

    c[1]      = a[0] / Tn;
    dcdT[1]   = -a[0] * Tref / pow(T,2.);
    dc2dTT[1] = 2.0e0 * a[0] * Tref / pow(T,3.);

    c[2]      = a[1] / Tn + a[2] + a[3] * Tn;
    dcdT[2]   = -a[1] * Tref / pow(T,2.) + a[3] / Tref;
    dc2dTT[2] = 2.0e0 * a[1] * Tref / pow(T,3.);

    c[3]      = a[4] / Tn + a[5] * Tn + a[6] * pow(Tn,2.);
    dcdT[3]   = -a[4] * Tref / pow(T,2.) + a[5] / Tref + 2.0e0 *
                a[6] * T / pow(Tref,2.);
    dc2dTT[3] = 2.0e0 * a[4] * Tref / pow(T,3.) + 2.0e0 * a[6] / pow(Tref,2.);

    c[4]      = a[7] / pow(Tn,2.) + a[8] / Tn + a[9];
    dcdT[4]   =-2.0e0 * a[7] * pow(Tref,2.) / pow(T,3.) - a[8] * Tref /
               pow(T,2.);
    dc2dTT[4] = 6.0e0 * a[7] * pow(Tref,2.) / pow(T,4.) + 2.0e0 * a[8] *
                Tref / pow(T,3.);

    *eps = 0.0e0;

    while (++k <= 5)
    {
        *eps += c[k-1] * pow(D,(double)(k - 1));
    }
    *dedP = 0.0e0;
    while (++j <=  4)
    {
        *dedP += j * c[j] * pow(D,(double)j);
    }
    *dedP *= beta;
    *dedT = 0.0e0;
    j=-1;
    while (++j <= 4)
    {
        *dedT += pow(D,(double)j) * (dcdT[j] - j * *alpha * c[j]);
    }
    *d2edT2 = 0.0e0;
    j=-1;
    while (++j <= 4)
    {
        *d2edT2 += pow(D,(double)j) * (dc2dTT[j] - j * (*alpha *
                                       dcdT[j] + c[j] * *daldT) - j * *alpha *
                                       (dcdT[j] - j * *alpha * c[j]));
    }
}

//--------------------------------------------------------------------//
/* epsBrn - calc Brown functions Z, Q, Y, and X from their eps, dedP, dedT
   and d2edT2 counterparts.           */
void TSupcrt::epsBrn(double *eps, double dedP, double dedT,double d2edT2,
                     double *Z, double *Q, double *Y, double *X)
{
    *Z = -1.0e0 / *eps;
    *Q =  1.0e0 / pow(*eps,2.) * dedP;
    *Y =  1.0e0 / pow(*eps,2.) * dedT;
    *X =  1.0e0 / pow(*eps,2.) * d2edT2 - 2.0e0 * *eps * pow(*Y,2.);
}

//--------------------------------------------------------------------//
/* triple - translate values U, S, H, A, G zero triple point
*  properties (Haar et al., 1984; Levelt Sengers et al., 1983) referenced
*  to triple  point properties data in Helgeson and   Kirkham, 1974a. */
void TSupcrt::triple(double T, WPROPS  *wr1)
{
    double TS;

    wr1->Sw    = wr1->Sw + tt->Stri;
    TS        = T * wr1->Sw - tt->Ttr * tt->Stri;
    wr1->Gw = wr1->Hw - TS + tt->Gtri;
    wr1->Aw = wr1->Uw - TS + tt->Atri;
    wr1->Hw = wr1->Hw + tt->Htri;
    wr1->Uw = wr1->Uw + tt->Utri;
}


//--------------------------------------------------------------------//
void TSupcrt::dimHGK(int isat,
                     int itripl, double t, double *p, double *d, int epseqn)
{
    double pbars, dkgm3, betaPa, betab, CpJKkg;

    wr.Aw   = fct.ad * a1.rt * un.fh;
    wr.Gw   = fct.gd * a1.rt * un.fh;
    wr.Sw   = fct.sd * ac->gascon * un.fh * un.ft;
    wr.Uw   = fct.ud * a1.rt * un.fh;
    wr.Hw   = fct.hd * a1.rt * un.fh;
    wr.Cvw  = fct.cvd * ac->gascon * un.fh * un.ft;
    wr.Cpw  = fct.cpd * ac->gascon * un.fh * un.ft;
    wr.Speedw = sqrt(fabs(fct.cpd * fct.dpdd * 1.0e3 /
                          fct.cvd)) * un.fs;
    wr.Betaw  = 1.0e0 / (*d * fct.dpdd * un.fp);
    wr.Alphaw = *d * fct.dvdt;
    wr.dAldT  = dalHGK(d,t,wr.Alphaw);                 /*!!!*/
    pbars = *p * 1.0e1;
    dkgm3 = *d * 1.0e3;
    betaPa = wr.Betaw * un.fp / 1.0e6;
    betab  = wr.Betaw * un.fp / 1.0e1;
    CpJKkg = wr.Cpw / un.fh / un.ft * 1.0e3;
    wr.Viscw  = viscos(t,pbars,dkgm3,betaPa) * un.fvd;
    wr.Tcondw = thcond(t,pbars,dkgm3,wr.Alphaw,betaPa) * un.fc * un.ft;

    if ((isat == 0) || (isat == 2))
        wr.Surtenw = 0.0e0;
    else
        wr.Surtenw = surten(t) * un.fst;

    Born92(t,pbars,dkgm3/1.0e3,betab,&wr.Alphaw,&wr.dAldT,
           &wr.Dielw,&wr.ZBorn,&wr.QBorn,&wr.YBorn,&wr.XBorn,epseqn);
    wr.Tdiffw = wr.Tcondw / un.fc / un.ft / (dkgm3 * CpJKkg) * un.fvk;
    if ( noZero(wr.Tcondw) )
        wr.Prndtlw = wr.Viscw / un.fvd * CpJKkg /
                     (wr.Tcondw / un.fc / un.ft);
    else
        wr.Prndtlw = 0.0e0;
    wr.Visckw = wr.Viscw / un.fvd / dkgm3 * un.fvk;
    wr.Albew  = wr.Alphaw / wr.Betaw;

    if (itripl == 1)
        triple(t, &wr);
}

//--------------------------------------------------------------------//
// (dliq & dvap),   (Gl-Gv)/RT  (delg)
void  TSupcrt::corr(int itripl, double t, double *p, double *dL,
                    double *dV, double *delg, int epseqn)
{
    double dguess, gl, gv;
    bb(t);
    dguess = *dL;
    if (*dL <= 0.0e0)
        dguess = 1.11e0 - 0.0004e0 * t;
    denHGK(dL,p,dguess,t, &fct.dpdd);
    ideal(t);
    thmHGK(dL,t);
    dimHGK(1,itripl,t,p,dL,epseqn);
    gl     = fct.gd;
    dguess = *dV;
    if (*dV <= 0.0e0)
        dguess = *p / a1.rt;
    denHGK(dV,p,dguess,t,&fct.dpdd);
    if (*dV < 5.0e-7)
        *dV = 5.0e-7;
    ideal(t);
    thmHGK(dV,t);
    /*    ᢮      ᮤ    FCTS
     *      ୮  (dimensionless); after
     *    pcorr's final call of corr (*delg < 10d-4)   */
    gv    = fct.gd;
    *delg = gl - gv;
}

//--------------------------------------------------------------------//
void TSupcrt::pcorr(int itripl, double t, double *p, double *dL,
                    double *dV, int epseqn)
{
    double dp;
    *p  = PsHGK(t);
    *dL= 0.0e0;
    *dV = 0.0e0;
    do
    {
        corr(itripl, t, p, dL, dV, &trp.delg, epseqn);
        dp   = trp.delg * ac->gascon * t / (1.0e0 / *dV - 1.0e0 / *dL);
        *p  += dp;
    }
    while ( fabs(trp.delg) > 1.0e-4 );
}

//--------------------------------------------------------------------//
// conver - Transform T,D to r,theta  and scaled equations.
void  TSupcrt::conver(double *rho,
                      double Tee, double rho1s, double *rhodi, double error1)
{
    int isig=0;
    double error2;
    double /*sd[2]*/ beta, delta, xk1, cc, besq, p11, aa, xk0,  betai,
    Tstar, dtstin, rhoweg, rhodit, drho, rho1co, twofaz, hold,
    y1, den1, den2, den12, tt1, rho1s2, slope;

    /* double alhi, alpha, deli, p1w, p2w, p4w, s00, s20; */
    beta  =co->a[5];
    delta =co->a[10];
    xk1   =co->a[11];
    cc    =co->a[0];
    /* alhi  =co->q[14];
       alpha =co->q[9];      */
    besq  =co->a[8];
    p11   =co->q[8];
    /* deli  =co->q[13];     */
    /* p1w =co->q[17];
       p2w   =co->q[18];
       p4w   =co->q[19];     */
    aa    =co->a[9];
    xk0   =co->a[6];
    /* s00 =co->a[16];       */
    /* s20 =co->a[17];       */
    betai =co->q[15];

    Tstar  = Tee + 1.0;
    dtstin = 1.0 - (1.0 / Tstar);
    par.r1     = dtstin;
    if (dtstin <= 0.0)
    {
        par.r1  = dtstin / (1.0 - besq);
        par.th1 = 1.0;
    }
    else
        par.th1 = 0.0;

    ss(par.r1, par.th1, dv.s, dv.sd);
    *rhodi   = 1.0 + p11 * dtstin;
    rhodit = *rhodi  + cc * dv.s[0] + cc * dv.s[1];
    drho   = *rho - rhodit;
    dv.amu    = 0.0;
    if (dtstin <= 0.0)
    {
        rho1co = xk0 * pow(par.r1,beta) + xk1 * pow(par.r1,betai);
        twofaz = rho1co;
        if (fabs(drho) <= twofaz)
        {
            //rho1s   = (rho1co * drho / fabs(drho)) + cc * dv.s[0];
            par.th1    = drho * fabs(drho);
            // error1 = 1.0;
            a2.r = par.r1;
            a2.th = par.th1;
            return;
        }
    }
    if ( approximatelyZero(drho) )
    {
        par.th1   = 0.0;
        par.r1    = dtstin;
        //rho1s  = cc * dv.s[0];
    }

    /*   rule for first pass   */
    y1   = dtstin;
    den1 = *rho - rhodit;

    rtheta(&par.r1, &par.th1, den1, y1);
    tt1       = par.th1 * par.th1;
    dv.amu  = aa * pow(par.r1,(beta * delta)) * par.th1 * (1.0 - tt1);
    y1       = dtstin + cc * dv.amu;

    ss(par.r1, par.th1, dv.s, dv.sd);
    rhoweg = xk1 * pow(par.r1,betai) * par.th1 + cc * dv.s[1];
    rho1s   = den1 + cc * dv.s[0] + rhoweg;
    error1 = *rho - *rhodi  - rho1s ;
    a2.r  = par.r1;
    a2.th = par.th1;

    if (fabs(error1) < 1.0e-5)
        return;

    /*   rule for second pass  */

    den12 = *rho - *rhodi  - cc * dv.s[1] + rhoweg;
    if (  approximatelyEqual( den12, den1 ) )
        den12 = den1 - 1.0e-6;

    rtheta(&par.r1,&par.th1,den12,y1);
    tt1      = par.th1 * par.th1;
    dv.amu = aa * pow(par.r1,(beta * delta)) * par.th1 * (1.0 - tt1);
    y1      = dtstin + cc * dv.amu;

    ss(par.r1, par.th1, dv.s, dv.sd);
    rhoweg = xk1 * pow(par.r1,betai) * par.th1 + cc * dv.s[1];
    rho1s2 = den12 + cc * dv.s[0] + rhoweg;
    error2 = *rho - *rhodi  - rho1s2;
    if (fabs(error2) <= 1.0e-5)
    {
        a2.r  = par.r1;
        a2.th = par.th1;
        //error1 = error2;
        // rho1s   = rho1s2;
        return;
    }

    /*    rule for nth pass   */

    den2   = den12;
    while( ++isig <= 10 )
    {
        slope  = (error2 - error1) / (den2 - den1);
        hold   = den2;
        den2   = den1 - (error1 / slope);

        rtheta(&par.r1,&par.th1,den2,y1);
        tt1      = par.th1 * par.th1;
        dv.amu = aa * pow(par.r1,(beta * delta)) * par.th1 * (1.0 - tt1);
        y1      = dtstin + cc * dv.amu;

        ss(par.r1, par.th1, dv.s, dv.sd);
        rhoweg = xk1 * pow(par.r1,betai) * par.th1 + cc * dv.s[1];
        rho1s   = den2 + cc * dv.s[0] + rhoweg;
        error1 = error2;
        error2 = *rho - *rhodi  - rho1s ;
        a2.r  = par.r1;
        a2.th = par.th1;
        if (fabs(error2) < 1.0e-6)
            return;
        den1 = hold;
    }
}

//--------------------------------------------------------------------//
//  ss - calc terms of the summation zat defined dPotl/dT and 1-th
// proizvodnuyu theta (s)  square polynomial.
void TSupcrt::ss(double r, double th1, double *s, double *sd) /*  s[2], sd[2] */
{
    double  sx[2];
    double alpha, beta, alhi, beti, s00, s20, s01, s21, tt1;
    /* double besq, delta, deli, gami, p00, p01;*/

    alpha= co->q[9];
    beta = co->a[5];
    /* besq = co->a[8];  */
    /* delta= co->a[10];
    deli = co->q[13];    */
    alhi = co->q[14];
    beti = co->q[15];
    /* gami = co->q[16];
    p00  = co->q[10];    */
    /* p01  = co->q[17]; */
    s00  = co->a[16];
    s20  = co->a[17];
    s01  = co->a[18];
    s21  = co->a[19];

    tt1    = th1 * th1;
    sx[0] = s00 + s20 * tt1;
    sd[0] = 2.0 * s20 * th1;
    sx[1] = s01 + s21 * tt1;
    sd[1] = 2.0 * s21 * th1;
    s[0]  = sx[0] * co->a[9] * co->a[6] * pow(r,(1.0 - alpha));
    s[1]  = sx[1] * co->a[9] * co->a[11] * pow(r,(1.0 - alhi));

    a1.dPdM  = pow(r,beta) * co->a[6] * th1 + co->a[0] * pow(r,(1.0-alpha)) *
               co->a[9] * co->a[6] * sx[0] + pow(r,beti) * co->a[11] * th1 +
               co->a[0] * pow(r,(1.0-alhi)) * co->a[9] * co->a[11] * sx[1];
}

//--------------------------------------------------------------------//
// rtheta - Fits data for 1.0 < theta < 1.000001. Calc:
//                        rho = em*theta*(r**beta)
//                        Tee = r*(1.0-besq*theta*theta)
//  Routine given by Moldover (1978): Jour. Res. NBS, v. 84, n. 4, p. 329 - 334.
void TSupcrt::rtheta(double *r, double *theta, double rho, double Tee)
{
    int n=0;
    double  beta, em, besq, absrho, bee, z, z2, z3, c, dz, tr, tmr;

    beta = co->a[5];
    em   = co->a[6];
    besq = co->a[8];
    if (em <= 0.0 || besq <= 1.0)
        goto MET600;

    absrho = fabs(rho);
    if (absrho < 1.0e-12)
        goto MET600;

    bee = sqrt(besq);
    if (fabs(Tee) < 1.0e-12)
        goto  MET495;

    if (Tee < 0.0)
        z = 1.0-(1.0-bee) * Tee / (1.0 - besq) * pow(em / absrho,1.0 / beta);
    else
        z = pow(1.0 + Tee * pow(em / bee / absrho,1.0 / beta), - beta);

    if (z > 1.00234 * bee)
        goto MET496;

    tr = fabs(Tee);
    c  = -rho * bee / em / pow(tr,beta);
    z  = z * rho / fabs(rho);
    while ( ++n <= 16)
    {
        z2 = z * z;
        z3 = 1.0 - z2;
        tmr= fabs(z3);
        dz = z3 * (z + c * pow(tmr,beta)) / (z3 + 2.0 * beta * z2);
        z  = z - dz;
        if (fabs(dz/z) < 1.0e-12)
            goto MET498;
    }

MET601:
    if (fabs(*theta) > 1.0001)
        *theta /= fabs(*theta);
    return;

MET498:
    *theta = z / bee;
    *r     = Tee /(1.0 - z * z);
    *r     = fabs(*r);
    return;

MET495:
    *theta = rho / fabs(rho) / bee;
    *r     = pow(rho / (em * (*theta)),1.0 / beta);
    return;

MET496:
    *theta = rho / fabs(rho);
    *r     = Tee / (1.0 - besq);
    *r     = fabs(*r);
    return;

MET600:
    if (fabs(Tee) < 1.0e-12)
        goto MET601;

    if (Tee < 0.0)
        goto MET496;
    *theta = 1.0e-12;
    *r     = Tee;
}

void TSupcrt::load_metastable(/*int phase,*/ WATERPARAM& wp)
{
    wp.Aw[0]     = wl.Aw;
    wp.Gw[0]     = wl.Gw;
    wp.Sw[0]     = wl.Sw;
    wp.Uw[0]     = wl.Uw;
    wp.Hw[0]     = wl.Hw;
    wp.Dielw[0]  = wl.Dielw;
    wp.ZBorn[0]  = wl.ZBorn;
    wp.Surtenw[0]= wl.Surtenw;
    wp.Cvw[0]    = wl.Cvw;
    wp.Cpw[0]    = wl.Cpw;
    wp.Alphaw[0] = wl.Alphaw;
    wp.Betaw[0]  = wl.Betaw;
    wp.YBorn[0]  = wl.YBorn;
    wp.QBorn[0]  = wl.QBorn;
    wp.XBorn[0]  = wl.XBorn;
    wp.Tcondw[0] = wl.Tcondw;
    wp.Tdiffw[0] = wl.Tdiffw;
    wp.Prndtlw[0]= wl.Prndtlw;
    wp.dAldT[0]  = wl.dAldT;
    wp.Albew[0]  = wl.Albew;
    wp.Speedw[0] = wl.Speedw;
    wp.Viscw[0]  = wl.Viscw;
    wp.Visckw[0] = wl.Visckw;
    wp.Gigw[0]   = id.gi;
    wp.Sigw[0]   = id.si;
    wp.Cpigw[0]  = id.cpi;
}

void TSupcrt::load(int phase, WATERPARAM& wp)
{
    if( !phase )
    {   // single phase region or vapor at Psat
    	// added water ideal gas properties (TW, 06.02.2008)
        wp.Aw[0]     = wr.Aw;
        wp.Gw[0]     = wr.Gw;
        wp.Sw[0]     = wr.Sw;
        wp.Uw[0]     = wr.Uw;
        wp.Hw[0]     = wr.Hw;
        wp.Dielw[0]  = wr.Dielw;
        wp.ZBorn[0]  = wr.ZBorn;
        wp.Surtenw[0]= wr.Surtenw;
        wp.Cvw[0]    = wr.Cvw;
        wp.Cpw[0]    = wr.Cpw;
        wp.Alphaw[0] = wr.Alphaw;
        wp.Betaw[0]  = wr.Betaw;
        wp.YBorn[0]  = wr.YBorn;
        wp.QBorn[0]  = wr.QBorn;
        wp.XBorn[0]  = wr.XBorn;
        wp.Tcondw[0] = wr.Tcondw;
        wp.Tdiffw[0] = wr.Tdiffw;
        wp.Prndtlw[0]= wr.Prndtlw;
        wp.dAldT[0]  = wr.dAldT;
        wp.Albew[0]  = wr.Albew;
        wp.Speedw[0] = wr.Speedw;
        wp.Viscw[0]  = wr.Viscw;
        wp.Visckw[0] = wr.Visckw;
        wp.Gigw[0]   = id.gi;
        wp.Sigw[0]   = id.si;
        wp.Cpigw[0]  = id.cpi;
    }
    else
    {   // liquid at Psat
        wp.Aw[1]     = wl.Aw;
        wp.Gw[1]     = wl.Gw;
        wp.Sw[1]     = wl.Sw;
        wp.Uw[1]     = wl.Uw;
        wp.Hw[1]     = wl.Hw;
        wp.Dielw[1]  = wl.Dielw;
        wp.ZBorn[1]  = wl.ZBorn;
        wp.Surtenw[1]= wl.Surtenw;
        wp.Cvw[1]    = wl.Cvw;
        wp.Cpw[1]    = wl.Cpw;
        wp.Alphaw[1] = wl.Alphaw;
        wp.Betaw[1]  = wl.Betaw;
        wp.YBorn[1]  = wl.YBorn;
        wp.QBorn[1]  = wl.QBorn;
        wp.XBorn[1]  = wl.XBorn;
        wp.Tcondw[1] = wl.Tcondw;
        wp.Tdiffw[1] = wl.Tdiffw;
        wp.Prndtlw[1]= wl.Prndtlw;
        wp.dAldT[1]  = wl.dAldT;
        wp.Albew[1]  = wl.Albew;
        wp.Speedw[1] = wl.Speedw;
        wp.Viscw[1]  = wl.Viscw;
        wp.Visckw[1] = wl.Visckw;
        wp.Gigw[1]   = id.gi;
        wp.Sigw[1]   = id.si;
        wp.Cpigw[1]  = id.cpi;
    }
}

//--------------------------------------------------------------------//
//   TdPsdT - return  T * (dPsat / dT).
double TSupcrt::TdPsdT(double t_)
{
//    int i=-1;
    double v, w, y, z, q, b = 0.0e0, c = 0.0e0, TdPsdT;

    double a[8]={-.78889166e1,  .25514255e1, -.6716169e1,  .33239495e2,
                 -.10538479e3,  .17435319e3, -.14839348e3, .48631602e2};
    v = t_ / 647.25e0;
    w = 1.0 - v;
    for( int i=0; i<=7; i++  ) // while (i++ <= 7)
    {
        z  = i;
        y  = a[i] * pow(w,(z + 1.0e0)/2.0e0);
        c += y / w * (0.5e0 - 0.5e0 * z - 1.0e0 / v);
        b += y;
    }
    q = b / v;
    TdPsdT = 22.093e0 * exp(q) * c;
    return(TdPsdT);
}

//--------------------------------------------------------------------//
//   TsHGK - return Tsaturation(P).
double TSupcrt::TsHGK(double Ps_)
{
    int k=0;
    double pl, tg, pp, dp, TsHGK;

    TsHGK = 0.0e0;
    if (Ps_ > 22.05e0)
        return(TsHGK);
    pl = 2.302585e0 + log(Ps_);
    tg = 372.83e0 + pl * (27.7589e0 + pl * (2.3819e0 +
                                            pl * (0.24834e0 + pl * 0.0193855e0)));
    /*  1   */
    while ( k++ <=  8)
    {
        if (tg < 273.15e0)
            tg = 273.15e0;
        if (tg > 647.00e0)
            tg = 647.00e0;
        pp = PsHGK(tg);
        dp = TdPsdT(tg);
        if( !(fabs(1.0e0 - pp/Ps_) < 1.0e-5 ) )
            tg *= (1.0e0 + (Ps_ - pp) / dp);
    }
    TsHGK = tg;
    return(TsHGK);
}


//--------------------------------------------------------------------//
/* tcorr - Calc T(P) saturation (t) and P (dL & dV) from  refinement
   of an initial approximation (TsHGK(p)) in accord with  Gl = Gv.  */
void TSupcrt::tcorr(int itripl, double *t, double *p, double *dL, double *dV,
                    int epseqn)
{
    double delg, dp;

    *t = TsHGK(*p);
    if ( approximatelyZero(*t) )
        return;
    *dL = 0.0e0;
    *dV = 0.0e0;
    do
    {
        a1.rt = *t * ac->gascon;
        corr(itripl,*t,p,dL,dV,&delg,epseqn);
        dp = delg * ac->gascon * *t / (1.0e0 / *dV - 1.0e0 / *dL);
        *t *= (1.0e0 - dp / TdPsdT(*t));
    }
    while ( !(fabs(delg) > 1.0e-4) );
}

//--------------------- End of s_subcrt.cpp ---------------------------

