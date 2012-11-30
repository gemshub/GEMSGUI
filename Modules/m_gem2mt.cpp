//-------------------------------------------------------------------
// $Id: m_gem2mt.cpp 1409 2009-08-21 15:34:43Z gems $
//
// Implementation of TGEM2MT class, config and calculation functions
//
// Copyright (C) 2005 S.Dmytriyeva, D.Kulik
//
// This file is part of a GEM-Selektor library for thermodynamic
// modelling by Gibbs energy minimization
// Uses: GEM-Selektor GUI GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the terms of GEMS3 Development
// Quality Assurance Licence (GEMS3.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//

#include <cmath>
#include <cstdio>
#ifndef __unix
#include <io.h>
#endif

#include "m_gem2mt.h"
#include "visor.h"
#include "m_syseq.h"

TGEM2MT* TGEM2MT::pm;

TGEM2MT::TGEM2MT( int nrt ):
        TCModule( nrt )
{
    nQ = 1;
    aFldKeysHelp.Add("Name of the modeling project");
    aFldKeysHelp.Add("Thermodynamic potential to minimize {G}");
    aFldKeysHelp.Add("Name of the parent chemical system definition (CSD)");
    aFldKeysHelp.Add("CSD (recipe) variant number <integer>");
    aFldKeysHelp.Add("Volume of the system, dm3");
    aFldKeysHelp.Add("Pressure, bar, or 0 for Psat(H2O)g");
    aFldKeysHelp.Add("Temperature, C");
    aFldKeysHelp.Add("Variant number for additional constraints");
    aFldKeysHelp.Add("Name of this reactive transport simulation task");
    aFldKeysHelp.Add("Batch simulator type code, a combination of {}");
    setKeyEditField(8);

    mtp=&mt[0];
    set_def();
    start_title =
       " Definition of a GEM2MT (Coupled RMT model)";

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
     if( keyType != KEY_TEMP )
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
            Error( key, "E08GTrem: Invalid record key (another Modelling Project)!");
        rt[RT_SYSEQ].MakeKey( RT_GEM2MT, pkey, RT_GEM2MT, 0, RT_GEM2MT, 1,
                               RT_GEM2MT, 2, RT_GEM2MT, 3, RT_GEM2MT, 4,
                               RT_GEM2MT, 5, RT_GEM2MT, 6, RT_GEM2MT, 7, K_END);
        if( rt[RT_SYSEQ].Find(pkey) <0 )
            Error( key, "E07GTrem: Invalid record key (no system)!");
    }
}

