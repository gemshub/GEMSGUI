//-------------------------------------------------------------------
// $Id: node.cpp 684 2005-11-23 13:17:15Z gems $
//
// Implementation of Tnode class including initialization and
// execution of GEMIPM2 kernel

// Works whith DATACH and DATABR structures
//
// Copyright (C) 2005,2008 S.Dmytriyeva, D.Kulik
//
// This file is part of a GEM-Selektor library for thermodynamic
// modelling by Gibbs energy minimization and of GEMIPM2K code
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------

#include "node.h"
#include "gdatastream.h"
#include "num_methods.h"
#include <math.h>
#include <algorithm>

#ifndef __unix
#include <io.h>
#endif

#ifndef IPMGEMPLUGIN
  #include "service.h"
  #include "visor.h"
#else
  istream& f_getline(istream& is, gstring& str, char delim);
#endif

TNode* TNode::na;

// Conversion factors
const double bar_to_Pa = 1e5,
               m3_to_cm3 = 1e6,
               g_to_kg = 1e-3;


// Checks if given temperature Tc and pressure P fit within the interpolation 
//intervals of the DATACH lookup arrays (returns true) or not (returns false)
bool  TNode::check_TP( double Tc, double P )
{
   bool okT = true, okP = true;
   double T_, P_;

   if( Tc <= CSD->TCval[0] - CSD->Ttol )
   { 				// Lower boundary of T interpolation interval
	 okT = false;
     T_ = CSD->TCval[0] - CSD->Ttol;
   }
   if( Tc >= CSD->TCval[CSD->nTp-1] + CSD->Ttol )
   {
	 okT = false;
     T_ = CSD->TCval[CSD->nTp-1] + CSD->Ttol;
   }
   if( okT == false )
   {
     fstream f_log("ipmlog.txt", ios::out|ios::app );
     f_log << "In node "<< CNode->NodeHandle << ",  Given TC= "<<  Tc <<
             "  is beyond the interpolation range for thermodynamic data near boundary T_= "
     		<< T_ << endl;
   }

   if( P <= CSD->Pval[0] - CSD->Ptol )
   {
	  okP = false;
      P_ = CSD->Pval[0] - CSD->Ptol;
   }
   if( P >= CSD->Pval[CSD->nPp-1] + CSD->Ptol )
   {
	  okP = false;
      P_ = CSD->Pval[CSD->nPp-1] + CSD->Ptol;
   }
   if( !okP )
   {
     fstream f_log("ipmlog.txt", ios::out|ios::app );
     f_log << "In node "<< CNode->NodeHandle << ", Given P= "<<  P <<
           "  is beyond the interpolation range for thermodynamic data near boundary P_= "
           << P_ << endl;
   }
   return ( okT && okP );
}

//-------------------------------------------------------------------------------------------------------------------------------
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
long int TNode::GEM_run( double InternalMass,  bool uPrimalSol )
{
  CalcTime = 0.0;
  PrecLoops = 0; NumIterFIA = 0; NumIterIPM = 0;
  
  double mass_temp=0.0, ScFact=0.0;
  long int i;

  for (i=0;i<CSD->nICb;i++)
  {
	mass_temp += CNode->bIC[i]*CSD->ICmm[IC_xDB_to_xCH(i)];
  }
 // mass_temp *= 1e-3;
  CNode->Ms=mass_temp;
  ScFact = InternalMass/mass_temp; // changed to scaling via newly calculated mass from bIC vector,

  try
  {
// f_log << " GEM_run() begin Mode= " << p_NodeStatusCH endl;
//---------------------------------------------
// Checking T and P  for interpolation intervals
   check_TP( CNode->TC, CNode->P);
// Unpacking work DATABR structure into MULTI (GEM IPM structure): uses DATACH
// setting up up PIA or AIA mode
   if( CNode->NodeStatusCH == NEED_GEM_SIA )
   {
	   pmm->pNP = 1;
	   unpackDataBr( uPrimalSol, ScFact );
   }
   else if( CNode->NodeStatusCH == NEED_GEM_AIA )
         {
	        pmm->pNP = 0; // As default setting AIA mode
	       unpackDataBr( false, ScFact );
         }
        else 
	           return CNode->NodeStatusCH;
   // GEM IPM calculation of equilibrium state in MULTI
   CalcTime = TProfil::pm->calcMulti( PrecLoops, NumIterFIA, NumIterIPM );
   // Extracting and packing GEM IPM results into work DATABR structure
    packDataBr( ScFact );
    CNode->IterDone = NumIterFIA+NumIterIPM;
//**************************************************************
// only for testing output results for files
//    GEM_write_dbr( "calculated_dbr.dat",  false );
//    GEM_print_ipm( "calc_multi.ipm" );
// *********************************************************
    // test error result GEM IPM calculation of equilibrium state in MULTI
    long int erCode = TProfil::pm->testMulti();

    if( erCode )
    {
        if( CNode->NodeStatusCH  == NEED_GEM_AIA )
          CNode->NodeStatusCH = BAD_GEM_AIA;
        else
          CNode->NodeStatusCH = BAD_GEM_SIA;
    }
    else
    {
      if( CNode->NodeStatusCH  == NEED_GEM_AIA )
          CNode->NodeStatusCH = OK_GEM_AIA;
      else
         CNode->NodeStatusCH = OK_GEM_SIA;
    }
   }
   catch(TError& err)
    {
	if( TProfil::pm->pa.p.PSM  )
	{
			fstream f_log("ipmlog.txt", ios::out|ios::app );
	        f_log << "Error Node:" << CNode->NodeHandle << ":time:" << CNode->Tm << ":dt:" << CNode->dt<< ": " <<
	          err.title.c_str() << ":" << endl;
	       if( TProfil::pm->pa.p.PSM == 2  )
	          f_log  << err.mess.c_str() << endl;
	}
     if( CNode->NodeStatusCH  == NEED_GEM_AIA )
       CNode->NodeStatusCH = ERR_GEM_AIA;
     else
       CNode->NodeStatusCH = ERR_GEM_SIA;

    }
    catch(...)
    {
     fstream f_log("ipmlog.txt", ios::out|ios::app );
     f_log << "Node:" << CNode->NodeHandle << ":time:" << CNode->Tm << ":dt:" << CNode->dt<< ": "
    		<< "gems2: Unknown exception: GEM calculation aborted" << endl;
     CNode->NodeStatusCH = T_ERROR_GEM;
    }
   return CNode->NodeStatusCH;
}

long int TNode::GEM_run( bool uPrimalSol )
{
  CalcTime = 0.0;
  PrecLoops = 0; NumIterFIA = 0; NumIterIPM = 0;
//
  try
  {
// f_log << " GEM_run() begin Mode= " << p_NodeStatusCH endl;
//---------------------------------------------
// Checking T and P  for interpolation intervals
   check_TP( CNode->TC, CNode->P);
// Unpacking work DATABR structure into MULTI (GEM IPM structure): uses DATACH
// setting up up PIA or AIA mode
   if( CNode->NodeStatusCH == NEED_GEM_SIA )
   {
	   pmm->pNP = 1;
	   unpackDataBr( uPrimalSol );
   }
   else if( CNode->NodeStatusCH == NEED_GEM_AIA )
	     {  pmm->pNP = 0; // As default setting AIA mode
	        unpackDataBr( false );
         }
        else
	       return CNode->NodeStatusCH;
   // GEM IPM calculation of equilibrium state in MULTI
   CalcTime = TProfil::pm->calcMulti( PrecLoops, NumIterFIA, NumIterIPM );
// Extracting and packing GEM IPM results into work DATABR structure
    packDataBr();
    CNode->IterDone = NumIterFIA+NumIterIPM;
//**************************************************************
// only for testing output results for files
//    GEM_write_dbr( "calculated_dbr.dat",  false );
//    GEM_print_ipm( "calc_multi.ipm" );
// *********************************************************

    // test error result GEM IPM calculation of equilibrium state in MULTI
    long int erCode = TProfil::pm->testMulti();

    if( erCode )
    {
        if( CNode->NodeStatusCH  == NEED_GEM_AIA )
          CNode->NodeStatusCH = BAD_GEM_AIA;
        else
          CNode->NodeStatusCH = BAD_GEM_SIA;
    }
    else
    {
      if( CNode->NodeStatusCH  == NEED_GEM_AIA )
          CNode->NodeStatusCH = OK_GEM_AIA;
      else
         CNode->NodeStatusCH = OK_GEM_SIA;
    }

   }
   catch(TError& err)
   {
	if( TProfil::pm->pa.p.PSM  )
	{
		fstream f_log("ipmlog.txt", ios::out|ios::app );
        f_log << "Error Node:" << CNode->NodeHandle << ":time:" << CNode->Tm << ":dt:" << CNode->dt<< ": " <<
          err.title.c_str() << ":" << endl;
       if( TProfil::pm->pa.p.PSM == 2  )
          f_log  << err.mess.c_str() << endl;
	}
    if( CNode->NodeStatusCH  == NEED_GEM_AIA )
      CNode->NodeStatusCH = ERR_GEM_AIA;
    else
      CNode->NodeStatusCH = ERR_GEM_SIA;

   }
   catch(...)
   {
    fstream f_log("ipmlog.txt", ios::out|ios::app );
    f_log << "Node:" << CNode->NodeHandle << ":time:" << CNode->Tm << ":dt:" << CNode->dt<< ": "
   		<< "gems2: Unknown exception: GEM calculation aborted" << endl;
    CNode->NodeStatusCH = T_ERROR_GEM;
    }
   return CNode->NodeStatusCH;
}



