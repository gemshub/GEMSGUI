//-------------------------------------------------------------------
// $Id: m_dualth.cpp 1121 2008-11-25 10:16:38Z gems $
//
// Implementation of TDualTh class, config and calculation functions
//
// Rewritten from C to C++ by S.Dmytriyeva
// Copyright (C) 2000-2005 S.Dmytriyeva, D.Kulik
//
// This file is part of a GEM-Selektor library for thermodynamic
// modelling by Gibbs energy minimization
// Uses: GEM-Selektor GUI GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the GPL v.3 license

//
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//

#include <cmath>

#include "m_dualth.h"
#include "m_syseq.h"
#include "visor.h"

TDualTh* TDualTh::pm;

TDualTh::TDualTh( uint nrt ):
        TCModule( nrt )
{
    nQ = 1;
    aFldKeysHelp.push_back("Name of the modeling project");
    aFldKeysHelp.push_back("Thermodynamic potential to minimize {G}");
    aFldKeysHelp.push_back("Name of the parent chemical system definition (CSD)");
    aFldKeysHelp.push_back("CSD (recipe) variant number <integer>");
    aFldKeysHelp.push_back("Volume of the system, dm3");
    aFldKeysHelp.push_back("Pressure, bar, or 0 for Psat(H2O)g");
    aFldKeysHelp.push_back("Temperature, C");
    aFldKeysHelp.push_back("Variant number for additional constraints");
    aFldKeysHelp.push_back("Identifier of this DualThermo calculation task");
    aFldKeysHelp.push_back("DualThermo task code: one of { E P S } and one of { M G A X } ");
    setKeyEditField(8);
    dtp=&dt[0];
    set_def();
    start_title =
       " Definition of a DualTh Simulator (batch calculation)";

}

// get key of record
std::string
TDualTh::GetKeyofRecord( const char *oldKey, const char *strTitle,
                          int keyType )
{
    std::string str;

    if( oldKey == 0 )
    {
        if(Filter.empty())
            str = ALLKEY;
        else str = Filter;
    }
    else str = oldKey;

    if( keyType==KEY_NEW  )
    { // Get key of base SyStat
        char pkey[MAXRKEYLEN+10];
        rt[RT_DUALTH]->SetKey(str.c_str());
        rt[RT_SYSEQ]->MakeKey( RT_DUALTH, pkey, RT_DUALTH, 0, RT_DUALTH, 1,
                               RT_DUALTH, 2, RT_DUALTH, 3, RT_DUALTH, 4,
                               RT_DUALTH, 5, RT_DUALTH, 6, RT_DUALTH, 7, K_END);
        str = TSysEq::pm->GetKeyofRecord( pkey,
          "Please, select a parent System for a new DualTh ", KEY_OLD );
        if(  str.empty() )
        {
            str = pkey;
            str+= "*:*:";
        }
        else
            str += "*:*:";
    }
    str = TCModule::GetKeyofRecord( str.c_str(), strTitle, keyType );
    if(  str.empty() )
        return str;
    rt[RT_DUALTH]->SetKey(str.c_str());
    if( keyType != KEY_TEMP )
    keyTest( str.c_str() );
    return str;
}

// test DualTh key to calc mode
void TDualTh::keyTest( const char *key )
{
    char pkey[MAXRKEYLEN+10];

    if( pVisor->ProfileMode )
    { // test project key
        std::string prfKey = TProfil::pm->projectName();
        size_t k = prfKey.length();
        if( memcmp(key, prfKey.c_str(), k ) ||
                ( key[k] != ':' && key[k] != ' ' && k<rt[RT_PARAM]->FldLen(0) )  )
            Error( key, "E08PErem: Invalid record key (another Modelling Project)!");
        rt[RT_SYSEQ]->MakeKey( RT_DUALTH, pkey, RT_DUALTH, 0, RT_DUALTH, 1,
                               RT_DUALTH, 2, RT_DUALTH, 3, RT_DUALTH, 4,
                               RT_DUALTH, 5, RT_DUALTH, 6, RT_DUALTH, 7, K_END);
        if( rt[RT_SYSEQ]->Find(pkey) <0 )
            Error( key, "E07PErem: Invalid record key (no system)!");
    }
}


