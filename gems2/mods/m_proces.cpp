//-------------------------------------------------------------------
// Id: gems/mods/m_proces.cpp  version 2.0.0   2001
//
// Implementation of TProcess class, config and calculation functions
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
const char *GEMS_PE_HTML = "gm_proces";

#include <math.h>
#include <stdio.h>

#include "m_proces.h"
#include "m_syseq.h"
#include "m_param.h"
#include "v_object.h"
#include "service.h"
#include "visor.h"
#include "visor_w.h"

TProcess* TProcess::pm;

TProcess::TProcess( int nrt ):
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
    aFldKeysHelp.Add("l<18 Identifier of this process simulator definition");
    aFldKeysHelp.Add("l<4  Batch simulator type code, a combination of {T, P, S, G, R}");

    pep=&pe[0];
    set_def();
    gd_gr = 0;
    start_title =
       " Definition of a Process Simulator (batch calculation)";
}

// get key of record
gstring
TProcess::GetKeyofRecord( const char *oldKey, const char *strTitle,
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
        rt[RT_PROCES].SetKey(str.c_str());
        rt[RT_SYSEQ].MakeKey( RT_PROCES, pkey, RT_PROCES, 0, RT_PROCES, 1,
                               RT_PROCES, 2, RT_PROCES, 3, RT_PROCES, 4,
                               RT_PROCES, 5, RT_PROCES, 6, RT_PROCES, 7, K_END);
        str = TSysEq::pm->GetKeyofRecord( pkey,
          "Please, select a parent system for a new Process ", KEY_OLD );
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
    rt[RT_PROCES].SetKey(str.c_str());
    keyTest( str.c_str() );
    return str;
}

// test Process key to calc mode
void TProcess::keyTest( const char *key )
{
    vstr pkey(MAXRKEYLEN+10);

    if( pVisor->ProfileMode == true )
    { // test project key
        gstring prfKey = gstring( rt[RT_PARAM].FldKey(0), 0, rt[RT_PARAM].FldLen(0));
        StripLine(prfKey);
        int k = prfKey.length();
        if( memcmp(key, prfKey.c_str(), k ) ||
                ( key[k] != ':' && key[k] != ' ' && k>=rt[RT_PARAM].FldLen(0) )  )
            Error( key, "Illegal key (another Modelling Project)!");
        rt[RT_SYSEQ].MakeKey( RT_PROCES, pkey, RT_PROCES, 0, RT_PROCES, 1,
                               RT_PROCES, 2, RT_PROCES, 3, RT_PROCES, 4,
                               RT_PROCES, 5, RT_PROCES, 6, RT_PROCES, 7, K_END);
        if( rt[RT_SYSEQ].Find(pkey) <0 )
            Error( key, "Illegal key (no system)!");
    }
}

