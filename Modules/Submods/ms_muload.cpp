//-------------------------------------------------------------------
// $Id: ms_muload.cpp 1156 2008-12-15 17:50:38Z gems $
//
// (c) 1992-2013 S.Dmitrieva, D.Kulik, K.Chudnenko
//
// Implementation of parts of the Interior Points Method (IPM) module
// for convex programming Gibbs energy minimization, described in:
// (Karpov, Chudnenko, Kulik (1997): American Journal of Science
//  v.297 p. 767-806)
//
// This file is part of a GEM-Selektor (GEMS) v.3.1.x program
// environment for thermodynamic modeling in geochemistry
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
#include <cstring>

#include "tmltsystem.h"
#include "m_syseq.h"
#include "v_object.h"
#include "s_formula.h"
#include "service.h"
#include "ms_rmults.h"
#include "ms_system.h"


// New: Loading parameters for kinetics-metastability models of all
// phases into structure for calculations of equlibria (added by DK on 13.03.2013)
//
void TMultiSystem::KinMetModLoad( )
{
    int kk, k, j, jj, kf, kfe=0, kp, kpe=0, ka, kae=0, ks, kse=0,
    JB, JE=0, jb, je=0, kc, kd, kce=0, kde=0, kx, kxe=0, ki, kie=0;
    long int jphl=0, jlphc=0;
    //vstr pkey(MAXRKEYLEN);
    vstr modT(164);
    char *kMod;
    time_t crt;
    TPhase* aPH = TPhase::pm;
    RMULTS* mup = TRMults::sm->GetMU();
    SYSTEM *syp = TSyst::sm->GetSY();

    if( pmp->pKMM >= 1 )
    return;

    // KinMet data extraction from Phase records (so far another run rel.to SolModLoad())
    for( kk=0, k=-1; kk<mup->Fi; kk++ )
    {
        JB = JE;
        JE += mup->Ll[kk];
        if( syp->Pcl[kk] == S_OFF )
            continue;
        k++;
        jb=je;
        je+= pmp->L1[k];

        // Indexes for extracting data from       arrays
        kx = kxe;
        kc = kce;
        kp = kpe;
        kf = kfe;
        ka = kae;
        ks = kse;
        kd = kde;
        ki = kie;

        aPH->TryRecInp( mup->SF[kk], crt, 0 ); // reading phase record

        // selecting type of the model
        memcpy( modT, aPH->php->kin_t+2, MAXKEYWD );
        memcpy( pmp->kMod[k], modT, MAXKEYWD );
        kMod = pmp->kMod[k];
        pmp->PfFact[k] = 1.;   // temporary
if( aPH->php->h0p && aPH->php->R0p )
    pmp->PfFact[k] = aPH->php->h0p/aPH->php->R0p;
/*
if(kMod[0] != KM_UNDEF )
{
    cout << "k:" << k << " kMod:" << kMod[0] << kMod[1] << kMod[2] << kMod[3] << kMod[4] << kMod[5];
    cout << " nPRk:" << pmp->LsKin[k*6] << " nSkr:" << pmp->LsKin[k*6+1] << " nrpC:" << pmp->LsKin[k*6+2] <<
    " naptC:" << pmp->LsKin[k*6+3] << " nAscC:" << pmp->LsKin[k*6+4] << " nFaces:" << pmp->LsKin[k*6+5] << endl;
}
*/
        // load coefficients of KinMet models into MULTI transfer arrays
LOAD_KKMCOEF:
        if( pmp->LsKin[k*6] )
        { // loading parameters for parallel reactions

            //  Copying the PR opcode array from Phase to MULTI
            if( aPH->php->ocPRk )
            {
               if( pmp->ocPRkC == NULL || kp+pmp->LsKin[k*6] > aObj[ o_wi_ocprkc ].GetN() )
                  pmp->ocPRkC = (long int (*)[2]) aObj[ o_wi_ocprkc ].Alloc(
                     kp+pmp->LsKin[k*6], 2, L_ );
               ErrorIf( pmp->ocPRkC == NULL, "KinMetModLoad",
                        "Error in reallocating memory for pmp->ocPRk" );
               for( jj=0; jj<aPH->php->nPRk; jj++ )
               {
    pmp->ocPRkC[kp+jj][0] = (long int)aPH->php->ocPRk[jj*2];
    pmp->ocPRkC[kp+jj][1] = (long int)aPH->php->ocPRk[jj*2+1];
               }
//     copyValues( pmp->ocPRkC+kp, aPH->php->ocPRk, pmp->LsKin[k*6]);
            }
            else
               pmp->LsKin[k*6] = 0;  // no PR codes and coefficients

            if( aPH->php->feSAr )
            {
                if( pmp->feSArC == NULL || kf+pmp->LsKin[k*6] > aObj[ o_wi_fsac ].GetN() )
                   pmp->feSArC = (double *) aObj[ o_wi_fsac ].Alloc(
                     (kf+pmp->LsKin[k*6]), 1, D_ );
                ErrorIf( pmp->feSArC == NULL, "KinMetModLoad",
                         "Error in reallocating memory for pmp->feSArC." );
                copyValues( pmp->feSArC+kf, aPH->php->feSAr, pmp->LsKin[k*6]);
            }
            else
                pmp->LsKin[k*6] = 0; // no PR codes and area fractions

            if( aPH->php->rpCon )
            {
                if( pmp->rpConC == NULL ||
                    kc+pmp->LsKin[k*6]*pmp->LsKin[k*6+2] > aObj[ o_wi_krpc ].GetN() )
                 pmp->rpConC = (double *) aObj[ o_wi_krpc ].Alloc(
                   (kc+pmp->LsKin[k*6]*pmp->LsKin[k*6+2]), 1, D_ );
                ErrorIf( pmp->rpConC == NULL, "KinMetModLoad",
                       "Error in reallocating memory for pmp->rpConC." );
                copyValues( pmp->rpConC+kc, aPH->php->rpCon, (pmp->LsKin[k*6]*pmp->LsKin[k*6+2]));
            }
            else { // no array with kinetic PR parameters in the Phase record
                pmp->LsKin[k*6+2] = 0;
            }

            if( aPH->php->Ascp )
            {
                if( pmp->AscpC == NULL || ks+pmp->LsKin[k*6+4] > aObj[ o_wi_ascpc ].GetN() )
//                  (int)(sizeof( pmp->AscpC )/sizeof(double)))
                 pmp->AscpC = (double *) aObj[ o_wi_ascpc ].Alloc(
                   ks+pmp->LsKin[k*6+4], 1, D_ );
                ErrorIf( pmp->AscpC == NULL, "KinMetModLoad",
                       "Error in reallocating memory for pmp->AscpC." );
                copyValues( pmp->AscpC+ks, aPH->php->Ascp, pmp->LsKin[k*6+4]);
            }
            else { // no array with Asur correction parameters in the Phase record
                pmp->LsKin[k*6+4] = 0;
            }

            if( aPH->php->lDCr && aPH->php->apCon)
            { // List of DC names involved in PR activity products
                if( pmp->xSKrC == NULL || kd+pmp->LsKin[k*6+1] > aObj[ o_wi_jcrdc ].GetN() )
//                  (int)(sizeof( pmp->xSKrC )/sizeof(long int)))
                 pmp->xSKrC = (long int *) aObj[ o_wi_jcrdc ].Alloc(
                   kd+pmp->LsKin[k*6+1], 1, L_ );
                ErrorIf( pmp->xSKrC == NULL, "KinMetModLoad",
                       "Error in reallocating memory for pmp->xSKrC." );
                if(  pmp->apConC == NULL || ka+pmp->LsKin[k*6+1] *pmp->LsKin[k*6]*pmp->LsKin[k*6+3]
                     > aObj[ o_wi_apconc ].GetN() )
   //               (int)(sizeof( pmp->apConC )/sizeof(double)))
                 pmp->apConC = (double *) aObj[ o_wi_apconc ].Alloc(
                   (ka+pmp->LsKin[k*6+1] *pmp->LsKin[k*6]*pmp->LsKin[k*6+3]), 1, D_ );
                ErrorIf( pmp->apConC == NULL, "KinMetModLoad",
                       "Error in reallocating memory for pmp->apConC." );
                int dcph = 0, dcInd = 0;
                char DCname[MAXDCNAME+2];
                for( jj=0; jj<aPH->php->nSkr; jj++ )
                {
                    strncpy( DCname, aPH->php->lDCr[jj]+(MAXSYMB+MAXDRGROUP), MAXDCNAME );
                    DCname[MAXDCNAME] = '\0';
                    dcInd = find_dcnum_multi( DCname );
// cout << dcph << ": " << DCname << " dcInd:" << dcInd << endl;
                    if( dcInd >= 0 )
                    {   // here, parameters of DCs not present in MULTI are skipped
                        pmp->xSKrC[kd+dcph] = (long int)dcInd;
                        copyValues( pmp->apConC+ka+dcph*pmp->LsKin[k*6]*pmp->LsKin[k*6+3],
                          aPH->php->apCon+jj*pmp->LsKin[k*6]*pmp->LsKin[k*6+3],
                          (pmp->LsKin[k*6]*pmp->LsKin[k*6+3]));
                        dcph++;
                    }
                }
                pmp->LsKin[k*6+1] = dcph;  // Set actual number of DCs used in PR activity products
            }
            else
            {
                pmp->LsKin[k*6+1] = 0; // no DC coefficients for PR activity product terms
            }
        }

        // Loading Linked phase parameters
        pmp->LsPhl[k*2] = aPH->php->nlPh;
        pmp->LsPhl[k*2+1] = aPH->php->nlPc;

        int dphl = 0;
        if( pmp->LsPhl[k*2] && pmp->LsPhl[k*2+1] )
        {   // coefficients of linkage parameters in linked phases
          if( aPH->php->lPhc /*&& aPH->php->IsoC*/ )
          {
              char ibuf[2] = "";
              if( pmp->lPhc == NULL || jlphc+pmp->LsPhl[k*2]*pmp->LsPhl[k*2+1] > aObj[ o_wi_lphc ].GetN() )
    //             > (int)(sizeof( pmp->lPhc )/sizeof(double)))
                pmp->lPhc = (double *) aObj[ o_wi_lphc ].Alloc(
                   jlphc+pmp->LsPhl[k*2]*pmp->LsPhl[k*2+1], 1, D_ );
            ErrorIf( pmp->lPhc == NULL, "KinMetModLoad",
                    "Error in reallocating memory for pmp->lPhc." );
            if( pmp->PhLin == NULL || jphl+pmp->LsPhl[k*2]  > aObj[ o_wi_phlin ].GetN() )
   //                 > (int)(sizeof( pmp->PhLin )/sizeof(long int (*)[2])))
                pmp->PhLin = (long int (*)[2]) aObj[ o_wi_phlin ].Alloc(
                   jphl+pmp->LsPhl[k*2], 2, L_ );
            ErrorIf( pmp->PhLin == NULL, "KinMetModLoad",
                    "Error in reallocating memory for pmp->PhLin." );
            char Pname[MAXSYMB+MAXPHNAME+2]; int phInd;
            for( jj=0; jj<aPH->php->nlPh; jj++ )
            {
                strncpy( Pname, aPH->php->lPh[jj], MAXSYMB );
                strncpy( Pname+MAXSYMB, aPH->php->lPh[jj]+(MAXSYMB+MAXPHSYMB), MAXPHNAME );
                Pname[MAXSYMB+MAXPHNAME] = '\0';
                phInd = find_phnum_multi( Pname );
// cout << dphl << ": " << Pname << " phInd: " << phInd << endl;
                if( phInd >= 0 )
                {   // here, parameters for phases not present in MULTI are skipped
                    pmp->PhLin[jphl+dphl][0] = phInd;
                    ibuf[0] = aPH->php->lPhC[jj];
                    pmp->PhLin[jphl+dphl][1] = (long int)atoi( ibuf );
                    copyValues( pmp->lPhc+jlphc+dphl*pmp->LsPhl[k*2+1],
                         aPH->php->lPhc+jj*pmp->LsPhl[k*2+1], pmp->LsPhl[k*2+1]);
                    dphl++;
                }
            }
          }
          else
          {
              pmp->LsPhl[k*2+1] = 0; // no DC coefficients
          }
          pmp->LsPhl[k*2] = dphl;
        }

        if( aPH->php->PumpCon && aPH->php->lICu && pmp->LsUpt[k*2+1] > 0 )
//                ( kMod[2] == KM_UPT_ENTRAP || kMod[2] == KM_IEX_FAST || kMod[2] == KM_IEX_SLOW ) )
            // kMod[0] == KM_PRO_UPT )  // so far only for uptake kinetics models!
        {
            int icph=0, icInd = 0;
            char ICname[MAXICNAME+2];

            if( pmp->xICuC == NULL || ki+pmp->LsUpt[k*2+1] > aObj[ o_wi_xicuc ].GetN() )
                pmp->xICuC = (long int *) aObj[ o_wi_xicuc ].Alloc( ki+pmp->LsUpt[k*2+1], 1, L_ );
            ErrorIf( pmp->xICuC == NULL, "KinMetModLoad",
                    "Error in reallocating memory for pmp->xICuC." );
            // For now, no compression of IC name list (and other stuff in TKinMet). TBD!!!
            for( jj=0, j=JB; jj<aPH->php->nDC; jj++, j++ )
            {
                if( syp->Dcl[j] == S_OFF ) // here, parameters of DCs not present in MULTI are skipped
                    continue;
                strncpy( ICname, aPH->php->lICu[jj], MAXICNAME );
                ICname[MAXICNAME] = '\0';
                icInd = find_icnum_multi( ICname );
    // cout << icph << ": " << ICname << " icInd:" << icInd << endl;
                if( icInd >= 0 )
                {
                    pmp->xICuC[ki+icph] = (long int)icInd;
                }
                else {
                  // This IC is missing in MULTI - potentially error!
                    ErrorIf( pmp->apConC == NULL, "KinMetModLoad",
                           "Error in reallocating memory for pmp->xICuC." );
                    pmp->xICuC[ki+icph] = -1L;
                }
                icph++;
            }
        }

        // move handles to process the next phase
        jphl  += pmp->LsPhl[k*2];
        jlphc += pmp->LsPhl[k*2]*pmp->LsPhl[k*2+1];

        kfe += pmp->LsKin[k*6];
        kpe += pmp->LsKin[k*6];
        kce += pmp->LsKin[k*6]*pmp->LsKin[k*6+2];
        kae += pmp->LsKin[k*6]*pmp->LsKin[k*6+1]*pmp->LsKin[k*6+3];
        kse += pmp->LsKin[k*6+4];
        kde += pmp->LsKin[k*6+1];
        kie += pmp->LsUpt[k*2+1];
//        if( aPH->php->PumpCon && kMod[0] == KM_PRO_UPT )
//            kie += pmp->L1[k];
     } // kk, k

 //    pmp->pKMM = 1;
 }


