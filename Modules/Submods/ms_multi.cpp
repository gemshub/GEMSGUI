//-------------------------------------------------------------------
// $Id: ms_multi.cpp 1353 2009-07-14 14:13:35Z gems $
//
// Implementation of TMulti class, configuration functions
//
// Rewritten from C to C++ by S.Dmytriyeva
// Copyright (C) 1995-2007 S.Dmytriyeva, D.Kulik
//
// This file is part of a GEM-Selektor library for thermodynamic
// modelling by Gibbs energy minimization and of the
// standalone GEMS4K code (define IPMGEMPLUGIN).
//
// This file may be distributed under the terms of GEMS4 Development
// Quality Assurance Licence (GEMS4.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//

#include "tmltsystem.h"
#include "ms_system.h"
#include "m_param.h"
#include "v_object.h"
#include "service.h"

TMultiSystem* TMultiSystem::sm;

// Constructor
//
TMultiSystem::TMultiSystem( int nrt ):
        TSubModule( nrt )
{
    sizeFI = 0;
    phKinMet = 0;

    // internal class instances
    pap = &TProfil::pm->pa;
    pBaseSyst = new TBaseSystem( pap );
    pActivity = new TActivity( pBaseSyst );

    pmp = pActivity->pmp;
    acp = &pActivity->act;

    pKinetics = new TKinetics( pmp->FI );
    pKinetics->set_def();
}


/// Set default information
void TMultiSystem::set_def( int /*q*/)
{
    pBaseSyst->set_def();
    pActivity->set_def();
}


const char* TMultiSystem::GetHtml()
{
       return GSM_MULTI_HTML;
}

