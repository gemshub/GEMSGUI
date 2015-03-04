//-------------------------------------------------------------------
// $Id: m_phase.cpp 1366 2009-07-18 20:54:59Z wagner $
//
// Implementation of TPhase class, config and calculation functions
//
// Rewritten from C to C++ by S.Dmytriyeva
// Copyright (C) 1995-2009 S.Dmytriyeva, D.Kulik, T. Wagner
//
// This file is part of a GEM-Selektor library for thermodynamic
// modelling by Gibbs energy minimization
// Uses: GEM-Selektor GUI GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the terms of GEMS3 Development
// Quality Assurance Licence (GEMS3.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//
#include <cmath>

#include "visor.h"
#include "service.h"
#include "m_dcomp.h"
#include "m_reacdc.h"
#include "s_formula.h"
#include "m_phase.h"
#include "m_param.h"
#include "v_object.h"
#include "filters_data.h"


// Setting up the DC/phase coeffs depending on the
// built-in model of mixing (from TSolMod class)
void TPhase::Set_SolMod_Phase_coef()
{

   switch(php->sol_t[SPHAS_TYP])
   {
       case SM_OTHER:   // Customized hardcoded solid-solution models
                      php->nscM = 0;   // NP_DC
                      php->npxM = 0;   // MaxOrd
                      php->ncpN = 0;   // NPar
                      php->ncpM = 0;   // NPcoef
                      break;
       case SM_BERMAN:   // Sublattice microscopic intra-site interaction model (multicomponent)
                      // php->nscM = 3;  // NP_DC  // reciprocal energies and their dependence on T
                      php->nrcp = 0;  // number of coefficients per reciprocal dG parameter
                      php->npxM = 4;  // MaxOrd
                      if( php->ncpN < 1 ) // NPar
                          php->ncpN = 1;
                      if( php->ncpN > (php->nDC * php->nDC * 4. ))
                          php->ncpN = php->nDC * php->nDC * 4.;   // Check max N of parameters!
                      php->ncpM = 3;  // NPcoef
                      break;
       case SM_CEF: // = '$', built-in multicomponent multisite solid-solution model (CALPHAD)
                       // php->nscM = 3;  // NP_DC  // reciprocal energies and their dependence on T
                       php->nrcp = 0;  // number of coefficients per reciprocal dG parameter
                       php->npxM = 4;  // MaxOrd
                       if( php->ncpN < 1 ) // NPar
                           php->ncpN = 1;
                       if( php->ncpN > (php->nDC * php->nDC * 4. ))
                       php->ncpN = php->nDC * php->nDC * 4.;   // Check max N of parameters!
                       php->ncpM = 3;  // NPcoef
       break;
                      break;
       case SM_VANLAAR:   // Van Laar model (multicomponent)
                      php->nscM = 1;  // NP_DC
                      php->npxM = 2;  // MaxOrd
                      if( php->ncpN < 1 ) // NPar
                          php->ncpN = 1;
                      if( php->ncpN > (php->nDC*(php->nDC-1)/2) )
                          php->ncpN = (php->nDC*(php->nDC-1)/2);
                      php->ncpM = 3;  // NPcoef
                      break;
       case SM_REGULAR:   // Regular model (multicomponent)
                      php->nscM = 0;  // NP_DC
                      php->npxM = 2;  // MaxOrd
                      if( php->ncpN < 1 ) // NPar
                          php->ncpN = 1;
                      if( php->ncpN > (php->nDC*(php->nDC-1)/2) )
                          php->ncpN = (php->nDC*(php->nDC-1)/2);
                      php->ncpM = 3;  // NPcoef
                      break;
       case SM_GUGGENM:   // Redlich-Kister model (multicomponent)
                      php->nscM = 0;  // NP_DC
                      php->npxM = 2;  // MaxOrd
                      if( php->ncpN < 1 ) // NPar
                          php->ncpN = 1;
                      if( php->ncpN > (php->nDC*(php->nDC-1)/2) )
                          php->ncpN = (php->nDC*(php->nDC-1)/2);
                      php->ncpM = 16;  // NPcoef
                      break;
      case SM_NRTLLIQ:   // NRTL liquid model (multicomponent), added 03.06.2008 (TW)
                      php->nscM = 0;  // NP_DC
                      php->npxM = 2;  // MaxOrd
                      if( php->ncpN < 1 ) // NPar
                          php->ncpN = 1;
                      if( php->ncpN > (php->nDC*(php->nDC-1)) )
                          php->ncpN = (php->nDC*(php->nDC-1));
                      php->ncpM = 6;  // NPcoef
                      break;
      case SM_WILSLIQ:   // Wilson liquid model (multicomponent), added 09.06.2008 (TW)
                      php->nscM = 0;  // NP_DC
                      php->npxM = 2;  // MaxOrd
                      if( php->ncpN < 1 ) // NPar
                          php->ncpN = 1;
                      if( php->ncpN > (php->nDC*(php->nDC-1)) )
                          php->ncpN = (php->nDC*(php->nDC-1));
                      php->ncpM = 4;  // NPcoef
                      break;
      case SM_REDKIS:   // Redlich-Kister model (binary)
                      php->nscM = 0;
                      php->npxM = 0;
                      php->ncpN = 1; php->ncpM = 3;
                      break;
      case SM_MARGB:  // Margules subregular model (binary)
                      php->nscM = 0;
                      php->npxM = 0;
                      php->ncpN = 2; php->ncpM = 3;
                      break;
      case SM_MARGT:  // Margules regular model (ternary)
                      php->nscM = 0;
                      php->npxM = 0;
                      php->ncpN = 4; php->ncpM = 3;
                      break;
      case SM_CGFLUID:  // Churakov-Gottschalk (CG) EoS
                      php->ncpN = 0;
                      php->ncpM = 0;
                      php->nscM = 12; // last changed 12.12.2008 (TW)
                      php->npxM = 0;
                      // php->nscN = 1; php->nscM = 4;  changed 07.12.2006 KD
                      break;
      case SM_PRFLUID:  // Peng-Robinson-Stryjek-Vera (PRSV) EoS, one binary interaction parameter
                      if( php->ncpN < 1 ) // NPar
                          php->ncpN = 1;
                      if( php->ncpN > (php->nDC*(php->nDC-1)/2) )
                          php->ncpN = (php->nDC*(php->nDC-1)/2);
                      php->ncpM = 2;  // NPcoef
                      php->nscM = 7;  // set to 7 to pull all parameters from CPg
                      php->npxM = 2;
                      break;
      case SM_SRFLUID:  // Soave-Redlich-Kwong (SRK) EoS, one binary interaction parameter
                      if( php->ncpN < 1 ) // NPar
                          php->ncpN = 1;
                      if( php->ncpN > (php->nDC*(php->nDC-1)/2) )
                          php->ncpN = (php->nDC*(php->nDC-1)/2);
                      php->ncpM = 2;  // NPcoef
                      php->nscM = 7;  // set to 7 to pull all parameters from CPg
                      php->npxM = 2;
                      break;
      case SM_PR78FL:  // Peng-Robinson (PR78) EoS, one binary interaction parameter
                      if( php->ncpN < 1 ) // NPar
                          php->ncpN = 1;
                      if( php->ncpN > (php->nDC*(php->nDC-1)/2) )
                          php->ncpN = (php->nDC*(php->nDC-1)/2);
                      php->ncpM = 2;  // NPcoef
                      php->nscM = 7;  // set to 7 to pull all parameters from CPg
                      php->npxM = 2;
                      break;
      case SM_CORKFL:  // compensated Redlich-Kwong (CORK) EoS, one binary interaction parameter
                      if( php->ncpN < 1 ) // NPar
                          php->ncpN = 1;
                      if( php->ncpN > (php->nDC*(php->nDC-1)/2) )
                          php->ncpN = (php->nDC*(php->nDC-1)/2);
                      php->ncpM = 1;  // NPcoef
                      php->nscM = 7;  // set to 7 to pull all parameters from CPg
                      php->npxM = 2;  // MaxOrd
                      break;
      case SM_STFLUID:  // Sterner-Pitzer (STP) EoS, one binary interaction parameter
                      if( php->ncpN < 1 ) // NPar
                          php->ncpN = 1;
                      if( php->ncpN > (php->nDC*(php->nDC-1)/2) )
                          php->ncpN = (php->nDC*(php->nDC-1)/2);
                      php->ncpM = 1;  // NPcoef
                      php->nscM = 7;  // set to 7 to pull all parameters from CPg
                      php->npxM = 2;  // MaxOrd
                      break;
      case SM_AQDAV:  // aqueous Davies
                      // php->ncpN = php->ncpM = 0;
                      php->ncpN = 2;
                      php->ncpM = 4; // changed 10.07.2008 DK
                      php->nscM = 0;
                      php->npxM = 0;
                      php->PphC = PH_AQUEL;
                      break;
      case SM_AQDH1:  // aqueous DH limiting law
                      php->ncpN = 2;
                      php->ncpM = 4;
                      php->nscM = 0;
                      php->npxM = 0;
                      php->PphC = PH_AQUEL;
                      break;
      case SM_AQDH2:  // aqueous DH, individual a0, individual bg
                      php->ncpN = 2;
                      php->ncpM = 4;
                      php->nscM = 2;
                      php->npxM = 0;
                      php->PphC = PH_AQUEL;
                      break;
      case SM_AQDH3:  // aqueous EDH Karpov, individual a0, common bg
                      php->ncpN = 2;
                      php->ncpM = 4;
                      php->nscM = 2;
                      php->npxM = 0;
                      php->PphC = PH_AQUEL;
                      break;
      case SM_AQDHH:  // aqueous EDH Helgeson, common a0 and bg
                      php->ncpN = 2;
                      php->ncpM = 4;
                      php->nscM = 0;
                      php->npxM = 0;
                      php->PphC = PH_AQUEL;
                      break;
      case SM_AQDHS:  // aqueous EDH Shvarov, common a0 and bg
                      php->ncpN = 2;
                      php->ncpM = 4;
                      php->nscM = 2;
                      php->npxM = 0;
                      php->PphC = PH_AQUEL;
                      break;
      case SM_AQSIT:  // SIT model in NEA variant - new implementation
                      php->nscM = 0;  // NP_DC
                      php->npxM = 2;  // MaxOrd
                      if( php->ncpN < 1 ) // NPar
                          php->ncpN = 1;
                      if( php->ncpN > (php->nDC*php->nDC/2) )
                          php->ncpN = (php->nDC*php->nDC/2);
                      php->ncpM = 2;  // NPcoef - changed from 1 to 2 on 13.05.09 (DK)
                      php->PphC = PH_AQUEL;
                      break;
      case SM_AQEXUQ: // built-in EUNIQUAC model for aqueous activity coeffs, changed 18.01.2009 (TW)
                      php->nscM = 2;  // NP_DC
                      php->npxM = 2;  // MaxOrd
                      if( php->ncpN < 1 ) // NPar
                          php->ncpN = 1;
                      if( php->ncpN > (php->nDC*(php->nDC-1)/2+php->nDC) )
                          php->ncpN = (php->nDC*(php->nDC-1)/2+php->nDC);
                       php->ncpM = 2;  // NPcoef
                       php->PphC = PH_AQUEL;
                       break;
      case SM_AQPITZ: // built-in Pitzer HMW aqueous activity coefficient model
                       php->nscM = 0;  // NP_DC
                       php->npxM = 4;  // MaxOrd
                       if( php->ncpN < 1 ) // NPar
                           php->ncpN = 1;
                       if( php->ncpN > (php->nDC*php->nDC) )
                           php->ncpN = (php->nDC*php->nDC);
                       if( php->ncpM <= 5 )
                           php->ncpM = 5;  // NPcoef
                       if( php->ncpM >= 8 )
                           php->ncpM = 8;
                       // php->nscN = 0;
                       php->PphC = PH_AQUEL;
                       break;
      case SM_AQELVIS:  // built-in ELVIS model for aqueous electrolytes
                       php->nscM = 8;                       // NP_DC  = cols of aDCc (rows of aDCc = NComp)
                       php->npxM = 2;                       // MaxOrd = cols of aIPx
                       if( php->ncpN < 1 )                  // NPar   = rows of aIPx and aIPc
                           php->ncpN = 1;
                       if( php->ncpN > (php->nDC*php->nDC) )
                           php->ncpN = (php->nDC*php->nDC);
                       php->ncpM = 8;                       // NPcoef = rows of aIPc
                       php->PphC = PH_AQUEL;
                       break;
      default:  // other models
                       break;
   }
}

