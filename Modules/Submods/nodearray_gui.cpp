#include <unistd.h>
//#include "nodearray_gui.h"
#include "m_gem2mt.h"
#include "zmqclient.h"
#include "visor.h"


TNodeArrayGUI::TNodeArrayGUI( long int nNod, TMultiBase *apm  ):
    TNodeArray(nNod)
{
    calcNodeGUI = new TNodeGUI(apm);
    calcNode = calcNodeGUI;
    internal_Node.reset( calcNodeGUI );
}

TNodeArrayGUI::TNodeArrayGUI( long int asizeN, long int asizeM, long int asizeK, TMultiBase *apm  ):
    TNodeArray(asizeN, asizeM, asizeK)
{
    calcNode = calcNodeGUI = new TNodeGUI(apm);
    internal_Node.reset( calcNodeGUI );
}


//   Here we call a loop on GEM calculations over nodes
//   parallelization should affect this loop only
//   return code   true   Ok
//                 false  Error in GEMipm calculation part
//
bool TNodeArrayGUI::CalcIPM_List( const TestModeGEMParam& modeParam, long int start_node, long int end_node, FILE* diffile )
{
    long int ii;
    bool iRet = true;
    DATABRPTR* C0 = pNodT0();
    DATABRPTR* C1 = pNodT1();
    bool* iaN = piaNode();     // indicators for IA in the nodes

    start_node = max( start_node, 0L );
    end_node = min( end_node, anNodes );

    for( ii = start_node; ii<= end_node; ii++) // node iteration
    {
        if( !CalcIPM_Node(  modeParam, calcNode, ii, C0, C1, iaN, diffile ) )
            iRet = false;
    }

    return iRet;
}

//   Here we do a GEM calculation in box ii
//   return code   true   Ok
//                 false  Error in GEMipm calculation part
//
bool TNodeArrayGUI::CalcIPM_Node( const TestModeGEMParam& modeParam, TNode* wrkNode,
                                  long int ii, DATABRPTR* C0, DATABRPTR* C1, bool* piaN, FILE* diffile )
{
    bool iRet = true;

    long int Mode = SmartMode( modeParam, ii, piaN   );
    bool needGEM = NeedGEMS( wrkNode, modeParam, C0[ii], C1[ii]  );

    if( needGEM )
    {
        long RetCode =  RunGEM( wrkNode, ii, Mode, C1 );

        // checking RetCode from GEM IPM calculation
        if( !(RetCode==OK_GEM_AIA || RetCode == OK_GEM_SIA ))
        {
            std::string err_msg = ErrorGEMsMessage( RetCode,  ii, modeParam.step  );
            iRet = false;

            if( diffile )
            {
                // write to file here
                fprintf( diffile, "\nError reported from GEMS3K module\n%s\n",
                         err_msg.c_str() );
            }
            else
            {
                err_msg += "\n Continue?";
                if( !vfQuestion( TGEM2MT::pm->window(),
                                 "Error reported from GEMIPM2 module",err_msg.c_str() ))
                    Error("Error reported from GEMIPM2 module",
                          "Process stopped by the user");
            }
        }
    }
    else { // GEM calculation for this node not needed
        C1[ii]->IterDone = 0; // number of GEMIPM iterations is set to 0
    }
    return iRet;
}

long int  TNodeArrayGUI::CalcNodeServer( TNode* wrkNode, long int  iNode, long int )
{
    long int  retCode = T_ERROR_GEM;

    zmq_message_t send_msg;
    send_msg.push_back("dbr");
    send_msg.push_back( wrkNode->databr_to_string( false, false ));
    send_msg.push_back( std::to_string(iNode) );

    auto recv_message = TProfil::pm->CalculateEquilibriumServer( send_msg );

    if( recv_message.size() >= 2 )
        retCode  =  atol( recv_message[0].c_str() );
    else
        Error("RunGEM", "Illegal number of messages" );

    if( retCode == OK_GEM_AIA || retCode ==  OK_GEM_SIA )
    {
        wrkNode->databr_from_string(recv_message[1]);
    }

    return retCode;
}

bool TNodeArrayGUI::InitNodeServer()
{
    na = this; // temporaly fix
    zmq_message_t send_msg;
    send_msg.push_back( "nodearray" );
    send_msg.push_back( calcNode->datach_to_string( false, false ) );
    send_msg.push_back( calcNode->gemipm_to_string( true, false, false ));
    send_msg.push_back( calcNode->databr_to_string( false, false ));

    auto recv_message = TProfil::pm->CalculateEquilibriumServer( send_msg );

    if( recv_message.size() >= 2 )
    {
        Error(recv_message[0].c_str(), recv_message[1].c_str() );
    }

    return true;
}



// Writing dataCH, dataBR structure to binary/text files
// and other necessary GEM2MT files
std::string TNodeArrayGUI::PutGEM2MTFiles(  QWidget* par, long int nIV,
                                            bool bin_mode, bool brief_mode, bool with_comments,
                                            bool putNodT1, bool addMui )
{
    // Get name of filenames structure
    std::string path = std::string( rt[RT_SYSEQ].FldKey(2), 0, rt[RT_SYSEQ].FldLen(2));;
    strip(path);
    if( bin_mode )
        path += "-bin.lst";
    else
        path += "-dat.lst";

AGAIN:
    // open file to output
    if( vfChooseFileSave(par, path, "Please, enter IPM work structure file name", "*.lst" ) == false )
        return "";

    if( !access(path.c_str(), 0 ) ) //file exists
        switch( vfQuestion3( par, path.c_str(), "This set of files exists!",
                             "&Overwrite", "&Rename", "&Cancel") )
        {
        case VF3_2:
            goto AGAIN;
        case VF3_1:
            break;
        case VF3_3:
            return path;
        }

    ProcessProgressFunction messageF = [nIV, par](const std::string& message, long point){
        return  pVisor->Message( par, "GEM2MT node array",  message.c_str() , point, nIV );
    };
    genGEMS3KInputFiles(  path, messageF, nIV, bin_mode, brief_mode, with_comments,
                          putNodT1, addMui );

    pVisor->CloseMessage();
    return path;
}

void TNodeArrayGUI::pVisor_Message(bool toclose, long int ndx, long int size )
{
    if( toclose )
        pVisor->CloseMessage();
    else
        pVisor->Message( nullptr, "GEM2MT node array",
                         "Reading from disk a set of node array files to resume an interrupted RMT task. "
                         "Please, wait...", ndx, size );
}


