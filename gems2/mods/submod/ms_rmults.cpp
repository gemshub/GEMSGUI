//-------------------------------------------------------------------
// $Id$
//
// Implementation of TRMults class, config and  load functions
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
// See http://les.web.psi.ch/Software/GEMS-PSI for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//
#include "v_object.h"
#include "m_param.h"
#include "m_phase.h"
#include "m_icomp.h"
#include "m_dcomp.h"
#include "m_compos.h"
#include "m_reacdc.h"
#include "s_formula.h"
#include "service.h"
#include "visor.h"


const char * defaultAqKey = "a   AQELIA  aq_gen          aq  Generic         ";
const char * defaultGasKey = "g   GASMXID gas_gen         gm  Generic         ";

TRMults::TRMults( int nrt ):
        TSubModule( nrt )
{
    set_def();
}

// link values to objects
void TRMults::ods_link( int /*q*/ )
{
    //aObj[ o_musymb].SetPtr(  mu.symb );
    //aObj[ o_mugrp].SetPtr(   mu.grp );
    aObj[ o_mupmv ].SetPtr(  &mu.PmvSA );   /*a 8*/
    aObj[ o_muname].SetPtr(  mu.name );
    aObj[ o_munotes].SetPtr( mu.notes );
    aObj[ o_mudim].SetPtr(  &mu.N );   /*i 12*/
    aObj[ o_munfl].SetPtr(  mu.Nfl );

    aObj[ o_mul1].SetPtr( mu.Ll );
    aObj[ o_mul1].SetDim( mu.Fi, 1 );
    aObj[ o_musf].SetPtr( mu.SF );
    aObj[ o_musf].SetDim( mu.Fi, 1 );
    aObj[ o_musm].SetPtr(  mu.SM );
    aObj[ o_musm].SetDim( mu.L, 1 );
    aObj[ o_musa].SetPtr( mu.SA );
    aObj[ o_musa].SetDim( mu.La, 1 );
    aObj[ o_musb].SetPtr( mu.SB );
    aObj[ o_musb].SetDim( mu.N, 1 );
    aObj[ o_mufn].SetPtr( mu.FN );
    aObj[ o_mufn].SetDim( mu.NfT, 1 );
    aObj[ o_musf2].SetPtr( mu.SF2 );
    aObj[ o_musf2].SetDim( mu.Fis, 1 );
    aObj[ o_musm2].SetPtr(  mu.SM2 );
    aObj[ o_musm2].SetDim( mu.Ls, 1 );

    aObj[ o_muphc].SetPtr( mu.PHC );
    aObj[ o_muphc].SetDim( mu.Fi, 1 );
    aObj[ o_mudcc].SetPtr( mu.DCC );
    aObj[ o_mudcc].SetDim( mu.L, 1 );
    aObj[ o_mudcs].SetPtr( mu.DCS );
    aObj[ o_mudcs].SetDim( mu.L, 1 );
    aObj[ o_muicc].SetPtr( mu.ICC );
    aObj[ o_muicc].SetDim( mu.N, 1 );


    aObj[ o_mudcf].SetPtr( mu.DCF );  //aObj[ o_mudcf].SetDim( 1,VIZmsize(mu.DCF));
    aObj[ o_muicf].SetPtr( mu.ICF );  //aObj[ o_muicf].SetDim( 1,VIZmsize(mu.ICF));
    aObj[ o_muidcref].SetPtr( mu.IDCref );
    aObj[ o_muidcref].SetDim( mu.N, 1 );
    aObj[ o_muval].SetPtr( mu.Val );
    aObj[ o_muval].SetDim( mu.N, 1 );
    aObj[ o_mupl].SetPtr( mu.Pl );
    aObj[ o_mupl].SetDim( mu.Ls, 1 );
    aObj[ o_mubc].SetPtr( mu.BC );
    aObj[ o_mubc].SetDim( mu.N, 1 );
    aObj[ o_muas].SetPtr( mu.AS );
    aObj[ o_muas].SetDim( mu.N, 1 );
    aObj[ o_muav].SetPtr( mu.AV );
    aObj[ o_muav].SetDim( mu.N, 1 );
}

