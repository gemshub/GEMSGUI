//-------------------------------------------------------------------
// Id: gems/mods/m_gtdemo.cpp  version 2.0.0   2001
//
// Implementation of TGtDemo class, config and calculation functions
//
// Rewritten from C to C++ by S.Dmytriyeva  970207 modified 010904
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
const char *GEMS_GTD_HTML = "gm_gtdemo";

#include <math.h>
#include <stdio.h>

#include "m_proces.h"
#include "m_probe.h"
//#include "m_sdata.h"
#include "v_object.h"
#include "service.h"
#include "visor.h"

TGtDemo* TGtDemo::pm;

bool TGtDemo::check_RT( int nrt )
{
    return ( nrt >RT_ICOMP && nrt < (int)aMod.GetCount());
}
///

TGtDemo::TGtDemo( int nrt ):
        TCModule( nrt )
{
    nQ = 1;
    aFldKeysHelp.Add("l<10 Identifier of the parent Modelling Project");
    aFldKeysHelp.Add("l<8  Top chain type for data sampling { PR SY PH RE...}");
    aFldKeysHelp.Add("l<16 Identifier of this GtDemo sampler definition");
    aFldKeysHelp.Add("l<4  Variant number of this GtDemo task <integer>");
    aFldKeysHelp.Add("l<26 Record key comment to this GtDemo task");
    gdp=&gd[0];
    set_def();
    start_title = " Definition of Data sampler, tabulator and plotter ";
    gd_gr = 0;
}

// link values to objects
void TGtDemo::ods_link( int q)
{
    aObj[ o_gdps].SetPtr(   &gd[q].PtAEF );   /*a 16*/
    aObj[ o_gdnlrk].SetPtr( &gd[q].Nlrk );   /*i 4*/
    aObj[ o_gdnwcq].SetPtr( &gd[q].Nwc );    /*i 2*/
    aObj[ o_gddimxy].SetPtr( gd[q].dimXY ); /*i 2*/
    aObj[ o_gddimef].SetPtr( gd[q].dimEF );  /*i 2*/
    aObj[ o_gdname].SetPtr(  gd[q].name );
    aObj[ o_gdcom].SetPtr(   gd[q].comment );
    aObj[ o_gdxnames].SetPtr(  gd[q].xNames );
    aObj[ o_gdynames].SetPtr(  gd[q].yNames );
    aObj[ o_gdaxis].SetPtr(   gd[q].axisType );   /* i 6 */
    aObj[ o_gwwkb].SetPtr(   gd[q].wcrk );
    aObj[ o_gdproc].SetPtr(   gd[q].prKey );
    aObj[ o_gdsize].SetPtr(    gd[q].size[0] ); /*f 8*/

    aObj[ o_gdlnam].SetPtr( gd[q].lNam0[0] );
    aObj[ o_gdlnam].SetDim( 1, gd[q].dimXY[1] );
    aObj[ o_gdlname].SetPtr( gd[q].lNamE[0] );
    aObj[ o_gdlname].SetDim( 1, gd[q].dimEF[1] );

    aObj[o_gdexpr].SetPtr( gd[q].expr );//aObj[o_gdexpr].SetDim(1,len(gd[q].expr));
    aObj[o_gdexpre].SetPtr(gd[q].exprE);//aObj[o_gdexpre].SetDim(1,l(gd[q].exprE));
    aObj[ o_gdrkey].SetPtr( gd[q].rkey );
    aObj[ o_gdrkey].SetDim(  gd[q].Nlrk, 1 );
    aObj[o_gdx0].SetPtr( gd[q].x0 );
    aObj[ o_gdx0].SetDim(gd[q].dimXY[0], 1 );
    aObj[o_gdy0].SetPtr( gd[q].y0 );
    aObj[ o_gdy0].SetDim(gd[q].dimXY[0], gd[q].dimXY[1] );
    aObj[ o_gdxe].SetPtr( gd[q].xE );
    aObj[o_gdxe].SetDim( gd[q].dimEF[0],1 );
    aObj[ o_gdye].SetPtr( gd[q].yE );
    aObj[ o_gdye].SetDim(gd[q].dimEF[0], gd[q].dimEF[1] );
    aObj[ o_gdwc].SetPtr( gd[q].wc );
    aObj[ o_gdwc].SetDim( gd[q].Nwc, 1 );
    aObj[ o_gdqp].SetPtr( gd[q].qp );
    aObj[ o_gdqp].SetDim( gd[q].Nlrk,  gd[q].Nqp);
    aObj[ o_gdplline].SetPtr( plot );
    aObj[ o_gdplline].SetDim( gd[q].dimXY[1]+gd[q].dimEF[1],  sizeof(TPlotLine));

    aObj[ o_gwetext].SetPtr( gd[q].etext ); // aObj[ o_gwetext].SetDim(1, 0 );
    aObj[ o_gwprtab].SetPtr( gd[q].prtab ); // aObj[ o_gwprtab].SetDim(1, 0);
    aObj[ o_gwtprn].SetPtr(  gd[q].tprn  ); // aObj[ o_gwtprn].SetDim( 1,l( gd[q].tprn));

    aObj[ o_gwnext].SetPtr(  &gd[q].Next );
    aObj[ o_gwjr].SetPtr(    &gd[q].jR );
    aObj[ o_gwi0].SetPtr(    &gd[q].i0 );
    aObj[ o_gwie].SetPtr(    &gd[q].iE );
    aObj[ o_gwj0].SetPtr(    &gd[q].j0 );
    aObj[ o_gwje].SetPtr(    &gd[q].jE );
    aObj[ o_gwwkey].SetPtr(  gd[q].Wkb );
    aObj[ o_gdsdref].SetPtr( gd[q].sdref);
    aObj[ o_gdsdref].SetDim( gd[q].Nsd, 1 );
    aObj[ o_gdsdval].SetPtr( gd[q].sdval);
    aObj[ o_gdsdval].SetDim( gd[q].Nsd, 1 );

    gdp=&gd[q];
}

