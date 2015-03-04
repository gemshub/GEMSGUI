//-------------------------------------------------------------------
// $Id: service.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of  some GUI service functions
//
// Copyright (C) 1996-2001  A.Rysin, S.Dmytriyeva
// Uses  string class (C) A.Rysin 1999
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (http://qt.nokia.com)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the terms of GEMS3 Development
// Quality Assurance Licence (GEMS3.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#ifndef _service_h
#define _service_h

#include <string>
#include <vector>
using namespace std;

class QWidget;
struct windowSetupData;
struct tableSetupData;

/// remove numbers!
const int VF_YES =6;
const int VF_NO = 7;
const int VF_CANCEL = 2;


/// Combines path, directory, name and extension to full pathname
string u_makepath(const string& dir,
           const string& name, const string& ext);

/// Splits full pathname to path, directory, name and extension
void u_splitpath(const string& Path, string& dir,
            string& name, string& ext);


int vfQuestYesNoCancel(QWidget* par, const string& title, const string& mess);

bool vfQuestion(QWidget* par, const string& title, const string& mess);

/// remove numbers!
const int VF3_1 = 6;
const int VF3_2 = 7;
const int VF3_3 = 2;

// returns VF3_1, VF3_2 or VF3_3
int vfQuestion3(QWidget* par, const string& title, const string& mess,
                const string& s1, const string& s2,  const string& s3="&Cancel",
                bool i_mov = false);
enum WarnType { vfInfo=0, vfWarn, vfErr };

void vfMessage(QWidget* par, const string& title,
               const string& mess, WarnType type=vfWarn);

int vfChoice(QWidget* par, vector<string>& arr, const char* prompt, int sel=0);
int vfChoice2(QWidget* par, vector<string>& arr, const char* prompt,
                         int sel, bool& all_);
int vfChoice(QWidget* par, const char* title, const char* prompt,
         int nVal, int *arr, int sel=0);

vector<int> vfMultiChoice(QWidget* par,
                          vector<string>& arr, const char* prompt);

vector<int> vfMultiChoiceSet(QWidget* par, vector<string>& arr,
                             const char* prompt, vector<int>& sel);

vector<string> vfMultiKeys(QWidget* par, const char* caption,
                            int iRt, const char* key );

vector<string> vfMultiKeysSet(QWidget* par, const char* caption,
                  int iRt, const char* key, vector<string>& sel );
vector<string> vfRDMultiKeysSet(QWidget* par, const char* caption,
                  const char* key, vector<string>& sel, short NsiT=0 );

bool vfListFiles(QWidget* par, bool show_dlg, const char * prfName,
                 vector<string>& fls, vector<int>& cnt );

string  vfKeyEdit(QWidget* par, const char* title, int iRt, const char* key=0);

string vfKeyProfile(QWidget* par, const char* title, int iRt,
        bool& chAqGas, bool& addFiles, bool& remake,
        bool& loadOld,int& makeDump, string& key_templ  );

string vfKeyTemplEdit(QWidget* par, const char* title,
             int iRt, const char* key=0,  bool allowTemplate=true);

bool vfKeyCanged(QWidget* par, const char* caption,
            string& from_str, string& to_str, int fldLen );

bool vfExcludeFillEdit(QWidget* par, const char* caption,
   vector<string>& aICkeys, vector<bool>& sel, double& fill_data );

bool vfChooseFileOpen(QWidget* par, string& path,
  const char* title=0 , const char* filter=0);
bool vfChooseFileSave(QWidget* par, string& path,
  const char* title=0 , const char* filter=0);
bool vfChooseDirectory(QWidget* par, string& path_,
       const char* title );
void vfMakeDirectory(QWidget* par, const char *dir );

double vfGetDouble( QWidget * par, const char* title, const char* label,
  double value = 0, double min = -1e8, double max = 1e8, int decimals = 4 );

struct elmWindowData;
struct setFiltersData;
struct pagesSetupData;

bool vfElements(QWidget* par, const char * prfName,
           elmWindowData& elm_data, setFiltersData& sf_data );

bool  vfSystemInput(QWidget* par, const char * p_key,
    vector<windowSetupData>& wnData, vector<tableSetupData>& tbData,
    vector<pagesSetupData>& scalarsList );

bool vfProcessSet(QWidget* par, const char * p_key,
       char flds[24], int size[8], short tabInt[6], double tabDoubl[24],
       string& calcScript, string& outScript, vector<string>& names,
       string& xName, string& yName );
bool vfGEM2MTSet(QWidget* par, const char * p_key,
       char flds[32], int size[20],  double Tai[4], double Pai[4],double Tau[3],
       string& calcScript, string& outScript, vector<string>& names,
       string& xName, string& yName, vector<int>& vtk1, vector<int>& vtk2 );
bool vfUnSpaceSet(QWidget* par, const char * p_key,
              char flds[38], int size[10] );
bool vfGtDemoSet(QWidget* par, const char * p_key, int size[8],
            string& prkey, string& script, vector<string>& names,
            string& xName, string& yName  );
bool vfComposSet(QWidget* par, const char * p_key,
            char flgs[6], int sizes[2], double& r2 );
bool vfDCompSet(QWidget* par, const char * p_key,
            char flgs[15], int sizes[4] );
bool vfReacDCSet(QWidget* par, const char * p_key,
            char flgs[12], int sizes[4] );
bool vfRTparmSet(QWidget* par, const char * p_key,
            char flgs[10], int sizes[7], float val[6],
            string& script, string& xName, string& yName,
            vector<string>& names  );
bool vfPhaseSet(QWidget* par, const char * p_key,
          char flgs[37], int sizes[30], double& r2 );
bool vfDualThSet(QWidget* par, const char * p_key,
            char flgs[20], int size[8]  );
bool vfProjectSet(QWidget* par, const char * p_key,
              char flds[38], int&  taskset );
bool vfSystemSet(QWidget* par, const char * p_key,
       char flgs[40],  string& name, string& comment, string& EQkey );

bool  vfAutoPhaseSet(QWidget* par, const char* pr_key,
                     string& a_key, string& g_key,
                     char& acode, char& gcode, float apar[8]);

bool vfLookupDialogSet(QWidget* wpar, char flags[6],
                double Tai[4], double Pai[4] );

void vfPhaseInfo(QWidget* wpar, bool system, int xph, string phname,
                 vector<int>& xdclist, vector<string>& dcnames, int xdc );

void stringToUtf8( string& value );



//----------------------------------------------------------------
// Help file name constant

extern const char *WZSTEP;
extern const char *MWPAGE;
extern const char *DBM;
extern const char *NUMSET;

// modules
extern const char *GM_COMPOS_WZ_HTML; //ComposWizard
extern const char *GM_COMPOS_HTML;    //m_compos.cpp
extern const char *GM_ICOMP_HTML;     // m_icomp.cpp
extern const char *GM_DCOMP_HTML;     //m_dcomp.cpp
extern const char *GM_DCOMP_WZ_HTML;  //DCompWizard
extern const char *GM_REACDC_HTML;    //m_reacdc.cpp
extern const char *GM_REACDC_WZ_HTML; //ReacDCWizard
extern const char *GM_RTPARM_HTML;    //m_rtparm.cpp
extern const char *GM_RTPARM_WZ_HTML; //RTparmWizard
extern const char *GM_PHASE_HTML;     //m_phase.cpp
extern const char *GM_PHASE_WZ_HTML;  //PhaseWizard
extern const char *GM_SDREF_HTML;     //m_sdata.cpp
extern const char *GM_CONST_HTML;     //m_const.cpp
extern const char *GM_PROJECT_HTML;   //m_param.cpp
extern const char *GM_PROJECT_WZ_HTML;//ProjectWizard
extern const char *GEMS_NUMSET_HTML;     //m_param.cpp
extern const char *GM_SYSEQ_HTML;        //m_syseq.cpp
extern const char *GEMS_SYSTEM_WZ_HTML;//SystemWizard
extern const char *GM_PROCES_HTML;     //m_proces.cpp
extern const char *GM_PROCES_WZ_HTML;  //ProcesWizard
extern const char *GM_GTDEMO_HTML;     //m_gtdemo.cpp
extern const char *GM_GTDEMO_WZ_HTML;  //GtDemoWizard
extern const char *GM_GEM2MT_HTML;     //m_gem2mt.cpp
extern const char *GM_GEM2MT_WZ_HTML;  //GEM2MTWizard
extern const char *GM_UNSPACE_HTML;    //m_unspace.cpp
extern const char *GM_UNSPACE_WZ_HTML; //UnSpaceWizard
extern const char *GM_DUALTH_HTML;     //m_dualth.cpp
extern const char *GM_DUALTH_WZ_HTML;  //DualThWizard
extern const char *GSM_SYSTEM_HTML;    //ms_system.cpp
extern const char *GSM_MTPARM_HTML;    //ms_mtparm.cpp
extern const char *GSM_EQDEMO_HTML;    //ms_calc.h
extern const char *GSM_MULTI_HTML;     //ms_calc.h
// dialogs
extern const char *GEMS_ABOUT_HTML;    // MainDialog
extern const char *GEMS_TUTOR_HTML;    // MainDialog
extern const char *GEMS_SETUP_HTML;    // SettingsDialog
extern const char *GEMS_TDBAS_HTML;    // DBDialog
extern const char *GEMS_MISCL_HTML;     // DBAdvDialog
extern const char *GEMS_FORMT_HTML;     // DBAdvDialog
extern const char *GEMS_REKEY_HTML;      // KeyFilter
extern const char *GEMS_MPROJ_HTML;      // KeyProfile
extern const char *GEMS_MPROJ_LF_HTML;   // ListFilesDialog
extern const char *GEMS_MPROJ_LF_HTML;   // ListFilesDialog
extern const char *GEMS_ONESYS_HTML;     // NewSystemDialog
extern const char *GEMS_HOWTO_HTML;
extern const char *GEMS_HOWTOB_HTML;
extern const char *GEMS_BCC_HTML;
extern const char *GEMS_IPM_HTML;
extern const char *GEMS_RECIPE_HTML;    // InputSystemDialog
extern const char *GEMS_GRAPH_HTML;     // GraphDialog
extern const char *GEMS_GRAPH_CUS_HTML; // LegendDialog
extern const char *GEMS_MISSING_HTML;   // ExcludeFillDialog
extern const char *GEMS_MODES_HTML;     // EQDialog
extern const char *GEMS_ELEMENTS_HTML;  // ElementsDialog
extern const char *GEMS_SETFILT_HTML;   // SetFiltersDialog
extern const char *GEMS_APHSETUP_HTML;  // AutoPhaseDialog
extern const char *GEMS_TEMPL_HTML;     // ChangeKeyDialog
extern const char *GEMS_PHINFO_HTML;    // PhaseInfoDialog
extern const char *GEMS_CALCUL_HTML;    // CalcDialog
extern const char *GEMS_WRIPMF_HTML;    // LookupDialog
extern const char *GEMS_MARK_HTML;      // SelectDialog
extern const char *GEMS_SELECT_HTML;    // KeyDialog

extern const char *GEMS_HOWTOSTART_HTML; // MainDialog
extern const char *GEMS_ABOUT_HTML;      // MainDialog
extern const char *GEMS_AUTHORS_HTML;    // MainDialog
extern const char *GEMS_THANKS_HTML;     // MainDialog
extern const char *GEMS_LICENSE_HTML;    // MainDialog

// old
extern const char *GEMS_TOC_HTML; // default for modules

#endif // _service_h

