//-------------------------------------------------------------------
// $Id$
//
/// \file node.cpp
/// Implementation of TNode class functionality including initialization
/// and execution of the GEM IPM 3 kernel
/// Works with DATACH and DATABR structures
//
// Copyright (c) 2005-2012 S.Dmytriyeva, D.Kulik, G.Kosakowski, F.Hingerl
// <GEMS Development Team, mailto:gems2.support@psi.ch>
//
// This file is part of the GEMS3K code for thermodynamic modelling
// by Gibbs energy minimization <http://gems.web.psi.ch/GEMS3K/>
//
// GEMS3K is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.
//
// GEMS3K is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GEMS3K code. If not, see <http://www.gnu.org/licenses/>.
//-------------------------------------------------------------------


#include "node_gui.h"
#include "GEMS3K/num_methods.h"
#include "visor.h"
#include "service.h"
#include "m_param.h"

TNodeGUI::TNodeGUI(TMultiBase *apm):TNode()
{
    internal_multi.reset();
    multi_base = apm;
    pmm = multi_base->GetPM();
}

void TNodeGUI::write_ThermoFun_format_stream(iostream &stream, bool compact)
{
    TProfil::pm->generate_ThermoFun_input_file_stream(stream, compact);
}

// Makes start DATACH and DATABR data from GEMS internal data (MULTI and other)
void TNodeGUI::MakeNodeStructures(
        long int anICb,       // number of stoichiometry units (<= nIC) used in the data bridge
        long int anDCb,      	// number of DC (chemical species, <= nDC) used in the data bridge
        long int anPHb,     	// number of phases (<= nPH) used in the data bridge
        long int* axIC,   // ICNL indices in DATABR IC vectors [nICb]
        long int* axDC,   // DCNL indices in DATABR DC list [nDCb]
        long int* axPH,   // PHNL indices in DATABR phase vectors [nPHb]
        bool no_interpolat,
        double* Tai, double* Pai,
        long int nTp_, long int nPp_, double Ttol_, double Ptol_  )
{
    long int ii;
    TCIntArray aSelIC;
    TCIntArray aSelDC;
    TCIntArray aSelPH;

    // make lists
    for( ii=0; ii<anICb; ii++)
        aSelIC.push_back( axIC[ii] );
    for( ii=0; ii<anDCb; ii++)
        aSelDC.push_back( axDC[ii] );
    for( ii=0; ii<anPHb; ii++)
        aSelPH.push_back( axPH[ii] );

    // set default data and realloc arrays
    makeStartDataChBR( 0, no_interpolat, aSelIC, aSelDC, aSelPH,
                       nTp_, nPp_, Ttol_, Ptol_, Tai, Pai );
}

// Make start DATACH and DATABR data from GEMS internal data (MULTI and other)
// Lookup arrays from arrays
void TNodeGUI::MakeNodeStructures( QWidget* par, bool select_all,bool no_interpolat,
                                double *Tai, double *Pai,
                                long int nTp_, long int nPp_, double Ttol_, double Ptol_  )
{
    TCIntArray aSelIC;
    TCIntArray aSelDC;
    TCIntArray aSelPH;

    // select lists
    getDataBridgeNames( par, select_all, aSelIC, aSelDC, aSelPH  );

    // set default data and realloc arrays
    makeStartDataChBR( par, no_interpolat, aSelIC, aSelDC, aSelPH,
                       nTp_, nPp_, Ttol_, Ptol_, Tai, Pai );
}

// Make start DATACH and DATABR data from GEMS internal data (MULTI and other)
// Lookup arays from iterators
void TNodeGUI::MakeNodeStructures( QWidget* par, bool select_all,
                                double Tai[4], double Pai[4]  )
{
    TCIntArray aSelIC;
    TCIntArray aSelDC;
    TCIntArray aSelPH;

    // select lists
    getDataBridgeNames( par, select_all, aSelIC, aSelDC, aSelPH  );

    // set default data and realloc arrays
    makeStartDataChBR( par, aSelIC, aSelDC, aSelPH, Tai, Pai );
}


