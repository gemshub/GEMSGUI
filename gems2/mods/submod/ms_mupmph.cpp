//-------------------------------------------------------------------
// $Id$
//
// Copyright (C) 1992-2000 S.Dmitrieva, D.Kulik, K.Chudnenko, I.Karpov
//
// Implementation of parts of the Interior Points Method (IPM) module
// for convex programming Gibbs energy minimization, described in:
// (Karpov, Chudnenko, Kulik (1997): American Journal of Science
//  v.297 p. 767-806)
//
// This file is part of a GEM-Selektor (GEMS) v.2.x.x program
// environment for thermodynamic modeling in geochemistry
// Uses: GEM-Vizor GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://les.web.psi.ch/Software/GEMS-PSI for more information
// E-mail: gems2.support@psi.ch; chud@igc.irk.ru
//-------------------------------------------------------------------
//
#include <math.h>

#include "m_param.h"
#include "m_proces.h"
#include "m_probe.h"
#include "s_formula.h"
#include "m_syseq.h"
#include "service.h"
#include "visor.h"


// Setup of flags for MULTY remake
// pNP,  //Mode of FIA selection: 0-auto-SIMPLEX,1-old eqstate,-1-user's choice
// pESU, // Unpack old eqstate from EQSTAT record?  0-no 1-yes
// pIPN, // State of IPN-arrays:  0-create; 1-available; -1 remake
// pBAL, // State of reloading CSD:  1- BAL only; 0-whole CSD
// pFAG, //State of initial lnGam load: 0-no, 1-on Mbel, 2-lnGmf, -1-SurEta
// pTPD, // State of reloading thermod data: 0- all    2 - no
// pULR, // reserved
void TProfil::PMtest( const char *key )
{
    float T, P;
    TSysEq* STat = (TSysEq*)(&aMod[RT_SYSEQ]);
    TProcess* Proc = (TProcess*)(&aMod[RT_PROCES]);
    TProbe* Prob = (TProbe*)(&aMod[RT_PROBE]);

    ///  pmp->pNP = -1;
    if( STat->ifCalcFlag())
        pmp->pESU = 1;
    else pmp->pESU = 0;

    if( pmp->pESU == 0 ) // no old solution
        pmp->pNP = 0;
    if( Proc->pep->Istat == P_EXECUTE )
    {
        if(Proc->pep->PvR1 == S_OFF )
            pmp->pNP = 0;
        else
            pmp->pNP = 1;
    }
    if( Prob->prp->pbcalc == P_EXECUTE )
    {
        if(Prob->prp->zond[11] == S_OFF )
            pmp->pNP = 0;
        else
            pmp->pNP = 1;
    }
    pmp->pBAL =  BAL_compare();
    if( !pmp->pBAL )
        pmp->pIPN = 0;
    if( multi->qEp.GetCount()<1 || multi->qEd.GetCount()<1 )
        pmp->pIPN = 0;
    // Get P and T from key
    gstring s = gstring( key,MAXMUNAME+MAXTDPCODE+MAXSYSNAME+MAXTIME+MAXPTN,MAXPTN);
    P = atof(s.c_str());
    s = gstring( key,MAXMUNAME+MAXTDPCODE+MAXSYSNAME+MAXTIME+MAXPTN+MAXPTN,MAXPTN);
    T = atof(s.c_str());

    if( fabs( tpp->curT - T ) > 1.e-10 ||
            fabs( tpp->curP - P ) > 1.e-10 )
    { // load new MTPARM on T or P
        mtparm->LoadMtparm( T, P );
        pmp->pTPD = 0;
    }
}

// -------------------------------------------------------------------
// Compare changes in the modified system relative to MULTI
// if some vectors were allocated or some dimensions changed - return 0;
// else if bulk composition or some constraints has changed - return 1;
// else if nothing has changed - return 2.
short TProfil::BAL_compare()
{
    int i,j,k;
    /* test sizes */
    if( pmp->N != syp->N || pmp->L != syp->L || pmp->Ls != syp->Ls
            || pmp->LO != syp->Lw || pmp->PG != syp->Lg
            || pmp->PSOL != syp->Lhc || pmp->Lads != syp->Lsor
            || pmp->FI != syp->Fi || pmp->FIs != syp->Fis )
        return 0;
    if(( syp->DLLim == S_ON || syp->DULim == S_ON ) && pmp->PLIM != 1 )
        return 0;
    else if( syp->DLLim == S_OFF && syp->DULim == S_OFF && pmp->PLIM == 1 )
        return 0;

    /* test selectors */
    for( i=0; i<pmp->N; i++ )
        if( syp->Icl[pmp->mui[i]] == S_OFF )
            return 0;
    for( j=0; j<pmp->L; j++ )
        if( syp->Dcl[pmp->muj[j]] == S_OFF )
            return 0;
    for( k=0; k<pmp->FI; k++ )
        if( syp->Pcl[pmp->muk[k]] == S_OFF )
            return 0;
    /* lists of components didn't change */
    /* test B */
    for( i=0; i<pmp->N; i++ )
        if( fabs( syp->B[pmp->mui[i]] - pmp->B[i] ) >= pa.p.DB )
            return 1;
    /* test other restrictions */
    for( j=0; j<pmp->L; j++ )
    {
        if( syp->PGEX != S_OFF )
            if( fabs( syp->GEX[pmp->muj[j]] - pmp->GEX[j]*pmp->RT ) >= 0.001 )
                return 1;
        if(( syp->DLLim != S_OFF ) && pmp->PLIM == 1 )
            if( fabs( syp->DLL[pmp->muj[j]] - pmp->DLL[j] ) >= 1e-19 )
                return 1;
        if(( syp->DULim != S_OFF ) && pmp->PLIM == 1 )
            if( fabs( syp->DUL[pmp->muj[j]] - pmp->DUL[j] ) >= 1e-19 )
                return 1;
//      Adsorption models - to be completed !!!!!
        ;
    }
    // bulk chem. compos. and constraints unchanged
    return 2;
}

