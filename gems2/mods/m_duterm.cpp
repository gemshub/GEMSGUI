//-------------------------------------------------------------------
// $Id$
// To be finalized in Version 4.0 (2005)
// Implementation of TDuterm class, config and calculation functions
//
// Rewritten from C to C++ by S.Dmytriyeva
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
const char *GEMS_DT_HTML = "gm_duterm";

#include <math.h>

#include "m_duterm.h"
#include "m_syseq.h"
#include "v_object.h"
#include "v_module.h"
#include "s_formula.h"
#include "service.h"
#include "visor.h"

TDuterm* TDuterm::pm;

TDuterm::TDuterm( int nrt ):
        TCModule( nrt )
{
    nQ = 1;
    aFldKeysHelp.Add("l<10 Identifier of root multisystem <-PROJECT");
    aFldKeysHelp.Add("l<3 Symbol of thermodynamic potential to minimize<-SYSTEM");
    aFldKeysHelp.Add("l<12 Identifier of chemical system definition <-SYSTEM");
    aFldKeysHelp.Add("l<5  CSD variant number <integer> <-SYSTEM");
    aFldKeysHelp.Add(
        "l<8  Volume V of the system (L) or 0 (no volume constraint) <float> <-EQSTAT");
    aFldKeysHelp.Add("l<8  Pressure P, bar <float> or 0 (Psat H2O) <-EQSTAT");
    aFldKeysHelp.Add("l<8  Temperature T, deg.C <float>  <-EQSTAT");
    aFldKeysHelp.Add("l<4  Variant number of thermodynamic PT-data <integer> <-EQSTAT");
    aFldKeysHelp.Add("l<4  Variant number of DUTERM-task <integer>");
    utp=&ut[0];
    set_def();
    start_title = " Dual-Thermodynamic calculations (under construction) ";
}

// get key of record
gstring
TDuterm::GetKeyofRecord( const char *oldKey, const char *strTitle,
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
        rt[RT_DUTERM].SetKey(str.c_str());
        rt[RT_SYSEQ].MakeKey( RT_DUTERM, pkey, RT_DUTERM, 0, RT_DUTERM, 1,
                               RT_DUTERM, 2, RT_DUTERM, 3, RT_DUTERM, 4,
                               RT_DUTERM, 5, RT_DUTERM, 6, RT_DUTERM, 7, K_END);
        str = TSysEq::pm->GetKeyofRecord( pkey,
         " Please, select a parent system for a new DUterm ", KEY_OLD );
        if(  str.empty() )
        {
            str = pkey.p;
            str += "*:";
        }
        else
            str += "*:";
    }
    str = TCModule::GetKeyofRecord(str.c_str(), strTitle, keyType );
    if(  str.empty() )
        return str;
    rt[RT_DUTERM].SetKey(str.c_str());
    keyTest( str.c_str() );
    return str;
}

// test TDuterm key to calc mode
void TDuterm::keyTest( const char *key )
{
    vstr pkey(MAXRKEYLEN+10);

    if( pVisor->ProfileMode == true )
    { // test project key
        gstring prfKey = gstring( rt[RT_PARAM].FldKey(0), 0, rt[RT_PARAM].FldLen(0));
        StripLine(prfKey);
        int k = prfKey.length();
        if( memcmp(key, prfKey.c_str(), k ) ||
                ( key[k] != ':' && key[k] != ' ' && k>=rt[RT_PARAM].FldLen(0) )  )
            Error( key, "Illegal key (another Project)!");
        rt[RT_SYSEQ].MakeKey( RT_DUTERM, pkey, RT_DUTERM, 0, RT_DUTERM, 1,
                               RT_DUTERM, 2, RT_DUTERM, 3, RT_DUTERM, 4,
                               RT_DUTERM, 5, RT_DUTERM, 6, RT_DUTERM, 7, K_END);
        if( rt[RT_SYSEQ].Find(pkey) <0 )
            Error( key, "Illegal key (no system)!");
    }
}

