//-------------------------------------------------------------------
// $Id$
//
// Copyright (C) 1992-2000  K.Chudnenko, D.Kulik, S.Dmitrieva
//
// Declaration of TFGLcalc class
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
//

#ifndef _s_fgl_h_
#define _s_fgl_h_

#include "m_dcomp.h"


/* copyright (c) Ircutsk 1995  FGL */
typedef struct
{ /* Working parametres */
    short Z;
    float MM;
    float FUG;
    float VLK;
    float DH;
    float DS;
    float P1;
    float P2;
    float TK;
    float TR;
    float PR;
    float GTP;
    float dU;
    float KT;
    float KP;
    float KZ;
    float KV;
    float WW;
    int IH;
    float HV;    // work variable
    float VRK;
    float VR0;
    float VRR;
    float Z0;
    float ZR;
    float Z1;
    float ZLK;
}
FGWORK;

typedef struct
{ /* Constant to fugitivnost */
    float COF[20][4];
    float PPP[5];
    float Tmin[9],Tmax[9],Pmin[9],Pmax[9];
}
FGCONST;


typedef struct
{ /* water fitches */
    float T;
    float P;
    float R;
    float ALP;
    float BE;
    float DAT;
    float DBT;
    float DAP;
    float DBP;
    float CHI;
    float FUG;
    float G1;
    float H1;
    float CP1;
    float S01;
    float GPG0;
    float FT;
    float GT;
    float UT;
    float HT;
    float ST;
    float CVT;
    float CPT;
}
H2O;

class TFGLcalc  // description thermodinamic properties of gases on block FGL
{
    // working parametera
    FGWORK fg;
    FGCONST fc;
    H2O tw;

protected:
    void DOK(float *PH,float *PB,int PGL);
    void ZAK(int PGL);
    float SAT( float T );
    float KNIGHT( float T );
    void fugaz();
    void fgl();
    int OPTION(float P);
    float PY();
    float PKDM(float P,float EPS);
    float NG();
    void  FESAX(int TT,float P1,float P2, float *GTP,float *DH );
    void  BERTOLE(float P1,float P2,float *GTP,float *DH);
    void  LEEK(  float P1, float P2,float *GTP,float *DH);
    void water_g( float P1, float P2,float *GTP,float *DH);
    void TERH2O(float RH);
    float HAAR(float T,float P);
    float ENTLG();
    void EXTRA( float P1, float P2,float *GTP,float *DH);
    void LIKES();
    float BISEC(float A,float B,float EPS,float EPS1, short K );
    float LB1();
    float LB2();
    float FD();
    float RK();
    float LK1();
    float LK2();
    float ROOT(short K,float A,float EPS,short N);
    void TABLE1();
    void BREED(float P1, float P2,float *GTP,float *DH );
    float densi1(float TR,float PR);
    float lfug(float TR,float PR);
    float ENTALPIA(float TR,float PR);
    float lagran(float *a, float *x, float *y, float x1, float x2,
                 int p,int p0,int p1);

public:

    TFGLcalc()
    {}
    //--- Value manipulation

    void calc_FGL( DCOMP *dcp, int p );

};

#endif  // _v_fgl_h
