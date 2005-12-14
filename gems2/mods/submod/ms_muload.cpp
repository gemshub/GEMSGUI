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
#include <stdio.h>
#include <string.h>

#include "m_param.h"
#include "s_formula.h"
#include "service.h"
#include "visor.h"
//#include "visor_w.h"
#include "stepwise.h"

enum translat_codes { /* codes for translations of equations */
    N_BEGIN = '{',  N_END = '}',    A_reset = '#',
    N_ARBG =  '[',  N_AREND = ']',  A_arcnt = '@',  EQSET_DELIM = '@',
    A_delim_IPM = '\\',
    /* indexes by  RMULTS, SYSTEM*/
    A_ICx = 'I', A_DCx = 'J', A_PHx = 'A', A_ACx = 'K', A_DCSx = 'S',
    /*indexes by MULTI */
    A_icx = 'i', A_dcx = 'j', A_phx = 'a', A_dcsx = 's',
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
    int kk, k, j, jj, jp, jkd,
    JB, JE=0, jb, je=0, kc, kd, kce=0, kde=0, Type;
    vstr pkey(MAXRKEYLEN);
    vstr modT(16);
    char *sMod;
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
            continue;  /* one component left in multicomponent phase */
        aPH->TryRecInp( mup->SF[kk], crt, 0 ); /*read information from phase */
        /* select type of model*/
        /*  modT = pmp->sMod[k]; */
        memcpy( modT, aPH->php->sol_t, MAXKEYWD );
        memcpy( pmp->sMod[k], modT, MAXKEYWD );
sMod = pmp->sMod[k];
        switch( modT[DCE_LINK] )
        {
        case SM_UNDEF:  /* no equations */
            if( modT[SGM_MODE] != SM_STNGAM )
                continue;
            if( modT[SPHAS_TYP] != SM_AQSIT )
            {                          // Not an aq phase with SIT model
               pmp->LsMod[k] = (short)(aPH->php->ncpN * aPH->php->ncpM);
               pmp->LsMdc[k] = aPH->php->nscM; // * aPH->php->nscN;
            }
            else { // Aq phase with SIT model - interaction coeffs must be compressed
               // checking dimensions of the pnc table
               int nCat=0, nAn=0, jj;

               for( jj = jb; jj<je; jj++ )
               {
                   if( pmp->EZ[jj] < 0 )
                    nAn++;
                  else if( pmp->EZ[jj] > 0 )
                    nCat++;
                  else ;
               }
               pmp->LsMod[k] = (short)(nCat * nAn);
               pmp->sitNcat = (short)nCat;
               pmp->sitNan = (short)nAn;
            }
            goto LOAD_NIDMCOEF;
        case SM_PRIVATE_:
        case SM_PUBLIC:   /* nonideal solution */
            pmp->LsMod[k] = (short)(aPH->php->ncpN * aPH->php->ncpM);
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
            aObj[ o_nqp ].SetPtr( pmp->Qp+k*QPSIZE );
            aObj[ o_nqd ].SetPtr( pmp->Qd+k*QDSIZE );  /* QDSIZE cells per phase */
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
            Error( "SolModLoad", "Error code of non-ideality model." );
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
        { /*translate expressions in non-ideality model for components */
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

        // load coefficients in non-ideality models
LOAD_NIDMCOEF:
        // Check if coeffs are properly compressed into MULTI !!!!!!!!!!!!!!!
        if( pmp->LsMod[k] )
        { // coefficients for all phases
            if( kc+pmp->LsMod[k] > (int)(sizeof( pmp->PMc )/sizeof(float)))
                pmp->PMc = (float *) aObj[ o_wi_pmc ].Alloc( (kc+pmp->LsMod[k]), 1, F_ );
            if( aPH->php->pnc )
            {
               if( modT[SPHAS_TYP] != SM_AQSIT )
                   memcpy( pmp->PMc+kc, aPH->php->pnc, pmp->LsMod[k]*sizeof(float));
               else { // Aq phase with SIT model
                  float *ppnc;
                  ppnc = PackSITcoeffs( k, JB, JE, jb, je, pmp->LsMod[k] );
                  if( ppnc )
                  {  // ppnc is actually pmp->sitE !
                     memcpy( pmp->PMc+kc, ppnc, pmp->LsMod[k]*sizeof(float));
                  }
                  else
                     memset( pmp->PMc+kc, 0, pmp->LsMod[k]*sizeof(float) );
               }
            }
            else pmp->LsMod[k] = 0;
        }
        if( pmp->LsMdc[k] )
        { /* coefficients for components */
            if( kd+pmp->LsMdc[k]*pmp->L1[k] > (int)(sizeof( pmp->DMc )/sizeof(float)))
                /*        pmp->DMc = (float *)realloc( (void *)pmp->DMc,
                           (kd+pmp->LsMdc[k]*pmp->L1[k])*sizeof(float));
                */        pmp->DMc = (float *) aObj[ o_wi_dmc ].Alloc(
                                     /* pmp->Ls Predlagayu postavit`*/
                                       kd+pmp->LsMdc[k]*pmp->L1[k], 1, F_ );

            ErrorIf( pmp->DMc == NULL, "SolModLoad", "Error by reallocating memory for pmp->DMc." );
            for( jj=jb, j=JB, jkd=0; j < JE; j++ )
            { /*set index of components */
                if( syp->Dcl[j] == S_OFF )
                    continue;
                jp = mup->Pl[j]; /* mup->Pl[pmp->muj[j]]; */
                memcpy( pmp->DMc+kd+jkd, aPH->php->scoef+jp*pmp->LsMdc[k],
                        pmp->LsMdc[k]*sizeof(float));
               // Correction of CG fluid EoS coeffs
               if( (aPH->php->PphC == PH_FLUID) && (sMod[SPHAS_TYP] == SM_FLUID) )
               {
                 if( tpp->PtvdVg != S_OFF && j-JB < tpp->Lg )
                 {
                    memcpy( pmp->DMc+kd+jkd, tpp->dVg +(j-JB)*4,
                       pmp->LsMdc[k]*sizeof(float));
                    pmp->Pparc[jj] = tpp->Fug[j-JB];
//                   pmp->Pparc[jp] = tpp->Fug[j-JB];
                 }
               }
               jkd += pmp->LsMdc[k]; jj++;
            } /* j */
        }
    } /* k */
    pmp->pIPN = 1;
}

// creates index lists and packs SIT coeffs table taken from Phase
// definition record; returns a pointer to packed coeffs. table
// Essential: aqueous phase must have index k = 0
float *
TProfil::PackSITcoeffs( int k, int JB, int JE, int jb, int je, int nCxA )
{
   int iCat=0, iAn=0, nCat/*=0*/, nAn/*=0*/, pos/*=0*/, icat=0, ian=0, nan/*=0*/;
   short j, js, *nxCAT, *nxAN; //, *nxcat, *nxan;
   gstring spName;
   TPhase* aPH = TPhase::pm;

   if( k )   // not an aqueous phase!
     return 0;
   // allocating dyn memory
   multi->sit_dyn_new();

   nxCAT = (short *)malloc( (JE-JB)*sizeof(short) );
   nxAN = (short *)malloc( (JE-JB)*sizeof(short) );
// !!!!!! check for malloc errors

// filling out MULTI SIT indexes for cations and anions
   for( j = (short)jb; j< (short)je; j++ )
   {
      if( pmp->EZ[j] < 0 )
          pmp->sitXan[ian++] = j;
      else if( pmp->EZ[j] > 0 )
          pmp->sitXcat[icat++] = j;
      else ;
   }
   if( icat * ian != nCxA || icat * ian != pmp->sitNan * pmp->sitNcat )
       Error( "PackSIT01",
              "Inconsistent numbers of cations and anions" );

   memset( pmp->sitE, 0, sizeof(float)*nCxA );
// Analyzing Phase/System DC list for cations and anions
   for( js=0; js < aPH->php->nDC; js++ )
   {  // Determining if cation or anion
      spName = gstring( aPH->php->SM[js], MAXSYMB+MAXDRGROUP, MAXDCNAME);
      spName.strip();
      pos = spName.length()-1;
      while( pos>0 && spName[pos] <= '9' && spName[pos] >= '0' )
          pos--;
      switch( spName[pos] )
      {
         case '-': nxAN[iAn++] = js;
                   break;
         case '+': nxCAT[iCat++] = js;
                   break;
         case '@':
         default:
                   continue;
      }
   }
   nCat = iCat;
   nAn = iAn;

nan = pmp->sitNan;
// Analyzing cations in the phase SIT table
   icat = 0;
   for( iCat = 0; iCat<nCat; iCat++ )
   {
//      J = nxCat[iCat];
      if( syp->Dcl[nxCAT[iCat]] == S_OFF )
         continue; // this cation and the whole line is switched off
      ian = 0;
      for( iAn = 0; iAn<nAn; iAn++ )
      {
         if( syp->Dcl[nxAN[iAn]] == S_OFF )
            continue;   // this anion (column) is switched off
         // Temperature dependence ??    
         pmp->sitE[ icat*nan + ian ] = aPH->php->pnc[ iCat*nAn + iAn ];
         ian++;
      }
      icat++;
   }
// check if ian*icat == nCxA
   if(nxAN)
      free(nxAN);
   if(nxCAT)
      free(nxCAT);
   return pmp->sitE;
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
           "E97MSTran: Error in Phase non-ideality model math scripts: " );
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
        ErrorIf( !EGb,
         "E12MSPrep:", "SolModLoad(): Missing operators group for a phase endmember." );
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
int TProfil::find_dcnum( char *name, int jb, int je, int LNmode )
{
    int j, jf[8], ii=0, len;

    len = strlen( name );
    if( LNmode == 1 )
    {
        for( j=jb; j<=je && ii<8; j++ )
            if(!memcmp(name, pmp->SM[j], min(len,MAXDCNAME)))
                jf[ii++] = j;
        if( !ii )
            Error( name, "E05MSPrep: DC name cannot be found in Multi lists..." );
    }
    if( LNmode == 0 )
    {
        for( j=jb; j<=je && ii<8; j++ )
            if(!memcmp(name, mup->SM[j]+MAXSYMB+MAXDRGROUP, min(len,MAXDCNAME)))
                jf[ii++] = j;
        if( !ii )
            Error( name, "E15MSPrep: DC name cannot be found in Project lists..." );
    }
    if( ii == 1 )
        return( jf[0] );

    /* more then one index found */
    vstr pbuf(164);
    sprintf( pbuf, "%d DC indices found for the name %s (->%d) \n"
             "Take the first one and continue (Y) or cancel (N)?", ii, name, LNmode );
    if( !vfQuestion(window(), "W20MSPrep: ", /*(const char *)*/pbuf.p ))
        Error( "E20MSPrep:", "Preprocessing cancelled by the user..." );

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
        if( !ii )
          Error( name, "E06MSPrep: Phase name cannot be found in Multi lists..." );
    }
    if( LNmode == 0 )
    {
        for( k=0; k < mup->Fi && ii < 8; k++ )
            if( !memcmp(name, mup->SF[k]+MAXSYMB+MAXPHSYMB, min(len,MAXPHNAME)))
                kf[ii++] = k;
        if( !ii )
          Error( name, "E16MSPrep: Phase name cannot be found in Project lists..." );
    }
    if( ii == 1 )
        return( kf[0] );
    /* more then one useful index */
    vstr pbuf(164);
    sprintf( pbuf, "%d of Phase indices found for the phase name %s (->%d) \n"
             "Take the first one and continue (Y) or cancel (N)?", ii, name, LNmode );
    if( !vfQuestion(window(), "W21MSPrep: ", /*(const char *)*/pbuf.p ))
        Error( "E21MSPrep: ", "Preprocessing cancelled by the user..." );

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
    vstr pbuf(164);
    sprintf( pbuf, "%d of PCO indices found for the PCO name %s (->%d) \n"
             "Take the first one and continue (Y) or cancel (N)?", ii, name, LNmode );
    if( !vfQuestion(window(), "W08MSPrep: ", /*(const char *)*/pbuf.p ))
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
char* MSDELIM = " +-*/^:[]();=$&|!<>?#\n\t";
//
void TProfil::ET_translate( int nOet, int nOpex, int JB, int JE, int jb, int je,
        tget_ndx *get_ndx )
{
    size_t eLen, ls, lb;
    int i, ii, pj, LNplace=1, Xplace=0, nO=0;
    char cstate, cc, *etext, *pexpr, *ecur, *cur, *next, *end,
      *prev, *last, iCode/*=A_NOx*/, odlab[MAXKEYWD+2]; // added KD 30.03.01
    vstr name(64), nbuf(64);

    etext = (char *)aObj[ nOet ].GetPtr();
    if( etext )
        eLen = aObj[ nOet ].GetM();
    else eLen =0;
    pexpr = (char *)aObj[ nOpex ].GetPtr();

    ls = strlen( pexpr );
    if( !ls )
      return;
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
                gstring err = "Identifier (variable) ";
                err += odlab;
                err += " is not known.";
                Error( "E00MSPrep: ", err.c_str() );
            }
            nO = ii;
            iCode = aObj[ii].GetIndexationCode();
            // Getting new indexation code  KD 30.03.01
                switch( iCode )
            {  /* analyze indexation code */
               default:
                  Error( "E01MSPrep: Unknown indexation base for data object", odlab );
//             case A_reset:
               case A_icx:
               case A_dcx:
               case A_dcsx:
               case A_phx:
                    LNplace = 1; /* search index in MULTI */
//                  cstate = iCode;
                    break;
               case A_ICx:
               case A_ACx:
               case A_DCx:
               case A_DCSx:
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
            cur++; /* change state of scanner */
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
            case A_DCSx:
                i = find_dcnum( name, JB, JE, LNplace );
                i -=  mup->Ls - mup->Lads ;
                break;
            case A_dcx:
                i = find_dcnum( name, jb, je, LNplace );
                break;
            case A_dcsx:
                i = find_dcnum( name, jb, je, LNplace );
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
            if( !get_ndx )
              pj=i;
            else
              pj = get_ndx( i, nO, Xplace );
            switch( cstate )
            { /*change name in the text */
            case A_icx:
            case A_ICx: /* index */
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
            strcpy( ecur, nbuf );
            ecur += lb;
            continue;
        }
        if( cc > 0 && cc < 38 ) /* reading name to pre-process */
        { next++;
            cc++;
            continue;
        }
        Error( "E03MSPrep: too long name for index substitution", cur );
    } /* end while */
    *ecur = 0;
}


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
    if( pmp->pBAL < 2 || pmp->pTPD < 2)
        MultiRemake( key );

    // unpackSysEq record

    if( pmp->pESU /*== 1*/ && pmp->pNP )     // problematic statement !!!!!!!!!
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
         Set_DC_limits(  DC_LIM_INIT );

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

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Calculation of full structure MULTI by expanding
// SysEq main vectors read from the database
//
void TProfil::EqstatExpand( const char *key )
{
    int i, j, k, jb, je/*=0*/, jpb, jpe=0, jdb, jde=0;
    double FitVar3;

//    if( !pmp->NR )       Sveta 30/08/01
        pmp->NR = pmp->N;

    /* Load thermodynamic data for DC, if necessary */
    if( pmp->pTPD < 2 )
    {
        CompG0Load();
        memcpy( pmp->stkey, key, EQ_RKLEN );
        pmp->stkey[EQ_RKLEN]='\0';
    }
    /* Load activity coeffs for phases-solutions */
    if( pmp->FIs )
    {
        for( j=0; j< pmp->Ls; j++ )
        {
            pmp->lnGmo[j] = pmp->lnGam[j];
            if( fabs( pmp->lnGam[j] ) <= 84. )
                pmp->Gamma[j] = exp( pmp->lnGam[j] );
            else pmp->Gamma[j] = 1;
        }
    }
    /* recalc kinetic restrictions for DC */
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
    /* set IPM weight multipliers for DC*/
    for(j=0;j<pmp->L;j++)
    {
        switch( pmp->RLC[j] )
        {
        case NO_LIM:
        case LOWER_LIM:
            pmp->W[j] = pmp->Y[j]-pmp->DLL[j];
            break;
        case UPPER_LIM:
            pmp->W[j] = pmp->DUL[j]-pmp->Y[j];
            break;
        case BOTH_LIM:
            pmp->W[j]= ((pmp->Y[j]-pmp->DLL[j])<(pmp->DUL[j]-pmp->Y[j]))?
                       (pmp->Y[j]-pmp->DLL[j]): (pmp->DUL[j]-pmp->Y[j]);
            break;
        default: /* greatest error */ ; //break;
            Error( "Internal error", "Wrong IPM weight factor type!" );
        }
        if( pmp->W[j] < 0. ) pmp->W[j]=0.;    /* !! know-how of Mr.Chudnenko !! */
    }
    /* Calculate elemental chemical potentials in J/mole */
    for( i=0; i<pmp->N; i++ )
        pmp->U_r[i] = pmp->U[i]*pmp->RT;

    ConCalc( pmp->X, pmp->XF, pmp->XFA);
    /* Calculate mass-balance deviations (moles) */
    MassBalanceDeviations( pmp->N, pmp->L, pmp->A, pmp->X, pmp->B, pmp->C );
    /* Calc Eh, pe, pH,and other stuff */
    if( pmp->E && pmp->LO )
        IS_EtaCalc();

    FitVar3 = pmp->FitVar[3];   /* Switch off smoothing factor */
    pmp->FitVar[3] = 1.0;
    /* Scan phases to retrieve concentrations and activities */
    je = 0;
    for( k=0; k<pmp->FIs; k++ )
    {
        jb = je;
        je = jb+pmp->L1[k];
        jpb = jpe;
        jpe += pmp->LsMod[k];
        jdb = jde;
        jde += pmp->LsMdc[k]*pmp->L1[k];

        if( pmp->PHC[k] == PH_SORPTION )
        {
            if( pmp->E && pmp->LO )
                GouyChapman( jb, je, k );
            /* calculation of surface activity terms */
            SurfaceActivityCoeff( jb, je, jpb, jdb, k );
//            SurfaceActivityTerm(  jb, je, k );
        }
        for( j=jb; j<je; j++ )
        {
            /* calc Excess Gibbs energies F0 and values c_j */
            pmp->F0[j] = Ej_init_calc( 0, j, k );
            pmp->G[j] = pmp->G0[j] + pmp->F0[j];
            /* pmp->Gamma[j] = exp( pmp->lnGam[j] ); */
        }
    }
    pmp->FitVar[3]=FitVar3;
    pmp->GX_ = pmp->FX * pmp->RT;
    /* calc Prime DC chemical potentials defined via g0_j, Wx_j and lnGam_j */
    PrimeChemicalPotentials( pmp->F, pmp->X, pmp->XF, pmp->XFA );
    /*calc Karpov phase criteria */
    f_alpha();
    /*calc gas partial pressures  -- obsolete? */
    GasParcP();
    pmp->pFAG = 2;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Load Thermodynamic Data from MTPARM to MULTI
void TProfil::CompG0Load()
{
    int j, jj, k, jb, je=0;
    float Gg = 0., Vv = 0.;

    /* pTPD state of reload t/d data 0-all, 1 G0, 2 do not*/
    if( pmp->pTPD < 1 )
    {
        pmp->T = pmp->Tc = tpp->T + C_to_K;
        pmp->TC = pmp->TCc = tpp->T;
        if( tpp->P > 1e-9 )
            pmp->P = pmp->Pc = tpp->P;
        else pmp->P = pmp->Pc = 1e-9;
pmp->FitVar[0] = pa.aqPar[0]; // Added 07.06.05 for T,P dependent b_gamma   KD 
        pmp->denW = tpp->RoW;
        pmp->denWg = tpp->RoV;
        pmp->epsW = tpp->EpsW;
        pmp->epsWg = tpp->EpsV;
        pmp->RT = tpp->RT; // R_CONSTANT * pm->Tc
        pmp->FRT = F_CONSTANT/pmp->RT;
        pmp->lnP = log( pmp->P );
    }
    if( pmp->pTPD <= 1 )
    {
        for( k=0; k<pmp->FI; k++ )
        {
            jb = je;
            je += pmp->L1[k];
            /*load t/d data from DC */
            for( j=jb; j<je; j++ )
            {
                jj = pmp->muj[j];
                if( syp->Guns )
                    Gg = syp->Guns[jj];
                pmp->G0[j] = Cj_init_calc( tpp->G[jj]+Gg, j, k );
            }
        }
    }
    if( !pmp->pTPD )
    {
        for( j=0; j<pmp->L; j++ )
        {
            jj = pmp->muj[j];

            if( tpp->PtvVm == S_ON )
                switch( pmp->PV )
                { /* make mol volumes of components */
                case VOL_CONSTR:
                    if( syp->Vuns )
                       Vv = syp->Vuns[jj];
                    pmp->A[j*pmp->N] = tpp->Vm[jj]+Vv;
                case VOL_CALC:
                case VOL_UNDEF:
                    if( syp->Vuns )
                       Vv = syp->Vuns[jj];
                    pmp->Vol[j] = (tpp->Vm[jj]+Vv ) * 10.;  /* ?���? */
                    break;
                }
            else pmp->Vol[j] = 0.0;

            /* load ather t/d parametres - do it! */
        }
    }
    pmp->pTPD = 2;
}


//--------------------- End of muload.cpp ---------------------------


