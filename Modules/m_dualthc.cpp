//-------------------------------------------------------------------
// $Id: m_dualthc.cpp 1374 2009-07-23 16:18:58Z gems $
//
// Implementation of TDualTh class, config and calculation functions
//
// Rewritten from C to C++ by S.Dmytriyeva
// Copyright (C) 1995-2001 S.Dmytriyeva, D.Kulik
//
// This file is part of a GEM-Selektor library for thermodynamic
// modelling by Gibbs energy minimization
// Uses: GEM-Selektor GUI GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the GPL v.3 license

//
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//


#include <cmath>
#include "m_dualth.h"
#include "m_syseq.h"
#include "m_icomp.h"
#include "m_compos.h"
#include "s_formula.h"
#include "visor.h"
#include "s_lsm.h"


// setup data before calculate EqStates
void TDualTh::Init_Generation()
{
   // put data to Bb
    Bb_Calc();
   // put data to Bn
    Bn_Calc();
}


// setup of the data before DualTh calculations
void TDualTh::Init_Analyse()
{
   dt_text_analyze();
   // for_n
   make_A( dtp->nM, dtp->for_n );
   build_mu_n();  // calculate new mu_n matrix
   CalcMoleFractNS();  // calculate mole fractions of end members here
}

// analyse calculated equlibria
void TDualTh::Analyse( )
{
  // Calculation of DualThermo results
  Calc_gmix_n( dtp->PsMode, dtp->PsSt );   // Retrieving table with mixing energies
  if( dtp->PsMode == DT_MODE_M || dtp->PsMode == DT_MODE_A )
  {
     // Activity coefficients were calculated from Calc_gmix_n !!
     Calc_muo_n( dtp->PsSt );  // Estimation of standard Gibbs energies
  }
  if( dtp->PsMode == DT_MODE_G )
  {
     Calc_gam_n( dtp->PsSt );  // Estimation of int. parameters and activity coeffs
//     Calc_act_n( dtp->PsSt );  // Calculation of activities
     // here call regression part, if necessary
     if( dtp->PsLSF != DT_LSF_N )
        RegressionLSM( 0 );
   // Built-in forward calculations of activity coefficients
     Calc_gam_forward( dtp->PvGam, dtp->PsIPf, 'F' ); // take fitted params
  }
//  if( dtp->PsMode == DT_MODE_X ) // Only calculation of activities
//  Calc_act_n( dtp->PsSt );
}

//-------------------------------------------------------------------------------

void TDualTh::get_RT_P( int ii, double& RT, double& P)
{
    if( dtp->PvTPI == S_ON && dtp->Tdq )
    {
       RT = R_CONSTANT * (dtp->Tdq[ii] + 273.15);
       P = dtp->Pdq[ii];
    }
    else
    {
       RT = R_CONSTANT * (dtp->Td[START_] + ii*dtp->Td[STEP_] + 273.15);
       P = dtp->Pd[START_] + ii*dtp->Pd[STEP_];
    }
}

// recalc working parametres
void TDualTh::dt_next()
{
     dtp->cT += dtp->Td[STEP_];
     dtp->cP += dtp->Pd[STEP_];
     dtp->cV += dtp->Vd[STEP_];
     dtp->c_tm += dtp->tmd[STEP_];
     dtp->c_NV += dtp->NVd[STEP_];
}

//make matrix An
void TDualTh::make_A( int siz_, char (*for_)[MAXFORMUNITDT] )
{
  // Get full matrix A
  std::vector<TFormula> aFo;
  std::string form;
  int ii;

  for( ii=0; ii<siz_; ii++ )
  {
     aFo.push_back( TFormula() );
     form = std::string( for_[ii], 0, MAXFORMUNITDT );
     strip( form );
     aFo[ii].SetFormula( form.c_str() ); // and ce_fscan
  }

  ErrorIf( dtp->Nb != TRMults::sm->GetMU()->N, GetName(),
               "Invalid data in dtp->Nb ");

  dtp->An = (float *)aObj[ o_dtan ]->Alloc( siz_, dtp->Nb, F_ );
  dtp->Asiz = (short)siz_;
  double *AA = new double[TRMults::sm->GetMU()->N]; // dtp->An must be double SD 21/07/2009
  fillValue(dtp->An, (float)0., (siz_*dtp->Nb) );
  for( ii=0; ii<siz_; ii++ )
  {  fillValue(AA, 0., TRMults::sm->GetMU()->N );
     aFo[ii].Stm_line( TRMults::sm->GetMU()->N, AA,
             (char *)TRMults::sm->GetMU()->SB, TRMults::sm->GetMU()->Val );
     copyValues( dtp->An+ii*TRMults::sm->GetMU()->N, AA, dtp->Nb );
  }
  delete[] AA;
  aFo.clear();
}

// make EqStat key  && calculate records
void TDualTh::calc_eqstat()
{
    vstr buf(40);
    double dummy = -1.;

    sprintf(buf, "%.4d", dtp->c_tm);
    memset(dtp->timep, 0, 5 );
    strncpy(dtp->timep, buf, 4 );
    sprintf(buf, "%.3d", dtp->c_NV);
    memset( dtp->NVp, 0, 4 );
    strncpy( dtp->NVp, buf, 3 );
    Gcvt( dtp->cT, 6, dtp->TCp );
    Gcvt( dtp->cP, 6, dtp->Pp );
    Gcvt( dtp->cV, 6, dtp->Bnamep );

   rt[RT_SYSEQ]->MakeKey( RT_DUALTH,  dtp->sykey, RT_DUALTH, 0, RT_DUALTH,1,
         RT_DUALTH, 2, K_IMM, dtp->timep, K_IMM, dtp->Bnamep,
         K_IMM, dtp->Pp, K_IMM, dtp->TCp, K_IMM, dtp->NVp, K_END );

   rt[RT_SYSEQ]->Find(dtp->sykey);
// calc current SyStat
   TProfil::pm->CalcEqstat( dummy );
   if( dtp->PsSYd == S_ON )
     TSysEq::pm->CmSave();           // save results to DB
   if( dtp->stld )
       memcpy( dtp->stld+dtp->q, dtp->sykey, EQ_RKLEN );
}

// Generating Ub matrix
void TDualTh::build_Ub()
{
 short i, ii;
 double RT/* = 2479.*/;

 dt_initiate( false );
 showMss = 1L;

 for( ii=0; ii<dtp->nQ; ii++)
 {
   dtp->q = ii;
   pVisor->Message( window(), GetName(),
             "Generation of EqStat records\n"
                 "Please, wait...", dtp->q, dtp->nQ);
   TProfil::pm->Clear_XeA_XeD_Phm_BIun();
   // put Bb data to system  ( into B and bi_)
    for( i=0; i<dtp->Nb; i++)
    {
       TSyst::sm->GetSY()->B[i] =  dtp->Bb[  dtp->q*dtp->Nb+i ];
       TSyst::sm->GetSY()->BI[i] =  dtp->Bb[  dtp->q*dtp->Nb+i];
       //TProfil::pm->syp->BIun[i] =  QUAN_MOL;
    }
// set zeros to xd_ and xa_
//    for( i=0; i < TProfil::pm->mup->L; i++)
//       TProfil::pm->syp->XeD[i] = 0.;
//    for( i=0; i < TProfil::pm->mup->La; i++)
//       TProfil::pm->syp->XeA[i] = 0.;

 // calculate EqStat record (Thermodynamic&Equlibria)
   //  TProfil::pm->pmp->pTPD = 0;
     calc_eqstat();

     RT = TMulti::sm->GetPM()->RT;
//   RT = R_CONSTANT * (Tdq[ii] + 273.15);
 // set zeros for result
    for( i=0; i<dtp->Nb; i++)
       dtp->Ub[dtp->q*dtp->Nb+i] = 0.;
 // copy calculated data from multy
    for( i=0; i<TMulti::sm->GetPM()->N; i++)
       dtp->Ub[ dtp->q*dtp->Nb + (TMulti::sm->GetPM()->mui[i])]
           = TMulti::sm->GetPM()->U[i] * RT;

    dt_next();      // Generate work values for the next EqStat rkey
    if( (dtp->PvTPI == S_ON) && dtp->Tdq && dtp->Pdq )
    {       // Take T and P from lists and store IS
        dtp->cT = dtp->Tdq[ii];
        dtp->cP = dtp->Pdq[ii];
        if(dtp->ISq)
           dtp->ISq[ii] = TMulti::sm->GetPM()->IC;
    }
  }
}

