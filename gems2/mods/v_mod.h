//-------------------------------------------------------------------
// $Id$
//
// Declaration of internak constants: objnumbers, sizes, codes ....
//
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

#ifndef _v_mod_h_
#define _v_mod_h_

#include "v_vals.h"

const unsigned int
 MAXMUNAME =      10,
   MAXSYSNAME =    12,
     MAXTIME =        5,
       MAXSYMB =        4,
          MAXTDPCODE =     3,
            MAXFLTYPE =      6,
              MAXLEVELS =      8,
                pplm    =        32,
                  TDBVERSION =     64,
                    MAXRKEYLEN =     80,
                      V_SD_RKLEN = 32,
                         V_SD_VALEN = 24;

const char S_OFF = '-',
                   S_ON = '+',
                          S_REM = '*',
                                  A_NUL ='?';

enum { RT_SDATA=0, RT_CONST=1 };
/*
enum { RT_GEM2MT=2, RT_PARAM=3, RT_ICOMP=4, RT_DCOMP=5, RT_COMPOS=6,
       RT_REACDC=7, RT_RTPARM=8, RT_PHASE=9, RT_SYSEQ=10, RT_PROCES=11,
       RT_UNSPACE=12, RT_GTDEMO=13, RT_DUALTH=14,

       MD_RMULTS=15, MD_MTPARM=16, MD_SYSTEM=17,
       MD_MULTI=18, MD_EQCALC=19, MD_EQDEMO=20
     };
*/
enum { RT_PARAM=2, RT_ICOMP=3, RT_DCOMP=4, RT_COMPOS=5,
       RT_REACDC=6, RT_RTPARM=7, RT_PHASE=8, RT_SYSEQ=9, RT_PROCES=10,
       RT_UNSPACE=11, RT_GTDEMO=12, RT_DUALTH=13, RT_GEM2MT=14,

       MD_RMULTS=15, MD_MTPARM=16, MD_SYSTEM=17,
       MD_MULTI=18, MD_EQCALC=19, MD_EQDEMO=20
     };

//#define IsFloatEmpty( v ) ( v>0. && v <= FLOAT_EMPTY ? 1:0 )
//#define IsDoubleEmpty( v ) ( v>0. && v <= DOUBLE_EMPTY ? 1:0 )
//extern const float FLOAT_EMPTY;
//extern const double DOUBLE_EMPTY;
inline bool IsFloatEmpty( const float v )
{
    return ( v>0. && v <= FLOAT_EMPTY);
}
inline bool IsDoubleEmpty( const double v )
{
    return ( v>0. && v <= DOUBLE_EMPTY);
}


enum std_object{
    o_hpage = 0, o_hrkey, o_hemsg, o_hedbuf, o_hrkeys,
    o_constlab, o_constdim, o_const, o_sdnref, o_sdauth, o_sdyear,
    o_sdclass,  o_sdauthr, o_sdtitle, o_sdedit,   o_sdvoly,
    o_sdpage, o_sdabstr, o_sdrefs, o_sdnote, o_sdbkwd,
    o_reckey, o_k_, o_ii_, o_prn_, o_rtkey, o_rttime,
    // SD 0ct 2005
    o_lms_jp, o_lms_kp, o_lms_itx, o_lms_delta, o_lms_yexp,
    o_lms_yfit, o_lms_tx, o_lms_para, o_lms_wexp,
    o_lms_paf
};

//---------------------------------
// Param
//---------------------------------

const int
MAXPARAMKEY =    24,
       MNST =   6,
              MST =   6, // number of surface types
                     MCAS  = 6,  // number of columns in SATC table
                            DFCN = 6; // number of columns in MASDJ table

const int
	MPP_TOT = 0,       // index of column with total mixed phase property
	MPP_STD = 1,       // index of column with standard property sum for mixed phases
	MPP_RES = 2,       // index of column with residual property sum for mixed phases
	MPP_ID = 3,        // index of column with ideal mixing property for the phases
	MPP_EX = 4,        // index of column with excess mixing property for the phases
	MIXPHPROPS = 5;    // Number of columns in the property table for mixed phases

