//-------------------------------------------------------------------
// $Id$
//
// Implementation of TRTParm class, config and calculation functions
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
const char *GEMS_RTP_HTML = "gm_rtparm";

#include <math.h>
#include <stdio.h>

#include "m_rtparm.h"
#include "m_param.h"
#include "m_dcomp.h"
#include "m_reacdc.h"
#include "m_sdata.h"
#include "v_object.h"
#include "visor.h"
#include "service.h"

TRTParm* TRTParm::pm;

// get key of record
gstring
TRTParm::GetKeyofRecord( const char *oldKey, const char *strTitle,
                         int keyType )
{
    gstring str;
    if( keyType == KEY_NEW )
    {
        switch ( vfQuestion3(window(), GetName(),
                             "Get key from records?", "&DComp", "&React" ))
        {
        case VF3_1:
            str = TDComp::pm->GetKeyofRecord( ALLKEY,
                 "Select DComp key to new RTparm.", true);
            str +="d:*:";
            break;
        case VF3_2:
            str = TReacDC::pm->GetKeyofRecord( ALLKEY,
                 "Select DComp key to new RTparm.", true);
            str +="r:*:";
            break;
        case VF3_3:
            break;
        }
        str = TCModule::GetKeyofRecord( str.c_str(), strTitle, keyType );
    }
    else str = TCModule::GetKeyofRecord( oldKey, strTitle, keyType );

    return str;
}



TRTParm::TRTParm( int nrt ):
        TCModule( nrt )
{
    nQ = 1;
    aFldKeysHelp.Add(
        "l<4  Phase state of (reaction-defined) DC <-REACDC (<-DCOMP)");
    aFldKeysHelp.Add(
        "l<8  Identifier of (REAC)DC group  <-REACDC (<-DCOMP)");
    aFldKeysHelp.Add(
        "l<16 Name of reaction-defined DC (compound) <-REACDC (<-DCOMP)");
    aFldKeysHelp.Add(
        "Code of thermodynamic dataset (file) <-REACDC (<-DCOMP)");
    aFldKeysHelp.Add(
    "l<4  Source of input data for RTPARM-task { r d }, r: REACDC, d: DCOMP");
    aFldKeysHelp.Add(
        "l<12  Variant number of RTPARM-task <integer>");
    rpp=&rp[0];
    set_def();
    start_title = " Tabulation/plots of thermodynamic data for one species";
    gd_gr = 0;
}