// Generating the mu_n matrix
void TDualTh::build_mu_n()
{
 short i, ii, j;
 double mu_sum;

// dt_initiate( false );

 for( ii=0; ii<dtp->nQ; ii++)
 {
   dtp->q = ii;
  // zero off a cell in mu_n
    for( j=0; j<dtp->nM; j++)
    {
       dtp->jm = j;
 //       dtp->mu_b[dtp->q*dtp->nM+j] = 0.;
//   calculate mu_b from Ub
       mu_sum = 0.0;
       for( i=0; i<dtp->Nb; i++)
         mu_sum += dtp->Ub[ii*dtp->Nb +i] * double(dtp->An[j*dtp->Nb +i]);
       dtp->mu_b[ii*dtp->nM+j] = mu_sum;
    }
  }
}

// Translate, analyze and unpack dual-thermo math script for activity coeffs
void TDualTh::dt_text_analyze()
{
  TProfil* PRof = dynamic_cast<TProfil *>( aMod[RT_PARAM].get());
/*  try
    {
      if( dtp->PvChi != S_OFF )
      {
        PRof->ET_translate( o_dttprn, o_dtcexpr,
                 0, PRof->mup->L, 0, PRof->pmp->L );
        rpn[0].GetEquat( (char *)aObj[o_dttprn].GetPtr() );
      }
    }
    catch( TError& xcpt )
    {
        char *erscan = (char *)aObj[o_dtcexpr].GetPtr();
        vfMessage(window(), xcpt.title, xcpt.mess);
        TDualTh::pm->CheckEqText(  erscan,
  "E93MSTran: Error in translation of math script \n"
  "            for mole fractions of non-basis DCs: " );
        Error(  GetName() , xcpt.mess.c_str() );
    }
*/
    try
    {
      if( dtp->PvGam != S_OFF )
      {

        PRof->ET_translate( o_dttprn, o_dtgexpr,
          0, TRMults::sm->GetMU()->L, 0, TMulti::sm->GetPM()->L );
        rpn[1].GetEquat( (char *)aObj[o_dttprn]->GetPtr() );
      }
    }
    catch( TError& xcpt )
    {
        char *erscan = (char *)aObj[o_dtgexpr]->GetPtr();
        vfMessage(window(), xcpt.title, xcpt.mess);
        TDualTh::pm->CheckEqText(  erscan,
  "E94MSTran: Error in translation of math script \n"
  "           for activity coefficients of non-basis DCs: " );
        Error(  GetName() , xcpt.mess.c_str() );
    }
}


//Recalc rpn structure of script for calculation of activity coefficients
void
TDualTh::CalcEquat()
{
     for( dtp->q=0; dtp->q<dtp->nQ; dtp->q++ )
       rpn[1].CalcEquat();
}


// Calculate data for matrix Bb
void
TDualTh::Bb_Calc()
{
    int i, j;
    double Msysb_bk, Tmolb_bk;
    double MsysC = 0., R1C = 0.;
    double Xincr, ICmw, DCmw;
    vstr  pkey(MAXRKEYLEN+10);
    float  *ICw;  //IC atomic (molar) masses [0:Nmax-1]
    float *A;
    time_t crt;

    if( dtp->PvICb == S_OFF  &&   dtp->PvAUb == S_OFF )
       return;

// get data fron IComp
    TIComp* aIC= dynamic_cast<TIComp *>( aMod[RT_ICOMP].get());
    aIC->ods_link(0);
    ICw = new float[dtp->Nb];
    memset( pkey, 0, MAXRKEYLEN+9 );
    for( i=0; i<dtp->Nb; i++ )
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
       make_A( dtp->La_b, dtp->for_b );

  Msysb_bk = dtp->Msysb;
  Tmolb_bk = dtp->Tmolb;

  for( int ii=0; ii< dtp->nQ; ii++ )
  {
    // set line in Bb to zeros
    memset( dtp->Bb + ii*dtp->Nb, 0, dtp->Nb*sizeof(double) );
    dtp->Msysb = Msysb_bk;
    dtp->Tmolb = Tmolb_bk;


    if( dtp->PvICb != S_OFF )
    { //  Through IC
        for( i=0; i<dtp->Nb; i++ )
        {
          if( approximatelyZero(dtp->CIb[ii*dtp->Nb + i]) ||
                 IsFloatEmpty( dtp->CIb[ ii*dtp->Nb + i ] ))
                continue;

          ICmw = ICw[i];
          Xincr = TCompos::pm->Reduce_Conc( dtp->CIclb[i],
             dtp->CIb[ii*dtp->Nb+i],
             ICmw, 1.0, dtp->Tmolb, dtp->Msysb, dtp->Mwatb,
             dtp->Vaqb, dtp->Maqb, dtp->Vsysb );

           dtp->Bb[ii*dtp->Nb+i] += Xincr;
           MsysC += Xincr*ICmw;
           R1C += Xincr;
        } //  i
    }

    if( dtp->PvAUb != S_OFF )
    { // formul list

      for( j=0; j < dtp->La_b; j++ )
      {
         A = dtp->An + j * dtp->Nb;
         if( approximatelyZero(dtp->CAb[ii*dtp->La_b + j]) ||
            IsFloatEmpty( dtp->CAb[ii*dtp->La_b + j] ))
                    continue;
         DCmw = 0.;
         for( i=0; i<dtp->Nb; i++ )
         // calculation of molar mass
             DCmw += A[i]* ICw[i];
         Xincr = TCompos::pm->Reduce_Conc( dtp->AUclb[j],
                dtp->CAb[ii*dtp->La_b + j],
                 DCmw, 1.0, dtp->Tmolb, dtp->Msysb, dtp->Mwatb,
                 dtp->Vaqb, dtp->Maqb, dtp->Vsysb );
         // recalc stoichiometry
         for( i=0; i<dtp->Nb; i++ )
          if( noZero(A[i]) )
          {
            dtp->Bb[ii*dtp->Nb+i] += Xincr*A[i]; // calc control sum
            MsysC += Xincr * A[i] * ICw[i];
            R1C += Xincr * A[i];
          }
        } //  j

      }
      MsysC /= 1e3;

      // Analyze control sum
      if( fabs( dtp->Tmolb ) < 1e-12 )
        dtp->Tmolb = R1C;
      if( fabs( dtp->Tmolb - R1C ) < 1e-12 || fabs( R1C ) < 1e-15 )
        /*Xincr = 1.*/;
       else
       { // normalisation
          Xincr = dtp->Tmolb / R1C;
          MsysC = 0.0;
          for( i=0; i<dtp->Nb; i++ )
            if( fabs( dtp->Bb[ii*dtp->Nb+i] ) >= 1e-12 )
            {
                dtp->Bb[ii*dtp->Nb+i] *= Xincr;
                MsysC += dtp->Bb[ii*dtp->Nb+i] * ICw[i];
            }
          dtp->Tmolb = R1C;
          MsysC /= 1e3;
       }

       if( fabs( dtp->Msysb ) < 1e-12 )
           dtp->Msysb = MsysC;
       if( fabs( dtp->Msysb - MsysC ) < 1e-12 || fabs( MsysC ) < 1e-15 )
        /*Xincr = 1.*/;
      else
      { // normalisation
         Xincr = dtp->Msysb / MsysC;
         R1C = 0.0;
         for( i=0; i<dtp->Nb; i++ )
             if( fabs( dtp->Bb[ii*dtp->Nb+i] ) >= 1e-12 )
             {
                dtp->Bb[ii*dtp->Nb+i] *= Xincr;
                R1C += dtp->Bb[ii*dtp->Nb+i];
             }
         dtp->Tmolb = R1C;
      }
  } // ii

  dtp->Msysb = Msysb_bk;
  dtp->Tmolb = Tmolb_bk;
  delete[]  ICw;

}

