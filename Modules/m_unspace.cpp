//-------------------------------------------------------------------
// $Id: m_unspace.cpp 1373 2009-07-22 12:25:22Z gems $
// Under construction, to be included in version 3.0 (2003)
// Implementation of TUnSpace class, config and calculation functions
//
// Rewritten from C to C++ by S.Dmytriyeva
// Copyright (C) 1995,2007 K.Chudnenko, S.Dmytriyeva, D.Kulik
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
#include <cstdio>
#ifndef _WIN32
#include <unistd.h>
#else
#include <io.h>
#endif

#include "m_unspace.h"
#include "m_syseq.h"
#include "visor.h"

TUnSpace* TUnSpace::pm;

TUnSpace::TUnSpace( uint nrt ):
        TCModule( nrt )
{
    nQ = 2;
    aFldKeysHelp.push_back("Name of the modeling project");
    aFldKeysHelp.push_back("Thermodynamic potential to minimize {G}");
    aFldKeysHelp.push_back("Name of the parent chemical system definition (CSD)");
    aFldKeysHelp.push_back("CSD (recipe) variant number <integer>");
    aFldKeysHelp.push_back("Volume of the system, dm3");
    aFldKeysHelp.push_back("Pressure, bar, or 0 for Psat(H2O)g");
    aFldKeysHelp.push_back("Temperature, C");
    aFldKeysHelp.push_back("Variant number for additional constraints");
    aFldKeysHelp.push_back("Name of this UnSpace calculation task");
    aFldKeysHelp.push_back("Payoff matrix function type code, optional {A, B, C, D, E, F}");
    setKeyEditField(8);

    usp=&us[1];
    set_def(1);
    usp=&us[0];
    set_def();
    start_title  = " Uncertainty Space Sensitivity Analysis ";

}


// get key of record
std::string
TUnSpace::GetKeyofRecord( const char *oldKey, const char *strTitle,
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
        rt[RT_UNSPACE]->SetKey(str.c_str());
        rt[RT_SYSEQ]->MakeKey( RT_UNSPACE, pkey, RT_UNSPACE, 0, RT_UNSPACE, 1,
                               RT_UNSPACE, 2, RT_UNSPACE, 3, RT_UNSPACE, 4,
                               RT_UNSPACE, 5, RT_UNSPACE, 6, RT_UNSPACE, 7, K_END);
        str = TSysEq::pm->GetKeyofRecord( pkey,
          "Please, select a parent System for a new UnSpace ", KEY_OLD );
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
    rt[RT_UNSPACE]->SetKey(str.c_str());
     if( keyType != KEY_TEMP )
         keyTest( str.c_str() );
    return str;
}

// test TUnSpace key to calc mode
void TUnSpace::keyTest( const char *key )
{
    char pkey[MAXRKEYLEN+10];

    if( pVisor->ProfileMode )
    { // test project key
        std::string prfKey = TProfil::pm->projectName();
        auto k = prfKey.length();
        if( memcmp(key, prfKey.c_str(), k ) ||
                ( key[k] != ':' && key[k] != ' ' && k<rt[RT_PARAM]->FldLen(0) )  )
            Error( key, "E08PErem: Invalid record key (another Modelling Project)!");
        rt[RT_SYSEQ]->MakeKey( RT_UNSPACE, pkey, RT_UNSPACE, 0, RT_UNSPACE, 1,
                               RT_UNSPACE, 2, RT_UNSPACE, 3, RT_UNSPACE, 4,
                               RT_UNSPACE, 5, RT_UNSPACE, 6, RT_UNSPACE, 7, K_END);
        if( rt[RT_SYSEQ]->Find(pkey) <0 )
            Error( key, "E07PErem: Invalid record key (no system)!");
    }
}



