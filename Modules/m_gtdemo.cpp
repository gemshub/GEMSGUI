//-------------------------------------------------------------------
// $Id: m_gtdemo.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of TGtDemo class, config and calculation functions
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

#include <cmath>
#include <cstdio>
#include <vector>
#include "m_proces.h"
#include "m_unspace.h"
#include "m_gtdemo.h"
#include "service.h"
#include "visor.h"

TGtDemo* TGtDemo::pm = nullptr;

bool TGtDemo::check_RT( int nrt )
{
    return ( nrt >RT_ICOMP && nrt < static_cast<int>(aMod.size()));
}

TGtDemo::TGtDemo( uint nrt ):
        TCModule( nrt )
{
    nQ = 1;
    aFldKeysHelp.push_back("Name of the modeling project");
    aFldKeysHelp.push_back("Top database chain type for data sampling { PR SY CO PH RE DC IC }");
    aFldKeysHelp.push_back("Name of this GtDemo data sampling task");
    aFldKeysHelp.push_back("Variant number of this GtDemo task");
    aFldKeysHelp.push_back("Record key comment to this GtDemo task");
    setKeyEditField(1);
    gdp=&gd[0];
    gdp->nRT = RT_ICOMP;
    set_def();
    start_title = " Definition of Data Sampling and Plotting Task ";
    gd_gr = nullptr;
}

const std::string& TGtDemo::GetString()
{
    titler = TProfil::pm->projectName();
    titler += " : ";
    titler += TSubModule::GetString();
    return titler;
}

// get key of record
std::string TGtDemo::GetKeyofRecord( const char *oldKey, const char *strTitle,
                                     int keyType )
{
    std::string str;

    if( oldKey == nullptr )
    {
        if(Filter.empty())
            str = ALLKEY;
        else str = Filter;
    }
    else str = oldKey;

    if( keyType==KEY_NEW  )
    {   // Get key of Project
        str = TProfil::pm->projectName();
        str+= ":*:*:*:*:";
    }
    str = TCModule::GetKeyofRecord( str.c_str(), strTitle, keyType );
    if(  str.empty() )
        return str;
    rt[RT_GTDEMO]->SetKey(str.c_str());
    if( keyType != KEY_TEMP )
        keyTest( str.c_str() );
    return str;
}


// test TGtDemo key to calc mode
void TGtDemo::keyTest( const char *key )
{
    if( pVisor->ProfileMode  )
    { // test project key
        std::string prfKey = TProfil::pm->projectName();
        auto k = prfKey.length();
        if( memcmp(key, prfKey.c_str(), k ) ||
                ( key[k] != ':' && key[k] != ' ' && k<rt[RT_PARAM]->FldLen(0) )  )
            Error( key, "E08PErem: Wrong TGtDemo record key (another Modelling Project)!");
    }
}

