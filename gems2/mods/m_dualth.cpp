//-------------------------------------------------------------------
// $Id$
//
// Implementation of TDualTh class, config and calculation functions
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

const char *GEMS_DT_HTML = "gm_dualth";

#include <math.h>
//#include <stdio.h>

#include "m_dualth.h"
#include "m_syseq.h"
#include "m_param.h"
#include "v_object.h"
#include "service.h"
#include "visor.h"

TDualTh* TDualTh::pm;

TDualTh::TDualTh( int nrt ):
        TCModule( nrt )
{
    nQ = 1;
    aFldKeysHelp.Add("l<10 Identifier of the parent modelling project <-Project");
    aFldKeysHelp.Add("l<3  Symbol of thermodynamic potential to minimize <-SysEq");
    aFldKeysHelp.Add("l<12 Identifier of the parent chemical system definition <-SysEq");
    aFldKeysHelp.Add("l<5  CSD variant number <integer> <-SysEq");
    aFldKeysHelp.Add("l<8  Volume V of the system (L) or 0 (no volume constraint) <float> <-SysEq");
    aFldKeysHelp.Add("l<8  Pressure P, bar <float> or 0 (Psat H2O) <-SysEq");
    aFldKeysHelp.Add("l<8  Temperature T, in Centigrade <float>  <-SysEq");
    aFldKeysHelp.Add("l<4  Variant number of thermodynamic PT-data <integer> <-SysEq");
    aFldKeysHelp.Add("l<18 Identifier of this dualth simulator definition");
    aFldKeysHelp.Add("l<4  Batch simulator type code, a combination of {T, P, S, G, R}");

    dtp=&dt[0];
    set_def();
    start_title =
       " Definition of a DualTh Simulator (batch calculation)";

}

// get key of record
gstring
TDualTh::GetKeyofRecord( const char *oldKey, const char *strTitle,
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

    if( keyType==KEY_NEW  )
    { // Get key of base SyStat
        vstr pkey(MAXRKEYLEN+10);
        rt[RT_DUALTH].SetKey(str.c_str());
        rt[RT_SYSEQ].MakeKey( RT_DUALTH, pkey, RT_DUALTH, 0, RT_DUALTH, 1,
                               RT_DUALTH, 2, RT_DUALTH, 3, RT_DUALTH, 4,
                               RT_DUALTH, 5, RT_DUALTH, 6, RT_DUALTH, 7, K_END);
        str = TSysEq::pm->GetKeyofRecord( pkey,
          "Please, select a parent System for a new DualTh ", KEY_OLD );
        if(  str.empty() )
        {
            str = pkey.p;
            str+= "*:*:";
        }
        else
            str += "*:*:";
    }
    str = TCModule::GetKeyofRecord( str.c_str(), strTitle, keyType );
    if(  str.empty() )
        return str;
    rt[RT_DUALTH].SetKey(str.c_str());
    keyTest( str.c_str() );
    return str;
}

// test DualTh key to calc mode
void TDualTh::keyTest( const char *key )
{
    vstr pkey(MAXRKEYLEN+10);

    if( pVisor->ProfileMode == true )
    { // test project key
        gstring prfKey = gstring( rt[RT_PARAM].FldKey(0), 0, rt[RT_PARAM].FldLen(0));
        StripLine(prfKey);
        int k = prfKey.length();
        if( memcmp(key, prfKey.c_str(), k ) ||
                ( key[k] != ':' && key[k] != ' ' && k<rt[RT_PARAM].FldLen(0) )  )
            Error( key, "E08PErem: Illegal record key (another Modelling Project)!");
        rt[RT_SYSEQ].MakeKey( RT_DUALTH, pkey, RT_DUALTH, 0, RT_DUALTH, 1,
                               RT_DUALTH, 2, RT_DUALTH, 3, RT_DUALTH, 4,
                               RT_DUALTH, 5, RT_DUALTH, 6, RT_DUALTH, 7, K_END);
        if( rt[RT_SYSEQ].Find(pkey) <0 )
            Error( key, "E07PErem: Illegal record key (no system)!");
    }
}


