//-------------------------------------------------------------------
// $Id: ms_muleq.cpp 1409 2009-08-21 15:34:43Z gems $
//
// Implementation of TMulti  load functions
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
#include <cstdio>
#include <iomanip>
#include <cmath>

#include "m_syseq.h"

// Setting sizes of packed MULTI arrays and scalars from GEMIPM calculation
void TMulti::setSizes()
{
    short j, Lp;
    TSysEq* STat = dynamic_cast<TSysEq *>(aMod[RT_SYSEQ].get());

    // short
    STat->stp->N = pm.N;
    STat->stp->Fi = pm.FI1;
    STat->stp->Fis = pm.FI1s;
    STat->stp->itIPM = pm.IT;
//    STat->stp->itPar = pm.W1;  // Number of IPM-2 loops KD 29.11.01 obsolete
    STat->stp->itPar = pm.ITaia;  // Added 20.06.2008 DK
    // float
    STat->stp->V = ( pm.VXc< 1e-38 ? 0: pm.VXc);
    STat->stp->T = pm.Tc;
    STat->stp->P = pm.Pc;
    STat->stp->H = pm.HXc;
    STat->stp->S = pm.SXc;
    STat->stp->UU = pm.FX;
    STat->stp->PCI = pm.PCI;
    STat->stp->ParE = pm.FitVar[3]; // smoothing factor

    // calculating number of DC to be packed
    for( Lp=0,j=0; j<pm.L; j++ )
        if( pm.X[j] >= fmin( pm.lowPosNum, pm.DcMinM ) )
            Lp++;
    STat->stp->L = Lp;
}

