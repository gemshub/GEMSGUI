//-------------------------------------------------------------------
// $Id: nodearray.h 684 2005-11-23 11:19:27Z gems $
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

#ifndef _nodearray_h_
#define _nodearray_h_

#include "m_param.h"
#include "datach.h"
#include "databr.h"

#ifndef IPMGEMPLUGIN
class QWidget;
#endif

enum  PTCODE // Codes of particle type
{
   ADVECTIVE = 21,
   DIFFUSIVE = 22,
   COLLOID = 23

};

struct  LOCATION // Location (coordinates) of a point in space
{
  float x,
        y,
        z;

  LOCATION():
   x(0.), y (0.), z(0.) {}

  LOCATION( float ax, float ay=0., float az=0. ):
     x(ax), y (ay), z(az) {}

  LOCATION( const LOCATION& loc ):
        x(loc.x), y (loc.y), z(loc.z) {}

//   const LOCATION& operator= (const LOCATION& loc);
//   {
//      x = loc.x;
//      y = loc.y;
//      z = loc.z;
//    }
};


class TNodeArray
{
    MULTI* pmm;   // Pointer to GEMIPM work data structure (TMULTI in ms_multi.h)

    DATACH* CSD;       // Pointer to chemical system data structure CSD

    DATABR* CNode;     // Pointer to a work node data bridge structure   CNode
                       // used for sending input data to and receiving results from GEM IPM
    DATABR* (*NodT0);  // array of nodes for current time point   DATABR* (*NodT0)
    DATABR* (*NodT1);  // array of nodes for previous time point  DATABR* (*NodT1)

    int anNodes;       // Number of allocated nodes
    int sizeN;
    int sizeM;
    int sizeK;

    LOCATION size;     // spatial dimensions of the medium ( x, 0, 0 - 1D; x,y,0 - 2D; x,0,z - 2D; x,y,z - 3D )
                       // defines topology of nodes (N of grid points per node): 1D- 2; 2D- 4; 3D- 8 )
                       // relative to coordinate origin (0,0,0) units
    LOCATION* grid;   // Array of grid point locations, size is anNodes

    char* tNode;     // Node type codes (see DataBR.h) size anNodes

#ifdef IPMGEMPLUGIN
                  // This is used in isolated GEMIPM2K module for coupled codes
    TMulti* multi;
    TProfil* profil;

#endif

    void allocMemory();
    void freeMemory();

    LOCATION getGrid( int iN, int jN, int kN ) const;
    bool isLocationInNode( int ii, int jj, int kk, LOCATION cxyz ) const;

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

   TNodeArray( int nNodes, MULTI *apm );   // constructor for integration in GEMS
   TNodeArray( int asizeN, int asizeM, int asizeK,MULTI *apm ); // uses 3D specification

#else

   static TNodeArray* na;   // static pointer to this class
                            // for the isolated GEMIPM2K module
  TNodeArray( int nNod );   // constructors for 1D arrangement of nodes
  TNodeArray( int asizeN, int asizeM, int asizeK ); // uses 3D specification

#endif

 inline
  int iNode( int indN, int indM, int indK ) const // make one node index from three
     { return  (( indK * sizeM + indM  ) * sizeN + indN);  }

   int indN( int ndx ) const // get i index from full
   { return  (ndx % sizeN);  }

   int indM( int ndx ) const // get j index from full
   {
    int j = (ndx - ndx % sizeN);
        j /=  sizeN;
    return  (j % sizeM);
   }

    int indK( int ndx ) const // get K index from full
    {
      int k = ndx - ndx % sizeN;
          k /= sizeN;
          k = k - k % sizeM;
      return  k/sizeM;
   }


   int  RunGEM( int indN, int indM, int indK, int Mode ) // Call GEM for one node
     { return RunGEM( iNode( indN, indM, indK ), Mode); }

    ~TNodeArray();      // destructor

    int nNodes()  const   // get number of nodes in node arrays
      { return anNodes; }

    int SizeN()  const
    { return sizeN; }

    int SizeM()  const
    { return sizeM; }

    int SizeK()  const
    { return sizeK; }

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

    DATABRPTR* pNodT0() const   // get pointer to array of nodes for current time point
    {
        return NodT0;
    }

    DATABRPTR* pNodT1() const  // get pointer to array of nodes for previous time point
    {
        return NodT1;
    }