// link values to objects
void TDualTh::ods_link( int q)
{
    dtp=&dt[q];

// static
    aObj[ o_dtpvfl ].SetPtr(  &dtp->PunE );    /*a 12*/
    aObj[ o_dtpsflg ].SetPtr(  &dtp->PsMode ); /*a 8*/
    aObj[ o_dtdim ].SetPtr(  &dtp->nQ );    /*i 8*/
    aObj[ o_dttmd ].SetPtr(  dtp->tmd );    /*i 3*/
    aObj[ o_dtnvd ].SetPtr(  dtp->NVd );    /*i 3*/
    aObj[ o_dtpd ].SetPtr(  dtp->Pd );    /*f 3*/
    aObj[ o_dttd ].SetPtr(  dtp->Td );    /*f 3*/
    aObj[ o_dtvd ].SetPtr(  dtp->Vd );    /*f 3*/
    aObj[ o_dtmsysb ].SetPtr(  &dtp->Msysb );    /*f 8*/
    aObj[ o_dtt ].SetPtr(  &dtp->cT );
    aObj[ o_dtp ].SetPtr(  &dtp->cP );
    aObj[ o_dtv ].SetPtr(  &dtp->cV );
    aObj[ o_dtq ].SetPtr(  &dtp->q );
    aObj[ o_dti ].SetPtr(  &dtp->i );
    aObj[ o_dtjm ].SetPtr(  &dtp->jm );
    aObj[ o_dtc_tm ].SetPtr(  &dtp->c_tm );
    aObj[ o_dtc_nv ].SetPtr(  &dtp->c_NV );
    aObj[ o_dtname ].SetPtr(  dtp->name );
    aObj[ o_dtnotes ].SetPtr(  dtp->notes );
    aObj[ o_dtpspv ].SetPtr(  &dtp->PunE );    /*a 20*/
    aObj[ o_dtshort ].SetPtr(  &dtp->nQ );    /*i 14*/
    aObj[ o_dtfloat ].SetPtr(  &dtp->Pd );    /*f 22*/
    aObj[ o_dtstkey ].SetPtr(  dtp->sykey );

// dynamic
    aObj[ o_dtbb ].SetPtr( dtp->Bb );
    aObj[ o_dtbb ].SetDim( dtp->nQ, dtp->Nb );
    aObj[ o_dtbn ].SetPtr( dtp->Bn );
    aObj[ o_dtbn ].SetDim( dtp->nQ, dtp->Nb );
    aObj[ o_dtub ].SetPtr( dtp->Ub );
    aObj[ o_dtub ].SetDim( dtp->nQ, dtp->Nb );
    aObj[ o_dtchi ].SetPtr( dtp->chi );
    aObj[ o_dtchi ].SetDim( dtp->nQ, dtp->nK );
    aObj[ o_dtmu_n ].SetPtr( dtp->mu_n );
    aObj[ o_dtmu_n ].SetDim( dtp->nQ, dtp->nK );
    aObj[ o_dtcoul ].SetPtr( dtp->Coul );
    aObj[ o_dtcoul ].SetDim( dtp->nQ, dtp->nK );
    aObj[ o_dtgam_n ].SetPtr( dtp->gam_n );
    aObj[ o_dtgam_n ].SetDim( dtp->nQ, dtp->nK );
    aObj[ o_dtavg_g ].SetPtr( dtp->avg_g );
    aObj[ o_dtavg_g ].SetDim( 1, dtp->nK );
    aObj[ o_dtsd_g ].SetPtr( dtp->sd_g );
    aObj[ o_dtsd_g ].SetDim( 1, dtp->nK );
    aObj[ o_dtmuo_n ].SetPtr( dtp->muo_n );
    aObj[ o_dtmuo_n ].SetDim( dtp->nQ, dtp->nK );
    aObj[ o_dtavg_m ].SetPtr( dtp->avg_m );
    aObj[ o_dtavg_m ].SetDim( 1, dtp->nK );
    aObj[ o_dtsd_m ].SetPtr( dtp->sd_m );
    aObj[ o_dtsd_m ].SetDim( 1, dtp->nK );
    aObj[ o_dtqpn ].SetPtr( dtp->qpn );
    aObj[ o_dtqpn ].SetDim( dtp->nK, dtp->Nqpn );
    aObj[ o_dtqpg ].SetPtr( dtp->qpg );
    aObj[ o_dtqpg ].SetDim( dtp->nK, dtp->Nqpg );
    aObj[ o_dtcib ].SetPtr( dtp->CIb );
    aObj[ o_dtcib ].SetDim( dtp->nQ, dtp->Nb );
    aObj[ o_dtcin ].SetPtr( dtp->CIn );
    aObj[ o_dtcin ].SetDim( dtp->nQ, dtp->Nb );
    aObj[ o_dtcab ].SetPtr( dtp->CAb );
    aObj[ o_dtcab ].SetDim( dtp->nQ, dtp->La_b );
    aObj[ o_dtcan ].SetPtr( dtp->CAn );
    aObj[ o_dtcan ].SetDim( dtp->nQ, dtp->La_b );
    aObj[ o_dtcexpr].SetPtr( dtp->cExpr );
    // aObj[ o_dtcexpr].SetDim( 1, VIZmsize(dtp->cExpr));
    aObj[ o_dtgexpr].SetPtr( dtp->gExpr );
    // aObj[ o_dtgexpr].SetDim( 1, VIZmsize(dtp->gExpr));
    aObj[ o_dtsdref].SetPtr(dtp->sdref);
    aObj[ o_dtsdref].SetDim( dtp->Nsd, 1 );
    aObj[ o_dtsdval].SetPtr(dtp->sdval);
    aObj[ o_dtsdval].SetDim( dtp->Nsd, 1 );
    aObj[ o_dtnam_b].SetPtr(dtp->nam_b);
    aObj[ o_dtnam_b].SetDim( dtp->nQ, 1 );
    aObj[ o_dtnam_n].SetPtr(dtp->nam_n);
    aObj[ o_dtnam_n].SetDim( dtp->nK, 1 );
    aObj[ o_dtnam_nr].SetPtr(dtp->nam_n);
    aObj[ o_dtnam_nr].SetDim( 1, dtp->nK );
    aObj[ o_dtfor_n].SetPtr(dtp->for_n);
    aObj[ o_dtfor_n].SetDim( 1, dtp->nK );
    aObj[ o_dtres].SetPtr(dtp->for_n);
    aObj[ o_dtres].SetDim(  dtp->nK, 1 );
    aObj[ o_dtfor_b].SetPtr(dtp->for_b);
    aObj[ o_dtfor_b].SetDim( 1, dtp->La_b );
    aObj[ o_dtstl].SetPtr(dtp->stld);
    aObj[ o_dtstl].SetDim( dtp->nQ, 1 );
    aObj[ o_dttyp_n].SetPtr(dtp->typ_n);
    aObj[ o_dttyp_n].SetDim( dtp->nK, 1 );
    aObj[ o_dtciclb].SetPtr(dtp->CIclb);
    aObj[ o_dtciclb].SetDim( 1, dtp->Nb );
    aObj[ o_dtcicln].SetPtr(dtp->CIcln);
    aObj[ o_dtcicln].SetDim( 1, dtp->Nb );
    aObj[ o_dtauclb].SetPtr(dtp->AUclb);
    aObj[ o_dtauclb].SetDim( 1, dtp->La_b );
    aObj[ o_dtaucln].SetPtr(dtp->AUcln);
    aObj[ o_dtaucln].SetDim( 1, dtp->La_b );
    aObj[ o_dtsbm].SetPtr(dtp->SBM);
    aObj[ o_dtsbm].SetDim( 1, dtp->Nb );

    aObj[ o_dtmuo_i].SetPtr(dtp->muo_i);
    aObj[ o_dtmuo_i].SetDim( dtp->nQ, dtp->nK);
    aObj[ o_dtact_n].SetPtr(dtp->act_n);
    aObj[ o_dtact_n].SetDim( dtp->nQ, dtp->nK );
    aObj[ o_dttdq].SetPtr(dtp->Tdq);
    aObj[ o_dttdq].SetDim( 1, dtp->nQ );
    aObj[ o_dtpdq].SetPtr(dtp->Pdq);
    aObj[ o_dtpdq].SetDim( 1, dtp->nQ );
    aObj[ o_dtisq].SetPtr(dtp->ISq);
    aObj[ o_dtisq].SetDim( 1, dtp->nQ );

    aObj[ o_dtan].SetPtr(dtp->An);
    aObj[ o_dtan].SetDim( dtp->Asiz, dtp->Nb );
    aObj[ o_dttprn].SetPtr(  dtp->tprn );
    //aObj[ o_dttprn].SetDim( 1, VIZmsize(dtp->tprn ));
}


