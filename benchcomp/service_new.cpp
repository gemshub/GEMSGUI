#ifndef _WIN32
#include <unistd.h>
#else
#include <io.h>
#endif
#include <iostream>

#include "service.h"
#include "visor.h"


bool vfExist(const std::string &file_path)
{
    return !(::access(file_path.c_str(), 0 ));
}

int vfQuestYesNoCancel(QWidget* par, const std::string& title, const std::string& mess)
{
    std::cout << "vfQuestYesNoCancel: " << title << " " << mess << std::endl;
    return 0;
}

bool vfQuestion(QWidget* par, const std::string& title, const std::string& mess)
{
    std::cout << "vfQuestion: " << title << " " << mess << std::endl;
    return false;
}

// returns VF3_1, VF3_2 or VF3_3
int vfQuestion3(QWidget* par, const std::string& title, const std::string& mess,
                const std::string& s1, const std::string& s2,  const std::string& s3, bool i_mov)
{
    std::cout << "vfQuestion3: " << title << " " << mess << std::endl;
    return 0;
}

int vfQuestionYesNoAll(QWidget* par, const std::string& title, const std::string& mess, const std::string& s1)
{
    std::cout << "vfQuestionYesNoAll: " << title << " " << mess << std::endl;
    return 0;
}

void vfMessage(QWidget* par, const std::string& title, const std::string& mess, WarnType type)
{
    std::cout << "vfMessage: " << title << " " << mess << std::endl;
}

int vfChoice(QWidget* par, TCStringArray& arr, const char* prompt, int sel)
{
    std::cout << "vfChoice: " << prompt << " " << arr.size() << std::endl;
    return 0;
}

int vfChoice2(QWidget* par, TCStringArray& arr, const char* prompt, int sel, bool& all_)
{
    std::cout << "vfChoice2: " << prompt << " " << arr.size() << std::endl;
    return 0;
}

int vfChoice(QWidget* par, const char* title, const char* prompt,
             int nVal, int *arr, int sel)
{
    std::cout << "vfChoice3: " << title << " " << prompt << std::endl;
    return 0;
}

TCIntArray vfMultiChoice(QWidget* par, TCStringArray& arr, const char* prompt)
{
    std::cout << "vfMultiChoice: " << prompt << " " << arr.size() << std::endl;
    return {};
}

TCIntArray vfMultiChoiceSet(QWidget* par, TCStringArray& arr, const char* prompt, TCIntArray& sel)
{
    std::cout << "vfMultiChoiceSet: " << prompt << " " << arr.size() << std::endl;
    return {};
}

TCStringArray vfMultiKeys(QWidget* par, const char* caption, unsigned int iRt, const char* key )
{
    std::cout << "vfMultiKeys: " << caption << " " << key << std::endl;
    return {};
}

TCStringArray vfMultiKeysSet(QWidget* par, const char* caption,
                             unsigned int iRt, const char* key, TCStringArray& sel )
{
    std::cout << "vfMultiKeysSet: " << caption << " " << key << std::endl;
    return {};
}
TCStringArray vfRDMultiKeysSet(QWidget* par, const char* caption,
                               const char* key, TCStringArray& sel, short NsuT )
{
    std::cout << "vfRDMultiKeysSet: " << caption << " " << key << std::endl;
    return {};
}

bool vfListFiles(QWidget* par, bool show_dlg, const char * prfName,
                 TCStringArray& fls, TCIntArray& cnt )
{
    std::cout << "vfListFiles: " << prfName << " " << show_dlg << std::endl;
    return false;
}

std::string vfKeyEdit(QWidget* par, const char* title, unsigned int iRt, const char* key)
{
    std::cout << "vfKeyEdit: " << title << " " << key << std::endl;
    return "";
}

std::string vfKeyProfile( QWidget* par, const char* caption, int iRt,
                         bool& chAqGas, bool& addFiles, bool& remake,
                         std::string& key_templ,
                         int& recalc_all, int& genGEMS3k, int& makeCalc)
{
    std::cout << "vfKeyProfile: " << caption << " " << iRt << std::endl;
    return "";
}


std::string vfKeyTemplEdit(QWidget* par, const char* title, unsigned int iRt, const char* key,
                           bool allowTemplate)
{
    std::cout << "vfKeyTemplEdit: " << title << " " << iRt << std::endl;
    return "";
}

bool vfKeyCanged(QWidget* par, const char* caption,
                 std::string& from_str, std::string& to_str, int fldLen )
{
    std::cout << "vfKeyCanged: " << caption << " " << to_str << std::endl;
    return "";
}

bool vfExcludeFillEdit(QWidget* par, const char* caption,
                       TCStringArray& aICkeys, std::vector<bool>& sel, double& fill_data )
{
    std::cout << "vfExcludeFillEdit: " << caption << " " << aICkeys.size() << std::endl;
    return "";
}

bool vfElements(QWidget* par, const char * prfName,
                elmWindowData& elm_data, setFiltersData& sf_data )
{
    std::cout << "vfElements: " << prfName << std::endl;
    return false;
}