// Loading parameters and scripts for the non-ideality models of
// phases-solutions into structure for calculations of equlibria
// Uses the IPN stack calculation module (c) 1991-1996 S.Dmytrieva
//
void TMultiSystem::SolModLoad()
{
    int kk, k, j, jj, ku, kue=0, Type=0, JB, JE=0, jb, je=0;
    char modT[164];
    char *sMod;
    time_t crt;
    TPhase* aPH = TPhase::pm;
    RMULTS* mup = TRMults::sm->GetMU();
    SYSTEM *syp = TSyst::sm->GetSY();

    if( pmp->pIPN >= 1 )           //SD 29/11/2006
       return;
    ErrorIf( !pmp->FIs, "SolModLoad", "No phases-solutions!" );

    if( pmp->pIPN <= 0 )
    {
        qEp.clear();
        qEd.clear();
    }

    // Data extraction from Phase records
    for( kk=0, k=-1; kk<mup->Fis; kk++ )
    {
        JB = JE;
        JE += mup->Ll[kk];
        if( syp->Pcl[kk] == S_OFF || kk >= mup->Fis )
            continue;
        k++;
        jb=je;
        je+= pmp->L1[k];

        // Indexes for extracting data from IPx, PMc and DMc arrays
        ku = kue;

        if( pmp->L1[k] == 1 )
            continue;  // one component is left in the multicomponent phase
        aPH->TryRecInp( mup->SF[kk], crt, 0 ); // reading phase record

        // Compress phase record
        if( aPH->php->Ppnc == S_ON && aPH->php->npxM > 0 )    // Check conditions of compressing!
            CompressPhaseIpxt( kk );

        // selecting type of the model
        memcpy( modT, aPH->php->sol_t, MAXKEYWD );
        memcpy( pmp->sMod[k], modT, MAXKEYWD );
        pmp->sMod[k][6] = aPH->php->kin_t[0];
        pmp->sMod[k][7] = aPH->php->kin_t[1];
        sMod = pmp->sMod[k];

        // Reload multi-site mixed moodels
        if( aPH->php->nMoi >0 )
        { TFormula aFo;
          vector<string> form_array;

           // build formula list
           for( jj=jb, j=JB; j < JE; j++ )
           { // set indexes of components - eliminating those switched off
                 if( syp->Dcl[j] == S_OFF )
                     continue;
                 form_array.push_back( aFo.form_extr( j, mup->L, mup->DCF ) );
           } /* j */
           ErrorIf( form_array.size()!=pmp->L1[k],"SolModLoad", "Test error."  );

           // get moiety structure from phase
           aPH->MakeSublatticeLists( form_array );
       }

        pActivity->SolModCreatePhase( k, jb, je, JB, JE, aPH->php,  sMod[SPHAS_TYP], sMod[MIX_TYP]  );

        // potentially an error - should be set in any DCE_LINK mode, also SM_UNDEF ?
        switch( modT[DCE_LINK] )
        {
        case SM_UNDEF:  // no script equations were specified
               if( modT[SGM_MODE] != SM_STNGAM )
                  continue;
               goto LOAD_NIDMCOEF;
        case SM_PRIVATE_:
        case SM_PUBLIC:   // nonideal solution
               //  Changed 07.12.2006   KD
            /*     if( !pm_GC_ods_link( q, k, jb, kc, kd ))
                 { iRet = AMW_FE; goto WRONG; }  */
            aObj[ o_ntc ].SetPtr( &pmp->TCc );
            aObj[ o_ntk ].SetPtr( &pmp->Tc );
            aObj[ o_np  ].SetPtr(  &pmp->Pc );
            aObj[ o_nis ].SetPtr( &pmp->IC );
            aObj[ o_ni  ].SetPtr(  &pmp->is );
            aObj[ o_njs ].SetPtr(  &pmp->js );
            aObj[ o_nnext ].SetPtr( &pmp->next );
            aObj[ o_nrt ].SetPtr( &pmp->RT );
            aObj[ o_nlnrt ].SetPtr(&pmp->FRT );
            aObj[ o_nqp ].SetPtr( pmp->Qp+k*QPSIZE );
            aObj[ o_nqd ].SetPtr( pmp->Qd+k*QDSIZE );  // QDSIZE cells per phase
            //aObj[ o_nncp].SetPtr( acp->LsMod_+k*3 );
            //aObj[ o_nncp].SetDim( 1, 3 );
            //aObj[ o_nncd].SetPtr( acp->LsMdc_+k*3 );
            //aObj[ o_nncd].SetDim( 1, 1 );
            aObj[ o_ndc ].SetPtr( pmp->L1+k );
            aObj[ o_nez ].SetPtr( pmp->EZ+jb );
            aObj[o_nez].SetN( pmp->L1[k]);
            //aObj[ o_npcv].SetPtr( acp->PMc_+kc );
            //aObj[o_npcv].SetDim( acp->LsMod_[k*3], acp->LsMod_[k*3+2]);
            //  Object for indexation of interaction parameters
            //aObj[ o_nu].SetPtr( acp->IPx_+kx );
            //aObj[o_nu].SetDim( acp->LsMod_[k*3], acp->LsMod_[k*3+1]);
            //aObj[ o_ndcm].SetPtr( acp->DMc_+kd );
            //aObj[o_ndcm].SetDim( pmp->L1[k], acp->LsMdc_[k*3]);
            aObj[ o_nmvol].SetPtr(pmp->Vol+jb );
            aObj[o_nmvol].SetN( pmp->L1[k]);
            aObj[ o_nppar].SetPtr(pmp->G0+jb );   // Changed 10.12.2008 by DK
            aObj[ o_nppar].SetN( pmp->L1[k]);
            aObj[ o_ngtn].SetPtr( pmp->fDQF+jb );     // changed 15.06.2011 by DK
            aObj[ o_ngtn].SetN( pmp->L1[k]);
            aObj[ o_ngam].SetPtr( pmp->Gamma+jb );
            aObj[ o_ngam].SetN( pmp->L1[k]);
            aObj[ o_nlngam].SetPtr( pmp->lnGam+jb );
            aObj[ o_nlngam].SetN( pmp->L1[k]);
            aObj[ o_nas].SetPtr(  pmp->A+pmp->N*jb );
            aObj[ o_nas].SetDim( pmp->L1[k], pmp->N);
            aObj[ o_nxa].SetPtr( pmp->XF+k );
            aObj[ o_nxaa ].SetPtr(  pmp->XFA+k );
            if( pmp->FIat > 0 )
            {
                aObj[ o_nxast].SetPtr( pmp->XFTS[k] );
                aObj[ o_nxcec].SetPtr( pmp->Xetaf[k] );
            }
            else
            {
                aObj[ o_nxast].SetPtr( NULL );
                aObj[ o_nxcec].SetPtr( NULL );
            }
            aObj[ o_nbmol].SetPtr(pmp->FVOL+k ); // Phase volume
            aObj[ o_nxx].SetPtr(  pmp->X+jb );
            aObj[o_nxx].SetN( pmp->L1[k]);
            aObj[ o_nwx].SetPtr(  pmp->Wx+jb );
            aObj[o_nwx].SetN( pmp->L1[k]);
            aObj[ o_nmju].SetPtr( pmp->Fx+jb );
            aObj[o_nmju].SetN( pmp->L1[k]);
            break;
        default: // error - invalid code of the mixing model
            Error( "SolModLoad", "Wrong code of the model of mixing" );
        } // end switch


        // Translating Phase math script equations of mixing model
        if( !aPH->php->pEq && !aPH->php->dEq )
        { // Error - no equations in record
            if( vfQuestion(window(), "SolModLoad",
                           "No expressions in non-ideality model: take as ideal?" ))
                break;
            else Error( "SolModLoad", "Error: no equations in record." );
        }
        if( aPH->php->pEq && *aPH->php->pEq && modT[SPHAS_DEP] != SM_UNDEF )
        { // translate expressions in the non-ideality model for the whole phase
            aObj[ o_neqtxt].SetPtr( aPH->php->pEq );
            Type = 0;
            if( pmp->PHC[k] != PH_SORPTION && pmp->PHC[k] != PH_POLYEL )
                sm_text_analyze( k, Type, JB, JE, jb, je );
            else sm_text_analyze( k, Type, 0, JE, 0, je );
        }
        else if( modT[SPHAS_DEP] == SM_UXDEP // || modT[SPHAS_TYP] == SM_ASYMM
                 )
            Error( "SolModLoad", "Error?: modT[SPHAS_DEP] == SM_UXDEP." );
        if( aPH->php->dEq && *aPH->php->dEq && modT[DCOMP_DEP] != SM_UNDEF )
        { // translate expressions in the non-ideality model for components
            if( modT[DCE_LINK] == SM_PUBLIC )
            { // one group of expressions for all components
                aObj[ o_neqtxt].SetPtr( aPH->php->dEq );
                Type = 1;
            }
            else
            { // separate group of expressions for each component
                aObj[ o_neqtxt].SetPtr( NULL );
                Type = 2;
            }
        }
        else if( modT[DCOMP_DEP] == SM_UXDEP || modT[DCOMP_DEP] == SM_TPDEP )
            Error( "SolModLoad",
                   "Error?: modT[DCOMP_DEP] == SM_UXDEP || modT[DCOMP_DEP] == SM_TPDEP" );
        if( pmp->PHC[k] != PH_SORPTION && pmp->PHC[k] != PH_POLYEL )
            sm_text_analyze( k, Type, JB, JE, jb, je );
        else  sm_text_analyze( k, Type, 0, JE, 0, je );


        // load coefficients of mixing models into MULTI transfer arrays
        // Valid for both built-in and scripted mixing models
LOAD_NIDMCOEF:

        // new: load coefficients and parameters for TSorpMod here
        if( pmp->sMod[k][6] != SM_UNDEF )
        {
            ;
        }
        if( pmp->sMod[k][7] != SM_UNDEF )
        {

        }

       // new: load uptake kinetics model parameters here  !!! compression of phase DCs not yet implemented !!!
        if( aPH->php->umpCon )
        {
            if( ku+pmp->LsUpt[k*2]*pmp->L1[k] > aObj[ o_wi_umpc ].GetN()
                    || pmp->UMpcC == NULL )
//              (int)(sizeof( pmp->UMpcC )/sizeof(double)))
             pmp->UMpcC = (double *) aObj[ o_wi_umpc ].Alloc(
               ku+pmp->LsUpt[k*2]*pmp->L1[k], 1, D_ );
            ErrorIf( pmp->UMpcC == NULL, "SolModLoad",
                   "Error in reallocating memory for pmp->UMpcC." );
            copyValues( pmp->UMpcC+ku, aPH->php->umpCon, pmp->LsUpt[k*2]*pmp->L1[k]);
        }
        else { // no array with uptake kinetics parameters in the Phase record
            pmp->LsUpt[k*2] = 0;
        }
        kue += pmp->LsUpt[k*2]*pmp->L1[k];

     } // k

    pmp->pIPN = 1;
 }