// link values to objects
void TProcess::ods_link( int q)
{
    //aObj[ o_pemus].SetPtr(   pe[q].mus );
    //aObj[ o_pepot].SetPtr(   pe[q].pot );
    //aObj[ o_pesymb].SetPtr(  pe[q].symb );
    //aObj[ o_petime_].SetPtr( pe[q].time_ );
    //aObj[ o_pepsymb].SetPtr( pe[q].psymb );
    //aObj[ o_pepcode].SetPtr( pe[q].pcode );
    aObj[ o_pepsts ].SetPtr( &pe[q].Istat );/*a 12*/
    aObj[ o_pepvv ].SetPtr(  &pe[q].Pvtm ); /*a 12*/
    aObj[ o_pentxi ].SetPtr( &pe[q].Nxi ); /*i 1*/
    aObj[ o_penpvt ].SetPtr( &pe[q].Nnu );  /*i 4*/
    aObj[ o_penphpe].SetPtr( &pe[q].NpH );  /*i 3*/
    aObj[ o_pemode ].SetPtr( &pe[q].Mode ); /*i 1*/
    aObj[ o_pedim1 ].SetPtr( &pe[q].NR1 );  /*i 5*/
    aObj[ o_pedim2 ].SetPtr( &pe[q].Ntm );  /*i 8*/
    aObj[ o_petmi ].SetPtr(  pe[q].tmi );  /*i 3*/
    aObj[ o_penvi ].SetPtr(  pe[q].NVi );  /*i 3*/
    aObj[ o_pepi  ].SetPtr(  pe[q].Pi );   /*d 3*/
    aObj[ o_peti  ].SetPtr(  pe[q].Ti );   /*d 3*/
    aObj[ o_pevi  ].SetPtr(  pe[q].Vi );   /*d 3*/
    aObj[ o_petaui].SetPtr(  pe[q].Taui ); /*d 3*/
    aObj[ o_pepxii].SetPtr(  pe[q].pXii ); /*d 3*/
    aObj[ o_penui ].SetPtr(  pe[q].Nui );  /*d 3*/
    aObj[ o_pephi ].SetPtr(  pe[q].pHi );  /*d 3*/
    aObj[ o_pepei ].SetPtr(  pe[q].pei );  /*d 3*/
    aObj[ o_pencnt].SetPtr(  &pe[q].NeMax );  /*i 2*/
    aObj[ o_penext].SetPtr(  &pe[q].Loop );
    aObj[ o_pei   ].SetPtr(  &pe[q].i );
    aObj[ o_pej   ].SetPtr(  &pe[q].j );
    aObj[ o_pec_nrk].SetPtr(&pe[q].c_nrk );
    aObj[ o_pec_tm ].SetPtr( &pe[q].c_tm );
    aObj[ o_pec_nv ].SetPtr( &pe[q].c_NV );
    aObj[ o_pec_p  ].SetPtr( &pe[q].c_P );
    aObj[ o_pec_v  ].SetPtr( &pe[q].c_V );
    aObj[ o_pec_t  ].SetPtr( &pe[q].c_T );
    aObj[ o_pec_tc ].SetPtr( &pe[q].c_TC );
    aObj[ o_pec_tau].SetPtr( &pe[q].c_Tau );
    aObj[ o_pec_pxi].SetPtr( &pe[q].c_pXi );
    aObj[ o_pec_xi ].SetPtr( &pe[q].c_Xi );
    aObj[ o_pec_nu ].SetPtr( &pe[q].c_Nu );
    aObj[ o_pec_ph ].SetPtr( &pe[q].c_pH );
    aObj[ o_pec_pe ].SetPtr( &pe[q].c_pe );
    aObj[ o_pec_eh ].SetPtr( &pe[q].c_Eh );
    /* aObj[ o_pestr].SetPtr(  &pe[q].Istat ); */
    aObj[ o_pename ].SetPtr(  pe[q].name );
    aObj[ o_penotes].SetPtr( pe[q].notes );
    aObj[ o_pepspv ].SetPtr( &pe[q].Istat );  /*a 24*/
    aObj[ o_peint1 ].SetPtr( &pe[q].Ntim );   /*i 29*/
    aObj[ o_pefloat].SetPtr( pe[q].Pi );     /*d 24*/

    aObj[ o_petmv].SetPtr( pe[q].tm );
    aObj[ o_petmv ].SetDim( pe[q].Ntm, 1 );
    aObj[ o_penvv].SetPtr( pe[q].nv );
    aObj[ o_penvv ].SetDim( pe[q].dNNV, 1 );
    aObj[ o_pepv].SetPtr(  pe[q].P );
    aObj[ o_pepv  ].SetDim( pe[q].dNP, 1 );
    aObj[ o_pevv].SetPtr( pe[q].V  );
    aObj[ o_pevv  ].SetDim( pe[q].dNV, 1 );
    aObj[ o_petv].SetPtr( pe[q].T );
    aObj[ o_petv  ].SetDim( pe[q].dNT, 1 );
    aObj[ o_petauv].SetPtr( pe[q].Tau );
    aObj[ o_petauv].SetDim( pe[q].NTau, 1 );
    aObj[ o_pepxiv].SetPtr( pe[q].pXi );
    aObj[ o_pepxiv].SetDim( pe[q].NpXi, 1 );
    aObj[ o_penuv].SetPtr( pe[q].Nu );
    aObj[ o_penuv ].SetDim( pe[q].NNu, 1 );
    aObj[ o_pekinv].SetPtr( pe[q].Kin );
    aObj[ o_pekinv].SetDim( pe[q].Nrea, pe[q].Nrp );
    aObj[ o_pemodc].SetPtr( pe[q].Modc );
    aObj[ o_pemodc].SetDim( pe[q].Ntm, pe[q].Nmc );
    aObj[ o_peexpr].SetPtr( pe[q].Expr );
    // aObj[ o_peexpr].SetDim( 1, VIZmsize(pe[q].Expr));
    aObj[ o_pestl].SetPtr( pe[q].stl );
    aObj[ o_pestl].SetDim( pe[q].NR1, 1 );
    aObj[ o_pesdref].SetPtr(pe[q].sdref);
    aObj[ o_pesdref].SetDim( pe[q].Nsd, 1 );
    aObj[ o_pesdval].SetPtr(pe[q].sdval);
    aObj[ o_pesdval].SetDim( pe[q].Nsd, 1 );
    aObj[ o_petprn].SetPtr(  pe[q].tprn );
    //aObj[ o_petprn].SetDim( 1, VIZmsize(pe[q].tprn ));

    aObj[ o_pegdkey].SetPtr( pe[q].gdkey );
    aObj[ o_pestkey].SetPtr( pe[q].stkey );
    aObj[ o_petpkey].SetPtr( pe[q].tpkey );
    aObj[ o_peint2].SetPtr(  &pe[q].NeMax );  /*i 8*/
    aObj[ o_pedouble].SetPtr(&pe[q].c_P );    /*d 11*/

    //graphics
    aObj[ o_pcdimxy].SetPtr( pe[q].dimXY ); /*i 2*/
    aObj[ o_pcdimef].SetPtr( pe[q].dimEF );  /*i 2*/
    aObj[ o_pcxnames].SetPtr(  pe[q].xNames );
    aObj[ o_pcynames].SetPtr(  pe[q].yNames );
    aObj[ o_pcaxis].SetPtr(   pe[q].axisType );   /* i 6 */
    aObj[ o_pcsize].SetPtr(    pe[q].size[0] ); /*f 8*/

    aObj[ o_pclnam].SetPtr( pe[q].lNam[0] );
    aObj[ o_pclnam].SetDim( 1, pe[q].dimXY[1] );
    aObj[ o_pclname].SetPtr( pe[q].lNamE[0] );
    aObj[ o_pclname].SetDim( 1, pe[q].dimEF[1] );
    aObj[o_pcexpr].SetPtr( pe[q].gr_expr );
        //aObj[o_pcexpr].SetDim(1,len(pe[q].gr_expr));
    aObj[o_pcx0].SetPtr( pe[q].x0 );
    aObj[o_pcx0].SetDim(pe[q].dimXY[0], 1 );
    aObj[o_pcy0].SetPtr( pe[q].y0 );
    aObj[o_pcy0].SetDim(pe[q].dimXY[0], pe[q].dimXY[1] );
    aObj[o_pcxe].SetPtr( pe[q].xE );
    aObj[o_pcxe].SetDim( pe[q].dimEF[0],1 );
    aObj[o_pcye].SetPtr( pe[q].yE );
    aObj[o_pcye].SetDim(pe[q].dimEF[0], pe[q].dimEF[1] );
    aObj[ o_pcplline].SetPtr( plot );
    aObj[ o_pcplline].SetDim( pe[q].dimXY[1]+pe[q].dimEF[1], sizeof(TPlotLine));

    pep=&pe[q];
}

