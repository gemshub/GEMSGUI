//-------------------------------------------------------------------
// $Id: ms_multi.h 581 2005-10-12 11:19:27Z gems $
//
// C/C++ interface between GEM IPM and FMT node array
// Working whith DATACH and DATABR structures
//
// Written by S.Dmytriyeva
// Copyright (C) 2004-2005 S.Dmytriyeva, D.Kulik
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
#ifndef _tnodearray_h_
#define _tnodearray_h_

#include "datach.h"
#include "databr.h"

#ifndef IPMGEMPLUGIN

#include "m_param.h"
//#include "v_ipnc.h"

#else

class GemDataStream;
#include "ms_multi.h"
#include "m_const.h"

#endif


class TNodeArray
{
    MULTI *pmm;

public:

   DATABR  *data_BR;
   DATACH  *data_CH;

#ifndef IPMGEMPLUGIN

   TNodeArray( MULTI *apm );

#else

   int nNodes;
   DATABR  *(*arr_BR);

   TNodeArray( int anN, MULTI *apm );

#endif

    ~TNodeArray();

    // working with array
    void GetNodeCopyFromArray( int ii, int nNodes, DATABRPTR* arr_BR );
    void SaveNodeCopyToArray( int ii, int nNodes, DATABRPTR* arr_BR );
    void CopyTo( DATABR *(*dBR) );

    // datach & databr
    void datach_to_file( GemDataStream& ff );
    void datach_from_file( GemDataStream& ff );
    void databr_to_file( GemDataStream& ff );
    void databr_from_file( GemDataStream& ff );
    void datach_realloc();
    void datach_free();
    void databr_realloc();
    void databr_free( DATABR *data_BR_ =0);
    void datach_to_text_file( fstream& ff );
    void datach_from_text_file( fstream& ff);
    void databr_to_text_file(fstream& ff );
    void databr_from_text_file(fstream& ff );

    void packDataBr();
    void unpackDataBr();

#ifndef IPMGEMPLUGIN

    void makeStartDataChBR(
         TCIntArray& selIC, TCIntArray& selDC, TCIntArray& selPH,
         short nTp_, short nPp_, float Ttol_, float Ptol_,
         float *Tai, float *Pai );
    void getG0_V0_H0_Cp0_matrix();

#else

    void GEM_input_from_MT(
       short p_NodeHandle,    // Node identification handle
       short p_NodeStatusCH,  // Node status code CH;  see typedef NODECODECH
       double p_T,     // Temperature T, K                        +      +      -     -
       double p_P,     // Pressure P, bar                         +      +      -     -
       double p_Ms,    // Mass of reactive subsystem, kg          +      +      -     -
       double p_dt,    // actual time step
       double p_dt1,   // priveous time step
       double  *p_dul, // upper kinetic restrictions [nDCb]            +      +      -     -
       double  *p_dll,  // lower kinetic restrictions [nDCb]            +      +      -     -
       double  *p_bIC  // bulk mole amounts of IC[nICb]                +      +      -     -
   );
   void GEM_input_back_to_MT(
       short &p_NodeHandle,    // Node identification handle
       short &p_NodeStatusCH,  // Node status code CH;  see typedef NODECODECH
       double &p_T,     // Temperature T, K                        +      +      -     -
       double &p_P,     // Pressure P, bar                         +      +      -     -
       double &p_Ms,    // Mass of reactive subsystem, kg          +      +      -     -
       double &p_dt,    // actual time step
       double &p_dt1,   // priveous time step
       double  *p_dul,  // upper kinetic restrictions [nDCb]            +      +      -     -
       double  *p_dll,  // lower kinetic restrictions [nDCb]            +      +      -     -
       double  *p_bIC  // bulk mole amounts of IC[nICb]                +      +      -     -
   );
   void GEM_output_to_MT(
       short &p_NodeHandle,    // Node identification handle
       short &p_NodeStatusCH,  // Node status code CH;  see typedef NODECODECH
       short &p_IterDone,      // Number of iterations performed by IPM (if not need GEM)
// Chemical scalar variables
       double &p_Vs,    // Volume V of reactive subsystem, cm3     -      -      +     +
       double &p_Gs,    // Gibbs energy of reactive subsystem (J)  -      -      +     +
       double &p_Hs,    // Enthalpy of reactive subsystem (J)      -      -      +     +
       double &p_IC,    // Effective molal aq ionic strength           -      -      +     +
       double &p_pH,    // pH of aqueous solution                      -      -      +     +
       double &p_pe,    // pe of aqueous solution                      -      -      +     +
       double &p_Eh,    // Eh of aqueous solution, V                   -      -      +     +
       double &p_denW,
       double &p_denWg, // Density of H2O(l) and steam at T,P      -      -      +     +
       double &p_epsW,
       double &p_epsWg, // Diel.const. of H2O(l) and steam at T,P  -      -      +     +
// Dynamic data - dimensions see in DATACH.H and DATAMT.H structures
// exchange of values occurs through lists of indices, e.g. xDC, xPH
       double  *p_xDC,    // DC mole amounts at equilibrium [nDCb]      -      -      +     +
       double  *p_gam,    // activity coeffs of DC [nDCb]               -      -      +     +
       double  *p_xPH,  // total mole amounts of phases [nPHb]          -      -      +     +
       double  *p_vPS,  // phase volume, cm3/mol        [nPSb]          -      -      +     +
       double  *p_mPS,  // phase (carrier) mass, g      [nPSb]          -      -      +     +
       double  *p_bPS,  // bulk compositions of phases  [nPSb][nICb]    -      -      +     +
       double  *p_xPA,  // amount of carrier in phases  [nPSb] ??       -      -      +     +
       double  *p_bIC,  // bulk mole amounts of IC[nICb]                +      +      -     -
       double  *p_rMB,  // MB Residuals from GEM IPM [nICb]             -      -      +     +
       double  *p_dul,  // upper kinetic restrictions [nDCb]            +      +      -     -
       double  *p_dll,  // lower kinetic restrictions [nDCb]            +      +      -     -
       double  *p_uIC  // IC chemical potentials (mol/mol)[nICb]       -      -      +     +
   );

#endif

};


#endif   // _tnodearray_h_