// Returns GEMIPM2 calculation time in seconds elapsed during the last call of GEM_run() - 
// can be used for monitoring the performance of calculations.
// Return value:  double number, may contain 0.0 if the calculation time is less than the 
//                internal time resolution of C/C++ function
double TNode::GEM_CalcTime()
{
  return CalcTime;
}

// To obtain the number of GEM IPM2 iterations performed during the last call of GEM_run() e.g. for monitoring the
// performance of GEMIPM2K in AIA or SIA modes, or for problem diagnostics.   
// Parameters:  long int variables per reference (must be allocated before calling GEM_Iterations(), previous values will be lost. See Return values.
// Return values:
//   Function         Total number of EFD + IPM iterations from the last call to GEM_run()
//   PrecLoops        Number of performed IPM-2 precision refinement loops
//   NumIterFIA       Total number of performed EnterFeasibleDomain() (EFD) iterations to obtain a feasible initial approximation for the IPM algorithm.
//   NumIterIPM       Total number of performed IPM main descent algorithm iterations.
long int TNode::GEM_Iterations( long int& PrecLoops_, long int& NumIterFIA_, long int& NumIterIPM_ )
{
	PrecLoops_ = PrecLoops;
	NumIterFIA_ = NumIterFIA;
	NumIterIPM_ = NumIterIPM;
	return NumIterFIA+NumIterIPM;
}

// (5) Reads another DBR file (with input system composition, T,P etc.). The DBR file must be compatible with 
// the currently loaded IPM and DCH files (see description  of GEM_init() function call).
// Parameters:
//    fname       Null-terminated (C) string containing a full path to the input DBR disk file.
//    binary_f    Flag defining whether the file specified in fname is in text fromat (false or 0, default) or in binary format (true or 1)
// Return values:     0  if successful; 1 if input file(s) has not found been or is corrupt; -1 if internal memory allocation error occurred. 
long int  TNode::GEM_read_dbr( const char* fname, bool binary_f )
{
  try
  {
    if( binary_f )
	{
       gstring str_file = fname;
	   GemDataStream in_br(str_file, ios::in|ios::binary);
       databr_from_file(in_br);
	}
   else
   {   fstream in_br(fname, ios::in );
       ErrorIf( !in_br.good() , fname, "DataBR Fileopen error");
       databr_from_text_file(in_br);
   }

    dbr_file_name = fname;

  } catch(TError& err)
    {
	  fstream f_log("ipmlog.txt", ios::out|ios::app );
      f_log << "Error Node:" << CNode->NodeHandle << ":time:" << CNode->Tm << ":dt:" << CNode->dt<< ": " <<
        err.title.c_str() << ":" <<  err.mess.c_str() << endl;
      return 1;
    }
    catch(...)
    {
      return -1;
    }
  return 0;
}

//-------------------------------------------------------------------
// (1) Initialization of GEM IPM2 data structures in coupled FMT-GEM programs 
//  that use GEMIPM2K module. Also reads in the IPM, DCH and DBR text input files. 
//  Parameters:
//  ipmfiles_lst_name - name of a text file that contains:
//    " -t/-b <DCH_DAT file name> <IPM_DAT file name> <dataBR file name1>,
//      ... , <dataBR file nameN> "
//    These files (one DCH_DAT, one IPM_DAT, and at least one dataBR file) must
//    exist in the same directory where the ipmfiles_lst_name file is located.
//    the DBR_DAT files in the above list are indexed as 1, 2, ... N (node handles)
//    and must contain valid initial chemical systems (of the same structure
//    as described in the DCH_DAT file) to set up the initial state of the FMT
//    node array. If -t flag or nothing is specified then all data files must
//    be in text (ASCII) format; if -b flag is specified then all data files
//    are  assumed to be binary (little-endian) files.
//  nodeTypes[nNodes] - optional parameter used only on the TNodeArray level,
//    array of node type (fortran) indexes of DBR_DAT files
//    in the ipmfiles_lst_name list. This array (handle for each FMT node),
//    specifies from which DBR_DAT file the initial chemical system should
//    be taken.
//  getNodT1 - optional flag, used only (if true) when reading multiple DBR files
//    after the coupled modeling task interruption in GEM-Selektor
//  This function returns:
//   0: OK; 1: GEM IPM read file error; -1: System error (e.g. memory allocation)
//
//-------------------------------------------------------------------
long int  TNode::GEM_init( const char* ipmfiles_lst_name,
                          long int* nodeTypes, bool getNodT1)
{
  long int i;
#ifdef IPMGEMPLUGIN
  fstream f_log("ipmlog.txt", ios::out|ios::app );
  try
    {
#else
      size_t npos = gstring::npos;
#endif
     bool binary_f = false;
     gstring lst_in = ipmfiles_lst_name;

// Get path
      size_t pos = lst_in.rfind("/");
      gstring Path = "";
      if( pos < npos )
       Path = lst_in.substr(0, pos+1);

//  open file stream for the file names list file
      fstream f_lst( lst_in.c_str(), ios::in );
      ErrorIf( !f_lst.good() , lst_in.c_str(), "Fileopen error");

      gstring datachbr_fn;
      f_getline( f_lst, datachbr_fn, ' ');

//  Syntax: -t/-b  "<DCH_DAT file name>"  "<IPM_DAT file name>"
//       "<DBR_DAT file1 name>" [, ... , "<DBR_DAT fileN name>"]

//Testing flag "-t" or "-b" (by default "-t")   // use binary or text files
      pos = datachbr_fn.find( '-');
      if( pos != /*gstring::*/npos )
      {
         if( datachbr_fn[pos+1] == 'b' )
            binary_f = true;
         f_getline( f_lst, datachbr_fn, ' ');
      }

      // Reading name of DCH_DAT file
      gstring dat_ch = Path + datachbr_fn;

      // Reading name of IPM_DAT file for structure MULTI (GEM IPM work structure)
      f_getline( f_lst, datachbr_fn, ' ');
      gstring mult_in = Path + datachbr_fn;

// Reading DCH_DAT file in binary or text format
      if( binary_f )
      {  GemDataStream f_ch(dat_ch, ios::in|ios::binary);
         datach_from_file(f_ch);
       }
      else
      { fstream f_ch(dat_ch.c_str(), ios::in );
         ErrorIf( !f_ch.good() , dat_ch.c_str(), "DCH_DAT fileopen error");
         datach_from_text_file(f_ch);
      }

// Reading IPM_DAT file into structure MULTI (GEM IPM work structure)
if( binary_f )
 {
   GemDataStream f_m(mult_in, ios::in|ios::binary);
#ifdef IPMGEMPLUGIN
    profil->readMulti(f_m);
#else
    TProfil::pm->readMulti(f_m);
#endif
  }
  else
  {
#ifdef IPMGEMPLUGIN
        profil->readMulti(mult_in.c_str());
#else
    TProfil::pm->readMulti(mult_in.c_str());
#endif
  }

// Prepare for reading DBR_DAT files
     i = 0;
     while( !f_lst.eof() )  // For all DBR_DAT files listed
     {

#ifndef IPMGEMPLUGIN
   pVisor->Message( 0, "GEM2MT node array",
      "Reading from disk a set of node array files to resume an interrupted RMT task.\n"
           "Please, wait...", i, nNodes() );
#endif

// Reading DBR_DAT file into work DATABR structure
         if( i )  // Comma only after the first DBR_DAT file!
            f_getline( f_lst, datachbr_fn, ',');
         else
            f_getline( f_lst, datachbr_fn, ' ');

         gstring dbr_file = Path + datachbr_fn;
         if( binary_f )
         {
             GemDataStream in_br(dbr_file, ios::in|ios::binary);
             databr_from_file(in_br);
          }
         else
          {   fstream in_br(dbr_file.c_str(), ios::in );
                 ErrorIf( !in_br.good() , datachbr_fn.c_str(),
                    "DBR_DAT fileopen error");
               databr_from_text_file(in_br);
          }
          if(!i)
        	  dbr_file_name = dbr_file;
// Unpacking work DATABR structure into MULTI (GEM IPM work structure): uses DATACH
//    unpackDataBr();

#ifndef IPMGEMPLUGIN
        if( getNodT1 )  // optional parameter used only when reading multiple
        	// DBR files after coupled modeling task interruption in GEM-Selektor
        {
           setNodeArray( dbr_file, i, binary_f );
        }
        else
#endif
        {
// Copying data from work DATABR structure into the node array
// (as specified in nodeTypes array)
           setNodeArray( i, nodeTypes  );
         }
          i++;
     }  // end while()
#ifndef IPMGEMPLUGIN
   pVisor->CloseMessage();
#endif

    ErrorIf( i==0, datachbr_fn.c_str(), "GEM_init() error: No DBR_DAT files read!" );
    checkNodeArray( i, nodeTypes, datachbr_fn.c_str()  );

   return 0;

#ifdef IPMGEMPLUGIN
    }
    catch(TError& err)
    {
      f_log << err.title.c_str() << "  : " << err.mess.c_str() << endl;
    }
    catch(...)
    {
        return -1;
    }
    return 1;
#endif
}

