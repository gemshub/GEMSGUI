//-------------------------------------------------------------------
// $Id: m_gem2mt.cpp 1409 2009-08-21 15:34:43Z gems $
//
// Implementation of TGEM2MT class, config and calculation functions
//
// Copyright (C) 2005 S.Dmytriyeva, D.Kulik
//
// This file is part of a GEM-Selektor library for thermodynamic
// modelling by Gibbs energy minimization
// Uses: GEM-Vizor GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//
const char *GEMS_MT_HTML = "gm_gem2mt";

#include <math.h>
#include <stdio.h>

#ifndef __unix
#include <io.h>
#endif

#include "m_gem2mt.h"
#include "nodearray.h"
#include "v_object.h"
#include "visor.h"
#include "m_syseq.h"
#include "m_param.h"
#include "service.h"

#include "v_mod.h"

TGEM2MT* TGEM2MT::pm;

TGEM2MT::TGEM2MT( int nrt ):
        TCModule( nrt )
{
    nQ = 1;
    aFldKeysHelp.Add("Modelling project name");
    aFldKeysHelp.Add("Thermodynamic potential to minimize {G}");
    aFldKeysHelp.Add("Identifier of the parent chemical system (CSD)");
    aFldKeysHelp.Add("CSD variant number");
    aFldKeysHelp.Add("Volume of the system (L) or 0 (no volume constraint)");
    aFldKeysHelp.Add("Pressure P in bar or 0 for Psat (H2O)");
    aFldKeysHelp.Add("Temperature (deg. C)");
    aFldKeysHelp.Add("Variant number of thermodynamic pressure-temperature data");
    aFldKeysHelp.Add("Identifier of this coupled model simulator definition");
    aFldKeysHelp.Add("Batch simulator type code, a combination of {}");

    mtp=&mt[0];
    set_def();
    start_title =
       " Definition of a GEM2MT (Coupled model)";

  na = 0;
  pa = 0;
}

// get key of record
gstring
TGEM2MT::GetKeyofRecord( const char *oldKey, const char *strTitle,
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
        rt[RT_GEM2MT].SetKey(str.c_str());
        rt[RT_SYSEQ].MakeKey( RT_GEM2MT, pkey, RT_GEM2MT, 0, RT_GEM2MT, 1,
                               RT_GEM2MT, 2, RT_GEM2MT, 3, RT_GEM2MT, 4,
                               RT_GEM2MT, 5, RT_GEM2MT, 6, RT_GEM2MT, 7, K_END);
        str = TSysEq::pm->GetKeyofRecord( pkey,
          "Please, select a parent System for a new GEM2MT ", KEY_OLD );
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
    rt[RT_GEM2MT].SetKey(str.c_str());
    keyTest( str.c_str() );
    return str;
}

// test GEM2MT key to calc mode
void TGEM2MT::keyTest( const char *key )
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
        rt[RT_SYSEQ].MakeKey( RT_GEM2MT, pkey, RT_GEM2MT, 0, RT_GEM2MT, 1,
                               RT_GEM2MT, 2, RT_GEM2MT, 3, RT_GEM2MT, 4,
                               RT_GEM2MT, 5, RT_GEM2MT, 6, RT_GEM2MT, 7, K_END);
        if( rt[RT_SYSEQ].Find(pkey) <0 )
            Error( key, "E07PErem: Illegal record key (no system)!");
    }
}