// link values to objects
void TRTParm::ods_link( int q)
{
    ErrorIf( q > nQ, GetName(), "Illegal link q>nQ.");
    // static objects
    aObj[ o_rpunit].SetPtr( &rp[q].What );    /*10*/
    aObj[ o_rpdim].SetPtr(  &rp[q].NP );      /*i3*/
    aObj[ o_rpmode].SetPtr( &rp[q].Mode );
    aObj[o_rpnumb].SetPtr(  &rp[q].Nsd );      /*i2*/
    aObj[o_rpdimxy].SetPtr(  rp[q].dimXY );    /*i2*/
    aObj[o_rpdimef].SetPtr(  rp[q].dimEF );    /*i2*/
    aObj[ o_rppi].SetPtr(    rp[q].Pi );       /*f3*/
    aObj[ o_rpti].SetPtr(    rp[q].Ti );       /*f3*/
    aObj[o_rtname].SetPtr(  rp[q].name );
    aObj[o_rtcom].SetPtr(   rp[q].comment );
    aObj[o_rtaxis].SetPtr(  rp[q].axisType );   /*i6*/
    aObj[o_rtsize].SetPtr(  rp[q].size[0] );    /*f8*/
    aObj[o_rtxnames].SetPtr(  rp[q].xNames );
    aObj[o_rtynames].SetPtr(  rp[q].yNames );

    aObj[o_rtjtp].SetPtr( &rp[q].jTP );    /*i1*/
    aObj[o_rtie].SetPtr(  &rp[q].iE );    /*i1*/
    aObj[o_rtje].SetPtr(  &rp[q].jE );    /*i1*/
    // dynamic objects
    aObj[ o_rtlnam].SetPtr( rp[q].lNam[0] );
    aObj[ o_rtlnam].SetDim( 1, rp[q].dimXY[1] );
    aObj[ o_rtlname].SetPtr( rp[q].lNamE[0] );
    aObj[ o_rtlname].SetDim( 1, rp[q].dimEF[1] );

    aObj[o_rtexpr].SetPtr( rp[q].expr );
    aObj[o_rtexpre].SetPtr(rp[q].exprE);
    aObj[ o_rtsdref].SetPtr( rp[q].sdref);
    aObj[ o_rtsdref].SetDim( rp[q].Nsd, 1 );
    aObj[ o_rtsdval].SetPtr( rp[q].sdval);
    aObj[ o_rtsdval].SetDim( rp[q].Nsd, 1 );
    aObj[o_rpxt].SetPtr( rp[q].T );
    aObj[ o_rpxt].SetDim(rp[q].dimXY[0], 1 );
    aObj[o_rpxp].SetPtr( rp[q].P );
    aObj[ o_rpxp].SetDim(rp[q].dimXY[0], 1 );
    aObj[o_rpyf ].SetPtr( rp[q].F );
    aObj[ o_rpyf].SetDim(rp[q].dimXY[0], rp[q].dimXY[1] );
    aObj[o_rpxte].SetPtr( rp[q].TE );
    aObj[ o_rpxte].SetDim(rp[q].dimEF[0], 1 );
    aObj[o_rpxpe].SetPtr( rp[q].PE );
    aObj[ o_rpxpe].SetDim(rp[q].dimEF[0], 1 );
    aObj[o_rpyte ].SetPtr( rp[q].FE );
    aObj[ o_rpyte].SetDim(rp[q].dimEF[0], rp[q].dimEF[1] );

    aObj[ o_rttext].SetPtr(  rp[q].trpn );
    aObj[ o_rppline].SetPtr( plot );
    aObj[ o_rppline].SetDim( rp[q].dimXY[1]+rp[q].dimEF[1], sizeof(TPlotLine));
    rpp=&rp[q];
}

// set dynamic Objects ptr to values
void TRTParm::dyn_set(int q)
{
    ErrorIf( rpp!=&rp[q], GetName(),
             "Illegal access to rp in dyn_set.");
//    memcpy( rpp->pstate, rt[nRT].UnpackKey(), RP_RKLEN-MAXSYMB-1 );
//    rpp->nvch = 0;
    rpp->lNam = (char (*)[MAXGRNAME])aObj[ o_rtlnam ].GetPtr();
    rpp->lNamE = (char (*)[MAXGRNAME])aObj[ o_rtlname ].GetPtr();
    rpp->expr = (char *)aObj[ o_rtexpr ].GetPtr();
    rpp->exprE = (char *)aObj[ o_rtexpre ].GetPtr();
    rpp->T    = (double *)aObj[ o_rpxt ].GetPtr();
    rpp->P    = (double *)aObj[ o_rpxp ].GetPtr();
    rpp->F    = (double *)aObj[ o_rpyf ].GetPtr();
    rpp->TE    = (double *)aObj[ o_rpxte ].GetPtr();
    rpp->PE    = (double *)aObj[ o_rpxpe ].GetPtr();
    rpp->FE    = (double *)aObj[ o_rpyte ].GetPtr();
    rpp->trpn  = (char *)aObj[ o_rttext ].GetPtr();
    plot  = (TPlotLine *)aObj[ o_rppline ].GetPtr();
    rpp->sdref = (char (*)[V_SD_RKLEN])aObj[ o_rtsdref ].GetPtr();
    rpp->sdval = (char (*)[V_SD_VALEN])aObj[ o_rtsdval ].GetPtr();
}

