//-------------------------------------------------------------------
// Id: gems/mods/m_probe2.cpp  version 2.0.0   2001
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
#include <math.h>
#include <stdio.h>

#include "v_object.h"
#include "m_probe.h"
#include "m_syseq.h"
#include "service.h"
#include "visor.h"

// insert changes in Profile to TSysEq
void TProbe::InsertChanges( TIArray<CompItem>& aIComp,
    TIArray<CompItem>& aPhase,  TIArray<CompItem>&aDComp )
{
    uint i;
    int j1;
    short  delta=0;
    bool ifRealloc = false;
    // insert changes to IComp
    if(aIComp.GetCount()<1)
        goto PHASE_TEST;
    for( i=0, j1=0; i<aIComp.GetCount(); i++)
    {
        if( prp->nnf && j1<prp->N )
        {
            while( prp->nnf[j1] < aIComp[i].line )
                prp->nnf[j1++] += delta;
            if( prp->nnf[j1] == aIComp[i].line )
            {
                if( aIComp[i].delta == -1 )
                {
                    prp->nnf[j1++] = -1;
                    ifRealloc = true;
                }
            }
        }
        delta+=aIComp[i].delta;
    }
    if( prp->nnf )
        for(; j1<prp->N; j1++)
            prp->nnf[j1] += delta;
    // insert changes to Phase
PHASE_TEST:
    delta = 0;
    if(aPhase.GetCount()<1)
        goto DCOMP_TEST;
    for( i=0, j1=0; i<aPhase.GetCount(); i++)
    {
        if( prp->kkf && j1<prp->FI)
        {
            while( prp->kkf[j1] < aPhase[i].line )
                prp->kkf[j1++] += delta;
            if( prp->kkf[j1] == aPhase[i].line )
            {
                if( aPhase[i].delta == -1 )
                {
                    prp->kkf[j1++] = -1;
                    ifRealloc = true;
                }
            }
        }
        delta+=aPhase[i].delta;
    }
    if( prp->kkf )
        for(; j1<prp->FI; j1++)
            prp->kkf[j1] += delta;
    // insert changes to Dcomp
DCOMP_TEST:
    delta = 0;
    if(aDComp.GetCount()<1)
        goto EXIT_TEST;
    for( i=0, j1=0; i<aDComp.GetCount(); i++)
    {
        if( prp->llf && j1<prp->L )
        {
            while( prp->llf[j1] < aDComp[i].line )
                prp->llf[j1++] += delta;
            if( prp->llf[j1] == aDComp[i].line )
            {
                if( aDComp[i].delta == -1 )
                {
                    prp->llf[j1++] = -1;
                    ifRealloc = true;
                }
            }
        }
        delta+=aDComp[i].delta;
    }
    if( prp->llf )
        for(; j1<prp->L; j1++)
            prp->llf[j1] += delta;
    // realloc memory if delete records
EXIT_TEST:
    if( ifRealloc )
    {
        newSizeifChange();
        dyn_new();
    }
}

// pack arrays if deleted records in base cfg
void TProbe::newSizeifChange()
{
    short i, j;

    // delete ICOMP records
    for( i=0, j=0; i<prp->N; i++ )
        if( prp->nnf[i] >= 0 ) // no delete num
        {
            prp->nnf[j] = prp->nnf[i];
            if( prp->Bs ) prp->Bs[j] = prp->Bs[i];
            if( prp->NgN ) prp->NgN[j] = prp->NgN[i];
            if( prp->IntN ) prp->Bs[j] = prp->IntN[i];
            j++;
        }
    prp->N = j;
    // delete PHASE records
    for( i=0, j=0; i<prp->FI; i++ )
        if( prp->kkf[i] >= 0 ) // no delete num
        {
            prp->kkf[j] = prp->kkf[i];
            if( prp->L1 ) prp->L1[j] = prp->L1[i];
            j++;
        }
    prp->FI = j;
    // delete DCOMP records
    for( i=0, j=0; i<prp->L; i++ )
        if( prp->llf[i] >= 0 ) // no delete num
        {
            prp->llf[j] = prp->llf[i];
            if( prp->NgLg ) prp->NgLg[j] = prp->NgLg[i];
            if( prp->NgLs ) prp->NgLs[j] = prp->NgLs[i];
            if( prp->NgLv ) prp->NgLv[j] = prp->NgLv[i];
            if( prp->NgLgam ) prp->NgLgam[j] = prp->NgLgam[i];
            if( prp->NgLgex ) prp->NgLgex[j] = prp->NgLgex[i];
            if( prp->IntLg ) prp->IntLg[j] = prp->IntLg[i];
            if( prp->IntLs ) prp->IntLs[j] = prp->IntLs[i];
            if( prp->IntLv ) prp->IntLv[j] = prp->IntLv[i];
            if( prp->IntLgam ) prp->IntLgam[j] = prp->IntLgam[i];
            if( prp->IntLgex ) prp->IntLgex[j] = prp->IntLgex[i];
            if( prp->Gs ) prp->Gs[j] = prp->Gs[i];
            if( prp->Ss ) prp->Ss[j] = prp->Ss[i];
            if( prp->Vs ) prp->Vs[j] = prp->Vs[i];
            if( prp->GAMs ) prp->GAMs[j] = prp->GAMs[i];
            if( prp->GEXs ) prp->GEXs[j] = prp->GEXs[i];
            j++;
        }
    prp->L = j;
}