// set dynamic Objects ptr to values
void TGtDemo::dyn_set(int q)
{
    ErrorIf( gdp!=&gd[q], GetName(), "Illegal access to gd in dyn_set.");
    // memcpy( gdp->symb , rt[nRT].UnpackKey(), GD_RKLEN );
    gdp->lNam0 = (char (*)[MAXGRNAME])aObj[ o_gdlnam ].GetPtr();
    gdp->lNamE = (char (*)[MAXGRNAME])aObj[ o_gdlname ].GetPtr();
    gdp->expr = (char *)aObj[ o_gdexpr ].GetPtr();
    gdp->exprE = (char *)aObj[ o_gdexpre ].GetPtr();
    gdp->rkey  = (char *)aObj[ o_gdrkey ].GetPtr();
    gdp->x0    = (double *)aObj[ o_gdx0 ].GetPtr();
    gdp->y0    = (double *)aObj[ o_gdy0 ].GetPtr();
    gdp->xE    = (double *)aObj[ o_gdxe ].GetPtr();
    gdp->yE    = (double *)aObj[ o_gdye ].GetPtr();
    gdp->wc    = (double *)aObj[ o_gdwc ].GetPtr();
    gdp->qp    = (double *)aObj[ o_gdqp ].GetPtr();
    gdp->etext = (char *)aObj[ o_gwetext ].GetPtr();
    gdp->prtab = (char *)aObj[ o_gwprtab ].GetPtr();
    gdp->tprn  = (char *)aObj[ o_gwtprn ].GetPtr();
    plot  = (TPlotLine *)aObj[ o_gdplline ].GetPtr();
    gdp->sdref = (char (*)[V_SD_RKLEN])aObj[ o_gdsdref ].GetPtr();
    gdp->sdval = (char (*)[V_SD_VALEN])aObj[ o_gdsdval ].GetPtr();
    gdp->rtLen =  (short)rt[ gdp->nRT ].KeyLen();
}

// free dynamic memory in objects and values
void TGtDemo::dyn_kill(int q)
{
    ErrorIf( gdp!=&gd[q], GetName(), "Illegal access to gd in dyn_kill.");
    gdp->lNam0 = (char (*)[MAXGRNAME])aObj[ o_gdlnam ].Free();
    gdp->lNamE = (char (*)[MAXGRNAME])aObj[ o_gdlname ].Free();
    gdp->expr = (char *)aObj[ o_gdexpr ].Free();
    gdp->exprE = (char *)aObj[ o_gdexpre ].Free();
    gdp->rkey  = (char *)aObj[ o_gdrkey ].Free();
    gdp->x0    = (double *)aObj[ o_gdx0 ].Free();
    gdp->y0    = (double *)aObj[ o_gdy0 ].Free();
    gdp->xE    = (double *)aObj[ o_gdxe ].Free();
    gdp->yE    = (double *)aObj[ o_gdye ].Free();
    gdp->wc    = (double *)aObj[ o_gdwc ].Free();
    gdp->qp    = (double *)aObj[ o_gdqp ].Free();
    gdp->etext = (char *)aObj[ o_gwetext ].Free();
    gdp->prtab = (char *)aObj[ o_gwprtab ].Free();
    gdp->tprn  = (char *)aObj[ o_gwtprn ].Free();
    plot = (TPlotLine*)aObj[ o_gdplline ].Free();
    gdp->sdref = (char (*)[V_SD_RKLEN])aObj[ o_gdsdref ].Free();
    gdp->sdval = (char (*)[V_SD_VALEN])aObj[ o_gdsdval ].Free();

}

