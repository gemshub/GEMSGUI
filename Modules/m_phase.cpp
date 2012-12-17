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
// This file may be distributed under the terms of GEMS3 Development
// Quality Assurance Licence (GEMS3.QAL)
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

TPhase* TPhase::pm;


TPhase::TPhase( int nrt ):
        TCModule( nrt )
{
    nQ = 1;
    aFldKeysHelp.Add("Code of phase state { a g f p l m h s d x }");
    aFldKeysHelp.Add("Group identifier for such phases (letters, digits)");
    aFldKeysHelp.Add("Name of this phase definition (letters, digits)");
    aFldKeysHelp.Add("Phase class { c d l gm ss ssd ls aq xsa xc }");
    aFldKeysHelp.Add("Comment to phase definition");
    php=&ph[0];
    set_def();
    start_title = " Definition of thermodynamic phase ";
}


// link values to objects
void TPhase::ods_link( int q)
{
    ErrorIf( q > nQ, GetName(), "E00PHrem: Invalid link q>nQ");

    aObj[ o_phsolt].SetPtr( ph[q].sol_t );
    aObj[ o_phps].SetPtr(  &ph[q].PphC );  // a 6
    aObj[ o_phndc].SetPtr( &ph[q].nDC );   // i 2
    aObj[ o_phpncp].SetPtr(&ph[q].ncpN );  // i 2
    aObj[ o_phnpx].SetPtr( &ph[q].npxM );  // i 1  added 07.12.2006 KD
    aObj[ o_phnsc].SetPtr( &ph[q].nscM );  // i 1  changed 07.12.2006 KD
    aObj[ o_phnsit].SetPtr(&ph[q].NsiT );  // i 2

// new 06/06/12
    aObj[ o_phkin_t].SetPtr(  ph[q].kin_t );  // a 6
    aObj[ o_phpdqf].SetPtr(  &ph[q].Pdqf );  // a 4
    aObj[ o_phpeipc].SetPtr(  &ph[q].PEIpc );  // a 6
    aObj[ o_phprpcon].SetPtr(  &ph[q].PrpCon );  // a 4
    aObj[ o_phnlph].SetPtr(  &ph[q].nlPh );  // i 4
    aObj[ o_phnumpc].SetPtr(  &ph[q].nPRk );  // i 4
    aObj[ o_phnei1].SetPtr(  &ph[q].nEIl );  // i 4
    aObj[ o_phnisoc].SetPtr(  &ph[q].nIsoC );  // i 4
    aObj[ o_phvpor].SetPtr(  &ph[q].Vpor );  // f 4
    aObj[ o_phpsdc].SetPtr(  &ph[q].psdC );  // f 4

    /* Record 22 fields */
    aObj[ o_phstr].SetPtr(  ph[q].sol_t );
    aObj[ o_phname].SetPtr( ph[q].name );
    aObj[ o_phnotes].SetPtr(ph[q].notes );
    aObj[ o_phdim].SetPtr( &ph[q].nDC );     // i 8
    aObj[ o_phdisep].SetPtr(&ph[q].Asur );   // f 8
    // NsiT
    aObj[ o_phscmc].SetPtr(  ph[q].SCMC );
    aObj[ o_phscmc].SetDim( ph[q].NsiT, 1 );
    aObj[ o_phxfiec].SetPtr( ph[q].XfIEC);
    aObj[ o_phxfiec].SetDim( ph[q].NsiT, 1);
    aObj[ o_phmsdt].SetPtr(  ph[q].MSDT );
    aObj[ o_phmsdt].SetDim( ph[q].NsiT, 2 );
    aObj[ o_phcapt].SetPtr(  ph[q].CapT );
    aObj[ o_phcapt].SetDim( ph[q].NsiT, 2 );
    aObj[ o_phfsit].SetPtr(  ph[q].FsiT );
    aObj[ o_phfsit].SetDim( ph[q].NsiT, 1 );
    //nDC
    aObj[ o_phsatc].SetPtr(  ph[q].SATC );
    aObj[ o_phsatc].SetDim( ph[q].nDC,  MCAS );
    aObj[ o_phmasdj].SetPtr( ph[q].MaSdj);
    aObj[ o_phmasdj].SetDim( ph[q].nDC, DFCN );
    aObj[ o_phpxres].SetPtr( ph[q].ipxt);  // changed 07.12.2006 KD
// if(ph[q].ipxt)
  aObj[ o_phpxres].SetDim( ph[q].ncpN, ph[q].npxM );
// else
//  aObj[ o_phpxres].SetDim( 0, 0 );  // Bugfix 02.06.2009 DK
    aObj[ o_phsm].SetPtr(   ph[q].SM[0] );
    aObj[ o_phsm].SetDim( ph[q].nDC, 1 );
    aObj[ o_phdcc].SetPtr(  ph[q].DCC );
    aObj[ o_phdcc].SetDim( ph[q].nDC, 1 );
    aObj[ o_phdcs].SetPtr(  ph[q].DCS );
    aObj[ o_phdcs].SetDim( ph[q].nDC, 1 );

    aObj[ o_phpnc].SetPtr(  ph[q].pnc );
    aObj[ o_phpnc].SetDim( ph[q].ncpN, ph[q].ncpM );
    aObj[ o_phscoef].SetPtr( ph[q].scoef );
    aObj[ o_phscoef].SetDim( ph[q].nDC, ph[q].nscM ); // changed 07.12.2006  KD
    // ??????
    aObj[ o_phpeq].SetPtr(  ph[q].pEq );
    //   aObj[ o_phpeq].SetDim( 1, strlen(ph[q].pEq));
    aObj[ o_phdeq].SetPtr(  ph[q].dEq );
    //   aObj[ o_phdeq].SetDim( 1, strlen(ph[q].dEq));
    // Nsd
    aObj[ o_phsdref].SetPtr( ph[q].sdref);
    aObj[ o_phsdref].SetDim( ph[q].Nsd, 1 );
    aObj[ o_phsdval].SetPtr( ph[q].sdval);
    aObj[ o_phsdval].SetDim( ph[q].Nsd, 1 );
    /// ??????
    aObj[o_phtprn].SetPtr( ph[q].tprn );

    // Added for SIT and Pitzer aqueous models implementation
    aObj[o_ph_w_lsc].SetPtr( ph[q].lsCat );
    aObj[o_ph_w_lsc].SetDim( ph[q].nCat, 1 );
    aObj[o_ph_w_lsa].SetPtr( ph[q].lsAn );
    aObj[o_ph_w_lsa].SetDim( ph[q].nAn, 1 );
    aObj[o_ph_w_lsn].SetPtr( ph[q].lsNs );
    aObj[o_ph_w_lsn].SetDim( ph[q].nNs, 1 );
    aObj[o_ph_w_nxc].SetPtr( ph[q].nxCat );
    aObj[o_ph_w_nxc].SetDim( ph[q].nCat, 1 );
    aObj[o_ph_w_nxa].SetPtr( ph[q].nxAn );
    aObj[o_ph_w_nxa].SetDim( ph[q].nAn, 1 );
    aObj[o_ph_w_nxn].SetPtr( ph[q].nxNs );
    aObj[o_ph_w_nxn].SetDim( ph[q].nNs, 1 );
    // added for multi-site mixin models implementation
aObj[o_ph_lsform].SetPtr( ph[q].lsForm );
aObj[o_ph_lsform].SetDim( ph[q].nDC, 1 );
aObj[o_ph_nsub].SetPtr( &ph[q].nSub );
aObj[o_ph_lsmoi].SetPtr( ph[q].lsMoi );
aObj[o_ph_lsmoi].SetDim( 1, ph[q].nMoi );
aObj[o_ph_ocpn].SetPtr( ph[q].OcpN );
aObj[o_ph_ocpn].SetDim( ph[q].nDC, ph[q].nMoi );
aObj[o_ph_nxsub].SetPtr( ph[q].nxSub );
aObj[o_ph_nxsub].SetDim( ph[q].nDC, ph[q].nMoi );
//    aObj[ o_phtprn].SetDim( 1,strlen(ph[q].tprn));

// new record 06/06/12
aObj[ o_phstr2].SetPtr(  &ph[q].Pdqf ); // a 20
aObj[ o_phdim2].SetPtr( &ph[q].nlPh );   // i 16
aObj[ o_phfloat2].SetPtr(&ph[q].Vpor );  // f 8

//dynamic
aObj[ o_phxsmd].SetPtr(  ph[q].xSmD );
aObj[ o_phxsmd].SetDim( ph[q].nDC, ph[q].NsiT );
aObj[ o_phxfaces].SetPtr(  ph[q].ocPRk );
aObj[ o_phxfaces].SetDim( ph[q].nPRk, 1 );

aObj[ o_phlphc1].SetPtr(  ph[q].lPhc );
aObj[ o_phlphc1].SetDim( ph[q].nlPh, ph[q].nlPc );
aObj[ o_phdqfc].SetPtr(  ph[q].DQFc );
aObj[ o_phdqfc].SetDim( ph[q].nDC, ph[q].ndqf );
aObj[ o_phrcpc].SetPtr(  ph[q].rcpc );
aObj[ o_phrcpc].SetDim( ph[q].nDC, ph[q].nrcp );

aObj[ o_pheipc].SetPtr(  ph[q].EIpc );
aObj[ o_pheipc].SetDim( ph[q].nEIl, ph[q].nEIp );
aObj[ o_phcdc].SetPtr(  ph[q].CDc );
aObj[ o_phcdc].SetDim( ph[q].nDC, ph[q].nCDc );
aObj[ o_phisop].SetPtr(  ph[q].IsoP );
aObj[ o_phisop].SetDim( ph[q].nDC, ph[q].nIsoC );
aObj[ o_phisos].SetPtr(  ph[q].IsoS );
aObj[ o_phisos].SetDim( ph[q].NsiT, ph[q].nIsoS );
aObj[ o_phfsac].SetPtr(  ph[q].feSAr );
aObj[ o_phfsac].SetDim( 1, ph[q].nPRk );
aObj[ o_phrpcon].SetPtr(  ph[q].rpCon );
aObj[ o_phrpcon].SetDim( ph[q].nPRk, ph[q].nrpC );
aObj[ o_phumpcon].SetPtr(  ph[q].umpCon );
aObj[ o_phumpcon].SetDim( ph[q].nDC, ph[q].numpC );

aObj[ o_phlph].SetPtr(  ph[q].lPh );
aObj[ o_phlph].SetDim( ph[q].nlPh, 1 );
aObj[ o_phldc].SetPtr(  ph[q].lDCr );
aObj[ o_phldc].SetDim( ph[q].nSkr, 1 );

aObj[ o_phdcpcl].SetPtr(  ph[q].dcpcl );
aObj[ o_phdcpcl].SetDim( 1, ph[q].nscM );
aObj[ o_phipicl].SetPtr(  ph[q].ipicl );
aObj[ o_phipicl].SetDim( ph[q].ncpN, 1 );
aObj[ o_phipccl].SetPtr(  ph[q].ipccl );
aObj[ o_phipccl].SetDim( 1, ph[q].ncpM );
aObj[ o_phrpkcl].SetPtr(  ph[q].rpkcl );
aObj[ o_phrpkcl].SetDim( 1, ph[q].nrpC );
aObj[ o_phrprcl].SetPtr(  ph[q].rprcl );
aObj[ o_phrprcl].SetDim( ph[q].nPRk, 1 );
aObj[ o_phumpcl].SetPtr(  ph[q].umpcl );
aObj[ o_phumpcl].SetDim( 1, ph[q].numpC );
aObj[ o_phsmcdl].SetPtr(  ph[q].smcDl );
aObj[ o_phsmcdl].SetDim( 1, ph[q].nIsoC );
aObj[ o_phsmcsl].SetPtr(  ph[q].smcSl );
aObj[ o_phsmcsl].SetDim( 1, ph[q].nIsoS );
aObj[ o_pheimpl].SetPtr(  ph[q].eimPl );
aObj[ o_pheimpl].SetDim( 1, ph[q].nCDc );
aObj[ o_pheimll].SetPtr(  ph[q].eimLl );
aObj[ o_pheimll].SetDim( 1, ph[q].nEIp );

aObj[ o_phlphc2].SetPtr(  ph[q].lPhC );
aObj[ o_phlphc2].SetDim( ph[q].nlPh, 1 );
aObj[ o_phisoc].SetPtr(  ph[q].IsoC );
aObj[ o_phisoc].SetDim( 2, ph[q].NsiT );

    php=&ph[q];
}