//-------------------------Statistics----------------------------------
// make list of phases in each solutions
void  TProbe::paragen( int N, GDSTAT gst )
/* Pclc - count in each groop of solution
* Pnum - phase numbers in groop of solution */
{ short *Pnum, *Pclc;
    int i, j, k1, k,i1;

    try
    {
        Pnum = new short[gst.nI];
        Pclc = new short[N];
        ErrorIf( !Pnum || !Pclc, GetName(), "Memory alloc error!");
        memset( Pnum, 0, gst.nI*sizeof(short));
        memset( Pclc, 0, N*sizeof(short));

        for( i=0; i<gst.nI; i++ )
            Pnum[i] =-2;
        k  = i = j = i1 = 0;
        k1 = 1;
        while( i < gst.nI )
            switch( gst.iopt[ i ] )
            {
            case -1:
                switch( Pnum[j] )
                {
                case -1:
                    if( k1 )
                    {
                        Pclc[k]++;
                        i++;
                        j=0;
                        i1=i;
                        k=0;
                    }
                    else
                    {
                        i = i1;
                        j++;
                        k++;
                        k1=1;
                    }
                    break;
                case -2:
                    Pclc[k]++;
                    i=i1;
                    while( gst.iopt[i] != -1 )
                        Pnum[j++] = gst.iopt[i++];
                    Pnum[j] = gst.iopt[i++];
                    i1 = i;
                    k=0;
                    j=0;
                    k1=1;
                    break;
                default:
                    k1 = 0;
                    while( Pnum[j] > -1 ) j++;
                }
                break;
            default:
                switch( Pnum[j] )
                {
                case -1:
                    i=i1;
                    j++;
                    k++;
                    k1=1;
                    break;
                case -2:
                    while( gst.iopt[i] != -1 ) i++;
                    break;
                default:
                    if( Pnum[j] != gst.iopt[i] )
                        k1 = 0;
                    i++;
                    j++;
                    break;
                }
            }
        j=0;
        while( Pclc[j] ) j++;
        i=0;
        while( Pnum[i] != -2) i++;
        prp->nF = i+j;
        prp->nI = i+j+14;
        prp->iopt = (short *)aObj[ o_priopt].Alloc( prp->nI, 1, I_ );
        i=1;
        j=0;
        k=1;
        prp->iopt[0] = Pclc[0];
        while( Pnum[j] != -2)
        {
            prp->iopt[i++] = Pnum[j];
            if( Pnum[j++] == -1 )
                prp->iopt[i++] = Pclc[k++];
        }
        delete[] Pclc;
        delete[] Pnum;
    }
    catch( TError& xcpt )
    {
        delete[] Pclc;
        delete[] Pnum;
        Error(  xcpt.title.c_str() , xcpt.mess.c_str() );
    }
}

short  TProbe::pb_min( int N, double *x)
{
    short i, i_m=0;
    double x_m;
    x_m = x[0];
    for( i=0; i<N; i++)
        if( x[i] < x_m )
        {
            x_m = x[i];
            i_m = i;
        }
    return( i_m );
}

