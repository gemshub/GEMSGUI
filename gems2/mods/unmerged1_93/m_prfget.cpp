//-------------------------------------------------------------------
// Id: gems/mods/m_prfget.cpp  version 2.0.0   2001
//
// Implementation of TProfile class, loading, making and calc functions
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
#include "v_object.h"
#include "service.h"
#include "visor.h"

#ifdef Use_mt_mode
#include "visor_w.h"
#endif


//Init data to ProfileMode calculate
bool
TProfil::initCalcMode()
{
    isSysEq = false;

    // free old data
    dyn_kill();
    // Open all files in list to select
    rt[RT_PARAM].OpenAllFiles();

    // Get Profile record key from old list
    bool changeAqGas = false,
            addfiles = false;
    gstring str = vfKeyProfile( window(),
           "System Profiles", nRT, changeAqGas, addfiles);

    if( str.empty() ) // cancel command
          return false;


    if( str == ALLKEY )
     {    if(  !NewProfileMode(  ) )
            return false;
      }
    else
       OpenProfileMode(  str.c_str(),changeAqGas, addfiles );

    pVisor->Message( 0, "Loading Profile", "Loading thermodynamic data", 80 );

    // Test MTparm
        mtparm->MTparmAlloc();
        mtparm->LoadMtparm( 25., 1. );

    // Start settins of SYSTEM
        syst->set_def();
        syst->dyn_new();
        syst->setDefData();
/*
    // Read and set def SysEq if it exist
        vstr _fstKeyFld(rt[RT_PARAM].FldLen(0), rt[RT_PARAM].FldKey(0));
        gstring fstKeyFld(_fstKeyFld);
        StripLine(fstKeyFld);
        Set_z_sp_config( fstKeyFld.c_str() );
*/

    // Get first  SYSEQ
        vstr pkey(81);
        rt[RT_SYSEQ].MakeKey( RT_PARAM, pkey, RT_PARAM, 0,
                               K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_END);
        rt[RT_SYSEQ].SetKey( pkey );
/*        TCStringArray aList;
        TCIntArray anR;
        rt[RT_SYSEQ].GetKeyList( pkey, aList, anR );

//  ms.Update("Loading start system definition", 90 );

        if( aList.GetCount()>0 )
            loadSystat( aList[0].c_str() );
*/
        // update windows informations
   pVisor->CloseMessage();
   ModUpdate("Insertion of Profile record finished OK");
        return true;
}

//Opening Existing Profile
void TProfil::OpenProfileMode( const char* key,
       bool changeAqGas, bool addFile )
{
   int Rnum = -1;
 try
 {
        Rnum = rt[RT_PARAM].Find( key );
        ErrorIf( Rnum < 0, "Profile" , "Profile record was not existed!");

      pVisor->Message( 0, "Loading Profile",
       "Opening data base files to Profile", 5 );

        rt[RT_PARAM].Get( Rnum ); // read record
        dyn_set();
        SetFN();                  // reopen files of data base

        if( addFile == true )
        {  // part to add files to list
           rt[RT_PARAM].SetKey( key );
           vstr _fstKeyFld(rt[RT_PARAM].FldLen(0), rt[RT_PARAM].FldKey(0));
           gstring fstKeyFld(_fstKeyFld);
           StripLine(fstKeyFld);

           GetFN( fstKeyFld.c_str() );
           SetFN();
        }

        rt[RT_PARAM].SetKey( key );
        SaveOldList();

        // delete default phases if it is
        int nRec = rt[RT_PHASE].Find( defaultAqKey );
        if( nRec >= 0 )
            rt[RT_PHASE].Del(nRec);
        nRec = rt[RT_PHASE].Find( defaultGasKey );
        if( nRec >= 0 )
            rt[RT_PHASE].Del(nRec);

      pVisor->Message( 0, "Loading Profile",
       "Loading lists of IComp, Compos, Phase\n"
                  "  and DComp/ReacDC record keys", 10 );

        // load lists of ICOMP, COMPOS, PHASE and DCOMP&REACT recordc keys
        Rnum = rt[RT_PARAM].Find( key );
        rmults->LoadRmults( false, changeAqGas );

      pVisor->Message( 0, "Loading Profile",
       "Detecting changes in thermodynamic database", 40 );


   TestChangeProfile();  // test and insert changes to data base file
   DeleteOldList();
   rt[RT_PARAM].Rep( Rnum );
 }
 catch( TError& xcpt )
 {
   pVisor->CloseMessage();
   rt[RT_PARAM].Get( Rnum );
   dyn_set();
   DeleteOldList();
        // delete default phases if it is
    int nRec = rt[RT_PHASE].Find( defaultAqKey );
    if( nRec >= 0 )
            rt[RT_PHASE].Del(nRec);
    nRec = rt[RT_PHASE].Find( defaultGasKey );
    if( nRec >= 0 )
            rt[RT_PHASE].Del(nRec);
    fEdit = false;
    throw; // going on
 }

}

