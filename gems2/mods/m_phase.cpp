//-------------------------------------------------------------------
// $Id$
//
// Implementation of TPhase class, config and calculation functions
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
const char *GEMS_PH_HTML = "gm_phase";

#include <math.h>

#include "v_object.h"
#include "visor.h"
#include "service.h"
#include "m_dcomp.h"
#include "m_reacdc.h"
#include "m_phase.h"
#include "m_param.h"
#include "filters_data.h"

TPhase* TPhase::pm;

TPhase::TPhase( int nrt ):
        TCModule( nrt )
{
    nQ = 1;
    aFldKeysHelp.Add(
        "l<4 Code of aggregate state of a phase { a g l h s d x }_");
    aFldKeysHelp.Add(
        "l<8 Symbol of this phase definition (letters, digits, _)");
    aFldKeysHelp.Add(
        "l<16 Name of this phase definition_");
    aFldKeysHelp.Add(
        "l<4 Phase class { c d l gm ss ssd ls aq xsa xc }_");
    aFldKeysHelp.Add(
        "l<16 Record key comment to phase definition_");
    php=&ph[0];
    set_def();
    start_title = " Definition of thermodynamic phase ";
}

// link values to objects
void TPhase::ods_link( int q)
{
    ErrorIf( q > nQ, GetName(), "E00PHrem: Illegal link q>nQ");
    //aObj[ o_phpst].SetPtr(  ph[q].pst );
    //aObj[ o_phsymb].SetPtr( ph[q].symb );
    //aObj[ o_phnam].SetPtr(  ph[q].nam );
    //aObj[ o_phcls].SetPtr(  ph[q].cls );
    //aObj[ o_phgrp].SetPtr(  ph[q].grp );
    aObj[ o_phsolt].SetPtr( ph[q].sol_t );
    aObj[ o_phps].SetPtr(  &ph[q].PphC );  // a 6
    aObj[ o_phndc].SetPtr( &ph[q].nDC );   // i 2
    aObj[ o_phpncp].SetPtr(&ph[q].ncpN );  // i 2
aObj[ o_phnpx].SetPtr( &ph[q].npxM );  // i 1  added 07.12.2006 KD
aObj[ o_phnsc].SetPtr( &ph[q].nscM );  // i 1  changed 07.12.2006 KD
    aObj[ o_phnsit].SetPtr(&ph[q].NsiT );  // i 2
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
//    aObj[ o_phsatc].SetDim( ph[q].nDC,  2 );
    aObj[ o_phsatc].SetDim( ph[q].nDC,  MCAS );
    aObj[ o_phmasdj].SetPtr( ph[q].MaSdj);
//    aObj[ o_phmasdj].SetDim( ph[q].nDC, 1 );
    aObj[ o_phmasdj].SetDim( ph[q].nDC, DFCN );
aObj[ o_phpxres].SetPtr( ph[q].ipxt);  // changed 07.12.2006 KD
    aObj[ o_phpxres].SetDim( ph[q].ncpN, ph[q].npxM );
    aObj[ o_phsm].SetPtr(   ph[q].SM[0] );
    aObj[ o_phsm].SetDim( ph[q].nDC, 1 );
    aObj[ o_phdcc].SetPtr(  ph[q].DCC );
    aObj[ o_phdcc].SetDim( ph[q].nDC, 1 );
    aObj[ o_phdcs].SetPtr(  ph[q].DCS );
    aObj[ o_phdcs].SetDim( ph[q].nDC, 1 );

    aObj[ o_phpnc].SetPtr(  ph[q].pnc );
    aObj[ o_phpnc].SetDim( ph[q].ncpN, ph[q].ncpM );
    aObj[ o_phscoef].SetPtr( ph[q].scoef );
//  aObj[ o_phscoef].SetDim( ph[q].nDC, ph[q].nscN*ph[q].nscM );
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

// Added for SIT aqueous model implementation
aObj[o_ph_w_lsc].SetPtr( ph[q].lsCat );
aObj[o_ph_w_lsc].SetDim( ph[q].nCat, 1 );
aObj[o_ph_w_lsa].SetPtr( ph[q].lsAn );
aObj[o_ph_w_lsa].SetDim( ph[q].nAn, 1 );
aObj[o_ph_w_nxc].SetPtr( ph[q].nxCat );
aObj[o_ph_w_nxc].SetDim( ph[q].nCat, 1 );
aObj[o_ph_w_nxa].SetPtr( ph[q].nxAn );
aObj[o_ph_w_nxa].SetDim( ph[q].nAn, 1 );

    //    aObj[ o_phtprn].SetDim( 1,strlen(ph[q].tprn));
    php=&ph[q];
}

// set dynamic Objects ptr to values
void TPhase::dyn_set(int q)
{
    ErrorIf( php!=&ph[q], GetName(),
             "E01PHrem: Illegal access to ph in dyn_set()");
    memcpy( php->pst, rt[nRT].UnpackKey(), PH_RKLEN );
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
ph[q].nxCat = (short *)aObj[ o_ph_w_nxc ].GetPtr();
ph[q].nxAn = (short *)aObj[ o_ph_w_nxa ].GetPtr();
}

// free dynamic memory in objects and values
void TPhase::dyn_kill(int q)
{
    ErrorIf( php!=&ph[q], GetName(), "E02PHrem: Illegal access to ph in dyn_kill()");
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
ph[q].nxCat = (short *)aObj[ o_ph_w_nxc ].Free();
ph[q].nxAn =  (short *)aObj[ o_ph_w_nxa ].Free();
}

