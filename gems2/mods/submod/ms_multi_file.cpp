
#include "m_param.h"
//---------------------------------------------------------//
// for test out data
void outArray( fstream& ff, char *name, short* arr, int size )
{
 ff << endl << name << endl;
 for( int ii=0, jj=1; ii<size; ii++, jj++  )
 {
    ff << arr[ii] << " ";
    if(jj == 10)
    { jj=0;  ff << endl;}
 }
}

void outArray( fstream& ff, char *name,  float* arr, int size )
{
 ff << endl << name << endl;
 for( int ii=0, jj=1; ii<size; ii++, jj++  )
 {
    ff << arr[ii] << " ";
    if(jj == 10)
    { jj=0;  ff << endl;}
 }
}


void outArray( fstream& ff, char *name, double* arr, int size )
{
 ff << endl << name << endl;
 for( int ii=0, jj=1; ii<size; ii++, jj++  )
 {
    ff << arr[ii] << " ";
    if(jj == 10)
    { jj=0;  ff << endl;}
 }
}

void TMulti::to_text_file( )
{
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

 fstream ff("Multi_Out", ios::out );
 ErrorIf( !ff.good() , "Multi_Out", "Fileopen error");

 ff << pm.stkey << endl;

   outArray( ff, "Short_Const",  &pm.N, 36 );
   outArray( ff, "Double_Const",  &pm.TC, 55 );
   outArray( ff, "EpsW", &EpsW, 1);
   outArray( ff, "RoW", &RoW, 1);

   //dynamic values

    // Part 1

    /* need  always to alloc vectors */
   outArray( ff, "L1", pm.L1,  pm.FI);
   outArray( ff, "muk", pm.muk, pm.FI);
   outArray( ff, "mui", pm.mui, pm.N);
   outArray( ff, "muj", pm.muj,  pm.L);
   outArray( ff, "DUL", pm.DUL,  pm.L);
   outArray( ff, "DLL", pm.DLL,  pm.L);
   outArray( ff, "Vol", pm.Vol,  pm.L);
   outArray( ff, "Pparc", pm.Pparc,  pm.L);
   outArray( ff, "MM", pm.MM,  pm.L);
   outArray( ff, "Awt", pm.Awt, pm.N);
   outArray( ff, "A", pm.A,  pm.N*pm.L);
   outArray( ff, "XFs", pm.XFs, pm.FI);
   outArray( ff, "Falps", pm.Falps,  pm.FI);
   outArray( ff, "G", pm.G,  pm.L);
   outArray( ff, "G0", pm.G0,  pm.L);
   outArray( ff, "lnGam", pm.lnGam,  pm.L);
   outArray( ff, "lnGmo", pm.lnGmo,  pm.L);
   outArray( ff, "B", pm.B,  pm.N);
   outArray( ff, "U", pm.U,  pm.N);
   outArray( ff, "U_r", pm.U_r,  pm.N);
   outArray( ff, "C", pm.C,  pm.N);
   outArray( ff, "XF", pm.XF,  pm.FI);
   outArray( ff, "YF", pm.YF,  pm.FI);
   outArray( ff, "Falp", pm.Falp,  pm.FI);
   outArray( ff, "X", pm.X,  pm.L);
   outArray( ff, "Y", pm.Y,  pm.L);
   outArray( ff, "XY", pm.XY,  pm.L);
   outArray( ff, "MU", pm.MU,  pm.L);
   outArray( ff, "EMU", pm.EMU,  pm.L);
   outArray( ff, "NMU", pm.NMU,  pm.L);
   outArray( ff, "W", pm.W,  pm.L);
   outArray( ff, "F", pm.F,  pm.L);
   outArray( ff, "F0", pm.F0,  pm.L);
   outArray( ff, "YOF", pm.YOF,  pm.FI);


   outArray( ff, "lnGmM", pm.lnGmM,  pm.L);
   outArray( ff, "GEX", pm.GEX,  pm.L);
   outArray( ff, "FVOL", pm.FVOL,  pm.FI);
   outArray( ff, "FWGT", pm.FWGT,  pm.FI);

    if( pm.L > 0 )
    {
      outArray( ff, "Y_la", pm.Y_la,  pm.L);
      outArray( ff, "Y_w", pm.Y_w,  pm.L);
      outArray( ff, "Fx", pm.Fx,  pm.L);
      outArray( ff, "Wx", pm.Wx,  pm.L);
      outArray( ff, "VL", pm.VL, pm.L);
      outArray( ff, "Gamma", pm.Gamma,  pm.L);
      outArray( ff, "lnGmf", pm.lnGmf,  pm.L);
      outArray( ff, "D", pm.D,  pm.L);
    }

   // Part 2  not requited arrays
    if( pm.FIs > 0 && pm.Ls > 0 )
    {
      outArray( ff, "BF", pm.BF,  pm.FIs*pm.N);
      outArray( ff, "XFA", pm.XFA,  pm.FIs);
      outArray( ff, "YFA", pm.YFA,  pm.FIs);
      outArray( ff, "LsMod", pm.LsMod, pm.FIs);
      outArray( ff, "LsMdc", pm.LsMdc, pm.FIs);
      outArray( ff, "PMc", pm.PMc,  pm.FIs);
      outArray( ff, "DMc", pm.DMc,  pm.Ls);
      outArray( ff, "PUL", pm.PUL,  pm.FIs);
      outArray( ff, "PLL", pm.PLL,  pm.FIs);

    }

    if( pm.LO > 1 )
    {
      outArray( ff, "Y_m", pm.Y_m,  pm.L);
      outArray( ff, "IC_m", pm.IC_m,  pm.N);
      outArray( ff, "IC_lm", pm.IC_lm,  pm.N);
      outArray( ff, "IC_wm", pm.IC_wm,  pm.N);
    }

    /* dispersion and sorbtion phases */
    if( PAalp != S_OFF )
    {
      outArray( ff, "Aalp", pm.Aalp, pm.FI);
      outArray( ff, "Xr0h0", &pm.Xr0h0[0][0],  pm.FI*2);
    }

   if( PSigm != S_OFF )
      outArray( ff, "Sigw", pm.Sigw,  pm.FI);

    if( PSigm != S_OFF )
      outArray( ff, "Sigg", pm.Sigg,  pm.FI);

    if( pm.E )
    {
      outArray( ff, "EZ", pm.EZ,  pm.L);
      outArray( ff, "Xcond", pm.Xcond,  pm.FI);
      outArray( ff, "Xeps", pm.Xeps,  pm.FI);
    }

    if( pm.FIat > 0 && /*pm.Lads > 0 &&Sveta 12/09/99*/ pm.FIs > 0 )
    { /* ADSORBTION AND ION IXCHANDG */
      outArray( ff, "SATNdx", &pm.SATNdx[0][0], 2*pm.Ls);

      outArray( ff, "Nfsp", &pm.Nfsp[0][0], pm.FIs*pm.FIat);
      outArray( ff, "MASDT", &pm.MASDT[0][0], pm.FIs*pm.FIat);
      outArray( ff, "XcapA", &pm.XcapA[0][0], pm.FIs*pm.FIat);
      outArray( ff, "XcapB", &pm.XcapB[0][0], pm.FIs*pm.FIat);
      outArray( ff, "XcapD", &pm.XcapD[0][0], pm.FIs*pm.FIat);
      outArray( ff, "XcapF", &pm.XcapF[0][0], pm.FIs*pm.FIat);
      outArray( ff, "XdlA", &pm.XdlA[0][0], pm.FIs*pm.FIat);
      outArray( ff, "XdlB", &pm.XdlB[0][0], pm.FIs*pm.FIat);
      outArray( ff, "XdlD", &pm.XdlD[0][0], pm.FIs*pm.FIat);
      outArray( ff, "XpsiA", &pm.XpsiA[0][0],  pm.FIs*pm.FIat);
      outArray( ff, "XpsiB", &pm.XpsiB[0][0],  pm.FIs*pm.FIat);
      outArray( ff, "XpsiD", &pm.XpsiD[0][0],  pm.FIs*pm.FIat);
      outArray( ff, "XlamA", &pm.XlamA[0][0], pm.FIs*pm.FIat);
      outArray( ff, "Xetaf", &pm.Xetaf[0][0], pm.FIs*pm.FIat);
      outArray( ff, "XetaA", &pm.XetaA[0][0],  pm.FIs*pm.FIat);
      outArray( ff, "XetaB", &pm.XetaB[0][0],  pm.FIs*pm.FIat);
      outArray( ff, "XFTS", &pm.XFTS[0][0],  pm.FIs*pm.FIat);

      outArray( ff, "MASDJ", pm.MASDJ, pm.Ls);
      outArray( ff, "lnSAT", pm.lnSAT,  pm.Ls);
    }

    if( pm.PG > 0 )
    {
      outArray( ff, "Fug", pm.Fug, pm.PG);
      outArray( ff, "Fug_l", pm.Fug_l, pm.PG);
      outArray( ff, "Ppg_l", pm.Ppg_l, pm.PG);
    }

    // Part 3

    if( pm.Ls > 1 && pm.FIs > 0 )
    {
      outArray( ff, "Wb", pm.Wb, pm.Ls);
      outArray( ff, "Wabs", pm.Wabs, pm.Ls);
      outArray( ff, "Rion", pm.Rion, pm.Ls);

      outArray( ff, "Qp", pm.Qp,  pm.FIs*20);
      outArray( ff, "Qd", pm.Qd,  20);

    }
}

