#include <iostream>
#include <string>
#include <cstdio>
#include "command.h"


Command::Command(zmq::context_t &ctx,std::string server_ip){
    messager= new zmq::socket_t(ctx, ZMQ_REQ);
    std::string command_channel_connection = "tcp://" + server_ip + ":5557";
    messager->connect(command_channel_connection.c_str());
}

void Command::ErrorResponse(std::string comm_type,std::string error,std::string message)
{
    int n;
    n=std::sprintf(buffer,cmd_error.c_str(),comm_type.c_str(),error.c_str(),message.c_str());
    std::string resp = SendMessage(n);
}

std::string Command::MatchConnect(std::string comm_type, std::string match_token, std::string team_name, std::string password)
{
    int n;
    n=std::sprintf(buffer,cmd_connect.c_str(),comm_type.c_str(),match_token.c_str(),team_name.c_str(),password.c_str());
    std::string resp = SendMessage(n);
    return resp;
}

void Command::Movement(std::string tank_id, std::string comm_type,std::string direction,double distance,std::string client_token)
{
    int n;
    n=std::sprintf(buffer,cmd_movement.c_str(),tank_id.c_str(),comm_type.c_str(),direction.c_str(),distance,client_token.c_str());
    std::string resp = SendMessage(n);
}

void Command::TankRotation(std::string tank_id, std::string comm_type,std::string direction,double rads,std::string client_token)
{
    int n;
    n=std::sprintf(buffer,cmd_tankRotation.c_str(),tank_id.c_str(),comm_type.c_str(),direction.c_str(),rads,client_token.c_str());
    std::string resp = SendMessage(n);
}

void Command::TurretRotation(std::string tank_id, std::string comm_type,std::string direction,double rads,std::string client_token)
{
    int n;
    n=std::sprintf(buffer, cmd_turretRotation.c_str(),tank_id.c_str(),comm_type.c_str(),direction.c_str(),rads,client_token.c_str());
    std::string resp = SendMessage(n);
}

void Command::Fire(std::string tank_id, std::string comm_type,std::string client_token)
{
    int n;
    n=std::sprintf(buffer,cmd_fire.c_str(),tank_id.c_str(),comm_type.c_str(),client_token.c_str());
    std::string resp = SendMessage(n);
}

void Command::Stop(std::string tank_id, std::string comm_type,std::string control,std::string client_token)
{
    int n;
    n=std::sprintf(buffer,cmd_stop.c_str(),tank_id.c_str(),comm_type.c_str(),control.c_str(),client_token.c_str());
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