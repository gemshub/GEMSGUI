//-------------------------------------------------------------------
Data Bridge Chemistry structure  (draft variant 2)
//-------------------------------------------------------------------
//
#ifndef _DataCh_H_
#define _DataCh_H_

typedef struct
{  // Structure DataCH
   // Dimensionalities
  unsigned short 
   // these dimensionalities should be the same as in MULTI (GEM IPM)
    nIC,        // total number of stoichiometry units in reactive part 
    nDC,      	// total number of DC (chemical species) in reactive part
    nPH,       	// total number of phases included into the IPM problem
    nPS,        // number of multicomponent phases <= nPH
    nTp,        // Number of temperature points in TD arrays
    nPp,        // Number of pressure points in TD arrays
    uRes1,
    uRes2,

  // These dimensionalities define sizes of dynamic data in DATABT structure!!!
  // Needed to reduce on storage demand for data bridge instances (nodes)!
  // Connection occurs through xIC, xPH and xDC lists!
    nICb,       // number of stoichiometry units (<= nIC) used in the data bridge  
    nDCb,      	// number of DC (chemical species, <= nDC) used in the data bridge
    nPHb,     	// number of phases (<= nPH) used in the data bridge
    nPSb,       // number of multicomponent phases (<= nPS) used in the data bridge
    uRes3,
    uRes4,

// lists, vectors and matrices
    *nDCinPH,  // number of DC included into each phase [nPH]

// These lists of indices connect the DATABR arrays with this structure
    *xIC,   // ICNL indices in DATABR IC vectors [nICb]
    *xDC,   // DCNL indices in DATABR DC list [nDCb]
    *xPH;   // PHNL indices in DATABR phase vectors [nPHb]

  float 
    *A;     // DC stoichiometry matrix A composed of a_ij [nIC][nDC]
            // float reduces storage demand here 
  double 
    Tmin, Tmax, Tstep, Ttol, // Temperature T, K, min.,-max., step, tolerance
    Pmin, Pmax, Pstep, Ptol, // Pressure P, bar, min.,-max., step, tolerance
    dRes1, dRes,

// Data vectors
// IC - related values
    *ICmm,   // IC atomic (molar) mass, g/mole [nIC]

// DC - related values  
    *DCmm,   // DC molar mass, g/mole [nDC]
// Thermodynamic data
// Require Lagrange extrapolation subroutine to extract data for given P,T    
    *G0,    // standard molar Gibbs energy [nDC, nPp, nTp]
    *V0,    // standard molar volume [nDC, nPp, nTp]
    *H0,    // standard molar enthalpy, reserved [nDC, nPp, nTp]
    *Cp0,   // st. molar heat capacity, reserved [nDC, nPp, nTp]

// Phase-related data 
    *Aalp;  // specific surface areas of phases in m2/g [nPH] 

// Name lists
 char
  (*ICNL)[MaxICN], // List of IC names in the system [nIC]
  (*DCNL)[MaxDCN], // List of DC names in the system [nDC]
  (*PHNL)[MaxPHN], // List of phase names in the system [nPH]

// Class code lists
    *ccIC,   // Class codes of IC [nIC], see  ICL_CLASSES
    *ccDC,   // Class codes of DC [nDC]  see  DCL_CLASSES 
    *ccDCW,  // Generic codes for DC [nDC], see SolDCLcodes  ??? 
    *ccPH;   // Class codes of phases [nPH], see PHL_CLASSES 
}
DATACH;

const unsigned int
    MaxICN =      6,
    MaxDCN =      16,
    MaxPHN =      16;

/* Work DC classifier codes  ccDCW ??? */
enum SolDCLcodes {
    DCl_SINGLE = 'U',        /* This DC is a single-component phase */
    DCl_SYMMETRIC = 'I',     /* This DC is in symmetric solution phase */
    DCl_ASYM_SPECIES = 'S', /*This is DC-solute(sorbate) in asymmetric phase */
    DCl_ASYM_CARRIER = 'W'  /*This is carrier(solvent) DC in asymmetric phase*/
};