enum param_objects {                  // work objects
    o_paver = o_lms_paf+1, //o_rttime+1,
    // BASE_PARAM
    o_papc, o_paprd,  /*i 3*/  o_padpwt, /*i 3*/
    o_papllg, o_pape, o_paiim, o_padg, o_padhb, o_pads, o_padk,
    o_padf,   /*d 2*/  o_padfy,  /*d 7*/  o_padb,  o_paag,  o_padgc,
    o_pagan,  /*d 3*/  o_padns,  o_paxmin, /*d 5*/  o_paeps,  /*d 2*/
    o_padkin, o_patprn, /*txt*/
    // default information
    o_padcpct, o_padcpdc,  o_pabcpc, o_parepct,   o_parepdc, o_parepvc,
    o_parppdc, o_parppvc, o_paphsol_t,  o_paphpvc,  o_pamupmv, o_patppdc,
    o_patppd2, o_patppvc, o_pasyppc,    o_pasypvc, o_pautppc,  o_papepsc,
    o_papepvc, o_pagdcode, o_pagdpsc,  o_pagdpcc,  o_pagdptc, o_pagdpgw,
    o_pasdref,
    o_parpnptv, /*i 3*/  o_parpmode,   o_partpi, o_partti, o_partvi,
    o_padrprtr,/*f 2*/ o_papmnum, /*f 3*/
    // RMULTS
    o_muphc, o_mudcc, /*o_mudcs,*/ o_muicc,  o_mudcf,
    o_muicf, o_muidcref, o_muval, o_mupl, o_mubc,
    o_muas, o_muav, o_musf2, o_musm2,
o_musm3, o_mudcc3,
o_nlicv, o_nlich, o_nldcv, o_nldch,
o_nldcvs, o_nldchs, o_nlphv, o_nlphh, // new !!!
    // MTPARM
    o_tpunits, o_tpsv, o_tpptv, o_tp_T,  o_tp_P,
    o_tp_RT,  o_tprow,  o_tpepsw, o_tpvisw, o_tp_L,
    o_tpmark,  o_tpg,   o_tpdevg,  o_tph,  o_tpdevh,
    o_tps,    o_tpdevs,  o_tpcp,  o_tpdevcp,  o_tpvm,
    o_tpdevv,  o_tpf,   o_tpu,   o_tpcv,  o_tpalp,
    o_tpbet,   o_tpwbor, o_tpwrad, o_tpfug,  o_tpdvg,
    // SYSTEM built-in
    o_syphst, o_syname, o_synotes, o_sypmod, o_sypvec,
    o_syn,   o_syl, o_syfi, o_sylbc,  o_sylns,
    o_sypmm,  o_sytmm,  o_syvmm,  o_syhmm,  o_symass,
    o_syvol,  o_syproc, o_syl1,  o_syrlc,  o_syrsc,
    o_syrflc, o_syrfsc, o_syicl,  o_sydcl,  o_sypcl,
    o_syacl,  o_sybiun,  o_syxdun, o_syxaun, o_syxpun,
    o_syb,    o_sydelb,  o_sybi,   o_syxea,  o_syxed,
    o_syphm,  o_sysatc,  o_sypparc, o_sydul, o_sydll,
    o_sygex,  o_sypul,   o_sypll,  o_syyof,  o_sylngmf,
    o_syaalp, o_symasdj, o_sysigm, o_syxr0h0, o_syxeps,
    o_synfsp, o_symasdt, o_syxcapa, o_syxcapb, o_syxfiec,
    o_syxcapf, o_syxlam, o_syscmt,  o_syguns, o_syvuns,
    // MULTI
    o_wi_pun,  o_wi_nnr, o_wi_ldim, o_wi_fdim, o_wo_fdim,
    o_wo_it, o_wi_mode, o_wo_ipar, o_wio_tc, o_wio_t,
    o_wio_p,  o_wo_vx, o_wo_gx, o_wo_ax, o_wo_ux,
    o_wo_hx,   o_wo_sx, o_wo_cpx, o_wo_cvx,   o_wi_t0,
    o_wi_ve, o_wi_mbx, o_wo_fx, o_wo_ic,  o_wd_ph,
    o_wd_pe, o_wd_eh, o_wi_mol,  o_wi_gwat,o_wi_ymet,
    o_wio_denw, o_wio_epsw, o_wo_psi, o_wo_lnp, o_wo_rt,
    o_wo_frt,  o_wo_yw,  o_wio_cons,  o_wio_fitv, o_wd_sitn,
    o_wi_l1,        o_wi_ipxpm, // added 07.12.2006
    o_wi_lsmod,  o_wi_lsmdc,  o_wi_mui, o_wi_muk, o_wi_muj,
    o_wi_satndx, o_wi_dul,  o_wi_dll,  o_wi_gex,  o_wi_pul,
    o_wi_pll, o_wi_yof,  o_wi_pmc, o_wi_dmc,  o_wio_vol,
    o_w_hym, o_w_vl,  o_wi_mm,  o_wio_h0,  o_wio_a0,
    o_wio_u0, o_wio_s0,  o_wio_cp0,  o_wio_cv0,  o_wi_pparc,
    o_wd_ym,  o_wd_yla,  o_wd_yw,  o_wd_gamma,  o_wi_lngmf,
    o_wio_lngmm, o_wi_ez,  o_wio_wb,  o_wio_wabs,  o_wi_rion,
    o_wi_aalp, o_wi_sigw,  o_wi_sigg,  o_wi_nfsp,  o_wi_masdt,
    o_wd_fvol,  o_wd_fwgt,   o_w_xcapa,  o_w_xcapb,  o_w_xcapd, o_w_xcapf,
    o_wo_xdla, o_wo_xdlb,  o_wo_xdld,  o_wo_psia,  o_wo_psib,
    o_wo_psid,  o_wi_xr0h0,  o_wi_xlama,  o_wi_xetaf,  o_wi_xcond,
    o_wi_xeps, o_wi_awt , o_wi_a,  o_wd_xfs,  o_wd_falps,
    o_wd_fug,  o_wd_fugl,  o_wd_ppgl,  o_wo_masdj,  o_wo_g,
    o_wi_g0,  o_wo_lngam,  o_w_lngmo,  o_wo_lnsat,  o_wi_b,
    o_wo_u,   o_wd_uc,  o_wd_ur, o_wo_c,  o_wd_icm,  o_wd_iclcm ,
    o_wd_icwm,  o_wo_bf, o_wo_bfc, o_w_xf ,  o_wo_yf,  o_ww_xfa,
    o_ww_yfa,  o_wo_falp,   o_w_xetaa,  o_w_xetab, o_w_xetad, o_w_xfts,
    o_w_x,  o_wo_y, o_w_xy ,  o_w_xu ,o_wi_qp, o_wi_qd,
    o_wo_mu,  o_w_emu,  o_w_nmu,  o_w_w,  o_wo_fxa,
    o_wo_wx,  o_wo_f,  o_w_f0,  o_w_d,  o_wi_smod,
    o_wd_sb,  o_wd_sm,  o_wd_sf,  o_wd_sfs,
    o_wd_sm2,
o_wd_sm3,  //  new
    o_wd_sf2, o_wd_txtb,
    o_wi_rlc,  o_wi_rsc,  o_wi_rflc,  o_wi_rfsc,  o_wi_icc,
    o_wi_dcc,
o_wi_dcc3, // new
    o_wi_phc,  o_wi_scm,  o_wi_satt,  o_wi_dccw,
    o_w_r, o_w_r1,
o_wo_it_efd,  o_wo_it_ipm,    //   o_wd_sitxcat, o_wd_sitxan,
    o_wd_site, o_w_sbh, o_w_tprn,
    // GammaCalc() - adjustable DOD for calculation of activity coefficients
    o_neqtxt,  o_nsmod,  o_ntc,  o_ntk,  o_np ,
    o_nis, o_ni,  o_njs,  o_nnext, o_nncp ,
    o_nncd,  o_ndc,  o_nbmol, o_nrt, o_nlnrt,
    o_nez,  o_npcv,  o_ndcm,  o_nmvol,  o_nppar,
    o_ngtn,  o_ngam,  o_nlngam,  o_nu,  o_nas,
    o_nxa,  o_nxaa,  o_nxast,  o_nxcec, o_nxx,
    o_nwx,   o_nmju,  o_nqp, o_nqd,  o_nwtext,
    // BASE  (in project record )
    o_spppar,   // Params
    o_mupmv, o_muname, o_munotes, o_mudim, o_munfl, // Rmults
    o_mul1,  o_musf, o_musm,  o_musa,  o_musb,  o_mufn, o_mudcs,
    o_tpstr,                                        // MTPARM
    o_sppconst, o_sppdatac
};