// free dynamic memory in objects and values
void TRTParm::dyn_kill(int q)
{
    ErrorIf( rpp!=&rp[q], GetName(),
             "Illegal access to rp in dyn_kill.");
    rpp->lNam = (char (*)[MAXGRNAME])aObj[ o_rtlnam ].Free();
    rpp->lNamE = (char (*)[MAXGRNAME])aObj[ o_rtlname ].Free();
    rpp->expr = (char *)aObj[ o_rtexpr ].Free();
    rpp->exprE = (char *)aObj[ o_rtexpre ].Free();
    rpp->T    = (double *)aObj[ o_rpxt ].Free();
    rpp->P    = (double *)aObj[ o_rpxp ].Free();
    rpp->F    = (double *)aObj[ o_rpyf ].Free();
    rpp->TE    = (double *)aObj[ o_rpxte ].Free();
    rpp->PE    = (double *)aObj[ o_rpxpe ].Free();
    rpp->FE    = (double *)aObj[ o_rpyte ].Free();
    rpp->trpn  = (char *)aObj[ o_rttext ].Free();
    plot  = (TPlotLine *)aObj[ o_rppline ].Free();
    rpp->sdref = (char (*)[V_SD_RKLEN])aObj[ o_rtsdref ].Free();
    rpp->sdval = (char (*)[V_SD_VALEN])aObj[ o_rtsdval ].Free();
}

// realloc dynamic memory
void TRTParm::dyn_new(int q)
{
    ErrorIf( rpp!=&rp[q], GetName(), "Illegal access to rp in dyn_new.");
    ErrorIf( rp[q].NV < 1, GetName(), "Total N of points (rp[q].NV) < 1.");

    rpp->lNam = (char (*)[MAXGRNAME])aObj[ o_rtlnam ].Alloc( 1,
                 rpp->dimXY[1], MAXGRNAME);
    rpp->expr = (char *)aObj[ o_rtexpr ].Alloc(1, 2048, S_);

    rpp->T  = (double *)aObj[ o_rpxt ].Alloc(rpp->dimXY[0], 1, D_);
    rpp->P  = (double *)aObj[ o_rpxp ].Alloc(rpp->dimXY[0], 1, D_);
    rpp->F  = (double *)aObj[ o_rpyf ].Alloc(rpp->dimXY[0], rpp->dimXY[1], D_);

    if( rpp->Pplot == S_OFF )
    {
        rpp->lNamE = (char (*)[MAXGRNAME])aObj[ o_rtlname ].Free();
        rpp->exprE = (char *)aObj[ o_rtexpre ].Free();
        rpp->TE    = (double *)aObj[ o_rpxte ].Free();
        rpp->PE    = (double *)aObj[ o_rpxpe ].Free();
        rpp->FE    = (double *)aObj[ o_rpyte ].Free();
        rpp->dimEF[1] = rpp->dimEF[0] = 0;
    }
    else
    {
        rpp->lNamE = (char (*)[MAXGRNAME])aObj[ o_rtlname ].Alloc(1,
                     rpp->dimEF[1], MAXGRNAME);
        rpp->exprE = (char *)aObj[ o_rtexpre ].Alloc(1, 2048, S_);
        rpp->TE    = (double *)aObj[ o_rpxte ].Alloc(rpp->dimEF[0], 1, D_);
        rpp->PE    = (double *)aObj[ o_rpxpe ].Alloc(rpp->dimEF[0], 1, D_);
        rpp->FE=(double *)aObj[ o_rpyte ].Alloc(rpp->dimEF[0],rpp->dimEF[1],D_);
    }
    if( rpp->Nsd > 0 )
    {
        rpp->sdref =
        (char (*)[V_SD_RKLEN])aObj[ o_rtsdref].Alloc( rpp->Nsd, 1, V_SD_RKLEN );
        rpp->sdval =
        (char (*)[V_SD_VALEN])aObj[ o_rtsdval].Alloc( rpp->Nsd, 1, V_SD_VALEN );
    }
    else
    {
        rpp->sdref = (char (*)[V_SD_RKLEN])aObj[ o_rtsdref ].Free();
        rpp->sdval = (char (*)[V_SD_VALEN])aObj[ o_rtsdval ].Free();
    }

    rpp->trpn = (char *)aObj[ o_rttext ].Alloc(1, 2048, S_);
}