// link values to objects
void TGtDemo::ods_link( int q)
{
    aObj[ o_gdps]->SetPtr(   &gd[q].PtAEF );   /*a 16*/
    aObj[ o_gdnlrk]->SetPtr( &gd[q].Nlrk );   /*i 4*/
    aObj[ o_gdnwcq]->SetPtr( &gd[q].Nwc );    /*i 2*/
    aObj[ o_gddimxy]->SetPtr( gd[q].dimXY ); /*i 2*/
    aObj[ o_gddimef]->SetPtr( gd[q].dimEF );  /*i 2*/
    aObj[ o_gdname]->SetPtr(  gd[q].name );
    aObj[ o_gdcom]->SetPtr(   gd[q].comment );
    aObj[ o_gdxnames]->SetPtr(  gd[q].xNames );
    aObj[ o_gdynames]->SetPtr(  gd[q].yNames );
    aObj[ o_gdaxis]->SetPtr(   gd[q].axisType );   /* i 6 */
    aObj[ o_gwwkb]->SetPtr(   gd[q].wcrk );
    aObj[ o_gdproc]->SetPtr(   gd[q].prKey );
    aObj[ o_gdsize]->SetPtr(    gd[q].size[0] ); /*f 8*/
    int dimPclnam = gd[q].dimXY[1];
    if(  gd[q].dimX > 1)
        dimPclnam +=  gd[q].dimX;
    aObj[ o_gdlnam]->SetPtr( gd[q].lNam0[0] );
    aObj[ o_gdlnam]->SetDim( 1, dimPclnam );
    aObj[ o_gdlname]->SetPtr( gd[q].lNamE[0] );
    aObj[ o_gdlname]->SetDim( 1, gd[q].dimEF[1] );

    aObj[o_gdexpr]->SetPtr( gd[q].expr );//aObj[o_gdexpr]->SetDim(1,len(gd[q].expr));
    aObj[o_gdexpre]->SetPtr(gd[q].exprE);//aObj[o_gdexpre]->SetDim(1,l(gd[q].exprE));
    aObj[ o_gdrkey]->SetPtr( gd[q].rkey );
    aObj[ o_gdrkey]->SetDim(  gd[q].Nlrk, 1 );
    if( gd[q].dimX <= 0)
        gd[q].dimX = 1;
    aObj[o_gdx0]->SetPtr( gd[q].x0 );
    aObj[ o_gdx0]->SetDim(gd[q].dimXY[0], gd[q].dimX );
    aObj[o_gdy0]->SetPtr( gd[q].y0 );
    aObj[ o_gdy0]->SetDim(gd[q].dimXY[0], gd[q].dimXY[1] );
    aObj[ o_gdxe]->SetPtr( gd[q].xE );
    aObj[o_gdxe]->SetDim( gd[q].dimEF[0],1 );
    aObj[ o_gdye]->SetPtr( gd[q].yE );
    aObj[ o_gdye]->SetDim(gd[q].dimEF[0], gd[q].dimEF[1] );
    aObj[ o_gdwc]->SetPtr( gd[q].wc );
    aObj[ o_gdwc]->SetDim( gd[q].Nwc, 1 );
    aObj[ o_gdqp]->SetPtr( gd[q].qp );
    aObj[ o_gdqp]->SetDim( gd[q].Nlrk,  gd[q].Nqp);
    aObj[ o_gdplline]->SetPtr( plot );
    aObj[ o_gdplline]->SetDim( gd[q].dimXY[1]+gd[q].dimEF[1],  sizeof(TPlotLine));

    aObj[ o_gwetext]->SetPtr( gd[q].etext ); // aObj[ o_gwetext]->SetDim(1, 0 );
    aObj[ o_gwprtab]->SetPtr( gd[q].prtab ); // aObj[ o_gwprtab]->SetDim(1, 0);
    aObj[ o_gwtprn]->SetPtr(  gd[q].tprn  ); // aObj[ o_gwtprn]->SetDim( 1,l( gd[q].tprn));

    aObj[ o_gwnext]->SetPtr(  &gd[q].Next );
    aObj[ o_gwjr]->SetPtr(    &gd[q].jR );
    aObj[ o_gwi0]->SetPtr(    &gd[q].i0 );
    aObj[ o_gwie]->SetPtr(    &gd[q].iE );
    aObj[ o_gwj0]->SetPtr(    &gd[q].j0 );
    aObj[ o_gwje]->SetPtr(    &gd[q].jE );
    aObj[ o_gwwkey]->SetPtr(  gd[q].Wkb );
    aObj[ o_gdsdref]->SetPtr( gd[q].sdref);
    aObj[ o_gdsdref]->SetDim( gd[q].Nsd, 1 );
    aObj[ o_gdsdval]->SetPtr( gd[q].sdval);
    aObj[ o_gdsdval]->SetDim( gd[q].Nsd, 1 );

    gdp=&gd[q];
}

// set dynamic Objects ptr to values
void TGtDemo::dyn_set(int q)
{
    ErrorIf( gdp!=&gd[q], GetName(), "E06GDrem: Attempt to access corrupted dynamic memory.");
    // memcpy( gdp->symb , rt[nRT].UnpackKey(), GD_RKLEN );

    // Change MAXGRNAME from 7 to 16
    if(aObj[ o_gdlnam ]->GetType() == 7 )
        gdp->lNam0 = static_cast<char (*)[MAXGRNAME]>(aObj[ o_gdlnam ]->Alloc( 1,
                     gdp->dimXY[1], MAXGRNAME));
    else
        gdp->lNam0 = static_cast<char (*)[MAXGRNAME]>(aObj[ o_gdlnam ]->GetPtr());

    if(gdp->PtAEF != S_OFF && aObj[ o_gdlname ]->GetType() == 7 )
        gdp->lNamE = static_cast<char (*)[MAXGRNAME]>(aObj[ o_gdlname ]->Alloc(1,
                     gdp->dimEF[1], MAXGRNAME));
    else
       gdp->lNamE = static_cast<char (*)[MAXGRNAME]>(aObj[ o_gdlname ]->GetPtr());

    gdp->expr = static_cast<char *>(aObj[ o_gdexpr ]->GetPtr());
    gdp->exprE = static_cast<char *>(aObj[ o_gdexpre ]->GetPtr());
    gdp->rkey  = static_cast<char *>(aObj[ o_gdrkey ]->GetPtr());
    gdp->x0    = static_cast<double *>(aObj[ o_gdx0 ]->GetPtr());
    gdp->dimX = aObj[ o_gdx0 ]->GetM();
    gdp->y0    = static_cast<double *>(aObj[ o_gdy0 ]->GetPtr());
    gdp->xE    = static_cast<double *>(aObj[ o_gdxe ]->GetPtr());
    gdp->yE    = static_cast<double *>(aObj[ o_gdye ]->GetPtr());
    gdp->wc    = static_cast<double *>(aObj[ o_gdwc ]->GetPtr());
    gdp->qp    = static_cast<double *>(aObj[ o_gdqp ]->GetPtr());
    gdp->etext = static_cast<char *>(aObj[ o_gwetext ]->GetPtr());
    gdp->prtab = static_cast<char *>(aObj[ o_gwprtab ]->GetPtr());
    gdp->tprn  = static_cast<char *>(aObj[ o_gwtprn ]->GetPtr());
    plot  = static_cast<TPlotLine *>(aObj[ o_gdplline ]->GetPtr());
    gdp->sdref = static_cast<char (*)[V_SD_RKLEN]>(aObj[ o_gdsdref ]->GetPtr());
    gdp->sdval = static_cast<char (*)[V_SD_VALEN]>(aObj[ o_gdsdval ]->GetPtr());
    gdp->rtLen =  rt[ gdp->nRT ]->KeyLen();
}

