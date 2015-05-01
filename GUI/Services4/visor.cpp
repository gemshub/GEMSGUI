//-------------------------------------------------------------------
// $Id: visor.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of TVisor class, setup and config functions
//
// Copyright (C) 1996-2012 A.Rysin,S.Dmytriyeva,D.Kulik
// Uses  string class (C) A.Rysin 1999
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (http://qt.nokia.com)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the terms of GEMS4 Development
// Quality Assurance Licence (GEMS4.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------
//
#include <qapplication.h>
#include <iostream>
//using namespace std;

#include <QDir>
#include <QString>
#include <QFile>
#include <QFileInfo>
#include <cstdlib>

#include "config.h"
#include "service.h"
#include "visor.h"
#include "units.h"
#include "page_w.h"
#include "v_ejdb_file.h"

#include "m_const.h"
#include "m_sdata.h"
#include "m_gem2mt.h"
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
#include "GemsMainWindow.h"

// subfolder and file names constants
const char *VISOR_INI = "data/vis_cn.ini";
const char *OBJECT_INI = "data/vis_od.ini";
const char *UNITS_INI = "data/units.ini";
const char *VISOBJ_DAT = "visor.data/visobj.dat";
const char *VISOR_DAT = "visor.data/visor.dat";
const char *DEFAULT_DB_DIR = "DB.default/";
const char *IMAGES_SRC_DIR = "img/";
const char *HELP_DB_DIR = "help/";
const char *HELP_SRC_DIR = "doc/html/";
const char *RESOURCES_DIR = "/Resources/";
const char *DEFAULT_USER_DIR= "/Library/Gems4/";
#ifdef oldDBusing        // Pointer to class contened the structure of main DB
const char *DEFAULT_PR_DIR= "projects/";
#else
const char *DEFAULT_PR_DIR= "proj_ejdb/";
#endif
const char *GEM_CONF = "gemsdbf.conf";
const char *VIS_CONF = "visor.conf";
const char *WIN_CONF = "windows.conf";

//----------------------------------------------------------------
// TVisor  class implementation
//----------------------------------------------------------------

TVisor::TVisor(int c, char *v[]):
        argc(c), argv(v)
{
    ProfileMode = MDD_DATABASE;
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
        strcpy(rest, RESOURCES_DIR);

		if( work_path[0] == '.' && work_path[1] == '/' )
            SysGEMDir = string(cur_dir) + (work_path+1);
		else
		if( work_path[0] != '/' )
            SysGEMDir = (string(cur_dir) + "/") + work_path;
		else
			SysGEMDir = work_path;
	}
	else {
	// non-standard executable path, search for resources starting with current dir
		SysGEMDir = cur_dir;
                SysGEMDir += "/gems3.app/Contents/Resources/";
	}
        UserGEMDir = getenv("HOME");
        UserGEMDir += DEFAULT_USER_DIR; // "/Library/gems3/";
#else  // Linux - in user's home directory
       // By default: /Resources in the same dir as the exe file;
       //       /Library/gems3/projects on the same level as the /Gems4-app dir.
    QString dirExe = QCoreApplication::applicationDirPath();
    SysGEMDir = dirExe.toUtf8().data();
    SysGEMDir += RESOURCES_DIR;
    QDir dirUp(dirExe);
    if( dirUp.cdUp() )
         dirExe = dirUp.path(); // + QDir::separator();
    LocalDir = dirExe.toUtf8().data();
    UserGEMDir = localDir() + DEFAULT_USER_DIR;
#endif // __unix

#else
      // windows - in any non-system directory on C:/ or D:/
      // By default: /Resources in the same dir as the exe file;
      //       /Library/gems3/projects on the same level as the /Gems4-app dir.
    QString dirExe = QCoreApplication::applicationDirPath();
    SysGEMDir = dirExe.toUtf8().data();
    SysGEMDir += RESOURCES_DIR;
    QDir dirUp(dirExe);
    if( dirUp.cdUp() )
         dirExe = dirUp.path();
    LocalDir = dirExe.toUtf8().data();
    UserGEMDir = localDir() + DEFAULT_USER_DIR;
// cout << SysGEMDir.c_str() << endl;
// cout << UserGEMDir.c_str() << endl;
#endif // win

    DefDBDir = DEFAULT_DB_DIR;
    DefProfDir = DEFAULT_PR_DIR;
    UserProfDir = DEFAULT_PR_DIR;
    ImgDir = IMAGES_SRC_DIR;

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
        if (SysGEMDir[SysGEMDir.length() - 1] == '/')
            SysGEMDir += '\0';
        SysGEMDir += RESOURCES_DIR;
    }
    if (iuser != 0)
    {
        if (argc <= iuser + 1)
            Error("Wrong options", "Wrong argument for option -u");
        UserGEMDir = argv[iuser + 1];
        if (UserGEMDir[UserGEMDir.length() - 1] != '/')
            UserGEMDir += '/';
    }

