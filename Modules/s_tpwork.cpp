//-------------------------------------------------------------------
// $Id: s_tpwork.cpp 968 2007-12-13 13:23:32Z gems $
//
// Copyright (C) 1992-2000  D.Kulik, S.Dmitrieva
//
// Implementation of TpworkList class - working t/d parametres
//
// This file is part of a GEM-Selektor (GEMS) v.3.1.x program
// environment for thermodynamic modeling in geochemistry
// Uses: GEM-Selektor GUI GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the GPL v.3 license

//
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch;
//-------------------------------------------------------------------
//

#include "v_object.h"
#include "s_tpwork.h"

TpworkList aW;
struct WATERPARAM aWp;
struct STATES aSta;
struct SPECS aSpc;

TpworkList::TpworkList():
        std::vector<std::shared_ptr<TPWORK>>()
{
    twp = nullptr;
}

TpworkList::~TpworkList()
{
    clear();
}

void TpworkList::ods_link( int q )
{
    if( q > static_cast<int>(size()) )
        Error( "E27DCrun: TpworkList", "Invalid number of TPWORK structures!");
    if( q == static_cast<int>(size()) )
    {
        push_back( std::shared_ptr<TPWORK>(new TPWORK()) );
        memset( (void *)at(q).get(), 0, sizeof(TPWORK) );
    }
    twp = at(q).get();
    aObj[ o_twdrkey ]->SetPtr(  twp->DRkey );
    aObj[ o_twunits ]->SetPtr( &twp->pSD ); /*4*/
    aObj[o_twpp]->SetPtr( &twp->P );        /*1*/
    aObj[o_twppr]->SetPtr( &twp->Pst );     /*1*/
    aObj[o_twtc]->SetPtr( &twp->TC );       /*1*/
    aObj[o_twtcr]->SetPtr( &twp->TCst );    /*1*/
    aObj[o_twtk]->SetPtr( &twp->T );        /*1*/
    aObj[o_twtkr]->SetPtr( &twp->Tst );     /*1*/
    aObj[o_tdsr]->SetPtr( &twp->dS );       /*1*/
    aObj[o_tws]->SetPtr( &twp->S );         /*1*/
    aObj[o_tdevs]->SetPtr( &twp->devS );    /*1*/
    aObj[o_tdgr]->SetPtr( &twp->dG );       /*1*/
    aObj[o_twg]->SetPtr( &twp->G );         /*1*/
    aObj[o_tdevg]->SetPtr( &twp->devG );    /*1*/
    aObj[o_twk]->SetPtr( &twp->K );         /*1*/
    aObj[o_tlogk]->SetPtr( &twp->lgK );     /*1*/
    aObj[o_tdlogK]->SetPtr( &twp->dlgK );   /*1*/
    aObj[o_tdhr]->SetPtr( &twp->dH );       /*1*/
    aObj[o_twh]->SetPtr( &twp->H );         /*1*/
    aObj[o_tdevh]->SetPtr( &twp->devH );    /*1*/
    aObj[o_tdcpr]->SetPtr( &twp->dCp );     /*1*/
    aObj[o_twcp]->SetPtr( &twp->Cp );       /*1*/
    aObj[o_tdevcp]->SetPtr( &twp->devCp );    /*1*/
    aObj[o_tdvr]->SetPtr( &twp->dV );       /*1*/
    aObj[o_twv]->SetPtr( &twp->V );         /*1*/
    aObj[o_tdevv]->SetPtr( &twp->devV );    /*1*/
    aObj[ o_twalpbet]->SetPtr( &twp->Alp ); /*2*/
    aObj[ o_twfugv  ]->SetPtr( &twp->Fug ); /*2*/
    aObj[ o_twRT    ]->SetPtr( &twp->RT );  /*1*/
    aObj[ o_twrow   ]->SetPtr( &twp->wRo ); /*1*/
    aObj[ o_twew    ]->SetPtr( &twp->wEps );/*1*/
    aObj[ o_tww  ]->SetPtr( twp->wtW );     /*10*/
}

void TpworkList::set_zero( int q )
{
    if( q >= static_cast<int>(size()) )
        Error( "TpworkList", "Invalid number of TPWORK structures!");
    memset( (void *)at(q).get(), 0, sizeof(TPWORK) );
}
//--------------------- End of s_tpwork.cpp ---------------------------