// set dynamic Objects ptr to values
void TPhase::dyn_set(int q)
{
    ErrorIf( php!=&ph[q], GetName(),
             "E01PHrem: Invalid access to ph in dyn_set()");
    memcpy( php->pst_, rt[nRT].UnpackKey(), PH_RKLEN );
    ph[q].SCMC =  (char *)aObj[ o_phscmc ].GetPtr();
    ph[q].FsiT =  (float *)aObj[ o_phfsit ].GetPtr();
    ph[q].XfIEC = (float *)aObj[ o_phxfiec ].GetPtr();
    ph[q].MSDT  = (float (*)[2])aObj[ o_phmsdt ].GetPtr();
    ph[q].CapT  = (float (*)[2])aObj[ o_phcapt ].GetPtr();
    ph[q].SATC  = (char (*)[MCAS])aObj[ o_phsatc ].GetPtr();
    ph[q].MaSdj = (float (*)[DFCN])aObj[ o_phmasdj ].GetPtr();
    ph[q].ipxt =  (short *)aObj[ o_phpxres ].GetPtr(); // changed 07.12.2006 KD
// For safe use of old Phase records without ipxt table   07.12.2006
if(!ph[q].ipxt )
   php->npxM = 0;
    ph[q].pnc =   (float *)aObj[ o_phpnc ].GetPtr();
    ph[q].scoef = (float *)aObj[ o_phscoef ].GetPtr();
    ph[q].SM =    (char (*)[DC_RKLEN])aObj[ o_phsm ].GetPtr();
    ph[q].DCC =   (char *)aObj[ o_phdcc ].GetPtr();
    ph[q].DCS =   (char *)aObj[ o_phdcs ].GetPtr();
    ph[q].pEq =   (char *)aObj[ o_phpeq ].GetPtr();
    ph[q].dEq =   (char *)aObj[ o_phdeq ].GetPtr();
    ph[q].sdref = (char (*)[V_SD_RKLEN])aObj[ o_phsdref ].GetPtr();
    ph[q].sdval = (char (*)[V_SD_VALEN])aObj[ o_phsdval ].GetPtr();
    ph[q].tprn = (char *)aObj[ o_phtprn ].GetPtr();
    // Added for SIT aqueous model
    ph[q].lsCat = (char (*)[MAXDCNAME])aObj[ o_ph_w_lsc ].GetPtr();
    ph[q].lsAn = (char (*)[MAXDCNAME])aObj[ o_ph_w_lsa ].GetPtr();
    ph[q].lsNs = (char (*)[MAXDCNAME])aObj[ o_ph_w_lsn ].GetPtr();
    ph[q].nxCat = (short *)aObj[ o_ph_w_nxc ].GetPtr();
    ph[q].nxAn = (short *)aObj[ o_ph_w_nxa ].GetPtr();
    ph[q].nxNs = (short *)aObj[ o_ph_w_nxn ].GetPtr();
// added for multi-site mixin models implementation
    moiety_new( ph[q].nDC, ph[q].nMoi, true );

// new record 06/06/12
    ph[q].xSmD =  (short *)aObj[  o_phxsmd ].GetPtr();
    ph[q].ocPRk =  (short *)aObj[ o_phxfaces ].GetPtr();
    ph[q].lPhc =  (float *)aObj[ o_phlphc1].GetPtr();
    ph[q].DQFc =  (float *)aObj[ o_phdqfc].GetPtr();
    ph[q].rcpc =  (float *)aObj[ o_phrcpc].GetPtr();
    ph[q].EIpc =  (float *)aObj[ o_pheipc].GetPtr( );
    ph[q].CDc =  (float *)aObj[ o_phcdc].GetPtr( );
    ph[q].IsoP =  (float *)aObj[ o_phisop].GetPtr( );
    ph[q].IsoS =  (float *)aObj[ o_phisos].GetPtr( );
    ph[q].feSAr =  (float *)aObj[ o_phfsac].GetPtr( );
    ph[q].rpCon =  (float *)aObj[ o_phrpcon].GetPtr( );
    ph[q].umpCon =  (float *)aObj[ o_phumpcon].GetPtr( );
    ph[q].lPh =  (char (*)[PH_RKLEN])aObj[ o_phlph].GetPtr( );
    ph[q].lDCr =  (char (*)[DC_RKLEN])aObj[ o_phldc].GetPtr( );
    ph[q].dcpcl =  (char (*)[MAXDCNAME])aObj[ o_phdcpcl].GetPtr( );
    ph[q].ipicl =  (char (*)[MAXDCNAME])aObj[ o_phipicl].GetPtr( );
    ph[q].ipccl =  (char (*)[MAXDCNAME])aObj[ o_phipccl].GetPtr( );
    ph[q].rpkcl =  (char (*)[MAXDCNAME])aObj[ o_phrpkcl].GetPtr( );
    ph[q].rprcl =  (char (*)[MAXDCNAME])aObj[ o_phrprcl].GetPtr( );
    ph[q].umpcl =  (char (*)[MAXDCNAME])aObj[ o_phumpcl].GetPtr( );
    ph[q].smcDl =  (char (*)[MAXDCNAME])aObj[ o_phsmcdl].GetPtr( );
    ph[q].smcSl =  (char (*)[MAXDCNAME])aObj[ o_phsmcsl].GetPtr( );
    ph[q].eimPl =  (char (*)[MAXDCNAME])aObj[ o_pheimpl].GetPtr( );
    ph[q].eimLl =  (char (*)[MAXDCNAME])aObj[ o_pheimll].GetPtr( );
    ph[q].lPhC =  (char *)aObj[ o_phlphc2].GetPtr( );
    ph[q].IsoC =  (char *)aObj[ o_phisoc].GetPtr( );
}


// free dynamic memory in objects and values
void TPhase::dyn_kill(int q)
{
    ErrorIf( php!=&ph[q], GetName(), "E02PHrem: Invalid access to ph in dyn_kill()");
    ph[q].SCMC =  (char *)aObj[ o_phscmc ].Free();
    ph[q].FsiT =  (float *)aObj[ o_phfsit ].Free();
    ph[q].XfIEC = (float *)aObj[ o_phxfiec ].Free();
    ph[q].MSDT =  (float (*)[2])aObj[ o_phmsdt ].Free();
    ph[q].CapT =  (float (*)[2])aObj[ o_phcapt ].Free();
    ph[q].SATC =  (char (*)[MCAS])aObj[ o_phsatc ].Free();
    ph[q].MaSdj = (float (*)[DFCN])aObj[ o_phmasdj ].Free();
    ph[q].ipxt =  (short *)aObj[ o_phpxres ].Free();  // changed 07.12.2006  KD
    ph[q].pnc =   (float *)aObj[ o_phpnc ].Free();
    ph[q].scoef = (float *)aObj[ o_phscoef ].Free();
    ph[q].SM =    (char (*)[DC_RKLEN])aObj[ o_phsm ].Free();
    ph[q].DCC =   (char *)aObj[ o_phdcc ].Free();
    ph[q].DCS =   (char *)aObj[ o_phdcs ].Free();
    ph[q].pEq =   (char *)aObj[ o_phpeq ].Free();
    ph[q].dEq =   (char *)aObj[ o_phdeq ].Free();
    ph[q].sdref = (char (*)[V_SD_RKLEN])aObj[ o_phsdref ].Free();
    ph[q].sdval = (char (*)[V_SD_VALEN])aObj[ o_phsdval ].Free();
    ph[q].tprn = (char *)aObj[ o_phtprn ].Free();
    ph[q].lsCat = (char (*)[MAXDCNAME])aObj[ o_ph_w_lsc ].Free();
    ph[q].lsAn =  (char (*)[MAXDCNAME])aObj[ o_ph_w_lsa ].Free();
    ph[q].lsNs =  (char (*)[MAXDCNAME])aObj[ o_ph_w_lsn ].Free();
    ph[q].nxCat = (short *)aObj[ o_ph_w_nxc ].Free();
    ph[q].nxAn =  (short *)aObj[ o_ph_w_nxa ].Free();
    ph[q].nxNs =  (short *)aObj[ o_ph_w_nxn ].Free();
// added for multi-site mixin models implementation
    ph[q].lsForm = (char (*)[MAXFORMULA])aObj[o_ph_lsform].Free();
    ph[q].lsMoi = (char (*)[MAXDCNAME])aObj[o_ph_lsmoi].Free();
    ph[q].OcpN =   (float *)aObj[o_ph_ocpn].Free();
    ph[q].nxSub = (short *)aObj[o_ph_nxsub].Free();

    // new record 06/06/12
    ph[q].xSmD =  (short *)aObj[  o_phxsmd ].Free();
    ph[q].ocPRk =  (short *)aObj[ o_phxfaces ].Free();
    ph[q].lPhc =  (float *)aObj[ o_phlphc1].Free();
    ph[q].DQFc =  (float *)aObj[ o_phdqfc].Free();
    ph[q].rcpc =  (float *)aObj[ o_phrcpc].Free();
    ph[q].EIpc =  (float *)aObj[ o_pheipc].Free( );
    ph[q].CDc =  (float *)aObj[ o_phcdc].Free( );
    ph[q].IsoP =  (float *)aObj[ o_phisop].Free( );
    ph[q].IsoS =  (float *)aObj[ o_phisos].Free( );
    ph[q].feSAr =  (float *)aObj[ o_phfsac].Free( );
    ph[q].rpCon =  (float *)aObj[ o_phrpcon].Free( );
    ph[q].umpCon =  (float *)aObj[ o_phumpcon].Free( );
    ph[q].lPh =  (char (*)[PH_RKLEN])aObj[ o_phlph].Free( );
    ph[q].lDCr =  (char (*)[DC_RKLEN])aObj[ o_phldc].Free( );
    ph[q].dcpcl =  (char (*)[MAXDCNAME])aObj[ o_phdcpcl].Free( );
    ph[q].ipicl =  (char (*)[MAXDCNAME])aObj[ o_phipicl].Free( );
    ph[q].ipccl =  (char (*)[MAXDCNAME])aObj[ o_phipccl].Free( );
    ph[q].rpkcl =  (char (*)[MAXDCNAME])aObj[ o_phrpkcl].Free( );
    ph[q].rprcl =  (char (*)[MAXDCNAME])aObj[ o_phrprcl].Free( );
    ph[q].umpcl =  (char (*)[MAXDCNAME])aObj[ o_phumpcl].Free( );
    ph[q].smcDl =  (char (*)[MAXDCNAME])aObj[ o_phsmcdl].Free( );
    ph[q].smcSl =  (char (*)[MAXDCNAME])aObj[ o_phsmcsl].Free( );
    ph[q].eimPl =  (char (*)[MAXDCNAME])aObj[ o_pheimpl].Free( );
    ph[q].eimLl =  (char (*)[MAXDCNAME])aObj[ o_pheimll].Free( );
    ph[q].lPhC =  (char *)aObj[ o_phlphc2].Free( );
    ph[q].IsoC =  (char *)aObj[ o_phisoc].Free( );

}


