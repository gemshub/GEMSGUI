//-------------------------------------------------------------------
// Id: gems/mods/m_prfget2.cpp  version 2.0.0   2001
//
// Implementation of TProfile class, elements list manipulation functions
//
// Rewritten from C to C++ by S.Dmytriyeva  970207 modified 010904
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
#include "m_param.h"
#include "m_probe.h"
#include "m_syseq.h"
#include "service.h"


// save old lists of keys to compare
void TProfil::SaveOldList()
{
    Nold = mup->N;
    Lold = mup->L;
    Fiold = mup->Fi;
    Fisold = mup->Fis;
    Laold = mup->La;
    Lsold = mup->Ls;
    SBold = (char (*)[IC_RKLEN]) new char[mup->N*IC_RKLEN];
    memcpy( SBold, mup->SB, mup->N*IC_RKLEN*sizeof(char));
    SAold = (char (*)[BC_RKLEN]) new char[mup->La*BC_RKLEN];
    memcpy( SAold, mup->SA, mup->La*BC_RKLEN*sizeof(char));
    Llold =   new short[mup->Fi];
    memcpy( Llold, mup->Ll, mup->Fi*sizeof(short));
    SFold = (char (*)[PH_RKLEN]) new char[mup->Fi*PH_RKLEN];
    memcpy( SFold, mup->SF, mup->Fi*sizeof(char)*PH_RKLEN );
    SMold = (char (*)[DC_RKLEN]) new char[mup->L*DC_RKLEN];
    memcpy( SMold, mup->SM, mup->L*sizeof(char)*DC_RKLEN );
}

// delete old lists of keys to compare
void TProfil::DeleteOldList()
{
    if( SBold )  delete[] SBold;
    SBold= 0;
    if( SAold )  delete[] SAold;
    SAold = 0;
    if(  Llold ) delete[]  Llold;
    Llold = 0;
    if( SFold ) delete[] SFold;
    SFold = 0;
    if( SMold ) delete[] SMold;
    SMold = 0;
}

// push element to list
void TProfil::Push( TIArray<CompItem>& aList, int aLine,
                    short aDelta, const char* dbKeywd, gstring aKey )
{
    gstring stt = aKey;
    if( aDelta < 0 )
        stt += " record deleted. Continue?";
    else stt += " record inserted. Continue?";
    if( !vfQuestion(window(), dbKeywd, stt.c_str()) )
        Error( dbKeywd, "Comparison error!" );
    aList.Add( new CompItem( aLine, aDelta));
}


// Compare IComp keys lists
void TProfil::ICcompare( TIArray<CompItem>& aIComp)
{
    int i, j, l;
    i = 0;
    j = 0;
    while( i<Nold && j<mup->N )
    {
        l = memcmp( SBold[i], mup->SB[j], IC_RKLEN );
        if( l==0 )
        {
            i++;
            j++;
        }
        else
            if( l<0 )
            {
                Push( aIComp, i, -1, "IComp", gstring(SBold[i], 0, IC_RKLEN) );
                i++;
            }
            else
            {
                Push( aIComp, i, 1, "IComp", gstring(mup->SB[j], 0, IC_RKLEN) );
                j++;
            }
    }
    while( i<Nold  )
    {
        Push( aIComp, i, -1, "IComp", gstring(SBold[i], 0, IC_RKLEN) );
        i++;
    }
    while( j<mup->N )
    {
        Push( aIComp, i, 1, "IComp", gstring(mup->SB[j], 0, IC_RKLEN) );
        j++;
    }
}

// Compare Compos keys lists
void TProfil::COMPcompare( TIArray<CompItem>& aCompos)
{
    int i, j, l;
    i = 0;
    j = 0;
    while( i<Laold && j<mup->La )
    {
        l = memcmp( SAold[i], mup->SA[j], BC_RKLEN );
        if( l==0 )
        {
            i++;
            j++;
        }
        else
            if( l<0 )
            {
                Push( aCompos, i, -1, "Compos", gstring(SAold[i], 0, BC_RKLEN) );
                i++;
            }
            else
            {
                Push( aCompos, i, 1, "Compos", gstring(mup->SA[j], 0, BC_RKLEN) );
                j++;
            }
    }
    while( i<Laold  )
    {
        Push( aCompos, i, -1, "Compos", gstring(SAold[i], 0, BC_RKLEN) );
        i++;
    }
    while( j<mup->La )
    {
        Push( aCompos, i, 1, "Compos", gstring(mup->SA[j], 0, BC_RKLEN) );
        j++;
    }
}

