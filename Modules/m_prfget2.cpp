//-------------------------------------------------------------------
// $Id: m_prfget2.cpp 1402 2009-08-17 11:57:38Z gems $
//
// Implementation of TProfile class, elements list manipulation functions
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
#ifndef _WIN32
#include <unistd.h>
#else
#include <io.h>
#endif

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include "m_unspace.h"
#include "m_dualth.h"
#include "m_gem2mt.h"
#include "m_syseq.h"
#include "visor.h"
#include "m_dcomp.h"
#include "m_icomp.h"
#include "m_compos.h"
#include "m_reacdc.h"
#include "m_sdata.h"
#include "m_const.h"
#include "m_proces.h"
#include "service.h"

// save old lists of keys to compare
void TProfil::SaveOldList()
{
    RMULTS* mup = rmults->GetMU();
    Nold = mup->N;
    Lold = mup->L;
    Fiold = mup->Fi;
    Fisold = mup->Fis;
    Laold = mup->La;
    Lsold = mup->Ls;
    SBold = reinterpret_cast<char (*)[IC_RKLEN]>( new char[mup->N*IC_RKLEN]);
    memcpy( SBold, mup->SB, mup->N*IC_RKLEN*sizeof(char));
    SAold = reinterpret_cast<char (*)[BC_RKLEN]>( new char[mup->La*BC_RKLEN]);
    memcpy( SAold, mup->SA, mup->La*BC_RKLEN*sizeof(char));
    Llold =   new short[mup->Fi];
    memcpy( Llold, mup->Ll, mup->Fi*sizeof(short));
    SFold = reinterpret_cast<char (*)[PH_RKLEN]>( new char[mup->Fi*PH_RKLEN]);
    memcpy( SFold, mup->SF, mup->Fi*sizeof(char)*PH_RKLEN );
    SMold = reinterpret_cast<char (*)[DC_RKLEN]>( new char[mup->L*DC_RKLEN]);
    memcpy( SMold, mup->SM, mup->L*sizeof(char)*DC_RKLEN );
}

// delete old lists of keys to compare
void TProfil::DeleteOldList()
{
    if( SBold )  delete[] SBold;
    SBold= nullptr;
    if( SAold )  delete[] SAold;
    SAold = nullptr;
    if(  Llold ) delete[]  Llold;
    Llold = nullptr;
    if( SFold ) delete[] SFold;
    SFold = nullptr;
    if( SMold ) delete[] SMold;
    SMold = nullptr;
}

// push element to the list - refurbished by DK on 15.02.2012
//
void TProfil::Push( std::vector<CompItem>& aList, int aLine,
                    short aDelta, const char* dbKeywd, std::string aKey )
{
   if( comp_change_all == false )
   {
       std::string stt = aKey;
       if( aDelta < 0 )
          stt += " record to be deleted from the project system. Action?";
       else stt += " record to be inserted into project system. Action?";

       switch( vfQuestion3(window(), dbKeywd, stt,
              "&Do it", "Do it for &All", "&Cancel" ))
       {
       case VF3_3:   // Skip: now skipping, as the user wants
                    Error( dbKeywd, "Comparison error!" );
                    break;
       case VF3_2:  // Do it for all
                    comp_change_all = true;
                    [[fallthrough]];
       case VF3_1:  // Do it for this item
                    aList.push_back(CompItem( aLine, aDelta));
       }
   }
   else
       aList.push_back( CompItem( aLine, aDelta));
}


// Compare IComp keys lists
void TProfil::ICcompare( std::vector<CompItem>& aIComp)
{
    int i, j, l;
    RMULTS* mup = rmults->GetMU();

    i = 0;
    j = 0;
    while( i<Nold && j<mup->N )
    {
        l = memcmp( SBold[i], mup->SB[j], IC_RKLEN-MAXICGROUP ); // 14/11/12 SD
        if( l==0 )
        {
            i++;
            j++;
        }
        else
            if( l<0 )
            {
                Push( aIComp, i, -1, "IComp", std::string(SBold[i], 0, IC_RKLEN) );
                i++;
            }
            else
            {
                Push( aIComp, i, 1, "IComp", std::string(mup->SB[j], 0, IC_RKLEN) );
                j++;
            }
    }
    while( i<Nold  )
    {
        Push( aIComp, i, -1, "IComp", std::string(SBold[i], 0, IC_RKLEN) );
        i++;
    }
    while( j<mup->N )
    {
        Push( aIComp, i, 1, "IComp", std::string(mup->SB[j], 0, IC_RKLEN) );
        j++;
    }
}

// Compare Compos keys lists
void TProfil::COMPcompare( std::vector<CompItem>& aCompos)
{
    int i, j, l;
    RMULTS* mup = rmults->GetMU();

    i = 0;
    j = 0;
    while( i<Laold && j<mup->La )
    {
        l = memcmp( SAold[i], mup->SA[j], BC_RKLEN-MAXCMPGROUP ); // 14/11/12 SD
        if( l==0 )
        {
            i++;
            j++;
        }
        else
            if( l<0 )
            {
                Push( aCompos, i, -1, "Compos", std::string(SAold[i], 0, BC_RKLEN) );
                i++;
            }
            else
            {
                Push( aCompos, i, 1, "Compos", std::string(mup->SA[j], 0, BC_RKLEN) );
                j++;
            }
    }
    while( i<Laold  )
    {
        Push( aCompos, i, -1, "Compos", std::string(SAold[i], 0, BC_RKLEN) );
        i++;
    }
    while( j<mup->La )
    {
        Push( aCompos, i, 1, "Compos", std::string(mup->SA[j], 0, BC_RKLEN) );
        j++;
    }
}

// compare DCOMP&REACT keys lists to one phase
void TProfil::DCcompare( std::vector<CompItem>& aList, int& i,
                         int& j, int nI, int nJ)
{
    int l;
    RMULTS* mup = rmults->GetMU();

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
                Push( aList, i, -1, "DComp/ReacDC", std::string(SMold[i], 0, DC_RKLEN) );
                i++;
            }
            else
            {
                Push( aList, i, 1, "DComp/ReacDC", std::string(mup->SM[j], 0, DC_RKLEN) );
                j++;
            }
    }
    while( i<nI  )
    {
        Push( aList, i, -1, "DComp/ReacDC", std::string(SMold[i], 0, DC_RKLEN) );
        i++;
    }
    while( j<nJ )
    {
        Push( aList, i, 1, "DComp/ReacDC", std::string(mup->SM[j], 0, DC_RKLEN) );
        j++;
    }
}

