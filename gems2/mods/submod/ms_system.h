//-------------------------------------------------------------------
// $Id$
//
// Declaration of TSyst class, config, load, pack, calc functions
//
// Rewritten from C to C++ by S.Dmytriyeva  
// Copyright (C) 1995-2001 S.Dmytriyeva, D.Kulik
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
//
#ifndef _ms_system_h_
#define _ms_system_h_

#include "v_mod.h"
#include "m_phase.h"

typedef struct
{  // SYSTEM is base to Project (local values)
    char
    //       mus[MAXMUNAME],   // Identifier of root multisystem <- RMULTS
    //       pot[MAXTDPCODE],  // Symbol of thermodynamic potential to minimize{G,GV}
    //       symb[MAXSYSNAME], // Identifier of Chemical System Definition (CSD)
    //       time_[MAXTIME], nchti, //CSD variant number for composition or DC restrictions
    //       what[MAXSYWHAT],       // Record Key Comment to SYSTEM definition

    //   PhmKey[EQ_RKLEN],//Key of EQSTATe record to supply phases for b vector calculation
    //   name[MAXFORMULA],   // Full name of this CSD definition
    //   notes[MAXFORMULA],  // Comments

    // Include and types flags for SYSTEM (+ -)
    PE,    // electroneutrality constraint PE { + - }
    PV,    // volume constraint PV { + - }
    PPfst, // P state variable  { + - }
    PVfst, // V state variable  { + - } (reserved)
    PTfst, // T state variable  { + - }
    PSfst, // S state variable  { + - } (reserved)
    PHfst, // H state variable  { + - } (reserved)
    PUfst, // U state variable  { + - } (reserved)
    PGmax, // Maximize or minimize G(X) { - } (reserved)
    PBeq,  // Account for electrostatic sorption models (DDLM,CCM,TLM) { +- }
    // Flags of arrays to be included into CSD: (+-*)
    PbIC,  // bi_ via IC quantities { + * - }
    PbDC,  // xd_ via DC quantities { + * - }
    PbAC,  // xa_ via COMPOS quantities { + * - }
    PbPH,  //xp_ via quantities of phases from EQSTATe pointed by rkey in SyPhEQ
    DLLim, // lower DC restrictions for x_j { + * - }
    PLLim, // pll_ lower restrictions for X_a (phases) { + * - } reserved
    DULim, // dul_ upper DC restrictions for x_j { + * - } reserved
    PULim, // pul_ upper restrictions for X_a (phases) { + * - } reserved
    PPHk,  // Interprete pll_ as kinetically fixed quantities of phases? res.
    PSATT, //PSATT_ to classify SAT calculation methods&allocation for sur DC
    PGEX,  // gEx_ excess free energies for (metastable) DC { + * - }
    PYOF,  // Yof_ metastability parameter for phases { + * - }
    PMaSdj, // MaSdj_ array for Density, Frumkin, CD-MUSIC (new) { + * - }
    PlnGf, // lnGmf_ Initial activity coefficients of DC { + * - }
    PAalp, // Aalp_ specific surface areas for phases { + * - }
    PSigm, //Sigm_ specific surface energy for solid phase-aqueous(gas)interface
    PXr0h0, // Xr0h0_ parameters r0 and h0 of particles (pores) { +*- } res.
    PXepsC, // XepsC_ diel.const & el.conductivity of phase carriers? res.
    PNfsp,  //Nfsp_,SCMT_: fractions and SCM codes for surface types? { +*- }
    PMaSdt, // get max dens of reacted sites on surtypes 1/nm2?
    PXcapF, /* Capacitances of Ba layer (FLM SAT)  new switch */
    PXcapA, // capacitance of A (0) EDL layer (TLM,CCM) for surface types
    PXcapB, // capacitance of B EDL layers (TLM,SGM) for surface types
    PXfIEC, // onstant-charge surface density or IEC on surtypes mkmol/g?
    PParc,  // Pparc_ partial pressures for DC { + * - }
    PdelB,  // delB_  uncertainties of bulk composition (vector b) elements
    PXlam,  // Xlam_ EDL discretness parameter for surface types, >0, <1,
    Plref;  // sSDref, sSDval  references to data sources { + * - }
    // at all 38 A_
    /* Fact size of arrays */
    short N,  // N of IC selected into system <= mu.N
    L,    // N of DC selected into system <= mu.L
    Ls,   // Ls - final N of DC in all multicomponent phases
    Lw,   // Lw - final N of aqueous+solvent species
    Lsor, // Lsor - final N of DC in sorption phases
    Lg,   // Lg - final N of DC in gaseous phase
    Lhc,  //  Lhc - final N of DC in liquid hydrocarbon phase (reserved)
    Fi,   // N of phases selected into system  Fi
    Fis,  // Fib - N of phases used in calculation of bulk composition
    Fib,  // Fib - N of phases used in calculation of bulk composition
    Fik,  // Fik - N of phases with kinetic restrictions to X_a (reserved)
    La,   // La - final N of COMPOSes used in calculation of b vector
    Lb,   // Lb - final N of DC formulae used in calculation of b vector
    Lk,   //Lk - N of DC with kinetic restrictions to x_j (dll_ or dul_)res.
    Na,   //Na- N of IC non-zero quantities used for calculation of vector b
    LO,   // L0 - H2O-solvent index in RMULTS DC list
    Nsd,  // Nsd -  N of Data Source SDref keys
    NsTm,  // Max number of surface types on sorption phases = MNST
    Le,   /* Number of DC to set up Gex or activ.coeff. */
    Fie,  /* Number of phases to set up YOF etc. */
    Fia,  /* Number of adsorption phases ON */
    Nr1;  /* Reserved */

    // Scalar parametres of state
    float Pmin, Pmax, // Pmin,Pmax for parametric problems (reserved)
    Tmin, Tmax, // Tmin,Tmax for parametric problems (reserved)
    Vmin, Vmax, // Vmin,Vmax for parametric problems (reserved)
    Hmin, Hmax, // Hmin,Hmax for parametric problems (reserved)
    // Params of system
    Mbel, // Molality of reference electrolyte Mbel (FIA of ionic strength)
    Mwat, //Anticipated mass (kg) of water-solvent for calculation of molalities
    Msys, // Anticipated total mass of the system (kg) for mass % calculation
    Maq,  // Anticipated mass of aqueous phase (kg) for calculation of ppm etc.
    MBX,  // final total mass of the system (kg) calculated from b vector
    R1,  //NMS-total number of IC moles in the system (for mole %% calculations)
    Vsys, // Anticipated volume of the system (L), for volume concentrations
    Vaq,  //Anticipated volume of aqueous phase (L) for calculation of molarities
    Time, // Tau - physical time (for PROCES) reserved
    KSI,  // Xi - current value of process extent variable (reserved)
    NU,   // Nu - current value of process extent variable (reserved)
    Rkin; // reserved

    short *Ll;  // N of DC selected within each phase [0:mu.Fi-1]
    char
    // Code classes see in S_CLASS.H
    *RLC,  //Classifier of DC restriction types {OLUB} res. x[j]  [mu.Ls]
    *RSC,  //Classifier of DC restriction scales {MGnvmLwCADE } x[j] [mu.Ls]
    *RFLC, //Classifier of phase restriction types {OLUB} res. XF[k] [mu.Fi]
    *RFSC, // Classifier of phase restriction scales {MGnvwsS} XF[k] [mu.Fi]
    // Selection switches
    *Icl,  // IC selection switches { + * - } [0:mu.N-1]
    *Dcl,  // DC selection switches { + * - } [0:mu.L-1]
    *Pcl,  // PHASE selection switches { + * - }[0:mu.FI-1]
    *Acl,  // COMPOS selection switches { + * - } [0:mu.La-1]
    //  Scales of elements
    *BIun,  // Scales of IC quantity/concentration (bi_) [0:mu.N-1]
    *XDun,  //Scales of DC quantity/concentration (xd_, dll_)[0:mu.L-1]
    *XAun,  // Scales of COMPOS quantity/concentration (xa_)[0:mu.La-1]
    *XPun   // Scales of phase quantity/concentration ( xp_, pll_[0:mu.Fi-1]
    ;
    double
    *B;  // Vector b of bulk chemical composition of the system, moles [0:mu.N-1]
    float
    *delB;//Uncertainties of bulk composition vector b elements,in BCun_ units [mu.N]
    double
    *BI,   // IC quantity/concentration to calculate  b vector [0:mu.N-1]
    *XeA,  // COMPOS quanity/concentration to calculate  b vector [0:mu.La-1]
    *XeD,  // DC quantity/concentration to calculate  b vector [0:mn.L-1]
    *Phm; //PHASE(equilibrium) quantity/concentration to calculate  b vector [mu.Fi]
    char (*SATC)[2]; /* Classifier of methods of SAT calculation { CNSI }[mu.Ls] */
    /* and allocation of sur DC to carrier components */
    float
    *Pparc, // DC partial pressures, bar [0:mu.L-1]
    /*VG*/  *DUL,// Vector of upper DC restrictions to x_j at eqstate (res)[mu.Ls]
    /*NG*/  *DLL,// Vector of lower DC restrictions to x_j at eqstate [mu.Ls]
    *GEX,// Excess free energy increment for DC, J/mole [mu.L]
    *PUL,//Vector of upper restrictions to phases X_a at eqstate(res.)[mu.Fi]
    *PLL,//Vector of lower restrictions to phases X_a at eqstate(res)[mu.Fi]
    *YOF,   // Phase metastability parameter, J/g [mu.Fi]
    *lnGmf, // ln of initial DC activity coefficients [mu.L]
    *Aalp,  // Specific surface area of dispersed phases, m2/g [mu.Fi]
    (*MaSdj)[DFCN], // Density, Frumkin, CD-MUSIC params new [mu.Ls][DFCN]
    (*Sigm)[2],//Specific surface energy of solid-aqueous (gas) interface, J/m2 [mu.Fi][2]
    (*Xr0h0)[2],//Parameters r0 and h0 of particles (pores at r0<0), nm reserved [mu.Fi][2]
    (*XEpsC)[2],// Dielectric constant & conductivity of phase carrier, reserved [mu.Fi][2]
    (*Nfsp)[MNST],// Fraction of surface types relative to carrier components [mu.Fis][6]
    (*MaSdt)[MNST],// Max.total reactive species density 1/nm2 [mu.Fis][6]
    (*XcapA)[MNST],// Inner-layer capacitance density (TLM,CCM) for surface types, F/m2 [mu.Fis][6]
    (*XcapB)[MNST],// Outer-layer capacitance sensity (TLM) for surface types, F/m2[mu.Fis][6]
    (*XcapF)[MNST], // Min.total reactive species density 1/nm2 [mu.Fis][6]
    (*XfIEC)[MNST], // Fix surf charge density or IEC mkeq/m2    [mu.Fis][6]
    (*Xlam)[MNST],  // Factor of discretnes IDL 0 < 1 - in PHASE  [mu.Fis][6] */
    *Guns,  //  mu.L work vector of uncertainty space increments to tp->G + sy->GEX
    *Vuns;  //  mu.L work vector of uncertainty space increments to tp->Vm 
    char
    (*SCMT)[MNST]; /* Classifier of EDL models for surface types [mu.Fis][NsTm] */
}
SYSTEM;