// realloc dynamic memory
void TPhase::dyn_new(int q)
{
    ErrorIf( php!=&ph[q], GetName(), "E03PHrem: Invalid access to ph in dyn_new()");
    ErrorIf( ph[q].nDC <= 0, GetName(), "E04PHrem: Number of DC in the phase definition <= 0");
    ph[q].SM = (char (*)[DC_RKLEN])aObj[ o_phsm ].Alloc( ph[q].nDC, 1, DC_RKLEN );
    ph[q].DCS = (char *)aObj[ o_phdcs ].Alloc( ph[q].nDC, 1, A_ );
    ph[q].DCC = (char *)aObj[ o_phdcc ].Alloc( ph[q].nDC, 1, A_ );

    if( ph[q].Psco == S_ON )
    {      ph[q].scoef = (float *)aObj[ o_phscoef].Alloc( ph[q].nDC, ph[q].nscM, F_ );
//                ph[q].nscN*ph[q].nscM, F_ );  changed 07.12.2006  KD
          ph[q].dcpcl =  (char (*)[MAXDCNAME])aObj[ o_phdcpcl].Alloc( 1, ph[q].nscM, MAXDCNAME );
    }
    else
    {    ph[q].scoef = (float *)aObj[ o_phscoef ].Free();
        ph[q].dcpcl =  (char (*)[MAXDCNAME])aObj[ o_phdcpcl].Free();
    }


    if( ph[q].Ppnc == S_ON )
    {
       ph[q].pnc = (float *)aObj[ o_phpnc ].Alloc( ph[q].ncpN, ph[q].ncpM, F_ );
       ph[q].ipicl =  (char (*)[MAXDCNAME])aObj[ o_phipicl].Alloc( ph[q].ncpN, 1, MAXDCNAME );
       ph[q].ipccl =  (char (*)[MAXDCNAME])aObj[ o_phipccl].Alloc( 1, ph[q].ncpM, MAXDCNAME );
    }
    else
    {
       ph[q].pnc =   (float *)aObj[ o_phpnc ].Free();
       ph[q].ipicl =  (char (*)[MAXDCNAME])aObj[ o_phipicl].Free();
       ph[q].ipccl =  (char (*)[MAXDCNAME])aObj[ o_phipccl].Free();
    }

    if( ph[q].Ppnc == S_ON && ph[q].npxM > 0 )             // Bugfix 02.06.2009 DK
        ph[q].ipxt = (short *)aObj[ o_phpxres ].Alloc( ph[q].ncpN, ph[q].npxM, I_);
    if( ph[q].npxM <= 0 )
    	ph[q].ipxt = (short *)aObj[ o_phpxres ].Free();
//    	aObj[ o_phpxres].SetDim( 0, 0 );

    if( ph[q].PFsiT == S_ON || ph[q].PFsiT == S_REM )
    {
        ph[q].SCMC =  (char *)aObj[ o_phscmc ].Alloc( ph[q].NsiT, 1, A_);
        ph[q].FsiT =  (float *)aObj[ o_phfsit ].Alloc( ph[q].NsiT, 1, F_);
        ph[q].XfIEC = (float *)aObj[ o_phxfiec ].Alloc( ph[q].NsiT, 1, F_);
        ph[q].MSDT =  (float (*)[2])aObj[ o_phmsdt ].Alloc( ph[q].NsiT, 2, F_);
        ph[q].CapT =  (float (*)[2])aObj[ o_phcapt ].Alloc( ph[q].NsiT, 2, F_);
//        ph[q].SATC =  (char (*)[2])aObj[ o_phsatc ].Alloc( ph[q].nDC, 2, A_);
        ph[q].SATC =  (char (*)[MCAS])aObj[ o_phsatc ].Alloc( ph[q].nDC, MCAS, A_);
//        ph[q].MaSdj = (float *)aObj[ o_phmasdj ].Alloc( ph[q].nDC, 1, F_);
    ph[q].MaSdj = (float (*)[DFCN])aObj[ o_phmasdj ].Alloc( ph[q].nDC, DFCN, F_);
    }
    else
    {
        ph[q].SCMC =  (char *)aObj[ o_phscmc ].Free();
        ph[q].FsiT =  (float *)aObj[ o_phfsit ].Free();
        ph[q].XfIEC = (float *)aObj[ o_phxfiec ].Free();
        ph[q].MSDT =  (float (*)[2])aObj[ o_phmsdt ].Free();
        ph[q].CapT =  (float (*)[2])aObj[ o_phcapt ].Free();
        ph[q].SATC =  (char (*)[MCAS])aObj[ o_phsatc ].Free();
        ph[q].MaSdj = (float (*)[DFCN])aObj[ o_phmasdj ].Free();
    }
    if( ph[q].Nsd )
    {
        ph[q].sdref = (char (*)[V_SD_RKLEN])aObj[ o_phsdref ].Alloc(
                          ph[q].Nsd, 1, V_SD_RKLEN);
        ph[q].sdval = (char (*)[V_SD_VALEN])aObj[ o_phsdval ].Alloc(
                          ph[q].Nsd, 1, V_SD_VALEN);
    }
    else
    {
        ph[q].sdref = (char (*)[V_SD_RKLEN])aObj[ o_phsdref ].Free();
        ph[q].sdval = (char (*)[V_SD_VALEN])aObj[ o_phsdval ].Free();
    }
    if( ph[q].pEq && ph[q].PpEq == S_OFF )
        ph[q].pEq  =  (char *)aObj[ o_phpeq ].Free();
    else
        if( ph[q].PpEq != S_OFF && !ph[q].pEq )
        {
            ph[q].pEq  =  (char *)aObj[ o_phpeq ].Alloc( 1, MAXFORMULA, S_);
            //*ph[q].pEq = '`';
        }
    if( ph[q].dEq && ph[q].PdEq == S_OFF )
        ph[q].dEq =   (char *)aObj[ o_phdeq ].Free();
    else
        if( ph[q].PdEq != S_OFF && !ph[q].dEq )
        {
            ph[q].dEq  =  (char *)aObj[ o_phdeq ].Alloc( 1, MAXFORMULA, S_);
            //*ph[q].dEq = '`';
        }

// Work objects for SIT, Pitzer, EUNIQUAC aqueous model
    if( ph[q].Ppnc == S_ON && ph[q].sol_t[SPHAS_TYP] == SM_AQSIT )
    {
         ph[q].lsCat = (char (*)[MAXDCNAME])aObj[ o_ph_w_lsc ].Alloc(
                          ph[q].nCat, 1, MAXDCNAME );
         ph[q].lsAn  = (char (*)[MAXDCNAME])aObj[ o_ph_w_lsa ].Alloc(
                          ph[q].nAn, 1, MAXDCNAME );
         ph[q].lsNs  = (char (*)[MAXDCNAME])aObj[ o_ph_w_lsn ].Alloc(
                          ph[q].nNs, 1, MAXDCNAME );
         ph[q].nxCat = (short *)aObj[ o_ph_w_nxc ].Alloc( ph[q].nCat, 1, I_);
         ph[q].nxAn  = (short *)aObj[ o_ph_w_nxa ].Alloc( ph[q].nAn, 1, I_);
         ph[q].nxNs  = (short *)aObj[ o_ph_w_nxn ].Alloc( ph[q].nNs, 1, I_);
    }
    else
    {
        ph[q].lsCat = (char (*)[MAXDCNAME])aObj[ o_ph_w_lsc ].Free();
        ph[q].lsAn =  (char (*)[MAXDCNAME])aObj[ o_ph_w_lsa ].Free();
        ph[q].lsNs =  (char (*)[MAXDCNAME])aObj[ o_ph_w_lsn ].Free();
        ph[q].nxCat = (short *)aObj[ o_ph_w_nxc ].Free();
        ph[q].nxAn =  (short *)aObj[ o_ph_w_nxa ].Free();
        ph[q].nxNs =  (short *)aObj[ o_ph_w_nxn ].Free();
    }
    moiety_new(ph[q].nDC, ph[q].nMoi, false );
    //  ph[q].tprn = (char *)aObj[ o_phtprn ].Free();

    // new record 06/06/12
    if( ph[q].PEIpc == S_ON )
    {
        ph[q].EIpc =  (float *)aObj[ o_pheipc].Alloc( ph[q].nEIl, ph[q].nEIp, F_ );
        ph[q].eimLl =  (char (*)[MAXDCNAME])aObj[ o_pheimll].Alloc( 1, ph[q].nEIp, MAXDCNAME );
    }
    else
    {
        ph[q].EIpc =  (float *)aObj[ o_pheipc].Free( );
        ph[q].eimLl =  (char (*)[MAXDCNAME])aObj[ o_pheimll].Free( );
    }
    if( ph[q].PCDc == S_ON )
    {
      ph[q].CDc =  (float *)aObj[ o_phcdc].Alloc( ph[q].nDC, ph[q].nCDc, F_ );
      ph[q].eimPl =  (char (*)[MAXDCNAME])aObj[ o_pheimpl].Alloc( 1, ph[q].nCDc, MAXDCNAME );
    }
    else
    {
        ph[q].CDc =  (float *)aObj[ o_phcdc].Free( );
        ph[q].eimPl =  (char (*)[MAXDCNAME])aObj[ o_pheimpl].Free( );
    }
    if( ph[q].PIsoC == S_ON )
    {
      ph[q].IsoP =  (float *)aObj[ o_phisop].Alloc( ph[q].nDC, ph[q].nIsoC, F_ );
      ph[q].smcDl =  (char (*)[MAXDCNAME])aObj[ o_phsmcdl].Alloc( 1, ph[q].nIsoC, MAXDCNAME );
    }
    else
    {
      ph[q].IsoP =  (float *)aObj[ o_phisop].Free( );
      ph[q].smcDl =  (char (*)[MAXDCNAME])aObj[ o_phsmcdl].Free( );
    }
    if( ph[q].PIsoS == S_ON )
    {
      ph[q].IsoS =  (float *)aObj[ o_phisos].Alloc( ph[q].NsiT, ph[q].nIsoS, F_ );
      ph[q].smcSl =  (char (*)[MAXDCNAME])aObj[ o_phsmcsl].Alloc( 1, ph[q].nIsoS, MAXDCNAME );
    }
    else
    {
     ph[q].IsoS =  (float *)aObj[ o_phisos].Free( );
     ph[q].smcSl =  (char (*)[MAXDCNAME])aObj[ o_phsmcsl].Free( );
    }
    if( ph[q].PsDiS == S_ON )
    {
        ph[q].xSmD =  (short *)aObj[  o_phxsmd ].Alloc( ph[q].nDC, ph[q].NsiT, I_ );
        ph[q].IsoC =  (char *)aObj[ o_phisoc].Alloc( 2, ph[q].NsiT, A_ );
    }
    else
    {
        ph[q].xSmD =  (short *)aObj[  o_phxsmd ].Free();
        ph[q].IsoC =  (char *)aObj[ o_phisoc].Free( );
    }
    if( ph[q].PrpCon == S_ON )
    {
      ph[q].rpCon =  (float *)aObj[ o_phrpcon].Alloc( ph[q].nPRk, ph[q].nrpC, F_ );
      ph[q].lDCr =  (char (*)[DC_RKLEN])aObj[ o_phldc].Alloc( ph[q].nSkr, 1, DC_RKLEN );
      ph[q].feSAr =  (float *)aObj[ o_phfsac].Alloc( ph[q].nPRk, 1, F_ );
      ph[q].ocPRk =  (short *)aObj[ o_phxfaces ].Alloc( ph[q].nPRk, 1, I_ );
      ph[q].rpkcl =  (char (*)[MAXDCNAME])aObj[ o_phrpkcl].Alloc( 1, ph[q].nrpC, MAXDCNAME );
      ph[q].rprcl =  (char (*)[MAXDCNAME])aObj[ o_phrprcl].Alloc( ph[q].nPRk, 1, MAXDCNAME );
    }
    else
    {
        ph[q].rpCon =  (float *)aObj[ o_phrpcon].Free( );
        ph[q].lDCr =  (char (*)[DC_RKLEN])aObj[ o_phldc].Free( );
        ph[q].feSAr =  (float *)aObj[ o_phfsac].Free( );
        ph[q].ocPRk =  (short *)aObj[ o_phxfaces ].Free();
        ph[q].rpkcl =  (char (*)[MAXDCNAME])aObj[ o_phrpkcl].Free( );
        ph[q].rprcl =  (char (*)[MAXDCNAME])aObj[ o_phrprcl].Free( );
    }
    if( ph[q].PumpCon == S_ON )
    {
      ph[q].umpCon =  (float *)aObj[ o_phumpcon].Alloc( ph[q].nDC, ph[q].numpC, F_ );
      ph[q].umpcl =  (char (*)[MAXDCNAME])aObj[ o_phumpcl].Alloc( 1, ph[q].numpC, MAXDCNAME );
    }
    else
    {
      ph[q].umpCon =  (float *)aObj[ o_phumpcon].Free( );
      ph[q].umpcl =  (char (*)[MAXDCNAME])aObj[ o_phumpcl].Free( );
    }
    if( ph[q].PlPhl == S_ON )
    {
        ph[q].lPh =  (char (*)[PH_RKLEN])aObj[ o_phlph].Alloc( ph[q].nlPh, 1, PH_RKLEN );
        ph[q].lPhC =  (char *)aObj[ o_phlphc2].Alloc( ph[q].nlPh, 1, A_ );
        ph[q].lPhc =  (float *)aObj[ o_phlphc1].Alloc( ph[q].nlPh, ph[q].nlPc, F_ );
    }
    else
    {
        ph[q].lPh =  (char (*)[PH_RKLEN])aObj[ o_phlph].Free( );
        ph[q].lPhC =  (char *)aObj[ o_phlphc2].Free( );
        ph[q].lPhc =  (float *)aObj[ o_phlphc1].Free();
    }

    if( ph[q].Pdqf == S_ON )
     ph[q].DQFc =  (float *)aObj[ o_phdqfc].Alloc( ph[q].nDC, ph[q].ndqf, F_ );
    else
     ph[q].DQFc =  (float *)aObj[ o_phdqfc].Free();

    if( ph[q].Pdqf == S_ON )
      ph[q].rcpc =  (float *)aObj[ o_phrcpc].Alloc( ph[q].nDC, ph[q].nrcp, F_ );
    else
      ph[q].rcpc =  (float *)aObj[ o_phrcpc].Free();
 }


// realloc dynamic memory
void TPhase::moiety_new( int nDC, int nMoi, bool setDefault )
{
    // Work objects added for multi-site mixin models implementation
    php->nMoi = nMoi;

    if(php->nMoi >0  )
    {
        php->lsForm = (char (*)[MAXFORMULA])aObj[o_ph_lsform].Alloc( nDC, 1, MAXFORMULA );
        php->lsMoi = (char (*)[MAXDCNAME])aObj[o_ph_lsmoi].Alloc( 1, php->nMoi, MAXDCNAME );
        php->OcpN =   (float *)aObj[o_ph_ocpn].Alloc( nDC, php->nMoi, F_ );
        php->nxSub = (short *)aObj[o_ph_nxsub].Alloc( nDC, php->nMoi, I_ );
        if( setDefault )
        {
            fillValue( php->nxSub, (short)-1, nDC*php->nMoi );
            fillValue( php->OcpN, (float)0., nDC*php->nMoi );
        }

    }
    else
    {
        php->lsForm = (char (*)[MAXFORMULA])aObj[o_ph_lsform].Free();
        php->lsMoi = (char (*)[MAXDCNAME])aObj[o_ph_lsmoi].Free();
        php->OcpN =   (float *)aObj[o_ph_ocpn].Free();
        php->nxSub = (short *)aObj[o_ph_nxsub].Free();
     }
}


