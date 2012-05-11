//-------------------------------------------------------------------
// $Id$
//
// Copyright (C) 2007-2009  T.Wagner, D.Kulik, S.Dmitrieva
//
// Implementation of the TSolMod base class and subclasses
// for activity models for condensed (solid and liquid) phases
// subclasses: TVanLaar, TRegular, TRedlichKister, TNRTL, TWilson
//				TMargulesTernary, TMargulesBinary, TGuggenheim
// Started by Th.Wagner and D.Kulik on 07.03.2007
//
// This file is part of a GEM-Selektor (GEMS) v.2.x.x program
// environment for thermodynamic modeling in geochemistry
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch; chud@igc.irk.ru
//-------------------------------------------------------------------

#include <math.h>
#include "s_fgl.h"
#include "m_const.h"



//--------------------------------------------------------------------------------------------------------------
// Generic constructor for the TSolMod class
//
TSolMod::TSolMod( long int NSpecies, long int NParams, long int NPcoefs, long int MaxOrder,
		long int NPperDC, long int NPTPperDC, char Mod_Code, char Mix_Code,
        long int *arIPx, double *arIPc, double *arDCc, double *arWx,
        double *arlnGam, double *aphVOL, double T_k, double P_bar ):
				ModCode(Mod_Code), MixCode(Mix_Code), NComp(NSpecies),  NPar(NParams), NPcoef(NPcoefs),
				MaxOrd(MaxOrder),  NP_DC(NPperDC), NPTP_DC(NPTPperDC), R_CONST(8.31451),
				Tk(T_k), Pbar(P_bar)
{
    // pointer assignments
    aIPx = arIPx;   // Direct access to index list and parameter coeff arrays!
    aIPc = arIPc;
    aIP = new double[NPar];
    aDCc = arDCc;
    if( NPTP_DC )
    {	aDC = new double *[NComp];
       for (long int i=0; i<NComp; i++)
           aDC[i] = new double[NPTP_DC];
    }
    else aDC = 0;
    x = arWx;
    phVOL = aphVOL;
    lnGamma = arlnGam;

    // initialize phase properties
    Gex = 0.0; Hex = 0.0; Sex = 0.0; CPex = 0.0; Vex = 0.0; Aex = 0.0; Uex = 0.0;
    Gid = 0.0; Hid = 0.0; Sid = 0.0; CPid = 0.0; Vid = 0.0; Aid = 0.0; Uid = 0.0;
    Gdq = 0.0; Hdq = 0.0; Sdq = 0.0; CPdq = 0.0; Vdq = 0.0; Adq = 0.0; Udq = 0.0;
    Grs = 0.0; Hrs = 0.0; Srs = 0.0; CPrs = 0.0; Vrs = 0.0; Ars = 0.0; Urs = 0.0;
}


bool TSolMod::testSizes( long int NSpecies, long int NParams,	long int NPcoefs,
		long int MaxOrder, long int NPperDC, char Mod_Code, char Mix_Code )
{
  return(  (ModCode == Mod_Code) && (NComp == NSpecies) && ( NPar == NParams) &&
		    (NPcoef == NPcoefs) && (MaxOrd == MaxOrder) &&  ( NP_DC == NPperDC) &&
		    (MixCode == Mix_Code) );
}


TSolMod::~TSolMod()
{
   delete[] aIP;
   if( aDC )
   {
	for ( long i=0; i<NComp; i++)
      delete[] aDC[i];
    delete[] aDC;
   }
}


long int TSolMod::UpdatePT ( double T_k, double P_bar )
{
	  Tk = T_k;
	  Pbar = P_bar;
	  return 0;
}


void TSolMod::GetPhaseName( const char *PhName )
{
	 strncpy( PhaseName, PhName, MAXPHASENAME );
	 PhaseName[MAXPHASENAME] = 0;
}





//=============================================================================================
// Van Laar model for solid solutions (c) TW March 2007
// References:  Holland & Powell (2003)
//=============================================================================================


// Generic constructor for the TVanLaar class
TVanLaar::TVanLaar( long int NSpecies, long int NParams, long int NPcoefs, long int MaxOrder,
        long int NPperDC, char Mod_Code, char Mix_Code, long int *arIPx,
        double *arIPc, double *arDCc, double *arWx, double *arlnGam,
        double *aphVOL, double T_k, double P_bar ):
        	TSolMod( NSpecies, NParams, NPcoefs, MaxOrder, NPperDC, 0, Mod_Code, Mix_Code,
						arIPx, arIPc, arDCc, arWx, arlnGam, aphVOL, T_k, P_bar )
{
	alloc_internal();
}


TVanLaar::~TVanLaar()
{
	free_internal();
}


void TVanLaar::alloc_internal()
{
	Wu = new double [NPar];
	Ws = new double [NPar];
	Wv = new double [NPar];
	Wpt = new double [NPar];
	Phi = new double [NComp];
	PsVol = new double [NComp];
}


void TVanLaar::free_internal()
{
	if(Wu) delete[]Wu;
	if(Ws) delete[]Ws;
	if(Wv) delete[]Wv;
	if(Wpt) delete[]Wpt;
	if(Phi) delete[]Phi;
	if(PsVol) delete[]PsVol;
}


// Calculates T,P corrected binary interaction parameters
long int TVanLaar::PTparam()
{
	long int j, ip;

    if ( NPcoef < 3 || NPar < 1 )
       return 1;

    for (j=0; j<NComp; j++)
    {
    	PsVol[j] = aDCc[NP_DC*j];  // reading pseudo-volumes
    }

    for (ip=0; ip<NPar; ip++)
	{
    	Wu[ip] = aIPc[NPcoef*ip];
		Ws[ip] = aIPc[NPcoef*ip+1];
		Wv[ip] = aIPc[NPcoef*ip+2];
		Wpt[ip] = Wu[ip]+ Ws[ip]*Tk + Wv[ip]*Pbar;
	    aIP[ip] = Wpt[ip];
	}
    return 0;
}


// Calculates activity coefficients
long int TVanLaar::MixMod()
{
	long int ip, j, i1, i2;
	double dj, dk, sumPhi;

	if ( NPcoef < 3 || NPar < 1 || NComp < 2 || MaxOrd < 2 || !x || !lnGamma )
		return 1;

	// calculating Phi values
	sumPhi = 0.;
	for (j=0; j<NComp; j++)
	{
		sumPhi +=  x[j]*PsVol[j];
	}

	if( fabs(sumPhi) < 1e-30 )
		return 2;    // to prevent zerodivide

	for (j=0; j<NComp; j++)
		Phi[j] = x[j]*PsVol[j]/sumPhi;

	// calculate activity coefficients
	for (j=0; j<NComp; j++)
	{
		lnGamRT = 0.;
		for (ip=0; ip<NPar; ip++)  // inter.parameters indexed with ip
		{
			i1 = aIPx[MaxOrd*ip];
			i2 = aIPx[MaxOrd*ip+1];

			if( j == i1 )
				dj = 1.;
			else
				dj = 0.;
			if( j == i2 )
				dk = 1.;
			else
				dk = 0.;
			lnGamRT -= (dj-Phi[i1])*(dk-Phi[i2])*Wpt[ip]
			             *2.*PsVol[j]/(PsVol[i1]+PsVol[i2]);
		}
		lnGam = lnGamRT/(R_CONST*Tk);
		lnGamma[j] = lnGam;
	}
	return 0;
}


