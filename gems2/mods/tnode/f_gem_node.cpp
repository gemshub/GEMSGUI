#include <iomanip>

#include "node.h"


extern "C"
int  __stdcall  F_GEM_INIT( char* string_, unsigned int length_ )
{

   string_[length_] = '\0';
  
   // gstring string_cto_i1c( string_, 0, length_);
	
	// Creating TNode structure accessible trough node pointer
    TNode* node = new TNode();

   // Here we read the files needed as input for initializing GEMIPM2K
   // The easiest way to prepare them is to use GEMS-PSI code (GEM2MT module)
   if( node->GEM_init( string_ /*string_cto_i1c.c_str()*/ ) )
       return 1;  // error occured during reading the files

   return 0;
}


// Parameter list may be extended in future with other DCH elements
extern "C"
int  __stdcall  F_GEM_GET_DCH( int& p_nICb, int& p_nDCb, float* p_A )
{

   p_nICb = TNode::na->pCSD()->nIC;
   p_nDCb = TNode::na->pCSD()->nDC;
    for( int i=0; i<p_nICb; i++ )
       for( int j=0; j<p_nDCb; j++ )
		   p_A[j*p_nICb+i] = nodeCH_A(j, i);
   // memcpy( p_A, TNode::na->pCSD()->A, p_nICb*p_nDCb*sizeof(float) );
   return 0;
}

//-------------------------------------------------------------------------
extern "C"
int  __stdcall   F_GEM_READ_NODE( char* string_, unsigned int length_,
   int& p_NodeHandle,    // Node identification handle
   int& p_NodeTypeHY,    // Node type (hydraulic); see typedef NODETYPE
   int& p_NodeTypeMT,    // Node type (mass transport); see typedef NODETYPE
   int& p_NodeStatusFMT, // Node status code FMT; see typedef NODECODEFMT
   int& p_NodeStatusCH,  // Node status code CH;  see typedef NODECODECH
   int& p_IterDone,      // Number of iterations performed by IPM (if not need GEM)
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
//  FMT variables (units need dimensionsless form)
   double &p_Tm,    // actual total simulation time
   double &p_dt,    // actual time step
   double &p_dt1,   // priveous time step
   double &p_Vt,    // total volume of node (voxel) = dx*dy*dz, m**3
   double &p_vp,	// advection velocity (in pores) in this node
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
   double &p_nto,   // tortuosity factor
// Dynamic data - dimensions see in DATACH.H and DATAMT.H structures
// exchange of values occurs through lists of indices, e.g. xDC, xPH
   double  *p_bIC,  // bulk mole amounts of IC[nICb]                +      +      -     -
   double  *p_rMB,  // MB Residuals from GEM IPM [nICb]             -      -      +     +
   double  *p_uIC,  // IC chemical potentials (mol/mol)[nICb]       -      -      +     +
   double  *p_xDC,    // DC mole amounts at equilibrium [nDCb]      -      -      +     +
   double  *p_gam,    // activity coeffs of DC [nDCb]               -      -      +     +
   double  *p_dul,  // upper kinetic restrictions [nDCb]            +      +      -     -
   double  *p_dll,  // lower kinetic restrictions [nDCb]            +      +      -     -
   double  *p_aPH,  // Specific surface areas of phases (m2/g)      +      +      -     -
   double  *p_xPH,  // total mole amounts of phases [nPHb]          -      -      +     +
   double  *p_vPS,  // phase volume, cm3/mol        [nPSb]          -      -      +     +
   double  *p_mPS,  // phase (carrier) mass, g      [nPSb]          -      -      +     +
   double  *p_bPS,  // bulk compositions of phases  [nPSb][nICb]    -      -      +     +
   double  *p_xPA,  // amount of carrier in phases  [nPSb] ??       -      -      +     +
  // What else?
   double  *p_dRes1
)
{
   short NodeHandle, NodeTypeHY,  NodeTypeMT, NodeStatusFMT, NodeStatusCH, IterDone;      

   NodeHandle = p_NodeHandle;
   NodeTypeHY = p_NodeTypeHY;
   NodeTypeMT = p_NodeTypeMT;
   NodeStatusFMT = p_NodeStatusFMT;
   NodeStatusCH = p_NodeStatusCH;
   IterDone =p_IterDone;      

	string_[length_] = '\0';
	// gstring dbr_file_name( string_, 0, length_);
    
 // Read DATABR structure from file
    TNode::na->GEM_read_dbr( false, string_ /*dbr_file_name.c_str()*/ );

 // re-calculating equilibrium by calling GEMIPM
 //	 short NodeStatusCH = TNode::na->GEM_run();
 //     if( !( NodeStatusCH == OK_GEM_AIA || NodeStatusCH == OK_GEM_PIA ) )
 //        return 1;

 // Extracting chemical data into FMT part
     TNode::na->GEM_restore_MT( NodeHandle, NodeStatusCH, p_T,
                                p_P, p_Vs, p_Ms, p_bIC, p_dul, p_dll, p_aPH );
      
  // Extracting GEMIPM output data to FMT part
     TNode::na->GEM_to_MT( NodeHandle, NodeStatusCH, IterDone,
                           p_Vs, p_Ms, p_Gs, p_Hs, p_IC, p_pH, p_pe, p_Eh,
                           p_rMB, p_uIC, p_xDC, p_gam, p_xPH, p_vPS, p_mPS, p_bPS, p_xPA );

 p_NodeHandle = NodeHandle;
 p_NodeTypeHY = NodeTypeHY;
 p_NodeTypeMT = NodeTypeMT;
 p_NodeStatusFMT = NodeStatusFMT;
 p_NodeStatusCH = NodeStatusCH;
 p_IterDone =IterDone;      

 return 0;
}

