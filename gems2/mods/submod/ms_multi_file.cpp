#include <math.h>
#include <iomanip.h>
#include <iostream>

#include "m_param.h"
#include "gdatastream.h"


void TMulti::inArray( fstream& ff, char *name, short* arr, int size )
{
 char buf[200];
 ff >> buf;
 if( memcmp( name, buf+1, strlen(name)))
    Error( buf, "Illegal name of array");
 for( int ii=0; ii<size; ii++  )
 {
    ff >> arr[ii];
 }
}

void TMulti::inArray( fstream& ff, char *name, float* arr, int size )
{
 char buf[200];
 ff >> buf;
 if( memcmp( name, buf+1, strlen(name)))
    Error( buf, "Illegal name of array");
 for( int ii=0; ii<size; ii++  )
 {
    ff >> arr[ii];
 }
}

void TMulti::inArray( fstream& ff, char *name, double* arr, int size )
{
 char buf[200];
 ff >> buf;
 if( memcmp( name, buf+1, strlen(name)))
    Error( buf, "Illegal name of array");
 for( int ii=0; ii<size; ii++  )
 {
    ff >> arr[ii];
 }
}


void TMulti::inArray( fstream& ff, char *name, char* arr,
                              int size, int arr_siz )
{
 char ch;
 char buf[200];
 ff >> buf;
 if( memcmp( name, buf+1, strlen(name)))
    Error( buf, "Illegal name of array");

 for( int ii=0; ii<size; ii++  )
 {
    ff.get(ch);
    while( ff.good() && ch != '\'' )
       ff.get(ch);
    ff.getline( buf, arr_siz+1, '\'');
    strncpy( arr +(ii*arr_siz), buf, arr_siz );
 }

}


//---------------------------------------------------------//
// for test out data
void TMulti::outArray( fstream& ff, char *name, char* arr,
                              int size, int arr_siz )
{
 ff << endl << "\'" << name << "\'" << endl;
 for( int ii=0, jj=0; ii<size; ii++, jj++  )
 {
    if(jj == 10)
    { jj=0;  ff << endl;}
    gstring str = gstring( arr +(ii*arr_siz), 0, arr_siz );
    str.strip();
    ff  << "\'" << str.c_str() << "\'" << " ";
 }
}



void TMulti::outArray( fstream& ff, char *name, short* arr, int size )
{
 ff << endl << "\'" << name << "\'" << endl;
 for( int ii=0, jj=0; ii<size; ii++, jj++  )
 {
    if(jj == 10)
    { jj=0;  ff << endl;}
    ff << arr[ii] << " ";
 }
}

void TMulti::outArray( fstream& ff, char *name,  float* arr,
            int size, int l_size )
{
 int sz = 10;
 if( l_size > 0 )
       sz = l_size;

 ff << endl << "\'" << name << "\'" << endl;
 for( int ii=0, jj=0; ii<size; ii++, jj++  )
 {
    if(jj == sz)
    { jj=0;  ff << endl;}
    ff << setprecision(7)  << arr[ii] << " ";
 }
}


void TMulti::outArray( fstream& ff, char *name, double* arr, int size )
{
 ff << endl << "\'" << name << "\'" << endl;
 for( int ii=0, jj=0; ii<size; ii++, jj++  )
 {
    if(jj == 10)
    { jj=0;  ff << endl;}
    ff << setprecision(14)  << arr[ii] << " ";
 }
}

void TMulti::to_text_file( gstring& path )
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

   gstring Path_ = path;
   gstring dir;
   gstring name;
   gstring ext;

   u_splitpath( Path_, dir, name, ext );
   Path_ = u_makepath( dir, name, "txt" );

  fstream ff(Path_.c_str(), ios::out );
  ErrorIf( !ff.good() , Path_.c_str(), "Fileopen error");

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
      int LsModSum = 0;
      int LsMdcSum = 0;
      for(int i=0; i<pm.FIs; i++)
      {
        LsModSum += pm.LsMod[i];
        LsMdcSum += (pm.LsMdc[i]*pm.L1[i]);
      }
      outArray( ff, "PMc", pm.PMc,  LsModSum);
      outArray( ff, "DMc", pm.DMc,  LsMdcSum);

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