// free dynamic memory in objects and values
void TGtDemo::dyn_kill(int q)
{
    ErrorIf( gdp!=&gd[q], GetName(), "E05GDrem: Attempt to access corrupted dynamic memory.");
    gdp->lNam0 = static_cast<char (*)[MAXGRNAME]>(aObj[ o_gdlnam ]->Free());
    gdp->lNamE = static_cast<char (*)[MAXGRNAME]>(aObj[ o_gdlname ]->Free());
    gdp->expr = static_cast<char *>(aObj[ o_gdexpr ]->Free());
    gdp->exprE = static_cast<char *>(aObj[ o_gdexpre ]->Free());
    gdp->rkey  = static_cast<char *>(aObj[ o_gdrkey ]->Free());
    gdp->x0    = static_cast<double *>(aObj[ o_gdx0 ]->Free());
    gdp->y0    = static_cast<double *>(aObj[ o_gdy0 ]->Free());
    gdp->xE    = static_cast<double *>(aObj[ o_gdxe ]->Free());
    gdp->yE    = static_cast<double *>(aObj[ o_gdye ]->Free());
    gdp->wc    = static_cast<double *>(aObj[ o_gdwc ]->Free());
    gdp->qp    = static_cast<double *>(aObj[ o_gdqp ]->Free());
    gdp->etext = static_cast<char *>(aObj[ o_gwetext ]->Free());
    gdp->prtab = static_cast<char *>(aObj[ o_gwprtab ]->Free());
    gdp->tprn  = static_cast<char *>(aObj[ o_gwtprn ]->Free());
    plot = static_cast<TPlotLine*>(aObj[ o_gdplline ]->Free());
    gdp->sdref = static_cast<char (*)[V_SD_RKLEN]>(aObj[ o_gdsdref ]->Free());
    gdp->sdval = static_cast<char (*)[V_SD_VALEN]>(aObj[ o_gdsdval ]->Free());

}

// realloc dynamic memory
void TGtDemo::dyn_new(int q)
{
    ErrorIf( gdp!=&gd[q], GetName(), "E04GDrem: Attempt to access corrupted dynamic memory.");

    int dimPclnam = gdp->dimXY[1];
    if(  gdp->dimX > 1)
        dimPclnam +=  gdp->dimX;
    gdp->lNam0 = static_cast<char (*)[MAXGRNAME]>(aObj[ o_gdlnam ]->Alloc( 1,
                 dimPclnam, MAXGRNAME));
    gdp->expr = static_cast<char *>(aObj[ o_gdexpr ]->Alloc(1, 2048, S_));

    gdp->x0    = static_cast<double *>(aObj[ o_gdx0 ]->Alloc(gdp->dimXY[0], gdp->dimX, D_));
    gdp->y0    = static_cast<double *>(aObj[ o_gdy0 ]->Alloc(gdp->dimXY[0], gdp->dimXY[1], D_));

    if( gdp->PtAEF == S_OFF )
    {
        gdp->lNamE = static_cast<char (*)[MAXGRNAME]>(aObj[ o_gdlname ]->Free());
        gdp->exprE = static_cast<char *>(aObj[ o_gdexpre ]->Free());
        gdp->xE    = static_cast<double *>(aObj[ o_gdxe ]->Free());
        gdp->yE    = static_cast<double *>(aObj[ o_gdye ]->Free());
        gdp->dimEF[1] = gdp->dimEF[0] = 0;
    }
    else
    {
        gdp->lNamE = static_cast<char (*)[MAXGRNAME]>(aObj[ o_gdlname ]->Alloc(1,
                     gdp->dimEF[1], MAXGRNAME));
        gdp->exprE = static_cast<char *>(aObj[ o_gdexpre ]->Alloc(1, 2048, S_));
        gdp->xE    = static_cast<double *>(aObj[ o_gdxe ]->Alloc(gdp->dimEF[0], 1, D_));
        gdp->yE    = static_cast<double *>(aObj[ o_gdye ]->Alloc(gdp->dimEF[0],gdp->dimEF[1], D_));
    }
    if( gdp->Nwc > 0 )
        gdp->wc    = static_cast<double *>(aObj[ o_gdwc ]->Alloc(1, gdp->Nwc, D_));
    else  gdp->wc    = static_cast<double *>(aObj[ o_gdwc ]->Free());

    if( gdp->Nqp>0 && gdp->Nlrk>0 )
        gdp->qp    = static_cast<double *>(aObj[ o_gdqp ]->Alloc( gdp->Nlrk, gdp->Nqp, D_ ));
    else    gdp->qp    = static_cast<double *>(aObj[ o_gdqp ]->Free());

    if( gdp->Nsd > 0 )
    {
        gdp->sdref =
            static_cast<char (*)[V_SD_RKLEN]>(aObj[ o_gdsdref]->Alloc( gdp->Nsd, 1, V_SD_RKLEN ));
        gdp->sdval =
            static_cast<char (*)[V_SD_VALEN]>(aObj[ o_gdsdval]->Alloc( gdp->Nsd, 1, V_SD_VALEN ));
    }
    else
    {
        gdp->sdref = static_cast<char (*)[V_SD_RKLEN]>(aObj[ o_gdsdref ]->Free());
        gdp->sdval = static_cast<char (*)[V_SD_VALEN]>(aObj[ o_gdsdval ]->Free());
    }

    gdp->etext = static_cast<char *>(aObj[ o_gwetext ]->Alloc(1, 2048, S_));
    gdp->prtab = static_cast<char *>(aObj[ o_gwprtab ]->Alloc(1, 2048, S_));

}

