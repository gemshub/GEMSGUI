//-------------------------------------------------------------------
// $Id: nodearray.cpp 684 2005-11-23 13:17:15Z gems $
//
// C/C++ interface between GEM IPM and FMT node array
// Working whith DATACH and DATABR structures
//
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

#include "nodearray.h"
#include "gdatastream.h"
#include <math.h>
istream& f_getline(istream& is, gstring& str, char delim);

#ifndef IPMGEMPLUGIN
  #include "service.h"
#else
  TNodeArray* TNodeArray::na;
#endif


//---------------------------------------------------------//

void TNodeArray::allocMemory()
{
  int ii;
// alloc memory for data bidge structures
    CSD = new DATACH;
    CNode = new DATABR;

// alloc memory for all nodes at current time point
    NodT0 = new  DATABRPTR[anNodes];
    for(  ii=0; ii<anNodes; ii++ )
        NodT0[ii] = 0;

// alloc memory for all nodes at previous time point
    NodT1 = new  DATABRPTR[anNodes];
    for(  ii=0; ii<anNodes; ii++ )
        NodT1[ii] = 0;

#ifdef IPMGEMPLUGIN
// internal structures
    multi = new TMulti();
    pmm = multi->GetPM();
    profil = new TProfil( multi );
    TProfil::pm = profil;
#endif
}

void TNodeArray::freeMemory()
{
   int ii;
   datach_free();
   CSD = 0;
   CNode = databr_free( CNode );

   if( anNodes )
   { if( NodT0 )
       for(  ii=0; ii<anNodes; ii++ )
        if( NodT0[ii] )
           NodT0[ii] = databr_free(NodT0[ii]);
     delete[]  NodT0;
     NodT0 = 0;
     if( NodT1 )
       for(  ii=0; ii<anNodes; ii++ )
        if( NodT1[ii] )
          NodT1[ii] = databr_free(NodT1[ii]);
     delete[]  NodT1;
     NodT1 = 0;
   }

#ifdef IPMGEMPLUGIN
  delete[] multi;
  delete[] profil;
#endif

}


#ifndef IPMGEMPLUGIN

// Make start DATACH and DATABR data from GEMS internal data (MULTI and other)
void TNodeArray::MakeNodeStructures( QWidget* par, bool select_all,
    float *Tai, float *Pai,
    short nTp_, short nPp_, float Ttol_, float Ptol_  )
{

  TMulti* mult = TProfil::pm->multi;
  TCStringArray aList;
  TCIntArray aSelIC;
  TCIntArray aSelDC;
  TCIntArray aSelPH;

// select lists


    aList.Clear();
    for(int ii=0; ii< mult->GetPM()->N; ii++ )
    {  if( select_all )
         aSelIC.Add( ii );
       else
         aList.Add( gstring( mult->GetPM()->SB[ii], 0, MAXICNAME+MAXSYMB));
    }
    if( !select_all  )
      aSelIC = vfMultiChoice(par, aList,
          "Please, mark independent components for selection into DataBridge");

    aList.Clear();
    for(int ii=0; ii< mult->GetPM()->L; ii++ )
    {  if( select_all )
         aSelDC.Add( ii );
       else
       aList.Add( gstring( mult->GetPM()->SM[ii], 0, MAXDCNAME));
    }
    if( !select_all  )
       aSelDC = vfMultiChoice(par, aList,
         "Please, mark dependent components for selection into DataBridge");

    aList.Clear();
    for(int ii=0; ii< mult->GetPM()->FI; ii++ )
    {  if( select_all )
         aSelPH.Add( ii );
       else
       aList.Add( gstring( mult->GetPM()->SF[ii], 0, MAXPHNAME+MAXSYMB));
    }
    if( !select_all  )
       aSelPH = vfMultiChoice(par, aList,
         "Please, mark phases for selection into DataBridge");


// set default data and realloc arrays
   makeStartDataChBR( aSelIC, aSelDC, aSelPH,
                      nTp_, nPp_, Ttol_, Ptol_, Tai, Pai );
}


// Writing dataCH, dataBR structure to binary/text files
// and other nessassary GEM2MT files
void TNodeArray::PutGEM2MTFiles( QWidget* par, int nIV, bool textmode, bool binmode )
{
  if( !textmode && !binmode )
    return;

  // MakeNodeStructures must be called and setuped data to NodT0 array before

  TMulti* mult = TProfil::pm->multi;
  fstream fout;
  fstream fout_d;
  gstring Path_;
  gstring dir;
  gstring name;
  gstring newname;
  gstring path;
  char buf[20];

  // Get name of filenames structure
   path = gstring( rt[RT_SYSEQ].FldKey(2), 0, rt[RT_SYSEQ].FldLen(2));;
   path.strip();
   path += ".ipm";

AGAIN:
      // open file to output
   if( vfChooseFileSave(par, path,
          "Please, enter IPM work structure file name", "*.ipm" ) == false )
               return;
   u_splitpath( path, dir, name, newname );
   if( !(::access(path.c_str(), 0 )) ) //file exists
     switch( vfQuestion3( par, name.c_str(),
        "This set of files exists!",
             "&Overwrite", "&Rename", "&Cancel") )
            {
            case VF3_2:
                goto AGAIN;
                break;
            case VF3_1:
                break;
            case VF3_3:
                return;
            }

//  putting MULTI to binary file
    GemDataStream  ff(path, ios::out|ios::binary);
    ff.writeArray( &TProfil::pm->pa.p.PC, 10 );
    ff.writeArray( &TProfil::pm->pa.p.DG, 28 );
    mult->to_file( ff, path );

// out dataCH to binary file
   newname = name+"-dch";
   if( binmode )
   {  Path_ = u_makepath( dir, newname, "bin" );
      GemDataStream  f_ch1(Path_, ios::out|ios::binary);
      datach_to_file(f_ch1);
      f_ch1.close();
   }

// out dataCH to text file
   if( textmode )
   {  //newname = name+"-dch";
      Path_ = u_makepath( dir, newname, "dat" );
      fstream  f_ch2(Path_.c_str(), ios::out);
      datach_to_text_file(f_ch2);
      f_ch2.close();
   }

// making special files
// put data to IPMRUN.BAT file
   Path_ = u_makepath( dir, name/*"IPMRUN"*/, "BAT" );
   fout.open(Path_.c_str(), ios::out);
   fout << "echo off\n";
   fout << "rem Normal runs\n";
   if( textmode )
      fout << "gemipm2k.exe " << name.c_str() <<
            ".ipm " << name.c_str() << "-dat.lst\n";
   if( binmode )
       fout << "rem gemipm2k.exe " << name.c_str() <<
             ".ipm " << name.c_str() << "-bin.lst\n";
   fout.close();

// put data to pmfiles-bin.lst file
   if( binmode )
   {   newname = name+"-bin";
       Path_ = u_makepath( dir, newname/*"ipmfiles-bin"*/, "lst" );
       fout.open(Path_.c_str(), ios::out);
       fout << "-b \"" << name.c_str() << "-dch.bin\"";
   }
// put data to pmfiles-dat.lst file
   if( textmode )
   {   newname = name+"-dat";
       Path_ = u_makepath( dir, newname/*"ipmfiles-dat"*/, "lst" );
       fout_d.open(Path_.c_str(), ios::out);
       fout_d << "-t \"" << name.c_str() << "-dch.dat\"";
   }

 nIV = min( nIV, nNodes() );
 for( int ii = 0; ii < nIV; ii++ )
 {
   if( !NodT0[ii] )
      continue;

//   pVisor->Message( par, GetName(),
//      "Generation of databr files for initial states\n"
//           "Please, wait...", ii, nIV );
   // Save databr
    CopyWorkNodeFromArray( ii, anNodes, NodT0 );

   sprintf( buf, "%d", ii );
   // dataBR files - binary
    if( binmode )
    {
       newname =  name + + "-dbr-"  + buf;
       Path_ = u_makepath( dir, newname, "bin" );
       GemDataStream  f_br1(Path_, ios::out|ios::binary);
       databr_to_file(f_br1);
       f_br1.close();
       fout << ", \"" << newname.c_str() << ".bin\"";
     }

      if( textmode )
      {
        newname = name + "-dbr-" + buf;
        Path_ = u_makepath( dir, newname, "dat" );
        fstream  f_br2(Path_.c_str(), ios::out);
        databr_to_text_file(f_br2);
        f_br2.close();
        fout_d << ", \"" << newname.c_str() << ".dat\"";
     }
 } // ii
// pVisor->CloseMessage();
}