// realloc dynamic memory
void TGtDemo::dyn_new(int q)
{
    ErrorIf( gdp!=&gd[q], GetName(), "Illegal access to gd in dyn_new.");

    gdp->lNam0 = (char (*)[MAXGRNAME])aObj[ o_gdlnam ].Alloc( 1,
                 gdp->dimXY[1], MAXGRNAME);
    gdp->expr = (char *)aObj[ o_gdexpr ].Alloc(1, 2048, S_);

    gdp->x0    = (double *)aObj[ o_gdx0 ].Alloc(gdp->dimXY[0], 1, D_);
    gdp->y0    = (double *)aObj[ o_gdy0 ].Alloc(gdp->dimXY[0], gdp->dimXY[1], D_);

    if( gdp->PtAEF == S_OFF )
    {
        gdp->lNamE = (char (*)[MAXGRNAME])aObj[ o_gdlname ].Free();
        gdp->exprE = (char *)aObj[ o_gdexpre ].Free();
        gdp->xE    = (double *)aObj[ o_gdxe ].Free();
        gdp->yE    = (double *)aObj[ o_gdye ].Free();
        gdp->dimEF[1] = gdp->dimEF[0] = 0;
    }
    else
    {
        gdp->lNamE = (char (*)[MAXGRNAME])aObj[ o_gdlname ].Alloc(1,
                     gdp->dimEF[1], MAXGRNAME);
        gdp->exprE = (char *)aObj[ o_gdexpre ].Alloc(1, 2048, S_);
        gdp->xE    = (double *)aObj[ o_gdxe ].Alloc(gdp->dimEF[0], 1, D_);
        gdp->yE    = (double *)aObj[ o_gdye ].Alloc(gdp->dimEF[0],gdp->dimEF[1], D_);
    }
    if( gdp->Nwc > 0 )
        gdp->wc    = (double *)aObj[ o_gdwc ].Alloc(1, gdp->Nwc, D_);
    else  gdp->wc    = (double *)aObj[ o_gdwc ].Free();

    if( gdp->Nqp>0 && gdp->Nlrk>0 )
        gdp->qp    = (double *)aObj[ o_gdqp ].Alloc( gdp->Nlrk, gdp->Nqp, D_ );
    else    gdp->qp    = (double *)aObj[ o_gdqp ].Free();

    if( gdp->Nsd > 0 )
    {
        gdp->sdref =
            (char (*)[V_SD_RKLEN])aObj[ o_gdsdref].Alloc( gdp->Nsd, 1, V_SD_RKLEN );
        gdp->sdval =
            (char (*)[V_SD_VALEN])aObj[ o_gdsdval].Alloc( gdp->Nsd, 1, V_SD_VALEN );
    }
    else
    {
        gdp->sdref = (char (*)[V_SD_RKLEN])aObj[ o_gdsdref ].Free();
        gdp->sdval = (char (*)[V_SD_VALEN])aObj[ o_gdsdval ].Free();
    }

    gdp->etext = (char *)aObj[ o_gwetext ].Alloc(1, 2048, S_);
    gdp->prtab = (char *)aObj[ o_gwprtab ].Alloc(1, 2048, S_);

}

// set default mode for rt type
void TGtDemo::gd_ps_set()
{

    if( gdp->PsPE != S_OFF && gdp->nRT == RT_SYSEQ)
        gdp->nRT = RT_PROCES;
    if( gdp->PsPB != S_OFF && gdp->nRT == RT_SYSEQ)
        gdp->nRT = RT_PROBE;

    switch( gdp->nRT )
    {
    case RT_ICOMP:
        strncpy( &gdp->PsIC, "+--------------", 15);
        break;
    case RT_DCOMP:
        strncpy( &gdp->PsIC, "-+-------------", 15);
        break;
    case RT_COMPOS:
        strncpy( &gdp->PsIC, "--+------------", 15);
        break;
    case RT_REACDC:
        strncpy( &gdp->PsIC, "---+-----------", 15);
        break;
    case RT_RTPARM:
        strncpy( &gdp->PsIC, "---++----------", 15);
        break;
    case RT_PHASE:
        strncpy( &gdp->PsIC, "-----+---------", 15);
        break;
    case RT_SYSEQ:
        strncpy( &gdp->PsIC, "------+--------", 15);
        break;
    case RT_PROCES:
        strncpy( &gdp->PsIC,"------++-------", 15);
        gdp->nRT = RT_SYSEQ;
        break;
    case RT_PROBE:
        strncpy( &gdp->PsIC, "------+-+------", 15);
        gdp->nRT = RT_SYSEQ;
        break;
        //case RT_DUTERM:strncpy( &gdp->PsIC,"------+--+-----", 15); break;
    default:
        Error( GetName(), "Illegal record type.");
    }
}

