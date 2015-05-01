//-------------------------------------------------------------------
// $Id: s_supcrt.h 1024 2008-02-07 08:00:49Z wagner $
//
// Copyright (C) 1992-2000  D.Kulik, S.Dmitrieva
//
// Implementation of TSupcrt class
//
// This file is part of a GEM-Selektor (GEMS) v.3.1.x program
// environment for thermodynamic modeling in geochemistry
// Uses: GEM-Selektor GUI GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the terms of GEMS4 Development
// Quality Assurance Licence (GEMS4.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch;
//-------------------------------------------------------------------
//
#ifndef _s_supcrt_h_
#define _s_supcrt_h_

#include "s_tpwork.h"

double TdegK(int it, double t);

typedef struct
{ //work structure  t/d properties of water-solution
    // ( see at WATERPARAM)
    double Aw, Gw, Sw, Uw, Hw, Cvw, Cpw, Speedw, Alphaw,
    Betaw, Dielw, Viscw, Tcondw, Surtenw, Tdiffw,
    Prndtlw, Visckw, Albew;
    double ZBorn, YBorn, QBorn, dAldT, XBorn;
}
WPROPS;

typedef struct
{ /*  abc2    */
    double r, th;
}
ABC2;

typedef struct
{ /*  abc1,  abc3,   RTcurr  */
    double dPdM, dPdTcd, rt;
}
ABC1;

typedef struct
{// local values
    double T;
    double P;
    double Ps;
    double D;
    double Dv;
    double Dl;
    double delg;  /* (Gl-Gv)/RT                     */
}
TERM_PR;

typedef struct
{ // set local parametres to fact scales
    double ft;
    double fd;
    double fvd;
    double fvk;
    double fs;
    double fp;
    double fh;
    double fst;
    double fc;
}
UNITS;

typedef struct
{ /*  satur   */
    int iphase;
    double Dliq, Dvap, DH2O;
}
SATUR;

typedef struct
{ /* qqqq */
    double q0, q5;
}
QQQQ;

typedef struct
{ /*  fcts  */
    double ad, gd, sd, ud, hd, cvd, cpd,
    dpdt, dvdt, dpdd, cjtt, cjth;
}
FCTS;

typedef struct
{ /*  basef */
    double ab, gb, sb, ub, hb, cvb, pb, dpdtb;
}
BASEF;

typedef struct
{ /*  resf */
    double ar, gr, sr, ur, hr, cvr, dpdtr;
}
RESF;

typedef struct
{ /*  idf  */
    double ai, gi, si, ui, hi, cvi, cpi;
}
IDF;

typedef struct
{ /*  therm  */
    double AE, GE, U, H, Entrop, Cp, Cv,
    betaw, alphw, heat, Speed;
}
THERM;

typedef struct
{ /* param  */
    double r1, th1;
}
PARAM;

typedef struct
{ /*  deri2  */
    double dPdD, dPdT;
}
DERI2;

typedef struct
{ /*  deriv  */
    double amu, s[2], sd[2], Pw, Tw, dTw,
    dM0dT, dP0dT, d2PdM2, d2PdMT,
    d2PdT2, p0th, p1th, xk[2];
}
DERIV;

typedef struct
{ /* io  */
    int iconf;
    double rterm, wterm, reacf, pronf, tabf, plotf;
}
IO_Y;

typedef struct
{ /*  aconst   */
    double wm, gascon, tz, aa, uref, sref, zb, dzb, yb;
}
ACONST;

typedef struct
{ /*  nconst  */
    double g[40];
    int    ii[40], jj[40], nc;
}
NCONST;

typedef struct
{ /*  ellcon   */
    double g1, g2, gf, b1, b2, b1t, b2t, b1tt, b2tt;
}
ELLCON;

typedef struct
{ /*  bconst   */
    double bp[10], bq[10];
}
BCONST;

typedef struct
{ /*  addcon   */
    double atz[4], adz[4], aat[4], aad[4];
}
ADDCON;

typedef struct
{ /*  HGKcrt   */
    double tcHGK, dcHGK, pcHGK;
}
HGK_CRT;

typedef struct
{ /*  tolers   */
    double TTOL, PTOL, DTOL, XTOL, EXPTOL, FPTOL;
}
TOLERS;

typedef struct
{ /*  HGKbnd   */
    double Ttop, Tbtm, Ptop, Pbtm, Dtop, Dbtm;
}
HGK_BND;

typedef struct
{ /*  liqice   */
    double sDli1, sPli1, sDli37, sPli37,
    sDIB30, Tli13, Pli13, Dli13, TnIB30, DnIB30;
}
LIQICE;

typedef struct
{ /*  tpoint   */
    double Utri, Stri, Htri, Atri, Gtri,
    Ttr, Ptripl, Dltrip, Dvtrip;
}
TPOINT;

typedef struct
{ /*  crits    */
    double Tc, rhoC, Pc, Pcon, Ucon, Scon, dPcon;
}
CRITS;

typedef struct
{ /*  coefs   */
    double a[20], q[20], x[11];
}
COEFS;

