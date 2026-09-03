#include <filesystem>
namespace fs = std::filesystem;

#include "service.h"
#include "visor.h"

TCStringArray vfFiles(const std::string& dir_name, const std::string& ext) {
    TCStringArray files;
    fs::path dir_path(dir_name);
    for (const auto& entry : fs::directory_iterator(dir_path)) {
        if (entry.is_regular_file() && (ext.empty() || entry.path().extension() == ext)) {
            std::string file = entry.path().filename().string();
            gui_logger->trace("Adding file: {}", file);
            files.push_back(file);
        }
    }
    return files;
}


bool vfExist(const std::string &file_path)
{
    // return !(::access(file_path.c_str(), 0 ));
    fs::path ps(file_path);
    return fs::exists(ps);
}

int vfQuestYesNoCancel(QWidget*, const std::string& title, const std::string& mess)
{
    gui_logger->info("vfQuestYesNoCancel {} {}", title, mess);
    return 0;
}

bool vfQuestion(QWidget*, const std::string& title, const std::string& mess)
{
    gui_logger->info("vfQuestion {} {}", title, mess);
    return false;
}

// returns VF3_1, VF3_2 or VF3_3
int vfQuestion3(QWidget*, const std::string& title, const std::string& mess,
                const std::string&, const std::string&, const std::string&, bool )
{
    gui_logger->info("vfQuestion3 {} {}", title, mess);
    return 0;
}

int vfQuestionYesNoAll(QWidget*, const std::string& title, const std::string& mess, const std::string&)
{
    gui_logger->info("vfQuestionYesNoAll {} {}", title, mess);
    return 0;
}

void vfMessage(QWidget*, const std::string& title, const std::string& mess, WarnType)
{
    gui_logger->info("vfMessage {} {}", title, mess);
}

int vfChoice(QWidget*, TCStringArray& arr, const char* prompt, int)
{
    gui_logger->info("vfChoice {} {}", prompt, arr.size());
    return 0;
}

int vfChoice2(QWidget*, TCStringArray& arr, const char* prompt, int, bool&)
{
    gui_logger->info("vfChoice2 {} {}", prompt, arr.size());
    return 0;
}

int vfChoice(QWidget*, const char* title, const char* prompt, int nVal, int *, int)
{
    gui_logger->info("vfChoice3 {} {} {}", title, prompt, nVal);
    return 0;
}

TCIntArray vfMultiChoice(QWidget*, TCStringArray& arr, const char* prompt)
{
    gui_logger->info("vfMultiChoice {} {}", prompt, arr.size());
    return {};
}

TCIntArray vfMultiChoiceSet(QWidget*, TCStringArray& arr, const char* prompt, TCIntArray&)
{
    gui_logger->info("vfMultiChoiceSet {} {}", prompt, arr.size());
    return {};
}

TCStringArray vfMultiKeys(QWidget*, const char* caption, unsigned int iRt, const char* key)
{
    gui_logger->info("vfMultiKeys {} {} {}", caption, key, iRt);
    return {};
}

TCStringArray vfMultiKeysSet(QWidget*, const char* caption,
                             unsigned int iRt, const char* key, TCStringArray&)
{
    gui_logger->info("vfMultiKeysSet {} {} {}", caption, key, iRt);
    return {};
}

TCStringArray vfRDMultiKeysSet(QWidget*, const char* caption,
                               const char* key, TCStringArray&, short)
{
    gui_logger->info("vfRDMultiKeysSet {} {}", caption, key);
    return {};
}

bool vfListFiles(QWidget*, bool show_dlg, const char* prfName,
                 TCStringArray&, TCIntArray&)
{
    gui_logger->info("vfListFiles {} {}", prfName, show_dlg);
    return false;
}

std::string vfKeyEdit(QWidget*, const char* title, unsigned int iRt, const char* key)
{
    gui_logger->info("vfKeyEdit {} {} {}", title, iRt, key);
    return "";
}

std::string vfKeyProfile(QWidget*, const char* caption, int iRt,
                         bool&, bool&, bool&, std::string&, int&, int&, int&)
{
    gui_logger->info("vfKeyProfile {} {}", caption, iRt);
    return "";
}


std::string vfKeyTemplEdit(QWidget*, const char* title, unsigned int iRt, const char* key, bool)
{
    gui_logger->info("vfKeyTemplEdit {} {} {}", title, iRt, key);
    return "";
}

