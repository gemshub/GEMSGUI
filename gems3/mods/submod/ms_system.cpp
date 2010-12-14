//-------------------------------------------------------------------
// $Id: ms_system.cpp 1391 2009-08-10 12:58:16Z gems $
//
// Implementation of TSyst class, config, load and pack functions
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

#include <math.h>
#include "m_param.h"
#include "m_syseq.h"
#include "v_object.h"
#include "service.h"
#include "visor.h"

const char A_EMPTY='`';

TSyst::TSyst( int nrt, RMULTS* mu_ ):
        TSubModule( nrt ), mup(mu_)
{
    set_def();
}

void TSyst::EvPageChanged(int nPage)
{
    if( sy.N == 0 )
        return; // no Equstat load
    switch(nPage)
    {
    case 3:
        ICompExit();
        break;
    case 2:
        PhaseExit();
        break;
    case 1:
        DCompExit();
        break;
    }
}

void TSyst::CellChanged(bool val )
{
    TSysEq::pm->CellChanged(val);
    TSubModule::CellChanged(val);
}

// link values to objects
void TSyst::ods_link( int /*q*/)
{
    // aObj[ o_symus].SetPtr( sy.mus );
    // aObj[ o_sypot].SetPtr( sy.pot );
    // aObj[ o_sysymb].SetPtr(  sy.symb );
    // aObj[ o_sytime].SetPtr(  sy.time_ );
    // aObj[ o_sywhat].SetPtr(  sy.what );
    // aObj[ o_syphst].SetPtr(  sy.PhmKey );       !!!!!! Check DOD
    // aObj[ o_syname].SetPtr(  sy.name );
    // aObj[ o_synotes].SetPtr( sy.notes );
    aObj[ o_sypmod].SetPtr( &sy.PE );   /*a 10*/
    aObj[ o_sypvec].SetPtr( &sy.PbIC ); /*a 28*/
    aObj[ o_syn   ].SetPtr( &sy.N );
    aObj[ o_syl   ].SetPtr( &sy.L );    /*i 6*/
    aObj[ o_syfi  ].SetPtr( &sy.Fi );   /*i 4*/
    aObj[ o_sylbc].SetPtr(  &sy.La );   /*i 4*/
    aObj[ o_sylns].SetPtr(  &sy.LO );   /*i 7*/
    aObj[ o_sypmm].SetPtr(  &sy.Pmin ); /*f 3*/
    aObj[ o_sytmm].SetPtr(  &sy.Tmin ); /*f 3*/
    aObj[ o_sytdev].SetPtr(  &sy.Tdev1 ); /*f 2*/
    aObj[ o_symsolids].SetPtr(  &sy.Msolids ); /*f 1*/
    aObj[ o_symass].SetPtr( &sy.Mbel ); /*f 6*/
    aObj[ o_syvol].SetPtr(  &sy.Vsys ); /*f 2*/
    aObj[ o_syproc].SetPtr( &sy.Time ); /*f 3*/
    aObj[ o_syl1].SetPtr(    sy.Ll );
    aObj[ o_syl1].SetDim( mup->Fi, 1 );
    aObj[ o_syrlc].SetPtr(   sy.RLC );
    aObj[ o_syrlc].SetDim( mup->L, 1 );
    aObj[ o_syrsc].SetPtr(   sy.RSC );
    aObj[ o_syrsc].SetDim( mup->L, 1 );
    aObj[ o_syrflc].SetPtr(  sy.RFLC );
    aObj[ o_syrflc].SetDim( mup->Fi, 1 );
    aObj[ o_syrfsc].SetPtr(  sy.RFSC );
    aObj[ o_syrfsc].SetDim( mup->Fi, 1 );
    aObj[ o_syicl].SetPtr(   sy.Icl );
    aObj[ o_syicl].SetDim( mup->N, 1 );
    aObj[ o_sydcl].SetPtr(   sy.Dcl );
    aObj[ o_sydcl].SetDim( mup->L, 1 );
    aObj[ o_sypcl].SetPtr(   sy.Pcl );
    aObj[ o_sypcl].SetDim( mup->Fi, 1 );
    aObj[ o_syacl].SetPtr(   sy.Acl );
    aObj[ o_syacl].SetDim( mup->La, 1 );
    aObj[ o_sybiun].SetPtr(  sy.BIun );
    aObj[ o_sybiun].SetDim( mup->N, 1 );
    aObj[ o_syxdun].SetPtr(  sy.XDun );
    aObj[ o_syxdun].SetDim( mup->L, 1 );
    aObj[ o_syxaun].SetPtr(  sy.XAun );
    aObj[ o_syxaun].SetDim( mup->La, 1 );
    aObj[ o_syxpun].SetPtr(  sy.XPun );
    aObj[ o_syxpun].SetDim( mup->Fi, 1 );
    aObj[ o_syb].SetPtr(     sy.B );
    aObj[ o_syb].SetDim( mup->N, 1 );
    aObj[ o_sydelb].SetPtr(  sy.delB );
    aObj[ o_sydelb].SetDim( mup->N, 1 );
    aObj[ o_sybi].SetPtr(    sy.BI );
    aObj[ o_sybi].SetDim( mup->N, 1 );
    aObj[ o_syxea].SetPtr(   sy.XeA );
    aObj[ o_syxea].SetDim( mup->La, 1 );
    aObj[ o_syxed].SetPtr(   sy.XeD );
    aObj[ o_syxed].SetDim( mup->L, 1 );
    aObj[ o_syphm].SetPtr(   sy.Phm );
    aObj[ o_syphm].SetDim( mup->Fi, 1 );
    aObj[ o_sypparc].SetPtr( sy.Pparc );
    aObj[ o_sypparc].SetDim( mup->L, 1 );
    aObj[ o_sysatc].SetPtr(  sy.SATC );
    aObj[ o_sysatc].SetDim( mup->Lads, MCAS );
//    aObj[ o_sysatc].SetDim( mup->Ls, 2 );

    aObj[ o_sydul].SetPtr(   sy.DUL );
    aObj[ o_sydul].SetDim( mup->L, 1 );
    aObj[ o_sydll].SetPtr(   sy.DLL );
    aObj[ o_sydll].SetDim( mup->L, 1 );
    aObj[ o_sygex].SetPtr(   sy.GEX );
    aObj[ o_sygex].SetDim( mup->L, 1 );
    aObj[ o_sypul].SetPtr(   sy.PUL );
    aObj[ o_sypul].SetDim( mup->Fi, 1 );
    aObj[ o_sypll].SetPtr(   sy.PLL );
    aObj[ o_sypll].SetDim( mup->Fi, 1 );
    aObj[ o_syyof].SetPtr(   sy.YOF );
    aObj[ o_syyof].SetDim( mup->Fi, 1 );
    aObj[ o_sylngmf].SetPtr( sy.lnGmf );
    aObj[ o_sylngmf].SetDim( mup->L, 1 );
    aObj[ o_symasdj].SetPtr( sy.MaSdj );
//    aObj[ o_symasdj].SetDim( mup->Ls, 1 );
    aObj[ o_symasdj].SetDim( mup->Lads, DFCN );  // changed 25.10.2004 by KD
    aObj[ o_syaalp].SetPtr(  sy.Aalp );
    aObj[ o_syaalp].SetDim( mup->Fi, 1 );
    aObj[ o_sysigm].SetPtr(  sy.Sigm );
    aObj[ o_sysigm].SetDim( mup->Fi, 2 );
    aObj[ o_syxr0h0].SetPtr( sy.Xr0h0 );
    aObj[ o_syxr0h0].SetDim( mup->Fi, 2 );
    aObj[ o_syxeps].SetPtr(  sy.XEpsC );
    aObj[ o_syxeps].SetDim( mup->Fi, 2 );
    aObj[ o_synfsp].SetPtr( sy.Nfsp );
    aObj[o_synfsp].SetDim( mup->Fis, sy.NsTm );
    aObj[ o_symasdt].SetPtr(sy.MaSdt);
    aObj[o_symasdt].SetDim(mup->Fis, sy.NsTm );
    aObj[ o_syxcapa].SetPtr(sy.XcapA );
    aObj[o_syxcapa].SetDim(mup->Fis, sy.NsTm );
    aObj[ o_syxcapb].SetPtr(sy.XcapB );
    aObj[o_syxcapb].SetDim(mup->Fis, sy.NsTm );
    aObj[ o_syxfiec].SetPtr(sy.XfIEC );
    aObj[o_syxfiec].SetDim(mup->Fis, sy.NsTm );
    aObj[ o_syscmt].SetPtr( sy.SCMT );
    aObj[o_syscmt].SetDim( mup->Fis, sy.NsTm );
    aObj[ o_syxcapf].SetPtr(sy.XcapF);
    aObj[o_syxcapf].SetDim(mup->Fis, sy.NsTm);
    aObj[ o_syxlam].SetPtr(sy.Xlam);
    aObj[o_syxlam].SetDim(mup->Fis, sy.NsTm );
    
    aObj[ o_syguns].SetPtr(   sy.Guns );
    aObj[ o_syguns].SetDim( mup->L, 1 );
    aObj[ o_syvuns].SetPtr(   sy.Vuns );
    aObj[ o_syvuns].SetDim( mup->L, 1 );
}

