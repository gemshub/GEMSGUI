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
// standalone GEMS3K code (define IPMGEMPLUGIN).
//
// This file may be distributed under the terms of GEMS3 Development
// Quality Assurance Licence (GEMS3.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//
#include "m_param.h"

TMulti* TMulti::sm;

// Constructor
//
TMulti::TMulti( int nrt ):
        TSubModule( nrt )
{
    set_def();
    pmp = &pm;
    node = 0;

    sizeN = 0;
    AA=0;
    BB=0;
    arrL=0;
    arrAN=0;
    
U_mean = 0;
U_M2 = 0;
U_CVo = 0;
U_CV = 0;
ICNud = 0;

    sizeFIs = 0;
    phSolMod = 0;
}

const char* TMulti::GetHtml()
{
       return GSM_MULTI_HTML;
}

// link values to objects
void TMulti::ods_link( int )
{
    //aObj[ o_wd_stkey].SetPtr( pm.stkey );
    //aObj[ o_wd_nv_].SetPtr( pm.NV_ );
    aObj[ o_wi_pun ].SetPtr( &pm.PunE ); /*a 4*/
    aObj[ o_wi_nnr ].SetPtr( &pm.N );    /*l 2*/
    aObj[ o_wi_ldim ].SetPtr( &pm.L );   /*l 6*/
    aObj[ o_wi_fdim ].SetPtr( &pm.FI );  /*l 3*/
    aObj[ o_wo_fdim ].SetPtr( &pm.FI1 ); /*l 3*/
    aObj[ o_wo_it ].SetPtr( &pm.IT );
    aObj[ o_wo_it_efd ].SetPtr( &pm.ITF ); /* pm.ITF */
    aObj[ o_wo_it_ipm ].SetPtr( &pm.ITG ); /* pm.ITG */
    aObj[ o_wi_mode ].SetPtr( &pm.E );   /*l 4*/
    aObj[ o_wo_ipar ].SetPtr( &pm.Ec );  /*l 17*/
    aObj[ o_wio_tc ].SetPtr( &pm.TC );   /*d 2*/
    aObj[ o_wio_t ].SetPtr( &pm.T );     /*d 2*/
    aObj[ o_wio_p ].SetPtr( &pm.P );     /*d 2*/
    aObj[ o_wo_vx ].SetPtr( &pm.VX_ );   /*d 2*/
    aObj[ o_wo_gx ].SetPtr( &pm.GX_ );   /*d 2*/
    aObj[ o_wo_ax ].SetPtr( &pm.AX_ );   /*d 2*/
    aObj[ o_wo_ux ].SetPtr( &pm.UX_ );   /*d 2*/
    aObj[ o_wo_hx ].SetPtr( &pm.HX_ );   /*d 2*/
    aObj[ o_wo_sx ].SetPtr( &pm.SX_ );   /*d 2*/
    aObj[ o_wo_cpx ].SetPtr( &pm.CpX_ ); /*d 2*/
    aObj[ o_wo_cvx ].SetPtr( &pm.CvX_ ); /*d 2*/
    aObj[ o_wi_ktau2 ].SetPtr( &pm.kTau );
    aObj[ o_wi_t0 ].SetPtr( &pm.TMols );
    aObj[ o_wi_ve ].SetPtr( &pm.SMols );    /*VE*/
    aObj[ o_wi_mbx ].SetPtr( &pm.MBX );
    aObj[ o_wo_fx ].SetPtr( &pm.FX );
    aObj[ o_wo_ic ].SetPtr( &pm.IC );
    aObj[ o_wd_ph ].SetPtr( &pm.pH );
    aObj[ o_wd_pe ].SetPtr( &pm.pe );
    aObj[ o_wd_eh ].SetPtr( &pm.Eh );
    aObj[ o_wi_mol ].SetPtr( &pm.DHBM );
    aObj[ o_wi_gwat ].SetPtr( &pm.GWAT );
    aObj[ o_wi_ymet ].SetPtr( &pm.YMET );
    aObj[ o_wio_denw ].SetPtr(&pm.denW ); /*d 10*/
    aObj[ o_wio_epsw ].SetPtr(&pm.epsW ); /*d 10*/
    aObj[ o_wo_psi ].SetPtr( &pm.PCI );   /*d 2*/
    aObj[ o_wo_lnp ].SetPtr( &pm.lnP );
    aObj[ o_wo_rt ].SetPtr( &pm.RT );
    aObj[ o_wo_frt ].SetPtr( &pm.FRT );
    aObj[ o_wo_yw ].SetPtr( &pm.Yw );
    aObj[ o_wio_cons ].SetPtr(&pm.ln5551 );/*d 6*/
    aObj[ o_wio_pmxs ].SetPtr(pm.FitVar ); /*d 1*/
    aObj[ o_wio_fitv ].SetPtr(pm.FitVar+1 ); /*d 4*/
    aObj[ o_wd_sitn ].SetPtr(&pm.sitNcat );

    // dynamic DODs, part 1
    aObj[ o_wi_l1 ].SetPtr( pm.L1 );
    aObj[ o_wi_l1 ].SetDim( pm.FI, 1 );
    //  Added 07.12.2006 - new object for solution models  KD
    aObj[ o_wi_ipxpm ].SetPtr( pm.IPx );
    //   Dimension is calculated on MULTI loading
    aObj[ o_wi_lsmod ].SetPtr(pm.LsMod );
   // aObj[ o_wi_lsmod ].SetDim( pm.FIs, 1 );  changed 07.12.2006  KD
    aObj[ o_wi_lsmod ].SetDim( pm.FIs, 3 ); // 3 columns
    aObj[ o_wi_lsmdc ].SetPtr(pm.LsMdc );
    aObj[ o_wi_lsmdc ].SetDim( pm.FIs, 3 );
    aObj[ o_wi_mui ].SetPtr( pm.mui );
    aObj[ o_wi_mui ].SetDim( pm.N, 1 );
    aObj[ o_wi_muk ].SetPtr( pm.muk );
    aObj[ o_wi_muk ].SetDim( pm.FI, 1 );
    aObj[ o_wi_muj ].SetPtr( pm.muj );
    aObj[ o_wi_muj ].SetDim( pm.L, 1 );
    aObj[ o_wi_dul ].SetPtr( pm.DUL );
    aObj[ o_wi_dul ].SetDim( pm.L, 1 );
    aObj[ o_wi_dll ].SetPtr( pm.DLL );
    aObj[ o_wi_dll ].SetDim( pm.L, 1 );
    aObj[ o_wi_gex ].SetPtr( pm.fDQF );
    aObj[ o_wi_gex ].SetDim( pm.L, 1 );
    aObj[ o_wi_pul ].SetPtr( pm.PUL );
    aObj[ o_wi_pul ].SetDim( pm.FIs, 1 );
    aObj[ o_wi_pll ].SetPtr( pm.PLL );
    aObj[ o_wi_pll ].SetDim( pm.FIs, 1 );
    aObj[ o_wi_yof ].SetPtr( pm.YOF );
    aObj[ o_wi_yof ].SetDim( pm.FI, 1 );
    /* SIZE OF PMc- CALC M1=1 */
    aObj[ o_wi_pmc ].SetPtr( pm.PMc );  // OBsetDim( o_wi_pmc,  M1, 1 );
    /* SIZE OF DMc- CALC M2=1 */
    aObj[ o_wi_dmc ].SetPtr( pm.DMc );      // OBsetDim( o_wi_Dmc,  M2, 1 );
    aObj[ o_wi_moisn ].SetPtr( pm.MoiSN );  // OBsetDim( o_wi_moisn,  M3, 1 );
    aObj[ o_wo_sitfr ].SetPtr( pm.SitFr );  // work array of site fractions
    aObj[ o_wio_vol ].SetPtr( pm.Vol );
    aObj[ o_wio_vol ].SetDim( pm.L, 1 );
    aObj[ o_w_hym ].SetPtr( pm.HYM );
    aObj[ o_w_hym ].SetDim( pm.L, 1 );
    aObj[ o_w_vl ].SetPtr( pm.VL );
    aObj[ o_w_vl ].SetDim( pm.L, 1 );
    aObj[ o_wi_mm ].SetPtr( pm.MM );
    aObj[ o_wi_mm ].SetDim( pm.L, 1 );
    aObj[ o_wio_h0 ].SetPtr( pm.H0 );
    aObj[ o_wio_h0 ].SetDim( pm.L, 1 );
    aObj[ o_wio_a0 ].SetPtr( pm.A0 );
    aObj[ o_wio_a0 ].SetDim( pm.L, 1 );
    aObj[ o_wio_u0 ].SetPtr( pm.U0 );
    aObj[ o_wio_u0 ].SetDim( pm.L, 1 );
    aObj[ o_wio_s0 ].SetPtr( pm.S0 );
    aObj[ o_wio_s0 ].SetDim( pm.L, 1 );
    aObj[ o_wio_cp0 ].SetPtr( pm.Cp0 );
    aObj[ o_wio_cp0 ].SetDim( pm.L, 1 );
    aObj[ o_wio_cv0 ].SetPtr( pm.Cv0 );
    aObj[ o_wio_cv0 ].SetDim( pm.L, 1 );
    aObj[ o_wi_pparc ].SetPtr(pm.Pparc );
    aObj[ o_wi_pparc ].SetDim( pm.L, 1 );

    // part 2
    aObj[o_wi_satndx].SetPtr( pm.SATX );  // changed 27.10.2004 for CD MUSIC
    if( pm.FIat > 0 ) aObj[ o_wi_satndx ].SetDim( pm.Lads, 4 );
//
    aObj[ o_wd_ym].SetPtr( pm.Y_m );
    aObj[ o_wd_ym ].SetDim( pm.L, 1 );
    aObj[ o_wd_yla].SetPtr( pm.Y_la );
    aObj[ o_wd_yla ].SetDim( pm.L, 1 );
    aObj[ o_wd_yw].SetPtr( pm.Y_w );
    aObj[ o_wd_yw ].SetDim( pm.L, 1 );
    aObj[ o_wd_gamma].SetPtr( pm.Gamma );
    aObj[ o_wd_gamma ].SetDim( pm.L, 1);
    aObj[ o_wi_lngmf].SetPtr( pm.lnGmf );
    aObj[ o_wi_lngmf ].SetDim( pm.L, 1);
    aObj[ o_wio_lngmm].SetPtr(pm.lnGmM );
    aObj[ o_wio_lngmm ].SetDim(pm.L, 1);
    aObj[ o_wi_ez].SetPtr( pm.EZ );
    aObj[ o_wi_ez ].SetDim( pm.L, 1 );
    aObj[ o_wi_aalp].SetPtr( pm.Aalp );
    aObj[ o_wi_aalp ].SetDim( pm.FI, 1 );
    aObj[ o_wi_sigw].SetPtr( pm.Sigw );
    aObj[ o_wi_sigw ].SetDim( pm.FI, 1 );
    aObj[ o_wi_sigg].SetPtr( pm.Sigg );
    aObj[ o_wi_sigg ].SetDim( pm.FI, 1 );
    aObj[ o_wi_nfsp].SetPtr( pm.Nfsp );
    aObj[o_wi_nfsp].SetDim(pm.FIs,pm.FIat);
    aObj[ o_wi_masdt].SetPtr( pm.MASDT );
    aObj[o_wi_masdt].SetDim(pm.FIs,pm.FIat);
    //    aObj[ o_wi_misdt].SetPtr( pm.MISDT );  aObj[o_wi_misdt].SetDim(pm.FIs,pm.FIat);
    aObj[ o_wd_fvol].SetPtr( pm.FVOL );
    aObj[ o_wd_fvol ].SetDim( pm.FI, 1 );
    aObj[ o_wd_fwgt].SetPtr( pm.FWGT );
    aObj[ o_wd_fwgt ].SetDim( pm.FI, 1 );
    aObj[ o_w_xcapa].SetPtr( pm.XcapA );
    aObj[o_w_xcapa].SetDim(pm.FIs,pm.FIat);
    aObj[ o_w_xcapb].SetPtr( pm.XcapB );
    aObj[o_w_xcapb].SetDim(pm.FIs,pm.FIat);
    aObj[ o_w_xcapd].SetPtr( pm.XcapD );
    aObj[o_w_xcapd].SetDim(pm.FIs,pm.FIat);
    aObj[ o_w_xcapf].SetPtr( pm.XcapF );
    aObj[o_w_xcapf].SetDim(pm.FIs,pm.FIat);
    aObj[ o_wo_xdla].SetPtr( pm.XdlA );
    aObj[ o_wo_xdla].SetDim(pm.FIs,pm.FIat);
    aObj[ o_wo_xdlb].SetPtr( pm.XdlB );
    aObj[ o_wo_xdlb].SetDim(pm.FIs,pm.FIat);
    aObj[ o_wo_xdld].SetPtr( pm.XdlD );
    aObj[ o_wo_xdld].SetDim(pm.FIs,pm.FIat);
    aObj[ o_wo_psia].SetPtr( pm.XpsiA );
    aObj[o_wo_psia].SetDim(pm.FIs,pm.FIat);
    aObj[ o_wo_psib].SetPtr( pm.XpsiB );
    aObj[o_wo_psib].SetDim(pm.FIs,pm.FIat);
    aObj[ o_wo_psid].SetPtr( pm.XpsiD );
    aObj[o_wo_psid].SetDim(pm.FIs,pm.FIat);
    aObj[ o_wi_xr0h0].SetPtr( pm.Xr0h0 );
    aObj[o_wi_xr0h0 ].SetDim(pm.FI, 2);
    aObj[ o_wi_xlama].SetPtr( pm.XlamA );
    aObj[o_wi_xlama].SetDim(pm.FIs,pm.FIat);
    aObj[ o_wi_xetaf].SetPtr( pm.Xetaf );
    aObj[o_wi_xetaf].SetDim(pm.FIs,pm.FIat);
    aObj[ o_wi_xcond].SetPtr( pm.Xcond );
    aObj[ o_wi_xcond ].SetDim(pm.FI, 1 );
    aObj[ o_wi_xeps].SetPtr( pm.Xeps );
    aObj[ o_wi_xeps ].SetDim( pm.FI, 1 );
    aObj[ o_wi_awt].SetPtr( pm.Awt );
    aObj[ o_wi_awt ].SetDim( pm.N, 1 );
    aObj[ o_wi_a].SetPtr( pm.A );
    aObj[ o_wi_a ].SetDim( pm.L, pm.N );
    aObj[ o_wd_xfs].SetPtr( pm.XFs );
    aObj[ o_wd_xfs ].SetDim( pm.FI, 1 );
    aObj[ o_wd_falps].SetPtr( pm.Falps );
    aObj[ o_wd_falps ].SetDim( pm.FI, 1 );
    aObj[ o_wo_g].SetPtr( pm.G );
    aObj[ o_wo_g ].SetDim( pm.L, 1 );
    aObj[ o_wi_g0].SetPtr( pm.G0 );
    aObj[ o_wi_g0 ].SetDim( pm.L, 1 );
    aObj[ o_wo_lngam].SetPtr( pm.lnGam );
    aObj[ o_wo_lngam ].SetDim( pm.L, 1);
    aObj[ o_w_lngmo].SetPtr( pm.lnGmo );
    aObj[ o_w_lngmo ].SetDim( pm.L, 1 );
    aObj[ o_wi_b].SetPtr( pm.B );
    aObj[ o_wi_b ].SetDim( pm.N, 1 );
    aObj[ o_wo_u].SetPtr( pm.U );
    aObj[ o_wo_u ].SetDim( pm.N, 1 );
    aObj[ o_wd_uc].SetPtr( pm.Uc );
    aObj[ o_wd_uc ].SetDim( pm.N, 2 );
    aObj[ o_wd_uefd].SetPtr( pm.Uefd );
    aObj[ o_wd_uefd ].SetDim( pm.N, 1 );
    aObj[ o_wd_ur].SetPtr( pm.U_r );
    aObj[ o_wd_ur ].SetDim( pm.N, 1 );
    aObj[ o_wo_c].SetPtr( pm.C );
    aObj[ o_wo_c ].SetDim( pm.N, 1 );
    aObj[ o_wd_icm].SetPtr( pm.IC_m );
    aObj[ o_wd_icm ].SetDim( pm.N, 1 );
    aObj[ o_wd_iclcm].SetPtr( pm.IC_lm );
    aObj[ o_wd_iclcm ].SetDim( pm.N, 1 );
    aObj[ o_wd_icwm].SetPtr( pm.IC_wm );
    aObj[ o_wd_icwm ].SetDim( pm.N, 1 );

    // part 3
    aObj[ o_w_xfts].SetPtr( pm.XFTS );
    aObj[ o_w_xfts ].SetDim(pm.FIs,pm.FIat);
//  Changed 27.10.2004 for CD MUSIC
    aObj[ o_wo_masdj].SetPtr( pm.MASDJ );
    aObj[ o_wo_masdj ].SetDim( pm.Lads, DFCN );
//    aObj[ o_wo_masdj ].SetDim( pm.Ls, 1 );
    aObj[ o_wo_bf].SetPtr( pm.BF );
    aObj[ o_wo_bf ].SetDim( pm.FIs, pm.N );
    aObj[ o_wo_bfc].SetPtr( pm.BFC );
    aObj[ o_wo_bfc ].SetDim( 1, pm.N );
    aObj[ o_w_xf].SetPtr( pm.XF );
    aObj[ o_w_xf ].SetDim( pm.FI, 1 );
    aObj[ o_wo_yf].SetPtr( pm.YF );
    aObj[ o_wo_yf ].SetDim( pm.FI, 1 );
    aObj[ o_ww_xfa].SetPtr( pm.XFA );
    aObj[ o_ww_xfa ].SetDim( pm.FIs, 1 );
    aObj[ o_ww_yfa].SetPtr( pm.YFA );
    aObj[ o_ww_yfa ].SetDim( pm.FIs, 1 );
    aObj[ o_wo_falp].SetPtr( pm.Falp );
    aObj[ o_wo_falp ].SetDim( pm.FI, 1 );
    aObj[ o_w_xetaa].SetPtr( pm.XetaA );
    aObj[ o_w_xetaa].SetDim(pm.FIs,pm.FIat);
    aObj[ o_w_xetab].SetPtr( pm.XetaB );
    aObj[ o_w_xetab].SetDim(pm.FIs,pm.FIat);
    aObj[ o_w_xetad].SetPtr( pm.XetaD );    // Added 12.09.05 KD
    aObj[ o_w_xetad].SetDim(pm.FIs,pm.FIat);
    aObj[ o_w_x].SetPtr( pm.X );
    aObj[ o_w_x ].SetDim( pm.L, 1 );
    aObj[ o_wo_y].SetPtr( pm.Y );
    aObj[ o_wo_y ].SetDim( pm.L, 1 );
    aObj[ o_w_xy].SetPtr( pm.XY );
    aObj[ o_w_xy ].SetDim( pm.L, 1 );
    aObj[ o_w_xu].SetPtr( pm.XU );
    aObj[ o_w_xu ].SetDim( pm.L, 1 );
    aObj[ o_wi_qp].SetPtr( pm.Qp );        // aObj[ o_wi_qp ].SetDim( pm., 1 );
    aObj[ o_wi_qd].SetPtr( pm.Qd );        // aObj[ o_wi_qd ].SetDim( pm., 1 );
    aObj[ o_wo_mu].SetPtr( pm.MU );
    aObj[ o_wo_mu ].SetDim( pm.L, 1 );
    aObj[ o_w_emu].SetPtr( pm.EMU );
    aObj[ o_w_emu ].SetDim( pm.L, 1 );
    aObj[ o_w_nmu].SetPtr( pm.NMU );
    aObj[ o_w_nmu ].SetDim( pm.L, 1 );
    aObj[ o_w_w].SetPtr( pm.W );
    aObj[ o_w_w ].SetDim( pm.L, 1 );
    aObj[ o_wo_fxa].SetPtr( pm.Fx );
    aObj[ o_wo_fxa ].SetDim( pm.L, 1 );
    aObj[ o_wo_wx].SetPtr( pm.Wx );
    aObj[ o_wo_wx ].SetDim( pm.L, 1 );
    aObj[ o_wo_f].SetPtr( pm.F );
    aObj[ o_wo_f ].SetDim( pm.L, 1 );
    aObj[ o_w_f0].SetPtr( pm.F0 );
    aObj[ o_w_f0 ].SetDim( pm.L, 1 );
    // Changed for CD MUSIC 27.10.2004 KD
    aObj[ o_w_d ].SetPtr( pm.D );
    aObj[ o_w_d ].SetDim( MST, MST );
    aObj[ o_wi_smod ].SetPtr( pm.sMod );
    aObj[ o_wi_smod ].SetDim( pm.FIs, 1 );
    aObj[ o_wi_dcmod ].SetPtr( pm.dcMod );
    aObj[ o_wi_dcmod ].SetDim( pm.L, 1 );
    aObj[ o_wd_sb ].SetPtr( pm.SB );
    aObj[ o_wd_sb ].SetDim( pm.N, 1 );
    aObj[ o_w_sbh ].SetPtr( pm.SB1 );
    aObj[ o_w_sbh ].SetDim( 1, pm.N );
    aObj[ o_wd_sm ].SetPtr( pm.SM );
    aObj[ o_wd_sm ].SetDim( pm.L, 1 );
    aObj[ o_wd_sf ].SetPtr( pm.SF );
    aObj[ o_wd_sf ].SetDim( pm.FI, 1 );
    aObj[ o_wd_sm2 ].SetPtr( pm.SM2 );
    aObj[ o_wd_sm2 ].SetDim( pm.Ls, 1 );
// Added 27.10.2004 for CD MUSIC models
    aObj[ o_wd_sm3 ].SetPtr( pm.SM3 );
    aObj[ o_wd_sm3 ].SetDim( pm.Lads, 1 );
//
    aObj[ o_wd_sf2].SetPtr( pm.SF2 );
    aObj[ o_wd_sf2 ].SetDim( pm.FIs, 1 );
    aObj[ o_wd_sfs].SetPtr( pm.SFs );
    aObj[ o_wd_sfs ].SetDim( pm.FI, 1 );
    aObj[ o_wd_txtb].SetPtr( pm.pbuf );    //aObj[o_wd_txtb].SetDim(1,size(pm.pbuf));
    aObj[ o_wi_rlc].SetPtr( pm.RLC );
    aObj[ o_wi_rlc ].SetDim( pm.L, 1 );
    aObj[ o_wi_rsc].SetPtr( pm.RSC );
    aObj[ o_wi_rsc ].SetDim( pm.L, 1 );
    aObj[ o_wi_rflc].SetPtr( pm.RFLC );
    aObj[ o_wi_rflc ].SetDim( pm.FIs, 1 );
    aObj[ o_wi_rfsc].SetPtr( pm.RFSC );
    aObj[ o_wi_rfsc ].SetDim( pm.FIs, 1 );
    aObj[ o_wi_icc].SetPtr( pm.ICC );
    aObj[ o_wi_icc ].SetDim( pm.N, 1 );
    aObj[ o_wi_dcc].SetPtr( pm.DCC );
    aObj[ o_wi_dcc ].SetDim( pm.L, 1 );
//  Added 27.10.2004 for CD MUSIC models
    aObj[ o_wi_dcc3 ].SetPtr( pm.DCC3 );
    aObj[ o_wi_dcc3 ].SetDim( pm.Lads, 1 );
//
    aObj[ o_wi_scm].SetPtr( pm.SCM );
    aObj[ o_wi_scm ].SetDim(pm.FIs,pm.FIat);
// Changed 27.10.2004
    aObj[ o_wi_satt].SetPtr( pm.SATT );
    aObj[ o_wi_satt ].SetDim( pm.Lads, 1 );
//
    aObj[ o_wi_dccw].SetPtr( pm.DCCW );
    aObj[ o_wi_dccw ].SetDim( pm.L, 1 );
    aObj[ o_wi_phc].SetPtr( pm.PHC );
    aObj[ o_wi_phc ].SetDim( pm.FI, 1 );
    aObj[ o_wd_fug].SetPtr( pm.Fug );
    aObj[ o_wd_fug ].SetDim( pm.PG, 1 );
    aObj[ o_wd_fugl].SetPtr( pm.Fug_l );
    aObj[ o_wd_fugl ].SetDim( pm.PG, 1 );
    aObj[ o_wd_ppgl].SetPtr( pm.Ppg_l );
    aObj[ o_wd_ppgl ].SetDim( pm.PG, 1 );
    aObj[ o_wio_wb].SetPtr( pm.Wb );
    aObj[ o_wio_wb ].SetDim( pm.Ls, 1 );
    aObj[ o_wio_wabs].SetPtr( pm.Wabs );
    aObj[ o_wio_wabs ].SetDim( pm.Ls, 1 );
    aObj[ o_wi_rion].SetPtr( pm.Rion );
    aObj[ o_wi_rion ].SetDim( pm.Ls, 1 );
//    aObj[ o_wo_lnsat].SetPtr( pm.lnSAT );
//    aObj[ o_wo_lnsat ].SetDim( pm.Ls, 1 );
    aObj[ o_wo_lnsat].SetPtr( pm.lnSAC );
    aObj[ o_wo_lnsat ].SetDim( pm.Lads, 4 );

// 11/02/2009
aObj[ o_wo_vph].SetPtr( pm.VPh );
aObj[ o_wo_vph ].SetDim( pm.FIs, MIXPHPROPS );
aObj[ o_wo_gph].SetPtr( pm.GPh );
aObj[ o_wo_gph ].SetDim( pm.FIs, MIXPHPROPS );
aObj[ o_wo_hph].SetPtr( pm.HPh );
aObj[ o_wo_hph ].SetDim( pm.FIs, MIXPHPROPS );
aObj[ o_wo_sph].SetPtr( pm.SPh );
aObj[ o_wo_sph ].SetDim( pm.FIs, MIXPHPROPS );
aObj[ o_wo_cph].SetPtr( pm.CPh );
aObj[ o_wo_cph ].SetDim( pm.FIs, MIXPHPROPS );
aObj[ o_wo_aph].SetPtr( pm.APh );
aObj[ o_wo_aph ].SetDim( pm.FIs, MIXPHPROPS );
aObj[ o_wo_uph].SetPtr( pm.UPh );
aObj[ o_wo_uph ].SetDim( pm.FIs, MIXPHPROPS );

// New phase stuff 06/06/12
     aObj[ o_wi_lsmdc2].SetPtr( pm.LsMdc2 );
     aObj[ o_wi_lsmdc2 ].SetDim( pm.FIs, 3 );
     aObj[ o_wi_lsphl].SetPtr( pm.LsPhl);
     aObj[ o_wi_lsphl ].SetDim( pm.FI, 2 );
     aObj[ o_wi_phlin].SetPtr( pm.PhLin ); //FI
// TSolMod stuff
     aObj[ o_wi_lphc].SetPtr( pm.lPhc );
     aObj[ o_wi_dqfc].SetPtr( pm.DQFc );
     aObj[ o_wi_rcpc].SetPtr( pm.rcpc );
     aObj[ o_wi_lndqft].SetPtr( pm.lnDQFt );
     aObj[ o_wi_lndqft].SetDim( pm.Ls, 1 );
     aObj[ o_wi_lnrcpt].SetPtr( pm.lnRcpt );
     aObj[ o_wi_lnrcpt].SetDim( pm.Ls, 1 );
     aObj[ o_wi_lnext].SetPtr( pm.lnEXt );
     aObj[ o_wi_lnext].SetDim( pm.Ls, 1 );
     aObj[ o_wi_lncnft].SetPtr( pm.lnCnft );
     aObj[ o_wi_lncnft].SetDim( pm.Ls, 1 );
//TSorpMod & TKinMet stuff
     aObj[ o_wi_sormc].SetPtr( pm.SorMc );
     aObj[ o_wi_sormc].SetDim( pm.FIs, 16 );
// TSorpMod stuff
     aObj[ o_wi_lsesmo].SetPtr( pm.LsESmo );
     aObj[ o_wi_lsesmo].SetDim( pm.FIs, 4 );
     aObj[ o_wi_lsismo].SetPtr( pm.LsISmo );
     aObj[ o_wi_lsismo].SetDim( pm.FIs, 4 );
     aObj[ o_wi_xsmd].SetPtr( pm.xSMd );
     aObj[ o_wi_eimc].SetPtr( pm.EImc );
     aObj[ o_wi_mcdc].SetPtr( pm.mCDc );
     aObj[ o_wi_isopc].SetPtr( pm.IsoPc );
     aObj[ o_wi_isosc].SetPtr( pm.IsoSc );
     aObj[ o_wi_lnscal].SetPtr( pm.lnScalT );
     aObj[ o_wi_lnscal].SetDim( pm.Ls, 1 );
     aObj[ o_wi_lnsact].SetPtr( pm.lnSACT );
     aObj[ o_wi_lnsact].SetDim( pm.Ls, 1 );
     aObj[ o_wi_lngamf].SetPtr( pm.lnGammF );
     aObj[ o_wi_lngamf].SetDim( pm.Ls, 1 );
     aObj[ o_wi_cterms].SetPtr( pm.CTerms );
     aObj[ o_wi_cterms].SetDim( pm.Ls, 1 );
     aObj[ o_wi_isoct].SetPtr( pm.IsoCt );
// TKinMet stuff
        aObj[ o_wi_lskin].SetPtr( pm.LsKin );
        aObj[ o_wi_lskin].SetDim( pm.FI, 4 );
        aObj[ o_wi_lsupt].SetPtr( pm.LsUpt );
        aObj[ o_wi_lsupt].SetDim( pm.FIs, 2 );
        aObj[ o_wi_jcrdc].SetPtr( pm.jCrDC );
        aObj[ o_wi_xfaces].SetPtr( pm.xfaces );
        aObj[ o_wi_fsac].SetPtr( pm.fSak );
        aObj[ o_wi_krpc].SetPtr( pm.Krpc );
        aObj[ o_wi_umpc].SetPtr( pm.UMpc );
        aObj[ o_wi_kmod].SetPtr( pm. kMod);
        aObj[ o_wi_kmod].SetDim( pm.FI, 1 );
}