//      outArray( ff, "MASDJ", pm.MASDJ, pm.Ls);
      outArray( ff, "MASDJ", &pm.MASDJ[0][0], pm.Ls*DFCN);
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
void TMulti::to_file( GemDataStream& ff, gstring& path  )
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


   ff.writeArray(pm.stkey, sizeof(char)*(EQ_RKLEN+5));
   ff.writeArray( &pm.N, 36);
   ff.writeArray(&pm.TC, 55);
   ff << PAalp;
   ff << PSigm;
   ff << EpsW;
   ff << RoW;

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
      ff.writeArray(pm.D, pm.L);
    }

   // Part 2  not requited arrays
    if( pm.FIs > 0 && pm.Ls > 0 )
    {
      ff.writeArray(pm.BF, pm.FIs*pm.N);
      ff.writeArray(pm.XFA, pm.FIs);
      ff.writeArray(pm.YFA, pm.FIs);
      ff.writeArray(pm.LsMod, pm.FIs);
      ff.writeArray(pm.LsMdc, pm.FIs);
      int LsModSum = 0;
      int LsMdcSum = 0;
      for(int i=0; i<pm.FIs; i++)
      {
        LsModSum += pm.LsMod[i];
        LsMdcSum += (pm.LsMdc[i]*pm.L1[i]);
      }
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
      ff.writeArray((float *)pm.Xr0h0, pm.FI*2);
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
    { /* ADSORBTION AND ION IXCHANDG */
      ff.writeArray((short*)pm.SATNdx, 2*pm.Ls);
      ff.writeArray((char*)pm.SCM, pm.FIs*pm.FIat);

      ff.writeArray((float*)pm.Nfsp, pm.FIs*pm.FIat);
      ff.writeArray((float*)pm.MASDT, pm.FIs*pm.FIat);
      ff.writeArray((float*)pm.XcapA, pm.FIs*pm.FIat);
      ff.writeArray((float*)pm.XcapB, pm.FIs*pm.FIat);
      ff.writeArray((float*)pm.XcapD, pm.FIs*pm.FIat);
      ff.writeArray((float*)pm.XcapF, pm.FIs*pm.FIat);
      ff.writeArray((float*)pm.XdlA, pm.FIs*pm.FIat);
      ff.writeArray((float*)pm.XdlB, pm.FIs*pm.FIat);
      ff.writeArray((float*)pm.XdlD, pm.FIs*pm.FIat);
      ff.writeArray((double*)pm.XpsiA, pm.FIs*pm.FIat);
      ff.writeArray((double*)pm.XpsiB, pm.FIs*pm.FIat);
      ff.writeArray((double*)pm.XpsiD, pm.FIs*pm.FIat);
      ff.writeArray((float*)pm.XlamA, pm.FIs*pm.FIat);
      ff.writeArray((float*)pm.Xetaf, pm.FIs*pm.FIat);
      ff.writeArray((double*)pm.XetaA, pm.FIs*pm.FIat);
      ff.writeArray((double*)pm.XetaB, pm.FIs*pm.FIat);
      ff.writeArray((double*)pm.XFTS, pm.FIs*pm.FIat);

      ff.writeArray(pm.SATT, pm.Ls);
      ff.writeArray((float*)pm.MASDJ, pm.Ls*DFCN);
//      ff.writeArray(pm.MASDJ, pm.Ls);
      ff.writeArray(pm.lnSAT, pm.Ls);
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

      ff.writeArray(pm.Qp,pm.FIs*20);
      ff.writeArray(pm.Qd, 20);
   }

  to_text_file( path );

}

