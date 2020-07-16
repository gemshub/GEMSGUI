#ifndef NODE_GUI_H
#define NODE_GUI_H

#include "GEMS3K/node.h"
#include "array.h"
class QWidget;

class TNodeGUI: public TNode
{

public:

    //static TNode* na;   // static pointer to this TNode class instance

    /// Constructor of the class instance in memory in GEMS environment
    explicit TNodeGUI( TMultiBase* apm );

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


    /// Generate request strings for ZMQ server
    std::vector<std::string> generate_send_msg( bool add_head );

    /// Set up data from response strings from ZMQ server
    /// \return true if success
    bool set_resv_msg( std::vector<std::string>&& msg_return );


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


    // Virtual function for interaction with TNodeArray class
    //virtual void  setNodeArray( std::string& , long int , bool ) { }
    void init_into_gems3k() override {}

    double readMultiServer(long NodeStatusCH, const std::vector<std::string> &recv_msg);
};

#endif // NODE_GUI_H