// link values to objects
void TDuterm::ods_link( int q)
{
    // aObj[ o_utstk].SetPtr( utp->stkey );
    // aObj[ o_utnuv].SetPtr( utp->NUV );
    aObj[ o_utis ].SetPtr( utp->IS );   /*f 2*/
    aObj[ o_utph ].SetPtr( utp->pH );   /*f 2*/
    aObj[ o_uteh ].SetPtr( utp->Eh );   /*f 2*/
    aObj[ o_utyms].SetPtr( utp->YmS );  /*f 2*/
    aObj[ o_utstv].SetPtr( &utp->T );    /*f 3*/
    aObj[ o_utmvs].SetPtr( &utp->Mwat ); /*f 4*/
    aObj[ o_utpvs].SetPtr( &utp->PvDC ); /*a 10*/
    aObj[ o_utdim].SetPtr( &utp->Ld );   /*i 8*/
    aObj[ o_utfloat].SetPtr(utp->IS );
    aObj[ o_utname].SetPtr( utp->name );
    aObj[ o_utnotes].SetPtr(utp->notes );
    aObj[ o_utphkey].SetPtr(utp->phkey );
    aObj[ o_utllf].SetPtr(  utp->llf );
    aObj[ o_utllf].SetDim( utp->Ld, 1 );
    aObj[ o_utnnf].SetPtr(  utp->nnf );
    aObj[ o_utnnf].SetDim( utp->Nr, 1 );
    aObj[ o_utsdm].SetPtr(  utp->SDM );
    aObj[ o_utsdm].SetDim( utp->L, 1 );
    aObj[ o_utddf].SetPtr(  utp->DDF );
    aObj[ o_utddf].SetDim( 1, utp->L*32 );
    aObj[ o_utdcc].SetPtr(  utp->DCC );
    aObj[ o_utdcc].SetDim( utp->L, 1 );
    aObj[ o_utdcs].SetPtr(  utp->DCS );
    aObj[ o_utdcs].SetDim( utp->L, 1 );
    aObj[ o_utreslt].SetPtr(utp->UtRes );
    aObj[ o_utreslt].SetDim( utp->L, 1 );
    aObj[ o_utune].SetPtr(  utp->UnE );
    aObj[ o_utune].SetDim( utp->L, 1 );
    aObj[ o_utunwx].SetPtr( utp->UnWx );
    aObj[ o_utunwx].SetDim( utp->L, 1 );
    aObj[ o_utunic].SetPtr(utp->UnICm );
    aObj[ o_utunic].SetDim( utp->Nr ,1 );
    aObj[ o_utsbm].SetPtr(  utp->SBM );
    aObj[ o_utsbm].SetDim( utp->Nr, 1 );
    aObj[ o_utexpr].SetPtr( utp->Expr );
    aObj[ o_utmu].SetPtr(   utp->MU );
    aObj[ o_utmu].SetDim(  utp->L, 1 );
    aObj[ o_utgt0].SetPtr(  utp->gT0 );
    aObj[ o_utgt0].SetDim( utp->L, 1 );
    aObj[ o_utgtp].SetPtr(  utp->gTP );
    aObj[ o_utgtp].SetDim( utp->L, 1 );
    aObj[ o_utgex].SetPtr(  utp->gEx );
    aObj[ o_utgex].SetDim( utp->L ,1 );
    aObj[ o_utgexk].SetPtr( utp->gExK );
    aObj[ o_utgexk].SetDim(utp->L ,1 );
    aObj[ o_utuic].SetPtr(  utp->UIC );
    aObj[ o_utuic].SetDim( utp->Nr, 1 );
    aObj[ o_ut_qp].SetPtr(  utp->qp );
    aObj[ o_ut_qp].SetDim( utp->Ncq, 1 );
    aObj[ o_utwx].SetPtr(   utp->WX );
    aObj[ o_utwx].SetDim(  utp->L, 1 );
    aObj[ o_utcx].SetPtr(   utp->CX );
    aObj[ o_utcx].SetDim(  utp->L, 1 );
    aObj[ o_utgam].SetPtr(  utp->Gam );
    aObj[ o_utgam].SetDim( utp->L, 1 );
    aObj[ o_utpsi].SetPtr(  utp->Psi );
    aObj[ o_utpsi].SetDim( utp->L ,1 );
    aObj[ o_utnsph].SetPtr( utp->Nsph );
    aObj[ o_utnsph].SetDim(utp->L ,1 );
    aObj[ o_utnsig].SetPtr( utp->Nsig );
    aObj[ o_utnsig].SetDim(utp->L ,1 );
    aObj[ o_utarea].SetPtr( utp->Area );
    aObj[ o_utarea].SetDim(utp->L ,1 );
    aObj[ o_utsigm].SetPtr( utp->Sigm );
    aObj[ o_utsigm].SetDim(utp->L ,1 );
    aObj[ o_utwarm].SetPtr( utp->WArm );
    aObj[ o_utwarm].SetDim(utp->L ,1 );
    aObj[ o_utyarm].SetPtr( utp->YArm );
    aObj[ o_utyarm].SetDim(utp->L ,1 );
    aObj[ o_uterrea].SetPtr(utp->ErrEa );
    aObj[ o_uterrea].SetDim( utp->Nr, 1 );
    aObj[ o_uticme].SetPtr(utp->ICmE );
    aObj[ o_uticme].SetDim( utp->Nr, 1 );
    aObj[ o_uticmc].SetPtr(utp->ICmC );
    aObj[ o_uticmc].SetDim( utp->Nr, 1 );
    aObj[ o_ut_ime].SetPtr(utp->ImE );
    aObj[ o_ut_ime].SetDim( utp->Nr, 1 );
    aObj[ o_ut_ima].SetPtr(utp->ImA );
    aObj[ o_ut_ima].SetDim( utp->Nr, 1 );
    aObj[ o_utwirm].SetPtr(utp->Wrm );
    aObj[ o_utwirm].SetDim( utp->Nr, 1 );
    aObj[ o_utyirm].SetPtr(utp->Yrm );
    aObj[ o_utyirm].SetDim( utp->Nr, 1 );
    aObj[ o_utsdref].SetPtr(utp->sdref );
    aObj[ o_utsdref].SetDim(utp->Nsd, 1 );
    aObj[ o_utsdval].SetPtr(utp->sdval );
    aObj[ o_utsdval].SetDim(utp->Nsd, 1 );
    // work
    aObj[ o_uttpkey].SetPtr(utp->tpkey );
    aObj[ o_uta].SetPtr(    utp->A );
    aObj[ o_uta].SetDim( utp->L ,utp->Nr );
    aObj[ o_uttprn].SetPtr( utp->tprn );
    /* aObj[ o_uttprn].SetDim( 1,VIZmsize(st[q].tprn )); */
    utp=&ut[q];
}

