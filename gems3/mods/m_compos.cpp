//-------------------------------------------------------------------
// $Id: m_compos.cpp 1379 2009-07-29 14:53:22Z gems $
//
// Implementation of TCompos class, config and calculation functions
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
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//
const char *GEMS_PCO_HTML = "gm_compos";

#include <math.h>
#include "service.h"
#include "v_object.h"
#include "visor.h"
#include "m_compos.h"
#include "m_param.h"
#include "m_icomp.h"
#include "m_dcomp.h"
//#include "m_sdata.h"
#include "m_reacdc.h"
#include "s_formula.h"
#include "filters_data.h"

TCompos* TCompos::pm;

TCompos::TCompos( int nrt ):
        TCModule( nrt )
{
    aFldKeysHelp.Add(
        "l<16 Identifier of Geochemical Composition Object (GCO)");
    aFldKeysHelp.Add(
        "l<4  Code of GCO type { AQ RO GA FL HC PM MIN }");
    aFldKeysHelp.Add(
        "l<28 Record Key Comment to GCO description");
    bcp=&bc[0];
    set_def();
    start_title = " Predefined composition objects (PCO) ";

    bcp->A = 0;
    C = 0;
    CI = 0;
    CIcl = 0;
}

TCompos::~TCompos()
{
    bc_work_dyn_kill();
}

// link values to objects
void TCompos::ods_link( int q)
{
    //  aObj[ o_bcnam].SetPtr(  bc[q].cname );
    //  aObj[ o_bctype].SetPtr( bc[q].type );
    //  aObj[ o_bcgroup].SetPtr(bc[q].group );
    aObj[ o_bcpcc].SetPtr(  &bc[q].PcIC );  /*6*/
    aObj[ o_bcname].SetPtr( bc[q].name );
    aObj[ o_bcnotes].SetPtr(bc[q].notes );
    aObj[ o_bcdim].SetPtr(  &bc[q].N );      /*6*/
    aObj[ o_bcmvol].SetPtr( &bc[q].Msys );   /*8*/

    //if( bc[q].N > 0 ) {
    aObj[ o_bccv].SetPtr( bc[q].C );
    aObj[ o_bccv ].SetDim( bc[q].N, 1 );
    aObj[ o_bcsb].SetPtr( bc[q].SB );
    aObj[ o_bcsb].SetDim( bc[q].N, 1 );
    aObj[ o_bccicl].SetPtr( bc[q].CIcl );
    aObj[ o_bccicl].SetDim( bc[q].N, 1 );
    aObj[ o_bcci].SetPtr( bc[q].CI );
    aObj[ o_bcci].SetDim( bc[q].N, 1 );
    aObj[ o_bcdelc].SetPtr( bc[q].delC );
    aObj[ o_bcdelc].SetDim( bc[q].N, 1 );
    //}
    // if( bc[q].La > 0 ) {
    aObj[ o_bcsa].SetPtr( bc[q].SA );
    aObj[ o_bcsa].SetDim( bc[q].La, 1 );
    aObj[ o_bccacl].SetPtr( bc[q].AUcl );
    aObj[ o_bccacl].SetDim( bc[q].La, 1 );
    aObj[ o_bcca].SetPtr( bc[q].CA );
    aObj[ o_bcca].SetDim( bc[q].La, 1 );
    //}
    //if( bc[q].Ld > 0 ) {
    aObj[ o_bcsmk].SetPtr(bc[q].SM );
    aObj[ o_bcsmk].SetDim( bc[q].Ld, 1 );
    aObj[ o_bccdcl].SetPtr( bc[q].CDcl );
    aObj[ o_bccdcl].SetDim( bc[q].Ld, 1 );
    aObj[ o_bcdcs].SetPtr( bc[q].DCS );
    aObj[ o_bcdcs].SetDim( bc[q].Ld, 1 );
    aObj[ o_bccd].SetPtr( bc[q].CD );
    aObj[ o_bccd].SetDim( bc[q].Ld, 1 );
    // }
    //if( bc[q].Nsd > 0 ){
    aObj[ o_bcsdref ].SetPtr( bc[q].sdref );
    aObj[ o_bcsdref ].SetDim( bc[q].Nsd, 1 );
    aObj[ o_bcsdval ].SetPtr( bc[q].sdval );
    aObj[ o_bcsdval ].SetDim( bc[q].Nsd, 1 );
    //}
    aObj[ o_bccfor].SetPtr( bc[q].CFOR );
    aObj[ o_bccfor].SetDim( 1, MAXCMPFORM );
    /*  aObj[ o_bctprn].SetPtr( bc[q].tprn );  */
    /*  OBsetDim( o_bctprn, 1, SPPTPRNBUFSIZE ); */
    bcp=&bc[q];
}