// Calculate data for matrix Bn
void
TDualTh::Bn_Calc()
{
    int i, j;
    double Msysb_bk, Tmolb_bk;
    double MsysC = 0., R1C = 0.;
    double Xincr, ICmw, DCmw;
    vstr  pkey(MAXRKEYLEN+10);
    float  *ICw;  //IC atomic (molar) masses [0:Nmax-1]
    float *A;
    time_t crt;

    if( dtp->PvICn == S_OFF  &&   dtp->PvAUb == S_OFF )
       return;

// get data fron IComp
    TIComp* aIC=dynamic_cast<TIComp *>( aMod[RT_ICOMP].get());
    aIC->ods_link(0);
    ICw = new float[dtp->Nb];
    memset( pkey, 0, MAXRKEYLEN+9 );
    for( i=0; i<dtp->Nb; i++ )
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
       make_A( dtp->La_b, dtp->for_b );

  Msysb_bk = dtp->Msysb;
  Tmolb_bk = dtp->Tmolb;

  for( int ii=0; ii< dtp->nQ; ii++ )
  {
    // set line in Bn to zeros
    memset( dtp->Bn + ii*dtp->Nb, 0, dtp->Nb*sizeof(double) );
    dtp->Msysb = Msysb_bk;
    dtp->Tmolb = Tmolb_bk;


    if( dtp->PvICn != S_OFF )
    { //  Through IC
        for( i=0; i<dtp->Nb; i++ )
        {
          if( approximatelyZero(dtp->CIn[ii*dtp->Nb + i]) ||
                 IsFloatEmpty( dtp->CIn[ ii*dtp->Nb + i ] ))
                continue;

          ICmw = ICw[i];
          Xincr = TCompos::pm->Reduce_Conc( dtp->CIcln[i],
             dtp->CIn[ii*dtp->Nb+i],
             ICmw, 1.0, dtp->Tmolb, dtp->Msysb, dtp->Mwatb,
             dtp->Vaqb, dtp->Maqb, dtp->Vsysb );

           dtp->Bn[ii*dtp->Nb+i] += Xincr;
           MsysC += Xincr*ICmw;
           R1C += Xincr;
        } //  i
    }

 if( dtp->PvAUb != S_OFF )
 { // formul list

    for( j=0; j < dtp->La_b; j++ )
    {
         A = dtp->An + j * dtp->Nb;
         if( approximatelyZero(dtp->CAn[ii*dtp->La_b + j]) ||
            IsFloatEmpty( dtp->CAn[ii*dtp->La_b + j] ))
                    continue;
         DCmw = 0.;
         for( i=0; i<dtp->Nb; i++ )
         // calculation of molar mass
             DCmw += A[i]* ICw[i];
         Xincr = TCompos::pm->Reduce_Conc( dtp->AUcln[j],
                dtp->CAn[ii*dtp->La_b + j],
                 DCmw, 1.0, dtp->Tmolb, dtp->Msysb, dtp->Mwatb,
                 dtp->Vaqb, dtp->Maqb, dtp->Vsysb );
         // recalc stoichiometry
         for( i=0; i<dtp->Nb; i++ )
          if( noZero(A[i]) )
          {
            dtp->Bn[ii*dtp->Nb+i] += Xincr*A[i]; // calc control sum
            MsysC += Xincr * A[i] * ICw[i];
            R1C += Xincr * A[i];
          }
        } //  j

     }
      MsysC /= 1e3;
      // Analyze control sum
      if( fabs( dtp->Tmolb ) < 1e-12 )
        dtp->Tmolb = R1C;
      if( fabs( dtp->Tmolb - R1C ) < 1e-12 || fabs( R1C ) < 1e-15 )
        /*Xincr = 1.*/;
       else
       { // normalisation
          Xincr = dtp->Tmolb / R1C;
          MsysC = 0.0;
          for( i=0; i<dtp->Nb; i++ )
            if( fabs( dtp->Bn[ii*dtp->Nb+i] ) >= 1e-12 )
            {
                dtp->Bn[ii*dtp->Nb+i] *= Xincr;
                MsysC += dtp->Bn[ii*dtp->Nb+i] * ICw[i];
            }
          dtp->Tmolb = R1C;
          MsysC /= 1e3;
       }

       if( fabs( dtp->Msysb ) < 1e-12 )
           dtp->Msysb = MsysC;
       if( fabs( dtp->Msysb - MsysC ) < 1e-12 || fabs( MsysC ) < 1e-15 )
        /*Xincr = 1.*/;
      else
      { // normalisation
         Xincr = dtp->Msysb / MsysC;
         R1C = 0.0;
         for( i=0; i<dtp->Nb; i++ )
             if( fabs( dtp->Bn[ii*dtp->Nb+i] ) >= 1e-12 )
             {
                dtp->Bn[ii*dtp->Nb+i] *= Xincr;
                R1C += dtp->Bn[ii*dtp->Nb+i];
             }
         dtp->Tmolb = R1C;
      }
  } // ii

  dtp->Msysb = Msysb_bk;
  dtp->Tmolb = Tmolb_bk;
  delete[]  ICw;

}

int
TDualTh::CalcMoleFractNS()  // Use SVD method
{
// if( aObj[o_dtchi].IsEmpty( 0, 0 ))   Disabled by KD 16.04.2006
// {
  short ii, jj;
  double *bb = new double[dtp->Nb];
  for( ii=0; ii< dtp->nQ; ii++ )
  {
     bool if_resid = false;
     TSVDcalc task_Axb( dtp->Nb, dtp->nM,
       dtp->An, dtp->Bn+(ii*dtp->Nb), dtp->chi+(ii*dtp->nM) );
     if( task_Axb.CalcSVD( true ) > 0 )
     {
            std::string str = "Mole fractions calculation routine ";
            str += " finds more than one solution for experiment ";
            str +=  dtp->nam_b[ii];
            str += "\n Please, check its non-basis bulk composition.";
            vfMessage( 0,  "W01SLrun: CalcMoleFractNS", str.c_str() );
     }
    // calculate new bn
     task_Axb.CalcB( true, bb );
     for( jj=0; jj<dtp->Nb; jj++ )
     {
       double b_ = bb[jj] - dtp->Bn[ ii*dtp->Nb+jj ];
       if( fabs(b_) > 1e-6 )
          if_resid = true;
       if( dtp->CIn )
          dtp->CIn[ ii*dtp->Nb+jj ] +=  ROUND_EXP( b_, 6 );
     }
     if( if_resid ) // residual
     {
            std::string str = "Mole fractions calculation routine ";
            str += " finds no exact solution for experiment ";
            str +=  dtp->nam_b[ii];
            str += ".\nLeast-squares solution Residuals ";
            str += "will be incremented in the CIn table.";
            vfMessage( 0,  "W02SLrun: CalcMoleFractNS", str.c_str() );
     }
    // normalise
     double cnt=0.;
     for( jj=0; jj<dtp->nM; jj++ )
        cnt += dtp->chi[ii*dtp->nM+jj];
     for( jj=0; jj<dtp->nM; jj++ )
        dtp->chi[ii*dtp->nM+jj] = ROUND_EXP( dtp->chi[ii*dtp->nM+jj] /cnt, 6);
  }
  delete[] bb;
// }
 return 1;

}

