//-------------------------------------------------------------------
// Data Bridge Template structure   Draft variant 3 30/11/2003
//-------------------------------------------------------------------
//
//       CH: chemical structure GEMS
//      FMT: fluid mass transport
//

#ifndef _DataBr_H_
#define _DataBr_H_

typedef struct
{  // DATABR - template for node data bridge instances
   short
     NodeHandle,    // Node identification handle
     NodeTypeHY,    // Node type (hydraulic); see typedef NODETYPE
     NodeTypeMT,    // Node type (mass transport); see typedef NODETYPE
     NodeStatusFMT, // Node status code FMT; see typedef NODECODEFMT 
     NodeStatusCH,  // Node status code CH;  see typedef NODECODECH
     IterDone;      // Number of iterations performed by IPM (if not need GEM)
     
/*  see DATACH structure
   unsigned short
    nICb,       // number of stoichiometry units (<= nIC) used in the data bridge  
    nDCb,      	// number of DC (chemical species, <= nDC) used in the data bridge
    nPHb,     	// number of phases (<= nPH) used in the data bridge
    nPSb;       // number of multicomponent phases (<= nPS) used in the data bridge
*/

//      Usage of this variable:                          MT-DB DB-GEM GEM-DB DB-MT       
   double
// Chemical scalar variables
    T,   	// Temperature T, K                        +      +      -     -
    P,   	// Pressure P, bar                         +      +      -     -
    Vs,         // Volume V of reactive subsystem, cm3     -      -      +     +
    Vi,         // Volume of inert subsystem  ?            +      -      -     +
    Ms,         // Mass of reactive subsystem, kg          +      +      -     -
    Mi,         // Mass of inert part, kg    ?             +      -      -     +

    Gs,         // Gibbs energy of reactive subsystem (J)  -      -      +     +
    Hs, 	// Enthalpy of reactive subsystem (J)      -      -      +     +
    Hi,         // Enthalpy of inert subsystem (J) ?       +      -      -     +

    IC,     // Effective molal aq ionic strength           -      -      +     +
    pH,     // pH of aqueous solution                      -      -      +     +
    pe,     // pe of aqueous solution                      -      -      +     +
    Eh,     // Eh of aqueous solution, V                   -      -      +     +
    denW,denWg, // Density of H2O(l) and steam at T,P      -      -      +     +
    epsW,epsWg, // Diel.const. of H2O(l) and steam at T,P  -      -      +     +

//  FMT variables (units need dimensionsless form)

    Tm,          // actual total simulation time
    dt,         // actual time step
    dt1,        // priveous time step
    ot,		// output time control for postprocessing
    Vt,		// total volume of node (voxel) = dx*dy*dz, m**3
    eps,	// effective (actual) porosity normalized to 1
    Km,		// actual permeability, m**2
    Kf,		// actual DARCY`s constant, m**2/s
    S,		// specific storage coefficient, dimensionless
    Tr,         // transmissivity m**2/s
    h,		// actual hydraulic head (hydraulic potential), m
    rho,	// actual carrier density for density driven flow, g/cm**3
    al,		// specific longitudinal dispersivity of porous media, m
    at,		// specific transversal dispersivity of porous media, m
    av,		// specific vertical dispersivity of porous media, m
    hDl,	// hydraulic longitudinal dispersivity, m**2/s, diffusities from chemical database
    hDt,	// hydraulic transversal dispersivity, m**2/s			 	
    hDv,	// hydraulic vertical dispersivity, m**2/s
    nto;	// tortuosity
   
// Dynamic data - dimensions see in DATACH.H and DATAMT.H structures
// exchange of values occurs through lists of indices, e.g. xDC, xPH 
    
   double
   // DC (species) in reactive subsystem
    *xDC,    // DC mole amounts at equilibrium [nDCb]      -      -      +     +
    *gam,    // activity coeffs of DC [nDCb]               -      -      +     +

   // Phases in reactive subsystem
    *xPH,  // total mole amounts of phases [nPHb]          -      -      +     +

    *vPS,  // phase volume, cm3/mol        [nPSb]          -      -      +     +
    *mPS,  // phase (carrier) mass, g      [nPSb]          -      -      +     +
    *bPS,  // bulk compositions of phases  [nPSb][nICb]    -      -      +     +
    *xPA,  // amount of carrier in phases  [nPSb] ??       -      -      +     +

    *dul,  // upper kinetic restrictions [nDCb]           +      +      -     -
    *dll,  // lower kinetic restrictions [nDCb]           +      +      -     -
  // IC (stoichiometry units)
    *bIC,  // bulk mole amounts of IC[nICb]                +      +      -     -
    *rMB,  // MB Residuals from GEM IPM [nICb]             -      -      +     +
    *uIC,  // IC chemical potentials (mol/mol)[nICb]       -      -      +     +

  // What else? 
    *dRes1,
    *dRes2;
}
DATABR;

typedef DATABR*  DATABRPTR;

typedef enum {  // NodeStatus codes GEMS
 NEED_GEM_AIA = 1,   //To calculate with simplex IA
 OK_GEM_AIA   = 2,   // OK calculated from simplex IA
 BAD_GEM_AIA  = 3,   // Bad result from simplex IA
 ERR_GEM_AIA  = 4,   // Failed to calculated from simplex IA
 NEED_GEM_PIA = 5,   //To calculate without simplex from previous solution
 OK_GEM_PIA   = 6,   // OK calculated without simplex from previous solution
 BAD_GEM_PIA  = 7,   // Bad result without simplex from previous solution
 ERR_GEM_PIA  = 8,   // Failed to calculated without simplex
 TERROR_GEM   = 9    // Terminal error GemIPM
} NODECODECH;


typedef enum {  // NodeStatus codes FluidMassTransport
 
 Initial_RUN   = 1,
 OK_Hydraulic  = 2,
 BAD_Hydraulic = 3,  // unsufficient convergence
 OK_Transport  = 4,
 BAD_Transport = 5,  // unsufficient convergence
 NEED_RecalcMT = 6,
 OK_MassBal    = 7,
 OK_RecalcPar  = 8,
 Bad_Recalc    = 9,
 OK_Time       = 10
} NODECODEFMT;
 

typedef enum {  // NodeType codes Hydraulic/masstransport
 normal       = 0, // normal node
 border1      = 1, // 1st order node boundary condition (NBC)
 border2      = 2, // second order NBC
 border3      = 3, // third order NBC
 initital     = 4  // initial conditions (e.g. input time depended functions)
} NODETYPE;


#endif   //_DataBr_h