// set dynamic Objects ptr to values
void TRMults::dyn_set(int /*q*/)
{
    mu.SF  = (char (*)[PH_RKLEN])aObj[ o_musf ].GetPtr();
    mu.SM  = (char (*)[DC_RKLEN])aObj[ o_musm ].GetPtr();
    mu.SF2  = (char (*)[PH_RKLEN])aObj[ o_musf2 ].GetPtr();
    mu.SM2  = (char (*)[DC_RKLEN])aObj[ o_musm2 ].GetPtr();
    mu.SA  = (char (*)[BC_RKLEN])aObj[ o_musa ].GetPtr();
    mu.SB  = (char (*)[IC_RKLEN])aObj[ o_musb ].GetPtr();
    mu.FN  = (char (*)[FileNameLen])aObj[ o_mufn ].GetPtr();
    mu.PHC = (char *)aObj[ o_muphc ].GetPtr();
    mu.DCC = (char *)aObj[ o_mudcc ].GetPtr();
    mu.DCS = (char *)aObj[ o_mudcs ].GetPtr();
    mu.Pl  = (short *)aObj[ o_mupl ].GetPtr();
    mu.ICC = (char *)aObj[ o_muicc ].GetPtr();
    mu.DCF = (char *)aObj[ o_mudcf ].GetPtr();
    mu.ICF = (char *)aObj[ o_muicf ].GetPtr();
    mu.IDCref = (char (*)[DC_RKLEN])aObj[ o_muidcref ].GetPtr();
    mu.Ll  = (short *)aObj[ o_mul1 ].GetPtr();
    mu.Val = (short *)aObj[ o_muval ].GetPtr();
    mu.BC  = (float *)aObj[ o_mubc ].GetPtr();
    mu.AS  = (float *)aObj[ o_muas ].GetPtr();
    mu.AV  = (float *)aObj[ o_muav ].GetPtr();
}

// free dynamic memory in objects and values
void TRMults::dyn_kill(int /*q*/)
{
    mu.SF  = (char (*)[PH_RKLEN])aObj[ o_musf ].Free();
    mu.SM  = (char (*)[DC_RKLEN])aObj[ o_musm ].Free();
    mu.SF2  = (char (*)[PH_RKLEN])aObj[ o_musf2 ].Free();
    mu.SM2  = (char (*)[DC_RKLEN])aObj[ o_musm2 ].Free();
    mu.SA  = (char (*)[BC_RKLEN])aObj[ o_musa ].Free();
    mu.SB  = (char (*)[IC_RKLEN])aObj[ o_musb ].Free();
    mu.FN  = (char (*)[FileNameLen])aObj[ o_mufn ].Free();
    mu.PHC = (char *)aObj[ o_muphc ].Free();
    mu.DCC = (char *)aObj[ o_mudcc ].Free();
    mu.DCS = (char *)aObj[ o_mudcs ].Free();
    mu.Pl  = (short *)aObj[ o_mupl ].Free();
    mu.ICC = (char *)aObj[ o_muicc ].Free();
    mu.DCF = (char *)aObj[ o_mudcf ].Free();
    mu.ICF = (char *)aObj[ o_muicf ].Free();
    mu.IDCref = (char (*)[DC_RKLEN])aObj[ o_muidcref ].Free();
    mu.Ll  = (short *)aObj[ o_mul1 ].Free();
    mu.Val = (short *)aObj[ o_muval ].Free();
    mu.BC  = (float *)aObj[ o_mubc ].Free();
    mu.AS  = (float *)aObj[ o_muas ].Free();
    mu.AV  = (float *)aObj[ o_muav ].Free();
}

