//-------------------------------------------------------------------
// $Id: node.h 1066 2008-05-16 14:16:59Z gems $
//
// TNode class - implements a simple C/C++ interface
// between GEM IPM and FMT codes
// Works with DATACH and work DATABR structures
// without using the Tnodearray class
//
// (c) 2006,2008 S.Dmytriyeva, D.Kulik
//
// This file is part of GEMIPM2K and GEMS-PSI codes for
// thermodynamic modelling by Gibbs energy minimization
// developed in the Laboratory for Waste Management,
//   Paul Scherrer Institute

// This file may be distributed under the licence terms
// defined in GEMIPM2K.QAL
//
// See also http://gems.web.psi.ch/
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

extern const double bar_to_Pa,
               m3_to_cm3;

class TNode
{
  gstring dbr_file_name;  // place for the *dbr. I/O file name

protected:
   MULTI* pmm;  // Pointer to GEM IPM work data structure (see ms_multi.h)

#ifdef IPMGEMPLUGIN
       // These pointers are only used in standalone GEMIPM2K programs
    TMulti* multi;
    TProfil* profil;
#endif

    DATACH* CSD;  // Pointer to chemical system data structure CSD (DATACH)
    DATABR* CNode;  // Pointer to a work node data bridge structure (node)
         // used for exchanging input data and results between FMT and GEM IPM

    // These four values are set by the last GEM_run() call
    double CalcTime;  // GEMIPM2 calculation time, s
    long int
        PrecLoops,    // Number of performed IPM-2 precision refinement loops
        NumIterFIA,   // Total Number of performed FIA entry iterations
        NumIterIPM;   // Total Number of performed IPM main iterations

    // Tests Tc as a grid point for the interpolation of thermodynamic data
    // Returns index in the lookup grid array or -1  if it is not a grid point
    long int  check_grid_T( double Tc );

    // Tests P as a grid point for the interpolation of thermodynamic data
    // Return index in the lookup grid array or -1 if it is not a grid point
    long int  check_grid_P( double P );

    void allocMemory();
    void freeMemory();

   // Functions that maintain DATACH and DATABR memory allocation
    void datach_realloc();
    void datach_free();
    void datach_reset();
    void databr_realloc();
    void databr_reset( DATABR *CNode, long int level=0 );

    // deletes fields of DATABR structure indicated by data_BR_
    // and sets the pointer data_BR_ to NULL
    DATABR* databr_free( DATABR* data_BR_ =0);

    // Binary i/o functions
    // including file i/o using GemDataStream class (with account for endianness)
      // writes CSD (DATACH structure) to a binary DCH file
    void datach_to_file( GemDataStream& ff );
      // reads CSD (DATACH structure) from a binary DCH file
    void datach_from_file( GemDataStream& ff );
      // writes node (work DATABR structure) to a binary DBR file
    void databr_to_file( GemDataStream& ff );
      // reads node (work DATABR structure) from a binary DBR file
    void databr_from_file( GemDataStream& ff );

    // Text i/o functions
    // writes CSD (DATACH structure) to a text DCH file
    // brief_mode - Do not write data items that contain only default values
    // with_comments -Write files with comments for all data entries ( in text mode)
    void datach_to_text_file( fstream& ff, bool with_comments = true, bool brief_mode = false );
    // reads CSD (DATACH structure) from a text DCH file
    void datach_from_text_file( fstream& ff);
    // writes work node (DATABR structure) to a text DBR file
    // brief_mode - Do not write data items that contain only default values
    // with_comments -Write files with comments for all data entries ( in text mode)
    void databr_to_text_file(fstream& ff, bool with_comments = true, bool brief_mode = false  );
     // reads work node (DATABR structure) from a text DBR file
    void databr_from_text_file(fstream& ff );