// link values to objects
void TUnSpace::ods_link( int q)
{
  int qq = q;
  if( q == -1 )
    q = 0;
  if( q == -2 )
    q = 1;

  usp=&us[q];

// static objects
   aObj[ o_unsts ]->SetPtr( &usp->PunE );       /* a6 */
   aObj[ o_unpsflg ]->SetPtr( &usp->PsUnInt );  /* a14 */
   aObj[ o_unpaflg ]->SetPtr( &usp->Pa_f_pha ); /* a10 */
   aObj[ o_unpaflg4 ]->SetPtr( &usp->Pa_Adapt ); /* a4 */
   aObj[ o_unpvflg ]->SetPtr( &usp->PvPOM );    /* a8 */
   aObj[ o_unsiz ]->SetPtr( &usp->N );           /* i6 */
   aObj[ o_unq ]->SetPtr( &usp->Q );
   aObj[ o_unres ]->SetPtr( &usp->ob );
   aObj[ o_unng ]->SetPtr( &usp->nG );
   aObj[ o_unngtpv ]->SetPtr( &usp->NgT );
   aObj[ o_unpnt ]->SetPtr( &usp->Lapl );
   aObj[ o_unnmb ]->SetPtr( &usp->nl );
   aObj[ o_unnpha ]->SetPtr( &usp->nPhA );
   aObj[ o_un_t ]->SetPtr( &usp->t );
   aObj[ o_un_q ]->SetPtr( &usp->q );
   aObj[ o_un_g ]->SetPtr( &usp->g );
   aObj[ o_un_i ]->SetPtr( &usp->i );
   aObj[ o_un_j ]->SetPtr( &usp->j );
   aObj[ o_un_k ]->SetPtr( &usp->k );
   aObj[ o_un_ka ]->SetPtr( &usp->ka );
   aObj[ o_undimxy]->SetPtr( usp->dimXY ); /*i 2*/
   aObj[ o_undimef]->SetPtr( usp->dimEF );  /*i 2*/
   aObj[ o_un_tc ]->SetPtr( usp->T );
   aObj[ o_un_p ]->SetPtr( usp->P );
   aObj[ o_un_v ]->SetPtr( usp->V );
   aObj[ o_un_ph ]->SetPtr( &usp->pH_lo );
   aObj[ o_un_eh ]->SetPtr( &usp->Eh_lo );
   aObj[ o_un_ic ]->SetPtr( &usp->IC_lo );
   aObj[ o_unquan ]->SetPtr( &usp->quan_lev );
   aObj[ o_unname ]->SetPtr( usp->name );
   aObj[ o_unnotes ]->SetPtr( usp->notes );
   aObj[ o_unpapv ]->SetPtr( &usp->PunE );   /* a 38 */
   aObj[ o_unint ]->SetPtr( &usp->N );       /* i 36 */
   aObj[ o_unfloat ]->SetPtr( usp->T );     /* f 22 */
   aObj[ o_un_cr ]->SetPtr( &usp->CrL );     /* d 4 */
   aObj[ o_unstkey ]->SetPtr( usp->stkey );                 //33
  // names of columns
   aObj[ o_ununicn ]->SetPtr( usp->UnICn );
   aObj[ o_ununicn ]->SetDim( 1, UNSP_SIZE1 );
   aObj[ o_unugdcn ]->SetPtr( usp->UgDCn );
   aObj[ o_unugdcn ]->SetDim( 1, UNSP_SIZE1 );
   aObj[ o_unuadcn ]->SetPtr( usp->UaDCn );
   aObj[ o_unuadcn ]->SetDim( 1, UNSP_SIZE1 );
   aObj[ o_unundcan ]->SetPtr( usp->UnDCAn );
   aObj[ o_unundcan ]->SetDim( 1, UNSP_SIZE2 );

  //dynamic arrays
    aObj[ o_unpbd]->SetPtr( usp->PbD );
    aObj[ o_unpbd]->SetDim( usp->nG, 1 );
    aObj[ o_unnglg]->SetPtr( usp->NgLg );
    aObj[ o_unnglg]->SetDim( usp->L, 1 );
    aObj[ o_unnggam]->SetPtr( usp->NgGam );
    aObj[ o_unnggam]->SetDim( usp->Ls, 1 );
    aObj[ o_unngls]->SetPtr( usp->NgLs );
    aObj[ o_unngls]->SetDim( usp->L, 1 );
    aObj[ o_unnglv]->SetPtr( usp->NgLv );
    aObj[ o_unnglv]->SetDim( usp->L, 1 );
    aObj[ o_unngnb]->SetPtr( usp->NgNb );
    aObj[ o_unngnb]->SetDim( usp->N, 1 );
    aObj[ o_unf_pha]->SetPtr( usp->f_PhA );
    aObj[ o_unf_pha]->SetDim( usp->N, 1 );
    aObj[ o_unintlgo]->SetPtr( usp->IntLg );
    aObj[ o_unintlgo]->SetDim( usp->L, 2 );
    aObj[ o_unintgam]->SetPtr( usp->IntGam );
    aObj[ o_unintgam]->SetDim( usp->Ls, 2 );
    aObj[ o_unintls]->SetPtr( usp->IntLs );
    aObj[ o_unintls]->SetDim( usp->L, 2 );
    aObj[ o_unintlv]->SetPtr( usp->IntLv );
    aObj[ o_unintlv]->SetDim( usp->L, 2 );
    aObj[ o_unintnb]->SetPtr( usp->IntNb );
    aObj[ o_unintnb]->SetDim( usp->N, 2 );
    aObj[ o_unm_t_lo]->SetPtr( usp->m_t_lo );
    aObj[ o_unm_t_lo]->SetDim( usp->N, 1 );
    aObj[ o_unm_t_up]->SetPtr( usp->m_t_up );
    aObj[ o_unm_t_up]->SetDim( usp->N, 1 );
    aObj[ o_unfug_lo]->SetPtr( usp->fug_lo );
    aObj[ o_unfug_lo]->SetDim( usp->Ls, 1 );
    aObj[ o_unfug_up]->SetPtr( usp->fug_up );
    aObj[ o_unfug_up]->SetDim( usp->Ls, 1 );
    aObj[ o_ungs]->SetPtr( usp->Gs );
    aObj[ o_ungs]->SetDim( usp->L, 2 );
    aObj[ o_unss]->SetPtr( usp->Ss );
    aObj[ o_unss]->SetDim( usp->L, 2 );
    aObj[ o_unvs]->SetPtr( usp->Vs );
    aObj[ o_unvs]->SetDim( usp->L, 2 );
    aObj[ o_ungams]->SetPtr( usp->GAMs );
    aObj[ o_ungams]->SetDim( usp->Ls, 2 );
    aObj[ o_unncp]->SetPtr( usp->ncp );
    aObj[ o_unncp]->SetDim( usp->Q, usp->nG );
    aObj[ o_unbs]->SetPtr( usp->Bs );
    aObj[ o_unbs]->SetDim( usp->N, 2 );
    aObj[ o_unvg]->SetPtr( usp->vG );
    aObj[ o_unvg]->SetDim( usp->Q, usp->L );
// added 2005
    aObj[ o_unbc]->SetPtr( usp->vB );
    aObj[ o_unbc]->SetDim( usp->Q, usp->N );
    aObj[ o_unsc]->SetPtr( usp->vS );
    aObj[ o_unsc]->SetDim( usp->Q, usp->L );
    aObj[ o_unmvc]->SetPtr( usp->vmV );
    aObj[ o_unmvc]->SetDim( usp->Q, usp->L );
    aObj[ o_unnidp]->SetPtr( usp->vNidP );
    aObj[ o_unnidp]->SetDim( usp->Q, usp->Ls );

    aObj[ o_unvy]->SetPtr( usp->vY );
    aObj[ o_unvy]->SetDim( usp->Q, usp->L );
    aObj[ o_unvyf]->SetPtr( usp->vYF );
    aObj[ o_unvyf]->SetDim( usp->Q, usp->Fi );
    aObj[ o_unvgam]->SetPtr( usp->vGam );
    aObj[ o_unvgam]->SetDim( usp->Q, usp->L );
    aObj[ o_unvmol]->SetPtr( usp->vMol );
    aObj[ o_unvmol]->SetDim( usp->Q, usp->N );
    aObj[ o_unvu]->SetPtr( usp->vU );
    aObj[ o_unvu]->SetDim( usp->Q, usp->N );
    aObj[ o_unvfug]->SetPtr( usp->vFug );
    aObj[ o_unvfug]->SetDim( usp->Q, usp->Ls );
    aObj[ o_unvt]->SetPtr( usp->vT );
    aObj[ o_unvt]->SetDim( usp->Q, 1 );
    aObj[ o_unvp]->SetPtr( usp->vP );
    aObj[ o_unvp]->SetDim( usp->Q, 1 );
    aObj[ o_unvv]->SetPtr( usp->vV );
    aObj[ o_unvv]->SetDim( usp->Q, 1 );
    aObj[ o_unvph]->SetPtr( usp->vpH );
    aObj[ o_unvph]->SetDim( usp->Q, 3 );
    aObj[ o_unovb]->SetPtr( usp->OVB );
    aObj[ o_unovb]->SetDim( usp->nGB+1, 1 );
    aObj[ o_unovr]->SetPtr( usp->OVR );
    aObj[ o_unovr]->SetDim( usp->nGR+1, 1 );
    aObj[ o_unovn]->SetPtr( usp->OVN );
    aObj[ o_unovn]->SetDim( usp->nGN+1, 1 );
    aObj[ o_unqucx]->SetPtr( usp->quanCx );
    aObj[ o_unqucx]->SetDim( usp->qQ, 4 );
    aObj[ o_unqucv]->SetPtr( usp->quanCv );
    aObj[ o_unqucv]->SetDim( usp->qQ, 4 );
    aObj[ o_unsgp]->SetPtr( usp->SGp );
    aObj[ o_unsgp]->SetDim( usp->nG, 1 );

  if( qq >= 0)
  {
  //  work (not in record)
    aObj[ o_unphndx]->SetPtr( usp->PhAndx );
    aObj[ o_unphndx]->SetDim( usp->nPhA, usp->N );  // must be set nPhA = Q
    aObj[ o_unsv]->SetPtr( usp->sv );
    aObj[ o_unsv]->SetDim( usp->Q, 1 );
    aObj[ o_unphnum]->SetPtr( usp->PhNum );
    aObj[ o_unphnum]->SetDim( usp->nPhA, 1 );
    aObj[ o_unphaid]->SetPtr( usp->PhAID );
    aObj[ o_unphaid]->SetDim( usp->nPhA, 1 );
    aObj[ o_unphalst]->SetPtr( usp->PhAlst );
    aObj[ o_unphalst]->SetDim( usp->nPhA, 1 );
    aObj[ o_unafreg]->SetPtr( usp->PhAfreq );
    aObj[ o_unafreg]->SetDim( usp->nPhA, 1 );
    aObj[ o_unpmr]->SetPtr( usp->pmr );
    aObj[ o_unpmr]->SetDim( usp->Q, 1 );
    aObj[ o_unpom]->SetPtr( usp->POM );
    aObj[ o_unpom]->SetDim( usp->Q, usp->Q );
    aObj[ o_unpor]->SetPtr( usp->POR );
    aObj[ o_unpor]->SetDim( 1, usp->Q );
    aObj[ o_una]->SetPtr( usp->A );
    aObj[ o_una]->SetDim( usp->L, usp->N );
    aObj[ o_unzcp]->SetPtr( usp->Zcp );
    aObj[ o_unzcp]->SetDim( usp->Q, 1 );
    aObj[ o_unzmin]->SetPtr( usp->Zmin );
    aObj[ o_unzmin]->SetDim( usp->Q, 1 );
    aObj[ o_unzmax]->SetPtr( usp->Zmax );
    aObj[ o_unzmax]->SetDim( usp->Q, 1 );
    aObj[ o_unzmaxabs]->SetPtr( usp->ZmaxAbs );
    aObj[ o_unzmaxabs]->SetDim( usp->Q, 1 );
    aObj[ o_unhom]->SetPtr( usp->Hom );
    aObj[ o_unhom]->SetDim( usp->Q, 1 );
    aObj[ o_unprob]->SetPtr( usp->Prob );
    aObj[ o_unprob]->SetDim( usp->Q, 1 );
    aObj[ o_ununic]->SetPtr( usp->UnIC );
    aObj[ o_ununic]->SetDim( usp->N, UNSP_SIZE1 );
    aObj[ o_unugdc]->SetPtr( usp->UgDC );
    aObj[ o_unugdc]->SetDim( usp->nPG, UNSP_SIZE1 );
    aObj[ o_unuadc]->SetPtr( usp->UaDC );
    aObj[ o_unuadc]->SetDim( usp->L, UNSP_SIZE1 );
    aObj[ o_unundca]->SetPtr( usp->UnDCA );
    aObj[ o_unundca]->SetDim( usp->nPG, UNSP_SIZE2 );
   }
   //  internal
    aObj[ o_unsdref]->SetPtr(usp->sdref);
    aObj[ o_unsdref]->SetDim(usp->Nsd, 1 );
    aObj[ o_unsdval]->SetPtr(usp->sdval);
    aObj[ o_unsdval]->SetDim(usp->Nsd, 1 );
    aObj[ o_unstl]->SetPtr( usp->stl );
    aObj[ o_unstl]->SetDim( usp->Q, 1 );
    aObj[o_unexpr]->SetPtr( usp->Expr );
        //aObj[o_unexpr]->SetDim(1,len(usp->Expr));
    aObj[ o_untprn]->SetPtr( usp->tprn );

    //graphics
    aObj[ o_unxnames]->SetPtr(  usp->xNames );
    aObj[ o_unynames]->SetPtr(  usp->yNames );
    aObj[ o_unaxis ]->SetPtr(   usp->axisType );   /* i 6 */
    aObj[ o_unsize]->SetPtr(    usp->size[0] );    /* f 8 */


    aObj[ o_unlnam]->SetPtr( usp->lNam[0] );
    aObj[ o_unlnam]->SetDim( 1, usp->dimXY[1]+usp->dimEF[1] );
    aObj[ o_unlname]->SetPtr( usp->ParNames[0] );
    aObj[ o_unlname]->SetDim( usp->nPG, 1 );
    aObj[o_ungexpr]->SetPtr( usp->ExprGraph );
        //aObj[o_ungexpr]->SetDim(1,len(usp->ExprGraph));
    aObj[o_unxa]->SetPtr( usp->x0 );
    aObj[o_unxa]->SetDim(usp->dimXY[0], 1 );
    aObj[o_unyc]->SetPtr( usp->y0 );
    aObj[o_unyc]->SetDim(usp->dimXY[0], usp->dimXY[1] );
    aObj[o_unxs]->SetPtr( usp->xE );
    aObj[o_unxs]->SetDim( usp->dimEF[0],1 );
    aObj[o_unys]->SetPtr( usp->yE );
    aObj[o_unys]->SetDim(usp->dimEF[0], usp->dimEF[1] );
    aObj[ o_unplline]->SetPtr( plot );
    aObj[ o_unplline]->SetDim( usp->dimXY[1]+usp->dimEF[1], sizeof(TPlotLine));
}