// Writing dataCH structure to binary file
void TNodeArray::makeStartDataChBR(
  TCIntArray& selIC, TCIntArray& selDC, TCIntArray& selPH,
  short nTp_, short nPp_, float Ttol_, float Ptol_,
  float *Tai, float *Pai )
{
// set sizes for DataCh
  uint ii;
  short i1;
// realloc memory for     DATACH  *CSD;  and  DATABR  *CNode;

  if( !CSD )
     CSD = new DATACH;
  if( !CNode )
     CNode = new DATABR;


  CSD->nIC = pmm->N;
  CSD->nDC = pmm->L;
  CSD->nPH = pmm->FI;
  CSD->nPS = pmm->FIs;
  CSD->nTp = nTp_;
  CSD->nPp = nPp_;
  if( pmm->Aalp )
    CSD->nAalp = 1;
  else
    CSD->nAalp = 0;
  CSD->uRes2 = 0;

// These dimensionalities define sizes of dynamic data in DATABT structure!!!

  CSD->nICb = (short)selIC.GetCount();
  CSD->nDCb = (short)selDC.GetCount();
  CSD->nPHb = (short)selPH.GetCount();
  CSD->nPSb = 0;
  for( ii=0; ii< selPH.GetCount(); ii++, CSD->nPSb++ )
   if( selPH[ii] >= pmm->FIs )
       break;
  CSD->uRes3 = 0;
  CSD->uRes4 = 0;
  CSD->dRes1 = 0.;
  CSD->dRes = 0.;

  CSD->Ttol = Ttol_;
  CSD->Ptol = Ptol_;

// realloc structures DataCh&DataBr

  datach_realloc();
  databr_realloc();

// set dynamic data to DataCH

  memcpy( CSD->nDCinPH, pmm->L1 , CSD->nPH*sizeof(short));
  for( ii=0; ii< selIC.GetCount(); ii++ )
    CSD->xIC[ii] = (short)selIC[ii];
  for( ii=0; ii< selDC.GetCount(); ii++ )
    CSD->xDC[ii] = (short)selDC[ii];
  for( ii=0; ii< selPH.GetCount(); ii++ )
    CSD->xPH[ii] = (short)selPH[ii];

  memcpy( CSD->A, pmm->A , CSD->nIC*CSD->nDC*sizeof(float));

  for( i1=0; i1< CSD->nIC; i1++ )
     CSD->ICmm[i1] = pmm->Awt[i1];

  memcpy( CSD->DCmm, pmm->MM , CSD->nDC*sizeof(double));
  memset( CSD->DD, 0, CSD->nDC*sizeof(double));

  if( CSD->nAalp >0 )
      for( i1=0; i1< CSD->nPH; i1++ )
         CSD->Aalp[i1] = pmm->Aalp[i1];

  memcpy( CSD->ICNL, pmm->SB , MaxICN*CSD->nIC*sizeof(char));
  memcpy( CSD->DCNL, pmm->SM , MaxDCN*CSD->nDC*sizeof(char));
  memcpy( CSD->PHNL, pmm->SF , MaxPHN*CSD->nPH*sizeof(char));

  memcpy( CSD->ccIC, pmm->ICC , CSD->nIC*sizeof(char));
  memcpy( CSD->ccDC, pmm->DCC , CSD->nDC*sizeof(char));
  memcpy( CSD->ccDCW, pmm->DCCW , CSD->nDC*sizeof(char));
  memcpy( CSD->ccPH, pmm->PHC , CSD->nPH*sizeof(char));

// set default data to DataBr

   CNode->NodeHandle = 0;
   CNode->NodeTypeHY = initital;
   CNode->NodeTypeMT = normal;
   CNode->NodeStatusFMT = Initial_RUN;
//   CNode->NodeStatusCH = NEED_GEM_AIA;
   if( pmm->pNP == 0 )
    CNode->NodeStatusCH = NEED_GEM_AIA;
  else
     CNode->NodeStatusCH = NEED_GEM_PIA;

   CNode->IterDone = 0;

   memset( &CNode->T, 0, 36*sizeof(double));
   CNode->T = pmm->Tc; //25
   CNode->P = pmm->Pc; //1
   CNode->Ms = pmm->MBX;

// arrays
   for( i1=0; i1<CSD->nICb; i1++ )
    CNode->bIC[i1] = pmm->B[ CSD->xIC[i1] ];

   for( i1=0; i1<CSD->nDCb; i1++ )
   {
     CNode->dul[i1] = pmm->DUL[ CSD->xDC[i1] ];
     CNode->dll[i1] = pmm->DLL[ CSD->xDC[i1] ];
    }

// set calculated&dynamic data to DataBR

   packDataBr();

// must be changed to matrix structure  ???????
// setted CSD->nPp*CSD->nTp = 1
   for( i1=0; i1<CSD->nTp; i1++ )
    CSD->Tval[i1] = Tai[i1];
   for( i1=0; i1<CSD->nPp; i1++ )
    CSD->Pval[i1] = Pai[i1];

   getG0_V0_H0_Cp0_matrix();

}

void TNodeArray::getG0_V0_H0_Cp0_matrix()
{

  double cT, cP/*, cDC*/;
  double *G0, *V0, *H0, *Cp0, roW, epsW;

  G0 =  new double[TProfil::pm->mup->L];
  V0 =  new double[TProfil::pm->mup->L];
  if ( pmm->H0 )
    H0 =  new double[TProfil::pm->mup->L];
  else
    H0 = 0;
  if ( pmm->Cp0 )
    Cp0 = new double[TProfil::pm->mup->L];
  else
    Cp0 = 0;

  for( int ii=0; ii<CSD->nTp; ii++)
  {
    cT = CSD->Tval[ii];
    for( int jj=0; jj<CSD->nPp; jj++)
    {
      cP = CSD->Pval[jj];
     // calc new G0, V0, H0, Cp0
     TProfil::pm->LoadFromMtparm( cT, cP, G0, V0, H0, Cp0, roW, epsW );
     CSD->roW[ jj * CSD->nTp + ii] = roW;
     CSD->epsW[ jj * CSD->nTp + ii] = epsW;
     // copy to arrays
     for(int kk=0; kk<CSD->nDC; kk++)
      {
         int ll = ( kk * CSD->nPp + jj) * CSD->nTp + ii;
         CSD->G0[ll] =  G0[pmm->muj[kk]]; //
         CSD->V0[ll] =  V0[pmm->muj[kk]];
         if ( H0 )
           CSD->H0[ll] = H0[pmm->muj[kk]];
         else
           CSD->H0[ll] = 0.;
         if ( Cp0 )
           CSD->Cp0[ll] = Cp0[pmm->muj[kk]];
         else
           CSD->Cp0[ll] = 0.;
       }
     }
  }

  // free memory
  delete[] G0;
  delete[] V0;
  if( H0 )
   delete[] H0;
  if( Cp0 )
   delete[] Cp0;
}

TNodeArray::TNodeArray( int nNod, MULTI *apm  )
{
    pmm = apm;
    anNodes = nNod;
    CSD = 0;
    CNode = 0;
    NodT0 = 0;  // nodes at current time point
    NodT1 = 0;  // nodes at previous time point

    allocMemory();

}

#else

TNodeArray::TNodeArray( int nNod  ):
 anNodes(nNod)
{
  sizeN = anNodes;
  sizeM = sizeK =1;
  allocMemory();

}

TNodeArray::TNodeArray( int asizeN, int asizeM, int asizeK ):
sizeN(asizeN), sizeM(asizeM), sizeK(asizeK)
{
  anNodes = asizeN*asizeM*asizeK;
  allocMemory();
}

#endif


TNodeArray::~TNodeArray()
{
   freeMemory();
}

#ifdef IPMGEMPLUGIN

