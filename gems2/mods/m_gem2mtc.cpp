//-------------------------------------------------------------------
// $$
//
// Implementation of TGEM2MT class, calculation functions
//
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

#include <math.h>
#include <stdio.h>

#include "m_gem2mt.h"
#include "m_syseq.h"
#include "m_param.h"
#include "service.h"
#include "visor.h"
#include "gdatastream.h"
//#include "ms_multi.h"



// reset mt counters
void TGEM2MT::mt_reset()
{
// setup flags and counters
  mtp->gStat = '0';
  mtp->iStat = '0';
  mtp->cT = mtp->Tai[START_];
  mtp->cP = mtp->Pai[START_];
  mtp->cTau = mtp->Tau[START_];
  mtp->ctm = mtp->tmi[START_];
  mtp->cnv = mtp->NVi[START_];
  mtp->qc = 0;
  mtp->kv = 0;
  mtp->jt = 0;
}

// setup begin initalization
void TGEM2MT::init_arrays( bool mode )
{
  vstr tbuf(100);

// set data to SBM (IComp names)
    if( mtp->SBM )
      for(int ii=0; ii< mtp->Nb; ii++ )
        memcpy( mtp->SBM[ii], TProfil::pm->pmp->SB[ii], MAXICNAME+MAXSYMB  );

// setup flags and counters
   mt_reset();

  if( mode )
  {
    int ii;

    mtp->Msysb = 0.;
    mtp->Vsysb = 0.;
    mtp->Mwatb = 1.;
    mtp->Maqb = 1.;
    mtp->Vaqb = 1.;

    for( ii=0; ii<mtp->Nb; ii++)
     mtp->CIclb[ii] = 'g';

    for( ii=0; ii<mtp->nIV; ii++)
    {
     sprintf( tbuf, "Variant%d", ii );
     strncpy( mtp->nam_i[ii], tbuf, MAXIDNAME );
    }

    for( ii=0; ii<mtp->Lbi; ii++)
    {
     mtp->AUcln[ii] = 'M';
     strncpy( mtp->for_i[ii], "H2O", 4 );
    }

    for( ii=0; ii<mtp->nFD; ii++)
    {
     strcpy( mtp->FDLid[ii], "F1" );
     strcpy( mtp->FDLop[ii], "ADD" );
    }

    for( ii=0; ii<mtp->nPG; ii++)
    {
     strcpy( mtp->FDLmp[ii], "P1" );
     strcpy( mtp->MPGid[ii], "P1" );
    }

    if( mtp->UMPG)
      for( ii=0; ii<TProfil::pm->pmp->FI; ii++)
         mtp->UMPG[ii] = 'g';

// setup default graphiks lines
   if( mtp->PvEF != S_OFF  )
        for(int i=0; i<mtp->nYE; i++ )
        {
            sprintf( tbuf, "%s%d", TProfil::pm->pa.GDpsc, i+1 );
            if( !*mtp->lNamE[i] || *mtp->lNamE[i] == ' ' )
                strncpy( mtp->lNamE[i], tbuf,  MAXGRNAME );
        }
    if( mtp->PvMSg != S_OFF  )
         for(int j=0; j< mtp->nYS; j++ )
         {
            sprintf( tbuf, "%s%d", TProfil::pm->pa.GDpsc, j+1 );
            if( !*mtp->lNam[j]|| *mtp->lNam[j] == ' ' )
               strncpy( mtp->lNam[j], tbuf, MAXGRNAME );
          }
  }
}

// recalc working parametres
void TGEM2MT::mt_next()
{
     mtp->cT += mtp->Tai[STEP_];
     mtp->cP += mtp->Pai[STEP_];
     mtp->cTau += mtp->Tau[STEP_];
//     mtp->cV += 0;
     mtp->ctm += mtp->tmi[STEP_];
     mtp->cnv += mtp->NVi[STEP_];
}

