//-------------------------------------------------------------------
// $Id$
//
// Implementation of writing/reading IPM work data structure files
//
// Copyright (C) 2006-2007 S.Dmytriyeva
//
// This file is part of the GEM-Vizor library and GEMIPM2K
// code package
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#include <math.h>

#include "io_arrays.h"
#include "m_param.h"
#include "gdatastream.h"

void TMulti::getLsModsum( long int& LsModSum, long int& LsIPxSum )
{  LsModSum = 0;
   LsIPxSum = 0;
   for(long int i=0; i<pm.FIs; i++)
   {
     LsModSum += (pm.LsMod[i*3]*pm.LsMod[i*3+2]);
     LsIPxSum += (pm.LsMod[i*3]*pm.LsMod[i*3+1]);
   }
}


void TMulti::getLsMdcsum( long int& LsMdcSum )
{  LsMdcSum = 0;
   for(long int i=0; i<pm.FIs; i++)
     LsMdcSum += (pm.LsMdc[i]*pm.L1[i]);
}

//---------------------------------------------------------//
//set default information
void TMulti::set_def( long int /*q*/)
{
    //mem_cpy( &pm.PunE, "jjbC", 4 );
    pm.PunE = 'j';         // Units of energy  { j;  J c C N reserved }
    pm.PunV = 'j';         // Units of volume  { j;  c L a reserved }
    pm.PunP = 'b';        // Units of pressure  { b;  B p P A reserved }
    pm.PunT = 'C';         // Units of temperature  { C; K F reserved }

    // mem_set( &pm.N, 0, 36*sizeof(long int));
    pm.N = 0;        	// N - number of IC in IPM problem
    pm.NR = 0;       	// NR - dimensions of R matrix
    pm.L = 0;        	// L -   number of DC in IPM problem
    pm.Ls = 0;       	// Ls -   total number of DC in multi-component phases
    pm.LO = 0;       	// LO -   index of water-solvent in IPM DC list
    pm.PG = 0;       	// PG -   number of DC in gas phase
    pm.PSOL = 0;     	// PSOL - number of DC in liquid hydrocarbon phase
    pm.Lads = 0;     	// Lads - number of DC in sorption phases
    pm.FI = 0;       	// FI -   number of phases in IPM problem
    pm.FIs = 0;      	// FIs -   number of multicomponent phases
    pm.FIa = 0;      	// FIa -   number of sorption phases
    pm.FI1 = 0;     // FI1 -   number of phases present in eqstate
    pm.FI1s = 0;    // FI1s -   number of multicomponent phases present in eqstate
    pm.FI1a = 0;    // FI1a -   number of sorption phases present in eqstate
    pm.IT = 0;      // It - number of completed IPM iterations
    pm.E = 0;       // PE - flag of electroneutrality constraint { 0 1 }
    pm.PD = 0;      // PD - mode of calling GammaCalc() { 0 1 2 3 4 }
    pm.PV = 0;      // PV - flag of system volume constraint { 0 1 }
    pm.PLIM = 0;    // PU - flag of activation of DC/phase restrictions { 0 1 }
    pm.Ec = 0;    // GammaCalc() return code: 0 (OK) or 1 (error)
    pm.K2 = 0;    // Number of Selekt2() loops
    pm.PZ = 0;    // Indicator of IPM-2 precision algorithm activation    funT = 0; sysT = 0;
    pm.pNP = 0; //Mode of FIA selection: 0-auto-SIMPLEX = 0;1-old eqstate = 0;-1-user's choice
    pm.pESU = 0;  // Unpack old eqstate from EQSTAT record?  0-no 1-yes
    pm.pIPN = 0;  // State of IPN-arrays:  0-create; 1-available; -1 remake
    pm.pBAL = 0;  // State of reloading CSD:  1- BAL only; 0-whole CSD
    pm.pFAG_ = 0;  // reserved SD
    pm.pTPD = 0;  // State of reloading thermod data: 0- all  1 - G0 only  2 - no
    pm.pULR = 0;  // Start recalc kinetic constraints (0-do not = 0; 1-do )internal
    pm.ITaia = 0;  // Number of IPM iterations completed in AIA mode (renamed from pRR1)
    pm.FIat = 0;   // max. number of surface site types
    pm.MK = 0;     // PM return code: 0 - continue;  1 - converged
    pm.W1 = 0;     // internal IPM-2 indicator
    pm.is = 0;     // is - index of IC for IPN equations ( GammaCalc() )
    pm.js = 0;     // js - index of DC for IPN equations ( GammaCalc() )
    pm.next = 0;
    pm.sitNcat = 0;    // SIT: number of cations
    pm.sitNan = 0;     // SIT: number of anions

    // mem_set( &pm.TC, 0, 54*sizeof(double));
    pm.TC = pm.TCc = 0.; 	// Temperature T = 0.; min.-max. (0 = 0.;2000 C)
    pm.T = pm.Tc = 0.;   	// T = 0.; min.-max. K
    pm.P = pm.Pc = 0.;   	// Pressure P = 0.; min.-max.(0 = 0.;10000 bar)
    pm.VX_ = pm.VXc = 0.;    // V(X) - volume of the system = 0.; min.-max. = 0.; cm3
    pm.GX_ = pm.GXc = 0.;    // Gibbs potential of the system G(X) = 0.; min.-max. (J)
    pm.AX_ = pm.AXc = 0.;    // Helmholtz potential of the system F(X) = 0.; reserved
    pm.UX_ = pm.UXc = 0.;  	// Internal energy of the system U(X) = 0.; reserved
    pm.HX_ = pm.HXc = 0.; 	// Total enthalpy of the system H(X) = 0.; reserved
    pm.SX_ = pm.SXc = 0.; 	// Total entropy of the system S(X) = 0.; reserved
    pm.CpX_ = pm.CpXc = 0.;  // reserved
    pm.CvX_ = pm.CvXc = 0.;  // reserved
    pm.T0 = 0.;         // reserved
    pm.VE = 0.;         // reserved
    pm.MBX = 0.;        // Total mass of the system = 0.; kg
    pm.FX = 0.;    	// Current Gibbs potential of the system in IPM = 0.; moles
    pm.IC = 0.;         // Effective molal ionic strength of aqueous electrolyte
    pm.pH = 0.;         // pH of aqueous solution
    pm.pe = 0.;         // pe of aqueous solution
    pm.Eh = 0.;         // Eh of aqueous solution = 0.; V
    pm.DHBM = 0.;       // Adjusted balance precision criterion (IPM-2 )
    pm.DSM = 0.;        // min value phase DS (IPM-2)
    pm.GWAT = 0.;       // used in ipm_gamma()
    pm.YMET = 0.;       // reserved
    fillValue( pm.denW, 0., 5 );
    fillValue( pm.denWg, 0., 5 );
    fillValue( pm.epsW, 0., 5 );
    fillValue( pm.epsWg, 0., 5 );
    pm.PCI = 0.;        // Current value of Dikin criterion of IPM convergence DK>=DX
    pm.DX = 0.;         // IPM convergence criterion threshold DX (1e-5)
    pm.lnP = 0.;        // log Ptotal
    pm.RT = 0.;         // RT: 8.31451*T (J/mole/K)
    pm.FRT = 0.;        // F/RT = 0.; F - Faraday constant = 96485.309 C/mol
    pm.Yw = 0.;         // Current number of moles of solvent in aqueous phase
    pm.ln5551 = 0.;     // ln(55.508373) = 4.0165339
    pm.aqsTail = 0.;    // v_j asymmetry correction factor for aqueous species
    pm.lowPosNum = 0.;  // Minimum DC amount defining the Ls set (1e-19)
    pm.logXw = 0.;      // work variable
    pm.logYFk = 0.;     // work variable
    pm.YFk = 0.;        // Current number of moles in a multicomponent phase
    pm.FitVar[0] =pm.FitVar[1] = pm.FitVar[2]= pm.FitVar[3]= pm.FitVar[4] = 0.;

    // pointers
    pm.sitNcat = 0;
    pm.sitNan = 0;
    pm.L1    = 0;
    pm.LsMod = 0;
    pm.LsMdc = 0;
    pm.mui   = 0;
    pm.muk   = 0;
    pm.muj   = 0;
    pm.SATX =0;
    pm.DUL   = 0;
    pm.DLL   = 0;
    pm.GEX   = 0;
    pm.PUL   = 0;
    pm.PLL   = 0;
    pm.YOF   = 0;
    pm.PMc   = 0;
    pm.DMc   = 0;
    pm.Vol   = 0;
    pm.HYM   = 0;
    pm.VL    = 0;
    pm.MM    = 0;
    pm.H0    = 0;
    pm.A0    = 0;
    pm.U0    = 0;
    pm.S0    = 0;
    pm.Cp0   = 0;
    pm.Cv0   = 0;
    pm.Pparc = 0;
    pm.Y_m   = 0;
    pm.Y_la  = 0;
    pm.Y_w   = 0;
    pm.Gamma = 0;
    pm.lnGmf = 0;
    pm.lnGmM = 0;
    pm.EZ    = 0;
    pm.Wb    = 0;
    pm.Wabs  = 0;
    pm.Rion  = 0;
    pm.Aalp  = 0;
    pm.Sigw  = 0;
    pm.Sigg  = 0;
    pm.Nfsp  = 0;
    pm.MASDT = 0;
    pm.FVOL  = 0;
    pm.FWGT  = 0;
    pm.XcapA = 0;
    pm.XcapB = 0;
    pm.XcapD = 0;
    pm.XdlA  = 0;
    pm.XdlB  = 0;
    pm.XdlD  = 0;
    pm.XpsiA = 0;
    pm.XpsiB = 0;
    pm.XpsiD = 0;
    pm.Xr0h0 = 0;
    pm.XlamA = 0;
    pm.Xetaf = 0;
    pm.Xcond = 0;
    pm.Xeps  = 0;
    pm.Awt   = 0;
    pm.A     = 0;
    pm.XFs   = 0;
        pm.Falps = 0;
        pm.Fug   = 0;
        pm.Fug_l = 0;
        pm.Ppg_l = 0;
        pm.XFTS  = 0;
        pm.MASDJ = 0;
        pm.G     = 0;
        pm.G0    = 0;
        pm.lnGam = 0;
        pm.lnGmo = 0;
//        pm.lnSAT = 0;
        pm.lnSAC = 0;
        pm.B     = 0;
        pm.U     = 0;
        pm.Uc     = 0;
        pm.U_r   = 0;
        pm.C     = 0;
        pm.IC_m  = 0;
        pm.IC_lm = 0;
        pm.IC_wm = 0;
        pm.BF    = 0;
        pm.BFC    = 0;
        pm.XF    = 0;
        pm.YF    = 0;
        pm.XFA   = 0;
        pm.YFA   = 0;
        pm.Falp  = 0;
        pm.XetaA = 0;
        pm.XetaB = 0;
        pm.XetaD = 0;       
        pm.X     = 0;
        pm.Y     = 0;
        pm.XY    = 0;
        pm.XU    = 0;
        pm.Qp    = 0;
        pm.Qd    = 0;
        pm.MU    = 0;
        pm.EMU   = 0;
        pm.NMU   = 0;
        pm.W     = 0;
        pm.Fx    = 0;
        pm.Wx    = 0;
        pm.F     = 0;
        pm.F0    = 0;
        pm.D     = 0;
     //   pm.R     = 0;
     //   pm.R1    = 0;
        pm.sMod  = 0;
        pm.SB    = 0;
        pm.SB1    = 0; 
        pm.SM    = 0;
        pm.SF    = 0;
        pm.SFs   = 0;
        pm.pbuf  = 0; 
        pm.RLC   = 0;
        pm.RSC   = 0;
        pm.RFLC  = 0;
        pm.RFSC  = 0;
        pm.ICC   = 0;
        pm.DCC   = 0;
        pm.PHC   = 0;
        pm.SCM   = 0;
        pm.SATT  = 0;
        pm.DCCW  = 0;
        pm.XcapF = 0;
        pm.SM2    = 0;
        pm.SM3    = 0;
        pm.SF2    = 0;
        pm.DCC3   = 0;
//  Added 16.11.2004 by Sveta
//    pm.sitXcat = 0;
//    pm.sitXan = 0;
//    pm.sitE = 0;
pm.IPx = 0;
pm.ITF = pm.ITG = 0;
pm.VPh = 0;
pm.GPh = 0;
pm.HPh = 0;
pm.SPh = 0;
pm.CPh = 0;
pm.APh = 0;
pm.UPh = 0;

}