//make and analyze statistical matr
void  TProbe::pb_matr( int QT, GDSTAT gst )
{
    int i, j, k, L, N;
    double *max_j, *min_j, *e_j, *min_i, *r_j, r,X;
    TProfil* PRof = (TProfil*)(&aMod[RT_PARAM]);
    TGtDemo* GtD = (TGtDemo*)(&aMod[RT_GTDEMO]);

    try
    {
        ErrorIf( !prp->iopt || prp->nI<=0, "Statistic", "No data to analiz!" );
        L = PRof->pmp->L;
        N = PRof->pmp->N;
        max_j = new double[QT];
        memset(max_j, 0, QT*sizeof(double));
        min_j = new double[QT];
        memset(min_j, 0, QT*sizeof(double));
        e_j   = new double[QT];
        memset(e_j, 0, QT*sizeof(double));
        min_i = new double[QT];
        memset(min_i, 0, QT*sizeof(double));
        r_j = new double[QT];
        memset(r_j, 0, QT*sizeof(double));
        ErrorIf( !max_j || !min_j || !e_j || !min_i || !r_j,
                 "Statistic", "Memory alloc error!" );
        if( gst.G )  delete[] gst.G;
        gst.G = new double[L];
        if( gst.b )  delete[] gst.b;
        gst.b = new double[N];
        if( gst.opt )  delete[] gst.opt;
        gst.opt = new double[QT*QT];
        ErrorIf( !gst.G || ! gst.b || !gst.opt, "Statistic", "Memory alloc error!" );
        memset( gst.opt, 0, QT*QT*sizeof(double) );
        // main cicle
        for( j=0; j<QT; j++ )
        {  // read SysEq record and unpack data
            PRof->loadSystat( GtD->GetRkey(j) );
            memcpy( gst.G, PRof->pmp->G,L*sizeof(double));
            memcpy( gst.b, PRof->pmp->B,N*sizeof(double));
            for( i=0; i<QT; i++ )
            {
                PRof->loadSystat( GtD->GetRkey(i) );
                gst.opt[i*QT+j] = PRof->pb_GX( gst.G );
                for( k=0; k<N; k++)
                    gst.opt[i*QT+j] -= PRof->pmp->U[k] * gst.b[k];
                gst.opt[i*QT+j] = fabs(gst.opt[i*QT+j]);
            }
        } //j
        for( i=0; i<QT; i++ )
        {
            min_i[i] = gst.opt[0*QT+i];
            min_j[i] = gst.opt[i*QT+0];
        }
        for( i=0; i<QT; i++ )
            for( j=0; j<QT; j++ )
            {
                if( j != i )
                {
                    min_i[j] = min( min_i[j], gst.opt[i*QT+j]);
                    min_j[i] = min( min_i[i], gst.opt[i*QT+j]);
                }
                max_j[i] = max( max_j[i], gst.opt[i*QT+j]);
                e_j[i] += gst.opt[i*QT+j];
            }
        for( r=0.0, j=0; j<QT; j++ )
        {
            for( i=0; i<QT; i++ )
                r_j[j] = max( r_j[j], gst.opt[i*QT+j]-min_i[j]);
            e_j[j] /= QT;
            r += e_j[j];
            max_j[j] = (min_j[j]+max_j[j])/2;
        }
        r /= QT;

        /* check by criterion*/
        /* criterion Valda  */
        prp->iopt[ prp->nF] = pb_min( QT, max_j);
        /*criterion  Laplasa */
        prp->iopt[ prp->nF+1] = pb_min( QT, e_j);
        /* criterion Gurvitsa (a=0) */
        prp->iopt[ prp->nF+5] = pb_min( QT, min_j);
        /* criterion Gurvitsa  (a=0.5) */
        prp->iopt[ prp->nF+6] = pb_min( QT, e_j);
        /* criterion Sevidga  */
        prp->iopt[ prp->nF+4] = pb_min( QT, r_j);
        for(i=0; i<QT; i++ )
        {
            min_j[i] = e_j[i];
            max_j[i] = fabs(e_j[i]-r);
        }
        /*criterion Karpova (medium)*/
        prp->iopt[ prp->nF+2] = pb_min( QT, max_j);
        for(i=0;i<QT-1;i++)
        {
            k=i;
            while(1)
            {
                j=k+1;
                if(e_j[k]<=e_j[j]) break;
                X=e_j[k];
                e_j[k]=e_j[j];
                e_j[j]=X;
                k--;
                if(k<0) break;
            }
        }
        X = e_j[ QT/2+1 ];
        for( j=0; j<QT; j++)
            if( fabs( X-min_j[j] ) < 1e-9 ) break;
        /* criterion Karpova  (mediana)*/
        prp->iopt[ prp->nF+3] = j;

        if( max_j ) delete[]  max_j;
        if( min_j ) delete[]  min_j;
        if( e_j )   delete[]  e_j;
        if( min_i ) delete[]  min_i;
        if( r_j )   delete[]  r_j;
        if( gst.G )  delete[] gst.G;
        gst.G = 0;
        if( gst.b )  delete[] gst.b;
        gst.b = 0;
        if( gst.opt )  delete[] gst.opt;
        gst.opt = 0;

    }
    catch( TError& xcpt )
    {
        if( max_j ) delete[]  max_j;
        if( min_j ) delete[]  min_j;
        if( e_j )   delete[]  e_j;
        if( min_i ) delete[]  min_i;
        if( r_j )   delete[]  r_j;
        if( gst.G )  delete[] gst.G;
        gst.G = 0;
        if( gst.b )  delete[] gst.b;
        gst.b = 0;
        if( gst.opt )  delete[] gst.opt;
        gst.opt = 0;
        Error(  xcpt.title.c_str() , xcpt.mess.c_str() );
    }
}

