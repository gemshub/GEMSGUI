//-------------------------------------------------------------------
// Id: gems/vizor/visor.cpp  version 2.0.0    2001
//
// Implementation of TVisor class, setup and config functions
//
// Created : 970207    Modified: 010908
//
// Copyright (C) 1996-2001 A.Rysin,S.Dmytriyeva,D.Kulik
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license #1435515
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://les.web.psi.ch/Software/GEMS-PSI/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------
//
#include <qapplication.h>
#include <iostream>
using namespace std;

#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <stdlib.h>

#include "config.h"
#include "service.h"
#include "visor.h"
#include "units.h"
#include "visor_w.h"
#include "page_win_w.h"

#include "m_const.h"
#include "m_sdata.h"
#include "m_integ.h"
#include "m_param.h"
#include "m_icomp.h"
#include "m_dcomp.h"
#include "m_compos.h"
#include "m_reacdc.h"
#include "m_rtparm.h"
#include "m_phase.h"
#include "m_syseq.h"
#include "m_proces.h"
#include "m_probe.h"
#include "m_gtdemo.h"
#include "m_duterm.h"


// file names constants
const char *VISOR_INI = "data/vis_cn.ini";
const char *OBJECT_INI = "data/vis_od.ini";
const char *UNITS_INI = "data/units.ini";

const char *VISOBJ_DAT = "visor.data/visobj.dat";
const char *VISOR_DAT = "visor.data/visor.dat";

//const char *GEM_CFG = "gems.cfg";
//const char *WIN_CFG = "windows.cfg";

const char *GEM_CONF = "gemsdbf.conf";
const char *VIS_CONF = "visor.conf";
const char *WIN_CONF = "windows.conf";


//----------------------------------------------------------------
// TVisor  class implementation
//----------------------------------------------------------------


TVisor::TVisor(int c, char *v[]):
        argc(c), argv(v)
{
    DefDBDir = "DB.default/";
    DefProfDir = "project/";
    UserProfDir = "projects/";

    DocDir = "doc/html/";
    ImgDir = "img/";

    ProfileMode = false;
    DBChangedMode = false;
    isElementsProfileMode = true;

    char* env_s;

#ifdef __unix
    env_s = getenv("GEMS_DIR");
    if (env_s)
    {
        SysGEMDir = env_s;
        if (SysGEMDir[SysGEMDir.length() - 1] != '/')
            SysGEMDir += '/';
    }
    else
    {
#ifndef GEMS_RELEASE
        SysGEMDir = getenv("HOME");
        SysGEMDir += "/GEMS.sys/";
#else
SysGEMDir = "/usr/lib/gems/";
#endif //GEMS_RELEASE
    }

    env_s = getenv("GEMS_USERDIR");
    if (env_s)
    {
        UserGEMDir = env_s;
        if (UserGEMDir[UserGEMDir.length() - 1] != '/')
            UserGEMDir += '/';
    }
    else
    {
        UserGEMDir = getenv("HOME");
        UserGEMDir += "/GEMS/";
    }


#else // Win32 - to expand (comm.line etc.)
    SysGEMDir = "c:/gems.sys/";
    UserGEMDir = "c:/gems/";

    // If either of two directories is located on other disk(s)
    // or not in the root directory, then in Autoexec.bat,
    // it must be set explicitly, for instance,
    //   SET GEMS_SYSDIR=d:\MYGEMS\gems.sys
    //   SET GEMS_USERDIR=d:\MYGEMS\gemsdata

    env_s = getenv("GEMS_SYSDIR");
    if (env_s)
    {
        SysGEMDir = env_s;
        if (SysGEMDir[SysGEMDir.length() - 1] != '/')
            SysGEMDir += '/';
    }

    env_s = getenv("GEMS_USERDIR");
    if (env_s)
    {
        UserGEMDir = env_s;
        if (UserGEMDir[UserGEMDir.length() - 1] != '/')
            UserGEMDir += '/';
    }

#endif //__unix

    // parsing options -s and -u if given

    int isys = 0;		// index of sysdir option
    int iuser = 0;		// index of userdir option

    for (int ii = 1; ii < argc; ii++)
    {
        if (strcmp(argv[ii], "-s") == 0
                || strcmp(argv[ii], "--system-dir") == 0 )
            isys = ii;
        else if (strcmp(argv[ii], "-u") == 0
                 || strcmp(argv[ii], "--user-dir") == 0 )
            iuser = ii;
    }
    if (isys != 0)
    {
        if (argc <= isys + 1)
            Error("Wrong options", "Wrong argument for option -s");
        SysGEMDir = argv[isys + 1];
        if (SysGEMDir[SysGEMDir.length() - 1] != '/')
            SysGEMDir += '/';
    }
    if (iuser != 0)
    {
        if (argc <= iuser + 1)
            Error("Wrong options", "Wrong argument for option -u");
        UserGEMDir = argv[iuser + 1];
        if (UserGEMDir[UserGEMDir.length() - 1] != '/')
            UserGEMDir += '/';
    }
}