// link values to objects
void TDualTh::ods_link( int q)
{
    dtp=&dt[q];

// static
    aObj[ o_dtpvfl ]->SetPtr(  &dtp->PunE );    /*a 12*/
    aObj[ o_dtpsflg ]->SetPtr(  &dtp->PsMode ); /*a 8*/
    aObj[ o_dtdim ]->SetPtr(  &dtp->nQ );    /*i 8*/
    aObj[ o_dttmd ]->SetPtr(  dtp->tmd );    /*i 3*/
    aObj[ o_dtnvd ]->SetPtr(  dtp->NVd );    /*i 3*/
    aObj[ o_dtpd ]->SetPtr(  dtp->Pd );    /*f 3*/
    aObj[ o_dttd ]->SetPtr(  dtp->Td );    /*f 3*/
    aObj[ o_dtvd ]->SetPtr(  dtp->Vd );    /*f 3*/
    aObj[ o_dtmsysb ]->SetPtr(  &dtp->Msysb );    /*f 8*/
    aObj[ o_dtt ]->SetPtr(  &dtp->cT );
    aObj[ o_dtp ]->SetPtr(  &dtp->cP );
    aObj[ o_dtv ]->SetPtr(  &dtp->cV );
    aObj[ o_dtq ]->SetPtr(  &dtp->q );
    aObj[ o_dkp ]->SetPtr(  &dtp->kp );
    aObj[ o_dti ]->SetPtr(  &dtp->i );
    aObj[ o_dtjm ]->SetPtr(  &dtp->jm );
    aObj[ o_dtc_tm ]->SetPtr(  &dtp->c_tm );
    aObj[ o_dtc_nv ]->SetPtr(  &dtp->c_NV );
    aObj[ o_dtname ]->SetPtr(  dtp->name );
    aObj[ o_dtnotes ]->SetPtr(  dtp->notes );
    aObj[ o_dtpspv ]->SetPtr(  &dtp->PunE );    /*a 20*/
    aObj[ o_dtshort ]->SetPtr(  &dtp->nQ );    /*i 14*/
    aObj[ o_dtfloat ]->SetPtr(  &dtp->Pd );    /*f 22*/
    aObj[ o_dtstkey ]->SetPtr(  dtp->sykey );
//LM fitter data
    aObj[ o_lmft_s ]->SetPtr(  &dtp->n_par );/* i4 */
    aObj[ o_lxi2 ]->SetPtr(  &dtp->xi2 );    /* d2 */
// dynamic
    aObj[ o_dtbb ]->SetPtr( dtp->Bb );
    aObj[ o_dtbb ]->SetDim( dtp->nQ, dtp->Nb );
    aObj[ o_dtbn ]->SetPtr( dtp->Bn );
    aObj[ o_dtbn ]->SetDim( dtp->nQ, dtp->Nb );
    aObj[ o_dtub ]->SetPtr( dtp->Ub );
    aObj[ o_dtub ]->SetDim( dtp->nQ, dtp->Nb );
    aObj[ o_dtchi ]->SetPtr( dtp->chi );
    aObj[ o_dtchi ]->SetDim( dtp->nQ, dtp->nM );
    aObj[ o_dteta_b]->SetPtr(dtp->eta_b);
    aObj[ o_dteta_b]->SetDim( dtp->nQ, dtp->nM );
    aObj[ o_dtact_n]->SetPtr(dtp->act_n);
    aObj[ o_dtact_n]->SetDim( dtp->nQ, dtp->nM );
    aObj[ o_dtgam_n ]->SetPtr( dtp->gam_n );
    aObj[ o_dtgam_n ]->SetDim( dtp->nQ, dtp->nM );
    aObj[ o_dtcoul ]->SetPtr( dtp->Coul );
    aObj[ o_dtcoul ]->SetDim( dtp->nQ, dtp->nM );
    aObj[ o_dtmu_n ]->SetPtr( dtp->mu_b );
    aObj[ o_dtmu_n ]->SetDim( dtp->nQ, dtp->nM );
    aObj[ o_dtmu_o ]->SetPtr( dtp->mu_o );
    aObj[ o_dtmu_o ]->SetDim( dtp->nQ, dtp->nM );
    aObj[ o_dtavsd_a ]->SetPtr( dtp->avsd_a );
    aObj[ o_dtavsd_a ]->SetDim( 2, dtp->nM );
    aObj[ o_dtavsd_o ]->SetPtr( dtp->avsd_o );
    aObj[ o_dtavsd_o ]->SetDim( 2, dtp->nM );
    aObj[ o_dtyconst ]->SetPtr( dtp->yconst );
    aObj[ o_dtyconst ]->SetDim( dtp->nQ, 1 );
    aObj[ o_dtqpg ]->SetPtr( dtp->qpg );
    aObj[ o_dtqpg ]->SetDim( dtp->nM, dtp->Nqpg );
    aObj[ o_dtcib ]->SetPtr( dtp->CIb );
    aObj[ o_dtcib ]->SetDim( dtp->nQ, dtp->Nb );
    aObj[ o_dtcin ]->SetPtr( dtp->CIn );
    aObj[ o_dtcin ]->SetDim( dtp->nQ, dtp->Nb );
    aObj[ o_dtcab ]->SetPtr( dtp->CAb );
    aObj[ o_dtcab ]->SetDim( dtp->nQ, dtp->La_b );
    aObj[ o_dtcan ]->SetPtr( dtp->CAn );
    aObj[ o_dtcan ]->SetDim( dtp->nQ, dtp->La_b );
    aObj[ o_dtcexpr]->SetPtr( dtp->cExpr );
    // aObj[ o_dtcexpr]->SetDim( 1, VIZmsize(dtp->cExpr));
    aObj[ o_dtgexpr]->SetPtr( dtp->gExpr );
    // aObj[ o_dtgexpr]->SetDim( 1, VIZmsize(dtp->gExpr));
    aObj[ o_dtsdref]->SetPtr(dtp->sdref);
    aObj[ o_dtsdref]->SetDim( dtp->Nsd, 1 );
    aObj[ o_dtsdval]->SetPtr(dtp->sdval);
    aObj[ o_dtsdval]->SetDim( dtp->Nsd, 1 );
    aObj[ o_dtnam_b]->SetPtr(dtp->nam_b);
    aObj[ o_dtnam_b]->SetDim( dtp->nQ, 1 );
    aObj[ o_dtnam_n]->SetPtr(dtp->nam_n);
    aObj[ o_dtnam_n]->SetDim( dtp->nM, 1 );
    aObj[ o_dtpar_n]->SetPtr(dtp->par_n);
    aObj[ o_dtpar_n]->SetDim( 1, dtp->nP );
    aObj[ o_dtnam_nr]->SetPtr(dtp->nam_n);
    aObj[ o_dtnam_nr]->SetDim( 1, dtp->nM );
    aObj[ o_dtfor_n]->SetPtr(dtp->for_n);
    aObj[ o_dtfor_n]->SetDim( 1, dtp->nM );
    aObj[ o_dtres]->SetPtr(dtp->for_n);
    aObj[ o_dtres]->SetDim(  dtp->nM, 1 );
    aObj[ o_dtfor_b]->SetPtr(dtp->for_b);
    aObj[ o_dtfor_b]->SetDim( 1, dtp->La_b );
    aObj[ o_dtstl]->SetPtr(dtp->stld);
    aObj[ o_dtstl]->SetDim( dtp->nQ, 1 );
    aObj[ o_dttyp_n]->SetPtr(dtp->typ_n);
    aObj[ o_dttyp_n]->SetDim( dtp->nM, 1 );
    aObj[ o_dtciclb]->SetPtr(dtp->CIclb);
    aObj[ o_dtciclb]->SetDim( 1, dtp->Nb );
    aObj[ o_dtcicln]->SetPtr(dtp->CIcln);
    aObj[ o_dtcicln]->SetDim( 1, dtp->Nb );
    aObj[ o_dtauclb]->SetPtr(dtp->AUclb);
    aObj[ o_dtauclb]->SetDim( 1, dtp->La_b );
    aObj[ o_dtaucln]->SetPtr(dtp->AUcln);
    aObj[ o_dtaucln]->SetDim( 1, dtp->La_b );
    aObj[ o_dtsbm]->SetPtr(dtp->SBM);
    aObj[ o_dtsbm]->SetDim( 1, dtp->Nb );
    aObj[ o_dtmu_a]->SetPtr(dtp->mu_a);
    aObj[ o_dtmu_a]->SetDim( dtp->nQ, dtp->nM);
    aObj[ o_dttdq]->SetPtr(dtp->Tdq);
    aObj[ o_dttdq]->SetDim( 1, dtp->nQ );
    aObj[ o_dtpdq]->SetPtr(dtp->Pdq);
    aObj[ o_dtpdq]->SetDim( 1, dtp->nQ );
    aObj[ o_dtisq]->SetPtr(dtp->ISq);
    aObj[ o_dtisq]->SetDim( 1, dtp->nQ );
    aObj[ o_dtwacp]->SetPtr(dtp->wa_cp);
    aObj[ o_dtwacp]->SetDim( 1, dtp->nP );
    aObj[ o_gmx_n ]->SetPtr( dtp->gmx_n );
    aObj[ o_gmx_n ]->SetDim( dtp->nQ, 5 );
    aObj[ o_dtwa ]->SetPtr( dtp->Wa );
    aObj[ o_dtwa ]->SetDim( dtp->nQ, dtp->nP );
    aObj[ o_dtavsd_w ]->SetPtr( dtp->avsd_w );
    aObj[ o_dtavsd_w ]->SetDim( 2, dtp->nP );
    aObj[ o_dtan]->SetPtr(dtp->An);
    aObj[ o_dtan]->SetDim( dtp->nM, dtp->Nb );  // dtp->Asiz ?????
    aObj[ o_dttprn]->SetPtr(  dtp->tprn );
     //aObj[ o_dttprn]->SetDim( 1, VIZmsize(dtp->tprn ));

// Added for lmfit SD oct 2005 
    aObj[ o_dttdat ]->SetPtr( dtp->tdat );
    aObj[ o_dttdat ]->SetDim( dtp->nQ, dtp->nM );
    aObj[ o_dtydat ]->SetPtr( dtp->ydat );
    aObj[ o_dtydat ]->SetDim( dtp->nQ, 1 );
    aObj[ o_dtwdat ]->SetPtr( dtp->wdat );
    aObj[ o_dtwdat ]->SetDim( dtp->nQ, 1 );
    aObj[ o_dtpar ]->SetPtr( dtp->par );
    aObj[ o_dtpar ]->SetDim( 1, dtp->nP );
    aObj[ o_dtwpar ]->SetPtr( dtp->wpar );
    aObj[ o_dtwpar ]->SetDim( 1, dtp->nP );
    aObj[ o_dtsdpar ]->SetPtr( dtp->sdpar );
    aObj[ o_dtsdpar ]->SetDim( 1, dtp->nP );
    aObj[ o_dtwa_ap ]->SetPtr( dtp->Wa_ap );
    aObj[ o_dtwa_ap ]->SetDim( 1, dtp->nP );

}