// get record type
short TGtDemo::gd_rectype( )
{
    TCStringArray buf;
    int nRType;

    for( uint i=RT_ICOMP; i< RT_DUTERM; i++ )
        if( i != RT_GTDEMO )
            buf.Add( aMod[i].GetName());
    nRType = gdp->nRT;
    if( nRType > RT_GTDEMO )
        nRType--;
    nRType -= RT_ICOMP;
    do
    {
        nRType = vfChoice(window(), buf,
                          "gd_rttype Please, choose chain type for demo data extraction",
                          nRType);
    }
    while( nRType<0 );
    nRType += RT_ICOMP;
    if( nRType >= RT_GTDEMO ) nRType++;
    return (short)nRType;
}

//set default information
void TGtDemo::set_def( int q)
{
    short nR = gdp->nRT;

    ErrorIf( gdp!=&gd[q], GetName(), "Illegal access to gd in set_def.");
    memset( gd, 0, sizeof( GTDEMO ));
    if( check_RT( nR) == true )
        gdp->nRT = nR;
    else gdp->nRT = RT_ICOMP;
    strcpy( gdp->wcrk, "*");
    strcpy( gdp->prKey, "*");
    gdp->PtAEF = '-';
    gdp->Nsd = 0;

    gd_ps_set();
    gdp->lNam0 = 0;
    gdp->lNamE = 0;
    gdp->expr  = 0;
    gdp->exprE = 0;
    gdp->rkey  = 0;
    gdp->x0    = 0;
    gdp->y0    = 0;
    gdp->xE    = 0;
    gdp->yE    = 0;
    gdp->wc    = 0;
    gdp->qp    = 0;
    gdp->etext = 0;
    gdp->prtab = 0;
    gdp->tprn  = 0;
    plot       = 0;
    gdp->sdref = 0;
    gdp->sdval = 0;

    // Statistic
    gst.iopt = 0;
    gst.opt  = 0;
    gst.G    = 0;
    gst.b    = 0;
}


// Input nessasery data and links objects
void TGtDemo::RecInput( const char *key )
{
    TCModule::RecInput( key );
}

// Get list of records to make GTDemo
void TGtDemo::bld_rec_list( )
{
    //TProfil *aProf=(TProfil *)aMod[RT_PARAM];
    TCStringArray aRklist;
    TCIntArray anRk;
    TCIntArray aMrk;
    gstring str;
    char *key_p;
    int i, Nr;
    short rtlen = (short)rt[gdp->nRT].KeyLen();

AGAIN:
    key_p=0;
    if( !strchr( gdp->prKey, '*') )
        key_p = gdp->prKey;
    if( gdp->PsPE != S_OFF )   // GTdemo by Process
    {
        TProcess::pm->RecordLoadinProfile(key_p);
        strncpy( gdp->prKey, rt[RT_PROCES].PackKey(), MAXRKEYLEN );
        for( i=0; i<aObj[o_pestl].GetN(); i++ )
            aRklist.Add( aObj[o_pestl].GetString( i, 0 ));
    }
    else  if( gdp->PsPB != S_OFF ) // GTdemo by Probe
    {
        TProbe::pm->RecordLoadinProfile(key_p);
        strncpy( gdp->prKey, rt[RT_PROBE].PackKey(), MAXRKEYLEN );
        for( i=0; i<aObj[o_prstl].GetN(); i++ )
            aRklist.Add( aObj[o_prstl].GetString( i, 0 ));
    }
    else // other type of records
    {
AGAINRC:    //get  keypart
        str = vfKeyTemplEdit(window(), "Set template", gdp->nRT, gdp->wcrk );
        //      if(  str== "" )   Bugfix 19.12.00  DAK
        //          goto AGAINRC;
        Nr = rt[gdp->nRT].GetKeyList( str.c_str(), aRklist, anRk );
        if( Nr<1 )
            if( vfQuestion(window(), GetName(),
                           "No record keys matching a template! Repeat?"))
                goto AGAINRC;
            else Error( GetName(), "No record keys matching a template!" );
    }
    if( gdp->rkey )  // old selections
    {
        aMrk.Clear();
        for(uint j=0; j<aRklist.GetCount() ; j++ )
            for( i=0; i<gdp->Nlrk; i++ )
            {
                if( strncmp( gdp->rkey+i*rtlen, aRklist[j].c_str(), rtlen ))
                    continue;
                aMrk.Add(j);
                break;
            }
    }
    // Select records list
    aMrk = vfMultiChoiceSet(window(), aRklist,
                            "gd_keys   Marking record keys for data extraction", aMrk );
    if( aMrk.GetCount() < 1 )
        if( vfQuestion(window(), GetName(), "No record keys selected! Repeat marking?" ))
            goto AGAIN;
        else Error( GetName(), "No record keys selected!");
    gdp->Nlrk = (short)aMrk.GetCount();
    gdp->rkey = (char *)aObj[ o_gdrkey ].Alloc( gdp->Nlrk, 1, rtlen );
    // make list of record
    for( i=0; i<gdp->Nlrk; i++ )
        memcpy( gdp->rkey+i*rtlen, aRklist[aMrk[i]].c_str(), rtlen );
}