//---------------------------------
// IComp
//---------------------------------

const unsigned int
MAXICNAME =      6,
                 MAXICGROUP =    22,
                                 MAXFORMULA =     81,
                                                  IC_RKLEN = 32,
                                                             DC_RKLEN = 32;

enum icomp_objects {                  // work objects
    //  o_icsymb= o_sppdatac+1, o_ictype, o_icgrp,
    o_icawt= o_sppdatac+1, o_icass, o_icacp,
    o_icavs,  o_icarad, o_icidis, o_icval, o_icnum, o_iccoor,
    // record
    o_icsst, o_icname, o_icform,  o_icdc,  o_icfloat, o_icint, o_ictprn
};

typedef enum {  /* classes of IC*/
    IC_ELEMENT  =  'e',  // chemical element (except oxygen and hydrogen)
    IC_OXYGEN   =  'o',  // oxygen
    IC_HYDROGEN =  'h',  // hydrogen (natural mixture of isotopes) H
    IC_PROTIUM   = 'p',  // protium (reserved) Hp
    IC_DEYTERIUM = 'd',  // deuterium (reserved) D
    IC_TRITIUM  =  't',  // tritium (reserved) T
    IC_FORMULA  =  'f',  // formula unit (eg. for Sio - a symbol of SiO2)
    IC_METALION =  'm',  // metal ion (cation), reserved
    IC_LIGAND   =  'l',  // ligand (anion), reserved
    IC_ADDIT    =  'a',  // IC with unknown stoichiometry (eg: Hum, humic acid)
    IC_ISOTOPE  =  'i',  // isotope of chemical element from 1 to 250
    IC_OXYGEN16 =  'q',  // q  - oxygen 16O (reserved)
    IC_OXYGEN18 =  'r',  //  r  - oxygen 18O (reserved)
    IC_CHARGE   =  'z',  // z  - electrical charge
    IC_VOLUME   =  'v'   // volume
} IC_CLASSES;


//---------------------------------
// DComp
//---------------------------------

const int
MAXDRGROUP =     8,
                 MAXDCNAME =      16,
                                  MAXRMTM =        9;

enum dcomp_objects {                  // work objects
    // o_dcpst = o_ictprn+1, o_dcps, o_dcn, o_dcatc
    o_dcpct = o_ictprn+1, o_dcpdc,  o_dcrmtm, o_dczz, o_dcmwt,
    o_dcmvs,    o_dcgs,    o_dchs,      o_dcss,  o_dccps,
    o_dcptst,  o_dcstlam, o_dcbeal,  o_dcder,
    // Record 18 fields
    o_dcstr,   /* ! */  o_dcname,  o_dcform,  o_dcdim,   /*i 4 */
    o_dcfloat1, /*f 4 */  o_dcdbl1, /*d 4 */ o_dcfloat2, /*f 12 */
    o_dccpint, /*f dyn */  o_dccp,  /*f dyn */ o_dccpfs,  /*f dyn */
    o_dchkf,   /*f dyn */  o_dcvt,  /*f dyn */ o_dccritpg,/*f dyn */
    o_dcodc,   /*f dyn */  o_dcftp, /*f dyn */  o_dcftpb,  /*f dyn */
    o_dccemp,  /*f dyn */  o_dcfttyp, /*s dyn */ o_dcsdref, /*s dyn */
    o_dcsdval, /*s dyn */  o_dctprn, /*txt*/
};

enum DC_CNTRL_CODES {
    // General codes for methods of calculation of DC thermodynamic parameters
    CTPM_CPT = 'C',  /* on the basis of empirical heat capacity equation Cp=f(T),
                        used for DComp and ReacDC definitions (solid, liquid, gaseous
                        substances and aqueous species) */
    CTPM_HKF = 'H',  /* on the basis of Helgeson-Kirkham-Flowers equation of state
                        (HKF EOS), used for aqueous species (DCOMP) or for reactions
                         bwtween them (ReacDC) */
    CTPM_REA = 'K',  /* calculation of logK of reactions as they depend functionally
                        on T and P, used only for ReacDC definitions of all phase
                        types of substances. */
    CTPM_EOS = 'S',  /* calculations via general equations of state (reserved) */
    CTPM_ISO = 'I',  /* calculation of parameters for isotopic forms of compounds
                        from fractionation constants and their dependency on T
                        (after Grichuk,1988) - ReacDC only */
    CTPM_SOR = 'X',  /* calculations via adsorption or ion exchange constants, using
                        LFER correlations (Sverjensky 1993,1994) - ReacDC only (reserved). */