// set dynamic Objects ptr to values
void TSyst::dyn_set(int /*q*/)
{
    sy.Ll = (short *)aObj[ o_syl1 ].GetPtr();
    sy.RLC = (char *)aObj[ o_syrlc ].GetPtr();
    sy.RSC = (char *)aObj[ o_syrsc ].GetPtr();
    sy.RFLC = (char *)aObj[ o_syrflc ].GetPtr();
    sy.RFSC = (char *)aObj[ o_syrfsc ].GetPtr();
    sy.Icl = (char *)aObj[ o_syicl ].GetPtr();
    sy.Dcl = (char *)aObj[ o_sydcl ].GetPtr();
    sy.Pcl = (char *)aObj[ o_sypcl ].GetPtr();
    sy.Acl = (char *)aObj[ o_syacl ].GetPtr();
    sy.BIun = (char *)aObj[ o_sybiun ].GetPtr();
    sy.XDun = (char *)aObj[ o_syxdun ].GetPtr();
    sy.XAun = (char *)aObj[ o_syxaun ].GetPtr();
    sy.XPun = (char *)aObj[ o_syxpun ].GetPtr();
    sy.B = (double *)aObj[ o_syb ].GetPtr();
    sy.delB = (float *)aObj[ o_sydelb ].GetPtr();
    sy.BI = (double *)aObj[ o_sybi ].GetPtr();
    sy.XeA = (double *)aObj[ o_syxea ].GetPtr();
    sy.XeD = (double *)aObj[ o_syxed ].GetPtr();
    sy.Phm = (double *)aObj[ o_syphm ].GetPtr();
    sy.Pparc = (float *)aObj[ o_sypparc ].GetPtr();
    sy.SATC = (char (*)[MCAS])aObj[ o_sysatc ].GetPtr();

    sy.DUL = (float *)aObj[ o_sydul ].GetPtr();
    sy.DLL = (float *)aObj[ o_sydll ].GetPtr();
    sy.GEX = (float *)aObj[ o_sygex ].GetPtr();
    sy.PUL = (float *)aObj[ o_sypul ].GetPtr();
    sy.PLL = (float *)aObj[ o_sypll ].GetPtr();
    sy.YOF = (float *)aObj[ o_syyof ].GetPtr();
    sy.lnGmf = (float *)aObj[ o_sylngmf ].GetPtr();
    sy.MaSdj = (float (*)[DFCN])aObj[ o_symasdj ].GetPtr();
    sy.Aalp = (float *)aObj[ o_syaalp ].GetPtr();
    sy.Sigm = (float (*)[2])aObj[ o_sysigm ].GetPtr();
    sy.Xr0h0 = (float (*)[2])aObj[ o_syxr0h0 ].GetPtr();
    sy.XEpsC = (float (*)[2])aObj[ o_syxeps ].GetPtr();
    sy.Nfsp =  (float (*)[MNST])aObj[ o_synfsp ].GetPtr();
    sy.MaSdt = (float (*)[MNST])aObj[ o_symasdt ].GetPtr();
    sy.XcapA = (float (*)[MNST])aObj[ o_syxcapa ].GetPtr();
    sy.XcapB = (float (*)[MNST])aObj[ o_syxcapb ].GetPtr();
    sy.XfIEC = (float (*)[MNST])aObj[ o_syxfiec ].GetPtr();
    sy.SCMT  = (char (*)[MNST])aObj[ o_syscmt ].GetPtr();
    sy.XcapF = (float (*)[MNST])aObj[ o_syxcapf].GetPtr();
    sy.Xlam =  (float (*)[MNST])aObj[ o_syxlam].GetPtr();
    
    sy.Guns = (float *)aObj[ o_syguns ].GetPtr();
    sy.Vuns = (float *)aObj[ o_syvuns ].GetPtr();
}

// free dynamic memory in objects and values
void TSyst::dyn_kill(int /*q*/)
{
    sy.Ll = (short *)aObj[ o_syl1 ].Free();
    sy.RLC = (char *)aObj[ o_syrlc ].Free();
    sy.RSC = (char *)aObj[ o_syrsc ].Free();
    sy.RFLC = (char *)aObj[ o_syrflc ].Free();
    sy.RFSC = (char *)aObj[ o_syrfsc ].Free();
    sy.Icl = (char *)aObj[ o_syicl ].Free();
    sy.Dcl = (char *)aObj[ o_sydcl ].Free();
    sy.Pcl = (char *)aObj[ o_sypcl ].Free();
    sy.Acl = (char *)aObj[ o_syacl ].Free();
    sy.BIun = (char *)aObj[ o_sybiun ].Free();
    sy.XDun = (char *)aObj[ o_syxdun ].Free();
    sy.XAun = (char *)aObj[ o_syxaun ].Free();
    sy.XPun = (char *)aObj[ o_syxpun ].Free();
    sy.B = (double *)aObj[ o_syb ].Free();
    sy.delB = (float *)aObj[ o_sydelb ].Free();
    sy.BI = (double *)aObj[ o_sybi ].Free();
    sy.XeA = (double *)aObj[ o_syxea ].Free();
    sy.XeD = (double *)aObj[ o_syxed ].Free();
    sy.Phm = (double *)aObj[ o_syphm ].Free();
    sy.Pparc = (float *)aObj[ o_sypparc ].Free();
    sy.SATC = (char (*)[MCAS])aObj[ o_sysatc ].Free();

    sy.DUL = (float *)aObj[ o_sydul ].Free();
    sy.DLL = (float *)aObj[ o_sydll ].Free();
    sy.GEX = (float *)aObj[ o_sygex ].Free();
    sy.PUL = (float *)aObj[ o_sypul ].Free();
    sy.PLL = (float *)aObj[ o_sypll ].Free();
    sy.YOF = (float *)aObj[ o_syyof ].Free();
    sy.lnGmf = (float *)aObj[ o_sylngmf ].Free();
    sy.MaSdj = (float (*)[DFCN])aObj[ o_symasdj ].Free();
    sy.Aalp = (float *)aObj[ o_syaalp ].Free();
    sy.Sigm = (float (*)[2])aObj[ o_sysigm ].Free();
    sy.Xr0h0 = (float (*)[2])aObj[ o_syxr0h0 ].Free();
    sy.XEpsC = (float (*)[2])aObj[ o_syxeps ].Free();
    sy.Nfsp =  (float (*)[MNST])aObj[ o_synfsp ].Free();
    sy.MaSdt = (float (*)[MNST])aObj[ o_symasdt ].Free();
    sy.XcapA = (float (*)[MNST])aObj[ o_syxcapa ].Free();
    sy.XcapB = (float (*)[MNST])aObj[ o_syxcapb ].Free();
    sy.XfIEC = (float (*)[MNST])aObj[ o_syxfiec ].Free();
    sy.SCMT  = (char (*)[MNST])aObj[ o_syscmt ].Free();
    sy.XcapF = (float (*)[MNST])aObj[ o_syxcapf].Free();
    sy.Xlam =  (float (*)[MNST])aObj[ o_syxlam].Free();

    sy.Guns = (float *)aObj[ o_syguns ].Free();
    sy.Vuns = (float *)aObj[ o_syvuns ].Free();
}

