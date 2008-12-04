//-------------------------------------------------------------------
// $Id$
//
// Copyright (C) 2003-2007  S.Churakov, Th.Wagner, D.Kulik, S.Dmitrieva
//
// Declaration of new implementation of Fluid EoS classes
// (PRSV and CG EoS models)
//
// This file is part of a GEM-Selektor (GEMS) v.2.x.x program
// environment for thermodynamic modeling in geochemistry
// and part of the standalone GEMIPM2K code
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch; chud@igc.irk.ru
//-------------------------------------------------------------------
//


#ifndef _s_fgl_h_
#define _s_fgl_h_

// Added 09 May 2003
// Definition of a class for CG EOS calculations for fluids
// Incorporates a C++ program written by Sergey Churakov (CSCS ETHZ)
// implementing papers by Churakov & Gottschalk 2003 GCA v.67 p. 2397-2414
// and p. 2415-2425
// Reference: http://people.web.psi.ch/churakov/, sergey.churakov@psi.ch

/*-----------------07.11.99 12:14-------------------
 Structure of parameters for LJ and Shokmayer potential
 sig(A)=F1+F5*T*1E-4
 epsilon(K)=F2+F3*Exp(-F4*T*1E-3)
 polaris(A)=F6
 mu(debue)=F7+F8*Exp(F9*T)
--------------------------------------------------*/
//#define MOLNUM (12)
//#define MOLPOL (6)
//#define MOLNONPOL (6)
/* Ar CO2 CH4 O2 N2 H2 CO SO2 HCl H2O H2S NH3 */

class EOSPARAM
{
  //static double Told;
  unsigned long int isize;  // int isize;
  double emix,s3mix;
  long int NComp;
   double *epspar,*sig3par;
   double *XX;
   double *eps;
   double *eps05;
   double *sigpar;
   double *mpar;
   double *apar;
   double *aredpar;
   double *m2par;
   double **mixpar;
  void init(double*,double *, unsigned long int );
  void allocate(unsigned long int );

  void copy(double* sours,double *dest,unsigned long int num);
  void norm(double *X,unsigned long int mNum);

  public:
  double *XX0;

  EOSPARAM():isize(0),emix(0),s3mix(0),NComp(0){};
    //EOSPARAM(double*data, unsigned nn):isize(0){allocate(nn);init(data,nn);};

    EOSPARAM(double *Xtmp, double *data, unsigned long int nn)
    :isize(0),emix(0),s3mix(0),NComp(0)
         {allocate(nn);init(Xtmp,data,nn);};

  ~EOSPARAM();

  double EPS05(unsigned long int i)           {return eps05[i];};
  double X(unsigned long int i)               {return XX[i];};
  double EPS(unsigned long int i)             {return eps[i];};
  double EMIX(void){return emix;};
  double S3MIX(void){return s3mix;};
  double MIXS3(unsigned long int i,unsigned long int j)
  {
    if (i==j) return sig3par[i];
    if (i<j) return mixpar[i][j]; else return mixpar[j][i];
  };

  double MIXES3(unsigned long int i,unsigned long int j)
  {
    if ( i==j ) return epspar[i];
    if (i<j) return mixpar[j][i]; else return mixpar[i][j];
  };

  double SIG3(unsigned long int i){return sig3par[i];};
  double M2R(unsigned long int i) {return m2par[i];};
  double A(unsigned long int i)  {return apar[i];};

  unsigned long int ParamMix( double *Xin);
  void PureParam(double* ,double*,double*,double* );
  long int NCmp(){return NComp;};
};


class TCGFcalc // Churakov & Gottschalk (2003) EOS calculations
{
  private:

  const double
          PI,    // pi
          TWOPI,    // 2.*pi
          PISIX,    // pi/6.
          TWOPOW1SIX,   // 2^(1/6)
          DELTA,
          DELTAMOLLIM,
          R,  NA,  P1,
          PP2, P3, P4,
          P5,  P6, P7,
          P8,  P9, P10,
          AA1, AA2, AA3,
          A4, A5, A6,
          BB1, BB2, BB3,
          B4,  B5,  B6,
          A00, A01, A10,
          A11, A12, A21,
          A22, A23, A31,
          A32, A33, A34;

  public:

        TCGFcalc():
         PI( 3.141592653589793120),    // pi
         TWOPI( 6.283185307179586230),    // 2.*pi
         PISIX( 0.523598775598298927),    // pi/6.
         TWOPOW1SIX(1.12246204830937302),   // 2^(1/6)
         DELTA (0.00001),
         DELTAMOLLIM (0.0000001),
         R(8.31439),
         NA(0.6023),
         P1(1.186892378996),
         PP2(-0.4721963005527),
         P3(3.259515855283),
         P4(3.055229342609),
         P5(1.095409321023),
         P6(1.282306659774E-2),
         P7(9.55712461425E-2),
         P8(13.67807693107),
         P9(35.75464856619),
         P10(16.04724381643),
         AA1(-0.120078459237),
         AA2(-.808712488307),
         AA3(.321543801337),
         A4(1.16965477132),
         A5(-.410564939543),
         A6(-.516834310691),
         BB1(-2.18839961483),
         BB2(1.59897428009),
         BB3(-.392578806128),
         B4(-.189396607904),
         B5(-.576898496254),
         B6(-0.0185167641359),
         A00(.9985937977069455),
         A01(.5079834224407451),
         A10(1.021887697885469),
         A11(-5.136619463333883),
         A12(-5.196188074016755),
         A21(-6.049240839050804),
         A22(18.67848155616692),
         A23(20.10652684217768),
         A31(9.896491419756988),
         A32(14.6738380473899),
         A33(-77.44825116542995),
         A34(-4.82871082941229)
      {}

protected:

   void copy(double* sours,double *dest,unsigned long int num);
   void norm(double *X,unsigned long int mNum);
   double RPA(double beta,double nuw);
   double dHS(double beta,double ro );
   inline double fI1_6(double nuw)
{
   return (1.+(A4+(A5+A6*nuw)*nuw)*nuw)/
   ((1.+(AA1+(AA2+AA3*nuw)*nuw)*nuw)*3.);
};

inline double fI1_12(double nuw)
{
   return (1.+(B4+(B5+B6*nuw)*nuw)*nuw)/
   ((1.+(BB1+(BB2+BB3*nuw)*nuw)*nuw)*9.);
};


inline double fa0(double nuw ,double nu1w2)
{
     return (A00 + A01*nuw)/nu1w2;
};

inline double fa1(double nuw ,double nu1w3)
{
     return (A10+(A11+A12*nuw)*nuw)/nu1w3;
};

inline double fa2(double nuw ,double nu1w4)
{
     return ((A21+(A22+A23*nuw)*nuw)*nuw)/nu1w4;
};

inline double fa3(double nuw ,double nu1w5)
{
     return ((A31+(A32+(A33+A34*nuw)*nuw)*nuw)*nuw)/nu1w5;
};

   double DIntegral(double T, double ro, unsigned long int IType);
   double LIntegral(double T, double ro, unsigned long int IType);
   double KIntegral(double T, double ro, unsigned long int IType);
   double K23_13(double T, double ro);
   double J6LJ(double T,double ro);
   double FDipPair(double T,double ro,double m2);
   double UWCANum(double T,double ro);
   double ZWCANum(double T,double ro);