//-----------------------------------------------------------------
// work with lists

//Returns DCH index of IC given the IC Name string (null-terminated) 
// or -1 if no such name was found in the DATACH IC name list
long int TNode::IC_name_to_xCH( const char *Name )
{
  long int ii, len = strlen( Name );
  len =  min(len,MaxICN);

  for(ii = 0; ii<CSD->nIC; ii++ )
       if(!memcmp(Name, CSD->ICNL[ii], len ))
        if( len == MaxICN || CSD->ICNL[ii][len] == ' ' || CSD->ICNL[ii][len] == '\0' )
         return ii;
  return -1;
}

// Returns DCH index of DC given the DC Name string
// or -1 if no such name was found in the DATACH DC name list
 long int TNode::DC_name_to_xCH( const char *Name )
 {
  long int ii, len = strlen( Name );
  len =  min(len,MaxDCN);

  for( ii = 0; ii<CSD->nDC; ii++ )
       if(!memcmp(Name, CSD->DCNL[ii], min(len,MaxDCN)))
        if( len == MaxDCN || CSD->DCNL[ii][len] == ' ' || CSD->DCNL[ii][len] == '\0' )
         return ii;
  return -1;
 }

// Returns DCH index of Phase given the Phase Name string
// or -1 if no such name was found in the DATACH Phase name list
long int TNode::Ph_name_to_xCH( const char *Name )
{
  long int ii, len = strlen( Name );
  len =  min(len,MaxPHN);

  for( ii = 0; ii<CSD->nPH; ii++ )
       if(!memcmp(Name, CSD->PHNL[ii], min(len,MaxPHN)))
        if( len == MaxPHN || CSD->PHNL[ii][len] == ' ' || CSD->PHNL[ii][len] == '\0' )
         return ii;
  return -1;
}

// Converts the IC DCH index into the IC DBR index
// or returns -1 if this IC is not used in the data bridge
long int TNode::IC_xCH_to_xDB( const long int xCH )
{
  for(long int ii = 0; ii<CSD->nICb; ii++ )
       if( CSD->xic[ii] == xCH )
         return ii;
  return -1;
}

// Converts the DC DCH index into the DC DBR index
// or returns -1 if this DC is not used in the data bridge
long int TNode::DC_xCH_to_xDB( const long int xCH )
{
  for(long int ii = 0; ii<CSD->nDCb; ii++ )
       if( CSD->xdc[ii] == xCH )
         return ii;
  return -1;
}

// Converts the Phase DCH index into the Phase DBR index
// or returns -1 if this Phase is not used in the data bridge
long int TNode::Ph_xCH_to_xDB( const long int xCH )
{
  for(long int ii = 0; ii<CSD->nPHb; ii++ )
       if( CSD->xph[ii] == xCH )
         return ii;
  return -1;
}

// Returns the DCH index of the first DC belonging to the phase with DCH index Phx 
 long int  TNode::Phx_to_DCx( const long int Phx )
 {
   long int k, DCx = 0;
   for( k=0; k<CSD->nPHb; k++ )
   {
     if( k == Phx )
      break;
     DCx += CSD->nDCinPH[ k];
   }
   return DCx;
 }

 // Returns the DCH index of the first DC belonging to the phase with DCH index Phx, 
 // plus returns through the nDCinPh (reference) parameter the number of DCs included into this phase 
 long int  TNode::PhtoDC_DCH( const long int Phx, long int& nDCinPh )
 {
   long int k, DCx = 0;
   for( k=0; k<CSD->nPHb; k++ )
   {
     if( k == Phx )
      break;
     DCx += CSD->nDCinPH[ k];
   }
   nDCinPh = CSD->nDCinPH[k];
   return DCx;
 }

// Returns the DBR index of the first DC belonging to the phase with DBR index Phx, 
//plus returns through the nDCinPh (reference) parameter the number of DCs included into DBR for this phase
 long int  TNode::PhtoDC_DBR( const long int Phx, long int& nDCinPh )
 {
   long int ii, DCx, DCxCH, PhxCH, nDCinPhCH;

   PhxCH = Ph_xDB_to_xCH( Phx );
   DCxCH = PhtoDC_DCH( PhxCH, nDCinPhCH );

   DCx = -1;
   nDCinPh = 0;
   for( ii = 0; ii<CSD->nDCb; ii++ )
   {
      if( CSD->xdc[ii] >= DCxCH )
      {
        if( CSD->xdc[ii] >= DCxCH+nDCinPhCH  )
          break;
        nDCinPh++;
        if( DCx == -1)
          DCx = ii;
      }
   }
   return DCx;
 }

 // Test Tc as lying in the vicinity of a grid point for the interpolation of thermodynamic data
 // Return index of the node in lookup array or -1
 long int  TNode::check_grid_T( double Tc )
 {
   long int jj;
   for( jj=0; jj<CSD->nTp; jj++)
     if( fabs( Tc - CSD->TCval[jj] ) < CSD->Ttol )
        return jj;
   return -1;
 }

 // Test P as lying in the vicinity of a grid point for the interpolation of thermodynamic data
 // Return index of the node in lookup array or -1
 long int  TNode::check_grid_P( double P )
 {
   long int jj;
   for( jj=0; jj<CSD->nPp; jj++)
     if( fabs( P - CSD->Pval[jj] ) < CSD->Ptol )
       return jj;
   return -1;
 }

 // Tests Tc and P as a grid point for the interpolation of thermodynamic data using DATACH 
 // lookup arrays. Returns -1L if interpolation is needed, or 1D index of the lookup array element 
 // if Tc and P fit within the respective tolerances. 
 // For producing lookup arrays (in GEMS), we recommend using step for temperature less or equal to 10 degrees 
 // in order to assure good accuracy of interpolation especially for S0 and Cp0 of aqueous species.     
  long int  TNode::check_grid_TP(  double Tc, double P )
  {
    long int xT, xP, ndx=-1;

    xT = check_grid_T( Tc );
    xP = check_grid_P( P );
    if( xT >=0 && xP>= 0 )
     ndx =  xP * CSD->nTp + xT;
    return ndx;
  }

 // Returns the (interpolated) G0 value for Tc, P from the DCH structure in J/mol
 //    ( xCH is the DC index in DATACH)
 //  In the case of error (e.g. Tc and P out of range) returns 7777777.
  double  TNode::DC_G0_TP( const long int xCH, double Tc, double P )
  {
    long int xTP, jj;
    double G0;

    if( check_TP( Tc, P ) == false )
    	return 7777777.;

    xTP = check_grid_TP( Tc, P );
    jj =  xCH * CSD->nPp * CSD->nTp;

    if( xTP >= 0 )
       G0 = CSD->G0[ jj + xTP ];
    else
       G0 = LagranInterp( CSD->Pval, CSD->TCval, CSD->G0+jj,
               P, Tc, CSD->nTp, CSD->nPp, 1 );
    return G0;
 }

  // Access to interpolated V0 for Tc, P from the DCH structure (in J/Pa)
  //   (xCH the DC DCH index)
  // If error (e.g. Tc or P out of range) returns -777
  double  TNode::DC_V0_TP( const long int xCH, double Tc, double P )
  {
    long int xTP, jj;
    double V0;

    if( check_TP( Tc, P ) == false )
    	return -777.;

    xTP = check_grid_TP( Tc, P );
    jj =  xCH * CSD->nPp * CSD->nTp;

    if( xTP >= 0 )
       V0 = CSD->V0[ jj + xTP ];
    else
       V0 = LagranInterp( CSD->Pval, CSD->TCval, CSD->V0+jj,
                P, Tc, CSD->nTp, CSD->nPp, 1 );
    return V0;
}

 // Retrieval of Phase Volume ( xBR the Ph DBR index)
 double  TNode::Ph_Volume( const long int xBR )
 {
   double vol;
   if( xBR < CSD->nPSb )
    vol = CNode->vPS[xBR];
   else
   {
     long int xDC = Phx_to_DCx( Ph_xDB_to_xCH( xBR ));
     vol = DC_V0_TP( xDC, CNode->TC, CNode->P )*10.; // from J/bar to cm3/mol
     vol *= CNode->xDC[DC_xCH_to_xDB(xDC)];
   }
   return vol;
 }

  // Retrieval of Phase mass kg ( xBR the Ph DBR index)
  double  TNode::Ph_Mass( const long int xBR )
  {
     double mass;
     if( xBR < CSD->nPSb )
        mass = CNode->mPS[xBR];
     else
     {
        long int xDC = Phx_to_DCx( Ph_xDB_to_xCH( xBR ));
        mass = CNode->xDC[ DC_xCH_to_xDB(xDC) ] * CSD->DCmm[xDC];
     }
    return mass;
  }

  //kg44 Retrieval of activity ( xCH is the DCH index of the dependent component)
  double  TNode::DC_Activity( const long int xCH )
   {
	return 	pow(10.0,pmm->Y_la[xCH]);
   }

  // Retrieval of bulk Phase composition ( xBR the Ph DBR index), works also for pure phases
  double *TNode::Ph_BC( const long int xBR, double* ARout )
  {
    long int ii;
    if( !ARout )
      ARout = new double[ CSD->nICb ];   // Potential memory leak ! ! ! ! ! ! ! !

    if( xBR < CSD->nPSb )
       for( ii=0; ii<pCSD()->nICb; ii++ )
         ARout[ii] = CNode->bPS[ xBR * CSD->nICb + ii ];
    else
    {
      long int DCx = Phx_to_DCx( Ph_xDB_to_xCH(xBR) );
      for( ii=0; ii<pCSD()->nICb; ii++ )
      {
         ARout[ii] = CSD->A[ IC_xDB_to_xCH(ii) + DCx * CSD->nIC];
         ARout[ii] *= CNode->xDC[ DC_xCH_to_xDB(DCx) ];
      }
    }
    return ARout;
  }