// Compare Phase and DComp&React keys lists
void TProfil::PHcompare( std::vector<CompItem>& aPhase,
                         std::vector<CompItem>& aDComp)
{
    int i, j, l;
    int id =0, jd=0;
    RMULTS* mup = rmults->GetMU();

    i = 0;
    j = 0;
    while( i<Fisold && j<mup->Fis )
    {
        l = memcmp( SFold[i], mup->SF[j], PH_RKLEN-MAXPHGROUP ); // fix KD 24.06.03
        if( l==0 )
        {
            DCcompare( aDComp, id, jd, id+Llold[i], jd+mup->Ll[j]);
            i++;
            j++;
        }
        else
            if( l<0 )
            {
                Push( aPhase, i, -1, "Phase", std::string(SFold[i], 0, PH_RKLEN) );
                for( int ii=id; ii<id+Llold[i]; ii++)
                    Push( aDComp, ii, -1, "DComp/ReacDC", std::string(SMold[ii], 0, DC_RKLEN) );
                id += Llold[i];
                i++;
            }
            else
            {
                Push( aPhase, i, 1, "Phase", std::string(mup->SF[j], 0, PH_RKLEN) );
                for( int jj=jd; jj<jd+mup->Ll[j]; jj++)
                    Push( aDComp, id, 1, "DComp/ReacDC", std::string(mup->SM[jj], 0, DC_RKLEN) );
                jd += mup->Ll[j];
                j++;
            }
    }
    while( i<Fisold  )
    {
        Push( aPhase, i, -1, "Phase", std::string(SFold[i], 0, PH_RKLEN) );
        for( int ii=id; ii<id+Llold[i]; ii++)
            Push( aDComp, ii, -1, "DComp/ReacDC", std::string(SMold[ii], 0, DC_RKLEN) );
        id += Llold[i];
        i++;
    }
    while( j<mup->Fis )
    {
        Push( aPhase, i, 1, "Phase", std::string(mup->SF[j], 0, PH_RKLEN) );
        for( int jj=jd; jj<jd+mup->Ll[j]; jj++)
            Push( aDComp, id, 1, "DComp/ReacDC", std::string(mup->SM[jj], 0, DC_RKLEN) );
        jd += mup->Ll[j];
        j++;
    }

    // compare one-component phase
    i = Fisold;
    j = mup->Fis;
    id =Lsold;
    jd=mup->Ls;
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
                Push( aPhase, i, -1, "Phase", std::string(SFold[i], 0, PH_RKLEN) );
                for( int ii=id; ii<id+Llold[i]; ii++)
                    Push( aDComp, ii, -1, "DComp/ReacDC", std::string(SMold[ii], 0, DC_RKLEN) );
                id += Llold[i];
                i++;
            }
            else
            {
                Push( aPhase, i, 1, "Phase", std::string(mup->SF[j], 0, PH_RKLEN) );
                for( int jj=jd; jj<jd+mup->Ll[j]; jj++)
                    Push( aDComp, id, 1, "DComp/ReacDC", std::string(mup->SM[jj], 0, DC_RKLEN) );
                jd += mup->Ll[j];
                j++;
            }
    }
    while( i<Fiold  )
    {
        Push( aPhase, i, -1, "Phase", std::string(SFold[i], 0, PH_RKLEN) );
        for( int ii=id; ii<id+Llold[i]; ii++)
            Push( aDComp, ii, -1, "DComp/ReacDC", std::string(SMold[ii], 0, DC_RKLEN) );
        id += Llold[i];
        i++;
    }
    while( j<mup->Fi )
    {
        Push( aPhase, i, 1, "Phase", std::string(mup->SF[j], 0, PH_RKLEN) );
        for( int jj=jd; jj<jd+mup->Ll[j]; jj++)
            Push( aDComp, id, 1, "DComp/ReacDC", std::string(mup->SM[jj], 0, DC_RKLEN) );
        jd += mup->Ll[j];
        j++;
    }
}

// test and insert changes to data base file
void TProfil::TestChangeProfile()
{
    char pkey[81];

    comp_change_all = false;
    // compare ICOMP list
    std::vector<CompItem> aIComp;  // list of IC changes
    ICcompare( aIComp);
    // compare COMPOS list
    std::vector<CompItem> aCompos;  // list of Compos changes
    COMPcompare( aCompos );
    // compare PHASE list
    std::vector<CompItem> aPhase;  // list of Phase changes
    std::vector<CompItem> aDComp;  // list of DComp changes
    PHcompare( aPhase, aDComp );

    if( aIComp.size()<1 && aCompos.size()<1 &&
            aPhase.size()<1 && aDComp.size()<1 )
        return;

    TCStringArray aList;
    TCIntArray anR;

    TSysEq* aSE= dynamic_cast<TSysEq *>(aMod[RT_SYSEQ].get());
    aSE->ods_link(0);


    // Insert changes to GEM2MT ( do it before Insert changes to SYSEQ  30/06/2011 )
    if( aIComp.size()>=1 || aPhase.size()>=1 || aDComp.size()>=1 )
    {
        aList.clear();
        anR.clear();

        rt[RT_GEM2MT]->MakeKey( RT_PARAM, pkey, RT_PARAM, 0,
                                K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_END);
        rt[RT_GEM2MT]->GetKeyList( pkey, aList, anR );

        TGEM2MT* aMT= dynamic_cast<TGEM2MT *>(aMod[RT_GEM2MT].get());
        aMT->ods_link(0);
        for(uint i=0; i< aList.size(); i++)
        {
            pVisor->Message( nullptr, "Loading Modelling Project",
                             "Detecting changes in TGEM2MT", 40+(5*i/aList.size()) );
            aMT->RecInput( aList[i].c_str() );
            //Get base SysEq key from TGEM2MT key
            rt[RT_SYSEQ]->MakeKey( RT_GEM2MT, pkey, RT_GEM2MT, 0, RT_GEM2MT, 1,
                                   RT_GEM2MT, 2, RT_GEM2MT, 3, RT_GEM2MT, 4,
                                   RT_GEM2MT, 5, RT_GEM2MT, 6, RT_GEM2MT, 7, K_END);
            // read SysEq record
            aSE->RecInput( pkey );
            aMT->InsertChanges( aIComp, aPhase, aDComp  );
            aMT->RecSave( aList[i].c_str(), true );
        }
    }

    aList.clear();
    anR.clear();

    // Insert changes to SYSEQ
    rt[RT_SYSEQ]->MakeKey( RT_PARAM, pkey, RT_PARAM, 0,
                           K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_END);
    rt[RT_SYSEQ]->GetKeyList( pkey, aList, anR );

    for(uint i=0; i< aList.size(); i++)
    {
        pVisor->Message( nullptr, "Loading Modelling Project",
                         "Detecting changes in SYSEQ", 45+(45*i/aList.size()) );
        //    int nRt = rt[RT_SYSEQ].Find( aList[i].c_str() );
        aSE->RecInput( aList[i].c_str() );
        aSE->InsertChanges( aIComp, aCompos, aPhase, aDComp );
        syst->loadData( false );
        syst->SyTestSizes(); //  Sveta 16/11/99
        aSE->RecSave( aList[i].c_str(), true );
        //    rt[RT_SYSEQ].Rep( nRt );
    }

    // Insert changes to DUALTH
    if( aIComp.size()>=1  )
    {
        aList.clear();
        anR.clear();

        rt[RT_DUALTH]->MakeKey( RT_PARAM, pkey, RT_PARAM, 0,
                                K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_END);
        rt[RT_DUALTH]->GetKeyList( pkey, aList, anR );

        TDualTh* aDU= dynamic_cast<TDualTh *>(aMod[RT_DUALTH].get());
        aDU->ods_link(0);
        for(size_t i=0; i< aList.size(); i++)
        {
            pVisor->Message( nullptr, "Loading Modelling Project",
                             "Detecting changes in DUALTH", 90+(5*i/aList.size()) );
            aDU->RecInput( aList[i].c_str() );
            aDU->InsertChanges( aIComp  );
            aDU->RecSave( aList[i].c_str(), true );
        }

    }

    // Insert changes to RT_UNSPACE
    if( aIComp.size()<1 &&  aPhase.size()<1 && aDComp.size()<1 )
        return;
    aList.clear();
    anR.clear();
    rt[RT_UNSPACE]->MakeKey( RT_PARAM, pkey, RT_PARAM, 0,
                             K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_END);
    rt[RT_UNSPACE]->GetKeyList( pkey, aList, anR );

    TUnSpace* aPB=dynamic_cast<TUnSpace *>(aMod[RT_UNSPACE].get());
    aPB->ods_link(-1); //0
    if( aList.size() > 0 )
    {  for(size_t i=0; i< aList.size(); i++)
        {
            pVisor->Message( nullptr, "Loading Modelling Project",
                             "Detecting changes in SYSEQ", 95+(5*i/aList.size()) );

            //    aPB->ods_link(0);
            aPB->RecInput( aList[i].c_str() );
            aPB->InsertChanges( aIComp, aPhase, aDComp );
            aPB->RecSave( aList[i].c_str(), true );
        }
        //        aPB->ods_link(1);
        //        aPB->din_kill(1);
        //        aPB->ods_link(0);
    }
}