// link values to objects
void TMultiSystem::ods_link( int )
{
    //aObj[ o_wd_stkey].SetPtr( pmp->stkey );
    //aObj[ o_wd_nv_].SetPtr( pmp->NV_ );
    aObj[ o_wi_pun ].SetPtr( &pmp->PunE ); /*a 4*/
    aObj[ o_wi_nnr ].SetPtr( &pmp->N );    /*l 2*/
    aObj[ o_wi_ldim ].SetPtr( &pmp->L );   /*l 6*/
    aObj[ o_wi_fdim ].SetPtr( &pmp->FI );  /*l 3*/
    aObj[ o_wo_fdim ].SetPtr( &pmp->FI1 ); /*l 3*/
    aObj[ o_wo_it ].SetPtr( &pmp->IT );
aObj[ o_wo_it_efd ].SetPtr( &pmp->ITF ); /* pmp->ITF */
aObj[ o_wo_it_ipm ].SetPtr( &pmp->ITG ); /* pmp->ITG */
    aObj[ o_wi_mode ].SetPtr( &pmp->E );   /*l 4*/
    aObj[ o_wo_ipar ].SetPtr( &pmp->Ec );  /*l 17*/
    aObj[ o_wio_tc ].SetPtr( &pmp->TC );   /*d 2*/
    aObj[ o_wio_t ].SetPtr( &pmp->T );     /*d 2*/
    aObj[ o_wio_p ].SetPtr( &pmp->P );     /*d 2*/
    aObj[ o_wo_vx ].SetPtr( &pmp->VX_ );   /*d 2*/
    aObj[ o_wo_gx ].SetPtr( &pmp->GX_ );   /*d 2*/
    aObj[ o_wo_ax ].SetPtr( &pmp->AX_ );   /*d 2*/
    aObj[ o_wo_ux ].SetPtr( &pmp->UX_ );   /*d 2*/
    aObj[ o_wo_hx ].SetPtr( &pmp->HX_ );   /*d 2*/
    aObj[ o_wo_sx ].SetPtr( &pmp->SX_ );   /*d 2*/
    aObj[ o_wo_cpx ].SetPtr( &pmp->CpX_ ); /*d 2*/
    aObj[ o_wo_cvx ].SetPtr( &pmp->CvX_ ); /*d 2*/
    aObj[ o_wi_ktau2 ].SetPtr( &pmp->kTau );
    aObj[ o_wi_t0 ].SetPtr( &pmp->TMols );
    aObj[ o_wi_ve ].SetPtr( &pmp->SMols );    /*VE*/
    aObj[ o_wi_mbx ].SetPtr( &pmp->MBX );
    aObj[ o_wo_fx ].SetPtr( &pmp->FX );
    aObj[ o_wo_ic ].SetPtr( &pmp->IC );
    aObj[ o_wd_ph ].SetPtr( &pmp->pH );
    aObj[ o_wd_pe ].SetPtr( &pmp->pe );
    aObj[ o_wd_eh ].SetPtr( &pmp->Eh );
    aObj[ o_wi_mol ].SetPtr( &pmp->DHBM );
    aObj[ o_wi_gwat ].SetPtr( &pmp->GWAT );
    aObj[ o_wi_ymet ].SetPtr( &pmp->YMET );
    aObj[ o_wio_denw ].SetPtr(&acp->denW ); /*d 10*/
    aObj[ o_wio_epsw ].SetPtr(&acp->epsW ); /*d 10*/
    aObj[ o_wo_psi ].SetPtr( &pmp->PCI );   /*d 2*/
    aObj[ o_wo_lnp ].SetPtr( &pmp->lnP );
    aObj[ o_wo_rt ].SetPtr( &pmp->RT );
    aObj[ o_wo_frt ].SetPtr( &pmp->FRT );
    aObj[ o_wo_yw ].SetPtr( &pmp->Yw );
    aObj[ o_wio_cons ].SetPtr(&pmp->ln5551 );/*d 6*/
    aObj[ o_wio_pmxs ].SetPtr(pmp->FitVar ); /*d 1*/
    aObj[ o_wio_fitv ].SetPtr(pmp->FitVar+1 ); /*d 4*/
    aObj[ o_wd_sitn ].SetPtr(&pmp->sitNcat );

    // dynamic DODs, part 1
    aObj[ o_wi_l1 ].SetPtr( pmp->L1 );
    aObj[ o_wi_l1 ].SetDim( pmp->FI, 1 );
//  Added 07.12.2006 - new object for solution models  KD
aObj[ o_wi_ipxpm ].SetPtr( acp->IPx_ );
//  Dimension is calculated on MULTI loading
    aObj[ o_wi_lsmod ].SetPtr(acp->LsMod_ );
// aObj[ o_wi_lsmod ].SetDim( pmp->FIs, 1 );  changed 07.12.2006  KD
aObj[ o_wi_lsmod ].SetDim( pmp->FIs, 3 ); // 3 columns
    aObj[ o_wi_lsmdc ].SetPtr(acp->LsMdc_ );
    aObj[ o_wi_lsmdc ].SetDim( pmp->FIs, 3 );
    aObj[ o_wi_mui ].SetPtr( pmp->mui );
    aObj[ o_wi_mui ].SetDim( pmp->N, 1 );
    aObj[ o_wi_muk ].SetPtr( pmp->muk );
    aObj[ o_wi_muk ].SetDim( pmp->FI, 1 );
    aObj[ o_wi_muj ].SetPtr( pmp->muj );
    aObj[ o_wi_muj ].SetDim( pmp->L, 1 );
    aObj[ o_wi_dul ].SetPtr( pmp->DUL );
    aObj[ o_wi_dul ].SetDim( pmp->L, 1 );
    aObj[ o_wi_dll ].SetPtr( pmp->DLL );
    aObj[ o_wi_dll ].SetDim( pmp->L, 1 );
    aObj[ o_wi_gex ].SetPtr( pmp->fDQF );
    aObj[ o_wi_gex ].SetDim( pmp->L, 1 );
    aObj[ o_wi_pul ].SetPtr( pmp->PUL );
    aObj[ o_wi_pul ].SetDim( pmp->FIs, 1 );
    aObj[ o_wi_pll ].SetPtr( pmp->PLL );
    aObj[ o_wi_pll ].SetDim( pmp->FIs, 1 );
    aObj[ o_wi_yof ].SetPtr( pmp->YOF );
    aObj[ o_wi_yof ].SetDim( pmp->FI, 1 );

//
    /* SIZE OF PMc- CALC M1=1 */
    aObj[ o_wi_pmc ].SetPtr( acp->PMc_ );  // OBsetDim( o_wi_pmc,  M1, 1 );
    /* SIZE OF DMc- CALC M2=1 */
    aObj[ o_wi_dmc ].SetPtr( acp->DMc_ );      // OBsetDim( o_wi_Dmc,  M2, 1 );
    aObj[ o_wi_moisn ].SetPtr( acp->MoiSN_ );  // OBsetDim( o_wi_moisn,  M3, 1 );
    aObj[ o_wo_sitfr ].SetPtr( acp->SitFr_ );  // work array of site fractions
    aObj[ o_wio_vol ].SetPtr( pmp->Vol );
    aObj[ o_wio_vol ].SetDim( pmp->L, 1 );
    aObj[ o_w_hym ].SetPtr( pmp->HYM );
    aObj[ o_w_hym ].SetDim( pmp->L, 1 );
    aObj[ o_w_vl ].SetPtr( pmp->VL );
    aObj[ o_w_vl ].SetDim( pmp->L, 1 );
    aObj[ o_wi_mm ].SetPtr( pmp->MM );
    aObj[ o_wi_mm ].SetDim( pmp->L, 1 );
    aObj[ o_wio_h0 ].SetPtr( pmp->H0 );
    aObj[ o_wio_h0 ].SetDim( pmp->L, 1 );
    aObj[ o_wio_a0 ].SetPtr( pmp->A0 );
    aObj[ o_wio_a0 ].SetDim( pmp->L, 1 );
    aObj[ o_wio_u0 ].SetPtr( pmp->U0 );
    aObj[ o_wio_u0 ].SetDim( pmp->L, 1 );
    aObj[ o_wio_s0 ].SetPtr( pmp->S0 );
    aObj[ o_wio_s0 ].SetDim( pmp->L, 1 );
    aObj[ o_wio_cp0 ].SetPtr( pmp->Cp0 );
    aObj[ o_wio_cp0 ].SetDim( pmp->L, 1 );
    aObj[ o_wio_cv0 ].SetPtr( pmp->Cv0 );
    aObj[ o_wio_cv0 ].SetDim( pmp->L, 1 );
    aObj[ o_wi_pparc ].SetPtr(acp->Pparc_ );
    aObj[ o_wi_pparc ].SetDim( pmp->L, 1 );

    // part 2
    aObj[o_wi_satndx].SetPtr( pmp->SATX );  // changed 27.10.2004 for CD MUSIC
    if( pmp->FIat > 0 ) aObj[ o_wi_satndx ].SetDim( pmp->Lads, 4 );
//
    aObj[ o_wd_ym].SetPtr( pmp->Y_m );
    aObj[ o_wd_ym ].SetDim( pmp->L, 1 );
    aObj[ o_wd_yla].SetPtr( pmp->Y_la );
    aObj[ o_wd_yla ].SetDim( pmp->L, 1 );
    aObj[ o_wd_yw].SetPtr( pmp->Y_w );
    aObj[ o_wd_yw ].SetDim( pmp->L, 1 );
    aObj[ o_wd_gamma].SetPtr( pmp->Gamma );
    aObj[ o_wd_gamma ].SetDim( pmp->L, 1);
    aObj[ o_wi_lngmf].SetPtr( pmp->lnGmf );
    aObj[ o_wi_lngmf ].SetDim( pmp->L, 1);
    aObj[ o_wio_lngmm].SetPtr(pmp->lnGmM );
    aObj[ o_wio_lngmm ].SetDim(pmp->L, 1);
    aObj[ o_wi_ez].SetPtr( pmp->EZ );
    aObj[ o_wi_ez ].SetDim( pmp->L, 1 );
    aObj[ o_wi_aalp].SetPtr( pmp->Aalp );
    aObj[ o_wi_aalp ].SetDim( pmp->FI, 1 );
    aObj[ o_wi_sigw].SetPtr( pmp->Sigw );
    aObj[ o_wi_sigw ].SetDim( pmp->FI, 1 );
    aObj[ o_wi_sigg].SetPtr( pmp->Sigg );
    aObj[ o_wi_sigg ].SetDim( pmp->FI, 1 );
    aObj[ o_wi_nfsp].SetPtr( pmp->Nfsp );
    aObj[o_wi_nfsp].SetDim(pmp->FIs,pmp->FIat);
    aObj[ o_wi_masdt].SetPtr( pmp->MASDT );
    aObj[o_wi_masdt].SetDim(pmp->FIs,pmp->FIat);
    //    aObj[ o_wi_misdt].SetPtr( pmp->MISDT );  aObj[o_wi_misdt].SetDim(pmp->FIs,pmp->FIat);
    aObj[ o_wd_fvol].SetPtr( pmp->FVOL );
    aObj[ o_wd_fvol ].SetDim( pmp->FI, 1 );
    aObj[ o_wd_fwgt].SetPtr( pmp->FWGT );
    aObj[ o_wd_fwgt ].SetDim( pmp->FI, 1 );
    aObj[ o_w_xcapa].SetPtr( pmp->XcapA );
    aObj[o_w_xcapa].SetDim(pmp->FIs,pmp->FIat);
    aObj[ o_w_xcapb].SetPtr( pmp->XcapB );
    aObj[o_w_xcapb].SetDim(pmp->FIs,pmp->FIat);
    aObj[ o_w_xcapd].SetPtr( pmp->XcapD );
    aObj[o_w_xcapd].SetDim(pmp->FIs,pmp->FIat);
    aObj[ o_w_xcapf].SetPtr( pmp->XcapF );
    aObj[o_w_xcapf].SetDim(pmp->FIs,pmp->FIat);
    aObj[ o_wo_xdla].SetPtr( pmp->XdlA );
    aObj[ o_wo_xdla].SetDim(pmp->FIs,pmp->FIat);
    aObj[ o_wo_xdlb].SetPtr( pmp->XdlB );
    aObj[ o_wo_xdlb].SetDim(pmp->FIs,pmp->FIat);
    aObj[ o_wo_xdld].SetPtr( pmp->XdlD );
    aObj[ o_wo_xdld].SetDim(pmp->FIs,pmp->FIat);
    aObj[ o_wo_psia].SetPtr( pmp->XpsiA );
    aObj[o_wo_psia].SetDim(pmp->FIs,pmp->FIat);
    aObj[ o_wo_psib].SetPtr( pmp->XpsiB );
    aObj[o_wo_psib].SetDim(pmp->FIs,pmp->FIat);
    aObj[ o_wo_psid].SetPtr( pmp->XpsiD );
    aObj[o_wo_psid].SetDim(pmp->FIs,pmp->FIat);
    aObj[ o_wi_xr0h0].SetPtr( pmp->Xr0h0 );
    aObj[o_wi_xr0h0 ].SetDim(pmp->FI, 2);
    aObj[ o_wi_xlama].SetPtr( pmp->XlamA );
    aObj[o_wi_xlama].SetDim(pmp->FIs,pmp->FIat);
    aObj[ o_wi_xetaf].SetPtr( pmp->Xetaf );
    aObj[o_wi_xetaf].SetDim(pmp->FIs,pmp->FIat);
    aObj[ o_wi_xcond].SetPtr( pmp->Xcond );
    aObj[ o_wi_xcond ].SetDim(pmp->FI, 1 );
    aObj[ o_wi_xeps].SetPtr( pmp->Xeps );
    aObj[ o_wi_xeps ].SetDim( pmp->FI, 1 );
    aObj[ o_wi_awt].SetPtr( pmp->Awt );
    aObj[ o_wi_awt ].SetDim( pmp->N, 1 );
    aObj[ o_wi_a].SetPtr( pmp->A );
    aObj[ o_wi_a ].SetDim( pmp->L, pmp->N );
    aObj[ o_wd_xfs].SetPtr( pmp->XFs );
    aObj[ o_wd_xfs ].SetDim( pmp->FI, 1 );
    aObj[ o_wd_falps].SetPtr( pmp->Falps );
    aObj[ o_wd_falps ].SetDim( pmp->FI, 1 );
    aObj[ o_wo_g].SetPtr( pmp->G );
    aObj[ o_wo_g ].SetDim( pmp->L, 1 );
    aObj[ o_wi_g0].SetPtr( pmp->G0 );
    aObj[ o_wi_g0 ].SetDim( pmp->L, 1 );
    aObj[ o_wo_lngam].SetPtr( pmp->lnGam );
    aObj[ o_wo_lngam ].SetDim( pmp->L, 1);
    aObj[ o_w_lngmo].SetPtr( pmp->lnGmo );
    aObj[ o_w_lngmo ].SetDim( pmp->L, 1 );
    aObj[ o_wi_b].SetPtr( pmp->B );
    aObj[ o_wi_b ].SetDim( pmp->N, 1 );
    aObj[ o_wo_u].SetPtr( pmp->U );
    aObj[ o_wo_u ].SetDim( pmp->N, 1 );
    aObj[ o_wd_uc].SetPtr( pmp->Uc );
    aObj[ o_wd_uc ].SetDim( pmp->N, 2 );
    aObj[ o_wd_uefd].SetPtr( pmp->Uefd );
    aObj[ o_wd_uefd ].SetDim( pmp->N, 1 );
    aObj[ o_wd_ur].SetPtr( pmp->U_r );
    aObj[ o_wd_ur ].SetDim( pmp->N, 1 );
    aObj[ o_wo_c].SetPtr( pmp->C );
    aObj[ o_wo_c ].SetDim( pmp->N, 1 );
    aObj[ o_wd_icm].SetPtr( pmp->IC_m );
    aObj[ o_wd_icm ].SetDim( pmp->N, 1 );
    aObj[ o_wd_iclcm].SetPtr( pmp->IC_lm );
    aObj[ o_wd_iclcm ].SetDim( pmp->N, 1 );
    aObj[ o_wd_icwm].SetPtr( pmp->IC_wm );
    aObj[ o_wd_icwm ].SetDim( pmp->N, 1 );

    // part 3
    aObj[ o_w_xfts].SetPtr( pmp->XFTS );
    aObj[ o_w_xfts ].SetDim(pmp->FIs,pmp->FIat);
//  Changed 27.10.2004 for CD MUSIC
    aObj[ o_wo_masdj].SetPtr( pmp->MASDJ );
    aObj[ o_wo_masdj ].SetDim( pmp->Lads, DFCN );
//    aObj[ o_wo_masdj ].SetDim( pmp->Ls, 1 );
    aObj[ o_wo_bf].SetPtr( pmp->BF );
    aObj[ o_wo_bf ].SetDim( pmp->FIs, pmp->N );
    aObj[ o_wo_bfc ].SetPtr( pmp->BFC );
    aObj[ o_wo_bfc ].SetDim( pmp->N, 1 );// corrected 07.07.2013 for correct use in scripts
aObj[ o_wo_bfc3 ].SetPtr( pmp->BFC );  // Added 22.10.2013
aObj[ o_wo_bfc3 ].SetDim( 1, pmp->N );// for displaying in EqDemo and other screenforms
    aObj[ o_w_xf].SetPtr( pmp->XF );
    aObj[ o_w_xf ].SetDim( pmp->FI, 1 );
    aObj[ o_wo_yf].SetPtr( pmp->YF );
    aObj[ o_wo_yf ].SetDim( pmp->FI, 1 );
    aObj[ o_ww_xfa].SetPtr( pmp->XFA );
    aObj[ o_ww_xfa ].SetDim( pmp->FIs, 1 );
    aObj[ o_ww_yfa].SetPtr( pmp->YFA );
    aObj[ o_ww_yfa ].SetDim( pmp->FIs, 1 );
    aObj[ o_wo_falp].SetPtr( pmp->Falp );
    aObj[ o_wo_falp ].SetDim( pmp->FI, 1 );
    aObj[ o_wo_gamfs].SetPtr( pmp->GamFs );
    aObj[ o_wo_gamfs].SetDim( pmp->L, 1 );
    aObj[ o_w_xetaa].SetPtr( pmp->XetaA );
    aObj[ o_w_xetaa].SetDim(pmp->FIs,pmp->FIat);
    aObj[ o_w_xetab].SetPtr( pmp->XetaB );
    aObj[ o_w_xetab].SetDim(pmp->FIs,pmp->FIat);
aObj[ o_w_xetad].SetPtr( pmp->XetaD );    // Added 12.09.05 KD
aObj[ o_w_xetad].SetDim(pmp->FIs,pmp->FIat);
    aObj[ o_w_x].SetPtr( pmp->X );
    aObj[ o_w_x ].SetDim( pmp->L, 1 );
    aObj[ o_wo_y].SetPtr( pmp->Y );
    aObj[ o_wo_y ].SetDim( pmp->L, 1 );
    aObj[ o_w_xy].SetPtr( pmp->XY );
    aObj[ o_w_xy ].SetDim( pmp->L, 1 );
    aObj[ o_w_xu].SetPtr( pmp->XU );
    aObj[ o_w_xu ].SetDim( pmp->L, 1 );
    aObj[ o_wi_qp].SetPtr( pmp->Qp );        // aObj[ o_wi_qp ].SetDim( pmp->, 1 );
    aObj[ o_wi_qd].SetPtr( pmp->Qd );        // aObj[ o_wi_qd ].SetDim( pmp->, 1 );
    aObj[ o_wo_mu].SetPtr( pmp->MU );
    aObj[ o_wo_mu ].SetDim( pmp->L, 1 );
    aObj[ o_w_emu].SetPtr( pmp->EMU );
    aObj[ o_w_emu ].SetDim( pmp->L, 1 );
    aObj[ o_w_nmu].SetPtr( pmp->NMU );
    aObj[ o_w_nmu ].SetDim( pmp->L, 1 );
    aObj[ o_w_w].SetPtr( pmp->W );
    aObj[ o_w_w ].SetDim( pmp->L, 1 );
    aObj[ o_wo_fxa].SetPtr( pmp->Fx );
    aObj[ o_wo_fxa ].SetDim( pmp->L, 1 );
    aObj[ o_wo_wx].SetPtr( pmp->Wx );
    aObj[ o_wo_wx ].SetDim( pmp->L, 1 );
    aObj[ o_wo_f].SetPtr( pmp->F );
    aObj[ o_wo_f ].SetDim( pmp->L, 1 );
    aObj[ o_w_f0].SetPtr( pmp->F0 );
    aObj[ o_w_f0 ].SetDim( pmp->L, 1 );
// Changed for CD MUSIC 27.10.2004 KD
    aObj[ o_w_d ].SetPtr( pmp->D );
    aObj[ o_w_d ].SetDim( MST, MST );
    aObj[ o_wi_smod ].SetPtr( pmp->sMod );
    aObj[ o_wi_smod ].SetDim( pmp->FIs, 1 );
 aObj[ o_wi_dcmod ].SetPtr( acp->dcMod );
 aObj[ o_wi_dcmod ].SetDim( pmp->L, 1 );
    aObj[ o_wd_sb ].SetPtr( pmp->SB );
    aObj[ o_wd_sb ].SetDim( pmp->N, 1 );
    aObj[ o_w_sbh ].SetPtr( pmp->SB1 );
    aObj[ o_w_sbh ].SetDim( 1, pmp->N );
    aObj[ o_wd_sm ].SetPtr( pmp->SM );
    aObj[ o_wd_sm ].SetDim( pmp->L, 1 );
    aObj[ o_wd_sf ].SetPtr( pmp->SF );
    aObj[ o_wd_sf ].SetDim( pmp->FI, 1 );
    aObj[ o_wd_sm2 ].SetPtr( pmp->SM2 );
    aObj[ o_wd_sm2 ].SetDim( pmp->Ls, 1 );
// Added 27.10.2004 for CD MUSIC models
    aObj[ o_wd_sm3 ].SetPtr( pmp->SM3 );
    aObj[ o_wd_sm3 ].SetDim( pmp->Lads, 1 );
//
    aObj[ o_wd_sf2].SetPtr( pmp->SF2 );
    aObj[ o_wd_sf2 ].SetDim( pmp->FIs, 1 );
    aObj[ o_wd_sfs].SetPtr( pmp->SFs );
    aObj[ o_wd_sfs ].SetDim( pmp->FI, 1 );
    aObj[ o_wd_txtb].SetPtr( pmp->pbuf );    //aObj[o_wd_txtb].SetDim(1,size(pmp->pbuf));
    aObj[ o_wi_rlc].SetPtr( pmp->RLC );
    aObj[ o_wi_rlc ].SetDim( pmp->L, 1 );
    aObj[ o_wi_rsc].SetPtr( pmp->RSC );
    aObj[ o_wi_rsc ].SetDim( pmp->L, 1 );
    aObj[ o_wi_rflc].SetPtr( pmp->RFLC );
    aObj[ o_wi_rflc ].SetDim( pmp->FIs, 1 );
    aObj[ o_wi_rfsc].SetPtr( pmp->RFSC );
    aObj[ o_wi_rfsc ].SetDim( pmp->FIs, 1 );
    aObj[ o_wi_icc].SetPtr( pmp->ICC );
    aObj[ o_wi_icc ].SetDim( pmp->N, 1 );
    aObj[ o_wi_dcc].SetPtr( pmp->DCC );
    aObj[ o_wi_dcc ].SetDim( pmp->L, 1 );
//  Added 27.10.2004 for CD MUSIC models
    aObj[ o_wi_dcc3 ].SetPtr( pmp->DCC3 );
    aObj[ o_wi_dcc3 ].SetDim( pmp->Lads, 1 );
//
    aObj[ o_wi_scm].SetPtr( pmp->SCM );
    aObj[ o_wi_scm ].SetDim(pmp->FIs,pmp->FIat);
// Changed 27.10.2004
    aObj[ o_wi_satt].SetPtr( pmp->SATT );
    aObj[ o_wi_satt ].SetDim( pmp->Lads, 1 );
//
    aObj[ o_wi_dccw].SetPtr( pmp->DCCW );
    aObj[ o_wi_dccw ].SetDim( pmp->L, 1 );
    aObj[ o_wi_phc].SetPtr( pmp->PHC );
    aObj[ o_wi_phc ].SetDim( pmp->FI, 1 );
    aObj[ o_wd_fug].SetPtr( pmp->Fug );
    aObj[ o_wd_fug ].SetDim( pmp->PG, 1 );
    aObj[ o_wd_fugl].SetPtr( pmp->Fug_l );
    aObj[ o_wd_fugl ].SetDim( pmp->PG, 1 );
    aObj[ o_wd_ppgl].SetPtr( pmp->Ppg_l );
    aObj[ o_wd_ppgl ].SetDim( pmp->PG, 1 );
    aObj[ o_wio_wb].SetPtr( pmp->Wb );
    aObj[ o_wio_wb ].SetDim( pmp->Ls, 1 );
    aObj[ o_wio_wabs].SetPtr( pmp->Wabs );
    aObj[ o_wio_wabs ].SetDim( pmp->Ls, 1 );
    aObj[ o_wi_rion].SetPtr( pmp->Rion );
    aObj[ o_wi_rion ].SetDim( pmp->Ls, 1 );
//    aObj[ o_wo_lnsat].SetPtr( pmp->lnSAT );
//    aObj[ o_wo_lnsat ].SetDim( pmp->Ls, 1 );
aObj[ o_wo_lnsat].SetPtr( pmp->lnSAC );
aObj[ o_wo_lnsat ].SetDim( pmp->Lads, 4 );

// 11/02/2009
aObj[ o_wo_vph].SetPtr( acp->VPh );
aObj[ o_wo_vph ].SetDim( pmp->FIs, MIXPHPROPS );
aObj[ o_wo_gph].SetPtr( acp->GPh );
aObj[ o_wo_gph ].SetDim( pmp->FIs, MIXPHPROPS );
aObj[ o_wo_hph].SetPtr( acp->HPh );
aObj[ o_wo_hph ].SetDim( pmp->FIs, MIXPHPROPS );
aObj[ o_wo_sph].SetPtr( acp->SPh );
aObj[ o_wo_sph ].SetDim( pmp->FIs, MIXPHPROPS );
aObj[ o_wo_cph].SetPtr( acp->CPh );
aObj[ o_wo_cph ].SetDim( pmp->FIs, MIXPHPROPS );
aObj[ o_wo_aph].SetPtr( acp->APh );
aObj[ o_wo_aph ].SetDim( pmp->FIs, MIXPHPROPS );
aObj[ o_wo_uph].SetPtr( acp->UPh );
aObj[ o_wo_uph ].SetDim( pmp->FIs, MIXPHPROPS );

// New phase stuff 06/06/12
     aObj[ o_wi_lsmdc2].SetPtr( acp->LsMdc2_ );
     aObj[ o_wi_lsmdc2 ].SetDim( pmp->FIs, 3 );
     aObj[ o_wi_lsphl].SetPtr( pmp->LsPhl);
     aObj[ o_wi_lsphl ].SetDim( pmp->FI, 2 );
     aObj[ o_wi_phlin].SetPtr( pmp->PhLin ); //FI
// TSolMod stuff
     aObj[ o_wi_lphc].SetPtr( pmp->lPhc );
     aObj[ o_wi_dqfc].SetPtr( acp->DQFc_ );
//     aObj[ o_wi_rcpc].SetPtr( pmp->rcpc );
////     aObj[ o_wi_lndqft].SetPtr( acp->lnDQFt );
////     aObj[ o_wi_lndqft].SetDim( pmp->Ls, 1 );
////     aObj[ o_wi_lnrcpt].SetPtr( acp->lnRcpt );
////     aObj[ o_wi_lnrcpt].SetDim( pmp->Ls, 1 );
////     aObj[ o_wi_lnexet].SetPtr( acp->lnExet );
////     aObj[ o_wi_lnexet].SetDim( pmp->Ls, 1 );
////     aObj[ o_wi_lncnft].SetPtr( acp->lnCnft );
////     aObj[ o_wi_lncnft].SetDim( pmp->Ls, 1 );
//TSorpMod & TKinMet stuff
     aObj[ o_wi_sormc].SetPtr( acp->SorMc );
     aObj[ o_wi_sormc].SetDim( pmp->FIs, 16 );
// TSorpMod stuff
     aObj[ o_wi_lsesmo].SetPtr( acp->LsESmo );
     aObj[ o_wi_lsesmo].SetDim( pmp->FIs, 4 );
     aObj[ o_wi_lsismo].SetPtr( acp->LsISmo );
     aObj[ o_wi_lsismo].SetDim( pmp->FIs, 4 );
     aObj[ o_wi_xsmd].SetPtr( acp->xSMd );
     aObj[ o_wi_eimc].SetPtr( acp->EImc );
     aObj[ o_wi_mcdc].SetPtr( acp->mCDc );
     aObj[ o_wi_isopc].SetPtr( acp->IsoPc );
     aObj[ o_wi_isosc].SetPtr( acp->IsoSc );
     aObj[ o_wi_lnscal].SetPtr( acp->lnScalT );
     aObj[ o_wi_lnscal].SetDim( pmp->Ls, 1 );
     aObj[ o_wi_lnsact].SetPtr( acp->lnSACT );
     aObj[ o_wi_lnsact].SetDim( pmp->Ls, 1 );
     aObj[ o_wi_lngamf].SetPtr( acp->lnGammF );
     aObj[ o_wi_lngamf].SetDim( pmp->Ls, 1 );
     aObj[ o_wi_cterms].SetPtr( acp->CTerms );
     aObj[ o_wi_cterms].SetDim( pmp->Ls, 1 );
     aObj[ o_wi_isoct].SetPtr( acp->IsoCt );
// TKinMet stuff
        aObj[ o_wi_lskin].SetPtr( pmp->LsKin );
        aObj[ o_wi_lskin].SetDim( pmp->FI, 6 );
        aObj[ o_wi_lsupt].SetPtr( pmp->LsUpt );
        aObj[ o_wi_lsupt].SetDim( pmp->FIs, 2 );
        aObj[ o_wi_jcrdc].SetPtr( pmp->xSKrC );
        aObj[ o_wi_ocprkc].SetPtr( pmp->ocPRkC );
        aObj[ o_wi_fsac].SetPtr( pmp->feSArC );
        aObj[ o_wi_krpc].SetPtr( pmp->rpConC );
        aObj[ o_wi_apconc].SetPtr( pmp->apConC );
        aObj[ o_wi_ascpc].SetPtr( pmp->AscpC );
        aObj[ o_wi_umpc].SetPtr( pmp->UMpcC );
        aObj[ o_wi_kmod].SetPtr( pmp-> kMod);
        aObj[ o_wi_kmod].SetDim( pmp->FI, 1 );
aObj[ o_wi_xicuc].SetPtr( pmp->xICuC );
// new
    aObj[ o_wi_pffact ].SetPtr( pmp->PfFact );
    aObj[ o_wi_pffact ].SetDim( pmp->FI, 1 );
    aObj[ o_wo_prto ].SetPtr( pmp->PrT );
    aObj[ o_wo_prto ].SetDim( pmp->FI, 1 );
    aObj[ o_wo_pkto ].SetPtr( pmp->PkT );
    aObj[ o_wo_pkto ].SetDim( pmp->FI, 1 );
    aObj[ o_wo_pvto ].SetPtr( pmp->PvT );
    aObj[ o_wo_pvto ].SetDim( pmp->FI, 1 );
    aObj[ o_wo_emrd ].SetPtr( pmp->emRd );
    aObj[ o_wo_emrd ].SetDim( pmp->Ls, 1 );
    aObj[ o_wo_emdf ].SetPtr( pmp->emDf );
    aObj[ o_wo_emdf ].SetDim( pmp->Ls, 1 );
}

