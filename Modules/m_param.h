//-------------------------------------------------------------------
// $Id$
//
/// \file m_param.h
/// Declaration of TProfil class, config and calculation functions
//
// Copyright (c) 1995-2012 S.Dmytriyeva, D.Kulik
// <GEMS Development Team, mailto:gems2.support@psi.ch>
//
// This file is part of the GEMS3K code for thermodynamic modelling
// by Gibbs energy minimization <http://gems.web.psi.ch/GEMS3K/>
//
// GEMS3K is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.

// GEMS3K is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with GEMS3K code. If not, see <http://www.gnu.org/licenses/>.
//-------------------------------------------------------------------
//

#ifndef _m_param_h_
#define _m_param_h_

#include "ms_rmults.h"
#include "ms_mtparm.h"
#include "ms_system.h"
#include "ms_multi_new.h"
#include "ms_calc.h"

class GemDataStream;
class QWidget;

extern long int showMss;

extern SPP_SETTING pa_;

struct CompItem
{
    int line;
    short delta; // RpnItemType
    CompItem(int nLine, short nDelta):
        line(nLine), delta(nDelta)
    {}

};

// Module TParam (RMULTS+MTPARM+SYSTEM+MULTY see SubModules)
class TProfil : public TCModule
{
    TRMults* rmults;
    TMTparm* mtparm;
    TSyst* syst;
    TMulti* multi_internal;

    bool newRecord;
    // new 12.12.12 data to Project record
    char *internalBufer;  // text bufer for internal Project settings
    // (now only for built-in TDB)

    // to compare with old Project
    bool comp_change_all;
    char (*SFold)[PH_RKLEN];// List of PHASE definition keys [0:Fi-1]             DB
    char (*SMold)[DC_RKLEN];// List of DC definition keys (DCOMP, REACDC) [0:L-1] DB
    char (*SAold)[BC_RKLEN];// List of COMPOS definition keys [0:La-1]            DB
    char (*SBold)[IC_RKLEN];// List of ICOMP record keys (stoichiometry basis)[0:N-1] DB
    short *Llold;// L1 vector, shows a number of DC included to each phase [0:Fi-1] DB
    short Nold,     // N of IC, incl. Zz (charge) and Vol (volume)
    Lold,       // L   - of DC - total for all phases
    Fiold,      // FI  - total number of phases
    Fisold,     // FIs - total number of multi-component phases
    Laold,      // La  - of references to COMPOS records
    Lsold;      // Ls  - total number of DC in multi-component phases

    // data to load SysEq <project>:G:z_cp_config:0:0:1:25:0
    bool  isSysEq;
    TCIntArray DCon;
    TCIntArray PHon;
    TCIntArray DCoff;
    TCIntArray PHoff;

protected:

    void InitFN( const char * prfName, const char* prfTemplate );
    void RenameFN( const char * prfName, const char* prfTemplate );
    bool GetFN( const char * prfName, bool show_dlg=true );
    void SetFN();
    bool rCopyFilterProfile( const char * prfName );

    void OpenProfileMode(
            const char* key, bool changeAqGas, bool addFile,  bool remakeRec );
    bool NewProfileMode( bool remakeRec, std::string& key_templ );
    bool NewProfileModeElements( bool remakeRec, std::string& key_templ );
    /// Recalculate all systems in SIA (2) or AIA mode;
    /// save result to file if  make_dump_file is true
    void CalcAllSystems( int SIA_or_AIA, bool make_dump_file );


    /// Dump all Systems&Processes to GEMS3K module
    /// \param calcMode - system calculate mode: 0 - no calculation; 2- NEED_GEM_SIA; 1-NEED_GEM_AIA
    /// \param brief_mode - Do not write data items that contain only default values
    /// \param add_mui - Print internal indices in RMULTS to IPM file for reading into Gems back
    void GEMS3KallSystems(int makeCalc, bool brief_mode=true, bool add_mui = false);

    /// Generate for current loaded system MULTI, DATACH and DATABR files structure prepared from GEMS.
    /// Prints files for separate coupled FMT-GEM programs that use GEMS3K module
    /// \param filepath - IPM work structure file path&name
    /// \param brief_mode - Do not write data items that contain only default values
    /// \param add_mui - Print internal indices in RMULTS to IPM file for reading into Gems back
    void CurrentSystem2GEMS3K( const std::string& filepath, bool brief_mode, bool add_mui = false );

    /// Load&Calculate system and Generate MULTI, DATACH and DATABR files structure prepared from GEMS.
    /// Prints files for separate coupled FMT-GEM programs that use GEMS3K module
    /// \param key - loads system record before generate
    /// \param calcMode - system calculate mode: 0 - no calculation; 2- NEED_GEM_SIA; 1-NEED_GEM_AIA
    /// \param filepath - IPM work structure file path&name
    /// \param brief_mode - Do not write data items that contain only default values
    /// \param add_mui - Print internal indices in RMULTS to IPM file for reading into Gems back
    void System2GEMS3K( const std::string key, int calcMode, const std::string& filepath, bool brief_mode, bool add_mui = false );

    /// For all systems in project generate MULTI, DATACH and DATABR files structure prepared from GEMS.
    /// Prints files for separate coupled FMT-GEM programs that use GEMS3K module
    /// \param savedSystems - system records key to be ignored
    /// \param calc_mode - system calculate mode: 0 - no calculation; 2- NEED_GEM_SIA; 1-NEED_GEM_AIA
    /// \param files_dir - IPM work structure directory name
    /// \param brief_mode - Do not write data items that contain only default values
    /// \param add_mui - Print internal indices in RMULTS to IPM file for reading into Gems back
    void allSystems2GEMS3K( TCStringArray& savedSystems, int calc_mode, const std::string& files_dir, bool brief_mode, bool add_mui = false);