// link values to objects
void TGEM2MT::ods_link(int q)
{
    mtp=&mt[q];

// static
    aObj[o_mtpufl].SetPtr( &mtp->PunE );   /* a4 */
aObj[o_mtpvfl].SetPtr( &mtp->PvICi );  /* a14*/
aObj[o_mtpsfl].SetPtr( &mtp->PsMode ); /* a14 */
    aObj[o_mtnc].SetPtr( &mtp->nC );     /* l1 */
    aObj[o_mtnv].SetPtr( &mtp->nIV );     /* l1 */
aObj[o_mtnfd].SetPtr( &mtp->nFD );  /* l1 */
aObj[o_mtnsfd].SetPtr( &mtp->nSFD );  /* l1 */
aObj[o_mtnmgp].SetPtr( &mtp->nPG );  /* l1 */
    aObj[o_mtcipf].SetPtr( &mtp->nEl );    /* l3 */
    aObj[o_mtszt].SetPtr( &mtp->Lbi );     /* l8 */
    aObj[o_mtnsne].SetPtr( &mtp->ntM );   /* l4 */
    aObj[o_mtptai].SetPtr( &mtp->nPai );  /* l2 */
aObj[o_mtchbr].SetPtr( &mtp->Lsf );  /* l10 */
    aObj[o_mttmi].SetPtr( mtp->tmi );     /* l3 */
    aObj[o_mtnvi].SetPtr( mtp->NVi );     /* l3 */
//    aObj[o_mtaxis].SetPtr( mtp->axisType ); /* l6 */
    aObj[o_mtpai].SetPtr( mtp->Pai );     /* d4 */
    aObj[o_mttai].SetPtr( mtp->Tai );     /* d4 */
    aObj[o_mttau].SetPtr( mtp->Tau );     /* d3 */
    aObj[o_mtsizelc].SetPtr( mtp->sizeLc );     /* d3 */
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

// DBase
    aObj[o_mtname].SetPtr(  mtp->name );
    aObj[o_mtnotes].SetPtr(   mtp->notes );
 aObj[o_mtflag].SetPtr( &mtp->PunE );    /* a32!!! */
 aObj[o_mtshort].SetPtr( &mtp->nC );     /* l38!!! */
    aObj[o_mtaxis].SetPtr( mtp->axisType ); /* i6 */
    aObj[o_mtworks].SetPtr( &mtp->ctm );    /* l12 */
    aObj[o_mtworkf].SetPtr( &mtp->cT );     /* d10 */
    aObj[o_mtdoudl].SetPtr( &mtp->Msysb );    /* d9 */
    aObj[o_mtadpar].SetPtr( &mtp->tf );    /* d11 */
aObj[o_mtfloat].SetPtr( mtp->Pai );    /* d14 */
aObj[o_mtsize].SetPtr( mtp->size[0] );  /* f8 */
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
aObj[ o_mtptvi].SetPtr( mtp->PTVm);
aObj[ o_mtptvi].SetDim( mtp->nIV, 5 );
aObj[ o_mtstap].SetPtr( mtp->StaP);
aObj[ o_mtstap].SetDim( mtp->nC, 4 );
aObj[ o_mtxflds].SetPtr( mtp->xVTKfld);
aObj[ o_mtxflds].SetDim( mtp->nVTKfld, 1 );
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
    aObj[ o_mtmpgid].SetPtr( mtp->MGPid );
    aObj[ o_mtmpgid].SetDim(  mtp->nPG, 1 );
    aObj[ o_mtumpg].SetPtr( mtp->UMGP );
    aObj[ o_mtumpg].SetDim( mtp->FIf, 1 );
    aObj[ o_mtbm].SetPtr( mtp->SBM );
    aObj[ o_mtbm].SetDim(  1, mtp->Nb );
    aObj[ o_mtplline].SetPtr( plot );
    aObj[ o_mtplline].SetDim( mtp->nYS+mtp->nYE,  sizeof(TPlotLine));
    aObj[ o_mtbsf].SetPtr( mtp->BSF);
    aObj[ o_mtbsf].SetDim( mtp->nSFD, mtp->Nf );
    aObj[ o_mtmb].SetPtr( mtp->MB);
    aObj[ o_mtmb].SetDim( mtp->nC, mtp->Nf );
    aObj[ o_mtdmb].SetPtr( mtp->dMB);
    aObj[ o_mtdmb].SetDim( mtp->nC, mtp->Nf  );
    aObj[ o_mtddc].SetPtr( mtp->DDc);
    aObj[ o_mtddc].SetDim( mtp->Lsf, 1 );
    aObj[ o_mtdic].SetPtr( mtp->DIc);
    aObj[ o_mtdic].SetDim( mtp->Nf, 1 );
    aObj[ o_mtdel].SetPtr( mtp->DEl);
    aObj[ o_mtdel].SetDim( mtp->nEl, 1 );
    aObj[ o_mtfor_e].SetPtr( mtp->for_e);
    aObj[ o_mtfor_e].SetDim( mtp->nEl, 1 );
    aObj[o_mt_xic].SetPtr( mtp->xIC );
    aObj[o_mt_xic].SetDim( mtp->nICb, 1 );
    aObj[o_mt_xdc].SetPtr( mtp->xDC );
    aObj[o_mt_xdc].SetDim( mtp->nDCb, 1 );
    aObj[o_mt_xph].SetPtr( mtp->xPH );
    aObj[o_mt_xph].SetDim( mtp->nPHb, 1 );
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

aObj[ o_mtres1].SetPtr( mtp->arr1);
aObj[ o_mtres1].SetDim( mtp->nC, 1 );
aObj[ o_mtres2].SetPtr( mtp->arr2);
aObj[ o_mtres2].SetDim( mtp->nC, 1 );

// work
    aObj[ o_mtan].SetPtr(mtp->An);
//    aObj[ o_mtan].SetDim( dtp->nK, dtp->Nb );
    aObj[ o_mtan].SetDim( mtp->Lbi, mtp->Nb );
    aObj[ o_mtae].SetPtr(mtp->Ae);
    aObj[ o_mtae].SetDim( mtp->nEl, mtp->Nb );

    aObj[ o_mtgfc].SetPtr(mtp->gfc);
    aObj[ o_mtgfc].SetDim( mtp->nC*mtp->nPG, mtp->Nf );
    aObj[ o_mtyfb].SetPtr(mtp->yfb);
    aObj[ o_mtyfb].SetDim( mtp->nC*mtp->nPG, mtp->Nf );
    aObj[ o_mttt].SetPtr(mtp->tt);
    aObj[ o_mttt].SetDim( mtp->nC * mtp->Nf, 9 );

    aObj[ o_mwetext].SetPtr( mtp->etext );
    // aObj[ o_mwetext].SetDim(1, 0 );
    aObj[ o_mwtprn].SetPtr(  mtp->tprn  );
    // aObj[ o_mwtprn].SetDim( 1,l( mtp->tprn));

}

