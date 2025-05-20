//-------------------------------------------------------------------
// $Id$
/// \file nodearray.h
/// Contains declaration of TNodeArray class implementing an advanced
/// interface for development of coupled codes involving GEMS3K.
//
/// \class TNodeArray nodearray.h
/// Implements an advanced (level 2) C/C++ interface with GEMS3K for the
/// development of coupled reactive transport codes.
/// Works with DATACH and an array of DATABR structures; uses TNode class
//
// Copyright (C) 2006-2020 S.Dmytriyeva, D.Kulik
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
//

#ifndef NODEARRAY_GUI_H
#define NODEARRAY_GUI_H

#include "GEMS3K/nodearray.h"
#include "node_gui.h"
class QWidget;


class TNodeArrayGUI : public TNodeArray
{
    TNodeGUI* calcNodeGUI;

    TestModeGEMParam mode_param;
    FILE* diffile = nullptr;

#ifdef USE_GEMS3K_SERVER
    size_t requests_number=0;
    size_t sended_requests = 0;
    size_t resv_responce = 0;

    ///  Here we do a GEM calculation in box ii (implementation thread-safe)
    bool CalcIPM_Node(  const TestModeGEMParam& modeParam, TNode* wrkNode,
                        long int ii, DATABRPTR* C0, DATABRPTR* C1, bool* iaN, FILE* diffile  ) override;
#endif

    void pVisor_Message( bool toclose, long int ndx = 0, long int size = 0 ) override;

    // These calls are used only inside of GEMS-PSI GEM2MT module
    /// Constructor for integration in GEM2MT module of GEMS-PSI
    TNodeArrayGUI(long int nNodes, TMultiBase *apm);

    /// Constructor that uses 3D node arrangement
    TNodeArrayGUI(long int asizeN, long int asizeM, long int asizeK, TMultiBase *apm);

public:
    // Used in GEMIPM2 standalone module only
    /// Constructors for 1D arrangement of nodes
    [[nodiscard]] static std::shared_ptr<TNodeArrayGUI> create(long int nNodes, TMultiBase *apm) {
        auto ret = std::shared_ptr<TNodeArrayGUI>(new TNodeArrayGUI(nNodes, apm));
        na = ret.get();
        return ret;
    }
    /// Constructor that uses 3D node arrangement
    [[nodiscard]] static std::shared_ptr<TNodeArrayGUI> create(long int asizeN, long int asizeM, long int asizeK, TMultiBase *apm) {
        auto ret = std::shared_ptr<TNodeArrayGUI>(new TNodeArrayGUI(asizeN, asizeM, asizeK, apm));
        na = ret.get();
        return ret;
    }

    /// Prints MULTI, DATACH and DATABR files structure prepared from GEMS.
    /// Prints files for separate coupled FMT-GEM programs that use GEMS3K module
    /// or if putNodT1 == true  as a break point for the running FMT calculation
    /// \param nIV - Number of allocated nodes
    /// \param type_b    defines if the file is in binary format ('b'), in text format ('t') or in json format ('j').
    /// \param brief_mode - Do not write data items that contain only default values
    /// \param with_comments -Write files with comments for all data entries ( in text mode)  or as "pretty JSON"  ( json mode )
    /// \param addMui - Print internal indices in RMULTS to IPM file for reading into Gems back
    std::string PutGEM2MTFiles(  QWidget* par, long int nIV,
                                 char  type_b, bool brief_mode = false, bool with_comments = true,
                                 bool putNodT1=false, bool addMui=false );


    /// Overloaded variant - takes lists of ICs, DCs and phases according to
    /// already existing index vectors axIC, axDC, axPH (with anICb, anDCb,
    /// anPHb, respectively)
    void InitCalcNodeStructures(  long int anICb, long int anDCb,  long int anPHb,
                                  long int* axIC, long int* axDC,  long int* axPH, bool no_interpolat,
                                  double* Tai, double* Pai,  long int nTp_,
                                  long int nPp_, double Ttol_, double Ptol_  )
    {
        calcNodeGUI->MakeNodeStructures(  anICb, anDCb,  anPHb,
                                          axIC, axDC,  axPH, no_interpolat,
                                          Tai,  Pai,  nTp_,  nPp_, Ttol_,  Ptol_  ) ;
    }