//-------------------------------------------------------------------
// Translates, analyzes and unpacks equations of phase non-ideal model
// Type: 0-equations for the whole phase,
//       1 - one group of equations for all DC,
//       2 - separate group of expressions fo each component
//  npj, JB, JE - indexes of phase and DC in RMULTS (Project)
//  jb, je - indexes of DC in structure MULTI
//  (C) S.Dmytrieva 1991-1996
//
void TMultiSystem::sm_text_analyze( int nph, int Type,
                               int JB, int JE, int jb, int je )
{
    char *erscan = 0;
    string save;

    erscan = (char *)aObj[ o_nwtext].Alloc(  1, 2048, S_ );
    //AGAIN: // translate equations
    try
    {
        if( Type == 2 )
        {
            save = PressSolMod(nph );
            aObj[ o_neqtxt ].SetPtr( (char *)save.c_str() );
        }
        ET_translate( o_nwtext, o_neqtxt, JB, JE, jb, je );
        if( Type )
        {
            qEd.resize(nph, 0);
            qEd.push_back( new IPNCalc() );
            qEd[nph]->GetEquat( (char *)aObj[o_nwtext].GetPtr() );
        }
        else
        {
            qEp.resize(nph, 0);
            qEp.push_back( new IPNCalc() );
            qEp[nph]->GetEquat( (char *)aObj[o_nwtext].GetPtr() );
        }
    }
    catch( TError& xcpt )
    {
        vfMessage(window(), xcpt.title, xcpt.mess);
        /*bool   iRet = */
        TPhase::pm->CheckEqText(  erscan,
           "E97MSTran: Error in Phase non-ideality mixing model scripts: " );
        /*  if( iRet )  goto AGAIN;  */
        Error(  "SolModLoad", xcpt.mess );
    }
    /* if( Type ) // print results to rpn_log.txt
        qEd[nph].PrintEquat( (char *)aObj[o_neqtxt].GetPtr(), fstream& f);
       else
        qEp[nph].PrintEquat( (char *)aObj[o_neqtxt].GetPtr(), fstream& f);
     */
}