TVisor::~TVisor()
{}


void
TVisor::Setup()
{
    bool option_d = false;
    bool option_f = false;
    bool option_c = false;

    for (int ii = 1; ii < argc; ii++)	//Sveta 16/06/1999
    {
#ifndef GEMS_RELEASE
        if (strcmp(argv[ii], "-d") == 0
                || strcmp(argv[ii], "--from-ini-files") == 0 )
            option_d = true;
        else
#endif
            if (strcmp(argv[ii], "-f") == 0
                    || strcmp(argv[ii], "--allow-db-change") == 0 )
                option_f = true;
            else if (strcmp(argv[ii], "-c") == 0
                     || strcmp(argv[ii], "--with-default-config") == 0 )
                option_c = true;
    }
#ifndef GEMS_RELEASE
    if (option_d)
        load();
    else
#endif // GEMS_RELEASE
        fromDAT(option_c);

    // Sveta permission to change data in special DB files
    if (option_f)
        DBChangedMode = true;
}


const int lnWINSIG = 2;
const char SigBEG[lnWINSIG + 1] = "Vs";
const char SigEND[lnWINSIG + 1] = "sX";

#ifndef GEMS_RELEASE
void
TVisor::load()
{
  gstring fname = sysGEMDir();
          fname += OBJECT_INI;

//    TConfig cnf1(OBJECT_INI, ' ');
    aObj.load(fname.c_str());
    defaultCFG();
    initModules();
    fname = sysGEMDir();
    fname += UNITS_INI;
    aUnits.load(fname.c_str());

    fname = sysGEMDir();
    fname += VISOR_INI;

#ifdef __unix
    TConfig cnf(fname.c_str(), ' ');
#else
    TConfig& cnf = *new TConfig(fname.c_str(), ' ');
#endif

    for (uint ii = 0; ii < aMod.GetCount(); ii++)
        aWinInfo.Add(new CWinInfo(aMod[ii], cnf));

    toDAT();

    toModCFG();
    toWinCFG();
}

void
TVisor::toDAT()
{
    gstring fname = sysGEMDir();
    fname += VISOBJ_DAT;

    ofstream obj_dat(fname.c_str(), ios::binary | ios::out);
    // begin signature
    obj_dat << SigBEG;
    aObj.toDAT(obj_dat);
    // end signature
    obj_dat << SigEND;
    obj_dat.close();

    fname = sysGEMDir();
    fname += VISOR_DAT;

    ofstream visor_dat(fname.c_str(), ios::binary | ios::out);
    // begin signature
    visor_dat << SigBEG;

    int n = aMod.GetCount();
    visor_dat.write((char *) &n, sizeof n);
    for (int ii = 0; ii < n; ii++)
        aWinInfo[ii].toDAT(visor_dat);

    // end signature
    visor_dat << SigEND;

    // Units' part to save
    aUnits.toDAT(visor_dat);
}
#endif // GEMS_RELEASE

