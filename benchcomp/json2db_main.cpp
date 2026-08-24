//-------------------------------------------------------------------
// json2db: proof-of-concept standalone (non-GUI) tool that creates a
// new GEM-Selektor .pdb/.ndx database file pair for a given keyword
// from a JSON array of records, reusing the same TDataBase/TCModule
// machinery the GUI's "Import from JSON" action uses internally.
//-------------------------------------------------------------------
#include <iostream>
#include <map>

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "visor.h"
#include "service.h"
#include "v_dbfile.h"

namespace {

// Keyword -> RT_* index (see Modules/v_mod.h). aMod[]/rt[] are both indexed
// by this same RT_* constant (GUI/Services4/visor.cpp's initModules()/
// defaultCFG() push modules and their DB files in RT_* order).
const std::map<std::string, int> kKeywordToRT = {
    {"icomp",  RT_ICOMP},
    {"dcomp",  RT_DCOMP},
    {"phase",  RT_PHASE},
    {"reacdc", RT_REACDC},
    {"sdref",  RT_SDATA},
};

void show_usage(const std::string& name)
{
    std::cout << "Usage: " << name << " -j JSON_FILE -k KEYWORD [-t TAG] [-o OUTPUT_DIR] [-s SYSTEM_DIR] [-u USER_DIR]\n"
              << "\nProof of concept: creates a new <KEYWORD>.<TAG>.pdb/.ndx database file pair\n"
              << "from a JSON array of records, with no GUI involved.\n\n"
              << "Options:\n"
              << "  -j, --json FILE      JSON file with an array of records (required)\n"
              << "  -k, --keyword NAME   Target keyword: icomp, dcomp, phase, reacdc, sdref (required)\n"
              << "  -o, --output DIR     Output directory for the new .pdb/.ndx pair (default: ./out)\n"
              << "  -t, --tag TAG        Everything the real GEM-Selektor DB files put between the\n"
              << "                       keyword and the extension, dot(s) and all -- e.g. for\n"
              << "                       icomp.THEREDA.ver01-2026-r17.pdb pass -t THEREDA.ver01-2026-r17\n"
              << "                       (default: poc). Same option as the older -n/--name.\n"
              << "  -s, --system-dir DIR GEM-Selektor Resources directory (parsed by TVisor)\n"
              << "  -u, --user-dir DIR   GEM-Selektor user profile directory (parsed by TVisor)\n"
              << "  -h, --help           show this help message\n";
}

} // namespace

int main(int argc, char* argv[])
{
    std::string jsonFile;
    std::string outDir = "out";
    std::string name = "poc";
    std::string keyword;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            show_usage("json2db");
            return 0;
        } else if ((arg == "-j" || arg == "--json") && i + 1 < argc) {
            jsonFile = argv[++i];
        } else if ((arg == "-o" || arg == "--output") && i + 1 < argc) {
            outDir = argv[++i];
        } else if ((arg == "-n" || arg == "--name" || arg == "-t" || arg == "--tag") && i + 1 < argc) {
            name = argv[++i];
        } else if ((arg == "-k" || arg == "--keyword") && i + 1 < argc) {
            keyword = argv[++i];
        }
        // -s/-u are consumed by TVisor's own constructor below, further down.
    }

    if (jsonFile.empty() || keyword.empty()) {
        std::cerr << "Missing required -j JSON_FILE and/or -k KEYWORD argument\n";
        show_usage("json2db");
        return 1;
    }
    auto rtIt = kKeywordToRT.find(keyword);
    if (rtIt == kKeywordToRT.end()) {
        std::cerr << "Unknown keyword '" << keyword << "'. Supported:";
        for (const auto& kv : kKeywordToRT)
            std::cerr << " " << kv.first;
        std::cerr << std::endl;
        return 1;
    }
    int rtIndex = rtIt->second;
    if (!outDir.empty() && outDir.back() == '/')
        outDir.pop_back();

    try {
        // Boots the same schema/config machinery the GUI uses (loads the
        // object table, registers modules, attaches whatever DB files
        // already exist) but with no QApplication/QMainWindow involved
        // (this target is built with -DNO_GUI, see benchcomp/CMakeLists.txt).
        auto pvisor_sh = std::make_shared<TVisor>(argc, argv);
        pVisor = pvisor_sh.get();
        pVisor->Setup();

        TDataBase* db = rt[rtIndex].get();
        TCModule* mod = aMod[rtIndex].get();

        // askOverwrite=0: only create the directory if missing. Passing a
        // nonzero value here wipes it first (fs::remove_all in service_new.cpp)
        // -- fatal for this tool since multiple keywords share one output dir.
        vfMakeDirectory(nullptr, outDir.c_str(), 0);
        std::string path = outDir + "/" + db->GetKeywd() + "." + name + "." + PDB_EXT;

        // Attach a brand-new, empty .pdb/.ndx pair to the target database.
        // AddFileToList() creates both files on disk (TDataBase::Create())
        // and returns the file's slot ("fnum") for direct, non-interactive
        // use below -- this is what lets AddRecordToFile() skip the
        // GUI's "choose a database file" dialog entirely.
        int fnum = db->AddFileToList(new TDBFile(path));
        std::cout << "Created " << path << " (and matching .ndx)" << std::endl;

        QFile jf(QString::fromStdString(jsonFile));
        if (!jf.open(QIODevice::ReadOnly)) {
            std::cerr << "Cannot open JSON file: " << jsonFile << std::endl;
            return 1;
        }
        QJsonArray records = QJsonDocument::fromJson(jf.readAll()).array();

        int written = 0;
        for (const auto& val : records) {
            mod->dyn_kill();
            mod->set_def(); // defaults, mirrors TCModule::RecListFromJSON()
            std::string key = db->fromJsonObjectNew(val.toObject());
            int Rnum = db->Find(key.c_str());
            if (Rnum >= 0)
                db->Rep(Rnum);
            else
                db->AddRecordToFile(key.c_str(), fnum);
            ++written;
        }
        mod->dyn_set();
        db->Close();

        std::cout << "Wrote " << written << " record(s) from " << jsonFile
                   << " into " << path << std::endl;
        pVisor->CanClose();
        return 0;
    }
    catch (TError& err) {
        std::cerr << err.title << ": " << err.mess << std::endl;
    }
    catch (std::exception& e) {
        std::cerr << "std::exception: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "unknown exception" << std::endl;
    }
    return 1;
}