// set dynamic Objects ptr to values
void TDualTh::dyn_set(int q)
{
    ErrorIf( dtp!=&dt[q], GetName(),
      "E06DTrem: Illegal access to dt in dyn_set");
// dynamic
    dtp->Bb = (double *)aObj[ o_dtbb ].GetPtr();
    dtp->Bn = (double *)aObj[ o_dtbn ].GetPtr();
    dtp->Ub = (double *)aObj[ o_dtub ].GetPtr();
    dtp->chi = (double *)aObj[ o_dtchi ].GetPtr();
    dtp->mu_n = (double *)aObj[ o_dtmu_n ].GetPtr();
    dtp->Coul = (double *)aObj[ o_dtcoul ].GetPtr();
    dtp->gam_n = (double *)aObj[ o_dtgam_n ].GetPtr();
    dtp->avg_g = (double *)aObj[ o_dtavg_g ].GetPtr();
    dtp->sd_g = (double *)aObj[ o_dtsd_g ].GetPtr();
    dtp->muo_n = (double *)aObj[ o_dtmuo_n ].GetPtr();
    dtp->avg_m = (double *)aObj[ o_dtavg_m ].GetPtr();
    dtp->sd_m = (double *)aObj[ o_dtsd_m ].GetPtr();
    dtp->qpn = (double *)aObj[ o_dtqpn ].GetPtr();
    dtp->qpg = (double *)aObj[ o_dtqpg ].GetPtr();
    dtp->CIb = (float *)aObj[ o_dtcib ].GetPtr();
    dtp->CIn = (float *)aObj[ o_dtcin ].GetPtr();
    dtp->CAb = (float *)aObj[ o_dtcab ].GetPtr();
    dtp->CAn = (float *)aObj[ o_dtcan ].GetPtr();
    dtp->cExpr = (char *)aObj[ o_dtcexpr ].GetPtr();
    dtp->gExpr = (char *)aObj[ o_dtgexpr ].GetPtr();
    dtp->sdref = (char (*)[V_SD_RKLEN])aObj[ o_dtsdref ].GetPtr();
    dtp->sdval = (char (*)[V_SD_VALEN])aObj[ o_dtsdval ].GetPtr();
    dtp->nam_b = (char (*)[MAXIDNAME])aObj[ o_dtnam_b ].GetPtr();
    dtp->nam_n = (char (*)[MAXIDNAME])aObj[ o_dtnam_n ].GetPtr();
    aObj[ o_dtnam_nr].SetPtr(dtp->nam_n);
    aObj[ o_dtnam_nr].SetDim( 1, dtp->nK );
    dtp->for_n = (char (*)[MAXFORMUNITDT])aObj[ o_dtfor_n ].GetPtr();
    aObj[ o_dtres].SetPtr(dtp->for_n);
    aObj[ o_dtres].SetDim(  dtp->nK, 1 );

    dtp->for_b = (char (*)[MAXFORMUNITDT])aObj[ o_dtfor_b ].GetPtr();
    dtp->stld = (char (*)[EQ_RKLEN])aObj[ o_dtstl ].GetPtr();
    dtp->typ_n = (char *)aObj[ o_dttyp_n ].GetPtr();
    dtp->CIclb = (char *)aObj[ o_dtciclb ].GetPtr();
    dtp->CIcln = (char *)aObj[ o_dtcicln ].GetPtr();
    dtp->AUclb = (char *)aObj[ o_dtauclb ].GetPtr();
    dtp->AUcln = (char *)aObj[ o_dtaucln ].GetPtr();
    dtp->SBM = (char (*)[MAXICNAME+MAXSYMB])aObj[ o_dtsbm ].GetPtr();

    dtp->muo_i = (double *)aObj[ o_dtmuo_i ].GetPtr();
    dtp->act_n = (double *)aObj[ o_dtact_n ].GetPtr();
    dtp->Tdq = (float *)aObj[ o_dttdq ].GetPtr();
    dtp->Pdq = (float *)aObj[ o_dtpdq ].GetPtr();
    dtp->ISq = (float *)aObj[ o_dtisq ].GetPtr();

    dtp->An = (float *)aObj[ o_dtan ].GetPtr();
    dtp->tprn = (char *)aObj[ o_dttprn ].GetPtr();
}


