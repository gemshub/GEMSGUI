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
// See http://les.web.psi.ch/Software/GEMS-PSI for more information
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
    char DRkey[DC_RKLEN],    // DCOMP or REACDC rkey with source data
    pSD,                // data origin { d n i f r }
    pTM,   //Terminal state for  n i r : { + - * }
    unE,                // Units of energy  { j c J C n N }
    unV;                // Units of volume  { c C j L m }
   double P,Pst,     //current pressure,bar (10^5 Pa), reference pressure
    TC, TCst,      // TC - current temperature, TCr - reference temperature
    T, Tst,        // T -  current temperature,Tr - reference temperature
    dS, S, devS,   // dSr at TP, S0 for reacdef DC, devS - uncertainty
    dG, G, devG,   // dGr at TP, gT0  partial molar(molal) Gibbs energy at TP
    // uncertainty of gT0
    K, lgK, dlgK,  //equilibrium constant for TP, logK of reac, uncertainty
    dH, H, devH,   // dHr at TP, hT0  partial molal enthalpy, uncertainty
    dCp, Cp,devCp, //dCpr at TP, Cp0  partial molar heat capacity,uncertainty
    dV, V, devV,   // dVr at TP,Vm0  partial molar volume at TP,uncertainty
    Alp, Bet,      // Compressibility (reserved), expandability (reserved)
    Fug, dVg,      // molar fugacity at TP (reserved), dVm of gas (reserved)
    RT,            // RT factor
    wRo, wEps,     //Density of H2O fluid, Dielectric constant of H2O fluid
    wtW[10];       //Work cells for RTparam script
// work objects copied from DComp structure (invisible in GUI)
    float *CPg,   // Input critical parameters (for FGL)
    *Cemp,        // Array of empirical EOS coefficients (CG EOS)
    mwt,          // molar mass of DC
    TClow;        // Lowest TC for CG EoS 
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
{ // WATER termodinamic
    bool init;
    double Aw[2];
    double Gw[2];
    double Sw[2];
    double Uw[2];
    double Hw[2];
    double Cvw[2];
    double Cpw[2];
    double Speedw[2];
    double Alphaw[2];
    double Betaw[2];
    double Dielw[2];
    double Viscw[2];
    double Tcondw[2];
    double Surtenw[2];
    double Tdiffw[2];
    double Prndtlw[2];
    double Visckw[2];
    double Albew[2];  /* Alphaw / Betaw */
    double ZBorn[2];
    double YBorn[2];
    double QBorn[2];
    double dAldT[2];
    double XBorn[2];
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
{// ≈ƒ»Õ»÷€ »«Ã≈–≈Õ»ﬂ
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