// set dynamic Objects ptr to values
void TDuterm::dyn_set(int q)
{
    ErrorIf( utp!=&ut[q], GetName(), "Illegal access to ut in dyn_set.");
    utp->llf  =  (short *)aObj[ o_utllf ].GetPtr();
    utp->nnf  =  (short *)aObj[ o_utnnf ].GetPtr();
    utp->SDM  =  (char (*)[DC_RKLEN])aObj[ o_utsdm ].GetPtr();
    utp->DDF  =  (char *)aObj[ o_utddf ].GetPtr();
    utp->DCC  =  (char *)aObj[ o_utdcc ].GetPtr();
    utp->DCS  =  (char *)aObj[ o_utdcs ].GetPtr();
    utp->UtRes = (char *)aObj[ o_utreslt ].GetPtr();
    utp->UnE  =  (char *)aObj[ o_utune ].GetPtr();
    utp->UnWx  = (char *)aObj[ o_utunwx ].GetPtr();
    utp->UnICm = (char *)aObj[ o_utunic ].GetPtr();
    utp->SBM  =  (char (*)[MAXICNAME+MAXSYMB])aObj[ o_utsbm ].GetPtr();
    utp->Expr  = (char *)aObj[ o_utexpr ].GetPtr();
    utp->MU   =  (double *)aObj[ o_utmu ].GetPtr();
    utp->gT0   = (double *)aObj[ o_utgt0 ].GetPtr();
    utp->gTP   = (double *)aObj[ o_utgtp ].GetPtr();
    utp->gEx   = (double *)aObj[ o_utgex ].GetPtr();
    utp->gExK  = (double *)aObj[ o_utgexk ].GetPtr();
    utp->UIC  =  (double *)aObj[ o_utuic ].GetPtr();
    utp->qp  =   (double *)aObj[ o_ut_qp ].GetPtr();
    utp->WX   =  (float *)aObj[ o_utwx ].GetPtr();
    utp->CX   =  (float *)aObj[ o_utcx ].GetPtr();
    utp->Gam   = (float *)aObj[ o_utgam ].GetPtr();
    utp->Psi   = (float *)aObj[ o_utpsi ].GetPtr();
    utp->Nsph  = (float *)aObj[ o_utnsph ].GetPtr();
    utp->Nsig  = (float *)aObj[ o_utnsig ].GetPtr();
    utp->Area  = (float *)aObj[ o_utarea ].GetPtr();
    utp->Sigm  = (float *)aObj[ o_utsigm ].GetPtr();
    utp->WArm  = (float *)aObj[ o_utwarm ].GetPtr();
    utp->YArm  = (float *)aObj[ o_utyarm ].GetPtr();
    utp->ErrEa = (float *)aObj[ o_uterrea ].GetPtr();
    utp->ICmE  = (float *)aObj[ o_uticme ].GetPtr();
    utp->ICmC  = (float *)aObj[ o_uticmc ].GetPtr();
    utp->ImE  =  (float *)aObj[ o_ut_ime ].GetPtr();
    utp->ImA  =  (float *)aObj[ o_ut_ima ].GetPtr();
    utp->Wrm  =  (float *)aObj[ o_utwirm ].GetPtr();
    utp->Yrm  =  (float *)aObj[ o_utyirm ].GetPtr();
    utp->sdref = (char (*)[V_SD_RKLEN])aObj[ o_utsdref ].GetPtr();
    utp->sdval = (char (*)[V_SD_VALEN])aObj[ o_utsdval ].GetPtr();
    utp->A     = (float *)aObj[ o_uta ].GetPtr();
    utp->tprn  = (char *)aObj[ o_uttprn ].GetPtr();
}