//set default information
void TRTParm::set_def( int q)
{
    ErrorIf( rpp!=&rp[q], GetName(), "Illegal access to rp in set_def.");
    TProfil *aPa=(TProfil *)(&aMod[RT_PARAM]);

    memcpy( &rp[q].What, aPa->pa.RPpdc, 10 );
    strcpy( rp[q].name,  "`" );   // Fixed for debugging
    strcpy( rp[q].comment, "`" );
    strcpy( rp[q].xNames,  "xTP" );   // Fixed for debugging
    strcpy( rp[q].yNames, "yF" );

    rp[q].NT = aPa->pa.NT;
    rp[q].NP = aPa->pa.NP;
    rp[q].Mode = aPa->pa.Mode;
    rp[q].NV = aPa->pa.NV;
    switch( rp[q].Mode )
    {
    default:
    case 0:
        rp[q].NV = max( rp[q].NP, rp[q].NT );
        break;
    case 1:
    case 2:
        rp[q].NV = rp[q].NP * rp[q].NT;
        break;
    case 3:
        rp[q].NV = min( rp[q].NP, rp[q].NT );
        break;
    }
    rp[q].Pi[0] = aPa->pa.Pi[0];
    rp[q].Pi[1] = aPa->pa.Pi[1];
    rp[q].Pi[2] = aPa->pa.Pi[2];
    rp[q].Ti[0] = aPa->pa.Ti[0];
    rp[q].Ti[1] = aPa->pa.Ti[1];
    rp[q].Ti[2] = aPa->pa.Ti[2];
    memset( &rpp->Nsd, 0, 12*sizeof(short));
    rpp->lNam = 0;
    rpp->lNamE = 0;
    rpp->expr = 0;
    rpp->exprE = 0;
    rpp->T    = 0;
    rpp->P    = 0;
    rpp->F    = 0;
    rpp->TE    = 0;
    rpp->PE    = 0;
    rpp->FE    = 0;
    rpp->trpn  = 0;
    plot  = 0;
    rpp->sdref = 0;
    rpp->sdval = 0;
}

// return true if necessary recalc
bool TRTParm::check_input( const char *key, int Level )
{
    vstr pkey(MAXRKEYLEN+10);
    const char *srec;
    time_t tr, tra;
    bool iRet= false;

    rt[RT_REACDC].MakeKey( RT_RTPARM, pkey,  RT_RTPARM, 0, RT_RTPARM, 1,
                            RT_RTPARM, 2, RT_RTPARM, 3, K_END );
    srec = key + MAXSYMB+MAXDRGROUP+MAXDCNAME+MAXSYMB;
    if( *srec == SRC_DCOMP )
    {
        TDComp* aDC=(TDComp *)(&aMod[RT_DCOMP]);
        aDC->ods_link(0);
        aDC->TCModule::TryRecInp( pkey, tr, 0 );
    }
    else
    {
        TReacDC* aRC=(TReacDC *)(&aMod[RT_REACDC]);
        aRC->ods_link(0); //Sveta 16/11/99
        aRC->TCModule::TryRecInp( pkey, tr, 0 );
    }
    if( abs(Level) == 1 )
    {
        tra = rt[nRT].Rtime();
        if( tr >= tra )
            iRet = true;
    }
    else iRet = true;
    return iRet;
}


// Input nessasery data and links objects
void TRTParm::RecInput( const char *key )
{
    TCModule::RecInput( key );
}

