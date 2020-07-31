//-------------------------------------------------------------------
// $Id: m_syst2.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of TSysEq class, config functions
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
#include "m_syseq.h"
#include "visor.h"

// link values to objects
void TSysEq::ods_link( int q)
{
    // System ------------------------------------------------------------------
    // static
    aObj[o_ssphst]->SetPtr( ss[q].PhmKey );
    aObj[o_ssname]->SetPtr( ss[q].name );
    aObj[o_ssnotes]->SetPtr( ss[q].notes );
    aObj[o_ssswch]->SetPtr( ss[q].switches ); //a 40
    aObj[o_sspx]->SetPtr( ss[q].Px );         // a 14
    aObj[o_ssdm]->SetPtr( ss[q].DM );         // i 24
    aObj[o_sspmm]->SetPtr( ss[q].PPm );       // f 20
    // dynamic
    aObj[o_ssnnc]->SetPtr( ss[q].nnc );
    aObj[o_ssnnc]->SetDim(ss[q].DM[14], 1 );
    aObj[o_ssllc]->SetPtr( ss[q].llc );
    aObj[o_ssllc]->SetDim(ss[q].DM[11], 1 );
    aObj[o_ssphc]->SetPtr( ss[q].phc );
    aObj[o_ssphc]->SetDim(ss[q].DM[9], 1 );
    aObj[o_ssdcc]->SetPtr( ss[q].dcc );
    aObj[o_ssdcc]->SetDim(ss[q].DM[12], 1 );
    aObj[o_ssnnf]->SetPtr( ss[q].nnf );
    aObj[o_ssnnf]->SetDim(ss[q].DM[0], 1 );
    aObj[o_ssphf]->SetPtr( ss[q].phf );
    aObj[o_ssphf]->SetDim(ss[q].DM[22],1);
    aObj[o_ssdcf]->SetPtr( ss[q].dcf );
    aObj[o_ssdcf]->SetDim(ss[q].DM[23], 1 );
    aObj[o_sspha]->SetPtr( ss[q].pha );
    aObj[o_sspha]->SetDim(ss[q].DM[20], 1 );
    aObj[o_ssdca]->SetPtr( ss[q].dca );
    aObj[o_ssdca]->SetDim(ss[q].DM[4], 1 );
    aObj[o_ssphk]->SetPtr( ss[q].phk );
    aObj[o_ssphk]->SetDim(ss[q].DM[10], 1 );
    aObj[o_ssdck]->SetPtr( ss[q].dck );
    aObj[o_ssdck]->SetDim(ss[q].DM[13], 1 );
    aObj[o_ssdce]->SetPtr( ss[q].dce );
    aObj[o_ssdce]->SetDim(ss[q].DM[18], 1 );
    aObj[o_ssphe]->SetPtr( ss[q].phe );
    aObj[o_ssphe]->SetDim(ss[q].DM[19], 1 );
    aObj[o_ssacl]->SetPtr( ss[q].Acl );
    aObj[o_ssacl]->SetDim(ss[q].DM[11], 1 );
    aObj[o_ssbiun]->SetPtr( ss[q].BIun );
    aObj[o_ssbiun]->SetDim(ss[q].DM[14], 1 );
    aObj[o_ssxdun]->SetPtr( ss[q].XDun );
    aObj[o_ssxdun]->SetDim(ss[q].DM[12], 1 );
    aObj[o_ssxaun]->SetPtr( ss[q].XAun );
    aObj[o_ssxaun]->SetDim(ss[q].DM[11], 1 );
    aObj[o_ssxpun]->SetPtr( ss[q].XPun );
    aObj[o_ssxpun]->SetDim(ss[q].DM[9], 1 );
    aObj[o_ssbi]->SetPtr( ss[q].BI );
    aObj[o_ssbi]->SetDim(ss[q].DM[14], 1 );
    aObj[o_ssxea]->SetPtr( ss[q].XeA );
    aObj[o_ssxea]->SetDim(ss[q].DM[11], 1 );
    aObj[o_ssxed]->SetPtr( ss[q].XeD );
    aObj[o_ssxed]->SetDim(ss[q].DM[12], 1 );
    aObj[o_ssphm]->SetPtr( ss[q].Phm );
    aObj[o_ssphm]->SetDim(ss[q].DM[9], 1 );
    aObj[o_ssb]->SetPtr( ss[q].B );
    aObj[o_ssb]->SetDim(ss[q].DM[0], 1 );
    aObj[o_ssdelb]->SetPtr( ss[q].delB );
    aObj[o_ssdelb]->SetDim(ss[q].DM[0], 1 );
    aObj[o_sspparc]->SetPtr(ss[q].Pparc);
    aObj[o_sspparc]->SetDim(ss[q].DM[18], 1);
    aObj[o_ssgex]->SetPtr( ss[q].GEX );
    aObj[o_ssgex]->SetDim(ss[q].DM[18], 1 );
    aObj[o_sslngmf]->SetPtr(ss[q].lnGmf);
    aObj[o_sslngmf]->SetDim(ss[q].DM[18], 1);
    aObj[o_ssaalp]->SetPtr( ss[q].Aalp );
    aObj[o_ssaalp]->SetDim(ss[q].DM[19], 1 );
    aObj[o_ssyof]->SetPtr( ss[q].YOF );
    aObj[o_ssyof]->SetDim(ss[q].DM[19], 1 );
    aObj[o_sssigm]->SetPtr(ss[q].Sigm);
    aObj[o_sssigm]->SetDim(ss[q].DM[19], 2 );
    aObj[o_ssxr0h0]->SetPtr( ss[q].Xr0h0);
    aObj[o_ssxr0h0]->SetDim(ss[q].DM[19], 2);
    aObj[o_ssxeps]->SetPtr( ss[q].XEpsC );
    aObj[o_ssxeps]->SetDim(ss[q].DM[19], 2 );
    aObj[o_ssrlc]->SetPtr( ss[q].RLC );
    aObj[o_ssrlc]->SetDim(ss[q].DM[13], 1 );
    aObj[o_ssrsc]->SetPtr( ss[q].RSC );
    aObj[o_ssrsc]->SetDim(ss[q].DM[13], 1 );
    aObj[o_ssrflc]->SetPtr( ss[q].RFLC );
    aObj[o_ssrflc]->SetDim(ss[q].DM[10], 1 );
    aObj[o_ssrfsc]->SetPtr( ss[q].RFSC );
    aObj[o_ssrfsc]->SetDim(ss[q].DM[10], 1 );
    aObj[o_ssdul]->SetPtr( ss[q].DUL );
    aObj[o_ssdul]->SetDim(ss[q].DM[13], 1 );
    aObj[o_ssdll]->SetPtr( ss[q].DLL );
    aObj[o_ssdll]->SetDim(ss[q].DM[13], 1 );
    aObj[o_sspul]->SetPtr( ss[q].PUL );
    aObj[o_sspul]->SetDim(ss[q].DM[10], 1 );
    aObj[o_sspll]->SetPtr( ss[q].PLL );
    aObj[o_sspll]->SetDim(ss[q].DM[10], 1 );
    aObj[o_sssatc]->SetPtr( ss[q].SATC );
//    aObj[o_sssatc]->SetDim(ss[q].DM[4], 2 );
    aObj[o_sssatc]->SetDim(ss[q].DM[4], MCAS );
    aObj[o_ssmasdj]->SetPtr(ss[q].MaSdj);
//    aObj[o_ssmasdj]->SetDim(ss[q].DM[4], 1 );
    aObj[o_ssmasdj]->SetDim(ss[q].DM[4], DFCN );
    aObj[o_ssscmt]->SetPtr( ss[q].SCMT );
    aObj[o_ssscmt]->SetDim(ss[q].DM[20], ss[q].DM[17] );
    aObj[o_ssnfsp]->SetPtr( ss[q].Nfsp );
    aObj[o_ssnfsp]->SetDim(ss[q].DM[20], ss[q].DM[17] );
    aObj[o_ssmasdt]->SetPtr( ss[q].MaSdt );
    aObj[o_ssmasdt]->SetDim(ss[q].DM[20], ss[q].DM[17] );
    aObj[o_ssxcapa]->SetPtr( ss[q].XcapA );
    aObj[o_ssxcapa]->SetDim(ss[q].DM[20], ss[q].DM[17] );
    aObj[o_ssxcapb]->SetPtr( ss[q].XcapB );
    aObj[o_ssxcapb]->SetDim(ss[q].DM[20], ss[q].DM[17] );
    aObj[o_ssxcapf]->SetPtr( ss[q].XcapF );
    aObj[o_ssxcapf]->SetDim(ss[q].DM[20], ss[q].DM[17] );
    aObj[o_ssxfiec]->SetPtr( ss[q].XfIEC );
    aObj[o_ssxfiec]->SetDim(ss[q].DM[20], ss[q].DM[17] );
    aObj[o_ssxlam]->SetPtr( ss[q].Xlam );
    aObj[o_ssxlam]->SetDim(ss[q].DM[20], ss[q].DM[17] );
    aObj[ o_sysdref ]->SetPtr( ss[q].sdref );
    aObj[ o_dcsdref ]->SetDim( ss[q].DM[16], 1 );
    aObj[ o_sysdval ]->SetPtr( ss[q].sdval );
    aObj[ o_dcsdval ]->SetDim( ss[q].DM[16], 1 );
    // Equstat ------------------------------------------------------------------

    aObj[ o_stdim]->SetPtr( &st[q].N );  /*i 4*/
    aObj[ o_stitr]->SetPtr( &st[q].itIPM );  /*i 4*/
    aObj[ o_sttdp]->SetPtr( &st[q].P );     /*f 6*/
    aObj[ o_stcrit]->SetPtr(&st[q].PCI );   /*f 4*/
    /* Record  12 fields */
    aObj[ o_stint]->SetPtr(  &st[q].N );    /*i 8*/
    aObj[ o_stfloat]->SetPtr(&st[q].P );    /*f 10*/
    aObj[ o_stnnf]->SetPtr( st[q].nnf );
    aObj[ o_stnnf]->SetDim( st[q].N, 1 );
    aObj[ o_stllf]->SetPtr( st[q].llf );
    aObj[ o_stllf]->SetDim( st[q].L, 1 );
    aObj[ o_stphf]->SetPtr( st[q].phf );
    aObj[ o_stphf]->SetDim( st[q].Fis, 1 );
    aObj[ o_stb]->SetPtr(   st[q].B );
    aObj[ o_stb]->SetDim(   st[q].N, 1 );
    aObj[ o_stu]->SetPtr(   st[q].U );
    aObj[ o_stu]->SetDim(   st[q].N, 1 );
    aObj[ o_sty]->SetPtr(   st[q].Y );
    aObj[ o_sty]->SetDim(   st[q].L, 1 );
    aObj[ o_stgam]->SetPtr( st[q].lnGam );
    aObj[ o_stgam]->SetDim( st[q].L, 1 );
    aObj[ o_stba]->SetPtr( st[q].Ba );
    aObj[ o_stba]->SetDim( st[q].Fis, st[q].N );
    aObj[ o_stvm]->SetPtr( st[q].VM );
    aObj[ o_stvm]->SetDim( st[q].Fis, 5 );
    aObj[ o_stdic]->SetPtr( st[q].DIC );
    aObj[ o_stdic]->SetDim( st[q].N, 2 );
    /* work arrays */
    // aObj[ o_stsbp]->SetPtr( st[q].SBp ); aObj[ o_stsbp]->SetDim( st[q].N, 1 );
    // aObj[ o_stsmp]->SetPtr( st[q].SMp ); aObj[ o_stsmp]->SetDim( st[q].L, 1 );
    // aObj[ o_stsfp]->SetPtr( st[q].SFp ); aObj[ o_stsfp]->SetDim( st[q].Fis, 1 );
    stp=&st[q];
    ssp=&ss[q];
    //  set_def(q);

}