typedef enum {  /* classes of IC*/
    IC_ELEMENT  =  'e',  // chemical element (except oxygen and hydrogen)
    IC_OXYGEN   =  'o',  // oxygen
    IC_HYDROGEN =  'h',  // hydrogen (natural mixture of isotopes) H
    IC_PROTIUM   = 'p',  // protium (reserved) Hp
    IC_DEYTERIUM = 'd',  // deuterium (reserved) D
    IC_TRITIUM  =  't',  // tritium (reserved) T
    IC_FORMULA  =  'f',  // formula unit (eg. for Sio - a symbol of SiO2)
    IC_METALION =  'm',  // metal ion (cation), reserved
    IC_LIGAND   =  'l',  // ligand (anion), reserved
    IC_ADDIT    =  'a',  // IC with unknown stoichiometry (eg: Hum, humic acid)
    IC_ISOTOPE  =  'i',  // isotope of chemical element from 1 to 250
    IC_OXYGEN16 =  'q',  // q  - oxygen 16O (reserved)
    IC_OXYGEN18 =  'r',  //  r  - oxygen 18O (reserved)
    IC_CHARGE   =  'z',  // z  - electrical charge
    IC_VOLUME   =  'v'   // volume
} ICL_CLASSES;

typedef enum {  /* Classifications of DC */
    /*Aqueous electrolyte phase:*/
    DC_AQ_PROTON   = 'T',      // hydrogen ion H+
    DC_AQ_ELECTRON = 'E',      // electron (as a DC)
    DC_AQ_SPECIES  = 'S',      // other aqueous species (ions, complexes and ion pairs)
    DC_AQ_SOLVENT  = 'W',      // water H2O (major solvent)
    DC_AQ_SOLVCOM  = 'L',      // other components of a solvent (eg. alcohol)
    /*Gas phase ( G code can be used for all gases; V,C,H,N codes are reserved
    for future use of the built-in equations of state in FGL module): */
    DC_GAS_COMP    = 'G',   // other gases
    DC_GAS_H2O     = 'V',   // H2O steam
    DC_GAS_CO2     = 'C',   // CO2 (carbon dioxide)
    DC_GAS_H2      = 'H',   // H2 hydrogen
    DC_GAS_N2      = 'N',   // N2 nitrogen
    /* Solid/liquid non-electrolyte multicomponent phases:*/
    DC_SOL_IDEAL   = 'I',   // end-member component with ideal behaviour
    DC_SOL_MINOR   = 'J',   // junior component (Henry's Law)
    DC_SOL_MAJOR   = 'M',   // major component (Raoult's Law)
    /* Sorption phases and poly(oligo)electrolytes */
    DC_SUR_CARRIER = 'Q',   // Principal end-member of solid carrier
    DC_SUR_MINAL   = 'P',   // Minor end-member of solid carrier
    DC_PEL_CARRIER = 'R',   // Carrier of poly(oligo)electrolyte for future use

    DC_SSC_A0 = '0', DC_SSC_A1 = '2', DC_SSC_A2 = '4', DC_SSC_A3 = '6',
    DC_SSC_A4 = '8', /* Strong surface complex on site type 0,1,2,3,4 - A plane */
    DC_WSC_A0 = '1', DC_WSC_A1 = '3', DC_WSC_A2 = '5', DC_WSC_A3 = '7',
    DC_WSC_A4 = '9', /* Weak surface complex on site type 0,1,2,3,4 - B plane */
    DC_IESC_A  = 'A', /* Strong exchange ion const-charge plane */
    DC_IEWC_B  = 'B', /* Weak exchange ion const-charge plane */

    /* Aliaces for 1-site model */
    DC_SUR_GROUP    = 'X',  /* Surface site A plane -> '0' */
    DC_SUR_COMPLEX = 'Y',  /* Strong sur. complex A plane -> '0' */
    DC_SUR_IPAIR   = 'Z',  /* Weak sur complex B plane -> '1' */

    /* Single-component phases:*/
    DC_SCP_CONDEN  = 'O',   // DC forming a single-component phase

    /* Here add some special classes for diffusion etc. */
    DCaquoCATION   = 'c', 
    DCaquoANION    = 'n', 
    DCaquoLIGAND   = 'l', 
    DCaquoCOMPLEX  = 'x',
    DCaquoIONPAIR  = 'p',
    DCaquoGAS      = 'g',
} DCL_CLASSES;

typedef enum {  /* Possible values */
    PH_AQUEL    = 'a',  // aqueous electrolyte
    PH_GASMIX   = 'g',  // mixture of gases
    PH_FLUID    = 'f',  // fluid phase
    PH_LIQUID   = 'l',  // non-electrolyte liquid (melt)
    PH_SORPTION = 'x',  // dilspersed solid with adsorption (ion exchange) in aqueous
    PH_POLYEL = 'y',    // colloidal poly- (oligo)electrolyte
    PH_SINCOND  = 's',  // condenced solid phase, also multicomponent
    PH_SINDIS   = 'd',  // dispersed solid phase, also multicomponent
} PHL_CLASSES;

#endif   //_DataCH_H

