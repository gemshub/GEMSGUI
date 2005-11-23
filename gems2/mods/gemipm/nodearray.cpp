//-------------------------------------------------------------------
// $Id$
//
// GEMIPM2K kernel module
//
// C/C++ interface between GEM IPM and FMT node array
// Uses DATACH and DATABR structures
//
// Copyright (C) 2004-2005 S.Dmytriyeva, D.Kulik, W.Pfingsten, F.Enzmann
//
// This file is part of a GEM-Selektor library for thermodynamic
// modelling by Gibbs energy minimization
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://les.web.psi.ch/Software/GEMS-PSI for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------


#include "verror.h"
#include "tnodearray.h"
#include "gdatastream.h"
istream& f_getline(istream& is, gstring& str, char delim);


//-------------------------------------------------------------------
// NewNodeArray()
// Allocates a new FMT node array of DATABR structures and
// reads in the data from MULTI, DATACH, DATABR files prepared
// using the GEMS-PSI RMT module
//  Parameters:
//  nNodes            - requested number of nodes (>= 1)
//  MULTI_filename    - name of binary file with the MULTI structure
//  ipmfiles_lst_name - name of a text file that contains:
//    " -t/-b <dataCH file name>, <dataBR file name1>, ... <dataBR file nameN> "
//    These files (one dataCH file, at least one dataBR file) must exist in
//    the current directory; the dataBR files in the above list are indexed
//    as 1, 2, ... N (node types) and must contain valid initial chemical
//    systems (of the same structure described in the dataCH file) to set up
//    the initial state of the FMT node array. If -t flag is specified
//    then dataCH and dataBR files must be in text (ASCII) format;
//    if -b or nothing is specified then dataCH and dataBR files are
//    assumed to be binary (little-endian) files.
//  nodeTypes[nNodes] - array of node type (fortran) indexes of dataBR files in
//    the ipmfiles_lst_name list. This array, for each FMT node, specifies
//    from which dataBR file the initial chemical system should be taken.
//  Function returns:
//   0: OK; 1: GEMIPM read file error; -1: System error (e.g. memory allocation)
//
//-------------------------------------------------------------------
int  NewNodeArray( int &sizeN, int &sizeM, int &sizeK,
                   const char*  MULTI_filename,
                   const char *ipmfiles_lst_name, int *nodeTypes )
{
  int i;

  fstream f_log("ipmlog.txt", ios::out );
  try
    {
// Allocate memory for internal structures
      TNodeArray::na = new TNodeArray( sizeN, sizeM, sizeK );
      // = TNodeArray::na->profil;
      bool binary_f = true;
      gstring multu_in = MULTI_filename;
      gstring chbr_in = ipmfiles_lst_name;
      int nNodes = TNodeArray::na->nNodes();

// Reading structure MULTI (GEM IPM work structure)
      GemDataStream f_m(multu_in, ios::in|ios::binary);
      TProfil::pm->readMulti(f_m);

// output multy
    gstring strr = "out_multi.ipm";
    GemDataStream o_m( strr, ios::out|ios::binary);
    TProfil::pm->outMulti(o_m, strr );

// Reading name of dataCH file and names of dataBR files
//  -t/-b  "<dataCH file name>" ,"<dataBR file1 name>", ..., "<dataBR fileN name>"
      fstream f_chbr(chbr_in.c_str(), ios::in );
      ErrorIf( !f_chbr.good() , chbr_in.c_str(), "Fileopen error");

      gstring datachbr_file;
      f_getline( f_chbr, datachbr_file, ' ');

//Testing flag "-t" or "-b" (by default "-b")   // use bynary or text files as input
      size_t pos = datachbr_file.find( '-');
      if( pos != /*gstring::*/npos )
      {
         if( datachbr_file[pos+1] == 't' )
            binary_f = false;
         f_getline( f_chbr, datachbr_file, ',');
      }

// Reading dataCH structure from file
     gstring dat_ch = datachbr_file;
      if( binary_f )
      {  GemDataStream f_ch(dat_ch, ios::in|ios::binary);
         TNodeArray::na->datach_from_file(f_ch);
       }
      else
      { fstream f_ch(dat_ch.c_str(), ios::in );
         ErrorIf( !f_ch.good() , dat_ch.c_str(), "DataCH Fileopen error");
         TNodeArray::na->datach_from_text_file(f_ch);
      }

     i = 0;
     while( !f_chbr.eof() )  // For all dataBR files listed
     {
// Reading work dataBR structure from file
         f_getline( f_chbr, datachbr_file, ',');

         if( binary_f )
         {
             GemDataStream in_br(datachbr_file, ios::in|ios::binary);
             TNodeArray::na->databr_from_file(in_br);
          }
         else
          {   fstream in_br(datachbr_file.c_str(), ios::in );
		 ErrorIf( !in_br.good() , datachbr_file.c_str(),
                    "DataBR Fileopen error");
               TNodeArray::na->databr_from_text_file(in_br);
          }

// Unpacking work DATABR structure into MULTI (GEM IPM work structure): uses DATACH
//    TProfil::pm->multi->unpackDataBr();

// Copying data from work DATABR structure into the node array
// (as specified in nodeTypes array)
     for( int ii=0; ii<nNodes; ii++)
         if(  (!nodeTypes && i==0) ||
              ( nodeTypes && (nodeTypes[ii] == i+1 )) )
                  {    TNodeArray::na->data_BR->NodeHandle = ii+1;
                       TNodeArray::na->SaveNodeCopyToArray(ii, nNodes,
                             TNodeArray::na->arrBR_0);
                       TNodeArray::na->GetNodeCopyFromArray(ii, nNodes,
                             TNodeArray::na->arrBR_0);
                   }
          i++;
     }

    ErrorIf( i==0, datachbr_file.c_str(), "NewNodeArray() error: No dataBR files read!" );
    if(nodeTypes)
      for( int ii=0; ii<nNodes; ii++)
      if(   nodeTypes[ii]<=0 || nodeTypes[ii] >= i+1 )
           Error( datachbr_file.c_str(),
              "NewNodeArray() error: Undefined boundary condition!" );

    return 0;

    }
    catch(TError& err)
    {
      f_log << err.title.c_str() << "  : " << err.mess.c_str();
    }
    catch(...)
    {
        return -1;
    }
    return 1;
}