// realloc dynamic memory
void TSyst::dyn_new(int /*q*/)
{
    ErrorIf( mup->L < 2, GetName(), "mup->L < 2");

    /* Necessary arrays */
    sy.Ll = (short *)aObj[ o_syl1].Alloc( mup->Fi, 1, I_ );
    sy.B = (double *)aObj[ o_syb].Alloc( mup->N, 1, D_ );
    sy.Icl = (char *)aObj[ o_syicl].Alloc( mup->N, 1, A_ );
    sy.Dcl = (char *)aObj[ o_sydcl].Alloc( mup->L, 1, A_ );
    sy.Pcl = (char *)aObj[ o_sypcl].Alloc( mup->Fi, 1, A_ );
    if( mup->La )
        sy.Acl = (char *)aObj[ o_syacl].Alloc( mup->La, 1, A_ );
    else sy.Acl = (char *)aObj[ o_syacl ].Free();

    // no default arrays
    /* compositions */
    if( sy.PbIC != S_OFF )
    {
        sy.BI = (double *)aObj[ o_sybi].Alloc( mup->N, 1, D_ );
        sy.BIun = (char *)aObj[ o_sybiun].Alloc( mup->N, 1, A_ );
    }
    else
    {
        sy.BI = (double *)aObj[ o_sybi ].Free();
        sy.BIun = (char *)aObj[ o_sybiun ].Free();
    }
    if( sy.PbIC != S_OFF && sy.PdelB != S_OFF )
        sy.delB = (float *)aObj[ o_sydelb].Alloc( mup->N, 1, F_);
    else sy.delB = (float *)aObj[ o_sydelb ].Free();

    if( sy.PbDC != S_OFF )
    {
        sy.XeD = (double *)aObj[ o_syxed].Alloc( mup->L, 1, D_ );
        sy.XDun = (char *)aObj[ o_syxdun].Alloc( mup->L, 1, A_ );
    }
    else
    {
        sy.XeD = (double *)aObj[ o_syxed ].Free();
        sy.XDun = (char *)aObj[ o_syxdun ].Free();
    }
    if( sy.PbAC != S_OFF && mup->La > 0 )
    {
        sy.XeA = (double *)aObj[ o_syxea].Alloc( mup->La, 1, D_);
        sy.XAun = (char *)aObj[ o_syxaun].Alloc( mup->La, 1, A_ );
    }
    else
    {
        sy.XAun = (char *)aObj[ o_syxaun ].Free();
        sy.XeA = (double *)aObj[ o_syxea ].Free();
    }
    if( sy.PbPH != S_OFF )
    {
        sy.Phm = (double *)aObj[ o_syphm].Alloc( mup->Fi, 1, D_ );
        sy.XPun = (char *)aObj[ o_syxpun].Alloc( mup->Fi, 1, A_ );
    }
    else
    {
        sy.Phm = (double *)aObj[ o_syphm ].Free();
        sy.XPun = (char *)aObj[ o_syxpun ].Free();
    }

    /* limits */
    if( sy.DLLim != S_OFF )
        sy.DLL = (float *)aObj[ o_sydll].Alloc( mup->L, 1, F_ );
    else sy.DLL = (float *)aObj[ o_sydll ].Free();

    if( sy.DULim != S_OFF )
        sy.DUL = (float *)aObj[ o_sydul].Alloc( mup->L, 1, F_ );
    else sy.DUL = (float *)aObj[ o_sydul ].Free();

    if( sy.DULim != S_OFF || sy.DLLim != S_OFF )
    {
        sy.RLC = (char *)aObj[ o_syrlc].Alloc( mup->L, 1, A_ );
        sy.RSC = (char *)aObj[ o_syrsc].Alloc( mup->L, 1, A_ );
    }
    else
    {
        sy.RLC = (char *)aObj[ o_syrlc ].Free();
        sy.RSC = (char *)aObj[ o_syrsc ].Free();
    }
    if( sy.PLLim != S_OFF )
        sy.PLL = (float *)aObj[ o_sypll].Alloc( mup->Fi, 1, F_ );
    else sy.PLL = (float *)aObj[ o_sypll ].Free();

    if( sy.PULim != S_OFF )
        sy.PUL = (float *)aObj[ o_sypul].Alloc( mup->Fi, 1, F_ );
    else sy.PUL = (float *)aObj[ o_sypul ].Free();

    if( sy.PULim != S_OFF || sy.PLLim != S_OFF )
    {
        sy.RFLC = (char *)aObj[ o_syrflc].Alloc( mup->Fi, 1, A_ );
        sy.RFSC = (char *)aObj[ o_syrfsc].Alloc( mup->Fi, 1, A_ );
    }
    else
    {
        sy.RFLC = (char *)aObj[ o_syrflc ].Free();
        sy.RFSC = (char *)aObj[ o_syrfsc ].Free();
    }

    /* add parameterd */
    if( sy.PYOF != S_OFF )
        sy.YOF = (float *)aObj[ o_syyof].Alloc( mup->Fi, 1, F_ );
    else sy.YOF = (float *)aObj[ o_syyof ].Free();

    if( sy.PParc != S_OFF )
        sy.Pparc = (float *)aObj[ o_sypparc].Alloc( mup->L, 1, F_ );
    else sy.Pparc = (float *)aObj[ o_sypparc ].Free();

    if( sy.PSATT != S_OFF )
        sy.SATC = (char (*)[MCAS])aObj[ o_sysatc].Alloc( mup->Lads, MCAS, A_ );
//        sy.SATC = (char (*)[2])aObj[ o_sysatc].Alloc( mup->Ls, 2, A_ );
    else sy.SATC = (char (*)[MCAS])aObj[ o_sysatc ].Free();

    if( sy.PGEX != S_OFF )
        sy.GEX = (float *)aObj[ o_sygex].Alloc( mup->L, 1, F_ );
    else sy.GEX = (float *)aObj[ o_sygex ].Free();

    if( sy.PMaSdj != S_OFF )
        sy.MaSdj = (float (*)[DFCN])aObj[ o_symasdj].Alloc( mup->Lads, DFCN, F_ );
//  sy.MaSdj = (float *)aObj[ o_symasdj].Alloc( mup->Ls, 1, F_ );
    else sy.MaSdj = (float (*)[DFCN])aObj[ o_symasdj ].Free();

    if( sy.PlnGf != S_OFF )
        sy.lnGmf = (float *)aObj[ o_sylngmf].Alloc( mup->L, 1, F_ );
    else sy.lnGmf = (float *)aObj[ o_sylngmf ].Free();

    /* Params of dispers phase */
    if( sy.PAalp != S_OFF )
        sy.Aalp = (float *)aObj[ o_syaalp].Alloc( mup->Fi, 1, F_ );
    else sy.Aalp = (float *)aObj[ o_syaalp ].Free();

    /* Modifications  */
    if( sy.PSigm != S_OFF )
        sy.Sigm = (float (*)[2])aObj[ o_sysigm].Alloc( mup->Fi, 2, F_ );
    else sy.Sigm = (float (*)[2])aObj[ o_sysigm ].Free();

    if( sy.PXepsC != S_OFF )
        sy.XEpsC = (float (*)[2])aObj[ o_syxeps].Alloc( mup->Fi, 2, F_ );
    else  sy.XEpsC = (float (*)[2])aObj[ o_syxeps ].Free();

    if( sy.PXr0h0 != S_OFF )
        sy.Xr0h0 = (float (*)[2])aObj[ o_syxr0h0].Alloc( mup->Fi, 2, F_ );
    else sy.Xr0h0 = (float (*)[2])aObj[ o_syxr0h0 ].Free();

    if( sy.PNfsp != S_OFF && sy.NsTm > 0 )
    {
        sy.Nfsp = (float (*)[MNST])aObj[ o_synfsp].Alloc(  mup->Fis, sy.NsTm, F_ );
        sy.SCMT = (char (*)[MNST])aObj[ o_syscmt].Alloc( mup->Fis, sy.NsTm, A_ );
    }
    else
    {
        sy.Nfsp =  (float (*)[MNST])aObj[ o_synfsp ].Free();
        sy.SCMT  = (char (*)[MNST])aObj[ o_syscmt ].Free();
    }

    if( sy.PMaSdt != S_OFF && sy.NsTm > 0 )
        sy.MaSdt = (float (*)[MNST])aObj[ o_symasdt].Alloc(  mup->Fis, sy.NsTm, F_ );
    else sy.MaSdt = (float (*)[MNST])aObj[ o_symasdt ].Free();

    if( sy.PXcapA != S_OFF && sy.NsTm > 0 )
        sy.XcapA = (float (*)[MNST])aObj[ o_syxcapa].Alloc(  mup->Fis, sy.NsTm, F_ );
    else sy.XcapA = (float (*)[MNST])aObj[ o_syxcapa ].Free();

    if( sy.PXcapB != S_OFF && sy.NsTm > 0 )
        sy.XcapB = (float (*)[MNST])aObj[ o_syxcapb].Alloc(  mup->Fis, sy.NsTm, F_ );
    else sy.XcapB = (float (*)[MNST])aObj[ o_syxcapb ].Free();

    if( sy.PXfIEC != S_OFF && sy.NsTm > 0 )
        sy.XfIEC = (float (*)[MNST])aObj[ o_syxfiec].Alloc(  mup->Fis, sy.NsTm, F_ );
    else sy.XfIEC = (float (*)[MNST])aObj[ o_syxfiec ].Free();

    if( sy.PXcapF != S_OFF && sy.NsTm > 0 )
        sy.XcapF = (float (*)[MNST])aObj[ o_syxcapf].Alloc(  mup->Fis, sy.NsTm, F_ );
    else sy.XcapF = (float (*)[MNST])aObj[ o_syxcapf ].Free();

    if( sy.PXlam != S_OFF && sy.NsTm > 0 )
        sy.Xlam = (float (*)[MNST])aObj[ o_syxlam].Alloc(  mup->Fis, sy.NsTm, F_ );
    else sy.Xlam = (float (*)[MNST])aObj[ o_syxlam ].Free();

    sy.Guns = (float *)aObj[ o_syguns ].Alloc( mup->L, 1, F_ );
    sy.Vuns = (float *)aObj[ o_syvuns ].Alloc( mup->L, 1, F_ );
}


//set default information
void TSyst::set_def( int /*q*/)
{
    // strcpy( sy.name, "`");
    // strcpy( sy.notes, "`");
    // memset( sy.PhmKey, 0, EQ_RKLEN );

    TProfil *aPa=(TProfil *)(&aMod[RT_PARAM]);

    bool is_a = false;
    bool is_x = false;
    bool is_g = false;
    if( pVisor->ProfileMode == true )
        for( int i=0; i<mup->Fi; i++)
        {
            if( mup->PHC[i] == PH_AQUEL )
                is_a = true;
            if( mup->PHC[i] == PH_SORPTION )
                is_x = true;
            if( mup->PHC[i] == PH_GASMIX || mup->PHC[i] == PH_PLASMA )
                is_g = true;
        }
    else is_x = true;
    memcpy( &sy.PE, aPa->pa.SYppc, 10 );
    memcpy( &sy.PbIC, aPa->pa.SYpvc, 28 );
    if( !is_a  && !is_x  && !is_g )
        sy.PE = '-';
    if( is_x )
    {
        sy.PBeq = '+';
        sy.PSATT = '+';
        sy.PMaSdj = '+';
        sy.PNfsp = '+';
        sy.PMaSdt = '+';
        sy.PXcapA = '+';
        sy.PXcapB = '+';
    }
    memset( &sy.N, 0, 22*sizeof(short));
    memset( &sy.Pmin, 0, 20*sizeof(float));
    if( is_x )
        sy.NsTm = MNST;


    sy.Ll = 0;
    sy.RLC = 0;
    sy.RSC = 0;
    sy.RFLC = 0;
    sy.RFSC = 0;
    sy.Icl = 0;
    sy.Dcl = 0;
    sy.Pcl = 0;
    sy.Acl = 0;
    sy.BIun = 0;
    sy.XDun = 0;
    sy.XAun = 0;
    sy.XPun = 0;
    sy.B = 0;
    sy.delB = 0;
    sy.BI = 0;
    sy.XeA = 0;
    sy.XeD = 0;
    sy.Phm = 0;
    sy.Pparc = 0;
    sy.SATC = 0;
    sy.DUL = 0;
    sy.DLL = 0;
    sy.GEX = 0;
    sy.PUL = 0;
    sy.PLL = 0;
    sy.YOF = 0;
    sy.lnGmf = 0;
    sy.MaSdj = 0;
    sy.Aalp = 0;
    sy.Sigm = 0;
    sy.Xr0h0 = 0;
    sy.XEpsC = 0;
    sy.Nfsp =  0;
    sy.MaSdt = 0;
    sy.XcapA = 0;
    sy.XcapB = 0;
    sy.XfIEC = 0;
    sy.SCMT  = 0;
    sy.XcapF = 0;
    sy.Xlam =  0;

    sy.Guns =  0;
    sy.Vuns =  0;

    //  memset ( sy.PhmKey, ' ', EQ_RKLEN-1 );  // Added 27.10.99 DAK
    //   sy.PhmKey[EQ_RKLEN-1] = 0;              //
}

// load data from TSysEq

void TSyst::loadData( bool newRec, int reBuildType )
{
    TSysEq* STat = (TSysEq*)(&aMod[RT_SYSEQ]);

    //  STat->ods_link(0);
    //Sveta 16/11/99  if( memcmp( &sy.PE, STat->ssp->switches, 38 ) )
    {
        memcpy( &sy.PE, STat->ssp->switches, 38 );
        memcpy( &sy.N, STat->ssp->DM, 22*sizeof(short) );
        memcpy( &sy.Pmin, STat->ssp->PPm, 20*sizeof(float) );
        dyn_new();
    }
    if( newRec == false )
    {
        setDefData();
        if( reBuildType != VF3_1 )
            unpackData();
    }
    else
    {
        /*TProfil *aPa=(TProfil *)aMod[RT_PARAM];
        for( int i=0; i<mup->N; i++ )
            sy.B[i] = aPa->pa.p.DB*10;  */
        // Sveta 17/05/99  if( !vfQuestion(window(), GetName(), "Retain previous SyStat definition?") )
        if( sy.PbPH != S_OFF && sy.XPun[mup->Fi-1] != QUAN_GRAM )
        {
            memset( sy.XPun, QUAN_GRAM, mup->Fi );
            memset( sy.Phm, 0, mup->Fi*sizeof(double) );
            //	  memcpy( sy.PhmKey, STat->ssp->PhmKey, EQ_RKLEN-1 );
        }
        // The above  block inserted by DAK 27.10.99
        int indk=0;
        if( sy.DLLim != S_OFF && sy.RSC[mup->L-1] != QUAN_MOL )
        {
           int i; indk=1;
           for( i=0; i<mup->L; i++ )
           {
               sy.DLL[i] = 0.;
               sy.RSC[i] = QUAN_MOL;
               if( sy.DULim != S_OFF )
                  sy.RLC[i] = BOTH_LIM;
               else sy.RLC[i] = LOWER_LIM;
           }
        }
        if( sy.DULim != S_OFF && ( indk == 1 || sy.RSC[mup->L-1] != QUAN_MOL ))
        {
           int i;
           for( i=0; i<mup->L; i++ )
           {
               sy.DUL[i] = 1e6;
               sy.RSC[i] = QUAN_MOL;
               if( sy.DLLim != S_OFF )
                  sy.RLC[i] = BOTH_LIM;
               else sy.RLC[i] = UPPER_LIM;
           }
        }
        // The above block inserted by KD on 13.05.02

        if( reBuildType == VF3_1 )
            setDefData();

        mark_ic_to_dc();
        mark_dc_to_ph();
        mark_ph_to_dc();
        mark_dc_to_ic();
        mark_ic_to_bc();
        make_syst_sizes();//         make_syst();
        contentsChanged = true;
    }
}

