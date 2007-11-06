//-------------------------------------------------------------------
// $Id$
//
// (c) 1992-2007 S.Dmitrieva, D.Kulik, K.Chudnenko, I.Karpov
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
#include "s_formula.h"
#include "service.h"

// Aseembling base GEM-IPM structure to calculate equilibrium states
//
// 1) Makes full  stoichiometry matrix A using the sy.Dcl list
// 2) Looks through Phase records and loads data into mixing model arrays
//    (optionally translating non-ideality Phase scripts)
// 3) Continues looking through one-component Phases, to seek for
//    specific surfaces and related surface parameters
// 4) Terforms necessary re-scaling and conversions of data
// 5) Tests whether it is possible to take old primal (x, gamma) and dual
//   (u) solutions as an initial approximation
//
void TMulti::MultiRemake( const char *key )
{
    int i, N;
    short ii;
    vstr pkey(MAXRKEYLEN);
    double VS;
    SPP_SETTING *pa = &TProfil::pm->pa;

    memcpy( pmp->stkey, key, EQ_RKLEN );
    pmp->stkey[EQ_RKLEN] = '\0';
//  setting array sizes in MULTI data structure
    pmp->PunE = tpp->PunE;
    pmp->PunV = tpp->PunV;
    pmp->PunP = tpp->PunP;
    pmp->PunT = tpp->PunT;
    pmp->N =    syp->N;
    pmp->NR =    syp->N; // 20/02/2007
    pmp->L =    syp->L;
    pmp->Ls = syp->Ls;
    pmp->LO =   syp->Lw/*-1*/;
    pmp->PG =   syp->Lg;
    pmp->PSOL = syp->Lhc;
    pmp->Lads = syp->Lsor;
    pmp->FI = syp->Fi;
    pmp->FIs = syp->Fis;
//   pmp->FIa = syp->N;
    pmp->FI1 = 0;
    pmp->FI1s = 0;
    pmp->FI1a = 0;
    if( syp->DLLim == S_ON || syp->DULim == S_ON )
        pmp->PLIM = 1;
    else pmp->PLIM = 0;

    pmp->IT = 0; pmp->ITF = 0; pmp->ITG = 0;
    if( syp->PE != S_OFF )
        pmp->E = 1;
    else pmp->E = 0;
    if( syp->NsTm > 0 && syp->PNfsp != S_OFF )
        pmp->FIat = MST;
    else pmp->FIat = 0;
    pmp->PD = pa->p.PD;

    pmp->T = pmp->Tc = tpp->T + C_to_K;
    pmp->TC = pmp->TCc = tpp->T;
    pmp->P = pmp->Pc = tpp->P;
    pmp->T0 = 273.15;    // not used at present
    pmp->MBX = syp->MBX;
    pmp->FX = 7777777.;
    pmp->pH = pmp->Eh = pmp->pe = 0.0;
    pmp->GWAT = syp->Mwat;
    pmp->YMET = 0;
    pmp->PCI = 0.0;

// setting volume balance constraints, if necessary
    memcpy( pkey, pmp->stkey+48-MAXSYWHAT, MAXPTN );
    pkey[MAXPTN] = 0;
    VS = atof( pkey );
    if( VS <= 0 ) // no volume balance needed
    {
        pmp->VX_ = pmp->VXc = pmp->VE = 0.0;
        pmp->PV = 0;
    }
    else
    {  // volume of the system is given
        pmp->VX_ = pmp->VXc = pmp->VE = VS;
        pmp->PV = syp->PV;
    }

    pmp->Ec = pmp->K2 = pmp->MK = 0;
    pmp->PZ = pa->p.DW; // IPM-2 default
    pmp->W1 = 0;
    pmp->is = 0;
    pmp->js = 0;
    pmp->next  = 0;
    pmp->IC =  syp->Mbel;
    pmp->FitVar[0] = pa->aqPar[0]; // setting T,P dependent b_gamma parameters
    pmp->denW = tpp->RoW;
    pmp->denWg = tpp->RoV;
    pmp->epsW = tpp->EpsW;
    pmp->epsWg = tpp->EpsV;
    pmp->GWAT = syp->Mwat * 55.508373;

    pmp->RT = tpp->RT;  // R_CONSTANT * pmp->Tc
    pmp->FRT = F_CONSTANT/pmp->RT;

    pmp->ln5551 = 4.0165339;
    pmp->lowPosNum = pa->p.DcMin;
    pmp->logXw = -16.;
    pmp->logYFk = -9.;
///    pmp->lnP = 0.;
///    if( tpp->P != 1. )  // non-reference pressure
///        pmp->lnP = log( tpp->P );
    pmp->DX = pa->p.DK;

    pmp->FitVar[4] = pa->p.AG;
    pmp->pRR1 = 0;   // IPM smoothing factor and level

    // Reallocating memory, if necessary
    if( !pmp->pBAL )
        dyn_new();
    if( pmp->pBAL == 2 )
         goto NEXT2;

    // loading parameters for ICs (independent components)
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
    }
