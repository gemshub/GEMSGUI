//-------------------------------------------------------------------
// $ Id:$
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
#include "s_formula.h"
#include "m_icomp.h"
#include "m_compos.h"




// reset mt counters
void TGEM2MT::mt_reset()
{
// setup flags and counters
  mtp->gStat = '0';
  mtp->iStat = '0';
//  mtp->cT = mtp->Tai[START_];
//  mtp->cP = mtp->Pai[START_];
  mtp->cV = 0.;
  mtp->cTau = mtp->Tau[START_];
  mtp->ctm = mtp->tmi[START_];
  mtp->cnv = mtp->NVi[START_];
  mtp->qc = 0;
  mtp->kv = 0;
  mtp->jt = 0;
  mtp->cT = mtp->Ti[START_];
  mtp->cP = mtp->Pi[START_];
}

// setup begin initalization
void TGEM2MT::init_arrays( bool mode )
{
  vstr tbuf(100);

// set data to SBM (IComp names)
    if( mtp->SBM )
      for(int ii=0; ii< mtp->Nb; ii++ )
        memcpy( mtp->SBM[ii], TProfil::pm->mup->SB[ii], MAXICNAME/*+MAXSYMB*/  );

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
     mtp->CIclb[ii] = QUAN_MOL;

    for( ii=0; ii<mtp->nIV; ii++)
    {
     sprintf( tbuf, "Variant%d", ii );
     strncpy( mtp->nam_i[ii], tbuf, MAXIDNAME );
     mtp->Pi[ii] = 1.;
     mtp->Ti[ii] = 25.;
    }

    for( ii=0; ii<mtp->Lbi; ii++)
    {
     mtp->AUcln[ii] = QUAN_MOL;
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
         mtp->UMPG[ii] = QUAN_GRAM;

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

// generate Tval and Pval arrays
void TGEM2MT::gen_TPval()
{
  int ii;
  mtp->cT = mtp->Tai[START_];
  mtp->cP = mtp->Pai[START_];

  for( ii=0; ii<mtp->nTai; ii++ )
  {
    mtp->Tval[ii] = mtp->cT;
    mtp->cT += mtp->Tai[STEP_];
  }

  for( ii=0; ii<mtp->nPai; ii++ )
  {
    mtp->Pval[ii] = mtp->cP;
    mtp->cP += mtp->Pai[STEP_];
  }
}


// recalc working parametres
void TGEM2MT::mt_next()
{
//     mtp->cT += mtp->Tai[STEP_];
//     mtp->cP += mtp->Pai[STEP_];
     mtp->cTau += mtp->Tau[STEP_];
//     mtp->cV += 0;
     mtp->ctm += mtp->tmi[STEP_];
     mtp->cnv += mtp->NVi[STEP_];
}

// make EqStat key  && calculate records
void TGEM2MT::calc_eqstat()
{
    vstr buf(40);

    mtp->cT = mtp->Ti[mtp->kv];
    mtp->cP = mtp->Pi[mtp->kv];

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

//make matrix An  As Bb_Calc in Dualth
void TGEM2MT::make_A( int siz_, char (*for_)[MAXFORMUNITDT] )
{
  // Get full matrix A
  TIArray<TFormula> aFo;
  gstring form;
  int ii;

  if( !siz_ )
  { mtp->An = (float *)aObj[ o_mtan ].Free();
    return;
  }
  for( ii=0; ii<siz_; ii++ )
  {
     aFo.Add( new TFormula() );
     form = gstring( for_[ii], 0, MAXFORMUNITDT );
     form.strip();
     aFo[ii].SetFormula( form.c_str() ); // and ce_fscan
  }

  ErrorIf( mtp->Nb != TProfil::pm->mup->N, GetName(),
               "Illegal data in mtp->Nb ");

  mtp->An = (float *)aObj[ o_mtan ].Alloc( siz_, mtp->Nb, F_ );
  memset(mtp->An, 0, sizeof(float)*(siz_*mtp->Nb) );
  for( ii=0; ii<siz_; ii++ )
     aFo[ii].Stm_line( TProfil::pm->mup->N, mtp->An+ii*TProfil::pm->mup->N,
           (char *)TProfil::pm->mup->SB, TProfil::pm->mup->Val );
  aFo.Clear();
}

// Calculate data for matrix Bn As Bb_Calc in Dualth
void
TGEM2MT::Bn_Calc()
{
    int i, j;
    double Msysb_bk, Tmolb_bk;
    double MsysC = 0., R1C = 0.;
    double Xincr, ICmw, DCmw;
    vstr  pkey(MAXRKEYLEN+10);
    float  *ICw;  //IC atomic (molar) masses [0:Nmax-1]
    float *A;
    time_t crt;

    if( mtp->PvICi == S_OFF  &&   mtp->PvAUi == S_OFF )
       return;

// get data fron IComp
    TIComp* aIC=(TIComp *)(&aMod[RT_ICOMP]);
    aIC->ods_link(0);
    ICw = new float[mtp->Nb];
    memset( pkey, 0, MAXRKEYLEN+9 );
    for( i=0; i<mtp->Nb; i++ )
    {
        // load molar mass
        memcpy( pkey, TProfil::pm->mup->SB[i], IC_RKLEN );
        pkey[IC_RKLEN] = 0;
        aIC->TryRecInp( pkey, crt, 0 );
        // atomic mass and valence
        if( IsFloatEmpty( aIC->icp->awt ))
            ICw[i] = 0;
        else ICw[i] = aIC->icp->awt;
        // icp->val;
    }

// make An from dtp->for_b
       make_A( mtp->Lbi, mtp->for_i );

  Msysb_bk = mtp->Msysb;
  Tmolb_bk = mtp->Tmolb;

  for( int ii=0; ii< mtp->nIV; ii++ )
  {
    // set line in Bb to zeros
    memset( mtp->Bn + ii*mtp->Nb, 0, mtp->Nb*sizeof(double) );
    mtp->Msysb = Msysb_bk;
    mtp->Tmolb = Tmolb_bk;


    if( mtp->PvICi != S_OFF )
    { //  Through IC
        for( i=0; i<mtp->Nb; i++ )
        {
          if( !mtp->CIb[ii*mtp->Nb + i] ||
                 IsFloatEmpty( mtp->CIb[ ii*mtp->Nb + i ] ))
                continue;

          ICmw = ICw[i];
          Xincr = TCompos::pm->Reduce_Conc( mtp->CIclb[i],
             mtp->CIb[ii*mtp->Nb+i],
             ICmw, 1.0, mtp->Tmolb, mtp->Msysb, mtp->Mwatb,
             mtp->Vaqb, mtp->Maqb, mtp->Vsysb );

           mtp->Bn[ii*mtp->Nb+i] += Xincr;
           MsysC += Xincr*ICmw;
           R1C += Xincr;
        } //  i
    }

    if( mtp->PvAUi != S_OFF )
    { // formul list

      for( j=0; j < mtp->Lbi; j++ )
      {
         A = mtp->An + j * mtp->Nb;
         if( !mtp->CAb[ii*mtp->Lbi + j] ||
            IsFloatEmpty( mtp->CAb[ii*mtp->Lbi + j] ))
                    continue;
         DCmw = 0.;
         for( i=0; i<mtp->Nb; i++ )
         // calculation of molar mass
             DCmw += A[i]* ICw[i];
         Xincr = TCompos::pm->Reduce_Conc( mtp->AUcln[j],
                mtp->CAb[ii*mtp->Lbi + j],
                 DCmw, 1.0, mtp->Tmolb, mtp->Msysb, mtp->Mwatb,
                 mtp->Vaqb, mtp->Maqb, mtp->Vsysb );
         // recalc stoichiometry
         for( i=0; i<mtp->Nb; i++ )
          if( A[i] )
          {
            mtp->Bn[ii*mtp->Nb+i] += Xincr*A[i]; // calc control sum
            MsysC += Xincr * A[i] * ICw[i];
            R1C += Xincr * A[i];
          }
        } //  j

      }
      MsysC /= 1e3;

      // Analyze control sum
      if( fabs( mtp->Tmolb ) < 1e-12 )
        mtp->Tmolb = R1C;
      if( fabs( mtp->Tmolb - R1C ) < 1e-12 || fabs( R1C ) < 1e-15 )
        /*Xincr = 1.*/;
       else
       { // normalisation
          Xincr = mtp->Tmolb / R1C;
          MsysC = 0.0;
          for( i=0; i<mtp->Nb; i++ )
            if( fabs( mtp->Bn[ii*mtp->Nb+i] ) >= 1e-12 )
            {
                mtp->Bn[ii*mtp->Nb+i] *= Xincr;
                MsysC += mtp->Bn[ii*mtp->Nb+i] * ICw[i];
            }
          mtp->Tmolb = R1C;
          MsysC /= 1e3;
       }

       if( fabs( mtp->Msysb ) < 1e-12 )
           mtp->Msysb = MsysC;
       if( fabs( mtp->Msysb - MsysC ) < 1e-12 || fabs( MsysC ) < 1e-15 )
        /*Xincr = 1.*/;
      else
      { // normalisation
         Xincr = mtp->Msysb / MsysC;
         R1C = 0.0;
         for( i=0; i<mtp->Nb; i++ )
             if( fabs( mtp->Bn[ii*mtp->Nb+i] ) >= 1e-12 )
             {
                mtp->Bn[ii*mtp->Nb+i] *= Xincr;
                R1C += mtp->Bn[ii*mtp->Nb+i];
             }
         mtp->Tmolb = R1C;
      }
  } // ii

  mtp->Msysb = Msysb_bk;
  mtp->Tmolb = Tmolb_bk;
  delete[]  ICw;
}


// Generating new SysEq
void TGEM2MT::gen_task()
{
    int i;
 // put Bb data to system  ( into B and bi_)
    for( i=0; i<mtp->Nb; i++)
    {
       TProfil::pm->syp->B[i] =  mtp->Bn[ mtp->kv*mtp->Nb+i ];
       TProfil::pm->syp->BI[i] =  mtp->Bn[ mtp->kv*mtp->Nb+i ];
       TProfil::pm->syp->BIun[i] =  QUAN_MOL;
    }
// set zeros to xd_ and xa_
    for( i=0; i < TProfil::pm->mup->L; i++)
       TProfil::pm->syp->XeD[i] = 0.;
    for( i=0; i < TProfil::pm->mup->La; i++)
       TProfil::pm->syp->XeA[i] = 0.;


 // calculate EqStat record (Thermodynamic&Equlibria)
   //  TProfil::pm->pmp->pTPD = 0;
     calc_eqstat();

//calculate graphics
   if( mtp->PvMSg )
   {
      mtp->jt = min( mtp->kv, (short)(mtp->nS-1));
      rpn[1].CalcEquat();
   }
   aMod[RT_GEM2MT].ModUpdate("GEM2MT data sampling in progress...");

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

// generate Tval&Pval arrays
if( mtp->PsTPai != S_OFF )
    gen_TPval();

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
   mult->makeStartDataChBR( aSelIC, aSelDC, aSelPH,
      mtp->nTai,  mtp->nPai, mtp->Tai[3], mtp->Pai[3], mtp->Tval, mtp->Pval );

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