// set dynamic Objects ptr to values
void TProcess::dyn_set(int q)
{
    ErrorIf( pep!=&pe[q], GetName(), "Illegal access to pe in dyn_set.");
    // memcpy( pep->key , rt[nRT].UnpackKey(), PE_RKLEN );
    pe[q].tm =   (short *)aObj[ o_petmv ].GetPtr();
    pe[q].nv =   (short *)aObj[ o_penvv ].GetPtr();
    pe[q].P =    (double *)aObj[ o_pepv ].GetPtr();
    pe[q].V =    (double *)aObj[ o_pevv ].GetPtr();
    pe[q].T =    (double *)aObj[ o_petv ].GetPtr();
    pe[q].Tau =  (double *)aObj[ o_petauv ].GetPtr();
    pe[q].pXi =  (double *)aObj[ o_pepxiv ].GetPtr();
    pe[q].Nu =   (double *)aObj[ o_penuv ].GetPtr();
    pe[q].Kin =  (double *)aObj[ o_pekinv ].GetPtr();
    pe[q].Modc = (double *)aObj[ o_pemodc ].GetPtr();
    pe[q].Expr = (char *)aObj[ o_peexpr ].GetPtr();
    pe[q].stl =  (char (*)[EQ_RKLEN])aObj[ o_pestl ].GetPtr();
    pe[q].sdref = (char (*)[V_SD_RKLEN])aObj[ o_pesdref ].GetPtr();
    pe[q].sdval = (char (*)[V_SD_VALEN])aObj[ o_pesdval ].GetPtr();
    pe[q].tprn = (char *)aObj[ o_petprn ].GetPtr();
    // graphics
    pe[q].lNam = (char (*)[MAXGRNAME])aObj[ o_pclnam ].GetPtr();
    pe[q].lNamE = (char (*)[MAXGRNAME])aObj[ o_pclname ].GetPtr();
    pe[q].gr_expr = (char *)aObj[ o_pcexpr ].GetPtr();
    pe[q].x0    = (double *)aObj[ o_pcx0 ].GetPtr();
    pe[q].y0    = (double *)aObj[ o_pcy0 ].GetPtr();
    pe[q].xE    = (double *)aObj[ o_pcxe ].GetPtr();
    pe[q].yE    = (double *)aObj[ o_pcye ].GetPtr();
    plot  = (TPlotLine *)aObj[ o_pcplline ].GetPtr();
}

// free dynamic memory in objects and values
void TProcess::dyn_kill(int q)
{
    ErrorIf( pep!=&pe[q], GetName(), "Illegal access to pe in dyn_kill.");
    pe[q].tm =   (short *)aObj[ o_petmv ].Free();
    pe[q].nv =   (short *)aObj[ o_penvv ].Free();
    pe[q].P =    (double *)aObj[ o_pepv ].Free();
    pe[q].V =    (double *)aObj[ o_pevv ].Free();
    pe[q].T =    (double *)aObj[ o_petv ].Free();
    pe[q].Tau =  (double *)aObj[ o_petauv ].Free();
    pe[q].pXi =  (double *)aObj[ o_pepxiv ].Free();
    pe[q].Nu =   (double *)aObj[ o_penuv ].Free();
    pe[q].Kin =  (double *)aObj[ o_pekinv ].Free();
    pe[q].Modc = (double *)aObj[ o_pemodc ].Free();
    pe[q].Expr = (char *)aObj[ o_peexpr ].Free();
    pe[q].stl =  (char (*)[EQ_RKLEN])aObj[ o_pestl ].Free();
    pe[q].sdref = (char (*)[V_SD_RKLEN])aObj[ o_pesdref ].Free();
    pe[q].sdval = (char (*)[V_SD_VALEN])aObj[ o_pesdval ].Free();
    pe[q].tprn = (char *)aObj[ o_petprn ].Free();
    // graphics
    pe[q].lNam = (char (*)[MAXGRNAME])aObj[ o_pclnam ].Free();
    pe[q].lNamE = (char (*)[MAXGRNAME])aObj[ o_pclname ].Free();
    pe[q].gr_expr = (char *)aObj[ o_pcexpr ].Free();
    pe[q].x0    = (double *)aObj[ o_pcx0 ].Free();
    pe[q].y0    = (double *)aObj[ o_pcy0 ].Free();
    pe[q].xE    = (double *)aObj[ o_pcxe ].Free();
    pe[q].yE    = (double *)aObj[ o_pcye ].Free();
    plot  = (TPlotLine *)aObj[ o_pcplline ].Free();
}