// Data of SYSTEM
class TSyst :
            public TSubModule
{
    RMULTS* mup;
    SYSTEM sy;
    gstring titler;


protected:
    void phase_data_load();
    void sorption_data_load( TPhase* aPH, int k );
    void set_aqu_gas_phase();
    void unpackData();
    void make_syst_sizes();
    void make_syst();
    void systbc_calc( int mode );
    void PHbcalc( double *MsysC, double *MaqC, double *R1C,
                  double *VaqC, double *VsysC );
    void stbal( int N, int L, float *Smatr, double *DCstc, double *ICm );
    // mark data
    void mark_ic_to_bc();
    //void mark_ph_to_dc();
    void mark_dc_to_ph();
    void mark_dc_to_ic();
    void mark_ic_to_dc();

    void EvPageChanged(int nPage);

    void syIC_pr_tab( const char *key  );
    void syDC_pr_tab( const char *key  );
    void syPh_pr_tab( const char *key  );

public:

    void CellChanged();
    SYSTEM* GetSY()
    {
        return &sy;
    }

    TSyst( int nrt, RMULTS* mup );

    const char* GetName() const
    {
        return "System";
    }

    void ods_link( int i=0);
    void dyn_set( int i=0);
    void dyn_kill( int i=0);
    void dyn_new( int i=0);
    void set_def( int i=0);


    // test data
    void ICompExit();
    void DCompExit();
    void PhaseExit();

    void loadData( bool newRec, int reBuildType = 2 );
    void setDefData();

    void mark_ph_to_dc();
    void SyTest();
    void SyTestSizes();
    void setSizes();
    void packData();
    void packData( TCIntArray PHon, TCIntArray PHoff,
                   TCIntArray DCon, TCIntArray DCoff);
    bool BccCalculated();

    //Multi calck
    double MolWeight( int N, float *ICaw, float *Smline );

    const gstring& GetString()
    {
     titler = rt[RT_SYSEQ].PackKey();
     //titler += " : ";
     //titler += TSubModule::GetString();
     return titler;
    }

};

typedef enum {  /* Limits on DC and phases */
    /* type of lmits */
    NO_LIM = 'O', LOWER_LIM ='L', UPPER_LIM = 'U', BOTH_LIM ='B',
    /* mode recalc of limits Set_DC_Limits() */
    DC_LIM_INIT = 0, DC_LIM_CURRENT = 1
} DC_LIMITS;

#endif  //_ms_system_h_