// Exit IComp window => change IComp selections
void TSyst::ICompExit()
{
    mark_ic_to_bc();
    mark_ic_to_dc();
    mark_dc_to_ph();
}

// Exit DComp window => change DComp selections
void TSyst::DCompExit()
{
    mark_dc_to_ph();
    mark_dc_to_ic();
    mark_ic_to_bc();
    // Sveta 17/05/99    set_aqu_gas_phase();
}

// Exit Phase window => change Phas selections

void TSyst::PhaseExit()
{
    // Sveta 17/05/99    set_aqu_gas_phase();
    mark_ph_to_dc();
    mark_dc_to_ic();
    mark_ic_to_bc();
}

// System test functions ( chek system arrays )
void TSyst::SyTest()
{
    //  if( contentsChanged == false )     // no changes
    //      return;
    if( strchr( rt[RT_SYSEQ].PackKey(), '*') != NULL )
        Error( "The system is not defined! ",
               "Please, create a new record key\n and specify bulk composition of the system");

    // Added as bugfix for System remake  08.02.2010 DK
    if( sy.PE != S_OFF && mup->ICC[mup->N-1] == IC_CHARGE )
           sy.Icl[mup->N-1] = S_ON;
    if( sy.PE == S_OFF && mup->ICC[mup->N-1] == IC_CHARGE )
           sy.Icl[mup->N-1] = S_OFF;

    mark_ic_to_dc();
    mark_dc_to_ph();
    mark_ph_to_dc();
    mark_dc_to_ic();
    mark_ic_to_bc();
    make_syst();
}

void TSyst::SyTestSizes()
{
    //  if( contentsChanged == false )     // no changes
    //      return;
    if( strchr( rt[RT_SYSEQ].PackKey(), '*') != NULL )
        Error( "The system is not defined! ",
               "Please, create a new record key\n and specify bulk composition of the system");
    mark_ic_to_dc();
    mark_dc_to_ph();
    mark_ph_to_dc();
    mark_dc_to_ic();
    mark_ic_to_bc();
    make_syst_sizes();
}

//#define ms(i,j) (sy.MaSdj[(j)+(i)*D_F_CD_NP])
// added 25.10.2004 by KD

// Set default informations to arrays
void TSyst::setDefData()
{
    int i, j, k;

    if( sy.B )
        memset( sy.B, 0, mup->N*sizeof(double) );
    if( mup->La && sy.PbAC != S_OFF )
    {
        memset( sy.Acl, S_REM, mup->La );
        memset( sy.XAun, QUAN_GRAM, mup->La );
        memset( sy.XeA, 0, mup->La*sizeof(double) );
    }
    if( sy.PbIC != S_OFF )
    {
        memset( sy.Icl, S_REM, mup->N );
        memset( sy.BIun, QUAN_MOL, mup->N );
        memset( sy.BI, 0, mup->N*sizeof(double) );
    }
    if( sy.PbIC != S_OFF && sy.PdelB != S_OFF )
        memset( sy.delB, 0, mup->N*sizeof(float) );
    if( sy.PbDC != S_OFF )
    {
        memset( sy.Dcl, S_REM, mup->L );
        memset( sy.XDun, QUAN_MOL, mup->L );
        memset( sy.XeD, 0, mup->L*sizeof(double) );
        sy.PbDC = S_ON;
    }
    memset( sy.Pcl, S_REM, mup->Fi );
    if( sy.PbPH != S_OFF )
    {
        //       memset( sy.Pcl, S_REM, mup->Fi );
        memset( sy.XPun, QUAN_GRAM, mup->Fi );
        memset( sy.Phm, 0, mup->Fi*sizeof(double) );
    }
    if( sy.DLLim != S_OFF )
    {
        for( i=0; i<mup->L; i++ )
        {
            sy.DLL[i] = 0.;
            sy.RSC[i] = QUAN_MOL;
            if( sy.DULim != S_OFF )
                sy.RLC[i] = BOTH_LIM;
            else sy.RLC[i] = LOWER_LIM;
        }
    }
    if( sy.DULim != S_OFF )
    {
        for( i=0; i<mup->L; i++ )
        {
            sy.DUL[i] = 1e6;
            sy.RSC[i] = QUAN_MOL;
            if( sy.DLLim != S_OFF )
                sy.RLC[i] = BOTH_LIM;
            else sy.RLC[i] = UPPER_LIM;
        }
    }
    if( sy.PLLim != S_OFF )
    {
        for( i=0; i<mup->Fi; i++ )
        {
            sy.PLL[i] = 0.;
            sy.RFSC[i] = QUAN_MOL;
            if( sy.PULim != S_OFF )
                sy.RFLC[i] = BOTH_LIM;
            else sy.RFLC[i] = UPPER_LIM;
        }
    }
    if( sy.PULim != S_OFF )
    {
        for( i=0; i<mup->Fi; i++ )
        {
            sy.PUL[i] = 1e6;
            sy.RFSC[i] = QUAN_MOL;
            if( sy.PLLim != S_OFF )
                sy.RFLC[i] = BOTH_LIM;
            else sy.RFLC[i] = UPPER_LIM;
        }
    }
    if( sy.PSATT != S_OFF )
        memset( sy.SATC, A_EMPTY, mup->Lads*MCAS );
//        memset( sy.SATC, A_EMPTY, /* SAT_INDEF,*/ mup->Ls*2 );
    if( sy.PMaSdj != S_OFF && sy.NsTm > 0 )
        for( i=0; i<mup->Lads; i++ )
           for( j=0; j < DFCN; j++ )
               sy.MaSdj[i][j] = FLOAT_EMPTY; // changed 25.10.2004 by KD
//             sy.MaSdj[i] = FLOAT_EMPTY;
    if( sy.PNfsp == S_REM && sy.NsTm > 0 )
    {
        memset( sy.SCMT, A_EMPTY, /* SC_NOT_USED,*/ mup->Fis*sy.NsTm );
        for( k=0; k<mup->Fis; k++ )
            for( i=0; i<sy.NsTm; i++ )
            {
                sy.Nfsp[k][i] = FLOAT_EMPTY;
                if( sy.PMaSdt != S_OFF ) sy.MaSdt[k][i] = FLOAT_EMPTY;
                if( sy.PXfIEC != S_OFF ) sy.XfIEC[k][i] = FLOAT_EMPTY;
                if( sy.PXlam != S_OFF ) sy.Xlam[k][i] = FLOAT_EMPTY;
                if( sy.PBeq != S_OFF )
                {
                    if( sy.PXcapA != S_OFF ) sy.XcapA[k][i] = FLOAT_EMPTY;
                    if( sy.PXcapB != S_OFF ) sy.XcapB[k][i] = FLOAT_EMPTY;
                    if( sy.PXcapF != S_OFF ) sy.XcapF[k][i] = FLOAT_EMPTY;
                }
            }
        /*......*/
    }
    if( sy.PYOF != S_OFF )
        memset( sy.YOF, 0, mup->Fi*sizeof(float) );
    if( sy.PParc != S_OFF )
        memset( sy.Pparc, 0, mup->L*sizeof(float) );
    if( sy.PGEX != S_OFF )
        memset( sy.GEX, 0, mup->L*sizeof(float) );
    if( sy.PlnGf != S_OFF )
        memset( sy.lnGmf, 0, mup->L*sizeof(float) );
    // load start data from phases
    phase_data_load();
    // set selected aqueous and gaseous phase
    // Sveta 17/05/99   set_aqu_gas_phase();
}

// Set default informations to arrays
void TSyst::setDefCheck()
{
    if( mup->La && sy.PbAC != S_OFF )
        memset( sy.Acl, S_ON, mup->La );
    if( sy.PbIC != S_OFF )
        memset( sy.Icl, S_ON, mup->N );
    if( sy.PbDC != S_OFF )
        memset( sy.Dcl, S_ON, mup->L );
    memset( sy.Pcl, S_ON, mup->Fi );
 }

// Load default informations from phase records into arrays

void TSyst::phase_data_load()
{
    time_t crt;
    TPhase* aPH=(TPhase *)(&aMod[RT_PHASE]);
    aPH->ods_link(0);

    for(short k=0; k<mup->Fi; k++ )
    {
        aPH->TryRecInp( mup->SF[k], crt, 0 );
        switch( mup->PHC[k] )
        { /* Load begin parametres */
        case PH_AQUEL:
        case PH_FLUID:
        case PH_GASMIX:
        case PH_PLASMA:
        case PH_HCARBL:
        case PH_LIQUID:
        case PH_SINCOND:
        case PH_SIMELT:
            break;
        case PH_POLYEL:
        case PH_SORPTION:
            sorption_data_load( aPH, k );
        case PH_SINDIS:
/*            if( sy.PAalp != S_OFF )
                sy.Aalp[k] = aPH->php->Asur;
            if( sy.PSigm != S_OFF )
            {
                sy.Sigm[k][0] = aPH->php->Sigma0;
                sy.Sigm[k][1] = aPH->php->SigmaG;
            }
            if( sy.PXr0h0 != S_OFF )
            {
                sy.Xr0h0[k][0] = aPH->php->R0p;
                sy.Xr0h0[k][1] = aPH->php->h0p;
            }
            if( sy.PXepsC != S_OFF )
            {
                sy.XEpsC[k][0] = aPH->php->Eps;
                sy.XEpsC[k][1] = aPH->php->Cond;
            } */
            break;
        default:
            ; /* Error ! */
        }
        // Moved by DK on 25.07.2006
        if( sy.PAalp != S_OFF )
            sy.Aalp[k] = aPH->php->Asur;
        if( sy.PSigm != S_OFF )
        {
            sy.Sigm[k][0] = aPH->php->Sigma0;
            sy.Sigm[k][1] = aPH->php->SigmaG;
        }
        if( sy.PXr0h0 != S_OFF )
        {
            sy.Xr0h0[k][0] = aPH->php->R0p;
            sy.Xr0h0[k][1] = aPH->php->h0p;
        }
        if( sy.PXepsC != S_OFF )
        {
            sy.XEpsC[k][0] = aPH->php->Eps;
            sy.XEpsC[k][1] = aPH->php->Cond;
        }
    }  /* k */
}