// reading MULTI from binary file
void TMulti::from_file( GemDataStream& ff )
{
   //static values
   char PAalp;
   char PSigm;
   float EpsW;
   float RoW;

   ff.readArray(pm.stkey, sizeof(char)*(EQ_RKLEN+5));
   ff.readArray( &pm.N, 36);
   ff.readArray(&pm.TC, 55);
   ff >> PAalp;
   ff >> PSigm;
   ff >> EpsW;
   ff >> RoW;



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
      ff.readArray(pm.D, pm.L);
    }

   // Part 2  not requited arrays
    if( pm.FIs > 0 && pm.Ls > 0 )
    {
      ff.readArray(pm.BF, pm.FIs*pm.N);
      ff.readArray(pm.XFA, pm.FIs);
      ff.readArray(pm.YFA, pm.FIs);
      ff.readArray(pm.LsMod, pm.FIs);
      ff.readArray(pm.LsMdc, pm.FIs);

      int LsModSum = 0;
      int LsMdcSum = 0;
      for(int i=0; i<pm.FIs; i++)
      {
        LsModSum += pm.LsMod[i];
        LsMdcSum += (pm.LsMdc[i]*pm.L1[i]);
      }
     pm.PMc = new float[LsModSum];
     pm.DMc = new float[LsMdcSum];
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
      ff.readArray((float *)pm.Xr0h0, pm.FI*2);
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
      ff.readArray((short*)pm.SATNdx, 2*pm.Ls);
      ff.readArray((char*)pm.SCM, pm.FIs*pm.FIat);

      ff.readArray((float*)pm.Nfsp, pm.FIs*pm.FIat);
      ff.readArray((float*)pm.MASDT, pm.FIs*pm.FIat);
      ff.readArray((float*)pm.XcapA, pm.FIs*pm.FIat);
      ff.readArray((float*)pm.XcapB, pm.FIs*pm.FIat);
      ff.readArray((float*)pm.XcapD, pm.FIs*pm.FIat);
      ff.readArray((float*)pm.XcapF, pm.FIs*pm.FIat);
      ff.readArray((float*)pm.XdlA, pm.FIs*pm.FIat);
      ff.readArray((float*)pm.XdlB, pm.FIs*pm.FIat);
      ff.readArray((float*)pm.XdlD, pm.FIs*pm.FIat);
      ff.readArray((double*)pm.XpsiA, pm.FIs*pm.FIat);
      ff.readArray((double*)pm.XpsiB, pm.FIs*pm.FIat);
      ff.readArray((double*)pm.XpsiD, pm.FIs*pm.FIat);
      ff.readArray((float*)pm.XlamA, pm.FIs*pm.FIat);
      ff.readArray((float*)pm.Xetaf, pm.FIs*pm.FIat);
      ff.readArray((double*)pm.XetaA, pm.FIs*pm.FIat);
      ff.readArray((double*)pm.XetaB, pm.FIs*pm.FIat);
      ff.readArray((double*)pm.XFTS, pm.FIs*pm.FIat);

      ff.readArray(pm.SATT, pm.Ls);
      ff.readArray((float*)pm.MASDJ, pm.Ls*DFCN);
//      ff.readArray(pm.MASDJ, pm.Ls);
      ff.readArray(pm.lnSAT, pm.Ls);
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

      ff.readArray(pm.Qp,pm.FIs*20);
      ff.readArray(pm.Qd, 20);
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
     pm.PMc = 0;
     pm.DMc = 0;
//   pm.PMc = new float[pm.FIs];
//   pm.DMc = new float[pm.Ls];
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
   pm.SCM  = new char[pm.FIs][MST];

    pm.Nfsp = new float[pm.FIs][MST];
    pm.MASDT = new float[pm.FIs][MST];
    pm.XcapA = new float[pm.FIs][MST];
    pm.XcapB = new float[pm.FIs][MST];
    pm.XcapD = new float[pm.FIs][MST];
    pm.XcapF = new float[pm.FIs][MST];
    pm.XdlA = new float[pm.FIs][MST];
    pm.XdlB = new float[pm.FIs][MST];
    pm.XdlD = new float[pm.FIs][MST];
    pm.XpsiA = new double[pm.FIs][MST];
    pm.XpsiB = new double[pm.FIs][MST];
    pm.XpsiD = new double[pm.FIs][MST];
    pm.XlamA = new float[pm.FIs][MST];
    pm.Xetaf = new float[pm.FIs][MST];
    pm.XetaA = new double[pm.FIs][MST];
    pm.XetaB = new double[pm.FIs][MST];
    pm.MASDJ = new float[pm.Ls][DFCN];
//    pm.MASDJ = new float[pm.Ls];
    pm.XFTS = new double[pm.FIs][MST];
    pm.lnSAT = new double[pm.Ls];
    pm.SATT = new char[pm.Ls];
 }
