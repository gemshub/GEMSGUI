//-------------------------------------------------------------------
// Id: gems/mods/m_probe.cpp  version 2.0.0   2001
// Under construction, to be included in version 3.0 (2003)
// Implementation of TProbe class, config and calculation functions
//
// Rewritten from C to C++ by S.Dmytriyeva  970207 modified 010904
// Copyright (C) 1995-2001 K.Chudnenko, S.Dmytriyeva, D.Kulik
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
const char *GEMS_USP_HTML = "gm_probe";

#include <math.h>
#include <stdio.h>

#include "m_probe.h"
#include "m_syseq.h"
#include "v_object.h"
#include "service.h"
#include "visor.h"

TProbe* TProbe::pm;

const float defPB = 0.01;

TProbe::TProbe( int nrt ):
        TCModule( nrt )
{
    nQ = 1;
    aFldKeysHelp.Add("l<10 Identifier of modelling project_");
    aFldKeysHelp.Add("l<3 Symbol of thermodynamic potential to minimize_");
    aFldKeysHelp.Add("l<12 Identifier of chemical system definition_");
    aFldKeysHelp.Add("l<5  Composition variant number <integer>_");
    aFldKeysHelp.Add(
        "l<8  Volume V of the system (in L) or 0 (no volume constraint)_");
    aFldKeysHelp.Add("l<8  Pressure P, bar <float> or 0 (Psat H2O)_");
    aFldKeysHelp.Add("l<8  Temperature T, deg.C <float>_");
    aFldKeysHelp.Add("l<4  Constraints variant number_");
    aFldKeysHelp.Add("l<4  Number of PROBE-task <integer>_");
    prp=&pr[0];
    set_def();
    start_title  = " Probing Uncertainty Space (under construction) ";
}

// get key of record
gstring
TProbe::GetKeyofRecord( const char *oldKey, const char *strTitle,
                        int keyType )
{
    gstring str;

    if( oldKey == 0 )
    {
        if(Filter.empty())
            str = ALLKEY;
        else str = Filter;
    }
    else str = oldKey;

    if( keyType == KEY_NEW )
    { // Get key of base SyStat
        vstr pkey(MAXRKEYLEN+10);
        rt[RT_PROBE].SetKey(str.c_str());
        rt[RT_SYSEQ].MakeKey( RT_PROBE, pkey, RT_PROBE, 0, RT_PROBE, 1,
                               RT_PROBE, 2, RT_PROBE, 3, RT_PROBE, 4,
                               RT_PROBE, 5, RT_PROBE, 6, RT_PROBE, 7, K_END);
        str = TSysEq::pm->GetKeyofRecord( pkey,
         " Please, select a parent system for a new Probe ", KEY_OLD);
        if(  str.empty() )
        {
            str = pkey.p;
            str += "*:";
        }
        else
            str += "*:";
    }
    str = TCModule::GetKeyofRecord( str.c_str(), strTitle, keyType );
    if(  str.empty() )
        return str;
    rt[RT_PROBE].SetKey(str.c_str());
    keyTest( str.c_str() );
    return str;
}

// test Probe key to calc mode
void TProbe::keyTest( const char *key )
{
    vstr pkey(MAXRKEYLEN+10);

    if( pVisor->ProfileMode == true )
    { // test project key
        gstring prfKey = gstring( rt[RT_PARAM].FldKey(0), 0, rt[RT_PARAM].FldLen(0));
        StripLine(prfKey);
        int k = prfKey.length();
        if( memcmp(key, prfKey.c_str(), k ) ||
                ( key[k] != ':' && key[k] != ' ' && k>=rt[RT_PARAM].FldLen(0) )  )
            Error( key, "Illegal key (another Modelling Project)!");
        rt[RT_SYSEQ].MakeKey( RT_PROBE, pkey, RT_PROBE, 0, RT_PROBE, 1,
                               RT_PROBE, 2, RT_PROBE, 3, RT_PROBE, 4,
                               RT_PROBE, 5, RT_PROBE, 6, RT_PROBE, 7, K_END);
        if( rt[RT_SYSEQ].Find(pkey) <0 )
            Error( key, "Illegal key (no system)!");
    }
}

