//-------------------------------------------------------------------
// $Id$
//
// Implementation of TVisor class, setup and config functions
//
// Copyright (C) 1996-2001 A.Rysin,S.Dmytriyeva,D.Kulik
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license
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
#include "v_dbfile.h"

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
#include "m_unspace.h"
#include "m_gtdemo.h"
#include "m_dualth.h"


// file names constants

const char *VISOR_INI = "data/vis_cn.ini";
const char *OBJECT_INI = "data/vis_od.ini";
const char *UNITS_INI = "data/units.ini";

const char *VISOBJ_DAT = "visor.data/visobj.dat";
const char *VISOR_DAT = "visor.data/visor.dat";

const char *GEM_CONF = "gemsdbf.conf";
const char *VIS_CONF = "visor.conf";
const char *WIN_CONF = "windows.conf";


//----------------------------------------------------------------
// TVisor  class implementation
//----------------------------------------------------------------


TVisor::TVisor(int c, char *v[]):
        argc(c), argv(v)
{
    ProfileMode = false;
    dbChangeMode = false;
    isElementsProfileMode = true;

#ifdef __unix
#ifdef __APPLE__
	char work_path[PATH_MAX];
	char cur_dir[PATH_MAX];
	
	// let's try to find resources by path of the executable
	getcwd(cur_dir, PATH_MAX);
	strcpy(work_path, argv[0]);
	char* rest = strstr(work_path, "/MacOS/gems");
	if( rest != NULL ) {
		strcpy(rest, "/Resources/");

		if( work_path[0] == '.' && work_path[1] == '/' ) 
			SysGEMDir = gstring(cur_dir) + (work_path+1);
		else
		if( work_path[0] != '/' )
			SysGEMDir = (gstring(cur_dir) + "/") + work_path;
		else 
			SysGEMDir = work_path;
	}
	else {
	// non-standard executable path, search for resources starting with current dir
		SysGEMDir = cur_dir;
		SysGEMDir += "/gems2.app/Contents/Resources/";
	}
        UserGEMDir = getenv("HOME");
	UserGEMDir += "/Library/gems2/";
#else
#ifdef GEMS_RELEASE
	SysGEMDir = "/usr/share/gems2/";
#else
        SysGEMDir = getenv("HOME");
        SysGEMDir += "/.gems2_shared/";
#endif
	UserGEMDir += getenv("HOME");
	UserGEMDir += "/.gems2/";
#endif //__APPLE__
#else //__unix
    SysGEMDir = "c:/GEMS2test/program/";
    UserGEMDir = "c:/GEMS2test/";
#endif //__unix


    char* env_s = getenv("GEMS_SYSDIR");
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


    DefDBDir = "DB.default/";
    DefProfDir = "project/";
    UserProfDir = "projects/";
    ImgDir = "img/";

    RemoteDocURL = "http://les.web.psi.ch/Software/GEMS-PSI/doc/html/";
    LocalDoc = true;


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

    LocalDocDir = SysGEMDir + "doc/html/";

}

TVisor::~TVisor()
{}