// set dynamic Objects ptr to values

void TSysEq::dyn_set(int q)
{
    // System  ------------------------------------------------------------------
    ErrorIf( ssp!=&ss[q]||stp!=&st[q], GetName(),
             "Invalid access to ss in dyn_set.");
    ss[q].nnc = (short *)aObj[o_ssnnc]->GetPtr();
    ss[q].llc = (short *)aObj[o_ssllc]->GetPtr();
    ss[q].phc = (short *)aObj[o_ssphc]->GetPtr();
    ss[q].dcc = (short *)aObj[o_ssdcc]->GetPtr();
    ss[q].nnf = (short *)aObj[o_ssnnf]->GetPtr();
    ss[q].phf = (short *)aObj[o_ssphf]->GetPtr();
    ss[q].dcf = (short *)aObj[o_ssdcf]->GetPtr();
    ss[q].pha = (short *)aObj[o_sspha]->GetPtr();
    ss[q].dca = (short *)aObj[o_ssdca]->GetPtr();
    ss[q].phk = (short *)aObj[o_ssphk]->GetPtr();
    ss[q].dck = (short *)aObj[o_ssdck]->GetPtr();
    ss[q].dce = (short *)aObj[o_ssdce]->GetPtr();
    ss[q].phe = (short *)aObj[o_ssphe]->GetPtr();
    ss[q].Acl = (char *)aObj[o_ssacl]->GetPtr();
    ss[q].BIun = (char *)aObj[o_ssbiun]->GetPtr();
    ss[q].XDun = (char *)aObj[o_ssxdun]->GetPtr();
    ss[q].XAun = (char *)aObj[o_ssxaun]->GetPtr();
    ss[q].XPun = (char *)aObj[o_ssxpun]->GetPtr();
    ss[q].BI = (double *)aObj[o_ssbi]->GetPtr();
    ss[q].XeA = (double *)aObj[o_ssxea]->GetPtr();
    ss[q].XeD = (double *)aObj[o_ssxed]->GetPtr();
    ss[q].Phm = (double *)aObj[o_ssphm]->GetPtr();
    ss[q].B = (double *)aObj[o_ssb]->GetPtr();
    ss[q].RLC= (char *)aObj[o_ssrlc]->GetPtr();
    ss[q].RSC = (char *)aObj[o_ssrsc]->GetPtr();
    ss[q].RFLC = (char *)aObj[o_ssrflc]->GetPtr();
    ss[q].RFSC = (char *)aObj[o_ssrfsc]->GetPtr();

    if( aObj[o_ssdelb]->GetType() == F_ )
       ss[q].delB = (double *)aObj[o_ssdelb]->Alloc( ss[q].DM[0], 1, D_ );
    else
       ss[q].delB = (double *)aObj[o_ssdelb]->GetPtr();
    if( aObj[o_sspparc]->GetType() == F_ )
       ss[q].Pparc = (double *)aObj[o_sspparc]->Alloc( ss[q].DM[18], 1, D_ );
    else
       ss[q].Pparc = (double *)aObj[o_sspparc]->GetPtr();
    if( aObj[o_ssgex]->GetType() == F_ )
       ss[q].GEX = (double *)aObj[o_ssgex]->Alloc( ss[q].DM[18], 1, D_ );
    else
       ss[q].GEX = (double *)aObj[o_ssgex]->GetPtr();
    if( aObj[o_sslngmf]->GetType() == F_ )
       ss[q].lnGmf = (double *)aObj[o_sslngmf]->Alloc( ss[q].DM[18], 1, D_ );
    else
       ss[q].lnGmf = (double *)aObj[o_sslngmf]->GetPtr();
    if( aObj[o_ssaalp]->GetType() == F_ )
       ss[q].Aalp = (double *)aObj[o_ssaalp]->Alloc( ss[q].DM[19], 1, D_ );
    else
       ss[q].Aalp = (double *)aObj[o_ssaalp]->GetPtr();
      if( aObj[o_ssyof]->GetType() == F_ )
       ss[q].YOF = (double *)aObj[o_ssyof]->Alloc( ss[q].DM[19], 1, D_ );
    else
       ss[q].YOF = (double *)aObj[o_ssyof]->GetPtr();
    if( aObj[o_ssdul]->GetType() == F_ )
       ss[q].DUL = (double *)aObj[o_ssdul]->Alloc( ss[q].DM[13], 1, D_ );
    else
       ss[q].DUL = (double *)aObj[o_ssdul]->GetPtr();
    if( aObj[o_ssdll]->GetType() == F_ )
       ss[q].DLL = (double *)aObj[o_ssdll]->Alloc( ss[q].DM[13], 1, D_ );
    else
       ss[q].DLL = (double *)aObj[o_ssdll]->GetPtr();
    if( aObj[o_sspul]->GetType() == F_ )
       ss[q].PUL = (double *)aObj[o_sspul]->Alloc( ss[q].DM[10], 1, D_ );
    else
       ss[q].PUL = (double *)aObj[o_sspul]->GetPtr();

    if( aObj[o_sspll]->GetType() == F_ )
       ss[q].PLL = (double *)aObj[o_sspll]->Alloc( ss[q].DM[10], 1, D_ );
    else
       ss[q].PLL = (double *)aObj[o_sspll]->GetPtr();

    if( aObj[o_sssigm]->GetType() == F_ )
       ss[q].Sigm = (double (*)[2])aObj[o_sssigm]->Alloc( ss[q].DM[19], 2, D_ );
    else
       ss[q].Sigm = (double (*)[2])aObj[o_sssigm]->GetPtr();
    if( aObj[o_ssxr0h0]->GetType() == F_ )
       ss[q].Xr0h0 = (double (*)[2])aObj[o_ssxr0h0]->Alloc( ss[q].DM[19], 2, D_ );
    else
       ss[q].Xr0h0 = (double (*)[2])aObj[o_ssxr0h0]->GetPtr();
    if( aObj[o_ssxeps]->GetType() == F_ )
       ss[q].XEpsC = (double (*)[2])aObj[o_ssxeps]->Alloc( ss[q].DM[19], 2, D_ );
    else
       ss[q].XEpsC = (double (*)[2])aObj[o_ssxeps]->GetPtr();

    ss[q].SATC = (char (*)[MCAS])aObj[o_sssatc]->GetPtr();
    ss[q].MaSdj= (float (*)[DFCN])aObj[o_ssmasdj]->GetPtr();
    ss[q].SCMT = (char (*)[MNST])aObj[o_ssscmt]->GetPtr();
    ss[q].Nfsp = (float (*)[MNST])aObj[o_ssnfsp]->GetPtr();
    ss[q].MaSdt = (float (*)[MNST])aObj[o_ssmasdt]->GetPtr();
    ss[q].XcapA= (float (*)[MNST])aObj[o_ssxcapa]->GetPtr();
    ss[q].XcapB= (float (*)[MNST])aObj[o_ssxcapb]->GetPtr();
    ss[q].XcapF = (float (*)[MNST])aObj[o_ssxcapf]->GetPtr();
    ss[q].XfIEC= (float (*)[MNST])aObj[o_ssxfiec]->GetPtr();
    ss[q].Xlam = (float (*)[MNST])aObj[o_ssxlam]->GetPtr();

    ss[q].sdref= (char (*)[V_SD_RKLEN])aObj[ o_sysdref ]->GetPtr();
    ss[q].sdval= (char (*)[V_SD_VALEN])aObj[ o_sysdval ]->GetPtr();

    // Equstat ------------------------------------------------------------------
    st[q].nnf = (short *)aObj[ o_stnnf ]->GetPtr();
    st[q].llf = (short *)aObj[ o_stllf ]->GetPtr();
    st[q].phf = (short *)aObj[ o_stphf ]->GetPtr();
    st[q].B = (double *)aObj[ o_stb ]->GetPtr();
    st[q].U = (double *)aObj[ o_stu ]->GetPtr();
    st[q].Y = (double *)aObj[ o_sty ]->GetPtr();
    st[q].lnGam = (double *)aObj[ o_stgam ]->GetPtr();
    st[q].Ba = (double *)aObj[ o_stba ]->GetPtr();
    st[q].VM = (double (*)[5])aObj[ o_stvm ]->GetPtr();
    st[q].DIC = (double (*)[2])aObj[ o_stdic ]->GetPtr();

    // st[q].SBp = (char (*)[MAXICNAME+MAXSYMB])aObj[ o_stsbp ]->GetPtr();
    // st[q].SMp = (char (*)[MAXDCNAME])aObj[ o_stsmp ]->GetPtr();
    // st[q].SFp = (char (*)[MAXPHNAME])aObj[ o_stsfp ]->GetPtr();
}

