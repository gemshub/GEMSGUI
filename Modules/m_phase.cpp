//-------------------------------------------------------------------
// $Id: m_phase.cpp 1366 2009-07-18 20:54:59Z wagner $
//
// Implementation of TPhase class, config and calculation functions
//
// Rewritten from C to C++ by S.Dmytriyeva
// Copyright (C) 1995-2009 S.Dmytriyeva, D.Kulik, T. Wagner
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
#include <cmath>

#include "visor.h"
#include "service.h"
#include "m_dcomp.h"
#include "m_reacdc.h"
#include "s_formula.h"
#include "m_phase.h"
#include "m_param.h"
#include "filters_data.h"
#include "GEMS3K/jsonconfig.h"

int rkeycmp(const void *e1, const void *e2);
int rkeycmp(const void *e1, const void *e2)
{
    int RCmp;
    RCmp = memcmp( e1, e2, DC_RKLEN );
    return RCmp;
}

TPhase* TPhase::pm;


TPhase::TPhase( uint nrt ):
        TCModule( nrt )
{
    nQ = 1;
    aFldKeysHelp.push_back("Code of phase state { a g f p l m h s d x i z }");
    aFldKeysHelp.push_back("Group identifier for such phases (letters, digits)");
    aFldKeysHelp.push_back("Name of this phase definition (letters, digits)");
    aFldKeysHelp.push_back("Phase class { c d l gm ss ssd ls aq xsa xc }");
    aFldKeysHelp.push_back("Comment to phase definition");
    php=&ph[0];
    set_def();
    start_title = " Definition of thermodynamic phase ";
}


// link values to objects
void TPhase::ods_link( int q)
{
    ErrorIf( q > nQ, GetName(), "E00PHrem: Invalid link q>nQ");

    aObj[ o_phsolt]->SetPtr( ph[q].sol_t );
    aObj[ o_phps]->SetPtr(  &ph[q].PphC );  // a 6
    aObj[ o_phndc]->SetPtr( &ph[q].nDC );   // i 2
    aObj[ o_phpncp]->SetPtr(&ph[q].ncpN );  // i 2
    aObj[ o_phnpx]->SetPtr( &ph[q].npxM );  // i 1  added 07.12.2006 KD
    aObj[ o_phnsc]->SetPtr( &ph[q].nscM );  // i 1  changed 07.12.2006 KD
    aObj[ o_phnsit]->SetPtr(&ph[q].NsuT );  // i 2

// new 06/06/12
    aObj[ o_phkin_t]->SetPtr(  ph[q].kin_t );  // a 8
    aObj[ o_phpdqf]->SetPtr(  &ph[q].Pdqf );  // a 7
    aObj[ o_phpeipc]->SetPtr(  &ph[q].PEIpc );  // a 6
    aObj[ o_phprpcon]->SetPtr(  &ph[q].PrpCon );  // a 3
    aObj[ o_phnlph]->SetPtr(  &ph[q].nlPh );  // i 9
    aObj[ o_phnumpc]->SetPtr(  &ph[q].nPRk );  // i 6
    aObj[ o_phnei1]->SetPtr(  &ph[q].nEIl );  // i 4
    aObj[ o_phnisoc]->SetPtr(  &ph[q].nIsoC );  // i 5
    aObj[ o_phvpor]->SetPtr(  &ph[q].Vpor );  // f 4
    aObj[ o_phpsdc]->SetPtr(  &ph[q].psdC );  // f 4

    /* Record 22 fields */
    aObj[ o_phstr]->SetPtr(  ph[q].sol_t );
    aObj[ o_phname]->SetPtr( ph[q].name );
    aObj[ o_phnotes]->SetPtr(ph[q].notes );
    aObj[ o_phdim]->SetPtr( &ph[q].nDC );     // i 8
    aObj[ o_phdisep]->SetPtr(&ph[q].Asur );   // f 8
    // NsuT
    aObj[ o_phscmc]->SetPtr(  ph[q].SCMC );
    aObj[ o_phscmc]->SetDim( ph[q].NsuT, 1 );
    aObj[ o_phxfiec]->SetPtr( ph[q].XfIEC);
    aObj[ o_phxfiec]->SetDim( ph[q].NsuT, 1);
    aObj[ o_phmsdt]->SetPtr(  ph[q].MSDT );
    aObj[ o_phmsdt]->SetDim( ph[q].NsuT, 2 );
    aObj[ o_phcapt]->SetPtr(  ph[q].CapT );
    aObj[ o_phcapt]->SetDim( ph[q].NsuT, 2 );
    aObj[ o_phfsit]->SetPtr(  ph[q].FsiT );
    aObj[ o_phfsit]->SetDim( ph[q].NsuT, 1 );
    //nDC
    aObj[ o_phsatc]->SetPtr(  ph[q].SATC );
    aObj[ o_phsatc]->SetDim( ph[q].nDC,  MCAS );
    aObj[ o_phmasdj]->SetPtr( ph[q].MaSdj);
    aObj[ o_phmasdj]->SetDim( ph[q].nDC, DFCN );
    aObj[ o_phpxres]->SetPtr( ph[q].ipxt);  // changed 07.12.2006 KD
// if(ph[q].ipxt)
  aObj[ o_phpxres]->SetDim( ph[q].ncpN, ph[q].npxM );
// else
//  aObj[ o_phpxres]->SetDim( 0, 0 );  // Bugfix 02.06.2009 DK
    aObj[ o_phsm]->SetPtr(   ph[q].SM[0] );
    aObj[ o_phsm]->SetDim( ph[q].nDC, 1 );
    aObj[ o_phdcc]->SetPtr(  ph[q].DCC );
    aObj[ o_phdcc]->SetDim( ph[q].nDC, 1 );
    aObj[ o_phdcs]->SetPtr(  ph[q].DCS );
    aObj[ o_phdcs]->SetDim( ph[q].nDC, 1 );

    aObj[ o_phpnc]->SetPtr(  ph[q].pnc );
    aObj[ o_phpnc]->SetDim( ph[q].ncpN, ph[q].ncpM );
    aObj[ o_phscoef]->SetPtr( ph[q].scoef );
    aObj[ o_phscoef]->SetDim( ph[q].nDC, ph[q].nscM ); // changed 07.12.2006  KD
    // ??????
    aObj[ o_phpeq]->SetPtr(  ph[q].pEq );
    //   aObj[ o_phpeq]->SetDim( 1, strlen(ph[q].pEq));
    aObj[ o_phdeq]->SetPtr(  ph[q].dEq );
    //   aObj[ o_phdeq]->SetDim( 1, strlen(ph[q].dEq));
    // Nsd
    aObj[ o_phsdref]->SetPtr( ph[q].sdref);
    aObj[ o_phsdref]->SetDim( ph[q].Nsd, 1 );
    aObj[ o_phsdval]->SetPtr( ph[q].sdval);
    aObj[ o_phsdval]->SetDim( ph[q].Nsd, 1 );
    /// ??????
    aObj[o_phtprn]->SetPtr( ph[q].tprn );

    // Added for SIT and Pitzer aqueous models implementation
    aObj[o_ph_w_lsc]->SetPtr( ph[q].lsCat );
    aObj[o_ph_w_lsc]->SetDim( ph[q].nCat, 1 );
    aObj[o_ph_w_lsa]->SetPtr( ph[q].lsAn );
    aObj[o_ph_w_lsa]->SetDim( ph[q].nAn, 1 );
    aObj[o_ph_w_lsn]->SetPtr( ph[q].lsNs );
    aObj[o_ph_w_lsn]->SetDim( ph[q].nNs, 1 );
    aObj[o_ph_w_nxc]->SetPtr( ph[q].nxCat );
    aObj[o_ph_w_nxc]->SetDim( ph[q].nCat, 1 );
    aObj[o_ph_w_nxa]->SetPtr( ph[q].nxAn );
    aObj[o_ph_w_nxa]->SetDim( ph[q].nAn, 1 );
    aObj[o_ph_w_nxn]->SetPtr( ph[q].nxNs );
    aObj[o_ph_w_nxn]->SetDim( ph[q].nNs, 1 );
    // added for multi-site mixin models implementation
aObj[o_ph_lsform]->SetPtr( ph[q].lsForm );
aObj[o_ph_lsform]->SetDim( ph[q].nDC, 1 );
aObj[o_ph_nsub]->SetPtr( &ph[q].nSub );
aObj[o_ph_lsmoi]->SetPtr( ph[q].lsMoi );
aObj[o_ph_lsmoi]->SetDim( 1, ph[q].nMoi );
aObj[o_ph_ocpn]->SetPtr( ph[q].OcpN );
aObj[o_ph_ocpn]->SetDim( ph[q].nDC, ph[q].nMoi );
aObj[o_ph_nxsub]->SetPtr( ph[q].nxSub );
aObj[o_ph_nxsub]->SetDim( ph[q].nDC, ph[q].nMoi );
//    aObj[ o_phtprn]->SetDim( 1,strlen(ph[q].tprn));

// new record 06/06/12
aObj[ o_phstr2]->SetPtr(  &ph[q].Pdqf ); // a 24
aObj[ o_phdim2]->SetPtr( &ph[q].nlPh );   // i 24
aObj[ o_phfloat2]->SetPtr(&ph[q].Vpor );  // f 8

//dynamic
aObj[ o_phxsmd]->SetPtr(  ph[q].xSmD );
aObj[ o_phxsmd]->SetDim( ph[q].nDC, ph[q].nSiT );
aObj[ o_phocprk]->SetPtr(  ph[q].ocPRk );
aObj[ o_phocprk]->SetDim( ph[q].nPRk, 2 );

aObj[ o_phlphc1]->SetPtr(  ph[q].lPhc );
aObj[ o_phlphc1]->SetDim( ph[q].nlPh, ph[q].nlPc );
aObj[ o_phdqfc]->SetPtr(  ph[q].DQFc );
aObj[ o_phdqfc]->SetDim( ph[q].nDC, ph[q].ndqf );
aObj[ o_phrcpc]->SetPtr(  ph[q].rcpc );
aObj[ o_phrcpc]->SetDim( ph[q].nDC, ph[q].nrcp );

aObj[ o_pheipc]->SetPtr(  ph[q].EIpc );
aObj[ o_pheipc]->SetDim( ph[q].nEIl, ph[q].nEIp );
aObj[ o_phcdc]->SetPtr(  ph[q].CDc );
aObj[ o_phcdc]->SetDim( ph[q].nDC, ph[q].nCDc );
aObj[ o_phisop]->SetPtr(  ph[q].IsoP );
aObj[ o_phisop]->SetDim( ph[q].nDC, ph[q].nIsoC );
aObj[ o_phisos]->SetPtr(  ph[q].IsoS );
aObj[ o_phisos]->SetDim( ph[q].nSiT, ph[q].nIsoS );
aObj[ o_phfesar]->SetPtr(  ph[q].feSAr );
aObj[ o_phfesar]->SetDim( ph[q].nPRk, 1 );
aObj[ o_phrpcon]->SetPtr(  ph[q].rpCon );
aObj[ o_phrpcon]->SetDim( ph[q].nPRk, ph[q].nrpC );
aObj[ o_phumpcon]->SetPtr(  ph[q].umpCon );
aObj[ o_phumpcon]->SetDim( ph[q].nDC, ph[q].numpC );

aObj[ o_phlph]->SetPtr(  ph[q].lPh );
aObj[ o_phlph]->SetDim( ph[q].nlPh, 1 );
aObj[ o_phldcr]->SetPtr(  ph[q].lDCr );
aObj[ o_phldcr]->SetDim( ph[q].nSkr, 1 );
aObj[ o_phldcd]->SetPtr(  ph[q].lDCd );
aObj[ o_phldcd]->SetDim( ph[q].nSkr, 1 );
//
aObj[ o_phlicu]->SetPtr(  ph[q].lICu );
aObj[ o_phlicu]->SetDim( ph[q].nDC, 1 );
//
aObj[ o_phdcpcl]->SetPtr(  ph[q].dcpcl );
aObj[ o_phdcpcl]->SetDim( 1, ph[q].nscM );
aObj[ o_phipicl]->SetPtr(  ph[q].ipicl );
aObj[ o_phipicl]->SetDim( ph[q].ncpN, 1 );
aObj[ o_phipccl]->SetPtr(  ph[q].ipccl );
aObj[ o_phipccl]->SetDim( 1, ph[q].ncpM );
aObj[ o_phrpkcl]->SetPtr(  ph[q].rpkcl );
aObj[ o_phrpkcl]->SetDim( 1, ph[q].nrpC );
aObj[ o_phrprcl]->SetPtr(  ph[q].rprcl );
aObj[ o_phrprcl]->SetDim( ph[q].nPRk, 1 );
aObj[ o_phumpcl]->SetPtr(  ph[q].umpcl );
aObj[ o_phumpcl]->SetDim( 1, ph[q].numpC );
aObj[ o_phsmcdl]->SetPtr(  ph[q].smcDl );
aObj[ o_phsmcdl]->SetDim( 1, ph[q].nIsoC );
aObj[ o_phsmcsl]->SetPtr(  ph[q].smcSl );
aObj[ o_phsmcsl]->SetDim( 1, ph[q].nIsoS );
aObj[ o_pheimpl]->SetPtr(  ph[q].eimPl );
aObj[ o_pheimpl]->SetDim( 1, ph[q].nCDc );
aObj[ o_pheimll]->SetPtr(  ph[q].eimLl );
aObj[ o_pheimll]->SetDim( 1, ph[q].nEIp );

aObj[ o_phlphc2]->SetPtr(  ph[q].lPhC );
aObj[ o_phlphc2]->SetDim( ph[q].nlPh, 1 );
aObj[ o_phisoc]->SetPtr(  ph[q].IsoC );
aObj[ o_phisoc]->SetDim( 2, ph[q].nSiT );

// added 17/12/12
aObj[ o_phixsolv]->SetPtr(  ph[q].ixsolv );
aObj[ o_phixsolv]->SetDim( 2, ph[q].nsolv );
aObj[ o_phixdiel]->SetPtr(  ph[q].ixdiel );
aObj[ o_phixdiel]->SetDim( 2, ph[q].ndiel );
aObj[ o_phsolvc]->SetPtr(  ph[q].solvc );
aObj[ o_phsolvc]->SetDim( ph[q].ncsolv, ph[q].nsolv );
aObj[ o_phdielc]->SetPtr(  ph[q].dielc );
aObj[ o_phdielc]->SetDim( ph[q].ncdiel, ph[q].ndiel);
aObj[ o_phdhc]->SetPtr(  ph[q].dhc );
aObj[ o_phdhc]->SetDim( 1, ph[q].ndh );
aObj[ o_phapcon]->SetPtr(  ph[q].apCon );
aObj[ o_phapcon]->SetDim( ph[q].nSkr, ph[q].nPRk * ph[q].naptC );
aObj[ o_phascp]->SetPtr(  ph[q].Ascp );
aObj[ o_phascp]->SetDim( 1, ph[q].nAscC );

 php=&ph[q];
}


