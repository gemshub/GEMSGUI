//-------------------------------------------------------------------
// $Id$
//
// Implementation of TMulti  load functions
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
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//
#include <stdio.h>
#include <iomanip>
#include <math.h>

#include "m_param.h"
#include "m_syseq.h"
#include "service.h"

// Setting sizes of packed MULTI arrays and scalars from GEMIPM calculation
void TMulti::setSizes()
{
    short j, Lp;
    TSysEq* STat = (TSysEq*)(&aMod[RT_SYSEQ]);

    // short
    STat->stp->N = pm.N;
    STat->stp->Fi = pm.FI1;
    STat->stp->Fis = pm.FI1s;
    STat->stp->itIPM = pm.IT;
//    STat->stp->itPar = pm.W1;  // Number of IPM-2 loops KD 29.11.01 obsolete
    STat->stp->itPar = pm.ITaia;  // Added 20.06.2008 DK
    // float
    STat->stp->V = pm.VXc;
    STat->stp->T = pm.Tc;
    STat->stp->P = pm.Pc;
    STat->stp->H = pm.HXc;
    STat->stp->S = pm.SXc;
    STat->stp->UU = pm.FX;
    STat->stp->PCI = pm.PCI;
    STat->stp->ParE = pm.FitVar[3]; // smoothing factor

    // calculatging number of DC to be packed
    for( Lp=0,j=0; j<pm.L; j++ )
        if( pm.X[j] > pm.lowPosNum )
            Lp++;
    STat->stp->L = Lp;
}

// Packed MULTI arrays into SysEq record
void TMulti::packData()
{
    short i,j;
    TSysEq* STat = (TSysEq*)(&aMod[RT_SYSEQ]);

    for( i=0,j=0; j<pm.L; j++ )
        if( pm.X[j] > pm.lowPosNum )
        {
            STat->stp->llf[i] = pm.muj[j];
            STat->stp->Y[i] = pm.X[j];
            STat->stp->lnGam[i] = pm.lnGam[j];
            i++;
        }
    for( i=0; i<pm.N; i++ )
    {
        STat->stp->nnf[i] = pm.mui[i];
        STat->stp->B[i] = pm.B[i];
        STat->stp->U[i] = pm.U[i];
    }

    for( i=0, j=0; j<pm.FIs; j++ )
        if( pm.YF[j] > 1e-18 )
        {
            STat->stp->phf[i] = pm.muk[j];
            for( int k=0; k<pm.N; k++ )
              *(STat->stp->Ba+i*pm.N+k) = *(pm.BF+j*pm.N+k);
            i++;
        }
}

// Packing MULTI arrays into SysEq record (to z_sp_conf mode)
//
void TMulti::packData( TCIntArray PHon, TCIntArray DCon )
{
    short i,j;
    TSysEq* STat = (TSysEq*)(&aMod[RT_SYSEQ]);

    for( i=0,j=0; j<pm.L; j++ )
        if( pm.X[j] > pm.lowPosNum )
        {
            STat->stp->llf[i] = (short)DCon[pm.muj[j]];
            STat->stp->Y[i] = pm.X[j];
            STat->stp->lnGam[i] = pm.lnGam[j];
            i++;
        }
    for( i=0; i<pm.N; i++ )
    {
        STat->stp->nnf[i] = pm.mui[i];
        STat->stp->B[i] = pm.B[i];
        STat->stp->U[i] = pm.U[i];
    }

    for( i=0, j=0; j<pm.FIs; j++ )
        if( pm.YF[j] > 1e-18 )
        {
            STat->stp->phf[i] = (short)PHon[pm.muk[j]];
            for( int k=0; k<pm.N; k++ )
              *(STat->stp->Ba+i*pm.N+k) = *(pm.BF+j*pm.N+k);
            i++;
        }
}

