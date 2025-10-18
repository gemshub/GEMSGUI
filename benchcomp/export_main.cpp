#include <iostream>
#include "visor.h"
#include "m_param.h"


void show_usage(const std::string &name);
int extract_args(int argc, char* argv[], std::string& projects_conf, std::string& export_dir);


// -d -s .
int main(int argc, char *argv[])
{
    std::string projects_conf;
    std::string export_dir{"gems3k_export"};

    try {

        if(extract_args( argc, argv, projects_conf, export_dir )) {
            return 1;
        }

        auto pvisor_sh = std::make_shared<TVisor>(argc, argv);
        pVisor = pvisor_sh.get();
        pVisor->Setup();

        std::string profile_key = "Kaolinite Test-JNC                              ";
        if( !TProfil::pm->initCalcMode(profile_key.c_str()) )
        {
            pVisor->ProfileMode = MDD_DATABASE;
            std::cout << "Error when read project: " << profile_key;
            return 1;
        }


        // Generate data from process
        TCStringArray savedSystems;

        // Save systems
        std::string systemsPath = export_dir+"/";
        vfMakeDirectory(nullptr, systemsPath.c_str(), 2);
        TProfil::pm->allSystems2GEMS3K( savedSystems, true, systemsPath, true, false);

        pVisor->CanClose();
        return 0;
    }
    catch(TError& err)
    {
        std::cout  << err.title << err.mess << std::endl;
        gui_logger->error("Export error: {}", err.mess);
    }
    catch(std::exception& e)
    {
        std::cout  << "std::exception: " << e.what() << std::endl;
        gui_logger->error("std::exception: {}", e.what());
    }
    catch(...)
    {
        std::cout  << "unknown exception" << std::endl;
        gui_logger->error("unknown exception");
    }
    return -1;
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
              << "\t-c,\t--with-default-config   \tfile configuration if project subfolder(s) were added/removed (default false) \n"
//              << "\t-v,\t--with-default-settings \trestore default settings (default false) \n"
//              << "\t-f,\t--allow-db-change       \twrite access to database files in \\DB.default\\ (default false) \n\n"
              << std::endl;
}


int extract_args( int argc, char* argv[], std::string& projects_conf, std::string& export_dir)
{
    int i=0;
    for(i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if((arg == "-h") || (arg == "--help")) {
            show_usage( "export_gems3" );
            return 1;
        }
        else if((arg == "-p") || (arg == "--projects-config")) {
            if (i + 1 < argc) {
                projects_conf = argv[++i];
            } else {
                std::cerr << "--projects-config option requires one argument." << std::endl;
                return 1;
            }
        }
        else if ((arg == "-e") || (arg == "--export-path")) {
            if (i + 1 < argc) {
                export_dir = argv[++i];
            } else {
                std::cerr << "--export-path option requires one argument." << std::endl;
                return 1;
            }
        }
    }
    if( projects_conf.empty() ) {
        std::cerr << "Undefined projects config JSON file path" << std::endl;
       // return 1;
    }
    return 0;
}