    // virtual functions for interaction with TNodeArray class (not used at TNode level)
    virtual void  setNodeArray( long int , long int*  ) { }
    virtual void  checkNodeArray( long int, long int*, const char* ) { }
    virtual long int nNodes()  const // virtual call for interaction with TNodeArray class
    { return 1; }

#ifndef IPMGEMPLUGIN
    // Integration in GEMS-PSI GUI environment
    // Prepares and writes DCH and DBR files for reading into the coupled code
    void makeStartDataChBR( QWidget* par,
         TCIntArray& selIC, TCIntArray& selDC, TCIntArray& selPH,
         short nTp_, short nPp_, float Ttol_, float Ptol_,
         float *Tai, float *Pai );

    // Creates lookup arrays for interpolation of thermodynamic data
    void G0_V0_H0_Cp0_DD_arrays( QWidget* par ); // to be written into DCH file

    // Virtual function for interaction with TNodeArray class
    virtual void  setNodeArray( gstring& , long int , bool ) { }
#endif

public:

static TNode* na;   // static pointer to this TNode class instance

#ifndef IPMGEMPLUGIN
// Constructor of the class instance in memory in GEMS environment
  TNode( MULTI *apm );   
#else
  // Constructor of the class instance in memory for standalone GEMIPM2K or coupled program
  TNode();    
#endif