// set dynamic Objects ptr to values
void TCompos::dyn_set(int q)
{
    ErrorIf( bcp!=&bc[q], GetName(), "Illegal access to bc in dyn_set.");
    bc[q].C = (double *)aObj[ o_bccv ].GetPtr();
    bc[q].SB = (char (*)[MAXICNAME+MAXSYMB])aObj[ o_bcsb ].GetPtr();
    bc[q].CIcl = (char *)aObj[ o_bccicl ].GetPtr();
    bc[q].CI = (float *)aObj[ o_bcci ].GetPtr();
    bc[q].delC = (float *)aObj[ o_bcdelc ].GetPtr();
    bc[q].SA = (char (*)[MAXFORMUNIT])aObj[ o_bcsa ].GetPtr();
    bc[q].AUcl = (char *)aObj[ o_bccacl ].GetPtr();
    bc[q].CA = (float *)aObj[ o_bcca ].GetPtr();
    bc[q].SM = (char (*)[DC_RKLEN])aObj[ o_bcsmk ].GetPtr();
    bc[q].CDcl = (char *)aObj[ o_bccdcl ].GetPtr();
    bc[q].CD = (float *)aObj[ o_bccd ].GetPtr();
    bc[q].DCS = (char *)aObj[ o_bcdcs ].GetPtr();
    bc[q].sdref = (char (*)[V_SD_RKLEN])aObj[ o_bcsdref ].GetPtr();
    bc[q].sdval = (char (*)[V_SD_VALEN])aObj[ o_bcsdval ].GetPtr();
    bc[q].CFOR = (char *)aObj[ o_bccfor ].GetPtr();
    /* bc[q].tprn =   (char *)aObj[ o_bctprn ].GetPtr(); */
}

// free dynamic memory in objects and values
void TCompos::dyn_kill(int q)
{
    ErrorIf( bcp!=&bc[q], GetName(), "Illegal access to bc in dyn_kill.");
    bc[q].C = (double *)aObj[ o_bccv ].Free();
    bc[q].SB = (char (*)[MAXICNAME+MAXSYMB])aObj[ o_bcsb ].Free();
    bc[q].CIcl = (char *)aObj[ o_bccicl ].Free();
    bc[q].CI = (float *)aObj[ o_bcci ].Free();
    bc[q].delC = (float *)aObj[ o_bcdelc ].Free();
    bc[q].SA = (char (*)[MAXFORMUNIT])aObj[ o_bcsa ].Free();
    bc[q].AUcl = (char *)aObj[ o_bccacl ].Free();
    bc[q].CA = (float *)aObj[ o_bcca ].Free();
    bc[q].SM = (char (*)[DC_RKLEN])aObj[ o_bcsmk ].Free();
    bc[q].CDcl = (char *)aObj[ o_bccdcl ].Free();
    bc[q].CD = (float *)aObj[ o_bccd ].Free();
    bc[q].DCS = (char *)aObj[ o_bcdcs ].Free();
    bc[q].sdref = (char (*)[V_SD_RKLEN])aObj[ o_bcsdref ].Free();
    bc[q].sdval = (char (*)[V_SD_VALEN])aObj[ o_bcsdval ].Free();
    bc[q].CFOR = (char *)aObj[ o_bccfor  ].Free();
    bc[q].tprn =  (char *)aObj[ o_bctprn  ].Free();
}

// realloc dynamic memory
void TCompos::dyn_new(int q)
{
    ErrorIf( bcp!=&bc[q], GetName(), "Illegal access to bc in dyn_new.");
    //  ErrorIf( bc[q].Nmax < 1, GetName(), "bc[q].Nmax < 1" ); DAK
    ErrorIf( (bc[q].N < 1)&&(bc[q].Nmax < 1), GetName(), "bc[q].N,Nmax < 1" );

    // bc[q].C = (double *)aObj[ o_bccv].Alloc( bc[q].Nmax, 1, D_ );  DAK
    if( bc[q].N > 0 )
        bc[q].C = (double *)aObj[ o_bccv].Alloc( bc[q].N, 1, D_ );
    else
        bc[q].C = (double *)aObj[ o_bccv].Alloc( bc[q].Nmax, 1, D_ );
    //

    if( bc[q].PcDC != S_OFF && bc[q].Ld > 0 )
    {
        bc[q].SM = (char (*)[DC_RKLEN])aObj[ o_bcsmk ].Alloc( bc[q].Ld, 1, DC_RKLEN);
        bc[q].CDcl = (char *)aObj[ o_bccdcl ].Alloc( bc[q].Ld, 1, A_);
        bc[q].CD = (float *)aObj[ o_bccd ].Alloc(  bc[q].Ld, 1, F_);
        bc[q].DCS = (char *)aObj[ o_bcdcs ].Alloc( bc[q].Ld, 1, A_);
    }
    else
    {
        bc[q].PcDC = S_OFF;
        bc[q].SM = (char (*)[DC_RKLEN])aObj[ o_bcsmk ].Free();
        bc[q].CDcl = (char *)aObj[ o_bccdcl ].Free();
        bc[q].CD = (float *)aObj[ o_bccd ].Free();
        bc[q].DCS = (char *)aObj[ o_bcdcs ].Free();
    }

    if( bc[q].PcAU != S_OFF && bc[q].La > 0)
    {
        bc[q].SA = (char (*)[MAXFORMUNIT])aObj[ o_bcsa ].Alloc(bc[q].La, 1,MAXFORMUNIT);
        bc[q].AUcl = (char *)aObj[ o_bccacl ].Alloc( bc[q].La, 1, A_);
        bc[q].CA = (float *)aObj[ o_bcca ].Alloc( bc[q].La, 1, F_);
    }
    else
    {
        bc[q].PcAU = S_OFF;
        bc[q].SA = (char (*)[MAXFORMUNIT])aObj[ o_bcsa ].Free();
        bc[q].AUcl = (char *)aObj[ o_bccacl ].Free();
        bc[q].CA = (float *)aObj[ o_bcca ].Free();
    }
    if( bc[q].PcIC != S_OFF)
    {
        bc[q].SB = (char (*)[MAXICNAME+MAXSYMB])aObj[ o_bcsb ].Alloc( bc[q].N,
                   1, MAXICNAME+MAXSYMB);
        bc[q].CIcl = (char *)aObj[ o_bccicl ].Alloc(  bc[q].N, 1, A_);
        bc[q].CI = (float *)aObj[ o_bcci ].Alloc(  bc[q].N, 1, F_);
    }
    else
    {
        bc[q].PcIC = S_OFF;
        bc[q].SB = (char (*)[MAXICNAME+MAXSYMB])aObj[ o_bcsb ].Free();
        bc[q].CIcl = (char *)aObj[ o_bccicl ].Free();
        bc[q].CI = (float *)aObj[ o_bcci ].Free();
    }
    if( bc[q].PcFO != S_OFF )
        bc[q].CFOR = (char *)aObj[ o_bccfor  ].Alloc( 1, MAXCMPFORM , S_);
    else
        bc[q].CFOR = (char *)aObj[ o_bccfor  ].Free();

    if( bc[q].PcdC != S_OFF )
        bc[q].delC = (float *)aObj[ o_bcdelc].Alloc(  bc[q].N, 1, F_);
    else
        bc[q].delC = (float *)aObj[ o_bcdelc ].Free();

    if( bc[q].Nsd > 0 )
    {
        bc[q].sdref = (char (*)[V_SD_RKLEN])aObj[ o_bcsdref ].Alloc(bc[q].Nsd,
                      1, V_SD_RKLEN);
        bc[q].sdval = (char (*)[V_SD_VALEN])aObj[ o_bcsdval ].Alloc(bc[q].Nsd,
                      1, V_SD_VALEN);
    }
    else
    {
        bc[q].sdref = (char (*)[V_SD_RKLEN])aObj[ o_bcsdref ].Free();
        bc[q].sdval = (char (*)[V_SD_VALEN])aObj[ o_bcsdval ].Free();
    }
    // if( bc[q].tprn == 0 )
    //   bc[q].tprn = (char *)aObj[o_bctprn].Alloc( 1, 256, S_ );
}