//-------------------------------------------------------------------
// NewNodeArray()
// reads in the data from MULTI, DATACH, DATABR files prepared
// using the GEMS-PSI RMT module
//  Parameters:
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
int  TNodeArray::NewNodeArray( const char*  MULTI_filename,
                   const char* ipmfiles_lst_name, int* nodeTypes )
{
  int i;
  fstream f_log("ipmlog.txt", ios::out|ios::app );
  try
    {
      bool binary_f = true;
      gstring multu_in = MULTI_filename;
      gstring chbr_in = ipmfiles_lst_name;

// Reading structure MULTI (GEM IPM work structure)
      GemDataStream f_m(multu_in, ios::in|ios::binary);
      profil->readMulti(f_m);

// output multy
//      gstring strr = "out_multi.ipm";
//      GemDataStream o_m( strr, ios::out|ios::binary);
//      profil->outMulti(o_m, strr );

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
         datach_from_file(f_ch);
       }
      else
      { fstream f_ch(dat_ch.c_str(), ios::in );
         ErrorIf( !f_ch.good() , dat_ch.c_str(), "DataCH Fileopen error");
         datach_from_text_file(f_ch);
      }

     i = 0;
     while( !f_chbr.eof() )  // For all dataBR files listed
     {
// Reading work dataBR structure from file
         f_getline( f_chbr, datachbr_file, ',');

         if( binary_f )
         {
             GemDataStream in_br(datachbr_file, ios::in|ios::binary);
             databr_from_file(in_br);
          }
         else
          {   fstream in_br(datachbr_file.c_str(), ios::in );
		 ErrorIf( !in_br.good() , datachbr_file.c_str(),
                    "DataBR Fileopen error");
               databr_from_text_file(in_br);
          }

// Unpacking work DATABR structure into MULTI (GEM IPM work structure): uses DATACH
//    unpackDataBr();

// Copying data from work DATABR structure into the node array
// (as specified in nodeTypes array)
     for( int ii=0; ii<anNodes; ii++)
         if(  (!nodeTypes && i==0) ||
              ( nodeTypes && (nodeTypes[ii] == i+1 )) )
                  {    CNode->NodeHandle = i+1;
                       MoveWorkNodeToArray(ii, anNodes, NodT0);
                       CopyWorkNodeFromArray(ii, anNodes,NodT0);
                       MoveWorkNodeToArray(ii, anNodes, NodT1);
                       CopyWorkNodeFromArray(ii, anNodes,NodT1);
                   }
          i++;
     }

    ErrorIf( i==0, datachbr_file.c_str(), "NewNodeArray() error: No dataBR files read!" );
    if(nodeTypes)
      for( int ii=0; ii<anNodes; ii++)
      if(   nodeTypes[ii]<=0 || nodeTypes[ii] >= i+1 )
           Error( datachbr_file.c_str(),
              "NewNodeArray() error: Undefined boundary condition!" );

    return 0;

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
}

#endif
#include <stdexcept>
//-------------------------------------------------------------------------
// RunGEM()
// GEM IPM calculation of equilibrium state for the iNode node
// from array NodT0. Mode - mode of GEMS calculation
//
//  Function returns:
//   0: OK; 1: GEMIPM2K calculation error; 1: system error
//
//-------------------------------------------------------------------

int  TNodeArray::RunGEM( int  iNode, int Mode )
{
//  fstream f_log("ipmlog.txt", ios::out|ios::app );
  try
  {
// f_log << " MAIF_CALC begin Mode= " << p_NodeStatusCH << " iNode= " << iNode << endl;
//---------------------------------------------

   CopyWorkNodeFromArray( iNode, anNodes, NodT0 );
// Unpacking work DATABR structure into MULTI (GEM IPM work structure): uses DATACH
   unpackDataBr();
// set up Mode
   CNode->NodeStatusCH = (short)Mode;
// GEM IPM calculation of equilibrium state in MULTI
    TProfil::pm->calcMulti();
// Extracting and packing GEM IPM results into work DATABR structure
    packDataBr();

/**************************************************************
// only for testing output results for files
    printfGEM( "calc_multi.ipm", "calculated_dbr.dat", "calculated.dbr" );
//********************************************************* */

// Copying data for node iNode back from work DATABR structure into the node array
   MoveWorkNodeToArray( iNode, anNodes, NodT0 );
   if( CNode->NodeStatusCH  == NEED_GEM_AIA )
         CNode->NodeStatusCH = OK_GEM_AIA;
   else
         CNode->NodeStatusCH = OK_GEM_PIA;

//    return 0;
}
    catch(TError& err)
    {
     fstream f_log("ipmlog.txt", ios::out|ios::app );
     f_log << err.title.c_str() << ": " << err.mess.c_str() << endl;
     if( CNode->NodeStatusCH  == NEED_GEM_AIA )
       CNode->NodeStatusCH = BAD_GEM_AIA;
     else
       CNode->NodeStatusCH = BAD_GEM_PIA;

    }
/*    catch( const exception & e )
    {
     fstream f_log("ipmlog.txt", ios::out|ios::app );
     f_log << "Get an exeption: " << e.what() << endl;
       if( CNode->NodeStatusCH  == NEED_GEM_AIA )
         CNode->NodeStatusCH = ERR_GEM_AIA;
       else
         CNode->NodeStatusCH = ERR_GEM_PIA;
//       return -1;
    }
*/    catch(...)
    {
     fstream f_log("ipmlog.txt", ios::out|ios::app );
     f_log << "gems2: Unknown exception: program aborted" << endl;
       if( CNode->NodeStatusCH  == NEED_GEM_AIA )
         CNode->NodeStatusCH = ERR_GEM_AIA;
       else
         CNode->NodeStatusCH = ERR_GEM_PIA;
//       return -1;
    }
//    return 1;
   return CNode->NodeStatusCH;
}


void  TNodeArray::printfGEM( const char* multi_file,
                             const char* databr_text,
                             const char* databr_bin )
{
//**************************************************************
// only for testing output results for files
// binary DATABR
    gstring strr;
   if( databr_bin )
   {  strr = databr_bin;
      GemDataStream out_br(strr, ios::out|ios::binary);
      databr_to_file(out_br);
   }
// text DATABR
   if( databr_text )
   {  fstream out_br_t(databr_text, ios::out );
      ErrorIf( !out_br_t.good() , databr_text,
                "DataBR text file open error");
      databr_to_text_file(out_br_t);
   }
// output multy
    if( multi_file )
   {  strr = multi_file;
      GemDataStream o_m( strr, ios::out|ios::binary);
#ifndef IPMGEMPLUGIN
       o_m.writeArray( &(TProfil::pm->pa.p.PC), 10 );
       o_m.writeArray( &TProfil::pm->pa.p.DG, 28 );
       TProfil::pm->multi->to_file( o_m, strr );
#else
       TProfil::pm->outMulti(o_m, strr );
#endif
    }
//********************************************************* */

}

// Data collection for monitoring differences
// Prints difference increments in a all nodes (cells) for time point t / at
void TNodeArray::logDiffsIC( FILE* diffile, int t, double at, int nx, int every_t )
{
  double dc;
  int i, ie;

  if( t % every_t )
    return;

  fprintf( diffile, "\nStep= %-8d  Time= %-12.4g\nNode#   ", t, at );
  for( ie=0; ie < int(CSD->nICb); ie++ )
    fprintf( diffile, "%-12.4s ", CSD->ICNL[ie] );
  for (i=0; i<nx; i++)    // node iteration
  {
     fprintf( diffile, "\n%5d   ", i );
     for( ie=0; ie < int(CSD->nICb); ie++ )
     {
        dc = NodT1[i]->bIC[ie] - NodT0[i]->bIC[ie];
        fprintf( diffile, "%-12.4g ", dc );
     }
  }
  fprintf( diffile, "\n" );
}

