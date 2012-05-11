//-------------------------------------------------------------------
// TNodeArray class - implements an advanced (level 2)
// C/C++ interface with GEM IPM for development of coupled
// and GEM-FMT codes
// Works with DATACH and work DATABR structures
// Uses the TNode class
//
// Written by S.Dmytriyeva,  D.Kulik
// Copyright (C) 2006 S.Dmytriyeva, D.Kulik
//
// This file is part of GEMIPM2K and GEMS-PSI codes for
// thermodynamic modelling by Gibbs energy minimization

// This file may be distributed under the licence terms
// defined in GEMIPM2K.QAL
//
// See also http://gems.web.psi.ch/
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//

#ifndef _nodearray_h_
#define _nodearray_h_

#include "node.h"

// These structures are needed for implementation of Random Walk and
// similar particle-based transport algorithms
enum  PTCODE // Codes of particle type
{
   DISSOLVED = 20,
   ADVECTIVE = 21,
   DIFFUSIVE = 22,
   COLLOID = 23
};

struct  LOCATION // Location (coordinates) of a point in space
{                // for implementation of particle transport algorithms
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

// Definition of TNodeArray class

class TNodeArray : public TNode
{
    DATABR* (*NodT0);  // array of nodes for previous time point
    DATABR* (*NodT1);  // array of nodes for current time point

    int anNodes;       // Number of allocated nodes
    int sizeN;		   // Number of nodes along x direction
    int sizeM;           // Number of nodes along y direction
    int sizeK;           // Number of nodes along z direction

    LOCATION size;     // spatial dimensions of the medium ( x, 0, 0 - 1D; x,y,0 - 2D; x,0,z - 2D; x,y,z - 3D )
                       // defines topology of nodes (N of grid points per node): 1D- 2; 2D- 4; 3D- 8 )
                       // relative to coordinate origin (0,0,0) units
    LOCATION* grid;    // Array of grid point locations, size is anNodes

    char* tcNode;      // Node type codes (see databr.h), size anNodes
    bool* iaNode;      // GEM IA status for all nodes (true: NEED_GEM_AIA, false: NEED_GEM_PIA)

    void allocMemory();
    void freeMemory();

   // Prototypes of functions to manage location of particles
   // within nodes relative to the whole grid of the node walls
   LOCATION getGrid( int iN, int jN, int kN ) const;

   // Test if the location cxyz resides in the node ( ii,jj,kk )
   bool isLocationInNode( int ii, int jj, int kk, LOCATION cxyz ) const;
   // Test if the location cxyz resides in the node with absolute index iNode
   bool isLocationInNode( int iNode, LOCATION cxyz ) const;

public:

  static TNodeArray* na;   // static pointer to this class

#ifndef IPMGEMPLUGIN
// These calls are used only inside of GEMS-PSI GEM2MT module

   // constructor for integration in GEM2MT module of GEMS-PSI
   TNodeArray( int nNodes, MULTI *apm );

   // constructor that uses 3D node arrangement
   TNodeArray( int asizeN, int asizeM, int asizeK,MULTI *apm );

  // Prints MULTI, DATACH and DATABR files structure prepared from GEMS
  // for separate coupled FMT-GEM programs that use GEMIPM2K module
  // or if putNodT1 == true  as a break point for the running FMT calculation
  gstring PutGEM2MTFiles(  QWidget* par, int nIV,
      bool multi_bin_mode, bool bin_mode, bool putNodT1=false );

   // Reads DATABR files saved by GEMS as a break point of the FMT calculation
   // Copying data from work DATABR structure into the node array NodT0
   // and read DATABR structure into the node array NodT1 from file dbr_file
   void  setNodeArray( gstring& dbr_file, int ndx, bool binary_f );

#else
// Used in GEMIPM2 standalone module only
   TNodeArray( int nNod );   // constructors for 1D arrangement of nodes
   TNodeArray( int asizeN, int asizeM, int asizeK );
   // constructor that uses 3D node arrangement
#endif

// makes one absolute node index from three spatial coordinate indexes
   inline int iNode( int indN, int indM, int indK ) const
     { return  (( indK * sizeM + indM  ) * sizeN + indN);  }

     // get i index along N (x axis) from the absolute index ndx
   inline int indN( int ndx ) const
    { return  (ndx % sizeN);  }

    // get j index along M (y axis) from the absolute index ndx
   inline int indM( int ndx ) const
    {
     int j = (ndx - ndx % sizeN);
         j /=  sizeN;
     return  (j % sizeM);
    }

    // get k index along K (z axis) from the absolute index ndx
   inline int indK( int ndx ) const
    {
      int k = ndx - ndx % sizeN;
          k /= sizeN;
          k = k - k % sizeM;
      return  k/sizeM;
    }

    ~TNodeArray();      // destructor

    int nNodes() const  // get total number of nodes in the node array
    { return anNodes; }

    int SizeN() const  // get number of nodes in N direction (along x coordinate)
    { return sizeN; }

