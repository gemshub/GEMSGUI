//-------------------------------------------------------------------
// $Id$
// Under construction, to be included in version 3.0 (2003)
// Implementation of TUnSpace class, config and calculation functions
//
// Rewritten from C to C++ by S.Dmytriyeva
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

const char *GEMS_USP_HTML = "gm_unspace";

#include <math.h>
#include <stdio.h>

#include "m_unspace.h"
#include "m_syseq.h"
#include "v_object.h"
#include "service.h"
#include "visor.h"

TUnSpace* TUnSpace::pm;

TUnSpace::TUnSpace( int nrt ):
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
    aFldKeysHelp.Add("l<18 Identifier of this unspace simulator definition");
    aFldKeysHelp.Add("l<4  Batch simulator type code, a combination of {T, P, S, G, R}");

    usp=&us[0];
    set_def();
    start_title  = " Probing Uncertainty Space  ";

}


// get key of record
gstring
TUnSpace::GetKeyofRecord( const char *oldKey, const char *strTitle,
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
        rt[RT_UNSPACE].SetKey(str.c_str());
        rt[RT_SYSEQ].MakeKey( RT_UNSPACE, pkey, RT_UNSPACE, 0, RT_UNSPACE, 1,
                               RT_UNSPACE, 2, RT_UNSPACE, 3, RT_UNSPACE, 4,
                               RT_UNSPACE, 5, RT_UNSPACE, 6, RT_UNSPACE, 7, K_END);
        str = TSysEq::pm->GetKeyofRecord( pkey,
          "Please, select a parent System for a new UnSpace ", KEY_OLD );
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
    rt[RT_UNSPACE].SetKey(str.c_str());
    keyTest( str.c_str() );
    return str;
}

// test TUnSpace key to calc mode
void TUnSpace::keyTest( const char *key )
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
        rt[RT_SYSEQ].MakeKey( RT_UNSPACE, pkey, RT_UNSPACE, 0, RT_UNSPACE, 1,
                               RT_UNSPACE, 2, RT_UNSPACE, 3, RT_UNSPACE, 4,
                               RT_UNSPACE, 5, RT_UNSPACE, 6, RT_UNSPACE, 7, K_END);
        if( rt[RT_SYSEQ].Find(pkey) <0 )
            Error( key, "E07PErem: Illegal record key (no system)!");
    }
}