//   ErrorIf( N != pmp->N, "Multi", "Multi make error: N != pmp->N" );
    if( N != pmp->N )
        vfMessage( window(),
                   "Multi make error: N != pmp->N",
		   "Please, press the BCC button first!" );
NEXT2:
    // loading data for DCs (dependent components)
    multi_sys_dc();

    // loading data for phases, incl. sorption and solution models
    multi_sys_ph();

    if( pmp->pBAL < 2 )
        ConvertDCC(); // Loading generic species codes

   if(  !pmp->pBAL || pmp->pTPD < 2)
      CompG0Load();

    // Tests on integrity of CSD can be added here
     pmp->pBAL = 2;
}

//Load data for DC from Modelling Project definition to MULTI structure
//
void TMulti::multi_sys_dc()
{
    int j, ii, L, iZ=0;
    short jj, jja, ja, kk;
    float a, *A, Vv =0.;
    double mm;
    TIArray<TFormula> aFo;
    gstring form;

    ErrorIf( !tpp->G, "Multi", "Multi make error: !tpp->G" );
///    int xVol = 0;   // SD 09/02/2007
///    if( pmp->PV == VOL_CONSTR )
///      xVol = getXvolume();

    if( !pmp->pBAL )
    {  // making full stoichiometry matrix from DC formula list
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
        // loading data for dependent components
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
            if( j < syp->Ls && j >= syp->Ls - syp->Lsor)
            {   // assembling DC name list for sorption surface species
                ja = (short)(j-(syp->Ls-syp->Lsor));
                memcpy( pmp->SM3[ja], mup->SM[jj]+MAXSYMB+MAXDRGROUP, MAXDCNAME );
               // surface species DC class code (usage will be obsolete)
                pmp->DCC3[ja] = mup->DCC[jj];
            }
            mm = 0.0;
            for( ii=0; ii<pmp->N; ii++ ) // compressing the stoichiometry matrix
            {
                a = A[jj*mup->N + pmp->mui[ii]];
                pmp->A[j*pmp->N+ii] = a;
                mm += a * pmp->Awt[ii];   // calculating molar mass of DC
            }
            pmp->MM[j] = mm;
        }
        delete[] A;
        // A = 0;
//     ErrorIf( L != pmp->L, "Multi", "Multi make error: L != pmp->L" );
        if( L != pmp->L )
            vfMessage(  window(), "Multi make error: L != pmp->L", "Please, press BCC!" );

    }

    if( pmp->E )
    {   // index of charge
        for(  ii=0; ii<pmp->N; ii++ )
            if( pmp->ICC[ii] == IC_CHARGE )
                goto CH_FOUND;
        // error no charge
        if( vfQuestion(window(), "Multi", "No DC with formula charge are included:\n"
                       "Proceed (Y), Cancel (N)?"  ))
        {    pmp->E = 0;
             syp->PE=S_OFF;  // Change Sveta 14/10/2002
        }
        else
            Error( "Multi","No DC with formula charge are included" );
CH_FOUND:
        iZ = ii;
    }

    for( j=-1, jj=0; jj<mup->L; jj++ ) // Main loop for data loading
    {
        if( syp->Dcl[jj] == S_OFF )
            continue;
        j++;
        pmp->DCC[j] = mup->DCC[jj];
        if( pmp->E )
            pmp->EZ[j] = pmp->A[pmp->N*j+iZ];
        pmp->Pparc[j] = pmp->Pc;
        pmp->F0[j] = 0.0;
        pmp->XY[j] = 0.0;
        pmp->G0[j] = tpp->G[jj];

///        if( tpp->PtvVm == S_ON )
///            switch( pmp->PV )
///            { // calculating actual molar volume of DC
///            case VOL_CONSTR:
///              if( syp->Vuns )
///                       Vv = syp->Vuns[jj];
///                pmp->A[j*pmp->N+xVol] = tpp->Vm[jj] + Vv;
///            case VOL_CALC:
///            case VOL_UNDEF:
///                if( syp->Vuns )
///                       Vv = syp->Vuns[jj];
///                pmp->Vol[j] = ( tpp->Vm[jj] + Vv )* 10.; // Check!
///               break;
///            }
///        else pmp->Vol[j] = 0.0;

// testing kinetic/metastability constraints
        if( pmp->PLIM && jj < mup->L )
        { // KM constraints necessary!
            pmp->RLC[j] = syp->RLC[jj];
            pmp->RSC[j] = syp->RSC[jj];
            if( syp->DUL )
                pmp->DUL[j] = syp->DUL[jj];
            else pmp->DUL[j] = 1e6;
            ja = (short)(j-(pmp->Ls-pmp->Lads));
            if( pmp->lnSAC && ja < pmp->Lads && ja >= 0 )
               pmp->lnSAC[ja][3] = pmp->DUL[j]; // Copy of DUL for SACT refining
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
// !!!!!!!!!!!!!!!!!!!!!! Insert here a case that checks units of
//    measurement for the G0 increment
             pmp->GEX[j] = syp->GEX[jj]/pmp->RT;
        else pmp->GEX[j] = 0.0;  // Standard free energy increments

        if( syp->PParc != S_OFF )
            pmp->Pparc[j] = syp->Pparc[jj];
        pmp->lnGmM[j] = 0.0;
        if( jj < mup->Ls && syp->PlnGf != S_OFF )
        {
            pmp->lnGmf[j] = syp->lnGmf[jj];
            if( !TProfil::pm->pa.p.PSM )  // Constant activity coefficients
                pmp->lnGmM[j] = syp->lnGmf[jj];
        }
        else pmp->lnGmf[j] = 0.0;

        if( !( j < pmp->Ls && j >= pmp->Ls - pmp->Lads ) )
            continue;   // the following is not done for non-surface species

        ja = (short)(j-(pmp->Ls-pmp->Lads));
        jja = (short)(jj-(mup->Ls-mup->Lads));
// Loading MaSdj - max.sur.densities for non-competitive sorbates */
        if( syp->PMaSdj != S_OFF )
        {
            for( kk=0; kk<DFCN; kk++ )
               if( !IsFloatEmpty( syp->MaSdj[jja][kk] ) )
                   pmp->MASDJ[ja][kk] = syp->MaSdj[jja][kk];
               else pmp->MASDJ[ja][kk]= 0.0;
        }
        if( syp->PSATT != S_OFF )
        { // Loading SATC - codes of methods for SAT calculation
            pmp->SATT[ja] = syp->SATC[jja][SA_MCA];

         // Loading allocation to surface types
            switch( syp->SATC[jja][SA_STX] )
            {
              default:
              case CST_0:
                pmp->SATX[ja][XL_ST] = 0;
                break;
              case CST_1:
                pmp->SATX[ja][XL_ST] = 1;
                break;
              case CST_2:
                pmp->SATX[ja][XL_ST] = 2;
                break;
              case CST_3:
                pmp->SATX[ja][XL_ST] = 3;
                break;
              case CST_4:
                pmp->SATX[ja][XL_ST] = 4;
                break;
              case CST_5:
                pmp->SATX[ja][XL_ST] = 5;
                break;
            }

            // Loading allocation to carrier end-members
            switch( syp->SATC[jja][SA_EMX] )
            {
              case CCA_0:
                pmp->SATX[ja][XL_EM] = 0;
                break;
              case CCA_1:
                pmp->SATX[ja][XL_EM] = 1;
                break;
              case CCA_2:
                pmp->SATX[ja][XL_EM] = 2;
                break;
              case CCA_3:
                pmp->SATX[ja][XL_EM] = 3;
                break;
              case CCA_4:
                pmp->SATX[ja][XL_EM] = 4;
                break;
              case CCA_5:
                pmp->SATX[ja][XL_EM] = 5;
                break;
//              case CCA_6:
//                pmp->SATX[ja][XL_EM] = 6;
//                break;
//              case CCA_7:
//                pmp->SATX[ja][XL_EM] = 7;
//                break;
//              case CCA_8:
//                pmp->SATX[ja][XL_EM] = 8;
//                break;
//              case CCA_9:
//                pmp->SATX[ja][XL_EM] = 9;
//                break;
              case CCA_VOL:
                pmp->SATX[ja][XL_EM] = -1; // whole sorbent
                break;
              default:
                pmp->SATX[ja][XL_EM] = -2; // main DC of the sorbent
                break;
            }

            // Loading allocation to surface sites
            switch( syp->SATC[jja][SA_SITX] )
            {
              default:
              case CSI_0:
                pmp->SATX[ja][XL_SI] = 0;
                break;
              case CSI_1:
                pmp->SATX[ja][XL_SI] = 1;
                break;
              case CSI_2:
                pmp->SATX[ja][XL_SI] = 2;
                break;
              case CSI_3:
                pmp->SATX[ja][XL_SI] = 3;
                break;
              case CSI_4:
                pmp->SATX[ja][XL_SI] = 4;
                break;
              case CSI_5:
                pmp->SATX[ja][XL_SI] = 5;
                break;
            }
            // Loading charge allocation to surface planes
            switch( syp->SATC[jja][SA_PLAX] )
            {
              default:
              case SPL_0:   // only to 0 plane (sorbent surface plane)
                   pmp->SATX[ja][XL_SP] = 0;
                   break;
              case SPL_1:
              case SPL_B:   // to beta plane or plane 1 in CD 3-layer
                   pmp->SATX[ja][XL_SP] = 1;
                   break;
              case SPL_2:
              case SPL_D:   // to DL plane or plane 2 in CD 3-layer
                   pmp->SATX[ja][XL_SP] = 2;
                   break;
              case SPL_3:
              case SPL_C:   // to OS anion beta plane (4-layer model, reserved)
                   pmp->SATX[ja][XL_SP] = 3;
                   break;
            }
        }
    }
}