// Data collection for monitoring 1D profiles in debugging FMT models
// Prints dissolved elemental molarities in all cells for time point t / at
void TNodeArray::logProfileAqIC( FILE* logfile, int t, double at, int nx, int every_t )
{
  double pm;
  int i, ie;
  if( t % every_t )
    return;
  fprintf( logfile, "\nStep= %-8d  Time= %-12.4g     Dissolved IC total concentrations, M\n", t, at/(365*86400) );
  fprintf(logfile, "%s","Node#   ");
  for( ie=0; ie < int(CSD->nICb); ie++ )
    fprintf( logfile, "%-12.4s ", CSD->ICNL[ie] );
  for (i=0; i<nx; i++)    // node iteration
  {
     fprintf( logfile, "\n%5d   ", i );
     for( ie=0; ie < int(CSD->nICb); ie++ )
     {
       pm = NodT0[i]->bPS[ie]/NodT0[i]->vPS[0]*1000.;  // Assumes there is aq phase!
                 // total dissolved element molarity
       fprintf( logfile, "%-12.4g ", pm );
     }
  }
  fprintf( logfile, "\n" );
}

// Data collection for monitoring 1D profiles
// Prints total elemental amounts in all cells for time point t / at
void TNodeArray::logProfileTotIC( FILE* logfile, int t, double at, int nx, int every_t )
{
  double pm;
  int i, ie;
  if( t % every_t )
    return;
  fprintf( logfile, "\nStep= %-8d  Time= %-12.4g     Bulk IC amounts, moles\n", t, at/(365*86400) );
  fprintf(logfile, "%s","Node#   ");
  for( ie=0; ie < int(CSD->nICb); ie++ )
    fprintf( logfile, "%-12.4s ", CSD->ICNL[ie] );
  for (i=0; i<nx; i++)    // node iteration
  {
     fprintf( logfile, "\n%5d   ", i );
     for( ie=0; ie < int(CSD->nICb); ie++ )
     {
       pm = NodT0[i]->bIC[ie];
       fprintf( logfile, "%-12.4g ", pm );
     }
  }
  fprintf( logfile, "\n" );
}

// Prints amounts of reactive phases in all cells for time point t / at
void TNodeArray::logProfilePhMol( FILE* logfile, int t, double at, int nx, int every_t )
{
  double pm;
  int i, ip;
  if( t % every_t )
    return;
  fprintf( logfile, "\nStep= %-8d  Time= %-12.4g     Amounts of reactive phases, moles\n", t, at/(365*86400) );
  fprintf(logfile, "%s","Node#   ");
  for( ip=0; ip < int(CSD->nPHb); ip++ )
    fprintf( logfile, "%-12.12s ", CSD->PHNL[ip]+4 );
  for (i=0; i<nx; i++)    // node iteration
  {
     fprintf( logfile, "\n%5d   ", i );
     for( ip=0; ip < int(CSD->nPHb); ip++ )
     {
       pm = NodT0[i]->xPH[ip];
       fprintf( logfile, "%-12.4g ", pm );
     }
  }
  fprintf( logfile, "\n" );
}

// Prints dissolved species molarities in all cells for time point t / at
void TNodeArray::logProfileAqDC( FILE* logfile, int t, double at, int nx, int every_t )
{
  double pm;
  int i, is;
  if( t % every_t )
    return;
  fprintf( logfile, "\nStep= %-8d  Time= %-12.4g     Dissolved species concentrations, M\n", t, at/(365*86400) );
  fprintf(logfile, "%s","Node#   ");
  for( is=0; is < int(CSD->nDCb); is++ )
    fprintf( logfile, "%-12.4s ", CSD->DCNL[is] );
  for (i=0; i<nx; i++)    // node iteration
  {
     fprintf( logfile, "\n%5d   ", i );
     for( is=0; is < int(CSD->nDCinPH[0]); is++ )
     {
       pm = NodT0[i]->xDC[is]/NodT0[i]->vPS[0]*1000.;  // Assumes there is aq phase!
                 // dissolved species molarity
       fprintf( logfile, "%-12.4g ", pm );
     }
  }
  fprintf( logfile, "\n" );
}

// Copying data for node ii from node array into work DATABR structure
void TNodeArray::CopyWorkNodeFromArray( int ii, int nNodes, DATABRPTR* arr_BR )
{
  // from arr_BR[ii] to CNode structure
  if( ii < 0 || ii>= nNodes )
    return;
  // memory must be allocated before

  memcpy( &CNode->NodeHandle, &arr_BR[ii]->NodeHandle, 6*sizeof(short));
  memcpy( &CNode->T, &arr_BR[ii]->T, 36*sizeof(double));
// Dynamic data - dimensions see in DATACH.H and DATAMT.H structures
// exchange of values occurs through lists of indices, e.g. xDC, xPH
  memcpy( CNode->xDC, arr_BR[ii]->xDC, CSD->nDCb*sizeof(double) );
  memcpy( CNode->gam, arr_BR[ii]->gam, CSD->nDCb*sizeof(double) );
  memcpy( CNode->xPH, arr_BR[ii]->xPH, CSD->nPHb*sizeof(double) );
  memcpy( CNode->vPS, arr_BR[ii]->vPS, CSD->nPSb*sizeof(double) );
  memcpy( CNode->mPS, arr_BR[ii]->mPS, CSD->nPSb*sizeof(double) );

  memcpy( CNode->bPS, arr_BR[ii]->bPS,
                          CSD->nPSb*CSD->nICb*sizeof(double) );
  memcpy( CNode->xPA, arr_BR[ii]->xPA, CSD->nPSb*sizeof(double) );
  memcpy( CNode->dul, arr_BR[ii]->dul, CSD->nDCb*sizeof(double) );
  memcpy( CNode->dll, arr_BR[ii]->dll, CSD->nDCb*sizeof(double) );
  memcpy( CNode->bIC, arr_BR[ii]->bIC, CSD->nICb*sizeof(double) );
  memcpy( CNode->rMB, arr_BR[ii]->rMB, CSD->nICb*sizeof(double) );
  memcpy( CNode->uIC, arr_BR[ii]->uIC, CSD->nICb*sizeof(double) );
  CNode->dRes1 = 0;
  CNode->dRes2 = 0;
}

// Copying data for node iNode back from work DATABR structure into the node array
void TNodeArray::MoveWorkNodeToArray( int ii, int nNodes, DATABRPTR* arr_BR )
{
  if( ii < 0 || ii>= nNodes )
    return;
  if( arr_BR[ii] )
  {
       arr_BR[ii] = databr_free( arr_BR[ii]);
       // delete[] arr_BR[ii];
  }
  arr_BR[ii] = CNode;
// alloc new memory
  CNode = new DATABR;
  databr_realloc();
  memset( &CNode->T, 0, 36*sizeof(double));
}

void TNodeArray::CopyNodeFromTo( int ndx, int nNod,
                       DATABRPTR* arr_From, DATABRPTR* arr_To )
{
  if( !arr_From || !arr_To )
      return;
  CopyWorkNodeFromArray( ndx, nNod, arr_From );
  MoveWorkNodeToArray( ndx,  nNod, arr_To );
}