// make EqStat key  && calculate records
void TGEM2MT::calc_eqstat()
{
    vstr buf(40);

    sprintf(buf, "%.4d", mtp->ctm);
    memset(mtp->timep, 0, 5 );
    strncpy(mtp->timep, buf, 4 );
    sprintf(buf, "%.3d", mtp->cnv);
    memset( mtp->NVp, 0, 4 );
    strncpy( mtp->NVp, buf, 3 );
    gcvt( mtp->cT, 6, mtp->TCp );
    gcvt( mtp->cP, 6, mtp->Pp );
    gcvt( mtp->cV, 6, mtp->Bnamep );

   rt[RT_SYSEQ].MakeKey( RT_GEM2MT,  mtp->sykey, RT_GEM2MT, 0, RT_GEM2MT,1,
         RT_GEM2MT, 2, K_IMM, mtp->timep, K_IMM, mtp->Bnamep,
         K_IMM, mtp->Pp, K_IMM, mtp->TCp, K_IMM, mtp->NVp, K_END );

   rt[RT_SYSEQ].Find(mtp->sykey);
// calc current SyStat
   TProfil::pm->CalcEqstat( false );
   if( mtp->PsSYd == S_ON )
     TSysEq::pm->CmSave();           // save results to DB
   if( mtp->stld )
       memcpy( mtp->stld+mtp->kv, mtp->sykey, EQ_RKLEN );
}


// Generating new SysEq
void TGEM2MT::gen_task()
{
 // put Bb data to system  ( into B and bi_)
    for(int i=0; i<mtp->Nb; i++)
    {
       TProfil::pm->syp->B[i] =  mtp->Bn[ mtp->kv*mtp->Nb+i ];
       TProfil::pm->syp->BI[i] = mtp->Bn[ mtp->kv*mtp->Nb+i ];
     }
 // calculate EqStat record (Thermodynamic&Equlibria)
   //  TProfil::pm->pmp->pTPD = 0;
     calc_eqstat();
}


