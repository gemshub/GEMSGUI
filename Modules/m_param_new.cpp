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

#define OLD

#include "m_syseq.h"
#include "visor.h"
#include "nodearray.h"
#include <zmq.hpp>
#include <zmq_addon.hpp>

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

   // run gem_ipm
   auto ret = CalculateEquilibriumServer( mode, add_mui, brief_mode  );

   return ret;
}


// Run process of calculate equilibria into the GEMS3K side
double  TProfil::CalculateEquilibriumServer( const std::string& amode, bool addMui, bool brief_mode )
{
    double ret=0.;
    try
    {
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

        std::string mode = "system";
        auto dchjson = na->getCalcNode().datach_to_string( false, brief_mode );
        auto ipmjson = gemipm_to_string( addMui, false, brief_mode );
        auto dbrjson = na->getCalcNode().databr_to_string( false, brief_mode );

        //cout << dchjson <<  "\n\n-------------------------------------------\n\n"  << std::endl;
        //cout << ipmjson <<  "\n\n-------------------------------------------\n\n"  << std::endl;
        cout << dbrjson <<  "\n\n-------------------------------------------\n\n"  << std::endl;

        //  Prepare our context and socket
        zmq::context_t context (1);
        zmq::socket_t socket (context, ZMQ_REQ);

        std::cout << "Connecting to hello world server…" << std::endl;
        socket.connect ("tcp://localhost:5555");

        //  Do request, waiting each time for a response
        std::vector<zmq::message_t> msgs_vec;
        msgs_vec.push_back( zmq::message_t(mode.begin(), mode.end()));
        msgs_vec.push_back( zmq::message_t(dchjson.begin(), dchjson.end()));
        msgs_vec.push_back( zmq::message_t(ipmjson.begin(), ipmjson.end()));
        msgs_vec.push_back( zmq::message_t(dbrjson.begin(), dbrjson.end()));
        auto iret = zmq::send_multipart(socket, msgs_vec);


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

#endif


/* Run process of calculate equilibria into the GEMS3K side
double  TProfil::CalculateEquilibriumServer( const gstring& lst_f_name )
{
    double ret=0.;
    try
    {
        auto dbr_lst_f_name = lst_f_name;
        dbr_lst_f_name = dbr_lst_f_name.replace("-dat","-dbr");

        // Creates TNode structure instance accessible through the "node" pointer
        TNode* node  = new TNode(multi->GetPM());

        // (1) Initialization of GEMS3K internal data by reading  files
        //     whose names are given in the lst_f_name
        if( node->GEM_init( lst_f_name.c_str() ) )
        {
            // error occured during reading the files
            cout << "error occured during reading the files" << endl;
            return ret;
        }

        // (2) re-calculating equilibrium by calling GEMS3K, getting the status back
        long NodeStatusCH = node->GEM_run( false );
        ret  = node->GEM_CalcTime();

        if( NodeStatusCH == OK_GEM_AIA || NodeStatusCH == OK_GEM_SIA  ){
            // (3) Writing results in default DBR file
            node->GEM_write_dbr( dbr_lst_f_name.c_str(), false, true, false );
            node->GEM_print_ipm( "GEMipmOK.txt" );   // possible debugging printout
        }
        else {
            // (4) possible return status analysis, error message
            node->GEM_print_ipm( "GEMipmError.txt" );   // possible debugging printout
            return ret; // GEM IPM did not converge properly - error message needed
        }

    }catch(TError& err)
    {


    }
    catch(...)
    {

    }
    return ret;
}*/

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
