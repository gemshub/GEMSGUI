//-------------------------------------------------------------------
// $Id: ms_rmults.cpp 1409 2009-08-21 15:34:43Z gems $
//
// Implementation of TRMults class, config and  load functions
//
// Rewritten from C to C++ by S.Dmytriyeva
// Copyright (C) 1995-2012 S.Dmytriyeva, D.Kulik
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
#include "ms_rmults.h"
#include "m_param.h"
#include "m_phase.h"
#include "m_icomp.h"
#include "m_dcomp.h"
#include "m_compos.h"
#include "m_reacdc.h"
#include "s_formula.h"
#include "visor.h"

TRMults* TRMults::sm;

TRMults::TRMults( int nrt ):
        TSubModule( nrt )
{
    set_def();
}

// link values to objects
void TRMults::ods_link( int /*q*/ )
{
    //aObj[ o_musymb]->SetPtr(  mu.symb );
    //aObj[ o_mugrp]->SetPtr(   mu.grp );
    aObj[ o_mupmv ]->SetPtr(  &mu.PmvSA );   /*a 8*/
    aObj[ o_muname]->SetPtr(  mu.name );
    aObj[ o_munotes]->SetPtr( mu.notes );
    aObj[ o_mudim]->SetPtr(  &mu.N );   /*i 12*/
    aObj[ o_munfl]->SetPtr(  mu.Nfl );

    aObj[ o_mul1]->SetPtr( mu.Ll );
    aObj[ o_mul1]->SetDim( mu.Fi, 1 );
    aObj[ o_musf]->SetPtr( mu.SF );
    aObj[ o_musf]->SetDim( mu.Fi, 1 );
    aObj[ o_musm]->SetPtr(  mu.SM );
    aObj[ o_musm]->SetDim( mu.L, 1 );
    aObj[ o_musa]->SetPtr( mu.SA );
    aObj[ o_musa]->SetDim( mu.La, 1 );
    aObj[ o_musb]->SetPtr( mu.SB );
    aObj[ o_musb]->SetDim( mu.N, 1 );
    aObj[ o_mufn]->SetPtr( mu.FN );
    aObj[ o_mufn]->SetDim( mu.NfT, 1 );
    aObj[ o_musf2]->SetPtr( mu.SF2 );
    aObj[ o_musf2]->SetDim( mu.Fis, 1 );
    aObj[ o_musm2]->SetPtr(  mu.SM2 );
    aObj[ o_musm2]->SetDim( mu.Ls, 1 );
aObj[ o_musm3]->SetPtr(  mu.SM3 );
aObj[ o_musm3]->SetDim( mu.Lads, 1 );
aObj[ o_nlicv]->SetPtr(  mu.nlICv );
aObj[ o_nlicv]->SetDim( mu.N, 1 );
aObj[ o_nlich]->SetPtr(  mu.nlICv );
aObj[ o_nlich]->SetDim( 1, mu.N );
aObj[ o_nldcv]->SetPtr(  mu.nlDCv );
aObj[ o_nldcv]->SetDim( mu.L, 1 );
aObj[ o_nldch]->SetPtr(  mu.nlDCv );
aObj[ o_nldch]->SetDim( 1, mu.L );
aObj[ o_nldcvs]->SetPtr(  mu.nlDCv );
aObj[ o_nldcvs]->SetDim( mu.Ls, 1 );
aObj[ o_nldchs]->SetPtr(  mu.nlDCv );
aObj[ o_nldchs]->SetDim( 1, mu.Ls );
aObj[ o_nlphv]->SetPtr(  mu.nlPHv );
aObj[ o_nlphv]->SetDim( mu.Fi, 1 );
aObj[ o_nlphh]->SetPtr(  mu.nlPHv );
aObj[ o_nlphh]->SetDim( 1, mu.Fi );
// end of add
    aObj[ o_muphc]->SetPtr( mu.PHC );
    aObj[ o_muphc]->SetDim( mu.Fi, 1 );
    aObj[ o_mudcc]->SetPtr( mu.DCC );
    aObj[ o_mudcc]->SetDim( mu.L, 1 );
aObj[ o_mudcc3]->SetPtr( mu.DCC3 );
aObj[ o_mudcc3]->SetDim( mu.Lads, 1 );
    aObj[ o_mudcs]->SetPtr( mu.DCS );
    aObj[ o_mudcs]->SetDim( mu.L, 1 );
    aObj[ o_muicc]->SetPtr( mu.ICC );
    aObj[ o_muicc]->SetDim( mu.N, 1 );

    aObj[ o_mudcf]->SetPtr( mu.DCF );  //aObj[ o_mudcf]->SetDim( 1,VIZmsize(mu.DCF));
    aObj[ o_muicf]->SetPtr( mu.ICF );  //aObj[ o_muicf]->SetDim( 1,VIZmsize(mu.ICF));
    aObj[ o_muidcref]->SetPtr( mu.IDCref );
    aObj[ o_muidcref]->SetDim( mu.N, 1 );
    aObj[ o_muval]->SetPtr( mu.Val );
    aObj[ o_muval]->SetDim( mu.N, 1 );
    aObj[ o_mupl]->SetPtr( mu.Pl );
    aObj[ o_mupl]->SetDim( mu.Ls, 1 );
    aObj[ o_mubc]->SetPtr( mu.BC );
    aObj[ o_mubc]->SetDim( mu.N, 1 );
    aObj[ o_muas]->SetPtr( mu.AS );
    aObj[ o_muas]->SetDim( mu.N, 1 );
    aObj[ o_muav]->SetPtr( mu.AV );
    aObj[ o_muav]->SetDim( mu.N, 1 );
}