// set dynamic Objects ptr to values
void TDualTh::dyn_set(int q)
{
    ErrorIf( dtp!=&dt[q], GetName(),
      "E06DTrem: Invalid access to dt in dyn_set");
// dynamic
    dtp->Bb = (double *)aObj[ o_dtbb ]->GetPtr();
    dtp->Bn = (double *)aObj[ o_dtbn ]->GetPtr();
    dtp->Ub = (double *)aObj[ o_dtub ]->GetPtr();
    dtp->chi = (double *)aObj[ o_dtchi ]->GetPtr();
    dtp->eta_b = (double *)aObj[ o_dteta_b ]->GetPtr();
    dtp->act_n = (double *)aObj[ o_dtact_n ]->GetPtr();
    dtp->gam_n = (double *)aObj[ o_dtgam_n ]->GetPtr();
    dtp->Coul = (double *)aObj[ o_dtcoul ]->GetPtr();
    dtp->mu_b = (double *)aObj[ o_dtmu_n ]->GetPtr();
    dtp->mu_o = (double *)aObj[ o_dtmu_o ]->GetPtr();

    dtp->avsd_a = (double *)aObj[ o_dtavsd_a ]->GetPtr();
    dtp->avsd_o = (double *)aObj[ o_dtavsd_o ]->GetPtr();
    dtp->yconst = (double *)aObj[ o_dtyconst ]->GetPtr();
    dtp->qpg = (double *)aObj[ o_dtqpg ]->GetPtr();
    dtp->CIb = (float *)aObj[ o_dtcib ]->GetPtr();
    dtp->CIn = (float *)aObj[ o_dtcin ]->GetPtr();
    dtp->CAb = (float *)aObj[ o_dtcab ]->GetPtr();
    dtp->CAn = (float *)aObj[ o_dtcan ]->GetPtr();
    dtp->cExpr = (char *)aObj[ o_dtcexpr ]->GetPtr();
    dtp->gExpr = (char *)aObj[ o_dtgexpr ]->GetPtr();
    dtp->sdref = (char (*)[V_SD_RKLEN])aObj[ o_dtsdref ]->GetPtr();
    dtp->sdval = (char (*)[V_SD_VALEN])aObj[ o_dtsdval ]->GetPtr();
    dtp->nam_b = (char (*)[MAXIDNAME])aObj[ o_dtnam_b ]->GetPtr();
    dtp->nam_n = (char (*)[MAXIDNAME])aObj[ o_dtnam_n ]->GetPtr();
    dtp->par_n = (char (*)[MAXIDNAME])aObj[ o_dtpar_n ]->GetPtr();
    aObj[ o_dtnam_nr]->SetPtr(dtp->nam_n);
    aObj[ o_dtnam_nr]->SetDim( 1, dtp->nM );
    dtp->for_n = (char (*)[MAXFORMUNITDT])aObj[ o_dtfor_n ]->GetPtr();
    aObj[ o_dtres]->SetPtr(dtp->for_n);
    aObj[ o_dtres]->SetDim(  dtp->nM, 1 );
    dtp->for_b = (char (*)[MAXFORMUNITDT])aObj[ o_dtfor_b ]->GetPtr();
    dtp->stld = (char (*)[EQ_RKLEN])aObj[ o_dtstl ]->GetPtr();
    dtp->typ_n = (char *)aObj[ o_dttyp_n ]->GetPtr();
    dtp->CIclb = (char *)aObj[ o_dtciclb ]->GetPtr();
    dtp->CIcln = (char *)aObj[ o_dtcicln ]->GetPtr();
    dtp->AUclb = (char *)aObj[ o_dtauclb ]->GetPtr();
    dtp->AUcln = (char *)aObj[ o_dtaucln ]->GetPtr();
    dtp->SBM = (char (*)[MAXICNAME+MAXSYMB])aObj[ o_dtsbm ]->GetPtr();
    dtp->mu_a = (double *)aObj[ o_dtmu_a ]->GetPtr();
    dtp->Tdq = (float *)aObj[ o_dttdq ]->GetPtr();
    dtp->Pdq = (float *)aObj[ o_dtpdq ]->GetPtr();
    dtp->ISq = (float *)aObj[ o_dtisq ]->GetPtr();
    dtp->wa_cp = (short *)aObj[ o_dtwacp ]->GetPtr();
    dtp->gmx_n = (double (*)[5])aObj[ o_gmx_n ]->GetPtr();
    dtp->Wa = (double *)aObj[ o_dtwa ]->GetPtr();
    dtp->avsd_w = (double *)aObj[ o_dtavsd_w ]->GetPtr();

    dtp->An = (float *)aObj[ o_dtan ]->GetPtr();
    dtp->tprn = (char *)aObj[ o_dttprn ]->GetPtr();

    dtp->wdat = (double *)aObj[ o_dtwdat ]->GetPtr();
    dtp->wpar = (double *)aObj[ o_dtwpar ]->GetPtr();
    dtp->Wa_ap = (double *)aObj[ o_dtwa_ap ]->GetPtr();
// Added for lmfit SD oct 2005
    dtp->tdat = (double *)aObj[ o_dttdat ]->Free();
    dtp->ydat = (double *)aObj[ o_dtydat ]->Free();
    dtp->par  = (double *)aObj[ o_dtpar  ]->Free();
    dtp->sdpar = (double *)aObj[ o_dtsdpar ]->Free();
    // free internal arrays
    aObj[ o_lms_delta ]->Free();
    aObj[ o_lms_yfit  ]->Free();
    aObj[ o_lms_paf ]->Free();

}