//---------------------------------------------------------//
// writing MULTI to binary file
void TMulti::to_file( GemDataStream& ff  )
{
   if( pm.N < 2 || pm.L < 2 || pm.FI < 1 )
        Error( GetName(), "pm.N < 2 || pm.L < 2 || pm.FI < 1" );

   //static values
   char PAalp;
   char PSigm;


#ifndef IPMGEMPLUGIN

   PAalp = syp->PAalp;
   PSigm = syp->PSigm;
#else
   PAalp = PAalp_;
   PSigm = PSigm_;
#endif


   ff.writeArray(pm.stkey, sizeof(char)*(EQ_RKLEN+5));
   ff.writeArray( &pm.N, 38);
   ff.writeArray(&pm.TC, 55);
   ff << PAalp;
   ff << PSigm;
   ff.writeArray( pm.denW, 5);
   ff.writeArray( pm.denWg, 5);
   ff.writeArray( pm.epsW, 5);
   ff.writeArray( pm.epsWg, 5);

   //dynamic values

    // Part 1

    /* need  always to alloc vectors */
   ff.writeArray(pm.L1,  pm.FI);
   ff.writeArray(pm.muk, pm.FI);
   ff.writeArray(pm.mui, pm.N);
   ff.writeArray(pm.muj, pm.L);
   ff.writeArray(pm.DUL, pm.L);
   ff.writeArray(pm.DLL, pm.L);
   ff.writeArray(pm.Vol, pm.L);
   ff.writeArray(pm.Pparc, pm.L);
   ff.writeArray(pm.MM, pm.L);
   ff.writeArray(pm.Awt, pm.N);
   ff.writeArray(pm.A,  pm.N*pm.L);
   ff.writeArray(pm.XFs, pm.FI);
   ff.writeArray(pm.Falps, pm.FI);
   ff.writeArray(pm.G, pm.L);
   ff.writeArray(pm.G0, pm.L);
   ff.writeArray(pm.lnGam, pm.L);
   ff.writeArray(pm.lnGmo, pm.L);
   ff.writeArray(pm.B, pm.N);
   ff.writeArray(pm.U,  pm.N);
   ff.writeArray(pm.U_r, pm.N);
   ff.writeArray(pm.C, pm.N);
   ff.writeArray(pm.XF, pm.FI);
   ff.writeArray(pm.YF, pm.FI);
   ff.writeArray(pm.Falp, pm.FI);
   ff.writeArray(pm.X, pm.L);
   ff.writeArray(pm.Y, pm.L);
   ff.writeArray(pm.XY, pm.L);
   ff.writeArray(pm.MU, pm.L);
   ff.writeArray(pm.EMU,  pm.L);
   ff.writeArray(pm.NMU, pm.L);
   ff.writeArray(pm.W, pm.L);
   ff.writeArray(pm.F, pm.L);
   ff.writeArray(pm.F0, pm.L);
   ff.writeArray(pm.YOF, pm.FI);

   ff.writeArray((char*)pm.SB, (MAXICNAME+MAXSYMB)*pm.N);
   ff.writeArray((char*)pm.SB1, MAXICNAME * pm.N);
   ff.writeArray((char*)pm.SFs, (MAXPHNAME+MAXSYMB)*pm.FI);
   ff.writeArray((char*)pm.SM, MAXDCNAME * pm.L);
   ff.writeArray((char*)pm.SF, (MAXPHNAME+MAXSYMB)*pm.FI);
   ff.writeArray((char*)pm.SM2, MAXDCNAME * pm.Ls);
   ff.writeArray((char*)pm.SF2, (MAXPHNAME+MAXSYMB)*pm.FIs);

   ff.writeArray( pm.RLC, pm.L);
   ff.writeArray( pm.RSC, pm.L);
   ff.writeArray( pm.ICC, pm.N);
   ff.writeArray( pm.DCC, pm.L);
   ff.writeArray( pm.PHC, pm.FI);
   ff.writeArray( pm.DCCW, pm.L);

   ff.writeArray( pm.lnGmM, pm.L);
   ff.writeArray( pm.GEX,   pm.L);
   ff.writeArray( pm.FVOL, pm.FI);
   ff.writeArray( pm.FWGT, pm.FI);

    if( pm.L > 0 )
    {
      ff.writeArray(pm.Y_la, pm.L);
      ff.writeArray(pm.Y_w, pm.L);
      ff.writeArray(pm.Fx, pm.L);
      ff.writeArray(pm.Wx, pm.L);
      ff.writeArray(pm.VL, pm.L);
      ff.writeArray(pm.Gamma, pm.L);
      ff.writeArray(pm.lnGmf, pm.L);
//      ff.writeArray(pm.D, pm.L);
    }

   // Part 2  not requited arrays
    if( pm.FIs > 0 && pm.Ls > 0 )
    {
      ff.writeArray(pm.BF, pm.FIs*pm.N);
      ff.writeArray(pm.XFA, pm.FIs);
      ff.writeArray(pm.YFA, pm.FIs);
      ff.writeArray(pm.LsMod, pm.FIs*3);
      ff.writeArray(pm.LsMdc, pm.FIs);
      long int LsModSum;
      long int LsIPxSum;
      long int LsMdcSum;
      getLsModsum( LsModSum, LsIPxSum );
      getLsMdcsum( LsMdcSum );
      ff.writeArray(pm.IPx, LsIPxSum);
      ff.writeArray(pm.PMc, LsModSum);
      ff.writeArray(pm.DMc, LsMdcSum);
      ff.writeArray(pm.PUL, pm.FIs);
      ff.writeArray(pm.PLL, pm.FIs);

      ff.writeArray((char*)pm.sMod, 6*pm.FIs);
      ff.writeArray( pm.RFLC, pm.FIs);
      ff.writeArray( pm.RFSC, pm.FIs);
    }

    if( pm.LO > 1 )
    {
      ff.writeArray(pm.Y_m, pm.L);
      ff.writeArray(pm.IC_m, pm.N);
      ff.writeArray(pm.IC_lm, pm.N);
      ff.writeArray(pm.IC_wm,  pm.N);
    }

    /* dispersion and sorbtion phases */
    if( PAalp != S_OFF )
    {
      ff.writeArray(pm.Aalp, pm.FI);
      ff.writeArray((double *)pm.Xr0h0, pm.FI*2);
    }

   if( PSigm != S_OFF )
      ff.writeArray(pm.Sigw, pm.FI);

    if( PSigm != S_OFF )
      ff.writeArray(pm.Sigg, pm.FI);

    if( pm.E )
    {
      ff.writeArray(pm.EZ,  pm.L);
      ff.writeArray(pm.Xcond, pm.FI);
      ff.writeArray(pm.Xeps,  pm.FI);
    }

    if( pm.FIat > 0 && /*pm.Lads > 0 &&Sveta 12/09/99*/ pm.FIs > 0 )
    { /* ADSORPTION AND ION EXCHANGE */
      ff.writeArray((char*)pm.SCM, pm.FIs*pm.FIat);
      ff.writeArray((double*)pm.Nfsp, pm.FIs*pm.FIat);
      ff.writeArray((double*)pm.MASDT, pm.FIs*pm.FIat);
      ff.writeArray((double*)pm.XcapA, pm.FIs*pm.FIat);
      ff.writeArray((double*)pm.XcapB, pm.FIs*pm.FIat);
      ff.writeArray((double*)pm.XcapD, pm.FIs*pm.FIat);
      ff.writeArray((double*)pm.XcapF, pm.FIs*pm.FIat);
      ff.writeArray((double*)pm.XdlA, pm.FIs*pm.FIat);
      ff.writeArray((double*)pm.XdlB, pm.FIs*pm.FIat);
      ff.writeArray((double*)pm.XdlD, pm.FIs*pm.FIat);
      ff.writeArray((double*)pm.XpsiA, pm.FIs*pm.FIat);
      ff.writeArray((double*)pm.XpsiB, pm.FIs*pm.FIat);
      ff.writeArray((double*)pm.XpsiD, pm.FIs*pm.FIat);
      ff.writeArray((double*)pm.XlamA, pm.FIs*pm.FIat);
      ff.writeArray((double*)pm.Xetaf, pm.FIs*pm.FIat);
      ff.writeArray((double*)pm.XetaA, pm.FIs*pm.FIat);
      ff.writeArray((double*)pm.XetaB, pm.FIs*pm.FIat);
      ff.writeArray((double*)pm.XetaD, pm.FIs*pm.FIat);    
      ff.writeArray((double*)pm.XFTS, pm.FIs*pm.FIat);

ff.writeArray((long int*)pm.SATX, pm.Lads*4);
ff.writeArray(pm.SATT, pm.Lads);
ff.writeArray((double*)pm.MASDJ, pm.Lads*DFCN);
//      ff.writeArray(pm.MASDJ, pm.Ls);
ff.writeArray( (double*)pm.lnSAC, pm.Lads*4 );
ff.writeArray((char*)pm.SM3, MAXDCNAME * pm.Lads);
ff.writeArray( pm.DCC3, pm.Lads);
ff.writeArray((double*)pm.D, MST*MST);
    }

    if( pm.PG > 0 )
    {
      ff.writeArray(pm.Fug,  pm.PG);
      ff.writeArray(pm.Fug_l,  pm.PG);
      ff.writeArray(pm.Ppg_l,  pm.PG);
    }

    // Part 3

    if( pm.Ls > 1 && pm.FIs > 0 )
    {
      ff.writeArray(pm.Wb, pm.Ls);
      ff.writeArray(pm.Wabs, pm.Ls);
      ff.writeArray(pm.Rion, pm.Ls);

      ff.writeArray(pm.Qp, pm.FIs*QPSIZE);
      ff.writeArray(pm.Qd, pm.FIs*QDSIZE);
   }
   	ff.writeArray( pm.H0, pm.L);
   	ff.writeArray( pm.A0, pm.L);
   	ff.writeArray( pm.U0, pm.L);
   	ff.writeArray( pm.S0, pm.L);
   	ff.writeArray( pm.Cp0, pm.L);

//  Added 16.11.2004 by Sveta
//   if( pm.sitNcat*pm.sitNcat )
//     ff.writeArray( pm.sitE, pm.sitNcat*pm.sitNan );
//   if( pm.sitNcat )
//     ff.writeArray( pm.sitXcat, pm.sitNcat );
//   if( pm.sitNan )
//     ff.writeArray( pm.sitXan, pm.sitNan );
}