void
TVisor::Setup()
{
    bool option_d = false;
    bool option_f = false;
    bool default_settings = false;
#ifdef __APPLE__
    bool default_config = true;
    pVisorImp->setConfigAutosave( true );
#else
    bool default_config = false;
#endif

    for (int ii = 1; ii < argc; ii++)	//Sveta 16/06/1999
    {
        if (strcmp(argv[ii], "-d") == 0
                || strcmp(argv[ii], "--from-ini-files") == 0 )
        {   
           option_d = true;
           pVisorImp->setConfigAutosave( true );
        }
        else
	if (strcmp(argv[ii], "-f") == 0
                || strcmp(argv[ii], "--allow-db-change") == 0 ) 
	{
            option_f = true;
	}
        else 
	if (strcmp(argv[ii], "-c") == 0
                || strcmp(argv[ii], "--with-default-config") == 0 ) 
	{
            default_config = true;
            pVisorImp->setConfigAutosave( true );
        }
        else 
	if (strcmp(argv[ii], "-v") == 0
                || strcmp(argv[ii], "--with-default-settings") == 0 ) 
	{
            default_settings = true;
            pVisorImp->setConfigAutosave( true );
        }
    }

    // check home dir
    gstring dir = userGEMDir();
    QDir userGEM(dir.c_str());

    bool firstTimeStart = !userGEM.exists();

    if (firstTimeStart)
    {
        default_config = true;
        default_settings = true;
        pVisorImp->setConfigAutosave( true );

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

        cout << "Creating GEMS user directory " << userProfDir().c_str() << endl;
#else
        cmd = "xcopy -r ";
        cmd += sysProfDir();
        cmd += DefProfDir;
        cmd += "*.* ";
        cmd += userProfDir();

        ofstream fdbg("out.log");
        fdbg << "Creating GEMS user directory " << cmd.c_str() << endl;
#endif

        if (system(cmd.c_str()) != 0)
            throw TFatalError("GEMS Init", "Cannot copy default project to user directory");
    }


    if (option_d)
        load();
    else
        fromDAT(default_config, default_settings);

    // Sveta permission to change data in special DB files
    if (option_f)
        dbChangeMode = true;
}


const int lnWINSIG = 2;
const char SigBEG[lnWINSIG + 1] = "Vs";
const char SigEND[lnWINSIG + 1] = "sX";

void
TVisor::load()
{
    gstring fname = sysGEMDir() + OBJECT_INI;

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


const char *vSigERROR_VISOR = "Error in visor data file visor.dat - wrong markers";
const char *vSigERROR_VISOBJ = "Error in visor data file visobj.dat - wrong markers";
const char *vSigTITLE = "Configurator";

void
TVisor::fromDAT(bool default_config /*option_c*/, bool default_settings/*option_v*/)
{
    gstring fname = sysGEMDir() + VISOBJ_DAT;

    // objects' DAT
    ifstream obj_dat(fname.c_str(), ios::binary | ios::in);

    if ( !obj_dat.good() ) {
	gstring message = "Can't open ";
	message += fname;
        throw TError(vSigTITLE, message);
    }

    char sg[2];
    obj_dat.read(sg, sizeof sg);
    if (sg[0] != SigBEG[0] || sg[1] != SigBEG[1])
        throw TError(vSigTITLE, vSigERROR_VISOBJ);

    aObj.fromDAT(obj_dat);

    obj_dat.read(sg, sizeof sg);
    if (sg[0] != SigEND[0] || sg[1] != SigEND[1])
        throw TError(vSigTITLE, vSigERROR_VISOBJ);

    obj_dat.close();


    if( default_config )
    {
        defaultCFG();
        toModCFG();
    }
    else
    {
        fromModCFG();
    }

    initModules();

    // loading static info for visor (DAT files)
    fname = sysGEMDir() + VISOR_DAT;

    ifstream visor_dat(fname.c_str(), ios::binary | ios::in);

    if ( !obj_dat.good() ) {
	gstring message = "Can't open ";
	message += fname;
        throw TError(vSigTITLE, message);
    }

    visor_dat.read(sg, sizeof sg);
    if (sg[0] != SigBEG[0] || sg[1] != SigBEG[1])
        throw TError(vSigTITLE, vSigERROR_VISOR);

    int n;
    visor_dat.read((char *) &n, sizeof n);
    // if( n!= aMod.GetCount() )
    //   throw TError();

    aWinInfo.Clear(n + 4);
    for (int ii = 0; ii < n; ii++)
        aWinInfo.Add(new CWinInfo(aMod[ii], visor_dat));

    visor_dat.read(sg, sizeof sg);
    if (sg[0] != SigEND[0] || sg[1] != SigEND[1])
        throw TError(vSigTITLE, vSigERROR_VISOR);

    // Units' part to load
    aUnits.fromDAT(visor_dat);

    if( default_settings )
        toWinCFG();
    else
        fromWinCFG();
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

//    if( pVisorImp->getCellFont() != pVisorImp->getDefaultFont() )
	f_win_ini << "general_font_string\t=\t\"" << pVisorImp->getCellFont().toString() << "\"" << endl;
//    if( pVisorImp->getAxisLabelFont() != pVisorImp->getDefaultFont() )
	f_win_ini << "axis_label_font_string\t=\t\"" << pVisorImp->getAxisLabelFont().toString() << "\"" << endl;

    int win_num = aWinInfo.GetCount();
    f_win_ini << "number_of_windows\t=\t" << win_num << endl;
    f_win_ini << "config_autosave\t=\t" << pVisorImp->getConfigAutosave() << endl;

    f_win_ini << "local_doc_dir\t=\t" << LocalDocDir.c_str() << endl;
    f_win_ini << "remote_doc_url\t=\t" << RemoteDocURL.c_str() << endl;
    f_win_ini << "local_doc\t=\t" << LocalDoc << endl;
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
            else if( name == "general_font_string" ) {
                    visor_conf.getcStr(name);
                    name.strip();
		    QFont cellFont;
                    cellFont.fromString( name.c_str() );
		    pVisorImp->setCellFont(cellFont);
                }
        	else if( name == "axis_label_font_string" ) {
                    visor_conf.getcStr(name);
                    name.strip();
		    QFont axisLabelFont;
                    axisLabelFont.fromString( name.c_str() );
		    pVisorImp->setAxisLabelFont(axisLabelFont);
                }
                else if( name == "number_of_windows" ) {
                    win_num = visor_conf.getcInt();
                }
            	else if( name == "config_autosave" ) {
				pVisorImp->setConfigAutosave(visor_conf.getcInt());
			}
            	else if( name == "local_doc_dir" ) {
				gstring gstr;
				visor_conf.getcStr(gstr);
				setLocalDocDir(gstr);
			}
            	else if( name == "remote_doc_url" ) {
				gstring gstr;
				visor_conf.getcStr(gstr);
				setRemoteDocURL(gstr);
			}
            	else if( name == "local_doc" ) {
				setLocalDoc(visor_conf.getcInt());
			}
			
        name = visor_conf.getNext();
    }

    // Window-specific settings
    gstring fwin_ini_name = /*userGEMDir*/userProfDir() + WIN_CONF;
    ifstream f_win_ini(fwin_ini_name.c_str() );
    ErrorIf(!f_win_ini.good(), "GEMS Init",
            "Error reading configurator file (windows.conf)" );

 //   vstr name_str(256);
    // error Sveta 13/06/2001 reads only #
 //   f_win_ini >> name_str.p; // Don't compile in BCB5 without .p

    for (uint ii = 0; ii < aWinInfo.GetCount(); ii++)
        aWinInfo[ii].fromWinCFG(f_win_ini);
}