// set dynamic Objects ptr to values
void TMulti::dyn_set(int /*q*/)
{

   pm.L1    = (long int *)aObj[ o_wi_l1 ].GetPtr();
   pm.IPx = (long int *)aObj[ o_wi_ipxpm ].GetPtr();   // added 07.12.2006  KD
   pm.LsMod = (long int *)aObj[ o_wi_lsmod ].GetPtr();
    pm.LsMdc = (long int *)aObj[ o_wi_lsmdc ].GetPtr();
    pm.mui   = (long int *)aObj[ o_wi_mui ].GetPtr();
    pm.muk   = (long int *)aObj[ o_wi_muk ].GetPtr();
    pm.muj   = (long int *)aObj[ o_wi_muj ].GetPtr();
    pm.SATX =(long int (*)[4])aObj[ o_wi_satndx ].GetPtr();
    pm.DUL   = (double *)aObj[ o_wi_dul ].GetPtr();
    pm.DLL   = (double *)aObj[ o_wi_dll ].GetPtr();
    pm.fDQF   = (double *)aObj[ o_wi_gex ].GetPtr();
    pm.PUL   = (double *)aObj[ o_wi_pul ].GetPtr();
    pm.PLL   = (double *)aObj[ o_wi_pll ].GetPtr();
    pm.YOF   = (double *)aObj[ o_wi_yof ].GetPtr();
    pm.PMc   = (double *)aObj[ o_wi_pmc ].GetPtr();
    pm.DMc   = (double *)aObj[ o_wi_dmc ].GetPtr();
    pm.MoiSN  = (double *)aObj[ o_wi_moisn ].GetPtr();
    pm.SitFr  = (double *)aObj[ o_wo_sitfr ].GetPtr();
    pm.Vol   = (double *)aObj[ o_wio_vol ].GetPtr();
    pm.HYM   = (double *)aObj[ o_w_hym ].GetPtr();
    pm.VL    = (double *)aObj[ o_w_vl ].GetPtr();
    pm.MM    = (double *)aObj[ o_wi_mm ].GetPtr();
    pm.H0    = (double *)aObj[ o_wio_h0 ].GetPtr();
    pm.A0    = (double *)aObj[ o_wio_a0 ].GetPtr();
    pm.U0    = (double *)aObj[ o_wio_u0 ].GetPtr();
    pm.S0    = (double *)aObj[ o_wio_s0 ].GetPtr();
    pm.Cp0   = (double *)aObj[ o_wio_cp0 ].GetPtr();
    pm.Cv0   = (double *)aObj[ o_wio_cv0 ].GetPtr();
    pm.Pparc = (double *)aObj[ o_wi_pparc ].GetPtr();
    pm.Y_m   = (double *)aObj[ o_wd_ym ].GetPtr();
    pm.Y_la  = (double *)aObj[ o_wd_yla ].GetPtr();
    pm.Y_w   = (double *)aObj[ o_wd_yw ].GetPtr();
    pm.Gamma = (double *)aObj[ o_wd_gamma ].GetPtr();
    pm.lnGmf = (double *)aObj[ o_wi_lngmf ].GetPtr();
    pm.lnGmM = (double *)aObj[ o_wio_lngmm ].GetPtr();
    pm.EZ    = (double *)aObj[ o_wi_ez ].GetPtr();
    pm.Wb    = (double *)aObj[ o_wio_wb ].GetPtr();
    pm.Wabs  = (double *)aObj[ o_wio_wabs ].GetPtr();
    pm.Rion  = (double *)aObj[ o_wi_rion ].GetPtr();
    pm.Aalp  = (double *)aObj[ o_wi_aalp ].GetPtr();
    pm.Sigw  = (double *)aObj[ o_wi_sigw ].GetPtr();
    pm.Sigg  = (double *)aObj[ o_wi_sigg ].GetPtr();
    pm.Nfsp  = (double (*)[MST])aObj[ o_wi_nfsp ].GetPtr();
    pm.MASDT = (double (*)[MST])aObj[ o_wi_masdt ].GetPtr();
    pm.FVOL  = (double *)aObj[ o_wd_fvol ].GetPtr();
    pm.FWGT  = (double *)aObj[ o_wd_fwgt ].GetPtr();
    pm.XcapA = (double (*)[MST])aObj[ o_w_xcapa ].GetPtr();
    pm.XcapB = (double (*)[MST])aObj[ o_w_xcapb ].GetPtr();
    pm.XcapD = (double (*)[MST])aObj[ o_w_xcapd ].GetPtr();
    pm.XcapF = (double (*)[MST])aObj[ o_w_xcapf ].GetPtr();
    pm.XdlA  = (double (*)[MST])aObj[ o_wo_xdla ].GetPtr();
    pm.XdlB  = (double (*)[MST])aObj[ o_wo_xdlb ].GetPtr();
    pm.XdlD  = (double (*)[MST])aObj[ o_wo_xdld ].GetPtr();
    pm.XpsiA = (double (*)[MST])aObj[ o_wo_psia ].GetPtr();
    pm.XpsiB = (double (*)[MST])aObj[ o_wo_psib ].GetPtr();
    pm.XpsiD = (double (*)[MST])aObj[ o_wo_psid ].GetPtr();
    pm.Xr0h0 = (double (*)[2])aObj[ o_wi_xr0h0 ].GetPtr();
    pm.XlamA = (double (*)[MST])aObj[ o_wi_xlama ].GetPtr();
    pm.Xetaf = (double (*)[MST])aObj[ o_wi_xetaf ].GetPtr();
    pm.Xcond = (double *)aObj[ o_wi_xcond ].GetPtr();
    pm.Xeps  = (double *)aObj[ o_wi_xeps ].GetPtr();
    pm.Awt   = (double *)aObj[ o_wi_awt ].GetPtr();
    pm.A     = (double *)aObj[ o_wi_a ].GetPtr();
    pm.XFs   = (double *)aObj[ o_wd_xfs ].GetPtr();
    pm.Falps = (double *)aObj[ o_wd_falps ].GetPtr();
    pm.Fug   = (double *)aObj[ o_wd_fug ].GetPtr();
    pm.Fug_l = (double *)aObj[ o_wd_fugl ].GetPtr();
    pm.Ppg_l = (double *)aObj[ o_wd_ppgl ].GetPtr();
    pm.XFTS  = (double (*)[MST])aObj[ o_w_xfts ].GetPtr();
    pm.MASDJ = (double (*)[DFCN])aObj[ o_wo_masdj ].GetPtr();
    pm.G     = (double *)aObj[ o_wo_g ].GetPtr();
    pm.G0    = (double *)aObj[ o_wi_g0 ].GetPtr();
    pm.lnGam = (double *)aObj[ o_wo_lngam ].GetPtr();
    pm.lnGmo = (double *)aObj[ o_w_lngmo ].GetPtr();
    pm.lnSAC = (double (*)[4])aObj[ o_wo_lnsat ].GetPtr();
//    pm.lnSAT = (double *)aObj[ o_wo_lnsat ].GetPtr();
    pm.B     = (double *)aObj[ o_wi_b ].GetPtr();
    pm.U     = (double *)aObj[ o_wo_u ].GetPtr();
    pm.Uc   = (double (*)[2])aObj[ o_wd_uc ].GetPtr();
    pm.Uefd   = (double *)aObj[ o_wd_uefd ].GetPtr();
    pm.U_r   = (double *)aObj[ o_wd_ur ].GetPtr();
    pm.C     = (double *)aObj[ o_wo_c ].GetPtr();
    pm.IC_m  = (double *)aObj[ o_wd_icm ].GetPtr();
    pm.IC_lm = (double *)aObj[ o_wd_iclcm ].GetPtr();
    pm.IC_wm = (double *)aObj[ o_wd_icwm ].GetPtr();
    pm.BF    = (double *)aObj[ o_wo_bf ].GetPtr();
    pm.BFC    = (double *)aObj[ o_wo_bfc ].GetPtr();
    pm.XF    = (double *)aObj[ o_w_xf ].GetPtr();
    pm.YF    = (double *)aObj[ o_wo_yf ].GetPtr();
    pm.XFA   = (double *)aObj[ o_ww_xfa ].GetPtr();
    pm.YFA   = (double *)aObj[ o_ww_yfa ].GetPtr();
    pm.Falp  = (double *)aObj[ o_wo_falp ].GetPtr();
    pm.XetaA = (double (*)[MST])aObj[ o_w_xetaa ].GetPtr();
    pm.XetaB = (double (*)[MST])aObj[ o_w_xetab ].GetPtr();
    pm.XetaD = (double (*)[MST])aObj[ o_w_xetad ].GetPtr(); // added 12.09.05 KD
    pm.X     = (double *)aObj[ o_w_x ].GetPtr();
    pm.Y     = (double *)aObj[ o_wo_y ].GetPtr();
    pm.XY    = (double *)aObj[ o_w_xy ].GetPtr();
    pm.XU    = (double *)aObj[ o_w_xu ].GetPtr();
    pm.Qp    = (double *)aObj[ o_wi_qp ].GetPtr();
    pm.Qd    = (double *)aObj[ o_wi_qd ].GetPtr();
    pm.MU    = (double *)aObj[ o_wo_mu ].GetPtr();
    pm.EMU   = (double *)aObj[ o_w_emu ].GetPtr();
    pm.NMU   = (double *)aObj[ o_w_nmu ].GetPtr();
    pm.W     = (double *)aObj[ o_w_w ].GetPtr();
    pm.Fx    = (double *)aObj[ o_wo_fxa ].GetPtr();
    pm.Wx    = (double *)aObj[ o_wo_wx ].GetPtr();
    pm.F     = (double *)aObj[ o_wo_f ].GetPtr();
    pm.F0    = (double *)aObj[ o_w_f0 ].GetPtr();
    pm.D     = (double (*)[MST])aObj[ o_w_d ].GetPtr();
//    pm.R     = (double *)aObj[ o_w_r ].GetPtr();
//    pm.R1    = (double *)aObj[ o_w_r1 ].GetPtr();
    pm.sMod  = (char (*)[8])aObj[ o_wi_smod ].GetPtr();
    pm.dcMod  = (char (*)[6])aObj[ o_wi_dcmod ].GetPtr();
    pm.SB    = (char (*)[MAXICNAME+MAXSYMB])aObj[ o_wd_sb ].GetPtr();
    pm.SB1    = (char (*)[MAXICNAME])aObj[ o_w_sbh ].GetPtr();
    pm.SM    = (char (*)[MAXDCNAME])aObj[ o_wd_sm ].GetPtr();
    pm.SF    = (char (*)[MAXPHNAME+MAXSYMB])aObj[ o_wd_sf ].GetPtr();
    pm.SM2    = (char (*)[MAXDCNAME])aObj[ o_wd_sm2 ].GetPtr();
    pm.SM3    = (char (*)[MAXDCNAME])aObj[ o_wd_sm3 ].GetPtr();
    pm.SF2    = (char (*)[MAXPHNAME+MAXSYMB])aObj[ o_wd_sf2 ].GetPtr();
    pm.SFs   = (char (*)[MAXPHNAME+MAXSYMB])aObj[ o_wd_sfs ].GetPtr();
    pm.pbuf  = (char *)aObj[ o_wd_txtb ].GetPtr();
    pm.RLC   = (char *)aObj[ o_wi_rlc ].GetPtr();
    pm.RSC   = (char *)aObj[ o_wi_rsc ].GetPtr();
    pm.RFLC  = (char *)aObj[ o_wi_rflc ].GetPtr();
    pm.RFSC  = (char *)aObj[ o_wi_rfsc ].GetPtr();
    pm.ICC   = (char *)aObj[ o_wi_icc ].GetPtr();
    pm.DCC   = (char *)aObj[ o_wi_dcc ].GetPtr();
    pm.DCC3   = (char *)aObj[ o_wi_dcc3 ].GetPtr();
    pm.PHC   = (char *)aObj[ o_wi_phc ].GetPtr();
    pm.SCM   = (char (*)[MST])aObj[ o_wi_scm ].GetPtr();
    pm.SATT  = (char *)aObj[ o_wi_satt ].GetPtr();
    pm.DCCW  = (char *)aObj[ o_wi_dccw ].GetPtr();
    // 11/02/2009
    pm.VPh = (double (*)[MIXPHPROPS])aObj[ o_wo_vph].GetPtr();
    pm.GPh = (double (*)[MIXPHPROPS])aObj[ o_wo_gph].GetPtr();
    pm.HPh = (double (*)[MIXPHPROPS])aObj[ o_wo_hph].GetPtr();
    pm.SPh = (double (*)[MIXPHPROPS])aObj[ o_wo_sph].GetPtr();
    pm.CPh = (double (*)[MIXPHPROPS])aObj[ o_wo_cph].GetPtr();
    pm.APh = (double (*)[MIXPHPROPS])aObj[ o_wo_aph].GetPtr();
    pm.UPh = (double (*)[MIXPHPROPS])aObj[ o_wo_uph].GetPtr();
    // New phase stuff 06/06/12
    pm.LsMdc2  = (long int *)aObj[ o_wi_lsmdc2].GetPtr();
    pm.LsPhl   = (long int *)aObj[ o_wi_lsphl].GetPtr();
    pm.PhLin   = (long int *)aObj[ o_wi_phlin].GetPtr();
    // TSolMod stuff
    pm.lPhc   = (double *)aObj[ o_wi_lphc].GetPtr();
    pm.DQFc   = (double *)aObj[ o_wi_dqfc].GetPtr();
    pm.rcpc   = (double *)aObj[ o_wi_rcpc].GetPtr();
    pm.lnDQFt   = (double *)aObj[ o_wi_lndqft].GetPtr();
    pm.lnRcpt   = (double *)aObj[ o_wi_lnrcpt].GetPtr();
    pm.lnEXt   = (double *)aObj[ o_wi_lnext].GetPtr();
    pm.lnCnft   = (double *)aObj[ o_wi_lncnft].GetPtr();
    //TSorpMod & TKinMet stuff
    pm.SorMc   = (double *)aObj[ o_wi_sormc].GetPtr();
    // TSorpMod stuff
    pm.LsESmo   = (long int *)aObj[ o_wi_lsesmo].GetPtr();
    pm.LsISmo   = (long int *)aObj[ o_wi_lsismo].GetPtr();
    pm.xSMd   = (long int *)aObj[ o_wi_xsmd].GetPtr();
    pm.EImc   = (double *)aObj[ o_wi_eimc].GetPtr();
    pm.mCDc   = (double *)aObj[ o_wi_mcdc].GetPtr();
    pm.IsoPc   = (double *)aObj[ o_wi_isopc].GetPtr();
    pm.IsoSc   = (double *)aObj[ o_wi_isosc].GetPtr();
    pm.lnScalT   = (double *)aObj[ o_wi_lnscal].GetPtr();
    pm.lnSACT   = (double *)aObj[ o_wi_lnsact].GetPtr();
    pm.lnGammF   = (double *)aObj[ o_wi_lngamf].GetPtr();
    pm.CTerms   = (double *)aObj[ o_wi_cterms].GetPtr();
    pm.IsoCt   = (char *)aObj[ o_wi_isoct].GetPtr();
    // TKinMet stuff
    pm.LsKin   = (long int *)aObj[ o_wi_lskin].GetPtr();
    pm.LsUpt   = (long int *)aObj[ o_wi_lsupt].GetPtr();
    pm.jCrDC   = (long int *)aObj[ o_wi_jcrdc].GetPtr();
    pm.xfaces   = (long int *)aObj[ o_wi_xfaces].GetPtr();
    pm.fSak   = (double *)aObj[ o_wi_fsac].GetPtr();
    pm.Krpc   = (double *)aObj[ o_wi_krpc].GetPtr();
    pm.UMpc   = (double *)aObj[ o_wi_umpc].GetPtr();
    pm.kMod   = (char(*)[4])aObj[ o_wi_kmod].GetPtr();
}