// calculates bulk phase excess properties
long int TVanLaar::ExcessProp( double *Zex )
{
	long int ip, j, i1, i2;
	double sumPhi, g, v, s, u;

	if ( NPcoef < 3 || NPar < 1 || NComp < 2 || MaxOrd < 2 || !x || !lnGamma )
		return 1;

	// calculating Phi values
	sumPhi = 0.;
	for (j=0; j<NComp; j++)
	{
		PsVol[j] = aDCc[NP_DC*j];  // reading pseudo-volumes
	    sumPhi +=  x[j]*PsVol[j];
	}

	if( fabs(sumPhi) < 1e-30 )
		return 2;    // to prevent zerodivide!

	for (j=0; j<NComp; j++)
	    Phi[j] = x[j]*PsVol[j]/sumPhi;

	// calculate bulk phase excess properties
	g = 0.0; s = 0.0; v = 0.0; u = 0.0;

	for (ip=0; ip<NPar; ip++)
	{
		i1 = aIPx[MaxOrd*ip];
	    i2 = aIPx[MaxOrd*ip+1];
	    g += Phi[i1]*Phi[i2]*2.*sumPhi/(PsVol[i1]+PsVol[i2])*Wpt[ip];
	    v += Phi[i1]*Phi[i2]*2.*sumPhi/(PsVol[i1]+PsVol[i2])*Wv[ip];
	    u += Phi[i1]*Phi[i2]*2.*sumPhi/(PsVol[i1]+PsVol[i2])*Wu[ip];
	    s -= Phi[i1]*Phi[i2]*2.*sumPhi/(PsVol[i1]+PsVol[i2])*Ws[ip];
	 }

	 Gex = g;
	 Sex = s;
	 CPex = 0.0;
	 Vex = v;
	 Uex = u;
	 Hex = Uex + Vex*Pbar;
	 Aex = Gex - Vex*Pbar;
	 Uex = Hex - Vex*Pbar;

	 // assignments (excess properties)
	 Zex[0] = Gex;
	 Zex[1] = Hex;
	 Zex[2] = Sex;
	 Zex[3] = CPex;
	 Zex[4] = Vex;
	 Zex[5] = Aex;
	 Zex[6] = Uex;

	 return 0;
}


// calculates ideal mixing properties
long int TVanLaar::IdealProp( double *Zid )
{
	long int j;
	double si;
	si = 0.0;
	for (j=0; j<NComp; j++)
	{
		if ( x[j] > 1.0e-32 )
			si += x[j]*log(x[j]);
	}
	Hid = 0.0;
	CPid = 0.0;
	Vid = 0.0;
	Sid = (-1.)*R_CONST*si;
	Gid = Hid - Sid*Tk;
	Aid = Gid - Vid*Pbar;
	Uid = Hid - Vid*Pbar;

	// assignments (ideal mixing properties)
	Zid[0] = Gid;
	Zid[1] = Hid;
	Zid[2] = Sid;
	Zid[3] = CPid;
	Zid[4] = Vid;
	Zid[5] = Aid;
	Zid[6] = Uid;

	return 0;
}





//=============================================================================================
// Regular model for multicomponent solid solutions (c) TW March 2007
// References:  Holland & Powell (1993)
//=============================================================================================


// Generic constructor for the TRegular class
TRegular::TRegular( long int NSpecies, long int NParams, long int NPcoefs, long int MaxOrder,
        long int NPperDC, char Mod_Code, char Mix_Code, long int *arIPx,
        double *arIPc, double *arDCc, double *arWx, double *arlnGam,
        double *aphVOL, double T_k, double P_bar ):
        	TSolMod( NSpecies, NParams, NPcoefs, MaxOrder, NPperDC, 0, Mod_Code, Mix_Code,
						arIPx, arIPc, arDCc, arWx, arlnGam, aphVOL, T_k, P_bar )
{
	alloc_internal();
}


TRegular::~TRegular()
{
	free_internal();
}


void TRegular::alloc_internal()
{
	Wu = new double [NPar];
	Ws = new double [NPar];
	Wv = new double [NPar];
	Wpt = new double [NPar];
}


void TRegular::free_internal()
{
	if(Wu) delete[]Wu;
	if(Ws) delete[]Ws;
	if(Wv) delete[]Wv;
	if(Wpt) delete[]Wpt;
}


// Calculates T,P corrected binary interaction parameters
long int TRegular::PTparam()
{
	long int ip;

	if ( NPcoef < 3 || NPar < 1 )
	           return 1;

	for (ip=0; ip<NPar; ip++)
	{
		Wu[ip] = aIPc[NPcoef*ip];
		Ws[ip] = aIPc[NPcoef*ip+1];
		Wv[ip] = aIPc[NPcoef*ip+2];
		Wpt[ip] = Wu[ip]+ Ws[ip]*Tk + Wv[ip]*Pbar;
	    aIP[ip] = Wpt[ip];
	}
	return 0;
}


// Calculates activity coefficients
long int TRegular::MixMod()
{
	long int ip, j, i1, i2;
	double dj, dk;

	if ( NPcoef < 3 || NPar < 1 || NComp < 2 || MaxOrd < 2 || !x || !lnGamma )
		return 1;

	// calculate activity coefficients
	for (j=0; j<NComp; j++)
	{
		lnGamRT = 0.;

		for (ip=0; ip<NPar; ip++)  // inter.parameters indexed with ip
		{
			i1 = aIPx[MaxOrd*ip];
			i2 = aIPx[MaxOrd*ip+1];

			if( j == i1 )
				dj = 1.;
			else
				dj = 0.;
			if( j == i2 )
				dk = 1.;
			else
				dk = 0.;
			lnGamRT -= (dj-x[i1])*(dk-x[i2])*Wpt[ip];
		}

		lnGam = lnGamRT/(R_CONST*Tk);
		lnGamma[j] = lnGam;
	}
	return 0;
}