// set dynamic Objects ptr to values
void TRMults::dyn_set(int /*q*/)
{
    mu.SF  = (char (*)[PH_RKLEN])aObj[ o_musf ]->GetPtr();
    mu.SM  = (char (*)[DC_RKLEN])aObj[ o_musm ]->GetPtr();
    mu.SF2  = (char (*)[PH_RKLEN])aObj[ o_musf2 ]->GetPtr();
    mu.SM2  = (char (*)[DC_RKLEN])aObj[ o_musm2 ]->GetPtr();
mu.SM3  = (char (*)[DC_RKLEN])aObj[ o_musm3 ]->GetPtr();
// Added  Sveta 03/06/05
mu.nlICv  = (char (*)[MAXICNAME+MAXSYMB])aObj[ o_nlicv ]->GetPtr();
mu.nlDCv  = (char (*)[MAXDCNAME])aObj[ o_nldcv ]->GetPtr();
mu.nlPHv  = (char (*)[MAXPHNAME])aObj[ o_nlphv ]->GetPtr();
aObj[ o_nlich]->SetPtr(  mu.nlICv );
aObj[ o_nlich]->SetDim( 1, mu.N );
aObj[ o_nldch]->SetPtr(  mu.nlDCv );
aObj[ o_nldch]->SetDim( 1, mu.L );
aObj[ o_nldcvs]->SetPtr(  mu.nlDCv );
aObj[ o_nldcvs]->SetDim( mu.Ls, 1 );
aObj[ o_nldchs]->SetPtr(  mu.nlDCv );
aObj[ o_nldchs]->SetDim( 1, mu.Ls );
aObj[ o_nlphh]->SetPtr(  mu.nlPHv );
aObj[ o_nlphh]->SetDim( 1, mu.Fi );
// end of add

    mu.SA  = (char (*)[BC_RKLEN])aObj[ o_musa ]->GetPtr();
    mu.SB  = (char (*)[IC_RKLEN])aObj[ o_musb ]->GetPtr();
    mu.FN  = (char (*)[MAX_FILENAME_LEN])aObj[ o_mufn ]->GetPtr();
    mu.PHC = (char *)aObj[ o_muphc ]->GetPtr();
    mu.DCC = (char *)aObj[ o_mudcc ]->GetPtr();
mu.DCC3 = (char *)aObj[ o_mudcc3 ]->GetPtr();
    mu.DCS = (char *)aObj[ o_mudcs ]->GetPtr();
    mu.Pl  = (short *)aObj[ o_mupl ]->GetPtr();
    mu.ICC = (char *)aObj[ o_muicc ]->GetPtr();
    mu.DCF = (char *)aObj[ o_mudcf ]->GetPtr();
    mu.ICF = (char *)aObj[ o_muicf ]->GetPtr();
    mu.IDCref = (char (*)[DC_RKLEN])aObj[ o_muidcref ]->GetPtr();
    mu.Ll  = (short *)aObj[ o_mul1 ]->GetPtr();
    mu.Val = (short *)aObj[ o_muval ]->GetPtr();
    mu.BC  = (float *)aObj[ o_mubc ]->GetPtr();
    mu.AS  = (float *)aObj[ o_muas ]->GetPtr();
    mu.AV  = (float *)aObj[ o_muav ]->GetPtr();
}

// free dynamic memory in objects and values
void TRMults::dyn_kill(int /*q*/)
{
    mu.SF  = (char (*)[PH_RKLEN])aObj[ o_musf ]->Free();
    mu.SM  = (char (*)[DC_RKLEN])aObj[ o_musm ]->Free();
    mu.SF2  = (char (*)[PH_RKLEN])aObj[ o_musf2 ]->Free();
    mu.SM2  = (char (*)[DC_RKLEN])aObj[ o_musm2 ]->Free();
mu.SM3  = (char (*)[DC_RKLEN])aObj[ o_musm3 ]->Free();
// Added  Sveta 03/06/05
mu.nlICv  = (char (*)[MAXICNAME+MAXSYMB])aObj[ o_nlicv ]->Free();
mu.nlDCv  = (char (*)[MAXDCNAME])aObj[ o_nldcv ]->Free();
mu.nlPHv  = (char (*)[MAXPHNAME])aObj[ o_nlphv ]->Free();
aObj[ o_nlich]->SetPtr(  0 );
aObj[ o_nlich]->SetDim( 1, 0 );
aObj[ o_nldch]->SetPtr( 0 );
aObj[ o_nldch]->SetDim( 1, 0 );
aObj[ o_nldcvs]->SetPtr(  0 );
aObj[ o_nldcvs]->SetDim( 0, 1 );
aObj[ o_nldchs]->SetPtr(  0 );
aObj[ o_nldchs]->SetDim( 1, 0 );
aObj[ o_nlphh]->SetPtr(  0 );
aObj[ o_nlphh]->SetDim( 1, 0 );
// end of add
    mu.SA  = (char (*)[BC_RKLEN])aObj[ o_musa ]->Free();
    mu.SB  = (char (*)[IC_RKLEN])aObj[ o_musb ]->Free();
    mu.FN  = (char (*)[MAX_FILENAME_LEN])aObj[ o_mufn ]->Free();
    mu.PHC = (char *)aObj[ o_muphc ]->Free();
    mu.DCC = (char *)aObj[ o_mudcc ]->Free();
mu.DCC3 = (char *)aObj[ o_mudcc3 ]->Free();
    mu.DCS = (char *)aObj[ o_mudcs ]->Free();
    mu.Pl  = (short *)aObj[ o_mupl ]->Free();
    mu.ICC = (char *)aObj[ o_muicc ]->Free();
    mu.DCF = (char *)aObj[ o_mudcf ]->Free();
    mu.ICF = (char *)aObj[ o_muicf ]->Free();
    mu.IDCref = (char (*)[DC_RKLEN])aObj[ o_muidcref ]->Free();
    mu.Ll  = (short *)aObj[ o_mul1 ]->Free();
    mu.Val = (short *)aObj[ o_muval ]->Free();
    mu.BC  = (float *)aObj[ o_mubc ]->Free();
    mu.AS  = (float *)aObj[ o_muas ]->Free();
    mu.AV  = (float *)aObj[ o_muav ]->Free();
}