// Unpacking SysEq arrays to multi (Y to pm.Y)
//
void TMulti::unpackData()
{
    TSysEq* STat = (TSysEq*)(&aMod[RT_SYSEQ]);
    TProfil* Prf = (TProfil*)(&aMod[RT_PARAM]);

    int i, j, js, jp, is, ip;

   if( pm.pESU == 2 )   // this SysEq record has already been unpacked
   {    // pm.IT = 0;   // Debugging 12.03.2008 DK
        return;
   }

    for( is=0; is<STat->stp->N; is++ )
    {
        i = STat->stp->nnf[is];
        // looking for this IComp in MULTI
        for( j=0; j<pm.N; j++ )
            if( i != pm.mui[j] )
                continue;
            else
            {
                ip = j;
                goto FOUNDI;
            }
        pm.pNP = 1;
        //   gstring err = "No '";
        //   err += gstring(TProfil::pm->mup->SB[i], 0, IC_RKLEN);
        //   err += "' IComp in system.";
        Error( GetName(), "no such IComp in this system" );
FOUNDI:
        pm.U[ip] = STat->stp->U[is];
        if( pm.pESU != 2 ) pm.B[ip] = STat->stp->B[is];    // Added
    }

    // Cleaning  Y and lnGam vectors
    for( j=0; j<pm.L; j++ )
    {
        pm.Y[j] = 0.0;
        pm.lnGam[j] = 0.0;
    }

    for( js=0; js<STat->stp->L; js++ )
    {
        j = Prf->indDC( STat->stp->llf[js] );
        // looking for a DC in MULTI
        for( i=0; i<pm.L; i++ )
            if( j != pm.muj[i] )
                continue;
            else
            {
                jp = i;
                goto FOUND;
            }
        pm.pNP = 1;
        Error( GetName(), "No this DComp in the system" );
FOUND:
        pm.Y[jp] /* = pm.X[jp] */ = STat->stp->Y[js];
        pm.lnGam[jp] = STat->stp->lnGam[js];
    }

    // short
    pm.ITaia = STat->stp->itPar;  // Level of tinkle supressor
//    pm.pRR1 = STat->stp->itPar;  // Level of tinkle supressor
    pm.FI1 = STat->stp->Fi;
    pm.FI1s = STat->stp->Fis;
pm.IT = STat->stp->itIPM;
//    pm.IT = 0;         Debugging 12.03.2008 DK
    pm.W1 = 0; pm.K2 = 0;
    // float
    pm.FitVar[3] = STat->stp->ParE;  // Smoothing factor
    pm.FX  = STat->stp->UU;  // GX normalized
    pm.PCI = STat->stp->PCI;
    pm.VXc = STat->stp->V;
    pm.HXc = STat->stp->H;
    pm.SXc = STat->stp->S;
    // pm.IC = STat->stp->Res1;

    pm.pESU = 2;  // SysEq unpack flag set
}

// load data from TSysEq (EQstat => Multi)
void TMulti::loadData( bool newRec )
{
    // nesessary to realloc data after new system   (before function )
    // setDefData();
    if( newRec == false )
        unpackData();
}