// link values to objects
void TProbe::ods_link( int q)
{
    // aObj[ o_prstk ].SetPtr( prb[q].stkey1 );
    // aObj[ o_prnuv ].SetPtr( prb[q].NUV );
    aObj[ o_prdim ].SetPtr( &prp->N );    /*i 10*/
    aObj[ o_prntpv].SetPtr( &prp->NgT );  /*i 3*/
    aObj[ o_printt].SetPtr( &prp->T );    /*f 2*/
    aObj[ o_printp].SetPtr( &prp->P );    /*f 2*/
    aObj[ o_printv].SetPtr( &prp->V );    /*f 2*/
    aObj[ o_prname].SetPtr( prp->name );
    aObj[ o_prnotes].SetPtr( prp->notes );
    aObj[ o_przond].SetPtr( prp->zond );   /*a 14*/
    aObj[ o_print].SetPtr(  &prp->N );     /*i 16*/
    aObj[ o_prfloat].SetPtr( &prp->T );  /*f 6*/
    // N
    aObj[ o_prnnf ].SetPtr( prp->nnf );
    aObj[ o_prnnf].SetDim( prp->N, 1 );
    aObj[ o_prbs  ].SetPtr( prp->Bs );
    aObj[ o_prbs].SetDim( prp->N, 1 );
    aObj[ o_prngn ].SetPtr( prp->NgN );
    aObj[ o_prngn].SetDim( prp->N, 1 );
    aObj[ o_prin  ].SetPtr( prp->IntN );
    aObj[ o_prin].SetDim( prp->N, 1 );
    aObj[ o_prsbp ].SetPtr( prp->SBp );
    aObj[ o_prsbp].SetDim( prp->N, 1 );
    // L
    aObj[ o_prllf].SetPtr( prp->llf );
    aObj[ o_prllf].SetDim( prp->L, 1 );
    aObj[ o_prnglg].SetPtr( prp->NgLg );
    aObj[ o_prnglg].SetDim( prp->L, 1 );
    aObj[ o_prngls].SetPtr( prp->NgLs );
    aObj[ o_prngls].SetDim( prp->L, 1 );
    aObj[ o_prnglv].SetPtr( prp->NgLv );
    aObj[ o_prnglv].SetDim( prp->L, 1 );
    aObj[ o_prlgam].SetPtr( prp->NgLgam );
    aObj[ o_prlgam].SetDim( prp->L, 1 );
    aObj[ o_prlgex].SetPtr( prp->NgLgex );
    aObj[ o_prlgex].SetDim( prp->L, 1 );
    aObj[ o_prilg].SetPtr( prp->IntLg );
    aObj[ o_prilg].SetDim( prp->L, 1 );
    aObj[ o_prils].SetPtr( prp->IntLs );
    aObj[ o_prils].SetDim( prp->L, 1 );
    aObj[ o_prilv].SetPtr( prp->IntLv );
    aObj[ o_prilv].SetDim( prp->L, 1 );
    aObj[ o_prigm].SetPtr( prp->IntLgam );
    aObj[ o_prigm].SetDim( prp->L, 1 );
    aObj[ o_prigx].SetPtr( prp->IntLgex );
    aObj[ o_prigx].SetDim( prp->L, 1 );
    aObj[ o_prgs].SetPtr( prp->Gs );
    aObj[ o_prgs].SetDim( prp->L, 1 );
    aObj[ o_prss].SetPtr( prp->Ss );
    aObj[ o_prss].SetDim( prp->L, 1 );
    aObj[ o_prvs].SetPtr( prp->Vs );
    aObj[ o_prvs].SetDim( prp->L, 1 );
    aObj[ o_prgams].SetPtr( prp->GAMs );
    aObj[ o_prgams].SetDim( prp->L, 1 );
    aObj[ o_prgex].SetPtr( prp->GEXs );
    aObj[ o_prgex].SetDim( prp->L, 1 );
    aObj[ o_prsmp].SetPtr( prp->SMp );
    aObj[ o_prsmp].SetDim( prp->L, 1 );
    // FI
    aObj[ o_prkkf].SetPtr( prp->kkf );
    aObj[ o_prkkf].SetDim( prp->FI, 1 );
    aObj[ o_prl1].SetPtr( prp->L1 );
    aObj[ o_prl1].SetDim( prp->FI, 1 );
    aObj[ o_prsfp].SetPtr( prp->SFp );
    aObj[ o_prsfp].SetDim( prp->FI, 1 );
    //KG
    aObj[ o_przpg].SetPtr( prp->ZPg );
    aObj[ o_przpg].SetDim( prp->kG, 1 );
    aObj[ o_prsgp].SetPtr( prp->SGp );
    aObj[ o_prsgp].SetDim( prp->kG, 1 );
    //KGR
    aObj[ o_provr].SetPtr( prp->OVR );
    aObj[ o_provr].SetDim( prp->kGR, 1 );
    //nI
    aObj[ o_priopt].SetPtr( prp->iopt );
    aObj[ o_priopt].SetDim( prp->nI, 1 );
    aObj[ o_prexpr].SetPtr( prp->Expr );  //aObj[ o_prexpr].SetDim( 1, VIZmsize(prp->Expr));
    aObj[ o_prstl].SetPtr( prp->stl );
    aObj[ o_prstl].SetDim( prp->QT, 1 );
    aObj[ o_prsdref].SetPtr(prp->sdref);
    aObj[ o_prsdref].SetDim( prp->Nsd, 1 );
    aObj[ o_prsdval].SetPtr(prp->sdval);
    aObj[ o_prsdval].SetDim( prp->Nsd, 1 );
    aObj[ o_prtprn].SetPtr( prp->tprn ); // aObj[ o_prtprn].SetDim( 1 VIZmsize(prp->tprn ));
    /* work data */
    aObj[ o_prnmv ].SetPtr( &prp->nMV );
    aObj[ o_prloop].SetPtr( &prp->Loop );
    aObj[ o_prnrk ].SetPtr( &prp->c_nrk );
    aObj[ o_prtm  ].SetPtr( &prp->c_tm );
    aObj[ o_prnv  ].SetPtr( &prp->c_NV );
    prp=&pr[q];
}