    /// For all processes in project generate MULTI, DATACH and DATABR files structure prepared from GEMS.
    /// Prints files for separate coupled FMT-GEM programs that use GEMS3K module
    /// \param savedSystems - system records key to be ignored
    /// \param files_dir - IPM work structure directory name
    /// \param brief_mode - Do not write data items that contain only default values
    /// \param add_mui - Print internal indices in RMULTS to IPM file for reading into Gems back
    void allProcess2GEMS3K( TCStringArray& savedSystems, const std::string& files_dir, bool brief_mode, bool add_mui = false );

    void SaveOldList();
    void DeleteOldList();
    void TestChangeProfile();
    void Push( std::vector<CompItem>& aList, int aLine,
               short aDelta, const char* dbKeywd, std::string aKey );
    void ICcompare( std::vector<CompItem>& aIComp);
    void COMPcompare( std::vector<CompItem>& aCompos);
    void DCcompare( std::vector<CompItem>& aList, int& i,int& j, int nI, int nJ);
    void PHcompare( std::vector<CompItem>& aPhase, std::vector<CompItem>& aDComp);

    // multi load
    short BAL_compare();

public:

    static TProfil* pm;

    bool userCancel1;
    bool stepWise1;
    bool calcFinished;
    const char * status;

    bool useAqPhase;
    bool useGasPhase;
    /// Last reply for charge mismatch in calculated bulk composition.
    int charge_mismatch_quest_reply = VF_UNDEF;

    SPP_SETTING pa;

    TProfil( uint nrt );
    void InitSubModules();

    const char* GetName() const
    {
        return "Project";
    }

    void ods_link(int i=0);
    void dyn_set(int i=0);
    void dyn_kill(int i=0);
    void dyn_new(int i=0);
    void set_def(int i=0);
    void DeleteRecord( const char *key, bool errinNo=true );
    void MakeQuery();
    const char* GetHtml();

    // Load Thermodynamic data from Database
    void CheckMtparam();

    // Setup one of 5 default IPM numerical settings
    void ChangeSettings(int nSettings);

    // work with Project
    bool initCalcMode( const char * profileKey );
    void loadSystat( const char *key=nullptr );
    void newSystat( int mode );
    void deriveSystat();
    void PackSystat();
    //    double CalcEqstat( bool show_progress=true );
    double CalcEqstat(double &kdTime, const long kTimeStep = 0, const double kTime = 0.  );
    double ComputeEquilibriumState( /*long int& NumPrecLoops,*/ long int& NumIterFIA, long int& NumIterIPM );
/// Run process of calculate equilibria into the GEMSGUI shell
    void CalculateEquilibriumGUI();
    int  indDC( int );
    int  indPH( int );
    void  deleteAutoGenerated();
    void  systbcInput( QWidget* par, const char * p_key );
    int PhIndexforDC( int xdc, bool system );
    std::string PhNameforDC( int xdc, bool system );
    std::string PhNameforDC( int xdc, int& xph, bool system );
    TCStringArray DCNamesforPh( const char *PhName, bool system );
    void DCNamesforPh( int xph, bool system, vector<int>& xdc, vector<std::string>& dcnames);

    // Multi make functions
    void PMtest( const char *key );

    void LoadFromMtparm( QWidget* par, DATACH *CSD, bool no_interpolat );
    void CalcBcc(); // Calc bulk composition
    void ShowPhaseWindow(QWidget* par, const char *objName, int nLine);
    //void ShowEqPhaseWindow();
    void ShowDBWindow( const char *objName, int nLine=0 );
    void Clear_XeA_XeD_Phm_BIun();

    // Proces make functions
    void ET_translate( int nOet, int nOpex, int JB, int JE, int jb, int je,
                       tget_ndx *get_ndx = nullptr )
    { multi_internal->ET_translate( nOet, nOpex, JB, JE, jb, je, get_ndx); }
    void getNamesList( int nO, TCStringArray& lst )
    { multi_internal->getNamesList(nO, lst); }
    double MolWeight( int N, double *ICaw, double *Smline )
    { 	return syst->MolWeight( N, ICaw, Smline ); }
    void SyTestSizes()
    { 	syst->SyTestSizes(); }

    inline double HelmholtzEnergy( double x )
    {
        return multi_internal->HelmholtzEnergy(x);
    }
    inline double InternalEnergy( double TC, double P )
    {
        return multi_internal->InternalEnergy( TC, P );
    }

    //test
    /// The export to ThermoFun JSON format file should include all IComp, DComp and ReacDC records
    /// from the project database, not just the records needed for a particular system
    /// (where some elements, DComps or ReacDCs can be switched off) as done in preparation of DCH lookup arrays.
    ///  \param stream     stream to output json file
    void  generate_ThermoFun_input_file_stream( std::iostream& stream, bool compact);

    void makeGEM2MTFiles(QWidget* par);
    void outMultiTxt( const char *path, bool append=false  )
    {
        multi_internal->to_text_file( path, append );
    }
    void CmReadMulti(const char* path);
    bool CompareProjectName( const char* SysKey );
    void ChangeTPinKey( double T, double P );
    void SetSysSwitchesFromMulti( );
};

#endif  // _m_param_h