// set default mode for rt type
void TGtDemo::gd_ps_set()
{
    switch( gdp->nRT )
    {
    case RT_ICOMP:
        memcpy( &gdp->PsIC, "+-----------", 12);
        gdp->PsIPNCalc = '-';
        break;
    case RT_DCOMP:
        memcpy( &gdp->PsIC, "-+----------", 12);
        gdp->PsIPNCalc = '+';
        break;
    case RT_COMPOS:
        memcpy( &gdp->PsIC, "--+---------", 12);
        gdp->PsIPNCalc = '+';
        break;
    case RT_REACDC:
        memcpy( &gdp->PsIC, "---+--------", 12);
        gdp->PsIPNCalc = '+';
        break;
    case RT_RTPARM:
        memcpy( &gdp->PsIC, "---++-------", 12);
        gdp->PsIPNCalc = '+';
        break;
    case RT_PHASE:
        memcpy( &gdp->PsIC, "-----+------", 12);
        gdp->PsIPNCalc = '+';
        break;
    case RT_SYSEQ:
        memcpy( &gdp->PsIC, "------+-----", 12);
        gdp->PsIPNCalc = '-';
        break;
    case RT_PROCES:
        memcpy( &gdp->PsIC, "------++----", 12);
        gdp->PsIPNCalc = '-';
        gdp->nRT = RT_SYSEQ;
        break;
//    case RT_UNSPACE:
//        memcpy( &gdp->PsIC, "------+-+---", 12);
//        gdp->nRT = RT_SYSEQ;
//       break;
//    case RT_DUALTH:memcpy( &gdp->PsIC,"------+--+--", 12); break;
    default:
        Error( GetName(), " E02GDrem: Wrong record type selected for sampling...");
    }
}

int TGtDemo::rt_flags_get()
{
    int nRT1 = RT_ICOMP;
    // nRT1 from flags
    if( gdp->PsTR != S_OFF )
        nRT1 = RT_GEM2MT;
    else if( gdp->PsUT != S_OFF )
        nRT1 = RT_DUALTH;
    else if( gdp->PsPB != S_OFF )
        nRT1 = RT_UNSPACE;
    else if( gdp->PsPE != S_OFF )
        nRT1 = RT_PROCES;
    else if( gdp->PsST != S_OFF )
        nRT1 = RT_SYSEQ;
    else if( gdp->PsPH != S_OFF )
        nRT1 = RT_PHASE;
    else if( gdp->PsRP != S_OFF )
        nRT1 = RT_RTPARM;
    else if( gdp->PsRE != S_OFF )
        nRT1 = RT_REACDC;
    else if( gdp->PsBC != S_OFF )
        nRT1 = RT_COMPOS;
    else  if( gdp->PsDC != S_OFF )
        nRT1 = RT_DCOMP;

    return nRT1;
}