int TProfil::indPH( int i )
{
    if( isSysEq == false )
        return i;

    for( size_t ii=0; ii<PHon.size(); ii++)
        if( i == PHon[ii])
            return static_cast<int>(ii);

    return -1;
}

int TProfil::indDC( int i )
{
    if( isSysEq == false )
        return i;
    for( size_t ii=0; ii<DCon.size(); ii++)
        if( i == DCon[ii])
            return static_cast<int>(ii);
    return -1;
}

void TProfil::CalcAllSystems( int SIA_or_AIA, bool make_dump_file )
{
    char pkey[81];
    char tbuf[150];
    std::string str_file;
    TCStringArray aList;
    TCIntArray anR;
    bool outFile = make_dump_file;
    uint i=0, nbad=0;
    int iRet;
    double dTime=0.; int kTimeStep =0; double kTime=0.;

    rt[RT_SYSEQ]->MakeKey( RT_PARAM, pkey, RT_PARAM, 0,
                           K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_END);
    rt[RT_SYSEQ]->GetKeyList( pkey, aList, anR );
    std::string ProfName(pkey);

    if( outFile )
    {
        //get file name
        size_t pos = ProfName.find(':');
        ProfName = ProfName.substr(0,pos);
        str_file = ProfName;
        str_file += "_" + curDateSmol('_')+"_dump_out.txt";
        // open file to output
AGAIN:
        if( vfChooseFileSave(nullptr/*window()*/, str_file,
                             "Please, enter output file name", "*.out" ) == false )
            return;
        if( !access(str_file.c_str(), 0 ) ) //file exists
            switch( vfQuestion3( nullptr/*window()*/, str_file,
                                 "This set of files exists!",
                                 "&Overwrite", "&Rename", "&Cancel") )
            {
            case VF3_2:
                goto AGAIN;
            case VF3_1:
            {
                fstream ff(str_file, ios::out );
            }
                break;
            case VF3_3: // only calc and save to db
                outFile = false;
                break;
                // return;
            }
    }

    pVisor->CloseMessage();
    MULTI *pmp = multi_internal->GetPM();
    TSysEq* aSE= dynamic_cast<TSysEq *>(aMod[RT_SYSEQ].get());
    aSE->ods_link(0);
    for(nbad =0,  i=0; i< aList.size(); i++)
    {
        sprintf( tbuf, "Project: %s; Systems: %d; Errors: %d", ProfName.c_str(), i, nbad );
        iRet =  pVisor->Message( nullptr, "Re-calculating and saving all equilibria", tbuf, i, aList.size() );
        if( iRet )
            break;

        loadSystat( aList[i].c_str() );
        if( SIA_or_AIA == 2 ) // NEED_GEM_SIA
            pmp->pNP = 1;
        else
            pmp->pNP = 0;     // NEED_GEM_AIA;

        bool goodCalc = true;
        try
        {
            showMss = 0L;
            CalcEqstat( dTime, kTimeStep, kTime );
        }
        catch( TError& /*xcpt*/ )
        {
            nbad++;
            goodCalc = false;
        }
        if( outFile )
            outMultiTxt( str_file.c_str(), true );
        if( goodCalc )  // Comment out for testing saving bad results (BugIssue0018)
            aSE->RecSave( aList[i].c_str(), true );
    }
    if( outFile )
    {
        fstream ff1(str_file, ios::out|ios::app);
        sprintf( tbuf, "\n\nProject: %s; Systems: %d; Errors: %d", ProfName.c_str(), i, nbad );
        ff1 << tbuf << endl;
    }

}

//Show IComp, DComp or other reactions from list
void TProfil::ShowDBWindow( const char *objName, int nLine )
{
    std::string s;
    RMULTS* mup = rmults->GetMU();
     MULTI *pmp = multi_internal->GetPM();
    time_t tr;
    const char* title = "Demonstrate in calculate Mode.";

    switch( *objName )
    {
    case 'I': // Icomp : IC_v__ or ICnam
        if( strncmp(objName, aObj[o_musb]->GetKeywd(), MAXKEYWD)==0)
            s = char_array_to_string( mup->SB[nLine], IC_RKLEN );
        else  if( strncmp(objName, aObj[o_wd_sb]->GetKeywd(), MAXKEYWD)==0 ||
                  strncmp(objName, aObj[o_w_sbh]->GetKeywd(), MAXKEYWD)==0 )
            s = char_array_to_string( mup->SB[pmp->mui[nLine]], IC_RKLEN );
        else break;
        TIComp::pm->RecInput( s.c_str() );
        TIComp::pm->Show(window(), title, false/*true*/);
        break;
    case 'C': // Compod : CC_v__
        if( strncmp(objName, aObj[o_musa]->GetKeywd(), MAXKEYWD)==0)
            s = char_array_to_string( mup->SA[nLine], BC_RKLEN );
        else break;
        TCompos::pm->RecInput( s.c_str() );
        TCompos::pm->Show(window(), title, false/*true*/);
        break;
    case 'P': // Phase : Ph_v__ or Ph_v2 or Phnam or Phnam2
        if( strncmp(objName, aObj[o_musf]->GetKeywd(), MAXKEYWD)==0 ||
                strncmp(objName, aObj[o_musf2]->GetKeywd(), MAXKEYWD)==0 )
            s = char_array_to_string( mup->SF[nLine], PH_RKLEN );
        else  if( strncmp(objName, aObj[o_wd_sf]->GetKeywd(), MAXKEYWD)==0 ||
                  strncmp(objName, aObj[o_wd_sf2]->GetKeywd(), MAXKEYWD)==0 )
            s = char_array_to_string( mup->SF[pmp->muk[nLine]], PH_RKLEN );
        else break;
        TPhase::pm->RecInput( s.c_str() );
        TPhase::pm->Show(window(), title, false/*true*/);
        break;
    case 'D': // Phase : DC_v__ or DC_v2 or DCnam or DCnam2
        if( strncmp(objName, aObj[o_musm]->GetKeywd(), MAXKEYWD)==0 ||
                strncmp(objName, aObj[o_musm2]->GetKeywd(), MAXKEYWD)==0 )
            s = char_array_to_string( mup->SM[nLine], DC_RKLEN );
        else  if( strncmp(objName, aObj[o_wd_sm]->GetKeywd(), MAXKEYWD)==0 ||
                  strncmp(objName, aObj[o_wd_sm2]->GetKeywd(), MAXKEYWD)==0 )
        {
            nLine = pmp->muj[nLine];
            s = char_array_to_string( mup->SM[nLine], DC_RKLEN );
        }
        else break;
        if( mup->DCS[nLine] == SRC_DCOMP )
        {
            //        TDComp::pm->RecInput( s.c_str() );
            TDComp::pm->TryRecInp( s.c_str(), tr, 0 );
            TDComp::pm->Show(window(), title, false/*true*/);
        }
        else
        {
            //              TReacDC::pm->RecInput( s.c_str() );
            TReacDC::pm->TryRecInp( s.c_str(), tr, 0 );
            TReacDC::pm->Show(window(), title, false/*true*/);
        }
        break;
    }
}