// set dynamic Objects ptr to values
void TMultiSystem::dyn_set(int /*q*/)
{
   pmp->L1    = (long int *)aObj[ o_wi_l1 ].GetPtr();
   acp->IPx_ = (long int *)aObj[ o_wi_ipxpm ].GetPtr();   // added 07.12.2006  KD
   acp->LsMod_ = (long int *)aObj[ o_wi_lsmod ].GetPtr();
    acp->LsMdc_ = (long int *)aObj[ o_wi_lsmdc ].GetPtr();
    pmp->mui   = (long int *)aObj[ o_wi_mui ].GetPtr();
    pmp->muk   = (long int *)aObj[ o_wi_muk ].GetPtr();
    pmp->muj   = (long int *)aObj[ o_wi_muj ].GetPtr();
    pmp->SATX =(long int (*)[4])aObj[ o_wi_satndx ].GetPtr();
    pmp->DUL   = (double *)aObj[ o_wi_dul ].GetPtr();
    pmp->DLL   = (double *)aObj[ o_wi_dll ].GetPtr();
    pmp->fDQF   = (double *)aObj[ o_wi_gex ].GetPtr();
    pmp->PUL   = (double *)aObj[ o_wi_pul ].GetPtr();
    pmp->PLL   = (double *)aObj[ o_wi_pll ].GetPtr();
    pmp->YOF   = (double *)aObj[ o_wi_yof ].GetPtr();
// new
    pmp->PfFact  = (double *)aObj[ o_wi_pffact ].GetPtr();
    pmp->PrT   = (double *)aObj[ o_wo_prto ].GetPtr();
    pmp->PkT   = (double *)aObj[ o_wo_pkto ].GetPtr();
    pmp->PvT   = (double *)aObj[ o_wo_pvto ].GetPtr();
    pmp->emRd   = (double *)aObj[ o_wo_emrd ].GetPtr();
    pmp->emDf   = (double *)aObj[ o_wo_emdf ].GetPtr();
//
    acp->PMc_   = (double *)aObj[ o_wi_pmc ].GetPtr();
    acp->DMc_   = (double *)aObj[ o_wi_dmc ].GetPtr();
    acp->MoiSN_  = (double *)aObj[ o_wi_moisn ].GetPtr();
    acp->SitFr_  = (double *)aObj[ o_wo_sitfr ].GetPtr();
    pmp->Vol   = (double *)aObj[ o_wio_vol ].GetPtr();
    pmp->HYM   = (double *)aObj[ o_w_hym ].GetPtr();
    pmp->VL    = (double *)aObj[ o_w_vl ].GetPtr();
    pmp->MM    = (double *)aObj[ o_wi_mm ].GetPtr();
    pmp->H0    = (double *)aObj[ o_wio_h0 ].GetPtr();
    pmp->A0    = (double *)aObj[ o_wio_a0 ].GetPtr();
    pmp->U0    = (double *)aObj[ o_wio_u0 ].GetPtr();
    pmp->S0    = (double *)aObj[ o_wio_s0 ].GetPtr();
    pmp->Cp0   = (double *)aObj[ o_wio_cp0 ].GetPtr();
    pmp->Cv0   = (double *)aObj[ o_wio_cv0 ].GetPtr();
    acp->Pparc_ = (double *)aObj[ o_wi_pparc ].GetPtr();
    pmp->Y_m   = (double *)aObj[ o_wd_ym ].GetPtr();
    pmp->Y_la  = (double *)aObj[ o_wd_yla ].GetPtr();
    pmp->Y_w   = (double *)aObj[ o_wd_yw ].GetPtr();
    pmp->Gamma = (double *)aObj[ o_wd_gamma ].GetPtr();
    pmp->lnGmf = (double *)aObj[ o_wi_lngmf ].GetPtr();
    pmp->lnGmM = (double *)aObj[ o_wio_lngmm ].GetPtr();
    pmp->EZ    = (double *)aObj[ o_wi_ez ].GetPtr();
    pmp->Wb    = (double *)aObj[ o_wio_wb ].GetPtr();
    pmp->Wabs  = (double *)aObj[ o_wio_wabs ].GetPtr();
    pmp->Rion  = (double *)aObj[ o_wi_rion ].GetPtr();
    pmp->Aalp  = (double *)aObj[ o_wi_aalp ].GetPtr();
    pmp->Sigw  = (double *)aObj[ o_wi_sigw ].GetPtr();
    pmp->Sigg  = (double *)aObj[ o_wi_sigg ].GetPtr();
    pmp->Nfsp  = (double (*)[MST])aObj[ o_wi_nfsp ].GetPtr();
    pmp->MASDT = (double (*)[MST])aObj[ o_wi_masdt ].GetPtr();
    pmp->FVOL  = (double *)aObj[ o_wd_fvol ].GetPtr();
    pmp->FWGT  = (double *)aObj[ o_wd_fwgt ].GetPtr();
    pmp->XcapA = (double (*)[MST])aObj[ o_w_xcapa ].GetPtr();
    pmp->XcapB = (double (*)[MST])aObj[ o_w_xcapb ].GetPtr();
    pmp->XcapD = (double (*)[MST])aObj[ o_w_xcapd ].GetPtr();
    pmp->XcapF = (double (*)[MST])aObj[ o_w_xcapf ].GetPtr();
    pmp->XdlA  = (double (*)[MST])aObj[ o_wo_xdla ].GetPtr();
    pmp->XdlB  = (double (*)[MST])aObj[ o_wo_xdlb ].GetPtr();
    pmp->XdlD  = (double (*)[MST])aObj[ o_wo_xdld ].GetPtr();
    pmp->XpsiA = (double (*)[MST])aObj[ o_wo_psia ].GetPtr();
    pmp->XpsiB = (double (*)[MST])aObj[ o_wo_psib ].GetPtr();
    pmp->XpsiD = (double (*)[MST])aObj[ o_wo_psid ].GetPtr();
    pmp->Xr0h0 = (double (*)[2])aObj[ o_wi_xr0h0 ].GetPtr();
    pmp->XlamA = (double (*)[MST])aObj[ o_wi_xlama ].GetPtr();
    pmp->Xetaf = (double (*)[MST])aObj[ o_wi_xetaf ].GetPtr();
    pmp->Xcond = (double *)aObj[ o_wi_xcond ].GetPtr();
    pmp->Xeps  = (double *)aObj[ o_wi_xeps ].GetPtr();
    pmp->Awt   = (double *)aObj[ o_wi_awt ].GetPtr();
    pmp->A     = (double *)aObj[ o_wi_a ].GetPtr();
    pmp->XFs   = (double *)aObj[ o_wd_xfs ].GetPtr();
    pmp->Falps = (double *)aObj[ o_wd_falps ].GetPtr();
    pmp->Fug   = (double *)aObj[ o_wd_fug ].GetPtr();
    pmp->Fug_l = (double *)aObj[ o_wd_fugl ].GetPtr();
    pmp->Ppg_l = (double *)aObj[ o_wd_ppgl ].GetPtr();
    pmp->XFTS  = (double (*)[MST])aObj[ o_w_xfts ].GetPtr();
    pmp->MASDJ = (double (*)[DFCN])aObj[ o_wo_masdj ].GetPtr();
    pmp->G     = (double *)aObj[ o_wo_g ].GetPtr();
    pmp->G0    = (double *)aObj[ o_wi_g0 ].GetPtr();
    pmp->lnGam = (double *)aObj[ o_wo_lngam ].GetPtr();
    pmp->lnGmo = (double *)aObj[ o_w_lngmo ].GetPtr();
    pmp->lnSAC = (double (*)[4])aObj[ o_wo_lnsat ].GetPtr();
//    pmp->lnSAT = (double *)aObj[ o_wo_lnsat ].GetPtr();
    pmp->B     = (double *)aObj[ o_wi_b ].GetPtr();
    pmp->U     = (double *)aObj[ o_wo_u ].GetPtr();
    pmp->Uc   = (double (*)[2])aObj[ o_wd_uc ].GetPtr();
    pmp->Uefd   = (double *)aObj[ o_wd_uefd ].GetPtr();
    pmp->U_r   = (double *)aObj[ o_wd_ur ].GetPtr();
    pmp->C     = (double *)aObj[ o_wo_c ].GetPtr();
    pmp->IC_m  = (double *)aObj[ o_wd_icm ].GetPtr();
    pmp->IC_lm = (double *)aObj[ o_wd_iclcm ].GetPtr();
    pmp->IC_wm = (double *)aObj[ o_wd_icwm ].GetPtr();
    pmp->BF    = (double *)aObj[ o_wo_bf ].GetPtr();
    pmp->BFC    = (double *)aObj[ o_wo_bfc ].GetPtr();
    pmp->XF    = (double *)aObj[ o_w_xf ].GetPtr();
    pmp->YF    = (double *)aObj[ o_wo_yf ].GetPtr();
    pmp->XFA   = (double *)aObj[ o_ww_xfa ].GetPtr();
    pmp->YFA   = (double *)aObj[ o_ww_yfa ].GetPtr();
    pmp->Falp  = (double *)aObj[ o_wo_falp ].GetPtr();
pmp->GamFs  = (double *)aObj[ o_wo_gamfs ].GetPtr();
    pmp->XetaA = (double (*)[MST])aObj[ o_w_xetaa ].GetPtr();
    pmp->XetaB = (double (*)[MST])aObj[ o_w_xetab ].GetPtr();
    pmp->XetaD = (double (*)[MST])aObj[ o_w_xetad ].GetPtr(); // added 12.09.05 KD
    pmp->X     = (double *)aObj[ o_w_x ].GetPtr();
    pmp->Y     = (double *)aObj[ o_wo_y ].GetPtr();
    pmp->XY    = (double *)aObj[ o_w_xy ].GetPtr();
    pmp->XU    = (double *)aObj[ o_w_xu ].GetPtr();
    pmp->Qp    = (double *)aObj[ o_wi_qp ].GetPtr();
    pmp->Qd    = (double *)aObj[ o_wi_qd ].GetPtr();
    pmp->MU    = (double *)aObj[ o_wo_mu ].GetPtr();
    pmp->EMU   = (double *)aObj[ o_w_emu ].GetPtr();
    pmp->NMU   = (double *)aObj[ o_w_nmu ].GetPtr();
    pmp->W     = (double *)aObj[ o_w_w ].GetPtr();
    pmp->Fx    = (double *)aObj[ o_wo_fxa ].GetPtr();
    pmp->Wx    = (double *)aObj[ o_wo_wx ].GetPtr();
    pmp->F     = (double *)aObj[ o_wo_f ].GetPtr();
    pmp->F0    = (double *)aObj[ o_w_f0 ].GetPtr();
    pmp->D     = (double (*)[MST])aObj[ o_w_d ].GetPtr();
//    pmp->R     = (double *)aObj[ o_w_r ].GetPtr();
//    pmp->R1    = (double *)aObj[ o_w_r1 ].GetPtr();
    pmp->sMod  = (char (*)[8])aObj[ o_wi_smod ].GetPtr();
 acp->dcMod  = (char (*)[6])aObj[ o_wi_dcmod ].GetPtr();
    pmp->SB    = (char (*)[MAXICNAME+MAXSYMB])aObj[ o_wd_sb ].GetPtr();
    pmp->SB1    = (char (*)[MAXICNAME])aObj[ o_w_sbh ].GetPtr();
    pmp->SM    = (char (*)[MAXDCNAME])aObj[ o_wd_sm ].GetPtr();
    pmp->SF    = (char (*)[MAXPHNAME+MAXSYMB])aObj[ o_wd_sf ].GetPtr();
    pmp->SM2    = (char (*)[MAXDCNAME])aObj[ o_wd_sm2 ].GetPtr();
    pmp->SM3    = (char (*)[MAXDCNAME])aObj[ o_wd_sm3 ].GetPtr();
    pmp->SF2    = (char (*)[MAXPHNAME+MAXSYMB])aObj[ o_wd_sf2 ].GetPtr();
    pmp->SFs   = (char (*)[MAXPHNAME+MAXSYMB])aObj[ o_wd_sfs ].GetPtr();
    pmp->pbuf  = (char *)aObj[ o_wd_txtb ].GetPtr();
    pmp->RLC   = (char *)aObj[ o_wi_rlc ].GetPtr();
    pmp->RSC   = (char *)aObj[ o_wi_rsc ].GetPtr();
    pmp->RFLC  = (char *)aObj[ o_wi_rflc ].GetPtr();
    pmp->RFSC  = (char *)aObj[ o_wi_rfsc ].GetPtr();
    pmp->ICC   = (char *)aObj[ o_wi_icc ].GetPtr();
    pmp->DCC   = (char *)aObj[ o_wi_dcc ].GetPtr();
    pmp->DCC3   = (char *)aObj[ o_wi_dcc3 ].GetPtr();
    pmp->PHC   = (char *)aObj[ o_wi_phc ].GetPtr();
    pmp->SCM   = (char (*)[MST])aObj[ o_wi_scm ].GetPtr();
    pmp->SATT  = (char *)aObj[ o_wi_satt ].GetPtr();
    pmp->DCCW  = (char *)aObj[ o_wi_dccw ].GetPtr();
    // 11/02/2009
    acp->VPh = (double (*)[MIXPHPROPS])aObj[ o_wo_vph].GetPtr();
    acp->GPh = (double (*)[MIXPHPROPS])aObj[ o_wo_gph].GetPtr();
    acp->HPh = (double (*)[MIXPHPROPS])aObj[ o_wo_hph].GetPtr();
    acp->SPh = (double (*)[MIXPHPROPS])aObj[ o_wo_sph].GetPtr();
    acp->CPh = (double (*)[MIXPHPROPS])aObj[ o_wo_cph].GetPtr();
    acp->APh = (double (*)[MIXPHPROPS])aObj[ o_wo_aph].GetPtr();
    acp->UPh = (double (*)[MIXPHPROPS])aObj[ o_wo_uph].GetPtr();
    // New phase stuff 06/06/12
    acp->LsMdc2_  = (long int *)aObj[ o_wi_lsmdc2].GetPtr();
    pmp->LsPhl   = (long int *)aObj[ o_wi_lsphl].GetPtr();
    pmp->PhLin   = (long int (*)[2])aObj[ o_wi_phlin].GetPtr();
    // TSolMod stuff
    pmp->lPhc   = (double *)aObj[ o_wi_lphc].GetPtr();
    acp->DQFc_   = (double *)aObj[ o_wi_dqfc].GetPtr();
//    pmp->rcpc   = (double *)aObj[ o_wi_rcpc].GetPtr();
    //pmp->lnDQFt   = (double *)aObj[ o_wi_lndqft].GetPtr();
    //pmp->lnRcpt   = (double *)aObj[ o_wi_lnrcpt].GetPtr();
    //pmp->lnExet   = (double *)aObj[ o_wi_lnexet].GetPtr();
    //pmp->lnCnft   = (double *)aObj[ o_wi_lncnft].GetPtr();
    //TSorpMod & TKinMet stuff
    acp->SorMc   = (double *)aObj[ o_wi_sormc].GetPtr();
    // TSorpMod stuff
    acp->LsESmo   = (long int *)aObj[ o_wi_lsesmo].GetPtr();
    acp->LsISmo   = (long int *)aObj[ o_wi_lsismo].GetPtr();
    acp->xSMd   = (long int *)aObj[ o_wi_xsmd].GetPtr();
    acp->EImc   = (double *)aObj[ o_wi_eimc].GetPtr();
    acp->mCDc   = (double *)aObj[ o_wi_mcdc].GetPtr();
    acp->IsoPc   = (double *)aObj[ o_wi_isopc].GetPtr();
    acp->IsoSc   = (double *)aObj[ o_wi_isosc].GetPtr();
    acp->lnScalT   = (double *)aObj[ o_wi_lnscal].GetPtr();
    acp->lnSACT   = (double *)aObj[ o_wi_lnsact].GetPtr();
    acp->lnGammF   = (double *)aObj[ o_wi_lngamf].GetPtr();
    acp->CTerms   = (double *)aObj[ o_wi_cterms].GetPtr();
    acp->IsoCt   = (char *)aObj[ o_wi_isoct].GetPtr();
    // TKinMet stuff
    pmp->LsKin   = (long int *)aObj[ o_wi_lskin].GetPtr();
    pmp->LsUpt   = (long int *)aObj[ o_wi_lsupt].GetPtr();
    pmp->xSKrC   = (long int *)aObj[ o_wi_jcrdc].GetPtr();
    pmp->ocPRkC   = (long int (*)[2])aObj[ o_wi_ocprkc].GetPtr();
    pmp->feSArC   = (double *)aObj[ o_wi_fsac].GetPtr();
    pmp->rpConC   = (double *)aObj[ o_wi_krpc].GetPtr();
    pmp->apConC   = (double *)aObj[ o_wi_apconc].GetPtr();
    pmp->AscpC   = (double *)aObj[ o_wi_ascpc].GetPtr();
    pmp->UMpcC   = (double *)aObj[ o_wi_umpc].GetPtr();
    pmp->kMod   = (char(*)[6])aObj[ o_wi_kmod].GetPtr();
pmp->xICuC   = (long int *)aObj[ o_wi_xicuc].GetPtr();
}


