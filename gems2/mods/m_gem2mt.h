//-------------------------------------------------------------------
// $Id$
// To be finalized in Version 3.0 (2006)
// Declaration of GEM2MT class, config and calculation functions
//
// Copyright (C) 2005 D.Kulik, S. Dmytriyeva
//
// This file is part of a GEM-Selektor library for thermodynamic
// modelling by Gibbs energy minimization
// Uses: GEM-Vizor GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://les.web.psi.ch/Software/GEMS-PSI for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------

#ifndef _m_gem2mt_h_
#define _m_gem2mt_h_

#include "m_param.h"
#include "v_ipnc.h"
#include "graph.h"
#include "v_module.h"

const int MT_RKLEN = 80;


typedef struct
{ // Description of GEM2MT data structure (prototype of 21 Feb. 2005)
  // Record key format: the same as Proces
  char
   PunE,          // Units of energy   { j;  J c C N reserved }
   PunV,          // Units of volume   { j;  c L a reserved }
   PunP,          //  Units of pressure  { b;  B p P A reserved }
   PunT,          // Units of temperature  { C; K F reserved }

// Allocation flags
   PvICi,    // Use IC quantities for initial system compositions? { + * - }
   PvAUi,    // Use formula units for initial sub-system compositions? { + * - }
   PvMSt,    // Use math script for running the mass transport (+ -)?
   PvMSg,    // Use math script for graphic presentation (+ -)?
   PvEF,      // Use empirical data for graphics  (+ -)?
   PvPGD,    // Use phase groups definitions (+ -)?
   PvFDL,    // Use flux definition list (+ -)
   PvSd,     // reserved?

     // Controls on operation
   PsMode,  // Code of GEM2MT mode of operation { S F A D T }
//  Status and control flags (+ -)
   gStat,  // GEM2MT generation status: 0 -indefinite; 1 on-going generation run;
        //  2 - done generation run; 3 - generation run error (+ 5: the same in stepwise mode)
   iStat,  // GEM2MT iteration status:  0 - indefinite; 1 ready for analysis;
        //  2 - analysis run; 3 - analysis done; ( + 5: the same using stepwise mode)
   PsSYd,  // Save generated SysEq records to data base (+ -)
   PsSdat,  //  Save Multi, DataCH and inital DataBR files as text files
   PsSbin,  //  Save Multi, DataCH and inital DataBR files as binary files
   PsNbin,  //  Save also the nodes DataBR array as a binary file
   PsRes1,  // reserved

   name[MAXFORMULA],  //  Name of GEM2MT task
   notes[MAXFORMULA]  //  Comments
    ;
  short
// input I
   nC,   // nQ - number of local equilibrium compartments (nodes)
// xC, yC, zC  numbers of nodes along x, y, z coordinates
   nIV,  // number of initial variants of the chemical system, nIV <= nC
   nPG,  // number of mobile phase groups (0 or >1)
   nFD,  // number of flux definitions    (0 or >1)
   Lbi,  // Lb - number of formula units to set compositions in initial variants
   Nsd,  // N of references to data sources
   Nqpt, // Number of elements in the script work array qpi for transport
   Nqpg, // Number of elements in the script work array qpc for graphics
   Nb,   // N - number of independent components (set automatically from Multi)
   FIb,   // N - number of independent components (set automatically from Multi)
   bTau, // Time point for the simulation break (Tau[0] at start)
   nS,   // Total number of points to sample the results in xt, yt
   nYS,  // number of plots (columns in the yt array)
   nE,   // Total number of experiments points (in xEt, yEt) to plot over
   nYE,  // number of experimental parameters (columns in the yEt array)
   nPai,  // Number of P points in MTP interpolation array in DataCH ( 1 to 10 )
   nTai,  // Number of T points in MTP interpolation array in DataCH ( 1 to 20 )

// iterators for generating syseq record keys for initial system variants
   tmi[3],   // SYSTEM CSD definition #: start, end, step (initial)
   NVi[3],    // Restrictions variant #: start, end, step
   axisType[6],  // axis graph type, background(3) reserved(2)
   *DiCp,   // array of indexes of initial system variants for
             // distributing to nodes [nC]
   (*FDLi)[2] //[nFD][2] Box indices in the flux definition list
   ;
  float        // input
   *Pi,    // Pressure P, bar for initial systems (values within Pai range) [nIV]
   *Ti,    // Temperature T, C for initial systems (values within Pai range) [nIV]
   *Vi;    // Volume of the system (L) [nIV] usually zeros
  double
  // Input for compositions of initial systems
   Msysb, // Masses (kg) and volumes (L) for initial systems: Ms (total mass, normalize)
   Vsysb, // Vs (total volume of the object, for volume concentrations)
   Mwatb, // M(H2O) (mass of water-solvent for molalities)
   Maqb,  // Maq (mass of aqueous solution for ppm etc.)
   Vaqb,  // Vaq (volume of aqueous solution for molarities)
   Pgb,   // Pg (pressure in gas, for partial pressures)
   Tmolb, // MOL total mole amount for basis sub-system composition calculations
   WmCb,  // mole fraction of the carrier DC (e.g. sorbent or solvent)
   Asur;  // Specific surface area of the sorbent (for adsorbed species)
  float
// Iterators for MTP interpolation array in DataCH
   Pai[3],  // Pressure P, bar: start, end, increment for MTP array in DataCH
   Tai[3],  // Temperature T, C: start, end, increment for MTP array in DataCH
   Tau[3],  // Physical time iterator
// graphics
   size[2][4], // Graph axis scale for the region and the fragment
   *xEt,    // Abscissa for experimental points [nXE]
   *yEt;    // Ordinates for experimental points to plot [nXE, nYE]
 double
   *Bn,    //  [nIV][N] Table of bulk compositions of initial systems
   *qpi,   //  [Nqpi] Work array for initial systems math script
   *qpc,    //  [Nqpc] Work array for mass transport math script,
   *xt,    //  Abscissa for sampled data [nS]
   *yt     //  Ordinates for sampled data [nS][nYS]
   //
//   *Bc,    // table of bulk compositions of reactive part of nodes
//  More to be added here for seq reactors?
    ;
 float
   *CIb, // [nIV][N] Table of quantity/concentration of IC in initial systems
   *CAb, // [nIV][Lbi] Table of quantity/concentration of formulae for initial systems
   (*FDLf)[2], // [nFD][2] Part of the flux defnition list (MPG quantities)
   *PGT  // Quantities of phases in MPG [Fi][nPG]
    ;
 char
   *tExpr,  // Math script text for calculation of mass transport
   *gExpr,  // Math script text for calculation of data sampling and plotting
   (*sdref)[V_SD_RKLEN], // "List of bibl. refs to data sources" [0:Nsd-1]
   (*sdval)[V_SD_VALEN],  // "Parameters taken from the respective data sources"[0:Nsd-1]
   (*nam_i)[MAXIDNAME], // [nIV][12] id names of initial systems
   (*for_i)[MAXFORMUNITDT], // [Lbi][40] formulae for setting initial system compositions
   (*stld)[EQ_RKLEN], // List of SysEq record keys for initial systems [nIV]
//
   *CIclb, // [N] Units of IC quantity/concentration for initial systems compositions
   *AUcln, // [Lbi] Units of setting UDF quantities for initial system compositions
   (*FDLid)[MAXSYMB], // [nFD] ID of fluxes
   (*FDLop)[MAXSYMB], // [nFD] Operation codes (letters)
   (*FDLmp)[MAXSYMB], // [nPG] ID of MPG to move in this flux
   (*MPGid)[MAXSYMB], // [nPG] ID list of mobile phase groups
   *UMPG,  // [nFi] units for setting phase quantities in MPG (see PGT ),
//
   (*SBM)[MAXICNAME+MAXSYMB],  // Keys (names) of IC
//  graphics
    xNames[MAXAXISNAME],        // Abscissa name
    yNames[MAXAXISNAME],       // Ordinate name
    (*lNam)[MAXGRNAME],        // List of ID of lines on Graph [nYS]
    (*lNamE)[MAXGRNAME];       // List of ID of lines of empirical data [nYE]


/* Work arrays */
   char sykey[EQ_RKLEN+10],    // Key of currently processed SysEq record
   *etext,              // internal
   *tprn;              // internal
//work data
 short
   ctm,    // current CSD #
   cnv,    //  current restriction variant #
   qc,     // current index of the compartment ( 1 to nC )
   kv,     // current index of the initial system variant (1 to nIV )
   jqc,    // script c-style index (= qc-1) for transport
   jqs,    // script c-style index (= qc-1) for graphics
   jt,      // index of sampled point (for sampling scripts)
   rei1,
   rei2,
   rei3,
   rei4,
   rei5
   ;

 float
   cT, // current value of T
   cP, // current value of P
   cV, // current value of V
   cTau, // current physical time
   dTau, // current time step value
   oTau, // old time step value
   ref1,
   ref2,
   ref3,
   ref4
  ;

   char timep[16], TCp[16], Pp[16], NVp[16], Bnamep[16];
}
GEM2MT;