//--------------------------------------------------------------------
// Packs the script of PRIVATE type of solution phase
//
string TMultiSystem::PressSolMod( int nP )
{
    int j, jp, k, ks, jb=0, je=0, EGlen;
    char *EGb;
    TPhase* aPH=TPhase::pm;
    RMULTS* mup = TRMults::sm->GetMU();
    SYSTEM *syp = TSyst::sm->GetSY();
    string etext;

    for( k=0,ks=-1; k < mup->Fis && ks < nP; k++ )
    {  // calculating indexes
        jb = je;
        je += mup->Ll[k];
        if( syp->Pcl[k] == S_ON )
        {
            ks++;
            // jse += pmp->L1[ks];
        }
    }
    // list of components in solution
    for( j=jb; j<je; j++ )
    {
        if( syp->Dcl[j] == S_OFF )
            continue;
        jp = mup->Pl[j];  // index component in phase
        EGlen = 0;
        // get equation number jp in  PHASE
        EGb = ExtractEG( aPH->php->dEq, jp, &EGlen, aPH->php->nDC );
        ErrorIf( !EGb,
         "E12MSPrep:", "SolModLoad(): Missing operators group for a phase endmember." );
        if( EGlen )
        {  // set text to buffer
            etext += string( EGb, 0, EGlen );
            etext +=  "\n" ;
        }
    }
    return etext;
}

