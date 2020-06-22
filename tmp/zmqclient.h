#ifndef ZMQCLIENT_H
#define ZMQCLIENT_H

#include <string>
#include <vector>
#include <memory>
#include <zmq.hpp>

using zmq_message_t = std::vector<std::string>;

/// Class for send/get commands from ZMQ GEMS server
class ZMQClient
{

public:

    explicit ZMQClient();

    /// Run process of calculate equilibria into the GEMS3K side
    zmq_message_t  calculateEquilibriumServer( const zmq_message_t& msg_data );

protected:

    zmq::context_t context;
    zmq::socket_t socket;

};

#endif // ZMQCLIENT_H