int
TDualTh::RegressionLSM( int /*Mode*/ )  // task or minimization
{
 int ii, jj;

 lmfit_new();

 // for internal constants
 double RT, P;
 for( ii=0; ii<dtp->nQ; ii++ )
  if(  dtp->PsIPf == DT_IPF_R  )
  {
    get_RT_P( ii, RT, P);
    dtp->yconst[ii] = RT;
  }
  else dtp->yconst[ii] = 1;

 // setup start data t, y, p
 // wpar, wdat must be given as input
 for( ii=0; ii<dtp->nQ; ii++ )
    for( jj=0; jj<dtp->nM; jj++ )
      dtp->tdat[ii*dtp->nM+jj] = dtp->chi[ii*dtp->nM+jj];

 for( ii=0; ii<dtp->nQ; ii++ )  // normalizing if necessary
 {
    switch( dtp->PsIPu )
    {
      default:
      case DT_IPU_J:  // J/mol
                     dtp->ydat[ii] = dtp->gmx_n[ii][0];
                     break;
      case DT_IPU_K:   // kJ/mol
                     dtp->ydat[ii] = dtp->gmx_n[ii][0]*0.001;
                     break;
      case DT_IPU_N:   // normalized
                     if( dtp->PsIPf != DT_IPF_R )
                        dtp->ydat[ii] = dtp->gmx_n[ii][0]/dtp->yconst[ii];
                     else
                        dtp->ydat[ii] = dtp->gmx_n[ii][0];
                     break;
    }
 }

 for( ii=0; ii<dtp->nP; ii++ )
   // dtp->par[ii] = dtp->avsd_w[ii];  // initial parameter values
   dtp->par[ii] = dtp->Wa_ap[ii];

  //  setup internal structure for calc
 char afType  = dtp->PsIPf;
 char *arpn = 0;
 //  setup internal structure for calc
 if( dtp->PvChi != S_OFF )
 {
   afType  = MATHSCRIPT_FIT;
   TProfil* PRof = dynamic_cast<TProfil *>( aMod[RT_PARAM].get());

   PRof->ET_translate( o_dttprn, o_dtcexpr,
     0, TRMults::sm->GetMU()->L, 0, TMulti::sm->GetPM()->L );
   arpn = (char *)aObj[o_dttprn]->GetPtr() ;
 }
 TLMDataType data( afType, TEST_EVL, // may be changed for flags
                       dtp->nQ, dtp->nM, dtp->nP, dtp->tdat,
                       dtp->ydat, dtp->yconst, dtp->wdat, arpn );
 // calculate minimization
 if( dtp->PsLSF == DT_LSF_L || dtp->PsLSF == DT_LSF_C )
 {
     TLMmin task( dtp->par, &data);
     task.Calc( dtp->sdpar, dtp->Wa_ap, dtp->wpar, dtp->wa_cp );
 }

 if( dtp->PsLSF == DT_LSF_S || dtp->PsLSF == DT_LSF_B )
 {
     TSVDcalc task_svd( dtp->par, &data);
     task_svd.CalcMin( dtp->sdpar );
 }

 // storing the results
 dtp->xi2 = data.getXi2();
 for( ii=0; ii<dtp->nP; ii++ )
 {  dtp->avsd_w[ii] = dtp->par[ii];  // mean data
    dtp->avsd_w[ii+dtp->nP] = dtp->sdpar[ii];  // mean data
 }

 return 1;
}

//----------------------------------------------------------------------------
void
TDualTh::Calc_muo_n( char eState )
{ // calculate muo_n DualTh
 short ii, j;
 double muo=0, mua=0, lnGam/*=1.*/, Dsur, RT/* = 2479.*/, P/*=1.*/, lnFmol=4.016535;
 double Chi, lnChi;
// dt_initiate( false );
  Dsur = dtp->WmCb - 1.;
  if( Dsur < 0. )
    Dsur = 0.;

 for( ii=0; ii < dtp->nQ; ii++)
 {
    dtp->q = ii;
    get_RT_P( ii, RT, P );

    for( j=0; j<dtp->nM; j++)
    {
       dtp->jm = j;
       lnGam = log( dtp->gam_n[ii*dtp->nM+j] );
       if( dtp->PsMode == DT_MODE_A)
           lnGam = 0.0;

       Chi = dtp->chi[ii*dtp->nM+j];  // Check of mole fraction and lnChi
       if( Chi < 1e-20 )
       {
          lnChi = log( 1e-20 );
          Chi = 1e-20;
          std::string str = "Zero or negative mole fraction chi";
            str += " encountered in the experiment ";
            str +=  dtp->nam_b[ii];
            str += ".\n Mole fraction is set to chi=1e-20 in ";
            str += "calculation of mu0 for the end member ";
            str += dtp->nam_n[j];
            str += ".\n Please, check end-member stoichiometries.";
            str += ".\n Click Ok to continue...";
            vfMessage( 0,  "W04SLrun: CalcMuoNS", str.c_str() );
        }
       else
          lnChi = log(dtp->chi[ii*dtp->nM+j]);

       if( eState == DT_STATE_S )
       {
        switch(dtp->typ_n[j])  // Stoich. saturation
        {
          default:
          case DC_SCP_CONDEN:
              muo = dtp->mu_b[ii*dtp->nM+j];
              break;
          case DC_SOL_IDEAL:
          case DC_SOL_MINOR: case DC_SOL_MINDEP: case DC_SOL_MAJDEP:
          case DC_SOL_MAJOR: // Check this calculation !!!!  Fixed 23.10.05 KD
              muo = dtp->gmx_n[ii][4] -dtp->gmx_n[ii][2]
                   -dtp->gmx_n[ii][4] +dtp->mu_o[ii*dtp->nM+j];
              muo /= Chi;
              break;
        }
        mua = muo + RT * lnGam;  // to be improved
       }
       else
       {     // Equilibrium
        switch(dtp->typ_n[j])
        {
          case DC_SCP_CONDEN:
              muo = mua = dtp->mu_b[ii*dtp->nM+j];
              break;
          case DC_SOL_IDEAL:
          case DC_SOL_MINDEP:
          case DC_SOL_MAJDEP:
          case DC_SOL_MINOR:
          case DC_SOL_MAJOR:
              muo = dtp->mu_b[ii*dtp->nM+j] - RT*( lnGam + lnChi );
              mua = muo + RT*lnGam;
              break;
          case DC_AQ_ELECTRON:
          case DC_AQ_PROTON:
          case DC_AQ_SPECIES: case DC_AQ_SURCOMP:
              muo = dtp->mu_b[ii*dtp->nM+j] + RT*( Dsur + lnFmol - lnGam - lnChi );
              mua = muo + RT*lnGam;
              break;
          case DC_AQ_SOLVENT:
          case DC_AQ_SOLVCOM:
              muo = dtp->mu_b[ii*dtp->nM+j] + RT * ( Dsur - 1. + 1. / ( 1.+Dsur )
                    - lnGam - lnChi);
              mua = muo;
              break;
          case DC_GAS_COMP:
          case DC_GAS_H2O:
          case DC_GAS_CO2:   /* gases */
          case DC_GAS_H2:
          case DC_GAS_N2:
              muo = dtp->mu_b[ii*dtp->nM+j] - RT*(lnGam + lnChi - log(P) );
              mua = muo + RT*lnGam;
              break;
/*
// adsorption:
        case DC_SUR_GROUP:
            pmp->Y_la[j] = ln_to_lg * ( Muj - pmp->G0[j]
                                         + Dsur + DsurT/( 1.0+DsurT ) + lnFmol );
            break;
        case DC_SSC_A0:
        case DC_SSC_A1:
        case DC_SSC_A2:
        case DC_SSC_A3:
        case DC_SSC_A4:
        case DC_WSC_A0:
        case DC_WSC_A1:
        case DC_WSC_A2:
        case DC_WSC_A3:
        case DC_WSC_A4:  // case DC_SUR_GROUP:
        case DC_SUR_COMPLEX:
        case DC_SUR_IPAIR:
        case DC_IESC_A:
        case DC_IEWC_B:
            DsurT = MMC * pmp->Aalp[k] * pa.p.DNS*1.66054e-6;
            pmp->Y_la[j] = ln_to_lg * ( Muj - pmp->G0[j]
                                         + Dsur + DsurT/( 1.0+DsurT ) + lnFmol );
            break; // Coulombic term to be considered !!!!!!!!!!
        case DC_PEL_CARRIER:
        case DC_SUR_MINAL:
        case DC_SUR_CARRIER: // sorbent
            DsurT = MMC * pmp->Aalp[k] * pa.p.DNS*1.66054e-6;
            pmp->Y_la[j] = ln_to_lg * ( Muj - pmp->G0[j]
                           + Dsur - 1. + 1./(1.+Dsur) - DsurT + DsurT/(1+DsurT) );
            break;
*/
         default:
            break; /* error in DC class code */
        }
       }
       dtp->mu_a[ii*dtp->nM+j] = mua;    // dtp->mu_o[ii*dtp->nM+j] = muo;
       dtp->mu_o[ii*dtp->nM+j] = muo;
    }
  }
  Calc_mua_n_stat( eState );
  Calc_muo_n_stat( eState );
}

// Calculation of average and standard deviation
#define d(i,j) (*(DataTable+(j)+(i)*M))
double
TDualTh::ColumnAverage( double *DataTable, short N, short M, short ColInd  )
{
   short ir, jc;
   double Average = 0.0;

   jc = ColInd;
   if( jc < 0 || jc >= M || N <= 0 )
     return 0.0;  // column index or number of rows is invalid

   for(ir=0; ir<N; ir++)
      Average += d(ir,jc)/(double)N;

   return Average;
}

