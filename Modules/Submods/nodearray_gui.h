#ifndef NODEARRAY_GUI_H
#define NODEARRAY_GUI_H

#include "nodearray.h"
#include "node_gui.h"
class QWidget;

class TNodeArrayGUI : public TNodeArray
{
public:

    // These calls are used only inside of GEMS-PSI GEM2MT module

    /// Constructor for integration in GEM2MT module of GEMS-PSI
    TNodeArrayGUI( long int nNodes, TMultiBase *apm );

    /// Constructor that uses 3D node arrangement
    TNodeArrayGUI( long int asizeN, long int asizeM, long int asizeK, TMultiBase *apm );

    ///  Here we do a GEM calculation in boxes from  start_node to end_node
    bool CalcIPM_List( const TestModeGEMParam& modeParam, long int start_node, long int end_node, FILE* diffile ) override;

    ///  Here we run command to setup GEMS3_server for GEM calculation in boxes from  start_node to end_node
    bool InitNodeServer();
    ///  Here we run command a GEM calculation in box iNode on to GEMS3_server
    long int CalcNodeServer(TNode& wrkNode, long int  iNode, long int Mode) override;

    /// Prints MULTI, DATACH and DATABR files structure prepared from GEMS.
    /// Prints files for separate coupled FMT-GEM programs that use GEMS3K module
    /// or if putNodT1 == true  as a break point for the running FMT calculation
    /// \param nIV - Number of allocated nodes
    /// \param bin_mode - Write IPM, DCH and DBR files in binary mode ( false - txt mode)
    /// \param brief_mode - Do not write data items that contain only default values
    /// \param with_comments -Write files with comments for all data entries ( in text mode)
    /// \param addMui - Print internal indices in RMULTS to IPM file for reading into Gems back
    std::string PutGEM2MTFiles(  QWidget* par, long int nIV,
                                 bool bin_mode = false, bool brief_mode = false, bool with_comments = true,
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

    const TNode& LinkToNode( long int ndx, long int nNodes, DATABRPTR* anyNodeArray )
    {
        CopyWorkNodeFromArray( calcNode, ndx, nNodes, anyNodeArray );
        return calcNode;
    }

    void SaveToNode( long int ndx, long int nNodes, DATABRPTR* anyNodeArray )
    {
        // Save databr
        calcNode.packDataBr();
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
            anyNodeArray[ndx] = calcNode.databr_free( anyNodeArray[ndx] );
            delete[] anyNodeArray[ndx];
        }
        anyNodeArray[ndx] = allocNewDBR( calcNode );
        if( !NodT1[ndx] )
            NodT1[ndx] = allocNewDBR( calcNode);
        return anyNodeArray[ndx];
    }


protected:

    TNodeGUI* calcNodeGUI;

    ///  Here we do a GEM calculation in box ii (implementation thread-safe)
    bool CalcIPM_Node(  const TestModeGEMParam& modeParam, TNode& wrkNode,
                        long int ii, DATABRPTR* C0, DATABRPTR* C1, bool* iaN, FILE* diffile  ) override;

    void pVisor_Message( bool toclose, long int ndx = 0, long int size = 0 ) override;

};

#endif // NODEARRAY_GUI_H