//---------------------------------------------------------//

void TNode::allocMemory()
{
// memory allocation for data bridge structures
    CSD = new DATACH;
    CNode = new DATABR;

    // mem_set( CSD, 0, sizeof(DATACH) );
    datach_reset();
    // mem_set( CNode, 0, sizeof(DATABR) );
    databr_reset( CNode, 2 );

#ifdef IPMGEMPLUGIN
// internal structures
    multi = new TMulti();
    pmm = multi->GetPM();
    profil = new TProfil( multi );
    TProfil::pm = profil;
#endif
}

void TNode::freeMemory()
{
   datach_free();
   CSD = 0;
   CNode = databr_free( CNode );

#ifdef IPMGEMPLUGIN
  delete multi;
  delete profil;
#endif
}

#ifndef IPMGEMPLUGIN

// Makes start DATACH and DATABR data from GEMS internal data (MULTI and other)
void TNode::MakeNodeStructures(
		short anICb,       // number of stoichiometry units (<= nIC) used in the data bridge
		short anDCb,      	// number of DC (chemical species, <= nDC) used in the data bridge
		short anPHb,     	// number of phases (<= nPH) used in the data bridge
		short* axIC,   // ICNL indices in DATABR IC vectors [nICb]
		short* axDC,   // DCNL indices in DATABR DC list [nDCb]
		short* axPH,   // PHNL indices in DATABR phase vectors [nPHb]
    float* Tai, float* Pai,
    short nTp_, short nPp_, float Ttol_, float Ptol_  )
{
  int ii;
  TCIntArray aSelIC;
  TCIntArray aSelDC;
  TCIntArray aSelPH;

// make lists
  for( ii=0; ii<anICb; ii++)
     aSelIC.Add( axIC[ii] );
  for( ii=0; ii<anDCb; ii++)
     aSelDC.Add( axDC[ii] );
  for( ii=0; ii<anPHb; ii++)
     aSelPH.Add( axPH[ii] );

// set default data and realloc arrays
   makeStartDataChBR( 0, aSelIC, aSelDC, aSelPH,
                      nTp_, nPp_, Ttol_, Ptol_, Tai, Pai );
}

// Make start DATACH and DATABR data from GEMS internal data (MULTI and other)
void TNode::MakeNodeStructures( QWidget* par, bool select_all,
    float *Tai, float *Pai,
    short nTp_, short nPp_, float Ttol_, float Ptol_  )
{

  //MULTI *mult = TProfil::pm->pmp;
  TCStringArray aList;
  TCIntArray aSelIC;
  TCIntArray aSelDC;
  TCIntArray aSelPH;

// select lists
    aList.Clear();
    for(int ii=0; ii< pmm->N; ii++ )
    {  if( select_all )
         aSelIC.Add( ii );
       else
         aList.Add( gstring( pmm->SB[ii], 0, MAXICNAME+MAXSYMB));
    }
    if( !select_all  )
      aSelIC = vfMultiChoice(par, aList,
          "Please, mark independent components for selection into DataBridge");

    aList.Clear();
    for(int ii=0; ii< pmm->L; ii++ )
    {  if( select_all )
         aSelDC.Add( ii );
       else
       aList.Add( gstring( pmm->SM[ii], 0, MAXDCNAME));
    }
    if( !select_all  )
       aSelDC = vfMultiChoice(par, aList,
         "Please, mark dependent components for selection into DataBridge");

    aList.Clear();
    for(int ii=0; ii< pmm->FI; ii++ )
    {  if( select_all )
         aSelPH.Add( ii );
       else
       aList.Add( gstring( pmm->SF[ii], 0, MAXPHNAME+MAXSYMB));
    }
    if( !select_all  )
       aSelPH = vfMultiChoice(par, aList,
         "Please, mark phases for selection into DataBridge");


// set default data and realloc arrays
   makeStartDataChBR( par, aSelIC, aSelDC, aSelPH,
                      nTp_, nPp_, Ttol_, Ptol_, Tai, Pai );
}


// Writing dataCH structure to binary file
void TNode::makeStartDataChBR( QWidget* par,
  TCIntArray& selIC, TCIntArray& selDC, TCIntArray& selPH,
  short nTp_, short nPp_, float Ttol_, float Ptol_,
  float *Tai, float *Pai )
{
// set sizes for DataCh
  uint ii;
  int i1;
// reallocates memory for     DATACH  *CSD;  and  DATABR  *CNode;
  if( !CSD )
     CSD = new DATACH;
  if( !CNode )
     CNode = new DATABR;

  CSD->nIC = pmm->N;
  CSD->nDC = pmm->L;
  CSD->nDCs = pmm->Ls;
  CSD->nPH = pmm->FI;
  CSD->nPS = pmm->FIs;
  CSD->nTp = nTp_;
  CSD->nPp = nPp_;
  if( pmm->Aalp )
    CSD->nAalp = 1;
  else
    CSD->nAalp = 0;
  CSD->iGrd = 0;

// These dimensionalities define sizes of dynamic data in DATABR structure!!!

  CSD->nICb = (long int)selIC.GetCount();
  CSD->nDCb = (long int)selDC.GetCount();
  CSD->nPHb = (long int)selPH.GetCount();
  CSD->nPSb = 0;
  for( ii=0; ii< selPH.GetCount(); ii++, CSD->nPSb++ )
   if( selPH[ii] >= pmm->FIs )
       break;
  CSD->uRes1 = 0;
  CSD->dRes1 = 0.;
  CSD->dRes2 = 0.;

  CSD->Ttol = Ttol_;
  CSD->Ptol = Ptol_*bar_to_Pa;

// realloc structures DataCh&DataBr

  datach_realloc();
  databr_realloc();

// set dynamic data to DataCH

  for( ii=0; ii< selIC.GetCount(); ii++ )
    CSD->xic[ii] = (long int)selIC[ii];
  for( ii=0; ii< selDC.GetCount(); ii++ )
    CSD->xdc[ii] = (long int)selDC[ii];
  for( ii=0; ii< selPH.GetCount(); ii++ )
    CSD->xph[ii] = (long int)selPH[ii];

  for( i1=0; i1< CSD->nIC*CSD->nDC; i1++ )
    CSD->A[i1] = pmm->A[i1];

  for( i1=0; i1< CSD->nPH; i1++ )
  {
    CSD->nDCinPH[i1] = pmm->L1[i1];
    CSD->ccPH[i1] = pmm->PHC[i1];
    fillValue( CSD->PHNL[i1], ' ', MaxPHN );
    copyValues( CSD->PHNL[i1], pmm->SF[i1]+MAXSYMB, min(MaxPHN,(long int)MAXPHNAME) );
  }
  for( i1=0; i1< CSD->nIC; i1++ )
  {
    CSD->ICmm[i1] = pmm->Awt[i1]*g_to_kg;
    CSD->ccIC[i1] = pmm->ICC[i1];
    copyValues( CSD->ICNL[i1], pmm->SB[i1] , min(MaxICN,(long int)MAXICNAME) );
  }
  for( i1=0; i1< CSD->nDC; i1++ )
  {
    CSD->DCmm[i1] = pmm->MM[i1]*g_to_kg;
    CSD->ccDC[i1] = pmm->DCC[i1];
    copyValues( CSD->DCNL[i1], pmm->SM[i1] , min(MaxDCN,(long int)MAXDCNAME) );
  }
//  fillValue( CSD->DD, 0., CSD->nDCs);

// set default data to DataBr
  // mem_set( &CNode->TC, 0, 32*sizeof(double));
  // CNode->NodeHandle = 0;
  // CNode->NodeTypeHY = normal;
  // CNode->NodeTypeMT = normal;
  // CNode->NodeStatusFMT = Initial_RUN;
  //   CNode->NodeStatusCH = NEED_GEM_AIA;
  // CNode->IterDone = 0;
  databr_reset( CNode, 1 );

  if( pmm->pNP == 0 )
   CNode->NodeStatusCH = NEED_GEM_AIA;
  else
    CNode->NodeStatusCH = NEED_GEM_SIA;

  CNode->TC = pmm->TCc; //25
  CNode->P = pmm->Pc*bar_to_Pa; //1
  CNode->Ms = pmm->MBX; // in kg

// arrays
   for( i1=0; i1<CSD->nICb; i1++ )
    CNode->bIC[i1] = pmm->B[ CSD->xic[i1] ];

   for( i1=0; i1<CSD->nDCb; i1++ )
   {
     CNode->dul[i1] = pmm->DUL[ CSD->xdc[i1] ];
     CNode->dll[i1] = pmm->DLL[ CSD->xdc[i1] ];
    }

   if( CSD->nAalp >0 )
      for( i1=0; i1< CSD->nPHb; i1++ )
        CNode->aPH[i1] = pmm->Aalp[CSD->xph[i1]]/g_to_kg;

// puts calculated & dynamic data to DataBR
   packDataBr();

// must be changed to matrix structure  ???????
// setted CSD->nPp*CSD->nTp = 1
   for( i1=0; i1<CSD->nTp; i1++ )
    CSD->TCval[i1] = Tai[i1];
   for( i1=0; i1<CSD->nPp; i1++ )
    CSD->Pval[i1] = Pai[i1];

   G0_V0_H0_Cp0_DD_arrays( par );

   for( i1=0; i1<CSD->nPp; i1++ )
    CSD->Pval[i1] = Pai[i1]*bar_to_Pa;

   if(  CSD->iGrd  )
     for( i1=0; i1< CSD->nDCs*CSD->nPp*CSD->nTp; i1++ )
       CSD->DD[i1] = 0.;
}

