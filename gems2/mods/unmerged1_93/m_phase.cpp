//-------------------------------------------------------------------
// Id: gems/mods/m_phase.cpp  version 2.0.0   2001
//
// Implementation of TPhase class, config and calculation functions
//
// Rewritten from C to C++ by S.Dmytriyeva  970207 modified 010904
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
const char *GEMS_PH_HTML = "gm_phase";

#include <math.h>

#include "v_object.h"
#include "visor.h"
#include "service.h"
#include "m_dcomp.h"
#include "m_reacdc.h"
#include "m_phase.h"
#include "m_param.h"

TPhase* TPhase::pm;

TPhase::TPhase( int nrt ):
        TCModule( nrt )
{
    nQ = 1;
    aFldKeysHelp.Add(
        "l<4 Code of aggregate state of a phase { a g p m l x d h }_");
    aFldKeysHelp.Add(
        "l<8 Symbol of phase definition (letters, digits, _)");
    aFldKeysHelp.Add(
        "l<16 Name of phase definition_");
    aFldKeysHelp.Add(
        "l<4 Phase class { c l h u aq gm ss xs xg xa pl ls hm mm }_");
    aFldKeysHelp.Add(
        "l<16 Record key comment to phase definition_");
    php=&ph[0];
    set_def();
    start_title = " Definition of thermodynamic phase ";
}

// link values to objects
void TPhase::ods_link( int q)
{
    ErrorIf( q > nQ, GetName(), "Illegal link q>nQ.");
    //aObj[ o_phpst].SetPtr(  ph[q].pst );
    //aObj[ o_phsymb].SetPtr( ph[q].symb );
    //aObj[ o_phnam].SetPtr(  ph[q].nam );
    //aObj[ o_phcls].SetPtr(  ph[q].cls );
    //aObj[ o_phgrp].SetPtr(  ph[q].grp );
    aObj[ o_phsolt].SetPtr( ph[q].sol_t );
    aObj[ o_phps].SetPtr(  &ph[q].PphC );  /*a 6*/
    aObj[ o_phndc].SetPtr( &ph[q].nDC );   /*i 2*/
    aObj[ o_phpncp].SetPtr(&ph[q].ncpN );  /*i 2*/
    aObj[ o_phnsc].SetPtr( &ph[q].nscN );  /*i 2*/
    aObj[ o_phnsit].SetPtr(&ph[q].NsiT );  /*i 2*/
    /* Record 22 fields */
    aObj[ o_phstr].SetPtr(  ph[q].sol_t );
    aObj[ o_phname].SetPtr( ph[q].name );
    aObj[ o_phnotes].SetPtr(ph[q].notes );
    aObj[ o_phdim].SetPtr( &ph[q].nDC );     /*i 8*/
    aObj[ o_phdisep].SetPtr(&ph[q].Asur );    /*f 8*/
    // NsiT
    aObj[ o_phscmc].SetPtr(  ph[q].SCMC );
    aObj[ o_phscmc].SetDim( ph[q].NsiT, 1 );
    aObj[ o_phxfiec].SetPtr( ph[q].XfIEC);
    aObj[ o_phxfiec].SetDim( ph[q].NsiT, 1);
    aObj[ o_phmsdt].SetPtr(  ph[q].MSDT );
    aObj[ o_phmsdt].SetDim( ph[q].NsiT, 2 );
    aObj[ o_phcapt].SetPtr(  ph[q].CapT );
    aObj[ o_phcapt].SetDim( ph[q].NsiT, 2 );
    aObj[ o_phfsit].SetPtr(  ph[q].FsiT );
    aObj[ o_phfsit].SetDim( ph[q].NsiT, 1 );
    //nDC
    aObj[ o_phsatc].SetPtr(  ph[q].SATC );
    aObj[ o_phsatc].SetDim( ph[q].nDC,  2 );
    aObj[ o_phmasdj].SetPtr( ph[q].MaSdj);
    aObj[ o_phmasdj].SetDim( ph[q].nDC, 1 );
    aObj[ o_phpxres].SetPtr( ph[q].PXres);
    aObj[ o_phpxres].SetDim( ph[q].nDC, 1 );
    aObj[ o_phsm].SetPtr(   ph[q].SM[0] );
    aObj[ o_phsm].SetDim( ph[q].nDC, 1 );
    aObj[ o_phdcc].SetPtr(  ph[q].DCC );
    aObj[ o_phdcc].SetDim( ph[q].nDC, 1 );
    aObj[ o_phdcs].SetPtr(  ph[q].DCS );
    aObj[ o_phdcs].SetDim( ph[q].nDC, 1 );

    aObj[ o_phpnc].SetPtr(  ph[q].pnc );
    aObj[ o_phpnc].SetDim( ph[q].ncpN, ph[q].ncpM );
    aObj[ o_phscoef].SetPtr( ph[q].scoef );
    aObj[ o_phscoef].SetDim( ph[q].nDC, ph[q].nscN*ph[q].nscM );
    // ??????
    aObj[ o_phpeq].SetPtr(  ph[q].pEq );
    //   aObj[ o_phpeq].SetDim( 1, strlen(ph[q].pEq));
    aObj[ o_phdeq].SetPtr(  ph[q].dEq );
    //   aObj[ o_phdeq].SetDim( 1, strlen(ph[q].dEq));
    // Nsd
    aObj[ o_phsdref].SetPtr( ph[q].sdref);
    aObj[ o_phsdref].SetDim( ph[q].Nsd, 1 );
    aObj[ o_phsdval].SetPtr( ph[q].sdval);
    aObj[ o_phsdval].SetDim( ph[q].Nsd, 1 );
    /// ??????
    aObj[o_phtprn].SetPtr( ph[q].tprn );
    //    aObj[ o_phtprn].SetDim( 1,strlen(ph[q].tprn));
    php=&ph[q];
}