//------------------------------------------------------------------
//Get groop of equations number jp from Etext
// Returns *EGlen=0 if there is no equations for the group jp
//
char *TMultiSystem::ExtractEG( char *Etext, int jp, int *EGlen, int Nes )
{
    int j;
    char *begin, *end;
    begin = Etext;
    if( jp )
        for(j=0; j<jp; j++)
{ /* search begin of group */  Again:
            begin = strchr( begin, A_delim_IPM );
            /* begin = strchr( begin, EQSET_DELIM ); */
            if( !begin && jp < Nes-1 )
                return NULL;
            begin++;
            if( *begin != EQSET_DELIM )
                /* This is not the end of previous eq group */
                goto Again;
            if( !begin && jp < Nes-1 )
                return 0;
            begin++; /* Found */
        }
    /* set length of block */
    end = begin;
AgainE:
    end = strchr( end, A_delim_IPM );
    /*   if( !end && jp < Nes-1 ) can be < eqns than DC! final \@ */
    if( !end && jp >= 0 && jp < Nes-1 )
    {
        *EGlen = 0;
        return begin;
    }
    if( !end && jp == Nes-1 )
    {
        *EGlen = strlen( begin );
        return begin;
    }
    end++;
    if( *end != EQSET_DELIM )
        goto AgainE;
    end++;
    *EGlen = end-begin;
    return begin;
}