//Rebild record structure before calc
int
TRTParm::RecBuild( const char *key, int mode  )
{
AGAIN_SETUP:
    int ret = TCModule::RecBuild( key, mode );
    if( ret == VF_CANCEL )
        return ret;
    if( ret == VF3_1 )
    {
        strncpy( rpp->name, db->FldKey(2), db->FldLen(2));
        rpp->name[db->FldLen(2)] = '\0';
    }

    switch( rpp->Mode )
    {
    default:
    case 0:
        rpp->NV = max( rpp->NP, rpp->NT );
        break;
    case 1:
    case 2:
        rpp->NV = rpp->NP * rpp->NT;
        break;
    case 3:
        rpp->NV = min( rpp->NP, rpp->NT );
        break;
    }
    if( rpp->NV < 1 || rpp->NV > 4192 )
    {
        if(vfQuestion(window(), GetName(),
                 "Invalid mode of calculation of P and T arrays,\n"
                 "or wrong number of TP points! Change?"))
            goto AGAIN_SETUP;
        else   Error( GetName(), "Invalid mode of calculation.");
    }

    if( rpp->Pplot == S_OFF )
         if( rpp->dimEF[0]>0 && rpp->dimEF[1]> 0)
             rpp->Pplot = S_ON;

    if( (rpp->dimEF[0]<=0 || rpp->dimEF[1]<=0) && rpp->Pplot != S_OFF )
    {
        vfMessage(window(), GetName(),
        "Wrong dimensions set for the empirical data array!");
        goto AGAIN_SETUP;
    }

    if(  rpp->dimXY[1] < 1 || rpp->dimXY[1] > 20 )
    {
        vfMessage(window(), GetName(),
        "Wrong dimensions set for the data array!");
        goto AGAIN_SETUP;
    }
    rpp->What = *(key + MAXSYMB+MAXDRGROUP+MAXDCNAME+MAXSYMB);
    rpp->dimXY[0] = rpp->NV;
    dyn_new();

    TProfil *aPa=TProfil::pm;
    vstr tbuf(100);

    if( rpp->Pplot != S_OFF  )
    {
        for(int i=0; i<rpp->dimEF[1]; i++ )
        {
            sprintf( tbuf, "%s%d", aPa->pa.GDpsc, i+1 );
            if( !*rpp->lNamE[i] || *rpp->lNamE[i] == ' ' )
                strncpy( rpp->lNamE[i], tbuf,MAXGRNAME );
        }
    }
    for(int j=0; j< rpp->dimXY[1]; j++ )
    {
        sprintf( tbuf, "%s%d", aPa->pa.GDpsc, j+1 );
        if( !*rpp->lNam[j]|| *rpp->lNam[j] == ' ' )
            strncpy( rpp->lNam[j], tbuf, MAXGRNAME );
    }

    pVisor->Update();
    return ret;
}

// Translate, analyze and unpack equations of process
void TRTParm::expr_analyze()
{
    try
    {
        rpn[0].GetEquat( (char *)aObj[o_rtexpr].GetPtr() );
    }
    catch( TError& xcpt )
    {
        char *erscan = (char *)aObj[o_rtexpr].GetPtr();
        vfMessage(window(), xcpt.title, xcpt.mess);
        CheckEqText(  erscan,
                      "Error in translation of equations of demonstration " );
        Error(  GetName() , xcpt.mess.c_str() );
    }
}

// Translate and calculate equations of empirical data
void TRTParm::exprE_calc()
{
    try
    {
     if( !aObj[o_rtexpre].IsNull())
        if(  (!aObj[o_rtexpre].IsEmpty(0,0)) &&
             (strncmp( (char *)aObj[o_rtexpre].GetPtr(), "---", 3) != 0))
             rpn[1].GetEquat( (char *)aObj[o_rtexpre].GetPtr() );
    }
    catch( TError& xcpt )
    {
        char *erscan = (char *)aObj[o_rtexpre].GetPtr();
        vfMessage(window(), xcpt.title, xcpt.mess);
        CheckEqText(  erscan,
                      "Error in translation of equations of empirical data" );
        Error(  GetName() , xcpt.mess.c_str() );
    }
    // calc equations
    for( rpp->iE = 0; rpp->iE< rpp->dimEF[0]; rpp->iE++ )
        rpn[1].CalcEquat();
}