const char *vSigERROR = "Error in visor data file (visor.dat)";
const char *vSigERROR1 = "Error in visor data file (visobj.dat)";
const char *vSigTITLE = "Configurator";

void
TVisor::fromDAT(bool option_c)
{
    gstring fname = sysGEMDir();
    fname += VISOBJ_DAT;

    // objects' DAT
    ifstream obj_dat(fname.c_str(), ios::binary | ios::in);
    char sg[2];
    obj_dat.read(sg, sizeof sg);
    if (sg[0] != SigBEG[0] || sg[1] != SigBEG[1])
        throw TError(vSigERROR1, vSigTITLE);

    aObj.fromDAT(obj_dat);

    obj_dat.read(sg, sizeof sg);
    if (sg[0] != SigEND[0] || sg[1] != SigEND[1])
        throw TError(vSigERROR1, vSigTITLE);

    obj_dat.close();

    // add modules
    gstring dir = userGEMDir();
    QDir userGEM(dir.c_str());

    bool firstTimeStart = !userGEM.exists();

    if (firstTimeStart)
    {
        // make home GEM directories
        if( !userGEM.mkdir(userGEMDir().c_str(), true)
                || !userGEM.mkdir(userProfDir().c_str(), true) )
            throw TFatalError("GEMS Init", "Cannot create user GEMS directory");

        // copy default project

        gstring cmd;

#ifdef __unix
        cmd = "cp -r ";
        cmd += sysProfDir();
        cmd += "* ";
        cmd += userProfDir();

        cout << "Creating GEMS user directory..." << endl;
        //      cout << cmd << endl;

#else
        cmd = "xcopy -r ";
        cmd += sysProfDir();
        cmd += DefProfDir;
        cmd += "*.* ";
        cmd += userProfDir();

        ofstream fdbg("out.log");
        fdbg << "Creating GEMS user directory..." << endl;
        fdbg << cmd.c_str() << endl;
#endif

        if (system(cmd.c_str()) != 0)
            throw TFatalError("GEMS Init", "Cannot copy default project to user directory");
    }

    if( firstTimeStart || option_c )
    {
        defaultCFG();
        // save initial state of databases to gems.cfg
        toModCFG();
    }
    else
    {
        fromModCFG();
    }

    initModules();

    // loading static info for visor (DAT files)
    fname = sysGEMDir();
    fname += VISOR_DAT;

    ifstream visor_dat(fname.c_str(), ios::binary | ios::in);

    visor_dat.read(sg, sizeof sg);
    if (sg[0] != SigBEG[0] || sg[1] != SigBEG[1])
        throw TError(vSigERROR, vSigTITLE);

    int n;
    visor_dat.read((char *) &n, sizeof n);
    // if( n!= aMod.GetCount() )
    //   throw TError();

    aWinInfo.Clear(n + 4);
    for (int ii = 0; ii < n; ii++)
        aWinInfo.Add(new CWinInfo(aMod[ii], visor_dat));

    visor_dat.read(sg, sizeof sg);
    if (sg[0] != SigEND[0] || sg[1] != SigEND[1])
        throw TError(vSigERROR, vSigTITLE);

    // Units' part to load
    aUnits.fromDAT(visor_dat);


    if( firstTimeStart || option_c )
    {
        // save initial settings of windows to windows.cfg
        toWinCFG();
    }
    else
    {
        fromWinCFG();
    }
}

void
TVisor::toModCFG()
{
    gstring fname = userProfDir();//userGEMDir();
    fname += GEM_CONF;

    fstream f_gems(fname.c_str(), ios::out /*| ios::binary*/);
    ErrorIf(!f_gems.good(), "GEMS Init",
            "Error writing configuration file (gemsdbf.conf)");
    rt.toCFG(f_gems);
#ifdef __unix
    cerr << "gems.cfg saved " << endl;
#endif
}