// realloc dynamic memory
void TProcess::dyn_new(int q)
{
    ErrorIf( pep!=&pe[q], GetName(), "Illegal access to pe in dyn_new.");

    if( pe[q].Pvtm != S_OFF )
        pe[q].tm = (short *)aObj[ o_petmv ].Alloc( pe[q].Ntm, 1, I_);
    else  pe[q].tm =   (short *)aObj[ o_petmv ].Free();

    if( pe[q].PvNV != S_OFF )
        pe[q].nv = (short *)aObj[ o_penvv ].Alloc( pe[q].dNNV, 1, I_);
    else  pe[q].nv =   (short *)aObj[ o_penvv ].Free();

    if( pe[q].PvP != S_OFF )
        pe[q].P = (double *)aObj[ o_pepv ].Alloc( pe[q].dNP, 1, D_);
    else  pe[q].P =    (double *)aObj[ o_pepv ].Free();

    if( pe[q].PvT != S_OFF )
        pe[q].T = (double *)aObj[ o_petv].Alloc( pe[q].dNT, 1, D_);
    else  pe[q].T =    (double *)aObj[ o_petv ].Free();

    if( pe[q].PvV != S_OFF )
        pe[q].V = (double *)aObj[ o_pevv].Alloc( pe[q].dNV, 1, D_);
    else  pe[q].V =    (double *)aObj[ o_pevv ].Free();

    if( pe[q].PvTau != S_OFF )
        pe[q].Tau = (double *)aObj[ o_petauv ].Alloc( pe[q].NTau, 1, D_);
    else  pe[q].Tau =  (double *)aObj[ o_petauv ].Free();

    if( pe[q].PvpXi != S_OFF )
        pe[q].pXi = (double *)aObj[ o_pepxiv].Alloc( pe[q].NpXi, 1, D_);
    else  pe[q].pXi =  (double *)aObj[ o_pepxiv ].Free();

    if( pe[q].PvNu != S_OFF )
        pe[q].Nu = (double *)aObj[ o_penuv].Alloc( pe[q].NNu, 1, D_);
    else  pe[q].Nu =   (double *)aObj[ o_penuv ].Free();

    if( pe[q].PvKin != S_OFF )
        pe[q].Kin = (double *)aObj[ o_pekinv].Alloc( pe[q].Nrea, pe[q].Nrp, D_);
    else  pe[q].Kin =  (double *)aObj[ o_pekinv ].Free();

    if( pe[q].PvModc != S_OFF )
        pe[q].Modc = (double *)aObj[ o_pemodc].Alloc( pe[q].Ntm, pe[q].Nmc, D_);
    else  pe[q].Modc = (double *)aObj[ o_pemodc ].Free();

    if( pe[q].PsEqn != S_OFF )
    {
        pe[q].Expr = (char *)aObj[ o_peexpr].Alloc( 1, 2048, S_);
        pe[q].tprn = (char *)aObj[ o_petprn].Alloc( 1, 2048, S_);
    }
    else
    {
        pe[q].Expr = (char *)aObj[ o_peexpr ].Free();
        pe[q].tprn = (char *)aObj[ o_petprn ].Free();
    }

    if( pe[q].Nsd > 0 )
    {
        pe[q].sdref =
            (char (*)[V_SD_RKLEN])aObj[ o_pesdref].Alloc( pe[q].Nsd, 1, V_SD_RKLEN );
        pe[q].sdval =
            (char (*)[V_SD_VALEN])aObj[ o_pesdval].Alloc( pe[q].Nsd, 1, V_SD_VALEN );
    }
    else
    {
        pe[q].sdref = (char (*)[V_SD_RKLEN])aObj[ o_pesdref ].Free();
        pe[q].sdval = (char (*)[V_SD_VALEN])aObj[ o_pesdval ].Free();
    }
    pe[q].stl = (char (*)[EQ_RKLEN])aObj[ o_pestl].Alloc( pe[q].NR1, 1, EQ_RKLEN );


   // graphics
    if( pe[q].PsGR != S_OFF )
    {
      pe[q].lNam = (char (*)[MAXGRNAME])aObj[ o_pclnam ].Alloc( 1,
                 pe[q].dimXY[1], MAXGRNAME);
      pe[q].gr_expr = (char *)aObj[ o_pcexpr ].Alloc( 1, 2048, S_);
      pe[q].x0    = (double *)aObj[ o_pcx0 ].Alloc(pe[q].dimXY[0], 1, D_);
      pe[q].y0    = (double *)aObj[ o_pcy0 ].Alloc(
                      pe[q].dimXY[0], pe[q].dimXY[1], D_);

    }
    else
    {
      pe[q].lNam = (char (*)[MAXGRNAME])aObj[ o_pclnam ].Free();
      pe[q].gr_expr = (char *)aObj[ o_pcexpr ].Free();
      pe[q].x0    = (double *)aObj[ o_pcx0 ].Free();
      pe[q].y0    = (double *)aObj[ o_pcy0 ].Free();
      pe[q].dimXY[1] = 0;
      plot  = (TPlotLine *)aObj[ o_pcplline ].Free();
   }
    if( pe[q].PvEF == S_OFF )
    {
       pe[q].lNamE = (char (*)[MAXGRNAME])aObj[ o_pclname ].Free();
       pe[q].xE    = (double *)aObj[ o_pcxe ].Free();
       pe[q].yE    = (double *)aObj[ o_pcye ].Free();
       pe[q].dimEF[1] = pe[q].dimEF[0] = 0;
    }
    else
    {
        pe[q].lNamE = (char (*)[MAXGRNAME])aObj[ o_pclname ].Alloc(1,
                     pe[q].dimEF[1], MAXGRNAME);
        pe[q].xE    = (double *)aObj[ o_pcxe ].Alloc(pe[q].dimEF[0], 1, D_);
        pe[q].yE    = (double *)aObj[ o_pcye ].Alloc(
                  pe[q].dimEF[0],pe[q].dimEF[1], D_);
    }
}

void  TProcess::set_type_flags( char type)
{
   TProfil *aPa= TProfil::pm;

    switch( type )
    {
    case P_TITR:
        strncpy( &pep->Istat, "0++-+--++-+-", 12);
        break;
    case P_GOLD_TITR:
        strncpy( &pep->Istat, "0++-+--+--++", 12);
        break;
    case P_REACT:
        strncpy( &pep->Istat, "0++-++-+--+-", 12);
        break;
    case P_SYST:
        strncpy( &pep->Istat, "0++-+-----+-", 12);
        break;
    case P_PVT:
    default:
        memcpy( &pep->Istat, aPa->pa.PEpsc, 12 );
    }

}

//set default information
void TProcess::set_def( int q)
{
    ErrorIf( pep!=&pe[q], GetName(), "Illegal access to pe in set_def");
    TProfil *aPa= TProfil::pm;

    set_type_flags( rt[rtNum()].FldKey(9)[0] );
    memcpy( &pe[q].Pvtm, aPa->pa.PEpvc, 12 );
//    strncpy( pe[q].name,  rt[rtNum()].FldKey(8), MAXFORMULA );
    strcpy( pe[q].name,  "`" );   // Fixed for debugging
    strcpy( pe[q].notes, "`" );

    memset( &pe[q].Ntim, 0, sizeof(short)*39 );
    memset( pe[q].Pi, 0, sizeof(double)*24 );
    memset( pe[q].gdkey, 0, sizeof(char)*GD_RKLEN );
    memset( pe[q].stkey, 0, sizeof(char)*(EQ_RKLEN+10) );
    memset( pe[q].tpkey, 0, sizeof(char)*TP_RKLEN );
    memset( &pe[q].NeMax, 0, sizeof(short)*8 );
    memset( &pe[q].c_P, 0, sizeof(double)*11 );
    pe[q].NeMax =400; // fixed by DAK - was only 100

    pe[q].tm =  0;
    pe[q].nv =  0;
    pe[q].P =   0;
    pe[q].V =   0;
    pe[q].T =   0;
    pe[q].Tau = 0;
    pe[q].pXi = 0;
    pe[q].Nu =  0;
    pe[q].Kin = 0;
    pe[q].Modc =0;
    pe[q].Expr =0;
    pe[q].stl = 0;
    pe[q].sdref = 0;
    pe[q].sdval = 0;
    pe[q].tprn = 0;
    // graphics
    strcpy( pe[q].xNames, "xp" );  
    strcpy( pe[q].yNames, "yp" );
    pe[q].dimEF[1] = pe[q].dimEF[0] = 0;
    pe[q].dimXY[1] = pe[q].dimXY[0] = 0;
    pe[q].lNam = 0;
    pe[q].lNamE = 0;
    pe[q].gr_expr = 0;
    pe[q].x0    = 0;
    pe[q].y0    = 0;
    pe[q].xE    = 0;
    pe[q].yE    = 0;
    plot  = 0;

}