// set dynamic Objects ptr to values
void TPhase::dyn_set(int q)
{
    ErrorIf( php!=&ph[q], GetName(),
             "E01PHrem: Invalid access to ph in dyn_set()");
    memcpy( php->pst_, rt[nRT]->UnpackKey(), PH_RKLEN );
    ph[q].SCMC =  static_cast<char *>(aObj[ o_phscmc ]->GetPtr());
    ph[q].FsiT =  static_cast<float *>(aObj[ o_phfsit ]->GetPtr());
    ph[q].XfIEC = static_cast<float *>(aObj[ o_phxfiec ]->GetPtr());
    ph[q].MSDT  = static_cast<float (*)[2]>(aObj[ o_phmsdt ]->GetPtr());
    ph[q].CapT  = static_cast<float (*)[2]>(aObj[ o_phcapt ]->GetPtr());
    ph[q].SATC  = static_cast<char (*)[MCAS]>(aObj[ o_phsatc ]->GetPtr());
    ph[q].MaSdj = static_cast<float (*)[DFCN]>(aObj[ o_phmasdj ]->GetPtr());
    ph[q].ipxt =  static_cast<short *>(aObj[ o_phpxres ]->GetPtr()); // changed 07.12.2006 KD
    // For safe use of old Phase records without ipxt table   07.12.2006
    if(!ph[q].ipxt )
        php->npxM = 0;
    ph[q].pnc =   static_cast<float *>(aObj[ o_phpnc ]->GetPtr());
    ph[q].scoef = static_cast<float *>(aObj[ o_phscoef ]->GetPtr());
    ph[q].SM =    static_cast<char (*)[DC_RKLEN]>(aObj[ o_phsm ]->GetPtr());
    ph[q].DCC =   static_cast<char *>(aObj[ o_phdcc ]->GetPtr());
    ph[q].DCS =   static_cast<char *>(aObj[ o_phdcs ]->GetPtr());
    ph[q].pEq =   static_cast<char *>(aObj[ o_phpeq ]->GetPtr());
    ph[q].dEq =   static_cast<char *>(aObj[ o_phdeq ]->GetPtr());
    ph[q].sdref = static_cast<char (*)[V_SD_RKLEN]>(aObj[ o_phsdref ]->GetPtr());
    ph[q].sdval = static_cast<char (*)[V_SD_VALEN]>(aObj[ o_phsdval ]->GetPtr());
    ph[q].tprn = static_cast<char *>(aObj[ o_phtprn ]->GetPtr());

    // Added for SIT aqueous model
    // Work objects for SIT, Pitzer, EUNIQUAC aqueous model
    if( ph[q].Ppnc == S_ON && ph[q].sol_t[SPHAS_TYP] == SM_AQSIT )
    {
        ph[q].lsCat = static_cast<char (*)[MAXDCNAME]>(aObj[ o_ph_w_lsc ]->GetPtr());
        ph[q].lsAn = static_cast<char (*)[MAXDCNAME]>(aObj[ o_ph_w_lsa ]->GetPtr());
        ph[q].lsNs = static_cast<char (*)[MAXDCNAME]>(aObj[ o_ph_w_lsn ]->GetPtr());
        ph[q].nxCat = static_cast<short *>(aObj[ o_ph_w_nxc ]->GetPtr());
        ph[q].nxAn = static_cast<short *>(aObj[ o_ph_w_nxa ]->GetPtr());
        ph[q].nxNs = static_cast<short *>(aObj[ o_ph_w_nxn ]->GetPtr());
    }
    else
    {
        ph[q].lsCat = static_cast<char (*)[MAXDCNAME]>(aObj[ o_ph_w_lsc ]->Free());
        ph[q].lsAn =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_ph_w_lsa ]->Free());
        ph[q].lsNs =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_ph_w_lsn ]->Free());
        ph[q].nxCat = static_cast<short *>(aObj[ o_ph_w_nxc ]->Free());
        ph[q].nxAn =  static_cast<short *>(aObj[ o_ph_w_nxa ]->Free());
        ph[q].nxNs =  static_cast<short *>(aObj[ o_ph_w_nxn ]->Free());
    }
    // added for multi-site mixin models implementation
    moiety_new( ph[q].nDC, ph[q].nMoi, true );

    // new record 06/06/12
    ph[q].xSmD =  static_cast<short *>(aObj[  o_phxsmd ]->GetPtr());
    ph[q].ocPRk =  static_cast<short *>(aObj[ o_phocprk ]->GetPtr());
    ph[q].lPhc =  static_cast<float *>(aObj[ o_phlphc1]->GetPtr());
    ph[q].DQFc =  static_cast<float *>(aObj[ o_phdqfc]->GetPtr());
    ph[q].rcpc =  static_cast<float *>(aObj[ o_phrcpc]->GetPtr());
    ph[q].EIpc = static_cast<float *>(aObj[ o_pheipc]->GetPtr());
    ph[q].CDc =  static_cast<float *>(aObj[ o_phcdc]->GetPtr());
    ph[q].IsoP =  static_cast<float *>(aObj[ o_phisop]->GetPtr());
    ph[q].IsoS =  static_cast<float *>(aObj[ o_phisos]->GetPtr());
    ph[q].feSAr =  static_cast<float *>(aObj[ o_phfesar]->GetPtr());
    ph[q].rpCon =  static_cast<float *>(aObj[ o_phrpcon]->GetPtr());
    ph[q].umpCon =  static_cast<float *>(aObj[ o_phumpcon]->GetPtr());
    ph[q].lPh =  static_cast<char (*)[PH_RKLEN]>(aObj[ o_phlph]->GetPtr());
    ph[q].lDCr =  static_cast<char (*)[DC_RKLEN]>(aObj[ o_phldcr]->GetPtr());
    //
    ph[q].lICu =  static_cast<char (*)[MAXICNAME]>(aObj[ o_phlicu]->GetPtr());
    //
    ph[q].lDCd =  static_cast<char *>(aObj[ o_phldcd]->GetPtr( ));
    ph[q].dcpcl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_phdcpcl]->GetPtr());
    ph[q].ipicl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_phipicl]->GetPtr());
    ph[q].ipccl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_phipccl]->GetPtr());
    ph[q].rpkcl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_phrpkcl]->GetPtr());
    ph[q].rprcl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_phrprcl]->GetPtr());
    ph[q].umpcl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_phumpcl]->GetPtr());
    ph[q].smcDl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_phsmcdl]->GetPtr());
    ph[q].smcSl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_phsmcsl]->GetPtr());
    ph[q].eimPl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_pheimpl]->GetPtr());
    ph[q].eimLl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_pheimll]->GetPtr());
    ph[q].lPhC =  static_cast<char *>(aObj[ o_phlphc2]->GetPtr( ));
    ph[q].IsoC =  static_cast<char *>(aObj[ o_phisoc]->GetPtr( ));
    // added 17/12/12
    ph[q].ixsolv = static_cast<short *>(aObj[ o_phixsolv]->GetPtr());
    ph[q].ixdiel =  static_cast<short *>(aObj[ o_phixdiel]->GetPtr());
    ph[q].solvc =  static_cast<float *>(aObj[ o_phsolvc]->GetPtr());
    ph[q].dielc =  static_cast<float *>(aObj[ o_phdielc]->GetPtr());
    ph[q].dhc  =  static_cast<float *>(aObj[ o_phdhc]->GetPtr());
    ph[q].apCon =  static_cast<float *>(aObj[ o_phapcon]->GetPtr());
    ph[q].Ascp = static_cast<float *>(aObj[ o_phascp]->GetPtr());
}


