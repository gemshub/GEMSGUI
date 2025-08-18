//-------------------------------------------------------------------
// $Id: visor.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of TVisor class, setup and config functions
//
// Copyright (C) 1996-2012 A.Rysin,S.Dmytriyeva,D.Kulik
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (https://qt.io/download-open-source)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the GPL v.3 license

//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------
//
#include <QApplication>
#ifndef _WIN32
#include <unistd.h>
//#else
//#include <io.h>
#endif

#include <QDir>
#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QJsonObject>
#include <QJsonDocument>
#include <cstdlib>

#include "GEMS3K/jsonconfig.h"
#include "service.h"
#include "visor.h"
#include "units.h"
#include "page_w.h"

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
const char *DEFAULT_USER_DIR= "/Library/Gems3/";
const char *DEFAULT_PR_DIR= "projects/";
const char *GEM_CONF = "gemsdbf.conf";
const char *VIS_CONF = "visor.conf";
const char *WIN_CONF = "windows.conf";

//----------------------------------------------------------------
// TVisor  class implementation
//----------------------------------------------------------------

TVisor::TVisor(int c, char *v[]):
        argc(c), argv(v)
{
   Q_INIT_RESOURCE(GUI);

    ProfileMode = MDD_DATABASE;
    dbChangeMode = false;
    isElementsProfileMode = true;

#ifndef _WIN32
#ifdef __APPLE__

    QString dirExe = QCoreApplication::applicationDirPath();
    auto app_index = dirExe.lastIndexOf("/gem-selektor.app/Contents", -1, Qt::CaseInsensitive );

    if( app_index >= 0 )
    {
        SysGEMDir = dirExe.left(app_index).toStdString() + "/gem-selektor.app/Contents/Resources/";
    }
    else {
        // non-standard executable path, search for resources starting with current dir
        SysGEMDir = dirExe.toStdString() + "/gem-selektor.app/Contents/Resources/";
    }
    UserGEMDir = home_dir() + DEFAULT_USER_DIR; // "/Library/gems3/";

#else  // Linux - in user's home directory
       // By default: /Resources in the same dir as the exe file;
       //       /Library/gems3/projects on the same level as the /Gems3-app dir.
    QString dirExe = QCoreApplication::applicationDirPath();
    if(dirExe.endsWith("/bin") || dirExe.endsWith("\bin")) { // Try found Resource up level
        dirExe.chop(4);
    }
    SysGEMDir = dirExe.toStdString();
    SysGEMDir += RESOURCES_DIR;
    //ServerGems3Dir = dirExe.toStdString();
    QDir dirUp(dirExe);
    if( dirUp.cdUp() )
         dirExe = dirUp.path(); // + QDir::separator();
    LocalDir = dirExe.toStdString();

#ifdef NDEBUG
    UserGEMDir = home_dir() + DEFAULT_USER_DIR;
#else
    UserGEMDir =  localDir() + DEFAULT_USER_DIR;
#endif

#endif // __unix

#else
      // windows - in any non-system directory on C:/ or D:/
      // By default: /Resources in the same dir as the exe file;
      //       /Library/gems3/projects on the same level as the /Gems3-app dir.
    QString dirExe = QCoreApplication::applicationDirPath();
    if(dirExe.endsWith("/bin") || dirExe.endsWith("\bin")) { // Try found Resource up level
        dirExe.chop(4);
    }
    SysGEMDir = dirExe.toStdString();
    SysGEMDir += RESOURCES_DIR;
    QDir dirUp(dirExe);
    if( dirUp.cdUp() )
         dirExe = dirUp.path();
    LocalDir = dirExe.toStdString();
#ifdef NDEBUG
    UserGEMDir =  home_dir() + DEFAULT_USER_DIR;
#else
    UserGEMDir =  localDir() + DEFAULT_USER_DIR;
#endif
#endif // win

    DefDBDir = DEFAULT_DB_DIR;
    DefProfDir = DEFAULT_PR_DIR;
    UserProfDir = DEFAULT_PR_DIR;
    ImgDir = IMAGES_SRC_DIR;

    // parsing options -s and -u if given

    int isys = 0;		// index of sysdir option
    int iuser = 0;		// index of userdir option
    //int iserver = 0;		// index of server option

    for (int ii = 1; ii < argc; ii++)
    {
        if (strcmp(argv[ii], "-s") == 0
                || strcmp(argv[ii], "--system-dir") == 0 )
            isys = ii;
        else if (strcmp(argv[ii], "-u") == 0
                 || strcmp(argv[ii], "--user-dir") == 0 )
            iuser = ii;
        // else if (strcmp(argv[ii], "-g") == 0
        //          || strcmp(argv[ii], "--gems-server-dir") == 0 )
        //     iserver = ii;
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
    // if (iserver != 0)
    // {
    //     if (argc <= iserver + 1)
    //         Error("Wrong options", "Wrong argument for option -g");
    //     ServerGems3Dir = argv[iserver + 1];
    // }

//    LocalDir = userGEMDir();
    LocalDocDir = SysGEMDir + HELP_DB_DIR;
    //RemoteDocURL = "http://gems.web.psi.ch/doc/html/";
    RemoteHTML = SysGEMDir + HELP_SRC_DIR;
    LocalDoc = true;
    DefaultBuiltinTDB = "psi-nagra";  // PSI-Nagra TDB update TT 03.04.2013
//    DefaultBuiltinTDB = "kernel";   temporary for using old Nagra-PSI (2003) dataset
//    DefaultBuiltinTDB = "psinagra";  // To be used after update to PSI-Nagra 2012

    GemsSettings::data_logger_directory = UserGEMDir+"logs/";
    GemsSettings::settings_file_name = SysGEMDir+"gemsgui-config.json";
    gemsSettings();
    // spdlog levels :  trace = 0, debug = 1, info = 2, warn = 3, err = 4, critical = 5, off = 6
    //gemsSettings().gems3k_update_loggers( true, "gems3k_gui.log", spdlog::level::info);
    //gui_logger->set_level(spdlog::level::info);


// For debugging the directories
    gui_logger->info("Local    : {}", LocalDir);
    gui_logger->info("SysGEM   : {}", SysGEMDir);
    gui_logger->info("UserGEM  : {}", UserGEMDir);
    gui_logger->debug("UserProj : {}", UserProfDir);
    gui_logger->debug("LocalDoc : {}", LocalDocDir);
    gui_logger->debug("LocalHTML: {}", RemoteHTML);
/*
#ifndef _WIN32
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

void TVisor::Setup()
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

    if (argc == 1 ) // No command line parameters - assume as -d   since v.3.8.0
    {
        option_d = true;
        // default_config = true;
        pVisorImp->setConfigAutosave( true );
    }

    // check home dir
    string dir = userGEMDir();
    QDir userGEM(dir.c_str());

    bool firstTimeStart = !userGEM.exists(userProfDir().c_str());

    if (firstTimeStart)
    {
        default_config = true;
        default_settings = true;
        pVisorImp->setConfigAutosave( true );

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

        gui_logger->debug("make home GEM directories");
        gui_logger->debug("UserGEM *: {}", UserGEMDir);
        if(!userGEM.exists(userGEMDir().c_str())) {
            if( !userGEM.mkdir(userGEMDir().c_str()) )
                throw TFatalError("GEMS Init", "Cannot create user GEMS directory");
        }
        gui_logger->debug("UserProj*: {}", UserProfDir);
        if( !userGEM.mkdir(userProfDir().c_str()) )
            throw TFatalError("GEMS Init", "Cannot create user GEMS projects directory");

        // copy default project
        string cmd;

#ifndef _WIN32
        cmd = "cp -r ";
        cmd += sysProfDir();
        cmd += "* ";
        cmd += userProfDir();

        gui_logger->debug("Creating GEMS user directory:  {}", cmd);
#else
        string sprdir = sysProfDir();
        string uprdir = userProfDir();
        QDir sysProjD( sprdir.c_str() );
        QDir usrProjD( uprdir.c_str() );
        QString sPD = sysProjD.absolutePath();
        QString uPD = usrProjD.absolutePath();

        cmd = "xcopy \"";
        cmd += 	qPrintable(	sysProjD.toNativeSeparators( sPD ) );
        //        cmd += DefProfDir;
        cmd += "\" \"";
        cmd += 	qPrintable( usrProjD.toNativeSeparators( uPD ) );
        cmd += "\" /e /y";

        gui_logger->debug("Creating GEMS user directory:  {}", cmd);
#endif

        if (system(cmd.c_str()) != 0)
            throw TFatalError("GEMS Init", "Cannot copy default projects to user directory");
    }

    if (option_d)
        load();
    else
        fromDAT(default_config, default_settings);

    aObj[o_n0w_mps]->SetPtr(0);
    aObj[o_n1w_mps]->SetPtr(0);
    aObj[o_n0w_vps]->SetPtr(0);
    aObj[o_n1w_vps]->SetPtr(0);
    aObj[o_n0w_m_t]->SetPtr(0);
    aObj[o_n1w_m_t]->SetPtr(0);
    aObj[o_n0w_con]->SetPtr(0);
    aObj[o_n1w_con]->SetPtr(0);
    aObj[o_n0w_mju]->SetPtr(0);
    aObj[o_n1w_mju]->SetPtr(0);
    aObj[o_n0w_lga]->SetPtr(0);
    aObj[o_n1w_lga]->SetPtr(0);

    // Sveta permission to change data in special DB files
    if (option_f)
        dbChangeMode = true;
}


const int lnWINSIG = 2;
const char SigBEG[lnWINSIG + 1] = "Vs";
const char SigEND[lnWINSIG + 1] = "sX";

void TVisor::load()
{
    string fname = sysGEMDir() + OBJECT_INI;
    gui_logger->debug("TVisor::load {}", fname);
    aObj.load(fname.c_str());

    defaultCFG();
    initModules();

    fname = sysGEMDir();
    fname += UNITS_INI;
    aUnits.load(fname);

    fname = sysGEMDir();
    fname += VISOR_INI;

    TJsonConfig cnf( std::string(fname) + ".json");
    for (size_t ii = 0; ii < aMod.size(); ii++)
        aWinInfo.push_back( std::make_shared<CWinInfo>(*aMod[ii], cnf));

    toDAT();
    toModCFG();
    toWinCFG();
}

void
TVisor::toDAT()
{
    string fname = sysGEMDir();
    fname += VISOBJ_DAT;

    ofstream obj_dat(fname, ios::binary | ios::out);
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

    int n = aMod.size(); // Do not change type, used in configuration
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
        std::string message = "Can't open ";
        message += fname.c_str();
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
        std::string message = "Can't open ";
        message += fname.c_str();
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
        aWinInfo.push_back( std::make_shared<CWinInfo>(*aMod[ii], visor_dat));

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
    rt.toCFG(f_gems);
#ifndef _WIN32
    gui_logger->info("gems.cfg saved");
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
    rt.fromCFG(f_gems);
#ifndef _WIN32
    gui_logger->info("gems.cfg read");
#endif
}

void TVisor::toWinCFG()
{
    string fname_ini = /*userGEMDir*/userProfDir() + VIS_CONF + ".json";

    QJsonObject win_cfg_object;
    win_cfg_object["color_scheme"] = pVisorImp->getColorScheme();
    win_cfg_object["double_precision"] = pVisorImp->getDoubleDigits();
    win_cfg_object["update_interval"] = pVisorImp->updateInterval();
    win_cfg_object["general_font_string"] = pVisorImp->getCellFont().toString();
    win_cfg_object["axis_label_font_string"] = pVisorImp->getAxisLabelFont().toString();
    win_cfg_object["number_of_windows"] = static_cast<int>(aWinInfo.size());
    win_cfg_object["config_autosave"] = pVisorImp->getConfigAutosave();
    win_cfg_object["local_dir"] = QString::fromStdString(LocalDir);
    win_cfg_object["local_doc_dir"] = QString::fromStdString(LocalDocDir);
    win_cfg_object["remote_doc_url"] = QString::fromStdString(RemoteHTML);
    win_cfg_object["local_doc"] = LocalDoc;
    win_cfg_object["current_mode"] = ProfileMode;
    win_cfg_object["current_project"] = rt[RT_PARAM]->PackKey();
    win_cfg_object["current_system"] = rt[RT_SYSEQ]->PackKey();
    win_cfg_object["default_built_in_TDB"] = QString::fromStdString(DefaultBuiltinTDB);

    fstream f_win_ini(fname_ini.c_str(), ios::out );
    ErrorIf(!f_win_ini.good(), "GEMS Init",
            "Error writing configurator file (visor.conf)");
    QJsonDocument doc(win_cfg_object);
    QString str_json(doc.toJson());
    f_win_ini << str_json.toStdString()  << endl;
    f_win_ini.close();

    // Window-specific settings
    fname_ini = /*userGEMDir*/userProfDir() + WIN_CONF;
    f_win_ini.open(fname_ini.c_str(), ios::out );
    ErrorIf(!f_win_ini.good(), "GEMS Init",
            "Error writing configurator file (windows.conf)" );
    //    f_win_ini << "# Format of the file and the order should be exactly the same" << endl;
    for (size_t ii = 0; ii < aWinInfo.size(); ii++)
        aWinInfo[ii]->toWinCFG(f_win_ini);
    f_win_ini.close();
}

void TVisor::fromWinCFG()
{
    string fname_ini = /*userGEMDir*/userProfDir() + VIS_CONF;

    TJsonConfig visor_conf( fname_ini+".json" );
    pVisorImp->setColorScheme(visor_conf.value_or_default("color_scheme", 0));
    pVisorImp->setDoubleDigits(visor_conf.value_or_default("double_precision", pVisorImp->getDoubleDigits()));
    pVisorImp->setUpdateInterval(visor_conf.value_or_default("update_interval", pVisorImp->updateInterval()));
    std::string font_str = visor_conf.value_or_default<std::string>("general_font_string", "");
    if( !font_str.empty() ) {
        strip( font_str );
        QFont cellFont;
        cellFont.fromString( font_str.c_str() );
        pVisorImp->setCellFont(cellFont);
    }
    font_str = visor_conf.value_or_default<std::string>("axis_label_font_string", "");
    if( !font_str.empty() ) {
        strip( font_str );
        QFont axisLabelFont;
        axisLabelFont.fromString( font_str.c_str() );
        pVisorImp->setAxisLabelFont(axisLabelFont);
    }
    pVisorImp->setConfigAutosave(visor_conf.value_or_default("config_autosave",  pVisorImp->getConfigAutosave()));
    setLocalDir(visor_conf.value_or_default<std::string>("local_dir", LocalDir));
    setLocalDocDir(visor_conf.value_or_default<std::string>("local_doc_dir", LocalDocDir));
    setRemoteHTML(visor_conf.value_or_default<std::string>("remote_doc_url", RemoteHTML));
    setLocalDoc(visor_conf.value_or_default("local_doc",  LocalDoc));
    ProfileMode = visor_conf.value_or_default("current_mode",  ProfileMode);
    setDefaultBuiltinTDB(visor_conf.value_or_default<std::string>("default_built_in_TDB", DefaultBuiltinTDB));
    lastProjectKey = visor_conf.value_or_default<std::string>("current_project", lastProjectKey);
    lastSystemKey = visor_conf.value_or_default<std::string>("current_system", lastSystemKey);

    // Window-specific settings
    string fwin_ini_name = /*userGEMDir*/userProfDir() + WIN_CONF;
    ifstream f_win_ini(fwin_ini_name.c_str() );
    ErrorIf(!f_win_ini.good(), "GEMS Init",
            "Error reading configurator file (windows.conf)" );

    for (size_t ii = 0; ii < aWinInfo.size(); ii++)
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

string TVisor::filePathFromName(const string& filename, const string& extension)
{
    auto fname_default = filename;
    replace_all(fname_default, " <>:\"/\\|?*.", '_' );
    if( fname_default.empty() )
        fname_default =  "empty";

    auto  file_path  =  pVisor->localDir();
    file_path  +=  "/";
    file_path  +=  fname_default;
    file_path  +=  ".";
    file_path  +=  extension;
    return  file_path;
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
        aObj[o_wo_bfc3]->SetPtr(0);
        aObj[ o_neqtxt]->SetPtr(0);
        aObj[ o_dtnam_nr]->SetPtr(0);
        aObj[ o_dtres]->SetPtr(0);
        aObj[ o_unpmr]->SetPtr(0);
        aObj[ o_nlich]->SetPtr(0);
        aObj[ o_nldch]->SetPtr(0);
        aObj[ o_nldcvs]->SetPtr(0);
        aObj[ o_nldchs]->SetPtr(0);
        aObj[ o_nlphh]->SetPtr(0);
        TGEM2MT::pm->FreeNa();

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


    // RT_PROFIL default
    unsigned char param_rkfrm[2] = { MAXMUNAME, MAXMUGROUP };
    rt.push_back( std::make_shared<TDataBase>(rt.size(), "projec", true, true,
                         o_spppar, 15, 0, 2, param_rkfrm));      // 12.12.12 added new object to Project record

    // RT_ICOMP default
    unsigned char icomp_rkfrm[3] = { MAXICNAME, MAXSYMB, MAXICGROUP };
    rt.push_back( std::make_shared<TDataBase>(rt.size(), "icomp", false, true,
                         o_icsst, 6, 0, 3, icomp_rkfrm));
    rt.back()->updateJsonOD(o_icawt, o_icint);

    // RT_DCOMP default
    unsigned char dcomp_rkfrm[4] = { MAXSYMB, MAXDRGROUP, MAXDCNAME, MAXSYMB };
    rt.push_back( std::make_shared<TDataBase>(rt.size(), "dcomp", false, true,
                         o_dcstr, 20, 0, 4, dcomp_rkfrm));
    rt.back()->updateJsonOD(o_dcpct, o_dcsdval);

    // RT_COMPOS default
    unsigned char compos_rkfrm[3] = { MAXCMPNAME, MAXSYMB, MAXCMPGROUP };
    rt.push_back( std::make_shared<TDataBase>(rt.size(), "compos", false, true,
                         o_bcpcc, 20, 0, 3, compos_rkfrm));

    // RT_REACDC default
    unsigned char reacdc_rkfrm[4] = { MAXSYMB, MAXDRGROUP, MAXDCNAME, MAXSYMB };
    rt.push_back( std::make_shared<TDataBase>(rt.size(), "reacdc", false, true,
                         o_restr, 20, 0, 4, reacdc_rkfrm));
    rt.back()->updateJsonOD(o_repct, o_resdval);

    // RT_RTPARM default
    unsigned char rtparm_rkfrm[6] =
        { MAXSYMB, MAXDRGROUP, MAXDCNAME, MAXSYMB, MAXNV, MAXRTNAME };
    rt.push_back( std::make_shared<TDataBase>(rt.size(), "rtparm", true, true,
                         o_rtname, 27, 0, 6, rtparm_rkfrm));

    // RT_PHASE default
    unsigned char phase_rkfrm[5] =
        { MAXSYMB, MAXPHSYMB, MAXPHNAME, MAXSYMB, MAXPHGROUP };
    rt.push_back( std::make_shared<TDataBase>(rt.size(), "phase", true, true,
                         o_phstr, 22+38/*13/06/13*/, 0, 5, phase_rkfrm));
    rt.back()->updateJsonOD(o_phsolt, o_phlicu);

    // RT_SYSEQ default
    unsigned char syseq_rkfrm[8] = { MAXMUNAME, MAXTDPCODE, MAXSYSNAME,
                                     MAXTIME, MAXPTN, MAXPTN, MAXPTN, MAXNV
                                   };
    rt.push_back( std::make_shared<TDataBase>(rt.size(), "syseq", false, true,
                         o_ssphst, 71, 0, 8, syseq_rkfrm));

    // RT_PROCES default
    unsigned char proces_rkfrm[10] = { MAXMUNAME, MAXTDPCODE, MAXSYSNAME,
                                       MAXTIME, MAXPTN, MAXPTN, MAXPTN, MAXNV, MAXPENAME, MAXPECODE
                                     };
    rt.push_back( std::make_shared<TDataBase>(rt.size(), "proces", true, true,
                         o_pestr, 26+14/*11/03/02*/, 0, 10, proces_rkfrm));

    // RT_UNSPACE default
    unsigned char unspace_rkfrm[10] = { MAXMUNAME, MAXTDPCODE, MAXSYSNAME,
                 MAXTIME, MAXPTN, MAXPTN, MAXPTN, MAXNV, MAXPENAME, MAXPECODE
                                       };
    rt.push_back( std::make_shared<TDataBase>(rt.size(), "unspac", true, true,
                         o_unname, 70, 0, 10, unspace_rkfrm));


    // RT_GTDEMO default
    unsigned char gtdemo_rkfrm[5] =
        { MAXMUNAME, MAXDATATYPE, MAXGTDCODE, MAXNV, MAXGDGROUP };
    rt.push_back( std::make_shared<TDataBase>(rt.size(), "gtdemo", true, true,
                         o_gdps, 27, 0, 5, gtdemo_rkfrm));

    // RT_DUALTH default
    unsigned char dualth_rkfrm[10] = { MAXMUNAME, MAXTDPCODE, MAXSYSNAME,
       MAXTIME, MAXPTN, MAXPTN, MAXPTN, MAXNV, MAXPENAME, MAXPECODE
                                  };
    rt.push_back( std::make_shared<TDataBase>(rt.size(), "dualth", true, true,
                         o_dtname, 53, 0, 10, dualth_rkfrm));

    // RT_GEM2MT default
    unsigned char gem2mt_rkfrm[10] = { MAXMUNAME, MAXTDPCODE, MAXSYSNAME,
       MAXTIME, MAXPTN, MAXPTN, MAXPTN, MAXNV, MAXPENAME, MAXPECODE
                                  };
    rt.push_back( std::make_shared<TDataBase>(rt.size(), "gem2mt", true, true,
                         o_mtname, 67, 0, 10, gem2mt_rkfrm));

    // read default database
    TCStringArray aDBFiles = readPDBDir(pVisor->sysDBDir().c_str(), "*.pdb");
    //  readPDBDir(pVisor->userProfDir().c_str());

    for (size_t jj = 0; jj < rt.size(); jj++)
    {
        int cnt = 0;
         for (size_t ii = 0; ii < aDBFiles.size(); ii++)
        { string flnm = string(aDBFiles[ii], 0, aDBFiles[ii].find("."));
            if ( flnm == rt[jj]->GetKeywd() ||
                 ( jj == RT_UNSPACE && flnm == "probe" ) ||   //set up old name
                 ( jj == RT_DUALTH && flnm == "duterm" ) )   //set up old name
            {
                string path = pVisor->sysDBDir();
                path += aDBFiles[ii];
                rt[jj]->AddFile(path.c_str());
                cnt++;
            }
        }
        rt[jj]->specialFilesNum = cnt;
    }

    // reading project dirs
    TCStringArray aDBDirs = readDirs(pVisor->userProfDir().c_str());
    for (size_t ii = 0; ii < aDBDirs.size(); ii++)
    {
        string dir(pVisor->userProfDir());
        dir += aDBDirs[ii];
        aDBFiles = readPDBDir(dir.c_str(), "*.pdb");

        for (size_t jj = 0; jj < rt.size(); jj++)
        {
          for (size_t kk = 0; kk < aDBFiles.size(); kk++)
          { string flnm = string(aDBFiles[kk], 0, aDBFiles[kk].find("."));
            if ( flnm == rt[jj]->GetKeywd() ||
                ( jj == RT_UNSPACE && flnm == "probe" ) ||   //set up old name
                ( jj == RT_DUALTH && flnm == "duterm" ) )   //set up old name
                {
                    string path(dir);
                    path += "/";
                    path += aDBFiles[kk];
                    rt[jj]->AddFile(path.c_str());
                }
          }
        }
    }

}

TCStringArray readDirs(const char *dir)
{
    TCStringArray aFiles;
    gui_logger->debug("GEMS DB dir: {}", dir);
    QDir thisDir(dir);
//    if (!thisDir.isReadable())
//        throw TFatalError("GEMS Init", std::string(dir) + ": GEMS DB directory is not readable");

    thisDir.setFilter(QDir::Dirs);
    //    thisDir.setNameFilter("*.pdb");

    QFileInfoList files = thisDir.entryInfoList();
    if (files.empty())
        return aFiles;

    QListIterator<QFileInfo> it(files);
    QFileInfo f;
    while (it.hasNext())
    {
        f = it.next();;
        if (f.isDir() && f.fileName() != "." && f.fileName() != "..")
        {
            gui_logger->debug("Adding dir: {}", f.fileName().toStdString());
            aFiles.push_back( f.fileName().toStdString());
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

    //--QDir::setCurrent(dir);
    thisDir.setFilter(QDir::Files);

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
            gui_logger->trace("Adding file: {}", f.fileName().toStdString());
            aFiles.push_back(f.fileName().toStdString());
        }
        // else 'special file'
    }

    //--QDir::setCurrent(dir);
    // delete files in module list
    std::string path;
    for (size_t ii = 0; ii < aFiles.size(); ii++)
    {
        if (string(aFiles[ii], aFiles[ii].rfind(".") + 1) == "pdb")
        {
            for (size_t jj = 0; jj < rt.size(); jj++)
                if (string(aFiles[ii], 0, aFiles[ii].find("."))
                        == rt[jj]->GetKeywd())
                {
                    path = dir;
                    path += "/";
                    path += aFiles[ii];
                    gui_logger->debug("DelFile: {}", path);
                    rt[jj]->Close();
                    rt[jj]->DelFile(path);
                    rt[jj]->Open(true, UPDATE_DBV, {});
                }
        }
        path = dir;
        path += "/";
        path += aFiles[ii].c_str();
        gui_logger->debug("remove: {}", path);
        QFile ff(path.c_str()/*aFiles[ii].c_str()*/);
        ff.remove();
    }

    thisDir.rmdir(dir);
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

  char *p = new char[ff_tmp.size()+2];
  ff_tmp.read(p, ff_tmp.size());
  ff_new.write(p, ff_tmp.size());
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

TCStringArray TVisor::readPDBDir(const char *dir, const char *filter )
{
    TCStringArray aFiles;

    QDir thisDir(dir);
//    if (!thisDir.isReadable())
//    {
//#ifndef _WIN32
//        gui_logger->error("{} directory is not readable", dir);
//#endif
//        throw TFatalError(/*"GEMS Init"*/dir, "GEMS DB directory is not readable");
//    }
    QStringList afilt(filter);

    thisDir.setFilter(QDir::Files);
    thisDir.setNameFilters(afilt);

    QFileInfoList files = thisDir.entryInfoList(); //Qt3to4 
    if (files.empty()) //Qt3to4
        return aFiles;

    QListIterator<QFileInfo> it(files);
    QFileInfo f;
    while ( it.hasNext() ) //qt3to4
    {
        f = it.next();;
        if (f.isSymLink() || f.isFile())
        {
            gui_logger->trace("Adding file: {}", f.fileName().toStdString());
            aFiles.push_back(f.fileName().toStdString());
        }
        // else 'special file'
    }

    return aFiles;
}

// added Sveta 14/08/2001
//void
//TVisor::OpenHelp(const char* file, const char* item, int page)
//{
//    pVisorImp->OpenHelp( file );
//}

void
TVisor::OpenModule(QWidget* parent, uint i, int page, int viewmode, bool select)
{
    pVisorImp->OpenModule( parent, i, page, viewmode,  select);
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


TVisor *pVisor;

// ----------- End of  visor.cpp ----------------------------