// free dynamic memory in objects and values
void TDualTh::dyn_kill(int q)
{
    ErrorIf( dtp!=&dt[q], GetName(),
       "E05DTrem: Illegal access to dt in dyn_kill");
// dynamic
    dtp->Bb = (double *)aObj[ o_dtbb ].Free();
    dtp->Bn = (double *)aObj[ o_dtbn ].Free();
    dtp->Ub = (double *)aObj[ o_dtub ].Free();
    dtp->chi = (double *)aObj[ o_dtchi ].Free();
    dtp->mu_n = (double *)aObj[ o_dtmu_n ].Free();
    dtp->Coul = (double *)aObj[ o_dtcoul ].Free();
    dtp->gam_n = (double *)aObj[ o_dtgam_n ].Free();
    dtp->avg_g = (double *)aObj[ o_dtavg_g ].Free();
    dtp->sd_g = (double *)aObj[ o_dtsd_g ].Free();
    dtp->muo_n = (double *)aObj[ o_dtmuo_n ].Free();
    dtp->avg_m = (double *)aObj[ o_dtavg_m ].Free();
    dtp->sd_m = (double *)aObj[ o_dtsd_m ].Free();
    dtp->qpn = (double *)aObj[ o_dtqpn ].Free();
    dtp->qpg = (double *)aObj[ o_dtqpg ].Free();
    dtp->CIb = (float *)aObj[ o_dtcib ].Free();
    dtp->CIn = (float *)aObj[ o_dtcin ].Free();
    dtp->CAb = (float *)aObj[ o_dtcab ].Free();
    dtp->CAn = (float *)aObj[ o_dtcan ].Free();
    dtp->cExpr = (char *)aObj[ o_dtcexpr ].Free();
    dtp->gExpr = (char *)aObj[ o_dtgexpr ].Free();
    dtp->sdref = (char (*)[V_SD_RKLEN])aObj[ o_dtsdref ].Free();
    dtp->sdval = (char (*)[V_SD_VALEN])aObj[ o_dtsdval ].Free();
    dtp->nam_b = (char (*)[MAXIDNAME])aObj[ o_dtnam_b ].Free();
    dtp->nam_n = (char (*)[MAXIDNAME])aObj[ o_dtnam_n ].Free();
    aObj[ o_dtnam_nr].SetPtr(0);
    //aObj[ o_dtnam_nr].SetDim( 1, 0 );
    dtp->for_n = (char (*)[MAXFORMUNITDT])aObj[ o_dtfor_n ].Free();
    aObj[ o_dtres].SetPtr(0);
//    aObj[ o_dtres].SetDim(  dtp->nK, 1 );

    dtp->for_b = (char (*)[MAXFORMUNITDT])aObj[ o_dtfor_b ].Free();
    dtp->stld = (char (*)[EQ_RKLEN])aObj[ o_dtstl ].Free();
    dtp->typ_n = (char *)aObj[ o_dttyp_n ].Free();
    dtp->CIclb = (char *)aObj[ o_dtciclb ].Free();
    dtp->CIcln = (char *)aObj[ o_dtcicln ].Free();
    dtp->AUclb = (char *)aObj[ o_dtauclb ].Free();
    dtp->AUcln = (char *)aObj[ o_dtaucln ].Free();
    dtp->SBM = (char (*)[MAXICNAME+MAXSYMB])aObj[ o_dtsbm ].Free();

    dtp->muo_i = (double *)aObj[ o_dtmuo_i ].Free();
    dtp->act_n = (double *)aObj[ o_dtact_n ].Free();
    dtp->Tdq = (float *)aObj[ o_dttdq ].Free();
    dtp->Pdq = (float *)aObj[ o_dtpdq ].Free();
    dtp->ISq = (float *)aObj[ o_dtisq ].Free();

    dtp->An = (float *)aObj[ o_dtan ].Free();
    dtp->tprn = (char *)aObj[ o_dttprn ].Free();
}