// free dynamic memory in objects and values
void TPhase::dyn_kill(int q)
{
    ErrorIf( php!=&ph[q], GetName(), "E02PHrem: Invalid access to ph in dyn_kill()");
    ph[q].SCMC =  static_cast<char *>(aObj[ o_phscmc ]->Free());
    ph[q].FsiT =  static_cast<float *>(aObj[ o_phfsit ]->Free());
    ph[q].XfIEC = static_cast<float *>(aObj[ o_phxfiec ]->Free());
    ph[q].MSDT =  static_cast<float (*)[2]>(aObj[ o_phmsdt ]->Free());
    ph[q].CapT =  static_cast<float (*)[2]>(aObj[ o_phcapt ]->Free());
    ph[q].SATC =  static_cast<char (*)[MCAS]>(aObj[ o_phsatc ]->Free());
    ph[q].MaSdj = static_cast<float (*)[DFCN]>(aObj[ o_phmasdj ]->Free());
    ph[q].ipxt =  static_cast<short *>(aObj[ o_phpxres ]->Free());  // changed 07.12.2006  KD
    ph[q].pnc =   static_cast<float *>(aObj[ o_phpnc ]->Free());
    ph[q].scoef = static_cast<float *>(aObj[ o_phscoef ]->Free());
    ph[q].SM =    static_cast<char (*)[DC_RKLEN]>(aObj[ o_phsm ]->Free());
    ph[q].DCC =   static_cast<char *>(aObj[ o_phdcc ]->Free());
    ph[q].DCS =   static_cast<char *>(aObj[ o_phdcs ]->Free());
    ph[q].pEq =   static_cast<char *>(aObj[ o_phpeq ]->Free());
    ph[q].dEq =   static_cast<char *>(aObj[ o_phdeq ]->Free());
    ph[q].sdref = static_cast<char (*)[V_SD_RKLEN]>(aObj[ o_phsdref ]->Free());
    ph[q].sdval = static_cast<char (*)[V_SD_VALEN]>(aObj[ o_phsdval ]->Free());
    ph[q].tprn = static_cast<char *>(aObj[ o_phtprn ]->Free());
    ph[q].lsCat = static_cast<char (*)[MAXDCNAME]>(aObj[ o_ph_w_lsc ]->Free());
    ph[q].lsAn =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_ph_w_lsa ]->Free());
    ph[q].lsNs =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_ph_w_lsn ]->Free());
    ph[q].nxCat = static_cast<short *>(aObj[ o_ph_w_nxc ]->Free());
    ph[q].nxAn =  static_cast<short *>(aObj[ o_ph_w_nxa ]->Free());
    ph[q].nxNs =  static_cast<short *>(aObj[ o_ph_w_nxn ]->Free());
// added for multi-site mixin models implementation
    ph[q].lsForm = static_cast<char (*)[MAXFORMULA]>(aObj[o_ph_lsform]->Free());
    ph[q].lsMoi = static_cast<char (*)[MAXDCNAME]>(aObj[o_ph_lsmoi]->Free());
    ph[q].OcpN =   static_cast<float *>(aObj[o_ph_ocpn]->Free());
    ph[q].nxSub = static_cast<short *>(aObj[o_ph_nxsub]->Free());

    // new record 06/06/12
    ph[q].xSmD =  static_cast<short *>(aObj[  o_phxsmd ]->Free());
    ph[q].ocPRk =  static_cast<short *>(aObj[ o_phocprk ]->Free());
    ph[q].lPhc =  static_cast<float *>(aObj[ o_phlphc1]->Free());
    ph[q].DQFc =  static_cast<float *>(aObj[ o_phdqfc]->Free());
    ph[q].rcpc =  static_cast<float *>(aObj[ o_phrcpc]->Free());
    ph[q].EIpc =  static_cast<float *>(aObj[ o_pheipc]->Free());
    ph[q].CDc =  static_cast<float *>(aObj[ o_phcdc]->Free());
    ph[q].IsoP =  static_cast<float *>(aObj[ o_phisop]->Free());
    ph[q].IsoS =  static_cast<float *>(aObj[ o_phisos]->Free());
    ph[q].feSAr =  static_cast<float *>(aObj[ o_phfesar]->Free());
    ph[q].rpCon =  static_cast<float *>(aObj[ o_phrpcon]->Free());
    ph[q].umpCon =  static_cast<float *>(aObj[ o_phumpcon]->Free() );
    ph[q].lPh =  static_cast<char (*)[PH_RKLEN]>(aObj[ o_phlph]->Free());
    ph[q].lDCr =  static_cast<char (*)[DC_RKLEN]>(aObj[ o_phldcr]->Free());
    ph[q].lDCd =  static_cast<char *>(aObj[ o_phldcd]->Free());
    ph[q].dcpcl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_phdcpcl]->Free());
    ph[q].ipicl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_phipicl]->Free());
    ph[q].ipccl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_phipccl]->Free());
    ph[q].rpkcl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_phrpkcl]->Free());
    ph[q].rprcl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_phrprcl]->Free());
    ph[q].umpcl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_phumpcl]->Free());
    ph[q].smcDl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_phsmcdl]->Free());
    ph[q].smcSl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_phsmcsl]->Free());
    ph[q].eimPl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_pheimpl]->Free());
    ph[q].eimLl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_pheimll]->Free());
    ph[q].lPhC =  static_cast<char *>(aObj[ o_phlphc2]->Free());
    ph[q].IsoC =  static_cast<char *>(aObj[ o_phisoc]->Free());
    // added 17/12/12
    ph[q].ixsolv =  static_cast<short *>(aObj[ o_phixsolv]->Free());
    ph[q].ixdiel =  static_cast<short *>(aObj[ o_phixdiel]->Free());
    ph[q].solvc =  static_cast<float *>(aObj[ o_phsolvc]->Free());
    ph[q].dielc =  static_cast<float *>(aObj[ o_phdielc]->Free());
    ph[q].dhc  =  static_cast<float *>(aObj[ o_phdhc]->Free());
    ph[q].apCon = static_cast<float *>(aObj[ o_phapcon]->Free());
    ph[q].Ascp =  static_cast<float *>(aObj[ o_phascp]->Free());
ph[q].lICu =  static_cast<char (*)[MAXICNAME]>(aObj[ o_phlicu]->Free());
}


// realloc dynamic memory
void TPhase::dyn_new(int q)
{
    ErrorIf( php!=&ph[q], GetName(), "E03PHrem: Invalid access to ph in dyn_new()");
    ErrorIf( ph[q].nDC <= 0, GetName(), "E04PHrem: Number of DC in the phase definition <= 0");
    ph[q].SM = static_cast<char (*)[DC_RKLEN]>(aObj[ o_phsm ]->Alloc( ph[q].nDC, 1, DC_RKLEN ));
    ph[q].DCS = static_cast<char *>(aObj[ o_phdcs ]->Alloc( ph[q].nDC, 1, A_ ));
    ph[q].DCC = static_cast<char *>(aObj[ o_phdcc ]->Alloc( ph[q].nDC, 1, A_ ));

    if( ph[q].Psco == S_ON )
    {      ph[q].scoef = static_cast<float *>(aObj[ o_phscoef]->Alloc( ph[q].nDC, ph[q].nscM, F_ ));
//                ph[q].nscN*ph[q].nscM, F_ );  changed 07.12.2006  KD
          ph[q].dcpcl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_phdcpcl]->Alloc( 1, ph[q].nscM, MAXDCNAME ));
    }
    else
    {    ph[q].scoef = static_cast<float *>(aObj[ o_phscoef ]->Free());
        ph[q].dcpcl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_phdcpcl]->Free());
    }


    if( ph[q].Ppnc == S_ON )
    {
       ph[q].pnc = static_cast<float *>(aObj[ o_phpnc ]->Alloc( ph[q].ncpN, ph[q].ncpM, F_ ));
       gui_logger->trace("ph[q].Ppnc == S_ON ncpN={} ncpM={}", ph[q].ncpN, ph[q].ncpM);
       ph[q].ipicl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_phipicl]->Alloc( ph[q].ncpN, 1, MAXDCNAME ));
       ph[q].ipccl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_phipccl]->Alloc( 1, ph[q].ncpM, MAXDCNAME ));
    }
    else
    {
       ph[q].pnc =   static_cast<float *>(aObj[ o_phpnc ]->Free());
       ph[q].ipicl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_phipicl]->Free());
       ph[q].ipccl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_phipccl]->Free());
    }

    if( ph[q].Ppnc == S_ON && ph[q].npxM > 0 )             // Bugfix 02.06.2009 DK
        ph[q].ipxt = static_cast<short *>(aObj[ o_phpxres ]->Alloc( ph[q].ncpN, ph[q].npxM, I_));
    if( ph[q].npxM <= 0 )
        ph[q].ipxt = static_cast<short *>(aObj[ o_phpxres ]->Free());
//    	aObj[ o_phpxres]->SetDim( 0, 0 );

    if( ph[q].PFsiT == S_ON || ph[q].PFsiT == S_REM )
    {
        ph[q].SCMC =  static_cast<char *>(aObj[ o_phscmc ]->Alloc( ph[q].NsuT, 1, A_));
        ph[q].FsiT =  static_cast<float *>(aObj[ o_phfsit ]->Alloc( ph[q].NsuT, 1, F_));
        ph[q].XfIEC = static_cast<float *>(aObj[ o_phxfiec ]->Alloc( ph[q].NsuT, 1, F_));
        ph[q].MSDT =  static_cast<float (*)[2]>(aObj[ o_phmsdt ]->Alloc( ph[q].NsuT, 2, F_));
        ph[q].CapT =  static_cast<float (*)[2]>(aObj[ o_phcapt ]->Alloc( ph[q].NsuT, 2, F_));
//        ph[q].SATC =  (char (*)[2]>(aObj[ o_phsatc ]->Alloc( ph[q].nDC, 2, A_);
        ph[q].SATC =  static_cast<char (*)[MCAS]>(aObj[ o_phsatc ]->Alloc( ph[q].nDC, MCAS, A_));
//        ph[q].MaSdj = static_cast<float *>(aObj[ o_phmasdj ]->Alloc( ph[q].nDC, 1, F_);
    ph[q].MaSdj = static_cast<float (*)[DFCN]>(aObj[ o_phmasdj ]->Alloc( ph[q].nDC, DFCN, F_));
    }
    else
    {
        ph[q].SCMC =  static_cast<char *>(aObj[ o_phscmc ]->Free());
        ph[q].FsiT =  static_cast<float *>(aObj[ o_phfsit ]->Free());
        ph[q].XfIEC = static_cast<float *>(aObj[ o_phxfiec ]->Free());
        ph[q].MSDT =  static_cast<float (*)[2]>(aObj[ o_phmsdt ]->Free());
        ph[q].CapT =  static_cast<float (*)[2]>(aObj[ o_phcapt ]->Free());
        ph[q].SATC =  static_cast<char (*)[MCAS]>(aObj[ o_phsatc ]->Free());
        ph[q].MaSdj = static_cast<float (*)[DFCN]>(aObj[ o_phmasdj ]->Free());
    }
    if( ph[q].Nsd )
    {
        ph[q].sdref = static_cast<char (*)[V_SD_RKLEN]>(aObj[ o_phsdref ]->Alloc(
                          ph[q].Nsd, 1, V_SD_RKLEN));
        ph[q].sdval = static_cast<char (*)[V_SD_VALEN]>(aObj[ o_phsdval ]->Alloc(
                          ph[q].Nsd, 1, V_SD_VALEN));
    }
    else
    {
        ph[q].sdref = static_cast<char (*)[V_SD_RKLEN]>(aObj[ o_phsdref ]->Free());
        ph[q].sdval = static_cast<char (*)[V_SD_VALEN]>(aObj[ o_phsdval ]->Free());
    }
    if( ph[q].pEq && ph[q].PpEq == S_OFF )
        ph[q].pEq  =  static_cast<char *>(aObj[ o_phpeq ]->Free());
    else
        if( ph[q].PpEq != S_OFF && !ph[q].pEq )
        {
            ph[q].pEq  =  static_cast<char *>(aObj[ o_phpeq ]->Alloc( 1, MAXFORMULA, S_));
            //*ph[q].pEq = '`';
        }
    if( ph[q].dEq && ph[q].PdEq == S_OFF )
        ph[q].dEq =   static_cast<char *>(aObj[ o_phdeq ]->Free());
    else
        if( ph[q].PdEq != S_OFF && !ph[q].dEq )
        {
            ph[q].dEq  =  static_cast<char *>(aObj[ o_phdeq ]->Alloc( 1, MAXFORMULA, S_));
            //*ph[q].dEq = '`';
        }