    //Codes for temperature corrections of thermodynamic properties
    CTM_CST = 'S',   /* calculation of ordinary integral using 11-term Cp=f(T) equations
                        (up to 3 phase transitions or up to 3 Cp=f(T) equations for DComp) */
    CTM_CHP = 'H',   /* the same with modifications by Holland and Powell (1998) (minerals); */
    CTM_BER = 'B',   /* the same with modifications by Berman (1988) (minerals) */
    CTM_FEI = 'F',   /* Cp=f(T) equation by Fei and Saxena (1986) (reserved) */
    CTM_LGX = 'X',   /* REACDC: calculation of reaction properties from empirical
                        function lgK=f(T) in 7-term form obtained from 5-term Cp(T) */
    CTM_HKF = 'K',   /* calculation with HKF EOS (for aqueous species) as in SUPCRT92 */
    CTM_WAT = 'W',   /* calculation of H2O water(steam) properties from HGK and LVS
                        equations of state (also using subroutines of SUPCRT92) */
    CTM_WAS = 'V',   /* calculation of H2O water (steam) properties from IAPWS-95
                        equation of state (reserved) */
    CTM_LGK = 'L',   /* calculation from empirical function lgK=f(T) in 7-term form
                       (Nordstrom and Munoz, 1988) - ReacDC only */
    CTM_EK0 = '0',   /* one-term extrapolation at dHr = 0 and logK = const added 15.07.03 */
    CTM_EK1 = '1',   /* one-term extrapolation (Gu et al.,1994), dGr(T)= const */
    CTM_EK2 = '2',   /* two-term extrapolation Vant Hoff) - ReacDC only */
    CTM_EK3 = '3',   /* three-term extrapolation (dCpr = const) - ReacDC only */
    CTM_IKZ = 'Z',   /* calculation using Lagrange polynomial interpolation over the
                        array of lgK for several fixed T (P) points (ReacDC only). */
    CTM_DKR = 'R',   /* calculation of lgK of dissociation reactions at elevated TP
                        from density equation (Marshall and Franck, 1978) - ReacDC only*/
    CTM_PPE = 'E',   /* Prediction of standard state properties of aqueous hydroxide complexes
                        (dissociation reactions) using Pronsprep-OH algorithm (Shock et al. 1997)
                        added 28.03.2008 */
    CTM_MRB = 'Y',   /* Calculation of deltaR with modified Ryzhenko-Bryzgalin
                        model (added by TW and DK on 03.08.2007 */
    CTM_CPG = 'C',   /* reserved (FGL) */
    CTM_IFG = 'G',   /* calculation of G0 for isotopic forms from equations (Grichuk,
                      1 988) - ReacDC only */
    CTM_KAS = 'K',   /* reserved */
    CTM_DAS = 'D',   /* reserved */

    // Codes for pressure corrections of thermodynamic properties
    CPM_OFF  = 'N',  /* no account for pressure dependence */
    CPM_NUL  = '0',  /* account for pressure dependence is already specified as
                        inherent to a method */
    CPM_ONE  = '1',  /* V0 */
    CPM_TWO  = '2',
    CPM_TRI  = '3',
    CPM_CON  = 'C',  /* molar volume of DC or dVr are assumed independent of P and T */
    CPM_VKE  = 'K',  /* molar volume is calculated from empirical equation V=f(T,P)
                        (modification by Dorogokupets et.al. 1988) */
    CPM_VBE  = 'V',  /* calculation via V=f(T,P) as modified by (Berman,1988) */
    CPM_VBM  = 'B',  /* alloc. array; calculate Birch-Murnaghan (1947) V(T,P) */
    CPM_CEH  = 'E',  /* DC molar volume V(T,P) is calculated from isothermal compres-
                        sibility and isobaric expansibility (Holland and Powell, 1998) */
    CPM_GAS  = 'G',  /* calculation of fugacities of gases from their critical para-
                        meters and corresponding state theory (reserved) */
    CPM_PRSV = 'R',  /* Peng-Robinson-Stryjek-Vera EOS for gases and fluids (nonelectrolytes)
                        (added by T.Wagner in July 2006) */
    CPM_EMP  = 'S',  /* calculation from Churakov-Gottschalk (2003) EOS for gases
                        and fluids (nonelectrolytes) */
    CPM_SRK  = 'T',  /* Soave-Redlich-Kwong EOS for gases and fluids (nonelectrolytes) */

    CPM_HKF  = 'F',  /* account for pressure up to 5 kbar by HKF (Tanger,Helgeson,1988) */
    CPM_AKI  = 'A',  /* calculation of partial molal volumes for aqueous nonelectrolyte species
                        using EOS (Akinfiev and Diamond, 2003) */
    CPM_PCR  = 'P',  /* PARCOR estimation of HKF EoS parameters 19.05.98 */
    CPM_INK  = 'Z'   /* calculation using Lagrange polinomial interpolation over
                        the array  of lgK(T,P) */

};

enum DC_phase_state {   // Code of phase state identification of DC record keys
    CP_AQU   = 'a',  // aqueous electrolyte
    CP_GAS   = 'g',  // gaseous species
    CP_GASI  = 'p',  // plasma species
    CP_FLUID = 'f',  // component of fluid
    CP_SOLID = 's',  // components of condensed solid phases, incl. sorption carriers
    CP_LIQID = 'l',  // components of condensed liquid non-electrolyte phases
    CP_MELT  = 'm',  // non-aqueous electrolyte species (eq., in magmatic melt)
    CP_SSPC  = 'c',  // surface complex (adsorbate)
    CP_SORB  = 'x',  // exchangeable ion
    CP_CPEL = 'y',   // complex on poly- (oligo)electrolyte (reserved)
    CP_MACR = 'q',   // carrier (macromolecule) of poly- (oligo)electrolyte,
    // or constant charge carrier of clay mineral or ion exchange resin
    CP_HCARB = 'h',  // components of mixture of liquid hydrocarbons
    CP_UNIV  = 'u'   // component of condensed phase with I type phase transitions
};

//---------------------------------
// TPWORK
//---------------------------------

enum tpwork_objects {   /* TPWORK */
    o_twdrkey = o_dctprn+1, o_twunits, o_twpp,  o_twppr,
    o_twtc, o_twtcr, o_twtk, o_twtkr,
    o_tdsr, o_tws, o_tdevs, o_tdgr,
    o_twg, o_tdevg, o_twk, o_tlogk,
    o_tdlogK, o_tdhr, o_twh, o_tdevh,
    o_tdcpr, o_twcp, o_tdevcp, o_tdvr,
    o_twv, o_tdevv, o_twalpbet/*2*/, o_twfugv/*2*/,
    o_twRT, o_twrow, o_twew, o_tww,
};

//---------------------------------
// Compos
//---------------------------------

const   int
BC_RKLEN = 48,
           MAXCMPNAME =     16,
                            MAXCMPGROUP =    28,
                                             MAXFORMUNIT=     20;

enum compos_objects {   // COMPOS
    // o_bcnam  =  o_twBorn+1, o_bctype, o_bcgroup,
    o_bcpcc=  o_tww+1,  /*6*/  o_bcname,  o_bcnotes,  o_bcdim,  /*4*/
    o_bcmvol, /*8*/  o_bccv,   /*d dyn*/   o_bccfor, /* txt */
    o_bcsb,   /*s dyn */  o_bcsa,   /*s dyn */  o_bcsmk,  /*s dyn */
    o_bccicl, /*a dyn */  o_bccdcl, /*a dyn */  o_bccacl, /*a dyn */
    o_bcdcs,/*a dyn*/ o_bcci,/*f dyn*/ o_bccd,/*f dyn*/ o_bcca,/*f dyn*/
    o_bcdelc, /*added 3/03/97*/  o_bcsdref, /*s dyn */  o_bcsdval, /*s dyn */
    /* Work var & arrays */
    o_bcsb1, /*s dyn */  o_bcicw, /*f dyn */ o_bctprn, /*txt*/
};