// calculates bulk phase excess properties
long int TRegular::ExcessProp( double *Zex )
{
	long int ip, i1, i2;
	double g, v, s, u;

	if ( NPcoef < 3 || NPar < 1 || NComp < 2 || MaxOrd < 2 || !x || !lnGamma )
		return 1;

	// calculate bulk phase excess properties
	g = 0.0; s = 0.0; v = 0.0; u = 0.0;

	for (ip=0; ip<NPar; ip++)
	{
		i1 = aIPx[MaxOrd*ip];
		i2 = aIPx[MaxOrd*ip+1];
		g += x[i1]*x[i2]*Wpt[ip];
		v += x[i1]*x[i2]*Wv[ip];
		u += x[i1]*x[i2]*Wu[ip];
		s -= x[i1]*x[i2]*Ws[ip];
	}

	Gex = g;
	Sex = s;
	CPex = 0.0;
	Vex = v;
	Uex = u;
	Hex = Uex + Vex*Pbar;
	Aex = Gex - Vex*Pbar;
	Uex = Hex - Vex*Pbar;

	// assignments (excess properties)
	Zex[0] = Gex;
	Zex[1] = Hex;
	Zex[2] = Sex;
	Zex[3] = CPex;
	Zex[4] = Vex;
	Zex[5] = Aex;
	Zex[6] = Uex;

	return 0;
}


// calculates ideal mixing properties
long int TRegular::IdealProp( double *Zid )
{
	long int j;
	double si;
	si = 0.0;
	for (j=0; j<NComp; j++)
	{
		if ( x[j] > 1.0e-32 )
			si += x[j]*log(x[j]);
	}
	Hid = 0.0;
	CPid = 0.0;
	Vid = 0.0;
	Sid = (-1.)*R_CONST*si;
	Gid = Hid - Sid*Tk;
	Aid = Gid - Vid*Pbar;
	Uid = Hid - Vid*Pbar;

	// assignments (ideal mixing properties)
	Zid[0] = Gid;
	Zid[1] = Hid;
	Zid[2] = Sid;
	Zid[3] = CPid;
	Zid[4] = Vid;
	Zid[5] = Aid;
	Zid[6] = Uid;

	return 0;
}





//=============================================================================================
// Redlich-Kister model for multicomponent solid solutions (c) TW March 2007
// References: Hillert (1998)
//=============================================================================================


// Generic constructor for the TRedlichKister class
TRedlichKister::TRedlichKister( long int NSpecies, long int NParams, long int NPcoefs, long int MaxOrder,
        long int NPperDC, char Mod_Code, char Mix_Code, long int *arIPx,
        double *arIPc, double *arDCc, double *arWx, double *arlnGam,
        double *aphVOL, double T_k, double P_bar ):
        	TSolMod( NSpecies, NParams, NPcoefs, MaxOrder, NPperDC, 0, Mod_Code, Mix_Code,
						arIPx, arIPc, arDCc, arWx, arlnGam, aphVOL, T_k, P_bar )
{
	alloc_internal();
}


TRedlichKister::~TRedlichKister()
{
	free_internal();
}


void TRedlichKister::alloc_internal()
{
	Lu = new double [NPar][4];
	Ls = new double [NPar][4];
	Lcp = new double [NPar][4];
	Lv = new double [NPar][4];
	Lpt = new double [NPar][4];
}


void TRedlichKister::free_internal()
{
	if(Lu) delete[]Lu;
	if(Ls) delete[]Ls;
	if(Lv) delete[]Lv;
	if(Lpt) delete[]Lpt;
	if(Lcp) delete[]Lcp;
}


//   Calculates T,P corrected binary interaction parameters
long int TRedlichKister::PTparam()
{
	long int ip;

	if ( NPcoef < 16 || NPar < 1 )
		return 1;

	// read in interaction parameters
	for (ip=0; ip<NPar; ip++)
	{
		Lu[ip][0] = aIPc[NPcoef*ip+0];
	   	Ls[ip][0] = aIPc[NPcoef*ip+1];
	   	Lcp[ip][0] = aIPc[NPcoef*ip+2];
	   	Lv[ip][0] = aIPc[NPcoef*ip+3];
	   	Lpt[ip][0] = Lu[ip][0] + Ls[ip][0]*Tk + Lcp[ip][0]*Tk*log(Tk) + Lv[ip][0]*Pbar;
	    aIP[ip] = Lpt[ip][0];

	   	Lu[ip][1] = aIPc[NPcoef*ip+4];
	   	Ls[ip][1] = aIPc[NPcoef*ip+5];
	   	Lcp[ip][1] = aIPc[NPcoef*ip+6];
	   	Lv[ip][1] = aIPc[NPcoef*ip+7];
	   	Lpt[ip][1] = Lu[ip][1] + Ls[ip][1]*Tk + Lcp[ip][1]*Tk*log(Tk) + Lv[ip][1]*Pbar;

	   	Lu[ip][2] = aIPc[NPcoef*ip+8];
	   	Ls[ip][2] = aIPc[NPcoef*ip+9];
	   	Lcp[ip][2] = aIPc[NPcoef*ip+10];
	   	Lv[ip][2] = aIPc[NPcoef*ip+11];
	   	Lpt[ip][2] = Lu[ip][2] + Ls[ip][2]*Tk + Lcp[ip][2]*Tk*log(Tk) + Lv[ip][2]*Pbar;

	   	Lu[ip][3] = aIPc[NPcoef*ip+12];
	   	Ls[ip][3] = aIPc[NPcoef*ip+13];
	   	Lcp[ip][3] = aIPc[NPcoef*ip+14];
	   	Lv[ip][3] = aIPc[NPcoef*ip+15];
	   	Lpt[ip][3] = Lu[ip][3] + Ls[ip][3]*Tk + Lcp[ip][3]*Tk*log(Tk) + Lv[ip][3]*Pbar;

	}
	return 0;
}


// Calculates activity coefficients
long int TRedlichKister::MixMod()
{
	long int ip, j, i1, i2, L, I, J;
	double L0, L1, L2, L3;

	if ( NPcoef < 16 || NPar < 1 || NComp < 2 || MaxOrd < 2 || !x || !lnGamma )
		return 1;

	// loop over species
	for (j=0; j<NComp; j++)
	{
		lnGamRT = 0.;
		for (ip=0; ip<NPar; ip++)  // inter.parameters indexed with ip
		{
			i1 = aIPx[MaxOrd*ip];
			i2 = aIPx[MaxOrd*ip+1];

			if ( j == i1 || j == i2) // interaction terms with j
			{
				if ( i1 == j ) // check order of idexes
				{
					L = i1;
					I = i2;
					L0 = Lpt[ip][0];
					L1 = Lpt[ip][1];
					L2 = Lpt[ip][2];
					L3 = Lpt[ip][3];
				}
				else
				{
					L = i2;
					I = i1;
					L0 = Lpt[ip][0];
					L1 = -Lpt[ip][1];
					L2 = Lpt[ip][2];
					L3 = -Lpt[ip][3];
				}

				lnGamRT += L0*x[I]*(1.-x[L])
					+ L1*x[I]*(2.*(1.-x[L])*(x[L]-x[I])+x[I])
					+ L2*x[I]*(x[L]-x[I])*(3.*(1.-x[L])*(x[L]-x[I])+2.*x[I])
					+ L3*x[I]*pow((x[L]-x[I]),2.)*(4.*(1.-x[L])*(x[L]-x[I])+3.*x[I]);
			}

			else // interaction terms without j
			{
				I = i1;
				J = i2;
				L0 = Lpt[ip][0];
				L1 = Lpt[ip][1];
				L2 = Lpt[ip][2];
				L3 = Lpt[ip][3];

				lnGamRT -= x[I]*x[J]*( L0 + L1*2.*(x[I]-x[J])
					+ L2*3.*pow((x[I]-x[J]),2.)
					+ L3*4.*pow((x[I]-x[J]),3.) );
			}
		}

		lnGam = lnGamRT/(R_CONST*Tk);
		lnGamma[j] = lnGam;
	} // j

   	return 0;
}


