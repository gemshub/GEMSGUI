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


// setup data before analyse
void TDualTh::Init_Analyse()
{
   dt_text_analyze();
   // put data to Bn
   Bn_Calc();

}

// analyse calculated equlibriums
void TDualTh::Analyse( )
{
 // only as examle
  if( dtp->PvChi == S_ON )
   CalcEquat( A_CHI );
  if( dtp->PvGam == S_ON )
   CalcEquat( A_GAM );

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
   if( dtp->nK <=0 )
   {
      i = false;
      dtp->nK = 1;
   }
   if( dtp->PvSd == S_ON && dtp->Nsd <=0 )
     dtp->Nsd = 1;

   if( dtp->La_b <=0 )
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
  dtp->aStat = UNSP_AS_INDEF;

  dtp->cT = dtp->Td[START_];
  dtp->cP = dtp->Pd[START_];
  dtp->cV = dtp->Vd[START_];
  dtp->c_tm = dtp->tmd[START_];
  dtp->c_NV = dtp->NVd[START_];

  dtp->q = 0;
  dtp->i = 0;
  dtp->jm = 0;

  if( mode )
  {
    int ii;
    vstr tbuf(100);

    dtp->gStat = UNSP_GS_INDEF;

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

    for( ii=0; ii<dtp->nK; ii++)
    {
     dtp->typ_n[ii] = 'I';
     dtp->AUcln[ii] = 'M';
     strncpy( dtp->for_n[ii], "H2O", 4 );
     sprintf( tbuf, "Endmember%d", ii );
     strncpy( dtp->nam_n[ii], tbuf, MAXIDNAME );
    }

    for( ii=0; ii<dtp->La_b; ii++)
    {
     dtp->AUclb[ii] = 'g';
     strncpy( dtp->for_b[ii], "H2O", 4 );
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
  dtp->Asiz = siz_;
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

// building Ub matrix
void TDualTh::build_Ub()
{
 short i, ii;

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
       TProfil::pm->syp->B[i] = dtp->Bb[ dtp->q*dtp->Nb+i ];
       TProfil::pm->syp->BI[i] = dtp->Bb[ dtp->q*dtp->Nb+i ];
     }
 // calculate EqStat record (Thermodynamic&Equlibria)
   //  TProfil::pm->pmp->pTPD = 0;
     calc_eqstat();

 // set zeros for result
    for( i=0; i<dtp->Nb; i++)
       dtp->Ub[dtp->q*dtp->Nb+i] = 0.;
 // copy calculated data from multy
    for( i=0; i<TProfil::pm->pmp->N; i++)
       dtp->Ub[ dtp->q*dtp->Nb + (TProfil::pm->pmp->mui[i])]
           = TProfil::pm->pmp->U[i];

    dt_next();
  }
}

// Translate, analyze and unpack equations of the process
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

// make An from dtp->for_n
       make_A( dtp->nK, dtp->for_n );

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


    for( j=0; j < dtp->nK; j++ )
    {
         A = dtp->An + j * dtp->Nb;
         if( !dtp->CAn[ii*dtp->nK + j] ||
            IsFloatEmpty( dtp->CAn[ii*dtp->nK + j] ))
                    continue;
         DCmw = 0.;
         for( i=0; i<dtp->Nb; i++ )
         // calculation of molar mass
             DCmw += A[i]* ICw[i];
         Xincr = TCompos::pm->Reduce_Conc( dtp->AUcln[j],
                dtp->CAn[ii*dtp->nK + j],
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


// ------------------- End of m_dualthc.cpp --------------------------
