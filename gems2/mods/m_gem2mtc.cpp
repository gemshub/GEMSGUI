//-------------------------------------------------------------------
// $Id$
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


bool
TGEM2MT::test_sizes( )
{
  gstring err_str;


  mtp->Nb = TProfil::pm->mup->N;
  mtp->FIb = TProfil::pm->mup->Fi;
  mtp->Lb = TProfil::pm->mup->L;


  if( mtp->nC<=0 || mtp->nIV <= 0)
  {  err_str +=
"W02PErem: You forgot to specify the number of inital variants! \n Please, do it!";
    mtp->nC = mtp->nIV = 1;
  }

  if( mtp->PvAUi != S_OFF )
    if( mtp->Lbi <= 0 )
    {  err_str +=
"W02PErem: You forgot to specify the number of formula units! \n Please, do it!";
       mtp->Lbi = 1;
     }

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

  if( mtp->PvMSg != S_OFF )
    if( mtp->nC <= 0 || mtp->nYS <= 0 )
    {  err_str +=
"W02PErem: You forgot to specify the number of nodes or plots! \n Please, do it!";
      if( mtp->nC <= 0 ) mtp->nC = 1;
      if( mtp->nYS <= 0 ) mtp->nYS = 1;
     }

  if( mtp->PvEF != S_OFF )
    if( mtp->nE <= 0 || mtp->nYE <= 0 )
    {  err_str +=
"W02PErem: You forgot to specify the number of experimental points! \n Please, do it!";
      if( mtp->nE <= 0 ) mtp->nE = 1;
      if( mtp->nYE <= 0 ) mtp->nYE = 1;
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
  TMulti* mult = TProfil::pm->multi;
  TCStringArray aList;
  TCIntArray aSelIC;
  TCIntArray aSelDC;
  TCIntArray aSelPH;
  int ii;

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
    for( ii=0; ii< mult->GetPM()->N; ii++ )
    {  if( select_all )
         aSelIC.Add( ii );
       else
         aList.Add( gstring( mult->GetPM()->SB[ii], 0, MAXICNAME+MAXSYMB));
    }
    if( !select_all  )
      aSelIC = vfMultiChoiceSet(window(), aList,
          "Please, mark independent components for selection into DataBridge",
          aSelIC);

    aList.Clear();
    for( ii=0; ii< mult->GetPM()->L; ii++ )
    {  if( select_all )
         aSelDC.Add( ii );
       else
       aList.Add( gstring( mult->GetPM()->SM[ii], 0, MAXDCNAME));
    }
    if( !select_all  )
       aSelDC = vfMultiChoiceSet(window(), aList,
         "Please, mark dependent components for selection into DataBridge",
         aSelDC );

    aList.Clear();
    for( ii=0; ii< mult->GetPM()->FI; ii++ )
    {  if( select_all )
         aSelPH.Add( ii );
       else
       aList.Add( gstring( mult->GetPM()->SF[ii], 0, MAXPHNAME+MAXSYMB));
    }
    if( !select_all  )
       aSelPH = vfMultiChoiceSet(window(), aList,
         "Please, mark phases for selection into DataBridge",
         aSelPH);

// These dimensionalities define sizes of dynamic data in DATABT structure
  mtp->nICb = (short)aSelIC.GetCount();
  mtp->nDCb = (short)aSelDC.GetCount();
  mtp->nPHb = (short)aSelPH.GetCount();
  mtp->nPSb = 0;
  for( ii=0; ii< aSelPH.GetCount(); ii++, mtp->nPSb++ )
   if( aSelPH[ii] >= TProfil::pm->pmp->FIs )
       break;

// realloc memory
  dyn_new();

// set dynamic data
  for( ii=0; ii< aSelIC.GetCount(); ii++ )
    mtp->xIC[ii] = (short)aSelIC[ii];
  for( ii=0; ii< aSelDC.GetCount(); ii++ )
    mtp->xDC[ii] = (short)aSelDC[ii];
  for( ii=0; ii< aSelPH.GetCount(); ii++ )
    mtp->xPH[ii] = (short)aSelPH[ii];
}



// reset mt counters
void TGEM2MT::mt_reset()
{
// setup  counters
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
  mtp->gStat = GS_INDEF;
  mtp->iStat = GS_INDEF;
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

  aMod[RT_GEM2MT].ModUpdate("GEM2MT data sampling in progress...");

}


void TGEM2MT::outMulti()
{
  // generate Tval&Pval arrays
  if( mtp->PsTPai != S_OFF )
     gen_TPval();

   na->MakeNodeStructures( mtp->nICb, mtp->nDCb,  mtp->nPHb,
     mtp->xIC, mtp->xDC, mtp->xPH,
     mtp->Tval, mtp->Pval,
     mtp->nTai,  mtp->nPai, mtp->Tai[3], mtp->Pai[3]  );

   for( mtp->kv = 0; mtp->kv < mtp->nIV; mtp->kv++ )
   {
     pVisor->Message( window(), GetName(),
      "Generation of EqStat records\n"
           "Please, wait...", mtp->kv, mtp->nIV);

     // Make new Systat record & calculate it
     gen_task();

     // Save databr
     na->packDataBr();
     na->MoveWorkNodeToArray( mtp->kv, mtp->nC, na->pNodT0() );

     mt_next();      // Generate work values for the next EqStat rkey

 } // mtp->kv

  pVisor->CloseMessage();

  na->PutGEM2MTFiles( window(),
       mtp->nIV, mtp->PsSdat!=S_OFF, mtp->PsSdat==S_OFF );

}


// ===========================================================


// Show graphic lines for all nodes
void TGEM2MT::CalcGraph()
{
  if( mtp->PvMSg  == S_OFF  )
    return;

 // refresh graphic if it showed
//  if( gd_gr )
//    gd_gr->Show();

// LinkCSD(0);
 for( int ii=0; ii<mtp->nC; ii++)
 {
   mtp->jt = min( ii, (mtp->nC-1));
   mtp->qc = ii;
   LinkNode0(ii);
   LinkNode1(ii);
   rpn[1].CalcEquat();
//   CalcPoint( ii );
 }
 // show graphic
 // use refresh if need
  if( gd_gr )
    gd_gr->Show();

 // reset system
 //  LinkNode0(-1);
 //  LinkNode1(-1);
}

// working with scripts --------------------------------------------
// Translate, analyze and unpack equations (o_mttexpr or o_mtgexpr)

int get_ndx_( int i, int nO, int Xplace )
{
  int ii, jj=i;

  if( nO ==o_n0_bps || nO == o_n1_bps )
   if( Xplace )
      nO = o_n0_bic;

  int N, type = 0;
  short *arr;

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
                   arr = (short *)aObj[o_mt_xdc].GetPtr();
                   type = 2;
                   break;
     case o_n1_xph:
     case o_n0_xph: //CH->nPHb
                    N = aObj[o_mt_xph].GetN();
                    arr = (short *)aObj[o_mt_xph].GetPtr();
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
                    N = aObj[o_mtchbr].Get(3);
                    arr = (short *)aObj[o_mt_xph].GetPtr();
                    type = 4;
                    break;
     case o_n0_bic:  //CH->nICb
     case o_n0_rmb:
     case o_n0_uic:
     case o_n1_bic:  //CH->nICb
     case o_n1_rmb:
     case o_n1_uic:
                    N = aObj[o_mt_xic].GetN();
                    arr = (short *)aObj[o_mt_xic].GetPtr();
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
     ErrorIf( ii == N, "Illegal component name for DataBr",
           "E91MSTran: Error in translation of GtDemo math script ");
   }

  return jj;
}

