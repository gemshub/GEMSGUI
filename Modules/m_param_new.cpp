//-------------------------------------------------------------------
// $Id: m_prfget.cpp 1360 2009-07-15 13:37:30Z gems $
//
// Implementation of TProfile class, loading, making and calc functions
//
// Rewritten from C to C++ by S.Dmytriyeva
// Copyright (C) 1995-2001 S.Dmytriyeva, D.Kulik
//
// This file is part of a GEM-Selektor library for thermodynamic
// modelling by Gibbs energy minimization
// Uses: GEM-Selektor GUI GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the GPL v.3 license

//
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//

//#define OLD

#include "m_syseq.h"
#include "visor.h"
#include "nodearray.h"
#include "zmqclient.h"
#include "GemsMainWindow.h"

#ifdef OLD

// Run process of calculate equilibria into the GEMSGUI shell
double  TProfil::CalculateEquilibriumGUI( const gstring& lst_path )
{
    bool brief_mode = false;
    bool add_mui = true;

    CurrentSystem2GEMS3K( lst_path, brief_mode, add_mui );

    // run gem_ipm
    auto ret = CalculateEquilibriumServerOld( lst_path );

    CmReadMultiServer( lst_path.c_str() );

    return ret;
}

// Run process of calculate equilibria into the GEMS3K side
double  TProfil::CalculateEquilibriumServerOld( const gstring& lst_f_name )
{
    double ret=0.;
    try
    {
        std::string path = lst_f_name.c_str();

        //  Prepare our context and socket
        zmq::context_t context (1);
        zmq::socket_t socket (context, ZMQ_REQ);

        std::cout << "Connecting to hello world server…" << std::endl;
        socket.connect ("tcp://localhost:5555");

        //  Do request, waiting each time for a response

        zmq::send_flags snd_flags=zmq::send_flags::none;
        zmq::message_t request (path.begin(), path.end());
        std::cout << "Sending:" << request.str() << std::endl;
        socket.send (request, snd_flags);

        //  Get the reply.
        zmq::recv_flags rsv_flag = zmq::recv_flags::none;
        zmq::message_t reply;
        socket.recv (reply, rsv_flag);
        std::cout << "Received:" << reply.str() << std::endl;
        ret = std::stod(reply.to_string(), nullptr );


    }catch(TError& err)
    {
    }
    catch(...)
    {

    }
    return ret;
}

// Reading structure MULTI (GEM IPM work structure)
void TProfil::CmReadMultiServer( const char* path )
{
    multi->dyn_set();

    MULTI* pmp = multi->GetPM();
    std::shared_ptr<TNode> na( new TNode( pmp ));

    if( na->GEM_init( path ) )
    {
        Error( path, "GEMS3K Init() error: \n"
                     "Some GEMS3K input files are corrupt or cannot be found.");
    }

    // Unpacking the actual contents of DBR file including speciation
    na->unpackDataBr( true );
    for( int j=0; j < pmp->L; j++ )
        pmp->X[j] = pmp->Y[j];
    pmp->TC = pmp->TCc;
    pmp->T =  pmp->Tc;
    pmp->P =  pmp->Pc;

    pmp->pESU = 2;  // SysEq unpack flag set

    multi->EqstatExpand( /*pmp->stkey,*/ true );
    pmp->FI1 = 0;  // Recomputing the number of non-zeroed-off phases
    pmp->FI1s = 0;
    for(int i=0; i<pmp->FI; i++ )
    {
        if( pmp->YF[i] >= min( pmp->PhMinM, 1e-22 ) )  // Check 1e-22 !!!!!
        {
            pmp->FI1++;
            if( i < pmp->FIs )
                pmp->FI1s++;
        }
    }
    for( int i=0; i<pmp->L; i++)
        pmp->G[i] = pmp->G0[i];

    //cout << setprecision(15) <<" pmp->Y_la[4] " << pmp->Y_la[4] << " pmp->lnGam[4] " << pmp->lnGam[4] << endl;
    ///   !!! G[] and F[] different after IPM and EqstatExpand
}


#else

// Run process of calculate equilibria into the GEMSGUI shell
double  TProfil::CalculateEquilibriumGUI( const gstring& amode )
{
    std::string mode = amode.c_str();
    bool brief_mode = false;
    bool add_mui = true;
    long int NodeStatusCH = T_ERROR_GEM;
    double time = 0.;

    auto send_msg = CurrentSystem2GEMS3Kjson(  brief_mode, add_mui );
    //for( const auto& msg: msg_data )
    //  std::cout << msg <<  "\n----------------------\n"  << std::endl;

    // run gem_ipm
    auto recv_message = pVisorImp->getZMQclient()->calculateEquilibriumServer(send_msg);
    //for( const auto& msg: recv_message )
    //  std::cout << msg <<  "\n----------------------\n"  << std::endl;

    if( recv_message.size() >= 2 )
        NodeStatusCH =  atol( recv_message[0].c_str() );
    else
        Error("CalculateEquilibriumGUI", "Illegal number of messages" );

    switch( NodeStatusCH )
    {
    case OK_GEM_AIA:
    case OK_GEM_SIA: // unpack dbr data
        time = readMultiServer( NodeStatusCH, send_msg, recv_message );
        break;
    case BAD_GEM_AIA:
    case BAD_GEM_SIA:  // unpack dbr data
        time = readMultiServer( NodeStatusCH, send_msg, recv_message );
        Error( (recv_message.end()-1)->c_str(), recv_message.back().c_str() );
        break;
    case ERR_GEM_AIA:
    case ERR_GEM_SIA:
    case T_ERROR_GEM:
        Error( (recv_message.end()-1)->c_str(), recv_message.back().c_str() );
    }

    return time;
}