// Make base structure to calculate equilibrium state
//    Scheme of work
// 1) Make full  stoichiometry matrix  by list sy.Dcl
// 6) look over PHASE records and load dala from it
//    translate equations of non-ideality
// 7) continue look over one-component PHASE, if we have
//    despers phases and electrical parametres
// 8) recalc necessary values to enter into task
// 9} test possibilities to take old solution as first
//    prime solution  and unpack it
//
void TProfil::MultiRemake( const char *key )
{
    int i, N;
    short ii;
    vstr pkey(MAXRKEYLEN);
    double VS;

    memcpy( pmp->stkey, key, EQ_RKLEN );
    pmp->stkey[EQ_RKLEN] = '\0';
    /* load sizes of arrays to  MULTI */
    pmp->PunE = tpp->PunE;
    pmp->PunV = tpp->PunV;
    pmp->PunP = tpp->PunP;
    pmp->PunT = tpp->PunT;
    pmp->N =    syp->N;
    pmp->L =    syp->L;
    pmp->Ls = syp->Ls;
    pmp->LO =   syp->Lw/*-1*/;
    pmp->PG =   syp->Lg;
    pmp->PSOL = syp->Lhc;
    pmp->Lads = syp->Lsor;
    pmp->FI = syp->Fi;
    pmp->FIs = syp->Fis;
    /* pmp->FIa = syp->N; */
    pmp->FI1 = 0;
    pmp->FI1s = 0;
    pmp->FI1a = 0;
    if( syp->DLLim == S_ON || syp->DULim == S_ON )
        pmp->PLIM = 1;
    else pmp->PLIM = 0;

    pmp->IT = 0;
    if( syp->PE != S_OFF )
        pmp->E = 1;
    else pmp->E = 0;
    if( syp->NsTm > 0 && syp->PNfsp != S_OFF )
        pmp->FIat = MST;
    else pmp->FIat = 0;
    pmp->PD = pa.p.PD;

    pmp->T = pmp->Tc = tpp->T + C_to_K;
    pmp->TC = pmp->TCc = tpp->T;
    pmp->P = pmp->Pc = tpp->P;
    pmp->T0 = 273.15;    // not used anywhere 
    pmp->MBX = syp->MBX;
    pmp->FX = 7777777.;
    pmp->pH = pmp->Eh = pmp->pe = 0.0;
    pmp->GWAT = syp->Mwat;
    pmp->YMET = 0;
    pmp->PCI = 0.0;

    /* set restrictions to volume of the system */
    memcpy( pkey, pmp->stkey+48/*SY_RKLEN*/-MAXSYWHAT, MAXPTN );
    pkey[MAXPTN] = 0;
    VS = atof( pkey );
    if( VS <= 0 ) /* volume dont fix */
    {
        pmp->VX_ = pmp->VXc = pmp->VE = 0.0;
        pmp->PV = 0;
    }
    else
    {  /* volume gives */
        pmp->VX_ = pmp->VXc = pmp->VE = VS;
        pmp->PV = syp->PV;
    }

    pmp->Ec = pmp->K2 = pmp->MK = 0;
    pmp->PZ = pa.p.DW; // IPM-2 default   
    pmp->W1 = 0;
    pmp->is = 0;
    pmp->js = 0;
    pmp->next  = 0;
    pmp->IC =  syp->Mbel;

    pmp->denW = tpp->RoW;
    pmp->denWg = tpp->RoV;
    pmp->epsW = tpp->EpsW;
    pmp->epsWg = tpp->EpsV;
    pmp->GWAT = syp->Mwat * 55.508373;

    pmp->RT = tpp->RT;  // R_CONSTANT * pmp->Tc
    pmp->FRT = F_CONSTANT/pmp->RT; 

    pmp->ln5551 = 4.0165339;
    pmp->lowPosNum = pa.p.DcMin;
    pmp->logXw = -16.;
    pmp->logYFk = -9.;
    pmp->lnP = 0.;
    if( tpp->P != 1. )  // Non-reference Pressure
        pmp->lnP = log( tpp->P );
    pmp->DX = pa.p.DK;

    pmp->FitVar[4] = pa.p.AG;
    pmp->pRR1 = 0;      //IPM smoothing factor and level

    // realloc memory if necessary
    if( !pmp->pBAL )
        multi->dyn_new();
    if( pmp->pBAL == 2 )
        /*N=pmp->N;*/  goto NEXT2;
    // load parameters for independent components
    for( N=0, i=-1, ii=0; ii< mup->N; ii++ )
    {
        if( syp->Icl[ii] == S_OFF )
            continue;
        i++;
        N++;
        pmp->mui[i] = ii;
        pmp->ICC[i] = mup->ICC[ii];
        pmp->Awt[i] = mup->BC[ii];
        pmp->B[i] = syp->B[ii];
        pmp->U[i] = pmp->U_r[i] = pmp->C[i] = 0.0;
        memcpy( pmp->SB[i], mup->SB[ii], MAXICNAME+MAXSYMB );
        memcpy( pmp->SB1[i], mup->SB[ii], MAXICNAME  );
        switch( pmp->ICC[i] )
        {
        case IC_ELEMENT:
        case IC_ISOTOPE:
        case IC_FORMULA:
        case IC_METALION:
        case IC_LIGAND:
        case IC_ADDIT:
            break;

        case IC_OXYGEN16:
        case IC_OXYGEN18: /* index and quantity of O? */
        case IC_OXYGEN:
            break;

        case IC_PROTIUM:
        case IC_DEYTERIUM:
        case IC_TRITIUM:  /* index and quantity of  H? */
        case IC_HYDROGEN:
            break;

        case IC_CHARGE:   /* charge - index */
            if( !pmp->E )
                break;  /* may be errorа */
            break;
        case IC_VOLUME:   /* volume IC - index */
            if( pmp->PV == VOL_UNDEF )
                break;  /* may be errorаа */
            break;
        default: /* error in code IComp */
            ;
        }
    }
    //   ErrorIf( N != pmp->N, "Multi", "Multi make error: N != pmp->N" );
    if( N != pmp->N )
        vfMessage( window(),
                   "Multi make error: N != pmp->N", "Please, press BCC button first!" );
NEXT2:
    // load data for dependent components
    multi_sys_dc();

    /* Учесть отметки экстраполяции в MTPARM ? *
    if( tp->mark )
      for( pmp->L=0,j=0; j<mu->L; j++ )
      {
        if(( pa.ptm == TPMARK_VALID &&
          ( tp->mark[j] == CP_NOT_VALID || tp->mark[j] == PM_NOT_EXIST ))
          || ( pa.ptm == TPMARK_PMODF && tp->mark[j] == PM_NOT_EXIST ))
          { * G = 7777777; *
            sy->Dcl[j] = DELETE_C; sy->L--;
            if( sy->LO && j<mu->Laq-1 ) sy->LO--;
            if( sy->PG && j >= mu->Laq && j< mu->Laq+mu->Pg ) sy->PG--;
          }
        if( sy->Dcl[j] == DEFAULT_C )
          pmp->L++; * Подсчет количества включаемых з.к. *
      }
    if( pmp->L < pmp->N )
      if( !YNonly( msg(em_ltNdc) ))
        goto WRONG;
    * Дать предупреждение ? */

    // load data by phases, sorption and solution models
    multi_sys_ph();

    if( pmp->pBAL < 2 )
        ConvertDCC(); // Load codes DCCW
    /* Tests ? */

    pmp->pBAL = 2;
}

