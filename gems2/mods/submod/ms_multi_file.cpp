
#include "m_param.h"

// writing MULTI to binary file
void TMulti::to_file( fstream& ff )
{
   if( pm.N < 2 || pm.L < 2 || pm.FI < 1 )
        Error( GetName(), "pm.N < 2 || pm.L < 2 || pm.FI < 1" );

   //static values
   char PAalp;
   char PSigm;
   float EpsW;
   float RoW;

#ifndef IPMGEMPLUGIN
   PAalp = syp->PAalp;
   PSigm = syp->PSigm;
   EpsW = TProfil::pm->tpp->EpsW;
   RoW = TProfil::pm->tpp->RoW;
#else
   PAalp = PAalp_;
   PSigm = PSigm_;
   EpsW = EpsW_;
   RoW = RoW_;
#endif


   ff.write (pm.stkey, sizeof(char)*(EQ_RKLEN+5));
   ff.write ((char*)&pm.N, sizeof(short)*36);
   ff.write ((char*)&pm.TC, sizeof(double)*55);
   ff.write (&PAalp, sizeof(char));
   ff.write (&PSigm, sizeof(char));
   ff.write ((char*)&EpsW, sizeof(float));
   ff.write ((char*)&RoW, sizeof(float));

   //dynamic values

    // Part 1

    /* need  always to alloc vectors */
   ff.write ((char*)pm.L1,  sizeof(short)*pm.FI);
   ff.write ((char*)pm.muk, sizeof(short)*pm.FI);
   ff.write ((char*)pm.mui, sizeof(short)*pm.N);
   ff.write ((char*)pm.muj, sizeof(short)*pm.L);
   ff.write ((char*)pm.DUL, sizeof(double)*pm.L);
   ff.write ((char*)pm.DLL, sizeof(double)*pm.L);
   ff.write ((char*)pm.Vol, sizeof(double)*pm.L);
   ff.write ((char*)pm.Pparc, sizeof(double)*pm.L);
   ff.write ((char*)pm.MM, sizeof(double)*pm.L);
   ff.write ((char*)pm.Awt, sizeof(float)*pm.N);
   ff.write ((char*)pm.A, sizeof(float)*pm.N*pm.L);
   ff.write ((char*)pm.XFs, sizeof(float)*pm.FI);
   ff.write ((char*)pm.Falps, sizeof(float)*pm.FI);
   ff.write ((char*)pm.G, sizeof(double)*pm.L);
   ff.write ((char*)pm.G0, sizeof(double)*pm.L);
   ff.write ((char*)pm.lnGam, sizeof(double)*pm.L);
   ff.write ((char*)pm.lnGmo, sizeof(double)*pm.L);
   ff.write ((char*)pm.B, sizeof(double)*pm.N);
   ff.write ((char*)pm.U, sizeof(double)*pm.N);
   ff.write ((char*)pm.U_r, sizeof(double)*pm.N);
   ff.write ((char*)pm.C, sizeof(double)*pm.N);
   ff.write ((char*)pm.XF, sizeof(double)*pm.FI);
   ff.write ((char*)pm.YF, sizeof(double)*pm.FI);
   ff.write ((char*)pm.Falp, sizeof(double)*pm.FI);
   ff.write ((char*)pm.X, sizeof(double)*pm.L);
   ff.write ((char*)pm.Y, sizeof(double)*pm.L);
   ff.write ((char*)pm.XY, sizeof(double)*pm.L);
   ff.write ((char*)pm.MU, sizeof(double)*pm.L);
   ff.write ((char*)pm.EMU, sizeof(double)*pm.L);
   ff.write ((char*)pm.NMU, sizeof(double)*pm.L);
   ff.write ((char*)pm.W, sizeof(double)*pm.L);
   ff.write ((char*)pm.F, sizeof(double)*pm.L);
   ff.write ((char*)pm.F0, sizeof(double)*pm.L);
   ff.write ((char*)pm.YOF, sizeof(double)*pm.FI);

   ff.write ((char*)pm.SB, sizeof(char)*(MAXICNAME+MAXSYMB)*pm.N);
   ff.write ((char*)pm.SB1, sizeof(char)* MAXICNAME * pm.N);
   ff.write ((char*)pm.SFs, sizeof(char)*(MAXPHNAME+MAXSYMB)*pm.FI);
   ff.write ((char*)pm.SM, sizeof(char)* MAXDCNAME * pm.L);
   ff.write ((char*)pm.SF, sizeof(char)*(MAXPHNAME+MAXSYMB)*pm.FI);
   ff.write ((char*)pm.SM2, sizeof(char)* MAXDCNAME * pm.Ls);
   ff.write ((char*)pm.SF2, sizeof(char)*(MAXPHNAME+MAXSYMB)*pm.FIs);

   ff.write ( pm.RLC, sizeof(char)  * pm.L);
   ff.write ( pm.RSC, sizeof(char)  * pm.L);
   ff.write ( pm.ICC, sizeof(char)  * pm.N);
   ff.write ( pm.DCC, sizeof(char)  * pm.L);
   ff.write ( pm.PHC, sizeof(char)  * pm.FI);
   ff.write ( pm.DCCW, sizeof(char)  * pm.L);

   ff.write ((char*)pm.lnGmM, sizeof(double)*pm.L);
   ff.write ((char*)pm.GEX, sizeof(double)*pm.L);
   ff.write ((char*)pm.FVOL, sizeof(double)*pm.FI);
   ff.write ((char*)pm.FWGT, sizeof(double)*pm.FI);

    if( pm.L > 0 )
    {
      ff.write ((char*)pm.Y_la, sizeof(double)*pm.L);
      ff.write ((char*)pm.Y_w, sizeof(double)*pm.L);
      ff.write ((char*)pm.Fx, sizeof(double)*pm.L);
      ff.write ((char*)pm.Wx, sizeof(double)*pm.L);
      ff.write ((char*)pm.VL, sizeof(float)*pm.L);
      ff.write ((char*)pm.Gamma, sizeof(double)*pm.L);
      ff.write ((char*)pm.lnGmf, sizeof(double)*pm.L);
      ff.write ((char*)pm.D, sizeof(double)*pm.L);
    }

   // Part 2  not requited arrays
    if( pm.FIs > 0 && pm.Ls > 0 )
    {
      ff.write ((char*)pm.BF, sizeof(double)*pm.FIs*pm.N);
      ff.write ((char*)pm.XFA, sizeof(double)*pm.FIs);
      ff.write ((char*)pm.YFA, sizeof(double)*pm.FIs);
      ff.write ((char*)pm.LsMod, sizeof(short)*pm.FIs);
      ff.write ((char*)pm.LsMdc, sizeof(short)*pm.FIs);
      ff.write ((char*)pm.PMc, sizeof(float)*pm.FIs);
      ff.write ((char*)pm.DMc, sizeof(float)*pm.Ls);
      ff.write ((char*)pm.PUL, sizeof(double)*pm.FIs);
      ff.write ((char*)pm.PLL, sizeof(double)*pm.FIs);

      ff.write ((char*)pm.sMod, sizeof(char)*6*pm.FIs);
      ff.write ( pm.RFLC, sizeof(char)* pm.FIs);
      ff.write ( pm.RFSC, sizeof(char)* pm.FIs);
    }

    if( pm.LO > 1 )
    {
      ff.write ((char*)pm.Y_m, sizeof(double)*pm.L);
      ff.write ((char*)pm.IC_m, sizeof(double)*pm.N);
      ff.write ((char*)pm.IC_lm, sizeof(double)*pm.N);
      ff.write ((char*)pm.IC_wm, sizeof(double)*pm.N);
    }

    /* dispersion and sorbtion phases */
    if( PAalp != S_OFF )
    {
      ff.write ((char*)pm.Aalp, sizeof(float)*pm.FI);
      ff.write ((char*)pm.Xr0h0, sizeof(float)*pm.FI*2);
    }

   if( PSigm != S_OFF )
      ff.write ((char*)pm.Sigw, sizeof(float)*pm.FI);

    if( PSigm != S_OFF )
      ff.write ((char*)pm.Sigg, sizeof(float)*pm.FI);

    if( pm.E )
    {
      ff.write ((char*)pm.EZ, sizeof(double)*pm.L);
      ff.write ((char*)pm.Xcond, sizeof(float)*pm.FI);
      ff.write ((char*)pm.Xeps, sizeof(float)*pm.FI);
    }

    if( pm.FIat > 0 && /*pm.Lads > 0 &&Sveta 12/09/99*/ pm.FIs > 0 )
    { /* ADSORBTION AND ION IXCHANDG */
      ff.write ((char*)pm.SATNdx, sizeof(short)*2*pm.Ls);
      ff.write ((char*)pm.SCM, sizeof(char)*pm.FIs*pm.FIat);

      ff.write ((char*)pm.Nfsp, sizeof(float)*pm.FIs*pm.FIat);
      ff.write ((char*)pm.MASDT, sizeof(float)*pm.FIs*pm.FIat);
      ff.write ((char*)pm.XcapA, sizeof(float)*pm.FIs*pm.FIat);
      ff.write ((char*)pm.XcapB, sizeof(float)*pm.FIs*pm.FIat);
      ff.write ((char*)pm.XcapD, sizeof(float)*pm.FIs*pm.FIat);
      ff.write ((char*)pm.XcapF, sizeof(float)*pm.FIs*pm.FIat);
      ff.write ((char*)pm.XdlA, sizeof(float)*pm.FIs*pm.FIat);
      ff.write ((char*)pm.XdlB, sizeof(float)*pm.FIs*pm.FIat);
      ff.write ((char*)pm.XdlD, sizeof(float)*pm.FIs*pm.FIat);
      ff.write ((char*)pm.XpsiA, sizeof(double)*pm.FIs*pm.FIat);
      ff.write ((char*)pm.XpsiB, sizeof(double)*pm.FIs*pm.FIat);
      ff.write ((char*)pm.XpsiD, sizeof(double)*pm.FIs*pm.FIat);
      ff.write ((char*)pm.XlamA, sizeof(float)*pm.FIs*pm.FIat);
      ff.write ((char*)pm.Xetaf, sizeof(float)*pm.FIs*pm.FIat);
      ff.write ((char*)pm.XetaA, sizeof(double)*pm.FIs*pm.FIat);
      ff.write ((char*)pm.XetaB, sizeof(double)*pm.FIs*pm.FIat);
      ff.write ((char*)pm.XFTS, sizeof(double)*pm.FIs*pm.FIat);

      ff.write (pm.SATT, sizeof(char)*pm.Ls);
      ff.write ((char*)pm.MASDJ, sizeof(float)*pm.Ls);
      ff.write ((char*)pm.lnSAT, sizeof(double)*pm.Ls);
    }

    if( pm.PG > 0 )
    {
      ff.write ((char*)pm.Fug, sizeof(float)*pm.PG);
      ff.write ((char*)pm.Fug_l, sizeof(float)*pm.PG);
      ff.write ((char*)pm.Ppg_l, sizeof(float)*pm.PG);
    }

    // Part 3

    if( pm.Ls > 1 && pm.FIs > 0 )
    {
      ff.write ((char*)pm.Wb, sizeof(float)*pm.Ls);
      ff.write ((char*)pm.Wabs, sizeof(float)*pm.Ls);
      ff.write ((char*)pm.Rion, sizeof(float)*pm.Ls);

      ff.write ((char*)pm.Qp, sizeof(double)*pm.FIs*20);
      ff.write ((char*)pm.Qd, sizeof(double)*20);

    }
}