// realloc dynamic memory
void TRMults::dyn_new(int /*q*/)
{
    mu.FN = (char (*)[FileNameLen])aObj[ o_mufn].Alloc( mu.NfT, 1, FileNameLen );
    if( mu.PmvIC == S_ON )
    {
        mu.SB = (char (*)[IC_RKLEN])aObj[ o_musb].Alloc( mu.N, 1, IC_RKLEN );
        mu.ICC = (char *)aObj[ o_muicc].Alloc( mu.N, 1, A_ );
        mu.Val = (short *)aObj[ o_muval].Alloc( mu.N, 1, I_ );
        mu.BC = (float *)aObj[ o_mubc].Alloc( mu.N, 1, F_ );
    }
    else  Error( GetName(), "PmvIC != S_ON" );

    if( mu.PmvIP == S_ON )
    {
        mu.AS = (float *)aObj[ o_muas].Alloc( mu.N, 1, F_ );
        mu.AV = (float *)aObj[ o_muav].Alloc( mu.N, 1, F_ );
    }
    else
    {
        mu.AS  = (float *)aObj[ o_muas ].Free();
        mu.AV  = (float *)aObj[ o_muav ].Free();
    }

    if( mu.PmvID == S_ON )
        mu.IDCref = (char (*)[DC_RKLEN])aObj[o_muidcref].Alloc(mu.N, 1, DC_RKLEN );
    else  mu.IDCref = (char (*)[DC_RKLEN])aObj[ o_muidcref ].Free();

    if( mu.PmvIF == S_ON )
        mu.ICF = (char *)aObj[ o_muicf].Alloc( 1, mu.N*10, S_ );
    else mu.ICF = (char *)aObj[ o_muicf ].Free();

    if( mu.PmvSA == S_ON )
        mu.SA = (char (*)[BC_RKLEN])aObj[ o_musa].Alloc( mu.La, 1, BC_RKLEN );
    else mu.SA  = (char (*)[BC_RKLEN])aObj[ o_musa ].Free();

    if( mu.PmvPH == S_ON )
    {
        mu.SF = (char (*)[PH_RKLEN])aObj[ o_musf].Alloc( mu.Fi, 1, PH_RKLEN );
        mu.SF2 = (char (*)[PH_RKLEN])aObj[ o_musf2].Alloc( mu.Fis, 1, PH_RKLEN );
        mu.Ll = (short *)aObj[ o_mul1].Alloc( mu.Fi, 1, I_ );
        mu.PHC = (char *)aObj[ o_muphc].Alloc( mu.Fi, 1, A_ );
    }
    else Error( GetName(), "PmvPH != S_ON" );

    if( mu.PmvDC == S_ON )
    {
        mu.SM = (char (*)[DC_RKLEN])aObj[ o_musm].Alloc( mu.L, 1, DC_RKLEN );
        mu.SM2 = (char (*)[DC_RKLEN])aObj[ o_musm2].Alloc( mu.Ls, 1, DC_RKLEN );
        mu.DCC = (char *)aObj[ o_mudcc].Alloc( mu.L, 1, A_ );
        mu.DCS = (char *)aObj[ o_mudcs].Alloc( mu.L, 1, A_ );
        if( mu.Ls )
            mu.Pl = (short *)aObj[ o_mupl].Alloc( mu.Ls, 1, I_ );
        else  mu.Pl  = (short *)aObj[ o_mupl ].Free();
    }
    else
    {
        mu.SM  = (char (*)[DC_RKLEN])aObj[ o_musm ].Free();
        mu.SM2  = (char (*)[DC_RKLEN])aObj[ o_musm2 ].Free();
        mu.DCC = (char *)aObj[ o_mudcc ].Free();
        mu.DCS = (char *)aObj[ o_mudcs ].Free();
        mu.Pl  = (short *)aObj[ o_mupl ].Free();
    }
    if( mu.PmvDF == S_ON )
    {
        short DCFlen = mu.FiE;
        mu.FiE = 0;
        mu.DCF = (char *)aObj[ o_mudcf].Alloc( 1, DCFlen*MAXFORMULA, S_ );
    }
    else  mu.DCF = (char *)aObj[ o_mudcf ].Free();
}