// free dynamic memory in objects and values

void TSysEq::dyn_kill(int q)
{
    // System ------------------------------------------------------------------
    ErrorIf( ssp!=&ss[q]||stp!=&st[q], GetName(),
             "Invalid access to ss in dyn_kill.");
    ss[q].nnc = (short *)aObj[ o_ssnnc ]->Free();
    ss[q].llc = (short *)aObj[ o_ssllc ]->Free();
    ss[q].phc = (short *)aObj[ o_ssphc ]->Free();
    ss[q].dcc = (short *)aObj[ o_ssdcc ]->Free();
    ss[q].nnf = (short *)aObj[ o_ssnnf ]->Free();
    ss[q].phf = (short *)aObj[ o_ssphf ]->Free();
    ss[q].dcf = (short *)aObj[ o_ssdcf ]->Free();
    ss[q].pha = (short *)aObj[ o_sspha ]->Free();
    ss[q].dca = (short *)aObj[ o_ssdca ]->Free();
    ss[q].phk = (short *)aObj[ o_ssphk ]->Free();
    ss[q].dck = (short *)aObj[ o_ssdck ]->Free();
    ss[q].dce = (short *)aObj[ o_ssdce ]->Free();
    ss[q].phe = (short *)aObj[ o_ssphe ]->Free();
    ss[q].Acl = (char *)aObj[ o_ssacl ]->Free();
    ss[q].BIun = (char *)aObj[ o_ssbiun ]->Free();
    ss[q].XDun = (char *)aObj[ o_ssxdun ]->Free();
    ss[q].XAun = (char *)aObj[ o_ssxaun ]->Free();
    ss[q].XPun = (char *)aObj[ o_ssxpun ]->Free();
    ss[q].BI = (double *)aObj[ o_ssbi ]->Free();
    ss[q].XeA = (double *)aObj[ o_ssxea ]->Free();
    ss[q].XeD = (double *)aObj[ o_ssxed ]->Free();
    ss[q].Phm = (double *)aObj[ o_ssphm ]->Free();
    ss[q].B = (double *)aObj[ o_ssb ]->Free();
    ss[q].delB= (double *)aObj[ o_ssdelb ]->Free();
    ss[q].Pparc = (double *)aObj[ o_sspparc ]->Free();
    ss[q].GEX = (double *)aObj[ o_ssgex ]->Free();
    ss[q].lnGmf= (double *)aObj[ o_sslngmf ]->Free();
    ss[q].Aalp = (double *)aObj[ o_ssaalp ]->Free();
    ss[q].YOF= (double *)aObj[ o_ssyof ]->Free();
    ss[q].Sigm= (double (*)[2])aObj[ o_sssigm ]->Free();
    ss[q].Xr0h0= (double (*)[2])aObj[ o_ssxr0h0 ]->Free();
    ss[q].XEpsC = (double (*)[2])aObj[ o_ssxeps ]->Free();
    ss[q].RLC= (char *)aObj[ o_ssrlc ]->Free();
    ss[q].RSC = (char *)aObj[ o_ssrsc ]->Free();
    ss[q].RFLC = (char *)aObj[ o_ssrflc ]->Free();
    ss[q].RFSC = (char *)aObj[ o_ssrfsc ]->Free();
    ss[q].DUL = (double *)aObj[ o_ssdul ]->Free();
    ss[q].DLL  = (double *)aObj[ o_ssdll ]->Free();
    ss[q].PUL = (double *)aObj[ o_sspul ]->Free();
    ss[q].PLL = (double *)aObj[ o_sspll ]->Free();
    ss[q].SATC = (char (*)[MCAS])aObj[ o_sssatc ]->Free();
    ss[q].MaSdj= (float (*)[DFCN])aObj[ o_ssmasdj ]->Free();
    ss[q].SCMT = (char (*)[MNST])aObj[ o_ssscmt ]->Free();
    ss[q].Nfsp = (float (*)[MNST])aObj[ o_ssnfsp ]->Free();
    ss[q].MaSdt = (float (*)[MNST])aObj[ o_ssmasdt ]->Free();
    ss[q].XcapA= (float (*)[MNST])aObj[ o_ssxcapa ]->Free();
    ss[q].XcapB= (float (*)[MNST])aObj[ o_ssxcapb ]->Free();
    ss[q].XcapF = (float (*)[MNST])aObj[ o_ssxcapf ]->Free();
    ss[q].XfIEC= (float (*)[MNST])aObj[ o_ssxfiec ]->Free();
    ss[q].Xlam = (float (*)[MNST])aObj[ o_ssxlam ]->Free();
    ss[q].sdref= (char (*)[V_SD_RKLEN])aObj[ o_sysdref ]->Free();
    ss[q].sdval= (char (*)[V_SD_VALEN])aObj[ o_sysdval ]->Free();

    // Equstat ------------------------------------------------------------------
    st[q].nnf = (short *)aObj[ o_stnnf ]->Free();
    st[q].llf = (short *)aObj[ o_stllf ]->Free();
    st[q].phf = (short *)aObj[ o_stphf ]->Free();
    st[q].B = (double *)aObj[ o_stb ]->Free();
    st[q].U = (double *)aObj[ o_stu ]->Free();
    st[q].Y = (double *)aObj[ o_sty ]->Free();
    st[q].lnGam = (double *)aObj[ o_stgam ]->Free();
    st[q].Ba = (double *)aObj[ o_stba ]->Free();
    st[q].VM = (double (*)[5])aObj[ o_stvm ]->Free();
    st[q].DIC = (double (*)[2])aObj[ o_stdic ]->Free();

    // st[q].SBp = (char (*)[MAXICNAME+MAXSYMB])aObj[ o_stsbp ]->Free();
    // st[q].SMp = (char (*)[MAXDCNAME])aObj[ o_stsmp ]->Free();
    // st[q].SFp = (char (*)[MAXPHNAME])aObj[ o_stsfp ]->Free();
}