// realloc dynamic memory
void TRMults::dyn_new(int /*q*/)
{
    mu.FN = (char (*)[MAX_FILENAME_LEN])aObj[ o_mufn]->Alloc( mu.NfT, 1, MAX_FILENAME_LEN );
    if( mu.PmvIC == S_ON )
    {
        mu.SB = (char (*)[IC_RKLEN])aObj[ o_musb]->Alloc( mu.N, 1, IC_RKLEN );
        mu.ICC = (char *)aObj[ o_muicc]->Alloc( mu.N, 1, A_ );
        mu.Val = (short *)aObj[ o_muval]->Alloc( mu.N, 1, I_ );
        mu.BC = (float *)aObj[ o_mubc]->Alloc( mu.N, 1, F_ );
mu.nlICv  = (char (*)[MAXICNAME+MAXSYMB])aObj[ o_nlicv ]->Alloc(
                         mu.N, 1, MAXICNAME+MAXSYMB );
aObj[ o_nlich]->SetPtr(  mu.nlICv );
aObj[ o_nlich]->SetDim( 1, mu.N );
    }
    else  Error( GetName(), "PmvIC != S_ON" );

    if( mu.PmvIP == S_ON )
    {
        mu.AS = (float *)aObj[ o_muas]->Alloc( mu.N, 1, F_ );
        mu.AV = (float *)aObj[ o_muav]->Alloc( mu.N, 1, F_ );
    }
    else
    {
        mu.AS  = (float *)aObj[ o_muas ]->Free();
        mu.AV  = (float *)aObj[ o_muav ]->Free();
    }

    if( mu.PmvID == S_ON )
        mu.IDCref = (char (*)[DC_RKLEN])aObj[o_muidcref]->Alloc(mu.N, 1, DC_RKLEN );
    else  mu.IDCref = (char (*)[DC_RKLEN])aObj[ o_muidcref ]->Free();

    if( mu.PmvIF == S_ON )
        mu.ICF = (char *)aObj[ o_muicf]->Alloc( 1, mu.N*10, S_ );
    else mu.ICF = (char *)aObj[ o_muicf ]->Free();

    if( mu.PmvSA == S_ON )
        mu.SA = (char (*)[BC_RKLEN])aObj[ o_musa]->Alloc( mu.La, 1, BC_RKLEN );
    else mu.SA  = (char (*)[BC_RKLEN])aObj[ o_musa ]->Free();

    if( mu.PmvPH == S_ON )
    {
        mu.SF = (char (*)[PH_RKLEN])aObj[ o_musf]->Alloc( mu.Fi, 1, PH_RKLEN );
        mu.SF2 = (char (*)[PH_RKLEN])aObj[ o_musf2]->Alloc( mu.Fis, 1, PH_RKLEN );
        mu.Ll = (short *)aObj[ o_mul1]->Alloc( mu.Fi, 1, I_ );
        mu.PHC = (char *)aObj[ o_muphc]->Alloc( mu.Fi, 1, A_ );
mu.nlPHv  = (char (*)[MAXPHNAME])aObj[ o_nlphv ]->Alloc( mu.Fi, 1, MAXPHNAME );
aObj[ o_nlphh]->SetPtr(  mu.nlPHv );
aObj[ o_nlphh]->SetDim( 1, mu.Fi );
    }
    else Error( GetName(), "PmvPH != S_ON" );

    if( mu.PmvDC == S_ON )
    {
        mu.SM = (char (*)[DC_RKLEN])aObj[ o_musm]->Alloc( mu.L, 1, DC_RKLEN );
        mu.SM2 = (char (*)[DC_RKLEN])aObj[ o_musm2]->Alloc( mu.Ls, 1, DC_RKLEN );
        mu.DCC = (char *)aObj[ o_mudcc]->Alloc( mu.L, 1, A_ );
        mu.DCS = (char *)aObj[ o_mudcs]->Alloc( mu.L, 1, A_ );
        if( mu.Ls )
            mu.Pl = (short *)aObj[ o_mupl]->Alloc( mu.Ls, 1, I_ );
        else  mu.Pl  = (short *)aObj[ o_mupl ]->Free();
if(mu.Lads) // added 28.10.2004 by KD for implementation of CD MUSIC
{
   mu.SM3 = (char (*)[DC_RKLEN])aObj[ o_musm3]->Alloc( mu.Lads, 1, DC_RKLEN );
   mu.DCC3 = (char *)aObj[ o_mudcc3]->Alloc( mu.Lads, 1, A_ );
}
else {
   mu.SM3  = (char (*)[DC_RKLEN])aObj[ o_musm3 ]->Free();
   mu.DCC3 = (char *)aObj[ o_mudcc3 ]->Free();
}
mu.nlDCv  = (char (*)[MAXDCNAME])aObj[ o_nldcv ]->Alloc( mu.L, 1, MAXDCNAME);
aObj[ o_nldch]->SetPtr(  mu.nlDCv );
aObj[ o_nldch]->SetDim( 1, mu.L );
aObj[ o_nldcvs]->SetPtr(  mu.nlDCv );
aObj[ o_nldcvs]->SetDim( mu.Ls, 1 );
aObj[ o_nldchs]->SetPtr(  mu.nlDCv );
aObj[ o_nldchs]->SetDim( 1, mu.Ls );
    }
    else
    {
        mu.SM  = (char (*)[DC_RKLEN])aObj[ o_musm ]->Free();
        mu.SM2  = (char (*)[DC_RKLEN])aObj[ o_musm2 ]->Free();
        mu.DCC = (char *)aObj[ o_mudcc ]->Free();
        mu.DCS = (char *)aObj[ o_mudcs ]->Free();
        mu.Pl  = (short *)aObj[ o_mupl ]->Free();
mu.nlDCv  = (char (*)[MAXDCNAME])aObj[ o_nldcv ]->Free();
aObj[ o_nldch]->SetPtr(  0 );
aObj[ o_nldch]->SetDim( 1, 0 );
aObj[ o_nldcvs]->SetPtr(  0 );
aObj[ o_nldcvs]->SetDim( 0, 1 );
aObj[ o_nldchs]->SetPtr(  0 );
aObj[ o_nldchs]->SetDim( 1, 0 );
    }
    if( mu.PmvDF == S_ON )
    {
        short DCFlen = mu.FiE;
        mu.FiE = 0;
        mu.DCF = (char *)aObj[ o_mudcf]->Alloc( 1, DCFlen*MAXFORMULA, S_ );
    }
    else  mu.DCF = (char *)aObj[ o_mudcf ]->Free();
}