#include "filters_data.h"
// Save file configuration to Project structure
bool TProfil::rCopyFilterProfile( const char * prfName )
{

    uint ii;
    setFiltersData sf_data;
    elmWindowData  elm_data;
    std::set<std::string> SDlist;
    SDlist.clear();

//    Save list of phases from template profile:
    TCStringArray names1;
    names1.push_back(prfName);
    rt[RT_ICOMP]->OpenOnlyFromList(names1);
    rt[RT_PHASE]->OpenOnlyFromList(names1);

   TCStringArray PHkeys;
   TCIntArray    ICcnt;
   rt[RT_PHASE]->GetKeyList( "*:*:*:*:*:", PHkeys, ICcnt );


   // get template project configuration
   if( internalBufer )
      elm_data.aSelNames =  internalBufer;

//    TCStringArray dbNames;
//    bool aAqueous, aGaseous, aSorption;
//    if( !vfElements(window(), prfName, ICkeys,
//            dbNames, aAqueous, aGaseous, aSorption ))
    if( !vfElements(pVisor->window(), prfName, elm_data, sf_data ))
      return false;

// save built-in default configuration
     internalBufer = static_cast<char *>(aObj[ o_sptext]->Alloc( 1, elm_data.aSelNames.length()+10, S_));
     aObj[o_sptext]->SetString( elm_data.aSelNames.c_str(),0,0);


//    elm_data.flNames.Add(prfName);
    pVisor->Message( nullptr, "Loading Modelling Project",
"Copying Kernel database records to Modelling Project. Please, wait...", 10  );

    // added to project file icomp.kernel.prfname
    // and copy to it selected records
    // add to last key field first symbol from prfname
    // close all kernel files
    TIComp* aICdata=  dynamic_cast<TIComp *>(aMod[RT_ICOMP].get());
    aICdata->CopyElements( prfName, elm_data, sf_data.ic_d );
    ICcnt.clear();
    for( ii=0; ii<elm_data.ICrds.size(); ii++ )
       ICcnt.push_back(0);

    //compos
    TCompos* aCOdata=dynamic_cast<TCompos *>(aMod[RT_COMPOS].get());
    TCStringArray aCMnoused;
    aCOdata->CopyRecords( prfName, aCMnoused, elm_data, sf_data.cm_d, SDlist );

    //dcomp
    TDComp* aDCdata=dynamic_cast<TDComp *>(aMod[RT_DCOMP].get());
    aDCdata->CopyRecords( prfName, ICcnt, elm_data, sf_data.dc_d, SDlist );

    //reacds
    TReacDC* aRDdata= dynamic_cast<TReacDC *>(aMod[RT_REACDC].get());
    aRDdata->CopyRecords( prfName, ICcnt, elm_data, sf_data.rd_d, SDlist );

    //phase
    TPhase* aPHdata=dynamic_cast<TPhase *>(aMod[RT_PHASE].get());
    TCStringArray aPHnoused;
    aPHdata->CopyRecords( prfName, aPHnoused, PHkeys, elm_data, sf_data.ph_d, SDlist );

    //sdref
    TSData* aSDat= dynamic_cast<TSData *>(aMod[RT_SDATA].get());
    aSDat->CopyRecords( prfName, SDlist );

    //const for future
    TConst* aConst= dynamic_cast<TConst *>(aMod[RT_CONST].get());
    aConst->CopyRecords( prfName );

    //show errors
    TCStringArray aICnoused;
    for( ii=0; ii<elm_data.ICrds.size(); ii++ )
       if( ICcnt[ii] == 0 )
         aICnoused.push_back(elm_data.ICrds[ii]);

    if( aICnoused.size() > 0 )
      vfChoice(  window(), aICnoused, "List of unused Independent Components" );

    if( aPHnoused.size() > 0 || aCMnoused.size() > 0)
    {  // List of Phases or Compos with some species discarded
        ios::openmode mod = ios::out;
        std::string filename = pVisor->userGEMDir();
                filename +=  "DiscardedRecords.txt";
// This question is not needed anymore  DK 27.10.2005
/*      if( !(::access( filename, 0 )) ) //file exists
            switch( vfQuestion3( window(), filename,
                                 "This file exists! What to do?",
                                 "&Append", "&Overwrite", "&Cancel") )
            {
            case VF3_2:
                mod = ios::out;
                break;
            case VF3_1:
                mod = ios::out|ios::app;
                break;
            case VF3_3:
                return true;LoadMtparm
            }
*/
        fstream f( filename, mod );
        ErrorIf( !f.good() , filename, "Fileopen error");
        f <<   "Discarded Phase records\n";
        for( ii=0; ii<aPHnoused.size(); ii++ )
             f << aPHnoused[ii].c_str() <<  "\n";
        f <<   "\n\nDiscarded Compos records\n";
        for( ii=0; ii<aCMnoused.size(); ii++ )
             f << aCMnoused[ii].c_str() <<  "\n";
        f <<   "\n";
        ErrorIf( !f.good() , filename.c_str(), "Writefile error");
   }



    /*fstream f( "SDrefRecords.txt", ios::out );
    f << endl << "test out of sdref keys" << endl;
    for( ii=0; ii<SDlist.GetCount(); ii++ )
      f << SDlist[ii].c_str() << endl;*/

    useAqPhase = elm_data.flags[cbAqueous_];
    useGasPhase = elm_data.flags[cbGaseous_];
    return true;
}

