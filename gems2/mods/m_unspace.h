//-------------------------------------------------------------------
// $Id$
// Under construction, to be included in version 3.0 (2004)
// Declaration of TUnSpace class, config and calculation functions
// (formerly TProbe class rewritten from C to C++ by S.Dmytriyeva   
// Copyright (C) 1995-2004 K.Chudnenko, S.Dmytriyeva, D.Kulik
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
#ifndef _m_unspace_h_
#define _m_unspace_h_

#include "m_param.h"

const int UNSP_RKLEN = 80;

typedef struct
{ // Description  UnSpace
    char
    //  the same record key as in Proces
    name[MAXFORMULA],      // Full name of UnSpace task
    notes[MAXFORMULA],     // Comments
//  Status and control flags (+-)
    Gstat, // UnSpace generation status: 0 -indefinite; 1 generation run;
           //  2 - done generation run; 3 - generation run error (+ 5 the same in stepwise mode)
    Astat, // UnSpace analysis status:  0 - indefinite; 1 ready for 
           // analysis; 2 - analysis run; 3 - done; ( + 5: the same using stepwise mode)

// Flags that control construction and sampling of UnSpace
    PsUnInt, //  ( % A ) int UnInt;  units of interval uncertainty for G S V: 0-%; 1-absolute
    PsUnInB, //  ( % A L)            units for intervals for B (% percent abs; A abs; L - log)  
    PsGen[6], // (+ - 0 1 ) int zond[6]; zond type  [0]-G; [1]-S; [2]-b; [3]-T; [4]-P; [5]-V [6] ParGamma
    PsSY,  // Save generated SysEq records to data base (+ -)
    PsEqn, // Will math script be specified in this UnSpace definition (+ -)

// Plags that control analysis of sampled results
    Pa_f_phgr, //  (+ -)            flag of input the filter phase association
    Pa_f_mol,  //  (+ -) int f_mol; flag? parameter of input the filters of molality
    Pa_f_fug,  //  (+ -) int f_fug; flag? parameter of input the filters of fugacity
    Pa_f_mfr,  //                   flag of input the filters on mole fractions 
    Pa_f_pH,   //  (+ -) int f_pH;  flag? parameter of input the filters of pH
    Pa_f_Eh,   //  (+ -) int f_Eh;  flag? parameter of input the filters of Eh
    Pa_Adapt,  //  (+ -) int Adapt; Adaptive loops for G(298): 0-no; >=1-yes (max number of loops)  
    Pa_OF,     //  (A B C D E F) OF;   function to choose payoff matrix function  eq.18 
    Pa_Crit,   //  (0 1 2 3 4) int Crit; criterion: 0-PA frequency statistics, 1-Laplace(quan), 
                   // 2-Homenuk(quan) 3-Laplace (single), 4-Homenuk (single)  
// Allocation flags
    PvPOM,     //  (+ -) allocation flag for payoff matrix (if Q <= 1001)    + - + -
    PvPOR,     //  (+ -) allocation flag for payoff matrix row (if Q > 1001) + + - -
    PvGs,      //  (+ -) allocation flag for G0-related vectors
    PvSs,      //  (+ -) allocation flag for S0-related vectors
    PvVs,      //  (+ -) allocation flag for V0-related vectors
    PvPgam,    //  (+ -) allocation flag for ParGamma-related vectors (reserved)
    Pv07,
    Pv08;


// From Selektor-C 
    short  // Dimensionalities related to UnSpace problem (Project/System indexation) 
// input 
    N,      //   Number of independent components   -> mup->N
    L,      //   Number of dependent components     -> mup->L 
    Ls,     //   Total number of DC in multi-comp. phases -> mup->Ls 
    Fi,     //   Number of phases                   -> mup->Fi
    Fis,    //   Number of multi-component phases   -> mup->Fis
    Nsd,    //   N of data source references (default 0)

    Q,     // 0 < Q < 1001 QT number of sample points to be generated
    QT,    // the same (in Kostya's subroutines) 
// input II                                                                           
    nG,    //  int kG;      total number of UnSpace groups (coordinates) kGB or kGN + kGR 
    nGB,   //  int kGB;     N of UnSpace groups used in generation by Belov algorithm 
    nGN,   //  int kGN;     N of UnSpace groups with MC normal distribution law (if nGB = 0)
    nGR,   //  int kGR;     N of UnSpace groups with MC uniform distribution law (if nGB = 0)
    NgT,   //  int NgT;     Group index for uncertain T, 0- T not uncertain
    NgP,   //  int NgP;     Group index for uncertain P, 0- P not uncertain
    NgV,   //               Group index for unvertain V, 0- not uncertain (reserved)

// output
    Lapl,  //  int Lapl;   index of optimal sample point by Laplace (within 0 to Q-1)
    Hurw,  //  int Hurw;   index of optimal solution by Hurtvitz
    Wald,  //  int Wald;   index of optimal solution by Wald
    Homen, // int Homen;  index of optimal solution by Homenyuk
    nl,    // int kl;      number of solutions Laplace (1 or in quantile int(quan_lev*float(Q)/100.) ) 
    nh,    // int kh;      number of solutions Hurtvitz 
    nw,    // int kw;      number of solutions Wald 
    nHom,  // int kHom;    number of solutions Homenyuk 
    t,     // current  index t of the row in payoff matrix     
    q;     // current index q of the column in payoff matrix   

// input
  float 
    T,      // Deterministic value of T
    IntT,    //  float IntT;  0 or uncertainty half-interval for T (in K) 
    P,      // Deterministic value of P
    IntP,    //  float IntP;  0 or uncertainty half-interval for P (in bars)
    V,      // Deterministic value of V
    IntV,    //  float IntV;  0 or uncertainty half-interval for V (in cm3? L?) 
// filters
    pH_lo,   // float pH_n;    pH lower filter limit 
    pH_up,   // float pH_v;    pH upper filter limit 
    Eh_lo,   // float Eh_n;    Eh lower filter limit 
    Eh_up,   // float Eh_v;    Eh upper filter limit 
// Quantile level (size)
    quan_lev; // float kvant_per;  // percent kvantil       (0.05) 

// calculated 
    double
    CrL,     //  double krL;    Laplace criterion value ???????????????????????????
    CrH,     //  double krH;    criteria Hurtvitc 
    CrW,     //  double krW;    criteria Wald 
    CrHom;   //  double krHom;  criteria Homenuk 

// Arrays:

// Input
   short 
   *L1,    //  [L]  L1 vector, shows a number of DC included to each phase
   *PbD,   //  [nG]  int *ZP; [kG] probability law code for groups: 0- Belov; 1-uniform; 2-normal
   *NgLg,  //  [L]  int *NgLg; [L] 0-deterministic; >0-number of groups varying G 
   *NgGam, //  [Ls]     0-deterministic; >0 number of groups varying gamma params (reserved)
   *NgLs,  //  [L]  int *NgLs; [L] 0-deterministic; >0-number of groups varying S 
   *NgLv,  //  [L]  int *NgLv; [L] 0-deterministic; >0-number of groups varying V 
   *NgNb,  //  [N]  int *NgN;  [N] 0-deterministic; >0 number of groups varying b 
   *f_PhA; //  [N]  filter for known equilibrium phase association 
                        
   float
   *IntLg0, //  [L]  float *IntLg0;  [L]  uncertainty half-intervals for G -initial 
   *IntGam, //  [Ls]     half-intervals for gamma params  (reserved)
   *IntLs,  //  [L]  float *IntLs;   [L] uncertainty half-intervals for S 
   *IntLv,  //  [L]  float *IntLv;   [L] uncertainty half-intervals for V  
   *IntNb,  //  [N]  float *IntN;    [N] uncertainty half-intervals for b    (scale: log? )
   *m_t_lo, //  [N]  double *mol_n;   [N] total molality filter - lower limits (def. 0)
   *m_t_up, //  [N]  double *mol_v;   [N] total molality filter - upper limits (def. 10)
   *fug_lo, //  [Ls] double *fug_n;   [PG] fugacity filter - lower [Ls] mole fraction for SS ? log? 
   *fug_up, //  [Ls] double *fug_v;   [PG] fugacity filter - upper limits 
                                      
// Input II 
   *IntLg,  //  [L]  float *IntLg;   G uncertainty half intervals (corrected)
   *Gs,     //  [L]  float *Gs;       copy of deterministic values of G298 [L]
   *Ss,     //  [L]  float *Ss;       copy of deterministic values of S298 [L]
   *Vs,     //  [L]  float *Vs;       copy of deterministic values of V298 [L]
   *GAMs;   //  [Ls]                   deterministic Par Gamma (reserved)  
double
   *Bs,     //  [N]  double *Bs;       copy of deterministic bulk composition b [N]

// output for payoff matrix
   *vG,     //   [Q][L]  double *vG;      G298[QT*L] from sample input variants 
   *vY,     //   [Q][L]  double *vY;      Y[QT*L]    from sample solution variants 
   *vYF,    //   [Q][FI]  double *vYF;     YF[QT*FI] from sample solution variants
   *vGam,   //   [Q][L]  double *vGAMMA;  Gam[QT*L]  from sample solution variants
   *vMol,   //   [Q][N]  double *vMOL;    molality [QT*N] from sample solution variants 
               // for filtering/statistics 
   *vU,     //   [Q][N]  double *vU;     dual potentials  u[QT*N]  from sample solution variants 
               // Units ?
   *vFug;   //   [Q][Ls]  double *vFug;   fugacity/activity from sample solution variants 
float
   *vT,     //   [Q]   float  *vT;      T[QT] temperatures from sample input variants
   *vP,     //   [Q]   float  *vP;      P[QT] pressures from sample input variants
   *vpH,    //   [Q]   double *vpH;     pH[QT]  pH aq from sample solution variants for filtering 
   *OVB,     //   [nGB+1]  float *OVB;  pseudo-random numbers for Belov [kGB+1]
   *OVR,     //   [nGR+1]  float *OVR;  pseudo-random numbers by MC uniform distribution [kGR+1]
   *OVN;     //   [nGN+1]  float *OVN;  pseudo-random numbers by MC normal distribution  [kGN+1]
short
   *quanLap,  //   [nl][Q]    int *kvantLapl;  indices of variants taken in quantile Laplace 
   *quanHom;  //   [nHom][Q]  int *kvantHom;   indices of variants taken in quantile Homenyuk 

// Work arrays
short 
   nPhA,     //  < Q      Number of different phase assemblages in all sampled variants
   *PhAndx,  //  [Q][N] -> [nPhA][N]  Table of system indices of phases to identify PhA
   *sv,      //  [Q]   sv[bp->QT]  Indices of PhA ids in sampled variants (PhAndx)

   *PhNum;   //  [nPhA]  vector of number of sampled points containing PhA 
float
   *PhAfreq, //  [nPhA]  frequency of samples points containing PhA (PhNum*100/Q)

   *POM,     //  [Q][Q]  payoff matrix   optional (see PvPOM flag)     
   *POR,     //  [Q]  payoff matrix row   optional (see PvPOR flag)    
   *pmr;     //  pointer to the beginning of current line in payoff matrix ( PvROM/PvPOR )
double
   *Zcp,     //  [Q]   Zcp[bp->QT]   mean in rows of pay-off matrix for Laplace criteria 
   *Zmin,    //  [Q]   Zmin[bp->QT]     min row pay-off matrix  for Hurwitz criteria 
   *Zmax,    //  [Q]   Zmax[bp->QT]     max row pay-off matrix  for Hurwitz criteria 
   *ZmaxAbs, //  [Q]   Zmaxabs[bp->QT]  abs(max) row pay-off matrix  for Wald criteria  
   *Hom,     //  [Q]   Hom[bp->QT]      array of calculated Homenyuk criteria values  
   *Prob;    //  [Q]   posib[bp->QT]    probabilities for calculating Homenuk criteria 

// input GEMS
    char
   (*PhAID)[8],   // [nPhA] list of PhA identifiers (def. 0001 0002 ... )
   (*PhAlst)[80], // [nPhA] list of PhA titles (made of Phase names)

    *ZPg,      // Code of distribution law for groups [kG] {b u n}  or 
               // 0-uniform distribution law, 1-normal distribution law
    *Expr,     // Text with math script equations (params activity coeffs ??? reserved ) 
    (*SGp)[MAXPHNAME],    //List of UnSpace group names [kG]
    (*stl)[EQ_RKLEN],     // List of generated SysEq records [Q]
    (*sdref)[V_SD_RKLEN], // List of SDref keys to data sources [Nsd]
    (*sdval)[V_SD_VALEN],  // Comments to data sources [Nsd]
    stkey[EQ_RKLEN+10],  // currently loaded (processed) SysEq key
    char *tprn;               // internal

} 
UNSPACE;