//---------------------------------------------------------//

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
   to_text_file();
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

// set old data dynamic memory
void TMulti::dyn_new_test(MULTI& tes)
{
  // Part 1
 if( flCopy == true )
   dyn_kill_test(tes);
 memset(&tes, 0, sizeof(MULTI));
 memcpy( &tes.N, &pm.N, 36*sizeof(short) );
 memcpy( &tes.TC, &pm.TC, 54*sizeof(double) );

 // tes.A = (float *)aObj[ o_wi_a].Alloc( tes.L, tes.N, F_ );
// tes.SB = (char (*)[MAXICNAME+MAXSYMB])aObj[ o_wd_sb].Alloc(
//    tes.N, 1, MAXICNAME+MAXSYMB );
// Error in free memory
//aObj[ o_w_sbh].SetPtr( tes.SB ); aObj[ o_w_sbh ].SetDim( tes.N, 1 );
// tes.SFs = (char (*)[MAXPHNAME+MAXSYMB])aObj[ o_wd_sfs].Alloc(
//    tes.FI, 1, MAXPHNAME+MAXSYMB );
// tes.SM = (char (*)[MAXDCNAME])aObj[ o_wd_sm].Alloc( tes.L, 1, MAXDCNAME );
// tes.SF = (char (*)[MAXPHNAME+MAXSYMB])aObj[ o_wd_sf].Alloc(
//     tes.FI, 1, MAXPHNAME+MAXSYMB );
// tes.SM2 = (char (*)[MAXDCNAME])aObj[ o_wd_sm2].Alloc( tes.Ls, 1, MAXDCNAME );
// tes.SF2 = (char (*)[MAXPHNAME+MAXSYMB])aObj[ o_wd_sf2].Alloc(
//     tes.FIs, 1, MAXPHNAME+MAXSYMB );

// tes.RLC = (char *)aObj[ o_wi_rlc].Alloc( tes.L, 1, A_ );
// tes.RSC = (char *)aObj[ o_wi_rsc].Alloc( tes.L, 1, A_ );
// tes.ICC = (char *)aObj[ o_wi_icc].Alloc( tes.N, 1, A_ );
// tes.DCC = (char *)aObj[ o_wi_dcc].Alloc( tes.L, 1, A_ );
// tes.PHC = (char *)aObj[ o_wi_phc].Alloc( tes.FI, 1, A_ );
// tes.DCCW = (char *)aObj[ o_wi_dccw].Alloc( tes.L, 1, A_ );

 // need  always to alloc vectors
 tes.L1 = new short[pm.FI];
 memcpy( tes.L1, pm.L1, pm.FI*sizeof(short) );
 tes.muk = new short[pm.FI];
 memcpy( tes.muk, pm.muk, pm.FI*sizeof(short) );
 tes.mui = new short[pm.N];
 memcpy( tes.mui, pm.mui, pm.N*sizeof(short) );
 tes.muj = new short[pm.L];
 memcpy( tes.muj, pm.muj, pm.L*sizeof(short) );

 tes.DUL = new double[pm.L];
 memcpy( tes.DUL, pm.DUL, pm.L*sizeof(double) );
 tes.DLL = new double[pm.L];
 memcpy( tes.DLL, pm.DLL, pm.L*sizeof(double) );
 tes.Vol = new double[pm.L];
 memcpy( tes.Vol, pm.Vol, pm.L*sizeof(double) );
 tes.Pparc = new double[pm.L];
 memcpy( tes.Pparc, pm.Pparc, pm.L*sizeof(double) );
 tes.MM = new double[pm.L];
 memcpy( tes.MM, pm.MM, pm.L*sizeof(double) );
 tes.Awt = new float[pm.N];
 memcpy( tes.Awt, pm.Awt, pm.N*sizeof(float) );
 tes.XFs = new float[pm.FI];
 memcpy( tes.XFs, pm.XFs, pm.FI*sizeof(float) );
 tes.Falps = new float[pm.FI];
 memcpy( tes.Falps, pm.Falps, pm.FI*sizeof(float) );
 tes.G = new double[pm.L];
 memcpy( tes.G, pm.G, pm.L*sizeof(double) );
 tes.G0 = new double[pm.L];
 memcpy( tes.G0, pm.G0, pm.L*sizeof(double) );
 tes.lnGam = new double[pm.L];
 memcpy( tes.lnGam, pm.lnGam, pm.L*sizeof(double) );
 tes.lnGmo = new double[pm.L];
 memcpy( tes.lnGmo, pm.lnGmo, pm.L*sizeof(double) );
 tes.B = new double[pm.N];
 memcpy( tes.B, pm.B, pm.N*sizeof(double) );
 tes.U = new double[pm.N];
 memcpy( tes.U, pm.U, pm.N*sizeof(double) );
 tes.U_r = new double[pm.N];
 memcpy( tes.U_r, pm.U_r, pm.N*sizeof(double) );
 tes.C = new double[pm.N];
 memcpy( tes.C, pm.C, pm.N*sizeof(double) );
 tes.XF = new double[pm.FI];
 memcpy( tes.XF, pm.XF, pm.FI*sizeof(double) );
 tes.YF = new double[pm.FI];
 memcpy( tes.YF, pm.YF, pm.FI*sizeof(double) );
 tes.Falp = new double[pm.FI];
 memcpy( tes.Falp, pm.Falp, pm.FI*sizeof(double) );
 tes.X = new double[pm.L];
 memcpy( tes.X, pm.X, pm.L*sizeof(double) );
 tes.Y = new double[pm.L];
 memcpy( tes.Y, pm.Y, pm.L*sizeof(double) );
 tes.XY = new double[pm.L];
 memcpy( tes.XY, pm.XY, pm.L*sizeof(double) );
 tes.MU = new double[pm.L];
 memcpy( tes.MU, pm.MU, pm.L*sizeof(double) );
 tes.EMU = new double[pm.L];
 memcpy( tes.EMU, pm.EMU, pm.L*sizeof(double) );
 tes.NMU = new double[pm.L];
 memcpy( tes.NMU, pm.NMU, pm.L*sizeof(double) );
 tes.W = new double[pm.L];
 memcpy( tes.W, pm.W, pm.L*sizeof(double) );
 tes.F = new double[pm.L];
 memcpy( tes.F, pm.F, pm.L*sizeof(double) );
 tes.F0 = new double[pm.L];
 memcpy( tes.F0, pm.F0, pm.L*sizeof(double) );
 tes.YOF = new double[pm.FI];
 memcpy( tes.YOF, pm.YOF, pm.FI*sizeof(double) );
 tes.lnGmM = new double[pm.L];
 memcpy( tes.lnGmM, pm.lnGmM, pm.L*sizeof(double) );
 tes.GEX = new double[pm.L];
 memcpy( tes.GEX, pm.GEX, pm.L*sizeof(double) );
 tes.FVOL = new double[pm.FI];
 memcpy( tes.FVOL, pm.FVOL, pm.FI*sizeof(double) );
 tes.FWGT = new double[pm.FI];
 memcpy( tes.FWGT, pm.FWGT, pm.FI*sizeof(double) );

 if( tes.L > 0 )
 {
   tes.Y_la = new double[pm.L];
   memcpy( tes.Y_la, pm.Y_la, pm.L*sizeof(double) );
   tes.Y_w = new double[pm.L];
   memcpy( tes.Y_w, pm.Y_w, pm.L*sizeof(double) );
   tes.Fx = new double[pm.L];
   memcpy( tes.Fx, pm.Fx, pm.L*sizeof(double) );
   tes.Wx = new double[pm.L];
   memcpy( tes.Wx, pm.Wx, pm.L*sizeof(double) );
   tes.VL = new float[pm.L];
   memcpy( tes.VL, pm.VL, pm.L*sizeof(float) );
   tes.Gamma = new double[pm.L];
   memcpy( tes.Gamma, pm.Gamma, pm.L*sizeof(double) );
   tes.lnGmf = new double[pm.L];
   memcpy( tes.lnGmf, pm.lnGmf, pm.L*sizeof(double) );
   tes.D = new double[pm.L];
   memcpy( tes.D, pm.D, pm.L*sizeof(double) );
 }

   // Part 2  not requited arrays

 if( tes.FIs > 0 && tes.Ls > 0 )
 {
   tes.BF = new double[pm.FIs*pm.N];
   memcpy( tes.BF, pm.BF, pm.FIs*pm.N*sizeof(double) );
   tes.XFA = new double[pm.FIs];
   memcpy( tes.XFA, pm.XFA, pm.FIs*sizeof(double) );
   tes.YFA = new double[pm.FIs];
   memcpy( tes.YFA, pm.YFA, pm.FIs*sizeof(double) );
//     tes.LsMod = (short *)aObj[ o_wi_lsmod].Alloc( tes.FIs, 1, I_ );
//     tes.LsMdc = (short *)aObj[ o_wi_lsmdc].Alloc( tes.FIs, 1, I_ );
//    PMs and DMS may be realloced after load arrays LsMod and LsMdc
//   tes.PMc = new float[pm.FIs];
//   memcpy( tes.PMc, pm.PMc, pm.FIs*sizeof(float) );
//   tes.DMc = new float[pm.Ls];
//   memcpy( tes.DMc, pm.DMc, pm.Ls*sizeof(float) );
   tes.PUL = new double[pm.FIs];
   memcpy( tes.PUL, pm.PUL, pm.FIs*sizeof(double) );
   tes.PLL = new double[pm.FIs];
   memcpy( tes.PLL, pm.PLL, pm.FIs*sizeof(double) );
//     tes.YOF = (float *)aObj[ o_wi_yof].Alloc( tes.FI, 1, F_ );
//     tes.sMod = (char (*)[6])aObj[ o_wi_smod].Alloc( tes.FIs, 1, 6 );
//     tes.RFLC = (char *)aObj[ o_wi_rflc].Alloc( tes.FIs, 1, A_ );
//     tes.RFSC = (char *)aObj[ o_wi_rfsc].Alloc( tes.FIs, 1, A_ );
 }

 if( tes.LO > 1 )
 {
   tes.Y_m = new double[pm.L];
   memcpy( tes.Y_m, pm.Y_m, pm.L*sizeof(double) );
   tes.IC_m = new double[pm.N];
   memcpy( tes.IC_m, pm.IC_m, pm.N*sizeof(double) );
   tes.IC_lm = new double[pm.N];
   memcpy( tes.IC_lm, pm.IC_lm, pm.N*sizeof(double) );
   tes.IC_wm = new double[pm.N];
   memcpy( tes.IC_wm, pm.IC_wm, pm.N*sizeof(double) );
 }
 // dispersion and sorbtion phases
 if( PAalp_ != S_OFF )
 {
   tes.Aalp = new float[pm.FI];
   memcpy( tes.Aalp, pm.Aalp, pm.FI*sizeof(float) );
   tes.Xr0h0 = new float[pm.FI][2];
   memcpy( tes.Xr0h0, pm.Xr0h0, pm.FI*2*sizeof(float) );
 }
 if( PSigm_ != S_OFF )
 {    tes.Sigw = new float[pm.FI];
     memcpy( tes.Sigw, pm.Sigw, pm.FI*sizeof(float) );
 }
 if( PSigm_ != S_OFF )
 {    tes.Sigg = new float[pm.FI];
     memcpy( tes.Sigg, pm.Sigg, pm.FI*sizeof(float) );
 }
 if( tes.E )
 {
    tes.EZ = new double[pm.L];
     memcpy( tes.EZ, pm.EZ, pm.L*sizeof(double) );
    tes.Xcond = new float[pm.FI];
     memcpy( tes.Xcond, pm.Xcond, pm.FI*sizeof(float) );
    tes.Xeps = new float[pm.FI];
     memcpy( tes.Xeps, pm.Xeps, pm.FI*sizeof(float) );
 }

 if( tes.FIat > 0 && tes.Lads > 0 && tes.FIs > 0 )
 { // ADSORBTION AND ION EXCHANGE
//   tes.SATNdx = (short (*)[2])aObj[ o_wi_satndx].Alloc( tes.Ls, 2, I_ );
//   tes.SCM  = (char (*)[MST])aObj[ o_wi_scm].Alloc( tes.FIs, tes.FIat, A_ );
//   memset( tes.SCM, SC_NOT_USED, tes.FIs*tes.FIat );
    tes.Nfsp = new float[pm.FIs][6];
     memcpy( tes.Nfsp, pm.Nfsp, pm.FIs*6*sizeof(float) );
    tes.MASDT = new float[pm.FIs][6];
     memcpy( tes.MASDT, pm.MASDT, pm.FIs*6*sizeof(float) );
    tes.XcapA = new float[pm.FIs][6];
     memcpy( tes.XcapA, pm.XcapA, pm.FIs*6*sizeof(float) );
    tes.XcapB = new float[pm.FIs][6];
     memcpy( tes.XcapB, pm.XcapB, pm.FIs*6*sizeof(float) );
    tes.XcapD = new float[pm.FIs][6];
     memcpy( tes.XcapD, pm.XcapD, pm.FIs*6*sizeof(float) );
    tes.XcapF = new float[pm.FIs][6];
     memcpy( tes.XcapF, pm.XcapF, pm.FIs*6*sizeof(float) );
    tes.XdlA = new float[pm.FIs][6];
     memcpy( tes.XdlA, pm.XdlA, pm.FIs*6*sizeof(float) );
    tes.XdlB = new float[pm.FIs][6];
     memcpy( tes.XdlB, pm.XdlB, pm.FIs*6*sizeof(float) );
    tes.XdlD = new float[pm.FIs][6];
     memcpy( tes.XdlD, pm.XdlD, pm.FIs*6*sizeof(float) );
    tes.XpsiA = new double[pm.FIs][6];
     memcpy( tes.XpsiA, pm.XpsiA, pm.FIs*6*sizeof(double) );
    tes.XpsiB = new double[pm.FIs][6];
     memcpy( tes.XpsiB, pm.XpsiB, pm.FIs*6*sizeof(double) );
    tes.XpsiD = new double[pm.FIs][6];
     memcpy( tes.XpsiD, pm.XpsiD, pm.FIs*6*sizeof(double) );
    tes.XlamA = new float[pm.FIs][6];
     memcpy( tes.XlamA, pm.XlamA, pm.FIs*6*sizeof(float) );
    tes.Xetaf = new float[pm.FIs][6];
     memcpy( tes.Xetaf, pm.Xetaf, pm.FIs*6*sizeof(float) );
    tes.MASDJ = new float[pm.Ls];
     memcpy( tes.MASDJ, pm.MASDJ, pm.Ls*sizeof(float) );

    tes.XetaA = new double[pm.FIs][6];
     memcpy( tes.XetaA, pm.XetaA, pm.FIs*6*sizeof(double) );
    tes.XetaB = new double[pm.FIs][6];
     memcpy( tes.XetaB, pm.XetaB, pm.FIs*6*sizeof(double) );
    tes.XFTS = new double[pm.FIs][6];
     memcpy( tes.XFTS, pm.XFTS, pm.FIs*6*sizeof(double) );
    tes.lnSAT = new double[pm.Ls];
     memcpy( tes.lnSAT, pm.lnSAT, pm.Ls*sizeof(double) );
//   tes.SATT = (char *)aObj[ o_wi_satt].Alloc( tes.Ls, 1, A_ );
 }

 if( tes.PG > 0 )
 {
//   tes.Fug = (float *)aObj[ o_wd_fug].Alloc( tes.PG, 1, F_ );
//   tes.Fug_l = (float *)aObj[ o_wd_fugl].Alloc( tes.PG, 1, F_ );
//   tes.Ppg_l = (float *)aObj[ o_wd_ppgl].Alloc( tes.PG, 1, F_ );
 }

   // Part 3

 if( tes.Ls > 1 && tes.FIs > 0 )
 {
    tes.Wb = new float[pm.Ls];
     memcpy( tes.Wb, pm.Wb, pm.Ls*sizeof(float) );
    tes.Wabs = new float[pm.Ls];
     memcpy( tes.Wabs, pm.Wabs, pm.Ls*sizeof(float) );
    tes.Rion = new float[pm.Ls];
     memcpy( tes.Rion, pm.Rion, pm.Ls*sizeof(float) );
//   tes.Qp = (double *)aObj[ o_wi_qp].Alloc( tes.FIs, 20, D_ );
//     tes.Qd = (double *)aObj[ o_wi_qd].Alloc( 1, 20, D_ );

 }
  flCopy = true;
// tes.R = (float *)aObj[ o_w_r].Alloc( tes.N, tes.N+1, D_ );

}
// free dynamic memory in objects and values
void TMulti::dyn_kill_test(MULTI& tes)
{
// memcpy( &pm.PunE, "jjbC", 4 );
// memset( &pm.N, 0, 36*sizeof(short));
// memset( &pm.TC, 0, 54*sizeof(double));
 delete[] tes.L1; /* delete[] tes.LsMod;   delete[] tes.LsMdc; */
 delete[] tes.mui;    delete[] tes.muk  ;    delete[] tes.muj  ;
 delete[] tes.SATNdx;    delete[] tes.DUL  ;    delete[] tes.DLL  ;
 delete[] tes.GEX;    delete[] tes.PUL  ;    delete[] tes.PLL  ;
 delete[] tes.YOF; /*  delete[] tes.PMc  ;    delete[] tes.DMc  ; */
 delete[] tes.Vol;    delete[] tes.HYM  ;    delete[] tes.VL   ;
 delete[] tes.MM ;    delete[] tes.H0   ;    delete[] tes.A0   ;
 delete[] tes.U0;    delete[] tes.S0   ;    delete[] tes.Cp0  ;
 delete[] tes.Cv0;    delete[] tes.Pparc;    delete[] tes.Y_m  ;
 delete[] tes.Y_la;    delete[] tes.Y_w  ;    delete[] tes.Gamma;
 delete[] tes.lnGmf;    delete[] tes.lnGmM;    delete[] tes.EZ   ;
 delete[] tes.Wb;    delete[] tes.Wabs ;    delete[] tes.Rion ;
 delete[] tes.Aalp;    delete[] tes.Sigw ;    delete[] tes.Sigg ;
 delete[] tes.Nfsp;    delete[] tes.MASDT;    delete[] tes.FVOL ;
 delete[] tes.FWGT;    delete[] tes.XcapA;    delete[] tes.XcapB;
 delete[] tes.XcapD;    delete[] tes.XdlA ;    delete[] tes.XdlB ;
 delete[] tes.XdlD;    delete[] tes.XpsiA;    delete[] tes.XpsiB;
 delete[] tes.XpsiD;    delete[] tes.Xr0h0;    delete[] tes.XlamA;
 delete[] tes.Xetaf;    delete[] tes.Xcond;    delete[] tes.Xeps ;
 delete[] tes.Awt;    delete[] tes.A    ;    delete[] tes.XFs  ;
 delete[] tes.Falps;    delete[] tes.Fug  ;    delete[] tes.Fug_l;
 delete[] tes.Ppg_l;    delete[] tes.XFTS ;    delete[] tes.MASDJ;
 delete[] tes.G;    delete[] tes.G0   ;    delete[] tes.lnGam;
 delete[] tes.lnGmo;    delete[] tes.lnSAT;    delete[] tes.B    ;
 delete[] tes.U;    delete[] tes.U_r  ;    delete[] tes.C    ;
 delete[] tes.IC_m;    delete[] tes.IC_lm;    delete[] tes.IC_wm;
 delete[] tes.BF;    delete[] tes.XF   ;    delete[] tes.YF   ;
 delete[] tes.XFA;    delete[] tes.YFA  ;    delete[] tes.Falp ;
 delete[] tes.XetaA;    delete[] tes.XetaB;    delete[] tes.X    ;
 delete[] tes.Y;    delete[] tes.XY   ;    delete[] tes.Qp   ;
 delete[] tes.Qd;    delete[] tes.MU   ;    delete[] tes.EMU  ;
 delete[] tes.NMU;    delete[] tes.W    ;    delete[] tes.Fx   ;
 delete[] tes.Wx;    delete[] tes.F    ;    delete[] tes.F0   ;
 delete[] tes.D;    delete[] tes.R    ;    delete[] tes.R1   ;
 /* delete[] tes.sMod; */   delete[] tes.SB   ;    delete[] tes.SM   ;
 delete[] tes.SF;    delete[] tes.SFs  ;    delete[] tes.pbuf ; //
 delete[] tes.RLC;    delete[] tes.RSC  ;   /* delete[] tes.RFLC ;
 delete[] tes.RFSC;  */  delete[] tes.ICC  ;    delete[] tes.DCC  ;
 delete[] tes.PHC;    delete[] tes.SCM  ;  /*  delete[] tes.SATT ; */
 delete[] tes.DCCW;    delete[] tes.XcapF;    delete[] tes.SM2   ;
 delete[] tes.SF2;

}

