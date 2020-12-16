///
///  Client part of Asynchronous client-to-server (DEALER to ROUTER)
///  http://zguide.zeromq.org/cpp:asyncsrv
///


#pragma once

#include <vector>
#include <iostream>
#include <thread>
#include <memory>
#include <functional>

#include <zmq.hpp>
#include <zmq_addon.hpp>

using zmq_message_t = std::vector<std::string>;

/// \class  zmq_client_t  our client task.
/// It connects to the server, and then sends requests generated from  task_data
/// and collects responses as they arrive with exporting the result to task_data.
template <class T>
class zmq_client_t
{
public:

    zmq_client_t( T& task, const std::string& address= "tcp://localhost:5570" )
        : task_data( task ),
          ctx(1),
          client_socket(ctx, ZMQ_DEALER)
    {
        // generate random identity
        char identity[10] = { "CC66-C879" };
        //sprintf(identity, "%04X-%04X", within(0x10000), within(0x10000));
        client_socket.setsockopt(ZMQ_IDENTITY, identity, strlen(identity));
        client_socket.connect(address);
    }

    bool run_task()
    {
        zmq::pollitem_t items[] = {
            { static_cast<void*>(client_socket), 0, ZMQ_POLLIN, 0 } };

        try
        {
            //  Do request, waiting each time for a response
            if( !send_data( client_socket ) )
                return false;

            while ( true )
            {
                zmq::poll(items, 1, 10);
                if (items[0].revents & ZMQ_POLLIN)
                {
                    return resv_task(client_socket);
                }
            }
        }
        catch (std::exception &e)
        {
            std::cout << "Connect to server error... " <<  e.what() << std::endl;
        }
        return false;
    }

    void start_loop()
    {
        zmq::pollitem_t items[] = {
            { static_cast<void*>(client_socket), 0, ZMQ_POLLIN, 0 } };

        try
        {
            while ( task_data.wait_next() )
            {
                {
                    zmq::poll(items, 1, 10);
                    if (items[0].revents & ZMQ_POLLIN)
                    {
                        resv_task(client_socket);
                    }
                }
                if( task_data.has_next() )
                {
                    send_data( client_socket );
                }
            }
        }
        catch (std::exception &e) {}
    }


    void start_list()
    {
        zmq::pollitem_t items[] = {
            { static_cast<void*>(client_socket), 0, ZMQ_POLLIN, 0 } };

        try
        {
            while ( task_data.has_next())
            {
                send_data(client_socket );
            }

            while ( task_data.wait_next() )
            {
                zmq::poll(items, 1, 10);
                if (items[0].revents & ZMQ_POLLIN)
                {
                    resv_task(client_socket);
                }
            }
        }
        catch (std::exception &e) {}
    }

protected:

    bool send_data( zmq::socket_t& socket )
    {
        zmq_message_t send_msg = task_data.generate_send_msg( false );

        if( send_msg.empty() ) // skip send-reply
          return false;

        //  Do request, waiting each time for a response
        std::vector<zmq::message_t> in_msgs;
        for( const auto& msg: send_msg)
            in_msgs.push_back( zmq::message_t(msg.begin(), msg.end()));
        /*auto iret =*/ zmq::send_multipart( socket, in_msgs);
        return true;
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