//-------------------------------------------------------------------------

extern "C"
int  __stdcall   F_GEM_CALC_NODE( 
   int& p_NodeHandle,    // Node identification handle
   int& p_NodeTypeHY,    // Node type (hydraulic); see typedef NODETYPE
   int& p_NodeTypeMT,    // Node type (mass transport); see typedef NODETYPE
   int& p_NodeStatusFMT, // Node status code FMT; see typedef NODECODEFMT
   int& p_NodeStatusCH,  // Node status code CH;  see typedef NODECODECH
   int& p_IterDone,      // Number of iterations performed by IPM (if not need GEM)
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
//  FMT variables (units need dimensionsless form)
//   double &p_Tm,    // actual total simulation time
//   double &p_dt,    // actual time step
//   double &p_dt1,   // priveous time step
//   double &p_Vt,    // total volume of node (voxel) = dx*dy*dz, m**3
//   double &p_vp,	// advection velocity (in pores) in this node
//   double &p_eps,   // effective (actual) porosity normalized to 1
//   double &p_Km,    // actual permeability, m**2
//   double &p_Kf,    // actual DARCY`s constant, m**2/s
//   double &p_S,	    // specific storage coefficient, dimensionless
//   double &p_Tr,    // transmissivity m**2/s
//   double &p_h,	    // actual hydraulic head (hydraulic potential), m
//   double &p_rho,   // actual carrier density for density driven flow, g/cm**3
//   double &p_al,    // specific longitudinal dispersivity of porous media, m
//   double &p_at,    // specific transversal dispersivity of porous media, m
//   double &p_av,    // specific vertical dispersivity of porous media, m
//   double &p_hDl,   // hydraulic longitudinal dispersivity, m**2/s, diffusities from chemical database
//   double &p_hDt,   // hydraulic transversal dispersivity, m**2/s
//   double &p_hDv,   // hydraulic vertical dispersivity, m**2/s
//   double &p_nto,   // tortuosity factor
// Dynamic data - dimensions see in DATACH.H and DATAMT.H structures
// exchange of values occurs through lists of indices, e.g. xDC, xPH
   double  *p_bIC,  // bulk mole amounts of IC[nICb]                +      +      -     -
   double  *p_rMB,  // MB Residuals from GEM IPM [nICb]             -      -      +     +
   double  *p_uIC,  // IC chemical potentials (mol/mol)[nICb]       -      -      +     +
   double  *p_xDC,    // DC mole amounts at equilibrium [nDCb]      -      -      +     +
   double  *p_gam,    // activity coeffs of DC [nDCb]               -      -      +     +
   double  *p_dul,  // upper kinetic restrictions [nDCb]           +      +      -     -
   double  *p_dll,  // lower kinetic restrictions [nDCb]           +      +      -     -
   double  *p_aPH,  // Specific surface areas of phases (m2/g)      +      +      -     -
   double  *p_xPH,  // total mole amounts of phases [nPHb]          -      -      +     +
   double  *p_vPS,  // phase volume, cm3/mol        [nPSb]          -      -      +     +
   double  *p_mPS,  // phase (carrier) mass, g      [nPSb]          -      -      +     +
   double  *p_bPS,  // bulk compositions of phases  [nPSb][nICb]    -      -      +     +
   double  *p_xPA  // amount of carrier in phases  [nPSb] ??       -      -      +     +
  // What else?
  // double  *p_dRes1
 )
{
  int iRet = 0;

  short NodeHandle, NodeStatusCH, IterDone;      

  NodeHandle = p_NodeHandle;
  NodeStatusCH = p_NodeStatusCH;
  IterDone = p_IterDone;      
  // (2) ----------------------------------------------
  // Work loop for the coupled FMT-GEM modelling

  // Setting input data for GEMIPM
   TNode::na->GEM_from_MT( NodeHandle, NodeStatusCH,
             p_T, p_P, p_Vs, p_Ms, p_bIC, p_dul, p_dll,  p_aPH );

 // Calling GEMIPM calculation
   iRet = TNode::na->GEM_run( );
   if( !( iRet == OK_GEM_AIA || iRet == OK_GEM_PIA ) )
   { 
	  return 1;
   }

  // Extracting GEMIPM output data to FMT part
   TNode::na->GEM_to_MT( NodeHandle, NodeStatusCH, IterDone,
       p_Vs, p_Ms, p_Gs, p_Hs, p_IC, p_pH, p_pe, p_Eh, p_rMB, p_uIC,
       p_xDC, p_gam, p_xPH, p_vPS, p_mPS, p_bPS, p_xPA  );

 p_NodeHandle = NodeHandle;
 p_NodeStatusCH = NodeStatusCH;
 p_IterDone =IterDone;      

/**************************************************************
  int ii;
  int nIC, nDC, nPH;
 // Extracting data bridge dimensionalities
   nIC = TProfil::pm->multi->CSD->nICb;
   nDC = TProfil::pm->multi->CSD->nDCb;
   nPH = TProfil::pm->multi->CSD->nPHb;

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

  return 0;
}