//set default information
void TPhase::set_def( int q)
{
    ErrorIf( php!=&ph[q], GetName(), "E05PHrem: Invalid access to ph in set_def()");
    TProfil *aPa=(TProfil *)(&aMod[RT_PARAM]);
    memcpy( ph[q].sol_t, aPa->pa.PHsol_t, 6 );
    memcpy( &ph[q].PphC, aPa->pa.PHpvc, 6 );
    strcpy( ph[q].name, "`");
    strcpy( ph[q].notes, "`" );

    ph[q].Nsd = 1;
    ph[q].nDC = ph[q].ncpN = ph[q].ncpM =0;
    ph[q].NsiT = ph[q].nscM = ph[q].npxM = ph[q].nMoi = 0;
    ph[q].Asur =    0;//FLOAT_EMPTY;
    ph[q].Sigma0 =  0;//FLOAT_EMPTY;
    ph[q].SigmaG =  0;//FLOAT_EMPTY;
    ph[q].R0p =     0;//FLOAT_EMPTY;
    ph[q].h0p =     0;//FLOAT_EMPTY;
    ph[q].Eps =     0;//FLOAT_EMPTY;
    ph[q].Cond =    0;//FLOAT_EMPTY;
    ph[q].Rsp1 =    0;//FLOAT_EMPTY;
    ph[q].SCMC =  0;
    ph[q].FsiT =  0;
    ph[q].XfIEC = 0;
    ph[q].MSDT =  0;
    ph[q].CapT =  0;
    ph[q].SATC =  0;
    ph[q].MaSdj = 0;
    ph[q].ipxt = 0;
    ph[q].pnc =   0;
    ph[q].scoef = 0;
    ph[q].SM =    0;
    ph[q].DCC =   0;
    ph[q].DCS =   0;
    ph[q].pEq =   0;
    ph[q].dEq =   0;
    ph[q].sdref = 0;
    ph[q].sdval = 0;
    ph[q].tprn = 0;
    // Work objects for SIT
    ph[q].nCat = 0;
    ph[q].nAn = 0;
    ph[q].nNs = 0;
    ph[q].lsCat = 0;
    ph[q].lsAn =  0;
    ph[q].lsNs =  0;
    ph[q].nxCat = 0;
    ph[q].nxAn =  0;
    ph[q].nxNs =  0;

    ph[q].lsForm = 0;
    ph[q].lsMoi = 0;
    ph[q].OcpN =  0;
    ph[q].nxSub = 0;

    // new record 06/06/12
    memcpy( ph[q].kin_t, "NNNNNN", 6 );
    memcpy( &ph[q].Pdqf, "---------------", 14 );

    //short
    ph[q].nlPh = 0;
    ph[q].nlPc = 0;
    ph[q].ndqf=  3;
    ph[q].nrcp=  3;
    ph[q].nPRk = 0;
    ph[q].nSkr = 0;
    ph[q].nrpC = 0;
    ph[q].numpC = 0;
    ph[q].nEIl = 0;
    ph[q].nEIp = 1;
    ph[q].nCDc = 0;
    ph[q].iRes3 = 0;
    ph[q].nIsoC = 0;
    ph[q].nIsoS = 0;
    ph[q].mDe = 1;
    ph[q].iRes4 = 0;
    //float
    ph[q].Vpor = 0.;
    ph[q].fSAs = 1.;
    ph[q].fPV = 1.;
    ph[q].fRes1 = 0.;
    ph[q].psdC = 0.;
    ph[q].pvdC = 0.;
    ph[q].IEC = 0.;
    ph[q].fRes2 = 0.;

    ph[q].xSmD = 0;
    ph[q].ocPRk = 0;
    ph[q].lPhc = 0;
    ph[q].DQFc = 0;
    ph[q].rcpc = 0;
    ph[q].EIpc = 0;
    ph[q].CDc = 0;
    ph[q].IsoP = 0;
    ph[q].IsoS = 0;
    ph[q].feSAr = 0;
    ph[q].rpCon = 0;
    ph[q].umpCon = 0;
    ph[q].lPh = 0;
    ph[q].lDCr = 0;
    ph[q].dcpcl = 0;
    ph[q].ipicl = 0;
    ph[q].ipccl = 0;
    ph[q].rpkcl = 0;
    ph[q].rprcl = 0;
    ph[q].umpcl = 0;
    ph[q].smcDl = 0;
    ph[q].smcSl = 0;
    ph[q].eimPl = 0;
    ph[q].eimLl = 0;
    ph[q].lPhC = 0;
    ph[q].IsoC = 0;
}


// Input necessary data and links objects
void TPhase::RecInput( const char *key )
{
    TCModule::RecInput( key );
}


static int rkeycmp(const void *e1, const void *e2)
{
    int RCmp;
    RCmp = memcmp( e1, e2, DC_RKLEN );
    return RCmp;
}


// opens window with 'Remake record' parameters
void
TPhase::MakeQuery()
{
//    pImp->MakeQuery();   Preparing for calling Phase Wizard
    const char * p_key;
    char flgs[33];
    int size[22];
    double r2 = php->Asur;

    p_key  = db->PackKey();
    memcpy( flgs, php->sol_t, 33);
    size[0] = php->Nsd;
    size[1] = php->ncpN;
    size[2] = php->ncpM;
    size[3] = php->nscM;
    size[4] = php->npxM;
    size[5] = php->NsiT;
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
    size[21] = php->iRes4;

    if( !vfPhaseSet( window(), p_key, flgs, size, r2 ))
         Error( p_key, "Phase record configuration cancelled by the user!" );
     //  return;   // cancel

    memcpy( php->sol_t, flgs, 33);
    php->Nsd = (short)size[0];
    php->ncpN = (short)size[1];
    php->ncpM = (short)size[2];
    php->nscM = (short)size[3];
    php->npxM = (short)size[4];
    php->NsiT = (short)size[5];
    php->nlPh = (short)size[6];
    php->nlPc = (short)size[7];
    php->ndqf = (short)size[8];
    php->nrcp = (short)size[9];
    php->nPRk = (short)size[10];
    php->nSkr = (short)size[11];
    php->nrpC = (short)size[12];
    php->numpC = (short)size[13];
    php->nEIl = (short)size[14];
    php->nEIp = (short)size[15];
    php->nCDc = (short)size[16];
    php->iRes3 = (short)size[17];
    php->nIsoC = (short)size[18];
    php->nIsoS = (short)size[19];
    php->mDe = (short)size[20];
    php->iRes4 = (short)size[21];

    php->Asur = r2;
}

// Setting up the DC/phase coeffs depending on the
// built-in activity coeff model
void TPhase::Set_DC_Phase_coef()
{
   php->sol_t[DCOMP_DEP] = SM_UNDEF;
   php->sol_t[SPHAS_DEP] = SM_UNDEF;
   php->sol_t[DCE_LINK] = SM_UNDEF;

   switch(php->sol_t[SPHAS_TYP])
   {
       case SM_OTHER:   // Customized hardcoded solid-solution models
                      php->nscM = 0;   // NP_DC
                      php->npxM = 0;   // MaxOrd
                      php->ncpN = 0;   // NPar
                      php->ncpM = 0;   // NPcoef
                      break;
       case SM_BERMAN:   // Sublattice microscopic intra-site interaction model (multicomponent)
                      php->nscM = 1;  // NP_DC
                      php->npxM = 4;  // MaxOrd
                      if( php->ncpN < 1 ) // NPar
                          php->ncpN = 1;
                      if( php->ncpN > (php->nDC * php->nDC * 4. ))
                          php->ncpN = php->nDC * php->nDC * 4.;   // Check max N of parameters!
                      php->ncpM = 3;  // NPcoef
                      break;
       case SM_VANLAAR:   // Van Laar model (multicomponent)
                      php->nscM = 1;  // NP_DC
                      php->npxM = 2;  // MaxOrd
                      if( php->ncpN < 1 ) // NPar
                          php->ncpN = 1;
                      if( php->ncpN > (php->nDC*(php->nDC-1)/2) )
                          php->ncpN = (php->nDC*(php->nDC-1)/2);
                      php->ncpM = 3;  // NPcoef
                      break;
       case SM_REGULAR:   // Regular model (multicomponent)
                      php->nscM = 0;  // NP_DC
                      php->npxM = 2;  // MaxOrd
                      if( php->ncpN < 1 ) // NPar
                          php->ncpN = 1;
                      if( php->ncpN > (php->nDC*(php->nDC-1)/2) )
                          php->ncpN = (php->nDC*(php->nDC-1)/2);
                      php->ncpM = 3;  // NPcoef
                      break;
       case SM_GUGGENM:   // Redlich-Kister model (multicomponent)
                      php->nscM = 0;  // NP_DC
                      php->npxM = 2;  // MaxOrd
                      if( php->ncpN < 1 ) // NPar
                          php->ncpN = 1;
                      if( php->ncpN > (php->nDC*(php->nDC-1)/2) )
                          php->ncpN = (php->nDC*(php->nDC-1)/2);
                      php->ncpM = 16;  // NPcoef
                      break;
      case SM_NRTLLIQ:   // NRTL liquid model (multicomponent), added 03.06.2008 (TW)
                      php->nscM = 0;  // NP_DC
                      php->npxM = 2;  // MaxOrd
                      if( php->ncpN < 1 ) // NPar
                          php->ncpN = 1;
                      if( php->ncpN > (php->nDC*(php->nDC-1)) )
                          php->ncpN = (php->nDC*(php->nDC-1));
                      php->ncpM = 6;  // NPcoef
                      break;
      case SM_WILSLIQ:   // Wilson liquid model (multicomponent), added 09.06.2008 (TW)
                      php->nscM = 0;  // NP_DC
                      php->npxM = 2;  // MaxOrd
                      if( php->ncpN < 1 ) // NPar
                          php->ncpN = 1;
                      if( php->ncpN > (php->nDC*(php->nDC-1)) )
                          php->ncpN = (php->nDC*(php->nDC-1));
                      php->ncpM = 4;  // NPcoef
                      break;
      case SM_REDKIS:   // Redlich-Kister model (binary)
                      php->nscM = 0;
                      php->npxM = 0;
                      php->ncpN = 1; php->ncpM = 3;
                      break;
      case SM_MARGB:  // Margules subregular model (binary)
                      php->nscM = 0;
                      php->npxM = 0;
                      php->ncpN = 2; php->ncpM = 3;
                      break;
      case SM_MARGT:  // Margules regular model (ternary)
                      php->nscM = 0;
                      php->npxM = 0;
                      php->ncpN = 4; php->ncpM = 3;
                      break;
      case SM_CGFLUID:  // Churakov-Gottschalk (CG) EoS
                      php->ncpN = 0;
                      php->ncpM = 0;
                      php->nscM = 12; // last changed 12.12.2008 (TW)
                      php->npxM = 0;
                      // php->nscN = 1; php->nscM = 4;  changed 07.12.2006 KD
                      break;
      case SM_PRFLUID:  // Peng-Robinson-Stryjek-Vera (PRSV) EoS, one binary interaction parameter
                      if( php->ncpN < 1 ) // NPar
                          php->ncpN = 1;
                      if( php->ncpN > (php->nDC*(php->nDC-1)/2) )
                          php->ncpN = (php->nDC*(php->nDC-1)/2);
                      php->ncpM = 2;  // NPcoef
                      php->nscM = 7;  // set to 7 to pull all parameters from CPg
                      php->npxM = 2;
                      break;
      case SM_SRFLUID:  // Soave-Redlich-Kwong (SRK) EoS, one binary interaction parameter
                      if( php->ncpN < 1 ) // NPar
                          php->ncpN = 1;
                      if( php->ncpN > (php->nDC*(php->nDC-1)/2) )
                          php->ncpN = (php->nDC*(php->nDC-1)/2);
                      php->ncpM = 2;  // NPcoef
                      php->nscM = 7;  // set to 7 to pull all parameters from CPg
                      php->npxM = 2;
                      break;
      case SM_PR78FL:  // Peng-Robinson (PR78) EoS, one binary interaction parameter
                      if( php->ncpN < 1 ) // NPar
                          php->ncpN = 1;
                      if( php->ncpN > (php->nDC*(php->nDC-1)/2) )
                          php->ncpN = (php->nDC*(php->nDC-1)/2);
                      php->ncpM = 2;  // NPcoef
                      php->nscM = 7;  // set to 7 to pull all parameters from CPg
                      php->npxM = 2;
                      break;
      case SM_CORKFL:  // compensated Redlich-Kwong (CORK) EoS, one binary interaction parameter
                      if( php->ncpN < 1 ) // NPar
                          php->ncpN = 1;
                      if( php->ncpN > (php->nDC*(php->nDC-1)/2) )
                          php->ncpN = (php->nDC*(php->nDC-1)/2);
                      php->ncpM = 1;  // NPcoef
                      php->nscM = 7;  // set to 7 to pull all parameters from CPg
                      php->npxM = 2;  // MaxOrd
                      break;
      case SM_STFLUID:  // Sterner-Pitzer (STP) EoS, one binary interaction parameter
                      if( php->ncpN < 1 ) // NPar
                          php->ncpN = 1;
                      if( php->ncpN > (php->nDC*(php->nDC-1)/2) )
                          php->ncpN = (php->nDC*(php->nDC-1)/2);
                      php->ncpM = 1;  // NPcoef
                      php->nscM = 7;  // set to 7 to pull all parameters from CPg
                      php->npxM = 2;  // MaxOrd
                      break;
      case SM_AQDAV:  // aqueous Davies
                      // php->ncpN = php->ncpM = 0;
                      php->ncpN = 2;
                      php->ncpM = 4; // changed 10.07.2008 DK
                      php->nscM = 0;
                      php->npxM = 0;
                      php->PphC = PH_AQUEL;
                      break;
      case SM_AQDH1:  // aqueous DH limiting law
                      php->ncpN = 2;
                      php->ncpM = 4;
                      php->nscM = 0;
                      php->npxM = 0;
                      php->PphC = PH_AQUEL;
                      break;
      case SM_AQDH2:  // aqueous DH, individual a0, individual bg
                      php->ncpN = 2;
                      php->ncpM = 4;
                      php->nscM = 2;
                      php->npxM = 0;
                      php->PphC = PH_AQUEL;
                      break;
      case SM_AQDH3:  // aqueous EDH Karpov, individual a0, common bg
                      php->ncpN = 2;
                      php->ncpM = 4;
                      php->nscM = 2;
                      php->npxM = 0;
                      php->PphC = PH_AQUEL;
                      break;
      case SM_AQDHH:  // aqueous EDH Helgeson, common a0 and bg
                      php->ncpN = 2;
                      php->ncpM = 4;
                      php->nscM = 0;
                      php->npxM = 0;
                      php->PphC = PH_AQUEL;
                      break;
      case SM_AQDHS:  // aqueous EDH Shvarov, common a0 and bg
                      php->ncpN = 2;
                      php->ncpM = 4;
                      php->nscM = 2;
                      php->npxM = 0;
                      php->PphC = PH_AQUEL;
                      break;
      case SM_AQSIT:  // SIT model in NEA variant - new implementation
                      php->nscM = 0;  // NP_DC
                      php->npxM = 2;  // MaxOrd
                      if( php->ncpN < 1 ) // NPar
                          php->ncpN = 1;
                      if( php->ncpN > (php->nDC*php->nDC/2) )
                          php->ncpN = (php->nDC*php->nDC/2);
                      php->ncpM = 2;  // NPcoef - changed from 1 to 2 on 13.05.09 (DK)
                      php->PphC = PH_AQUEL;
                      break;
      case SM_AQEXUQ: // built-in EUNIQUAC model for aqueous activity coeffs, changed 18.01.2009 (TW)
                      php->nscM = 2;  // NP_DC
                      php->npxM = 2;  // MaxOrd
                      if( php->ncpN < 1 ) // NPar
                          php->ncpN = 1;
                      if( php->ncpN > (php->nDC*(php->nDC-1)/2+php->nDC) )
                          php->ncpN = (php->nDC*(php->nDC-1)/2+php->nDC);
                       php->ncpM = 2;  // NPcoef
                       php->PphC = PH_AQUEL;
                       break;
      case SM_AQPITZ: // built-in Pitzer HMW aqueous activity coefficient model
                       php->nscM = 0;  // NP_DC
                       php->npxM = 4;  // MaxOrd
                       if( php->ncpN < 1 ) // NPar
                           php->ncpN = 1;
                       if( php->ncpN > (php->nDC*php->nDC) )
                           php->ncpN = (php->nDC*php->nDC);
                       if( php->ncpM <= 5 )
                           php->ncpM = 5;  // NPcoef
                       if( php->ncpM >= 8 )
                           php->ncpM = 8;
                       // php->nscN = 0;
                       php->PphC = PH_AQUEL;
                       break;
      case SM_AQELVIS:  // built-in ELVIS model for aqueous electrolytes
                       php->nscM = 8;                       // NP_DC  = cols of aDCc (rows of aDCc = NComp)
                       php->npxM = 2;                       // MaxOrd = cols of aIPx
                       if( php->ncpN < 1 )                  // NPar   = rows of aIPx and aIPc
                           php->ncpN = 1;
                       if( php->ncpN > (php->nDC*php->nDC) )
                           php->ncpN = (php->nDC*php->nDC);
                       php->ncpM = 8;                       // NPcoef = rows of aIPc
                       php->PphC = PH_AQUEL;
                       break;
      default:  // other models
                       break;
   }
}