// return true if nessasary recalc base SYSEQ
bool TProcess::check_input( const char * /*key*/, int /*Level*/ )
{
    vstr pkey(MAXRKEYLEN+10);
    if( pVisor->ProfileMode != true )
        return true;

    TProfil* PRof = (TProfil*)(&aMod[RT_PARAM]);
    //Get base SysEq key from process key
    rt[RT_SYSEQ].MakeKey( RT_PROCES, pkey, RT_PROCES, 0, RT_PROCES, 1,
                           RT_PROCES, 2, RT_PROCES, 3, RT_PROCES, 4,
                           RT_PROCES, 5, RT_PROCES, 6, RT_PROCES, 7, K_END);
    // read SysEq record and unpack data
    PRof->loadSystat( pkey );
    // test changes in system after process calc
    pep->syt = rt[RT_SYSEQ].Rtime();
    if( pep->syt > rt[nRT].Rtime() )
        return true;
    return false;
}

// test data size in record
bool TProcess::pe_dimValid()
{
    bool i=true;
    // test data size
    ErrorIf( pep->Nxi<=0 || pep->Nxi > 1000, GetName(),
             "You forgot to specify the number of points! \n Please, do it!");
    if( pep->NeMax <= pep->Nxi )
    {
        i=false;
        pep->NeMax =400;
    }  // fixed by DAK - was 200
    pep->Ntm = pep->Nxi;
    pep->dNNV = pep->Nxi;
    pep->dNP = pep->Nxi;
    pep->dNT = pep->Nxi;
    pep->dNV = pep->Nxi;
    pep->NTau = pep->Nxi;
    pep->NpXi = pep->Nxi;
    pep->NNu = pep->Nxi;

    if( pep->PvKin != S_OFF && ( pep->Nrea <= 0 || pep->Nrp <= 0 ))
    {
        i=false;
        pep->Nrea = pep->Nrp = 2;
    }
    if( pep->PvModc != S_OFF && pep->Nmc <= 0 )
    {
        i=false;
        pep->Nmc = 1;
    }

    if(  !pep->NR1 ) pep->NR1 = pep->Nxi;

    if( pep->PsGR == S_OFF )
        pep->PvEF = S_OFF;
    if( pep->PsGR != S_OFF )
    {
        pep->dimXY[0] = pep->Nxi;
        if( pep->dimXY[1] == 0 )
        {
          i = false;
          pep->dimXY[1] = 2;
        }
    }

    if( i==false )
        vfMessage(window(), GetName(), "Invalid dimensions in PROCES data structure!", vfErr);
    // set and test flags
    if( pep->PsIN != S_OFF || pep->PsPro != S_OFF )
    {
        pep->PsBC = S_ON;
        pep->PsUX = S_ON;
        pep->PsEqn = S_ON;
    }
    if( pep->PsGT != S_OFF )
    {
        pep->PsBC = S_ON;
        pep->PsUX = S_ON;
    }

    if( pep->PsIN !=S_OFF && pep->PsPro != S_OFF)
    {
        i=false;
        vfMessage(window(), GetName(), "Invalid mode of PROCES simulations", vfErr);
    }
    return i;
}

// set begin initalization
void TProcess::pe_initiate()
{
    pep->Istat = P_STARTED;
    pep->Loop = 1;
    pep->Nst = START_;
    pep->i = START_;
    pep->j = START_;
    pep->c_nrk = START_;
    pep->c_tm = pep->tmi[START_];
    pep->c_NV = pep->NVi[START_];
    pep->c_P = pep->Pi[START_];
    pep->c_T = pep->Ti[START_];
    pep->c_V = pep->Vi[START_];
    pep->c_Tau = pep->Taui[START_];
    pep->c_pXi = pep->pXii[START_];
    pep->c_Nu = pep->Nui[START_];
    pep->c_pH = pep->pHi[START_];
    pep->c_pe = pep->pei[START_];
    pep->NP = 0;
}

// recalc working parametres, if no math script is given
void TProcess::pe_next()
{
    pep->NP = 1;
    if( !pep->Nst )
        return;
    pep->j = pep->Nst;
    pep->c_nrk = min( pep->Nst, (short)(pep->NR1-1));
    pep->c_tm += pep->tmi[STEP_];
    pep->c_NV += pep->NVi[STEP_];
    pep->c_P += pep->Pi[STEP_];
    pep->c_T += pep->Ti[STEP_];
    pep->c_V += pep->Vi[STEP_];
    pep->c_Tau += pep->Taui[STEP_];
    pep->c_pXi += pep->pXii[STEP_];
    pep->c_Nu += pep->Nui[STEP_];
    pep->c_pH += pep->pHi[STEP_];
    pep->c_pe += pep->pei[STEP_];
}

// Input necessary data and links objects
void TProcess::RecInput( const char *key )
{
    TCModule::RecInput( key );
}

//Rebuild record structure before calc
int
TProcess::RecBuild( const char *key, int mode  )
{

    TProfil *aPa=TProfil::pm;
    vstr tbuf(100);
    if( pVisor->ProfileMode != true )
        Error( GetName(), "Please, do it in Project mode!" );


   char type = '!';
   int  sizes[6];
   int ret;

AGAIN:
   if( mode == VF_CLEARALL )
   {
     type  = rt[rtNum()].FldKey(9)[0];
     ret =  mode;
     if( !vfProcessSet( window(), key, type, sizes ))
        return ret;   // cancel
     dyn_kill();
     set_def(); // set default data or zero if necessary
     set_type_flags( type );
     pep->Nxi = (short)sizes[0];
     pep->Nsd = (short)sizes[5];
     pep->Nmc = (short)sizes[1];
     if( pep->Nmc == 0  )
         pep->PvModc = S_OFF;
     else
         pep->PvModc = S_ON;
     pep->dimXY[1] = (short)sizes[2];
     if( pep->dimXY[1] == 0  )
         pep->PsGR = S_OFF;
     else
     {    pep->PsGR = S_ON;
          pep->dimXY[0] = pep->Nxi;
     }
     pep->dimEF[0] = (short)sizes[3];
     pep->dimEF[1] = (short)sizes[4];
     if( pep->dimEF[1] == 0 || pep->dimEF[0] == 0  )
         pep->PvEF = S_OFF;
     else
         pep->PvEF = S_ON;
   }
   else
   {
     ret = TCModule::RecBuild( key, mode );
     if( ret == VF_BYPASS )
      goto SET_OK;
   }
   if( pe_dimValid()==false  )
        goto AGAIN;
   dyn_new();

   if( pep->PvEF != S_OFF  )
        for(int i=0; i<pep->dimEF[1]; i++ )
        {
            sprintf( tbuf, "%s%d", aPa->pa.GDpsc, i+1 );
            if( !*pep->lNamE[i] || *pep->lNamE[i] == ' ' )
                strncpy( pep->lNamE[i], tbuf,MAXGRNAME );
        }
    if( pep->PsGR != S_OFF  )
         for(int j=0; j< pep->dimXY[1]; j++ )
         {
            sprintf( tbuf, "%s%d", aPa->pa.GDpsc, j+1 );
            if( !*pep->lNam[j]|| *pep->lNam[j] == ' ' )
               strncpy( pep->lNam[j], tbuf, MAXGRNAME );
          }

SET_OK:  // set begin of calc
    pe_initiate();
    pVisor->Update();
    return ret;
}

