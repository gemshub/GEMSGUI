#pragma once

#include <iostream>
#include <zmq.hpp>
#include <zmq_addon.hpp>

using zmq_message_t = std::vector<std::string>;

/// \class  zmq_req_client_t - REQ-REP client task.
///  It connects to the server, and then sends request generated from  task_data
///  and get response and exporting the result to task_data.
template <class T>
class zmq_req_client_t
{
public:

    explicit zmq_req_client_t( T& task, const std::string& address= "tcp://localhost:5555" ):
        task_data( task ),
        ctx(1), client_socket( ctx, zmq::socket_type::req )
    {
        client_socket.connect( address );
    }

    bool run_task()
    {
        try
        {
            //  Do request, waiting each time for a response
            send_data( client_socket );
            //  Get the reply.
            return resv_task( client_socket );
        }
        catch (std::exception &e)
        {
            std::cout << "Connect to server error... " <<  e.what() << std::endl;
        }

        return false;
    }


protected:

    void send_data( zmq::socket_t& socket )
    {
        zmq_message_t send_msg = task_data.generate_send_msg( true );

        //  Do request, waiting each time for a response
        std::vector<zmq::message_t> in_msgs;
        for( const auto& msg: send_msg)
            in_msgs.push_back( zmq::message_t(msg.begin(), msg.end()));
        /*auto iret =*/ zmq::send_multipart( socket, in_msgs);
    }

    bool resv_task( zmq::socket_t& socket )
    {
        //  Get the reply.
        zmq_message_t msg_return;
        std::vector<zmq::message_t> out_msgs;

        [[maybe_unused]] auto oret = zmq::recv_multipart( socket, std::back_inserter(out_msgs));
        for( const auto& omsg: out_msgs)
            msg_return.push_back( omsg.to_string() );

        out_msgs.erase( out_msgs.begin() ); // identity

        return task_data.set_resv_msg( std::move(msg_return) );
    }

private:

    T& task_data;
    zmq::context_t ctx;
    zmq::socket_t client_socket;

};