// set dynamic Objects ptr to values
void TPhase::dyn_set(int q)
{
    ErrorIf( php!=&ph[q], GetName(),
             "Illegal access to ph in dyn_set.");
    memcpy( php->pst, rt[nRT].UnpackKey(), PH_RKLEN );
    ph[q].SCMC =  (char *)aObj[ o_phscmc ].GetPtr();
    ph[q].FsiT =  (float *)aObj[ o_phfsit ].GetPtr();
    ph[q].XfIEC = (float *)aObj[ o_phxfiec ].GetPtr();
    ph[q].MSDT  = (float (*)[2])aObj[ o_phmsdt ].GetPtr();
    ph[q].CapT  = (float (*)[2])aObj[ o_phcapt ].GetPtr();
    ph[q].SATC  = (char (*)[2])aObj[ o_phsatc ].GetPtr();
    ph[q].MaSdj = (float *)aObj[ o_phmasdj ].GetPtr();
    ph[q].PXres = (float *)aObj[ o_phpxres ].GetPtr();
    ph[q].pnc =   (float *)aObj[ o_phpnc ].GetPtr();
    ph[q].scoef = (float *)aObj[ o_phscoef ].GetPtr();
    ph[q].SM =    (char (*)[DC_RKLEN])aObj[ o_phsm ].GetPtr();
    ph[q].DCC =   (char *)aObj[ o_phdcc ].GetPtr();
    ph[q].DCS =   (char *)aObj[ o_phdcs ].GetPtr();
    ph[q].pEq =   (char *)aObj[ o_phpeq ].GetPtr();
    ph[q].dEq =   (char *)aObj[ o_phdeq ].GetPtr();
    ph[q].sdref = (char (*)[V_SD_RKLEN])aObj[ o_phsdref ].GetPtr();
    ph[q].sdval = (char (*)[V_SD_VALEN])aObj[ o_phsdval ].GetPtr();
    ph[q].tprn = (char *)aObj[ o_phtprn ].GetPtr();
}

// free dynamic memory in objects and values
void TPhase::dyn_kill(int q)
{
    ErrorIf( php!=&ph[q], GetName(), "Illegal access to ph in dyn_kill.");
    ph[q].SCMC =  (char *)aObj[ o_phscmc ].Free();
    ph[q].FsiT =  (float *)aObj[ o_phfsit ].Free();
    ph[q].XfIEC = (float *)aObj[ o_phxfiec ].Free();
    ph[q].MSDT =  (float (*)[2])aObj[ o_phmsdt ].Free();
    ph[q].CapT =  (float (*)[2])aObj[ o_phcapt ].Free();
    ph[q].SATC =  (char (*)[2])aObj[ o_phsatc ].Free();
    ph[q].MaSdj = (float *)aObj[ o_phmasdj ].Free();
    ph[q].PXres = (float *)aObj[ o_phpxres ].Free();
    ph[q].pnc =   (float *)aObj[ o_phpnc ].Free();
    ph[q].scoef = (float *)aObj[ o_phscoef ].Free();
    ph[q].SM =    (char (*)[DC_RKLEN])aObj[ o_phsm ].Free();
    ph[q].DCC =   (char *)aObj[ o_phdcc ].Free();
    ph[q].DCS =   (char *)aObj[ o_phdcs ].Free();
    ph[q].pEq =   (char *)aObj[ o_phpeq ].Free();
    ph[q].dEq =   (char *)aObj[ o_phdeq ].Free();
    ph[q].sdref = (char (*)[V_SD_RKLEN])aObj[ o_phsdref ].Free();
    ph[q].sdval = (char (*)[V_SD_VALEN])aObj[ o_phsdval ].Free();
    ph[q].tprn = (char *)aObj[ o_phtprn ].Free();
}