// Build lists names of components for selection into DataBridge
void TNodeGUI::getDataBridgeNames( QWidget* par, bool select_all,
                                TCIntArray& aSelIC, TCIntArray& aSelDC, TCIntArray& aSelPH  )
{
    TCStringArray aList;

    // select lists
    aList.clear();
    for(long int ii=0; ii< pmm->N; ii++ )
    {  if( select_all )
            aSelIC.push_back( ii );
        else
            aList.push_back( char_array_to_string( pmm->SB[ii], MAXICNAME+MAXSYMB));
    }
    if( !select_all  )
        aSelIC = vfMultiChoice(par, aList,
                               "Please, mark independent components for selection into DataBridge");

    aList.clear();
    for(long int ii=0; ii< pmm->L; ii++ )
    {  if( select_all )
            aSelDC.push_back( ii );
        else
            aList.push_back( char_array_to_string( pmm->SM[ii], MAXDCNAME));
    }
    if( !select_all  )
        aSelDC = vfMultiChoice(par, aList,
                               "Please, mark dependent components for selection into DataBridge");

    aList.clear();
    for(long int ii=0; ii< pmm->FI; ii++ )
    {  if( select_all )
            aSelPH.push_back( ii );
        else
            aList.push_back( char_array_to_string( pmm->SF[ii], MAXPHNAME+MAXSYMB));
    }
    if( !select_all  )
        aSelPH = vfMultiChoice(par, aList,
                               "Please, mark phases for selection into DataBridge");
}

// Building internal dataCH and DataBR structures from Multi
void TNodeGUI::setupDataChBR( TCIntArray& selIC, TCIntArray& selDC, TCIntArray& selPH,
                           long int nTp_, long int nPp_, bool no_interpolation )
{
    // set sizes for DataCh
    uint ii;
    long int i1;
    // reallocates memory for     DATACH  *CSD;  and  DATABR  *CNode;
    if( !CSD )
        CSD = new DATACH;
    if( !CNode )
        CNode = new DATABR;

    CSD->nIC = pmm->N;
    CSD->nDC = pmm->L;
    CSD->nDCs = pmm->Ls;
    CSD->nPH = pmm->FI;
    CSD->nPS = pmm->FIs;
    CSD->nTp = nTp_;
    CSD->nPp = nPp_;
    if( pmm->Aalp )
        CSD->nAalp = 1;
    else
        CSD->nAalp = 0;
    CSD->iGrd = 0;

    // These dimensionalities define sizes of dynamic data in DATABR structure!!!

    CSD->nICb = static_cast<int>(selIC.size());
    CSD->nDCb = static_cast<int>(selDC.size());
    CSD->nPHb = static_cast<int>(selPH.size());
    CSD->nPSb = 0;
    for( ii=0; ii< selPH.size(); ii++, CSD->nPSb++ )
        if( selPH[ii] >= pmm->FIs )
            break;
    if( no_interpolation )
        CSD->mLook = 1;
    else
        CSD->mLook = 0;

    CSD->dRes1 = 0.;
    CSD->dRes2 = 0.;

    // realloc structures DataCh&DataBr
    dbr_dch_api::datach_realloc(CSD);
    dbr_dch_api::databr_free_internal(CNode);
    databr_realloc();

    // set dynamic data to DataCH

    for( ii=0; ii< selIC.size(); ii++ )
        CSD->xic[ii] = (long int)selIC[ii];
    for( ii=0; ii< selDC.size(); ii++ )
        CSD->xdc[ii] = (long int)selDC[ii];
    for( ii=0; ii< selPH.size(); ii++ )
        CSD->xph[ii] = (long int)selPH[ii];

    for( i1=0; i1< CSD->nIC*CSD->nDC; i1++ )
        CSD->A[i1] = pmm->A[i1];

    for( i1=0; i1< CSD->nPH; i1++ )
    {
        CSD->nDCinPH[i1] = pmm->L1[i1];
        CSD->ccPH[i1] = pmm->PHC[i1];
        fillValue( CSD->PHNL[i1], ' ', MaxPHN );
        copyValues( CSD->PHNL[i1], pmm->SF[i1]+MAXSYMB, min(MaxPHN,(long int)MAXPHNAME) );
    }
    for( i1=0; i1< CSD->nIC; i1++ )
    {
        CSD->ICmm[i1] = pmm->Awt[i1]/kg_to_g;
        CSD->ccIC[i1] = pmm->ICC[i1];
        copyValues( CSD->ICNL[i1], pmm->SB[i1] , min(MaxICN,(long int)MAXICNAME) );
    }
    for( i1=0; i1< CSD->nDC; i1++ )
    {
        CSD->DCmm[i1] = pmm->MM[i1]/kg_to_g;
        CSD->ccDC[i1] = pmm->DCC[i1];
        copyValues( CSD->DCNL[i1], pmm->SM[i1] , min(MaxDCN,(long int)MAXDCNAME) );
    }

    // set default data to DataBr
    // mem_set( &CNode->TK, 0, 32*sizeof(double));
    // CNode->NodeHandle = 0;
    // CNode->NodeTypeHY = normal;
    // CNode->NodeTypeMT = normal;
    // CNode->NodeStatusFMT = Initial_RUN;
    //   CNode->NodeStatusCH = NEED_GEM_AIA;
    // CNode->IterDone = 0;
    dbr_dch_api::databr_reset( CNode, 1 );

    if( pmm->pNP == 0 )
        CNode->NodeStatusCH = NEED_GEM_AIA;
    else
        CNode->NodeStatusCH = NEED_GEM_SIA;

    CNode->TK = pmm->TCc+C_to_K; //25
    CNode->P = pmm->Pc*bar_to_Pa; //1
    CNode->Ms = pmm->MBX; // in kg

    // arrays
    for( i1=0; i1<CSD->nICb; i1++ )
        CNode->bIC[i1] = pmm->B[ CSD->xic[i1] ];

    for( i1=0; i1<CSD->nDCb; i1++ )
    {
        CNode->dul[i1] = pmm->DUL[ CSD->xdc[i1] ];
        CNode->dll[i1] = pmm->DLL[ CSD->xdc[i1] ];
    }

    if( CSD->nAalp >0 )
        for( i1=0; i1< CSD->nPHb; i1++ )
            CNode->aPH[i1] = pmm->Aalp[CSD->xph[i1]]*kg_to_g;

    // puts calculated & dynamic data to DataBR
    packDataBr();

    if(  CSD->iGrd  )
        for( i1=0; i1< CSD->nDCs*gridTP(); i1++ )
            CSD->DD[i1] = 0.;
}