double
TDualTh::ColumnStdev( double *DataTable, double ColAvg, short N, short M,
   short ColInd )
{
   short ir, jc;
   double StDev, Col2Avg=0.0;

   jc = ColInd;
   if( jc < 0 || jc >= M || N <= 0 )
     return 0.0;  // column index or number of rows is invalid

// calculate average of squares
   for(ir=0; ir<N; ir++)
      Col2Avg += d(ir,jc)*d(ir,jc)/(double)N;

// calculate StDev
   StDev = sqrt( Col2Avg - ColAvg*ColAvg );

   return StDev;
}

#undef d

// Calculation of statistics over muo_n columns
void
TDualTh::Calc_muo_n_stat( char /*eState*/ )
{
   short k;
   for( k=0; k< dtp->nM; k++ )
   {
      dtp->avsd_o[k] = ColumnAverage( dtp->mu_o, dtp->nQ, dtp->nM, k  );
      dtp->avsd_o[k+dtp->nM] = ColumnStdev( dtp->mu_o, dtp->avsd_o[k], dtp->nQ,
                     dtp->nM, k  );
   }
}

// Calculation of statistics over mua_n columns
void
TDualTh::Calc_mua_n_stat( char /*eState*/ )
{
   short k;
   for( k=0; k< dtp->nM; k++ )
   {
      dtp->avsd_a[k] = ColumnAverage( dtp->mu_a, dtp->nQ, dtp->nM, k  );
      dtp->avsd_a[k+dtp->nM] = ColumnStdev( dtp->mu_a, dtp->avsd_a[k], dtp->nQ,
                     dtp->nM, k  );
   }
}

// Calculation of mixing energies (not partial)
void
TDualTh::Calc_gmix_n( char ModE, char /*StP*/ ) // calculation of mixing energies
{
  double Gmixt, Gmix, Gmech, Gid, Gex=0, chi, RT, P;
  short j, ii;

  if( !( ModE == DT_MODE_G || ModE == DT_MODE_M || ModE == DT_MODE_A ) )
     return;

  if( ModE == DT_MODE_M )
  {                    //  Forward calculation of activity coefficients
     Calc_gam_forward( dtp->PvGam, dtp->PsIPf, 'A' );  // take apriori params
  }
  else if( ModE == DT_MODE_A )
  {                   //  Clearing gamma values?
     for(ii=0; ii<dtp->nQ; ii++)
       for( j=0; j<dtp->nM; j++)
          dtp->gam_n[ii*dtp->nM+j] = 1.0;
  }
  for( ii=0; ii<dtp->nQ; ii++)
  {
    dtp->q = ii;
    get_RT_P( ii, RT, P );

    Gmix = Gmixt = Gmech = Gid = 0.0;
    if( ModE == DT_MODE_G )
    {
       for( j=0; j<dtp->nM; j++)
       {  // Calculation of Gmix for q-th experiment
          chi = dtp->chi[ii*dtp->nM+j];  // mole fraction
          if(chi <= 1e-9) chi = 1e-9;
          Gmixt += dtp->mu_b[ii*dtp->nM+j] * chi;   // incr Gibbs energy of mixture
          Gmech += dtp->mu_o[ii*dtp->nM+j] * chi;   // incr Gibbs energy of mechanic mixture
          Gid += RT * chi * log( chi );             // incr Gibbs energy of Raoult ideal mixing
       }
       //  Calculation of Gmix and Gex for q-th experiment
       Gmix = Gmixt - Gmech; // total Gibbs energy of mixing
       Gex = Gmix - Gid;     // excess Gibbs energy of mixing
    }
    else if( ModE == DT_MODE_M || ModE == DT_MODE_A )
    {  // Calculation of g0 or g* values
       for( j=0; j<dtp->nM; j++)
       {  // Calculation of Gmix for q-th experiment
          chi = dtp->chi[ii*dtp->nM+j];  // mole fraction
          if(chi <= 1e-9) chi = 1e-9;
          Gmixt += dtp->mu_b[ii*dtp->nM+j] * chi;   // incr Gibbs energy of mixture
          Gid += RT * chi * log( chi );             // incr Gibbs energy of Raoult ideal mixing
          Gex += RT * chi * log( dtp->gam_n[ii*dtp->nM+j] ); // incr excess Gibbs energy
       }
       //  Calculation of Gmix and Gmech for q-th experiment
       Gmix = Gex + Gid; // Gibbs energy of mixing
       Gmech = Gmixt - Gmix; // Gibbs energy of mechanical mixture
    }
    // putting results into gmx_n[nQ][5] table
    dtp->gmx_n[ii][0] = fabs(Gex)<1e-6? 0.: Gex;   // excess Gibbs energy of mixing
    dtp->gmx_n[ii][1] = fabs(Gid)<1e-6? 0.: Gid;   // Gibbs energy of Raoult ideal mixing
    dtp->gmx_n[ii][2] = fabs(Gmix)<1e-6?0.: Gmix; // total Gibbs energy of mixing
    dtp->gmx_n[ii][3] = Gmech;    // Gibbs energy of mechanical mixture
    dtp->gmx_n[ii][4] = Gmixt;    // integral Gibbs energy of the mixture
  }  // end ii
}

// Calculation of statistics over Wg (gamma interaction parameters)
void
TDualTh::Calc_alp_n_stat( char /*eState*/ )
{
   short k;

   for( k=0; k< dtp->nP; k++ )
   {
      dtp->avsd_w[k] = ColumnAverage( dtp->Wa, dtp->nQ, dtp->nP, k  );
      dtp->avsd_w[dtp->nP+k] = ColumnStdev( dtp->Wa, dtp->avsd_w[k], dtp->nQ,
                     dtp->nP, k  );
   }
}