//Making new Profile
bool TProfil::NewProfileMode()
{
 try
 {
    bool templ_key = false;
    bool elements_mode = false;

    gstring  templ_str;
AGAIN:
    gstring  key_str = GetKeyofRecord( ALLKEY,
            "Please, enter a new profile key", KEY_NEW );
    if( key_str.empty() )
      return false; // cancel command

    rt[RT_PARAM].SetKey( key_str.c_str() );
    vstr _fstKeyFld(rt[RT_PARAM].FldLen(0), rt[RT_PARAM].FldKey(0));
    gstring fstKeyFld(_fstKeyFld);
    StripLine(fstKeyFld);

    //Test equal profile names
    templ_str = fstKeyFld;
    templ_str += ":*:";
    TCStringArray aKey__;
    TCIntArray anR__;

    if(  db->GetKeyList( templ_str.c_str(), aKey__, anR__  ) >0 )
    {
      vfMessage(window(), fstKeyFld.c_str(),
        "Illegal name of Profile.\nTry again...");
      goto AGAIN;
    }
BACK:
   // new elements mode
   if( vfQuestion(window(), "Test", "Select elements mode") )
   {
    elements_mode = true;
    dyn_kill();
    set_def(); // set default data or zero if necessary
   }
   else
   {// select profile record key to template
    templ_str = GetKeyofRecord( ALLKEY,
            "Please, select a template profile", KEY_OLD );
    if( !templ_str.empty() )
    {
      templ_key = true;

      int  Rnum = rt[RT_PARAM].Find( templ_str.c_str() );
      ErrorIf( Rnum < 0, templ_str.c_str() ,
          "Profile record was not existed!");
      rt[RT_PARAM].Get( Rnum ); // read record
      dyn_set();
      SetFN();                  // reopen files of data base
    }
    else
    {
       dyn_kill();
       set_def(); // set default data or zero if necessary
    }
   }

   RecBuild( key_str.c_str() );  // Edit flags

   pVisor->Message( window(), "Loading Profile",
      "Opening data base files to Profile", 5  );

   rt[RT_PARAM].SetKey( key_str.c_str() );
//     vstr _fstKeyFld(rt[RT_PARAM].FldLen(0), rt[RT_PARAM].FldKey(0));
//     gstring fstKeyFld(_fstKeyFld);
//     StripLine(fstKeyFld);

     if( templ_key == false  )
        InitFN( fstKeyFld.c_str(), 0  ); // make Profile directory
     else  // using existing Profile
     {
        rt[RT_PARAM].SetKey( templ_str.c_str() );
        vstr _fstKeyFld_t(rt[RT_PARAM].FldLen(0), rt[RT_PARAM].FldKey(0));
        gstring fstKeyFld_t(_fstKeyFld_t);
        StripLine(fstKeyFld_t);

        InitFN( fstKeyFld.c_str(), fstKeyFld_t.c_str()  ); // make Profile directory
        RenameFN( fstKeyFld.c_str(), fstKeyFld_t.c_str()  );
      }
      if( elements_mode )
      {
        if( !rCopyFilterProfile( fstKeyFld.c_str() ) )
         ;//goto BACK;
      }
      // get opens files list
      if( !GetFN( fstKeyFld.c_str() ) )
        Error( key_str.c_str(),
           "Profile configuration aborted by the user!" );
      SetFN();

       if( templ_key == true  )
       {
         rt[RT_PARAM].SetKey( key_str.c_str() );
         SaveOldList();

         // delete default phases if it is
         int nRec = rt[RT_PHASE].Find( defaultAqKey );
         if( nRec >= 0 )
             rt[RT_PHASE].Del(nRec);
         nRec = rt[RT_PHASE].Find( defaultGasKey );
         if( nRec >= 0 )
             rt[RT_PHASE].Del(nRec);
       }

   pVisor->Message( 0, "Loading Profile",
      "Loading lists of IComp, Compos, Phase\n"
                "  and DComp/ReacDC record keys", 10);

        // load lists of ICOMP, COMPOS, PHASE and DCOMP&REACT recordc keys
        rt[RT_PARAM].SetKey( key_str.c_str() );
        rmults->LoadRmults( true, true );

   pVisor->Message( 0, "Loading Profile",
         "Detecting changes in thermodynamic database", 40 );

   if( templ_key == true )
   {
            TestChangeProfile();  // test and insert changes to data base file
            DeleteOldList();
   }

  // save results   RecSave(str.c_str());
   AddRecord( key_str.c_str() );

 }
 catch( TError& xcpt )
    {
     pVisor->CloseMessage();
      fEdit = false;
      //delete profile directory, if Profile record create error
      gstring fstKeyFld =
                gstring(rt[RT_PARAM].FldKey(0), 0, rt[RT_PARAM].FldLen(0));
      StripLine(fstKeyFld);

      gstring Path = pVisor->userProfDir();
      Path += fstKeyFld;
      pVisor->deleteDBDir(Path.c_str());
      throw;
    }
    return true;
}