// set dynamic Objects ptr to values
void TProbe::dyn_set(int q)
{
    int i;
    TProfil* PRof = (TProfil*)(&aMod[RT_PARAM]);

    ErrorIf( prp!=&pr[q], GetName(),
             "Illegal access to pr in dyn_set.");
    prp->nnf = (short *)aObj[ o_prnnf].GetPtr();
    prp->Bs = (double *)aObj[ o_prbs].GetPtr();
    prp->NgN = (short *)aObj[ o_prngn].GetPtr();
    prp->IntN = (float *)aObj[ o_prin].GetPtr();
    prp->SBp = (char (*)[MAXICNAME+MAXSYMB])aObj[ o_prsbp].GetPtr();
    prp->llf = (short *)aObj[ o_prllf].GetPtr();  /* L */
    prp->NgLg = (short *)aObj[ o_prnglg].GetPtr();
    prp->NgLs = (short *)aObj[ o_prngls].GetPtr();
    prp->NgLv = (short *)aObj[ o_prnglv].GetPtr();
    prp->NgLgam = (short *)aObj[ o_prlgam].GetPtr();
    prp->NgLgex = (short *)aObj[ o_prlgex].GetPtr();
    prp->IntLg = (float *)aObj[ o_prilg].GetPtr();
    prp->IntLs = (float *)aObj[ o_prils].GetPtr();
    prp->IntLv = (float *)aObj[ o_prilv].GetPtr();
    prp->IntLgam = (float *)aObj[ o_prigm].GetPtr();
    prp->IntLgex = (float *)aObj[ o_prigx].GetPtr();
    prp->OVR = (float *)aObj[ o_provr].GetPtr();
    prp->iopt = (short *)aObj[ o_priopt].GetPtr();
    prp->Gs = (float *)aObj[ o_prgs].GetPtr();
    prp->Ss = (float *)aObj[ o_prss].GetPtr();
    prp->Vs = (float *)aObj[ o_prvs].GetPtr();
    prp->GAMs = (float *)aObj[ o_prgams].GetPtr();
    prp->GEXs = (float *)aObj[ o_prgex].GetPtr();
    prp->SMp = (char (*)[MAXDCNAME])aObj[ o_prsmp].GetPtr();
    prp->kkf = (short *)aObj[ o_prkkf].GetPtr(); /* FI */
    prp->L1 = (short *)aObj[ o_prl1].GetPtr();
    prp->SFp = (char (*)[MAXPHNAME])aObj[ o_prsfp].GetPtr();
    prp->ZPg = (char *)aObj[ o_przpg].GetPtr();    /* KG */
    prp->SGp = (char (*)[MAXPHNAME])aObj[ o_prsgp].GetPtr();
    prp->Expr = (char *)aObj[ o_prexpr].GetPtr();
    prp->stl = (char (*)[EQ_RKLEN])aObj[ o_prstl].GetPtr();
    prp->sdref = (char (*)[V_SD_RKLEN])aObj[ o_prsdref].GetPtr();
    prp->sdval = (char (*)[V_SD_VALEN])aObj[ o_prsdval].GetPtr();
    prp->tprn = (char *)aObj[ o_prtprn].GetPtr();
    // make work arrays
    if( pVisor->ProfileMode != true )
        return;
    if( prp->N )
    {
        prp->SBp =(char (*)[MAXICNAME+MAXSYMB])aObj[ o_prsbp].Alloc(
                      prp->N, 1, MAXICNAME+MAXSYMB );
        for( i=0; i<prp->N; i++ )
            memcpy( prp->SBp[i], PRof->mup->SB[prp->nnf[i]], MAXICNAME+MAXSYMB );
    }
    else prp->SBp = (char (*)[MAXICNAME+MAXSYMB])aObj[ o_prsbp].Free();
    if( prp->L )
    {
        prp->SMp =(char (*)[MAXDCNAME])aObj[ o_prsmp].Alloc( prp->L, 1, MAXDCNAME );
        for( i=0; i<prp->L; i++ )
            memcpy( prp->SMp[i], PRof->mup->SM[prp->llf[i]], MAXDCNAME );
    }
    else  prp->SMp = (char (*)[MAXDCNAME])aObj[ o_prsmp].Free();
}

// free dynamic memory in objects and values
void TProbe::dyn_kill(int q)
{
    ErrorIf( prp!=&pr[q], GetName(),
             "Illegal access to pr in dyn_kill.");
    prp->nnf = (short *)aObj[ o_prnnf].Free();
    prp->Bs = (double *)aObj[ o_prbs].Free();
    prp->NgN = (short *)aObj[ o_prngn].Free();
    prp->IntN = (float *)aObj[ o_prin].Free();
    prp->SBp = (char (*)[MAXICNAME+MAXSYMB])aObj[ o_prsbp].Free();
    prp->llf = (short *)aObj[ o_prllf].Free();  /* L */
    prp->NgLg = (short *)aObj[ o_prnglg].Free();
    prp->NgLs = (short *)aObj[ o_prngls].Free();
    prp->NgLv = (short *)aObj[ o_prnglv].Free();
    prp->NgLgam = (short *)aObj[ o_prlgam].Free();
    prp->NgLgex = (short *)aObj[ o_prlgex].Free();
    prp->IntLg = (float *)aObj[ o_prilg].Free();
    prp->IntLs = (float *)aObj[ o_prils].Free();
    prp->IntLv = (float *)aObj[ o_prilv].Free();
    prp->IntLgam = (float *)aObj[ o_prigm].Free();
    prp->IntLgex = (float *)aObj[ o_prigx].Free();
    prp->OVR = (float *)aObj[ o_provr].Free();
    prp->iopt = (short *)aObj[ o_priopt].Free();
    prp->Gs = (float *)aObj[ o_prgs].Free();
    prp->Ss = (float *)aObj[ o_prss].Free();
    prp->Vs = (float *)aObj[ o_prvs].Free();
    prp->GAMs = (float *)aObj[ o_prgams].Free();
    prp->GEXs = (float *)aObj[ o_prgex].Free();
    prp->SMp = (char (*)[MAXDCNAME])aObj[ o_prsmp].Free();
    prp->kkf = (short *)aObj[ o_prkkf].Free(); /* FI */
    prp->L1 = (short *)aObj[ o_prl1].Free();
    prp->SFp = (char (*)[MAXPHNAME])aObj[ o_prsfp].Free();
    prp->ZPg = (char *)aObj[ o_przpg].Free();    /* KG */
    prp->SGp = (char (*)[MAXPHNAME])aObj[ o_prsgp].Free();
    prp->Expr = (char *)aObj[ o_prexpr].Free();
    prp->stl = (char (*)[EQ_RKLEN])aObj[ o_prstl].Free();
    prp->sdref = (char (*)[V_SD_RKLEN])aObj[ o_prsdref].Free();
    prp->sdval = (char (*)[V_SD_VALEN])aObj[ o_prsdval].Free();
    prp->tprn = (char *)aObj[ o_prtprn].Free();
}