//    LocalDir = userGEMDir();
    LocalDocDir = SysGEMDir + HELP_DB_DIR;
    //RemoteDocURL = "http://gems.web.psi.ch/doc/html/";
    RemoteHTML = SysGEMDir + HELP_SRC_DIR;
    LocalDoc = true;
    DefaultBuiltinTDB = "psi-nagra";  // PSI-Nagra TDB update TT 03.04.2013
//    DefaultBuiltinTDB = "kernel";   temporary for using old Nagra-PSI (2003) dataset
//    DefaultBuiltinTDB = "psinagra";  // To be used after update to PSI-Nagra 2012
// For debugging the directories
// cout << "Local    : " << LocalDir.c_str() << endl;
// cout << "SysGEM   : " << SysGEMDir.c_str() << endl;
// cout << "UserGEM  : " << UserGEMDir.c_str() << endl;
// cout << "UserProj : " << UserProfDir.c_str() << endl;
// cout << "LocalDoc : " << LocalDocDir.c_str() << endl;
// cout << "LocalHTML: " << RemoteHTML.c_str() << endl;
//
/*
#ifdef __unix
#ifndef GEMS_RELEASE
// added SD oct 2005
     if( LocalDocDir[0] == '.' && LocalDocDir[1] == '/' )
     {
	char cur_dir[PATH_MAX];

	// let's try to find resources by path of the executable
	getcwd(cur_dir, PATH_MAX);
    LocalDocDir = string(cur_dir) + string(LocalDocDir,1);
        RemoteHTML = string(cur_dir) + string(RemoteHTML,1);
     }
#endif
#endif
*/
}

TVisor::~TVisor()
{}

QWidget* TVisor::window()
{
    return pVisorImp;
}

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
    string dir = userGEMDir();
    QDir userGEM(dir.c_str());

    bool firstTimeStart = !userGEM.exists();

    if (firstTimeStart)
    {
        default_config = true;
        default_settings = true;
        pVisorImp->setConfigAutosave( true );

//#ifdef __unix
         // build Library
        string dirUp = string( dir,0, dir.length()-1);
        size_t pos = dirUp.rfind("/");
        if( pos != string::npos )
        {
          dirUp = dirUp.substr(0,pos);
          QDir userGEMUP(dirUp.c_str());
          if(!userGEMUP.exists())
              if( !userGEMUP.mkdir(dirUp.c_str()) )
                  throw TFatalError("GEMS Init", "Cannot create user GEMS directory");
         }
//#endif
// make home GEM directories
// cout << "UserGEM *: " << UserGEMDir.c_str() << endl;
// cout << "UserProj*: " << UserProfDir.c_str() << endl;
        if( !userGEM.mkdir(userGEMDir().c_str()) )
            throw TFatalError("GEMS Init", "Cannot create user GEMS directory");
        if( !userGEM.mkdir(userOldProfDir().c_str()) )
            throw TFatalError("GEMS Init", "Cannot create user GEMS projects directory");
        if( !userGEM.mkdir(userEJDBProfDir().c_str()) )
            throw TFatalError("GEMS Init", "Cannot create user GEMS EJDB projects directory");

        // copy default project
        string cmd;

#ifdef __unix
        cmd = "cp -r ";
        cmd += sysProfDir();
        cmd += "* ";
        cmd += userProfDir();

        cout << "Creating GEMS user directory:  " << userProfDir().c_str() << endl;
#else
string sprdir = sysProfDir();
string uprdir = userProfDir();
QDir sysProjD( sprdir.c_str() );
QDir usrProjD( uprdir.c_str() );
QString sPD = sysProjD.absolutePath();
QString uPD = usrProjD.absolutePath();

        cmd = "xcopy \"";
        cmd += 	qPrintable(	sysProjD.toNativeSeparators( sPD ) );
        cmd += "\" \"";
        cmd += 	qPrintable( usrProjD.toNativeSeparators( uPD ) );
        cmd += "\" /e /y";

        gstring fname = pVisor->userGEMDir();
                fname += "out.log";
        ofstream fdbg(fname.c_str());
        fdbg << "Creating GEMS user directory:  " << cmd.c_str() << endl;
#endif

        if (system(cmd.c_str()) != 0)
            throw TFatalError("GEMS Init", "Cannot copy default projects to user directory");
    }
    else
    {
        // check home dir
        string dirPr = userProfDir();
        QDir userProg(dirPr.c_str());

        if (!userProg.exists())
        {
            default_config = true;
            default_settings = true;
            pVisorImp->setConfigAutosave( true );

            if( !userGEM.mkdir(userProfDir().c_str()) )
                throw TFatalError("GEMS Init", "Cannot create user GEMS EJDB projects directory");

            // copy default project
            string cmd;

    #ifdef __unix
            cmd = "cp -r ";
            cmd += sysProfDir();
            cmd += "* ";
            cmd += userProfDir();

            cout << "Creating GEMS user directory:  " << userProfDir().c_str() << endl;
    #else
    string sprdir = sysProfDir();
    string uprdir = userProfDir();
    QDir sysProjD( sprdir.c_str() );
    QDir usrProjD( uprdir.c_str() );
    QString sPD = sysProjD.absolutePath();
    QString uPD = usrProjD.absolutePath();

            cmd = "xcopy \"";
            cmd += 	qPrintable(	sysProjD.toNativeSeparators( sPD ) );
            cmd += "\" \"";
            cmd += 	qPrintable( usrProjD.toNativeSeparators( uPD ) );
            cmd += "\" /e /y";

            gstring fname = pVisor->userGEMDir();
                    fname += "out.log";
            ofstream fdbg(fname.c_str());
            fdbg << "Creating GEMS user directory:  " << cmd.c_str() << endl;
    #endif

            if (system(cmd.c_str()) != 0)
                throw TFatalError("GEMS Init", "Cannot copy default projects to user directory");
        }
    }