//#define mp(i,j) (aPH->php->MaSdj[(j)+(i)*D_F_CD_NP])
// Added on 25.10.2004 by KD

// Load default informations from sorption phases
void TSyst::sorption_data_load( TPhase* aPH, int k )
{
    int ist, i, j, jb=0, je, jp;
    unsigned ii, pnDC;

    for( ist=0; ist<sy.NsTm; ist++ )
    {
        if( ist < aPH->php->NsiT )
        {
            if( sy.PNfsp != S_OFF )
            {
                sy.Nfsp[k][ist] = aPH->php->FsiT[ist];
                sy.SCMT[k][ist] = aPH->php->SCMC[ist];
                /*       sy.CSTS[k][ist] = aPH->php->SCMT[ist][1]; */
                sy.MaSdt[k][ist] = aPH->php->MSDT[ist][0];
                //       sy.MiSdt[k][ist] = aPH->php->MSDT[ist][1];
            }

            if( sy.PXfIEC != S_OFF )
                sy.XfIEC[k][ist] = aPH->php->XfIEC[ist];
            if( sy.PXcapA != S_OFF )
                sy.XcapA[k][ist] = aPH->php->CapT[ist][0];
            if( sy.PXcapB != S_OFF )
                sy.XcapB[k][ist] = aPH->php->CapT[ist][1];
        }
        else
        {
            if( sy.PNfsp != S_OFF )
            {
                sy.Nfsp[k][ist] = FLOAT_EMPTY;
                /*     sy.CSTS[k][ist] = CCA_VOL;  */
                sy.SCMT[k][ist] = SC_NOT_USED;
                sy.MaSdt[k][ist] = FLOAT_EMPTY;
                //       sy.MiSdt[k][ist] = FLOAT_EMPTY;
            }

            if( sy.PXfIEC != S_OFF )
                sy.XfIEC[k][ist] = FLOAT_EMPTY;
            if( sy.PXcapA != S_OFF )
                sy.XcapA[k][ist] = FLOAT_EMPTY;
            if( sy.PXcapB != S_OFF )
                sy.XcapB[k][ist] = FLOAT_EMPTY;
        }
    } /* end for ist */
    if( sy.PSATT != S_OFF && sy.PMaSdj != S_OFF )
    {  /* Load arrays SATC_ and MaSdj_ */
      int ja, kk;
        pnDC = aPH->php->nDC;
// Calculate indexes of begin and end of list of DC in current phase in RMULTS
        for( i=0; i<k; i++ )
            jb += mup->Ll[i];
        je = jb + mup->Ll[k];       // Ls
        for( j=jb; j<je; j++ )
        { /* Check dependent component and search it's name in phase */
            for( ii=0; ii<pnDC; ii++ )
            {
                if( !memcmp( mup->SM[j], aPH->php->SM[ii], DC_RKLEN-4 ) )
                    break;
            }
            if( ii>=pnDC )
                continue;
            jp = ii;
            ja = j  - (mup->Ls - mup->Lads); // index in SATC_ and MaSdj_
            /* data copy */
            for(kk=0; kk<MCAS; kk++)
              sy.SATC[ja][kk]  = aPH->php->SATC[jp][kk];
//            sy.SATC[j][0] = aPH->php->SATC[jp][0];
//            sy.SATC[j][1] = aPH->php->SATC[jp][1];
// Extended by KD on 25.10.2004
            for(kk=0; kk<DFCN; kk++)
              sy.MaSdj[ja][kk]  = aPH->php->MaSdj[jp][kk];
//            sy.MaSdj[j][PI_DEN]  = aPH->php->MaSdj[jp][PI_DEN];
//            sy.MaSdj[j] = aPH->php->MaSdj[jp];
        }  /* end j */
    }
}

// Mark selected aqueous and gaseous phase
void TSyst::set_aqu_gas_phase()
{
    for(short k=0; k<mup->Fi; k++ )
    {
        if( *mup->SF[k] == 'a' || *mup->SF[k] == 'g' )
            sy.Pcl[k] = S_OFF;
    }
    sy.Pcl[mup->nAq] = S_ON;
    sy.Pcl[mup->nGas] = S_ON;
    mark_ph_to_dc();
}

//#define mt(i,j) (STat->ssp->MaSdj[(j)+(i)*D_F_CD_NP])
// Added on 25.10.2004 by KD

// unpack data from TSysEq record
void TSyst::unpackData()
{
    short i,j;
    int ind;

    TSysEq* STat = (TSysEq*)(&aMod[RT_SYSEQ]);
    TProfil *Prf = (TProfil*)(&aMod[RT_PARAM]);
    //   STat->ods_link(0);
    // copy static data
    memcpy( &sy.N, STat->ssp->DM, 22*sizeof(short) );
    memcpy( &sy.Pmin, STat->ssp->PPm, 20*sizeof(float) );
    // copy dynamic data
    if( sy.PbIC != S_OFF )
        for( i=0; i<sy.Na; i++)       // nnc no zero in BI
        {
            sy.BIun[STat->ssp->nnc[i]] = STat->ssp->BIun[i];
            sy.BI[STat->ssp->nnc[i]] = STat->ssp->BI[i];
        }
    if( sy.PbAC != S_OFF && mup->La > 0 )
        for( i=0; i<sy.La; i++)       // llc no zero in XeA
        {
            sy.Acl[STat->ssp->llc[i]] = STat->ssp->Acl[i];
            sy.XAun[STat->ssp->llc[i]] = STat->ssp->XAun[i];
            sy.XeA[STat->ssp->llc[i]] = STat->ssp->XeA[i];
        }
    if( sy.PbPH != S_OFF )
        for( i=0; i<sy.Fib; i++)       // phc no zero in Phm
        {
            ind = Prf->indPH( STat->ssp->phc[i] );
            if( ind < 0 ) continue;
            sy.XPun[ind] = STat->ssp->XPun[i];
            sy.Phm[ind] = STat->ssp->Phm[i];
        }
    if( sy.PbDC != S_OFF )
        for( i=0; i<sy.Lb; i++)       // dcc no zero in XeD
        {
            ind = Prf->indDC( STat->ssp->dcc[i] );
            if( ind < 0 ) continue;
            sy.XDun[ind] = STat->ssp->XDun[i];
            sy.XeD[ind] = STat->ssp->XeD[i];
        }
    memset( sy.Icl, S_OFF, mup->N );
    for( i=0; i<sy.N; i++)       // nnf IC ON in system
    {
        sy.Icl[STat->ssp->nnf[i]] = S_ON;
        sy.B[STat->ssp->nnf[i]] = STat->ssp->B[i];
        if( sy.PdelB != S_OFF && sy.PbIC != S_OFF )
            sy.delB[STat->ssp->nnf[i]] = STat->ssp->delB[i];
    }
    memset( sy.Pcl, S_ON, mup->Fi );
    for( i=0; i<STat->ssp->DM[22]; i++)       // phf phase OFF in system
    {
        ind = Prf->indPH( STat->ssp->phf[i] );
        if( ind < 0 ) continue;
        sy.Pcl[ind] = S_OFF;
    }
    memset( sy.Dcl, S_ON, mup->L );
    for( i=0; i<STat->ssp->DM[23]; i++)       // dcf dcomp OFF in system
    {
        ind = Prf->indDC( STat->ssp->dcf[i] );
        if( ind < 0 ) continue;
        sy.Dcl[ind] = S_OFF;
    }

//  for( i=0; i<sy.Fia; i++) // pha Phase ON adsorbtion  ( mup->PHC[]=='x' )
    for( i=0; i<STat->ssp->DM[20]; i++)
    {
        ind = Prf->indPH( STat->ssp->pha[i] );
        if( ind < 0 ) continue;
        for( j=0; j<sy.NsTm; j++ )
        {
            if( sy.PNfsp != S_OFF )
                sy.SCMT[ind][j] = STat->ssp->SCMT[i][j];
            if( sy.PNfsp != S_OFF )
                sy.Nfsp[ind][j] = STat->ssp->Nfsp[i][j];
            if( sy.PMaSdt != S_OFF )
                sy.MaSdt[ind][j] = STat->ssp->MaSdt[i][j];
            if( sy.PXcapA != S_OFF )
                sy.XcapA[ind][j] = STat->ssp->XcapA[i][j];
            if( sy.PXcapB != S_OFF )
                sy.XcapB[ind][j] = STat->ssp->XcapB[i][j];
            if( sy.PXcapF != S_OFF )
                sy.XcapF[ind][j] = STat->ssp->XcapF[i][j];
            if( sy.PXfIEC != S_OFF )
                sy.XfIEC[ind][j] = STat->ssp->XfIEC[i][j];
            if( sy.PXlam != S_OFF )
                sy.Xlam[ind][j] = STat->ssp->Xlam[i][j];
        }
    }
//   for( i=0; i<sy.Lsor; i++) dca dcomp ON adsorption ( mup->DCC[]=='X' )
    for( i=0; i<STat->ssp->DM[4]; i++)
    {
        ind = Prf->indDC( STat->ssp->dca[i] ) - ( mup->Ls - mup->Lads );
        if( ind < 0 )
           continue;
        if( sy.PSATT != S_OFF )
        {
            int mtM, msM, /* mtN,  msN, */kk;
            msM = aObj[o_sysatc].GetM();
            mtM = aObj[o_sssatc].GetM();
//            msN = aObj[o_sysatc].GetN();
//            mtN = aObj[o_sssatc].GetN();
            if(mtM >= msM)
            {
               for( kk=0; kk<MCAS; kk++ )
                    sy.SATC[ind][kk] = STat->ssp->SATC[i][kk];
            }
            else
            {  char * satc_ = &STat->ssp->SATC[0][0];
               sy.SATC[ind][SA_MCA] = satc_[ i*2 + SA_MCA ];
               sy.SATC[ind][SA_EMX] = satc_[ i*2 + SA_EMX ];
//               sy.SATC[ind][0] = STat->ssp->SATC[i][0];
//               sy.SATC[ind][1] = STat->ssp->SATC[i][1];
            }
        }
        if( sy.PMaSdj != S_OFF )
// Extended by KD on 25.10.2004
        {
            int mtM, msM, /* mtN,  msN,*/ kk ;
            msM = aObj[o_symasdj].GetM();
            mtM = aObj[o_ssmasdj].GetM();
//            msN = aObj[o_symasdj].GetN();
//            mtN = aObj[o_ssmasdj].GetN();
            if(mtM >= msM)
            {
               for( kk=0; kk<DFCN; kk++ )
                  sy.MaSdj[ind][kk] = STat->ssp->MaSdj[i][kk];
//               sy.MaSdj[ind][PI_CD0] = STat->ssp->MaSdj[i][PI_CD0];
            }
            else { // For reading old SysEq records
               float *masdj_ = &STat->ssp->MaSdj[0][0];
               sy.MaSdj[ind][PI_DEN]  = masdj_[i*mtM/*2*/];     // check if *2
//            sy.MaSdj[ind]  = STat->ssp->MaSdj[i];
            }
        }
    }
    if( sy.PULim != S_OFF || sy.PLLim != S_OFF )
        for( i=0; i<sy.Fik; i++) // phk  no zero in PUL/PLL
        {
            ind = Prf->indPH( STat->ssp->phk[i] );
            if( ind < 0 ) continue;

            if( sy.PULim != S_OFF )
                sy.PUL[ind] = STat->ssp->PUL[i];
            if( sy.PLLim != S_OFF )
                sy.PLL[ind] = STat->ssp->PLL[i];
            sy.RFLC[ind] = STat->ssp->RFLC[i];
            sy.RFSC[ind] = STat->ssp->RFSC[i];
        }
    if( sy.DULim != S_OFF || sy.DLLim != S_OFF )
        for( i=0; i<sy.Lk; i++) // dck  no zero in DUL/DLL
        {
            ind = Prf->indDC( STat->ssp->dck[i] );
            if( ind < 0 ) continue;

            if( sy.DULim != S_OFF )
                sy.DUL[ind] = STat->ssp->DUL[i];
            if( sy.DLLim != S_OFF )
                sy.DLL[ind] = STat->ssp->DLL[i];
            sy.RLC[ind] = STat->ssp->RLC[i];
            sy.RSC[ind] = STat->ssp->RSC[i];
        }
    for( i=0; i<sy.Le; i++) // dce  no zero in GEX/lnGmf
    {
        ind = Prf->indDC( STat->ssp->dce[i] );
        if( ind < 0 ) continue;

        if( sy.PGEX != S_OFF )  sy.GEX[ind] = STat->ssp->GEX[i];
        if( sy.PlnGf != S_OFF ) sy.lnGmf[ind] = STat->ssp->lnGmf[i];
        if( sy.PParc != S_OFF ) sy.Pparc[ind] = STat->ssp->Pparc[i];
    }
    for( i=0; i<sy.Fie; i++) // phe  no zero in YOF/Aalp
    {
        ind = Prf->indPH( STat->ssp->phe[i] );
        if( ind < 0 ) continue;

        if( sy.PAalp != S_OFF ) sy.Aalp[ind] = STat->ssp->Aalp[i];
        if( sy.PYOF != S_OFF )  sy.YOF[ind] = STat->ssp->YOF[i];
        if( sy.PSigm != S_OFF )
        {
            sy.Sigm[ind][0] = STat->ssp->Sigm[i][0];
            sy.Sigm[ind][1] = STat->ssp->Sigm[i][1];
        }
        if( sy.PXr0h0 != S_OFF )
        {
            sy.Xr0h0[ind][0] = STat->ssp->Xr0h0[i][0];
            sy.Xr0h0[ind][1] = STat->ssp->Xr0h0[i][1];
        }
        if( sy.PXepsC != S_OFF )
        {
            sy.XEpsC[ind][0] = STat->ssp->XEpsC[i][0];
            sy.XEpsC[ind][1] = STat->ssp->XEpsC[i][1];
        }
    }
    // set sy.Ll
    short ii=0, Nn/*, k*/;
    for( i=0, j=0/*, k=0*/; i<mup->Fi; i++ )
    {
        Nn = mup->Ll[i];
        ii+=Nn;
        sy.Ll[i] = 0;
        for(; j<ii; j++ )
            if( sy.Dcl[j] != S_OFF )
            {
                sy.Ll[i]++;
                /*k++;*/
            }
    }
    sy.LO = (short)Prf->indDC( sy.LO );

}