void TMulti::Test_Eq( int size, double *p1, double *p2, const char *key )
{
  vstr buf(150);
  for( int i=0; i<size; i++ )
    if( fabs(p1[i]-p2[i]) > fabs(p1[i])*1e-7 )
    {
       sprintf(buf, "%7s[%3d]:  %12.8g  %12.8g", key, i, p1[i], p2[i]);
#ifndef IPMGEMPLUGIN
//       vfMessage( window(), "Double ", buf.p );
#endif
       cout << "Double " << buf.p << endl;
//       break;
    }
}

void TMulti::Test_Eq( int size, float *p1, float *p2, const char *key )
{
  vstr buf(150);
  for( int i=0; i<size; i++ )
    if( p1[i] != p2[i] )
    {
       sprintf(buf, "%7s[%3d]:  %12.8g  %12.8g", key, i, p1[i], p2[i]);
#ifndef IPMGEMPLUGIN
//       vfMessage(window(), "Float ", buf.p);
#endif
       cout << "Float  " << buf.p << endl;
//       break;
    }
}

void TMulti::Test_Eq( int size, short *p1, short *p2, const char *key )
{
  vstr buf(150);
  for( int i=0; i<size; i++ )
    if( p1[i] != p2[i] )
    {
       sprintf(buf, "%7s[%3d]:  %12d  %12d", key, i, p1[i], p2[i]);
#ifndef IPMGEMPLUGIN
//       vfMessage(window(), "Float ", buf.p);
#endif
       cout << "Short  " << buf.p << endl;
//       break;
    }
}