typedef enum { // Units of measurement of quontities and concentrations
    /* number of components and phases */
    QUAN_MKMOL = 'Y',  QUAN_MMOL = 'h',  QUAN_MOL = 'M',  // NUMBER OF MOLES
    QUAN_MGRAM = 'y',  QUAN_GRAM = 'g',  QUAN_KILO = 'G', // MASS
    /* concentrations of components and phases*/
    CON_MOLFR = 'n', CON_MOLPROC = 'N', CON_pMOLFR = 'f', // MOLE FRACTION
    CON_VOLFR = 'v', CON_VOLPROC = 'V', CON_pVOLFR = 'u', // VOLUME FRACTION
    CON_MOLAL = 'm', CON_MMOLAL =  'i', CON_pMOLAL = 'p', // MOLALITY
    CON_MOLAR = 'L', CON_MMOLAR =  'j', CON_pMOLAR = 'q', // MOLARITY
    CON_WTFR  = 'w', CON_WTPROC =  '%', CON_PPM =    'P', // MASS FRACTION
    CON_AQWFR = 'C', CON_AQWPROC = 'c', CON_AQPPM =  'a', // CONCENTRATION
    // aqueous species
    CON_AQGPL = 'd', CON_AQMGPL = 'e', CON_AQMKGPL = 'b',//VOLUME CONCENTRATION

    //Units of measurement of pressure Pr, P  { b B p P A }'
    PVT_BAR =  'b', // bar - default, 1 bar = 1e5 Pa
    PVT_KBAR = 'B', // kbar, 1 kbar = 1000 bar
    PVT_PASC = 'p', // Pascal (Pa)
    PVT_KPASC = 'P',// MPa, 1 MPa = 10 bar = 1e6 Pa
    PVT_ATM =  'A', // atm, 1 atm = 1.013 bar
    //Attention: Only b code can be used in this version!

    //Units of measurement of molar volume  { c j a L m }'
    PVT_CM3 =  'c', /*cm3, cm3/mole*/
    PVT_LITR =  'L', // liters (L) - volume of the system only, 1 L = 1000 cm3
    PVT_JBAR =  'j', // J/bar, 10 cm3/mole = 1 J/bar
    PVT_CBAR = 'a',  // (cal/bar), 41.84 cm3/mole = 1 cal/bar
    // m  - reserved.
    //Attention: only j code can be used in this version!

    //Units of measurement of reference temperature Tr { C K F }'
    PVT_CELS = 'C', /*degrees Celsius (C)*/
    PVT_KELVIN = 'K', /*Kelvins (K), 0 C = 273.15 K*/
    PVT_FAREN = 'F',  /*degrees Fahrenheit (F)*/
    //Attention: Only C code can be used in this version.

    // Units of measurement of energy values { j c J C n N }
    TDAT_JOUL = 'j',/* Joules (J/mole)*/ TDAT_KJOUL = 'J', /*kilojoules (kJ/mole)*/
    TDAT_CAL = 'c', /* calories (cal/mole); 1 cal = 4.184 J; */
    TDAT_KCAL = 'C', /*kilocalories (kcal/mole)*/
    TDAT_NORM = 'N' /*normalized (mole/mole, J/mole/RT, cal/mole/RT)*/
                // Attention: Only j code can be used in this version!
} SPPA_UNITS;


typedef enum {  /* Classifications of DC */
    /* Type of input data for */
    SRC_DCOMP =  'd',  // the key points to existing PDB record in DCOMP chain
    SRC_REACDC = 'r',  // the key points to existing PDB record in REACDC chain
    SRC_NEWDC =  'n',  // the key new reaction-defined component
    SRC_NEWISO = 'i',  // the same as n, but this component is an isotopic form
    SRC_FICT =   'f',  // fictive species
    /*Aqueous electrolyte phase:*/
    DC_AQ_PROTON   = 'T',      // hydrogen ion H+
    DC_AQ_ELECTRON = 'E',      // electron (as a DC)
    DC_AQ_SPECIES  = 'S',      // other aqueous species (ions, complexes and ion pairs)
    DC_AQ_SOLVENT  = 'W',      // water H2O (major solvent)
    DC_AQ_SOLVCOM  = 'L',      // other components of a solvent (eg. alcohol)
    /*Gas phase ( G code can be used for all gases; V,C,H,N codes are reserved
    for future use of the built-in equations of state in FGL module): */
    DC_GAS_COMP    = 'G',   // other gases
    DC_GAS_H2O     = 'V',   // H2O steam
    DC_GAS_CO2     = 'C',   // CO2 (carbon dioxide)
    DC_GAS_H2      = 'H',   // H2 hydrogen
    DC_GAS_N2      = 'N',   // N2 nitrogen
    /* Solid/liquid non-electrolyte multicomponent phases:*/
    DC_SOL_IDEAL   = 'I',   // end-member component with ideal behaviour
    DC_SOL_MINOR   = 'J',   // junior component (Henry's Law)
    DC_SOL_MAJOR   = 'M',   // major component (Raoult's Law)
    /* Sorption phases and poly(oligo)electrolytes */
    DC_SUR_CARRIER = 'Q',   /* Principal end-member of solid carrier */
    DC_SUR_MINAL   = 'P',   /* Minor end-member of solid carrier */
    DC_PEL_CARRIER = 'R',   /* Carrier of poly(oligo)electrolyte */

    DC_SSC_A0 = '0', DC_SSC_A1 = '2', DC_SSC_A2 = '4', DC_SSC_A3 = '6',
    DC_SSC_A4 = '8', /* Strong surface complex on site type 0,1,2,3,4 - A plane */
    DC_WSC_A0 = '1', DC_WSC_A1 = '3', DC_WSC_A2 = '5', DC_WSC_A3 = '7',
    DC_WSC_A4 = '9', /* Weak surface complex on site type 0,1,2,3,4 - B plane */
    DC_IESC_A  = 'A', /* Strong exchange ion const-charge plane */
    DC_IEWC_B  = 'B', /* Weak exchange ion const-charge plane */

    /* Aliaces for 1-site model */
    DC_SUR_GROUP   = 'X',  /* Surface site A plane -> '0' */
    DC_SUR_COMPLEX = 'Y',  /* Strong sur. complex A plane -> '0' */
    DC_SUR_IPAIR   = 'Z',  /* Weak sur complex B plane -> '1' */

    /* Single-component phases:*/
    DC_SCP_CONDEN  = 'O',   // DC forming a single-component phase

} DC_CLASSES;