//Rebild record structure before calc
int
TGtDemo::RecBuild( const char *key, int mode  )
{
    TProfil *aPa=TProfil::pm;
    vstr tbuf(100);
    int i;

    if( gst.iopt )
        delete[] gst.iopt;
    gst.iopt = 0;
AGAIN:
    if( gdp->PsPE != S_OFF && gdp->nRT == RT_SYSEQ)
        gdp->nRT = RT_PROCES;
    if( gdp->PsPB != S_OFF && gdp->nRT == RT_SYSEQ)
        gdp->nRT = RT_PROBE;


    gdp->nRT = gd_rectype();
    gd_ps_set();
    if( pVisor->ProfileMode != true  && gdp->nRT == RT_SYSEQ )
        Error( GetName(), "Do it in Project mode!" );
    int ret = TCModule::RecBuild( key, mode );

//    gd_ps_set();
    if( pVisor->ProfileMode != true  && gdp->nRT == RT_SYSEQ )
        Error( GetName(), "Do it in Project mode!" );
    if( ret == VF_CANCEL )
        return ret;
    if(  gdp->Nwc<0 || gdp->Nqp<0 || gdp->dimEF[0]<0 || gdp->dimEF[1]<0  ||
            ( (gdp->dimEF[0]==0 || gdp->dimEF[1]==0) && gdp->PtAEF != S_OFF ) )
    {
        vfMessage(window(), GetName(), "Illegal sizes or flags!");
        goto AGAIN;
    }
    gdp->rtLen =  (short)rt[ gdp->nRT ].KeyLen();
    bld_rec_list();
    gdp->dimXY[0] = gdp->Nlrk;
    if(  gdp->dimXY[1] <= 0 )  gdp->dimXY[1] = 2;

    if( gdp->dimXY[0] < gdp->Nlrk  ||
            gdp->dimXY[1] <= 0 || gdp->dimXY[1] > 20 )
        goto AGAIN;
    dyn_new();
    if( gdp->PtAEF != S_OFF && gdp->exprE && !*gdp->exprE )
    {
        strcpy( gdp->exprE, "next=:next;" );
        for( i=0; i<gdp->dimEF[1]; i++ )
        {
            sprintf( tbuf, "%s%d", aPa->pa.GDpsc, i+1 );
            if( !*gdp->lNamE[i] || *gdp->lNamE[i] == ' ' )
                strncpy( gdp->lNamE[i], tbuf,MAXGRNAME );
        }
    }
    for(int j=0; j< gdp->dimXY[1]; j++ )
    {
        sprintf( tbuf, "%s%d", aPa->pa.GDpsc, j+1 );
        if( !*gdp->lNam0[j]|| *gdp->lNam0[j] == ' ' )
            strncpy( gdp->lNam0[j], tbuf, MAXGRNAME );
    }
    pVisor->Update();
    return ret;
}


// Translate, analyze and unpack equations of process
void TGtDemo::gd_text_analyze()
{
    try
    {
        TProfil* PRof = TProfil::pm;
        int mupL=0, pmpL =0;

        if( pVisor->ProfileMode == true )
        {
            mupL = PRof->mup->L;
            pmpL = PRof->pmp->L;
        }
        PRof->ET_translate( o_gwetext, o_gdexpr, 0, mupL, 0, pmpL );
        rpn[0].GetEquat( (char *)aObj[o_gwetext].GetPtr() );
    }
    catch( TError& xcpt )
    {
        char *erscan = (char *)aObj[o_gdexpr].GetPtr();
        vfMessage(window(), xcpt.title, xcpt.mess);
        /*bool   iRet = */
        CheckEqText(  erscan,
                      "Error in translation of equations of demonstration " );
        /*  if( iRet )
               goto AGAIN;  */
        Error(  GetName() , xcpt.mess.c_str() );
    }
}

// Translate and calculate equations of empirical data
void TGtDemo::gd_EF_calc()
{
    try
    {
        rpn[1].GetEquat( (char *)aObj[o_gdexpre].GetPtr() );
    }
    catch( TError& xcpt )
    {
        char *erscan = (char *)aObj[o_gdexpre].GetPtr();
        vfMessage(window(), xcpt.title, xcpt.mess);
        /*bool   iRet = */
        CheckEqText(  erscan,
                      "Error in translation of equations of empirical data" );
        Error(  GetName() , xcpt.mess.c_str() );
    }
    // calc equations
    for( gdp->iE = 0; gdp->iE< gdp->dimEF[0]; gdp->iE++ )
        rpn[1].CalcEquat();
}