//Recalc record structure
void
TRTParm::RecCalc( const char *key )
{
  short ij, i, j;
  double P_old;

    /* insert values to arrays  rp->T, rp->P, rp->RT; */
   switch( rpp->Mode )
    {
    default:
    case 0: /* insert by hand */
        break;
    case 1: /* by P, then by T */
        for( ij=0, i=0; i<rpp->NT; i++ )
        {
            rpp->T[ij] = rpp->Ti[0] + rpp->Ti[2] * i;
            if( rpp->T[ij] > rpp->Ti[1] )
                rpp->T[ij] = rpp->Ti[1];
            for( j=0; j<rpp->NP; j++ )
            {
                rpp->P[ij] = rpp->Pi[0] + rpp->Pi[2] * j;
                if( rpp->P[ij] > rpp->Pi[1] )
                    rpp->P[i] = rpp->Pi[1];
                if( j ) rpp->T[ij] = rpp->T[ij-1];
                ij++;
            }
        }
        break;
    case 2: /* by T, then by P */
        for( ij=0, i=0; i<rpp->NP; i++ )
        {
            rpp->P[ij] = rpp->Pi[0] + rpp->Pi[2] * i;
            if( rpp->P[ij] > rpp->Pi[1] )
                rpp->P[ij] = rpp->Pi[1];
            for( j=0; j<rpp->NT; j++ )
            {
                rpp->T[ij] = rpp->Ti[0] + rpp->Ti[2] * j;
                if( rpp->T[ij] > rpp->Ti[1] )
                    rpp->T[ij] = rpp->Ti[1];
                if( j ) rpp->P[ij] = rpp->P[ij-1];
                ij++;
            }
        }
        break;
    case 3: /* T,P index parallel */
        for( i=0; i<rpp->NV; i++ )
        {
            rpp->P[i] = rpp->Pi[0] + rpp->Pi[2] * i;
            if( rpp->P[i] > rpp->Pi[1] )
                rpp->P[i] = rpp->Pi[1];
            rpp->T[i] = rpp->Ti[0] + rpp->Ti[2] * i;
            if( rpp->T[i] > rpp->Ti[1] )
                rpp->T[i] = rpp->Ti[1];
        }
        break;
    }
    pVisor->Update();
    expr_analyze(); // Translate equations before calc
    // Translate and calculate equations of empirical data
    if( rpp->Pplot != S_OFF )
        exprE_calc();


    /* calc t/d  properties */
    for( j=0; j<rpp->NV; j++ )
    {
        rpp->jTP = j;
        pVisor->Message( window(), GetName(),
           "Sampling the xT, xP, yF data \n"
                 "Please, wait...", j, rpp->NV );
        // rpp->RT[j] = R_CONSTANT * ( rpp->T[j] + C_to_K );
         aW.ods_link( 0 );
        /* clear new work structure */
        aW.set_zero( 0 );
        aW.twp->P = P_old = rpp->P[j];
        aW.twp->TC = rpp->T[j];
        aW.twp->T = aW.twp->TC + C_to_K;
        aW.twp->unE = rpp->PunE;
        aW.twp->unV = rpp->PunV;
        /* calc t/d properties of component  */
        switch( rpp->What )
        {
        case SRC_DCOMP:
            {
                TDComp* aDC=(TDComp *)(&aMod[RT_DCOMP]);
                aW.twp->TCst = aDC->dcp->TCst;
                aDC->DCthermo( 0, 0 );
            }
            break;
        case SRC_REACDC:
            {
                TReacDC* aRDC=(TReacDC *)(&aMod[RT_REACDC]);
                aW.twp->TCst = aRDC->rcp->TCst;
                aRDC->RCthermo( 0, 0 );
                aRDC->ods_link(0);
            }
            break;
        default:
            break;
        }
        /* Set resalts to arreys */
        aW.ods_link(0);
        if( P_old < 1e-6 && rpp->P[j] < 1e-6 )
        {  /* set pressision calc by TNP*/
            rpp->P[j] = aW.twp->P;
        }
        // calculate equations of  data
        rpn[0].CalcEquat();
        aMod[nRT].ModUpdate("Calculation of RTparm arrays");
    }
    SetString("RTPARM arrays calculated OK");
    TCModule::RecCalc(key);
    pVisor->CloseMessage();

}