// set dynamic Objects ptr to values
void TUnSpace::dyn_set(int q)
{
    ErrorIf( usp!=&us[q], GetName(),
             "Invalid access to us in dyn_set.");

    usp->UnICn = (char (*)[NAME_SIZE])aObj[ o_ununicn ]->GetPtr();
    usp->UgDCn = (char (*)[NAME_SIZE])aObj[ o_unugdcn ]->GetPtr();
    usp->UaDCn = (char (*)[NAME_SIZE])aObj[ o_unuadcn ]->GetPtr();
    usp->UnDCAn = (char (*)[NAME_SIZE])aObj[ o_unundcan ]->GetPtr();
   //dynamic arrays
    usp->PbD =  (short *)aObj[ o_unpbd]->GetPtr();
    usp->NgLg = (short *)aObj[ o_unnglg]->GetPtr();
    usp->NgGam =(short *)aObj[ o_unnggam]->GetPtr();
    usp->NgLs = (short *)aObj[ o_unngls]->GetPtr();
    usp->NgLv = (short *)aObj[ o_unnglv]->GetPtr();
    usp->NgNb = (short *)aObj[ o_unngnb]->GetPtr();
    usp->f_PhA = (short *)aObj[ o_unf_pha]->GetPtr();
    usp->IntLg = (float (*)[2])aObj[ o_unintlgo]->GetPtr();
    usp->IntGam = (float (*)[2])aObj[ o_unintgam]->GetPtr();
    usp->IntLs = (float (*)[2])aObj[ o_unintls]->GetPtr();
    usp->IntLv = (float (*)[2])aObj[ o_unintlv]->GetPtr();
    usp->IntNb = (float (*)[2])aObj[ o_unintnb]->GetPtr();
    usp->m_t_lo = (float *)aObj[ o_unm_t_lo]->GetPtr();
    usp->m_t_up = (float *)aObj[ o_unm_t_up]->GetPtr();
    usp->fug_lo = (float *)aObj[ o_unfug_lo]->GetPtr();
    usp->fug_up = (float *)aObj[ o_unfug_up]->GetPtr();
    usp->Gs = (float (*)[2])aObj[ o_ungs]->GetPtr();
    usp->Ss = (float (*)[2])aObj[ o_unss]->GetPtr();
    usp->Vs = (float (*)[2])aObj[ o_unvs]->GetPtr();
    usp->GAMs = (float (*)[2])aObj[ o_ungams]->GetPtr();
    usp->ncp = (float *)aObj[ o_unncp]->GetPtr();
    usp->Bs = (double (*)[2])aObj[ o_unbs]->GetPtr();
    usp->vG = (double *)aObj[ o_unvg]->GetPtr();
// added 2005
    usp->vB = (double *)aObj[ o_unbc]->GetPtr();
    usp->vS = (double *)aObj[ o_unsc]->GetPtr();
    usp->vmV = (double *)aObj[ o_unmvc]->GetPtr();
    usp->vNidP = (double *)aObj[ o_unnidp]->GetPtr();

    usp->vY = (double *)aObj[ o_unvy]->GetPtr();
    usp->vYF = (double *)aObj[ o_unvyf]->GetPtr();
    usp->vGam = (double *)aObj[ o_unvgam]->GetPtr();
    usp->vMol = (double *)aObj[ o_unvmol]->GetPtr();
    usp->vU = (double *)aObj[ o_unvu]->GetPtr();
    usp->vFug = (double *)aObj[ o_unvfug]->GetPtr();
    usp->vT = (float *)aObj[ o_unvt]->GetPtr();
    usp->vP = (float *)aObj[ o_unvp]->GetPtr();
    usp->vV = (float *)aObj[ o_unvv]->GetPtr();
    usp->vpH = (float (*)[3])aObj[ o_unvph]->GetPtr();
    usp->OVB = (float *)aObj[ o_unovb]->GetPtr();
    usp->OVR = (float *)aObj[ o_unovr]->GetPtr();
    usp->OVN = (float *)aObj[ o_unovn]->GetPtr();
    usp->quanCx = (short (*)[4])aObj[ o_unqucx]->GetPtr();
    usp->quanCv = (float (*)[4])aObj[ o_unqucv]->GetPtr();
    usp->SGp = (char (*)[MAXPHNAME])aObj[ o_unsgp]->GetPtr();

 //  internal
    usp->stl = (char (*)[EQ_RKLEN])aObj[ o_unstl]->GetPtr();
    usp->Expr = (char *)aObj[o_unexpr]->GetPtr();
    usp->sdref = (char (*)[V_SD_RKLEN])aObj[ o_unsdref]->GetPtr();
    usp->sdval = (char (*)[V_SD_VALEN])aObj[ o_unsdval]->GetPtr();
// graphics

    // Change MAXGRNAME from 7 to 16
    if( usp->PsGraph != S_OFF && aObj[ o_unlnam ]->GetType() == 7 )
       usp->lNam = (char (*)[MAXGRNAME])aObj[ o_unlnam ]->Alloc( 1,
                  usp->dimXY[1]+usp->dimEF[1], MAXGRNAME);
    else
       usp->lNam = (char (*)[MAXGRNAME])aObj[ o_unlnam ]->GetPtr();

    usp->ParNames = (char (*)[PARNAME_SIZE])aObj[ o_unlname ]->GetPtr();
    usp->ExprGraph = (char *)aObj[ o_ungexpr ]->GetPtr();
    usp->x0    = (double *)aObj[ o_unxa ]->GetPtr();
    usp->y0    = (double *)aObj[ o_unyc ]->GetPtr();
    usp->xE    = (float *)aObj[ o_unxs ]->GetPtr();
    usp->yE    = (float *)aObj[ o_unys ]->GetPtr();
    plot  = (TPlotLine *)aObj[ o_unplline ]->GetPtr();
    usp->tprn = (char *)aObj[ o_untprn]->GetPtr();

/*---------------------------------------------------------------------
//  work (not in record)
    usp->PhAndx = (short *)aObj[ o_unphndx]->GetPtr();
    usp->sv = (short *)aObj[ o_unsv]->GetPtr();
    usp->PhNum = (short *)aObj[ o_unphnum]->GetPtr();
    usp->PhAID = (char (*)[8])aObj[ o_unphaid]->GetPtr();
    usp->PhAlst = (char (*)[80])aObj[ o_unphalst]->GetPtr();
    usp->PhAfreq = (float *)aObj[ o_unafreg]->GetPtr();
//    usp->pmr = (float *)aObj[ o_unpmr]->GetPtr();
    usp->POM = (float *)aObj[ o_unpom]->GetPtr();
    usp->POR = (float *)aObj[ o_unpor]->GetPtr();
    usp->A = (double *)aObj[ o_una]->GetPtr();
    usp->Zcp = (double *)aObj[ o_unzcp]->GetPtr();
    usp->Zmin = (double *)aObj[ o_unzmin]->GetPtr();
    usp->Zmax = (double *)aObj[ o_unzmax]->GetPtr();
    usp->ZmaxAbs = (double *)aObj[ o_unzmaxabs]->GetPtr();
    usp->Hom = (double *)aObj[ o_unhom]->GetPtr();
    usp->Prob = (double *)aObj[ o_unprob]->GetPtr();
    usp->UnIC = (double (*)[UNSP_SIZE1])aObj[ o_ununic]->GetPtr();
    usp->UgDC = (double (*)[UNSP_SIZE1])aObj[ o_unugdc]->GetPtr();
    usp->UaDC = (double (*)[UNSP_SIZE1])aObj[ o_unuadc]->GetPtr();
    usp->UnDCA = (double (*)[UNSP_SIZE2])aObj[ o_unundca]->GetPtr();
*/

    // make work arrays
    if( !pVisor->ProfileMode )
        return;
//    work_arrays_new();
}


