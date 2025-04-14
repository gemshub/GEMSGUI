//-------------------------------------------------------------------
// $Id: m_syseq.cpp 1409 2009-08-21 15:34:43Z gems $
//
// Implementation of TSysEq class, config and  packed functions
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

TSysEq* TSysEq::pm;

TSysEq::TSysEq( uint nrt ):
        TCModule( nrt )
{
    nQ = 2;
    aFldKeysHelp.push_back("Name of the modeling project");
    aFldKeysHelp.push_back("Thermodynamic potential to minimize {G GV}");
    aFldKeysHelp.push_back("Name of the chemical system definition (CSD)");
    aFldKeysHelp.push_back("CSD (recipe) variant number <integer>");
    aFldKeysHelp.push_back("Volume of the system, dm3 (0 if no volume constraint)");
    aFldKeysHelp.push_back("Pressure, bar, or 0 for Psat(H2O)g");
    aFldKeysHelp.push_back("Temperature, C (>= 0)");
    aFldKeysHelp.push_back("Variant number for additional constraints");
    setKeyEditField(1);

    stp=&st[1];
    ssp=&ss[1];
    set_def(1);
    stp=&st[0];
    ssp=&ss[0];
    set_def();
    start_title = " Packed definition of chemical system / equilibrium state ";
}

bool TSysEq::ifCalcFlag()
{

    return stp->Flags&0x1;
}

void TSysEq::setCalcFlag( bool ifC )
{
    if(ifC == true)
        stp->Flags =(stp->Flags|0x1);
    else
        stp->Flags =(stp->Flags&(0xF-0x1));
}


// Input nessasery data and links objects
void TSysEq::RecInput( const char *key )
{
    keyTest( key );
    TCModule::RecInput( key );
}

//Save record structure
void
TSysEq::RecSave( const char *key, bool onOld )
{
    if( pVisor->ProfileMode  )
    { // test system and pack
        TProfil *aPa= dynamic_cast<TProfil *>( aMod[RT_PARAM].get());
        aPa->PackSystat();
    }
    TCModule::RecSave( key, onOld );
}


// test Project key to calc mode
void TSysEq::keyTest( const char *key )
{
    if( pVisor->ProfileMode )
    { // test project key
        std::string prfKey = TProfil::pm->projectName();
        auto k = prfKey.length();
        if( memcmp(key, prfKey.c_str(), k ) ||
                ( key[k] != ':' && key[k] != ' ' && k<rt[RT_PARAM]->FldLen(0) )  )
            Error( GetName(), "Invalid key!");
    }
}

/* opens window with 'Remake record' parameters
*/
void TSysEq::MakeQuery()
{
    const char * p_key;
    std::string name = std::string(ssp->name, 0, MAXFORMULA);
    std::string notes = std::string(ssp->notes, 0, MAXFORMULA);
    std::string EQkey = std::string(ssp->PhmKey, 0, EQ_RKLEN);

    p_key  = db->PackKey();


    if( !vfSystemSet( window(), p_key, ssp->switches, name,notes,EQkey ))
         Error( p_key, "Project record configuration cancelled by the user!" );

    memcpy(ssp->name, name.c_str(), MAXFORMULA);
    memcpy(ssp->notes, notes.c_str(), MAXFORMULA);
    memcpy(ssp->PhmKey, EQkey.c_str(), EQ_RKLEN);

}