//set default information
void TRMults::set_def( int /*q*/)
{
    TProfil *aPa=(TProfil *)(&aMod[RT_PARAM]);
    memcpy( &mu.PmvSA, aPa->pa.MUpmv, 10 );
//    strcpy( mu.name, "`");
//    strcpy( mu.notes, "`");
    memset( mu.name, 0, MAXFORMULA);
    memset( mu.notes, 0, MAXFORMULA);
    strncpy( mu.name, rt[RT_PARAM].FldKey(0), rt[RT_PARAM].FldLen(0) );
    strncpy( mu.notes, rt[RT_PARAM].FldKey(1), rt[RT_PARAM].FldLen(1) );
    memset( &mu.N, 0, 14*sizeof(short));
    mu.SF  = 0;
    mu.SM  = 0;
    mu.SF2  = 0;
    mu.SM2  = 0;
    mu.SA  = 0;
    mu.SB  = 0;
    mu.FN  = 0;
    mu.PHC = 0;
    mu.DCC = 0;
    mu.DCS = 0;
    mu.Pl  = 0;
    mu.ICC = 0;
    mu.DCF = 0;
    mu.ICF = 0;
    mu.IDCref = 0;
    mu.Ll  = 0;
    mu.Val = 0;
    mu.BC  = 0;
    mu.AS  = 0;
    mu.AV  = 0;
    //GetFileCfg();
}

// Make list of DComp and React record keys on PHases

void TRMults::DCListLoad(  gstring& AqKey, gstring& GasKey,
                           bool useLst, TCStringArray lst )
{
    uint k;
    int j, kk;
    time_t tim;
    int SPHP=1;  /* at first test phases-solution */

    // Get all records of PHase
    TCStringArray aPhaseList;
    TCIntArray anRPhase;
    if( useLst == false )
    {
        rt[RT_PHASE].GetKeyList( "*:*:*:*:*:", aPhaseList, anRPhase );
        uint ii=0;
        while( ii < aPhaseList.GetCount() )
        {
            if( *aPhaseList[ii].c_str() == 'a' && aPhaseList[ii] !=  AqKey )
            {
                aPhaseList.Remove(ii);
                anRPhase.Remove(ii);
            }
            else
                if( *aPhaseList[ii].c_str() == 'g' && aPhaseList[ii] !=  GasKey )
                    /*memcmp( aPhaseList[ii], GasKey.c_str(), PH_RKLEN ))*/
                { aPhaseList.Remove(ii);
                    anRPhase.Remove(ii);
                }
                else
                { //Sveta 21/09/99 Compare first 4 fields delete last
                    // overite mehanism from Dima
                    if( ii > 0)
                    {
                        if( !strncmp(aPhaseList[ii-1].c_str(),
                                     aPhaseList[ii].c_str(), PH_RKLEN-MAXPHGROUP  ))
                        {
                            aPhaseList.Remove(ii);
                            anRPhase.Remove(ii);
                        }
                        else ii++;
                    }
                    else ii++;
                }
        }
    }
    else
    {
        for( uint ii=0; ii<lst.GetCount(); ii++)
            aPhaseList.Add(lst[ii]);
    }

    if( aPhaseList.GetCount()<1 )
        Error("RMULT", "No records of Phases!");
    mu.Fi  = aPhaseList.GetCount();
    mu.PmvPH = S_ON;
    mu.Ll = (short *)aObj[ o_mul1].Alloc( mu.Fi, 1, I_ );
    mu.SF = (char (*)[PH_RKLEN])aObj[ o_musf].Alloc( mu.Fi, 1, PH_RKLEN );
    // List of DCOMP&REACDC component in Project
    TCStringArray List;
    TPhase* aPH=(TPhase *)(&aMod[RT_PHASE]);
    aPH->ods_link(0);
    // Build list of DCOMP&REACDC from PHASE
    kk=-1;
    mu.Ls = mu.Fis = 0;
TEST2:
    for( k=0; k<aPhaseList.GetCount(); k++)
    {
        // test and load keys of DCOMP anf REACDC
        aPH->TryRecInp( aPhaseList[k].c_str(), tim, 0 );
        if( aPH->php->nDC > 1 && SPHP==0 )
            continue;
        if( aPH->php->nDC <= 1 && SPHP==1 )
            continue;
        kk++;
        memcpy( mu.SF[kk], aPhaseList[k].c_str(), PH_RKLEN );
        mu.Ll[kk] = 0;
        for( j=0; j<aPH->php->nDC; j++ )
        {
            // test to exist of DCOMP or REACDC record later
            vstr ss(DC_RKLEN, aPH->php->SM[j]);
            List.Add(ss.p);
            mu.Ll[kk]++;
        } /* j */
        if( mu.Ll[kk] > 1 ) // multicomponent phase
        {
            mu.Ls += mu.Ll[kk];
            mu.Fis++;
        }
    } /* k */
    if( SPHP==1)
    {
        SPHP=0;
        goto TEST2;
    }
    mu.L = List.GetCount();
    mu.FiE= mu.L;
    mu.PmvDC = S_ON;
    mu.SM = (char (*)[DC_RKLEN])aObj[ o_musm].Alloc( mu.L, 1, DC_RKLEN );
    for(uint i=0; i< List.GetCount(); i++)
        memcpy( mu.SM[i], List[i].c_str(), DC_RKLEN );
}