// make fields of SyStat key
void
TProcess::pe_qekey()
{
    vstr buf(40);

    sprintf(buf, "%.4d", pep->c_tm);
    memset(pep->timep, 0, 5 );
    strncpy(pep->timep, buf, 4 );
    sprintf(buf, "%.3d", pep->c_NV);
    memset( pep->NVp, 0, 4 );
    strncpy( pep->NVp, buf, 3 );
    gcvt( pep->c_T, 6, pep->TCp );
    gcvt( pep->c_P, 6, pep->Pp );
    gcvt( pep->c_V, 6, pep->Bnamep );
}

// save current values in process iterstions to arrays
void
TProcess::pe_test()
{
    short nIt;

    if( pep->PsIN != S_OFF )
    {
        nIt = pep->i;
        pep->j = nIt;
    }
    else
    {
        nIt = pep->Nst;
        pep->j = nIt /*+1*/;
    }
    pep->c_nrk = min( nIt, (short)(pep->NR1-1));

    if( pep->Ntm>nIt && pep->Pvtm != S_OFF ) pep->tm[nIt] = pep->c_tm;
    if( pep->dNNV>nIt && pep->PvNV != S_OFF ) pep->nv[nIt] = pep->c_NV;
    if( pep->dNP>nIt && pep->PvP != S_OFF ) pep->P[nIt] = pep->c_P;
    if( pep->dNT>nIt && pep->PvT != S_OFF ) pep->T[nIt] = pep->c_T;
    if( pep->dNV>nIt && pep->PvV != S_OFF ) pep->V[nIt] = pep->c_V;
    if( pep->NTau>nIt && pep->PvTau != S_OFF ) pep->Tau[nIt] = pep->c_Tau;
    if( pep->NpXi>nIt && pep->PvpXi != S_OFF ) pep->pXi[nIt] = pep->c_pXi;
    if( pep->NNu>nIt && pep->PvNu != S_OFF )  pep->Nu[nIt] = pep->c_Nu;

    // test increments

    if( pep->tmi[STEP_]!= 0 )
      if( pep->c_tm == pep->tmi[STOP_] )
          pep->Loop = 0;

    if( pep->NVi[STEP_]!= 0 )
      if( pep->c_NV == pep->NVi[STOP_] )
          pep->Loop = 0;

    if( pep->Pi[STEP_]!= 0 )
      if( fabs( pep->c_P - pep->Pi[STOP_]) < fabs(pep->Pi[STEP_]) )
          pep->Loop = 0;

    if( pep->Ti[STEP_]!= 0 )
      if( fabs( pep->c_T - pep->Ti[STOP_]) < fabs(pep->Ti[STEP_]) )
          pep->Loop = 0;

    if( pep->Vi[STEP_]!= 0 )
      if( fabs( pep->c_V - pep->Vi[STOP_]) < fabs(pep->Vi[STEP_]) )
          pep->Loop = 0;

    if( pep->Taui[STEP_]!= 0 )
      if( fabs( pep->c_Tau - pep->Taui[STOP_]) < fabs(pep->Taui[STEP_]) )
          pep->Loop = 0;

    if( pep->pXii[STEP_]!= 0 )
      if( fabs( pep->c_pXi - pep->pXii[STOP_]) < fabs(pep->pXii[STEP_]) )
          pep->Loop = 0;

   if( pep->Nui[STEP_]!= 0 )
      if( fabs( pep->c_Nu - pep->Nui[STOP_]) < fabs(pep->Nui[STEP_]) )
          pep->Loop = 0;

   if( pep->Nui[STEP_]!= 0 )
      if( fabs( pep->c_pH - pep->Nui[STOP_]) < fabs(pep->Nui[STEP_]) )
          pep->Loop = 0;

   if( pep->pei[STEP_]!= 0 )
      if( fabs( pep->c_pe - pep->pei[STOP_]) < fabs(pep->pei[STEP_]) )
          pep->Loop = 0;
    /////

    if( pep->PsIN != S_OFF  )  nIt--;
    if( nIt >= pep->Nxi-1 || pep->Nst >= pep->NeMax )
              pep->Loop = 0;
    if( pep->PsPro != S_OFF && pep->NP == 1)
        pep->Loop = 1;


    pe_qekey();
}

// Translate, analyze and unpack equations of the process
void TProcess::pe_text_analyze()
{
  TProfil* PRof = (TProfil*)(&aMod[RT_PARAM]);
    try
    {
      if( pep->PsEqn != S_OFF )
      {
        PRof->ET_translate( o_petprn, o_peexpr, 0, PRof->mup->L, 0, PRof->pmp->L );
        rpn[0].GetEquat( (char *)aObj[o_petprn].GetPtr() );
      }
    }
    catch( TError& xcpt )
    {
        char *erscan = (char *)aObj[o_peexpr].GetPtr();
        vfMessage(window(), xcpt.title, xcpt.mess);
        TProcess::pm->CheckEqText(  erscan,
                    "Error in translation of equations of process: " );
        Error(  GetName() , xcpt.mess.c_str() );
    }

    try
    {
      if( pep->PsGR != S_OFF )
      {

        PRof->ET_translate( o_petprn, o_pcexpr, 0, PRof->mup->L, 0, PRof->pmp->L );
        rpn[1].GetEquat( (char *)aObj[o_petprn].GetPtr() );
      }
    }
    catch( TError& xcpt )
    {
        char *erscan = (char *)aObj[o_pcexpr].GetPtr();
        vfMessage(window(), xcpt.title, xcpt.mess);
        TProcess::pm->CheckEqText(  erscan,
             "Error in translation of graph equations of process: " );
        Error(  GetName() , xcpt.mess.c_str() );
    }
}