#define SORPTION_DC "0246813579ABXYZPQ"

//---------------------------------
// REACDC
//---------------------------------


enum reacdc_objects {   // reacdc
    //o_repst = o_bctprn+1, o_reps, o_redcn, o_reatyp,
    o_repct= o_bctprn+1, o_reprun,   /*6*/
    o_repra,    /*8*/ o_rermtm,  o_rezz,  o_remwt,
    o_reks,   /*d 3*/   o_regs,   /*d 3*/   o_rehs,   /*d 3*/
    o_ress,   /*f 3*/   o_recps,  /*f 3*/   o_remvs,  /*f 3*/
    o_renix,  /*f 3*/   o_reptst, /*f 2*/   o_realbet,/*f 2*/
    o_reder,  /*f 2*/
    /* record 20 fields */
    o_restr,  o_rename,  o_reform,  o_redck, /*s dyn*/  o_rerdc, /*c dyn*/
    o_redim,    /*8*/     o_refloat1, /*2*/     o_redbl1,   /*9*/
    o_refloat2, /*18*/    o_rescdc,  /*d dyn*/  o_retcint, /*f dyn*/
    o_repint,  /*f dyn*/  o_redcp,   /*f dyn*/  o_rehkfc,  /*f dyn*/
    o_redvt,   /*f dyn*/  o_redst,   /*f dyn*/  o_repkt,   /*f dyn*/
    o_relgK,   /*f dyn*/  o_resdref, /*s dyn*/  o_resdval, /*s dyn*/
    /* work arrays */
    o_repardc, /* d dyn */  o_retprn, /*txt*/
};

//---------------------------------
// RTParm
//---------------------------------

const  int MAXNV =          4;

enum rtparm_objects {   // 44  BD 43
    o_rtname = o_retprn+1, o_rtcom, o_rpunit, o_rpdim,
    o_rpmode, o_rpnumb, o_rpdimxy, o_rpdimef,
    o_rppi, o_rpti, o_rtaxis, o_rtsize,
    o_rtxnames, o_rtynames, o_rtlnam, o_rtlname,
    o_rtexpr, o_rtexpre, o_rtsdref, o_rtsdval,
    o_rpxt, o_rpxp, o_rpyf, o_rpxte,
    o_rpxpe, o_rpyte, o_rppline, o_rtjtp,
    o_rtie, o_rtje, o_rttext,
};

//---------------------------------
// Phase
//---------------------------------

const int
MAXPHNAME =      16,
                 PH_RKLEN = 48;

enum phase_objects {   // PHASE
    //o_phpst,   o_phsymb,  o_phnam,  o_phcls,  o_phgrp,
    o_phsolt = o_rttext+1,  o_phps,   /*a 6*/   o_phndc,  /*i 2*/
    o_phpncp, /*i 2*/   o_phnpx, /*i 1*/ o_phnsc, /*i 1*/  o_phnsit, /*i 3*/
    /* Record 22 fields */
    o_phstr,     o_phname,     o_phnotes,     o_phdim,   /*i 8*/
    o_phdisep,  /*f 8*/ o_phscmc, o_phxfiec, o_phmsdt,
    o_phcapt, o_phsatc, o_phmasdj,  o_phpxres,  /*dyn */
    o_phfsit,  /*f dyn*/  o_phpnc,   /*f dyn*/   o_phscoef, /*f dyn*/
    o_phsm,    /*s dyn*/  o_phdcc,   /*a dyn*/   o_phdcs,   /*a dyn*/
    o_phpeq,   /* txt */  o_phdeq,   /* txt */   o_phsdref, /*s dyn*/
    o_phsdval, /*s dyn*/
o_ph_w_lsc, o_ph_w_lsa, o_ph_w_lsn, o_ph_w_nxc, o_ph_w_nxa, o_ph_w_nxn, // new work dyn objects
    o_phtprn, /*txt*/
};


//---------------------------------
// SysEq = System(packed)+Eqstat
//---------------------------------

const int MAXSYWHAT =     18;
const int MAXPTN =         8;

enum syseq_objects {   // System
    o_ssphst = o_phtprn+1,  o_ssname,  o_ssnotes, o_ssswch, o_sspx,
    o_ssdm,  o_sspmm,  o_ssnnc,  o_ssllc,  o_ssphc,
    o_ssdcc,  o_ssnnf,  o_ssphf,  o_ssdcf,  o_sspha,
    o_ssdca,  o_ssphk,  o_ssdck,  o_ssdce,  o_ssphe,
    o_ssacl,  o_ssbiun,  o_ssxdun,  o_ssxaun,  o_ssxpun,
    o_ssbi,  o_ssxea,  o_ssxed,  o_ssphm, o_ssb,
    o_ssdelb,  o_sspparc,  o_ssgex,  o_sslngmf, o_ssaalp,
    o_ssyof,  o_sssigm,  o_ssxr0h0, o_ssxeps, o_ssrlc,
    o_ssrsc, o_ssrflc,  o_ssrfsc,  o_ssdul, o_ssdll,
    o_sspul,  o_sspll,  o_sssatc,  o_ssmasdj,  o_ssscmt,
    o_ssnfsp,  o_ssmasdt,  o_ssxcapa,  o_ssxcapb, o_ssxcapf,
    o_ssxfiec, o_ssxlam, o_sysdref,  o_sysdval,
    // Equstat
    o_stint,   o_stfloat, o_stnnf, o_stllf, o_stphf,
    o_stb,     o_stu,     o_sty,    o_stgam, o_stba,
    o_stvm,   o_stdic,
    o_stdim, o_stitr, o_sttdp, o_stcrit,
    o_stsbp, o_stsmp, o_stsfp,
};