// realloc dynamic memory for SYSTEM

void TSysEq::dyn_new( int q )
{
    ErrorIf( ssp!=&ss[q]||stp!=&st[q], GetName(),
             "Invalid access to ss in dyn_new.");
    // System ------------------------------------------------------------------
    //ErrorIf( mup->L < 2, GetName(), "mup->L < 2");

    /* Necessary arrays */
    if( ss[q].DM[14] )
        ss[q].nnc = (short *)aObj[ o_ssnnc]->Alloc( ss[q].DM[14], 1, I_ );
    else ss[q].nnc = (short *)aObj[o_ssnnc ]->Free();
    if( ss[q].DM[11] )
        ss[q].llc = (short *)aObj[o_ssllc ]->Alloc( ss[q].DM[11], 1, I_ );
    else ss[q].llc = (short *)aObj[o_ssllc ]->Free();
    if( ss[q].DM[9] )
        ss[q].phc = (short *)aObj[ o_ssphc]->Alloc( ss[q].DM[9], 1, I_ );
    else ss[q].phc = (short *)aObj[o_ssphc ]->Free();
    if( ss[q].DM[12] )
        ss[q].dcc = (short *)aObj[o_ssdcc ]->Alloc( ss[q].DM[12], 1, I_ );
    else ss[q].dcc = (short *)aObj[ o_ssdcc]->Free();
    if( ss[q].DM[0] )
        ss[q].nnf = (short *)aObj[o_ssnnf ]->Alloc( ss[q].DM[0], 1, I_ );
    else ss[q].nnf = (short *)aObj[ o_ssnnf]->Free();
    if( ss[q].DM[22] )
        ss[q].phf = (short *)aObj[o_ssphf ]->Alloc( ss[q].DM[22], 1, I_ );
    else ss[q].phf = (short *)aObj[o_ssphf ]->Free();
    if( ss[q].DM[23] )
        ss[q].dcf = (short *)aObj[ o_ssdcf]->Alloc( ss[q].DM[23], 1, I_ );
    else ss[q].dcf = (short *)aObj[ o_ssdcf]->Free();
    if( ss[q].DM[20] )
        ss[q].pha = (short *)aObj[ o_sspha]->Alloc( ss[q].DM[20], 1, I_ );
    else ss[q].pha = (short *)aObj[o_sspha ]->Free();
    if( ss[q].DM[4] )
        ss[q].dca = (short *)aObj[o_ssdca ]->Alloc( ss[q].DM[4], 1, I_ );
    else ss[q].dca = (short *)aObj[o_ssdca ]->Free();
    if( ss[q].DM[10] )
        ss[q].phk = (short *)aObj[o_ssphk ]->Alloc( ss[q].DM[10], 1, I_ );
    else ss[q].phk = (short *)aObj[o_ssphk ]->Free();
    if( ss[q].DM[13] )
        ss[q].dck = (short *)aObj[ o_ssdck]->Alloc( ss[q].DM[13], 1, I_ );
    else ss[q].dck = (short *)aObj[ o_ssdck]->Free();
    if( ss[q].DM[18] )
        ss[q].dce = (short *)aObj[ o_ssdce]->Alloc( ss[q].DM[18], 1, I_ );
    else ss[q].dce = (short *)aObj[ o_ssdce]->Free();
    if( ss[q].DM[19] )
        ss[q].phe = (short *)aObj[ o_ssphe]->Alloc( ss[q].DM[19], 1, I_ );
    else ss[q].phe = (short *)aObj[o_ssphe ]->Free();

    ss[q].B = (double *)aObj[ o_ssb]->Alloc( ss[q].DM[0], 1, D_ );
    if( ss[q].DM[11] )
        ss[q].Acl = (char *)aObj[ o_ssacl]->Alloc( ss[q].DM[11], 1, A_ );
    else ss[q].Acl = (char *)aObj[ o_ssacl ]->Free();

    // no default arrays
    /* compositions */
    if( ss[q].switches[10] != S_OFF )
    {
        ss[q].BI = (double *)aObj[ o_ssbi]->Alloc( ss[q].DM[14], 1, D_ );
        ss[q].BIun = (char *)aObj[ o_ssbiun]->Alloc( ss[q].DM[14], 1, A_ );
    }
    else
    {
        ss[q].BI = (double *)aObj[ o_ssbi ]->Free();
        ss[q].BIun = (char *)aObj[ o_ssbiun ]->Free();
    }
    if( ss[q].switches[10] != S_OFF && ss[q].switches[35] != S_OFF )
        ss[q].delB = (double *)aObj[ o_ssdelb]->Alloc( ss[q].DM[0], 1, D_);
    else ss[q].delB = (double *)aObj[ o_ssdelb ]->Free();

    if( ss[q].switches[11] != S_OFF && ss[q].DM[12]>0 )
    {
        ss[q].XeD = (double *)aObj[ o_ssxed]->Alloc( ss[q].DM[12], 1, D_ );
        ss[q].XDun = (char *)aObj[ o_ssxdun]->Alloc( ss[q].DM[12], 1, A_ );
    }
    else
    {
        ss[q].XeD = (double *)aObj[ o_ssxed ]->Free();
        ss[q].XDun = (char *)aObj[ o_ssxdun ]->Free();
    }
    if( ss[q].switches[12] != S_OFF && ss[q].DM[11] > 0 )
    {
        ss[q].XeA = (double *)aObj[ o_ssxea]->Alloc( ss[q].DM[11], 1, D_);
        ss[q].XAun = (char *)aObj[ o_ssxaun]->Alloc( ss[q].DM[11], 1, A_ );
    }
    else
    {
        ss[q].XAun = (char *)aObj[ o_ssxaun ]->Free();
        ss[q].XeA = (double *)aObj[ o_ssxea ]->Free();
    }
    if( ss[q].switches[13] != S_OFF )
    {
        ss[q].Phm = (double *)aObj[ o_ssphm]->Alloc( ss[q].DM[9], 1, D_ );
        ss[q].XPun = (char *)aObj[ o_ssxpun]->Alloc( ss[q].DM[9], 1, A_ );
    }
    else
    {
        ss[q].Phm = (double *)aObj[ o_ssphm ]->Free();
        ss[q].XPun = (char *)aObj[ o_ssxpun ]->Free();
    }
    /* limits */
    if( ss[q].switches[14] != S_OFF )
        ss[q].DLL = (double *)aObj[ o_ssdll]->Alloc( ss[q].DM[13], 1, D_ );
    else ss[q].DLL = (double *)aObj[ o_ssdll ]->Free();

    if( ss[q].switches[16]!= S_OFF )
        ss[q].DUL = (double *)aObj[ o_ssdul]->Alloc( ss[q].DM[13], 1, D_ );
    else ss[q].DUL = (double *)aObj[ o_ssdul ]->Free();

    if( ss[q].switches[16] != S_OFF || ss[q].switches[14] != S_OFF )
    {
        ss[q].RLC = (char *)aObj[ o_ssrlc]->Alloc( ss[q].DM[13], 1, A_ );
        ss[q].RSC = (char *)aObj[ o_ssrsc]->Alloc( ss[q].DM[13], 1, A_ );
    }
    else
    {
        ss[q].RLC = (char *)aObj[ o_ssrlc ]->Free();
        ss[q].RSC = (char *)aObj[ o_ssrsc ]->Free();
    }
    if( ss[q].switches[15] != S_OFF )
        ss[q].PLL = (double *)aObj[ o_sspll]->Alloc( ss[q].DM[10], 1, D_ );
    else ss[q].PLL = (double *)aObj[ o_sspll ]->Free();

    if( ss[q].switches[17] != S_OFF )
        ss[q].PUL = (double *)aObj[ o_sspul]->Alloc( ss[q].DM[10], 1, D_ );
    else ss[q].PUL = (double *)aObj[ o_sspul ]->Free();

    if( ss[q].switches[17] != S_OFF || ss[q].switches[15] != S_OFF )
    {
        ss[q].RFLC = (char *)aObj[ o_ssrflc]->Alloc( ss[q].DM[10], 1, A_ );
        ss[q].RFSC = (char *)aObj[ o_ssrfsc]->Alloc( ss[q].DM[10], 1, A_ );
    }
    else
    {
        ss[q].RFLC = (char *)aObj[ o_ssrflc ]->Free();
        ss[q].RFSC = (char *)aObj[ o_ssrfsc ]->Free();
    }
    /* add parameterd */
    if( ss[q].switches[21] != S_OFF && ss[q].DM[19] > 0 )
        ss[q].YOF = (double *)aObj[ o_ssyof]->Alloc( ss[q].DM[19], 1, D_ );
    else ss[q].YOF = (double *)aObj[ o_ssyof ]->Free();

    if( ss[q].switches[34] != S_OFF && ss[q].DM[18] > 0 )
        ss[q].Pparc = (double *)aObj[ o_sspparc]->Alloc( ss[q].DM[18], 1, D_ );
    else ss[q].Pparc = (double *)aObj[ o_sspparc ]->Free();

    if( ss[q].switches[19] != S_OFF && ss[q].DM[4] > 0 )
        ss[q].SATC = (char (*)[MCAS])aObj[ o_sssatc]->Alloc( ss[q].DM[4], MCAS, A_ );
//      ss[q].SATC = (char (*)[2])aObj[ o_sssatc]->Alloc( ss[q].DM[4], 2, A_ );
    else ss[q].SATC = (char (*)[MCAS])aObj[ o_sssatc ]->Free();

    if( ss[q].switches[20] != S_OFF && ss[q].DM[18] > 0 )
        ss[q].GEX = (double *)aObj[ o_ssgex]->Alloc( ss[q].DM[18], 1, D_ );
    else ss[q].GEX = (double *)aObj[ o_ssgex ]->Free();

    if( ss[q].switches[29] != S_OFF && ss[q].DM[4] > 0 )
        ss[q].MaSdj = (float (*)[DFCN])aObj[ o_ssmasdj]->Alloc( ss[q].DM[4], DFCN, F_ );
//      ss[q].MaSdj = (float *)aObj[ o_ssmasdj]->Alloc( ss[q].DM[4], 1, F_ );
    else ss[q].MaSdj = (float (*)[DFCN])aObj[ o_ssmasdj ]->Free();

    if( ss[q].switches[23] != S_OFF && ss[q].DM[18] > 0 )
        ss[q].lnGmf = (double *)aObj[ o_sslngmf]->Alloc( ss[q].DM[18], 1, D_ );
    else ss[q].lnGmf = (double *)aObj[ o_sslngmf ]->Free();

    /* Params of dispers phase */
    if( ss[q].switches[24] != S_OFF && ss[q].DM[19] > 0 )
        ss[q].Aalp = (double *)aObj[ o_ssaalp]->Alloc( ss[q].DM[19], 1, D_ );
    else ss[q].Aalp = (double *)aObj[ o_ssaalp ]->Free();

    /* Modifications  */
    if( ss[q].switches[25] != S_OFF && ss[q].DM[19] > 0 )
        ss[q].Sigm = (double (*)[2])aObj[ o_sssigm]->Alloc( ss[q].DM[19], 2, D_ );
    else ss[q].Sigm = (double (*)[2])aObj[ o_sssigm ]->Free();

    if( ss[q].switches[27] != S_OFF )
        ss[q].XEpsC = (double (*)[2])aObj[ o_ssxeps]->Alloc( ss[q].DM[19], 2, D_ );
    else  ss[q].XEpsC = (double (*)[2])aObj[ o_ssxeps ]->Free();

    if( ss[q].switches[26] != S_OFF )
        ss[q].Xr0h0 = (double (*)[2])aObj[ o_ssxr0h0]->Alloc( ss[q].DM[19], 2, D_ );
    else ss[q].Xr0h0 = (double (*)[2])aObj[ o_ssxr0h0 ]->Free();

    if( ss[q].switches[28] != S_OFF && ss[q].DM[20] > 0 )
    {
        ss[q].Nfsp =(float (*)[MNST])aObj[o_ssnfsp]->Alloc(ss[q].DM[20], ss[q].DM[17], F_);
        ss[q].SCMT =(char (*)[MNST])aObj[o_ssscmt]->Alloc(ss[q].DM[20], ss[q].DM[17], A_);
    }
    else
    {
        ss[q].Nfsp =  (float (*)[MNST])aObj[ o_ssnfsp ]->Free();
        ss[q].SCMT  = (char (*)[MNST])aObj[ o_ssscmt ]->Free();
    }

    if( ss[q].switches[29] != S_OFF && ss[q].DM[20] > 0 )
        ss[q].MaSdt = (float (*)[MNST])aObj[ o_ssmasdt]->Alloc(
                          ss[q].DM[20], ss[q].DM[17], F_ );
    else ss[q].MaSdt = (float (*)[MNST])aObj[ o_ssmasdt ]->Free();

    if( ss[q].switches[31] != S_OFF && ss[q].DM[20] > 0 )
        ss[q].XcapA = (float (*)[MNST])aObj[ o_ssxcapa]->Alloc(
                          ss[q].DM[20], ss[q].DM[17], F_ );
    else ss[q].XcapA = (float (*)[MNST])aObj[ o_ssxcapa ]->Free();

    if( ss[q].switches[32] != S_OFF && ss[q].DM[20] > 0 )
        ss[q].XcapB = (float (*)[MNST])aObj[ o_ssxcapb]->Alloc(
                          ss[q].DM[20], ss[q].DM[17], F_ );
    else ss[q].XcapB = (float (*)[MNST])aObj[ o_ssxcapb ]->Free();

    if( ss[q].switches[33] != S_OFF && ss[q].DM[20] > 0 )
        ss[q].XfIEC = (float (*)[MNST])aObj[ o_ssxfiec]->Alloc(
                          ss[q].DM[20], ss[q].DM[17], F_ );
    else ss[q].XfIEC = (float (*)[MNST])aObj[ o_ssxfiec ]->Free();

    if( ss[q].switches[30] != S_OFF && ss[q].DM[20] > 0 )
        ss[q].XcapF = (float (*)[MNST])aObj[ o_ssxcapf]->Alloc(
                          ss[q].DM[20], ss[q].DM[17], F_ );
    else ss[q].XcapF = (float (*)[MNST])aObj[ o_ssxcapf ]->Free();

    if( ss[q].switches[36] != S_OFF && ss[q].DM[20] > 0 )
        ss[q].Xlam = (float (*)[MNST])aObj[ o_ssxlam]->Alloc(
                         ss[q].DM[20], ss[q].DM[17], F_ );
    else ss[q].Xlam = (float (*)[MNST])aObj[ o_ssxlam ]->Free();

    if( ss[q].DM[16] == 0 )
    {
        ss[q].sdref = (char (*)[V_SD_RKLEN])aObj[ o_sysdref ]->Free();
        ss[q].sdval = (char (*)[V_SD_VALEN])aObj[ o_sysdval ]->Free();
    }
    else
    {
        ss[q].sdref = (char (*)[V_SD_RKLEN])aObj[ o_sysdref]->Alloc(
                          ss[q].DM[16], 1,V_SD_RKLEN );
        ss[q].sdval=(char (*)[V_SD_VALEN])aObj[o_sysdval]->Alloc(
                        ss[q].DM[16], 1, V_SD_VALEN );
    }

    // Equstat ------------------------------------------------------------------
    st[q].nnf = (short *)aObj[ o_stnnf ]->Alloc( st[q].N, 1, I_ );
    st[q].llf = (short *)aObj[ o_stllf ]->Alloc( st[q].L, 1, I_ );
    st[q].phf = (short *)aObj[ o_stphf ]->Alloc( st[q].Fis, 1, I_ );
    st[q].B = (double *)aObj[ o_stb ]->Alloc( st[q].N, 1, D_ );
    st[q].U = (double *)aObj[ o_stu ]->Alloc( st[q].N, 1, D_ );
    st[q].Y = (double *)aObj[ o_sty ]->Alloc( st[q].L, 1, D_ );
    st[q].lnGam = (double *)aObj[ o_stgam ]->Alloc( st[q].L, 1, D_ );
    st[q].Ba = (double *)aObj[ o_stba ]->Alloc( st[q].Fis, st[q].N, D_ );
    st[q].VM = (double (*)[5])aObj[ o_stvm ]->Alloc( st[q].Fis, 5, D_ );
    st[q].DIC = (double (*)[2])aObj[ o_stdic ]->Alloc( st[q].N, 2, D_ );
    // st[q].SBp = (char (*)[MAXICNAME+MAXSYMB])aObj[ o_stsbp ]->Alloc( st[q].N, 1,
    //                                                            MAXICNAME+MAXSYMB);
    // st[q].SMp = (char (*)[MAXDCNAME])aObj[ o_stsmp ]->Alloc( st[q].L, 1, MAXDCNAME);
    // st[q].SFp = (char (*)[MAXPHNAME])aObj[ o_stsfp ]->Alloc( st[q].Fis,1,MAXPHNAME);
}

