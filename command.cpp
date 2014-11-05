#include <iostream>
#include <string>
#include <cstdio>
#include "command.h"
#include "StateParser/rapidjson/document.h"

Command::Command(zmq::context_t &ctx,std::string server_ip){
    messager= new zmq::socket_t(ctx, ZMQ_REQ);
    std::string command_channel_connection = "tcp://" + server_ip + ":5557";
    messager->connect(command_channel_connection.c_str());
}

/* Dont think we need this.
void Command::ErrorResponse(std::string comm_type,std::string error,std::string message)
{
    int n;
    n=std::sprintf(buffer,cmd_error.c_str(),comm_type.c_str(),error.c_str(),message.c_str());
    std::string resp = SendMessage(n);
}*/

std::string Command::MatchConnect(std::string match_token, std::string password)
{
    int n;
    n=std::sprintf(buffer, cmd_connect.c_str(), match_token.c_str(), password.c_str());
    std::string resp = SendMessage(n);
    rapidjson::Document dom;
    dom.Parse(resp.c_str());
    if(dom.HasParseError()){
        std::cout << "Error parsing match connect resp: " << resp << std::endl;
        return "";
    } else{
        const rapidjson::Value& d_client_token = dom["client_token"];
        assert(d_client_token.IsString());
        client_token = d_client_token.GetString();
    }
    return client_token;
}

void Command::Movement(std::string tank_id, double distance)
{
    int n;
    n=std::sprintf(buffer, cmd_movement_fwd.c_str(),tank_id.c_str(), distance, client_token.c_str());
    std::string resp = SendMessage(n);
}

void Command::TankRotation(std::string tank_id, double rads)
{
    int n;
    n=std::sprintf(buffer, cmd_tankRotation_cw.c_str(), tank_id.c_str(), rads, client_token.c_str());
    std::string resp = SendMessage(n);
}

void Command::TurretRotation(std::string tank_id, double rads)
{
    int n;
    n=std::sprintf(buffer, cmd_turretRotation_cw.c_str(), tank_id.c_str(), rads, client_token.c_str());
    std::string resp = SendMessage(n);
}

void Command::Fire(std::string tank_id)
{
    int n;
    n=std::sprintf(buffer, cmd_fire.c_str(), tank_id.c_str(), client_token.c_str());
    printf(buffer);
    std::string resp = SendMessage(n);
}

void Command::Stop(std::string tank_id, std::string control)
{
    int n;
    n=std::sprintf(buffer, cmd_stop.c_str(), tank_id.c_str(), control.c_str(), client_token.c_str());
    std::string resp = SendMessage(n);
}

std::string Command::SendMessage(int n)
{
    zmq::message_t connect_msg(n);
    memcpy(connect_msg.data(), buffer,n);
    if(messager->send(connect_msg)){
        zmq::message_t resp;
        messager->recv(&resp);
        std::string data(static_cast<char*>(resp.data()), resp.size());
        std::cout << "recieved response: " << data << std::endl;
        return data;

    } else {
        // TODO: return something useful
        std::cout << "Failed to connect to game..." << std::endl;
        return "";
    }
}