//========================================================================
// SD 22/01/2010
// Define a single system recipe

void TProfil::systbcInput( QWidget* par, const char * p_key )
{
    int i,j;
    RMULTS* mup = rmults->GetMU();
    SYSTEM *syp = syst->GetSY();

    // define window
    std::vector<windowSetupData> wnData;
    // define table
    std::vector<tableSetupData> tbData;
    // define scalars
    std::vector<pagesSetupData> scalarsList;

    // add bulk chemical composition from COMPOS
    if( syp->PbAC != S_OFF && mup->La )
    {
        wnData.push_back( windowSetupData( "Compos",
                   o_syxea, 5, o_syxaun, o_syacl,  0., QUAN_GRAM) );
        for( j=0; j<mup->La; j++ )
        {
            if( syp->Acl[j] == S_OFF || approximatelyZero(syp->XeA[j]) || IsDoubleEmpty( syp->XeA[j] ))
                continue;
            tbData.push_back(  tableSetupData( wnData.size()-1, o_syxea,
                  aObj[o_syxea]->GetKeywd(), j, "",  syp->XeA[j], syp->XAun[j] ));
        } //  j
    }

    // add compositions from DCOMP/REACDC
    if( syp->PbDC != S_OFF )
    {
        wnData.push_back( windowSetupData( "DComp",
                o_syxed, 5, o_syxdun, o_sydcl, 0., QUAN_MOL) );
        for( j=0; j<mup->L; j++ )
        {
            if( syp->Dcl[j] == S_OFF || approximatelyZero(syp->XeD[j]) || IsDoubleEmpty( syp->XeD[j] ))
                continue;
            tbData.push_back( tableSetupData( wnData.size()-1, o_syxed,
                aObj[o_syxed]->GetKeywd(), j, "",  syp->XeD[j], syp->XDun[j] ));

        } //  j
    }

    if( syp->PbIC != S_OFF )
    { //  add bulk chemical composition from IComp
        wnData.push_back( windowSetupData( "IComp",
                       o_sybi, 5, o_sybiun, o_syicl, 0.,QUAN_MOL) );
        for( i=0; i<mup->N; i++ )
        {
            if( syp->Icl[i] == S_OFF || approximatelyZero(syp->BI[i]) || IsDoubleEmpty(syp->BI[i] ))
                continue;
            tbData.push_back( tableSetupData( wnData.size()-1, o_sybi,
                   aObj[o_sybi]->GetKeywd(), i, "",  syp->BI[i], syp->BIun[i] ));
        }
    }

    // add bulk chemical composition from phases
    if( syp->PbPH != S_OFF )
    {
      wnData.push_back( windowSetupData( "Phase",
                     o_syphm, 5, o_syxpun, o_sypcl, 0.,QUAN_GRAM) );
      for( i=0; i<mup->Fi; i++ )
      {
         if( syp->Pcl[i] == S_OFF || approximatelyZero(syp->Phm[i]) || IsDoubleEmpty(syp->XPun[i] ))
          continue;
        tbData.push_back( tableSetupData( wnData.size()-1, o_syphm,
             aObj[o_syphm]->GetKeywd(), i, "",  syp->Phm[i], syp->XPun[i] ));
      }
    }

    // and other dll, dul, Gex
    if(  syp->DLLim != S_OFF  )
    {
        wnData.push_back( windowSetupData( "Kin.lower",
            o_sydll, 4, o_syrsc, o_sydcl, 0., QUAN_MOL ) );
        for( i=0; i<mup->L; i++ )
        {
          if(  syp->Dcl[i] != S_OFF && syp->DLL[i] > 0 )
           tbData.push_back( tableSetupData( wnData.size()-1, o_sydll,
                 aObj[o_sydll]->GetKeywd(), i, "",  syp->DLL[i], syp->RSC[i] ));
        }
    }
    if(  syp->DULim != S_OFF  )
    {
        wnData.push_back( windowSetupData( "Kin.upper",
                o_sydul, 4, o_syrsc, o_sydcl, 1e+006, QUAN_MOL) );
        for( i=0; i<mup->L; i++ )
        {
          if( syp->Dcl[i] != S_OFF && syp->DUL[i] < 1e6 )
           tbData.push_back( tableSetupData( wnData.size()-1, o_sydul,
                 aObj[o_sydul]->GetKeywd(), i, "",  syp->DUL[i], syp->RSC[i] ));
        }
    }
    if( syp->PGEX != S_OFF )
    {
      wnData.push_back(  windowSetupData( "G0 shift",
               o_sygex, 7, -1, o_sydcl, 0.,'J' ) );
      for( i=0; i<mup->L; i++ )
      {
        if(  syp->Dcl[i] != S_OFF && fabs(syp->GEX[i]) > 0  )
         tbData.push_back( tableSetupData( wnData.size()-1, o_sygex,
               aObj[o_sygex]->GetKeywd(), i, "",  syp->GEX[i], 'J' ));
      }
    }

    // add static list
    wnData.push_back(  windowSetupData( "Other Inputs", -1, 0, -1, -1, 0.,'_' ) );
    // build scalar list
    scalarsList.push_back(  pagesSetupData("g Solids(MbXs)", o_symsolids)); // Total mass of solids (g) from another equilibrium (bXs object) to add to the recipe
 //   scalarsList.push_back(  pagesSetupData("Reserved", o_symass,0)); // Molality of reference electrolyte
    scalarsList.push_back(  pagesSetupData("kg H2O-solvent", o_symass,1)); // Anticipated mass (kg) of water-solvent
    scalarsList.push_back(  pagesSetupData("kg System", o_symass,2)); // Anticipated total mass of the system (kg)
    scalarsList.push_back(  pagesSetupData("kg Aqueous(Maq)", o_symass,3)); // Anticipated mass of aqueous phase Maq (kg)
 //   scalarsList.push_back(  pagesSetupData("kg Total (MBX)", o_symass,4)); // Final total mass of the system MBX (kg)_
 //   scalarsList.push_back(  pagesSetupData("Total IC moles (NMS)", o_symass,5)); // Total number of IC moles in the system NMS  (for mole %% calculations)
    scalarsList.push_back(  pagesSetupData("dm3 System(Vsys)", o_syvol,0)); // Anticipated volume Vsys of the system
    scalarsList.push_back(  pagesSetupData("dm3 Aqueous(Vaq)", o_syvol,1)); // Anticipated volume Vaq  of aqueous phase (L) for molarities
    scalarsList.push_back(  pagesSetupData("P_Min,bar", o_sypmm,0)); // min, max, increment for the pressure interpolation
    scalarsList.push_back(  pagesSetupData("P_Max,bar", o_sypmm,1));
    scalarsList.push_back(  pagesSetupData("P_Step,bar", o_sypmm,2));
    scalarsList.push_back(  pagesSetupData("T_Min,C", o_sytmm,0)); // min, max, increment for the temperature interpolation
    scalarsList.push_back(  pagesSetupData("T_Max,C", o_sytmm,1));
    scalarsList.push_back(  pagesSetupData("T_Step,C", o_sytmm,2));
    // add static values for table
    if(  fabs( syp->Msolids ) > 0  )
     tbData.push_back(  tableSetupData( wnData.size()-1, o_symsolids,
           aObj[o_symass]->GetKeywd(), 0, "",  syp->Msolids, '_' ));
//    if(  fabs( syp->Mbel ) > 0  )
//     tbData.push_back(  tableSetupData( wnData.size()-1, o_symass,
//           aObj[o_symass]->GetKeywd(), 0, "",  syp->Mbel, '_' ));
if(  !approximatelyEqual( fabs( syp->Mwat ), 1.f ) )  // Fixed for new defaults by DK 27.02.2012
     tbData.push_back(  tableSetupData( wnData.size()-1, o_symass,
           aObj[o_symass]->GetKeywd(), 1, "",  syp->Mwat, '_' ));
if(  !approximatelyEqual( fabs( syp->Msys ), 1.f)  )
     tbData.push_back(  tableSetupData( wnData.size()-1, o_symass,
           aObj[o_symass]->GetKeywd(), 2, "",  syp->Msys, '_' ));
if(  !approximatelyEqual( fabs( syp->Maq ), 1.f )  )
     tbData.push_back(  tableSetupData( wnData.size()-1, o_symass,
           aObj[o_symass]->GetKeywd(), 3, "",  syp->Maq, '_' ));
//   if(  fabs( syp->MBX ) > 0  )
//     tbData.push_back(  tableSetupData( wnData.size()-1, o_symass,
//           aObj[o_symass]->GetKeywd(), 4, "",  syp->MBX, '_' ));
//    if(  fabs( syp->R1 ) > 0  )
//    tbData.push_back(  tableSetupData( wnData.size()-1, o_symass,
//          aObj[o_symass]->GetKeywd(), 5, "",  syp->R1, '_' ));
if(  !approximatelyEqual( fabs( syp->Vsys ), 1.f)  )
     tbData.push_back(  tableSetupData( wnData.size()-1, o_syvol,
           aObj[o_syvol]->GetKeywd(), 0, "",  syp->Vsys, '_' ));
if(  !approximatelyEqual( fabs( syp->Vaq),  1.f)  )
     tbData.push_back(  tableSetupData( wnData.size()-1, o_syvol,
           aObj[o_syvol]->GetKeywd(), 1, "",  syp->Vaq, '_' ));
    if(  fabs( syp->Pmin ) > 0  )
     tbData.push_back(  tableSetupData( wnData.size()-1, o_sypmm,
           aObj[o_sypmm]->GetKeywd(), 0, "",  syp->Pmin, '_' ));
    if(  fabs( syp->Pmax ) > 0  )
     tbData.push_back(  tableSetupData( wnData.size()-1, o_sypmm,
           aObj[o_sypmm]->GetKeywd(), 1, "",  syp->Pmax, '_' ));
    if(  fabs( syp->Pinc ) > 0  )
     tbData.push_back(  tableSetupData( wnData.size()-1, o_sypmm,
           aObj[o_sypmm]->GetKeywd(), 2, "",  syp->Pinc, '_' ));

    if(  fabs( syp->Tmin ) > 0  )
     tbData.push_back(  tableSetupData( wnData.size()-1, o_sytmm,
           aObj[o_sytmm]->GetKeywd(), 0, "",  syp->Tmin, '_' ));
    if(  fabs( syp->Tmax ) > 0  )
     tbData.push_back(  tableSetupData( wnData.size()-1, o_sytmm,
           aObj[o_sytmm]->GetKeywd(), 1, "",  syp->Tmax, '_' ));
    if(  fabs( syp->Tinc ) > 0  )
     tbData.push_back(  tableSetupData( wnData.size()-1, o_sytmm,
           aObj[o_sytmm]->GetKeywd(), 2, "",  syp->Tinc, '_' ));

   // change bulk chemical composition
    if( !vfSystemInput( par, p_key, wnData, tbData, scalarsList ))
        return;      // cancel

    // clear old values
    if( mup->La && syp->PbAC != S_OFF )
    {
        fillValue( syp->XAun, char(QUAN_GRAM), mup->La );
        fillValue( syp->XeA, 0., mup->La );
    }
    if( syp->PbIC != S_OFF )
    {
        fillValue( syp->BIun, char(QUAN_MOL), mup->N );
        fillValue( syp->BI, 0., mup->N );
    }
    if( syp->PbDC != S_OFF )
    {
        fillValue( syp->XDun, char(QUAN_MOL), mup->L );
        fillValue( syp->XeD, 0., mup->L );
    }
    if( syp->PbPH != S_OFF )
    {
        fillValue( syp->XPun, char(QUAN_GRAM), mup->Fi );
        fillValue( syp->Phm, 0., mup->Fi );
    }
    if( syp->DLLim != S_OFF )
    {
        fillValue( syp->RSC, char(QUAN_MOL), mup->L );
        fillValue( syp->DLL, (0.), mup->L );
    }
    if( syp->DULim != S_OFF )
    {
        fillValue( syp->RSC, char(QUAN_MOL), mup->L );
        fillValue( syp->DUL, (1e6), mup->L );
    }
    if( syp->PGEX != S_OFF )
        fillValue( syp->GEX, (0.), mup->L );

    // clear static values
    syp->Msolids = 0.;
//    syp->Mbel = 0.;
 syp->Mwat  = 1.;  // Changed defaults from 0 to 1   DK 27.02.2012
 syp->Msys = 1.;
 syp->Maq = 1.;
//    syp->MBX = 0.;
//    syp->R1 = 0.;
 syp->Vsys = 1.;
 syp->Vaq = 1.;
    syp->Pmin = 0.;
    syp->Pmax = 0.;
    syp->Pinc = 0.;
    syp->Tmin = 0.;
    syp->Tmax = 0.;
    syp->Tinc = 0.;

    // inset data from tbData
    for(size_t ii=0; ii< tbData.size() ; ii++)
    {
     uint nO = tbData[ii].nObj;
     if( aObj[nO]->GetN() > 1 )
         aObj[ nO ]->Put( tbData[ii].val, tbData[ii].nIdx, 0 );
     else
         aObj[ nO ]->Put( tbData[ii].val, 0, tbData[ii].nIdx );

     int nOunit = wnData[ tbData[ii].nWin].nOunit;
     if(nOunit >=0 )
        aObj[ nOunit ]->SetString( &tbData[ii].unit, tbData[ii].nIdx, 0);
     int nOswitch = wnData[ tbData[ii].nWin].nSwitch;
     if(nOswitch >=0 )
        aObj[ nOswitch ]->SetString("+",tbData[ii].nIdx, 0);
    }

    TSysEq::pm->CellChanged();
}