// realloc dynamic memory
void TPhase::dyn_new(int q)
{
    ErrorIf( php!=&ph[q], GetName(), "E03PHrem: Illegal access to ph in dyn_new()");
    ErrorIf( ph[q].nDC <= 0, GetName(), "E04PHrem: Number of DC in the phase definition <= 0");
    ph[q].SM = (char (*)[DC_RKLEN])aObj[ o_phsm ].Alloc( ph[q].nDC, 1, DC_RKLEN );
    ph[q].DCS = (char *)aObj[ o_phdcs ].Alloc( ph[q].nDC, 1, A_ );
    ph[q].DCC = (char *)aObj[ o_phdcc ].Alloc( ph[q].nDC, 1, A_ );

    if( ph[q].Psco == S_ON )
ph[q].scoef = (float *)aObj[ o_phscoef].Alloc( ph[q].nDC, ph[q].nscM, F_ );
//                ph[q].nscN*ph[q].nscM, F_ );  changed 07.12.2006  KD
    else
        ph[q].scoef = (float *)aObj[ o_phscoef ].Free();

    if( ph[q].Ppnc == S_ON )
    {
      ph[q].pnc = (float *)aObj[ o_phpnc ].Alloc( ph[q].ncpN, ph[q].ncpM, F_ );
      ph[q].ipxt = (short *)aObj[ o_phpxres ].Alloc( ph[q].ncpN, ph[q].npxM, I_);
    }
    else
    {    ph[q].pnc =   (float *)aObj[ o_phpnc ].Free();
         ph[q].ipxt = (short *)aObj[ o_phpxres ].Free();
    }

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
            *ph[q].pEq = '`';
        }
    if( ph[q].dEq && ph[q].PdEq == S_OFF )
        ph[q].dEq =   (char *)aObj[ o_phdeq ].Free();
    else
        if( ph[q].PdEq != S_OFF && !ph[q].dEq )
        {
            ph[q].dEq  =  (char *)aObj[ o_phdeq ].Alloc( 1, MAXFORMULA, S_);
            *ph[q].dEq = '`';
        }

// Work objects for SIT aqueous model
    if( ph[q].Ppnc == S_ON && ph[q].sol_t[SPHAS_TYP] == SM_AQSIT )
    {
         ph[q].lsCat = (char (*)[MAXDCNAME])aObj[ o_ph_w_lsc ].Alloc(
                          ph[q].nCat, 1, MAXDCNAME );
         ph[q].lsAn  = (char (*)[MAXDCNAME])aObj[ o_ph_w_lsa ].Alloc(
                          ph[q].nAn, 1, MAXDCNAME );
         ph[q].nxCat = (short *)aObj[ o_ph_w_nxc ].Alloc( ph[q].nCat, 1, I_);
         ph[q].nxAn  = (short *)aObj[ o_ph_w_nxa ].Alloc( ph[q].nAn, 1, I_);
    }
    else {
        ph[q].lsCat = (char (*)[MAXDCNAME])aObj[ o_ph_w_lsc ].Free();
        ph[q].lsAn =  (char (*)[MAXDCNAME])aObj[ o_ph_w_lsa ].Free();
        ph[q].nxCat = (short *)aObj[ o_ph_w_nxc ].Free();
        ph[q].nxAn =  (short *)aObj[ o_ph_w_nxa ].Free();
    }
    //  ph[q].tprn = (char *)aObj[ o_phtprn ].Free();
}

//set default information

void TPhase::set_def( int q)
{
    ErrorIf( php!=&ph[q], GetName(), "E05PHrem: Illegal access to ph in set_def()");
    TProfil *aPa=(TProfil *)(&aMod[RT_PARAM]);
    memcpy( ph[q].sol_t, aPa->pa.PHsol_t, 6 );
    memcpy( &ph[q].PphC, aPa->pa.PHpvc, 6 );
    strcpy( ph[q].name, "`");
    strcpy( ph[q].notes, "`" );

    ph[q].Nsd = 1;
    ph[q].nDC = ph[q].ncpN = ph[q].ncpM =0;
    ph[q].NsiT = ph[q].nscM = ph[q].npxM = ph[q].NR1 = 0;
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
//    ph[q].PXres = 0;  changed 07.12.2006  by KD
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
ph[q].lsCat = 0;
ph[q].lsAn =  0;
ph[q].nxCat = 0;
ph[q].nxAn =  0;

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
    char flgs[12];
    int size[6];
    double r2 = php->Asur;

    p_key  = db->PackKey();
    memcpy( flgs, php->sol_t, 12);
    size[0] = php->Nsd;
    size[1] = php->ncpN;
    size[2] = php->ncpM;
    size[3] = php->nscM;
    size[4] = php->npxM;
    size[5] = php->NsiT;

    if( !vfPhaseSet( window(), p_key, flgs, size, r2 ))
         Error( p_key, "Phase record configuration cancelled by the user!" );
     //  return;   // cancel

    memcpy( php->sol_t, flgs, 12);
    php->Nsd = (short)size[0];
    php->ncpN = (short)size[1];
    php->ncpM = (short)size[2];
    php->nscM = (short)size[3];
    php->npxM = (short)size[4];
    php->NsiT = (short)size[5];
    php->Asur = r2;
}