// Packed MULTI arrays into SysEq record
void TMulti::packData()
{
    short i,j;
    TSysEq* STat = dynamic_cast<TSysEq *>(aMod[RT_SYSEQ].get());

    if( pm.N != STat->stp->N  ) // crash if error in calculation System
    {
      STat->setCalcFlag( false );
      return;
    }

    for( i=0,j=0; j<pm.L; j++ )
        if( pm.X[j] >= fmin( pm.lowPosNum, pm.DcMinM ) )
        {
            if( i>= STat->stp->L ) // crash if error in calculation System
            {
              STat->setCalcFlag( false );
              return;
            }
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
           if( i>= STat->stp->Fis ) // crash if error in calculation System
           {
              STat->setCalcFlag( false );
              return;
            }

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
    TSysEq* STat = dynamic_cast<TSysEq *>(aMod[RT_SYSEQ].get());

    for( i=0,j=0; j<pm.L; j++ )
        if( pm.X[j] >= fmin( pm.lowPosNum, pm.DcMinM ))
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
    TSysEq* STat = dynamic_cast<TSysEq *>( aMod[RT_SYSEQ].get());
    TProfil* Prf = dynamic_cast<TProfil *>( aMod[RT_PARAM].get());

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
        { string err = "No ";
          err += string(TRMults::sm->GetMU()->SB[i], 0, MAXICNAME);
          err += " IComp in the system!";
          Error( GetName(), err /*"no such IComp in this system"*/ );
        }
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

// Get T, P, V from record key
void TMulti::MultiKeyInit( const char*key )
{
   double V, T, P;
   char typeMin;

   memcpy( pmp->stkey, key, EQ_RKLEN );
   pmp->stkey[EQ_RKLEN] = '\0';

   
   // Get V, P and T from SysEq record key
   string s = string( key,MAXMUNAME+MAXTDPCODE+MAXSYSNAME+MAXTIME,MAXPTN);
   V = atof(s.c_str());
   s = string( key,MAXMUNAME+MAXTDPCODE+MAXSYSNAME+MAXTIME+MAXPTN,MAXPTN);
   P = atof(s.c_str());
   s = string( key,MAXMUNAME+MAXTDPCODE+MAXSYSNAME+MAXTIME+MAXPTN+MAXPTN,MAXPTN);
   T = atof(s.c_str());
   s = string( key,MAXMUNAME, MAXTDPCODE);

   typeMin = s[0];
   switch( typeMin )
   {
     case  A_TV: pmp->tMin = A_TV_; break;
     case  U_SV: pmp->tMin = U_SV_; break;
     case  H_PS: pmp->tMin = H_PS_; break;
     case  _S_PH: pmp->tMin = _S_PH_; break;
     case  _S_UV: pmp->tMin = _S_UV_; break;
     case  G_TP:
     default: pmp->tMin = G_TP_; break;
   }

   //if( fabs ( pmp->VE - V ) > 1.e-10 )
   //    pmp->VE = V;
   
   if( fabs( TMTparm::sm->GetTP()->curT - T ) > 1.e-10 ||
           fabs( TMTparm::sm->GetTP()->curP - P ) > 1.e-10 )
   { // load new MTPARM on T or P
   //    mtparm->LoadMtparm( T, P );
       pmp->T = pmp->Tc = T + C_to_K;
       pmp->TC = pmp->TCc = T;
       pmp->P = pmp->Pc = P;
       pmp->RT = R_CONSTANT * pmp->Tc;
       pmp->FRT = F_CONSTANT/pmp->RT;
       pmp->pTPD = 0;
   }

   if( V <= 1e-20 ) // no volume balance needed
       {
           pmp->VX_ = pmp->VXc = 0.0;
           pmp->PV = VOL_CALC;
       }
    else
       {  // volume of the system is given
           pmp->VX_ = pmp->VXc = V;
           if(TSyst::sm->GetSY()->PV == S_ON )
              pmp->PV = VOL_CONSTR;
           else
              pmp->PV = VOL_UNDEF;
       }
}


    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Finalizing the full structure MULTI by expanding the SysEq record
// read from the database
//
void TMulti::EqstatExpand( /*const char *key,*/ bool calcActivityModels/*, bool calcKineticModels*/ )
{
    long int i, j, kk;//, jb, je=0, jpb, jpe=0, jdb, jde=0;
//    double FitVar3;
    SPP_SETTING *pa = &TProfil::pm->pa;
    pmp->NR = pmp->N;

    bool AllPhasesPure = true;   // Added by DK on 09.03.2010
    // checking if all phases are pure
    for( kk=0; kk < pmp->FI; kk++ )
        if( pmp->L1[kk] > 1 )
            AllPhasesPure = false;

    TotalPhasesAmounts( pmp->X, pmp->XF, pmp->XFA );
    for( j=0; j<pmp->L; j++ )
        pmp->Y[j] = pmp->X[j];

    for( kk=0; kk<pmp->FI; kk++ )
    {
        pmp->YF[kk] = pmp->XF[kk];
        if( kk<pmp->FIs )
            pmp->YFA[kk] = pmp->XFA[kk];
    }
    // calculate DC (species) concentrations and activities
    CalculateConcentrations( pmp->X, pmp->XF, pmp->XFA);

    // recalculate kinetic restrictions for DC
    if( pmp->pULR && pmp->PLIM )
//         Set_DC_limits( DC_LIM_INIT );
        Set_DC_limits( true );    // test 29.07.15 (dangerous!!!!!!!!!!)

    // calculate elemental chemical potentials in J/mole
    for( i=0; i<pmp->N; i++ )
        pmp->U_r[i] = pmp->U[i]*pmp->RT;

    // Calculate mass-balance residuals (moles)
    MassBalanceResiduals( pmp->N, pmp->L, pmp->A, pmp->X, pmp->B, pmp->C );

    // Calc Eh, pe, pH,and other stuff
    if( pmp->E && pmp->LO )
        IS_EtaCalc();

    // set IPM weight multipliers for DC
    WeightMultipliers( true /*false*/ );

// New: TKinMet stuff
//if( calcKineticModels )
//{
//    if( pmp->pKMM <= 0 )
//    {
//        KinMetModLoad();
//        pmp->pKMM = 1;
//    }
//}
    // test multicomponent phases and load data for mixing models
    //
    if( pmp->FIs && AllPhasesPure == false )   // bugfix DK 11.03.2010
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

      if( calcActivityModels )  // added 10.04.2012  DK
      {
    	if( pmp->pIPN <=0 )  // mixing models finalized
        {
             // not done if these models are already present in MULTI !
           pmp->PD = abs(TProfil::pm->pa.p.PD);
           SolModLoad();   // Call point to loading scripts for mixing models
        }
    	pmp->pIPN = 1;

        //   double FitVar3 = pmp->FitVar[3];  // Debugging: Reset the smoothing factor
        //   pmp->FitVar[3] = 1.0;
        CalculateActivityCoefficients( LINK_TP_MODE );
       // Computing DQF, FugPure and G wherever necessary; Activity coeffs are restored from lnGmo
        //   pmp->FitVar[3]=FitVar3;  // Debugging
        if(pmp->E && pmp->LO && pmp->Lads )  // Calling this only when sorption models are present
        {
            jb = 0;
            for( k=0; k<pmp->FIs; k++ )
            { // loop on solution phases
               jb = je;
               je += pmp->L1[k];
               if( pmp->PHC[k] == PH_POLYEL || pmp->PHC[k] == PH_SORPTION )
               {
		  if( pmp->PHC[0] == PH_AQUEL && pmp->XF[k] > pmp->DSM
                       && (pmp->XFA[0] > pmp->ScMinM && pmp->XF[0] > pmp->XwMinM ))  // fixed 30.08.2009 DK
		       GouyChapman( jb, je, k );
               }
            }
            //CalculateActivityCoefficients( LINK_UX_MODE );
        }
        CalculateActivityCoefficients( LINK_UX_MODE );
      }
      else
      {   double  LnGam = 0.;
          je =0;
          for( k=0; k<pm.FI; k++ )
          { // loop on phases
              jb = je;
              je += pm.L1[k];
             // Real mode for activity coefficients
             double lnGamG;
             for( j=jb; j<je; j++ )
             {
               if( pm.DCC[j] == DC_AQ_SURCOMP )  // Workaround for aqueous surface complexes DK 22.07.09
                  pm.lnGam[j] = 0.0;
               lnGamG = PhaseSpecificGamma( j, jb, je, k, 1 );
               LnGam = pm.lnGam[j];
               if( fabs( lnGamG ) > 1e-9 )
                  LnGam += lnGamG;
               pm.lnGmo[j] = LnGam;
               if( fabs( LnGam ) < 84. )   // before 26.02.08: < 42.
                      pm.Gamma[j] = PhaseSpecificGamma( j, jb, je, k, 0 );
               else pm.Gamma[j] = 1.0;

               pm.F0[j] = DC_PrimalChemicalPotentialUpdate( j, k );
               pm.G[j] = pm.G0[j] + pm.fDQF[j] + pm.F0[j];
             }
          }
      }

    }
    else {  // no multi-component phases
        pmp->PD = 0;
        pmp->pIPN = 1;
    }
    pmp->FX = pb_GX( pmp->G );
    pmp->GX_ = pmp->FX * pmp->RT;

    // Calculate primal DC chemical potentials defined via g0_j, Wx_j and lnGam_j
    PrimalChemicalPotentials( pmp->F, pmp->X, pmp->XF, pmp->XFA );

    if( pa->p.PC == 1 )
    {  //calculate Karpov phase stability criteria
       KarpovsPhaseStabilityCriteria();
    }
    else if( pa->p.PC >= 2 )
    {
       StabilityIndexes( );
    }

    // getting total mass of solid phases in the system
    pmp->FitVar[0] = bfc_mass();   // added DK 02.03.2012

    // dynamic work arrays - loading initial data  (added 07.03.2008)
    for( kk=0; kk<pmp->FI; kk++ )
    {
        pmp->XFs[kk] = pmp->XF[kk];
        pmp->Falps[kk] = pmp->Falp[kk];
        memcpy( pmp->SFs[kk], pmp->SF[kk], MAXPHNAME+MAXSYMB );
    }

    //calculate gas partial pressures  -- obsolete?, retained evtl. for old process scripts
    GasParcP();
}

//--------------------- End of ms_muleq.cpp ---------------------------