   double FWCA(double T,double ro);
   double FTOTALMIX(double T_Real,double ro_Real,EOSPARAM& param);
   double UTOTALMIX(double T_Real,double ro_Real,EOSPARAM& param);
   double ZTOTALMIX(double T_Real,double ro_Real,EOSPARAM& param);
   double PTOTALMIX(double T_Real,double ro_Real,EOSPARAM& param);
   double ROTOTALMIX(double P,double TT,EOSPARAM& param);

public:
    //
   double PRESSURE(double *X, double *param, unsigned long int NN, double ro, double T );
   double DENSITY(double *X,double *param, unsigned long int NN ,double Pbar, double T );
   long int CGActivCoefRhoT(double *X,double *param, double *act, unsigned long int NN,
     double ro, double T );
   long int CGActivCoefPT(double *X,double *param,double *act, unsigned long int NN,
     double Pbar, double T, double &roro );

    long int CGcalcFug( void );  // Calc. fugacity for 1 species at X=1
    long int CGFugacityPT( float *EoSparam, double *EoSparPT, double &Fugacity,
        double &Volume, double P, double T, double &roro );
    long int CGFugacityPT( double *EoSparam, double *EoSparPT, double &Fugacity,
        double &Volume, double P, double T, double &roro );
    // Calculates residual enthalpy and entropy
    long int CGEnthalpy(double *X, double *param, double *param1, unsigned long int NN,
         double ro, double T, double &H, double &S );
    double GetDELTA( void )
    {
    	return DELTA;
    }
};


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Added 07 March 2007 by TW and DK; extended 25.11.2008 by DK
// Definition of a class for built-in solution models

class TSolMod
{

protected:
        char ModCode;   // Code of the mixing model

        long int NComp;    	// Number of components in the solution phase
        long int NPar;     	// Number of non-zero interaction parameters
        long int NPcoef;   	// Number of coeffs per parameter (columns in the aIPc table)
        long int MaxOrd;   	// max. parameter order (or number of columns in aIPx)
        long int NP_DC;    	// Number of coeffs per one DC in the phase (columns in aDCc)
        long int *aIPx;  	// Pointer to list of indexes of non-zero interaction parameters

        double R_CONST; // R constant
        double RhoW;	// Density of liquid water, added 04.06.2008 (TW)
        double EpsW;	// Dielectric constant of liquid water
//        double IonStr;	// Ionic strength
        double Tk;    	// Temperature, K
        double Pbar;  	// Pressure, bar

