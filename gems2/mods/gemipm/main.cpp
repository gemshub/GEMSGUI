//masstransport
//-------------------------------------------------------------------

//#include <iostream>

#include "verror.h"
#include "m_param.h"
#include "gdatastream.h"

extern "C" int __stdcall MAIF_START( int nNodes,
   int  c_to_i1[30], int c_to_i2[30], int *nodeTypes );

extern "C" int __stdcall MAIF_CALC( int iNode,
   short& p_NodeHandle,    // Node identification handle
   short& p_NodeTypeHY,    // Node type (hydraulic); see typedef NODETYPE
   short& p_NodeTypeMT,    // Node type (mass transport); see typedef NODETYPE
   short& p_NodeStatusFMT, // Node status code FMT; see typedef NODECODEFMT
   short& p_NodeStatusCH,  // Node status code CH;  see typedef NODECODECH
   short& p_IterDone,      // Number of iterations performed by IPM (if not need GEM)
// Chemical scalar variables
   double& p_T,     // Temperature T, K                        +      +      -     -
   double& p_P,     // Pressure P, bar                         +      +      -     -
   double& p_Vs,    // Volume V of reactive subsystem, cm3     -      -      +     +
   double& p_Vi,    // Volume of inert subsystem  ?            +      -      -     +
   double& p_Ms,    // Mass of reactive subsystem, kg          +      +      -     -
   double& p_Mi,    // Mass of inert part, kg    ?             +      -      -     +
   double& p_Gs,    // Gibbs energy of reactive subsystem (J)  -      -      +     +
   double& p_Hs,    // Enthalpy of reactive subsystem (J)      -      -      +     +
   double& p_Hi,    // Enthalpy of inert subsystem (J) ?       +      -      -     +
   double& p_IC,    // Effective molal aq ionic strength           -      -      +     +
   double& p_pH,    // pH of aqueous solution                      -      -      +     +
   double& p_pe,    // pe of aqueous solution                      -      -      +     +
   double& p_Eh,    // Eh of aqueous solution, V                   -      -      +     +
   double& p_denW,
   double& p_denWg, // Density of H2O(l) and steam at T,P      -      -      +     +
   double& p_epsW,
   double& p_epsWg, // Diel.const. of H2O(l) and steam at T,P  -      -      +     +
//  FMT variables (units need dimensionsless form)
   double& p_Tm,    // actual total simulation time
   double& p_dt,    // actual time step
   double& p_dt1,   // priveous time step
   double& p_ot,    // output time control for postprocessing
   double& p_Vt,    // total volume of node (voxel) = dx*dy*dz, m**3
   double& p_eps,   // effective (actual) porosity normalized to 1
   double& p_Km,    // actual permeability, m**2
   double& p_Kf,    // actual DARCY`s constant, m**2/s
   double& p_S,	    // specific storage coefficient, dimensionless
   double& p_Tr,    // transmissivity m**2/s
   double& p_h,	    // actual hydraulic head (hydraulic potential), m
   double& p_rho,   // actual carrier density for density driven flow, g/cm**3
   double& p_al,    // specific longitudinal dispersivity of porous media, m
   double& p_at,    // specific transversal dispersivity of porous media, m
   double& p_av,    // specific vertical dispersivity of porous media, m
   double& p_hDl,   // hydraulic longitudinal dispersivity, m**2/s, diffusities from chemical database
   double& p_hDt,   // hydraulic transversal dispersivity, m**2/s
   double& p_hDv,   // hydraulic vertical dispersivity, m**2/s
   double& p_nPe,   // node Peclet number, dimensionless
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

      DATABR  *dBR;
      DATABR  *(*dBR1);

int
main(int argc, char* argv[])
{
     gstring multu_in = "input_multi.txt";
     gstring chbr_in   = "mtr_data.txt";

      // from argv
      if (argc >= 2 )
        multu_in = argv[1];
      if (argc >= 3 )
        chbr_in = argv[2]; \
      int  c_to_i1[30];
      int  c_to_i2[30];
      for( int i=0; i<30; i++ )
      {
        c_to_i1[i]=multu_in[i];
        c_to_i2[i]=chbr_in[i];
      }

      MAIF_START( 3, c_to_i1, c_to_i2, 0 );
//      DATABR
      dBR = 0;
//      DATABR *(
      dBR1 = &dBR;

      TProfil::pm->multi->CopyTo(dBR1);

      MAIF_CALC( 0,
        dBR->NodeHandle, dBR->NodeTypeHY, dBR->NodeTypeMT,
        dBR->NodeStatusFMT, dBR->NodeStatusCH,  dBR->IterDone,
        dBR->T, dBR->P, dBR->Vs, dBR->Vi, dBR->Ms, dBR->Mi,
        dBR->Gs, dBR->Hs, dBR->Hi, dBR->IC, dBR->pH, dBR->pe,
        dBR->Eh, dBR->denW, dBR->denWg, dBR->epsW, dBR->epsWg, dBR->Tm,
        dBR->dt, dBR->dt1, dBR->ot, dBR->Vt, dBR->eps, dBR->Km,
        dBR->Kf, dBR->S, dBR->Tr, dBR->h, dBR->rho, dBR->al,
        dBR->at, dBR->av, dBR->hDl, dBR->hDt, dBR->hDv, dBR->nPe,
        dBR->xDC,dBR->gam, dBR->xPH, dBR->vPS, dBR->mPS,
        dBR->bPS,dBR->xPA, dBR->bIC, dBR->rMB, dBR->uIC,
        dBR->dRes1, dBR->dRes2);

/*      MAIF_CALC( 1,
        dBR->NodeHandle, dBR->NodeTypeHY, dBR->NodeTypeMT,
        dBR->NodeStatusFMT, dBR->NodeStatusCH,  dBR->IterDone,
        dBR->T, dBR->P, dBR->Vs, dBR->Vi, dBR->Ms, dBR->Mi,
        dBR->Gs, dBR->Hs, dBR->Hi, dBR->IC, dBR->pH, dBR->pe,
        dBR->Eh, dBR->denW, dBR->denWg, dBR->epsW, dBR->epsWg, dBR->Tm,
        dBR->dt, dBR->dt1, dBR->ot, dBR->Vt, dBR->eps, dBR->Km,
        dBR->Kf, dBR->S, dBR->Tr, dBR->h, dBR->rho, dBR->al,
        dBR->at, dBR->av, dBR->hDl, dBR->hDt, dBR->hDv, dBR->nPe,
        dBR->xDC,dBR->gam, dBR->xPH, dBR->vPS, dBR->mPS,
        dBR->bPS,dBR->xPA, dBR->bIC, dBR->rMB, dBR->uIC,
        dBR->dRes1, dBR->dRes2);
      MAIF_CALC( 2,
        dBR->NodeHandle, dBR->NodeTypeHY, dBR->NodeTypeMT,
        dBR->NodeStatusFMT, dBR->NodeStatusCH,  dBR->IterDone,
        dBR->T, dBR->P, dBR->Vs, dBR->Vi, dBR->Ms, dBR->Mi,
        dBR->Gs, dBR->Hs, dBR->Hi, dBR->IC, dBR->pH, dBR->pe,
        dBR->Eh, dBR->denW, dBR->denWg, dBR->epsW, dBR->epsWg, dBR->Tm,
        dBR->dt, dBR->dt1, dBR->ot, dBR->Vt, dBR->eps, dBR->Km,
        dBR->Kf, dBR->S, dBR->Tr, dBR->h, dBR->rho, dBR->al,
        dBR->at, dBR->av, dBR->hDl, dBR->hDt, dBR->hDv, dBR->nPe,
        dBR->xDC,dBR->gam, dBR->xPH, dBR->vPS, dBR->mPS,
        dBR->bPS,dBR->xPA, dBR->bIC, dBR->rMB, dBR->uIC,
        dBR->dRes1, dBR->dRes2);
*/
    return 1;
}

//--------------------- End of main.cpp ---------------------------

