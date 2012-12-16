//-------------------------------------------------------------------
// $Id: m_gem2mtc.cpp 1379 2009-07-29 14:53:22Z gems $
//
// Implementation of TGEM2MT class, calculation functions
//
// Copyright (C) 2005 S.Dmytriyeva, D.Kulik
//
// This file is part of a GEM-Selektor library for thermodynamic
// modelling by Gibbs energy minimization
// Uses: GEM-Selektor GUI GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the terms of GEMS3 Development
// Quality Assurance Licence (GEMS3.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//

#include <cmath>
#include <cstdio>
#include "m_gem2mt.h"
#include "m_syseq.h"
#include "visor.h"
#include "s_formula.h"
#include "m_icomp.h"
#include "m_compos.h"

bool
TGEM2MT::test_sizes( )
{
  gstring err_str;


  mtp->Nb = TRMults::sm->GetMU()->N;
  mtp->FIb = TRMults::sm->GetMU()->Fi;
  mtp->Lb = TRMults::sm->GetMU()->L;
  mtp->Lsf = TRMults::sm->GetMU()->Ls;


  if( mtp->nC<=0 || mtp->nIV <= 0)
  {  err_str +=
"W02PErem: You forgot to specify the number of inital variants! \n Please, do it!";
    mtp->nC = mtp->nIV = 1;
  }

  if( mtp->nC < mtp->nIV )
        mtp->nC = mtp->nIV;

  if( mtp->PvAUi != S_OFF )
    if( mtp->Lbi <= 0 )
    {  err_str +=
"W02PErem: You forgot to specify the number of formula units! \n Please, do it!";
       mtp->Lbi = 1;
     }

  /*
  if( mtp->PvFDL != S_OFF )
    if( mtp->nFD <= 0 )
    {  err_str +=
"W02PErem: You forgot to specify the number of flux definitions! \n Please, do it!";
       mtp->nFD = 1;
     }

  if( mtp->PvSFL != S_OFF )
    if( mtp->nSFD <= 0 )
    {  err_str +=
"W02PErem: You forgot to specify the source fluxes\n"
" and elemental stoichiometries for them! \n Please, do it!";
       mtp->nSFD = 1;
     }

  if( mtp->PvPGD != S_OFF )
    if( mtp->nPG <= 0 )
    {  err_str +=
"W02PErem: You forgot to specify the number of mobile phase groups! \n Please, do it!";
       mtp->nPG = 1;
     }

  */
  if( mtp->PvMSg != S_OFF )
    if( mtp->nC <= 0 || mtp->nYS <= 0 )
    {  err_str +=
"W02PErem: You forgot to specify the number of nodes or plots! \n Please, do it!";
      if( mtp->nC <= 0 ) mtp->nC = 1;
      if( mtp->nYS <= 0 ) mtp->nYS = 1;
     }
 /*
  if( mtp->PvEF != S_OFF )
    if( mtp->nE <= 0 || mtp->nYE <= 0 )
    {  err_str +=
"W02PErem: You forgot to specify the number of experimental points! \n Please, do it!";
      if( mtp->nE <= 0 ) mtp->nE = 1;
      if( mtp->nYE <= 0 ) mtp->nYE = 1;
     }
*/

 if( mtp->PsMode == RMT_MODE_W  )
  if( mtp->nPTypes <= 0 )
  {  err_str +=
"W02PErem: You forgot to specify the number of particles types ! \n Please, do it!";
      mtp->nPTypes = 10;
   }

   if( !err_str.empty() )
    {
        vfMessage(window(), GetName(), err_str.c_str(), vfErr);
        return false;
    }
    return true;
}

// Make start IComp, Dcomp, PHase selections to DATACH
void TGEM2MT::SelectNodeStructures( bool select_all )
{
  MULTI *mult = TMulti::sm->GetPM();
  TCStringArray aList;
  TCIntArray aSelIC;
  TCIntArray aSelDC;
  TCIntArray aSelPH;
  long int ii;
  uint jj;

  if( !select_all ) // use old selections
  {
     for( ii=0; ii<mtp->nICb; ii++)
       aSelIC.Add( mtp->xIC[ii] );
     for( ii=0; ii<mtp->nDCb; ii++)
       aSelDC.Add( mtp->xDC[ii] );
     for( ii=0; ii<mtp->nPHb; ii++)
       aSelPH.Add( mtp->xPH[ii] );
  }

// select lists
    aList.Clear();
    for( ii=0; ii< mult->N; ii++ )
    {  if( select_all )
         aSelIC.Add( ii );
       else
         aList.Add( gstring( mult->SB[ii], 0, MAXICNAME+MAXSYMB));
    }
    if( !select_all  )
      aSelIC = vfMultiChoiceSet(window(), aList,
          "Please, mark independent components for selection into DataBridge",
          aSelIC);

    aList.Clear();
    for( ii=0; ii< mult->L; ii++ )
    {  if( select_all )
         aSelDC.Add( ii );
       else
       aList.Add( gstring( mult->SM[ii], 0, MAXDCNAME));
    }
    if( !select_all  )
       aSelDC = vfMultiChoiceSet(window(), aList,
         "Please, mark dependent components for selection into DataBridge",
         aSelDC );

    aList.Clear();
    for( ii=0; ii< mult->FI; ii++ )
    {  if( select_all )
         aSelPH.Add( ii );
       else
       aList.Add( gstring( mult->SF[ii], 0, MAXPHNAME+MAXSYMB));
    }
    if( !select_all  )
       aSelPH = vfMultiChoiceSet(window(), aList,
         "Please, mark phases for selection into DataBridge",
         aSelPH);

// These dimensionalities define sizes of dynamic data in DATABR structure
  mtp->nICb = aSelIC.GetCount();
  mtp->nDCb = aSelDC.GetCount();
  mtp->nPHb = aSelPH.GetCount();
  //int nPSb = 0;
  //for( jj=0; jj< aSelPH.GetCount(); jj++, nPSb++ )
  // if( aSelPH[jj] >= mult->FIs )
  //     break;
  mtp->Nf =  mtp->nICb;
  mtp->FIf = mtp->nPHb;

  
// realloc memory
  dyn_new();

// set dynamic data
  for( jj=0; jj< aSelIC.GetCount(); jj++ )
    mtp->xIC[jj] = aSelIC[jj];
  for( jj=0; jj< aSelDC.GetCount(); jj++ )
    mtp->xDC[jj] = aSelDC[jj];
  for( jj=0; jj< aSelPH.GetCount(); jj++ )
    mtp->xPH[jj] = aSelPH[jj];
}