//Loading data for phases and mixing models into the structure MULTI
//
void TMulti::multi_sys_ph()
{
    int k, i;
    short kk, j, je, jb, ja=0;
    vstr pkey(MAXRKEYLEN);
    time_t crt;
    double G;
    double PMM;  // Phase mean mol. mass
    short Cjs, car_l[32], car_c; // current index carrier sorbent
    TPhase* aPH=(TPhase *)(&aMod[RT_PHASE]);
    aPH->ods_link(0);

    pmp->Ls = 0;
    pmp->PG = 0;
    pmp->FIs = syp->Fis;
    pmp->FIa = 0;
    pmp->Lads = 0;   // Is this really needed?
//    ja=0;
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
            {
               pmp->LsMod[k*3] = aPH->php->ncpN;
               pmp->LsMod[k*3+2] = aPH->php->ncpM;
               pmp->LsMod[k*3+1] = aPH->php->npxM;
            }
            else pmp->LsMod[k*3] = pmp->LsMod[k*3+1] = pmp->LsMod[k*3+2] =0;
            if( aPH->php->Psco == S_ON )
                pmp->LsMdc[k] = aPH->php->nscM;
            else pmp->LsMdc[k] = 0;
        }
        else  // nothing to read in the Phase record
            if( k<pmp->FIs )
            {
                pmp->LsMod[k*3] = 0;
                pmp->LsMod[k*3+1] = 0;
                pmp->LsMod[k*3+2] = 0;
                pmp->LsMdc[k] = 0;
                memset( pmp->sMod[k], ' ', 6 );
            }