  virtual ~TNode();      // destructor

// Typical sequence for using TNode class ----------------------------------
// (1)
// Initialization of GEM IPM2 data structures in coupled FMT-GEM programs 
//  that use GEMIPM2K module. Also reads in the IPM, DCH and DBR text input files. 
//  Parameters: 
//   ipmfiles_lst_name  pointer to a null-terminated C string with a path to a text file
//                      containing the list of names of  GEMIPM2K input files.
//                      Example: file "test.lst" with a content:    -t "dch.dat" "ipm.dat" "dbr-0.dat" 
//                      (-t  tells that input files are in text format)
//   nodeTypes          optional parameter used only on the TNodeArray, the initial node contents
//                      from DATABR files will be distributed among nodes in array according to the distribution list nodeTypes
//   getNodT1           optional parameter used only when reading multiple DBR files after modeling 
//                      task interruption  in GEM-Selektor 
//  Return values: 0  if successful; 1 if input file(s) were not found or corrupt; -1 if internal memory allocation error occurred. 
    long int  GEM_init( const char *ipmfiles_lst_name,
                   long int *nodeTypes = 0, bool getNodT1 = false);

#ifdef IPMGEMPLUGIN
//  Calls for direct coupling of a FMT code with GEMIPM2K

// (6) Passes (copies) the GEMIPM2K input data from the work instance of DATABR structure. 
//  This call is useful after the GEM_init() (1) and GEM_run() (2) calls to initialize the arrays which keep the
//   chemical data for all nodes used in the mass-transport model. 
   void GEM_restore_MT(
    long int  &p_NodeHandle,   // Node identification handle
    long int  &p_NodeStatusCH, // Node status code;  see typedef NODECODECH
                      //                                    GEM input output  FMT control
    double &p_TC,      // Temperature T, C                            +       -      -
    double &p_P,      // Pressure P,  Pa                              +       -      -
    double &p_Vs,     // Volume V of reactive subsystem,  m3         (+)      -      +
    double &p_Ms,     // Mass of reactive subsystem, kg               -       -      +
    double *p_bIC,    // Bulk mole amounts of IC  [nICb]              +       -      -
    double *p_dul,    // Upper restrictions to amounts of DC [nDCb]   +       -      -
    double *p_dll,    // Lower restrictions to amounts of DC [nDCb]   +       -      -
    double *p_aPH     // Specific surface areas of phases,m2/kg[nPHb] +       -      -
   );

// (8) Loads the GEMIPM2K input data for a given mass-transport node into the work instance of DATABR structure.  
//     This call is usually preceeding the GEM_run() call
void GEM_from_MT(
 long int  p_NodeHandle,   // Node identification handle
 long int  p_NodeStatusCH, // Node status code (NEED_GEM_SIA or NEED_GEM_AIA)
                  //                                              GEM input output  FMT control
 double p_TC,     // Temperature T, C                                 +       -      -
 double p_P,      // Pressure P, Pa                                   +       -      -
 double p_Vs,     // Volume V of reactive subsystem, m3               -       -      +
 double p_Ms,     // Mass of reactive subsystem, kg                   -       -      +
 double *p_bIC,   // Bulk mole amounts of IC [nICb]                   +       -      -
 double *p_dul,   // Upper restrictions to amounts of DC [nDCb]       +       -      -
 double *p_dll,   // Lower restrictions to amounts of DC [nDCb]       +       -      -
 double *p_aPH    // Specific surface areas of phases, m2/kg [nPHb]   +       -      -
 );

//(8a) Loads the GEMIPM2K input data for a given mass-transport node into the work instance of DATABR structure.
//This overloaded variant uses the xDC speciation vector for setting the 
// new bulk chemical composition to be used in the next GEM_run() calculation. 
void GEM_from_MT(  
 long int  p_NodeHandle,   // Node identification handle
 long int  p_NodeStatusCH, // Node status code (NEED_GEM_SIA or NEED_GEM_AIA)
                  //                                              GEM input output  FMT control
 double p_TC,     // Temperature T, C                                 +       -      -
 double p_P,      // Pressure P, Pa                                   +       -      -
 double p_Vs,     // Volume V of reactive subsystem, m3               -       -      +
 double p_Ms,     // Mass of reactive subsystem, kg                   -       -      +
 double *p_bIC,   // Bulk mole amounts of IC [nICb]                   +       -      -
 double *p_dul,   // Upper restrictions to amounts of DC [nDCb]       +       -      -
 double *p_dll,   // Lower restrictions to amounts of DC [nDCb]       +       -      -
 double *p_aPH,    // Specific surface areas of phases, m2/kg [nPHb]   +       -      -
 double *p_xDC    // Mole amounts of DCs [nDCb] - will be convoluted
                  // and added to the bIC GEM input vector (if full speciation
                  // and not just increments then p_bIC vector must be zeroed off -
                  // it will be calculated from p_xDC and stoichiometry matrix A
);

//(8b) Loads the GEMIPM2K input data for a given mass-transport node into the work instance of DATABR structure.
//In addition, provides access to speciation vector p_xDC and DC activity coefficients p_gam that will be used in
// GEM "smart initial approximation" SIA mode if dBR->NodeStatusCH == NEED_GEM_SIA (5) and
// uPrimalSol = true are set for the GEM_run() call (see Section 2) . This works only when the DATACH
//  structure contains a full list of Dependent Components used in GEM IPM2 calculations. 
void GEM_from_MT(
 long int  p_NodeHandle,   // Node identification handle
 long int  p_NodeStatusCH, // Node status code (NEED_GEM_SIA or NEED_GEM_AIA)
                  //                                              GEM input output  FMT control
 double p_TC,     // Temperature T, C                                 +       -      -
 double p_P,      // Pressure P, Pa                                   +       -      -
 double p_Vs,     // Volume V of reactive subsystem, m3               -       -      +
 double p_Ms,     // Mass of reactive subsystem, kg                   -       -      +
 double *p_bIC,   // Bulk mole amounts of IC [nICb]                   +       -      -
 double *p_dul,   // Upper restrictions to amounts of DC [nDCb]       +       -      -
 double *p_dll,   // Lower restrictions to amounts of DC [nDCb]       +       -      -
 double *p_aPH,   // Specific surface areas of phases, m2/kg [nPHb]   +       -      -
 double *p_xDC,  // Mole amounts of DCs [nDCb] - old primal soln.     +      -      -
 double *p_gam   // DC activity coefficients [nDCb] - old primal s.   +      -      -
);

// (9) Optional, for passing the current mass transport iteration information into the work 
// DATABR structure (e.g. for using it in tracing/debugging or in writing DBR files for nodes)
void GEM_set_MT(
//   long int  NodeTypeHY,    // Node type (hydraulic); see typedef NODETYPE
//   long int  NodeTypeMT,    // Node type (mass transport); see typedef NODETYPE
   double p_Tm,      // Actual total simulation time, s                        +       -      -
   double p_dt       // Actual time step, s                          +       -      -
);
#endif

// (5) Reads another DBR file (with input system composition, T,P etc.). The DBR file must be compatible with 
// the currently loaded IPM and DCH files (see description  of GEM_init() function call).
// Parameters:
//    fname       Null-terminated (C) string containing a full path to the input DBR disk file.
//    binary_f    Flag defining whether the file specified in fname is in text fromat (false or 0, default) or in binary format (true or 1)
// Return values:     0  if successful; 1 if input file(s) has not found been or is corrupt; -1 if internal memory allocation error occurred.
   long int GEM_read_dbr( const char* fname, bool binary_f=false );

// (2) Main call for GEM IPM calculations using the input bulk composition, temperature, pressure 
//   and metastability constraints provided in the work instance of DATABR structure.  
//   Actual calculation will be performed only when dBR->NodeStatusCH == NEED_GEM_SIA (5) or dBR->NodeStatusCH = NEED_GEM_AIA (1).
//   By other values of NodeStatusCH, no calculation will be performed and the status will remain unchanged.
//  In "smart initial approximation" (SIA) mode, the program can automatically switch into the "automatic initial
//  approximation" (AIA) mode and return  OK_GEM_AIA instead of OK_GEM_SIA.
//  The variant with one function parameter performs no internal scaling of the mass of the system. 
//   Parameters:
//   uPrimalSol  flag to define the mode of GEM smart initial approximation
//               (only if dBR->NodeStatusCH = NEED_GEM_SIA has been set before GEM_run() call).
//               false  (0) -  use speciation and activity coefficients from previous GEM_run() calculation
//               true  (1)  -  use speciation provided in the DATABR memory structure (e.g. after reading the DBR file)  
//  InternalMass Mass (kg) to which the input bulk composition (provided in DATABR memory structure) will be scaled 
//               internally during the GEM IPM calculation (results will be scaled back to the original mass).
//               Default value - 1 kg, reasonable range from 0.01 to 100 kg. This scaling is used for achieving
//               better convergence and balance accuracy of GEM IPM2 algorithm.
//  Return values:    NodeStatusCH  (the same as set in dBR->NodeStatusCH). Possible values (see "databr.h" file for the full list)
   long int  GEM_run( double InternalMass = 1., bool uPrimalSol = false  );
   long int  GEM_run( bool uPrimalSol );   // calls GEM for a work node

// Returns GEMIPM2 calculation time in seconds elapsed during the last call of GEM_run() - can be used for monitoring
//                      the performance of calculations.
// Return value:  double number, may contain 0.0 if the calculation time is less than the internal time resolution of C/C++ function
   double GEM_CalcTime();

// To obtain the number of GEM IPM2 iterations performed during the last call of GEM_run() e.g. for monitoring the
// performance of GEMIPM2K in AIA or SIA modes, or for problem diagnostics.   
// Parameters:  long int variables per reference (must be allocated before calling GEM_Iterations(), previous values will be lost. See Return values.
// Return values:
//   Function         Total number of EFD + IPM iterations from the last call to GEM_run()
//   PrecLoops        Number of performed IPM-2 precision refinement loops
//   NumIterFIA       Total number of performed EnterFeasibleDomain() (EFD) iterations to obtain a feasible initial approximation for the IPM algorithm.
//   NumIterIPM       Total number of performed IPM main descent algorithm iterations.
   long int GEM_Iterations( long int& PrecLoops, long int& NumIterFIA, long int& NumIterIPM );

// (3) Writes the contents of the work instance of the DATABR structure into a disk file with path name  fname.
//   Parameters: 
//   fname         null-terminated (C) string containing a full path to the DBR disk file to be written.
//                 NULL  - the disk file name path stored in the  dbr_file_name  field of the TNode class instance
//                 will be used, extended with ".out".  Usually the dbr_file_name field contains the path to the last input DBR file.
//   binary_f      defines if the file is to be written in binary format (true or 1, good for interruption of coupled modeling task
//                 if called in the loop for each node), or in text format (false or 0, default).
//   with_comments (text format only): defines the mode of output of comments written before each data tag and  content 
//                 in the DBR file. If set to true (1), the comments will be written for all data entries (default). 
//                 If   false (0), comments will not be written. 
//  brief_mode     if true, tells that do not write data items,  that contain only default values in text format
   void  GEM_write_dbr( const char* fname,  bool binary_f=false, 
		                  bool with_comments = true, bool brief_mode = false);

// (4) Produces a formatted text file with detailed contents (scalars and arrays) of the GEM IPM work structure. 
// This call is useful when GEM_run() returns with a NodeStatusCH value indicating a GEM calculation error
// (see  above).  Another use is for a detailed comparison of a test system calculation after the version upgrade of GEMIPM2K.
// Parameters: fname   null-terminated (C) string containing a full path to the disk file to be written.
//                     NULL  - the disk file name path stored in the  dbr_file_name  field of the TNode class instance will be used,
//                     extended with ".dump.out".  Usually the dbr_file_name field contains the path to the last input DBR file.
   void  GEM_print_ipm( const char* fname );

#ifdef IPMGEMPLUGIN
// (7)  Retrieves the GEMIPM2 chemical speciation calculation results from the work DATABR structure instance
//   into memory provided by the mass transport part. Dimensions and order of elements in the arrays must correspond
//   to those in currently existing DATACH memory structure.
   void GEM_to_MT(
   long int &p_NodeHandle,    // Node identification handle
   long int &p_NodeStatusCH,  // Node status code (changed after GEM calculation); see typedef NODECODECH
   long int &p_IterDone,      // Number of iterations performed in the last GEM IPM calculation
                         //                                                  GEM input output  FMT control
    // Chemical scalar variables
    double &p_Vs,    // Total volume V of reactive subsystem at given P,T, m3    -      -      +     +
    double &p_Ms,    // Total mass of the reactive subsystem, kg                 -      -      +     +
    double &p_Gs,    // Total Gibbs energy of the reactive subsystem, J          -      -      +     +
    double &p_Hs,    // Total enthalpy of reactive subsystem, J (reserved)       -      -      +     +
    double &p_IC,    // Effective aqueous ionic strength, molal                  -      -      +     +
    double &p_pH,    // pH of aqueous solution                                   -      -      +     +
    double &p_pe,    // pe of aqueous solution                                   -      -      +     +
    double &p_Eh,    // Eh of aqueous solution, V                                -      -      +     +
    // Dynamic data - dimensions see in DATACH.H structure
    double  *p_rMB,  // Mole balance residuals for Independent Components [nICb] -      -       +     +
    double  *p_uIC,  // Dual solution: IC chemical potentials, mol/mol [nICb]    -      -       +     +
    double  *p_xDC,  // Primal solution: DC mole amounts  [nDCb]                 -      -       +     +
    double  *p_gam,  // External activity coefficients of DC [nDCb]              -      -       +     +
    double  *p_xPH,  // Total mole amounts of all phases [nPHb]                  -      -       +     +
    double  *p_vPS,  // Total volumes of multicomponent phases, m3   [nPSb]      -      -       +     +
    double  *p_mPS,  // Total mass of multicomponent phase (carrier),kg [nPSb]   -      -       +     +
    double  *p_bPS,  // Bulk compositions of phases  [nPSb][nICb]                -      -       +     +
    double  *p_xPA   //Amount of carrier in a multicomponent asymmetric phase[nPSb]-    -       +     +
  );

#endif

// Access methods for direct or protected manipulation of CSD and DBR data
//
    DATACH* pCSD() const  // Get the pointer to chemical system definition data structure
    {     return CSD;   }