// Extracting and packing GEM IPM results into work DATABR structure
void TNodeArray::packDataBr()
{
 short ii;

// set default data to DataBr
#ifndef IPMGEMPLUGIN
   CNode->NodeHandle = 0;
   CNode->NodeTypeHY = initital;
   CNode->NodeTypeMT = normal;
   CNode->NodeStatusFMT = Initial_RUN;
   //   CNode->NodeStatusCH = NEED_GEM_AIA;
   if( pmm->pNP == 0 )
    CNode->NodeStatusCH = NEED_GEM_AIA;
  else
     CNode->NodeStatusCH = NEED_GEM_PIA;
#else

 // numbers
  if( pmm->pNP == 0 )
    CNode->NodeStatusCH = OK_GEM_AIA;
  else
    CNode->NodeStatusCH = OK_GEM_PIA;

#endif

   CNode->T = pmm->Tc; //25
   CNode->P = pmm->Pc; //1
   CNode->IterDone = pmm->IT;

// values
  CNode->Vs = pmm->VXc;
  CNode->Gs = pmm->FX;
  CNode->Hs = pmm->HXc;
  CNode->IC = pmm->IC;
  CNode->pH = pmm->pH;
  CNode->pe = pmm->pe;
//  CNode->Eh = pmm->Eh;
  CNode->Eh = pmm->FitVar[3];
  CNode->denW = pmm->denW;
  CNode->denWg = pmm->denWg;
  CNode->epsW = pmm->epsW;
  CNode->epsWg = pmm->epsWg;
  CNode->Ms = pmm->MBX;

  // arrays
   for( ii=0; ii<CSD->nDCb; ii++ )
    CNode->xDC[ii] = pmm->X[ CSD->xDC[ii] ];
   for( ii=0; ii<CSD->nDCb; ii++ )
    CNode->gam[ii] = pmm->Gamma[ CSD->xDC[ii] ];

   for( ii=0; ii<CSD->nPHb; ii++ )
    CNode->xPH[ii] = pmm->XF[ CSD->xPH[ii] ];
   for( ii=0; ii<CSD->nPSb; ii++ )
    CNode->vPS[ii] = pmm->FVOL[ CSD->xPH[ii] ];
   for( ii=0; ii<CSD->nPSb; ii++ )
    CNode->mPS[ii] = pmm->FWGT[ CSD->xPH[ii] ];

   for( ii=0; ii<CSD->nPSb; ii++ )
   for(short jj=0; jj<CSD->nICb; jj++ )
   { int   new_ndx= (ii*CSD->nICb)+jj,
           mul_ndx = ( CSD->xPH[ii]*CSD->nIC )+ CSD->xIC[jj];
     CNode->bPS[new_ndx] = pmm->BF[ mul_ndx ];
   }
   for( ii=0; ii<CSD->nPSb; ii++ )
    CNode->xPA[ii] = pmm->XFA[ CSD->xPH[ii] ];

   for( ii=0; ii<CSD->nDCb; ii++ )          //??? only insert
   {
    CNode->dul[ii] = pmm->DUL[ CSD->xDC[ii] ];
    CNode->dll[ii] = pmm->DLL[ CSD->xDC[ii] ];
   }
   for( ii=0; ii<CSD->nICb; ii++ )          //??? only insert
    CNode->bIC[ii] = pmm->B[ CSD->xIC[ii] ];
   for( ii=0; ii<CSD->nICb; ii++ )
    CNode->rMB[ii] = pmm->C[ CSD->xIC[ii] ];
   for( ii=0; ii<CSD->nICb; ii++ )
    CNode->uIC[ii] = pmm->U[ CSD->xIC[ii] ];

}

// Unpacking work DATABR structure into MULTI
//(GEM IPM work structure): uses DATACH
void TNodeArray::unpackDataBr()
{
 short ii;
 double Gamm;
// numbers

  if( CNode->NodeStatusCH >= NEED_GEM_PIA )
   pmm->pNP = 1;
  else
   pmm->pNP = 0; //  NEED_GEM_AIA;
  CNode->IterDone = 0;
  pmm->IT = 0;
// values
  pmm->Tc = CNode->T;
  pmm->Pc  = CNode->P;
  pmm->MBX = CNode->Ms;
  pmm->IC = CNode->IC;
pmm->FitVar[3] = CNode->Eh;
// arrays
   for( ii=0; ii<CSD->nDCb; ii++ )
   {
    pmm->DUL[ CSD->xDC[ii] ] = CNode->dul[ii];
    pmm->DLL[ CSD->xDC[ii] ] = CNode->dll[ii];
   }
   for( ii=0; ii<CSD->nICb; ii++ )
    pmm->B[ CSD->xIC[ii] ] = CNode->bIC[ii];

// added  to compare SD 15/07/04
   for( ii=0; ii<CSD->nDCb; ii++ )
    pmm->X[ CSD->xDC[ii] ] = pmm->Y[ CSD->xDC[ii] ] = CNode->xDC[ii];
   for( ii=0; ii<CSD->nDCb; ii++ )
   {
      Gamm = CNode->gam[ii];
      pmm->Gamma[ CSD->xDC[ii] ] = Gamm;
      pmm->lnGmo[ CSD->xDC[ii] ] = pmm->lnGam[ CSD->xDC[ii] ] = log(Gamm);
   }
   for( ii=0; ii<CSD->nPHb; ii++ )
    pmm->XF[ CSD->xPH[ii] ] = pmm->YF[ CSD->xPH[ii] ] = CNode->xPH[ii];
   for( ii=0; ii<CSD->nPSb; ii++ )
    pmm->FVOL[ CSD->xPH[ii] ] = CNode->vPS[ii];
   for( ii=0; ii<CSD->nPSb; ii++ )
    pmm->FWGT[ CSD->xPH[ii] ] = CNode->mPS[ii];

   for( ii=0; ii<CSD->nPSb; ii++ )
   for(short jj=0; jj<CSD->nICb; jj++ )
   { int new_ndx= (ii*CSD->nICb)+jj,
           mul_ndx = ( CSD->xPH[ii]*CSD->nIC )+ CSD->xIC[jj];
     pmm->BF[ mul_ndx ] = CNode->bPS[new_ndx];
   }
   for( ii=0; ii<CSD->nPSb; ii++ )
    pmm->XFA[ CSD->xPH[ii] ] = pmm->YFA[ CSD->xPH[ii] ] = CNode->xPA[ii];

   for( ii=0; ii<CSD->nICb; ii++ )
    pmm->C[ CSD->xIC[ii] ] = CNode->rMB[ii];
   for( ii=0; ii<CSD->nICb; ii++ )
    pmm->U[ CSD->xIC[ii] ] = CNode->uIC[ii];

}

//---------------------------------------------------------------

// new structures i/o

// Writting DataCH to binary file
void TNodeArray::datach_to_file( GemDataStream& ff )
{
// const data
   ff.writeArray( &CSD->nIC, 14 );
   ff.writeArray( &CSD->Ttol, 4 );

//dynamic data
   ff.writeArray( CSD->nDCinPH, CSD->nPH );
//   if( CSD->nICb >0 )
   ff.writeArray( CSD->xIC, CSD->nICb );
   ff.writeArray( CSD->xDC, CSD->nDCb );
   ff.writeArray( CSD->xPH, CSD->nPHb );

   ff.writeArray( CSD->A, CSD->nIC*CSD->nDC );
   ff.writeArray( CSD->ICmm, CSD->nIC );
   ff.writeArray( CSD->DCmm, CSD->nDC );
   ff.writeArray( CSD->DD, CSD->nDC );

   ff.writeArray( CSD->Tval,  CSD->nTp );
   ff.writeArray( CSD->Pval,  CSD->nPp );

   ff.writeArray( CSD->roW,  CSD->nPp*CSD->nTp );
   ff.writeArray( CSD->epsW, CSD->nPp*CSD->nTp );
   ff.writeArray( CSD->G0,  CSD->nDC*CSD->nPp*CSD->nTp );
   ff.writeArray( CSD->V0,  CSD->nDC*CSD->nPp*CSD->nTp );
   ff.writeArray( CSD->H0,  CSD->nDC*CSD->nPp*CSD->nTp );
   ff.writeArray( CSD->Cp0, CSD->nDC*CSD->nPp*CSD->nTp );

   if( CSD->nAalp >0 )
     ff.writeArray( CSD->Aalp, CSD->nPH );

   ff.writeArray( (char *)CSD->ICNL, MaxICN*CSD->nIC );
   ff.writeArray( (char *)CSD->DCNL, MaxDCN*CSD->nDC );
   ff.writeArray( (char *)CSD->PHNL, MaxPHN*CSD->nPH );

   ff.writeArray( CSD->ccIC, CSD->nIC );
   ff.writeArray( CSD->ccDC, CSD->nDC );
   ff.writeArray( CSD->ccDCW, CSD->nDC );
   ff.writeArray( CSD->ccPH, CSD->nPH );

}