//---------------------------------
// Process
//---------------------------------

const int
TP_RKLEN = 30,
           GD_RKLEN = 64,
                      EQ_RKLEN = 58,
                                 MAXPECODE =      4,
                                                  MAXPENAME =     18;

enum proces_objects {   // PROCES
    // o_pemus,   o_pepot,   o_pesymb,   o_petime_,   o_pepsymb,   o_pepcode,
    o_pepsts = o_stsfp+1 , /*a 12*/    o_pepvv,  /*a 12*/   o_pentxi, /*i 3*/
    o_penpvt, /*i 3*/  o_penphpe,/*i 3*/   o_pemode, /*i 2*/   o_pedim1,  /*i 4*/
    o_pedim2, /*i 8*/  o_petmi,  /*i 3*/   o_penvi,  /*i 3*/   o_pepi,   /*f 3*/
    o_peti,   /*f 3*/  o_pevi,   /*f 3*/   o_petaui, /*f 3*/   o_pepxii, /*f 3*/
    o_penui,  /*f 3*/  o_pephi,  /*f 3*/   o_pepei,  /*f 3*/  o_pencnt, /*i 2*/
    o_penext, o_pei,    o_pej,    o_pec_nrk,    o_pec_tm,    o_pec_nv,
    o_pec_p,  o_pec_v,    o_pec_t,    o_pec_tc,    o_pec_tau,    o_pec_pxi,
    o_pec_xi, o_pec_nu,    o_pec_ph,    o_pec_pe,    o_pec_eh,
    // Record 26
    o_pestr, /*reserved*/    o_pename,    o_penotes,
    o_pepspv, /*a 24*/    o_peint1, /*i 29*/    o_pefloat,/*f 24*/
    o_petmv, /*i dyn*/    o_penvv, /*i dyn*/    o_pepv,  /*f dyn*/
    o_pevv,  /*f dyn*/    o_petv,  /*f dyn*/    o_petauv,/*f dyn*/
    o_pepxiv,/*f dyn*/    o_penuv, /*f dyn*/    o_pekinv,/*f dyn*/
    o_pemodc,/*f dyn*/    o_peexpr,/* txt */    o_pestl, /*s dyn*/
    o_pesdref,/*s dyn*/   o_pesdval,/*s dyn*/  o_pegdkey,
    o_pestkey,            o_petpkey,            o_peint2, /*i 8*/
    o_pedouble,/*d 11*/   o_petprn ,/*txt*/
    /* new gtdemo */
    o_pcxnames,           o_pcynames,           o_pcdimxy,
    o_pcdimef,            o_pcaxis,             o_pcsize,
    o_pclnam,             o_pclname,            o_pcexpr,
    o_pcx0,               o_pcy0,               o_pcxe,
    o_pcye,               o_pcplline,
    /* end new */
    };

enum statcode {  /* Code status of process */
    P_UNDEF='0', P_STARTED, P_EXECUTE, P_INTERRUPT, P_FINISHED,
    P_MT_MODE, P_MT_STARTED, P_MT_EXECUTE, P_MT_INTERRUPT, P_MT_FINISHED };

enum pe_valind { /* index control */
    START_, STOP_, STEP_
};



//---------------------------------
// GtDemo
//---------------------------------

enum grr_constants { // gstring len for graph
    MAXAXISNAME = 9,
    MAXGSNAME = 70,
    MAXGRNAME = 7
};


enum gtdemo_objects {   // gtdemo
    o_gdps = o_pcplline+1, o_gdnlrk, o_gdnwcq, o_gddimxy, o_gddimef,
    o_gdname, o_gdcom, o_gdxnames,  o_gdynames, o_gdaxis,
    o_gwwkb, o_gdproc, o_gdsize,  o_gdlnam,  o_gdlname,
    o_gdexpr, o_gdexpre, o_gdrkey, o_gdsdref, o_gdsdval,
    o_gdx0,  o_gdy0,
    o_gdxe, o_gdye,  o_gdwc, o_gdqp, o_gdplline,

    o_gwjr, o_gwnext, o_gwwkey,  o_gwi0, o_gwie,
    o_gwj0, o_gwje,  o_gwetext, o_gwprtab, o_gwtprn,

};

//---------------------------------
// UnSpace
//---------------------------------


enum unspace_objects {                                                //120
    o_unsts = o_gwtprn+1, o_unpsflg, o_unpaflg, o_unpaflg4, o_unpvflg,
    o_unsiz, o_unq, o_unres, o_unng,  o_unngtpv,
    o_unpnt, o_unnmb, o_unnpha, o_un_t, o_un_q,
    o_un_g, o_un_i, o_un_j, o_un_k, o_un_ka,
    o_undimxy, o_undimef, o_un_tc, o_un_p, o_un_v,
    o_un_ph,  o_un_eh, o_un_ic, o_unquan,
// database  70
    o_unname, o_unnotes, o_unpapv, o_unint, o_unfloat,
    o_un_cr, o_unpbd,  o_unnglg, o_unnggam, o_unngls,
    o_unnglv, o_unngnb,  o_unf_pha, o_unintlgo, o_unintgam,
    o_unintls, o_unintlv, o_unintnb, o_unm_t_lo, o_unm_t_up,
    o_unfug_lo, o_unfug_up, o_ungs,  o_unss,  o_unvs,
    o_ungams,  o_unncp, o_unbs, o_unvg,  o_unbc,
    o_unsc, o_unmvc, o_unnidp,  o_unvy,  o_unvyf,
    o_unvgam,   o_unvmol, o_unvu, o_unvfug, o_unvt,
    o_unvp, o_unvv,  o_unvph, o_unovb, o_unovr,
    o_unovn, o_unqucx, o_unqucv,  o_ununicn, o_unugdcn,
    o_unuadcn, o_unundcan, o_unsgp, o_unexpr, o_unstl,
    o_unsdref,  o_unsdval, o_unstkey, o_unxnames, o_unynames,
    o_unaxis, o_unsize,  o_unlnam,   o_unlname,  o_ungexpr,
    o_unxa, o_unyc, o_unxs,  o_unys, o_unplline,
// end database
    o_unphndx,  o_unsv,   o_unphnum, o_unphaid, o_unphalst,
    o_unafreg, o_unpmr,  o_unpom, o_unpor, o_una,
    o_unzcp, o_unzmin, o_unzmax, o_unzmaxabs, o_unhom,
    o_unprob, o_ununic, o_unugdc,  o_unuadc, o_unundca,
    o_untprn,};