//Rebild record structure before calc
int
TSysEq::RecBuild( const char *key, int mode  )
{
    if( !pVisor->ProfileMode )
        Error( GetName(), "Invalid mode!");


    if( !ssp->DM[0] || !ssp->DM[1] )
    {
        dyn_kill();
        set_def();
    }
    int ret = TCModule::RecBuild( key, mode  );
    // System
    if( ret == VF_CANCEL && ssp->DM[0] && ssp->DM[1] )
        return ret;
    if( ssp->switches[13] != S_OFF )
    {
//        rt[RT_SYSEQ].MakeKey( RT_PARAM, pkey, RT_PARAM, 0,
//            K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_END);

        std::string skey = std::string(ssp->PhmKey, 0, EQ_RKLEN);
        if( /* !skey.empty() && */ skey[0] != '-' && skey != S_EMPTY )
        {
           if( skey.empty() || skey[0] == '\0' || skey[0] == ' ' )
              skey = "*";
           else
              rt[RT_SYSEQ]->SetKey( skey.c_str() ); //Find( skey.c_str() );

          skey = vfKeyEdit( window(),
               "Please, select one SysEq record",  nRT,  skey.c_str() );
          if( skey.empty() )
               memcpy( ssp->PhmKey, S_EMPTY, EQ_RKLEN );
          else
               memcpy( ssp->PhmKey, skey.c_str(), EQ_RKLEN );

          rt[RT_SYSEQ]->SetKey( key ); // DAK fixed 27.10.99
       }
    }
    // Check flags to alloc data

    if( ssp->switches[24] != S_OFF && ssp->switches[25] != S_OFF )
    {
        ssp->switches[21] = S_REM;
        ssp->switches[20] = S_REM;
    }
    /* Check for surface types (sorption) */
    if( ssp->switches[28] != S_OFF && ssp->switches[24] != S_OFF )
    { /* ssp->switches[28] = S_REM; */ ssp->DM[17] = MNST;
        ssp->switches[29] = S_REM;
        ssp->switches[30] = S_REM;
        /* ssp->switches[]PCSTS = S_REM; */
        ssp->switches[19] = S_REM;
        ssp->switches[22] = S_REM;
        ssp->switches[33] = S_REM;
        if( ssp->switches[9] != S_OFF )
        {
            ssp->switches[31]= S_REM;
            ssp->switches[32]= S_REM;
        }
    }
    else
    {  /* No surface area or surface types were specified! */
        ssp->switches[28] = S_OFF;
        ssp->switches[19] = S_OFF;
        ssp->switches[22] = S_OFF;
        ssp->switches[31] = S_OFF;
        ssp->switches[32] = S_OFF; /* ssp->switches[]PCSTS  = S_OFF; */
        ssp->switches[29] = S_OFF;
        ssp->switches[30] = S_OFF;
        ssp->DM[17] = 0;
    }
    //TProfil *aPa= dynamic_cast<TProfil *>( aMod[RT_PARAM].get());
    // for phase separation (exsolution)
    ssp->switches[21] = S_REM;
    ssp->switches[20] = S_REM;
    ssp->switches[23] = S_REM;

    //Eqstat
    setCalcFlag( false );

    //Realloc memory
    dyn_new();

    pVisor->Update();
    return ret;
}

//Recalc record structure
void
TSysEq::RecCalc( const char *key )
{
    if( !pVisor->ProfileMode )
        Error( GetName(), "Invalid mode!");
    TCModule::RecCalc(key);
}


// insert changes in Project to TSysEq