// opens window with 'Remake record' parameters
void TGtDemo::MakeQuery()
{
    const char * p_key;
    TCStringArray namesLines;
    TCStringArray rkeyList;
    int size[8];
    char flgs[16];

    std::string script;
    if( gdp->expr )
        script = gdp->expr;

    std::string xName = gdp->xNames;
    std::string yName = gdp->yNames;

    p_key  = db->PackKey();
    size[0] = rt_flags_get();//gdp->nRT;
    size[1] = gdp->Nsd;
    size[2] = gdp->Nwc;
    size[3] = gdp->Nqp;
    size[4] = gdp->dimEF[0];
    size[5] = gdp->dimEF[1];
    size[6] = gdp->dimXY[1];
    size[7] = gdp->dimX;

    std::string prkey = char_array_to_string( gdp->prKey, MAXRKEYLEN);
    if( prkey.empty() || prkey == "`")
        prkey = "*";

    //short rtlen = rt[gdp->nRT]->KeyLen();
    for(int i=0; i<gdp->Nlrk; i++ )
        rkeyList.push_back(char_array_to_string( gdp->rkey+i*gdp->rtLen, gdp->rtLen));

    memcpy( flgs, &gdp->PtAEF, 16);
    if( !vfGtDemoSet( window(), p_key, flgs, size,  prkey, script, namesLines, xName, yName, rkeyList ))
        Error( p_key, "GtDemo record configuration cancelled by the user!" );

    memcpy( &gdp->PtAEF, flgs, 16);
    gdp->nRT = static_cast<short>(size[0]);
    gdp->Nsd = static_cast<short>(size[1]);
    gdp->Nwc = static_cast<short>(size[2]);
    gdp->Nqp = static_cast<short>(size[3]);
    gdp->dimEF[0] = static_cast<short>(size[4]);
    gdp->dimEF[1] = static_cast<short>(size[5]);
    gdp->dimXY[1] = static_cast<short>(size[6]);
    gdp->dimX = static_cast<short>(size[7]);
    // set up process key
    if( gdp->nRT <= RT_SYSEQ )
        prkey = "*";
    strncpy( gdp->prKey, prkey.c_str(), MAXRKEYLEN );

    if( !gdp->expr )
        gdp->expr = static_cast<char *>(aObj[ o_gdexpr ]->Alloc(1, 2048, S_));
    aObj[o_gdexpr]->SetString( script.c_str(),0,0 );

    if(namesLines.size() > 0)
    {
        int dimPclnam = gdp->dimXY[1];
        int ndxy = 0;
        if(  gdp->dimX > 1)
        {      
            dimPclnam +=  gdp->dimX;
            ndxy =gdp->dimX;
        }
        gdp->lNam0 = static_cast<char (*)[MAXGRNAME]>(aObj[ o_gdlnam ]->Alloc( 1,
                     dimPclnam, MAXGRNAME));
        for(size_t ii=0; ii< min<size_t>( namesLines.size(),gdp->dimXY[1]); ii++)
        {
            strncpy( gdp->lNam0[ii+ndxy], namesLines[ii].c_str(), MAXGRNAME );
        }
        strncpy(gdp->xNames, xName.c_str(), MAXAXISNAME );
        strncpy(gdp->yNames, yName.c_str(), MAXAXISNAME );
    }

    // setup flags
    gd_ps_set();
    if( gdp->dimEF[0] >0 && gdp->dimEF[1] >0)
        gdp->PtAEF = S_ON;
    else
        gdp->PtAEF = S_OFF;

    if( gdp->nRT == RT_PROCES  ||  gdp->nRT == RT_UNSPACE )
        gdp->nRT = RT_SYSEQ;

    gdp->rtLen =  rt[ gdp->nRT ]->KeyLen();
    gdp->Nlrk = rkeyList.size();
    gdp->rkey = static_cast<char *>(aObj[ o_gdrkey ]->Alloc( gdp->Nlrk, 1, gdp->rtLen ));
    // make list of record
    for(int i=0; i<gdp->Nlrk; i++ )
        memcpy( gdp->rkey+i*gdp->rtLen, rkeyList[i].c_str(), gdp->rtLen );
    gdp->dimXY[0] = gdp->Nlrk;
    if(  gdp->dimXY[1] <= 0 )
        gdp->dimXY[1] = 2;
}

//set default information
void TGtDemo::set_def( int q)
{
    short nR = gdp->nRT;

    ErrorIf( gdp!=&gd[q], GetName(), "E03GDrem: Attempt to access corrupted dynamic memory.");
    memset( gd, 0, sizeof( GTDEMO ));
    if( check_RT( nR ) == true )
        gdp->nRT = nR;
    else gdp->nRT = RT_ICOMP;
    strcpy( gdp->wcrk, "*");
    strcpy( gdp->prKey, "*");
    gdp->PtAEF = '-';
    gdp->Nsd = 0;
    gdp->dimX = 1;
    gdp->dimXY[1] = 1;
    gdp->dimXY[0] = 0;
    memcpy( gdp->xNames, TProfil::pm->pa.GDpcc[0], MAXAXISNAME );
    memcpy( gdp->yNames, TProfil::pm->pa.GDpcc[1], MAXAXISNAME );

    gd_ps_set();
    gdp->PsSys = '-';
    gdp->PsRes4 = '-';
    gdp->lNam0 = nullptr;
    gdp->lNamE = nullptr;
    gdp->expr  = nullptr;
    gdp->exprE = nullptr;
    gdp->rkey  = nullptr;
    gdp->x0    = nullptr;
    gdp->y0    = nullptr;
    gdp->xE    = nullptr;
    gdp->yE    = nullptr;
    gdp->wc    = nullptr;
    gdp->qp    = nullptr;
    gdp->etext = nullptr;
    gdp->prtab = nullptr;
    gdp->tprn  = nullptr;
    plot       = nullptr;
    gdp->sdref = nullptr;
    gdp->sdval = nullptr;

    // Statistic
    gst.iopt = nullptr;
    gst.opt  = nullptr;
    gst.G    = nullptr;
    gst.b    = nullptr;
}