// free dynamic memory in objects and values
void TDuterm::dyn_kill(int q)
{
    ErrorIf( utp!=&ut[q], GetName(), "Illegal access to ut in dyn_kill.");
    utp->llf  =  (short *)aObj[ o_utllf ].Free();
    utp->nnf  =  (short *)aObj[ o_utnnf ].Free();
    utp->SDM  =  (char (*)[DC_RKLEN])aObj[ o_utsdm ].Free();
    utp->DDF  =  (char *)aObj[ o_utddf ].Free();
    utp->DCC  =  (char *)aObj[ o_utdcc ].Free();
    utp->DCS  =  (char *)aObj[ o_utdcs ].Free();
    utp->UtRes = (char *)aObj[ o_utreslt ].Free();
    utp->UnE  =  (char *)aObj[ o_utune ].Free();
    utp->UnWx  = (char *)aObj[ o_utunwx ].Free();
    utp->UnICm = (char *)aObj[ o_utunic ].Free();
    utp->SBM  =  (char (*)[MAXICNAME+MAXSYMB])aObj[ o_utsbm ].Free();
    utp->Expr  = (char *)aObj[ o_utexpr ].Free();
    utp->MU   =  (double *)aObj[ o_utmu ].Free();
    utp->gT0   = (double *)aObj[ o_utgt0 ].Free();
    utp->gTP   = (double *)aObj[ o_utgtp ].Free();
    utp->gEx   = (double *)aObj[ o_utgex ].Free();
    utp->gExK  = (double *)aObj[ o_utgexk ].Free();
    utp->UIC  =  (double *)aObj[ o_utuic ].Free();
    utp->qp  =   (double *)aObj[ o_ut_qp ].Free();
    utp->WX   =  (float *)aObj[ o_utwx ].Free();
    utp->CX   =  (float *)aObj[ o_utcx ].Free();
    utp->Gam   = (float *)aObj[ o_utgam ].Free();
    utp->Psi   = (float *)aObj[ o_utpsi ].Free();
    utp->Nsph  = (float *)aObj[ o_utnsph ].Free();
    utp->Nsig  = (float *)aObj[ o_utnsig ].Free();
    utp->Area  = (float *)aObj[ o_utarea ].Free();
    utp->Sigm  = (float *)aObj[ o_utsigm ].Free();
    utp->WArm  = (float *)aObj[ o_utwarm ].Free();
    utp->YArm  = (float *)aObj[ o_utyarm ].Free();
    utp->ErrEa = (float *)aObj[ o_uterrea ].Free();
    utp->ICmE  = (float *)aObj[ o_uticme ].Free();
    utp->ICmC  = (float *)aObj[ o_uticmc ].Free();
    utp->ImE  =  (float *)aObj[ o_ut_ime ].Free();
    utp->ImA  =  (float *)aObj[ o_ut_ima ].Free();
    utp->Wrm  =  (float *)aObj[ o_utwirm ].Free();
    utp->Yrm  =  (float *)aObj[ o_utyirm ].Free();
    utp->sdref = (char (*)[V_SD_RKLEN])aObj[ o_utsdref ].Free();
    utp->sdval = (char (*)[V_SD_VALEN])aObj[ o_utsdval ].Free();
    utp->A     = (float *)aObj[ o_uta ].Free();
    utp->tprn  = (char *)aObj[ o_uttprn ].Free();
}

