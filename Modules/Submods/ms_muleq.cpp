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
// This file may be distributed under the terms of GEMS4 Development
// Quality Assurance Licence (GEMS4.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//
#include <cstdio>
#include <iomanip>
#include <cmath>

#include "tmltsystem.h"
#include "m_syseq.h"
#include "v_user.h"
#include "ms_mtparm.h"
#include "ms_rmults.h"
#include "ms_system.h"

// Setting sizes of packed MULTI arrays and scalars from GEMIPM calculation
void TMultiSystem::setSizes()
{
    short j, Lp;
    TSysEq* STat = (TSysEq*)(aMod[RT_SYSEQ]);

    // short
    STat->stp->N = pmp->N;
    STat->stp->Fi = pmp->FI1;
    STat->stp->Fis = pmp->FI1s;
    STat->stp->itIPM = pmp->IT;
//    STat->stp->itPar = pmp->W1;  // Number of IPM-2 loops KD 29.11.01 obsolete
    STat->stp->itPar = pmp->ITaia;  // Added 20.06.2008 DK
    // float
    STat->stp->V = pmp->VXc;
    STat->stp->T = pmp->Tc;
    STat->stp->P = pmp->Pc;
    STat->stp->H = pmp->HXc;
    STat->stp->S = pmp->SXc;
    STat->stp->UU = pmp->FX;
    STat->stp->PCI = pmp->PCI;
    STat->stp->ParE = pmp->FitVar[3]; // smoothing factor

    // calculating number of DC to be packed
    for( Lp=0,j=0; j<pmp->L; j++ )
        if( pmp->X[j] >= fmin( pmp->lowPosNum, pmp->DcMinM ) )
            Lp++;
    STat->stp->L = Lp;
}

// Packed MULTI arrays into SysEq record
void TMultiSystem::packData()
{
    short i,j;
    TSysEq* STat = (TSysEq*)(aMod[RT_SYSEQ]);

    if( pmp->N != STat->stp->N  ) // crash if error in calculation System
    {
      STat->setCalcFlag( false );
      return;
    }

    for( i=0,j=0; j<pmp->L; j++ )
        if( pmp->X[j] >= fmin( pmp->lowPosNum, pmp->DcMinM ) )
        {
            if( i>= STat->stp->L ) // crash if error in calculation System
            {
              STat->setCalcFlag( false );
              return;
            }
            STat->stp->llf[i] = pmp->muj[j];
            STat->stp->Y[i] = pmp->X[j];
            STat->stp->lnGam[i] = pmp->lnGam[j];
            i++;
        }
    for( i=0; i<pmp->N; i++ )
    {

        STat->stp->nnf[i] = pmp->mui[i];
        STat->stp->B[i] = pmp->B[i];
        STat->stp->U[i] = pmp->U[i];
    }

    for( i=0, j=0; j<pmp->FIs; j++ )
        if( pmp->YF[j] > 1e-18 )
        {
           if( i>= STat->stp->Fis ) // crash if error in calculation System
           {
              STat->setCalcFlag( false );
              return;
            }

            STat->stp->phf[i] = pmp->muk[j];
            for( int k=0; k<pmp->N; k++ )
                *(STat->stp->Ba+i*pmp->N+k) = *(pmp->BF+j*pmp->N+k);
            i++;
        }
}

// Packing MULTI arrays into SysEq record (to z_sp_conf mode)
//
void TMultiSystem::packData( const vector<int>& PHon, const vector<int>& DCon )
{
    short i,j;
    TSysEq* STat = (TSysEq*)(aMod[RT_SYSEQ]);

    for( i=0,j=0; j<pmp->L; j++ )
        if( pmp->X[j] >= fmin( pmp->lowPosNum, pmp->DcMinM ))
        {
            STat->stp->llf[i] = (short)DCon[pmp->muj[j]];
            STat->stp->Y[i] = pmp->X[j];
            STat->stp->lnGam[i] = pmp->lnGam[j];
            i++;
        }
    for( i=0; i<pmp->N; i++ )
    {
        STat->stp->nnf[i] = pmp->mui[i];
        STat->stp->B[i] = pmp->B[i];
        STat->stp->U[i] = pmp->U[i];
    }

    for( i=0, j=0; j<pmp->FIs; j++ )
        if( pmp->YF[j] > 1e-18 )
        {
            STat->stp->phf[i] = (short)PHon[pmp->muk[j]];
            for( int k=0; k<pmp->N; k++ )
              *(STat->stp->Ba+i*pmp->N+k) = *(pmp->BF+j*pmp->N+k);
            i++;
        }
}