//Rebuild record structure before calc
int
TPhase::RecBuild( const char *key, int mode  )
{
    int iic, i;
    short nCat, nAn;
    vstr pkeydc(81);
    vstr pkeyrd(81);

    TCStringArray aDclist;
    TCStringArray aRclist;
    TCStringArray aDclist_old;
    TCStringArray aRclist_old;

    gstring str;
    TProfil *aPa=(TProfil *)(&aMod[RT_PARAM]);
    php->PphC = php->pst[0];
    if( php->PphC == PH_FLUID /* && php->sol_t[SPHAS_TYP] == SM_OTHER */ )
    {
//        php->nscN = 1; php->nscM = MAXEOSPARAM;  changed 07.12.2006 KD
        php->nscM = MAXEOSPARAM;
    }

AGAIN_SETUP:
    int ret = TCModule::RecBuild( key, mode );
    if( ret == VF_CANCEL  &&!( !php->PphC || php->PphC == ' '))
        return ret;
    memcpy( php->pst, rt[nRT].UnpackKey(), PH_RKLEN );
     if( ret == VF3_1 )
    {
        strncpy( php->name, db->FldKey(2), db->FldLen(2));
        php->name[db->FldLen(2)] = '\0';
    }
    // Setting up the DC/phase coeffs depending on the
    // built-in activity coeff model
    if( php->sol_t[SGM_MODE] == SM_STNGAM )
    {
       php->sol_t[DCOMP_DEP] = SM_UNDEF;
       php->sol_t[SPHAS_DEP] = SM_UNDEF;
       php->sol_t[DCE_LINK] = SM_UNDEF;
//       php->sol_t[SCM_TYPE] = SM_UNDEF;

       switch(php->sol_t[SPHAS_TYP])
       {              // These all have to be checked !!!!!!!!!!!!!!!!!!!!!!!
          case SM_VANLAAR:   // Van Laar model multicomponent
                          php->nscM = 1;  // NP_DC
                          php->npxM = 2;  // MaxOrd
                          if( php->ncpN < 1 ) // NPar
                              php->ncpN = 1;
                          if( php->ncpN > php->nDC*php->nDC/2 )
                              php->ncpN = php->nDC*php->nDC/2;
                          php->ncpM = 4;  // NPcoef
                          break;
          case SM_REGULAR:   // Regular model multicomponent
                          php->nscM = 0;  // NP_DC
                          php->npxM = 2;  // MaxOrd
                          if( php->ncpN < 1 ) // NPar
                              php->ncpN = 1;
                          if( php->ncpN > php->nDC*php->nDC/2 )
                              php->ncpN = php->nDC*php->nDC/2;
                          php->ncpM = 4;  // NPcoef
                          break;
          case SM_GUGGENM:   // Redlich-Kister model multicomponent
                          php->nscM = 0;  // NP_DC
                          php->npxM = 2;  // MaxOrd
                          if( php->ncpN < 1 ) // NPar
                              php->ncpN = 1;
                          if( php->ncpN > php->nDC*php->nDC/2 )
                              php->ncpN = php->nDC*php->nDC/2;
                          php->ncpM = 20;  // NPcoef
                          break;
          case SM_REDKIS:   // Redlich-Kister
                          php->nscM = 0;
                          php->npxM = 0;
                          php->ncpN = 1; php->ncpM = 3;
                          break;
          case SM_MARGB:  // Margules binary subregular
                          php->nscM = 0;
                          php->npxM = 0;
                          php->ncpN = 2; php->ncpM = 3;
                          break;
          case SM_MARGT:  // Margules ternary regular
                          php->nscM = 0;
                          php->npxM = 0;
                          php->ncpN = 4; php->ncpM = 3;
                          break;
          case SM_CGFLUID:  // Churakov-Gottschalk EoS
                          php->ncpN = 0;
                          php->ncpM = 0;
                          php->nscM = 20; // 4;   changed 16.02.2007
                          php->npxM = 0;
//                          php->nscN = 1; php->nscM = 4;  changed 07.12.2006 KD
                          break;
// Added 20.07.2006, modified 20.03.2007
          case SM_PRFLUID:  // Peng-Robinson EoS - with interaction parameters
//                          php->ncpN = max( (short)2, php->nDC );
//                          php->ncpM = max( (short)2, php->nDC );
                          if( php->ncpN < 1 ) // NPar
                              php->ncpN = 1;
                          if( php->ncpN > php->nDC*php->nDC/2 )
                              php->ncpN = php->nDC*php->nDC/2;
                          php->ncpM = 1;  // For now, no T,P dependence 
                          php->nscM = 12; // increased to 12 (31.05.2008 TW)
                          php->npxM = 2;
                          break;

          case SM_AQDAV:  // Aqueous Davies
                          php->ncpN = php->ncpM = 0;
                          php->nscM = php->npxM = 0;
                          php->PphC = PH_AQUEL;
                          break;
          case SM_AQDH1:  // Aqueous DH LL
                          php->ncpN = php->ncpM = 0;
                          php->nscM = php->npxM = 0;
                          php->PphC = PH_AQUEL;
                          break;
          case SM_AQDH2:  // DH Kielland, salt-out for neutral
                          php->ncpN = 2; php->ncpM = 4;
                          php->nscM = 1;
                          php->npxM = 0;
                          php->PphC = PH_AQUEL;
                          break;
          case SM_AQDH3:  // EDH Kielland, salt-out for neutral
                          php->ncpN = 2; php->ncpM = 4;
                          php->nscM = 1;
                          php->npxM = 0;
                          php->PphC = PH_AQUEL;
                          break;
          case SM_AQDHH:  // EDH Helgeson, common a0, salt-out for neutral
                          php->ncpN = 2; php->ncpM = 4;
                          php->nscM = 0;
                          php->npxM = 0;
                          php->PphC = PH_AQUEL;
                          break;
          case SM_AQSIT:  // SIT model in NEA variant - new implementation
                          php->nscM = 0;  // NP_DC
                          php->npxM = 2;  // MaxOrd
                          if( php->ncpN < 1 ) // NPar
                              php->ncpN = 1;
                          if( php->ncpN > php->nDC*php->nDC/2 )
                              php->ncpN = php->nDC*php->nDC/2;
                          php->ncpM = 1;  // NPcoef
                          php->PphC = PH_AQUEL;
                          break;
          default:  // other models
             break;
       }
    }
    if( php->nscM < 0 || php->npxM < 0 || php->ncpN < 0 || php->ncpM < 0 ||
            php->ncpN*php->ncpM > MAXPNCOEF || php->Nsd < 0 || php->Nsd > 16 ||
            php->NsiT < 0 || php->NsiT > 6 )
    {
        if(vfQuestion(window(), GetName(),
 "W06PHrem: Invalid number of coeffs in the non-ideal solution model!\n Repeat (Y) or Cancel (N)?"))
            goto AGAIN_SETUP;
        else   Error( GetName(), "E07PHrem: The user cancelled remaking Phase definition !");
    }
    SetString("PH_make   Remaking Phase definition");
    pVisor->Update();

    //DCOMP keypart
    rt[RT_DCOMP].MakeKey( RT_PHASE, pkeydc, K_ACT, 0, K_ANY, K_ANY, K_ANY, K_END);
    if( pkeydc[1] != ':') pkeydc[1] = '*';
if( php->NsiT > 0 )  // template for adsorption
  pkeydc[0] = CP_SOLID; // added by KD 25.10.2004

    //REACDC  keypart
    rt[RT_REACDC].MakeKey( RT_PHASE, pkeyrd, K_ACT, 0, K_ANY, K_ANY, K_ANY, K_END );
    if( pkeyrd[1] != ':') pkeyrd[1] = '*';
if( php->NsiT > 0 )  // template for adsorption
  pkeyrd[0] = CP_SSPC;  // added by KD 25.10.2004

    if( php->nDC && php->SM )
    {
        /* Build old selections DCOMP and REACDC */
        aDclist_old.Clear();
        aRclist_old.Clear();
        for( i=0; i<php->nDC; i++ )
        {
          gstring key_dr = gstring( php->SM[i], 0, DC_RKLEN );
          if( php->DCS[i] == SRC_DCOMP )
          {
              rt[RT_DCOMP].SetKey( key_dr.c_str() );
              rt[RT_DCOMP].SetFldKey( 3, "*" );
              aDclist_old.Add( rt[RT_DCOMP].UnpackKey() );
          }
            else
                if( php->DCS[i] == SRC_REACDC )
                {
                  rt[RT_REACDC].SetKey( key_dr.c_str() );
                  rt[RT_REACDC].SetFldKey( 3, "*" );
                  aRclist_old.Add( rt[RT_REACDC].UnpackKey() );
                }
        }
    }

AGAINRC:
    aRclist = vfMultiKeysSet( window(),
       "Please, mark ReacDC keys to be included into the Phase",
       RT_REACDC, pkeyrd, aRclist_old );
    aDclist = vfMultiKeysSet( window(),
       "Please, mark DComp keys to be included into the Phase",
       RT_DCOMP, pkeydc, aDclist_old );


    if( aRclist.GetCount() < 1 && aDclist.GetCount() < 1 )
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

    php->nDC = (short)(aDclist.GetCount() + aRclist.GetCount());
//    php->NR1 = aRclist.GetCount();   comm.out by KD on 25.10.2004
    iic = aDclist.GetCount();

//-------------------------------------------------------
    nCat = 0; nAn = 0;

    if( php->PphC == PH_AQUEL && php->sol_t[SPHAS_TYP] == SM_AQSIT )
    {  // pre-proc. loop for SIT: determining number of cations and anions
       int pos;
       gstring spName;
       for( i=0; i<php->nDC; i++ )
       {
          if( i < iic )
            spName = gstring( aDclist[i], MAXSYMB+MAXDRGROUP, MAXDCNAME);
         else
            spName = gstring( aRclist[i-iic], MAXSYMB+MAXDRGROUP, MAXDCNAME);

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
           case '@':
           default:
                     break;
         }
       }
       if( nCat <=0 || nCat >= php->nDC || nAn <=0 || nCat >= php->nDC )
            Error( GetName(),
              "E39PHrem: No cations or no anions - SIT model cannot be applied...");
        php->nCat = nCat;
        php->nAn = nAn;
    }
