#ifndef COMMANDCHANNEL_H
#define COMMANDCHANNEL_H

#include <zmq.hpp>
#include <vector>
#include "command.h"

class CommandChannel
{
private:
    std::string client_token;
    zmq::socket_t* channel;
    char buffer[2000];

    std::string SendMessage();

public:
    CommandChannel(zmq::context_t &ctx, std::string server_ip, std::string match_token, std::string password); // Constructor that parses the game state response message
    ~CommandChannel(){
        delete channel;
    }
    void SendCommand(Command& command);
    void SendCommands(std::vector<Command*> commands);
};

#endif