// Setting up the default parameters depending on the
// built-in kinetics/metastability model (from TSolMod class)
//
void TPhase::Set_KinMet_Phase_coef()
{

    switch(php->kin_t[KinProCode])
    {   //KinProCode
        case KM_PRO_MWR:  // = 'M' Kinetics of generic dissolution/precipitation (no uptake, ionex, adsorption)
            php->PumpCon == S_OFF;
            break;
        case KM_PRO_UPT:  // = 'U' Kinetics of uptake/entrapment (of minor/trace element) into solid solution
            php->PumpCon = S_ON;
            break;
        case KM_PRO_IEX:  // = 'X' Kinetics of ion exchange (clays, C-S-H, zeolites, ...)

            break;
        case KM_PRO_ADS:  // = 'A' Kinetics of adsorption (on MWI), redox

            break;
        case KM_PRO_NUPR: // = 'P' Kinetics of nucleation and precipitation

            break;
        default:  // other models
            break;
    }

    switch(php->kin_t[KinModCode])
    {
    //KinModCode
        case KM_MOD_TST: // = 'T' Generic TST dissolution/precipitation model following Schott ea 2012
            php->PrpCon = S_ON;
            php->PapCon = S_ON;
            if( php->nPRk < 1 || php->nPRk > 12 )  // bugfix 29.07.13 by DK, report by BT
                php->nPRk = 2;
//            php->nSkr = 1;
            php->nrpC = 14;
            php->naptC = 1;
//            php->nFaces = 1;
//            php->nAscC = 4;
            break;
        case KM_MOD_PAL: // = 'P' Dissolution/precipitation model of the form (Palandri 2004)
            php->PrpCon = S_ON;
            php->PapCon = S_ON;
            if( php->nPRk < 1 || php->nPRk > 12 )  // bugfix 29.07.13 by DK, report by BT
                php->nPRk = 3;
//            php->nSkr = 2;
            php->nrpC = 14;
            php->naptC = 1;
            php->nFaces = 1;
//            php->nAscC = 4;
            break;
        case KM_MOD_WOL: // = 'W' Carbonate growth model following (Wolthers 2012)
            php->PrpCon = S_ON;
            php->PapCon = S_ON;
            php->nPRk = 1;
            php->nSkr = 2;
            php->nrpC = 14;
            php->naptC = 1;
            php->nFaces = 1;
//            php->nAscC = 4;
            break;
        case KM_MOD_NUGR: // = 'U' Mineral nucleation and growth model with nuclei/particle size distr. (TBD)

            break;
        default:  // other models
            break;
    }

    switch(php->kin_t[KinSorpCode])
    {

    //KinSorpCode
        case KM_UPT_ENTRAP: // = 'E' Unified entrapment model (Thien,Kulik,Curti 2012)
            php->PumpCon = S_ON;
            php->numpC = 6;
            break;
        case KM_UPT_UPDP:   // = 'M' DePaolo (2011) uptake kinetics model
            php->PumpCon = S_ON;
            php->numpC = 6;
            break;
        case KM_UPT_SEMO:   // = 'G' Growth (surface) entrapment model (Watson 2004)
            php->PumpCon = S_ON;
            php->numpC = 6;
            break;
        case KM_IEX_FAST:   // = 'F' Fast ion exchange kinetics (e.g. montmorillonite, CSH)

            break;
        case KM_IEX_SLOW:   // = 'L' Slow ion exchange kinetics (e.g. illite, zeolites)

            break;
        case KM_ADS_INHIB:  // = 'I' Adsorption inhibition

            break;
        case KM_NUCL_SSMP:  // = 'P' Solid solution nucleation model (Prieto 2013)

            break;
        default:  // other models
            break;
    }

    switch(php->kin_t[KinLinkCode])
    {   //KinLinkCode
        case KM_LNK_SURF: // = 'S' Link to (fraction of) solid substrate surface
        case KM_LNK_PVOL: // = 'P' Link to (fraction of) solid substrate (pore) volume
        case KM_LNK_MASS: // = 'M' Link to (fraction of) solid substrate mass
        if( php->nlPc < 1 || php->nlPc > 4 )
            php->nlPc = 4;
            break;
        default:  // other models
            break;
    }

    switch(php->kin_t[KinSizedCode])
    {   //KinSizedCode
        case KM_SIZED_ETM: // = 'T',   ///  Empirical f(time) cubic polynomial f = a + bt +ct^2 + dt^3 (default)
        case KM_SIZED_ESI: // = 'S',   ///  Empirical f(lgSI) cubic polynomial f = a + bt +ct^2 + dt^3
        case KM_SIZED_ESA: // = 'A',   ///  Empirical f(sarea-change) cubic polynomial f = a + bt +ct^2 + dt^3
        case KM_SIZED_EVOL: // = 'V',  ///  Empirical f(volume-change) cubic polynomial f = a + bt +ct^2 + dt^3
        case KM_SIZED_MASS: // = 'M',  ///  Empirical f(mass-change) cubic polynomial f = a + bt +ct^2 + dt^3
        case KM_SIZED_MOL: //  = 'X',   ///  Empirical f(amount-change) cubic polynomial f = a + bt +ct^2 + dt^3
            php->nAscC = 4;
            break;
        case KM_SIZED_UNI: // = 'U'	Uniform particle/pore size distribution
        case KM_SIZED_BIN: // = 'B'	Binodal particle/pore size distribution
        case KM_SIZED_FUN: // = 'F' Empirical distribution function
            break;
        default:  // other models
            break;
    }

    switch(php->kin_t[KinResCode])
    {
        case KM_RES_SURF_N: // = 'A',   surface-scaled rate constant (k in mol/m2/s), default
        case KM_RES_SURF_M: // = 'M',   surface-scaled rate constant (k in kg/m2/s)
        case KM_RES_PVS_N:  //  = 'V',   pore-volume-scaled rate constant (k in mol/m3/s)
        case KM_RES_PVS_M:  //  = 'W',   pore-volume-scaled rate constant (k in kg/m3/s)
        case KM_RES_ABS_N:  //  = 'F',   absolute (unscaled) rate constant (k in mol/s)
        case KM_RES_ABS_M:  //  = 'G',   absolute (unscaled) rate constant (k in kg/s)
        case KM_LIN_RATE:   //   = 'L'    linear growth/dissolution rate constant (v in m/s)
        default:
        break;
    }
}

void TPhase::Set_SorpMod_Phase_coef()
{
    // new
}


// Build ReacDC/DComp keys (aDclist) to be included into the Phase
void TPhase::makeReacDCompList( const char *caption, vector<string>& aDclist,
                   short& nDC,  char (*SM)[DC_RKLEN], char * DCS, bool bNsuT)
{
    int i, aNsuT = 0;
    string pkeyrd;
    vector<string> aDclist_old;

    aDclist.clear();

    //REACDC&DCOMP  keypart
    rt[RT_REACDC].MakeKey( RT_PHASE, pkeyrd, K_ACT, 0, K_ANY, K_ANY, K_ANY, K_END );
   if( pkeyrd[1] != ':')
       pkeyrd[1] = '*';
   if( bNsuT && php->NsuT > 0 )  // template for adsorption
    {   pkeyrd[0] = CP_SSPC;  // added by KD 25.10.2004
        aNsuT = php->NsuT;
    }

    if( nDC && SM )
    {
      // Build old selections DCOMP and REACDC
       aDclist_old.clear();
       for( i=0; i<nDC; i++ )
       {
          string key_dr = string( SM[i], 0, DC_RKLEN );
          if( DCS[i] == SRC_DCOMP )
          {
            rt[RT_DCOMP].SetKey( key_dr.c_str() );
            rt[RT_DCOMP].SetFldKey( 3, "*" );
            key_dr  = string(1, DCS[i]);
            key_dr += ' ';
            key_dr += rt[RT_DCOMP].UnpackKey();
          }
          else
            if( DCS[i] == SRC_REACDC )
            {
              rt[RT_REACDC].SetKey( key_dr.c_str() );
              rt[RT_REACDC].SetFldKey( 3, "*" );
              key_dr  = string(1, DCS[i]);
              key_dr += ' ';
              key_dr += rt[RT_REACDC].UnpackKey();
            }
         aDclist_old.push_back( key_dr );
      }
    }

AGAINRC:
     aDclist = vfRDMultiKeysSet( window(), caption,
                  pkeyrd.c_str(), aDclist_old, aNsuT  );

     if( aDclist.size() < 1 )
     {
        switch ( vfQuestion3(window(), GetName(),
        "W08PHrem: Number of selected ReacDC/DComp keys < 1.\n"
        " Mark again, proceed without ReacDC/DComp or Cancel?",
        "&Repeat", "&Proceed"))
        {
          case VF3_1:
            goto AGAINRC;
            ;
          case VF3_2:
            break;
          case VF3_3:  Error( GetName(),
             "E09PHrem: No ReacDC/DComp records selected into Phase...");
        }
      }
      nDC = (short)(aDclist.size());
}