    int  RunGEM( int ndx, int Mode );   // calls GEM for a node ndx
                                        // (cleans the work node structure)

    // working with node array

    //  Copies data for a node ndx from array of nodes anyNodeArray that contains nNodes
    //  into work node data bridge structure
    void CopyWorkNodeFromArray( int ndx, int nNodes, DATABRPTR* anyNodeArray );

    //  Moves work node data to ndx element of node array anyNodeArray that has nNodes
    //  Previous contents of ndx elements will be lost, work node will be allocated
    //  new and will contain no data
    void MoveWorkNodeToArray( int ndx, int nNodes, DATABRPTR* anyNodeArray );

    // copies a node from node array arr_From to the same place in node array arr_To
    // previous contents of ndx element in arr_To will be lost
    // uses work node structure which will be newly allocated and will contain no
    // data afterwards
    void CopyNodeFromTo( int ndx, int nNodes, DATABRPTR* arr_From, DATABRPTR* arr_To );

    void packDataBr();      //  packs GEMIPM output into work node data bridge structure
    void unpackDataBr();    //  unpacks work node data bridge structure into GEMIPM data structure

    // If the corresponding file names are not null, prints current
    // multi and/or work databr structures to files with these names
    void  printfGEM( const char* multi_file, const char* databr_text,
                             const char* databr_bin );

    // Data collection for monitoring differences
    // Prints difference increments in a all nodes (cells) for step t (time point at)
    void logDiffsIC( FILE* diffile, int t, double at, int nx, int every_t );
    // Prints dissolved elemental molarities in all cells for time point t / at
    void logProfileAqIC( FILE* logfile, int t, double at, int nx, int every_t );
    // Prints total elemental amounts in all cells for time point t / at
    void logProfileTotIC( FILE* logfile, int t, double at, int nx, int every_t );
    // Prints amounts of phases in all cells for time point t / at
    void logProfilePhMol( FILE* logfile, int t, double at, int nx, int every_t );
    // Prints dissolved species molarities in all cells for time point t / at
    void logProfileAqDC( FILE* logfile, int t, double at, int nx, int every_t );


    // For separate coupled FMT-GEM programs that use GEMIPM2K module
    // Reads in the MULTI, DATACH and DATABR files prepared from GEMS
    // and fills out nodes in node arrays according to distribution vector
    // nodeTypes
    int  NewNodeArray( const char*  MULTI_filename,
       const char *ipmfiles_lst_name, int *nodeTypes, bool getNodT1 = false);

    //---------------------------------------------------------
    // working with grid

    // Set grid coordinate array use predefined array aGrid
    // or set up regular scale
     void SetGrid( float aSize[3], float (*aGrid)[3] = 0 );
     // Finds a node absolute index for the current
     // point location (uses grid coordinate array grid[])
     // performance-important functions to be used e.g. in particle tracking methods
     int FindNodeFromLocation( LOCATION cxyz, int old_node = -1 ) const;
     // get 3D sizes for node (  from cxyz[0] - to cxyz[1] )
     void GetNodeSizes( int ndx, LOCATION cxyz[2] );
     // get 3D location for node (  from cxyz[0] - to cxyz[1] )
     LOCATION& GetNodeLocation( int ndx )
     { return grid[ndx]; }
     // get 3D size of region
     LOCATION& GetSize()
     { return size; }
     // get full mass particle type in node ndx
     double GetNodeMass( int ndx, char type, char tcode, unsigned char ips );
     // move mass m_v from node ndx_from to node ind_to, particle type
     void MoveParticleMass( int ndx_from, int ind_to,
            char type, char tcode, unsigned char ips, double m_v );


#ifndef IPMGEMPLUGIN

   // Makes start DATACH and DATABR data using GEMS internal data (MULTI and other)
   void MakeNodeStructures( QWidget* par, bool select_all,
    float *Tai, float *Pai,
    short nTp_ = 1 , short nPp_ = 1 , float Ttol_ = 1., float Ptol_ =1. );
    void MakeNodeStructures(  short anICb, short anDCb,  short anPHb,
    short* axIC, short* axDC,  short* axPH,
    float* Tai, float* Pai,
    short nTp_, short nPp_, float Ttol_, float Ptol_  );
    gstring PutGEM2MTFiles( QWidget* par, int nIV,
      bool textmode, bool binmode, bool putNodT1 = false );

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

#endif   // _nodearray_h_

