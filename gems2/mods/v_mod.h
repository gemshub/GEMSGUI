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
// See http://les.web.psi.ch/Software/GEMS-PSI for more information
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

enum { RT_INTEG=2, RT_PARAM=3, RT_ICOMP=4, RT_DCOMP=5, RT_COMPOS=6,
       RT_REACDC=7, RT_RTPARM=8, RT_PHASE=9, RT_SYSEQ=10, RT_PROCES=11,
       RT_PROBE=12, RT_GTDEMO=13, RT_DUTERM=14,

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

//---------------------------------
// Integ
//---------------------------------

enum std_object{
    o_hpage = 0, o_hrkey, o_hemsg, o_hedbuf, o_hrkeys,
    o_constlab, o_constdim, o_const, o_sdnref, o_sdauth, o_sdyear,
    o_sdclass,  o_sdauthr, o_sdtitle, o_sdedit,   o_sdvoly,
    o_sdpage, o_sdabstr, o_sdrefs, o_sdnote, o_sdbkwd,
    o_reckey, o_k_, o_ii_, o_prn_, o_rtkey, o_rttime
};

enum integ_objects {                  // work objects
    o_i_allx = o_rttime+1, o_i_ally, o_iallst, o_iallpr,
    o_ipstr, o_igexpr, o_ipdoub, o_iparam, o_ipy_bg, o_ig_txt,
    o_igkey, o_igname, o_igbgen, o_igeps, o_igstep,
    o_ival_x, o_imaxit, o_ignstp, o_ival_y, o_ivaldy
};


//---------------------------------
// Param
//---------------------------------

const int
MAXPARAMKEY =    24,
                 MNST =           6,
                                  MST =           MNST;

enum param_objects {                  // work objects
    o_paver =  o_ivaldy+1,
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
    // MTPARM
    o_tpunits, o_tpsv, o_tpptv, o_tp_T,  o_tp_P,
    o_tp_RT,  o_tprow,  o_tpepsw, o_tpvisw, o_tp_L,
    o_tpmark,  o_tpg,   o_tpdevg,  o_tph,  o_tpdevh,
    o_tps,    o_tpdevs,  o_tpcp,  o_tpdevcp,  o_tpvm,
    o_tpdevv,  o_tpf,   o_tpu,   o_tpcv,  o_tpalp,
    o_tpbet,   o_tpwbor, o_tpwrad, o_tpfug,  o_tpdvg,
    // SYSTEM inbiuld
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
    o_syxcapf, o_syxlam, o_syscmt,
    // MULTI
    o_wi_pun,  o_wi_nnr, o_wi_ldim, o_wi_fdim, o_wo_fdim,
    o_wo_it, o_wi_mode, o_wo_ipar, o_wio_tc, o_wio_t,
    o_wio_p,  o_wo_vx, o_wo_gx, o_wo_ax, o_wo_ux,
    o_wo_hx,   o_wo_sx, o_wo_cpx, o_wo_cvx,   o_wi_t0,
    o_wi_ve, o_wi_mbx, o_wo_fx, o_wo_ic,  o_wd_ph,
    o_wd_pe, o_wd_eh, o_wi_mol,  o_wi_gwat,o_wi_ymet,
    o_wio_denw, o_wio_epsw, o_wo_psi, o_wo_lnp, o_wo_rt,
    o_wo_frt,  o_wo_yw,  o_wio_cons,  o_wio_fitv,  o_wi_l1,
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
    o_wo_u,   o_wd_ur,  o_wo_c,  o_wd_icm,  o_wd_iclcm ,
    o_wd_icwm,  o_wo_bf,  o_w_xf ,  o_wo_yf,  o_ww_xfa,
    o_ww_yfa,  o_wo_falp,   o_w_xetaa,  o_w_xetab,  o_w_xfts,
    o_w_x,  o_wo_y, o_w_xy , o_wi_qp, o_wi_qd,
    o_wo_mu,  o_w_emu,  o_w_nmu,  o_w_w,  o_wo_fxa,
    o_wo_wx,  o_wo_f,  o_w_f0,  o_w_d,  o_wi_smod,
    o_wd_sb,  o_wd_sm,  o_wd_sf,  o_wd_sfs,
    o_wd_sm2, o_wd_sf2, o_wd_txtb,
    o_wi_rlc,  o_wi_rsc,  o_wi_rflc,  o_wi_rfsc,  o_wi_icc,
    o_wi_dcc, o_wi_phc,  o_wi_scm,  o_wi_satt,  o_wi_dccw,
    o_w_r, o_w_r1, o_w_sbh, o_w_tprn,
    // GammaCalc() - adjustable DOD for calculation of activity coefficients
    o_neqtxt,  o_nsmod,  o_ntc,  o_ntk,  o_np ,
    o_nis, o_ni,  o_njs,  o_nnext, o_nncp ,
    o_nncd,  o_ndc,  o_nbmol, o_nrt, o_nlnrt,
    o_nez,  o_npcv,  o_ndcm,  o_nmvol,  o_nppar,
    o_ngtn,  o_ngam,  o_nlngam,  o_nu,  o_nas,
    o_nxa,  o_nxaa,  o_nxast,  o_nxcec, o_nxx,
    o_nwx,   o_nmju,  o_nqp, o_nqd,  o_nwtext,
    // BASE
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
    //Codes for methods of calculation of DC thermod. parameters
    CTPM_CPT = 'C',  /* on the basis of empirical heat capacity equation Cp=f(T),
                    used for DCOMP and REACDC definitions (solid, liquid, gaseous
                     substances and aqueous species). */
    CTPM_HKF = 'H',  /* on the basis of Helgeson-Kirkham-Flowers equation of state
                        (HKF EOS), used for aqueous species (DCOMP) or for reactions
                        bwtween them (REACDC). */
    CTPM_REA = 'K',/*calculation of logK of reactions as they depend functionally
                       on T and P, used only for REACDC definitions of all phase
                       types of substances. */
    CTPM_EOS = 'S',  /*calculations via general equations of state (reserved) */
    CTPM_ISO = 'I',  /* calculation of parameters for isotopic forms of compounds
                         from fractionation constants and their dependency on T
                        (after Grichuk,1988); REACDC only. */
    CTPM_SOR = 'X',  /* calculations via adsorption or ion exchange constants, using
                 LFER correlations (Sverjensky 1993,1994) REACDC only (reserved). */