void TGEM2MT::outMulti()
{

  TMulti* mult = TProfil::pm->multi;
  TCStringArray aList;
  fstream fout;
  fstream fout_d;
  gstring Path_;
  gstring dir;
  gstring name;
  gstring newname;
  gstring path;

if( mtp->PsSdat != S_OFF || mtp->PsSbin != S_OFF )
{
// added for dataCH and DataBr structures
// select lists
    aList.Clear();
    for(int ii=0; ii< mult->GetPM()->N; ii++ )
       aList.Add( gstring( mult->GetPM()->SB[ii], 0, MAXICNAME+MAXSYMB));
    TCIntArray aSelIC = vfMultiChoice(window(), aList,
         "Please, mark independent components for selection into DataBridge");

    aList.Clear();
    for(int ii=0; ii< mult->GetPM()->L; ii++ )
       aList.Add( gstring( mult->GetPM()->SM[ii], 0, MAXDCNAME));
    TCIntArray aSelDC = vfMultiChoice(window(), aList,
         "Please, mark dependent components for selection into DataBridge");

    aList.Clear();
    for(int ii=0; ii< mult->GetPM()->FI; ii++ )
       aList.Add( gstring( mult->GetPM()->SF[ii], 0, MAXPHNAME+MAXSYMB));
    TCIntArray aSelPH = vfMultiChoice(window(), aList,
         "Please, mark phases for selection into DataBridge");


// Get name of filenames structure
   path = gstring( rt[RT_SYSEQ].FldKey(2), 0, rt[RT_SYSEQ].FldLen(2));;
   path.strip();
   path += ".ipm";
      // open file to output
   if( vfChooseFileSave(window(), path,
          "Please, enter IPM work structure file name", "*.ipm" ) == false )
               return;

   u_splitpath( path, dir, name, newname );

//  putting MULTI to binary file
    GemDataStream  ff(path, ios::out|ios::binary);
    ff.writeArray( &TProfil::pm->pa.p.PC, 10 );
    ff.writeArray( &TProfil::pm->pa.p.DG, 28 );
    mult->to_file( ff, path );

// set default data and realloc arrays
   mult->makeStartDataChBR( aSelIC, aSelDC, aSelPH );

// out dataCH to binary file
   if( mtp->PsSbin != S_OFF )
   {  Path_ = u_makepath( dir, name, "dch" );
//   filename = "GEMSystem.dch";
//   if( vfChooseFileSave(window(), filename,
//          "Please, enter DataCH binary file name", "*.dch" )  )
//   {
       GemDataStream  f_ch1(Path_, ios::out|ios::binary);
       mult->datach_to_file(f_ch1);
       f_ch1.close();
//   }
   }

// out dataCH to text file
   if( mtp->PsSdat != S_OFF )
   {  newname = name+"-dch";
      Path_ = u_makepath( dir, newname, "dat" );
//   filename = "GEMSystem.dat";
//   if( vfChooseFileSave(window(), filename,
//          "Please, enter DataCH text file name", "*.dat" )  )
//   {
      fstream  f_ch2(Path_.c_str(), ios::out);
      mult->datach_to_text_file(f_ch2);
      f_ch2.close();
//   }
   }

// making special files
// put data to IPMRUN.BAT file
   Path_ = u_makepath( dir, "IPMRUN", "BAT" );
   fout.open(Path_.c_str(), ios::out);
   fout << "echo off\n";
   fout << "rem Normal runs\n";
   if( mtp->PsSdat != S_OFF )
      fout << "gemipm2k.exe " << name.c_str() <<
            ".ipm ipmfiles-dat.lst\n";
   if( mtp->PsSbin != S_OFF )
       fout << "rem gemipm2k.exe " << name.c_str() <<
             ".ipm ipmfiles-bin.lst\n";
   fout.close();

// put data to pmfiles-bin.lst file
   if( mtp->PsSbin != S_OFF )
   {   Path_ = u_makepath( dir, "ipmfiles-bin", "lst" );
       fout.open(Path_.c_str(), ios::out);
       fout << "-b \"" << name.c_str() << ".dch\" \"";
   }
// put data to pmfiles-dat.lst file
   if( mtp->PsSdat != S_OFF )
   {   Path_ = u_makepath( dir, "ipmfiles-dat", "lst" );
       fout_d.open(Path_.c_str(), ios::out);
       fout_d << "-t \"" << name.c_str() << "-dch.dat\" \"";
   }
 }  // end if

 for( mtp->kv = 0; mtp->kv < mtp->nIV; mtp->kv++ )
 {
   pVisor->Message( window(), GetName(),
      "Generation of EqStat records\n"
           "Please, wait...", mtp->kv, mtp->nIV);

  // Make new Systat record & calculate it
     gen_task();

  // Save databr
    if( mtp->PsSdat != S_OFF || mtp->PsSbin != S_OFF )
       mult->packDataBr();
   // dataBR files - binary
    if( mtp->PsSbin != S_OFF )
    {
       newname =  name + "-" + mtp->timep;
       Path_ = u_makepath( dir, newname, "dbr" );
//   filename = "GEMNode.dbr";
//   if( vfChooseFileSave(window(), filename,
//          "Please, enter DataBR binary file name", "*.dbr" )  )
//   {
        GemDataStream  f_br1(Path_, ios::out|ios::binary);
        mult->databr_to_file(f_br1);
        f_br1.close();
//   }
        fout << ", " << newname.c_str() << ".dbr\"";
     }

      if( mtp->PsSdat != S_OFF )
      {
        newname = name + "-dbr-" + mtp->timep;
        Path_ = u_makepath( dir, newname, "dat" );
//   filename = "GEMNode.dat";
//   if( vfChooseFileSave(window(), filename,
//          "Please, enter DataBR text file name", "*.dat" )  )
//   {
        fstream  f_br2(Path_.c_str(), ios::out);
        mult->databr_to_text_file(f_br2);
        f_br2.close();
//   }
        fout_d << ", " << newname.c_str() << ".dat\"";
     }

     mt_next();      // Generate work values for the next EqStat rkey

 } // mtp->kv

  pVisor->CloseMessage();

  if( mtp->PsSdat != S_OFF || mtp->PsSbin != S_OFF )
  {   mult->datach_free();
      mult->databr_free();
  }
}



// --------------------- end of m_gem2mtc.cpp ---------------------------