// Search of component index by name ( *name )
// Find index IC: LNmode=0: by lists RMULTS;  mup->
//	          LNmode=1: by lists MULTI;   pmp->
//  Return index >=0 or exception if no match
//
int TMultiSystem::find_icnum( char *name, int LNmode )
{
    int i;
    char ICs[IC_RKLEN+10];
    char *nbg, *nend;
    TFormula aFo;

    nbg = name;
    nend = name+strlen(name)-1;
    fillValue( ICs, ' ', IC_RKLEN );
    ICs[/*MAXSYMB*/IC_RKLEN] = 0;
    if( isdigit( name[0] ))
{ /* this is isotope 34S */ MOVEDIGIT:
        ICs[MAXICNAME] = *nbg;
        nbg++;
        if( nbg<nend && isdigit( *nbg ))
            goto MOVEDIGIT;
    }
    else if( islower( name[0] ))
    { /* this is letter - type of IC,  e.g. f Sio, a Hum */
        ICs[MAXICNAME] = *nbg;
        nbg++;
    }
    else
    { /* this is ordinary IC */
        ICs[MAXICNAME] = IC_ELEMENT;
    }
    memcpy( ICs, nbg, min( (size_t)6, strlen( nbg )));
    aFo.fixup_ics( ICs );
    if( LNmode == 1 )
    {
        for( i=0; i<pmp->N; i++ )
        {
            if( !memcmp( ICs, pmp->SB[i], MAXICNAME/*+MAXSYMB*/ ))
                return i;
        }
        Error( ICs, "E04MSPrep: IC name cannot be found in GEM IPM work data lists..." );
    }
    if( LNmode == 0 )
    {
        RMULTS* mup = TRMults::sm->GetMU();
        for( i=0; i<mup->N; i++ )
        {
            if( !memcmp( ICs, mup->SB[i], MAXICNAME+MAXSYMB ))
                return i;
        }
        Error( ICs, "E14MSPrep: IC name cannot be found in Project system definition lists..." );
    }
    Error( ICs, "E09MSPrep: Invalid IC search mode parameter..." );
    return -1;
}

// Search of component index by name ( *name )
// Find index DC: LNmode=0: by lists RMULTS;  mup->
//		  LNmode=1: by lists MULTI;   pmp->
//  jb and je - range to search
//  Return index >=0 or exeption if no match
//
int TMultiSystem::find_dcnum( char *name, int jb, int je, int LNmode, char *stmt )
{
    int j, jf[20], ii=0, len, jj;

    len = strlen( name );
    if( LNmode == 1 )
    {
        for( j=jb; j<=je && ii<20; j++ )
            if(!memcmp(name, pmp->SM[j], min(len,MAXDCNAME)))
                jf[ii++] = j;
        if( !ii )
            Error( name, "E05MSPrep: DC name cannot be found in GEM IPM work data lists..." );
    }
    if( LNmode == 0 )
    {
        for( j=jb; j<=je && ii<20; j++ )
            if(!memcmp(name, TRMults::sm->GetMU()->SM[j]+MAXSYMB+MAXDRGROUP,
                       min(len,MAXDCNAME)))
                jf[ii++] = j;
        if( !ii )
            Error( name, "E15MSPrep: DC name cannot be found in Project system definition lists..." );
    }

    /* more then one index found */
    if( LNmode == 0 )
    {
        jj=0;
        for( j=0; j<ii; j++ )
        {    if( TSyst::sm->GetSY()->Dcl[jf[j]] == S_OFF )
                continue;
             jf[jj] = jf[j];
             jj++;
        }
       if( !jj )
       {
         if( !vfQuestion(window(), name,
              "W25MSPrep: All DCs with this name are switched off!\n"
              "the changes programmed in the script will not be effective...\n"
              "Continue (Y) or cancel (N)?"))
          Error( "E25MSPrep:", "Preprocessing cancelled by the user..." );
       }
       else
        ii = jj;
    }

    if( ii == 1 )
        return( jf[0] );

    string pbuf = "Please, select DC index for the DC name ";
            pbuf += name;
            pbuf += "\n in the first script statement below \n";
            pbuf += stmt;
    //sprintf( pbuf, "Please, select DC index for the DC name %s\n in the first script statement below %s", name, stmt );

   j = vfChoice( window(),  "W20MSPrep: ", pbuf.c_str(), ii, jf);
   if( j<0 )
        Error( "E20MSPrep:", "Preprocessing cancelled by the user..." );

   return( j );
}

/// Search of Phase index by name ( *name )
/// Find index in lists MULTI;  pmp->SF
///  \return index >=0 or -1
//
int TMultiSystem::find_phnum_multi( const char *name)
{
    int k, len;

    len = strlen( name );
    for( k=0; k < pmp->FI; k++ )
      if( !memcmp(name, pmp->SF[k], min(len,(int)(MAXPHSYMB+MAXSYMB))))
                return k;
    return -1;
}


/// Search of IComp index by name ( *name )
/// Find index in lists MULTI;  pmp->SB1
///  \return index >=0 or -1
//
int TMultiSystem::find_icnum_multi( const char *name)
{
    int i, len;

    len = strlen( name );
    for( i=0; i < pmp->N; i++ )
      if( !memcmp(name, pmp->SB1[i], min(len,(int)(MAXICNAME))))
                return i;
    return -1;
}

/// Search of DComp index by name ( *name )
/// Find index in lists MULTI;  pmp->SM
///  \return index >=0 or -1
//
int TMultiSystem::find_dcnum_multi( const char *name)
{
    int j, len;

    len = strlen( name );
    for( j=0; j < pmp->L; j++ )
      if( !memcmp(name, pmp->SM[j], min(len,(int)(MAXDCNAME))))
                return j;
    return -1;
}

