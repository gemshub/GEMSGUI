#include <iostream>
#include "GEMS3K/jsonconfig.h"
#include "visor.h"
#include "m_param.h"
#include "nodearray_gui.h"


struct ExportTask {
    /// Current system key from project
    std::string system_record;

    /// IPM work structure file path&name
    std::string ipmfiles_lst_name;

    /// Flag for calculation of equilibrium state before export.
    /// Possible values: 0 - no recalculate system; 1 - NEED_GEM_AIA; 2 - NEED_GEM_SIA.
    int calc_mode=0;

    /// Setup for exporting T look-up arrays
    std::vector<double> Tai;

    /// Setup for exporting P look-up arrays
    std::vector<double> Pai;

    /// Number of allocated nodes
    long int nIV = 1;

    /// Write IPM, DCH and DBR files in binary, txt or json mode)
    std::string io_mode = "-j";

    /// Do not write data items that contain only default values
    bool brief_mode = false;

    /// Write files with comments for all data entries ( in text mode )
    bool with_comments = false;

    /// Print internal indices in RMULTS to IPM file for reading into Gems back
    bool add_mui = false;

    /// Prints files for separate coupled FMT-GEM programs that use GEMS3K module
    /// or if putNodT1 == true  as a break point for the running FMT calculation
    bool putNodT1 = false;
};

void show_usage(const std::string &name);
int extract_args(int argc, char* argv[], std::string& projects_conf, std::string& export_dir);
GEMS3KGenerator::IOModes get_mode(const std::string &str_mode);
void CurrentSystem2GEMS3K(const std::string& filepath, const ExportTask& settings);
void System2GEMS3K(const std::string& key, const std::string& files_dir, const ExportTask& settings);


// -d -s . -p template_export.json
int main(int argc, char *argv[])
{
    std::string projects_conf;
    std::string export_dir{"gems3k"};

    try {

        if(extract_args( argc, argv, projects_conf, export_dir )) {
            return 1;
        }


        // init visor data
        auto pvisor_sh = std::make_shared<TVisor>(argc, argv);
        pVisor = pvisor_sh.get();
        pVisor->Setup();

        // loop from config file
        ExportTask top_settings;
        TJsonConfig cnf(projects_conf);
        gui_logger->info("Export: {}", cnf.value_or_default<std::string>("comment", "gems3k export"));

        std::string project_key;
        top_settings.calc_mode = cnf.value_or_default<int>("calc_mode", top_settings.calc_mode);
        top_settings.io_mode = cnf.value_or_default<std::string>("io_mode", top_settings.io_mode);
        top_settings.brief_mode = cnf.value_or_default<bool>("brief_mode", top_settings.brief_mode);
        top_settings.with_comments = cnf.value_or_default<bool>("with_comments", top_settings.with_comments);

        for(const auto& project_conf: cnf.section("projects")->to_vector()) {
            project_key = project_conf.value_must_exist<std::string>("key");
            gui_logger->info("Project : {}", project_key);

            // read project
            pVisor->ProfileMode = MDD_SYSTEM;
            if(!TProfil::pm->initCalcMode(project_key.c_str())) {
                pVisor->ProfileMode = MDD_DATABASE;
                std::cout << "Error when read project: " << project_key;
                gui_logger->error("Error when read project: ", project_key);
                return 1;
            }

            TMulti::sm->GetPM()->pIPN = 0;
            auto system_section = project_conf.section("systems");
            if(system_section) {

                for(const auto& system_conf: system_section->to_vector()) {

                    ExportTask system_settings;
                    system_settings.system_record = system_conf.value_must_exist<std::string>("key");
                    gui_logger->info("System : {}", system_settings.system_record);

                    system_settings.calc_mode = system_conf.value_or_default<int>("calc_mode", top_settings.calc_mode);
                    system_settings.io_mode = system_conf.value_or_default<std::string>("io_mode", top_settings.io_mode);
                    system_settings.brief_mode = system_conf.value_or_default<bool>("brief_mode", top_settings.brief_mode);
                    system_settings.with_comments = system_conf.value_or_default<bool>("with_comments", top_settings.with_comments);

                    auto tai_section = system_conf.section("Tai");
                    if( tai_section ) {
                        for(const auto& tai_conf: tai_section->to_vector()) {
                            system_settings.Tai.push_back(tai_conf.get_as<double>());
                        }
                    }
                    auto pai_section = system_conf.section("Pai");
                    if( pai_section ) {
                        for(const auto& pai_conf: pai_section->to_vector()) {
                            system_settings.Pai.push_back(pai_conf.get_as<double>());
                        }
                    }
                    // export system
                    System2GEMS3K(system_settings.system_record, export_dir, system_settings);
                }
            }
            else {
                // Save all systems
                TCStringArray saved_systems;
                vfMakeDirectory(nullptr, export_dir.c_str(), 2);
                TProfil::pm->allSystems2GEMS3K(saved_systems, true, export_dir, top_settings.brief_mode, false);
            }
        }

        pVisor->CanClose();
        return 0;
    }
    catch(TError& err) {
        std::cout  << err.title << err.mess << std::endl;
        gui_logger->error("Export error: {}", err.mess);
    }
    catch(std::exception& e) {
        std::cout  << "std::exception: " << e.what() << std::endl;
        gui_logger->error("std::exception: {}", e.what());
    }
    catch(...)  {
        std::cout  << "unknown exception" << std::endl;
        gui_logger->error("unknown exception");
    }
    //pVisor->CanClose();
    return 1;
}