// free dynamic memory in objects and values
void TUnSpace::dyn_kill(int q)
{
    ErrorIf( usp!=&us[q], GetName(),
             "Invalid access to us in dyn_kill.");

    usp->UnICn = (char (*)[NAME_SIZE])aObj[ o_ununicn ]->Free();
    usp->UgDCn = (char (*)[NAME_SIZE])aObj[ o_unugdcn ]->Free();
    usp->UaDCn = (char (*)[NAME_SIZE])aObj[ o_unuadcn ]->Free();
    usp->UnDCAn = (char (*)[NAME_SIZE])aObj[ o_unundcan ]->Free();
  //dynamic arrays
    usp->PbD =  (short *)aObj[ o_unpbd]->Free();
    usp->NgLg = (short *)aObj[ o_unnglg]->Free();
    usp->NgGam =(short *)aObj[ o_unnggam]->Free();
    usp->NgLs = (short *)aObj[ o_unngls]->Free();
    usp->NgLv = (short *)aObj[ o_unnglv]->Free();
    usp->NgNb = (short *)aObj[ o_unngnb]->Free();
    usp->f_PhA = (short *)aObj[ o_unf_pha]->Free();
    usp->IntLg = (float (*)[2])aObj[ o_unintlgo]->Free();
    usp->IntGam = (float (*)[2])aObj[ o_unintgam]->Free();
    usp->IntLs = (float (*)[2])aObj[ o_unintls]->Free();
    usp->IntLv = (float (*)[2])aObj[ o_unintlv]->Free();
    usp->IntNb = (float (*)[2])aObj[ o_unintnb]->Free();
    usp->m_t_lo = (float *)aObj[ o_unm_t_lo]->Free();
    usp->m_t_up = (float *)aObj[ o_unm_t_up]->Free();
    usp->fug_lo = (float *)aObj[ o_unfug_lo]->Free();
    usp->fug_up = (float *)aObj[ o_unfug_up]->Free();
    usp->Gs = (float (*)[2])aObj[ o_ungs]->Free();
    usp->Ss = (float (*)[2])aObj[ o_unss]->Free();
    usp->Vs = (float (*)[2])aObj[ o_unvs]->Free();
    usp->GAMs = (float (*)[2])aObj[ o_ungams]->Free();
    usp->ncp = (float *)aObj[ o_unncp]->Free();
    usp->Bs = (double (*)[2])aObj[ o_unbs]->Free();
    usp->vG = (double *)aObj[ o_unvg]->Free();

    usp->vB = (double *)aObj[ o_unbc]->Free();
    usp->vS = (double *)aObj[ o_unsc]->Free();
    usp->vmV = (double *)aObj[ o_unmvc]->Free();
    usp->vNidP = (double *)aObj[ o_unnidp]->Free();

    usp->vY = (double *)aObj[ o_unvy]->Free();
    usp->vYF = (double *)aObj[ o_unvyf]->Free();
    usp->vGam = (double *)aObj[ o_unvgam]->Free();
    usp->vMol = (double *)aObj[ o_unvmol]->Free();
    usp->vU = (double *)aObj[ o_unvu]->Free();
    usp->vFug = (double *)aObj[ o_unvfug]->Free();
    usp->vT = (float *)aObj[ o_unvt]->Free();
    usp->vP = (float *)aObj[ o_unvp]->Free();
    usp->vpH = (float (*)[3])aObj[ o_unvph]->Free();
    usp->OVB = (float *)aObj[ o_unovb]->Free();
    usp->OVR = (float *)aObj[ o_unovr]->Free();
    usp->OVN = (float *)aObj[ o_unovn]->Free();
    usp->quanCx = (short (*)[4])aObj[ o_unqucx]->Free();
    usp->quanCv = (float (*)[4])aObj[ o_unqucv]->Free();
    usp->SGp = (char (*)[MAXPHNAME])aObj[ o_unsgp]->Free();

 //  internal
    usp->stl = (char (*)[EQ_RKLEN])aObj[ o_unstl]->Free();
    usp->Expr = (char *)aObj[o_unexpr]->Free();
    usp->sdref = (char (*)[V_SD_RKLEN])aObj[ o_unsdref]->Free();
    usp->sdval = (char (*)[V_SD_VALEN])aObj[ o_unsdval]->Free();
// graphics
    usp->lNam = (char (*)[MAXGRNAME])aObj[ o_unlnam ]->Free();
    usp->ParNames = (char (*)[PARNAME_SIZE])aObj[ o_unlname ]->Free();
    usp->ExprGraph = (char *)aObj[ o_ungexpr ]->Free();
    usp->x0    = (double *)aObj[ o_unxa ]->Free();
    usp->y0    = (double *)aObj[ o_unyc ]->Free();
    usp->xE    = (float *)aObj[ o_unxs ]->Free();
    usp->yE    = (float *)aObj[ o_unys ]->Free();
    plot  = (TPlotLine *)aObj[ o_unplline ]->Free();

    usp->tprn = (char *)aObj[ o_untprn]->Free();

   if( q == 0)
      work_dyn_kill();
}