// Work objects for SIT, Pitzer, EUNIQUAC aqueous model
    if( ph[q].Ppnc == S_ON && ph[q].sol_t[SPHAS_TYP] == SM_AQSIT )
    {
         ph[q].lsCat = static_cast<char (*)[MAXDCNAME]>(aObj[ o_ph_w_lsc ]->Alloc(
                          ph[q].nCat, 1, MAXDCNAME ));
         ph[q].lsAn  = static_cast<char (*)[MAXDCNAME]>(aObj[ o_ph_w_lsa ]->Alloc(
                          ph[q].nAn, 1, MAXDCNAME ));
         ph[q].lsNs  = static_cast<char (*)[MAXDCNAME]>(aObj[ o_ph_w_lsn ]->Alloc(
                          ph[q].nNs, 1, MAXDCNAME ));
         ph[q].nxCat = static_cast<short *>(aObj[ o_ph_w_nxc ]->Alloc( ph[q].nCat, 1, I_));
         ph[q].nxAn  = static_cast<short *>(aObj[ o_ph_w_nxa ]->Alloc( ph[q].nAn, 1, I_));
         ph[q].nxNs  = static_cast<short *>(aObj[ o_ph_w_nxn ]->Alloc( ph[q].nNs, 1, I_));
    }
    else
    {
        ph[q].lsCat = static_cast<char (*)[MAXDCNAME]>(aObj[ o_ph_w_lsc ]->Free());
        ph[q].lsAn =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_ph_w_lsa ]->Free());
        ph[q].lsNs =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_ph_w_lsn ]->Free());
        ph[q].nxCat = static_cast<short *>(aObj[ o_ph_w_nxc ]->Free());
        ph[q].nxAn =  static_cast<short *>(aObj[ o_ph_w_nxa ]->Free());
        ph[q].nxNs =  static_cast<short *>(aObj[ o_ph_w_nxn ]->Free());
    }
    moiety_new(ph[q].nDC, ph[q].nMoi, false );
    //  ph[q].tprn = static_cast<char *>(aObj[ o_phtprn ]->Free();

    // new record 06/06/12
    if( ph[q].PEIpc == S_ON )
    {
        ph[q].EIpc =  static_cast<float *>(aObj[ o_pheipc]->Alloc( ph[q].nEIl, ph[q].nEIp, F_ ));
        ph[q].eimLl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_pheimll]->Alloc( 1, ph[q].nEIp, MAXDCNAME ));
    }
    else
    {
        ph[q].EIpc =  static_cast<float *>(aObj[ o_pheipc]->Free( ));
        ph[q].eimLl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_pheimll]->Free());
    }
    if( ph[q].PCDc == S_ON )
    {
      ph[q].CDc =  static_cast<float *>(aObj[ o_phcdc]->Alloc( ph[q].nDC, ph[q].nCDc, F_ ));
      ph[q].eimPl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_pheimpl]->Alloc( 1, ph[q].nCDc, MAXDCNAME ));
    }
    else
    {
        ph[q].CDc =  static_cast<float *>(aObj[ o_phcdc]->Free( ));
        ph[q].eimPl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_pheimpl]->Free());
    }
    if( ph[q].PIsoC == S_ON )
    {
      ph[q].IsoP =  static_cast<float *>(aObj[ o_phisop]->Alloc( ph[q].nDC, ph[q].nIsoC, F_ ));
      ph[q].smcDl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_phsmcdl]->Alloc( 1, ph[q].nIsoC, MAXDCNAME ));
    }
    else
    {
      ph[q].IsoP = static_cast<float *>(aObj[ o_phisop]->Free());
      ph[q].smcDl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_phsmcdl]->Free());
    }
    if( ph[q].PIsoS == S_ON )
    {
      ph[q].IsoS =  static_cast<float *>(aObj[ o_phisos]->Alloc( ph[q].nSiT, ph[q].nIsoS, F_ ));
      ph[q].smcSl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_phsmcsl]->Alloc( 1, ph[q].nIsoS, MAXDCNAME ));
    }
    else
    {
     ph[q].IsoS =  static_cast<float *>(aObj[ o_phisos]->Free( ));
     ph[q].smcSl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_phsmcsl]->Free());
    }
    if( ph[q].PsDiS == S_ON )
    {
        ph[q].xSmD =  static_cast<short *>(aObj[  o_phxsmd ]->Alloc( ph[q].nDC, ph[q].nSiT, I_ ));
        ph[q].IsoC =  static_cast<char *>(aObj[ o_phisoc]->Alloc( 2, ph[q].nSiT, A_ ));
    }
    else
    {
        ph[q].xSmD =  static_cast<short *>(aObj[  o_phxsmd ]->Free());
        ph[q].IsoC =  static_cast<char *>(aObj[ o_phisoc]->Free( ));
    }
    if( ph[q].PrpCon == S_ON )
    {
      ph[q].rpCon =  static_cast<float *>(aObj[ o_phrpcon]->Alloc( ph[q].nPRk, ph[q].nrpC, F_ ));
      //ph[q].lDCr =  (char (*)[DC_RKLEN])aObj[ o_phldc]->Alloc( ph[q].nSkr, 1, DC_RKLEN );
      ph[q].feSAr =  static_cast<float *>(aObj[ o_phfesar]->Alloc( ph[q].nPRk, 1, F_ ));
      ph[q].ocPRk =  static_cast<short *>(aObj[ o_phocprk ]->Alloc( ph[q].nPRk, 2, I_ ));
      ph[q].rpkcl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_phrpkcl]->Alloc( 1, ph[q].nrpC, MAXDCNAME ));
      ph[q].rprcl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_phrprcl]->Alloc( ph[q].nPRk, 1, MAXDCNAME ));
    }
    else
    {
        ph[q].rpCon =  static_cast<float *>(aObj[ o_phrpcon]->Free( ));
        //ph[q].lDCr =  (char (*)[DC_RKLEN]>(aObj[ o_phldc]->Free( );
        ph[q].feSAr =  static_cast<float *>(aObj[ o_phfesar]->Free( ));
        ph[q].ocPRk =  static_cast<short *>(aObj[ o_phocprk ]->Free());
        ph[q].rpkcl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_phrpkcl]->Free());
        ph[q].rprcl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_phrprcl]->Free());
    }
    if( ph[q].PapCon == S_ON )
    {
        ph[q].lDCr =  static_cast<char (*)[DC_RKLEN]>(aObj[ o_phldcr]->Alloc( ph[q].nSkr, 1, DC_RKLEN ));
        ph[q].lDCd =  static_cast<char *>(aObj[ o_phldcd]->Alloc( ph[q].nSkr, 1, A_ ));
      ph[q].apCon =  static_cast<float *>(aObj[ o_phapcon]->Alloc( ph[q].nSkr, ph[q].nPRk*ph[q].naptC, F_ ));
    }
    else
    {
        ph[q].lDCr =  static_cast<char (*)[DC_RKLEN]>(aObj[ o_phldcr]->Free());
        ph[q].lDCd =  static_cast<char *>(aObj[ o_phldcd]->Free());
        ph[q].apCon =  static_cast<float *>(aObj[ o_phapcon]->Free());
    }

    if( ph[q].nAscC > 0 )
      ph[q].Ascp =  static_cast<float *>(aObj[ o_phascp]->Alloc( 1, ph[q].nAscC, F_ ));
    else
        ph[q].Ascp =  static_cast<float *>(aObj[ o_phascp]->Free());

    if( ph[q].PumpCon == S_ON )
    {
      ph[q].umpCon =  static_cast<float *>(aObj[ o_phumpcon]->Alloc( ph[q].nDC, ph[q].numpC, F_ ));
      ph[q].umpcl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_phumpcl]->Alloc( 1, ph[q].numpC, MAXDCNAME ));
ph[q].lICu =  static_cast<char (*)[MAXICNAME]>(aObj[ o_phlicu]->Alloc( ph[q].nDC, 1, MAXICNAME ));
    }
    else
    {
      ph[q].umpCon =  static_cast<float *>(aObj[ o_phumpcon]->Free( ));
      ph[q].umpcl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_phumpcl]->Free());
ph[q].lICu =  static_cast<char (*)[MAXICNAME]>(aObj[ o_phlicu]->Free());
    }
    if( ph[q].PlPhl == S_ON )
    {
        ph[q].lPh =  static_cast<char (*)[PH_RKLEN]>(aObj[ o_phlph]->Alloc( ph[q].nlPh, 1, PH_RKLEN ));
        ph[q].lPhC =  static_cast<char *>(aObj[ o_phlphc2]->Alloc( ph[q].nlPh, 1, A_ ));
        ph[q].lPhc =  static_cast<float *>(aObj[ o_phlphc1]->Alloc( ph[q].nlPh, ph[q].nlPc, F_ ));
    }
    else
    {
        ph[q].lPh =  static_cast<char (*)[PH_RKLEN]>(aObj[ o_phlph]->Free());
        ph[q].lPhC =  static_cast<char *>(aObj[ o_phlphc2]->Free());
        ph[q].lPhc =  static_cast<float *>(aObj[ o_phlphc1]->Free());
    }

    if( ph[q].Pdqf == S_ON )
     ph[q].DQFc =  static_cast<float *>(aObj[ o_phdqfc]->Alloc( ph[q].nDC, ph[q].ndqf, F_ ));
    else
     ph[q].DQFc =  static_cast<float *>(aObj[ o_phdqfc]->Free());

    if( ph[q].Prcp == S_ON )
      ph[q].rcpc =  static_cast<float *>(aObj[ o_phrcpc]->Alloc( ph[q].nDC, ph[q].nrcp, F_ ));
    else
      ph[q].rcpc =  static_cast<float *>(aObj[ o_phrcpc]->Free());

    // added 17/12/12
    if( ph[q].Psol == S_ON )
    {
        ph[q].ixsolv =  static_cast<short *>(aObj[ o_phixsolv]->Alloc( 2, ph[q].nsolv, I_ ));
        ph[q].solvc =  static_cast<float *>(aObj[ o_phsolvc]->Alloc( ph[q].ncsolv, ph[q].nsolv, F_ ));
    }
    else
    {
        ph[q].ixsolv =  static_cast<short *>(aObj[ o_phixsolv]->Free());
        ph[q].solvc =  static_cast<float *>(aObj[ o_phsolvc]->Free());
    }
    if( ph[q].Pdiel == S_ON )
    {
        ph[q].ixdiel =  static_cast<short *>(aObj[ o_phixdiel]->Alloc( 2, ph[q].ndiel, I_ ));
        ph[q].dielc =  static_cast<float *>(aObj[ o_phdielc]->Alloc( ph[q].ncdiel, ph[q].ndiel, F_ ));
    }
    else
    {
        ph[q].ixdiel =  static_cast<short *>(aObj[ o_phixdiel]->Free());
        ph[q].dielc =  static_cast<float *>(aObj[ o_phdielc]->Free());
    }
    if( ph[q].Pdh == S_ON )
        ph[q].dhc =  static_cast<float *>(aObj[ o_phdhc]->Alloc( 1, ph[q].ndh, F_ ));
    else
        ph[q].dhc =  static_cast<float *>(aObj[ o_phdhc]->Free());
}