// set dynamic Objects ptr to values
void TGEM2MT::dyn_set(int q)
{
    ErrorIf( mtp!=&mt[q], GetName(),
       "E06GTrem: Attempt to access corrupted dynamic memory.");

    mtp->lNam = ( char (*)[MAXGRNAME] )aObj[ o_mtlnam].GetPtr();
    mtp->lNamE = (char (*)[MAXGRNAME])aObj[ o_mtlname].GetPtr();
    mtp->tExpr = (char *)aObj[o_mttexpr].GetPtr();
    mtp->gExpr = (char *)aObj[o_mtgexpr].GetPtr();
    mtp->sdref = (char (*)[V_SD_RKLEN])aObj[ o_mtsdref].GetPtr();
    mtp->sdval = (char (*)[V_SD_VALEN])aObj[ o_mtsdval].GetPtr();
    mtp->DiCp = (long int (*)[2])aObj[ o_mtdicp].GetPtr();
    mtp->FDLi = (long int (*)[2])aObj[ o_mtfdli].GetPtr();
    mtp->PTVm = (double (*)[5])aObj[ o_mtptvi].GetPtr();
    mtp->StaP = (double (*)[4])aObj[ o_mtstap].GetPtr();
    mtp->xVTKfld = (long int (*)[2])aObj[ o_mtxflds].GetPtr();
    mtp->xEt = (double *)aObj[ o_mtxet].GetPtr();
    mtp->yEt = (double *)aObj[ o_mtyet].GetPtr();
    mtp->Bn = (double *)aObj[ o_mtbn].GetPtr();
    mtp->HydP = (double (*)[SIZE_HYDP])aObj[ o_mthydp].GetPtr();
    mtp->qpi = (double *)aObj[ o_mtqpi].GetPtr();
    mtp->qpc = (double *)aObj[ o_mtqpc].GetPtr();
    mtp->xt = (double *)aObj[ o_mtxt].GetPtr();
    mtp->yt = (double *)aObj[ o_mtyt].GetPtr();
    mtp->CIb = (double *)aObj[ o_mtcib].GetPtr();
    mtp->CAb = (double *)aObj[ o_mtcab].GetPtr();
    mtp->FDLf = (double (*)[4])aObj[ o_mtfdlf].GetPtr();
    mtp->PGT = (double *)aObj[ o_mtpgt].GetPtr();
    mtp->Tval = (double *)aObj[ o_mttval].GetPtr();
    mtp->Pval = (double *)aObj[ o_mtpval].GetPtr();
    mtp->nam_i = (char (*)[MAXIDNAME])aObj[ o_mtnam_i].GetPtr();
    mtp->for_i = (char (*)[MAXFORMUNITDT])aObj[ o_mtfor_i].GetPtr();
    mtp->stld = (char (*)[EQ_RKLEN])aObj[ o_mtstld].GetPtr();
    mtp->CIclb = (char *)aObj[ o_mtciclb].GetPtr();
    mtp->AUcln = (char *)aObj[ o_mtaucln].GetPtr();
    mtp->FDLid = (char (*)[MAXSYMB])aObj[ o_mtfdlid].GetPtr();
    mtp->FDLop = (char (*)[MAXSYMB])aObj[ o_mtfdlop].GetPtr();
    mtp->FDLmp = (char (*)[MAXSYMB])aObj[ o_mtfdlmp].GetPtr();
    mtp->MGPid = (char (*)[MAXSYMB])aObj[ o_mtmpgid].GetPtr();
    mtp->UMGP = (char *)aObj[ o_mtumpg].GetPtr();
    mtp->SBM = (char (*)[MAXICNAME+MAXSYMB])aObj[ o_mtbm].GetPtr();
    plot = (TPlotLine *)aObj[ o_mtplline].GetPtr();
    mtp->BSF = (double *)aObj[ o_mtbsf].GetPtr();
    mtp->MB = (double *)aObj[ o_mtmb].GetPtr();
    mtp->dMB = (double *)aObj[ o_mtdmb].GetPtr();
    mtp->DDc = (double*)aObj[ o_mtddc].GetPtr();
    mtp->DIc = (double*)aObj[ o_mtdic].GetPtr();
    mtp->DEl = (double*)aObj[ o_mtdel].GetPtr();
    mtp->for_e = (char (*)[MAXFORMUNITDT])aObj[ o_mtfor_e].GetPtr();
    mtp->xIC = (long int *)aObj[o_mt_xic].GetPtr();
    mtp->xDC = (long int *)aObj[o_mt_xdc].GetPtr();
    mtp->xPH = (long int *)aObj[o_mt_xph].GetPtr();
    mtp->grid = (double (*)[3])aObj[o_mtgrid].GetPtr();
    mtp->NPmean = (long int *)aObj[o_mtnpmean].GetPtr();
    mtp->nPmin = (long int *)aObj[o_mtnpmin].GetPtr();
    mtp->nPmax = (long int *)aObj[o_mtnpmax].GetPtr();
    mtp->ParTD = (long int (*)[6])aObj[o_mtpartd].GetPtr();

 mtp->arr1 = (double *)aObj[ o_mtres1].GetPtr();
 mtp->arr2 = (double *)aObj[ o_mtres2].GetPtr();

// work
    mtp->An = (double *)aObj[ o_mtan].GetPtr();
    mtp->Ae = (double *)aObj[ o_mtae].GetPtr();
    mtp->gfc = (double *)aObj[ o_mtgfc].GetPtr();
    mtp->yfb = (double *)aObj[ o_mtyfb].GetPtr();
    mtp->tt = (double (*)[9])aObj[ o_mttt].GetPtr();
    mtp->etext = (char *)aObj[ o_mwetext].GetPtr();
    mtp->tprn = (char *)aObj[ o_mwtprn].GetPtr();
}

