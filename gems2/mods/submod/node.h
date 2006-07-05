//-------------------------------------------------------------------
// $Id: node.h 684 2005-11-23 11:19:27Z gems $
//
// C/C++ interface between GEM IPM and FMT node array
// Working whith DATACH and DATABR structures
//
// Written by S.Dmytriyeva
// Copyright (C) 2005 S.Dmytriyeva, D.Kulik
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

#ifndef _node_h_
#define _node_h_

#include "m_param.h"
#include "datach.h"
#include "databr.h"

#ifndef IPMGEMPLUGIN
class QWidget;
#endif

class TNode
{
    MULTI* pmm;   // Pointer to GEMIPM work data structure (TMULTI in ms_multi.h)

#ifdef IPMGEMPLUGIN
                  // This is used in isolated GEMIPM2K module for coupled codes
    TMulti* multi;
    TProfil* profil;

#endif

protected:

    DATACH* CSD;       // Pointer to chemical system data structure CSD

    DATABR* CNode;     // Pointer to a work node data bridge structure   CNode
                   // used for sending input data to and receiving results from GEM IPM

    void allocMemory();
    void freeMemory();

    // datach & databr
    void datach_to_file( GemDataStream& ff );   // writes CSD (DATACH structure) to binary file
    void datach_from_file( GemDataStream& ff ); // reads CSD (DATACH structure) from binary file
    void databr_to_file( GemDataStream& ff );   // writes work node (DATABR structure) to binary file
    void databr_from_file( GemDataStream& ff ); // reads work node (DATABR structure) from binary file
    void datach_realloc();
    void datach_free();
    void databr_realloc();
    DATABR* databr_free( DATABR* data_BR_ =0);  // deletes fields of DATABR structure indicated by data_BR_
                                                // and sets the pointer data_BR_ to NULL
    void datach_to_text_file( fstream& ff );    // writes CSD (DATACH structure) to text file
    void datach_from_text_file( fstream& ff);   // reads CSD (DATACH structure) from text file
    void databr_to_text_file(fstream& ff );     // writes work node (DATABR structure) to text file
    void databr_from_text_file(fstream& ff );   // reads work node (DATABR structure) from text file


#ifndef IPMGEMPLUGIN

    // Integration in GEMS - prepares DATACH and DATABR files for reading into the coupled code
    void makeStartDataChBR(
         TCIntArray& selIC, TCIntArray& selDC, TCIntArray& selPH,
         short nTp_, short nPp_, float Ttol_, float Ptol_,
         float *Tai, float *Pai );
    void getG0_V0_H0_Cp0_matrix();  // creates arrays of thermodynamic data for interpolation
                                    // which are written into DATACH file
#endif

public:

#ifndef IPMGEMPLUGIN

   TNode( MULTI *apm );   // constructor for integration in GEMS

#else

  static TNode* na;   // static pointer to this class
                      // for the isolated GEMIPM2K module
  TNode();   // constructors one node

#endif

    virtual ~TNode();      // destructor

    virtual int nNodes()  const   // get number of nodes in node arrays
    { return 1; }

    DATACH* pCSD() const  // get pointer to chemical system data structure
    {
        return CSD;
    }

    double cT() const     // get current Temperature T, K
    {
        return CNode->T;
    }

    double cP() const     // get current Pressure P, bar
    {
        return CNode->P;
    }

    void setNodeHandle( int jj )   // setup Node identification handle
    {
      CNode->NodeHandle = (short)jj;
    }

    void packDataBr();      //  packs GEMIPM output into work node data bridge structure
    void unpackDataBr();    //  unpacks work node data bridge structure into GEMIPM data structure

    int  RunGEM( int Mode );   // calls GEM for a node ndx
                           // (cleans the work node structure)
    // If the corresponding file names are not null, prints current
    // multi and/or work databr structures to files with these names
    void  printfGEM( const char* multi_file, const char* databr_text,
                             const char* databr_bin );

    // For separate coupled FMT-GEM programs that use GEMIPM2K module
    // Reads in the MULTI, DATACH and DATABR files prepared from GEMS
    // and fills out nodes in node arrays according to distribution vector
    // nodeTypes
    int  NewNodeArray( const char*  MULTI_filename,
       const char *ipmfiles_lst_name, int *nodeTypes = 0, bool getNodT1 = false);
    virtual void  setNodeArray( int , int*  ) { }
    virtual void  checkNodeArray( int, int*, const char* ) { }


#ifndef IPMGEMPLUGIN

   // Makes start DATACH and DATABR data using GEMS internal data (MULTI and other)
    void MakeNodeStructures( QWidget* par, bool select_all,
    float *Tai, float *Pai,
    short nTp_ = 1 , short nPp_ = 1 , float Ttol_ = 1., float Ptol_ =1. );
    void MakeNodeStructures(  short anICb, short anDCb,  short anPHb,
    short* axIC, short* axDC,  short* axPH,
    float* Tai, float* Pai,
    short nTp_, short nPp_, float Ttol_, float Ptol_  );

    virtual void  setNodeArray( gstring& , int , bool ) { }

#else

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Specific coupling issues
//  Wrappers for coupling MCOTAC-GEMS

    void GEM_input_from_MT(    // GEM_input_from_MCOTAC
       short p_NodeHandle,    // Node identification handle
       short p_NodeStatusCH,  // Node status code CH;  see typedef NODECODECH
       double p_T,     // Temperature T, K                        +      +      -     -
       double p_P,     // Pressure P, bar                         +      +      -     -
       double p_Ms,    // Mass of reactive subsystem, kg          +      +      -     -
       double p_dt,    // actual time step
       double p_dt1,   // priveous time step
       double  *p_dul, // upper kinetic restrictions [nDCb]       +      +      -     -
       double  *p_dll,  // lower kinetic restrictions [nDCb]      +      +      -     -
       double  *p_bIC  // bulk mole amounts of IC[nICb]           +      +      -     -
   );

   void GEM_input_back_to_MT(    // GEM_input_copy_to_MCOTAC
       short &p_NodeHandle,    // Node identification handle
       short &p_NodeStatusCH,  // Node status code CH;  see typedef NODECODECH
       double &p_T,     // Temperature T, K                        +      +      -     -
       double &p_P,     // Pressure P, bar                         +      +      -     -
       double &p_Ms,    // Mass of reactive subsystem, kg          +      +      -     -
       double &p_dt,    // actual time step
       double &p_dt1,   // priveous time step
       double  *p_dul,  // upper kinetic restrictions [nDCb]       +      +      -     -
       double  *p_dll,  // lower kinetic restrictions [nDCb]       +      +      -     -
       double  *p_bIC  // bulk mole amounts of IC[nICb]            +      +      -     -
   );

   void GEM_output_to_MT(      // GEM_output_to_MCOTAC
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

#endif   // _node_h_