// compare DCOMP&REACT keys lists to one phase
void TProfil::DCcompare( TIArray<CompItem>& aList, int& i,
                         int& j, int nI, int nJ)
{
    int l;
    while( i<nI && j<nJ )
    {
        l = memcmp( SMold[i], mup->SM[j], DC_RKLEN-MAXSYMB );
        if( l==0 )
        {
            i++;
            j++;
        }
        else
            if( l<0 )
            {
                Push( aList, i, -1, "DComp/ReacDC", gstring(SMold[i], 0, DC_RKLEN) );
                i++;
            }
            else
            {
                Push( aList, i, 1, "DComp/ReacDC", gstring(mup->SM[j], 0, DC_RKLEN) );
                j++;
            }
    }
    while( i<nI  )
    {
        Push( aList, i, -1, "DComp/ReacDC", gstring(SMold[i], 0, DC_RKLEN) );
        i++;
    }
    while( j<nJ )
    {
        Push( aList, i, 1, "DComp/ReacDC", gstring(mup->SM[j], 0, DC_RKLEN) );
        j++;
    }
}

// Compare Phase and DComp&React keys lists
void TProfil::PHcompare( TIArray<CompItem>& aPhase,
                         TIArray<CompItem>& aDComp)
{
    int i, j, l;
    int id =0, jd=0;

    i = 0;
    j = 0;
    while( i<Fisold && j<mup->Fis )
    {
        l = memcmp( SFold[i], mup->SF[j], PH_RKLEN );
        if( l==0 )
        {
            DCcompare( aDComp, id, jd, id+Llold[i], jd+mup->Ll[j]);
            i++;
            j++;
        }
        else
            if( l<0 )
            {
                Push( aPhase, i, -1, "Phase", gstring(SFold[i], 0, PH_RKLEN) );
                for( int ii=id; ii<id+Llold[i]; ii++)
                    Push( aDComp, ii, -1, "DComp/ReacDC", gstring(SMold[ii], 0, DC_RKLEN) );
                id += Llold[i];
                i++;
            }
            else
            {
                Push( aPhase, i, 1, "Phase", gstring(mup->SF[j], 0, PH_RKLEN) );
                for( int jj=jd; jj<jd+mup->Ll[j]; jj++)
                    Push( aDComp, id, 1, "DComp/ReacDC", gstring(mup->SM[jj], 0, DC_RKLEN) );
                jd += mup->Ll[j];
                j++;
            }
    }
    while( i<Fisold  )
    {
        Push( aPhase, i, -1, "Phase", gstring(SFold[i], 0, PH_RKLEN) );
        for( int ii=id; ii<id+Llold[i]; ii++)
            Push( aDComp, ii, -1, "DComp/ReacDC", gstring(SMold[ii], 0, DC_RKLEN) );
        id += Llold[i];
        i++;
    }
    while( j<mup->Fis )
    {
        Push( aPhase, i, 1, "Phase", gstring(mup->SF[j], 0, PH_RKLEN) );
        for( int jj=jd; jj<jd+mup->Ll[j]; jj++)
            Push( aDComp, id, 1, "DComp/ReacDC", gstring(mup->SM[jj], 0, DC_RKLEN) );
        jd += mup->Ll[j];
        j++;
    }

    // compare one-component phase
    i = Fisold;
    j = mup->Fis;
    id =Lsold, jd=mup->Ls;
    while( i<Fiold && j<mup->Fi )
    {
        l = memcmp( SFold[i], mup->SF[j], PH_RKLEN );
        if( l==0 )
        {
            DCcompare( aDComp, id, jd, id+Llold[i], jd+mup->Ll[j] );
            i++;
            j++;
        }
        else
            if( l<0 )
            {
                Push( aPhase, i, -1, "Phase", gstring(SFold[i], 0, PH_RKLEN) );
                for( int ii=id; ii<id+Llold[i]; ii++)
                    Push( aDComp, ii, -1, "DComp/ReacDC", gstring(SMold[ii], 0, DC_RKLEN) );
                id += Llold[i];
                i++;
            }
            else
            {
                Push( aPhase, i, 1, "Phase", gstring(mup->SF[j], 0, PH_RKLEN) );
                for( int jj=jd; jj<jd+mup->Ll[j]; jj++)
                    Push( aDComp, id, 1, "DComp/ReacDC", gstring(mup->SM[jj], 0, DC_RKLEN) );
                jd += mup->Ll[j];
                j++;
            }
    }
    while( i<Fiold  )
    {
        Push( aPhase, i, -1, "Phase", gstring(SFold[i], 0, PH_RKLEN) );
        for( int ii=id; ii<id+Llold[i]; ii++)
            Push( aDComp, ii, -1, "DComp/ReacDC", gstring(SMold[ii], 0, DC_RKLEN) );
        id += Llold[i];
        i++;
    }
    while( j<mup->Fi )
    {
        Push( aPhase, i, 1, "Phase", gstring(mup->SF[j], 0, PH_RKLEN) );
        for( int jj=jd; jj<jd+mup->Ll[j]; jj++)
            Push( aDComp, id, 1, "DComp/ReacDC", gstring(mup->SM[jj], 0, DC_RKLEN) );
        jd += mup->Ll[j];
        j++;
    }
}