// Build Phase keys (aPhlist) to be included into the Phase
void TPhase::makePhaseList( const char *caption, vector<string>& aPhlist )
{
    string pkeyrd = "*:*:*:*:*:";
    vector<string> aPhlist_old;

    aPhlist.clear();
    if( php->nlPh && php->lPh )
    {
      // Build old selections DCOMP and REACDC
       aPhlist_old.clear();
       for(int i=0; i<php->nlPh; i++ )
       {
          string key_dr = string( php->lPh[i], 0, PH_RKLEN );
          aPhlist_old.push_back( key_dr );
       }
    }
AGAINRC:
     aPhlist = vfMultiKeysSet( window(), caption, RT_PHASE, pkeyrd.c_str(), aPhlist_old  );
     if( aPhlist.size() < 1 )
     {
        switch ( vfQuestion3(window(), GetName(),
        "W08PHrem: Number of selected Phase keys < 1.\n"
        " Mark again, proceed without Phase or Cancel?",
        "&Repeat", "&Proceed"))
        {
          case VF3_1:
            goto AGAINRC;
            ;
          case VF3_2:
            break;
          case VF3_3:  Error( GetName(),
             "E09PHrem: No Phase records selected into Phase...");
        }
      }
      php->nlPh = (short)(aPhlist.size());
 }

/*
// Build Phase keys (aIclist) to be included into the Phase
void TPhase::makeICompList( const char *caption, TCStringArray& aIclist )
{
    gstring ikeyrd = "*:*:*:";
    TCStringArray aIclist_old;

    aIclist.Clear();
    if( php->PumpCon )
    {
      // Build old selections DCOMP and REACDC
       aIclist_old.Clear();
       for(int j=0; j<php->nDC; j++ )
       {
          gstring key_dr = gstring( php->lICu[j], 0, IC_RKLEN );
          aIclist_old.Add( key_dr );
       }
    }
AGAINRC:
     aIclist = vfMultiKeysSet( window(), caption, RT_ICOMP, ikeyrd.c_str(), aIclist_old  );
     if( aIclist.GetCount() < 1 )
     {
        switch ( vfQuestion3(window(), GetName(),
        "W08PHrem: Number of selected IComp keys < 1.\n"
        " Mark again, proceed without ICs, or Cancel?",
        "&Repeat", "&Proceed"))
        {
          case VF3_1:
            goto AGAINRC;
            ;
          case VF3_2:
            break;
          case VF3_3:  Error( GetName(),
             "E09PHrem: No IComp records selected into Phase uptake kinetics model...");
        }
      }
//      php->nlICu = (short)(aIclist.GetCount());
 }
*/

// Load DC classes from records and set to DCC
void TPhase::LoadDCC()
{
    vstr dcn(MAXRKEYLEN);
    char Ctype;
    time_t crt;
    TDComp* aDC=(TDComp *)(aMod[RT_DCOMP]);
    TReacDC* aRDC=(TReacDC *)(aMod[RT_REACDC]);
    memset( dcn, 0, MAXRKEYLEN );

    for(int i=0; i<php->nDC; i++ )
    {

      php->DCS[i] = php->SM[i][DC_RKLEN-1]; // species class code
      php->SM[i][DC_RKLEN-1] = ' ';

      // Get key
       memcpy( dcn, php->SM[i], DC_RKLEN );
       dcn[DC_RKLEN]=0;
       Ctype = A_NUL;
      // Read record and extract data
      if( php->DCS[i] == SRC_DCOMP )
      {
         aDC->TryRecInp( dcn, crt, 0 );
         Ctype = aDC->dcp->PdcC;
      }
      else
        if( php->DCS[i] == SRC_REACDC )
        {
            aRDC->TryRecInp( dcn, crt, 0 );
            Ctype = aRDC->rcp->PreC;
        }

      // test model and types of components
      if( php->Pinternal1 == S_ON || php->DCC[i] == A_NUL || php->DCC[i] == '`')
      {
        if( Ctype == DC_SCP_CONDEN && php->nDC >=2 &&
            !( php->PphC == PH_AQUEL || php->PphC == PH_GASMIX || php->PphC == PH_SORPTION ))
           Ctype = DC_SOL_MAJOR;
         php->DCC[i] = Ctype;
      }
    }  // i
}


// Pre-proc. loop for SIT or Pitzer: determining number of cations and anion
// in aDclist
void TPhase::DetNumbCatAn(vector<string>& aDclist)
{
  short nCat = 0, nAn = 0, nNs = 0;

 if( php->PphC == PH_AQUEL &&
     ( php->sol_t[SPHAS_TYP] == SM_AQSIT || php->sol_t[SPHAS_TYP] == SM_AQPITZ ))
 {
    int pos;
    string spName;
    for(int i=0; i<php->nDC/*-1*/; i++ ) // BugFix SD 26/11/2010  different number of neitral species
    {
      spName = string( aDclist[i], MAXSYMB+MAXDRGROUP+2, MAXDCNAME);

      strip(spName);
      pos = spName.length()-1;
      while( pos>0 && spName[pos] <=  '9' && spName[pos] >= '0' )
          pos--;
      switch( spName[pos] )
      {
        case '-': nAn++;
                  break;
        case '+': nCat++;
                  break;
        case '@': nNs++;   // neutral species except H2O
                  break;
        default:
                  break;
      }
    }

    if( nCat <=0 || nCat >= php->nDC || nAn <=0 || nCat >= php->nDC )
         Error( GetName(), "E39PHrem: No cations or no anions - SIT model cannot be applied...");
     php->nCat = nCat;
     php->nAn = nAn;
     php->nNs = nNs;
  }
}


// Assembling indices and name lists for cations and anions
void
TPhase::MakeCatAnLists( bool WorkCount, bool WorkAlloc, bool FillOut )
{
   int pos;
   short i, iCat=0, iAn=0, iNs=0, nAn, nCat, nNs;
   string spName;

   if( WorkCount )
   {   // pre-proc. loop: determining number of cations, anions and neutral species
      nAn=0, nCat=0, nNs=0;
      for( i=0; i<php->nDC/*-1*/; i++ ) // BugFix SD 26/11/2010  different number of neitral species
      {
         spName = string( php->SM[i], MAXSYMB+MAXDRGROUP, MAXDCNAME);
         strip(spName);
         pos = spName.length()-1;
         while( pos>0 && spName[pos] <=  '9' && spName[pos] >= '0' )
             pos--;
         switch( spName[pos] )
         {
           case '-': nAn++;
                     break;
           case '+': nCat++;
                     break;
           case '@': nNs++;  // H2O@ is not included
                     break;
           default:
                     break;
         }
      }
      if( nCat <=0 || nCat >= php->nDC || nAn <=0 || nCat >= php->nDC )
           Error( GetName(),
              "E39PHrem: No cations or no anions - SIT model cannot be applied...");
      php->nCat = nCat;
      php->nAn = nAn;
      php->nNs = nNs;
   }

   if( WorkAlloc )
   {
      if( php->Ppnc == S_ON
    		  && (php->sol_t[SPHAS_TYP] == SM_AQSIT || php->sol_t[SPHAS_TYP] == SM_AQPITZ) )
      {
         php->lsCat = (char (*)[MAXDCNAME])aObj[ o_ph_w_lsc ].Alloc(
                          php->nCat, 1, MAXDCNAME );
         php->lsAn  = (char (*)[MAXDCNAME])aObj[ o_ph_w_lsa ].Alloc(
                          php->nAn, 1, MAXDCNAME );
         php->nxCat = (short *)aObj[ o_ph_w_nxc ].Alloc( php->nCat, 1, I_);
         php->nxAn  = (short *)aObj[ o_ph_w_nxa ].Alloc( php->nAn, 1, I_);
         if( php->nNs )
         {
             php->lsNs  = (char (*)[MAXDCNAME])aObj[ o_ph_w_lsn ].Alloc(
                              php->nNs, 1, MAXDCNAME );
             php->nxNs  = (short *)aObj[ o_ph_w_nxn ].Alloc( php->nNs, 1, I_);
         }
      }
      else {

        php->nSub =  0;
        php->lsCat = (char (*)[MAXDCNAME])aObj[ o_ph_w_lsc ].Free();
        php->lsAn =  (char (*)[MAXDCNAME])aObj[ o_ph_w_lsa ].Free();
        php->lsNs =  (char (*)[MAXDCNAME])aObj[ o_ph_w_lsn ].Free();
        php->nxCat = (short *)aObj[ o_ph_w_nxc ].Free();
        php->nxAn =  (short *)aObj[ o_ph_w_nxa ].Free();
        php->nxNs =  (short *)aObj[ o_ph_w_nxn ].Free();

      }
   }

   if( FillOut )
   {
     for( i=0; i<php->nDC/*-1*/; i++ ) // BugFix SD 26/11/2010  different number of neitral species
     { // Determining if cation or anion
       spName = string( php->SM[i], MAXSYMB+MAXDRGROUP, MAXDCNAME);
       strip(spName);
       pos = spName.length()-1;
       while( pos>0 && spName[pos] <= '9' && spName[pos] >= '0' )
          pos--;
       switch( spName[pos] )
       {
          case '-': memcpy( php->lsAn[iAn], php->SM[i]+MAXSYMB+MAXDRGROUP, MAXDCNAME );
                    php->nxAn[iAn++] = i;
                    break;
          case '+': memcpy( php->lsCat[iCat], php->SM[i]+MAXSYMB+MAXDRGROUP, MAXDCNAME );
                    php->nxCat[iCat++] = i;
                    break;
          case '@': memcpy( php->lsNs[iNs], php->SM[i]+MAXSYMB+MAXDRGROUP, MAXDCNAME );
				    php->nxNs[iNs++] = i;
          default:
                  continue;
        }
     }
     if( iCat != php->nCat || iAn != php->nAn || iNs != php->nNs )
       Error( GetName(),
        "E38PHrem: Mismatch in the number of cations, anions or neutral species...");
   }
}