//---------------------------------
// Duterm
//---------------------------------


enum dualth_objects {
o_dtpvfl= o_untprn+1, o_dtpsflg, o_dtdim, o_dttmd, o_dtnvd,
o_dtpd,   o_dttd,   o_dtvd, o_dtmsysb, o_dtt,
o_dtp,   o_dtv, o_dtres,  o_dtq, o_dkp, o_dti,
o_dtjm,  o_dtc_tm, o_dtc_nv, o_dtnam_nr,
// Added SD oct 2005
o_lmft_s, o_lxi2, o_dttdat, o_dtydat, o_dtpar,
o_dtsdpar,
// Data Base
  o_dtname, o_dtnotes,  o_dtpspv, o_dtshort, o_dtfloat,
  o_dtan, o_dtbb, o_dtbn, o_dtub, o_dtchi,
// changed SD oct 2005 43
  o_dteta_b, o_dtact_n, o_dtgam_n, o_dtcoul, o_dtmu_n,
  o_dtmu_o, o_dtavsd_o, o_dtmu_a, o_dtavsd_a, o_gmx_n,
  o_dtwa, o_dtavsd_w, o_dtyconst, o_dtqpg, o_dtcib,
  o_dtcin, o_dtcab, o_dtcan,  o_dtcexpr, o_dtgexpr,
  o_dtsdref, o_dtsdval, o_dtnam_b, o_dtnam_n, o_dtpar_n,
  o_dtfor_n, o_dtfor_b, o_dtstl, o_dttyp_n, o_dtciclb,
  o_dtcicln, o_dtauclb,  o_dtaucln, o_dtsbm, o_dttdq,
  o_dtpdq, o_dtisq, o_dtwacp, o_dtwdat, o_dtwpar,
  o_dtwa_ap, o_dtstkey, o_dttprn,
};

typedef enum {

    GS_INDEF   = '0',
    GS_GOING    = '1',
    GS_DONE    = '2',
    GS_ERR     = '3',

    AS_INDEF   = '0',
    AS_READY   = '1',
    AS_RUN     = '2',
    AS_DONE    = '3',


} GS_AS_CLASSES;

const int  MAXIDNAME = 12;
const   int MAXFORMUNITDT=     40;

//---------------------------------
// GEM2MT
//---------------------------------

enum gem2mt_objects {
  o_mtpufl = o_dttprn+1, o_mtpvfl, o_mtpsfl, o_mtcipf, o_mtszt,
  o_mtnsne, o_mtptai, o_mtchbr, o_mttmi,  o_mtnvi, o_mtaxis,
  o_mtpai,  o_mttai,  o_mttau,  o_mtsize, o_mtsizelc,
// DBase 58+5=63
  o_mtname, o_mtnotes, o_mtflag, o_mtshort, o_mtworks,
  o_mtworkf, o_mtdoudl, o_mtadpar, o_mtfloat, o_mtxnames,
  o_mtynames, o_mtlnam, o_mtlname, o_mttexpr, o_mtgexpr,
  o_mtsdref, o_mtsdval, o_mtdicp, o_mt_xic,  o_mt_xdc,
  o_mt_xph,  o_mtfdli,  o_mtpi,   o_mtti,   o_mtvi,
  o_mtxet,  o_mtyet, o_mtddc, o_mtdic, o_mtdel,
  o_mtbn, o_mthydp, o_mtbsf, o_mtmb, o_mtdmb,
  o_mtqpi,  o_mtqpc, o_mtxt, o_mtyt,   o_mtcib,
  o_mtcab, o_mtfdlf, o_mtpgt, o_mttval, o_mtpval,
  o_mtnam_i, o_mtfor_i, o_mtfor_e, o_mtstld, o_mtciclb,
  o_mtaucln, o_mtfdlid, o_mtfdlop, o_mtfdlmp, o_mtmpgid,
  o_mtumpg,  o_mtbm,
o_mtgrid, o_mtnpmean, o_mtnpmin, o_mtnpmax, o_mtpartd,
  o_mtplline,
//work
  o_mtsykey, o_mtan, o_mtae,
  o_mtgc, o_mwetext, o_mwtprn, o_mtctm,  o_mtcnv,
  o_mtqc, o_mtkv, o_mtjqc, o_mtjqs,  o_mtjt,
  o_mtjdd, o_mtjdi, o_mtide, o_mtct_, o_mtrei5,
  o_mtct, o_mtcp, o_mtcv,  o_mtctau, o_mtdtau,
  o_mtotau, o_mtdx, o_mtref2, o_mtref3, o_mtref4
};

enum CHstruct_objects {  // data objects from DataCH structure for GEM2MT scripts
  o_ch_nicb = o_mtref4+1, o_ch_ndcb, o_ch_nphb, o_ch_npsb,
  o_ch_xic, o_ch_xdc, o_ch_xph, o_ch_a,
  o_ch_icmm, o_ch_dcmm, o_ch_dd, o_ch_aalp
};  // 12 DODs

enum FMTnode_objects {
  o_n0_ct = o_ch_aalp+1, o_n1_ct, o_n0_cs, o_n1_cs, o_n0_ts, o_n1_ts,
  o_n0_xdc, o_n1_xdc, o_n0_gam, o_n1_gam, o_n0_xph, o_n1_xph, o_n0_vps,
  o_n1_vps, o_n0_mps, o_n1_mps, o_n0_bps, o_n1_bps, o_n0_xpa, o_n1_xpa,
  o_n0_dul, o_n1_dul, o_n0_dll, o_n1_dll, o_n0_bic, o_n1_bic, o_n0_rmb,
  o_n1_rmb, o_n0_uic, o_n1_uic, o_n0_dr1, o_n1_dr1, o_n0_dr2, o_n1_dr2
};  // 34 DODs

#endif // _v_mod_h