void TNode::G0_V0_H0_Cp0_DD_arrays( QWidget* par )
{
  int kk, jj, ii, ll;
  double cT, cP;
  double *G0, *V0, *H0, *Cp0, *S0, *A0, *U0, denW[5], epsW[5], denWg[5], epsWg[5];
  int *tp_mark;

  G0 =  new double[TProfil::pm->mup->L];
  V0 =  new double[TProfil::pm->mup->L];
  H0 =  new double[TProfil::pm->mup->L];
  Cp0 = new double[TProfil::pm->mup->L];
  S0 = new double[TProfil::pm->mup->L];
  A0 = new double[TProfil::pm->mup->L];
  U0 = new double[TProfil::pm->mup->L];
  tp_mark = new int[TProfil::pm->mup->L];
  fillValue( tp_mark, 0, TProfil::pm->mup->L);

  for(  ii=0; ii<CSD->nTp; ii++)
  {
    cT = CSD->TCval[ii];
    for(  jj=0; jj<CSD->nPp; jj++)
    {
     if( par )
    	pVisor->Message( par, "Building lookup arrays",
            "Loading thermodynamic data", ii*CSD->nPp+jj, CSD->nTp*CSD->nPp );

     cP = CSD->Pval[jj];
     // calculates new G0, V0, H0, Cp0, S0
    TProfil::pm->LoadFromMtparm( cT, cP, G0, V0, H0, S0, Cp0,
    		 A0, U0, denW, epsW, denWg, epsWg, tp_mark);
     for( kk=0; kk<5; kk++)
     {
        ll = ( kk * CSD->nPp + jj) * CSD->nTp + ii;
        CSD->denW[ ll ] = denW[kk]*1e3;
        CSD->epsW[ ll ] = epsW[kk];
        CSD->denWg[ ll ] = denWg[kk]*1e3;
        CSD->epsWg[ ll ] = epsWg[kk];
      }
      // copy to arrays
     for( kk=0; kk<CSD->nDC; kk++)
      {
         ll = ( kk * CSD->nPp + jj) * CSD->nTp + ii;
         CSD->G0[ll] =  G0[pmm->muj[kk]]; //
         CSD->V0[ll] =  V0[pmm->muj[kk]]*1e-5;
         CSD->H0[ll] = H0[pmm->muj[kk]];
         CSD->Cp0[ll] = Cp0[pmm->muj[kk]];
         CSD->S0[ll] = S0[pmm->muj[kk]];
         CSD->A0[ll] = A0[pmm->muj[kk]];
         CSD->U0[ll] = U0[pmm->muj[kk]];
      }
     }
  }
 if( par )
  pVisor->CloseMessage();
  gstring err = "";
  for( ii =0, kk=0; kk<CSD->nDC; kk++)
   {
	  if( tp_mark[pmm->muj[kk]]==1 )
	  {	  err +=" ";
	      err += gstring(CSD->DCNL[kk],0, MaxDCN);
	      if(!((++ii)%5)) err += "\n";
	  }
  }
  if( !err.empty() )
	  vfMessage(par,"Not quality for TP dependencies of DC", err.c_str());
  // free memory
  delete[] G0;
  delete[] V0;
  delete[] H0;
  delete[] Cp0;
  delete[] S0;
  delete[] A0;
  delete[] U0;
}

//Constructor of the class instance in memory
TNode::TNode( MULTI *apm  )
{
    pmm = apm;
    CSD = 0;
    CNode = 0;
    allocMemory();
    na = this;
    dbr_file_name = "dbr_file_name";
}

#else
// Constructor of the class instance in memory for standalone GEMIPM2K or coupled program
TNode::TNode()
{
  CSD = 0;
  CNode = 0;
  allocMemory();
  na = this;
  dbr_file_name = "dbr_file_name";
}

#endif


TNode::~TNode()
{
   freeMemory();
   na = 0;
}

// Extracting and packing GEM IPM results into work DATABR structure
void TNode::packDataBr()
{
 long int ii;

// set default data to DataBr
#ifndef IPMGEMPLUGIN
   CNode->NodeHandle = 0;
//   CNode->NodeTypeHY = normal;
   CNode->NodeTypeMT = normal;
   CNode->NodeStatusFMT = Initial_RUN;
#endif
//   CNode->NodeStatusCH = NEED_GEM_AIA;
   if( pmm->pNP == 0 )
    CNode->NodeStatusCH = NEED_GEM_AIA;
  else
     CNode->NodeStatusCH = NEED_GEM_SIA;

   CNode->TC = pmm->TCc; //25
   CNode->P = pmm->Pc*bar_to_Pa; //1
//   CNode->IterDone = pmm->IT;
   CNode->IterDone = pmm->ITF+pmm->IT;   // Now complete number of FIA and IPM iterations
// values
  CNode->Vs = pmm->VXc*1.e-6; // from cm3 to m3
  CNode->Gs = pmm->FX;
  CNode->Hs = pmm->HXc;
  CNode->IC = pmm->IC;
  CNode->pH = pmm->pH;
  CNode->pe = pmm->pe;
//  CNode->Eh = pmm->FitVar[3];  Bugfix 19.12.2006  KD
  CNode->Eh = pmm->Eh;
  CNode->Ms = pmm->MBX;

  // arrays
   for( ii=0; ii<CSD->nPHb; ii++ )
   {  CNode->xPH[ii] = pmm->XF[ CSD->xph[ii] ];
      if( CSD->nAalp >0 )
       CNode->aPH[ii] = pmm->Aalp[ CSD->xph[ii] ]/g_to_kg;//??? only insert
   }
   for( ii=0; ii<CSD->nPSb; ii++ )
   {   CNode->vPS[ii] = pmm->FVOL[ CSD->xph[ii] ]/m3_to_cm3;
       CNode->mPS[ii] = pmm->FWGT[ CSD->xph[ii] ]*g_to_kg;
       CNode->xPA[ii] = pmm->XFA[ CSD->xph[ii] ];
   }
   for( ii=0; ii<CSD->nPSb; ii++ )
   for(long int jj=0; jj<CSD->nICb; jj++ )
   { long int   new_ndx= (ii*CSD->nICb)+jj,
           mul_ndx = ( CSD->xph[ii]*CSD->nIC )+ CSD->xic[jj];
     CNode->bPS[new_ndx] = pmm->BF[ mul_ndx ];
   }
   for( ii=0; ii<CSD->nDCb; ii++ )
   {
      CNode->xDC[ii] = pmm->X[ CSD->xdc[ii] ];
      CNode->gam[ii] = pmm->Gamma[ CSD->xdc[ii] ];
      CNode->dul[ii] = pmm->DUL[ CSD->xdc[ii] ];// 09/02/2009 SD only insert
      CNode->dll[ii] = pmm->DLL[ CSD->xdc[ii] ];// 09/02/2009 SD only insert
   }
   for( ii=0; ii<CSD->nICb; ii++ )
   {  CNode->bIC[ii] = pmm->B[ CSD->xic[ii] ];// 09/02/2009 SD only insert
      CNode->rMB[ii] = pmm->C[ CSD->xic[ii] ];
      CNode->uIC[ii] = pmm->U[ CSD->xic[ii] ];
   }
}