// realloc dynamic memory
void TPhase::moiety_new( int nDC, int nMoi, bool setDefault )
{
    // Work objects added for multi-site mixin models implementation
    php->nMoi = nMoi;

    if(php->nMoi >0  )
    {
        php->lsForm = static_cast<char (*)[MAXFORMULA]>(aObj[o_ph_lsform]->Alloc( nDC, 1, MAXFORMULA ));
        php->lsMoi = static_cast<char (*)[MAXDCNAME]>(aObj[o_ph_lsmoi]->Alloc( 1, php->nMoi, MAXDCNAME ));
        php->OcpN =   static_cast<float *>(aObj[o_ph_ocpn]->Alloc( nDC, php->nMoi, F_ ));
        php->nxSub = static_cast<short *>(aObj[o_ph_nxsub]->Alloc( nDC, php->nMoi, I_ ));
        if( setDefault )
        {
            fillValue( php->nxSub, static_cast<short>(-1), nDC*php->nMoi );
            fillValue( php->OcpN, static_cast<float>(0.), nDC*php->nMoi );
        }

    }
    else
    {
        php->lsForm = static_cast<char (*)[MAXFORMULA]>(aObj[o_ph_lsform]->Free());
        php->lsMoi = static_cast<char (*)[MAXDCNAME]>(aObj[o_ph_lsmoi]->Free());
        php->OcpN =   static_cast<float *>(aObj[o_ph_ocpn]->Free());
        php->nxSub = static_cast<short *>(aObj[o_ph_nxsub]->Free());
     }
}


//set default information
void TPhase::set_def( int q)
{
    ErrorIf( php!=&ph[q], GetName(), "E05PHrem: Invalid access to ph in set_def()");
    TProfil *aPa=dynamic_cast<TProfil *>(aMod[RT_PARAM].get());
    memcpy( ph[q].sol_t, aPa->pa.PHsol_t, 6 );
    memcpy( &ph[q].PphC, aPa->pa.PHpvc, 6 );
    strcpy( ph[q].name, "enter here a Phase name ...");
    strcpy( ph[q].notes, "enter here a comment to this Phase definition ..." );

    ph[q].Nsd = 1;
    ph[q].nDC = ph[q].ncpN = ph[q].ncpM =0;
    ph[q].NsuT = ph[q].nscM = ph[q].npxM = ph[q].nMoi = 0;
    ph[q].Asur =    0;//FLOAT_EMPTY;
    ph[q].Sigma0 =  0;//FLOAT_EMPTY;
    ph[q].SigmaG =  0;//FLOAT_EMPTY;
    ph[q].R0p =     0;//FLOAT_EMPTY;
    ph[q].h0p =     0;//FLOAT_EMPTY;
    ph[q].Eps =     0;//FLOAT_EMPTY;
    ph[q].Cond =    0;//FLOAT_EMPTY;
    ph[q].Rsp1 =    0;//FLOAT_EMPTY;
    ph[q].SCMC =  nullptr;
    ph[q].FsiT =  nullptr;
    ph[q].XfIEC = nullptr;
    ph[q].MSDT =  nullptr;
    ph[q].CapT =  nullptr;
    ph[q].SATC =  nullptr;
    ph[q].MaSdj = nullptr;
    ph[q].ipxt = nullptr;
    ph[q].pnc =   nullptr;
    ph[q].scoef = nullptr;
    ph[q].SM =    nullptr;
    ph[q].DCC =   nullptr;
    ph[q].DCS =   nullptr;
    ph[q].pEq =   nullptr;
    ph[q].dEq =   nullptr;
    ph[q].sdref = nullptr;
    ph[q].sdval = nullptr;
    ph[q].tprn = nullptr;
    // Work objects for SIT
    ph[q].nCat = 0;
    ph[q].nAn = 0;
    ph[q].nNs = 0;
    ph[q].lsCat = nullptr;
    ph[q].lsAn =  nullptr;
    ph[q].lsNs =  nullptr;
    ph[q].nxCat = nullptr;
    ph[q].nxAn =  nullptr;
    ph[q].nxNs =  nullptr;

    ph[q].lsForm = nullptr;
    ph[q].lsMoi = nullptr;
    ph[q].OcpN =  nullptr;
    ph[q].nxSub = nullptr;

    // new record 06/06/12
    memcpy( ph[q].kin_t, "NNNNNNNN", 8 );
    memcpy( &ph[q].Pdqf, "------------------", 17 );

    //short
    ph[q].nlPh = 0;
    ph[q].nlPc = 0;
    ph[q].ndqf=  0;
    ph[q].nrcp=  0;
    ph[q].ncsolv = 0;
    ph[q].nsolv = 0;
    ph[q].ncdiel = 0;
    ph[q].ndiel = 0;
    ph[q].ndh = 0;

    ph[q].nPRk = 0;
    ph[q].nSkr = 0;
    ph[q].nrpC = 0;
    ph[q].naptC = 0;
    ph[q].numpC = 0;
    ph[q].nAscC = 0;
    ph[q].nEIl = 0;
    ph[q].nEIp = 0;
    ph[q].nCDc = 0;
    ph[q].iRes3 = 0;
    ph[q].nIsoC = 0;
    ph[q].nIsoS = 0;
    ph[q].mDe = 0;
    ph[q].nFaces = 0;
    //float
    ph[q].Vpor = 0.;
    ph[q].fSAs = 1.;
    ph[q].fPV = 1.;
    ph[q].fRes1 = 0.;
    ph[q].psdC = 0.;
    ph[q].pvdC = 0.;
    ph[q].IEC = 0.;
    ph[q].fRes2 = 0.;

    ph[q].xSmD = nullptr;
    ph[q].ocPRk = nullptr;
    ph[q].lPhc = nullptr;
    ph[q].DQFc = nullptr;
    ph[q].rcpc = nullptr;
    ph[q].EIpc = nullptr;
    ph[q].CDc = nullptr;
    ph[q].IsoP = nullptr;
    ph[q].IsoS = nullptr;
    ph[q].feSAr = nullptr;
    ph[q].rpCon = nullptr;
    ph[q].umpCon = nullptr;
    ph[q].lPh = nullptr;
    ph[q].lDCr = nullptr;
    ph[q].lDCd = nullptr;
    ph[q].dcpcl = nullptr;
    ph[q].ipicl = nullptr;
    ph[q].ipccl = nullptr;
    ph[q].rpkcl = nullptr;
    ph[q].rprcl = nullptr;
    ph[q].umpcl = nullptr;
    ph[q].smcDl = nullptr;
    ph[q].smcSl = nullptr;
    ph[q].eimPl = nullptr;
    ph[q].eimLl = nullptr;
    ph[q].lPhC = nullptr;
    ph[q].IsoC = nullptr;
    // added 17/12/12
    ph[q].ixsolv = nullptr;
    ph[q].ixdiel = nullptr;
    ph[q].solvc =  nullptr;
    ph[q].dielc = nullptr;
    ph[q].dhc  =  nullptr;
    ph[q].apCon = nullptr;
    ph[q].Ascp = nullptr;
ph[q].lICu = nullptr;
}


// Input necessary data and links objects
void TPhase::RecInput( const char *key )
{
    TCModule::RecInput( key );
}

static TCStringArray aDclist;
static TCStringArray aPhlist;
static TCStringArray aDcSkrl;

// opens window with 'Remake record' parameters
void TPhase::MakeQuery()
{
//    pImp->MakeQuery();   Preparing for calling Phase Wizard
    std::string p_key;
    char flgs[37];
    int size[30];
    double r2 = php->Asur;

    p_key  = db->PackKey();
    memcpy( flgs, php->sol_t, 37);
    size[0] = php->Nsd;
    size[1] = php->ncpN;
    size[2] = php->ncpM;
    size[3] = php->nscM;
    size[4] = php->npxM;
    size[5] = php->NsuT;
// new
    size[6] = php->nlPh;
    size[7] = php->nlPc;
    size[8] = php->ndqf;
    size[9] = php->nrcp;
    size[10] = php->nPRk;
    size[11] = php->nSkr;
    size[12] = php->nrpC;
    size[13] = php->numpC;
    size[14] = php->nEIl;
    size[15] = php->nEIp;
    size[16] = php->nCDc;
    size[17] = php->iRes3;
    size[18] = php->nIsoC;
    size[19] = php->nIsoS;
    size[20] = php->mDe;
    size[21] = php->nFaces;
    // added 18/12/12
    size[22] = php->ncsolv;
    size[23] = php->nsolv;
    size[24] = php->ncdiel;
    size[25] = php->ndiel;
    size[26] = php->ndh;
    size[27] = php->naptC;
    size[28] = php->nAscC;
    size[29] = php->nSiT;

    if( !vfPhaseSet( window(), p_key.c_str(), flgs, size, r2, aDclist, aPhlist, aDcSkrl ))
    {
       db->SetKey(p_key.c_str());
       Error( p_key, "Phase record configuration cancelled by the user!" );
    }
    db->SetKey(p_key.c_str());
    memcpy( php->sol_t, flgs, 37);
    php->Nsd = static_cast<short>(size[0]);
    php->ncpN = static_cast<short>(size[1]);
    php->ncpM = static_cast<short>(size[2]);
    php->nscM = static_cast<short>(size[3]);
    php->npxM = static_cast<short>(size[4]);
    php->NsuT = static_cast<short>(size[5]);
    php->nlPh = static_cast<short>(size[6]);
    php->nlPc = static_cast<short>(size[7]);
    php->ndqf = static_cast<short>(size[8]);
    php->nrcp = static_cast<short>(size[9]);
    php->nPRk = static_cast<short>(size[10]);
    php->nSkr = static_cast<short>(size[11]);
    php->nrpC = static_cast<short>(size[12]);
    php->numpC = static_cast<short>(size[13]);
    php->nEIl = static_cast<short>(size[14]);
    php->nEIp = static_cast<short>(size[15]);
    php->nCDc = static_cast<short>(size[16]);
    php->iRes3 = static_cast<short>(size[17]);
    php->nIsoC = static_cast<short>(size[18]);
    php->nIsoS = static_cast<short>(size[19]);
    php->mDe = static_cast<short>(size[20]);
    php->nFaces = static_cast<short>(size[21]);
    // added 18/12/12
    php->ncsolv = static_cast<short>(size[22]);
    php->nsolv = static_cast<short>(size[23]);
    php->ncdiel= static_cast<short>(size[24]);
    php->ndiel= static_cast<short>(size[25]);
    php->ndh= static_cast<short>(size[26]);
    php->naptC= static_cast<short>(size[27]);
    php->nAscC= static_cast<short>(size[28]);
    php->nSiT= static_cast<short>(size[29]);

    php->Asur = r2;
    php->nDC = aDclist.size();
    php->nlPh = aPhlist.size();
    php->nSkr = aDcSkrl.size();
}