const int MAXMOIETY = 60;

void TPhase::MakeSublatticeLists( const vector<string>& form_array  )
{
    TFormula form;
    vector<MOITERM> moit_;
    int ii, jj;
    int i1, nSites =0;
    int nMoi =  0;

    // allocate default buffer and setup default values
    moiety_new( form_array.size(), MAXMOIETY, true );

    for( jj=0; jj<form_array.size(); jj++ )
    {
      nSites =  form.BuildMoiety(form_array[jj].c_str(), moit_);
      if( jj== 0)
        php->nSub = nSites;
      else
        ErrorIf( php->nSub!=nSites, "MakeSublatticeLists", "Different number of sites" );

      for( ii=0; ii<moit_.size(); ii++ )
      {
         for( i1=0; i1<nMoi; i1++)
         {
           if( moit_[ii].name == string(php->lsMoi[i1]) )
              break;
         }
         if( i1 == nMoi )
         {
             strncpy( php->lsMoi[ i1], moit_[ii].name.c_str(), MAXDCNAME);
             nMoi++;
             ErrorIf( nMoi == MAXMOIETY, "MakeSublatticeLists", "Too many moieties" );
         }
         php->nxSub[ jj*php->nMoi+i1] = moit_[ii].site;
         php->OcpN[ jj*php->nMoi+i1] = moit_[ii].nj;
      }
    }

   // re allocate memory
   moiety_new( form_array.size(), nMoi, false );


   // setup data to formula list
   if( php->nMoi >0 )
     for( jj=0; jj<form_array.size(); jj++ )
        strncpy( php->lsForm[jj], form_array[jj].c_str(), MAXFORMULA);

}

#define s(i,j) php->scoef[(j)+(i)*nsc]
//#define sit(i,j) php->pnc[(j)+(i)*nAn] // not used SD 11/12/2008


//Recalc record structure
void
TPhase::CalcPhaseRecord(  bool getDCC  )
{
    int  i, pa0=0, Kielland=0;
    short nsc=0;
    char dcn[MAXRKEYLEN];
    char Ctype;
    float a0=0., bp=0., Z=0., cN=0., Fi=0.;
    time_t crt;
    vector<string> form_array;

    TDComp* aDC=(TDComp *)(aMod[RT_DCOMP]);
    TReacDC* aRDC=(TReacDC *)(aMod[RT_REACDC]);
    aDC->ods_link(0);
    aRDC->ods_link(0);

    /* Question of get effective ion radii of aqueous species */
    if( (php->PphC == PH_AQUEL) && php->scoef )
    {
        if( pVisor->ProfileMode == true || vfQuestion(window(), GetName(),
        		"Parameters of aqueous species: Collect from DComp/ReacDC records?"))
        {
            pa0 = 1;
            Kielland = 0;
        }
        else
        {
            switch ( vfQuestion3(window(), GetName(),
            "W10PHrem: Shall I set default values automatically:\n"
            "from (Kielland 1936), from (Helgeson ea 1981) or \n"
            "Not collect and not set automatically? ",
                 "&Kielland", "&Helgeson" ) )
            {
            case VF3_1:
                Kielland = 1;
                pa0 = 0;
                break;
            case VF3_2:
                Kielland = 1;
                pa0 = 1;
                break;  /* A Pitzer ? */
            case VF3_3:
                pa0 = 0;
                Kielland = 0;
                break;
            }
        }
    }

    //=====================================================================================
    // main loop for records
    nsc = php->nscM;
    int kx, mcex;

    memset( dcn, 0, MAXRKEYLEN );
    for( i=0; i<php->nDC; i++ )
    {  /*Get key */
        memcpy( dcn, php->SM[i], DC_RKLEN );
        dcn[DC_RKLEN]=0;
        Ctype = A_NUL;
        /* Read record and extract data */
        if( php->DCS[i] == SRC_DCOMP )
        {
           aDC->TryRecInp( dcn, crt, 0 );
           a0 = aDC->dcp->Der;
           bp = aDC->dcp->DerB;
           Z = aDC->dcp->Zz;
           Ctype = aDC->dcp->PdcC;
           form_array.push_back( string(aDC->dcp->form,0,MAXFORMULA));
           cN = aDC->dcp->Comp;
           Fi = aDC->dcp->Expa;

           if( ( php->PphC == PH_FLUID || php->PphC == PH_LIQUID ||
                 php->PphC == PH_GASMIX ) && php->scoef )
           {
               // Collecting coefficients of EoS for fluids and gases
               if( aDC->dcp->Cemp && !aDC->dcp->CPg )
               {    // CG fluid model coefficients
                  mcex = min( aDC->dcp->Nemp, nsc );
                  for( kx=0; kx< nsc; kx++ )
                  {
                    if( kx < mcex ) // Copying only what is possible
                      s(i,kx) = aDC->dcp->Cemp[kx];
                    else
                      s(i,kx) = 0.;
                  }
               }
               if( aDC->dcp->CPg && !aDC->dcp->Cemp )
               {
                  mcex = min( MAXCRITPARAM, (int)nsc );  // PRSV, SRK, PR78, CORK and STP model coefficients
                  for( kx=0; kx< nsc; kx++ )
                  {
                    if( kx < mcex ) // Copying only what is possible
                      s(i,kx) = aDC->dcp->CPg[kx];
                    else
                      s(i,kx) = 0.;
                  }
               }
               if( !aDC->dcp->CPg && !aDC->dcp->Cemp )
               { // Error - no array in DComp or ReacDC!
                  for( kx=0; kx< nsc; kx++ )
                      s(i,kx) = 0.0;
               }
           }
        }
        else
            if( php->DCS[i] == SRC_REACDC )
            {
                aRDC->TryRecInp( dcn, crt, 0 );
                a0 = aRDC->rcp->Der;
                bp = aRDC->rcp->DerB;
                Z = aRDC->rcp->Zz;
                Ctype = aRDC->rcp->PreC;
                form_array.push_back( string(aRDC->rcp->form,0,MAXFORMULA));
                cN = aRDC->rcp->Comp;
                Fi = aRDC->rcp->Expa;
            }

        /* test model and types of components */
        if( php->DCC[i] == A_NUL || php->DCC[i] == '`')
        {
            if( Ctype == DC_SCP_CONDEN && php->nDC >=2 &&
                !( php->PphC == PH_AQUEL || php->PphC == PH_GASMIX || php->PphC == PH_SORPTION ))
               Ctype = DC_SOL_MAJOR;
            php->DCC[i] = Ctype;
        }

        if( php->PphC == PH_AQUEL && php->scoef )
        {
            Z = fabs(Z);
            if( pa0 && !Kielland )
            {
                s(i,0) = a0;
                if( nsc > 1 ) s(i,1) = bp;
            }
            if( Kielland && !pa0 )
            {
                if( Z < 0.01 ) s(i,0) = 0;
                if( fabs(Z-1.0) < 0.01 ) s(i,0) = 3.5;
                if( fabs(Z-2.0) < 0.01 ) s(i,0) = 5.0;
                if( Z > 2.99 ) s(i,0) = 9.0; /* Make a table ! */
            }
            if( Kielland && pa0 )  /* Helgeson */
            { if( Z < 0.01 ) s(i,0) = 0;
                if( Z > 0.99 ) s(i,0) = 3.84;
                /* do it by HKF ! */    //  3.72 !
            }
        }

        if( php->PphC == PH_SORPTION || php->PphC == PH_POLYEL )
        {
            // Transferring data into MaSdj array   KD 25.10.2004
            // Now a0 and b0 contain CD (charge distribution) for 0 and beta planes, resp.
                    if(  php->MaSdj )  // && php->NR1 == DFCN
                    {
                       if( fabs( cN ) < 1e-20 )
                          cN = 0.0;
                       php->MaSdj[i][PI_P2] = cN;  // Dentateness or Frumkin isotherm parameter cN
                       if( fabs( Fi ) < 1e-20 )
                          Fi = 0.0;
                       php->MaSdj[i][PI_P1] = Fi;  // Frumkin isotherm parameter Alpha-Fi
            // EDL CD parameters
                       if( fabs( a0 ) < 1e-20 )
                           a0 = Z;
                       if( fabs( bp ) < 1e-20 )
                           bp = 0;
                       if( !(php->DCC[i] == DC_SUR_CARRIER ||
                             php->DCC[i] == DC_SUR_MINAL ||
                             php->DCC[i] == DC_PEL_CARRIER) )
                       {
                          php->MaSdj[i][PI_CD0] = a0;
                          php->MaSdj[i][PI_CDB] = bp;
                          php->MaSdj[i][PI_P1] = Fi;
                          php->MaSdj[i][PI_P2] = cN;
                       }
                       else
                       { //  Sorbent has no data for surface charges
                          php->MaSdj[i][PI_CD0] = 0;
                          php->MaSdj[i][PI_CDB] = 0;
                          php->MaSdj[i][PI_P1] = 0;
                          php->MaSdj[i][PI_P2] = 0;
                       }
                    }
        }


    }  // i

    //=====================================================================================

    if( php->PphC == PH_SINCOND || php->PphC == PH_SINDIS
            || php->PphC == PH_LIQUID || php->PphC == PH_SIMELT )   // added DK 29.03.2012
        MakeSublatticeLists( form_array  );


    if( php->Asur > 1. )
    {
        if( php->nDC == 1 )
        {
            php->PphC = PH_SINDIS;
        }
        else if( php->NsuT >= 1. )
        {
            php->PphC = PH_SORPTION;
        }
    }

}