// Reading dataCH structure from binary file
void TNodeArray::datach_from_file( GemDataStream& ff )
{
// const data
   ff.readArray( &CSD->nIC, 14 );
   ff.readArray( &CSD->Ttol, 4 );

  datach_realloc();
  databr_realloc();

//dynamic data
   ff.readArray( CSD->nDCinPH, CSD->nPH );
//   if( CSD->nICb >0 )
   ff.readArray( CSD->xIC, CSD->nICb );
   ff.readArray( CSD->xDC, CSD->nDCb );
   ff.readArray( CSD->xPH, CSD->nPHb );

   ff.readArray( CSD->A, CSD->nIC*CSD->nDC );
   ff.readArray( CSD->ICmm, CSD->nIC );
   ff.readArray( CSD->DCmm, CSD->nDC );
   ff.readArray( CSD->DD, CSD->nDC );

   ff.readArray( CSD->Tval,  CSD->nTp );
   ff.readArray( CSD->Pval,  CSD->nPp );

   ff.readArray( CSD->roW,  CSD->nPp*CSD->nTp );
   ff.readArray( CSD->epsW, CSD->nPp*CSD->nTp );
   ff.readArray( CSD->G0,  CSD->nDC*CSD->nPp*CSD->nTp );
   ff.readArray( CSD->V0,  CSD->nDC*CSD->nPp*CSD->nTp );
   ff.readArray( CSD->H0,  CSD->nDC*CSD->nPp*CSD->nTp );
   ff.readArray( CSD->Cp0, CSD->nDC*CSD->nPp*CSD->nTp );

   if( CSD->nAalp >0 )
     ff.readArray( CSD->Aalp, CSD->nPH );

   ff.readArray( (char *)CSD->ICNL, MaxICN*CSD->nIC );
   ff.readArray( (char *)CSD->DCNL, MaxDCN*CSD->nDC );
   ff.readArray( (char *)CSD->PHNL, MaxPHN*CSD->nPH );

   ff.readArray( CSD->ccIC, CSD->nIC );
   ff.readArray( CSD->ccDC, CSD->nDC );
   ff.readArray( CSD->ccDCW, CSD->nDC );
   ff.readArray( CSD->ccPH, CSD->nPH );

}

void TNodeArray::datach_to_text_file( fstream& ff )
{
// fstream ff("DataCH.out", ios::out );
// ErrorIf( !ff.good() , "DataCH.out", "Fileopen error");

  outArray( ff, "sCon",  &CSD->nIC, 14 );
  outArray( ff, "dCon",  &CSD->Ttol, 4 );

//dynamic data
   outArray( ff, "nDCinPH", CSD->nDCinPH, CSD->nPH);
//   if( CSD->nICb >0 )
   outArray( ff, "xIC", CSD->xIC, CSD->nICb);
   outArray( ff, "xDC", CSD->xDC, CSD->nDCb);
   outArray( ff, "xPH", CSD->xPH, CSD->nPHb);

   outArray( ff, "A", CSD->A, CSD->nDC*CSD->nIC, CSD->nIC );
   outArray( ff, "ICmm", CSD->ICmm, CSD->nIC);
   outArray( ff, "DCmm", CSD->DCmm, CSD->nDC);
   outArray( ff, "DD", CSD->DD, CSD->nDC);

   outArray( ff, "Tval", CSD->Tval, CSD->nTp );
   outArray( ff, "Pval", CSD->Pval, CSD->nPp );

   outArray( ff, "roW", CSD->roW, CSD->nPp*CSD->nTp );
   outArray( ff, "epsW", CSD->epsW,  CSD->nPp*CSD->nTp );
   outArray( ff, "G0", CSD->G0, CSD->nDC*CSD->nPp*CSD->nTp,
                                    CSD->nPp*CSD->nTp );
   outArray( ff, "V0", CSD->V0,  CSD->nDC*CSD->nPp*CSD->nTp,
                                    CSD->nPp*CSD->nTp );
   outArray( ff, "H0", CSD->H0,  CSD->nDC*CSD->nPp*CSD->nTp,
                                     CSD->nPp*CSD->nTp );
   outArray( ff, "Cp0", CSD->Cp0,CSD->nDC*CSD->nPp*CSD->nTp,
                                     CSD->nPp*CSD->nTp  );

   if( CSD->nAalp >0 )
      outArray( ff, "Aalp", CSD->Aalp, CSD->nPH);

   outArray( ff, "ICNL", CSD->ICNL[0], CSD->nIC, MaxICN );
   outArray( ff, "DCNL", CSD->DCNL[0], CSD->nDC, MaxDCN );
   outArray( ff, "PHNL", CSD->PHNL[0], CSD->nPH, MaxPHN );

   outArray( ff, "ccIC", CSD->ccIC, CSD->nIC, 1 );
   outArray( ff, "ccDC", CSD->ccDC, CSD->nDC, 1 );
   outArray( ff, "ccDCW", CSD->ccDCW, CSD->nDC, 1 );
   outArray( ff, "ccPH", CSD->ccPH, CSD->nPH, 1 );

}

// Reading dataCH structure from text file
void TNodeArray::datach_from_text_file(fstream& ff)
{
// fstream ff("DataCH.out", ios::in );
// ErrorIf( !ff.good() , "DataCH.out", "Fileopen error");

  inArray( ff, "sCon",  &CSD->nIC, 14 );
  inArray( ff, "dCon",  &CSD->Ttol, 4 );

  datach_realloc();
  databr_realloc();

//dynamic data
   inArray( ff, "nDCinPH", CSD->nDCinPH, CSD->nPH);
//   if( CSD->nICb >0 )
   inArray( ff, "xIC", CSD->xIC, CSD->nICb);
   inArray( ff, "xDC", CSD->xDC, CSD->nDCb);
   inArray( ff, "xPH", CSD->xPH, CSD->nPHb);

   inArray( ff, "A", CSD->A, CSD->nDC*CSD->nIC );
   inArray( ff, "ICmm", CSD->ICmm, CSD->nIC);
   inArray( ff, "DCmm", CSD->DCmm, CSD->nDC);
   inArray( ff, "DD", CSD->DD, CSD->nDC);

   inArray( ff, "Tval", CSD->Tval, CSD->nTp );
   inArray( ff, "Pval", CSD->Pval, CSD->nPp );

   inArray( ff, "roW", CSD->roW,   CSD->nPp*CSD->nTp);
   inArray( ff, "epsW", CSD->epsW, CSD->nPp*CSD->nTp);
   inArray( ff, "G0", CSD->G0,  CSD->nDC*CSD->nPp*CSD->nTp);
   inArray( ff, "V0", CSD->V0,  CSD->nDC*CSD->nPp*CSD->nTp);
   inArray( ff, "H0", CSD->H0,  CSD->nDC*CSD->nPp*CSD->nTp);
   inArray( ff, "Cp0", CSD->Cp0,CSD->nDC*CSD->nPp*CSD->nTp);

   if( CSD->nAalp >0 )
      inArray( ff, "Aalp", CSD->Aalp, CSD->nPH);

   inArray( ff, "ICNL", CSD->ICNL[0], CSD->nIC, MaxICN );
   inArray( ff, "DCNL", CSD->DCNL[0], CSD->nDC, MaxDCN );
   inArray( ff, "PHNL", CSD->PHNL[0], CSD->nPH, MaxPHN );

   inArray( ff, "ccIC", CSD->ccIC, CSD->nIC, 1 );
   inArray( ff, "ccDC", CSD->ccDC, CSD->nDC, 1 );
   inArray( ff, "ccDCW", CSD->ccDCW, CSD->nDC, 1 );
   inArray( ff, "ccPH", CSD->ccPH, CSD->nPH, 1 );
}

// allocate DataCH structure
void TNodeArray::datach_realloc()
{
 CSD->nDCinPH = new short[CSD->nPH];

 if( CSD->nICb >0 )
   CSD->xIC = new short[CSD->nICb];
 else  CSD->xIC = 0;
 if( CSD->nDCb >0 )
   CSD->xDC = new short[CSD->nDCb];
 else  CSD->xDC = 0;
 if( CSD->nPHb >0 )
   CSD->xPH = new short[CSD->nPHb];
 else  CSD->xPH = 0;

  CSD->A = new float[CSD->nIC*CSD->nDC];
  CSD->ICmm = new double[CSD->nIC];
  CSD->DCmm = new double[CSD->nDC];
  CSD->DD = new double[CSD->nDC];

  CSD->Tval = new float[CSD->nTp];
  CSD->Pval = new float[CSD->nPp];

  CSD->roW = new double[ CSD->nPp*CSD->nTp];
  CSD->epsW = new double[ CSD->nPp*CSD->nTp];
  CSD->G0 = new double[CSD->nDC*CSD->nPp*CSD->nTp];
  CSD->V0 = new double[CSD->nDC*CSD->nPp*CSD->nTp];
  CSD->H0 = new double[CSD->nDC*CSD->nPp*CSD->nTp];
  CSD->Cp0 = new double[CSD->nDC*CSD->nPp*CSD->nTp];

  if( CSD->nAalp >0 )
     CSD->Aalp = new double[CSD->nPH];
  else CSD->Aalp = 0;

  CSD->ICNL = new char[CSD->nIC][MaxICN];
  CSD->DCNL = new char[CSD->nDC][MaxDCN];
  CSD->PHNL = new char[CSD->nPH][MaxPHN];

  CSD->ccIC = new char[CSD->nIC];
  CSD->ccDC = new char[CSD->nDC];
  CSD->ccDCW = new char[CSD->nDC];
  CSD->ccPH = new char[CSD->nPH];
}