//Make record of base Rmults from files
void TRMults::MakeRecordLists( gstring& AqKey, gstring& GasKey )
{
    uint i;
    // Get all records of IComp
    TCStringArray aICList;
    TCIntArray anRIC;
    rt[RT_ICOMP].GetKeyList( "*:*:*:", aICList, anRIC );
    if( aICList.GetCount()<1 )
        Error("RMULT", "No records of Independent component!");
    mu.N  = aICList.GetCount();
    mu.PmvIC = S_ON;
    mu.SB = (char (*)[IC_RKLEN])aObj[ o_musb].Alloc( mu.N, 1, IC_RKLEN );
    for( i=0; i< aICList.GetCount(); i++)
        memcpy( mu.SB[i], aICList[i].c_str(), IC_RKLEN );

    // Get all records of Compos
    TCStringArray aCompList;
    TCIntArray anRComp;
    rt[RT_COMPOS].GetKeyList( "*:*:*:", aCompList, anRComp );
    mu.La = aCompList.GetCount();
    if( mu.La<1 )
        mu.PmvSA = S_OFF;
    else
    {
        mu.PmvSA = S_ON;
        mu.SA = (char (*)[BC_RKLEN])aObj[ o_musa].Alloc( mu.La, 1, BC_RKLEN );
        for( i=0; i< aCompList.GetCount(); i++)
            memcpy( mu.SA[i], aCompList[i].c_str(), BC_RKLEN );
    }
    // Get all records of PHase  and Get DCOMP&REACT list
    DCListLoad( AqKey, GasKey );
}

// Make list of ICOMP and set data to work arrays
void TRMults::ICmake()
{
    int ii, j;
    time_t crt;
    TIComp* aIC=(TIComp *)(&aMod[RT_ICOMP]);
    aIC->ods_link(0);

    for( ii=0; ii<mu.N; ii++ )
    {
        // read record ICOMP
        aIC->TryRecInp( mu.SB[ii], crt, 0 );
        mu.ICC[ii] = mu.SB[ii][MAXICNAME];  /* icp->type[0] */
        if( isdigit( mu.ICC[ii] ))
            mu.ICC[ii] = IC_ISOTOPE;
        // load atom ves and other data from record
        if( IsFloatEmpty( aIC->icp->awt ))
            mu.BC[ii] = 0;
        else mu.BC[ii] = aIC->icp->awt;
        mu.Val[ii] = aIC->icp->val;
        if( mu.PmvID == S_ON )
            memcpy( mu.IDCref[ii], aIC->icp->dc_ref, DC_RKLEN );
        if( mu.PmvIF == S_ON )
        {  // formule of IC do more !!
            if( !ii ) mu.ICF[0] = 0;
            j = strlen( mu.ICF );
            if( ii )
            {
                mu.ICF[j++] = ',';
                mu.ICF[j++] = '\n';
            }
            memcpy( mu.ICF+j, aIC->icp->form, strlen(aIC->icp->form) );
            *(mu.ICF+j+strlen(aIC->icp->form))='\0';
        }
        if( mu.PmvIP == S_ON )
        {
            if( IsFloatEmpty( aIC->icp->aSs ))
                mu.AS[ii] = 0;
            else mu.AS[ii] = aIC->icp->aSs;
            if( IsFloatEmpty( aIC->icp->aVs ))
                mu.AV[ii] = 0;
            else mu.AV[ii] = aIC->icp->aVs;
        }
    }
    if( mu.PmvIF == S_ON )
        mu.ICF = (char *)aObj[ o_muicf].Alloc( mu.N, j+strlen(mu.ICF)+2, S_ );
}