//Load data for DC from Modelling Project definition to structure MULTI
//
void TProfil::multi_sys_dc()
{
    int j, ii, L, iZ;
    short jj;
    float a, *A;
    double mm;
    TIArray<TFormula> aFo;
    gstring form;

    ErrorIf( !tpp->G, "Multi", "Multi make error: !tpp->G" );

    if( !pmp->pBAL )
    {  // make full stoichiometry matrix from DC formula list
        // load formulae
        for( ii=0; ii<mup->L; ii++ )
        {
            aFo.Add( new TFormula() );
            form = aFo[ii].form_extr( ii, mup->L, mup->DCF );
            aFo[ii].SetFormula( form.c_str() ); // and ce_fscan
        }

        A = new float[mup->N*mup->L];
        memset(A, 0, sizeof(float)*(mup->N*mup->L) );
        for( ii=0; ii<mup->L; ii++ )
            aFo[ii].Stm_line( mup->N, A+ii*mup->N, (char *)mup->SB, mup->Val );
        aFo.Clear();
        // load data by dependent components
        for( L=0, j=-1, jj=0; jj<mup->L; jj++ )
        {
            if( syp->Dcl[jj] == S_OFF )
                continue;
            j++;
            L++;
            pmp->DCC[j] = mup->DCC[jj];
            pmp->muj[j] = jj;
            pmp->lnGam[j] = 0.0;
            pmp->lnGmo[j] = 0.0;
            pmp->X[j] = 0.0;
            pmp->Y[j] = 0.0;
            pmp->XY[j] = 0.0;
            memcpy( pmp->SM[j], mup->SM[jj]+MAXSYMB+MAXDRGROUP, MAXDCNAME );
            if( j < syp->Ls )
                memcpy( pmp->SM2[j], mup->SM[jj]+MAXSYMB+MAXDRGROUP, MAXDCNAME );
            mm = 0.0;
            for( ii=0; ii<pmp->N; ii++ ) /* compressing stoichiometry matrix */
            {
                a = A[jj*mup->N + pmp->mui[ii]];
                pmp->A[j*pmp->N+ii] = a;
                mm += a * pmp->Awt[ii];
            }
            pmp->MM[j] = mm;
            /*  pmp->MM[j] = MolWeight( pmp->N, pmp->Awt, pmp->A+j*pmp->N ); */
        }
        delete[] A;
        // A = 0;
        //     ErrorIf( L != pmp->L, "Multi", "Multi make error: L != pmp->L" );
        if( L != pmp->L )
            vfMessage(  window(), "Multi make error: L != pmp->L", "Please, press BCC!" );

    }
    /*if( pmp->pBAL < 2 || pmp->pTPD < 2 )
      {  Reload parametres of components */
    if( pmp->E )
    { /*index of charge */
        for( iZ=0, ii=0; ii<pmp->N; ii++ )
            if( pmp->ICC[ii] == IC_CHARGE )
                goto CH_FOUND;
        // error no charge
        if( vfQuestion(window(), "Multi", "No DC with formula charge are included:\n"
                       "Proceed (Y), Cancel (N)?"  ))
            pmp->E = 0;
        else
            Error( "Multi","No DC with formula charge are included" );
CH_FOUND:
        iZ = ii;
    }
    for( j=-1, jj=0; jj<mup->L; jj++ )
    {
        if( syp->Dcl[jj] == S_OFF )
            continue;
        j++;
        pmp->DCC[j] = mup->DCC[jj];
        if( pmp->E )
            pmp->EZ[j] = pmp->A[pmp->N*j+iZ];
        pmp->Pparc[j] = pmp->Pc;
        /*    pmp->lnGmo[j] = pmp->lnGam[j] = */
        pmp->F0[j] = 0.0;
        /*    pmp->X[j] = pmp->Y[j] = */
        pmp->XY[j] = 0.0;
        pmp->G0[j] = tpp->G[jj]; //  /(RT) ? + ln P ? + ln 55.51 ?
        /* Doubtful !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

        if( tpp->PtvVm == S_ON )
            switch( pmp->PV )
            { /* calc mol volume of component*/
            case VOL_CONSTR:
                pmp->A[j*pmp->N] = tpp->Vm[jj];
            case VOL_CALC:
            case VOL_UNDEF:
                pmp->Vol[j] = tpp->Vm[jj] * 10.; /* Check! */
                break;
            }
        else pmp->Vol[j] = 0.0;
        /* test kinetic constraints */
        if( pmp->PLIM && jj < mup->L )  /* Not Ls */
        { /* restrictions set! */
            pmp->RLC[j] = syp->RLC[jj];
            pmp->RSC[j] = syp->RSC[jj];
            if( syp->DUL )
                pmp->DUL[j] = syp->DUL[jj];
            else pmp->DUL[j] = 1e6;
            if( syp->DLL )
                pmp->DLL[j] = syp->DLL[jj];
            else pmp->DLL[j] = 0.0;
        }
        else
        {
            pmp->RLC[j] = NO_LIM;
            pmp->RSC[j] = QUAN_MOL;
            pmp->DUL[j] = 1e6;  // tc_DCmaxm trivial restrictions!
            pmp->DLL[j] = 0.0;
        }
        // additional data
        if( syp->PGEX != S_OFF /* && syp->PYOF != S_ON */ )
            pmp->GEX[j] = syp->GEX[jj]/pmp->RT;
        else pmp->GEX[j] = 0.0;  // Standard free energy increments

        if( syp->PParc != S_OFF )
            pmp->Pparc[j] = syp->Pparc[jj];
        pmp->lnGmM[j] = 0.0;
        if( jj < mup->Ls && syp->PlnGf != S_OFF )
        {
            pmp->lnGmf[j] = syp->lnGmf[jj];
            if( !pa.p.PSM )  // Constant activity coefficients
                pmp->lnGmM[j] = syp->lnGmf[jj];
        }
        else pmp->lnGmf[j] = 0.0;
        /*
             if( pmp->DCC[j] == DC_PEL_CARRIER || pmp->DCC[j] == DC_SUR_CARRIER
                || pmp->DCC[j] == DC_SUR_MINAL )
              continue;
        */
        // Loading MaSdj - max.sur.densities for non-competitive sorbates */
        if( jj < mup->Ls && syp->PMaSdj != S_OFF )
        {
            if( !IsFloatEmpty( syp->MaSdj[jj] ) )
                pmp->MASDJ[j] = syp->MaSdj[jj];    /* 1/nm2! */
            else pmp->MASDJ[j] = 0;
        }
        if( jj < mup->Ls && syp->PSATT != S_OFF )
        { /* Loading SATC - codes of methods for SAT calculation */
            pmp->SATT[j] = syp->SATC[jj][0];
            /* Loading allocation to carrier end-members */
            switch( syp->SATC[jj][1] )
            {
            case CCA_0:
                pmp->SATNdx[j][1] = 0;
                break;
            case CCA_1:
                pmp->SATNdx[j][1] = 1;
                break;
            case CCA_2:
                pmp->SATNdx[j][1] = 2;
                break;
            case CCA_3:
                pmp->SATNdx[j][1] = 3;
                break;
            case CCA_4:
                pmp->SATNdx[j][1] = 4;
                break;
            case CCA_5:
                pmp->SATNdx[j][1] = 5;
                break;
            case CCA_6:
                pmp->SATNdx[j][1] = 6;
                break;
            case CCA_7:
                pmp->SATNdx[j][1] = 7;
                break;
            case CCA_8:
                pmp->SATNdx[j][1] = 8;
                break;
            case CCA_9:
                pmp->SATNdx[j][1] = 9;
                break;
            case CCA_VOL:
                pmp->SATNdx[j][1] = -1; /* whole carrier */
                break;
            default:
                pmp->SATNdx[j][1] = -2; /* main carrier DC */
                break;
            }
        }
    }
    pmp->pTPD = 2; /* it has been =1, don`t load more! */
    /*  } */
}