void
TDualTh::Calc_gam_n( char eState )
{  // calculate gamma and interaction parameters using DualTh methods
 short ii, j;
 double muoi, gam=1., Dsur, RT, P, lnFmol=4.016535;
 double Gex, /*Gmix,Gmech,Gid,Gmixt,*/ chi, chiPr, Wg, lnGam1, lnGam2,
        alp0, alp1, chi1, chi2, /*Gex_,*/ W12, W21;

// dt_initiate( false );
   Dsur = dtp->WmCb - 1.;
   if( Dsur < 0. )
     Dsur = 0.;

 for( ii=0; ii<dtp->nQ; ii++)
 {
    dtp->q = ii;
    get_RT_P( ii, RT, P );

  // Calculation of activity coefficients
    if( eState == DT_STATE_S )
    {  // Stoichiometric saturation - applies to SS end-members only!
      chiPr = 1.;
      for( j=0; j<dtp->nM; j++)
         chiPr *= dtp->chi[ii*dtp->nM+j];  // mole fraction product
//  Retrieval of Gmix and Gex for q-th experiment
//      Gmixt = dtp->gmx_n[ii][4];   //  Gibbs energy of mixture
//      Gmech = dtp->gmx_n[ii][3];   //  Gibbs energy of mechanic mixture
//      Gmix = dtp->gmx_n[ii][2];    //  total Gibbs energy of mixing
//      Gid = dtp->gmx_n[ii][1];     //  Gibbs energy of Raoult ideal mixing
      Gex = dtp->gmx_n[ii][0];     // excess Gibbs energy of mixing

// Interaction parameter (regular binary Guggenheim only)!
      if( dtp->nM == 2 && dtp->nP )
      {
          Wg = Gex / chiPr;
          alp0 = Wg/RT;
          switch( dtp->PsIPf ) // put it into Wa column
          {
             case DT_IPF_R:  // Redlich-Kister
                            if(dtp->PsIPu == DT_IPU_N )
                            {
                               dtp->Wa[ii*dtp->nP] = alp0;
                               break;
                            }
                            [[fallthrough]];
             case DT_IPF_G:  // Guggenheim
             case DT_IPF_T:  // Thompson-Waldbaum
                            if(dtp->PsIPu == DT_IPU_J )
                               dtp->Wa[ii*dtp->nP] = Wg;
                            else if( dtp->PsIPu == DT_IPU_K )
                               dtp->Wa[ii*dtp->nP] = Wg*0.001;
                            else if( dtp->PsIPu == DT_IPU_N )
                               dtp->Wa[ii*dtp->nP] = alp0;
             default: break;
          }

// calculate activity coeffs of regular binary Guggenheim model
          lnGam1 = Wg/RT * dtp->chi[ii*dtp->nM+1] * dtp->chi[ii*dtp->nM+1];
          lnGam2 = Wg/RT * dtp->chi[ii*dtp->nM] * dtp->chi[ii*dtp->nM];

// truncating (for numerics) - error messages to be added
          if( lnGam1 < -15. )
              lnGam1 = -15;
          if( lnGam2 < -15. )
              lnGam2 = -15;
          if( lnGam1 > 15. )
              lnGam1 = 15;
          if( lnGam2 > 15. )
              lnGam2 = 15;
// putting it provisionally in gam_n cells as activity coeffs
          dtp->gam_n[ii*dtp->nM] = exp( lnGam1 );
          dtp->gam_n[ii*dtp->nM+1] = exp( lnGam2 );
      }
    }
    else { // Equilibrium - direct estimation of activity coefficients possible
      for( j=0; j<dtp->nM; j++)
      {
        dtp->jm = j;
        muoi = dtp->mu_o[ii*dtp->nM+j];
        switch(dtp->typ_n[j])
        {
          case DC_SCP_CONDEN:
              gam = exp((dtp->mu_b[ii*dtp->nM+j] - muoi)/RT);
              break;
          case DC_SOL_IDEAL:
          case DC_SOL_MINDEP:
          case DC_SOL_MAJDEP:
          case DC_SOL_MINOR:
          case DC_SOL_MAJOR:
              gam = exp((dtp->mu_b[ii*dtp->nM+j] - muoi)/RT
                   - log(dtp->chi[ii*dtp->nM+j]));
              break;
          case DC_AQ_ELECTRON:
          case DC_AQ_PROTON:
          case DC_AQ_SPECIES: case DC_AQ_SURCOMP:
              gam = exp((dtp->mu_b[ii*dtp->nM+j] - muoi)/RT + Dsur + lnFmol
                   - log(dtp->chi[ii*dtp->nM+j]));
              break;
          case DC_AQ_SOLVENT:
          case DC_AQ_SOLVCOM:
              gam = exp((dtp->mu_b[ii*dtp->nM+j] - muoi)/RT + Dsur - 1.
                    + 1. / ( 1.+Dsur ) - log(dtp->chi[ii*dtp->nM+j]));
              break;
          case DC_GAS_COMP:
          case DC_GAS_H2O:
          case DC_GAS_CO2:   /* gases */
          case DC_GAS_H2:
          case DC_GAS_N2:
              gam = exp((dtp->mu_b[ii*dtp->nM+j] - muoi)/RT
                   - log(dtp->chi[ii*dtp->nM+j]) + log(P));
              break;
/*
// adsorption:
        case DC_SUR_GROUP:
            pmp->Y_la[j] = ln_to_lg * ( Muj - pmp->G0[j]
                                         + Dsur + DsurT/( 1.0+DsurT ) + lnFmol );
            break;
        case DC_SSC_A0:
        case DC_SSC_A1:
        case DC_SSC_A2:
        case DC_SSC_A3:
        case DC_SSC_A4:
        case DC_WSC_A0:
        case DC_WSC_A1:
        case DC_WSC_A2:
        case DC_WSC_A3:
        case DC_WSC_A4:  // case DC_SUR_GROUP:
        case DC_SUR_COMPLEX:
        case DC_SUR_IPAIR:
        case DC_IESC_A:
        case DC_IEWC_B:
            DsurT = MMC * pmp->Aalp[k] * pa.p.DNS*1.66054e-6;
            pmp->Y_la[j] = ln_to_lg * ( Muj - pmp->G0[j]
                                         + Dsur + DsurT/( 1.0+DsurT ) + lnFmol );
            break; // Coulombic term to be considered !!!!!!!!!!
        case DC_PEL_CARRIER:
        case DC_SUR_MINAL:
        case DC_SUR_CARRIER: // sorbent
            DsurT = MMC * pmp->Aalp[k] * pa.p.DNS*1.66054e-6;
            pmp->Y_la[j] = ln_to_lg * ( Muj - pmp->G0[j]
                           + Dsur - 1. + 1./(1.+Dsur) - DsurT + DsurT/(1+DsurT) );
            break;
*/
         default:
            break; /* error in DC class code */
        }
        dtp->gam_n[ii*dtp->nM+j] = gam;
      } // j

      // Estimation of interaction parameters from activity coefficients
      Gex = 0.;
      chiPr = 1.;
      for( j=0; j<dtp->nM; j++)
      {
         chi = dtp->chi[ii*dtp->nM+j];
         chiPr *= chi;  // mole fraction product
         Gex += chi * RT * log( dtp->gam_n[ii*dtp->nM+j] ); // excess Gibbs energy
      }
     // Gex_ = dtp->gmx_n[ii][0];  // excess Gibbs energy for comparison/debugging
// Direct calculation of interaction parameter (binary solution only)!
      if( dtp->nM == 2 )
      {
        if( dtp->nP == 1 )
        {  //  Regular Guggenheim/Margules model
           Wg = Gex / chiPr;    // single parameter
           alp0 = Wg/RT;
           switch( dtp->PsIPf ) // put it into Wa column
           {
             case DT_IPF_R:  // Redlich-Kister
                            if(dtp->PsIPu == DT_IPU_N )
                            {
                               dtp->Wa[ii] = alp0;
                               break;
                            }
                            [[fallthrough]];
             case DT_IPF_G:  // Guggenheim
             case DT_IPF_T:  // Thompson-Waldbaum
                            if(dtp->PsIPu == DT_IPU_J )
                               dtp->Wa[ii] = Wg;
                            else if( dtp->PsIPu == DT_IPU_K )
                               dtp->Wa[ii] = Wg*0.001;
                            else if( dtp->PsIPu == DT_IPU_N )
                               dtp->Wa[ii] = alp0;
             default: break;
           }
        }
        else if( dtp->nP == 2 )
        {  // Subregular Guggenheim parameters directly from activity coeffs
           //double w12, w21, a0, a1;
           lnGam1 = log( dtp->gam_n[ii*dtp->nM] );
           lnGam2 = log( dtp->gam_n[ii*dtp->nM+1] );
           chi1 = dtp->chi[ii*dtp->nM];
           chi2 = dtp->chi[ii*dtp->nM+1];
           alp0 = 0.5*( lnGam1/chi2/chi2 * (3.*chi2-chi1)
                      + lnGam2/chi1/chi1 * (3.*chi1-chi2));
           alp1 = 0.5*( lnGam1/chi2/chi2 - lnGam2/chi1/chi1 );
           //w12 = 2.*lnGam2/chi1 +lnGam1/chi2/chi2 *(chi2-chi1);
           //w21 = 2.*lnGam1/chi2 +lnGam2/chi1/chi1 *(chi1-chi2);
           W12 = RT*(alp0-alp1);
           W21 = RT*(alp0+alp1);
           //a0 = 0.5*(w12+w21);
           //a1 = 0.5*(w12-w21);

           switch( dtp->PsIPf ) // put results into Wa columns
           {
             case DT_IPF_R:  // Redlich-Kister
                            if(dtp->PsIPu == DT_IPU_N )
                            {
                               dtp->Wa[ii] = alp0;
                               dtp->Wa[ii+1] = alp1;
                               break;
                            }
                            [[fallthrough]];
             case DT_IPF_G:  // Guggenheim
                            if(dtp->PsIPu == DT_IPU_J )
                            {
                               dtp->Wa[ii] = alp0*RT;
                               dtp->Wa[ii+1] = alp1*RT;
                            }
                            else if( dtp->PsIPu == DT_IPU_K )
                            {
                               dtp->Wa[ii] = alp0*RT*0.001;
                               dtp->Wa[ii+1] = alp0*RT*0.001;
                            }
                            else if( dtp->PsIPu == DT_IPU_N )
                            {
                               dtp->Wa[ii] = alp0;
                               dtp->Wa[ii+1] = alp1;
                            }
                            break;
             case DT_IPF_T:  // Thompson-Waldbaum
                            if(dtp->PsIPu == DT_IPU_J )
                            {
                               dtp->Wa[ii] = W12;
                               dtp->Wa[ii+1] = W21;
                            }
                            else if( dtp->PsIPu == DT_IPU_K )
                            {
                               dtp->Wa[ii] = W12*0.001;
                               dtp->Wa[ii+1] = W21*0.001;
                            }
                            else if( dtp->PsIPu == DT_IPU_N )
                            {
                               dtp->Wa[ii] = W12/RT;
                               dtp->Wa[ii+1] = W21/RT;
                            }
             default: break;
          }
        }
      }
    } // end else
  } //  end for ii

  Calc_alp_n_stat( eState ); // statistics over interaction parameters
}

