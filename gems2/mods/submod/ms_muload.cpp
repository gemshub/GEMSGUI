//-------------------------------------------------------------------
// Id: gems/mods/submod/mu_load.cpp
//      version 2.0.0                           edited 2001-09-08
//  Created: 1992-1997
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
#include <stdio.h>
#include <string.h>

#include "m_param.h"
#include "s_formula.h"
#include "service.h"
#include "visor.h"
#include "visor_w.h"
#include "stepwise.h"

enum translat_codes { /* codes for translations of equations */
    N_BEGIN = '{',  N_END = '}',    A_reset = '#',
    N_ARBG =  '[',  N_AREND = ']',  A_arcnt = '@',  EQSET_DELIM = '@',
    A_delim_IPM = '\\',
    /* indexes by  RMULTS, SYSTEM*/
    A_ICx = 'I', A_DCx = 'J', A_PHx = 'A', A_ACx = 'K',
    /*indexes by MULTI */
    A_icx = 'i', A_dcx = 'j', A_phx = 'a',
    /* No indexation related to this data object */
    A_NOx = 'N',
    /* Tables [I][J], [i][j], [J][I], ... */
    A_ICxDCx = 'B', A_icxdcx = 'b', A_DCxICx = 'C', A_dcxicx = 'c',
    A_PHxICx = 'D', A_phxicx = 'd', A_ICxPHx = 'E', A_icxphx = 'e'
};