// realloc dynamic memory
void TDualTh::dyn_new(int q)
{
    ErrorIf( dtp!=&dt[q], GetName(),
      "E04DTrem: Illegal access to dt in dyn_new.");

// dynamic
    dtp->Bb = (double *)aObj[ o_dtbb ].Alloc( dtp->nQ, dtp->Nb, D_ );
    dtp->Bn = (double *)aObj[ o_dtbn ].Alloc( dtp->nQ, dtp->Nb, D_ );
    dtp->Ub = (double *)aObj[ o_dtub ].Alloc( dtp->nQ, dtp->Nb, D_ );
    dtp->chi = (double *)aObj[ o_dtchi ].Alloc( dtp->nQ, dtp->nK, D_ );
    dtp->mu_n = (double *)aObj[ o_dtmu_n ].Alloc( dtp->nQ, dtp->nK, D_ );

    if( dtp->PvCoul == S_ON )
      dtp->Coul = (double *)aObj[ o_dtcoul ].Alloc( dtp->nQ, dtp->nK, D_ );
    else
      dtp->Coul = (double *)aObj[ o_dtcoul ].Free();

    dtp->gam_n = (double *)aObj[ o_dtgam_n ].Alloc( dtp->nQ, dtp->nK, D_ );
    dtp->avg_g = (double *)aObj[ o_dtavg_g ].Alloc( 1, dtp->nK,  D_ );
    dtp->sd_g = (double *)aObj[ o_dtsd_g ].Alloc( 1, dtp->nK, D_ );
    dtp->muo_n = (double *)aObj[ o_dtmuo_n ].Alloc( dtp->nQ, dtp->nK, D_ );
    dtp->avg_m = (double *)aObj[ o_dtavg_m ].Alloc( 1, dtp->nK, D_ );
    dtp->sd_m = (double *)aObj[ o_dtsd_m ].Alloc( 1, dtp->nK,  D_ );

    if( dtp->Nqpn>0 )
      dtp->qpn = (double *)aObj[ o_dtqpn ].Alloc( dtp->nK, dtp->Nqpn,  D_ );
    else
      dtp->qpn = (double *)aObj[ o_dtqpn ].Free();
    if( dtp->Nqpg>0 )
      dtp->qpg = (double *)aObj[ o_dtqpg ].Alloc( dtp->nK, dtp->Nqpg, D_ );
    else
      dtp->qpg = (double *)aObj[ o_dtqpg ].Free();

    if( dtp->PvICb == S_ON )
    {
      dtp->CIclb = (char *)aObj[ o_dtciclb ].Alloc( 1, dtp->Nb, A_ );
      dtp->CIb = (float *)aObj[ o_dtcib ].Alloc( dtp->nQ, dtp->Nb, F_ );
    }
    else
    {
      dtp->CIb = (float *)aObj[ o_dtcib ].Free();
      dtp->CIclb = (char *)aObj[ o_dtciclb ].Free();
    }
    if( dtp->PvICn == S_ON )
    {
      dtp->CIn = (float *)aObj[ o_dtcin ].Alloc( dtp->nQ, dtp->Nb, F_ );
      dtp->CIcln = (char *)aObj[ o_dtcicln ].Alloc( 1, dtp->Nb,  A_ );
    }
    else
    {
      dtp->CIn = (float *)aObj[ o_dtcin ].Free();
      dtp->CIcln = (char *)aObj[ o_dtcicln ].Free();
    }
    if( dtp->PvAUb == S_ON )
    {
      dtp->CAb = (float *)aObj[ o_dtcab ].Alloc( dtp->nQ, dtp->La_b, F_ );
      dtp->AUclb = (char *)aObj[ o_dtauclb ].Alloc( 1, dtp->La_b, A_ );
      dtp->CAn = (float *)aObj[ o_dtcan ].Alloc( dtp->nQ, dtp->La_b, F_ );
      dtp->AUcln = (char *)aObj[ o_dtaucln ].Alloc( 1, dtp->La_b, A_ );
    }
    else
    {
      dtp->CAb = (float *)aObj[ o_dtcab ].Free();
      dtp->AUclb = (char *)aObj[ o_dtauclb ].Free();
      dtp->CAn = (float *)aObj[ o_dtcan ].Free();
      dtp->AUcln = (char *)aObj[ o_dtaucln ].Free();
    }

    if( dtp->PvChi == S_ON )
        dtp->cExpr = (char *)aObj[ o_dtcexpr].Alloc( 1, 2048, S_);
    else
        dtp->cExpr = (char *)aObj[ o_dtcexpr ].Free();
    if( dtp->PvGam == S_ON )
        dtp->gExpr = (char *)aObj[ o_dtgexpr].Alloc( 1, 2048, S_);
    else
        dtp->gExpr = (char *)aObj[ o_dtgexpr ].Free();

    if( dtp->PvGam == S_ON || dtp->PvChi == S_ON )
        dtp->tprn = (char *)aObj[ o_dttprn].Alloc( 1, 2048, S_);
    else
        dtp->tprn = (char *)aObj[ o_dttprn ].Free();

    if( dtp->Nsd > 0 )
    {
        dtp->sdref =
            (char (*)[V_SD_RKLEN])aObj[ o_dtsdref].Alloc( dtp->Nsd, 1, V_SD_RKLEN );
        dtp->sdval =
            (char (*)[V_SD_VALEN])aObj[ o_dtsdval].Alloc( dtp->Nsd, 1, V_SD_VALEN );
    }
    else
    {
        dtp->sdref = (char (*)[V_SD_RKLEN])aObj[ o_dtsdref ].Free();
        dtp->sdval = (char (*)[V_SD_VALEN])aObj[ o_dtsdval ].Free();
    }
   dtp->nam_b =
       (char (*)[MAXIDNAME])aObj[ o_dtnam_b].Alloc( dtp->nQ, 1, MAXIDNAME );
   dtp->nam_n =
       (char (*)[MAXIDNAME])aObj[ o_dtnam_n].Alloc( dtp->nK, 1, MAXIDNAME );
   aObj[ o_dtnam_nr].SetPtr(dtp->nam_n);
   aObj[ o_dtnam_nr].SetDim( 1, dtp->nK );

   dtp->for_n = (char (*)[MAXFORMUNITDT])aObj[ o_dtfor_n].Alloc(
                                         1,  dtp->nK, MAXFORMUNITDT );
   aObj[ o_dtres].SetPtr(dtp->for_n);
   aObj[ o_dtres].SetDim(  dtp->nK, 1 );

   if( dtp->La_b > 0 )
     dtp->for_b = (char (*)[MAXFORMUNITDT])aObj[ o_dtfor_b].Alloc(
                                             1, dtp->La_b, MAXFORMUNITDT );
   else
     dtp->for_b = (char (*)[MAXFORMUNITDT])aObj[ o_dtfor_b ].Free();

   dtp->stld = (char (*)[EQ_RKLEN])aObj[ o_dtstl].Alloc( dtp->nQ, 1, EQ_RKLEN );
   dtp->typ_n = (char *)aObj[ o_dttyp_n ].Alloc( dtp->nK, 1, A_ );
   dtp->SBM = (char (*)[MAXICNAME+MAXSYMB])aObj[ o_dtsbm].Alloc(
                                     1, dtp->Nb, MAXICNAME+MAXSYMB );

    dtp->muo_i = (double *)aObj[ o_dtmuo_i ].Alloc( dtp->nQ, dtp->nK, D_ );
    dtp->act_n = (double *)aObj[ o_dtact_n ].Alloc( dtp->nQ, dtp->nK, D_ );
    dtp->Tdq = (float *)aObj[ o_dttdq ].Alloc( dtp->nQ, 1, F_ );
    dtp->Pdq = (float *)aObj[ o_dtpdq ].Alloc( dtp->nQ, 1, F_ );
    dtp->ISq = (float *)aObj[ o_dtisq ].Alloc( dtp->nQ, 1, F_ );

   dtp->An = (float *)aObj[ o_dtan ].Alloc( dtp->nK, dtp->Nb, F_ );
   dtp->Asiz = dtp->nK;
}