bool vfKeyCanged(QWidget*, const char* caption, std::string& from_str, std::string& to_str, int)
{
    gui_logger->info("vfKeyCanged {} {} {}", caption, from_str, to_str);
    return false;
}

bool vfExcludeFillEdit(QWidget*, const char* caption, TCStringArray& aICkeys, std::vector<bool>&, double&)
{
    gui_logger->info("vfExcludeFillEdit {} {}", caption, aICkeys.size());
    return false;
}

bool vfElements(QWidget*, const char* prfName, elmWindowData&, setFiltersData&)
{
    gui_logger->info("vfElements {}", prfName);
    return false;
}

bool vfSystemInput(QWidget*, const char* p_key, std::vector<windowSetupData>&,
                   std::vector<tableSetupData>&, std::vector<pagesSetupData>&)
{
    gui_logger->info("vfSystemInput {}", p_key);
    return false;
}

bool vfProcessSet(QWidget*, const char* p_key, char [24], int [8], short [6], double [24],
                  std::string&, std::string&, TCStringArray&,  std::string&, std::string&)
{
    gui_logger->info("vfProcessSet {}", p_key);
    return false;
}

bool vfGEM2MTSet(QWidget*, const char* p_key, char [32], int [20], double [4], double [4], double [3],
                 std::string&, std::string&, TCStringArray&, std::string&, std::string&, TCIntArray&, TCIntArray&)
{
    gui_logger->info("vfGEM2MTSet {}", p_key);
    return false;
}

bool vfUnSpaceSet(QWidget*, const char* p_key, char [38], int [10])
{
    gui_logger->info("vfUnSpaceSet {}", p_key);
    return false;
}

bool vfGtDemoSet(QWidget*, const char* p_key, char [16], int [8], std::string&, std::string&,
                 TCStringArray&, std::string&, std::string&, TCStringArray&)
{
    gui_logger->info("vfGtDemoSet {}", p_key);
    return false;
}

bool vfComposSet(QWidget*, const char* p_key, char [6], int [2], double&)
{
    gui_logger->info("vfComposSet {}", p_key);
    return false;
}

bool vfDCompSet(QWidget*, const char* p_key, char [15], int [4])
{
    gui_logger->info("vfDCompSet {}", p_key);
    return false;
}

bool vfReacDCSet(QWidget*, const char* p_key, char [12], int [4], TCStringArray&)
{
    gui_logger->info("vfReacDCSet {}", p_key);
    return false;
}

bool vfRTparmSet(QWidget*, const char* p_key, char [10], int [7], double [6],
                 std::string&, std::string&, std::string&, TCStringArray&)
{
    gui_logger->info("vfRTparmSet {}", p_key);
    return false;
}

bool vfPhaseSet(QWidget*, const char* p_key, char [37], int [30], double&,
                TCStringArray&, TCStringArray&, TCStringArray&)
{
    gui_logger->info("vfPhaseSet {}", p_key);
    return false;
}

bool vfDualThSet(QWidget*, const char* p_key, char [20], int [8])
{
    gui_logger->info("vfDualThSet {}", p_key);
    return false;
}

bool vfProjectSet(QWidget*, const char* p_key, char [38], int&)
{
    gui_logger->info("vfProjectSet {}", p_key);
    return false;
}

bool vfSystemSet(QWidget*, const char* p_key, char [40], std::string&, std::string&, std::string&)
{
    gui_logger->info("vfSystemSet {}", p_key);
    return false;
}

bool vfAutoPhaseSet(QWidget*, const char* pr_key, std::string& a_key,
                    std::string& g_key, char&, char&, float [8])
{
    gui_logger->info("vfAutoPhaseSet {} {} {}", pr_key, a_key, g_key);
    return false;
}

bool vfLookupDialogSet(QWidget*, char flags[6], double [4], double [4])
{
    gui_logger->info("vfSystemSet {}", flags);
    return false;
}

void vfPhaseInfo(QWidget*, bool, int, std::string phname,
                 std::vector<int>&, std::vector<std::string>&, int)
{
    gui_logger->info("vfPhaseInfo {}", phname);
}