int TProfil::PhIndexforDC( int xdc, bool system )
{
  int k, DCx = 0;
  RMULTS* mup = rmults->GetMU();
  MULTI*  pmp = multi_internal->GetPM();

  if( system )
  { for( k=0; k<mup->Fi; k++ )
    {
      DCx += mup->Ll[k];
      if( xdc < DCx )
        break;
    }
  }
  else
  { for( k=0; k<pmp->FI; k++ )
    {
      DCx += pmp->L1[k];
      if( xdc < DCx )
        break;
    }
  }
  return k;
}

std::string TProfil::PhNameforDC( int xdc, bool system )
{
  int k = PhIndexforDC( xdc, system );

  if( system )
   return char_array_to_string( rmults->GetMU()->SF[k]+MAXSYMB+MAXPHSYMB, MAXPHNAME);
  else
   return char_array_to_string( multi_internal->GetPM()->SF[k]+MAXSYMB, MAXPHNAME);
}


std::string TProfil::PhNameforDC( int xdc, int& xph, bool system )
{
  xph = PhIndexforDC( xdc, system );

  if( system )
   return char_array_to_string( rmults->GetMU()->SF[xph], PH_RKLEN);
  else
   return char_array_to_string( multi_internal->GetPM()->SF[xph], MAXPHNAME+MAXSYMB);
}