//set default information
void TRMults::set_def( int /*q*/)
{
    TProfil *aPa= dynamic_cast<TProfil *>(aMod[RT_PARAM].get());
    memcpy( &mu.PmvSA, aPa->pa.MUpmv, 10 );
//    strcpy( mu.name, "`");
//    strcpy( mu.notes, "`");
    fillValue( mu.name, '\0', MAXFORMULA);
    fillValue( mu.notes, '\0', MAXFORMULA);
    strncpy( mu.name, rt[RT_PARAM]->FldKey(0), rt[RT_PARAM]->FldLen(0) );
    strncpy( mu.notes, rt[RT_PARAM]->FldKey(1), rt[RT_PARAM]->FldLen(1) );
    memset( &mu.N, 0, 14*sizeof(short));
    mu.SF  = 0;
    mu.SM  = 0;
    mu.SF2  = 0;
    mu.SM2  = 0;
mu.SM3  = 0;
    mu.SA  = 0;
    mu.SB  = 0;
    mu.FN  = 0;
    mu.PHC = 0;
    mu.DCC = 0;
mu.DCC3 = 0;
mu.nlICv  = 0;
mu.nlDCv  = 0;
mu.nlPHv  = 0;
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

void TRMults::DCListLoad(  TCStringArray AqKey, TCStringArray GasKey,
                           bool useLst, TCStringArray lst )
{
    uint k, jj;
    int j, kk;
    time_t tim;
    int SPHP=1;  /* at first test phases-solutions */

    // Get all records of PHase
    TCStringArray aPhaseList;
    TCIntArray anRPhase;
    if( useLst == false )
    {
        rt[RT_PHASE]->GetKeyList( "*:*:*:*:*:", aPhaseList, anRPhase );
        uint ii=0;
        while( ii < aPhaseList.size() )
        {
            if( *aPhaseList[ii].c_str() == 'a' )
            {

                for(jj=0; jj<AqKey.size(); jj++)
                {
                    if( aPhaseList[ii] ==  AqKey[jj])
                     break;
                }
                if( jj>=AqKey.size() )
                {
                  aPhaseList.erase(aPhaseList.begin() +ii);
                  anRPhase.erase( anRPhase.begin()+ii);
                }
               else ii++;
            }
            else
                if( *aPhaseList[ii].c_str() == 'g' || *aPhaseList[ii].c_str() == 'f' )
                {

                    for(jj=0; jj<GasKey.size(); jj++)
                    {
                        if(aPhaseList[ii] ==  GasKey[jj])
                         break;
                    }
                    if( jj>=GasKey.size() )
                    {
                      aPhaseList.erase(aPhaseList.begin() +ii);
                      anRPhase.erase( anRPhase.begin()+ii);
                    }
                   else ii++;
                }
                else
                { //Sveta 21/09/99 Compare first 4 fields delete last
                    // overite mehanism from Dima
                    if( ii > 0)
                    {
                        if( !strncmp(aPhaseList[ii-1].c_str(),
                                     aPhaseList[ii].c_str(), PH_RKLEN-MAXPHGROUP  ))
                        {
                            aPhaseList.erase(aPhaseList.begin() +ii);
                            anRPhase.erase( anRPhase.begin()+ii);
                        }
                        else ii++;
                    }
                    else ii++;
                }
        }
    }
    else
    {
        for( size_t ii=0; ii<lst.size(); ii++)
            aPhaseList.push_back(lst[ii]);
    }

    if( aPhaseList.size()<1 )
        Error("RMULT", "No records of Phases!");
    mu.Fi  = aPhaseList.size();
    mu.PmvPH = S_ON;
    mu.Ll = (short *)aObj[ o_mul1]->Alloc( mu.Fi, 1, I_ );
    mu.SF = (char (*)[PH_RKLEN])aObj[ o_musf]->Alloc( mu.Fi, 1, PH_RKLEN );
    // List of DCOMP&REACDC component in Project
    TCStringArray List;
    TPhase* aPH= dynamic_cast<TPhase *>(aMod[RT_PHASE].get());
    aPH->ods_link(0);
    // Build list of DCOMP&REACDC from PHASE
    kk=-1;
    mu.Ls = mu.Lads = mu.Fis = 0;
TEST2:
    for( k=0; k<aPhaseList.size(); k++)
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
            auto ss = string(aPH->php->SM[j], 0, DC_RKLEN);
            List.push_back(ss);
            mu.Ll[kk]++;
        } /* j */
        if( mu.Ll[kk] > 1 ) // multicomponent phase
        {
            mu.Ls += mu.Ll[kk];
            mu.Fis++;
            if( aPH->php->NsuT > 0 && aPH->php->NsuT < 5 )
               mu.Lads += mu.Ll[kk];
        }
    } /* k */
    if( SPHP==1)
    {
        SPHP=0;
        goto TEST2;
    }
    mu.L = List.size();
    mu.FiE= mu.L;
    mu.PmvDC = S_ON;
    mu.SM = (char (*)[DC_RKLEN])aObj[ o_musm]->Alloc( mu.L, 1, DC_RKLEN );
    for(size_t i=0; i< List.size(); i++)
    {
        memcpy( mu.SM[i], List[i].c_str(), DC_RKLEN );
    }
}

