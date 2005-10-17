//-------------------------------------------------------------------
// $Id$
//
// Implementation of TDualTh class, config and calculation functions
//
// Rewritten from C to C++ by S.Dmytriyeva
// Copyright (C) 1995-2001 S.Dmytriyeva, D.Kulik
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
//#include <stdio.h>

#include "m_dualth.h"
#include "m_syseq.h"
#include "m_icomp.h"
#include "m_compos.h"
#include "s_formula.h"
//#include "m_param.h"
//#include "v_object.h"
#include "service.h"
#include "visor.h"


// setup data before calculate EqStates
void TDualTh::Init_Generation()
{
   // put data to Bb
    Bb_Calc();
}


// setup of the data before DualTh calculations
void TDualTh::Init_Analyse()
{
   dt_text_analyze();
   // put data to Bn
   Bn_Calc();
   // for_n
   make_A( dtp->nM, dtp->for_n );
   build_mu_n();  // calculate new mu_n matrix
//   CalcMoleFractNS();
   // calculate mole fractions of end members here
}

// analyse calculated equlibria
void TDualTh::Analyse( )
{
 // only as an example by now
  if( dtp->PvChi == S_ON )
     CalcEquat( A_CHI );
  if( dtp->PvGam == S_ON )
     CalcEquat( A_GAM );
  // Calculation of DualThermo results
  Calc_gmix_n( dtp->PsMode, dtp->PsSt );   // Retrieving table with mixing energies
  if( dtp->PsMode == DT_MODE_M || dtp->PsMode == DT_MODE_A )
  {
     Calc_muo_n( dtp->PsSt );  // Estimation of standard Gibbs energies
  }
  if( dtp->PsMode == DT_MODE_G )
  {
     Calc_gam_n( dtp->PsSt );  // Estimation of int. parameters and activity coeffs
     Calc_act_n( dtp->PsSt );  // Calculation of activities
     // here call regression part, if necessary

     //
  }
  if( dtp->PsMode == DT_MODE_X )
     Calc_act_n( dtp->PsSt );
}

//set sizes of arrays
bool TDualTh::test_sizes( )
{
    bool i=true;

   // the same sizes
   dtp->Nb = TProfil::pm->mup->N;

   if( dtp->nQ <=0 )
   {
      i = false;
      dtp->nQ = 1;
   }
   if( dtp->nM <=0 )
   {
      i = false;
      dtp->nM = 2;
   }
   if( dtp->PvSd == S_ON && dtp->Nsd <=0 )
     dtp->Nsd = 1;

   if( dtp->PvAUb == S_ON && dtp->La_b <=0  )
   { i= false;
     dtp->La_b = TProfil::pm->mup->N;
   }

   if( i==false )
        vfMessage(window(), GetName(),
   "E00DTrem: Invalid dimensions in DualTh data structure!", vfErr);

    return i;
}