// realloc dynamic memory
void TDuterm::dyn_new(int q)
{
    ErrorIf( utp!=&ut[q], GetName(), "Illegal access to ut in dyn_new.");
    utp->SDM=(char (*)[DC_RKLEN])aObj[o_utsdm].Alloc(utp->L,1,DC_RKLEN);
    utp->DDF = (char *)aObj[o_utddf].Alloc( 1, MAXFORMULA*utp->L, S_ );
    utp->DCC = (char *)aObj[ o_utdcc].Alloc( utp->L, 1, A_ );
    utp->DCS = (char *)aObj[ o_utdcs].Alloc( utp->L, 1, A_ );
    utp->UtRes = (char *)aObj[ o_utreslt].Alloc( utp->L, 1, A_ );
    utp->UnE = (char *)aObj[ o_utune].Alloc( utp->L, 1, A_ );
    utp->UnWx = (char *)aObj[ o_utunwx].Alloc( utp->L, 1, A_ );
    utp->MU = (double *)aObj[ o_utmu].Alloc( utp->L, 1, D_ );
    utp->gTP = (double *)aObj[ o_utgtp].Alloc( utp->L, 1, D_ );
    utp->gT0 = (double *)aObj[ o_utgt0].Alloc( utp->L, 1, D_ );
    utp->WX = (float *)aObj[ o_utwx].Alloc( utp->L, 1, F_ );
    utp->CX = (float *)aObj[ o_utcx].Alloc( utp->L, 1, F_ );

    if( utp->PvDC != S_OFF && utp->Ld > 0 )
        utp->llf = (short *)aObj[ o_utllf].Alloc( utp->Ld, 1, I_ );
    else utp->llf  =  (short *)aObj[ o_utllf ].Free();

    if( utp->PvSol != S_OFF )
    {
        utp->Gam = (float *)aObj[ o_utgam].Alloc( utp->L, 1, F_ );
        utp->gEx = (double *)aObj[ o_utgex].Alloc( utp->L, 1, D_ );
    }
    else
    {
        utp->Gam = (float *)aObj[ o_utgam].Free();
        utp->gEx = (double *)aObj[ o_utgex].Free();
    }
    if( utp->PvKin != S_OFF )
        utp->gExK = (double *)aObj[ o_utgexk].Alloc( utp->L, 1, D_ );
    else utp->gExK = (double *)aObj[ o_utgexk].Free();

    if( utp->PvDis != S_OFF )
    {
        utp->Area = (float *)aObj[ o_utarea].Alloc( utp->L, 1, F_ );
        utp->Sigm = (float *)aObj[ o_utsigm].Alloc( utp->L, 1, F_ );
    }
    else
    {
        utp->Area  = (float *)aObj[ o_utarea ].Free();
        utp->Sigm  = (float *)aObj[ o_utsigm ].Free();
    }

    if( utp->PvSur != S_OFF )
    {
        utp->Psi = (float *)aObj[ o_utpsi].Alloc( utp->L, 1, F_ );
        utp->Nsph = (float *)aObj[ o_utnsph].Alloc( utp->L, 1, F_ );
        utp->Nsig = (float *)aObj[ o_utnsig].Alloc( utp->L, 1, F_ );
    }
    else
    {
        utp->Psi   = (float *)aObj[ o_utpsi ].Free();
        utp->Nsph  = (float *)aObj[ o_utnsph ].Free();
        utp->Nsig  = (float *)aObj[ o_utnsig ].Free();
    }

    if( utp->PvSd != S_OFF && utp->Nsd > 0 )
    {
        utp->sdref=(char (*)[V_SD_RKLEN])aObj[o_utsdref].Alloc(utp->Nsd,1,V_SD_RKLEN);
        utp->sdval=(char (*)[V_SD_VALEN])aObj[o_utsdval].Alloc(utp->Nsd,1,V_SD_VALEN);
    }
    else
    {
        utp->sdref = (char (*)[V_SD_RKLEN])aObj[ o_utsdref ].Free();
        utp->sdval = (char (*)[V_SD_VALEN])aObj[ o_utsdval ].Free();
    }
    if( utp->PvYmS != S_OFF )
    {
        utp->WArm = (float *)aObj[ o_utwarm].Alloc( utp->L, 1, F_ );
        utp->YArm = (float *)aObj[ o_utyarm].Alloc( utp->L, 1, F_ );
        utp->UnICm = (char *)aObj[ o_utunic].Alloc( utp->Nr, 1, A_ );
        utp->SBM = (char (*)[MAXICNAME+MAXSYMB])aObj[o_utsbm].Alloc( utp->Nr,
                   1, MAXICNAME+MAXSYMB );
        utp->nnf = (short *)aObj[ o_utnnf].Alloc( utp->Nr, 1, I_ );
        utp->ErrEa = (float *)aObj[ o_uterrea].Alloc( utp->Nr, 1, F_ );
        utp->ICmE = (float *)aObj[ o_uticme].Alloc( utp->Nr, 1, F_ );
        utp->ICmC = (float *)aObj[ o_uticmc].Alloc( utp->Nr, 1, F_ );
        utp->ImE = (float *)aObj[ o_ut_ime].Alloc( utp->Nr, 1, F_ );
        utp->ImA = (float *)aObj[ o_ut_ima].Alloc( utp->Nr, 1, F_ );
        utp->Wrm = (float *)aObj[ o_utwirm].Alloc( utp->Nr, 1, F_ );
        utp->Yrm = (float *)aObj[ o_utyirm].Alloc( utp->Nr, 1, F_ );
        utp->UIC = (double *)aObj[ o_utuic].Alloc( utp->Nr, 1, D_ );
    }
    else
    {
        utp->WArm = (float *)aObj[ o_utwarm].Free();
        utp->YArm = (float *)aObj[ o_utyarm].Free();
        utp->UnICm = (char *)aObj[ o_utunic].Free();
        utp->SBM = (char (*)[MAXICNAME+MAXSYMB])aObj[o_utsbm].Free();
        utp->nnf = (short *)aObj[ o_utnnf].Free();
        utp->ErrEa = (float *)aObj[ o_uterrea].Free();
        utp->ICmE = (float *)aObj[ o_uticme].Free();
        utp->ICmC = (float *)aObj[ o_uticmc].Free();
        utp->ImE = (float *)aObj[ o_ut_ime].Free();
        utp->ImA = (float *)aObj[ o_ut_ima].Free();
        utp->Wrm = (float *)aObj[ o_utwirm].Free();
        utp->Yrm = (float *)aObj[ o_utyirm].Free();
        utp->UIC = (double *)aObj[ o_utuic].Free();
    }

    if( utp->PvEqn != S_OFF )
        utp->Expr = (char *)aObj[ o_utexpr].Alloc( 1, 2048, S_ );
    else  utp->Expr = (char *)aObj[ o_utexpr].Free();

    if( utp->PvEqn != S_OFF && utp->Ncq > 0 )
        utp->qp = (double *)aObj[ o_ut_qp].Alloc( 1, utp->Ncq, D_ );
    else  utp->qp = (double *)aObj[ o_ut_qp].Free();
}

//set default information
void TDuterm::set_def( int q)
{
    ErrorIf( utp!=&ut[q], GetName(), "Illegal access to ut in set_def.");
    TProfil* PRof = (TProfil*)(&aMod[RT_PARAM]);

    memcpy( &utp->PvDC, PRof->pa.UTppc, 10 );
    strcpy( utp->name, "`" );
    strcpy( utp->notes, "`" );
    memset( &utp->phkey, 0, sizeof(char)*(PH_RKLEN) );
    memset( &utp->Ld, 0, sizeof(short)*8 );
    memset( &utp->T, 0, sizeof(float)*10 );
    utp->IS[0] =0.;
    utp->IS[1] =0.;
    utp->pH[0] =0.;
    utp->pH[1] =0.;
    utp->Eh[0] =0.;
    utp->Eh[1] =0.;
    utp->YmS[0] =0.;
    utp->YmS[1] =0.;

    utp->llf  = 0;
    utp->nnf  = 0;
    utp->SDM  = 0;
    utp->DDF  = 0;
    utp->DCC  = 0;
    utp->DCS  = 0;
    utp->UtRes =0;
    utp->UnE  = 0;
    utp->UnWx  =0;
    utp->UnICm = 0;
    utp->SBM  = 0;
    utp->Expr  = 0;
    utp->MU   =  0;
    utp->gT0   = 0;
    utp->gTP   = 0;
    utp->gEx   = 0;
    utp->gExK  = 0;
    utp->UIC  =  0;
    utp->qp  =   0;
    utp->WX   =  0;
    utp->CX   =  0;
    utp->Gam   = 0;
    utp->Psi   = 0;
    utp->Nsph  = 0;
    utp->Nsig  = 0;
    utp->Area  = 0;
    utp->Sigm  = 0;
    utp->WArm  = 0;
    utp->YArm  = 0;
    utp->ErrEa = 0;
    utp->ICmE  = 0;
    utp->ICmC  = 0;
    utp->ImE  =  0;
    utp->ImA  =  0;
    utp->Wrm  =  0;
    utp->Yrm  =  0;
    utp->sdref = 0;
    utp->sdval = 0;
    utp->A     = 0;
    utp->tprn  = 0;
}