// calc function for Method of Gold dividion
double TProcess::f_proc( double x )
{
    TProfil* PRof = (TProfil*)(&aMod[RT_PARAM]);
    pep->Loop = 2;
    pep->c_Eh = x;
    CalcEquat();
    // calc SyStat on iterations
    PRof->CalcEqstat( false /*pointShow==-1*/ ); // calc current SyStat
    pep->Loop = 1;
    CalcEquat();
    return( pep->c_Nu );
}


// Method of Gold Section ( PsPro != S_OFF)
void
TProcess::proc_titr()
{
    double Fa, Fb, Fx1, Fx2, a, b, x1, x2;

    if(  !pep->Nst ) x1 = pep->pXii[0];
    else x1 = pep->c_Eh;
    x2 = pep->pXii[1];
    a = min( x1, x2 );
    b = max( x1, x2 );
    if( (b-a) < pep->pXii[2]) goto DONE;
    x1 = a + .382*(b-a);
    x2 = a + .618*(b-a);
    Fa = f_proc( a);
    if( fabs(Fa) < pep->Nui[1] )
    {
        b = a;
        goto DONE;
    }
    Fb = f_proc( b);
    if(  fabs(Fb) < pep->Nui[1] )
    {
        a = b;
        goto DONE;
    }
    if( (Fa*Fb) > 0)
        Error( GetName(), "No result in specified interval! Change interval!");
    Fx1 = f_proc( x1);
    Fx2 = f_proc( x2);
    do
    {
        if( fabs( Fx1 ) < pep->Nui[1] )
        {
            a = b = x1;
            goto DONE;
        }
        if( fabs( Fx2 ) < pep->Nui[1] )
        {
            a = b = x2;
            goto DONE;
        }
        if( fabs( Fx1) > fabs( Fx2) )
        {
            a = x1;
            if( (b-a) < pep->pXii[2]) goto DONE;
            x1 = x2;
            Fx1 = Fx2;
            x2 = a + .618*(b-a);
            Fx2 = f_proc( x2);
        }
        else
        {
            b = x2;
            if( (b-a) < pep->pXii[2]) goto DONE;
            x2 = x1;
            Fx2 = Fx1;
            x1 = a + .382*(b-a);
            Fx1 = f_proc( x1);
        }
    }
    while( (b-a) > pep->pXii[2] );
DONE:
    x1 = (a+b)/2;
    pep->Loop = 2;
    pep->c_Eh = x1;
    CalcEquat();
}

//Recalc rpn structure
void
TProcess::CalcEquat(  )
{
    try
    {
        rpn[0].CalcEquat();
    }
    catch( TError& xcpt)
    {
        gstring str = xcpt.mess;
        str += "\n Cancel process?";
        if( vfQuestion(  window(), xcpt.title, str))
            Error( GetName(), "Process simulation halted");
    }
}

//Recalc record structure
void
TProcess::RecCalc( const char *key )
{
    int nRec;
    TProfil* PRof = (TProfil*)(&aMod[RT_PARAM]);

    if( pVisor->ProfileMode != true )
        Error( GetName(), "Please, do it in Project mode!" );

    if( pep->Istat != P_EXECUTE )
    {
        if(  pep->PsGR != S_OFF && vfQuestion(window(),
             GetName(), "Use graphic window?") )
        {
            RecordPlot( key );
            pointShow = START_;
        }
        else pointShow = -1;
        pe_initiate();
    }

    pep->Istat = P_EXECUTE;
    TCModule::RecCalc(key);

    pe_qekey();
    pep->pet= rt[RT_PROCES].Rtime();
    // load current record SysEq
//    if( pep->PsUX != S_OFF  )
//    {
        if( !pep->Nst )
            rt[RT_SYSEQ].MakeKey( RT_PROCES, pep->stkey, RT_PROCES, 0, RT_PROCES,1,
             RT_PROCES, 2,  RT_PROCES, 3, RT_PROCES, 4, RT_PROCES, 5,
                            RT_PROCES, 6, RT_PROCES, 7, K_END );
        else
            rt[RT_SYSEQ].MakeKey( RT_PROCES,  pep->stkey, RT_PROCES, 0, RT_PROCES,1,
             RT_PROCES, 2, K_IMM, pep->timep, K_IMM, pep->Bnamep,
               K_IMM, pep->Pp, K_IMM, pep->TCp, K_IMM, pep->NVp, K_END );

        nRec = rt[RT_SYSEQ].Find(pep->stkey);
        if( nRec >= 0)
        {
            PRof->loadSystat( pep->stkey );   // read SysEq record and unpack data
            pep->syt = rt[RT_SYSEQ].Rtime(); // test changes in system after process calc
        }
        //       if( nRec < 0 || pep->syt < pep->pet )

        { // current key in base set before
            PRof->CalcEqstat( false /*pointShow==-1*/); // calc current SyStat
//	    pVisorImp->CalcMulti();

            TSysEq::pm->CmSave();  // save results
        }

//    }
    pe_text_analyze();  //translate equations of process

    ModUpdate("Pe_calc    Process simulation");

    while( pep->Loop ) // main cycle of process
    {
     if( pointShow==-1 )
       pVisor->Message( window(), GetName(),
                 "Calculating process; \n"
                 "Please, wait...", pep->c_nrk, pep->NR1);

        // calc equations of process
        if( pep->PsPro == S_OFF )
        {
            pe_next();

            if( pep->PsEqn != S_OFF )
                CalcEquat();

            if( !pep->Loop )
                break;
        }
        else // titration
        {
            if(  pep->NP == 1)
            {
                pep->NP = 0;
                rt[RT_SYSEQ].MakeKey( RT_PROCES,  pep->stkey, RT_PROCES, 0, RT_PROCES,1,
                                       RT_PROCES, 2, K_IMM, pep->timep, K_IMM, pep->Bnamep,
                                       K_IMM, pep->Pp, K_IMM, pep->TCp, K_IMM, pep->NVp, K_END );
                proc_titr();
            }
            else
                pe_next();
        }
        // save work parametres and make key fields
        pe_test();
        if( pep->PsIN != S_OFF  && !pep->Loop )
            break;

        // Sveta   /*aMod[RT_PROCES].*/ModUpdate("Pe_calc    Process simulation");
        // Stop Process from Andy Sveta

        // make system ( SyTest() always in CalcEqstat() )
        if( pep->PsBC != S_OFF || pep->PsRS != S_OFF)
        {
            if( pep->PsGT != S_OFF ) // evaporation
                memcpy( TSysEq::pm->ssp->PhmKey, pep->stkey, EQ_RKLEN );
            /*       if( pep->PsUX != S_OFF) // recalc system
                     if( pep->PsSY != S_OFF )
                       sprintf( sy[q].notes,"pXi = %15lg, Nu = %15lg",
                                              pep->c_pXi, pep->c_Nu);
             */
        }

        // new SyStat key
        rt[RT_SYSEQ].MakeKey( RT_PROCES,  pep->stkey, RT_PROCES, 0, RT_PROCES,1,
                               RT_PROCES, 2, K_IMM, pep->timep, K_IMM, pep->Bnamep,
                               K_IMM, pep->Pp, K_IMM, pep->TCp, K_IMM, pep->NVp, K_END );
        // test record make before
        nRec = rt[RT_SYSEQ].Find(pep->stkey);
        if( nRec >= 0 )
            pep->syt = rt[RT_SYSEQ].GetTime( nRec );
//        if( nRec < 0 || pep->PsUX != S_OFF || pep->syt < pep->pet )
//        {
            PRof->CalcEqstat( false/*pointShow==-1*/); // calc current SyStat
//	    pVisorImp->CalcMulti();

           if( pep->PsSY != S_OFF  || pep->PsUX != S_OFF  )
                 TSysEq::pm->CmSave();  // save results
//        }


     // set results

        if(  pep->PsGR != S_OFF )
            rpn[1].CalcEquat();
        if( pep->stl )
            memcpy( pep->stl+pep->c_nrk, pep->stkey, EQ_RKLEN );

        if( pointShow >= 0 )
          CalcPoint( pep->c_nrk);

        if( !(pep->PsPro != S_OFF && pep->NP == 1 ))
            pep->Nst++;

    }  /* end while() */

//    pVisor->CloseProgress();
    pep->Istat = P_FINISHED;
    if( pointShow==-1 )
       pVisor->CloseMessage();
// Get startup syseq record for fitting
    rt[RT_SYSEQ].MakeKey( RT_PROCES, pep->stkey, RT_PROCES, 0, RT_PROCES,1,
             RT_PROCES, 2,  RT_PROCES, 3, RT_PROCES, 4, RT_PROCES, 5,
                            RT_PROCES, 6, RT_PROCES, 7, K_END );
    nRec = rt[RT_SYSEQ].Find(pep->stkey);
    if( nRec >= 0)
       PRof->loadSystat( pep->stkey );   // read SysEq record and unpack data

    ModUpdate("Pe_calc    Finished OK");

}