// =================================================================
// Moved from MS_MULOAD.CPP

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Load Thermodynamic Data from MTPARM to MULTI
void TMulti::CompG0Load()
{
    long int j, jj, k, jb, je=0;
    double Go, Gg=0., Ge, Vv = 0.;

    // ->pTPD state of reload t/d data 0-all, 1 G0, Vol, 2 do not load
    if( pmp->pTPD < 1 )
    {
        pmp->T = pmp->Tc = tpp->T + C_to_K;
        pmp->TC = pmp->TCc = tpp->T;
        if( tpp->P > 1e-9 )
            pmp->P = pmp->Pc = tpp->P;
        else
        	pmp->P = pmp->Pc = 1e-9;
        pmp->FitVar[0] = TProfil::pm->pa.aqPar[0];
        pmp->RT =  R_CONSTANT * pmp->Tc; // tpp->RT; // test 07/12/2007
        pmp->FRT = F_CONSTANT/pmp->RT;
        pmp->lnP = log( pmp->P );

        pmp->denW[0] = tpp->RoW;
        pmp->denW[1] = tpp->dRdTW;
        pmp->denW[2] = tpp->d2RdT2W;
        pmp->denW[3] = tpp->dRdPW;
        pmp->denW[4] = tpp->d2RdP2W;
        pmp->denWg[0] = tpp->RoV;
        pmp->denWg[1] = tpp->dRdTV;
        pmp->denWg[2] = tpp->d2RdT2V;
        pmp->denWg[3] = tpp->dRdPV;
        pmp->denWg[4] = tpp->d2RdP2V;
        pmp->epsW[0] = tpp->EpsW;
        pmp->epsW[1] = tpp->dEdTW;
        pmp->epsW[2] = tpp->d2EdT2W;
        pmp->epsW[3] = tpp->dEdPW;
        pmp->epsW[4] = tpp->d2EdP2W;
        pmp->epsWg[0] = tpp->EpsV;
        pmp->epsWg[1] = tpp->dEdTV;
        pmp->epsWg[2] = tpp->d2EdT2V;
        pmp->epsWg[3] = tpp->dEdPV;
        pmp->epsWg[4] = tpp->d2EdP2V;

    }
    if( pmp->pTPD <= 1 )
    {
      int xVol = 0;
      if( tpp->PtvVm == S_ON && pmp->PV == VOL_CONSTR )
        xVol = getXvolume();

        for( k=0; k<pmp->FI; k++ )
        {
            jb = je;
            je += pmp->L1[k];

            for( j=jb; j<je; j++ )
            {
                jj = pmp->muj[j];
   // loading G0  - re-furbished 07.03.2008  DK
                Go = tpp->G[jj]; //  G0(T,P) value taken from MTPARM
                if( syp->Guns )  // This is used mainly in UnSpace calculations
                    Gg = syp->Guns[jj];    // User-set increment to G0 from project system
                if( syp->GEX && syp->PGEX != S_OFF )   // User-set increment to G0 from project system
                 	Ge = syp->GEX[jj];     //now Ge is integrated into pmp->G0 (since 07.03.2008) DK
  	// !!!!!!! Insert here a case that checks units of measurement for the G0 increment
                pmp->G0[j] = Cj_init_calc( Go+Gg+Ge, j, k );

                //  loading Vol
                if( tpp->PtvVm == S_ON )
                    switch( pmp->PV )
                    { // loading molar volumes of components into A matrix
                    case VOL_CONSTR:
                        if( syp->Vuns )
                           Vv = syp->Vuns[jj];
                        pmp->A[j*pmp->N+xVol] = tpp->Vm[jj]+Vv;
                    case VOL_CALC:
                    case VOL_UNDEF:
                        if( syp->Vuns )
                           Vv = syp->Vuns[jj];
                        pmp->Vol[j] = (tpp->Vm[jj]+Vv ) * 10.;
                        break;
                    }
                else pmp->Vol[j] = 0.0;
            }
        }
    }

  Alloc_internal(); // performance optimization 08/02/2007
//  cout << "G0Load  T = " << tpp-> T << " P= " << tpp->P << endl;
//  cout << " G[0] " << setprecision(18) << scientific << pmp->G0[0] << endl;
//  cout << "Guns " << syp->Guns[pmp->muj[0]] << endl;
//  cout << "GEX " << syp->GEX[pmp->muj[0]] << endl;
//  cout << "G " << tpp->G[pmp->muj[0]] << endl;
//  cout << "YOF " << pmp->YOF[0] << endl;
//  cout << "pmp->ln5551 " << pmp->ln5551 << endl;
//  cout << "pmp->RT " << pmp->RT << endl;
//  cout << "pmp->Pc " << pmp->Pc << endl;
  pmp->pTPD = 2;
}

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Finalizing the full structure MULTI by expanding the SysEq record
// read from the database
//
void TMulti::EqstatExpand( const char *key )
{
    long int i, j, k, jb, je=0; // jpb, jpe=0, jdb, jde=0;
//    double FitVar3;
    SPP_SETTING *pa = &TProfil::pm->pa;
    pmp->NR = pmp->N;

    // recalculate kinetic restrictions for DC
    if( pmp->pULR && pmp->PLIM )
         Set_DC_limits( DC_LIM_INIT );

    TotalPhases( pmp->X, pmp->XF, pmp->XFA );
    for( j=0; j<pmp->L; j++ )
        pmp->Y[j] = pmp->X[j];

    for( k=0; k<pmp->FI; k++ )
    {
        pmp->YF[k] = pmp->XF[k];
        if( k<pmp->FIs )
            pmp->YFA[k] = pmp->XFA[k];
    }

    // set IPM weight multipliers for DC
    WeightMultipliers( false );

    // calculate elemental chemical potentials in J/mole
    for( i=0; i<pmp->N; i++ )
        pmp->U_r[i] = pmp->U[i]*pmp->RT;

    // calculate DC (species) concentrations
    ConCalc( pmp->X, pmp->XF, pmp->XFA);

    // Calculate mass-balance residuals (moles)
    MassBalanceResiduals( pmp->N, pmp->L, pmp->A, pmp->X, pmp->B, pmp->C );

    // Calc Eh, pe, pH,and other stuff
    if( pmp->E && pmp->LO )
        IS_EtaCalc();

    // test multicomponent phases and load data for mixing models
    // Added experimentally 07.03.2008   by DK
    if( pmp->FIs )
    {
      int k, jb, je=0;
      for( k=0; k<pmp->FIs; k++ )
      { // loop on solution phases
            jb = je;
            je += pmp->L1[k];
   	 // Load activity coeffs for phases-solutions
       	for( j=jb; j< je; j++ )
           {
               pmp->lnGmo[j] = pmp->lnGam[j];
               if( fabs( pmp->lnGam[j] ) <= 84. )
   //                pmp->Gamma[j] = exp( pmp->lnGam[j] );
            	  pmp->Gamma[j] = PhaseSpecificGamma( j, jb, je, k, 0 );
               else pmp->Gamma[j] = 1.0;
           } // j
        }  // k
    	if( pmp->pIPN <=0 )  // mixing models finalized
        {
             // not done if these models are already present in MULTI !
           pmp->PD = TProfil::pm->pa.p.PD;
           SolModLoad();   // Call point to loading scripts for mixing models
        }
    	pmp->pIPN = 1;

if(pmp->E && pmp->LO && pmp->Lads )  // Calling this only when sorption models are present
{
	for( k=0; k<pmp->FIs; k++ )
	{ // loop on solution phases
	   jb = je;
	   je += pmp->L1[k];
	   if( pmp->PHC[k] == PH_POLYEL || pmp->PHC[k] == PH_SORPTION )
	   {
		  if( pmp->PHC[0] == PH_AQUEL && pmp->XF[k] > pmp->DSM
		       && (pmp->XFA[0] > pmp->lowPosNum && pmp->XF[0] > pa->p.XwMin ))
		       GouyChapman( jb, je, k );
	   }
	}
//   GammaCalc( LINK_UX_MODE);
}
//   double FitVar3 = pmp->FitVar[3];  // Reset the smoothing factor
//   pmp->FitVar[3] = 1.0;
       GammaCalc( LINK_TP_MODE);   // Computing DQF, FugPure and G wherever necessary
                                   // Activity coeffs are restored from lnGmo
//   pmp->FitVar[3]=FitVar3;
    }
    else {  // no multi-component phases
        pmp->PD = 0;
        pmp->pIPN = 1;
    }
    pmp->GX_ = pmp->FX * pmp->RT;

    // Calculate primal DC chemical potentials defined via g0_j, Wx_j and lnGam_j
    PrimalChemicalPotentials( pmp->F, pmp->X, pmp->XF, pmp->XFA );

    //calculate Karpov phase stability criteria
    f_alpha();

    // dynamic work arrays - loading initial data  (added 07.03.2008)
    for( k=0; k<pmp->FI; k++ )
    {
        pmp->XFs[k] = pmp->XF[k];
        pmp->Falps[k] = pmp->Falp[k];
        memcpy( pmp->SFs[k], pmp->SF[k], MAXPHNAME+MAXSYMB );
    }

    //calculate gas partial pressures  -- obsolete?, retained evtl. for old process scripts
    GasParcP();
}