// link values to objects
void TUnSpace::ods_link( int q)
{
  usp=&us[q];

// static objects
   aObj[ o_unsts ].SetPtr( &usp->PunE );       /* a6 */
   aObj[ o_unpsflg ].SetPtr( &usp->PsUnInt );  /* a14 */
   aObj[ o_unpaflg ].SetPtr( &usp->Pa_f_pha ); /* a10 */
   aObj[ o_unpvflg ].SetPtr( &usp->PvPOM );    /* a8 */
   aObj[ o_unsiz ].SetPtr( &usp->N );           /* i6 */
   aObj[ o_unq ].SetPtr( &usp->Q );
   aObj[ o_unres ].SetPtr( &usp->ob );
   aObj[ o_unng ].SetPtr( &usp->nG );
   aObj[ o_unngtpv ].SetPtr( &usp->NgT );
   aObj[ o_unpnt ].SetPtr( &usp->Lapl );
   aObj[ o_unnmb ].SetPtr( &usp->nl );
   aObj[ o_unnpha ].SetPtr( &usp->nPhA );
   aObj[ o_un_t ].SetPtr( &usp->t );
   aObj[ o_un_q ].SetPtr( &usp->q );
   aObj[ o_un_g ].SetPtr( &usp->g );
   aObj[ o_un_i ].SetPtr( &usp->i );
   aObj[ o_un_j ].SetPtr( &usp->j );
   aObj[ o_un_k ].SetPtr( &usp->k );
   aObj[ o_un_ka ].SetPtr( &usp->ka );
   aObj[ o_un_tc ].SetPtr( &usp->T );
   aObj[ o_un_p ].SetPtr( &usp->P );
   aObj[ o_un_v ].SetPtr( &usp->V );
   aObj[ o_un_ph ].SetPtr( &usp->pH_lo );
   aObj[ o_un_eh ].SetPtr( &usp->Eh_lo );
   aObj[ o_un_ic ].SetPtr( &usp->IC_lo );
   aObj[ o_unquan ].SetPtr( &usp->quan_lev );
   aObj[ o_unname ].SetPtr( usp->name );
   aObj[ o_unnotes ].SetPtr( usp->notes );
   aObj[ o_unpapv ].SetPtr( &usp->PunE );   /* a 38 */
   aObj[ o_unint ].SetPtr( &usp->N );       /* i 32 */
   aObj[ o_unfloat ].SetPtr( &usp->T );     /* f 16 */
   aObj[ o_un_cr ].SetPtr( &usp->CrL );     /* d 4 */
   aObj[ o_unstkey ].SetPtr( usp->stkey );                 //33
  // names of columns
   aObj[ o_ununicn ].SetPtr( usp->UnICn );
   aObj[ o_ununicn ].SetDim( 1, UNSP_SIZE1 );
   aObj[ o_unugdcn ].SetPtr( usp->UgDCn );
   aObj[ o_unugdcn ].SetDim( 1, UNSP_SIZE1 );
   aObj[ o_unundcan ].SetPtr( usp->UnDCAn );
   aObj[ o_unundcan ].SetDim( 1, UNSP_SIZE2 );

  //dynamic arrays
    aObj[ o_unpbd].SetPtr( usp->PbD );
    aObj[ o_unpbd].SetDim( usp->nG, 1 );
    aObj[ o_unnglg].SetPtr( usp->NgLg );
    aObj[ o_unnglg].SetDim( usp->L, 1 );
    aObj[ o_unnggam].SetPtr( usp->NgGam );
    aObj[ o_unnggam].SetDim( usp->Ls, 1 );
    aObj[ o_unngls].SetPtr( usp->NgLs );
    aObj[ o_unngls].SetDim( usp->L, 1 );
    aObj[ o_unnglv].SetPtr( usp->NgLv );
    aObj[ o_unnglv].SetDim( usp->L, 1 );
    aObj[ o_unngnb].SetPtr( usp->NgNb );
    aObj[ o_unngnb].SetDim( usp->N, 1 );
    aObj[ o_unf_pha].SetPtr( usp->f_PhA );
    aObj[ o_unf_pha].SetDim( usp->N, 1 );
    aObj[ o_unintlgo].SetPtr( usp->IntLg0 );
    aObj[ o_unintlgo].SetDim( usp->L, 1 );
    aObj[ o_unintgam].SetPtr( usp->IntGam );
    aObj[ o_unintgam].SetDim( usp->Ls, 1 );
    aObj[ o_unintls].SetPtr( usp->IntLs );
    aObj[ o_unintls].SetDim( usp->L, 1 );
    aObj[ o_unintlv].SetPtr( usp->IntLv );
    aObj[ o_unintlv].SetDim( usp->L, 1 );
    aObj[ o_unintnb].SetPtr( usp->IntNb );
    aObj[ o_unintnb].SetDim( usp->N, 1 );
    aObj[ o_unm_t_lo].SetPtr( usp->m_t_lo );
    aObj[ o_unm_t_lo].SetDim( usp->N, 1 );
    aObj[ o_unm_t_up].SetPtr( usp->m_t_up );
    aObj[ o_unm_t_up].SetDim( usp->N, 1 );
    aObj[ o_unintnb].SetPtr( usp->fug_lo );
    aObj[ o_unintnb].SetDim( usp->Ls, 1 );
    aObj[ o_unfug_up].SetPtr( usp->fug_up );
    aObj[ o_unfug_up].SetDim( usp->Ls, 1 );
    aObj[ o_unintlg].SetPtr( usp->IntLg );
    aObj[ o_unintlg].SetDim( usp->L, 1 );
    aObj[ o_ungs].SetPtr( usp->Gs );
    aObj[ o_ungs].SetDim( usp->L, 1 );
    aObj[ o_unss].SetPtr( usp->Ss );
    aObj[ o_unss].SetDim( usp->L, 1 );
    aObj[ o_unvs].SetPtr( usp->Vs );
    aObj[ o_unvs].SetDim( usp->L, 1 );
    aObj[ o_ungams].SetPtr( usp->GAMs );
    aObj[ o_ungams].SetDim( usp->Ls, 1 );
    aObj[ o_unncp].SetPtr( usp->ncp );
    aObj[ o_unncp].SetDim( usp->Q, usp->nG );
    aObj[ o_unbs].SetPtr( usp->Bs );
    aObj[ o_unbs].SetDim( usp->N, 1 );
    aObj[ o_unvg].SetPtr( usp->vG );
    aObj[ o_unvg].SetDim( usp->Q, usp->L );
    aObj[ o_unvy].SetPtr( usp->vY );
    aObj[ o_unvy].SetDim( usp->Q, usp->L );
    aObj[ o_unvyf].SetPtr( usp->vYF );
    aObj[ o_unvyf].SetDim( usp->Q, usp->Fi );
    aObj[ o_unvgam].SetPtr( usp->vGam );
    aObj[ o_unvgam].SetDim( usp->Q, usp->L );
    aObj[ o_unvmol].SetPtr( usp->vMol );
    aObj[ o_unvmol].SetDim( usp->Q, usp->N );
    aObj[ o_unvu].SetPtr( usp->vU );
    aObj[ o_unvu].SetDim( usp->Q, usp->N );
    aObj[ o_unvfug].SetPtr( usp->vFug );
    aObj[ o_unvfug].SetDim( usp->Q, usp->Ls );
    aObj[ o_unvt].SetPtr( usp->vT );
    aObj[ o_unvt].SetDim( usp->Q, 1 );
    aObj[ o_unvp].SetPtr( usp->vP );
    aObj[ o_unvp].SetDim( usp->Q, 1 );
    aObj[ o_unvph].SetPtr( usp->vpH );
    aObj[ o_unvph].SetDim( usp->Q, 1 );
    aObj[ o_unovb].SetPtr( usp->OVB );
    aObj[ o_unovb].SetDim( usp->nGB+1, 1 );
    aObj[ o_unovr].SetPtr( usp->OVR );
    aObj[ o_unovr].SetDim( usp->nGR+1, 1 );
    aObj[ o_unovn].SetPtr( usp->OVN );
    aObj[ o_unovn].SetDim( usp->nGN+1, 1 );
    aObj[ o_unqulapl].SetPtr( usp->quanLap );
    aObj[ o_unqulapl].SetDim( usp->qQ, 1 );
    aObj[ o_unquhom].SetPtr( usp->quanHom );
    aObj[ o_unquhom].SetDim( usp->qQ, 1 );
    aObj[ o_unsgp].SetPtr( usp->SGp );
    aObj[ o_unsgp].SetDim( usp->nG, 1 );

//  work (not in record)
    aObj[ o_unphndx].SetPtr( usp->PhAndx );
    aObj[ o_unphndx].SetDim( usp->nPhA, usp->N );  // must be set nPhA = Q
    aObj[ o_unsv].SetPtr( usp->sv );
    aObj[ o_unsv].SetDim( usp->Q, 1 );
    aObj[ o_unphnum].SetPtr( usp->PhNum );
    aObj[ o_unphnum].SetDim( usp->nPhA, 1 );
    aObj[ o_unphaid].SetPtr( usp->PhAID );
    aObj[ o_unphaid].SetDim( usp->nPhA, 1 );
    aObj[ o_unphalst].SetPtr( usp->PhAlst );
    aObj[ o_unphalst].SetDim( usp->nPhA, 1 );
    aObj[ o_unafreg].SetPtr( usp->PhAfreq );
    aObj[ o_unafreg].SetDim( usp->nPhA, 1 );
    aObj[ o_unpmr].SetPtr( usp->pmr );
    aObj[ o_unpmr].SetDim( usp->Q, 1 );
    aObj[ o_unpom].SetPtr( usp->POM );
    aObj[ o_unpom].SetDim( usp->Q, usp->Q );
    aObj[ o_unpor].SetPtr( usp->POR );
    aObj[ o_unpor].SetDim( usp->Q, 1 );
    aObj[ o_una].SetPtr( usp->A );
    aObj[ o_una].SetDim( usp->L, usp->N );
    aObj[ o_unzcp].SetPtr( usp->Zcp );
    aObj[ o_unzcp].SetDim( usp->Q, 1 );
    aObj[ o_unzmin].SetPtr( usp->Zmin );
    aObj[ o_unzmin].SetDim( usp->Q, 1 );
    aObj[ o_unzmax].SetPtr( usp->Zmax );
    aObj[ o_unzmax].SetDim( usp->Q, 1 );
    aObj[ o_unzmaxabs].SetPtr( usp->ZmaxAbs );
    aObj[ o_unzmaxabs].SetDim( usp->Q, 1 );
    aObj[ o_unhom].SetPtr( usp->Hom );
    aObj[ o_unhom].SetDim( usp->Q, 1 );
    aObj[ o_unprob].SetPtr( usp->Prob );
    aObj[ o_unprob].SetDim( usp->Q, 1 );
    aObj[ o_ununic].SetPtr( usp->UnIC );
    aObj[ o_ununic].SetDim( usp->N, UNSP_SIZE1 );
    aObj[ o_unugdc].SetPtr( usp->UgDC );
    aObj[ o_unugdc].SetDim( usp->L, UNSP_SIZE1 );
    aObj[ o_unuadc].SetPtr( usp->UaDC );
    aObj[ o_unuadc].SetDim( usp->L, UNSP_SIZE1 );
    aObj[ o_unundca].SetPtr( usp->UnDCA );
    aObj[ o_unundca].SetDim( usp->L, UNSP_SIZE2 );

   //  internal
    aObj[ o_unsdref].SetPtr(usp->sdref);
    aObj[ o_unsdref].SetDim(usp->Nsd, 1 );
    aObj[ o_unsdval].SetPtr(usp->sdval);
    aObj[ o_unsdval].SetDim(usp->Nsd, 1 );
    aObj[ o_unstl].SetPtr( usp->stl );
    aObj[ o_unstl].SetDim( usp->Q, 1 );
    aObj[o_unexpr].SetPtr( usp->Expr );
        //aObj[o_unexpr].SetDim(1,len(usp->Expr));
    aObj[ o_untprn].SetPtr( usp->tprn );

    //graphics
    aObj[ o_unxnames].SetPtr(  usp->xNames );
    aObj[ o_unynames].SetPtr(  usp->yNames );
    aObj[ o_unaxis ].SetPtr(   usp->axisType );   /* i 6 */
    aObj[ o_ucsize].SetPtr(    usp->size[0] );    /* f 8 */
    aObj[ o_unplline].SetPtr( 0/*plot*/ );
    aObj[ o_unplline].SetDim( 0, sizeof(TPlotLine));

}