// Build ReacDC/DComp keys (aDclist) to be included into the Phase
void TPhase::makeReacDCompList(TCStringArray& aDclist)
{
    int i;
    vstr pkeyrd(81);
    TCStringArray aDclist_old;

    aDclist.Clear();

    //REACDC&DCOMP  keypart
    rt[RT_REACDC].MakeKey( RT_PHASE, pkeyrd, K_ACT, 0, K_ANY, K_ANY, K_ANY, K_END );
   if( pkeyrd[1] != ':')
       pkeyrd[1] = '*';
   if( php->NsiT > 0 )  // template for adsorption
       pkeyrd[0] = CP_SSPC;  // added by KD 25.10.2004

    if( php->nDC && php->SM )
    {
      // Build old selections DCOMP and REACDC
       aDclist_old.Clear();
       for( i=0; i<php->nDC; i++ )
       {
          gstring key_dr = gstring( php->SM[i], 0, DC_RKLEN );
          if( php->DCS[i] == SRC_DCOMP )
          {
            rt[RT_DCOMP].SetKey( key_dr.c_str() );
            rt[RT_DCOMP].SetFldKey( 3, "*" );
            key_dr  = gstring(1, php->DCS[i]);
            key_dr += ' ';
            key_dr += rt[RT_DCOMP].UnpackKey();
          }
          else
            if( php->DCS[i] == SRC_REACDC )
            {
              rt[RT_REACDC].SetKey( key_dr.c_str() );
              rt[RT_REACDC].SetFldKey( 3, "*" );
              key_dr  = gstring(1, php->DCS[i]);
              key_dr += ' ';
              key_dr += rt[RT_REACDC].UnpackKey();
            }
         aDclist_old.Add( key_dr );
      }
    }

AGAINRC:
     aDclist = vfRDMultiKeysSet( window(),
          "Please, mark ReacDC/DComp keys to be included into the Phase",
                  pkeyrd, aDclist_old, php->NsiT  );

     if( aDclist.GetCount() < 1 )
     {
        switch ( vfQuestion3(window(), GetName(),
        "W08PHrem: Number of selected ReacDC/DComp keys < 1.\n"
        " Mark again, proceed without ReacDC/DComp or Cancel?",
        "&Repeat", "&Proceed"))
        {
          case VF3_1:
            goto AGAINRC;
            ;
          case VF3_2:
            break;
          case VF3_3:  Error( GetName(),
             "E09PHrem: No ReacDC/DComp records selected into Phase...");
        }
      }

      php->nDC = (short)(aDclist.GetCount());
}


// Load DC classes from records and set to DCC
void TPhase::LoadDCC()
{
    vstr dcn(MAXRKEYLEN);
    char Ctype;
    time_t crt;
    TDComp* aDC=(TDComp *)(&aMod[RT_DCOMP]);
    TReacDC* aRDC=(TReacDC *)(&aMod[RT_REACDC]);
    memset( dcn, 0, MAXRKEYLEN );

    for(int i=0; i<php->nDC; i++ )
    {

      php->DCS[i] = php->SM[i][DC_RKLEN-1]; // species class code
      php->SM[i][DC_RKLEN-1] = ' ';

      // Get key
       memcpy( dcn, php->SM[i], DC_RKLEN );
       dcn[DC_RKLEN]=0;
       Ctype = A_NUL;
      // Read record and extract data
      if( php->DCS[i] == SRC_DCOMP )
      {
         aDC->TryRecInp( dcn, crt, 0 );
         Ctype = aDC->dcp->PdcC;
      }
      else
        if( php->DCS[i] == SRC_REACDC )
        {
            aRDC->TryRecInp( dcn, crt, 0 );
            Ctype = aRDC->rcp->PreC;
        }

      // test model and types of components
      if( php->Pinternal1 == S_ON || php->DCC[i] == A_NUL || php->DCC[i] == '`')
      {
        if( Ctype == DC_SCP_CONDEN && php->nDC >=2 &&
            !( php->PphC == PH_AQUEL || php->PphC == PH_GASMIX || php->PphC == PH_SORPTION ))
           Ctype = DC_SOL_MAJOR;
         php->DCC[i] = Ctype;
      }
    }  // i
}

//Rebuild record structure before calc
int
TPhase::RecBuild( const char *key, int mode  )
{
    int  i;
    TCStringArray aDclist;
    gstring str;
    TProfil *aPa=(TProfil *)(&aMod[RT_PARAM]);

    php->PphC = php->pst_[0];
    if( php->PphC == PH_FLUID /* && php->sol_t[SPHAS_TYP] == SM_OTHER */ )
    {
        php->nscM = MAXEOSPARAM;
    }

    php->Pinternal1 = S_OFF;
    if( db->FindCurrent(key) < 0 )
       php->Pinternal1 = S_ON;      // refresh DC codes if new record

AGAIN_SETUP:
    int ret = TCModule::RecBuild( key, mode );
    if( ret == VF_CANCEL  &&!( !php->PphC || php->PphC == ' '))
        return ret;

    // Change Phase key
    memcpy( php->pst_, rt[nRT].UnpackKey(), PH_RKLEN );
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

        // mark ReacDC/DComp keys to be included into the Phase
        makeReacDCompList( aDclist );
        // Pre-proc. loop for SIT or Pitzer: determining number of cations and anion
        DetNumbCatAn(aDclist);

    //---------------------------------------------------------------------

    // Setting up the DC/phase coeffs depending on the
    // built-in activity coeff model
    if( php->sol_t[SGM_MODE] == SM_STNGAM )
       Set_DC_Phase_coef();

    // test sizes
    if( php->nscM < 0 || php->npxM < 0 || php->ncpN < 0 || php->ncpM < 0 ||
            php->ncpN*php->ncpM > MAXPNCOEF || php->Nsd < 0 || php->Nsd > 16 ||
            php->NsiT < 0 || php->NsiT > 6 )
    {
        if(vfQuestion(window(), GetName(),
        		"W06PHrem: Invalid number of coeffs in the non-ideal solution model!\n Repeat (Y) or Cancel (N)?"))
            goto AGAIN_SETUP;
        else   Error( GetName(), "E07PHrem: The user cancelled remaking Phase definition !");
    }

    // insert coeff of model of solid and other data
    if( php->nscM )
        php->Psco = S_ON;
      else  php->Psco = S_OFF;
    if( php->ncpN * php->ncpM )
        php->Ppnc = S_ON;
      else php->Ppnc = S_OFF;
    if( !php->NsiT )
       php->PFsiT = S_OFF;
    if( php->sol_t[DCOMP_DEP] == SM_UNDEF )
        php->PdEq = S_OFF;
      else php->PdEq = S_ON;
    if( php->sol_t[SPHAS_DEP] == SM_UNDEF )
        php->PpEq = S_OFF;
      else php->PpEq = S_ON;
    if( (php->NsiT != S_OFF) && php->sol_t[SPHAS_TYP] == SM_SURCOM )
    {
       php->nMoi = 0; php->nSub = 0;
    }

    dyn_new(0);  // reallocation of memory

    // Get list of components : add aMcv and aMrv
    for( i=0; i<php->nDC; i++ )
    {
        memcpy( php->SM[i], aDclist[i].c_str()+2, DC_RKLEN );
        php->SM[i][DC_RKLEN-1] = aDclist[i].c_str()[0];
    }
    // Sorting the list of dependent components
    if( php->nDC >= 2 )         // >= may change behavior !
        qsort( php->SM[0], (size_t)php->nDC, DC_RKLEN, rkeycmp );

    // Load DC classes from records and set to DCC
    LoadDCC();

//---------------------------------------------------------------------
// old  part
    if( php->NsiT > 0 && (php->PFsiT == S_REM || php->PFsiT == S_ON  ))
    {  /* Setup of default values */
        php->PphC = PH_SORPTION;
        for( i=0; i<php->NsiT; i++ )
        { /* if( !php->SCMC[i] || php->SCMC[i]==A_NUL ) */
            // php->SCMC[i] = php->sol_t[SCM_TYPE];  // fixed, 24.07.2006 (DK)
            php->SCMC[i] = SC_BSM;  // changed, 14.07.2009 (TW)

            if( !php->FsiT )
                php->FsiT[i] = 1./php->NsiT;
            if( !php->MSDT[i][0] )
                php->MSDT[i][0] = aPa->pa.p.DNS;
            if( !php->MSDT[i][1] )
                php->MSDT[i][1] = 4.0; // Third-layer capacitance (reserved)
            if( !php->CapT[i][0] )
                php->CapT[i][0] = 1.0; // C1 inner capacitance
            if( !php->CapT[i][1] )
                php->CapT[i][1] = 0.2; // C2 outer capacitance
            if( !php->XfIEC )
                php->XfIEC[i] = 0;
        }
        for( i=0; i<php->nDC; i++ )
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

    SetString("PH_make   Remake of Phase definition OK");
    pVisor->Update();
    return ret;
}


// Pre-proc. loop for SIT or Pitzer: determining number of cations and anion
// in aDclist
void TPhase::DetNumbCatAn(TCStringArray& aDclist)
{
  short nCat = 0, nAn = 0, nNs = 0;

 if( php->PphC == PH_AQUEL &&
     ( php->sol_t[SPHAS_TYP] == SM_AQSIT || php->sol_t[SPHAS_TYP] == SM_AQPITZ ))
 {
    int pos;
    gstring spName;
    for(int i=0; i<php->nDC/*-1*/; i++ ) // BugFix SD 26/11/2010  different number of neitral species
    {
      spName = gstring( aDclist[i], MAXSYMB+MAXDRGROUP+2, MAXDCNAME);

      spName.strip();
      pos = spName.length()-1;
      while( pos>0 && spName[pos] <=  '9' && spName[pos] >= '0' )
          pos--;
      switch( spName[pos] )
      {
        case '-': nAn++;
                  break;
        case '+': nCat++;
                  break;
        case '@': nNs++;   // neutral species except H2O
                  break;
        default:
                  break;
      }
    }

    if( nCat <=0 || nCat >= php->nDC || nAn <=0 || nCat >= php->nDC )
         Error( GetName(), "E39PHrem: No cations or no anions - SIT model cannot be applied...");
     php->nCat = nCat;
     php->nAn = nAn;
     php->nNs = nNs;
  }
}


// Assembling indices and name lists for cations and anions
void
TPhase::MakeCatAnLists( bool WorkCount, bool WorkAlloc, bool FillOut )
{
   int pos;
   short i, iCat=0, iAn=0, iNs=0, nAn, nCat, nNs;
   gstring spName;

   if( WorkCount )
   {   // pre-proc. loop: determining number of cations, anions and neutral species
      nAn=0, nCat=0, nNs=0;
      for( i=0; i<php->nDC/*-1*/; i++ ) // BugFix SD 26/11/2010  different number of neitral species
      {
         spName = gstring( php->SM[i], MAXSYMB+MAXDRGROUP, MAXDCNAME);
         spName.strip();
         pos = spName.length()-1;
         while( pos>0 && spName[pos] <=  '9' && spName[pos] >= '0' )
             pos--;
         switch( spName[pos] )
         {
           case '-': nAn++;
                     break;
           case '+': nCat++;
                     break;
           case '@': nNs++;  // H2O@ is not included
                     break;
           default:
                     break;
         }
      }
      if( nCat <=0 || nCat >= php->nDC || nAn <=0 || nCat >= php->nDC )
           Error( GetName(),
              "E39PHrem: No cations or no anions - SIT model cannot be applied...");
      php->nCat = nCat;
      php->nAn = nAn;
      php->nNs = nNs;
   }

   if( WorkAlloc )
   {
      if( php->Ppnc == S_ON
    		  && (php->sol_t[SPHAS_TYP] == SM_AQSIT || php->sol_t[SPHAS_TYP] == SM_AQPITZ) )
      {
         php->lsCat = (char (*)[MAXDCNAME])aObj[ o_ph_w_lsc ].Alloc(
                          php->nCat, 1, MAXDCNAME );
         php->lsAn  = (char (*)[MAXDCNAME])aObj[ o_ph_w_lsa ].Alloc(
                          php->nAn, 1, MAXDCNAME );
         php->nxCat = (short *)aObj[ o_ph_w_nxc ].Alloc( php->nCat, 1, I_);
         php->nxAn  = (short *)aObj[ o_ph_w_nxa ].Alloc( php->nAn, 1, I_);
         if( php->nNs )
         {
             php->lsNs  = (char (*)[MAXDCNAME])aObj[ o_ph_w_lsn ].Alloc(
                              php->nNs, 1, MAXDCNAME );
             php->nxNs  = (short *)aObj[ o_ph_w_nxn ].Alloc( php->nNs, 1, I_);
         }
      }
      else {

        php->nSub =  0;
        php->lsCat = (char (*)[MAXDCNAME])aObj[ o_ph_w_lsc ].Free();
        php->lsAn =  (char (*)[MAXDCNAME])aObj[ o_ph_w_lsa ].Free();
        php->lsNs =  (char (*)[MAXDCNAME])aObj[ o_ph_w_lsn ].Free();
        php->nxCat = (short *)aObj[ o_ph_w_nxc ].Free();
        php->nxAn =  (short *)aObj[ o_ph_w_nxa ].Free();
        php->nxNs =  (short *)aObj[ o_ph_w_nxn ].Free();

      }
   }

   if( FillOut )
   {
     for( i=0; i<php->nDC/*-1*/; i++ ) // BugFix SD 26/11/2010  different number of neitral species
     { // Determining if cation or anion
       spName = gstring( php->SM[i], MAXSYMB+MAXDRGROUP, MAXDCNAME);
       spName.strip();
       pos = spName.length()-1;
       while( pos>0 && spName[pos] <= '9' && spName[pos] >= '0' )
          pos--;
       switch( spName[pos] )
       {
          case '-': memcpy( php->lsAn[iAn], php->SM[i]+MAXSYMB+MAXDRGROUP, MAXDCNAME );
                    php->nxAn[iAn++] = i;
                    break;
          case '+': memcpy( php->lsCat[iCat], php->SM[i]+MAXSYMB+MAXDRGROUP, MAXDCNAME );
                    php->nxCat[iCat++] = i;
                    break;
          case '@': memcpy( php->lsNs[iNs], php->SM[i]+MAXSYMB+MAXDRGROUP, MAXDCNAME );
				    php->nxNs[iNs++] = i;
          default:
                  continue;
        }
     }
     if( iCat != php->nCat || iAn != php->nAn || iNs != php->nNs )
       Error( GetName(),
        "E38PHrem: Mismatch in the number of cations, anions or neutral species...");
   }
}

const int MAXMOIETY = 60;

// Assembling indices and name lists for Multi-site (sublattice) SS models
void TPhase::MakeSublatticeLists( TCStringArray& form_array  )
{
    TFormula form;
    TIArray<MOITERM> moit_;
    uint ii, jj;
    int i1, nSites =0;
    int nMoi =  0;

    // allocate default buffer and setup default values
    moiety_new( form_array.GetCount(), MAXMOIETY, true );

    for( jj=0; jj<form_array.GetCount(); jj++ )
    {
      nSites =  form.BuildMoiety(form_array[jj].c_str(), moit_);
      if( jj== 0)
        php->nSub = nSites;
      else
        ErrorIf( php->nSub!=nSites, "MakeSublatticeLists", "Different number of sites" );

      for( ii=0; ii<moit_.GetCount(); ii++ )
      {
         for( i1=0; i1<nMoi; i1++)
         {
           if( moit_[ii].name == string(php->lsMoi[i1]) )
              break;
         }
         if( i1 == nMoi )
         {
             strncpy( php->lsMoi[ i1], moit_[ii].name.c_str(), MAXDCNAME);
             nMoi++;
             ErrorIf( nMoi == MAXMOIETY, "MakeSublatticeLists", "Too many moieties" );
         }
         php->nxSub[ jj*php->nMoi+i1] = moit_[ii].site;
         php->OcpN[ jj*php->nMoi+i1] = moit_[ii].nj;
      }
    }

   // re allocate memory
   moiety_new( form_array.GetCount(), nMoi, false );


   // setup data to formula list
   if( php->nMoi >0 )
     for( jj=0; jj<form_array.GetCount(); jj++ )
        strncpy( php->lsForm[jj], form_array[jj].c_str(), MAXFORMULA);

}

#define s(i,j) php->scoef[(j)+(i)*nsc]
#define sit(i,j) php->pnc[(j)+(i)*nAn] // not used SD 11/12/2008
// #define m(i,j) php->MaSdj[(j)+(i)*DFCN]
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
    CalcPhaseRecord( getDCC );
    SetString("PH_solm   PHASE-solution model OK");
    TCModule::RecCalc(key);
}