        double *aIPc;  	// Table of interaction parameter coefficients
        double *aIP;    // Vector of interaction parameters corrected to T,P of interest
        double *aDCc;  	// End-member parameter coefficients
        double *x;    	// Pointer to mole fractions of end members (provided)
        double *phVOL;    // phase volumes, cm3/mol                   [0:FI-1]

// Results
        double Gam;   	// work cell for activity coefficient of end member
        double lnGamRT;
        double lnGam;
        double Gex;   	// Molar excess Gibbs energy
        double Vex;   	// Excess molar volume
        double Hex;   	// Excess molar enthalpy
        double Sex;   	// Excess molar entropy
        double CPex;  	// Excess heat capacity
        double *lnGamma;   // Pointer to ln activity coefficients of end members
                           // (memory must be provided from the calling program)
        
public:
// Generic constructor
    TSolMod( long int NSpecies, long int NParams, long int NPcoefs, long int MaxOrder,
         long int NPperDC, double T_k, double P_bar, char Mod_Code,
         long int* arIPx, double* arIPc, double* arDCc,
         double *arWx, double *arlnGam, double *aphVOL, double dW, double eW );
    virtual ~TSolMod();

// 
    virtual long int PTparam( )
        { return 0;}
    virtual long int MixMod( ) 
        { return 0;}
    virtual void getExcessProp(  double &Gex_, double &Vex_, double &Hex_, double &Sex_,
    		double &CPex_ ) 
    {  Gex_ = Gex;
       Vex_ = Vex;
       Hex_ = Hex;
       Sex_ = Sex;
	   CPex_ = CPex;
	};

// PRSV can also be moved here

// Prototypes for other models to be added here
// Darken ...

// SIT model reimplementation for aqueous electrolyte solutions

// Extended UNIQUAC model for aqueous electrolyte solutions

};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Added 19 July 2006 by Th.Wagner and D.Kulik
// Definition of a class for PRSV EOS calculations for fluids
// Incorporates a C++ program written by Thomas Wagner (Univ. Tuebingen)
// Reference: http://www.uni-tuebingen.de/uni/emi/ag-markl/pages/wagner/
// th.wagner@uni-tuebingen.de
//
class TPRSVcalc: public TSolMod 
// Peng-Robinson-Styjek-Vera EOS calculations
{
  private:

	 double PhVol;   // bar, T Kelvin, phase volume in cm3
     double *Pparc;   // DC partial pressures/ pure fugacities, bar (Pc by default) [0:L-1]

     // main work arrays
     double (*Eosparm)[6];   // EoS parameters
     double (*Pureparm)[5];  // Parameters a and b for cubic EoS
     double (*Fugpure)[5];   // Fugacity parameters of pure gas species
     double (*Fugci)[4];     // Fugacity parameters of species in the mixture

     double **KK0ij;    //  Constant term of the binary interaction parameter
     double **KK1ij;    //  T-dependent term
     double **AAij;     //  binary a terms in the mixture

 	void alloc_internal();
 	void free_internal();

    public:

    TPRSVcalc( long int NCmp, double Pp, double Tkp );
    TPRSVcalc( long int NSpecies, long int NParams, long int NPcoefs, long int MaxOrder,
         long int NPperDC, double T_k, double P_bar, char Mod_Code,
         long int* arIPx, double* arIPc, double* arDCc,
         double *arWx, double *arlnGam, double *aphVOL, double * aPparc, double dW, double eW );

    ~TPRSVcalc();

    long int MixMod();

    // Calculation of internal tables (at each GEM iteration)
	long int PTparam();

	// Called from IPM-Gamma() where activity coefficients are computed
//   long int PRActivCoefPT( 
//        double *fugpure );

   long int CalcFugPure( void );
   // Calc. fugacity for 1 species at X=1
   long int PRFugacityPT( double P, double Tk, double *EoSparam, double *Eos2parPT,
        double &Fugacity, double &Volume, double &DeltaH, double &DeltaS );

protected:

	long int PureParam( double *params ); // calculates a and b arrays
	long int AB(double Tcrit, double omg, double k1, double k2, double k3, double Pcrit,
			double &apure, double &bpure, double &sqrAL, double &ac, double &dALdT);
//	int B(double Tcrit, double Pcrit, double &bpure);
	long int FugacityPure( void ); // Calculates the fugacity of pure species
	long int Cardano(double a2, double a1, double a0, double &z1, double &z2, double &z3);
	long int MixParam( double &amix, double &bmix);
	long int FugacityMix( double amix, double bmix,
     double &fugmix, double &zmix, double &vmix);
	long int FugacitySpec( double *fugpure, double *params  );

//	long int GetEosParam( float *params ); // Loads EoS parameters for NComp species
//  long int GetMoleFract( double *Wx ); // Loads mole fractions for NComp species
	double ObtainResults( double *ActCoef ); // returns activity coeffs and phase volume

};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// SIT model reimplementation for aqueous electrolyte solutions
class TSIT: public TSolMod
{
private:
	
	double *aZ;    // Vector of species charges (for aqueous models)
	double *aM;    // Vector of species molality (for aqueous models)
	double I;	// Ionic strength

	inline double IonicStr()
	{
	    double Is=0.;
		for(long int ii=0; ii<NComp; ii++ )
		  Is += aZ[ii]*aZ[ii]*aM[ii];
		return 0.5*Is;
	}

	
public:

	// Constructor
	TSIT( long int NSpecies, long int NParams, long int NPcoefs, long int MaxOrder,
	         long int NPperDC, double T_k, double P_bar, char Mod_Code,
	         long int* arIPx, double* arIPc, double* arDCc,
	         double *arWx, double *arlnGam, double *aphVOL, double *arM, double *arZ, 
	         double dW, double eW );
	