void TSysEq::InsertChanges( std::vector<CompItem>& aIComp,
                            std::vector<CompItem>& aCompos, std::vector<CompItem>& aPhase,
                            std::vector<CompItem>&aDComp )
{
    size_t i;
    int j1, j2, j3, j4, j5, j6;
    short  delta=0;
    bool ifRealloc = false;
    // insert changes to IComp
    if(aIComp.size()<1)
        goto COMPOS_TEST;

    for( i=0, j1=0, j2=0, j3=0; i<aIComp.size(); i++)
    {
        if( ssp->nnc )
        {
            while( (ssp->nnc[j1] < aIComp[i].line) && (j1<ssp->DM[14]) )
                // IC     on  in  BI[]      [sy.Na]
                ssp->nnc[j1++] += delta;
            if( (ssp->nnc[j1] == aIComp[i].line) && (j1<ssp->DM[14]) )
            {
                if( aIComp[i].delta == -1 )
                {
                    ssp->nnc[j1++] = -1;
                    ifRealloc = true;
                }
            }
        }
        if( ssp->nnf )
        {
            while( ( ssp->nnf[j2] < aIComp[i].line ) && (j2<ssp->DM[0]) )
                // IC    ON   in B[], system [sy.N]
                ssp->nnf[j2++] += delta;
            if( (ssp->nnf[j2] == aIComp[i].line) && (j2<ssp->DM[0]) )
            {
                if( aIComp[i].delta == -1 )
                {
                    ssp->nnf[j2++] = -1;
                    ifRealloc = true;
                }
            }
        }
        if( stp->nnf )
        {
            while( ( stp->nnf[j3] < aIComp[i].line) && (j3<stp->N ) ) // RMULTS-indices of IC [N] EQUSTAT
                stp->nnf[j3++] += delta;
            if( (stp->nnf[j3] == aIComp[i].line) && (j3<stp->N) )
            {
                if( aIComp[i].delta == -1 )
                {
                    stp->nnf[j3++] = -1;
                    ifRealloc = true;
                }
            }
        }
        delta+=aIComp[i].delta;
    }
    if( ssp->nnc )
        for(; j1<ssp->DM[14]; j1++)
            ssp->nnc[j1] += delta;
    if( ssp->nnf )
        for(; j2<ssp->DM[0]; j2++)
            ssp->nnf[j2] += delta;
    if( stp->nnf )
        for(; j3<stp->N; j3++)
            stp->nnf[j3] += delta;

    // insert changes to Compos
COMPOS_TEST:
    if(aCompos.size()<1)
        goto PHASE_TEST;
    for( i=0, j1=0, delta=0; i<aCompos.size(); i++)
    {
        if( ssp->llc )
        {
            while( (ssp->llc[j1] < aCompos[i].line) && (j1<ssp->DM[11]) )
                // COMPOS on  in  XeA[] [sy.La]
                ssp->llc[j1++] += delta;
            if( (ssp->llc[j1] == aCompos[i].line) && (j1<ssp->DM[11]) )
            {
                if( aCompos[i].delta == -1 )
                {
                    ssp->llc[j1++] = -1;
                    ifRealloc = true;
                }
            }
        }
        delta+=aCompos[i].delta;
    }
    if( ssp->llc )
        for(; j1<ssp->DM[11]; j1++)
            ssp->llc[j1] += delta;

    // insert changes to Phase
PHASE_TEST:
    delta = 0;
    if(aPhase.size()<1)
        goto DCOMP_TEST;

    for( i=0, j1=0, j2=0, j3=0, j4=0, j5=0, j6=0; i<aPhase.size(); i++)
    {
        if( ssp->phc )
        {
            while( (ssp->phc[j1] < aPhase[i].line) && (j1<ssp->DM[9]) )
                // PHASE  on  in  Phm[] [sy.Fib]
                ssp->phc[j1++] += delta;
            if( (ssp->phc[j1] == aPhase[i].line) && (j1<ssp->DM[9]) )
            {
                if( aPhase[i].delta == -1 )
                {
                    ssp->phc[j1++] = -1;
                    ifRealloc = true;
                }
            }
        }
        if( ssp->phf )
        {
            while( (ssp->phf[j2] < aPhase[i].line) && (j2<ssp->DM[22]) )
                //PHASE OFF in system [mu.FI-sy.Fi]
                ssp->phf[j2++] += delta;
            if( (ssp->phf[j2] == aPhase[i].line) && (j2<ssp->DM[22]) )
            {
                if( aPhase[i].delta == -1 )
                {
                    ssp->phf[j2++] = -1;
                    ifRealloc = true;
                }
            }
        }
        if( ssp->pha )  // PHASE ON   adsorption EDL data [sy.Fia]
        {
            while( (ssp->pha[j3] < aPhase[i].line) && (j3<ssp->DM[20]) )
                ssp->pha[j3++] += delta;
            if( ( ssp->pha[j3] == aPhase[i].line) && (j3<ssp->DM[20]) )
            {
                if( aPhase[i].delta == -1 )
                {
                    ssp->pha[j3++] = -1;
                    ifRealloc = true;
                }
            }
        }
        if( ssp->phk )  // PHASE ON  in PUL[]/PLL[]     [sy.Fik]
        {
            while( (ssp->phk[j4] < aPhase[i].line) && (j4<ssp->DM[10]) )
                ssp->phk[j4++] += delta;
            if( (ssp->phk[j4] == aPhase[i].line) && (j4<ssp->DM[10]) )
            {
                if( aPhase[i].delta == -1 )
                {
                    ssp->phk[j4++] = -1;
                    ifRealloc = true;
                }
            }
        }
        if( ssp->phe )  // PHASE ON  in YOF[], Aalp[]   [sy.Fie]
        {
            while( (ssp->phe[j5] < aPhase[i].line) && (j5<ssp->DM[19]) )
                ssp->phe[j5++] += delta;
            if( (ssp->phe[j5] == aPhase[i].line) && (j5<ssp->DM[19]) )
            {
                if( aPhase[i].delta == -1 )
                {
                    ssp->phe[j5++] = -1;
                    ifRealloc = true;
                }
            }
        }
       if( stp->phf )  // PROJECT indices of multicomp. phases in eqstate [Fis]
        {
            while( (stp->phf[j6] < aPhase[i].line) && (j6<stp->Fis) )
                stp->phf[j6++] += delta;
            if( (stp->phf[j6] == aPhase[i].line) && (j6<stp->Fis) )
            {
                if( aPhase[i].delta == -1 )
                {
                    stp->phf[j6++] = -1;
                    ifRealloc = true;
                }
            }
        }
        delta+=aPhase[i].delta;
    }
    if( ssp->phc )
        for(; j1<ssp->DM[9]; j1++)
            ssp->phc[j1] += delta;
    if( ssp->phf )
        for(; j2<ssp->DM[22]; j2++)
            ssp->phf[j2] += delta;
    if( ssp->pha )
        for(; j3<ssp->DM[20]; j3++)
            ssp->pha[j3] += delta;
    if( ssp->phk )
        for(; j4<ssp->DM[10]; j4++)
            ssp->phk[j4] += delta;
    if( ssp->phe )
        for(; j5<ssp->DM[19]; j5++)
            ssp->phe[j5] += delta;

    if( stp->phf )
        for(; j6<stp->Fis; j6++)
            stp->phf[j6] += delta;

    // insert changes to Dcomp
DCOMP_TEST:

    delta = 0;
    if(aDComp.size()<1)
        goto EXIT_TEST;


    for( i=0, j1=0, j2=0, j3=0, j4=0, j5=0, j6=0; i<aDComp.size(); i++)
    {
        if( ssp->dcc ) // DC     on  in  XeD[]     [sy.Lb]
        {
            while( (ssp->dcc[j1] < aDComp[i].line) && (j1<ssp->DM[12]) )
                ssp->dcc[j1++] += delta;
            if( (ssp->dcc[j1] == aDComp[i].line) && (j1<ssp->DM[12]) )
            {
                if( aDComp[i].delta == -1 )
                {
                    ssp->dcc[j1++] = -1;
                    ifRealloc = true;
                }
            }
        }
        if( ssp->dcf ) // DC    OFF  in system  [mu.L-sy.L]
        {
            while( (ssp->dcf[j2] < aDComp[i].line) && (j2<ssp->DM[23]) )
                ssp->dcf[j2++] += delta;
            if( (ssp->dcf[j2] == aDComp[i].line) && (j2<ssp->DM[23]) )
            {
                if( aDComp[i].delta == -1 )
                {
                    ssp->dcf[j2++] = -1;
                    ifRealloc = true;
                }
            }
        }
        if( ssp->dca ) // DC    ON   adsorption SAT data [sy.Lsor]
        {
            while( (ssp->dca[j3] < aDComp[i].line) && (j3<ssp->DM[4]) )
                ssp->dca[j3++] += delta;
            if( (ssp->dca[j3] == aDComp[i].line) && (j3<ssp->DM[4]) )
            {
                if( aDComp[i].delta == -1 )
                {
                    ssp->dca[j3++] = -1;
                    ifRealloc = true;
                }
            }
        }
        if( ssp->dck ) // DC    ON  in DUL[]/DLL[]     [sy.Lk]
        {
            while( (ssp->dck[j4] < aDComp[i].line) && (j4<ssp->DM[13]) )
                ssp->dck[j4++] += delta;
            if( (ssp->dck[j4] == aDComp[i].line) && (j4<ssp->DM[13]) )
            {
                if( aDComp[i].delta == -1 )
                {
                    ssp->dck[j4++] = -1;
                    ifRealloc = true;
                }
            }
        }
        if( ssp->dce ) // DC    ON  in GEX[], lnGmf[]  [sy.Le]
        {
            while( (ssp->dce[j5] < aDComp[i].line) && (j5<ssp->DM[18]) )
                ssp->dce[j5++] += delta;
            if( (ssp->dce[j5] == aDComp[i].line) && (j5<ssp->DM[18]) )
            {
                if( aDComp[i].delta == -1 )
                {
                    ssp->dce[j5++] = -1;
                    ifRealloc = true;
                }
            }
        }
        if( stp->llf ) // RMULTS-indices of DC [L]
        {
            while( (stp->llf[j6] < aDComp[i].line) && (j6<stp->L) )
                stp->llf[j6++] += delta;
            if( (stp->llf[j6] == aDComp[i].line) && (j6<stp->L) )
            {
                if( aDComp[i].delta == -1 )
                {
                    stp->llf[j6++] = -1;
                    ifRealloc = true;
                }
            }
        }
        delta+=aDComp[i].delta;
    }
    if( ssp->dcc )
        for(; j1<ssp->DM[12]; j1++)
            ssp->dcc[j1] += delta;
    if( ssp->dcf )
        for(; j2<ssp->DM[23]; j2++)
            ssp->dcf[j2] += delta;
    if( ssp->dca )
        for(; j3<ssp->DM[4]; j3++)
            ssp->dca[j3] += delta;
    if( ssp->dck )
        for(; j4<ssp->DM[13]; j4++)
            ssp->dck[j4] += delta;
    if( ssp->dce )
        for(; j5<ssp->DM[18]; j5++)
            ssp->dce[j5] += delta;
    if( stp->llf )
        for(; j6<stp->L; j6++)
            stp->llf[j6] += delta;

    // realloc memory if delete records
EXIT_TEST:
    if( ifRealloc )
    {
        newSizeifChange();
        dyn_new();
    }
    setCalcFlag( false );

}