// Test by comparing old and new dynamic memory of the MULTI structure
void TMulti::dyn__test(MULTI& tes)
{
 cout << "Test  begin:     previous     current" << endl;
 Test_Eq( 36, &tes.N, &pm.N, "IFlags" );
 Test_Eq( 54, &tes.TC, &pm.TC, "Scalars" );
 Test_Eq( pm.L, tes.DUL, pm.DUL, "DUL" );
 Test_Eq( pm.L, tes.DLL, pm.DLL, "DLL" );
 Test_Eq( pm.L, tes.Vol, pm.Vol, "Vol" );
 Test_Eq( pm.L, tes.Pparc, pm.Pparc, "Pparc" );
 Test_Eq( pm.L, tes.MM, pm.MM, "MM" );
 Test_Eq( pm.N, tes.Awt, pm.Awt, "Awt" );
// Test_Eq( pm.FI, tes.XFs, pm.XFs, "XFs" );
// Test_Eq( pm.FI, tes.Falps, pm.Falps, "Falps" );
 Test_Eq( pm.L, tes.G, pm.G, "G" );
 Test_Eq( pm.L, tes.G0, pm.G0, "G0" );
 Test_Eq( pm.L, tes.lnGam, pm.lnGam, "lnGam" );
 Test_Eq( pm.L, tes.lnGmo, pm.lnGmo, "lnGmo" );
 Test_Eq( pm.N, tes.B, pm.B, "B" );
 Test_Eq( pm.N, tes.U, pm.U, "U" );
 Test_Eq( pm.N, tes.U_r, pm.U_r, "U_r" );
 Test_Eq( pm.N, tes.C, pm.C, "C" );
 Test_Eq( pm.FI, tes.XF, pm.XF, "XF" );
 Test_Eq( pm.FI, tes.YF, pm.YF, "YF" );
 Test_Eq( pm.FI, tes.Falp, pm.Falp, "Falp" );
 Test_Eq( pm.L, tes.X, pm.X, "X" );
 Test_Eq( pm.L, tes.Y, pm.Y, "Y" );
 Test_Eq( pm.L, tes.XY, pm.XY, "XY" );
 Test_Eq( pm.L, tes.MU, pm.MU, "MU" );
 Test_Eq( pm.L, tes.EMU, pm.EMU, "EMU" );
 Test_Eq( pm.L, tes.NMU, pm.NMU, "NMU" );
 Test_Eq( pm.L, tes.W, pm.W, "W" );
 Test_Eq( pm.L, tes.F, pm.F, "F" );
 Test_Eq( pm.L, tes.F0, pm.F0, "F0" );
 Test_Eq( pm.FI, tes.YOF, pm.YOF, "YOF" );
 Test_Eq( pm.FI, tes.FVOL, pm.FVOL, "FVOL" );
 Test_Eq( pm.FI, tes.FWGT, pm.FWGT, "FWGT" );
 Test_Eq( pm.L, tes.lnGmM, pm.lnGmM, "lnGmM" );
 Test_Eq( pm.L, tes.GEX, pm.GEX, "GEX" );

 if( tes.L > 0 )
 {
 Test_Eq( pm.L, tes.Y_la, pm.Y_la, "Y_la" );
 Test_Eq( pm.L, tes.Y_w, pm.Y_w, "Y_w" );
 Test_Eq( pm.L, tes.Fx, pm.Fx, "Fx" );
 Test_Eq( pm.L, tes.Wx, pm.Wx, "Wx" );
 Test_Eq( pm.L, tes.VL, pm.VL, "VL" );
 Test_Eq( pm.L, tes.Gamma, pm.Gamma, "Gamma" );
 Test_Eq( pm.L, tes.lnGmf, pm.lnGmf, "lnGmf" );
 Test_Eq( pm.L, tes.D, pm.D, "D" );
 }

   // Part 2  not required arrays

 if( tes.FIs > 0 && tes.Ls > 0 )
 {
 Test_Eq( pm.FIs*pm.N, tes.BF, pm.BF, "BF" );
 Test_Eq( pm.FIs, tes.XFA, pm.XFA, "XFA" );
 Test_Eq( pm.FIs, tes.YFA, pm.YFA, "YFA" );
// Test_Eq( pm.FIs, tes.PMc, pm.PMc, "PMc" ); /* not allocated ! */
// Test_Eq( pm.FIs, tes.DMc, pm.DMc, "DMc" ); /* not allocated ! */
// Test_Eq( pm.FIs, tes.PUL, pm.PUL, "PUL" );
// Test_Eq( pm.FIs, tes.PLL, pm.PLL, "PLL" );
}

 if( tes.LO > 1 )
 {
 Test_Eq( pm.L, tes.Y_m, pm.Y_m, "Y_m" );
 Test_Eq( pm.N, tes.IC_m, pm.IC_m, "IC_m" );
 Test_Eq( pm.N, tes.IC_lm, pm.IC_lm, "IC_lm" );
 Test_Eq( pm.N, tes.IC_wm, pm.IC_wm, "IC_wm" );
 }
 // sorption phases
 if( PAalp_ != S_OFF )
 {
 Test_Eq( pm.FI, tes.Aalp, pm.Aalp, "Aalp" );
// Test_Eq( pm.FI*2, &tes.Xr0h0[0][0], &pm.Xr0h0[0][0], "Xr0h0" );
 }
 if( PSigm_ != S_OFF )
   Test_Eq( pm.FI, tes.Sigw, pm.Sigw, "Sigw" );
 if( PSigm_ != S_OFF )
   Test_Eq( pm.FI, tes.Sigg, pm.Sigg, "Sigg" );
 if( tes.E )
 {
 Test_Eq( pm.L, tes.EZ, pm.EZ, "EZ" );
 Test_Eq( pm.FI, tes.Xcond, pm.Xcond, "Xcond" );
 Test_Eq( pm.FI, tes.Xeps, pm.Xeps, "Xeps" );
 }

 if( tes.FIat > 0 && tes.Lads > 0 && tes.FIs > 0 )
 {
 Test_Eq( pm.FIs*6, &tes.Nfsp[0][0], &pm.Nfsp[0][0], "Nfsp" );
 Test_Eq( pm.FIs*6, &tes.MASDT[0][0], &pm.MASDT[0][0], "MASDT" );
 Test_Eq( pm.FIs*6, &tes.XcapA[0][0], &pm.XcapA[0][0], "XcapA" );
 Test_Eq( pm.FIs*6, &tes.XcapB[0][0], &pm.XcapB[0][0], "XcapB" );
 Test_Eq( pm.FIs*6, &tes.XcapD[0][0], &pm.XcapD[0][0], "XcapD" );
 Test_Eq( pm.FIs*6, &tes.XcapF[0][0], &pm.XcapF[0][0], "XcapF" );
 Test_Eq( pm.FIs*6, &tes.XdlA[0][0], &pm.XdlA[0][0], "XdlA" );
 Test_Eq( pm.FIs*6, &tes.XdlB[0][0], &pm.XdlB[0][0], "XdlB" );
 Test_Eq( pm.FIs*6, &tes.XdlD[0][0], &pm.XdlD[0][0], "XdlD" );
 Test_Eq( pm.FIs*6, &tes.XpsiA[0][0], &pm.XpsiA[0][0], "XpsiA" );
 Test_Eq( pm.FIs*6, &tes.XpsiB[0][0], &pm.XpsiB[0][0], "XpsiB" );
 Test_Eq( pm.FIs*6, &tes.XpsiD[0][0], &pm.XpsiD[0][0], "XpsiD" );
 Test_Eq( pm.FIs*6, &tes.XlamA[0][0], &pm.XlamA[0][0], "XlamA" );
 Test_Eq( pm.FIs*6, &tes.Xetaf[0][0], &pm.Xetaf[0][0], "Xetaf" );
 Test_Eq( pm.FIs*6, &tes.XpsiA[0][0], &pm.XpsiA[0][0], "XpsiA" );
 Test_Eq( pm.Ls, tes.MASDJ, pm.MASDJ, "MASDJ" );
 Test_Eq( pm.FIs*6, &tes.XetaA[0][0], &pm.XetaA[0][0], "XetaA" );
 Test_Eq( pm.FIs*6, &tes.XetaB[0][0], &pm.XetaB[0][0], "XetaB" );
 Test_Eq( pm.FIs*6, &tes.XFTS[0][0], &pm.XFTS[0][0], "XFTS" );
// Test_Eq( pm.Ls, tes.lnSAT, pm.lnSAT, "lnSAT" );
 }

 if( tes.PG > 0 )
 {
//   tes.Fug = (float *)aObj[ o_wd_fug].Alloc( tes.PG, 1, F_ );
//   tes.Fug_l = (float *)aObj[ o_wd_fugl].Alloc( tes.PG, 1, F_ );
//   tes.Ppg_l = (float *)aObj[ o_wd_ppgl].Alloc( tes.PG, 1, F_ );
 }

   // Part 3

// if( tes.Ls > 1 && tes.FIs > 0 )
// {
// Test_Eq( pm.Ls, tes.Wb, pm.Wb, "Wb" );
// Test_Eq( pm.Ls, tes.Wabs, pm.Wabs, "Wabs" );
// Test_Eq( pm.Ls, tes.Rion, pm.Rion, "Rion" );
// }
  cout << "Test sequence end: " << endl;
}

#endif