// read record number nI from curent nRT
void
TGtDemo::gd_rec_read( int nI )
{

    ErrorIf( nI < 0 || nI > gdp->Nlrk, GetName(), "Illegal record number!" );

    memset( gdp->Wkb, 0, MAXRKEYLEN );
    strncpy( gdp->Wkb, gdp->rkey+nI*gdp->rtLen, gdp->rtLen );

    if( gdp->nRT == RT_SYSEQ && pVisor->ProfileMode == true )
    {
        TProfil* PRof = TProfil::pm;
        PRof->loadSystat( gdp->Wkb );

        if( gdp->PsPB != S_OFF && *gdp->prKey) // Probe mode
        {
            if( !nI )
            {
                if( gst.iopt )
                    delete[] gst.iopt;
                gst.iopt = new short[gdp->Nlrk*(PRof->mup->Fi+1)];
                memset( gst.iopt, 0, gdp->Nlrk*(PRof->mup->Fi+1)*sizeof(short) );
                gst.nI = 0;
            }
            if(  gst.iopt ) // get list of resalt phases
            {
                for(short k=0; k<PRof->pmp->FI; k++)
                    if( PRof->pmp->XF[k] > PRof->pmp->DSM )
                        gst.iopt[ gst.nI++ ] = PRof->pmp->muk[k];
                gst.iopt[ gst.nI++ ] = -1;
            }
        }
    }
    else
    {
        ((TCModule *)&aMod[gdp->nRT])->RecInput( gdp->Wkb );
        ((TCModule *)&aMod[gdp->nRT])->check_input( gdp->Wkb );
    }
}

//Recalc record structure
void
TGtDemo::RecCalc( const char *key )
{
    if( pVisor->ProfileMode != true  && gdp->nRT == RT_SYSEQ )
        Error( GetName(), "Do it in Project mode!" );
    TCModule::RecCalc(key);

    strcpy( gdp->SYS_key, "*" );
    if( gdp->PsPE != S_OFF && *gdp->prKey)   /* read process record */
    {    TProcess::pm->RecInput( gdp->prKey );
         if( TProcess::pm->NoSave())
           Error( key, "x0, y0 arrays in this GtDemo definition \n"
           "can only be updated by Process recalculation.");
    }
    if( gdp->PsPB != S_OFF && *gdp->prKey)    /* read probe record */
        TProbe::pm->RecInput( gdp->prKey );

    for( gdp->jR = 0; gdp->jR< gdp->Nlrk; gdp->jR++ )
    {
        pVisor->Message( window(), GetName(),
                 "Sampling the x0, y0 data \n"
                 "Please, wait...", gdp->jR, gdp->Nlrk);
        gd_rec_read( gdp->jR );
        if( strcmp( gdp->SYS_key, rt[RT_SYSEQ].UnpackKey() ))
        {  /* retranlate text of equations */
            gd_text_analyze();
            strncpy( gdp->SYS_key, rt[RT_SYSEQ].UnpackKey(), EQ_RKLEN );
        }
        rpn[0].CalcEquat();
        aMod[RT_GTDEMO].ModUpdate("GD_calc   Calculation of GTDEMO arrays");
        // Stop Process from Andy Sveta
    }
    /* calc empirical data */
    if( gdp->PtAEF != S_OFF && gdp->exprE && *gdp->expr )
        gd_EF_calc();
    // calc statistic
    if( gdp->PsPB != S_OFF && gst.iopt && gst.nI )
    {
        TProbe* PRob = TProbe::pm;
        PRob->paragen( gdp->Nlrk, gst );
        PRob->pb_matr( gdp->Nlrk, gst );
        PRob->RecSave( gdp->prKey );
    }
    pVisor->CloseMessage();
    // set graph and demo resalts gtdemo_demo( key );
}



void
TGtDemo::RecordPlot( const char* /*key*/ )
{

     if(  gdp->PsRes4 == S_ON)
      {
           gdp->PsRes4 = S_OFF;
           fEdit = true;
           return;
      }

    TIArray<TPlot> plt;

    plt.Add( new TPlot(o_gdx0, o_gdy0 ));
    int  nLn = plt[ 0 ].getLinesNumber();
    if( gdp->PtAEF != S_OFF )
    {
        plt.Add( new TPlot(o_gdxe, o_gdye ));
        nLn += plt[1].getLinesNumber();
    }
    if( plot )
    {
        int oldN = aObj[o_gdplline].GetN();
        TPlotLine defpl("", 4);

        plot = (TPlotLine * )aObj[ o_gdplline ].Alloc( nLn, sizeof(TPlotLine) );
        for(int ii=0; ii<nLn; ii++ )
        {
            if( ii >= oldN )
                plot[ii] = defpl;
            if(ii < gdp->dimXY[1] )
                strncpy( plot[ii].name, gdp->lNam0[ii], MAXGRNAME );
            else
                strncpy( plot[ii].name, gdp->lNamE[ii-gdp->dimXY[1]], MAXGRNAME );
            plot[ii].name[MAXGRNAME] = '\0';
        }
        gd_gr = new GraphWindow( this, plt, gdp->name,
                                     gdp->size[0], gdp->size[1], plot,
                                     gdp->axisType, gdp->xNames, gdp->yNames);
    }
    else
    {
      TCStringArray lnames;
      int ii;
      for( ii=0; ii<gdp->dimXY[1]; ii++ )
          lnames.Add( gstring(gdp->lNam0[ii], 0, MAXGRNAME ));
      for( ii=0; ii<gdp->dimEF[1]; ii++ )
          lnames.Add( gstring( gdp->lNamE[ii], 0, MAXGRNAME ));
      gd_gr = new GraphWindow( this, plt, gdp->name,
          gdp->xNames, gdp->yNames, lnames );
    }
}


