//-------------------------------------------------------------------
// $Id$
//
/// \file ms_multi.h
/// Declaration of TMulti class, configuration, and related functions
/// based on the IPM work data structure MULTI
//
/// \struct MULTI ms_multi.h
/// Contains chemical thermodynamic work data for GEM IPM-3 algorithm
//
// Copyright (c) 1995-2013 S.Dmytriyeva, D.Kulik, T.Wagner
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
#ifndef ms_multi_new_h
#define ms_multi_new_h

#include "GEMS3K/ms_multi.h"
#include "v_module.h"
#include "v_ipnc.h"
// Internal subroutine for ET_translate() to process Phase scripts
typedef int (tget_ndx)( int nI, int nO, int Xplace );

struct SPP_SETTING
{   // Base Parametres of SP
    char ver[TDBVERSION]; // Version & Copyright 64
    BASE_PARAM p; // Flags and thresholds for numeric modules
    char           // default codes of values
    DCpct[7],      // Default DCOMP flags and codes
    DCpdc[10],     // Default DCOMP class and units
    BCpc[7],       // Default COMPOS configuration
    REpct[7],      // Default REACDC flags and codes

    REpdc[7],      // Default REACDC class and units
    REpvc[9],      // Default REACDC configuration
    RPpdc[11],      // Default RTPARM flags and codes
    RPpvc[33],     // Default RTPARM configuration  reserved
    PHsol_t[7],    // Default PHASE model codes
    PHpvc[7],      // Default PHASE configuration
    MUpmv[11],     // Default RMULTS configuration
    TPpdc[9],      // Default class and units for MTPARM
    TPpvc[21],     // Default MTPARM configuration
    SYppc[11],     // Default class and flags for SYSTEM
    SYpvc[29],     // Default SYSTEM confifuration
    UTppc[11],     // Default DUTERM class and flags
    PEpsc[13],     // Default PROCES class and flags
    PEpvc[13],     // Default PROCES configuration
    GDcode[2][20], // Default names of screen and graphs in GTDEMO ????
    GDpsc[7],      // Default names of lines on GTDEMO graphs
    GDpcc[2][9],   // Default axis names for GTDEMO
    GDptc[7],      // Default GTDEMO configuration
    GDpgw[7],      // Default setup of graphs in GTDEMO
    SDrefKey[32],  // sdref key
    Reserv[50-32]    // (reserved) objects later
    ;
    // for RTPARM
    short NP,NT,  // Default N of points (RTPARM): P, T
    NV,       // reserved
    Mode,     // Default indexation mode RTPARM
    ResShort[5];
    float        // RTPARM
    Pi[3],    // Default interval for pressure
    Ti[3],    // Default interval for temperature, C
    Vi[3],    // Default interval for volume, cm3
    DRpst, DRtcst,   // Default Pr, Tr for DCOMP & REACDC
    lowPosNum, // MULTI Cutoff moles of DC (Ls set) { 1e-19 };
    logXw,     // log(1e-16)
    logYFk,    // log(1e-9)
    aqPar[5];  // b_gamma, a0, NeutPolicy, GamH2O, b_gam_T_dep for auto aq phase model
    //    ResFloat;   // one parameter for auto gas/fluid phase

    void write(GemDataStream& oss);
    void read(GemDataStream& oss);
};


// Data of MULTI
class TMulti: public TMultiBase, public TSubModule
{

public:

    static TMulti* sm;

    // This is used only in GEM-Selektor
    std::vector<std::shared_ptr<IPNCalc>> qEp;
    std::vector<std::shared_ptr<IPNCalc>> qEd;

    TMulti( int nrt );

    ~TMulti()
    {
        //      Free_TSolMod();     // Added 06.05.2011 DK - possible bugfix
        Free_internal();
        Free_uDD();
    }

    BASE_PARAM* base_param() const override;

    const char* GetName() const override
    {  return "Multi";  }
    void ods_link( int i=0) override;
    void dyn_set( int i=0) override;
    void dyn_kill( int i=0 ) override;
    void dyn_new( int i=0) override;
    void set_def( int i=0) override
    {
        TMultiBase::set_def(i);
    }
    void multi_realloc( char /*PAalp*/, char /*PSigm*/ ) override
    {
        dyn_new();
    }

    // ms_muleq.cpp
    void packData();
    void packData( TCIntArray PHon, TCIntArray DCon );
    void setSizes();
    void loadData( bool newRec ) override;
    void unpackData();