//set default information

void TCompos::set_def( int q)
{
    ErrorIf( bcp!=&bc[q], GetName(), "Illegal access to bc in set_def.");
    TProfil *aPa=(TProfil *)(&aMod[RT_PARAM]);
    memcpy( &bc[q].PcIC, aPa->pa.BCpc, 6 );
    strcpy( bc[q].name, "element");
    strcpy( bc[q].notes, "`" );
    bc[q].N =0;
    bc[q].Ld =0;
    bc[q].La =0;
    bc[q].Nsd =0;
    bc[q].Nmax =0;
    bc[q].Nc  =0;
    //    float
    bc[q].Msys =0;
    bc[q].Vsys =0;
    bc[q].Mwat =0;
    bc[q].Maq  =0;
    bc[q].Vaq  =0;
    bc[q].Pg  =0;
    bc[q].R1  =0;
    bc[q].R2 =1;
    // pointers
    bc[q].C=0;
    bc[q].CFOR=0;
    bc[q].SB =0;
    bc[q].SA =0;
    bc[q].SM =0;
    bc[q].CIcl =0;
    bc[q].CDcl = 0;
    bc[q].AUcl = 0;
    bc[q].DCS =0;
    bc[q].CI = 0;
    bc[q].CD = 0;
    bc[q].CA = 0;
    bc[q].delC = 0;
    bc[q].sdref = 0;
    bc[q].sdval = 0;
    bc[q].A =0;
    bc[q].ICw = 0;
    bc[q].SB1 = 0;
    bc[q].tprn = 0;
}

// Input necessary data and link objects
void TCompos::RecInput( const char *key )
{
    TCModule::RecInput( key );
}

void TCompos::bc_work_dyn_new()
{
    // get all ICOMP
    // if( bcp->Nmax )
    {
        TCStringArray aKey;
        TCIntArray anR;
        int Nic = rt[RT_ICOMP].GetKeyList( "*:*:*:", aKey, anR );
        ErrorIf( Nic<1, GetName(),
                 "ICOMP data record keys are not selected \n"
                 "(maybe, some PDB chain files are not linked)");
        bcp->Nmax = (short)Nic;
        bcp->SB1 = (char (*)[IC_RKLEN])aObj[ o_bcsb1 ].Alloc(bcp->Nmax, 1, IC_RKLEN);
        for( int i=0; i<Nic; i++)
            memcpy( bcp->SB1[i], aKey[i].c_str(), IC_RKLEN);
    }
    bcp->ICw = (double *)aObj[ o_bcicw ].Alloc(bcp->Nmax, 1, D_);
    if( bcp->A )
        delete[] bcp->A;
    bcp->A = new double[bcp->Nmax];
    memset(bcp->A, 0, sizeof(double)*(bcp->Nmax) );

    // Realloc COMPOS: Inserted by DAK 22.10.99
    bcp->C = (double *)aObj[ o_bccv].Alloc( bcp->Nmax, 1, D_ );
    bcp->SB = (char (*)[MAXICNAME+MAXSYMB])aObj[ o_bcsb ].Alloc( bcp->Nmax,
              1, MAXICNAME+MAXSYMB);
    //

    if( C )
    {
        delete[] C;
        C=0;
    }
    if( CI )
    {
        delete[] CI;
        CI=0;
    }
    if( CIcl )
    {
        delete[] CIcl;
        CIcl=0;
    }
}