//---------------------------------------------------------------------

    /* insert coeff of model of solid and other data */
    if( php->nscM ) php->Psco = S_ON;
    else  php->Psco = S_OFF;
// if( php->nscM ) php->Psco = S_ON;   07.12.2006  KD   Check later!
//    else  php->Psco = S_OFF;
    if( php->ncpN * php->ncpM ) php->Ppnc = S_ON;
    else php->Ppnc = S_OFF;
    if( !php->NsiT )
         php->PFsiT = S_OFF;
    if( php->sol_t[DCOMP_DEP] == SM_UNDEF ) php->PdEq = S_OFF;
    else php->PdEq = S_ON;
    if( php->sol_t[SPHAS_DEP] == SM_UNDEF ) php->PpEq = S_OFF;
    else php->PpEq = S_ON;

    dyn_new(0);  // reallocation of memory

    /* Get list of components : add aMcv and aMrv */
    for( i=0; i<php->nDC; i++ )
    {
        if( i < iic )
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
    // Sorting the list of dependent components
    if( php->nDC >= 2 )         // >= may change behavior !
        qsort( php->SM[0], (size_t)php->nDC, DC_RKLEN, rkeycmp );
    for( i=0; i<php->nDC; i++ )
    {
        php->DCS[i] = php->SM[i][DC_RKLEN-1]; /* cod istochnic */
        php->SM[i][DC_RKLEN-1] = ' ';
        if( !php->DCC[i] || php->DCC[i] == '`' )
            php->DCC[i] = 'I'/*'`'*/;
    }
    switch( php->pst[0] )
    { /* Set type of phase */
    case CP_AQU:
        php->PphC = PH_AQUEL;
        break;
    case CP_GASI:
        php->PphC = PH_PLASMA;
        break;
    case CP_GAS:
        php->PphC = PH_GASMIX;
        break;
   case CP_FLUID:
        php->PphC = PH_FLUID;
        break;
        //
    case CP_LIQID:
        php->PphC = PH_LIQUID;
        break;
    case CP_SOLID: /* if( php->nDC == 1 ) Fixed 23.10.99 by DAK */
        php->PphC = PH_SINCOND;
        break;
        //
    case CP_HCARB:
        php->PphC = PH_HCARBL;
        break;
    case CP_SSPC:
    case CP_SORB:
        php->PphC = PH_SORPTION;
        break;
    default:
        php->PphC = '?';
    }

    if( php->NsiT > 0 && (php->PFsiT == S_REM || php->PFsiT == S_ON  ))
    {  /* Setup of default values */
        php->PphC = PH_SORPTION;
        for( i=0; i<php->NsiT; i++ )
        { /* if( !php->SCMC[i] || php->SCMC[i]==A_NUL ) */
            php->SCMC[i] = php->sol_t[SCM_TYPE];  // fix by DK 24.07.2006
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
    if( php->NsiT > 0 && php->PFsiT == S_ON )
        php->NR1 = DFCN; // Added for CD-MUSIC by KD on 25.10.2004

//---------------------------------------------------------------------
    if( php->PphC == PH_AQUEL && php->sol_t[SPHAS_TYP] == SM_AQSIT )
    {  // Filling out name and index lists for cations and anions
       if( !php->lsCat )                      // Crash fix 11.05.07 KD
       {
           php->Ppnc = S_ON;
           MakeCatAnLists( true, true, true );
       }
       else
           MakeCatAnLists( false, false, true);
    }
//--------------------------------------------------------------
    SetString("PH_make   Remake of Phase definition OK");
    pVisor->Update();
    return ret;
}

// Assembling indices and name lists for cations and anions
void
TPhase::MakeCatAnLists( bool WorkCount, bool WorkAlloc, bool FillOut )
{
   int pos;
   short i, iCat=0, iAn=0, nAn, nCat;
   gstring spName;

   if( WorkCount )
   {   // pre-proc. loop for SIT: determining number of cations and anions
      nAn=0, nCat=0;
      for( i=0; i<php->nDC; i++ )
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
           case '@':
           default:
                     break;
         }
      }
      if( nCat <=0 || nCat >= php->nDC || nAn <=0 || nCat >= php->nDC )
           Error( GetName(),
              "E39PHrem: No cations or no anions - SIT model cannot be applied...");
      php->nCat = nCat;
      php->nAn = nAn;
   }

   if( WorkAlloc )
   {
      if( php->Ppnc == S_ON && php->sol_t[SPHAS_TYP] == SM_AQSIT )
      {
         php->lsCat = (char (*)[MAXDCNAME])aObj[ o_ph_w_lsc ].Alloc(
                          php->nCat, 1, MAXDCNAME );
         php->lsAn  = (char (*)[MAXDCNAME])aObj[ o_ph_w_lsa ].Alloc(
                          php->nAn, 1, MAXDCNAME );
         php->nxCat = (short *)aObj[ o_ph_w_nxc ].Alloc( php->nCat, 1, I_);
         php->nxAn  = (short *)aObj[ o_ph_w_nxa ].Alloc( php->nAn, 1, I_);
      }
      else {
        php->lsCat = (char (*)[MAXDCNAME])aObj[ o_ph_w_lsc ].Free();
        php->lsAn =  (char (*)[MAXDCNAME])aObj[ o_ph_w_lsa ].Free();
        php->nxCat = (short *)aObj[ o_ph_w_nxc ].Free();
        php->nxAn =  (short *)aObj[ o_ph_w_nxa ].Free();
      }
   }

   if( FillOut )
   {
     for( i=0; i<php->nDC; i++ )
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
          case '@':
          default:
                  continue;
        }
     }
     if( iCat != php->nCat || iAn != php->nAn )
       Error( GetName(),
        "E38PHrem: Mismatch in the number of cations or anions in SIT model...");
   }
}