// free dynamic memory in objects and values
void TGEM2MT::dyn_kill(int q)
{
    ErrorIf( mtp!=&mt[q], GetName(),
       "E05GTrem: Attempt to access corrupted dynamic memory.");

    mtp->lNam = (char (*)[MAXGRNAME])aObj[ o_mtlnam].Free();
    mtp->lNamE = (char (*)[MAXGRNAME])aObj[ o_mtlname].Free();
    mtp->tExpr = (char *)aObj[o_mttexpr].Free();
    mtp->gExpr = (char *)aObj[o_mtgexpr].Free();
    mtp->sdref = (char (*)[V_SD_RKLEN])aObj[ o_mtsdref].Free();
    mtp->sdval = (char (*)[V_SD_VALEN])aObj[ o_mtsdval].Free();
    mtp->DiCp = (long int (*)[2])aObj[ o_mtdicp].Free();
    mtp->FDLi = (long int (*)[2])aObj[ o_mtfdli].Free();
    mtp->PTVm = (double (*)[5])aObj[ o_mtptvi].Free();
    mtp->StaP = (double (*)[4])aObj[ o_mtstap].Free();
    mtp->xVTKfld = (long int (*)[2])aObj[ o_mtxflds].Free();
    mtp->xEt = (double *)aObj[ o_mtxet].Free();
    mtp->yEt = (double *)aObj[ o_mtyet].Free();
    mtp->Bn = (double *)aObj[ o_mtbn].Free();
    mtp->HydP = (double (*)[SIZE_HYDP])aObj[ o_mthydp].Free();
    mtp->qpi = (double *)aObj[ o_mtqpi].Free();
    mtp->qpc = (double *)aObj[ o_mtqpc].Free();
    mtp->xt = (double *)aObj[ o_mtxt].Free();
    mtp->yt = (double *)aObj[ o_mtyt].Free();
    mtp->CIb = (double *)aObj[ o_mtcib].Free();
    mtp->CAb = (double *)aObj[ o_mtcab].Free();
    mtp->FDLf = (double (*)[4])aObj[ o_mtfdlf].Free();
    mtp->PGT = (double *)aObj[ o_mtpgt].Free();
    mtp->Tval = (double *)aObj[ o_mttval].Free();
    mtp->Pval = (double *)aObj[ o_mtpval].Free();
    mtp->nam_i = (char (*)[MAXIDNAME])aObj[ o_mtnam_i].Free();
    mtp->for_i = (char (*)[MAXFORMUNITDT])aObj[ o_mtfor_i].Free();
    mtp->stld = (char (*)[EQ_RKLEN])aObj[ o_mtstld].Free();
    mtp->CIclb = (char *)aObj[ o_mtciclb].Free();
    mtp->AUcln = (char *)aObj[ o_mtaucln].Free();
    mtp->FDLid = (char (*)[MAXSYMB])aObj[ o_mtfdlid].Free();
    mtp->FDLop = (char (*)[MAXSYMB])aObj[ o_mtfdlop].Free();
    mtp->FDLmp = (char (*)[MAXSYMB])aObj[ o_mtfdlmp].Free();
    mtp->MGPid = (char (*)[MAXSYMB])aObj[ o_mtmpgid].Free();
    mtp->UMGP = (char *)aObj[ o_mtumpg].Free();
    mtp->SBM = (char (*)[MAXICNAME+MAXSYMB])aObj[ o_mtbm].Free();
    plot = (TPlotLine *)aObj[ o_mtplline].Free();
   mtp->BSF = (double *)aObj[ o_mtbsf].Free();
   mtp->MB = (double *)aObj[ o_mtmb].Free();
   mtp->dMB = (double *)aObj[ o_mtdmb].Free();
   mtp->DDc = (double*)aObj[ o_mtddc].Free();
   mtp->DIc = (double*)aObj[ o_mtdic].Free();
   mtp->DEl = (double*)aObj[ o_mtdel].Free();
   mtp->for_e = (char (*)[MAXFORMUNITDT])aObj[ o_mtfor_e].Free();
   mtp->xIC = (long int *)aObj[o_mt_xic].Free();
   mtp->xDC = (long int *)aObj[o_mt_xdc].Free();
   mtp->xPH = (long int *)aObj[o_mt_xph].Free();
   mtp->grid = (double (*)[3])aObj[o_mtgrid].Free();
   mtp->NPmean = (long int *)aObj[o_mtnpmean].Free();
   mtp->nPmin = (long int *)aObj[o_mtnpmin].Free();
   mtp->nPmax = (long int *)aObj[o_mtnpmax].Free();
   mtp->ParTD = (long int (*)[6])aObj[o_mtpartd].Free();

mtp->arr1 = (double *)aObj[ o_mtres1].Free();
mtp->arr2 = (double *)aObj[ o_mtres2].Free();

// work
   mtp->An = (double *)aObj[ o_mtan].Free();
   mtp->Ae = (double *)aObj[ o_mtae].Free();
   mtp->gfc = (double *)aObj[ o_mtgfc].Free();
   mtp->yfb = (double *)aObj[ o_mtyfb].Free();
   mtp->tt = (double (*)[9])aObj[ o_mttt].Free();
   mtp->etext = (char *)aObj[ o_mwetext].Free();
   mtp->tprn = (char *)aObj[ o_mwtprn].Free();
   FreeNa();
   freeNodeWork();
}

