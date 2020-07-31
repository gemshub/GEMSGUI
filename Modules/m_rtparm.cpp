//-------------------------------------------------------------------
// $Id: m_rtparm.cpp 1178 2009-01-19 17:09:37Z gems $
//
// Implementation of TRTParm class, config and calculation functions
//
// Rewritten from C to C++ by S.Dmytriyeva  
// Copyright (C) 1995-2001 S.Dmytriyeva, D.Kulik
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

//#include <cmath>
//#include <cstdio>
#include "m_rtparm.h"
#include "m_param.h"
#include "m_dcomp.h"
#include "m_reacdc.h"
#include "visor.h"
#include "service.h"

TRTParm* TRTParm::pm;

// get key of record
std::string
TRTParm::GetKeyofRecord( const char *oldKey, const char *strTitle,
                         int keyType )
{
    std::string str;
    if( keyType == KEY_NEW )
    {
        switch ( vfQuestion3(window(), GetName(),
            "Please, select a source record from:", "&DComp", "&ReacDC" ))
        {
        case VF3_1:
            str = TDComp::pm->GetKeyofRecord( ALLKEY,
                 "Select one key of a source DComp record ", true);
            str +="d:*:";
            break;
        case VF3_2:
            str = TReacDC::pm->GetKeyofRecord( ALLKEY,
                 "Select one key of a source ReacDC record ", true);
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



TRTParm::TRTParm( uint nrt ):
        TCModule( nrt )
{
    nQ = 1;
    aFldKeysHelp.push_back("Phase state of source Dependent Component (DComp or ReacDC)");
    aFldKeysHelp.push_back("Group to which source Dependent Component belongs");
    aFldKeysHelp.push_back("Name of source Dependent Component");
    aFldKeysHelp.push_back("Code of source thermodynamic data set");
    aFldKeysHelp.push_back("Source of input data for DC { r d }");
    aFldKeysHelp.push_back("Variant number of this RTParm calculation task <integer>");
    setKeyEditField(5);
    rpp=&rp[0];
    set_def();
    start_title = " Tabulation/plot of thermodynamic data for one DC (species)";
    gd_gr = nullptr;
}

// link values to objects
void TRTParm::ods_link( int q)
{
    ErrorIf( q > nQ, GetName(), "E00RTrem: Attempt to access corrupt dynamic memory.");
    // 3 objects
    aObj[ o_rpunit]->SetPtr( &rp[q].What );    /*10*/
    aObj[ o_rpdim]->SetPtr(  &rp[q].NP );      /*i3*/
    aObj[ o_rpmode]->SetPtr( &rp[q].Mode );
    aObj[o_rpnumb]->SetPtr(  &rp[q].Nsd );      /*i2*/
    aObj[o_rpdimxy]->SetPtr(  rp[q].dimXY );    /*i2*/
    aObj[o_rpdimef]->SetPtr(  rp[q].dimEF );    /*i2*/
    aObj[ o_rppi]->SetPtr(    rp[q].Pi );       /*f3*/
    aObj[ o_rpti]->SetPtr(    rp[q].Ti );       /*f3*/
    aObj[o_rtname]->SetPtr(  rp[q].name );
    aObj[o_rtcom]->SetPtr(   rp[q].comment );
    aObj[o_rtaxis]->SetPtr(  rp[q].axisType );   /*i6*/
    aObj[o_rtsize]->SetPtr(  rp[q].size[0] );    /*f8*/
    aObj[o_rtxnames]->SetPtr(  rp[q].xNames );
    aObj[o_rtynames]->SetPtr(  rp[q].yNames );

    aObj[o_rtjtp]->SetPtr( &rp[q].jTP );    /*i1*/
    aObj[o_rtie]->SetPtr(  &rp[q].iE );    /*i1*/
    aObj[o_rtje]->SetPtr(  &rp[q].jE );    /*i1*/
    // dynamic objects
    aObj[ o_rtlnam]->SetPtr( rp[q].lNam[0] );
    aObj[ o_rtlnam]->SetDim( 1, rp[q].dimXY[1] );
    aObj[ o_rtlname]->SetPtr( rp[q].lNamE[0] );
    aObj[ o_rtlname]->SetDim( 1, rp[q].dimEF[1] );

    aObj[o_rtexpr]->SetPtr( rp[q].expr );
    aObj[o_rtexpre]->SetPtr(rp[q].exprE);
    aObj[ o_rtsdref]->SetPtr( rp[q].sdref);
    aObj[ o_rtsdref]->SetDim( rp[q].Nsd, 1 );
    aObj[ o_rtsdval]->SetPtr( rp[q].sdval);
    aObj[ o_rtsdval]->SetDim( rp[q].Nsd, 1 );
    aObj[o_rpxt]->SetPtr( rp[q].T );
    aObj[ o_rpxt]->SetDim(rp[q].dimXY[0], 1 );
    aObj[o_rpxp]->SetPtr( rp[q].P );
    aObj[ o_rpxp]->SetDim(rp[q].dimXY[0], 1 );
    aObj[o_rpyf ]->SetPtr( rp[q].F );
    aObj[ o_rpyf]->SetDim(rp[q].dimXY[0], rp[q].dimXY[1] );
    aObj[o_rpxte]->SetPtr( rp[q].TE );
    aObj[ o_rpxte]->SetDim(rp[q].dimEF[0], 1 );
    aObj[o_rpxpe]->SetPtr( rp[q].PE );
    aObj[ o_rpxpe]->SetDim(rp[q].dimEF[0], 1 );
    aObj[o_rpyte ]->SetPtr( rp[q].FE );
    aObj[ o_rpyte]->SetDim(rp[q].dimEF[0], rp[q].dimEF[1] );

    aObj[ o_rttext]->SetPtr(  rp[q].trpn );
    aObj[ o_rppline]->SetPtr( plot );
    aObj[ o_rppline]->SetDim( rp[q].dimXY[1]+rp[q].dimEF[1], sizeof(TPlotLine));
    rpp=&rp[q];
}

// set dynamic Objects ptr to values
void TRTParm::dyn_set(int q)
{
    ErrorIf( rpp!=&rp[q], GetName(),
             "E01RTrem: Attempt to allocate corrupt dynamic memory.");
//    memcpy( rpp->pstate, rt[nRT].UnpackKey(), RP_RKLEN-MAXSYMB-1 );
//    rpp->nvch = 0;
    // Change MAXGRNAME from 7 to 16
    if(aObj[ o_rtlnam ]->GetType() == 7 )
        rpp->lNam = static_cast<char (*)[MAXGRNAME]>(aObj[ o_rtlnam ]->Alloc( 1, rpp->dimXY[1], MAXGRNAME));
    else
        rpp->lNam = static_cast<char (*)[MAXGRNAME]>(aObj[ o_rtlnam ]->GetPtr());

    if(aObj[ o_rtlname ]->GetType() == 7 && rpp->Pplot != S_OFF )
        rpp->lNamE = static_cast<char (*)[MAXGRNAME]>(aObj[ o_rtlname ]->Alloc(1,
                     rpp->dimEF[1], MAXGRNAME));
    else
        rpp->lNamE = static_cast<char (*)[MAXGRNAME]>(aObj[ o_rtlname ]->GetPtr());



    rpp->expr = static_cast<char *>(aObj[ o_rtexpr ]->GetPtr());
    rpp->exprE = static_cast<char *>(aObj[ o_rtexpre ]->GetPtr());
    rpp->T    = static_cast<double *>(aObj[ o_rpxt ]->GetPtr());
    rpp->P    = static_cast<double *>(aObj[ o_rpxp ]->GetPtr());
    rpp->F    = static_cast<double *>(aObj[ o_rpyf ]->GetPtr());
    rpp->TE    = static_cast<double *>(aObj[ o_rpxte ]->GetPtr());
    rpp->PE    = static_cast<double *>(aObj[ o_rpxpe ]->GetPtr());
    rpp->FE    = static_cast<double *>(aObj[ o_rpyte ]->GetPtr());
    rpp->trpn  = static_cast<char *>(aObj[ o_rttext ]->GetPtr());
    plot  = static_cast<TPlotLine *>(aObj[ o_rppline ]->GetPtr());
    rpp->sdref = static_cast<char (*)[V_SD_RKLEN]>(aObj[ o_rtsdref ]->GetPtr());
    rpp->sdval = static_cast<char (*)[V_SD_VALEN]>(aObj[ o_rtsdval ]->GetPtr());
}

// free dynamic memory in objects and values
void TRTParm::dyn_kill(int q)
{
    ErrorIf( rpp!=&rp[q], GetName(),
             "E02RTrem: Attempt to free corrupt dynamic memory.");
    rpp->lNam = static_cast<char (*)[MAXGRNAME]>(aObj[ o_rtlnam ]->Free());
    rpp->lNamE = static_cast<char (*)[MAXGRNAME]>(aObj[ o_rtlname ]->Free());
    rpp->expr = static_cast<char *>(aObj[ o_rtexpr ]->Free());
    rpp->exprE = static_cast<char *>(aObj[ o_rtexpre ]->Free());
    rpp->T    = static_cast<double *>(aObj[ o_rpxt ]->Free());
    rpp->P    = static_cast<double *>(aObj[ o_rpxp ]->Free());
    rpp->F    = static_cast<double *>(aObj[ o_rpyf ]->Free());
    rpp->TE    = static_cast<double *>(aObj[ o_rpxte ]->Free());
    rpp->PE    = static_cast<double *>(aObj[ o_rpxpe ]->Free());
    rpp->FE    = static_cast<double *>(aObj[ o_rpyte ]->Free());
    rpp->trpn  = static_cast<char *>(aObj[ o_rttext ]->Free());
    plot  = static_cast<TPlotLine *>(aObj[ o_rppline ]->Free());
    rpp->sdref = static_cast<char (*)[V_SD_RKLEN]>(aObj[ o_rtsdref ]->Free());
    rpp->sdval = static_cast<char (*)[V_SD_VALEN]>(aObj[ o_rtsdval ]->Free());
}

// realloc dynamic memory
void TRTParm::dyn_new(int q)
{
    ErrorIf( rpp!=&rp[q], GetName(), "E03RTrem: Attempt to allocate corrupt dynamic memory.");
    ErrorIf( rp[q].NV < 1, GetName(), "E04RTrem: Dynamic memory corruption in RTParm data structure");

    rpp->lNam = static_cast<char (*)[MAXGRNAME]>(aObj[ o_rtlnam ]->Alloc( 1,
                 rpp->dimXY[1], MAXGRNAME));
    rpp->expr = static_cast<char *>(aObj[ o_rtexpr ]->Alloc(1, 2048, S_));

    rpp->T  = static_cast<double *>(aObj[ o_rpxt ]->Alloc(rpp->dimXY[0], 1, D_));
    rpp->P  = static_cast<double *>(aObj[ o_rpxp ]->Alloc(rpp->dimXY[0], 1, D_));
    rpp->F  = static_cast<double *>(aObj[ o_rpyf ]->Alloc(rpp->dimXY[0], rpp->dimXY[1], D_));

    if( rpp->Pplot == S_OFF )
    {
        rpp->lNamE = static_cast<char (*)[MAXGRNAME]>(aObj[ o_rtlname ]->Free());
        rpp->exprE = static_cast<char *>(aObj[ o_rtexpre ]->Free());
        rpp->TE    = static_cast<double *>(aObj[ o_rpxte ]->Free());
        rpp->PE    = static_cast<double *>(aObj[ o_rpxpe ]->Free());
        rpp->FE    = static_cast<double *>(aObj[ o_rpyte ]->Free());
        rpp->dimEF[1] = rpp->dimEF[0] = 0;
    }
    else
    {
        rpp->lNamE = static_cast<char (*)[MAXGRNAME]>(aObj[ o_rtlname ]->Alloc(1,
                     rpp->dimEF[1], MAXGRNAME));
        rpp->exprE = static_cast<char *>(aObj[ o_rtexpre ]->Alloc(1, 2048, S_));
        rpp->TE    = static_cast<double *>(aObj[ o_rpxte ]->Alloc(rpp->dimEF[0], 1, D_));
        rpp->PE    = static_cast<double *>(aObj[ o_rpxpe ]->Alloc(rpp->dimEF[0], 1, D_));
        rpp->FE=static_cast<double *>(aObj[ o_rpyte ]->Alloc(rpp->dimEF[0],rpp->dimEF[1],D_));
    }
    if( rpp->Nsd > 0 )
    {
        rpp->sdref =  static_cast<char (*)[V_SD_RKLEN]>(aObj[ o_rtsdref]->Alloc( rpp->Nsd, 1, V_SD_RKLEN ));
        rpp->sdval =  static_cast<char (*)[V_SD_VALEN]>(aObj[ o_rtsdval]->Alloc( rpp->Nsd, 1, V_SD_VALEN ));
    }
    else
    {
        rpp->sdref = static_cast<char (*)[V_SD_RKLEN]>(aObj[ o_rtsdref ]->Free());
        rpp->sdval = static_cast<char (*)[V_SD_VALEN]>(aObj[ o_rtsdval ]->Free());
    }

    rpp->trpn = static_cast<char *>(aObj[ o_rttext ]->Alloc(1, 2048, S_));
}


//set default information - streamlined on Apr.1,2003 by KD
void TRTParm::set_def( int q)
{
    ErrorIf( rpp!=&rp[q], GetName(), "E05RTrem: Dynamic memory corruption in RTParm data structure.");
    TProfil *aPa= dynamic_cast<TProfil *>(aMod[RT_PARAM].get());

    memcpy( &rp[q].What, aPa->pa.RPpdc, 10 );
    strcpy( rp[q].name,  "T (and P) corrections: g0 function of " );   // Fixed for debugging
    strcpy( rp[q].comment, "Please, change the script and/or remake, if necessary" );
    strcpy( rp[q].xNames,  "C  / bar" );   // Fixed for debugging
    strcpy( rp[q].yNames, "kJ/mol" );

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
        rp[q].NV = (rp[q].NP * rp[q].NT);
        break;
    case 3:
        rp[q].NV = max( rp[q].NP, rp[q].NT );
        break;
    }
    rp[q].Pi[0] = aPa->pa.Pi[0];
    rp[q].Pi[1] = aPa->pa.Pi[1];
    rp[q].Pi[2] = aPa->pa.Pi[2];
    rp[q].Ti[0] = aPa->pa.Ti[0];
    rp[q].Ti[1] = aPa->pa.Ti[1];
    rp[q].Ti[2] = aPa->pa.Ti[2];
    memset( &rpp->Nsd, 0, 12*sizeof(short));
    rpp->lNam = nullptr;
    rpp->lNamE = nullptr;
    rpp->expr = nullptr;
    rpp->exprE = nullptr;
    rpp->T    = nullptr;
    rpp->P    = nullptr;
    rpp->F    = nullptr;
    rpp->TE    = nullptr;
    rpp->PE    = nullptr;
    rpp->FE    = nullptr;
    rpp->trpn  = nullptr;
    plot  = nullptr;
    rpp->sdref = nullptr;
    rpp->sdval = nullptr;
// Added to set a default script 01.04.2003 KD
//    rpp->Pplot = S_ON;
    rpp->dimXY[1] = 1;
    rpp->expr = static_cast<char *>(aObj[ o_rtexpr ]->Alloc(1, 2048, S_));
    strcpy( static_cast<char *>(aObj[o_rtexpr]->GetPtr()),  " yF[jTP][0] =: twG/1000;\n  " );
    rpp->lNam = static_cast<char (*)[MAXGRNAME]>(aObj[ o_rtlnam ]->Alloc( 1, rpp->dimXY[1], MAXGRNAME));
    strcpy( rpp->lNam[0], "g0 ");
}

// return true if recalc is necessary
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
        TDComp* aDC=   dynamic_cast<TDComp *>(aMod[RT_DCOMP].get());
        aDC->ods_link(0);
        aDC->TCModule::TryRecInp( pkey, tr, 0 );
    }
    else
    {
        TReacDC* aRC= dynamic_cast<TReacDC *>(aMod[RT_REACDC].get());
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
    
    pVisor->Update();
    return iRet;
}


// Input necessary data and link objects
void TRTParm::RecInput( const char *key )
{
    TCModule::RecInput( key );
}

/* opens window with 'Remake record' parameters
*/
void
TRTParm::MakeQuery()
{
//    pImp->MakeQuery();
    const char * p_key;
    char flgs[10];
    int size[7];
    double val[6];

    p_key  = db->PackKey();
    memcpy( flgs, &rpp->What, 10);
    size[0] = rpp->NP;
    size[1] = rpp->NT;
    size[2] = rpp->Mode;
    size[3] = rpp->Nsd;
    size[4] = rpp->dimEF[0];
    size[5] = rpp->dimEF[1];
    size[6] = rpp->dimXY[1];

    val[0] = rpp->Ti[0];
    val[1] = rpp->Ti[1];
    val[2] = rpp->Ti[2];
    val[3] = rpp->Pi[0];
    val[4] = rpp->Pi[1];
    val[5] = rpp->Pi[2];

    // for scripts
    TCStringArray namesLines;
    std::string calcScript;
    if( rpp->expr )
     calcScript = rpp->expr;
    std::string xName = rpp->xNames;
    std::string yName = rpp->yNames;

    if( !vfRTparmSet( window(), p_key, flgs, size, val, calcScript, xName, yName, namesLines ))
         Error( p_key, "E06RTrem: RTParm record configuration cancelled by the user!" );
     //  return;   // cancel

    memcpy( &rpp->What, flgs, 10);
    rpp->NP = size[0];
    rpp->NT = size[1];
    rpp->Mode = size[2];
    rpp->Nsd = size[3];
    rpp->dimEF[0] = size[4];
    rpp->dimEF[1] = size[5];
    rpp->dimXY[1] = size[6];

    rpp->Ti[0] = val[0];
    rpp->Ti[1] = val[1];
    rpp->Ti[2] = val[2];
    rpp->Pi[0] = val[3];
    rpp->Pi[1] = val[4];
    rpp->Pi[2] = val[5];

    // from scripts
    if( !rpp->expr )
       rpp->expr = static_cast<char *>(aObj[ o_rtexpr ]->Alloc(1, 2048, S_));
    aObj[o_rtexpr]->SetString( calcScript.c_str(),0,0);
    if(namesLines.size() > 0)
     {
        rpp->lNam = static_cast<char (*)[MAXGRNAME]>(aObj[ o_rtlnam ]->Alloc( 1,
                         rpp->dimXY[1], MAXGRNAME));
        for(size_t ii=0; ii< min<size_t>( namesLines.size(),rpp->dimXY[1]); ii++)
        {
          strncpy(  rpp->lNam[ii], namesLines[ii].c_str(), MAXGRNAME );
        }
        strncpy( rpp->xNames, xName.c_str(), MAXAXISNAME  );
        strncpy( rpp->yNames, yName.c_str(), MAXAXISNAME );
     }
}

//Rebuild record structure before calculation
int
TRTParm::RecBuild( const char *key, int mode  )
{

AGAIN_SETUP:
    int ret = TCModule::RecBuild( key, mode );
    if( ret == VF_CANCEL )
        return ret;
    if( ret == VF3_1 )
    {
        int ilx, len;
        strncat( rpp->name, db->FldKey(2), db->FldLen(2));
        len = strlen( rpp->name );
        for( ilx=len-1; ilx>0; ilx-- )
        { // remove tail spaces
          if(rpp->name[ilx] == ' ')
            continue;
          rpp->name[ilx+1] = 0;
          break;
        }
    }
    rpp->What = *(key + MAXSYMB+MAXDRGROUP+MAXDCNAME+MAXSYMB); // Foolproof !

    switch( rpp->Mode )
    {
    default:
    case 0: // the user must enter all P and T values in xP and xT vectors
        rpp->NV = max( rpp->NP, rpp->NT );
        break;
    case 1: // increments in cycle on P nested into cycle on T
    case 2: // increments in cycle on T nested into cycle on P (default)
        rpp->NV = (rpp->NP * rpp->NT);
        break;
    case 3: // parallel increments of T and P in one cycle
        rpp->NV = max( rpp->NP, rpp->NT );
        break;
    }
    if( rpp->NV < 1 || rpp->NV > 4192 )
    {
        if(vfQuestion(window(), GetName(),
                 "W07RTrem: Invalid mode of calculation of P and T arrays,\n"
                 "or wrong number of TP points! Change?"))
            goto AGAIN_SETUP;
        else   Error( GetName(), "E08RTrem: Invalid mode of calculation - bailing out...");
    }

    if( rpp->Pplot == S_OFF )
         if( rpp->dimEF[0]>0 && rpp->dimEF[1]> 0)
             rpp->Pplot = S_ON;

    if( (rpp->dimEF[0]<=0 || rpp->dimEF[1]<=0) && rpp->Pplot != S_OFF )
    {
        vfMessage(window(), GetName(),
        "W07RTrem: Wrong dimensions set for the empirical data array ytE!");
        goto AGAIN_SETUP;
    }

    if(  rpp->dimXY[1] < 1 || rpp->dimXY[1] > 20 )
    {
        vfMessage(window(), GetName(),
        "W08RTrem: Wrong dimensions set for the data array yF!");
        goto AGAIN_SETUP;
    }
//    rpp->What = *(key + MAXSYMB+MAXDRGROUP+MAXDCNAME+MAXSYMB);
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
        rpn[0].GetEquat( static_cast<char *>(aObj[o_rtexpr]->GetPtr()) );
    }
    catch( TError& xcpt )
    {
        char *erscan = static_cast<char *>(aObj[o_rtexpr]->GetPtr());
        vfMessage(window(), xcpt.title, xcpt.mess);
        CheckEqText(  erscan,
              "E95MSTran: Error in translation of RTParm script: " );
        Error(  GetName() , xcpt.mess.c_str() );
    }
}