// return true if nessasary recalc base SYSEQ
bool TDuterm::check_input( const char */*key*/, int /*Level*/ )
{
    vstr pkey(MAXRKEYLEN+10);
    if( pVisor->ProfileMode != true )
        return true;

    TProfil* PRof = (TProfil*)(&aMod[RT_PARAM]);
    //Get base SysEq key from process key
    rt[RT_SYSEQ].MakeKey( RT_DUTERM, pkey, RT_DUTERM, 0, RT_DUTERM, 1,
                           RT_DUTERM, 2, RT_DUTERM, 3, RT_DUTERM, 4,
                           RT_DUTERM, 5, RT_DUTERM, 6, RT_DUTERM, 7, K_END);
    // read SysEq record and unpack data
    PRof->loadSystat( pkey );
    // test changes in system after process calc
    if( rt[RT_SYSEQ].Rtime() > rt[nRT].Rtime() )
        return true;
    return false;
}

//Rebild record structure before calc
int
TDuterm::RecBuild( const char *key, int mode  )
{
    TProfil* PRof = (TProfil*)(&aMod[RT_PARAM]);
    TCStringArray aDclist;
    TCIntArray aMdv;
    TCStringArray aIclist;
    TCIntArray aMiv;
    short i, j, ju, jm, jj, k, jp, kp, ist=0, ND, LD;
    bool TO_LOAD = false;
    TFormula aFo;
    gstring form;


    if( pVisor->ProfileMode != true )
        Error( GetName(), "Do it in Project mode!" );

    LD = utp->Ld;
    ND = utp->Nr;
AGAIN:
    int ret = TCModule::RecBuild( key, mode );
    if( ret == VF_CANCEL )
        goto SET_OK;
    if( ret == VF3_1 )
        TO_LOAD = true;
    if( utp->Nsd > 0 ) utp->PvSd = S_ON;
    else utp->PvSd = S_OFF;

    // Select Dcomp list

    for( i=0; i<PRof->mup->L; i++ )
        aDclist.Add( PRof->mup->SM[i]);
    aMdv.Clear();
    if( LD > 0 && utp->llf )
        for( i=0; i<LD; i++ )
            aMdv.Add( utp->llf[i]  );
    else
        for( i=0; i<TSysEq::pm->stp->L; i++ )
            aMdv.Add( TSysEq::pm->stp->llf[i]  );
LOOP_MARKDC: // select DC from calc sostav
    aMdv = vfMultiChoiceSet(window(), aDclist,"c_ddcp  (mark DC to be included)", aMdv );
    LD = aMdv.GetCount();
    if( LD < 1 )
    {
        if( vfQuestion(window(), GetName(),"No DComp keys selected for DUTERM definition!"
                       "\nRepeat marking keys?" ))
            goto LOOP_MARKDC;
        else Error( GetName(),"No DComp keys selected for DUTERM definition!" );
    }
    utp->Ld = LD;
    utp->L = utp->La+utp->Ld;
    if(  utp->La < 0 || utp->La > PRof->mup->L || utp->Ncq  < 0 )
        goto AGAIN;

    if( utp->PvYmS != S_OFF )
    { // Select IC to deviation calc
        // Select Icomp list
        for( i=0; i<PRof->mup->N; i++ )
            aIclist.Add( PRof->mup->SB[i]);
        aMiv.Clear();
        if( ND > 0 && utp->nnf )
            for( i=0; i<ND; i++ )
                aMiv.Add( utp->nnf[i]  );
        else
            for( i=0; i<TSysEq::pm->stp->N; i++ )
                aMiv.Add(TSysEq::pm->stp->nnf[i]  );
LOOP_MARKIC: // select DC from calc sostav
        aMiv = vfMultiChoiceSet(window(), aIclist,"c_dicp  (mark IC to be included)", aMiv );
        ND = aMiv.GetCount();
        if( ND < 1 )
        {
            if( vfQuestion(window(), GetName(),"No IComp keys selected for DUTERM definition!"
                           "\nRepeat marking keys?" ))
                goto LOOP_MARKIC;
            else Error( GetName(),"No IComp keys selected for DUTERM definition!" );
        }
        utp->Nr = ND;
    }
    else utp->Nr = 0;

    /* Make here select of phase key*/
    // realloc data
    dyn_new();
    if( LD > 0 )  // Make DC list
    {
        for( j=0; j<LD; j++ )
            utp->llf[j] = aMdv[j];
        utp->PvDC = S_ON;
    }
    else utp->PvDC = S_OFF;

    if( utp->PvYmS != S_OFF && ND > 0 ) // Make IC list
    {
        for( i=0; i<ND; i++ )
            utp->nnf[i] = aMiv[i];
        utp->PvYmS = S_ON;
    }
    else utp->PvYmS = S_OFF;

    /* load names and formules of usual and add DC */
    /*  if( L1 == utp->L )  goto TAILS;  */
    *utp->DDF = 0;
    for( ju = 0; ju < utp->L; ju++ )
    {
        if( ju < utp->Ld )
        {
            jm = utp->llf[ju];
            for( i=0; i<PRof->pmp->L; i++ )
            { /*search indexes in MULTI */
                if( PRof->pmp->muj[i] == jm )
                {
                    jp = i;
                    /* get phase index */
                    for(jj=0, k=0; k<PRof->pmp->FI; k++ )
                    {
                        if( jp >= jj && jp < jj+PRof->pmp->L1[k] )
                        {
                            kp = k; //km = PRof->pmp->muk[k];
                            break;
                        }
                        jj+=PRof->pmp->L1[k];
                    }
                    goto JP_GET;
                }
            }
            /* Error: This is not DC  in MULTI */
            kp = -1; /*km = -1;*/
            jp = -1;
            /* Get phase index in RMULTS */
            for(jj=0, k=0; k<PRof->mup->Fi; k++ )
            {
                if( jm >= jj && jm < jj+PRof->mup->Ll[k] )
                { /*km = k;*/ break;
                }
                jj+=PRof->mup->Ll[k];
            }
JP_GET:
            form = aFo.form_extr( jm, PRof->mup->L, PRof->mup->DCF );
            if( ju > 0 )
                strcat( utp->DDF, ",\n" );
            strncat( utp->DDF, form.c_str(), MAXFORMULA-4 );
            memcpy( utp->SDM[ju], PRof->mup->SM[jm], DC_RKLEN );
            utp->DCC[ju] = PRof->mup->DCC[jm];
            utp->DCS[ju] = PRof->mup->DCS[jm];
            utp->gT0[ju] = PRof->tpp->G[jm];
            utp->MU[ju] = 0.0;
            if( TO_LOAD )
            {
                utp->WX[ju] = 1.0;
                utp->Gam[ju] = 1.0;
                utp->UnWx[ju] = CON_MOLFR;
            }
            utp->UnE[ju] = PRof->tpp->PunE;
            utp->UtRes[ju] = DUT_GETPX; /*by default !!! */

            if( utp->PvSol != S_OFF && TO_LOAD )
            {
                if( jp >= 0 )
                {
                    utp->WX[ju] = PRof->pmp->Wx[jp];
                    utp->Gam[ju] = exp( PRof->pmp->lnGam[jp] );
                }
                else
                {
                    utp->WX[ju] = 1.0;
                    utp->Gam[ju] = 1.0;
                }
                switch( PRof->mup->DCC[jm] )
                {
                case DC_AQ_PROTON:
                case DC_AQ_ELECTRON:
                case DC_AQ_SPECIES:
                    utp->UnWx[ju] = CON_MOLAL;
                    utp->WX[ju] = PRof->pmp->Y_m[jp];
                    utp->Gam[ju] = PRof->pmp->Gamma[jp];
                    break;
                case DC_AQ_SOLVENT:
                case DC_SUR_MINAL:
                case DC_SUR_CARRIER:
                case DC_GAS_COMP:
                case DC_GAS_H2O:
                case DC_GAS_CO2:
                case DC_GAS_H2:
                case DC_GAS_N2:
                case DC_SCP_CONDEN:
                case DC_SOL_IDEAL:
                case DC_SOL_MINOR:
                case DC_SOL_MAJOR:
                    utp->UnWx[ju] = CON_MOLFR;
                    utp->Gam[ju] = PRof->pmp->Gamma[jp];
                    break;
                case DC_SUR_SITE:
                case DC_SUR_COMPLEX: /* case DC_SUR_EX_ION: */
                case DC_SUR_IPAIR:
                default:
                    utp->UnWx[ju] = SUR_CONN;
                    utp->Gam[ju] = PRof->pmp->Gamma[jp];
                    break;
                }
                utp->UtRes[jm] = DUT_GETPX;
            }
            if( utp->PvDis != S_OFF && TO_LOAD )
                if( kp >= 0 )
                {
                    utp->Area[ju] = PRof->pmp->Aalp[kp];
                    utp->Sigm[ju] = PRof->pmp->Sigw[kp];
                }
                else
                {
                    utp->Area[ju] = 0.0;
                    utp->Sigm[ju] = 0.0;
                }
            if( utp->PvSur != S_OFF && TO_LOAD )
                if( kp >= 0 && PRof->pmp->PHC[kp] == PH_SORPTION )
                {
                    switch( utp->DCC[ju] )
                    {
                    case DC_SUR_MINAL:
                    case DC_SUR_CARRIER:
                        break;
                    case DC_SUR_SITE:
                    case DC_SUR_COMPLEX:
                    default:
                        utp->Psi[ju] = PRof->pmp->XpsiA[kp][ist];
                        break;
                    case DC_SUR_IPAIR:
                        utp->Psi[ju] = PRof->pmp->XpsiB[kp][ist];
                        break;
                        /*
                                    case DC_SUR_EX_ION: utp->Psi[ju] = PRof->pmp->XpsiD[kp][ist];
                                                        break;
                        */
                    }
                    utp->Nsph[ju] = PRof->pmp->Nfsp[kp][ist];
                    utp->Nsig[ju] = PRof->pmp->MASDT[kp][ist];
                }
                else
                {
                    utp->Psi[ju] = 0.0;
                    utp->Nsph[ju] = FLOAT_EMPTY; /* Do more ! */
                    utp->Nsig[ju] = 1.0;
                }
            if( utp->PvKin != S_OFF && TO_LOAD )
                if( jp >= 0 )
                {
                    utp->gExK[ju] = PRof->pmp->GEX[jp];
                }
                else
                {
                    if( PRof->syp->GEX )
                        utp->gExK[ju] = PRof->syp->GEX[jm];
                    else utp->gExK[ju] = DOUBLE_EMPTY;
                }
        }
        else
            if(TO_LOAD)
            { /* add formules */
                memcpy( utp->SDM[ju], "Component_N          ", DC_RKLEN );
                strcat( utp->DDF, ",\nFormula_N" );
                utp->DCC[ju] = 'I';  /* undefined type */
                utp->DCS[ju] = 'n';
                utp->UnE[ju] = PRof->tpp->PunE;
                utp->UnWx[ju] = CON_MOLFR;
                utp->gT0[ju] = 7777777;
                utp->UtRes[ju] = DUT_G0TP;  /* by default !!! */
                utp->WX[ju] = 1.0;
                utp->Gam[ju] = 1.0;
                utp->MU[ju] = 0.0;

                if( utp->PvSol != S_OFF && TO_LOAD )
                {
                    utp->WX[ju] = 1.;
                    utp->Gam[ju] = 1.;
                    utp->UnWx[ju] = CON_MOLFR;
                }
                if( utp->PvDis != S_OFF && TO_LOAD )
                {
                    utp->Area[ju] = 1.;
                    utp->Sigm[ju] = 0.4;
                }
                if( utp->PvSur != S_OFF && TO_LOAD )
                {
                    utp->Psi[ju] = 0.;
                    utp->Nsph[ju] = 7.;
                    utp->Nsig[ju] = 1.;
                }
                if( utp->PvKin != S_OFF && TO_LOAD )
                {
                    utp->gExK[ju] = 0.;
                }
            }
    }
    // TAILS:
    if( /* TO_LOAD && */ PRof->pmp->LO )
    {   //scalar_load( TO_LOAD );
        utp->IS[1] = PRof->pmp->IC;
        utp->pH[1] = PRof->pmp->pH;
        utp->Eh[1] = PRof->pmp->Eh;
        utp->Mwat = PRof->pmp->X[PRof->pmp->LO]*PRof->pmp->MM[PRof->pmp->LO]/1000.;
        utp->Vsys = PRof->pmp->VXc;
        utp->Msys = PRof->pmp->MBX;
        utp->Maq = PRof->pmp->FWGT[0]/1000;
        utp->Vaq = PRof->pmp->FVOL[0]/1000.;
    }
    if( utp->PvYmS != S_OFF && TO_LOAD )
    {
        for( ju=0; ju<utp->L; ju++ )
        {
            /*  utp->CX[ju] = 1.; */
            utp->WArm[ju] = 1.;
            utp->YArm[ju] = 0.;
        }

        for(short im, ip, iu=0; iu<utp->Nr; iu++ )
        { /* get IC indexes */
            im = utp->nnf[iu];
            for( i=0; i<PRof->pmp->N; i++ )
                if( im == PRof->pmp->mui[i] )
                {
                    ip = i;
                    goto ICI_GET;
                }
            Error( GetName(), "Illegal IC name!");
ICI_GET:
            /* Load IC parametres */
            memcpy( utp->SBM[iu], PRof->pmp->SB[ip], MAXICNAME+MAXSYMB );
            utp->UnICm[iu] = CON_MOLAL;
            utp->ErrEa[iu] = 0.;
            utp->ICmE[iu] = 0.;
            utp->ICmC[iu] = PRof->pmp->IC_m[ip];
            utp->ImE[iu] = 0.;
            utp->ImA[iu] = 0.;
            utp->Yrm[iu] = 0.;
            utp->Wrm[iu] = 1.;
            utp->UIC[iu] = PRof->pmp->U_r[ip];
        }
    }

SET_OK:
    pVisor->Update();
    return ret;
}

