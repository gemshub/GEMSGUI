//#include  <iostream>

#include "io_arrays.h"
#include "m_param.h"
#include "node.h"
#include <iomanip>

#ifdef IPMGEMPLUGIN
  istream& f_getline(istream& is, gstring& str, char delim);
#endif

bool _comment = true;

//===================================================================

outField MULTI_static_fields[44] =  {
  { "pa_DB" , 0, 0 },
  { "pa_DHB", 0 , 0 },
  { "pa_EPS" , 0 , 0 },
  { "pa_DK", 0 , 0 },
  { "pa_DF" , 0 , 0 },
  { "pa_DP", 0 , 0 },
  { "pa_IIM", 0 , 0 },
  { "pa_PD" , 0 , 0 },
  { "pa_PRD" , 0 , 0 },
  { "pa_AG" , 0 , 0 },
  { "pa_DGC" , 0 , 0 },
  { "pa_PSM" , 0 , 0 },
  { "pa_GAR" , 0 , 0 },
  { "pa_GAH" , 0 , 0 },
  { "pa_DS", 0 , 0 },
  { "pa_XwMin" , 0 , 0 },
  { "pa_ScMin", 0 , 0 },
  { "pa_DcMin" , 0 , 0 },
  { "pa_PhMin" , 0 , 0 },
  { "pa_ICmin" , 0 , 0 },
  { "pa_PC" , 0 , 0 },
  { "pa_DFM" , 0 , 0 },
  { "pa_DFYw" , 0 , 0 },
  { "pa_DFYaq" , 0 , 0 },
  { "pa_DFYid" , 0 , 0 },
  { "pa_DFYr" , 0 , 0 },
  { "pa_DFYh" , 0 , 0 },
  { "pa_DFYc" , 0 , 0 },
  { "pa_DFYs", 0 , 0 },
  { "pa_DW", 0 , 0 },
  { "pa_DT", 0 , 0 },
  { "pa_GAS", 0 , 0 },
  { "pa_DNS" , 0 , 0 },
  { "pa_IEPS" , 0 , 0 },
  { "pKin" , 1 , 0 },
  { "pa_DKIN" , 0 , 0 },
  { "pa_PE" , 0 , 0 },
  { "PV" , 1 , 0 },
  { "PSOL" , 1 , 0 },
  { "PAalp" , 1 , 0 },
  { "PSigm" , 1 , 0 },
  { "Lads" , 1 , 0 },
  { "FIa" , 1 , 0 },
  { "FIat" , 1 , 0 }
};

outField MULTI_dynamic_fields[27] =  {
//read dynamic values from txt file
   {  "sMod", 1 , 0 },
   {  "LsMod", 1 , 0 },
   {  "LsMdc", 1 , 0 },
   {  "B", 1 , 0 },
   {  "DCCW", 1 , 0 },
   {  "Pparc", 1 , 0 },
   {  "GEX", 1 , 0 },
   {  "lnGmf", 1 , 0 },
   {  "RLC", 1 , 0 },
   {  "RSC", 1 , 0 },
   {  "DLL", 1 , 0 },
   {  "DUL", 1 , 0 },
   {  "Aalp", 1 , 0 },
   {  "Sigw", 1 , 0 },
   {  "Sigg", 1 , 0 },
   {  "YOF", 1 , 0 },
   {  "Nfsp", 1 , 0 },
   {  "MASDT", 1 , 0 },
   {  "C1", 1 , 0 },
   {  "C2", 1 , 0 },
   {  "C3", 1 , 0 },
   {  "pCh", 1 , 0 },
   {  "SATX", 1 , 0 },
   {  "MASDJ", 1 , 0 },
   {  "SCM", 1 , 0 },
   {  "SACT", 1 , 0 },
   {  "DCads", 1 , 0 }
};


//===================================================================