// setup begin initalization
void TGEM2MT::init_arrays( bool mode )
{
  vstr tbuf(100);

// set data to SBM (IComp names)
    if( mtp->SBM )
      for(long int ii=0; ii< mtp->Nb; ii++ )
        memcpy( mtp->SBM[ii], TRMults::sm->GetMU()->SB[ii], MAXICNAME/*+MAXSYMB*/  );

// setup flags and counters
  mtp->gStat = GS_INDEF;
  mtp->iStat = GS_INDEF;
  mt_reset();

  if( mode )
  {
    long int ii;
    double cT =  TMulti::sm->GetPM()->TCc;
    double cP =  TMulti::sm->GetPM()->Pc;


    mtp->Msysb = 0.;
    mtp->Vsysb = 0.;
    mtp->Mwatb = 1.;
    mtp->Maqb = 1.;
    mtp->Vaqb = 1.;

    if( mtp->PvICi != S_OFF )
      for( ii=0; ii<mtp->Nb; ii++)
          mtp->CIclb[ii] = QUAN_MOL;

    for( ii=0; ii<mtp->nIV; ii++)
    {
     sprintf( tbuf, "System%ld", ii );
     strncpy( mtp->nam_i[ii], tbuf, MAXIDNAME );
     mtp->PTVm[ii][0] = cP;
     mtp->PTVm[ii][1] = cT;
     mtp->PTVm[ii][2] = 0.;
     mtp->PTVm[ii][3] = 1.;
    }

    for( ii=0; ii<mtp->nC; ii++)
    {
     mtp->StaP[ii][0] = cP;
     mtp->StaP[ii][1] = cT;
     mtp->StaP[ii][2] = 0.;  // initial node volumes
     mtp->StaP[ii][3] = 1.;  // initial node masses
    }

    for( ii=0; ii<mtp->Lbi; ii++)
    {
     mtp->AUcln[ii] = QUAN_GRAM;
     memcpy( mtp->for_i[ii], "H2O", 4 );
    }

// set up defaults for particles
    if( mtp->PsMode == RMT_MODE_W  )
     for( ii=0; ii< mtp->nPTypes; ii++ )
     {
        mtp->NPmean[ii] = 500;
        mtp->nPmin[ii] = 100;
        mtp->nPmax[ii] = 1000;
        mtp->ParTD[ii][0] = ii;
        mtp->ParTD[ii][1] = MOBILE_C_MASS;
        mtp->ParTD[ii][2] = DISSOLVED;
        mtp->ParTD[ii][3] = 0;
        mtp->ParTD[ii][4] = 0;
        mtp->ParTD[ii][5] = 0;
     }


 // setup default graphiks lines
   if( mtp->PvEF != S_OFF  )
        for(long int i=0; i<mtp->nYE; i++ )
        {
            sprintf( tbuf, "%s%ld", TProfil::pm->pa.GDpsc, i+1 );
            if( !*mtp->lNamE[i] || *mtp->lNamE[i] == ' ' )
                strncpy( mtp->lNamE[i], tbuf,  MAXGRNAME );
        }
   /* remake script setup
      if( mtp->PvMSg != S_OFF  )
         for(long int j=0; j< mtp->nYS; j++ )
         {
            sprintf( tbuf, "%s%d", TProfil::pm->pa.GDpsc, j+1 );
            if( !*mtp->lNam[j]|| *mtp->lNam[j] == ' ' )
               strncpy( mtp->lNam[j], tbuf, MAXGRNAME );
          }
    */
  }

  if( mtp->PsMode == RMT_MODE_S || mtp->PsMode == RMT_MODE_F || mtp->PsMode == RMT_MODE_B )
  {
     long int ii;
     gstring phName = "0";
     double xaq= 0.;
     double xgas = 0.;
     double xsld = 0.;

     phName = "Pg1";
     switch( mtp->PsMPh )
     {
      case MGP_TT_AQGF: phName = "flu"; xgas = 1.; xaq = 1.;  // '3'
         break;
      case MGP_TT_AQS: phName = "aq"; xaq = 1.;  // '1'
         break;
      case MGP_TT_GASF: phName = "gas"; xgas = 1.;  // '2'
         break;
      case MGP_TT_SOLID: phName = "sld"; xsld = 1.; // '4'
         break;
      default: break;
     }
     if( mtp->nPG > 0 && !(!*mtp->MGPid[0] || *mtp->MGPid[0] == ' '))
       {  phName = gstring(mtp->MGPid[0], 0, MAXSYMB);
            phName.strip();
       }

     if( mode )  // only start
     for( ii=0; ii<mtp->nPG; ii++)
     {
        if( !*mtp->MGPid[ii] || *mtp->MGPid[ii] == ' ' || *mtp->MGPid[ii] == '`' )
            strcpy( mtp->MGPid[ii], phName.c_str() );
        for(long int k=0; k<mtp->FIf; k++ )
        {
             char  PHC_ = TMulti::sm->GetPM()->PHC[mtp->xPH[k]];

             if( PHC_ == PH_AQUEL )
                  mtp->PGT[ii*mtp->FIf+k] = xaq;
               else
                  if( PHC_ == PH_GASMIX || PHC_ == PH_FLUID || PHC_ == PH_PLASMA )
                   mtp->PGT[ii*mtp->FIf+k] = xgas;
                  else
                   mtp->PGT[ii*mtp->FIf+k] = xsld;
         }
     }

     if( mtp->UMGP)
       for( ii=0; ii<mtp->FIf; ii++)
         if( !mtp->UMGP[ii] || mtp->UMGP[ii] == ' '|| mtp->UMGP[ii] == '`' )
             mtp->UMGP[ii] = QUAN_MOL;


     if( mtp->PvFDL != S_OFF )
       for( ii=0; ii<mtp->nFD; ii++)
       {
         if( !*mtp->FDLmp[ii] || *mtp->FDLmp[ii] == ' ' || *mtp->FDLmp[ii] == '`' )
            strcpy( mtp->FDLmp[ii], phName.c_str() );
         if( !*mtp->FDLid[ii] || *mtp->FDLid[ii] == ' ' )
             strcpy( mtp->FDLid[ii], "qj" );
       }
    }
  }