// reading MULTI from binary file
void TMulti::from_file( GemDataStream& ff )
{
   //static values
   char PAalp;
   char PSigm;
   
   ff.readArray(pm.stkey, sizeof(char)*(EQ_RKLEN+5));
   ff.readArray( &pm.N, 38);
   ff.readArray(&pm.TC, 55);
   ff >> PAalp;
   ff >> PSigm;
   ff.readArray( pm.denW, 5);
   ff.readArray( pm.denWg, 5);
   ff.readArray( pm.epsW, 5);
   ff.readArray( pm.epsWg, 5);

#ifndef IPMGEMPLUGIN
//   syp->PAalp = PAalp;
//   syp->PSigm = PSigm;
#else
   PAalp_ = PAalp;
   PSigm_ = PSigm;
#endif

   //realloc memory
#ifdef IPMGEMPLUGIN
   multi_realloc( PAalp, PSigm );
#else
   dyn_new();
#endif

   //dynamic values
    // Part 1

    /* need  always to alloc vectors */
   ff.readArray(pm.L1,  pm.FI);
   ff.readArray(pm.muk, pm.FI);
   ff.readArray(pm.mui, pm.N);
   ff.readArray(pm.muj, pm.L);
   ff.readArray(pm.DUL, pm.L);
   ff.readArray(pm.DLL, pm.L);
   ff.readArray(pm.Vol, pm.L);
   ff.readArray(pm.Pparc, pm.L);
   ff.readArray(pm.MM, pm.L);
   ff.readArray(pm.Awt, pm.N);
   ff.readArray(pm.A,  pm.N*pm.L);
   ff.readArray(pm.XFs, pm.FI);
   ff.readArray(pm.Falps, pm.FI);
   ff.readArray(pm.G, pm.L);
   ff.readArray(pm.G0, pm.L);
   ff.readArray(pm.lnGam, pm.L);
   ff.readArray(pm.lnGmo, pm.L);
   ff.readArray(pm.B, pm.N);
   ff.readArray(pm.U,  pm.N);
   ff.readArray(pm.U_r, pm.N);
   ff.readArray(pm.C, pm.N);
   ff.readArray(pm.XF, pm.FI);
   ff.readArray(pm.YF, pm.FI);
   ff.readArray(pm.Falp, pm.FI);
   ff.readArray(pm.X, pm.L);
   ff.readArray(pm.Y, pm.L);
   ff.readArray(pm.XY, pm.L);
   ff.readArray(pm.MU, pm.L);
   ff.readArray(pm.EMU,  pm.L);
   ff.readArray(pm.NMU, pm.L);
   ff.readArray(pm.W, pm.L);
   ff.readArray(pm.F, pm.L);
   ff.readArray(pm.F0, pm.L);
   ff.readArray(pm.YOF, pm.FI);

   ff.readArray((char*)pm.SB, (MAXICNAME+MAXSYMB)*pm.N);
   ff.readArray((char*)pm.SB1, MAXICNAME * pm.N);
   ff.readArray((char*)pm.SFs, (MAXPHNAME+MAXSYMB)*pm.FI);
   ff.readArray((char*)pm.SM, MAXDCNAME * pm.L);
   ff.readArray((char*)pm.SF, (MAXPHNAME+MAXSYMB)*pm.FI);
   ff.readArray((char*)pm.SM2, MAXDCNAME * pm.Ls);
   ff.readArray((char*)pm.SF2, (MAXPHNAME+MAXSYMB)*pm.FIs);

   ff.readArray( pm.RLC, pm.L);
   ff.readArray( pm.RSC, pm.L);
   ff.readArray( pm.ICC, pm.N);
   ff.readArray( pm.DCC, pm.L);
   ff.readArray( pm.PHC, pm.FI);
   ff.readArray( pm.DCCW, pm.L);

   ff.readArray( pm.lnGmM, pm.L);
   ff.readArray( pm.GEX,   pm.L);
   ff.readArray( pm.FVOL, pm.FI);
   ff.readArray( pm.FWGT, pm.FI);

    if( pm.L > 0 )
    {
      ff.readArray(pm.Y_la, pm.L);
      ff.readArray(pm.Y_w, pm.L);
      ff.readArray(pm.Fx, pm.L);
      ff.readArray(pm.Wx, pm.L);
      ff.readArray(pm.VL, pm.L);
      ff.readArray(pm.Gamma, pm.L);
      ff.readArray(pm.lnGmf, pm.L);
//      ff.readArray(pm.D, pm.L);
    }

   // Part 2  not requited arrays
    if( pm.FIs > 0 && pm.Ls > 0 )
    {
      ff.readArray(pm.BF, pm.FIs*pm.N);
      ff.readArray(pm.XFA, pm.FIs);
      ff.readArray(pm.YFA, pm.FIs);
      ff.readArray(pm.LsMod, pm.FIs*3);
      ff.readArray(pm.LsMdc, pm.FIs);
      long int LsModSum;
      long int LsIPxSum;
      long int LsMdcSum;
      getLsModsum( LsModSum, LsIPxSum );
      getLsMdcsum( LsMdcSum );

#ifdef IPMGEMPLUGIN
      pm.IPx = new long int[LsIPxSum];
      pm.PMc = new double[LsModSum];
      pm.DMc = new double[LsMdcSum];
#else
      pm.IPx = (long int *)aObj[ o_wi_ipxpm ].Alloc(LsIPxSum, 1, L_); 
      pm.PMc = (double *)aObj[ o_wi_pmc].Alloc( LsModSum, 1, D_);
      pm.DMc = (double *)aObj[ o_wi_dmc].Alloc( LsMdcSum, 1, D_ );
#endif

      ff.readArray(pm.IPx, LsIPxSum);
      ff.readArray(pm.PMc, LsModSum);
      ff.readArray(pm.DMc, LsMdcSum);
      ff.readArray(pm.PUL, pm.FIs);
      ff.readArray(pm.PLL, pm.FIs);

      ff.readArray((char*)pm.sMod, 6*pm.FIs);
      ff.readArray( pm.RFLC, pm.FIs);
      ff.readArray( pm.RFSC, pm.FIs);
    }

    if( pm.LO > 1 )
    {
      ff.readArray(pm.Y_m, pm.L);
      ff.readArray(pm.IC_m, pm.N);
      ff.readArray(pm.IC_lm, pm.N);
      ff.readArray(pm.IC_wm,  pm.N);
    }

    /* dispersion and sorbtion phases */
    if( PAalp != S_OFF )
    {
      ff.readArray(pm.Aalp, pm.FI);
      ff.readArray((double *)pm.Xr0h0, pm.FI*2);
    }

   if( PSigm != S_OFF )
      ff.readArray(pm.Sigw, pm.FI);

    if( PSigm != S_OFF )
      ff.readArray(pm.Sigg, pm.FI);

    if( pm.E )
    {
      ff.readArray(pm.EZ,  pm.L);
      ff.readArray(pm.Xcond, pm.FI);
      ff.readArray(pm.Xeps,  pm.FI);
    }

    if( pm.FIat > 0 && /*pm.Lads > 0 &&Sveta 12/09/99*/ pm.FIs > 0 )
    { /* ADSORBTION AND ION IXCHANDG */
      ff.readArray((char*)pm.SCM, pm.FIs*pm.FIat);
      ff.readArray((double*)pm.Nfsp, pm.FIs*pm.FIat);
      ff.readArray((double*)pm.MASDT, pm.FIs*pm.FIat);
      ff.readArray((double*)pm.XcapA, pm.FIs*pm.FIat);
      ff.readArray((double*)pm.XcapB, pm.FIs*pm.FIat);
      ff.readArray((double*)pm.XcapD, pm.FIs*pm.FIat);
      ff.readArray((double*)pm.XcapF, pm.FIs*pm.FIat);
      ff.readArray((double*)pm.XdlA, pm.FIs*pm.FIat);
      ff.readArray((double*)pm.XdlB, pm.FIs*pm.FIat);
      ff.readArray((double*)pm.XdlD, pm.FIs*pm.FIat);
      ff.readArray((double*)pm.XpsiA, pm.FIs*pm.FIat);
      ff.readArray((double*)pm.XpsiB, pm.FIs*pm.FIat);
      ff.readArray((double*)pm.XpsiD, pm.FIs*pm.FIat);
      ff.readArray((double*)pm.XlamA, pm.FIs*pm.FIat);
      ff.readArray((double*)pm.Xetaf, pm.FIs*pm.FIat);
      ff.readArray((double*)pm.XetaA, pm.FIs*pm.FIat);
      ff.readArray((double*)pm.XetaB, pm.FIs*pm.FIat);
      ff.readArray((double*)pm.XetaD, pm.FIs*pm.FIat);    
      ff.readArray((double*)pm.XFTS, pm.FIs*pm.FIat);

ff.readArray((long int*)pm.SATX, pm.Lads*4);
ff.readArray(pm.SATT, pm.Lads);
ff.readArray((double*)pm.MASDJ, pm.Lads*DFCN);
//      ff.readArray(pm.MASDJ, pm.Ls);
ff.readArray((double*)pm.lnSAC, pm.Lads*4);
ff.readArray((char*)pm.SM3, MAXDCNAME * pm.Lads);
ff.readArray( pm.DCC3, pm.Lads);
ff.readArray((double*)pm.D, MST*MST);

    }

    if( pm.PG > 0 )
    {
      ff.readArray(pm.Fug,  pm.PG);
      ff.readArray(pm.Fug_l,  pm.PG);
      ff.readArray(pm.Ppg_l,  pm.PG);
    }

    // Part 3

    if( pm.Ls > 1 && pm.FIs > 0 )
    {
      ff.readArray(pm.Wb, pm.Ls);
      ff.readArray(pm.Wabs, pm.Ls);
      ff.readArray(pm.Rion, pm.Ls);

      ff.readArray(pm.Qp, pm.FIs*QPSIZE);
      ff.readArray(pm.Qd, pm.FIs*QDSIZE);
   }
   	ff.readArray( pm.H0, pm.L);
   	ff.readArray( pm.A0, pm.L);
   	ff.readArray( pm.U0, pm.L);
   	ff.readArray( pm.S0, pm.L);
   	ff.readArray( pm.Cp0, pm.L);
//  Added 16.11.2004 by Sveta
//   if( pm.sitNcat*pm.sitNcat )
//     ff.readArray( pm.sitE, pm.sitNcat*pm.sitNan );
//   if( pm.sitNcat )
//     ff.readArray( pm.sitXcat, pm.sitNcat );
//   if( pm.sitNan )
//     ff.readArray( pm.sitXan, pm.sitNan );
}


