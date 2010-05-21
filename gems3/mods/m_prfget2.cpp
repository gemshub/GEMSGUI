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

#include "filters_data.h"
#include "m_param.h"
#include "m_unspace.h"
#include "m_dualth.h"
#include "m_gem2mt.h"
#include "m_syseq.h"
#include "service.h"
#include "visor.h"
#include "m_dcomp.h"
#include "m_icomp.h"
#include "m_compos.h"
#include "m_phase.h"
#include "m_reacdc.h"
#include "EquatSetupWidget.h"


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
    vstr tbuf(150);
    gstring str_file;
	TCStringArray aList;
    TCIntArray anR;
    bool outFile = true;
    uint i, nbad;
    int iRet;

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
        "Please, enter output file name", "*.out" ) == false )
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
          case VF3_3: // only calc and save to db
        	  outFile = false;
              break;
        	  // return;
          }

    pVisor->CloseMessage();
    TSysEq* aSE=(TSysEq *)(&aMod[RT_SYSEQ]);
    aSE->ods_link(0);
    for(nbad =0,  i=0; i< aList.GetCount(); i++)
    {

      //    int nRt = rt[RT_SYSEQ].Find( aList[i].c_str() );
      //
        sprintf( tbuf, "Project: %s; Systems: %d; Errors: %d", ProfName.c_str(), i, nbad );
        iRet =  pVisor->Message( 0, "Re-calculating and saving all equilibria", tbuf.p, i, aList.GetCount() );
      if( iRet )
        break;

      loadSystat( aList[i].c_str() );
 	   if( makeDump == 2 ) //NEED_GEM_SIA
 		  pmp->pNP = 1;
      else
    	  pmp->pNP = 0; //  NEED_GEM_AIA;

       bool goodCalc = true;
      try
       {
 	   	showMss = 0L;
 	    ccTime += CalcEqstat( false);
        }
        catch( TError& xcpt )
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
      fstream ff1(str_file.c_str(), ios::out|ios::app);
      sprintf( tbuf, "\n\nProject: %s; Systems: %d; Errors: %d", ProfName.c_str(), i, nbad );
      ff1 << tbuf.p << endl;
    }

}

