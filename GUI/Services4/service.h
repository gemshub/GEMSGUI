//-------------------------------------------------------------------
// $Id: service.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of  some GUI service functions
//
// Copyright (C) 1996-2001  A.Rysin, S.Dmytriyeva
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

#ifndef _service_h
#define _service_h

#include <vector>
#include "v_user.h"


class QWidget;
struct windowSetupData;
struct tableSetupData;

/// remove numbers!
const int VF_YES =6;
const int VF_NO = 7;
const int VF_CANCEL = 2;

int vfQuestYesNoCancel(QWidget* par, const std::string& title, const std::string& mess);

bool vfQuestion(QWidget* par, const std::string& title, const std::string& mess);

/// remove numbers!
const int VF3_1 = 6;
const int VF3_2 = 7;
const int VF3_3 = 2;

// returns VF3_1, VF3_2 or VF3_3
int vfQuestion3(QWidget* par, const std::string& title, const std::string& mess,
                const std::string& s1, const std::string& s2,  const std::string& s3="&Cancel",
                bool i_mov = false);

enum WarnType { vfInfo=0, vfWarn, vfErr };

void
vfMessage(QWidget* par, const std::string& title, const std::string& mess, WarnType type=vfWarn);

int
vfChoice(QWidget* par, TCStringArray& arr, const char* prompt, int sel=0);
int
vfChoice2(QWidget* par, TCStringArray& arr, const char* prompt,
                         int sel, bool& all_);
int
vfChoice(QWidget* par, const char* title, const char* prompt,
         int nVal, int *arr, int sel=0);

TCIntArray
vfMultiChoice(QWidget* par, TCStringArray& arr, const char* prompt);

TCIntArray
vfMultiChoiceSet(QWidget* par, TCStringArray& arr, const char* prompt, TCIntArray& sel);

TCStringArray
vfMultiKeys(QWidget* par, const char* caption,
        unsigned int iRt, const char* key );

TCStringArray
vfMultiKeysSet(QWidget* par, const char* caption,
        unsigned int iRt, const char* key, TCStringArray& sel );
TCStringArray vfRDMultiKeysSet(QWidget* par, const char* caption,
         const char* key, TCStringArray& sel, short NsuT=0 );

bool
vfListFiles(QWidget* par, bool show_dlg, const char * prfName,
                TCStringArray& fls, TCIntArray& cnt );

std::string
vfKeyEdit(QWidget* par, const char* title, unsigned int iRt, const char* key=0);

std::string
vfKeyProfile(QWidget* par, const char* title, int iRt,
    bool& chAqGas, bool& addFiles, bool& remake, int& makeDump,
    std::string& key_templ, bool& genGEMS3k, bool& brief_mode  );

std::string
vfKeyTemplEdit(QWidget* par, const char* title, unsigned int iRt, const char* key=0,
               bool allowTemplate=true);
bool
vfKeyCanged(QWidget* par, const char* caption,
            std::string& from_str, std::string& to_str, int fldLen );

bool
vfExcludeFillEdit(QWidget* par, const char* caption,
   TCStringArray& aICkeys, std::vector<bool>& sel, double& fill_data );


bool vfChooseFileOpen(QWidget* par, std::string& path,
  const char* title=0 , const char* filter=0);
bool vfChooseFileSave(QWidget* par, std::string& path,
  const char* title=0 , const char* filter=0);
bool vfChooseDirectory(QWidget* par, std::string& path_,
       const char* title );

// askOverwrite = 0 - not ask
// askOverwrite = 1 - ask and cancel
// askOverwrite = 2 - ask and clear
void vfMakeDirectory(QWidget* par, const char *dir, int askOverwrite = 0 );
double vfGetDouble( QWidget * par, const char* title, const char* label,
  double value = 0, double min = -1e8, double max = 1e8, int decimals = 4 );


struct elmWindowData;
struct setFiltersData;
struct pagesSetupData
{
   std::string pageName;
   int nObj;         // index of object
   int ndx;         //  second index in object
   std::string ndxName; // if ndxName not empty ( ndx is first ndx ), second from list

   pagesSetupData( const char * pName, int aobj, int andx = 0, const char*  andxName = nullptr):
        pageName(pName),  nObj(aobj), ndx(andx)
   {
     if( andxName )
     {
         ndxName = andxName;
         pageName += "(";
         pageName += ndxName;
         pageName += ")";
     }
     else
       ndxName = "";
   }

};

bool
vfElements(QWidget* par, const char * prfName,
           elmWindowData& elm_data, setFiltersData& sf_data );

bool  vfSystemInput(QWidget* par, const char * p_key,
    std::vector<windowSetupData>& wnData, std::vector<tableSetupData>& tbData,
    std::vector<pagesSetupData>& scalarsList );

bool
vfProcessSet(QWidget* par, const char * p_key,
              char flds[24], int size[8], short tabInt[6], double tabDoubl[24],
              std::string& calcScript, std::string& outScript, TCStringArray& names,
              std::string& xName, std::string& yName );
bool
vfGEM2MTSet(QWidget* par, const char * p_key,
              char flds[32], int size[20],  double Tai[4], double Pai[4],double Tau[3],
            std::string& calcScript, std::string& outScript, TCStringArray& names,
            std::string& xName, std::string& yName, TCIntArray& vtk1, TCIntArray& vtk2 );
bool
vfUnSpaceSet(QWidget* par, const char * p_key,
              char flds[38], int size[10] );
bool
vfGtDemoSet(QWidget* par, const char * p_key, int size[8],
            std::string& prkey, std::string& script, TCStringArray& names,
            std::string& xName, std::string& yName  );

bool
vfComposSet(QWidget* par, const char * p_key,
            char flgs[6], int sizes[2], double& r2 );
bool
vfDCompSet(QWidget* par, const char * p_key,
            char flgs[15], int sizes[4] );
bool
vfReacDCSet(QWidget* par, const char * p_key,
            char flgs[12], int sizes[4], TCStringArray& sel );

bool
vfRTparmSet(QWidget* par, const char * p_key,
            char flgs[10], int sizes[7], double val[6],
            std::string& script, std::string& xName, std::string& yName, TCStringArray& names  );

bool
vfPhaseSet( QWidget* par, const char * p_key,
            char flgs[37], int sizes[30], double& r2, TCStringArray& rd_sel,
            TCStringArray& phase_sel, TCStringArray& lDCr_sel );

bool
vfDualThSet(QWidget* par, const char * p_key,
            char flgs[20], int size[8]  );

bool
vfProjectSet(QWidget* par, const char * p_key,
              char flds[38], int&  taskset );
bool
vfSystemSet(QWidget* par, const char * p_key,
            char flgs[40],  std::string& name, std::string& comment, std::string& EQkey );
bool
 vfAutoPhaseSet(QWidget* par, const char* pr_key, std::string& a_key, std::string& g_key,
        char& acode, char& gcode, float apar[8]);

bool vfLookupDialogSet(QWidget* wpar, char flags[6],
                double Tai[4], double Pai[4] );

void vfPhaseInfo(QWidget* wpar, bool system, int xph, std::string phname,
                 vector<int>& xdclist, vector<std::string>& dcnames, int xdc );


//class TObject;
//void vfObjToFile(QWidget* par, TObject* obj);

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