// link values to objects
void TGEM2MT::ods_link(int q)
{
    mtp=&mt[q];

// static
    aObj[o_mtpufl].SetPtr( &mtp->PunE );   /* a4 */
aObj[o_mtpvfl].SetPtr( &mtp->PvICi );  /* a10*/
    aObj[o_mtpsfl].SetPtr( &mtp->PsMode ); /* a8 */
    aObj[o_mtcipf].SetPtr( &mtp->nC );     /* i8 */
    aObj[o_mtszt].SetPtr( &mtp->Lbi );     /* i8 */
    aObj[o_mtnsne].SetPtr( &mtp->ntM );   /* i4 */
    aObj[o_mtptai].SetPtr( &mtp->nPai );  /* i3 */
    aObj[o_mtchbr].SetPtr( &mtp->nICb );  /* i5 */
    aObj[o_mttmi].SetPtr( mtp->tmi );     /* i3 */
    aObj[o_mtnvi].SetPtr( mtp->NVi );     /* i3 */
    aObj[o_mtaxis].SetPtr( mtp->axisType ); /* i6 */
    aObj[o_mtpai].SetPtr( mtp->Pai );     /* f4 */
    aObj[o_mttai].SetPtr( mtp->Tai );     /* f4 */
    aObj[o_mttau].SetPtr( mtp->Tau );     /* f3 */
    aObj[o_mtsize].SetPtr( mtp->size[0] );     /* f8 */
aObj[o_mtsizelc].SetPtr( mtp->sizeLc );     /* f3 */
// work
    aObj[o_mtsykey].SetPtr( mtp->sykey );
    aObj[o_mtctm].SetPtr( &mtp->ctm );
    aObj[o_mtcnv].SetPtr( &mtp->cnv );
    aObj[o_mtqc].SetPtr( &mtp->qc );
    aObj[o_mtkv].SetPtr( &mtp->kv );
    aObj[o_mtjqc].SetPtr( &mtp->jqc );
    aObj[o_mtjqs].SetPtr( &mtp->jqs );
    aObj[o_mtjt].SetPtr( &mtp->jt );
    aObj[o_mtjdd].SetPtr( &mtp->jdd );
    aObj[o_mtjdi].SetPtr( &mtp->jdi );
    aObj[o_mtide].SetPtr( &mtp->ide );
    aObj[o_mtct_].SetPtr( &mtp->ct );
    aObj[o_mtrei5].SetPtr( &mtp->rei5 );
    aObj[o_mtct].SetPtr( &mtp->cT );
    aObj[o_mtcp].SetPtr( &mtp->cP );
    aObj[o_mtcv].SetPtr( &mtp->cV );
    aObj[o_mtctau].SetPtr( &mtp->cTau );
    aObj[o_mtdtau].SetPtr( &mtp->dTau );
    aObj[o_mtotau].SetPtr( &mtp->oTau );
    aObj[o_mtdx].SetPtr( &mtp->dx );
    aObj[o_mtref2].SetPtr( &mtp->TimeGEM );
    aObj[o_mtref3].SetPtr( &mtp->ref3 );
    aObj[o_mtref4].SetPtr( &mtp->ref4 );

// DBase  58
    aObj[o_mtname].SetPtr(  mtp->name );
    aObj[o_mtnotes].SetPtr(   mtp->notes );
aObj[o_mtflag].SetPtr( &mtp->PunE );    /* a22 */
    aObj[o_mtshort].SetPtr( &mtp->nC );     /* i40 */
    aObj[o_mtworks].SetPtr( &mtp->ctm );     /* i12 */
    aObj[o_mtworkf].SetPtr( &mtp->cT );     /* f10 */
    aObj[o_mtdoudl].SetPtr( &mtp->Msysb );    /* d9 */
    aObj[o_mtadpar].SetPtr( &mtp->fVel );    /* d11 */
aObj[o_mtfloat].SetPtr( mtp->Pai );    /* f22 */
    aObj[ o_mtxnames].SetPtr(  mtp->xNames );
    aObj[ o_mtynames].SetPtr(  mtp->yNames );

// dynamic
    aObj[ o_mtlnam].SetPtr( mtp->lNam[0] );
    aObj[ o_mtlnam].SetDim( 1, mtp->nYS );
    aObj[ o_mtlname].SetPtr( mtp->lNamE[0] );
    aObj[ o_mtlname].SetDim( 1, mtp->nYE );
    aObj[o_mttexpr].SetPtr( mtp->tExpr );
    //aObj[o_mttexpr].SetDim(1,len(mtp->tExpr));
    aObj[o_mtgexpr].SetPtr( mtp->gExpr);
    //aObj[o_mtgexpr].SetDim(1,l(mtp->gExpr));
    aObj[ o_mtsdref].SetPtr( mtp->sdref);
    aObj[ o_mtsdref].SetDim( mtp->Nsd, 1 );
    aObj[ o_mtsdval].SetPtr( mtp->sdval);
    aObj[ o_mtsdval].SetDim( mtp->Nsd, 1 );
    aObj[ o_mtdicp].SetPtr( mtp->DiCp);
    aObj[ o_mtdicp].SetDim( mtp->nC, 2 );
    aObj[ o_mtfdli].SetPtr( mtp->FDLi);
    aObj[ o_mtfdli].SetDim( mtp->nFD, 2 );
    aObj[ o_mtpi].SetPtr( mtp->Pi);
    aObj[ o_mtpi].SetDim( mtp->nIV, 1 );
    aObj[ o_mtti].SetPtr( mtp->Ti);
    aObj[ o_mtti].SetDim( mtp->nIV, 1 );
    aObj[ o_mtvi].SetPtr( mtp->Vi);
    aObj[ o_mtvi].SetDim( mtp->nIV, 1 );
    aObj[ o_mtxet].SetPtr( mtp->xEt);
    aObj[ o_mtxet].SetDim( mtp->nE, 1 );
    aObj[ o_mtyet].SetPtr( mtp->yEt);
    aObj[ o_mtyet].SetDim( mtp->nE, mtp->nYE );
    aObj[ o_mtbn].SetPtr( mtp->Bn);
    aObj[ o_mtbn].SetDim( mtp->nIV, mtp->Nb );
    aObj[ o_mthydp].SetPtr( mtp->HydP);
    aObj[ o_mthydp].SetDim( mtp->nC, SIZE_HYDP );
    aObj[ o_mtqpi].SetPtr( mtp->qpi);
    aObj[ o_mtqpi].SetDim( mtp->Nqpt, 1 );
    aObj[ o_mtqpc].SetPtr( mtp->qpc);
    aObj[ o_mtqpc].SetDim( mtp->Nqpg, 1 );
    aObj[ o_mtxt].SetPtr( mtp->xt);
    aObj[ o_mtxt].SetDim( mtp->nC, 1 );
    aObj[ o_mtyt].SetPtr( mtp->yt);
    aObj[ o_mtyt].SetDim( mtp->nC, mtp->nYS );
    aObj[ o_mtcib].SetPtr( mtp->CIb);
    aObj[ o_mtcib].SetDim( mtp->nIV, mtp->Nb );
    aObj[ o_mtcab].SetPtr( mtp->CAb);
    aObj[ o_mtcab].SetDim( mtp->nIV, mtp->Lbi );
    aObj[ o_mtfdlf].SetPtr( mtp->FDLf);
    aObj[ o_mtfdlf].SetDim( mtp->nFD, 4 );
    aObj[ o_mtpgt].SetPtr( mtp->PGT);
    aObj[ o_mtpgt].SetDim( mtp->FIf, mtp->nPG );
    aObj[ o_mttval].SetPtr( mtp->Tval);
    aObj[ o_mttval].SetDim( mtp->nTai, 1 );
    aObj[ o_mtpval].SetPtr( mtp->Pval);
    aObj[ o_mtpval].SetDim( mtp->nPai, 1 );
    aObj[ o_mtnam_i].SetPtr( mtp->nam_i );
    aObj[ o_mtnam_i].SetDim(  mtp->nIV, 1 );
    aObj[ o_mtfor_i].SetPtr( mtp->for_i );
    aObj[ o_mtfor_i].SetDim(  mtp->Lbi, 1 );
    aObj[ o_mtstld].SetPtr( mtp->stld );
    aObj[ o_mtstld].SetDim(  mtp->nIV, 1 );
    aObj[ o_mtciclb].SetPtr( mtp->CIclb );
    aObj[ o_mtciclb].SetDim(  1, mtp->Nb );
    aObj[ o_mtaucln].SetPtr( mtp->AUcln );
    aObj[ o_mtaucln].SetDim(  mtp->Lbi, 1 );
    aObj[ o_mtfdlid].SetPtr( mtp->FDLid );
    aObj[ o_mtfdlid].SetDim(  mtp->nFD, 1 );
    aObj[ o_mtfdlop].SetPtr( mtp->FDLop );
    aObj[ o_mtfdlop].SetDim(  mtp->nFD, 1 );
    aObj[ o_mtfdlmp].SetPtr( mtp->FDLmp );
    aObj[ o_mtfdlmp].SetDim(  mtp->nFD, 1 );
    aObj[ o_mtmpgid].SetPtr( mtp->MPGid );
    aObj[ o_mtmpgid].SetDim(  mtp->nPG, 1 );
    aObj[ o_mtumpg].SetPtr( mtp->UMPG );
    aObj[ o_mtumpg].SetDim( mtp->FIf, 1 );
    aObj[ o_mtbm].SetPtr( mtp->SBM );
    aObj[ o_mtbm].SetDim(  1, mtp->Nb );
    aObj[ o_mtplline].SetPtr( plot );
    aObj[ o_mtplline].SetDim( mtp->nYS+mtp->nYE,  sizeof(TPlotLine));
    //added 30/11/2005
    aObj[ o_mtbsf].SetPtr( mtp->BSF);
    aObj[ o_mtbsf].SetDim( mtp->nSFD, mtp->Nf );
    aObj[ o_mtmb].SetPtr( mtp->MB);
    aObj[ o_mtmb].SetDim( mtp->nC, mtp->Nf );
    aObj[ o_mtdmb].SetPtr( mtp->dMB);
    aObj[ o_mtdmb].SetDim( mtp->nC, mtp->Nf  );
    //added 07/12/2005
    aObj[ o_mtddc].SetPtr( mtp->DDc);
    aObj[ o_mtddc].SetDim( mtp->Lsf, 1 );
    aObj[ o_mtdic].SetPtr( mtp->DIc);
    aObj[ o_mtdic].SetDim( mtp->Nf, 1 );
    aObj[ o_mtdel].SetPtr( mtp->DEl);
    aObj[ o_mtdel].SetDim( mtp->nEl, 1 );
    aObj[ o_mtfor_e].SetPtr( mtp->for_e);
    aObj[ o_mtfor_e].SetDim( mtp->nEl, 1 );
     //added 13/12/2005
    aObj[o_mt_xic].SetPtr( mtp->xIC );
    aObj[o_mt_xic].SetDim( mtp->nICb, 1 );
    aObj[o_mt_xdc].SetPtr( mtp->xDC );
    aObj[o_mt_xdc].SetDim( mtp->nDCb, 1 );
    aObj[o_mt_xph].SetPtr( mtp->xPH );
    aObj[o_mt_xph].SetDim( mtp->nPHb, 1 );
//added 28/03/2006
aObj[o_mtgrid].SetPtr( mtp->grid[0] );
aObj[o_mtgrid].SetDim( mtp->nC, 3 );
aObj[o_mtnpmean].SetPtr( mtp->NPmean );
aObj[o_mtnpmean].SetDim( mtp->nPTypes, 1 );
aObj[o_mtnpmin].SetPtr( mtp->nPmin );
aObj[o_mtnpmin].SetDim( mtp->nPTypes, 1 );
aObj[o_mtnpmax].SetPtr( mtp->nPmax );
aObj[o_mtnpmax].SetDim( mtp->nPTypes, 1 );
aObj[o_mtpartd].SetPtr( mtp->ParTD );
aObj[o_mtpartd].SetDim( mtp->nPTypes, 6 );

// work
    aObj[ o_mtan].SetPtr(mtp->An);
//    aObj[ o_mtan].SetDim( dtp->nK, dtp->Nb );
    aObj[ o_mtan].SetDim( mtp->Lbi, mtp->Nb );
    aObj[ o_mtae].SetPtr(mtp->Ae);
    aObj[ o_mtae].SetDim( mtp->nEl, mtp->Nb );

    aObj[ o_mtgc].SetPtr(mtp->gfc);
    aObj[ o_mtgc].SetDim( mtp->nC*mtp->nPG, mtp->Nf );

    aObj[ o_mwetext].SetPtr( mtp->etext );
    // aObj[ o_mwetext].SetDim(1, 0 );
    aObj[ o_mwtprn].SetPtr(  mtp->tprn  );
    // aObj[ o_mwtprn].SetDim( 1,l( mtp->tprn));

}