TCStringArray TProfil::DCNamesforPh( const char *PhName, bool system )
{
  int k, j, DCx = 0;
  auto len = strlen( PhName );
  RMULTS* mup = rmults->GetMU();
  MULTI*  pmp = multi_internal->GetPM();
  TCStringArray DCnames;
  std::string dcstr;

  if( system )
  { for( k=0; k<mup->Fi; k++ )
    {
      if( !memcmp(PhName, mup->SF[k]+MAXSYMB+MAXPHSYMB, min<size_t>(len,MAXPHNAME)))
        break;
      DCx += mup->Ll[k];
    }
    for( j= DCx; j<DCx+mup->Ll[k];j++ )
      {
        dcstr = char_array_to_string( mup->SM[j]+MAXSYMB+MAXDRGROUP, MAXDCNAME );
        strip( dcstr );
        DCnames.push_back(dcstr);
      }
  }
  else
  { for( k=0; k<pmp->FI; k++ )
    {
       if( !memcmp(PhName, pmp->SF[k]+MAXSYMB, min<size_t>(len,MAXPHNAME)))
           break;
       DCx += pmp->L1[k];
    }
    for( j= DCx; j<DCx+pmp->L1[k];j++ )
    {   dcstr =  char_array_to_string( pmp->SM[j], MAXDCNAME );
        strip( dcstr );
        DCnames.push_back(dcstr);
    }
  }
  return DCnames;
}

void TProfil::DCNamesforPh( int xph, bool system, vector<int>& xdc, vector<std::string>& dcnames)
{
    int k, j, DCx = 0;
    RMULTS* mup = rmults->GetMU();
    MULTI*  pmp = multi_internal->GetPM();

    if( system )
    { for( k=0; k<xph; k++ )
        DCx += mup->Ll[k];
      for( j= DCx; j<DCx+mup->Ll[xph];j++ )
        {
          xdc.push_back(j);
          dcnames.push_back( char_array_to_string( mup->SM[j], DC_RKLEN ));
        }
    }
    else
    {
      for( k=0; k<xph; k++ )
         DCx += pmp->L1[k];
      for( j= DCx; j<DCx+pmp->L1[k];j++ )
      {
          xdc.push_back(j);
          dcnames.push_back( char_array_to_string( pmp->SM[j], MAXDCNAME ));
      }
    }

}

//Show Phase DComp connections
void TProfil::ShowPhaseWindow( QWidget* par, const char *objName, int nLine )
{
    bool system = true;
    int  xph, xdc = -1;
    std::string phname;
    vector<int> xdclist;
    vector<std::string> dcnames;

    switch( *objName )
    {
    case 'D': // Phase : DC_v__ or DC_v2 or DCnam or DCnam2
        xdc = nLine;
        if( strncmp(objName, aObj[o_musm]->GetKeywd(), MAXKEYWD)==0 ||
                strncmp(objName, aObj[o_musm2]->GetKeywd(), MAXKEYWD)==0 )
           system = true;
        else  if( strncmp(objName, aObj[o_wd_sm]->GetKeywd(), MAXKEYWD)==0 ||
                  strncmp(objName, aObj[o_wd_sm2]->GetKeywd(), MAXKEYWD)==0 )
                   system = false;
               else
                  return;
        phname = PhNameforDC( xdc, xph, system );
        break;
    case 'P': // Phase : Ph_v__ or Ph_v2 or Phnam or Phnam2
        xph = nLine;
        xdc = -1;
        if( strncmp(objName, aObj[o_musf]->GetKeywd(), MAXKEYWD)==0 ||
                strncmp(objName, aObj[o_musf2]->GetKeywd(), MAXKEYWD)==0 )
        {    system = true;
             phname = char_array_to_string( rmults->GetMU()->SF[xph], PH_RKLEN);
        }
        else  if( strncmp(objName, aObj[o_wd_sf]->GetKeywd(), MAXKEYWD)==0 ||
                  strncmp(objName, aObj[o_wd_sf2]->GetKeywd(), MAXKEYWD)==0 )
             {
                system = false;
                phname = char_array_to_string( multi_internal->GetPM()->SF[xph], MAXSYMB+MAXPHNAME);
             }
            else
                return;
        break;
  }
  DCNamesforPh( xph, system, xdclist, dcnames);
  if( xdc < 0 )
      xdc = xdclist[0];

  vfPhaseInfo( par, system, xph, phname, xdclist, dcnames, xdc );
}

void TProfil::CurrentSystem2GEMS3K( const std::string& filepath, bool brief_mode, bool add_mui )
{
    double Tai[4], Pai[4];
    MULTI *pmp = TMulti::sm->GetPM();

    Tai[0] = Tai[1] = pmp->TCc;
    Pai[0] = Pai[1] = pmp->Pc;
    Tai[2] = Pai[2] = 0.;
    Tai[3] = Pai[3] = 0.1;

    auto na = TNodeArrayGUI::create(1, TMulti::sm);
    // realloc and setup data for dataCH and DataBr structures
    na->MakeNodeStructuresOne( nullptr, true , Tai, Pai  );

    ProcessProgressFunction messageF = [filepath](const std::string& message, long point){
        gui_logger->info("GEM3k output: {} {} point {}", filepath, message, point);
        return false;
    };
    na->genGEMS3KInputFiles(  filepath, messageF, 1, GEMS3KGenerator::default_type_f, brief_mode, false, false, add_mui );
}

void TProfil::System2GEMS3K( const std::string key, int calcMode, const std::string& filepath, bool brief_mode, bool add_mui )
{
    loadSystat( key.c_str() );

    /*  Do we need recalculate system before? */
    if( calcMode )
    {
        MULTI *pmp = multi_internal->GetPM();
        double dTime=0.; int kTimeStep =0; double kTime=0.;

        if( calcMode == 2 ) //NEED_GEM_SIA
            pmp->pNP = 1;
        else
            pmp->pNP = 0; //  NEED_GEM_AIA;

        CalcEqstat( dTime, kTimeStep, kTime );
    }
    CurrentSystem2GEMS3K( filepath, brief_mode, add_mui );
    auto multi_name = filepath.substr(0,filepath.length()-3);
    multi_name += "txt";
    outMultiTxt(multi_name.c_str(), true);
}