//set default information

void TSysEq::set_def( int q)
{
    ErrorIf( ssp!=&ss[q]||stp!=&st[q], GetName(),
             "Invalid access to ss in set_def.");
    // System ------------------------------------------------------------------
    TProfil *aPa= dynamic_cast<TProfil *>(aMod[RT_PARAM].get());
    strcpy( ss[q].name, "`");
    strcpy( ss[q].notes, "`");
    memset( ss[q].PhmKey, 0, EQ_RKLEN );
    memset( ss[q].PhmKey, '-', 3 );  // Fix for default, DK 23.10.2011
    bool is_a = false;
    bool is_x = false;
    bool is_g = false;

    if( pVisor->ProfileMode == true )
        for( int i=0; i<TRMults::sm->GetMU()->Fi; i++)
        {
            if( TRMults::sm->GetMU()->PHC[i] == PH_AQUEL )
                is_a = true;
            if( TRMults::sm->GetMU()->PHC[i] == PH_SORPTION )
                is_x = true;
           if( TRMults::sm->GetMU()->PHC[i] == PH_GASMIX ||
               TRMults::sm->GetMU()->PHC[i] == PH_PLASMA ||
               TRMults::sm->GetMU()->PHC[i] == PH_FLUID )
                is_g = true;

        }
    else is_a = true;
    memcpy( ss[q].switches, aPa->pa.SYppc, 10 );
    memcpy( &ss[q].switches[10], aPa->pa.SYpvc, 28 );
    memcpy( &ss[q].switches[38], "--", 2 );
    if( !is_a  && !is_x  && !is_g )
        ss[q].switches[0] = '-';
    if( is_x )
    {
        ss[q].switches[9] = '+';
        ss[q].switches[19] = '+';
        ss[q].switches[22] = '+';
        ss[q].switches[28] = '+';
        ss[q].switches[29] = '+';
        ss[q].switches[31] = '+';
        ss[q].switches[32] = '+';
    }

    memset( ss[q].Px, '-', 14 );  //????????????????
    memset( ss[q].DM, 0, 24*sizeof(short));
    memset( ss[q].PPm, 0, 20*sizeof(float));
ss[q].PPm[9]  = 1.;  // Mwat  // These defaults changed from 0 to 1 by DK 27.02.2012
ss[q].PPm[10] = 1.;  // Msys
ss[q].PPm[11] = 1.;  // Maq
ss[q].PPm[14] = 1.;  // Vsys
ss[q].PPm[15] = 1.;  // Vaq
    if( is_x )
        ss[q].DM[17] = MNST;
    ss[q].nnc = 0;
    ss[q].llc = 0;
    ss[q].phc = 0;
    ss[q].dcc = 0;
    ss[q].nnf = 0;
    ss[q].phf = 0;
    ss[q].dcf = 0;
    ss[q].pha = 0;
    ss[q].dca = 0;
    ss[q].phk = 0;
    ss[q].dck = 0;
    ss[q].dce = 0;
    ss[q].phe = 0;
    ss[q].Acl = 0;
    ss[q].BIun =0;
    ss[q].XDun =0;
    ss[q].XAun =0;
    ss[q].XPun = 0;
    ss[q].BI = 0;
    ss[q].XeA = 0;
    ss[q].XeD = 0;
    ss[q].Phm = 0;
    ss[q].B = 0;
    ss[q].delB= 0;
    ss[q].Pparc = 0;
    ss[q].GEX = 0;
    ss[q].lnGmf= 0;
    ss[q].Aalp = 0;
    ss[q].YOF= 0;
    ss[q].Sigm= 0;
    ss[q].Xr0h0= 0;
    ss[q].XEpsC = 0;
    ss[q].RLC= 0;
    ss[q].RSC = 0;
    ss[q].RFLC = 0;
    ss[q].RFSC = 0;
    ss[q].DUL = 0;
    ss[q].DLL  = 0;
    ss[q].PUL = 0;
    ss[q].PLL = 0;
    ss[q].SATC = 0;
    ss[q].MaSdj= 0;
    ss[q].SCMT = 0;
    ss[q].Nfsp = 0;
    ss[q].MaSdt =0;
    ss[q].XcapA= 0;
    ss[q].XcapB= 0;
    ss[q].XcapF =0;
    ss[q].XfIEC= 0;
    ss[q].Xlam = 0;
    ss[q].sdref= 0;
    ss[q].sdval= 0;
    // Equstat ------------------------------------------------------------------
    memset( &st[q].N, 0, 8*sizeof(short));
    memset( &st[q].P, 0, 10*sizeof(float));
    st[q].nnf = 0;
    st[q].llf = 0;
    st[q].phf = 0;
    st[q].B = 0;
    st[q].U = 0;
    st[q].Y = 0;
    st[q].lnGam = 0;
    st[q].Ba = 0;
    st[q].VM = 0;
    st[q].DIC = 0;
    st[q].SBp = 0;
    st[q].SMp = 0;
    st[q].SFp = 0;
}



//--------------------- End of m_syst2.cpp ---------------------------