// free dynamic memory in objects and values
void TDualTh::dyn_kill(int q)
{
    ErrorIf( dtp!=&dt[q], GetName(),
       "E05DTrem: Invalid access to dt in dyn_kill");
// dynamic
    dtp->Bb = (double *)aObj[ o_dtbb ]->Free();
    dtp->Bn = (double *)aObj[ o_dtbn ]->Free();
    dtp->Ub = (double *)aObj[ o_dtub ]->Free();
    dtp->chi = (double *)aObj[ o_dtchi ]->Free();
    dtp->eta_b = (double *)aObj[ o_dteta_b ]->Free();
    dtp->act_n = (double *)aObj[ o_dtact_n ]->Free();
    dtp->gam_n = (double *)aObj[ o_dtgam_n ]->Free();
    dtp->Coul = (double *)aObj[ o_dtcoul ]->Free();
    dtp->mu_b = (double *)aObj[ o_dtmu_n ]->Free();
    dtp->mu_o = (double *)aObj[ o_dtmu_o ]->Free();

    dtp->avsd_a = (double *)aObj[ o_dtavsd_a ]->Free();
    dtp->avsd_o = (double *)aObj[ o_dtavsd_o ]->Free();
    dtp->yconst = (double *)aObj[ o_dtyconst ]->Free();
    dtp->qpg = (double *)aObj[ o_dtqpg ]->Free();
    dtp->CIb = (float *)aObj[ o_dtcib ]->Free();
    dtp->CIn = (float *)aObj[ o_dtcin ]->Free();
    dtp->CAb = (float *)aObj[ o_dtcab ]->Free();
    dtp->CAn = (float *)aObj[ o_dtcan ]->Free();
    dtp->cExpr = (char *)aObj[ o_dtcexpr ]->Free();
    dtp->gExpr = (char *)aObj[ o_dtgexpr ]->Free();
    dtp->sdref = (char (*)[V_SD_RKLEN])aObj[ o_dtsdref ]->Free();
    dtp->sdval = (char (*)[V_SD_VALEN])aObj[ o_dtsdval ]->Free();
    dtp->nam_b = (char (*)[MAXIDNAME])aObj[ o_dtnam_b ]->Free();
    dtp->nam_n = (char (*)[MAXIDNAME])aObj[ o_dtnam_n ]->Free();
    dtp->par_n = (char (*)[MAXIDNAME])aObj[ o_dtpar_n ]->Free();
    aObj[ o_dtnam_nr]->SetPtr(0);
    //aObj[ o_dtnam_nr]->SetDim( 1, 0 );
    dtp->for_n = (char (*)[MAXFORMUNITDT])aObj[ o_dtfor_n ]->Free();
    aObj[ o_dtres]->SetPtr(0);
//    aObj[ o_dtres]->SetDim(  dtp->nM, 1 );

    dtp->for_b = (char (*)[MAXFORMUNITDT])aObj[ o_dtfor_b ]->Free();
    dtp->stld = (char (*)[EQ_RKLEN])aObj[ o_dtstl ]->Free();
    dtp->typ_n = (char *)aObj[ o_dttyp_n ]->Free();
    dtp->CIclb = (char *)aObj[ o_dtciclb ]->Free();
    dtp->CIcln = (char *)aObj[ o_dtcicln ]->Free();
    dtp->AUclb = (char *)aObj[ o_dtauclb ]->Free();
    dtp->AUcln = (char *)aObj[ o_dtaucln ]->Free();
    dtp->SBM = (char (*)[MAXICNAME+MAXSYMB])aObj[ o_dtsbm ]->Free();

    dtp->mu_a = (double *)aObj[ o_dtmu_a ]->Free();
    dtp->Tdq = (float *)aObj[ o_dttdq ]->Free();
    dtp->Pdq = (float *)aObj[ o_dtpdq ]->Free();
    dtp->ISq = (float *)aObj[ o_dtisq ]->Free();
//Added  new objects
    dtp->wa_cp = (short *)aObj[ o_dtwacp ]->Free();
    dtp->gmx_n = (double (*)[5])aObj[ o_gmx_n ]->Free();
    dtp->Wa = (double *)aObj[ o_dtwa ]->Free();
    dtp->avsd_w = (double *)aObj[ o_dtavsd_w ]->Free();

    dtp->An = (float *)aObj[ o_dtan ]->Free();
    dtp->tprn = (char *)aObj[ o_dttprn ]->Free();

// Added for lmfit SD oct 2005
    dtp->tdat = (double *)aObj[ o_dttdat ]->Free();
    dtp->ydat = (double *)aObj[ o_dtydat ]->Free();
    dtp->wdat = (double *)aObj[ o_dtwdat ]->Free();
    dtp->par  = (double *)aObj[ o_dtpar  ]->Free();
    dtp->wpar = (double *)aObj[ o_dtwpar ]->Free();
    dtp->sdpar = (double *)aObj[ o_dtsdpar ]->Free();
    dtp->Wa_ap = (double *)aObj[ o_dtwa_ap ]->Free();
   // free internal arrays
    aObj[ o_lms_delta ]->Free();
    aObj[ o_lms_yfit  ]->Free();
    aObj[ o_lms_paf ]->Free();
}