// calculates bulk phase excess properties
long int TRedlichKister::ExcessProp( double *Zex )
{
	long int ip, i1, i2;
	double LU, LS, LCP, LV, LPT, g, v, s, cp, u;

	if ( NPcoef < 16 || NPar < 1 || NComp < 2 || MaxOrd < 2 || !x || !lnGamma )
		return 1;

   	// calculate bulk phase excess properties
   	g = 0.0; s = 0.0; cp = 0.0; v = 0.0; u = 0.0;

   	for (ip=0; ip<NPar; ip++)
   	{
   		i1 = aIPx[MaxOrd*ip];
   	   	i2 = aIPx[MaxOrd*ip+1];

      	LPT = Lpt[ip][0] + Lpt[ip][1]*(x[i1]-x[i2])
      			+ Lpt[ip][2]*pow((x[i1]-x[i2]),2.)
      			+ Lpt[ip][3]*pow((x[i1]-x[i2]),3.);

      	LV = Lv[ip][0] + Lv[ip][1]*(x[i1]-x[i2])
      			+ Lv[ip][2]*pow((x[i1]-x[i2]),2.)
      			+ Lv[ip][3]*pow((x[i1]-x[i2]),3.);

   	   	LU = (Lu[ip][0]-Lcp[ip][0]*Tk)
   	  			+ (Lu[ip][1]-Lcp[ip][1]*Tk)*(x[i1]-x[i2])
      			+ (Lu[ip][2]-Lcp[ip][2]*Tk)*pow((x[i1]-x[i2]),2.)
      			+ (Lu[ip][3]-Lcp[ip][3]*Tk)*pow((x[i1]-x[i2]),3.);

   	   	LS = (-Ls[ip][0]-Lcp[ip][0]*(1.+log(Tk)))
      			+ (-Ls[ip][1]-Lcp[ip][1]*(1.+log(Tk)))*(x[i1]-x[i2])
      			+ (-Ls[ip][2]-Lcp[ip][2]*(1.+log(Tk)))*pow((x[i1]-x[i2]),2.)
      			+ (-Ls[ip][3]-Lcp[ip][3]*(1.+log(Tk)))*pow((x[i1]-x[i2]),3.);

   	   	LCP = (-Lcp[ip][0]) + (-Lcp[ip][1])*(x[i1]-x[i2])
      			+ (-Lcp[ip][2])*pow((x[i1]-x[i2]),2.)
      			+ (-Lcp[ip][3])*pow((x[i1]-x[i2]),3.);

      	g += x[i1]*x[i2]*LPT;
      	v += x[i1]*x[i2]*LV;
      	u += x[i1]*x[i2]*LU;
      	s += x[i1]*x[i2]*LS;
      	cp += x[i1]*x[i2]*LCP;
  	}

	Gex = g;
	Sex = s;
	CPex = cp;
	Vex = v;
	Uex = u;
	Hex = Uex + Vex*Pbar;
	Aex = Gex - Vex*Pbar;
	Uex = Hex - Vex*Pbar;

	// assignments (excess properties)
	Zex[0] = Gex;
	Zex[1] = Hex;
	Zex[2] = Sex;
	Zex[3] = CPex;
	Zex[4] = Vex;
	Zex[5] = Aex;
	Zex[6] = Uex;

	return 0;
}


// calculates ideal mixing properties
long int TRedlichKister::IdealProp( double *Zid )
{

	long int j;
	double si;
	si = 0.0;
	for (j=0; j<NComp; j++)
	{
		if ( x[j] > 1.0e-32 )
			si += x[j]*log(x[j]);
	}
	Hid = 0.0;
	CPid = 0.0;
	Vid = 0.0;
	Sid = (-1.)*R_CONST*si;
	Gid = Hid - Sid*Tk;
	Aid = Gid - Vid*Pbar;
	Uid = Hid - Vid*Pbar;

	// assignments (ideal mixing properties)
	Zid[0] = Gid;
	Zid[1] = Hid;
	Zid[2] = Sid;
	Zid[3] = CPid;
	Zid[4] = Vid;
	Zid[5] = Aid;
	Zid[6] = Uid;

	return 0;
}





//=============================================================================================
// NRTL model for liquid solutions (c) TW June 2008
// References: Renon and Prausnitz (1968), Prausnitz et al. (1997)
//=============================================================================================


// Generic constructor for the TNRTL class
TNRTL::TNRTL( long int NSpecies, long int NParams, long int NPcoefs, long int MaxOrder,
        long int NPperDC, char Mod_Code, char Mix_Code, long int *arIPx,
        double *arIPc, double *arDCc, double *arWx, double *arlnGam,
        double *aphVOL, double T_k, double P_bar ):
        	TSolMod( NSpecies, NParams, NPcoefs, MaxOrder, NPperDC, 0, Mod_Code, Mix_Code,
						arIPx, arIPc, arDCc, arWx, arlnGam, aphVOL, T_k, P_bar )
{
	alloc_internal();
}


TNRTL::~TNRTL()
{
	free_internal();
}


void TNRTL::alloc_internal()
{
	Tau = new double *[NComp];
	dTau = new double *[NComp];
	d2Tau = new double *[NComp];
	Alp = new double *[NComp];
	dAlp = new double *[NComp];
	d2Alp = new double *[NComp];
	G = new double *[NComp];
	dG = new double *[NComp];
	d2G = new double *[NComp];

    for (long int j=0; j<NComp; j++)
    {
    	Tau[j] = new double [NComp];
    	dTau[j] = new double [NComp];
    	d2Tau[j] = new double [NComp];
    	Alp[j] = new double [NComp];
    	dAlp[j] = new double [NComp];
    	d2Alp[j] = new double [NComp];
		G[j] = new double [NComp];
		dG[j] = new double [NComp];
		d2G[j] = new double [NComp];
	}
}