bool vfSystemInput(QWidget* par, const char * p_key,
                   std::vector<windowSetupData>& wnData, std::vector<tableSetupData>& tbData,
                   std::vector<pagesSetupData>& scalarsList )
{
    std::cout << "vfSystemInput: " << p_key << std::endl;
    return false;
}

bool vfProcessSet(QWidget* par, const char * p_key,
                  char flds[24], int size[8], short tabInt[6], double tabDoubl[24],
                  std::string& calcScript, std::string& outScript, TCStringArray& names,
                  std::string& xName, std::string& yName )
{
    std::cout << "vfProcessSet: " << p_key << std::endl;
    return false;
}

bool vfGEM2MTSet(QWidget* par, const char * p_key,
                 char flds[32], int size[20],  double Tai[4], double Pai[4],double Tau[3],
                 std::string& calcScript, std::string& outScript, TCStringArray& names,
                 std::string& xName, std::string& yName, TCIntArray& vtk1, TCIntArray& vtk2 )
{
    std::cout << "vfGEM2MTSet: " << p_key << std::endl;
    return false;
}

bool vfUnSpaceSet(QWidget* par, const char * p_key, char flds[38], int size[10] )
{
    std::cout << "vfUnSpaceSet: " << p_key << std::endl;
    return false;
}

bool vfGtDemoSet(QWidget* par, const char * p_key, char flgs[16], int size[8],
                 std::string& prkey, std::string& script, TCStringArray& names,
                 std::string& xName, std::string& yName, TCStringArray& keys )
{
    std::cout << "vfGtDemoSet: " << p_key << std::endl;
    return false;
}

bool vfComposSet(QWidget* par, const char * p_key,
                 char flgs[6], int sizes[2], double& r2 )
{
    std::cout << "vfComposSet: " << p_key << std::endl;
    return false;
}

bool vfDCompSet(QWidget* par, const char * p_key,
                char flgs[15], int sizes[4] )
{
    std::cout << "vfDCompSet: " << p_key << std::endl;
    return false;
}

bool vfReacDCSet(QWidget* par, const char * p_key,
                 char flgs[12], int sizes[4], TCStringArray& sel )
{
    std::cout << "vfReacDCSet: " << p_key << std::endl;
    return false;
}

bool vfRTparmSet(QWidget* par, const char * p_key,
                 char flgs[10], int sizes[7], double val[6],
                 std::string& script, std::string& xName, std::string& yName, TCStringArray& names)
{
    std::cout << "vfRTparmSet: " << p_key << std::endl;
    return false;
}

bool vfPhaseSet( QWidget* par, const char * p_key,
                char flgs[37], int sizes[30], double& r2, TCStringArray& rd_sel,
                TCStringArray& phase_sel, TCStringArray& lDCr_sel )
{
    std::cout << "vfPhaseSet: " << p_key << std::endl;
    return false;
}

bool vfDualThSet(QWidget* par, const char * p_key, char flgs[20], int size[8] )
{
    std::cout << "vfDualThSet: " << p_key << std::endl;
    return false;
}

bool vfProjectSet(QWidget* par, const char * p_key, char flds[38], int&  taskset )
{
    std::cout << "vfProjectSet: " << p_key << std::endl;
    return false;
}

bool vfSystemSet(QWidget* par, const char * p_key,
                 char flgs[40],  std::string& name, std::string& comment, std::string& EQkey )
{
    std::cout << "vfSystemSet: " << p_key << std::endl;
    return false;
}

bool vfAutoPhaseSet(QWidget* par, const char* pr_key, std::string& a_key, std::string& g_key,
                    char& acode, char& gcode, float apar[8])
{
    std::cout << "vfAutoPhaseSet: " << pr_key << std::endl;
    return false;
}

bool vfLookupDialogSet(QWidget* wpar, char flags[6], double Tai[4], double Pai[4] )
{
    std::cout << "vfLookupDialogSet: " << flags << std::endl;
    return false;
}

void vfPhaseInfo(QWidget* wpar, bool system, int xph, std::string phname,
                 std::vector<int>& xdclist, std::vector<std::string>& dcnames, int xdc )
{
    std::cout << "vfPhaseInfo: " << phname << std::endl;
}


bool vfChooseFileOpen(QWidget* par, std::string& path,
                      const char* title, const char* filter)
{
    std::cout << "vfChooseFileOpen: " << title << std::endl;
    return false;
}

bool vfChooseFileSave(QWidget* par, std::string& path,
                      const char* title, const char* filter)
{
    std::cout << "vfChooseFileSave: " << path << std::endl;
    return false;
}

bool vfChooseDirectory(QWidget* par, std::string& path_, const char* title )
{
    std::cout << "vfChooseDirectory: " << title << std::endl;
    return false;
}

void vfMakeDirectory(QWidget* par, const char *dir, int askOverwrite)
{
    std::cout << "vfMakeDirectory: " << dir << std::endl;
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