void TDualTh::lmfit_new()
{

// Added for lmfit SD oct 2005 
// from DB
    dtp->wdat = (double *)aObj[ o_dtwdat ]->Alloc( dtp->nQ, 1, D_ );
    dtp->wpar = (double *)aObj[ o_dtwpar ]->Alloc( 1, dtp->nP, D_ );
    dtp->Wa_ap = (double *)aObj[ o_dtwa_ap ]->Alloc( 1, dtp->nP, D_ );
// internal
    dtp->tdat = (double *)aObj[ o_dttdat ]->Alloc( dtp->nQ, dtp->nM, D_ );
    dtp->ydat = (double *)aObj[ o_dtydat ]->Alloc( dtp->nQ, 1, D_ );
    dtp->par = (double *)aObj[ o_dtpar ]->Alloc( 1, dtp->nP, D_ );
    dtp->sdpar = (double *)aObj[ o_dtsdpar ]->Alloc( 1, dtp->nP, D_ );
   // free internal arrays
    aObj[ o_lms_delta ]->Free();
    aObj[ o_lms_yfit  ]->Free();
    aObj[ o_lms_paf ]->Free();
}



// realloc dynamic memory
void TDualTh::dyn_new(int q)
{
    ErrorIf( dtp!=&dt[q], GetName(),
      "E04DTrem: Invalid access to dt in dyn_new.");

// dynamic
    dtp->Bb = (double *)aObj[ o_dtbb ]->Alloc( dtp->nQ, dtp->Nb, D_ );
    dtp->Bn = (double *)aObj[ o_dtbn ]->Alloc( dtp->nQ, dtp->Nb, D_ );
    dtp->Ub = (double *)aObj[ o_dtub ]->Alloc( dtp->nQ, dtp->Nb, D_ );
    dtp->chi = (double *)aObj[ o_dtchi ]->Alloc( dtp->nQ, dtp->nM, D_ );
    dtp->mu_b = (double *)aObj[ o_dtmu_n ]->Alloc( dtp->nQ, dtp->nM, D_ );

    if( dtp->PvCoul == S_ON )
      dtp->Coul = (double *)aObj[ o_dtcoul ]->Alloc( dtp->nQ, dtp->nM, D_ );
    else
      dtp->Coul = (double *)aObj[ o_dtcoul ]->Free();

    dtp->eta_b = (double *)aObj[ o_dteta_b ]->Alloc( dtp->nQ, dtp->nM, D_ );
    dtp->gam_n = (double *)aObj[ o_dtgam_n ]->Alloc( dtp->nQ, dtp->nM, D_ );
    dtp->mu_o = (double *)aObj[ o_dtmu_o ]->Alloc( dtp->nQ, dtp->nM, D_ );
    dtp->avsd_a = (double *)aObj[ o_dtavsd_a ]->Alloc( 2, dtp->nM, D_ );
    dtp->avsd_o = (double *)aObj[ o_dtavsd_o ]->Alloc( 2, dtp->nM, D_ );

  dtp->yconst = (double *)aObj[ o_dtyconst ]->Alloc( dtp->nQ, 1,  D_ );
    if( dtp->Nqpg>0 )
      dtp->qpg = (double *)aObj[ o_dtqpg ]->Alloc( dtp->nM, dtp->Nqpg, D_ );
    else
      dtp->qpg = (double *)aObj[ o_dtqpg ]->Free();

    if( dtp->PvICb == S_ON )
    {
      dtp->CIclb = (char *)aObj[ o_dtciclb ]->Alloc( 1, dtp->Nb, A_ );
      dtp->CIb = (float *)aObj[ o_dtcib ]->Alloc( dtp->nQ, dtp->Nb, F_ );
    }
    else
    {
      dtp->CIb = (float *)aObj[ o_dtcib ]->Free();
      dtp->CIclb = (char *)aObj[ o_dtciclb ]->Free();
    }
    if( dtp->PvICn == S_ON )
    {
      dtp->CIn = (float *)aObj[ o_dtcin ]->Alloc( dtp->nQ, dtp->Nb, F_ );
      dtp->CIcln = (char *)aObj[ o_dtcicln ]->Alloc( 1, dtp->Nb,  A_ );
    }
    else
    {
      dtp->CIn = (float *)aObj[ o_dtcin ]->Free();
      dtp->CIcln = (char *)aObj[ o_dtcicln ]->Free();
    }
    if( dtp->PvAUb == S_ON )
    {
      dtp->CAb = (float *)aObj[ o_dtcab ]->Alloc( dtp->nQ, dtp->La_b, F_ );
      dtp->AUclb = (char *)aObj[ o_dtauclb ]->Alloc( 1, dtp->La_b, A_ );
      dtp->CAn = (float *)aObj[ o_dtcan ]->Alloc( dtp->nQ, dtp->La_b, F_ );
      dtp->AUcln = (char *)aObj[ o_dtaucln ]->Alloc( 1, dtp->La_b, A_ );
    }
    else
    {
      dtp->CAb = (float *)aObj[ o_dtcab ]->Free();
      dtp->AUclb = (char *)aObj[ o_dtauclb ]->Free();
      dtp->CAn = (float *)aObj[ o_dtcan ]->Free();
      dtp->AUcln = (char *)aObj[ o_dtaucln ]->Free();
    }

    if( dtp->PvChi == S_ON )
        dtp->cExpr = (char *)aObj[ o_dtcexpr]->Alloc( 1, 2048, S_);
    else
        dtp->cExpr = (char *)aObj[ o_dtcexpr ]->Free();
    if( dtp->PvGam == S_ON )
        dtp->gExpr = (char *)aObj[ o_dtgexpr]->Alloc( 1, 2048, S_);
    else
        dtp->gExpr = (char *)aObj[ o_dtgexpr ]->Free();

    if( dtp->PvGam == S_ON || dtp->PvChi == S_ON )
        dtp->tprn = (char *)aObj[ o_dttprn]->Alloc( 1, 2048, S_);
    else
        dtp->tprn = (char *)aObj[ o_dttprn ]->Free();

    if( dtp->Nsd > 0 )
    {
        dtp->sdref =
            (char (*)[V_SD_RKLEN])aObj[ o_dtsdref]->Alloc( dtp->Nsd, 1, V_SD_RKLEN );
        dtp->sdval =
            (char (*)[V_SD_VALEN])aObj[ o_dtsdval]->Alloc( dtp->Nsd, 1, V_SD_VALEN );
    }
    else
    {
        dtp->sdref = (char (*)[V_SD_RKLEN])aObj[ o_dtsdref ]->Free();
        dtp->sdval = (char (*)[V_SD_VALEN])aObj[ o_dtsdval ]->Free();
    }
   dtp->nam_b =
       (char (*)[MAXIDNAME])aObj[ o_dtnam_b]->Alloc( dtp->nQ, 1, MAXIDNAME );
   dtp->nam_n =
       (char (*)[MAXIDNAME])aObj[ o_dtnam_n]->Alloc( dtp->nM, 1, MAXIDNAME );
   dtp->par_n =
       (char (*)[MAXIDNAME])aObj[ o_dtpar_n]->Alloc( 1, dtp->nP, MAXIDNAME );
   aObj[ o_dtnam_nr]->SetPtr(dtp->nam_n);
   aObj[ o_dtnam_nr]->SetDim( 1, dtp->nM );

   dtp->for_n = (char (*)[MAXFORMUNITDT])aObj[ o_dtfor_n]->Alloc(
                                         1,  dtp->nM, MAXFORMUNITDT );
   aObj[ o_dtres]->SetPtr(dtp->for_n);
   aObj[ o_dtres]->SetDim(  dtp->nM, 1 );

   if( dtp->La_b > 0 )
     dtp->for_b = (char (*)[MAXFORMUNITDT])aObj[ o_dtfor_b]->Alloc(
                                             1, dtp->La_b, MAXFORMUNITDT );
   else
     dtp->for_b = (char (*)[MAXFORMUNITDT])aObj[ o_dtfor_b ]->Free();

   dtp->stld = (char (*)[EQ_RKLEN])aObj[ o_dtstl]->Alloc( dtp->nQ, 1, EQ_RKLEN );
   dtp->typ_n = (char *)aObj[ o_dttyp_n ]->Alloc( dtp->nM, 1, A_ );
   dtp->SBM = (char (*)[MAXICNAME+MAXSYMB])aObj[ o_dtsbm]->Alloc(
                                     1, dtp->Nb, MAXICNAME+MAXSYMB );

    dtp->mu_a = (double *)aObj[ o_dtmu_a ]->Alloc( dtp->nQ, dtp->nM, D_ );
    dtp->act_n = (double *)aObj[ o_dtact_n ]->Alloc( dtp->nQ, dtp->nM, D_ );
   if( dtp->PvTPI == S_ON )
   { dtp->Tdq = (float *)aObj[ o_dttdq ]->Alloc( dtp->nQ, 1, F_ );
     dtp->Pdq = (float *)aObj[ o_dtpdq ]->Alloc( dtp->nQ, 1, F_ );
     dtp->ISq = (float *)aObj[ o_dtisq ]->Alloc( dtp->nQ, 1, F_ );
   }
   else
   { dtp->Tdq = (float *)aObj[ o_dttdq ]->Free();
     dtp->Pdq = (float *)aObj[ o_dtpdq ]->Free();
     dtp->ISq = (float *)aObj[ o_dtisq ]->Free();
   }

//Added  new objects
    dtp->wa_cp = (short *)aObj[ o_dtwacp ]->Alloc( 1, dtp->nP, I_ );

    dtp->gmx_n = (double (*)[5])aObj[ o_gmx_n ]->Alloc( dtp->nQ, 5, D_ );
    if( dtp->nP > 0 )
    {  dtp->Wa = (double *)aObj[ o_dtwa ]->Alloc( dtp->nQ, dtp->nP, D_ );
       dtp->avsd_w = (double *)aObj[ o_dtavsd_w ]->Alloc( 2, dtp->nP, D_ );
    }
    else
    {   dtp->Wa = (double *)aObj[ o_dtwa ]->Free();
        dtp->avsd_w = (double *)aObj[ o_dtavsd_w ]->Free();
    }

   dtp->An = (float *)aObj[ o_dtan ]->Alloc( dtp->nM, dtp->Nb, F_ );
   dtp->Asiz = dtp->nM;


lmfit_new();

}