// Search of phase index by name ( *name )
// Find index: LNmode=0: in lists RMULTS; mup->
//	       LNmode=1: in lists MULTI;  pmp->
//  Return index >=0 or exception if no match
//
int TMultiSystem::find_phnum( char *name, int LNmode )
{
    int k, kf[8], ii=0, len;

    len = strlen( name );
    if( LNmode == 1 )
    {
        for( k=0; k < pmp->FI && ii < 8; k++ )
            if( !memcmp(name, pmp->SF[k]+MAXSYMB, min(len,MAXPHNAME)))
                kf[ii++] = k;
        if( !ii )
          Error( name, "E06MSPrep: Phase name cannot be found in GEM IPM work data lists..." );
    }
    if( LNmode == 0 )
    {
        RMULTS* mup = TRMults::sm->GetMU();
        for( k=0; k < mup->Fi && ii < 8; k++ )
            if( !memcmp(name, mup->SF[k]+MAXSYMB+MAXPHSYMB, min(len,MAXPHNAME)))
                kf[ii++] = k;
        if( !ii )
          Error( name, "E16MSPrep: Phase name cannot be found in Project system definition lists..." );
    }
    if( ii == 1 )
        return( kf[0] );
    /* more then one useful index */
    char pbuf_[164];
    string pbuf;
    sprintf( pbuf_, "%d", ii);
    pbuf = pbuf_;
    pbuf += " of Phase indices found for the phase name ";
    pbuf += name;
    sprintf( pbuf_, "(->%d) \n", LNmode);
    pbuf += pbuf_;
    pbuf +="Take the first one and continue (Y) or cancel (N)?";

    //sprintf( pbuf_, "%d of Phase indices found for the phase name %s (->%d) \n"
    //         "Take the first one and continue (Y) or cancel (N)?", ii, name, LNmode );
    if( !vfQuestion(window(), "W21MSPrep: ", pbuf.c_str() ))
        Error( "E21MSPrep: ", "Preprocessing cancelled by the user..." );

    return( kf[0] );
}

// Search of composition index by name ( *name )
// Find index: LNmode=0: in lists RMULTS; mup->
//	       LNmode=1: in lists MULTI;  pmp->
//  Return index >=0 or exception if no match
//
int TMultiSystem::find_acnum( char *name, int LNmode )
{
    int j, jf[8], ii=0, len;
    RMULTS* mup = TRMults::sm->GetMU();

    if( mup->La <= 0 )
        Error( name, "E10MSPrep: This modelling project has no PCO name lists." );
    len = strlen( name );
    for( j=0; j<mup->La && ii< 8; j++ )
        if( !memcmp(name, mup->SA[j], min(len,MAXCMPNAME)))
            jf[ii++]=j;
    if( !ii )
        Error( name, "E07MSPrep: PCO name cannot be found in Project system definition lists..." );
    if( ii == 1 )
        return( jf[0] );
    /* more then one matching index */
    char pbuf_[164];
    string pbuf;
    sprintf( pbuf_, "%d", ii);
    pbuf = pbuf_;
    pbuf += " of PCO indices found for the PCO name ";
    pbuf += name;
    sprintf( pbuf_, "(->%d) \n", LNmode);
    pbuf += pbuf_;
    pbuf +="Take the first one and continue (Y) or cancel (N)?";

    if( !vfQuestion(window(), "W08MSPrep: ", pbuf.c_str() ))
        Error( "E08MSPrep: ", "Preprocessing cancelled by the user..." );

    return( jf[0] );
}