// packed system arrays (get size and alloc memory)
void TSyst::setSizes()
{
    short i;
    TProfil *aPa=(TProfil *)(&aMod[RT_PARAM]);
    TSysEq* STat = (TSysEq*)(&aMod[RT_SYSEQ]);
    //  STat->ods_link(0); did before

    // ICOMP
    sy.Na= sy.N = 0;
    for( i=0; i<mup->N; i++)
    {
        if( sy.PbIC != S_OFF )
            if( fabs(sy.BI[i])  > aPa->pa.p.DB )
                sy.Na++;
        if( sy.Icl[i] == S_ON )
            sy.N++;
    }
    // COMPOS
    sy.La=0;
    if( sy.PbAC != S_OFF && mup->La > 0 )
        for( i=0; i<mup->La; i++)
        {
            if( sy.Acl[i] != S_OFF && (  fabs(sy.XeA[i])  > aPa->pa.p.DB ))
                sy.La++;
        }
    // PHASE
    sy.Fib = sy.Fia = sy.Fie = sy.Fik = 0;
    STat->ssp->DM[22] = 0;
    for( i=0; i<mup->Fi; i++)
    {
        if( sy.PbPH != S_OFF )
            if( fabs(sy.Phm[i])  > aPa->pa.p.DB )
                sy.Fib++;
        if( sy.Pcl[i] == S_OFF)
            STat->ssp->DM[22]++;
        if( ( sy.PULim != S_OFF && fabs(sy.PUL[i]) > aPa->pa.p.DB )||
                ( sy.PLLim != S_OFF && fabs(sy.PLL[i]) > aPa->pa.p.DB ))
            sy.Fik++;
        if( ( sy.PAalp != S_OFF && fabs(sy.Aalp[i]) > aPa->pa.p.DB )||
                ( sy.PYOF != S_OFF && fabs(sy.YOF[i]) > aPa->pa.p.DB ))
            sy.Fie++;
        if( i >= mup->Fis )
            continue;
        if( sy.Pcl[i] != S_OFF && mup->PHC[i] == 'x' )
            sy.Fia++;
    }
    // DCOMP
    sy.Lb = sy.Lsor = sy.Le = sy.Lk = 0;
    STat->ssp->DM[23] = 0;
    for( i=0; i<mup->L; i++)       // llc no zero in XeA
    {
        if( sy.PbDC != S_OFF )
            if( fabs(sy.XeD[i])  > aPa->pa.p.DB )
                sy.Lb++;
        if( sy.Dcl[i] == S_OFF)
            STat->ssp->DM[23]++;
        if( ( sy.PGEX != S_OFF && fabs(sy.GEX[i]) > aPa->pa.p.DB )||
                ( sy.PlnGf != S_OFF && fabs(sy.lnGmf[i]) > aPa->pa.p.DB ))
            sy.Le++;
        if( ( sy.DULim != S_OFF && sy.DUL[i] < 1e6/*> aPa->pa.p.DB*/ )||
                ( sy.DLLim != S_OFF && sy.DLL[i] > 0/*aPa->pa.p.DB */))
            sy.Lk++;

        if( i >= mup->Ls )
            continue;

        if( sy.Dcl[i] != S_OFF && (strchr( SORPTION_DC, mup->DCC[i] ) != NULL ) )
            sy.Lsor++;
    }
    // copy static data
    memcpy( STat->ssp->switches, &sy.PE,38*sizeof(char) );
    memcpy( STat->ssp->DM, &sy.N,22*sizeof(short) );
    memcpy( STat->ssp->PPm, &sy.Pmin, 20*sizeof(float) );
    //  STat->dyn_new();
}

// packed system arrays (get size and alloc memory)