void
TVisor::fromModCFG()
{
    gstring fname = userProfDir();//userGEMDir();
    fname += GEM_CONF;

    fstream f_gems(fname.c_str(), ios::in | ios::binary );
    ErrorIf(!f_gems.good(), "GEMS Init",
            "Error reading configuration file (gemsdbf.conf)");
    rt.fromCFG(f_gems);
#ifdef __unix
    cerr << "gems.cfg read " << endl;
#endif    
}

void
TVisor::toWinCFG()
{
    gstring fname_ini = /*userGEMDir*/userProfDir() + VIS_CONF;

    fstream f_win_ini(fname_ini.c_str(), ios::out );
    ErrorIf(!f_win_ini.good(), "GEMS Init",
            "Error writing configurator file (visor.conf)");

    int n = pVisorImp->getDoubleDigits();
    f_win_ini << "double_precision\t=\t" << n << endl;
    f_win_ini << "update_interval\t=\t" << pVisorImp->updateInterval() << endl;

    QFont font = pVisorImp->getCellFont();

#ifdef __unix
    f_win_ini << "general_font_raw\t=\t" << font.rawName() << endl;
#endif
    // these are exported for both OS (for Unix->Win export :-)
    f_win_ini << "general_font_family\t=\t" << font.family() << endl;
    f_win_ini << "general_font_points\t=\t" << font.pointSize() << endl;
    f_win_ini << "general_font_weight\t=\t" << font.weight() << endl;

    int win_num = aWinInfo.GetCount();
    f_win_ini << "number_of_windows\t=\t" << win_num << endl;
    f_win_ini << "config_autosave\t=\t" << pVisorImp->getConfigAutosave() << endl;
    f_win_ini.close();

    // Window-specific settings
    fname_ini = /*userGEMDir*/userProfDir() + WIN_CONF;
    f_win_ini.open(fname_ini.c_str(), ios::out );
    ErrorIf(!f_win_ini.good(), "GEMS Init",
            "Error writing configurator file (windows.conf)" );

//    f_win_ini << "# Format of the file and the order should be exactly the same" << endl;
    for (int ii = 0; ii < win_num; ii++)
        aWinInfo[ii].toWinCFG(f_win_ini);

    f_win_ini.close();
}

void
TVisor::fromWinCFG()
{
    gstring fname_ini = /*userGEMDir*/userProfDir() + VIS_CONF;

#ifdef __unix
    TConfig visor_conf( fname_ini.c_str() );
#else
    TConfig& visor_conf = *new TConfig( fname_ini.c_str() );
#endif

    int win_num = 0;
    QFont general_font = QFont::defaultFont();
    gstring name = visor_conf.getFirst();

    while ( !name.empty() )
    {
        if( name == "double_precision" ) {
            pVisorImp->setDoubleDigits( visor_conf.getcInt() );
        }
        else
            if( name == "update_interval" ) {
                pVisorImp->setUpdateInterval( visor_conf.getcInt() );
            }
            else
#ifdef __unix
                if( name == "general_font_raw" ) {
                    QFont general_font;
                    visor_conf.getcStr(name);
                    name.strip();
                    general_font.setRawName( name.c_str() );
                    pVisorImp->setCellFont( general_font );
                }
#else
                if( name == "general_font_family" ) {
                    visor_conf.getcStr(name);
                    name.strip();
                    general_font.setFamily( name.c_str() );
                }
                else if( name == "general_font_size" ) {
                    general_font.setPointSize( visor_conf.getcInt() );
                }
                else if( name == "general_font_weight" ) {
                    general_font.setWeight( visor_conf.getcInt() );
                }
#endif
                else if( name == "number_of_windows" ) {
                    win_num = visor_conf.getcInt();
                }
            	else if( name == "config_autosave" ) {
				pVisorImp->setConfigAutosave(visor_conf.getcInt());
			}
			
        name = visor_conf.getNext();
    }

    pVisorImp->setCellFont( general_font );

    // Window-specific settings
    gstring fwin_ini_name = /*userGEMDir*/userProfDir() + WIN_CONF;
    ifstream f_win_ini(fwin_ini_name.c_str() );
    ErrorIf(!f_win_ini.good(), "GEMS Init",
            "Error reading configurator file (windows.conf)" );

 //   vstr name_str(256);
    // error Sveta 13/06/2001 reads only #
 //   f_win_ini >> name_str.p; // Don't compile in BCB5 without .p

    for (int ii = 0; ii < aWinInfo.GetCount(); ii++)
        aWinInfo[ii].fromWinCFG(f_win_ini);
}