// realloc dynamic memory
void TProbe::dyn_new(int q)
{
    ErrorIf( prp!=&pr[q], GetName(),
             "Illegal access to pr in dyn_new.");
    if( prp->N )
    {
        prp->nnf =(short *)aObj[ o_prnnf].Alloc(  prp->N, 1, I_ );
        prp->SBp =(char (*)[MAXICNAME+MAXSYMB])aObj[ o_prsbp].Alloc(
                      prp->N, 1, MAXICNAME+MAXSYMB );
    }
    else
    {
        prp->nnf = (short *)aObj[ o_prnnf].Free();
        prp->SBp = (char (*)[MAXICNAME+MAXSYMB])aObj[ o_prsbp].Free();
    }
    if( prp->zond[2] != S_OFF && prp->N )   //b
    {
        prp->Bs =(double *)aObj[ o_prbs].Alloc(  prp->N, 1, D_ );
        prp->NgN =(short *)aObj[ o_prngn].Alloc( prp->N, 1, I_ );
        prp->IntN =(float *)aObj[ o_prin].Alloc( prp->N, 1, F_ );
    }
    else
    {
        prp->Bs = (double *)aObj[ o_prbs].Free();
        prp->NgN = (short *)aObj[ o_prngn].Free();
        prp->IntN = (float *)aObj[ o_prin].Free();
    }
    if( prp->L )
    {
        prp->llf =(short *)aObj[ o_prllf].Alloc( prp->L, 1, I_ );
        prp->SMp =(char (*)[MAXDCNAME])aObj[ o_prsmp].Alloc( prp->L, 1, MAXDCNAME );
    }
    else
    {
        prp->llf = (short *)aObj[ o_prllf].Free();  /* L */
        prp->SMp = (char (*)[MAXDCNAME])aObj[ o_prsmp].Free();
    }
    if( prp->zond[0] != S_OFF && prp->L )
    {
        prp->Gs =(float *)aObj[ o_prgs].Alloc( prp->L, 1, F_ );
        prp->NgLg =(short *)aObj[ o_prnglg].Alloc( prp->L, 1, I_ );
        prp->IntLg =(float *)aObj[ o_prilg].Alloc( prp->L, 1, F_ );
    }
    else
    {
        prp->NgLg = (short *)aObj[ o_prnglg].Free();
        prp->IntLg = (float *)aObj[ o_prilg].Free();
        prp->Gs = (float *)aObj[ o_prgs].Free();
    }
    if( prp->zond[1] != S_OFF && prp->L )
    {
        prp->Ss =(float *)aObj[ o_prss].Alloc( prp->L, 1, F_ );
        prp->NgLs =(short *)aObj[ o_prngls].Alloc( prp->L, 1, I_ );
        prp->IntLs =(float *)aObj[ o_prils].Alloc( prp->L, 1, F_ );
    }
    else
    {
        prp->NgLs = (short *)aObj[ o_prngls].Free();
        prp->IntLs = (float *)aObj[ o_prils].Free();
        prp->Ss = (float *)aObj[ o_prss].Free();
    }
    if( prp->zond[5] != S_OFF && prp->L )
    {
        prp->Vs =(float *)aObj[ o_prvs].Alloc( prp->L, 1, F_ );
        prp->NgLv =(short *)aObj[ o_prnglv].Alloc( prp->L, 1, I_ );
        prp->IntLv =(float *)aObj[ o_prilv].Alloc( prp->L, 1, F_ );
    }
    else
    {
        prp->NgLv = (short *)aObj[ o_prnglv].Free();
        prp->IntLv = (float *)aObj[ o_prilv].Free();
        prp->Vs = (float *)aObj[ o_prvs].Free();
    }
    if( prp->zond[6] != S_OFF && prp->L )
    {
        prp->GAMs =(float *)aObj[ o_prgams].Alloc( prp->L, 1, F_ );
        prp->NgLgam =(short *)aObj[ o_prlgam].Alloc( prp->L, 1, I_ );
        prp->IntLgam =(float *)aObj[ o_prigm].Alloc( prp->L, 1, F_ );
    }
    else
    {
        prp->NgLgam = (short *)aObj[ o_prlgam].Free();
        prp->IntLgam = (float *)aObj[ o_prigm].Free();
        prp->GAMs = (float *)aObj[ o_prgams].Free();
    }
    if( prp->zond[7] != S_OFF && prp->L )
    {
        prp->GEXs =(float *)aObj[ o_prgex].Alloc( prp->L, 1, F_ );
        prp->NgLgex =(short *)aObj[ o_prlgex].Alloc( prp->L, 1, I_ );
        prp->IntLgex =(float *)aObj[ o_prigx].Alloc( prp->L, 1, F_ );
    }
    else
    {
        prp->NgLgex = (short *)aObj[ o_prlgex].Free();
        prp->IntLgex = (float *)aObj[ o_prigx].Free();
        prp->GEXs = (float *)aObj[ o_prgex].Free();
    }
    if( prp->FI )
    {
        prp->kkf =(short *)aObj[ o_prkkf].Alloc( prp->FI, 1, I_ );
        prp->L1 =(short *)aObj[ o_prl1].Alloc( prp->FI, 1, I_ );
        prp->SFp =(char (*)[MAXPHNAME])aObj[ o_prsfp].Alloc( prp->FI, 1,MAXPHNAME );
    }
    else
    {
        prp->kkf = (short *)aObj[ o_prkkf].Free(); /* FI */
        prp->L1 = (short *)aObj[ o_prl1].Free();
        prp->SFp = (char (*)[MAXPHNAME])aObj[ o_prsfp].Free();
    }
    if( prp->kG )
    {
        prp->ZPg =(char *)aObj[ o_przpg].Alloc( prp->kG, 1, A_ );
        prp->SGp =(char (*)[MAXPHNAME])aObj[ o_prsgp].Alloc( prp->kG, 1, MAXPHNAME );
    }
    else
    {
        prp->ZPg = (char *)aObj[ o_przpg].Free();    /* KG */
        prp->SGp = (char (*)[MAXPHNAME])aObj[ o_prsgp].Free();
    }
    if( prp->Zexpr != S_OFF )
    {
        prp->Expr =(char *)aObj[ o_prexpr].Alloc( 1, 2048, S_ );
        prp->tprn =(char *)aObj[ o_prtprn].Alloc( 1, 2048, S_ );
    }
    else
    {
        prp->Expr = (char *)aObj[ o_prexpr].Free();
        prp->tprn = (char *)aObj[ o_prtprn].Free();
    }
    if( prp->Nsd > 0 )
    {
        prp->sdref =(char (*)[V_SD_RKLEN])aObj[ o_prsdref].Alloc(
                        prp->Nsd, 1, V_SD_RKLEN );
        prp->sdval =(char (*)[V_SD_VALEN])aObj[ o_prsdval].Alloc(
                        prp->Nsd, 1, V_SD_VALEN );
    }
    else
    {
        prp->sdref = (char (*)[V_SD_RKLEN])aObj[ o_prsdref].Free();
        prp->sdval = (char (*)[V_SD_VALEN])aObj[ o_prsdval].Free();
    }
    if( prp->kGR )
        prp->OVR =(float *)aObj[ o_provr].Alloc( prp->kGR+1, 1, F_ );
    else   prp->OVR = (float *)aObj[ o_provr].Free();

    prp->nI = 0;
    prp->iopt = (short *)aObj[ o_priopt].Free();
    prp->stl =(char (*)[EQ_RKLEN])aObj[ o_prstl].Alloc( prp->QT, 1, EQ_RKLEN );
}