// Extracting and packing GEM IPM results into work DATABR structure
// with re-scaling the internal constant-mass MULTI system definition
// back to real node size
//
void TNode::packDataBr( double ScFact )
{
  long int ii;

  if( ScFact < 1e-6 )    // foolproof
 	 ScFact = 1e-6;
  if( ScFact > 1e6 )
 	 ScFact = 1e6;
  if( ScFact < 0. )
 	 ScFact = 1.;
 // set default data to DataBr
#ifndef IPMGEMPLUGIN
   CNode->NodeHandle = 0;
//   CNode->NodeTypeHY = normal;
   CNode->NodeTypeMT = normal;
   CNode->NodeStatusFMT = Initial_RUN;
#endif
   //   CNode->NodeStatusCH = NEED_GEM_AIA;
   if( pmm->pNP == 0 )
    CNode->NodeStatusCH = NEED_GEM_AIA;
  else
     CNode->NodeStatusCH = NEED_GEM_SIA;

   CNode->TC = pmm->TCc; //25
   CNode->P = pmm->Pc*bar_to_Pa; //1
   CNode->IterDone = pmm->ITF+pmm->IT;   // Now complete number of FIA and IPM iterations
// values
  CNode->Vs = pmm->VXc*1.e-6 / ScFact; // from cm3 to m3
  CNode->Gs = pmm->FX  / ScFact;
  CNode->Hs = pmm->HXc  / ScFact;
  CNode->IC = pmm->IC;
  CNode->pH = pmm->pH;
  CNode->pe = pmm->pe;
//  CNode->Eh = pmm->FitVar[3];  Bugfix 19.12.2006  KD
  CNode->Eh = pmm->Eh;
  CNode->Ms = pmm->MBX / ScFact;

  // arrays
   for( ii=0; ii<CSD->nPHb; ii++ )
   {  CNode->xPH[ii] = pmm->XF[ CSD->xph[ii] ] / ScFact;
      if( CSD->nAalp >0 )
       CNode->aPH[ii] = pmm->Aalp[ CSD->xph[ii] ]/g_to_kg;//??? only insert
   }
   for( ii=0; ii<CSD->nPSb; ii++ )
   {   CNode->vPS[ii] = pmm->FVOL[ CSD->xph[ii] ]/m3_to_cm3  / ScFact;
       CNode->mPS[ii] = pmm->FWGT[ CSD->xph[ii] ] * g_to_kg / ScFact;
       CNode->xPA[ii] = pmm->XFA[ CSD->xph[ii] ]  / ScFact;
   }
   for( ii=0; ii<CSD->nPSb; ii++ )
   for(long int jj=0; jj<CSD->nICb; jj++ )
   { long int   new_ndx= (ii*CSD->nICb)+jj,
           mul_ndx = ( CSD->xph[ii]*CSD->nIC )+ CSD->xic[jj];
     CNode->bPS[new_ndx] = pmm->BF[ mul_ndx ]  / ScFact;
   }
   for( ii=0; ii<CSD->nDCb; ii++ )
   {
      CNode->xDC[ii] = pmm->X[ CSD->xdc[ii] ]  / ScFact;
      CNode->gam[ii] = pmm->Gamma[ CSD->xdc[ii] ];
//      CNode->dul[ii] = pmm->DUL[ CSD->xdc[ii] ] / ScFact;  // 09/02/2009 SD
//      if( pmm->DLL[ CSD->xdc[ii] ] < 1e-20 )
//    	  CNode->dll[ii] = 0.;
//      else CNode->dll[ii] = pmm->DLL[ CSD->xdc[ii] ] / ScFact;  //
   }
   for( ii=0; ii<CSD->nICb; ii++ )
   {  // CNode->bIC[ii] = pmm->B[ CSD->xic[ii] ] / ScFact;// 09/02/2009 SD only insert
      CNode->rMB[ii] = pmm->C[ CSD->xic[ii] ] / ScFact;
      CNode->uIC[ii] = pmm->U[ CSD->xic[ii] ];
   }
}

// Unpacking work DATABR structure into MULTI
//(GEM IPM work structure): uses DATACH
//  if uPrimalSol is true then the primal solution (vectors x, gamma, IC etc.)
//  will be unpacked - as an option for PIA mode with previous GEM solution from
//  the same node.
//  If uPrimalSol = false then the primal solution data will not be unpacked
//  into the MULTI structure (AIA mode or SIA mode with primal solution retained
//    in the MULTI structure from previous IPM calculation)
void TNode::unpackDataBr( bool uPrimalSol )
{
 long int ii;

#ifdef IPMGEMPLUGIN
 char buf[300];
 sprintf( buf, "Node:%ld:time:%lg:dt:%lg", CNode->NodeHandle, CNode->Tm, CNode->dt );
 strncpy( pmm->stkey, buf, EQ_RKLEN );
#endif
  pmm->TCc = CNode->TC;
  pmm->Tc = CNode->TC+C_to_K;
  pmm->Pc  = CNode->P/bar_to_Pa;
  pmm->VXc = CNode->Vs/1.e-6; // from cm3 to m3
  // Obligatory arrays - always unpacked!
  for( ii=0; ii<CSD->nDCb; ii++ )
  {
    pmm->DUL[ CSD->xdc[ii] ] = CNode->dul[ii];
    pmm->DLL[ CSD->xdc[ii] ] = CNode->dll[ii];
  }
  for( ii=0; ii<CSD->nICb; ii++ )
    pmm->B[ CSD->xic[ii] ] = CNode->bIC[ii];
  for( ii=0; ii<CSD->nPHb; ii++ )
  {
    if( CSD->nAalp >0 )
        pmm->Aalp[ CSD->xph[ii] ] = CNode->aPH[ii]*g_to_kg;
  }

 if( !uPrimalSol )
 {    //  Using primal solution retained in the MULTI structure instead -
    ; // the primal solution data from the DATABR structure are not unpacked
//   pmm->IT = 0;
 }
 else {   // Unpacking primal solution provided in the node DATABR structure
  pmm->IT = 0;
  pmm->MBX = CNode->Ms;
  pmm->IC = CNode->IC;
  pmm->Eh = CNode->Eh;
  for( ii=0; ii<CSD->nDCb; ii++ )
  /*    pmm->X[ CSD->xdc[ii] ] = */
        pmm->Y[ CSD->xdc[ii] ] = CNode->xDC[ii];
  for( ii=0; ii<CSD->nDCb; ii++ )
  {
     pmm->lnGam[ CSD->xdc[ii] ] = log( CNode->gam[ii] );
  //       Gamm = CNode->gam[ii];
  //      pmm->Gamma[ CSD->xdc[ii] ] = Gamm;
  //      pmm->lnGmo[ CSD->xdc[ii] ] = pmm->lnGam[ CSD->xdc[ii] ] = log(Gamm);
  }
  for( ii=0; ii<CSD->nPSb; ii++ )
   pmm->FVOL[ CSD->xph[ii] ] = CNode->vPS[ii]*m3_to_cm3;
  for( ii=0; ii<CSD->nPSb; ii++ )
   pmm->FWGT[ CSD->xph[ii] ] = CNode->mPS[ii]/g_to_kg;

  for( ii=0; ii<CSD->nPHb; ii++ )
  {
    pmm->XF[ CSD->xph[ii] ] =
    pmm->YF[ CSD->xph[ii] ] = CNode->xPH[ii];
  }

  for( long int k=0; k<CSD->nPSb; k++ )
  for(long int i=0; i<CSD->nICb; i++ )
  { long int dbr_ndx= (k*CSD->nICb)+i,
          mul_ndx = ( CSD->xph[k]*CSD->nIC )+ CSD->xic[i];
    pmm->BF[ mul_ndx ] = CNode->bPS[dbr_ndx];
  }

  for( ii=0; ii<CSD->nPSb; ii++ )
   pmm->XFA[ CSD->xph[ii] ] = pmm->YFA[ CSD->xph[ii] ] = CNode->xPA[ii];

  for( ii=0; ii<CSD->nICb; ii++ )
   pmm->C[ CSD->xic[ii] ] = CNode->rMB[ii];
  for( ii=0; ii<CSD->nICb; ii++ )
   pmm->U[ CSD->xic[ii] ] = CNode->uIC[ii];
 }
//  End
}