// realloc dynamic memory
void TPhase::dyn_new(int q)
{
    ErrorIf( php!=&ph[q], GetName(), "Illegal access to ph in dyn_new.");
    ErrorIf( ph[q].nDC <= 0, GetName(), "N of DC in phase definition <= 0.");
    ph[q].SM = (char (*)[DC_RKLEN])aObj[ o_phsm ].Alloc( ph[q].nDC, 1, DC_RKLEN );
    ph[q].DCS = (char *)aObj[ o_phdcs ].Alloc( ph[q].nDC, 1, A_ );
    ph[q].DCC = (char *)aObj[ o_phdcc ].Alloc( ph[q].nDC, 1, A_ );

    if( ph[q].Psco == S_ON )
        ph[q].scoef = (float *)aObj[ o_phscoef].Alloc( ph[q].nDC,
                      ph[q].nscN*ph[q].nscM, F_ );
    else
        ph[q].scoef = (float *)aObj[ o_phscoef ].Free();

    if( ph[q].Ppnc == S_ON )
        ph[q].pnc = (float *)aObj[ o_phpnc ].Alloc( ph[q].ncpN, ph[q].ncpM, F_ );
    else
        ph[q].pnc =   (float *)aObj[ o_phpnc ].Free();

    if( ph[q].PFsiT == S_ON || ph[q].PFsiT == S_REM )
    {
        ph[q].SCMC =  (char *)aObj[ o_phscmc ].Alloc( ph[q].NsiT, 1, A_);
        ph[q].FsiT =  (float *)aObj[ o_phfsit ].Alloc( ph[q].NsiT, 1, F_);
        ph[q].XfIEC = (float *)aObj[ o_phxfiec ].Alloc( ph[q].NsiT, 1, F_);
        ph[q].MSDT =  (float (*)[2])aObj[ o_phmsdt ].Alloc( ph[q].NsiT, 2, F_);
        ph[q].CapT =  (float (*)[2])aObj[ o_phcapt ].Alloc( ph[q].NsiT, 2, F_);
        ph[q].SATC =  (char (*)[2])aObj[ o_phsatc ].Alloc( ph[q].nDC, 2, A_);
        ph[q].MaSdj = (float *)aObj[ o_phmasdj ].Alloc( ph[q].nDC, 1, F_);
        ph[q].PXres = (float *)aObj[ o_phpxres ].Alloc( ph[q].nDC, 1, F_);
    }
    else
    {
        ph[q].SCMC =  (char *)aObj[ o_phscmc ].Free();
        ph[q].FsiT =  (float *)aObj[ o_phfsit ].Free();
        ph[q].XfIEC = (float *)aObj[ o_phxfiec ].Free();
        ph[q].MSDT =  (float (*)[2])aObj[ o_phmsdt ].Free();
        ph[q].CapT =  (float (*)[2])aObj[ o_phcapt ].Free();
        ph[q].SATC =  (char (*)[2])aObj[ o_phsatc ].Free();
        ph[q].MaSdj = (float *)aObj[ o_phmasdj ].Free();
        ph[q].PXres = (float *)aObj[ o_phpxres ].Free();
    }
    if( ph[q].Nsd )
    {
        ph[q].sdref = (char (*)[V_SD_RKLEN])aObj[ o_phsdref ].Alloc(
                          ph[q].Nsd, 1, V_SD_RKLEN);
        ph[q].sdval = (char (*)[V_SD_VALEN])aObj[ o_phsdval ].Alloc(
                          ph[q].Nsd, 1, V_SD_VALEN);
    }
    else
    {
        ph[q].sdref = (char (*)[V_SD_RKLEN])aObj[ o_phsdref ].Free();
        ph[q].sdval = (char (*)[V_SD_VALEN])aObj[ o_phsdval ].Free();
    }
    if( ph[q].pEq && ph[q].PpEq == S_OFF )
        ph[q].pEq  =  (char *)aObj[ o_phpeq ].Free();
    else
        if( ph[q].PpEq != S_OFF && !ph[q].pEq )
        {
            ph[q].pEq  =  (char *)aObj[ o_phpeq ].Alloc( 1, MAXFORMULA, S_);
            *ph[q].pEq = '`';
        }
    if( ph[q].dEq && ph[q].PdEq == S_OFF )
        ph[q].dEq =   (char *)aObj[ o_phdeq ].Free();
    else
        if( ph[q].PdEq != S_OFF && !ph[q].dEq )
        {
            ph[q].dEq  =  (char *)aObj[ o_phdeq ].Alloc( 1, MAXFORMULA, S_);
            *ph[q].dEq = '`';
        }
    //  ph[q].tprn = (char *)aObj[ o_phtprn ].Free();
}

//set default information

void TPhase::set_def( int q)
{
    ErrorIf( php!=&ph[q], GetName(), "Illegal access to ph in set_def.");
    TProfil *aPa=(TProfil *)(&aMod[RT_PARAM]);
    memcpy( ph[q].sol_t, aPa->pa.PHsol_t, 6 );
    memcpy( &ph[q].PphC, aPa->pa.PHpvc, 6 );
    strcpy( ph[q].name, "`");
    strcpy( ph[q].notes, "`" );

    ph[q].Nsd = 1;
    ph[q].nDC = ph[q].ncpN = ph[q].ncpM =0;
    ph[q].NsiT = ph[q].nscN = ph[q].nscM = ph[q].NR1 = 0;
    ph[q].Asur =  FLOAT_EMPTY;
    ph[q].Sigma0 =  FLOAT_EMPTY;
    ph[q].SigmaG =  FLOAT_EMPTY;
    ph[q].R0p =  FLOAT_EMPTY;
    ph[q].h0p =  FLOAT_EMPTY;
    ph[q].Eps =  FLOAT_EMPTY;
    ph[q].Cond =  FLOAT_EMPTY;
    ph[q].Rsp1 =  FLOAT_EMPTY;
    ph[q].SCMC =  0;
    ph[q].FsiT =  0;
    ph[q].XfIEC = 0;
    ph[q].MSDT =  0;
    ph[q].CapT =  0;
    ph[q].SATC =  0;
    ph[q].MaSdj = 0;
    ph[q].PXres = 0;
    ph[q].pnc =   0;
    ph[q].scoef = 0;
    ph[q].SM =    0;
    ph[q].DCC =   0;
    ph[q].DCS =   0;
    ph[q].pEq =   0;
    ph[q].dEq =   0;
    ph[q].sdref = 0;
    ph[q].sdval = 0;
    ph[q].tprn = 0;
}