// load  SysEq and unpack data
// record from data base
void TProfil::loadSystat( const char *key )
{
    vstr pkey(81);
    TSysEq* STat = (TSysEq*)(&aMod[RT_SYSEQ]);
    //  STat->ods_link(0);
    gstring str;

    if( key==0 )
    {
        rt[RT_SYSEQ].MakeKey( RT_PARAM, pkey, RT_PARAM, 0,
                               K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_END);
        str = STat->GetKeyofRecord( pkey, "Existing systems and equilibria", KEY_OLD );
        ErrorIf( str.empty(), GetName(), "Operation cancelled!");
    }
    else str=gstring(key);
    STat->keyTest( str.c_str() );

    // get record
    STat->RecInput( str.c_str() );


    // unpack to SYSTEM structure
    syst->loadData( false );  // set def and unpack syseq to system

    // Test MULTY for change (if new System cfg or T, P - new)
    gstring keyp = gstring( rt[RT_SYSEQ].UnpackKey(), 0, rt[RT_SYSEQ].KeyLen() );
    PMtest( keyp.c_str() );
    //  if( pmp->pBAL < 2 )  // rebuild multi
    MultiRemake( keyp.c_str() );
    if( pmp->pESU )      // unpack old solution
    {
        multi->loadData( false );  // unpack syseq to multi
        for(short j=0; j< pmp->L; j++ )
            pmp->X[j] = pmp->Y[j];
        pmp->pFAG =1; // Sveta 11/05/99
    }

    if( pmp->pFAG == 0 || pmp->pFAG == 1 )
        EqstatExpand( keyp.c_str() );
    pVisor->Update();
}