//Load data on phases and solution models to structure MULTI
//
void TProfil::multi_sys_ph()
{
    int k, i;
    short kk, j, je, jb;
    vstr pkey(MAXRKEYLEN);
    time_t crt;
    double G;
    double PMM;  /* average mol. phase  weight */
    short Cjs, car_l[32], car_c/*, cQ*/; /* current index carrier sorbent  */
    TPhase* aPH=(TPhase *)(&aMod[RT_PHASE]);
    aPH->ods_link(0);


    /* load data to phases */
    pmp->FIs = syp->Fis;
    pmp->FIa = 0;
    pmp->Lads = 0;

    for( kk=0, /*FI=0,*/ k=-1, /*jb=0,*/ je=0; kk<mup->Fi; kk++ )
    {
        if( syp->Pcl[kk] == S_OFF )
            continue;
        k++; // FI++;  //PMM = 0.0;
        pmp->L1[k] = syp->Ll[kk];
        jb = je;
        je += pmp->L1[k];
        pmp->muk[k] = kk;
        pmp->PHC[k] = mup->PHC[kk];
        memcpy( pmp->SF[k], mup->SF[kk], MAXSYMB );
        memcpy( pmp->SF[k]+MAXSYMB, mup->SF[kk]+MAXSYMB+MAXPHSYMB, MAXPHNAME );
        if( k < syp->Fis )
        {
            memcpy( pmp->SF2[k], mup->SF[kk], MAXSYMB );
            memcpy( pmp->SF2[k]+MAXSYMB, mup->SF[kk]+MAXSYMB+MAXPHSYMB, MAXPHNAME );
        }
        if( kk < mup->Fis && mup->Ll[kk] > 1 )
        { // load phase record
            /* ????????????????????????????? */  if( pmp->pNP && pmp->pBAL )
                goto PARLOAD;
            aPH->TryRecInp( mup->SF[kk], crt, 0 );
            // read informations from phase-solution
            memcpy( pmp->sMod[k], aPH->php->sol_t, 6 );
            if( aPH->php->Ppnc == S_ON )
                pmp->LsMod[k] = aPH->php->ncpN * aPH->php->ncpM;  /* ?error? */
            else pmp->LsMod[k] = 0;
            if( aPH->php->Psco == S_ON )
                pmp->LsMdc[k] = aPH->php->nscN * aPH->php->nscM;
            else pmp->LsMdc[k] = 0;
        }
        else  // nothing to read in phase record
            if( k<pmp->FIs )
            {
                pmp->LsMod[k] = 0;
                pmp->LsMdc[k] = 0;
                memset( pmp->sMod[k], ' ', 6 );
            }
PARLOAD:
        ;
        switch( pmp->PHC[k] )
        { // load parameters necessary
        case PH_AQUEL:
            break;
        case PH_GASMIX:
        case PH_PLASMA:
            pmp->PG = pmp->L1[k];
            break;
        case PH_HCARBL:
            pmp->PSOL = pmp->L1[k];
            break;
        case PH_SOLUTION:
        case PH_SINCOND:
        case PH_SIMELT:
            break;
        case PH_POLYEL:
        case PH_SORPTION:
            pmp->FIa++;
            ph_sur_param( k, kk );
            for(i=0; i<32; i++ )
                car_l[i]=-1;
            car_c=0; // cQ=0;
        case PH_SINDIS:
            if( syp->PAalp != S_OFF )
                pmp->Aalp[k] = syp->Aalp[kk];
            if( syp->PSigm != S_OFF )
            {
                pmp->Sigw[k] = syp->Sigm[kk][0];
                pmp->Sigg[k] = syp->Sigm[kk][1];
            }
            if( syp->PXepsC != S_OFF )
            {
                pmp->Xcond[k] = syp->XEpsC[kk][1];
                pmp->Xeps[k] = syp->XEpsC[kk][0];
            }
            if( syp->PXr0h0 != S_OFF )
            {
                pmp->Xr0h0[k][0] = syp->Xr0h0[kk][0];
                pmp->Xr0h0[k][1] = syp->Xr0h0[kk][1];
            }
            break;
        default:
            ; /* error! */
        }

        PMM = 0.0;
        Cjs = -1;
        // cycle by DC in phase, calc of it mean mol. mass
        for( j=jb; j<je; j++ )
        {
            switch( pmp->DCC[j] )
            { /* counters */
            case DC_AQ_SOLVENT:
                pmp->LO = j;
                PMM = pmp->MM[j];
                break;
            case DC_PEL_CARRIER:
            case DC_SUR_CARRIER:
                Cjs = j;
                PMM = pmp->MM[j];
                /*cQ=car_c;*/
                car_l[car_c++]=j;
                break;
            case DC_SUR_MINAL:
                car_l[car_c++]=j;
                break;
                /*   Remapping DC codes to indices in pm->SATNdx */
                /* even - strong surface complex on site type 0,1,2,3,4 - A plane */
            case DC_SSC_A0:
                pmp->SATNdx[j][0] = AT_SA0;
                pmp->Lads++;
                break;
            case DC_SSC_A1:
                pmp->SATNdx[j][0] = AT_SA1;
                pmp->Lads++;
                break;
            case DC_SSC_A2:
                pmp->SATNdx[j][0] = AT_SA2;
                pmp->Lads++;
                break;
            case DC_SSC_A3:
                pmp->SATNdx[j][0] = AT_SA3;
                pmp->Lads++;
                break;
            case DC_SSC_A4:
                pmp->SATNdx[j][0] = AT_SA4;
                pmp->Lads++;
                break;
                /* odd - weak surface complex on site type 0,1,2,3,4 - B plane */
            case DC_WSC_A0:
                pmp->SATNdx[j][0] = AT_SB0;
                pmp->Lads++;
                break;
            case DC_WSC_A1:
                pmp->SATNdx[j][0] = AT_SB1;
                pmp->Lads++;
                break;
            case DC_WSC_A2:
                pmp->SATNdx[j][0] = AT_SB2;
                pmp->Lads++;
                break;
            case DC_WSC_A3:
                pmp->SATNdx[j][0] = AT_SB3;
                pmp->Lads++;
                break;
            case DC_WSC_A4:
                pmp->SATNdx[j][0] = AT_SB4;
                pmp->Lads++;
                break;
                /* Strong exchange ion at const-charge plane */
            case DC_IESC_A:
                pmp->SATNdx[j][0] = AT_SA5;
                pmp->Lads++;
                break;
                /* Weak exchange ion at const-charge plane */
            case DC_IEWC_B:
                pmp->SATNdx[j][0] = AT_SB5;
                pmp->Lads++;
                break;
                /* Aliaces for 1-site model */
                /* Surface site A plane -> '0' */
            case DC_SUR_SITE:
                pmp->SATNdx[j][0] = AT_SA0;
                pmp->Lads++;
                break;
                /* Strong sur. complex A plane -> '0' */
            case DC_SUR_COMPLEX:
                pmp->SATNdx[j][0] = AT_SA0;
                pmp->Lads++;
                break;
                /* Weak sur complex B plane -> '1' */
            case DC_SUR_IPAIR:
                pmp->SATNdx[j][0] = AT_SB0;
                pmp->Lads++;
                break;
                /* End extension */
            case DC_SOL_MAJOR:
                Cjs = j;
            case DC_SOL_MINOR:
                PMM += pmp->MM[j];
                break;
            default: /* if( isdigit( pmp->DCC[j] ))
                                    pmp->Lads++;
                                  else */ 
                PMM += pmp->MM[j]; /* test it???? */
                break;
            }
        }  /* j */

        if( pmp->PHC[k] == PH_POLYEL || pmp->PHC[k] == PH_SORPTION )
            /* Analyzing assignment of surtypes */
            ph_surtype_assign( k, kk, jb, je, car_l, car_c, Cjs );

        if( syp->PYOF != S_OFF )
            switch( pmp->PHC[k] )
            { /* recalc free energi metastable of phase from ed./g to ed./mol */
            case PH_POLYEL:
            case PH_SORPTION:
                if( Cjs >= 0 )
                {
                    PMM = pmp->MM[Cjs];
                    pmp->FWGT[k] = PMM;
                }
                /*  leave normalized to 1 gram pmp->ln1Na[k] *= PMM; */
            case PH_AQUEL:
                pmp->YOF[k] = syp->YOF[kk] * PMM / pmp->RT;
                break;
            default:
                if( Cjs >=0 ) /* if have MAJOR-component of solutionа */
                    PMM = pmp->MM[Cjs];
                pmp->YOF[k] = syp->YOF[kk] * PMM / pmp->RT;
                break;
            }
        else pmp->YOF[k] = 0.0;

        for( j=jb; j<je; j++ )
        {
            G = Cj_init_calc( pmp->G0[j], j, k );
            /* Calculation of normalized values of G0 function */
            pmp->G0[j] = G;
        } /* j */
        // test of kinetic constraints (units of measurement!)
        if( pmp->PLIM && k >= pmp->FIs /* pmp->PPHk != S_OFF */ )
        { /* restrictions set! */
            /* one-component phases */
            if( syp->PLLim != S_OFF || syp->PULim != S_OFF )
            {
                pmp->RLC[pmp->Ls+k-pmp->FIs] = syp->RFLC[kk];
                pmp->RSC[pmp->Ls+k-pmp->FIs] = syp->RFSC[kk];
            }
            if( syp->PUL )
                pmp->DUL[pmp->Ls+k-pmp->FIs] = syp->PUL[kk];
            if( syp->PLL )
                pmp->DLL[pmp->Ls+k-pmp->FIs] = syp->PLL[kk];
        }
        if( pmp->PLIM && k < pmp->FIs )
            /*    ( syp->PLLim != S_OFF || syp->PULim != S_OFF ) )  */
        { /* solutions */
            if( syp->RFLC )
                pmp->RFLC[k] = syp->RFLC[kk];
            else pmp->RFLC[k] = NO_LIM;
            if( syp->RFSC )
                pmp->RFSC[k] = syp->RFSC[kk];
            else pmp->RFSC[k] = QUAN_MOL;
            if( syp->PUL )
                pmp->PUL[k] = syp->PUL[kk];
            else pmp->PUL[k] = 1e6;
            if( syp->PLL )
                pmp->PLL[k] = syp->PLL[kk];
            else pmp->PLL[k] = 0.0;
        }
        if( !pmp->PLIM && k<pmp->FIs )
        { /* no set restrictions */
            pmp->RFLC[k] = NO_LIM;
            pmp->RFSC[k] = QUAN_MOL;
            pmp->PUL[k] = 1e6;
            pmp->PLL[k] = 0.0;
        }

        /* Кинетическая фиксация фазы - не сделано !!!*
        * Надо знать состав фазы-раствора *;
        * syp->XFk[kk] *
        * Проверка на кинетическую фиксацию *
             if( syp->PPHk != S_OFF )
             {
                 if( sy->Lk )
                 { int jk;
                   for( jk=ZERO; jk<sy->Lk; jk++ )
                   if( sy->llk[jk] == jm )
                   {  if( !bc->VG && bc->Xk )
                       if( bc->Xk[jk] && !ISEMPTY( bc->Xk[jk] ))
                         pmp->VG[jp] = bc->Xk[jk] + bc->Xk[jk]*sp->DKIN;
                       else pmp->VG[jp] = 1e6;
                      else if( bc->VG && bc->VG[jk] > 1e-18 )
                             pmp->VG[jp] = bc->VG[jk];
                          else pmp->VG[jp] = (float)tc_DCmaxm;
                      if( !bc->NG && bc->Xk )
                      { pmp->NG[jp] = bc->Xk[jk] - bc->Xk[jk]*sp->DKIN;
                       * if( pmp->NG[jp] < sp->DKIN )
                         pmp->NG[jp] = sp->DKIN; *
                      }
                      else if( bc->NG && bc->NG[jk] > F_EMPTY )
                            pmp->NG[jp] = bc->NG[jk];
                          else pmp->NG[jp] = DZERO;
                   }
                 }
                 if( pmp->VG[jp] < 1e-15 ) pmp->VG[jp] = (float)tc_DCmaxm;
             }
        */
    }  /* k */
    pmp->pTPD = 2;
}