void TMulti::to_text_file_gemipm( const char *path )
{
  SPP_SETTING *pa = &TProfil::pm->pa;

   //static values
   char PAalp;
   char PSigm;
   float EpsW;
   float RoW;

#ifndef IPMGEMPLUGIN
   PAalp = syp->PAalp;
   PSigm = syp->PSigm;
   EpsW = TProfil::pm->tpp->EpsW;
   RoW = TProfil::pm->tpp->RoW;
#else
   PAalp = PAalp_;
   PSigm = PSigm_;
   EpsW = EpsW_;
   RoW = RoW_;
#endif
  fstream ff( path, ios::out );
  ErrorIf( !ff.good() , path, "Fileopen error");
  TPrintArrays  prar(ff);

if( _comment )
{   ff << "# GEMIPM2K v. 0.725" << endl;
   ff << "# Prototype 12.07.2006" << endl;
   ff << "# Comments marked with #" << endl << endl;
   ff << "# Template for the ipm-dat text input file for the internal MULTI data" << endl;
   ff << "# (should be read after the DATACH file and before DATABR files)" << endl << endl;
   ff << "# ID key of the initial chemical system definition" << endl;
}
  ff << "\"" << pmp->stkey << "\"" << endl << endl;
// static data
  if( _comment )
  { ff << "## (1) Controls of the numerical behavior of the GEM IPM algorithm" << endl;
    ff << "# Minimum amount of independent component in bulk composition (except charge Zz), moles" << endl;
  }
   ff << left << setw(12) << "<pa_DB> " <<  right << setw(8) << pa->p.DB << endl;
   if( _comment )
     ff << "\n# Maximum allowed mass balance residual (moles) for major independent components" << endl;
   ff << left << setw(12) << "<pa_DHB> " << right << setw(8) <<  pa->p.DHB << endl;
   if( _comment )
     ff << "# Precision criterion of the simplex() procedure to obtain automatic initial approximation" << endl;
   ff << left << setw(12) << "<pa_EPS> " <<  right << setw(8) << pa->p.EPS << endl;
   if( _comment )
     ff << "\n# IPM convergence threshold for the Dikin criterion (1e-6 < DK < 1e-4)" << endl;
   ff << left << setw(12) << "<pa_DK> " <<  right << setw(8) << pa->p.DK << endl;
   if( _comment )
     ff << "\n# Threshold for the application of the Karpov phase stability criterion f_alpha" << endl;
   ff << left << setw(12) << "<pa_DF> " <<  right << setw(8) << pa->p.DF << endl;
   if( _comment )
     ff << "\n# Maximum allowed number of iterations in the EnterFeasibleDomain() procedure" << endl;
   ff << left << setw(12) << "<pa_DP> " << right << setw(8) << pa->p.DP << endl;
   if( _comment )
     ff << "\n# Maximum allowed number of iterations in the MainIPM_Descent() procedure" << endl;
   ff << left << setw(12) << "<pa_IIM> " << right << setw(8) <<  pa->p.IIM << endl;
   if( _comment )
     ff << "\n# Control on calling built-in Debye-Hueckel() and other models for aqueous activity coefficients" << endl;
   ff << left << setw(12) << "<pa_PD> " <<  right << setw(8) << pa->p.PD << endl;
   if( _comment )
   { ff << "\n# Positive number: control of calling IPM_gamma() on iterations of GEM EFD and IPM (default 3)" << endl;
     ff << "# Negative number: the number of additional EFD-IPM loops to improve the GEM final solution" << endl;
   }
   ff << left << setw(12) << "<pa_PRD> " <<  right << setw(8) << pa->p.PRD << endl;
   if( _comment )
     ff << "\n# Smoothing parameters controlling convergence in highly non-ideal systems " << endl;
   ff << left << setw(12) << "<pa_AG> " <<  right << setw(8) << pa->p.AG << endl;
   ff << left << setw(12) << "<pa_DGC> " <<  right << setw(8) << pa->p.DGC << endl;
   if( _comment )
     ff << "\n# Flag for using initial activity coefficients for simplex() initial approximation (1-enable, 0-disable)" << endl;
   ff << left << setw(12) << "<pa_PSM> " <<  right << setw(8) << pa->p.PSM << endl;
   if( _comment )
     ff << "# Activity coefficient values for simplex(): GAR for major and GAH for minor components" << endl;
   ff << left << setw(12) << "<pa_GAR> " <<  right << setw(8) << pa->p.GAR << endl;
   ff << left << setw(12) << "<pa_GAH> " <<  right << setw(8) << pa->p.GAH << endl;
   if( _comment )
     ff << "\n# Cutoff threshold for the amount of phase for phase elimination " << endl;
   ff << left << setw(12) << "<pa_DS> " << right << setw(8) <<  pa->p.DS << endl;
   if( _comment )
   {  ff << "\n# Cutoffs for elimination of: Xw water solvent; Sc - solid sorbent; Dc - solution or surface species; " << endl;
      ff << "# Ph - non-electrolyte solution phases" << endl;
   }
   ff << left << setw(12) << "<pa_XwMin> " <<  right << setw(8) << pa->p.XwMin << endl;
   ff << left << setw(12) << "<pa_ScMin> " <<  right << setw(8) << pa->p.ScMin << endl;
   ff << left << setw(12) << "<pa_DcMin> " <<  right << setw(8) << pa->p.DcMin << endl;
   ff << left << setw(12) << "<pa_PhMin> " <<  right << setw(8) << pa->p.PhMin << endl;
   if( _comment )
     ff << "\n# Minimal ionic strength below which the activity coefficients for aqueous species are set to 1" << endl;
   ff << left << setw(12) << "<pa_ICmin> " <<  right << setw(8) << pa->p.ICmin << endl;
   if( _comment )
     ff << "\n# Mode of Selekt2() procedure operation" << endl;
   ff << left << setw(12) << "<pa_PC> " <<  right << setw(8) << pa->p.PC << endl;
   if( _comment )
     ff << "# Threshold of Karpov stability criterion for insertion of a phase in Selekt2() procedure" << endl;
   ff << left << setw(12) << "<pa_DFM> " <<  right << setw(8) << pa->p.DFM << endl;
   if( _comment )
   {  ff << "# Insertion amounts used after the simplex() initial approximation and in Selekt2() algorithm" << endl;
      ff << "# DFYw - water solvent;" << endl;
   }
   ff << left << setw(12) << "<pa_DFYw> " <<  right << setw(8) << pa->p.DFYw << endl;
   if( _comment )
     ff << "# DFYaq - aqueous species;" << endl;
   ff << left << setw(12) << "<pa_DFYaq> " <<  right << setw(8) << pa->p.DFYaq << endl;
   if( _comment )
     ff << "\n# DFYid - ideal solution components;" << endl;
//   ff << left << setw(12) << "<pa_PC> " <<  right << setw(8) << pa->p.PC << endl;
   ff << left << setw(12) << "<pa_DFYid> " <<  right << setw(8) << pa->p.DFYid << endl;
   if( _comment )
     ff << "# DFYr - major solution components;" << endl;
   ff << left << setw(12) << "<pa_DFYr> " <<  right << setw(8) << pa->p.DFYr << endl;
   if( _comment )
     ff << "# DFYh - minor solution components;" << endl;
   ff << left << setw(12) << "<pa_DFYh> " <<  right << setw(8) << pa->p.DFYh << endl;
   if( _comment )
     ff << "# DFYc - single-component phase; " << endl;
   ff << left << setw(12) << "<pa_DFYc> " <<  right << setw(8) << pa->p.DFYc << endl;
   if( _comment )
     ff << "# Insertion amount of single-component phase (Selekt2() algorithm only)" << endl;
   ff << left << setw(12) << "<pa_DFYs> " << right << setw(8) <<  pa->p.DFYs << endl;
   if( _comment )
   {  ff << "\n# Parameters for high-accuracy IPM-2 algorithm " << endl;
      ff << "# Number of the IPM-2 enhancement loops for high-accuracy mass balance (from 0 to 14)" << endl;
   }
   ff << left << setw(12) << "<pa_DW> " << right << setw(8) << pa->p.DW  << endl;
   if( _comment )
     ff << "# Exponent for dual-thermodynamic restoring of low amounts of solutes (+2 to -5), relative to DHBM" << endl;
   ff << left << setw(12) << "<pa_DT> " << right << setw(8) << pa->p.DT  << endl;
   if( _comment )
     ff << "\n# IPM2 balance accuracy control factor DHBM[i]/b[i]" << endl;
   ff << left << setw(12) << "<pa_GAS> " << right << setw(8) <<  pa->p.GAS << endl;
//   ff << "# 'pa_DG'        1e-30  now internal in LU decomposition procedure from JAMA-TNT" << endl << endl;
//  ff << left << setw(12) << "<pa_DG> " <<  right << setw(8) << pa->p.DG << endl;
   if( _comment )
     ff << "# Standard surface density (nm-2) for calculating activity of surface species" << endl;
   ff << left << setw(12) << "<pa_DNS> " <<  right << setw(8) << pa->p.DNS << endl;
   if( _comment )
     ff << "# Control parameter of SACT calculation in sorption/surface complexation models" << endl;
   ff << left << setw(12) << "<pa_IEPS> " <<  right << setw(8) << pa->p.IEPS << endl;
   if( _comment )
     ff << "\n# Flag for using metastability/kinetic constraints on calculated DC amounts (see dll, dul arrays)" << endl;
   ff << left << setw(12) << "<pKin> " <<  right << setw(8) << pmp->PLIM << endl;
   if( _comment )
     ff << "# Tolerance on amount of DC with two-side metastability constraints set in dll, dul (moles) " << endl;
   ff << left << setw(12) << "<pa_DKIN> " <<  right << setw(8) << pa->p.DKIN << endl;
//   ff << "# 'pa_PLLG'          0 used only in GEMS-PSI shell" << endl;
//   ff << left << setw(12) << "<pa_PLLG> " <<  right << setw(8) << pa->p.PLLG << endl;
   if( _comment )
     ff << "\n# Flag for using electroneutrality condition in GEM IPM calculations " << endl;
   ff << left << setw(12) << "<pa_PE> " <<  right << setw(8) << pa->p.PE << endl;
//   ff << "# 'E'                1" << endl;
//   ff << left << setw(12) << "<E> " <<  right << setw(8) << pmp->E << endl;
   if( _comment )
     ff << "\n# Flag for the volume balance constraint (on Vol IC)" << endl;
   ff << left << setw(12) << "<PV> " <<  right << setw(8) << pmp->PV << endl;
//   ff << "# These dimensions can be calculated from the DATACH information" << endl;
//   ff << "# 'Ls'              23" << endl;
//   ff << "# 'LO'              18" << endl;
//   ff << "# 'PG'               4" << endl;
//   ff << left << setw(12) << "<Ls> " <<  right << setw(8) << pmp->Ls << endl;
//   ff << left << setw(12) << "<LO> " <<  right << setw(8) << pmp->LO << endl;
//   ff << left << setw(12) << "<PG> " <<  right << setw(8) << pmp->PG << endl;
   if( _comment )
       ff << "\n# Total number of DCs in liquid hydrocarbon phases" << endl;
   ff << left << setw(12) << "<PSOL> " <<  right << setw(8) << pmp->PSOL << endl;
//   ff << "# Do not know if this stuff is really necessary" << endl;
//   ff << "# 'GWAT'         55.51" << endl;
//   ff << "# 'EpsW'       78.2451" << endl;
//   ff << "# 'RoW'       0.997061" << endl << endl;
//   ff << left << setw(12) << "<GWAT> " <<  right << setw(8) << pmp->GWAT << endl;
//   ff << left << setw(12) << "<EpsW> " <<  right << setw(8) << EpsW << endl;
//   ff << left << setw(12) << "<RoW>  " <<  right << setw(8) << RoW << endl;
   if( _comment )
     ff << "\n# Flag for using (+) or ignoring (-) specific surface areas of phases " << endl;
   ff << left << setw(12) << "<PAalp> " <<  right << setw(6) <<
      "\'" << PAalp << "\'" << endl;
   if( _comment )
    ff << "\n# Flag for using (+) or ignoring (-) specific surface free energies of phase interfaces " << endl;
   ff << left << setw(12) << "<PSigm> " <<  right << setw(6) <<
      "\'" << PSigm << "\'" << endl;
   if( _comment )
   {  ff << "\n## (2) Important dimensionalities that affect memory allocation" << endl;
      ff << "# Total number of dependent components in sorption phases included in this system" << endl;
   }
   ff << left << setw(12) << "<Lads> " <<  right << setw(8) << pmp->Lads << endl;
   if( _comment )
     ff << "# Number of sorption phases included in this system" << endl;
   ff << left << setw(12) << "<FIa> " <<  right << setw(8) << pmp->FIa << endl;
   if( _comment )
     ff << "# Allowed number of surface types per adsorption phase (default: 6 if FIa > 0)" << endl;
   ff << left << setw(12) << "<FIat> " <<  right << setw(8) << pmp->FIat << endl << endl;
//   ff << left << setw(12) << "<FIat> " <<  right << setw(8) << pmp->FIat << endl;
//   ff << left << setw(12) << "<sitNc> " <<  right << setw(8) << pmp->sitNcat << endl;
//   ff << left << setw(12) << "<sitNa> " <<  right << setw(8) << pmp->sitNan << endl;

ff << "\n<EndStatic>\n\n";
//dynamic arrays
if( pm.FIs > 0 && pm.Ls > 0 )
{
  if( _comment )
  {   ff << "## (3) Initial data for multicomponent phases (see DATACH file for dimension nPHs)" << endl;
      ff << "# Codes for mixing models of multicomponent phases";
  }
  prar.writeArray(  "sMod", pmp->sMod[0], pmp->FIs, 6 );
  int LsModSum = 0;
  int LsMdcSum = 0;
  for(int i=0; i<pmp->FIs; i++)
  {
     LsModSum += pmp->LsMod[i];
     LsMdcSum += (pmp->LsMdc[i]*pmp->L1[i]);
  }

  if( _comment )
  {  ff << "\n\n# Dimensions for parameters of non-ideal mixing models for each multicomponent phase" << endl;
     ff << "# Number of parameters per phase";
  }
  prar.writeArray(  "LsMod", pmp->LsMod, pmp->FIs);
  if(LsModSum )
   {
     if( _comment )
        ff << "\n\n# Parameters of non-ideal mixing models for multicomponent phases ";
    prar.writeArray(  "PMc", pmp->PMc,  LsModSum);
   }
   if( _comment )
     ff << "\n\n# Number of parameters per component of the phase";
   prar.writeArray(  "LsMdc", pmp->LsMdc, pmp->FIs);
   if(LsMdcSum )
   {   if( _comment )
          ff << "\n\n# Parameters of non-ideal mixing models for components in the phase ";
    prar.writeArray(  "DMc", pmp->DMc,  LsMdcSum);
   }
  }
  if( _comment )
  {  ff << "\n\n## (4) Some data arrays which are not provided in DATACH and DATABR files" << endl;
     ff << "# Full total bulk composition of the initial system (vector b) - see DATACH file for dimension nIC";
  }
  prar.writeArray(  "B", pmp->B,  pmp->N);
   if( _comment )
   {  ff << "\n\n# Initial data for DCs - see DATACH file for dimensions nDC, nDCs" << endl;
      ff << "# generic DC classes (asymmetric, solvent, ideal, single)";
   }
  prar.writeArray(  "DCCW", pmp->DCCW,  pmp->L, 1);
   if( _comment )
      ff << "\n\n# Partial pressures (fugacities) of dependent components (for setting constant chemical potentials)";
  prar.writeArray(  "Pparc", pmp->Pparc,  pmp->L);
 //  ff << "\n\n# This is not necessary - can be calculated from G0 ???????????";
 // prar.writeArray(  "G0", pmp->G0,  pmp->L);
   if( _comment )
      ff << "\n\n# DC G0 increments for adjustments";
  prar.writeArray(  "GEX", pmp->GEX,  pmp->L);
   if( _comment )
      ff << "\n\n# DC Fixed (start) activity coefficients";
  prar.writeArray(  "lnGmf", pmp->lnGmf,  pmp->L);
//   if( pmp->E )
//   {
//     if( _comment )
//        ff << "\n\n# DC Unit formula charges - can be extracted from the stoich. matrix ????";
 //   prar.writeArray(  "EZ", pmp->EZ,  pmp->L);
//   }
   if( _comment )
   {  ff << "\n\n# (5) Section for metastability/ kinetic constraints" << endl;
      ff << "# Code of metastability/kinetic constraints for DCs";
   }
  prar.writeArray(  "RLC", pmp->RLC, pmp->L, 1 );
   if( _comment )
     ff << "\n\n# Units of metastability/kinetic constraints for DCs (see vectors dul, dll)";
  prar.writeArray(  "RSC", pmp->RSC, pmp->L, 1 );
   if( _comment )
     ff << "\n\n# Full vector of lower metastability constraints on DC amounts in the system";
  prar.writeArray(  "DLL", pmp->DLL,  pmp->L);
   if( _comment )
     ff << "\n\n# Full vector of upper metastability constraints on DC amounts in the system";
  prar.writeArray(  "DUL", pmp->DUL,  pmp->L);
   if( _comment )
   {  ff << "\n\n# (6) Initial data for phases" << endl;
      ff << "\n# Specific surface areas of phases (whole list)";
   }
  prar.writeArray(  "Aalp", pmp->Aalp,  pmp->FI);
   if( PSigm != S_OFF )
   {
      if( _comment )
         ff << "\n\n# Specific surface free energy for phase-water interface";
     prar.writeArray(  "Sigw", pmp->Sigw,  pmp->FI);
      if( _comment )
         ff << "\n\n# Specific surface free energy for phase-gas interface";
     prar.writeArray(  "Sigg", pmp->Sigg,  pmp->FI);
   }
   if( _comment )
      ff << "\n\n# Surface energy or metastability parameters for phases";
  prar.writeArray(  "YOF", pmp->YOF,  pmp->FI);
   if( pm.FIat > 0 && /*pm.Lads > 0 &&Sveta 12/09/99*/ pm.FIs > 0 )
    { /* ADSORPTION AND ION EXCHANGE */
      if( _comment )
      {  ff << "\n\n# (7) Initial data for sorption" << endl;
         ff << "\n# Function of sorbent surface allocated to surface types";
      }
     prar.writeArray(  "Nfsp", &pmp->Nfsp[0][0], pmp->FIs*pmp->FIat, pmp->FIat);
      if( _comment )
        ff << "\n# Total maximum site  density per surface type, mkmol/g";
     prar.writeArray(  "MASDT", &pmp->MASDT[0][0], pmp->FIs*pmp->FIat, pmp->FIat);
      if( _comment )
        ff << "\n# Inner capacitance density parameter C1 (TLM, BSM, CCM), F/m2";
     prar.writeArray(  "C1", &pmp->XcapA[0][0], pmp->FIs*pmp->FIat, pmp->FIat);
      if( _comment )
        ff << "\n# Outer capacitance density parameter C2 (TLM, 3LM), F/m2";
     prar.writeArray(  "C2", &pmp->XcapB[0][0], pmp->FIs*pmp->FIat, pmp->FIat);
      if( _comment )
        ff << "\n# Third capacitance density parameter C3 (reserved)";
     prar.writeArray(  "C3", &pmp->XcapF[0][0], pmp->FIs*pmp->FIat, pmp->FIat);
      if( _comment )
        ff << "\n# Density of permanent surface type charge (mkeq/m2)";
     prar.writeArray(  "pCh", &pmp->Xetaf[0][0], pmp->FIs*pmp->FIat, pmp->FIat);

     if( _comment )
     {   ff << "\n# Setup of surface sites and specres: link to";
         ff << "\n# [0] surface type; [1] sorbent emd member;";
         ff << "\n# [2] surface site in surf. type; [3] surface EDL plane";
     }
     prar.writeArray(  "SATX", &pmp->SATX[0][0], pmp->Lads*4, 4);
      if( _comment )
      {  ff << "\n# Parameters of surface binding model:";
         ff << "\n# [0] max site density mmol/g; [1] charge allocated to 0 plane;";
         ff << "\n# [2] charge allocated to beta -or third plane; [3] Frumuin interaction parameter;";
         ff << "\n# [4] dentateness or CN; [5] reserved isoterm parameter.";
      }
     prar.writeArray(  "MASDJ", &pmp->MASDJ[0][0], pmp->Lads*DFCN, DFCN);
      if( _comment )
         ff << "\n# Classifier of EDL models applied to surface types.";
     prar.writeArray(  "SCM", pmp->SCM[0], pmp->FIs, pmp->FIat );
      if( _comment )
         ff << "\n# Classifier of applied SACT terms.";
     prar.writeArray(  "SACT", pmp->SATT, pmp->Lads, 1 );
      if( _comment )
         ff << "\n# Classifier of cpecies in sorption phase.";
     prar.writeArray(  "DCads", pmp->DCC3, pmp->Lads, 1 );
    }
//outArray( ff, "Vol", pmp->Vol,  pmp->L);
//outArray( ff, "G0", pmp->G0,  pmp->L);
//outArray( ff, "PUL", pmp->PUL,  pmp->L);
//outArray( ff, "PLL", pmp->PLL,  pmp->L);
//outArray( ff, "lnGam", pmp->lnGam,  pmp->L);
//outArray( ff, "F0", pmp->F0,  pmp->L);

/*
   if( pm.sitNcat*pm.sitNcat )
    prar.writeArray(  "sitE", pmp->sitE, pmp->sitNcat*pmp->sitNan );
   if( pm.sitNcat )
    prar.writeArray(  "sitXc", pmp->sitXcat, pmp->sitNcat );
   if( pm.sitNan )
     prar.writeArray(  "sitXa", pmp->sitXan, pmp->sitNan );
*/
 if( _comment )
   ff << "\n\n# End of file" << endl;

}

