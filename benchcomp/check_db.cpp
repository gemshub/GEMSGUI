#include <iostream>
#include <spdlog/fmt/fmt.h>
#include <spdlog/fmt/ranges.h>
#include "visor.h"
#include "service.h"


int check_current(TCModule* mod, int nfile);
void show_usage(const std::string &name);
int extract_args(int argc, char* argv[], bool& only_special_files, std::string& folder);

// -d -s .
int main(int argc, char *argv[])
{
    // Check only default database
    bool only_special_files{false};
    // Check only in directory
    std::string check_folder{};
    // return code
    int ret =0;
    std::shared_ptr<TVisor> pvisor_sh;

    try {

        if(extract_args( argc, argv, only_special_files, check_folder )) {
            return 1;
        }

        // init visor data
        pvisor_sh = std::make_shared<TVisor>(argc, argv);
        pVisor = pvisor_sh.get();
        pVisor->Setup();

        if(check_folder.empty()) {
            gui_logger->info("Check {} ", (only_special_files? "DB.default" : "all files" ));

            for(size_t ii=0; ii<aMod.size(); ++ii)  {
                if(aMod[ii]->IsSubModule()) {
                    continue;
                }
                int num_of_files = (only_special_files? rt[aMod[ii]->rtNum()]->specialFilesNum : rt[aMod[ii]->rtNum()]->numFiles());
                for(int ff=0; ff<num_of_files; ++ff) {
                    ret += check_current(aMod[ii].get(), ff);
                }
            }
        }
        else {
            gui_logger->info("Check: {}", check_folder);
            auto files = vfFiles(check_folder, ".pdb");

            for(size_t ii=0; ii<aMod.size(); ++ii)  {
                if(aMod[ii]->IsSubModule()) {
                    continue;
                }
                auto *db = rt[aMod[ii]->rtNum()].get();
                for(const auto& file: files) {
                    std::string flnm = std::string(file, 0, file.find("."));
                    if(flnm == db->GetKeywd()) {
                        std::string path = check_folder+"/"+file;
                        db->AddFile(path.c_str());
                        ret += check_current(aMod[ii].get(), db->numFiles()-1);
                    }
                }
            }
        }

        pVisor->CanClose();
        return ret;
    }
    catch(TError& err) {
        std::cout  << err.title << err.mess << std::endl;
        gui_logger->error("Internal error: {}", err.mess);
    }
    catch(std::exception& e) {
        std::cout  << "std::exception: " << e.what() << std::endl;
        gui_logger->error("std::exception: {}", e.what());
    }
    catch(...)  {
        std::cout  << "unknown exception" << std::endl;
        gui_logger->error("unknown exception");
    }
    if(pvisor_sh) {
        pvisor_sh->CanClose();
    }
    return -1;
}

int check_current(TCModule* mod, int nfile)
{
    int ret = 0; // no errors in db
    TCStringArray names;
    TCIntArray indx;
    TCIntArray sel;

    TDataBase* db = rt[mod->rtNum()].get();
    db->Close();
    db->Open(true, UPDATE_DBV, {nfile});
    db->SetKey(ALLKEY);

    // check current file name
    db->GetFileList(openf, names, indx, sel);
    gui_logger->info("File {} {}", fmt::join(names, " "), db->RecCount());

    // get all record keys
    db->GetKeyList("*", names, indx);
    //gui_logger->info("Records {}", fmt::join(names, " "));

    for(size_t ii=0; ii<indx.size(); ++ii) {

        try {
            gui_logger->debug("Record {} {}", indx[ii], names[ii]);
            // Try read
            db->Get(indx[ii]);
            // Check the difference between the dynamic object size and the defined sizes
            if(!mod->ods_check()) {
                gui_logger->error("{} illegal object size", names[ii]);
                ret = 1;
            }
        }
        catch(TError& err) {
            ret = 1;
            gui_logger->error("{} error: {}", names[ii], err.mess);
        }
        catch(std::exception& e) {
            ret = 2;
            gui_logger->error("{} std::exception: {}", names[ii], e.what());
        }
        catch(...)  {
            ret = 2;
            gui_logger->error("{} unknown exception ", names[ii]);
        }
    }
    return ret;
}

void show_usage(const std::string &name)
{
    std::cout << "Usage: " << name << " [ option(s) ] "
              << "\nExport database projects to IPM, DCH and DBR files\n"
              << "Options:\n"
              << "\t-h,\t--help  \t\tshow this help message\n\n"
              << "\t-k,\t--only_kernel   \tcheck only DB.default \n"
              << "\t-f,\t--new-folder  FOLDER   \tpath for folder with new files \n\n"
              // file path
              << "\t-s,\t--system-dir   FOLDER   \tpath for Resources \n"
              << "\t-u,\t--user-dir     FOLDER   \tGEMS projects location \n\n"
              // run mode
              << "\t-d,\t--from-ini-files        \tremake DOD and module dialog configurators (default false) \n"
              << "\t-c,\t--with-default-config   \tfile configuration if project subfolder(s) were added/removed (default true) \n"
              << "\t-n,\t--no-with-default-config    \t use file configuration if project subfolder(s) (default false) \n"
              //              << "\t-v,\t--with-default-settings \trestore default settings (default false) \n"
              //              << "\t-f,\t--allow-db-change       \twrite access to database files in \\DB.default\\ (default false) \n\n"
              << std::endl;
}


int extract_args(int argc, char* argv[], bool& only_special_files, std::string& folder)
{
    for(int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if((arg == "-h") || (arg == "--help")) {
            show_usage( "check_db" );
            return 1;
        }
        else if((arg == "-k") || (arg == "--only_kernel")) {
            only_special_files = true;
        }
        else if((arg == "-f") || (arg == "--new-folder")) {
            if(i + 1 < argc) {
                folder = argv[++i];
            } else {
                std::cerr << "--new-folder option requires one argument." << std::endl;
                return 1;
            }
        }
    }
    return 0;
}