    DATABR* pCNode() const  // Get pointer to work node data structure
    {        return CNode;
    }  // usage on the level of Tnodearray is not recommended !

    // These methods get contents of fields in the work node structure
    double cTC() const     // Get current node Temperature T, C
    {  return CNode->TC;   }

    double cP() const     // Get current node Pressure P, Pa
    {        return CNode->P;   }

    // Set current node identification handle to value of parameter jj
    void setNodeHandle( long int jj )
    {      CNode->NodeHandle = jj;  }

    // Resizes the node chemical system size by factor Factor; 
    // returns new mass of the chemical system in kg
    double ResizeNode( double Factor );

// Useful methods facilitating the communication between DataCH (or FMT)
// and DataBR (or node) data structures for components and phases
// (i.e. between the chemical system definition and the node)

  //Returns DCH index of IC given the IC Name string (null-terminated) 
  // or -1 if no such name was found in the DATACH IC name list
  long int IC_name_to_xCH( const char *Name );

   // Returns DCH index of DC given the DC Name string
   // or -1 if no such name was found in the DATACH DC name list
   long int DC_name_to_xCH( const char *Name );

   // Returns DCH index of Phase given the Phase Name string
   // or -1 if no such name was found in the DATACH Phase name list
   long int Ph_name_to_xCH( const char *Name );