//Show IComp, DComp or other reactions from list
void TProfil::ShowDBWindow( const char *objName, int nLine )
{
    gstring s;
    time_t tr;
    const char* title = "Demonstrate in calculate Mode.";

    switch( *objName )
    {
    case 'I': // Icomp : IC_v__ or ICnam
        if( strncmp(objName, aObj[o_musb].GetKeywd(), MAXKEYWD)==0)
            s = gstring( mup->SB[nLine], 0, IC_RKLEN );
        else  if( strncmp(objName, aObj[o_wd_sb].GetKeywd(), MAXKEYWD)==0 ||
                  strncmp(objName, aObj[o_w_sbh].GetKeywd(), MAXKEYWD)==0 )
            s = gstring( mup->SB[pmp->mui[nLine]], 0, IC_RKLEN );
        else break;
        TIComp::pm->RecInput( s.c_str() );
        TIComp::pm->Show(window(), title, true);
        break;
    case 'C': // Compod : CC_v__
        if( strncmp(objName, aObj[o_musa].GetKeywd(), MAXKEYWD)==0)
            s = gstring( mup->SA[nLine], 0, BC_RKLEN );
        else break;
        TCompos::pm->RecInput( s.c_str() );
        TCompos::pm->Show(window(), title, true);
        break;
    case 'P': // Phase : Ph_v__ or Ph_v2 or Phnam or Phnam2
        if( strncmp(objName, aObj[o_musf].GetKeywd(), MAXKEYWD)==0 ||
                strncmp(objName, aObj[o_musf2].GetKeywd(), MAXKEYWD)==0 )
            s = gstring( mup->SF[nLine], 0, PH_RKLEN );
        else  if( strncmp(objName, aObj[o_wd_sf].GetKeywd(), MAXKEYWD)==0 ||
                  strncmp(objName, aObj[o_wd_sf2].GetKeywd(), MAXKEYWD)==0 )
            s = gstring( mup->SF[pmp->muk[nLine]], 0, PH_RKLEN );
        else break;
        TPhase::pm->RecInput( s.c_str() );
        TPhase::pm->Show(window(), title, true);
        break;
    case 'D': // Phase : DC_v__ or DC_v2 or DCnam or DCnam2
        if( strncmp(objName, aObj[o_musm].GetKeywd(), MAXKEYWD)==0 ||
                strncmp(objName, aObj[o_musm2].GetKeywd(), MAXKEYWD)==0 )
            s = gstring( mup->SM[nLine], 0, DC_RKLEN );
        else  if( strncmp(objName, aObj[o_wd_sm].GetKeywd(), MAXKEYWD)==0 ||
                  strncmp(objName, aObj[o_wd_sm2].GetKeywd(), MAXKEYWD)==0 )
        {
            nLine = pmp->muj[nLine];
            s = gstring( mup->SM[nLine], 0, DC_RKLEN );
        }
        else break;
        if( mup->DCS[nLine] == SRC_DCOMP )
        {
            //        TDComp::pm->RecInput( s.c_str() );
            TDComp::pm->TryRecInp( s.c_str(), tr, 0 );
            TDComp::pm->Show(window(), title, true);
        }
        else
        {
            //              TReacDC::pm->RecInput( s.c_str() );
            TReacDC::pm->TryRecInp( s.c_str(), tr, 0 );
            TReacDC::pm->Show(window(), title, true);
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

//    Save list of phases from template profile:
    TCStringArray names1;
    names1.Add(prfName);
    rt[RT_ICOMP].OpenOnlyFromList(names1);
    rt[RT_PHASE].OpenOnlyFromList(names1);

   TCStringArray PHkeys;
   TCIntArray    ICcnt;
   rt[RT_PHASE].GetKeyList( "*:*:*:*:*:", PHkeys, ICcnt );

//    TCStringArray dbNames;
//    bool aAqueous, aGaseous, aSorption;
//    if( !vfElements(window(), prfName, ICkeys,
//            dbNames, aAqueous, aGaseous, aSorption ))
    if( !vfElements(window(), prfName, elm_data, sf_data ))
      return false;

//    elm_data.flNames.Add(prfName);
    pVisor->Message( 0, "Loading Modelling Project",
"Copying Kernel database records to Modelling Project;\n Please, wait...", 10  );

    // added to project file icomp.kernel.prfname
    // and copy to it selected records
    // add to last key field first symbol from prfname
    // close all kernel files
    TIComp* aICdata=(TIComp *)(&aMod[RT_ICOMP]);
    aICdata->CopyElements( prfName, elm_data, sf_data.ic_d );
    ICcnt.Clear();
    for( ii=0; ii<elm_data.ICrds.GetCount(); ii++ )
       ICcnt.Add(0);

    //compos
    TCompos* aCOdata=(TCompos *)(&aMod[RT_COMPOS]);
    TCStringArray aCMnoused;
    aCOdata->CopyRecords( prfName, aCMnoused, elm_data, sf_data.cm_d );

    //dcomp
    TDComp* aDCdata=(TDComp *)(&aMod[RT_DCOMP]);
    aDCdata->CopyRecords( prfName, ICcnt, elm_data, sf_data.dc_d );

    //reacds
    TReacDC* aRDdata=(TReacDC *)(&aMod[RT_REACDC]);
    aRDdata->CopyRecords( prfName, ICcnt, elm_data, sf_data.rd_d );

    //phase
    TPhase* aPHdata=(TPhase *)(&aMod[RT_PHASE]);
    TCStringArray aPHnoused;
    aPHdata->CopyRecords( prfName, aPHnoused, PHkeys, elm_data, sf_data.ph_d );

    //show errors
    TCStringArray aICnoused;
    for( ii=0; ii<elm_data.ICrds.GetCount(); ii++ )
       if( ICcnt[ii] == 0 )
         aICnoused.Add(elm_data.ICrds[ii]);

    if( aICnoused.GetCount() > 0 )
      vfChoice(  window(), aICnoused, "List of unused Independent Components" );

    if( aPHnoused.GetCount() > 0 || aCMnoused.GetCount() > 0)
    {  // List of Phases or Compos with some species discarded
        ios::openmode mod = ios::out;
        const char *filename = "DiscardedRecords.txt";
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
        for( ii=0; ii<aPHnoused.GetCount(); ii++ )
             f << aPHnoused[ii].c_str() <<  "\n";
        f <<   "\n\nDiscarded Compos records\n";
        for( ii=0; ii<aCMnoused.GetCount(); ii++ )
             f << aCMnoused[ii].c_str() <<  "\n";
        f <<   "\n";
        ErrorIf( !f.good() , filename, "Writefile error");
   }

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

    // define window
    TIArray<windowSetupData> wnData;
    // define table
    TIArray<tableSetupData> tbData;
    // define scalars
    TIArray<pagesSetupData> scalarsList;

    // add bulk chemical composition from COMPOS
    if( syp->PbAC != S_OFF && mup->La )
    {
        wnData.Add( new windowSetupData( "Compos",
                   o_syxea, 5, o_syxaun, o_syacl,  0., QUAN_GRAM) );
        for( j=0; j<mup->La; j++ )
        {
            if( syp->Acl[j] == S_OFF || !syp->XeA[j] || IsDoubleEmpty( syp->XeA[j] ))
                continue;
            tbData.Add( new tableSetupData( wnData.GetCount()-1, o_syxea,
                  aObj[o_syxea].GetKeywd(), j, "",  syp->XeA[j], syp->XAun[j] ));
        } //  j
    }

    // add compositions from DCOMP/REACDC
    if( syp->PbDC != S_OFF )
    {
        wnData.Add( new windowSetupData( "DComp",
                o_syxed, 5, o_syxdun, o_sydcl, 0., QUAN_MOL) );
        for( j=0; j<mup->L; j++ )
        {
            if( syp->Dcl[j] == S_OFF || !syp->XeD[j] || IsDoubleEmpty( syp->XeD[j] ))
                continue;
            tbData.Add( new tableSetupData( wnData.GetCount()-1, o_syxed,
                aObj[o_syxed].GetKeywd(), j, "",  syp->XeD[j], syp->XDun[j] ));

        } //  j
    }

    if( syp->PbIC != S_OFF )
    { //  add bulk chemical composition from IComp
        wnData.Add( new windowSetupData( "IComp",
                       o_sybi, 5, o_sybiun, o_syicl, 0.,QUAN_MOL) );
        for( i=0; i<mup->N; i++ )
        {
            if( syp->Icl[i] == S_OFF || !syp->BI[i] || IsDoubleEmpty(syp->BI[i] ))
                continue;
            tbData.Add( new tableSetupData( wnData.GetCount()-1, o_sybi,
                   aObj[o_sybi].GetKeywd(), i, "",  syp->BI[i], syp->BIun[i] ));
        }
    }

    // add bulk chemical composition from phases
    if( syp->PbPH != S_OFF )
    {
      wnData.Add( new windowSetupData( "Phase",
                     o_syphm, 5, o_syxpun, o_sypcl, 0.,QUAN_GRAM) );
      for( i=0; i<mup->Fi; i++ )
      {
         if( syp->Pcl[i] == S_OFF || !syp->Phm[i] || IsDoubleEmpty(syp->XPun[i] ))
          continue;
        tbData.Add( new tableSetupData( wnData.GetCount()-1, o_syphm,
             aObj[o_syphm].GetKeywd(), i, "",  syp->Phm[i], syp->XPun[i] ));
      }
    }

    // and other dll, dul, Gex
    if(  syp->DLLim != S_OFF  )
    {
        wnData.Add( new windowSetupData( "Kin.lower",
            o_sydll, 4, o_syrsc, o_sydcl, 0., QUAN_MOL ) );
        for( i=0; i<mup->L; i++ )
        {
          if(  syp->Dcl[i] != S_OFF && syp->DLL[i] > 0 )
           tbData.Add( new tableSetupData( wnData.GetCount()-1, o_sydll,
                 aObj[o_sydll].GetKeywd(), i, "",  syp->DLL[i], syp->RSC[i] ));
        }
    }
    if(  syp->DULim != S_OFF  )
    {
        wnData.Add( new windowSetupData( "Kin.upper",
                o_sydul, 4, o_syrsc, o_sydcl, 1e+006, QUAN_MOL) );
        for( i=0; i<mup->L; i++ )
        {
          if( syp->Dcl[i] != S_OFF && syp->DUL[i] < 1e6 )
           tbData.Add( new tableSetupData( wnData.GetCount()-1, o_sydul,
                 aObj[o_sydul].GetKeywd(), i, "",  syp->DUL[i], syp->RSC[i] ));
        }
    }
    if( syp->PGEX != S_OFF )
    {
      wnData.Add( new windowSetupData( "G0 shift",
               o_sygex, 7, -1, o_sydcl, 0.,'J' ) );
      for( i=0; i<mup->L; i++ )
      {
        if(  syp->Dcl[i] != S_OFF && fabs(syp->GEX[i]) > 0  )
         tbData.Add( new tableSetupData( wnData.GetCount()-1, o_sygex,
               aObj[o_sygex].GetKeywd(), i, "",  syp->GEX[i], 'J' ));
      }
    }

    // add static list
    wnData.Add( new windowSetupData( "Other Inputs", -1, 0, -1, -1, 0.,'_' ) );
    // build scalar list
    scalarsList.Add( new pagesSetupData("g Solids(MbXs)", o_symsolids)); // Total mass of solids (g) from another equilibrium (bXs object) to add to the recipe
 //   scalarsList.Add( new pagesSetupData("Reserved", o_symass,0)); // Molality of reference electrolyte
    scalarsList.Add( new pagesSetupData("kg H2O-solvent", o_symass,1)); // Anticipated mass (kg) of water-solvent
    scalarsList.Add( new pagesSetupData("kg System", o_symass,2)); // Anticipated total mass of the system (kg)
    scalarsList.Add( new pagesSetupData("kg Aqueous(Maq)", o_symass,3)); // Anticipated mass of aqueous phase Maq (kg)
 //   scalarsList.Add( new pagesSetupData("kg Total (MBX)", o_symass,4)); // Final total mass of the system MBX (kg)_
 //   scalarsList.Add( new pagesSetupData("Total IC moles (NMS)", o_symass,5)); // Total number of IC moles in the system NMS  (for mole %% calculations)
    scalarsList.Add( new pagesSetupData("dm3 System(Vsys)", o_syvol,0)); // Anticipated volume Vsys of the system
    scalarsList.Add( new pagesSetupData("dm3 Aqueous(Vaq)", o_syvol,1)); // Anticipated volume Vaq  of aqueous phase (L) for molarities
    scalarsList.Add( new pagesSetupData("P_Min,bar", o_sypmm,0)); // min, max, increment for the pressure interpolation
    scalarsList.Add( new pagesSetupData("P_Max,bar", o_sypmm,1));
    scalarsList.Add( new pagesSetupData("P_Step,bar", o_sypmm,2));
    scalarsList.Add( new pagesSetupData("T_Min,C", o_sytmm,0)); // min, max, increment for the temperature interpolation
    scalarsList.Add( new pagesSetupData("T_Max,C", o_sytmm,1));
    scalarsList.Add( new pagesSetupData("T_Step,C", o_sytmm,2));
    // add static values for table
    if(  fabs( syp->Msolids ) > 0  )
     tbData.Add( new tableSetupData( wnData.GetCount()-1, o_symsolids,
           aObj[o_symass].GetKeywd(), 0, "",  syp->Msolids, '_' ));
//    if(  fabs( syp->Mbel ) > 0  )
//     tbData.Add( new tableSetupData( wnData.GetCount()-1, o_symass,
//           aObj[o_symass].GetKeywd(), 0, "",  syp->Mbel, '_' ));
    if(  fabs( syp->Mwat ) > 0  )
     tbData.Add( new tableSetupData( wnData.GetCount()-1, o_symass,
           aObj[o_symass].GetKeywd(), 1, "",  syp->Mwat, '_' ));
    if(  fabs( syp->Msys ) > 0  )
     tbData.Add( new tableSetupData( wnData.GetCount()-1, o_symass,
           aObj[o_symass].GetKeywd(), 2, "",  syp->Msys, '_' ));
    if(  fabs( syp->Maq ) > 0  )
     tbData.Add( new tableSetupData( wnData.GetCount()-1, o_symass,
           aObj[o_symass].GetKeywd(), 3, "",  syp->Maq, '_' ));
//   if(  fabs( syp->MBX ) > 0  )
//     tbData.Add( new tableSetupData( wnData.GetCount()-1, o_symass,
//           aObj[o_symass].GetKeywd(), 4, "",  syp->MBX, '_' ));
//    if(  fabs( syp->R1 ) > 0  )
//    tbData.Add( new tableSetupData( wnData.GetCount()-1, o_symass,
//          aObj[o_symass].GetKeywd(), 5, "",  syp->R1, '_' ));
    if(  fabs( syp->Vsys ) > 0  )
     tbData.Add( new tableSetupData( wnData.GetCount()-1, o_syvol,
           aObj[o_syvol].GetKeywd(), 0, "",  syp->Vsys, '_' ));
    if(  fabs( syp->Vaq ) > 0  )
     tbData.Add( new tableSetupData( wnData.GetCount()-1, o_syvol,
           aObj[o_syvol].GetKeywd(), 1, "",  syp->Vaq, '_' ));
    if(  fabs( syp->Pmin ) > 0  )
     tbData.Add( new tableSetupData( wnData.GetCount()-1, o_sypmm,
           aObj[o_sypmm].GetKeywd(), 0, "",  syp->Pmin, '_' ));
    if(  fabs( syp->Pmax ) > 0  )
     tbData.Add( new tableSetupData( wnData.GetCount()-1, o_sypmm,
           aObj[o_sypmm].GetKeywd(), 1, "",  syp->Pmax, '_' ));
    if(  fabs( syp->Pinc ) > 0  )
     tbData.Add( new tableSetupData( wnData.GetCount()-1, o_sypmm,
           aObj[o_sypmm].GetKeywd(), 2, "",  syp->Pinc, '_' ));

    if(  fabs( syp->Tmin ) > 0  )
     tbData.Add( new tableSetupData( wnData.GetCount()-1, o_sytmm,
           aObj[o_sytmm].GetKeywd(), 0, "",  syp->Tmin, '_' ));
    if(  fabs( syp->Tmax ) > 0  )
     tbData.Add( new tableSetupData( wnData.GetCount()-1, o_sytmm,
           aObj[o_sytmm].GetKeywd(), 1, "",  syp->Tmax, '_' ));
    if(  fabs( syp->Tinc ) > 0  )
     tbData.Add( new tableSetupData( wnData.GetCount()-1, o_sytmm,
           aObj[o_sytmm].GetKeywd(), 2, "",  syp->Tinc, '_' ));

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
        fillValue( syp->DLL, float(0.), mup->L );
    }
    if( syp->DULim != S_OFF )
    {
        fillValue( syp->RSC, char(QUAN_MOL), mup->L );
        fillValue( syp->DUL, float(1e6), mup->L );
    }
    if( syp->PGEX != S_OFF )
        fillValue( syp->GEX, float(0.), mup->L );

    // clear static values
    syp->Msolids = 0.;
//    syp->Mbel = 0.;
    syp->Mwat  = 0.;
    syp->Msys = 0.;
    syp->Maq = 0.;
//    syp->MBX = 0.;
//    syp->R1 = 0.;
    syp->Vsys = 0.;
    syp->Vaq = 0.;
    syp->Pmin = 0.;
    syp->Pmax = 0.;
    syp->Pinc = 0.;
    syp->Tmin = 0.;
    syp->Tmax = 0.;
    syp->Tinc = 0.;

    // inset data from tbData
    for(uint ii=0; ii< tbData.GetCount() ; ii++)
    {
     int nO = tbData[ii].nObj;
     if( aObj[nO].GetN() > 1 )
         aObj[ nO ].Put( tbData[ii].val, tbData[ii].nIdx, 0 );
     else
         aObj[ nO ].Put( tbData[ii].val, 0, tbData[ii].nIdx );

     int nOunit = wnData[ tbData[ii].nWin].nOunit;
     if(nOunit >=0 )
        aObj[ nOunit ].SetString( &tbData[ii].unit, tbData[ii].nIdx, 0);
      int nOswitch = wnData[ tbData[ii].nWin].nSwitch;
      if(nOswitch >=0 )
        aObj[ nOswitch ].SetString("+",tbData[ii].nIdx, 0);
    }

    TSysEq::pm->CellChanged();
}


gstring TProfil::PhNameforDC( int xdc, bool system )
{
  int k, DCx = 0;

  if( system )
  { for( k=0; k<mup->Fi; k++ )
    {
      DCx += mup->Ll[k];
      if( xdc < DCx )
        break;
    }
   return gstring( mup->SF[k]+MAXSYMB+MAXPHSYMB, 0, MAXPHNAME);
  }
  else
  { for( k=0; k<pmp->FI; k++ )
    {
      DCx += pmp->L1[k];
      if( xdc < DCx )
        break;
    }
   return gstring( pmp->SF[k]+MAXSYMB, 0, MAXPHNAME);
 }
}

TCStringArray TProfil::DCNamesforPh( const char *PhName, bool system )
{
  int k, j, DCx = 0, len = strlen( PhName );
  TCStringArray DCnames;
  gstring dcstr;

  if( system )
  { for( k=0; k<mup->Fi; k++ )
    {
      if( !memcmp(PhName, mup->SF[k]+MAXSYMB+MAXPHSYMB, min(len,MAXPHNAME)))
        break;
      DCx += mup->Ll[k];
    }
    for( j= DCx; j<DCx+mup->Ll[k];j++ )
      {
        dcstr = gstring( mup->SM[j]+MAXSYMB+MAXDRGROUP ,0, MAXDCNAME );
        dcstr.strip();
        DCnames.Add(dcstr);
      }
  }
  else
  { for( k=0; k<pmp->FI; k++ )
    {
       if( !memcmp(PhName, pmp->SF[k]+MAXSYMB, min(len,MAXPHNAME)))
           break;
       DCx += pmp->L1[k];
    }
    for( j= DCx; j<DCx+pmp->L1[k];j++ )
    {   dcstr =  gstring( pmp->SM[j],0, MAXDCNAME );
        dcstr.strip();
        DCnames.Add(dcstr);
    }
  }
  return DCnames;
}


//------------------ End of m_prfget2.cpp --------------------------

