//-------------------------------------------------------------------
// $Id: ms_muload.cpp 1156 2008-12-15 17:50:38Z gems $
//
// (c) 1992-2007 S.Dmitrieva, D.Kulik, K.Chudnenko, I.Karpov
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

#include "m_syseq.h"
#include "s_formula.h"

enum translat_codes { // codes for translations of math script equations
    N_BEGIN = '{',  N_END = '}',    A_reset = '#',
    N_ARBG =  '[',  N_AREND = ']',  A_arcnt = '@',  EQSET_DELIM = '@',
    A_delim_IPM = '\\',
    // Indexes in the project (system)
    A_ICx = 'I', A_DCx = 'J', A_PHx = 'A', A_ACx = 'K', A_DCSx = 'S',
    // Indexes in MULTI
    A_icx = 'i', A_dcx = 'j', A_phx = 'a', A_dcsx = 's',
    // No indexation related to this data object
    A_NOx = 'N',
    // Tables [I][J], [i][j], [J][I], ...
    A_ICxDCx = 'B', A_icxdcx = 'b', A_DCxICx = 'C', A_dcxicx = 'c',
    A_PHxICx = 'D', A_phxicx = 'd', A_ICxPHx = 'E', A_icxphx = 'e'
};

// Loading of scripts for the non-ideality models of
// phases - solutions into structure for calculations of equlibria
// Uses the IPN stack calculation module (c) 1991-1996 S.Dmytrieva
//
void TMulti::SolModLoad( )
{
    int kk, k, j, jj, jp, jkd,
    JB, JE=0, jb, je=0, kc, kd, kce=0, kde=0, kx, kxe=0, ksn = 0, ksf=0, Type=0;
    //vstr pkey(MAXRKEYLEN);
    vstr modT(164);
    char *sMod;
    time_t crt;
    TPhase* aPH = TPhase::pm;
    RMULTS* mup = TRMults::sm->GetMU();
    SYSTEM *syp = TSyst::sm->GetSY();

if( pmp->pIPN >= 1 )           //SD 29/11/2006
    return;
    ErrorIf( !pmp->FIs, "SolModLoad", "No phases-solutions!" );
    // reallocating arrays for script calculations
    if( pmp->pIPN <= 0 )
    {
        qEp.Clear();
        qEd.Clear();
    }
    // Data extraction from Phase records
    for( kk=0, k=-1; kk<mup->Fis; kk++ )
    {
        JB = JE;
        JE += mup->Ll[kk];
        if( syp->Pcl[kk] == S_OFF || kk >= mup->Fis )
            continue;
        k++;
        pmp->LsMod[k*3] = pmp->LsMod[k*3+1] = pmp->LsMod[k*3+2] = 0;
        pmp->LsMdc[k*3] = pmp->LsMdc[k*3+1] = pmp->LsMdc[k*3+2] = 0;
        jb=je;
        je+= pmp->L1[k];

        // Indexes for extracting data from IPx, PMc and DMc arrays
        kx = kxe;                  // added 07.12.2006 by KD
        kc = kce;
        kd = kde;

        if( pmp->L1[k] == 1 )
            continue;  // one component is left in the multicomponent phase
        aPH->TryRecInp( mup->SF[kk], crt, 0 ); // reading phase record
        // Added SD 20/01/2010
        if( aPH->php->Ppnc == S_ON && aPH->php->npxM > 0 )
            CompressPhaseIpxt( kk );
        // selecting type of the model
        memcpy( modT, aPH->php->sol_t, MAXKEYWD );
        memcpy( pmp->sMod[k], modT, MAXKEYWD );
        pmp->sMod[k][6] = aPH->php->kin_t[0];
        pmp->sMod[k][7] = aPH->php->kin_t[1];
        sMod = pmp->sMod[k];

        // 16/11/2010 added for multi-site mixed moodels
        if( aPH->php->nMoi >0 )
        { TFormula aFo;
          TCStringArray form_array;

           // build formula list
           for( jj=jb, j=JB; j < JE; j++ )
           { // set indexes of components - eliminating those switched off
                 if( syp->Dcl[j] == S_OFF )
                     continue;
                 form_array.Add( aFo.form_extr( j, mup->L, mup->DCF ) );
           } /* j */
           ErrorIf( form_array.GetCount()!=pmp->L1[k],"SolModLoad", "Test error."  );

           // get moiety structure from phase
           aPH->MakeSublatticeLists( form_array );
           pmp->LsMdc[k*3+1] = aPH->php->nSub;
           pmp->LsMdc[k*3+2] = aPH->php->nMoi;

           // realloc memory
           if( ksn+pmp->LsMdc[k*3+1]*pmp->LsMdc[k*3+2]*pmp->L1[k] >
                   (int)(sizeof( pmp->MoiSN )/sizeof(double)))
              pmp->MoiSN = (double *) aObj[ o_wi_moisn ].Alloc(
                     (ksn+pmp->LsMdc[k*3+1]*pmp->LsMdc[k*3+2]*pmp->L1[k]), 1, D_ );
           ErrorIf( pmp->MoiSN == NULL, "SolModLoad",
                         "Error in reallocating memory for pmp->MoiSN." );
           fillValue( pmp->MoiSN+ksn, 0., pmp->LsMdc[k*3+1]*pmp->LsMdc[k*3+2]*pmp->L1[k] );

           // copy data to MoiSN
           for( j=0, jj=jb; jj < je; j++, jj++ )
           { // set indexes of components - eliminating those switched off
              for( int ii = 0; ii< pmp->LsMdc[k*3+2]; ii++ )
              {
                 int site = aPH->php->nxSub[j*pmp->LsMdc[k*3+2]+ii];
                 if( site >=0 && site < pmp->LsMdc[k*3+1] )
                    pmp->MoiSN[ksn+ j*pmp->LsMdc[k*3+1]*pmp->LsMdc[k*3+2] +site*pmp->LsMdc[k*3+2]+ii]=
                              aPH->php-> OcpN[j*pmp->LsMdc[k*3+2]+ii];
              }
           } /* jj */
           // realloc memory for the collection of site fractions arrays
           if( ksf+pmp->LsMdc[k*3+1]*pmp->LsMdc[k*3+2] >
                   (int)(sizeof( pmp->SitFr )/sizeof(double)))
              pmp->SitFr = (double *) aObj[ o_wo_sitfr ].Alloc(
                     (ksf+pmp->LsMdc[k*3+1]*pmp->LsMdc[k*3+2]), 1, D_ );
           ErrorIf( pmp->SitFr == NULL, "SolModLoad",
                         "Error in reallocating memory for pmp->SitFr." );
           fillValue( pmp->SitFr+ksf, 0., pmp->LsMdc[k*3+1]*pmp->LsMdc[k*3+2] );

       }
 // potentially an error - should be set in any DCE_LINK mode, also SM_UNDEF ?
        switch( modT[DCE_LINK] )
        {
        case SM_UNDEF:  // no script equations were specified
               if( modT[SGM_MODE] != SM_STNGAM )
                  continue;
               pmp->LsMod[k*3] = aPH->php->ncpN;  // number of interaction parameters
               pmp->LsMod[k*3+1] = aPH->php->npxM; // max. order of interaction parameters
               pmp->LsMod[k*3+2] = aPH->php->ncpM; // number of coeffs per int.parameter
               pmp->LsMdc[k*3] = aPH->php->nscM; // changed 07.12.2006  KD
               goto LOAD_NIDMCOEF;
        case SM_PRIVATE_:
        case SM_PUBLIC:   // nonideal solution
               //  Changed 07.12.2006   KD
               pmp->LsMod[k*3] = aPH->php->ncpN;  // number of interaction parameters
               pmp->LsMod[k*3+1] = aPH->php->npxM; // max. order of interaction parameters
               pmp->LsMod[k*3+2] = aPH->php->ncpM; // number of coeffs per int.parameter
               pmp->LsMdc[k*3] = aPH->php->nscM; // changed 07.12.2006  KD
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
//      aObj[ o_nncp].SetPtr( pmp->LsMod+k );  changes 07.12.2006  KD
            aObj[ o_nncp].SetPtr( pmp->LsMod+k*3 );
            aObj[ o_nncp].SetDim( 1, 3 );
            aObj[ o_nncd].SetPtr( pmp->LsMdc+k*3 );
            aObj[ o_nncd].SetDim( 1, 1 );
            aObj[ o_ndc ].SetPtr( pmp->L1+k );
            aObj[ o_nez ].SetPtr( pmp->EZ+jb );
            aObj[o_nez].SetN( pmp->L1[k]);

//            aObj[ o_npcv].SetPtr( pmp->PMc+kc );
//            aObj[o_npcv].SetDim( 1, pmp->LsMod[k]);  // Changed 07.12.2006 KD
            aObj[ o_npcv].SetPtr( pmp->PMc+kc );
            aObj[o_npcv].SetDim( pmp->LsMod[k*3], pmp->LsMod[k*3+2]);
            //  Object for indexation of interaction parameters
            aObj[ o_nu].SetPtr( pmp->IPx+kx );
            aObj[o_nu].SetDim( pmp->LsMod[k*3], pmp->LsMod[k*3+1]);

            aObj[ o_ndcm].SetPtr( pmp->DMc+kd );
            aObj[o_ndcm].SetDim( pmp->L1[k], pmp->LsMdc[k*3]);

            aObj[ o_nmvol].SetPtr(pmp->Vol+jb );
            aObj[o_nmvol].SetN( pmp->L1[k]);
            aObj[ o_nppar].SetPtr(pmp->G0+jb );   // Changed 10.12.2008 by DK
            aObj[ o_nppar].SetN( pmp->L1[k]);
//            aObj[ o_ngtn].SetPtr( pmp->G0+jb );
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
        if( pmp->LsMod[k*3] )
        { // loading interaction parameters - rewritten 07.12.2006 by KD
          if( aPH->php->pnc )
          {
              if( kc+pmp->LsMod[k*3]*pmp->LsMod[k*3+2] >
                  (int)(sizeof( pmp->PMc )/sizeof(double)))
                 pmp->PMc = (double *) aObj[ o_wi_pmc ].Alloc(
                   (kc+pmp->LsMod[k*3]*pmp->LsMod[k*3+2]), 1, D_ );
              ErrorIf( pmp->PMc == NULL, "SolModLoad",
                       "Error in reallocating memory for pmp->PMc." );

              copyValues( pmp->PMc+kc, aPH->php->pnc, (pmp->LsMod[k*3]*pmp->LsMod[k*3+2]));
          }
          else { // no array with interaction parameters in the Phase record
            pmp->LsMod[k*3] = 0;
            pmp->LsMod[k*3+2] = 0;
          }
          //  Copying the IP index array from Phase to MULTI
          if( aPH->php->ipxt )
          {
             if( kx+pmp->LsMod[k*3]*pmp->LsMod[k*3+1] >
                (int)(sizeof( pmp->IPx )/sizeof(long int)))
             pmp->IPx = (long int *) aObj[ o_wi_ipxpm ].Alloc(
                (kx+pmp->LsMod[k*3]*pmp->LsMod[k*3+1]), 1, L_ );
             ErrorIf( pmp->IPx == NULL, "SolModLoad",
                      "Error in reallocating memory for pmp->IPx" );

             copyValues( pmp->IPx+kx, aPH->php->ipxt,
                     (pmp->LsMod[k*3]*pmp->LsMod[k*3+1]));
          }
          else
             pmp->LsMod[k*3+1] = 0;  // no IP indexation table
        }
        if( pmp->LsMdc[k*3] )
        {   // coefficients for end member components
          if( aPH->php->scoef )
          {
            if( kd+pmp->LsMdc[k*3]*pmp->L1[k]
                 > (int)(sizeof( pmp->DMc )/sizeof(double)))
                pmp->DMc = (double *) aObj[ o_wi_dmc ].Alloc(
                   kd+pmp->LsMdc[k*3]*pmp->L1[k], 1, D_ );
            ErrorIf( pmp->DMc == NULL, "SolModLoad",
                    "Error in reallocating memory for pmp->DMc." );
            for( jj=jb, j=JB, jkd=0; j < JE; j++ )
            { // set indexes of components - eliminating those switched off
                if( syp->Dcl[j] == S_OFF )
                    continue;
                jp = mup->Pl[j]; // mup->Pl[pmp->muj[j]];
                copyValues( pmp->DMc+kd+jkd, aPH->php->scoef+jp*pmp->LsMdc[k*3],
                        pmp->LsMdc[k*3]);
                jkd += pmp->LsMdc[k*3]; jj++;
            } /* j */
          }
          else pmp->LsMdc[k*3] = 0; // no DC coefficients
        }
        kxe += pmp->LsMod[k*3]*pmp->LsMod[k*3+1];
        kce += pmp->LsMod[k*3]*pmp->LsMod[k*3+2];  // Changed 10.12.2006  by KD&SD
        kde += pmp->LsMdc[k*3]*pmp->L1[k];
        ksn += pmp->LsMdc[k*3+1]*pmp->LsMdc[k*3+2]*pmp->L1[k];
        ksf += pmp->LsMdc[k*3+1]*pmp->LsMdc[k*3+2];
     } // k
//    if( ksn<=0 )
//      pm.MoiSN  = (double *)aObj[ o_wi_moisn ].Free();

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
void TMulti::sm_text_analyze( int nph, int Type,
                               int JB, int JE, int jb, int je )
{
    char *erscan = 0;
    gstring save;

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
            qEd.AddAt( new IPNCalc(), nph );
            qEd[nph].GetEquat( (char *)aObj[o_nwtext].GetPtr() );
        }
        else
        {
            qEp.AddAt( new IPNCalc(), nph );
            qEp[nph].GetEquat( (char *)aObj[o_nwtext].GetPtr() );
        }
    }
    catch( TError& xcpt )
    {
        vfMessage(window(), xcpt.title, xcpt.mess);
        /*bool   iRet = */
        TPhase::pm->CheckEqText(  erscan,
           "E97MSTran: Error in Phase non-ideality model math scripts: " );
        /*  if( iRet )  goto AGAIN;  */
        Error(  "SolModLoad", xcpt.mess.c_str() );
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
gstring TMulti::PressSolMod( int nP )
{
    int j, jp, k, ks, jb=0, je=0, EGlen;
    char *EGb;
    TPhase* aPH=TPhase::pm;
    RMULTS* mup = TRMults::sm->GetMU();
    SYSTEM *syp = TSyst::sm->GetSY();
    gstring etext;

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
            etext += gstring( EGb, 0, EGlen );
            etext +=  "\n" ;
        }
    }
    return etext;
}

