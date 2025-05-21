#include "nodearray_gui.h"
#include "visor.h"
#include "v_mod.h"
#include "service.h"
#include "GEMS3K/gems3k_impex.h"

#ifndef _WIN32
#include <unistd.h>
#else
#include <io.h>
#endif

#ifdef USE_GEMS3K_SERVER
#ifdef NO_ASYNC_SERVER
#include "gemsreaktoro/zmq_req_client.hpp"
#else
#include "gemsreaktoro/zmq_client.hpp"
#endif
#endif


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

// Writing dataCH, dataBR structure to binary/text files
// and other necessary GEM2MT files
std::string TNodeArrayGUI::PutGEM2MTFiles(  QWidget* par, long int nIV,
                                          char  type_b, bool brief_mode, bool with_comments,
                                          bool putNodT1, bool addMui )
{
    // Get name of filenames structure
    std::string path = char_array_to_string( rt[RT_SYSEQ]->FldKey(2), rt[RT_SYSEQ]->FldLen(2));
    strip(path);
    GEMS3KGenerator::IOModes type_f=GEMS3KGenerator::f_key_value;
    switch( type_b)
    {
    case '-':
    case 'b': type_f = GEMS3KGenerator::f_binary; break;
    case 'j': type_f = GEMS3KGenerator::f_json; break;
    case 'f': type_f = GEMS3KGenerator::f_thermofun; break;
    case 'o': type_f = GEMS3KGenerator::f_kv_thermofun; break;
    }

    if( type_f==GEMS3KGenerator::f_binary )
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
    genGEMS3KInputFiles(  path, messageF, nIV, type_f, brief_mode, with_comments,
                        putNodT1, addMui );

    pVisor->CloseMessage();
    return path;
}

#ifdef USE_GEMS3K_SERVER

//   Here we call a loop on GEM calculations over nodes
//   parallelization should affect this loop only
//   return code   true   Ok
//                 false  Error in GEMipm calculation part
//
bool TNodeArrayGUI::CalcIPM_List( const TestModeGEMParam& modeParam, long int start_node,
                                  long int end_node, FILE* adiffile )
{
    mode_param = modeParam;
    diffile = adiffile;

    start_node = max( start_node, 0L );
    end_node = min( end_node, anNodes );
    reset_requests( start_node, end_node );

#ifdef NO_ASYNC_SERVER

    zmq_req_client_t<TNodeArrayGUI> zmqclient(*this);

    while( gems_task.has_next() )
    {
        zmqclient.run_task();
    }

#else
    zmq_client_t<TNodeArrayGUI> zmqclient(*this);
    zmqclient.start_list();
#endif

    return true;
}

//   Here we do a GEM calculation in box ii
//   return code   true   Ok
//                 false  Error in GEMipm calculation part
//
bool TNodeArrayGUI::CalcIPM_Node( const TestModeGEMParam& modeParam, TNode* ,
                                  long int ii, DATABRPTR* , DATABRPTR* , bool*, FILE* adiffile )
{
    mode_param = modeParam;
    sended_requests = ii;
    diffile = adiffile;

#ifdef NO_ASYNC_SERVER
    zmq_req_client_t<TNodeArrayGUI> zmqclient(*this);
#else
    zmq_client_t<TNodeArrayGUI> zmqclient(*this);
#endif

    return zmqclient.run_task();
}

std::vector<string> TNodeArrayGUI::generate_send_msg( bool add_head )
{
    DATABRPTR* C0 = pNodT0();
    DATABRPTR* C1 = pNodT1();
    bool* iaN = piaNode();     // indicators for IA in the nodes

    long int Mode = SmartMode( mode_param, sended_requests, iaN   );
    bool needGEM = NeedGEMS( calcNode, mode_param, C0[sended_requests], C1[sended_requests]  );

    if( !needGEM )
    {
        C1[sended_requests]->IterDone = 0; // number of GEMIPM iterations is set to 0
        sended_requests++;
        resv_responce++;
        return {}; // skip send-reply
    }

    // Copy data from the iNode node from array NodT1 to the work DATABR structure
    CopyWorkNodeFromArray( calcNode, sended_requests, anNodes, C1 );
    // GEM IPM calculation of equilibrium state in MULTI
    calcNode->pCNode()->NodeStatusCH = std::abs(Mode);
    sended_requests++;

    zmq_message_t send_msg;
    if( add_head )
        send_msg.push_back( std::to_string(calcNode->pCNode()->NodeHandle) );
    send_msg.push_back( calcNode->datach_to_string( false, false ) );
    send_msg.push_back( calcNode->gemipm_to_string( true, false, false ));
    send_msg.push_back( calcNode->databr_to_string( false, false ));
    if( GEMS3KGenerator::default_type_f>=GEMS3KGenerator::f_thermofun ) {
    std::stringstream fun_json;
    calcNode->write_ThermoFun_format_stream(fun_json, true);
    send_msg.push_back(fun_json.str());
    }
    TNode::node_logger->debug("Send NodeHandle... {}", calcNode->pCNode()->NodeHandle);
    return send_msg;
}


bool TNodeArrayGUI::set_resv_msg( std::vector<string> &&recv_message )
{
    long int  NodeStatusCH = T_ERROR_GEM;
    long int node_handle=-1;
    resv_responce++;

    if( recv_message.size() >= 2 )
        NodeStatusCH  =  atol( recv_message[0].c_str() );
    else
        Error("RunGEM", "Illegal number of messages" );

    switch( NodeStatusCH )
    {
    case BAD_GEM_AIA:
    case BAD_GEM_SIA:  // unpack dbr data
    case OK_GEM_AIA:
    case OK_GEM_SIA:
        // unpack dbr data
        calcNode->databr_from_string( recv_message[1] );
        node_handle = calcNode->pCNode()->NodeHandle;
        break;
    case ERR_GEM_AIA:
    case ERR_GEM_SIA:
    case T_ERROR_GEM:
        break;
    }

    if( NodeStatusCH == OK_GEM_AIA || NodeStatusCH ==  OK_GEM_SIA )
    {
        // Copying data for node iNode back from work DATABR structure into the node array
        //   if( retCode == OK_GEM_AIA ||
        //       retCode == OK_GEM_PIA  )
        MoveWorkNodeToArray( calcNode, node_handle, anNodes, pNodT1() );
    }
    else
    {
        std::string err_msg = ErrorGEMsMessage( NodeStatusCH, node_handle, mode_param.step  );

        if( diffile )
        {
            // write to file here
            fprintf( diffile, "\nError reported from GEMS3K module\n%s\n",
                     err_msg.c_str() );
        }
        else
        {
            err_msg += "\n Continue?";
            if( !vfQuestion( nullptr /*TGEM2MT::pm->window()*/,
                             "Error reported from GEMIPM2 module",err_msg ))
                Error("Error reported from GEMIPM2 module",
                      "Process stopped by the user");
        }
        return false;
    }
    return true;
}

#endif

void TNodeArrayGUI::pVisor_Message(bool toclose, long int ndx, long int size1 )
{
    if( toclose )
        pVisor->CloseMessage();
    else
        pVisor->Message( nullptr, "GEM2MT node array",
                         "Reading from disk a set of node array files to resume an interrupted RMT task. "
                         "Please, wait...", ndx, size1 );
}