//Remake/Create mode: rebuild Phase record structure before calculations
int TPhase::RecBuild( const char *key, int mode  )
{
    ///TCStringArray aDclist;
    ///TCStringArray aPhlist;
    ///TCStringArray aDcSkrl;
//    TCStringArray aIclist;
//    std::string str;
    TProfil *aPa=dynamic_cast<TProfil *>(aMod[RT_PARAM].get());
    // old flag values to reset  parameter indexes comment
    char old_sol[7], old_kin[9];

    php->PphC = php->pst_[0];
    if( php->PphC == PH_FLUID /* && php->sol_t[SPHAS_TYP] == SM_OTHER */ )
    {
        php->nscM = MAXEOSPARAM;
    }

    php->Pinternal1 = S_OFF;
    if( db->FindCurrent(key) < 0 )
       php->Pinternal1 = S_ON;      // refresh DC codes if new record

    strncpy( old_sol, php->sol_t, 6);
    strncpy( old_kin, php->kin_t, 8);

    // build old selection keys
    aDclist.clear();
    aPhlist.clear();
    aDcSkrl.clear();
    makeReacDCompListNew( aDclist, php->nDC, php->SM, php->DCS );
    if( php->PlPhl != S_OFF )
    {
        makePhaseListNew( aPhlist );
        //db->SetKey(php->pst_);
     }
    if( php->PapCon != S_OFF )
    {
       //php->pst_[0] = '*';
       //db->SetKey(php->pst_);
       makeReacDCompListNew( aDcSkrl, php->nSkr, php->lDCr, php->lDCd );
       //php->pst_[0] = php->PphC;
       //db->SetKey(php->pst_);
    }

AGAIN_SETUP:
    int ret = TCModule::RecBuild( key, mode );
    if( ret == VF_CANCEL  &&!( !php->PphC || php->PphC == ' '))
        return ret;

    // Change Phase key
    memcpy( php->pst_, rt[nRT]->UnpackKey(), PH_RKLEN );
    php->pst_[0] = php->PphC;
    db->SetKey(php->pst_);

    if( ret == VF3_1 )
    {
        strncpy( php->name, db->FldKey(2), db->FldLen(2));
        php->name[db->FldLen(2)] = '\0';
    }

    //---------------------------------------------------------------------------------
        php->nSub = 0;
        SetString("PH_make   Remaking Phase definition");
        pVisor->Update();

        // Pre-proc. loop for SIT or Pitzer: determining number of cations and anion
        DetNumbCatAn(aDclist);

    //---------------------------------------------------------------------

    // Setting up the DC/phase coeffs depending on the
    // built-in activity coeff model
    if( php->sol_t[SGM_MODE] == SM_STNGAM )
       Set_SolMod_Phase_coef();

    // test sizes
    if( php->nscM < 0 || php->npxM < 0 || php->ncpN < 0 || php->ncpM < 0 ||
            php->ncpN*php->ncpM > MAXPNCOEF || php->Nsd < 0 || php->Nsd > 16 ||
            php->NsuT < 0 || php->NsuT > 6 )
    {
        if(vfQuestion(window(), GetName(),
        		"W06PHrem: Invalid number of coeffs in the non-ideal solution model!\n Repeat (Y) or Cancel (N)?"))
            goto AGAIN_SETUP;
        else   Error( GetName(), "E07PHrem: The user cancelled remaking Phase definition !");
    }

    // insert coeff of model of solid and other data
    if( php->nscM )
        php->Psco = S_ON;
    else
        php->Psco = S_OFF;
    if( php->ncpN && php->ncpM )
        php->Ppnc = S_ON;
    else
        php->Ppnc = S_OFF;
    if( !php->NsuT )
        php->PFsiT = S_OFF;
    if( php->sol_t[DCOMP_DEP] == SM_UNDEF )
        php->PdEq = S_OFF;
    else
        php->PdEq = S_ON;
    if( php->sol_t[SPHAS_DEP] == SM_UNDEF )
        php->PpEq = S_OFF;
    else
        php->PpEq = S_ON;
    if( (php->NsuT != S_OFF) && php->sol_t[SPHAS_TYP] == SM_SURCOM )
    {
       php->nMoi = 0; php->nSub = 0;
    }
    if( php->nDC < 2 )
        php->nMoi = 0;
    if( php->nMoi > 0 )
       php->PdEq = S_ON;

    if( php->kin_t[KinProCode] != KM_UNDEF )
       Set_KinMet_Phase_coef();

// Test sizes

    if( php->kin_t[0] != KM_UNDEF )
       Set_SorpMod_Phase_coef();

// Test sizes

    dyn_new(0);  // reallocation of memory

    // Get list of components : add aMcv and aMrv
    for(int ii=0; ii<php->nDC; ii++ )
    {
        memcpy( php->SM[ii], aDclist[ii].c_str()+2, DC_RKLEN );  // +2  check!
        php->SM[ii][DC_RKLEN-1] = aDclist[ii].c_str()[0];
    }
    // Sorting the list of dependent components
    if( php->nDC >= 2 )         // >= may change behavior !
        qsort( php->SM[0], php->nDC, DC_RKLEN, rkeycmp );

    // Load DC classes from records and set to DCC
    LoadDCC();

    if( php->PlPhl != S_OFF )
    {
        for(int i=0; i<php->nlPh; i++ )
        {
            memcpy( php->lPh[i], aPhlist[i].c_str(), PH_RKLEN );
            php->lPh[i][PH_RKLEN-1] = aPhlist[i].c_str()[0];
        }
        qsort( php->lPh[0], php->nlPh, PH_RKLEN, rkeycmp );
    }

    if( php->PapCon != S_OFF )
    { // Get list of components for parallel reactions
      for(int i=0; i<php->nSkr; i++ )
      {
        memcpy( php->lDCr[i], aDcSkrl[i].c_str()+2, DC_RKLEN );  // +2  check!
        php->lDCr[i][DC_RKLEN-1] = aDcSkrl[i].c_str()[0];
      }
      // Sorting the list of dependent components
      if( php->nSkr >= 2 )         // >= may change behavior !
        qsort( php->lDCr[0], php->nSkr, DC_RKLEN, rkeycmp );
      for(int i=0; i<php->nSkr; i++ )
      {
       php->lDCd[i] = php->lDCr[i][DC_RKLEN-1]; // species class code
       php->lDCr[i][DC_RKLEN-1] = ' ';
      }
    }

    if( php->PumpCon != S_OFF && php->nDC > 1 )   // added 13.06.13 by DK
    {
            for( int j=0; j<php->nDC; j++ ) // Set default selection of elements for end members
            {
                memcpy( php->lICu[j], "Tr       ", MAXICNAME );
                php->lICu[j][MAXICNAME-1] = '\0';
            }
    }

//---------------------------------------------------------------------
// old  part
    if( php->NsuT > 0 && (php->PFsiT == S_REM || php->PFsiT == S_ON  ))
    {  /* Setup of default values */
        php->PphC = PH_SORPTION;
        for(int i=0; i<php->NsuT; i++ )
        { /* if( !php->SCMC[i] || php->SCMC[i]==A_NUL ) */
            // php->SCMC[i] = php->sol_t[SCM_TYPE];  // fixed, 24.07.2006 (DK)
            php->SCMC[i] = SC_BSM;  // changed, 14.07.2009 (TW)

            if( !php->FsiT )
                php->FsiT[i] = 1.f/php->NsuT;
            if( approximatelyZero(php->MSDT[i][0]) )
                php->MSDT[i][0] = aPa->pa.p.DNS;
            if( approximatelyZero(php->MSDT[i][1]) )
                php->MSDT[i][1] = 4.0; // Third-layer capacitance (reserved)
            if( approximatelyZero(php->CapT[i][0]) )
                php->CapT[i][0] = 1.0; // C1 inner capacitance
            if( approximatelyZero(php->CapT[i][1]) )
                php->CapT[i][1] = 0.2f; // C2 outer capacitance
            if( !php->XfIEC )
                php->XfIEC[i] = 0;
        }
        for(int i=0; i<php->nDC; i++ )
        {
            php->SATC[i][SA_MCA] = SAT_L_COMP;  // default Langmuir comp.
            php->SATC[i][SA_EMX] = CCA_VOL;  // default per whole sorbent
            php->SATC[i][SA_STX] = CST_0;    // default - 0 surface type
            php->SATC[i][SA_PLAX] = SPL_0;   // default - zero EDL plane
            php->SATC[i][SA_SITX] = CSI_0;   // default - 0 site type
            php->SATC[i][SA_UNIT] = SDU_N;   // default - 'n'
        }
        php->PFsiT = S_ON;
    }

//---------------------------------------------------------------------
    if( php->PphC == PH_AQUEL &&
    		( php->sol_t[SPHAS_TYP] == SM_AQSIT || php->sol_t[SPHAS_TYP] == SM_AQPITZ ) )
    {  // Filling out name and index lists for cations and anions
       if( !php->lsCat )
       {
           php->Ppnc = S_ON;
           MakeCatAnLists( true, true, true );
       }
       else
           MakeCatAnLists( false, false, true);
    }
//--------------------------------------------------------------

    if( php->PpEq != S_OFF && php->pEq && !*php->pEq )
        strcpy( php->pEq, "---" );
    if( php->PdEq != S_OFF && php->dEq && !*php->dEq)
        strcpy( php->dEq, "---" );

    // set up default comments
    set_def_comments( ret == VF3_1, old_sol, old_kin );

    SetString("PH_make   Remake of Phase definition OK");
    pVisor->Update();
    return ret;
}

//#define s(i,j) php->scoef[(j)+(i)*nsc]
//#define sit(i,j) php->pnc[(j)+(i)*nAn] // not used SD 11/12/2008

//Recalc record structure
void
TPhase::RecCalc( const char *key )
{
    bool getDCC = false;

    if( php->PphC == PH_AQUEL &&
    	( php->sol_t[SPHAS_TYP] == SM_AQSIT || php->sol_t[SPHAS_TYP] == SM_AQPITZ ) )
    {  // refreshing lists for SIT coefficients
       MakeCatAnLists( true, true, true );
    }

    if( php->PphC == PH_AQUEL && ( php->sol_t[SPHAS_TYP] == SM_AQSIT
    		|| php->sol_t[SPHAS_TYP] == SM_AQPITZ ) &&
        php->nxAn[0] <= 0 && php->nxCat[0] <=0 )
    {
        getDCC = vfQuestion(window(), GetName(),
          "For SIT or Pitzer model, please remake the record if not yet done.\n"
           "Proceed with extracting DC parameters (Y)?");
        if(!getDCC)
           return;
    }

    if( ( php->PphC == PH_FLUID || php->PphC == PH_LIQUID ||
          php->PphC == PH_GASMIX ) && php->scoef )
    {
        getDCC = vfQuestion(window(), GetName(),
          "For cubic EoS models for gas/fluid mixtures: \n"
           "Proceed with extracting EoS parameters from DComp records (Y)?");
        if(!getDCC)
           return;
    }

    CalcPhaseRecord( /*getDCC*/ );
    SetString("PH_solm   PHASE-solution model OK");
    TCModule::RecCalc(key);
}

void TPhase::RecordPrint(const char* /*key_*/)
{
   // generate input
    TCIntArray aDCused;
    int i, cnt;
    aDCused.clear();
    for( i=0, cnt=0; i<php->nDC; i++ )
    {
       // test to exist of DCOMP or REACDC record later
       // only 3 fields
       std::string key = char_array_to_string( php->SM[i], DC_RKLEN);

       if( key.find("Mg") == std::string::npos )
       { aDCused.push_back(cnt); cnt++; }
       else
         aDCused.push_back(-1);
    } // i

    // compress
    CompressRecord( cnt, aDCused );

    int res = vfQuestion3(window(), "Question",
                          "Output to file (Yes) or use print script (No)?",
                          "Yes", "No", "Cancel");
    if( res == VF3_1 )
    {
        fstream f(GemsSettings::with_directory("noMg-test.txt"), ios::out);
        ErrorIf( !f.good() , GetName(), "File write error");

        aObj[o_reckey]->SetPtr( const_cast<void*>(static_cast<const void *>("test")));
        aObj[o_reckey]->toTXT(f);
        for(int no=db->GetObjFirst(); no<db->GetObjFirst()+db->GetObjCount();  no++)
            aObj[no]->toTXT(f);

        ErrorIf( !f.good() , GetName(), "Filewrite error");

    }

}