//-----------------------------------------------------------------------
// Preprocessor of species, phases and component names in math scripts.
// Changes fragments like \J ... <objName>{CaSO4@} to <objName>[45]
// or equivalent: <objName>[{CaSO4@}+5] to <objName>[45+5]
//  Default:  \j  (DC in MULTI)
//  Data object nOpex contains initial text of equations;
//         nOet  resulting (pre-processed) text.
//  jb - je range of DC in  MULTI,  JB - JE  - that in RMULTS.
// Modified by KD 30.03.01 to ignore \J ... switches and instead
// obtain indexation codes from data object name and DOD.
//
const char* MSDELIM = " +-*/^:[]();=$&|!<>?#\n\t";
//
void TMultiSystem::ET_translate( int nOet, int nOpex, int JB, int JE, int jb, int je,
        tget_ndx *get_ndx )
{
    size_t eLen, ls, lb;
    int i=0, ii, pj, LNplace=1, Xplace=0, nO=0;
    char cstate, cc, *etext, *pexpr, *ecur, *cur, *next, *end,
      *prev, *last, *stmt, iCode, odlab[MAXKEYWD+2];
    char name[164];
    char nbuf[164];

    etext = (char *)aObj[ nOet ].GetPtr();
    if( etext )
        eLen = aObj[ nOet ].GetM();
    else eLen =0;
    pexpr = (char *)aObj[ nOpex ].GetPtr();

    ls = strlen( pexpr );
    if( !ls )
      return;
    end = pexpr+ls;
    //if( ls+1 > eLen )
    //    etext = (char *)aObj[ nOet ].Alloc( 1, ls+ls/6, S_ );
    aObj[ nOet ].Free();
    etext = (char *)aObj[ nOet ].Alloc( 1, ls+ls/6, S_ );
    ecur = etext;
    cur = pexpr;
    next = cur;
    stmt = next;

    if( *cur != '$' ) //SD&DK fixed error 05/12/2007)
    {
      *ecur = ' ';
       ecur++;
    }

    cc=0;
    cstate = A_dcx;
    while( next < end )
    {  // loop for analyzing the text
        if( *next == ';')
           stmt = next+1;

        if( *next == '$' )
        {
            while( !strchr("\r\n\0", *next ))
            {
                *ecur = *cur;
                ecur++;
                cur++;
                next++;
            }
            continue;
        }

        if( *next != A_delim_IPM && !cc && *next != N_BEGIN )
        {   // Flush text if not relevant for preprocessing
            *ecur = *cur;
            ecur++;
            cur++;
            next++;
            continue;
        }
        if( *next == N_BEGIN && !cc )
        { // '{' - begin name of component
            // back search to a delimiter - pick up the DOD label
            if( *(ecur-1) == N_ARBG )// '['? Space between [ and { not allowed
            {  last = ecur-2;
               // what if previous one is ']'?
               if( *last == N_AREND )
               {  // This name sits in a second index to identifier
                  Xplace=1; last--;
                  // Looking for '[' - max. 20 chars deep back
                  for( ii=0; ii<20; ii++ )
                  {
                     if( *(last-ii) != N_ARBG )
                       continue;
                     else {
                       last -= ii+1; break;
                     }
                  }
               }
               else Xplace=0;
            }
            else {
                  Xplace=0; last = ecur-1;  // first index field
            }
            // Reading DOD ( variable ID )
            prev=last-MAXKEYWD; // Max ID length
            for( ii=0; ii<MAXKEYWD+2; ii++ )
            { // seeking back for a delimiter
                if( strchr( MSDELIM, *(last-ii-1)) )
                { // delimiter found...
                     prev = last-ii;
                     break;
                }
            }
            // Copying DOD label from text
            fillValue( odlab, '\0', MAXKEYWD+2 );
            if( (last+1-prev) > 0 )
                memcpy( odlab, prev, last+1-prev );
            ii = aObj.Find( odlab );
            if( ii<0 )
            { // wrong data object name
                string err = "Identifier (variable) ";
                err += odlab;
                err += " is not known.";
                Error( "E00MSPrep: ", err.c_str() );
            }
            nO = ii;
            iCode = aObj[ii].GetIndexationCode();
            // Getting new indexation code  KD 30.03.01
                switch( iCode )
            {  // analyze indexation code
               default:
                  Error( "E01MSPrep: Unknown indexation base for data object", odlab );
//             case A_reset:
               case A_icx:
               case A_dcx:
               case A_dcsx:
               case A_phx:
                    LNplace = 1; // search index in MULTI
//                  cstate = iCode;
                    break;
               case A_ICx:
               case A_ACx:
               case A_DCx:
               case A_DCSx:
               case A_PHx:
                    LNplace = 0;   // search index in RMULTS
//                  cstate = iCode;
                    break;
               // Tables
               case A_ICxDCx:
               case A_DCxICx:
               case A_PHxICx:
               case A_ICxPHx:
                    LNplace = 0; // search index in RMULTS
//                  cstate = iCode;
                    break;
               case A_icxdcx:
               case A_dcxicx:
               case A_phxicx:
               case A_icxphx:
                    LNplace = 1;   // search index in MULTI
//                  cstate = iCode;
                    break;
            }
            cstate = iCode;
            // preparing for getting species/phase/icomp/compos name in
            fillValue( name, ' ', 63 );
            name[63] = 0;
            cc = 1;
            cur++;
            next++;
            continue;
        }
        if( *next == A_delim_IPM && !cc )
        {   // An old-style switch found
            next++;
            cur++;    // change state of scanner
            next++;   // Skip indexation code  KD 30.03.01
            cur++;
            continue;
        }
        if( *next == N_END && cc > 0 && cc < 38 )
        { // '}' - end name of component
            memcpy( name, cur, next-cur );
            name[next-cur+1] = 0;
            cc = 0;
            next++;
            cur = next;
            // search and replace component name by current index
            switch( cstate )
            {
            case A_icx:
            case A_ICx:
                i = find_icnum( name, LNplace );
                break;
            case A_DCx:
                i = find_dcnum( name, JB, JE, LNplace, stmt );
                break;
            case A_DCSx:
                i = find_dcnum( name, JB, JE, LNplace,stmt );
                i -=  TRMults::sm->GetMU()->Ls - TRMults::sm->GetMU()->Lads ;
                break;
            case A_dcx:
                i = find_dcnum( name, jb, je, LNplace,stmt );
                break;
            case A_dcsx:
                i = find_dcnum( name, jb, je, LNplace,stmt );
                i -= pmp->Ls - pmp->Lads;
                break;
            case A_phx:
            case A_PHx:
                i = find_phnum( name, LNplace );
                break;
            case A_ACx:
                i = find_acnum( name, LNplace );
                break;
            case A_ICxDCx:
                if( !Xplace )
                   i = find_icnum( name, LNplace );
                else
                   i = find_dcnum( name, JB, JE, LNplace, stmt );
                break;
            case A_icxdcx:
                if( !Xplace )
                   i = find_icnum( name, LNplace );
                else
                   i = find_dcnum( name, jb, je, LNplace, stmt );
                break;
            case A_DCxICx:
                if( Xplace )
                   i = find_icnum( name, LNplace );
                else
                   i = find_dcnum( name, JB, JE, LNplace, stmt );
                break;
            case A_dcxicx:
                if( Xplace )
                   i = find_icnum( name, LNplace );
                else
                   i = find_dcnum( name, jb, je, LNplace, stmt );
                break;
            case A_ICxPHx:
            case A_icxphx:
                if( Xplace )
                   i = find_phnum( name, LNplace );
                else
                   i = find_icnum( name, LNplace );
                break;
            case A_PHxICx:
            case A_phxicx:
                if( !Xplace )
                  i = find_phnum( name, LNplace );
                else
                  i = find_icnum( name, LNplace );
                break;
            }
            if( !get_ndx )
              pj=i;
            else
              pj = get_ndx( i, nO, Xplace );
            switch( cstate )
            { // change name in the text
            case A_icx:
            case A_ICx: // index
            case A_dcx:
            case A_dcsx:
            case A_DCx:
            case A_DCSx:
            case A_PHx:
            case A_ACx:
            case A_phx:
            case A_ICxDCx:
            case A_icxdcx:
            case A_DCxICx:
            case A_dcxicx:
            case A_PHxICx:
            case A_phxicx:
            case A_ICxPHx:
            case A_icxphx:
                break;
            default:
                Error( "E02MSPrep: Cannot substitute index for ", cur );
            }
            if( *(ecur-1) != N_ARBG )
                sprintf( nbuf, "[%d]", pj );
            else
                sprintf( nbuf, "%d", pj );
            lb = strlen( nbuf );
//            strcpy( ecur, nbuf );
            memcpy( ecur, nbuf, lb );
            ecur += lb;
            continue;
        }
        if( cc > 0 && cc < 38 ) // reading name to pre-process
        { next++;
            cc++;
            continue;
        }
        Error( "E03MSPrep: too long name for index substitution", cur );
    } // end while
    *ecur = 0;
}




//--------------------- End of muload.cpp ---------------------------