/// Prepares and writes DCH and DBR files for reading into the coupled code
void TNodeGUI::makeStartDataChBR( QWidget* par, bool no_interpolat,
                               TCIntArray& selIC, TCIntArray& selDC, TCIntArray& selPH,
                               long int  nTp_, long int  nPp_, double Ttol_, double Ptol_,
                               double *Tai, double *Pai )
{
    long int  i1;

    setupDataChBR( selIC, selDC, selPH, nTp_, nPp_, no_interpolat );

    CSD->Ttol = Ttol_;
    CSD->Ptol = Ptol_*bar_to_Pa;
    fillValue(CSD->Psat, 1e-5, CSD->nTp );

    // Build Look up array
    for( i1=0; i1<CSD->nTp; i1++ )
        CSD->TKval[i1] = Tai[i1]+C_to_K;
    for( i1=0; i1<CSD->nPp; i1++ )
        CSD->Pval[i1] = Pai[i1]*bar_to_Pa;

    TProfil::pm->LoadFromMtparm( par, CSD, no_interpolat );

    //for( i1=0; i1<CSD->nPp; i1++ )
    // CSD->Pval[i1] = Pai[i1]*bar_to_Pa;

}

/// Prepares and writes DCH and DBR files for reading into the coupled code
void TNodeGUI::makeStartDataChBR( QWidget* par,
                               TCIntArray& selIC, TCIntArray& selDC, TCIntArray& selPH,
                               double Tai[4], double Pai[4] )
{
    long int nT, nP, i1;
    double cT, cP;

    nT = getNpoints( Tai );
    nP = getNpoints( Pai );

    setupDataChBR( selIC, selDC, selPH, nT, nP, false ); // only grid

    CSD->Ttol = Tai[3];
    CSD->Ptol = Pai[3]*bar_to_Pa;
    fillValue(CSD->Psat, 1e-5, CSD->nTp );
    // Build Look up array
    cT = Tai[START_];
    for( i1=0; i1<CSD->nTp; i1++ )
    {
        CSD->TKval[i1] = cT+C_to_K;
        cT+= Tai[2];
    }
    cP = Pai[START_];
    for( i1=0; i1<CSD->nPp; i1++ )
    {
        CSD->Pval[i1] = cP*bar_to_Pa;
        cP+= Pai[2];
    }

    TProfil::pm->LoadFromMtparm( par, CSD, false ); // only grid

    //cP = Pai[START_];
    //for( i1=0; i1<CSD->nPp; i1++ )
    // {
    //  CSD->Pval[i1] = cP*bar_to_Pa;
    //  cP+= Pai[2];
    //}

}

// Test temperature and pressure values for the interpolation grid
bool TNodeGUI::TestTPGrid(  double Tai[4], double Pai[4] )
{
    bool notChanged = true;

    if( Tai[0]+C_to_K < CSD->TKval[0] ||
            Tai[1]+C_to_K > CSD->TKval[CSD->nTp-1] ||
            Pai[0]*bar_to_Pa < CSD->Pval[0] ||
            Pai[1]*bar_to_Pa > CSD->Pval[CSD->nPp-1] )
        notChanged = false;    // interval not into a grid

    return notChanged;
}