#define s(i,j) php->scoef[(j)+(i)*nsc]
#define sit(i,j) php->pnc[(j)+(i)*nAn]
// #define m(i,j) php->MaSdj[(j)+(i)*DFCN]
//Recalc record structure
void
TPhase::RecCalc( const char *key )
{
    bool getDCC;

    if( php->PphC == PH_AQUEL && php->sol_t[SPHAS_TYP] == SM_AQSIT )
    {  // refreshing lists for SIT coefficients
       MakeCatAnLists( true, true, true );
    }

    getDCC = vfQuestion(window(), GetName(),
       "Extract parameters from DComp/ReacDC records and refresh DC class codes?");

    if( php->PphC == PH_AQUEL && php->sol_t[SPHAS_TYP] == SM_AQSIT &&
        php->nxAn[0] <= 0 && php->nxCat[0] <=0 )
    {
        getDCC = vfQuestion(window(), GetName(),
          "For the SIT model, please remake the record if not yet done.\n Proceed with extracting DC parameters (Y)?");
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
    int  i, /*iic,*/ pa0=0, Kielland;
    short nsc;
    vstr dcn(MAXRKEYLEN);
    char Ctype;
    float a0, bp, Z, cN, Fi;
    time_t crt;

//    TProfil *aPa=(TProfil *)(&aMod[RT_PARAM]);
    TDComp* aDC=(TDComp *)(&aMod[RT_DCOMP]);
    TReacDC* aRDC=(TReacDC *)(&aMod[RT_REACDC]);
    aDC->ods_link(0);
    aRDC->ods_link(0);

    /* get effective ion radii of aqueous species */
    if( (php->PphC == PH_AQUEL /* || php->PphC == PH_SORPTION ||
                  php->PphC == PH_POLYEL */ ) && php->scoef )
    {
//        nsc = (short)(php->nscN * php->nscM);
        nsc = php->nscM;
    //        nCat = 0; nAn = 0;
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
    memset( dcn, 0, MAXRKEYLEN );
    for( i=0; i<php->nDC; i++ )
    {  /*Get key */
        memcpy( dcn, php->SM[i], DC_RKLEN );
        dcn[DC_RKLEN]=0;
        Ctype = A_NUL;
        /* Read record */
        if( php->DCS[i] == SRC_DCOMP )
        {
            aDC->TryRecInp( dcn, crt, 0 );
            a0 = aDC->dcp->Der;
            bp = aDC->dcp->DerB;
            Z = aDC->dcp->Zz;
            Ctype = aDC->dcp->PdcC;

        }
        else
            if( php->DCS[i] == SRC_REACDC )
            {
                aRDC->TryRecInp( dcn, crt, 0 );
                a0 = aRDC->rcp->Der;
                bp = aRDC->rcp->DerB;
                Z = aRDC->rcp->Zz;
                Ctype = aRDC->rcp->PreC;
            }
        /* test model and types of components */
        if( getDCC==true || php->DCC[i] == A_NUL || php->DCC[i] == '`')
            php->DCC[i] = Ctype;

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
    }  // i

    if( php->PphC == PH_SORPTION || php->PphC == PH_POLYEL )
    {
// Rewritten by KD on 13.09.04 for Frumkin and on 25.10.04 for CD EDL models
//      nsc = (short)(php->nscN * php->nscM);
      nsc = php->nscM;
      memset( dcn, 0, MAXRKEYLEN );
      for( i=0; i<php->nDC; i++ )
      {  /*Get key */
           memcpy( dcn, php->SM[i], DC_RKLEN );
           dcn[DC_RKLEN]=0;
           Ctype = A_NUL;
           /* Read record */
        if( php->DCS[i] == SRC_DCOMP )
        {
            aDC->TryRecInp( dcn, crt, 0 );
            a0 = aDC->dcp->Der;
            bp = aDC->dcp->DerB;
            Z = aDC->dcp->Zz;
            cN = aDC->dcp->Comp;
            Fi = aDC->dcp->Expa;
            Ctype = aDC->dcp->PdcC;
         }
        else if( php->DCS[i] == SRC_REACDC )
        {
             aRDC->TryRecInp( dcn, crt, 0 );
             a0 = aRDC->rcp->Der;
             bp = aRDC->rcp->DerB;
             Z = aRDC->rcp->Zz;
             cN = aRDC->rcp->Comp;
             Fi = aRDC->rcp->Expa;
             Ctype = aRDC->rcp->PreC;
        }
        if( getDCC==true || php->DCC[i] == A_NUL || php->DCC[i] == '`')
            php->DCC[i] = Ctype;

// Transferring data into MaSdj array   KD 25.10.2004
// Now a0 and b0 contain CD (charge distribution) for 0 and beta planes, resp.
        if( php->NR1 == DFCN && php->MaSdj )
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
      } // i
    }

    // Collecting coefficients of EoS for fluids and gases
    if( ( php->PphC == PH_FLUID || php->PphC == PH_LIQUID ||
          php->PphC == PH_GASMIX ) && php->scoef )
    {
        int kx, mcex;
//        nsc = (short)(php->nscN * php->nscM);
        nsc = php->nscM;
        if( pVisor->ProfileMode == true || vfQuestion(window(), GetName(),
   "PRSV or CG2003 fluid EoS coefficients: Collect from DComp records?"))
        {
           memset( dcn, 0, MAXRKEYLEN );
           for( i=0; i<php->nDC; i++ )
           {  /*Get key */
              memcpy( dcn, php->SM[i], DC_RKLEN );
              dcn[DC_RKLEN]=0;
              Ctype = A_NUL;
              /* Read record */
              if( php->DCS[i] == SRC_DCOMP )
              {
                 aDC->TryRecInp( dcn, crt, 0 );
                 if( aDC->dcp->Cemp && !aDC->dcp->CPg )
                 {    // Churakov-Gottschalk model  coefficients
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
                    mcex = min( MAXCRITPARAM, (int)nsc );  // PRSV model
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
                 Ctype = aDC->dcp->PdcC;
              }
              if( getDCC==true || php->DCC[i] == A_NUL || php->DCC[i] == '`')
                 php->DCC[i] = Ctype;
           } // i
        }
     }
// NEXT:  /* define more precisely code of phase  */
//    iic = 0;
    if( php->Asur > 1. )
        if( php->nDC == 1 )
        {
            php->PphC = PH_SINDIS;   /* iic++; */
        }
        else if( php->NsiT >= 1. )
        {
            php->PphC = PH_SORPTION; /* iic++; */
        }
//    if( iic ) goto NEXT;
}

//-------------------------------------------------------------------------
// called from Project - extended by KD on 16.06.03 to add CG EoS
// Re-written (for AutoPhaseWizard) by KD on 31.07.03
//
void TPhase::newAqGasPhase( const char * akey, const char *gkey, int file,
   const char amod, const char gmod, float apar[4], float gpar[4],
   bool useLst, TCStringArray lst )
{
//    TProfil *aPa=(TProfil *)(&aMod[RT_PARAM]);
    const char *part;
    char nbuf[MAXFORMULA];
    gstring Name = "Auto-set ";

//  Setup of aqueous phase
    switch( amod )
    {
       case 'N': // No aqueous phase
       case 'U': // User-selected aqueous phase
                 goto MAKE_GAS_PHASE;
       case 'D': // Davies equation, no a0 needed; opt. bg for neutral aq sp.
                memcpy( php->sol_t, "DNNSNN", 6 );
                memcpy( &php->PphC, "a-----", 6 );
                php->ncpN = 0; php->ncpM = 0;
                php->nscM = 0; php->npxM = 0;
                Name += "ion-association model, Davies equation";
                apar[0] = apar[1] = apar[2] = 0.0;
                sprintf( nbuf, "Parameters: I_max =%-5.3f ", apar[3] );
                break;
       case 'H': // EDH (Helgeson) with common bg and common a0
                memcpy( php->sol_t, "HNNSNN", 6 );
                memcpy( &php->PphC, "a+----", 6 );
                php->ncpN = 2; php->ncpM = 4;
                php->nscM = 0; php->npxM = 0;
                Name += "ion-association model, EDH(H) equation, common ion size";
    sprintf( nbuf, "Parameters: b_gamma= %-5.3f; a_size= %-5.3f; neutral= %-4.1f; I_max =%-5.3f ",
                 apar[0], apar[1], apar[2], apar[3] );
                break;
       case '3': // EDH with Kielland a0 and common bg
                memcpy( php->sol_t, "3NNSNN", 6 );
                memcpy( &php->PphC, "a++---", 6 );
                php->ncpN = 2; php->ncpM = 4;
                php->nscM = 1; php->npxM = 0;
                Name += "ion-association model, EDH(K) equation, Kielland ion sizes";
                apar[1] = 0.0;
    sprintf( nbuf, "Parameters: b_gamma= %-5.3f; neutral= %-4.1f; I_max =%-5.3f ",
                 apar[0], apar[2], apar[3] );
                break;
       case '2': // DH without bg and Kielland a0
                memcpy( php->sol_t, "2NNSNN", 6 );
                memcpy( &php->PphC, "a++---", 6 );
                php->ncpN = 2; php->ncpM = 4;
                php->nscM = 1; php->npxM = 0;
                Name += "ion-association model, DH equation, Kielland ion sizes";
                apar[1] = 0.0;
    sprintf( nbuf, "Parameters: b_gamma= %-5.3f; neutral= %-4.1f; I_max =%-5.3f ",
                 apar[0], apar[2], apar[3] );
                break;
       case '1': // DH limiting law (no a0 and bg required)
                memcpy( php->sol_t, "1NNSNN", 6 );
                memcpy( &php->PphC, "a-----", 6 );
                php->ncpN = 0; php->ncpM = 0;
                php->nscM = 0; php->npxM = 0;
                Name += "ion-association model, Debye-Hueckel limiting law";
                apar[0] = apar[1] = apar[2] = 0.0;
//                apar[3] = 0.01;
                sprintf( nbuf, "Parameters: I_max =%-5.3f ", apar[3] );
                break;
       default: // Unrecognized code - error message ?
       case 'S': // SIT - under construction
                 goto MAKE_GAS_PHASE;
    }
    strcpy( php->name, Name.c_str() );
    strcpy( php->notes, nbuf );
    part = "a:*:*:*:";

// Call assembling of the aqueous phase
    AssemblePhase( akey, part, apar, file, useLst, lst );

MAKE_GAS_PHASE:
    Name = "Auto-set ";
    switch( gmod )
    {
      case 'N': // No gas/fluid phase in the system
      case 'U': // User - selected from database
                goto DONE;
      case 'I':  // Ideal mixture (default)
              memcpy( php->sol_t, "INNINN", 6 );
              memcpy( &php->PphC, "g-----", 6 );
              php->ncpN = 0; php->ncpM = 0;
              php->nscM = 0; php->npxM = 0;
              Name += "ideal mixture of ideal or real gases";
              strcpy( php->name, Name.c_str() );
              strcpy( php->notes,
     "Applicable at low P - elevated T (with CST gases - at elevated P)" );
              break;
      case 'F':  // Fluid CG EoS model
              memcpy( php->sol_t, "FNNSNN", 6 );
              memcpy( &php->PphC, "f-+---", 6 );
              php->ncpN = 0; php->ncpM = 0;
              php->nscM = 20; // 4;  // changed 07.12.2006  KD
              php->npxM = 0;
              Name += "Perturbation-based EoS (Churakov&Gottschalk,2003)";
              strcpy( php->name, Name.c_str() );
              strcpy( php->notes,
     "Applicable at high P - moderate T for mixed non-electrolyte fluids" );
              break;
      case SM_PRFLUID: // 'P': Peng-Robinson EoS, under construction
// Not auto-set phase!
              break;
      default:  // unrecognized code
              goto DONE;
    }
    part = "g:*:*:*:";
    if( gkey[0] == 'f' )
        part = "f:*:*:*:";
// Assembling gas phase
    AssemblePhase( gkey, part, gpar, file, useLst, lst );

// Do sometning else here?
   DONE:
    contentsChanged = false;
}

// Assembling the phase (automatically generated aq or gas/fluid)
// Separated by KD on 31.07.03
void
TPhase::AssemblePhase( const char* key, const char* part, float param[4],
    int file, bool useLst, TCStringArray lst )
{

    TProfil *aPa=(TProfil *)(&aMod[RT_PARAM]);

// Initializing
    php->Nsd = 0;
    php->nDC = 0;
    php->NsiT = php->NR1 = 0;
    php->Asur =   php->Sigma0 =  php->SigmaG =   php->R0p =
                  php->h0p =  php->Eps =  php->Cond =  php->Rsp1 =  0.;

    //Get lists
    TCStringArray aDclist;
    TCIntArray anRDc;
    TCStringArray aRclist;
    TCIntArray anRRc;
    int Ndc, Nrc, iic, i;

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
        php->NR1 = (short)aRclist.GetCount();
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
        for( i=0, php->NR1=0; i<php->nDC; i++ )
        {
            if( lst[i].c_str()[DC_RKLEN-1] == SRC_REACDC)
                php->NR1++;
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

// set model parameters, if necessary  !!!!!!!!!!!!! check !
    if( php->pnc && ( php->ncpN * php->ncpM >= 8 ) )
    {
       php->pnc[5] = param[0];
       php->pnc[6] = param[1];
       php->pnc[7] = param[2];
    }
// Calculating the phase record and saving it to database
    CalcPhaseRecord( true );
    int  Rnum = db->Find( key );
    if( Rnum<0 )
        db->AddRecordToFile( key, file );
    else
        db->Rep( Rnum );
//    contentsChanged = false;
}


void
TPhase::CmHelp()
{
    pVisor->OpenHelp( GEMS_PH_HTML );  //  05.01.01
}

void TPhase::CopyRecords( const char * prfName, TCStringArray& aPHnoused,
            TCStringArray& aPHtmp,
            elmWindowData el_data, phSetupData st_data )
{
    TCIntArray anR;
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
     if( !st_data.flags[PHcopyA_] && ( pKey1[0] == 'a' || pKey1[0] == 'g' ))
       continue;

// cbGaseous
     if( !el_data.flags[cbGaseous_] && pKey1[0] == 'g' )
       continue;

// cbAqueous
     if( !el_data.flags[cbAqueous_] && ( pKey1[0] == 'a' || pKey1[0] == 'x' ))
       continue;

// cbSorption
     if( !el_data.flags[cbSorption_] && pKey1[0] == 'x' )
       continue;
//     if( !st_data.flags[PHcopyY_] && aPHkey[ii][0] == '?' )
//       continue;

// copy liquid phases together with solid ones
     if( !st_data.flags[PHcopyL_] && ( pKey1[0] == 'l' || pKey1[0] == 'h' ))
       continue;

// cbSolids  - single-component
     if( !el_data.flags[cbSolids_] && ( pKey1[0] == 's' ||
          pKey1[0] == 'd' || pKey1[0] == 'l' || pKey1[0] == 'h' )
//        && ( pKey4[0] == 'c' && pKey4[1] == ' ' ||
//          pKey4[0] == 'd' && pKey4[1] == ' ' ||
//          pKey4[0] == 'l' && pKey4[1] == ' ' ))
          && pKey4[1] == ' ' )
        continue;

// cbSolutions - multi-component, non-gas, non-electrolyte
     if( !el_data.flags[cbSolutions_] && ( pKey1[0] == 's'
        || pKey1[0] == 'l' || pKey1[0] == 'd' || pKey1[0] == 'h' )
    //  &&  !( pKey4[0] == 'c' || pKey4[0] == 'd' || pKey4[0] == 'l' ))
          && pKey4[1] != ' ' )
        continue;

       // test the same component (overload) 30/11/2006
       gstring stt = aPHkey[ii].substr(0,MAXSYMB+MAXPHSYMB+MAXPHNAME+MAXSYMB);
       for( j=0; j<aICkey_new.GetCount(); j++ )
          if( stt ==  aICkey_new[j])
              break;
       if( j<aICkey_new.GetCount() )
            continue;

// Read the record here
     RecInput( aPHkey[ii].c_str() );

// Copy non-ideal phases?
     if( !st_data.flags[PHcopyN_] && php->nDC > 1 &&
         php->sol_t[0] != 'N' && php->sol_t[0] !='I'  )
       continue;

// Test existence of DComp/ReacDC records
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
         cnt++;
     } // i

     if( cnt < php->nDC && !( !st_data.flags[PHcopyF_] && cnt > 0  ))
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
     AddRecord( str.c_str(), fnum_ );
     aICkey_new.Add( stt );  // 30/11/2006
   }

    // close all no project files
    TCStringArray names1;
    names1.Add(prfName);
    db->OpenOnlyFromList(names1);
}

// ----------------- End of m_phase.cpp -------------------------