// set dynamic Objects ptr to values
void TUnSpace::dyn_set(int q)
{
    ErrorIf( usp!=&us[q], GetName(),
             "Illegal access to us in dyn_set.");

    usp->UnICn = (char (*)[NAME_SIZE])aObj[ o_ununicn ].GetPtr();
    usp->UgDCn = (char (*)[NAME_SIZE])aObj[ o_unugdcn ].GetPtr();
    usp->UnDCAn = (char (*)[NAME_SIZE])aObj[ o_unundcan ].GetPtr();
   //dynamic arrays
    usp->PbD =  (short *)aObj[ o_unpbd].GetPtr();
    usp->NgLg = (short *)aObj[ o_unnglg].GetPtr();
    usp->NgGam =(short *)aObj[ o_unnggam].GetPtr();
    usp->NgLs = (short *)aObj[ o_unngls].GetPtr();
    usp->NgLv = (short *)aObj[ o_unnglv].GetPtr();
    usp->NgNb = (short *)aObj[ o_unngnb].GetPtr();
    usp->f_PhA = (short *)aObj[ o_unf_pha].GetPtr();
    usp->IntLg0 = (float *)aObj[ o_unintlgo].GetPtr();
    usp->IntGam = (float *)aObj[ o_unintgam].GetPtr();
    usp->IntLs = (float *)aObj[ o_unintls].GetPtr();
    usp->IntLv = (float *)aObj[ o_unintlv].GetPtr();
    usp->IntNb = (float *)aObj[ o_unintnb].GetPtr();
    usp->m_t_lo = (float *)aObj[ o_unm_t_lo].GetPtr();
    usp->m_t_up = (float *)aObj[ o_unm_t_up].GetPtr();
    usp->fug_lo = (float *)aObj[ o_unintnb].GetPtr();
    usp->fug_up = (float *)aObj[ o_unfug_up].GetPtr();
    usp->IntLg = (float *)aObj[ o_unintlg].GetPtr();
    usp->Gs = (float *)aObj[ o_ungs].GetPtr();
    usp->Ss = (float *)aObj[ o_unss].GetPtr();
    usp->Vs = (float *)aObj[ o_unvs].GetPtr();
    usp->GAMs = (float *)aObj[ o_ungams].GetPtr();
    usp->ncp = (float *)aObj[ o_unncp].GetPtr();
    usp->Bs = (double *)aObj[ o_unbs].GetPtr();
    usp->vG = (double *)aObj[ o_unvg].GetPtr();
    usp->vY = (double *)aObj[ o_unvy].GetPtr();
    usp->vYF = (double *)aObj[ o_unvyf].GetPtr();
    usp->vGam = (double *)aObj[ o_unvgam].GetPtr();
    usp->vMol = (double *)aObj[ o_unvmol].GetPtr();
    usp->vU = (double *)aObj[ o_unvu].GetPtr();
    usp->vFug = (double *)aObj[ o_unvfug].GetPtr();
    usp->vT = (float *)aObj[ o_unvt].GetPtr();
    usp->vP = (float *)aObj[ o_unvp].GetPtr();
    usp->vpH = (float *)aObj[ o_unvph].GetPtr();
    usp->OVB = (float *)aObj[ o_unovb].GetPtr();
    usp->OVR = (float *)aObj[ o_unovr].GetPtr();
    usp->OVN = (float *)aObj[ o_unovn].GetPtr();
    usp->quanLap = (short *)aObj[ o_unqulapl].GetPtr();
    usp->quanHom = (short *)aObj[ o_unquhom].GetPtr();
    usp->SGp = (char (*)[MAXPHNAME])aObj[ o_unsgp].GetPtr();

 //  internal
    usp->stl = (char (*)[EQ_RKLEN])aObj[ o_unstl].GetPtr();
    usp->Expr = (char *)aObj[o_unexpr].GetPtr();
    usp->sdref = (char (*)[V_SD_RKLEN])aObj[ o_unsdref].GetPtr();
    usp->sdval = (char (*)[V_SD_VALEN])aObj[ o_unsdval].GetPtr();
    plot  = (TPlotLine *)aObj[ o_unplline ].GetPtr();
    usp->tprn = (char *)aObj[ o_untprn].GetPtr();

//  work (not in record)
    usp->PhAndx = (short *)aObj[ o_unphndx].GetPtr();
    usp->sv = (short *)aObj[ o_unsv].GetPtr();
    usp->PhNum = (short *)aObj[ o_unphnum].GetPtr();
    usp->PhAID = (char (*)[8])aObj[ o_unphaid].GetPtr();
    usp->PhAlst = (char (*)[80])aObj[ o_unphalst].GetPtr();
    usp->PhAfreq = (float *)aObj[ o_unafreg].GetPtr();
    usp->pmr = (float *)aObj[ o_unpmr].GetPtr();
    usp->POM = (float *)aObj[ o_unpom].GetPtr();
    usp->POR = (float *)aObj[ o_unpor].GetPtr();
    usp->A = (float *)aObj[ o_una].GetPtr();
    usp->Zcp = (double *)aObj[ o_unzcp].GetPtr();
    usp->Zmin = (double *)aObj[ o_unzmin].GetPtr();
    usp->Zmax = (double *)aObj[ o_unzmax].GetPtr();
    usp->ZmaxAbs = (double *)aObj[ o_unzmaxabs].GetPtr();
    usp->Hom = (double *)aObj[ o_unhom].GetPtr();
    usp->Prob = (double *)aObj[ o_unprob].GetPtr();
    usp->UnIC = (double (*)[UNSP_SIZE1])aObj[ o_ununic].GetPtr();
    usp->UgDC = (double (*)[UNSP_SIZE1])aObj[ o_unugdc].GetPtr();
    usp->UaDC = (double (*)[UNSP_SIZE1])aObj[ o_unuadc].GetPtr();
    usp->UnDCA = (double (*)[UNSP_SIZE2])aObj[ o_unundca].GetPtr();

    // make work arrays
    if( pVisor->ProfileMode != true )
        return;
//    work_arrays_new();
}


