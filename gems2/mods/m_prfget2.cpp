//-------------------------------------------------------------------
// $Id$
//
// Implementation of TProfile class, elements list manipulation functions
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
#ifdef __unix
#include <unistd.h>
#else
#include <io.h>
#endif

#include "m_param.h"
#include "m_unspace.h"
#include "m_dualth.h"
#include "m_gem2mt.h"
#include "m_syseq.h"
#include "service.h"
#include "visor.h"


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


    if( comp_change_all == false )
      switch( vfQuestion3(window(), dbKeywd, stt.c_str(),
              "&Yes", "Yes for &All", "&Cancel" ))
    {
     case VF3_3:
           Error( dbKeywd, "Comparison error!" );
     case VF3_2:
                comp_change_all = true;
     case VF3_1:   ;

     }
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
        l = memcmp( SBold[i], mup->SB[j], IC_RKLEN-MAXICGROUP ); // 14/11/12 SD
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
        l = memcmp( SAold[i], mup->SA[j], BC_RKLEN-MAXCMPGROUP ); // 14/11/12 SD
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

    comp_change_all = false;
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

    // Insert changes to DUALTH
    if( aIComp.GetCount()>=1  )
    {
      aList.Clear();
      anR.Clear();

       rt[RT_DUALTH].MakeKey( RT_PARAM, pkey, RT_PARAM, 0,
       K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_END);
       rt[RT_DUALTH].GetKeyList( pkey, aList, anR );

       TDualTh* aDU= (TDualTh *)(&aMod[RT_DUALTH]);
       aDU->ods_link(0);
       for(uint i=0; i< aList.GetCount(); i++)
       {
         aDU->RecInput( aList[i].c_str() );
         aDU->InsertChanges( aIComp  );
         aDU->RecSave( aList[i].c_str(), true );
       }

    }

    // Insert changes to GEM2MT
    if( aIComp.GetCount()>=1 || aPhase.GetCount()>=1 )
    {
      aList.Clear();
      anR.Clear();

       rt[RT_GEM2MT].MakeKey( RT_PARAM, pkey, RT_PARAM, 0,
       K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_END);
       rt[RT_GEM2MT].GetKeyList( pkey, aList, anR );

       TGEM2MT* aMT= (TGEM2MT *)(&aMod[RT_GEM2MT]);
       aMT->ods_link(0);
       for(uint i=0; i< aList.GetCount(); i++)
       {
         aMT->RecInput( aList[i].c_str() );
         aMT->InsertChanges( aPhase, aIComp, aDComp  );
         aMT->RecSave( aList[i].c_str(), true );
       }

    }

    // Insert changes to PROBE
    if( aIComp.GetCount()<1 &&
            aPhase.GetCount()<1 && aDComp.GetCount()<1 )
        return;
    aList.Clear();
    anR.Clear();
    rt[RT_UNSPACE].MakeKey( RT_PARAM, pkey, RT_PARAM, 0,
      K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_END);
    rt[RT_UNSPACE].GetKeyList( pkey, aList, anR );

    TUnSpace* aPB=(TUnSpace *)(&aMod[RT_UNSPACE]);
    aPB->ods_link(-1); //0
    if( aList.GetCount() > 0 )
    {  for(uint i=0; i< aList.GetCount(); i++)
       {
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

void TProfil::CalcAllSystems( int makeDump )
{
    double ccTime = 0.;
    vstr pkey(81);
    gstring str_file;
	TCStringArray aList;
    TCIntArray anR;

    rt[RT_SYSEQ].MakeKey( RT_PARAM, pkey, RT_PARAM, 0,
                           K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_END);
    rt[RT_SYSEQ].GetKeyList( pkey, aList, anR );

    //get file name
    gstring ProfName(pkey);
    size_t pos = ProfName.find(':');
    ProfName = ProfName.substr(0,pos);
    str_file = ProfName + "_" + curDateSmol('_')+".Dump.out";
    // open file to output
AGAIN:
    if( vfChooseFileSave(0/*window()*/, str_file,
        "Please, enter dump file name", "*.out" ) == false )
             return;
     if( !access(str_file.c_str(), 0 ) ) //file exists
      switch( vfQuestion3( 0/*window()*/, str_file.c_str(),
      "This set of files exists!",
           "&Overwrite", "&Rename", "&Cancel") )
          {
          case VF3_2:
              goto AGAIN;
          case VF3_1:
              {
        	  fstream ff(str_file.c_str(), ios::out );
              }
              break;
          case VF3_3:
              return;
          }

    pVisor->CloseMessage();
    TSysEq* aSE=(TSysEq *)(&aMod[RT_SYSEQ]);
    aSE->ods_link(0);
    for(uint i=0; i< aList.GetCount(); i++)
    {
       //    int nRt = rt[RT_SYSEQ].Find( aList[i].c_str() );
        pVisor->Message( 0, "Loading Modelling Project",
           "Calculating and dumping all systems", i, aList.GetCount() );
       loadSystat( aList[i].c_str() );
 	   if( makeDump == 2 ) //NEED_GEM_SIA
 		  pmp->pNP = 1;
      else
    	  pmp->pNP = 0; //  NEED_GEM_AIA;
     
 	  try
       {
 	   	showMss = 0L;
 	    ccTime += CalcEqstat( false);
 	    }
 	  catch( TError& xcpt )
 	    {}
       outMultiTxt( str_file.c_str(), true );    
        // aSE->RecSave( aList[i].c_str(), true );
    }

}

/*
// Show phase information in System Configurator
// not used anymore ?
// This has to be re-implemented using ListView dialog
void TProfil::ShowPhaseWindow()
{
    int k, nPHrec;
    TCStringArray aPHList;

    for( k=0; k<mup->Fi; k++ )
        aPHList.Add( gstring( mup->SF[k], 0, PH_RKLEN) );
    nPHrec = vfChoice(window(), aPHList, "Select a Phase name to show record");
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
    nPHrec = vfChoice(window(), aPHList, "Select a Phase name to show record");
    if( nPHrec >= 0 )
    {
        TPhase::pm->RecInput( mup->SF[nPHrec] );
        TPhase::pm->Show(window(), "Phase demo mode", true);
    }
}

*/
//------------------ End of m_prfget2.cpp --------------------------

