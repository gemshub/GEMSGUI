//-------------------------------------------------------------------
// Id: gems/mods/s_tpwork.cpp
//      version 2.0.0                           edited 2001-09-08
//  Created: 1992-1997
//
// Copyright (C) 1992-2000  D.Kulik, S.Dmitrieva
//
// Implementation of TpworkList class - working t/d parametres
//
// This file is part of a GEM-Selektor (GEMS) v.2.x.x program
// environment for thermodynamic modeling in geochemistry
// Uses: GEM-Vizor GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://les.web.psi.ch/Software/GEMS-PSI for more information
// E-mail: gems2.support@psi.ch; chud@igc.irk.ru
//-------------------------------------------------------------------
//
#include "v_object.h"
#include "s_tpwork.h"

TpworkList aW;
struct WATERPARAM aWp;
struct STATES aSta;
struct SPECS aSpc;

TpworkList::TpworkList():
        TIArray<TPWORK>( 10, 1 )
{
    twp=0;
}

TpworkList::~TpworkList()
{
    Clear();
}

void TpworkList::ods_link( int q )
{
    if( q > (int)GetCount() )
        Error( "TpworkList", "Illegal number TPWORK!");
    if( q == (int)GetCount() )
    {
        Add( new TPWORK() );
        memset( (void *)&elem(q), 0, sizeof(TPWORK) );
    }
    twp = &elem( q );
    aObj[ o_twdrkey ].SetPtr(  twp->DRkey );
    aObj[ o_twunits ].SetPtr( &twp->pSD ); /*4*/
    aObj[o_twpp].SetPtr( &twp->P );        /*1*/
    aObj[o_twppr].SetPtr( &twp->Pst );     /*1*/
    aObj[o_twtc].SetPtr( &twp->TC );       /*1*/
    aObj[o_twtcr].SetPtr( &twp->TCst );    /*1*/
    aObj[o_twtk].SetPtr( &twp->T );        /*1*/
    aObj[o_twtkr].SetPtr( &twp->Tst );     /*1*/
    aObj[o_tdsr].SetPtr( &twp->dS );       /*1*/
    aObj[o_tws].SetPtr( &twp->S );         /*1*/
    aObj[o_tdevs].SetPtr( &twp->devS );    /*1*/
    aObj[o_tdgr].SetPtr( &twp->dG );       /*1*/
    aObj[o_twg].SetPtr( &twp->G );         /*1*/
    aObj[o_tdevg].SetPtr( &twp->devG );    /*1*/
    aObj[o_twk].SetPtr( &twp->K );         /*1*/
    aObj[o_tlogk].SetPtr( &twp->lgK );     /*1*/
    aObj[o_tdlogK].SetPtr( &twp->dlgK );   /*1*/
    aObj[o_tdhr].SetPtr( &twp->dH );       /*1*/
    aObj[o_twh].SetPtr( &twp->H );         /*1*/
    aObj[o_tdevh].SetPtr( &twp->devH );    /*1*/
    aObj[o_tdcpr].SetPtr( &twp->dCp );     /*1*/
    aObj[o_twcp].SetPtr( &twp->Cp );       /*1*/
    aObj[o_tdevcp].SetPtr( &twp->devCp );    /*1*/
    aObj[o_tdvr].SetPtr( &twp->dV );       /*1*/
    aObj[o_twv].SetPtr( &twp->V );         /*1*/
    aObj[o_tdevv].SetPtr( &twp->devV );    /*1*/
    aObj[ o_twalpbet].SetPtr( &twp->Alp ); /*2*/
    aObj[ o_twfugv  ].SetPtr( &twp->Fug ); /*2*/
    aObj[ o_twRT    ].SetPtr( &twp->RT );  /*1*/
    aObj[ o_twrow   ].SetPtr( &twp->wRo ); /*1*/
    aObj[ o_twew    ].SetPtr( &twp->wEps );/*1*/
    aObj[ o_tww  ].SetPtr( twp->wtW );     /*10*/
}

void TpworkList::set_zero( int q )
{
    if( q >= (int)GetCount() )
        Error( "TpworkList", "Illegal number TPWORK!");
    memset( (void *)&elem(q), 0, sizeof(TPWORK) );
}
//--------------------- End of s_tpwork.cpp ---------------------------