// free dynamic memory in objects and values
void TUnSpace::dyn_kill(int q)
{
    ErrorIf( usp!=&us[q], GetName(),
             "Illegal access to us in dyn_kill.");

    usp->UnICn = (char (*)[NAME_SIZE])aObj[ o_ununicn ].Free();
    usp->UgDCn = (char (*)[NAME_SIZE])aObj[ o_unugdcn ].Free();
    usp->UnDCAn = (char (*)[NAME_SIZE])aObj[ o_unundcan ].Free();
  //dynamic arrays
    usp->PbD =  (short *)aObj[ o_unpbd].Free();
    usp->NgLg = (short *)aObj[ o_unnglg].Free();
    usp->NgGam =(short *)aObj[ o_unnggam].Free();
    usp->NgLs = (short *)aObj[ o_unngls].Free();
    usp->NgLv = (short *)aObj[ o_unnglv].Free();
    usp->NgNb = (short *)aObj[ o_unngnb].Free();
    usp->f_PhA = (short *)aObj[ o_unf_pha].Free();
    usp->IntLg0 = (float *)aObj[ o_unintlgo].Free();
    usp->IntGam = (float *)aObj[ o_unintgam].Free();
    usp->IntLs = (float *)aObj[ o_unintls].Free();
    usp->IntLv = (float *)aObj[ o_unintlv].Free();
    usp->IntNb = (float *)aObj[ o_unintnb].Free();
    usp->m_t_lo = (float *)aObj[ o_unm_t_lo].Free();
    usp->m_t_up = (float *)aObj[ o_unm_t_up].Free();
    usp->fug_lo = (float *)aObj[ o_unintnb].Free();
    usp->fug_up = (float *)aObj[ o_unfug_up].Free();
    usp->IntLg = (float *)aObj[ o_unintlg].Free();
    usp->Gs = (float *)aObj[ o_ungs].Free();
    usp->Ss = (float *)aObj[ o_unss].Free();
    usp->Vs = (float *)aObj[ o_unvs].Free();
    usp->GAMs = (float *)aObj[ o_ungams].Free();
    usp->ncp = (float *)aObj[ o_unncp].Free();
    usp->Bs = (double *)aObj[ o_unbs].Free();
    usp->vG = (double *)aObj[ o_unvg].Free();
    usp->vY = (double *)aObj[ o_unvy].Free();
    usp->vYF = (double *)aObj[ o_unvyf].Free();
    usp->vGam = (double *)aObj[ o_unvgam].Free();
    usp->vMol = (double *)aObj[ o_unvmol].Free();
    usp->vU = (double *)aObj[ o_unvu].Free();
    usp->vFug = (double *)aObj[ o_unvfug].Free();
    usp->vT = (float *)aObj[ o_unvt].Free();
    usp->vP = (float *)aObj[ o_unvp].Free();
    usp->vpH = (float *)aObj[ o_unvph].Free();
    usp->OVB = (float *)aObj[ o_unovb].Free();
    usp->OVR = (float *)aObj[ o_unovr].Free();
    usp->OVN = (float *)aObj[ o_unovn].Free();
    usp->quanLap = (short *)aObj[ o_unqulapl].Free();
    usp->quanHom = (short *)aObj[ o_unquhom].Free();
    usp->SGp = (char (*)[MAXPHNAME])aObj[ o_unsgp].Free();

 //  internal
    usp->stl = (char (*)[EQ_RKLEN])aObj[ o_unstl].Free();
    usp->Expr = (char *)aObj[o_unexpr].Free();
    usp->sdref = (char (*)[V_SD_RKLEN])aObj[ o_unsdref].Free();
    usp->sdval = (char (*)[V_SD_VALEN])aObj[ o_unsdval].Free();
    plot  = (TPlotLine *)aObj[ o_unplline ].Free();
    usp->tprn = (char *)aObj[ o_untprn].Free();

   work_dyn_kill();
}