const char* TPhase::GetHtml()
{
   return GM_PHASE_HTML;
}

void TPhase::CopyRecords( const char * prfName, TCStringArray& aPHnoused,
                          TCStringArray& aPHtmp, elmWindowData el_data,
                          phSetupData st_data, std::set<std::string>& SDlist )
{
    TCIntArray anR;
    TCIntArray aDCused;
    TCStringArray aPHkey;
    aPHnoused.clear();

    // open selected kernel files
    //db->OpenOnlyFromList(el_data.flNames);
    int fnum_ = db->GetOpenFileNum(prfName);

    // delete the equvalent keys
    TCStringArray aICkey_new;         // 30/11/2006
    aICkey_new.clear();

    // get list of records
    db->GetKeyList( "*:*:*:*:*:", aPHkey, anR );

    //  test&copy  selected records
    // ( add to last key field first symbol from prfname )
    uint j;
    int i, cnt;
    bool nRec;
    const char *pKey1;//, *pKey4;
    std::string key_from_template;
    bool compressible_record;
    for(uint ii=0; ii<aPHkey.size(); ii++ )
    {
        // compare keys for template project
        uint jj;
        compressible_record = false;
        if(!memcmp(key_from_template.c_str(), aPHkey[ii].c_str(),
                    PH_RKLEN-MAXPHGROUP ))
            continue;
        key_from_template.clear();
        for( jj=0; jj<aPHtmp.size(); jj++ )
            if( !memcmp( aPHtmp[jj].c_str(), aPHkey[ii].c_str(),
                         PH_RKLEN-MAXPHGROUP ))
                break;
        if( jj<aPHtmp.size() )
        {
            key_from_template = aPHtmp[jj];
            //continue;
        }

        // Sorting out phase recs using setup in Elements and SetFilter dialogs
        pKey1 = aPHkey[ii].c_str();
        // Phase Filters
        if( !el_data.flags[cbAqueous_] && pKey1[0] == 'a' )
            continue;
        if( !el_data.flags[cbGaseous_] && ( pKey1[0] == 'g') )
            continue;
        if( !el_data.flags[cbFluid_] && ( pKey1[0] == 'f') )
            continue;
        if( !el_data.flags[cbPlasma_] && ( pKey1[0] == 'p') )
            continue;
        if( !el_data.flags[cbSolids_] && ( pKey1[0] == 's') )
            continue;
        if( !el_data.flags[cbSindis_] && ( pKey1[0] == 'd') )
            continue;
        if( !el_data.flags[cbLiquid_] && ( pKey1[0] == 'l') )
            continue;
        if( !el_data.flags[cbSimelt_] && ( pKey1[0] == 'm') )
            continue;
        if( !el_data.flags[cbSorption_] && ( pKey1[0] == 'x' || pKey1[0] == 'i' || pKey1[0] == 'z' ) )
            continue;
        if( !el_data.flags[cbPolyel_] && ( pKey1[0] == 'y') )
            continue;
        if( !el_data.flags[cbHcarbl_] && ( pKey1[0] == 'h') )
            continue;

        // test the same component (overload) 30/11/2006
        std::string stt = aPHkey[ii].substr(0,MAXSYMB+MAXPHSYMB+MAXPHNAME+MAXSYMB);
        for( j=0; j<aICkey_new.size(); j++ )
            if( stt ==  aICkey_new[j])
                break;
        if( j<aICkey_new.size() )
            continue;

        // Read the record here
        RecInput( aPHkey[ii].c_str() );

        // cbSolutions - multi-component, non-gas, non-electrolyte
        if( el_data.flags[cbSolutions_] && ( pKey1[0] != 'a'
                                             && pKey1[0] != 'g' && pKey1[0] != 'f' && pKey1[0] != 'i' && pKey1[0] != 'z'
                                             && pKey1[0] != 'p' && pKey1[0] != 'x' && pKey1[0] != 'h' ) )
            if( php->nDC > 1 )
                continue;

        // Copy non-ideal phases?
        //    if( !st_data.flags[PHcopyN_] && php->nDC > 1 &&
        //        php->sol_t[0] != 'N' && php->sol_t[0] !='I'  )
        //      continue;

        // Test existence of DComp/ReacDC records
        aDCused.clear();
        for( i=0, cnt=0; i<php->nDC; i++ )
        {
            // test to exist of DCOMP or REACDC record later
            // only 3 fields
            std::string key = char_array_to_string( php->SM[i], DC_RKLEN);
            if( php->DCS[i] == SRC_DCOMP )
                nRec = rt[RT_DCOMP]->FindPart( php->SM[i], 3 );
            else
                nRec = rt[RT_REACDC]->FindPart( php->SM[i], 3 );
            if( nRec )
            { aDCused.push_back(cnt); cnt++; }
            else
                aDCused.push_back(-1);
        } // i

        if( cnt < php->nDC && !( !st_data.flags[PHcopyF_] && cnt > 1  )) // copy  that retain full
        {
            if( st_data.flags[PHcopyD_] && php->nDC > 1 && cnt > 0  )
                aPHnoused.push_back( aPHkey[ii] );
            continue;
        }
        // overwrite compressed even all dcomps  if( cnt < php->nDC ) // added 14/12/12 test for skipping incompressible phases-solutions
        {
            switch( php->sol_t[SGM_MODE] )
            {
            case SM_IDEAL: break;  //I
            case SM_STNGAM: // S
                switch( php->sol_t[SPHAS_TYP] )
                {
                case SM_IDEAL: // =  'I',	// ideal solution or single-component phase
                case SM_BERMAN: // = 'B',    // built-in multicomponent microscopic (a)symmetric solid-solution model (reserved)
                case SM_CEF: //  = '$',    //     built-in multicomponent multisite solid-solution model (CALPHAD)
                    //             case SM_REDKIS: // = 'G', 	// built-in binary Guggenheim (Redlich-Kister) solid-solution model
                    //             case SM_MARGB: // = 'M',	// built-in binary Margules solid-solutions (subregular)
                    //             case SM_MARGT: // = 'T',	// built-in ternary Margules solid-solution (regular)
                case SM_VANLAAR: // = 'V',	// built-in multi-component Van Laar solid-solution model
                case SM_GUGGENM: // = 'K',	// built-in multi-component Guggenheim solid-solution model
                case SM_REGULAR: // = 'R',	// built-in multi-component Regular solid-solution model
                case SM_NRTLLIQ: // = 'L',	// built-in multi-component NRTL model for liquid solutions
                case SM_WILSLIQ: // = 'W',	// built-in multi-component Wilson model for liquid solutions
                case SM_CGFLUID: // = 'F',	// built-in multi-component Churakov-Gottschalk (CG) fluid EoS model
                case SM_PRFLUID: // = 'P',	// built-in Peng-Robinson-Stryjek-Vera (PRSV) fluid EoS model
                    //             case SM_PCFLUID: // = '5',   // built-in perturbed-chain statistical-association (PCSAFT) fluid EoS model (reserved)
                case SM_STFLUID: // = '6',   // built-in Sterner-Pitzer (STP) fluid EoS model
                case SM_PR78FL: // = '7',	// built-in Peng-Robinson (PR78) fluid EoS model
                case SM_CORKFL: // = '8',    // built-in compensated Redlich-Kwong (CORK) fluid EoS model
                    //             case SM_REFLUID: // = '9',   // built-in reference EoS fluid model (reserved)
                case SM_SRFLUID: // = 'E',	// built-in Soave-Redlich-Kwong (SRK) fluid EoS model
                case SM_AQDAV: // = 'D',	// built-in Davies model (with 0.3) for aqueous electrolytes
                case SM_AQDH1: // = '1',	// built-in Debye-Hueckel limiting law for aqueous electrolytes
                case SM_AQDH2: // = '2',	// built-in 2-term Debye-Hueckel model for aqueous electrolytes
                case SM_AQDH3: // = '3',	// built-in 3-term Debye-Hueckel model for aqueous electrolytes (Karpov version)
                case SM_AQDHH: // = 'H',	// built-in 3-term Debye-Hueckel model for aqueous electrolytes (Helgeson version)
                case SM_AQDHS: // = 'Y',	// built-in 3-term Debye-Hueckel model for aqueous electrolytes (Shvarov version)
                case SM_AQSIT: // = 'S',	// built-in SIT model for aqueous electrolytes
                case SM_AQEXUQ: // = 'Q',    // built-in extended UNIQUAC model for aqueous electrolytes
                case SM_AQPITZ: // = 'Z',    // built-in Pitzer HMW model for aqueous electrolytes
                    //             case SM_AQMIX: // = 'C',     // built-in mixed-solvent aqueous Debye-Hueckel model (reserved)
                    //             case SM_AQELVIS: // = 'J',   // built-in modified extended UNIQUAC model (ELVIS) for aqueous electrolytes (reserved)
                    //             case SM_IONEX: // = 'X',     // ion exchange (Donnan, Nikolskii) (reserved)
                case SM_SURCOM: // = 'A',	// models of surface complexation at solid-aqueous interface
                    //             case SM_USERDEF: // = 'U',	// user-defined mixing model (scripts in Phase record)
                    //             case SM_OTHER: // = 'O',	// other built-in phase-specific models of non-ideal solutions (selected by phase name)
                    compressible_record = true;
                    break;
                default:
                    if( cnt < php->nDC )
                    {
                    if( st_data.flags[PHcopyD_] && php->nDC > 1 && cnt > 0  )
                        aPHnoused.push_back( aPHkey[ii] );
                    continue;
                    }
                }
                break;
            case SM_NOSTGAM: // N
                if( cnt < php->nDC )
                {
                  if( php->sol_t[DCOMP_DEP] != 'N' || php->sol_t[SPHAS_DEP] != 'N' )
                  {
                    if( st_data.flags[PHcopyD_] && php->nDC > 1 && cnt > 0  )
                        aPHnoused.push_back( aPHkey[ii] );
                    continue;
                  }
                }
                break;
            }
        }

        if( !key_from_template.empty() )
        {
            if(compressible_record)
            {
                CompressRecord( cnt, aDCused );
                RecSave(key_from_template.c_str());
            }
        }
        else
        {
            // !!! changing record key
            std::string str= std::string(db->FldKey( 4 ), 0, db->FldLen( 4 ));
            ChangeforTempl( str, st_data.from_templ,
                            st_data.to_templ, db->FldLen( 4 ));
            str += ":";
            std::string str1 = std::string(db->FldKey( 3 ), 0, db->FldLen( 3 ));
            strip( str1 );
            str = str1 + ":" + str;
            str1 = std::string(db->FldKey( 2 ), 0, db->FldLen( 2 ));
            strip( str1 );
            str = str1 + ":" + str;
            str1 = std::string(db->FldKey( 1 ), 0, db->FldLen( 1 ));
            strip( str1 );
            str = str1 + ":" + str;
            str1 = std::string(db->FldKey( 0 ), 0, db->FldLen( 0 ));
            strip( str1 );
            str = str1 + ":" + str;
            CompressRecord( cnt, aDCused );
            //Point SaveRecord
            if( AddRecordTest( str.c_str(), fnum_ ))
            {  aICkey_new.push_back( stt );  // 30/11/2006
                for(int isd=0; isd<php->Nsd; isd++)
                {
                    std::string sdkey = char_array_to_string( php->sdref[isd],V_SD_RKLEN);
                    strip( sdkey );
                    SDlist.insert( sdkey );
                }
            }
        }
    }

    // close all no project files
    TCStringArray names1;
    names1.push_back(prfName);
    db->OpenOnlyFromList(names1);
}