//set default information
void TProbe::set_def( int q)
{
    ErrorIf( prp!=&pr[q], GetName(),
             "Illegal access to pr in set_def.");
    memcpy( &prp->zond, "------------", 12 );
    prp->Zexpr = '-';
    prp->pbcalc = '0';
    strcpy( prp->name, "`" );
    strcpy( prp->notes, "`" );
    memset( &prp->N, 0, sizeof(short)*16 );
    memset( &prp->T, 0, sizeof(float)*6 );
    memset( &prp->nMV, 0, sizeof(short)*5 );
    memset( &prp->stkey, 0, sizeof(char)*(EQ_RKLEN+10) );

    prp->nnf = 0;
    prp->Bs = 0;
    prp->NgN = 0;
    prp->IntN = 0;
    prp->SBp = 0;
    prp->llf = 0;
    prp->NgLg = 0;
    prp->NgLs = 0;
    prp->NgLv = 0;
    prp->NgLgam = 0;
    prp->NgLgex = 0;
    prp->IntLg = 0;
    prp->IntLs = 0;
    prp->IntLv = 0;
    prp->IntLgam = 0;
    prp->IntLgex = 0;
    prp->OVR = 0;
    prp->iopt = 0;
    prp->Gs = 0;
    prp->Ss = 0;
    prp->Vs = 0;
    prp->GAMs = 0;
    prp->GEXs = 0;
    prp->SMp = 0;
    prp->kkf = 0;
    prp->L1 = 0;
    prp->SFp = 0;
    prp->ZPg = 0;
    prp->SGp =  0;
    prp->Expr = 0;
    prp->stl = 0;
    prp->sdref = 0;
    prp->sdval = 0;
    prp->tprn = 0;
}

// return true if nessasary recalc base SYSEQ
bool TProbe::check_input( const char */*key*/, int /*Level*/ )
{
    vstr pkey(MAXRKEYLEN+10);
    if( pVisor->ProfileMode != true )
        return true;

    TProfil* PRof = (TProfil*)(&aMod[RT_PARAM]);
    //Get base SysEq key from process key
    rt[RT_SYSEQ].MakeKey( RT_PROBE, pkey, RT_PROBE, 0, RT_PROBE, 1,
                           RT_PROBE, 2, RT_PROBE, 3, RT_PROBE, 4,
                           RT_PROBE, 5, RT_PROBE, 6, RT_PROBE, 7, K_END);
    // read SysEq record and unpack data
    PRof->loadSystat( pkey );
    // test changes in system after process calc
    if( rt[RT_SYSEQ].Rtime() > rt[nRT].Rtime() )
        return true;
    return false;
}

// Input nessasery data and links objects
void TProbe::RecInput( const char *key )
{
    TCModule::RecInput( key );
}

short smp[90]=
    {    0,   1,   2,   3,   5,   7,  11,  13,  17,  19,
         23,  29,  31,  37,  43,  53,  61,  71,  79,  89,
         97, 101, 109, 127, 137, 149, 157, 167, 179, 191,
         199, 211, 223, 233, 241, 251, 263, 271, 281, 293,
         307, 317, 331, 347, 359, 373, 383, 401, 419, 431,
         443, 457, 467, 487, 499, 509, 521, 557, 577, 599,
         601, 653, 701, 751, 797, 853, 877, 907, 953, 997,
         1009,1051,1103,1201,1249,1301,1361,1399,1409,1451,
         1499,1601,1699,1721,1753,1801,1847,1901,1951,1999 };
// Get  a prime number of PROBE points
short TProbe::get_simple( short oldSimp )
{
    TCStringArray arr;
    vstr pbuf(20);
    int sel=0;

    for(int i=0; i <90; i++)
    {
        if( oldSimp == smp[i] )
            sel =i;
        sprintf( pbuf, "%-5d", smp[i]);
        arr.Add( pbuf.p );
    }
    do
    {
        sel = vfChoice(window(), arr, "Pb_numQT  Select a prime number of PROBE points", sel);
    }
    while( sel<0 );
    return( smp[sel] );
}

// test  a prime number of PROBE points
bool TProbe::is_simple( short  Simp )
{
    for(int i=0; i <90; i++)
    {
        if( Simp == smp[i] )
            return true;
        if(  Simp < smp[i] )
            return false;
    }
    return false;
}

// calc interval
float TProbe::UnsG( float x, float dx )
{
    float p;
    if( dx > 1e-4 ) p = dx;
    else p = fabs(x)*defPB;
    return( p);
}

int TProbe::WL(int i,int j,int QT)
{
    int W = i*j - floor(i*j/QT)*QT;
    if(QT-W<W) W=QT-W;
    return(W);
}

// start generate Pseudo-random numbers of uniform distribution law
void TProbe::BELOV()
{
    int QT,NG,QT2,i,j,k,D,B,W,MC,m2,*LI,*LI1;

    QT=prp->QT;
    NG=prp->kGR;
    QT2=(QT-1)/2;
    LI= new int[QT2+1];
    LI1= new int[QT2+1];
    if( !LI || !LI1 )
        Error( GetName(), "No memory in BELOV.");
    memset( LI, 0, (QT2+1)*sizeof(int));
    memset( LI1, 0, (QT2+1)*sizeof(int));
    W=floor(NG/QT2);
    if(W)
    {
        k=1;
        for(i=1;i<=QT2;i++)
            for(j=0;j<=W-1;j++)
                prp->OVR[i+j*QT2]=i;
    }
    else k=0;
    W*=QT2;
    if(W>=NG)
    {
        delete[] LI;
        delete[] LI1;
        return;
    }
    prp->OVR[W+1]=k*QT2+1;
    for(i=1;i<=QT2;i++)
        LI[i]=WL(i,prp->OVR[W+1],QT);
    for(D=W+2;D<=NG;D++)
    {
        MC=0;
        for(j=1;j<=QT;j++)
        {
            for(i=1;i<=QT2;i++)
                LI1[i]=WL(i,j,QT)+LI[i];
            B=LI1[1];
            for(i=2;i<=QT2;i++)
                if(LI1[i]<B) B=LI1[i];
            m2=0;
            if(B==MC)
                for(i=1;i<=D-1;i++)
                    if(prp->OVR[i]==prp->OVR[D])
                    {
                        prp->OVR[D]=j;
                        m2=1;
                        break;
                    }
            if(!m2&&B>MC)
            {
                MC=B;
                prp->OVR[D]=j;
            }
        }
        for(i=1;i<=QT2;i++)
            LI[i]+=WL(i,prp->OVR[D],QT);
    }
    delete[] LI;
    delete[] LI1;
}