void TCompos::bc_work_dyn_kill()
{
    bcp->Nmax = 0;  // Restored by DAK 22.10.99
    bcp->SB1 = (char (*)[IC_RKLEN])aObj[ o_bcsb1 ].Free();
    bcp->ICw = (double *)aObj[ o_bcicw ].Free();
    delete[] bcp->A;
    bcp->A =0;
    if( C )
    {
        delete[] C;
        C=0;
    }
    if( CI )
    {
        delete[] CI;
        CI=0;
    }
    if( CIcl )
    {
        delete[] CIcl;
        CIcl=0;
    }
}

/* opens window with 'Remake record' parameters
*/
void
TCompos::MakeQuery()
{
//    pImp->MakeQuery();
    const char * p_key;
    char flgs[6];
    int size[2];
    double r2 = bcp->R2;

    p_key  = db->PackKey();
    memcpy( flgs, &bcp->PcIC, 6);
    size[0] = bcp->La;
    size[1] = bcp->Nsd;

    if( !vfComposSet( window(), p_key, flgs, size, r2 ))
         Error( p_key, "Compos record configuration cancelled by the user!" );
     //  return;   // cancel

    memcpy( &bcp->PcIC, flgs, 6);
    bcp->La = (short)size[0];
    bcp->Nsd = (short)size[1];
    bcp->R2 = (float)r2;

}


//Rebuild record structure before calc
int
TCompos::RecBuild( const char *key, int mode  )
{
    int oldIC=0, oldDC=0;
    vstr pkey(81);
    int i;
    gstring str;
    TCStringArray aIclist;
    TCStringArray aDclist;
    TCStringArray aRclist;
    TCStringArray aIclist_old;
    TCStringArray aDclist_old;
    TCStringArray aRclist_old;

//AGAIN_MOD:
    if( bcp->PcIC != S_OFF  ) oldIC = bcp->N;
    if( bcp->PcDC != S_OFF  ) oldDC = bcp->Ld;

    int ret = TCModule::RecBuild( key, mode );
    if( ret == VF3_3 && !( !bcp->PcIC || bcp->PcIC == ' ' ) )
        return ret;
    if( ret == VF3_1 )
    {
        strncpy( bcp->name, db->FldKey(2), db->FldLen(2));
        bcp->name[db->FldLen(2)] = '\0';
        oldIC = 0;
        oldDC = 0;
    }
/*
    if(  bcp->La < 0 ||  bcp->La > 64 || bcp->Nsd < 0 || bcp->Nsd > 4 )
        if( vfQuestion( window(), GetName(),
                        "Invalid number of formulae or SD references! Proceed?" ))
            goto AGAIN_MOD;
        else
            Error( GetName(), "Invalid number of formulae or SD references!");
*/
    bcp->N = 0;
    // select ICOMP
    // Build old selections
    aIclist_old.Clear();
    for( i=0; i<oldIC; i++ )
    {
      str = gstring( bcp->SB[i], 0, MAXICNAME+MAXSYMB );
      str += "*                     ";
      aIclist_old.Add( str );
    }
LOOP_MARKIC:
    aIclist = vfMultiKeysSet( window(),
       "Please, mark IComp keys for PCO definition",
       RT_ICOMP, "*:*:*:", aIclist_old );

    bcp->Nmax = (short)aIclist.GetCount();
    // must be Nic = rt[RT_ICOMP].GetKeyList("*:*:*:",.,.);
    bcp->PcIC = S_REM;
    if( aIclist.GetCount() < 1 )
        switch ( vfQuestion3( window(), GetName(),
         " < 1 IComp keys marked for PCO definition\n"
         "Repeat marking, Proceed to defining PCO in different ways, or\n"
         " Cancel assembling PCO definition?", "&Repeat", "&Proceed" ))
        {
        case VF3_2:
            bcp->PcIC = S_OFF;
            break;
        case VF3_1:
            goto LOOP_MARKIC;
        case VF3_3:
            Error( GetName(),
            " < 1 IComp keys selected for PCO definition...");
        }
    else
     {    bcp->PcIC = S_ON;
          bcp->N = (short)aIclist.GetCount(); /*   bc[q].Nmax = Nic;  */
     }

    //  select DCOMP if PcDC = S_ON
    if( bcp->PcDC == S_OFF)
    /*!vfQuestion( window(), GetName(),
    "Will DCOMP stoichiometry be used for COMPOS definition?" )*/
    {
        aDclist.Clear();
        goto COMP_COUNT; //goto RE_SELECT;
    }
    rt[RT_DCOMP].MakeKey( RT_COMPOS, pkey, K_ANY, K_ANY, K_ANY, K_ANY, K_END);
    aDclist_old.Clear();
    for( i=0; i<oldDC; i++ )
      if( bcp->DCS[i]  == SRC_DCOMP )
      {
        str = gstring( bcp->SM[i], 0, DC_RKLEN );
        aDclist_old.Add( str );
      }
LOOP_MARKDC:
    aDclist = vfMultiKeysSet( window(),
       " Please, mark DComp keys for use in PCO definition",
       RT_DCOMP, pkey, aDclist_old );

    // select REACT
    aRclist_old.Clear();
    for( i=0; i<oldDC; i++ )
      if( bcp->DCS[i]  == SRC_REACDC )
      {
        str = gstring( bcp->SM[i], 0, DC_RKLEN );
        aRclist_old.Add( str );
      }
    aRclist = vfMultiKeysSet( window(),
       "Please, mark ReacDC keys for use in PCO definition",
       RT_REACDC, pkey, aRclist_old );

    if( aRclist.GetCount() < 1 && aDclist.GetCount() < 1 )
        switch ( vfQuestion3( window(), GetName(),
                  " < 1 ReacDC&DComp keys marked for PCO.\n"
                  " Repeat marking,  \n"
                  "Proceed to defining composition in different ways or\n"
                  "Cancel assembling PCO definition?", "&Repeat", "&Proceed" ))
        {
        case VF3_1:
            goto LOOP_MARKDC;
        case VF3_2:   /*if( Nc )*/
            break;
        case VF3_3:
            Error( GetName(),
                " < 1 ReacDC records selected for PCO definition");
        }

COMP_COUNT:

    bcp->Ld = (short)(aRclist.GetCount()+aDclist.GetCount());
    if( bcp->Ld < 1 )
        bcp->PcDC = S_OFF;
    else bcp->PcDC = S_ON;
    if( bcp->La > 0 )
        bcp->PcAU = S_ON;
    else bcp->PcAU = S_OFF;
    if( bcp->PcFO != S_OFF )
        bcp->PcFO = S_ON;
    // realloc memory
    dyn_new();

    if( bcp->PcIC != S_OFF )
        for(uint l=0; l<aIclist.GetCount(); l++ )
        { // Get list IC
            memcpy( bcp->SB[l], aIclist[l].c_str(), MAXICNAME+MAXSYMB );
            if( !bcp->CIcl[l] || bcp->CIcl[l]==A_NUL )
                bcp->CIcl[l] = QUAN_MOL;
        }

    if( bcp->PcDC != S_OFF )
        for( i=0; i<bcp->Ld; i++ )
        {
          int Nr = aRclist.GetCount();
            if( i<Nr )
            {
                memcpy( bcp->SM[i], aRclist[i].c_str(), DC_RKLEN );
                bcp->DCS[i] = SRC_REACDC;
                if( !bcp->CDcl[i] || bcp->CDcl[i]==A_NUL )
                    bcp->CDcl[i] = QUAN_MOL;
            }
            else
            {
                memcpy( bcp->SM[i], aDclist[i-Nr].c_str(), DC_RKLEN );
                bcp->DCS[i] = SRC_DCOMP;
                if( !bcp->CDcl[i] || bcp->CDcl[i]==A_NUL)
                    bcp->CDcl[i] = QUAN_MOL;
            }
        }

    if( bcp->PcAU != S_OFF )
        for( i=0; i<bcp->La; i++ )
        { // set vector AUcl
            if( !bcp->AUcl[i] || bcp->AUcl[i]==A_NUL)
            {
                bcp->AUcl[i] = QUAN_MOL;
                bcp->SA[i][0] = '`';
            }
        }

    if( bcp->PcFO != S_OFF && !bcp->CFOR[0] )
    {
        bcp->CFOR[0] = '`';
        bcp->CFOR[1] = 0;
    }
    pVisor->Update();
    return ret;
}