// Make list of PHASE and set data to work arrays
void TRMults::PHmake()
{
    int  jj, ii,  kk, forlen, sflast=-1;
    short ij, i;
    time_t crt;
    char *Formula, *item, dkey[MAXRKEYLEN];
    TPhase* aPH=(TPhase *)(&aMod[RT_PHASE]);
    aPH->ods_link(0);
    TDComp* aDC=(TDComp *)(&aMod[RT_DCOMP]);
    aDC->ods_link(0);
    TReacDC* aRC=(TReacDC *)(&aMod[RT_REACDC]);
    aRC->ods_link(0);

    mu.FiE = 0;
    if( mu.PmvDF == S_ON )
    {
        mu.DCF[0] = 0;
        item = mu.DCF;
    }
    ii=0;
    for( kk=0; kk<mu.Fi; kk++ ) // phase list
    {
        jj=ii;
        ii=jj+mu.Ll[kk];
        // read Phase record
        aPH->TryRecInp( mu.SF[kk], crt, 0 );
        mu.PHC[kk] = aPH->php->PphC;
        // test and load   DCOMP and REACDC
        memset( dkey, 0, MAXRKEYLEN );

        for( ; jj<ii; jj++ )
        {
            memcpy( dkey, mu.SM[jj], DC_RKLEN );
            ij = 0;
            if( aPH->php->nDC > 1 )
            {    /*  search key in phase list */
                for( i=0, ij=-1; i<aPH->php->nDC; i++ )
                {
                    if( memcmp( dkey, aPH->php->SM[i], DC_RKLEN-MAXSYMB ))
                        continue;
                    ij=i;
                    break;
                }
                ErrorIf(ij<0,"RMULTS", "!!!!!!!!!!!!!!!!!");
                if( jj<mu.Ls ) mu.Pl[jj] = ij;
            }
            memcpy( dkey, aPH->php->SM[ij], DC_RKLEN );
            if( aPH->php->DCS[ij] == SRC_DCOMP )
            {
                TDComp::pm->TryRecInp( dkey, crt, 0 );
                Formula = aDC->dcp->form;
            }
            if( aPH->php->DCS[ij] == SRC_REACDC )
            {
                TReacDC::pm->TryRecInp( dkey, crt, 0 );
                Formula = aRC->rcp->form;
            }
            /* dependent component read! */
            mu.DCS[jj] =  aPH->php->DCS[ij]; /* mu[p].DCS[jj]; */
            mu.DCC[jj] =  aPH->php->DCC[ij]; /* mu[p].DCC[jj]; */
            if( mu.PmvDF == S_ON )
            { // insert dependent component formula
                forlen = strlen( Formula );
                for( ; forlen >0; forlen-- )
                    if( Formula[forlen-1] != ' ' )
                        break;
                ErrorIf( !forlen, "RMUL", "No formula in DECOMP or REACDC!" );
                if( sflast < 0 )
                    sflast = strlen( mu.DCF );
                if( sflast )
                {
                    item[sflast++] = ',';
                    item[sflast++] = '\n';
                }
                strncpy( item+sflast, Formula, forlen );
                sflast+=forlen;
                item[sflast]=0;
            }
        } /* jj */
        switch( aPH->php->PphC )
        {
        case PH_AQUEL:
            mu.Laq = mu.Ll[kk];
            continue;
        case PH_PLASMA:
        case PH_GASMIX:
            mu.Pg = mu.Ll[kk];
            continue;
        case PH_HCARBL:
            mu.Lhc = mu.Ll[kk];
            continue;
        case PH_SORPTION:
            mu.FiE += mu.Ll[kk];
        default:
            continue;
        }
    } /* kk */
    mu.DCF = (char *)aObj[ o_mudcf].Alloc( 1, sflast+2, S_ );
}