// Inputs necessary data and links objects
void TGtDemo::RecInput( const char *key )
{
    TCModule::RecInput( key );
}

//Rebild record structure before calc
int TGtDemo::RecBuild( const char *key, int mode  )
{
    TProfil *aPa=TProfil::pm;
    char tbuf[100];
    int i;

    if( gst.iopt )
        delete[] gst.iopt;
    gst.iopt = nullptr;

    int ret = TCModule::RecBuild( key, mode );
    if( !pVisor->ProfileMode  && gdp->nRT >= RT_SYSEQ )
        Error( GetName(), "E02GDexec: Please, do it in the Equilibria Calculation mode" );

    if( ret == VF_CANCEL )
        return ret;
    dyn_new();

    if( gdp->PtAEF != S_OFF && gdp->exprE && !*gdp->exprE )
    {
        strncpy( gdp->exprE, "next=:next;", 20 );
        for( i=0; i<gdp->dimEF[1]; i++ )
        {
            sprintf( tbuf, "%s%d", aPa->pa.GDpsc, i+1 );
            if( !*gdp->lNamE[i] || *gdp->lNamE[i] == ' ' )
                strncpy( gdp->lNamE[i], tbuf,MAXGRNAME );
        }
    }
    int ndxy = 0;
    if(  gdp->dimX > 1)
        ndxy =gdp->dimX;
    for(int j=0; j< gdp->dimXY[1]; j++ )
    {
        sprintf( tbuf, "%s%d", aPa->pa.GDpsc, j+1 );
        if( !*gdp->lNam0[j+ndxy]|| *gdp->lNam0[j+ndxy] == ' ' )
            strncpy( gdp->lNam0[j+ndxy], tbuf, MAXGRNAME );
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

        if( pVisor->ProfileMode  )
        {
            mupL = TRMults::sm->GetMU()->L;
            pmpL = TMulti::sm->GetPM()->L;
        }
        PRof->ET_translate( o_gwetext, o_gdexpr, 0, mupL, 0, pmpL );
        rpn[0].GetEquat( static_cast<char *>(aObj[o_gwetext]->GetPtr()) );
    }
    catch( TError& xcpt )
    {
        char *erscan = static_cast<char *>(aObj[o_gdexpr]->GetPtr());
        vfMessage(window(), xcpt.title, xcpt.mess);
        /*bool   iRet = */
        CheckEqText(  erscan,
               "E91MSTran: Error in analyzing the GtDemo sampling script: " );
        /*  if( iRet )
               goto AGAIN;  */
        Error(  GetName() , xcpt.mess );
    }
}

// Translate and calculate equations of empirical data
void TGtDemo::gd_EF_calc()
{
    try
    {
        rpn[1].GetEquat( static_cast<char *>(aObj[o_gdexpre]->GetPtr()) );
    }
    catch( TError& xcpt )
    {
        char *erscan = static_cast<char *>(aObj[o_gdexpre]->GetPtr());
        vfMessage(window(), xcpt.title, xcpt.mess);
        /*bool   iRet = */
        CheckEqText(  erscan,
    "E92MSTran: Error in analyzing the GtDemo script for empirical data: " );
        Error(  GetName() , xcpt.mess );
    }
    // calc equations
    for( gdp->iE = 0; gdp->iE< gdp->dimEF[0]; gdp->iE++ )
        rpn[1].CalcEquat();
}

// read record number nI from curent nRT
void TGtDemo::gd_rec_read( int nI )
{

    ErrorIf( nI < 0 || nI > gdp->Nlrk, GetName(), "E03GDexec: Invalid database record handle" );

    memset( gdp->Wkb, 0, MAXRKEYLEN );
    strncpy( gdp->Wkb, gdp->rkey+nI*gdp->rtLen, gdp->rtLen );

    if( gdp->nRT == RT_SYSEQ && pVisor->ProfileMode )
    {
        TProfil* PRof = TProfil::pm;
        PRof->loadSystat( gdp->Wkb );

        if( gdp->PsSys ==  S_ON )
        {
            double dTime=0.; int kTimeStep =0; double kTime=0.;
            PRof->CalcEqstat( dTime, kTimeStep, kTime );
        }

        if( gdp->PsPB != S_OFF && *gdp->prKey) // Probe mode
        {
            if( !nI )
            {
                if( gst.iopt )
                    delete[] gst.iopt;
                gst.iopt = new short[gdp->Nlrk*(TRMults::sm->GetMU()->Fi+1)];
                memset( gst.iopt, 0, gdp->Nlrk*(TRMults::sm->GetMU()->Fi+1)*sizeof(short) );
                gst.nI = 0;
            }
            if(  gst.iopt ) // get list of resalt phases
            {
                for(short k=0; k<TMulti::sm->GetPM()->FI; k++)
                    if( TMulti::sm->GetPM()->XF[k] > TMulti::sm->GetPM()->DSM )
                        gst.iopt[ gst.nI++ ] = TMulti::sm->GetPM()->muk[k];
                gst.iopt[ gst.nI++ ] = -1;
            }
        }
    }
    else
    {
        auto pmod = dynamic_cast<TCModule *>(aMod[gdp->nRT].get());
        if( pmod )
        {
            pmod->RecInput( gdp->Wkb );
            pmod->check_input( gdp->Wkb );
        }
    }
}