	// Destructor
	~TSIT() { }
	
    long int MixMod(); 

    // Calculation of internal tables (at each GEM iteration)
	long int PTparam() { return 0; }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//   Van Laar model for solid solutions (c) TW March 2007
// References:  Holland & Powell (2003)
class TVanLaar: public TSolMod
{
private:
	double *Wu;
	double *Ws;
	double *Wv;
	double *Wpt;   // Interaction coeffs at P-T
	double *Phi;   // Mixing terms
	double *PsVol; // End member volume parameters
	
	void alloc_internal();
	void free_internal();

public:

	// Constructor
	TVanLaar( long int NSpecies, long int NParams, long int NPcoefs, long int MaxOrder,
	         long int NPperDC, double T_k, double P_bar, char Mod_Code,
	         long int* arIPx, double* arIPc, double* arDCc,
	         double *arWx, double *arlnGam, double *aphVOL,  
	         double dW, double eW );
	
	// Destructor
	~TVanLaar();
	
    long int MixMod(); 

    // Calculation of internal tables (at each GEM iteration)
	long int PTparam();
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Regular model for multicomponent solid solutions (c) TW March 2007
// References:  Holland & Powell (1993)
class TRegular: public TSolMod
{
private:
   double *Wu;
   double *Ws;
   double *Wv;
   double *Wpt;   // Interaction coeffs at P-T
	
	void alloc_internal();
	void free_internal();

public:

	// Constructor
	TRegular( long int NSpecies, long int NParams, long int NPcoefs, long int MaxOrder,
	         long int NPperDC, double T_k, double P_bar, char Mod_Code,
	         long int* arIPx, double* arIPc, double* arDCc,
	         double *arWx, double *arlnGam, double *aphVOL,  
	         double dW, double eW  );
	
	// Destructor
	~TRegular();
	
    long int MixMod(); 

    // Calculation of internal tables (at each GEM iteration)
	long int PTparam();
};
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Redlich-Kister model for multicomponent solid solutions (c) TW March 2007
// References: Hillert (1998)
class TRedlichKister: public TSolMod
{
private:
	   double (*Lu)[4];
	   double (*Ls)[4];
	   double (*Lcp)[4];
	   double (*Lv)[4];
	   double (*Lpt)[4];
	
	void alloc_internal();
	void free_internal();

public:

	// Constructor
	TRedlichKister( long int NSpecies, long int NParams, long int NPcoefs, long int MaxOrder,
	         long int NPperDC, double T_k, double P_bar, char Mod_Code,
	         long int* arIPx, double* arIPc, double* arDCc,
	         double *arWx, double *arlnGam, double *aphVOL,
	         double dW, double eW  );
	
	// Destructor
	~TRedlichKister();
	
    long int MixMod(); 

    // Calculation of internal tables (at each GEM iteration)
	long int PTparam();
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// NRTL model for liquid solutions (c) TW June 2008
// References: Renon and Prausnitz (1968), Prausnitz et al. (1997)
class TNRTL: public TSolMod
{
private:
	double **Tau;
	double **dTau;
	double **d2Tau;
	double **Alp;
	double **dAlp;
	double **d2Alp;
	double **G;
	double **dG;
	double **d2G;
	
	void alloc_internal();
	void free_internal();

public:

	// Constructor
	TNRTL( long int NSpecies, long int NParams, long int NPcoefs, long int MaxOrder,
	         long int NPperDC, double T_k, double P_bar, char Mod_Code,
	         long int* arIPx, double* arIPc, double* arDCc,
	         double *arWx, double *arlnGam, double *aphVOL, 
	         double dW, double eW  );
	
	// Destructor
	~TNRTL();
	
    long int MixMod(); 