// Loading surface type parameters
void TProfil::ph_sur_param( int k, int kk )
{
    int i;

    for( i=0; i< syp->NsTm; i++ )
    {
        if( syp->SCMT[kk][i] == SC_NOT_USED || fabs(syp->Nfsp[kk][i])<1e-9 )
        { /*this type surface of dont used*/
            pmp->Nfsp[k][i] =  FLOAT_EMPTY;
            pmp->MASDT[k][i] = FLOAT_EMPTY;
            //    pmp->MISDT[k][i] = FLOAT_EMPTY;
            pmp->XcapA[k][i] = FLOAT_EMPTY;
            pmp->XcapB[k][i] = FLOAT_EMPTY;
            pmp->Xetaf[k][i] = FLOAT_EMPTY;
            pmp->SCM[k][i] = SC_NOT_USED;
            /*  pmp->CSTS[k][i] = CCA_VOL; */
            continue;
        }
        pmp->SCM[k][i] = syp->SCMT[kk][i];
        /*  pmp->CSTS[k][i] = syp->CSTS[kk][i]; */
        pmp->Nfsp[k][i] = syp->Nfsp[kk][i];  /* fraction! */

        /* (1e18/m2 m2/g)/(6.022e23 1/mol)=1.66054 mkmol/g */

        if( !syp->MaSdt || syp->MaSdt[kk][i] < 1e-9 )
            pmp->MASDT[k][i] = pa.p.DNS * 1.66054 * syp->Aalp[kk];
        /* default, mkmol/g */
        else pmp->MASDT[k][i] = syp->MaSdt[kk][i]*syp->Aalp[kk]*1.66054;
        /* max. in mkmol/g */

        // if( !syp->MiSdt || syp->MiSdt[kk][i] < 1e-9 )
        //     pmp->MISDT[k][i] = 1.0; /* default, mkmol/g */
        //  else pmp->MISDT[k][i] = syp->MiSdt[kk][i]*syp->Aalp[kk]*1.66054;
        /* min. in mkmol/g */
        /*
          pmp->XCEC[k][i] = syp->Nsph[kk][i] * syp->Aalp[kk]*
                               syp->Nfsp[kk][i]*1.6606;        mkmol/g */

        if( syp->PXfIEC != S_OFF )
        {  /* Permanent surface charge density Ion-exchange capacity */
           // XfIEC[] in mkeq/m2; Xetaf[] in C/m2
            if( fabs(syp->XfIEC[kk][i])>1e-9 )
                pmp->Xetaf[k][i] = syp->XfIEC[kk][i]*.09648531;
            else pmp->Xetaf[k][i] = 0.0;
        }
        if( syp->PXcapA != S_OFF )
            pmp->XcapA[k][i] = syp->XcapA[kk][i];
        else pmp->XcapA[k][i] = 0.;
        if( syp->PXcapB != S_OFF )
            pmp->XcapB[k][i] = syp->XcapB[kk][i];
        else pmp->XcapB[k][i] = 0.;

        pmp->XdlA[k][i] = 0.0;
        pmp->XdlB[k][i] = 0.0;
        pmp->XdlD[k][i] = 0.0;
        pmp->XcapD[k][i] = 0.;
        /*
          if( syp->PXlam != S_OFF )
            pmp->XlamA[k][i] = syp->Xlam[kk][i];
          else pmp->XlamA[k][i] = 0.;
          if( syp->PXetf != S_OFF )
              pmp->Xetaf[k][i] = syp->Xetaf[kk][i];
          else pmp->Xetaf[k][i] = 0.;
        */
    } /* end for i */
}

