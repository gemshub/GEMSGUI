//-------------------------------------------------------------------
// $Id$
/// \file node.h
/// Declaration of TNode class that implements a simple C/C++ interface
/// between GEMS3K and another code.
//
/// \class TNode node.h
/// Implements a simple C/C++ interface between GEM IPM and FMT codes.
/// Works with DATACH and work DATABR structures without using
/// the TNodearray class.
//
// Copyright (c) 2006-2021 S.Dmytriyeva, D.Kulik, G.Kosakowski, G.D.Miron, A.Leal
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

// You should have received a copy of the GNU General Public License
// along with GEMS3K code. If not, see <http://www.gnu.org/licenses/>.
//-------------------------------------------------------------------

#ifndef NODE_GUI_H
#define NODE_GUI_H

#include "GEMS3K/node.h"
#include "v_user.h"
class QWidget;


/// \class TNode (GEMS3K kernel)
/// Implements a simple C/C++ interface between GEM IPM and FMT codes.
/// Works with DATACH and work DATABR structures without using
/// the TNodearray class.
class TNodeGUI: public TNode
{

public:

    /// Constructor of the class instance in memory in GEMS environment
    explicit TNodeGUI(TMultiBase* apm);
    TNodeGUI( const TNodeGUI& otherNode ) = delete;

    void packDataBr() override   ///<  Packs GEMIPM calculation results into work node structure
    {
        //   CNode->NodeHandle = 0;
        //   CNode->NodeTypeHY = normal;
           CNode->NodeTypeMT = normal;
           CNode->NodeStatusFMT = Initial_RUN;
           TNode::packDataBr();
    }

    // These calls are used only inside the GEMS-PSI GEM2MT module

    /// Makes start DATACH and DATABR data using GEMS internal data (MULTI and other)
    /// interaction variant. The user must select ICs, DCs and phases to be included
    /// in DATABR lists
    void MakeNodeStructures( QWidget* par, bool select_all,bool no_interpolat,
                             double *Tai, double *Pai, long int nTp_ = 1 ,
                             long int nPp_ = 1 , double Ttol_ = 1., double Ptol_ =1. );
    /// Makes start DATACH and DATABR data using GEMS internal data (MULTI and other)
    /// interaction variant. The user must select ICs, DCs and phases to be included
    /// in DATABR lists
    /// Lookup arays from iterators
    void MakeNodeStructures( QWidget* par, bool select_all,
                             double Tai[4], double Pai[4]  );


    /// Overloaded variant - takes lists of ICs, DCs and phases according to
    /// already existing index vectors axIC, axDC, axPH (with anICb, anDCb,
    /// anPHb, respectively)
    void MakeNodeStructures(  long int anICb, long int anDCb,  long int anPHb,
                              long int* axIC, long int* axDC,  long int* axPH, bool no_interpolat,
                              double* Tai, double* Pai,  long int nTp_,
                              long int nPp_, double Ttol_, double Ptol_  );

    /// Test temperature and pressure values for the interpolation grid
    bool TestTPGrid(  double Tai[4], double Pai[4] );

#ifdef USE_GEMS3K_SERVER
    /// Generate request strings for ZMQ server
    std::vector<std::string> generate_send_msg( bool add_head );

    /// Set up data from response strings from ZMQ server
    /// \return true if success
    bool set_resv_msg( std::vector<std::string>&& msg_return );
#endif

    /// The export to ThermoFun JSON format file should include all IComp, DComp and ReacDC records
    /// from the project database, not just the records needed for a particular system
    /// (where some elements, DComps or ReacDCs can be switched off) as done in preparation of DCH lookup arrays.
    ///  \param stream     stream to output json file
    virtual void  write_ThermoFun_format_stream(std::iostream& stream, bool compact) override;

protected:

    // Integration in GEMS-PSI GUI environment
    // Prepares and writes DCH and DBR files for reading into the coupled code
    void makeStartDataChBR( QWidget* par, bool no_interpolat,
                            TCIntArray& selIC, TCIntArray& selDC, TCIntArray& selPH,
                            long int nTp_, long int nPp_, double Ttol_, double Ptol_,
                            double *Tai, double *Pai );
    void makeStartDataChBR( QWidget* par,
                            TCIntArray& selIC, TCIntArray& selDC, TCIntArray& selPH,
                            double Tai[4], double Pai[4] );

    // Building internal dataCH and DataBR structures from Multi
    void setupDataChBR( TCIntArray& selIC, TCIntArray& selDC, TCIntArray& selPH,
                        long int nTp_, long int nPp_, bool use_grid );
    // Build lists names of components for selection into DataBridge
    void getDataBridgeNames( QWidget* par, bool select_all,
                             TCIntArray& selIC, TCIntArray& selDC, TCIntArray& selPH  );

    void init_into_gems3k() override {}

#ifdef USE_GEMS3K_SERVER
    double readMultiServer(long NodeStatusCH, const std::vector<std::string> &recv_msg);
#endif

};

#endif // NODE_GUI_H