//Calculate one point to graph
void
TProcess::CalcPoint( int nPoint )
{
    if( nPoint >= pep->dimXY[0] )
     return;
    // Add point to graph screen
    if( gd_gr )
        gd_gr->AddPoint( 0, nPoint );
}

void
TProcess::RecordPlot( const char* /*key*/ )
{

    TIArray<TPlot> plt;

    plt.Add( new TPlot(o_pcx0, o_pcy0 ));
    int  nLn = plt[ 0 ].getLinesNumber();
    if( pep->PvEF != S_OFF )
    {
        plt.Add( new TPlot(o_pcxe, o_pcye ));
        nLn += plt[1].getLinesNumber();
    }
    if( plot )
    {
        int oldN = aObj[o_pcplline].GetN();
        TPlotLine defpl("", 4);

        plot = (TPlotLine * )aObj[ o_pcplline ].Alloc( nLn, sizeof(TPlotLine) );
        for(int ii=0; ii<nLn; ii++ )
        {
            if( ii >= oldN )
                plot[ii] = defpl;
            if(ii < pep->dimXY[1] )
                strncpy( plot[ii].name, pep->lNam[ii], MAXGRNAME );
            else
                strncpy( plot[ii].name, pep->lNamE[ii-pep->dimXY[1]], MAXGRNAME );
            plot[ii].name[MAXGRNAME] = '\0';
        }
        gd_gr = new GraphWindow( this, plt, pep->name,
                                     pep->size[0], pep->size[1], plot,
                                     pep->axisType, pep->xNames, pep->yNames);
    }
    else
    {
      TCStringArray lnames;
      int ii;
      for( ii=0; ii<pep->dimXY[1]; ii++ )
          lnames.Add( gstring(pep->lNam[ii], 0, MAXGRNAME ));
      for( ii=0; ii<pep->dimEF[1]; ii++ )
          lnames.Add( gstring( pep->lNamE[ii], 0, MAXGRNAME ));
      gd_gr = new GraphWindow( this, plt, pep->name,
          pep->xNames, pep->yNames, lnames );
    }
}


bool
TProcess::SaveGraphData( GraphData *gr )
{
// We can only have one Plot dialog (modal one) so condition should be omitted!!
     if( !gd_gr )
      return false;
     if( gr != gd_gr->getGraphData() )
      return false;
    pep->axisType[0] = (short)gr->axisType;
    pep->axisType[1] = (short)gr->b_color[0];
    pep->axisType[2] = (short)gr->b_color[1];
    pep->axisType[3] = (short)gr->b_color[2];
    strncpy( pep->xNames, gr->xName.c_str(), 9);
    strncpy( pep->yNames, gr->yName.c_str(), 9);
    memcpy( &pep->size[0], gr->region, 4*sizeof(float) );
    memcpy( &pep->size[1], gr->part,  4*sizeof(float) );

    plot = (TPlotLine *)
           aObj[ o_pcplline].Alloc( gr->lines.GetCount(), sizeof(TPlotLine));
    for(uint ii=0; ii<gr->lines.GetCount(); ii++ )
    {
        plot[ii] = gr->lines[ii];
        //  lNam and lNamE back
        if(ii < pep->dimXY[1] )
            strncpy(  pep->lNam[ii], plot[ii].name, MAXGRNAME );
        else
            strncpy(  pep->lNamE[ii-pep->dimXY[1]], plot[ii].name, MAXGRNAME );
    }
    //    }
    pVisor->Update();
    fEdit = true;
    return true;
}


void
TProcess::CmHelp()
{
    pVisor->OpenHelp( GEMS_PE_HTML );  //  05.01.01
}



// ------------------- End of m_process.cpp --------------------------

