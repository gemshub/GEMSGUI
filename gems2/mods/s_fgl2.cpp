//-------------------------------------------------------------------
// $Id$
//
// Copyright (c) 2003-2007   S.Churakov, Th.Wagner,
//    D.Kulik, S.Dmitrieva
//
// Implementation of parts of TPRSVcalc and TCFGcalc classes
// called from m_dcomp.cpp
//
// This file is part of a GEM-Selektor (GEMS) v.2.x.x program
// environment for thermodynamic modeling in geochemistry
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
//
int TPRSVcalc::CalcFugPure( void )
{
    double T, P, Fugcoeff = 0.1, Volume = 0.0, DeltaH=0, DeltaS=0;
    float *Coeff;
    double Eos2parPT[5] = { 0.0, 0.0, 0.0, 0.0, 0.0 } ;
    int retCode = 0;

    ErrorIf( !aW.twp, "PRSV EoS", "Undefined twp");

    P = aW.twp->P;    /* P in 10^5 Pa? */
    T = aW.twp->TC+273.15;   /* T?in K */

    Coeff = aW.twp->CPg;     /* pointer to coeffs of CG EOS */

// Calling PRSV EoS functions here

    if( T >= aW.twp->TClow +273.15 && T < 1e4 && P >= 1e-5 && P < 1e5 )
       retCode = PRFugacityPT( P, T, Coeff, Eos2parPT, Fugcoeff, Volume,
            DeltaH, DeltaS );
    else {
            Fugcoeff = 1.;
            Volume = 8.31451*T/P;
            aW.twp->V = Volume;
            aW.twp->Fug = Fugcoeff*P;
            return retCode;
          }

    aW.twp->G += 8.31451 * T * log( Fugcoeff );   // from fugacity coeff
    /* add enthalpy and enthropy increments */
    aW.twp->H +=  DeltaH;   // in J/mol - to be completed
    aW.twp->S +=  DeltaS;   // to be completed
    aW.twp->V = Volume; /* /10.  in J/bar */
    aW.twp->Fug = Fugcoeff * P;   /* fugacity at P */

//  passing corrected EoS coeffs to calculation of fluid mixtures
    aW.twp->wtW[6] = Eos2parPT[0];      // a
    aW.twp->wtW[7] = Eos2parPT[1];      // b
// three more to add !!!
    return retCode;
}


//--------------------------------------------------------------------//
//
int TCGFcalc::CGcalcFug( void )
{
    double T, P, Fugacity = 0.1, Volume = 0.0, DeltaH=0, DeltaS=0;
    float *Coeff, Eos4parPT[4] = { 0.0, 0.0, 0.0, 0.0 } ;
    int retCode = 0;

    ErrorIf( !aW.twp, "CG EoS", "Undefined twp");

    P = aW.twp->P;    /* P in 10^5 Pa? */
    T = aW.twp->TC+273.15;   /* T?in K */

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
            aW.twp->wtW[6] = Coeff[0];
            if( aW.twp->wtW[6] < 1. || aW.twp->wtW[6] > 10. )
                aW.twp->wtW[6] = 1.;                 // foolproof temporary
            aW.twp->wtW[7] = Coeff[1];
            aW.twp->wtW[8] = Coeff[2];
            aW.twp->wtW[9] = Coeff[3];
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
if( aW.twp->wtW[6] < 1. || aW.twp->wtW[6] > 10. )
  aW.twp->wtW[6] = 1.;                            // foolproof temporary
    aW.twp->wtW[7] = Eos4parPT[1];
    aW.twp->wtW[8] = Eos4parPT[2];
    aW.twp->wtW[9] = Eos4parPT[3];
//
    return retCode;
}


// -----------------------------------------------------------------------------
// Implementation of the TSolMod class
// Started by Th.Wagner and D.Kulik on 07.03.2007



// Generic constructor for the TSolMod class
//
TSolMod::TSolMod( int NSpecies, int NParams, int NPcoefs, int MaxOrder,
       int NPperDC, double T_k, double P_bar, char Mod_Code,
       short* arIPx, float* arIPc, float* arDCc,
       double *arWx, double *arlnGam )
{
    R_CONST = 8.31451;
    NComp = NSpecies;
    NPar = NParams;
    NPcoef = NPcoefs;
    MaxOrd = MaxOrder;
    NP_DC = NPperDC;
    Tk = T_k;
    Pbar = P_bar;
    ModCode = Mod_Code;

    // pointer assignment
    aIPx = arIPx;   // Direct access to index list and parameter coeff arrays!
    aIPc = arIPc;
    aDCc = arDCc;
    x = arWx;
    lnGamma = arlnGam;
}