//  Reorganized by KD on E.Curti' comment 04.04.01
bool TVisor::CanClose()
{
    if( pVisorImp->getConfigAutosave() ) {
        Exit();
        return true;
    }

    switch (vfQuestion3(pVisorImp,
    		"Exit GEM-Selektor", "Save configuration?",
		"Do not save", "Save"))
    {
    case VF3_1:
        return true;
    case VF3_2:
        Exit();
        return true;
    case VF3_3:
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
        addModule(TUnSpace::pm = new TUnSpace(RT_UNSPACE));
        addModule(TGtDemo::pm = new TGtDemo(RT_GTDEMO));
        addModule(TDualTh::pm = new TDualTh(RT_DUALTH));

        TProfil::pm->InitSubModules();
    }
    catch(TError & xcpt)
    {
        throw TFatalError(xcpt);
    }
}

extern const char * dfAqKeyD ;
extern const char * dfAqKeyH ;
extern const char * dfAqKey3 ;
extern const char * dfAqKey2 ;
extern const char * dfAqKey1 ;
extern const char * dfAqKeyS ;
extern const char * dfGasKey ;
extern const char * dfFluKey ;

// Exit of program, save cfg
void
TVisor::Exit()
{
    try
    {
        // delete auto-generated aq and gas phases if still in database
        int nRec = rt[RT_PHASE].Find( dfAqKeyD );
        if( nRec >= 0 )
            rt[RT_PHASE].Del(nRec);
        nRec = rt[RT_PHASE].Find( dfAqKeyH );
        if( nRec >= 0 )
            rt[RT_PHASE].Del(nRec);
        nRec = rt[RT_PHASE].Find( dfAqKey3 );
        if( nRec >= 0 )
            rt[RT_PHASE].Del(nRec);
        nRec = rt[RT_PHASE].Find( dfAqKey2 );
        if( nRec >= 0 )
            rt[RT_PHASE].Del(nRec);
        nRec = rt[RT_PHASE].Find( dfAqKey1 );
        if( nRec >= 0 )
            rt[RT_PHASE].Del(nRec);
        nRec = rt[RT_PHASE].Find( dfGasKey );
        if( nRec >= 0 )
            rt[RT_PHASE].Del(nRec);
        nRec = rt[RT_PHASE].Find( dfFluKey );
        if( nRec >= 0 )
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
                         o_pestr, 26+14/*11/03/02*/, 0, 10, proces_rkfrm));

    // RT_UNSPACE default
    unsigned char unspace_rkfrm[10] = { MAXMUNAME, MAXTDPCODE, MAXSYSNAME,
                 MAXTIME, MAXPTN, MAXPTN, MAXPTN, MAXNV, MAXPENAME, MAXPECODE
                                       };
    rt.Add(new TDataBase(rt.GetCount(), "unspac", true, true,
                         o_unname, 58, 0, 10, unspace_rkfrm));


    // RT_GTDEMO default
    unsigned char gtdemo_rkfrm[5] =
        { MAXMUNAME, MAXDATATYPE, MAXGTDCODE, MAXNV, MAXGDGROUP };
    rt.Add(new TDataBase(rt.GetCount(), "gtdemo", true, true,
                         o_gdps, 27, 0, 5, gtdemo_rkfrm));

    // RT_DUALTH default
    unsigned char dualth_rkfrm[10] = { MAXMUNAME, MAXTDPCODE, MAXSYSNAME,
       MAXTIME, MAXPTN, MAXPTN, MAXPTN, MAXNV, MAXPENAME, MAXPECODE
                                  };
    rt.Add(new TDataBase(rt.GetCount(), "dualth", true, true,
                         o_dtname, 46, 0, 10, dualth_rkfrm));

    // read default database
    TCStringArray aDBFiles = readPDBDir(pVisor->sysDBDir().c_str(), "*.pdb");
    //  readPDBDir(pVisor->userProfDir().c_str());

    for (uint jj = 0; jj < rt.GetCount(); jj++)
    {
        int cnt = 0;
         for (uint ii = 0; ii < aDBFiles.GetCount(); ii++)
        { gstring flnm = gstring(aDBFiles[ii], 0, aDBFiles[ii].find("."));
            if ( flnm == rt[jj].GetKeywd() ||
                 ( jj == RT_UNSPACE && flnm == "probe" ) ||   //set up old name
                 ( jj == RT_DUALTH && flnm == "duterm" ) )   //set up old name
            {
                gstring path = pVisor->sysDBDir();
                path += aDBFiles[ii];
                rt[jj].AddFile(path);
                cnt++;
            }
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
          { gstring flnm = gstring(aDBFiles[kk], 0, aDBFiles[kk].find("."));
            if ( flnm == rt[jj].GetKeywd() ||
                ( jj == RT_UNSPACE && flnm == "probe" ) ||   //set up old name
                ( jj == RT_DUALTH && flnm == "duterm" ) )   //set up old name
                {
                    gstring path(dir);
                    path += "/";
                    path += aDBFiles[kk];
                    rt[jj].AddFile(path);
                }
          }
        }
    }

}

TCStringArray readDirs(const char *dir)
{
    TCStringArray aFiles;

    QDir thisDir(dir);
    if (!thisDir.isReadable())
        throw TFatalError("GEMS Init", gstring(dir) + ": GEMS DB directory is not readable");

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
TVisor::ProcessProgress( QWidget* parent )
{
    pVisorImp->ProcessProgress( parent );
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