    /// Makes start DATACH and DATABR data using GEMS internal data (MULTI and other)
    /// interaction variant. The user must select ICs, DCs and phases to be included
    /// in DATABR lists
    /// Lookup arays from iterators
    void MakeNodeStructuresOne( QWidget* par, bool select_all,
                                double Tai[4], double Pai[4]  )
    {
        calcNodeGUI->MakeNodeStructures( par, select_all,  Tai, Pai  );
        // setup dataBR and NodeT0 data
        NodT0[0] = allocNewDBR( calcNode);
        NodT1[0] = allocNewDBR( calcNode);
        MoveWorkNodeToArray( calcNode, 0, 1, na->pNodT0() );
        MoveWorkNodeToArray( calcNode, 0, 1, na->pNodT1() );
    }

    /// Makes start DATACH and DATABR data using GEMS internal data (MULTI and other)
    /// interaction variant. The user must select ICs, DCs and phases to be included
    /// in DATABR lists
    void MakeNodeStructuresOne( QWidget* par, bool select_all,bool no_interpolat,
                                double *Tai, double *Pai, long int nTp_ = 1 ,
                                long int nPp_ = 1 , double Ttol_ = 1., double Ptol_ =1. )
    {
        calcNodeGUI->MakeNodeStructures( par, select_all, no_interpolat,
                                         Tai, Pai, nTp_, nPp_, Ttol_, Ptol_ );
        NodT0[0] = allocNewDBR( calcNode);
        NodT1[0] = allocNewDBR( calcNode);
        MoveWorkNodeToArray( calcNode, 0, 1, na->pNodT0() );
        MoveWorkNodeToArray( calcNode, 0, 1, na->pNodT1() );
    }

    const TNode* LinkToNode( long int ndx, long int nNodes, DATABRPTR* anyNodeArray )
    {
        CopyWorkNodeFromArray( calcNode, ndx, nNodes, anyNodeArray );
        return calcNode;
    }

    void SaveToNode( long int ndx, long int nNodes, DATABRPTR* anyNodeArray )
    {
        // Save databr
        calcNode->packDataBr();
        if( !NodT0[ndx] )
            NodT0[ndx] = allocNewDBR( calcNode);
        if( !NodT1[ndx] )
            NodT1[ndx] = allocNewDBR( calcNode);
        MoveWorkNodeToArray( calcNode, ndx, nNodes, anyNodeArray );
    }

    DATABR *reallocDBR( long int ndx, long int nNodes, DATABRPTR* anyNodeArray)
    {
        if( ndx < 0 || ndx>= nNodes )
            return nullptr;
        // free old memory
        if( anyNodeArray[ndx] )
        {
            anyNodeArray[ndx] = calcNode->databr_free( anyNodeArray[ndx] );
            delete[] anyNodeArray[ndx];
        }
        anyNodeArray[ndx] = allocNewDBR( calcNode );
        if( !NodT1[ndx] )
            NodT1[ndx] = allocNewDBR( calcNode);
        return anyNodeArray[ndx];
    }

#ifdef USE_GEMS3K_SERVER
    ///  Here we do a GEM calculation in boxes from  start_node to end_node
    bool CalcIPM_List( const TestModeGEMParam& modeParam, long int start_node, long int end_node, FILE* diffile ) override;

    /// False if all subtasks were calculated
    bool wait_next()  const
    {
      return resv_responce <= requests_number;
    }

    /// False if all subtasks were calculated
    bool has_next()  const
    {
      return sended_requests <= requests_number;
    }

    /// Generate request strings for ZMQ server
    std::vector<std::string> generate_send_msg( bool add_head );

    /// Set up data from response strings from ZMQ server
    /// \return true if success
    bool set_resv_msg( std::vector<std::string>&& msg_return );

    void reset_requests( size_t start_node,
                         size_t end_node )
    {
        requests_number=end_node;
        sended_requests = start_node;
        resv_responce = start_node;
    }

#endif

};

#endif // NODEARRAY_GUI_H