//------------------------------------------------------------------
//Get groop of equations number jp from Etext
// Returns *EGlen=0 if there is no equations for the group jp
//
char *TMulti::ExtractEG( char *Etext, int jp, int *EGlen, int Nes )
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
int TMulti::find_icnum( char *name, int LNmode )
{
    int i;
    vstr ICs(IC_RKLEN+10);
    char *nbg, *nend;
    TFormula aFo;

    nbg = name;
    nend = name+strlen(name)-1;
    fillValue( ICs.p, ' ', IC_RKLEN );
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
        Error( ICs.p, "E04MSPrep: IC name cannot be found in Multi lists..." );
    }
    if( LNmode == 0 )
    {
        RMULTS* mup = TRMults::sm->GetMU();
        for( i=0; i<mup->N; i++ )
        {
            if( !memcmp( ICs, mup->SB[i], MAXICNAME+MAXSYMB ))
                return i;
        }
        Error( ICs.p, "E14MSPrep: IC name cannot be found in Project lists..." );
    }
    Error( ICs.p, "E09MSPrep: Invalid IC search mode parameter..." );
    return -1;
}

// Search of component index by name ( *name )
// Find index DC: LNmode=0: by lists RMULTS;  mup->
//		  LNmode=1: by lists MULTI;   pmp->
//  jb and je - range to search
//  Return index >=0 or exeption if no match
//
int TMulti::find_dcnum( char *name, int jb, int je, int LNmode, char *stmt )
{
    int j, jf[20], ii=0, len, jj;
    //vstr pbuf(164);

    len = strlen( name );
    if( LNmode == 1 )
    {
        for( j=jb; j<=je && ii<20; j++ )
            if(!memcmp(name, pmp->SM[j], min(len,MAXDCNAME)))
                jf[ii++] = j;
        if( !ii )
            Error( name, "E05MSPrep: DC name cannot be found in Multi lists..." );
    }
    if( LNmode == 0 )
    {
        for( j=jb; j<=je && ii<20; j++ )
            if(!memcmp(name, TRMults::sm->GetMU()->SM[j]+MAXSYMB+MAXDRGROUP,
                       min(len,MAXDCNAME)))
                jf[ii++] = j;
        if( !ii )
            Error( name, "E15MSPrep: DC name cannot be found in Project lists..." );
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

    gstring pbuf = "Please, select DC index for the DC name ";
            pbuf += name;
            pbuf += "\n in the first script statement below \n";
            pbuf += stmt;
    //sprintf( pbuf, "Please, select DC index for the DC name %s\n in the first script statement below %s", name, stmt );

   j = vfChoice( window(),  "W20MSPrep: ", pbuf.c_str(), ii, jf);
   if( j<0 )
        Error( "E20MSPrep:", "Preprocessing cancelled by the user..." );

   return( j );
}

// Search of phase index by name ( *name )
// Find index: LNmode=0: in lists RMULTS; mup->
//	       LNmode=1: in lists MULTI;  pmp->
//  Return index >=0 or exception if no match
//
int TMulti::find_phnum( char *name, int LNmode )
{
    int k, kf[8], ii=0, len;

    len = strlen( name );
    if( LNmode == 1 )
    {
        for( k=0; k < pmp->FI && ii < 8; k++ )
            if( !memcmp(name, pmp->SF[k]+MAXSYMB, min(len,MAXPHNAME)))
                kf[ii++] = k;
        if( !ii )
          Error( name, "E06MSPrep: Phase name cannot be found in Multi lists..." );
    }
    if( LNmode == 0 )
    {
        RMULTS* mup = TRMults::sm->GetMU();
        for( k=0; k < mup->Fi && ii < 8; k++ )
            if( !memcmp(name, mup->SF[k]+MAXSYMB+MAXPHSYMB, min(len,MAXPHNAME)))
                kf[ii++] = k;
        if( !ii )
          Error( name, "E16MSPrep: Phase name cannot be found in Project lists..." );
    }
    if( ii == 1 )
        return( kf[0] );
    /* more then one useful index */
    vstr pbuf_(164);
    gstring pbuf;
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
int TMulti::find_acnum( char *name, int LNmode )
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
        Error( name, "E07MSPrep: PCO name cannot be found in Project lists..." );
    if( ii == 1 )
        return( jf[0] );
    /* more then one matching index */
    //vstr pbuf(164);
    //sprintf( pbuf, "%d of PCO indices found for the PCO name %s (->%d) \n"
    //         "Take the first one and continue (Y) or cancel (N)?", ii, name, LNmode );
    vstr pbuf_(164);
    gstring pbuf;
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
void TMulti::ET_translate( int nOet, int nOpex, int JB, int JE, int jb, int je,
        tget_ndx *get_ndx )
{
    size_t eLen, ls, lb;
    int i=0, ii, pj, LNplace=1, Xplace=0, nO=0;
    char cstate, cc, *etext, *pexpr, *ecur, *cur, *next, *end,
      *prev, *last, *stmt, iCode, odlab[MAXKEYWD+2];
    vstr name(164), nbuf(164);

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
                gstring err = "Identifier (variable) ";
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
            fillValue( name.p, ' ', 63 );
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


// Added  SD 22/01/2010
// Build list names of index for object
void TMulti::getNamesList( int nO, TCStringArray& lst )
{
   lst.Clear();

   int i, iCode = aObj[nO].GetIndexationCode();
   RMULTS* mup = TRMults::sm->GetMU();

   long int nOsz = aObj[nO].GetN();

    // Getting new indexation code
   switch( iCode )
   {  // analyze indexation code
       default: //break;
          Error( "E01MSWin: Unknown indexation base for data object",aObj[nO].GetKeywd()  );
       case A_icx:
                  for( i=0; i<pmp->N; i++ )
                     lst.Add( gstring( pmp->SB[i], 0, MAXICNAME ));
                  break;
       case A_dcx:
       case A_dcxicx:
                   for( i=0; i<min(pmp->L,nOsz); i++ )
                       lst.Add( gstring( pmp->SM[i],0, MAXDCNAME) );
                   break;
       case A_dcsx:
                   for( i=pmp->Ls + pmp->Lads; i<pmp->L; i++ )
                       lst.Add( gstring( pmp->SM[i],0, MAXDCNAME) );
                   break;
       case A_phx:
                   for( i=0; i<min(pmp->FI,nOsz); i++ )
                     lst.Add( gstring( pmp->SF[i]+MAXSYMB, 0, MAXPHNAME));
                   break;
       case A_phxicx:
                   for( i=0; i<pmp->N; i++ )
                      lst.Add( gstring( pmp->SB[i], 0, MAXICNAME ));
                   break;
                   //for( i=0; i<pmp->FIs; i++ )
                   //  lst.Add( gstring( pmp->SF[i]+MAXSYMB, 0, MAXPHNAME));
                   //break;
       case A_ICx:
                  for( i=0; i<mup->N; i++ )
                       lst.Add( gstring( mup->SB[i], 0, MAXICNAME ));
                   break;
       case A_ACx:
                  for( i=0; i<mup->La; i++ )
                      lst.Add( gstring( mup->SA[i], 0, MAXCMPNAME ));
                   break;
       case A_DCx:
                   for( i=0; i<mup->L; i++ )
                      lst.Add( gstring( mup->SM[i]+MAXSYMB+MAXDRGROUP ,0, MAXDCNAME ));
                   break;
       case A_DCSx:
                  for( i=mup->Ls+mup->Lads; i<mup->L; i++ )
                    lst.Add( gstring( mup->SM[i]+MAXSYMB+MAXDRGROUP ,0, MAXDCNAME ));
                  break;
       case A_PHx:
                   for( i=0; i<mup->Fi; i++ )
                       lst.Add( gstring( mup->SF[i]+MAXSYMB+MAXPHSYMB, 0, MAXPHNAME));
                   break;
   }
}



//--------------------- End of muload.cpp ---------------------------