//  Reorganized by KD on E.Curti' comment 04.04.01
bool TVisor::CanClose()
{
    if( pVisorImp->getConfigAutosave() ) {
        Exit();
        return true;
    }

    switch (vfQuestYesNoCancel(pVisorImp,
        "GEM-Selektor Exit", "Normal, without saving configuration?"))
    {
    case VF_YES:
        return true;
    case VF_NO:
        Exit();
        return true;
    case VF_CANCEL:
        ;
    }
    return false;
}

void
TVisor::Update(bool force)
{
    pVisorImp->Update(force);
}

void
TVisor::CalcFinished()
{
    pVisorImp->CalcFinished();
}

void
TVisor::addModule(TCModule * pm, bool selectFiles)
{
    aMod.Add(pm);
    pm->ods_link();
    //  pm->dyn_set();

    TCIntArray arr;
    if (selectFiles)
        arr = pm->SelectFileList(openf | closef);
    rt[pm->rtNum()].Open(selectFiles, UPDATE_DBV, arr);
    rt[pm->rtNum()].SetKey(ALLKEY);
}

//Init work structures
void
TVisor::initModules()
{
    try
    {
        addModule(TSData::pm = new TSData(RT_SDATA));
        addModule(TConst::pm = new TConst(RT_CONST));
        addModule(TInteg::pm = new TInteg(RT_INTEG));
        addModule(TProfil::pm = new TProfil(RT_PARAM));
        addModule(TIComp::pm = new TIComp(RT_ICOMP));
        addModule(TDComp::pm = new TDComp(RT_DCOMP));
        addModule(TCompos::pm = new TCompos(RT_COMPOS));
        addModule(TReacDC::pm = new TReacDC(RT_REACDC));
        addModule(TRTParm::pm = new TRTParm(RT_RTPARM)); // see below
        addModule(TPhase::pm = new TPhase(RT_PHASE));	// see below
        addModule(TSysEq::pm = new TSysEq(RT_SYSEQ));
        addModule(TProcess::pm = new TProcess(RT_PROCES));
        addModule(TProbe::pm = new TProbe(RT_PROBE));
        addModule(TGtDemo::pm = new TGtDemo(RT_GTDEMO));
        addModule(TDuterm::pm = new TDuterm(RT_DUTERM));

        TProfil::pm->InitSubModules();
    }
    catch(TError & xcpt)
    {
        throw TFatalError(xcpt);
    }
}

// Exit of program, save cfg
void
TVisor::Exit()
{
    try
    {
        // delete default aq and gas phases if they exist
        int nRec = rt[RT_PHASE].Find(defaultAqKey);
        if (nRec >= 0)
            rt[RT_PHASE].Del(nRec);
        nRec = rt[RT_PHASE].Find(defaultGasKey);
        if (nRec >= 0)
            rt[RT_PHASE].Del(nRec);

        toModCFG();
        toWinCFG();
    }
    catch(TError & xcpt)
    {
        throw TFatalError(xcpt);
    }
}

//TCStringArray readPDBDir(const char *dir);
TCStringArray readDirs(const char *dir);