// Translate and calculate equations of empirical data
void TRTParm::exprE_calc()
{
    try
    {
     if( !aObj[o_rtexpre]->IsNull())
        if(  (!aObj[o_rtexpre]->IsEmpty(0,0)) &&
             (strncmp( static_cast<char *>(aObj[o_rtexpre]->GetPtr()), "---", 3) != 0))
             rpn[1].GetEquat( static_cast<char *>(aObj[o_rtexpre]->GetPtr()) );
    }
    catch( TError& xcpt )
    {
        char *erscan = static_cast<char *>(aObj[o_rtexpre]->GetPtr());
        vfMessage(window(), xcpt.title, xcpt.mess);
        CheckEqText(  erscan,
  "E96MSTran: Error in RTParm script for empirical data: " );
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
  double P_old, P, TC;
  
  //float K_to_C = 0.;
  //if(rpp->Ptun == PVT_KELVIN )
  //	  K_to_C = -C_to_K;
  
    /* insert values to arrays  rp->T, rp->P, rp->RT; */
   switch( rpp->Mode )
    {
    default:
    case 0: // the user must enter all P and T values in xP and xT vectors
        break;
    case 1: // increments in cycle on P nested into cycle on T
        for( ij=0, i=0; i<rpp->NT; i++ )
        {
            rpp->T[ij] = (rpp->Ti[0]) + rpp->Ti[2] * i;
            for( j=0; j<rpp->NP; j++ )
            {
                rpp->P[ij] = rpp->Pi[0] + rpp->Pi[2] * j;
                if( j ) rpp->T[ij] = rpp->T[ij-1];
                ij++;
            }
        }
        break;
    case 2: // increments in cycle on T nested into cycle on P (default)
        for( ij=0, i=0; i<rpp->NP; i++ )
        {
            rpp->P[ij] = rpp->Pi[0] + rpp->Pi[2] * i;
            for( j=0; j<rpp->NT; j++ )
            {
                rpp->T[ij] = (rpp->Ti[0]) + rpp->Ti[2] * j;
                if( j ) rpp->P[ij] = rpp->P[ij-1];
                ij++;
            }
        }
        break;
    case 3: // parallel increments of T and P in one cycle
        for( i=0; i<rpp->NV; i++ )
        {
            rpp->P[i] = rpp->Pi[0] + rpp->Pi[2] * i;
            rpp->T[i] = (rpp->Ti[0]) + rpp->Ti[2] * i;
        }
        break;
    }
    pVisor->Update();
    expr_analyze(); // Translate equations before calc
    // Translate and calculate equations of empirical data
    if( rpp->Pplot != S_OFF )
        exprE_calc();

//    aSpc.on_sat_curve = false; // 01.06.2016

    /* calc t/d  properties */
    for( j=0; j<rpp->NV; j++ )
    {
        rpp->jTP = j;
        pVisor->Message( window(), GetName(),
           "Sampling the xT, xP, yF data. "
                 "Please, wait...", j, rpp->NV );
        // rpp->RT[j] = R_CONSTANT * ( rpp->T[j] + C_to_K );
         aW.ods_link( 0 );
        /* clear new work structure */
        aW.set_zero( 0 );
        switch( rpp->Ppun)
        {
        case 'b':
        default: P =rpp->P[j];
                 break;
        case 'B': P =rpp->P[j]*1000;
                 break;
        case 'p': P =rpp->P[j]/1e5;
                 break;
        case 'P': P =rpp->P[j]*10;
                 break;
        case 'A': P =rpp->P[j]/1.013;
                 break;
        }
        aW.twp->P = P_old = P /*rpp->P[j]*/;
        TC = rpp->T[j];
        if( rpp->Ptun == 'K' )
          TC = TC - C_to_K;
        aW.twp->TC = TC/*rpp->T[j]*/;
        aW.twp->T = aW.twp->TC + C_to_K;
        aW.twp->unE = rpp->PunE;
        aW.twp->unV = rpp->PunV;
        /* calc t/d properties of component */
        switch( rpp->What )
        {
        case SRC_DCOMP:
            {
                TDComp* aDC=dynamic_cast<TDComp *>(aMod[RT_DCOMP].get());
                aW.twp->TCst = aDC->dcp->TCst;
                aW.twp->Tst = aW.twp->TCst + C_to_K;
                aW.twp->Pst = aDC->dcp->Pst;
                aSpc.on_sat_curve = false; // 01.06.2016
                aDC->DCthermo( 0, 0 );
            }
            break;
        case SRC_REACDC:
            {
                TReacDC* aRDC=dynamic_cast<TReacDC *>(aMod[RT_REACDC].get());
                aW.twp->TCst = aRDC->rcp->TCst;
                aW.twp->Tst = aW.twp->TCst + C_to_K;
                aW.twp->Pst = aRDC->rcp->Pst;
                aRDC->RCthermo( 0, 0 );
                aRDC->ods_link(0);
               aW.ods_link(0);
            }
            break;
        default:
            break;
        }
        /* Set results to arrays */
//        aW.ods_link(0);
        if( P_old < 1.00001e-5 && rpp->P[j] < 1.00001e-5 )
        {  // Set calculated pressure if it was zero in RTparam record 
            rpp->P[j] = aW.twp->P + 3e-5;  //Adding 3 Pa to make sure that
                          // the next calculations are in liquid H2O field!
        }
        // calculate equations of  data
        rpn[0].CalcEquat();
        aMod[nRT]->ModUpdate("Calculation of RTparm arrays");
    }
    SetString("RTPARM arrays calculated OK");
    TCModule::RecCalc(key);
    pVisor->CloseMessage();
    check_input( key );
    pVisor->Update();
}

const char* TRTParm::GetHtml()
{
   return GM_RTPARM_HTML;
}


void
TRTParm::RecordPlot( const char* /*key*/ )
{
   std::vector<TPlot> plt;

   if( rpp->Pabs == 'P')
     plt.push_back( TPlot( o_rpxp, o_rpyf ));
   else
     plt.push_back( TPlot( o_rpxt, o_rpyf ));

   int  nLn = plt[ 0 ].getLinesNumber();
   if( rpp->Pplot != S_OFF )
    {
       if( rpp->Pabs == 'P')
         plt.push_back( TPlot( o_rpxpe, o_rpyte ));
       else
         plt.push_back( TPlot( o_rpxte, o_rpyte ));
        nLn += plt[1].getLinesNumber();
    }
    if( plot )
    {
        int oldN = aObj[o_rppline]->GetN();

        plot = static_cast<TPlotLine * >(aObj[ o_rppline ]->Alloc( nLn, sizeof(TPlotLine) ));
        for(int ii=0; ii<nLn; ii++ )
        {
            if( ii >= oldN )
            {
                if(ii < rpp->dimXY[1] )
                {
                    TPlotLine defpl(ii, nLn, "",6,0,2);
                    plot[ii] = defpl;
                }
                else
                {
                    TPlotLine defpl(ii, nLn, "",7,7,0);
                    plot[ii] = defpl;
                }
            }
            if(ii < rpp->dimXY[1] )
                 plot[ii].setName( rpp->lNam[ii]);
                // strncpy( plot[ii].name, rpp->lNam[ii], MAXGRNAME-1 );
            else
                plot[ii].setName( rpp->lNamE[ii-rpp->dimXY[1]]);
                //strncpy( plot[ii].name, rpp->lNamE[ii-rpp->dimXY[1]], MAXGRNAME-1 );
           // plot[ii].name[MAXGRNAME-1] = '\0';
        }
        gd_gr = updateGraphWindow( gd_gr, this, plt, rpp->name,
                                     rpp->size[0], rpp->size[1], plot,
                                     rpp->axisType, rpp->xNames, rpp->yNames);
    }
    else
    {
      TCStringArray lnames;
      int ii;
      for( ii=0; ii<rpp->dimXY[1]; ii++ )
          lnames.push_back( std::string(rpp->lNam[ii], 0, MAXGRNAME ));
      for( ii=0; ii<rpp->dimEF[1]; ii++ )
          lnames.push_back( std::string( rpp->lNamE[ii], 0, MAXGRNAME ));
      gd_gr = updateGraphWindow( gd_gr, this, plt, rpp->name,
          rpp->xNames, rpp->yNames, lnames );
    }
}

bool TRTParm::SaveChartData( jsonui::ChartData* gr )
{

    // We can only have one Plot dialog (modal one) so condition should be omitted!!
    if( !gd_gr )
        return false;

    strncpy(  rpp->name, gr->title.c_str(), MAXGSNAME );
    rpp->axisType[0] = static_cast<short>(gr->axisTypeX);
    rpp->axisType[5] = static_cast<short>(gr->axisTypeY);
    rpp->axisType[4] = static_cast<short>(gr->getGraphType());
    rpp->axisType[1] = static_cast<short>(gr->b_color[0]);
    rpp->axisType[2] = static_cast<short>(gr->b_color[1]);
    rpp->axisType[3] = static_cast<short>(gr->b_color[2]);
    strncpy( rpp->xNames, gr->xName.c_str(), MAXAXISNAME);
    strncpy( rpp->yNames, gr->yName.c_str(), MAXAXISNAME);
    for(int ii=0; ii<4; ii++ )
    {
        rpp->size[0][ii] =  static_cast<float>(gr->region[ii]);
        rpp->size[1][ii] =  static_cast<float>(gr->part[ii]);
    }

    plot = static_cast<TPlotLine *>(aObj[ o_rppline]->Alloc( gr->getSeriesNumber(), sizeof(TPlotLine)));
    for(int ii=0; ii<gr->getSeriesNumber(); ii++ )
    {
        plot[ii] = convertor( gr->lineData( ii ) );
        //  lNam0 and lNamE back
        if(ii < rpp->dimXY[1] )
            strncpy(  rpp->lNam[ii], plot[ii].getName().c_str(), MAXGRNAME );
        else
            strncpy(  rpp->lNamE[ii-rpp->dimXY[1]], plot[ii].getName().c_str(), MAXGRNAME );
    }
   //if( gr->graphType == ISOLINES )
   //    gr->getColorList();

    pVisor->Update();
    contentsChanged = true;

    return true;
}

//------------------- End of m_rtparm.cpp ---------------------------