PARLOAD: if( k < syp->Fis )
             pmp->Ls += pmp->L1[k];
        switch( pmp->PHC[k] )
        { // loading necessary parameters
        case PH_AQUEL:
             break;
        case PH_GASMIX:
        case PH_PLASMA:
        case PH_FLUID:
//            pmp->PG = pmp->L1[k];
              pmp->PG += pmp->L1[k];
            break;
        case PH_HCARBL:
            pmp->PSOL = pmp->L1[k];
            break;
        case PH_LIQUID:
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
/*           if( syp->PAalp != S_OFF )
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
            }     */
            break;
        default:
            ; // Error message may be needed here!
        }
        // Moved by DK on 25.07.2006
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
        PMM = 0.0;
        Cjs = -1;
        // Loop over DCs in Phase, calculation of phase mean mol. mass
        for( j=jb; j<je; j++ )
        {
            switch( pmp->DCC[j] )
            {
            case DC_AQ_SOLVENT:
                pmp->LO = j;
                PMM = pmp->MM[j];
                break;
            case DC_PEL_CARRIER:
            case DC_SUR_CARRIER:
                Cjs = j;
                PMM = pmp->MM[j];
                car_l[car_c++]=j;  pmp->Lads++; ja++;
                break;
            case DC_SUR_MINAL:
                car_l[car_c++]=j;  pmp->Lads++; ja++;
                break;
  /* Remapping DC codes to indices in pm->SATX - compat with old-style SCMs *
   * even - inner-sphere complex on surface type 0,1,2,3,4 - A plane */
            case DC_SSC_A0:
//                pmp->SATX[ja][XL_ST] = AT_SA0;
                pmp->Lads++; ja++;
                break;
            case DC_SSC_A1:
//                pmp->SATX[ja][XL_ST] = AT_SA1;
                pmp->Lads++; ja++;
                break;
            case DC_SSC_A2:
//                pmp->SATX[ja][XL_ST] = AT_SA2;
                pmp->Lads++; ja++;
                break;
            case DC_SSC_A3:
//                pmp->SATX[ja][XL_ST] = AT_SA3;
                pmp->Lads++; ja++;
                break;
            case DC_SSC_A4:
//                pmp->SATX[ja][XL_ST] = AT_SA4;
                pmp->Lads++; ja++;
                break;
  /* odd - outer-sphere complex on surface type 0,1,2,3,4 - beta plane */
            case DC_WSC_A0:
//                pmp->SATX[ja][XL_ST] = AT_SB0;
                pmp->Lads++; ja++;
                break;
            case DC_WSC_A1:
//                pmp->SATX[ja][XL_ST] = AT_SB1;
                pmp->Lads++; ja++;
                break;
            case DC_WSC_A2:
//                pmp->SATX[ja][XL_ST] = AT_SB2;
                pmp->Lads++; ja++;
                break;
            case DC_WSC_A3:
//                pmp->SATX[ja][XL_ST] = AT_SB3;
                pmp->Lads++; ja++;
                break;
            case DC_WSC_A4:
//                pmp->SATX[ja][XL_ST] = AT_SB4;
                pmp->Lads++; ja++;
                break;
  /* Inner-sphere exchange ion at const-charge plane */
            case DC_IESC_A:
//                pmp->SATX[ja][XL_ST] = AT_SA5;
                pmp->Lads++; ja++;
                break;
  /* Outer-sphere exchange ion at const-charge plane */
            case DC_IEWC_B:
//                pmp->SATX[ja][XL_ST] = AT_SB5;
                pmp->Lads++; ja++;
                break;
  /* Aliaces for single-site model *
        * Surface site A plane -> '0' */
            case DC_SUR_GROUP:
//                pmp->SATX[ja][XL_ST] = AT_SA0;
                pmp->Lads++; ja++;
                break;
        /* Strong sur. complex A plane -> '0' */
            case DC_SUR_COMPLEX:
//                pmp->SATX[ja][XL_ST] = AT_SA0;
                pmp->Lads++; ja++;
                break;
       /* Weak sur complex B plane -> '1' */
            case DC_SUR_IPAIR:
//                pmp->SATX[ja][XL_ST] = AT_SB0;
                pmp->Lads++; ja++;
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
                PMM += pmp->MM[j]/pmp->L1[k]; /* test it last change 4.12.2006 */
                break;
            }
        }  /* j */

        if( pmp->PHC[k] == PH_POLYEL || pmp->PHC[k] == PH_SORPTION )
            /* Analyzing assignment of surtypes */
            ph_surtype_assign( k, kk, jb, je, car_l, car_c, Cjs );

        if( syp->PYOF != S_OFF )
            switch( pmp->PHC[k] )
            { // convert free energy of metastable phase from J/g to J/mol */
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
                if( Cjs >=0 ) // if there is a MAJOR component in the solutionï¿½
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

    }  /* k */
}