// free dynamic memory in objects and values
void TUnSpace::work_dyn_kill()
{

//  work (not in record)
    usp->PhAndx = (short *)aObj[ o_unphndx]->Free();
    usp->sv = (short *)aObj[ o_unsv]->Free();
    usp->PhNum = (short *)aObj[ o_unphnum]->Free();
    usp->PhAID = (char (*)[8])aObj[ o_unphaid]->Free();
    usp->PhAlst = (char (*)[80])aObj[ o_unphalst]->Free();
    usp->PhAfreq = (float *)aObj[ o_unafreg]->Free();
    usp->pmr = 0;
    usp->POM = (float *)aObj[ o_unpom]->Free();
    usp->POR = (float *)aObj[ o_unpor]->Free();
    aObj[ o_unpmr]->SetPtr( usp->pmr );
    aObj[ o_unpmr]->SetDim( usp->Q, 1 );
    usp->A = (double *)aObj[ o_una]->Free();
    usp->Zcp = (double *)aObj[ o_unzcp]->Free();
    usp->Zmin = (double *)aObj[ o_unzmin]->Free();
    usp->Zmax = (double *)aObj[ o_unzmax]->Free();
    usp->ZmaxAbs = (double *)aObj[ o_unzmaxabs]->Free();
    usp->Hom = (double *)aObj[ o_unhom]->Free();
    usp->Prob = (double *)aObj[ o_unprob]->Free();
    usp->UnIC = (double (*)[UNSP_SIZE1])aObj[ o_ununic]->Free();
    usp->UgDC = (double (*)[UNSP_SIZE1])aObj[ o_unugdc]->Free();
    usp->UaDC = (double (*)[UNSP_SIZE1])aObj[ o_unuadc]->Free();
    usp->UnDCA = (double (*)[UNSP_SIZE2])aObj[ o_unundca]->Free();
}

//realloc dynamic memory for work arrays
void TUnSpace::phase_lists_new()
{

// alloc memory for nPhA size

if( usp->nPhA > 0 )
  {
    usp->PhAndx = (short *)aObj[ o_unphndx]->Alloc( usp->nPhA, usp->N, I_);
    usp->PhNum = (short *)aObj[ o_unphnum]->Alloc( usp->nPhA, 1 , I_ );
    usp->PhAID = (char (*)[8])aObj[ o_unphaid]->Alloc( usp->nPhA, 1, 8 );
    usp->PhAlst = (char (*)[80])aObj[ o_unphalst]->Alloc( usp->nPhA, 1, 80 );
    usp->PhAfreq = (float *)aObj[ o_unafreg]->Alloc( usp->nPhA, 1, F_ );
  }
else
  {  usp->PhAndx = (short *)aObj[ o_unphndx]->Free();
     usp->PhNum = (short *)aObj[ o_unphnum]->Free();
     usp->PhAID = (char (*)[8])aObj[ o_unphaid]->Free();
     usp->PhAlst = (char (*)[80])aObj[ o_unphalst]->Free();
     usp->PhAfreq = (float *)aObj[ o_unafreg]->Free();
  }

}

//realloc dynamic memory for work arrays
void TUnSpace::work_dyn_new()
{

//  work (not in record)
    usp->A = (double *)aObj[ o_una]->Alloc( usp->L, usp->N, D_);
    usp->sv = (short *)aObj[ o_unsv]->Alloc( usp->Q, 1, I_ );

    usp->Zcp = (double *)aObj[ o_unzcp]->Alloc( usp->Q, 1, D_);
    usp->Zmin = (double *)aObj[ o_unzmin]->Alloc(usp->Q, 1, D_);
    usp->Zmax = (double *)aObj[ o_unzmax]->Alloc(usp->Q, 1, D_);
    usp->ZmaxAbs = (double *)aObj[ o_unzmaxabs]->Alloc(usp->Q, 1, D_);
    usp->Hom = (double *)aObj[ o_unhom]->Alloc(usp->Q, 1, D_);
    usp->Prob = (double *)aObj[ o_unprob]->Alloc(usp->Q, 1, D_);

    usp->pmr = 0;
    if( usp->PvPOR == S_ON )
    {  usp->POR = (float *)aObj[ o_unpor]->Alloc( 1, usp->Q, F_ );
       usp->pmr = usp->POR;
    }
    else
      usp->POR = (float *)aObj[ o_unpor]->Free();

    if( usp->PvPOM == S_ON )
    {
      usp->POM = (float *)aObj[ o_unpom]->Alloc( usp->Q, usp->Q, F_ );
      usp->pmr = usp->POM;
    }
    else   usp->POM = (float *)aObj[ o_unpom]->Free();

    aObj[ o_unpmr]->SetPtr( usp->pmr );
    aObj[ o_unpmr]->SetDim( usp->Q, 1 );


  usp->UnIC = (double (*)[UNSP_SIZE1])aObj[ o_ununic]->Alloc(
                    usp->N, UNSP_SIZE1, D_ );
  usp->UgDC = (double (*)[UNSP_SIZE1])aObj[ o_unugdc]->Alloc(
                    usp->nPG, UNSP_SIZE1, D_ );
  usp->UaDC = (double (*)[UNSP_SIZE1])aObj[ o_unuadc]->Alloc(
                    usp->Ls, UNSP_SIZE1, D_ );
  usp->UnDCA = (double (*)[UNSP_SIZE2])aObj[ o_unundca]->Alloc(
                    usp->nPG, UNSP_SIZE2, D_ );

}