//set default information
void TDualTh::set_def( int q)
{
    ErrorIf( dtp!=&dt[q], GetName(),
        "E03DTrem: Invalid access to dt in set_def");

    TProfil *aPa= TProfil::pm;

    memcpy( &dtp->PunE, aPa->pa.TPpdc, 4 );
    memcpy( &dtp->PvCoul, aPa->pa.UTppc, 10 );
    memcpy( &dtp->gStat, "00+IJN", 6 );
    strcpy( dtp->name, "`" );
    strcpy( dtp->notes, "`" );

    memset( &dtp->nQ, 0, sizeof(short)*8 );
    memset( &dtp->Msysb, 0, sizeof(float)*12 );
    fillValue( dtp->sykey, '\0', (EQ_RKLEN+10) );
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
    dtp->eta_b = 0;
    dtp->act_n = 0;
    dtp->gam_n = 0;
    dtp->Coul = 0;
    dtp->mu_b = 0;
    dtp->mu_o = 0;
    dtp->avsd_a = 0;
    dtp->avsd_o = 0;
    dtp->yconst = 0;
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
    dtp->par_n = 0;
    dtp->for_n = 0;
    dtp->for_b = 0;
    dtp->stld = 0;
    dtp->typ_n = 0;
    dtp->CIclb = 0;
    dtp->CIcln = 0;
    dtp->AUclb = 0;
    dtp->AUcln = 0;
    dtp->SBM = 0;

    dtp->mu_a = 0;
    dtp->Tdq = 0;
    dtp->Pdq = 0;
    dtp->ISq = 0;
    dtp->wa_cp = 0;
    dtp->gmx_n = 0;
    dtp->Wa = 0;
    dtp->avsd_w = 0;
    dtp->An = 0;
    dtp->tprn = 0;
// Added for lmfit SD oct 2005 
    dtp->tdat = 0;
    dtp->ydat = 0;
    dtp->wdat = 0;
    dtp->par  = 0;
    dtp->wpar = 0;
    dtp->sdpar = 0;
    dtp->Wa_ap = 0;

}