//-------------------------------------------------------------------------
// called from Project - extended by KD on 16.06.03 to add CG EoS
// Re-written (for AutoPhaseWizard) by KD on 31.07.03
void TPhase::newAqGasPhase( const char * akey, const char *gkey, int file,
   const char amod, const char gmod, float apar[8], /*float gpar[4],*/
   bool useLst )
{
    vector<string> lst;
//    TProfil *aPa=(TProfil *)(aMod[RT_PARAM]);
    const char *part;
    char nbuf[MAXFORMULA*2], neutbuf[16], H2Obuf[16], tempdbuf[16];
    string Name = "Auto-set ";

//  Setup of aqueous phase
    if( !apar[2] )
    	strcpy(neutbuf, "1.0");
    else strcpy(neutbuf, "b_q*IS");
    if( !apar[3] )
    	strcpy(H2Obuf, "1.0");
    else strcpy(H2Obuf, "calculate");
    if( !apar[4] )
        strcpy( tempdbuf, "0");
    else sprintf( tempdbuf, "%c", (char)(apar[4]+'0'));
// cout << "newAqGasPhase: " << amod << endl;

    switch( amod )
    {
       case 'N': // No aqueous phase
       case 'U': // User-selected aqueous phase
                 goto MAKE_GAS_PHASE;
       case 'D': // Davies model (no a0 and bg required)
                memcpy( php->sol_t, "DNNSNN", 6 );
                // memcpy( &php->PphC, "a-----", 6 );
                memcpy( &php->PphC, "a+----", 6 );  // changed 10.07.2008 DK
                php->ncpN = 2;
                php->ncpM = 4;
                php->nscM = 0;
                php->npxM = 0;
                apar[0] = 0.0;
                apar[1] = 0.0;
					Name += "ion-association model, Davies equation";
					sprintf( nbuf, "Parameters: gam_neut= %s; gam_H2O= %s", neutbuf, H2Obuf );
                break;
       case 'H': // EDH model with common bg and common a0 (Helgeson)
                memcpy( php->sol_t, "HNNSNN", 6 );
                memcpy( &php->PphC, "a+----", 6 );
                php->ncpN = 2;
                php->ncpM = 4;
                php->nscM = 0;
                php->npxM = 0;
					Name += "ion-association model, EDH(H) equation, common ion size";
					sprintf( nbuf, ": b_gamma= %-5.3f, T_dep= %s; a_size= %-5.3f; gam_neut= %s, gam_H2O= %s ",
							apar[0], tempdbuf, apar[1], neutbuf, H2Obuf );
                break;
       case 'Y': // EDH model with common bg and common a0 (Shvarov)
                memcpy( php->sol_t, "YNNSNN", 6 );
                memcpy( &php->PphC, "a++---", 6 );
                php->ncpN = 2;
                php->ncpM = 4;
                php->nscM = 2; // correction by TW from trunk r.2443
                php->npxM = 0;
					Name += "ion-association model, EDH(S) equation, common ion size";
					sprintf( nbuf, ": b_gamma= %-5.3f, T_dep= %s; a_size= %-5.3f; gam_neut= %s, gam_H2O= %s ",
							apar[0], tempdbuf, apar[1], neutbuf, H2Obuf );
                break;
       case '3': // EDH model with individual (Kielland) a0 and common bg (Karpov)
                memcpy( php->sol_t, "3NNSNN", 6 );
                memcpy( &php->PphC, "a++---", 6 );
                php->ncpN = 2;
                php->ncpM = 4;
                php->nscM = 2;
                php->npxM = 0;
                apar[1] = 0.0;
					Name += "ion-association model, EDH(K) equation, individual ion sizes";
					sprintf( nbuf, ": b_gamma= %-5.3f, T_dep= %s; a_size=specific; gam_neut= %s; gam_H2O= %s ",
							apar[0], tempdbuf, neutbuf, H2Obuf );
                break;
       case '2': // DH model with individual (Kielland) a0 and optional bg for neutral species
                memcpy( php->sol_t, "2NNSNN", 6 );
                memcpy( &php->PphC, "a++---", 6 );
                php->ncpN = 2;
                php->ncpM = 4;
                php->nscM = 2;
                php->npxM = 0;
                apar[0] = 0.0;
                apar[1] = 0.0;
					Name += "ion-association model, DH equation, individual ion sizes";
					sprintf( nbuf, ": b_gamma= %-5.3f; a_size=specific; gam_neut= %s; gam_H2O= %s ",
							apar[0], neutbuf, H2Obuf );
                break;
       case '1': // DH limiting law (no a0 and bg required)
                memcpy( php->sol_t, "1NNSNN", 6 );
                memcpy( &php->PphC, "a+----", 6 );  // changed 30.05.2009 (TW)
                php->ncpN = 2;
                php->ncpM = 4;
                php->nscM = 0;
                php->npxM = 0;
                apar[0] = 0.0;
                apar[1] = 0.0;
                // apar[2] = 0.0;
					Name += "ion-association model, Debye-Hueckel limiting law";
					sprintf( nbuf, "Parameters: gam_H2O= %s ", H2Obuf );
                break;
       default: // Unrecognized code - error message ?
       case 'S': // SIT - under testing
                 goto MAKE_GAS_PHASE;
    }
    memcpy( php->name, Name.c_str(), MAXFORMULA );
    memcpy( php->notes, nbuf, MAXFORMULA );
    part = "a:*:*:*:";

    // Call assembling of the aqueous phase
    AssemblePhase( akey, part, apar, file, useLst, lst, 6 );

MAKE_GAS_PHASE:
    Name = "Auto-set ";
// cout << "newAqGasPhase Gas: " << gmod << endl;

    switch( gmod )
    {
      case 'N': // No gas/fluid phase in the system
      case 'U': // User - selected from database
                goto DONE;
      case 'I':  // Ideal gas mixture (default)
              memcpy( php->sol_t, "INNINN", 6 );
              memcpy( &php->PphC, "g-----", 6 );
              php->ncpN = 0;
              php->ncpM = 0;
              php->nscM = 0;
              php->npxM = 0;
              Name += "Ideal mixture of ideal or real gases";
              strncpy( php->name, Name.c_str(), MAXFORMULA );
              memcpy( php->notes, "Applicable only at low P and elevated T", MAXFORMULA );
              break;
      case 'F':  // CG fluid EoS model
              memcpy( php->sol_t, "FNNSNN", 6 );
              memcpy( &php->PphC, "f-+---", 6 );
              php->ncpN = 0;
              php->ncpM = 0;
              php->nscM = 12; // last changed 12.12.2008 (TW)
              php->npxM = 0;
              Name += "Churakov-Gottschalk (CG) EoS model";
              strncpy( php->name, Name.c_str(), MAXFORMULA );
              memcpy( php->notes,
                          "Applicable at high P and moderate T", MAXFORMULA );
              break;
      case 'P':  // PRSV fluid EoS model
              memcpy( php->sol_t, "PNNSNN", 6 );
              memcpy( &php->PphC, "f++---", 6 );
              if( php->ncpN < 1 ) // NPar
            	  php->ncpN = 1;
              if( php->ncpN > (php->nDC*(php->nDC-1)/2) )
            	  php->ncpN = (php->nDC*(php->nDC-1)/2);
              // php->ncpN = 0;
              php->ncpM = 2;
              php->nscM = 7;
              php->npxM = 2;
              Name += "Peng-Robinson-Stryjek-Vera (PRSV) EoS model";
              strncpy( php->name, Name.c_str(), MAXFORMULA );
              memcpy( php->notes,
                          "Applicable at moderate P and moderate T" , MAXFORMULA);
              break;
      case 'E':  // SRK fluid EoS model
              memcpy( php->sol_t, "ENNSNN", 6 );
              memcpy( &php->PphC, "f++---", 6 );
              if( php->ncpN < 1 ) // NPar
            	  php->ncpN = 1;
              if( php->ncpN > (php->nDC*(php->nDC-1)/2) )
            	  php->ncpN = (php->nDC*(php->nDC-1)/2);
              // php->ncpN = 0;
              php->ncpM = 2;
              php->nscM = 7;
              php->npxM = 2;
              Name += "Soave-Redlich-Kwong (SRK) EoS model";
              strncpy( php->name, Name.c_str(), MAXFORMULA );
              memcpy( php->notes,
                          "Applicable at moderate P and moderate T", MAXFORMULA );
              break;
      case '7':  // PR78 fluid EoS model
              memcpy( php->sol_t, "7NNSNN", 6 );
              memcpy( &php->PphC, "f++---", 6 );
              if( php->ncpN < 1 ) // NPar
            	  php->ncpN = 1;
              if( php->ncpN > (php->nDC*(php->nDC-1)/2) )
            	  php->ncpN = (php->nDC*(php->nDC-1)/2);
              // php->ncpN = 0;
              php->ncpM = 2;
              php->nscM = 7;
              php->npxM = 2;
              Name += "Peng-Rosinson (PR78) EoS model";
              strncpy( php->name, Name.c_str(), MAXFORMULA );
              memcpy( php->notes,
                          "Applicable at moderate P and moderate T", MAXFORMULA );
              break;
      default:  // unrecognized code
              goto DONE;
    }
    part = "g:*:*:*:";
    if( gkey[0] == 'f' )
        part = "f:*:*:*:";

    // Assembling gas phase
    AssemblePhase( gkey, part, 0, file, useLst, lst, 0 );

    // Do sometning else here?
    DONE:
    contentsChanged = false;
}