// free dynamic memory in objects and values
void TMultiSystem::dyn_kill( int /*q*/)
{
    pmp->L1    = (long int *)aObj[ o_wi_l1 ].Free();
    acp->IPx_ = (long int *)aObj[ o_wi_ipxpm ].Free();   // added 07.12.2006  KD
    acp->LsMod_ = (long int *)aObj[ o_wi_lsmod ].Free();
    acp->LsMdc_ = (long int *)aObj[ o_wi_lsmdc ].Free();
    pmp->mui   = (long int *)aObj[ o_wi_mui ].Free();
    pmp->muk   = (long int *)aObj[ o_wi_muk ].Free();
    pmp->muj   = (long int *)aObj[ o_wi_muj ].Free();
    pmp->SATX =(long int (*)[4])aObj[ o_wi_satndx ].Free();
    pmp->DUL   = (double *)aObj[ o_wi_dul ].Free();
    pmp->DLL   = (double *)aObj[ o_wi_dll ].Free();
    pmp->fDQF   = (double *)aObj[ o_wi_gex ].Free();
    pmp->PUL   = (double *)aObj[ o_wi_pul ].Free();
    pmp->PLL   = (double *)aObj[ o_wi_pll ].Free();
    pmp->YOF   = (double *)aObj[ o_wi_yof ].Free();
//  new
    pmp->PfFact   = (double *)aObj[ o_wi_pffact ].Free();
    pmp->PrT   = (double *)aObj[ o_wo_prto ].Free();
    pmp->PkT   = (double *)aObj[ o_wo_pkto ].Free();
    pmp->PvT   = (double *)aObj[ o_wo_pvto ].Free();
    pmp->emRd   = (double *)aObj[ o_wo_emrd ].Free();
    pmp->emDf   = (double *)aObj[ o_wo_emdf ].Free();
//
    acp->PMc_   = (double *)aObj[ o_wi_pmc ].Free();
    acp->DMc_   = (double *)aObj[ o_wi_dmc ].Free();
    acp->MoiSN_  = (double *)aObj[ o_wi_moisn ].Free();
    acp->SitFr_  = (double *)aObj[ o_wo_sitfr ].Free();
    pmp->Vol   = (double *)aObj[ o_wio_vol ].Free();
    pmp->HYM   = (double *)aObj[ o_w_hym ].Free();
    pmp->VL    = (double *)aObj[ o_w_vl ].Free();
    pmp->MM    = (double *)aObj[ o_wi_mm ].Free();
    pmp->H0    = (double *)aObj[ o_wio_h0 ].Free();
    pmp->A0    = (double *)aObj[ o_wio_a0 ].Free();
    pmp->U0    = (double *)aObj[ o_wio_u0 ].Free();
    pmp->S0    = (double *)aObj[ o_wio_s0 ].Free();
    pmp->Cp0   = (double *)aObj[ o_wio_cp0 ].Free();
    pmp->Cv0   = (double *)aObj[ o_wio_cv0 ].Free();
    acp->Pparc_ = (double *)aObj[ o_wi_pparc ].Free();
    pmp->Y_m   = (double *)aObj[ o_wd_ym ].Free();
    pmp->Y_la  = (double *)aObj[ o_wd_yla ].Free();
    pmp->Y_w   = (double *)aObj[ o_wd_yw ].Free();
    pmp->Gamma = (double *)aObj[ o_wd_gamma ].Free();
    pmp->lnGmf = (double *)aObj[ o_wi_lngmf ].Free();
    pmp->lnGmM = (double *)aObj[ o_wio_lngmm ].Free();
    pmp->EZ    = (double *)aObj[ o_wi_ez ].Free();
    pmp->Wb    = (double *)aObj[ o_wio_wb ].Free();
    pmp->Wabs  = (double *)aObj[ o_wio_wabs ].Free();
    pmp->Rion  = (double *)aObj[ o_wi_rion ].Free();
    pmp->Aalp  = (double *)aObj[ o_wi_aalp ].Free();
    pmp->Sigw  = (double *)aObj[ o_wi_sigw ].Free();
    pmp->Sigg  = (double *)aObj[ o_wi_sigg ].Free();
    pmp->Nfsp  = (double (*)[MST])aObj[ o_wi_nfsp ].Free();
    pmp->MASDT = (double (*)[MST])aObj[ o_wi_masdt ].Free();
    pmp->FVOL  = (double *)aObj[ o_wd_fvol ].Free();
    pmp->FWGT  = (double *)aObj[ o_wd_fwgt ].Free();
    pmp->XcapA = (double (*)[MST])aObj[ o_w_xcapa ].Free();
    pmp->XcapB = (double (*)[MST])aObj[ o_w_xcapb ].Free();
    pmp->XcapD = (double (*)[MST])aObj[ o_w_xcapd ].Free();
    pmp->XcapF = (double (*)[MST])aObj[ o_w_xcapf ].Free();
    pmp->XdlA  = (double (*)[MST])aObj[ o_wo_xdla ].Free();
    pmp->XdlB  = (double (*)[MST])aObj[ o_wo_xdlb ].Free();
    pmp->XdlD  = (double (*)[MST])aObj[ o_wo_xdld ].Free();
    pmp->XpsiA = (double (*)[MST])aObj[ o_wo_psia ].Free();
    pmp->XpsiB = (double (*)[MST])aObj[ o_wo_psib ].Free();
    pmp->XpsiD = (double (*)[MST])aObj[ o_wo_psid ].Free();
    pmp->Xr0h0 = (double (*)[2])aObj[ o_wi_xr0h0 ].Free();
    pmp->XlamA = (double (*)[MST])aObj[ o_wi_xlama ].Free();
    pmp->Xetaf = (double (*)[MST])aObj[ o_wi_xetaf ].Free();
    pmp->Xcond = (double *)aObj[ o_wi_xcond ].Free();
    pmp->Xeps  = (double *)aObj[ o_wi_xeps ].Free();
    pmp->Awt   = (double *)aObj[ o_wi_awt ].Free();
    pmp->A     = (double *)aObj[ o_wi_a ].Free();
    pmp->XFs   = (double *)aObj[ o_wd_xfs ].Free();
    pmp->Falps = (double *)aObj[ o_wd_falps ].Free();
    pmp->Fug   = (double *)aObj[ o_wd_fug ].Free();
    pmp->Fug_l = (double *)aObj[ o_wd_fugl ].Free();
    pmp->Ppg_l = (double *)aObj[ o_wd_ppgl ].Free();
    pmp->XFTS  = (double (*)[MST])aObj[ o_w_xfts ].Free();
    pmp->MASDJ = (double (*)[DFCN])aObj[ o_wo_masdj ].Free();
    pmp->G     = (double *)aObj[ o_wo_g ].Free();
    pmp->G0    = (double *)aObj[ o_wi_g0 ].Free();
    pmp->lnGam = (double *)aObj[ o_wo_lngam ].Free();
    pmp->lnGmo = (double *)aObj[ o_w_lngmo ].Free();
//    pmp->lnSAT = (double *)aObj[ o_wo_lnsat ].Free();
    pmp->lnSAC = (double (*)[4])aObj[ o_wo_lnsat ].Free();
    pmp->B     = (double *)aObj[ o_wi_b ].Free();
    pmp->U     = (double *)aObj[ o_wo_u ].Free();
    pmp->Uc   = (double (*)[2])aObj[ o_wd_uc ].Free();
    pmp->Uefd   = (double *)aObj[ o_wd_uefd ].Free();
    pmp->U_r   = (double *)aObj[ o_wd_ur ].Free();
    pmp->C     = (double *)aObj[ o_wo_c ].Free();
    pmp->IC_m  = (double *)aObj[ o_wd_icm ].Free();
    pmp->IC_lm = (double *)aObj[ o_wd_iclcm ].Free();
    pmp->IC_wm = (double *)aObj[ o_wd_icwm ].Free();
    pmp->BF    = (double *)aObj[ o_wo_bf ].Free();
    pmp->BFC    = (double *)aObj[ o_wo_bfc ].Free(); // aObj[ o_wo_bfc3 ].SetPtr( NULL );
    aObj[ o_wo_bfc3 ].SetPtr( 0 );  // Added 22.10.2013
    aObj[ o_wo_bfc3 ].SetDim( 1, 0 );// for displaying in EqDemo and other screenforms
    pmp->XF    = (double *)aObj[ o_w_xf ].Free();
    pmp->YF    = (double *)aObj[ o_wo_yf ].Free();
    pmp->XFA   = (double *)aObj[ o_ww_xfa ].Free();
    pmp->YFA   = (double *)aObj[ o_ww_yfa ].Free();
    pmp->Falp  = (double *)aObj[ o_wo_falp ].Free();
pmp->GamFs  = (double *)aObj[ o_wo_gamfs ].Free();
    pmp->XetaA = (double (*)[MST])aObj[ o_w_xetaa ].Free();
    pmp->XetaB = (double (*)[MST])aObj[ o_w_xetab ].Free();
    pmp->XetaD = (double (*)[MST])aObj[ o_w_xetad ].Free();
    pmp->X     = (double *)aObj[ o_w_x ].Free();
    pmp->Y     = (double *)aObj[ o_wo_y ].Free();
    pmp->XY    = (double *)aObj[ o_w_xy ].Free();
    pmp->XU    = (double *)aObj[ o_w_xu ].Free();
    pmp->Qp    = (double *)aObj[ o_wi_qp ].Free();
    pmp->Qd    = (double *)aObj[ o_wi_qd ].Free();
    pmp->MU    = (double *)aObj[ o_wo_mu ].Free();
    pmp->EMU   = (double *)aObj[ o_w_emu ].Free();
    pmp->NMU   = (double *)aObj[ o_w_nmu ].Free();
    pmp->W     = (double *)aObj[ o_w_w ].Free();
    pmp->Fx    = (double *)aObj[ o_wo_fxa ].Free();
    pmp->Wx    = (double *)aObj[ o_wo_wx ].Free();
    pmp->F     = (double *)aObj[ o_wo_f ].Free();
    pmp->F0    = (double *)aObj[ o_w_f0 ].Free();
    pmp->D     = (double (*)[MST])aObj[ o_w_d ].Free();
//    pmp->R     = (double *)aObj[ o_w_r ].Free();
//    pmp->R1    = (double *)aObj[ o_w_r1 ].Free();
    pmp->sMod  = (char (*)[8])aObj[ o_wi_smod ].Free();
acp->dcMod  = (char (*)[6])aObj[ o_wi_dcmod ].Free();
    pmp->SB    = (char (*)[MAXICNAME+MAXSYMB])aObj[ o_wd_sb ].Free();
    pmp->SB1   = (char (*)[MAXICNAME])aObj[ o_w_sbh ].Free();
    pmp->SM    = (char (*)[MAXDCNAME])aObj[ o_wd_sm ].Free();
    pmp->SF    = (char (*)[MAXPHNAME+MAXSYMB])aObj[ o_wd_sf ].Free();
    pmp->SM2    = (char (*)[MAXDCNAME])aObj[ o_wd_sm2 ].Free();
    pmp->SM3    = (char (*)[MAXDCNAME])aObj[ o_wd_sm3 ].Free();
    pmp->SF2    = (char (*)[MAXPHNAME+MAXSYMB])aObj[ o_wd_sf2 ].Free();
    pmp->SFs   = (char (*)[MAXPHNAME+MAXSYMB])aObj[ o_wd_sfs ].Free();
    pmp->pbuf  = (char *)aObj[ o_wd_txtb ].Free(); //
    pmp->RLC   = (char *)aObj[ o_wi_rlc ].Free();
    pmp->RSC   = (char *)aObj[ o_wi_rsc ].Free();
    pmp->RFLC  = (char *)aObj[ o_wi_rflc ].Free();
    pmp->RFSC  = (char *)aObj[ o_wi_rfsc ].Free();
    pmp->ICC   = (char *)aObj[ o_wi_icc ].Free();
    pmp->DCC   = (char *)aObj[ o_wi_dcc ].Free();
    pmp->DCC3   = (char *)aObj[ o_wi_dcc3 ].Free();
    pmp->PHC   = (char *)aObj[ o_wi_phc ].Free();
    pmp->SCM   = (char (*)[MST])aObj[ o_wi_scm ].Free();
    pmp->SATT  = (char *)aObj[ o_wi_satt ].Free();
    pmp->DCCW  = (char *)aObj[ o_wi_dccw ].Free();
    // 11/02/2009
    acp->VPh = (double (*)[MIXPHPROPS])aObj[ o_wo_vph].Free();
    acp->GPh = (double (*)[MIXPHPROPS])aObj[ o_wo_gph].Free();
    acp->HPh = (double (*)[MIXPHPROPS])aObj[ o_wo_hph].Free();
    acp->SPh = (double (*)[MIXPHPROPS])aObj[ o_wo_sph].Free();
    acp->CPh = (double (*)[MIXPHPROPS])aObj[ o_wo_cph].Free();
    acp->APh = (double (*)[MIXPHPROPS])aObj[ o_wo_aph].Free();
    acp->UPh = (double (*)[MIXPHPROPS])aObj[ o_wo_uph].Free();
    // New phase stuff 06/06/12
    acp->LsMdc2_  = (long int *)aObj[ o_wi_lsmdc2].Free();
    pmp->LsPhl   = (long int *)aObj[ o_wi_lsphl].Free();
    pmp->PhLin   = (long int (*)[2])aObj[ o_wi_phlin].Free();
    // TSolMod stuff
    pmp->lPhc   = (double *)aObj[ o_wi_lphc].Free();
    acp->DQFc_   = (double *)aObj[ o_wi_dqfc].Free();
//    pmp->rcpc   = (double *)aObj[ o_wi_rcpc].Free();
    //pmp->lnDQFt   = (double *)aObj[ o_wi_lndqft].Free();
    //pmp->lnRcpt   = (double *)aObj[ o_wi_lnrcpt].Free();
    //pmp->lnExet   = (double *)aObj[ o_wi_lnexet].Free();
    //pmp->lnCnft   = (double *)aObj[ o_wi_lncnft].Free();
    //TSorpMod & TKinMet stuff
    acp->SorMc   = (double *)aObj[ o_wi_sormc].Free();
    // TSorpMod stuff
    acp->LsESmo   = (long int *)aObj[ o_wi_lsesmo].Free();
    acp->LsISmo   = (long int *)aObj[ o_wi_lsismo].Free();
    acp->xSMd   = (long int *)aObj[ o_wi_xsmd].Free();
    acp->EImc   = (double *)aObj[ o_wi_eimc].Free();
    acp->mCDc   = (double *)aObj[ o_wi_mcdc].Free();
    acp->IsoPc   = (double *)aObj[ o_wi_isopc].Free();
    acp->IsoSc   = (double *)aObj[ o_wi_isosc].Free();
    acp->lnScalT   = (double *)aObj[ o_wi_lnscal].Free();
    acp->lnSACT   = (double *)aObj[ o_wi_lnsact].Free();
    acp->lnGammF   = (double *)aObj[ o_wi_lngamf].Free();
    acp->CTerms   = (double *)aObj[ o_wi_cterms].Free();
    acp->IsoCt   = (char *)aObj[ o_wi_isoct].Free();
    // TKinMet stuff
    pmp->LsKin   = (long int *)aObj[ o_wi_lskin].Free();
    pmp->LsUpt   = (long int *)aObj[ o_wi_lsupt].Free();
    pmp->xSKrC   = (long int *)aObj[ o_wi_jcrdc].Free();
    pmp->ocPRkC   = (long int (*)[2])aObj[ o_wi_ocprkc].Free();
    pmp->feSArC   = (double *)aObj[ o_wi_fsac].Free();
    pmp->rpConC   = (double *)aObj[ o_wi_krpc].Free();
    pmp->apConC   = (double *)aObj[ o_wi_apconc].Free();
    pmp->AscpC   = (double *)aObj[ o_wi_ascpc].Free();
    pmp->UMpcC   = (double *)aObj[ o_wi_umpc].Free();
    pmp->kMod   = (char(*)[6])aObj[ o_wi_kmod].Free();
    pmp->xICuC   = (long int *)aObj[ o_wi_xicuc].Free();

    pBaseSyst->base_free();
    pActivity->activity_free();
    Free_TKinMet();
}