    int SizeM() const  // get number of nodes in M direction (along y coordinate)
    { return sizeM; }

    int SizeK() const  // get number of nodes in K direction (along z coordinate)
    { return sizeK; }

    DATABRPTR* pNodT0() const // get pointer to array of nodes for the previous time point
    { return NodT0; }

    DATABRPTR* pNodT1() const // get pointer to array of nodes for the current time point
    { return NodT1; }

    bool* piaNode() const // get pointer to IA switches for nodes
    { return iaNode; }
    
    // Calls GEM IPM calculation for a node with absolute index ndx
    int RunGEM( int ndx, int Mode );

    // Calls GEM IPM for one node with three indexes (along x,y,z)
    int  RunGEM( int indN, int indM, int indK, int Mode )
    { return RunGEM( iNode( indN, indM, indK ), Mode); }
        // (both calls clean the work node DATABR structure)

    // Copies data from the work DATABR structure into the node ndx in  ?????
    // the node arrays NodT0 and NodT1  (as specified in nodeTypes array)
    void  setNodeArray( int ndx, int* nodeTypes  );

   // test setup of the boundary condition for all nodes in the task
    void  checkNodeArray(int i, int* nodeTypes, const char*  datachbr_file );

   //---------------------------------------------------------
   // Methods for working with node arrays (access to data from DBR)
   // Calculate phase (carrier) mass, g  of single component phase
   double get_mPH( int ia, int nodex, int PHx );
   // Calculate phase volume, cm3/mol  of single component phase
   double get_vPH( int ia, int nodex, int PHx );
   // Calculate bulk compositions  of single component phase
   double get_bPH( int ia, int nodex, int PHx, int IC );


   //---------------------------------------------------------
   // Methods for working with node arrays

    //  Copies data for a node ndx from the array of nodes anyNodeArray that
     // contains nNodes into the work node data bridge structure
    void CopyWorkNodeFromArray( int ndx, int nNodes, DATABRPTR* anyNodeArray );

    //  Moves work node data to the ndx element of the node array anyNodeArray
     // that has nNodes. Previous contents of the ndx element will be lost,
     // work node will be allocated new and will contain no data
    void MoveWorkNodeToArray( int ndx, int nNodes, DATABRPTR* anyNodeArray );

    // Copies a node from the node array arr_From to the same place in the
     // node array arr_To. Previous contents of the ndx element in arr_To
     // will be lost. Uses the work node structure which will be newly
     // allocated and contain no data afterwards
    void CopyNodeFromTo( int ndx, int nNodes, DATABRPTR* arr_From,
         DATABRPTR* arr_To );

    //---------------------------------------------------------
    // Data collection for monitoring differences

    // Prints difference increments in all nodes (cells) for step t (time point at)
    void logDiffsIC( FILE* diffile, int t, double at, int nx, int every_t );

    // Prints dissolved elemental molarities in all cells for time point t / at
    void logProfileAqIC( FILE* logfile, int t, double at, int nx, int every_t );

    // Prints total elemental amounts in all cells for time point t / at
    void logProfileTotIC( FILE* logfile, int t, double at, int nx, int every_t );

    // Prints amounts of phases in all cells for time point t / at
    void logProfilePhMol( FILE* logfile, int t, double at, int nx, int every_t );

    // Prints dissolved species molarities in all cells for time point t / at
    void logProfileAqDC( FILE* logfile, int t, double at, int nx, int every_t );

    //---------------------------------------------------------
    // Working with the node grid (mainly used in Random Walk algorithms)

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

     // get 3D size of the whole region
     LOCATION& GetSize()
     { return size; }

     // get full mass particle type in the node ndx
     double GetNodeMass( int ndx, char type, char tcode, unsigned char ips );

     // move a mass m_v from node ndx_from to node ind_to, for particle type
     void MoveParticleMass( int ndx_from, int ind_to, char type, char ComponentMode, 
    		 char tcode, unsigned char ips, double m_v );

};

//IC node data access macroses
#define node0_bIC( nodex, ICx ) (TNodeArray::na->pNodT0()[(nodex)]->bIC[(ICx)])
#define node1_bIC( nodex, ICx ) (TNodeArray::na->pNodT1()[(nodex)]->bIC[(ICx)])
#define node0_rMB( nodex, ICx ) (TNodeArray::na->pNodT0()[(nodex)]->rMB[(ICx)])
#define node1_rMB( nodex, ICx ) (TNodeArray::na->pNodT1()[(nodex)]->rMB[(ICx)])
#define node0_uIC( nodex, ICx ) (TNodeArray::na->pNodT0()[(nodex)]->uIC[(ICx)])
#define node1_uIC( nodex, ICx ) (TNodeArray::na->pNodT1()[(nodex)]->uIC[(ICx)])

//DC node data access macroses