// Assigning surface types to carrier (DC) - added 5/13/97 DAK
void TProfil::ph_surtype_assign( int k, int kk, int jb, int je,
                                 short car_l[], int car_c, short Cjs )
{
    int j, jcl, ist;
    /*  double SATst; */

    for( j=jb; j<je; j++ )
    {
        if( pmp->DCC[j] == DC_PEL_CARRIER || pmp->DCC[j] == DC_SUR_CARRIER
                || pmp->DCC[j] == DC_SUR_MINAL )
            continue;

        ist = pmp->SATNdx[j][0] / MSPN;   /* index of surtype */

        jcl = pmp->SATNdx[j][1];
        if( jcl < car_c && jcl >= 0 )
            pmp->SATNdx[j][1] = car_l[jcl];
        else if( jcl >= car_c || jcl == -1 )
            pmp->SATNdx[j][1] = -1;   /* whole carrier */
        else pmp->SATNdx[j][1] = Cjs; /* main carrier DC */

        if( pmp->SATT[j] != SAT_COMP )
            pmp->MASDJ[j] *= syp->Aalp[kk]*1.66054;  /* mkmol/g */
        if( !pmp->MASDJ[j] )
        {
            //      if( pmp->SATT[j] == SAT_SITE )
            //         pmp->MASDJ[j] = pmp->MASDT[k][ist] - pmp->MISDT[k][ist] ;
            if( pmp->SATT[j] == SAT_NCOMP )
                pmp->MASDJ[j] = pmp->MASDT[k][ist];
            if( pmp->SATT[j] == SAT_COMP )
                pmp->MASDJ[j] = 1.0;
        }
        /*  if( pmp->SATT[j] == SAT_SITE )
            { * Correction for neutr.site at Г0,max *
              SATst = pa->p.DNS*1.66054*pmp->Aalp[k]/
                 max( pmp->MASDT[k][ist]-pmp->MISDT[k][ist], pmp->MASDJ[j] );
              pmp->GEX[j] += log( SATst );
            } */
    } /* for j */
}