void TNRTL::free_internal()
{
	// cleaning memory
	for (long int j=0; j<NComp; j++)
	{
		delete[]Tau[j];
	   	delete[]dTau[j];
	   	delete[]d2Tau[j];
	   	delete[]Alp[j];
	   	delete[]dAlp[j];
	   	delete[]d2Alp[j];
		delete[]G[j];
		delete[]dG[j];
		delete[]d2G[j];
	}
	delete[]Tau;
	delete[]dTau;
	delete[]d2Tau;
	delete[]Alp;
	delete[]dAlp;
	delete[]d2Alp;
	delete[]G;
	delete[]dG;
	delete[]d2G;
}


//   Calculates T,P corrected binary interaction parameters
long int TNRTL::PTparam()
{
	long int ip, i, j, i1, i2;
	double A, B, C, D, E, F, tau, dtau, d2tau, alp, dalp, d2alp;

    if ( NPcoef < 6 || NPar < 1 )
       return 1;

	// fill internal arrays of interaction parameters with standard value
	for (j=0; j<NComp; j++)
	{
		for ( i=0; i<NComp; i++ )
		{
			Tau[j][i] = 0.0;
			dTau[j][i] = 0.0;
			d2Tau[j][i] = 0.0;
			Alp[j][i] = 0.0;
			dAlp[j][i] = 0.0;
			d2Alp[j][i] = 0.0;
			G[j][i] = 1.0;
			dG[j][i] = 0.0;
			d2G[j][i] = 0.0;
		}
	}

	// read and convert parameters that have non-standard value
	for (ip=0; ip<NPar; ip++)
	{
		i1 = aIPx[MaxOrd*ip];
		i2 = aIPx[MaxOrd*ip+1];
		A = aIPc[NPcoef*ip+0];
		B = aIPc[NPcoef*ip+1];
		C = aIPc[NPcoef*ip+2];
		D = aIPc[NPcoef*ip+3];
		E = aIPc[NPcoef*ip+4];
		F = aIPc[NPcoef*ip+5];

		tau = A + B/Tk + C*Tk + D*log(Tk);	// partial derivatives of tau and alp
		dtau = - B/pow(Tk,2.) + C + D/Tk;
		d2tau = 2.*B/pow(Tk,3.) - D/pow(Tk,2.);
		alp = E + F*(Tk-273.15);
		dalp = F;
		d2alp = 0.0;

		Tau[i1][i2] = tau;
		dTau[i1][i2] =  dtau;
		d2Tau[i1][i2] = d2tau;
		Alp[i1][i2] = alp;
		dAlp[i1][i2] = dalp;
		d2Alp[i1][i2] =  d2alp;

		G[i1][i2] = exp(-Alp[i1][i2]*Tau[i1][i2]);
		dG[i1][i2] = - ( dAlp[i1][i2]*Tau[i1][i2] + Alp[i1][i2]*dTau[i1][i2] )
				* exp(-Alp[i1][i2]*Tau[i1][i2]);
		d2G[i1][i2] = - ( (d2Alp[i1][i2]*Tau[i1][i2] + 2.*dAlp[i1][i2]*dTau[i1][i2]
				+ Alp[i1][i2]*d2Tau[i1][i2])*G[i1][i2]
				+ (dAlp[i1][i2]*Tau[i1][i2] + Alp[i1][i2]*dTau[i1][i2])*dG[i1][i2] );

		// old version with constant Alp
		// dG[i1][i2] = -Alp[i1][i2] * exp( -Alp[i1][i2]*Tau[i1][i2] ) * dTau[i1][i2];
		// d2G[i1][i2] = -Alp[i1][i2]*(-Alp[i1][i2]*exp(-Alp[i1][i2]*Tau[i1][i2])*dTau[i1][i2]*dTau[i1][i2]
		//		+ exp(-Alp[i1][i2]*Tau[i1][i2])*d2Tau[i1][i2]);
	}
	return 0;
}


// Calculates activity coefficients
long int TNRTL::MixMod()
{
	long int  j, i, k;
	double K, L, M, N, O, lnGam;

	if ( NPcoef < 6 || NPar < 1 || NComp < 2 || MaxOrd < 2 || !x || !lnGamma )
	        return 1;

	// loop over species
	for (j=0; j<NComp; j++)
	{
		lnGam = 0.0;
		K = 0.0;
		L = 0.0;
		M = 0.0;
		for (i=0; i<NComp; i++)
		{
			N = 0.0;
			O = 0.0;
			K += ( x[i]*Tau[i][j]*G[i][j] );
			L += ( x[i]*G[i][j] );
			for (k=0; k<NComp; k++)
			{
				N += ( x[k]*G[k][i] );
				O += ( x[k]*Tau[k][i]*G[k][i] );
			}
			M += ( x[i]*G[j][i]/N * ( Tau[j][i] - O/N ) );
		}
		lnGam = K/L + M;
		lnGamma[j] = lnGam;
	}
	return 0;
}


// calculates bulk phase excess properties
long int TNRTL::ExcessProp( double *Zex )
{
	long int  j, i;
	double U, dU, V, dV, d2U, d2V, g, dg, d2g;

	if ( NPcoef < 6 || NPar < 1 || NComp < 2 || MaxOrd < 2 || !x || !lnGamma )
	        return 1;

	// calculate bulk phase excess properties
   	Gex = 0.0; Sex = 0.0; Hex = 0.0; CPex = 0.0; Vex = 0.0;
   	g = 0.0; dg = 0.0; d2g = 0.0;

   	for (j=0; j<NComp; j++)
   	{
		U = 0.0;
		V = 0.0;
		dU = 0.0;
		dV = 0.0;
		d2U = 0.0;
		d2V = 0.0;
		for (i=0; i<NComp; i++)
		{
			U += x[i]*Tau[i][j]*G[i][j];
			V += x[i]*G[i][j];
			dU += x[i] * ( dTau[i][j]*G[i][j] + Tau[i][j]*dG[i][j] );
			dV += x[i]*dG[i][j];
			d2U += x[i] * ( d2Tau[i][j]*G[i][j] + 2.*dTau[i][j]*dG[i][j]
					+ Tau[i][j]*d2G[i][j] );
			d2V += x[i]*d2G[i][j];
		}
		g += x[j]*U/V;
		dg += x[j] * (dU*V-U*dV)/pow(V,2.);
		d2g += x[j] * ( (d2U*V+dU*dV)/pow(V,2.) - (dU*V)*(2.*dV)/pow(V,3.)
				- (dU*dV+U*d2V)/pow(V,2.) + (U*dV)*(2.*dV)/pow(V,3.) );
	}

   	// final calculations
	Gex = g*R_CONST*Tk;
	Hex = -R_CONST*pow(Tk,2.)*dg;
	Sex = (Hex-Gex)/Tk;
	CPex = -R_CONST * ( 2.*Tk*dg + pow(Tk,2.)*d2g );
	Aex = Gex - Vex*Pbar;
	Uex = Hex - Vex*Pbar;

	// assignments (excess properties)
	Zex[0] = Gex;
	Zex[1] = Hex;
	Zex[2] = Sex;
	Zex[3] = CPex;
	Zex[4] = Vex;
	Zex[5] = Aex;
	Zex[6] = Uex;

	return 0;
}