//Recalculate the GtDemo record
void TGtDemo::RecCalc( const char *key )
{
    if( !pVisor->ProfileMode  && gdp->nRT == RT_SYSEQ )
        Error( GetName(), "E02GDexec: Please, do it in the Equilibria Calculation mode" );
    TCModule::RecCalc(key);

    strcpy( gdp->SYS_key, "*" );
    if( gdp->PsPE != S_OFF && *gdp->prKey)   /* read process record */
    {
        TProcess::pm->RecInput( gdp->prKey );
        if( TProcess::pm->NoSave())
           Error( key, "E04GDexec: The x0, y0 arrays can only be updated after"
                       "the Process simulator re-calculation when saving systems");
    }
    if( gdp->PsPB != S_OFF && *gdp->prKey)    /* read probe record */
        TUnSpace::pm->RecInput( gdp->prKey );

    for( gdp->jR = 0; gdp->jR< gdp->Nlrk; gdp->jR++ )
    {
        pVisor->Message( window(), GetName(),
                 "Sampling the results into GtDemo x0, y0 arrays. "
                 "Please, wait...", gdp->jR, gdp->Nlrk);
        gd_rec_read( gdp->jR );
        if( strcmp( gdp->SYS_key, rt[RT_SYSEQ]->UnpackKey() ))
        {  /* retranslate the math script text */
            gd_text_analyze();
            strncpy( gdp->SYS_key, rt[RT_SYSEQ]->UnpackKey(), EQ_RKLEN );
        }
        rpn[0].CalcEquat( gdp->PsIPNCalc == S_ON );
        aMod[RT_GTDEMO]->ModUpdate("GtDemo data sampling in progress...");
        // Stop Process from Andy Sveta
    }
    /* calc empirical data */
    if( gdp->PtAEF != S_OFF && gdp->exprE && *gdp->expr )
        gd_EF_calc();
/*    // calc statistic
    if( gdp->PsPB != S_OFF && gst.iopt && gst.nI )
    {
        TUnSpace* PRob = TUnSpace::pm;
        PRob->paragen( gdp->Nlrk, gst );
        PRob->pb_matr( gdp->Nlrk, gst );
        PRob->RecSave( gdp->prKey );
    }
*/    pVisor->CloseMessage();
    // set graph and demo resalts gtdemo_demo( key );
}


void TGtDemo::RecordPlot( const char* /*key*/ )
{

    if(  gdp->PsRes4 == S_ON)
    {
        gdp->PsRes4 = S_OFF;
        contentsChanged = true;
        return;
    }

    std::vector<TPlot> plt;

    plt.push_back( TPlot(o_gdx0, o_gdy0 ));
    int  nLn = plt[ 0 ].getLinesNumber();
    if( gdp->PtAEF != S_OFF )
    {
        plt.push_back( TPlot(o_gdxe, o_gdye ));
        nLn += plt[1].getLinesNumber();
    }
    int ndxy = 0;
    if(  gdp->dimX > 1)
        ndxy =gdp->dimX;

    if( plot )
    {
        int oldN = aObj[o_gdplline]->GetN();

        plot = static_cast<TPlotLine * >(aObj[ o_gdplline ]->Alloc( nLn, sizeof(TPlotLine) ));
        for(int ii=0; ii<nLn; ii++ )
        {
            if( ii >= oldN )
            {
                if(ii < gdp->dimXY[1] )
                {
                    TPlotLine defpl(ii, nLn, "",13,2,3);
                    plot[ii] = defpl;
                }
                else
                {
                    TPlotLine defpl(ii, nLn, "",15,25,0);
                    plot[ii] = defpl;
                }
            }
            if(ii < gdp->dimXY[1] )
                plot[ii].setName( gdp->lNam0[ii+ndxy]);
            //strncpy( plot[ii].name, gdp->lNam0[ii], MAXGRNAME-1 );
            else
                plot[ii].setName( gdp->lNamE[ii-gdp->dimXY[1]]);
            //strncpy( plot[ii].name, gdp->lNamE[ii-gdp->dimXY[1]], MAXGRNAME-1 );
            //plot[ii].name[MAXGRNAME-1] = '\0';
        }
        gd_gr = updateGraphWindow( gd_gr, this, plt, gdp->name,
                                   gdp->size[0], gdp->size[1], plot,
                gdp->axisType, gdp->xNames, gdp->yNames);
    }
    else
    {
        std::vector<TPlotLine> def_plt_lines;
        def_plt_lines.push_back(TPlotLine( "",13,2,3));
        def_plt_lines.push_back(TPlotLine( "",15,25,0));
        TCStringArray lnames;
        int ii;
        for( ii=0; ii<gdp->dimXY[1]; ii++ )
            lnames.push_back( char_array_to_string(gdp->lNam0[ii+ndxy], MAXGRNAME ));
        for( ii=0; ii<gdp->dimEF[1]; ii++ )
            lnames.push_back( char_array_to_string( gdp->lNamE[ii], MAXGRNAME ));
        gd_gr = updateGraphWindow( gd_gr, this, plt, gdp->name,
                                   gdp->xNames, gdp->yNames, lnames, def_plt_lines );
    }
}