// Convert class codes of DC into general codes of IPM
void TProfil::ConvertDCC()
{
    int i, j, k, iRet=0;
    char DCCW;

    j=0;
    for( k=0; k< pmp->FI; k++ )
    { /* cycle by phases  */
        i=j+pmp->L1[k];
        if( pmp->L1[k] == 1 )
        {
            pmp->DCCW[j] = DC_SINGLE;
            goto NEXT_PHASE;
        }
        for( ; j<i; j++ )
        { /* cykle by DC. */
            switch( pmp->DCC[j] ) /* selection necessary expressions  v_j */
            {
            case DC_SCP_CONDEN:
                DCCW = DC_SINGLE;
                break;
            case DC_GAS_COMP:
            case DC_GAS_H2O:
            case DC_GAS_CO2:
            case DC_GAS_H2:
            case DC_GAS_N2:
            case DC_SOL_IDEAL:
            case DC_SOL_MINOR:
            case DC_SOL_MAJOR:
                DCCW = DC_SYMMETRIC;
                break;
            case DC_AQ_PROTON:
            case DC_AQ_ELECTRON:
            case DC_AQ_SPECIES:
                DCCW = DC_ASYM_SPECIES;
                break;
            case DC_AQ_SOLVCOM:
            case DC_AQ_SOLVENT:
                DCCW = DC_ASYM_CARRIER;
                break;
            case DC_IESC_A:
            case DC_IEWC_B:
                DCCW = DC_ASYM_SPECIES;
                break;
                /* Remapping */
            case DC_SUR_SITE:
            case DC_SUR_COMPLEX:
                DCCW = DC_ASYM_SPECIES;
                pmp->DCC[j] = DC_SSC_A0;
                break;
            case DC_SUR_IPAIR:
                DCCW = DC_ASYM_SPECIES;
                pmp->DCC[j] = DC_WSC_A0;
                break;
            case DC_SUR_MINAL:
            case DC_SUR_CARRIER:
            case DC_PEL_CARRIER:
                DCCW = DC_ASYM_CARRIER;
                break;
            default:
                if( isdigit( pmp->DCC[j] ))
                {
                    if( pmp->PHC[k] == PH_SORPTION ||
                            pmp->PHC[k] == PH_POLYEL )
                    {
                        DCCW = DC_ASYM_SPECIES;
                        break;
                    }
                }
                DCCW = DC_SINGLE;
                iRet++;  /* error in codeа */
            }
            pmp->DCCW[j] = DCCW;
        }   /* j */
NEXT_PHASE:
        j = i;
    }  /* k */
    ErrorIf( iRet>0, "Multi", "Error in DCC code.");
}