#ifdef IPMGEMPLUGIN

// realloc dynamic memory
void TMulti::multi_realloc( char PAalp, char PSigm )
{
  long int ii, jj ;
   if( pm.N < 2 || pm.L < 2 || pm.FI < 1 )
        Error( GetName(), "pm.N < 2 || pm.L < 2 || pm.FI < 1" );

    // Part 1
     // need  always to alloc vectors
 pm.L1 = new long int[pm.FI];
 pm.muk = new long int[pm.FI];
 for( ii=0; ii<pm.FI; ii++)
 {   pm.L1[ii] = 0;
	 pm.muk[ii] = ii;
 }
 pm.mui = new long int[pm.N];
 for( ii=0; ii<pm.N; ii++)
   pm.mui[ii] = ii;
 pm.muj = new long int[pm.L];
 for( ii=0; ii<pm.L; ii++)
   pm.muj[ii] = ii;

 pm.DUL = new double[pm.L];
 pm.DLL = new double[pm.L];
 pm.Vol = new double[pm.L];
 pm.Pparc = new double[pm.L];
 pm.MM = new double[pm.L];
 pm.G = new double[pm.L];
 pm.G0 = new double[pm.L];
 pm.lnGam = new double[pm.L];
 pm.lnGmo = new double[pm.L];
 pm.X = new double[pm.L];
 pm.Y = new double[pm.L];
 pm.XY = new double[pm.L];
 pm.MU = new double[pm.L];
 pm.EMU = new double[pm.L];
 pm.NMU = new double[pm.L];
 pm.W = new double[pm.L];
 pm.F = new double[pm.L];
 pm.F0 = new double[pm.L];
 pm.RLC = new char[pm.L];
 pm.RSC = new char[pm.L];
 pm.DCC = new char[pm.L];
 pm.DCCW = new char[pm.L];
 pm.lnGmM = new double[pm.L];
 pm.GEX = new double[pm.L]; //24
 for( ii=0; ii<pm.L; ii++ )
 {
	 pm.DUL[ii] = 1e6;
	 pm.DLL[ii] = 0.0;
	 pm.Vol[ii] = 0.0;
     pm.Pparc[ii] = 1.;
     pm.MM[ii] = 0.0;
     pm.G[ii] = 0.0;
     pm.G0[ii] = 0.0;
     pm.lnGam[ii] = 0.0;
     pm.lnGmo[ii] = 0.0;
     pm.X[ii] = 0.0;
     pm.Y[ii] = 0.0;
     pm.XY[ii] = 0.0;
     pm.MU[ii] = 0.0;
     pm.EMU[ii] = 0.0;
     pm.NMU[ii] = 0.0;
     pm.W[ii] = 0.0;
     pm.F[ii] = 0.0;
     pm.F0[ii] = 0.0;
     pm.RLC[ii] = 'B';
     pm.RSC[ii] = 'M';
     pm.DCC[ii] = 0;
     pm.DCCW[ii] = 0;
     pm.lnGmM[ii] = 0.0;
     pm.GEX[ii] = 0.0;
 }

 pm.A = new double[pm.N*pm.L];
 for( ii=0; ii<pm.N*pm.L; ii++ )
	 pm.A[ii] = 0.0;

 pm.Awt = new double[pm.N];
 pm.B = new double[pm.N];
 pm.U = new double[pm.N];
 pm.U_r = new double[pm.N];
 pm.C = new double[pm.N];
 pm.ICC = new char[pm.N];  //6
 for( ii=0; ii<pm.N; ii++ )
 {
	 pm.Awt[ii] = 0.0;
	 pm.B[ii] = 0.0;
	 pm.U[ii] = 0.0;
	 pm.U_r[ii] = 0.0;
	 pm.C[ii] = 0.0;
	 pm.ICC[ii] = 0;
 }

 pm.XFs = new double[pm.FI];
 pm.Falps = new double[pm.FI];
 pm.XF = new double[pm.FI];
 pm.YF = new double[pm.FI];
 pm.Falp = new double[pm.FI];
 pm.YOF = new double[pm.FI];
 pm.PHC = new char[pm.FI];
 pm.FVOL = new double[pm.FI];
 pm.FWGT = new double[pm.FI]; //9
 for( ii=0; ii<pm.FI; ii++ )
 {
	 pm.XFs[ii] = 0.0;
	 pm.Falps[ii] = 0.0;
	 pm.XF[ii] = 0.0;
	 pm.YF[ii] = 0.0;
	 pm.Falp[ii] = 0.0;
	 pm.YOF[ii] = 0.0;
	 pm.PHC[ii] = 0;
	 pm.FVOL[ii] = 0.0;
	 pm.FWGT[ii] = 0.0;
 }

  pm.SB = new char[pm.N][MAXICNAME+MAXSYMB];
  pm.SB1 = new char[pm.N][MAXICNAME];
  for( ii=0; ii<pm.N; ii++)
  {
	  fillValue( pm.SB[ii], '\0', MAXICNAME+MAXSYMB);
	  fillValue( pm.SB1[ii], '\0', MAXICNAME);
  }
  pm.SF = new char[pm.FI][MAXPHNAME+MAXSYMB];
  pm.SFs = new char[pm.FI][MAXPHNAME+MAXSYMB];
  for( ii=0; ii<pm.FI; ii++)
  {
 	  fillValue( pm.SF[ii], '\0', MAXPHNAME+MAXSYMB);
 	  fillValue( pm.SFs[ii], '\0',MAXPHNAME+MAXSYMB);
   }
  pm.SM = new char[pm.L][MAXDCNAME];
  for( ii=0; ii<pm.L; ii++)
 	  fillValue( pm.SM[ii], '\0', MAXDCNAME);
  pm.SM2 = new char[pm.Ls][MAXDCNAME];
  for( ii=0; ii<pm.Ls; ii++)
 	  fillValue( pm.SM2[ii], '\0', MAXDCNAME);
  pm.SF2 = new char[pm.FIs][MAXPHNAME+MAXSYMB];
  for( ii=0; ii<pm.FIs; ii++)
 	  fillValue( pm.SF2[ii], '\0', MAXPHNAME+MAXSYMB);

 if( pm.L > 0 )
 {
   pm.Y_la = new double[pm.L];
   pm.Y_w = new double[pm.L];
   pm.Fx = new double[pm.L];
   pm.Wx = new double[pm.L];
   pm.VL = new double[pm.L];
   pm.Gamma = new double[pm.L];
   pm.lnGmf = new double[pm.L]; //7
   for( ii=0; ii<pm.L; ii++ )
   {
	   pm.Y_la[ii] = 0.0;
	   pm.Y_w[ii] = 0.0;
	   pm.Fx[ii] = 0.0;
	   pm.Wx[ii] = 0.0;
	   pm.VL[ii] = 0.0;
	   pm.Gamma[ii] = 0.0;
	   pm.lnGmf[ii] = 0.0;
   }
   //   pm.D = new double[pm.L];
 }
 else
 {
   pm.Y_la = 0;
   pm.Y_w = 0;
   pm.Fx = 0;
   pm.Wx = 0;
   pm.VL = 0;
   pm.Gamma = 0;
   pm.lnGmf = 0;
//   pm.D = 0;
 }

   // Part 2  not always required arrays

 if( pm.FIs > 0 && pm.Ls > 0 )
 {
   pm.BF = new double[pm.FIs*pm.N];
   for( ii=0; ii<pm.FIs*pm.N; ii++ )
	   pm.BF[ii] = 0.0;
   pm.BFC = new double[pm.N];
   for( ii=0; ii<pm.N; ii++ )
	   pm.BFC[ii] = 0.0;

   pm.XFA = new double[pm.FIs];
   pm.YFA = new double[pm.FIs];
   pm.LsMdc = new long int[pm.FIs];
   pm.PUL = new double[pm.FIs];
   pm.PLL = new double[pm.FIs]; //5
   for( ii=0; ii<pm.FIs; ii++ )
   {
	   pm.XFA[ii] = 0.0;
	   pm.YFA[ii] = 0.0;
	   pm.LsMdc[ii] = 0.0;
	   pm.PUL[ii] = 1e6;
	   pm.PLL[ii] = 0.0;
   }
   pm.IPx = 0;
   pm.PMc = 0;
   pm.DMc = 0;
   pm.LsMod = new long int[pm.FIs*3];
   for( ii=0; ii<pm.FIs*3; ii++ )
     pm.LsMod[ii] =0;

   pm.sMod = new char[pm.FIs][6];
   pm.RFLC = new char[pm.FIs];
   pm.RFSC = new char[pm.FIs];
   for( ii=0; ii<pm.FIs; ii++)
   {
 	  fillValue( pm.sMod[ii], '\0', 6);
 	  pm.RFLC[ii] = 0;
      pm.RFSC[ii] = 0;
   }

 }
 else
 {
   pm.BF = 0;
   pm.BFC = 0;
   pm.XFA = 0;
   pm.YFA = 0;
   pm.LsMod = 0;
   pm.LsMdc = 0;
   pm.PMc = 0;
   pm.DMc = 0;
   pm.PUL = 0;
   pm.PLL = 0;

   pm.sMod = 0;
   pm.RFLC = 0;
   pm.RFSC = 0;
 }

 if( pm.LO > 1 )
 {
   pm.Y_m = new double[pm.L];
   for( ii=0; ii<pm.L; ii++ )
	   pm.Y_m[ii] = 0.0;
   pm.IC_m = new double[pm.N];
   pm.IC_lm = new double[pm.N];
   pm.IC_wm = new double[pm.N];
   for( ii=0; ii<pm.N; ii++ )
   {
	   pm.IC_m[ii] = 0.0;
	   pm.IC_lm[ii] = 0.0;
	   pm.IC_wm[ii] = 0.0;
   }
 }
 else
 {
   pm.Y_m = 0;
   pm.IC_m = 0;
   pm.IC_lm = 0;
   pm.IC_wm = 0;
 }

 // dispersion and sorbtion phases
 if( PAalp != S_OFF )
 {
   pm.Aalp = new double[pm.FI];
   for( ii=0; ii<pm.FI; ii++ )
	   pm.Aalp[ii] = 0.0;
   pm.Xr0h0 = new double[pm.FI][2];
   for( ii=0; ii<pm.FI; ii++ )
	  pm.Xr0h0[ii][0] =  pm.Xr0h0[ii][1] = 0.0;
 }
 else
 {
   pm.Aalp = 0;
   pm.Xr0h0 = 0;
 }

 if( PSigm != S_OFF )
 {   pm.Sigw = new double[pm.FI];
     pm.Sigg = new double[pm.FI];
     for( ii=0; ii<pm.FI; ii++ )
     {
    	 pm.Sigw[ii] = 0.0;
    	 pm.Sigg[ii] = 0.0;
     }
 }
 else
 {   pm.Sigw = 0;
     pm.Sigg = 0;
 }

 if( pm.E )
 {
    pm.EZ = new double[pm.L];
    for( ii=0; ii<pm.L; ii++ )
    	pm.EZ[ii] = 0.0;
    pm.Xcond = new double[pm.FI];
    pm.Xeps = new double[pm.FI];
    for( ii=0; ii<pm.FI; ii++ )
    {
        pm.Xcond[ii] = 0.0;
        pm.Xeps[ii] = 0.0;
    }
 }
 else
 {
    pm.EZ = 0;
    pm.Xcond = 0;
    pm.Xeps = 0;
 }

 if( pm.FIat > 0 /*&& pm.Lads > 0*/ && pm.FIs > 0 )
 { // ADSORBTION AND ION IXCHANDG
   pm.SATX = new long int[pm.Lads][4];
   pm.MASDJ = new double[pm.Lads][DFCN];
   pm.lnSAC = new double[pm.Lads][4];
   for( ii=0; ii<pm.Lads; ii++ )
   {
	   pm.SATX[ii][0] = pm.SATX[ii][1] = pm.SATX[ii][2] = pm.SATX[ii][3] = 0;
	   pm.lnSAC[ii][0] = pm.lnSAC[ii][1] = pm.lnSAC[ii][2] = pm.lnSAC[ii][3] = 0.0;
	  for( jj=0; jj<MST; jj++ )
		  pm.MASDJ[ii][jj] = 0.0;
   }

   pm.SCM  = new char[pm.FIs][MST];
   pm.Nfsp = new double[pm.FIs][MST];
   pm.MASDT = new double[pm.FIs][MST];
   pm.XcapA = new double[pm.FIs][MST];
   pm.XcapB = new double[pm.FIs][MST];
   pm.XcapD = new double[pm.FIs][MST];
   pm.XcapF = new double[pm.FIs][MST];
   pm.XdlA = new double[pm.FIs][MST];
   pm.XdlB = new double[pm.FIs][MST];
   pm.XdlD = new double[pm.FIs][MST];
   pm.XpsiA = new double[pm.FIs][MST];
   pm.XpsiB = new double[pm.FIs][MST];
   pm.XpsiD = new double[pm.FIs][MST];
   pm.XlamA = new double[pm.FIs][MST];
   pm.Xetaf = new double[pm.FIs][MST];
   pm.XetaA = new double[pm.FIs][MST];
   pm.XetaB = new double[pm.FIs][MST];
   pm.XetaD = new double[pm.FIs][MST];
   pm.XFTS = new double[pm.FIs][MST];  //19
   for( ii=0; ii<pm.FIs; ii++ )
   	  for( jj=0; jj<MST; jj++ )
   	  {
   		  pm.SCM[ii][jj]  = 0;
	      pm.Nfsp[ii][jj] = 0.0;
	      pm.MASDT[ii][jj] = 0.0;
	      pm.XcapA[ii][jj] = 0.0;
	      pm.XcapB[ii][jj] = 0.0;
	      pm.XcapD[ii][jj] = 0.0;
	      pm.XcapF[ii][jj] = 0.0;
	      pm.XdlA[ii][jj] = 0.0;
	      pm.XdlB[ii][jj] = 0.0;
	      pm.XdlD[ii][jj] = 0.0;
	      pm.XpsiA[ii][jj] = 0.0;
	      pm.XpsiB[ii][jj] = 0.0;
	      pm.XpsiD[ii][jj] = 0.0;
	      pm.XlamA[ii][jj] = 0.0;
	      pm.Xetaf[ii][jj] = 0.0;
	      pm.XetaA[ii][jj] = 0.0;
	      pm.XetaB[ii][jj] = 0.0;
	      pm.XetaD[ii][jj] = 0.0;
	      pm.XFTS[ii][jj] = 0.0;
   	  }

  pm.SATT = new char[pm.Lads];
  pm.SM3 = new char[pm.Lads][MAXDCNAME];
  pm.DCC3 = new char[pm.Lads];
  for( ii=0; ii<pm.Lads; ii++)
  {
	  fillValue( pm.SM3[ii], '\0', MAXDCNAME);
	  pm.SATT[ii] = 0;
     pm.DCC3[ii] = 0;
  }

  pm.D = new double[MST][MST];
  for( ii=0; ii<MST; ii++ )
  	  for( jj=0; jj<MST; jj++ )
   	      pm.D[ii][jj] = 0.0;

 }
else
 { // ADSORPTION AND ION EXCHANGE
   pm.SCM  = 0;
    pm.Nfsp = 0;
    pm.MASDT = 0;
    pm.XcapA = 0;
    pm.XcapB = 0;
    pm.XcapD = 0;
    pm.XcapF = 0;
    pm.XdlA = 0;
    pm.XdlB = 0;
    pm.XdlD = 0;
    pm.XpsiA = 0;
    pm.XpsiB = 0;
    pm.XpsiD = 0;
    pm.XlamA = 0;
    pm.Xetaf = 0;
    pm.XetaA = 0;
    pm.XetaB = 0;
    pm.XetaD = 0;
    pm.MASDJ = 0;
    pm.XFTS = 0;
    pm.lnSAC = 0;
    pm.SATT = 0;
    pm.SM3 = 0;
    pm.DCC3 = 0;
    pm.D = 0;
 }

 if( pm.PG > 0 )
 {
  pm.Fug = new double[pm.PG];
  pm.Fug_l = new double[pm.PG];
  pm.Ppg_l = new double[pm.PG];
  for( ii=0; ii<pm.PG; ii++ )
  {
	  pm.Fug[ii] = 0.;
	  pm.Fug_l[ii] = 0.;
	  pm.Ppg_l[ii] = 0.;
  }
 }
else
 {
  pm.Fug = 0;
  pm.Fug_l = 0;
  pm.Ppg_l = 0;
 }

   // Part 3
 if( pm.Ls > 1 && pm.FIs > 0 )
 {
    pm.Wb = new double[pm.Ls];
    pm.Wabs = new double[pm.Ls];
    pm.Rion = new double[pm.Ls];
    for( ii=0; ii<pm.Ls; ii++ )
    {
        pm.Wb[ii] = 0.;
        pm.Wabs[ii] = 0.;
        pm.Rion[ii] = 0.;
    }
    pm.Qp = new double[pm.FIs*QPSIZE];
    pm.Qd = new double[pm.FIs*QDSIZE];
    for( ii=0; ii<pm.FIs*QPSIZE; ii++ )
        {
            pm.Qp[ii] = 0.;
            pm.Qd[ii] = 0.;

        }
 }
 else
 {
    pm.Wb = 0;
    pm.Wabs = 0;
    pm.Rion = 0;
    pm.Qp = 0;
    pm.Qd = 0;

 }

 // added SD 03/02/2009
 pm.XU = new double[pm.L];
 for( ii=0; ii<pm.L; ii++ )
 	  pm.XU[ii] = 0.;
 pm.Uc = new double[pm.N];
  for( ii=0; ii<pm.N; ii++ )
  	  pm.Uc[ii] = 0.;

  pm.Cp0   = new double[pm.L];
  pm.H0    = new double[pm.L];
  pm.U0    = new double[pm.L];
  pm.S0    = new double[pm.L];
  pm.A0    = new double[pm.L];
  for( ii=0; ii<pm.L; ii++ )
  {
	  pm.Cp0[ii]   = 0.;
	  pm.H0[ii]    = 0.;
	  pm.U0[ii]    = 0.;
	  pm.S0[ii]    = 0.;
	  pm.A0[ii]    = 0.;
	  
  }
  pm.VPh   = new double[pm.FIs][MIXPHPROPS];
  pm.GPh   = new double[pm.FIs][MIXPHPROPS];
  pm.HPh   = new double[pm.FIs][MIXPHPROPS];
  pm.SPh   = new double[pm.FIs][MIXPHPROPS];
  pm.CPh   = new double[pm.FIs][MIXPHPROPS];
  pm.APh   = new double[pm.FIs][MIXPHPROPS];
  pm.UPh   = new double[pm.FIs][MIXPHPROPS];
  for( ii=0; ii<pm.FIs; ii++ )
    for( jj=0; jj<MIXPHPROPS; jj++ )
  {
	  pm.VPh[ii][jj]  = 0.;
	  pm.GPh[ii][jj]  = 0.;
	  pm.HPh[ii][jj]  = 0.;
	  pm.SPh[ii][jj]  = 0.;
	  pm.CPh[ii][jj]  = 0.;
	  pm.APh[ii][jj]  = 0.;
	  pm.UPh[ii][jj]  = 0.;
  }
	  
 Alloc_TSolMod( pm.FIs );

//  Added 16.11.2004 by Sveta
//    if( pm.sitNcat*pm.sitNcat )
//    { pm.sitE = new double[pm.sitNcat*pm.sitNan];
//      fillValue(pm.sitE, 0, pm.sitNcat*pm.sitNan);
//    }
//    else
//       pm.sitE = 0;
//    if( pm.sitNcat )
//    {  pm.sitXcat = new long int[pm.sitNcat];
//       fillValue(pm.sitXcat, 0, pm.sitNcat);
//     }
//    else
//       pm.sitXcat = 0;
//    if( pm.sitNan )
//    {   pm.sitXan = new long int[pm.sitNan];
//        fillValue(pm.sitXan, 0, pm.sitNan);
//    }
//    else
//       pm.sitXan = 0;
}