typedef enum {  
  // codes of distribution law for UnSpace groups  formerly PRB_CLASSES
    UNSP_DL_BELOV    = 'b',     /* belov */
    UNSP_DL_UNIFORM  = 'u',     /* uniform */
    UNSP_DL_NORMAL   = 'n',      /* normal */

  //  

} UNSP_CLASSES;




// Current Probe
class TUnSpace : public TCModule
{
    UnSpace usp[1];

protected:

    //IPNCalc rpn;       // IPN of equats of process  -- Expr
    void keyTest( const char *key );
    short get_simple( short oldSimp );
    bool is_simple( short  Simp );
    float UnsG( float x, float dx );
    void pr_init();
    void BELOV();
    int WL(int i,int j,int QT);
    float PNTBEL(int J,int QT,int OV);
    float PNTNOR(int I,int J,int QT);
    void NexT( int J, int iT, int iP );
    short  pb_min( int N, double *x);
//    void  _pr_tab( const char *key );

public:

    static TProbe* pm;

    PROBE *prp;

    TProbe( int nrt );

    const char* GetName() const
    {
        return "Probe";
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
    void CmHelp();

    //changes
    void InsertChanges( TIArray<CompItem>& aIComp,
                        TIArray<CompItem>& aPhase,  TIArray<CompItem>&aDComp );
    void newSizeifChange();

    //STATISTIC
    void paragen( int N, GDSTAT gst );
    void pb_matr( int QT, GDSTAT gst );

    short GetnI() const
    {
        return prp->nI;
    }
    short GetnF() const
    {
        return prp->nF;
    }
    short GetQT() const
    {
        return prp->QT;
    }
    short *GetIopt() const
    {
        return prp->iopt;
    }

};

#endif  // _m_probe_h