// Conversion of concentration units to moles
//
double TCompos::Reduce_Conc( char UNITP, double Xe, double DCmw, double Vm,
    double R1, double Msys, double Mwat, double Vaq, double Maq, double Vsys )
{
    double Xincr = 0.;
    switch( UNITP )
    {  // Quantities
    case QUAN_MKMOL: /*'Y'*/
        Xincr = Xe / 1e6;
        goto FINISH;
    case QUAN_MMOL:  /*'h'*/
        Xincr = Xe / 1e3;
        goto FINISH;
    case QUAN_MOL:   /*'M'*/
        Xincr = Xe;
        goto FINISH;
    }
    if( DCmw > 1e-12 )
        switch( UNITP )
        {
        case QUAN_MGRAM: /*'y'*/
            Xincr = Xe / DCmw / 1e3;
            goto FINISH;
        case QUAN_GRAM:  /*'g'*/
            Xincr = Xe / DCmw;
            goto FINISH;
        case QUAN_KILO:  /*'G'*/
            Xincr = Xe * 1e3 / DCmw;
            goto FINISH;
        }
    /* Concentrations */
    if( fabs( R1 ) > 1e-12 )
        switch( UNITP )
        { // mole fractions relative to total moles in the system
        case CON_MOLFR:  /*'n'*/
            Xincr = Xe * R1;
            goto FINISH;
        case CON_MOLPROC:/*'N'*/
            Xincr = Xe / 100. * R1;
            goto FINISH;
        case CON_pMOLFR: /*'f'*/
            if( Xe > -1. && Xe < 15 )
                Xincr = pow(10., -Xe )* R1;
            goto FINISH;
        }
    if( fabs( Vsys ) > 1e-12 && Vm > 1e-12 )
        switch( UNITP )   /* Volumes */
        {
        case CON_VOLFR:  /*'v'*/
            Xincr = Xe * Vsys * 1e3 / Vm;
            goto FINISH;
        case CON_VOLPROC:/*'V'*/
            Xincr = Xe * Vsys * 10. / Vm;
            goto FINISH;
        case CON_pVOLFR: /*'u'*/
            if( Xe > -1. && Xe < 15 )
                Xincr = pow( 10., -Xe ) * Vsys / Vm;
            goto FINISH;
        }
    if( fabs( Msys ) > 1e-12 && DCmw > 1e-12 )
        switch( UNITP ) // Mass fractions relative to mass of the system
        {
        case CON_WTFR:   /*'w'*/
            Xincr = Xe * Msys * 1e3 / DCmw;
            goto FINISH;
        case CON_WTPROC: /*'%'*/
            Xincr = Xe * Msys *10. / DCmw;
            goto FINISH;
        case CON_PPM:    /*'P'*/
            Xincr = Xe * Msys / 1e3 / DCmw;
            goto FINISH;
        }
    if( fabs( Mwat ) > 1e-12 )
        switch( UNITP ) /* Molalities */
        {
        case CON_MOLAL:  /*'m'*/
            Xincr = Xe * Mwat;
            goto FINISH;
        case CON_MMOLAL: /*'i'*/
            Xincr = Xe / 1e3 * Mwat;
            goto FINISH;
        case CON_pMOLAL: /*'p'*/
            if( Xe > -1. && Xe < 15 )
                Xincr = pow( 10., -Xe ) * Mwat;
            goto FINISH;
        }
    if( fabs( Vaq ) > 1e-12 )
        switch( UNITP )  /* Molarities */
        {
        case CON_MOLAR:  /*'L'*/
            Xincr = Xe * Vaq;
            goto FINISH;
        case CON_MMOLAR: /*'j'*/
            Xincr = Xe * Vaq / 1e3;
            goto FINISH;
        case CON_pMOLAR: /*'q'*/
            if( Xe > -1. && Xe < 15 )
                Xincr = pow( 10., -Xe ) * Vaq;
            goto FINISH;
            /* g/l, mg/l, mkg/l */
        case CON_AQGPL: /*'d'*/
            Xincr = Xe * Vaq / DCmw;
            goto FINISH;
        case CON_AQMGPL: /*'e'*/
            Xincr = Xe * Vaq / DCmw / 1e3;
            goto FINISH;
        case CON_AQMKGPL: /*'b'*/
            Xincr = Xe * Vaq / DCmw / 1e6;
            goto FINISH;
        }
    if( fabs( Maq ) > 1e-12 && DCmw > 1e-12 )
        switch( UNITP )     /* Weight concentrations */
        {
        case CON_AQWFR:  /*'C'*/
            Xincr = Xe * Maq * 1e3 / DCmw;
            goto FINISH;
        case CON_AQWPROC:/*'c'*/
            Xincr = Xe * 10. * Maq / DCmw;
            goto FINISH;
        case CON_AQPPM:  /*'a'*/
            Xincr = Xe * Maq / 1e3 / DCmw;
            goto FINISH;
        }
    /* Error */
FINISH:
    return Xincr;
}