// pVisor->makeDBDir(userEJDBProfDir().c_str(), false );
//pVisor->makeDBDir(userOldProfDir().c_str(), false );

    if (option_d)
        load();
    else
        fromDAT(default_config, default_settings);

    aObj[o_n0w_mps].SetPtr(0);
    aObj[o_n1w_mps].SetPtr(0);
    aObj[o_n0w_vps].SetPtr(0);
    aObj[o_n1w_vps].SetPtr(0);
    aObj[o_n0w_m_t].SetPtr(0);
    aObj[o_n1w_m_t].SetPtr(0);
    aObj[o_n0w_con].SetPtr(0);
    aObj[o_n1w_con].SetPtr(0);
    aObj[o_n0w_mju].SetPtr(0);
    aObj[o_n1w_mju].SetPtr(0);
    aObj[o_n0w_lga].SetPtr(0);
    aObj[o_n1w_lga].SetPtr(0);

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
    string fname = sysGEMDir() + OBJECT_INI;
// cout << "fname: " << fname.c_str() << endl;
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

    for (int ii = 0; ii < aMod.size(); ii++)
     aWinInfo.push_back( new CWinInfo(*aMod[ii], cnf));

    cnf.close(); // close vis_cn.dat file after reading it
    toDAT();
    toModCFG();
    toWinCFG();
}