TSolMod::~TSolMod()
{



// In principle, the stuff below is not necessary if the memory is not
// allocated within the class
	aIPx = NULL;
	aIPc = NULL;
	aDCc = NULL;
	x = NULL;
	lnGamma = NULL;
}



// Van Laar model for solid solutions (c) TW March 2007
// Calculates T,P corrected binary interaction parameters
// Returns 0 if Ok or 1 if error
int
TSolMod::VanLaarPT()
{
// calculates P-T dependence of binary interaction parameters
	int ip;
	double Wij[4];

        if ( /* ModCode != SM_VANLAAR || */ NPcoef < 4 || NPar < 1 )
           return 1;  // foolproof!

	for (ip=0; ip<NPar; ip++)
	{
        Wij[0] = (double)aIPc[NPcoef*ip];
        Wij[1] = (double)aIPc[NPcoef*ip+1];
        Wij[2] = (double)aIPc[NPcoef*ip+2];
	    Wij[3] = Wij[0]+ Wij[1]*Tk + Wij[2]*Pbar;
	    aIPc[NPcoef*ip+3] = (float)Wij[3];
	}
	return 0;
}



// Van Laar model for solid solutions (c) TW March 2007
// References:  Holland & Powell (2003)
// Calculates activity coefficients and excess functions
// Returns 0 if Ok or not 0 if error
//
int
TSolMod::VanLaarMixMod( double &Gex_, double &Vex_, double &Hex_, double &Sex_,
         double &CPex_ )
{
   int ip, j;
   int index1, index2;
   double dj, dk;
   double sumPhi; // Sum of Phi terms
   double *Wh;
   double *Ws;
   double *Wv;
   double *Wpt;   // Interaction coeffs at P-T
   double *Phi;   // Mixing terms
   double *PsVol; // End member volume parameters

   if ( /* ModCode != SM_VANLAAR || */ NPcoef < 4 || NPar < 1 || NComp < 2
         || MaxOrd < 2 || !x || !lnGamma )
           return 1;  // foolproof!

   Wh = new double [NPar];
   Ws = new double [NPar];
   Wv = new double [NPar];
   Wpt = new double [NPar];
   Phi = new double [NComp];
   PsVol = new double [NComp];

   if( !Wpt || !Phi || !PsVol )
        return -1;  // memory alloc failure

	// read P-T corrected interaction parameters
   for (ip=0; ip<NPar; ip++)
   {
        Wh[ip] = (double)aIPc[NPcoef*ip];
	Ws[ip] = (double)aIPc[NPcoef*ip+1];
	Wv[ip] = (double)aIPc[NPcoef*ip+2];
	Wpt[ip] = (double)aIPc[NPcoef*ip+3]; // were stored in VanLaarPT()
   }

   // calculating Phi values
   sumPhi = 0.;
   for (j=0; j<NComp; j++)
   {
       PsVol[j] = (double)aDCc[NP_DC*j];  // reading pseudo-volumes
       sumPhi +=  x[j]*PsVol[j];
   }
   if( fabs(sumPhi) < 1e-30 )
       return 2;    // to prevent zerodivide!

   for (j=0; j<NComp; j++)
       Phi[j] = x[j]*PsVol[j]/sumPhi;

   // calculate activity coefficients
   for (j=0; j<NComp; j++)      // index end members with j
   {
	lnGamRT = 0.;
	for (ip=0; ip<NPar; ip++)  // inter.parameters indexed with ip
	{
        index1 = (int)aIPx[MaxOrd*ip];
	    index2 = (int)aIPx[MaxOrd*ip+1];

   	    if( j == index1 )
		dj = 1.;
	    else
		dj = 0.;
	    if( j == index2 )
		dk = 1.;
	    else
		dk = 0.;
	    lnGamRT -= (dj-Phi[index1])*(dk-Phi[index2])*Wpt[ip]
                         *2.*PsVol[j]/(PsVol[index1]+PsVol[index2]);
	}
        lnGam = lnGamRT/(R_CONST*Tk);
//	Gam = exp(lnGam);
	lnGamma[j] = lnGam;
	}

   // calculate bulk phase excess properties
   Gex = 0.;
   Vex = 0.;
   Hex = 0.;
   Sex = 0.;
   CPex = 0.;

   for (ip=0; ip<NPar; ip++)
   {
      index1 = (int)aIPx[MaxOrd*ip];
      index2 = (int)aIPx[MaxOrd*ip+1];
      Gex += Phi[index1]*Phi[index2]*2.*sumPhi/(PsVol[index1]+PsVol[index2])*Wpt[ip];
      Vex += Phi[index1]*Phi[index2]*2.*sumPhi/(PsVol[index1]+PsVol[index2])*Wv[ip];
      Hex += Phi[index1]*Phi[index2]*2.*sumPhi/(PsVol[index1]+PsVol[index2])*Wh[ip];
      Sex -= Phi[index1]*Phi[index2]*2.*sumPhi/(PsVol[index1]+PsVol[index2])*Wv[ip];
   }

   Gex_ = Gex;
   Vex_ = Vex;
   Hex_ = Hex;
   Sex_ = Sex;
   CPex_ = CPex;

   delete[]Wh;
   delete[]Ws;
   delete[]Wv;
   delete[]Wpt;
   delete[]Phi;
   delete[]PsVol;
   return 0;
}