// #define sm(i,j) ssp->MaSdj[(j)+(i)*D_F_CD_NP]
// added by KD on 25.10.2004

// pack arrays if deleted records in base cfg
void TSysEq::newSizeifChange()
{
    short i, j, k;

    // delete ICOMP records
    for( i=0, j=0; i<ssp->DM[14]; i++ )      // System ncc
        if( ssp->nnc[i] >= 0 ) // no delete num
        {
            ssp->nnc[j] = ssp->nnc[i];
            if( ssp->BIun ) ssp->BIun[j] = ssp->BIun[i];
            if( ssp->BI )   ssp->BI[j] = ssp->BI[i];
            j++;
        }
    ssp->DM[14] = j;

    for( i=0, j=0; i<ssp->DM[0]; i++ )      // System nnf
        if( ssp->nnf[i] >= 0 ) // no delete num
        {
            ssp->nnf[j] = ssp->nnf[i];
            if( ssp->B )    ssp->B[j] = ssp->B[i];
            if( ssp->delB ) ssp->delB[j] = ssp->delB[i];
            j++;
        }
    ssp->DM[0] = j;

    for( i=0, j=0; i<stp->N; i++ )      // Equstat nnf
        if( stp->nnf[i] >= 0 ) // no delete num
        {
            stp->nnf[j] = stp->nnf[i];
            if( stp->B ) stp->B[j] = stp->B[i];
            if( stp->U ) stp->U[j] = stp->U[i];
            if( stp->DIC )
            {
                stp->DIC[j][0] = stp->DIC[i][0];
                stp->DIC[j][1] = stp->DIC[i][1];
            }
            if( stp->Ba )
                for( k=0; k<stp->Fis; k++ )
                    stp->Ba[k*stp->N+j] = stp->Ba[k*stp->N+i];
            j++;
        }
    stp->N = j;


    // delete COMPOS records
    for( i=0, j=0; i<ssp->DM[11]; i++ )      // System llc
        if( ssp->llc[i] >= 0 ) // no delete num
        {
            ssp->llc[j] = ssp->llc[i];
            if( ssp->Acl ) ssp->Acl[j] = ssp->Acl[i];
            if( ssp->XAun ) ssp->XAun[j] = ssp->XAun[i];
            if( ssp->XeA )   ssp->XeA[j] = ssp->XeA[i];
            j++;
        }
    ssp->DM[11] = j;

    // delete PHASE records
    for( i=0, j=0; i<ssp->DM[9]; i++ )      // System phc
        if( ssp->phc[i] >= 0 ) // no delete num
        {
            ssp->phc[j] = ssp->phc[i];
            if( ssp->XPun ) ssp->XPun[j] = ssp->XPun[i];
            if( ssp->Phm ) ssp->Phm[j] = ssp->Phm[i];
            j++;
        }
    ssp->DM[9] = j;

    for( i=0, j=0; i<ssp->DM[22]; i++ )      // System phf
        if( ssp->phf[i] >= 0 ) // no delete num
        {
            ssp->phf[j] = ssp->phf[i];
            j++;
        }
    ssp->DM[22] = j;

    for( i=0, j=0; i<ssp->DM[20]; i++ )      // System pha
        if( ssp->pha[i] >= 0 ) // no delete num
        {
            ssp->pha[j] = ssp->pha[i];
            for( k=0; k<ssp->DM[17]; k++ )
            {
                if( ssp->SCMT ) ssp->SCMT[j][k] = ssp->SCMT[i][k];
                if( ssp->Nfsp ) ssp->Nfsp[j][k] = ssp->Nfsp[i][k];
                if( ssp->MaSdt ) ssp->MaSdt[j][k] = ssp->MaSdt[i][k];
                if( ssp->XcapA ) ssp->XcapA[j][k] = ssp->XcapA[i][k];
                if( ssp->XcapB ) ssp->XcapB[j][k] = ssp->XcapB[i][k];
                if( ssp->XcapF ) ssp->XcapF[j][k] = ssp->XcapF[i][k];
                if( ssp->XfIEC ) ssp->XfIEC[j][k] = ssp->XfIEC[i][k];
                if( ssp->Xlam ) ssp->Xlam[j][k] = ssp->Xlam[i][k];
            }
            j++;
        }
    ssp->DM[20] = j;

    for( i=0, j=0; i<ssp->DM[10]; i++ )      // System phk
        if( ssp->phk[i] >= 0 ) // no delete num
        {
            ssp->phk[j] = ssp->phk[i];
            if( ssp->PUL ) ssp->PUL[j] = ssp->PUL[i];
            if( ssp->PLL ) ssp->PLL[j] = ssp->PLL[i];
            if( ssp->RFLC ) ssp->RFLC[j] = ssp->RFLC[i];
            if( ssp->RFSC ) ssp->RFSC[j] = ssp->RFSC[i];
            j++;
        }
    ssp->DM[10] = j;

    for( i=0, j=0; i<ssp->DM[19]; i++ )      // System phe
        if( ssp->phe[i] >= 0 ) // no delete num
        {
            ssp->phe[j] = ssp->phe[i];
            if( ssp->Aalp ) ssp->Aalp[j] = ssp->Aalp[i];
            if( ssp->YOF ) ssp->YOF[j] = ssp->YOF[i];
            if( ssp->Sigm )
            {
                ssp->Sigm[j][0] = ssp->Sigm[i][0];
                ssp->Sigm[j][1] = ssp->Sigm[i][1];
            }
            if( ssp->Xr0h0 )
            {
                ssp->Xr0h0[j][0] = ssp->Xr0h0[i][0];
                ssp->Xr0h0[j][1] = ssp->Xr0h0[i][1];
            }
            if( ssp->XEpsC )
            {
                ssp->XEpsC[j][0] = ssp->XEpsC[i][0];
                ssp->XEpsC[j][1] = ssp->XEpsC[i][1];
            }
            j++;
        }
    ssp->DM[19] = j;

    for( i=0, j=0; i<stp->Fis; i++ )      // Equstat phf
        if( stp->phf[i] >= 0 ) // no delete num
        {
            stp->phf[j] = stp->phf[i];
            if( stp->Ba )
                for( k=0; k<stp->N; k++ )
                    stp->Ba[j*stp->N+k] = stp->Ba[i*stp->N+k];
            if( stp->VM )
            {
                stp->VM[j][0] = stp->VM[i][0];
                stp->VM[j][1] = stp->VM[i][1];
                stp->VM[j][2] = stp->VM[i][2];
                stp->VM[j][3] = stp->VM[i][3];
                stp->VM[j][4] = stp->VM[i][4];
            }
            j++;
        }
    stp->Fis = j;

    // delete DCOMP records
    for( i=0, j=0; i<ssp->DM[12]; i++ )      // System dcc
        if( ssp->dcc[i] >= 0 ) // no delete num
        {
            ssp->dcc[j] = ssp->dcc[i];
            if( ssp->XDun ) ssp->XDun[j] = ssp->XDun[i];
            if( ssp->XeD ) ssp->XeD[j] = ssp->XeD[i];
            j++;
        }
    ssp->DM[12] = j;

    for( i=0, j=0; i<ssp->DM[23]; i++ )      // System dcf
        if( ssp->dcf[i] >= 0 ) // no delete num
        {
            ssp->dcf[j] = ssp->dcf[i];
            j++;
        }
    ssp->DM[23] = j;


    for( i=0, j=0; i<ssp->DM[4]; i++ )      // System dca
        if( ssp->dca[i] >= 0 ) // no delete num
        {
            ssp->dca[j] = ssp->dca[i];
            if( ssp->MaSdj )
            {
// Extended by KD on 25.10.2004
              int kk;
              for(kk=0; kk<DFCN; kk++)
                  ssp->MaSdj[j][kk]  = ssp->MaSdj[i][kk];
//               ssp->MaSdj[j] = ssp->MaSdj[i];
            }
            if( ssp->SATC )
            {
// Extended by KD on 27.10.2004
              int kk;
              for(kk=0; kk<MCAS; kk++)
                  ssp->SATC[j][kk]  = ssp->SATC[i][kk];
//              ssp->SATC[j][0] = ssp->SATC[i][0];
//              ssp->SATC[j][1] = ssp->SATC[i][1];
            }
            j++;
        }
    ssp->DM[4] = j;

    for( i=0, j=0; i<ssp->DM[13]; i++ )      // System dck
        if( ssp->dck[i] >= 0 ) // no delete num
        {
            ssp->dck[j] = ssp->dck[i];
            if( ssp->DUL ) ssp->DUL[j] = ssp->DUL[i];
            if( ssp->DLL ) ssp->DLL[j] = ssp->DLL[i];
            if( ssp->RLC ) ssp->RLC[j] = ssp->RLC[i];
            if( ssp->RSC ) ssp->RSC[j] = ssp->RSC[i];
            j++;
        }
    ssp->DM[13] = j;

    for( i=0, j=0; i<ssp->DM[18]; i++ )      // System dce
        if( ssp->dce[i] >= 0 ) // no delete num
        {
            ssp->dce[j] = ssp->dce[i];
            if( ssp->GEX ) ssp->GEX[j] = ssp->GEX[i];
            if( ssp->lnGmf ) ssp->lnGmf[j] = ssp->lnGmf[i];
            if( ssp->Pparc ) ssp->Pparc[j] = ssp->Pparc[i];
            j++;
        }
    ssp->DM[18] = j;

    for( i=0, j=0; i<stp->L; i++ )      // Equstat llf
        if( stp->llf[i] >= 0 ) // no delete num
        {
            stp->llf[j] = stp->llf[i];
            if( stp->Y ) stp->Y[j] = stp->Y[i];
            if( stp->lnGam ) stp->lnGam[j] = stp->lnGam[i];
            j++;
        }
    stp->L = j;
}