void TProfil::allSystems2GEMS3K( TCStringArray& savedSystems, int calc_mode, const std::string& files_dir, bool brief_mode, bool add_mui )
{
    pVisor->CloseMessage();

    char pkey[200];
    TCStringArray aList;
    TCIntArray anR;
    std::string packkey;
    std::string systemname;

    rt[RT_SYSEQ]->MakeKey( RT_PARAM, pkey, RT_PARAM, 0,
                          K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_END);
    rt[RT_SYSEQ]->GetKeyList( pkey, aList, anR );

    aMod[RT_SYSEQ]->ods_link(0);

    for( size_t ii=0; ii< aList.size(); ii++)
    {
        // test exists
        if( rt[RT_SYSEQ]->Find( aList[ii].c_str() ) < 0 )
            continue;
        // get key in pack form
        packkey = rt[RT_SYSEQ]->PackKey();

        //test :000: in last field
        if( std::string(rt[RT_SYSEQ]->FldKey(7)).find("000") != std::string::npos )
            continue;
        // test output before
        if( std::find(savedSystems.begin(), savedSystems.end(), packkey) != savedSystems.end()  )
            continue;
        savedSystems.push_back(packkey);

        // generate name and create directory
        systemname = packkey.c_str();
        KeyToName(systemname);
        std::string recordPath = files_dir + systemname + "/";
        vfMakeDirectory( nullptr, recordPath.c_str(), 0 );

        recordPath += systemname+ "-dat.lst";
        try {
            gui_logger->info("generate name: {}", packkey);
            System2GEMS3K( packkey, calc_mode, recordPath, brief_mode, add_mui );
        } catch (TError& xcpt) {
            gui_logger->error("Record out error: {}  :{}", packkey, xcpt.mess);
        }

        // stop point
         if( pVisor->Message( nullptr, "Re-calculating and saving all equilibria", pkey, ii, aList.size() ))
            break;
    }
}

void TProfil::allProcess2GEMS3K( TCStringArray& savedSystems, const std::string& files_dir, bool brief_mode, bool add_mui )
{
    pVisor->CloseMessage();

    char pkey[200];
    TCStringArray aList;
    TCIntArray anR;
    std::string process_name;

    rt[RT_PROCES]->MakeKey( RT_PARAM, pkey, RT_PARAM, 0,
                            K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_END);
    rt[RT_PROCES]->GetKeyList( pkey, aList, anR );
    aMod[RT_PROCES]->ods_link(0);


    if( aList.size() > 0 )
        vfMakeDirectory( nullptr, files_dir.c_str(), 2 );

    for( size_t ii=0; ii< aList.size(); ii++)
    {
        // test exists
        if( rt[RT_PROCES]->Find( aList[ii].c_str() ) < 0 )
            continue;

        TProcess::pm->RecInput( aList[ii].c_str() );
        TProcess::pm->check_input( aList[ii].c_str(), 0 );

        // generate name and create directory
        process_name = rt[RT_PROCES]->PackKey();
        strip( process_name );
        KeyToName(process_name);
        std::string recordPath = files_dir + process_name + "/";
        vfMakeDirectory( nullptr, recordPath.c_str(), 0 );

        recordPath += process_name+ "-dat.lst";
        try {
            TProcess::pm->genGEM3K( recordPath, savedSystems, brief_mode, add_mui );
        } catch (TError& xcpt) {
            gui_logger->error("Process record out error: {}  :{}", aList[ii], xcpt.mess);
        }

        // stop point
         if( pVisor->Message( nullptr, "Generating GEMS3K for process records", pkey, ii, aList.size() ))
            break;
    }
}

// makeCalc: 0 - no recalculation; 2- NEED_GEM_SIA; 1-NEED_GEM_AIA
void TProfil::GEMS3KallSystems( int makeCalc, bool brief_mode, bool add_mui )
{
    // Select destination
    std::string dir;
    if( !vfChooseDirectory( nullptr, dir,"Please, enter output directory location." ))
        return;

    try{
        // Generate data from process
        TCStringArray savedSystems;
        std::string processPath = dir + "/Processes/";
        //vfMakeDirectory( nullptr, processPath.c_str(), 2 );
        allProcess2GEMS3K( savedSystems, processPath, brief_mode, add_mui );

        // Save systems
        std::string systemsPath = dir + "/Standalone/";
        vfMakeDirectory( nullptr, systemsPath.c_str(), 2 );
        allSystems2GEMS3K( savedSystems, makeCalc, systemsPath, brief_mode, add_mui );
    }
    catch( TError& xcpt )
    {
       gui_logger->error("Record out error: {}", xcpt.mess);
    }
}

void TProfil::generate_ThermoFun_input_file_stream(iostream &stream, bool compact)
{
    QJsonObject thermo_data;
    QJsonArray datasources;
    datasources.append("gems.web.psi.ch");
    thermo_data["datasources"] = datasources;
    QJsonArray thermodataset;
    thermodataset.append("GEMS3gui");
    thermo_data["thermodataset"] = thermodataset;
    thermo_data["date"] = (curDate()+" "+curTime()).c_str();
    thermo_data["elements"] =  TIComp::pm->all_to_thermofun();
    auto subst_arr = TDComp::pm->all_to_thermofun();
    thermo_data["reactions"] = TReacDC::pm->all_to_thermofun(subst_arr);
    thermo_data["substances"] = subst_arr;
    QJsonDocument saveDoc(thermo_data);
    auto json_str = saveDoc.toJson((compact ? QJsonDocument::Compact :QJsonDocument::Indented)).toStdString();
    stream << json_str << std::endl;
}


void  TProfil::exportJsonFiles(QWidget* par)
{
    // Select destination
    string project_name = projectName();
    std::string dir;
    if( !vfChooseDirectory( par, dir,"Please, enter output directory location." ))
        return;

    for(size_t i=RT_ICOMP; i<RT_SYSEQ; i++) {
        if( aMod[i]->IsSubModule() )
            continue;
        std::string filename = dir+ "/" + aMod[i]->GetName() + "." +project_name + ".json";
        dynamic_cast<TCModule*>(aMod[i].get())->RecListToJSON("*", filename, true);
    }
}

void  TProfil::importJsonFiles(QWidget* par)
{
    std::string dir;
    if( !vfChooseDirectory( par, dir,"Please, enter input directory location." ))
        return;

    QDir thisDir(dir.c_str());
    if (!thisDir.exists())
        return;
    thisDir.setFilter(QDir::Files);

    for(size_t i=RT_ICOMP; i<RT_SYSEQ; i++) {
        if( aMod[i]->IsSubModule() )
            continue;

         std::string file_filter = std::string(aMod[i]->GetName()) + ".*.json";
         auto files = thisDir.entryList( {file_filter.c_str()});
         foreach(QString filename, files) {
           //std::cout <<filename.toStdString() << std::endl;
           dynamic_cast<TCModule*>(aMod[i].get())->RecListFromJSON(dir+"/"+filename.toStdString());
         }
    }
}

//------------------ End of m_prfget2.cpp --------------------------

