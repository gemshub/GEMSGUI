//masstransport
//-------------------------------------------------------------------
// Test run GEMIPM2k
//#include <iostream>

#include "verror.h"
#include "m_param.h"
#include "gdatastream.h"

// extern "C" int __stdcall MAIF_START( int nNodes,
//   int  c_to_i1[30], int c_to_i2[30], int *nodeTypes );

extern "C" int MAIF_START( int nNodes,
   int  c_to_i1[30], int c_to_i2[30], int *nodeTypes );

// extern "C" int __stdcall MAIF_CALC( int iNode,
extern "C" int MAIF_CALC( int iNodeF, // negative: input only (fortran index)
   short &p_NodeHandle,    // Node identification handle
   short &p_NodeTypeHY,    // Node type (hydraulic); see typedef NODETYPE
   short &p_NodeTypeMT,    // Node type (mass transport); see typedef NODETYPE
   short &p_NodeStatusFMT, // Node status code FMT; see typedef NODECODEFMT
   short &p_NodeStatusCH,  // Node status code CH;  see typedef NODECODECH
   short &p_IterDone,      // Number of iterations performed by IPM (if not need GEM)
// Chemical scalar variables
   double &p_T,     // Temperature T, K                        +      +      -     -
   double &p_P,     // Pressure P, bar                         +      +      -     -
   double &p_Vs,    // Volume V of reactive subsystem, cm3     -      -      +     +
   double &p_Vi,    // Volume of inert subsystem  ?            +      -      -     +
   double &p_Ms,    // Mass of reactive subsystem, kg          +      +      -     -
   double &p_Mi,    // Mass of inert part, kg    ?             +      -      -     +
   double &p_Gs,    // Gibbs energy of reactive subsystem (J)  -      -      +     +
   double &p_Hs,    // Enthalpy of reactive subsystem (J)      -      -      +     +
   double &p_Hi,    // Enthalpy of inert subsystem (J) ?       +      -      -     +
   double &p_IC,    // Effective molal aq ionic strength           -      -      +     +
   double &p_pH,    // pH of aqueous solution                      -      -      +     +
   double &p_pe,    // pe of aqueous solution                      -      -      +     +
   double &p_Eh,    // Eh of aqueous solution, V                   -      -      +     +
   double &p_denW,
   double &p_denWg, // Density of H2O(l) and steam at T,P      -      -      +     +
   double &p_epsW,
   double &p_epsWg, // Diel.const. of H2O(l) and steam at T,P  -      -      +     +
//  FMT variables (units need dimensionsless form)
   double &p_Tm,    // actual total simulation time
   double &p_dt,    // actual time step
   double &p_dt1,   // priveous time step
   double &p_ot,    // output time control for postprocessing
   double &p_Vt,    // total volume of node (voxel) = dx*dy*dz, m**3
   double &p_eps,   // effective (actual) porosity normalized to 1
   double &p_Km,    // actual permeability, m**2
   double &p_Kf,    // actual DARCY`s constant, m**2/s
   double &p_S,	    // specific storage coefficient, dimensionless
   double &p_Tr,    // transmissivity m**2/s
   double &p_h,	    // actual hydraulic head (hydraulic potential), m
   double &p_rho,   // actual carrier density for density driven flow, g/cm**3
   double &p_al,    // specific longitudinal dispersivity of porous media, m
   double &p_at,    // specific transversal dispersivity of porous media, m
   double &p_av,    // specific vertical dispersivity of porous media, m
   double &p_hDl,   // hydraulic longitudinal dispersivity, m**2/s, diffusities from chemical database
   double &p_hDt,   // hydraulic transversal dispersivity, m**2/s
   double &p_hDv,   // hydraulic vertical dispersivity, m**2/s
   double &p_nPe,   // node Peclet number, dimensionless
// Dynamic data - dimensions see in DATACH.H and DATAMT.H structures
// exchange of values occurs through lists of indices, e.g. xDC, xPH
   double  *p_xDC,    // DC mole amounts at equilibrium [nDCb]      -      -      +     +
   double  *p_gam,    // activity coeffs of DC [nDCb]               -      -      +     +
   double  *p_xPH,  // total mole amounts of phases [nPHb]          -      -      +     +
   double  *p_vPS,  // phase volume, cm3/mol        [nPSb]          -      -      +     +
   double  *p_mPS,  // phase (carrier) mass, g      [nPSb]          -      -      +     +
   double  *p_bPS,  // bulk compositions of phases  [nPSb][nICb]    -      -      +     +
   double  *p_xPA,  // amount of carrier in phases  [nPSb] ??       -      -      +     +
   double  *p_bIC,  // bulk mole amounts of IC[nICb]                +      +      -     -
   double  *p_rMB,  // MB Residuals from GEM IPM [nICb]             -      -      +     +
   double  *p_uIC,  // IC chemical potentials (mol/mol)[nICb]       -      -      +     +
  // What else?
   double  *p_dRes1,
   double  *p_dRes2
);

//      DATABR  *dBR;
//      DATABR  *(*dBR1);