// Reallocation of dynamic memory
void TMulti::multi_free()
{
    // Part 1
     // need  always to alloc vectors
 if( pm.L1) delete[] pm.L1;
 if( pm.muk) delete[] pm.muk;
 if( pm.mui) delete[] pm.mui;
 if( pm.muj) delete[] pm.muj;

 if( pm.DUL ) delete[] pm.DUL;
 if( pm.DLL ) delete[] pm.DLL;
 if( pm.Vol ) delete[] pm.Vol;
 if( pm.Pparc ) delete[] pm.Pparc;
 if( pm.MM ) delete[] pm.MM;
 if( pm.Awt ) delete[] pm.Awt;
 if( pm.A ) delete[] pm.A;
 if( pm.XFs ) delete[] pm.XFs;
 if( pm.Falps ) delete[] pm.Falps;
 if( pm.G ) delete[] pm.G;
 if( pm.G0 ) delete[] pm.G0 ;
 if( pm.lnGam ) delete[] pm.lnGam;
 if( pm.lnGmo ) delete[] pm.lnGmo;
 if( pm.B ) delete[] pm.B;
 if( pm.U ) delete[] pm.U;
 if( pm.U_r ) delete[] pm.U_r;
 if( pm.C ) delete[] pm.C;
 if( pm.XF ) delete[] pm.XF;
 if( pm.YF ) delete[] pm.YF;
 if( pm.Falp ) delete[] pm.Falp;
 if( pm.X ) delete[] pm.X;
 if( pm.Y ) delete[] pm.Y;
 if( pm.XY ) delete[] pm.XY;
 if( pm.MU ) delete[] pm.MU;
 if( pm.EMU ) delete[] pm.EMU;
 if( pm.NMU ) delete[] pm.NMU;
 if( pm.W ) delete[] pm.W;
 if( pm.F ) delete[] pm.F;
 if( pm.F0 ) delete[] pm.F0;
 if( pm.YOF ) delete[] pm.YOF;

 if(   pm.SB ) delete[] pm.SB;
 if(   pm.SB1 ) delete[] pm.SB1;
 if(   pm.SFs ) delete[] pm.SFs;
 if(   pm.SM ) delete[] pm.SM;
 if(   pm.SF ) delete[] pm.SF;
 if(   pm.SM2 ) delete[] pm.SM2;
 if(   pm.SF2 ) delete[] pm.SF2;
 if(   pm.RLC ) delete[] pm.RLC;
 if(   pm.RSC ) delete[] pm.RSC;
 if(   pm.ICC ) delete[] pm.ICC;
 if(   pm.DCC ) delete[] pm.DCC;
 if(   pm.PHC ) delete[] pm.PHC;
 if(   pm.DCCW ) delete[] pm.DCCW;
 if( pm.lnGmM ) delete[] pm.lnGmM;
 if( pm.GEX ) delete[] pm.GEX;
 if( pm.FVOL ) delete[] pm.FVOL;
 if( pm.FWGT ) delete[] pm.FWGT;

   if( pm.Y_la ) delete[] pm.Y_la;
   if( pm.Y_w ) delete[] pm.Y_w;
   if( pm.Fx ) delete[] pm.Fx;
   if( pm.Wx ) delete[] pm.Wx;
   if( pm.VL ) delete[] pm.VL;
   if( pm.Gamma ) delete[] pm.Gamma;
   if( pm.lnGmf ) delete[] pm.lnGmf;
//   if( pm.D ) delete[] pm.D;

   // Part 2  not requited arrays

   if( pm.BF ) delete[] pm.BF;
if( pm.BFC ) delete[] pm.BFC;
   if( pm.XFA ) delete[] pm.XFA;
   if( pm.YFA ) delete[] pm.YFA;
   if( pm.LsMod ) delete[] pm.LsMod;
   if( pm.LsMdc ) delete[] pm.LsMdc;
   if( pm.IPx ) delete[] pm.IPx;
   if( pm.PMc ) delete[] pm.PMc;
   if( pm.DMc ) delete[] pm.DMc;
   if( pm.PUL ) delete[] pm.PUL;
   if( pm.PLL ) delete[] pm.PLL;
   if( pm.sMod ) delete[] pm.sMod;
   if( pm.RFLC ) delete[] pm.RFLC;
   if( pm.RFSC ) delete[] pm.RFSC;

   if( pm.Y_m ) delete[] pm.Y_m;
   if( pm.IC_m ) delete[] pm.IC_m;
   if( pm.IC_lm ) delete[] pm.IC_lm;
   if( pm.IC_wm ) delete[] pm.IC_wm;

   if( pm.Aalp ) delete[] pm.Aalp;
   if( pm.Xr0h0 ) delete[] pm.Xr0h0;

   if( pm.Sigw ) delete[] pm.Sigw;
   if( pm.Sigg ) delete[] pm.Sigg;

   if( pm.EZ ) delete[] pm.EZ;
   if( pm.Xcond ) delete[] pm.Xcond;
   if( pm.Xeps ) delete[] pm.Xeps;


if( pm.SATX ) delete[] pm.SATX;
   if( pm.SCM ) delete[] pm.SCM;
   if( pm.Nfsp ) delete[] pm.Nfsp;
   if( pm.MASDT ) delete[] pm.MASDT;
   if( pm.XcapA ) delete[] pm.XcapA;
   if( pm.XcapB ) delete[] pm.XcapB;
   if( pm.XcapD ) delete[] pm.XcapD;
   if( pm.XcapF ) delete[] pm.XcapF;
   if( pm.XdlA ) delete[] pm.XdlA;
   if( pm.XdlB ) delete[] pm.XdlB;
   if( pm.XdlD ) delete[] pm.XdlD;
   if( pm.XpsiA ) delete[] pm.XpsiA;
   if( pm.XpsiB ) delete[] pm.XpsiB;
   if( pm.XpsiD ) delete[] pm.XpsiD;
   if( pm.XlamA ) delete[] pm.XlamA;
   if( pm.Xetaf ) delete[] pm.Xetaf;
   if( pm.XetaA ) delete[] pm.XetaA;
   if( pm.XetaB ) delete[] pm.XetaB;
if( pm.XetaD ) delete[] pm.XetaD;
if( pm.MASDJ ) delete[] pm.MASDJ;
   if( pm.XFTS ) delete[] pm.XFTS;
if( pm.lnSAC ) delete[] pm.lnSAC;
if( pm.SATT ) delete[] pm.SATT;
if( pm.SM3 ) delete[] pm.SM3;
if( pm.DCC3 ) delete[] pm.DCC3;
if( pm.D ) delete[] pm.D;


   if( pm.Fug ) delete[] pm.Fug;
   if( pm.Fug_l ) delete[] pm.Fug_l;
   if( pm.Ppg_l ) delete[] pm.Ppg_l;

   // Part 3
   if( pm.Wb ) delete[] pm.Wb;
   if( pm.Wabs ) delete[] pm.Wabs;
   if( pm.Rion ) delete[] pm.Rion;
   if( pm.Qp ) delete[] pm.Qp;
   if( pm.Qd ) delete[] pm.Qd;

   // added SD 03/02/2009
    if( pm.XU ) delete[] pm.XU;
    if( pm.Uc ) delete[] pm.Uc;

    if(pm.H0)  	delete[] pm.H0;
    if(pm.A0)  	delete[] pm.A0;
    if(pm.U0)  	delete[] pm.U0;
    if(pm.S0)  	delete[] pm.S0;
    if(pm.Cp0) 	delete[] pm.Cp0;

    if(pm.VPh)  	delete[] pm.VPh;
    if(pm.GPh)  	delete[] pm.GPh;
    if(pm.HPh)  	delete[] pm.HPh;
    if(pm.SPh)  	delete[] pm.SPh;
    if(pm.CPh)  	delete[] pm.CPh;
    if(pm.APh)  	delete[] pm.APh;
    if(pm.UPh)  	delete[] pm.UPh;

 //  Added 16.11.2004 by Sveta
//    if( pm.sitE )     delete[] pm.sitE;
//    if( pm.sitXcat )  delete[] pm.sitXcat;
//    if( pm.sitXan )    delete[] pm.sitXan;

    // optimization 08/02/2007
    Free_TSolMod();
    Free_internal();
}

