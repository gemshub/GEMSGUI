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

#ifndef __unix
#include <io.h>
#endif

#ifndef IPMGEMPLUGIN
  #include "service.h"
  #include "visor.h"
#else
  istream& f_getline(istream& is, gstring& str, char delim);
  TNodeArray* TNodeArray::na;
#endif


//---------------------------------------------------------//

void TNodeArray::allocMemory()
{
  int ii;
// alloc memory for data bidge structures
    CSD = new DATACH;
    CNode = new DATABR;

    memset( CSD, 0, sizeof(DATACH) );
    memset( CNode, 0, sizeof(DATABR) );

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

  if( grid )
     delete[] grid;
  if( tNode)
     delete[] tNode;

#ifdef IPMGEMPLUGIN
  delete[] multi;
  delete[] profil;
#endif

}


#ifndef IPMGEMPLUGIN

// Make start DATACH and DATABR data from GEMS internal data (MULTI and other)
void TNodeArray::MakeNodeStructures(
    short anICb,       // number of stoichiometry units (<= nIC) used in the data bridge
    short anDCb,      	// number of DC (chemical species, <= nDC) used in the data bridge
    short anPHb,     	// number of phases (<= nPH) used in the data bridge
    short* axIC,   // ICNL indices in DATABR IC vectors [nICb]
    short* axDC,   // DCNL indices in DATABR DC list [nDCb]
    short* axPH,   // PHNL indices in DATABR phase vectors [nPHb]
    float* Tai, float* Pai,
    short nTp_, short nPp_, float Ttol_, float Ptol_  )
{
  short ii;
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
   makeStartDataChBR( aSelIC, aSelDC, aSelPH,
                      nTp_, nPp_, Ttol_, Ptol_, Tai, Pai );
}

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
gstring TNodeArray::PutGEM2MTFiles( QWidget* par, int nIV,
      bool textmode, bool binmode, bool putNodT1  )
{
  if( !textmode && !binmode )
    return "";

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
               return "";
   u_splitpath( path, dir, name, newname );
   if( !access(path.c_str(), 0 ) ) //file exists
     switch( vfQuestion3( par, name.c_str(),
        "This set of files exists!",
             "&Overwrite", "&Rename", "&Cancel") )
            {
            case VF3_2:
                goto AGAIN;
            case VF3_1:
                break;
            case VF3_3:
                return path;
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

   pVisor->Message( par, "GEM2MT node array",
      "Writing to disk a set of node array files from interrupted RMT task.\n"
           "Please, wait...", ii, nIV );
   // Save databr
   CopyWorkNodeFromArray( ii, anNodes, NodT0 );

   sprintf( buf, "%4.4d", ii );
   // dataBR files - binary
   if( binmode )
    {
       newname =  name + + "-dbr-0-"  + buf;
       Path_ = u_makepath( dir, newname, "bin" );
       GemDataStream  f_br1(Path_, ios::out|ios::binary);
       databr_to_file(f_br1);
       f_br1.close();
       fout << ", \"" << newname.c_str() << ".bin\"";
     }

   if( textmode )
      {
        newname = name + "-dbr-0-" + buf;
        Path_ = u_makepath( dir, newname, "dat" );
        fstream  f_br2(Path_.c_str(), ios::out);
        databr_to_text_file(f_br2);
        f_br2.close();
        fout_d << ", \"" << newname.c_str() << ".dat\"";
     }

   if( putNodT1 && NodT1[ii]) // put NodT1[ii] data
   {

       // Save databr
      CopyWorkNodeFromArray( ii, anNodes, NodT1 );

      // dataBR files - binary
      if( binmode )
      {
         newname =  name + + "-dbr-1-"  + buf;
         Path_ = u_makepath( dir, newname, "bin" );
         GemDataStream  f_br1(Path_, ios::out|ios::binary);
         databr_to_file(f_br1);
         f_br1.close();
//         fout << ", \"" << newname.c_str() << ".bin\"";
      }

      if( textmode )
      {
         newname = name + "-dbr-1-" + buf;
         Path_ = u_makepath( dir, newname, "dat" );
         fstream  f_br2(Path_.c_str(), ios::out);
         databr_to_text_file(f_br2);
         f_br2.close();
//         fout_d << ", \"" << newname.c_str() << ".dat\"";
      }
   }
 } // ii
 pVisor->CloseMessage();
 return path;
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
   CNode->NodeTypeHY = normal;
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
    sizeN = anNodes;
    sizeM = sizeK =1;
    CSD = 0;
    CNode = 0;
    NodT0 = 0;  // nodes at current time point
    NodT1 = 0;  // nodes at previous time point
    grid  = 0;   // Array of grid point locations, size is anNodes+1
    tNode = 0;     // Node type codes (see DataBR.h) size anNodes+1
    allocMemory();
}

