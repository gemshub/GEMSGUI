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
const char *GEMS_FMT_HTML = "gm_gem2mt";

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
    aFldKeysHelp.Add("l<24  integrator Key");
    set_def();
    start_title = " Coupled models: Under construction";
}

// link values to objects
void TInteg::ods_link(int)
{
    aObj[o_ipstr].SetPtr( keywd );
    aObj[o_igexpr].SetPtr( &Nequ );
    aObj[o_ipdoub].SetPtr( &x_bg );
    aObj[o_igkey].SetPtr( keywd );
    aObj[o_igname].SetPtr( name );
    aObj[o_igbgen].SetPtr( &x_bg );
    aObj[o_igeps].SetPtr( &Eps );
    aObj[o_igstep].SetPtr( &step );
    aObj[o_ival_x].SetPtr( &arg_x );
    aObj[o_imaxit].SetPtr( &MaxIter );
    aObj[o_ignstp].SetPtr( &nfcn );
    /*y_bg = 0;
    param = 0;
    TxtEqu = 0;
    val_y = 0;
    valdy = 0;
    allx = 0;
    ally = 0;
    allpr = 0;
    allst = 0;*/
}

// set dynamic Objects ptr to values
void TInteg::dyn_set(int)
{
    y_bg = (double *)aObj[o_ipy_bg].GetPtr();
    param = (double *)aObj[o_iparam].GetPtr();
    TxtEqu = (char *)aObj[o_ig_txt].GetPtr();
    val_y = (double *)aObj[o_ival_y].GetPtr();
    valdy = (double *)aObj[o_ivaldy].GetPtr();
    allx = (double *)aObj[o_i_allx].GetPtr();
    ally = (double *)aObj[o_i_ally].GetPtr();
    allpr = (short *)aObj[o_iallpr].GetPtr();
    allst = (double *)aObj[o_iallst].GetPtr();
}

// free dynamic memory in objects and values
void TInteg::dyn_kill(int)
{
    y_bg = (double *)aObj[o_ipy_bg].Free();
    param = (double *)aObj[o_iparam].Free();
    TxtEqu = (char *)aObj[o_ig_txt].Free();
    val_y = (double *)aObj[o_ival_y].Free();
    valdy = (double *)aObj[o_ivaldy].Free();
    allx = (double *)aObj[o_i_allx].Free();
    ally = (double *)aObj[o_i_ally].Free();
    allpr = (short *)aObj[o_iallpr].Free();
    allst = (double *)aObj[o_iallst].Free();
}

// realloc dynamic memory
void TInteg::dyn_new(int)
{
    if( TxtEqu == 0 )
        TxtEqu = (char *)aObj[o_ig_txt].Alloc( 1, 256, S_ );
    y_bg = (double *)aObj[o_ipy_bg].Alloc( Nequ, 1, D_ );
    param = (double *)aObj[o_iparam].Alloc( Nequ, 1, D_ );
    val_y = (double *)aObj[o_ival_y].Alloc( Nequ, 1, D_ );
    valdy = (double *)aObj[o_ivaldy].Alloc( Nequ, 1, D_ );
    allx = (double *)aObj[o_i_allx].Alloc( INT_BLOK, 1, D_ );
    ally = (double *)aObj[o_i_ally].Alloc( INT_BLOK, Nequ, D_ );
    allpr = (short *)aObj[o_iallpr].Alloc( INT_BLOK, 1, I_ );
    allst = (double *)aObj[o_iallst].Alloc( INT_BLOK, 1, D_ );
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