   //Returns DBR index of IC given the IC Name string 
   // or -1 if no such name was found in the DATACH IC name list
   inline long int IC_name_to_xDB( const char *Name )
   { return IC_xCH_to_xDB( IC_name_to_xCH( Name ) ); }

   // Returns DBR index of DC given the DC Name string
   // or -1 if no such name was found in the DATACH DC name list
   inline long int DC_name_to_xDB( const char *Name )
   { return DC_xCH_to_xDB( DC_name_to_xCH( Name ) ); }

   // Returns DBR index of Phase given the Phase Name string
   // or -1 if no such name was found in the DATACH Phase name list
   inline long int Ph_name_to_xDB( const char *Name )
   { return Ph_xCH_to_xDB( Ph_name_to_xCH( Name ) ); }

   // Converts the IC DCH index into the IC DBR index
   // or returns -1 if this IC is not used in the data bridge
   long int IC_xCH_to_xDB( const long int xCH );

   // Converts the DC DCH index into the DC DBR index
   // or returns -1 if this DC is not used in the data bridge
   long int DC_xCH_to_xDB( const long int xCH );

   // Converts the Phase DCH index into the Phase DBR index
   // or returns -1 if this Phase is not used in the data bridge
   long int Ph_xCH_to_xDB( const long int xCH );