struct STORE
{ /*  store   */
    int isav1;
    double sav2, sav3, sav4, sav5, sav6, sav7,
    sav8, sav9, sav10, sav11, sav12, sav13,
    sav14, sav15, sav16, sav17, sav18, sav19;
    STORE():isav1(0)
    {
        memset( &sav2, 0, sizeof(double)*18);
    }
};

class TSupcrt  // description of thermodinamic properties of water
{
    // working parametera
    // WLIQUID wl;
    WPROPS wl;
    WPROPS wr;
    ABC2 a2;
    ABC1 a1;
    TERM_PR trp;
    UNITS un;
    SATUR sa;
    QQQQ qq;
    FCTS fct;
    BASEF ba;
    RESF res;
    IDF id;
    THERM th;
    PARAM par;
    DERI2 d2;
    DERIV dv;
    IO_Y io;
    //
    ACONST   *ac;
    NCONST   *nc;
    ELLCON   *el;
    BCONST   *bcn;
    ADDCON   *ad;
    HGK_CRT  *hc;
    TOLERS   *to;
    HGK_BND  *hb;
    LIQICE   *li;
    TPOINT   *tt;
    CRITS    *cr;
    COEFS    *co;

protected:
    void load(int phase, WATERPARAM& wp);
    void unit(int it, int id, int ip, int ih, int itripl);
    void tpset();
    void valid(int it, int id, int ip, int ih, int itripl, int isat,
               int iopt, int useLVS, int epseqn, double Temp, double *Pres,
               double *Dens0, int *eR);
    int valspc(int it, int id, int ip, int ih, int itripl,
               int isat, int iopt, int useLVS, int epseqn);
    //  double TdegK(int it, double t);
    int  valTD(double T, double D, int isat, int epseqn);
    void pcorr(int itripl, double t, double *p, double *dL,double *dV, int epseqn);
    double PsHGK(double t);
    void  corr(int itripl, double t, double *p, double *dL,
               double *dV, double *delg, int epseqn);
    void bb(double t);
    void resid(double t, double *d);
    void base(double *d, double t);
    void denHGK(double *d, double *p, double dguess, double t, double *dpdd);
    void  ideal(double t);
    void thmHGK(double *d, double t);
    void dimHGK(int isat,int itripl, double t, double *p, double *d, int epseqn);
    double dalHGK(double *d, double t, double alpha);
    double viscos(double Tk, double Pbars, double Dkgm3, double betaPa);
    double thcond(double Tk, double Pbars, double Dkgm3, double alph,
                  double betaPa);
    double surten(double Tsatur);
    void Born92(double TK, double Pbars, double Dgcm3, double betab,
                double *alphaK, double *daldT, double *eps, double *Z,
                double *Q, double *Y, double *X, int epseqn);
    void JN91(double T, double D, double beta, double *alpha,
              double *daldT, double *eps, double *dedP, double *dedT, double *d2edT2);
    void epsBrn(double *eps, double dedP, double dedT,double d2edT2,
                double *Z, double *Q, double *Y, double *X);
    void triple(double T, WPROPS  *wr);
    double Pfind(int isat, double T, double DD);
    void conver(double *rho, double Tee, double rho1s,
                double *rhodi,double error1);
    void ss(double r, double th, double *s, double *sd);
    void rtheta(double *r, double *theta, double rho, double Tee);
    void  aux(double r1, double th1, double *d2PdT2, double *d2PdMT,
              double *d2PdM2, double aa, double *xk, double *sd, double Cvcoex);
    void denLVS(int isat, double T, double P);
    double Psublm(double Temp);
    int  valTP(double T, double P);
    void crtreg(int isat, int iopt, int it, double *T, double *P,
                double *D, int *eR);
    void LVSeqn(int isat, int iopt, int itripl, double TC,
                double *P, double *Dens0, int epseqn);
    void LVSsat(int iopt, int isat, double *T, double *P, double *D);
    double TsLVS(int isat, double Pres);
    void restor( struct STORE sto );
    void backup( struct STORE &sto );
    void thmLVS(int isat, double T, double r1, double th1);
    void dimLVS(int isat, int itripl, double theta, double T, double *Pbars,
                double *dL, double *dV, WPROPS *www, int epseqn);
    double dalLVS(double D, double T, double P, double alpha);
    void cpswap();
    void HGKeqn(int isat, int iopt, int itripl, double Temp,
                double *Pres, double *Dens0, int epseqn);
    void HGKsat(int& isat, int iopt, int itripl, double Temp,
                double *Pres, double *Dens, int epseqn);
    void tcorr(int itripl, double *t, double *p, double *dL,
               double *dV,int epseqn);
    double TsHGK(double Ps_);
    double TdPsdT(double t_);
    void calcv3(int iopt, int itripl, double Temp, double *Pres,
                double *Dens, int epseqn);
    double TdegUS(int it, double t);
public:

    TSupcrt();
    //--- Value manipulation
    void Supcrt_H2O( double TC, double *P );

};

#endif   // _s_subcrt_h