// rebuild loading before Systat
void TProfil::deriveSystat()
{
    TSysEq::pm->setCalcFlag( false );

    gstring keyp = rt[RT_SYSEQ].UnpackKey();
    if( keyp.find_first_of( "*?") != gstring::npos )
        Error("SyStat", "Undefined current record!");
    int ret = TSysEq::pm->RecBuild( keyp.c_str() );
    //  fEdit = false;
    //  TSysEq::pm->CloseWin();
    //  fEdit = true;

    // unpack to SYSTEM structure
    syst->loadData( false, ret );  // set def and unpack syseq to system

    // Test MULTY for change (if new System cfg or T, P - new)

    PMtest( keyp.c_str() );
    //    if( pmp->pBAL < 2 )  // rebuild multi
    MultiRemake( keyp.c_str() );
    if( pmp->pESU )      // unpack old solution
    {
        multi->loadData( false );  // unpack syseq to multi
        for(short j=0; j< pmp->L; j++ )
            pmp->X[j] = pmp->Y[j];
        pmp->pFAG =1;
    }
    if( pmp->pFAG == 0 || pmp->pFAG == 1 )
        EqstatExpand( keyp.c_str() );

    pVisor->Update();
    pVisor->OpenModule(window(), MD_SYSTEM);
}


//  build new Systat record
void TProfil::newSystat()
{ //vstr pkey(81);

    //rt[RT_SYSEQ].MakeKey( RT_PARAM, pkey, RT_PARAM, 0,
    //            K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_ANY, K_END);
    TSysEq::pm->setCalcFlag( false );

    gstring str = TSysEq::pm->GetKeyofRecord( rt[RT_SYSEQ].PackKey(),
                  "Please, enter a new key: ", KEY_NEW );
    if( str.empty() )
        return;
    if( rt[RT_SYSEQ].Find( str.c_str() ) >= 0 )
        Error("SyStat", "This record already exists!");

    int ret = TSysEq::pm->RecBuild( str.c_str() );
    //  fEdit = false;
    //  TSysEq::pm->CloseWin();
    //  fEdit = true;

    syst->loadData( true, ret ); // set def and unpack syseq to system

    // Test MULTY for change (if new System cfg or T, P - new)
    gstring keyp = rt[RT_SYSEQ].UnpackKey();
    PMtest( keyp.c_str() );
    //    if( pmp->pBAL < 2 )  // rebuild multi
    MultiRemake( keyp.c_str() );
    if( pmp->pESU )      // unpack old solution
    {
        multi->loadData( false );  // unpack syseq to multi
        for(short j=0; j< pmp->L; j++ )
            pmp->X[j] = pmp->Y[j];
        pmp->pFAG =1;
    }
    if( /* pmp->pFAG == 0 || */ pmp->pFAG == 1 ) // !!!!!!!!! 20.06.01
        EqstatExpand( keyp.c_str() );

    pVisor->OpenModule(window(), MD_SYSTEM);
    pVisor->Update();
}


// packed syseq arrays
void TProfil::PackSystat()
{
    TSysEq* STat = (TSysEq*)(&aMod[RT_SYSEQ]);
    //  STat->ods_link(0);

    //  syst->SyTest(); //  Sveta 15/05/99 ??? do it
    syst->setSizes();
    if( isSysEq ) // add to z_sp_config show mode 07/12/99
    {
        STat->ssp->DM[22] += PHoff.GetCount();
        STat->ssp->DM[23] += DCoff.GetCount();
    }
    multi->setSizes();
    STat->dyn_new();
    if( isSysEq ) // add to z_sp_config show mode 07/12/99
        syst->packData( PHon, PHoff, DCon, DCoff);
    else syst->packData();
    if( isSysEq ) // add to z_sp_config show mode 07/12/99
        multi->packData( PHon, DCon );
    else multi->packData();
    pVisor->Update();







    //Sveta
}

//Calculate system bulk composition        line # 300

void TProfil::CalcBcc()
{
    //     Switchoffs etc. - most important for the users!
    syst->SyTest();
    pVisor->Update();
    TSysEq::pm->CellChanged();
    syst->SyTest();               // Must be done twice
    pVisor->Update(true);         // added 24.01.00
    TSysEq::pm->CellChanged();    // DAK
}