// Input nessasery data and links objects
void TPhase::RecInput( const char *key )
{
    TCModule::RecInput( key );
}

static int rkeycmp(const void *e1, const void *e2)
{
    int RCmp;
    RCmp = memcmp( e1, e2, DC_RKLEN );
    return RCmp;
}

//Rebild record structure before calc
int
TPhase::RecBuild( const char *key )
{
    int i, iic, j, Ndc, Nrc;
    vstr pkey(81);

    TCStringArray aDclist;
    TCIntArray anRDc;
    TCIntArray aMcv;
    TCStringArray aRclist;
    TCIntArray anRRc;
    TCIntArray aMrv;
    gstring str;
    TProfil *aPa=(TProfil *)(&aMod[RT_PARAM]);

AGAIN_SETUP:
    int ret = TCModule::RecBuild( key );
    if( ret == VF_CANCEL  &&!( !php->PphC || php->PphC == ' '))
        return ret;
    if( php->nscN < 0 || php->nscM < 0 || php->ncpN < 0 || php->ncpM < 0 ||
            php->ncpN*php->ncpM > MAXPNCOEF || php->Nsd < 0 || php->Nsd > 16 ||
            php->NsiT < 0 || php->NsiT > 6 )
    {
        if(vfQuestion(window(), GetName(),
                      "Invalid number of coeffs in the non-ideal solution model! Proceed?"))
            goto AGAIN_SETUP;
        else   Error( GetName(), "Invalid mode of calculation.");
    }
    SetString("PH_make   Remaking PHASE definition");
    pVisor->Update();
    //DCOMP keypart
    rt[RT_DCOMP].MakeKey( RT_PHASE, pkey, K_ACT, 0, K_ANY, K_ANY, K_ANY, K_END);
    if( pkey[1] != ':') pkey[1] = '*';
AGAINDC:
    str = TDComp::pm->GetKeyofRecord( pkey, "Set template", KEY_TEMP );
//  if(  str== "" )    Bugfix 19.12.00  DAK
//     goto AGAINDC;
    Ndc = rt[RT_DCOMP].GetKeyList( str.c_str(), aDclist, anRDc );
    if( Ndc<1 )
        if( vfQuestion(window(), GetName(),
                       "No DCOMP records or error at PDB access! Repeat?"))
            goto AGAINDC;
    //REACDC  keypart
    rt[RT_REACDC].MakeKey( RT_PHASE, pkey, K_ACT, 0, K_ANY, K_ANY, K_ANY, K_END );
    if( pkey[1] != ':') pkey[1] = '*';
AGAINRC:
    str = TReacDC::pm->GetKeyofRecord( pkey, "Set template", KEY_TEMP);
//    if(  str== "" )  bugfix  19.12.00  DAK
//       goto AGAINRC;
    Nrc = rt[RT_REACDC].GetKeyList( str.c_str(), aRclist, anRRc );
    if( Nrc<1 )
        if( vfQuestion(window(), GetName(),
                       "No REACDC records or error at PDB access! Repeat?"))
            goto AGAINRC;
        else ErrorIf( Ndc<1, GetName(), "No keys DCOMP and REACDC.");

    if( php->nDC && php->SM )
    {
        /* Build old selections DCOMP and REACDC */
        aMcv.Clear();
        aMrv.Clear();
        for( i=0; i<php->nDC; i++ )
        {
            if( php->DCS[i] == SRC_DCOMP )
            {
                for( j=0; j< Ndc; j++ )
                {
                    if( memcmp( php->SM[i], aDclist[j].c_str(), DC_RKLEN-MAXSYMB ))
                        continue;
                    aMcv.Add(j);
                    break;
                }
            }
            else
                if( php->DCS[i] == SRC_REACDC )
                {
                    for( j=0; j< Nrc; j++ )
                    {
                        if( memcmp( php->SM[i], aRclist[j].c_str(), DC_RKLEN-MAXSYMB ))
                            continue;
                        aMrv.Add(j);
                        break;
                    }
                }
        }
    }
    if( Nrc > 0 )
    {
        aMrv = vfMultiChoiceSet(window(), aRclist,
                                "Please, mark ReacDC keys to be included into Phase", aMrv );
        if( aMrv.GetCount() < 1 )
            switch ( vfQuestion3(window(), GetName(), "Number of selected ReacDC keys < 1.\n"
                                 " Mark again, proceed without ReacDC or Cancel?", "&Repeat", "&Proceed"))
            {
            case VF3_1:
                goto AGAINRC;
                ;
            case VF3_2:
                break;
            case VF3_3:
                Error( GetName(),"No ReacDC records selected into Phase...");
            }
    }
    if( Ndc > 0 )
    {
        aMcv = vfMultiChoiceSet(window(), aDclist,
                                "Please, mark DComp keys to be included into Phase", aMcv );
        if( aMcv.GetCount() < 1 )
            switch ( vfQuestion3(window(), GetName(),"Number of selected DComp record keys < 1.\n"
                                 " Mark again, proceed without DComp or Cancel?", "&Repeat", "&Proceed"))
            {
            case VF3_1:
                goto AGAINDC;
                ;
            case VF3_2:
                break;
            case VF3_3:
                Error( GetName(),"No DComp records selected into Phase...");
            }
    }

    /* Sort list of selections */
    /*   if( aMcv.GetCount() >= 2 )
          aMcv.qsort(); // int array
       if( aMrv.GetCount() >= 2 )
          aMrv.qsort(); // int array
    */
    php->nDC = aMcv.GetCount() + aMrv.GetCount();
    php->NR1 = aMrv.GetCount();
    iic = aMcv.GetCount();

    /* insert coeff of model of solid and other data */
    if( php->nscN * php->nscM ) php->Psco = S_ON;
    else  php->Psco = S_OFF;
    if( php->ncpN * php->ncpM ) php->Ppnc = S_ON;
    else php->Ppnc = S_OFF;
    if( !php->NsiT ) /* && php->PFsiT != S_REM ) php->PFsiT = S_ON;
               else */  php->PFsiT = S_OFF;
    if( php->sol_t[DCOMP_DEP] == SM_UNDEF ) php->PdEq = S_OFF;
    else php->PdEq = S_ON;
    if( php->sol_t[SPHAS_DEP] == SM_UNDEF ) php->PpEq = S_OFF;
    else php->PpEq = S_ON;

    dyn_new(0);

    /* Get list of component : add aMcv and aMrv */
    for( i=0; i<php->nDC; i++ )
    {
        if( i< aMcv.GetCount() )
        {
            memcpy( php->SM[i], aDclist[aMcv[i]].c_str(), DC_RKLEN );
            php->SM[i][DC_RKLEN-1] = SRC_DCOMP;
        }
        else
        {
            memcpy( php->SM[i], aRclist[aMrv[i-iic]].c_str(), DC_RKLEN );
            php->SM[i][DC_RKLEN-1] = SRC_REACDC;
        }
    }
    /* Sort list of component */
    if( php->nDC > 2 )
        qsort( php->SM[0], (size_t)php->nDC, DC_RKLEN, rkeycmp );

    for( i=0; i<php->nDC; i++ )
    {
        php->DCS[i] = php->SM[i][DC_RKLEN-1]; /* cod istochnic  */
        php->SM[i][DC_RKLEN-1] = ' ';
        if( !php->DCC[i] )
            php->DCC[i] = '`';
    }
    switch( php->pst[0] )
    { /* Set type of phase */
    case CP_AQU:
        php->PphC = PH_AQUEL;
        break;
    case CP_GASI:
        php->PphC = PH_PLASMA;
        break;
    case CP_GAS:
        php->PphC = PH_GASMIX;
        break;
        //
    case CP_LIQID:
        php->PphC = PH_SOLUTION;
        break;
    case CP_SOLID: /* if( php->nDC == 1 ) Fixed 23.10.99 by DAK */
        php->PphC = PH_SINCOND;
        break;
        //
    case CP_HCARB:
        php->PphC = PH_HCARBL;
        break;
    case CP_SSPC:
    case CP_SORB:
        php->PphC = PH_SORPTION;
        break;
    default:
        php->PphC = '?';
    }

    if( php->NsiT > 0 && php->PFsiT == S_REM )
    {  /* Setup of default values */
        for( i=0; i<php->NsiT; i++ )
        { /* if( !php->SCMC[i] || php->SCMC[i]==A_NUL ) */
            php->SCMC[i] = SC_TLM;  /* Default! fix by DAK 23.10.99  */
            if( !php->MSDT[i][0] )
                php->MSDT[i][0] = aPa->pa.p.DNS;
            if( !php->MSDT[i][1] )
                php->MSDT[i][1] = 0.6022; /* 1/nm2; = 1 mkmol/m2 */
            if( !php->CapT[i][0] )
                php->CapT[i][0] = 1.0; /* A plane */
            if( !php->CapT[i][1] )
                php->CapT[i][1] = 0.2; /* B plane */
        }
        for( i=0; i<php->nDC; i++ )
        {
            /* if( !php->SATC[i][0] || php->SATC[i][0] ==A_NUL ) */
            php->SATC[i][0] = SAT_COMP;
            /* if( !php->SATC[i][1] || php->SATC[i][1] ==A_NUL ) */
            php->SATC[i][1] = CCA_VOL;  /* Default! */
            /* ..................... */
        }
        php->PFsiT = S_ON;
    }
    SetString("PH_make   Remaking PHASE definition OK");
    pVisor->Update();
    return ret;
}