// reading MULTI from binary file
void TMulti::from_file( fstream& ff )
{
   //static values
   char PAalp;
   char PSigm;
   float EpsW;
   float RoW;

   ff.read (pm.stkey, sizeof(char)*(EQ_RKLEN+5));
   ff.read ((char*)&pm.N, sizeof(short)*36);
   ff.read ((char*)&pm.TC, sizeof(double)*55);
   ff.read (&PAalp, sizeof(char));
   ff.read (&PSigm, sizeof(char));
   ff.read ((char*)&EpsW, sizeof(float));
   ff.read ((char*)&RoW, sizeof(float));


#ifndef IPMGEMPLUGIN
//   syp->PAalp = PAalp;
//   syp->PSigm = PSigm;
#else
   PAalp_ = PAalp;
   PSigm_ = PSigm;
   EpsW_ = EpsW;
   RoW_ =  RoW;
#endif

   //realloc memory
#ifdef IPMGEMPLUGIN
   multi_realloc( PAalp, PSigm );
#endif

   //dynamic values

    // Part 1

    /* need  always to alloc vectors */
   ff.read ((char*)pm.L1,  sizeof(short)*pm.FI);
   ff.read ((char*)pm.muk, sizeof(short)*pm.FI);
   ff.read ((char*)pm.mui, sizeof(short)*pm.N);
   ff.read ((char*)pm.muj, sizeof(short)*pm.L);
   ff.read ((char*)pm.DUL, sizeof(double)*pm.L);
   ff.read ((char*)pm.DLL, sizeof(double)*pm.L);
   ff.read ((char*)pm.Vol, sizeof(double)*pm.L);
   ff.read ((char*)pm.Pparc, sizeof(double)*pm.L);
   ff.read ((char*)pm.MM, sizeof(double)*pm.L);
   ff.read ((char*)pm.Awt, sizeof(float)*pm.N);
   ff.read ((char*)pm.A, sizeof(float)*pm.N*pm.L);
   ff.read ((char*)pm.XFs, sizeof(float)*pm.FI);
   ff.read ((char*)pm.Falps, sizeof(float)*pm.FI);
   ff.read ((char*)pm.G, sizeof(double)*pm.L);
   ff.read ((char*)pm.G0, sizeof(double)*pm.L);
   ff.read ((char*)pm.lnGam, sizeof(double)*pm.L);
   ff.read ((char*)pm.lnGmo, sizeof(double)*pm.L);
   ff.read ((char*)pm.B, sizeof(double)*pm.N);
   ff.read ((char*)pm.U, sizeof(double)*pm.N);
   ff.read ((char*)pm.U_r, sizeof(double)*pm.N);
   ff.read ((char*)pm.C, sizeof(double)*pm.N);
   ff.read ((char*)pm.XF, sizeof(double)*pm.FI);
   ff.read ((char*)pm.YF, sizeof(double)*pm.FI);
   ff.read ((char*)pm.Falp, sizeof(double)*pm.FI);
   ff.read ((char*)pm.X, sizeof(double)*pm.L);
   ff.read ((char*)pm.Y, sizeof(double)*pm.L);
   ff.read ((char*)pm.XY, sizeof(double)*pm.L);
   ff.read ((char*)pm.MU, sizeof(double)*pm.L);
   ff.read ((char*)pm.EMU, sizeof(double)*pm.L);
   ff.read ((char*)pm.NMU, sizeof(double)*pm.L);
   ff.read ((char*)pm.W, sizeof(double)*pm.L);
   ff.read ((char*)pm.F, sizeof(double)*pm.L);
   ff.read ((char*)pm.F0, sizeof(double)*pm.L);
   ff.read ((char*)pm.YOF, sizeof(double)*pm.FI);

   ff.read ((char*)pm.SB, sizeof(char)*(MAXICNAME+MAXSYMB)*pm.N);
   ff.read ((char*)pm.SB1, sizeof(char)* MAXICNAME * pm.N);
   ff.read ((char*)pm.SFs, sizeof(char)*(MAXPHNAME+MAXSYMB)*pm.FI);
   ff.read ((char*)pm.SM, sizeof(char)* MAXDCNAME * pm.L);
   ff.read ((char*)pm.SF, sizeof(char)*(MAXPHNAME+MAXSYMB)*pm.FI);
   ff.read ((char*)pm.SM2, sizeof(char)* MAXDCNAME * pm.Ls);
   ff.read ((char*)pm.SF2, sizeof(char)*(MAXPHNAME+MAXSYMB)*pm.FIs);

   ff.read ( pm.RLC, sizeof(char)  * pm.L);
   ff.read ( pm.RSC, sizeof(char)  * pm.L);
   ff.read ( pm.ICC, sizeof(char)  * pm.N);
   ff.read ( pm.DCC, sizeof(char)  * pm.L);
   ff.read ( pm.PHC, sizeof(char)  * pm.FI);
   ff.read ( pm.DCCW, sizeof(char)  * pm.L);

   ff.read ((char*)pm.lnGmM, sizeof(double)*pm.L);
   ff.read ((char*)pm.GEX, sizeof(double)*pm.L);
   ff.read ((char*)pm.FVOL, sizeof(double)*pm.FI);
   ff.read ((char*)pm.FWGT, sizeof(double)*pm.FI);

    if( pm.L > 0 )
    {
      ff.read ((char*)pm.Y_la, sizeof(double)*pm.L);
      ff.read ((char*)pm.Y_w, sizeof(double)*pm.L);
      ff.read ((char*)pm.Fx, sizeof(double)*pm.L);
      ff.read ((char*)pm.Wx, sizeof(double)*pm.L);
      ff.read ((char*)pm.VL, sizeof(float)*pm.L);
      ff.read ((char*)pm.Gamma, sizeof(double)*pm.L);
      ff.read ((char*)pm.lnGmf, sizeof(double)*pm.L);
      ff.read ((char*)pm.D, sizeof(double)*pm.L);
    }

   // Part 2  not requited arrays
    if( pm.FIs > 0 && pm.Ls > 0 )
    {
      ff.read ((char*)pm.BF, sizeof(double)*pm.FIs*pm.N);
      ff.read ((char*)pm.XFA, sizeof(double)*pm.FIs);
      ff.read ((char*)pm.YFA, sizeof(double)*pm.FIs);
      ff.read ((char*)pm.LsMod, sizeof(short)*pm.FIs);
      ff.read ((char*)pm.LsMdc, sizeof(short)*pm.FIs);
      ff.read ((char*)pm.PMc, sizeof(float)*pm.FIs);
      ff.read ((char*)pm.DMc, sizeof(float)*pm.Ls);
      ff.read ((char*)pm.PUL, sizeof(double)*pm.FIs);
      ff.read ((char*)pm.PLL, sizeof(double)*pm.FIs);

      ff.read ((char*)pm.sMod, sizeof(char)*6*pm.FIs);
      ff.read ( pm.RFLC, sizeof(char)* pm.FIs);
      ff.read ( pm.RFSC, sizeof(char)* pm.FIs);
    }

    if( pm.LO > 1 )
    {
      ff.read ((char*)pm.Y_m, sizeof(double)*pm.L);
      ff.read ((char*)pm.IC_m, sizeof(double)*pm.N);
      ff.read ((char*)pm.IC_lm, sizeof(double)*pm.N);
      ff.read ((char*)pm.IC_wm, sizeof(double)*pm.N);
    }

    /* dispersion and sorbtion phases */
    if( PAalp != S_OFF )
    {
      ff.read ((char*)pm.Aalp, sizeof(float)*pm.FI);
      ff.read ((char*)pm.Xr0h0, sizeof(float)*pm.FI*2);
    }

   if( PSigm != S_OFF )
      ff.read ((char*)pm.Sigw, sizeof(float)*pm.FI);

    if( PSigm != S_OFF )
      ff.read ((char*)pm.Sigg, sizeof(float)*pm.FI);

    if( pm.E )
    {
      ff.read ((char*)pm.EZ, sizeof(double)*pm.L);
      ff.read ((char*)pm.Xcond, sizeof(float)*pm.FI);
      ff.read ((char*)pm.Xeps, sizeof(float)*pm.FI);
    }

    if( pm.FIat > 0 && /*pm.Lads > 0 &&Sveta 12/09/99*/ pm.FIs > 0 )
    { /* ADSORBTION AND ION IXCHANDG */
      ff.read ((char*)pm.SATNdx, sizeof(short)*2*pm.Ls);
      ff.read ((char*)pm.SCM, sizeof(char)*pm.FIs*pm.FIat);

      ff.read ((char*)pm.Nfsp, sizeof(float)*pm.FIs*pm.FIat);
      ff.read ((char*)pm.MASDT, sizeof(float)*pm.FIs*pm.FIat);
      ff.read ((char*)pm.XcapA, sizeof(float)*pm.FIs*pm.FIat);
      ff.read ((char*)pm.XcapB, sizeof(float)*pm.FIs*pm.FIat);
      ff.read ((char*)pm.XcapD, sizeof(float)*pm.FIs*pm.FIat);
      ff.read ((char*)pm.XcapF, sizeof(float)*pm.FIs*pm.FIat);
      ff.read ((char*)pm.XdlA, sizeof(float)*pm.FIs*pm.FIat);
      ff.read ((char*)pm.XdlB, sizeof(float)*pm.FIs*pm.FIat);
      ff.read ((char*)pm.XdlD, sizeof(float)*pm.FIs*pm.FIat);
      ff.read ((char*)pm.XpsiA, sizeof(double)*pm.FIs*pm.FIat);
      ff.read ((char*)pm.XpsiB, sizeof(double)*pm.FIs*pm.FIat);
      ff.read ((char*)pm.XpsiD, sizeof(double)*pm.FIs*pm.FIat);
      ff.read ((char*)pm.XlamA, sizeof(float)*pm.FIs*pm.FIat);
      ff.read ((char*)pm.Xetaf, sizeof(float)*pm.FIs*pm.FIat);
      ff.read ((char*)pm.XetaA, sizeof(double)*pm.FIs*pm.FIat);
      ff.read ((char*)pm.XetaB, sizeof(double)*pm.FIs*pm.FIat);
      ff.read ((char*)pm.XFTS, sizeof(double)*pm.FIs*pm.FIat);

      ff.read (pm.SATT, sizeof(char)*pm.Ls);
      ff.read ((char*)pm.MASDJ, sizeof(float)*pm.Ls);
      ff.read ((char*)pm.lnSAT, sizeof(double)*pm.Ls);
    }

    if( pm.PG > 0 )
    {
      ff.read ((char*)pm.Fug, sizeof(float)*pm.PG);
      ff.read ((char*)pm.Fug_l, sizeof(float)*pm.PG);
      ff.read ((char*)pm.Ppg_l, sizeof(float)*pm.PG);
    }

    // Part 3

    if( pm.Ls > 1 && pm.FIs > 0 )
    {
      ff.read ((char*)pm.Wb, sizeof(float)*pm.Ls);
      ff.read ((char*)pm.Wabs, sizeof(float)*pm.Ls);
      ff.read ((char*)pm.Rion, sizeof(float)*pm.Ls);

      ff.read ((char*)pm.Qp, sizeof(double)*pm.FIs*20);
      ff.read ((char*)pm.Qd, sizeof(double)*20);

    }

}