// generate Tval and Pval arrays
void TGEM2MT::gen_TPval()
{
  long int ii;
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
//     mtp->cV += 0;
     mtp->ctm += mtp->tmi[STEP_];
     mtp->cnv += mtp->NVi[STEP_];
}

// make EqStat key  && calculate records
void TGEM2MT::calc_eqstat( bool startSys )
{
    vstr buf(40);

    if( startSys )
    {  mtp->cT = mtp->PTVm[mtp->kv][1];
       mtp->cP = mtp->PTVm[mtp->kv][0];
    }
    else
    {
        if( fabs( mtp->StaP[mtp->qc][1] - mtp->cT ) > 1.e-10 ||
               fabs( mtp->StaP[mtp->qc][0] - mtp->cP ) > 1.e-10 )
        { // load new MTPARM on T or P
            mtp->cT = mtp->StaP[mtp->qc][1];
            mtp->cP = mtp->StaP[mtp->qc][0];
        }
//        else
//            return; // system not changed  !!!! this is doubtful... DK 4.12.2011
    }

    sprintf(buf, "%.4ld", mtp->ctm);
    memset(mtp->timep, 0, 5 );
    strncpy(mtp->timep, buf, 4 );
    sprintf(buf, "%.3ld", mtp->cnv);
    memset( mtp->NVp, 0, 4 );
    strncpy( mtp->NVp, buf, 3 );
    Gcvt( mtp->cT, 6, mtp->TCp );
    Gcvt( mtp->cP, 6, mtp->Pp );
    Gcvt( mtp->cV, 6, mtp->Bnamep );

    rt[RT_SYSEQ].MakeKey( RT_GEM2MT,  mtp->sykey, RT_GEM2MT, 0, RT_GEM2MT,1,
         RT_GEM2MT, 2, K_IMM, mtp->timep, K_IMM, mtp->Bnamep,
         K_IMM, mtp->Pp, K_IMM, mtp->TCp, K_IMM, mtp->NVp, K_END );
//    rt[RT_SYSEQ].Find(mtp->sykey); // do we need this in RMT_MODE_S ?

    if( mtp->PsMode == RMT_MODE_S ) // S mode - avoid calculating equilibrium, just set b vector
    {
           TMulti::sm->GetPM()->TCc = mtp->cT;
           //TK =  pmp->TC+C_to_K;
           TMulti::sm->GetPM()->Pc= mtp->cP;
           // loading parameters for ICs (independent components)
           for( long int i=-1, ii=0; ii< mtp->Nb; ii++ )
           {
              if( TSyst::sm->GetSY()->Icl[ii] == S_OFF )
                continue;
              i++;
              TMulti::sm->GetPM()->B[i] = TSyst::sm->GetSY()->B[ii];
           }
    }
    else {
           rt[RT_SYSEQ].Find(mtp->sykey);
           // calculate current EqStat
           TProfil::pm->CalcEqstat( false );
    }

   if( startSys )
   {
      if( mtp->PsSYd == S_ON && mtp->PsMode != RMT_MODE_S)
        TSysEq::pm->CmSave();           // save results to DB

     if( mtp->stld )
       memcpy( mtp->stld+mtp->kv, mtp->sykey, EQ_RKLEN );
   }

   aMod[RT_GEM2MT].ModUpdate("GEM2MT data sampling in progress...");
}

//make matrix An  As Bb_Calc in Dualth
void TGEM2MT::make_A( long int siz_, char (*for_)[MAXFORMUNITDT] )
{
  // Get full matrix A
  TIArray<TFormula> aFo;
  gstring form;
  long int ii;

  if( !siz_ )
  { mtp->An = (double *)aObj[ o_mtan ].Free();
    return;
  }
  for( ii=0; ii<siz_; ii++ )
  {
     aFo.Add( new TFormula() );
     form = gstring( for_[ii], 0, MAXFORMUNITDT );
     form.strip();
     aFo[ii].SetFormula( form.c_str() ); // and ce_fscan
  }

  ErrorIf( mtp->Nb != TRMults::sm->GetMU()->N, GetName(),
               "Invalid data in mtp->Nb ");

  mtp->An = (double *)aObj[ o_mtan ].Alloc( siz_, mtp->Nb, D_ );
  fillValue(mtp->An, 0., (siz_*mtp->Nb) );
  for( ii=0; ii<siz_; ii++ )
     aFo[ii].Stm_line( TRMults::sm->GetMU()->N, mtp->An+ii*TRMults::sm->GetMU()->N,
           (char *)TRMults::sm->GetMU()->SB, TRMults::sm->GetMU()->Val );
  aFo.Clear();
}