// free dynamic memory in objects and values
void TUnSpace::work_dyn_kill()
{

//  work (not in record)
    usp->PhAndx = (short *)aObj[ o_unphndx].Free();
    usp->sv = (short *)aObj[ o_unsv].Free();
    usp->PhNum = (short *)aObj[ o_unphnum].Free();
    usp->PhAID = (char (*)[8])aObj[ o_unphaid].Free();
    usp->PhAlst = (char (*)[80])aObj[ o_unphalst].Free();
    usp->PhAfreq = (float *)aObj[ o_unafreg].Free();
    usp->pmr = 0;
    usp->POM = (float *)aObj[ o_unpom].Free();
    usp->POR = (float *)aObj[ o_unpor].Free();
    usp->A = (float *)aObj[ o_una].Free();
    usp->Zcp = (double *)aObj[ o_unzcp].Free();
    usp->Zmin = (double *)aObj[ o_unzmin].Free();
    usp->Zmax = (double *)aObj[ o_unzmax].Free();
    usp->ZmaxAbs = (double *)aObj[ o_unzmaxabs].Free();
    usp->Hom = (double *)aObj[ o_unhom].Free();
    usp->Prob = (double *)aObj[ o_unprob].Free();
    usp->UnIC = (double (*)[UNSP_SIZE1])aObj[ o_ununic].Free();
    usp->UgDC = (double (*)[UNSP_SIZE1])aObj[ o_unugdc].Free();
    usp->UaDC = (double (*)[UNSP_SIZE1])aObj[ o_unuadc].Free();
    usp->UnDCA = (double (*)[UNSP_SIZE2])aObj[ o_unundca].Free();
}

//realloc dynamic memory for work arrays
void TUnSpace::phase_lists_new()
{

// alloc memory for nPhA size

if( usp->nPhA > 0 )
  {
    usp->PhAndx = (short *)aObj[ o_unphndx].Alloc( usp->nPhA, usp->N, I_);
    usp->PhNum = (short *)aObj[ o_unphnum].Alloc( usp->nPhA, 1 , I_ );
    usp->PhAID = (char (*)[8])aObj[ o_unphaid].Alloc( usp->nPhA, 1, 8 );
    usp->PhAlst = (char (*)[80])aObj[ o_unphalst].Alloc( usp->nPhA, 1, 80 );
    usp->PhAfreq = (float *)aObj[ o_unafreg].Alloc( usp->nPhA, 1, F_ );
  }
else
  {  usp->PhAndx = (short *)aObj[ o_unphndx].Free();
     usp->PhNum = (short *)aObj[ o_unphnum].Free();
     usp->PhAID = (char (*)[8])aObj[ o_unphaid].Free();
     usp->PhAlst = (char (*)[80])aObj[ o_unphalst].Free();
     usp->PhAfreq = (float *)aObj[ o_unafreg].Free();
  }

}

//realloc dynamic memory for work arrays
void TUnSpace::work_dyn_new()
{

//  work (not in record)
    usp->A = (float *)aObj[ o_una].Alloc( usp->L, usp->N, F_);
    usp->sv = (short *)aObj[ o_unsv].Alloc( usp->Q, 1, I_ );

    usp->Zcp = (double *)aObj[ o_unzcp].Alloc( usp->Q, 1, D_);
    usp->Zmin = (double *)aObj[ o_unzmin].Alloc(usp->Q, 1, D_);
    usp->Zmax = (double *)aObj[ o_unzmax].Alloc(usp->Q, 1, D_);
    usp->ZmaxAbs = (double *)aObj[ o_unzmaxabs].Alloc(usp->Q, 1, D_);
    usp->Hom = (double *)aObj[ o_unhom].Alloc(usp->Q, 1, D_);
    usp->Prob = (double *)aObj[ o_unprob].Alloc(usp->Q, 1, D_);

    usp->pmr = 0;
    if( usp->PvPOR == S_ON )
    {  usp->POR = (float *)aObj[ o_unpor].Alloc( 1, usp->Q, F_ );
       usp->pmr = usp->POR;
    }
    else
      usp->POR = (float *)aObj[ o_unpor].Free();

    if( usp->PvPOM == S_ON )
    {
      usp->POM = (float *)aObj[ o_unpom].Alloc( usp->Q, usp->Q, F_ );
      usp->pmr = usp->POM;
    }
    else   usp->POM = (float *)aObj[ o_unpom].Free();


  usp->UnIC = (double (*)[UNSP_SIZE1])aObj[ o_ununic].Alloc(
                    usp->N, UNSP_SIZE1, D_ );
  usp->UgDC = (double (*)[UNSP_SIZE1])aObj[ o_unugdc].Alloc(
                    usp->L, UNSP_SIZE1, D_ );
  usp->UaDC = (double (*)[UNSP_SIZE1])aObj[ o_unuadc].Alloc(
                    usp->Ls, UNSP_SIZE1, D_ );
  usp->UnDCA = (double (*)[UNSP_SIZE2])aObj[ o_unundca].Alloc(
                    usp->L, UNSP_SIZE2, D_ );

}