// return true if nesessary, recalc base SYSEQ
bool TDualTh::check_input( const char * /*key*/, int /*Level*/ )
{
    char pkey[MAXRKEYLEN+10];
    if( !pVisor->ProfileMode )
        return true;

    //Get base SysEq key from process key
    rt[RT_SYSEQ]->MakeKey( RT_DUALTH, pkey, RT_DUALTH, 0, RT_DUALTH, 1,
                           RT_DUALTH, 2, RT_DUALTH, 3, RT_DUALTH, 4,
                           RT_DUALTH, 5, RT_DUALTH, 6, RT_DUALTH, 7, K_END);
    // read SysEq record and unpack data
    TProfil::pm->loadSystat( pkey );
   // test changes in system after process calc
    if( rt[RT_SYSEQ]->Rtime() > rt[nRT]->Rtime() )
        return true;
    return false;
}


// Input necessary data and links objects
void TDualTh::RecInput( const char *key )
{
    TCModule::RecInput( key );
}

//Rebuild record structure before calc
/* opens window with 'Remake record' parameters
*/
void
TDualTh::MakeQuery()
{
//    pImp->MakeQuery();
    const char * p_key;
    char flgs[20];
    int size[8];

    p_key  = db->PackKey();
    memcpy( flgs, &dtp->PunE, 20);
    size[0] = dtp->nQ;
    size[1] = dtp->La_b;
    size[2] = dtp->nM;
    size[3] = dtp->Nsd;
    size[7] = dtp->nP;

    if( !vfDualThSet( window(), p_key, flgs, size ))
         Error( p_key, "DualTh record configuration cancelled by the user!" );
     //  return;   // cancel

    memcpy( &dtp->PunE, flgs, 20);
    dtp->nQ = (short)size[0];
    dtp->La_b = (short)size[1];
    dtp->nM = (short)size[2];
    dtp->Nsd = (short)size[3];
    dtp->nP = (short)size[7];
    dtp->Nqpn = 0;            //reserved
    if( dtp->PvGam != S_OFF )
       dtp->Nqpg = 20;
    else
       dtp->Nqpg = 0;
    if( dtp->PvAUb != S_ON )
    {
       dtp->PvICb = S_ON;
       dtp->PvICn = S_ON;
    }
}

int
TDualTh::RecBuild( const char *key, int mode  )
{

 if( !pVisor->ProfileMode )
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
   dtp->gStat = GS_INDEF;
     // set data to SBM
   for(int ii=0; ii< dtp->Nb; ii++ )
        memcpy( dtp->SBM[ii], TRMults::sm->GetMU()->SB[ii], MAXICNAME  );

   pVisor->Update();
   return ret;
}

//Recalc record structure
void
TDualTh::RecCalc( const char *key )
{
    if( !pVisor->ProfileMode )
        Error( GetName(), "E02DTexec: Please, do it in the Project mode!" );


//   pVisor->Message( window(), GetName(),
//             "Generation of EqStat records\n"
//                 "Please, wait...", 0, usp->nQ);

// generate part
   if( dtp->gStat != GS_DONE )
   {
    dtp->gStat = GS_GOING;
    Init_Generation();
    build_Ub();
    dtp->gStat = GS_DONE;
   }

//analyse part
  Init_Analyse();
  Analyse();
  dtp->aStat = AS_DONE;

  pVisor->CloseMessage();
  TCModule::RecCalc(key);
}

const char* TDualTh::GetHtml()
{
   return GM_DUALTH_HTML;
}