bool TGtDemo::SaveChartData( jsonui::ChartData* gr )
{
    // We can only have one Plot dialog (modal one) so condition should be omitted!!
    if( !gd_gr )
        return false;

    strncpy(  gdp->name, gr->title.c_str(), MAXGSNAME );
    gdp->axisType[0] = static_cast<short>(gr->axisTypeX);
    gdp->axisType[5] = static_cast<short>(gr->axisTypeY);
    gdp->axisType[4] = static_cast<short>(gr->getGraphType());
    gdp->axisType[1] = static_cast<short>(gr->b_color[0]);
    gdp->axisType[2] = static_cast<short>(gr->b_color[1]);
    gdp->axisType[3] = static_cast<short>(gr->b_color[2]);
    strncpy( gdp->xNames, gr->xName.c_str(), MAXAXISNAME);
    strncpy( gdp->yNames, gr->yName.c_str(), MAXAXISNAME);
    for(int ii=0; ii<4; ii++ )
    {
        gdp->size[0][ii] =  static_cast<float>(gr->region[ii]);
        gdp->size[1][ii] =  static_cast<float>(gr->part[ii]);
    }

    int ndxy = 0;
    if(  gdp->dimX > 1)
        ndxy =gdp->dimX;
    plot = static_cast<TPlotLine *>(aObj[ o_gdplline]->Alloc( gr->getSeriesNumber(), sizeof(TPlotLine)));
    for(int ii=0; ii<gr->getSeriesNumber(); ii++ )
    {
        plot[ii] = convertor( gr->lineData( ii ) );
        //  lNam0 and lNamE back
        if( ii < gdp->dimXY[1] )
            strncpy(  gdp->lNam0[ii+ndxy], plot[ii].getName().c_str(), MAXGRNAME );
        else
            strncpy(  gdp->lNamE[ii-gdp->dimXY[1]], plot[ii].getName().c_str(), MAXGRNAME );
    }
    //if( gr->graphType == ISOLINES )
    //   gr->getColorList();

    pVisor->Update();
    contentsChanged = true;

    return true;
}

const char* TGtDemo::GetHtml()
{
   return GM_GTDEMO_HTML;
}

//-----------------------------------------------------------------------
// Statistic to probe
/*
void
TGtDemo::elst(int N,double *U,double *par)
//   N  -  size; U  -   vector to analyze
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
                                "Criterion Laplasa",           "Criterion Karpova (medium)",
                                "Criterion Karpova (median)", "Criterion Sevidga",
                                "Criterion Gurvitsa (a=0)"  ,  "Criterion Gurvitsa (a=0.5)"};

// Table of Probe resalts
void TGtDemo::probe_stat( const char *key )
{
    int i, j, k, N, M, nPar;
    short nI, nF, QT,*iopt;
    size_t tblen;
    char pbuf[121];
    char  (*lNames)[MAXGRNAME];
    double *y, *U, par[9];
    TUnSpace* Prob = dynamic_cast<TUnSpace *>( aMod[RT_UNSPACE].get());
    TProfil* PRof = dynamic_cast<TProfil *>( aMod[RT_PARAM].get());

    if( gdp->PsPB != S_OFF && *gdp->prKey)    // read probe record 
        TUnSpace::pm->RecInput( gdp->prKey );
    else
        Error( GetName(), "No TUnSpace mode in GtDemo!");

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
        prTab.PutString( (char *)aObj[o_gdexpr]->GetPtr() );
    }
    prTab.PutEnd();
    // Put results to object
    const char *tab= prTab.GetTable();
    tblen = strlen(tab)+1; // Get size of text buf 
    gdp->prtab = (char *)aObj[ o_gwprtab ]->Alloc( 1, tblen, S_ );
    memcpy( gdp->prtab, tab, tblen );

    vfObjToFile(window(), &aObj[o_gwprtab]);
} */
//--------------------- End of m_gtdemo.cpp ---------------------------