    //Codes for T dependencies of energy parameters'
    // (C) CTPM_CPT
    CTM_CST = 'S',/*calculation of ordinary integral using 11-term Cp=f(T) equations
                     (up to 3 phase transitions or up to 3 Cp=f(T) equations
                      for a DCOMP definition) */
    CTM_CHP = 'H',//the same with modifications by Holland&Powell,1990 (minerals);
    CTM_BER = 'B',// the same with modifications by Berman,1988 (minerals)
    CTM_FEI = 'F',// Cp=f(T) equation by Fei&Saxena,1986 (reserved)
    // (H) CTPM_HKF
    CTM_HKF = 'K',//calculation with HKF EOS (for aqueous species) as in SUPCRT92
    CTM_WAT = 'W',/* calculation of H2O water(steam) properties from HGK and LVS
                        equations of state (also using subroutines of SUPCRT92); */
    CTM_AKI = 'A',// modified HKF EOS for ion pairs by Akinfiev,1992 (reserved);
    // (K) CTPM_REA
    CTM_LGK = 'L',/* calculation from empirical function lgK=f(T) in 7-term form
                      (Nordstrom&Munoz,1986) - REACDC only; */

    CTM_LGX = 'X',  /* reserved */
    CTM_EK1 = '1', /* calculation from a one-parametric approximation for iso-
                     coulombic reactions (Gu et al.,1994), assuming dGr(T) = dGr(Tr)
                     = const and dSr(Tr) = 0 - REACDC only;*/
    CTM_EK2 = '2', /* the same, but using a two-parametric approximation + dHr(Tr)
                    (Vant Hoff equation) - REACDC only;а */
    CTM_EK3 = '3',  /*the same, using a three-parameter approximation
                     (dCpr = const)- REACDC only;*/
    CTM_IKZ = 'Z',/* calculation using Lagrange polynomial interpolation over the
                    array of lgK for several fixed T (P) points (REACDC only). */
    CTM_DKR = 'R',  /* calculation of lgK of dissociation reactions at elevated TP
                         from equation: lgK= A + B*RO(H2O) (Frantz & Marshall, 1982)
                       - REACDC only (reserved); */
    CTM_PPE = 'E',  /* Dissociation reaction for PRONSPREP 19.05.98 */
    /*CTM_DKE = 'E',  * the same using electrostatic model of Ryzhenko-Bryzgalin
                      (reserved); */
    // (S) CTPM_EOS
    CTM_CPG = 'C',  /* reserved (FGL) */ //(S)С - reserved
    // (I) CTPM_ISO
    CTM_IFG = 'G',  /* calculation of G0 for isotopic forms from equations (Grichuk,
                      1988) - REACDC only. */
    // (X) CTPM_SOR
    CTM_KAS = 'K',  /* reserved */
    CTM_DAS = 'D',  /* reserved */
    // Codes for accounting P-dependencies
    // 1.Codes valid for any combination of method - and T-dependence codes:
    CPM_OFF  = 'N', /* no account for pressure dependence; */
    CPM_NUL  = '0', /* account for pressure dependence is already specified as
                      inherent to a method; */
    CPM_ONE  = '1', /*  V0 */
    CPM_TWO  = '2',
    CPM_TRI  = '3',
    CPM_CON  = 'C',/*molar volume of DC or dVr are assumed independent of P and T */
    // 2.Codes valid for calculations based on empirical Cp=f(T) equation;
    // CTPM_CPT and CTPM_REA
    CPM_VKE  = 'K', /*molar volume is calculated from empirical equation V=f(T,P)
                           (modification by Dorogokupets et.al. 1988); */
    CPM_VBE  = 'V', /* calculation via V=f(T,P) as modified by (Berman,1988); */
    // CTPM_CPT