//realloc dynamic memory for work arrays
void TUnSpace::nG_dyn_new()
{

  if(usp->nG)
  {
    usp->PbD =  (short *)aObj[ o_unpbd].Alloc( usp->nG, 1, I_);
    usp->SGp = (char (*)[MAXPHNAME])aObj[ o_unsgp].Alloc(usp->nG, 1,MAXPHNAME);
    usp->ncp = (float *)aObj[ o_unncp].Alloc( usp->Q, usp->nG, F_);
  }  else
      { usp->PbD = (short *)aObj[ o_unpbd].Free();
        usp->SGp = (char (*)[MAXPHNAME])aObj[ o_unsgp].Free();
        usp->ncp = (float *)aObj[ o_unncp].Free();
       }
  if(usp->nGB)
    usp->OVB = (float *)aObj[ o_unovb].Alloc( usp->nGB+1, 1, F_ );
  else
   usp->OVB = (float *)aObj[ o_unovb].Free();

  if(usp->nGR)
    usp->OVR = (float *)aObj[ o_unovr].Alloc( usp->nGR+1, 1, F_ );
  else
   usp->OVR = (float *)aObj[ o_unovr].Free();

  if(usp->nGN)
    usp->OVN = (float *)aObj[ o_unovn].Alloc( usp->nGN+1, 1, F_ );
  else
   usp->OVN = (float *)aObj[ o_unovn].Free();

}

// realloc dynamic memory
void TUnSpace::dyn_new(int q)
{
  ErrorIf( usp!=&us[q], GetName(),
             "Illegal access to us in dyn_new.");

    usp->UnICn = (char (*)[NAME_SIZE])aObj[ o_ununicn ].Alloc(
       1, UNSP_SIZE1, NAME_SIZE );
    usp->UgDCn = (char (*)[NAME_SIZE])aObj[ o_unugdcn ].Alloc(
       1, UNSP_SIZE1, NAME_SIZE );
    usp->UnDCAn = (char (*)[NAME_SIZE])aObj[ o_unundcan ].Alloc(
       1, UNSP_SIZE2, NAME_SIZE );
  // allocation memory for arrays

  if(usp->PsGen[0] == S_ON )
  {
    usp->Gs = (float *)aObj[ o_ungs].Alloc( usp->L, 1, F_ );
    usp->NgLg = (short *)aObj[ o_unnglg].Alloc( usp->L, 1, I_ );
    usp->IntLg = (float *)aObj[ o_unintlg].Alloc( usp->L, 1, F_ );
    usp->IntLg0 = (float *)aObj[ o_unintlgo].Alloc( usp->L, 1, F_ );

    usp->vG = (double *)aObj[ o_unvg].Alloc( usp->Q, usp->L, D_);
    usp->vY = (double *)aObj[ o_unvy].Alloc( usp->Q, usp->L, D_);
    usp->vYF = (double *)aObj[ o_unvyf].Alloc( usp->Q, usp->Fi, D_);
    usp->vGam = (double *)aObj[ o_unvgam].Alloc( usp->Q, usp->L, D_);
    usp->vMol = (double *)aObj[ o_unvmol].Alloc( usp->Q, usp->N, D_);
    usp->vU = (double *)aObj[ o_unvu].Alloc( usp->Q, usp->N, D_);
    usp->vpH = (float *)aObj[ o_unvph].Alloc(usp->Q, 1, F_);
    usp->vT = (float *)aObj[ o_unvt].Alloc(usp->Q, 1, F_);
    usp->vP = (float *)aObj[ o_unvp].Alloc(usp->Q, 1, F_);

    usp->qQ = (int)(usp->quan_lev*usp->Q);
    if(usp->qQ<1)
        usp->qQ=1;
    usp->quanLap = (short *)aObj[ o_unqulapl].Alloc( usp->qQ, 1, I_ );
    usp->quanHom = (short *)aObj[ o_unquhom].Alloc( usp->qQ, 1, I_ );
  }
  else
  {
       usp->Gs = (float *)aObj[ o_ungs].Free();
       usp->NgLg = (short *)aObj[ o_unnglg].Free();
       usp->IntLg = (float *)aObj[ o_unintlg].Free();
       usp->IntLg0 = (float *)aObj[ o_unintlgo].Free();

       usp->vG = (double *)aObj[ o_unvg].Free();
       usp->vY = (double *)aObj[ o_unvy].Free();
       usp->vYF = (double *)aObj[ o_unvyf].Free();
       usp->vGam = (double *)aObj[ o_unvgam].Free();
       usp->vMol = (double *)aObj[ o_unvmol].Free();
       usp->vU = (double *)aObj[ o_unvu].Free();
       usp->vpH = (float *)aObj[ o_unvph].Free();
       usp->vT = (float *)aObj[ o_unvt].Free();
       usp->vP = (float *)aObj[ o_unvp].Free();
       usp->quanLap = (short *)aObj[ o_unqulapl].Free();
       usp->quanHom = (short *)aObj[ o_unquhom].Free();
  }


  if(usp->PsGen[0] == S_ON  && usp->Pa_f_mol == S_ON)
  {
    usp->m_t_lo = (float *)aObj[ o_unm_t_lo].Alloc( usp->N, 1, F_);
    usp->m_t_up = (float *)aObj[ o_unm_t_up].Alloc( usp->N, 1, F_);
  } else
     {
       usp->m_t_lo = (float *)aObj[ o_unm_t_lo].Free();
      usp->m_t_up = (float *)aObj[ o_unm_t_up].Free();
    }

  if( usp->PsGen[0] == S_ON  && usp->Ls )
       usp->vFug = (double *)aObj[ o_unvfug].Alloc(usp->Q, usp->Ls, D_);
  else
       usp->vFug = (double *)aObj[ o_unvfug].Free();

  if( usp->PsGen[0] == S_ON  && usp->Ls && usp->Pa_f_fug== S_ON )
  {
    usp->fug_lo = (float *)aObj[ o_unfug_lo].Alloc( usp->Ls, 1, F_ );
    usp->fug_up = (float *)aObj[ o_unfug_up].Alloc( usp->Ls, 1, F_ );
  }else
    {
       usp->fug_lo = (float *)aObj[ o_unfug_lo].Free();
       usp->fug_up = (float *)aObj[ o_unfug_up].Free();
    }

  if(usp->PsGen[1]== S_ON)
  {
    usp->Ss = (float *)aObj[ o_unss].Alloc( usp->L, 1, F_ );
    usp->NgLs = (short *)aObj[ o_unngls].Alloc( usp->L, 1, I_);
    usp->IntLs = (float *)aObj[ o_unintls].Alloc( usp->L, 1, F_);
  }else
   {
    usp->Ss = (float *)aObj[ o_unss].Free();
    usp->NgLs = (short *)aObj[ o_unngls].Free();
    usp->IntLs = (float *)aObj[ o_unintls].Free();
   }

  if(usp->PsGen[5]== S_ON)
  {
    usp->Vs = (float *)aObj[ o_unvs].Alloc( usp->L, 1, F_ );
    usp->NgLv = (short *)aObj[ o_unnglv].Alloc(usp->L, 1, I_);
    usp->IntLv = (float *)aObj[ o_unintlv].Alloc(usp->L, 1, F_);
  }else
    {
      usp->Vs = (float *)aObj[ o_unvs].Free();
      usp->NgLv = (short *)aObj[ o_unnglv].Free();
     usp->IntLv = (float *)aObj[ o_unintlv].Free();
    }

  if(usp->PsGen[2]== S_ON)
  {
    usp->NgNb = (short *)aObj[ o_unngnb].Alloc(usp->N, 1, I_);
    usp->IntNb = (float *)aObj[ o_unintnb].Alloc(usp->N, 1, F_);
    usp->Bs = (double *)aObj[ o_unbs].Alloc(usp->N, 1, D_);
  } else
    {
      usp->NgNb = (short *)aObj[ o_unngnb].Free();
      usp->IntNb = (float *)aObj[ o_unintnb].Free();
      usp->Bs = (double *)aObj[ o_unbs].Free();
     }

  if(usp->PsGen[6]== S_ON)   // new by DK
  {
    usp->NgGam =(short *)aObj[ o_unnggam].Alloc(usp->Ls, 1, I_);
    usp->IntGam = (float *)aObj[ o_unintgam].Alloc(usp->Ls, 1, F_);
    usp->GAMs = (float *)aObj[ o_ungams].Alloc(usp->Ls, 1, F_);
  } else
    {
    usp->NgGam =(short *)aObj[ o_unnggam].Free();
    usp->IntGam = (float *)aObj[ o_unintgam].Free();
    usp->GAMs = (float *)aObj[ o_ungams].Free();
     }

  if(/*usp->PsGen[0??] == S_ON  &&*/ usp->Pa_f_pha == S_ON)
    usp->f_PhA = (short *)aObj[ o_unf_pha].Alloc(usp->N, 1, I_);
   else
      usp->f_PhA = (short *)aObj[ o_unf_pha].Free();

  nG_dyn_new();                //!!!!!!!!!!!!!!!!!!

 //  internal
   usp->stl = (char (*)[EQ_RKLEN])aObj[ o_unstl].Alloc( usp->Q, 1, EQ_RKLEN);
   if( usp->PsEqn != S_OFF )
    {
      usp->Expr = (char *)aObj[o_unexpr].Alloc(1, 2048, S_ );
      usp->tprn = (char *)aObj[ o_untprn].Alloc(1, 2048, S_ );
    }
    else
    {
      usp->Expr = (char *)aObj[o_unexpr].Free();
      usp->tprn = (char *)aObj[ o_untprn].Free();
    }
   if( usp->Nsd > 0 )
   {
        usp->sdref =(char (*)[V_SD_RKLEN])aObj[ o_unsdref].Alloc(
                        usp->Nsd, 1, V_SD_RKLEN );
        usp->sdval =(char (*)[V_SD_VALEN])aObj[ o_unsdval].Alloc(
                        usp->Nsd, 1, V_SD_VALEN );
    }
    else
    {
        usp->sdref = (char (*)[V_SD_RKLEN])aObj[ o_unsdref].Free();
        usp->sdval = (char (*)[V_SD_VALEN])aObj[ o_unsdval].Free();
    }
   plot  = (TPlotLine *)aObj[ o_unplline ].Free();

  work_dyn_new();
  phase_lists_new();

}