//realloc dynamic memory for work arrays
void TUnSpace::nG_dyn_new()
{

  if(usp->nG)
  {
    usp->PbD =  (short *)aObj[ o_unpbd]->Alloc( usp->nG, 1, I_);
    usp->SGp = (char (*)[MAXPHNAME])aObj[ o_unsgp]->Alloc(usp->nG, 1,MAXPHNAME);
    usp->ncp = (float *)aObj[ o_unncp]->Alloc( usp->Q, usp->nG, F_);
  }  else
      { usp->PbD = (short *)aObj[ o_unpbd]->Free();
        usp->SGp = (char (*)[MAXPHNAME])aObj[ o_unsgp]->Free();
        usp->ncp = (float *)aObj[ o_unncp]->Free();
       }
  if(usp->nGB)
    usp->OVB = (float *)aObj[ o_unovb]->Alloc( usp->nGB+1, 1, F_ );
  else
   usp->OVB = (float *)aObj[ o_unovb]->Free();

  if(usp->nGR)
    usp->OVR = (float *)aObj[ o_unovr]->Alloc( usp->nGR+1, 1, F_ );
  else
   usp->OVR = (float *)aObj[ o_unovr]->Free();

  if(usp->nGN)
    usp->OVN = (float *)aObj[ o_unovn]->Alloc( usp->nGN+1, 1, F_ );
  else
   usp->OVN = (float *)aObj[ o_unovn]->Free();

  if( usp->nPG )
    usp->ParNames = (char (*)[PARNAME_SIZE])aObj[ o_unlname ]->Alloc(
                     usp->nPG, 1, PARNAME_SIZE);
  else
    usp->ParNames = (char (*)[PARNAME_SIZE])aObj[ o_unlname ]->Free();
}

// realloc dynamic memory
void TUnSpace::dyn_new(int q)
{
  ErrorIf( usp!=&us[q], GetName(),
             "Invalid access to us in dyn_new.");

    usp->UnICn = (char (*)[NAME_SIZE])aObj[ o_ununicn ]->Alloc(
       1, UNSP_SIZE1, NAME_SIZE );
    usp->UgDCn = (char (*)[NAME_SIZE])aObj[ o_unugdcn ]->Alloc(
       1, UNSP_SIZE1, NAME_SIZE );
    usp->UaDCn = (char (*)[NAME_SIZE])aObj[ o_unuadcn ]->Alloc(
       1, UNSP_SIZE1, NAME_SIZE );
    usp->UnDCAn = (char (*)[NAME_SIZE])aObj[ o_unundcan ]->Alloc(
       1, UNSP_SIZE2, NAME_SIZE );
  // allocation memory for arrays

  if(usp->PsGen[0] == S_ON )
  {
    usp->Gs = (float (*)[2])aObj[ o_ungs]->Alloc( usp->L, 2, F_ );
    usp->NgLg = (short *)aObj[ o_unnglg]->Alloc( usp->L, 1, I_ );
    usp->IntLg = (float (*)[2])aObj[ o_unintlgo]->Alloc( usp->L, 2, F_ );

    usp->vG = (double *)aObj[ o_unvg]->Alloc( usp->Q, usp->L, D_);
    usp->vY = (double *)aObj[ o_unvy]->Alloc( usp->Q, usp->L, D_);
    usp->vYF = (double *)aObj[ o_unvyf]->Alloc( usp->Q, usp->Fi, D_);
    usp->vGam = (double *)aObj[ o_unvgam]->Alloc( usp->Q, usp->L, D_);
    usp->vMol = (double *)aObj[ o_unvmol]->Alloc( usp->Q, usp->N, D_);
    usp->vU = (double *)aObj[ o_unvu]->Alloc( usp->Q, usp->N, D_);
    usp->vpH = (float (*)[3])aObj[ o_unvph]->Alloc(usp->Q, 3, F_);
    usp->vT = (float *)aObj[ o_unvt]->Alloc(usp->Q, 1, F_);
    usp->vP = (float *)aObj[ o_unvp]->Alloc(usp->Q, 1, F_);
    usp->vV = (float *)aObj[ o_unvv]->Alloc(usp->Q, 1, F_);

    usp->qQ = (short)(usp->quan_lev*usp->Q);
    if(usp->qQ<1)
        usp->qQ=1;
    usp->quanCx = (short (*)[4])aObj[ o_unqucx]->Alloc( usp->qQ, 4, I_ );
    usp->quanCv = (float (*)[4])aObj[ o_unqucv]->Alloc( usp->qQ, 4, F_ );
  }
  else
  {
       usp->Gs = (float (*)[2])aObj[ o_ungs]->Free();
       usp->NgLg = (short *)aObj[ o_unnglg]->Free();
       usp->IntLg = (float (*)[2])aObj[ o_unintlgo]->Free();

       usp->vG = (double *)aObj[ o_unvg]->Free();
       usp->vY = (double *)aObj[ o_unvy]->Free();
       usp->vYF = (double *)aObj[ o_unvyf]->Free();
       usp->vGam = (double *)aObj[ o_unvgam]->Free();
       usp->vMol = (double *)aObj[ o_unvmol]->Free();
       usp->vU = (double *)aObj[ o_unvu]->Free();
       usp->vpH = (float (*)[3])aObj[ o_unvph]->Free();
       usp->vT = (float *)aObj[ o_unvt]->Free();
       usp->vP = (float *)aObj[ o_unvp]->Free();
       usp->vV = (float *)aObj[ o_unvv]->Free();
       usp->quanCx = (short (*)[4])aObj[ o_unqucx]->Free();
       usp->quanCv = (float (*)[4])aObj[ o_unqucv]->Free();
  }


  if(usp->PsGen[0] == S_ON  && usp->Pa_f_mol == S_ON)
  {
    usp->m_t_lo = (float *)aObj[ o_unm_t_lo]->Alloc( usp->N, 1, F_);
    usp->m_t_up = (float *)aObj[ o_unm_t_up]->Alloc( usp->N, 1, F_);
  } else
     {
       usp->m_t_lo = (float *)aObj[ o_unm_t_lo]->Free();
      usp->m_t_up = (float *)aObj[ o_unm_t_up]->Free();
    }

  if( usp->PsGen[0] == S_ON  && usp->Ls )
       usp->vFug = (double *)aObj[ o_unvfug]->Alloc(usp->Q, usp->Ls, D_);
  else
       usp->vFug = (double *)aObj[ o_unvfug]->Free();

  if( usp->PsGen[0] == S_ON  && usp->Ls && usp->Pa_f_fug== S_ON )
  {
    usp->fug_lo = (float *)aObj[ o_unfug_lo]->Alloc( usp->Ls, 1, F_ );
    usp->fug_up = (float *)aObj[ o_unfug_up]->Alloc( usp->Ls, 1, F_ );
  }else
    {
       usp->fug_lo = (float *)aObj[ o_unfug_lo]->Free();
       usp->fug_up = (float *)aObj[ o_unfug_up]->Free();
    }

  if(usp->PsGen[1]== S_ON)
  {
    usp->Ss = (float (*)[2])aObj[ o_unss]->Alloc( usp->L, 2, F_ );
    usp->NgLs = (short *)aObj[ o_unngls]->Alloc( usp->L, 1, I_);
    usp->IntLs = (float (*)[2])aObj[ o_unintls]->Alloc( usp->L, 2, F_);
    usp->vS = (double *)aObj[ o_unsc]->Alloc( usp->Q, usp->L, D_);

  }else
   {
    usp->Ss = (float (*)[2])aObj[ o_unss]->Free();
    usp->NgLs = (short *)aObj[ o_unngls]->Free();
    usp->IntLs = (float (*)[2])aObj[ o_unintls]->Free();
    usp->vS = (double *)aObj[ o_unsc]->Free();
   }

  if(usp->PsGen[5]== S_ON)
  {
    usp->Vs = (float (*)[2])aObj[ o_unvs]->Alloc( usp->L, 2, F_ );
    usp->NgLv = (short *)aObj[ o_unnglv]->Alloc(usp->L, 1, I_);
    usp->IntLv = (float (*)[2])aObj[ o_unintlv]->Alloc(usp->L, 2, F_);
    usp->vmV = (double *)aObj[ o_unmvc]->Alloc( usp->Q, usp->L, D_);
  }else
    {
      usp->Vs = (float (*)[2])aObj[ o_unvs]->Free();
      usp->NgLv = (short *)aObj[ o_unnglv]->Free();
      usp->IntLv = (float (*)[2])aObj[ o_unintlv]->Free();
      usp->vmV = (double *)aObj[ o_unmvc]->Free();
    }

  if(usp->PsGen[2]== S_ON)
  {
    usp->NgNb = (short *)aObj[ o_unngnb]->Alloc(usp->N, 1, I_);
    usp->IntNb = (float (*)[2])aObj[ o_unintnb]->Alloc(usp->N, 2, F_);
    usp->Bs = (double (*)[2])aObj[ o_unbs]->Alloc(usp->N, 2, D_);
    usp->vB = (double *)aObj[ o_unbc]->Alloc( usp->Q, usp->N, D_);
  } else
    {
      usp->NgNb = (short *)aObj[ o_unngnb]->Free();
      usp->IntNb = (float (*)[2])aObj[ o_unintnb]->Free();
      usp->Bs = (double (*)[2])aObj[ o_unbs]->Free();
      usp->vB = (double *)aObj[ o_unbc]->Free();
     }

  if(usp->PsGen[6]== S_ON && usp->Ls )   // new by DK
  {
    usp->NgGam =(short *)aObj[ o_unnggam]->Alloc(usp->Ls, 1, I_);
    usp->IntGam = (float (*)[2])aObj[ o_unintgam]->Alloc(usp->Ls, 2, F_);
    usp->GAMs = (float (*)[2])aObj[ o_ungams]->Alloc(usp->Ls, 2, F_);
    usp->vNidP = (double *)aObj[ o_unnidp]->Alloc( usp->Q, usp->Ls, D_);
  } else
    {
    usp->NgGam =(short *)aObj[ o_unnggam]->Free();
    usp->IntGam = (float (*)[2])aObj[ o_unintgam]->Free();
    usp->GAMs = (float (*)[2])aObj[ o_ungams]->Free();
    usp->vNidP = (double *)aObj[ o_unnidp]->Free();
     }

  if(/*usp->PsGen[0??] == S_ON  &&*/ usp->Pa_f_pha == S_ON)
    usp->f_PhA = (short *)aObj[ o_unf_pha]->Alloc(usp->N, 1, I_);
   else
      usp->f_PhA = (short *)aObj[ o_unf_pha]->Free();

  if( q == 0)
     nG_dyn_new();                //!!!!!!!!!!!!!!!!!!

 //  internal
   usp->stl = (char (*)[EQ_RKLEN])aObj[ o_unstl]->Alloc( usp->Q, 1, EQ_RKLEN);
   if( usp->PsEqn != S_OFF )
    {
      usp->Expr = (char *)aObj[o_unexpr]->Alloc(1, 2048, S_ );
      usp->tprn = (char *)aObj[ o_untprn]->Alloc(1, 2048, S_ );
    }
    else
    {
      usp->Expr = (char *)aObj[o_unexpr]->Free();
      usp->tprn = (char *)aObj[ o_untprn]->Free();
    }
   if( usp->Nsd > 0 )
   {
        usp->sdref =(char (*)[V_SD_RKLEN])aObj[ o_unsdref]->Alloc(
                        usp->Nsd, 1, V_SD_RKLEN );
        usp->sdval =(char (*)[V_SD_VALEN])aObj[ o_unsdval]->Alloc(
                        usp->Nsd, 1, V_SD_VALEN );
    }
    else
    {
        usp->sdref = (char (*)[V_SD_RKLEN])aObj[ o_unsdref]->Free();
        usp->sdval = (char (*)[V_SD_VALEN])aObj[ o_unsdval]->Free();
    }

// graphics
   if( usp->PsGraph != S_OFF )
    {
      usp->lNam = (char (*)[MAXGRNAME])aObj[ o_unlnam ]->Alloc( 1,
                 usp->dimXY[1]+usp->dimEF[1], MAXGRNAME);
      usp->ExprGraph = (char *)aObj[ o_ungexpr ]->Alloc( 1, 2048, S_);
      usp->x0    = (double *)aObj[ o_unxa ]->Alloc(usp->dimXY[0], 1, D_);
      usp->y0    = (double *)aObj[ o_unyc ]->Alloc(
                      usp->dimXY[0], usp->dimXY[1], D_);
      usp->xE    = (float *)aObj[ o_unxs ]->Alloc(usp->dimEF[0], 1, F_);
      usp->yE    = (float *)aObj[ o_unys ]->Alloc(
                  usp->dimEF[0],usp->dimEF[1], F_);
      usp->tprn = (char *)aObj[ o_untprn]->Alloc(1, 2048, S_ );
    }
    else
    {
       usp->lNam = (char (*)[MAXGRNAME])aObj[ o_unlnam ]->Free();
       usp->ExprGraph = (char *)aObj[ o_ungexpr ]->Free();
       usp->x0    = (double *)aObj[ o_unxa ]->Free();
       usp->y0    = (double *)aObj[ o_unyc ]->Free();
       usp->xE    = (float *)aObj[ o_unxs ]->Free();
       usp->yE    = (float *)aObj[ o_unys ]->Free();
       plot  = (TPlotLine *)aObj[ o_unplline ]->Free();
       usp->dimXY[0] = 0;
       usp->dimEF[0] = 0;
     }

   if( q == 0)
   {  work_dyn_new();
      phase_lists_new();
    }
}