// Unpacking work DATABR structure into internally scaled constant mass MULTI
//(GEM IPM work structure): uses DATACH
//  if uPrimalSol is true then the primal solution (vectors x, gamma, IC etc.)
//  will be unpacked - as an option for PIA mode with previous GEM solution from
//  the same node.
//  If uPrimalSol = false then the primal solution data will not be unpacked
//  into the MULTI structure (AIA mode or SIA mode with primal solution retained
//    in the MULTI structure from previous IPM calculation)
void TNode::unpackDataBr( bool uPrimalSol, double ScFact )
{
 long int ii;

#ifdef IPMGEMPLUGIN
 char buf[300];
 sprintf( buf, "Node:%ld:time:%lg:dt:%lg", CNode->NodeHandle, CNode->Tm, CNode->dt );
 strncpy( pmm->stkey, buf, EQ_RKLEN );
#endif

 if( ScFact < 1e-6 )    // foolproof
	 ScFact = 1e-6;
 if( ScFact > 1e6 )
	 ScFact = 1e6;
  pmm->TCc = CNode->TC;
  pmm->Tc = CNode->TC+C_to_K;
  pmm->Pc  = CNode->P/bar_to_Pa;
  pmm->VXc = CNode->Vs /1.e-6 *ScFact; // from cm3 to m3
  // Obligatory arrays - always unpacked!
  for( ii=0; ii<CSD->nDCb; ii++ )
  {
    pmm->DUL[ CSD->xdc[ii] ] = CNode->dul[ii]* ScFact;
    if(	pmm->DUL[ CSD->xdc[ii] ] > 1e6 )		
       pmm->DUL[ CSD->xdc[ii] ] = 1e6;          // Bugfix for upper metastability limit

    pmm->DLL[ CSD->xdc[ii] ] = CNode->dll[ii];
    if( CNode->dll[ii] > 0. )
    	pmm->DLL[ CSD->xdc[ii] ] *= ScFact;

    if( pmm->DUL[ CSD->xdc[ii] ] < pmm->DLL[ CSD->xdc[ii] ] )
    {
       char buf[300];
       sprintf(buf, "Upper kinetic restrictions smolest than lower for DC&RC %-6.6s",
            		 pmm->SM[CSD->xdc[ii]] );
    	Error("unpackDataBr", buf );
    }
  }
  for( ii=0; ii<CSD->nICb; ii++ )
  {
    pmm->B[ CSD->xic[ii] ] = CNode->bIC[ii] * ScFact;
    if( ii < CSD->nICb-1 && pmm->B[ CSD->xic[ii] ] < TProfil::pm->pa.p.DB )
    {
       char buf[300];
       sprintf(buf, "Bulk mole amounts of IC  %-6.6s is %lg",
            		 pmm->SB[CSD->xic[ii]], pmm->B[ CSD->xic[ii] ] );
    	Error("unpackDataBr", buf );
    }

  }
  for( ii=0; ii<CSD->nPHb; ii++ )
  {
    if( CSD->nAalp >0 )
        pmm->Aalp[ CSD->xph[ii] ] = CNode->aPH[ii]*g_to_kg;
  }

 if( !uPrimalSol )
 {    //  Using primal solution retained in the MULTI structure instead -
    ; // the primal solution data from the DATABR structure are not unpacked
//   pmm->IT = 0;
 }
 else {   // Unpacking primal solution provided in the node DATABR structure
  pmm->MBX = CNode->Ms * ScFact;
  pmm->IC = CNode->IC;
//  pmm->FitVar[3] = CNode->Eh;  Bugfix 19.12.2006  KD
  pmm->Eh = CNode->Eh;
  for( ii=0; ii<CSD->nDCb; ii++ )
  /*    pmm->X[ CSD->xdc[ii] ] = */
        pmm->Y[ CSD->xdc[ii] ] = CNode->xDC[ii] * ScFact;
  for( ii=0; ii<CSD->nDCb; ii++ )
  {
     pmm->lnGam[ CSD->xdc[ii] ] = log( CNode->gam[ii] );
  }
  for( ii=0; ii<CSD->nPSb; ii++ )
   pmm->FVOL[ CSD->xph[ii] ] = CNode->vPS[ii]* m3_to_cm3 * ScFact;
  for( ii=0; ii<CSD->nPSb; ii++ )
   pmm->FWGT[ CSD->xph[ii] ] = CNode->mPS[ii] /g_to_kg * ScFact;

  for( ii=0; ii<CSD->nPHb; ii++ )
  {
    pmm->XF[ CSD->xph[ii] ] =
    pmm->YF[ CSD->xph[ii] ] = CNode->xPH[ii] * ScFact;
  }

  for( long int k=0; k<CSD->nPSb; k++ )
  for(long int i=0; i<CSD->nICb; i++ )
  { long int dbr_ndx= (k*CSD->nICb)+i,
          mul_ndx = ( CSD->xph[k]*CSD->nIC )+ CSD->xic[i];
    pmm->BF[ mul_ndx ] = CNode->bPS[dbr_ndx] * ScFact;
  }

  for( ii=0; ii<CSD->nPSb; ii++ )
   pmm->XFA[ CSD->xph[ii] ] = pmm->YFA[ CSD->xph[ii] ] = CNode->xPA[ii] * ScFact;

  for( ii=0; ii<CSD->nICb; ii++ )
   pmm->C[ CSD->xic[ii] ] = CNode->rMB[ii] * ScFact;  // Is this really needed?
  for( ii=0; ii<CSD->nICb; ii++ )
   pmm->U[ CSD->xic[ii] ] = CNode->uIC[ii];
 }
//  End
}

// Resizes the work node chemical system
// Returns new node mass Ms
double TNode::ResizeNode( double Factor )
{
   long int ii;
	if( Factor < 1e-6 )    // foolproof
		 Factor = 1e-6;
	 if( Factor > 1e6 )
		 Factor = 1e6;
	 if( Factor < 0. )

     for( ii=0; ii<CSD->nDCb; ii++ )
	{
	   if( CNode->dul[ii] < 1e6 )
		   CNode->dul[ii] *= Factor;
       if( CNode->dll[ii] > 0. )
    	   CNode->dll[ii] *= Factor;
	}
	for( ii=0; ii<CSD->nICb; ii++ )
	      CNode->bIC[ii] *= Factor;
    CNode->Ms *= Factor;
    CNode->Vs *= Factor;
    CNode->Gs *= Factor;
    CNode->Hs *= Factor;

	for( ii=0; ii<CSD->nDCb; ii++ )
	     CNode->xDC[ii] *= Factor;
	for( ii=0; ii<CSD->nPSb; ii++ )
	     CNode->vPS[ii] *= Factor;
	for( ii=0; ii<CSD->nPSb; ii++ )
	     CNode->mPS[ii] *= Factor;

	for( ii=0; ii<CSD->nPHb; ii++ )
	     CNode->xPH[ii] *= Factor;

    for( long int k=0; k<CSD->nPSb; k++ )
    	for(long int i=0; i<CSD->nICb; i++ )
		{
    		long int dbr_ndx= (k*CSD->nICb)+i;
		    CNode->bPS[dbr_ndx] *= Factor;
		}

    for( ii=0; ii<CSD->nPSb; ii++ )
	    CNode->xPA[ii] *= Factor;
	for( ii=0; ii<CSD->nICb; ii++ )
		CNode->rMB[ii] *= Factor;  // Is this really needed?
		//  End
    return CNode->Ms;
}

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
void  TNode::GEM_write_dbr( const char* fname, bool binary_f, bool with_comments, bool brief_mode )
   {
       gstring str_file;
       if( fname == 0)
    	   str_file = dbr_file_name+".out";
       else
           str_file = fname;

	   if( binary_f )
           {
            // gstring str_file = fname;
              GemDataStream out_br(str_file, ios::out|ios::binary);
              databr_to_file(out_br);
           }
      else
      {  fstream out_br(str_file.c_str(), ios::out );
         ErrorIf( !out_br.good() , str_file.c_str(), "DataBR text make error");
         databr_to_text_file(out_br, with_comments, brief_mode);
      }
   }

// (4) Produces a formatted text file with detailed contents (scalars and arrays) of the GEM IPM work structure. 
// This call is useful when GEM_run() returns with a NodeStatusCH value indicating a GEM calculation error
// (see  above).  Another use is for a detailed comparison of a test system calculation after the version upgrade of GEMIPM2K.
// Parameters: fname   null-terminated (C) string containing a full path to the disk file to be written.
//                     NULL  - the disk file name path stored in the  dbr_file_name  field of the TNode class instance will be used,
//                     extended with ".dump.out".  Usually the dbr_file_name field contains the path to the last input DBR file.
   void  TNode::GEM_print_ipm( const char* fname )
   {
     gstring str_file;
     if( fname == 0)
    	   str_file = dbr_file_name + ".Dump.out";
     else
           str_file = fname;

	   TProfil::pm->outMultiTxt( str_file.c_str()  );
   }