//set default information
void TUnSpace::set_def( int q)
{
    ErrorIf( usp!=&us[q], GetName(),
             "Illegal access to us in set_def.");

   TProfil *aPa=(TProfil *)(&aMod[RT_PARAM]);

    strcpy( usp->name,  "`" );
    strcpy( usp->notes, "`" );
    memset( usp->stkey, 0, sizeof(char)*(EQ_RKLEN+10) );
   memcpy( &usp->PunE, aPa->pa.TPpdc, 4 );
   usp->Gstat = UNSP_GS_INDEF;
   usp->Astat = UNSP_AS_INDEF;
   memcpy( &usp->PsUnInt, "%%AA+------+--", 14 );
   memcpy( &usp->Pa_f_pha, "------0B0-", 10 );
   usp->Pa_Crit = UNSP_CRIT_LAPL_QAN;
   memcpy( &usp->PvPOM, "-+------", 8 );

   memset( &usp->N, 0, 32*sizeof(int));
   memset( &usp->T, 0, 16*sizeof(float));
   usp->quan_lev = 0.05;

    usp->UnICn = 0;
    usp->UgDCn = 0;
    usp->UnDCAn = 0;
// set up pointers
  //dynamic arrays
    usp->PbD =  0;
    usp->NgLg = 0;
    usp->NgGam = 0;
    usp->NgLs = 0;
    usp->NgLv = 0;
    usp->NgNb = 0;
    usp->f_PhA = 0;
    usp->IntLg0 = 0;
    usp->IntGam = 0;
    usp->IntLs = 0;
    usp->IntLv = 0;
    usp->IntNb = 0;
    usp->m_t_lo = 0;
    usp->m_t_up = 0;
    usp->fug_lo = 0;
    usp->fug_up = 0;
    usp->IntLg = 0;
    usp->Gs = 0;
    usp->Ss = 0;
    usp->Vs = 0;
    usp->GAMs = 0;
    usp->ncp = 0;
    usp->Bs = 0;
    usp->vG = 0;
    usp->vY = 0;
    usp->vYF = 0;
    usp->vGam = 0;
    usp->vMol = 0;
    usp->vU = 0;
    usp->vFug = 0;
    usp->vT = 0;
    usp->vP = 0;
    usp->vpH = 0;
    usp->OVB = 0;
    usp->OVR = 0;
    usp->OVN = 0;
    usp->quanLap = 0;
    usp->quanHom = 0;
    usp->SGp = 0;

 //  internal
    usp->stl = 0;
    usp->Expr = 0;
    usp->sdref = 0;
    usp->sdval = 0;
    usp->tprn = 0;
    // graphics
    strcpy( usp->xNames, "xp" );
    strcpy( usp->yNames, "yp" );
    plot  = 0;

//  work (not in record)
    usp->PhAndx = 0;
    usp->sv = 0;
    usp->PhNum = 0;
    usp->PhAID = 0;
    usp->PhAlst = 0;
    usp->PhAfreq = 0;
    usp->pmr = 0;
    usp->POM = 0;
    usp->POR = 0;
    usp->A = 0;
    usp->Zcp = 0;
    usp->Zmin = 0;
    usp->Zmax = 0;
    usp->ZmaxAbs = 0;
    usp->Hom = 0;
    usp->Prob = 0;
    usp->UnIC = 0;
    usp->UgDC = 0;
    usp->UaDC = 0;
    usp->UnDCA = 0;
}