// free dynamic memory
void TNodeArray::datach_free()
{
 if( CSD->nDCinPH )
  { delete[] CSD->nDCinPH;
    CSD->nDCinPH = 0;
  }
 if( CSD->xIC )
  { delete[] CSD->xIC;
    CSD->xIC = 0;
  }
 if( CSD->xDC )
  { delete[] CSD->xDC;
    CSD->xDC = 0;
  }
 if( CSD->xPH )
  { delete[] CSD->xPH;
    CSD->xPH = 0;
  }
 if( CSD->A )
  { delete[] CSD->A;
    CSD->A = 0;
  }
 if( CSD->ICmm )
  { delete[] CSD->ICmm;
    CSD->ICmm = 0;
  }
 if( CSD->DCmm )
  { delete[] CSD->DCmm;
    CSD->DCmm = 0;
  }
 if( CSD->DD )
  { delete[] CSD->DD;
    CSD->DD = 0;
  }

 if( CSD->Tval )
  { delete[] CSD->Tval;
    CSD->Tval = 0;
  }
 if( CSD->Pval )
  { delete[] CSD->Pval;
    CSD->Pval = 0;
  }

 if( CSD->roW )
  { delete[] CSD->roW;
    CSD->roW = 0;
  }
 if( CSD->epsW )
  { delete[] CSD->epsW;
    CSD->epsW = 0;
  }
 if( CSD->G0 )
  { delete[] CSD->G0;
    CSD->G0 = 0;
  }
 if( CSD->V0 )
  { delete[] CSD->V0;
    CSD->V0 = 0;
  }
 if( CSD->H0 )
  { delete[] CSD->H0;
    CSD->H0 = 0;
  }
 if( CSD->Cp0 )
  { delete[] CSD->Cp0;
    CSD->Cp0 = 0;
  }
 if( CSD->Aalp )
  { delete[] CSD->Aalp;
    CSD->Aalp = 0;
  }

 if( CSD->ICNL )
  { delete[] CSD->ICNL;
    CSD->ICNL = 0;
  }
 if( CSD->DCNL )
  { delete[] CSD->DCNL;
    CSD->DCNL = 0;
  }
 if( CSD->PHNL )
  { delete[] CSD->PHNL;
    CSD->PHNL = 0;
  }

 if( CSD->ccIC )
  { delete[] CSD->ccIC;
    CSD->ccIC = 0;
  }
 if( CSD->ccDC )
  { delete[] CSD->ccDC;
    CSD->ccDC = 0;
  }
 if( CSD->ccDCW )
  { delete[] CSD->ccDCW;
    CSD->ccDCW = 0;
  }
 if( CSD->ccPH )
  { delete[] CSD->ccPH;
    CSD->ccPH = 0;
  }
 // delete[] CSD;
}

// writing DataBR to binary file
void TNodeArray::databr_to_file( GemDataStream& ff )
{
// const data
   ff.writeArray( &CNode->NodeHandle, 6 );
   ff.writeArray( &CNode->T, 36 );

//dynamic data
   ff.writeArray( CNode->xDC, CSD->nDCb );
   ff.writeArray( CNode->gam, CSD->nDCb );
   ff.writeArray( CNode->xPH, CSD->nPHb );
   ff.writeArray( CNode->vPS, CSD->nPSb );
   ff.writeArray( CNode->mPS, CSD->nPSb );

   ff.writeArray( CNode->bPS, CSD->nPSb*CSD->nICb );
   ff.writeArray( CNode->xPA, CSD->nPSb );
   ff.writeArray( CNode->dul, CSD->nDCb );
   ff.writeArray( CNode->dll, CSD->nDCb );
   ff.writeArray( CNode->bIC, CSD->nICb );
   ff.writeArray( CNode->rMB, CSD->nICb );
   ff.writeArray( CNode->uIC, CSD->nICb );

   CNode->dRes1 = 0;
   CNode->dRes2 = 0;

//   datach_to_text_file();
//   databr_to_text_file();
}

// Reading work dataBR structure from binary file
void TNodeArray::databr_from_file( GemDataStream& ff )
{
// const data
   ff.readArray( &CNode->NodeHandle, 6 );
   ff.readArray( &CNode->T, 36 );

//dynamic data
   ff.readArray( CNode->xDC, CSD->nDCb );
   ff.readArray( CNode->gam, CSD->nDCb );
   ff.readArray( CNode->xPH, CSD->nPHb );
   ff.readArray( CNode->vPS, CSD->nPSb );
   ff.readArray( CNode->mPS, CSD->nPSb );

   ff.readArray( CNode->bPS, CSD->nPSb*CSD->nICb );
   ff.readArray( CNode->xPA, CSD->nPSb );
   ff.readArray( CNode->dul, CSD->nDCb );
   ff.readArray( CNode->dll, CSD->nDCb );
   ff.readArray( CNode->bIC, CSD->nICb );
   ff.readArray( CNode->rMB, CSD->nICb );
   ff.readArray( CNode->uIC, CSD->nICb );

   CNode->dRes1 = 0;
   CNode->dRes2 = 0;
}


void TNodeArray::databr_to_text_file( fstream& ff )
{
// fstream ff("DataBR.out", ios::out );
// ErrorIf( !ff.good() , "DataCH.out", "Fileopen error");

  outArray( ff, "sCon",  &CNode->NodeHandle, 6 );
  outArray( ff, "dCon",  &CNode->T, 36 );

  outArray( ff, "xDC",  CNode->xDC, CSD->nDCb );
  outArray( ff, "gam",  CNode->gam, CSD->nDCb );
  outArray( ff, "xPH",  CNode->xPH, CSD->nPHb );
  outArray( ff, "vPS",  CNode->vPS, CSD->nPSb );
  outArray( ff, "mPS",  CNode->mPS, CSD->nPSb );


  outArray( ff, "bPS",  CNode->bPS, CSD->nPSb*CSD->nICb );
  outArray( ff, "xPA",  CNode->xPA, CSD->nPSb );
  outArray( ff, "dul",  CNode->dul, CSD->nDCb );
  outArray( ff, "dll",  CNode->dll, CSD->nDCb );
  outArray( ff, "bIC",  CNode->bIC, CSD->nICb );
  outArray( ff, "rMB",  CNode->rMB, CSD->nICb );
  outArray( ff, "uIC",  CNode->uIC, CSD->nICb );

}

// Reading work dataBR structure from text file
void TNodeArray::databr_from_text_file( fstream& ff )
{
// fstream ff("DataBR.out", ios::out );
// ErrorIf( !ff.good() , "DataCH.out", "Fileopen error");

  inArray( ff, "sCon",  &CNode->NodeHandle, 6 );
  inArray( ff, "dCon",  &CNode->T, 36 );

  inArray( ff, "xDC",  CNode->xDC, CSD->nDCb );
  inArray( ff, "gam",  CNode->gam, CSD->nDCb );
  inArray( ff, "xPH",  CNode->xPH, CSD->nPHb );
  inArray( ff, "vPS",  CNode->vPS, CSD->nPSb );
  inArray( ff, "mPS",  CNode->mPS, CSD->nPSb );

  inArray( ff, "bPS",  CNode->bPS, CSD->nPSb*CSD->nICb );
  inArray( ff, "xPA",  CNode->xPA, CSD->nPSb );
  inArray( ff, "dul",  CNode->dul, CSD->nDCb );
  inArray( ff, "dll",  CNode->dll, CSD->nDCb );
  inArray( ff, "bIC",  CNode->bIC, CSD->nICb );
  inArray( ff, "rMB",  CNode->rMB, CSD->nICb );
  inArray( ff, "uIC",  CNode->uIC, CSD->nICb );
}