// insert changes in Project to TDualTh
void TDualTh::InsertChanges( std::vector<CompItem>& aIComp )
{
    // insert changes to IComp
    if(aIComp.size()<1 || dtp->nQ < 1)
        return;

   // alloc memory & copy data from db

    int j, ii, jj=0;
    uint i=0;
    int Nold = dtp->Nb;
    double *p_Bb = new double[dtp->nQ*dtp->Nb];
        memcpy( p_Bb, dtp->Bb, dtp->nQ*dtp->Nb*sizeof(double));
    double *p_Bn = new double[dtp->nQ*dtp->Nb];
       memcpy( p_Bn, dtp->Bn, dtp->nQ*dtp->Nb*sizeof(double));
    double *p_Ub = new double[dtp->nQ*dtp->Nb];
       memcpy( p_Ub, dtp->Ub, dtp->nQ*dtp->Nb*sizeof(double));

    char *p_CIclb=0;
    float *p_CIb=0;
    char *p_CIcln=0;
    float *p_CIn=0;

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
   float *p_An = new float[dtp->nM*dtp->Nb];
    if( dtp->An )
      memcpy( p_An, dtp->An, dtp->nM*dtp->Nb*sizeof(float));

    // alloc new memory
     dtp->Nb = TRMults::sm->GetMU()->N;
     dyn_new();
     dtp->gStat = GS_INDEF;
     dtp->aStat = AS_INDEF;

//***************************************************
    for( ii=0; ii< dtp->Nb; ii++ )
      memcpy( dtp->SBM[ii], TRMults::sm->GetMU()->SB[ii], MAXICNAME  );

    ii = 0;
    while( jj < dtp->Nb )
    {
      if( i < aIComp.size() &&  aIComp[i].line == ii )
      {
        if( aIComp[i].delta == 1 )
        { // add line
          for( j =0; j<dtp->nQ; j++ )
          {
            dtp->Bb[j*dtp->Nb+jj] = 0.;
            dtp->Bn[j*dtp->Nb+jj] = 0.;
            dtp->Ub[j*dtp->Nb+jj] = 0.;
            if( dtp->PvICb == S_ON )
              dtp->CIb[j*dtp->Nb+jj] = 0.;
            if( dtp->PvICn == S_ON )
              dtp->CIn[j*dtp->Nb+jj] = 0.;
          }
          if( dtp->PvICb == S_ON )
              dtp->CIclb[jj] = QUAN_MOL;
          if( dtp->PvICn == S_ON )
              dtp->CIcln[jj] = QUAN_MOL;

          for( j =0; j<dtp->nM; j++ )
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
                 dtp->CIb[j*dtp->Nb+jj] = p_CIb[j*Nold+ii];
               if( dtp->PvICn == S_ON )
                 dtp->CIn[j*dtp->Nb+jj] = p_CIn[j*Nold+ii];
             }
               if( dtp->PvICb == S_ON )
                 dtp->CIclb[jj] = p_CIclb[ii];
               if( dtp->PvICn == S_ON )
                 dtp->CIcln[jj] = p_CIcln[ii];
             for( j =0; j<dtp->nM; j++ )
                dtp->An[j*dtp->Nb+jj] = p_An[j*Nold+ii];
          }
        jj++;
        ii++;
       }
    }
//*************************************************************

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

//set sizes of arrays
bool TDualTh::test_sizes( )
{
    bool i=true;

   // the same sizes
   dtp->Nb = TRMults::sm->GetMU()->N;

   if( dtp->nQ <=0 )
   {
      i = false;
      dtp->nQ = 1;
   }
   if( dtp->nM <=0 )
   {
      i = false;
      dtp->nM = 2;
   }
   if( dtp->PvSd == S_ON && dtp->Nsd <=0 )
     dtp->Nsd = 1;

   if( dtp->PvAUb == S_ON && dtp->La_b <=0  )
   { i= false;
     dtp->La_b = TRMults::sm->GetMU()->N;
   }

   if( i==false )
        vfMessage(window(), GetName(),
   "E00DTrem: Invalid dimensions in DualTh data structure!", vfErr);

    return i;
}

// set begin initalization
void TDualTh::dt_initiate( bool mode )
{
  dtp->aStat = AS_INDEF;

  dtp->cT = dtp->Td[START_];
  dtp->cP = dtp->Pd[START_];
  dtp->cV = dtp->Vd[START_];
  dtp->c_tm = dtp->tmd[START_];
  dtp->c_NV = dtp->NVd[START_];

  if( (dtp->PvTPI == S_ON) && dtp->Tdq && dtp->Pdq )
  {       // Take T and P from lists
     dtp->cT = dtp->Tdq[0];
     dtp->cP = dtp->Pdq[0];
  }
  dtp->q = 0;
  dtp->i = 0;
  dtp->jm = 0;

  if( mode )
  {
    int ii,i;
    char tbuf[100];

    dtp->gStat = GS_INDEF;

    dtp->Msysb = 0.;
    dtp->Vsysb = 0.;
    dtp->Mwatb = 1.;
    dtp->Maqb = 1.;
    dtp->Vaqb = 1.;

    for( ii=0; ii<dtp->Nb; ii++)
    {
     dtp->CIclb[ii] = QUAN_MOL;
     dtp->CIcln[ii] = QUAN_MOL;
    }

    for( ii=0; ii<dtp->nQ; ii++)
    {
     sprintf( tbuf, "Experiment%d", ii );
     strncpy( dtp->nam_b[ii], tbuf, MAXIDNAME );
    }

    for( ii=0; ii<dtp->nM; ii++)
    {
     dtp->typ_n[ii] = 'I';
     sprintf( tbuf, "Endmember%d", ii );
     strncpy( dtp->nam_n[ii], tbuf, MAXIDNAME );
     for( i=0; i<dtp->nQ; i++)
     {
        dtp->gam_n[i*dtp->nM+ii] = 1.;  // default for gamma 1.0
        dtp->chi[i*dtp->nM+ii] = DOUBLE_EMPTY;  // default for chi
     }
     dtp->wdat[ii] = 1.;
    }

    for( ii=0; ii<dtp->La_b; ii++)
    {
     dtp->AUclb[ii] = QUAN_MOL;
     strncpy( dtp->for_b[ii], "H2O", 4 );
     dtp->AUcln[ii] = QUAN_MOL;
     strncpy( dtp->for_n[ii], "H2O", 4 );
    }

    for( ii=0; ii<dtp->nP; ii++)
    {
      dtp->Wa_ap[ii] = 1.;
      dtp->wpar[ii] = 0.;
      dtp->wa_cp[ii] = 0;
    }


  }
}

// ------------------- End of m_dualth.cpp --------------------------