// realloc dynamic memory
void TGEM2MT::dyn_new(int q)
{
  ErrorIf( mtp!=&mt[q], GetName(),
      "E04GTrem: Attempt to access corrupted dynamic memory.");

 mtp->xIC = (long int *)aObj[o_mt_xic].Alloc( mtp->nICb, 1, L_);
 mtp->xDC = (long int *)aObj[o_mt_xdc].Alloc( mtp->nDCb, 1, L_);
 mtp->xPH = (long int *)aObj[o_mt_xph].Alloc( mtp->nPHb, 1, L_);


 if( mtp->PvGrid == S_OFF )
   mtp->grid = (double (*)[3])aObj[o_mtgrid].Free();
 else
   mtp->grid = (double (*)[3])aObj[o_mtgrid].Alloc( mtp->nC, 3, D_ );

 if( mtp->PsMode == RMT_MODE_W  )
 {
   mtp->NPmean = (long int *)aObj[o_mtnpmean].Alloc( mtp->nPTypes, 1, L_ );
   mtp->nPmin = (long int *)aObj[o_mtnpmin].Alloc( mtp->nPTypes, 1, L_ );
   mtp->nPmax = (long int *)aObj[o_mtnpmax].Alloc( mtp->nPTypes, 1, L_ );
   mtp->ParTD = (long int (*)[6])aObj[o_mtpartd].Alloc( mtp->nPTypes, 6, L_ );
 }
 else
 {
   mtp->NPmean = (long int *)aObj[o_mtnpmean].Free();
   mtp->nPmin = (long int *)aObj[o_mtnpmin].Free();
   mtp->nPmax = (long int *)aObj[o_mtnpmax].Free();
   mtp->ParTD = (long int (*)[6])aObj[o_mtpartd].Free();
 }


 mtp->nam_i=(char (*)[MAXIDNAME])aObj[o_mtnam_i].Alloc( mtp->nIV, 1, MAXIDNAME);
 mtp->PTVm = (double (*)[5])aObj[ o_mtptvi].Alloc(mtp->nIV, 5, D_);


 mtp->DiCp = (long int (*)[2])aObj[ o_mtdicp].Alloc( mtp->nC, 2, L_);
 mtp->StaP = (double (*)[4])aObj[ o_mtstap].Alloc( mtp->nC, 4, D_);

 if( mtp->PvnVTK == S_OFF )
   mtp->xVTKfld = (long int (*)[2])aObj[ o_mtxflds].Free();
 else
   mtp->xVTKfld = (long int (*)[2])aObj[ o_mtxflds].Alloc( mtp->nVTKfld, 2, L_ );

 mtp->stld = (char (*)[EQ_RKLEN])aObj[ o_mtstld].Alloc( mtp->nIV, 1, EQ_RKLEN);

 mtp->Tval  = (double *)aObj[ o_mttval ].Alloc( mtp->nTai, 1, D_);
 mtp->Pval  = (double *)aObj[ o_mtpval ].Alloc( mtp->nPai, 1, D_);

 mtp->Bn = (double *)aObj[ o_mtbn].Alloc( mtp->nIV, mtp->Nb, D_);
 mtp->SBM = (char (*)[MAXICNAME+MAXSYMB])aObj[ o_mtbm].Alloc(
             1, mtp->Nb, MAXICNAME+MAXSYMB);

 if( mtp->PsMode != RMT_MODE_S  && mtp->PsMode != RMT_MODE_F && mtp->PsMode != RMT_MODE_B )
 {
     mtp->HydP = (double (*)[SIZE_HYDP])aObj[ o_mthydp].Alloc(
         mtp->nC,SIZE_HYDP, D_);
  }
 else
    {
      mtp->HydP = (double (*)[SIZE_HYDP])aObj[ o_mthydp].Free();
    }


 if( mtp->PvICi == S_OFF )
    {
      mtp->CIb = (double *)aObj[ o_mtcib].Free();
      mtp->CIclb = (char *)aObj[ o_mtciclb].Free();
    }
    else
    {
      mtp->CIb = (double *)aObj[ o_mtcib].Alloc( mtp->nIV, mtp->Nb, D_);
      mtp->CIclb = (char *)aObj[ o_mtciclb].Alloc(1, mtp->Nb, A_);
    }

if( mtp->PvAUi == S_OFF )
    {
      mtp->CAb = (double *)aObj[ o_mtcab].Free();
      mtp->for_i = (char (*)[MAXFORMUNITDT])aObj[ o_mtfor_i].Free();
      mtp->AUcln = (char *)aObj[ o_mtaucln].Free();
      mtp->An = (double *)aObj[ o_mtan].Free();
      mtp->Lbi = 0;
    }
    else
    {
      mtp->CAb = (double *)aObj[ o_mtcab ].Alloc( mtp->nIV, mtp->Lbi, D_);
      mtp->for_i = (char (*)[MAXFORMUNITDT])aObj[ o_mtfor_i].Alloc(
          1, mtp->Lbi, MAXFORMUNITDT);
      mtp->AUcln = (char *)aObj[ o_mtaucln].Alloc( 1, mtp->Lbi, A_);
      mtp->An = (double *)aObj[ o_mtan].Alloc( mtp->Lbi, mtp->Nb, D_ );
    }

if( mtp->PvFDL == S_OFF )
   {
     mtp->FDLi = (long int (*)[2])aObj[ o_mtfdli].Free();
     mtp->FDLf = (double (*)[4])aObj[ o_mtfdlf].Free();
     mtp->FDLid = (char (*)[MAXSYMB])aObj[ o_mtfdlid].Free();
     mtp->FDLop = (char (*)[MAXSYMB])aObj[ o_mtfdlop].Free();
     mtp->FDLmp = (char (*)[MAXSYMB])aObj[ o_mtfdlmp].Free();
     mtp->nFD = 0;
  }
   else
   {
      mtp->FDLi = (long int (*)[2])aObj[ o_mtfdli ].Alloc( mtp->nFD, 2, L_);
      mtp->FDLf = (double (*)[4])aObj[ o_mtfdlf ].Alloc( mtp->nFD, 4, D_);
      mtp->FDLid=(char (*)[MAXSYMB])aObj[o_mtfdlid].Alloc( mtp->nFD, 1, MAXSYMB);
      mtp->FDLop=(char (*)[MAXSYMB])aObj[o_mtfdlop].Alloc( mtp->nFD, 1, MAXSYMB);
      mtp->FDLmp = (char (*)[MAXSYMB])aObj[ o_mtfdlmp ].Alloc(
                    mtp->nFD, 1, MAXSYMB);
   }

 if( mtp->PvPGD == S_OFF )
   {
     mtp->PGT = (double *)aObj[ o_mtpgt].Free();
     mtp->MGPid = (char (*)[MAXSYMB])aObj[ o_mtmpgid].Free();
     mtp->UMGP = (char *)aObj[ o_mtumpg].Free();
     mtp->nPG = 0;
   }
   else
   {
      mtp->PGT  = (double *)aObj[ o_mtpgt ].Alloc( mtp->FIf, mtp->nPG, D_);
      mtp->MGPid = (char (*)[MAXSYMB])aObj[ o_mtmpgid ].Alloc(
                    1, mtp->nPG, MAXSYMB);
      mtp->UMGP = (char *)aObj[ o_mtumpg].Alloc( mtp->FIf, 1, A_);
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


   if( mtp->PvDDc == S_OFF )
     mtp->DDc = (double*)aObj[ o_mtddc].Free();
   else
     mtp->DDc = (double*)aObj[ o_mtddc].Alloc( mtp->Lsf, 1, D_);

   if( mtp->PvDIc == S_OFF )
     mtp->DIc = (double*)aObj[ o_mtdic].Free();
   else
     mtp->DIc = (double*)aObj[ o_mtdic].Alloc( mtp->Nf, 1, D_);

   if( mtp->nEl <= 0  )
   {
     mtp->DEl = (double*)aObj[ o_mtdel].Free();
     mtp->for_e = (char (*)[MAXFORMUNITDT])aObj[ o_mtfor_e].Free();
     mtp->Ae = (double*)aObj[ o_mtae].Free();
     mtp->nEl = 0;
   }
   else
   {
     mtp->DEl = (double*)aObj[ o_mtdel].Alloc( mtp->nEl, 1, D_);
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

   if( mtp->PvMSt == S_OFF )
      mtp->tExpr = (char *)aObj[o_mttexpr].Free();
   else
      mtp->tExpr = (char *)aObj[ o_mttexpr ].Alloc(1, 4096, S_);

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
      mtp->xEt = (double *)aObj[ o_mtxet].Free();
      mtp->yEt = (double *)aObj[ o_mtyet].Free();
      mtp->nE = mtp->nYE = 0;
    }
    else
    {
        mtp->lNamE = (char (*)[MAXGRNAME])aObj[ o_mtlname ].Alloc(
                              1, mtp->nYE, MAXGRNAME);
        mtp->xEt   = (double *)aObj[ o_mtxet ].Alloc(mtp->nE, 1, D_);
        mtp->yEt   = (double *)aObj[ o_mtyet ].Alloc(mtp->nE, mtp->nYE, D_);
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

mtp->arr1 = (double *)aObj[ o_mtres1].Free();
mtp->arr2 = (double *)aObj[ o_mtres2].Free();

    mtp->etext = (char *)aObj[ o_mwetext ].Alloc(1, 4096, S_);
    mtp->tprn = (char *)aObj[ o_mwtprn ].Alloc(1, 2048, S_);


    //mtp->gfc = (double *)aObj[ o_mtgfc].Free();
    //mtp->yfb = (double *)aObj[ o_mtyfb].Free();
    //mtp->tt = (double *)aObj[ o_mttt].Free();

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
    char flgs[32];
    int size[20];

    // set up for mathscripts
    double Tai[4], Pai[4];

    Tai[0] = Tai[1] = TMulti::sm->GetPM()->TCc;
    Pai[0] = Pai[1] = TMulti::sm->GetPM()->Pc;
    Tai[2] = Pai[2] = 0.;
    Tai[3] = Pai[3] = 0.1;

    FreeNa();
    na = new TNodeArray( 1, TMulti::sm->GetPM() );
    // realloc and setup data for dataCH and DataBr structures
    na->MakeNodeStructures( 0, true , Tai, Pai  );

    // setup dataBR and NodeT0 data
    //na->packDataBr();
    na->MoveWorkNodeToArray( 0, 1, na->pNodT0() );
    na->CopyNodeFromTo( 0, 1, na->pNodT0(), na->pNodT1() );

    allocNodeWork();
    LinkCSD(0);
    LinkNode0(0);
    LinkNode1(0);

    p_key  = db->PackKey();
    memcpy( flgs, &mtp->PunE, 32);
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
    size[17] = mtp->Nqpt;
    size[18] = mtp->nVTKfld;
    size[19] = mtp->nRes1;

    // for scripts
    TCStringArray namesLines;
    gstring calcScript="";
    gstring outScript="";
    if( mtp->tExpr )
     calcScript = mtp->tExpr;
    if( mtp->gExpr )
      outScript = mtp->gExpr;
    gstring xName = mtp->xNames;
    gstring yName = mtp->yNames;


    TCIntArray vtk1;
    TCIntArray vtk2;
    if( mtp->PvnVTK != S_OFF )
    { // use selected fields to VTK
      for( long int ii=0; ii<mtp->nVTKfld; ii++)
       { vtk1.Add( mtp->xVTKfld[ii][0]);
         vtk2.Add( mtp->xVTKfld[ii][1]);
       }
    }

    if( !vfGEM2MTSet( window(), p_key, flgs, size, mtp->Tai, mtp->Pai,mtp->Tau,
                    calcScript, outScript, namesLines, xName, yName, vtk1, vtk2 ))
         Error( p_key, "GEM2MT record configuration cancelled by the user!" );
     //  return;   // cancel

    memcpy( &mtp->PunE, flgs, 32);
    mtp->nC = (long int)size[0];
    mtp->nIV = (long int)size[1];
    mtp->ntM = (long int)size[2];
    mtp->nPG = (long int)size[3];
    mtp->nFD = (long int)size[4];
    mtp->nSFD = (long int)size[5];
    mtp->nYS = (long int)size[6];
    mtp->nE = (long int)size[7];
    mtp->nYE = (long int)size[8];
    mtp->Lbi = (long int)size[9];
    mtp->nEl = (long int)size[10];
    mtp->Nqpg = (long int)size[11];
    mtp->nPai = (long int)size[12];
    mtp->nTai = (long int)size[13];
    mtp->Nsd = (long int)size[14];
    mtp->nPTypes = (long int)size[15];
    mtp->nProps = (long int)size[16];
    mtp->Nqpt = (long int)size[17];
    mtp->nVTKfld = (long int)size[18];
    mtp->nRes1 = (long int)size[19];

    // from scripts


    if( !outScript.empty() )
    { if( !mtp->gExpr )
         mtp->gExpr = (char *)aObj[ o_mtgexpr ].Alloc(1, 2048, S_);
      aObj[o_mtgexpr].SetString( outScript.c_str(),0,0);
    }

    if(namesLines.GetCount() > 0)
    {
        mtp->lNam = (char (*)[MAXGRNAME])aObj[ o_mtlnam ].Alloc(
                      1, mtp->nYS, MAXGRNAME);
       for(long int ii=0; ii< min( (long int)namesLines.GetCount(),mtp->nYS); ii++)
       {
           strncpy(  mtp->lNam[ii], namesLines[ii].c_str(), MAXGRNAME );
        }
       strncpy(mtp->xNames, xName.c_str(), MAXAXISNAME );
       strncpy(mtp->yNames, yName.c_str(), MAXAXISNAME );
    }

  if( !calcScript.empty() )
   { if( !mtp->tExpr )
        mtp->tExpr = (char *)aObj[ o_mttexpr ].Alloc(1, 4096, S_);
     aObj[o_mttexpr].SetString( calcScript.c_str(),0,0);
   }

  if( mtp->PvnVTK != S_OFF )
  { // use selected fields to VTK
      mtp->nVTKfld = vtk1.GetCount();
      mtp->xVTKfld = (long int (*)[2])aObj[ o_mtxflds].Alloc( mtp->nVTKfld, 2, L_ );
     for( long int ii=0; ii<mtp->nVTKfld; ii++)
     { mtp->xVTKfld[ii][0] = vtk1[ii];
       mtp->xVTKfld[ii][1] = vtk2[ii];
     }
  }
}

//Rebild record structure before calc
int
TGEM2MT::RecBuild( const char *key, int mode )
{
    if( pVisor->ProfileMode != true )
      Error( GetName(), "E09DTrem: Please, do it in the Project mode!" );

    bool setdef = false;

    /* set up for mathscripts
    double Tai[4], Pai[4];

    FreeNa();
    na = new TNodeArray( 1, TMulti::sm->GetPM() );
    // realloc and setup data for dataCH and DataBr structures
    na->MakeNodeStructures( 0, true , Tai, Pai  );

    // setup dataBR and NodeT0 data
    //na->packDataBr();
    na->MoveWorkNodeToArray( 0, 1, na->pNodT0() );
    na->CopyNodeFromTo( 0, 1, na->pNodT0(), na->pNodT1() );

    allocNodeWork();
    LinkCSD(0);
    LinkNode0(0);
    LinkNode1(0);
 */
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

    SelectNodeStructures( mtp->PvDCH==S_OFF );
    //dyn_new() in function SelectNodeStructures

    init_arrays( setdef ); //clear all

    SetString("Record build OK");
    pVisor->Update();

    //FreeNa();
    return ret;
}

void TGEM2MT::AllocNa()
{
  FreeNa();
  na = new TNodeArray( mtp->nC, TMulti::sm->GetPM() );

  // use particles
  if( mtp->PsMode == RMT_MODE_W  )
  {
     na->SetGrid( mtp->sizeLc, mtp->grid );   // set up grid structure
     pa = new TParticleArray( mtp->nPTypes, mtp->nProps,
           mtp->NPmean, mtp->ParTD, mtp->nPmin, mtp->nPmax, na );
   }
}

void TGEM2MT::FreeNa()
{
  if( pa )
  { delete pa;
    pa = 0;
  }
  if(na)
   delete na;
   na = 0;
//   freeNodeWork();
   LinkNode0(-1);
   LinkNode1(-1);
   LinkCSD(-1);
 }


//Calculate record
void
TGEM2MT::RecCalc( const char * key )
{
 try
 {
    bool iRet;

     if( pVisor->ProfileMode != true  )
       Error( GetName(), "E02GTexec: Please, do it in the Equilibria Calculation mode" );

   if( mtp->PsVTK != S_OFF )
   {
     prefixVTK = gstring( db->FldKey(8), 0, db->FldLen(8) );
     prefixVTK.strip();
     prefixVTK += "_";
     prefixVTK += gstring( db->FldKey(9), 0, db->FldLen(9) );
     prefixVTK.strip();
     prefixVTK += "_";
     nameVTK = db->PackKey();
     nameVTK = nameVTK.replaceall(":", "_");

     if( vfChooseDirectory(window(), pathVTK,
          "Please, browse or enter the GEM2MT VTK output directory name" ) )
     {
         pathVTK += "/";
         pathVTK += nameVTK.c_str();
         pathVTK += "/";
         vfMakeDirectory(window(), pathVTK.c_str() );
     }
   }

   AllocNa();

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
       gstring lst_f_name;
       gstring dbr_lst_f_name;
     if( mtp->notes[0] == '@' )
     {   lst_f_name = gstring( mtp->notes, 1, MAXFORMULA-1 );
         dbr_lst_f_name = lst_f_name;
         dbr_lst_f_name = dbr_lst_f_name.replace("-dat","-dbr");
      } // open file to read
     else
     {  if( vfChooseFileOpen(window(), lst_f_name,
          "Please, edit the GEMS3K filelist name", "*.lst" ) == false )
            return;
        if( vfChooseFileOpen(window(), dbr_lst_f_name,
                   "Please, edit the GEMS3K filelist name", "*.lst" ) == false )
             return;
      }
     //TMulti::sm->Free_TSolMod();
     na->GEM_init( lst_f_name.c_str(), dbr_lst_f_name.c_str(), 0, true );
     CalcIPM( NEED_GEM_AIA, 0, mtp->nC, 0 );
     if( mtp->PsMode == RMT_MODE_W  )
     {
      putHydP( na->pNodT0() );
      putHydP( na->pNodT1() );
      pa->setUpCounters();
     }
    }

   if( mtp->PvMSt != S_OFF )
        Expr_analyze( o_mttexpr );

   TCModule::RecCalc( key );

   if( mtp->PsMode == RMT_MODE_T )
   {   // calculate start data
     mtp->iStat = AS_READY;
     CalcStartScript();
     outMulti();
     mtp->iStat = AS_DONE;
   }
   else
   // if( mtp->PsMode == 'A' || mtp->PsMode == 'D' || mtp->PsMode == 'T' ) 'W' 'V'
   {   // calculate start data

      if( mtp->iStat != AS_RUN )
      {
          CalcStartScript();
          NewNodeArray();  // set up start DATACH structure and DATABR arrays structure
      }

      allocNodeWork();
      LinkCSD(0);

     if( mtp->PvMSg != S_OFF )
         Expr_analyze( o_mtgexpr );
     
       // internal calc
  #ifdef Use_mt_mode
     if( mtp->PsSmode != S_OFF  )
     { // use thread
        userCancel = false;
        try
        {
           pVisor->ProcessProgress(window(), RT_GEM2MT );
         }
         catch( TError& xcpt )
         {
           mtp->gStat = GS_ERR;
           mtp->iStat = AS_INDEF;
           vfMessage(window(), xcpt.title, xcpt.mess);
         }
     }
     else
     {  iRet = internalCalc();
         if(iRet)
           savePoint();
         else
            mtp->iStat = AS_DONE;
     }
#else
      iRet = internalCalc();
      if(iRet)
        savePoint();
      else
         mtp->iStat = AS_DONE;
#endif
   }

  //FreeNa();
  }
  catch( TError& xcpt )
  {
     //FreeNa();
     mtp->gStat = GS_ERR;
     mtp->iStat = AS_INDEF;
     Error(  xcpt.title.c_str(), xcpt.mess.c_str() );
  }
}

void TGEM2MT::savePoint( )
{
    // canceled calculations
       if( vfQuestion( window(), "GEM2MT task interrupt",
          "RMT calculations have been stopped by the user.\n"
          "Save node array into a set of disk files to resume the task later (Yes),\n"
          " or cancel the RMT task (No)?" ) )
       {
         gstring path = na->PutGEM2MTFiles( window(), mtp->nC,
           mtp->PsSdat==S_OFF, mtp->PsSdef!=S_OFF, mtp->PsScom!=S_OFF, true, true ); // with Nod0 and Nod1
         mtp->notes[0] = '@';
         strncpy( mtp->notes+1, path.c_str(), MAXFORMULA-1 );
         // save GEM2MT recort
         gstring key_str = db->PackKey();
         RecSave( key_str.c_str() );
         //na->GEM_init( path.c_str(), 0, true );//test
         mtp->iStat = AS_RUN;
       }
 }

const char* TGEM2MT::GetHtml()
{
   return GM_GEM2MT_HTML;
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
            //mtp->PsScom=S_OFF;
            fstream ff( filename.c_str(), ios::out );
	        ErrorIf( !ff.good() , filename.c_str(), "Fileopen error");
            to_text_file( ff,  mtp->PsScom!=S_OFF, mtp->PsSdef!=S_OFF, filename.c_str() );

		}
    na = new TNodeArray( mtp->nC, TMulti::sm->GetPM() );
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
    char  *p_CIclb=0;
    double *p_CIb=0;
    double *p_Bn = new double[mtp->nIV*mtp->Nb];
    memcpy( p_Bn, mtp->Bn, mtp->nIV*mtp->Nb*sizeof(double));
    if( mtp->PvICi != S_OFF )
    {
      p_CIclb = new char[mtp->Nb];
      memcpy( p_CIclb, mtp->CIclb, mtp->Nb*sizeof(char));
      p_CIb = new double[mtp->nIV*mtp->Nb];
      memcpy( p_CIb, mtp->CIb, mtp->nIV*mtp->Nb*sizeof(double));
    }


    // Before calculation better rebuild record
    // We not insert changes to xDC, xIC, xPH and set flag to sellect all records to DBR
       mtp->PvDCH = S_OFF;
    // !!!! When you changed numbers of
    //   mtp->nICb; // number of stoichiometry units (<= nIC) used in the data bridge
    // you must changed arrays BSF and DIc,
    //   mtp->nDCb; // number of DC (chemical species, <= nDC) used in the data bridge
    // you must changed arrays BSF and DDc
    //   mtp->nPHb;   // number of phases (<= nPH) used in the data bridge
    // you must changed arrays PGT and UMGP

    // alloc new memory
    mtp->Nb = TRMults::sm->GetMU()->N;
    mtp->FIb = TRMults::sm->GetMU()->Fi;
    mtp->Lb = TRMults::sm->GetMU()->L;

    dyn_new();

//***************************************************
    for( ii=0; ii< mtp->Nb; ii++ )
       memcpy( mtp->SBM[ii], TRMults::sm->GetMU()->SB[ii], MAXICNAME  );

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
          }
        jj++;
        ii++;
       }
    }


// free memory
   delete[] p_Bn;
   if( mtp->PvICi != S_OFF )
   {
     delete[] p_CIclb;
     delete[] p_CIb;
    }

}

// --------------------- end of m_gem2mt.cpp ---------------------------


