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

#include "node.h"

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


class TNodeArray : public TNode
{
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


    void allocMemory();
    void freeMemory();

    LOCATION getGrid( int iN, int jN, int kN ) const;
    bool isLocationInNode( int ii, int jj, int kk, LOCATION cxyz ) const;

public:

#ifndef IPMGEMPLUGIN

   TNodeArray( int nNodes, MULTI *apm );   // constructor for integration in GEMS
   TNodeArray( int asizeN, int asizeM, int asizeK,MULTI *apm ); // uses 3D specification

   gstring PutGEM2MTFiles( QWidget* par, int nIV,
     bool textmode, bool binmode, bool putNodT1 = false );

   void  setNodeArray( gstring& dbr_file, int ndx, bool binary_f );

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

    ~TNodeArray();      // destructor

    int nNodes()  const   // get number of nodes in node arrays
    { return anNodes; }

    int SizeN()  const
    { return sizeN; }

    int SizeM()  const
    { return sizeM; }

    int SizeK()  const
    { return sizeK; }

    DATABRPTR* pNodT0() const   // get pointer to array of nodes for current time point
    { return NodT0; }

    DATABRPTR* pNodT1() const  // get pointer to array of nodes for previous time point
    { return NodT1; }

    int  RunGEM( int ndx, int Mode );   // calls GEM for a node ndx
    int  RunGEM( int indN, int indM, int indK, int Mode ) // Call GEM for one node
    { return RunGEM( iNode( indN, indM, indK ), Mode); }
                                        // (cleans the work node structure)

   // Copying data from work DATABR structure into the node array
   // (as specified in nodeTypes array)
   void  setNodeArray( int ndx, int* nodeTypes  );
   void  checkNodeArray(int i, int* nodeTypes, const char*  datachbr_file );

   //---------------------------------------------------------
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

    //---------------------------------------------------------
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

};

#endif   // _nodearray_h_