// calculates ideal mixing properties
long int TNRTL::IdealProp( double *Zid )
{
	long int j;
	double si;
	si = 0.0;
	for (j=0; j<NComp; j++)
	{
		if ( x[j] > 1.0e-32 )
			si += x[j]*log(x[j]);
	}
	Hid = 0.0;
	CPid = 0.0;
	Vid = 0.0;
	Sid = (-1.)*R_CONST*si;
	Gid = Hid - Sid*Tk;
	Aid = Gid - Vid*Pbar;
	Uid = Hid - Vid*Pbar;

	// assignments (ideal mixing properties)
	Zid[0] = Gid;
	Zid[1] = Hid;
	Zid[2] = Sid;
	Zid[3] = CPid;
	Zid[4] = Vid;
	Zid[5] = Aid;
	Zid[6] = Uid;

	return 0;
}





//=============================================================================================
// Wilson model for liquid solutions (c) TW June 2008
// References: Prausnitz et al. (1997)
//=============================================================================================


// Generic constructor for the TWilson class
TWilson::TWilson( long int NSpecies, long int NParams, long int NPcoefs, long int MaxOrder,
        long int NPperDC, char Mod_Code, char Mix_Code, long int *arIPx,
        double *arIPc, double *arDCc, double *arWx, double *arlnGam,
        double *aphVOL, double T_k, double P_bar ):
        	TSolMod( NSpecies, NParams, NPcoefs, MaxOrder, NPperDC, 0, Mod_Code, Mix_Code,
						arIPx, arIPc, arDCc, arWx, arlnGam, aphVOL, T_k, P_bar )
{
	alloc_internal();
}


TWilson::~TWilson()
{
	free_internal();
}


void TWilson::alloc_internal()
{
	Lam = new double *[NComp];
	dLam = new double *[NComp];
	d2Lam = new double *[NComp];

    for (long int j=0; j<NComp; j++)
    {
    	Lam[j] = new double [NComp];
		dLam[j] = new double [NComp];
		d2Lam[j] = new double [NComp];
	}
}


void TWilson::free_internal()
{
   	// cleaning memory
   	for (long int j=0; j<NComp; j++)
   	{
   		delete[]Lam[j];
		delete[]dLam[j];
		delete[]d2Lam[j];
	}
	delete[]Lam;
	delete[]dLam;
	delete[]d2Lam;
}


// Calculates T-corrected interaction parameters
long int TWilson::PTparam()
{
	long int ip, i, j, i1, i2;
	double A, B, C, D, lam, dlam, d2lam;

    if ( NPcoef < 4 || NPar < 1 )
           return 1;

	// fill internal arrays of interaction parameters with standard value
	for (j=0; j<NComp; j++)
	{
		for ( i=0; i<NComp; i++ )
		{
			Lam[j][i] = 1.0;
			dLam[j][i] = 0.0;
			d2Lam[j][i] = 0.0;
		}
	}
	// read and convert parameters that have non-standard value
	for (ip=0; ip<NPar; ip++)
	{
		A = aIPc[NPcoef*ip+0];
		B = aIPc[NPcoef*ip+1];
		C = aIPc[NPcoef*ip+2];
		D = aIPc[NPcoef*ip+3];
		lam = exp( A + B/Tk + C*Tk + D*log(Tk) );
		dlam = lam*( - B/pow(Tk,2.) + C + D/Tk );
		d2lam = dlam*( - B/pow(Tk,2.) + C + D/Tk ) + lam*( 2.*B/pow(Tk,3.) - D/pow(Tk,2.) );

		i1 = aIPx[MaxOrd*ip];
		i2 = aIPx[MaxOrd*ip+1];
		Lam[i1][i2] = lam;
		dLam[i1][i2] = dlam;
		d2Lam[i1][i2] = d2lam;
	}
	return 0;
}


// Calculates activity coefficients
long int TWilson::MixMod()
{
	long int  j, i, k;
	double K, L, M, lnGam;

	if ( NPcoef < 4 || NPar < 1 || NComp < 2 || MaxOrd < 2 || !x || !lnGamma )
	        return 1;

	// loop over species
	for (j=0; j<NComp; j++)
	{
		lnGam = 0.0;
		K = 0.0;
		L = 0.0;
		for (i=0; i<NComp; i++)
		{
			M = 0.0;
			K += x[i]*Lam[j][i];
			for (k=0; k<NComp; k++)
			{
				M += x[k]*Lam[i][k];
			}
			L += x[i]*Lam[i][j]/M;
		}
		lnGam = 1.-log(K)-L;
		lnGamma[j] = lnGam;
	}

	return 0;
}


// calculates bulk phase excess properties
long int TWilson::ExcessProp( double *Zex )
{
	long int  j, i;
	double U, dU, d2U, g, dg, d2g;

	if ( NPcoef < 4 || NPar < 1 || NComp < 2 || MaxOrd < 2 || !x || !lnGamma )
	        return 1;

	// calculate bulk phase excess properties
	Gex = 0.0; Sex = 0.0; Hex = 0.0; CPex = 0.0; Vex = 0.0;
	g = 0.0; dg = 0.0; d2g = 0.0;

	// loop over species
	for (j=0; j<NComp; j++)
	{
		U = 0.0;
		dU = 0.0;
		d2U = 0.0;
		for (i=0; i<NComp; i++)
		{
			U += x[i]*Lam[j][i];
			dU += x[i]*dLam[j][i];
			d2U += x[i]*d2Lam[j][i];
		}
		g -= x[j]*log(U);
		dg -= x[j]*(1./U)*dU;
		d2g -= x[j] * ( (-1./pow(U,2.))*dU*dU + (1./U)*d2U );  // corrected 11.06.2008 (TW)
	}

	// final calculations
	Gex = g*R_CONST*Tk;
	Hex = -R_CONST*pow(Tk,2.)*dg;
	Sex = (Hex-Gex)/Tk;
	CPex = -R_CONST * ( 2.*Tk*dg + pow(Tk,2.)*d2g );

	// assignments (excess properties)
	Aex = Gex - Vex*Pbar;
	Uex = Hex - Vex*Pbar;
	Zex[0] = Gex;
	Zex[1] = Hex;
	Zex[2] = Sex;
	Zex[3] = CPex;
	Zex[4] = Vex;
	Zex[5] = Aex;
	Zex[6] = Uex;

	return 0;
}