void
TDualTh::Calc_act_n( char eState )
{ // calculate DualTh activities

 short ii, j;
 double muoi, lna = 1., Dsur, RT, P, lnP, lnFmol=4.016535;

// dt_initiate( false );
  Dsur = dtp->WmCb - 1.;
  if( Dsur < 0. )
    Dsur = 0.;

 for( ii=0; ii<dtp->nQ; ii++)
 {
    dtp->q = ii;
    get_RT_P( ii, RT, P );
    lnP = log( P );
  // zero off a cell in mu_n
    for( j=0; j<dtp->nM; j++)
    {
       dtp->jm = j;
       muoi = dtp->mu_o[ii*dtp->nM+j];
       if( eState == DT_STATE_S )
        switch(dtp->typ_n[j])  // Stoich. saturation - to be written
        {          // for now, the same calculation as at equilibrium
          default:
          case DC_SCP_CONDEN:
                    lna = (dtp->mu_b[ii*dtp->nM+j] - muoi)/RT;
                    break;
               case DC_AQ_ELECTRON: case DC_AQ_PROTON:  case DC_AQ_SPECIES: case DC_AQ_SURCOMP:
                    lna = (dtp->mu_b[ii*dtp->nM+j] - muoi)/RT
                            + Dsur + lnFmol;
                    break;
               case DC_AQ_SOLVENT: case DC_AQ_SOLVCOM:
                    lna = (dtp->mu_b[ii*dtp->nM+j] - muoi)/RT
                            + Dsur - 1. + 1. / ( 1.+Dsur );
                    break;
               case DC_GAS_COMP: case DC_GAS_H2O:  case DC_GAS_CO2:
               case DC_GAS_H2: case DC_GAS_N2:
                    lna = ( dtp->mu_b[ii*dtp->nM+j] - muoi )/RT
                            + lnP; // This makes partial fugacity (in activity scale)
                    break;
               case DC_SOL_IDEAL: case DC_SOL_MINOR: case DC_SOL_MAJOR: case DC_SOL_MINDEP: case DC_SOL_MAJDEP:
//                    lna = (dtp->mu_b[ii*dtp->nM+j] - muoi)/RT;
               // Only direct calculation is possible in this case
                    lna = log( dtp->chi[ii*dtp->nM+j] ) +
                          log( dtp->gam_n[ii*dtp->nM+j] );
                    break;
        } // case
       else
        switch(dtp->typ_n[j])  // Equilibrium
        {
          default:
          case DC_SCP_CONDEN: // actually, this is the saturation index
                    lna = (dtp->mu_b[ii*dtp->nM+j] - muoi)/RT;
                    break;
               case DC_AQ_ELECTRON: case DC_AQ_PROTON:  case DC_AQ_SPECIES: case DC_AQ_SURCOMP:
                    lna = (dtp->mu_b[ii*dtp->nM+j] - muoi)/RT
                            + Dsur + lnFmol;
                    break;
               case DC_AQ_SOLVENT: case DC_AQ_SOLVCOM:
                    lna = (dtp->mu_b[ii*dtp->nM+j] - muoi)/RT
                            + Dsur - 1. + 1. / ( 1.+Dsur );
                    break;
               case DC_GAS_COMP: case DC_GAS_H2O:  case DC_GAS_CO2:
               case DC_GAS_H2: case DC_GAS_N2:
                    lna = (dtp->mu_b[ii*dtp->nM+j] - muoi)/RT
                            + lnP; // Converts to activity scale
                    break;
               case DC_SOL_IDEAL: case DC_SOL_MINOR: case DC_SOL_MAJOR: case DC_SOL_MINDEP: case DC_SOL_MAJDEP:
                    lna = (dtp->mu_b[ii*dtp->nM+j] - muoi)/RT;
                    break;
/*               case DC_SUR_GROUP:
                    DsurT = MMC * pmp->Aalp[k] * pa.p.DNS*1.66054e-6;
                    pmp->Y_la[j] = ln_to_lg * ( Muj - pmp->G0[j]
                                         + Dsur + DsurT/( 1.0+DsurT ) + lnFmol );
                    break;
               case DC_SSC_A0: case DC_SSC_A1: case DC_SSC_A2: case DC_SSC_A3:
               case DC_SSC_A4: case DC_WSC_A0: case DC_WSC_A1: case DC_WSC_A2:
               case DC_WSC_A3: case DC_WSC_A4: case DC_SUR_COMPLEX:
               case DC_SUR_IPAIR: case DC_IESC_A: case DC_IEWC_B:
                    DsurT = MMC * pmp->Aalp[k] * pa.p.DNS*1.66054e-6;
                    pmp->Y_la[j] = ln_to_lg * ( Muj - pmp->G0[j]
                                         + Dsur + DsurT/( 1.0+DsurT ) + lnFmol );
                    break; // Coulombic term to be considered !!!!!!!!!!
               case DC_PEL_CARRIER: case DC_SUR_MINAL: case DC_SUR_CARRIER: // sorbent
                    DsurT = MMC * pmp->Aalp[k] * pa.p.DNS*1.66054e-6;
                    pmp->Y_la[j] = ln_to_lg * ( Muj - pmp->G0[j]
                       + Dsur - 1. + 1./(1.+Dsur) - DsurT + DsurT/(1+DsurT) );
                    break;
*/       } // case

         dtp->act_n[ii*dtp->nM+j] = exp( lna );
         if( dtp->PsMode == DT_MODE_G )
         {  // Computing g* values
            dtp->mu_a[ii*dtp->nM+j] = dtp->mu_o[ii*dtp->nM+j]
               + RT * log( dtp->gam_n[ii*dtp->nM+j] );
         }
      } // j
   }  // ii
   if( dtp->PsMode == DT_MODE_G )
     Calc_mua_n_stat( eState );   // Adding statistics
}


// --------------------------------------------------------------------------

// Forward calculation of activity coefficients of end members
void
TDualTh::Calc_gam_forward( char PvGam, char PsIPf, char WhereIPar )
{
   double alp[40], chi[20], gam[20], ScaleF = 1., RT, P;
   short q, j, k, RetC;

   memset( alp, 0, sizeof(double)*40 );
   memset( chi, 0, sizeof(double)*20 );
   memset( gam, 0, sizeof(double)*20 );

   if( PvGam != S_OFF )
   {  // Equations set in the script
      CalcEquat();
      return;
   }
   for( k=0; k<dtp->nP; k++ )
   {                             // Getting parameters (re-scaling, if needed)
       if( WhereIPar == 'F' )    // Fitted parameters
           alp[k] = dtp->Wa[k];
       else alp[k] = dtp->Wa_ap[k]; // A priori parameters
   }
   for( q=0; q<dtp->nQ; q++ )
   {
     if(PsIPf != DT_IPF_I)
     {
       get_RT_P( q, RT, P );  // Getting value of RT
       ScaleF = 1.;
       switch( dtp->PsIPu )  // Analyzing units and setting up the the scale factor
       {                     // such that parameters are dimensionless
          case DT_IPU_K:   ScaleF *= 1000.; [[fallthrough]];
          case DT_IPU_J:   ScaleF /= RT;
          case DT_IPU_N:
          default:         break;
       }

       for( j=0; j<dtp->nM; j++ )  // Getting mole fractions
         chi[j] = dtp->chi[dtp->nM*q+j];

       switch(PsIPf)
       {
         case DT_IPF_R:  // Redlich-Kister
             RetC = Guggenheim( gam, chi, alp, dtp->nM, dtp->nP, ScaleF );
                          break;
         case DT_IPF_G:  // Guggenheim
             RetC = Guggenheim( gam, chi, alp, dtp->nM, dtp->nP, ScaleF );
                          break;
         case DT_IPF_T:  // Thompson-Waldbaum
             RetC = TWMargules( gam, chi, alp, dtp->nM, dtp->nP, ScaleF );
                          break;
         case DT_IPF_M:  // Margules original
             RetC = MargulesO( gam, chi, alp, dtp->nM, dtp->nP, ScaleF );
                          break;
         case DT_IPF_V:  // Van Laar
             RetC = VanLaar( gam, chi, alp, dtp->nM, dtp->nP, ScaleF );
                          break;
         case DT_IPF_B:  // Bale-Pelton: Solvent has index 0
             RetC = BalePelton( gam, chi, alp, dtp->nM, dtp->nP, ScaleF, 0 );
                          break;
         case DT_IPF_D:  // Darken's Quadratic
             RetC = DarkenQuad( gam, chi, alp, dtp->nM, dtp->nP, ScaleF, 0 );
                          break;
         default:        break; // Other
       }
       if( RetC )
       {  // Error messages
         ;
       }
       for( j=0; j<dtp->nM; j++ )  // Getting back activity coeffs
          dtp->gam_n[dtp->nM*q+j] = ROUND_EXP( gam[j], 5 );
     }
     else  // ideal Raoult model
     {
       for( j=0; j<dtp->nM; j++ )  // Setting unity activity coeffs
          dtp->gam_n[dtp->nM*q+j] = 1.0;
     }
   } // end q
   return;
}