// default configuration
void
TVisor::defaultCFG()
{
    rt.Init();

    // RT_INTEG default
    unsigned char integ_rkfrm[1] = { 24 };
    rt.Add(new TDataBase(rt.GetCount(), "integ", true, true,
                         o_ipstr, 6, 0, 1, integ_rkfrm));

    // RT_PROFIL default
    unsigned char param_rkfrm[2] = { MAXMUNAME, MAXMUGROUP };
    rt.Add(new TDataBase(rt.GetCount(), "projec", true, true,
                         o_spppar, 13, 0, 2, param_rkfrm));

    // RT_ICOMP default
    unsigned char icomp_rkfrm[3] = { MAXICNAME, MAXSYMB, MAXICGROUP };
    rt.Add(new TDataBase(rt.GetCount(), "icomp", false, true,
                         o_icsst, 6, 0, 3, icomp_rkfrm));
    // RT_DCOMP default
    unsigned char dcomp_rkfrm[4] = { MAXSYMB, MAXDRGROUP, MAXDCNAME, MAXSYMB };
    rt.Add(new TDataBase(rt.GetCount(), "dcomp", false, true,
                         o_dcstr, 20, 0, 4, dcomp_rkfrm));

    // RT_COMPOS default
    unsigned char compos_rkfrm[3] = { MAXCMPNAME, MAXSYMB, MAXCMPGROUP };
    rt.Add(new TDataBase(rt.GetCount(), "compos", false, true,
                         o_bcpcc, 20, 0, 3, compos_rkfrm));

    // RT_REACDC default
    unsigned char reacdc_rkfrm[4] = { MAXSYMB, MAXDRGROUP, MAXDCNAME, MAXSYMB };
    rt.Add(new TDataBase(rt.GetCount(), "reacdc", false, true,
                         o_restr, 20, 0, 4, reacdc_rkfrm));

    // RT_RTPARM default
    unsigned char rtparm_rkfrm[6] =
        { MAXSYMB, MAXDRGROUP, MAXDCNAME, MAXSYMB, MAXNV, MAXRTNAME };
    rt.Add(new TDataBase(rt.GetCount(), "rtparm", true, true,
                         o_rtname, 27, 0, 6, rtparm_rkfrm));

    // RT_PHASE default
    unsigned char phase_rkfrm[5] =
        { MAXSYMB, MAXPHSYMB, MAXPHNAME, MAXSYMB, MAXPHGROUP };
    rt.Add(new TDataBase(rt.GetCount(), "phase", true, true,
                         o_phstr, 22, 0, 5, phase_rkfrm));

    // RT_SYSEQ default
    unsigned char syseq_rkfrm[8] = { MAXMUNAME, MAXTDPCODE, MAXSYSNAME,
                                     MAXTIME, MAXPTN, MAXPTN, MAXPTN, MAXNV
                                   };
    rt.Add(new TDataBase(rt.GetCount(), "syseq", false, true,
                         o_ssphst, 71, 0, 8, syseq_rkfrm));

    // RT_PROCES default
    unsigned char proces_rkfrm[10] = { MAXMUNAME, MAXTDPCODE, MAXSYSNAME,
                                       MAXTIME, MAXPTN, MAXPTN, MAXPTN, MAXNV, MAXPENAME, MAXPECODE
                                     };
    rt.Add(new TDataBase(rt.GetCount(), "proces", true, true,
                         o_pestr, 26, 0, 10, proces_rkfrm));

    // RT_PROBE default
    unsigned char probe_rkfrm[9] =
        { MAXMUNAME, MAXTDPCODE, MAXSYSNAME, MAXTIME, MAXPTN,
          MAXPTN, MAXPTN, MAXNV, MAXNV
        };
    rt.Add(new TDataBase(rt.GetCount(), "probe", true, true,
                         o_prname, 35, 0, 9, probe_rkfrm));

    // RT_GTDEMO default
    unsigned char gtdemo_rkfrm[5] =
        { MAXMUNAME, MAXDATATYPE, MAXGTDCODE, MAXNV, MAXGDGROUP };
    rt.Add(new TDataBase(rt.GetCount(), "gtdemo", true, true,
                         o_gdps, 27, 0, 5, gtdemo_rkfrm));
    // RT_DUTERM default
    unsigned char duterm_rkfrm[9] =
        { MAXMUNAME, MAXTDPCODE, MAXSYSNAME, MAXTIME, MAXPTN,
          MAXPTN, MAXPTN, MAXNV, MAXNV
        };
    rt.Add(new TDataBase(rt.GetCount(), "duterm", true, true,
                         o_utpvs, 45, 0, 9, duterm_rkfrm));

    // read default database
    TCStringArray aDBFiles = readPDBDir(pVisor->sysDBDir().c_str(), "*.pdb");
    //  readPDBDir(pVisor->userProfDir().c_str());

    for (uint jj = 0; jj < rt.GetCount(); jj++)
    {
        int cnt = 0;
        for (uint ii = 0; ii < aDBFiles.GetCount(); ii++)
            if (gstring(aDBFiles[ii], 0, aDBFiles[ii].find("."))
                    == rt[jj].GetKeywd())
            {
                gstring path = pVisor->sysDBDir();
                path += aDBFiles[ii];
                rt[jj].AddFile(path);
                cnt++;
            }
        rt[jj].specialFilesNum = cnt;
    }

    // reading project dirs
    TCStringArray aDBDirs = readDirs(pVisor->userProfDir().c_str());
    for (uint ii = 0; ii < aDBDirs.GetCount(); ii++)
    {
        gstring dir(pVisor->userProfDir());
        dir += aDBDirs[ii];
        aDBFiles = readPDBDir(dir.c_str(), "*.pdb");

        for (uint jj = 0; jj < rt.GetCount(); jj++)
        {
            for (uint kk = 0; kk < aDBFiles.GetCount(); kk++)
                if (gstring(aDBFiles[kk], 0, aDBFiles[kk].find("."))
                        == rt[jj].GetKeywd())
                {
                    gstring path(dir);
                    path += "/";
                    path += aDBFiles[kk];
                    rt[jj].AddFile(path);
                }
        }
    }

}