//Make record of base Rmults from files
void TRMults::MakeRecordLists( TCStringArray AqKey, TCStringArray GasKey )
{
    uint i;
    // Get all records of IComp
    TCStringArray aICList;
    TCIntArray anRIC;
    rt[RT_ICOMP]->GetKeyList( "*:*:*:", aICList, anRIC );
    if( aICList.size()<1 )
        Error("RMULT", "No records of Independent component!");
    mu.N  = aICList.size();
    mu.PmvIC = S_ON;
    mu.SB = (char (*)[IC_RKLEN])aObj[ o_musb]->Alloc( mu.N, 1, IC_RKLEN );
    for( i=0; i< aICList.size(); i++)
        memcpy( mu.SB[i], aICList[i].c_str(), IC_RKLEN );

    // Get all records of Compos
    TCStringArray aCompList;
    TCIntArray anRComp;
    rt[RT_COMPOS]->GetKeyList( "*:*:*:", aCompList, anRComp );
    mu.La = aCompList.size();
    if( mu.La<1 )
        mu.PmvSA = S_OFF;
    else
    {
        mu.PmvSA = S_ON;
        mu.SA = (char (*)[BC_RKLEN])aObj[ o_musa]->Alloc( mu.La, 1, BC_RKLEN );
        for( i=0; i< aCompList.size(); i++)
            memcpy( mu.SA[i], aCompList[i].c_str(), BC_RKLEN );
    }
    // Get all records of PHase  and Get DCOMP&REACT list
    DCListLoad( AqKey, GasKey );
}

// Make list of ICOMP and set data to work arrays
void TRMults::ICmake()
{
    int ii;
    size_t j=0;
    time_t crt;
    TIComp* aIC=dynamic_cast<TIComp *>(aMod[RT_ICOMP].get());
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
        mu.ICF = (char *)aObj[ o_muicf]->Alloc( mu.N, j+strlen(mu.ICF)+2, S_ );
}