// Forward calculation of end-member activity coefficients in various models
// Uses mole fractions x[] and interaction coefficients alp. The latter must be
// ordered corresponding to mole fractions x[], and will be first converted to
// dimensionless scale using cFactor. Then Gam is calculated whenever possible.
// Returns 0 if success and non-zero error code if error has occurred).

// Guggenheim (scaleF = 1/RT) and Redlich-Kister ( scaleF = 1) models
short
TDualTh::Guggenheim( double Gam[], const double x[], const double alp[],
         const short nM, const short nP, const double scaleF )
{
    double a0, a1, a2;
    double gam0=1., gam1=1., lnGam0=0., lnGam1=0.;
    //short i;

    if( nM != 2 || nP > 3 ) // Binary only and up to 3 parameters
       return -1;  // wrong dimensions

    a0 = alp[0]*scaleF;
    a1 = alp[1]*scaleF;
    a2 = alp[2]*scaleF;
    if( a0 > 15. )
        a0 = 15.;
    if( a0 < -15. )
        a0 = -15.;
    if( a1 > 15. )
        a1 = 15.;
    if( a1 < -15. )
        a1 = -15.;
    if( a2 > 15. )
        a2 = 15.;
    if( a2 < -15. )
        a2 = -15.;

    lnGam0 = x[1]*x[1]*(a0+a1*(3.*x[0]-x[1])+a2*(x[0]-x[1])*(5.*x[0]-x[1]));
    lnGam1 = x[0]*x[0]*(a0-a1*(3.*x[1]-x[0])+a2*(x[1]-x[0])*(5.*x[1]-x[0]));
/*
    lnGam0 = (a0+3.*a1+5.*a2)*x[1]*x[1] - 4.*(a1+4.*a2)*x[1]*x[1]*x[1]
                   + 12.*a2*x[1]*x[1]*x[1]*x[1];
    lnGam1 = (a0-3.*a1+5.*a2)*x[0]*x[0] + 4.*(a1-4.*a2)*x[0]*x[0]*x[0]
                   + 12.*a2*x[0]*x[0]*x[0]*x[0];
*/
    gam0 = exp( lnGam0 );
    gam1 = exp( lnGam1 );
    Gam[0] = gam0;
    Gam[1] = gam1;
    return 0;
}

// Margules - scaleF must normalize interaction parameters
short
TDualTh::TWMargules( double Gam[], const double x[], const double W[],
         const short nM, const short nP, const double scaleF )
{
    double w[10];
    double /*gam[4],*/ lnGam[4];
    short i/*, j*/;

    if( nM > 4 || nP > 10 ) // up to ternary regular
       return -1;  // wrong dimensions
    // symmetric for multicomponent; asymmetric for binary

    // Binary symmetric or asymmetric case
    if( nM == 2 && nP == 2 )
    {
       for( i=0; i<2; i++ ) // cleanup and coeff conversion
         w[i] = i<nP? W[i]*scaleF: 0.0;
       lnGam[0] = (w[0] + 2.*(w[1]-w[0])*x[0])*x[1]*x[1];
       lnGam[1] = (w[1] + 2.*(w[0]-w[1])*x[1])*x[0]*x[0];
       Gam[0] = exp( lnGam[0] );
       Gam[1] = exp( lnGam[1] );
       return 0;
    }

    // Symmetric up to quaternary, only with binary IP
    for( i=0; i<10; i++ ) // cleanup and coeff conversion
       w[i] = i<nP? W[i]*scaleF: 0.0;

    if( nM == 3 && nP >= 3 )  // Anderson&Crerar93 p.395
    {              // w12 = w[0]; w23 = w[1]; w13 = w[2]
       lnGam[0] = w[0]*x[1]*x[1]+ w[2]*x[2]*x[2]+ (w[0]+w[2]-w[1])*x[1]*x[2];
       lnGam[1] = w[1]*x[2]*x[2]+ w[0]*x[0]*x[0]+ (w[0]+w[1]-w[2])*x[2]*x[0];
       lnGam[2] = w[1]*x[1]*x[1]+ w[2]*x[0]*x[0]+ (w[1]+w[2]-w[0])*x[0]*x[1];
       Gam[0] = exp( lnGam[0] );
       Gam[1] = exp( lnGam[1] );
       Gam[2] = exp( lnGam[2] );
    }

    if( nM == 4 && nP >= 6 )  // Anderson&Crerar93 p.395
    {  // w12 = ac[0]; w13 = ac[1]; w14 = ac[2]; w23 = ac[3]; w24 = ac[4]; w34 = ac[5]
        lnGam[0] = w[0]*x[1]*x[1] + w[1]*x[2]*x[2] + w[2]*x[3]*x[3] +
           (w[0]+w[1]-w[3])*x[1]*x[2] + (w[0]+w[2]-w[4])*x[1]*x[3]
              + (w[1]+w[2]-w[5])*x[2]*x[3];
        lnGam[1] = w[3]*x[2]*x[2] + w[4]*x[3]*x[3] + w[0]*x[0]*x[0] +
           (w[0]+w[1]-w[3])*x[1]*x[2] + (w[0]+w[2]-w[4])*x[1]*x[3]
              + (w[1]+w[2]-w[5])*x[2]*x[3];
        lnGam[2] = w[0]*x[1]*x[1] + w[1]*x[2]*x[2] + w[2]*x[3]*x[3] +
           (w[0]+w[1]-w[3])*x[1]*x[2] + (w[0]+w[2]-w[4])*x[1]*x[3]
              + (w[1]+w[2]-w[5])*x[2]*x[3];
        lnGam[3] = w[0]*x[1]*x[1] + w[1]*x[2]*x[2] + w[2]*x[3]*x[3] +
           (w[0]+w[1]-w[3])*x[1]*x[2] + (w[0]+w[2]-w[4])*x[1]*x[3]
              + (w[1]+w[2]-w[5])*x[2]*x[3];
 // check permutation
       Gam[0] = exp( lnGam[0] );
       Gam[1] = exp( lnGam[1] );
       Gam[2] = exp( lnGam[2] );
       Gam[3] = exp( lnGam[3] );
    }
    return 0;
}

// Original Margules binary model - to be implemented
short
TDualTh::MargulesO( double Gam[], const double /*x*/[], const double /*W*/[],
         const short nM, const short /*nP*/, const double /*scaleF*/ )
{
    short j;
    // binary only - yet to be written
    for( j=0; j<nM; j++ )
      Gam[j] = 1.;

    return 1;
}

// Van Laar multicomponent formalism - to be implemented
short
TDualTh::VanLaar( double Gam[], const double /*x*/[], const double /*alp*/[],
         const short nM, const short /*nP*/, const double /*scaleF*/ )
{
    short j;
    // binary only - yet to be written
    for( j=0; j<nM; j++ )
      Gam[j] = 1.;

    return 1;
}

// Bale & Pelton 1990 Dilute SS Formalism - to be implemented
short
TDualTh::BalePelton( double Gam[], const double /*x*/[], const double /*eps*/[],
         const short nM, const short /*nP*/, const double /*scaleF*/, const short /*SolvX*/ )
{
    short j;
    // binary ???  multicomp. with e' only
    for( j=0; j<nM; j++ )
      Gam[j] = 1.;

    return 1;
}

// Darken's quadratic formalism - to be implemented
short
TDualTh::DarkenQuad( double Gam[], const double /*x*/[], const double /*eps*/[],
         const short nM, const short /*nP*/, const double /*scaleF*/, const short /*SolvX*/ )
{
    short j;
    // binary ???  only
    for( j=0; j<nM; j++ )
      Gam[j] = 1.;

    return 1;
}

// ------------------- End of m_dualthc.cpp --------------------------