// Calc value cj (G0) by type of DC and standart value g(T,P)
// k - index of phase, j - index DC in phase
// if error code return 777777777.
double TProfil::Cj_init_calc( double g0, int j, int k )
{
    double G, YOF;

    G = g0/pmp->RT;
    /*  if( k < pmp->FIs )  */
    YOF = pmp->YOF[k];
    /* єўхёЄ№ хфшэшЎ√ шчьхЁхэш !!!!! */
    switch( pmp->DCC[j] )
    { /* Aqueous electrolyte */
    case DC_AQ_PROTON:
    case DC_AQ_ELECTRON:
    case DC_AQ_SPECIES:
        G += pmp->ln5551;
        /* calc mol weight of solvent !!!!!!!!!!!!!!!!!!!!!!!!!!! */
    case DC_AQ_SOLVCOM:
    case DC_AQ_SOLVENT:
        if( syp->PYOF != S_OFF )
            pmp->GEX[j] += YOF;
        break;
        /* as phase- test add ln P general !!!!!!!!!!!!!!!!!!!!!! */
    case DC_GAS_COMP: /* gases exept H2O and CO2 */
    case DC_GAS_H2O: /* index to switch off? */
    case DC_GAS_CO2:
    case DC_GAS_H2:
    case DC_GAS_N2:
        if( pmp->Pparc[j] != 1.0 && pmp->Pparc[j] > 1e-30 )
            G += log( pmp->Pparc[j] );
        /* Solution non-electrolyte */
    case DC_SCP_CONDEN: /*a single-component phase */
    case DC_SOL_IDEAL:
    case DC_SOL_MINOR:
    case DC_SOL_MAJOR:
    case DC_SUR_MINAL:
    case DC_SUR_CARRIER:
    case DC_PEL_CARRIER:
        if( syp->PYOF != S_OFF )
            pmp->GEX[j] += YOF;
        break;
        /* Sorption phases */
    case DC_SSC_A0:
    case DC_SSC_A1:
    case DC_SSC_A2:
    case DC_SSC_A3:
    case DC_SSC_A4:
    case DC_WSC_A0:
    case DC_WSC_A1:
    case DC_WSC_A2:
    case DC_WSC_A3:
    case DC_WSC_A4:
    case DC_SUR_SITE:
    case DC_SUR_COMPLEX:
    case DC_SUR_IPAIR:
    case DC_IESC_A:
    case DC_IEWC_B:
        G += pmp->ln5551;
        break;
    default: /* error code */
        return 7777777.;
    }
    return G += pmp->GEX[j];
}

// End of file ms_mupmph.cpp
// --------------------------------------------------------------------