  // amount of DC with index DCx from T0 node with index nodex
#define node0_xDC( nodex, DCx ) (TNodeArray::na->pNodT0()[(nodex)]->xDC[(DCx)])
  // amount of DC with index DCx from T1 node with index nodex
#define node1_xDC( nodex, DCx ) (TNodeArray::na->pNodT1()[(nodex)]->xDC[(DCx)])

  // activity coefficient of DC with index DCx from T0 node with index nodex
#define node0_gam( nodex, DCx ) (TNodeArray::na->pNodT0()[(nodex)]->gam[(DCx)])
  // activity coefficient of DC with index DCx from T1 node with index nodex
#define node1_gam( nodex, DCx ) (TNodeArray::na->pNodT1()[(nodex)]->gam[(DCx)])

  // upper constraint on amount of DC with index DCx from T0 node with index nodex
#define node0_dul( nodex, DCx ) (TNodeArray::na->pNodT0()[(nodex)]->dul[(DCx)])
  // upper constraint on amount of DC with index DCx from T1 node with index nodex
#define node1_dul( nodex, DCx ) (TNodeArray::na->pNodT1()[(nodex)]->dul[(DCx)])

  // lower constraint on amount of DC with index DCx from T0 node with index nodex
#define node0_dll( nodex, DCx ) (TNodeArray::na->pNodT0()[(nodex)]->dll[(DCx)])
  // lower constraint on amount of DC with index DCx from T1 node with index nodex
#define node1_dll( nodex, DCx ) (TNodeArray::na->pNodT1()[(nodex)]->dll[(DCx)])

//Phase node data access macroses
  // amount of phase with index PHx from T0 node with index nodex
#define node0_xPH( nodex, PHx ) (TNodeArray::na->pNodT0()[(nodex)]->xPH[(PHx)])
  // amount of phase with index PHx from T1 node with index nodex
#define node1_xPH( nodex, PHx ) (TNodeArray::na->pNodT0()[(nodex)]->xPH[(PHx)])

  // volume of multicomponent phase with index PHx from T0 node with index nodex
#define node0_vPS( nodex, PHx ) (TNodeArray::na->pNodT0()[(nodex)]->vPS[(PHx)])
  // volume of multicomponent phase with index PHx from T1 node with index nodex
#define node1_vPS( nodex, PHx ) (TNodeArray::na->pNodT1()[(nodex)]->vPS[(PHx)])

  // volume of single-component phase with index PHx from T0 node with index nodex
#define node0_vPH( nodex, PHx ) (TNodeArray::na->get_vPH( 0, (nodex), (PHx)))
  // volume of single-component phase with index PHx from T1 node with index nodex
#define node1_vPH( nodex, PHx ) (TNodeArray::na->get_vPH( 1, (nodex), (PHx)))

  // mass of multicomponent phase with index PHx from T0 node with index nodex
#define node0_mPS( nodex, PHx ) (TNodeArray::na->pNodT0()[(nodex)]->mPS[(PHx)])
  // mass of multicomponent phase with index PHx from T1 node with index nodex
#define node1_mPS( nodex, PHx ) (TNodeArray::na->pNodT1()[(nodex)]->mPS[(PHx)])

  // mass of single-component phase with index PHx from T0 node with index nodex
#define node0_mPH( nodex, PHx )  (TNodeArray::na->get_mPH( 0, (nodex), (PHx)))
  // mass of single-component phase with index PHx from T1 node with index nodex
#define node1_mPH( nodex, PHx )  (TNodeArray::na->get_mPH( 1, (nodex), (PHx)))


  // amount of solvent/sorbent in phase with index PHx from T0 node with index nodex
#define node0_xPA( nodex, PHx ) (TNodeArray::na->pNodT0()[(nodex)]->xPA[(PHx)])
  // amount of solvent/sorbent in phase with index PHx from T1 node with index nodex
#define node1_xPA( nodex, PHx ) (TNodeArray::na->pNodT1()[(nodex)]->xPA[(PHx)])

// Phase compositions node data access macroses
// amount of independent component ICx in multi-component phase PHx in T0 node nodex
#define node0_bPS( nodex, PHx, ICx ) ( TNodeArray::na->pNodT0()[(nodex)]->bPS[ \
                                       (PHx)*TNodeArray::na->pCSD()->nICb+(ICx)])
// amount of independent component ICx in multi-component phase PHx in T1 node nodex
#define node1_bPS( nodex, PHx, ICx ) ( TNodeArray::na->pNodT1()[(nodex)]->bPS[ \
                                       (PHx)*TNodeArray::na->pCSD()->nICb+(ICx)])

// amount of independent component ICx in single-component phase PHx in T0 node nodex
#define node0_bPH( nodex, PHx, ICx )  (TNodeArray::na->get_bPH( 0, (nodex), (PHx), (ICx)))
// amount of independent component ICx in single-component phase PHx in T0 node nodex
#define node1_bPH( nodex, PHx, ICx )  (TNodeArray::na->get_bPH( 0, (nodex), (PHx), (ICx)))

#endif   // _nodearray_h_

// end nodearray.h