else
 { // ADSORPTION AND ION EXCHANGE
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
    tes.Nfsp = new float[pm.FIs][MST];
     memcpy( tes.Nfsp, pm.Nfsp, pm.FIs*MST*sizeof(float) );
    tes.MASDT = new float[pm.FIs][MST];
     memcpy( tes.MASDT, pm.MASDT, pm.FIs*MST*sizeof(float) );
    tes.XcapA = new float[pm.FIs][MST];
     memcpy( tes.XcapA, pm.XcapA, pm.FIs*MST*sizeof(float) );
    tes.XcapB = new float[pm.FIs][MST];
     memcpy( tes.XcapB, pm.XcapB, pm.FIs*MST*sizeof(float) );
    tes.XcapD = new float[pm.FIs][MST];
     memcpy( tes.XcapD, pm.XcapD, pm.FIs*MST*sizeof(float) );
    tes.XcapF = new float[pm.FIs][MST];
     memcpy( tes.XcapF, pm.XcapF, pm.FIs*MST*sizeof(float) );
    tes.XdlA = new float[pm.FIs][MST];
     memcpy( tes.XdlA, pm.XdlA, pm.FIs*MST*sizeof(float) );
    tes.XdlB = new float[pm.FIs][MST];
     memcpy( tes.XdlB, pm.XdlB, pm.FIs*MST*sizeof(float) );
    tes.XdlD = new float[pm.FIs][MST];
     memcpy( tes.XdlD, pm.XdlD, pm.FIs*MST*sizeof(float) );
    tes.XpsiA = new double[pm.FIs][MST];
     memcpy( tes.XpsiA, pm.XpsiA, pm.FIs*MST*sizeof(double) );
    tes.XpsiB = new double[pm.FIs][MST];
     memcpy( tes.XpsiB, pm.XpsiB, pm.FIs*MST*sizeof(double) );
    tes.XpsiD = new double[pm.FIs][MST];
     memcpy( tes.XpsiD, pm.XpsiD, pm.FIs*MST*sizeof(double) );
    tes.XlamA = new float[pm.FIs][MST];
     memcpy( tes.XlamA, pm.XlamA, pm.FIs*MST*sizeof(float) );
    tes.Xetaf = new float[pm.FIs][MST];
     memcpy( tes.Xetaf, pm.Xetaf, pm.FIs*MST*sizeof(float) );
    tes.MASDJ = new float[pm.Ls][DFCN];
     memcpy( tes.MASDJ, pm.MASDJ, pm.Ls*DFCN*sizeof(float) );
//    tes.MASDJ = new float[pm.Ls];
//     memcpy( tes.MASDJ, pm.MASDJ, pm.Ls*sizeof(float) );
    tes.XetaA = new double[pm.FIs][MST];
     memcpy( tes.XetaA, pm.XetaA, pm.FIs*MST*sizeof(double) );
    tes.XetaB = new double[pm.FIs][MST];
     memcpy( tes.XetaB, pm.XetaB, pm.FIs*MST*sizeof(double) );
    tes.XFTS = new double[pm.FIs][MST];
     memcpy( tes.XFTS, pm.XFTS, pm.FIs*MST*sizeof(double) );
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
//#ifndef IPMGEMPLUGIN
//       vfMessage( window(), "Double ", buf.p );
//#endif
//       std::cout << "Double " << buf.p << endl;
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
//#ifndef IPMGEMPLUGIN
//       vfMessage(window(), "Float ", buf.p);
//#endif
//       cout << "Float  " << buf.p << endl;
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
//#ifndef IPMGEMPLUGIN
//       vfMessage(window(), "Float ", buf.p);
//#endif
//       cout << "Short  " << buf.p << endl;
//       break;
    }
}

// Test by comparing old and new dynamic memory of the MULTI structure
void TMulti::dyn__test(MULTI& tes)
{
// cout << "Test  begin:     previous     current" << endl;
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
 Test_Eq( pm.FIs*MST, &tes.Nfsp[0][0], &pm.Nfsp[0][0], "Nfsp" );
 Test_Eq( pm.FIs*MST, &tes.MASDT[0][0], &pm.MASDT[0][0], "MASDT" );
 Test_Eq( pm.FIs*MST, &tes.XcapA[0][0], &pm.XcapA[0][0], "XcapA" );
 Test_Eq( pm.FIs*MST, &tes.XcapB[0][0], &pm.XcapB[0][0], "XcapB" );
 Test_Eq( pm.FIs*MST, &tes.XcapD[0][0], &pm.XcapD[0][0], "XcapD" );
 Test_Eq( pm.FIs*MST, &tes.XcapF[0][0], &pm.XcapF[0][0], "XcapF" );
 Test_Eq( pm.FIs*MST, &tes.XdlA[0][0], &pm.XdlA[0][0], "XdlA" );
 Test_Eq( pm.FIs*MST, &tes.XdlB[0][0], &pm.XdlB[0][0], "XdlB" );
 Test_Eq( pm.FIs*MST, &tes.XdlD[0][0], &pm.XdlD[0][0], "XdlD" );
 Test_Eq( pm.FIs*MST, &tes.XpsiA[0][0], &pm.XpsiA[0][0], "XpsiA" );
 Test_Eq( pm.FIs*MST, &tes.XpsiB[0][0], &pm.XpsiB[0][0], "XpsiB" );
 Test_Eq( pm.FIs*MST, &tes.XpsiD[0][0], &pm.XpsiD[0][0], "XpsiD" );
 Test_Eq( pm.FIs*MST, &tes.XlamA[0][0], &pm.XlamA[0][0], "XlamA" );
 Test_Eq( pm.FIs*MST, &tes.Xetaf[0][0], &pm.Xetaf[0][0], "Xetaf" );
 Test_Eq( pm.FIs*MST, &tes.XpsiA[0][0], &pm.XpsiA[0][0], "XpsiA" );
 Test_Eq( pm.Ls*DFCN, &tes.MASDJ[0][0], &pm.MASDJ[0][0], "MASDJ" );
// Test_Eq( pm.Ls, tes.MASDJ, pm.MASDJ, "MASDJ" );
 Test_Eq( pm.FIs*MST, &tes.XetaA[0][0], &pm.XetaA[0][0], "XetaA" );
 Test_Eq( pm.FIs*MST, &tes.XetaB[0][0], &pm.XetaB[0][0], "XetaB" );
 Test_Eq( pm.FIs*MST, &tes.XFTS[0][0], &pm.XFTS[0][0], "XFTS" );
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
//  cout << "Test sequence end: " << endl;
}

#endif