TCStringArray readDirs(const char *dir)
{
    TCStringArray aFiles;

    QDir thisDir(dir);
    if (!thisDir.isReadable())
    {
#ifdef __unix
        cout << "Error :" << dir << endl;
#endif        
        throw TFatalError(/*"GEMS Init"*/dir, "GEMS DB directory is not readable");
    }

    thisDir.setFilter(QDir::Dirs);
    //    thisDir.setNameFilter("*.pdb");

    const QFileInfoList *files = thisDir.entryInfoList();
    if (!files)
        return aFiles;

    QFileInfoListIterator it(*files);
    QFileInfo *f;
    while ((f = it.current()) != 0)
    {
        ++it;
        //      if ( f->fileName() == "." || f->fileName() == ".." )
        //          contunue;

        if (f->isDir() && f->fileName() != "." && f->fileName() != "..")
        {
            //          cout << "Adding dir: " << f->fileName() << endl;
            aFiles.Add((const char *) f->fileName());
        }
        // else 'special file'
    }


    return aFiles;
}

void
TVisor::deleteDBDir(const char *dir)
{
    TCStringArray aFiles;

    QDir thisDir(dir);
    if (!thisDir.exists())
        return;

    QDir::setCurrent(dir);
    thisDir.setFilter(QDir::Files);
    thisDir.setNameFilter("*");

    const QFileInfoList *files = thisDir.entryInfoList();
    if (!files)
        return;

    QFileInfoListIterator it(*files);
    QFileInfo *f;

    while ((f = it.current()) != 0)
    {
        ++it;
        //      if ( f->fileName() == "." || f->fileName() == ".." )
        //          contunue;

        if (f->isSymLink() || f->isFile())
        {
            //          cout << "Adding file: " << f->fileName() << endl;
            aFiles.Add((const char *) f->fileName());
        }
        // else 'special file'
    }



    QDir::setCurrent(dir);
    // delete files in module list
    for (uint ii = 0; ii < aFiles.GetCount(); ii++)
    {
        if (gstring(aFiles[ii], aFiles[ii].rfind(".") + 1) == "pdb")
        {
            for (uint jj = 0; jj < rt.GetCount(); jj++)
                if (gstring(aFiles[ii], 0, aFiles[ii].find("."))
                        == rt[jj].GetKeywd())
                {
                    gstring path(dir);
                    path += "/";
                    path += aFiles[ii];
                    //cout << path << endl;
                    rt[jj].DelFile(path);
                }
        }
        QFile ff(aFiles[ii].c_str());
        ff.remove();
    }

    thisDir.rmdir(dir, true);
}