//----------------------------Print to file results
/*
const char *pbz_tab_2   ="| %c | %3d |%13.6g|%13.7g  |";
const char *pbz_tab_4   ="|================";  //17
const char *pbz_tab_5   ="|   Component    ";  //17
const char *pbz_tab_5a  ="|                ";  //17
const char *pbz_tab_7   ="|%-16.16s";  //17
// pbz_tab_4 = pbz_tab_6 = pbz_tab_9
// pbz_tab_10 = pbz_tab_12 = pbz_tab_14
const char *pbz_tab_10  ="|====|============|============";  //31
const char *pbz_tab_11  ="|Grp.|   Value    | half-interl";  //31
const char *pbz_tab_11a ="|    |  %6.6s    |            ";  //31
const char *pbz_tab_13  ="| %3d|%12.6g|%12.6g";  //31
const char *pbz_tab_15 = "|";

//Print record of Probe
void
TProbe::RecordPrint( const char *key)
{
    if( pVisor->ProfileMode != true )
        Error( GetName(), "Do it in Profile mode!" );
    _pr_tab( key );
}

// Print resalts to table
void  TProbe::_pr_tab( const char *key )
{
    int i, j, M=0;
    unsigned long tblen;
    vstr pbuf(121);
    //  TProfil* PRof = (TProfil*)aMod[RT_PARAM];

    if( prp->L )  //sizes of arrays
    {
        if( prp->zond[0] != S_OFF ) M++;
        if( prp->zond[1] != S_OFF ) M++;
        if( prp->zond[5] != S_OFF ) M++;
        if( prp->zond[6] != S_OFF ) M++;
        if( prp->zond[7] != S_OFF ) M++;
    }
    SetString("Please, wait until arrays will be sampled");
    pVisor->Update();
    //Table heard
    TPrintTable prTab( prp->name, GetName(), key );

    // sound by T, P, V
    prTab.PutString( "|===|=====|=============|===============|\n"
                     "|   | Grp.|   Value     | half-interval |\n"
                     "|===|=====|=============|===============|" );
    sprintf( pbuf, pbz_tab_2, 'T', prp->NgT, prp->T, prp->IntT );
    prTab.PutString( pbuf );
    sprintf( pbuf, pbz_tab_2, 'P', prp->NgP, prp->P, prp->IntP );
    prTab.PutString( pbuf );
    sprintf( pbuf, pbz_tab_2, 'V', prp->NgV, prp->V, prp->IntV );
    prTab.PutString( pbuf );
    prTab.PutString( "|===|=====|=============|===============|\n" );

    // interval by  b
    if( prp->N && prp->zond[2] != S_OFF )
    {
        prTab.PutString( "\n   Sound IC by b" );
        prTab.PutString( pbz_tab_4, 17, false );
        prTab.PutString( pbz_tab_10, 31, false );
        prTab.PutString( pbz_tab_15 );

        prTab.PutString( pbz_tab_5, 17, false );
        prTab.PutString( pbz_tab_11, 31, false );
        prTab.PutString( pbz_tab_15 );

        prTab.PutString( pbz_tab_4, 17, false );
        prTab.PutString( pbz_tab_10, 31, false );
        prTab.PutString( pbz_tab_15 );

        for( i=0; i<prp->N; i++ )
        {
            sprintf( pbuf, pbz_tab_7, prp->SBp[i] );
            prTab.PutString( pbuf, strlen(pbuf), false );
            sprintf( pbuf, pbz_tab_13,prp->NgN[i],prp->Bs[i],prp->IntN[i]);
            prTab.PutString( pbuf, strlen(pbuf), false );
            prTab.PutString( pbz_tab_15 );
        }
        prTab.PutString( pbz_tab_4, 17, false );
        prTab.PutString( pbz_tab_10, 31, false );
        prTab.PutString( pbz_tab_15 );
    }
    // intervals by DC
    if( prp->L  )
    {
        prTab.PutString( "\n   Sound by DC" );
        prTab.PutString( pbz_tab_4, 17, false );
        for( j=0; j<M; j++ )
            prTab.PutString( pbz_tab_10, 31, false );
        prTab.PutString( pbz_tab_15 );

        prTab.PutString( pbz_tab_5a, 17, false );
        if( prp->zond[0] != S_OFF )
        {
            sprintf( pbuf, pbz_tab_11a, "G" );
            prTab.PutString( pbuf, 31, false );
        }
        if( prp->zond[1] != S_OFF )
        {
            sprintf( pbuf, pbz_tab_11a, "S" );
            prTab.PutString( pbuf, 31, false );
        }
        if( prp->zond[5] != S_OFF )
        {
            sprintf( pbuf, pbz_tab_11a, "V" );
            prTab.PutString( pbuf, 31, false );
        }
        if( prp->zond[6] != S_OFF )
        {
            sprintf( pbuf, pbz_tab_11a, "Gamma" );
            prTab.PutString( pbuf, 31, false );
        }
        if( prp->zond[7] != S_OFF )
        {
            sprintf( pbuf, pbz_tab_11a, "Gex" );
            prTab.PutString( pbuf, 31, false );
        }
        prTab.PutString( pbz_tab_15 );

        prTab.PutString( pbz_tab_5, 17, false );
        for( j=0; j<M; j++ )
            prTab.PutString( pbz_tab_11, 31, false );
        prTab.PutString( pbz_tab_15 );

        prTab.PutString( pbz_tab_4, 17, false );
        for( j=0; j<M; j++ )
            prTab.PutString( pbz_tab_10, 31, false );
        prTab.PutString( pbz_tab_15 );

        for( i=0; i<prp->L; i++ )
        {
            sprintf( pbuf, pbz_tab_7, prp->SMp[i] );
            prTab.PutString( pbuf, strlen(pbuf), false );
            if( prp->zond[0] != S_OFF )
            {
                sprintf( pbuf, pbz_tab_13, prp->NgLg[i],
                         prp->Gs[i],prp->IntLg[i]);
                prTab.PutString( pbuf, strlen(pbuf), false );
            }
            if( prp->zond[1] != S_OFF )
            {
                sprintf( pbuf, pbz_tab_13, prp->NgLs[i],
                         prp->Ss[i],prp->IntLs[i]);
                prTab.PutString( pbuf, strlen(pbuf), false );
            }
            if( prp->zond[5] != S_OFF )
            {
                sprintf( pbuf, pbz_tab_13, prp->NgLv[i],
                         prp->Vs[i],prp->IntLv[i]);
                prTab.PutString( pbuf, strlen(pbuf), false );
            }
            if( prp->zond[6] != S_OFF )
            {
                sprintf( pbuf, pbz_tab_13, prp->NgLgam[i],
                         prp->GAMs[i],prp->IntLgam[i]);
                prTab.PutString( pbuf, strlen(pbuf), false );
            }
            if( prp->zond[7] != S_OFF )
            {
                sprintf( pbuf, pbz_tab_13, prp->NgLgex[i],
                         prp->GEXs[i],prp->IntLgex[i]);
                prTab.PutString( pbuf, strlen(pbuf), false );
            }
            prTab.PutString( pbz_tab_15 );
        }
        prTab.PutString( pbz_tab_4, 18, false );
        for( j=0; j<M; j++ )
            prTab.PutString( pbz_tab_10, 31, false );
        prTab.PutString( pbz_tab_15 );
    }
    // list og groups
    prTab.PutString( "\n\n           List of Profile groups   \n"
                     "|=======|====================|=======|\n"
                     "|  N    | Name of group      | Type  |\n"
                     "|=======|====================|=======|" );
    for( i=0; i<prp->kG; i++ )
    {
        sprintf( pbuf, "³  %-4d |  %-18.16s|   %c   |",
                 i+1, prp->SGp[i], prp->ZPg[i] );
        prTab.PutString( pbuf );
    }
    prTab.PutString( "|=======|====================|=======|\n" );
    // SYSEQ keys
    prTab.PutString( "\n List of tasks:\n----------------------------\n" );
    for( i=0; i<prp->QT; i++ )
        prTab.PutString( prp->stl[i], EQ_RKLEN );
    prTab.PutEnd( );
    // Put resalts to object
    const char *tab= prTab.GetTable();
    tblen = strlen(tab)+1; // Get size of text buf 
    prp->tprn = (char *)aObj[ o_prtprn ].Alloc( 1, tblen, S_ );
    memcpy( prp->tprn, tab, tblen );
    vfObjToFile(window(), &aObj[o_prtprn]);
} */

//-------------------------- End of m_probe2.cpp ----------------------------