    CPM_VBM  = 'B', /* alloc. array; calculate Birch-Murnaghan (1947) V(T,P) */
    CPM_CEH  = 'E',/*DC molar volume V(T,P) is calculated from isothermal compres-
                      sibility and isobaric expandability (Holland&Powell,1990); */
    CPM_GAS  = 'G', /*calculation of fugacities of gases from their critical para-
                      meters and corresponding state theory (FGL-Karpov ea.1995)
                      (reserved);а */
    // 3. Codes used together with calculations by modified HKF EOS:
    // CTPM_HKF
    CPM_HKF  = 'F',/*account for pressure up to 5 kbar by HKF (Tanger,Helgeson,1988)*/
    /*  CPM_AKI  = 'A', *calculation of partial molal volumes for aqueous ions after
          EOS (Akinfiev, 1995) - reserved; */
    CPM_PCR  = 'P',  /* PARCOR estimation of HKF EOS params 19.05.98 */

    // CTPM_REA
    CPM_INK  = 'Z', /* calculation using Lagrange polinomial interpolation over
                     the array  of lgK(T,P). */
    CPM_EMP  = 'S'  /* calculation from Equation of State (reserved) */
               /*4. Codes for calculations of pressure impact on properties derived via
                  REACDC lgK functions.
                K - V=f(T,P) equation in the form (Dorogokupets et.al.,1988);
                V - V=f(T,P) equation in the form (Berman,1988);
                Z - calculation using Lagrange polinomial interpolation over the array
                    of lgK(T,P).
                0 - for 1,2,3 - parametric approximations of isocoulombic reactions
                    assuming dVr = const.
                                              */
};

enum DC_phase_state {   // Code of phase state identification of DC record keys
    CP_AQU   = 'a',  // aqueous electrolyte
    CP_GAS   = 'g',  // gaseous species
    CP_GASI  = 'p',  // plasma species
    CP_SOLID = 's',  // components of condensed solid phases, incl. sorption carriers
    CP_LIQID = 'l',  // components of condensed liquid non-electrolyte phases
    CP_MELT  = 'm',  // non-aqueous electrolyte species (eq., in magmatic melt)
    CP_SSPC  = 'c',  // surface complex (adsorbate)
    CP_SORB  = 'x',  // exchangeable ion
    CP_CPEL = 'y',   // complex on poly- (oligo)electrolyte (reserved)
    CP_MACR = 'q',   // carrier (macromolecule) of poly- (oligo)electrolyte,
    // or constantcharge carrier of clay mineral or ion exchange resin
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
    QUAN_MKMOL = 'Y',  QUAN_MMOL = 'h',  QUAN_MOL = 'M', // NUMBER OF MOLES
    QUAN_MGRAM = 'y',  QUAN_GRAM = 'g',  QUAN_KILO = 'G',// MASS
    /* concentrations of components and phases*/
    CON_MOLFR = 'n', CON_MOLPROC = 'N', CON_pMOLFR = 'f', // MOLE FRACTION
    CON_VOLFR = 'v', CON_VOLPROC = 'V', CON_pVOLFR = 'u', // VOLUME FRACTION
    CON_MOLAL = 'm', CON_MMOLAL =  'i', CON_pMOLAL = 'p', // MOLALITY
    CON_MOLAR = 'L', CON_MMOLAR =  'j', CON_pMOLAR = 'q', // MOLARITY
    CON_WTFR  = 'w', CON_WTPROC =  '%', CON_PPM =    'P', // MASS FRACTION
    CON_AQWFR = 'C', CON_AQWPROC = 'c', CON_AQPPM =  'a', // CONCENTRATION
    // aqueous species
    CON_AQGPL = 'd', CON_AQMGPL = 'e', CON_AQMKGPL = 'b',//VOLUME CONCENTRATION