// set dynamic Objects ptr to values
void TGEM2MT::dyn_set(int q)
{
    ErrorIf( mtp!=&mt[q], GetName(),
       "E06GDrem: Illegal access to mt in dyn_set");
    // Change MAXGRNAME from 7 to 16
    if( mtp->PvMSg != S_OFF && aObj[ o_mtlnam ].GetType() == 7  )
    {
      mtp->lNam = (char (*)[MAXGRNAME])aObj[ o_mtlnam ].Alloc(
                    1, mtp->nYS, MAXGRNAME);
    }
    else
     mtp->lNam = ( char (*)[MAXGRNAME] )aObj[ o_mtlnam].GetPtr();

    if( mtp->PvEF != S_OFF && aObj[ o_mtlname ].GetType() == 7  )
    {
        mtp->lNamE = (char (*)[MAXGRNAME])aObj[ o_mtlname ].Alloc(
                              1, mtp->nYE, MAXGRNAME);
    }
    else
      mtp->lNamE = (char (*)[MAXGRNAME])aObj[ o_mtlname].GetPtr();

    mtp->tExpr = (char *)aObj[o_mttexpr].GetPtr();
    mtp->gExpr = (char *)aObj[o_mtgexpr].GetPtr();
    mtp->sdref = (char (*)[V_SD_RKLEN])aObj[ o_mtsdref].GetPtr();
    mtp->sdval = (char (*)[V_SD_VALEN])aObj[ o_mtsdval].GetPtr();
    mtp->DiCp = (short (*)[2])aObj[ o_mtdicp].GetPtr();
    mtp->FDLi = (short (*)[2])aObj[ o_mtfdli].GetPtr();
    mtp->Pi = (float *)aObj[ o_mtpi].GetPtr();
    mtp->Ti = (float *)aObj[ o_mtti].GetPtr();
    mtp->Vi = (float *)aObj[ o_mtvi].GetPtr();
    mtp->xEt = (float *)aObj[ o_mtxet].GetPtr();
    mtp->yEt = (float *)aObj[ o_mtyet].GetPtr();
    mtp->Bn = (double *)aObj[ o_mtbn].GetPtr();
    mtp->HydP = (double (*)[SIZE_HYDP])aObj[ o_mthydp].GetPtr();
    mtp->qpi = (double *)aObj[ o_mtqpi].GetPtr();
    mtp->qpc = (double *)aObj[ o_mtqpc].GetPtr();
    mtp->xt = (double *)aObj[ o_mtxt].GetPtr();
    mtp->yt = (double *)aObj[ o_mtyt].GetPtr();
    mtp->CIb = (float *)aObj[ o_mtcib].GetPtr();
    mtp->CAb = (float *)aObj[ o_mtcab].GetPtr();
    mtp->FDLf = (float (*)[4])aObj[ o_mtfdlf].GetPtr();
    mtp->PGT = (float *)aObj[ o_mtpgt].GetPtr();
    mtp->Tval = (float *)aObj[ o_mttval].GetPtr();
    mtp->Pval = (float *)aObj[ o_mtpval].GetPtr();
    mtp->nam_i = (char (*)[MAXIDNAME])aObj[ o_mtnam_i].GetPtr();
    mtp->for_i = (char (*)[MAXFORMUNITDT])aObj[ o_mtfor_i].GetPtr();
    mtp->stld = (char (*)[EQ_RKLEN])aObj[ o_mtstld].GetPtr();
    mtp->CIclb = (char *)aObj[ o_mtciclb].GetPtr();
    mtp->AUcln = (char *)aObj[ o_mtaucln].GetPtr();
    mtp->FDLid = (char (*)[MAXSYMB])aObj[ o_mtfdlid].GetPtr();
    mtp->FDLop = (char (*)[MAXSYMB])aObj[ o_mtfdlop].GetPtr();
    mtp->FDLmp = (char (*)[MAXSYMB])aObj[ o_mtfdlmp].GetPtr();
    mtp->MPGid = (char (*)[MAXSYMB])aObj[ o_mtmpgid].GetPtr();
    mtp->UMPG = (char *)aObj[ o_mtumpg].GetPtr();
    mtp->SBM = (char (*)[MAXICNAME+MAXSYMB])aObj[ o_mtbm].GetPtr();
    plot = (TPlotLine *)aObj[ o_mtplline].GetPtr();
   //added 30/11/2005
   mtp->BSF = (double *)aObj[ o_mtbsf].GetPtr();
   mtp->MB = (double *)aObj[ o_mtmb].GetPtr();
   mtp->dMB = (double *)aObj[ o_mtdmb].GetPtr();
   //added 07/12/2005
   mtp->DDc = (float*)aObj[ o_mtddc].GetPtr();
   mtp->DIc = (float*)aObj[ o_mtdic].GetPtr();
   mtp->DEl = (float*)aObj[ o_mtdel].GetPtr();
   mtp->for_e = (char (*)[MAXFORMUNITDT])aObj[ o_mtfor_e].GetPtr();
   //added 13/12/2005
   mtp->xIC = (short *)aObj[o_mt_xic].GetPtr();
   mtp->xDC = (short *)aObj[o_mt_xdc].GetPtr();
   mtp->xPH = (short *)aObj[o_mt_xph].GetPtr();
   //added 28/03/2006
mtp->grid = (float (*)[3])aObj[o_mtgrid].GetPtr();
mtp->NPmean = (short *)aObj[o_mtnpmean].GetPtr();
mtp->nPmin = (short *)aObj[o_mtnpmin].GetPtr();
mtp->nPmax = (short *)aObj[o_mtnpmax].GetPtr();
mtp->ParTD = (short (*)[6])aObj[o_mtpartd].GetPtr();

// work
    mtp->An = (double *)aObj[ o_mtan].GetPtr();
mtp->Ae = (double *)aObj[ o_mtae].GetPtr();
    mtp->gfc = (double *)aObj[ o_mtgc].GetPtr();
    mtp->etext = (char *)aObj[ o_mwetext].GetPtr();
    mtp->tprn = (char *)aObj[ o_mwtprn].GetPtr();
}