// Make list of PHASE and set data to work arrays
void TRMults::PHmake()
{
    int  jj, ii,  kk, forlen, sflast=-1;
    short ij, i;
    char buf[MAXFORMULA];
    time_t crt;
    char *Formula=0, *item=buf, dkey[MAXRKEYLEN];
    TPhase* aPH= dynamic_cast<TPhase *>( aMod[RT_PHASE].get());
    aPH->ods_link(0);
    TDComp* aDC= dynamic_cast<TDComp *>( aMod[RT_DCOMP].get());
    aDC->ods_link(0);
    TReacDC* aRC= dynamic_cast<TReacDC *>( aMod[RT_REACDC].get());
    aRC->ods_link(0);

    mu.FiE = 0;
    mu.Pg = 0;
    mu.Lhc = 0;
    mu.Laq =  0;
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
        // test and load � DCOMP and REACDC
        fillValue( dkey, '\0', MAXRKEYLEN );

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
        case PH_FLUID:
            mu.Pg += mu.Ll[kk];  // Corrected on 13.06.03 by KD
            continue;
        case PH_HCARBL:
            mu.Lhc += mu.Ll[kk]; // Corrected on 13.06.03 by KD
            continue;
        case PH_SORPTION:
            mu.FiE += mu.Ll[kk];
            continue;
        default:
            continue;
        }
    } /* kk */
    mu.DCF = (char *)aObj[ o_mudcf]->Alloc( 1, sflast+2, S_ );
}

//realloc memory and load data to structure RMULTS
void TRMults::LoadRmults( bool NewRec, bool changePhases )
{
    int file;
    //string AqKey("a:*:*:*:*:");
    TCStringArray AqKey;
    //string PrKey("g:*:*:*:*:");
    TCStringArray GasKey;
    //string FluKey;
    char amod = SM_AQDAV;    // Added KD 25.01.02
    char gmod = SM_IDEAL;    // Added KD 16.06.03
    float aparam[8]/*, gparam[4]*/;

    TProfil *aPa=dynamic_cast<TProfil *>( aMod[RT_PARAM].get());
    MTPARM* tpp = TMTparm::sm->GetTP();
    if( tpp->Pbg < '0' || tpp->Pbg > '4' )
        tpp->Pbg = '0';
    //int resp = aPa->tpp->Pbg-'0';
    // Copying stored params for automatic aq models
    aparam[0] = aPa->pa.aqPar[0];
    aparam[1] = aPa->pa.aqPar[1];
    aparam[2] = aPa->pa.aqPar[2];
    aparam[3] = aPa->pa.aqPar[3];
    aparam[4] = aPa->pa.aqPar[4];
    aparam[5] = 0.;
    //gparam[0] = 0.; //  = aPa->pa.ResFloat;
    //gparam[1]=gparam[2]=gparam[3]=0.0;

    if( NewRec == false )
    { // Assemble aqueous and gaseous phase records automatically
        amod = mu.PmvAq;
        if( mu.nAq >= 0)
        {
            //AqKey.Add( char_array_to_string( mu.SF[mu.nAq], PH_RKLEN ));
            if( amod == SM_AQDH3 && aparam[0] < 1e-9 )
            {  // To use aq models from old versions
                aparam[0] = 0.064f;
                aparam[3] = 0.7f;
            }
        }
        //else {
        //     AqKey.Clear();// == "";
        //}
        gmod = mu.PmvGas;
        /*if( mu.nGas >= 0 )
        {
            GasKey.Add( char_array_to_string( mu.SF[mu.nGas], PH_RKLEN ) );
        }
        else {
             GasKey.Clear();// ="";
        }*/
        for(short kk=0; kk<mu.Fi; kk++ ) // phase list
        {
            //aqueous phase
            if(  mu.SF[kk][0] =='a' )
                AqKey.push_back( string(mu.SF[kk], 0, PH_RKLEN));
            //gaseous phase
            else if(  mu.SF[kk][0] =='g' || mu.SF[kk][0] =='f' )
                GasKey.push_back( string(mu.SF[kk], 0, PH_RKLEN));
            else break;
        }

    }
    string prfName = TProfil::pm->projectName();

    if( changePhases || mu.PmvAq == S_ON || mu.PmvGas == S_ON )
    {
        string  AqKey1 = "a:*:*:*:*:";
        if(AqKey.size()>0)
            AqKey1 = AqKey[0];

        string  GasKey1 = "g:*:*:*:*:";
        if(GasKey.size()>0)
            GasKey1 = GasKey[0];

NEW_PHASE_AGAIN:

        // Calling the wizard to set generated aq and gas phases
        if( !vfAutoPhaseSet( pVisor->window()/*window()*/, prfName.c_str(),
                             AqKey1, GasKey1, amod, gmod, aparam ) )
        {
            if( vfQuestion( window(), "Project: Attempt to cancel setup of phases",
                            "Are you really sure?\n Repeat phase setup (Yes) or\nCancel creating the project (No)?" ))
                goto NEW_PHASE_AGAIN;   // cancel - infinite loop?
            else
                Error( GetName(), "Project creation aborted by the user - bailing out..." );
        }

        tpp->Pbg = (int)aparam[4]+'0'; // resp+'0';  to check in TSolMod implementation


        // define Aq phase lists
        if( amod == '-' )
        {
            mu.PmvAq = S_OFF;
            AqKey.clear();
        }
        else { // Setting control parameters for the auto aq model
            mu.PmvAq = amod;
            aPa->pa.aqPar[0] = aparam[0];
            aPa->pa.aqPar[1] = aparam[1];
            aPa->pa.aqPar[2] = aparam[2];
            aPa->pa.aqPar[3] = aparam[3];
            aPa->pa.aqPar[4] = aparam[4];

            if( mu.PmvAq == 'U' )
            {   SelectAqGasPhase( 0,  AqKey ); // Select aqueous phase from list
                if( mu.PmvAq == S_ON ) // No phase to select from list or not selected - make default
                    goto NEW_PHASE_AGAIN;
            }
            else
            {
                AqKey.clear();
                AqKey.push_back(AqKey1);
            }
        }

        // define gas/fluid phase
        if( gmod == '-' )
        {
            mu.PmvGas = S_OFF;
            GasKey.clear();
        }
        else {
            mu.PmvGas = gmod;

            if( mu.PmvGas == 'U')
            {  SelectAqGasPhase( 1, GasKey ); // Select gaseous phase def
                if( mu.PmvGas == S_ON )    // No phase to select from list or not selected - make default
                    goto NEW_PHASE_AGAIN;
            }
            else
            {
                GasKey.clear();
                GasKey.push_back(GasKey1);
            }
        }

    }

    file =rt[RT_PHASE]->GetOpenFileNum( prfName.c_str() );

    // Creating automatic aq and/or gas phases (automatic phase only one )
    TPhase::pm->newAqGasPhase( ( AqKey.size()>0 ? AqKey[0]: ""),
                               ( GasKey.size()>0 ? GasKey[0]: ""),
                               file, amod, gmod, aparam );

    MakeRecordLists( AqKey, GasKey ); // build records lists and calc size of arrays

    dyn_new();         // alloc memory to structure RMULTS
    ICmake();          // load data from ICOMP
    PHmake();          // load data from PHASE and DCOMP&REACDC

    // insert data to short key lists
    memcpy( mu.SF2, mu.SF, mu.Fis*PH_RKLEN*sizeof(char));
    if( mu.Ls && mu.SM2 )  // Fix of a potential bug
        memcpy( mu.SM2, mu.SM, mu.Ls*DC_RKLEN*sizeof(char));
    if( mu.Lads && mu.SM3 && mu.DCC3 )
    {
        memcpy( mu.SM3, mu.SM+mu.Ls-mu.Lads, mu.Lads*DC_RKLEN*sizeof(char));
        memcpy( mu.DCC3, mu.DCC+mu.Ls-mu.Lads, mu.Lads*sizeof(char));
    }
    // Added Sveta 03/06/05
    if( mu.nlICv )
        for( int ii=0; ii<mu.N; ii++ )
            memcpy( mu.nlICv[ii], mu.SB[ii], MAXICNAME+MAXSYMB );
    if( mu.nlDCv )
        for( int ii=0; ii<mu.L; ii++ )
            memcpy( mu.nlDCv[ii], mu.SM[ii]+MAXSYMB+MAXDRGROUP, MAXDCNAME );
    if( mu.nlPHv )
        for( int ii=0; ii<mu.Fi; ii++ )
            memcpy( mu.nlPHv[ii], mu.SF[ii]+MAXSYMB+MAXPHSYMB, MAXPHNAME );

    // test data base ICOMP  before calc
    TestIComp();

    // Set nAq and nGas
    SetAqGas( ( AqKey.size()>0 ? AqKey[0].c_str(): nullptr),
              ( GasKey.size()>0 ? GasKey[0].c_str(): nullptr) );

}