    /* поверхностных величин */
    SUR_AREAM = 's',  SUR_AREAN = 'S', /* Уд.поверхность м2/г, м2/мол */
    SUR_CONA  = 'A', /* Поверхн. избыток мол/м2 */
    SUR_CONM  = 'D', /* Поверхн. избыток мол/г */
    SUR_CONN  = 'E', /* Поверхн. избыток мол/мол */

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
    DC_SUR_SITE    = 'X',  /* Surface site A plane -> '0' */
    DC_SUR_COMPLEX = 'Y',  /* Strong sur. complex A plane -> '0' */
    DC_SUR_IPAIR   = 'Z',  /* Weak sur complex B plane -> '1' */

    /* Single-component phases:*/
    DC_SCP_CONDEN  = 'O',   // DC forming a single-component phase

    /* Дополнительные коды для печати результатов расчета равновесий */
    DC_AQA_CATION   = 'c',  /* Катион */
    DC_AQA_ANION    = 'n',  /* Анион */
    DC_AQA_LIGAND   = 'l',  /* Органический лиганд */
    DC_AQA_COMPLEX  = 'x',  /* Комплекс в водном растворе */
    DC_AQA_IONPAIR  = 'p',  /* Ионная пара */
    DC_AQA_GAS      = 'g',  /* Растворенный газ (нейтральн.) */
    DC_AQA_ACID     = 'a',  /* Кислота */
    DC_AQA_BASE     = 'b',  /* Щелочь */
    DC_AQA_SALT     = 's',  /* Соль (недиссоц. электролит) */
    DC_AQA_HYDROX   = 'o'   /* Ион гидроксила */

} DC_CLASSES;

#define SORPTION_DC "0246813579ABXYZ"

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
    o_phpncp, /*i 2*/  o_phnsc,  /*i 2*/   o_phnsit, /*i 2*/
    /* Record 22 fields */
    o_phstr,     o_phname,     o_phnotes,     o_phdim,   /*i 8*/
    o_phdisep,  /*f 8*/ o_phscmc, o_phxfiec, o_phmsdt,
    o_phcapt, o_phsatc, o_phmasdj,  o_phpxres,  /*dyn */
    o_phfsit,  /*f dyn*/  o_phpnc,   /*f dyn*/   o_phscoef, /*f dyn*/
    o_phsm,    /*s dyn*/  o_phdcc,   /*a dyn*/   o_phdcs,   /*a dyn*/
    o_phpeq,   /* txt */  o_phdeq,   /* txt */   o_phsdref, /*s dyn*/
    o_phsdval, /*s dyn*/   o_phtprn, /*txt*/
};


//---------------------------------
// SysEq = System(packed)+Equstat
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
    o_pestr, /*reservedа*/    o_pename,    o_penotes,
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

enum statcode {  /* Code status of processа */
    P_UNDEF='0', P_STARTED, P_EXECUTE, P_INTERRUPT, P_FINISHED };



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
// Probe
//---------------------------------


enum probe_objects {
    o_prdim = o_gwtprn+1,  o_prntpv,  o_printt, o_printp,  o_printv,
    o_prname,  o_prnotes,  o_przond,  o_print,  o_prfloat,
    o_prl1,    o_prnnf,    o_prllf,   o_prkkf,  o_prnglg,
    o_prngls,  o_prnglv,   o_prlgam,  o_prlgex, o_prngn,
    o_prgs,    o_prilg,    o_prss,    o_prils,  o_prbs,
    o_prin,    o_prvs,     o_prilv,   o_prgams, o_prigm,
    o_prgex,   o_prigx,    o_provr,   o_priopt, o_prsgp,
    o_przpg,   o_prexpr,   o_prstl,   o_prsdref,o_prsdval,
    // work DOD
    o_prsbp,   o_prsmp,   o_prsfp,    o_prnmv,  o_prloop,
    o_prnrk,   o_prtm,    o_prnv,     o_prtprn,  };

//---------------------------------
// Duterm
//---------------------------------

enum duterm_objects {
    o_utis = o_prtprn+1, o_utph, o_uteh,  o_utyms,  o_utstv,
    o_utmvs,    o_utpvs,   o_utdim,  o_utfloat, o_utname,
    o_utnotes,  o_utphkey, o_utllf,  o_utnnf,   o_utsdm,
    o_utddf,    o_utdcc,   o_utdcs,  o_utreslt, o_utune,
    o_utunwx,   o_utunic,  o_utsbm,  o_utexpr,  o_utmu,
    o_utgt0,    o_utgtp,   o_utgex,  o_utgexk,  o_utuic,
    o_ut_qp,    o_utwx,    o_utcx,   o_utgam,   o_utpsi,
    o_utnsph,   o_utnsig,  o_utarea, o_utsigm,  o_utwarm,
    o_utyarm,   o_uterrea, o_uticme, o_uticmc,  o_ut_ime,
    o_ut_ima,   o_utwirm,  o_utyirm, o_utsdref, o_utsdval,
    o_uttpkey,  o_uta,     o_uttprn,  };

#endif // _v_mod_h