//set default information
void TDualTh::set_def( int q)
{
    ErrorIf( dtp!=&dt[q], GetName(),
        "E03DTrem: Illegal access to dt in set_def");

    TProfil *aPa= TProfil::pm;

    memcpy( &dtp->PunE, aPa->pa.TPpdc, 4 );
    memcpy( &dtp->PvCoul, aPa->pa.UTppc, 10 );
    memcpy( &dtp->gStat, "00+---", 6 );
    strcpy( dtp->name, "`" );
    strcpy( dtp->notes, "`" );

    memset( &dtp->nQ, 0, sizeof(short)*8 );
    memset( &dtp->Msysb, 0, sizeof(float)*12 );
    memset( dtp->sykey, 0, sizeof(char)*(EQ_RKLEN+10) );
    memset( &dtp->q, 0, sizeof(short)*5 );

    dtp->tmd[START_] = 1000;
    dtp->tmd[STOP_] =  1200;
    dtp->tmd[STEP_] =  10;
    dtp->NVd[START_] = 0;
    dtp->NVd[STOP_] =  0;
    dtp->NVd[STEP_] =  0;

    dtp->Vd[START_] = 0.;
    dtp->Vd[STOP_] = 0.;
    dtp->Vd[STEP_] = 0.;
    dtp->Pd[START_] = 1.;
    dtp->Pd[STOP_] = 1.;
    dtp->Pd[STEP_] = 0;
    dtp->Td[START_] = 25.;
    dtp->Td[STOP_] = 25.;
    dtp->Td[STEP_] = 0;
// dynamic
    dtp->Bb = 0;
    dtp->Bn = 0;
    dtp->Ub = 0;
    dtp->chi = 0;
    dtp->mu_n = 0;
    dtp->Coul = 0;
    dtp->gam_n = 0;
    dtp->avg_g = 0;
    dtp->sd_g = 0;
    dtp->muo_n = 0;
    dtp->avg_m = 0;
    dtp->sd_m = 0;
    dtp->qpn = 0;
    dtp->qpg = 0;
    dtp->CIb = 0;
    dtp->CIn = 0;
    dtp->CAb = 0;
    dtp->CAn = 0;
    dtp->cExpr = 0;
    dtp->gExpr = 0;
    dtp->sdref = 0;
    dtp->sdval = 0;
    dtp->nam_b = 0;
    dtp->nam_n = 0;
    dtp->for_n = 0;
    dtp->for_b = 0;
    dtp->stld = 0;
    dtp->typ_n = 0;
    dtp->CIclb = 0;
    dtp->CIcln = 0;
    dtp->AUclb = 0;
    dtp->AUcln = 0;
    dtp->SBM = 0;

    dtp->muo_i = 0;
    dtp->act_n = 0;
    dtp->Tdq = 0;
    dtp->Pdq = 0;
    dtp->ISq = 0;

    dtp->An = 0;
    dtp->tprn = 0;

}