//realloc memory and load data to structure RMULTS
void TRMults::LoadRmults( bool NewRec, bool changePhases )
{
    int file;
    gstring AqKey;
    gstring  GasKey;
    char emod = SM_AQDAV;    // Added KD 25.01.02

    if( NewRec == false )
    { // Select aqueous and gaseous phase
        if( mu.nAq >= 0)
            AqKey = gstring( mu.SF[mu.nAq], 0, PH_RKLEN );
        else AqKey == "";
        if( mu.nGas >= 0 )
            GasKey = gstring( mu.SF[mu.nGas], 0, PH_RKLEN );
        else GasKey ="";
    }

    NEW_AQ_PHASE_AGAIN:
    if( changePhases || mu.PmvAq == S_ON )
    {     // modified KD 25.01.02 to implement default Davies eqn
        switch( vfQuestion3( window(), "Project:", "Which aqueous phase model?",
               "&Auto DHH3", "Auto &Davies", "&Select from list" ))
        {
        case VF3_1:
            emod = SM_AQDH3; mu.PmvAq = emod;
            break;
        case VF3_2:
            emod = SM_AQDAV; mu.PmvAq = emod;
            break;
        case VF3_3:
            mu.PmvAq = S_REM; AqKey = "a";
            break;
        }
    }
    if( (mu.PmvAq == S_REM) &&
            (( NewRec== true ) ||  ( NewRec== false &&
                                     rt[RT_PHASE].Find(AqKey.c_str())<0 && AqKey[0] != 'a')))
        AqKey = SelectAqPhase(); // Select aqueous phase from list

    if( mu.PmvAq == S_ON )
     // No phase to select from list or not selected - make default
     goto NEW_AQ_PHASE_AGAIN;

    gstring prfName = gstring( rt[RT_PARAM].FldKey(0), 0, rt[RT_PARAM].FldLen(0) );
    StripLine( prfName );
    file =rt[RT_PHASE].GetOpenFileNum( prfName.c_str() );
    // make a default aqueous phase
    if( mu.PmvAq == SM_AQDH3 || mu.PmvAq == SM_AQDAV )  // default aqueous phase mode
    {
        emod = mu.PmvAq;
        TPhase::pm->newAqGasPhase( defaultAqKey, file, emod );
        AqKey = defaultAqKey;
    }

    if( changePhases )
    {
        switch( vfQuestion3( window(), "Project:", "Which gas phase model?",
                             "&Default", "&Select from list", "Remove" ))
        {
        case VF3_1:
            mu.PmvGas = S_ON;
            break;
        case VF3_2:
            mu.PmvGas = S_REM;
            GasKey = "g";
            break;
        case VF3_3:  // Remove gas phase
            mu.PmvGas = S_OFF;
            break;
        }
    }
    if( (mu.PmvGas != S_ON) &&
            (( NewRec== true ) || ( NewRec== false &&
                                    rt[RT_PHASE].Find(GasKey.c_str())<0 && GasKey[0] != 'g')))
        GasKey = SelectGasPhase(); // Select gaseous phase def

    if( mu.PmvGas == S_ON )  // default gaseous phase mode
    {
        TPhase::pm->newAqGasPhase( defaultGasKey, file );
        GasKey = defaultGasKey;
    }

    MakeRecordLists( AqKey, GasKey ); // build records lists and calc size of arrays


    dyn_new();         // alloc memory to structure RMULTS
    ICmake();          // load data from ICOMP
    PHmake();          // load data from PHASE and DCOMP&REACDC

    // insert data to short key lists
    memcpy( mu.SF2, mu.SF, mu.Fis*PH_RKLEN*sizeof(char));
    memcpy( mu.SM2, mu.SM, mu.Ls*DC_RKLEN*sizeof(char));
    // test data base ICOMP  before calc
    TestIComp();

    // Set nAq and nGas
    SetAqGas( AqKey.c_str() , GasKey.c_str() );
}