// free dynamic memory in objects and values
void TGEM2MT::dyn_kill(int q)
{
    ErrorIf( mtp!=&mt[q], GetName(),
       "E05DTrem: Illegal access to mt in dyn_kill");

    mtp->lNam = (char (*)[MAXGRNAME])aObj[ o_mtlnam].Free();
    mtp->lNamE = (char (*)[MAXGRNAME])aObj[ o_mtlname].Free();
    mtp->tExpr = (char *)aObj[o_mttexpr].Free();
    mtp->gExpr = (char *)aObj[o_mtgexpr].Free();
    mtp->sdref = (char (*)[V_SD_RKLEN])aObj[ o_mtsdref].Free();
    mtp->sdval = (char (*)[V_SD_VALEN])aObj[ o_mtsdval].Free();
    mtp->DiCp = (short (*)[2])aObj[ o_mtdicp].Free();
    mtp->FDLi = (short (*)[2])aObj[ o_mtfdli].Free();
    mtp->Pi = (float *)aObj[ o_mtpi].Free();
    mtp->Ti = (float *)aObj[ o_mtti].Free();
    mtp->Vi = (float *)aObj[ o_mtvi].Free();
    mtp->xEt = (float *)aObj[ o_mtxet].Free();
    mtp->yEt = (float *)aObj[ o_mtyet].Free();
    mtp->Bn = (double *)aObj[ o_mtbn].Free();
    mtp->HydP = (double (*)[SIZE_HYDP])aObj[ o_mthydp].Free();
    mtp->qpi = (double *)aObj[ o_mtqpi].Free();
    mtp->qpc = (double *)aObj[ o_mtqpc].Free();
    mtp->xt = (double *)aObj[ o_mtxt].Free();
    mtp->yt = (double *)aObj[ o_mtyt].Free();
    mtp->CIb = (float *)aObj[ o_mtcib].Free();
    mtp->CAb = (float *)aObj[ o_mtcab].Free();
    mtp->FDLf = (float (*)[4])aObj[ o_mtfdlf].Free();
    mtp->PGT = (float *)aObj[ o_mtpgt].Free();
    mtp->Tval = (float *)aObj[ o_mttval].Free();
    mtp->Pval = (float *)aObj[ o_mtpval].Free();
    mtp->nam_i = (char (*)[MAXIDNAME])aObj[ o_mtnam_i].Free();
    mtp->for_i = (char (*)[MAXFORMUNITDT])aObj[ o_mtfor_i].Free();
    mtp->stld = (char (*)[EQ_RKLEN])aObj[ o_mtstld].Free();
    mtp->CIclb = (char *)aObj[ o_mtciclb].Free();
    mtp->AUcln = (char *)aObj[ o_mtaucln].Free();
    mtp->FDLid = (char (*)[MAXSYMB])aObj[ o_mtfdlid].Free();
    mtp->FDLop = (char (*)[MAXSYMB])aObj[ o_mtfdlop].Free();
    mtp->FDLmp = (char (*)[MAXSYMB])aObj[ o_mtfdlmp].Free();
    mtp->MPGid = (char (*)[MAXSYMB])aObj[ o_mtmpgid].Free();
    mtp->UMPG = (char *)aObj[ o_mtumpg].Free();
    mtp->SBM = (char (*)[MAXICNAME+MAXSYMB])aObj[ o_mtbm].Free();
    plot = (TPlotLine *)aObj[ o_mtplline].Free();
   //added 30/11/2005
   mtp->BSF = (double *)aObj[ o_mtbsf].Free();
   mtp->MB = (double *)aObj[ o_mtmb].Free();
   mtp->dMB = (double *)aObj[ o_mtdmb].Free();
   //added 07/12/2005
   mtp->DDc = (float*)aObj[ o_mtddc].Free();
   mtp->DIc = (float*)aObj[ o_mtdic].Free();
   mtp->DEl = (float*)aObj[ o_mtdel].Free();
   mtp->for_e = (char (*)[MAXFORMUNITDT])aObj[ o_mtfor_e].Free();
   //added 13/12/2005
   mtp->xIC = (short *)aObj[o_mt_xic].Free();
   mtp->xDC = (short *)aObj[o_mt_xdc].Free();
   mtp->xPH = (short *)aObj[o_mt_xph].Free();
//added 28/03/2006
mtp->grid = (float (*)[3])aObj[o_mtgrid].Free();
mtp->NPmean = (short *)aObj[o_mtnpmean].Free();
mtp->nPmin = (short *)aObj[o_mtnpmin].Free();
mtp->nPmax = (short *)aObj[o_mtnpmax].Free();
mtp->ParTD = (short (*)[6])aObj[o_mtpartd].Free();

// work
    mtp->An = (double *)aObj[ o_mtan].Free();
    mtp->Ae = (double *)aObj[ o_mtae].Free();
    mtp->gfc = (double *)aObj[ o_mtgc].Free();
if( mtp->yfb) 
{	
	delete[] mtp->yfb;
    mtp->yfb = NULL;
}
if( tt ) 
{	
	delete[] tt;
    tt = NULL;
}

    mtp->etext = (char *)aObj[ o_mwetext].Free();
    mtp->tprn = (char *)aObj[ o_mwtprn].Free();
}