// return true if nesessary, recalc base SYSEQ
bool TDualTh::check_input( const char * /*key*/, int /*Level*/ )
{
    vstr pkey(MAXRKEYLEN+10);
    if( pVisor->ProfileMode != true )
        return true;

    //Get base SysEq key from process key
    rt[RT_SYSEQ].MakeKey( RT_DUALTH, pkey, RT_DUALTH, 0, RT_DUALTH, 1,
                           RT_DUALTH, 2, RT_DUALTH, 3, RT_DUALTH, 4,
                           RT_DUALTH, 5, RT_DUALTH, 6, RT_DUALTH, 7, K_END);
    // read SysEq record and unpack data
    TProfil::pm->loadSystat( pkey );
    // test changes in system after process calc
    if( rt[RT_SYSEQ].Rtime() > rt[nRT].Rtime() )
        return true;
    return false;
}


// Input necessary data and links objects
void TDualTh::RecInput( const char *key )
{
    TCModule::RecInput( key );
}

//Rebuild record structure before calc
int
TDualTh::RecBuild( const char *key, int mode  )
{

 if( pVisor->ProfileMode != true )
        Error( GetName(), "E09DTrem: Please, do it in the Project mode!" );
 bool set_def = false;

AGAIN:

    int ret = TCModule::RecBuild( key, mode );
    if( ret == VF_CANCEL )
        return ret;

   if( ret == VF3_1 )  // clear all
     set_def = true;

    if( !test_sizes() )
    {  mode = VF_REMAKE;
      goto AGAIN;
    }

   dyn_new();

   dt_initiate( set_def );
   // dtp->gStat = UNSP_GS_INDEF;
     // set data to SBM
   for(int ii=0; ii< dtp->Nb; ii++ )
        memcpy( dtp->SBM[ii], TProfil::pm->mup->SB[ii], MAXICNAME  );

   pVisor->Update();
   return ret;
}

//Recalc record structure
void
TDualTh::RecCalc( const char *key )
{
    if( pVisor->ProfileMode != true )
        Error( GetName(), "E02DTexec: Please, do it in the Project mode!" );


//   pVisor->Message( window(), GetName(),
//             "Generation of EqStat records\n"
//                 "Please, wait...", 0, usp->nQ);

// generate part
   if( dtp->gStat != UNSP_GS_DONE )
   {
    dtp->gStat = UNSP_GS_GOIN;
    Init_Generation();
    build_Ub();
    dtp->gStat = UNSP_GS_DONE;
   }

//analyse part
  Init_Analyse();
  Analyse();
  dtp->aStat = UNSP_AS_DONE;

  pVisor->CloseMessage();
  TCModule::RecCalc(key);
}

void
TDualTh::CmHelp()
{
    pVisor->OpenHelp( GEMS_DT_HTML );
}