    void MultiKeyInit( const char*key );
    void EqstatExpand( /*const char *key,*/  bool calcActivityModels/*, bool calcKineticModels*/ );
    void ET_translate( int nOet, int nOpex, int JB, int JE, int jb, int je,
                       tget_ndx *get_ndx = nullptr );
    void getNamesList( int nO, TCStringArray& lst );

    class UserCancelException {};

    /// connection to UnSpace
    double pb_GX( double *Gxx  );

    long int testMulti( ) override;


    // EXTERNAL FUNCTIONS
    void DC_LoadThermodynamicData();
    void DC_LoadThermodynamicData( TNode* aNa  ) override
    {
        if( aNa == nullptr)
          DC_LoadThermodynamicData();
        else
          TMultiBase::DC_LoadThermodynamicData( aNa );
     }



    long get_sizeFIs () { return sizeFIs; }

protected:

    void get_PAalp_PSigm(char &PAalp, char &PSigm) override;
    void STEP_POINT( const char* /*str*/) override;
    void alloc_IPx( long int LsIPxSum ) override;
    void alloc_PMc( long int LsModSum ) override;
    void alloc_DMc( long int LsMdcSum ) override;
    void alloc_MoiSN( long int LsMsnSum ) override;
    void alloc_SitFr( long int LsSitSum ) override;
    void alloc_DQFc( long int DQFcSum ) override;
    void alloc_PhLin( long int PhLinSum ) override;
    void alloc_lPhc( long int lPhcSum ) override;
    void alloc_xSMd( long int xSMdSum ) override;
    void alloc_IsoPc( long int IsoPcSum ) override;
    void alloc_IsoSc( long int IsoScSum ) override;
    void alloc_IsoCt( long int IsoCtSum ) override;
    void alloc_EImc( long int EImcSum ) override;
    void alloc_mCDc( long int mCDcSum ) override;
    void alloc_xSKrC( long int xSKrCSum ) override;
    void alloc_ocPRkC( long int ocPRkC_feSArC_Sum ) override;
    void alloc_feSArC( long int ocPRkC_feSArC_Sum ) override;
    void alloc_rpConC( long int rpConCSum ) override;
    void alloc_apConC( long int apConCSum ) override;
    void alloc_AscpC( long int AscpCSum ) override;
    void alloc_UMpcC( long int UMpcSum ) override;
    void alloc_xICuC( long int xICuCSum ) override;

    bool calculateActivityCoefficients_scripts( long int LinkMode, long k, long jb,
                                                long jpb, long jdb, long ipb, double pmpXFk ) override;
    bool testTSyst() const  override;
    void initalizeGEM_IPM_Data_GUI() override;
    void multiConstInit_PN() override;
    void GEM_IPM_Init_gui1() override;
    void GEM_IPM_Init_gui2() override;
    void GasParcP() override;
    void pm_GC_ods_link( long int k, long int jb, long int jpb, long int jdb, long int ipb );

    // bool GEM_IPM_InitialApproximation() override;
    //void load_all_thermodynamic_from_grid(TNode *aNa, double TK, double P) override;

private:

    // These pointers and methods are only used in GEMS-PSI
    void MultiSystemInit();
    void SystemToLookup();

    void multi_sys_dc();
    void multi_sys_ph();
    void ph_sur_param( int k, int kk );
    void ph_surtype_assign( int k, int kk, int jb, int je,
                            int car_l[], int car_c, int Cjs );
    void sm_text_analyze( int nph, int Type, int JB, int JE, int jb, int je );
    void SolModLoad();
    // new TKinMet stuff
    void KinMetModLoad();
    bool CompressPhaseIpxt( int kPH );
    std::string PressSolMod( int nP );
    char *ExtractEG( char *Etext, int jp, size_t& EGlen, int Nes );
    int find_icnum( char *name, int LNmode );
    int find_dcnum( char *name, int jb, int je, int LNmode, char *stmt  );
    int find_phnum( char *name, int LNmode );
    int find_acnum( char *name, int LNmode );
    int find_phnum_multi( const char *name);
    int find_dcnum_multi( const char *name);
    int find_icnum_multi( const char *name);
    int find_mgpnum(const char *name);
    int find_flnum(const char *name);
    const char* GetHtml() override;
};


#endif   //ms_multi_new_h