// Pointers to DataCH and DataBR (current) fields in memory ?
// They are located in MULTI

// Current GEM2MT
class TGEM2MT : public TCModule
{
    GEM2MT mt[1];

    IPNCalc rpn[2];      // IPN

    GraphWindow *gd_gr;
    TPlotLine *plot;
    gstring titler;

protected:

    void keyTest( const char *key );
    void Expr_analyze( int obj_num );
    void CalcPoint( int nPoint );
    bool test_sizes();
    void init_arrays( bool mode );
    void calc_eqstat();
    void outMulti();
    void mt_next();
    void mt_reset();
    void gen_task();
    void make_A( int siz_, char (*for_)[MAXFORMUNITDT] );
    void Bn_Calc();


/*    // internal
    bool test_sizes();
    void mt_initiate( bool mode = true );   // must be changed with DK
    void mt_next();
    void calc_eqstat();
    void mt_text_analyze();                 // translate &
    void CalcEquat( int type_ );            // calculate RPN
    void Biv_Calc();
    void MT_Calc();

    // last level
    void Init_Generation();
    void build_IV();         // generate initial systems
    void distribute_IV();    // distribute initial systems
    void save_DataCH();      // Save multi and dataCH files
    void save_IV();          // save initial systems as DataBR files
    void Iterate();          // analyse the results (change DK)
*/

public:

    static TGEM2MT* pm;

    GEM2MT *mtp;

    TGEM2MT( int nrt );

    const char* GetName() const
    {
        return "GEM2MT";
    }

    void ods_link( int i=0);
    void dyn_set( int i=0);
    void dyn_kill( int i=0);
    void dyn_new( int i=0);
    void set_def( int i=0);
    bool check_input( const char *key, int level=1 );
    gstring   GetKeyofRecord( const char *oldKey, const char *strTitle,
                              int keyType );

    void RecInput( const char *key );
    int RecBuild( const char *key, int mode = VF_UNDEF );
    void RecCalc( const char *key );
    void RecordPlot( const char *key );
    bool SaveGraphData( GraphData* graph );

    void CmHelp();

    void InsertChanges( TIArray<CompItem>& aIComp ); 
};


#endif //_m_gem2mt_h_