void
TRTParm::CmHelp()
{
    pVisor->OpenHelp( GEMS_RTP_HTML );  //  05.01.01
}


void
TRTParm::RecordPlot( const char* /*key*/ )
{
   TIArray<TPlot> plt;

   if( rpp->Pabs == 'P')
     plt.Add( new TPlot( o_rpxp, o_rpyf ));
   else
     plt.Add( new TPlot( o_rpxt, o_rpyf ));

   int  nLn = plt[ 0 ].getLinesNumber();
   if( rpp->Pplot != S_OFF )
    {
       if( rpp->Pabs == 'P')
         plt.Add( new TPlot( o_rpxpe, o_rpyte ));
       else
         plt.Add( new TPlot( o_rpxte, o_rpyte ));
        nLn += plt[1].getLinesNumber();
    }
    if( plot )
    {
        int oldN = aObj[o_rppline].GetN();
        TPlotLine defpl("", 4);

        plot = (TPlotLine * )aObj[ o_rppline ].Alloc( nLn, sizeof(TPlotLine) );
        for(int ii=0; ii<nLn; ii++ )
        {
            if( ii >= oldN )
                plot[ii] = defpl;
            if(ii < rpp->dimXY[1] )
                strncpy( plot[ii].name, rpp->lNam[ii], MAXGRNAME );
            else
                strncpy( plot[ii].name, rpp->lNamE[ii-rpp->dimXY[1]],
                     MAXGRNAME );
            plot[ii].name[MAXGRNAME] = '\0';
        }
        gd_gr = new GraphWindow( this, plt, rpp->name,
                                     rpp->size[0], rpp->size[1], plot,
                                     rpp->axisType, rpp->xNames, rpp->yNames);
    }
    else
    {
      TCStringArray lnames;
      int ii;
      for( ii=0; ii<rpp->dimXY[1]; ii++ )
          lnames.Add( gstring(rpp->lNam[ii], 0, MAXGRNAME ));
      for( ii=0; ii<rpp->dimEF[1]; ii++ )
          lnames.Add( gstring( rpp->lNamE[ii], 0, MAXGRNAME ));
      gd_gr = new GraphWindow( this, plt, rpp->name,
          rpp->xNames, rpp->yNames, lnames );
    }
}


bool
TRTParm::SaveGraphData( GraphData *gr )
{
// We can only have one Plot dialog (modal one) so condition should be omitted!!
     if( !gd_gr )
      return false;
     if( gr != gd_gr->getGraphData() )
      return false;
    rpp->axisType[0] = gr->axisType;
    rpp->axisType[1] = gr->b_color[0];
    rpp->axisType[2] = gr->b_color[1];
    rpp->axisType[3] = gr->b_color[2];
    strncpy( rpp->xNames, gr->xName.c_str(), 9);
    strncpy( rpp->yNames, gr->yName.c_str(), 9);
    memcpy( &rpp->size[0], gr->region, 4*sizeof(float) );
    memcpy( &rpp->size[1], gr->part,  4*sizeof(float) );

    plot = (TPlotLine *)
    aObj[ o_rppline].Alloc( gr->lines.GetCount(), sizeof(TPlotLine));
    for(uint ii=0; ii<gr->lines.GetCount(); ii++ )
    {
        plot[ii] = gr->lines[ii];
        //  lNam0 and lNamE back
        if(ii < rpp->dimXY[1] )
            strncpy(  rpp->lNam[ii], plot[ii].name, MAXGRNAME );
        else
            strncpy(  rpp->lNamE[ii-rpp->dimXY[1]], plot[ii].name, MAXGRNAME );
    }
    pVisor->Update();
    fEdit = true;
    return true;
}

//------------------- End of m_rtparm.cpp ---------------------------