bool
TGtDemo::SaveGraphData( GraphData *gr )
{
// We can only have one Plot dialog (modal one) so condition should be omitted!!
     if( !gd_gr )
      return false;
     if( gr != gd_gr->getGraphData() )
      return false;
    gdp->axisType[0] = gr->axisType;
    gdp->axisType[1] = gr->b_color[0];
    gdp->axisType[2] = gr->b_color[1];
    gdp->axisType[3] = gr->b_color[2];
    strncpy( gdp->xNames, gr->xName.c_str(), 9);
    strncpy( gdp->yNames, gr->yName.c_str(), 9);
    memcpy( &gdp->size[0], gr->region, 4*sizeof(float) );
    memcpy( &gdp->size[1], gr->part,  4*sizeof(float) );

    plot = (TPlotLine *)
           aObj[ o_gdplline].Alloc( gr->lines.GetCount(), sizeof(TPlotLine));
    for(uint ii=0; ii<gr->lines.GetCount(); ii++ )
    {
        plot[ii] = gr->lines[ii];
        //  lNam0 and lNamE back
        if(ii < gdp->dimXY[1] )
            strncpy(  gdp->lNam0[ii], plot[ii].name, MAXGRNAME );
        else
            strncpy(  gdp->lNamE[ii-gdp->dimXY[1]], plot[ii].name, MAXGRNAME );
    }
    //    }
    pVisor->Update();
    fEdit = true;
    return true;
}

void
TGtDemo::CmHelp()
{
    pVisor->OpenHelp( GEMS_GTD_HTML );  //  05.01.01
}