// Calculate data for matrix Bn as Bb_Calc
void
TGEM2MT::Bn_Calc()
{
    long int i, j;
    double Msysb_bk, Tmolb_bk;
    double MsysC = 0., R1C = 0.;
    double Xincr, ICmw, DCmw;
    vstr  pkey(MAXRKEYLEN+10);
    double  *ICw;  //IC atomic (molar) masses [0:Nmax-1]
    double *A;
    time_t crt;

    if( mtp->PvICi == S_OFF  &&   mtp->PvAUi == S_OFF )
       return;

// get data fron IComp
    TIComp* aIC=(TIComp *)(&aMod[RT_ICOMP]);
    aIC->ods_link(0);
    ICw = new double[mtp->Nb];
    memset( pkey, 0, MAXRKEYLEN+9 );
    for( i=0; i<mtp->Nb; i++ )
    {
        // load molar mass
        memcpy( pkey, TRMults::sm->GetMU()->SB[i], IC_RKLEN );
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

  for( long int ii=0; ii< mtp->nIV; ii++ )
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
    { // formula list

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
       MsysC = 0.;  // Calculation of the system mass in kg
       for( i=0; i<mtp->Nb; i++ )
          MsysC += mtp->Bn[ii*mtp->Nb+i] * ICw[i];
       mtp->Msysb = MsysC/1e3;
       mtp->PTVm[ii][4] = mtp->Msysb;
  } // ii

  mtp->Msysb = Msysb_bk;
  mtp->Tmolb = Tmolb_bk;
  delete[]  ICw;
}


// Generating new System for the node
//
void TGEM2MT::gen_task( bool startSys )
{
    long int i=0, kv = mtp->kv, qc = mtp->qc, Nb = mtp->Nb;
    double ScalingFactor = 1., Bi = 0., Bis = 0.;

    if( startSys )
    {
        TProfil::pm->Clear_XeA_XeD_Phm_BIun();
    }
    else
    {   // Take mass conversion factor from StaP[3] and PTvm[4]
        if(mtp->PTVm[kv][4] > 0. )
            ScalingFactor = mtp->StaP[qc][3] / mtp->PTVm[kv][4];
        if(fabs(log10(ScalingFactor)) < 1e-13)
            ScalingFactor = 1.;
    }
    // put Bb data to system  ( into B and bi_)
    for( i=0; i<mtp->Nb; i++)
    {
       Bi = mtp->Bn[ kv*Nb+i ];
       Bis = Bi*ScalingFactor;   // Debugging
       TSyst::sm->GetSY()->B[i] =  Bis;
       TSyst::sm->GetSY()->BI[i] = Bis;
    //   TSyst::sm->GetSY()->BIun[i] =  QUAN_MOL;
    }
}

void TGEM2MT::outMulti()
{
  // generate Tval&Pval arrays
  if( mtp->PsTPai != S_OFF )
     gen_TPval();

   na->MakeNodeStructures( mtp->nICb, mtp->nDCb,  mtp->nPHb,
     mtp->xIC, mtp->xDC, mtp->xPH, mtp->PsTPpath != S_OFF,
     mtp->Tval, mtp->Pval,
     mtp->nTai,  mtp->nPai, mtp->Tai[3], mtp->Pai[3]  );

   showMss = 1L;
   for( mtp->kv = 0; mtp->kv < mtp->nIV; mtp->kv++ )
   {
     mtp->qc = mtp->kv;
      pVisor->Message( window(), GetName(),
      "Calculation of equilibria in nodes. "
           "Please, wait...", mtp->kv, mtp->nIV);

     // Make new Systat record & calculate it
     gen_task( true );

     // calculate EqStat record (Thermodynamic&Equlibria)
     //  TProfil::pm->pmp->pTPD = 0;
     calc_eqstat( true );

     // Save databr
     na->packDataBr();
     na->MoveWorkNodeToArray( mtp->kv, mtp->nC, na->pNodT0() );
     na->pNodT0()[mtp->kv]->NodeTypeHY = mtp->DiCp[mtp->kv][1];

     mt_next();      // Generate work values for the next EqStat rkey

 } // mtp->kv

  pVisor->CloseMessage();

  allocNodeWork();  //????
  LinkCSD(0);

  na->PutGEM2MTFiles( window(),
       mtp->nIV, mtp->PsSdat==S_OFF, mtp->PsSdef!=S_OFF, mtp->PsScom!=S_OFF, false, false ); // mui,muj,muk do not output

  }


// ===========================================================


// Show plots for all nodes
void TGEM2MT::CalcGraph()
{
  if( mtp->PvMSg  == S_OFF )
    return;

 // refresh graph if it is shown
//  if( gd_gr )
//    gd_gr->Show();

// LinkCSD(0);
 for( long int ii=0; ii<mtp->nC; ii++)
 {
   mtp->jt = min( ii, (mtp->nC-1));
   mtp->qc = ii;
   LinkNode0(ii);
   LinkNode1(ii);
   rpn[1].CalcEquat();
//   CalcPoint( ii );
 }

  if( mtp->PsSmode != S_OFF  )
    return;
 // show graphic
 // use refresh if needed
 char buf[300];
 sprintf(buf, "  ( time %lg; step %ld )", mtp->cTau, mtp->ct );
  if( gd_gr )
    gd_gr->Show( buf);
}

// Calculation of initialization script
void TGEM2MT::CalcStartScript()
{
  if( mtp->PvMSt  == S_OFF  )
    return;

  // generate Ti, Pi, Vi, DiCp, HydP  arrays
    for( long int ii=0; ii< max( mtp->nC, mtp->nFD ); ii++)
    {
      mtp->jt = min( ii, (mtp->nC-1));
      mtp->qc = ii;
      rpn[0].CalcEquat();
    }

}

// working with scripts --------------------------------------------
// Translate, analyze and unpack equations (o_mttexpr or o_mtgexpr)

int get_ndx_(  int i,  int nO,  int Xplace )
{
   int ii, jj=i;

  if( nO ==o_n0_bps || nO == o_n1_bps )
   if( Xplace )
      nO = o_n0_bic;

  int N=0, type = 0;
  long int *arr=0;

  switch( nO )
  {
    case o_n0_xdc:  // CH->nDCb
    case o_n0_gam:
    case o_n0_dul:
    case o_n0_dll:
    case o_n1_xdc:  // CH->nDCb
    case o_n1_gam:
    case o_n1_dul:
    case o_n1_dll: N = aObj[o_mt_xdc].GetN();
                   arr = (long int *)aObj[o_mt_xdc].GetPtr();
                   type = 2;
                   break;
     case o_n1_xph:
     case o_n0_xph: //CH->nPHb
                    N = aObj[o_mt_xph].GetN();
                    arr = (long int *)aObj[o_mt_xph].GetPtr();
                    type = 3;
                    break;
    case  o_n0_vps:  // CH->nPSb
    case  o_n0_mps:
    case  o_n0_xpa:
    case  o_n0_bps:
    case  o_n1_vps:  // CH->nPSb
    case  o_n1_mps:
    case  o_n1_xpa:
    case  o_n1_bps:
                    N = (int)aObj[o_mtchbr].Get(0);
                    arr = (long int *)aObj[o_mt_xph].GetPtr();
                    type = 4;
                    break;
     case o_n0_bic:  //CH->nICb
     case o_n0_rmb:
     case o_n0_uic:
     case o_n0_bsp:
     case o_n1_bic:  //CH->nICb
     case o_n1_rmb:
     case o_n1_uic:
     case o_n1_bsp:
                    N = aObj[o_mt_xic].GetN();
                    arr = (long int *)aObj[o_mt_xic].GetPtr();
                    type = 1;
                    break;
  }
  if( type > 0 )
  {  for( ii=0; ii<N; ii++)
        if( arr[ii] == jj )
         {
              jj = ii;
              break;
         }
     ErrorIf( ii == N, "Invalid component name for DataBr",
           "E95MSTran: Error in translation of GEM2MT math script ");
   }

  return jj;
}

void TGEM2MT::Expr_analyze( int obj_num )
{
    try
    {
        TProfil* PRof = TProfil::pm;
        int mupL=0, pmpL =0;

        if( obj_num == o_mtgexpr )
        { // setup sizes
          LinkNode0(0);
          LinkNode1(0);
        }

        if( pVisor->ProfileMode == true )
        {
            mupL = (int)TRMults::sm->GetMU()->L;
            pmpL = (int)TMulti::sm->GetPM()->L;
        }
        PRof->ET_translate( (int)o_mwetext, (int)obj_num, 0,
             mupL, 0, pmpL, get_ndx_ );
        if( obj_num == o_mttexpr )
          rpn[0].GetEquat( (char *)aObj[o_mwetext].GetPtr() );
        else
          rpn[1].GetEquat( (char *)aObj[o_mwetext].GetPtr() );

        if( obj_num == o_mtgexpr )
        { // reset system
          LinkNode0(-1);
          LinkNode1(-1);
        }
    }
    catch( TError& xcpt )
    {
        if( obj_num == o_mtgexpr )
        { // reset system
          LinkNode0(-1);
          LinkNode1(-1);
        }

        char *erscan = (char *)aObj[o_mwetext/*obj_num*/].GetPtr();
        vfMessage(window(), xcpt.title, xcpt.mess);
        CheckEqText(  erscan,
               "E96MSTran: Error in translation of GEM2MT math script: " );
        Error(  GetName() , xcpt.mess.c_str() );
    }
}

// plotting the record -------------------------------------------------
//Added one point to graph
void
TGEM2MT::CalcPoint( int nPoint )
{
    if( mtp->PvMSg == S_OFF || nPoint >= mtp->nC )
     return;
    // Add point to graph screen
    if( gd_gr )
        gd_gr->AddPoint( 0, nPoint, true );
}

// Plotting record
void
TGEM2MT::RecordPlot( const char* /*key*/ )
{
    if( mtp->PvMSg == S_OFF )
      return;

    TIArray<TPlot> plt;

    plt.Add( new TPlot(o_mtxt, o_mtyt ));
    int  nLn = plt[ 0 ].getLinesNumber();
    if( mtp->PvEF != S_OFF )
    {
        plt.Add( new TPlot(o_mtxet, o_mtyet ));
        nLn += plt[1].getLinesNumber();
    }
    if( plot )
    {
        int oldN = aObj[o_mtplline].GetN();

        plot = (TPlotLine * )aObj[ o_mtplline ].Alloc( nLn, sizeof(TPlotLine) );
        for(int ii=0; ii<nLn; ii++ )
        {
            if( ii >= oldN )
            {
                if(ii < mtp->nYS )
                {
                    TPlotLine defpl(ii, nLn, "",6,0,2);
                    plot[ii] = defpl;
                }
                else
                {
                    TPlotLine defpl(ii, nLn, "",7,7,0);
                    plot[ii] = defpl;
                }
            }
            if(ii < mtp->nYS )
               plot[ii].setName( mtp->lNam[ii] );
               // strncpy( plot[ii].name, mtp->lNam[ii], MAXGRNAME-1 );
            else
                plot[ii].setName( mtp->lNamE[ii-mtp->nYS]);
               // strncpy( plot[ii].name, mtp->lNamE[ii-mtp->nYS], MAXGRNAME-1 );
            //plot[ii].name[MAXGRNAME-1] = '\0';
        }
        gd_gr = new GraphWindow( this, plt, mtp->name,
              mtp->size[0], mtp->size[1], plot,
              mtp->axisType, mtp->xNames, mtp->yNames);
    }
    else
    {
      TCStringArray lnames;
      int ii;
      for( ii=0; ii<mtp->nYS; ii++ )
          lnames.Add( gstring(mtp->lNam[ii], 0, MAXGRNAME ));
      for( ii=0; ii<mtp->nYE; ii++ )
          lnames.Add( gstring( mtp->lNamE[ii], 0, MAXGRNAME ));
      gd_gr = new GraphWindow( this, plt, mtp->name,
          mtp->xNames, mtp->yNames, lnames );
    }
}


// Save changes was done in Plotting dialog
bool
TGEM2MT::SaveGraphData( GraphData *gr )
{
// We can only have one Plot dialog (modal one) so condition should be omitted!!
     if( !gd_gr )
      return false;
     if( gr != gd_gr->getGraphData() )
      return false;
    mtp->axisType[0] = (short)gr->axisType;
    mtp->axisType[4] = (short)gr->graphType;
    mtp->axisType[1] = (short)gr->b_color[0];
    mtp->axisType[2] = (short)gr->b_color[1];
    mtp->axisType[3] = (short)gr->b_color[2];
    strncpy( mtp->xNames, gr->xName.c_str(), 9);
    strncpy( mtp->yNames, gr->yName.c_str(), 9);
    memcpy( &mtp->size[0], gr->region, 4*sizeof(float) );
    memcpy( &mtp->size[1], gr->part,  4*sizeof(float) );

    plot = (TPlotLine *) aObj[ o_mtplline].Alloc(
       gr->lines.GetCount(), sizeof(TPlotLine));
    for(int ii=0; ii<(int)gr->lines.GetCount(); ii++ )
    {
        plot[ii] = gr->lines[ii];
        //  lNam0 and lNamE back
        if( ii < mtp->nYS )
            strncpy(  mtp->lNam[ii], plot[ii].getName().c_str(), MAXGRNAME );
        else
            strncpy(  mtp->lNamE[ii-mtp->nYS], plot[ii].getName().c_str(), MAXGRNAME );
    }
    if( gr->graphType == ISOLINES )
       gr->getColorList();

    pVisor->Update();
    contentsChanged = true;

    return true;
}

// Link na->pNodT0()[nNode] for internal object list
// nNode < 0 set up NULL pointers  gfor objects
void  TGEM2MT::LinkNode0( long int nNode )
{
  long int ii;
  DATABR* BR;

  if( nNode >= 0   )
  {  BR = na->pNodT0()[nNode];
     DATACH* CH = na->pCSD();
     // static
     aObj[o_n0_ct].SetPtr( &BR->NodeHandle );   /* s6 */
     aObj[o_n0_cs].SetPtr( &BR->TK );            /* d17 */
     aObj[o_n0_ts].SetPtr( &BR->Tm );            /* d2 or d19 */
#ifndef NODEARRAYLEVEL
aObj[o_n0_ts].SetM( 2 );
#endif
     //dynamic
     aObj[o_n0_xdc].SetPtr( BR->xDC );
     aObj[o_n0_xdc].SetDim( CH->nDCb, 1 );
     aObj[o_n0_gam].SetPtr( BR->gam );
     aObj[o_n0_gam].SetDim( CH->nDCb, 1 );
     aObj[o_n0_xph].SetPtr( BR->xPH );
     aObj[o_n0_xph].SetDim( CH->nPHb, 1 );
     aObj[o_n0_vps].SetPtr( BR->vPS );
     aObj[o_n0_vps].SetDim( CH->nPSb, 1 );
     aObj[o_n0_mps].SetPtr( BR->mPS );
     aObj[o_n0_mps].SetDim( CH->nPSb, 1 );
     aObj[o_n0_bps].SetPtr( BR->bPS );
     aObj[o_n0_bps].SetDim( CH->nPSb, CH->nICb );
     aObj[o_n0_xpa].SetPtr( BR->xPA );
     aObj[o_n0_xpa].SetDim( CH->nPSb, 1 );
     aObj[o_n0_dul].SetPtr( BR->dul );
     aObj[o_n0_dul].SetDim( CH->nDCb, 1 );
     aObj[o_n0_dll].SetPtr( BR->dll );
     aObj[o_n0_dll].SetDim( CH->nDCb, 1 );
     aObj[o_n0_bic].SetPtr( BR->bIC );
     aObj[o_n0_bic].SetDim( CH->nICb, 1 );
     aObj[o_n0_rmb].SetPtr( BR->rMB );
     aObj[o_n0_rmb].SetDim( CH->nICb, 1 );
     aObj[o_n0_uic].SetPtr( BR->uIC );
     aObj[o_n0_uic].SetDim( CH->nICb, 1 );
     aObj[o_n0_bsp].SetPtr( BR->bSP );
     aObj[o_n0_bsp].SetDim( CH->nICb, 1 );
     aObj[o_n0_aph].SetPtr( BR->aPH );
     aObj[o_n0_aph].SetDim( CH->nPHb, 1 );
     // set data to work arrays
         na->CopyWorkNodeFromArray( nNode, mtp->nC,  na->pNodT0() );
         double *mps = (double *)aObj[o_n0w_mps].GetPtr();
         double *vps = (double *)aObj[o_n0w_vps].GetPtr();
         double *m_t = (double *)aObj[o_n0w_m_t].GetPtr();
         double *con = (double *)aObj[o_n0w_con].GetPtr();
         double *mju = (double *)aObj[o_n0w_mju].GetPtr();
         double *lga = (double *)aObj[o_n0w_lga].GetPtr();

         for( ii=0; ii<CH->nPHb; ii++)
         {
             mps[ii] = na->Ph_Mass(ii);
             vps[ii] = na->Ph_Volume(ii);
         }
         for( ii=0; ii<CH->nDCb; ii++)
         {
             con[ii] = na->Get_cDC(ii);
             mju[ii] = na->Get_muDC(ii, false );
             lga[ii] = na->Get_aDC(ii, false );
         }
         for( ii=0; ii<CH->nICb; ii++)
             m_t[ii] = na->Get_mIC( ii );
  }
  else
  {
     // static
     aObj[o_n0_ct].SetPtr( 0 );   /* s6 */
     aObj[o_n0_cs].SetPtr( 0 );            /* d17 */
     aObj[o_n0_ts].SetPtr( 0 );            /* d19 */
     //dynamic
     aObj[o_n0_xdc].SetPtr( 0 );
     aObj[o_n0_xdc].SetDim( 0, 1 );
     aObj[o_n0_gam].SetPtr( 0 );
     aObj[o_n0_gam].SetDim( 0, 1 );
     aObj[o_n0_xph].SetPtr( 0 );
     aObj[o_n0_xph].SetDim( 0, 1 );
     aObj[o_n0_vps].SetPtr( 0 );
     aObj[o_n0_vps].SetDim( 0, 1 );
     aObj[o_n0_mps].SetPtr( 0 );
     aObj[o_n0_mps].SetDim( 0, 1 );
     aObj[o_n0_bps].SetPtr( 0 );
     aObj[o_n0_bps].SetDim( 0,1 );
     aObj[o_n0_xpa].SetPtr( 0 );
     aObj[o_n0_xpa].SetDim( 0, 1 );
     aObj[o_n0_dul].SetPtr( 0 );
     aObj[o_n0_dul].SetDim( 0, 1 );
     aObj[o_n0_dll].SetPtr( 0 );
     aObj[o_n0_dll].SetDim( 0, 1 );
     aObj[o_n0_bic].SetPtr( 0 );
     aObj[o_n0_bic].SetDim( 0, 1 );
     aObj[o_n0_rmb].SetPtr( 0 );
     aObj[o_n0_rmb].SetDim( 0, 1 );
     aObj[o_n0_uic].SetPtr( 0 );
     aObj[o_n0_uic].SetDim( 0, 1 );
     aObj[o_n0_bsp].SetPtr( 0 );
     aObj[o_n0_bsp].SetDim( 0, 1 );
     aObj[o_n0_aph].SetPtr( 0);
     aObj[o_n0_aph].SetDim( 0, 1 );
   }
}


// Link na->pNodT1()[nNode] for internal object list
// nNode < 0 set up NULL pointers  for objects
void  TGEM2MT::LinkNode1(  long int nNode )
{
  DATABR* BR;
  long int ii;

  if( nNode >= 0   )
  {  BR = na->pNodT1()[nNode];
     DATACH* CH = na->pCSD();
     // static
     aObj[o_n1_ct].SetPtr( &BR->NodeHandle );   /* s6 */
     aObj[o_n1_cs].SetPtr( &BR->TK );            /* d17 */
     aObj[o_n1_ts].SetPtr( &BR->Tm );       // d2 or d19
#ifndef NODEARRAYLEVEL
aObj[o_n1_ts].SetM( 2 );
#endif
     //dynamic
     aObj[o_n1_xdc].SetPtr( BR->xDC );
     aObj[o_n1_xdc].SetDim( CH->nDCb, 1 );
     aObj[o_n1_gam].SetPtr( BR->gam );
     aObj[o_n1_gam].SetDim( CH->nDCb, 1 );
     aObj[o_n1_xph].SetPtr( BR->xPH );
     aObj[o_n1_xph].SetDim( CH->nPHb, 1 );
     aObj[o_n1_vps].SetPtr( BR->vPS );
     aObj[o_n1_vps].SetDim( CH->nPSb, 1 );
     aObj[o_n1_mps].SetPtr( BR->mPS );
     aObj[o_n1_mps].SetDim( CH->nPSb, 1 );
     aObj[o_n1_bps].SetPtr( BR->bPS );
     aObj[o_n1_bps].SetDim( CH->nPSb, CH->nICb );
     aObj[o_n1_xpa].SetPtr( BR->xPA );
     aObj[o_n1_xpa].SetDim( CH->nPSb, 1 );
     aObj[o_n1_dul].SetPtr( BR->dul );
     aObj[o_n1_dul].SetDim( CH->nDCb, 1 );
     aObj[o_n1_dll].SetPtr( BR->dll );
     aObj[o_n1_dll].SetDim( CH->nDCb, 1 );
     aObj[o_n1_bic].SetPtr( BR->bIC );
     aObj[o_n1_bic].SetDim( CH->nICb, 1 );
     aObj[o_n1_rmb].SetPtr( BR->rMB );
     aObj[o_n1_rmb].SetDim( CH->nICb, 1 );
     aObj[o_n1_uic].SetPtr( BR->uIC );
     aObj[o_n1_uic].SetDim( CH->nICb, 1 );
     aObj[o_n1_bsp].SetPtr( BR->bSP );
     aObj[o_n1_bsp].SetDim( CH->nICb, 1 );
     aObj[o_n1_aph].SetPtr( BR->aPH );
     aObj[o_n1_aph].SetDim( CH->nPHb, 1 );

 // set data to work arrays
     na->CopyWorkNodeFromArray( nNode, mtp->nC,  na->pNodT1() );
     double *mps = (double *)aObj[o_n1w_mps].GetPtr();
     double *vps = (double *)aObj[o_n1w_vps].GetPtr();
     double *m_t = (double *)aObj[o_n1w_m_t].GetPtr();
     double *con = (double *)aObj[o_n1w_con].GetPtr();
     double *mju = (double *)aObj[o_n1w_mju].GetPtr();
     double *lga = (double *)aObj[o_n1w_lga].GetPtr();

     for( ii=0; ii<CH->nPHb; ii++)
     {
         mps[ii] = na->Ph_Mass(ii);
         vps[ii] = na->Ph_Volume(ii);
     }
     for( ii=0; ii<CH->nDCb; ii++)
     {
         con[ii] = na->Get_cDC(ii);
         mju[ii] = na->Get_muDC(ii, false );
         lga[ii] = na->Get_aDC(ii, false );
     }
     for( ii=0; ii<CH->nICb; ii++)
         m_t[ii] = na->Get_mIC( ii );
  }
  else
  {
     // static
     aObj[o_n1_ct].SetPtr( 0 );   /* s6 */
     aObj[o_n1_cs].SetPtr( 0 );            /* d17 */
     aObj[o_n1_ts].SetPtr( 0 );            /* d19 */
     //dynamic
     aObj[o_n1_xdc].SetPtr( 0 );
     aObj[o_n1_xdc].SetDim( 0, 1 );
     aObj[o_n1_gam].SetPtr( 0 );
     aObj[o_n1_gam].SetDim( 0, 1 );
     aObj[o_n1_xph].SetPtr( 0 );
     aObj[o_n1_xph].SetDim( 0, 1 );
     aObj[o_n1_vps].SetPtr( 0 );
     aObj[o_n1_vps].SetDim( 0, 1 );
     aObj[o_n1_mps].SetPtr( 0 );
     aObj[o_n1_mps].SetDim( 0, 1 );
     aObj[o_n1_bps].SetPtr( 0 );
     aObj[o_n1_bps].SetDim( 0,1 );
     aObj[o_n1_xpa].SetPtr( 0 );
     aObj[o_n1_xpa].SetDim( 0, 1 );
     aObj[o_n1_dul].SetPtr( 0 );
     aObj[o_n1_dul].SetDim( 0, 1 );
     aObj[o_n1_dll].SetPtr( 0 );
     aObj[o_n1_dll].SetDim( 0, 1 );
     aObj[o_n1_bic].SetPtr( 0 );
     aObj[o_n1_bic].SetDim( 0, 1 );
     aObj[o_n1_rmb].SetPtr( 0 );
     aObj[o_n1_rmb].SetDim( 0, 1 );
     aObj[o_n1_uic].SetPtr( 0 );
     aObj[o_n1_uic].SetDim( 0, 1 );
     aObj[o_n1_bsp].SetPtr( 0 );
     aObj[o_n1_bsp].SetDim( 0, 1 );
     aObj[o_n1_aph].SetPtr( 0);
     aObj[o_n1_aph].SetDim( 0, 1 );
   }
}

// Link na->pCSD() structure for internal object list
// nNode < 0 set up NULL pointers  for objects
void  TGEM2MT::LinkCSD(  long int nNode )
{
  if( nNode >= 0   )
  {
     DATACH* CH = na->pCSD();
     // static
     aObj[o_ch_nicb].SetPtr( &CH->nICb );                     /* l1 */
     aObj[o_ch_ndcb].SetPtr( &CH->nDCb );                     /* l1 */
     aObj[o_ch_nphb].SetPtr( &CH->nPHb );                     /* l1 */
     aObj[o_ch_npsb].SetPtr( &CH->nPSb );                     /* l1 */
     //dynamic
     aObj[o_ch_xic].SetPtr( CH->xic );
     aObj[o_ch_xic].SetDim( CH->nICb, 1 );
     aObj[o_ch_xdc].SetPtr( CH->xdc );
     aObj[o_ch_xdc].SetDim( CH->nDCb, 1 );
     aObj[o_ch_xph].SetPtr( CH->xph );
     aObj[o_ch_xph].SetDim( CH->nPHb, 1 );
     aObj[o_ch_a].SetPtr( CH->A );
     aObj[o_ch_a].SetDim( CH->nDC ,CH->nIC );
     aObj[o_ch_icmm].SetPtr( CH->ICmm );
     aObj[o_ch_icmm].SetDim( CH->nIC, 1 );
     aObj[o_ch_dcmm].SetPtr( CH->DCmm );
     aObj[o_ch_dcmm].SetDim( CH->nDC, 1 );
     aObj[o_ch_dd].SetPtr( CH->DD );
     aObj[o_ch_dd].SetDim( CH->nDCs, na->gridTP());
//     aObj[o_ch_aalp].SetPtr( CH->Aalp );
//     aObj[o_ch_aalp].SetDim( CH->nPH, 1 );
  }
  else
  {
     // static
     aObj[o_ch_nicb].SetPtr( 0 );                     /* i1 */
     aObj[o_ch_ndcb].SetPtr( 0 );                     /* i1 */
     aObj[o_ch_nphb].SetPtr( 0 );                     /* i1 */
     aObj[o_ch_npsb].SetPtr( 0 );                     /* i1 */
     //dynamic
     aObj[o_ch_xic].SetPtr( 0 );
     aObj[o_ch_xic].SetDim( 0, 1 );
     aObj[o_ch_xdc].SetPtr( 0 );
     aObj[o_ch_xdc].SetDim( 0, 1 );
     aObj[o_ch_xph].SetPtr( 0 );
     aObj[o_ch_xph].SetDim( 0, 1 );
     aObj[o_ch_a].SetPtr( 0 );
     aObj[o_ch_a].SetDim( 0 ,1 );
     aObj[o_ch_icmm].SetPtr( 0 );
     aObj[o_ch_icmm].SetDim( 0, 1 );
     aObj[o_ch_dcmm].SetPtr( 0 );
     aObj[o_ch_dcmm].SetDim( 0, 1 );
     aObj[o_ch_dd].SetPtr( 0 );
     aObj[o_ch_dd].SetDim( 0, 1 );
     aObj[o_ch_aalp].SetPtr( 0);
     aObj[o_ch_aalp].SetDim( 0, 1 );
  }
}

// alloc memory for work arrays for nodes na->pNodT0() and na->pNodT1()
void  TGEM2MT::allocNodeWork()
{
  DATACH* CH = na->pCSD();
  aObj[o_n0w_mps].Alloc( CH->nPHb, 1, D_);
  aObj[o_n1w_mps].Alloc( CH->nPHb, 1, D_);
  aObj[o_n0w_vps].Alloc( CH->nPHb, 1, D_);
  aObj[o_n1w_vps].Alloc( CH->nPHb, 1, D_);
  aObj[o_n0w_m_t].Alloc( CH->nICb, 1, D_);
  aObj[o_n1w_m_t].Alloc( CH->nICb, 1, D_);
  aObj[o_n0w_con].Alloc( CH->nDCb, 1, D_);
  aObj[o_n1w_con].Alloc( CH->nDCb, 1, D_);
  aObj[o_n0w_mju].Alloc( CH->nDCb, 1, D_);
  aObj[o_n1w_mju].Alloc( CH->nDCb, 1, D_);
  aObj[o_n0w_lga].Alloc( CH->nDCb, 1, D_);
  aObj[o_n1w_lga].Alloc( CH->nDCb, 1, D_);
}

void  TGEM2MT::freeNodeWork()
{
  aObj[o_n0w_mps].Free();
  aObj[o_n1w_mps].Free();
  aObj[o_n0w_vps].Free();
  aObj[o_n1w_vps].Free();
  aObj[o_n0w_m_t].Free();
  aObj[o_n1w_m_t].Free();
  aObj[o_n0w_con].Free();
  aObj[o_n1w_con].Free();
  aObj[o_n0w_mju].Free();
  aObj[o_n1w_mju].Free();
  aObj[o_n0w_lga].Free();
  aObj[o_n1w_lga].Free();
}


// --------------------- end of m_gem2mtc.cpp ---------------------------