#define s(i,j) php->scoef[(j)+(i)*nsc]
//Recalc record structure
void
TPhase::RecCalc( const char *key )
{
    bool getDCC;
    getDCC = vfQuestion(window(), GetName(),"Remake DC classes list?");
    CalcPhaseRecord( getDCC );
    SetString("PH_solm   PHASE-solution model OK");
    TCModule::RecCalc(key);
}

//Recalc record structure
void
TPhase::CalcPhaseRecord(  bool getDCC  )
{
    int  i, iic, pa0=0, Kielland, nsc;
    vstr dcn(MAXRKEYLEN);
    char Ctype;
    float a0, bp, Z;
    time_t crt;


    TProfil *aPa=(TProfil *)(&aMod[RT_PARAM]);
    TDComp* aDC=(TDComp *)(&aMod[RT_DCOMP]);
    TReacDC* aRDC=(TReacDC *)(&aMod[RT_REACDC]);
    aDC->ods_link(0);
    aRDC->ods_link(0);

    /* get effective ion radii of aqueous */
    if( (php->PphC == PH_AQUEL /* || php->PphC == PH_SORPTION ||
                  php->PphC == PH_POLYEL */ ) && php->scoef )
    {
        nsc = php->nscN * php->nscM;
        if( pVisor->ProfileMode == true || vfQuestion(window(), GetName(),
                "Effective radii of aqueous species: Collect from DCOMP/REACDC records?"))
        {
            pa0 = 1;
            Kielland = 0;
        }
        else
        {
            switch ( vfQuestion3(window(), GetName(), "Set default values automatically:\n"
                                 "from (Kielland 1936), from (Helgeson ea 1981) or \n"
                                 "Not collect and not set automatically? ",
                                 "&Kielland", "&Helgeson" ) )
            {
            case VF3_1:
                Kielland = 1;
                pa0 = 0;
                break;
            case VF3_2:
                Kielland = 1;
                pa0 = 1;
                break;  /* A Pitzer ? */
            case VF3_3:
                pa0 = 0;
                Kielland = 0;
                break;
            }
        }
    }
    memset( dcn, 0, MAXRKEYLEN );
    for( i=0; i<php->nDC; i++ )
    {  /*Get key */
        memcpy( dcn, php->SM[i], DC_RKLEN );
        dcn[DC_RKLEN]=0;
        Ctype = A_NUL;
        /* Read record */
        if( php->DCS[i] == SRC_DCOMP )
        {
            aDC->TryRecInp( dcn, crt, 0 );
            a0 = aDC->dcp->Der;
            bp = aDC->dcp->DerB;
            Z = aDC->dcp->Zz;
            Ctype = aDC->dcp->PdcC;

        }
        else
            if( php->DCS[i] == SRC_REACDC )
            {
                aRDC->TryRecInp( dcn, crt, 0 );
                a0 = aRDC->rcp->Der;
                bp = aRDC->rcp->DerB;
                Z = aRDC->rcp->Zz;
                Ctype = aRDC->rcp->PreC;
            }
        /* test model and types of components */
        if( getDCC==true || php->DCC[i] == A_NUL || php->DCC[i] == '`')
            php->DCC[i] = Ctype;

        if( php->PphC == PH_AQUEL && php->scoef )
        {
            Z = fabs(Z);
            if( pa0 && !Kielland )
            {
                s(i,0) = a0;
                if( nsc > 1 ) s(i,1) = bp;
            }
            if( Kielland && !pa0 )
            {
                if( Z < 0.01 ) s(i,0) = 0;
                if( fabs(Z-1.0) < 0.01 ) s(i,0) = 3.5;
                if( fabs(Z-2.0) < 0.01 ) s(i,0) = 5.0;
                if( Z > 2.99 ) s(i,0) = 9.0; /* Make a table ! */
            }
            if( Kielland && pa0 )  /* Helgeson */
            { if( Z < 0.01 ) s(i,0) = 0;
                if( Z > 0.99 ) s(i,0) = 3.84;
                /* do it by HKF ! */
            }
        }
        if(( php->PphC == PH_SORPTION || php->PphC == PH_POLYEL) && php->scoef )
        {
            Z = fabs(Z);
            if( pa0 && !Kielland )
            {
                s(i,0) = a0;
                if( nsc > 1 ) s(i,1) = bp;
            }
            if( Kielland && !pa0 )
            {
                if( Z < 0.01 ) s(i,0) = aPa->pa.p.DNS;
                if( fabs(Z-1.0) < 0.01 ) s(i,0) = aPa->pa.p.DNS/2.;
                if( fabs(Z-2.0) < 0.01 ) s(i,0) = aPa->pa.p.DNS/3.;
                if( Z > 2.99 ) s(i,0) = 3.0; /* Make a table! */
            }
            if( Kielland && pa0 )  /* Default */
            { if( Z < 0.01 ) s(i,0) = aPa->pa.p.DNS;
                if( Z > 0.99 ) s(i,0) = 2.31;
            }
        }

    } /* i */
NEXT:  /* define more precisely code of phase  */
    iic = 0;
    if( php->Asur > 1. )
        if( php->nDC == 1 )
        {
            php->PphC = PH_SINDIS;   /* iic++; */
        }
        else if( php->NsiT >= 1. )
        {
            php->PphC = PH_SORPTION; /* iic++; */
        }
    if( iic ) goto NEXT;
}