// Assembling the phase (automatically generated aq or gas/fluid)
// Separated by KD on 31.07.03
void TPhase::AssemblePhase( const char* key, const char* part, float *param,
                            int file, bool useLst, const vector<string>& lst, int Npar )
{
    TProfil *aPa=(TProfil *)(aMod[RT_PARAM]);

// Initializing
memcpy( php->kin_t, "NNNNNNNN", 8 );
    php->Nsd = 0;
    php->nDC = 0;
    php->NsuT = php->nMoi = 0;
    php->Asur =   php->Sigma0 =  php->SigmaG =   php->R0p =
                  php->h0p =  php->Eps =  php->Cond =  php->Rsp1 =  0.;

    //Get lists
     vector<string> aDclist;
     vector<string> aRclist;
     int Ndc, Nrc, iic=0, i;

    if( useLst == false )
    {
        //DCOMP key list
        Ndc = rt[RT_DCOMP].GetKeyList( part, aDclist );
        if( part[0] == 'g' ) // Added Sveta 28/10/02 for plasma
        {
          vector<string> aDclist1;
          Ndc += rt[RT_DCOMP].GetKeyList( "p:*:*:*:", aDclist1 );
          for(int kk=0; kk<aDclist1.size(); kk++)
          {
            aDclist.push_back(aDclist1[kk]);
          }
        }
        vector<string>::iterator it;
        it = aDclist.begin();
        while( it< aDclist.end() )
        {
            if( !memcmp( it->c_str()+MAXSYMB+MAXDRGROUP, "E-", 2 ) ||
                    !memcmp( it->c_str()+MAXSYMB+MAXDRGROUP, "e-", 2 ) )
            {
                it = aDclist.erase(it);
                Ndc--;
            }
            else
                if( it>aDclist.begin() &&
                   !memcmp( it->c_str(), (it-1)->c_str(), DC_RKLEN-MAXSYMB ) )
                {
                    it = aDclist.erase(it);
                    Ndc--;
                }
                else it++;
        }
        //REACDC  key list
        Nrc = rt[RT_REACDC].GetKeyList( part, aRclist );
        if( part[0] == 'g' ) // Added Sveta 28/10/02 for plasma
        {
          vector<string> aDclist1;
          Nrc += rt[RT_REACDC].GetKeyList( "p:*:*:*:", aDclist1 );
          for(int kk=0; kk<aDclist1.size(); kk++)
          {
            aRclist.push_back(aDclist1[kk]);
          }
        }
        it = aRclist.begin();
        while( it< aRclist.end() )
        {
            if( it>aRclist.begin() &&
                   !memcmp( it->c_str(), (it-1)->c_str(), DC_RKLEN-MAXSYMB ) )
                {
                    it = aRclist.erase(it);
                    Nrc--;
                }
                else it++;
        }
        ErrorIf( Nrc<1&&Ndc<1,  /*key,*/  "AutoAssemblePhase:",
              " No DComp and ReacDC records found! ");
        php->nDC = (short)(Ndc + Nrc);
//        php->NR1 = (short)aRclist.GetCount();
        iic = aDclist.size();
    }
    else
        php->nDC = (short)lst.size(); // php->NR1 ?

    dyn_new(0);

    if( useLst == false )
    {   /* Get list of component : add aMcv and aMrv */
        for( i=0; i<php->nDC; i++ )
        {
            if( i < aDclist.size() )
            {
                memcpy( php->SM[i], aDclist[i].c_str(), DC_RKLEN );
                php->SM[i][DC_RKLEN-1] = SRC_DCOMP;
            }
            else
            {
                memcpy( php->SM[i], aRclist[i-iic].c_str(), DC_RKLEN );
                php->SM[i][DC_RKLEN-1] = SRC_REACDC;
            }
        }
        /* Sort list of components */
        if( php->nDC > 2 )
            qsort( php->SM[0], (size_t)php->nDC, DC_RKLEN, rkeycmp );
    }
    else
        for( i=0; i<php->nDC; i++ ) // , php->NR1=0
        {
//            if( lst[i].c_str()[DC_RKLEN-1] == SRC_REACDC)
//                php->NR1++;
            memcpy( php->SM[i], lst[i].c_str(), DC_RKLEN );
        }

    for( i=0; i<php->nDC; i++ )
    {
        php->DCS[i] = php->SM[i][DC_RKLEN-1]; /* source code */
        php->SM[i][DC_RKLEN-1] = ' ';
        php->DCC[i] = '`';
    }

    if( php->NsuT > 0 && php->PFsiT == S_REM )
    {  /* Setup of default values */
        for( i=0; i<php->NsuT; i++ )
        {
            if( !php->SCMC[i] || php->SCMC[i]==A_NUL )
                php->SCMC[i] = SC_CCM;  /* Default! */
            if( !php->MSDT[i][0] )
                php->MSDT[i][0] = aPa->pa.p.DNS;
            if( !php->MSDT[i][1] )
                php->MSDT[i][1] = 0.6022; /* 1/nm2; = 1 mkmol/m2 */
            if( !php->CapT[i][0] )
                php->CapT[i][0] = 1.0; /* A plane */
            if( !php->CapT[i][1] )
                php->CapT[i][1] = 0.2; /* B plane */
        }

        for( i=0; i<php->nDC; i++ )
        {   // Restoring defaults if zeros or null-character
            if( !php->SATC[i][SA_MCA] || php->SATC[i][SA_MCA] ==A_NUL )
                php->SATC[i][SA_MCA] = SAT_INDEF;
            if( !php->SATC[i][SA_EMX] || php->SATC[i][SA_EMX] ==A_NUL )
                php->SATC[i][SA_EMX] = CCA_VOL;  /* Default! */
            if( !php->SATC[i][SA_STX] || php->SATC[i][SA_STX] ==A_NUL )
                php->SATC[i][SA_STX] = CST_0;
            if( !php->SATC[i][SA_PLAX] || php->SATC[i][SA_EMX] ==A_NUL )
                php->SATC[i][SA_EMX] = SPL_0;  /* Default! */
            if( !php->SATC[i][SA_SITX] || php->SATC[i][SA_MCA] ==A_NUL )
                php->SATC[i][SA_MCA] = CSI_0;
            if( !php->SATC[i][SA_UNIT] || php->SATC[i][SA_EMX] ==A_NUL )
                php->SATC[i][SA_EMX] = SDU_N;  /* Default! */
        }
        php->PFsiT = S_ON;
    }
    // set up all 0 06/02/2007
    for( i=0; i<php->ncpN * php->ncpM; i++)
    	php->pnc[i] = 0.;

    for( i=0; i < min(Npar,(php->ncpN * php->ncpM)); i++ )
    	php->pnc[i] = param[i];

// Calculating the phase record and saving it to database
    CalcPhaseRecord( true );
    bool yesToAll=false;
    db->SaveRecordQuestion( key, file, yesToAll );
    // contentsChanged = false;
}