// free dynamic memory in objects and values
void TMulti::dyn_kill(int /*q*/)
{
    pm.L1    = (long int *)aObj[ o_wi_l1 ].Free();
    pm.IPx = (long int *)aObj[ o_wi_ipxpm ].Free();   // added 07.12.2006  KD
    pm.LsMod = (long int *)aObj[ o_wi_lsmod ].Free();
    pm.LsMdc = (long int *)aObj[ o_wi_lsmdc ].Free();
    pm.mui   = (long int *)aObj[ o_wi_mui ].Free();
    pm.muk   = (long int *)aObj[ o_wi_muk ].Free();
    pm.muj   = (long int *)aObj[ o_wi_muj ].Free();
    pm.SATX =(long int (*)[4])aObj[ o_wi_satndx ].Free();
    pm.DUL   = (double *)aObj[ o_wi_dul ].Free();
    pm.DLL   = (double *)aObj[ o_wi_dll ].Free();
    pm.fDQF   = (double *)aObj[ o_wi_gex ].Free();
    pm.PUL   = (double *)aObj[ o_wi_pul ].Free();
    pm.PLL   = (double *)aObj[ o_wi_pll ].Free();
    pm.YOF   = (double *)aObj[ o_wi_yof ].Free();
    pm.PMc   = (double *)aObj[ o_wi_pmc ].Free();
    pm.DMc   = (double *)aObj[ o_wi_dmc ].Free();
    pm.MoiSN  = (double *)aObj[ o_wi_moisn ].Free();
    pm.SitFr  = (double *)aObj[ o_wo_sitfr ].Free();
    pm.Vol   = (double *)aObj[ o_wio_vol ].Free();
    pm.HYM   = (double *)aObj[ o_w_hym ].Free();
    pm.VL    = (double *)aObj[ o_w_vl ].Free();
    pm.MM    = (double *)aObj[ o_wi_mm ].Free();
    pm.H0    = (double *)aObj[ o_wio_h0 ].Free();
    pm.A0    = (double *)aObj[ o_wio_a0 ].Free();
    pm.U0    = (double *)aObj[ o_wio_u0 ].Free();
    pm.S0    = (double *)aObj[ o_wio_s0 ].Free();
    pm.Cp0   = (double *)aObj[ o_wio_cp0 ].Free();
    pm.Cv0   = (double *)aObj[ o_wio_cv0 ].Free();
    pm.Pparc = (double *)aObj[ o_wi_pparc ].Free();
    pm.Y_m   = (double *)aObj[ o_wd_ym ].Free();
    pm.Y_la  = (double *)aObj[ o_wd_yla ].Free();
    pm.Y_w   = (double *)aObj[ o_wd_yw ].Free();
    pm.Gamma = (double *)aObj[ o_wd_gamma ].Free();
    pm.lnGmf = (double *)aObj[ o_wi_lngmf ].Free();
    pm.lnGmM = (double *)aObj[ o_wio_lngmm ].Free();
    pm.EZ    = (double *)aObj[ o_wi_ez ].Free();
    pm.Wb    = (double *)aObj[ o_wio_wb ].Free();
    pm.Wabs  = (double *)aObj[ o_wio_wabs ].Free();
    pm.Rion  = (double *)aObj[ o_wi_rion ].Free();
    pm.Aalp  = (double *)aObj[ o_wi_aalp ].Free();
    pm.Sigw  = (double *)aObj[ o_wi_sigw ].Free();
    pm.Sigg  = (double *)aObj[ o_wi_sigg ].Free();
    pm.Nfsp  = (double (*)[MST])aObj[ o_wi_nfsp ].Free();
    pm.MASDT = (double (*)[MST])aObj[ o_wi_masdt ].Free();
    pm.FVOL  = (double *)aObj[ o_wd_fvol ].Free();
    pm.FWGT  = (double *)aObj[ o_wd_fwgt ].Free();
    pm.XcapA = (double (*)[MST])aObj[ o_w_xcapa ].Free();
    pm.XcapB = (double (*)[MST])aObj[ o_w_xcapb ].Free();
    pm.XcapD = (double (*)[MST])aObj[ o_w_xcapd ].Free();
    pm.XcapF = (double (*)[MST])aObj[ o_w_xcapf ].Free();
    pm.XdlA  = (double (*)[MST])aObj[ o_wo_xdla ].Free();
    pm.XdlB  = (double (*)[MST])aObj[ o_wo_xdlb ].Free();
    pm.XdlD  = (double (*)[MST])aObj[ o_wo_xdld ].Free();
    pm.XpsiA = (double (*)[MST])aObj[ o_wo_psia ].Free();
    pm.XpsiB = (double (*)[MST])aObj[ o_wo_psib ].Free();
    pm.XpsiD = (double (*)[MST])aObj[ o_wo_psid ].Free();
    pm.Xr0h0 = (double (*)[2])aObj[ o_wi_xr0h0 ].Free();
    pm.XlamA = (double (*)[MST])aObj[ o_wi_xlama ].Free();
    pm.Xetaf = (double (*)[MST])aObj[ o_wi_xetaf ].Free();
    pm.Xcond = (double *)aObj[ o_wi_xcond ].Free();
    pm.Xeps  = (double *)aObj[ o_wi_xeps ].Free();
    pm.Awt   = (double *)aObj[ o_wi_awt ].Free();
    pm.A     = (double *)aObj[ o_wi_a ].Free();
    pm.XFs   = (double *)aObj[ o_wd_xfs ].Free();
    pm.Falps = (double *)aObj[ o_wd_falps ].Free();
    pm.Fug   = (double *)aObj[ o_wd_fug ].Free();
    pm.Fug_l = (double *)aObj[ o_wd_fugl ].Free();
    pm.Ppg_l = (double *)aObj[ o_wd_ppgl ].Free();
    pm.XFTS  = (double (*)[MST])aObj[ o_w_xfts ].Free();
    pm.MASDJ = (double (*)[DFCN])aObj[ o_wo_masdj ].Free();
    pm.G     = (double *)aObj[ o_wo_g ].Free();
    pm.G0    = (double *)aObj[ o_wi_g0 ].Free();
    pm.lnGam = (double *)aObj[ o_wo_lngam ].Free();
    pm.lnGmo = (double *)aObj[ o_w_lngmo ].Free();
//    pm.lnSAT = (double *)aObj[ o_wo_lnsat ].Free();
    pm.lnSAC = (double (*)[4])aObj[ o_wo_lnsat ].Free();
    pm.B     = (double *)aObj[ o_wi_b ].Free();
    pm.U     = (double *)aObj[ o_wo_u ].Free();
    pm.Uc   = (double (*)[2])aObj[ o_wd_uc ].Free();
    pm.Uefd   = (double *)aObj[ o_wd_uefd ].Free();
    pm.U_r   = (double *)aObj[ o_wd_ur ].Free();
    pm.C     = (double *)aObj[ o_wo_c ].Free();
    pm.IC_m  = (double *)aObj[ o_wd_icm ].Free();
    pm.IC_lm = (double *)aObj[ o_wd_iclcm ].Free();
    pm.IC_wm = (double *)aObj[ o_wd_icwm ].Free();
    pm.BF    = (double *)aObj[ o_wo_bf ].Free();
    pm.BFC    = (double *)aObj[ o_wo_bfc ].Free();
    pm.XF    = (double *)aObj[ o_w_xf ].Free();
    pm.YF    = (double *)aObj[ o_wo_yf ].Free();
    pm.XFA   = (double *)aObj[ o_ww_xfa ].Free();
    pm.YFA   = (double *)aObj[ o_ww_yfa ].Free();
    pm.Falp  = (double *)aObj[ o_wo_falp ].Free();
    pm.XetaA = (double (*)[MST])aObj[ o_w_xetaa ].Free();
    pm.XetaB = (double (*)[MST])aObj[ o_w_xetab ].Free();
    pm.XetaD = (double (*)[MST])aObj[ o_w_xetad ].Free();
    pm.X     = (double *)aObj[ o_w_x ].Free();
    pm.Y     = (double *)aObj[ o_wo_y ].Free();
    pm.XY    = (double *)aObj[ o_w_xy ].Free();
    pm.XU    = (double *)aObj[ o_w_xu ].Free();
    pm.Qp    = (double *)aObj[ o_wi_qp ].Free();
    pm.Qd    = (double *)aObj[ o_wi_qd ].Free();
    pm.MU    = (double *)aObj[ o_wo_mu ].Free();
    pm.EMU   = (double *)aObj[ o_w_emu ].Free();
    pm.NMU   = (double *)aObj[ o_w_nmu ].Free();
    pm.W     = (double *)aObj[ o_w_w ].Free();
    pm.Fx    = (double *)aObj[ o_wo_fxa ].Free();
    pm.Wx    = (double *)aObj[ o_wo_wx ].Free();
    pm.F     = (double *)aObj[ o_wo_f ].Free();
    pm.F0    = (double *)aObj[ o_w_f0 ].Free();
    pm.D     = (double (*)[MST])aObj[ o_w_d ].Free();
//    pm.R     = (double *)aObj[ o_w_r ].Free();
//    pm.R1    = (double *)aObj[ o_w_r1 ].Free();
    pm.sMod  = (char (*)[8])aObj[ o_wi_smod ].Free();
    pm.dcMod  = (char (*)[6])aObj[ o_wi_dcmod ].Free();
    pm.SB    = (char (*)[MAXICNAME+MAXSYMB])aObj[ o_wd_sb ].Free();
    pm.SB1   = (char (*)[MAXICNAME])aObj[ o_w_sbh ].Free();
    pm.SM    = (char (*)[MAXDCNAME])aObj[ o_wd_sm ].Free();
    pm.SF    = (char (*)[MAXPHNAME+MAXSYMB])aObj[ o_wd_sf ].Free();
    pm.SM2    = (char (*)[MAXDCNAME])aObj[ o_wd_sm2 ].Free();
    pm.SM3    = (char (*)[MAXDCNAME])aObj[ o_wd_sm3 ].Free();
    pm.SF2    = (char (*)[MAXPHNAME+MAXSYMB])aObj[ o_wd_sf2 ].Free();
    pm.SFs   = (char (*)[MAXPHNAME+MAXSYMB])aObj[ o_wd_sfs ].Free();
    pm.pbuf  = (char *)aObj[ o_wd_txtb ].Free(); //
    pm.RLC   = (char *)aObj[ o_wi_rlc ].Free();
    pm.RSC   = (char *)aObj[ o_wi_rsc ].Free();
    pm.RFLC  = (char *)aObj[ o_wi_rflc ].Free();
    pm.RFSC  = (char *)aObj[ o_wi_rfsc ].Free();
    pm.ICC   = (char *)aObj[ o_wi_icc ].Free();
    pm.DCC   = (char *)aObj[ o_wi_dcc ].Free();
    pm.DCC3   = (char *)aObj[ o_wi_dcc3 ].Free();
    pm.PHC   = (char *)aObj[ o_wi_phc ].Free();
    pm.SCM   = (char (*)[MST])aObj[ o_wi_scm ].Free();
    pm.SATT  = (char *)aObj[ o_wi_satt ].Free();
    pm.DCCW  = (char *)aObj[ o_wi_dccw ].Free();
    // 11/02/2009
    pm.VPh = (double (*)[MIXPHPROPS])aObj[ o_wo_vph].Free();
    pm.GPh = (double (*)[MIXPHPROPS])aObj[ o_wo_gph].Free();
    pm.HPh = (double (*)[MIXPHPROPS])aObj[ o_wo_hph].Free();
    pm.SPh = (double (*)[MIXPHPROPS])aObj[ o_wo_sph].Free();
    pm.CPh = (double (*)[MIXPHPROPS])aObj[ o_wo_cph].Free();
    pm.APh = (double (*)[MIXPHPROPS])aObj[ o_wo_aph].Free();
    pm.UPh = (double (*)[MIXPHPROPS])aObj[ o_wo_uph].Free();
    // New phase stuff 06/06/12
    pm.LsMdc2  = (long int *)aObj[ o_wi_lsmdc2].Free();
    pm.LsPhl   = (long int *)aObj[ o_wi_lsphl].Free();
    pm.PhLin   = (long int *)aObj[ o_wi_phlin].Free();
    // TSolMod stuff
    pm.lPhc   = (double *)aObj[ o_wi_lphc].Free();
    pm.DQFc   = (double *)aObj[ o_wi_dqfc].Free();
    pm.rcpc   = (double *)aObj[ o_wi_rcpc].Free();
    pm.lnDQFt   = (double *)aObj[ o_wi_lndqft].Free();
    pm.lnRcpt   = (double *)aObj[ o_wi_lnrcpt].Free();
    pm.lnEXt   = (double *)aObj[ o_wi_lnext].Free();
    pm.lnCnft   = (double *)aObj[ o_wi_lncnft].Free();
    //TSorpMod & TKinMet stuff
    pm.SorMc   = (double *)aObj[ o_wi_sormc].Free();
    // TSorpMod stuff
    pm.LsESmo   = (long int *)aObj[ o_wi_lsesmo].Free();
    pm.LsISmo   = (long int *)aObj[ o_wi_lsismo].Free();
    pm.xSMd   = (long int *)aObj[ o_wi_xsmd].Free();
    pm.EImc   = (double *)aObj[ o_wi_eimc].Free();
    pm.mCDc   = (double *)aObj[ o_wi_mcdc].Free();
    pm.IsoPc   = (double *)aObj[ o_wi_isopc].Free();
    pm.IsoSc   = (double *)aObj[ o_wi_isosc].Free();
    pm.lnScalT   = (double *)aObj[ o_wi_lnscal].Free();
    pm.lnSACT   = (double *)aObj[ o_wi_lnsact].Free();
    pm.lnGammF   = (double *)aObj[ o_wi_lngamf].Free();
    pm.CTerms   = (double *)aObj[ o_wi_cterms].Free();
    pm.IsoCt   = (char *)aObj[ o_wi_isoct].Free();
    // TKinMet stuff
    pm.LsKin   = (long int *)aObj[ o_wi_lskin].Free();
    pm.LsUpt   = (long int *)aObj[ o_wi_lsupt].Free();
    pm.jCrDC   = (long int *)aObj[ o_wi_jcrdc].Free();
    pm.xfaces   = (long int *)aObj[ o_wi_xfaces].Free();
    pm.fSak   = (double *)aObj[ o_wi_fsac].Free();
    pm.Krpc   = (double *)aObj[ o_wi_krpc].Free();
    pm.UMpc   = (double *)aObj[ o_wi_umpc].Free();
    pm.kMod   = (char(*)[4])aObj[ o_wi_kmod].Free();

    Free_TSolMod();
}