// realloc dynamic memory
void TGEM2MT::dyn_new(int q)
{
  ErrorIf( mtp!=&mt[q], GetName(),
      "E04DTrem: Illegal access to mt in dyn_new.");

 mtp->DiCp = (short (*)[2])aObj[ o_mtdicp].Alloc( mtp->nC, 2, I_);
//added 13/12/2005
 mtp->xIC = (short *)aObj[o_mt_xic].Alloc( mtp->nICb, 1, I_);
 mtp->xDC = (short *)aObj[o_mt_xdc].Alloc( mtp->nDCb, 1, I_);
 mtp->xPH = (short *)aObj[o_mt_xph].Alloc( mtp->nPHb, 1, I_);

 mtp->nam_i=(char (*)[MAXIDNAME])aObj[o_mtnam_i].Alloc( mtp->nIV, 1, MAXIDNAME);
 mtp->Pi = (float *)aObj[ o_mtpi].Alloc( mtp->nIV, 1, F_);
 mtp->Ti = (float *)aObj[ o_mtti].Alloc( mtp->nIV, 1, F_);
 mtp->Vi = (float *)aObj[ o_mtvi].Alloc( mtp->nIV, 1, F_);
 mtp->stld = (char (*)[EQ_RKLEN])aObj[ o_mtstld].Alloc( mtp->nIV, 1, EQ_RKLEN);

 mtp->Tval  = (float *)aObj[ o_mttval ].Alloc( mtp->nTai, 1, F_);
 mtp->Pval  = (float *)aObj[ o_mtpval ].Alloc( mtp->nPai, 1, F_);

 mtp->Bn = (double *)aObj[ o_mtbn].Alloc( mtp->nIV, mtp->Nb, D_);
 mtp->SBM = (char (*)[MAXICNAME+MAXSYMB])aObj[ o_mtbm].Alloc(
             1, mtp->Nb, MAXICNAME+MAXSYMB);

//   if( mtp->PsMode == GMT_MODE_A || mtp->PsMode == GMT_MODE_D || mtp->PsMode == GMT_MODE_T ||
//      mtp->PsMode == GMT_MODE_W || mtp->PsMode == GMT_MODE_V )
 {
     mtp->HydP = (double (*)[SIZE_HYDP])aObj[ o_mthydp].Alloc(
         mtp->nC,SIZE_HYDP, D_);
    }
//    else
//    {
//      mtp->HydP = (double (*)[SIZE_HYDP])aObj[ o_mthydp].Free();
//    }


 if( mtp->PvICi == S_OFF )
    {
      mtp->CIb = (float *)aObj[ o_mtcib].Free();
      mtp->CIclb = (char *)aObj[ o_mtciclb].Free();
    }
    else
    {
      mtp->CIb = (float *)aObj[ o_mtcib].Alloc( mtp->nIV, mtp->Nb, F_);
      mtp->CIclb = (char *)aObj[ o_mtciclb].Alloc(1, mtp->Nb, A_);
    }

if( mtp->PvAUi == S_OFF )
    {
      mtp->CAb = (float *)aObj[ o_mtcab].Free();
      mtp->for_i = (char (*)[MAXFORMUNITDT])aObj[ o_mtfor_i].Free();
      mtp->AUcln = (char *)aObj[ o_mtaucln].Free();
      mtp->An = (double *)aObj[ o_mtan].Free();
      mtp->Lbi = 0;
    }
    else
    {
      mtp->CAb = (float *)aObj[ o_mtcab ].Alloc( mtp->nIV, mtp->Lbi, F_);
      mtp->for_i = (char (*)[MAXFORMUNITDT])aObj[ o_mtfor_i].Alloc(
          1, mtp->Lbi, MAXFORMUNITDT);
      mtp->AUcln = (char *)aObj[ o_mtaucln].Alloc( 1, mtp->Lbi, A_);
      mtp->An = (double *)aObj[ o_mtan].Alloc( mtp->Lbi, mtp->Nb, D_ );
    }

if( mtp->PvFDL == S_OFF )
   {
     mtp->FDLi = (short (*)[2])aObj[ o_mtfdli].Free();
     mtp->FDLf = (float (*)[4])aObj[ o_mtfdlf].Free();
     mtp->FDLid = (char (*)[MAXSYMB])aObj[ o_mtfdlid].Free();
     mtp->FDLop = (char (*)[MAXSYMB])aObj[ o_mtfdlop].Free();
     mtp->FDLmp = (char (*)[MAXSYMB])aObj[ o_mtfdlmp].Free();
     mtp->nFD = 0;
  }
   else
   {
      mtp->FDLi = (short (*)[2])aObj[ o_mtfdli ].Alloc( mtp->nFD, 2, I_);
      mtp->FDLf = (float (*)[4])aObj[ o_mtfdlf ].Alloc( mtp->nFD, 4, F_);
      mtp->FDLid=(char (*)[MAXSYMB])aObj[o_mtfdlid].Alloc( mtp->nFD, 1, MAXSYMB);
      mtp->FDLop=(char (*)[MAXSYMB])aObj[o_mtfdlop].Alloc( mtp->nFD, 1, MAXSYMB);
      mtp->FDLmp = (char (*)[MAXSYMB])aObj[ o_mtfdlmp ].Alloc(
                    mtp->nFD, 1, MAXSYMB);
   }

 if( mtp->PvPGD == S_OFF )
   {
     mtp->PGT = (float *)aObj[ o_mtpgt].Free();
     mtp->MPGid = (char (*)[MAXSYMB])aObj[ o_mtmpgid].Free();
     mtp->UMPG = (char *)aObj[ o_mtumpg].Free();
     mtp->nPG = 0;
   }
   else
   {
      mtp->PGT  = (float *)aObj[ o_mtpgt ].Alloc( mtp->FIf, mtp->nPG, F_);
      mtp->MPGid = (char (*)[MAXSYMB])aObj[ o_mtmpgid ].Alloc(
                    1, mtp->nPG, MAXSYMB);
      mtp->UMPG = (char *)aObj[ o_mtumpg].Alloc( mtp->FIf, 1, A_);
   }

 if( mtp->PvSFL == S_OFF )
     mtp->BSF = (double *)aObj[ o_mtbsf].Free();
   else
     mtp->BSF = (double *)aObj[ o_mtbsf].Alloc( mtp->nSFD, mtp->Nf, D_);


 if( mtp->PvPGD != S_OFF && mtp->PvFDL != S_OFF )
   {
      mtp->MB = (double *)aObj[ o_mtmb].Alloc( mtp->nC, mtp->Nf, D_);
      mtp->dMB = (double *)aObj[ o_mtdmb].Alloc( mtp->nC, mtp->Nf, D_);
   }
   else
   {
      mtp->MB = (double *)aObj[ o_mtmb].Free();
      mtp->dMB = (double *)aObj[ o_mtdmb].Free();
   }

  //added 07/12/2005
   if( mtp->PsDDc == S_OFF )
     mtp->DDc = (float*)aObj[ o_mtddc].Free();
   else
     mtp->DDc = (float*)aObj[ o_mtddc].Alloc( mtp->Lsf, 1, F_);

   if( mtp->PsDIc == S_OFF )
     mtp->DIc = (float*)aObj[ o_mtdic].Free();
   else
     mtp->DIc = (float*)aObj[ o_mtdic].Alloc( mtp->Nf, 1, F_);

   if( mtp->nEl <= 0  )
   {
     mtp->DEl = (float*)aObj[ o_mtdel].Free();
     mtp->for_e = (char (*)[MAXFORMUNITDT])aObj[ o_mtfor_e].Free();
     mtp->Ae = (double*)aObj[ o_mtae].Free();
     mtp->nEl = 0;
   }
   else
   {
     mtp->DEl = (float*)aObj[ o_mtdel].Alloc( mtp->nEl, 1, F_);
     mtp->for_e = (char (*)[MAXFORMUNITDT])aObj[ o_mtfor_e].Alloc(
           mtp->nEl, 1, MAXFORMUNITDT);
     mtp->Ae = (double*)aObj[ o_mtae].Alloc( mtp->nEl, mtp->Nb, D_);
   }


//----------------------------------------------------------------
   if( mtp->Nqpt > 0  )
    mtp->qpi   = (double *)aObj[ o_mtqpi ].Alloc(mtp->Nqpt, 1, D_);
   else
    mtp->qpi = (double *)aObj[ o_mtqpi].Free();

   if( mtp->Nqpg > 0  )
    mtp->qpc   = (double *)aObj[ o_mtqpc ].Alloc(mtp->Nqpg, 1, D_);
   else
    mtp->qpc = (double *)aObj[ o_mtqpc].Free();

//   if( mtp->PvMSt == S_OFF )
      mtp->tExpr = (char *)aObj[o_mttexpr].Free();
//   else
//      mtp->tExpr = (char *)aObj[ o_mttexpr ].Alloc(1, 4096, S_);

    if( mtp->PvMSg == S_OFF )
    {
      mtp->lNam = (char (*)[MAXGRNAME])aObj[ o_mtlnam].Free();
      mtp->gExpr = (char *)aObj[o_mtgexpr].Free();
      mtp->xt = (double *)aObj[ o_mtxt].Free();
      mtp->yt = (double *)aObj[ o_mtyt].Free();
      plot = (TPlotLine *)aObj[ o_mtplline].Free();
/*      mtp->nS =  */  mtp->nYS = 0;
    }
    else
    {
      mtp->lNam = (char (*)[MAXGRNAME])aObj[ o_mtlnam ].Alloc(
                    1, mtp->nYS, MAXGRNAME);
      mtp->gExpr = (char *)aObj[ o_mtgexpr ].Alloc(1, 2048, S_);
      mtp->xt   = (double *)aObj[ o_mtxt ].Alloc(mtp->nC, 1, D_);
      mtp->yt   = (double *)aObj[ o_mtyt ].Alloc(mtp->nC, mtp->nYS, D_);
    }

    if( mtp->PvEF == S_OFF )
    {
      mtp->lNamE = (char (*)[MAXGRNAME])aObj[ o_mtlname].Free();
      mtp->xEt = (float *)aObj[ o_mtxet].Free();
      mtp->yEt = (float *)aObj[ o_mtyet].Free();
      mtp->nE = mtp->nYE = 0;
    }
    else
    {
        mtp->lNamE = (char (*)[MAXGRNAME])aObj[ o_mtlname ].Alloc(
                              1, mtp->nYE, MAXGRNAME);
        mtp->xEt   = (float *)aObj[ o_mtxet ].Alloc(mtp->nE, 1, F_);
        mtp->yEt   = (float *)aObj[ o_mtyet ].Alloc(mtp->nE, mtp->nYE, F_);
    }

//added 28/03/2006
    if( mtp->PvGrid == S_OFF )
      mtp->grid = (float (*)[3])aObj[o_mtgrid].Free();
    else
      mtp->grid = (float (*)[3])aObj[o_mtgrid].Alloc( mtp->nC, 3, F_ );

    if( mtp->PsMode == GMT_MODE_W || mtp->PsMode == GMT_MODE_V )
    {
      mtp->NPmean = (short *)aObj[o_mtnpmean].Alloc( mtp->nPTypes, 1, I_ );
      mtp->nPmin = (short *)aObj[o_mtnpmin].Alloc( mtp->nPTypes, 1, I_ );
      mtp->nPmax = (short *)aObj[o_mtnpmax].Alloc( mtp->nPTypes, 1, I_ );
      mtp->ParTD = (short (*)[6])aObj[o_mtpartd].Alloc( mtp->nPTypes, 6, I_ );
    }
    else
    {
      mtp->NPmean = (short *)aObj[o_mtnpmean].Free();
      mtp->nPmin = (short *)aObj[o_mtnpmin].Free();
      mtp->nPmax = (short *)aObj[o_mtnpmax].Free();
      mtp->ParTD = (short (*)[6])aObj[o_mtpartd].Free();
    }

    if( mtp->Nsd > 0 )
    {
      mtp->sdref =
        (char (*)[V_SD_RKLEN])aObj[ o_mtsdref].Alloc( mtp->Nsd, 1, V_SD_RKLEN );
      mtp->sdval =
         (char (*)[V_SD_VALEN])aObj[ o_mtsdval].Alloc( mtp->Nsd, 1, V_SD_VALEN );
    }
    else
    {
      mtp->sdref = (char (*)[V_SD_RKLEN])aObj[ o_mtsdref].Free();
      mtp->sdval = (char (*)[V_SD_VALEN])aObj[ o_mtsdval].Free();
    }
    mtp->etext = (char *)aObj[ o_mwetext ].Alloc(1, 4096, S_);
    mtp->tprn = (char *)aObj[ o_mwtprn ].Alloc(1, 2048, S_);
}