   // Converts the IC DBR index into the IC DCH index
   inline long int IC_xDB_to_xCH( const long int xBR )
   { return CSD->xic[xBR]; }

   // Converts the DC DBR index into the DC DCH index
   inline long int DC_xDB_to_xCH( const long int xBR )
   { return CSD->xdc[xBR]; }

   // Converts the Phase DBR index into the Phase DCH index
   inline long int Ph_xDB_to_xCH( const long int xBR )
   { return CSD->xph[xBR]; }

   // Returns the DCH index of the first DC belonging to the phase with DCH index Phx 
    long int Phx_to_DCx( const long int Phx );

   // Returns the DCH index of the first DC belonging to the phase with DCH index Phx, 
   // plus returns through the nDCinPh (reference) parameter the number of DCs included into this phase 
    long int  PhtoDC_DCH( const long int Phx, long int& nDCinPh );

   // Returns the DBR index of the first DC belonging to the phase with DBR index Phx, 
   //plus returns through the nDCinPh (reference) parameter the number of DCs included into DBR for this phase
    long int  PhtoDC_DBR( const long int Phx, long int& nDCinPh );

    // Data exchange methods between GEMIPM and work node DATABR structure
    // Are called inside of GEM_run()
    void packDataBr();   //  packs GEMIPM calculation results into work node structure
    void unpackDataBr( bool uPrimalSol ); //  unpacks work DATABR content into GEMIPM data structure