#ifdef USE_GEMS3K_SERVER

std::vector<string> TNodeGUI::generate_send_msg( bool add_head )
{
    bool brief_mode = false;
    bool add_mui = true;
    double Tai[4], Pai[4];

    // realloc and setup data for dataCH and DataBr structures
    Tai[0] = Tai[1] = pmm->TCc;
    Pai[0] = Pai[1] = pmm->Pc;
    Tai[2] = Pai[2] = 0.;
    Tai[3] = Pai[3] = 0.1;
    MakeNodeStructures( nullptr, true , Tai, Pai  );

    auto system_name = std::string(pmm->stkey, 0, EQ_RKLEN);
    std::vector<std::string> msg_data;
    if( add_head )
        msg_data.push_back("system");
    msg_data.push_back( datach_to_string( false, brief_mode ) );
    msg_data.push_back( gemipm_to_string( add_mui, false, brief_mode ));
    msg_data.push_back( databr_to_string( false, brief_mode ));
    if( GEMS3KGenerator::default_type_f>=GEMS3KGenerator::f_thermofun ) {
    std::stringstream fun_json;
    write_ThermoFun_format_stream(fun_json, true);
    msg_data.push_back(fun_json.str());
    node_logger->trace("Thermo {}", fun_json.str());
    }
    node_logger->info("Send system... {} type {}", system_name, static_cast<int>(GEMS3KGenerator::default_type_f));
    return msg_data;
}

bool TNodeGUI::set_resv_msg(std::vector<std::string> &&msg_return)
{
    //double time;
    long int NodeStatusCH = T_ERROR_GEM;

    if( msg_return.size() >= 2 )
        NodeStatusCH =  atol( msg_return[0].c_str() );
    else
        Error("TNodeGUI - run server: ", "Illegal number of messages" );

    switch( NodeStatusCH )
    {
    case OK_GEM_AIA:
    case OK_GEM_SIA: // unpack dbr data
        /*time =*/ readMultiServer( NodeStatusCH, msg_return );
        return true;
        break;
    case BAD_GEM_AIA:
    case BAD_GEM_SIA:  // unpack dbr data
        /*time =*/ readMultiServer( NodeStatusCH, msg_return );
        Error( *(msg_return.end()-2), msg_return.back() );
        break;
    case ERR_GEM_AIA:
    case ERR_GEM_SIA:
    case T_ERROR_GEM:
        Error( *(msg_return.end()-2), msg_return.back() );
    }
    return false;
}

// Reading structure MULTI (GEM IPM work structure)
double TNodeGUI::readMultiServer( long int NodeStatusCH, const std::vector<std::string>& recv_msg )
{
    double ret = 0;
    //auto new_dbr = send_msg[3];

    if( recv_msg.size() >= 3 && !recv_msg[2].empty() )
        ret = std::stod(recv_msg[2], nullptr );

    if( recv_msg.size() >= 2 && !recv_msg[1].empty() )
    {

        databr_from_string(recv_msg[1]);
        // Unpacking the actual contents of DBR file including speciation
        unpackDataBr( true );

        if( ( NodeStatusCH == OK_GEM_AIA || NodeStatusCH ==  OK_GEM_SIA) && recv_msg.size()>=6)
        {
            pmm->K2 =  atol( recv_msg[3].c_str() );
            pmm->ITF =  atol( recv_msg[4].c_str() );
            pmm->ITG =  atol( recv_msg[5].c_str() );
        }

        for( int j=0; j < pmm->L; j++ )
            pmm->X[j] = pmm->Y[j];
        pmm->TC = pmm->TCc;
        pmm->T =  pmm->Tc;
        pmm->P =  pmm->Pc;

        pmm->pESU = 2;  // SysEq unpack flag set

        TMulti* amulti = dynamic_cast<TMulti*>(multi_base);
        if( amulti)
            amulti->EqstatExpand( /*pmp->stkey,*/ true );
        pmm->FI1 = 0;  // Recomputing the number of non-zeroed-off phases
        pmm->FI1s = 0;
        for(int i=0; i<pmm->FI; i++ )
        {
            if( pmm->YF[i] >= min( pmm->PhMinM, 1e-22 ) )  // Check 1e-22 !!!!!
            {
                pmm->FI1++;
                if( i < pmm->FIs )
                    pmm->FI1s++;
            }
        }
        for( int i=0; i<pmm->L; i++)
            pmm->G[i] = pmm->G0[i];

    }
    ///   !!! G[] and F[] different after IPM and EqstatExpand
    return ret;
}

#endif