//set default information
void TUnSpace::set_def( int q)
{
    ErrorIf( usp!=&us[q], GetName(),
             "Invalid access to us in set_def.");

   TProfil *aPa= dynamic_cast<TProfil *>( aMod[RT_PARAM].get());

    strcpy( usp->name,  "`" );
    strcpy( usp->notes, "`" );
    memset( usp->stkey, 0, sizeof(char)*(EQ_RKLEN+10) );
   memcpy( &usp->PunE, aPa->pa.TPpdc, 4 );
   usp->Gstat = GS_INDEF;
   usp->Astat = AS_INDEF;
   memcpy( &usp->PsUnInt, "%%AA+-------++", 14 );
   memcpy( &usp->Pa_f_pha, "------0A1+", 10 );
   usp->Pa_Crit = UNSP_CRIT_PA;
   memcpy( &usp->PvPOM, "+-+---+-", 8 );

   memset( &usp->N, 0, 36*sizeof(int));
   memset( usp->T, 0, 22*sizeof(float));
   usp->quan_lev = 0.05f;

    usp->Q = 111;
    usp->nGB = 1;
    usp->UnICn = 0;
    usp->UgDCn = 0;
    usp->UaDCn = 0;
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
    usp->IntLg = 0;
    usp->IntGam = 0;
    usp->IntLs = 0;
    usp->IntLv = 0;
    usp->IntNb = 0;
    usp->m_t_lo = 0;
    usp->m_t_up = 0;
    usp->fug_lo = 0;
    usp->fug_up = 0;
    usp->Gs = 0;
    usp->Ss = 0;
    usp->Vs = 0;
    usp->GAMs = 0;
    usp->ncp = 0;
    usp->Bs = 0;
    usp->vG = 0;
    usp->vB = 0;
    usp->vS = 0;
    usp->vmV = 0;
    usp->vNidP = 0;

    usp->vY = 0;
    usp->vYF = 0;
    usp->vGam = 0;
    usp->vMol = 0;
    usp->vU = 0;
    usp->vFug = 0;
    usp->vT = 0;
    usp->vP = 0;
    usp->vV = 0;
    usp->vpH = 0;
    usp->OVB = 0;
    usp->OVR = 0;
    usp->OVN = 0;
    usp->quanCx = 0;
    usp->quanCv = 0;
    usp->SGp = 0;

 //  internal
    usp->stl = 0;
    usp->Expr = 0;
    usp->sdref = 0;
    usp->sdval = 0;
    usp->tprn = 0;
    // graphics
    strcpy( usp->xNames, "x" );
    strcpy( usp->yNames, "y" );
    usp->dimEF[1] = 5;
    usp->dimEF[0] = 0;
    usp->dimXY[1] = 3;
    usp->dimXY[0] = 0;
    usp->lNam = 0;
    usp->ParNames = 0;
    usp->ExprGraph = 0;
    usp->x0    = 0;
    usp->y0    = 0;
    usp->xE    = 0;
    usp->yE    = 0;
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
bool TUnSpace::check_input( const char* /*key*/, int /*Level*/ )
{
  char pkey[MAXRKEYLEN+10];
  if( !pVisor->ProfileMode )
        return true;

  TProfil* PRof = dynamic_cast<TProfil *>( aMod[RT_PARAM].get());
  //Get base SysEq key from UnSpace key
  rt[RT_SYSEQ]->MakeKey( RT_UNSPACE, pkey, RT_UNSPACE, 0, RT_UNSPACE, 1,
                           RT_UNSPACE, 2, RT_UNSPACE, 3, RT_UNSPACE, 4,
                           RT_UNSPACE, 5, RT_UNSPACE, 6, RT_UNSPACE, 7, K_END);
    // read SysEq record and unpack data
    PRof->loadSystat( pkey );
    // test changes in system after unspace calc
    if( rt[RT_SYSEQ]->Rtime() > rt[nRT]->Rtime() )
        return true;
    return false;
}

// Input nessasery data and links objects
void TUnSpace::RecInput( const char *key )
{
    TCModule::RecInput( key );
}

/* opens window with 'Remake record' parameters
*/
void
TUnSpace::MakeQuery()
{
//    pImp->MakeQuery();
    const char * p_key;
    char flgs[38];
    int size[10];

    p_key  = db->PackKey();
    memcpy( flgs, &usp->PunE, 38);
    size[0] = usp->Q;
    size[1] = (int)(usp->quan_lev*100);
    size[2] = usp->nGB;
    size[3] = usp->nGN;
    size[4] = usp->nGR;
    size[5] = usp->Nsd;
    size[6] = usp->dimXY[1]; // must be 3
    size[7] = usp->dimEF[0];
    size[8] = usp->dimEF[1]; // must be 5
    if( !vfUnSpaceSet( window(), p_key, flgs, size ))
         Error( p_key, "UnSpace record configuration cancelled by the user!" );
     //  return;   // cancel

    memcpy( &usp->PunE, flgs, 38);

    usp->Q = (short)size[0];
    usp->quan_lev = (float)size[1]/100.;
    usp->nGB = (short)size[2];
    usp->nGN = (short)size[3];
    usp->nGR = (short)size[4];
    usp->Nsd = (short)size[5];
    usp->dimXY[1] = (short)size[6]; // must be 3
    usp->dimEF[0] = (short)size[7];
    usp->dimEF[1] = (short)size[8]; // must be 5
}

//Rebild record structure before calc
int
TUnSpace::RecBuild( const char *key, int mode  )
{
  bool set_def = false;

  if( !pVisor->ProfileMode )
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
   usp->Gstat = GS_INDEF;
   usp->Astat = AS_INDEF;

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

   syu = TSyst::sm->GetSY();
   pmu = TMulti::sm->GetPM();
   tpu = TMTparm::sm->GetTP();

//    TProfil* PRof = dynamic_cast<TProfil *>( aMod[RT_PARAM].get());
    if( !pVisor->ProfileMode )
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
    {  nAdapt = (usp->Pa_Adapt-'0');
       usp->Gstat = GS_INDEF; // for Adapt mode need   buildTestedArrays
                                   // for each cicle
    }

    if( usp->Gstat != GS_DONE )
          init_generation();

   init_analyse();
   if( usp->PsGraph != S_OFF )
       text_analyze( o_ungexpr );

//   pVisor->Message( window(), GetName(),
//             "Generation of EqStat records\n"
//                 "Please, wait...", 0, usp->Q);

    for( int ii=0; ii<nAdapt; ii++ )
    {
      if( usp->Gstat != GS_DONE )
      {
          usp->Gstat = GS_GOING;
          usp->Astat = AS_INDEF;
          buildTestedArrays();
      }
      aMod[RT_UNSPACE]->ModUpdate("UnSpace data analysing in progress...");
      if( usp->PsGen[0] == S_ON )
      {
        analyseArrays();
        if( usp->PsGraph != S_OFF )
           calc_graph();

        if( usp->Pa_Adapt > '1')
           AdapG();                    // !!!! test Kostin beak ob =0 or ob>Q*0.95
      }

    }

  if( usp->Pa_Adapt > '1')
    usp->Pa_Adapt = '1';
  usp->Gstat = GS_DONE;   // for Adapt mode need   buildTestedArrays
                                 // for each cicle
  usp->Astat = AS_DONE;
//  aMod[RT_UNSPACE].ModUpdate("GtDemo data sampling in progress...");

  pVisor->CloseMessage();

  if( TSyst::sm->GetSY()->Guns )
          fillValue( TSyst::sm->GetSY()->Guns, 0., TRMults::sm->GetMU()->L );
  if( TSyst::sm->GetSY()->Vuns )
          fillValue( TSyst::sm->GetSY()->Vuns, 0., TRMults::sm->GetMU()->L );

// Get startup syseq record for fitting
  //Get base SysEq key from UnSpace key
  char pkey[MAXRKEYLEN+10];
  rt[RT_SYSEQ]->MakeKey( RT_UNSPACE, pkey, RT_UNSPACE, 0, RT_UNSPACE, 1,
                           RT_UNSPACE, 2, RT_UNSPACE, 3, RT_UNSPACE, 4,
                           RT_UNSPACE, 5, RT_UNSPACE, 6, RT_UNSPACE, 7, K_END);
    // read SysEq record and unpack data
  TProfil::pm->loadSystat( pkey );

  TCModule::RecCalc(key);
}

const char* TUnSpace::GetHtml()
{
   return GM_UNSPACE_HTML;
}

void
TUnSpace::RecordPrint( const char* key )
{
    int res = vfQuestion3(window(), "Question",
                    "Will you produce input files for standalone UnSpace (Yes) or use print script (No)?",
		       "Yes", "No", "Cancel");
	if( res == VF3_3 )
	    return;

	if( res == VF3_1 )
	{
                std::string filename;
		if( vfChooseFileSave(window(), filename,
				   "Please, enter the Unspace work structure file name", "*.unc" ) )
		{
		    if( !access(filename.c_str(), 0 ) ) //file exists
		        if( !vfQuestion( window(), filename.c_str(),
		        		"This file exists! Overwrite?") )
                   return;
            fstream ff( filename, ios::out );
            ErrorIf( !ff.good() , filename, "Fileopen error");
	        to_text_file( ff, true );

	        filename +=".res";
            fstream ff1( filename, ios::out );
            ErrorIf( !ff1.good() , filename, "Fileopen error");
	        result_to_text_file( ff1, true );
		}
		TProfil::pm->makeGEM2MTFiles( window() );
		//	     na->GEM_init( f_name.c_str(), 0, true );
	}
	else
	     TCModule::RecordPrint( key );
}