// added Sveta 23-07-2001

// copy file
void TVisor::CopyF( const char * fName, const char* fTempl )
{
  QFile ff_tmp(fTempl);
  if( !ff_tmp.open(IO_ReadOnly))
     Error(fTempl, "File copy error" );
  ff_tmp.flush();
  QFile ff_new(fName);
  if( !ff_new.open(IO_WriteOnly))
     Error(fName, "File copy error" );

  char *p = new char[ff_tmp.size()+2];
  ff_tmp.readBlock(p, ff_tmp.size());
  ff_new.writeBlock(p, ff_tmp.size());
  delete[] p;
  ff_tmp.close();
  ff_new.close();
}

void
TVisor::makeDBDir(const char *dir)
{
  // make directory dir (find system function)
    QDir d(dir);
    if ( d.exists() )
    { if( d.count()>2)
      {
         QStringList lst = d.entryList( "*.ndx;*.pdb" );
         if (lst.count()<=0)
           vfMessage(0, dir, "This directory is not empty.");
         else
          Error( dir, "Error creating Modelling Project directory!");
      }
      return;
    }
    if( !d.mkdir( dir ))
        Error( dir, "Error creating Modelling Project directory!");
}

TCStringArray TVisor::readPDBDir(const char *dir, const char *filter )
{
    TCStringArray aFiles;

    QDir thisDir(dir);
    if (!thisDir.isReadable())
    {
#ifdef __unix
        cout << "Error :" << dir << endl;
#endif
        throw TFatalError(/*"GEMS Init"*/dir, "GEMS DB directory is not readable");
    }
    thisDir.setFilter(QDir::Files);
    thisDir.setNameFilter(filter);

    const QFileInfoList *files = thisDir.entryInfoList();
    if (!files)
        return aFiles;

    QFileInfoListIterator it(*files);
    QFileInfo *f;
    while ((f = it.current()) != 0)
    {
        ++it;
        //      if ( f->fileName() == "." || f->fileName() == ".." )
        //          contunue;

        if (f->isSymLink() || f->isFile())
        {
            // cout << "Adding file: " << f->fileName() << endl;
            aFiles.Add((const char *) f->fileName());
        }
        // else 'special file'
    }


    return aFiles;
}

// added Sveta 14/08/2001
void
TVisor::OpenHelp(const char* file, const char* item, QWidget* parent)
{
    pVisorImp->OpenHelp( file );
}

void
TVisor::OpenModule(QWidget* parent, int i, int page, bool viewmode)
{
    pVisorImp->OpenModule( parent, i, page, viewmode);
}

void
TVisor::OpenProgress(bool step)
{
    pVisorImp->OpenProgress( step );
}

void
TVisor::CloseProgress()
{
    pVisorImp->CloseProgress();
}

void
TVisor::Message( QWidget* parent, const char* name,
             const char* msg, int prog, int total)
{
    pVisorImp->Message( parent, name, msg, prog, total);
}

void
TVisor::CloseMessage()
{
    pVisorImp->CloseMessage();
}


TVisor *pVisor;

// ----------- End of  visor.cpp ----------------------------