void TMulti::from_text_file_gemipm( const char *path )
{
  SPP_SETTING *pa = &TProfil::pm->pa;
  DATACH  *dCH = TNode::na->pCSD();
  int ii, nfild;

   //static values
   char PAalp;
   char PSigm;
   float EpsW;
   float RoW;

  memset( &pm.N, 0, 38*sizeof(short));
  memset( &pm.TC, 0, 55*sizeof(double));
  // get sizes from DATACH
  pmp->TC = pmp->P = 0;
  pmp->N = pmp->NR = dCH->nIC;
  pmp->L = dCH->nDC;
  pmp->FI = dCH->nPH;
  pmp->FIs = dCH->nPS;
  pmp->Ls = 0; //dCH->nDCs;
  for( ii=0; ii<dCH->nPS; ii++)
  {
    pmp->Ls += dCH->nDCinPH[ii];
    if( dCH->ccPH[ii] == 'a' )
     pmp->LO = pmp->Ls-1;
    if( dCH->ccPH[ii] == 'g' || dCH->ccPH[ii] == 'p' || dCH->ccPH[ii] == 'f')
      pmp->PG = dCH->nDCinPH[ii];
  }
  // read sizes and constants from txt file
  fstream ff( path, ios::in );
  ErrorIf( !ff.good() , path, "Fileopen error");

// static data
   TReadArrays  rdar( 44, MULTI_static_fields, ff);
   gstring str;
   rdar.skipSpace();
   f_getline( ff, str, '\n');
   memcpy( pmp->stkey, str.c_str(), EQ_RKLEN );

   nfild = rdar.findNext();
   while( nfild >=0 )
   {
     switch( nfild )
     { case 0: rdar.readArray( "pa_DB" , &pa->p.DB, 1);
               break;
       case 1: rdar.readArray("pa_DHB", &pa->p.DHB, 1);
               break;
       case 2: rdar.readArray("pa_EPS" , &pa->p.EPS, 1);
               break;
       case 3: rdar.readArray("pa_DK" , &pa->p.DK, 1);
               break;
       case 4: rdar.readArray("pa_DF" , &pa->p.DF, 1);
               break;
       case 5: rdar.readArray("pa_DP", &pa->p.DP, 1);
               break;
       case 6: rdar.readArray("pa_IIM", &pa->p.IIM, 1);
               break;
       case 7: rdar.readArray("pa_PD" , &pa->p.PD, 1);
               break;
       case 8: rdar.readArray("pa_PRD" , &pa->p.PRD, 1);
               break;
       case 9: rdar.readArray("pa_AG" , &pa->p.AG, 1);
               break;
       case 10: rdar.readArray("pa_DGC" , &pa->p.DGC, 1);
               break;
       case 11: rdar.readArray("pa_PSM" , &pa->p.PSM, 1);
               break;
       case 12: rdar.readArray("pa_GAR" , &pa->p.GAR, 1);
              break;
       case 13: rdar.readArray("pa_GAH" , &pa->p.GAH, 1);
              break;
       case 14: rdar.readArray("pa_DS", &pa->p.DS, 1);
              break;
       case 15: rdar.readArray("pa_XwMin" , &pa->p.XwMin, 1);
              break;
       case 16: rdar.readArray("pa_ScMin" , &pa->p.ScMin, 1);
              break;
       case 17: rdar.readArray("pa_DcMin" , &pa->p.DcMin, 1);
              break;
       case 18: rdar.readArray("pa_PhMin" , &pa->p.PhMin, 1);
              break;
       case 19: rdar.readArray("pa_ICmin" , &pa->p.ICmin, 1);
              break;
       case 20: rdar.readArray("pa_PC" , &pa->p.PC, 1);
              break;
       case 21: rdar.readArray("pa_DFM" , &pa->p.DFM, 1);
              break;
       case 22: rdar.readArray("pa_DFYw" , &pa->p.DFYw, 1);
              break;
       case 23: rdar.readArray("pa_DFYaq" , &pa->p.DFYaq, 1);
              break;
       case 24: rdar.readArray("pa_DFYid" , &pa->p.DFYid, 1);
              break;
       case 25: rdar.readArray("pa_DFYr" , &pa->p.DFYr, 1);
              break;
       case 26: rdar.readArray("pa_DFYh" , &pa->p.DFYh, 1);
              break;
       case 27: rdar.readArray("pa_DFYc" , &pa->p.DFYc, 1);
              break;
       case 28: rdar.readArray("pa_DFYs", &pa->p.DFYs, 1);
              break;
       case 29: rdar.readArray("pa_DW", &pa->p.DW , 1);
              break;
       case 30: rdar.readArray("pa_DT", &pa->p.DT , 1);
              break;
       case 31: rdar.readArray("pa_GAS", &pa->p.GAS, 1);
              break;
       case 32: rdar.readArray("pa_DNS" , &pa->p.DNS, 1);
              break;
       case 33: rdar.readArray("pa_IEPS" , &pa->p.IEPS, 1);
              break;
       case 34: rdar.readArray("pKin" , &pmp->PLIM, 1);
              break;
       case 35: rdar.readArray("pa_DKIN" , &pa->p.DKIN, 1);
              break;
       case 36: rdar.readArray("pa_PE" , &pa->p.PE, 1);
                 pmp->E = pa->p.PE;
              break;
       case 37: rdar.readArray("PV" , &pmp->PV, 1);
              break;
       case 38: rdar.readArray("PSOL" , &pmp->PSOL, 1);
              break;
       case 39: rdar.readArray("PAalp" , &PAalp, 1, 1);
              break;
       case 40: rdar.readArray("PSigm" , &PSigm, 1, 1);
              break;
       case 41: rdar.readArray("Lads" , &pmp->Lads, 1);
              break;
       case 42: rdar.readArray("FIa" , &pmp->FIa, 1);
              break;
       case 43: rdar.readArray("FIat" , &pmp->FIat, 1);
              break;
    }
   nfild = rdar.findNext();
 }

 // testing read
 gstring ret = rdar.testRead();
 if( !ret.empty() )
  { ret += " - fields must be readed from MULTY structure";
    Error( "Error", ret);
  }

//   if( dCH->ccPH[0] == PH_AQUEL )
//   {
//     RoW = dCH->roW[0];
//     EpsW = dCH->epsW[0];
//   }
//   else
//  {
    RoW = 0.99706137180;
    EpsW = 78.245147705;
//  }

#ifndef IPMGEMPLUGIN
//   syp->PAalp = PAalp;
//   syp->PSigm = PSigm;
#else
   PAalp_ = PAalp;
   PSigm_ = PSigm;
   EpsW_ = EpsW;
   RoW_ =  RoW;
#endif

   //realloc memory
#ifdef IPMGEMPLUGIN
   multi_realloc( PAalp, PSigm );
#endif

// get dynamic data from DATACH file
  for( ii=0; ii<dCH->nPH; ii++)
    pmp->L1[ii] = dCH->nDCinPH[ii];

  memcpy( pmp->A, dCH->A, dCH->nIC*dCH->nDC*sizeof(float));
  if( pmp->EZ )
  { int iZ=-1;
    for(  ii=0; ii<dCH->nDC; ii++ )
     if( dCH->ccIC[ii] == IC_CHARGE )
         break;
    if( ii< dCH->nDC )
    { iZ = ii;
      for( ii=0; ii<dCH->nDC; ii++)
          pmp->EZ[ii] = pmp->A[pmp->N*ii+iZ];
    }
  }
  for( ii=0; ii< dCH->nIC; ii++ )
   pmp->Awt[ii]  = dCH->ICmm[ii];
  memcpy( pmp->MM, dCH->DCmm, dCH->nDC*sizeof(double));

  memset( pmp->SB, ' ', MaxICN*dCH->nIC*sizeof(char));
  for( ii=0; ii< dCH->nIC; ii++ )
  {   memcpy( pmp->SB[ii], dCH->ICNL[ii], MaxICN*sizeof(char) );
      pmp->SB[ii][MaxICN] = dCH->ccIC[ii];
  }

  memcpy( pmp->SM, dCH->DCNL, MaxDCN*dCH->nDC*sizeof(char));

  memset( pmp->SF, ' ', MaxPHN*dCH->nPH*sizeof(char) );
  for( ii=0; ii< dCH->nPH; ii++ )
  {  memcpy( pmp->SF[ii]+4, dCH->PHNL[ii], MaxPHN*sizeof(char));
     pmp->SF[ii][0] = dCH->ccPH[ii];
  }

  memcpy( pmp->ICC, dCH->ccIC, dCH->nIC*sizeof(char));
  memcpy( pmp->DCC, dCH->ccDC, dCH->nDC*sizeof(char));
// !!!!  memcpy( pmp->DCCW, dCH->ccDCW, dCH->nDC*sizeof(char));
  memcpy( pmp->PHC, dCH->ccPH, dCH->nPH*sizeof(char));

//read dynamic values from txt file
   TReadArrays  rddar( 27, MULTI_dynamic_fields, ff);

// set up array flags for always fielfs

 if( !( pm.FIs > 0 && pm.Ls > 0 ) )
 {
   rddar.setNoAlws( short(0) /*"sMod"*/);
   rddar.setNoAlws( 1 /*"LsMod"*/);
   rddar.setNoAlws( 2 /*"LsMdc"*/);
 }
 if( PSigm == S_OFF )
 {
   rddar.setNoAlws( 13 /*"Sigw"*/);
   rddar.setNoAlws( 14 /*"Sigg"*/);
 }
 if( !( pm.FIat > 0 &&  pm.FIs > 0 ) )
 { /* ADSORPTION AND ION EXCHANGE */
   rddar.setNoAlws( 16 /*"Nfsp"*/);
   rddar.setNoAlws( 17 /*"MASDT"*/);
   rddar.setNoAlws( 18 /*"C1"*/);
   rddar.setNoAlws( 19 /*"C2"*/);
   rddar.setNoAlws( 20 /*"C3"*/);
   rddar.setNoAlws( 21 /*"pCh"*/);
   rddar.setNoAlws( 22 /*"SATX"*/);
   rddar.setNoAlws( 23 /*"MASDJ"*/);
   rddar.setNoAlws( 24 /*"SCM"*/);
   rddar.setNoAlws( 25 /*"SACT"*/);
   rddar.setNoAlws( 26 /*"DCads"*/);
 }

  // Read dynamic arrays
  nfild = rddar.findNext();
  while( nfild >=0 )
  {
    switch( nfild )
    { case 0: if( !pmp->sMod )
                Error( "Error", "Array sMod not used in task");
              rddar.readArray( "sMod" , pmp->sMod[0], pmp->FIs, 6 );
              break;
      case 1:{ if( !pmp->LsMod )
                Error( "Error", "Array LsMod not used in task");
              rddar.readArray( "LsMod" , pmp->LsMod, pmp->FIs) ;
              int LsModSum = 0;
              for(int i=0; i<pmp->FIs; i++)
                 LsModSum += pmp->LsMod[i];
             if(LsModSum )
              rddar.readArray( "PMc", pmp->PMc,  LsModSum);
              break;
             }
      case 2: { if( !pmp->LsMdc )
                   Error( "Error", "Array LsMdc not used in task");
                rddar.readArray( "LsMdc" , pmp->LsMdc, pmp->FIs );
                int LsMdcSum = 0;
                for(int i=0; i<pmp->FIs; i++)
                 LsMdcSum += (pmp->LsMdc[i]*pmp->L1[i]);
                if(LsMdcSum )
                 rddar.readArray( "DMc", pmp->DMc,  LsMdcSum);
                break;
              }
      case 3: rddar.readArray( "B", pmp->B,  pmp->N);
              break;
      case 4: rddar.readArray( "DCCW", pmp->DCCW,  pmp->L, 1);
              break;
      case 5: rddar.readArray( "Pparc", pmp->Pparc,  pmp->L);
              break;
      case 6: rddar.readArray( "GEX", pmp->GEX,  pmp->L);
              break;
      case 7: rddar.readArray( "lnGmf", pmp->lnGmf,  pmp->L);
              break;
      case 8: rddar.readArray( "RLC", pmp->RLC, pmp->L, 1 );
              break;
      case 9: rddar.readArray( "RSC", pmp->RSC, pmp->L, 1 );
              break;
      case 10: rddar.readArray( "DLL", pmp->DLL,  pmp->L);
              break;
      case 11: rddar.readArray( "DUL", pmp->DUL,  pmp->L);
              break;
      case 12: rddar.readArray( "Aalp", pmp->Aalp,  pmp->FI);
              break;
      case 13: if( !pmp->Sigw )
                Error( "Error", "Array Sigw not used in task");
              rddar.readArray( "Sigw", pmp->Sigw,  pmp->FI);
              break;
      case 14: if( !pmp->Sigg )
                Error( "Error", "Array Sigg not used in task");
              rddar.readArray( "Sigg", pmp->Sigg,  pmp->FI);
              break;
      case 15: rddar.readArray( "YOF", pmp->YOF,  pmp->FI);
              break;
      case 16: if( !pmp->Nfsp )
                Error( "Error", "Array Nfsp not used in task");
              rddar.readArray( "Nfsp", &pmp->Nfsp[0][0], pmp->FIs*pmp->FIat);
              break;
      case 17: if( !pmp->MASDT )
                Error( "Error", "Array MASDT not used in task");
              rddar.readArray( "MASDT", &pmp->MASDT[0][0], pmp->FIs*pmp->FIat);
              break;
      case 18: if( !pmp->XcapA )
                Error( "Error", "Array XcapA not used in task");
              rddar.readArray( "C1", &pmp->XcapA[0][0], pmp->FIs*pmp->FIat);
              break;
      case 19: if( !pmp->XcapB )
                Error( "Error", "Array XcapB not used in task");
              rddar.readArray( "C2", &pmp->XcapB[0][0], pmp->FIs*pmp->FIat);
              break;
      case 20: if( !pmp->XcapF )
                Error( "Error", "Array XcapF not used in task");
              rddar.readArray( "C3", &pmp->XcapF[0][0], pmp->FIs*pmp->FIat);
              break;
      case 21: if( !pmp->Xetaf )
                Error( "Error", "Array Xetaf not used in task");
              rddar.readArray( "pCh", &pmp->Xetaf[0][0], pmp->FIs*pmp->FIat);
              break;
      case 22: if( !pmp->SATX )
                Error( "Error", "Array SATX not used in task");
              rddar.readArray( "SATX", &pmp->SATX[0][0], pmp->Lads*4);
              break;
      case 23: if( !pmp->MASDJ )
                Error( "Error", "Array MASDJ not used in task");
              rddar.readArray( "MASDJ", &pmp->MASDJ[0][0], pmp->Lads*DFCN);
              break;
      case 24: if( !pmp->SCM )
                Error( "Error", "Array SCM not used in task");
              rddar.readArray( "SCM", pmp->SCM[0], pmp->FIs, pmp->FIat );
              break;
      case 25: if( !pmp->SATT )
                Error( "Error", "Array SATT not used in task");
              rddar.readArray( "SACT", pmp->SATT, pmp->Lads, 1 );
              break;
      case 26: if( !pmp->DCC3 )
                Error( "Error", "Array DCC3 not used in task");
               rddar.readArray( "DCads", pmp->DCC3, pmp->Lads, 1 );
               break;
    }
   nfild = rddar.findNext();
  }
/*
   if( pm.sitNcat*pm.sitNcat )
     rddar.readArray( "sitE", pmp->sitE, pmp->sitNcat*pmp->sitNan );
   if( pm.sitNcat )
     rddar.readArray( "sitXc", pmp->sitXcat, pmp->sitNcat );
   if( pm.sitNan )
     rddar.readArray( "sitXa", pmp->sitXan, pmp->sitNan );
*/
 // testing read
 ret = rddar.testRead();
 if( !ret.empty() )
  { ret += " - fields must be readed from MULTY structure";
    Error( "Error", ret);
  }
}

//=============================================================================