bool vfChooseFileOpen(QWidget*, std::string&, const char* title, const char* filter)
{
    gui_logger->info("vfChooseFileOpen {} {}", title, filter);
    return false;
}

bool vfChooseFileSave(QWidget*, std::string&, const char* title, const char* filter)
{
    gui_logger->info("vfChooseFileSave {} {}", title, filter);
    return false;
}

bool vfChooseDirectory(QWidget*, std::string&, const char* title)
{
    gui_logger->info("vfChooseDirectory {}", title);
    return false;
}

void vfMakeDirectory(QWidget* , const char *dir, int askOverwrite)
{
    fs::path ps(dir);
    if (fs::exists(ps))  {
        if(askOverwrite)  {
            fs::remove_all(ps);
        }
    }
    fs::create_directories(ps);
}

const char * dfAqKeyD =  "a   AQELIA  aq_gen          aq  Davies          ";
const char * dfAqKeyH =  "a   AQELIA  aq_gen          aq  EDH_H           ";
const char * dfAqKey3 =  "a   AQELIA  aq_gen          aq  EDH_K           ";
const char * dfAqKey2 =  "a   AQELIA  aq_gen          aq  DH_K            ";
const char * dfAqKey1 =  "a   AQELIA  aq_gen          aq  DH_LL           ";
const char * dfAqKeyY =  "a   AQELIA  aq_gen          aq  EDH_Y           ";
const char * dfAqKeyU =  "a   AQELSI  aq_spec         aq  User-Provided   ";
const char * dfGasKey =  "g   GASMXID gas_gen         gm  Ideal           ";
const char * dfFluKeyF=  "f   CG      fluid_gen       gm  CG_EoS          ";
const char * dfFluKey7=  "f   PR      fluid_gen       gm  PR_EoS          ";
const char * dfFluKeyP=  "f   PRSV    fluid_gen       gm  PRSV_EoS        ";
const char * dfFluKeyE=  "f   SRK     fluid_gen       gm  SRK_EoS         ";
const char * dfFluKeyU = "f   FLUID   fluid_spec      gm  User-Provided   ";

//----------------------------------------------------------------
// Help file entry point constants

const char *WZSTEP = "WZSTEP";
const char *MWPAGE = "MWPAGE";
const char *DBM = "DBM";
const char *NUMSET = "NUMSET";

// modules
const char *GM_COMPOS_WZ_HTML = "gm_compos";
const char *GM_COMPOS_HTML = "gm_compos";
const char *GM_ICOMP_HTML = "gm_icomp";          //m_icomp.cpp
const char *GM_DCOMP_HTML = "gm_dcomp";          //m_dcomp.cpp
const char *GM_DCOMP_WZ_HTML = "gm_dcomp";       //DCompWizard
const char *GM_REACDC_HTML = "gm_reacdc";        //m_reacdc.cpp
const char *GM_REACDC_WZ_HTML = "gm_reacdc";     //ReacDCWizard
const char *GM_RTPARM_HTML = "gm_rtparm";        //m_rtparm.cpp
const char *GM_RTPARM_WZ_HTML = "gm_rtparm";     //RTparmWizard
const char *GM_PHASE_HTML = "gm_phase";          //m_phase.cpp
const char *GM_PHASE_WZ_HTML = "gm_phase";       //PhaseWizard
const char *GM_SDREF_HTML = "gm_sdref";          //m_sdata.cpp
const char *GM_CONST_HTML = "gm_const";          //m_const.cpp
const char *GM_PROJECT_HTML = "gm_project";      //m_param.cpp
const char *GM_PROJECT_WZ_HTML = "gm_project";   //ProjectWizard
const char *GEMS_NUMSET_HTML = "gm_project";     //m_param.cpp
const char *GM_SYSEQ_HTML = "gm_syseq";          //m_syseq.cpp
const char *GEMS_SYSTEM_WZ_HTML = "gems_bcc";    //SystemWizard
const char *GM_PROCES_HTML = "gm_proces";        //m_proces.cpp
const char *GM_PROCES_WZ_HTML = "gm_proces";     //ProcesWizard
const char *GM_GTDEMO_HTML = "gm_gtdemo";        //m_gtdemo.cpp
const char *GM_GTDEMO_WZ_HTML = "gm_gtdemo";     //GtDemoWizard
const char *GM_GEM2MT_HTML = "gm_gem2mt";        //m_gem2mt.cpp
const char *GM_GEM2MT_WZ_HTML = "gm_gem2mt";     //GEM2MTWizard
const char *GM_UNSPACE_HTML = "gm_unspace";      //m_unspace.cpp
const char *GM_UNSPACE_WZ_HTML = "gm_unspace";   //UnSpaceWizard
const char *GM_DUALTH_HTML = "gm_dualth";        //m_dualth.cpp
const char *GM_DUALTH_WZ_HTML = "gm_dualth";     //DualThWizard