// insert changes in Project to TDualTh
void TDualTh::InsertChanges( TIArray<CompItem>& aIComp )
{
    // insert changes to IComp
    if(aIComp.GetCount()<1 || dtp->nQ < 1)
        return;

   // alloc memory & copy data from db

    int j, i=0, ii=0, jj =0;
    int Nold = dtp->Nb;
    double *p_Bb = new double[dtp->nQ*dtp->Nb];
        memcpy( p_Bb, dtp->Bb, dtp->nQ*dtp->Nb*sizeof(double));
    double *p_Bn = new double[dtp->nQ*dtp->Nb];
       memcpy( p_Bn, dtp->Bn, dtp->nQ*dtp->Nb*sizeof(double));
    double *p_Ub = new double[dtp->nQ*dtp->Nb];
       memcpy( p_Ub, dtp->Ub, dtp->nQ*dtp->Nb*sizeof(double));

    char *p_CIclb;
    float *p_CIb;
    char *p_CIcln;
    float *p_CIn;

   if( dtp->PvICb == S_ON )
   {
    p_CIclb = new char[dtp->Nb];
       memcpy( p_CIclb, dtp->CIclb, dtp->Nb*sizeof(char));
    p_CIb = new float[dtp->nQ*dtp->Nb];
       memcpy( p_CIb, dtp->CIb, dtp->nQ*dtp->Nb*sizeof(float));
    }
   if( dtp->PvICn == S_ON )
   {
    p_CIcln = new char[dtp->Nb];
       memcpy( p_CIcln, dtp->CIcln, dtp->Nb*sizeof(char));
    p_CIn = new float[dtp->nQ*dtp->Nb];
       memcpy( p_CIn, dtp->CIn, dtp->nQ*dtp->Nb*sizeof(float));
   }
   float *p_An = new float[dtp->nK*dtp->Nb];
    if( dtp->An )
      memcpy( p_An, dtp->An, dtp->nK*dtp->Nb*sizeof(float));

    // alloc new memory
     dtp->Nb = TProfil::pm->mup->N;
     dyn_new();

    for(int ii=0; ii< dtp->Nb; ii++ )
      memcpy( dtp->SBM[ii], TProfil::pm->mup->SB[ii], MAXICNAME  );

    while( jj < dtp->Nb )
    {
      if( i < aIComp.GetCount() &&  aIComp[i].line == ii )
      {
        if( aIComp[i].delta == 1 )
        { // add line
          for( j =0; j<dtp->nQ; j++ )
          {
            dtp->Bb[j*dtp->Nb+jj] = 0.;
            dtp->Bn[j*dtp->Nb+jj] = 0.;
            dtp->Ub[j*dtp->Nb+jj] = 0.;
            if( dtp->PvICb == S_ON )
            {
              dtp->CIb[j*dtp->Nb+jj] = 0.;
              dtp->CIclb[j*dtp->Nb+jj] = QUAN_GRAM;
            }
            if( dtp->PvICn == S_ON )
            {
              dtp->CIn[j*dtp->Nb+jj] = 0.;
              dtp->CIcln[j*dtp->Nb+jj] = QUAN_GRAM;
             }
          }
          for( j =0; j<dtp->nK; j++ )
            dtp->An[j*dtp->Nb+jj] = 0.;

          jj++;
         }
          else
           { // delete line
             ii++;
           }
        i++;
       }
       else
       {  // copy line
         if( ii < Nold )
         {
             for( j =0; j<dtp->nQ; j++ )
             {
               dtp->Bb[j*dtp->Nb+jj] = p_Bb[j*Nold+ii];
               dtp->Bn[j*dtp->Nb+jj] = p_Bn[j*Nold+ii];
               dtp->Ub[j*dtp->Nb+jj] = p_Ub[j*Nold+ii];
               if( dtp->PvICb == S_ON )
               {
                 dtp->CIb[j*dtp->Nb+jj] = p_CIb[j*Nold+ii];
                 dtp->CIclb[j*dtp->Nb+jj] = p_CIclb[j*Nold+ii];
                }
               if( dtp->PvICn == S_ON )
               {
                 dtp->CIn[j*dtp->Nb+jj] = p_CIn[j*Nold+ii];
                 dtp->CIcln[j*dtp->Nb+jj] = p_CIcln[j*Nold+ii];
               }
             }
             for( j =0; j<dtp->nK; j++ )
                dtp->An[j*dtp->Nb+jj] = p_An[j*Nold+ii];
          }
        jj++;
        ii++;
       }
    }
// free memory
   delete[] p_Bb;
   delete[] p_Bn;
   delete[] p_Ub;
   if( dtp->PvICb == S_ON )
   {
     delete[] p_CIclb;
     delete[] p_CIb;
    }
   if( dtp->PvICn == S_ON )
   { delete[] p_CIcln;
     delete[] p_CIn;
   }
   delete[] p_An;
}


// ------------------- End of m_dualth.cpp --------------------------