    void packDataBr( double ScFact );  // Overloaded variant with scaling to constant mass of internal system
    void unpackDataBr( bool uPrimalSol, double ScFact );

    // Access to interpolated thermodynamic data from DCH structure
    // Checks if given temperature Tc and pressure P fit within the interpolation 
    //intervals of the DATACH lookup arrays (returns true) or not (returns false)
    bool  check_TP( double Tc, double P );

    // Tests Tc and P as a grid point for the interpolation of thermodynamic data using DATACH 
    // lookup arrays. Returns -1L if interpolation is needed, or 1D index of the lookup array element 
    // if Tc and P fit within the respective tolerances. 
     long int  check_grid_TP(  double Tc, double P );

     // Access to interpolated G0 from DCH structure ( xCH is the DC DCH index)
     double  DC_G0_TP( const long int xCH, double Tc, double P );

     // Access to interpolated V0 from DCH structure ( xCH is the DC DCH index)
     double  DC_V0_TP( const long int xCH, double Tc, double P );

// To be provided - access to interpolated thermodynamic data from DCH structure
//  DC_H0_TP
//  DC_S0_TP
//  DC_Cp0_TP
//  DC_DD_TP

     // retrieval of activities (xCH is the DC DCH index)
     double  DC_Activity( const long int xCH );

     // Retrieval of Phase Volume ( xBR is DBR phase index), works also for pure phases
      double  Ph_Volume( const long int xBR );
     // Retrieval of Phase mass ( xBR is DBR phase index), works also for pure phases
      double  Ph_Mass( const long int xBR );
     // Retrieval of multi-component Phase composition ( xBR is DBR phase index)
     // Returns pointer to ARout which may also be allocated inside of Ph_BC()
      double* Ph_BC( const long int xBR, double *ARout=0 );


#ifndef IPMGEMPLUGIN
// These calls are used only inside the GEMS-PSI GEM2MT module

    // Makes start DATACH and DATABR data using GEMS internal data (MULTI and other)
    // interaction variant (the user must select ICs, DCs and phases to be included
    // in DATABR lists)
    void MakeNodeStructures( QWidget* par, bool select_all,
             float *Tai, float *Pai, short nTp_ = 1 ,
             short nPp_ = 1 , float Ttol_ = 1., float Ptol_ =1. );

    // Overloaded variant - takes lists of ICs, DCs and phases according to
    // already existing index vectors axIC, axDC, axPH (with anICb, anDCb,
    // anPHb, respectively)
    void MakeNodeStructures(  short anICb, short anDCb,  short anPHb,
    		short* axIC, short* axDC,  short* axPH,
             float* Tai, float* Pai,  short nTp_,
             short nPp_, float Ttol_, float Ptol_  );

#endif
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Data direct access macroses (for FMT programs written in C++)
// Work on both sides of assignment - use with caution!
//
// Molar mass of Independent Component with node DATABRIDGE index ICx
#define nodeCH_ICmm( ICx )  (  TNode::na->pCSD()->ICmm[ \
                               TNode::na->pCSD()->xic[(ICx)]] )

// Molar mass of Dependent Component with node DATABRIDGE index DCx
#define nodeCH_DCmm( DCx )  (  TNode::na->pCSD()->DCmm[ \
                               TNode::na->pCSD()->xdc[(DCx)]] )

// Redo into a function with interpolation
// Diffusion coefficient of dependent component with node DBr index ICx
// #define nodeCH_DD( DCx )    ( TNode::na->pCSD()->DD[
//                              TNode::na->pCSD()->xDC[(DCx)]] )

// stoichiometry coefficient A[j][i] of IC with node DATABRIDGE index ICx
// in the formula of DC with node index DCx
#define nodeCH_A( DCx, ICx )  ( (TNode::na->pCSD()->A[ \
                                 (TNode::na->pCSD()->xic[(ICx)])+ \
                                 (TNode::na->pCSD()->xdc[(DCx)]) * \
                                  TNode::na->pCSD()->nIC]) )

// more will be added soon!

#endif
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// _node_h_