#ifdef IPMGEMPLUGIN

// realloc dynamic memory
void TMulti::multi_realloc( char PAalp, char PSigm )
{
   if( pm.N < 2 || pm.L < 2 || pm.FI < 1 )
        Error( GetName(), "pm.N < 2 || pm.L < 2 || pm.FI < 1" );

    // Part 1
     // need  always to alloc vectors
 pm.L1 = new short[pm.FI];
 pm.muk = new short[pm.FI];
 pm.mui = new short[pm.N];
 pm.muj = new short[pm.L];

 pm.DUL = new double[pm.L];
 pm.DLL = new double[pm.L];
 pm.Vol = new double[pm.L];
 pm.Pparc = new double[pm.L];
 pm.MM = new double[pm.L];
 pm.Awt = new float[pm.N];
 pm.A = new float[pm.N*pm.L];
 pm.XFs = new float[pm.FI];
 pm.Falps = new float[pm.FI];
 pm.G = new double[pm.L];
 pm.G0 = new double[pm.L];
 pm.lnGam = new double[pm.L];
 pm.lnGmo = new double[pm.L];
 pm.B = new double[pm.N];
 pm.U = new double[pm.N];
 pm.U_r = new double[pm.N];
 pm.C = new double[pm.N];
 pm.XF = new double[pm.FI];
 pm.YF = new double[pm.FI];
 pm.Falp = new double[pm.FI];
 pm.X = new double[pm.L];
 pm.Y = new double[pm.L];
 pm.XY = new double[pm.L];
 pm.MU = new double[pm.L];
 pm.EMU = new double[pm.L];
 pm.NMU = new double[pm.L];
 pm.W = new double[pm.L];
 pm.F = new double[pm.L];
 pm.F0 = new double[pm.L];
 pm.YOF = new double[pm.FI];

    pm.SB = new char[pm.N][MAXICNAME+MAXSYMB];
    pm.SB1 = new char[pm.N][MAXICNAME];
    pm.SFs = new char[pm.FI][MAXPHNAME+MAXSYMB];
    pm.SM = new char[pm.L][MAXDCNAME];
    pm.SF = new char[pm.FI][MAXPHNAME+MAXSYMB];
    pm.SM2 = new char[pm.Ls][MAXDCNAME];
    pm.SF2 = new char[pm.FIs][MAXPHNAME+MAXSYMB];
    pm.RLC = new char[pm.L];
    pm.RSC = new char[pm.L];
    pm.ICC = new char[pm.N];
    pm.DCC = new char[pm.L];
    pm.PHC = new char[pm.FI];
    pm.DCCW = new char[pm.L];

 pm.lnGmM = new double[pm.L];
 pm.GEX = new double[pm.L];
 pm.FVOL = new double[pm.FI];
 pm.FWGT = new double[pm.FI];

 if( pm.L > 0 )
 {
   pm.Y_la = new double[pm.L];
   pm.Y_w = new double[pm.L];
   pm.Fx = new double[pm.L];
   pm.Wx = new double[pm.L];
   pm.VL = new float[pm.L];
   pm.Gamma = new double[pm.L];
   pm.lnGmf = new double[pm.L];
   pm.D = new double[pm.L];
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
   pm.D = 0;
 }

   // Part 2  not requited arrays

 if( pm.FIs > 0 && pm.Ls > 0 )
 {
   pm.BF = new double[pm.FIs*pm.N];
   pm.XFA = new double[pm.FIs];
   pm.YFA = new double[pm.FIs];
   pm.LsMod = new short[pm.FIs];
   pm.LsMdc = new short[pm.FIs];
   pm.PMc = new float[pm.FIs];
   pm.DMc = new float[pm.Ls];
   pm.PUL = new double[pm.FIs];
   pm.PLL = new double[pm.FIs];

   pm.sMod = new char[pm.FIs][6];
   pm.RFLC = new char[pm.FIs];
   pm.RFSC = new char[pm.FIs];
 }
 else
 {
   pm.BF = 0;
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
   pm.IC_m = new double[pm.N];
   pm.IC_lm = new double[pm.N];
   pm.IC_wm = new double[pm.N];
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
   pm.Aalp = new float[pm.FI];
   pm.Xr0h0 = new float[pm.FI][2];
 }
 else
 {
   pm.Aalp = 0;
   pm.Xr0h0 = 0;
 }

 if( PSigm != S_OFF )
 {    pm.Sigw = new float[pm.FI];
     pm.Sigg = new float[pm.FI];
 }
 else
 {   pm.Sigw = 0;
     pm.Sigg = 0;
 }

 if( pm.E )
 {
    pm.EZ = new double[pm.L];
    pm.Xcond = new float[pm.FI];
    pm.Xeps = new float[pm.FI];
 }
 else
 {
    pm.EZ = 0;
    pm.Xcond = 0;
    pm.Xeps = 0;
 }

 if( pm.FIat > 0 /*&& pm.Lads > 0*/ && pm.FIs > 0 )
 { // ADSORBTION AND ION IXCHANDG
   pm.SATNdx = new short[pm.Ls][2];
   pm.SCM  = new char[pm.FIs][6];

    pm.Nfsp = new float[pm.FIs][6];
    pm.MASDT = new float[pm.FIs][6];
    pm.XcapA = new float[pm.FIs][6];
    pm.XcapB = new float[pm.FIs][6];
    pm.XcapD = new float[pm.FIs][6];
    pm.XcapF = new float[pm.FIs][6];
    pm.XdlA = new float[pm.FIs][6];
    pm.XdlB = new float[pm.FIs][6];
    pm.XdlD = new float[pm.FIs][6];
    pm.XpsiA = new double[pm.FIs][6];
    pm.XpsiB = new double[pm.FIs][6];
    pm.XpsiD = new double[pm.FIs][6];
    pm.XlamA = new float[pm.FIs][6];
    pm.Xetaf = new float[pm.FIs][6];
    pm.XetaA = new double[pm.FIs][6];
    pm.XetaB = new double[pm.FIs][6];
    pm.MASDJ = new float[pm.Ls];
    pm.XFTS = new double[pm.FIs][6];
    pm.lnSAT = new double[pm.Ls];
    pm.SATT = new char[pm.Ls];
 }
else
 { // ADSORBTION AND ION IXCHANDG
   pm.SATNdx = 0;
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
    pm.MASDJ = 0;
    pm.XFTS = 0;
    pm.lnSAT = 0;
    pm.SATT = 0;
 }

 if( pm.PG > 0 ) 
 {
  pm.Fug = new float[pm.PG];
  pm.Fug_l = new float[pm.PG];
  pm.Ppg_l = new float[pm.PG];
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
    pm.Wb = new float[pm.Ls];
    pm.Wabs = new float[pm.Ls];
    pm.Rion = new float[pm.Ls];
    pm.Qp = new double[pm.FIs*20];
    pm.Qd = new double[20];

 }
 else
 {
    pm.Wb = 0;
    pm.Wabs = 0;
    pm.Rion = 0;
    pm.Qp = 0;
    pm.Qd = 0;

 }
}

// realloc dynamic memory
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
   if( pm.D ) delete[] pm.D;

   // Part 2  not requited arrays

   if( pm.BF ) delete[] pm.BF;
   if( pm.XFA ) delete[] pm.XFA;
   if( pm.YFA ) delete[] pm.YFA;
   if( pm.LsMod ) delete[] pm.LsMod;
   if( pm.LsMdc ) delete[] pm.LsMdc;
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


   if( pm.SATNdx ) delete[] pm.SATNdx;
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
   if( pm.MASDJ ) delete[] pm.MASDJ;
   if( pm.XFTS ) delete[] pm.XFTS;
   if( pm.lnSAT ) delete[] pm.lnSAT;
   if( pm.SATT ) delete[] pm.SATT;

   if( pm.Fug ) delete[] pm.Fug;
   if( pm.Fug_l ) delete[] pm.Fug_l;
   if( pm.Ppg_l ) delete[] pm.Ppg_l;

   // Part 3
   if( pm.Wb ) delete[] pm.Wb;
   if( pm.Wabs ) delete[] pm.Wabs;
   if( pm.Rion ) delete[] pm.Rion;
   if( pm.Qp ) delete[] pm.Qp;
   if( pm.Qd ) delete[] pm.Qd;

}

#endif