void
TVisor::toDAT()
{
    string fname = sysGEMDir();
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

    int n = aMod.size();
    visor_dat.write((char *) &n, sizeof n);
    for (int ii = 0; ii < n; ii++)
        aWinInfo[ii]->toDAT(visor_dat);

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
    string fname = sysGEMDir() + VISOBJ_DAT;

    // objects' DAT
    ifstream obj_dat(fname.c_str(), ios::binary | ios::in);

    if ( !obj_dat.good() ) {
    string message = "Can't open ";
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
    string message = "Can't open ";
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

    aWinInfo.clear();
    for (int ii = 0; ii < n; ii++)
        aWinInfo.push_back(  new CWinInfo(*aMod[ii], visor_dat));

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
    string fname = userProfDir();//userGEMDir();
    fname += GEM_CONF;

    fstream f_gems(fname.c_str(), ios::out /*| ios::binary*/);
    ErrorIf(!f_gems.good(), "GEMS Init",
            "Error writing configuration file (gemsdbf.conf)");
    rtOld.toCFG(f_gems);
    rtEJ.toCFG(f_gems);
    flsEJ.toCFG(f_gems);
#ifdef __unix
    cerr << "gems.cfg saved " << endl;
#endif
}

void
TVisor::fromModCFG()
{
    string fname = userProfDir();//userGEMDir();
    fname += GEM_CONF;

    fstream f_gems(fname.c_str(), ios::in | ios::binary );
    ErrorIf(!f_gems.good(), "GEMS Init",
            "Error reading configuration file (gemsdbf.conf)");
    rtOld.fromCFG(f_gems);
    rtEJ.fromCFG(f_gems);
    flsEJ.fromCFG(f_gems);
#ifdef __unix
    cerr << "gems.cfg read " << endl;
#endif
}

void
TVisor::toWinCFG()
{
    string fname_ini = /*userGEMDir*/userProfDir() + VIS_CONF;

    fstream f_win_ini(fname_ini.c_str(), ios::out );
    ErrorIf(!f_win_ini.good(), "GEMS Init",
            "Error writing configurator file (visor.conf)");

    int n = pVisorImp->getDoubleDigits();
    f_win_ini << "double_precision\t=\t" << n << endl;
    f_win_ini << "update_interval\t=\t" << pVisorImp->updateInterval() << endl;

//    if( pVisorImp->getCellFont() != pVisorImp->getDefaultFont() ) //Qt3to4
	f_win_ini << "general_font_string\t=\t\"" << 
               pVisorImp->getCellFont().toString().toUtf8().data() << "\"" << endl;
//    if( pVisorImp->getAxisLabelFont() != pVisorImp->getDefaultFont() ) //Qt3to4
	f_win_ini << "axis_label_font_string\t=\t\"" << 
           pVisorImp->getAxisLabelFont().toString().toUtf8().data() << "\"" << endl;

    int win_num = aWinInfo.size();
    f_win_ini << "number_of_windows\t=\t" << win_num << endl;
    f_win_ini << "config_autosave\t=\t" << pVisorImp->getConfigAutosave() << endl;

    f_win_ini << "local_dir\t=\t\"" << LocalDir.c_str() << "\""  << endl;
    f_win_ini << "local_doc_dir\t=\t\"" << LocalDocDir.c_str() << "\""  << endl;
    f_win_ini << "remote_doc_url\t=\t\"" << RemoteHTML.c_str() << "\"" << endl;
    f_win_ini << "local_doc\t=\t" << LocalDoc << endl;   // obsolete
    f_win_ini << "current_mode\t=\t" << ProfileMode << endl;
    f_win_ini << "default_built_in_TDB\t=\t\"" << DefaultBuiltinTDB.c_str() << "\"" << endl;
    f_win_ini << "current_project\t=\t\"" << rt[RT_PARAM].PackKey() << "\""  << endl;
    f_win_ini << "current_system\t=\t\"" << rt[RT_SYSEQ].PackKey() << "\""  << endl;
    f_win_ini.close();

    // Window-specific settings
    fname_ini = /*userGEMDir*/userProfDir() + WIN_CONF;
    f_win_ini.open(fname_ini.c_str(), ios::out );
    ErrorIf(!f_win_ini.good(), "GEMS Init",
            "Error writing configurator file (windows.conf)" );

//    f_win_ini << "# Format of the file and the order should be exactly the same" << endl;
    for (int ii = 0; ii < win_num; ii++)
        aWinInfo[ii]->toWinCFG(f_win_ini);

    f_win_ini.close();
}

void
TVisor::fromWinCFG()
{
    string fname_ini = /*userGEMDir*/userProfDir() + VIS_CONF;

#ifdef __unix
    TConfig visor_conf( fname_ini.c_str() );
#else
    TConfig& visor_conf = *new TConfig( fname_ini.c_str() );
#endif

    int win_num = 0;
    string name = visor_conf.getFirst();

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
                    strip(name);
		    QFont cellFont;
                    cellFont.fromString( name.c_str() );
		    pVisorImp->setCellFont(cellFont);
                }
        	else if( name == "axis_label_font_string" ) {
                    visor_conf.getcStr(name);
                    strip(name);
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
            	else if( name == "local_dir" ) {
                string gstr;
				visor_conf.getcStr(gstr);
				setLocalDir(gstr);
			}
            	else if( name == "local_doc_dir" ) {
                string gstr;
				visor_conf.getcStr(gstr);
				setLocalDocDir(gstr);
			}
            	else if( name == "remote_doc_url" ) {
                string gstr;
				visor_conf.getcStr(gstr);
                                setRemoteHTML(gstr);
			}
            	else if( name == "local_doc" ) {
				setLocalDoc(visor_conf.getcInt());
			}
            else if( name == "current_mode" ) {
               ProfileMode = visor_conf.getcInt();
            }
            else if( name == "default_built_in_TDB" ) {
                string gstr;
                visor_conf.getcStr(gstr);
                setDefaultBuiltinTDB(gstr);
            }
            else if( name == "current_project" ) {
               visor_conf.getcStr(lastProjectKey );
            }
            else if( name == "current_system" ) {
               visor_conf.getcStr(lastSystemKey );
            }

        name = visor_conf.getNext();
    }

    // Window-specific settings
    string fwin_ini_name = /*userGEMDir*/userProfDir() + WIN_CONF;
    ifstream f_win_ini(fwin_ini_name.c_str() );
    ErrorIf(!f_win_ini.good(), "GEMS Init",
            "Error reading configurator file (windows.conf)" );

 //   char name_str[256];
    // error Sveta 13/06/2001 reads only #
 //   f_win_ini >> name_str.p; // Don't compile in BCB5 without .p

    for (int ii = 0; ii < aWinInfo.size(); ii++)
        aWinInfo[ii]->fromWinCFG(f_win_ini);
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

//void
//TVisor::CalcFinished()
//{
//    pVisorImp->CalcFinished();
//}

void
TVisor::addModule(TCModule * pm, bool selectFiles)
{
    aMod.push_back(pm);
    pm->ods_link();
    //  pm->dyn_set();

    vector<int> arr;
    if (selectFiles)
        arr = pm->SelectFileList(openf | closef);
    rt[pm->rtNum()].Open(arr);
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
        addModule(TGEM2MT::pm = new TGEM2MT(RT_GEM2MT));

        TSData::pm->SetIcon(":/modules/Icons/SDrefModuleIcon.png");
        TConst::pm->SetIcon(":/Icons/gems50.png");
        TProfil::pm->SetIcon(":/modules/Icons/ModelingProjectIcon.png");
        TIComp::pm->SetIcon(":/modules/Icons/ICompModuleIcon.png");
        TDComp::pm->SetIcon(":/modules/Icons/DCompModuleIcon.png");
        TCompos::pm->SetIcon(":/modules/Icons/ComposModuleIcon.png");
        TReacDC::pm->SetIcon(":/modules/Icons/ReacDCModuleIcon.png");
        TRTParm::pm->SetIcon(":/modules/Icons/RTparmModuleIcon.png");
        TPhase::pm->SetIcon(":/modules/Icons/PhaseModuleIcon.png");
        TSysEq::pm->SetIcon(":/modules/Icons/SysEqModuleIcon.png");
        TProcess::pm->SetIcon(":/modules/Icons/ProcessModuleIcon.png");
        TUnSpace::pm->SetIcon(":/modules/Icons/UnSpaceModuleIcon.png");
        TGtDemo::pm->SetIcon(":/modules/Icons/GtDemoModuleIcon.png");
        TDualTh::pm->SetIcon(":/modules/Icons/DualThModuleIcon.png");
        TGEM2MT::pm->SetIcon(":/modules/Icons/GEM2MTModuleIcon.png");

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
        // delete auto-generated aq and gas phases if still in database
    	TProfil::pm->deleteAutoGenerated();

        toModCFG();
        toWinCFG();
        aObj[o_wo_bfc3].SetPtr(0);
        aObj[ o_neqtxt].SetPtr(0);
        aObj[ o_dtnam_nr].SetPtr(0);
        aObj[ o_dtres].SetPtr(0);
        aObj[ o_unpmr].SetPtr(0);
        aObj[ o_nlich].SetPtr(0);
        aObj[ o_nldch].SetPtr(0);
        aObj[ o_nldcvs].SetPtr(0);
        aObj[ o_nldchs].SetPtr(0);
        aObj[ o_nlphh].SetPtr(0);
        TGEM2MT::pm->FreeNa();

    }
    catch(TError & xcpt)
    {
        throw TFatalError(xcpt);
    }
}