// return true if nessasary recalc base SYSEQ
bool TUnSpace::check_input( const char */*key*/, int /*Level*/ )
{
  vstr pkey(MAXRKEYLEN+10);
  if( pVisor->ProfileMode != true )
        return true;

  TProfil* PRof = (TProfil*)(&aMod[RT_PARAM]);
  //Get base SysEq key from UnSpace key
  rt[RT_SYSEQ].MakeKey( RT_UNSPACE, pkey, RT_UNSPACE, 0, RT_UNSPACE, 1,
                           RT_UNSPACE, 2, RT_UNSPACE, 3, RT_UNSPACE, 4,
                           RT_UNSPACE, 5, RT_UNSPACE, 6, RT_UNSPACE, 7, K_END);
    // read SysEq record and unpack data
    PRof->loadSystat( pkey );
    // test changes in system after unspace calc
    if( rt[RT_SYSEQ].Rtime() > rt[nRT].Rtime() )
        return true;
    return false;
}

// Input nessasery data and links objects
void TUnSpace::RecInput( const char *key )
{
    TCModule::RecInput( key );
}

//Rebild record structure before calc
int
TUnSpace::RecBuild( const char *key, int mode  )
{
  bool set_def = false;

  if( pVisor->ProfileMode != true )
        Error( GetName(), "Do it in Project mode!" );
AGAIN:

    int ret = TCModule::RecBuild( key, mode );
    if( ret == VF_CANCEL )
        return ret;

   if( ret == VF3_1 )  // clear all
     set_def = true;

    if( !test_sizes() )
    {  mode = VF_REMAKE;    // first make change not remember: set_def_data_to_arrays();
       goto AGAIN;
    }
   usp->Gstat = UNSP_GS_INDEF;
   usp->Astat = UNSP_AS_INDEF;

   dyn_new();

   set_def_data_to_arrays( set_def );

    pVisor->Update();
    return ret;
}

//Recalc record structure
void
TUnSpace::RecCalc( const char *key )
{
    int nAdapt = 1;
//    TProfil* PRof = (TProfil*)(&aMod[RT_PARAM]);
    if( pVisor->ProfileMode != true )
        Error( GetName(), "E02PEexec: Please, do it in the Project mode!" );

    // load start syseq (alloc memory)
/*    rt[RT_SYSEQ].MakeKey( RT_UNSPACE, usp->stkey, RT_UNSPACE, 0, RT_UNSPACE,1,
             RT_UNSPACE, 2,  RT_UNSPACE, 3, RT_UNSPACE, 4, RT_UNSPACE, 5,
             RT_UNSPACE, 6, RT_UNSPACE, 7, K_END );
    int   nRec = rt[RT_SYSEQ].Find(usp->stkey);
    if( nRec >= 0)
         TProfil::pm->loadSystat( usp->stkey );   // read SysEq record and unpack data
*/


    if( usp->Pa_Adapt > '1')
    {  nAdapt = (int)(usp->Pa_Adapt-'0');
       usp->Gstat = UNSP_GS_INDEF; // for Adapt mode need   buildTestedArrays
                                   // for each cicle
    }

    if( usp->Gstat != UNSP_GS_DONE )
          init_generation();

   init_analyse();

//   pVisor->Message( window(), GetName(),
//             "Generation of EqStat records\n"
//                 "Please, wait...", 0, usp->Q);

    for( int ii=0; ii<nAdapt; ii++ )
    {
      if( usp->Gstat != UNSP_GS_DONE )
      {
          usp->Gstat = UNSP_GS_GOIN;
          usp->Astat = UNSP_AS_INDEF;
          buildTestedArrays();
      }
      aMod[RT_UNSPACE].ModUpdate("UnSpace data analysing in progress...");
      if( usp->PsGen[0] == S_ON )
      {
        analiseArrays();
        if( usp->Pa_Adapt > '1')
           AdapG();                    // !!!! test Kostin beak ob =0 or ob>Q*0.95
      }

    }
//    usp->Gstat = UNSP_GS_DONE;   // for Adapt mode need   buildTestedArrays
                                 // for each cicle
  usp->Astat = UNSP_AS_DONE;
//  aMod[RT_UNSPACE].ModUpdate("GtDemo data sampling in progress...");

  pVisor->CloseMessage();

//  TCModule::RecCalc(key);
  if( TProfil::pm->syp->Guns )
    memset( TProfil::pm->syp->Guns, 0, TProfil::pm->mup->L*sizeof(float) );
  if( TProfil::pm->syp->Vuns )
    memset( TProfil::pm->syp->Vuns, 0, TProfil::pm->mup->L*sizeof(float) );
}

void
TUnSpace::CmHelp()
{
    pVisor->OpenHelp( GEMS_USP_HTML );  //  05.01.01
}

// insert changes in Project to TUnSpace
void TUnSpace::InsertChanges( TIArray<CompItem>& aIComp,
    TIArray<CompItem>& aPhase,  TIArray<CompItem>&aDComp )
{
}