// Select aqueous (type = 0) or gas/fluid ( type = 1 ) phase from available or make default
// AqGasKey - input old selection, return new selection
void TRMults::SelectAqGasPhase( char AqGasType, TCStringArray& AqGasKey )
{
    uint ii, jj;
    std::string typePhase, msg1, msg2;
    TCStringArray aPhaseType;
    TCStringArray aKeysList;
    TCStringArray aPhaseList;
    TCIntArray anRPhase;

    if( AqGasType == 0 )
     {
        typePhase = " aqueous phase";
        aPhaseType.push_back("a:*:*:*:*:");
     }
    else
     {
        typePhase = " gas/fluid phase";
        aPhaseType.push_back("g:*:*:*:*:");
        aPhaseType.push_back("f:*:*:*:*:");
     }

     // Get all records of Phase for type
    for(ii=0; ii<aPhaseType.size(); ii++ )
    {
        rt[RT_PHASE]->GetKeyList( aPhaseType[ii].c_str(), aPhaseList, anRPhase );
        for( jj=0; jj<aPhaseList.size(); jj++ )
            aKeysList.push_back( aPhaseList[jj]);
    }

    if( aKeysList.size()<1 ) //no type phase in open data base files
    {
        msg1 = "Project: Choice of";
        msg1 += typePhase;
        msg2 = "No definitions of";
        msg2 += typePhase;
        msg2 += " available!\n Make default(Yes) or remove (No)?";

        if( vfQuestion( window(), msg1,msg2))
        {
           if(AqGasType )
            mu.PmvAq = S_ON;
           else mu.PmvGas = S_ON;
           return;
        }
        else {
               if(AqGasType )
                 mu.PmvAq = S_OFF;
               else mu.PmvGas = S_OFF;
               return;
        }
     }

    if( aKeysList.size()==1 )
    {
        AqGasKey.clear();
        AqGasKey.push_back( aKeysList[0] );
    }
    else
    {
       anRPhase.clear();
       // get selected phase  added Sveta 18/06/04
       for(ii=0; ii <aKeysList.size(); ii++)
       {
         for( jj=0; jj<AqGasKey.size(); jj++ )
            if( AqGasKey[jj] == aKeysList[ii] )
              anRPhase.push_back(ii);
       }
       msg1 = "Please, choose an";
       msg1+= typePhase +".";

AGAIN:  anRPhase = vfMultiChoiceSet(window(), aKeysList, msg1.c_str(), anRPhase );

    if( anRPhase.size() < 1 )
    {
      msg1 = "Project: What";
      msg1 += typePhase+"?";
      msg2 = "No definitions of";
      msg2 += typePhase+" selected!\n Choose again (Yes) or remove (No)?";

      if( vfQuestion( window(), msg1,msg2 ))
         goto AGAIN;
      else
      {
         if(AqGasType )
            mu.PmvAq = S_OFF;
          else mu.PmvGas = S_OFF;
          mu.PmvAq = S_OFF;
      }
    }
    else
    {
      AqGasKey.clear();
      for(ii=0; ii <anRPhase.size(); ii++)
          AqGasKey.push_back( aKeysList[anRPhase[ii]] );
    }
  }
 }