void readDirs(vector<string>& aFiles, const char *dir)
{
    aFiles.clear();
// cout << "GEMS DB dir: " << dir << endl;
    QDir thisDir(dir);
    if (!thisDir.isReadable())
        throw TFatalError("GEMS Init", string(dir) + ": GEMS DB directory is not readable");

    thisDir.setFilter(QDir::Dirs);
    //    thisDir.setNameFilter("*.pdb");

    QFileInfoList files = thisDir.entryInfoList(); //Qt3to4
    if (files.empty()) //Qt3to4
        return;

    QListIterator<QFileInfo> it(files);
    QFileInfo f;
    while ( it.hasNext() ) //qt3to4
    {
        f = it.next();;
        if (f.isDir() && f.fileName() != "." && f.fileName() != "..")
        {
            //          cout << "Adding dir: " << f->fileName() << endl;
            aFiles.push_back( f.fileName().toUtf8().data());
        }
        // else 'special file'
    }
}

// default configuration
void
TVisor::defaultCFG()
{
    rtOld.Init();
    rtEJ.Init();


    // RT_PROFIL default
    size_t param_rkfrm[2] = { MAXMUNAME, MAXMUGROUP };
    rtOld.push_back( TDataBase(rtOld.size(), "projec", true, true,
                         o_spppar, o_spppar+14/*15*/, 0, 2, param_rkfrm));      // 12.12.12 added new object to Project record
    rtEJ.push_back( TEJDataBase(rtEJ.size(), "projec",
                         o_paver, o_sptext, 0, 2, param_rkfrm));      // 12.12.12 added new object to Project record

    // RT_ICOMP default
    size_t icomp_rkfrm[3] = { MAXICNAME, MAXSYMB, MAXICGROUP };
    rtOld.push_back( TDataBase(rtOld.size(), "icomp", false, true,
                         o_icsst, o_icsst+5/*6*/, 0, 3, icomp_rkfrm));
    rtEJ.push_back( TEJDataBase(rtEJ.size(), "icomp",
                         o_icawt, o_icint, 0, 3, icomp_rkfrm));
    // RT_DCOMP default
    size_t dcomp_rkfrm[4] = { MAXSYMB, MAXDRGROUP, MAXDCNAME, MAXSYMB };
    rtOld.push_back( TDataBase(rtOld.size(), "dcomp", false, true,
                         o_dcstr, o_dcstr+19/*20*/, 0, 4, dcomp_rkfrm));
    rtEJ.push_back( TEJDataBase(rtEJ.size(), "dcomp",
                         o_dcpct, o_dcsdval, 0, 4, dcomp_rkfrm));

    // RT_COMPOS default
    size_t compos_rkfrm[3] = { MAXCMPNAME, MAXSYMB, MAXCMPGROUP };
    rtOld.push_back( TDataBase(rtOld.size(), "compos", false, true,
                         o_bcpcc, o_bcpcc+19/*20*/, 0, 3, compos_rkfrm));
    rtEJ.push_back( TEJDataBase(rtEJ.size(), "compos",
                         o_bcpcc, o_bcsdval, 0, 3, compos_rkfrm));

    // RT_REACDC default
    size_t reacdc_rkfrm[4] = { MAXSYMB, MAXDRGROUP, MAXDCNAME, MAXSYMB };
    rtOld.push_back(  TDataBase(rtOld.size(), "reacdc", false, true,
                         o_restr, o_restr+19/*20*/, 0, 4, reacdc_rkfrm));
    rtEJ.push_back(  TEJDataBase(rtEJ.size(), "reacdc",
                         o_repct, o_resdval, 0, 4, reacdc_rkfrm));

    // RT_RTPARM default
    size_t rtparm_rkfrm[6] =
        { MAXSYMB, MAXDRGROUP, MAXDCNAME, MAXSYMB, MAXNV, MAXRTNAME };
    rtOld.push_back(  TDataBase(rtOld.size(), "rtparm", true, true,
                         o_rtname,o_rtname+26/*27*/, 0, 6, rtparm_rkfrm));
    rtEJ.push_back(  TEJDataBase(rtEJ.size(), "rtparm",
                         o_rtname,o_rppline, 0, 6, rtparm_rkfrm));

    // RT_PHASE default
    size_t phase_rkfrm[5] =
        { MAXSYMB, MAXPHSYMB, MAXPHNAME, MAXSYMB, MAXPHGROUP };
    rtOld.push_back(  TDataBase(rtOld.size(), "phase", true, true,
                         o_phstr, o_phstr+21+38/*22*/, 0, 5, phase_rkfrm));
    rtEJ.push_back(  TEJDataBase(rtEJ.size(), "phase",
                         o_phsolt, o_phlicu, 0, 5, phase_rkfrm));

    // RT_SYSEQ default
    size_t syseq_rkfrm[8] = { MAXMUNAME, MAXTDPCODE, MAXSYSNAME,
                                     MAXTIME, MAXPTN, MAXPTN, MAXPTN, MAXNV
                                   };
    rtOld.push_back(  TDataBase(rtOld.size(), "syseq", false, true,
                         o_ssphst,o_ssphst+70/*71*/, 0, 8, syseq_rkfrm));
    rtEJ.push_back(  TEJDataBase(rtEJ.size(), "syseq",
                         o_ssphst,o_stcrit, 0, 8, syseq_rkfrm));

    // RT_PROCES default
    size_t proces_rkfrm[10] = { MAXMUNAME, MAXTDPCODE, MAXSYSNAME,
                                       MAXTIME, MAXPTN, MAXPTN, MAXPTN, MAXNV, MAXPENAME, MAXPECODE
                                     };
    rtOld.push_back(  TDataBase(rtOld.size(), "proces", true, true,
                         o_pestr, o_pestr+39/*40*/, 0, 10, proces_rkfrm));
    rtEJ.push_back(  TEJDataBase(rtEJ.size(), "proces",
                         o_pepsts, o_pcplline, 0, 10, proces_rkfrm));

    // RT_UNSPACE default
    size_t unspace_rkfrm[10] = { MAXMUNAME, MAXTDPCODE, MAXSYSNAME,
                 MAXTIME, MAXPTN, MAXPTN, MAXPTN, MAXNV, MAXPENAME, MAXPECODE
                                       };
    rtOld.push_back(  TDataBase(rtOld.size(), "unspac", true, true,
                         o_unname, o_unname+69/*70*/, 0, 10, unspace_rkfrm));
    rtEJ.push_back(  TEJDataBase(rtEJ.size(), "unspac",
                         o_unsts, o_unplline, 0, 10, unspace_rkfrm));

    // RT_GTDEMO default
    size_t gtdemo_rkfrm[5] =
        { MAXMUNAME, MAXDATATYPE, MAXGTDCODE, MAXNV, MAXGDGROUP };
    rtOld.push_back(  TDataBase(rtOld.size(), "gtdemo", true, true,
                         o_gdps, o_gdps+26/*27*/, 0, 5, gtdemo_rkfrm));
    rtEJ.push_back(  TEJDataBase(rtEJ.size(), "gtdemo",
                         o_gdps, o_gdplline, 0, 5, gtdemo_rkfrm));

    // RT_DUALTH default
    size_t dualth_rkfrm[10] = { MAXMUNAME, MAXTDPCODE, MAXSYSNAME,
       MAXTIME, MAXPTN, MAXPTN, MAXPTN, MAXNV, MAXPENAME, MAXPECODE
                                  };
    rtOld.push_back(  TDataBase(rtOld.size(), "dualth", true, true,
                         o_dtname, o_dtname+52/*53*/, 0, 10, dualth_rkfrm));
    rtEJ.push_back(  TEJDataBase(rtEJ.size(), "dualth",
                         o_dtpvfl, o_dttprn, 0, 10, dualth_rkfrm));

    // RT_GEM2MT default
    size_t gem2mt_rkfrm[10] = { MAXMUNAME, MAXTDPCODE, MAXSYSNAME,
       MAXTIME, MAXPTN, MAXPTN, MAXPTN, MAXNV, MAXPENAME, MAXPECODE
                                  };
    rtOld.push_back( TDataBase(rtOld.size(), "gem2mt", true, true,
                         o_mtname, o_mtname+66/*67*/, 0, 10, gem2mt_rkfrm));
    rtEJ.push_back( TEJDataBase(rtEJ.size(), "gem2mt",
                         o_mtpufl, o_mtplline, 0, 10, gem2mt_rkfrm));

    // reading default DataBase and proj_ejdb dirs for EJDB
    flsEJ.Init();

    // read default database
    vector<string> aDBFiles;
    readPDBDir(aDBFiles, pVisor->sysDBDir().c_str(), "*.pdb");

    for (int jj = 0; jj < rtOld.size(); jj++)
    {
        int cnt = 0;
        for (int ii = 0; ii < aDBFiles.size(); ii++)
        { string flnm = string( aDBFiles[ii], 0, aDBFiles[ii].find("."));
          if ( flnm == rtOld[jj].GetKeywd() ||
                 ( jj == RT_UNSPACE && flnm == "probe" ) ||   //set up old name
                 ( jj == RT_DUALTH && flnm == "duterm" ) )   //set up old name
            {
                string path = pVisor->sysDBDir().c_str();
                path += aDBFiles[ii];
                rtOld[jj].AddFile(path);
                cnt++;
            }
        }
        rtOld[jj].SetSpecialFilesNum( cnt);
    }

    // reading project dirs
    vector<string> aDBDirs;
    readDirs(aDBDirs, pVisor->userOldProfDir().c_str());
    for ( int ii = 0; ii < aDBDirs.size(); ii++)
    {
        string dir = pVisor->userOldProfDir().c_str();
        dir += aDBDirs[ii];
        readPDBDir(aDBFiles, dir.c_str(), "*.pdb");

        for (int jj = 0; jj < rtOld.size(); jj++)
        {
          for (int kk = 0; kk < aDBFiles.size(); kk++)
          { string flnm = string(aDBFiles[kk], 0, aDBFiles[kk].find("."));
            if ( flnm == rtOld[jj].GetKeywd() ||
                ( jj == RT_UNSPACE && flnm == "probe" ) ||   //set up old name
                ( jj == RT_DUALTH && flnm == "duterm" ) )   //set up old name
                {
                    string path(dir);
                    path += "/";
                    path += aDBFiles[kk];
                    rtOld[jj].AddFile(path);
                }
          }
        }
    }

}