// test and insert changes to data base file
void TProfil::TestChangeProfile()
{
    vstr pkey(81);

    // compare ICOMP list
    TIArray<CompItem> aIComp(10, 5);  // list of IC changes
    ICcompare( aIComp);
    // compare COMPOS list
    TIArray<CompItem> aCompos(10, 5);  // list of Compos changes
    COMPcompare( aCompos );
    // compare PHASE list
    TIArray<CompItem> aPhase(10, 5);  // list of Phase changes
    TIArray<CompItem> aDComp(10, 5);  // list of DComp changes
    PHcompare( aPhase, aDComp );

    // Insert changes to SYSEQ
    if( aIComp.GetCount()<1 && aCompos.GetCount()<1 &&
            aPhase.GetCount()<1 && aDComp.GetCount()<1 )
        return;
    TCStringArray aList;
    TCIntArray anR;
    rt[RT_SYSEQ].MakeKey( RT_PARAM, pkey, RT_PARAM, 0,
                           K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_END);
    rt[RT_SYSEQ].GetKeyList( pkey, aList, anR );

    TSysEq* aSE=(TSysEq *)(&aMod[RT_SYSEQ]);
    aSE->ods_link(0);
    for(uint i=0; i< aList.GetCount(); i++)
    {
        //    int nRt = rt[RT_SYSEQ].Find( aList[i].c_str() );
        aSE->RecInput( aList[i].c_str() );
        aSE->InsertChanges( aIComp, aCompos, aPhase, aDComp );
        syst->loadData( false );
        syst->SyTestSizes(); //  Sveta 16/11/99
        aSE->RecSave( aList[i].c_str(), true );
        //    rt[RT_SYSEQ].Rep( nRt );
    }
    // Insert changes to PROBE

    if( aIComp.GetCount()<1 &&
            aPhase.GetCount()<1 && aDComp.GetCount()<1 )
        return;
    aList.Clear();
    anR.Clear();
    rt[RT_PROBE].MakeKey( RT_PARAM, pkey, RT_PARAM, 0, K_ANY,
                           K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_END);
    rt[RT_PROBE].GetKeyList( pkey, aList, anR );

    TProbe* aPB=(TProbe *)(&aMod[RT_PROBE]);
    aPB->ods_link(0);
    for(uint i=0; i< aList.GetCount(); i++)
    {
        aPB->RecInput( aList[i].c_str() );
        aPB->InsertChanges( aIComp, aPhase, aDComp );
        aPB->RecSave( aList[i].c_str(), true );
    }
}


// load and set default SysEq to build short lists
// delete only phase and dcomp/reacdc fron default phases