const char *GSM_SYSTEM_HTML = "gems_bcc";        //ms_system.cpp
const char *GSM_MTPARM_HTML = "gems_systd";      //ms_mtparm.cpp
const char *GSM_EQDEMO_HTML = "gems_sys";        //ms_calc.h
const char *GSM_MULTI_HTML =  "gems_ipm";        //ms_calc.h

// dialogs
//const char *GEMS_ABOUT_HTML = "gems_about";       // MainDialog
const char *GEMS_TUTOR_HTML = "gems_tutorial";    // MainDialog
const char *GEMS_SETUP_HTML = "gems_setup";       // SettingsDialog
const char *GEMS_TDBAS_HTML = "gems_tdbase";      // DBDialog
const char *GEMS_MISCL_HTML = "gems_miscel";      // DBAdvDialog
const char *GEMS_FORMT_HTML = "gems_miscel.html#FORMATS";   // DBAdvDialog
const char *GEMS_REKEY_HTML = "gems_rekey";       // KeyFilter
const char *GEMS_MPROJ_HTML = "gm_project.html#HOWTO";    // KeyProfile
const char *GEMS_MPROJ_LF_HTML = "gm_project.html#FILES"; // ListFilesDialog
const char *GEMS_ONESYS_HTML = "gems_sys";        // NewSystemDialog
const char *GEMS_HOWTO_HTML = "gems_sys.html#HOWTO";
const char *GEMS_BCC_HTML = "gems_bcc";
const char *GEMS_HOWTOB_HTML = "gems_bcc.html#HOWTO";
const char *GEMS_IPM_HTML = "gems_ipm";
const char *GEMS_RECIPE_HTML = "gems_bcc.html#RECIPE"; // InputSystemDialog
const char *GEMS_GRAPH_HTML = "gems_graph"; // GraphDialog
const char *GEMS_GRAPH_CUS_HTML = "gems_graph.html#CUSTOMIZE"; // LegendDialog
const char *GEMS_MISSING_HTML = "gems_bcc.html#MISSING";  // ExcludeFillDialog
const char *GEMS_MODES_HTML = "gm_project.html#MODES";   //EQDialog
const char *GEMS_ELEMENTS_HTML = "gems_elements"; // ElementsDialog
const char *GEMS_SETFILT_HTML = "gems_elements.html#FILTERS"; // SetFiltersDialog
const char *GEMS_APHSETUP_HTML = "gems_aphsetup"; // AutoPhaseDialog
const char *GEMS_TEMPL_HTML = "gems_miscel.html#TEMPLATES";  // ChangeKeyDialog
const char *GEMS_PHINFO_HTML = "gems_miscel.html#PHASEINFO";  // PhaseInfoDialog
const char *GEMS_WRIPMF_HTML = "gems_ipm.html#WRITEIPMFILES"; // LookupDialog
const char *GEMS_MARK_HTML = "gems_miscel.html#MARK"; // SelectDialog
const char *GEMS_SELECT_HTML = "gems_miscel.html#SELECT"; // KeyDialog
const char *GEMS_CALCUL_HTML = "gems_miscel.html#CALCULATOR";  // CalcDialog
// MainDialog
const char *GEMS_HOWTOSTART_HTML = "gems_about.html#PAGE_HOWTOSTART";
//const char *GEMS_ABOUT_HTML = "gems_about.html#PAGE_ABOUT";
const char *GEMS_AUTHORS_HTML = "gems_about.html#PAGE_AUTHORS";
const char *GEMS_THANKS_HTML = "gems_about.html#PAGE_THANKS";
const char *GEMS_LICENSE_HTML = "gems_about.html#PAGE_LICENSE";
// old
const char *GEMS_TOC_HTML = "gems_toc"; // default for modules