// Unpacking SysEq arrays to multi (Y to pmp->Y)
//
void TMultiSystem::unpackData()
{
    TSysEq* STat = (TSysEq*)(aMod[RT_SYSEQ]);
    TProfil* Prf = (TProfil*)(aMod[RT_PARAM]);

    int i, j, js, jp, is, ip;

   if( pmp->pESU == 2 )   // this SysEq record has already been unpacked
   {    // pmp->IT = 0;   // Debugging 12.03.2008 DK
        return;
   }

    for( is=0; is<STat->stp->N; is++ )
    {
        i = STat->stp->nnf[is];
        // looking for this IComp in MULTI
        for( j=0; j<pmp->N; j++ )
            if( i != pmp->mui[j] )
                continue;
            else
            {
                ip = j;
                goto FOUNDI;
            }
        pmp->pNP = 1;
        { string err = "No ";
          err += string(TRMults::sm->GetMU()->SB[i], 0, MAXICNAME);
          err += " IComp in the system!";
          Error( GetName(), err /*"no such IComp in this system"*/ );
        }
FOUNDI:
        pmp->U[ip] = STat->stp->U[is];
        if( pmp->pESU != 2 ) pmp->B[ip] = STat->stp->B[is];    // Added
    }

    // Cleaning  Y and lnGam vectors
    for( j=0; j<pmp->L; j++ )
    {
        pmp->Y[j] = 0.0;
        pmp->lnGam[j] = 0.0;
    }

    for( js=0; js<STat->stp->L; js++ )
    {
        j = Prf->indDC( STat->stp->llf[js] );
        // looking for a DC in MULTI
        for( i=0; i<pmp->L; i++ )
            if( j != pmp->muj[i] )
                continue;
            else
            {
                jp = i;
                goto FOUND;
            }
        pmp->pNP = 1;
        Error( GetName(), "No this DComp in the system" );
FOUND:
        pmp->Y[jp] /* = pmp->X[jp] */ = STat->stp->Y[js];
        pmp->lnGam[jp] = STat->stp->lnGam[js];
    }

    // short
    pmp->ITaia = STat->stp->itPar;  // Level of tinkle supressor
//    pmp->pRR1 = STat->stp->itPar;  // Level of tinkle supressor
    pmp->FI1 = STat->stp->Fi;
    pmp->FI1s = STat->stp->Fis;
pmp->IT = STat->stp->itIPM;
//    pmp->IT = 0;         Debugging 12.03.2008 DK
    pmp->W1 = 0; pmp->K2 = 0;
    // float
    pmp->FitVar[3] = STat->stp->ParE;  // Smoothing factor
    pmp->FX  = STat->stp->UU;  // GX normalized
    pmp->PCI = STat->stp->PCI;
    pmp->VXc = STat->stp->V;
    pmp->HXc = STat->stp->H;
    pmp->SXc = STat->stp->S;
    // pmp->IC = STat->stp->Res1;

    pmp->pESU = 2;  // SysEq unpack flag set
}

// load data from TSysEq (EQstat => Multi)
void TMultiSystem::loadData( bool newRec )
{
    // nesessary to realloc data after new system   (before function )
    // setDefData();
    if( newRec == false )
        unpackData();
}



// Get T, P, V from record key
void TMultiSystem::MultiKeyInit( const char*key )
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

   if( V <= 0 ) // no volume balance needed
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