int
main( int argc, char* argv[] )
{
     gstring multu_in = "";
     gstring chbr_in   = "";

      // from argv
      if (argc >= 2 )
        multu_in = argv[1];
      if (argc >= 3 )
        chbr_in = argv[2];
      int  c_to_i1[30];
      int  c_to_i2[30];
      for( int i=0; i<30; i++ )
      {
        c_to_i1[i]=multu_in[i];
        c_to_i2[i]=chbr_in[i];
      }
//      dBR = 0;

// cout <<  multu_in.c_str() << " " << chbr_in.c_str() << endl;

// Initial reading of arrays
      MAIF_START( 1, c_to_i1, c_to_i2, 0 );

//      DATABR
//      dBR = 0;
//      DATABR *(
//      dBR1 = &dBR;
// cout << " After MAIF_START " << endl;
//      TProfil::pm->multi->CopyTo( dBR1 );
// cout << " Before MAIF_CALC Mode=" << dBR->NodeStatusCH << " IT=" << dBR->IterDone << endl;

  short m_NodeHandle=0, m_NodeTypeHY=0, m_NodeTypeMT=0,
        m_NodeStatusFMT=0, m_NodeStatusCH=0,  m_IterDone=0;
  double m_T=0., m_P=0., m_Vs=0., m_Vi=0., m_Ms=0., m_Mi=0.,
        m_Gs=0., m_Hs=0., m_Hi=0., m_IC=0., m_pH=0., m_pe=0.,
        m_Eh=0., m_denW=0., m_denWg=0., m_epsW=0., m_epsWg=0., m_Tm=0.,
        m_dt=0., m_dt1=0., m_ot=0., m_Vt=0., m_eps=0., m_Km=0.,
        m_Kf=0., m_S=0., m_Tr=0., m_h=0., m_rho=0., m_al=0.,
        m_at=0., m_av=0., m_hDl=0., m_hDt=0., m_hDv=0., m_nPe=0.;
  double *m_xDC, *m_gam, *m_xPH, *m_vPS, *m_mPS,
        *m_bPS, *m_xPA, *m_bIC, *m_rMB, *m_uIC;
  double m_dRes1[4], m_dRes2[6];

  int nIC=0, nDC=0, nPH=0, nPS=0, ndXf=-1;
  short *xDC, *xIC, *xPH;

   DATACH  *dCH = TProfil::pm->multi->data_CH;
   DATABR  *dBR = TProfil::pm->multi->data_BR;

if( !dCH || !dBR )
   return 1;

   // Extracting data bridge dimensionalities
   nIC = dCH->nICb;
   nDC = dCH->nDCb;
   nPH = dCH->nPHb;
   nPS = dCH->nPSb;

// xDC = dCH->xDC;
// xIC = dCH->xIC;
// xPH = dCH->xPH;

// cout << endl << "nIC= " << nIC << "  nDC= " << nDC <<
//   "  nPH= " << nPH << "  nPS =" << nPS << endl;
    m_xDC = (double*)malloc( nDC*sizeof(double) );
    m_gam = (double*)malloc( nDC*sizeof(double) );
    m_xPH = (double*)malloc( nPH*sizeof(double) );
    m_vPS = (double*)malloc( nPS*sizeof(double) );
    m_mPS = (double*)malloc( nPS*sizeof(double) );
    m_bPS = (double*)malloc( nDC*nPS*sizeof(double) );
    m_xPA = (double*)malloc( nPS*sizeof(double) );
    m_bIC = (double*)malloc( nIC*sizeof(double) );
    m_rMB = (double*)malloc( nIC*sizeof(double) );
    m_uIC = (double*)malloc( nIC*sizeof(double) );

    MAIF_CALC( ndXf, // Fortran index; negative means readonly
        m_NodeHandle, m_NodeTypeHY, m_NodeTypeMT,
        m_NodeStatusFMT, m_NodeStatusCH, m_IterDone,
        m_T, m_P, m_Vs, m_Vi, m_Ms, m_Mi,
        m_Gs, m_Hs, m_Hi, m_IC, m_pH, m_pe,
        m_Eh, m_denW, m_denWg, m_epsW, m_epsWg, m_Tm,
        m_dt, m_dt1, m_ot, m_Vt, m_eps, m_Km,
        m_Kf, m_S, m_Tr, m_h, m_rho, m_al,
        m_at, m_av, m_hDl, m_hDt, m_hDv, m_nPe,
        m_xDC, m_gam, m_xPH, m_vPS, m_mPS,
        m_bPS, m_xPA, m_bIC, m_rMB, m_uIC,
        m_dRes1, m_dRes2 );

// Printouts here

// cout << endl << "Read: " << m_NodeHandle << " " <<  m_NodeTypeHY << " "
//     << m_NodeTypeMT << " " << m_NodeStatusFMT << " " << m_NodeStatusCH
//     << " " << m_IterDone << endl;

// Something else?




// Finished!

    free( m_xDC );
    free( m_gam );
    free( m_xPH );
    free( m_vPS );
    free( m_mPS );
    free( m_bPS );
    free( m_xPA );
    free( m_bIC );
    free( m_rMB );
    free( m_uIC );

    return 0;
}

//--------------------- End of main.cpp ---------------------------