#ifdef Use_mt_mode

//Calculated multy
// this function is run in another thread!
// don't call any GUI (Qt or VisorImp) functions!!!
// exceptions should be kept inside the function either

void TProfil::CalcEqstat( bool prg)
{
    TSysEq* STat = (TSysEq*)(&aMod[RT_SYSEQ]);
    STat->ods_link(0);

    calcFinished = false;
    status = 0;

    syst->SyTest();
    if( !syst->BccCalculated() )
        Error( "System", "Please, specify bulk composition of the system!");

    gstring keyp = rt[RT_SYSEQ].UnpackKey();
    PMtest( keyp.c_str() );
    //MultiCalc( keyp.c_str() );
    pVisorImp->OpenProgress();
    MultiCalcInit( keyp.c_str() );
    if( AutoInitialApprox() == false )
        MultiCalcIterations();
    else //Show results   //if( wn[W_EQCALC].status )
        aMod[MD_EQCALC].ModUpdate("EQ_done  Equilibrium State: computed OK");

    calcFinished = true;
    
//    pVisor->Update();
//    pVisor->CalcFinished();
    STat->setCalcFlag( true );
    STat->CellChanged();
}


#else

//Calculated multy

void TProfil::CalcEqstat( bool prg )
{
    TSysEq* STat = (TSysEq*)(&aMod[RT_SYSEQ]);
    STat->ods_link(0);
    syst->SyTest();
    if( !syst->BccCalculated() )
        Error( "System", "Please, specify bulk composition of the system!");

    gstring keyp = rt[RT_SYSEQ].UnpackKey();
    PMtest( keyp.c_str() );
    //MultiCalc( keyp.c_str() );
    if( prg )
      pVisor->OpenProgress();
    MultiCalcInit( keyp.c_str() );
    if( AutoInitialApprox() == false )
        MultiCalcIterations();
    else //Show results   //if( wn[W_EQCALC].status )
        aMod[MD_EQCALC].ModUpdate("EQ_done  Equilibrium State: computed OK");

    pVisor->Update();
    pVisor->CalcFinished();
    STat->setCalcFlag( true );
    STat->CellChanged();
}

#endif

//Calculated multy with debug
void TProfil::DebagCalcEqstatInit()
{
    TSysEq* STat = (TSysEq*)(&aMod[RT_SYSEQ]);
    STat->ods_link(0);
    syst->SyTest();
    gstring keyp = rt[RT_SYSEQ].UnpackKey();
    PMtest( keyp.c_str() );
    // stop
    MultiCalcInit( keyp.c_str() );
    // stop
    STat->setCalcFlag( true );
    STat->CellChanged();
    pll=0;
    FXold=0.0;
    bool  stat = AutoInitialApprox();
    if( stat==true )
        vfMessage(window(), GetName(), "No iterations");
    pVisor->OpenProgress(!stat);
}

//Calculated multy with debug
bool TProfil::DebagCalcEqstatStep()
{
    bool ret = (MultiCalcMain( pll, FXold ) == true || !pmp->MK);
    //   pVisor->Update();
    return ret;
    // What do if break; ?
}

//Calculated multy with debug
//void TProfil::DebagCalcEqstatEnd()
//{
// aMod[MD_EQCALC].ModUpdate("EQ_done  EQilibrium STATe: computed by OK");
// change <Step> to <End>
//}