// reallocation of dynamic memory
void TMulti::dyn_new(int /*q*/)
{
   if( pm.N < 2 || pm.L < 2 || pm.FI < 1 )
        Error( GetName(), "pm.N < 2 || pm.L < 2 || pm.FI < 1" );
    // Part 1
    // always needed
    pm.L1 = (long int *)aObj[ o_wi_l1].Alloc( pm.FI, 1, L_ );
    pm.muk = (long int *)aObj[ o_wi_muk].Alloc( pm.FI, 1, L_ );
    pm.mui = (long int *)aObj[ o_wi_mui].Alloc( pm.N, 1, L_);
    pm.muj = (long int *)aObj[ o_wi_muj].Alloc( pm.L, 1, L_ );
    pm.DUL = (double *)aObj[ o_wi_dul].Alloc( pm.L, 1, D_ );
    pm.DLL = (double *)aObj[ o_wi_dll].Alloc( pm.L, 1, D_ );
    pm.Vol = (double *)aObj[ o_wio_vol].Alloc( pm.L, 1, D_ );
    pm.Pparc = (double *)aObj[ o_wi_pparc].Alloc( pm.L, 1, D_ );
    pm.MM = (double *)aObj[ o_wi_mm].Alloc( pm.L, 1, D_ );
    pm.Awt = (double *)aObj[ o_wi_awt].Alloc( pm.N, 1, D_ );
    pm.A = (double *)aObj[ o_wi_a].Alloc( pm.L, pm.N, D_ );
    pm.XFs = (double *)aObj[ o_wd_xfs].Alloc( pm.FI, 1, D_ );
    pm.Falps = (double *)aObj[ o_wd_falps].Alloc( pm.FI, 1, D_ );
    pm.G = (double *)aObj[ o_wo_g].Alloc( pm.L, 1, D_);
    pm.G0 = (double *)aObj[ o_wi_g0].Alloc( pm.L, 1, D_);
    pm.lnGam = (double *)aObj[ o_wo_lngam].Alloc( pm.L, 1, D_);
    pm.lnGmo = (double *)aObj[ o_w_lngmo].Alloc( pm.L, 1, D_);
    pm.B = (double *)aObj[ o_wi_b].Alloc( pm.N, 1, D_ );
    pm.U = (double *)aObj[ o_wo_u].Alloc( pm.N, 1, D_ );
    pm.Uc = (double (*)[2])aObj[ o_wd_uc].Alloc( pm.N, 2, D_ );
    pm.Uefd = (double *)aObj[ o_wd_uefd].Alloc( pm.N, 1, D_ );
    pm.U_r = (double *)aObj[ o_wd_ur].Alloc( pm.N, 1, D_ );
    pm.C = (double *)aObj[ o_wo_c].Alloc( pm.N, 1, D_ );
    pm.XF = (double *)aObj[ o_w_xf].Alloc( pm.FI, 1, D_ );
    pm.YF = (double *)aObj[ o_wo_yf].Alloc( pm.FI, 1, D_ );
    pm.Falp = (double *)aObj[ o_wo_falp].Alloc( pm.FI, 1, D_ );
    pm.X = (double *)aObj[ o_w_x].Alloc( pm.L, 1, D_);
    pm.Y = (double *)aObj[ o_wo_y].Alloc( pm.L, 1, D_);
    pm.XY = (double *)aObj[ o_w_xy].Alloc( pm.L, 1, D_);
    pm.XU = (double *)aObj[ o_w_xu].Alloc( pm.L, 1, D_);
    pm.MU = (double *)aObj[ o_wo_mu].Alloc( pm.L, 1, D_);
    pm.EMU = (double *)aObj[ o_w_emu].Alloc( pm.L, 1, D_);
    pm.NMU = (double *)aObj[ o_w_nmu].Alloc( pm.L, 1, D_);
    pm.W = (double *)aObj[ o_w_w].Alloc( pm.L, 1, D_);
    pm.F = (double *)aObj[ o_wo_f].Alloc( pm.L, 1, D_);
    pm.F0 = (double *)aObj[ o_w_f0].Alloc( pm.L, 1, D_);
    pm.YOF = (double *)aObj[ o_wi_yof].Alloc( pm.FI, 1, D_ );
    pm.SB = (char (*)[MAXICNAME+MAXSYMB])aObj[ o_wd_sb].Alloc(
                pm.N, 1, MAXICNAME+MAXSYMB );
    pm.SB1 = (char (*)[MAXICNAME])aObj[ o_w_sbh].Alloc( 1, pm.N, MAXICNAME );

    // Error in freeing memory
    //aObj[ o_w_sbh].SetPtr( pm.SB ); aObj[ o_w_sbh ].SetDim( pm.N, 1 );
    pm.SFs = (char (*)[MAXPHNAME+MAXSYMB])aObj[ o_wd_sfs].Alloc(
                 pm.FI, 1, MAXPHNAME+MAXSYMB );
    pm.SM = (char (*)[MAXDCNAME])aObj[ o_wd_sm].Alloc( pm.L, 1, MAXDCNAME );
    pm.SF = (char (*)[MAXPHNAME+MAXSYMB])aObj[ o_wd_sf].Alloc(
                pm.FI, 1, MAXPHNAME+MAXSYMB );
    pm.SM2 = (char (*)[MAXDCNAME])aObj[ o_wd_sm2].Alloc( pm.Ls, 1, MAXDCNAME );
    pm.SF2 = (char (*)[MAXPHNAME+MAXSYMB])aObj[ o_wd_sf2].Alloc(
                 pm.FIs, 1, MAXPHNAME+MAXSYMB );

    pm.RLC = (char *)aObj[ o_wi_rlc].Alloc( pm.L, 1, A_ );
    pm.RSC = (char *)aObj[ o_wi_rsc].Alloc( pm.L, 1, A_ );
    pm.ICC = (char *)aObj[ o_wi_icc].Alloc( pm.N, 1, A_ );
    pm.DCC = (char *)aObj[ o_wi_dcc].Alloc( pm.L, 1, A_ );
    pm.PHC = (char *)aObj[ o_wi_phc].Alloc( pm.FI, 1, A_ );
    pm.DCCW = (char *)aObj[ o_wi_dccw].Alloc( pm.L, 1, A_ );

    pm.lnGmM = (double *)aObj[ o_wio_lngmm].Alloc( pm.L, 1, D_ );
    pm.fDQF = (double *)aObj[ o_wi_gex].Alloc( pm.L, 1, D_ );
    pm.FVOL = (double *)aObj[ o_wd_fvol].Alloc( pm.FI, 1, D_ );
    pm.FWGT = (double *)aObj[ o_wd_fwgt].Alloc( pm.FI, 1, D_ );
    pm.BFC = (double *)aObj[ o_wo_bfc].Alloc( 1, pm.N, D_ );  // Bugfix 11.03.2010 DK

   pm.dcMod  = (char (*)[6])aObj[ o_wi_dcmod ].Alloc( pm.L, 1, 6 );

    if( pm.L > 0 )
    {
        pm.Y_la = (double *)aObj[ o_wd_yla].Alloc( pm.L, 1, D_ );
        pm.Y_w = (double *)aObj[ o_wd_yw].Alloc( pm.L, 1, D_ );
        pm.Fx = (double *)aObj[ o_wo_fxa].Alloc( pm.L, 1, D_);
        pm.Wx = (double *)aObj[ o_wo_wx].Alloc( pm.L, 1, D_);
        pm.VL = (double *)aObj[ o_w_vl].Alloc( pm.L, 1, D_ );
        pm.Gamma = (double *)aObj[ o_wd_gamma].Alloc( pm.L, 1, D_ );  /*!*/
        pm.lnGmf = (double *)aObj[ o_wi_lngmf].Alloc( pm.L, 1, D_ );  /*!*/
//        pm.D = (double *)aObj[ o_w_d].Alloc( pm.L, 1, D_);
    }
    else
    {
        pm.Y_la  = (double *)aObj[ o_wd_yla ].Free();
        pm.Y_w   = (double *)aObj[ o_wd_yw ].Free();
        pm.Fx    = (double *)aObj[ o_wo_fxa ].Free();
        pm.Wx    = (double *)aObj[ o_wo_wx ].Free();
        pm.VL    = (double *)aObj[ o_w_vl ].Free();
        pm.Gamma = (double *)aObj[ o_wd_gamma ].Free();
        pm.lnGmf = (double *)aObj[ o_wi_lngmf ].Free();
//        pm.D     = (double *)aObj[ o_w_d ].Free();
    }

   // Part 2 - not always required arrays
    if( pm.FIs > 0 && pm.Ls > 0 )
    {
       pm.BF = (double *)aObj[ o_wo_bf].Alloc( pm.FIs, pm.N, D_ );
//      pm.BFC = (double *)aObj[ o_wo_bfc].Alloc( 1, pm.N, D_ );
        pm.XFA = (double *)aObj[ o_ww_xfa].Alloc( pm.FIs, 1, D_ );
        pm.YFA = (double *)aObj[ o_ww_yfa].Alloc( pm.FIs, 1, D_ );
        pm.LsMod = (long int *)aObj[ o_wi_lsmod].Alloc( pm.FIs, 3, L_ );
        pm.LsMdc = (long int *)aObj[ o_wi_lsmdc].Alloc( pm.FIs, 3, L_ );
    // IPx, PMc and DMc may be reallocated after loading arrays LsMod and LsMdc
        pm.IPx = (long int *)aObj[ o_wi_ipxpm ].Alloc(pm.FIs, 1, L_);  // added 07.12.2006 KD
        pm.PMc = (double *)aObj[ o_wi_pmc].Alloc( pm.FIs, 1, D_);
        pm.DMc = (double *)aObj[ o_wi_dmc].Alloc( pm.Ls, 1, D_ );
        pm.MoiSN  = (double *)aObj[ o_wi_moisn ].Alloc( pm.Ls, 1, D_ );
        pm.SitFr  = (double *)aObj[ o_wo_sitfr ].Alloc( pm.Ls, 1, D_ );
        pm.PUL = (double *)aObj[ o_wi_pul].Alloc( pm.FIs, 1, D_);
        pm.PLL = (double *)aObj[ o_wi_pll].Alloc( pm.FIs, 1, D_);
        //     pm.YOF = (double *)aObj[ o_wi_yof].Alloc( pm.FI, 1, D_ );
        pm.sMod = (char (*)[8])aObj[ o_wi_smod].Alloc( pm.FIs, 1, 8 );
        pm.RFLC = (char *)aObj[ o_wi_rflc].Alloc( pm.FIs, 1, A_ );
        pm.RFSC = (char *)aObj[ o_wi_rfsc].Alloc( pm.FIs, 1, A_ );
    }
    else
    {
        pm.BF    = (double *)aObj[ o_wo_bf ].Free();
//        pm.BFC    = (double *)aObj[ o_wo_bfc ].Free();
        pm.XFA   = (double *)aObj[ o_ww_xfa ].Free();
        pm.YFA   = (double *)aObj[ o_ww_yfa ].Free();
        pm.LsMod = (long int *)aObj[ o_wi_lsmod ].Free();
        pm.LsMdc = (long int *)aObj[ o_wi_lsmdc ].Free();
        pm.IPx = (long int *)aObj[ o_wi_ipxpm ].Free();   // added 07.12.2006  KD
        pm.PUL   = (double *)aObj[ o_wi_pul ].Free();
        pm.PLL   = (double *)aObj[ o_wi_pll ].Free();
        //    pm.YOF   = (double *)aObj[ o_wi_yof ].Free();
        pm.PMc   = (double *)aObj[ o_wi_pmc ].Free();
        pm.DMc   = (double *)aObj[ o_wi_dmc ].Free();
        pm.MoiSN  = (double *)aObj[ o_wi_moisn ].Free();
        pm.SitFr  = (double *)aObj[ o_wo_sitfr ].Free();
        pm.sMod  = (char (*)[8])aObj[ o_wi_smod ].Free();
        pm.RFLC  = (char *)aObj[ o_wi_rflc ].Free();
        pm.RFSC  = (char *)aObj[ o_wi_rfsc ].Free();
    }
    if( pm.LO > 1 )
    {
        pm.Y_m = (double *)aObj[ o_wd_ym].Alloc( pm.L, 1, D_ );
        pm.IC_m = (double *)aObj[ o_wd_icm].Alloc( pm.N, 1, D_ );
        pm.IC_lm = (double *)aObj[ o_wd_iclcm].Alloc( pm.N, 1, D_ );
        pm.IC_wm = (double *)aObj[ o_wd_icwm].Alloc( pm.N, 1, D_ );
    }
    else
    {
        pm.IC_m  = (double *)aObj[ o_wd_icm ].Free();
        pm.IC_lm = (double *)aObj[ o_wd_iclcm ].Free();
        pm.IC_wm = (double *)aObj[ o_wd_icwm ].Free();
        pm.Y_m   = (double *)aObj[ o_wd_ym ].Free();
    }

    /* sorption phases */
    if( TSyst::sm->GetSY()->PAalp != S_OFF )
    {
        pm.Aalp = (double *)aObj[ o_wi_aalp].Alloc( pm.FI, 1, D_ );
        pm.Xr0h0 = (double (*)[2])aObj[ o_wi_xr0h0].Alloc( pm.FI, 2, D_ );
    }
    else
    {
        pm.Aalp  = (double *)aObj[ o_wi_aalp ].Free();
        pm.Xr0h0 = (double (*)[2])aObj[ o_wi_xr0h0 ].Free();
    }

   if( TSyst::sm->GetSY()->PSigm != S_OFF )
           pm.Sigw = (double *)aObj[ o_wi_sigw].Alloc( pm.FI, 1, D_ );
    else  pm.Sigw  = (double *)aObj[ o_wi_sigw ].Free();

    if( TSyst::sm->GetSY()->PSigm != S_OFF )
        pm.Sigg = (double *)aObj[ o_wi_sigg].Alloc( pm.FI, 1, D_ );
    else  pm.Sigg  = (double *)aObj[ o_wi_sigg ].Free();

    if( pm.E )
    {
        pm.EZ = (double *)aObj[ o_wi_ez].Alloc( pm.L, 1, D_ );
        pm.Xcond = (double *)aObj[ o_wi_xcond].Alloc( pm.FI, 1, D_ );
        pm.Xeps = (double *)aObj[ o_wi_xeps].Alloc( pm.FI, 1, D_ );
    }
    else
    {
        pm.EZ    = (double *)aObj[ o_wi_ez ].Free();
        pm.Xcond = (double *)aObj[ o_wi_xcond ].Free();
        pm.Xeps  = (double *)aObj[ o_wi_xeps ].Free();
    }

    if( pm.FIat > 0 && pm.FIs > 0 )
    { // ADSORPTION AND ION EXCHANGE
        pm.SATX = (long int (*)[4])aObj[ o_wi_satndx].Alloc( pm.Lads, 4, L_ );
        pm.SCM  = (char (*)[MST])aObj[ o_wi_scm].Alloc( pm.FIs, pm.FIat, A_ );
        //   fillValue( pm.SCM, SC_NOT_USED, pm.FIs*pm.FIat );
        pm.Nfsp = (double (*)[MST])aObj[ o_wi_nfsp].Alloc( pm.FIs, pm.FIat, D_ );
        pm.MASDT = (double (*)[MST])aObj[ o_wi_masdt].Alloc( pm.FIs, pm.FIat, D_ );
        pm.XcapA = (double (*)[MST])aObj[ o_w_xcapa].Alloc( pm.FIs, pm.FIat, D_ );
        pm.XcapB = (double (*)[MST])aObj[ o_w_xcapb].Alloc( pm.FIs, pm.FIat, D_ );
        pm.XcapD = (double (*)[MST])aObj[ o_w_xcapd].Alloc( pm.FIs, pm.FIat, D_ );
        pm.XcapF = (double (*)[MST])aObj[ o_w_xcapf].Alloc( pm.FIs, pm.FIat, D_ );
        pm.XdlA = (double (*)[MST])aObj[ o_wo_xdla].Alloc( pm.FIs, pm.FIat, D_ );
        pm.XdlB = (double (*)[MST])aObj[ o_wo_xdlb].Alloc( pm.FIs, pm.FIat, D_ );
        pm.XdlD = (double (*)[MST])aObj[ o_wo_xdld].Alloc( pm.FIs, pm.FIat, D_ );
        pm.XpsiA = (double (*)[MST])aObj[ o_wo_psia].Alloc( pm.FIs, pm.FIat, D_ );
        pm.XpsiB = (double (*)[MST])aObj[ o_wo_psib].Alloc( pm.FIs, pm.FIat, D_ );
        pm.XpsiD = (double (*)[MST])aObj[ o_wo_psid].Alloc( pm.FIs, pm.FIat, D_ );
        pm.XlamA = (double (*)[MST])aObj[ o_wi_xlama].Alloc( pm.FIs, pm.FIat, D_ );
        pm.Xetaf = (double (*)[MST])aObj[ o_wi_xetaf].Alloc( pm.FIs, pm.FIat, D_ );
        pm.XetaA = (double (*)[MST])aObj[ o_w_xetaa].Alloc( pm.FIs, pm.FIat, D_ );
        pm.XetaB = (double (*)[MST])aObj[ o_w_xetab].Alloc( pm.FIs, pm.FIat, D_ );
        pm.XetaD = (double (*)[MST])aObj[ o_w_xetad].Alloc( pm.FIs, pm.FIat, D_ );
        pm.XFTS = (double (*)[MST])aObj[ o_w_xfts].Alloc( pm.FIs, pm.FIat, D_ );
        pm.SATT = (char *)aObj[ o_wi_satt].Alloc( pm.Lads, 1, A_ );
        pm.MASDJ = (double (*)[DFCN])aObj[ o_wo_masdj].Alloc( pm.Lads, DFCN, D_ );
//        pm.MASDJ = (double *)aObj[ o_wo_masdj].Alloc( pm.Ls, 1, D_ );
        pm.D = (double (*)[MST])aObj[ o_w_d].Alloc( MST, MST, D_);
        pm.lnSAC = (double (*)[4])aObj[ o_wo_lnsat].Alloc( pm.Lads, 4, D_ );
        pm.SM3 = (char (*)[MAXDCNAME])aObj[ o_wd_sm3].Alloc( pm.Lads, 1, MAXDCNAME );
        pm.DCC3 = (char *)aObj[ o_wi_dcc3].Alloc( pm.Lads, 1, A_ );
    }
    else
    {
        pm.SATX =(long int (*)[4])aObj[ o_wi_satndx ].Free();
        pm.SCM   = (char (*)[MST])aObj[ o_wi_scm ].Free();
        pm.Nfsp  = (double (*)[MST])aObj[ o_wi_nfsp ].Free();
        pm.MASDT = (double (*)[MST])aObj[ o_wi_masdt ].Free();
        pm.XcapA = (double (*)[MST])aObj[ o_w_xcapa ].Free();
        pm.XcapB = (double (*)[MST])aObj[ o_w_xcapb ].Free();
        pm.XcapD = (double (*)[MST])aObj[ o_w_xcapd ].Free();
        pm.XcapF = (double (*)[MST])aObj[ o_w_xcapf ].Free();
        pm.XdlA  = (double (*)[MST])aObj[ o_wo_xdla ].Free();
        pm.XdlB  = (double (*)[MST])aObj[ o_wo_xdlb ].Free();
        pm.XdlD  = (double (*)[MST])aObj[ o_wo_xdld ].Free();
        pm.XpsiA = (double (*)[MST])aObj[ o_wo_psia ].Free();
        pm.XpsiB = (double (*)[MST])aObj[ o_wo_psib ].Free();
        pm.XpsiD = (double (*)[MST])aObj[ o_wo_psid ].Free();
        pm.XlamA = (double (*)[MST])aObj[ o_wi_xlama ].Free();
        pm.Xetaf = (double (*)[MST])aObj[ o_wi_xetaf ].Free();
        pm.XFTS  = (double (*)[MST])aObj[ o_w_xfts ].Free();
        pm.XetaA = (double (*)[MST])aObj[ o_w_xetaa ].Free();
        pm.XetaB = (double (*)[MST])aObj[ o_w_xetab ].Free();
        pm.XetaD = (double (*)[MST])aObj[ o_w_xetad ].Free();  // added 12.09.05  KD
        pm.SATT  = (char *)aObj[ o_wi_satt ].Free();
        pm.MASDJ = (double (*)[DFCN])aObj[ o_wo_masdj ].Free();
        pm.lnSAC = (double (*)[4])aObj[ o_wo_lnsat ].Free();
        pm.D = (double (*)[MST])aObj[ o_w_d ].Free();
        pm.SM3 = (char (*)[MAXDCNAME])aObj[ o_wd_sm3 ].Free();
        pm.DCC3 = (char *)aObj[ o_wi_dcc3 ].Free();
    }

    if( pm.PG > 0 )
    {
        pm.Fug = (double *)aObj[ o_wd_fug].Alloc( pm.PG, 1, D_ );
        pm.Fug_l = (double *)aObj[ o_wd_fugl].Alloc( pm.PG, 1, D_ );
        pm.Ppg_l = (double *)aObj[ o_wd_ppgl].Alloc( pm.PG, 1, D_ );
    }
    else
    {
        pm.Fug   = (double *)aObj[ o_wd_fug ].Free();
        pm.Fug_l = (double *)aObj[ o_wd_fugl ].Free();
        pm.Ppg_l = (double *)aObj[ o_wd_ppgl ].Free();
    }

    // Part 3

    if( pm.Ls > 1 && pm.FIs > 0 )
    {
        pm.Wb = (double *)aObj[ o_wio_wb].Alloc( pm.Ls, 1, D_ );
        pm.Wabs = (double *)aObj[ o_wio_wabs].Alloc( pm.Ls, 1, D_ );
        pm.Rion = (double *)aObj[ o_wi_rion].Alloc( pm.Ls, 1, D_ );
        pm.Qp = (double *)aObj[ o_wi_qp].Alloc( pm.FIs, QPSIZE, D_ );
        pm.Qd = (double *)aObj[ o_wi_qd].Alloc( pm.FIs, QDSIZE, D_ );
    }
    else
    {
        pm.Wb    = (double *)aObj[ o_wio_wb ].Free();
        pm.Wabs  = (double *)aObj[ o_wio_wabs ].Free();
        pm.Rion  = (double *)aObj[ o_wi_rion ].Free();
        pm.Qp    = (double *)aObj[ o_wi_qp ].Free();
        pm.Qd    = (double *)aObj[ o_wi_qd ].Free();
    }

// SD 14/07/2009
    pm.Cp0   = (double *)aObj[ o_wio_cp0 ].Alloc( pm.L, 1, D_ );
    pm.H0    = (double *)aObj[ o_wio_h0 ].Alloc( pm.L, 1, D_ );
    pm.U0    = (double *)aObj[ o_wio_u0 ].Alloc( pm.L, 1, D_ );
    pm.S0    = (double *)aObj[ o_wio_s0 ].Alloc( pm.L, 1, D_ );
    pm.A0    = (double *)aObj[ o_wio_a0 ].Alloc( pm.L, 1, D_ );
        
    // 11/02/2009
        pm.VPh = (double (*)[MIXPHPROPS])aObj[ o_wo_vph].Alloc(pm.FIs,MIXPHPROPS, D_);
        pm.GPh = (double (*)[MIXPHPROPS])aObj[ o_wo_gph].Alloc(pm.FIs,MIXPHPROPS, D_);
        pm.HPh = (double (*)[MIXPHPROPS])aObj[ o_wo_hph].Alloc(pm.FIs,MIXPHPROPS, D_);
        pm.SPh = (double (*)[MIXPHPROPS])aObj[ o_wo_sph].Alloc(pm.FIs,MIXPHPROPS, D_);
        pm.CPh = (double (*)[MIXPHPROPS])aObj[ o_wo_cph].Alloc(pm.FIs,MIXPHPROPS, D_);
        pm.APh = (double (*)[MIXPHPROPS])aObj[ o_wo_aph].Alloc(pm.FIs,MIXPHPROPS, D_);
        pm.UPh = (double (*)[MIXPHPROPS])aObj[ o_wo_uph].Alloc(pm.FIs,MIXPHPROPS, D_);

        // New phase stuff 06/06/12
        pm.LsMdc2  = (long int *)aObj[ o_wi_lsmdc2].Alloc( pm.FIs, 3, L_ );
        pm.LsPhl   = (long int *)aObj[ o_wi_lsphl].Alloc( pm.FI, 2, L_ );
        //pm.PhLin   = (long int *)aObj[ o_wi_phlin].Alloc( pm.FI*2, 1, L_ );
        // TSolMod stuff
        //pm.lPhc   = (double *)aObj[ o_wi_lphc].Alloc( pm.FI*2, 1, D_ );
        //pm.DQFc   = (double *)aObj[ o_wi_dqfc].Alloc( pm.FIs, 1, D_ );
        //pm.rcpc   = (double *)aObj[ o_wi_rcpc].Alloc( pm.FIs, 1, D_ );
        pm.lnDQFt   = (double *)aObj[ o_wi_lndqft].Alloc( pm.Ls, 1, D_ );
        pm.lnRcpt   = (double *)aObj[ o_wi_lnrcpt].Alloc( pm.Ls, 1, D_ );
        pm.lnEXt   = (double *)aObj[ o_wi_lnext].Alloc( pm.Ls, 1, D_ );
        pm.lnCnft   = (double *)aObj[ o_wi_lncnft].Alloc( pm.Ls, 1, D_ );
        //TSorpMod & TKinMet stuff
        pm.SorMc   = (double *)aObj[ o_wi_sormc].Alloc( pm.FIs, 16, D_ );
        // TSorpMod stuff
        pm.LsESmo   = (long int *)aObj[ o_wi_lsesmo].Alloc( pm.FIs, 4, L_ );
        pm.LsISmo   = (long int *)aObj[ o_wi_lsismo].Alloc( pm.FIs, 4, L_ );
        //pm.xSMd   = (long int *)aObj[ o_wi_xsmd].Alloc( pm.FIs, 1, D_ );
        //pm.EImc   = (double *)aObj[ o_wi_eimc].Alloc( pm.FIs, 1, D_ );
        //pm.mCDc   = (double *)aObj[ o_wi_mcdc].Alloc( pm.FIs, 1, D_ );
        //pm.IsoPc   = (double *)aObj[ o_wi_isopc].Alloc( pm.Fs, 1, D_ );
        //pm.IsoSc   = (double *)aObj[ o_wi_isosc].Alloc( pm.Fs, 1, D_ );
        pm.lnScalT   = (double *)aObj[ o_wi_lnscal].Alloc( pm.Ls, 1, D_ );
        pm.lnSACT   = (double *)aObj[ o_wi_lnsact].Alloc( pm.Ls, 1, D_ );
        pm.lnGammF   = (double *)aObj[ o_wi_lngamf].Alloc( pm.Ls, 1, D_ );
        pm.CTerms   = (double *)aObj[ o_wi_cterms].Alloc( pm.Ls, 1, D_ );
        //pm.IsoCt   = (char *)aObj[ o_wi_isoct].Alloc( pm.FIs, 1, A_ );
        // TKinMet stuff
        pm.LsKin   = (long int *)aObj[ o_wi_lskin].Alloc( pm.FI, 4, L_ );
        pm.LsUpt   = (long int *)aObj[ o_wi_lsupt].Alloc( pm.FIs, 2, L_ );
        //pm.jCrDC   = (long int *)aObj[ o_wi_jcrdc].Alloc( pm.FI, 1, L_ );
        //pm.xfaces   = (long int *)aObj[ o_wi_xfaces].Alloc( pm.FI, 1, L_ );
        //pm.fSak   = (double *)aObj[ o_wi_fsac].Alloc( pm.FI, 1, D_ );
        //pm.Krpc   = (double *)aObj[ o_wi_krpc].Alloc( pm.FI, 1, D_ );
        //pm.UMpc   = (double *)aObj[ o_wi_umpc].Alloc( pm.FIs, 1, D_ );
        pm.kMod   = (char(*)[4])aObj[ o_wi_kmod].Alloc( pm.FI, 1, 4 );

    Free_TSolMod(); //Bugfix SD 01/06/12
    Alloc_TSolMod( pm.FIs );

}

//--------------------- End of ms_multi.cpp ---------------------------