//-------------------------------------------------------------------------
// NodeCalcGEM()
// GEM IPM calculation of equilibrium state for the iNodeF node from array.
//
// Other parameters are elements of the DATABR structure.
//
// If the fortran node index iNodeF is negative then this call
// does not require any input parameters and will just return
// data from the node to FMT (readonly mode). Otherwise, at least p_T, p_P,
//  p_bIC, p_dll, p_dul and p_NodeStatusCH parameters must be specified.
//
//  Function returns:
//   0: OK; 1: GEMIPM2K calculation error; 1: system error
//
//-------------------------------------------------------------------

int  NodeCalcGEM( int  &readF, // negative means read only
   int &indN,  // fortran index; 0 working only with work structure
   int &indM,  // fortran index; 0 working only with work structure
   int &indK,  // fortran index; 0 working only with work structure
   short &p_NodeHandle,    // Node identification handle
   short &/*p_NodeTypeHY*/,    // Node type (hydraulic); see typedef NODETYPE
   short &/*p_NodeTypeMT*/,    // Node type (mass transport); see typedef NODETYPE
   short &/*p_NodeStatusFMT*/, // Node status code FMT; see typedef NODECODEFMT
   short &p_NodeStatusCH,  // Node status code CH;  see typedef NODECODECH
   short &p_IterDone,      // Number of iterations performed by IPM (if not need GEM)
// Chemical scalar variables
   double &p_T,     // Temperature T, K                        +      +      -     -
   double &p_P,     // Pressure P, bar                         +      +      -     -
   double &p_Vs,    // Volume V of reactive subsystem, cm3     -      -      +     +
   double &/*p_Vi*/,    // Volume of inert subsystem  ?            +      -      -     +
   double &p_Ms,    // Mass of reactive subsystem, kg          +      +      -     -
   double &/*p_Mi*/,    // Mass of inert part, kg    ?             +      -      -     +
   double &p_Gs,    // Gibbs energy of reactive subsystem (J)  -      -      +     +
   double &p_Hs,    // Enthalpy of reactive subsystem (J)      -      -      +     +
   double &/*p_Hi*/,    // Enthalpy of inert subsystem (J) ?       +      -      -     +
   double &p_IC,    // Effective molal aq ionic strength       -      -      +     +
   double &p_pH,    // pH of aqueous solution                  -      -      +     +
   double &p_pe,    // pe of aqueous solution                  -      -      +     +
   double &p_Eh,    // Eh of aqueous solution, V               -      -      +     +
   double &p_denW,
   double &p_denWg, // Density of H2O(l) and steam at T,P      -      -      +     +
   double &p_epsW,
   double &p_epsWg, // Diel.const. of H2O(l) and steam at T,P  -      -      +     +
//  FMT variables (units need dimensionsless form)
   double &/*p_Tm*/,    // actual total simulation time
   double &p_dt,    // actual time step
   double &p_dt1,   // priveous time step
   double &/*p_ot*/,    // output time control for postprocessing
   double &/*p_Vt*/,    // total volume of node (voxel) = dx*dy*dz, m**3
   double &/*p_eps*/,   // effective (actual) porosity normalized to 1
   double &/*p_Km*/,    // actual permeability, m**2
   double &/*p_Kf*/,    // actual DARCY`s constant, m**2/s
   double &/*p_S*/,	    // specific storage coefficient, dimensionless
   double &/*p_Tr*/,    // transmissivity m**2/s
   double &/*p_h*/,	    // actual hydraulic head (hydraulic potential), m
   double &/*p_rho*/,   // actual carrier density for density driven flow, g/cm**3
   double &/*p_al*/,    // specific longitudinal dispersivity of porous media, m
   double &/*p_at*/,    // specific transversal dispersivity of porous media, m
   double &/*p_av*/,    // specific vertical dispersivity of porous media, m
   double &/*p_hDl*/,   // hydraulic longitudinal dispersivity, m**2/s, diffusities from chemical database
   double &/*p_hDt*/,   // hydraulic transversal dispersivity, m**2/s
   double &/*p_hDv*/,   // hydraulic vertical dispersivity, m**2/s
   double &/*p_nPe*/,   // node Peclet number, dimensionless
// Dynamic data - dimensions see in DATACH.H and DATAMT.H structures
// exchange of values occurs through lists of indices, e.g. xDC, xPH
   double  *p_xDC,    // DC mole amounts at equilibrium [nDCb]      -      -      +     +
   double  *p_gam,    // activity coeffs of DC [nDCb]               -      -      +     +
   double  *p_xPH,  // total mole amounts of phases [nPHb]          -      -      +     +
   double  *p_vPS,  // phase volume, cm3/mol        [nPSb]          -      -      +     +
   double  *p_mPS,  // phase (carrier) mass, g      [nPSb]          -      -      +     +
   double  *p_bPS,  // bulk compositions of phases  [nPSb][nICb]    -      -      +     +
   double  *p_xPA,  // amount of carrier in phases  [nPSb] ??       -      -      +     +
   double  *p_dul,  // upper kinetic restrictions [nDCb]           +      +      -     -
   double  *p_dll,  // lower kinetic restrictions [nDCb]           +      +      -     -
   double  *p_bIC,  // bulk mole amounts of IC[nICb]                +      +      -     -
   double  *p_rMB,  // MB Residuals from GEM IPM [nICb]             -      -      +     +
   double  *p_uIC,  // IC chemical potentials (mol/mol)[nICb]       -      -      +     +
  // What else?
   double  */*p_dRes1*/,
   double  */*p_dRes2*/
)
{

// fortran to C index conversion & make one index from three
  int iNode = ( (indK-1) * TNodeArray::na->sizeM +( indM-1 ) ) *
                   TNodeArray::na->sizeN + (indN-1);

  int onlyWork = indK*indM*indN;
  fstream f_log("ipmlog.txt", ios::out|ios::app );
  try
  {
// cout << " MAIF_CALC begin Mode= " << p_NodeStatusCH << " iNode= " << iNode << endl;
//---------------------------------------------

// Copying data for node iNode from node array into work DATABR structure
if( onlyWork > 0)
   TNodeArray::na->GetNodeCopyFromArray( iNode, TNodeArray::na->anNodes,
                             TNodeArray::na->arrBR_0 );

if( readF > 0 )  // calculation mode: passing input GEM data changed on previous FMT iteration
{                 //                   into work DATABR structure
   TNodeArray::na->GEM_input_from_MT(  p_NodeHandle,  p_NodeStatusCH,
      p_T, p_P, p_Ms, p_dt, p_dt1,  p_dul, p_dll, p_bIC );   // test simplex
}

// Unpacking work DATABR structure into MULTI (GEM IPM work structure): uses DATACH
    TNodeArray::na->unpackDataBr();

// GEM IPM calculation of equilibrium state in MULTI
    TProfil::pm->calcMulti();

// Extracting and packing GEM IPM results into work DATABR structure
    TNodeArray::na->packDataBr();

// Copying results that must be returned into the FMT part into MAIF_CALC parameters
   TNodeArray::na->GEM_output_to_MT(
        p_NodeHandle, p_NodeStatusCH, p_IterDone,
        p_Vs, p_Gs, p_Hs, p_IC, p_pH, p_pe, p_Eh, p_denW,
        p_denWg, p_epsW, p_epsWg,
        p_xDC, p_gam, p_xPH, p_vPS, p_mPS, p_bPS,
        p_xPA, p_dul, p_dll, p_bIC, p_rMB, p_uIC );

if( readF < 0 )  // readonly mode: passing input GEM data to FMT
   TNodeArray::na->GEM_input_back_to_MT(p_NodeHandle,  p_NodeStatusCH,
      p_T, p_P, p_Ms, p_dt, p_dt1, p_dul, p_dll, p_bIC);


//**************************************************************
// only for testing output results for files
    gstring strr= "calculated.dbr";
// binary DATABR
    GemDataStream out_br(strr, ios::out|ios::binary);
    TNodeArray::na->databr_to_file(out_br);
// text DATABR
    fstream out_br_t("calculated_dbr.dat", ios::out );
    ErrorIf( !out_br_t.good() , "calculated_dbr.dat",
                "DataBR text file open error");
    TNodeArray::na->databr_to_text_file(out_br_t);
// output multy
    strr = "calc_multi.ipm";
    GemDataStream o_m( strr, ios::out|ios::binary);
    TProfil::pm->outMulti(o_m, strr );
//********************************************************* */

// Copying data for node iNode back from work DATABR structure into the node array
if( readF > 0 &&  onlyWork > 0)
    TNodeArray::na->SaveNodeCopyToArray( iNode, TNodeArray::na->anNodes,
                             TNodeArray::na->arrBR_0 );
    return 0;
}
    catch(TError& err)
    {
      f_log << err.title.c_str() << "  : " << err.mess.c_str();
    }
    catch(...)
    {
        return -1;
    }
    return 1;
}


// ------------------ End of nodearray.cpp -----------------------