// set begin initalization
void TDualTh::dt_initiate( bool mode )
{
  dtp->aStat = AS_INDEF;

  dtp->cT = dtp->Td[START_];
  dtp->cP = dtp->Pd[START_];
  dtp->cV = dtp->Vd[START_];
  dtp->c_tm = dtp->tmd[START_];
  dtp->c_NV = dtp->NVd[START_];

  if( (dtp->PvTPI == S_ON) && dtp->Tdq && dtp->Pdq )
  {       // Take T and P from lists
     dtp->cT = dtp->Tdq[0];
     dtp->cP = dtp->Pdq[0];
  }
  dtp->q = 0;
  dtp->i = 0;
  dtp->jm = 0;

  if( mode )
  {
    int ii,i;
    vstr tbuf(100);

    dtp->gStat = GS_INDEF;

    dtp->Msysb = 0.;
    dtp->Vsysb = 0.;
    dtp->Mwatb = 1.;
    dtp->Maqb = 1.;
    dtp->Vaqb = 1.;

    for( ii=0; ii<dtp->Nb; ii++)
    {
     dtp->CIclb[ii] = 'g';
     dtp->CIcln[ii] = 'g';
    }

    for( ii=0; ii<dtp->nQ; ii++)
    {
     sprintf( tbuf, "Experiment%d", ii );
     strncpy( dtp->nam_b[ii], tbuf, MAXIDNAME );
    }

    for( ii=0; ii<dtp->nM; ii++)
    {
     dtp->typ_n[ii] = 'I';
//     dtp->AUcln[ii] = 'M';
//     strncpy( dtp->for_n[ii], "H2O", 4 );
     sprintf( tbuf, "Endmember%d", ii );
     strncpy( dtp->nam_n[ii], tbuf, MAXIDNAME );
     for( i=0; i<dtp->nQ; i++)
     {
        dtp->gam_n[i*dtp->nM+ii] = 1.;  // default for gamma 1.0
        dtp->chi[i*dtp->nM+ii] = 1.;  // default for chi 1.0
     }
    }

    for( ii=0; ii<dtp->La_b; ii++)
    {
     dtp->AUclb[ii] = 'g';
     strncpy( dtp->for_b[ii], "H2O", 4 );
     dtp->AUcln[ii] = 'M';
     strncpy( dtp->for_n[ii], "H2O", 4 );
    }
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
  TIArray<TFormula> aFo;
  gstring form;
  int ii;

  for( ii=0; ii<siz_; ii++ )
  {
     aFo.Add( new TFormula() );
     form = gstring( for_[ii], 0, MAXFORMUNITDT );
     form.strip();
     aFo[ii].SetFormula( form.c_str() ); // and ce_fscan
  }

  ErrorIf( dtp->Nb != TProfil::pm->mup->N, GetName(),
               "Illegal data in dtp->Nb ");

  dtp->An = (float *)aObj[ o_dtan ].Alloc( siz_, dtp->Nb, F_ );
dtp->Asiz = (short)siz_;
  memset(dtp->An, 0, sizeof(float)*(siz_*dtp->Nb) );
  for( ii=0; ii<siz_; ii++ )
     aFo[ii].Stm_line( TProfil::pm->mup->N, dtp->An+ii*TProfil::pm->mup->N,
           (char *)TProfil::pm->mup->SB, TProfil::pm->mup->Val );
  aFo.Clear();
}

// make EqStat key  && calculate records
void TDualTh::calc_eqstat()
{
    vstr buf(40);

    sprintf(buf, "%.4d", dtp->c_tm);
    memset(dtp->timep, 0, 5 );
    strncpy(dtp->timep, buf, 4 );
    sprintf(buf, "%.3d", dtp->c_NV);
    memset( dtp->NVp, 0, 4 );
    strncpy( dtp->NVp, buf, 3 );
    gcvt( dtp->cT, 6, dtp->TCp );
    gcvt( dtp->cP, 6, dtp->Pp );
    gcvt( dtp->cV, 6, dtp->Bnamep );

   rt[RT_SYSEQ].MakeKey( RT_DUALTH,  dtp->sykey, RT_DUALTH, 0, RT_DUALTH,1,
         RT_DUALTH, 2, K_IMM, dtp->timep, K_IMM, dtp->Bnamep,
         K_IMM, dtp->Pp, K_IMM, dtp->TCp, K_IMM, dtp->NVp, K_END );

   rt[RT_SYSEQ].Find(dtp->sykey);
// calc current SyStat
   TProfil::pm->CalcEqstat( false );
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

 for( ii=0; ii<dtp->nQ; ii++)
 {
   dtp->q = ii;
   pVisor->Message( window(), GetName(),
             "Generation of EqStat records\n"
                 "Please, wait...", dtp->q, dtp->nQ);
     // put Bb data to system  ( into B and bi_)
    for( i=0; i<dtp->Nb; i++)
    {
       TProfil::pm->syp->B[i] =  dtp->Bb[  dtp->q*dtp->Nb+i ];
       TProfil::pm->syp->BI[i] =  dtp->Bb[  dtp->q*dtp->Nb+i];
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

     RT = TProfil::pm->pmp->RT;
//   RT = R_CONSTANT * (Tdq[ii] + 273.15);
 // set zeros for result
    for( i=0; i<dtp->Nb; i++)
       dtp->Ub[dtp->q*dtp->Nb+i] = 0.;
 // copy calculated data from multy
    for( i=0; i<TProfil::pm->pmp->N; i++)
       dtp->Ub[ dtp->q*dtp->Nb + (TProfil::pm->pmp->mui[i])]
           = TProfil::pm->pmp->U[i] * RT;

    dt_next();      // Generate work values for the next EqStat rkey
    if( (dtp->PvTPI == S_ON) && dtp->Tdq && dtp->Pdq )
    {       // Take T and P from lists and store IS
        dtp->cT = dtp->Tdq[ii];
        dtp->cP = dtp->Pdq[ii];
        if(dtp->ISq)
           dtp->ISq[ii] = TProfil::pm->pmp->IC;
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

// Translate, analyze and unpack dual-thermo math script for mole fractions
void TDualTh::dt_text_analyze()
{
  TProfil* PRof = (TProfil*)(&aMod[RT_PARAM]);
  try
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

    try
    {
      if( dtp->PvGam != S_OFF )
      {

        PRof->ET_translate( o_dttprn, o_dtgexpr,
                           0, PRof->mup->L, 0, PRof->pmp->L );
        rpn[1].GetEquat( (char *)aObj[o_dttprn].GetPtr() );
      }
    }
    catch( TError& xcpt )
    {
        char *erscan = (char *)aObj[o_dtgexpr].GetPtr();
        vfMessage(window(), xcpt.title, xcpt.mess);
        TDualTh::pm->CheckEqText(  erscan,
  "E94MSTran: Error in translation of math script \n"
  "           for activity coeffs of non-basis DCs: " );
        Error(  GetName() , xcpt.mess.c_str() );
    }
}


//Recalc rpn structure
void
TDualTh::CalcEquat( int type_ )
{
    try
    {
       switch( type_ )
       {
         case A_CHI:  rpn[0].CalcEquat();
                      break;
         case A_GAM:  rpn[1].CalcEquat();
         default:     break;
       }
    }
    catch( TError& xcpt)
    {
        gstring str = xcpt.mess;
        str += "\n Cancel DualTh?";
        if( vfQuestion(  window(), xcpt.title, str))
            Error( GetName(), "E00PEexec: Calculations stopped by the user");
    }
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
    TIComp* aIC=(TIComp *)(&aMod[RT_ICOMP]);
    aIC->ods_link(0);
    ICw = new float[dtp->Nb];
    memset( pkey, 0, MAXRKEYLEN+9 );
    for( i=0; i<dtp->Nb; i++ )
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
          if( !dtp->CIb[ii*dtp->Nb + i] ||
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
         if( !dtp->CAb[ii*dtp->La_b + j] ||
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
          if( A[i] )
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
    TIComp* aIC=(TIComp *)(&aMod[RT_ICOMP]);
    aIC->ods_link(0);
    ICw = new float[dtp->Nb];
    memset( pkey, 0, MAXRKEYLEN+9 );
    for( i=0; i<dtp->Nb; i++ )
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
          if( !dtp->CIn[ii*dtp->Nb + i] ||
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
         if( !dtp->CAn[ii*dtp->La_b + j] ||
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
          if( A[i] )
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

void
TDualTh::Calc_muo_n( char eState )
{ // calculate muo_n DualTh
 short ii, j;
 double muo, gam/*=1.*/, Dsur, RT/* = 2479.*/, P/*=1.*/, lnFmol=4.016535;

// dt_initiate( false );
  Dsur = dtp->WmCb - 1.;
  if( Dsur < 0. )
    Dsur = 0.;

 for( ii=0; ii < dtp->nQ; ii++)
 {
    dtp->q = ii;
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

    for( j=0; j<dtp->nM; j++)
    {
       dtp->jm = j;
       gam = dtp->gam_n[ii*dtp->nM+j];
       if( dtp->PsMode == DT_MODE_A)
           gam = 1.0;
       if( eState == DT_STATE_S )
        switch(dtp->typ_n[j])  // Stoich. saturation
        {
          default:
          case DC_SCP_CONDEN:
              muo = dtp->mu_b[ii*dtp->nM+j];
              break;
          case DC_SOL_IDEAL:
          case DC_SOL_MINOR:
          case DC_SOL_MAJOR: // Check this calculation !!!!
              muo = dtp->mu_b[ii*dtp->nM+j]
                  - RT*(gam + log(dtp->chi[ii*dtp->nM+j]));
              break;
        }
       else  // Equilibrium
        switch(dtp->typ_n[j])
        {
          case DC_SCP_CONDEN:
              muo = dtp->mu_b[ii*dtp->nM+j];
              break;
          case DC_SOL_IDEAL:
          case DC_SOL_MINOR:
          case DC_SOL_MAJOR:
              muo = dtp->mu_b[ii*dtp->nM+j]
                    - RT*(gam + log(dtp->chi[ii*dtp->nM+j]));
              break;
          case DC_AQ_ELECTRON:
          case DC_AQ_PROTON:
          case DC_AQ_SPECIES:
              muo = dtp->mu_b[ii*dtp->nM+j] + RT * ( Dsur + lnFmol - gam
                    - log(dtp->chi[ii*dtp->nM+j]));
              break;
          case DC_AQ_SOLVENT:
          case DC_AQ_SOLVCOM:
              muo = dtp->mu_b[ii*dtp->nM+j] + RT * ( Dsur - 1. + 1. / ( 1.+Dsur ) - gam
                    - log(dtp->chi[ii*dtp->nM+j]));
              break;
          case DC_GAS_COMP:
          case DC_GAS_H2O:
          case DC_GAS_CO2:   /* gases */
          case DC_GAS_H2:
          case DC_GAS_N2:
              muo = dtp->mu_b[ii*dtp->nM+j]
                    - RT*(gam + log(dtp->chi[ii*dtp->nM+j])
                    - log(P) );
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
       dtp->mu_o[ii*dtp->nM+j] = muo;
    }
  }
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

// Calculation of mixing energies (not partial)
void
TDualTh::Calc_gmix_n( char ModE, char StP ) // calculation of mixing energies
{
  double Gmixt, Gmix, Gmech, Gid, Gex, chi, RT; /* P */
  short j, ii;

  if( !( ModE == DT_MODE_G || ModE == DT_MODE_M || ModE == DT_MODE_A ) )
     return;
  for( ii=0; ii<dtp->nQ; ii++)
  {
    dtp->q = ii;
    if( dtp->PvTPI == S_ON && dtp->Tdq )
    {
       RT = R_CONSTANT * (dtp->Tdq[ii] + 273.15);
//       P = dtp->Pdq[ii];
    }
    else
    {
       RT = R_CONSTANT * (dtp->Td[START_] + ii*dtp->Td[STEP_] + 273.15);
//       P = dtp->Pd[START_] + ii*dtp->Pd[STEP_];
    }

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
    {
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
    dtp->gmx_n[ii][0] = Gex;      // excess Gibbs energy of mixing
    dtp->gmx_n[ii][1] = Gid;      // Gibbs energy of Raoult ideal mixing
    dtp->gmx_n[ii][2] = Gmix;     // total Gibbs energy of mixing
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
 double Gex, Gmix, Gmech, Gid, Gmixt, chi, chiPr, Wg, lnGam1, lnGam2,
        alp0, alp1, chi1, chi2, Gex_, W12, W21;

// dt_initiate( false );
   Dsur = dtp->WmCb - 1.;
   if( Dsur < 0. )
     Dsur = 0.;

 for( ii=0; ii<dtp->nQ; ii++)
 {
    dtp->q = ii;
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
          case DC_SOL_MINOR:
          case DC_SOL_MAJOR:
              gam = exp((dtp->mu_b[ii*dtp->nM+j] - muoi)/RT
                   - log(dtp->chi[ii*dtp->nM+j]));
              break;
          case DC_AQ_ELECTRON:
          case DC_AQ_PROTON:
          case DC_AQ_SPECIES:
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
      Gex_ = dtp->gmx_n[ii][0];  // excess Gibbs energy for comparison/debugging
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
           lnGam1 = dtp->gam_n[ii*dtp->nM];
           lnGam2 = dtp->gam_n[ii*dtp->nM+1];
           chi1 = dtp->chi[ii*dtp->nM];
           chi2 = dtp->chi[ii*dtp->nM+1];
           alp0 = 0.5*( lnGam1/chi2/chi2 * (3.*chi2-chi1)
                      + lnGam2/chi1/chi1 * (3.*chi1-chi2));
           alp1 = 0.5*( lnGam1/chi2/chi2 * lnGam2/chi1/chi1 );
           W12 = RT*(alp0-alp1);
           W21 = RT*(alp0+alp1);

           switch( dtp->PsIPf ) // put results into Wa columns
           {
             case DT_IPF_R:  // Redlich-Kister
                            if(dtp->PsIPu == DT_IPU_N )
                            {
                               dtp->Wa[ii] = alp0;
                               dtp->Wa[ii+1] = alp1;
                               break;
                            }
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
  // Do we need LS regression for interaction parameters here?

  //
}


void
TDualTh::Calc_act_n( char eState )
{ // calculate activity in DualTh variant

 short ii, j;
 double muoi, activ = 1., Dsur, RT, P, lnFmol=4.016535;

// dt_initiate( false );
  Dsur = dtp->WmCb - 1.;
  if( Dsur < 0. )
    Dsur = 0.;

 for( ii=0; ii<dtp->nQ; ii++)
 {
    dtp->q = ii;
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
  // zero off a cell in mu_n
    for( j=0; j<dtp->nM; j++)
    {
       dtp->jm = j;
       muoi = dtp->mu_o[ii*dtp->nM+j];
       if( eState == DT_STATE_S )
        switch(dtp->typ_n[j])  // Stoich. saturation - to be written !
        {          // for now, the same calculation as at equilibrium
          default:
          case DC_SCP_CONDEN:
                    activ = exp( (dtp->mu_b[ii*dtp->nM+j] - muoi)/RT );
                    break;
               case DC_AQ_ELECTRON: case DC_AQ_PROTON:  case DC_AQ_SPECIES:
                    activ = exp( (dtp->mu_b[ii*dtp->nM+j] - muoi)/RT
                            + Dsur + lnFmol);
                    break;
               case DC_AQ_SOLVENT: case DC_AQ_SOLVCOM:
                    activ = exp( (dtp->mu_b[ii*dtp->nM+j] - muoi)/RT
                            + Dsur - 1. + 1. / ( 1.+Dsur ) );
                    break;
               case DC_GAS_COMP: case DC_GAS_H2O:  case DC_GAS_CO2:
               case DC_GAS_H2: case DC_GAS_N2:
                    activ = exp( (dtp->mu_b[ii*dtp->nM+j] - muoi)/RT )
                            * P; // This makes partial fugacity (in activity scale)
                    break;
               case DC_SOL_IDEAL: case DC_SOL_MINOR: case DC_SOL_MAJOR:
                    activ = exp( (dtp->mu_b[ii*dtp->nM+j] - muoi)/RT );
                    break;
        } // case
       else
        switch(dtp->typ_n[j])  // Equilibrium
        {
          default:
          case DC_SCP_CONDEN: // actually, this is the saturation index
                    activ = exp( (dtp->mu_b[ii*dtp->nM+j] - muoi)/RT );
                    break;
               case DC_AQ_ELECTRON: case DC_AQ_PROTON:  case DC_AQ_SPECIES:
                    activ = exp((dtp->mu_b[ii*dtp->nM+j] - muoi)/RT
                            + Dsur + lnFmol);
                    break;
               case DC_AQ_SOLVENT: case DC_AQ_SOLVCOM:
                    activ = exp((dtp->mu_b[ii*dtp->nM+j] - muoi)/RT
                            + Dsur - 1. + 1. / ( 1.+Dsur ) );
                    break;
               case DC_GAS_COMP: case DC_GAS_H2O:  case DC_GAS_CO2:
               case DC_GAS_H2: case DC_GAS_N2:
                    activ = exp((dtp->mu_b[ii*dtp->nM+j] - muoi)/RT )
                            * P; // Converts to activity scale
                    break;
               case DC_SOL_IDEAL: case DC_SOL_MINOR: case DC_SOL_MAJOR:
                    activ = exp((dtp->mu_b[ii*dtp->nM+j] - muoi)/RT );
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

         dtp->act_n[ii*dtp->nM+j] = activ;
      } // j
   }  // ii
}

// ------------------- End of m_dualthc.cpp --------------------------
