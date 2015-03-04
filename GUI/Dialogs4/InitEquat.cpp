//-------------------------------------------------------------------
// $Id: InitEquat.cpp 968 2007-12-13 13:23:32Z gems $
//
// Declaration of windows for EquatSetup class
//
// Copyright (C) 2010  S.Dmytriyeva, D.Kulik
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (http://qt.nokia.com)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the terms of GEMS3 Development
// Quality Assurance Licence (GEMS3.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#include "EquatSetupWidget.h"
#include "m_param.h"

//  nRT = -1 TPwork
//  nRT = -2 DataBR
//  nRT  >= 0 type of SubModule
void GetListsnRT( int nRT, vector<pagesSetupData>& wnData, vector<pagesSetupData>& scalarsList )
{

  switch( nRT )
  {
  case RT_ICOMP:
     scalarsList.push_back(pagesSetupData("M0i",o_icawt));  //"Atomic (molar) mass of Independent Component "
     scalarsList.push_back( pagesSetupData("S0i", o_icass)); //"Standard-state absolute entropy S0 per g/atom "
 //    scalarsList.push_back( pagesSetupData("Cp0i",o_icacp)); //"Standard-state absolute heat capacity Cp0 per g/atom (reserved) "
 //    scalarsList.push_back( pagesSetupData("V0i",o_icavs));  //"Standard state volume V0 per g/atom (reserved) "
 //    scalarsList.push_back( pagesSetupData("Ri",o_icarad));  //"Ionic radius Angstrom (reserved) "
     scalarsList.push_back( pagesSetupData("IXi",o_icidis));  //"Relative isotope abundance (1 for chemical elements) "
     scalarsList.push_back( pagesSetupData("Valen",o_icval)); //"Default valence number (formula charge) of IC in compounds "
     scalarsList.push_back( pagesSetupData("indMT",o_icnum)); //"Index in Periodical (Mendeleev's) Table of chemical elements "
//     scalarsList.push_back( pagesSetupData("Zi",o_iccoor));   //"Default coordination number in compounds (reserved) "
     break;

  case RT_DCOMP:
     scalarsList.push_back( pagesSetupData("Zz",o_dczz));  //"Formula charge of this species (Dependent Component) "
     scalarsList.push_back( pagesSetupData("M0",o_dcmwt));  //"Molecular (gram-formula) mass, g/mol "
     scalarsList.push_back( pagesSetupData("V0d[0]",o_dcmvs));  //"Molar volume V0 of Dependent Component at standard state (J/bar)_
//     scalarsList.push_back( pagesSetupData("V0d[1]",o_dcmvs,1));  // Uncertainty of V0 (J/bar) "
     scalarsList.push_back( pagesSetupData("G0d[0]",o_dcgs)); //"Standard molar (partial molal) Gibbs energy of formation of DC from IC at Tr and Pr_
//     scalarsList.push_back( pagesSetupData("G0d[1]",o_dcgs,1)); // Uncertainty of standard molar (partial molal) Gibbs energy of formation G0u "
     scalarsList.push_back( pagesSetupData("H0d[0]",o_dchs)); // "Molar (partial molal) enthalpy of formation of DC from IC at standard state and Tr, Pr_
//     scalarsList.push_back( pagesSetupData("H0d[1]",o_dchs,1)); // Uncertainty of standard state molar enthalpy H0 (J/mol) "
     scalarsList.push_back( pagesSetupData("S0d[0]",o_dcss)); // "Absolute molar (partial molal) entropy of Dependent Component at the standard state_
//     scalarsList.push_back( pagesSetupData("S0d[1]",o_dcss,1)); //  Uncertainty of standard molar (partial molal) absolute entropy of formation "
     scalarsList.push_back( pagesSetupData("Cp0d[0]",o_dccps)); // "Absolute molar (partial molal) isobaric heat capacity Cp0 of DC at standard state and reference Tr, Pr_
//     scalarsList.push_back( pagesSetupData("Cp0d[1]",o_dccps,1)); //  Uncertainty of Cp0 at reference Tr, Pr "
//     scalarsList.push_back( pagesSetupData("PrTr[0]",o_dcptst)); //  "Reference pressure Pr (bar)_
//     scalarsList.push_back( pagesSetupData("PrTr[1]",o_dcptst,1)); //  Reference temperature Tr, C "
//     scalarsList.push_back( pagesSetupData("LamST[0]",o_dcstlam )); // "Entropy of lambda transition (J/K/mol), minerals only_
//     scalarsList.push_back( pagesSetupData("LamST[1]",o_dcstlam,1)); // Temperature of lambda transition (C), minerals only "
//     scalarsList.push_back( pagesSetupData("BetAlp[0]",o_dcbeal)); // "Coefficient of isothermal compressibility b_ (or CN/dentateness for surface species)
//     scalarsList.push_back( pagesSetupData("BetAlp[1]",o_dcbeal,1)); // Coefficient of isobaric expansibility a (or exponent parameter for surface species) "
     scalarsList.push_back( pagesSetupData("ab[0]", o_dcder)); // "Ion-size parameter ao (Debye-Hueckel) (or unit charge at zero plane for surface complexes)_
     scalarsList.push_back( pagesSetupData("ab[1]", o_dcder,1)); // Individual parameter b (optional) (or unit charge at the beta plane for surface complexes) "
     // dynamic
    scalarsList.push_back( pagesSetupData("TCint[0]", o_dccpint)); //"Lower temperature validity limit(s) for Cp = f (T) equation_
    scalarsList.push_back( pagesSetupData("TCint[1]", o_dccpint,1)); // Upper temperature validity limit(s) for Cp = f (T) equation "
    scalarsList.push_back( pagesSetupData("aiCpT[0]", o_dccp)); //  "Free term coefficient a0 of general Cp = f(T) equation_
    scalarsList.push_back( pagesSetupData("aiCpT[1]", o_dccp,1)); // Coefficient a1 of the a1*T term of general Cp = f(T) equation_
    scalarsList.push_back( pagesSetupData("aiCpT[2]", o_dccp,2)); // Coefficient a2  of the  a2*T^-2 term of general Cp = f(T) equation_
    scalarsList.push_back( pagesSetupData("aiCpT[3]", o_dccp,3)); // Coefficient a3  of the  a3*T^-0.5 term of general Cp = f(T) equation_
    scalarsList.push_back( pagesSetupData("aiCpT[4]", o_dccp,4)); // Coefficient a4  of the  a4*T^2 term of general Cp = f(T) equation_
    scalarsList.push_back( pagesSetupData("aiCpT[5]", o_dccp,5)); // Coefficient a5  of the  a5*T^3 term of general Cp = f(T) equation_
    scalarsList.push_back( pagesSetupData("aiCpT[6]", o_dccp,6)); // Coefficient a6  of the  a6*T^4 term of general Cp = f(T) equation_
    scalarsList.push_back( pagesSetupData("aiCpT[7]", o_dccp,7)); // Coefficient a7  of the  a7*T^-3 term of general Cp = f(T) equation_
    scalarsList.push_back( pagesSetupData("aiCpT[8]", o_dccp,8)); // Coefficient a8  of the  a8*T^-1 term of general Cp = f(T) equation_
    scalarsList.push_back( pagesSetupData("aiCpT[9]", o_dccp,9)); // Coefficient a9  of the  a9*T^0.5 term of general Cp = f(T) equation_
    scalarsList.push_back( pagesSetupData("aiCpT[10]", o_dccp,10)); //Coefficient a10  of the  a10*ln T term of general Cp = f(T) equation "
//    scalarsList.push_back( pagesSetupData("aiNEs[0]", o_dccpfs)); // "Empirical coefficients for non-electrolyte aqueous species "
    scalarsList.push_back( pagesSetupData("aiHKF[0]", o_dchkf)); // "Empirical coefficient a1 of the HKF EoS: cal/bar/mol_
    scalarsList.push_back( pagesSetupData("aiHKF[1]", o_dchkf)); // Empirical coefficient a2 of the HKF EoS: cal/mol_
    scalarsList.push_back( pagesSetupData("aiHKF[2]", o_dchkf)); // Empirical coefficient a3 of the HKF EoS: cal*K/bar/mol_
    scalarsList.push_back( pagesSetupData("aiHKF[3]", o_dchkf)); // Empirical coefficient a4 of the HKF EoS: cal*K/mol_
    scalarsList.push_back( pagesSetupData("aiHKF[4]", o_dchkf)); // Empirical coefficient c1 of the HKF EoS: cal/K/mol_
    scalarsList.push_back( pagesSetupData("aiHKF[5]", o_dchkf)); // Empirical coefficient c2 of the HKF EoS: cal*K/mol_
    scalarsList.push_back( pagesSetupData("aiHKF[6]", o_dchkf)); // wB Born factor of the HKF EoS: cal/mol_
//    scalarsList.push_back( pagesSetupData("aiVTP[0]", o_dcvt)); // "Coefficient  a0(T) of the molar volume equation_
//    scalarsList.push_back( pagesSetupData("aiVTP[1]", o_dcvt,1)); // Coefficient a1(T) of the molar volume equation_
//    scalarsList.push_back( pagesSetupData("aiVTP[2]", o_dcvt,2)); // Coefficient a2(T) of the molar volume equation_
//    scalarsList.push_back( pagesSetupData("aiVTP[3]", o_dcvt,3)); // Coefficient b0(P) of the molar volume equation_
//    scalarsList.push_back( pagesSetupData("aiVTP[4]", o_dcvt,4)); // Coefficient b1(P) of the molar volume equation "
//    scalarsList.push_back( pagesSetupData("CritPg[0]", o_dccritpg)); // "Critical temperature Tcr (K) for the PRSV fluid model_
//    scalarsList.push_back( pagesSetupData("CritPg[1]", o_dccritpg)); // Critical pressure Pcr (bar) for the PRSV fluid model_
//    scalarsList.push_back( pagesSetupData("CritPg[2]", o_dccritpg)); // Pitzer acentric factor W for the PRSV fluid model_
//    scalarsList.push_back( pagesSetupData("CritPg[3]", o_dccritpg)); // Parameter k1 for the PRSV fluid model_
//    scalarsList.push_back( pagesSetupData("CritPg[4]", o_dccritpg)); // Parameter k2 for the PRSV fluid model_
//    scalarsList.push_back( pagesSetupData("CritPg[5]", o_dccritpg)); // Parameter k3 for the PRSV fluid model_
    break;

  case RT_COMPOS:
    scalarsList.push_back( pagesSetupData("MasVol[0]",o_bcmvol)); // o_bcmvol  MasVol F_  1 8   N "Ms (input normalization mass, output total PCO mass)_
    scalarsList.push_back( pagesSetupData("MasVol[1]",o_bcmvol,1)); //     Vs (input normalisation volume, for specifying volume concentrations)_
    scalarsList.push_back( pagesSetupData("MasVol[2]",o_bcmvol,2)); //      Mw (input mass of water-solvent, for specifying molalities)_
    scalarsList.push_back( pagesSetupData("MasVol[3]",o_bcmvol,3)); //      Maq (input mass of aqueous solution for dissolved mass concentrations)_
    scalarsList.push_back( pagesSetupData("MasVol[4]",o_bcmvol,4)); //    Vaq (input volume of aqueous solution for molarities)_
    scalarsList.push_back( pagesSetupData("MasVol[5]",o_bcmvol,5)); //     Pg (input pressure in gas, for partial pressures)_
    scalarsList.push_back( pagesSetupData("MasVol[6]",o_bcmvol,6)); //      Ns (input total number of IC moles in this PCO)_
    scalarsList.push_back( pagesSetupData("MasVol[7]",o_bcmvol,7)); //     Mf (amount/concentration of compound specified by Ctext) "
// to discuss dynamic objects in Compos
    break;

  case RT_REACDC:
    scalarsList.push_back( pagesSetupData("Zz_",o_rezz));  // "Formula charge of a new species (Dependent Component) "
    scalarsList.push_back( pagesSetupData("M0_",o_remwt)); // "Molecular (gram-formula) mass of the new DC (species), g/mol "
    scalarsList.push_back( pagesSetupData("logKr[0]",o_reks)); // "Value of the equilibrium constant K of reaction at Tr,Pr_
    scalarsList.push_back( pagesSetupData("logKr[1]",o_reks,1)); // log K of reaction at Tr, Pr (for isotopic forms: 1000 ln(alpha) )_
//    scalarsList.push_back( pagesSetupData("logKr[2]",o_reks,2)); // Uncertainty of logK,  dev(logK) "
    scalarsList.push_back( pagesSetupData("G0r[0]",o_regs)); // "dGr - standard Gibbs energy effect of the reaction at Tr, Pr (J/mol)_
    scalarsList.push_back( pagesSetupData("G0r[1]",o_regs,1)); //  Standard molar (partial molal) Gibbs energy of formation G0 of new DC from IC at Tr and Pr (J/mol)_
 //   scalarsList.push_back( pagesSetupData("G0r[2]",o_regs,2)); // Uncertainty of G0,  dev(G0) (J/mol) "
    scalarsList.push_back( pagesSetupData("H0r[0]",o_rehs)); // "dHr - standard enthalpy effect of the reaction at Tr, Pr (J/mol)_
    scalarsList.push_back( pagesSetupData("H0r[1]",o_rehs,1)); //  Standard molar (partial molal) enthalpy of formation H0 of the new DC from IC at Tr and Pr (J/mol)_
 //   scalarsList.push_back( pagesSetupData("H0r[2]",o_rehs,2)); // Uncertainty of H0, dev(H0) (J/mol) "
    scalarsList.push_back( pagesSetupData("S0r[0]",o_ress)); //  "dSr - standard entropy effect of the reaction at Tr, Pr (J/K/mol)_
    scalarsList.push_back( pagesSetupData("S0r[1]",o_ress,1)); //  Absolute molar (partial molal) entropy S0 of  the new DC at standard state (J/K/mol)_
 //   scalarsList.push_back( pagesSetupData("S0r[2]",o_ress,2)); //  Uncertainty of S0, dev(S0) (J/K/mol) "
    scalarsList.push_back( pagesSetupData("Cp0r[0]",o_recps)); //    dCpr - standard isobaric heat capacity effect of the reaction at Tr, Pr (J/K/mol)_
    scalarsList.push_back( pagesSetupData("Cp0r[1]",o_recps,1)); //  Absolute molar (partial molal) isobaric heat capacity Cp0 of  the new DC at standard state (J/K/mol)_
 //   scalarsList.push_back( pagesSetupData("Cp0r[2]",o_recps,2)); //  Uncertainty of Cp0, dev(Cp0) (J/K/mol) "
    scalarsList.push_back( pagesSetupData("V0r[0]",o_remvs)); //   "Molar volume effect dVr of the reaction at Tr,Pr (J/bar)_
    scalarsList.push_back( pagesSetupData("V0r[1]",o_remvs,1)); //  Molar volume V0 of Dependent Component at standard state (J/bar)_
 //   scalarsList.push_back( pagesSetupData("V0r[2]",o_remvs,2)); //  Uncertainty of V0 dev(V0) (J/bar) "
 //   scalarsList.push_back( pagesSetupData("NisoX[0]",o_renix)); // "Mole fraction of counter-isotope in natural mixture (of chemical element)_
 //   scalarsList.push_back( pagesSetupData("PrTr_[0]",o_reptst)); // "Reference pressure Pr (bar)_
 //   scalarsList.push_back( pagesSetupData("PrTr_[1]",o_reptst,1)); // Reference temperature Tr (C) "
 //   scalarsList.push_back( pagesSetupData("BetAl_[0]",o_realbet)); // "Coefficient of isothermal compressibility b (or CN/dentateness for surface species)_
 //   scalarsList.push_back( pagesSetupData("BetAl_[1]",o_realbet,1)); //  Coefficient of isobaric expansibility a (or exponent parameter for surface species) "
    scalarsList.push_back( pagesSetupData("ab_[0]",o_reder)); // "Ion-size parameter ao  (Debye-Hueckel) or CD unit charge at zero plane for surface complexes_
    scalarsList.push_back( pagesSetupData("ab_[1]",o_reder,1)); // Individual parameter b (optional) (or unit charge at the beta plane for surface complexes) "
    // dynamic
   scalarsList.push_back( pagesSetupData("TCint_[0]",o_retcint)); // "Input interval of temperatures of the logK validity range; or vector of T values for logK interpolation "
   scalarsList.push_back( pagesSetupData("P_int[0]",o_repint));   // "Input interval of pressures of the logK validity range; or vector of P values for logK interpolation "
//   scalarsList.push_back( pagesSetupData("adCpT[0]",o_redcp));    // "Empirical coefficients of dCpr = f(T) equation: Free term coefficient a0_
//   scalarsList.push_back( pagesSetupData("adCpT[1]",o_redcp,1)); //  Coefficient a1 of the a1*T term of dCpr = f(T) equation_
//   scalarsList.push_back( pagesSetupData("adCpT[2]",o_redcp,2)); //  Coefficient a2  of the  a2*T^-2 term of dCpr = f(T) equation_
//   scalarsList.push_back( pagesSetupData("adCpT[3]",o_redcp,3)); //  Coefficient a3  of the  a3*T^-0.5 term of dCpr = f(T) equation_
//   scalarsList.push_back( pagesSetupData("adCpT[4]",o_redcp,4)); //  Coefficient a4  of the  a4*T^2 term of dCpr = f(T) equation_
   scalarsList.push_back( pagesSetupData("aiLgKr[0]",o_repkt,0)); // "a0 coefficient of logK = f(T) equation (or 1000*lnK = f(T) for an isotopic compound)_
   scalarsList.push_back( pagesSetupData("aiLgKr[1]",o_repkt,1)); //      a1 coefficient in the a1*T term of logK = f(T) equation (for isotopic species: in the a1/T term)_
   scalarsList.push_back( pagesSetupData("aiLgKr[2]",o_repkt,2)); //      a2 coefficient in the a2/T term of logK = f(T) equation (for isotopic species: in the a2/T^2 term)_
   scalarsList.push_back( pagesSetupData("aiLgKr[3]",o_repkt,3)); //      a3 coefficient in the a3*lnT term of logK = f(T) equation (for isotopic species: in the a3/T^3 term)_
   scalarsList.push_back( pagesSetupData("aiLgKr[4]",o_repkt,4)); //     a4 coefficient in the a4/T^2 term of logK = f(T) equation (for isotopic species: in the a4/T^4 term)_
   scalarsList.push_back( pagesSetupData("aiLgKr[5]",o_repkt,5)); //      a5 coefficient in the a5*T^2 term of logK = f(T) equation_
   scalarsList.push_back( pagesSetupData("aiLgKr[6]",o_repkt,6)); //      a6 coefficient in the a6/T^0.5 term of logK = f(T) equation_
    break;

 case RT_RTPARM:
//   scalarsList.push_back( pagesSetupData("rpPi[0]",o_rppi)); // "Pressure P, bar: start value of the pressure iterator_
//   scalarsList.push_back( pagesSetupData("rpPi[1]",o_rppi,1)); // Pressure P, bar: end value of the iterator_
//   scalarsList.push_back( pagesSetupData("rpPi[2]",o_rppi,2)); // ressure P, bar: step value of the iterator "
//   scalarsList.push_back( pagesSetupData("rpTi[0]",o_rpti)); //   o_rpti    rpTi   F_ 3 1    N "Temperature TC, centigrade: start value of the temperature iterator_
//   scalarsList.push_back( pagesSetupData("rpTi[1]",o_rpti,1)); //    Temperature TC, end value of the iterator_
//   scalarsList.push_back( pagesSetupData("rpTi[2]",o_rpti,2)); //    Temperature TC, step value of the iterator "

   scalarsList.push_back( pagesSetupData("xT",o_rpxt,-1)); //"T vector-column of temperature, usually an abscissa on the plots "
   scalarsList.push_back( pagesSetupData("xP",o_rpxp,-1)); //"P vector-column of pressures, optionally an abscissa on the plots "
//   scalarsList.push_back( pagesSetupData("yF",o_rpyf,-1)); // "RTParm table of sampled data "
//   scalarsList.push_back( pagesSetupData("xTE[0]",o_rpxte)); // "T vector (temperatures), abscissa on the plots for empirical data (optional) "
//   scalarsList.push_back( pagesSetupData("xPE[0]",o_rpxpe)); //"P vector (pressures), abscissa on the plots for empirical data (optional) "
//   scalarsList.push_back( pagesSetupData("ytE[0]",o_rpyte)); //"RTParm input table of empirical data (optional) "

    break;
case -1: //# TPwork DOD
// add Tpwork objects here
// DC molar properties
   scalarsList.push_back( pagesSetupData("twG",o_twg)); //"g0, molar (partial molal) Gibbs energy function of this species at T,P of interest "
   scalarsList.push_back( pagesSetupData("twH",o_twh)); //"h0, molar (partial molal) enthalpy function of this species at T,P of interest "
   scalarsList.push_back( pagesSetupData("twS",o_tws)); //"S0, absolute molar entropy of this species at T,P of interest "
   scalarsList.push_back( pagesSetupData("twCp",o_twcp)); // "Cp0, absolute molar constant-pressure heat capacity of this species at T,P of interest "
   scalarsList.push_back( pagesSetupData("twV",o_twv)); // "V0, molar (partial molal) volume of this species at T,P of interest "
// Reaction effects
   scalarsList.push_back( pagesSetupData("twK",o_twk)); //"K, equilibrium constant of the reaction at T,P of interest "
   scalarsList.push_back( pagesSetupData("tlogK",o_tlogk)); //"logK of reaction at T,P of interest "
   scalarsList.push_back( pagesSetupData("tdGr",o_tdgr)); //"dGr, Gibbs energy change in the reaction at T,P of interest "
   scalarsList.push_back( pagesSetupData("tdHr",o_tdhr)); //"dHr, enthalpy change in the reaction at T,P of interest "
   scalarsList.push_back( pagesSetupData("tdSr",o_tdsr)); //"dSr, entropy effect of reaction at T,P of interest "
   scalarsList.push_back( pagesSetupData("tdCpr",o_tdcpr)); //"dCpr, constant-pressure heat capacity effect of reaction at T,P of interest "
   scalarsList.push_back( pagesSetupData("tdVr",o_tdvr)); // "dVr, volume effect of reaction at T,P of interest "
   scalarsList.push_back( pagesSetupData("twRow",o_twrow)); //"Density of H2O fluid at T,P of interest (g/cm3) "
   scalarsList.push_back( pagesSetupData("twEw",o_twew)); // "Dielectric constant of H2O fluid at T,P of interest "
// current RT, T, P values
   scalarsList.push_back( pagesSetupData("twRT",o_twRT)); //"RT factor  at T of interest"
   scalarsList.push_back( pagesSetupData("twP",o_twpp)); // "P, pressure of interest, bar (10^5 Pa) "
//   scalarsList.push_back( pagesSetupData("twPr",o_twppr)); //"Pr, reference pressure "
   scalarsList.push_back( pagesSetupData("twTC",o_twtc)); // "TC, temperature of interest, C "
//   scalarsList.push_back( pagesSetupData("twTCr",o_twtcr)); // "TCr, reference temperature, C "
   scalarsList.push_back( pagesSetupData("twTK",o_twtk)); //  "T, temperature of interest, K "
//   scalarsList.push_back( pagesSetupData("twTKr",o_twtkr)); //"Tr, reference temperature, K "
// the rest
//   scalarsList.push_back( pagesSetupData("tdevS",o_tdevs)); //"devS, uncertainty of S0 at T,P of interest (reserved) "
//   scalarsList.push_back( pagesSetupData("tdevG",o_tdevg)); //"devG, uncertainty of g0 at T,P of interest (reserved) "
//   scalarsList.push_back( pagesSetupData("tdlogK",o_tdlogK)); // "dev_logK, uncertainty of logK at T,P of interest (reserved) "
//   scalarsList.push_back( pagesSetupData("tdevH",o_tdevh)); // "devH, uncertainty of h0 at T,P of interest (reserved) "
//   scalarsList.push_back( pagesSetupData("tdevCp",o_tdevcp)); // "devCp, uncertainty of Cp0 at T,P of interest (reserved) "
//   scalarsList.push_back( pagesSetupData("tdevV",o_tdevv)); // "devV, uncertainty of V0 at T,P of interest (reserved) "
//   scalarsList.push_back( pagesSetupData("ab_TP[0]",o_twalpbet)); //"Compressibility (reserved)_
//   scalarsList.push_back( pagesSetupData("ab_TP[1]",o_twalpbet,1)); // expandability (reserved) "
//   scalarsList.push_back( pagesSetupData("fugVTP[0]",o_twfugv)); //"molar fugacity at TP (reserved)_
//   scalarsList.push_back( pagesSetupData("fugVTP[1]",o_twfugv,1)); //  dVm of gas (reserved) "
   scalarsList.push_back( pagesSetupData( "tw[0]",o_tww )); // 10  N "Work cells for the tExpr math script "
   break;

  case RT_PHASE:
    scalarsList.push_back( pagesSetupData("DisPar[0]",o_phdisep,0)); //   "A  - phase specific surface area, m2/g_
    scalarsList.push_back( pagesSetupData("DisPar[1]",o_phdisep,1)); //     Sig0, specific surface free energy of phase-aqueous interface, J/m2_
    scalarsList.push_back( pagesSetupData("DisPar[2]",o_phdisep,2)); //     SigG, specific surface free energy of phase-gas interface, J/m2_
    scalarsList.push_back( pagesSetupData("DisPar[3]",o_phdisep,3)); //     Mean radius r0 for (spherical or cylindrical) particles, nm (reserved)_
    scalarsList.push_back( pagesSetupData("DisPar[4]",o_phdisep,4)); //     Mean thickness h0 for cylindrical or 0 for spherical particles, nm (reserved)_
    scalarsList.push_back( pagesSetupData("DisPar[5]",o_phdisep,5)); //     Dielectric constant for solid carrier at Tr (reserved)_
    scalarsList.push_back( pagesSetupData("DisPar[6]",o_phdisep,6)); //     Specific conductivity at Tr, Sm/m/cm2 (reserved)_
    scalarsList.push_back( pagesSetupData("DisPar[7]",o_phdisep,7)); //    Default maximum density for surface species, (12.05) 1/nm2 "
    // dynamic
//o_phxfiec XfIEC  F_  -3 1  N "Density of permanent surface charge, mkeq/m2 (for electrostatic SCMs) "
//o_phmsdt  MsDt   F_  -3 2  N "| Common maximum density of competitive surface complexes, 1/nm2 (for old C SAT only)_
//| SAT FFG parameters (reserved) "
//o_phcapt  Cap_t  F_  -3 2  N "| C1 - inner EDL capacitance density parameter, F/m2 (TLM, 3LM, BSM, CCM)_
//| C2 - outer EDL capacitance density parameter, F/m2 (TLM, 3LM)); anion C for BSM "
//o_phmasdj MaSDj  F_  -3 1  P "| Parameters for surface activity coefficent terms SACT: Surface (site) complete coverage density (1/nm2)_
//| Formula charge of surface species assigned to 0 EDL plane_
//| Formula charge of surface species assigned to b (1) or d (2) EDL plane (see SAT_C[3] code)_
//| SACT isotherm parameter 1 (interaction parameter aplha in Frumkin SACT, <0 repulsion, >0 attraction)_
//| SACT isotherm parameter 2 (e.g. denticity n in QCA SACT)_
//| Isotherm parameter 3, e.g. b in BET SACT (reserved) "
//o_phfsit  FSI_t  F_  -3 1  N "Fraction of surface type relative to the whole surface of the sorbent "
//o_phpnc   ph_cf  F_  -3 1  N "Array of interaction parameter coefficients for the non-ideal mixing model (optional) "
//o_phscoef dc_cf  F_  -3 1  P "Array of end-member-related coefficients for the non-ideal mixing model (optional)"
    break;

//  case RT_PARAM:
/*
o_papc    Pa_PC  I_  1 1   N "Mode of PhaseSelect() operation  { default 1 }"
o_paprd   Pa_PRD I_  1 3   N "Mode of calculation of activity coefficients { default 2; 1, 3, or 4 possible }_
 set to 3, or negative: number of global GEM refinement loops for highly non-ideal systems (-1 to -50, not recommended)_
 Level of diagnostic messages { 0- disabled (no ipmlog file)); 1- normal; 2-including warnings } "
o_padpwt  Pa_DPV I_  1 3   N "Maximal number of iterations in EnterFeasibleDomain() procedure { 300 }_
 Maximum number of allowed IPM-2 mass balance accuracy improvement loops { 30 }_
 IPM-2 cutoff exponent to recover tiny amounts in the primal x vector using the dual solution { -2 } "
o_papllg  Pa_LLG I_ 1 1    N "Tolerance for checking changes in dual solution on IPM2 refinement loops { default 200; max. 3000 } "
o_pape    Pa_PE  I_ 1 1    N "Include electroneutrality condition? { 0 1 } "
o_paiim   Pa_IIM I_ 1 1    N "Maximum allowed number of iterations in one GEM IPM descent run { 1000 } "
o_padg    Pa_DG  D_ 1 1    N "Threshold for minimum allowed descent step size in EntryFeasibleDomain(){ 1e-5 } "
o_padhb   Pa_DHB D_ 1 1    N "Max. allowed abs. mass balance residual (moles) for ICs { default 1e-12; 1e-7 to 1e-14 } "
o_pads    Pa_DS  D_ 1 1    N "Cutoff min. amount of stable phase in GEM IPM primal solution { default 1e-14; 1e-8 to 1e-16 } "
o_padk    Pa_DK  D_ 1 1    N "Tolerance threshold for the Dikin's criterion of IPM convergence { default 5e-7; 1e-7 to 1e-5 }"
o_padf    Pa_DF  D_ 1 2    N "Tolerance DF for Karpov's criterion (Fa > DF) for a lost stable phase to be inserted { 0.01 }_
 Tolerance for Karpov's criterion (Fa < -DFM) for a present unstable phase to be eliminated { 0.1 } "
o_padfy   Pa_DFY D_ 1 7    N "Insertion mole amount for water-solvent at Simplex()->MBR() bridge { 1e-6 }_
 Insertion mole amount for aqueous species at Simplex()->MBR() bridge { 1e-6 }_
 Insertion mole amount for species of ideal solutions at Simplex()->MBR() bridge { 1e-6 }_
 Insertion mole amount for a major species in a solution at Simplex()->MBR() bridge { 1e-6 }_
 Insertion mole amount for a junior species in a solution at Simplex()->MBR() bridge{ 1e-6 }_
 Insertion mole amount for a single-component phase at Simplex()->MBR() bridge { 1e-6 }_
 Insertion mole amount for a single-component phase in PhaseSelection() { 1e-7 } "
o_padb    Pa_DB  D_ 1 1    N "Cutoff minimum mole amount of Independent Component in the b vector { 1e-17 } "
o_paag    Pa_AG  D_ 1 1    N "Smoothing parameter 1 for non-ideal primal chemical potential increments { default 1.0; -1 to +1 } "
o_padgc   Pa_DGC D_ 1 1    N "Smoothing parameter 2 (exponent in smoothing function for non-ideal potentials { default -0.5 or 0.07; -1 to +1 } "
o_pagan   Pa_GAN D_ 1 3    N "Initial activity coefficient value for major species in a solution phase at LPP AIA  { 1 }_
 Initial activity coefficient value for minor species in a solution phase at LPP AIA { 1000 }_
 IPM-2 mass balance accuracy factor in MassBalanceRefinement()  { 0.001 } "
o_padns   Pa_DNS D_  1 1   N "Default reference density of surface species at standard state { 12.05 nm-2 } "
o_paxmin  Pa_XMI D_  1 5   N "Cutoff mole amount of water-solvent for aqueous phase elimination{ 1e-5 }_
 Cutoff mole amount of solid sorbent for sorption phase elimination { 1e-7 }_
 Cutoff mole amount for elimination of solute (sorbate) species { 1e-24 }_
 Cutoff mole amount for elimination of solution phases other than aqueous { default 1e-14, usually set the same as as Pa_DS }_
 Cutoff value of effective molal ionic strength to disable aq-gamma calculation { default 3e-5; 1e-6 to 1e-3 } "
o_paeps   Pa_EPS D_  1 2   N "Tolerance of the SolveSimplex() convergence { default 1e-10; 1e-6 to 1e-11 }_
 Tolerance for calculation of surface activity coefficient terms for surface species { default 1e-3; 1e-6 to 1e-2 } "
o_padkin  Pa_DKI D_  1 1   N "Tolerance for non-trivial metastability restrictions on amounts of dependent components, moles { 1e-7 } "
# Default numeric parameters
o_papmnum  pPMnum  F_ 1 3  N "Cutoff moles of DC (Ls set) { 1e-19 };
  log(1e-16));
  log(1e-9)"
   */
  //  break;

  case RT_PROCES:
//# Important objects for defining Process Simulators (iterators)
scalarsList.push_back( pagesSetupData("iTm[0]",o_petmi,0)); // "SysEq CSD variant number: start value_
scalarsList.push_back( pagesSetupData("iTm[1]",o_petmi,1)); // SysEq CSD variant number: end value_
scalarsList.push_back( pagesSetupData("iTm[2]",o_petmi,2)); // SysEq CSD variant number: step value "
scalarsList.push_back( pagesSetupData("cTm",o_pec_tm));     //"Current SysEq CSD variant number "
scalarsList.push_back( pagesSetupData("iV[0]",o_pevi,0)); // "Volume constraint of the system (L): start value_
scalarsList.push_back( pagesSetupData("iV[1]",o_pevi,1)); // Volume constraint of the  system:  end value_
scalarsList.push_back( pagesSetupData("iV[2]",o_pevi,2)); // Volume constraint of the system: increment "
scalarsList.push_back( pagesSetupData("cV",o_pec_v)); //"Current V - volume constraint (default: 0) "
scalarsList.push_back( pagesSetupData("iP[0]",o_pepi,0)); // "Pressure of interest P, bar: start (real) value_
scalarsList.push_back( pagesSetupData("iP[1]",o_pepi,1)); // Pressure of interest P, bar: end value_
scalarsList.push_back( pagesSetupData("iP[2]",o_pepi,2)); // Pressure of interest P, bar: step value "
scalarsList.push_back( pagesSetupData("cP",o_pec_p)); //"Current pressure of interest P (bar) "
scalarsList.push_back( pagesSetupData("iTC[0]",o_peti,0)); //"Temperature of interest TC, centigrade: start value_
scalarsList.push_back( pagesSetupData("iTC[1]",o_peti,1)); // Temperature of interest TC, centigrade: end value_
scalarsList.push_back( pagesSetupData("iTC[2]",o_peti,2)); // Temperature of interest TC, centigrade: step value "
scalarsList.push_back( pagesSetupData("cTC",o_pec_tc)); // "Current temperature of interest TC (centigrade) "
scalarsList.push_back( pagesSetupData("cT",o_pec_t)); //"Current temperature of interest T (Kelvin) "
scalarsList.push_back( pagesSetupData("iNv[0]",o_penvi,0)); //"TD variant number: start value_
scalarsList.push_back( pagesSetupData("iNv[1]",o_penvi,1)); // TD variant number: end value_
scalarsList.push_back( pagesSetupData("iNv[2]",o_penvi,2)); // TD variant number: step value "
scalarsList.push_back( pagesSetupData("cNV",o_pec_nv)); //"Current SysEq TD variant number "
scalarsList.push_back( pagesSetupData("iTau[0]",o_petaui,0)); //"Real time (tau), arbitrary units: start value_
scalarsList.push_back( pagesSetupData("iTau[1]",o_petaui,1)); // Real time (tau): end value_
scalarsList.push_back( pagesSetupData("iTau[2]",o_petaui,2)); // Real time (tau): step value "
scalarsList.push_back( pagesSetupData("cTau",o_pec_tau)); //"Current real time (tau) "
scalarsList.push_back( pagesSetupData("ipXi[0]",o_pepxii,0)); //N "Process extent parameter pXi = -log(Xi): start value_
scalarsList.push_back( pagesSetupData("ipXi[1]",o_pepxii,1)); // Process extent parameter pXi: end value (precision for inverse titrations)_
scalarsList.push_back( pagesSetupData("ipXi[2]",o_pepxii,2)); // Process extent parameter pXi: step value "
scalarsList.push_back( pagesSetupData("cpXi",o_pec_pxi)); //"Current pXi = -log(Xi), where Xi stands for a logarithmic reaction extent variable "
scalarsList.push_back( pagesSetupData("cXi",o_pec_xi)); //"Current Xi = 10^cpXi - the linear process extent variable "
scalarsList.push_back( pagesSetupData("iNu[0]",o_penui,0)); //"Factor of the process extent Nu: start value_
scalarsList.push_back( pagesSetupData("iNu[1]",o_penui,1)); // Factor of the process extent Nu: end value_
scalarsList.push_back( pagesSetupData("iNu[2]",o_penui,2)); // Factor of the process extent Nu: step value "
scalarsList.push_back( pagesSetupData("cNu",o_pec_nu)); //"Current Nu - the process extent factor "
scalarsList.push_back( pagesSetupData("ipH[0]",o_pephi,0)); //"Prescribed pH sequence (for inverse titrations): start value_
scalarsList.push_back( pagesSetupData("ipH[1]",o_pephi,1)); // Prescribed pH sequence: end value_
scalarsList.push_back( pagesSetupData("ipH[2]",o_pephi,2)); // Prescribed pH sequence: step size "
scalarsList.push_back( pagesSetupData("cpH",o_pec_ph)); //"Current pH at J-th inverse titration point "
scalarsList.push_back( pagesSetupData("ipe[0]",o_pepei,0)); //o_pepei   ipe    D_ 3 1    N "Prescribed pe values for inverse titration sequence: start value_
scalarsList.push_back( pagesSetupData("ipe[1]",o_pepei,1)); // Prescribed pe values: end value_
scalarsList.push_back( pagesSetupData("ipe[2]",o_pepei,2)); // Prescribed pe sequence: step size "
scalarsList.push_back( pagesSetupData("cpe",o_pec_pe)); //"Current pe at J-th inverse titration point "
scalarsList.push_back( pagesSetupData("cEh",o_pec_eh)); //"Current Eh value (aqueous systems only) "
//o_pencnt  NeIt   I_ 1 2    N "Max. number of iterations allowed (>= Ntxi[1]; for inverse titrations, > Ntxi[1]*15)_
// Number of completed main steps in the process simulation "
scalarsList.push_back( pagesSetupData("Next",o_penext)); //"A flag to continue (1) or finish (0) main process simulation steps "
scalarsList.push_back( pagesSetupData("I",o_pei)); //"Internal index in an inverse titration sequence step "
scalarsList.push_back( pagesSetupData("J",o_pej)); //"Process simulation main step index "
scalarsList.push_back( pagesSetupData("Jp",o_pec_nrk)); //"Current index of the processed SysEq in the record key list "

//# Optional storage vectors for the process simulator
scalarsList.push_back( pagesSetupData("vTm",o_petmv,-1)); //"Optional vector to collect VarCSD ( cTm ) numbers from main process simulaion steps "
scalarsList.push_back( pagesSetupData("vNV",o_penvv,-1)); //"Optional vector of VarTD (cNV) numbers from main process simulation steps "
scalarsList.push_back( pagesSetupData("vP",o_pepv,-1)); //"Optional vector of pressure values (cP) from main process simulation steps "
scalarsList.push_back( pagesSetupData("vV",o_pevv,-1)); //"Optional vector of volume constraint (cV) values from main process simulation steps "
scalarsList.push_back( pagesSetupData("vT",o_petv,-1)); //"Optional vector of temperatures (cT) from main process simulation steps "
scalarsList.push_back( pagesSetupData("vTau",o_petauv,-1)); //"Optional vector of Tau (physical time, cTau) values from main process simulation steps "
scalarsList.push_back( pagesSetupData("vpXi",o_pepxiv,-1)); //"Optional vector of pXi (-log process extent cpXi) values from main process simulation steps "
scalarsList.push_back( pagesSetupData("vNu",o_penuv,-1)); //"Optional vector of Nu (process extent factor cNu) values from main process simulation steps "
scalarsList.push_back( pagesSetupData("vKin[0]",o_pekinv)); //"Optional array of parameters for (reaction) kinetic dependencies "
scalarsList.push_back( pagesSetupData("modC",o_pemodc,-1)); //"Array of the process simulation parameters "
scalarsList.push_back( pagesSetupData("xp",o_pcx0,-1)); //"Vector-column of abscissa x "
scalarsList.push_back( pagesSetupData("yp",o_pcy0,-1)); //"Data table of ordinate(s) y "
scalarsList.push_back( pagesSetupData("xEp[0]",o_pcxe)); //"Vector-column of abscissa for empirical data xE "
scalarsList.push_back( pagesSetupData("yEp[0]",o_pcye)); //"Vector-column (or table) of ordinate(s) for empirical data yE "
    break;

  case RT_GTDEMO:
//# GTDEMO work DOD
scalarsList.push_back( pagesSetupData("jR",o_gwjr)); //"Index of currently sampled database record "
scalarsList.push_back( pagesSetupData("next",o_gwnext)); //"A flag to continue (1) or finish (0) the data sampling steps "
//o_gwi0    i0     I_ 1 1    N "Internal column index for the use in gExpr math scripts "
//o_gwie    iE     I_ 1 1    N "Internal column index for the use in gExprE math scripts "
//o_gwj0    j0     I_ 1 1    N "Internal row index for the use in gExpr math scripts "
//o_gwje    jE     I_ 1 1    N "Internal row index for the use in gExprE math scripts "
// dynamic
scalarsList.push_back( pagesSetupData("xE[0]",o_gdxe,0)); //"Vector-column of abscissa for empirical data "
scalarsList.push_back( pagesSetupData("yE[0]",o_gdye,0)); //"Vector-column (or table) of ordinate(s) for empirical data "
//o_gdwc    gd_wc  D_  -3 3  N "Vector-row of work cells for GtDemo math scripts (optional) "
//o_gdqp    gQp    D_  -3 3  N "Table of work cells - a row per sampled record (optional) "
    break;

//  case RT_UNSPACE:
/*
o_un_t   tu  I_ 1 1  N "< Q  current index t of payoff matrix row (calculated sampled point) (<Q)"
o_un_q   qu  I_ 1 1  N "current index q of payoff matrix column (input data variant) (<Q)"
o_un_g   gu  I_ 1 1  N "current index g of unspace group (< nG)"
o_un_i   iu  I_ 1 1  N "current index i of independent component (<N)"
o_un_j   ju  I_ 1 1  N "current index j of dependent component  (<L)"
o_un_k   ku  I_ 1 1  N "current index k of phase ( 0 to Fi-1)  (<Fi)"
o_un_ka  kau I_ 1 1  N "current index ka of phase assemblage  (<nPhA)"

o_un_tc  Tu F_ 5 1  N "Adapted value of T (Celsius)_
   Initial (deterministic) value of T (Celsius)_
   0 or adapted uncertainty half-interval for T (C)_
   0 or initial uncertainty half-interval for T (C)_
   Current value of T"
o_un_p   Pu F_ 5 1  N " Adapted value of P (bar)_
   Initial (deterministic) value of P (bar)_
   0 or adapted uncertainty half-interval for P (bar)_
   0 or initial uncertainty half-interval for P (bar)_
   Current value of P"
o_un_v   Vu F_ 5 1 N "Adapted value of constrained system volume V (in cm3)_
   Initial (deterministic) value of system volume V (in cm3)_
   0 or adapted uncertainty half-interval for system volume V (in cm3)_
   0 or initial uncertainty half-interval for system volume V (in cm3)_
   Current value of constrained system volume V"
o_un_ph   pHf F_ 2 1  N "pH lower filter limit (default 0)_
   pH upper filter limit (default 14)"
o_un_eh   Ehf F_ 2 1  N "Eh lower filter limit (default -1)_
  Eh upper filter limit (default 1)"
o_un_ic   ICf F_ 2 1 N "Ionic strength lower limit (default 0)_
  Ionic strength lower limit (default 3)"
o_unquan   qLev F_ 1 1  N "quantile level (default 0.05)"
o_un_cr   un_CrV D_ 4 1  N "Laplace single criterion value (from analysing payoff matrix)_
  Hurtvitz single criterion value (from analysing payoff matrix)_
  Wald single criterion value (from analysing payoff matrix)_
  Homenuk  single criterion value (from analysing payoff matrix)"

// dynamic
o_unpbd  unGCv    I_  -3 1  N "[nG] generation codes for unspace groups: 0- Belov; 1-uniform; 2-normal"
o_unnglg unGvG   I_  -3 1  J "[L]  0-deterministic DC; value >0 is index of a group of varying G to which this DC is assigned"
o_unnggam  unGvP I_  -3 1  J "[Ls] 0-deterministic DC; value >0 is index of a group varying gamma params (reserved)"
o_unngls  unGvS  I_  -3 1  J "[L]  0-deterministic DC; value >0 is index of a group varying S"
o_unnglv  unGvV  I_  -3 1  J "[L]  0-deterministic DC; value >0 is index of a group varying V"
o_unngnb  unGvB  I_  -3 1  I "[N]  0-deterministic DC; value >0 is index of a group varying b to which this IC is assigned"
o_unf_pha  PhAf  I_  -3 1  N  "[N]  input phase indexes of known equilibrium phase association for a primary filter (Pa_f_pha)"
# input uncertainty half-intervals; 0 means deterministic DC or IC
o_unintlgo  intG0  F_  -3 2  J "| adapted uncertainty half-intervals for G0 variation (flag PsUnInt defines units)_
| initial  uncertainty half-intervals for G0 variation (flag PsUnInt defines units)"
o_unintgam  intGam  F_  -3 2  J "| adapted uncertainty half-intervals for gamma params (reserved)_
| initial uncertainty half-intervals for gamma params (reserved)"
o_unintls   intS   F_  -3 2  J "| adapted uncertainty half-intervals for S (reserved)_
| initial uncertainty half-intervals for S (reserved)"
o_unintlv   intV   F_  -3 2  J "| adapted uncertainty half-intervals for V0 variation (flag PsUnInt defines units)_
| initial uncertainty half-intervals for V variation (flag PsUnInt defines units)"
o_unintnb   intB   F_  -3 2  I "| adapted uncertainty half-intervals for b variation (flag PsUnInB defines units)_
| initial uncertainty half-intervals for b variation (flag PsUnInB defines units)"
# input filters for sampled solution variants
o_unm_t_lo m_t_lo    F_  -3 1  I "total IC molality filter - lower limits (default 0/-20) (units defined in PsUnFltI flag)"
o_unm_t_up m_t_up    F_  -3 1  I "total IC molality filter - upper limits (default 10/+1) (units defined in PsUnFltI flag)"
o_unfug_lo fug_lo    F_  -3 1  J "fugacity/activity filter - lower limits (default 0/-20) (units defined in PsUnFltD flag)"
o_unfug_up fug_up    F_  -3 1  J "fugacity/activity filter - upper limits (default 10/+1) (units defined in PsUnFltD flag)"
# Input II (will be done automatically)
o_ungs un_Gs   F_  -3 2 J "| adapted values of G298 for DCs_
| input initial (deterministic) values of G298 for DCs"
o_unss un_Ss   F_  -3 2 J "| adapted values of S298 for DCs (reserved)_
| input initial (deterministic) values of S298 for DCs (reserved)"
o_unvs un_Vs   F_  -3 2 J "| adapted values of V298 for DCs_
| input initial (deterministic) values of V298 for DCs"
o_ungams unGAMs F_ -3 2 J "| adapted Gamma parameters (reserved)_
| initial Gamma parameters (reserved)"
o_unncp uncp  F_  -3 2  N "[Q][nG] table of normalised coordinates of points in uncertainty space (>= 0 <= 1)"
o_unbs un_Bs D_  -3 2  I "| adapted bulk composition amounts (vector b)_
| input initial (deterministic) bulk composition amounts (vector b)"
# collected GEM sample calculation output for creation of the payoff matrix
o_unvg   uGc   D_  -3 1  J "G0 values used in sample input data variants (indexes q j)"
o_unbc   uBc   D_  -3 1  J "B values used in sample input data variants (indexes q j)"
o_unsc   uSc   D_  -3 1  J "S values used in sample input data variants (indexes q j)"
o_unmvc  umVc  D_  -3 1  J "mV values used in sample input data variants (indexes q j)"
o_unnidp uNidP D_  -3 1  J "Gamma parameter values used in sample input data variants (indexes q j)"
o_unvy   uYc   D_  -3 1  J "x (DC mole amounts) from sample GEM solution variants (indexes t j)"
o_unvyf  uYFc  D_  -3 1  A "XF (phase mole amounts) from sample GEM solution variants (indexes t k)"
o_unvgam uGamc D_  -3 1  J "lnGam values for DC from sample GEM solution variants (indexes t j)"
o_unvmol um_tc D_  -3 1  I "m_t values of total IC molality from sample GEM solution variants (indexes t i)"
o_unvu   uUc   D_  -3 1  I "u values of dual chemical potentials from sample GEM solution variants (indexes t i)"
o_unvfug uFugc D_  -3 1  J "lga values of log10 fugacity/activity from sample GEM solution variants (indexes t j)"
# Math script preprocess indexes Q T G P must be added
o_unvt uTc   F_  -3 1  N "TC vector of temperatures (C) from sample input data variants (index q)"
o_unvp uPc   F_  -3 1  N "P vector of pressures (bar) from sample input data variants (index q)"
o_unvv uVc   F_  -3 1  N "V vector of system volumes (in cm3) (index q)"
o_unvph upHc F_  -3 1  N "vector of pH  values from sample GEM solution variants (index t)_
| vector of Eh  values from sample GEM solution variants (index t)_
| vector of IC  values from sample GEM solution variants (index t)"
o_unovb OVB   F_  -3 1  N "pseudo-random numbers for Belov algorithm"
o_unovr OVR   F_  -3 1  N "pseudo-random numbers by Monte Carlo with uniform distribution"
o_unovn OVN   F_  -3 1  N "pseudo-random numbers by Monte Carlo with normal distribution"
o_unqucx quOV I_  -3 4  N "| indices of sample GEM variants taken into quantile Laplace criterion_
| indices of sample GEM variants taken into quantile Hurtvitz criterion_
| indices of sample GEM variants taken into quantile Wald criterion_
| indices of sample GEM variants taken into quantile Homenyuk criterion"
o_unqucv  quCrV F_  -3 4  N "| Values of the quantile Laplace criterion_
| Values of the quantile Hurtvitz criterion_
| Values of the quantile Wald criterion_
| Values of the quantile Homenyuk criterion"

# Work arrays
o_unphndx PhAndx I_  -3 1  N "Table of system indices of phases to identify Phase Assemblages"
o_unsv    uPhsv   I_  -3 1  N "Indices of Phase Assemblage ids in sampled variants (PhAndx)"
o_unphnum PAnum  I_  -3 1  N "vector of number of sample GEM solution variants containing a phase association"
o_unafreg PAfreg F_  -3 1  N "frequency of sample GEM solution variants containing a phase association (PhNum*100/Q)"
o_unpmr pmr F_  -3 1  N "pointer to the beginning  of current line in payoff matrix ( PvROM/PvPOR )"
o_unpom POM F_  -3 1  N "payoff matrix     optional (see PvPOM flag)"
o_unpor POR F_  -3 1  N "payoff matrix row    optional (see PvPOR flag)"
o_una unA  D_  -3 1  N "work stoichiometry matrix in project/system indexation"
#
o_unzcp     Zcp    D_  -3 1  N "mean over rows of pay-off matrix for Laplace criterion"
o_unzmin    Zmin   D_  -3 1  N "minima in rows of pay-off matrix for Hurwitz criterion"
o_unzmax    Zmax   D_  -3 1  N "maxima in rows row pay-off matrix  for Hurwitz criterion"
o_unzmaxabs ZmaxAb D_  -3 1  N "abs(max)values in rows of pay-off matrix for Wald criterion"
o_unhom     Hom    D_  -3 1  N "array of calculated values for Homenyuk criterion"
o_unprob    Prob   D_  -3 1  N "array of probabilities for calculating Homenuk criterion"
#array for statistics output
o_ununic UnIC D_  -3 1  I "| Total dissolved IC molalities m_i for the deterministic variant_
| m_i averages for all sample variants that have passed through filters_
| 2-sigma deviations for the averages in (previous) column 1_
| m_i averages for sample variants with dominant phase assemblage_
| 2-sigma deviations for the averages in (previous) column 3_
| m_i averages over sample variants selected by quantile Laplace criterion_
| m_i averages over sample variants selected by quantile Hurvitz criterion_
| m_i averages over sample variants selected by quantile Wald criterion_
| m_i averages over sample variants selected by quantile Homeniuk criterion_
| reserved column "
o_unugdc UgDC D_  -3 10  J "| Uncertain input parameter (UIP) values for the deterministic variant_
| Average UIP values over all sample variants that have passed through filter(s)_
| 2-sigma deviations for the averages in (previous) column 1_
| Average UIP values over all sample variants with the dominant phase assemblage_
| 2-sigma deviations for the averages in (previous) column 3_
| Average UIP values over sample variants selected by quantile Laplace criterion_
| Average UIP values over sample variants selected by quantile Hurvitz criterion_
| Average UIP values over sample variants selected by quantile Wald criterion_
| Average UIP values over sample variants selected by quantile Homeniuk criterion_
| 2-sigma deviations for the averages related to the criterion used in adaptive loops (see unPafl[8]) "
o_unuadc UaDC D_  -3 1  J "| DC activities (fugacities for gases) for the deterministic variant_
| average DC activities for all sample variants that have passed through filters_
| 2-sigma deviations for the averages in (previous) column 1_
|  average DC activities for sample variants with dominant phase assemblage_
| 2-sigma deviations for the averages in (previous) column 3_
| average DC activities over sample variants selected by quantile Laplace criterion_
| average DC activities over sample variants selected by quantile Hurvitz criterion_
| average DC activities over sample variants selected by quantile Wald criterion_
| average DC activities over sample variants selected by quantile Homeniuk criterion_
| reserved column "
o_unundca UnDCA D_  -3 1  J "| Adaptively optimized UIP values_
| Adaptively optimized uncertainty half-intervals for the optimized UIP values_
| Minimum observed UIP values after adaptive loops_
| Maximum observed UIP values after adaptive loops_
| Average for UIP values over sample variants that have passed through filters at the last adaptive loop_
| Uncertainty half-intervals (2-sigma) for the averages in (previous) column 5_
| reserved_
| reserved "
 */
//    break;

//  case RT_DUALTH:
/*
o_dttmd   dtTm    I_ 3 1    N "SysEq CSD variant number: start value_
 SysEq CSD variant number: end value_
 SysEq CSD variant number: step value "
o_dtnvd   dtNv    I_ 3 1    N "Restrictions variant #: start value_
 Restrictions variant #: end value_
 Restrictions variant #: step value "
o_dtpd    Pd     F_ 3 1    N "Pressure P, bar: start (real) value_
 Pressure P, bar: end value_
 Pressure P, bar: step value "
o_dttd    Td     F_ 3 1    N "Temperature T, C: start value_
 Temperature T, C: end value_
 Temperature T, C: step value "
o_dtvd    Vd     F_ 3 1    N "Volume of the system (L): start value_
 Volume of the system (L):  end value_
 Volume of the system (L): increment "
o_dtmsysb  dtComp F_ 1 9  N "Masses (kg) and volumes (L) for compositions: Ms (total mass, normalization)_
 Vs (total volume of the object, for volume concentrations)_
 M(H2O) (mass of water-solvent for molalities)_
 Maq (mass of aqueous solution for ppm etc.)_
 Vaq (volume of aqueous solution for molarities)_
 Pg (pressure in gas, for partial pressures)_
 MOL total mole amount for basis sub-system composition calculations_
 Mole fraction of the carrier DC (e.g. sorbent or H2O solvent)_
 Asur (specific surface area of the sorbent), for surface species "
o_dtt  dtTc F_ 1 1  N "Current state parameter T, centigrade"
o_dtp  dtPc F_ 1 1  N "Current state parameter P, bar"
o_dtv  dtVc F_ 1 1  N "Current state parameter V, litres"

#work data
o_dtq  qt I_ 1 1  N "Current index of experiment"
o_dkp  kp I_ 1 1  N "index of the interaction parameter"
o_dti  it I_ 1 1  N "Current index of IC"
o_dtjm jm I_ 1 1  N "Current index of DC in non-basis sub-system"
o_dtc_tm ctm I_ 1 1  N "Current Tm - SYSTEM CSD number"
o_dtc_nv cNV_ I_ 1 1  N "Current Nv - MTPARM variant number"
o_lxi2 Xi2 D_ 2 1  N "Final SOS/DF value (quality of LS fit)_
  reserved "

#dynamic
o_dtan    An   F_  -3 1  I "Stoichiometry matrix for DC (end-member) stoichiometry candidates"
o_dtbb    Bb  D_  -3 1  I "Table of bulk compositions of basis sub-systems, moles"
o_dtbn    Bn  D_  -3 1  I "Table of bulk compositions of non-basis sub-systems, moles"
o_dtub    Ub  D_  -3 1  I "Table of dual solution values for basis sub-systems, J/mol"
o_dtchi   chi D_  -3 1  N "Table of mole fractions of DC (end-member) candidates"
o_dteta_b eta_b D_  -3 1  N "Table of DualTh chemical potentials of n(M) end-members, normalized"
o_dtact_n act_n D_  -3 1  N "Table of DualTh-calculated activities"
o_dtgam_n gam_n D_  -3 1  N "Table of activity coefficients for non-basis DC (end-members)"
o_dtcoul  Coul  D_  -3 1  N "Table of input Coulombic terms for surface complexes (J/mol)"
o_dtmu_n  mu_b  D_  -3 1  N "Table of DualTh chemical potentials of non-basis DC (end-members), J/mol"
o_dtmu_o  mu_o  D_  -3 1  N "Table of standard chemical potential (Gibbs energies) for non-basis end members, J/mol"
o_dtavsd_o avsd_o D_  -3 1  N "[2][M] Mean and st.dev. over mu_o columns (experiments) for non-basis DC (end-members)"
o_dtmu_a  mu_a D_  -3 1  N "[Q][M] Table of apparent standard Gibbs energy function G* for minor/trace non-basis end members"
o_dtavsd_a avsd_a D_  -3 1  N "[2][M] mean and st.dev. over mu_a cols (experiments) for DC candidates"
o_gmx_n   gmx_n D_ -3 1 N "| in J/mol for Q experiments : Excess Gibbs energy of mixing (col. 0)_
| Gibbs energy of ideal mixing (col. 1)_
| Total Gibbs energy of mixing (col. 2)_
| Gibbs energy of the mechanical mixture of pure end members (col. 3)_
| Integral Gibbs energy of the mixture phase (col. 4)_"
o_dtwa   Wa   D_ -3 1 N "[Q][P] interaction parameters for the mixing model estimated directly for each experiment"
o_dtavsd_w   avsd_w   D_ -3 1 N "[2][P] estimated interaction parameters with st.deviations, averaged from Wa or regressed from gmx_n"
o_dtyconst yconst   D_  -3 1  N "Constants used in observation prediction function in least-squares (LS) fitter (optional, default 1)"
o_dtqpg   qpg   D_  -3 1  N "Work array for gamma calculation math script"
o_dtcib   CIb   F_  -3 1  I "Input table of quantity/concentration of IC in basis sub-systems"
o_dtcin   CIn   F_  -3 1  I "Input table of quantity/concentration of IC in non-basis sub-systems"
o_dtcab   CAb   F_  -3 1  N "Input table of quantity/concentration of formulae for basis sub-systems"
o_dtcan   CAn   F_  -3 1  N "Input table of quantity/concentration of DC formulae for non-basis sub-systems"
o_dttdq Tdq F_  -3 1  N "Temperatures of experiments (optional input), centigrade"
o_dtpdq Pdq F_  -3 1  N "Pressures of experiments (optional input), bar"
o_dtisq ISq F_  -3 1  N "Effective ionic strength in aqueous solutions (auto-collected)"
o_dtwacp wa_cp  I_ -3 1  N "Input code for constrained LS interaction parameter fitting (LM method only): 0 - no constraints; 1 - upper; 2 - lower; 3 - two-side "
o_dtwdat  wdat  D_  -3 1  N "Input for least-squares (LS) fitter: array of weight factors for data, (default 1)"
o_dtwpar  wpar  D_  -3 1  N "Input for least-squares (LS) fitter: uncertainty half-interval for a priori constrained parameters (optional)"
o_dtwa_ap   Wa_ap D_  -3 1  N "Input: a priori interaction parameter values (calculation of activity coefficients, LS fitting)"

  */
    break;

  case RT_GEM2MT:

 //# GEMS2MT work DOD
 //scalarsList.push_back( pagesSetupData("_ctm",o_mtctm)); //current CSD #"
 //o_mtcnv    cnv   I_ 1 1    N "current restriction variant #"
 scalarsList.push_back( pagesSetupData("qc",o_mtqc)); // current index of the compartment ( 1 to nC )"
 //o_mtkv    kv     I_ 1 1    N "current index of the initial system variant (1 to nIV )"
 //o_mtjqc   jqc    I_ 1 1    N "script c-style index (= qc-1) for transport"
 //o_mtjqs   jqs    I_ 1 1    N "script c-style index (= qc-1) for graphics"
 scalarsList.push_back( pagesSetupData("jt",o_mtjt));//index of sampled point (for sampling scripts)"
 //o_mtjdd    jdd   I_ 1 1    N " current index of diffusing DC"
 //o_mtjdi    jdi   I_ 1 1    N " current index of diffusing IC"
 //o_mtide    ide   I_ 1 1    N " current index of diffusing electrolyte"
 scalarsList.push_back( pagesSetupData("ct",o_mtct_));// actual time iterator"
 //o_mtrei5    rei5   I_ 1 1    N " #"
 //o_mtct    cTt  D_ 1 1    N "current value of T"
 //o_mtcp    cPt  D_ 1 1    N "current value of P"
 //o_mtcv    cVt  D_ 1 1    N "current value of V"
 scalarsList.push_back( pagesSetupData("cTaut",o_mtctau));// current physical time"
 scalarsList.push_back( pagesSetupData("dTau",o_mtdtau)); // current time step value"
 //o_mtotau  oTau  D_ 1 1    N "old time step value"
 //o_mtdx    mtdx   D_ 1 1    N "node distance [L/nC]"
 //o_mtref2    ref2   D_ 1 1    N " #"
 //o_mtref3    ref3   D_ 1 1    N " #"
 //o_mtref4    ref4   D_ 1 1    N " #"
// o_mtpai    mtPai  F_ 4 1    N "Pressure P, bar: start (real) for MTP array in DataCH_
// o_mttai    mtTai  F_ 4 1    N "Temperature T, C: start (real) for MTP array in DataCH_
//scalarsList.push_back( pagesSetupData("mInput[0]",o_mtdoudl,0));//   Masses (kg) and volumes (L) for initial systems: Ms (total mass, normalize)_
//scalarsList.push_back( pagesSetupData("mInput[1]",o_mtdoudl,1));//   Vs (total volume of the object, for volume concentrations)_
//scalarsList.push_back( pagesSetupData("mInput[2]",o_mtdoudl,2));//   M(H2O) (mass of water-solvent for molalities)_
//scalarsList.push_back( pagesSetupData("mInput[3]",o_mtdoudl,3));//   Maq (mass of aqueous solution for ppm etc.)_
//scalarsList.push_back( pagesSetupData("mInput[4]",o_mtdoudl,4));//   Vaq (volume of aqueous solution for molarities)_
//scalarsList.push_back( pagesSetupData("mInput[5]",o_mtdoudl,5));//   Pg (pressure in gas, for partial pressures)_
//scalarsList.push_back( pagesSetupData("mInput[6]",o_mtdoudl,6));//   MOL total mole amount for basis sub-system composition calculations_
//scalarsList.push_back( pagesSetupData("mInput[7]",o_mtdoudl,7));//   mole fraction of the carrier DC (e.g. sorbent or solvent)_
//scalarsList.push_back( pagesSetupData("mInput[8]",o_mtdoudl,8));//   Specific surface area of the sorbent (for adsorbed species)"
//scalarsList.push_back( pagesSetupData("ADpar[0]",o_mtadpar,0));// o_mtadpar ADpar D_  1 11  N "Advection/diffusion mass transport:fluid advection velocity (m/sec)_
//scalarsList.push_back( pagesSetupData("ADpar[1]",o_mtadpar,1));//   column length (m)_
//scalarsList.push_back( pagesSetupData("ADpar[2]",o_mtadpar,2));//  time step reduction factor_
//scalarsList.push_back( pagesSetupData("ADpar[3]",o_mtadpar,3));//   cutoff factor for differences_
//scalarsList.push_back( pagesSetupData("ADpar[4]",o_mtadpar,4));//  cutoff factor for minimal amounts of IC in node bulk compositions_
//scalarsList.push_back( pagesSetupData("ADpar[5]",o_mtadpar,5));//   ADrs1,  reserved_
//scalarsList.push_back( pagesSetupData("ADpar[6]",o_mtadpar,6));//   ADrs2,  reserved_
//scalarsList.push_back( pagesSetupData("ADpar[7]",o_mtadpar,7));//   ADrs3,  reserved_
//scalarsList.push_back( pagesSetupData("ADpar[8]",o_mtadpar,8));//   ADrs4,  reserved_
//scalarsList.push_back( pagesSetupData("ADpar[9]",o_mtadpar,9));//   ADrs5,  reserved_
//scalarsList.push_back( pagesSetupData("ADpar[10]",o_mtadpar,10));//   ADrs6   reserved"
scalarsList.push_back( pagesSetupData("qpc[0]",o_mtqpc)); // Work array for mass transport math script"
scalarsList.push_back( pagesSetupData("HydP[qc][0]", o_mthydp,0));// | Initial hydraulic parameters in nodes: Vt - total volume of node = dx*dy*dz, m**3_
scalarsList.push_back( pagesSetupData("HydP[qc][1]", o_mthydp,1));//| vp - advection velocity (in pores) in this node (m/sec)_
scalarsList.push_back( pagesSetupData("HydP[qc][2]", o_mthydp,2));//| eps - effective (actual) porosity (0 < eps < 1)_
scalarsList.push_back( pagesSetupData("HydP[qc][3]", o_mthydp,3));//| Km - effective (actual) actual permeability, m**2_
scalarsList.push_back( pagesSetupData("HydP[qc][4]", o_mthydp,4));//| al - specific longitudinal dispersivity of porous medium, m_
scalarsList.push_back( pagesSetupData("HydP[qc][5]", o_mthydp,5));//| Dif - general diffusivity of disolved matter in the mode_
scalarsList.push_back( pagesSetupData("HydP[qc][6]", o_mthydp,6));//| nto - tortuosity factor"
//o_mtmb    MB  D_  -3 1  N "  [nC]  column of current masses of boxes or reservoirs (in kg) "
//o_mtdmb   dMB  D_  -3 1  I " [nC][Nb]  Table of current derivatives dM for elements in reservoirs "
//o_mtqpi    qpi   D_  -3 1  N " [Nqpi] Work array for initial systems math script"
//# work objects in DataCH structure (for use in GEM2MT scripts)
//o_ch_nicb   nICb  L_ 1 1    N "number of stoichiometry units (<= nIC) used in the data bridge"
//o_ch_ndcb   nDCb  L_ 1 1    N "number of DC (chemical species, <= nDC) used in the data bridge"
//o_ch_nphb   nPHb  L_ 1 1    N "number of phases (<= nPH) used in the data bridge"
//o_ch_npsb   nPSb  L_ 1 1    N "number of multicomponent phases (<= nPS) used in the data bridge"
//wnData.push_back( pagesSetupData("xIC", o_ch_xic));//ICNL indices in DATABR IC vectors [nICb]"
//wnData.push_back( pagesSetupData("xDC", o_ch_xdc));//DCNL indices in DATABR DC list [nDCb]"
//wnData.push_back( pagesSetupData("xPH", o_ch_xph));//PHNL indices in DATABR phase vectors [nPHb]"
//wnData.push_back( pagesSetupData("Amat", o_ch_a));//DC stoichiometry matrix A composed of a_ij [nIC][nDC]"
wnData.push_back( pagesSetupData("ICmm", o_ch_icmm));//IC atomic (molar) mass, g/mole [nIC]"
wnData.push_back( pagesSetupData("DCmm", o_ch_dcmm));//DC molar mass, g/mole [nDC]"
//o_ch_dd     DD    D_  -3 1  j "[nDC] diffusion koefficients, for now constant"
//o_ch_aalp   AalP  D_  -3 1  N "reserved"

 break;

 case -2:
//# Data Bridge access DOD list
      wnData.push_back( pagesSetupData("n1bIC", o_n1_bic));//bulk mole amounts of IC[nICb] in node 1 + + - -"
      wnData.push_back( pagesSetupData("n1uIC", o_n1_uic));//IC chemical potentials (mol/mol)[nICb]  - - + +"
      wnData.push_back( pagesSetupData("n1xDC", o_n1_xdc));//DC mole amounts at equilibrium in node 1 [nDCb] - - + +"
      wnData.push_back( pagesSetupData("n1gam", o_n1_gam));//activity coeffs of DC [nDCb] in node 1 - - + +"
      wnData.push_back( pagesSetupData("n1xPH", o_n1_xph));//total mole amounts of phases [nPHb] in node 1 - - + +"
      wnData.push_back( pagesSetupData("n1mPS", o_n1_mps));//phase mass, g [nPSb] in node 1 - - + +"
      wnData.push_back( pagesSetupData("n1vPS", o_n1_vps));//phase volume, cm3 [nPSb] in node 1 - - + +"
      wnData.push_back( pagesSetupData("n1xPA", o_n1_xpa));//amount of carrier in phases  [nPSb] ?? in node 1 - - + +"
      wnData.push_back( pagesSetupData("n1bSP", o_n1_bsp));//Bulk composition of all solids  [nICb] in node 1 - - + +"
      wnData.push_back( pagesSetupData("n1aPH", o_n1_aph));// Specific surface areas of phases (m2/kg) in node 1 + + - -"
      wnData.push_back( pagesSetupData("n1rMB", o_n1_rmb));//MB Residuals from GEM IPM [nICb] in node 1 - - + +"
      wnData.push_back( pagesSetupData("n1dll", o_n1_dll));//lower kinetic restrictions [nDCb] in node 1 + + - -"
      wnData.push_back( pagesSetupData("n1dul", o_n1_dul));//upper kinetic restrictions [nDCb] in node 1 + + - -"
      wnData.push_back( pagesSetupData("n1amrl", o_n1_amrl));
      wnData.push_back( pagesSetupData("n1amru", o_n1_amru));

wnData.push_back( pagesSetupData("n1mPH", o_n1w_mps));//masses of phases in equilibrium, kg [nPHb] in node 1 + + - -"
wnData.push_back( pagesSetupData("n1vPH", o_n1w_vps));//volumes of phases in equilibrium, m3 [nPSb] in node 1 + + - -"
wnData.push_back( pagesSetupData("n1m_t", o_n1w_m_t));//total dissolved molality of independent components, m [nICb] in node 1 + + - -"
wnData.push_back( pagesSetupData("n1con", o_n1w_con));//DC concentrations in phases (molal, mole fraction) [nDCb] in node 1 + + - -"
wnData.push_back( pagesSetupData("n1mju", o_n1w_mju));//DC chemical potentials in equilibrium, J/mol [nDCb] in node 1 + + - -"
wnData.push_back( pagesSetupData("n1lga", o_n1w_lga));//DC activities in equilibrium, in log10 scale [nDCb] in node 1 + + - -"

      for(int  k=0; k<TMulti::sm->GetPM()->FIs; k++ )
      {
          string name;
          name =string( TMulti::sm->GetPM()->SF[k]+MAXSYMB, 0, MAXPHNAME);
          strip(name);
          wnData.push_back( pagesSetupData("n1bPS", o_n1_bps, k, name.c_str() ));
      }
//      wnData.push_back( pagesSetupData("n1bPS", o_n1_bps));//bulk compositions of phases [nPSb][nICb] in node 1 - - + +"

scalarsList.push_back( pagesSetupData("n1ct[0]",o_n1_ct,0)); //Node 1 controls
scalarsList.push_back( pagesSetupData("n1ct[1]",o_n1_ct,1));
scalarsList.push_back( pagesSetupData("n1ct[2]",o_n1_ct,2));
scalarsList.push_back( pagesSetupData("n1ct[3]",o_n1_ct,3));
scalarsList.push_back( pagesSetupData("n1ct[4]",o_n1_ct,4));
scalarsList.push_back( pagesSetupData("n1ct[5]",o_n1_ct,5));

scalarsList.push_back( pagesSetupData("n1cs[0]",o_n1_cs,0)); // Node 1 scalar chemical variables
scalarsList.push_back( pagesSetupData("n1cs[1]",o_n1_cs,1));
scalarsList.push_back( pagesSetupData("n1cs[2]",o_n1_cs,2));
scalarsList.push_back( pagesSetupData("n1cs[3]",o_n1_cs,3));
scalarsList.push_back( pagesSetupData("n1cs[4]",o_n1_cs,4));
scalarsList.push_back( pagesSetupData("n1cs[5]",o_n1_cs,5));
scalarsList.push_back( pagesSetupData("n1cs[6]",o_n1_cs,6));
scalarsList.push_back( pagesSetupData("n1cs[7]",o_n1_cs,7));
scalarsList.push_back( pagesSetupData("n1cs[8]",o_n1_cs,8));
scalarsList.push_back( pagesSetupData("n1cs[9]",o_n1_cs,9));
scalarsList.push_back( pagesSetupData("n1cs[10]",o_n1_cs,10));
scalarsList.push_back( pagesSetupData("n1cs[11]",o_n1_cs,11));
scalarsList.push_back( pagesSetupData("n1cs[12]",o_n1_cs,12));

scalarsList.push_back( pagesSetupData("n1ts[0]",o_n1_ts,0)); // Node 1 scalar transport variables
scalarsList.push_back( pagesSetupData("n1ts[1]",o_n1_ts,1));
scalarsList.push_back( pagesSetupData("n1ts[2]",o_n1_ts,2));
scalarsList.push_back( pagesSetupData("n1ts[3]",o_n1_ts,3));
scalarsList.push_back( pagesSetupData("n1ts[4]",o_n1_ts,4));
scalarsList.push_back( pagesSetupData("n1ts[5]",o_n1_ts,5));
scalarsList.push_back( pagesSetupData("n1ts[6]",o_n1_ts,6));
scalarsList.push_back( pagesSetupData("n1ts[7]",o_n1_ts,7));
scalarsList.push_back( pagesSetupData("n1ts[8]",o_n1_ts,8));
scalarsList.push_back( pagesSetupData("n1ts[9]",o_n1_ts,9));
scalarsList.push_back( pagesSetupData("n1ts[10]",o_n1_ts,10));
scalarsList.push_back( pagesSetupData("n1ts[11]",o_n1_ts,11));
scalarsList.push_back( pagesSetupData("n1ts[12]",o_n1_ts,12));
scalarsList.push_back( pagesSetupData("n1ts[13]",o_n1_ts,13));
scalarsList.push_back( pagesSetupData("n1ts[14]",o_n1_ts,14));
scalarsList.push_back( pagesSetupData("n1ts[15]",o_n1_ts,15));
scalarsList.push_back( pagesSetupData("n1ts[16]",o_n1_ts,16));
scalarsList.push_back( pagesSetupData("n1ts[17]",o_n1_ts,17));
scalarsList.push_back( pagesSetupData("n1ts[18]",o_n1_ts,18));

    break;

// All this must be included in GtDemo selections for SysEq and Proces
// Submodules

  case MD_RMULTS:
//#  Modelling Project (Rmults) DOD fields
wnData.push_back( pagesSetupData("Vale_", o_muval)); //I "Vector of default valencies of IC "
wnData.push_back( pagesSetupData("M0ic",  o_mubc)); //I "Vector of IC atomic (molar) masses, g/mol "
// o_muas    S0ic   F_  -1 1  I "Vector of IC atomic entropies, J/K/mol (reserved) "
// o_muav    V0ic   F_  -1 1  I "Vector of IC molar volumes, cm3/mol (reserved) "
    break;

  case MD_MTPARM:
//# Modelling Project Mtparm DOD fields (J indexation)
scalarsList.push_back( pagesSetupData("T",o_tp_T)); //"Current temperature of interest (T) in C"
scalarsList.push_back( pagesSetupData("P",o_tp_P)); //"Current pressure of interest (P) "
scalarsList.push_back( pagesSetupData("RTf[0]",o_tp_RT,0)); //"Current RT factor (J/mol)_
scalarsList.push_back( pagesSetupData("RTf[1]",o_tp_RT,1)); // T of interest in Kelvins "
scalarsList.push_back( pagesSetupData("RoW[0][0]",o_tprow,0)); //"Current densities of liquid water and steam (in g/cm3) "
scalarsList.push_back( pagesSetupData("EpsW[0][0]",o_tpepsw,0)); // "Current dielectric constants of liquid water and steam "
scalarsList.push_back( pagesSetupData("VisW[0]",o_tpvisw,0)); //"Current dynamic viscosities of liquid water and steam (in kg/m*sec) "
//o_tp_L    LtpD   I_ 4 1    N "Dimensionalities: total L of DC (species)_
// Ls of DC (species) in multi-component phases_
// PG of DC (gases) in gas phase_
// Lx of DC - surface species "
// dynamic - thermod. data
wnData.push_back( pagesSetupData("gTP", o_tpg)); //"Molar (molal) standard Gibbs energy functions of Dependent Components at T,P of interest "
//o_tpdevg  gTPuns D_  -2 1  J "Uncertainties of gTP values (reserved for future versions) "
wnData.push_back( pagesSetupData("hTP", o_tph)); //"Molar (molal) standard enthalpy functions of Dependent Components at T,P of interest "
//o_tpdevh  hTPuns D_  -2 1  J "Uncertainties of hTP values (reserved for future versions) "
wnData.push_back( pagesSetupData("STP", o_tps)); //"Molar (molal) entropies of Dependent Components at T,P of interest "
//o_tpdevs  STPuns D_  -2 1  J "Uncertainties of STP values (reserved for future versions) "
wnData.push_back( pagesSetupData("CpTP", o_tpcp)); //"Molar (molal) standard entropies of Dependent Components at T, P of interest "
//o_tpdevcp CTPuns D_  -2 1  J "Uncertainties of CpTP values (reserved for future versions) "
wnData.push_back( pagesSetupData("VTP", o_tpvm)); //"Molar (molal) volumes of Dependent Components at T,P of interest "
//o_tpdevv  VTPuns D_  -2 1  J "Uncertainties of VTP values (reserved for future versions) "
//o_tpf     F      D_  -2 1  J "Helmholtz energy F(T) of Dependent Components (reserved) "
//o_tpu     U      D_  -2 1  J "Internal energy U(T) of Dependent Components (reserved) "
//o_tpcv    Cv     D_  -2 1  J "Heat capacity Cv(T) of Dependent Components (reserved) "
//o_tpalp   alpha  D_  -2 1  J "Compressibility of Dependent Components (reserved) "
//o_tpbet   beta   D_  -2 1  J "Expansibility of Dependent Components (reserved) "
//o_tpwbor  Wborn  D_  -2 1  J "Born factor, relative, for aqueous and surface species (reserved) "
//o_tpwrad  Wrad   D_  -2 1  J "Effective solvation radii (HKF) (reserved) "
//o_tpfug   Fug    D_  -2 1  J "Fugacities of Dependent Components (FGL, reserved) "
//o_tpdvg   dVg    D_  -2 4  N "Work array for CG EoS coeffs (internal), 4 per gas "
    break;

  case MD_SYSTEM:
//# SYSTEM Definition DOD (I J A K indexation)
    scalarsList.push_back( pagesSetupData("Masses[0]",o_symass,0)); //"Molality of reference electrolyte. Reserved for future use_
    scalarsList.push_back( pagesSetupData("Masses[1]",o_symass,1)); // Anticipated mass (kg) of water-solvent (Mwat)_
    scalarsList.push_back( pagesSetupData("Masses[2]",o_symass,2)); // Anticipated total mass of the system Msys (kg)_
    scalarsList.push_back( pagesSetupData("Masses[3]",o_symass,3)); // Anticipated mass of aqueous phase Maq (kg)_
    scalarsList.push_back( pagesSetupData("Masses[4]",o_symass,4)); // Final total mass of the system MBX (kg)_
    scalarsList.push_back( pagesSetupData("Masses[5]",o_symass,5)); // Total number of IC moles in the system NMS  (for mole %% calculations) "
    scalarsList.push_back( pagesSetupData("Volums[0]",o_syvol,0)); //"Anticipated volume Vsys of the system (L)_
    scalarsList.push_back( pagesSetupData("Volums[1]",o_syvol,1)); // Anticipated volume Vaq  of aqueous phase (L) for molarities "
scalarsList.push_back( pagesSetupData("N_",o_syn)); //"N, Number of Independent Components included into CSD "
scalarsList.push_back( pagesSetupData("L_[0]",o_syl,0)); //"L, Number of Dependent Components included into CSD_
scalarsList.push_back( pagesSetupData("L_[1]",o_syl,1)); // Number of Dependent Components included into  multicomponent phases (Ls)_
scalarsList.push_back( pagesSetupData("L_[2]",o_syl,2)); // Number of species selected into the aqueous phase (Lw, including solvent)_
scalarsList.push_back( pagesSetupData("L_[3]",o_syl,3)); // Number of species selected into sorption phase(s) (Lads)_
scalarsList.push_back( pagesSetupData("L_[4]",o_syl,4)); // Number of species selected into gas phase (Lg)_
scalarsList.push_back( pagesSetupData("L_[5]",o_syl,5)); // Number of species selected into the liquid hydrocarbon phase (Lhc, reserved) "
scalarsList.push_back( pagesSetupData("Fi_[0]",o_syfi,0)); //"Number of phases taken into this CSD (Fi)_
scalarsList.push_back( pagesSetupData("Fi_[1]",o_syfi,1)); // Number of multi-component phases in this CSD (Fis)_
scalarsList.push_back( pagesSetupData("Fi_[2]",o_syfi,2)); // Number of phases used in non-zero quantities in BCC calculation (sequential reactors mode only)_
scalarsList.push_back( pagesSetupData("Fi_[3]",o_syfi,3)); // Number of phases with kinetic/metastability restrictions (reserved) "
//o_sylbc   Lbc_   I_ 1 4    N "Number of PCO used in non-zero quantities/concentrations in BCC calculation (La)_
//Number of DC stoichiometries used in non-zero quantities/concentrations in BCC calculation (Lb)_
// Number of DC with non-trivial metastability restrictions imposed in dll_ or dul_ vectors (Lk)_
// Number of non-zero IC quantities (bi_ vector) used in BCC calculations (Na) "
//o_sylns   LO_    I_ 1 7    N "Index of H2O-solvent in the project system DC list (L0)_
// Number of allocated SDref record key references (in sSDref optional list)_
// Maximal number of surface types in sorption phases (NsTm)_
// Number of DC with non-zero values in gEx_ or lnGmf_ vectors (Le)_
// Number of phases with non-zero surface areas or metastability parameters in yOf vector (Fie)_
// Number of sorption phases included in this CSD (Fia)_
// Reserved "
//o_sypmm   Pmm    F_  1 3   N "min, max, increment for the pressure interpolation"
//o_sytmm   Tmm    F_  1 3   N "min, max, increment for the temperature interpolation "
//o_sytdev  TDev   F_  1 2   N "Target deviations for  minimization of thermodynamic potentials"
//o_symsolids  Msold  F_  1 1   N "Amount of total solids (g) from another equilibrium to add to B_ (provisional)"
//o_syproc  Procs  F_  1 3   N "Tau - physical time (for kinetic Process simulators) reserved_
// Xi - current value of process extent variable (reserved)_
// Nu - current value of process extent variable (reserved)_
// Reserved "
// dynamic input data for the system definition
wnData.push_back( pagesSetupData("xa_", o_syxea)); // K "Input quantities or concentrations of PCO contributing to B_ vector "
wnData.push_back( pagesSetupData("bi_", o_sybi)); // I "Input quantities or concentrations of Independent Components contributing to B_ vector "
wnData.push_back( pagesSetupData("B_", o_syb)); //"Input bulk composition of the system (moles of IC) obtained from BCC calculation "
//o_sydelb  B_uns  F_  -3 1  I "Uncertainties of the bulk composition vector B_  elements (moles) "
wnData.push_back( pagesSetupData("xd_", o_syxed)); // J "Input quantities or concentrations of Dependent Components contributing to B_ vector "
wnData.push_back( pagesSetupData("dul_", o_sydul)); // J "Upper metastability restrictions for DC mole quantities x at equilibrium state "
wnData.push_back( pagesSetupData("dll_", o_sydll)); //J "Lower metastability restrictions for DC mole quantities x at equilibrium state "
wnData.push_back( pagesSetupData("gEx_", o_sygex)); //J "Optional additions to input g(T,P) values for Dependent Components (J/mol) "
wnData.push_back( pagesSetupData("xp_", o_syphm)); // A "Input quantities or concentrations of Phases (from another SysEq) contributing to B_ vector "
//o_sypparc Pp_    F_  -3 1  J "Input partial pressures for Dependent Components, bar (reserved) "
//o_sypul   pul_   F_  -3 1  A "Vector of upper metastability restrictions to quantities of phases (reserved) "
//o_sypll   pll_   F_  -3 1  A "Vector of lower metastability restrictions to quantities of phases (reserved) "
wnData.push_back( pagesSetupData("Yof_", o_syyof)); // A "Input Phase - aqueous interface energy parameter, J/g "
//o_sylngmf lnGmf_ F_  -3 1  J "DC activity coefficient constants for LPP AIA (in ln scale) "
wnData.push_back( pagesSetupData("Aalp_", o_syaalp)); // A "Specific surface area A of phase, m2/g "
wnData.push_back( pagesSetupData("L1_", o_syl1)); //"Number of Dependent Components selected within each phase "
// adsorption
//o_syxr0h0 Xr0h0_ F_  -3 2  A "Parameters r0 and h0 of particles (pores at r0<0), nm reserved "
//o_syxeps  Xeps_  F_  -3 2  A "Dielectric constant & electric conductivity of the sorbent or solvent, reserved "
wnData.push_back( pagesSetupData("Nfsp_", o_synfsp)); // A "Fractions of sorbent surface allocated to surface types "
//o_symasdt MaSdt_ F_  -3 3  A "Total maximum site density for surface types, 1/nm2 (for competitive SAT) "
wnData.push_back( pagesSetupData("XcapA_", o_syxcapa)); //A "Inner capacitance density parameter C1 (TLM,BSM,CCM) for surface types, F/m2 "
wnData.push_back( pagesSetupData("XcapB_", o_syxcapb)); // A "Outer capacitance density parameter C2 (TLM) for surface types, F/m2 "
wnData.push_back( pagesSetupData("XfIEC_", o_syxfiec)); // A "Density of permanent surface charge (mkeq/m2) "
wnData.push_back( pagesSetupData("XcapF_", o_syxcapf)); // A "Anionic outer-sphere capacitance density parameter C3 (F/m2), reserved "
wnData.push_back( pagesSetupData("Xlam_", o_syxlam)); // A "FFG parameter for surface types, >0, <1 (reserved) "
wnData.push_back( pagesSetupData("MaSdj_[][0]", o_symasdj,0)); //S "| Parameters for surface activity correction terms: Surface (site) complete coverage density (1/nm2)_
wnData.push_back( pagesSetupData("MaSdj_[][1]", o_symasdj,1)); //| Formula unit charge assigned to 0 surface EDL plane_
wnData.push_back( pagesSetupData("MaSdj_[][2]", o_symasdj,2)); //| Formula unit charge assigned to beta EDL surface plane_
wnData.push_back( pagesSetupData("MaSdj_[][3]", o_symasdj,3)); //| Isotherm parameter 1 (interaction parameter in Frumkin isotherm)_
wnData.push_back( pagesSetupData("MaSdj_[][4]", o_symasdj,4)); //| Isotherm parameter 2 (dentateness in QCA isotherms)_
wnData.push_back( pagesSetupData("MaSdj_[][5]", o_symasdj,5)); //| Isotherm parameter 3 (reserved for other isotherms) "
wnData.push_back( pagesSetupData("Sigm_[][0]", o_sysigm,0)); // A "| Specific surface energy of phase-aqueous interface, J/m2_
wnData.push_back( pagesSetupData("Sigm_[][1]", o_sysigm,1)); //| Specific surface energy of phase-gas interface, J/m2 (not used yet)"
//o_syguns   gUns   F_  -3 1  J "work vector of uncertainty space increments to tp->G + sy->GEX(J/mol) "
//o_syvuns   vUns   F_  -3 1  J "work vector of uncertainty space increments to tp->Vm  (J/mol) "
    break;

  case MD_MULTI:
//# Modelling Project IPM MULTI DOD (i j a indexation and basis)
    scalarsList.push_back( pagesSetupData("Mbx",o_wi_mbx)); //"Total mass of the system, kg "
    scalarsList.push_back( pagesSetupData("GX",o_wo_fx)); //"Total equilibrium Gibbs potential of the system, in moles "
    scalarsList.push_back( pagesSetupData("IS",o_wo_ic)); //"Effective molal ionic strength of aqueous electrolyte "
    scalarsList.push_back( pagesSetupData("pH",o_wd_ph)); //"pH of aqueous solution (in molal activity scale) "
    scalarsList.push_back( pagesSetupData("pe",o_wd_pe)); //"pe of aqueous solution (in molal activity scale) "
    scalarsList.push_back( pagesSetupData("Eh",o_wd_eh)); //"Eh of aqueous solution, Volts "

    scalarsList.push_back( pagesSetupData("TC[0]",o_wio_tc)); //"Temperature (C) of interest (min-max interval for parametric problems) "
    scalarsList.push_back( pagesSetupData("TC[1]",o_wio_tc,1));
    scalarsList.push_back( pagesSetupData("TK[0]",o_wio_t)); //"Temperature (K) of interest T (min-max interval for parametric problems) "
    scalarsList.push_back( pagesSetupData("TK[1]",o_wio_t,1));
    scalarsList.push_back( pagesSetupData("PG[0]",o_wio_p)); //"General pressure P (bars), min.-max. for parametric problems "
    scalarsList.push_back( pagesSetupData("PG[1]",o_wio_p,1));
    scalarsList.push_back( pagesSetupData("Vx[0]",o_wo_vx)); //"Initial V and calculated V(x) volume of the system (cm3) "
    scalarsList.push_back( pagesSetupData("Vx[1]",o_wo_vx,1));
    //o_wo_gx   Gx     D_  2 1   N "Gibbs potential of the system in J (min.-max. for parametric problems) "
    //o_wo_ax   Fx     D_  2 1   N "Helmholtz potential of the system F(X), reserved"
    //o_wo_ux   Ux     D_  2 1   N "Internal energy of the system U(X), reserved"
    //o_wo_hx   Hx     D_  2 1   N "Total enthalpy of the system H(X), reserved"
    //o_wo_sx   Sx     D_  2 1   N "Total entropy of the system S(X), reserved"
    //o_wo_cpx  Cpx    D_  2 1   N "reserved"
    //o_wo_cvx  Cvx    D_  2 1   N "reserved"
    //o_wi_t0   T0     D_  1 1   N "reserved"
    //o_wi_ve   Vexpl  D_  1 1   N "reserved"
    scalarsList.push_back( pagesSetupData("It",o_wo_it)); //o_wo_it   It     L_ 1 1    N "Number of completed IPM algorithm iterations "
    scalarsList.push_back( pagesSetupData("ItEfd", o_wo_it_efd)); // "Number of performed iterations in  MassBalanceRefinement()"
    scalarsList.push_back( pagesSetupData("ItIpm", o_wo_it_ipm)); // "Number of performed iterations in  main IPM descent algorithm"
    scalarsList.push_back( pagesSetupData("Psi_DK[0]",o_wo_psi,0)); // "Current value for the Dikin's criterion of IPM convergence (CD)_
    scalarsList.push_back( pagesSetupData("Psi_DK[1]",o_wo_psi,1)); // Threshold value for the Dikin's criterion of IPM convergence (T_CD) "

scalarsList.push_back( pagesSetupData("_nnr[0]",o_wi_nnr,0)); //"Number of Independent Components in the IPM problem n(N)_
scalarsList.push_back( pagesSetupData("_nnr[1]",o_wi_nnr,1)); // NR - dimensions of the R matrix"
scalarsList.push_back( pagesSetupData("L[0]",o_wi_ldim,0)); //"Number of Dependent Components in the IPM problem n(L)_
scalarsList.push_back( pagesSetupData("L[1]",o_wi_ldim,1)); // Number of Dependent Components included into  multicomponent phases n(Ls)_
scalarsList.push_back( pagesSetupData("L[2]",o_wi_ldim,2)); // Index of water-solvent LO, equals number of aqueous species n(Laq)_
scalarsList.push_back( pagesSetupData("L[3]",o_wi_ldim,3)); // Number of Dependent Components included in the gas phase PG = n(Lg)_
scalarsList.push_back( pagesSetupData("L[4]",o_wi_ldim,4)); // Number of Dependent Components in the liquid hydrocarbon phase PSOL (reserved)_
scalarsList.push_back( pagesSetupData("L[5]",o_wi_ldim,5)); // Number of Dependent Components included in sorption phases n(Lads) "
scalarsList.push_back( pagesSetupData("Fi[0]",o_wi_fdim,0)); //"Number of Phases included in the IPM problem n(Fi)_
scalarsList.push_back( pagesSetupData("Fi[1]",o_wi_fdim,1)); // Number of multi-component phases n(Fis)_
scalarsList.push_back( pagesSetupData("Fi[2]",o_wi_fdim,2)); // Number of sorption multi-component phases n(Fia) "
scalarsList.push_back( pagesSetupData("FI[0]",o_wo_fdim,0)); //"Number of phases present in non-zero quantities at equilibrium state_
scalarsList.push_back( pagesSetupData("FI[1]",o_wo_fdim,1)); // Number of multi-component phases present in non-zero quantities at equilibrium state_
scalarsList.push_back( pagesSetupData("FI[2]",o_wo_fdim,2)); // Number of sorption phases present in non-zero quantities at equilibrium state "

//o_wi_mode pm_Mod L_ 1 4    N "PE - flag of electroneutrality constraint { 0 1 }_
// PD - mode of calling CalculateActivityCoefficients() { 0 1 2 3 4 }_
// PV - flag of system volume constraint { 0 1 }_
// PU - flag of kinetic restrictions to DC quantities x_j { 0 1 }"
//o_wo_ipar pmIpar L_  1 17  N "CalculateActivityCoefficients() return code: 0 (OK) or 1 (error)_
// Number of Selekt2() procedure loops_
// Indicator of the same second assignment in Selekt2()_
// Mode of FIADE() operation: 0-LPP AIA, 1-old speciation SIA, -1-raised old speciation SIA_
// Flag to expand the currently loaded SysEq record into Multi data structure_
// Flag to control binary data for Phase math script calculations_
// Flag to control re-loading CSD data into Multi_
// Flag to control loading of activity coefficients for initial approximation_
// Flag to control re-loading thermodynamic data into Multi_
// internal_
// internal_
// Max. number of surface types on sorption phase ( 6 )_
// IPM return code: 0 - continue;  1 - converged_
// internal DebyeHueckel3() indicator_
// is - index of IC for Phase math scripts ( CalculateActivityCoefficients() )_
// js - index of DC for Phase math scripts ( CalculateActivityCoefficients() )_
// Next - continue variable for Phase math scripts ( CalculateActivityCoefficients() )"
//o_wi_mol  bMol   D_  1 1   N "reserved"
//o_wi_gwat wMol   D_  1 1   N "reserved"
//o_wi_ymet yMet   D_  1 1   N "reserved"
scalarsList.push_back( pagesSetupData("denW[0][0]",o_wio_denw,0)); //"Density of H2O(l)_
scalarsList.push_back( pagesSetupData("denW[1][0]",o_wio_denw,1)); // Density of steam  at T,P of interest "
scalarsList.push_back( pagesSetupData("epsW[0][0]",o_wio_epsw,0)); //"Dielectric constant of H2O(l)_
scalarsList.push_back( pagesSetupData("epsW[1][0]",o_wio_epsw,1)); // Dielectric constant of steam  for T,P of interest "
scalarsList.push_back( pagesSetupData("lnP",o_wo_lnp)); //"Natural logarithm of total pressure "
scalarsList.push_back( pagesSetupData("RT",o_wo_rt)); //"Factor RT = 8.31451* T (J/mol), T is current temperature in Kelvins "
scalarsList.push_back( pagesSetupData("F_RT",o_wo_frt)); //"Factor F/RT, where F= 96485 C/mol is the Faraday's constant "
scalarsList.push_back( pagesSetupData("Xw",o_wo_yw)); //N "Current number of moles of the solvent in aqueous phase "

// dynamic
// primary output for ICs
wnData.push_back( pagesSetupData("u", o_wo_u)); //i "Chemical potentials of Independent Components at equilibrium state (mol/mol) "
wnData.push_back( pagesSetupData("ue", o_wd_ur)); // i "Chemical potentials of Independent Components at equilibrium state (J/mol) "
//o_wd_uc   uc     D_  -3 1  i  "Copy of Chemical potentials of Independent Components at equilibrium state (mol/mol) "
wnData.push_back( pagesSetupData("b", o_wi_b)); // i "Input bulk composition of the system (moles of Independent Components) "
wnData.push_back( pagesSetupData("Cb", o_wo_c)); // i "Mass balance residuals for Independent Components (at IPM convergence) "
wnData.push_back( pagesSetupData("m_t", o_wd_icm)); //i "Output total dissolved molalities of Independent Components at equilibrium state "
wnData.push_back( pagesSetupData("lgm_t", o_wd_iclcm)); // i "Decimal logarithms of total dissolved molalities of Independent Components at equilibrium state "
// wnData.push_back( pagesSetupData("w_t", o_wd_icwm)); //i "Output total dissolved mass concentrations of Independent Components at equilibrium state "
wnData.push_back( pagesSetupData("icM", o_wi_awt)); //i "Input  atomic (molar) masses of Independent Components, g/mol "

// primary output for Phases
wnData.push_back( pagesSetupData("Xa", o_w_xf)); //a "Output total mole quantities of phases "
//o_wo_yf   Xao    D_  -3 1  a "Work copy of the Xa  vector from previous IPM iteration "
wnData.push_back( pagesSetupData("Xwa", o_ww_xfa)); // a "Output mole quantities of the solvent (sorbent) in multi-component phases "
//o_ww_yfa  Xwao   D_  -3 1  a "Work copy of  the Xwa  vector from previous IPM iteration "
wnData.push_back( pagesSetupData("phVol", o_wd_fvol)); // a "Volumes of phases (in cm3) "
wnData.push_back( pagesSetupData("phM", o_wd_fwgt)); //a "Masses of phases (in grams) "
wnData.push_back( pagesSetupData("Fa", o_wo_falp)); //a "Values of the phase stability criterion f(alpha) "

for(int  k=0; k<TMulti::sm->GetPM()->FIs; k++ )
{
    string name;
    name =string( TMulti::sm->GetPM()->SF[k]+MAXSYMB, 0, MAXPHNAME);
    strip(name);
    wnData.push_back( pagesSetupData("bXa", o_wo_bf, k, name.c_str() ));
   //d "Output bulk compositions of multicomponent phases (in moles of Independent Components) "
}

wnData.push_back( pagesSetupData("bXs", o_wo_bfc )); // i "Total output bulk compositions of solid phases (in moles of Independent Components) "
wnData.push_back( pagesSetupData("L1", o_wi_l1)); //"Input number of Dependent Components included into each phase (l-alpha vector) "
wnData.push_back( pagesSetupData("Yof", o_wi_yof)); // a "Optional phase - aqueous interface energy increment (in (J/mol)/(RT) ) "
wnData.push_back( pagesSetupData("Aalp", o_wi_aalp)); //a "Specific surface area A of phases, m2/g "
wnData.push_back( pagesSetupData("Sigw", o_wi_sigw)); //a "Specific surface energy of phase - aqueous interface, J/m2 "
//o_wi_sigg Sigg   D_  -3 1  a "Specific surface energy of phase - gas interface, J/m2 (not used yet)"
//o_wi_pul  pul    D_  -3 1  a "reserved"
//o_wi_pll  pll    D_  -3 1  a "reserved"
//o_wi_pmc  pmc    D_  -3 1  N "Interaction coefficients for non-ideal mixing in multi-component phases "
//o_wi_dmc  dmc    D_  -3 1  N "Component-specific coefficients for non-ideal mixing in multi-component phases "

// primary output for DCs
wnData.push_back( pagesSetupData("x", o_w_x)); //j "Output mole quantities of Dependent Components (species) at equilibrium state (primal solution) "
//o_wo_y    y      D_  -3 1  j "Work copy of the x vector from previous IPM iteration "
//o_w_xy    xy     D_  -3 1  j "Work copy of the x vector from previous loop of Selekt2() procedure "
//o_w_xu    XU     D_  -3 1  j "Restored x in Mol_u - internal"
wnData.push_back( pagesSetupData("Wxx", o_wo_wx)); // j "Mole fractions of Dependent Components in multi-component phases "
wnData.push_back( pagesSetupData("my", o_wd_ym)); // j "Molality concentrations of Dependent Components (in mol per kg H2O) "
wnData.push_back( pagesSetupData("v", o_wo_f)); // j "Primal chemical potentials of Dependent Components (in mol/mol) "
wnData.push_back( pagesSetupData("vEx", o_w_f0)); // j "Excess molar Gibbs energy of Dependent Components, mol/mol "
wnData.push_back( pagesSetupData("lga", o_wd_yla)); // j "Log10 of general activity (fugacity) of Dependent Components at equilibrium state "
// wnData.push_back( pagesSetupData("wy", o_wd_yw)); // j "Mass concentrations of DC in multi-component phases, % (ppm in aqueous phase) "
wnData.push_back( pagesSetupData("gamma", o_wd_gamma)); // j "Activity coefficients of DCs (external) in practical scale (molal for aqueous species) "
wnData.push_back( pagesSetupData("lnGam", o_wo_lngam)); // j "Calculated  ln of internal (rational) DC activity coefficients in all phases "
//o_w_lngmo lnGmo  D_  -3 1  j "Copy of the lnGam vector from the previous IPM iteration "
//o_wo_lnsat lnSAT D_  -3 1  s "ln of surface activity coefficient terms for surface species "

// secondary input/output for Phases
//o_wi_ipxpm IPxPM L_  -3 1  a "List of indexes for interaction parameters in Phase solution models"
//o_wi_lsmod Lsmod L_  -3 1  a "Number of non-ideality coeffs per multicomponent phase "
//o_wi_lsmdc Lsmdc L_  -3 1  a "Number of non-ideality coefficients per one DC in multicomponent phases "
//o_wi_mui  mui    L_  -3 1  i "IC indices in the Project System "
//o_wi_muk  muk    L_  -3 1  a "Phase indices in the Project System "
//o_wi_muj  muj    L_  -3 1  j "DC indices in the Project System "
//o_wi_satndx sati   L_  -3 2  s "| Link indexes of surface species to: surface type { 0 ... 5 }_
//| Link index to sorbent end-member { 0 ... 5 -1 -2 }_
//| Link index to surface site on surface type { 0,...,5 }_
//| Link index to surface EDL plane { 0 b c d }_ "

// secondary input/output for DCs
wnData.push_back( pagesSetupData("dul", o_wi_dul)); //"Upper metastability restrictions for DC mole quantities x at equilibrium state "
wnData.push_back( pagesSetupData("dll", o_wi_dll)); //"Lower metastability restrictions for DC mole quantities x at equilibrium state "
wnData.push_back( pagesSetupData("c", o_wo_g)); //"Normalized energy function of Dependent Components, mol/mol "
wnData.push_back( pagesSetupData("g0", o_wi_g0)); // j "Molar (molal) standard Gibbs energy functions of Dependent Components at T,P of interest "
wnData.push_back( pagesSetupData("fDQF", o_wi_gex)); // j "Optional DQF parameters or pure gas fugacities for Dependent Components "
wnData.push_back( pagesSetupData("vol", o_wio_vol)); //o_wio_vol vol    D_  -3 1  j "Molar (partial molal) volumes of Dependent Components (in cm3/mol) at T,P of interest "
//o_wio_h0  h0     D_  -3 1  j "DC molar enthalpies, reserved"
//o_wio_a0  f0     D_  -3 1  j "DC molar Helmholtz energies, reserved"
//o_wio_u0  u0     D_  -3 1  j "DC molar internal energies, reserved"
//o_wio_s0  s0     D_  -3 1  j "DC molar entropies, reserved"
//o_wio_cp0 cp0    D_  -3 1  j "DC molar Cp, reserved"
//o_wio_cv0 cv0    D_  -3 1  j "DC molar Cv, reserved"
// wnData.push_back( pagesSetupData("Pparc", o_wi_pparc)); //  j "Partial pressure constraints for DC, bar (total P by default)"
//o_wi_lngmf lngmf D_  -3 1  j "Ln activity coefficient constants for LPP AIA only "
//o_wio_lngmm lngmm D_ -3 1  j "ln of DC metastability coefficients (reserved)"
wnData.push_back( pagesSetupData("mmDC", o_wi_mm)); // j "Molar (gram-formula) masses of Dependent Components (g/mol) "
wnData.push_back( pagesSetupData("ez", o_wi_ez)); //o_wi_ez   ez     D_  -3 1  j "Formula charges of Dependent Components "
wnData.push_back( pagesSetupData("A", o_wi_a)); //c "Input stoichiometry matrix of Dependent Components "

// Adsorption
wnData.push_back( pagesSetupData("nfsp", o_wi_nfsp)); // a "Fractions of sorbent surface allocated to surface types "
//o_wi_masdt masdt D_  -3 6  a "Total maximum site density for surface types, mkmol/g (for competitive SAT) "
wnData.push_back( pagesSetupData("capA", o_w_xcapa)); //a "Inner capacitance density parameter C1 (TLM,BSM,CCM) for surface types, F/m2 "
wnData.push_back( pagesSetupData("capB", o_w_xcapb)); // a "Outer capacitance density parameter C2 (TLM) for surface types, F/m2, or C_an (BSM) "
//o_w_xcapd capD   D_  -3 6  a "Capacitance density (reserved)"
wnData.push_back( pagesSetupData("capF", o_w_xcapf)); // a "Capacitance density of anionic EDL layer in FLM or CD-MUSIC, F/m2 (reserved) "
//o_wo_xdla dlhA   D_  -3 6  a "Reserved "
//o_wo_xdlb dlhB   D_  -3 6  a "Effective thickness of beta EDL layer, nm (reserved) "
//o_wo_xdld dlhD   D_  -3 6  a "Effective thickness of diffuse EDL layer, nm (reserved) "
wnData.push_back( pagesSetupData("psiA", o_wo_psia)); // a "Relative electrostatic potential on zero EDL plane (sorbent surface), V "
wnData.push_back( pagesSetupData("psiB", o_wo_psib)); // a "Relative electrostatic potential on beta EDL plane (outer-sphere), V "
wnData.push_back( pagesSetupData("psiD", o_wo_psid)); // a "Reserved "
//o_wi_xr0h0 r0h0  D_  -3 2  a "r0 and h0 parameters for phase particles (pores) (reserved) "
//o_wi_xlama lamA  D_  -3 6  a "FFG factor for surface species (reserved) "
wnData.push_back( pagesSetupData("etaF", o_wi_xetaf)); // a "Density of permanent surface charge, C/m2 "
//o_wi_xcond Cond  D_  -3 1  a "Electrical conductivity of the sorbent, reserved"
//o_wi_xeps Eps    D_  -3 1  a "Dielectric constant of the sorbent, reserved"
//o_wd_xfs  XFs    D_  -3 1  a "Current non-zero quantities of phases Xa at IPM iteration (likely to be removed in future versions) "
//o_wd_falps Fas   D_  -3 1  a "Current Karpov criteria of phase stability Fa for phases present in non-zero quantities (likely to be removed in future versions) "
//o_wd_fug  Gfug   D_  -3 1  N "Demo only: Partial fugacities of gases (in mole fraction scale) "
//o_wd_fugl lgFug  D_  -3 1  N "Demo only: Decimal logarithms of partial fugacities of gases "
//o_wd_ppgl lgPpG  D_  -3 1  N "Decimal logaritms of partial pressures of gases "
wnData.push_back( pagesSetupData("masdj[][0]", o_wo_masdj)); //s "| Parameters of surface species for calculation of activity correction terms: Max. density (umol/g)_
wnData.push_back( pagesSetupData("masdj[][1]", o_wo_masdj,1)); //| Species charge assigned to 0 EDL surface plane_
wnData.push_back( pagesSetupData("masdj[][2]", o_wo_masdj,2)); //| Species charge assigned to beta EDL surface plane_
wnData.push_back( pagesSetupData("masdj[][3]", o_wo_masdj,3)); //| Isotherm parameter 1 (Frumkin energy)_
wnData.push_back( pagesSetupData("masdj[][4]", o_wo_masdj,4)); //| Isotherm parameter 2 (Frumkin CN or dentateness )_
wnData.push_back( pagesSetupData("masdj[][5]", o_wo_masdj,5)); //| Isotherm parameter 3 (reserved ) "
//o_wo_vph  VPh   D_  -3 1  a   " Volume properties for mixed phases"
//o_wo_gph  GPh   D_  -3 1  a   " Gibbs energy properties for mixed phases "
//o_wo_hph  HPh   D_  -3 1  a   " Enthalpy properties for mixed phases"
//o_wo_sph  SPh   D_  -3 1  a   " Entropy properties for mixed phases "
//o_wo_cph  CPh   D_  -3 1  a   " Heat capacity Cp properties for mixed phases"
//o_wo_aph  APh   D_  -3 1  a   " Helmholtz energy properties for mixed phases "
//o_wo_uph  UPh   D_  -3 1  a   " Internal energy properties for mixed phases"
wnData.push_back( pagesSetupData("etaA", o_w_xetaa)); //a "Total surface charge on 0 EDL plane (sorbent surface), moles "
wnData.push_back( pagesSetupData("etaB", o_w_xetab)); // a "Total surface charge on 1 (beta) EDL plane (CD, outer-sphere), moles "
wnData.push_back( pagesSetupData("etaD", o_w_xetad)); //a "Total surface charge on 2 (d) EDL plane (CD, outer-sphere), moles "
wnData.push_back( pagesSetupData("Xfts", o_w_xfts)); // a "Total number of moles of surface species at surface type "
//o_wi_qp   qp     D_  -3 1  N "Work cells for execution of built-in functions or math scripts in multicomponent phases "
//o_wi_qd   qd     D_  -3 1  N "Work cells for execution of built-in functions or math scripts referring to a DC (species) in a non-ideal phase "
//o_wo_mu   mu     D_  -3 1  j "IPM vector Delta (direction of descent) "
//o_w_emu   emu    D_  -3 1  j "Exponent of DC increments to Karpov's phase stability criteria "
//o_w_nmu   nmu    D_  -3 1  j "DC increments to to Karpov's phase stability criteria "
//o_w_w     W      D_  -3 1  j "Weight multipliers for DC (incl. metastability restrictions) at IPM iteration "
//o_wo_fxa  fx     D_  -3 1  j "Dual chemical potentials of Dependent Components (in J/mol) "
//o_w_d     Dres   D_  -3 1  j "Residual deviations between the primal y vector and the DT-restored x vector (in IPM-2 procedures) "
//o_w_r     R      D_  -3 3  N "R matrix of IPM linear equations "
//o_w_r1    R1     D_  -3 3  N "Internal copy of R matrix "
//o_wd_site    sitE  F_  -3 1  N "obsolete - can be changed to anything else"
    break;

 }

}

