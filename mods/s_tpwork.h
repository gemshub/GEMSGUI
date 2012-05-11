//-------------------------------------------------------------------
// $Id$
//
// Copyright (C) 1992-2000  D.Kulik, S.Dmitrieva
//
// Declaration of TpworkList class - working t/d parametres
//
// This file is part of a GEM-Selektor (GEMS) v.2.x.x program
// environment for thermodynamic modeling in geochemistry
// Uses: GEM-Vizor GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch;
//-------------------------------------------------------------------
//
#include "v_object.h"

#ifndef _s_tpwork_h_
#define _s_tpwork_h_

#include "v_mod.h"
#include "array.h"

struct TPWORK
{      // working t/d parametres
    char DRkey[DC_RKLEN],	// DCOMP or REACDC rkey with source data
    pSD,                	// data origin { d n i f r }
    pTM,	//Terminal state for  n i r : { + - * }
    unE,	// Units of energy  { j c J C n N }
    unV;	// Units of volume  { c C j L m }
   double P,Pst,	//current pressure,bar (10^5 Pa), reference pressure
    TC, TCst,		// TC - current temperature, TCr - reference temperature
    T, Tst,			// T -  current temperature,Tr - reference temperature
    dS, S, devS,   	// dSr at TP, S0 for reacdef DC, devS - uncertainty
    dG, G, devG,   	// dGr at TP, gT0  partial molar(molal) Gibbs energy at TP
    // uncertainty of gT0
    K, lgK, dlgK,  	//equilibrium constant for TP, logK of reac, uncertainty
    dH, H, devH,   	// dHr at TP, hT0  partial molal enthalpy, uncertainty
    dCp, Cp,devCp, 	//dCpr at TP, Cp0  partial molar heat capacity,uncertainty
    dV, V, devV,   	// dVr at TP,Vm0  partial molar volume at TP,uncertainty
    Alp, Bet,      	// Compressibility (reserved), expandability (reserved)
    Fug, dVg,      	// molar fugacity at TP (reserved), dVm of gas (reserved)
    RT,            	// RT factor
    wRo, wEps,		// water density and dielectrical constant
    wdRdT, wdEdT,		// first T derivative
    wd2RdT2, wd2EdT2,	// second T derivative
    wdRdP, wdEdP,		// first P derivative
    wd2RdP2, wd2EdP2,	// second P derivative
    wAlp, wBet, 	// water expansibility and compressibility
    wdAlpdT,		// first T derivative of expansibility
    gfun,             // g function from HKF (added 07.06.05 by KD )
    wtW[12];       //Work cells for RTparam script (increased to 12 for PRSV model, 31.05.2008 TW)
// work objects copied from DComp structure (invisible in GUI)
    float *CPg,   // Input critical parameters (for FGL)
    *Cemp,        // Array of empirical EOS coefficients (CG EOS)
    mwt,          // molar mass of DC
    TClow;        // Lowest TC for CG or PRSV EoS
    char PdcC,    // DC code
    cre1, cre2, cre3;
    TPWORK()
    {}

};

// tpwork  container: aW
class TpworkList:
            public TIArray<TPWORK>
{
protected:

public:
    TpworkList();
    ~TpworkList();

    struct TPWORK* twp;
    struct TPWORK& WW( int q)
    {
        return elem(q);
    }
    void set_zero( int q );
    void ods_link( int q );
    bool firstAW() const
    {
        return twp == &elem(0);
    }
};

extern TpworkList aW;


struct WATERPARAM
{
	// WATER termodynamic properties - extracted from HGK subroutines (SUPCRT92)
	// old dimensions [2] restored, added ideal gas water properties, 06.02.2008 (TW)
    bool init;
    double Aw[2];        // Helmholtz energy
    double Gw[2];        // Gibbs energy
    double Sw[2];        // entropy
    double Uw[2];        // internal energy
    double Hw[2];        // enthalpy
    double Cvw[2];       // isochoric heat capacity
    double Cpw[2];       // isobaric heat capacity
    double Speedw[2];    // speed of sound
    double Alphaw[2];    // constant pressure expansion (alpha)
    double Betaw[2];     // constant temperature compressibility (beta)
    double Dielw[2];     // dielectrical constant
    double Viscw[2];     // dynamic viscosity
    double Tcondw[2];    // thermal conductivity
    double Surtenw[2];   // surface tension
    double Tdiffw[2];    // not clear (currently not used)
    double Prndtlw[2];   // Prandtl number (currently not used)
    double Visckw[2];    // kinetic viscosity (currently not used)
    double Albew[2];     // alpha/beta ratio
    double ZBorn[2];     // Born function Z
    double YBorn[2];     // Born function Y
    double QBorn[2];     // Born function Q
    double dAldT[2];     // T derivative of isobaric expansion
    double XBorn[2];     // Born function X
    double Gigw[2];      // ideal gas Gibbs energy
    double Sigw[2];      // ideal gas entropy
    double Cpigw[2];     // ideal gas isobaric heat capacity
    WATERPARAM()
    {
        init = false;
    }
};

extern struct WATERPARAM aWp;


struct STATES
{
    double Temp;    // C
    double Pres;    // bar
    double Dens[2]; // g/cm3
    STATES()
    {
        Temp = -274.;
        Pres = -1;
        Dens[0] =Dens[1] = 0.;
    }
    void setdef()
    {
        Temp = -274.;
        Pres = -1;
        Dens[0] =Dens[1] = 0.;
    }
};

extern struct STATES aSta;

struct SPECS
{// HGK CONTROLS AND SPECIFICAIONS
    int it;
    int id;
    int ip;
    int ih;
    int itripl;
    int isat;
    int iopt;
    int useLVS;
    int epseqn;
    int icrit;
    SPECS(): it(1)
    {
        memset( &id, '\0', sizeof(int) * 9);
    }
};

extern struct SPECS aSpc;

#endif // _s_tpwork_h