void TPhase::set_def_comments( bool clearall,
          const char* old_sol, const char *old_kin )
{
    int ii;
    char tbuf[100];

    if( php->Psco == S_ON )
      if( clearall || ( php->sol_t[SPHAS_TYP] != old_sol[SPHAS_TYP] ) ||
              ( php->sol_t[SGM_MODE] != old_sol[SGM_MODE] ) )
      {
//          for(ii=0; ii<php->nscM; ii++)   // problematic
//          {
//              sprintf( tbuf, "Int%d", ii+1 );
//              strncpy( php->dcpcl[ii], tbuf, MAXDCNAME );
//          }
          if( php->sol_t[SGM_MODE] == SM_STNGAM )
          {
              switch(php->sol_t[SPHAS_TYP])
              {
              case SM_OTHER:   // Customized hardcoded solid-solution models
                             break;
              case SM_BERMAN:   // Sublattice mixing model (multicomponent), site interactions
                   strncpy( php->dcpcl[0], "rc0", MAXDCNAME );
                   strncpy( php->dcpcl[1], "rc1 /T", MAXDCNAME );
                   strncpy( php->dcpcl[2], "rc2 *T^2", MAXDCNAME );
                  //               php->nscM = 1;
                  //          php->ncpM = 3;  // NPcoef
                             break;
              case SM_CEF:  //  = '$',    built-in multicomponent multisite solid-solution model (CALPHAD)
                  strncpy( php->dcpcl[0], "rc0", MAXDCNAME );
                  strncpy( php->dcpcl[1], "rc1 /T", MAXDCNAME );
                  strncpy( php->dcpcl[2], "rc2 *T^2", MAXDCNAME );
                            break;
              case SM_VANLAAR:   // Van Laar model (multicomponent)
                   //          php->ncpM = 3;  // NPcoef
                   strncpy( php->dcpcl[0], "fi", MAXDCNAME );
                   //          php->nscM = 1;
                             break;
              case SM_REGULAR:   // Regular model (multicomponent)
                    //         php->ncpM = 3;  // NPcoef
                             break;
              case SM_GUGGENM:   // Redlich-Kister model (multicomponent)
                         //    php->ncpM = 16;  // NPcoef
                             break;
             case SM_NRTLLIQ:   // NRTL liquid model (multicomponent), added 03.06.2008 (TW)
                         //    php->ncpM = 6;  // NPcoef
                             break;
             case SM_WILSLIQ:   // Wilson liquid model (multicomponent), added 09.06.2008 (TW)
                           //  php->ncpM = 4;  // NPcoef
                             break;
             case SM_REDKIS:   // Redlich-Kister model (binary)
                        //     php->ncpN = 1; php->ncpM = 3;
                             break;
             case SM_MARGB:  // Margules subregular model (binary)
                        //     php->ncpN = 2; php->ncpM = 3;
                             break;
             case SM_MARGT:  // Margules regular model (ternary)
                          //   php->ncpN = 4; php->ncpM = 3;
                             break;
             case SM_CGFLUID:  // Churakov-Gottschalk (CG) EoS
                    strncpy( php->dcpcl[0], "fi", MAXDCNAME );
                    strncpy( php->dcpcl[1], "fi", MAXDCNAME );
                    strncpy( php->dcpcl[2], "fi", MAXDCNAME );
                    strncpy( php->dcpcl[3], "fi", MAXDCNAME );
                    strncpy( php->dcpcl[4], "fi", MAXDCNAME );
                    strncpy( php->dcpcl[5], "fi", MAXDCNAME );
                    strncpy( php->dcpcl[6], "fi", MAXDCNAME );
                    strncpy( php->dcpcl[7], "fi", MAXDCNAME );
                    strncpy( php->dcpcl[8], "fi", MAXDCNAME );
                    strncpy( php->dcpcl[9], "fi", MAXDCNAME );
                    strncpy( php->dcpcl[10], "fi", MAXDCNAME );
                    strncpy( php->dcpcl[11], "fi", MAXDCNAME );
                          //   php->nscM = 12; // last changed 12.12.2008 (TW)
                             break;
             case SM_PRFLUID:  // Peng-Robinson-Stryjek-Vera (PRSV) EoS, one binary interaction parameter
                  strncpy( php->dcpcl[0], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[1], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[2], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[3], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[4], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[5], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[6], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[7], "fi", MAXDCNAME );
                  //             php->nscM = 7;
                        //     php->ncpM = 2;  // NPcoef
                             break;
             case SM_SRFLUID:  // Soave-Redlich-Kwong (SRK) EoS, one binary interaction parameter
                  strncpy( php->dcpcl[0], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[1], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[2], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[3], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[4], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[5], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[6], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[7], "fi", MAXDCNAME );
                  //              php->nscM = 7;
                        //       php->ncpM = 2;  // NPcoef
                             break;
             case SM_PR78FL:  // Peng-Robinson (PR78) EoS, one binary interaction parameter
                  strncpy( php->dcpcl[0], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[1], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[2], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[3], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[4], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[5], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[6], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[7], "fi", MAXDCNAME );
                  //              php->nscM = 7;
                        //     php->ncpM = 2;  // NPcoef
                             break;
             case SM_CORKFL:  // compensated Redlich-Kwong (CORK) EoS, one binary interaction parameter
                  strncpy( php->dcpcl[0], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[1], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[2], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[3], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[4], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[5], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[6], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[7], "fi", MAXDCNAME );
//               php->nscM = 7;
               //     php->ncpM = 1;  // NPcoef
                             break;
             case SM_STFLUID:  // Sterner-Pitzer (STP) EoS, one binary interaction parameter
                  strncpy( php->dcpcl[0], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[1], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[2], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[3], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[4], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[5], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[6], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[7], "fi", MAXDCNAME );
                  //              php->nscM = 7;
                             //    php->ncpM = 1;  // NPcoef
                             break;
             case SM_AQDAV:  // aqueous Davies
                           //  php->ncpM = 4; // changed 10.07.2008 DK
                             break;
             case SM_AQDH1:  // aqueous DH limiting law
                          //   php->ncpM = 4;
                             break;
             case SM_AQDH2:  // aqueous DH, individual a0, individual bg
                  strncpy( php->dcpcl[0], "a0", MAXDCNAME );
                  strncpy( php->dcpcl[1], "bg", MAXDCNAME );
                  // php->nscM = 2;
                          //   php->ncpM = 4;
                             break;
             case SM_AQDH3:  // aqueous EDH Karpov, individual a0, common bg
                  strncpy( php->dcpcl[0], "a0", MAXDCNAME );
                  strncpy( php->dcpcl[1], "bg", MAXDCNAME );
                  // php->nscM = 2;
                          //   php->ncpM = 4;
                             break;
             case SM_AQDHH:  // aqueous EDH Helgeson, common a0 and bg
                          //   php->ncpM = 4;
                             break;
             case SM_AQDHS:  // aqueous EDH Shvarov, common a0 and bg
                            // php->ncpM = 4;
                             break;
             case SM_AQSIT:  // SIT model in NEA variant - new implementation
                           //  php->ncpM = 2;  // NPcoef - changed from 1 to 2 on 13.05.09 (DK)
                             break;
             case SM_AQEXUQ: // built-in EUNIQUAC model for aqueous activity coeffs, changed 18.01.2009 (TW)
                  strncpy( php->dcpcl[0], "a0", MAXDCNAME );
                  strncpy( php->dcpcl[1], "bg", MAXDCNAME );
                  //                      php->nscM = 2;  // NP_DC
                            //  php->ncpM = 2;  // NPcoef
                              break;
             case SM_AQPITZ: // built-in Pitzer HMW aqueous activity coefficient model
                              break;
             case SM_AQELVIS:  // built-in ELVIS model for aqueous electrolytes
                  strncpy( php->dcpcl[0], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[1], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[2], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[3], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[4], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[5], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[6], "fi", MAXDCNAME );
                  strncpy( php->dcpcl[7], "fi", MAXDCNAME );
                  //                      php->nscM = 8;  // NP_DC
                          //    php->ncpM = 8;                       // NPcoef = rows of aIPc
                              break;
             default:  // other models
                              break;
              }
          }
      }

    if( php->Ppnc == S_ON )
     if( clearall || ( php->sol_t[SPHAS_TYP] != old_sol[SPHAS_TYP] ) ||
             ( php->sol_t[SGM_MODE] != old_sol[SGM_MODE] ))
     {
         if( php->sol_t[SGM_MODE] == SM_STNGAM )
         {
             for(ii=0; ii<php->ncpN; ii++)
             {
                 sprintf( tbuf, "IntPar%d", ii+1 );
                 strncpy( php->ipicl[ii], tbuf, MAXDCNAME );
             }

             switch(php->sol_t[SPHAS_TYP])
             {
                 case SM_OTHER:   // Customized hardcoded solid-solution models
                                break;
                 case SM_BERMAN:   // Sublattice intra-site interaction model (multicomponent)
                      strncpy( php->ipccl[0], "w1", MAXDCNAME );
                      strncpy( php->ipccl[1], "w2", MAXDCNAME );
                      strncpy( php->ipccl[2], "w3", MAXDCNAME );
                      //          php->ncpM = 3;  // NPcoef
                                break;
                 case SM_CEF: //  = '$',   multicomponent multisite solid-solution model (CALPHAD)
                    strncpy( php->ipccl[0], "w1", MAXDCNAME );
                    strncpy( php->ipccl[1], "w2", MAXDCNAME );
                    strncpy( php->ipccl[2], "w3", MAXDCNAME );
//                    strncpy( php->ipccl[3], "w4", MAXDCNAME );
                      break;
                 case SM_VANLAAR:   // Van Laar model (multicomponent)
                      strncpy( php->ipccl[0], "w1", MAXDCNAME );
                      strncpy( php->ipccl[1], "w2", MAXDCNAME );
                      strncpy( php->ipccl[2], "w3", MAXDCNAME );
                      //          php->ncpM = 3;  // NPcoef
                      //          php->nscM = 1;
                                break;
                 case SM_REGULAR:   // Regular model (multicomponent)
                       strncpy( php->ipccl[0], "w1", MAXDCNAME );
                       strncpy( php->ipccl[1], "w2", MAXDCNAME );
                       strncpy( php->ipccl[2], "w3", MAXDCNAME );
                       //         php->ncpM = 3;  // NPcoef
                                break;
                 case SM_GUGGENM:   // Redlich-Kister model (multicomponent)
                 strncpy( php->ipccl[0], "w01", MAXDCNAME );
                 strncpy( php->ipccl[1], "w02", MAXDCNAME );
                 strncpy( php->ipccl[2], "w03", MAXDCNAME );
                 strncpy( php->ipccl[3], "w04", MAXDCNAME );
                 strncpy( php->ipccl[4], "w11", MAXDCNAME );
                 strncpy( php->ipccl[5], "w12", MAXDCNAME );
                 strncpy( php->ipccl[6], "w13", MAXDCNAME );
                 strncpy( php->ipccl[7], "w14", MAXDCNAME );
                 strncpy( php->ipccl[8], "w21", MAXDCNAME );
                 strncpy( php->ipccl[9], "w22", MAXDCNAME );
                 strncpy( php->ipccl[10], "w23", MAXDCNAME );
                 strncpy( php->ipccl[11], "w24", MAXDCNAME );
                 strncpy( php->ipccl[12], "w31", MAXDCNAME );
                 strncpy( php->ipccl[13], "w32", MAXDCNAME );
                 strncpy( php->ipccl[14], "w33", MAXDCNAME );
                 strncpy( php->ipccl[15], "w34", MAXDCNAME );
                            //    php->ncpM = 16;  // NPcoef
                                break;
                case SM_NRTLLIQ:   // NRTL liquid model (multicomponent), added 03.06.2008 (TW)
                 strncpy( php->ipccl[0], "w01", MAXDCNAME );
                 strncpy( php->ipccl[1], "w02", MAXDCNAME );
                 strncpy( php->ipccl[2], "w03", MAXDCNAME );
                 strncpy( php->ipccl[3], "w04", MAXDCNAME );
                 strncpy( php->ipccl[4], "w05", MAXDCNAME );
                 strncpy( php->ipccl[5], "w06", MAXDCNAME );
                            //    php->ncpM = 6;  // NPcoef
                                break;
                case SM_WILSLIQ:   // Wilson liquid model (multicomponent), added 09.06.2008 (TW)
                 strncpy( php->ipccl[0], "w01", MAXDCNAME );
                 strncpy( php->ipccl[1], "w02", MAXDCNAME );
                 strncpy( php->ipccl[2], "w03", MAXDCNAME );
                 strncpy( php->ipccl[3], "w04", MAXDCNAME );
                              //  php->ncpM = 4;  // NPcoef
                                break;
                case SM_REDKIS:   // Redlich-Kister model (binary)
                 strncpy( php->ipccl[0], "w01", MAXDCNAME );
                 strncpy( php->ipccl[1], "w02", MAXDCNAME );
                 strncpy( php->ipccl[2], "w03", MAXDCNAME );
                           //     php->ncpN = 1; php->ncpM = 3;
                                break;
                case SM_MARGB:  // Margules subregular model (binary)
                 strncpy( php->ipccl[0], "w01", MAXDCNAME );
                 strncpy( php->ipccl[1], "w02", MAXDCNAME );
                 strncpy( php->ipccl[2], "w03", MAXDCNAME );
                           //     php->ncpN = 2; php->ncpM = 3;
                                break;
                case SM_MARGT:  // Margules regular model (ternary)
                 strncpy( php->ipccl[0], "w01", MAXDCNAME );
                 strncpy( php->ipccl[1], "w02", MAXDCNAME );
                 strncpy( php->ipccl[2], "w03", MAXDCNAME );
                 strncpy( php->ipccl[3], "w04", MAXDCNAME );
                             //   php->ncpN = 4; php->ncpM = 3;
                                break;
                case SM_CGFLUID:  // Churakov-Gottschalk (CG) EoS
                                break;
                case SM_PRFLUID:  // Peng-Robinson-Stryjek-Vera (PRSV) EoS, one binary interaction parameter
                 strncpy( php->ipccl[0], "w01", MAXDCNAME );
                 strncpy( php->ipccl[1], "w02", MAXDCNAME );
                           //     php->ncpM = 2;  // NPcoef
                                break;
                case SM_SRFLUID:  // Soave-Redlich-Kwong (SRK) EoS, one binary interaction parameter
                 strncpy( php->ipccl[0], "w01", MAXDCNAME );
                 strncpy( php->ipccl[1], "w02", MAXDCNAME );
                           //       php->ncpM = 2;  // NPcoef
                                break;
                case SM_PR78FL:  // Peng-Robinson (PR78) EoS, one binary interaction parameter
                 strncpy( php->ipccl[0], "w01", MAXDCNAME );
                 strncpy( php->ipccl[1], "w02", MAXDCNAME );
                           //     php->ncpM = 2;  // NPcoef
                                break;
                case SM_CORKFL:  // compensated Redlich-Kwong (CORK) EoS, one binary interaction parameter
                 strncpy( php->ipccl[0], "w01", MAXDCNAME );
                           //     php->ncpM = 1;  // NPcoef
                                break;
                case SM_STFLUID:  // Sterner-Pitzer (STP) EoS, one binary interaction parameter
                 strncpy( php->ipccl[0], "w01", MAXDCNAME );
                            //    php->ncpM = 1;  // NPcoef
                                break;
                case SM_AQDAV:  // aqueous Davies
                 strncpy( php->ipccl[0], "w01", MAXDCNAME );
                 strncpy( php->ipccl[1], "w02", MAXDCNAME );
                 strncpy( php->ipccl[2], "w03", MAXDCNAME );
                 strncpy( php->ipccl[3], "w04", MAXDCNAME );
                              //  php->ncpM = 4; // changed 10.07.2008 DK
                                break;
                case SM_AQDH1:  // aqueous DH limiting law
                 strncpy( php->ipccl[0], "w01", MAXDCNAME );
                 strncpy( php->ipccl[1], "w02", MAXDCNAME );
                 strncpy( php->ipccl[2], "w03", MAXDCNAME );
                 strncpy( php->ipccl[3], "w04", MAXDCNAME );
                             //   php->ncpM = 4;
                                break;
                case SM_AQDH2:  // aqueous DH, individual a0, individual bg
                 strncpy( php->ipccl[0], "w01", MAXDCNAME );
                 strncpy( php->ipccl[1], "w02", MAXDCNAME );
                 strncpy( php->ipccl[2], "w03", MAXDCNAME );
                 strncpy( php->ipccl[3], "w04", MAXDCNAME );
                             //   php->ncpM = 4;
                                break;
                case SM_AQDH3:  // aqueous EDH Karpov, individual a0, common bg
                 strncpy( php->ipccl[0], "w01", MAXDCNAME );
                 strncpy( php->ipccl[1], "w02", MAXDCNAME );
                 strncpy( php->ipccl[2], "w03", MAXDCNAME );
                 strncpy( php->ipccl[3], "w04", MAXDCNAME );
                             //   php->ncpM = 4;
                                break;
                case SM_AQDHH:  // aqueous EDH Helgeson, common a0 and bg
                 strncpy( php->ipccl[0], "w01", MAXDCNAME );
                 strncpy( php->ipccl[1], "w02", MAXDCNAME );
                 strncpy( php->ipccl[2], "w03", MAXDCNAME );
                 strncpy( php->ipccl[3], "w04", MAXDCNAME );
                             //   php->ncpM = 4;
                                break;
                case SM_AQDHS:  // aqueous EDH Shvarov, common a0 and bg
                 strncpy( php->ipccl[0], "w01", MAXDCNAME );
                 strncpy( php->ipccl[1], "w02", MAXDCNAME );
                 strncpy( php->ipccl[2], "w03", MAXDCNAME );
                 strncpy( php->ipccl[3], "w04", MAXDCNAME );
                               // php->ncpM = 4;
                                break;
                case SM_AQSIT:  // SIT model in NEA variant - new implementation
                 strncpy( php->ipccl[0], "w01", MAXDCNAME );
                 strncpy( php->ipccl[1], "w02", MAXDCNAME );
                              //  php->ncpM = 2;  // NPcoef - changed from 1 to 2 on 13.05.09 (DK)
                                break;
                case SM_AQEXUQ: // built-in EUNIQUAC model for aqueous activity coeffs, changed 18.01.2009 (TW)
                 strncpy( php->ipccl[0], "w01", MAXDCNAME );
                 strncpy( php->ipccl[1], "w02", MAXDCNAME );
                               //  php->ncpM = 2;  // NPcoef
                                 break;
                case SM_AQPITZ: // built-in Pitzer HMW aqueous activity coefficient model
                 for(ii=0; ii<php->ncpM; ii++)
                 {
                     sprintf( tbuf, "w%d", ii+1 );
                     strncpy( php->ipccl[ii], tbuf, MAXDCNAME );
                 }
                                 break;
                case SM_AQELVIS:  // built-in ELVIS model for aqueous electrolytes
                 strncpy( php->ipccl[0], "w01", MAXDCNAME );
                 strncpy( php->ipccl[1], "w02", MAXDCNAME );
                 strncpy( php->ipccl[2], "w03", MAXDCNAME );
                 strncpy( php->ipccl[3], "w04", MAXDCNAME );
                 strncpy( php->ipccl[4], "w11", MAXDCNAME );
                 strncpy( php->ipccl[5], "w12", MAXDCNAME );
                 strncpy( php->ipccl[6], "w13", MAXDCNAME );
                 strncpy( php->ipccl[7], "w14", MAXDCNAME );
                             //    php->ncpM = 8;                       // NPcoef = rows of aIPc
                                 break;
                default:  // other models
                                 break;
             }
         }
     }

    if( php->PrpCon == S_ON )
     if( clearall || ( php->kin_t[KinModCode] != old_kin[KinModCode] ) )
     {
         for(ii=0; ii<php->nPRk; ii++)
         {
             sprintf( tbuf, "p_reac_%d", ii+1 );
             strncpy( php->rprcl[ii], tbuf, MAXDCNAME );
         }

         switch(php->kin_t[KinModCode])
         {
         //KinModCode
             case KM_MOD_TST: // = 'T' Generic TST dissolution/precipitation model following Schott ea 2012
             strncpy( php->rpkcl[0], "k0", MAXDCNAME );
             strncpy( php->rpkcl[1], "K0", MAXDCNAME );
             strncpy( php->rpkcl[2], "AF", MAXDCNAME );
             strncpy( php->rpkcl[3], "Ea", MAXDCNAME );
             strncpy( php->rpkcl[4], "bI", MAXDCNAME );
             strncpy( php->rpkcl[5], "bpH", MAXDCNAME );
             strncpy( php->rpkcl[6], "bpe", MAXDCNAME );
             strncpy( php->rpkcl[7], "bEh", MAXDCNAME );
             strncpy( php->rpkcl[8], "p", MAXDCNAME );
             strncpy( php->rpkcl[9], "q", MAXDCNAME );
             strncpy( php->rpkcl[10], "m", MAXDCNAME );
             strncpy( php->rpkcl[11], "u", MAXDCNAME );
             strncpy( php->rpkcl[12], "OmgEf", MAXDCNAME );
             strncpy( php->rpkcl[13], "ResNuc", MAXDCNAME );
                 break;
             case KM_MOD_PAL: // = 'P' Dissolution/precipitation model of the form (Palandri 2004)
             strncpy( php->rpkcl[0], "k0", MAXDCNAME );
             strncpy( php->rpkcl[1], "K0", MAXDCNAME );
             strncpy( php->rpkcl[2], "AF", MAXDCNAME );
             strncpy( php->rpkcl[3], "Ea", MAXDCNAME );
             strncpy( php->rpkcl[4], "bI", MAXDCNAME );
             strncpy( php->rpkcl[5], "bpH", MAXDCNAME );
             strncpy( php->rpkcl[6], "bpe", MAXDCNAME );
             strncpy( php->rpkcl[7], "bEh", MAXDCNAME );
             strncpy( php->rpkcl[8], "p", MAXDCNAME );
             strncpy( php->rpkcl[9], "q", MAXDCNAME );
             strncpy( php->rpkcl[10], "m", MAXDCNAME );
             strncpy( php->rpkcl[11], "u", MAXDCNAME );
             strncpy( php->rpkcl[12], "OmgEf", MAXDCNAME );
             strncpy( php->rpkcl[13], "ResNuc", MAXDCNAME );
                 break;
             case KM_MOD_WOL: // = 'W' Carbonate growth model following (Wolthers 2012)
             strncpy( php->rpkcl[0], "k0", MAXDCNAME );
             strncpy( php->rpkcl[1], "K0", MAXDCNAME );
             strncpy( php->rpkcl[2], "AF", MAXDCNAME );
             strncpy( php->rpkcl[3], "Ea", MAXDCNAME );
             strncpy( php->rpkcl[4], "bI", MAXDCNAME );
             strncpy( php->rpkcl[5], "bpH", MAXDCNAME );
             strncpy( php->rpkcl[6], "bpe", MAXDCNAME );
             strncpy( php->rpkcl[7], "bEh", MAXDCNAME );
             strncpy( php->rpkcl[8], "p", MAXDCNAME );
             strncpy( php->rpkcl[9], "q", MAXDCNAME );
             strncpy( php->rpkcl[10], "m", MAXDCNAME );
             strncpy( php->rpkcl[11], "u", MAXDCNAME );
             strncpy( php->rpkcl[12], "OmgEf", MAXDCNAME );
             strncpy( php->rpkcl[13], "ResNuc", MAXDCNAME );
                 break;
             case KM_MOD_NUGR: // = 'U' Mineral nucleation and growth model with nuclei/particle size distr. (TBD)
                break;
             default:  // other models
                 break;
         }
     }

    if( php->PumpCon == S_ON )
     if( clearall || ( php->kin_t[KinProCode] != old_kin[KinProCode] ) )
     {

         // ph[q].umpcl =  (char (*)[MAXDCNAME])aObj[ o_phumpcl].Alloc( 1, ph[q].numpC, MAXDCNAME );
         switch(php->kin_t[KinProCode])
         {   //KinProCode
             case KM_PRO_MWR:  // = 'M' Kinetics of generic dissolution/precipitation (no uptake, ionex, adsorption)
                 php->PumpCon == S_OFF;
                 break;
             case KM_PRO_UPT:  // = 'U' Kinetics of uptake/entrapment (of minor/trace element) into solid solution
                 php->PumpCon = S_ON;
                 strncpy( php->umpcl[0], "F_Tr", MAXDCNAME );
                 strncpy( php->umpcl[1], "DelTr", MAXDCNAME );
                 strncpy( php->umpcl[2], "DsTr", MAXDCNAME );
                 strncpy( php->umpcl[3], "DlTr", MAXDCNAME );
                 strncpy( php->umpcl[4], "lsl", MAXDCNAME );
                 strncpy( php->umpcl[5], "msl", MAXDCNAME );
                 break;
             case KM_PRO_IEX:  // = 'X' Kinetics of ion exchange (clays, C-S-H, zeolites, ...)

                 break;
             case KM_PRO_ADS:  // = 'A' Kinetics of adsorption (on MWI), redox

                 break;
             case KM_PRO_NUPR: // = 'P' Kinetics of nucleation and precipitation

                 break;
             default:  // other models
                 break;
         }
     }

}



// ----------------- End of m_phase2.cpp -------------------------