//add new Profile structure
// Save file configuration to Profil structure
void TProfil::InitFN( const char * prfName, const char* prfTemplate )
{

    gstring Path = pVisor->userProfDir();
    Path += prfName;

    // make directory Path (find system function)
    pVisor->makeDBDir(Path.c_str());

   if( prfTemplate == 0 )
    { // creating empty files
      for(uint i=0; i<aMod.GetCount(); i++)
      {
        if( aMod[i].IsSubModule() )
            continue;
        rt[aMod[i].rtNum()].MakeInNewProfile( Path, prfName );
      }
     }
    else // copy records from template profile
    {
       gstring tmpDirPath = pVisor->userProfDir();
       tmpDirPath += prfTemplate;

       TCStringArray aFiles =
          pVisor->readPDBDir(tmpDirPath.c_str(), "*" );

       // copy files to new Prifile
     for (uint ii = 0; ii < aFiles.GetCount(); ii++)
     {
        gstring f_tmp = tmpDirPath;
                f_tmp += "/";
                f_tmp += aFiles[ii];
        gstring f_new = Path;
                f_new += "/";
        f_new += aFiles[ii].replace( prfTemplate, prfName);

       if ( !(gstring( aFiles[ii], 0, aFiles[ii].find("."))
                        ==  db->GetKeywd()))
          pVisor->CopyF( f_new.c_str(), f_tmp.c_str() );
    }

/*   // copy template profile

        gstring cmd;

#ifdef __unix
        cmd = "cp -r ";
        cmd += tmpDirPath;
        cmd += "/ * ";
        cmd += Path;

#else
        cmd = "xcopy -r ";
        cmd += tmpDirPath;
        cmd += "/ *.* ";
        cmd += Path;

#endif

        if (system(cmd.c_str()) != 0)
            throw TFatalError( prfName, "Cannot copy template profile");
*/

    // add files to module list
    for (uint ii = 0; ii < aFiles.GetCount(); ii++)
    {
       if (gstring(aFiles[ii], aFiles[ii].rfind(".") + 1) == "pdb")
        {
            for (uint jj = 0; jj < rt.GetCount(); jj++)
                if (gstring(aFiles[ii], 0, aFiles[ii].find("."))
                        == rt[jj].GetKeywd())
                {
                  gstring f_new = aFiles[ii].replace(
                     prfTemplate, prfName);
                  rt[jj].MakeInNewProfile( Path, prfName, f_new.c_str() );
                }
        }
    }
  }
}

// Rename records SysEq and >
void TProfil::RenameFN( const char * prfName, const char* prfTemplate )
{
    // Rename records in New Profile > SysEq
      for(uint i=RT_SYSEQ; i<aMod.GetCount(); i++)
      {
        if( aMod[i].IsSubModule() )
            continue;
        rt[i].RenameList(prfName, prfTemplate);
      }
}
// Save file configuration to Profil structure
bool TProfil::GetFN( const char * prfName )
{

    TCStringArray aFls;
    TCIntArray aCnt;
    if( ! vfListFiles(window(), prfName, aFls, aCnt ))
      return false;

    memset(mup->Nfl, 0, MAXNUMCHAINS*sizeof(short));
    mup->NfT = 0;
    for(uint i=0; i<aCnt.GetCount(); i++)
    {
        mup->Nfl[i] = aCnt[i];
        mup->NfT += mup->Nfl[i];
    }
    mup->FN = (char (*)[FileNameLen])aObj[ o_mufn].Alloc(
                  aFls.GetCount(), 1, FileNameLen );
    // insert files name to FN
    for(uint j=0; j<aFls.GetCount(); j++)
        strncpy( mup->FN[j], aFls[j].c_str(), FileNameLen);
    return true;
}


// Reopen file configuration on data in Profil structure
void TProfil::SetFN()
{
    unsigned i;
    short j, k=0;
    TCStringArray aFls;
    gstring s;
    for( i=0; i<aMod.GetCount(); i++)
    {
        if( aMod[i].IsSubModule() )
            continue;
        ///

        aFls.Clear();
        for(j=0; j<mup->Nfl[i]; j++)
        {
            s = gstring( mup->FN[k++], 0, FileNameLen);
            aFls.Add(s);
        }
        rt[aMod[i].rtNum()].SetNewOpenFileList( aFls );
    }
}

//------------------ End of m_prfget.cpp --------------------------


