#include <iomanip>

#include "m_param.h"

int  NewNodeArray( int &sizeN, int &sizeM, int &sizeK,
                   const char*  MULTI_filename,
                   const char *ipmfiles_lst_name, int *nodeTypes );
int  NodeCalcGEM( int  &readF, // negative means read only
   int &indN,  // fortran index; 0 working only with work structure
   int &indM,  // fortran index; 0 working only with work structure
   int &indK,  // fortran index; 0 working only with work structure
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
   double  *p_dul,  // upper kinetic restrictions [nDCb]           +      +      -     -
   double  *p_dll,  // lower kinetic restrictions [nDCb]           +      +      -     -
   double  *p_bIC,  // bulk mole amounts of IC[nICb]                +      +      -     -
   double  *p_rMB,  // MB Residuals from GEM IPM [nICb]             -      -      +     +
   double  *p_uIC,  // IC chemical potentials (mol/mol)[nICb]       -      -      +     +
  // What else?
   double  *p_dRes1,
   double  *p_dRes2
);
//-------------------------------------------------------------------
//-------------------------------------------------------------------

extern "C"
int /* __stdcall */ MAIF_START( int &sizeN, int &sizeM, int &sizeK,
  int  c_to_i1[30], int c_to_i2[30], int *nodeTypes )
{

    int i;
    char c_to_ic[30];
    char string_cto_i1c[31];
    char string_cto_i2c[31];

    for (i=0; i<=29;i++)
    {
      c_to_ic[i] = char (c_to_i1[i]);
      string_cto_i1c[i]= c_to_ic[i];
    }
    string_cto_i1c[30]= '\0';          // end string in cpp

    for (i=0; i<=29;i++)
    {
      	c_to_ic[i] = char (c_to_i2[i]);
        string_cto_i2c[i]= c_to_ic[i];
    }
    string_cto_i2c[30]= '\0';          // end string in cpp

    int iRet =  NewNodeArray(
         sizeN, sizeM, sizeK, string_cto_i1c, string_cto_i2c, nodeTypes);
    return iRet;
}


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
extern "C"
int /* __stdcall */  MAIF_CALC( int  readF, // negative means read only
   int indN,  // fortran index; 0 working only with work structure
   int indM,  // fortran index; 0 working only with work structure
   int indK,  // fortran index; 0 working only with work structure
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
   double  *p_dul,  // upper kinetic restrictions [nDCb]           +      +      -     -
   double  *p_dll,  // lower kinetic restrictions [nDCb]           +      +      -     -
   double  *p_bIC,  // bulk mole amounts of IC[nICb]                +      +      -     -
   double  *p_rMB,  // MB Residuals from GEM IPM [nICb]             -      -      +     +
   double  *p_uIC,  // IC chemical potentials (mol/mol)[nICb]       -      -      +     +
  // What else?
   double  *p_dRes1,
   double  *p_dRes2
)
{
  int iRet = NodeCalcGEM( readF, indN, indM,  indK,
            p_NodeHandle, p_NodeTypeHY, p_NodeTypeMT,
            p_NodeStatusFMT, p_NodeStatusCH, p_IterDone,
            p_T, p_P, p_Vs, p_Vi, p_Ms, p_Mi, p_Gs, p_Hs,
            p_Hi, p_IC, p_pH, p_pe, p_Eh, p_denW, p_denWg, p_epsW,
            p_epsWg, p_Tm, p_dt, p_dt1, p_ot, p_Vt, p_eps, p_Km,
            p_Kf, p_S, p_Tr, p_h, p_rho, p_al, p_at, p_av,
            p_hDl, p_hDt, p_hDv, p_nPe,
            p_xDC, p_gam, p_xPH, p_vPS, p_mPS, p_bPS, p_xPA,
            p_dul, p_dll,
            p_bIC, p_rMB, p_uIC, p_dRes1, p_dRes2  );

/**************************************************************
  int ii;
  int nIC, nDC, nPH;
 // Extracting data bridge dimensionalities
   nIC = TProfil::pm->multi->data_CH->nICb;
   nDC = TProfil::pm->multi->data_CH->nDCb;
   nPH = TProfil::pm->multi->data_CH->nPHb;

  fstream f_log("MAIF_CALC.txt", ios::out|ios::app );

  f_log << "Node = " <<  p_NodeHandle << "( " << readF << ", " <<
         indN << ", " << indM << ", " <<  indK << " )";
  f_log << "  NodeStatus = " <<  p_NodeStatusCH;
  f_log << "  ReturnStatus = " <<  iRet;
  f_log << "  IterDone = " <<  p_IterDone << endl;

  f_log << "p_xDC" << endl;
  for(ii=0; ii<nDC; ii++ )
    f_log << setprecision(15) << scientific << p_xDC[ii]<< ", ";
  f_log <<  endl;

  f_log << "p_gam" << endl;
  for(ii=0; ii<nDC; ii++ )
    f_log << setprecision(15) << scientific << p_gam[ii]<< ", ";
  f_log <<  endl;

  f_log << "p_xPH" << endl;
  for(ii=0; ii<nPH; ii++ )
    f_log << setprecision(15) << scientific << p_xPH[ii]<< ", ";
  f_log <<  endl;

  f_log << "p_uIC" << endl;
  for(ii=0; ii<nIC; ii++ )
    f_log << setprecision(15) << scientific << p_uIC[ii]<< ", ";
  f_log <<  endl;

  f_log << "p_rMB" << endl;
  for(ii=0; ii<nIC; ii++ )
    f_log << setprecision(15) << scientific << p_rMB[ii]<< ", ";
  f_log <<  endl;

  f_log <<  endl;
  f_log <<  endl;

//*****************************************************************/

  return iRet;

}