// return true if nesessary, recalc base SYSEQ
bool TGEM2MT::check_input( const char * /*key*/, int /*Level*/ )
{
    vstr pkey(MAXRKEYLEN+10);
    if( pVisor->ProfileMode != true )
        return true;

    //Get base SysEq key from process key
    rt[RT_SYSEQ].MakeKey( RT_GEM2MT, pkey, RT_GEM2MT, 0, RT_GEM2MT, 1,
                           RT_GEM2MT, 2, RT_GEM2MT, 3, RT_GEM2MT, 4,
                           RT_GEM2MT, 5, RT_GEM2MT, 6, RT_GEM2MT, 7, K_END);
    // read SysEq record and unpack data
    TProfil::pm->loadSystat( pkey );
    // test changes in system after process calc
    if( rt[RT_SYSEQ].Rtime() > rt[nRT].Rtime() )
        return true;
    return false;
}


//set default information
void TGEM2MT::set_def(int q)
{
    ErrorIf( mtp!=&mt[q], GetName(),
        "E03DTrem: Illegal access to mt in set_def");

//    TProfil *aPa= TProfil::pm;
    memcpy( &mtp->PunE, "jjbC", 4 );
    memcpy( &mtp->PvICi, "++--------S00-+--+", 18 );
    strcpy( mtp->name,  "`" );
    strcpy( mtp->notes, "`" );
    strcpy( mtp->xNames, "X" );
    strcpy( mtp->yNames, "Y" );
    memset( &mtp->nC, 0, sizeof(short)*28 );
    memset( &mtp->Msysb, 0, sizeof(double)*20 );
    memset( mtp->size[0], 0, sizeof(float)*8 );
    memset( mtp->sizeLc, 0, sizeof(float)*3 );
    memset( mtp->sykey, 0, sizeof(char)*(EQ_RKLEN+10) );
    mtp->nC =2;
    mtp->nIV =2;
    mtp->ntM =1000;
    mtp->cdv = 1e-9;
    mtp->cez = 1e-12; 
    mtp->nYS =0;
    mtp->nYE =1;
    mtp->nPai =1;
    mtp->nTai =1;
    mtp->tmi[START_] = 1000;
    mtp->tmi[STOP_] = 1200;
    mtp->tmi[STEP_] = 1;
    mtp->NVi[START_] = 0;
    mtp->NVi[STOP_] = 0;
    mtp->NVi[STEP_] = 0;
    mtp->Pai[START_] = 1.;
    mtp->Pai[STOP_] = 1.;
    mtp->Pai[STEP_] = 0.;
    mtp->Pai[3] = .5;      //Ptol
    mtp->Tai[START_] = 25.;
    mtp->Tai[STOP_] = 25.;
    mtp->Tai[STEP_] = 0.;
    mtp->Tai[3] = 1.;     //Ttol
    mtp->Tau[START_] = 0.;
    mtp->Tau[STOP_] = 0.;
    mtp->Tau[STEP_] = 0.;
// pointers
    mtp->lNam = NULL;
    mtp->lNamE = NULL;
    mtp->tExpr = 0;
    mtp->gExpr = 0;
    mtp->sdref = 0;
    mtp->sdval = 0;
    mtp->DiCp = 0;
    mtp->FDLi = 0;
    mtp->Pi = 0;
    mtp->Ti = 0;
    mtp->Vi = 0;
    mtp->xEt = 0;
    mtp->yEt = 0;
    mtp->Bn = 0;
    mtp->HydP = 0;
    mtp->qpi = 0;
    mtp->qpc = 0;
    mtp->xt = 0;
    mtp->yt = 0;
    mtp->CIb = 0;
    mtp->CAb = 0;
    mtp->FDLf = 0;
    mtp->PGT = 0;
    mtp->Tval = 0;
    mtp->Pval = 0;
    mtp->nam_i = 0;
    mtp->for_i = 0;
    mtp->stld = 0;
    mtp->CIclb = 0;
    mtp->AUcln = 0;
    mtp->FDLid = 0;
    mtp->FDLop = 0;
    mtp->FDLmp = 0;
    mtp->MPGid = 0;
    mtp->UMPG = 0;
    mtp->SBM = 0;
    plot = 0;
    mtp->BSF = 0;
    mtp->MB = 0;
    mtp->dMB = 0;
    mtp->DDc = 0;
    mtp->DIc = 0;
    mtp->DEl = 0;
    mtp->for_e = 0;
    mtp->xIC = 0;
    mtp->xDC = 0;
    mtp->xPH = 0;
    //added 28/03/2006
mtp->grid = 0;
mtp->NPmean = 0;
mtp->nPmin = 0;
mtp->nPmax = 0;
mtp->ParTD = 0;

// work
    mtp->An = 0;
   mtp->Ae = 0;
    mtp->gfc = 0;
    mtp->yfb = NULL; 
    tt = NULL; 
    mtp->etext = 0;
    mtp->tprn = 0;
}

// Input nessasery data and links objects
void TGEM2MT::RecInput( const char *key )
{
    //  strncpy( keywd, key, 24 );
    TCModule::RecInput( key );
}