// to Profile new
void TPhase::newAqGasPhase( const char *key, int file,
                            bool useLst, TCStringArray lst )
{
    TProfil *aPa=(TProfil *)(&aMod[RT_PARAM]);
    char *part;

    if( key[0] == 'a' )
        // set flags  and sizes
    {
        part = "a:*:*:*:";
        memcpy( php->sol_t, "3NNSNN", 6 );
        memcpy( &php->PphC, "a++---", 6 );
        php->ncpN = 1;
        php->ncpM =8;
        php->nscN = php->nscM = 1;

    }
    else
    {
        part = "g:*:*:*:";
        memcpy( php->sol_t, "INNINN", 6 );
        memcpy( &php->PphC, "g-----", 6 );
        php->ncpN = php->ncpM =0;
        php->nscN = php->nscM =0;
    }
    strncpy( php->name, key, MAXFORMULA-1);
    php->name[MAXFORMULA-1] = '\0';
    strcpy( php->notes, "`" );

    php->Nsd = 0;
    php->nDC = 0;
    php->NsiT = php->NR1 = 0;
    php->Asur =   php->Sigma0 =  php->SigmaG =   php->R0p =
                                     php->h0p =  php->Eps =  php->Cond =  php->Rsp1 =  0.;

    //Get lists
    TCStringArray aDclist;
    TCIntArray anRDc;
    TCStringArray aRclist;
    TCIntArray anRRc;
    int Ndc, Nrc, iic, i;

    if( useLst == false )
    {

        //DCOMP key list
        Ndc = rt[RT_DCOMP].GetKeyList( part, aDclist, anRDc );
        uint ii=0;
        while( ii< aDclist.GetCount() )
        {
            if( !memcmp( aDclist[ii].c_str()+MAXSYMB+MAXDRGROUP, "E-", 2 ) ||
                    !memcmp( aDclist[ii].c_str()+MAXSYMB+MAXDRGROUP, "e-", 2 ) )
            {
                aDclist.Remove(ii);
                anRDc.Remove(ii);
                Ndc--;
            }
            else
                if( ii>0 &&
                        !memcmp( aDclist[ii].c_str(), aDclist[ii-1].c_str(), DC_RKLEN-MAXSYMB ) )
                {
                    aDclist.Remove(ii);
                    anRDc.Remove(ii);
                    Ndc--;
                }
                else ii++;
        }
        //REACDC  key list
        Nrc = rt[RT_REACDC].GetKeyList( part, aRclist, anRRc );
        ii=0;
        while( ii< aRclist.GetCount() )
        {
            if( ii>0 &&
                    !memcmp( aRclist[ii].c_str(), aRclist[ii-1].c_str(), DC_RKLEN-MAXSYMB ) )
            {
                aRclist.Remove(ii);
                anRRc.Remove(ii);
                Nrc--;
            }
            else ii++;
        }


        ErrorIf( Nrc<1&&Ndc<1,  key, " No keys DCOMP and REACDC.");
        php->nDC = Ndc + Nrc;
        php->NR1 = aRclist.GetCount();
        iic = aDclist.GetCount();
    }
    else
        php->nDC = lst.GetCount(); // php->NR1 ?

    dyn_new(0);

    if( useLst == false )
    {   /* Get list of component : add aMcv and aMrv */
        for( i=0; i<php->nDC; i++ )
        {
            if( i<aDclist.GetCount() )
            {
                memcpy( php->SM[i], aDclist[i].c_str(), DC_RKLEN );
                php->SM[i][DC_RKLEN-1] = SRC_DCOMP;
            }
            else
            {
                memcpy( php->SM[i], aRclist[i-iic].c_str(), DC_RKLEN );
                php->SM[i][DC_RKLEN-1] = SRC_REACDC;
            }
        }
        /* Sort list of component */
        if( php->nDC > 2 )
            qsort( php->SM[0], (size_t)php->nDC, DC_RKLEN, rkeycmp );
    }
    else
        for( i=0, php->NR1=0; i<php->nDC; i++ )
        {
            if( lst[i].c_str()[DC_RKLEN-1] == SRC_REACDC)
                php->NR1++;
            memcpy( php->SM[i], lst[i].c_str(), DC_RKLEN );
        }

    for( i=0; i<php->nDC; i++ )
    {
        php->DCS[i] = php->SM[i][DC_RKLEN-1]; /* cod istochnic  */
        php->SM[i][DC_RKLEN-1] = ' ';
        php->DCC[i] = '`';
    }
    if( php->NsiT > 0 && php->PFsiT == S_REM )
    {  /* Setup of default values */
        for( i=0; i<php->NsiT; i++ )
        {
            if( !php->SCMC[i] || php->SCMC[i]==A_NUL )
                php->SCMC[i] = SC_CCM;  /* Default! */
            if( !php->MSDT[i][0] )
                php->MSDT[i][0] = aPa->pa.p.DNS;
            if( !php->MSDT[i][1] )
                php->MSDT[i][1] = 0.6022; /* 1/nm2; = 1 mkmol/m2 */
            if( !php->CapT[i][0] )
                php->CapT[i][0] = 1.0; /* A plane */
            if( !php->CapT[i][1] )
                php->CapT[i][1] = 0.2; /* B plane */
        }

        for( i=0; i<php->nDC; i++ )
        {
            if( !php->SATC[i][0] || php->SATC[i][0] ==A_NUL )
                php->SATC[i][0] = SAT_INDEF;
            if( !php->SATC[i][1] || php->SATC[i][1] ==A_NUL )
                php->SATC[i][1] = CCA_VOL;  /* Default! */
            /* ..................... */
        }
        php->PFsiT = S_ON;
    }

    if( key[0] == 'a' )
        if( php->pnc )
            php->pnc[5] = 0.064;
    CalcPhaseRecord( true );
    int  Rnum = db->Find( key );
    if( Rnum<0 )
        db->AddRecordToFile( key, file );
    else
        db->Rep( Rnum );
    fEdit = false;
}