#ifdef IPMGEMPLUGIN

// (9) Optional, for passing the current mass transport iteration information into the work 
// DATABR structure (e.g. for using it in tracing/debugging or in writing DBR files for nodes)
void TNode::GEM_set_MT(
   double p_Tm,      // actual total simulation time, s          +       -      -
   double p_dt       // actual time step, s                      +       -      -
)
{
  CNode->Tm = p_Tm;
  CNode->dt = p_dt;
}

// (6) Passes (copies) the GEMIPM2K input data from the work instance of DATABR structure. 
//  This call is useful after the GEM_init() (1) and GEM_run() (2) calls to initialize the arrays which keep the
//   chemical data for all nodes used in the mass-transport model. 
void TNode::GEM_restore_MT(
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
   )
{
  long int ii;
  p_NodeHandle = CNode->NodeHandle;
  p_NodeStatusCH = CNode->NodeStatusCH;
  p_TC = CNode->TC;
  p_P = CNode->P;
  p_Vs = CNode->Vs;
  p_Ms = CNode->Ms;
// Checking if no-simplex IA is Ok
   for( ii=0; ii<CSD->nICb; ii++ )
     p_bIC[ii] = CNode->bIC[ii];
   for( ii=0; ii<CSD->nDCb; ii++ )
   {  p_dul[ii] = CNode->dul[ii];
      p_dll[ii] = CNode->dll[ii];
   }
   if( CSD->nAalp >0 )
     for( ii=0; ii<CSD->nPHb; ii++ )
        p_aPH[ii] = CNode->aPH[ii];
}

// (7)  Retrieves the GEMIPM2 chemical speciation calculation results from the work DATABR structure instance
//   into memory provided by the mass transport part. Dimensions and order of elements in the arrays must correspond
//   to those in currently existing DATACH memory structure.
void TNode::GEM_to_MT(
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
 )   
{
   long int ii;
   p_NodeHandle = CNode->NodeHandle;
   p_NodeStatusCH = CNode->NodeStatusCH;
   p_IterDone = CNode->IterDone;

   p_Vs = CNode->Vs;
   p_Ms = CNode->Ms;
   p_Gs = CNode->Gs;
   p_Hs = CNode->Hs;
   p_IC = CNode->IC;
   p_pH = CNode->pH;
   p_pe = CNode->pe;
   p_Eh = CNode->Eh;

  for( ii=0; ii<CSD->nICb; ii++ )
  {
    p_rMB[ii] = CNode->rMB[ii];
    p_uIC[ii] = CNode->uIC[ii];
  }
  for( ii=0; ii<CSD->nDCb; ii++ )
  {
    p_xDC[ii] = CNode->xDC[ii];
    p_gam[ii] = CNode->gam[ii];
  }
  for( ii=0; ii<CSD->nPHb; ii++ )
    p_xPH[ii] = CNode->xPH[ii];
  for( ii=0; ii<CSD->nPSb; ii++ )
  {
    p_vPS[ii] = CNode->vPS[ii];
    p_mPS[ii] = CNode->mPS[ii];
    p_xPA[ii] = CNode->xPA[ii];
  }
  for( ii=0; ii<CSD->nPSb*CSD->nICb; ii++ )
    p_bPS[ii] = CNode->bPS[ii];
}

// (8) Loads the GEMIPM2K input data for a given mass-transport node into the work instance of DATABR structure.  
//     This call is usually preceeding the GEM_run() call
void TNode::GEM_from_MT(
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
 )
 {
     long int ii;
     bool useSimplex = false;

     CNode->NodeHandle = p_NodeHandle;
     CNode->NodeStatusCH = p_NodeStatusCH;
     CNode->TC = p_TC;
     CNode->P = p_P;
     CNode->Vs = p_Vs;
     CNode->Ms = p_Ms;
   // Checking if no-simplex IA is Ok
      for( ii=0; ii<CSD->nICb; ii++ )
      {  //  SD 11/02/05 for test
         //if( fabs(CNode->bIC[ii] - p_bIC[ii] ) > CNode->bIC[ii]*1e-4 ) // bugfix KD 21.11.04
          //     useSimplex = true;
        CNode->bIC[ii] = p_bIC[ii];
      }
      for( ii=0; ii<CSD->nDCb; ii++ )
      {
        CNode->dul[ii] = p_dul[ii];
        CNode->dll[ii] = p_dll[ii];
      }
       if( CSD->nAalp >0 )
        for( ii=0; ii<CSD->nPHb; ii++ )
            CNode->aPH[ii] = p_aPH[ii];
      if( useSimplex && CNode->NodeStatusCH == NEED_GEM_SIA )
        CNode->NodeStatusCH = NEED_GEM_AIA;
      // Switch only if SIA is ordered, leave if simplex is ordered (KD)
}

//(8a) Loads the GEMIPM2K input data for a given mass-transport node into the work instance of DATABR structure.
//This overloaded variant uses the xDC speciation vector for setting the 
// new bulk chemical composition to be used in the next GEM_run() calculation. 
void TNode::GEM_from_MT(  
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
)
{
  long int ii;
  bool useSimplex = false;

  CNode->NodeHandle = p_NodeHandle;
  CNode->NodeStatusCH = p_NodeStatusCH;
  CNode->TC = p_TC;
  CNode->P = p_P;
  CNode->Vs = p_Vs;
  CNode->Ms = p_Ms;
// Checking if no-simplex IA is Ok
   for( ii=0; ii<CSD->nICb; ii++ )
   {
     CNode->bIC[ii] = p_bIC[ii];
   }
   for( ii=0; ii<CSD->nDCb; ii++ )
   {
     CNode->dul[ii] = p_dul[ii];
     CNode->dll[ii] = p_dll[ii];
   }
    if( CSD->nAalp >0 )
     for( ii=0; ii<CSD->nPHb; ii++ )
         CNode->aPH[ii] = p_aPH[ii];
   if( useSimplex && CNode->NodeStatusCH == NEED_GEM_SIA )
     CNode->NodeStatusCH = NEED_GEM_AIA;
   // Switch only if SIA is ordered, leave if simplex is ordered (KD)

   // Optional part - convolution of xDC vector into bIC vector
   if( p_xDC )
   {  long int jj;
      // Correction of bIC vector by convoluting the amounts of DCs
      for( jj=0; jj<CSD->nDCb; jj++ )
        if( p_xDC[jj] )
          for( ii=0; ii<CSD->nICb; ii++ )
            CNode->bIC[ii] += p_xDC[jj] * nodeCH_A( jj, ii );
   }
}

//(8b) Loads the GEMIPM2K input data for a given mass-transport node into the work instance of DATABR structure.
//In addition, provides access to speciation vector p_xDC and DC activity coefficients p_gam that will be used in
// GEM "smart initial approximation" SIA mode if dBR->NodeStatusCH == NEED_GEM_SIA (5) and
// uPrimalSol = true are set for the GEM_run() call (see Section 2) . This works only when the DATACH
//  structure contains a full list of Dependent Components used in GEM IPM2 calculations. 
void TNode::GEM_from_MT(
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
)
{
  long int ii;

  CNode->NodeHandle = p_NodeHandle;
  CNode->NodeStatusCH = p_NodeStatusCH;
  CNode->TC = p_TC;
  CNode->P = p_P;
  CNode->Vs = p_Vs;
  CNode->Ms = p_Ms;
// Checking if no-simplex IA is Ok
   for( ii=0; ii<CSD->nICb; ii++ )
   {
     CNode->bIC[ii] = p_bIC[ii];
   }
   for( ii=0; ii<CSD->nDCb; ii++ )
   {
     CNode->dul[ii] = p_dul[ii];
     CNode->dll[ii] = p_dll[ii];
   }
    if( CSD->nAalp >0 )
     for( ii=0; ii<CSD->nPHb; ii++ )
         CNode->aPH[ii] = p_aPH[ii];

   // Optional part - copying old primal solution from p_xDC and p_gam vectors
   if( p_xDC && p_gam )
   {
      for( ii=0; ii<CSD->nDCb; ii++ )
      {
        CNode->xDC[ii] = p_xDC[ii];
        CNode->gam[ii] = p_gam[ii];
      }
   }
   else if( CNode->NodeStatusCH == NEED_GEM_SIA )
            CNode->NodeStatusCH = NEED_GEM_AIA;   // no complete old primal solution provided!

//  Discuss the policy!
//   if( p_xDC )
//   {  long int jj;
//      // Correction of bIC vector by convoluting the amounts of DCs
//      for( jj=0; jj<CSD->nDCb; jj++ )
//        if( p_xDC[jj] )
//          for( ii=0; ii<CSD->nICb; ii++ )
//            CNode->bIC[ii] += p_xDC[jj] * nodeCH_A( jj, ii );
//   }

}

#endif
//-----------------------End of node.cpp--------------------------