bool TPhase::CompressRecord( int nDCused, TCIntArray& DCused, const TCStringArray& sys_form_array, bool onlyIPX )
{
    int ii, nDCnew =0;
    int ncpNnew;


    if(nDCused == php->nDC ) // all DComp/ReacDC records exist
        return true;

    if( !onlyIPX )
    {
        if( php->PpEq != S_OFF && php->PdEq != S_OFF ) // do not compress if scripts
            return false;

        for( nDCnew=0, ii=0; ii<php->nDC; ii++ )
        {
            if( DCused[ii] == -1 )
                continue;

            memcpy( php->SM[nDCnew], php->SM[ii], DC_RKLEN );
            php->DCS[nDCnew] = php->DCS[ii];
            php->DCC[nDCnew] = php->DCC[ii];
            if( php->Psco == S_ON )
                copyValues( php->scoef+nDCnew*php->nscM, php->scoef+ii*php->nscM, php->nscM );

            if( php->PCDc == S_ON )
                copyValues( php->CDc+nDCnew*php->nCDc, php->CDc+ii*php->nCDc, php->nCDc );
            if( php->PIsoC == S_ON )
                copyValues( php->IsoP+nDCnew*php->nIsoC, php->IsoP+ii*php->nIsoC, php->nIsoC );
            if( php->PsDiS == S_ON )
                copyValues( php->xSmD+nDCnew*php->nSiT, php->xSmD+ii*php->nSiT, php->nSiT );
            if( php->PumpCon == S_ON )
                copyValues( php->umpCon+nDCnew*php->numpC, php->umpCon+ii*php->numpC, php->numpC );
            if( php->Pdqf == S_ON )
                copyValues( php->DQFc+nDCnew*php->ndqf, php->DQFc+ii*php->ndqf, php->ndqf );
            if( php->Pdqf == S_ON )
                copyValues( php->rcpc+nDCnew*php->nrcp, php->rcpc+ii*php->nrcp, php->nrcp );

            if( php->PFsiT != S_OFF )
            {
                copyValues( php->SATC[nDCnew], php->SATC[ii], MCAS );
                copyValues( php->MaSdj[nDCnew], php->MaSdj[ii], DFCN );
            }
            nDCnew++;
        }

        php->nDC  = nDCnew;
        // realloc memory
        php->SM = static_cast<char (*)[DC_RKLEN]>(aObj[ o_phsm ]->Alloc( php->nDC, 1, DC_RKLEN ));
        php->DCS = static_cast<char *>(aObj[ o_phdcs ]->Alloc( php->nDC, 1, A_ ));
        php->DCC = static_cast<char *>(aObj[ o_phdcc ]->Alloc( php->nDC, 1, A_ ));
        if( php->Psco == S_ON )
            php->scoef = static_cast<float *>(aObj[ o_phscoef]->Alloc( php->nDC, php->nscM, F_ ));
        if( php->PFsiT != S_OFF )
        {
            php->SATC =  static_cast<char (*)[MCAS]>(aObj[ o_phsatc ]->Alloc( php->nDC, MCAS, A_));
            php->MaSdj = static_cast<float (*)[DFCN]>(aObj[ o_phmasdj ]->Alloc( php->nDC, DFCN, F_));
        }
        if( php->PCDc == S_ON )
            php->CDc =  static_cast<float *>(aObj[ o_phcdc]->Alloc( php->nDC, php->nCDc, F_ ));
        if( php->PIsoC == S_ON )
            php->IsoP =  static_cast<float *>(aObj[ o_phisop]->Alloc( php->nDC, php->nIsoC, F_ ));
        if( php->PsDiS == S_ON )
            php->xSmD =  static_cast<short *>(aObj[  o_phxsmd ]->Alloc( php->nDC, php->nSiT, I_ ));
        if( php->PumpCon == S_ON )
            php->umpCon =  static_cast<float *>(aObj[ o_phumpcon]->Alloc( php->nDC, php->numpC, F_ ));
        if( php->Pdqf == S_ON )
            php->DQFc =  static_cast<float *>(aObj[ o_phdqfc]->Alloc( php->nDC, php->ndqf, F_ ));
        if( php->Pdqf == S_ON )
            php->rcpc =  static_cast<float *>(aObj[ o_phrcpc]->Alloc( php->nDC, php->nrcp, F_ ));
    }

    if( php->Ppnc == S_ON && php->npxM > 0 )
    {
        switch(php->sol_t[SPHAS_TYP])
        {
        case SM_BERMAN:
        case SM_CEF:
        {
            TCStringArray form_array = sys_form_array;
            if(!onlyIPX) // cpmpressed list
                DCused.clear();

            if( form_array.size() <= 0)   // not difined before
                form_array = readFormulaes( DCused);
            ncpNnew = CompressSublattice( form_array );
        }
            break;
        case SM_VANLAAR:
        case SM_GUGGENM:
        case SM_REGULAR:
            // Aqueous solutions
        case SM_AQSIT:
        case SM_AQEXUQ:
        case SM_AQPITZ:
        case SM_AQMIX:
        case SM_AQELVIS:
            // These gas/fluid models can optionally contain ipicl list if they contain pnc and ipxt arrays
        case SM_NRTLLIQ:
        case SM_WILSLIQ:
        case SM_CGFLUID:
        case SM_PRFLUID:
        case SM_PCFLUID:
        case SM_STFLUID:
        case SM_PR78FL:
        case SM_CORKFL:
        case SM_REFLUID:
        case SM_SRFLUID:
        default:
            ncpNnew = CompressDecomp(nDCnew , DCused);
            break;
        }

        php->ncpN = ncpNnew;
        php->pnc = static_cast<float *>(aObj[ o_phpnc ]->Alloc( php->ncpN, php->ncpM, F_ ));
        php->ipxt = static_cast<short *>(aObj[ o_phpxres ]->Alloc( php->ncpN, php->npxM, I_));
    }

    //dyn_new();
    return true;
}

int TPhase::CompressDecomp(int , const TCIntArray &DCused)
{
    int ii, jj, ncpNnew;
    int DCndx;

    for( ncpNnew=0, ii=0; ii<php->ncpN; ii++)
    {
      for(jj=0; jj<php->npxM; jj++)
      {
        DCndx = php->ipxt[ii*php->npxM+jj];
        if( DCndx  < 0  ) // for Pitzer model
          continue;
        ErrorIf(DCndx>=DCused.size(), "CompressDecomp", char_array_to_string( php->pst_, MAXPHNAME)+" illegal index in phase");
        DCndx =  DCused[static_cast<uint>(DCndx)];
        if( DCndx < 0  ) // non-existent component
          break;
        //if( !onlyIPX )
        php->ipxt[ii*php->npxM+jj] = static_cast<short>(DCndx);
      }
      if( jj<php->npxM ) // row with a non-existent component
        continue;

      copyValues( php->ipxt+ncpNnew*php->npxM, php->ipxt+ii*php->npxM, php->npxM );
      copyValues( php->pnc+ncpNnew*php->ncpM, php->pnc+ii*php->ncpM, php->ncpM );
      if( php->ipicl )
         memcpy( php->ipicl[ncpNnew], php->ipicl[ii], MAXDCNAME );
      ncpNnew++;
    }

   if(php->ipicl )
    php->ipicl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_phipicl]->Alloc( ncpNnew, 1, MAXDCNAME ));
   return  ncpNnew;
}

int TPhase::CompressSublattice( const TCStringArray& form_array )
{
    // from CalcPhaseRecord( /*getDCC*/ );
    if( !(php->PphC == PH_SINCOND || php->PphC == PH_SINDIS
          || php->PphC == PH_LIQUID || php->PphC == PH_SIMELT) )   // added DK 29.03.2012
        return 0;

    TCStringArray old_lsMoi = getSavedLsMoi();
    MakeSublatticeLists( form_array  );

    ErrorIf( static_cast<int>(old_lsMoi.size()) < php->nMoi, char_array_to_string( php->pst_, MAXPHNAME),
             "Please, recalculate phase record before execution.");

    TCIntArray  Moiused;
    size_t i1;
    short ii, jj;
    int ncpNnew, DCndx;

    for( i1=0; i1<old_lsMoi.size(); i1++ )
    {
        for( jj=0; jj<php->nMoi; jj++)
        {
            if( old_lsMoi[i1] == char_array_to_string( php->lsMoi[jj], MAXDCNAME) )
                break;
        }
        if( jj == php->nMoi )
            Moiused.push_back(-1);
        else
            Moiused.push_back(jj);
    }

    for( ncpNnew=0, ii=0; ii<php->ncpN; ii++)
    {
        jj=0;
        if( php->sol_t[SPHAS_TYP] == SM_BERMAN )
            jj=1;
        for(; jj<php->npxM; jj++)
        {
            DCndx = php->ipxt[ii*php->npxM+jj];
            if( DCndx  < 0 || DCndx >= static_cast<int>(Moiused.size()) )
                continue;
            DCndx =  Moiused[static_cast<size_t>(DCndx)];
            if( DCndx  < 0  ) // non-existent component
                break;
            php->ipxt[ii*php->npxM+jj] = static_cast<short>(DCndx);
        }
        if( jj<php->npxM ) // row with a non-existent component
            continue;

        copyValues( php->ipxt+ncpNnew*php->npxM, php->ipxt+ii*php->npxM, php->npxM );
        copyValues( php->pnc+ncpNnew*php->ncpM, php->pnc+ii*php->ncpM, php->ncpM );
        memcpy( php->ipicl[ncpNnew], php->ipicl[ii], MAXDCNAME );
        ncpNnew++;
    }

    php->ipicl =  static_cast<char (*)[MAXDCNAME]>(aObj[ o_phipicl]->Alloc( ncpNnew, 1, MAXDCNAME ));
    return  ncpNnew;
}


TCStringArray TPhase::readFormulaes( const TCIntArray&  DCused) const
{
    int  i;
    char dcn[MAXRKEYLEN];
    time_t crt;
    TCStringArray form_array;
    bool onlyused = static_cast<int>(DCused.size())>=php->nDC;

    TDComp* aDC=dynamic_cast<TDComp *>(aMod[RT_DCOMP].get());
    TReacDC* aRDC=dynamic_cast<TReacDC *>(aMod[RT_REACDC].get());
    aDC->ods_link(0);
    aRDC->ods_link(0);

    memset( dcn, 0, MAXRKEYLEN );
    for( i=0; i<php->nDC; i++ )
    {
        if( onlyused &&  DCused[static_cast<uint>(i)] < 0 ) // non-existent component
          continue;

        memcpy( dcn, php->SM[i], DC_RKLEN );
        dcn[DC_RKLEN]=0;
        /* Read record and extract data */
        if( php->DCS[i] == SRC_DCOMP )
        {
            aDC->TryRecInp( dcn, crt, 0 );
            form_array.push_back( std::string(aDC->dcp->form,0,MAXFORMULA));
        }
        else
            if( php->DCS[i] == SRC_REACDC )
            {
                aRDC->TryRecInp( dcn, crt, 0 );
                form_array.push_back( std::string(aRDC->rcp->form,0,MAXFORMULA));
            }
    }  // i
    return  form_array;
}

TCStringArray TPhase::getSavedLsMoi() const
{
    TCStringArray lsMoiOld;
    if( php->dEq && *php->dEq=='$' )
    {
       lsMoiOld = split( std::string(php->dEq,1, std::string::npos) ,";");
    }
    return lsMoiOld;
}

// ----------------- End of m_phase.cpp -------------------------