std::vector<std::string> TProfil::CurrentSystem2GEMS3Kjson( bool brief_mode, bool add_mui )
{
    std::vector<std::string> msg_data;

    double Tai[4], Pai[4];
    std::unique_ptr<TNodeArray> na;
    MULTI *pmp = TMulti::sm->GetPM();

    Tai[0] = Tai[1] = pmp->TCc;
    Pai[0] = Pai[1] = pmp->Pc;
    Tai[2] = Pai[2] = 0.;
    Tai[3] = Pai[3] = 0.1;

    na.reset( new TNodeArray( 1, pmp )) ;
    // realloc and setup data for dataCH and DataBr structures
    na->MakeNodeStructuresOne( nullptr, true , Tai, Pai  );

    msg_data.push_back( "system" );
    msg_data.push_back( na->getCalcNode().datach_to_string( false, brief_mode ) );
    msg_data.push_back( gemipm_to_string( add_mui, false, brief_mode ));
    msg_data.push_back( na->getCalcNode().databr_to_string( false, brief_mode ));
    return msg_data;
}

// Run process of calculate equilibria into the GEMS3K side
std::vector<std::string>  TProfil::CalculateEquilibriumServer( const std::vector<std::string>& msg_data )
{
    return pVisorImp->getZMQclient()->calculateEquilibriumServer(msg_data);
}

// Reading structure MULTI (GEM IPM work structure)
double TProfil::readMultiServer( long int NodeStatusCH, const std::vector<std::string>& send_msg, const std::vector<std::string>& recv_msg )
{
    double ret = 0;
    auto new_dbr = send_msg[3];

    if( recv_msg.size() >= 3 && !recv_msg[2].empty() )
        ret = std::stod(recv_msg[2], nullptr );

    if( recv_msg.size() >= 2 && !recv_msg[1].empty() )
    {
        new_dbr = recv_msg[1];
        //std::cout << new_dbr << std::endl;
    }

    multi->dyn_set();

    MULTI* pmp = multi->GetPM();
    std::shared_ptr<TNode> na( new TNode( pmp ));

    if( na->GEM_init( send_msg[1], send_msg[2], new_dbr ) )
    {
        Error( "GEMS3K Init() error:",  "Some GEMS3K data illegal.");
    }

    // Unpacking the actual contents of DBR file including speciation
    na->unpackDataBr( true );

    if( ( NodeStatusCH == OK_GEM_AIA || NodeStatusCH ==  OK_GEM_SIA) && recv_msg.size()>=6)
    {
        pmp->K2 =  atol( recv_msg[3].c_str() );
        pmp->ITF =  atol( recv_msg[4].c_str() );
        pmp->ITG =  atol( recv_msg[5].c_str() );
    }

    for( int j=0; j < pmp->L; j++ )
        pmp->X[j] = pmp->Y[j];
    pmp->TC = pmp->TCc;
    pmp->T =  pmp->Tc;
    pmp->P =  pmp->Pc;

    pmp->pESU = 2;  // SysEq unpack flag set

    multi->EqstatExpand( /*pmp->stkey,*/ true );
    pmp->FI1 = 0;  // Recomputing the number of non-zeroed-off phases
    pmp->FI1s = 0;
    for(int i=0; i<pmp->FI; i++ )
    {
        if( pmp->YF[i] >= min( pmp->PhMinM, 1e-22 ) )  // Check 1e-22 !!!!!
        {
            pmp->FI1++;
            if( i < pmp->FIs )
                pmp->FI1s++;
        }
    }
    for( int i=0; i<pmp->L; i++)
        pmp->G[i] = pmp->G0[i];

    //cout << setprecision(15) <<" pmp->Y_la[4] " << pmp->Y_la[4] << " pmp->lnGam[4] " << pmp->lnGam[4] << endl;
    ///   !!! G[] and F[] different after IPM and EqstatExpand
    return ret;
}


#endif


// http://zguide.zeromq.org/cpp:hwserver

// http://zguide.zeromq.org/cpp:hwclient

/*

Install

OSX (MacOS)

You need Brew installed and configured https://brew.sh/

>  brew install zmq

czmq and zyre are also available.

Linux

> apt-get install libzmq3-dev


// no example
https://github.com/zeromq/cppzmq
// exist example
https://github.com/zeromq/zmqpp



git clone https://github.com/zeromq/libzmq.git


git clone https://github.com/zeromq/cppzmq.git




*/




//  https://pastebin.com/53ArzzsK