/* opens window with 'Remake record' parameters
*/
void
TGEM2MT::MakeQuery()
{
//    pImp->MakeQuery();
    const char * p_key;
    char flgs[22];
    int size[17];

    p_key  = db->PackKey();
    memcpy( flgs, &mtp->PunE, 22);
    size[0] = mtp->nC;
    size[1] = mtp->nIV;
    size[2] = mtp->ntM;
    size[3] = mtp->nPG;
    size[4] = mtp->nFD;
    size[5] = mtp->nSFD;
    size[6] = mtp->nYS;
    size[7] = mtp->nE;
    size[8] = mtp->nYE;
    size[9] = mtp->Lbi;
    size[10] = mtp->nEl;
    size[11] = mtp->Nqpg;
    size[12] = mtp->nPai;
    size[13] = mtp->nTai;
    size[14] = mtp->Nsd;
    size[15] = mtp->nPTypes;
    size[16] = mtp->nProps;

    if( !vfGEM2MTSet( window(), p_key, flgs, size ))
         Error( p_key, "GEM2MT record configuration cancelled by the user!" );
     //  return;   // cancel

    memcpy( &mtp->PunE, flgs, 22);
    mtp->nC = (short)size[0];
    mtp->nIV = (short)size[1];
    mtp->ntM = (short)size[2];
    mtp->nPG = (short)size[3];
    mtp->nFD = (short)size[4];
    mtp->nSFD = (short)size[5];
    mtp->nYS = (short)size[6];
    mtp->nE = (short)size[7];
    mtp->nYE = (short)size[8];
    mtp->Lbi = (short)size[9];
    mtp->nEl = (short)size[10];
    mtp->Nqpg = (short)size[11];
    mtp->nPai = (short)size[12];
    mtp->nTai = (short)size[13];
    mtp->Nsd = (short)size[14];
    mtp->nPTypes = (short)size[15];
    mtp->nProps = (short)size[16];

}

//Rebild record structure before calc
int
TGEM2MT::RecBuild( const char *key, int mode )
{

    if( pVisor->ProfileMode != true )
      Error( GetName(), "E09DTrem: Please, do it in the Project mode!" );

    bool setdef = false;
AGAIN:
    int ret = TCModule::RecBuild( key, mode );
    if( ret == VF_CANCEL )
        return ret;

    if( ret == VF3_1 )
       setdef = true;

    if( !test_sizes() )
    {  mode = VF_REMAKE;
      goto AGAIN;
    }

    SelectNodeStructures( false );
    //dyn_new() in function SelectNodeStructures

    init_arrays( setdef ); //clear all

    SetString("Record build OK");
    pVisor->Update();
    return ret;
}

//Calculate record
void
TGEM2MT::RecCalc( const char * key )
{
 try
 {
   if( pVisor->ProfileMode != true  )
       Error( GetName(), "E02GDexec: Please, do it in the Project mode" );
   if( mtp->nICb == 0 && mtp->nDCb == 0 &&
       mtp->nPHb == 0  )
       Error( GetName(), "Added/deleted components in the project system.\n"
                         "Please, remake the record." );

   na = new TNodeArray( mtp->nC, TProfil::pm->pmp/*multi->GetPM()*/ );

   // use particles
   if( mtp->PsMode == GMT_MODE_W || mtp->PsMode == GMT_MODE_V )
   {
     na->SetGrid( mtp->sizeLc, mtp->grid );   // set up grid structure
     pa = new TParticleArray( mtp->nPTypes, mtp->nProps,
           mtp->NPmean, mtp->ParTD, mtp->nPmin, mtp->nPmax, na );
   }

  if( mtp->iStat == AS_RUN )
     if( !vfQuestion( window(), key,
        "This RMT task has been interrupted by the user.\n"
        "Read node array from disk files and resume calculations (Yes),\n"
        " or start from the beginning (No)?"))
    mtp->iStat =  GS_INDEF;

   if( mtp->iStat != AS_RUN  )
   {
     mtp->gStat = GS_GOING;
     mt_reset();
     Bn_Calc();
     mtp->gStat = GS_DONE;
   }
   else
   {
     gstring f_name;
     if( mtp->notes[0] == '@' )
        f_name = gstring( mtp->notes, 1, MAXFORMULA-1 );
     // open file to read
     else
       if( vfChooseFileOpen(window(), f_name,
          "Please, enter the IPM work structure file name", "*.lst" ) == false )
       {        delete na;
               return;
       }
     na->GEM_init( f_name.c_str(), 0, true );
     // use particles
     //   if( mtp->PsMode == GMT_MODE_W || mtp->PsMode == GMT_MODE_V )
     //   pa->ParticleArrayInit();
  }

   if( mtp->PsMode == GMT_MODE_S )
   {   // calculate start data
     mtp->iStat = AS_READY;
     outMulti();
     mtp->iStat = AS_DONE;
   }
   else
   // if( mtp->PsMode == 'A' || mtp->PsMode == 'D' || mtp->PsMode == 'T' ) 'W' 'V'
   {   // calculate start data

     if( mtp->iStat != AS_RUN  )
         NewNodeArray();  // set up start DATACH structure and DATABR arrays structure
     if( mtp->PvMSg != S_OFF )
         Expr_analyze( o_mtgexpr );
     
     bool iRet; 
     
     if( mtp->PsMode == GMT_MODE_F ) // Flux-box RMT scoping model
       iRet = CalcBoxModel( NEED_GEM_SIA );
     else
    	iRet =  Trans1D( NEED_GEM_SIA );
 
     if( iRet )
     { // canceled calculations
        if( vfQuestion( window(), "GEM2MT task interrupt",
           "RMT calculations have been stopped by the user.\n"
           "Save node array into a set of disk files to resume the task later (Yes),\n"
           " or cancel the RMT task (No)?" ) )
        {
          gstring path = na->PutGEM2MTFiles( window(), mtp->nC,
            mtp->PsSdat==S_OFF, false, false, true, true ); // with Nod0 and Nod1
          mtp->notes[0] = '@';
          strncpy( mtp->notes+1, path.c_str(), MAXFORMULA-1 );
          // save GEM2MT recort
          gstring key_str = db->PackKey();
          RecSave( key_str.c_str() );
          mtp->iStat = AS_RUN;
        }
     }
     else
        mtp->iStat = AS_DONE;
   }

  if( pa )
  { delete pa;
    pa = 0;
  }
   delete na;
   na = 0;
   LinkNode0(-1);
   LinkNode1(-1);
   LinkCSD(-1);

   TCModule::RecCalc( key );
  }
  catch( TError& xcpt )
  {
    if( pa )
    { delete pa;
      pa = 0;
    }
     delete na;
     na = 0;
     LinkNode0(-1);
     LinkNode1(-1);
     LinkCSD(-1);
     mtp->gStat = GS_ERR;
     mtp->iStat = AS_INDEF;
     Error(  xcpt.title.c_str(), xcpt.mess.c_str() );
  }
}


void
TGEM2MT::CmHelp()
{
    pVisor->OpenHelp( GEMS_MT_HTML );
}

void TGEM2MT::RecordPrint( const char* key )
{
    int res = vfQuestion3(window(), "Question",
                    "Will you produce input files for standalone TGEM2MT (Yes) or use print script (No)?",
		       "Yes", "No", "Cancel");
	if( res == VF3_3 )
	    return;

	if( res == VF3_1 )
	{
		gstring filename = "GEM2MT-task.dat";
		if( vfChooseFileSave(window(), filename,
				   "Please, enter the TGEM2MT work structure file name", "*.dat" ) )
		{
		    if( !access(filename.c_str(), 0 ) ) //file exists
		        if( !vfQuestion( window(), filename.c_str(),
		        		"This file exists! Overwrite?") )
                   return;
            fstream ff( filename.c_str(), ios::out );
	        ErrorIf( !ff.good() , filename.c_str(), "Fileopen error");
	        to_text_file( ff, true );

		}
    na = new TNodeArray( mtp->nC, TProfil::pm->pmp/*multi->GetPM()*/ );
    mtp->gStat = GS_GOING;
    mt_reset();
    Bn_Calc();
    mtp->gStat = GS_DONE;
    mtp->iStat = AS_READY;
    outMulti();
    mtp->iStat = AS_DONE;
    delete na;
    na = 0;
	}
	else
	     TCModule::RecordPrint( key );
}