// test data in Probe and set group arrays
void TProbe::pr_init()
{
    int i;
    /* number of groups <= kG */
    if( ( prp->NgT < 0 || prp->NgT > prp->kG) &&
            ( prp->NgP < 0 || prp->NgP > prp->kG) &&
            ( prp->NgV < 0 || prp->NgV > prp->kG) )
        Error( GetName(), "Illegal groups for T, P, V!");
    if( prp->NgN )
        for( i=0; i<prp->N; i++ )
            if(  prp->NgN[i] < 0 || prp->NgN[i] > prp->kG)
                Error( GetName(), "Illegal group indices for b variation!");

    for( i=0; i<prp->L; i++ )
    {
        if(prp->NgLg )
            if( prp->NgLg[i] < 0 || prp->NgLg[i] > prp->kG)
                Error( GetName(), "Illegal group indices for G0 variation!");
        if(prp->NgLs )
            if( prp->NgLs[i] < 0 || prp->NgLs[i] > prp->kG)
                Error( GetName(), "Illegal group indices for S0 variation!");
        if(prp->NgLv )
            if( prp->NgLv[i] < 0 || prp->NgLv[i] > prp->kG)
                Error( GetName(), "Illegal group indices for V0 variation!");
        if(prp->NgLgam )
            if( prp->NgLgam[i] < 0 || prp->NgLgam[i] > prp->kG)
                Error( GetName(), "Illegal group indices for GAMMA variation!");
        if(prp->NgLgex )
            if( prp->NgLgex[i] < 0 || prp->NgLgex[i] > prp->kG)
                Error( GetName(), "Illegal group indices for GEX variation!");
    }
    /* calc kGN kGR*/
    prp->kGR = prp->kGN = 0;
    for( i=0; i<prp->kG; i++)
        switch( prp->ZPg[i])
        {
        case PRB_UNIFORM:
            prp->kGR++;
            break;
        case PRB_NORMAL:
            prp->kGN++;
            break;
        default:
            Error( GetName(), "Illegal type distribution law (u/n)!");
        }
    if( prp->kGR )
    {
        prp->OVR = (float *)aObj[ o_provr ].Alloc( prp->kGR+1, 1, F_ );
        BELOV();
    }
    else  prp->OVR = (float *)aObj[ o_provr ].Free();
}