//Recalc record structure
void
TPhase::CalcPhaseRecord(  bool getDCC  )
{
    int  i, pa0=0, Kielland=0;
    short nsc=0;
    vstr dcn(MAXRKEYLEN);
    char Ctype;
    float a0=0., bp=0., Z=0., cN=0., Fi=0.;
    time_t crt;
    TCStringArray form_array;

    TDComp* aDC=(TDComp *)(&aMod[RT_DCOMP]);
    TReacDC* aRDC=(TReacDC *)(&aMod[RT_REACDC]);
    aDC->ods_link(0);
    aRDC->ods_link(0);

    /* Question of get effective ion radii of aqueous species */
    if( (php->PphC == PH_AQUEL) && php->scoef )
    {
        if( pVisor->ProfileMode == true || vfQuestion(window(), GetName(),
        		"Parameters of aqueous species: Collect from DComp/ReacDC records?"))
        {
            pa0 = 1;
            Kielland = 0;
        }
        else
        {
            switch ( vfQuestion3(window(), GetName(),
            "W10PHrem: Shall I set default values automatically:\n"
            "from (Kielland 1936), from (Helgeson ea 1981) or \n"
            "Not collect and not set automatically? ",
                 "&Kielland", "&Helgeson" ) )
            {
            case VF3_1:
                Kielland = 1;
                pa0 = 0;
                break;
            case VF3_2:
                Kielland = 1;
                pa0 = 1;
                break;  /* A Pitzer ? */
            case VF3_3:
                pa0 = 0;
                Kielland = 0;
                break;
            }
        }
    }

    //=====================================================================================
    // main loop for records
    nsc = php->nscM;
    int kx, mcex;

    memset( dcn, 0, MAXRKEYLEN );
    for( i=0; i<php->nDC; i++ )
    {  /*Get key */
        memcpy( dcn, php->SM[i], DC_RKLEN );
        dcn[DC_RKLEN]=0;
        Ctype = A_NUL;
        /* Read record and extract data */
        if( php->DCS[i] == SRC_DCOMP )
        {
           aDC->TryRecInp( dcn, crt, 0 );
           a0 = aDC->dcp->Der;
           bp = aDC->dcp->DerB;
           Z = aDC->dcp->Zz;
           Ctype = aDC->dcp->PdcC;
           form_array.Add( gstring(aDC->dcp->form,0,MAXFORMULA));
           cN = aDC->dcp->Comp;
           Fi = aDC->dcp->Expa;

           if( ( php->PphC == PH_FLUID || php->PphC == PH_LIQUID ||
                 php->PphC == PH_GASMIX ) && php->scoef )
           {
               // Collecting coefficients of EoS for fluids and gases
               if( aDC->dcp->Cemp && !aDC->dcp->CPg )
               {    // CG fluid model coefficients
                  mcex = min( aDC->dcp->Nemp, nsc );
                  for( kx=0; kx< nsc; kx++ )
                  {
                    if( kx < mcex ) // Copying only what is possible
                      s(i,kx) = aDC->dcp->Cemp[kx];
                    else
                      s(i,kx) = 0.;
                  }
               }
               if( aDC->dcp->CPg && !aDC->dcp->Cemp )
               {
                  mcex = min( MAXCRITPARAM, (int)nsc );  // PRSV, SRK, PR78, CORK and STP model coefficients
                  for( kx=0; kx< nsc; kx++ )
                  {
                    if( kx < mcex ) // Copying only what is possible
                      s(i,kx) = aDC->dcp->CPg[kx];
                    else
                      s(i,kx) = 0.;
                  }
               }
               if( !aDC->dcp->CPg && !aDC->dcp->Cemp )
               { // Error - no array in DComp or ReacDC!
                  for( kx=0; kx< nsc; kx++ )
                      s(i,kx) = 0.0;
               }
           }
        }
        else
            if( php->DCS[i] == SRC_REACDC )
            {
                aRDC->TryRecInp( dcn, crt, 0 );
                a0 = aRDC->rcp->Der;
                bp = aRDC->rcp->DerB;
                Z = aRDC->rcp->Zz;
                Ctype = aRDC->rcp->PreC;
                form_array.Add( gstring(aRDC->rcp->form,0,MAXFORMULA));
                cN = aRDC->rcp->Comp;
                Fi = aRDC->rcp->Expa;
            }

        /* test model and types of components */
        if( php->DCC[i] == A_NUL || php->DCC[i] == '`')
        {
            if( Ctype == DC_SCP_CONDEN && php->nDC >=2 &&
                !( php->PphC == PH_AQUEL || php->PphC == PH_GASMIX || php->PphC == PH_SORPTION ))
               Ctype = DC_SOL_MAJOR;
            php->DCC[i] = Ctype;
        }

        if( php->PphC == PH_AQUEL && php->scoef )
        {
            Z = fabs(Z);
            if( pa0 && !Kielland )
            {
                s(i,0) = a0;
                if( nsc > 1 ) s(i,1) = bp;
            }
            if( Kielland && !pa0 )
            {
                if( Z < 0.01 ) s(i,0) = 0;
                if( fabs(Z-1.0) < 0.01 ) s(i,0) = 3.5;
                if( fabs(Z-2.0) < 0.01 ) s(i,0) = 5.0;
                if( Z > 2.99 ) s(i,0) = 9.0; /* Make a table ! */
            }
            if( Kielland && pa0 )  /* Helgeson */
            { if( Z < 0.01 ) s(i,0) = 0;
                if( Z > 0.99 ) s(i,0) = 3.84;
                /* do it by HKF ! */    //  3.72 !
            }
        }

        if( php->PphC == PH_SORPTION || php->PphC == PH_POLYEL )
        {
            // Transferring data into MaSdj array   KD 25.10.2004
            // Now a0 and b0 contain CD (charge distribution) for 0 and beta planes, resp.
                    if(  php->MaSdj )  // && php->NR1 == DFCN
                    {
                       if( fabs( cN ) < 1e-20 )
                          cN = 0.0;
                       php->MaSdj[i][PI_P2] = cN;  // Dentateness or Frumkin isotherm parameter cN
                       if( fabs( Fi ) < 1e-20 )
                          Fi = 0.0;
                       php->MaSdj[i][PI_P1] = Fi;  // Frumkin isotherm parameter Alpha-Fi
            // EDL CD parameters
                       if( fabs( a0 ) < 1e-20 )
                           a0 = Z;
                       if( fabs( bp ) < 1e-20 )
                           bp = 0;
                       if( !(php->DCC[i] == DC_SUR_CARRIER ||
                             php->DCC[i] == DC_SUR_MINAL ||
                             php->DCC[i] == DC_PEL_CARRIER) )
                       {
                          php->MaSdj[i][PI_CD0] = a0;
                          php->MaSdj[i][PI_CDB] = bp;
                          php->MaSdj[i][PI_P1] = Fi;
                          php->MaSdj[i][PI_P2] = cN;
                       }
                       else
                       { //  Sorbent has no data for surface charges
                          php->MaSdj[i][PI_CD0] = 0;
                          php->MaSdj[i][PI_CDB] = 0;
                          php->MaSdj[i][PI_P1] = 0;
                          php->MaSdj[i][PI_P2] = 0;
                       }
                    }
        }


    }  // i

    //=====================================================================================

    if( php->PphC == PH_SINCOND || php->PphC == PH_SINDIS
            || php->PphC == PH_LIQUID || php->PphC == PH_SIMELT )   // added DK 29.03.2012
        MakeSublatticeLists( form_array  );

    if( php->Asur > 1. )
    {
        if( php->nDC == 1 )
        {
            php->PphC = PH_SINDIS;
        }
        else if( php->NsiT >= 1. )
        {
            php->PphC = PH_SORPTION;
        }
    }

}