//Rename record (Change first field: for SYSEQ, and >)
// and change key in tPhEQ
//Sveta 14/11/2002
void TSysEq::RenameList( const char* newName,
        const char *oldName )
{
    if( strlen(newName) > db->FldLen(0) )
      return;

    std::string str_old = char_array_to_string( oldName, db->FldLen(0) );
//04/09/01 ????    if( strlen(oldName)<FldLen(0) )
        str_old += ":";
    for( int i=1; i<db->KeyNumFlds(); i++)
            str_old += "*:";

    TCStringArray arKey;
    TCIntArray arR;

    auto Nrec = db->GetKeyList( str_old.c_str(), arKey, arR );
    if( Nrec < 1)
      return;

    int nrec;
    std::string str;

    for(uint i=0; i<Nrec; i++ )
    {
        nrec =  db->Find( arKey[i].c_str() );
        db->Get( nrec );
        dyn_set();
        // changing record key
        str = db->PackKey();
        db->Del( nrec );
        replace( str, oldName, newName);
        // change tPhEQ key (o_ssphst,PhmKey)
        std::string tPhkey = char_array_to_string( ssp->PhmKey, EQ_RKLEN );
        if( tPhkey.find(oldName) != std::string::npos)
        {
         db->SetKey( tPhkey.c_str() );
         tPhkey = db->PackKey();
         replace( tPhkey, oldName, newName );
         memcpy( ssp->PhmKey, tPhkey.c_str(), EQ_RKLEN );
        }
        db->AddRecordToFile( str.c_str(), db->fNum );
    }
}

// Help on module  ( ? button )
const char* TSysEq::GetHtml()
{
   return GM_SYSEQ_HTML;
}


//--------------------- End of m_syseq.cpp ---------------------------