//Rebild record structure before calc
int
TProbe::RecBuild( const char *key, int mode  )
{
    TCIntArray aMdv;
    TCIntArray aMiv;
    TProfil* PRof = (TProfil*)(&aMod[RT_PARAM]);
    int i;

    if( pVisor->ProfileMode != true )
        Error( GetName(), "Do it in Project mode!" );

AGAIN:
    prp->QT = get_simple( prp->QT );
    int ret = TCModule::RecBuild( key, mode );
    if( ret == VF_CANCEL )
        goto SET_OK;
    if( !is_simple(prp->QT) || prp->Nsd< 0   )
    {
        vfMessage(window(), GetName(), "Must be a prime number of PROBE points!");
        goto AGAIN;
    }
    /* Get Icomp list to calc variations */
    if( prp->zond[2] != S_OFF )
    {
        TCStringArray aIclist;

        for( i=0; i<PRof->mup->N; i++ )
            aIclist.Add( PRof->mup->SB[i]);
LOOP_MARKIC: // select IC from calc sostav
        if( prp->N && prp->nnf )// old selections in IComp
        {
            aMiv.Clear();
            for( i=0; i<prp->N; i++ )
                aMiv.Add( prp->nnf[i]  );
        }
        aMiv = vfMultiChoiceSet(window(), aIclist,
                                "pb_ic_keys  Select keys of uncertain independent components", aMiv );
        prp->N = aMiv.GetCount();
        if( prp->N < 1 )
        {
            if( vfQuestion(window(), GetName(),  "Select IC or cancel IC selection mode!" ))
                goto LOOP_MARKIC;
            else prp->zond[2] = S_OFF;
        }
    }
    else  prp->N = 0;

    /* Get Dcomp list to calc variations */
    if( prp->zond[0] != S_OFF || prp->zond[1] != S_OFF ||
            prp->zond[5] != S_OFF || prp->zond[6] != S_OFF ||
            prp->zond[7] != S_OFF )
    {
        TCStringArray aDclist;
        for( i=0; i<PRof->mup->L; i++ )
            aDclist.Add( PRof->mup->SM[i]);
LOOP_MARKDC: // select DC from calc sostav
        if( prp->L && prp->llf ) // old selections in DComp
        {
            aMdv.Clear();
            for( i=0; i<prp->L; i++ )
                aMdv.Add( prp->llf[i]  );
        }
        aMdv = vfMultiChoiceSet(window(), aDclist,
                                "pb_dc_keys  Select keys of uncertain dependent components", aMdv );
        prp->L= aMdv.GetCount();
        if( prp->L < 1 )
        {
            if( vfQuestion(window(), GetName(),  "Select DC or cancel DC selection mode!" ))
                goto LOOP_MARKDC;
            else
            {
                prp->zond[0] = S_OFF;
                prp->zond[1] = S_OFF;
                prp->zond[5] = S_OFF;
                prp->zond[6] = S_OFF;
                prp->zond[7] = S_OFF;
            }
        }
    }
    else  prp->L = 0;
    // realloc data
    dyn_new( 0 );
    /* set default data to Probe arrays  */
    for( i=0; i<prp->N; i++ )
    {
        prp->nnf[i] = aMiv[i];
        memcpy( prp->SBp[i], PRof->mup->SB[aMiv[i]], MAXICNAME+MAXSYMB );
        prp->Bs[i] = PRof->syp->B[aMiv[i]];
        prp->IntN[i] = fabs((float)prp->Bs[i])*defPB;
    }
    for( i=0; i<prp->L; i++ )
    {
        prp->llf[i] = aMdv[i];
        memcpy( prp->SMp[i], PRof->mup->SM[aMdv[i]], MAXDCNAME );
        if( prp->zond[0] != S_OFF )
        {
            prp->Gs[i] = PRof->tpp->G[aMdv[i]];
            if( PRof->tpp->devG )
                prp->IntLg[i] = UnsG( prp->Gs[i], PRof->tpp->devG[aMdv[i]] );
            else prp->IntLg[i] = fabs(prp->Gs[i])*defPB;
        }
        if( prp->zond[1] != S_OFF )
        {
            if( PRof->tpp->S )
                prp->Ss[i] = PRof->tpp->S[aMdv[i]];
            else prp->Ss[i] =0;
            if( PRof->tpp->devS )
                prp->IntLs[i] = UnsG( prp->Ss[i], PRof->tpp->devS[aMdv[i]]);
            else prp->IntLs[i] = fabs(prp->Ss[i])*defPB;
        }
        if( prp->zond[5] != S_OFF )
        {
            if( PRof->tpp->Vm )
                prp->Vs[i] = PRof->tpp->Vm[aMdv[i]];
            else prp->Vs[i] = 0;
            if( PRof->tpp->devV )
                prp->IntLv[i] = UnsG( prp->Vs[i], PRof->tpp->devV[aMdv[i]]);
            else prp->IntLv[i] = fabs(prp->Vs[i])*defPB;
        }
        if( prp->zond[7] != S_OFF )
        {
            if( PRof->syp->GEX )
                prp->GEXs[i] = PRof->syp->GEX[aMdv[i]];
            else prp->GEXs[i] = 0;
            prp->IntLgex[i] = fabs(prp->GEXs[i])*defPB;
        }
        if( prp->zond[6] != S_OFF )
        {
            TSysEq* STat = (TSysEq*)(&aMod[RT_SYSEQ]);
            int j;
            if( STat->stp->lnGam )
            {
                for( j=0; j<STat->stp->L; j++)
                    if( STat->stp->llf[j] == aMdv[i] )
                        break;
                if( j<STat->stp->L )
                    prp->GAMs[i] = STat->stp->lnGam[j];
                else prp->GAMs[i] = 0;
            }
            else prp->GAMs[i] = 0;
            prp->IntLgam[i] = fabs(prp->GAMs[i])*defPB;
        }
    }
    for( i=0; i<prp->kG; i++ )
        if( !prp->ZPg[i] || prp->ZPg[i]==A_NUL )
            prp->ZPg[i] = PRB_UNIFORM;
SET_OK:  // set begin of calc and init arrays
    prp->pbcalc = P_STARTED;
    //   pr_init();
    pVisor->Update();
    return ret;
}

// generate Pseudo-random numbers of normal distribution law
float TProbe::PNTNOR(int I,int J,int QT)
{
    int j,n=100;
    float R;
    /* J-iteration; I-group */
    srand(J*QT+I);
    R=0.;
    for(j=0;j<n;j++)
        R+=rand()/(RAND_MAX-1.);
    R=(R-n/2.)/pow(n/12.,0.5);
    return(R/2.+.5);
}

// start generate Pseudo-random numbers of uniform distribution law
float TProbe::PNTBEL(int J,int QT,int OV)
{
    float TB;
    TB=fabs(J*OV-floor(J*OV/QT)*QT);
    TB*=(QT-1)/2;
    while(1)
        if(TB>=QT) TB-=QT;
        else break;
    return(TB);
}