// calculates ideal mixing properties
long int TWilson::IdealProp( double *Zid )
{
	long int j;
	double si;
	si = 0.0;
	for (j=0; j<NComp; j++)
	{
		if ( x[j] > 1.0e-32 )
			si += x[j]*log(x[j]);
	}
	Hid = 0.0;
	CPid = 0.0;
	Vid = 0.0;
	Sid = (-1.)*R_CONST*si;
	Gid = Hid - Sid*Tk;
	Aid = Gid - Vid*Pbar;
	Uid = Hid - Vid*Pbar;

	// assignments (ideal mixing properties)
	Zid[0] = Gid;
	Zid[1] = Hid;
	Zid[2] = Sid;
	Zid[3] = CPid;
	Zid[4] = Vid;
	Zid[5] = Aid;
	Zid[6] = Uid;

	return 0;
}





//=============================================================================================
// Ternary Margules (regular) model for solid solutions (c) TW June 2009
// References: Anderson and Crerar (1993); Anderson (2006)
//=============================================================================================


// Generic constructor for the TRegular class
TMargules::TMargules( long int NSpecies, long int NParams, long int NPcoefs, long int MaxOrder,
        long int NPperDC, char Mod_Code, char Mix_Code, long int *arIPx,
        double *arIPc, double *arDCc, double *arWx, double *arlnGam,
        double *aphVOL, double T_k, double P_bar ):
        	TSolMod( NSpecies, NParams, NPcoefs, MaxOrder, NPperDC, 0, Mod_Code, Mix_Code,
						arIPx, arIPc, arDCc, arWx, arlnGam, aphVOL, T_k, P_bar )
{
	// empty constructor
}


TMargules::~TMargules()
{
	// empty destructor
}


// Calculates T,P corrected binary interaction parameters
long int TMargules::PTparam()
{
	if ( NPcoef < 3 || NPar < 4 )
	           return 1;

	// load parameters
	WU12 = aIPc[0];
	WS12 = aIPc[1];
	WV12 = aIPc[2];
	WU13 = aIPc[3];
	WS13 = aIPc[4];
	WV13 = aIPc[5];
	WU23 = aIPc[6];
	WS23 = aIPc[7];
	WV23 = aIPc[8];
	WU123 = aIPc[9];
	WS123 = aIPc[10];
	WV123 = aIPc[11];

	// calculate parameters at (T,P)
	WG12 = WU12 - Tk*WS12 + Pbar*WV12;
	WG13 = WU13 - Tk*WS13 + Pbar*WV13;
	WG23 = WU23 - Tk*WS23 + Pbar*WV23;
	WG123 = WU123 - Tk*WS123 + Pbar*WV123;

	return 0;
}


// Calculates activity coefficients
long int TMargules::MixMod()
{
	double a12, a13, a23, a123, lnGam1, lnGam2, lnGam3, X1, X2, X3;

	if ( NPcoef < 3 || NPar < 4 || NComp < 3 || !x || !lnGamma )
		return 1;

	a12 = WG12 / (R_CONST*Tk);
	a13 = WG13 / (R_CONST*Tk);
	a23 = WG23 / (R_CONST*Tk);
	a123 = WG123 / (R_CONST*Tk);

	X1 = x[0];
	X2 = x[1];
	X3 = x[2];

	// activity coefficients (normalized by RT)
	lnGam1 = a12*X2*(1.-X1) + a13*X3*(1.-X1) - a23*X2*X3
				+ a123*X2*X3*(1.-2.*X1);
	lnGam2 = a23*X3*(1.-X2) + a12*X1*(1.-X2) - a13*X1*X3
				+ a123*X1*X3*(1.-2.*X2);
	lnGam3 = a13*X1*(1.-X3) + a23*X2*(1.-X3) - a12*X1*X2
				+ a123*X1*X2*(1.-2.*X3);

	// assignments
	lnGamma[0] = lnGam1;
	lnGamma[1] = lnGam2;
	lnGamma[2] = lnGam3;

	return 0;
}


// calculates bulk phase excess properties
long int TMargules::ExcessProp( double *Zex )
{
	double X1, X2, X3;

	X1 = x[0];
	X2 = x[1];
	X3 = x[2];

	// excess properties
	Gex = X1*X2*WG12 + X1*X3*WG13 + X2*X3*WG23 + X1*X2*X3*WG123;
	Vex = X1*X2*WV12 + X1*X3*WV13 + X2*X3*WV23 + X1*X2*X3*WV123;
	Uex = X1*X2*WU12 + X1*X3*WU13 + X2*X3*WU23 + X1*X2*X3*WU123;
	Sex = X1*X2*WS12 + X1*X3*WS13 + X2*X3*WS23 + X1*X2*X3*WS123;
	CPex = 0.0;
	Hex = Uex + Vex*Pbar;
	Aex = Gex - Vex*Pbar;

	// assignments (excess properties)
	Zex[0] = Gex;
	Zex[1] = Hex;
	Zex[2] = Sex;
	Zex[3] = CPex;
	Zex[4] = Vex;
	Zex[5] = Aex;
	Zex[6] = Uex;

	return 0;
}


// calculates ideal mixing properties
long int TMargules::IdealProp( double *Zid )
{
	long int j;
	double si;
	si = 0.0;
	for (j=0; j<NComp; j++)
	{
		if ( x[j] > 1.0e-32 )
			si += x[j]*log(x[j]);
	}
	Hid = 0.0;
	CPid = 0.0;
	Vid = 0.0;
	Sid = (-1.)*R_CONST*si;
	Gid = Hid - Sid*Tk;
	Aid = Gid - Vid*Pbar;
	Uid = Hid - Vid*Pbar;

	// assignments (ideal mixing properties)
	Zid[0] = Gid;
	Zid[1] = Hid;
	Zid[2] = Sid;
	Zid[3] = CPid;
	Zid[4] = Vid;
	Zid[5] = Aid;
	Zid[6] = Uid;

	return 0;
}





//=============================================================================================
// Binary Margules (subregular) model for solid solutions (c) TW June 2009
// References: Anderson and Crerar (1993); Anderson (2006)
//=============================================================================================


// Generic constructor for the TRegular class
TSubregular::TSubregular( long int NSpecies, long int NParams, long int NPcoefs, long int MaxOrder,
        long int NPperDC, char Mod_Code, char Mix_Code, long int *arIPx,
        double *arIPc, double *arDCc, double *arWx, double *arlnGam,
        double *aphVOL, double T_k, double P_bar ):
        	TSolMod( NSpecies, NParams, NPcoefs, MaxOrder, NPperDC, 0, Mod_Code, Mix_Code,
						arIPx, arIPc, arDCc, arWx, arlnGam, aphVOL, T_k, P_bar )
{
	// empty constructor
}


TSubregular::~TSubregular()
{
	// empty destructor
}