    // Calculation of internal tables (at each GEM iteration)
	long int PTparam();
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Wilson model for liquid solutions (c) TW June 2008
// References: Prausnitz et al. (1997)
class TWilson: public TSolMod
{
private:
	double **Lam;
	double **dLam;
	double **d2Lam;
	
	void alloc_internal();
	void free_internal();

public:

	// Constructor
	TWilson( long int NSpecies, long int NParams, long int NPcoefs, long int MaxOrder,
	         long int NPperDC, double T_k, double P_bar, char Mod_Code,
	         long int* arIPx, double* arIPc, double* arDCc,
	         double *arWx, double *arlnGam, double *aphVOL, 
	         double dW, double eW  );
	
	// Destructor
	~TWilson();
	
    long int MixMod(); 

    // Calculation of internal tables (at each GEM iteration)
	long int PTparam();
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class TPitzer: public TSolMod
{

private:
	long int Nc;	 // Number of cations
	long int Na;     // Number of anions
	long int Nn;     // Number of neutral species
	long int Ns;     // Total number of aqueous species (without H2O); index of H2O in aq phase
	             // Conversion of species indexes between aq phase and Pitzer parameter tables
	long int *xcx;   // list of indexes of Nc cations in aqueous phase
	long int *xax;   // list of indexes of Na anions in aq phase
	long int *xnx;   // list of indexes of Nn neutral species in aq phase
	double *aZ;    // Vector of species charges (for aqueous models)
	double *aM;    // Vector of species molality (for aqueous models)

    double Aphi; //------------ Computing A- Factor
	double I;  //----------- Ionic Strength
	double Is;  //----------- Ionic Strength
	double Ffac; //----------- ------- F-Factor
	double	 Zfac; //----------- Z- Term

				// Input parameter arrays
	double *abet0;    // Beta0 table for cation-anion interactions [Nc][Na]
	double *abet1;    // Beta1 table for cation-anion interactions [Nc][Na]
	double *abet2;    // Beta2 table for cation-anion interactions [Nc][Na]
	double *aCphi;    // Cphi  table for cation-anion interactions [Nc][Na]
	double *aLam;     // Lam table for neutral-cation interactions [Nn][Nc]
	double *aLam1;    // Lam1 table for neutral-anion interactions [Nn][Na]
	double *aTheta;   // Theta table for cation-cation interactions [Nc][Nc]
	double *aTheta1;  // Theta1 table for anion-anion interactions [Na][Na]
	double *aPsi;     // Psi array for cation-cation-anion interactions [Nc][Nc][Na]
	double *aPsi1;    // Psi1 array for anion-anion-cation interactions [Na][Na][Nc]
	double *aZeta;    // Zeta array for neutral-cation-anion interactions [Nn][Nc][Na]
	
                  // Work parameter arrays
/*	double *B1;      // B' table for cation-anion interactions corrected for IS [Nc][Na]
	double *B2;      // B table for cation-anion interactions corrected for IS [Nc][Na]
	double *B3;      // B_phi table for cation-anion interactions corrected for IS [Nc][Na]
	double *Phi1;    // Phi' table for anion-anion interactions corrected for IS [Na][Na]
	double *Phi2;    // Phi table for cation-cation interactions corrected for IS [Nc][Nc]
	double *Phi3;    // PhiPhi table for anion-anion interactions corrected for IS [Na][Na]
	double *C;       // C table for cation-anion interactions corrected for charge [Nc][Na]
	double *Etheta;  // Etheta table for cation-cation interactions [Nc][Nc]
	double *Ethetap; // Etheta' table for anion-anion interactions [Na][Na]
	double bk[21];   // work space
	double dk[21];   // work space
*/
	enum eTableType
	{ bet0_ = -10, bet1_ = -11, bet2_ = -12, Cphi_ = -20, Lam_ = -30, Lam1_ = -31, 
	  Theta_ = -40,  Theta1_ = -41, Psi_ = -50, Psi1_ = -51, Zeta_ = -60
	};
	
 // internal calculations
	// Calculation of Etheta and Ethetap values
	void Ecalc( double z, double z1, double I, double Aphi, 
			double& Etheta, double& Ethetap);
	inline long int getN() const
	 { return Nc+Na+Nn; }

	double Z_Term();
	double A_Factor( double T );
	double IonicStr( double& I );
	void getAlp( long int c, long int a, double& alp, double& alp1 );
	double F_Factor( double Aphi, double I, double Is );
	double lnGammaN(  long int N );
	double lnGammaM(  long int M );
	double lnGammaX(  long int X );
	double lnGammaH2O();

  // internal setup	
	void calcSizes();
	void alloc_internal();
	void free_internal();
  // calc vector of interaction parameters corrected to T,P of interest
	void PTparam( double T ); 
  // build conversion of species indexes between aq phase and Pitzer parameter tables
	void setIndexes();
	void setValues();

    
	inline long int getIc( long int jj )
    { 
		for( long int ic=0; ic<Nc; ic++ )
			if( xcx[ic] == jj )
				return ic;
		return -1;
    }
	
	inline long int getIa( long int jj )
    { 
		for( long int ia=0; ia<Na; ia++ )
			if( xax[ia] == jj )
				return ia;
		return -1;
    }

	inline long int getIn( long int jj )
    { 
		for( long int in=0; in<Nn; in++ )
			if( xnx[in] == jj )
				return in;
		return -1;
    }

	inline double sum( double* arr, long int *xx, long int Narr )
    { 
		double sum_ =0.;
		for( long int i=0; i<Narr; i++ )
          sum_ += arr[ xx[i]];
		return sum_;
    }

public:
    // Constructor

	TPitzer( long int NSpecies, long int NParams, long int NPcoefs, long int MaxOrder,
	         long int NPperDC, double T_k, double P_bar, char Mod_Code,
	         long int* arIPx, double* arIPc, double* arDCc,
	         double *arWx, double *arlnGam, double *aphVOL, double *arM, double *arZ, 
	         double dW, double eW );
	
	// Destructor
	~TPitzer();
	
    long int MixMod() 
    { return Pitzer_calc_Gamma();}

    // Calculation of internal tables (at each GEM iteration)
	long int Pitzer_Init();
	
	// Calculation of activity coefficients
	long int Pitzer_calc_Gamma( );

	void Pitzer_test_out( const char *path );

};

#define IPc( ii, jj )  ( aIPc[ (ii) * NPcoef + jj ])
#define IPx( ii, jj )  ( aIPx[ (ii) * MaxOrd + jj ])

#define mc( ii ) (aM[ xcx[ii] ])
#define ma( ii ) (aM[ xax[ii] ])
#define mn( ii ) (aM[ xnx[ii] ])
#define zc( ii ) (aZ[ xcx[ii] ])
#define za( ii ) (aZ[ xax[ii] ])

#define bet0( c,a ) ( abet0[ ((c)*Na+a) ])
#define bet1( c,a ) ( abet1[ ((c)*Na+a) ])
#define bet2( c,a ) ( abet2[ ((c)*Na+a) ])
#define Cphi( c,a ) ( aCphi[ ((c)*Na+a) ])
#define Lam( n,c )  ( aLam[ ((n)*Nc+c) ])
#define Lam1( n,a )  ( aLam[ ((n)*Na+a) ])
#define Theta( c,c1 )  ( aTheta[ ((c)*Nc+c1) ])
#define Theta1( a,a1 )  ( aTheta1[ ((a)*Na+a1) ])

#define Psi( c,c1,a )  ( aPsi[ (( (c) * Nc + c1  ) * Na + a) ])
#define Psi1( a,a1,c )  ( aPsi1[ (( (a) * Na + a1  ) * Nc + c) ])
#define Zeta( n,c,a )  ( aZeta[ (( (n) * Nc + c  ) * Na + a) ])

#endif


// _s_fgl_h