//-----------------------------------------------------------------------
// Statistic to probe
/*
void
TGtDemo::elst(int N,double *U,double *par)
//   N  -  size; U  -   vector to analiz
//  par: {  Max,Min,M(mat. expectation),Me(median), D(dispersija)
//  SG (kvadrat.otklonenue),OS(error srednego), KV (coeff variation),
//  VS (variation of average)  }
{ int i,k,j;
    double X;
    for(i=0;i<N-1;i++)
    {
        k=i;
        while(1)
        {
            j=k+1;
            if(U[k]<=U[j]) break;
            X=U[k];
            U[k]=U[j];
            U[j]=X;
            k--;
            if(k<0) break;
        }
    }
    par[1]=U[N-1];
    par[0]=U[0];
    par[2]=par[4]=0.;
    for(i=0;i<N;i++)
        par[2]+=U[i];
    par[2]/=N;
    for(i=0;i<N;i++)
    {
        X=U[i]-par[2];
        par[4]+=X*X;
    }
    par[4]/=(N-1);
    par[5]=sqrt(par[4]);
    i=floor(N/2);
    if(i*2==N) par[3]=(U[i-1]+U[i])/2.;
    else  par[3]=U[i];
    if(fabs(par[2])>0.)
    {
        par[7]=fabs(par[5]/(par[2]))*100.;
        par[8]=fabs(par[5]/(par[2]*sqrt(N)))*100.;
    }
    else
        if(fabs(par[0]-par[1])<1e-10) par[7]=par[8]=0.;
        else par[7]=par[8]=7777;
    if(N>0)
        par[6]=par[5]/sqrt(N);
    else
        par[6]=0.;
}

char pb_kr1[7][30] ={           "Criterion Valda",
                                "Criterion Laplasa",           "Criterion Karpova (medium)",
                                "Criterion Karpova (median)", "Criterion Sevidga",
                                "Criterion Gurvitsa (a=0)"  ,  "Criterion Gurvitsa (a=0.5)"};

// Table of Probe resalts
void TGtDemo::probe_stat( const char *key )
{
    int i, j, k, N, M, nPar;
    short nI, nF, QT,*iopt;
    size_t tblen;
    vstr pbuf(121);
    char  (*lNames)[MAXGRNAME];
    double *y, *U, par[9];
    TProbe* PRob = (TProbe*)(&aMod[RT_PROBE]);
    TProfil* PRof = (TProfil*)(&aMod[RT_PARAM]);

    if( gdp->PsPB != S_OFF && *gdp->prKey)    // read probe record 
        TProbe::pm->RecInput( gdp->prKey );
    else
        Error( GetName(), "No Probe mode in GtDemo!");

    nI = PRob->GetnI();
    nF = PRob->GetnF();
    QT = PRob->GetQT();
    iopt = PRob->GetIopt();

    SetString("Please, wait until arrays will be sampled");
    pVisor->Update();
    //Table heard *

    TPrintTable prTab( gdp->name, GetName(), key );

    for( k=0; k<2; k++ ) // statistic by work arrays
    {
        if( k==0 )
        {
            N = gdp->dimXY[0];
            M = gdp->dimXY[1];
        }
        else
        {
            N = gdp->dimEF[0];
            M = gdp->dimEF[1];
        }
        if( !(M*N) ) continue;
        if( k==0)
            sprintf( pbuf, si_tab_3c, k, gdp->xNames, M, N  );
        else  sprintf( pbuf, si_tab_3c, k, gd_userf, M,  N  );
        prTab.PutString( pbuf );
        switch( k ) //set pointers
        {
        case 0:
            lNames = gdp->lNam0;
            y = gdp->y0;
            break;
        case 1:
            lNames = gdp->lNamE;
            y = gdp->yE;
            break;
        }
        prTab.PutString(
            "\n                                                                 STATISTICS \n"
            "|=========|=========|=========|=========|=======|========|==========|========|\n"
            "|Parameter|   Min   |   Max   | Mean    |Stand. | Mean   |Coeff.Var.|Var.mean|\n"
            "|         |         |         |         |deviat.|error   |    (%)   |   (%)  |\n"
            "|=========|=========|=========|=========|=======|========|==========|========|");
        U = new double[N];
        for( i=0; i<M; i++ )
        {
            for (j=0; j<N; j++)
                U[j]= y[j*M+i];
            elst( N, U, par);
            sprintf( pbuf,"|%-9.7s|%9.2e|%9.2e|%9.2e|%7.1e|%8.2e|  %6.2f  | %6.2f |",
                     lNames[i], par[0], par[1], par[2], par[3], par[4], par[5], par[6] );
            prTab.PutString( pbuf );
        }
        delete[] U;
        prTab.PutString(
            "|=========|=========|=========|=========|=======|========|==========|========|\n");
    }
    // select phases
    if( iopt && nI > 0 )
    {
        prTab.PutString( "\n                             PHASE ASSEMBLAGES \n"
                         "|=======|====================|===================|\n"
                         "| N     |      Phases        | Sampled Frequency |\n"
                         "|=======|====================|===================|");
        for( i=0,j=0, k=0; j< nF-1; j++)
        {
            if( iopt[j] == -1)
            {
                prTab.PutString( "|-------|--------------------|-------------------|");
                i = 0;
                continue;
            }
            switch(i++)
            {
            case 0:
                nPar = iopt[j];
                k++;
                break;
            case 1:
                sprintf( pbuf, "|  %-4d |  %-18.16s|   %3d ( %6.2f%% ) |", k,
                         PRof->mup->SF[iopt[j]]+ MAXSYMB+MAXPHSYMB, nPar,
                         (float)nPar/QT*100);
                prTab.PutString( pbuf );
                break;
            default:
                sprintf( pbuf, "|       |  %-18.16s|                   |",
                         PRof->mup->SF[iopt[j]]+ MAXSYMB+MAXPHSYMB );
                prTab.PutString( pbuf );
            }
        }
        prTab.PutString( "|=======|====================|===================|\n");
        //  Put Criterions
        prTab.PutString( "\n       Get optimal solution for criterion\n"
                         "|===========================|===============|\n"
                         "|       Criterion           | solution num. |\n"
                         "|===========================|===============|" );
        for( i=0, j=nF; i<7; i++, j++ )
        {
            sprintf( pbuf, "| %-26s|     %4d      |", pb_kr1[i], iopt[j] );
            prTab.PutString( pbuf );
        }
        prTab.PutString( "|===========================|===============|\n" );
    }
    // Put equations
    if( vfQuestion(window(), GetName(), "Print sampling equations?" ) )
    {
        prTab.PutString( "\n Sampling equations:\n--------------------------" );
        prTab.PutString( (char *)aObj[o_gdexpr].GetPtr() );
    }
    prTab.PutEnd();
    // Put results to object
    const char *tab= prTab.GetTable();
    tblen = strlen(tab)+1; // Get size of text buf 
    gdp->prtab = (char *)aObj[ o_gwprtab ].Alloc( 1, tblen, S_ );
    memcpy( gdp->prtab, tab, tblen );

    vfObjToFile(window(), &aObj[o_gwprtab]);
} */
//--------------------- End of m_gtdemo.cpp ---------------------------