//Calculation by IPM (preparing for calculation, unpacking data)
// parameter "key" contains SysEq record key
//
void TMulti::MultiCalcInit( const char *key )
{
    long int j, k, jb, je=0;
    SPP_SETTING *pa = &TProfil::pm->pa;
   // Bulk composition and/or dimensions changed ?
    if( pmp->pBAL < 2 || pmp->pTPD < 2)
    {
       // Allocating list of phases currently present in non-zero quantities
        MultiRemake( key );  // Reallocating/reloading MULTI
        // Allocating list of phases currently present in non-zero quantities
         if( !pmp->SFs )
            pmp->SFs = (char (*)[MAXPHNAME+MAXSYMB])aObj[ o_wd_sfs].Alloc(
                        pmp->FI, 1, MAXPHNAME+MAXSYMB );
    }
   // calculate mass of the system
   pmp->MBX = 0.0;
   for(int i=0; i<pmp->N; i++ )
      pmp->MBX += pmp->B[i] * pmp->Awt[i];
   pmp->MBX /= 1000.;

    // unpack SysEq record
    if( pmp->pESU /*== 1*/ && pmp->pNP ) // problematic - check ->pNP settings
    {
       loadData( false );  // unpack SysEq record into MULTI
       for( j=0; j< pmp->L; j++ )
         pmp->X[j] = pmp->Y[j];
//       pmp->IC = 0.;  //  Problematic statement!  blocked 13.03.2008 DK
       TotalPhases( pmp->X, pmp->XF, pmp->XFA );
       ConCalc( pmp->X, pmp->XF, pmp->XFA);             // 13.03.2008  DK
    }
    else // Simplex initial approximation to be done
    {
    	for( j=0; j<pmp->L; j++ )
    	{                           // cleaning work vectors
    		pmp->X[j] = pmp->Y[j] = pmp->lnGam[j] = pmp->lnGmo[j] = 0.0;
    		pmp->Gamma[j] = 1.0;
    	}
//    	pmp->FitVar[4] = pa->p.AG;
//        pmp->IT = 0;     // needed here to clean LINK_TP_MODE
    }

    // recalculating kinetic restrictions for DC amounts
     if( pmp->pULR && pmp->PLIM )
          Set_DC_limits(  DC_LIM_INIT );

    // test multicomponent phases and load data for mixing models
    if( pmp->FIs )
    {
     	// Load activity coeffs for phases-solutions
      int k, jb, je=0;
   	  for( k=0; k<pmp->FIs; k++ )
      { // loop on solution phases
         jb = je;
         je += pmp->L1[k];
   	 // Load activity coeffs for phases-solutions
       	 for( j=jb; j< je; j++ )
         {
            pmp->lnGmo[j] = pmp->lnGam[j];
            if( fabs( pmp->lnGam[j] ) <= 84. )
   //                pmp->Gamma[j] = exp( pmp->lnGam[j] );
           	   pmp->Gamma[j] = PhaseSpecificGamma( j, jb, je, k, 0 );
            else pmp->Gamma[j] = 1.0;
          } // j
       }  // k
   	   if( pmp->pIPN <=0 )  // mixing models finalized
       {
             // not done if these models are already present in MULTI !
           pmp->PD = TProfil::pm->pa.p.PD;
           SolModLoad();   // Call point to loading scripts for mixing models
       }
       pmp->pIPN = 1;

    	// Calc Eh, pe, pH,and other stuff
if( pmp->E && pmp->LO && pmp->pNP )
{
	ConCalc( pmp->X, pmp->XF, pmp->XFA);
	IS_EtaCalc();
    if( pmp->Lads )  // Calling this only when sorption models are present
    {
	   for( k=0; k<pmp->FIs; k++ )
	   { // loop on solution phases
	      jb = je;
	      je += pmp->L1[k];
	      if( pmp->PHC[k] == PH_POLYEL || pmp->PHC[k] == PH_SORPTION )
	      {
		     if( pmp->PHC[0] == PH_AQUEL && pmp->XF[k] > pmp->DSM
		       && (pmp->XFA[0] > pmp->lowPosNum && pmp->XF[0] > pa->p.XwMin ))
		       GouyChapman( jb, je, k );  // getting PSIs - elecrtic potentials on surface planes
	      }
	   }
    }
//  GammaCalc( LINK_UX_MODE );
}
//   double FitVar3 = pmp->FitVar[3];  // Reset the smoothing factor
//   pmp->FitVar[3] = 1.0;
    GammaCalc( LINK_TP_MODE);   // Computing DQF, FugPure and G wherever necessary
                                   // Activity coeffs are restored from lnGmo
//   pmp->FitVar[3]=FitVar3;
    }
    else {  // no multi-component phases
        pmp->PD = 0;
        pmp->pIPN = 1;
    }

    // dynamic work arrays - loading initial data
    for( k=0; k<pmp->FI; k++ )
    {
        pmp->XFs[k] = pmp->YF[k];
        pmp->Falps[k] = pmp->Falp[k];
        memcpy( pmp->SFs[k], pmp->SF[k], MAXPHNAME+MAXSYMB );
    }
}

//--------------------- End of ms_muleq.cpp ---------------------------