// calc one point number J
void TProbe::NexT( int J, int iT, int iP )
{
    vstr buf(40);
    int i, i1, i2, j;
    float RT, RP, R, TC, P, V;
    TProfil* PRof = (TProfil*)(&aMod[RT_PARAM]);

    if( prp->zond[0] != S_OFF || prp->zond[1] != S_OFF
            || prp->zond[5] != S_OFF )
    {
        prp->c_NV = J+100;
        sprintf( buf, "%d", prp->c_NV);
        memset( prp->NVp, 0, 4 );
        strncpy( prp->NVp, buf, 3 );
    }
    /* T */
    if( prp->zond[3] != S_OFF && prp->NgT > 0 )
    {
        if( prp->ZPg[prp->NgT-1] == PRB_UNIFORM )
        {
            RT = PNTBEL(J,prp->QT,prp->OVR[iT]);
            if(prp->QT>1) RT/=(prp->QT-1);
        }
        else RT=PNTNOR(iT,J,prp->QT);
        TC=prp->T-prp->IntT+2*prp->IntT*RT;
        if(TC<0.) TC=0.;
        gcvt( TC, 6, prp->TCp );
        TC = atof( prp->TCp );
    }
    else TC=prp->T;
    /* P */
    if( prp->zond[4] != S_OFF && prp->NgP > 0 )
    {
        if( prp->NgP == prp->NgT ) RP = RT;
        else
        {
            if( prp->ZPg[prp->NgP-1] == PRB_UNIFORM )
            {
                RP = PNTBEL(J,prp->QT,prp->OVR[iP]);
                if(prp->QT>1) RP/=(prp->QT-1);
            }
            else RP=PNTNOR(iP,J,prp->QT);
        }
        P = prp->P-prp->IntP+2*prp->IntP*RP;
        if(P<1e-5) P = 1e-5;
        gcvt( P, 6, prp->Pp );
        P = atof( prp->Pp );
    }
    else P=prp->P;
    if( prp->zond[4] != S_OFF || prp->zond[3] != S_OFF  )
    {
        PRof->mtparm->LoadMtparm( TC, P );  // new P, T  (MTPARM TC,P,prp->c_NV=0)
        PRof->pmp->pTPD = 0;
    }
    for( i=1, i1=1, i2=1 ; i<=prp->kG; i++ )
    {
        if( i == prp->NgP)
            R=RP;
        else if( i == prp->NgT)
            R=RT;
        else if( prp->ZPg[i-1] == PRB_UNIFORM )
        {
            R = PNTBEL(J,prp->QT,prp->OVR[i1]);
            if(prp->QT>1) R/=(prp->QT-1);
            i1++;
        }
        else
        {
            R=PNTNOR(i2,J,prp->QT);
            i2++;
        }
        /* V */
        if( prp->zond[8] != S_OFF )
            if(  prp->NgV == i )
            {
                V=prp->V-prp->IntV+2*prp->IntV*R;
                if(V<0.) V=0.;
                gcvt( V, 6, prp->Bnamep );
            }

        if(prp->zond[0] != S_OFF )
            for(j=0;j<prp->L;j++)
                if(  prp->NgLg[j] == i )
                {
                    PRof->tpp->G[ prp->llf[j]] =
                        prp->Gs[j]-prp->IntLg[j]+2*prp->IntLg[j]*R;
                }
        if(prp->zond[1] != S_OFF )
            for(j=0;j<prp->L;j++)
                if(  prp->NgLs[j] == i )
                {
                    PRof->tpp->S[ prp->llf[j]] =
                        prp->Ss[j] - prp->IntLs[j]+2*prp->IntLs[j]*R;
                }
        if(prp->zond[5] != S_OFF )
            for(j=0;j<prp->L;j++)
                if(  prp->NgLv[j] == i )
                {
                    PRof->tpp->Vm[ prp->llf[j]] =
                        prp->Vs[j]-prp->IntLv[j]+2*prp->IntLv[j]*R;
                }
        if(prp->zond[2] != S_OFF )
            for(j=0;j<prp->N;j++)
                if(  prp->NgN[j] == i )
                {
                    PRof->syp->BI[ prp->nnf[j]] = PRof->syp->B[ prp->nnf[j]]-
                                                  (prp->Bs[j]-prp->IntN[j]+2*prp->IntN[j]*R);
                }
        if(prp->zond[7] != S_OFF ) /* GEX */
            for(j=0;j<prp->L;j++)
                if(  prp->NgLgex[j] == i )
                {
                    PRof->syp->GEX[ prp->llf[j]] =
                        prp->GEXs[j]-prp->IntLgex[j]+2*prp->IntLgex[j]*R;
                }
        if(prp->zond[6] != S_OFF ) /* GAM */
            for(j=0;j<prp->L;j++)
                if(  prp->NgLgam[j] == i )
                {
                    PRof->syp->lnGmf[ prp->llf[j]] =
                        prp->GAMs[j]-prp->IntLgam[j]+2*prp->IntLgam[j]*R;
                }
    }
}


//Recalc record structure
void
TProbe::RecCalc( const char *key )
{
    if( pVisor->ProfileMode != true )
        Error( GetName(), "Do it in Project mode!" );
    TCModule::RecCalc(key);

    int I1, i,i1,i2,iT, iP;
    vstr buf(40);
    TProfil* PRof = (TProfil*)(&aMod[RT_PARAM]);

    pr_init();
    prp->pbcalc = P_EXECUTE;

    /* start data of P, T, V  */
    gcvt( prp->T, 6, prp->TCp );
    gcvt( prp->P, 6, prp->Pp );
    gcvt( prp->V, 6, prp->Bnamep );
    prp->c_NV = 0;
    sprintf( buf, "%d", prp->c_NV);
    memset( prp->NVp, 0, 4 );
    strncpy( prp->NVp, buf, 3 );

    /* indexes in OVR */
    iP = iT = 0;
    if( prp->zond[3] != S_OFF || prp->zond[4] != S_OFF )
    {
        for( i=1, i1=0, i2=0 ; i<=prp->kG; i++ )
        {
            if( prp->ZPg[i-1] == PRB_UNIFORM )
            {
                i1++;
                if( i == prp->NgT ) iT = i1;
                if( i == prp->NgP ) iP = i1;
            }
            else
            {
                i2++;
                if( i == prp->NgT ) iT = i2;
                if( i == prp->NgP ) iP = i2;
            }
        }
    }
    // main array
    for( I1 = 0; I1 < prp->QT; I1++ )
    {
        NexT( I1, iT, iP);
        aMod[RT_PROCES].ModUpdate("Pb_calc    PROBE task calculation");
        // Stop Probe from Andy Sveta

        if( prp->zond[0] != S_OFF || prp->zond[1] != S_OFF
                || prp->zond[5] != S_OFF )
        {
            prp->c_NV = I1+100;
            sprintf( buf, "%d", prp->c_NV);
            memset( prp->NVp, 0, 4 );
            strncpy( prp->NVp, buf, 3 );
        }

        if( prp->zond[2] != S_OFF || prp->zond[6] != S_OFF
                || prp->zond[7] != S_OFF )
        {
            prp->c_tm = I1+1000;
            sprintf( buf, "%d", prp->c_tm);
            memset(prp->timep, 0, 5 );
            strncpy(prp->timep, buf, 4 );
            // system test sy_test( prp->sykey, q )
        }

        // new SyStat key

        rt[RT_SYSEQ].MakeKey( RT_PROBE,  prp->stkey, RT_PROBE, 0, RT_PROBE,1,
                               RT_PROBE, 2, K_IMM, prp->timep, K_IMM, prp->Bnamep,
                               K_IMM, prp->Pp, K_IMM, prp->TCp, K_IMM, prp->NVp, K_END );
        rt[RT_SYSEQ].Find(prp->stkey);
        PRof->CalcEqstat(); // calc current SyStat
        TSysEq::pm->CmSave();  // save resalts

        if( prp->stl )
            memcpy( prp->stl+I1, prp->stkey, EQ_RKLEN );
    }
    prp->pbcalc = P_FINISHED;
}

void
TProbe::CmHelp()
{
    pVisor->OpenHelp( GEMS_USP_HTML );  //  05.01.01
}



//-------------------------- End of m_probe.cpp ----------------------------