void TVisor::deleteDBDir(const string& dir)
{
    vector<string> aFiles;

#ifndef oldDBusing
    int iDB = flsEJ.FindPath( dir );
    if( iDB > 0)
    {
       // close iDB collection
        for ( int jj = 0; jj < rt.size(); jj++)
            {
                rt[jj].Close();
                rt[jj].delFnum(iDB);
                rt[jj].Open();
            }
       // delete EJDB
        flsEJ.erase(flsEJ.begin()+iDB);
    }
#endif

    QDir thisDir(dir.c_str());
    if (!thisDir.exists())
        return;

    //--QDir::setCurrent(dir);
    thisDir.setFilter(QDir::Files);

    QFileInfoList files = thisDir.entryInfoList();
    if (files.empty())
        return;

    QListIterator<QFileInfo> it(files);
    QFileInfo f;
    while ( it.hasNext() )
    {
        f = it.next();;
        if (f.isSymLink() || f.isFile())
        {
            aFiles.push_back( f.fileName().toUtf8().data());
        }
        // else 'special file'
    }

    // delete files in module list
    string path;
    for ( int ii = 0; ii < aFiles.size(); ii++)
    {
#ifdef oldDBusing
        if (string(aFiles[ii], aFiles[ii].rfind(".") + 1) == "pdb")
        {
            for ( int jj = 0; jj < rtOld.size(); jj++)
                if (string(aFiles[ii], 0, aFiles[ii].find("."))
                        == rtOld[jj].GetKeywd())
                {
                    path = dir;
                    path += "/";
                    path += aFiles[ii];
                    //cout << path << endl;
                    rtOld[jj].Close();
                    rtOld[jj].DelFile(path);
                    rtOld[jj].Open();
                }
        }
#endif
        path = dir;
        path += "/";
        path += aFiles[ii];
        QFile ff(path.c_str());
        ff.remove();
    }

    thisDir.rmdir(dir.c_str());
}