void TSyst::packData()
{
    short i, i1, i2, i3, i4, i5, j;
    TProfil *aPa=(TProfil *)(&aMod[RT_PARAM]);
    TSysEq* STat = (TSysEq*)(&aMod[RT_SYSEQ]);

    // ICOMP
    for( i=0, i1=0, i2= 0; i<mup->N; i++)
    {
        if( sy.PbIC != S_OFF )
             if( fabs(sy.BI[i])  > aPa->pa.p.DB ) // nnc no zero in BI
            {
                ErrorIf( i1>= sy.Na, GetName(), "sy.Na illegal count" );
                STat->ssp->nnc[i1] = i;
                STat->ssp->BIun[i1] = sy.BIun[i];
                STat->ssp->BI[i1++]   = sy.BI[i];
            }
        if( sy.Icl[i] == S_ON )  // nnf IC ON in system
        {
            ErrorIf( i2>= sy.N, GetName(), "sy.N illegal count" );
            STat->ssp->nnf[i2] = i;
            STat->ssp->B[i2]   = sy.B[i];
            if( sy.PdelB != S_OFF && sy.PbIC != S_OFF )
                STat->ssp->delB[i2] = sy.delB[i];
            i2++;
        }
    }
    // COMPOS
    if( sy.PbAC != S_OFF && mup->La > 0 )
        for( i=0, i1=0; i<mup->La; i++) // llc no zero in XeA
            if( sy.Acl[i] != S_OFF && (fabs(sy.XeA[i])>aPa->pa.p.DB ))
            {
                ErrorIf( i1>= sy.La, GetName(), "sy.La illegal count" );
                STat->ssp->llc[i1] = i;
                STat->ssp->Acl[i1] = sy.Acl[i];
                STat->ssp->XAun[i1] = sy.XAun[i];
                STat->ssp->XeA[i1++] = sy.XeA[i];
            }
    // PHASE
    for( i=0, i1=0, i2=0, i3=0, i4=0, i5=0; i<mup->Fi; i++)
    {
        if( sy.PbPH != S_OFF )
            if( fabs(sy.Phm[i])  > aPa->pa.p.DB ) // phc no zero in Phm
            {
                ErrorIf( i1>= sy.Fib, GetName(), "sy.Fib illegal count" );
                STat->ssp->phc[i1] = i;
                STat->ssp->XPun[i1] = sy.XPun[i];
                STat->ssp->Phm[i1++]= sy.Phm[i];
            }
        if( sy.Pcl[i] == S_OFF)                // phf phase OFF in system
        {
            ErrorIf( i2>= STat->ssp->DM[22],GetName(),"mu.Fi-sy.Fi illegal count");
            STat->ssp->phf[i2++] = i;
        }
        // phk  no zero in PUL/PLL
        if( ( sy.PULim != S_OFF && fabs(sy.PUL[i]) > aPa->pa.p.DB )||
                ( sy.PLLim != S_OFF && fabs(sy.PLL[i]) > aPa->pa.p.DB ))
        {
            ErrorIf( i3>= sy.Fik, GetName(), "sy.Fik illegal count" );
            STat->ssp->phk[i3] = i;
            if( sy.PULim != S_OFF ) STat->ssp->PUL[i3] = sy.PUL[i];
            if( sy.PLLim != S_OFF ) STat->ssp->PLL[i3] = sy.PLL[i];
            STat->ssp->RFLC[i3] = sy.RFLC[i];
            STat->ssp->RFSC[i3++] = sy.RFSC[i];
        }
        // phe  no zero in YOF/Aalp
        if( ( sy.PAalp != S_OFF && fabs(sy.Aalp[i]) > aPa->pa.p.DB )||
                ( sy.PYOF != S_OFF && fabs(sy.YOF[i]) > aPa->pa.p.DB ))
        {
            ErrorIf( i4>= sy.Fie, GetName(), "sy.Fie illegal count" );
            STat->ssp->phe[i4] = i;
            if( sy.PAalp != S_OFF ) STat->ssp->Aalp[i4] = sy.Aalp[i];
            if( sy.PYOF != S_OFF )  STat->ssp->YOF[i4] = sy.YOF[i];
            if( sy.PSigm != S_OFF )
            {
                STat->ssp->Sigm[i4][0] = sy.Sigm[i][0];
                STat->ssp->Sigm[i4][1] = sy.Sigm[i][1];
            }
            if( sy.PXr0h0 != S_OFF )
            {
                STat->ssp->Xr0h0[i4][0] = sy.Xr0h0[i][0];
                STat->ssp->Xr0h0[i4][1] = sy.Xr0h0[i][1];
            }
            if( sy.PXepsC != S_OFF )
            {
                STat->ssp->XEpsC[i4][0] = sy.XEpsC[i][0];
                STat->ssp->XEpsC[i4][1] = sy.XEpsC[i][1];
            }
            i4++;
        }
        if( i >= mup->Fis )
            continue;
        // pha Phase ON adsorbtion  ( mup->PHC[]=='x' )
        if( sy.Pcl[i] != S_OFF && mup->PHC[i] == 'x' )
        {
            ErrorIf( i5>= sy.Fia, GetName(), "sy.Fia illegal count" );
            STat->ssp->pha[i5] = i;
            for( j=0; j<sy.NsTm; j++ )
            {
                if( sy.PNfsp != S_OFF )
                {
                    STat->ssp->SCMT[i5][j] = sy.SCMT[i][j];
                    STat->ssp->Nfsp[i5][j] = sy.Nfsp[i][j];
                }
                if( sy.PMaSdt != S_OFF )  STat->ssp->MaSdt[i5][j] = sy.MaSdt[i][j];
                if( sy.PXcapA != S_OFF )  STat->ssp->XcapA[i5][j] = sy.XcapA[i][j];
                if( sy.PXcapB != S_OFF )  STat->ssp->XcapB[i5][j] = sy.XcapB[i][j];
                if( sy.PXcapF != S_OFF )  STat->ssp->XcapF[i5][j] = sy.XcapF[i][j];
                if( sy.PXfIEC != S_OFF )  STat->ssp->XfIEC[i5][j] = sy.XfIEC[i][j];
                if( sy.PXlam != S_OFF )   STat->ssp->Xlam[i5][j]  = sy.Xlam[i][j];
            }
            i5++;
        }
    }

//  DCOMP
//    if( STat->ssp->MaSdj ) // Temporary!
//    {  // realloc to prevent memory corruption for old SysEq records
//       STat->ssp->MaSdj = (float (*)[DFCN])aObj[o_ssmasdj].Free();
//       STat->ssp->MaSdj = (float (*)[DFCN])aObj[ o_ssmasdj].Alloc( STat->ssp->DM[4], DFCN, F_ );
//    }
    for( i=0, i1=0, i2=0, i3=0, i4=0, i5=0; i<mup->L; i++)
    {
        if( sy.PbDC != S_OFF )
            if( fabs(sy.XeD[i])  > aPa->pa.p.DB )// dcc no zero in XeD
            {
                ErrorIf( i1>= sy.Lb, GetName(), "sy.Lb illegal count" );
                STat->ssp->dcc[i1] = i;
                STat->ssp->XDun[i1] = sy.XDun[i];
                STat->ssp->XeD[i1++]  = sy.XeD[i];
            }
        if( sy.Dcl[i] == S_OFF)   // dcf dcomp OFF in system
        {
            ErrorIf( i2>= STat->ssp->DM[23], GetName(), "mu.L-sy.L illegal count" );
            STat->ssp->dcf[i2++] = i;
        }
        // dce  no zero in GEX/lnGmf
        if( ( sy.PGEX != S_OFF && fabs(sy.GEX[i]) > aPa->pa.p.DB )||
                ( sy.PlnGf != S_OFF && fabs(sy.lnGmf[i]) > aPa->pa.p.DB ))
        {
            ErrorIf( i4>= sy.Le, GetName(), "sy.Le illegal count" );
            STat->ssp->dce[i4] = i;
            if( sy.PGEX != S_OFF )  STat->ssp->GEX[i4] = sy.GEX[i];
            if( sy.PlnGf != S_OFF ) STat->ssp->lnGmf[i4] = sy.lnGmf[i];
            if( sy.PParc != S_OFF ) STat->ssp->Pparc[i4] = sy.Pparc[i];
            i4++;
        }
        // dck  no zero in DUL/DLL
        if( ( sy.DULim != S_OFF && sy.DUL[i] < 1.0e6/*aPa->pa.p.DB */)||
                ( sy.DLLim != S_OFF && sy.DLL[i] > 0 /*aPa->pa.p.DB */))
        {
            ErrorIf( i3>= sy.Lk, GetName(), "sy.Lk illegal count" );
            STat->ssp->dck[i3] = i;
            if( sy.DULim != S_OFF ) STat->ssp->DUL[i3] = sy.DUL[i];
            if( sy.DLLim != S_OFF ) STat->ssp->DLL[i3] = sy.DLL[i];
            STat->ssp->RLC[i3] = sy.RLC[i];
            STat->ssp->RSC[i3++] = sy.RSC[i];
        }

        if( i >= mup->Ls )
            continue;

        // dca dcomp ON adsorption  ( mup->DCC[]=='X' ) Sveta 12/09/99 ? to Dima
        if( sy.Dcl[i] != S_OFF && ( strchr( SORPTION_DC, mup->DCC[i])  != NULL) )
        {
            int ia, kk;
            ErrorIf( i5 >= sy.Lsor, GetName(), "sy.Lsor invalid count" );
            ia = i - (mup->Ls-mup->Lads);
            STat->ssp->dca[i5] = i;
//            STat->ssp->dca[i5] = i;
            if( sy.PSATT != S_OFF )
            {   // irreversible migration from old data structure 27.10.2004
               for( kk=0; kk<DFCN; kk++ )
                   STat->ssp->SATC[i5][kk] = sy.SATC[ia][kk];
//                STat->ssp->SATC[i5][1] = sy.SATC[i][1];
            }
            if( sy.PMaSdj != S_OFF )
            {
// Extended by KD on 25.10.2004
               for( kk=0; kk<DFCN; kk++ )
                   STat->ssp->MaSdj[i5][kk]  = sy.MaSdj[ia][kk];
//               STat->ssp->MaSdj[i5][PI_CD0]  = sy.MaSdj[i][PI_CD0];
//              STat->ssp->MaSdj[i5] = sy.MaSdj[i];
            }
            i5++;
        }
    }
    contentsChanged = false;
}