GEMS3KGenerator::IOModes get_mode( const std::string &str_mode )
{
    GEMS3KGenerator::IOModes io_mode = GEMS3KGenerator::f_key_value;
    if( str_mode == "-b" )
        io_mode = GEMS3KGenerator::f_binary;
    else  if( str_mode == "-j" )
        io_mode = GEMS3KGenerator::f_json;
    else  if( str_mode == "-f" || str_mode == "-fun" )
        io_mode = GEMS3KGenerator::f_thermofun;
    else  if( str_mode == "-o" || str_mode == "-fun-kv" )
        io_mode = GEMS3KGenerator::f_kv_thermofun;

    return io_mode;
}

void CurrentSystem2GEMS3K(const std::string& filepath, const ExportTask& settings)
{
    double Tai[4], Pai[4];

    if( settings.Tai.size()<4 || settings.Pai.size()<4 ) {
        MULTI *pmp = TMulti::sm->GetPM();
        Tai[0] = Tai[1] = pmp->TCc;
        Pai[0] = Pai[1] = pmp->Pc;
        Tai[2] = Pai[2] = 0.;
        Tai[3] = 1;
        Pai[3] = 0.5;
    }
    else {
        Tai[0] = settings.Tai[0];
        Tai[1] = settings.Tai[1];
        Tai[2] = settings.Tai[2];
        Tai[3] = settings.Tai[3];
        Pai[0] = settings.Pai[0];
        Pai[1] = settings.Pai[1];
        Pai[2] = settings.Pai[2];
        Pai[3] = settings.Pai[3];
    }

    auto na = TNodeArrayGUI::create(1, TMulti::sm);
    // realloc and setup data for dataCH and DataBr structures
    na->MakeNodeStructuresOne(nullptr, true, Tai, Pai);

    ProcessProgressFunction messageF = [filepath](const std::string&, long){
        gui_logger->info("GEM3k output: {} ", filepath);
        return false;
    };
    na->genGEMS3KInputFiles(filepath, messageF, 1, get_mode(settings.io_mode),
                            settings.brief_mode, settings.with_comments, settings.putNodT1, settings.add_mui);
}

void System2GEMS3K(const std::string& key, const std::string& files_dir, const ExportTask& settings)
{
    // test exists
    if(rt[RT_SYSEQ]->Find(key.c_str()) < 0) {
        gui_logger->error("System not exists: {}", key);
        return;
    }
    // get key in pack form
    std::string packkey = rt[RT_SYSEQ]->PackKey();
    // generate name and create directory
    auto systemname = packkey;
    KeyToName(systemname);
    systemname = settings.io_mode.back()+std::string("_")+systemname;
    std::string record_path = files_dir + systemname + "/";
    vfMakeDirectory( nullptr, record_path.c_str(), 0 );
    record_path += systemname+ "-dat.lst";

    TProfil::pm->loadSystat(packkey.c_str());
    //  Do we need recalculate system before
    if(settings.calc_mode)  {
        MULTI *pmp = TMulti::sm->GetPM();
        double dTime=0.; int kTimeStep =0; double kTime=0.;

        if(settings.calc_mode == 2) //NEED_GEM_SIA
            pmp->pNP = 1;
        else
            pmp->pNP = 0; //  NEED_GEM_AIA;

        TProfil::pm->CalcEqstat(dTime, kTimeStep, kTime);
    }
    CurrentSystem2GEMS3K(record_path, settings);
}

void show_usage(const std::string &name)
{
    std::cout << "Usage: " << name << " [ option(s) ] -p|--projects-config JSON_FILE -e|--export-path FOLDER "
              << "\nExport database projects to IPM, DCH and DBR files\n"
              << "Options:\n"
              << "\t-h,\t--help  \t\tshow this help message\n\n"
              // file path
              << "\t-s,\t--system-dir   FOLDER   \tpath for Resources \n"
              << "\t-u,\t--user-dir     FOLDER   \tGEMS projects location \n\n"
              // run mode
              << "\t-d,\t--from-ini-files        \tremake DOD and module dialog configurators (default false) \n"
              << "\t-c,\t--with-default-config   \tfile configuration if project subfolder(s) were added/removed (default true) \n"
              << "\t-n,\t--no-with-default-config   \t use file configuration if project subfolder(s) (default false) \n"
              //              << "\t-v,\t--with-default-settings \trestore default settings (default false) \n"
              //              << "\t-f,\t--allow-db-change       \twrite access to database files in \\DB.default\\ (default false) \n\n"
              << std::endl;
}


int extract_args(int argc, char* argv[], std::string& projects_conf, std::string& export_dir)
{
    int i=0;
    for(i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if((arg == "-h") || (arg == "--help")) {
            show_usage( "gems3k-export" );
            return 1;
        }
        else if((arg == "-p") || (arg == "--projects-config")) {
            if(i + 1 < argc) {
                projects_conf = argv[++i];
            } else {
                std::cerr << "--projects-config option requires one argument." << std::endl;
                return 1;
            }
        }
        else if((arg == "-e") || (arg == "--export-path")) {
            if(i + 1 < argc) {
                export_dir = argv[++i];
            } else {
                std::cerr << "--export-path option requires one argument." << std::endl;
                return 1;
            }
        }
    }
    if(projects_conf.empty()) {
        std::cerr << "Undefined projects config JSON file path" << std::endl;
        return 1;
    }
    if(!vfExist(projects_conf)) {
        std::cerr << "Do not exist config JSON file: " << projects_conf << std::endl;
        return 1;
    }
    if(export_dir.back() != '/') {
        export_dir += "/";
    }
    return 0;
}