void TGEM2MT::Expr_analyze( int obj_num )
{
    try
    {
        TProfil* PRof = TProfil::pm;
        int mupL=0, pmpL =0;

         // setup sizes
         LinkCSD(0);
         LinkNode0(0);
         LinkNode1(0);

        if( pVisor->ProfileMode == true )
        {
            mupL = PRof->mup->L;
            pmpL = PRof->pmp->L;
        }
        PRof->ET_translate( (int)o_mwetext, (int)obj_num, 0,
             (int)mupL, 0, (int)pmpL, get_ndx_ );
        if( obj_num == o_mttexpr )
          rpn[0].GetEquat( (char *)aObj[o_mwetext].GetPtr() );
        else
          rpn[1].GetEquat( (char *)aObj[o_mwetext].GetPtr() );

         // reset system
        LinkNode0(-1);
        LinkNode1(-1);
    }
    catch( TError& xcpt )
    {
         // reset system
        LinkCSD(-1);
        LinkNode0(-1);
        LinkNode1(-1);

        char *erscan = (char *)aObj[o_mwetext/*obj_num*/].GetPtr();
        vfMessage(window(), xcpt.title, xcpt.mess);
        CheckEqText(  erscan,
               "E91MSTran: Error in translation of GtDemo math script: " );
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
        TPlotLine defpl("", 4);

        plot = (TPlotLine * )aObj[ o_mtplline ].Alloc( nLn, sizeof(TPlotLine) );
        for(int ii=0; ii<nLn; ii++ )
        {
            if( ii >= oldN )
                plot[ii] = defpl;
            if(ii < mtp->nYS )
                strncpy( plot[ii].name, mtp->lNam[ii], MAXGRNAME );
            else
                strncpy( plot[ii].name, mtp->lNamE[ii-mtp->nYS], MAXGRNAME );
            plot[ii].name[MAXGRNAME] = '\0';
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
            strncpy(  mtp->lNam[ii], plot[ii].name, MAXGRNAME );
        else
            strncpy(  mtp->lNamE[ii-mtp->nYS], plot[ii].name, MAXGRNAME );
    }
    if( gr->graphType == ISOLINES )
       gr->getColorList();

    pVisor->Update();
    contentsChanged = true;

    return true;
}

// Link na->pNodT0()[nNode] for internal object list
// nNode < 0 set up NULL pointers  gfor objects
void  TGEM2MT::LinkNode0(  int nNode )
{
  DATABR* BR;

  if( nNode >= 0   )
  {  BR = na->pNodT0()[nNode];
     DATACH* CH = na->pCSD();
     // static
     aObj[o_n0_ct].SetPtr( &BR->NodeHandle );   /* s6 */
     aObj[o_n0_cs].SetPtr( &BR->T );            /* d17 */
     aObj[o_n0_ts].SetPtr( &BR->Tm );            /* d19 */
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
     aObj[o_n0_dr1].SetPtr( BR->dRes1 );
     aObj[o_n0_dr1].SetDim( 0, 1 );
     aObj[o_n0_dr2].SetPtr( BR->dRes2 );
     aObj[o_n0_dr2].SetDim( 0, 1 );
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
     aObj[o_n0_dr1].SetPtr( 0 );
     aObj[o_n0_dr1].SetDim( 0, 1 );
     aObj[o_n0_dr2].SetPtr( 0);
     aObj[o_n0_dr2].SetDim( 0, 1 );
   }
}


// Link na->pNodT1()[nNode] for internal object list
// nNode < 0 set up NULL pointers  for objects
void  TGEM2MT::LinkNode1(  int nNode )
{
  DATABR* BR;

  if( nNode >= 0   )
  {  BR = na->pNodT1()[nNode];
     DATACH* CH = na->pCSD();
     // static
     aObj[o_n1_ct].SetPtr( &BR->NodeHandle );   /* s6 */
     aObj[o_n1_cs].SetPtr( &BR->T );            /* d17 */
     aObj[o_n1_ts].SetPtr( &BR->Tm );            /* d19 */
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
     aObj[o_n1_dr1].SetPtr( BR->dRes1 );
     aObj[o_n1_dr1].SetDim( 0, 1 );
     aObj[o_n1_dr2].SetPtr( BR->dRes2 );
     aObj[o_n1_dr2].SetDim( 0, 1 );
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
     aObj[o_n1_dr1].SetPtr( 0 );
     aObj[o_n1_dr1].SetDim( 0, 1 );
     aObj[o_n1_dr2].SetPtr( 0);
     aObj[o_n1_dr2].SetDim( 0, 1 );
   }
}

// Link na->pCSD() structure for internal object list
// nNode < 0 set up NULL pointers  for objects
void  TGEM2MT::LinkCSD(  int nNode )
{
  if( nNode >= 0   )
  {
     DATACH* CH = na->pCSD();
     // static
     aObj[o_ch_nicb].SetPtr( &CH->nICb );                     /* i1 */
     aObj[o_ch_ndcb].SetPtr( &CH->nDCb );                     /* i1 */
     aObj[o_ch_nphb].SetPtr( &CH->nPHb );                     /* i1 */
     aObj[o_ch_npsb].SetPtr( &CH->nPSb );                     /* i1 */
     //dynamic
     aObj[o_ch_xic].SetPtr( CH->xIC );
     aObj[o_ch_xic].SetDim( CH->nICb, 1 );
     aObj[o_ch_xdc].SetPtr( CH->xDC );
     aObj[o_ch_xdc].SetDim( CH->nDCb, 1 );
     aObj[o_ch_xph].SetPtr( CH->xPH );
     aObj[o_ch_xph].SetDim( CH->nPHb, 1 );
     aObj[o_ch_a].SetPtr( CH->A );
     aObj[o_ch_a].SetDim( CH->nDC ,CH->nIC );
     aObj[o_ch_icmm].SetPtr( CH->ICmm );
     aObj[o_ch_icmm].SetDim( CH->nIC, 1 );
     aObj[o_ch_dcmm].SetPtr( CH->DCmm );
     aObj[o_ch_dcmm].SetDim( CH->nDC, 1 );
     aObj[o_ch_dd].SetPtr( CH->DD );
     aObj[o_ch_dd].SetDim( CH->nDC, 1 );
     aObj[o_ch_aalp].SetPtr( CH->Aalp );
     aObj[o_ch_aalp].SetDim( CH->nPH, 1 );
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


// --------------------- end of m_gem2mtc.cpp ---------------------------