//-------------------------------------------------------------------------
// called from Project - extended by KD on 16.06.03 to add CG EoS
// Re-written (for AutoPhaseWizard) by KD on 31.07.03
void TPhase::newAqGasPhase( const char * akey, const char *gkey, int file,
   const char amod, const char gmod, float apar[8], /*float gpar[4],*/
   bool useLst, TCStringArray lst )
{
//    TProfil *aPa=(TProfil *)(&aMod[RT_PARAM]);
    const char *part;
    char nbuf[MAXFORMULA*2], neutbuf[16], H2Obuf[16], tempdbuf[16];
    gstring Name = "Auto-set ";

//  Setup of aqueous phase
    if( !apar[2] )
    	strcpy(neutbuf, "1.0");
    else strcpy(neutbuf, "b_q*IS");
    if( !apar[3] )
    	strcpy(H2Obuf, "1.0");
    else strcpy(H2Obuf, "calculate");
    if( !apar[4] )
        strcpy( tempdbuf, "0");
    else sprintf( tempdbuf, "%c", (char)(apar[4]+'0'));
// cout << "newAqGasPhase: " << amod << endl;
    switch( amod )
    {
       case 'N': // No aqueous phase
       case 'U': // User-selected aqueous phase
                 goto MAKE_GAS_PHASE;
       case 'D': // Davies model (no a0 and bg required)
                memcpy( php->sol_t, "DNNSNN", 6 );
                // memcpy( &php->PphC, "a-----", 6 );
                memcpy( &php->PphC, "a+----", 6 );  // changed 10.07.2008 DK
                php->ncpN = 2;
                php->ncpM = 4;
                php->nscM = 0;
                php->npxM = 0;
                apar[0] = 0.0;
                apar[1] = 0.0;
					Name += "ion-association model, Davies equation";
					sprintf( nbuf, "Parameters: gam_neut= %s; gam_H2O= %s", neutbuf, H2Obuf );
                break;
       case 'H': // EDH model with common bg and common a0 (Helgeson)
                memcpy( php->sol_t, "HNNSNN", 6 );
                memcpy( &php->PphC, "a+----", 6 );
                php->ncpN = 2;
                php->ncpM = 4;
                php->nscM = 0;
                php->npxM = 0;
					Name += "ion-association model, EDH(H) equation, common ion size";
					sprintf( nbuf, ": b_gamma= %-5.3f, T_dep= %s; a_size= %-5.3f; gam_neut= %s, gam_H2O= %s ",
							apar[0], tempdbuf, apar[1], neutbuf, H2Obuf );
                break;
       case 'Y': // EDH model with common bg and common a0 (Shvarov)
                memcpy( php->sol_t, "YNNSNN", 6 );
                memcpy( &php->PphC, "a++---", 6 );
                php->ncpN = 2;
                php->ncpM = 4;
                php->nscM = 0;
                php->npxM = 0;
					Name += "ion-association model, EDH(S) equation, common ion size";
					sprintf( nbuf, ": b_gamma= %-5.3f, T_dep= %s; a_size= %-5.3f; gam_neut= %s, gam_H2O= %s ",
							apar[0], tempdbuf, apar[1], neutbuf, H2Obuf );
                break;
       case '3': // EDH model with individual (Kielland) a0 and common bg (Karpov)
                memcpy( php->sol_t, "3NNSNN", 6 );
                memcpy( &php->PphC, "a++---", 6 );
                php->ncpN = 2;
                php->ncpM = 4;
                php->nscM = 2;
                php->npxM = 0;
                apar[1] = 0.0;
					Name += "ion-association model, EDH(K) equation, individual ion sizes";
					sprintf( nbuf, ": b_gamma= %-5.3f, T_dep= %s; a_size=specific; gam_neut= %s; gam_H2O= %s ",
							apar[0], tempdbuf, neutbuf, H2Obuf );
                break;
       case '2': // DH model with individual (Kielland) a0 and optional bg for neutral species
                memcpy( php->sol_t, "2NNSNN", 6 );
                memcpy( &php->PphC, "a++---", 6 );
                php->ncpN = 2;
                php->ncpM = 4;
                php->nscM = 2;
                php->npxM = 0;
                apar[0] = 0.0;
                apar[1] = 0.0;
					Name += "ion-association model, DH equation, individual ion sizes";
					sprintf( nbuf, ": b_gamma= %-5.3f; a_size=specific; gam_neut= %s; gam_H2O= %s ",
							apar[0], neutbuf, H2Obuf );
                break;
       case '1': // DH limiting law (no a0 and bg required)
                memcpy( php->sol_t, "1NNSNN", 6 );
                memcpy( &php->PphC, "a+----", 6 );  // changed 30.05.2009 (TW)
                php->ncpN = 2;
                php->ncpM = 4;
                php->nscM = 0;
                php->npxM = 0;
                apar[0] = 0.0;
                apar[1] = 0.0;
                // apar[2] = 0.0;
					Name += "ion-association model, Debye-Hueckel limiting law";
					sprintf( nbuf, "Parameters: gam_H2O= %s ", H2Obuf );
                break;
       default: // Unrecognized code - error message ?
       case 'S': // SIT - under testing
                 goto MAKE_GAS_PHASE;
    }
    memcpy( php->name, Name.c_str(), MAXFORMULA );
    memcpy( php->notes, nbuf, MAXFORMULA );
    part = "a:*:*:*:";

    // Call assembling of the aqueous phase
    AssemblePhase( akey, part, apar, file, useLst, lst, 6 );

MAKE_GAS_PHASE:
    Name = "Auto-set ";
// cout << "newAqGasPhase Gas: " << gmod << endl;
    switch( gmod )
    {
      case 'N': // No gas/fluid phase in the system
      case 'U': // User - selected from database
                goto DONE;
      case 'I':  // Ideal gas mixture (default)
              memcpy( php->sol_t, "INNINN", 6 );
              memcpy( &php->PphC, "g-----", 6 );
              php->ncpN = 0;
              php->ncpM = 0;
              php->nscM = 0;
              php->npxM = 0;
              Name += "Ideal mixture of ideal or real gases";
              strncpy( php->name, Name.c_str(), MAXFORMULA );
              memcpy( php->notes, "Applicable only at low P and elevated T", MAXFORMULA );
              break;
      case 'F':  // CG fluid EoS model
              memcpy( php->sol_t, "FNNSNN", 6 );
              memcpy( &php->PphC, "f-+---", 6 );
              php->ncpN = 0;
              php->ncpM = 0;
              php->nscM = 12; // last changed 12.12.2008 (TW)
              php->npxM = 0;
              Name += "Churakov-Gottschalk (CG) EoS model";
              strncpy( php->name, Name.c_str(), MAXFORMULA );
              memcpy( php->notes,
                          "Applicable at high P and moderate T", MAXFORMULA );
              break;
      case 'P':  // PRSV fluid EoS model
              memcpy( php->sol_t, "PNNSNN", 6 );
              memcpy( &php->PphC, "f++---", 6 );
              if( php->ncpN < 1 ) // NPar
            	  php->ncpN = 1;
              if( php->ncpN > (php->nDC*(php->nDC-1)/2) )
            	  php->ncpN = (php->nDC*(php->nDC-1)/2);
              // php->ncpN = 0;
              php->ncpM = 2;
              php->nscM = 7;
              php->npxM = 2;
              Name += "Peng-Robinson-Stryjek-Vera (PRSV) EoS model";
              strncpy( php->name, Name.c_str(), MAXFORMULA );
              memcpy( php->notes,
                          "Applicable at moderate P and moderate T" , MAXFORMULA);
              break;
      case 'E':  // SRK fluid EoS model
              memcpy( php->sol_t, "ENNSNN", 6 );
              memcpy( &php->PphC, "f++---", 6 );
              if( php->ncpN < 1 ) // NPar
            	  php->ncpN = 1;
              if( php->ncpN > (php->nDC*(php->nDC-1)/2) )
            	  php->ncpN = (php->nDC*(php->nDC-1)/2);
              // php->ncpN = 0;
              php->ncpM = 2;
              php->nscM = 7;
              php->npxM = 2;
              Name += "Soave-Redlich-Kwong (SRK) EoS model";
              strncpy( php->name, Name.c_str(), MAXFORMULA );
              memcpy( php->notes,
                          "Applicable at moderate P and moderate T", MAXFORMULA );
              break;
      case '7':  // PR78 fluid EoS model
              memcpy( php->sol_t, "7NNSNN", 6 );
              memcpy( &php->PphC, "f++---", 6 );
              if( php->ncpN < 1 ) // NPar
            	  php->ncpN = 1;
              if( php->ncpN > (php->nDC*(php->nDC-1)/2) )
            	  php->ncpN = (php->nDC*(php->nDC-1)/2);
              // php->ncpN = 0;
              php->ncpM = 2;
              php->nscM = 7;
              php->npxM = 2;
              Name += "Peng-Rosinson (PR78) EoS model";
              strncpy( php->name, Name.c_str(), MAXFORMULA );
              memcpy( php->notes,
                          "Applicable at moderate P and moderate T", MAXFORMULA );
              break;
      default:  // unrecognized code
              goto DONE;
    }
    part = "g:*:*:*:";
    if( gkey[0] == 'f' )
        part = "f:*:*:*:";

    // Assembling gas phase
    AssemblePhase( gkey, part, 0, file, useLst, lst, 0 );

    // Do sometning else here?
    DONE:
    contentsChanged = false;
}