//Recalc record structure
void
TDuterm::RecCalc( const char *key )
{
    if( pVisor->ProfileMode != true )
        Error( GetName(), "Do it in Project mode!" );
    duterm_calc();
    TCModule::RecCalc(key);
}

// Translate, analyze and unpack equations of process
void TDuterm::ut_text_analyze()
{
    if( utp->PvEqn == S_OFF )
        return;
    try
    {
        TProfil* PRof = (TProfil*)(&aMod[RT_PARAM]);

        PRof->ET_translate( o_uttprn, o_utexpr, 0,PRof->mup->L, 0, PRof->pmp->L );
        rpn.GetEquat( (char *)aObj[o_uttprn].GetPtr() );
    }
    catch( TError& xcpt )
    {
        char *erscan = (char *)aObj[o_utexpr].GetPtr();
        vfMessage(window(), xcpt.title, xcpt.mess);
        /*bool   iRet = */
        CheckEqText( erscan,
                      "Error in translation of DUterm math script: " );
        /*  if( iRet )
               goto AGAIN;  */
        Error(  GetName() , xcpt.mess.c_str() );
    }
}

void
TDuterm::CmHelp()
{
    pVisor->OpenHelp( GEMS_DT_HTML );  //  05.01.01
}


// -------------------- End of m_duterm.cpp -------------------------


