//-------------------------------------------------------------------
// $Id$
//
// Implementation of GEM2MT class, config and calculation functions
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
// See http://les.web.psi.ch/Software/GEMS-PSI for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//
const char *GEMS_MT_HTML = "gm_gem2mt";

#include <math.h>
#include <stdio.h>

#include "m_gem2mt.h"
#include "v_object.h"
#include "visor.h"
#include "service.h"

#include "v_mod.h"

GEM2MT* GEM2MT::pmt;

GEM2MT::GEM2MT( int nrt ):
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
    aFldKeysHelp.Add("l<18 Identifier of this coupled model simulator definition");
    aFldKeysHelp.Add("l<4  Batch simulator type code, a combination of {}");

    mtp=&mt[0];
    set_def();
    start_title =
       " Definition of a GEM2MT (Coupled model)";
}

// get key of record
gstring
GEM2MT::GetKeyofRecord( const char *oldKey, const char *strTitle,
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
void GEM2MT::keyTest( const char *key )
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
void GEM2MT::ods_link(int q)
{
    mtp=&mt[q];

// static
    aObj[o_mtpufl].SetPtr( &mtp->PunE );   /* a4 */
    aObj[o_mtpvfl].SetPtr( &mtp->PvICi );  /* a8 */
    aObj[o_mtpsfl].SetPtr( &mtp->PsMode ); /* a8 */
    aObj[o_mtcipf].SetPtr( &mtp->nC );     /* i4 */
    aObj[o_mtszt].SetPtr( &mtp->Lbi );     /* i6 */
    aObj[o_mtnsne].SetPtr( &mtp->nS );   /* i4 */
    aObj[o_mtptai].SetPtr( &mtp->nPai );  /* i2 */
    aObj[o_mttmi].SetPtr( mtp->tmi );     /* i3 */
    aObj[o_mtnvi].SetPtr( mtp->NVi );     /* i3 */
    aObj[o_mtaxis].SetPtr( mtp->axisType ); /* i6 */
    aObj[o_mtpai].SetPtr( mtp->Pai );     /* d3 */
    aObj[o_mttai].SetPtr( mtp->Tai );     /* d3 */
    aObj[o_mttau].SetPtr( mtp->Tau );     /* d3 */
    aObj[o_mtsize].SetPtr( mtp->size[0] );     /* d8 */
// work
    aObj[o_mtsykey].SetPtr( mtp->sykey );
    aObj[o_mtctm].SetPtr( &mtp->ctm );
    aObj[o_mtcnv].SetPtr( &mtp->cnv );
    aObj[o_mtqc].SetPtr( &mtp->qc );
    aObj[o_mtkv].SetPtr( &mtp->kv );
    aObj[o_mtjqc].SetPtr( &mtp->jqc );
    aObj[o_mtjqs].SetPtr( &mtp->jqs );
    aObj[o_mtjt].SetPtr( &mtp->jt );
    aObj[o_mtrei1].SetPtr( &mtp->rei1 );
    aObj[o_mtrei2].SetPtr( &mtp->rei2 );
    aObj[o_mtrei3].SetPtr( &mtp->rei3 );
    aObj[o_mtrei4].SetPtr( &mtp->rei4 );
    aObj[o_mtrei5].SetPtr( &mtp->rei5 );
    aObj[o_mtct].SetPtr( &mtp->cT );
    aObj[o_mtcp].SetPtr( &mtp->cP );
    aObj[o_mtcv].SetPtr( &mtp->cV );
    aObj[o_mtctau].SetPtr( &mtp->cTau );
    aObj[o_mtdtau].SetPtr( &mtp->dTau );
    aObj[o_mtotau].SetPtr( &mtp->oTau );
    aObj[o_mtref1].SetPtr( &mtp->ref1 );
    aObj[o_mtref2].SetPtr( &mtp->ref2 );
    aObj[o_mtref3].SetPtr( &mtp->ref3 );
    aObj[o_mtref4].SetPtr( &mtp->ref4 );

// DBase 46
    aObj[ o_mtname].SetPtr(  mtp->name );
    aObj[ o_mtnotes].SetPtr(   mtp->notes );
    aObj[o_mtflag].SetPtr( &mtp->PunE );    /* a20 */
    aObj[o_mtshort].SetPtr( &mtp->nC );     /* i28 */
    aObj[o_mtdoudl].SetPtr( &mtp->Msysb );    /* d9 */
    aObj[o_mtfloat].SetPtr( mtp->Pai );    /* f17 */
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
    aObj[ o_mtdicp].SetDim( mtp->nC, 1 );
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
    aObj[ o_mtqpi].SetPtr( mtp->qpi);
    aObj[ o_mtqpi].SetDim( mtp->Nqpt, 1 );
    aObj[ o_mtqpc].SetPtr( mtp->qpc);
    aObj[ o_mtqpc].SetDim( mtp->Nqpg, 1 );
    aObj[ o_mtxt].SetPtr( mtp->xt);
    aObj[ o_mtxt].SetDim( mtp->nS, 1 );
    aObj[ o_mtyt].SetPtr( mtp->yt);
    aObj[ o_mtyt].SetDim( mtp->nS, mtp->nYS );
    aObj[ o_mtcib].SetPtr( mtp->CIb);
    aObj[ o_mtcib].SetDim( mtp->nIV, mtp->Nb );
    aObj[ o_mtcin].SetPtr( mtp->CIn);
    aObj[ o_mtcin].SetDim( mtp->nIV, mtp->Nb );
    aObj[ o_mtcab].SetPtr( mtp->CAn);
    aObj[ o_mtcab].SetDim( mtp->nIV, mtp->Lbi );
    aObj[ o_mtfdlf].SetPtr( mtp->FDLf);
    aObj[ o_mtfdlf].SetDim( mtp->nFD, 2 );
    aObj[ o_mtpgt].SetPtr( mtp->PGT);
    aObj[ o_mtpgt].SetDim( ->Fi, mtp->nPG );       //???????????????????
    aObj[ o_mtnam_i].SetPtr( mtp->nam_i );
    aObj[ o_mtnam_i].SetDim(  mtp->nIV, 1 );
    aObj[ o_mtfor_i].SetPtr( mtp->for_i );
    aObj[ o_mtfor_i].SetDim(  mtp->Lbi, 1 );
    aObj[ o_mtstld].SetPtr( mtp->stld );
    aObj[ o_mtstld].SetDim(  mtp->nIV, 1 );
    aObj[ o_mtciclb].SetPtr( mtp->CIclb );
    aObj[ o_mtciclb].SetDim(  mtp->Nb, 1 );
    aObj[ o_mtaucln].SetPtr( mtp->AUcln );
    aObj[ o_mtaucln].SetDim(  mtp->Lbi, 1 );
    aObj[ o_mtfdlid].SetPtr( mtp->FDLid );
    aObj[ o_mtfdlid].SetDim(  mtp->nFD, 1 );
    aObj[ o_mtfdlop].SetPtr( mtp->FDLop );
    aObj[ o_mtfdlop].SetDim(  mtp->nFD, 1 );
    aObj[ o_mtfdlmp].SetPtr( mtp->FDLmp );
    aObj[ o_mtfdlmp].SetDim(  mtp->nPG, 1 );
    aObj[ o_mtmpgid].SetPtr( mtp->MPGid );
    aObj[ o_mtmpgid].SetDim(  mtp->nPG, 1 );
    aObj[ o_mtumpg].SetPtr( mtp->UMPG );
    aObj[ o_mtumpg].SetDim(  ->Fi, 1 );     //????????????????
    aObj[ o_mtbm].SetPtr( mtp->SBM );
    aObj[ o_mtbm].SetDim(  mtp->Nb, 1 );
    aObj[ o_mtplline].SetPtr( plot );
    aObj[ o_mtplline].SetDim( mtp->nYS+mtp->nYE,  sizeof(TPlotLine));
// work
    aObj[ o_mwetext].SetPtr( mtp->etext );
    // aObj[ o_mwetext].SetDim(1, 0 );
    aObj[ o_mwtprn].SetPtr(  mtp->tprn  );
    // aObj[ o_mwtprn].SetDim( 1,l( mtp->tprn));

}

// set dynamic Objects ptr to values
void GEM2MT::dyn_set(int q)
{
    ErrorIf( mtp!=&mt[q], GetName(),
       "E06GDrem: Illegal access to mt in dyn_set");

    mtp->lNam[0] = = (char (*)[MAXGRNAME])aObj[ o_mtlnam].GetPtr();
    mtp->lNamE[0] = (char (*)[MAXGRNAME])aObj[ o_mtlname].GetPtr();
    mtp->tExpr = (char *)aObj[o_mttexpr].GetPtr();
    mtp->gExpr = (char *)aObj[o_mtgexpr].GetPtr();
    mtp->sdref = (char (*)[V_SD_RKLEN])aObj[ o_mtsdref].GetPtr();
    mtp->sdval = (char (*)[V_SD_VALEN])aObj[ o_mtsdval].GetPtr();
    mtp->DiCp = (short *)aObj[ o_mtdicp].GetPtr();
    mtp->FDLi = (short (*)[2])aObj[ o_mtfdli].GetPtr();
    mtp->Pi = (float *)aObj[ o_mtpi].GetPtr();
    mtp->Ti = (float *)aObj[ o_mtti].GetPtr();
    mtp->Vi = (float *)aObj[ o_mtvi].GetPtr();
    mtp->xEt = (float *)aObj[ o_mtxet].GetPtr();
    mtp->yEt = (float *)aObj[ o_mtyet].GetPtr();
    mtp->Bn = (double *)aObj[ o_mtbn].GetPtr();
    mtp->qpi = (double *)aObj[ o_mtqpi].GetPtr();
    mtp->qpc = (double *)aObj[ o_mtqpc].GetPtr();
    mtp->xt = (double *)aObj[ o_mtxt].GetPtr();
    mtp->yt = (double *)aObj[ o_mtyt].GetPtr();
    mtp->CIb = (float *)aObj[ o_mtcib].GetPtr();
    mtp->CIn = (float *)aObj[ o_mtcin].GetPtr();
    mtp->CAn = (float *)aObj[ o_mtcab].GetPtr();
    mtp->FDLf = (float (*)[2])aObj[ o_mtfdlf].GetPtr();
    mtp->PGT = (float *)aObj[ o_mtpgt].GetPtr();
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
// work
    mtp->etext = (char *)aObj[ o_mwetext].GetPtr();
    mtp->tprn = (char *)aObj[ o_mwtprn].GetPtr();
}

// free dynamic memory in objects and values
void GEM2MT::dyn_kill(int q)
{
    ErrorIf( mtp!=&mt[q], GetName(),
       "E05DTrem: Illegal access to mt in dyn_kill");

    mtp->lNam[0] = = (char (*)[MAXGRNAME])aObj[ o_mtlnam].Free();
    mtp->lNamE[0] = (char (*)[MAXGRNAME])aObj[ o_mtlname].Free();
    mtp->tExpr = (char *)aObj[o_mttexpr].Free();
    mtp->gExpr = (char *)aObj[o_mtgexpr].Free();
    mtp->sdref = (char (*)[V_SD_RKLEN])aObj[ o_mtsdref].Free();
    mtp->sdval = (char (*)[V_SD_VALEN])aObj[ o_mtsdval].Free();
    mtp->DiCp = (short *)aObj[ o_mtdicp].Free();
    mtp->FDLi = (short (*)[2])aObj[ o_mtfdli].Free();
    mtp->Pi = (float *)aObj[ o_mtpi].Free();
    mtp->Ti = (float *)aObj[ o_mtti].Free();
    mtp->Vi = (float *)aObj[ o_mtvi].Free();
    mtp->xEt = (float *)aObj[ o_mtxet].Free();
    mtp->yEt = (float *)aObj[ o_mtyet].Free();
    mtp->Bn = (double *)aObj[ o_mtbn].Free();
    mtp->qpi = (double *)aObj[ o_mtqpi].Free();
    mtp->qpc = (double *)aObj[ o_mtqpc].Free();
    mtp->xt = (double *)aObj[ o_mtxt].Free();
    mtp->yt = (double *)aObj[ o_mtyt].Free();
    mtp->CIb = (float *)aObj[ o_mtcib].Free();
    mtp->CIn = (float *)aObj[ o_mtcin].Free();
    mtp->CAn = (float *)aObj[ o_mtcab].Free();
    mtp->FDLf = (float (*)[2])aObj[ o_mtfdlf].Free();
    mtp->PGT = (float *)aObj[ o_mtpgt].Free();
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
// work
    mtp->etext = (char *)aObj[ o_mwetext].Free();
    mtp->tprn = (char *)aObj[ o_mwtprn].Free();
}

// realloc dynamic memory
void GEM2MT::dyn_new(int q)
{
  ErrorIf( mtp!=&mt[q], GetName(),
      "E04DTrem: Illegal access to mt in dyn_new.");



}

//set default information
void TInteg::set_def(int)
{
    name[0][0] = '\0';
    name[1][0] = '\0';
    Nequ = 1;
    x_bg =0.;
    x_end = 1.;
    step_bg = 1.;
    Eps = 1e-10;
    step =0.;
    nfcn = nstep = naccept = nrejct = 0;
    arg_x = 0.;
    Bsize = 0;
    MaxIter = MAXITER;

    y_bg = 0;
    param = 0;
    TxtEqu = 0;
    val_y = 0;
    valdy = 0;
    allx = 0;
    ally = 0;
    allpr = 0;
    allst = 0;
    dyn_new();
}

// Input nessasery data and links objects
void TInteg::RecInput( const char *key )
{
    //  strncpy( keywd, key, 24 );
    TCModule::RecInput( key );
}

//Rebild record structure before calc
int
TInteg::RecBuild( const char *key, int mode )
{
    int ret = TCModule::RecBuild( key, mode );
    memcpy( keywd, key, 23 );
    if( ret != VF3_3 )
    {  //memcpy( keywd, key, 23 );
        step = step_bg;
    }
    dyn_new();
    SetString("Rec build OK");
    pVisor->Update();
    return ret;
}

//Calculate record
void
TInteg::RecCalc( const char * key )
{
    int Npoint, i;
    TCModule::RecCalc( key );

    //Расчет
    //Начальные установки
    dyn_new();
    arg_x = x_bg;
    for( i=0; i<Nequ; i++ )
        val_y[i] = y_bg[i];
    step = step_bg;
    nfcn = nstep = naccept = nrejct = 0;


    SetString("Integ: Calculating...");
    pVisor->Update();
    //сброс-настройка графики
    // запрос на поточечный вывод wi->FlagGraf = YNonly( msg(iv_yn_graph));
    char* ss = (char *)aObj[o_ig_txt].GetPtr();
    rpn.GetEquat( ss );  //Get IPN of equats
    // настройка экрана для поточечного вывода
    INTEG( o_ival_y, o_ivaldy, o_ival_x, Eps, step, x_bg, x_end );
    // rpn.CalcEquat();
    // realloc arrays for Npoint
    Npoint = naccept+1;
    allx = (double *)aObj[o_i_allx].Alloc( Npoint, 1 );
    ally = (double *)aObj[o_i_ally].Alloc( Npoint, Nequ );
    allst = (double *)aObj[o_iallst].Alloc( Npoint, 1 );
    allpr = (short *)aObj[o_iallpr].Alloc( Npoint, 1 );
    // установка графики после расчета

}

//--------------------------------------------------------------------
// Integration process

const int NMAX = 800;
const int KM = 8;
const double UROUND = 1.73e-18;
const double FAC1 = 2.e-2;
const double FAC2 = 4.0;
const double FAC3 = .9;
const double FAC4 = .8;
const double SAFE1 = .65;
const double SAFE2 = .94;
const double MAXSTEP = 1.7;
const int MAXINTEGEXPR = 200;

double *x;
double *dx;
double *tv;

static double tt[9][ MAXINTEGEXPR ];
static double hh[9];
static double w[9];
static double err;
static double epsd4;
static int    nj[9]={2,4,6,8,10,12,14,16,18};
static double a1[9]={3.0,7.0,13.0,21.0,31.0,43.0,57.0,73.0,91.0};

// calculate 1-step from system of equation ( расчет производных по ОПЗ )
void
TInteg::Solut( int ix, int id, int it,
               double *t1, double *t2, double t3 )
{
    aObj[ ix ].SetPtr( t1 );
    aObj[ id ].SetPtr( t2 );
    aObj[ it ].SetPtr( &t3 );
    rpn.CalcEquat();
    aObj[ it ].SetPtr( tv );
}

//Процедура вычисляет j-ю линию экстрополяционной таблицы
//и обеспечивает оптимальный шаг
void
TInteg::MIDEX( int j, double t, double h,
               int ix, int id, int it )
{
    double *z1=0, *z2=0, *dz=0;
    double hj, scal, fac, facmin, expo, ys, v1, v2;
    int i,m,mm,l,n;
    try
    {
        n = aObj[ix].GetN();
        z1 = new double[n];
        z2 = new double[n];
        dz = new double[n];
        memset( z1, 0, sizeof(double)*n );
        memset( z2, 0, sizeof(double)*n );
        memset( dz, 0, sizeof(double)*n );

        hj = h / (double)nj[ j ];
        // начальный шаг
        for( i=0; i < n; i++ )
        {
            z1[ i ] = x[ i ];
            z2[ i ] = x[ i ] + hj * dx[ i ];
        }
        // правило точной средней точки
        m = nj[ j ] - 1;
        for( mm=0; mm < m; mm++ )
        {
            Solut( ix, id, it, z2, dz, t+hj*(double)(mm+1) );
            for( i=0; i < n; i++ )
            {
                ys = z1[ i ];
                z1[ i ] = z2[ i ];
                z2[ i ] = ys + hj * dz[ i ] * 2.;
            }
        }
        // последний сглаживающий шаг
        Solut( ix, id, it, z2, dz, t+h );
        for( i=0; i < n; i++ )
            tt[ j ][ i ] = ( z1[ i ] + z2[ i ] + hj * dz[ i ] ) / 2.;
        nfcn += nj[ j ];
        //полиномиальная экстраполяция
        if(j == 0)
            goto VEL;
        for( l=j; l >= 1; l-- )
        {
            fac = pow( (double)nj[ j ] / (double)nj[ l-1 ], 2.) - 1.;
            for( i=0; i < n; i++ )
                tt[ l-1 ][ i ] = tt[ l ][ i ] +
                                 ( tt[ l ][ i ] - tt[ l-1 ][ i ] ) / fac;
        }
        err = 0e0;
        for( i=0; i < n; i++ )
        { // взбираемся по лестнице
            v2 = fabs( tt[ 0 ][ i ] );
            v1 = fabs( x[ i ] );
            v1 = max( v1, v2 );
            v2 = max( 1.e-6, UROUND / epsd4 );
            scal = max( v1, v2 );
            err += pow( (tt[ 0 ][ i ] - tt[ 1 ][ i ] ) / scal, 2. );
        }
        err = pow( err / (double)n, .5 );
        //вычисление оптимального шага
        expo = 1.e0 / (double)( 2 * ( j + 1 )-1 );
        facmin = pow( FAC1, expo );
        v1 = max( facmin, pow( err/epsd4, expo ) / SAFE2 );
        fac = min( FAC2/facmin, v1 );
        fac= 1.e0 / fac;
        hh[ j ] = min( h * fac, MAXSTEP );
        w[ j ] = a1[ j ] / hh[ j ];

VEL:
        delete[] z1;
        delete[] z2;
        delete[] dz;
    }
    catch( ... )
    {
        if( z1 ) delete[] z1;
        if( z2 ) delete[] z2;
        if( dz ) delete[] dz;
        Error( "INTEG ", "Error in MIXED!");
    }
}

//Вывод результата и перераспределение массивов
// (Поместить на таймер вывод результата)
void
TInteg::PR( int Ni,int pr, double x, double step, double *y )
{
    int i,dimN, dimM;
    dimN = aObj[o_i_allx].GetN();
    dimM = aObj[o_i_ally].GetM();
    if( Ni >= dimN )
    {
        dimN+=INT_BLOK;
        allx = (double *)aObj[o_i_allx].Alloc( dimN, 1 );
        ally = (double *)aObj[o_i_ally].Alloc( dimN, dimM );
        allst = (double *)aObj[o_iallst].Alloc( dimN, 1 );
        allpr = (short *)aObj[o_iallpr].Alloc( dimN, 1 );
    }
    allx[Ni] = x;
    allst[Ni] = step;
    allpr[Ni] = (short)pr;
    for( i=0; i<dimM; i++)
        ally[Ni*dimM+i] = y[i];
    /*if( wi->FlagGraf )  show new point in graphic scren
        { pl[PR_PLOT].CurNdxP[0] = Ni;
          pl[ST_PLOT].CurNdxP[0] = Ni;
          for( i=0; i<ip->Nequ; i++)
             pl[FN_PLOT].CurNdxP[i] = Ni;
          DrawCount (PR_PLOT, 1, 1);
          DrawCount (ST_PLOT, 2, 1);
          DrawCount (FN_PLOT, 3, 1);
        }
      else  */

    // ?????Перерисовка экрана на таймере  i = Vaccess( W_INTEG, msg(iv_wn_calc) );
    if( Ni%100 == 0 )
        pVisor->Update();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Функция INTEG решает задачу Коши вида
//		 dyi/dx = f( y1,y2, ... ,yn,x )  i=1,...,n
//               yi( x0 )=yi0
//  методом єкстраполяции с переменнім порядком и переменной
//  длиной шага
void
TInteg::INTEG( int iy, int id, int ix, double eps,
               double& step, double t_begin, double t_end )
{
    double  t, h1, h, v1;
    int     j,i,reject,last,kc,kopt,k;

    x = (double *)aObj[ iy ].GetPtr();
    dx = (double *)aObj[ id ].GetPtr();
    tv = (double *)aObj[ ix ].GetPtr();

    h = step;
    epsd4 = eps * SAFE1;
    v1 = -log10(eps)*.6+1.5;
    v1 = min( 8.0, v1 );
    k = (int)max( 3.0, v1 ) - 1;
    t = t_begin;
    h1 = t_end-t;
    v1 = min( MAXSTEP, h1/2. );
    h = min( h, v1 );
    PR( 0, k, t, h, x );
    err = w[ 0 ] = 0.0;
    reject = last = 0;   /*false*/

    //достигли конца?
    while( fabs( h1 ) >= UROUND )
    {
        v1 = min( h1, MAXSTEP);
        h = min( h, v1 );
        if( h >= ( h1 - UROUND ) )  last = 1;      /*true*/
        Solut(  iy, id, ix, x, dx, t );
        nfcn++;
        if (( nstep == 0 )||( last ))     //1-й и последний шаг
        {
            nstep++;
            for( j=0; j <= k; j++ )
            {
                kc=j;
                MIDEX( j, t, h,iy, id, ix );
                if( ( j > 0 ) && ( err <= eps ) ) goto l60;
            }
            goto l55;
        }
        //основной шаг интегрирования
l30:
        nstep++;
        ErrorIf( nstep >= MaxIter, "INTEG", "Excess iteration number" );
        kc = k-1;
        for( j=0; j <= kc; j++ )
            MIDEX( j, t, h, iy, id, ix );
        // сходимость в 1-й точке
        if( !( k == 1 || reject ) )
        {
            if( err <= eps ) goto l60;
            if( ( err/eps ) >
                    ( pow( (double)(nj[k+1]*nj[k])/4., 2. ) ) )  goto l100;
        }
        MIDEX( k, t, h, iy, id, ix );
        kc = k;
        if( err <= eps ) goto l60;
        // надежда на сходимость на к+1-м шаге
l55:
        if( err/eps > pow( (double)(nj[k])/2.0, 2.0 ) ) goto l100;
        kc = k + 1;
        MIDEX( kc, t, h, iy, id, ix );
        if( err > eps ) goto l100;
        // шаг допущен
l60:
        t += h;
        step = h;
        arg_x = t;
        for( i=0; i < aObj[ iy ].GetN(); i++ )
            x[i] = tt[0][i];
        Solut( iy, id, ix, x, dx, t );
        naccept++;
        PR( naccept, kc, t, h, x );
        //вычисление оптимального порядка
        if( kc == 1 )
        {
            kopt = 2;
            if( reject ) kopt = 1;
        }
        else if( kc <= k )
        {
            kopt = kc;
            if( w[kc-1] < w[kc]*FAC3 ) kopt = kc - 1;
            if( w[kc] < w[kc-1]*FAC3 )
                kopt = min( (kc+1) , (KM-1) );
        }
        else
        {
            kopt = kc-1;
            if( kc > 2 && w[kc-2] < w[kc-1]*FAC3 )
                kopt = kc - 2;
            if( w[kc] < w[kopt]*FAC3 )
                kopt = min( kc, (KM-1) );
        }
        // после недопушенного шага
        if( reject )
        {
            k = min( kopt, kc );
            h = min( h, hh[ k ] );
            reject = 0;           /*false*/
        }
        else
        {  // вычислим длину шага на следующем этапе
            if( kopt <= kc )
                h = hh[ kopt ];
            else if( kc < k && ( w[kc] < ( w[kc-1] * FAC4 ) ))
                h = hh[kc] * a1[kopt+1] / a1[kc];
            else h = hh[kc] * a1[kopt] / a1[kc];
            k = kopt;
        }
        h1=t_end-t;
    }     /*while*/
    *tv = t;
    aObj[ iy ].SetPtr( x );
    aObj[ id ].SetPtr( dx );
    aObj[ ix ].SetPtr( tv );
    step = h;
    return;
l100:
    k = min( k, kc );
    if( k > 1 && ( w[k-1] < w[k] * FAC3 ) )
        k--;
    nrejct++;
    h = hh[k];
    reject = 1;
    goto l30;
}


void
TInteg::RecordPlot( const char* /*key*/ )
{
/*Sveta 14/08/01    TIArray<TPlot> plt;

    plt.Add( new TPlot( o_i_allx, o_i_allx));
    pVisorImp->MakePlot( this, plt, name[0]);
    plt.Clear();

    plt.Add( new TPlot( o_i_allx, o_iallst));
    pVisorImp->MakePlot( this, plt, "Steps");
    plt.Clear();

    plt.Add( new TPlot( o_i_allx, o_iallpr));
    pVisorImp->MakePlot( this, plt, "Integration");
*/
}

void
TInteg::CmHelp()
{
    pVisor->OpenHelp( GEMS_INT_HTML ); 
}


// --------------------- end of integ.cpp ---------------------------