// Select aqueous phase from available or make default
gstring TRMults::SelectAqPhase()
{
    int i;
    TCStringArray aPhaseList;
    TCIntArray anRPhase;

    if( mu.PmvAq != S_REM )  // default phase mode
        goto DEF_KEY;
    // Get all records of Phase
    rt[RT_PHASE].GetKeyList( "a:*:*:*:*:", aPhaseList, anRPhase );
    if( aPhaseList.GetCount()<1 ) //no aqueous phase in open data base files
    {
        if( vfQuestion( window(), "Project: What aqueous phase?",
            "No definitions of aqueous phase available!\n Make default(Yes) or remove (No)?" ))
        {
          mu.PmvAq = S_ON; return "";
        }
        else {
          mu.PmvAq = S_OFF; return "";
        }
    }
    if( aPhaseList.GetCount()==1 )
        return  aPhaseList[0];
    // select phase
//    do
//    {
AGAIN:  i = vfChoice(window(), aPhaseList, "Please, choose an aqueous phase.");
//    }
//    while( i<0 );
    if( i < 0 )
    {            // rejected
      if( vfQuestion( window(), "Project: What aqueous phase?",
            "No definitions of aqueous phase selected!\n Choose again (Yes) or remove (No)?" ))
      goto AGAIN;
      else {
          mu.PmvAq = S_OFF; return "";
      }
    }
    else return  aPhaseList[i];
DEF_KEY:
    mu.PmvAq = S_ON;
    return gstring(defaultAqKey);

}

// Select gaseous phase from all or make default
gstring TRMults::SelectGasPhase()
{
    int i;
    TCStringArray aPhaseList;
    TCIntArray anRPhase;

    if( mu.PmvGas == S_ON )  // default phase mode
        goto DEF_KEY;
    // Get all records of PHase
    rt[RT_PHASE].GetKeyList( "g:*:*:*:*:", aPhaseList, anRPhase );
    if( aPhaseList.GetCount()<1 ) //no aqueous phase in open data base files
    { // Sveta 17/05/99  mu.PmvGas = S_ON;  goto DEF_KEY;
        return "";
    }
    if( aPhaseList.GetCount()==1 )
        return  aPhaseList[0];
    // select phase
    do
    {
        i = vfChoice(window(), aPhaseList, "mu_gaskey Please, choose gaseous phase.");
    }
    while( i<0 );
    return  aPhaseList[i];
DEF_KEY:
    return gstring(defaultGasKey);

}

//Set indexes of selected aqueous and gaseous phase
void TRMults::SetAqGas( const char* AqKey, const char* GasKey )
{
    mu.nAq = mu.nGas = -1;
    for(short kk=0, ii=0; kk<mu.Fi; kk++ ) // phase list
    {
        //aqueous phase
        if( !memcmp( mu.SF[kk], AqKey, PH_RKLEN ) )
        {
            mu.nAq=kk;
            ii++;
        }
        //gaseous phase
        if( !memcmp( mu.SF[kk], GasKey, PH_RKLEN ) )
        {
            mu.nGas=kk;
            ii++;
        }
        if( ii >= 2 )
            break;
    }
}

//Test ICOMPS in formula DComp, ReactDC, Compos
void TRMults::TestIComp()
{
    int  ii, j, i, ij;
    TFormula aFo;
    gstring form;

    // test formules React&DComp
    for( ii=0; ii<mu.L; ii++ )
    {
        form = aFo.form_extr( ii, mu.L, mu.DCF );
        aFo.SetFormula( form.c_str() ); // and ce_fscan
        aFo.TestIC( mu.SM[ii], mu.N, (char *)mu.SB);
    }
    // test compos
    TCompos* aCMP=(TCompos *)(&aMod[RT_COMPOS]);
    time_t crt;
    aCMP->ods_link(0);
    for( j=0; j<mu.La; j++ )
    { /* read record */
        aCMP->TryRecInp( mu.SA[j], crt, 0 );
        for( i=0; i< aCMP->bcp->N; i++ )
        {
            for( ii=0, ij=0; ii<mu.N; ii++ )
                if( !memcmp( mu.SB[ii], aCMP->bcp->SB[i], MAXICNAME+MAXSYMB ))
                    ij++;
            if( !ij )
            {
                gstring msg= "IComp: ";
                msg += gstring(aCMP->bcp->SB[i], 0,MAXICNAME+MAXSYMB );
                msg += "\n in Compos record: \n";
                msg += gstring(mu.SA[j], 0, BC_RKLEN );

                Error( "Illegal IComp", msg.c_str());
            }
        } /* i */
    } /*j*/

}
//--------------------- End of ms_rmults.cpp ---------------------------