TNodeArray::TNodeArray( int asizeN, int asizeM, int asizeK, MULTI *apm  ):
sizeN(asizeN), sizeM(asizeM), sizeK(asizeK)
{
  anNodes = asizeN*asizeM*asizeK;
  pmm = apm;
  CSD = 0;
  CNode = 0;
  NodT0 = 0;  // nodes at current time point
  NodT1 = 0;  // nodes at previous time point
  grid  = 0;   // Array of grid point locations, size is anNodes+1
  tNode = 0;     // Node type codes (see DataBR.h) size anNodes+1
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

//#ifdef IPMGEMPLUGIN

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
                   const char* ipmfiles_lst_name, int* nodeTypes, bool getNodT1)
{
  int i;
#ifdef IPMGEMPLUGIN
  fstream f_log("ipmlog.txt", ios::out|ios::app );
  try
    {
#else
      size_t npos = gstring::npos;
#endif
      bool binary_f = true;
      gstring multu_in = MULTI_filename;

// get path
      size_t pos = multu_in.rfind("/");
      gstring Path = "";
      if( pos < npos )
       Path = multu_in.substr(0, pos+1);
      gstring chbr_in = Path + ipmfiles_lst_name;

// Reading structure MULTI (GEM IPM work structure)
      GemDataStream f_m(multu_in, ios::in|ios::binary);
#ifdef IPMGEMPLUGIN
      profil->readMulti(f_m);
#else
      TProfil::pm->readMulti(f_m);
#endif

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
      pos = datachbr_file.find( '-');
      if( pos != /*gstring::*/npos )
      {
         if( datachbr_file[pos+1] == 't' )
            binary_f = false;
         f_getline( f_chbr, datachbr_file, ',');
      }

// Reading dataCH structure from file
     gstring dat_ch = Path + datachbr_file;
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

#ifndef IPMGEMPLUGIN
   pVisor->Message( 0, "GEM2MT node array",
      "Reading from disk a set of node array files to resume an interrupted RMT task.\n"
           "Please, wait...", i, anNodes );
#endif

// Reading work dataBR structure from file
         f_getline( f_chbr, datachbr_file, ',');

         gstring dbr_file = Path + datachbr_file;
         if( binary_f )
         {
             GemDataStream in_br(dbr_file, ios::in|ios::binary);
             databr_from_file(in_br);
          }
         else
          {   fstream in_br(dbr_file.c_str(), ios::in );
		 ErrorIf( !in_br.good() , datachbr_file.c_str(),
                    "DataBR Fileopen error");
               databr_from_text_file(in_br);
          }

// Unpacking work DATABR structure into MULTI (GEM IPM work structure): uses DATACH
//    unpackDataBr();

        if( getNodT1 )
        {
          MoveWorkNodeToArray(i, anNodes, NodT0);
          dbr_file = dbr_file.replace("dbr-0-","dbr-1-");
          if( binary_f )
          {
             GemDataStream in_br(dbr_file, ios::in|ios::binary);
             databr_from_file(in_br);
          }
          else
          {   fstream in_br(dbr_file.c_str(), ios::in );
		 ErrorIf( !in_br.good() , dbr_file.c_str(),
                    "DataBR Fileopen error");
               databr_from_text_file(in_br);
          }
          MoveWorkNodeToArray(i, anNodes, NodT1);
        }
        else
        {
// Copying data from work DATABR structure into the node array
// (as specified in nodeTypes array)
         for( int ii=0; ii<anNodes; ii++)
            if(  (!nodeTypes && i==0) ||
              ( nodeTypes && (nodeTypes[ii] == i+1 )) )
                  {    CNode->NodeHandle = (short)(i+1);
                       MoveWorkNodeToArray(ii, anNodes, NodT0);
                       CopyWorkNodeFromArray(ii, anNodes,NodT0);
                       MoveWorkNodeToArray(ii, anNodes, NodT1);
                       CopyWorkNodeFromArray(ii, anNodes,NodT1);
                   }
        }
          i++;
     }
#ifndef IPMGEMPLUGIN
   pVisor->CloseMessage();
#endif

    ErrorIf( i==0, datachbr_file.c_str(), "NewNodeArray() error: No dataBR files read!" );
    if(nodeTypes)
      for( int ii=0; ii<anNodes; ii++)
      if(   nodeTypes[ii]<=0 || nodeTypes[ii] >= i+1 )
           Error( datachbr_file.c_str(),
              "NewNodeArray() error: Undefined boundary condition!" );

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

//#endif
//#include <stdexcept>
//-------------------------------------------------------------------------
// RunGEM()
// GEM IPM calculation of equilibrium state for the iNode node
// from array NodT1. Mode - mode of GEMS calculation
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

   CopyWorkNodeFromArray( iNode, anNodes, NodT1 );
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
   MoveWorkNodeToArray( iNode, anNodes, NodT1 );
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
//   CNode->NodeTypeHY = normal;
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

//---------------------------------------------------------
// working with grid

// Set grid coordinate array use predefined array aGrid
// or set up regular scale
void TNodeArray::SetGrid( float aSize[3], float (*aGrid)[3] )
{
  int i, j, k, ndx;
  size.x =  aSize[0];
  size.y =  aSize[1];
  size.z =  aSize[2];

  LOCATION delta( size.x/sizeN, size.y/sizeM, size.z/sizeK );
  if( !grid )
      grid = new LOCATION[ anNodes ];

  for( i = 0; i < sizeN; i++ )
    for( j = 0; j < sizeM; j++ )
      for( k = 0; k < sizeK; k++ )
      {
         ndx = iNode( i, j, k );
         if( aGrid )
         {
           grid[ndx].x = aGrid[ndx][0];
           grid[ndx].y = aGrid[ndx][1];
           grid[ndx].z = aGrid[ndx][2];
         }
         else
         {
           grid[ndx].x =delta.x*i;
           grid[ndx].y =delta.y*j;
           grid[ndx].z =delta.z*k;
         }
      }
   // outside limits settted in size
}

// test location in node
bool TNodeArray::isLocationInNode( int ii, int jj, int kk, LOCATION cxyz ) const
{
  LOCATION maxl;

  if( ii<0 || ii>= sizeN || jj<0 ||
      jj >= sizeM || kk <0 || kk >= sizeK )
    return false;

  int ndx = iNode( ii, jj, kk );
  maxl = getGrid( ii+1, jj+1, kk+1 ); // only for rectangular
  // must be changed
  //  x = const, find new y,z srez i
  // analiz pryamougol`nika pri y1 == const, poisk z21 i z22
  // analiz otrezka po z2
  if( grid[ndx].x <= cxyz.x &&  cxyz.x < maxl.x &&
      grid[ndx].y <= cxyz.y &&  cxyz.y <= maxl.y &&
      grid[ndx].z <= cxyz.z &&  cxyz.z <= maxl.z )
           return true;

   return false; // location behind the node
}

// Finds a node absolute index for the current
// point location (uses grid coordinate array grid[])
// performance-important functions to be used e.g. in particle tracking methods
int TNodeArray::FindNodeFromLocation( LOCATION cxyz, int old_node ) const
{
  LOCATION maxl;
  int i, j, k, ndx;

 if( old_node == -1 )
 { // check all nodes
   for( i = 0; i < sizeN; i++ )
    for( j = 0; j < sizeM; j++ )
      for( k = 0; k < sizeK; k++ )
      {
        if(  isLocationInNode( i, j, k, cxyz ) )
         return iNode( i, j, k );
      }
 }
 else // check only nearest nodes
 {
   int i1, j1, k1;
   i1 = indN( old_node );
   j1 = indM( old_node );
   k1 = indK( old_node );

   for( i = i1-1; i <= i1+1; i++ )
     for( j = j1-1; j <= j1+1; j++ )
       for( k = k1-1; k <= k1+1; k++ )
       {
          if(  isLocationInNode( i, j, k, cxyz ) )
              return iNode( i, j, k );
       }
 }
 return -1; // behind region
}

// get current node location
// if iN, jN or kN more then corresponding sizeN, sizeM, sizeK
// return size of system
LOCATION TNodeArray::getGrid( int iN, int jN, int kN ) const
{
  LOCATION loc;
  int i1, j1, k1;

// only for test
  if( iN < 0 || iN > sizeN ||
      jN < 0 || jN > sizeM ||
      kN < 0 || kN > sizeK  )
   Error( "", "getGrid - programm error");

  if( iN == sizeN )  i1 = iN-1;
      else i1 = iN;
  if( jN == sizeM )  j1 = jN-1;
       else j1 = jN;
  if( kN == sizeK )  k1 = kN-1;
        else k1 = kN;

  loc = grid[ iNode( i1, j1, k1)];
  if( i1 != iN ) loc.x = size.x;
  if( j1 != jN ) loc.y = size.y;
  if( k1 != kN ) loc.z = size.z;

  return loc;
}

// get 3D sizes for node (  from cxyz[0] - to cxyz[1] )
// only for rectangular -  must be changed
// for any must be LOCATION cxyz[8]
void TNodeArray::GetNodeSizes( int ndx, LOCATION cxyz[2] )
{
  LOCATION maxl;
  int i, j, k;

  i = indN( ndx );
  j = indM( ndx );
  k = indK( ndx );

  cxyz[0] = grid[ndx];
  cxyz[1] = getGrid( i+1, j+1, k+1 );
  /*
  cxyz[1] = getGrid( i, j, k+1 );
  cxyz[2] = getGrid( i, j+1, k );
  cxyz[3] = getGrid( i, j+1, k+1 );
  cxyz[4] = getGrid( i+1, j, k );
  cxyz[5] = getGrid( i+1, j, k+1 );
  cxyz[6] = getGrid( i+1, j+1, k );
  cxyz[7] = getGrid( i+1, j+1, k+1 );
  */
}

// get full mass of all particles type ptype in node ndx
// ndx    -  (absolute) index of the node
// ptype  -  particle type index ( 1 to 255 )
// tcode  -  particle transport mechanism code (see enum PTCODE)
// ips   - DataBr index of phase or species to which this particle is connected
double TNodeArray::GetNodeMass( int ndx,
       char /*ptype*/, char tcode, unsigned char ips )
{
   double mass = 0.;
   DATABR* dbr = NodT1[ndx];

     switch( tcode )
     {
        case ADVECTIVE:
        case COLLOID: // mass = 0.;
                      // for(short ie=0; ie < CSD->nICb; ie++ )
                      //   mass += dbr->bPS[ips*CSD->nICb+ie]*CSD->ICmm[CSD->xIC[ie]];
                       mass = dbr->mPS[ips];
                        break;
        case DIFFUSIVE:
                      mass = dbr->xDC[ips]*CSD->DCmm[CSD->xDC[ips]];
                        break;
     }
   return mass;
}

// move mass m_v from node ndx_from to node ind_to, one particle move
// ndx_from    -  (absolute) index of the old node
// ndx_to     -  (absolute) index of the new  node
// ptype  -  particle type index ( 1 to 255 )
// tcode  -  particle transport mechanism code (see enum PTCODE)
// ips   - DataBr index of phase or species to which this particle is connected
// m_v -  mass or volume of the particle (depending on ptype and mmode)
void TNodeArray::MoveParticleMass( int ndx_from, int ndx_to,
       char /*type*/, char tcode, unsigned char ips, double m_v )
{
   double mass = 0., coeff, mol;
   DATABR* dbr = NodT1[ndx_from];

   switch( tcode )
   {
    case ADVECTIVE:
    case COLLOID: // mass = 0.;
                  // for(short ie=0; ie < CSD->nICb; ie++ )
                  //   mass += dbr->bPS[ips*CSD->nICb+ie]*CSD->ICmm[CSD->xIC[ie]];
                   mass = dbr->mPS[ips];
                  break;
    case DIFFUSIVE:
                  mass = dbr->xDC[ips]*CSD->DCmm[CSD->xDC[ips]];
                   break;
    }
   coeff = m_v/mass;

   for(short ie=0; ie < CSD->nICb; ie++ )
   {
     mol = 0.;
     switch( tcode )
     {
        case ADVECTIVE:
        case COLLOID:   mol = dbr->bPS[ips*CSD->nICb+ie]*coeff;
                        break;
        case DIFFUSIVE: mol =  dbr->xDC[ips]*CSD->A[CSD->xPH[ips]*CSD->nIC+CSD->xIC[ie]]*coeff;
                        break;
     }
     if( dbr->NodeTypeHY != NBC3source )
       dbr->bIC[ie] -= mol;

      if( ndx_to > 0 && ndx_to < anNodes )
      {
         if( NodT1[ndx_to]->NodeTypeHY != NBC3source )
           NodT1[ndx_to]->bIC[ie] += mol;
      }
      else
         if(dbr->NodeTypeHY != NBC3sink )
           Error( "W002MTRW", "Warning: Particle jumped outside the domain" );

   }
}

//-----------------------End of tnodearray.cpp--------------------------