// Calculates T,P corrected binary interaction parameters
long int TSubregular::PTparam()
{
	if ( NPcoef < 3 || NPar < 2 )
	           return 1;

	// load parameters
	WU12 = aIPc[0];
	WS12 = aIPc[1];
	WV12 = aIPc[2];
	WU21 = aIPc[3];
	WS21 = aIPc[4];
	WV21 = aIPc[5];

	// calculate parameters at (T,P)
	WG12 = WU12 - Tk*WS12 + Pbar*WV12;
	WG21 = WU21 - Tk*WS21 + Pbar*WV21;

	return 0;
}


// Calculates activity coefficients
long int TSubregular::MixMod()
{
	double a1, a2, lnGam1, lnGam2, X1, X2;

	if ( NPcoef < 3 || NPar < 2 || NComp < 2 || !x || !lnGamma )
		return 1;

	a1 = WG12 / (R_CONST*Tk);
	a2 = WG21 / (R_CONST*Tk);

	X1 = x[0];
	X2 = x[1];

	// activity coefficients (normalized by RT)
	lnGam1 = (2.*a2-a1)*X2*X2 + 2.*(a1-a2)*X2*X2*X2;
	lnGam2 = (2.*a1-a2)*X1*X1 + 2.*(a2-a1)*X1*X1*X1;

	// assignments
	lnGamma[0] = lnGam1;
	lnGamma[1] = lnGam2;

	return 0;
}


// calculates bulk phase excess properties
long int TSubregular::ExcessProp( double *Zex )
{
	double X1, X2;

	X1 = x[0];
	X2 = x[1];

	// excess properties
	Gex = X1*X2*( X2*WG12 + X1*WG21 );
	Vex = X1*X2*( X2*WV12 + X1*WV21 );
	Uex = X1*X2*( X2*WU12 + X1*WU21 );
	Sex = X1*X2*( X2*WS12 + X1*WS21 );
	CPex = 0.0;
	Hex = Uex + Vex*Pbar;
	Aex = Gex - Vex*Pbar;

	// assignments
	Zex[0] = Gex;
	Zex[1] = Hex;
	Zex[2] = Sex;
	Zex[3] = CPex;
	Zex[4] = Vex;
	Zex[5] = Aex;
	Zex[6] = Uex;

	return 0;
}


// calculates ideal mixing properties
long int TSubregular::IdealProp( double *Zid )
{
	long int j;
	double si;
	si = 0.0;
	for (j=0; j<NComp; j++)
	{
		if ( x[j] > 1.0e-32 )
			si += x[j]*log(x[j]);
	}
	Hid = 0.0;
	CPid = 0.0;
	Vid = 0.0;
	Sid = (-1.)*R_CONST*si;
	Gid = Hid - Sid*Tk;
	Aid = Gid - Vid*Pbar;
	Uid = Hid - Vid*Pbar;

	// assignments (ideal mixing properties)
	Zid[0] = Gid;
	Zid[1] = Hid;
	Zid[2] = Sid;
	Zid[3] = CPid;
	Zid[4] = Vid;
	Zid[5] = Aid;
	Zid[6] = Uid;

	return 0;
}





//=============================================================================================
// Binary Guggenheim (Redlich-Kister) model for solid solutions (c) TW June 2009
// References: Anderson and Crerar (1993); Anderson (2006)
//=============================================================================================


// Generic constructor for the TBinaryGuggenheim class
TGuggenheim::TGuggenheim( long int NSpecies, long int NParams, long int NPcoefs, long int MaxOrder,
        long int NPperDC, char Mod_Code, char Mix_Code, long int *arIPx,
        double *arIPc, double *arDCc, double *arWx, double *arlnGam,
        double *aphVOL, double T_k, double P_bar ):
        	TSolMod( NSpecies, NParams, NPcoefs, MaxOrder, NPperDC, 0, Mod_Code, Mix_Code,
						arIPx, arIPc, arDCc, arWx, arlnGam, aphVOL, T_k, P_bar )
{
	// empty constructor
}


TGuggenheim::~TGuggenheim()
{
	// empty destructor
}


// Calculates T,P corrected binary interaction parameters
long int TGuggenheim::PTparam()
{
	if ( NPcoef < 3 || NPar < 1 )
	           return 1;

	// load parameters
	a0 = aIPc[0];
	a1 = aIPc[1];
	a1 = aIPc[2];

	return 0;
}


// Calculates activity coefficients
long int TGuggenheim::MixMod()
{
	double lnGam1, lnGam2, X1, X2;

	if ( NPcoef < 3 || NPar < 1 || NComp < 2 || !x || !lnGamma )
		return 1;

	X1 = x[0];
	X2 = x[1];

	// activity coefficients (normalized by RT)
	lnGam1 = X2*X2*( a0 + a1*(3.*X1-X2) + a2*(X1-X2)*(5.*X1-X2) );
	lnGam2 = X1*X1*( a0 - a1*(3.*X2-X1) + a2*(X2-X1)*(5.*X2-X1) );

	// assignments
	lnGamma[0] = lnGam1;
	lnGamma[1] = lnGam2;

	return 0;
}


// calculates bulk phase excess properties
long int TGuggenheim::ExcessProp( double *Zex )
{
	double X1, X2;

	X1 = x[0];
	X2 = x[1];

	// excess properties
	Gex = (X1*X2*( a0 + a1*(X1-X2) + a2*pow((X1-X2),2.) ))* (R_CONST*Tk);
	Vex = 0.0;
	Uex = (X1*X2*( a0 + a1*(X1-X2) + a2*pow((X1-X2),2.) ))* (R_CONST*Tk);
	Sex = 0.0;
	CPex = 0.0;
	Hex = Uex + Vex*Pbar;
	Aex = Gex - Vex*Pbar;

	// assignments
	Zex[0] = Gex;
	Zex[1] = Hex;
	Zex[2] = Sex;
	Zex[3] = CPex;
	Zex[4] = Vex;
	Zex[5] = Aex;
	Zex[6] = Uex;

	return 0;
}


// calculates ideal mixing properties
long int TGuggenheim::IdealProp( double *Zid )
{
	long int j;
	double si;
	si = 0.0;
	for (j=0; j<NComp; j++)
	{
		if ( x[j] > 1.0e-32 )
			si += x[j]*log(x[j]);
	}
	Hid = 0.0;
	CPid = 0.0;
	Vid = 0.0;
	Sid = (-1.)*R_CONST*si;
	Gid = Hid - Sid*Tk;
	Aid = Gid - Vid*Pbar;
	Uid = Hid - Vid*Pbar;

	// assignments (ideal mixing properties)
	Zid[0] = Gid;
	Zid[1] = Hid;
	Zid[2] = Sid;
	Zid[3] = CPid;
	Zid[4] = Vid;
	Zid[5] = Aid;
	Zid[6] = Uid;

	return 0;
}






//--------------------- End of s_fgl2.cpp ---------------------------