void
TPhase::CmHelp()
{
    pVisor->OpenHelp( GEMS_PH_HTML );  //  05.01.01
}

#include "m_sdata.h"
void
TPhase::RecordPrint( const char *key )
{
// select  SDref key
 gstring sd_key;
  if( key )
  sd_key=key;
 else
 {
   if( php->sdref )
     for( int ii=0; ii<php->Nsd; ii++ )
       if( gstring( php->sdref[ii], 0, V_SD_RKLEN ).find( "pscript" )
           != gstring::npos)
       { sd_key = gstring( php->sdref[ii]);
         break;
        }
    if( sd_key.empty() )
       sd_key = "pscript:0000:phase:";
  }
 // read sdref record with format prn
 TSData::pm->RecInput( sd_key.c_str() );
 char * text_fmt = TSData::pm->getAbstr();
 if( !text_fmt )
   Error( sd_key.c_str(), "No format text in this record.");
 PrintSDref( sd_key.c_str(), text_fmt );
}


void TPhase::CopyRecords( const char * prfName, TCStringArray& rds,
            TCStringArray& names, TCStringArray& aPHnoused,
            bool aAqueous, bool aGaseous, bool aSorption)
{
    TCIntArray anR;
    TCStringArray aPHkey;
    aPHnoused.Clear();

    // open selected kernel files
    db->OpenOnlyFromList(names);

    // added to profile file phase.copy.prfname
    /*gstring Path = pVisor->userProfDir();
    Path += prfName;
    Path += "/";
    Path += db->GetKeywd();
    Path += ".";
    Path += "copy";
    Path += ".";
    Path += prfName;
    Path += ".";
    Path += PDB_EXT;
    TDBFile *aFl= new TDBFile( Path );
    int fnum_ = db->AddFileToList( aFl );
    */
    int fnum_ = db->GetOpenFileNum( prfName );

    // get list of records
    db->GetKeyList( "*:*:*:*:*:", aPHkey, anR );

    //  test&copy  selected records
    // ( add to last key field first symbol from prfname )
    int i, cnt;
    bool nRec;
    for(uint ii=0; ii<aPHkey.GetCount(); ii++ )
    {
     if( !aAqueous && ( *db->FldKey( 0 )== 'a' || *db->FldKey( 0 )== 'x' ))
       continue;
//     if( !aGaseous && *db->FldKey( 0 )== 'g' )
//       continue;
     if( !aSorption && *db->FldKey( 0 )== 'x' )
       continue;

     RecInput( aPHkey[ii].c_str() );
     //test record
     for( i=0, cnt=0; i<php->nDC; i++ )
     {
        // test to exist of DCOMP or REACDC record later
        // only 3 field
        gstring key = gstring( php->SM[i], 0, DC_RKLEN);
        if( php->DCS[i] == SRC_DCOMP )
            nRec = rt[RT_DCOMP].FindPart( php->SM[i], 3 );
        else
            nRec = rt[RT_REACDC].FindPart( php->SM[i], 3 );
        if( nRec )
         cnt++;
     } // i
     if( cnt < php->nDC )
     {
       if( php->nDC > 1 && cnt > 0 )
         aPHnoused.Add( aPHkey[ii] );
       continue;
     }
     // !!! changing record key
/*     gstring str= gstring(db->FldKey( 4 ), 0, db->FldLen( 4 ));
     for( i=0; i<db->FldLen( 4 ); i++ )
         if( str[i] == ' ' )
          break;
     if( i == db->FldLen( 4 ) )
          i--;
     if( str[i] == *prfName )
            str[i] = ' ';
      else
            str[i] = *prfName;
*/
    gstring str= gstring(db->FldKey( 4 ), 0, db->FldLen( 4 ));
    ChangeforTempl( str, "?*", "invcase*d", db->FldLen( 4 ));
        str += ":";
        gstring str1 = gstring(db->FldKey( 3 ), 0, db->FldLen( 3 ));
        str1.strip();
        str = str1 + ":" + str;
        str1 = gstring(db->FldKey( 2 ), 0, db->FldLen( 2 ));
        str1.strip();
        str = str1 + ":" + str;
        str1 = gstring(db->FldKey( 1 ), 0, db->FldLen( 1 ));
        str1.strip();
        str = str1 + ":" + str;
        str1 = gstring(db->FldKey( 0 ), 0, db->FldLen( 0 ));
        str1.strip();
        str = str1 + ":" + str;
     AddRecord( str.c_str(), fnum_ );
    }
    // close all no profile files
    TCStringArray names1;
    names1.Add(prfName);
    db->OpenOnlyFromList(names1);
}

// ----------------- End of m_phase.cpp -------------------------