// allocate DataBR structure
void TNodeArray::databr_realloc()
{
 CNode->xDC = new double[CSD->nDCb];
 CNode->gam = new double[CSD->nDCb];
 CNode->xPH = new double[CSD->nPHb];
 CNode->vPS = new double[CSD->nPSb];
 CNode->mPS = new double[CSD->nPSb];

 CNode->bPS = new double[CSD->nPSb*CSD->nICb];
 CNode->xPA = new double[CSD->nPSb];
 CNode->dul = new double[CSD->nDCb];
 CNode->dll = new double[CSD->nDCb];
 CNode->bIC = new double[CSD->nICb];
 CNode->rMB = new double[CSD->nICb];
 CNode->uIC = new double[CSD->nICb];

 CNode->dRes1 = 0;
 CNode->dRes2 = 0;
}

// free dynamic memory
DATABR * TNodeArray::databr_free( DATABR *CNode_ )
{

  if( CNode_ == 0)
    CNode_ = CNode;
  memset( &CNode_->NodeHandle, 0, 6*sizeof(short));
  memset( &CNode_->T, 0, 36*sizeof(double));

 if( CNode_->xDC )
  { delete[] CNode_->xDC;
    CNode_->xDC = 0;
  }
 if( CNode_->gam )
  { delete[] CNode_->gam;
    CNode_->gam = 0;
  }
 if( CNode_->xPH )
  { delete[] CNode_->xPH;
    CNode_->xPH = 0;
  }
 if( CNode_->vPS )
  { delete[] CNode_->vPS;
    CNode_->vPS = 0;
  }
 if( CNode_->mPS )
  { delete[] CNode_->mPS;
    CNode_->mPS = 0;
  }

 if( CNode_->bPS )
  { delete[] CNode_->bPS;
    CNode_->bPS = 0;
  }
 if( CNode_->xPA )
  { delete[] CNode_->xPA;
    CNode_->xPA = 0;
  }
 if( CNode_->dul )
  { delete[] CNode_->dul;
    CNode_->dul = 0;
  }
 if( CNode_->dll )
  { delete[] CNode_->dll;
    CNode_->dll = 0;
  }
 if( CNode_->bIC )
  { delete[] CNode_->bIC;
    CNode_->bIC = 0;
  }
 if( CNode_->rMB )
  { delete[] CNode_->rMB;
    CNode_->rMB = 0;
  }
 if( CNode_->uIC )
  { delete[] CNode_->uIC;
    CNode_->uIC = 0;
  }

 delete[] CNode_;
 return NULL;
}

#ifdef IPMGEMPLUGIN

// calculation mode: passing input GEM data changed on previous FMT iteration
//                   into work DATABR structure
void TNodeArray::GEM_input_from_MT(
   short p_NodeHandle,    // Node identification handle
   short p_NodeStatusCH,  // Node status code CH;  see typedef NODECODECH
   double p_T,     // Temperature T, K                        +      +      -     -
   double p_P,     // Pressure P, bar                         +      +      -     -
   double p_Ms,    // Mass of reactive subsystem, kg          +      +      -     -
   double p_dt,    // actual time step
   double p_dt1,   // priveous time step
   double  *p_dul,  // upper kinetic restrictions [nDCb]            +      +      -     -
   double  *p_dll,  // lower kinetic restrictions [nDCb]            +      +      -     -
   double  *p_bIC  // bulk mole amounts of IC[nICb]                +      +      -     -
   )
{
  int ii;
  bool useSimplex = false;

  CNode->NodeHandle = p_NodeHandle;
  CNode->NodeStatusCH = p_NodeStatusCH;
  CNode->T = p_T;
  CNode->P = p_P;
  CNode->Ms = p_Ms;
  CNode->dt = p_dt;
  CNode->dt1 = p_dt1;
// Checking if no-simplex IA is Ok
   for( ii=0; ii<CSD->nICb; ii++ )
   {  //  Sveta 11/02/05 for test
      //if( fabs(CNode->bIC[ii] - p_bIC[ii] ) > CNode->bIC[ii]*1e-4 ) // bugfix KD 21.11.04
       //     useSimplex = true;
     CNode->bIC[ii] = p_bIC[ii];
   }
   for( ii=0; ii<CSD->nDCb; ii++ )
   {
     CNode->dul[ii] = p_dul[ii];
     CNode->dll[ii] = p_dll[ii];
   }
   if( useSimplex && CNode->NodeStatusCH == NEED_GEM_PIA )
     CNode->NodeStatusCH = NEED_GEM_AIA;
   // Switch only if PIA is ordered, leave if simplex is ordered (KD)
}

// readonly mode: passing input GEM data to FMT
void TNodeArray::GEM_input_back_to_MT(
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
   )
{
 int ii;
  p_NodeHandle = CNode->NodeHandle;
  p_NodeStatusCH = CNode->NodeStatusCH;
  p_T = CNode->T;
  p_P = CNode->P;
  p_Ms = CNode->Ms;
  p_dt = CNode->dt;
  p_dt1 = CNode->dt1;
// Checking if no-simplex IA is Ok
   for( ii=0; ii<CSD->nICb; ii++ )
     p_bIC[ii] = CNode->bIC[ii];
   for( ii=0; ii<CSD->nDCb; ii++ )
   {  p_dul[ii] = CNode->dul[ii];
      p_dll[ii] = CNode->dll[ii];
   }
}

// Copying results that must be returned into the FMT part into MAIF_CALC parameters
void TNodeArray::GEM_output_to_MT(
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
   double  *p_dul,  // upper kinetic restrictions [nDCb]            +      +      -     -
   double  *p_dll,  // lower kinetic restrictions [nDCb]            +      +      -     -
   double  *p_bIC,  // bulk mole amounts of IC[nICb]                +      +      -     -
   double  *p_rMB,  // MB Residuals from GEM IPM [nICb]             -      -      +     +
   double  *p_uIC  // IC chemical potentials (mol/mol)[nICb]       -      -      +     +
   )
{

   p_NodeHandle = CNode->NodeHandle;
   p_NodeStatusCH = CNode->NodeStatusCH;
   p_IterDone = CNode->IterDone;

   p_Vs = CNode->Vs;
   p_Gs = CNode->Gs;
   p_Hs = CNode->Hs;
   p_IC = CNode->IC;
   p_pH = CNode->pH;
   p_pe = CNode->pe;
   p_Eh = CNode->Eh;
   p_denW = CNode->denW;
   p_denWg = CNode->denWg;
   p_epsW = CNode->epsW;
   p_epsWg = CNode->epsWg;

  memcpy( p_xDC, CNode->xDC, CSD->nDCb*sizeof(double) );
  memcpy( p_gam, CNode->gam, CSD->nDCb*sizeof(double) );
  memcpy( p_xPH, CNode->xPH, CSD->nPHb*sizeof(double) );
  memcpy( p_vPS, CNode->vPS, CSD->nPSb*sizeof(double) );
  memcpy( p_mPS, CNode->mPS, CSD->nPSb*sizeof(double) );
  memcpy( p_bPS, CNode->bPS, CSD->nPSb*CSD->nICb*sizeof(double) );
  memcpy( p_xPA, CNode->xPA, CSD->nPSb*sizeof(double) );
  memcpy( p_dul, CNode->dul, CSD->nDCb*sizeof(double) );
  memcpy( p_dll, CNode->dll, CSD->nDCb*sizeof(double) );
  memcpy( p_bIC, CNode->bIC, CSD->nICb*sizeof(double) );
  memcpy( p_rMB, CNode->rMB, CSD->nICb*sizeof(double) );
  memcpy( p_uIC, CNode->uIC, CSD->nICb*sizeof(double) );
}

#endif

//-----------------------End of tnodearray.cpp--------------------------