// Finalizing the full structure MULTI by expanding the SysEq record
// read from the database
//
void TMultiSystem::EqstatExpand( )
{
    long int i, j, k;
//    double FitVar3;
    SPP_SETTING *pa = &TProfil::pm->pa;
    pmp->NR = pmp->N;

    bool AllPhasesPure = true;   // Added by DK on 09.03.2010
    // checking if all phases are pure
    for( k=0; k < pmp->FI; k++ )
        if( pmp->L1[k] > 1 )
            AllPhasesPure = false;

    ///TotalPhasesAmounts( pmp->X, pmp->XF, pmp->XFA );
    ///for( j=0; j<pmp->L; j++ )
    ///    pmp->Y[j] = pmp->X[j];

    for( k=0; k<pmp->FI; k++ )
    {
        pmp->YF[k] = pmp->XF[k];
        if( k<pmp->FIs )
            pmp->YFA[k] = pmp->XFA[k];
    }
    // calculate DC (species) concentrations and activities
    ///CalculateConcentrations( pmp->X, pmp->XF, pmp->XFA);

    // recalculate kinetic restrictions for DC
    if( pmp->pULR && pmp->PLIM )
         pBaseSyst->Set_DC_limits( DC_LIM_INIT );

    // calculate elemental chemical potentials in J/mole
    for( i=0; i<pmp->N; i++ )
        pmp->U_r[i] = pmp->U[i]*pmp->RT;

    // Calculate mass-balance residuals (moles)
    pBaseSyst->MassBalanceResiduals( pmp->N, pmp->L, pmp->A, pmp->X, pmp->B, pmp->C );

    // Calc Eh, pe, pH,and other stuff
    if( pmp->E && pmp->LO )
        pBaseSyst->IS_EtaCalc();

    // set IPM weight multipliers for DC
    WeightMultipliers( false );

    // test multicomponent phases and load data for mixing models
    //
    if( pmp->FIs && AllPhasesPure == false )   // bugfix DK 11.03.2010
    {
      int k, jb, je=0;
      ///for( k=0; k<pmp->FIs; k++ )
      ///{ // loop on solution phases
      ///      jb = je;
      ///      je += pmp->L1[k];
         // Load activity coeffs for phases-solutions
      ///  for( j=jb; j< je; j++ )
      ///     {
      ///         pmp->lnGmo[j] = pmp->lnGam[j];
      ///         if( fabs( pmp->lnGam[j] ) <= 84. )
      ///      	  pmp->Gamma[j] = PhaseSpecificGamma( j, jb, je, k, 0 );
      ///         else pmp->Gamma[j] = 1.0;
      ///     } // j
      ///  }  // k

       CalculateActivityCoefficients( LINK_TP_MODE );
        // Computing DQF, FugPure and G wherever necessary; Activity coeffs are restored from lnGmo
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
       }
        CalculateActivityCoefficients( LINK_UX_MODE );
    }
    else {  // no multi-component phases
        pmp->PD = 0;
        pmp->pIPN = 1;
    }


    pmp->FX = pBaseSyst->pb_GX( pmp->G );
    pmp->GX_ = pmp->FX * pmp->RT;

    // Calculate primal DC chemical potentials defined via g0_j, Wx_j and lnGam_j
    PrimalChemicalPotentials( pmp->F, pmp->X, pmp->XF, pmp->XFA );

    if( pa->p.PC == 1 )
    {  //calculate Karpov phase stability criteria
       pBaseSyst->KarpovsPhaseStabilityCriteria();
    }
    else if( pa->p.PC >= 2 )
    {
       StabilityIndexes( );
    }

    // getting total mass of solid phases in the system
    pmp->FitVar[0] = pBaseSyst->bfc_mass();   // added DK 02.03.2012

    // dynamic work arrays - loading initial data  (added 07.03.2008)
    for( k=0; k<pmp->FI; k++ )
    {
        pmp->XFs[k] = pmp->XF[k];
        pmp->Falps[k] = pmp->Falp[k];
        memcpy( pmp->SFs[k], pmp->SF[k], MAXPHNAME+MAXSYMB );
    }

    //calculate gas partial pressures  -- obsolete?, retained evtl. for old process scripts
    pBaseSyst->GasParcP();
}

//--------------------- End of ms_muleq.cpp ---------------------------