// Loading of scripts for the non-ideality models of
// phases - solutions into structure for calculations of equlibria
// Uses IPN stack calculation module (C) S.Dmytrieva 1991-1996
//
void TProfil::SolModLoad( )
{
    int kk, k, j, jp, jkd,
    JB, JE=0, jb, je=0, kc, kd, kce=0, kde=0, Type;
    vstr pkey(MAXRKEYLEN);
    vstr modT(16);
    time_t crt;
    TPhase* aPH = TPhase::pm;

    if( pmp->pIPN >= 1 )
        return;
    ErrorIf( !pmp->FIs, "SolModLoad", "Invalid number of phases-solutions" );
    /* realloc arrays for script calculations */
    if( pmp->pIPN <= 0 )
    {
        multi->qEp.Clear();
        multi->qEd.Clear();
    }
    for( kk=0, k=-1; kk<mup->Fis; kk++ )
    {
        JB = JE;
        JE += mup->Ll[kk];
        if( syp->Pcl[kk] == S_OFF || kk >= mup->Fis )
            continue;
        k++;
        jb=je;
        je+= pmp->L1[k];
        kc = kce;
        kce += pmp->LsMod[k];
        kd = kde;
        kde += pmp->LsMdc[k]*pmp->L1[k];
        if( pmp->L1[k] == 1 )
            continue;  /* one component left in multicomponent phase  */
        aPH->TryRecInp( mup->SF[kk], crt, 0 ); /*read information from phase */
        /* select type of model*/
        /*  modT = pmp->sMod[k]; */
        memcpy( modT, aPH->php->sol_t, MAXKEYWD );
        memcpy( pmp->sMod[k], modT, MAXKEYWD );
        switch( modT[DCE_LINK] )
        {
        case SM_UNDEF:  /* no equations */
            if( modT[SGM_MODE] != SM_STNGAM )
                continue;
            pmp->LsMod[k] = aPH->php->ncpN * aPH->php->ncpM;
            pmp->LsMdc[k] = aPH->php->nscM; // * aPH->php->nscN;
            goto LOAD_NIDMCOEF;
        case SM_PRIVATE:
        case SM_PUBLIC:   /* nonideal solution */
            pmp->LsMod[k] = aPH->php->ncpN * aPH->php->ncpM;
//          pmp->LsMdc[k] = aPH->php->nscN * aPH->php->nscN;  ??
          pmp->LsMdc[k] = aPH->php->nscM;
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
            aObj[ o_nu  ].SetPtr( pmp->U );
            aObj[ o_nu ].SetN( pmp->N);
            aObj[ o_nqp ].SetPtr( pmp->Qp+k*20 );
            aObj[ o_nqd ].SetPtr( pmp->Qd );       /* 20 cells! */
            aObj[ o_nncp].SetPtr( pmp->LsMod+k );
            aObj[ o_nncd].SetPtr( pmp->LsMdc+k );
            aObj[ o_ndc ].SetPtr( pmp->L1+k );
            aObj[ o_nez ].SetPtr( pmp->EZ+jb );
            aObj[o_nez].SetN( pmp->L1[k]);
            aObj[ o_npcv].SetPtr( pmp->PMc+kc );
            aObj[o_npcv].SetDim( 1, pmp->LsMod[k]);
            aObj[ o_ndcm].SetPtr( pmp->DMc+kd );
            aObj[o_ndcm].SetDim( pmp->L1[k], pmp->LsMdc[k]);
            aObj[ o_nmvol].SetPtr(pmp->Vol+jb );
            aObj[o_nmvol].SetN( pmp->L1[k]);
            aObj[ o_nppar].SetPtr(pmp->VL+jb );
            aObj[ o_nppar].SetN( pmp->L1[k]);
            aObj[ o_ngtn].SetPtr( pmp->G0+jb );
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
            aObj[ o_nbmol].SetPtr(pmp->FVOL+k ); /* Phase volume */
            aObj[ o_nxx].SetPtr(  pmp->X+jb );
            aObj[o_nxx].SetN( pmp->L1[k]);
            aObj[ o_nwx].SetPtr(  pmp->Wx+jb );
            aObj[o_nwx].SetN( pmp->L1[k]);
            aObj[ o_nmju].SetPtr( pmp->Fx+jb );
            aObj[o_nmju].SetN( pmp->L1[k]);
            break;
        default: /* error code of model */
            Error( "SolModLoad", "Error code of model." );
        }
        /* translate equations of model  */
        if( !aPH->php->pEq && !aPH->php->dEq )
        { /* Error no equations in record */
            if( vfQuestion(window(), "SolModLoad",
                           "No expressions in non-ideality model: take as ideal?" ))
                break;
            else Error( "SolModLoad", "Error: no equations in record." );
        }
        if( aPH->php->pEq && *aPH->php->pEq && modT[SPHAS_DEP] != SM_UNDEF )
        { // translate expressions in non-ideality model fo all phase or/and solvent
            aObj[ o_neqtxt].SetPtr( aPH->php->pEq );
            Type = 0;
            if( pmp->PHC[k] != PH_SORPTION && pmp->PHC[k] != PH_POLYEL )
                sm_text_analyze( k, Type, JB, JE, jb, je );
            else sm_text_analyze( k, Type, 0, JE, 0, je );
        }
        else if( modT[SPHAS_DEP] == SM_UXDEP
                 /* || modT[SPHAS_TYP] == SM_ASYMM */ )
            Error( "SolModLoad", "Error?: modT[SPHAS_DEP] == SM_UXDEP." );
        if( aPH->php->dEq && *aPH->php->dEq && modT[DCOMP_DEP] != SM_UNDEF )
        { /*translate expressions in non-ideality model fo components */
            if( modT[DCE_LINK] == SM_PUBLIC )
            { /* one group of expressions for all components */
                aObj[ o_neqtxt].SetPtr( aPH->php->dEq );
                Type = 1;
            }
            else
            { /* separate group of expressions for each component */
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

        /* load coefficients in non-ideality model  */
LOAD_NIDMCOEF:

        // Check if coeffs are properly compressed into MULTI !!!!!!!!!!!!!!!
        if( pmp->LsMod[k] )
        { /* coefficients for all phases */
            if( kc+pmp->LsMod[k] > sizeof( pmp->PMc )/sizeof(float))
                pmp->PMc = (float *) aObj[ o_wi_pmc ].Alloc( (kc+pmp->LsMod[k]), 1, F_ );
            memcpy( pmp->PMc+kc, aPH->php->pnc, pmp->LsMod[k]*sizeof(float));
        }
        if( pmp->LsMdc[k] )
        { /* coefficients for components */
            if( kd+pmp->LsMdc[k]*pmp->L1[k] > sizeof( pmp->DMc )/sizeof(float))
                /*        pmp->DMc = (float *)realloc( (void *)pmp->DMc,
                           (kd+pmp->LsMdc[k]*pmp->L1[k])*sizeof(float));
                */        pmp->DMc = (float *) aObj[ o_wi_dmc ].Alloc(
                                         kd+pmp->LsMdc[k]*pmp->L1[k], 1, F_ );

            ErrorIf( pmp->DMc==NULL, "SolModLoad", "Error realloc memory for pmp->DMc." );
            for( j = JB, jkd=0; j < JE; j++ )
            { /*set index of components */
                if( syp->Dcl[j] == S_OFF )
                    continue;
                jp = mup->Pl[j]; /* mup->Pl[pmp->muj[j]]; */
                memcpy( pmp->DMc+kd+jkd, aPH->php->scoef+jp*pmp->LsMdc[k],
                        pmp->LsMdc[k]*sizeof(float));
                jkd += pmp->LsMdc[k];
            } /* j */
        }
    } /* k */
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
void TProfil::sm_text_analyze( int nph, int Type,
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
            multi->qEd.AddAt( new IPNCalc(), nph );
            multi->qEd[nph].GetEquat( (char *)aObj[o_nwtext].GetPtr() );
        }
        else
        {
            multi->qEp.AddAt( new IPNCalc(), nph );
            multi->qEp[nph].GetEquat( (char *)aObj[o_nwtext].GetPtr() );
        }
    }
    catch( TError& xcpt )
    {
        vfMessage(window(), xcpt.title, xcpt.mess);
        /*bool   iRet = */
        TPhase::pm->CheckEqText(  erscan,
                                  "Error in translation of equations in non-ideality model: " );
        /*  if( iRet )  goto AGAIN;  */
        Error(  "SolModLoad", xcpt.mess.c_str() );
    }
    /* if( Type ) // print results to rpn_log.txt
        multi->qEd[nph].PrintEquat( (char *)aObj[o_neqtxt].GetPtr(), fstream& f);
       else
        multi->qEp[nph].PrintEquat( (char *)aObj[o_neqtxt].GetPtr(), fstream& f);
     */
}

//--------------------------------------------------------------------
// Pack script of PRIVATE type of solution phase
gstring TProfil::PressSolMod( int nP )
{
    int j, jp, k, ks, jb, je=0, /*jse=0,*/ EGlen;
    char *EGb;
    TPhase* aPH=TPhase::pm;
    gstring etext;

    for( k=0,ks=-1; k < mup->Fis && ks < nP; k++ )
    {  /* calc indexes */
        jb = je;
        je += mup->Ll[k];
        if( syp->Pcl[k] == S_ON )
        {
            ks++;
            /*jse += pmp->L1[ks];*/
        }
    }
    /* list of components in solution */
    for( j=jb; j<je; j++ )
    {
        if( syp->Dcl[j] == S_OFF )
            continue;
        jp = mup->Pl[j];  /*index component in phase */
        EGlen = 0;
        /* get equation number jp in  PHASE */
        EGb = ExtractEG( aPH->php->dEq, jp, &EGlen, aPH->php->nDC );
        ErrorIf( !EGb, "SolModLoad", "No equations for component." );
        if( EGlen )
        {  /* set text to bufer */
            etext += gstring( EGb, 0, EGlen );
            etext +=  "\n" ;
        }
    }
    return etext;
}

//------------------------------------------------------------------
//Get groop of equations number jp from Etext
// Returns *EGlen=0 if there is no equations for the group jp
char *TProfil::ExtractEG( char *Etext, int jp, int *EGlen, int Nes )
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
int TProfil::find_icnum( char *name, int LNmode )
{
    int i;
    vstr ICs(IC_RKLEN+10);
    char *nbg, *nend;
    TFormula aFo;

    nbg = name;
    nend = name+strlen(name)-1;
    memset( ICs, ' ', IC_RKLEN );
    ICs[/*MAXSYMB*/IC_RKLEN] = 0;
    if( isdigit( name[0] ))
{ /* this is isotope 34S */ MOVEDIGIT:
        ICs[MAXICNAME] = *nbg;
        nbg++;
        if( nbg<nend && isdigit( *nbg ))
            goto MOVEDIGIT;
    }
    else if( islower( name[0] ))
    { /* this is letter  - type of IC, ­ e.g. f Sio, a Hum */
        ICs[MAXICNAME] = *nbg;
        nbg++;
    }
    else
    { /* this is ordinary IC */
        ICs[MAXICNAME] = IC_ELEMENT;
    }
    memcpy( ICs, nbg, min( (unsigned int)6, strlen( nbg )));
    aFo.fixup_ics( ICs );
    if( LNmode == 1 )
    {
        for( i=0; i<pmp->N; i++ )
        {
            if( !memcmp( ICs, pmp->SB[i], MAXICNAME/*+MAXSYMB*/ ))
                return i;
        }
        Error( ICs.p, "Invalid IC name in MULTI..." );
    }
    if( LNmode == 0 )
    {
        for( i=0; i<mup->N; i++ )
        {
            if( !memcmp( ICs, mup->SB[i], MAXICNAME+MAXSYMB ))
                return i;
        }
        Error( ICs.p, "Invalid IC name in RMULTS..." );
    }
    Error( ICs.p, "Invalid search mode parameter..." );
    return -1;
}

// Search of component index by name ( *name )
// Find index DC: LNmode=0: by lists RMULTS;  mup->
//		  LNmode=1: by lists MULTI;   pmp->
//  jb and je - range to search
//  Return index >=0 or exeption if no match
//
int TProfil::find_dcnum( char *name, int jb, int je, int LNmode )
{
    int j, jf[8], ii=0, len;

    len = strlen( name );
    if( LNmode == 1 )
    {
        for( j=jb; j<=je && ii<8; j++ )
            if(!memcmp(name, pmp->SM[j], min(len,MAXDCNAME)))
                jf[ii++] = j;
    }
    if( LNmode == 0 )
    {
        for( j=jb; j<=je && ii<8; j++ )
            if(!memcmp(name, mup->SM[j]+MAXSYMB+MAXDRGROUP, min(len,MAXDCNAME)))
                jf[ii++] = j;
    }
    if( !ii )
        Error( name, "Invalid DC name." );
    if( ii == 1 )
        return( jf[0] );

    /* more then one index found */
    vstr pbuf(164);
    sprintf( pbuf, "%d DC indices found for the name %s (->%d) \n"
             "Take first one and continue (Y) or cancel (N)?", ii, name, LNmode );
    if( !vfQuestion(window(), "MSPreprocessor", /*(const char *)*/pbuf.p ))
        Error( "MSPreprocessor", "Cancel translation..." );

    return( jf[0] );
}

// Search of phase index by name ( *name )
// Find index: LNmode=0: in lists RMULTS; mup->
//	       LNmode=1: in lists MULTI;  pmp->
//  Return index >=0 or exception if no match
//
int TProfil::find_phnum( char *name, int LNmode )
{
    int k, kf[8], ii=0, len;

    len = strlen( name );
    if( LNmode == 1 )
    {
        for( k=0; k < pmp->FI && ii < 8; k++ )
            if( !memcmp(name, pmp->SF[k]+MAXSYMB, min(len,MAXPHNAME)))
                kf[ii++] = k;
    }
    if( LNmode == 0 )
    {
        for( k=0; k < mup->Fi && ii < 8; k++ )
            if( !memcmp(name, mup->SF[k]+MAXSYMB+MAXPHSYMB, min(len,MAXPHNAME)))
                kf[ii++] = k;
    }
    if( !ii )
        Error( name, "Illegal phase name..." );
    if( ii == 1 )
        return( kf[0] );
    /* more then one useful index  */
    vstr pbuf(164);
    sprintf( pbuf, "%d phase indices found for the name %s (->%d) \n"
             "Take 1st and continue (Y) or cancel (N)?", ii, name, LNmode );
    if( !vfQuestion(window(), "MSPreprocessor", /*(const char *)*/pbuf.p ))
        Error( "MSPreprocessor", "Cancel translation..." );

    return( kf[0] );
}

// Search of composition index by name ( *name )
// Find index: LNmode=0: in lists RMULTS; mup->
//	       LNmode=1: in lists MULTI;  pmp->
//  Return index >=0 or exception if no match
//
int TProfil::find_acnum( char *name, int LNmode )
{
    int j, jf[8], ii=0, len;

    if( mup->La <= 0 )
        Error( name, "No compositions list in Modelling Project..." );
    len = strlen( name );
    for( j=0; j<mup->La && ii< 8; j++ )
        if( !memcmp(name, mup->SA[j], min(len,MAXCMPNAME)))
            jf[ii++]=j;
    if( !ii )
        Error( name, "Invalid composition name..." );
    if( ii == 1 )
        return( jf[0] );
    /* more then one matching index */
    vstr pbuf(164);
    sprintf( pbuf, "%d composition indices found for the name %s (->%d) \n"
             "Take 1st and continue (Y) or cancel (N)?", ii, name, LNmode );
    if( !vfQuestion(window(), "MSPreprocessor", /*(const char *)*/pbuf.p ))
        Error( "MSPreprocessor", "Cancel translation..." );

    return( jf[0] );
}

//-----------------------------------------------------------------------
// Preprocessor of species, phases and component names in math scripts.
// Changes fragments like \J ... <objName>{CaSO4@} to <objName>[45]
// or equivalent: <objName>[{CaSO4@}+5] to <objName>[45+5]
//  Default:  \j  (DC in MULTI)
//  Data object nOpex contains initial text of equations;
//         nOet  ­ resulting (pre-processed) text.
//  jb - je range of DC in  MULTI,  JB - JE  - that in RMULTS.
// Modified by KD 30.03.01 to ignore \J ... switches and instead
// obtain indexation codes from data object name and DOD.
//
char* MSDELIM = " +-*/^:[]();=$&|!<>?#\n\t";
//
void TProfil::ET_translate( int nOet, int nOpex, int JB, int JE, int jb, int je)
{
    size_t eLen, ls, lb;
    int i, ii, pj, LNplace=1, Xplace=0;
    char cstate, cc, *etext, *pexpr, *ecur, *cur, *next, *end,
      *prev, *last, iCode=A_NOx, odlab[MAXKEYWD+2]; // added KD 30.03.01
    vstr name(64), nbuf(64);

    etext = (char *)aObj[ nOet ].GetPtr();
    if( etext )
        eLen = aObj[ nOet ].GetM();
    else eLen =0;
    pexpr = (char *)aObj[ nOpex ].GetPtr();

    ls = strlen( pexpr );
    end = pexpr+ls;
    if( ls+1 > eLen )
        etext = (char *)aObj[ nOet ].Alloc( 1, ls+ls/6, S_ );
    ecur = etext;
    cur = pexpr;
    next = cur;
    cc=0;
    cstate = A_dcx;
    while( next < end )
    {  /* loop for analyzing the text */
        if( *next != A_delim_IPM && !cc && *next != N_BEGIN )
        {   // Flush text if not relevant for preprocessing
            *ecur = *cur;
            ecur++;
            cur++;
            next++;
            continue;
        }
        if( *next == N_BEGIN && !cc )
        { /* '{' - begin name of component */
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
            memset( odlab, 0, MAXKEYWD+2 );
            if( (last+1-prev) > 0 )
                memcpy( odlab, prev, last+1-prev );
            ii = aObj.Find( odlab );
            if( ii<0 )
            { // wrong data object name
                gstring err = "Variable ";
                err += odlab;
                err += " is not declared.";
                Error( "MSPreprocessor: ", err.c_str() );
            }
            iCode = aObj[ii].GetIndexationCode();
            // Getting new indexation code  KD 30.03.01
                switch( iCode )
            {  /* analyze indexation code */
               default:
                  Error( "MSPreprocessor: wrong data object", odlab );
//             case A_reset:
               case A_icx:
               case A_dcx:
               case A_phx:
                    LNplace = 1; /* search index in MULTI */
//                  cstate = iCode;
                    break;
               case A_ICx:
               case A_ACx:
               case A_DCx:
               case A_PHx:
                    LNplace = 0;   /* search index in RMULTS */
//                  cstate = iCode;
                    break;
               // Tables
               case A_ICxDCx:
               case A_DCxICx:
               case A_PHxICx:
               case A_ICxPHx:
                    LNplace = 0; /* search index in RMULTS */
//                  cstate = iCode;
                    break;
               case A_icxdcx:
               case A_dcxicx:
               case A_phxicx:
               case A_icxphx:
                    LNplace = 1;   /* search index in MULTI */
//                  cstate = iCode;
                    break;
            }
            cstate = iCode;
            // preparing for getting species/phase/icomp/compos name in
            memset( name, ' ', 63 );
            name[63] = 0;
            cc = 1;
            cur++;
            next++;
            continue;
        }
        if( *next == A_delim_IPM && !cc )
        {   // An old-style switch found
            next++;
            cur++; /* change state of scanner  */
            next++;  // Skip indexation code  KD 30.03.01
            cur++;
            continue;
        }
        if( *next == N_END && cc > 0 && cc < 38 )
        { /* '}' - end name of component */
            memcpy( name, cur, next-cur );
            name[next-cur+1] = 0;
            cc = 0;
            next++;
            cur = next;
            /* search and replace component name by current index */
            switch( cstate )
            {
            case A_icx:
            case A_ICx:
                i = find_icnum( name, LNplace );
                break;
            case A_DCx:
                i = find_dcnum( name, JB, JE, LNplace );
                break;
            case A_dcx:
                i = find_dcnum( name, jb, je, LNplace );
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
                   i = find_dcnum( name, JB, JE, LNplace );
                break;
            case A_icxdcx:
                if( !Xplace )
                   i = find_icnum( name, LNplace );
                else
                   i = find_dcnum( name, jb, je, LNplace );
                break;
            case A_DCxICx:
                if( Xplace )
                   i = find_icnum( name, LNplace );
                else
                   i = find_dcnum( name, JB, JE, LNplace );
                break;
            case A_dcxicx:
                if( Xplace )
                   i = find_icnum( name, LNplace );
                else
                   i = find_dcnum( name, jb, je, LNplace );
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
            pj=i;
            switch( cstate )
            { /*change name in the text */
            case A_icx:
            case A_ICx: /* index */
            case A_dcx:
            case A_DCx:
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
                Error( "MSPreprocessor: Invalid substitution", cur );
            }
            if( *(ecur-1) != N_ARBG )
                sprintf( nbuf, "[%d]", pj );
            else
                sprintf( nbuf, "%d", pj );
            lb = strlen( nbuf );
            strcpy( ecur, nbuf );
            ecur += lb;
            continue;
        }
        if( cc > 0 && cc < 38 ) /* reading name to pre-process */
        { next++;
            cc++;
            continue;
        }
        Error( "MSPreprocessor: too long name for substitution", cur );
    } /* end while */
    *ecur = 0;
}

// -----------------------------------------------------------------
const double tc_ISdelta = 1e-5;
const double LOWESTDC_ = 1e-6;

//Calculation by IPM (preparing for calculation, unpack data)
// key contains SysEq record key
//
void TProfil::MultiCalcInit( const char *key )
{
    int j, k;

    // 15/01/1999 Sveta test set zero
    /*    for( j=0; j<pmp->N; j++ )
        {       pmp->U[j] =0.;
                pmp->C[j] =0.;
        }
        for( j=0; j<pmp->FI; j++ )
        {      pmp->FVOL[j] =0.;
               pmp->FWGT[j] =0.;
               pmp->Falp[j] =0.;
               pmp->YF[j] =0.;
        }
       for( j=0; j<pmp->L; j++ )
       {
         pmp->Y[j] = 0.0;
         pmp->W[j] = 0.0;
         pmp->F[j] = 0.0;

         pmp->lnGam[j] = 0.0;
         pmp->MU[j] =0.;
         pmp->EMU[j] =0.;
         pmp->NMU[j] =0.;
       }
       pmp->VXc =0.;
       pmp->PCI =0.;
       pmp->Yw =0.;
       pmp->FX =0.;
       pmp->pH = pmp->Eh = pmp->pe = 0.0;
    */

    //   if( !pmp->pNP) // Sveta 12/05/99
    //   {    pmp->IC = syp->Mbel;
    //        pmp->logYFk = -9.;
    //	pmp->logXw = -16.;

    //        for( j=0; j<pmp->L; j++ )
    //         pmp->X[j] = pmp->Y[j] =  pmp->lnGam[j] = 0.0;
    //        for( j=0; j<pmp->N; j++ )
    //          pmp->U[j] = 0.0;
    //   }

    // Bulk composition and/or dimensions changed ?
    if( pmp->pBAL < 2 )
        MultiRemake( key );

    // unpack SysEq record
    if( pmp->pESU == 1 && pmp->pNP )
    {
        multi->unpackData(); // loading data from EqstatUnpack( key );
        pmp->IC = 0.;
        for( j=0; j< pmp->L; j++ )
            pmp->X[j] = pmp->Y[j];
        TotalPhases( pmp->X, pmp->XF, pmp->XFA );
    }
    else
        for( j=0; j<pmp->L; j++ )
            pmp->Y[j] = 0.0;

    //  if( wn[W_EQCALC].status )
    //    aSubMod[MD_EQCALC]->ModUpdate("PM_asm4   Assembling IPM arrays (4)");

    // loading thermodynamic data, if neccessary
    if( pmp->pTPD < 2 )
        CompG0Load( );

    for( j=0; j< pmp->L; j++ )
    {
        /* pmp->Y[j] = pmp->X[j]; */
        pmp->G[j] = pmp->G0[j]; /* pmp->GEX[j]; */
    }
    // test phases - solutions and load models
    if( pmp->FIs )
    {
        for( j=0; j< pmp->Ls; j++ )
        {
            pmp->lnGmo[j] = pmp->lnGam[j];
            pmp->Gamma[j] = 1.0;
        }
    }

    if( pmp->FIs && pmp->pIPN <=0 )
    { // not done if already present in MULTI !
        pmp->PD = pa.p.PD;
        SolModLoad();
        /*   pmp->pIPN = 1; */
        GammaCalc( LINK_TP_MODE);
    }
    else
    {
        if( !pmp->FIs )
        { /* no multi-component phases */
            pmp->PD = 0;
            pmp->pIPN = 1;
        }
    }

    // recalc restrictions for DC quantities
    if( pmp->pULR && pmp->PLIM )
        if( Set_DC_limits(  DC_LIM_INIT ))
        {
            if( !vfQuestion(window(), GetName(),
                            "Inconsistent metastability restrictions to DC or phases.\n"
                            "Continue calculation (take those restrictions as trivial)?" ) )
                Error("IPM error: ",
                      "Inconsistent metastability restrictions to DC or phases.");
        }

    // realloc memory for  R and R1
    pmp->R = (double *)aObj[o_w_r].Alloc( pmp->N, pmp->N+1, D_ );
    pmp->R1 = (double *)aObj[o_w_r1].Alloc( pmp->N, pmp->N+1 /* /2 */, D_ );
    memset( pmp->R, 0, pmp->N*(pmp->N+1)*sizeof(double));
    memset( pmp->R1, 0, pmp->N*(pmp->N+1)*sizeof(double));
    //   aObj[o_w_sbh].SetPtr( pmp->SB[0]  ); aObj[ o_w_sbh].SetDim( 1, pmp->N );
    if( !pmp->SFs )
        pmp->SFs = (char (*)[MAXPHNAME+MAXSYMB])aObj[ o_wd_sfs].Alloc(
                       pmp->FI, 1, MAXPHNAME+MAXSYMB );

    // dynamic arrays - begin load
    for( k=0; k<pmp->FI; k++ )
    {
        pmp->XFs[k] = pmp->YF[k];
        pmp->Falps[k] = pmp->Falp[k];
        memcpy( pmp->SFs[k], pmp->SF[k], MAXPHNAME+MAXSYMB );
    }
}

// Finding automatic initial approximation for the IPM algorithm
// using modified simplex method with two-side constraints
//
bool TProfil::AutoInitialApprox( )
{
    int i, j, k, NN;
    double minB=0.0, molB=0.0, sfactor=1.0;

// Kostya correct DK & DHB as automatic
    NN = pmp->N - pmp->E;
    minB=pmp->B[0];
    for(i=0;i<NN;i++)
    {
      if( pmp->B[i] < minB )
          minB = pmp->B[i];
      molB += pmp->B[i];
    }
if( minB < pa.p.DB )  // KD - foolproof 
    minB = pa.p.DB;

//  check Ymin (cutoff)
   if(pmp->lowPosNum>minB*0.01)
      pmp->lowPosNum=minB*0.01;

   sfactor = pow( molB, 0.4 )/7.7;
   pmp->DHBM = sfactor * pa.p.DHB;  // 2.5 root
   pmp->DHBM *= (0.097+0.95/(1+exp(-(log10(minB)+11)/0.4)));

   pmp->DX = pa.p.DK;
   pmp->DX *= sfactor;
   pmp->DX *= (0.097+0.95/(1+exp(-(log10(minB)+6.1)/0.54)));
   if( pmp->DX < 0.01 * pa.p.DK )
       pmp->DX = 0.01 * pa.p.DK;
   pmp->DSM = pa.p.DS;  // Shall we add  * sfactor ?

   pVisor->Update(false);
    // Analyzing if Simplex approximation is needed
    if( !pmp->pNP  )  //  || ( pmp->pNP == -1 &&
                      //  !vfQuestion(window(),  "IPM (initial approximation):",
                      //  "Previous minimization result (Y)\n Automatic SIMPLEX approximation (N)? " )))
    {   // Preparing to call Simplex method
        pmp->FitVar[4] = pa.p.AG;  // smoothing parameter init
        pmp->pRR1 = 0;
        TotalPhases( pmp->X, pmp->XF, pmp->XFA );
//      pmp->IC = 0.0;  Important - for reproducibility of simplex FIA
        if( pa.p.PSM && pmp->FIs )
            GammaCalc( LINK_FIA_MODE);
        pmp->IT = 0;
        pmp->pNP = 0;
        // simplex method is called here
        SimplexInitialApproximation( );
//  STEPWISE (0) - stop point for examining results from simplex IA
STEP_POINT();
        // no multi-component phases?
        if( !pmp->FIs )
            return true; // goto OVER; // solved !
        for( i=0; i<pmp->L; i++ )
            if( pmp->Y[i] <= /* LOWESTDC_ */ pmp->lowPosNum )
                //	       pmp->Y[i] = LOWESTDC_ / 10.;
                //         pmp->Y[i] = pa.p.DFYaq;
            {
                // Trace DC quantity into zeros!
                switch( pmp->DCC[i] )
                {
                case DC_AQ_PROTON:
                case DC_AQ_ELECTRON:
                case DC_AQ_SPECIES:
                    pmp->Y[i] =  pa.p.DFYaq;
                    break;
                case DC_AQ_SOLVCOM:
                case DC_AQ_SOLVENT:
                    pmp->Y[i] =  pa.p.DFYw;
                    break;

                case DC_GAS_H2O:
                case DC_GAS_CO2:
                case DC_GAS_H2:
                case DC_GAS_N2:
                case DC_GAS_COMP:
                case DC_SOL_IDEAL:
                    pmp->Y[i] = pa.p.DFYid;
                    break;

                case DC_SOL_MINOR:
                    pmp->Y[i] = pa.p.DFYh;
                    break;
                case DC_SOL_MAJOR:
                    pmp->Y[i] =  pa.p.DFYr;
                    break;

                case DC_SCP_CONDEN:
                    pmp->Y[i] =  pa.p.DFYc;
                    break;
                    // implementation for adsorption?
                default:
                    pmp->Y[i] =  pa.p.DFYaq;
                    break;
                }

                pmp->Y[i] *= sfactor;  // 2.5 root

            } // i
    }

    else  // Taking previous result as initial approximation
    {
        int jb, je;
        double LnGam, FitVar3;
        /*    pmp->IT *= pa.p.PLLG; */
        FitVar3 = pmp->FitVar[3];
        pmp->FitVar[3] = 1.0;
        TotalPhases( pmp->Y, pmp->YF, pmp->YFA );
        for( j=0; j< pmp->L; j++ )
            pmp->X[j] = pmp->Y[j];
        TotalPhases( pmp->X, pmp->XF, pmp->XFA );
        ConCalc( pmp->X, pmp->XF, pmp->XFA );
        if( pmp->E && pmp->LO )
            IS_EtaCalc();
        /*    if( pa.p.PSM )  corr. 13.4.96 */
        for( k=0, je=0; k<pmp->FI; k++ )
        {
            jb = je;
            je += pmp->L1[k];
            if( pmp->PHC[k] == PH_SORPTION )
            {
                GouyChapman(   jb, je, k );
                SurfaceActivityTerm(   jb, je, k );
            }
            for( j=jb; j<je; j++ )
            {
                LnGam = pmp->lnGam[j];
                if( fabs( LnGam ) > 1e-9 )
                    pmp->Gamma[j] = exp( LnGam );
                else pmp->Gamma[j] = 1.0;
                pmp->F0[j] = Ej_init_calc( 0.0, j, k  );
                pmp->G[j] = pmp->G0[j] + pmp->F0[j];
                pmp->lnGmo[j] = LnGam;
            }
        }
        pmp->FitVar[3] = FitVar3; // Restore smoothing parameter

        if( pmp->pNP <= -1 )
        { //  // With raising zeroed species and phases
           for( i=0; i<pmp->L; i++ )
              if( pmp->Y[i] <= pmp->lowPosNum )
              { // Put trace DC quantity instead of zeros!
                 switch( pmp->DCC[i] )
                 {
                   case DC_AQ_PROTON:
                   case DC_AQ_ELECTRON:
                   case DC_AQ_SPECIES: pmp->Y[i] = pa.p.DFYaq/1000.; break;
                   case DC_AQ_SOLVCOM:
                   case DC_AQ_SOLVENT: pmp->Y[i] = pa.p.DFYw;  break;

                   case DC_GAS_H2O: case DC_GAS_CO2: case DC_GAS_H2:
                   case DC_GAS_N2: case DC_GAS_COMP:
                   case DC_SOL_IDEAL:  pmp->Y[i] = pa.p.DFYid/1000.; break;

                   case DC_SOL_MINOR:  pmp->Y[i] = pa.p.DFYh/1000.; break;
                   case DC_SOL_MAJOR:  pmp->Y[i] = pa.p.DFYr/100.; break;

                   case DC_SCP_CONDEN: pmp->Y[i] = pa.p.DFYc/1000.; break;
                   // implementation for adsorption?
                   default:  pmp->Y[i] = pa.p.DFYaq/1000.; break;
                 }
              } // i
        }
    }
    pmp->MK = 1;
    // Sveta 18/01/1999 test init load
    /*   if( ((TMulti *)aSubMod[MD_MULTI])->flCopy == true )
         ((TMulti *)aSubMod[MD_MULTI])->dyn__test(
                ((TMulti *)aSubMod[MD_MULTI])->copy1);
      ((TMulti *)aSubMod[MD_MULTI])->dyn_new_test(
                ((TMulti *)aSubMod[MD_MULTI])->copy1);
    */
// STEPWISE (1) - stop point to see IA from old solution or raised simplex
STEP_POINT();
    return false;
    //   OVER: /* calc finished */
    //   if( wn[W_EQCALC].status )
    //     aSubMod[MD_EQCALC]->ModUpdate("EQ_done  EQilibrium STATe: computed OK");
    //   return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Main sequence of IPM calculations
//  Main place for implementation of diagnostics and setup
//  of IPM precision and convergence
//  Returns true   if IPM result is OK
//          false  if good result could not be obtained
//

bool TProfil::MultiCalcMain( int &pll, double &FXold )
{
    int i, j, RepeatSel=0, eRet;
    pmp->W1=0;

    if( pmp->pULR && pmp->PLIM )
        if( Set_DC_limits( DC_LIM_INIT ))
            if( !vfQuestion(window(), "IPM: ",
                            "Inconsistent metastability restrictions to DC or phases.\n"
                            "Continue calculation (take those restrictions as trivial)?" ))
                Error("IPM error: " ,
                      "Inconsistent metastability restrictions to DC or phases.");
    /* test insert in valid area */
    mEFD:
     if(pmp->PZ && pmp->W1)
     { for( i=0; i<pmp->L; i++ )
        pmp->Y[i]=pmp->X[i];
      TotalPhases( pmp->Y, pmp->YF, pmp->YFA );
     }
    eRet = EnterFeasibleDomain( );
    if( eRet )
        goto ERET_THINK;

// STEPWISE (2)  - stop point to examine output from EFD()
#ifdef Use_mt_mode

STEP_POINT();

#endif
   /* minimization  IPM */
    eRet = InteriorPointsMethod( );
// STEPWISE (3)  - stop point to examine output from IPM()
STEP_POINT();
ERET_THINK:  // Diagnostics of IPM results !!!!!!!!!!!!!!!!!!!!!!!!!!!!
    if( eRet )
    {   if(eRet==2 )
        { if( pmp->DX<1e-3 )
            {
               pmp->DX *= 10.;
               goto mEFD;
            }
          else
          if( vfQuestion(window(), "IPM:",
            "For a given IPM convergence criterion, vector b is not balanced,\n"
            "or DC standard-state thermodynamic data inconsistent. \n"
            "Browse debug data screen forms (Y) Skip to abnormal exit from IPM (N)?" ))
            {  pVisor->Update( true );
                return true;
            }
        }
     /*   if( !pmp->MK  )
            return true; //goto OVER;
        if( pmp->pNP > 0 ) // error of initial approximation
        {
            pmp->pNP = 0;
            pmp->PD = pa.p.PD;
            goto AGAIN;
        }  */
        else
        { if( pmp->DHBM<1e-5 )
            {
               pmp->DHBM *= 10.;
               goto mEFD;
            }
          else
            if( !vfQuestion(window(),  "Invalid initial approximation for IPM:",
                             "Proceed with automatic SIMPLEX approximation ?" ))
            Error("IPM error: ", "Invalid initial approximation for IPM.");
        }

      /*  else
        {
            pmp->DX *= 10.;
            pmp->pNP = 0;
            pmp->PD = pa.p.PD;
            goto AGAIN;
        }  */
    }
    pmp->FI1 = 0;
    pmp->FI1s = 0;
    for( i=0; i<pmp->FI; i++ )
        if( pmp->YF[i] > 1e-18 )
        {
            pmp->FI1++;
            if( i < pmp->FIs )
                pmp->FI1s++;
        }
    if( !pa.p.PC )    //  ???
        if( pmp->PD < 2 )
            return true; //break;
        else
        {
           // pmp->MK = 1;
            goto ITDTEST;
        }
    /* call Selekt2() */
   PhaseSelect();
   if( pa.p.PC == 2 )
       XmaxSAT_IPM2();  // Install upper limits to xj of surface species

   if( !pmp->MK )
     if( RepeatSel<3 )
       { RepeatSel++;
         goto mEFD;
       }
     else
         cout<< "PhaseSelect : Insert phase cannot be reached."<< endl;
    //   if( !vfQuestion(window(), "PhaseSelect : warning",
    //        "Insert phase cannot be reached. Continue?" ))
    //         return false;

   MassBalanceDeviations( pmp->N, pmp->L, pmp->A, pmp->X, pmp->B, pmp->C);
// STEPWISE (4) Stop point after PhaseSelect()
#ifdef Use_mt_mode

STEP_POINT();

#endif
   pVisor->Update( false );
   if(pmp->PZ && !pmp->W1)
    { pmp->W1++;           // IPM-2 precision algorithm - 1st run
      goto mEFD;
    }
   if(pmp->PZ && pmp->W1 && pmp->W1 <  pa.p.DW )
    for(i=0;i<pmp->N-pmp->E;i++)
     if( fabs(pmp->C[i]) > pmp->DHBM // * pa.p.GAS
         || fabs(pmp->C[i]) > pmp->B[i] * pa.p.GAS )
      if(pmp->W1 < pa.p.DW-1)
        {
            pmp->W1++;          // IPM-2 precision algorithm - 2nd run
            goto mEFD;
        }
       else
       { gstring  buf,buf1;
         vstr pl(5);
         int jj=0;
          for( j=0; j<pmp->N-pmp->E; j++ )
          //  if( fabs(pmp->C[j]) > pmp->DHBM  || fabs(pmp->C[j]) > pmp->B[j] * pa.p.GAS )
            if( fabs(pmp->C[j]) > pmp->B[j] * pa.p.GAS )
            { sprintf( pl, " %-2.2s  ", pmp->SB[j] );
              buf1 +=pl;
              jj=1;
            }
           if(!jj)
               goto ITDTEST;
           buf = "Prescribed balance precision cannot be reached\n for independent components: ";
           buf += buf1;
        //   buf += "Continue?";
        //  if( !vfQuestion(window(), "IPM : warning", buf.c_str() ))
        //      break;
          cout<< buf.c_str()<< endl;
          goto ITDTEST;
       }

ITDTEST: /* test flag modes */
 /*   if( !pmp->MK )
    {  // insert or deleted phase
        FXold = pmp->FX;
        goto RESET_GT;
    }
    if( pa.p.DK != pmp->DX && !pll )
    { // bad convergence - how to improve ?  ???????????????????????
        if( pmp->PD <= 1 )
        {
            pmp->MK = 0;
            pll=1;
            FXold = pmp->FX;
            goto RESET_GT;
        }
    }
    if( pmp->PD == 2 )  // test of total G(X) value
        if( fabs( FXold - pmp->FX ) > tc_ISdelta )
        {
            pmp->MK=0;
            FXold = pmp->FX;
            goto RESET_GT;
        }  */
    /* if( pmp->PD == 3 ) */
    /* pmp->MK = 0 */;
// RESET_GT:  ??????????????????????????????????
    memcpy( pmp->G, pmp->G0, pmp->L*sizeof(double));
    return false;
// AGAIN: Call simplex if necessary
//   return AutoInitialApprox();
}

//Calc equstat method IPM (iterations)
void TProfil::MultiCalcIterations()
{
    pll=0;
    FXold=0.0;

 /*   do
    { // cycle of iterations Selekt
        // Stop calculations here Sveta
        if( MultiCalcMain( pll, FXold ) == true )
            break;

        if( fStopCalc == true )	// 'Cancel' button
            return;	// should we do some cleanup?

    } while( !pmp->MK );  */
    MultiCalcMain( pll, FXold );

    //calc demo data
    for( int ii=0; ii<pmp->N; ii++ )
        pmp->U_r[ii] = pmp->U[ii]*pmp->RT;
    GasParcP();

    /* calc finished */
    //   if( wn[W_EQCALC].status )
    //  aMod[MD_EQCALC]->ModUpdate("EQ_done  EQilibrium STATe: computed OK");
}

// End of file ms_muload.cpp
// ----------------------------------------------------------------
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* Calculation of max.moles of surface species for SAT stabilization
*  to improve IPM-2 convergence at high SAT values  KD 08.03.02
*  xj0 values are placed as upper kunetic constraints 
*/
void TProfil::XmaxSAT_IPM2( void )
{
    int i, j, k, jb, je=0, ist, Cj, iSite[6];
    double XS0,  xj0, XVk, XSk, XSkC, xj, Mm, 
            SATst, a, xjn, q1, q2;

  if(!pmp->DUL )   // not possible to install upper kinetic constraint!
      return;

  for( k=0; k<pmp->FIs; k++ )
  { /* loop on phases */
     jb = je;
     je += pmp->L1[k];
     if( pmp->PHC[k] != PH_SORPTION )
          continue;

    if( pmp->XFA[k] < pmp->DSM ) // No sorbent retained by the IPM 
        continue;
    if( pmp->XF[k]-pmp->XFA[k] < pmp->lowPosNum /* *10. */ )
        continue;  /* No surface species */

    for(i=0; i<6; i++)
        iSite[i] = -1;

    /* Extraction of site indices */
    for( j=jb; j<je; j++ )
    {
        if( pmp->SATT[j] != SAT_SITE )
        {
            if( pmp->DCC[j] == DC_PEL_CARRIER || pmp->DCC[j] == DC_SUR_MINAL ||
                    pmp->DCC[j] == DC_SUR_CARRIER ) continue;
            ist = pmp->SATNdx[j][0] / MSPN; // MSPN = 2 - number of EDL planes
            continue;
        }
        ist = pmp->SATNdx[j][0] / MSPN;
        iSite[ist] = j;
    }

    for( j=jb; j<je; j++ )
    { /* Loop for DC */
        if( pmp->X[j] <= pmp->lowPosNum /* *10. */ )
            continue;  /* This surface DC has been killed by IPM */
        switch( pmp->DCC[j] )  /* code of species class */
        {
        default: /* pmp->lnGam[j] = 0.0; */
            continue;
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
        case DC_IEWC_B:
        case DC_SUR_COMPLEX:
        case DC_SUR_IPAIR:
        case DC_IESC_A:
            /* Calculate ist - index of surface type */
            ist = pmp->SATNdx[j][0] / MSPN;
            /* Cj - index of carrier DC */
            Cj = pmp->SATNdx[j][1];
            if( Cj < 0 )
            {  /* Assigned to the whole sorbent */
                XVk = pmp->XFA[k];
                Mm = pmp->FWGT[k] / XVk;
            }
            else
            { /* Assigned to one of the sorbent end-members */
                XVk = pmp->X[Cj];
                if( XVk < pmp->DSM/10.0 )
                    continue; /* This end-member is zeroed off by IPM */
                Mm = pmp->MM[Cj] * XVk/pmp->XFA[k];  // mol.mass
            }
            XSk = pmp->XFTS[k][ist]; /* Tot.moles of sorbates on surf.type */
            xj = pmp->X[j];  /* Current moles of this surf.species */
            a=1.0; /* Frumkin factor - reserved for extension to FFG isotherm */
            switch( pmp->SATT[j] )
            {
            case SAT_COMP: /* Competitive surface species on a surface type */
                /* a = fabs(pmp->MASDJ[j]); */
                if( iSite[ist] < 0 )
                    xjn = 0.0;
                else xjn = pmp->X[iSite[ist]]; // neutral site does not compete!
                XS0 = pmp->MASDT[k][ist] * XVk * Mm / 1e6
                      * pmp->Nfsp[k][ist]; /* expected total in moles */
                XSkC = XSk - xjn - xj; /* occupied by the competing species;
  	                                   this sorbate cannot compete to itself */
                /* New variant */
                if( XSkC < pa.p.IEPS )
                    XSkC = pa.p.IEPS;
                xj0 = XS0 - XSkC;    /* expected moles of this sorbate */
                if( xj0 < pa.p.IEPS )
                    xj0 = pa.p.IEPS;  /* ensuring that it is non-negative */
// Check!

                pmp->DUL[j] = xj0 - pa.p.IEPS; // XS0*(1.0-pa.p.IEPS);  //pa.p.IEPS;
                break;

            case SAT_NCOMP: /* Non-competitive surface species */
                xj0 = fabs(pmp->MASDJ[j]) * XVk * Mm / 1e6
                      * pmp->Nfsp[k][ist]; /* in moles */

                pmp->DUL[j] = xj0 - pa.p.IEPS; // xj0*(1.0-pa.p.IEPS); //pa.p.IEPS;
                break;

            case SAT_SITE:  /* Neutral surface site (e.g. >O0.5H@ group) */
                XS0 = pmp->MASDT[k][ist] * XVk * Mm / 1e6
                      * pmp->Nfsp[k][ist]; /* in moles */

                pmp->DUL[j] =  XS0-pa.p.IEPS; // xj0*(1.0-pa.p.IEPS);  //pa.p.IEPS;
                break;
            case SAT_INDEF: /* No SAT calculation */
            default:        /* pmp->lnGam[j] = 0.0; */
                break;
            }
        }
     }  /* j */
  } /* k */
}

// Don't forget to clear such pmp->DUL constraints on entering simplex IA!