// Loading surface type parameters
void TMulti::ph_sur_param( int k, int kk )
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
            pmp->MASDT[k][i] = TProfil::pm->pa.p.DNS * 1.66054 * syp->Aalp[kk];
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
// Extended for CD-MUSIC by KD 31.10.2004
void TMulti::ph_surtype_assign( int k, int kk, int jb, int je,
                                 short car_l[], int car_c, short Cjs )
{
    int j, jcl, ist/*=0*/, isi/*=0*/, ja/*=0*/;
    /*  double SATst; */

    for( j=jb; j<je; j++ )
    {
        if( pmp->DCC[j] == DC_PEL_CARRIER || pmp->DCC[j] == DC_SUR_CARRIER
                || pmp->DCC[j] == DC_SUR_MINAL )
            continue;
        ja = j - ( pmp->Ls - pmp->Lads );
ist = pmp->SATX[ja][XL_ST]; // / MSPN;   index of surface type
isi = pmp->SATX[ja][XL_SI];                 // index of site on this surface type
pmp->D[ist][isi] = 0.0;                    // cleanining the totals for sites
        jcl = pmp->SATX[ja][XL_EM];
        if( jcl < car_c && jcl >= 0 )
            pmp->SATX[ja][XL_EM] = car_l[jcl];
        else if( jcl >= car_c || jcl == -1 )
            pmp->SATX[ja][XL_EM] = -1;   /* whole carrier */
        else pmp->SATX[ja][XL_EM] = Cjs; /* main carrier DC */

        if( pmp->SATT[ja] != SAT_COMP )
// To be fixed !!!!!!!!!!!
            pmp->MASDJ[ja][PI_DEN] *= syp->Aalp[kk]*1.66054;  /* 1/nm2 to mkmol/g */
//            pmp->MASDJ[j] *= syp->Aalp[kk]*1.66054;  /* mkmol/g */
        if( !pmp->MASDJ[ja] )
        {
            //      if( pmp->SATT[j] == SAT_SITE )
            //         pmp->MASDJ[j] = pmp->MASDT[k][ist] - pmp->MISDT[k][ist] ;
            if( pmp->SATT[ja] == SAT_NCOMP )
                pmp->MASDJ[ja][PI_DEN] = pmp->MASDT[k][ist]; // Do we really need this?
//              pmp->MASDJ[j] = pmp->MASDT[k][ist];
            if( pmp->SATT[ja] == SAT_COMP )
                pmp->MASDJ[ja][PI_DEN] = 1.0;
//                pmp->MASDJ[j] = 1.0;
        }
        /*  if( pmp->SATT[j] == SAT_SITE )
            { * Correction for neutr.site at ï¿½0,max *
              SATst = pa->p.DNS*1.66054*pmp->Aalp[k]/
                 max( pmp->MASDT[k][ist]-pmp->MISDT[k][ist], pmp->MASDJ[j] );
              pmp->GEX[j] += log( SATst );
            } */
    } /* for j */
}


// End of file ms_mupmph.cpp
// --------------------------------------------------------------------