// reallocation of dynamic memory
void TMultiSystem::dyn_new(int /*q*/)
{
   if( pmp->N < 2 || pmp->L < 2 || pmp->FI < 1 )
        Error( GetName(), "pmp->N < 2 || pmp->L < 2 || pmp->FI < 1" );
    // Part 1
    // always needed
    pmp->L1 = (long int *)aObj[ o_wi_l1].Alloc( pmp->FI, 1, L_ );
    pmp->muk = (long int *)aObj[ o_wi_muk].Alloc( pmp->FI, 1, L_ );
    pmp->mui = (long int *)aObj[ o_wi_mui].Alloc( pmp->N, 1, L_);
    pmp->muj = (long int *)aObj[ o_wi_muj].Alloc( pmp->L, 1, L_ );
    pmp->DUL = (double *)aObj[ o_wi_dul].Alloc( pmp->L, 1, D_ );
    pmp->DLL = (double *)aObj[ o_wi_dll].Alloc( pmp->L, 1, D_ );
    pmp->Vol = (double *)aObj[ o_wio_vol].Alloc( pmp->L, 1, D_ );
    acp->Pparc_ = (double *)aObj[ o_wi_pparc].Alloc( pmp->L, 1, D_ );
    pmp->MM = (double *)aObj[ o_wi_mm].Alloc( pmp->L, 1, D_ );
    pmp->Awt = (double *)aObj[ o_wi_awt].Alloc( pmp->N, 1, D_ );
    pmp->A = (double *)aObj[ o_wi_a].Alloc( pmp->L, pmp->N, D_ );
    pmp->XFs = (double *)aObj[ o_wd_xfs].Alloc( pmp->FI, 1, D_ );
    pmp->Falps = (double *)aObj[ o_wd_falps].Alloc( pmp->FI, 1, D_ );
    pmp->G = (double *)aObj[ o_wo_g].Alloc( pmp->L, 1, D_);
    pmp->G0 = (double *)aObj[ o_wi_g0].Alloc( pmp->L, 1, D_);
    pmp->lnGam = (double *)aObj[ o_wo_lngam].Alloc( pmp->L, 1, D_);
    pmp->lnGmo = (double *)aObj[ o_w_lngmo].Alloc( pmp->L, 1, D_);
    pmp->B = (double *)aObj[ o_wi_b].Alloc( pmp->N, 1, D_ );
    pmp->U = (double *)aObj[ o_wo_u].Alloc( pmp->N, 1, D_ );
    pmp->Uc = (double (*)[2])aObj[ o_wd_uc].Alloc( pmp->N, 2, D_ );
    pmp->Uefd = (double *)aObj[ o_wd_uefd].Alloc( pmp->N, 1, D_ );
    pmp->U_r = (double *)aObj[ o_wd_ur].Alloc( pmp->N, 1, D_ );
    pmp->C = (double *)aObj[ o_wo_c].Alloc( pmp->N, 1, D_ );
    pmp->XF = (double *)aObj[ o_w_xf].Alloc( pmp->FI, 1, D_ );
    pmp->YF = (double *)aObj[ o_wo_yf].Alloc( pmp->FI, 1, D_ );
    pmp->Falp = (double *)aObj[ o_wo_falp].Alloc( pmp->FI, 1, D_ );
pmp->GamFs = (double *)aObj[ o_wo_gamfs].Alloc( pmp->L, 1, D_);
    pmp->X = (double *)aObj[ o_w_x].Alloc( pmp->L, 1, D_);
    pmp->Y = (double *)aObj[ o_wo_y].Alloc( pmp->L, 1, D_);
    pmp->XY = (double *)aObj[ o_w_xy].Alloc( pmp->L, 1, D_);
    pmp->XU = (double *)aObj[ o_w_xu].Alloc( pmp->L, 1, D_);
    pmp->MU = (double *)aObj[ o_wo_mu].Alloc( pmp->L, 1, D_);
    pmp->EMU = (double *)aObj[ o_w_emu].Alloc( pmp->L, 1, D_);
    pmp->NMU = (double *)aObj[ o_w_nmu].Alloc( pmp->L, 1, D_);
    pmp->W = (double *)aObj[ o_w_w].Alloc( pmp->L, 1, D_);
    pmp->F = (double *)aObj[ o_wo_f].Alloc( pmp->L, 1, D_);
    pmp->F0 = (double *)aObj[ o_w_f0].Alloc( pmp->L, 1, D_);
    pmp->YOF = (double *)aObj[ o_wi_yof].Alloc( pmp->FI, 1, D_ );
    pmp->SB = (char (*)[MAXICNAME+MAXSYMB])aObj[ o_wd_sb].Alloc(
                pmp->N, 1, MAXICNAME+MAXSYMB );
    pmp->SB1 = (char (*)[MAXICNAME])aObj[ o_w_sbh].Alloc( 1, pmp->N, MAXICNAME );

    // Error in freeing memory
    //aObj[ o_w_sbh].SetPtr( pmp->SB ); aObj[ o_w_sbh ].SetDim( pmp->N, 1 );
    pmp->SFs = (char (*)[MAXPHNAME+MAXSYMB])aObj[ o_wd_sfs].Alloc(
                 pmp->FI, 1, MAXPHNAME+MAXSYMB );
    pmp->SM = (char (*)[MAXDCNAME])aObj[ o_wd_sm].Alloc( pmp->L, 1, MAXDCNAME );
    pmp->SF = (char (*)[MAXPHNAME+MAXSYMB])aObj[ o_wd_sf].Alloc(
                pmp->FI, 1, MAXPHNAME+MAXSYMB );
    pmp->SM2 = (char (*)[MAXDCNAME])aObj[ o_wd_sm2].Alloc( pmp->Ls, 1, MAXDCNAME );
    pmp->SF2 = (char (*)[MAXPHNAME+MAXSYMB])aObj[ o_wd_sf2].Alloc(
                 pmp->FIs, 1, MAXPHNAME+MAXSYMB );

    pmp->RLC = (char *)aObj[ o_wi_rlc].Alloc( pmp->L, 1, A_ );
    pmp->RSC = (char *)aObj[ o_wi_rsc].Alloc( pmp->L, 1, A_ );
    pmp->ICC = (char *)aObj[ o_wi_icc].Alloc( pmp->N, 1, A_ );
    pmp->DCC = (char *)aObj[ o_wi_dcc].Alloc( pmp->L, 1, A_ );
    pmp->PHC = (char *)aObj[ o_wi_phc].Alloc( pmp->FI, 1, A_ );
    pmp->DCCW = (char *)aObj[ o_wi_dccw].Alloc( pmp->L, 1, A_ );

    pmp->lnGmM = (double *)aObj[ o_wio_lngmm].Alloc( pmp->L, 1, D_ );
    pmp->fDQF = (double *)aObj[ o_wi_gex].Alloc( pmp->L, 1, D_ );
    pmp->FVOL = (double *)aObj[ o_wd_fvol].Alloc( pmp->FI, 1, D_ );
    pmp->FWGT = (double *)aObj[ o_wd_fwgt].Alloc( pmp->FI, 1, D_ );
    pmp->BFC = (double *)aObj[ o_wo_bfc ].Alloc( pmp->N, 1, D_ ); // corrected 07.07.2013 for scripts
    aObj[ o_wo_bfc3 ].SetPtr( pmp->BFC );  // Added 22.10.2013
    aObj[ o_wo_bfc3 ].SetDim( 1, pmp->N );// for displaying in EqDemo and other screenforms
acp->dcMod  = (char (*)[6])aObj[ o_wi_dcmod ].Alloc( pmp->L, 1, 6 );

    if( pmp->L > 0 )
    {
        pmp->Y_la = (double *)aObj[ o_wd_yla].Alloc( pmp->L, 1, D_ );
        pmp->Y_w = (double *)aObj[ o_wd_yw].Alloc( pmp->L, 1, D_ );
        pmp->Fx = (double *)aObj[ o_wo_fxa].Alloc( pmp->L, 1, D_);
        pmp->Wx = (double *)aObj[ o_wo_wx].Alloc( pmp->L, 1, D_);
        pmp->VL = (double *)aObj[ o_w_vl].Alloc( pmp->L, 1, D_ );
        pmp->Gamma = (double *)aObj[ o_wd_gamma].Alloc( pmp->L, 1, D_ );  /*!*/
        pmp->lnGmf = (double *)aObj[ o_wi_lngmf].Alloc( pmp->L, 1, D_ );  /*!*/
//        pmp->D = (double *)aObj[ o_w_d].Alloc( pmp->L, 1, D_);
    }
    else
    {
        pmp->Y_la  = (double *)aObj[ o_wd_yla ].Free();
        pmp->Y_w   = (double *)aObj[ o_wd_yw ].Free();
        pmp->Fx    = (double *)aObj[ o_wo_fxa ].Free();
        pmp->Wx    = (double *)aObj[ o_wo_wx ].Free();
        pmp->VL    = (double *)aObj[ o_w_vl ].Free();
        pmp->Gamma = (double *)aObj[ o_wd_gamma ].Free();
        pmp->lnGmf = (double *)aObj[ o_wi_lngmf ].Free();
//        pmp->D     = (double *)aObj[ o_w_d ].Free();
    }

   // Part 2 - not always required arrays
    if( pmp->FIs > 0 && pmp->Ls > 0 )
    {
       pmp->BF = (double *)aObj[ o_wo_bf].Alloc( pmp->FIs, pmp->N, D_ );
        pmp->XFA = (double *)aObj[ o_ww_xfa].Alloc( pmp->FIs, 1, D_ );
        pmp->YFA = (double *)aObj[ o_ww_yfa].Alloc( pmp->FIs, 1, D_ );
        acp->LsMod_ = (long int *)aObj[ o_wi_lsmod].Alloc( pmp->FIs, 3, L_ );
        acp->LsMdc_ = (long int *)aObj[ o_wi_lsmdc].Alloc( pmp->FIs, 3, L_ );
    // IPx, PMc and DMc may be reallocated after loading arrays LsMod and LsMdc
        acp->IPx_ = (long int *)aObj[ o_wi_ipxpm ].Alloc(pmp->FIs, 1, L_);  // added 07.12.2006 KD
        acp->PMc_ = (double *)aObj[ o_wi_pmc].Alloc( pmp->FIs, 1, D_);
        acp->DMc_ = (double *)aObj[ o_wi_dmc].Alloc( pmp->Ls, 1, D_ );
        acp->MoiSN_  = (double *)aObj[ o_wi_moisn ].Alloc( pmp->Ls, 1, D_ );
        acp->SitFr_  = (double *)aObj[ o_wo_sitfr ].Alloc( pmp->Ls, 1, D_ );
        pmp->PUL = (double *)aObj[ o_wi_pul].Alloc( pmp->FIs, 1, D_);
        pmp->PLL = (double *)aObj[ o_wi_pll].Alloc( pmp->FIs, 1, D_);
        //     pmp->YOF = (double *)aObj[ o_wi_yof].Alloc( pmp->FI, 1, D_ );
        pmp->sMod = (char (*)[8])aObj[ o_wi_smod].Alloc( pmp->FIs, 1, 8 );
        pmp->RFLC = (char *)aObj[ o_wi_rflc].Alloc( pmp->FIs, 1, A_ );
        pmp->RFSC = (char *)aObj[ o_wi_rfsc].Alloc( pmp->FIs, 1, A_ );
    }
    else
    {
        pmp->BF    = (double *)aObj[ o_wo_bf ].Free();
        pmp->XFA   = (double *)aObj[ o_ww_xfa ].Free();
        pmp->YFA   = (double *)aObj[ o_ww_yfa ].Free();
        acp->LsMod_ = (long int *)aObj[ o_wi_lsmod ].Free();
        acp->LsMdc_ = (long int *)aObj[ o_wi_lsmdc ].Free();
        acp->IPx_ = (long int *)aObj[ o_wi_ipxpm ].Free();   // added 07.12.2006  KD
        pmp->PUL   = (double *)aObj[ o_wi_pul ].Free();
        pmp->PLL   = (double *)aObj[ o_wi_pll ].Free();
        //    pmp->YOF   = (double *)aObj[ o_wi_yof ].Free();
        acp->PMc_   = (double *)aObj[ o_wi_pmc ].Free();
        acp->DMc_   = (double *)aObj[ o_wi_dmc ].Free();
        acp->MoiSN_  = (double *)aObj[ o_wi_moisn ].Free();
        acp->SitFr_  = (double *)aObj[ o_wo_sitfr ].Free();
        pmp->sMod  = (char (*)[8])aObj[ o_wi_smod ].Free();
        pmp->RFLC  = (char *)aObj[ o_wi_rflc ].Free();
        pmp->RFSC  = (char *)aObj[ o_wi_rfsc ].Free();
    }
    if( pmp->LO > 1 )
    {
        pmp->Y_m = (double *)aObj[ o_wd_ym].Alloc( pmp->L, 1, D_ );
        pmp->IC_m = (double *)aObj[ o_wd_icm].Alloc( pmp->N, 1, D_ );
        pmp->IC_lm = (double *)aObj[ o_wd_iclcm].Alloc( pmp->N, 1, D_ );
        pmp->IC_wm = (double *)aObj[ o_wd_icwm].Alloc( pmp->N, 1, D_ );
    }
    else
    {
        pmp->IC_m  = (double *)aObj[ o_wd_icm ].Free();
        pmp->IC_lm = (double *)aObj[ o_wd_iclcm ].Free();
        pmp->IC_wm = (double *)aObj[ o_wd_icwm ].Free();
        pmp->Y_m   = (double *)aObj[ o_wd_ym ].Free();
    }

    /* sorption phases */
    if( TSyst::sm->GetSY()->PAalp != S_OFF )
    {
        pmp->Aalp = (double *)aObj[ o_wi_aalp].Alloc( pmp->FI, 1, D_ );
        pmp->Xr0h0 = (double (*)[2])aObj[ o_wi_xr0h0].Alloc( pmp->FI, 2, D_ );
    }
    else
    {
        pmp->Aalp  = (double *)aObj[ o_wi_aalp ].Free();
        pmp->Xr0h0 = (double (*)[2])aObj[ o_wi_xr0h0 ].Free();
    }

   if( TSyst::sm->GetSY()->PSigm != S_OFF )
           pmp->Sigw = (double *)aObj[ o_wi_sigw].Alloc( pmp->FI, 1, D_ );
    else  pmp->Sigw  = (double *)aObj[ o_wi_sigw ].Free();

    if( TSyst::sm->GetSY()->PSigm != S_OFF )
        pmp->Sigg = (double *)aObj[ o_wi_sigg].Alloc( pmp->FI, 1, D_ );
    else  pmp->Sigg  = (double *)aObj[ o_wi_sigg ].Free();

    if( pmp->E )
    {
        pmp->EZ = (double *)aObj[ o_wi_ez].Alloc( pmp->L, 1, D_ );
        pmp->Xcond = (double *)aObj[ o_wi_xcond].Alloc( pmp->FI, 1, D_ );
        pmp->Xeps = (double *)aObj[ o_wi_xeps].Alloc( pmp->FI, 1, D_ );
    }
    else
    {
        pmp->EZ    = (double *)aObj[ o_wi_ez ].Free();
        pmp->Xcond = (double *)aObj[ o_wi_xcond ].Free();
        pmp->Xeps  = (double *)aObj[ o_wi_xeps ].Free();
    }

    if( pmp->FIat > 0 && pmp->FIs > 0 )
    { // ADSORPTION AND ION EXCHANGE
        pmp->SATX = (long int (*)[4])aObj[ o_wi_satndx].Alloc( pmp->Lads, 4, L_ );
        pmp->SCM  = (char (*)[MST])aObj[ o_wi_scm].Alloc( pmp->FIs, pmp->FIat, A_ );
        //   fillValue( pmp->SCM, SC_NOT_USED, pmp->FIs*pmp->FIat );
        pmp->Nfsp = (double (*)[MST])aObj[ o_wi_nfsp].Alloc( pmp->FIs, pmp->FIat, D_ );
        pmp->MASDT = (double (*)[MST])aObj[ o_wi_masdt].Alloc( pmp->FIs, pmp->FIat, D_ );
        pmp->XcapA = (double (*)[MST])aObj[ o_w_xcapa].Alloc( pmp->FIs, pmp->FIat, D_ );
        pmp->XcapB = (double (*)[MST])aObj[ o_w_xcapb].Alloc( pmp->FIs, pmp->FIat, D_ );
        pmp->XcapD = (double (*)[MST])aObj[ o_w_xcapd].Alloc( pmp->FIs, pmp->FIat, D_ );
        pmp->XcapF = (double (*)[MST])aObj[ o_w_xcapf].Alloc( pmp->FIs, pmp->FIat, D_ );
        pmp->XdlA = (double (*)[MST])aObj[ o_wo_xdla].Alloc( pmp->FIs, pmp->FIat, D_ );
        pmp->XdlB = (double (*)[MST])aObj[ o_wo_xdlb].Alloc( pmp->FIs, pmp->FIat, D_ );
        pmp->XdlD = (double (*)[MST])aObj[ o_wo_xdld].Alloc( pmp->FIs, pmp->FIat, D_ );
        pmp->XpsiA = (double (*)[MST])aObj[ o_wo_psia].Alloc( pmp->FIs, pmp->FIat, D_ );
        pmp->XpsiB = (double (*)[MST])aObj[ o_wo_psib].Alloc( pmp->FIs, pmp->FIat, D_ );
        pmp->XpsiD = (double (*)[MST])aObj[ o_wo_psid].Alloc( pmp->FIs, pmp->FIat, D_ );
        pmp->XlamA = (double (*)[MST])aObj[ o_wi_xlama].Alloc( pmp->FIs, pmp->FIat, D_ );
        pmp->Xetaf = (double (*)[MST])aObj[ o_wi_xetaf].Alloc( pmp->FIs, pmp->FIat, D_ );
        pmp->XetaA = (double (*)[MST])aObj[ o_w_xetaa].Alloc( pmp->FIs, pmp->FIat, D_ );
        pmp->XetaB = (double (*)[MST])aObj[ o_w_xetab].Alloc( pmp->FIs, pmp->FIat, D_ );
        pmp->XetaD = (double (*)[MST])aObj[ o_w_xetad].Alloc( pmp->FIs, pmp->FIat, D_ );
        pmp->XFTS = (double (*)[MST])aObj[ o_w_xfts].Alloc( pmp->FIs, pmp->FIat, D_ );
        pmp->SATT = (char *)aObj[ o_wi_satt].Alloc( pmp->Lads, 1, A_ );
        pmp->MASDJ = (double (*)[DFCN])aObj[ o_wo_masdj].Alloc( pmp->Lads, DFCN, D_ );
//        pmp->MASDJ = (double *)aObj[ o_wo_masdj].Alloc( pmp->Ls, 1, D_ );
        pmp->D = (double (*)[MST])aObj[ o_w_d].Alloc( MST, MST, D_);
        pmp->lnSAC = (double (*)[4])aObj[ o_wo_lnsat].Alloc( pmp->Lads, 4, D_ );
        pmp->SM3 = (char (*)[MAXDCNAME])aObj[ o_wd_sm3].Alloc( pmp->Lads, 1, MAXDCNAME );
        pmp->DCC3 = (char *)aObj[ o_wi_dcc3].Alloc( pmp->Lads, 1, A_ );
    }
    else
    {
        pmp->SATX =(long int (*)[4])aObj[ o_wi_satndx ].Free();
        pmp->SCM   = (char (*)[MST])aObj[ o_wi_scm ].Free();
        pmp->Nfsp  = (double (*)[MST])aObj[ o_wi_nfsp ].Free();
        pmp->MASDT = (double (*)[MST])aObj[ o_wi_masdt ].Free();
        pmp->XcapA = (double (*)[MST])aObj[ o_w_xcapa ].Free();
        pmp->XcapB = (double (*)[MST])aObj[ o_w_xcapb ].Free();
        pmp->XcapD = (double (*)[MST])aObj[ o_w_xcapd ].Free();
        pmp->XcapF = (double (*)[MST])aObj[ o_w_xcapf ].Free();
        pmp->XdlA  = (double (*)[MST])aObj[ o_wo_xdla ].Free();
        pmp->XdlB  = (double (*)[MST])aObj[ o_wo_xdlb ].Free();
        pmp->XdlD  = (double (*)[MST])aObj[ o_wo_xdld ].Free();
        pmp->XpsiA = (double (*)[MST])aObj[ o_wo_psia ].Free();
        pmp->XpsiB = (double (*)[MST])aObj[ o_wo_psib ].Free();
        pmp->XpsiD = (double (*)[MST])aObj[ o_wo_psid ].Free();
        pmp->XlamA = (double (*)[MST])aObj[ o_wi_xlama ].Free();
        pmp->Xetaf = (double (*)[MST])aObj[ o_wi_xetaf ].Free();
        pmp->XFTS  = (double (*)[MST])aObj[ o_w_xfts ].Free();
        pmp->XetaA = (double (*)[MST])aObj[ o_w_xetaa ].Free();
        pmp->XetaB = (double (*)[MST])aObj[ o_w_xetab ].Free();
        pmp->XetaD = (double (*)[MST])aObj[ o_w_xetad ].Free();  // added 12.09.05  KD
        pmp->SATT  = (char *)aObj[ o_wi_satt ].Free();
        pmp->MASDJ = (double (*)[DFCN])aObj[ o_wo_masdj ].Free();
        pmp->lnSAC = (double (*)[4])aObj[ o_wo_lnsat ].Free();
        pmp->D = (double (*)[MST])aObj[ o_w_d ].Free();
        pmp->SM3 = (char (*)[MAXDCNAME])aObj[ o_wd_sm3 ].Free();
        pmp->DCC3 = (char *)aObj[ o_wi_dcc3 ].Free();
    }

    if( pmp->PG > 0 )
    {
        pmp->Fug = (double *)aObj[ o_wd_fug].Alloc( pmp->PG, 1, D_ );
        pmp->Fug_l = (double *)aObj[ o_wd_fugl].Alloc( pmp->PG, 1, D_ );
        pmp->Ppg_l = (double *)aObj[ o_wd_ppgl].Alloc( pmp->PG, 1, D_ );
    }
    else
    {
        pmp->Fug   = (double *)aObj[ o_wd_fug ].Free();
        pmp->Fug_l = (double *)aObj[ o_wd_fugl ].Free();
        pmp->Ppg_l = (double *)aObj[ o_wd_ppgl ].Free();
    }

    // Part 3

    if( pmp->Ls > 1 && pmp->FIs > 0 )
    {
        pmp->Wb = (double *)aObj[ o_wio_wb].Alloc( pmp->Ls, 1, D_ );
        pmp->Wabs = (double *)aObj[ o_wio_wabs].Alloc( pmp->Ls, 1, D_ );
        pmp->Rion = (double *)aObj[ o_wi_rion].Alloc( pmp->Ls, 1, D_ );
        pmp->Qp = (double *)aObj[ o_wi_qp].Alloc( pmp->FIs, QPSIZE, D_ );
        pmp->Qd = (double *)aObj[ o_wi_qd].Alloc( pmp->FIs, QDSIZE, D_ );
    }
    else
    {
        pmp->Wb    = (double *)aObj[ o_wio_wb ].Free();
        pmp->Wabs  = (double *)aObj[ o_wio_wabs ].Free();
        pmp->Rion  = (double *)aObj[ o_wi_rion ].Free();
        pmp->Qp    = (double *)aObj[ o_wi_qp ].Free();
        pmp->Qd    = (double *)aObj[ o_wi_qd ].Free();
    }

// SD 14/07/2009
    pmp->Cp0   = (double *)aObj[ o_wio_cp0 ].Alloc( pmp->L, 1, D_ );
    pmp->H0    = (double *)aObj[ o_wio_h0 ].Alloc( pmp->L, 1, D_ );
    pmp->U0    = (double *)aObj[ o_wio_u0 ].Alloc( pmp->L, 1, D_ );
    pmp->S0    = (double *)aObj[ o_wio_s0 ].Alloc( pmp->L, 1, D_ );
    pmp->A0    = (double *)aObj[ o_wio_a0 ].Alloc( pmp->L, 1, D_ );
        
    // 11/02/2009
        acp->VPh = (double (*)[MIXPHPROPS])aObj[ o_wo_vph].Alloc(pmp->FIs,MIXPHPROPS, D_);
        acp->GPh = (double (*)[MIXPHPROPS])aObj[ o_wo_gph].Alloc(pmp->FIs,MIXPHPROPS, D_);
        acp->HPh = (double (*)[MIXPHPROPS])aObj[ o_wo_hph].Alloc(pmp->FIs,MIXPHPROPS, D_);
        acp->SPh = (double (*)[MIXPHPROPS])aObj[ o_wo_sph].Alloc(pmp->FIs,MIXPHPROPS, D_);
        acp->CPh = (double (*)[MIXPHPROPS])aObj[ o_wo_cph].Alloc(pmp->FIs,MIXPHPROPS, D_);
        acp->APh = (double (*)[MIXPHPROPS])aObj[ o_wo_aph].Alloc(pmp->FIs,MIXPHPROPS, D_);
        acp->UPh = (double (*)[MIXPHPROPS])aObj[ o_wo_uph].Alloc(pmp->FIs,MIXPHPROPS, D_);

        // New phase stuff 06/06/12
        acp->LsMdc2_  = (long int *)aObj[ o_wi_lsmdc2].Alloc( pmp->FIs, 3, L_ );
        pmp->LsPhl   = (long int *)aObj[ o_wi_lsphl].Alloc( pmp->FI, 2, L_ );
        //pmp->PhLin   = (long int *)aObj[ o_wi_phlin].Alloc( pmp->FI*2, 1, L_ );
        // TSolMod stuff
        //pmp->lPhc   = (double *)aObj[ o_wi_lphc].Alloc( pmp->FI*2, 1, D_ );
        //pmp->DQFc   = (double *)aObj[ o_wi_dqfc].Alloc( pmp->FIs, 1, D_ );
        //pmp->rcpc   = (double *)aObj[ o_wi_rcpc].Alloc( pmp->FIs, 1, D_ );
        //pmp->lnDQFt   = (double *)aObj[ o_wi_lndqft].Alloc( pmp->Ls, 1, D_ );
        //pmp->lnRcpt   = (double *)aObj[ o_wi_lnrcpt].Alloc( pmp->Ls, 1, D_ );
        //pmp->lnExet   = (double *)aObj[ o_wi_lnexet].Alloc( pmp->Ls, 1, D_ );
        //pmp->lnCnft   = (double *)aObj[ o_wi_lncnft].Alloc( pmp->Ls, 1, D_ );
        //TSorpMod & TKinMet stuff
        acp->SorMc   = (double *)aObj[ o_wi_sormc].Alloc( pmp->FIs, 16, D_ );
        // TSorpMod stuff
        acp->LsESmo   = (long int *)aObj[ o_wi_lsesmo].Alloc( pmp->FIs, 4, L_ );
        acp->LsISmo   = (long int *)aObj[ o_wi_lsismo].Alloc( pmp->FIs, 4, L_ );
        //pmp->xSMd   = (long int *)aObj[ o_wi_xsmd].Alloc( pmp->FIs, 1, D_ );
        //pmp->EImc   = (double *)aObj[ o_wi_eimc].Alloc( pmp->FIs, 1, D_ );
        //pmp->mCDc   = (double *)aObj[ o_wi_mcdc].Alloc( pmp->FIs, 1, D_ );
        //pmp->IsoPc   = (double *)aObj[ o_wi_isopc].Alloc( pmp->Fs, 1, D_ );
        //pmp->IsoSc   = (double *)aObj[ o_wi_isosc].Alloc( pmp->Fs, 1, D_ );
        acp->lnScalT   = (double *)aObj[ o_wi_lnscal].Alloc( pmp->Ls, 1, D_ );
        acp->lnSACT   = (double *)aObj[ o_wi_lnsact].Alloc( pmp->Ls, 1, D_ );
        acp->lnGammF   = (double *)aObj[ o_wi_lngamf].Alloc( pmp->Ls, 1, D_ );
        acp->CTerms   = (double *)aObj[ o_wi_cterms].Alloc( pmp->Ls, 1, D_ );
        //pmp->IsoCt   = (char *)aObj[ o_wi_isoct].Alloc( pmp->FIs, 1, A_ );
        // TKinMet stuff
        pmp->LsKin   = (long int *)aObj[ o_wi_lskin].Alloc( pmp->FI, 6, L_ );
        pmp->LsUpt   = (long int *)aObj[ o_wi_lsupt].Alloc( pmp->FIs, 2, L_ );
        //pmp->xSKrC   = (long int *)aObj[ o_wi_jcrdc].Alloc( pmp->FI, 1, L_ );
        //pmp->ocPRkC   = (long int *)aObj[ o_wi_ocprkc].Alloc( pmp->FI, 1, L_ );
        //pmp->feSArC   = (double *)aObj[ o_wi_fsac].Alloc( pmp->FI, 1, D_ );
        //pmp->rpConC   = (double *)aObj[ o_wi_krpc].Alloc( pmp->FI, 1, D_ );
        //pmp->apConC   = (double *)aObj[ o_wi_apconc].Alloc( pmp->FI, 1, D_ );
        //pmp->AscpC   = (double *)aObj[ o_wi_ascpc].Alloc( pmp->FI, 1, D_ );
        //pmp->UMpcC   = (double *)aObj[ o_wi_umpc].Alloc( pmp->FIs, 1, D_ );
        pmp->kMod   = (char(*)[6])aObj[ o_wi_kmod].Alloc( pmp->FI, 1, 6 );
//  new (kinetics output, uptake output)
    pmp->PfFact = (double *)aObj[ o_wi_pffact].Alloc( pmp->FI, 1, D_ );
    pmp->PrT = (double *)aObj[ o_wo_prto].Alloc( pmp->FI, 1, D_ );
    pmp->PkT = (double *)aObj[ o_wo_pkto].Alloc( pmp->FI, 1, D_ );
    pmp->PvT = (double *)aObj[ o_wo_pvto].Alloc( pmp->FI, 1, D_ );
    pmp->emRd = (double *)aObj[ o_wo_emrd].Alloc( pmp->Ls, 1, D_ );
    pmp->emDf = (double *)aObj[ o_wo_emdf].Alloc( pmp->Ls, 1, D_ );
//

    //pActivity->activity_free();
    pActivity->activity_realloc();

    //Free_TKinMet();
    Alloc_TKinMet( pmp->FI );

}

//--------------------- End of ms_multi.cpp ---------------------------