// Calculate record
void
TCompos::RecCalc( const char* key )
{
    int i, im,j, Ld, La, LdLa;
    short wps;
    double MsysC = 0., R1C = 0.;
    double Xincr, ICmw, DCmw;
    double *A;
    vstr ICs(MAXRKEYLEN+10), pkey(MAXRKEYLEN+10);
    char *Formula, *CIcl=0;
    time_t crt, tim;
    int i1;

    TFormula aFo;
    TIComp* aIC=(TIComp *)(&aMod[RT_ICOMP]);
    aIC->ods_link(0);
    TDComp* aDC=(TDComp *)(&aMod[RT_DCOMP]);
    aDC->ods_link(0);
    TReacDC* aRC=(TReacDC *)(&aMod[RT_REACDC]);
    aRC->ods_link(0);

    bc_work_dyn_new();  // allocate work arrays and set bcp->Nmax
SPECIFY_C:

	memset( pkey, 0, MAXRKEYLEN+9 );
    for( i=0; i<bcp->Nmax; i++ )
    {
        /* load molar mass */
        memcpy( pkey, bcp->SB1[i], IC_RKLEN );
        pkey[IC_RKLEN] = 0;
        aIC->TryRecInp( pkey, crt, 0 );
        /* atomic mass and valence*/
        if( IsFloatEmpty( aIC->icp->awt ))
            bcp->ICw[i] = 0.;
        else bcp->ICw[i] = (double)aIC->icp->awt;
        /* icp->val; */
    }
//   NormDoubleRound(bcp->ICw, bcp->Nmax, 7 );

    if( !C )
        C = new double[bcp->Nmax];
    fillValue( C, 0., bcp->Nmax );

    if( bcp->PcIC != S_OFF )
    { /*  Through IC */
        if( !CI )
        {
            CI =new double[bcp->Nmax];
            memset( CI, 0, sizeof(double)*bcp->Nmax );
            CIcl = new char[bcp->Nmax];
            memset( CIcl, QUAN_MOL, bcp->Nmax );
        }
        for( i=0; i<bcp->N; i++ )
        {
            if( !bcp->CI[i] || IsFloatEmpty( bcp->CI[i] ))
                continue;
            /* get index im */
            for( im=0; im<bcp->Nmax; im++ )
                if( !memcmp( bcp->SB1[im], bcp->SB[i], (MAXICNAME+MAXSYMB)-1 ))
                    goto IC_FOUND;
            Error( GetName(), "No get index im!");
IC_FOUND:
            CI[im] = (double)bcp->CI[i];
            CIcl[im] = bcp->CIcl[i];
            ICmw = bcp->ICw[im];
            Xincr = Reduce_Conc( CIcl[im], CI[im], ICmw, 1.0, bcp->R1,
                                 bcp->Msys, bcp->Mwat, bcp->Vaq, bcp->Maq, bcp->Vsys );
            /*  if( Xincr < 1e-15 )
                   continue;  30.4.96 */
            C[im] += Xincr;
            /* VaqC = 0., VsysC = 0., */
            /* MaqC += Xincr*ICmw;  */
            MsysC += Xincr*ICmw;
            R1C += Xincr;
        } /*   i */
        bcp->PcIC = S_ON;
    }
    /* Using DCOMP/REACDC formula */
    if( bcp->PcDC != S_OFF || bcp->PcAU != S_OFF || bcp->PcFO != S_OFF )
    { /*  from DC */
        A = bcp->A;
        Ld = bcp->Ld;
        La = bcp->La;
        LdLa = Ld+La;
        if( bcp->PcFO != S_OFF )
            LdLa++;
        for( j=0; j < LdLa; j++ )
        {
            if( j<Ld )
            {
                if( !bcp->CD[j] || IsFloatEmpty( bcp->CD[j] ))
                    continue;
                /* Read DCOMP or REACDC */
                memcpy( pkey, bcp->SM[j], DC_RKLEN );
                /* Except t/database code field */
                pkey[DC_RKLEN-MAXSYMB] = '*';
                if( bcp->DCS[j] == SRC_DCOMP )
                    aDC->TryRecInp( pkey, tim, 0 );
                if( bcp->DCS[j] == SRC_REACDC )
                    aRC->TryRecInp( pkey, tim, 0 );
                if( bcp->DCS[j] == SRC_REACDC )
                {  /*aRC->dyn_set(0);*/ Formula = aRC->rcp->form;
                }
                else
                { /*aDC->dyn_set(0);*/  Formula = aDC->dcp->form;
                }
            }
            else if( j<bcp->Ld+bcp->La )
            {
                if( !bcp->CA[j-Ld] || IsFloatEmpty( bcp->CA[j-Ld] ))
                    continue;  /*load formula */
                Formula = bcp->SA[j-Ld];
                Formula[MAXFORMUNIT-1] = 0;
            }
            else Formula = bcp->CFOR;
            if( Formula[0] == '`' )
                continue;
            // Xincr = 0.; //    DCmw = 0.;   // memset( A, 0, sizeof(double)*bcp->Nmax );
            // Get formula and analyze it
            aFo.SetFormula( Formula );
            aFo.Stm_line( bcp->Nmax, A, (char *)bcp->SB1, NULL );
            aFo.Reset();
            DCmw = MolWeight( bcp->Nmax, bcp->ICw, A );
 /*
            for(int ii=0; ii<aFo.GetIn(); ii++ )
            { // terms
                ICs[IC_RKLEN-1]=0;
                fillValue( ICs.p, ' ', IC_RKLEN-1 );
                memcpy( ICs, aFo.GetCn( ii ), MAXICNAME+MAXSYMB );
                aFo.fixup_ics( ICs );
                for( i=0; i<bcp->Nmax; i++ )
                    if( !memcmp( ICs, bcp->SB1[i], MAXICNAME+MAXSYMB ))
                    {
                        A[i] += aFo.GetSC(ii);
                        DCmw += A[i]* bcp->ICw[i];
                    }
            } // ii
*/
            if( j<Ld )
                Xincr = Reduce_Conc( bcp->CDcl[j], (double)bcp->CD[j], DCmw, 1.0, bcp->R1,
                       bcp->Msys, bcp->Mwat, bcp->Vaq, bcp->Maq, bcp->Vsys );
            else if( j<bcp->Ld+bcp->La )
                Xincr = Reduce_Conc( bcp->AUcl[j-Ld], (double)bcp->CA[j-Ld], DCmw, 1.0,
                  bcp->R1, bcp->Msys, bcp->Mwat, bcp->Vaq, bcp->Maq, bcp->Vsys );
            else
                Xincr = Reduce_Conc( bcp->PcRes, (double)bcp->R2, DCmw, 1.0,
                 bcp->R1, bcp->Msys, bcp->Mwat, bcp->Vaq, bcp->Maq, bcp->Vsys );
            /*   if( Xincr < 1e-15 )
                   continue;   30.4.96 */
            /* recalc stoichiometry */
            for( i=0; i<bcp->Nmax; i++ )
                if( A[i] )
                {
                    C[i] += Xincr*A[i]; /* calc control sum */
                    MsysC += Xincr * A[i] * bcp->ICw[i];
                    R1C += Xincr * A[i];
                }
            /* VaqC = 0., VsysC = 0.,
            if( fabs( bcp->Msys ) > 1e-12 )
               MsysC += Xincr*DCmw;
            if( fabs( bcp->Maq ) > 1e-7 )
               MaqC += Xincr*DCmw;
            if( fabs( bcp->R1 ) > 1e-12 )
               R1C += Xincr; */
        } /*  j */
        if( bcp->PcDC != S_OFF )
            bcp->PcDC = S_ON;
        if( bcp->PcAU != S_OFF )
            bcp->PcAU = S_ON;
        if( bcp->PcFO != S_OFF )
            bcp->PcFO = S_ON;
    }
    MsysC /= 1e3;

    /* Analyze control sum */
    if( fabs( bcp->R1 ) < 1e-12 )
        bcp->R1 = R1C;
    if( fabs( (double)bcp->R1 - R1C ) < 1e-8 || fabs( R1C ) < 1e-15 )
        /*Xincr = 1.*/;
    else
    { /* normalisation */
        Xincr = (double)bcp->R1 / R1C;
        MsysC = 0.0;
        for( i=0; i<bcp->Nmax; i++ )
            if( fabs( C[i] ) >= 1e-12 )
            {
                C[i] *= Xincr;
                MsysC += C[i]*bcp->ICw[i];
            }
        bcp->R1 = R1C;
        MsysC /= 1e3;
    }

    if( fabs( bcp->Msys ) < 1e-12 )
        bcp->Msys = MsysC;
    if( fabs( (double)bcp->Msys - MsysC ) < 1e-8 || fabs( MsysC ) < 1e-15 )
        /*Xincr = 1.*/;
    else
    { /* normalisation */
        Xincr = (double)bcp->Msys / MsysC;
        R1C = 0.0;
        for( i=0; i<bcp->Nmax; i++ )
            if( fabs( C[i] ) >= 1e-12 )
            {
                C[i] *= Xincr;
                R1C += C[i];
            }
        bcp->R1 = R1C;
    }

    for( wps=0, i=0; i<bcp->Nmax; i++ )
        if( fabs( C[i] ) >= 1e-12 )
            wps++;
    if( wps < 1 )
        goto SPECIFY_C;
    bcp->N = wps;
    /* Realloc Compos back */
    bcp->C = (double *)aObj[ o_bccv].Alloc( bcp->N, 1, D_ );
    bcp->SB = (char (*)[MAXICNAME+MAXSYMB])aObj[ o_bcsb ].Alloc( bcp->N,
              1, MAXICNAME+MAXSYMB);
    if( bcp->PcIC != S_OFF )
    {
        bcp->CIcl = (char *)aObj[ o_bccicl ].Alloc(  bcp->N, 1, A_);
        bcp->CI = (float *)aObj[ o_bcci ].Alloc(  bcp->N, 1, F_);
    }
    /* load vectors */
    for( i1=-1,i=0; i<bcp->Nmax; i++ )
    {
        if( fabs( C[i] ) < 1e-12 )
            continue;
        i1++;
        bcp->C[i1] = C[i];
        memcpy( bcp->SB[i1], bcp->SB1[i], MAXICNAME+MAXSYMB );
        if( bcp->PcIC != S_OFF )
        {
            bcp->CI[i1] = CI[i];
            bcp->CIcl[i1] = CIcl[i];
        }
    }
//NormDoubleRound(bcp->C, bcp->N, 15 ); // SD 22/07/2009
    bc_work_dyn_kill();
    TCModule::RecCalc(key);
}