// Regular model for multicomponent solid solutions (c) TW March 2007
// Calculates T,P corrected binary interaction parameters
// Returns 0 if Ok or 1 if error
int
TSolMod::RegularPT()
{
// calculates P-T dependence of binary interaction parameters
	int ip;
	double Wij[4];

        if ( /* ModCode != SM_REGULAR || */ NPcoef < 4 || NPar < 1 )
           return 1;  // foolproof!

	for (ip=0; ip<NPar; ip++)
	{
        Wij[0] = (double)aIPc[NPcoef*ip];
        Wij[1] = (double)aIPc[NPcoef*ip+1];
        Wij[2] = (double)aIPc[NPcoef*ip+2];
	    Wij[3] = Wij[0]+ Wij[1]*Tk + Wij[2]*Pbar;
	    aIPc[NPcoef*ip+3] = (float)Wij[3];
	}
	return 0;
}



// Regular model for multicomponent solid solutions (c) TW March 2007
// References:  Holland & Powell (1993)
// Calculates activity coefficients and excess functions
// Returns 0 if Ok or not 0 if error
//
int
TSolMod::RegularMixMod( double &Gex_, double &Vex_, double &Hex_, double &Sex_,
         double &CPex_ )
{
   int ip, j;
   int index1, index2;
   double dj, dk;
   double *Wh;
   double *Ws;
   double *Wv;
   double *Wpt;   // Interaction coeffs at P-T

   if ( /* ModCode != SM_REGULAR || */ NPcoef < 4 || NPar < 1 || NComp < 2
         || MaxOrd < 2 || !x || !lnGamma )
           return 1;  // foolproof!

   Wh = new double [NPar];
   Ws = new double [NPar];
   Wv = new double [NPar];
   Wpt = new double [NPar];

   if( !Wpt || !Wh || !Ws || !Wv )
        return -1;  // memory alloc failure

	// read P-T corrected interaction parameters
   for (ip=0; ip<NPar; ip++)
   {
    Wh[ip] = (double)aIPc[NPcoef*ip];
	Ws[ip] = (double)aIPc[NPcoef*ip+1];
	Wv[ip] = (double)aIPc[NPcoef*ip+2];
	Wpt[ip] = (double)aIPc[NPcoef*ip+3]; // were stored in RegularPT()
   }

   // calculate activity coefficients
   for (j=0; j<NComp; j++)      // index end members with j
   {
	lnGamRT = 0.;
	for (ip=0; ip<NPar; ip++)  // inter.parameters indexed with ip
	{
        index1 = (int)aIPx[MaxOrd*ip];
	    index2 = (int)aIPx[MaxOrd*ip+1];

   	    if( j == index1 )
		dj = 1.;
	    else
		dj = 0.;
	    if( j == index2 )
		dk = 1.;
	    else
		dk = 0.;
	    lnGamRT -= (dj-x[index1])*(dk-x[index2])*Wpt[ip];
	}
        lnGam = lnGamRT/(R_CONST*Tk);
//	Gam = exp(lnGam);
	lnGamma[j] = lnGam;
	}

   // calculate bulk phase excess properties
   Gex = 0.;
   Vex = 0.;
   Hex = 0.;
   Sex = 0.;
   CPex = 0.;

   for (ip=0; ip<NPar; ip++)
   {
      index1 = (int)aIPx[MaxOrd*ip];
      index2 = (int)aIPx[MaxOrd*ip+1];
      Gex += x[index1]*x[index2]*Wpt[ip];
      Vex += x[index1]*x[index2]*Wv[ip];
      Hex += x[index1]*x[index2]*Wh[ip];
      Sex -= x[index1]*x[index2]*Wv[ip];
   }

   Gex_ = Gex;
   Vex_ = Vex;
   Hex_ = Hex;
   Sex_ = Sex;
   CPex_ = CPex;

   delete[]Wh;
   delete[]Ws;
   delete[]Wv;
   delete[]Wpt;
   return 0;
}



// add multi-component Redlich-Kister model here




//--------------------- End of s_fgl2.cpp ---------------------------