#endif

void TMulti::to_text_file( const char *path, bool append )
{
    //static values
   char PAalp;
   char PSigm;

#ifndef IPMGEMPLUGIN
   PAalp = syp->PAalp;
   PSigm = syp->PSigm;
#else
   PAalp = PAalp_;
   PSigm = PSigm_;
#endif

   ios::openmode mod = ios::out; 
    if( append ) 
     mod = ios::out|ios::app; 
  fstream ff(path, mod );
  ErrorIf( !ff.good() , path, "Fileopen error");

  if( append ) 
   ff << "\nNext record" << endl;
  ff << pm.stkey << endl;
//  TProfil::pm->pa.p.write(ff);
  
  TPrintArrays  prar(0,0,ff);

  prar.writeArray( "Short_PARAM",  &TProfil::pm->pa.p.PC, 10L );
  prar.writeArray( "Double_PARAM",  &TProfil::pm->pa.p.DG, 28L );
  prar.writeArray( "Short_Const",  &pm.N, 38L );
  prar.writeArray(  "Double_Const",  &pm.TC, 55, 20 );
  prar.writeArray(  "EpsW", pm.epsW, 5);
  prar.writeArray(  "EpsWg", pm.epsWg, 5);
  prar.writeArray(  "DenW", pm.denW, 5);
  prar.writeArray(  "DenWg", pm.denWg, 5);
  ff << endl << "Error Code " << pm.errorCode << endl;
  ff << "Error Message" << pm.errorBuf << endl;

   //dynamic values

    // Part 1

    /* need  always to alloc vectors */
  prar.writeArray(  "L1", pm.L1,  pm.FI);
  prar.writeArray(  "muk", pm.muk, pm.FI);
  prar.writeArray(  "mui", pm.mui, pm.N);
  prar.writeArray(  "muj", pm.muj,  pm.L);
  prar.writeArray(  "DUL", pm.DUL,  pm.L);
  prar.writeArray(  "DLL", pm.DLL,  pm.L);
  prar.writeArray(  "Vol", pm.Vol,  pm.L);
  prar.writeArray(  "Pparc", pm.Pparc,  pm.L);
  prar.writeArray(  "MM", pm.MM,  pm.L);
  prar.writeArray(  "Awt", pm.Awt, pm.N);
  prar.writeArray(  "A", pm.A,  pm.N*pm.L);
  prar.writeArray(  "XFs", pm.XFs, pm.FI);
  prar.writeArray(  "Falps", pm.Falps,  pm.FI);
  prar.writeArray(  "G", pm.G,  pm.L);
  prar.writeArray(  "G0", pm.G0,  pm.L);
  prar.writeArray(  "lnGam", pm.lnGam,  pm.L);
  prar.writeArray(  "lnGmo", pm.lnGmo,  pm.L);
  prar.writeArray(  "B", pm.B,  pm.N);
  prar.writeArray(  "U", pm.U,  pm.N);
  prar.writeArray(  "Uc", pm.Uc,  pm.N);
  prar.writeArray(  "U_r", pm.U_r,  pm.N);
  prar.writeArray(  "C", pm.C,  pm.N);
  prar.writeArray(  "XF", pm.XF,  pm.FI);
  prar.writeArray(  "YF", pm.YF,  pm.FI);
  prar.writeArray(  "Falp", pm.Falp,  pm.FI);
  prar.writeArray(  "X", pm.X,  pm.L);
  prar.writeArray(  "Y", pm.Y,  pm.L);
  prar.writeArray(  "XY", pm.XY,  pm.L);
  prar.writeArray(  "XU", pm.XU,  pm.L);
  prar.writeArray(  "MU", pm.MU,  pm.L);
  prar.writeArray(  "EMU", pm.EMU,  pm.L);
  prar.writeArray(  "NMU", pm.NMU,  pm.L);
  prar.writeArray(  "W", pm.W,  pm.L);
  prar.writeArray(  "F", pm.F,  pm.L);
  prar.writeArray(  "F0", pm.F0,  pm.L);
  prar.writeArray(  "YOF", pm.YOF,  pm.FI);


  prar.writeArray(  "lnGmM", pm.lnGmM,  pm.L);
  prar.writeArray(  "GEX", pm.GEX,  pm.L);
  prar.writeArray(  "FVOL", pm.FVOL,  pm.FI);
  prar.writeArray(  "FWGT", pm.FWGT,  pm.FI);

    if( pm.L > 0 )
    {
     prar.writeArray(  "Y_la", pm.Y_la,  pm.L);
     prar.writeArray(  "Y_w", pm.Y_w,  pm.L);
     prar.writeArray(  "Fx", pm.Fx,  pm.L);
     prar.writeArray(  "Wx", pm.Wx,  pm.L);
     prar.writeArray(  "VL", pm.VL, pm.L);
     prar.writeArray(  "Gamma", pm.Gamma,  pm.L);
     prar.writeArray(  "lnGmf", pm.lnGmf,  pm.L);
//     prar.writeArray(  "D", pm.D,  pm.L);
    }

   // Part 2  not always required arrays
    if( pm.FIs > 0 && pm.Ls > 0 )
    {
     prar.writeArray(  "BF", pm.BF,  pm.FIs*pm.N);
     prar.writeArray(  "BFC", pm.BFC, pm.N);
     prar.writeArray(  "XFA", pm.XFA,  pm.FIs);
     prar.writeArray(  "YFA", pm.YFA,  pm.FIs);
     prar.writeArray(  "LsMod", pm.LsMod, pm.FIs*3);
     prar.writeArray(  "LsMdc", pm.LsMdc, pm.FIs);
     long int LsModSum;
     long int LsIPxSum;
     long int LsMdcSum;
     getLsModsum( LsModSum, LsIPxSum );
     getLsMdcsum( LsMdcSum );
     prar.writeArray(  "IPxPH", pm.IPx,  LsIPxSum);
     prar.writeArray(  "PMc", pm.PMc,  LsModSum);
     prar.writeArray(  "DMc", pm.DMc,  LsMdcSum);

     prar.writeArray(  "PUL", pm.PUL,  pm.FIs);
     prar.writeArray(  "PLL", pm.PLL,  pm.FIs);

    }

    if( pm.LO > 1 )
    {
     prar.writeArray(  "Y_m", pm.Y_m,  pm.L);
     prar.writeArray(  "IC_m", pm.IC_m,  pm.N);
     prar.writeArray(  "IC_lm", pm.IC_lm,  pm.N);
     prar.writeArray(  "IC_wm", pm.IC_wm,  pm.N);
    }

    // dispersed and sorption phases
    if( PAalp != S_OFF )
    {
     prar.writeArray(  "Aalp", pm.Aalp, pm.FI);
     prar.writeArray(  "Xr0h0", &pm.Xr0h0[0][0],  pm.FI*2);
    }

   if( PSigm != S_OFF )
     prar.writeArray(  "Sigw", pm.Sigw,  pm.FI);

    if( PSigm != S_OFF )
     prar.writeArray(  "Sigg", pm.Sigg,  pm.FI);

    if( pm.E )
    {
     prar.writeArray(  "EZ", pm.EZ,  pm.L);
     prar.writeArray(  "Xcond", pm.Xcond,  pm.FI);
     prar.writeArray(  "Xeps", pm.Xeps,  pm.FI);
    }

    if( pm.FIat > 0 && /*pm.Lads > 0 &&Sveta 12/09/99*/ pm.FIs > 0 )
    { /* ADSORPTION AND ION EXCHANGE */
     prar.writeArray(  "Nfsp", &pm.Nfsp[0][0], pm.FIs*pm.FIat);
     prar.writeArray(  "MASDT", &pm.MASDT[0][0], pm.FIs*pm.FIat);
     prar.writeArray(  "XcapA", &pm.XcapA[0][0], pm.FIs*pm.FIat);
     prar.writeArray(  "XcapB", &pm.XcapB[0][0], pm.FIs*pm.FIat);
     prar.writeArray(  "XcapD", &pm.XcapD[0][0], pm.FIs*pm.FIat);
     prar.writeArray(  "XcapF", &pm.XcapF[0][0], pm.FIs*pm.FIat);
     prar.writeArray(  "XdlA", &pm.XdlA[0][0], pm.FIs*pm.FIat);
     prar.writeArray(  "XdlB", &pm.XdlB[0][0], pm.FIs*pm.FIat);
     prar.writeArray(  "XdlD", &pm.XdlD[0][0], pm.FIs*pm.FIat);
     prar.writeArray(  "XpsiA", &pm.XpsiA[0][0],  pm.FIs*pm.FIat);
     prar.writeArray(  "XpsiB", &pm.XpsiB[0][0],  pm.FIs*pm.FIat);
     prar.writeArray(  "XpsiD", &pm.XpsiD[0][0],  pm.FIs*pm.FIat);
     prar.writeArray(  "XlamA", &pm.XlamA[0][0], pm.FIs*pm.FIat);
     prar.writeArray(  "Xetaf", &pm.Xetaf[0][0], pm.FIs*pm.FIat);
     prar.writeArray(  "XetaA", &pm.XetaA[0][0],  pm.FIs*pm.FIat);
     prar.writeArray(  "XetaB", &pm.XetaB[0][0],  pm.FIs*pm.FIat);
     prar.writeArray(  "XetaD", &pm.XetaD[0][0],  pm.FIs*pm.FIat);
     prar.writeArray(  "XFTS", &pm.XFTS[0][0],  pm.FIs*pm.FIat);

     prar.writeArray(  "SATX", &pm.SATX[0][0], pm.Lads*4);
//     prar.writeArray(  "MASDJ", pm.MASDJ, pm.Ls);
     prar.writeArray(  "MASDJ", &pm.MASDJ[0][0], pm.Lads*DFCN);
     prar.writeArray(  "lnSAC", &pm.lnSAC[0][0],  pm.Lads*4);
     prar.writeArray(  "D", &pm.D[0][0], MST*MST);
    }

    if( pm.PG > 0 )
    {
     prar.writeArray(  "Fug", pm.Fug, pm.PG);
     prar.writeArray(  "Fug_l", pm.Fug_l, pm.PG);
     prar.writeArray(  "Ppg_l", pm.Ppg_l, pm.PG);
    }

    // Part 3

    if( pm.Ls > 1 && pm.FIs > 0 )
    {
     prar.writeArray(  "Wb", pm.Wb, pm.Ls);
     prar.writeArray(  "Wabs", pm.Wabs, pm.Ls);
     prar.writeArray(  "Rion", pm.Rion, pm.Ls);

     prar.writeArray(  "Qp", pm.Qp,  pm.FIs*QPSIZE);
     prar.writeArray(  "Qd", pm.Qd,  pm.FIs*QDSIZE);

    }

    if(pm.H0)
    	prar.writeArray("H0",pm.H0, pm.L);
    if(pm.A0)
    	prar.writeArray("A0",pm.A0, pm.L);
    if(pm.U0)
    	prar.writeArray("U0",pm.U0, pm.L);
    if(pm.S0)
    	prar.writeArray("S0",pm.S0, pm.L);
    if(pm.Cp0)
    	prar.writeArray("Cp0",pm.Cp0, pm.L);

    prar.writeArray(  "VPh", &pm.VPh[0][0], pm.FIs*MIXPHPROPS);
    prar.writeArray(  "GPh", &pm.GPh[0][0], pm.FIs*MIXPHPROPS);
    prar.writeArray(  "HPh", &pm.HPh[0][0], pm.FIs*MIXPHPROPS);
    prar.writeArray(  "SPh", &pm.SPh[0][0], pm.FIs*MIXPHPROPS);
    prar.writeArray(  "CPh", &pm.CPh[0][0], pm.FIs*MIXPHPROPS);
    prar.writeArray(  "APh", &pm.APh[0][0], pm.FIs*MIXPHPROPS);
    prar.writeArray(  "UPh", &pm.UPh[0][0], pm.FIs*MIXPHPROPS);

//  Added 16.11.2004 by Sveta
//   if( pm.sitNcat*pm.sitNcat )
//    prar.writeArray(  "sitE", pm.sitE, pm.sitNcat*pm.sitNan );
//   if( pm.sitNcat )
//    prar.writeArray(  "sitXcat", pm.sitXcat, pm.sitNcat );
//   if( pm.sitNan )
//     prar.writeArray(  "sitXan", pm.sitXan, pm.sitNan );
}

//--------------------- End of ms_multi_file.cpp ---------------------------