// Help on Compos module  ( ? button )
void
TCompos::CmHelp()
{
    pVisor->OpenHelp( GEMS_PCO_HTML );  //  05.01.01
}


void TCompos::CopyRecords( const char * prfName, TCStringArray& aCMnoused,
            elmWindowData el_data, cmSetupData st_data )
{
    TCIntArray anR;
    TCStringArray aComp;
    aCMnoused.Clear();

    // open selected kernel files
    // db->OpenOnlyFromList(el_data.flNames);
    int fnum_ = db->GetOpenFileNum( prfName );

   // delete the equvalent keys
   TCStringArray aICkey_new;         // 30/11/2006
   aICkey_new.Clear();

    // get list of records
    db->GetKeyList( "*:*:*:", aComp, anR );

    //  test&copy  selected records
    // ( add to last key field first symbol from prfname )
    uint j;
    int i, ij, itmp;
    uint jj;
    for(uint ii=0; ii<aComp.GetCount(); ii++ )
    {

      // test the same component (overload) 30/11/2006
      gstring stt = aComp[ii].substr(0,MAXCMPNAME+MAXSYMB);
      for( j=0; j<aICkey_new.GetCount(); j++ )
        if( stt ==  aICkey_new[j])
       break;
     if( j<aICkey_new.GetCount() )
       continue;

     RecInput( aComp[ii].c_str() );
     //test record
     ij = 0;
     itmp = 0;
     for( i=0; i< bcp->N; i++ )
     {
      for( jj=0; jj<el_data.ICrds.GetCount(); jj++ )
         if( !memcmp( el_data.ICrds[jj].c_str(), bcp->SB[i], MAXICNAME+MAXSYMB))
            {  ij++;
               break;
            }

      for( jj=0; jj<el_data.oldIComps.GetCount(); jj++ )
         if( !memcmp( el_data.oldIComps[jj].c_str(),
                           bcp->SB[i], MAXICNAME+MAXSYMB))
            {  itmp++;
               break;
            }

     } // i

     if( !(ij==bcp->N))
     {
        if( ij>0)
          aCMnoused.Add( aComp[ii] );
       continue;                // no all icomp
     }
     if( (itmp>=bcp->N))
      continue;                // all icomp in template


     // !!! changing record key
     gstring str= gstring(db->FldKey( 2 ), 0, db->FldLen( 2 ));
     ChangeforTempl( str, st_data.from_templ,
                    st_data.to_templ, db->FldLen( 2 ));
        str += ":";
        gstring str1 = gstring(db->FldKey( 1 ), 0, db->FldLen( 1 ));
        str1.strip();
        str = str1 + ":" + str;
        str1 = gstring(db->FldKey( 0 ), 0, db->FldLen( 0 ));
        str1.strip();
        str = str1 + ":" + str;
        AddRecord( str.c_str(), fnum_ );
        aICkey_new.Add( stt );  // 30/11/2006
    }

    // close all no project files
    TCStringArray names1;
    names1.Add(prfName);
    db->OpenOnlyFromList(names1);
}

//Calculate molar mass from the stoichiometry line and atomic masses
double
TCompos::MolWeight( int N, double *ICaw, double *Smline )
{
    int i;
    double MW = 0.0;

    for( i=0; i<N; i++ )
        if( ICaw[i] && Smline[i] )
            MW += (ICaw[i]) * (Smline[i]);

    return( MW );
}


// ----------------------- End of m_compos.cpp ---------------------


