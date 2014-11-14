#include "commandchannel.h"
#include "StateParser/rapidjson/document.h"
#include <iostream>

CommandChannel::CommandChannel(zmq::context_t &ctx, std::string server_ip, std::string match_token, std::string password){
    channel = new zmq::socket_t(ctx, ZMQ_REQ);
    std::string command_channel_connection = "tcp://" + server_ip + ":5557";
    channel->connect(command_channel_connection.c_str());
    ConnectCommand match_connect = ConnectCommand(match_token, password);
    // Send match connect command and parse client_token from it
    match_connect.GetCommandMessage("", buffer);

    std::string resp = SendMessage();
    if(resp.rfind("error") != std::string::npos){
        std::cout << "Received error response:\n" << resp << "\nFrom command:\n" << buffer << "\n";
        exit(-1);
    }
    rapidjson::Document dom;
    dom.Parse(resp.c_str());
    if(dom.HasParseError()){
        std::cout << "Error parsing match connect resp: " << resp << std::endl;
    } else{
        const rapidjson::Value& d_client_token = dom["client_token"];
        assert(d_client_token.IsString());
        client_token = d_client_token.GetString();
        std::cout << "Command channel received client token: " << client_token << std::endl;
    }
}

/**
* function which sends the message currently in the buffer.
*/
std::string CommandChannel::SendMessage(){
    zmq::message_t connect_msg(strlen(buffer));
    memcpy(connect_msg.data(), buffer, strlen(buffer));
    if(channel->send(connect_msg)){
        zmq::message_t resp;
        channel->recv(&resp);
        std::string data(static_cast<char*>(resp.data()), resp.size());
        return data;
    } else {
        // TODO: return something useful
        std::cout << "Failed to send message." << std::endl;
        return "";
    }
}

void CommandChannel::SendCommand(Command& command){
    command.GetCommandMessage(client_token, buffer);
    std::string resp = SendMessage();
    // TODO: check for error resp
    if(resp.rfind("error") != std::string::npos){
        std::cout << "Received error response:\n" << resp << "\nFrom command:\n" << buffer << "\n";
    }
}

void CommandChannel::SendCommands(std::queue<Command*> commands){
    while(!commands.empty()){
        Command* c = commands.front();
        commands.pop();
        SendCommand(*c);
        // delete reference command!
        delete c;
    }
}

