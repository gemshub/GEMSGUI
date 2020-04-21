#include <iostream>
#include "zmqclient.h"
#include <zmq_addon.hpp>

ZMQClient::ZMQClient():
    context(1), socket(context, ZMQ_REQ)
{

    std::cout << "Connecting to hello world server…" << std::endl;
    socket.connect ("tcp://localhost:5555");
}

// Run process of calculate equilibria into the GEMS3K side
zmq_message_t  ZMQClient::calculateEquilibriumServer( const zmq_message_t& msg_data )
{
    zmq_message_t msg_return;
    try
    {
       //  Do request, waiting each time for a response
        std::vector<zmq::message_t> msgs_vec;
        for( const auto& msg: msg_data)
            msgs_vec.push_back( zmq::message_t(msg.begin(), msg.end()));
        /*auto iret =*/ zmq::send_multipart(socket, msgs_vec);

        //  Get the reply.
        std::vector<zmq::message_t> omsgs;
        /*auto oret =*/ zmq::recv_multipart(socket, std::back_inserter(omsgs));
        for( const auto& omsg: omsgs)
            msg_return.push_back( omsg.to_string() );

    }
    catch(...)
    {
        std::cout << "Connect to server error..." << std::endl;
    }
    return msg_return;
}