// added Sveta 23-07-2001

// copy file
void TVisor::CopyF( const char * fName, const char* fTempl )
{
  QFile ff_tmp(fTempl);
  if( !ff_tmp.open(QIODevice::ReadOnly))
     Error(fTempl, "File copy error" );
  ff_tmp.flush();
  QFile ff_new(fName);
  if( !ff_new.open(QIODevice::WriteOnly))
     Error(fName, "File copy error" );

  auto_ptr<char> p( new char[ff_tmp.size()+2]);
  ff_tmp.read(p.get(), ff_tmp.size());
  ff_new.write(p.get(), ff_tmp.size());
  ff_tmp.close();
  ff_new.close();
}

void
TVisor::makeDBDir(const char *dir, bool testExist )
{
  // make directory dir (find system function)
    QDir d(dir);
    if (  d.exists() )
    { if( testExist && d.count()>2 )
      {
    	  QStringList filters = (QStringList() << "*.ndx" << "*.pdb");
         QStringList lst = d.entryList( filters, QDir::Files );
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

void TVisor::readPDBDir(vector<string>& aFiles, const char *dir, const char *filter )
{
    aFiles.clear();
    QDir thisDir(dir);
    if (!thisDir.isReadable())
    {
#ifdef __unix
        cout << "Error :" << dir << endl;
#endif
        throw TFatalError(/*"GEMS Init"*/dir, "GEMS DB directory is not readable");
    }
    QStringList afilt(filter);

    thisDir.setFilter(QDir::Files);
    thisDir.setNameFilters(afilt);

    QFileInfoList files = thisDir.entryInfoList(); //Qt3to4 
    if (files.empty()) //Qt3to4
        return;

    QListIterator<QFileInfo> it(files);
    QFileInfo f;
    while ( it.hasNext() ) //qt3to4
    {
        f = it.next();;
        if (f.isSymLink() || f.isFile())
        {
            // cout << "Adding file: " << f->fileName() << endl;
            aFiles.push_back( f.fileName().toUtf8().data());
        }
        // else 'special file'
    }
}

// added Sveta 14/08/2001
//void
//TVisor::OpenHelp(const char* file, const char* item, int page)
//{
//    pVisorImp->OpenHelp( file );
//}

void
TVisor::OpenModule(QWidget* parent, int i, int page, int viewmode, bool select)
{
    pVisorImp->OpenModule( parent, i, page, viewmode,  select);
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
TVisor::ProcessProgress( QWidget* parent, int nRT )
{
    pVisorImp->ProcessProgress( parent,  nRT );
}

// return true if canceled
bool
TVisor::Message( QWidget* parent, const char* name,
             const char* msg, int prog, int total, bool move)
{
    return pVisorImp->Message( parent, name, msg, prog, total, move);
}

void
TVisor::CloseMessage()
{
    pVisorImp->CloseMessage();
}

char TVisor::GetNumericalMethod()
{
    pVisorImp->GetNumericalMethod();
}


TVisor *pVisor;

// ----------- End of  visor.cpp ----------------------------