// insert changes in Project to GEM2MT
void TGEM2MT::InsertChanges( TIArray<CompItem>& aIComp,
      TIArray<CompItem>& aPhase,  TIArray<CompItem>&aDComp )
{

    // insert changes to IComp
    if(aIComp.GetCount()<1 && aPhase.GetCount()<1 && aDComp.GetCount()<1)
       return;

   // alloc memory & copy data from db

    int j, ii, jj;
    uint i;
    int Nold = mtp->Nb;
    //int FIold = mtp->FIb;
    //int Lsold = mtp->Lb;

    double *p_Bn = new double[mtp->nIV*mtp->Nb];
    memcpy( p_Bn, mtp->Bn, mtp->nIV*mtp->Nb*sizeof(double));

    char  *p_CIclb=0;
    float *p_CIb=0;
 //   float *p_PGT;   all this arrays must be changed by user after remake the record!!! 17/12/2007
 //   char  *p_UMPG;  Nf, Lsf, FIf sizes from selection undef after resize the project
 //   double* p_BSF;
 //   double* p_dMB;
 //   float *p_DDc;
 //   float *p_DIc;

    if( mtp->PvICi != S_OFF )
    {
      p_CIclb = new char[mtp->Nb];
      memcpy( p_CIclb, mtp->CIclb, mtp->Nb*sizeof(char));
      p_CIb = new float[mtp->nIV*mtp->Nb];
      memcpy( p_CIb, mtp->CIb, mtp->nIV*mtp->Nb*sizeof(float));
    }

//    if( mtp->PvPGD != S_OFF )
//    {
//      p_UMPG = new char[mtp->FIb];
//      memcpy( p_UMPG, mtp->UMPG, mtp->FIf*sizeof(char));
//      p_PGT = new float[mtp->nIV*mtp->Nb];
//      memcpy( p_PGT, mtp->PGT, mtp->nPG*mtp->FIf*sizeof(float));
//    }

//   if( mtp->PvSFL != S_OFF )
//   {
//     p_BSF = new double[mtp->nSFD*mtp->Nb];
//     memcpy( p_BSF, mtp->BSF, mtp->nSFD*mtp->Nf*sizeof(double));
//   }

//   if( mtp->PvPGD != S_OFF && mtp->PvFDL != S_OFF )
//   {
//      p_dMB = new double[mtp->nC*mtp->Nb];
//      memcpy( p_dMB, mtp->dMB, mtp->nC*mtp->Nf*sizeof(double));
//   }

//   if( mtp->PsDDc != S_OFF )
//   {
//      p_DDc = new float[mtp->Lb];
//      memcpy( p_DDc, mtp->DDc, mtp->Lsf*sizeof(float));
//   }
//   if( mtp->PsDIc != S_OFF )
//   {
//      p_DIc = new float[mtp->Nb];
//      memcpy( p_DIc, mtp->DIc, mtp->Nf*sizeof(float));
//   }

    // alloc new memory
     mtp->Nb = TProfil::pm->mup->N;
     mtp->FIb = TProfil::pm->mup->Fi;
     mtp->Lb = TProfil::pm->mup->L;
     mtp->nICb = 0;   // number of stoichiometry units (<= nIC) used in the data bridge
     mtp->nDCb = 0;   // number of DC (chemical species, <= nDC) used in the data bridge
     mtp->nPHb = 0;   // number of phases (<= nPH) used in the data bridge

     dyn_new();

//***************************************************
    for( ii=0; ii< mtp->Nb; ii++ )
      memcpy( mtp->SBM[ii], TProfil::pm->mup->SB[ii], MAXICNAME  );

    i=0; jj = 0; ii = 0;
    while( jj < mtp->Nb )
    {
      if( i < aIComp.GetCount() &&  aIComp[i].line == ii )
      {
        if( aIComp[i].delta == 1 )
        { // add line
          for( j =0; j<mtp->nIV; j++ )
          {
            mtp->Bn[j*mtp->Nb+jj] = 0.;
            if( mtp->PvICi != S_OFF )
              mtp->CIb[j*mtp->Nb+jj] = 0.;
          }
          if( mtp->PvICi != S_OFF )
              mtp->CIclb[jj] = QUAN_GRAM;
//          if( mtp->PvSFL != S_OFF )
//             for( j=0; j<mtp->nSFD; j++ )
//               mtp->BSF[j*mtp->Nf+jj] = 0.; // ??????
//          if( mtp->PvPGD != S_OFF && mtp->PvFDL != S_OFF )
//             for( j=0; j<mtp->nC; j++ )
//               mtp->dMB[j*mtp->Nf+jj] = 0.;
//          if( mtp->PsDIc != S_OFF )
//              mtp->DIc[jj] = 0.; // ??????
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
             for( j =0; j<mtp->nIV; j++ )
             {
               mtp->Bn[j*mtp->Nb+jj] = p_Bn[j*Nold+ii];
               if( mtp->PvICi != S_OFF )
                 mtp->CIb[j*mtp->Nb+jj] = p_CIb[j*Nold+ii];
             }
             if( mtp->PvICi != S_OFF )
                 mtp->CIclb[jj] = p_CIclb[ii];
//            if( mtp->PvSFL != S_OFF )
//               for( j=0; j<mtp->nSFD; j++ )
//                 mtp->BSF[j*mtp->Nf+jj] = p_BSF[j*Nold+ii]; ///????
//             if( mtp->PvPGD != S_OFF && mtp->PvFDL != S_OFF )
//               for( j=0; j<mtp->nC; j++ )
//                 mtp->dMB[j*mtp->Nf+jj] = p_dMB[j*Nold+ii];
//             if( mtp->PsDIc != S_OFF )
//               mtp->DIc[jj] =  p_DIc[ii]; // ????
          }
        jj++;
        ii++;
       }
    }
/*************************************************************
// Phases
 if( mtp->PvPGD != S_OFF )
 {    i=0; jj = 0; ii = 0;
    while( jj < mtp->FIb )
    {
      if( i < aPhase.GetCount() &&  aPhase[i].line == ii )
      {
        if( aPhase[i].delta == 1 )
        { // add line
          for( j =0; j<mtp->nPG; j++ )
            mtp->PGT[j*mtp->FIf+jj] = 0.;
          mtp->UMPG[jj] = QUAN_GRAM;
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
         if( ii < FIold )
         {
             for( j =0; j<mtp->nPG; j++ )
               mtp->PGT[j*mtp->FIf+jj] = p_PGT[j*FIold+ii];
             mtp->UMPG[jj] = p_UMPG[ii];
          }
        jj++;
        ii++;
      }
    }
 }
*/
/*************************************************************
// DCOMP
 if( mtp->PsDDc != S_OFF )
 {
    i=0; jj = 0; ii = 0;
    while( jj < mtp->Lsf )
    {
      if( i < aDComp.GetCount() &&  aDComp[i].line == ii )
      {
        if( aDComp[i].delta == 1 )
        { // add line
          mtp->DDc[jj] = 0.;
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
         if( ii < Lsold )
         {
             mtp->DDc[jj] = p_DDc[ii];
          }
        jj++;
        ii++;
      }
    }
 }
*/
//*************************************************************

// free memory
   delete[] p_Bn;
   if( mtp->PvICi != S_OFF )
   {
     delete[] p_CIclb;
     delete[] p_CIb;
    }
//   if( mtp->PvPGD != S_OFF )
//   {
//     delete[] p_PGT;
//     delete[] p_UMPG;
//    }
//   if( mtp->PvSFL != S_OFF )
//    delete[] p_BSF;
//   if( mtp->PvPGD != S_OFF && mtp->PvFDL != S_OFF )
//    delete[] p_dMB;

//   if( mtp->PsDDc != S_OFF )
//     delete[] p_DDc;
//   if( mtp->PsDIc != S_OFF )
//     delete[] p_DIc;

}

// --------------------- end of m_gem2mt.cpp ---------------------------