// pack system in z_sp_conf mode (save for full project mode)
void TSyst::packData( TCIntArray PHon, TCIntArray PHoff,
                      TCIntArray DCon, TCIntArray DCoff )
{
    short i, i1, i2, i3, i4, i5, j;
    uint iOff;
    TProfil *aPa=(TProfil *)(&aMod[RT_PARAM]);
    TSysEq* STat = (TSysEq*)(&aMod[RT_SYSEQ]);

    // ICOMP
    for( i=0, i1=0, i2= 0; i<mup->N; i++)
    {
        if( sy.PbIC != S_OFF )
            if( fabs(sy.BI[i])  > aPa->pa.p.DB ) // nnc no zero in BI
            {
                ErrorIf( i1>= sy.Na, GetName(), "sy.Na illegal count" );
                STat->ssp->nnc[i1] = i;
                STat->ssp->BIun[i1] = sy.BIun[i];
                STat->ssp->BI[i1++]   = sy.BI[i];
            }
        if( sy.Icl[i] == S_ON )  // nnf IC ON in system
        {
            ErrorIf( i2>= sy.N, GetName(), "sy.N illegal count" );
            STat->ssp->nnf[i2] = i;
            STat->ssp->B[i2]   = sy.B[i];
            if( sy.PdelB != S_OFF && sy.PbIC != S_OFF )
                STat->ssp->delB[i2] = sy.delB[i];
            i2++;
        }
    }
    // COMPOS
    if( sy.PbAC != S_OFF && mup->La > 0 )
        for( i=0, i1=0; i<mup->La; i++) // llc no zero in XeA
            if( sy.Acl[i] != S_OFF && (fabs(sy.XeA[i])>aPa->pa.p.DB ))
            {
                ErrorIf( i1>= sy.La, GetName(), "sy.La illegal count" );
                STat->ssp->llc[i1] = i;
                STat->ssp->Acl[i1] = sy.Acl[i];
                STat->ssp->XAun[i1] = sy.XAun[i];
                STat->ssp->XeA[i1++] = sy.XeA[i];
            }
    // PHASE
    for( i=0, i1=0, i2=0, i3=0, i4=0, i5=0, iOff=0; i<mup->Fi; i++)
    {
        if( sy.PbPH != S_OFF )
            if( fabs(sy.Phm[i])  > aPa->pa.p.DB ) // phc no zero in Phm
            {
                ErrorIf( i1>= sy.Fib, GetName(), "sy.Fib illegal count" );
                STat->ssp->phc[i1] = (short)PHon[i];
                STat->ssp->XPun[i1] = sy.XPun[i];
                STat->ssp->Phm[i1++]= sy.Phm[i];
            }
        if( sy.Pcl[i] == S_OFF)  // phf phase OFF in system
        {
            while( iOff< PHoff.GetCount() && PHoff[iOff] < PHon[i])
            {
                ErrorIf( i2>= STat->ssp->DM[22],GetName(),
                         "mu.Fi-sy.Fi illegal count");
                STat->ssp->phf[i2++] = (short)PHoff[iOff++];
            }
            ErrorIf( i2>= STat->ssp->DM[22],GetName(),
                     "mu.Fi-sy.Fi illegal count");
            STat->ssp->phf[i2++] = (short)PHon[i];
        }
        // phk  no zero in PUL/PLL
        if( ( sy.PULim != S_OFF && fabs(sy.PUL[i]) > aPa->pa.p.DB )||
                ( sy.PLLim != S_OFF && fabs(sy.PLL[i]) > aPa->pa.p.DB ))
        {
            ErrorIf( i3>= sy.Fik, GetName(), "sy.Fik illegal count" );
            STat->ssp->phk[i3] = (short)PHon[i];
            if( sy.PULim != S_OFF ) STat->ssp->PUL[i3] = sy.PUL[i];
            if( sy.PLLim != S_OFF ) STat->ssp->PLL[i3] = sy.PLL[i];
            STat->ssp->RFLC[i3] = sy.RFLC[i];
            STat->ssp->RFSC[i3++] = sy.RFSC[i];
        }
        // phe  no zero in YOF/Aalp
        if( ( sy.PAalp != S_OFF && fabs(sy.Aalp[i]) > aPa->pa.p.DB )||
                ( sy.PYOF != S_OFF && fabs(sy.YOF[i]) > aPa->pa.p.DB ))
        {
            ErrorIf( i4>= sy.Fie, GetName(), "sy.Fie illegal count" );
            STat->ssp->phe[i4] = (short)PHon[i];
            if( sy.PAalp != S_OFF ) STat->ssp->Aalp[i4] = sy.Aalp[i];
            if( sy.PYOF != S_OFF )  STat->ssp->YOF[i4] = sy.YOF[i];
            if( sy.PSigm != S_OFF )
            {
                STat->ssp->Sigm[i4][0] = sy.Sigm[i][0];
                STat->ssp->Sigm[i4][1] = sy.Sigm[i][1];
            }
            if( sy.PXr0h0 != S_OFF )
            {
                STat->ssp->Xr0h0[i4][0] = sy.Xr0h0[i][0];
                STat->ssp->Xr0h0[i4][1] = sy.Xr0h0[i][1];
            }
            if( sy.PXepsC != S_OFF )
            {
                STat->ssp->XEpsC[i4][0] = sy.XEpsC[i][0];
                STat->ssp->XEpsC[i4][1] = sy.XEpsC[i][1];
            }
            i4++;
        }
        if( i >= mup->Fis )
            continue;
        // pha Phase ON adsorbtion  ( mup->PHC[]=='x' )
        if( sy.Pcl[i] != S_OFF && mup->PHC[i] == 'x' )
        {
            ErrorIf( i5>= sy.Fia, GetName(), "sy.Fia illegal count" );
            STat->ssp->pha[i5] = (short)PHon[i];
            for( j=0; j<sy.NsTm; j++ )
            {
                if( sy.PNfsp != S_OFF )
                {
                    STat->ssp->SCMT[i5][j] = sy.SCMT[i][j];
                    STat->ssp->Nfsp[i5][j] = sy.Nfsp[i][j];
                }
                if( sy.PMaSdt != S_OFF )  STat->ssp->MaSdt[i5][j] = sy.MaSdt[i][j];
                if( sy.PXcapA != S_OFF )  STat->ssp->XcapA[i5][j] = sy.XcapA[i][j];
                if( sy.PXcapB != S_OFF )  STat->ssp->XcapB[i5][j] = sy.XcapB[i][j];
                if( sy.PXcapF != S_OFF )  STat->ssp->XcapF[i5][j] = sy.XcapF[i][j];
                if( sy.PXfIEC != S_OFF )  STat->ssp->XfIEC[i5][j] = sy.XfIEC[i][j];
                if( sy.PXlam != S_OFF )   STat->ssp->Xlam[i5][j]  = sy.Xlam[i][j];
            }
            i5++;
        }
    }
    while( iOff< PHoff.GetCount() )
    {
        ErrorIf( i2>= STat->ssp->DM[22],GetName(), "mu.Fi-sy.Fi illegal count");
        STat->ssp->phf[i2++] = (short)PHoff[iOff++];
    }

    // DCOMP
//    if( sy.PMaSdj != S_OFF ) // Temporary!!!!    25.10.2004 KD
//    {  // realloc to prevent memory corruption for old SysEq records
//       STat->ssp->MaSdj = (float (*)[DFCN])aObj[o_ssmasdj].Free();
//       STat->ssp->MaSdj = (float (*)[DFCN])aObj[ o_symasdj].Alloc( mup->Ls, DFCN, F_ );
//    }
    for( i=0, i1=0, i2=0, i3=0, i4=0, i5=0, iOff=0; i<mup->L; i++)
    {
        if( sy.PbDC != S_OFF )
            if( fabs(sy.XeD[i])  > aPa->pa.p.DB )// dcc no zero in XeD
            {
                ErrorIf( i1>= sy.Lb, GetName(), "sy.Lb illegal count" );
                STat->ssp->dcc[i1] = (short)DCon[i];
                STat->ssp->XDun[i1] = sy.XDun[i];
                STat->ssp->XeD[i1++]  = sy.XeD[i];
            }
        if( sy.Dcl[i] == S_OFF)   // dcf dcomp OFF in system
        {
            while( iOff< DCoff.GetCount() && DCoff[iOff]< DCon[i] )
            {
                ErrorIf( i2>= STat->ssp->DM[23],GetName(),
                         "mu.L-sy.L illegal count");
                STat->ssp->dcf[i2++] = (short)DCoff[iOff++];
            }

            ErrorIf( i2>= STat->ssp->DM[23], GetName(), "mu.L-sy.L illegal count" );
            STat->ssp->dcf[i2++] = (short)DCon[i];
        }
        // dce  no zero in GEX/lnGmf
        if( ( sy.PGEX != S_OFF && fabs(sy.GEX[i]) > aPa->pa.p.DB )||
                ( sy.PlnGf != S_OFF && fabs(sy.lnGmf[i]) > aPa->pa.p.DB ))
        {
            ErrorIf( i4>= sy.Le, GetName(), "sy.Le invalid count" );
            STat->ssp->dce[i4] = (short)DCon[i];
            if( sy.PGEX != S_OFF )  STat->ssp->GEX[i4] = sy.GEX[i];
            if( sy.PlnGf != S_OFF ) STat->ssp->lnGmf[i4] = sy.lnGmf[i];
            if( sy.PParc != S_OFF ) STat->ssp->Pparc[i4] = sy.Pparc[i];
            i4++;
        }
        // dck  no zero in DUL/DLL
        if( ( sy.DULim != S_OFF && sy.DUL[i] < 1e6/*> aPa->pa.p.DB*/ )||
                ( sy.DLLim != S_OFF && sy.DLL[i] > 0/*aPa->pa.p.DB*/ ))
        {
            ErrorIf( i3>= sy.Lk, GetName(), "sy.Lk illegal count" );
            STat->ssp->dck[i3] = (short)DCon[i];
            if( sy.DULim != S_OFF ) STat->ssp->DUL[i3] = sy.DUL[i];
            if( sy.DLLim != S_OFF ) STat->ssp->DLL[i3] = sy.DLL[i];
            STat->ssp->RLC[i3] = sy.RLC[i];
            STat->ssp->RSC[i3++] = sy.RSC[i];
        }

        if( i >= mup->Ls )
            continue;

        // dca dcomp ON adsorption  ( mup->DCC[]=='X' ) Sveta 12/09/99 ? to Dima
        if( sy.Dcl[i] != S_OFF && ( strchr( SORPTION_DC, mup->DCC[i])  != NULL) )
        {
            int ia, kk;
            ErrorIf( i5 >= sy.Lsor, GetName(), "sy.Lsor invalid count" );
            ia = i - (mup->Ls-mup->Lads);
            STat->ssp->dca[i5] = (short)DCon[i];
//            STat->ssp->dca[i5] = i;
            if( sy.PSATT != S_OFF )
            {   // irreversible migration from old data structure 27.10.2004
               for( kk=0; kk<DFCN; kk++ )
                   STat->ssp->SATC[i5][kk] = sy.SATC[ia][kk];
//                STat->ssp->SATC[i5][1] = sy.SATC[i][1];
            }
            if( sy.PMaSdj != S_OFF )
            {
// Extended by KD on 25.10.2004
               for( kk=0; kk<DFCN; kk++ )
                  STat->ssp->MaSdj[i5][kk]  = sy.MaSdj[ia][kk];
//               STat->ssp->MaSdj[i5][PI_CD0]  = sy.MaSdj[i][PI_CD0];
//              STat->ssp->MaSdj[i5] = sy.MaSdj[i];
            }
            i5++;
        }
    }
    while( iOff < DCoff.GetCount() )
    {
        ErrorIf( i2 >= STat->ssp->DM[23],GetName(),  "mu.L-sy.L invalid count");
        STat->ssp->dcf[i2++] = (short)DCoff[iOff++];
    }
    STat->ssp->DM[15] = (short)DCon[STat->ssp->DM[15]]; //sy.LO


    contentsChanged = false;
}

//--------------------- End of ms_system.cpp ---------------------------