void TProfil::Set_z_sp_config( const char *profil )
{

    // test existing def record
    gstring key = profil;
    key += ":G:z_sp_config:0:0:1:25:0:";

    int    nRec = rt[RT_SYSEQ].Find( key.c_str() );
    if( nRec < 0 )
    {
        isSysEq = false;
        return;
    }
    loadSystat( key.c_str() );

    int file = rt[RT_PHASE].GetOpenFileNum( profil );

    // delete only full phases and
    // dcomps/reacts from default aques&gases phases

    int ii=0, nAq=-1, nAqf, nGas=-1, nGasf, jj=0;


    while( ii < mup->Fi)
    {
        if( *mup->SF[ii] == 'a' )
        {
            nAq  = ii;
            nAqf = jj;
        }
        if( *mup->SF[ii] == 'g' )
        {
            nGas  = ii;
            nGasf = jj;
        }
        jj+= mup->Ll[ii];
        ii++;
    }

    gstring AqKey = gstring( mup->SF[nAq], 0, PH_RKLEN);
    gstring GasKey = gstring( mup->SF[nGas], 0, PH_RKLEN);

    if( nAq >=0 && mup->PmvAq == S_ON )
    {
        TCStringArray lst;
        for( ii=nAqf; ii<nAqf+mup->Ll[nAq]; ii++  )
            if( syp->Dcl[ii] != S_OFF )
            {
                mup->SM[ii][DC_RKLEN-1] = mup->DCS[ii];
                vstr ss( DC_RKLEN, mup->SM[ii] );
                lst.Add( gstring(ss.p) );
                mup->SM[ii][DC_RKLEN-1] = ' ';
            }
        TPhase::pm->newAqGasPhase( AqKey.c_str(), file, true, lst);
    }

    if( nGas >=0 && mup->PmvGas == S_ON )
    {
        TCStringArray lst;
        for( ii=nGasf; ii<nGasf+mup->Ll[nGas]; ii++  )
            if( syp->Dcl[ii] != S_OFF )
            {
                mup->SM[ii][DC_RKLEN-1] = mup->DCS[ii];
                vstr ss( DC_RKLEN, mup->SM[ii] );
                lst.Add( gstring(ss.p) );
                mup->SM[ii][DC_RKLEN-1] = ' ';
            }
        TPhase::pm->newAqGasPhase( GasKey.c_str(), file, true, lst);
    }

    //save DClist old
    TCStringArray DCList;
    for(int ii=0; ii<mup->L; ii++)
    {
        vstr ss( DC_RKLEN, mup->SM[ii] );
        DCList.Add( gstring(ss.p) );

    }

    // using IComp and Compos Lists old

    // Make new Phase and dcomp/react lists
    PHon.Clear();
    PHoff.Clear();
    TCStringArray Phaselst;

    for( ii=0; ii<mup->Fi; ii++)
    {
        if( syp->Pcl[ii] == S_OFF)
            PHoff.Add(ii);
        else
        {
            PHon.Add(ii);
            vstr ss( PH_RKLEN, mup->SF[ii] );
            Phaselst.Add( gstring(ss.p) );
        }

    }

    rmults->DCListLoad( AqKey, GasKey, true, Phaselst);

    rmults->dyn_new();         // alloc memory to structure RMULTS
    rmults->ICmake();          // load data from ICOMP
    rmults->PHmake();          // load data from PHASE and DCOMP&REACDC

    // insert data to short key lists
    memcpy( mup->SF2, mup->SF, mup->Fis*PH_RKLEN*sizeof(char));
    memcpy( mup->SM2, mup->SM, mup->Ls*DC_RKLEN*sizeof(char));
    // test data base ICOMP  before calc
    //rmults->TestIComp();

    // Set nAq and nGas
    rmults->SetAqGas( AqKey.c_str() , GasKey.c_str() );

    DCon.Clear();
    DCoff.Clear();

    // Save changes to DComp list
    for( jj=0, ii=0; ii<DCList.GetCount(); ii++)
    {
        if( !strncmp( DCList[ii].c_str(), mup->SM[jj], DC_RKLEN) )
        {
            DCon.Add(ii);
            jj++;
        }
        else DCoff.Add(ii);
    }

    isSysEq = true;

    multi->dyn_kill();
    multi->set_def();

    // Load new MTPARAM & SYSTEM

    // Test MTparm
    mtparm->MTparmAlloc();
    mtparm->LoadMtparm( 25., 1. );
    // Begin alloc of SYSTEM
    syst->dyn_kill();
    syst->set_def();
    syst->dyn_new();
    syst->setDefData();

}


int TProfil::indPH( int i )
{
    if( isSysEq == false )
        return i;

    for( uint ii=0; ii<PHon.GetCount(); ii++)
        if( i == PHon[ii])
            return ii;

    return -1;
}

int TProfil::indDC( int i )
{
    if( isSysEq == false )
        return i;
    for( uint ii=0; ii<DCon.GetCount(); ii++)
        if( i == DCon[ii])
            return ii;
    return -1;
}

// Show phase information in System Configurator
// This has to be re-implemented using ListView dialog
void TProfil::ShowPhaseWindow()
{
    int k, nPHrec;
    TCStringArray aPHList;

    for( k=0; k<mup->Fi; k++ )
        aPHList.Add( gstring( mup->SF[k], 0, PH_RKLEN) );
    nPHrec = vfChoice(window(), aPHList, "c_phnam  Select Phase name to show record");
    if( nPHrec >= 0 )
    {
        TPhase::pm->RecInput( mup->SF[nPHrec] );
        TPhase::pm->Show(window(), "Phase demo mode", true);
    }
}

// Show species/phase equilibrium data
// This has to be re-implemented using ListView dialog
void TProfil::ShowEqPhaseWindow()
{
    int k, nPHrec;
    TCStringArray aPHList;

    for( k=0; k<mup->Fi; k++ )
        aPHList.Add( gstring( mup->SF[k], 0, PH_RKLEN) );
    nPHrec = vfChoice(window(), aPHList, "c_phnam  Select Phase name to show record");
    if( nPHrec >= 0 )
    {
        TPhase::pm->RecInput( mup->SF[nPHrec] );
        TPhase::pm->Show(window(), "Phase demo mode", true);
    }
}




//------------------ End of m_prfget2.cpp --------------------------

