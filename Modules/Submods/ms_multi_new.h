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
#include "m_param.h"
#include "v_ipnc.h"

// Internal subroutine for ET_translate() to process Phase scripts
typedef int (tget_ndx)( int nI, int nO, int Xplace );
// TSolMod header
#include "GEMS3K/s_solmod.h"
// new: TsorpMod and TKinMet
#include "GEMS3K/s_sorpmod.h"
#include "GEMS3K/s_kinmet.h"


// Data of MULTI
class TMulti: public TMultiBase, public TSubModule
{

public:

    static TMulti* sm;

    // This is used only in GEM-Selektor
    TIArray<IPNCalc> qEp;
    TIArray<IPNCalc> qEd;

    TMulti( int nrt ):
        TMultiBase(nullptr),TSubModule( nrt )
    {}

    ~TMulti()
    {
        //      Free_TSolMod();     // Added 06.05.2011 DK - possible bugfix
        Free_internal();
        Free_uDD();
    }

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
   void multi_kill() override {}
   BASE_PARAM *pa_p_ptr() const override;

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

    //#else
    //    const TNode *node1;

    //    void multi_realloc( char PAalp, char PSigm );  /// could be virtual dyn_new
    //#endif

    long int testMulti( ) override;

    //connection to mass transport
//void to_file( GemDataStream& ff ) override;
//void to_text_file( const char *path, bool append=false  ) override;
//void from_file( GemDataStream& ff ) override;
//void to_text_file_gemipm( iostream& ff, bool addMui,
//                              bool with_comments = true, bool brief_mode = false ) override;
//void from_text_file_gemipm( iostream& ff,  DATACH  *dCH ) override;
//void copyMULTI( const TMulti& otherMulti );

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

    void GasParcP() override;
    void pm_GC_ods_link( long int k, long int jb, long int jpb, long int jdb, long int ipb ) override;
    bool calculateActivityCoefficients_scripts( long int LinkMode, long k, long jb,
                                                long jpb, long jdb, long ipb, double pmpXFk ) override;
    bool testTSyst( int ii ) const  override;
    void initalizeGEM_IPM_Data_GUI() override;
    void multiConstInit_PN() override;
    void GEM_IPM_Init_gui1() override;
    void GEM_IPM_Init_gui2() override;
    virtual void get_PAalp_PSigm(char &PAalp, char &PSigm) override;

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

    // bool GEM_IPM_InitialApproximation() override;

private:

    // These pointers and methods are only used in GEMS-PSI
    void MultiSystemInit();
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
    string PressSolMod( int nP );
    char *ExtractEG( char *Etext, int jp, int *EGlen, int Nes );
    int find_icnum( char *name, int LNmode );
    int find_dcnum( char *name, int jb, int je, int LNmode, char *stmt  );
    int find_phnum( char *name, int LNmode );
    int find_acnum( char *name, int LNmode );
    int find_phnum_multi( const char *name);
    int find_dcnum_multi( const char *name);
    int find_icnum_multi( const char *name);
    const char* GetHtml() override;
};


#endif   //ms_multi_new_h