/*
// Select aqueous phase from available or make default
string TRMults::SelectAqPhase(const char * dfKey )
{
    int i, sel=0;
    TCStringArray aPhaseList;
    TCIntArray anRPhase;

     // Get all records of Phase
    rt[RT_PHASE]->GetKeyList( "a:*:*:*:*:", aPhaseList, anRPhase );
    if( aPhaseList.GetCount()<1 ) //no aqueous phase in open data base files
    {
        if( vfQuestion( window(), "Project: Choice of aqueous phase?",
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
    // get selected phase  added Sveta 18/06/04
    for(uint ii=0; ii <aPhaseList.GetCount(); ii++)
    {
        if( dfKey == aPhaseList[ii] )
        {
            sel =ii;
            break;
        }
    }
    // select phase
//    do
//    {
AGAIN:  i = vfChoice(window(), aPhaseList,
                   "Please, choose an aqueous phase.", sel);
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
}

// Select gaseous phase from all or make default
string TRMults::SelectGasPhase(const char * dfKey )
{
    int i, sel=0;
    TCStringArray aPhaseList;
    TCIntArray anRPhase;

    // Get all records of PHase
    rt[RT_PHASE]->GetKeyList( "g:*:*:*:*:", aPhaseList, anRPhase );
    if( aPhaseList.GetCount()<1 ) //no gas phase in open data base files
    { // Sveta 17/05/99  mu.PmvGas = S_ON;  goto DEF_KEY;
       rt[RT_PHASE]->GetKeyList( "f:*:*:*:*:", aPhaseList, anRPhase );
       if( aPhaseList.GetCount()<1 )
       {
          if( vfQuestion( window(), "Project: Choice of gas/fluid phase?",
            "No definitions of gas/fluid phase available!\n Make default (Yes) or remove (No)?" ))
          {
            mu.PmvGas = S_ON; return "";
          }
          else {
            mu.PmvGas = S_OFF; return "";
          }
       }
    }
    if( aPhaseList.GetCount() == 1 )
        return  aPhaseList[0];
    // select phase
    rt[RT_PHASE]->GetKeyList( "*:*:*:*:*:", aPhaseList, anRPhase );
    // get selected phase  added Sveta 18/06/04
    for(uint ii=0; ii <aPhaseList.GetCount(); ii++)
    {
        if( dfKey == aPhaseList[ii] )
        {
            sel =ii;
            break;
        }
    }
    do
    {
      i = vfChoice(window(), aPhaseList,
         "Please, choose a gas (g) or fluid (f) phase", sel);
    }
    while( i<0 );
    return  aPhaseList[i];
}

*/

//Set indexes of selected aqueous and gaseous phase
void TRMults::SetAqGas( const char* AqKey, const char* GasKey )
{
    mu.nAq = mu.nGas = -1;
    for(short kk=0, ii=0; kk<mu.Fi; kk++ ) // phase list
    {
        //aqueous phase
        if( AqKey && !memcmp( mu.SF[kk], AqKey, PH_RKLEN ) )
        {
            mu.nAq=kk;
            ii++;
        }
        //gaseous phase
        if( GasKey && !memcmp( mu.SF[kk], GasKey, PH_RKLEN ) )
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
    string form;

    // test formules React&DComp
    for( ii=0; ii<mu.L; ii++ )
    {
        form = aFo.form_extr( ii, mu.L, mu.DCF );
        aFo.SetFormula( form.c_str() ); // and ce_fscan
        aFo.TestIC( mu.SM[ii], mu.N, (char *)mu.SB);
    }
    // test compos
    TCompos* aCMP=dynamic_cast<TCompos *>( aMod[RT_COMPOS].get());
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
                string msg= "IComp: ";
                msg += string(aCMP->bcp->SB[i], 0,MAXICNAME+MAXSYMB );
                msg += "\n in Compos record: \n";
                msg += string(mu.SA[j], 0, BC_RKLEN );

                Error( "Invalid IComp", msg );
            }
        } /* i */
    } /*j*/

}
//--------------------- End of ms_rmults.cpp ---------------------------