// Assembling the phase (automatically generated aq or gas/fluid)
// Separated by KD on 31.07.03
void
TPhase::AssemblePhase( const char* key, const char* part, float* param,
    int file, bool useLst, TCStringArray lst, int Npar )
{

    TProfil *aPa=(TProfil *)(&aMod[RT_PARAM]);

// Initializing
    php->Nsd = 0;
    php->nDC = 0;
    php->NsiT = php->nMoi = 0;
    php->Asur =   php->Sigma0 =  php->SigmaG =   php->R0p =
                  php->h0p =  php->Eps =  php->Cond =  php->Rsp1 =  0.;

    //Get lists
    TCStringArray aDclist;
    TCIntArray anRDc;
    TCStringArray aRclist;
    TCIntArray anRRc;
    int Ndc, Nrc, iic=0, i;

    if( useLst == false )
    {
        //DCOMP key list
        Ndc = rt[RT_DCOMP].GetKeyList( part, aDclist, anRDc );
        if( part[0] == 'g' ) // Added Sveta 28/10/02 for plasma
        {
          TCStringArray aDclist1;
          TCIntArray anRDc1;
          Ndc += rt[RT_DCOMP].GetKeyList( "p:*:*:*:", aDclist1, anRDc1 );
          for(uint kk=0; kk<aDclist1.GetCount(); kk++)
          {
            aDclist.Add(aDclist1[kk]);
            anRDc.Add(anRDc1[kk]);
          }
        }
        uint ii=0;
        while( ii< aDclist.GetCount() )
        {
            if( !memcmp( aDclist[ii].c_str()+MAXSYMB+MAXDRGROUP, "E-", 2 ) ||
                    !memcmp( aDclist[ii].c_str()+MAXSYMB+MAXDRGROUP, "e-", 2 ) )
            {
                aDclist.Remove(ii);
                anRDc.Remove(ii);
                Ndc--;
            }
            else
                if( ii>0 &&
                        !memcmp( aDclist[ii].c_str(), aDclist[ii-1].c_str(), DC_RKLEN-MAXSYMB ) )
                {
                    aDclist.Remove(ii);
                    anRDc.Remove(ii);
                    Ndc--;
                }
                else ii++;
        }
        //REACDC  key list
        Nrc = rt[RT_REACDC].GetKeyList( part, aRclist, anRRc );
        if( part[0] == 'g' ) // Added Sveta 28/10/02 for plasma
        {
          TCStringArray aDclist1;
          TCIntArray anRDc1;
          Nrc += rt[RT_REACDC].GetKeyList( "p:*:*:*:", aDclist1, anRDc1 );
          for(uint kk=0; kk<aDclist1.GetCount(); kk++)
          {
            aRclist.Add(aDclist1[kk]);
            anRRc.Add(anRDc1[kk]);
          }
        }
        ii=0;
        while( ii< aRclist.GetCount() )
        {
            if( ii>0 &&
                    !memcmp( aRclist[ii].c_str(), aRclist[ii-1].c_str(), DC_RKLEN-MAXSYMB ) )
            {
                aRclist.Remove(ii);
                anRRc.Remove(ii);
                Nrc--;
            }
            else ii++;
        }
        ErrorIf( Nrc<1&&Ndc<1,  /*key,*/  "AutoAssemblePhase:",
              " No DComp and ReacDC records found! ");
        php->nDC = (short)(Ndc + Nrc);
//        php->NR1 = (short)aRclist.GetCount();
        iic = aDclist.GetCount();
    }
    else
        php->nDC = (short)lst.GetCount(); // php->NR1 ?

    dyn_new(0);

    if( useLst == false )
    {   /* Get list of component : add aMcv and aMrv */
        for( i=0; i<php->nDC; i++ )
        {
            if( i < (int)aDclist.GetCount() )
            {
                memcpy( php->SM[i], aDclist[i].c_str(), DC_RKLEN );
                php->SM[i][DC_RKLEN-1] = SRC_DCOMP;
            }
            else
            {
                memcpy( php->SM[i], aRclist[i-iic].c_str(), DC_RKLEN );
                php->SM[i][DC_RKLEN-1] = SRC_REACDC;
            }
        }
        /* Sort list of components */
        if( php->nDC > 2 )
            qsort( php->SM[0], (size_t)php->nDC, DC_RKLEN, rkeycmp );
    }
    else
        for( i=0; i<php->nDC; i++ ) // , php->NR1=0
        {
//            if( lst[i].c_str()[DC_RKLEN-1] == SRC_REACDC)
//                php->NR1++;
            memcpy( php->SM[i], lst[i].c_str(), DC_RKLEN );
        }

    for( i=0; i<php->nDC; i++ )
    {
        php->DCS[i] = php->SM[i][DC_RKLEN-1]; /* source code */
        php->SM[i][DC_RKLEN-1] = ' ';
        php->DCC[i] = '`';
    }

    if( php->NsiT > 0 && php->PFsiT == S_REM )
    {  /* Setup of default values */
        for( i=0; i<php->NsiT; i++ )
        {
            if( !php->SCMC[i] || php->SCMC[i]==A_NUL )
                php->SCMC[i] = SC_CCM;  /* Default! */
            if( !php->MSDT[i][0] )
                php->MSDT[i][0] = aPa->pa.p.DNS;
            if( !php->MSDT[i][1] )
                php->MSDT[i][1] = 0.6022; /* 1/nm2; = 1 mkmol/m2 */
            if( !php->CapT[i][0] )
                php->CapT[i][0] = 1.0; /* A plane */
            if( !php->CapT[i][1] )
                php->CapT[i][1] = 0.2; /* B plane */
        }

        for( i=0; i<php->nDC; i++ )
        {   // Restoring defaults if zeros or null-character
            if( !php->SATC[i][SA_MCA] || php->SATC[i][SA_MCA] ==A_NUL )
                php->SATC[i][SA_MCA] = SAT_INDEF;
            if( !php->SATC[i][SA_EMX] || php->SATC[i][SA_EMX] ==A_NUL )
                php->SATC[i][SA_EMX] = CCA_VOL;  /* Default! */
            if( !php->SATC[i][SA_STX] || php->SATC[i][SA_STX] ==A_NUL )
                php->SATC[i][SA_STX] = CST_0;
            if( !php->SATC[i][SA_PLAX] || php->SATC[i][SA_EMX] ==A_NUL )
                php->SATC[i][SA_EMX] = SPL_0;  /* Default! */
            if( !php->SATC[i][SA_SITX] || php->SATC[i][SA_MCA] ==A_NUL )
                php->SATC[i][SA_MCA] = CSI_0;
            if( !php->SATC[i][SA_UNIT] || php->SATC[i][SA_EMX] ==A_NUL )
                php->SATC[i][SA_EMX] = SDU_N;  /* Default! */
        }
        php->PFsiT = S_ON;
    }
    // set up all 0 06/02/2007
    for( i=0; i<php->ncpN * php->ncpM; i++)
    	php->pnc[i] = 0.;

    for( i=0; i < min(Npar,(php->ncpN * php->ncpM)); i++ )
    	php->pnc[i] = param[i];

// Calculating the phase record and saving it to database
    CalcPhaseRecord( true );

    int  Rnum = db->Find( key );

    if( Rnum<0 )
        db->AddRecordToFile( key, file );
    else
      {
        gstring mess = key;
        mess += "\n";
        mess+=  "This record exists! Overwrite?";
        if( !vfQuestion( window(), "Automatically generated aq or gas/fluid",mess.c_str()) )
            Error( key, "Cancel automatically generated aq or gas/fluid!");
        db->Rep( Rnum );
       }
		// contentsChanged = false;
}

const char* TPhase::GetHtml()
{
   return GM_PHASE_HTML;
}

void TPhase::CopyRecords( const char * prfName, TCStringArray& aPHnoused,
            TCStringArray& aPHtmp, elmWindowData el_data,
            phSetupData st_data, TCStringArray& SDlist )
{
    TCIntArray anR;
    TCIntArray aDCused;
    TCStringArray aPHkey;
    aPHnoused.Clear();

    // open selected kernel files
    // db->OpenOnlyFromList(el_data.flNames);
    int fnum_ = db->GetOpenFileNum( prfName );

    // delete the equvalent keys
    TCStringArray aICkey_new;         // 30/11/2006
    aICkey_new.Clear();

  // get list of records
    db->GetKeyList( "*:*:*:*:*:", aPHkey, anR );

    //  test&copy  selected records
    // ( add to last key field first symbol from prfname )
    uint j;
    int i, cnt;
    bool nRec;
    const char *pKey1, *pKey4;
    for(uint ii=0; ii<aPHkey.GetCount(); ii++ )
    {
        uint jj;
// compare keys for template project
       for( jj=0; jj<aPHtmp.GetCount(); jj++ )
        if( !memcmp( aPHtmp[jj].c_str(), aPHkey[ii].c_str(),
                PH_RKLEN-MAXPHGROUP ))
         break;

     if( jj<aPHtmp.GetCount() )
        continue;

// Sorting out phase recs using setup in Elements and SetFilter dialogs
     pKey1 = aPHkey[ii].c_str();
     pKey4 = aPHkey[ii].c_str()+(MAXSYMB+MAXPHSYMB+MAXPHNAME)*sizeof(char);

// Copy phase record for aqueous and/or gas (fluid) phases
//     if( !st_data.flags[PHcopyA_] && ( pKey1[0] == 'a' || pKey1[0] == 'g' ))
//       continue;

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
     if( !el_data.flags[cbSorption_] && ( pKey1[0] == 'x') )
        continue;
     if( !el_data.flags[cbPolyel_] && ( pKey1[0] == 'y') )
        continue;
     if( !el_data.flags[cbHcarbl_] && ( pKey1[0] == 'h') )
        continue;


// copy liquid phases together with solid ones
//     if( !st_data.flags[PHcopyL_] && ( pKey1[0] == 'l' || pKey1[0] == 'h' ))
//       continue;

    // test the same component (overload) 30/11/2006
    gstring stt = aPHkey[ii].substr(0,MAXSYMB+MAXPHSYMB+MAXPHNAME+MAXSYMB);
    for( j=0; j<aICkey_new.GetCount(); j++ )
       if( stt ==  aICkey_new[j])
              break;
     if( j<aICkey_new.GetCount() )
            continue;

// Read the record here
     RecInput( aPHkey[ii].c_str() );

// cbSolutions - multi-component, non-gas, non-electrolyte
     if( el_data.flags[cbSolutions_] && ( pKey1[0] != 'a'
        && pKey1[0] != 'g' && pKey1[0] != 'f'
        && pKey1[0] != 'p' && pKey1[0] != 'x' && pKey1[0] != 'h' ) )
         if( php->nDC > 1 )
           continue;

 // Copy non-ideal phases?
 //    if( !st_data.flags[PHcopyN_] && php->nDC > 1 &&
 //        php->sol_t[0] != 'N' && php->sol_t[0] !='I'  )
 //      continue;

// Test existence of DComp/ReacDC records
     aDCused.Clear();
     for( i=0, cnt=0; i<php->nDC; i++ )
     {
        // test to exist of DCOMP or REACDC record later
        // only 3 fields
        gstring key = gstring( php->SM[i], 0, DC_RKLEN);
        if( php->DCS[i] == SRC_DCOMP )
            nRec = rt[RT_DCOMP].FindPart( php->SM[i], 3 );
        else
            nRec = rt[RT_REACDC].FindPart( php->SM[i], 3 );
        if( nRec )
        { aDCused.Add(cnt); cnt++; }
        else
          aDCused.Add(-1);
     } // i

     if( cnt < php->nDC && !( !st_data.flags[PHcopyF_] && cnt > 1  ))
     {
       if( st_data.flags[PHcopyD_] && php->nDC > 1 && cnt > 0  )
         aPHnoused.Add( aPHkey[ii] );
       continue;
     }

     // !!! changing record key
    gstring str= gstring(db->FldKey( 4 ), 0, db->FldLen( 4 ));
    ChangeforTempl( str, st_data.from_templ,
                    st_data.to_templ, db->FldLen( 4 ));
        str += ":";
        gstring str1 = gstring(db->FldKey( 3 ), 0, db->FldLen( 3 ));
        str1.strip();
        str = str1 + ":" + str;
        str1 = gstring(db->FldKey( 2 ), 0, db->FldLen( 2 ));
        str1.strip();
        str = str1 + ":" + str;
        str1 = gstring(db->FldKey( 1 ), 0, db->FldLen( 1 ));
        str1.strip();
        str = str1 + ":" + str;
        str1 = gstring(db->FldKey( 0 ), 0, db->FldLen( 0 ));
        str1.strip();
        str = str1 + ":" + str;
     CompressRecord( cnt, aDCused );
     //Point SaveRecord
     if( AddRecordTest( str.c_str(), fnum_ ))
     {  aICkey_new.Add( stt );  // 30/11/2006
        for(int isd=0; isd<php->Nsd; isd++)
        { gstring sdkey = gstring( php->sdref[isd], 0,V_SD_RKLEN);
          sdkey.strip();
          SDlist.AddUnique( sdkey );
        }
     }
   }

    // close all no project files
    TCStringArray names1;
    names1.Add(prfName);
    db->OpenOnlyFromList(names1);
}

bool TPhase::CompressRecord( int nDCused, TCIntArray& DCused, bool onlyIPX )
{
 int ii, jj, DCndx;
 int nDCnew;
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
    php->DCC[nDCnew] = php->DCC[ii];;
    if( php->Psco == S_ON )
      copyValues( php->scoef+nDCnew*php->nscM, php->scoef+ii*php->nscM, php->nscM );

    if( php->PCDc == S_ON )
      copyValues( php->CDc+nDCnew*php->nCDc, php->CDc+ii*php->nCDc, php->nCDc );
    if( php->PIsoC == S_ON )
      copyValues( php->IsoP+nDCnew*php->nIsoC, php->IsoP+ii*php->nIsoC, php->nIsoC );
    if( php->PsDiS == S_ON )
      copyValues( php->xSmD+nDCnew*php->NsiT, php->xSmD+ii*php->NsiT, php->NsiT );
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
   php->SM = (char (*)[DC_RKLEN])aObj[ o_phsm ].Alloc( php->nDC, 1, DC_RKLEN );
   php->DCS = (char *)aObj[ o_phdcs ].Alloc( php->nDC, 1, A_ );
   php->DCC = (char *)aObj[ o_phdcc ].Alloc( php->nDC, 1, A_ );
   if( php->Psco == S_ON )
      php->scoef = (float *)aObj[ o_phscoef].Alloc( php->nDC, php->nscM, F_ );
   if( php->PFsiT != S_OFF )
   {
      php->SATC =  (char (*)[MCAS])aObj[ o_phsatc ].Alloc( php->nDC, MCAS, A_);
      php->MaSdj = (float (*)[DFCN])aObj[ o_phmasdj ].Alloc( php->nDC, DFCN, F_);
   }
   if( php->PCDc == S_ON )
     php->CDc =  (float *)aObj[ o_phcdc].Alloc( php->nDC, php->nCDc, F_ );
   if( php->PIsoC == S_ON )
     php->IsoP =  (float *)aObj[ o_phisop].Alloc( php->nDC, php->nIsoC, F_ );
   if( php->PsDiS == S_ON )
       php->xSmD =  (short *)aObj[  o_phxsmd ].Alloc( php->nDC, php->NsiT, I_ );
   if( php->PumpCon == S_ON )
     php->umpCon =  (float *)aObj[ o_phumpcon].Alloc( php->nDC, php->numpC, F_ );
   if( php->Pdqf == S_ON )
    php->DQFc =  (float *)aObj[ o_phdqfc].Alloc( php->nDC, php->ndqf, F_ );
   if( php->Pdqf == S_ON )
     php->rcpc =  (float *)aObj[ o_phrcpc].Alloc( php->nDC, php->nrcp, F_ );

 }

 if( php->Ppnc == S_ON && php->npxM > 0 )
 {
   for( ncpNnew=0, ii=0; ii<php->ncpN; ii++)
   {
     for(jj=0; jj<php->npxM; jj++)
     {
       DCndx = php->ipxt[ii*php->npxM+jj];
       if( DCndx  < 0  ) // work for Pitzer
         continue;
       DCndx =  DCused[DCndx];
       if( DCndx  < 0  ) // not exist component
         break;
       //if( !onlyIPX )
         php->ipxt[ii*php->npxM+jj] = DCndx;
     }
     if( jj<php->npxM ) // line with  not existed component
       continue;

     copyValues( php->ipxt+ncpNnew*php->npxM, php->ipxt+ii*php->npxM, php->npxM );
     copyValues( php->pnc+ncpNnew*php->ncpM, php->pnc+ii*php->ncpM, php->ncpM );
     ncpNnew++;
   }
   php->ncpN = ncpNnew;
   php->pnc = (float *)aObj[ o_phpnc ].Alloc( php->ncpN, php->ncpM, F_ );
   php->ipxt = (short *)aObj[ o_phpxres ].Alloc( php->ncpN, php->npxM, I_);
}

//dyn_new();
return true;

}

// ----------------- End of m_phase.cpp -------------------------

