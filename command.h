#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include <zmq.hpp>

class Command
{
private:
    zmq::socket_t *messager;
    char buffer[1000];
    std::string cmd_error = "{\n"
            "\"comm_type\" : \"%s\",\n"
            "\"error\" : \"%s\",\n"
            "\"message\" : \"%s\",\n"
            "}";
    std::string cmd_connect = "{\n"
            "\"comm_type\" : \"%s\",\n"
            "\"match_token\" : \"%s\",\n"
            "\"team_name\" : \"%s\",\n"
            "\"password\" : \"%s\"\n"
            "}";
    std::string cmd_movement = "{\n"
            "\"tank_id\" : \"%s\",\n"
            "\"comm_type\" : \"%s\",\n"
            "\"direction\" : \"%s\",\n"
            "\"distance\" : \"%d\"\n"
            "\"client_token\" : \"%s\"\n"
            "}";
    std::string cmd_tankRotation = "{\n"
            "\"tank_id\" : \"%s\",\n"
            "\"comm_type\" : \"%s\",\n"
            "\"direction\" : \"%s\",\n"
            "\"distance\" : \"%d\"\n"
            "\"client_token\" : \"%s\"\n"
            "}";
    std::string cmd_turretRotation = "{\n"
            "\"tank_id\" : \"%s\",\n"
            "\"comm_type\" : \"%s\",\n"
            "\"direction\" : \"%s\",\n"
            "\"rads\" : \"%d\"\n"
            "\"client_token\" : \"%s\"\n"
            "}";
    std::string cmd_fire = "{\n"
            "\"tank_id\" : \"%s\",\n"
            "\"comm_type\" : \"%s\",\n"
            "\"client_token\" : \"%s\",\n"
            "}";
    std::string cmd_stop = "{\n"
            "\"tank_id\" : \"%s\",\n"
            "\"comm_type\" : \"%s\",\n"
            "\"control\" : \"%s\",\n"
            "\"client_token\" : \"%s\",\n"
            "}";

public:
     Command(zmq::context_t &ctx,std::string server_ip);
     void ErrorResponse(std::string comm_type,std::string error,std::string message);
     std::string MatchConnect(std::string comm_type, std::string match_token, std::string team_name, std::string password);
     void Movement(std::string tank_id, std::string comm_type,std::string direction,double distance,std::string client_token);
     void TankRotation(std::string tank_id, std::string comm_type,std::string direction,double rads,std::string client_token);
     void TurretRotation(std::string tank_id, std::string comm_type,std::string direction,double rads,std::string client_token);
     void Fire(std::string tank_id, std::string comm_type,std::string client_token);
     void Stop(std::string tank_id, std::string comm_type,std::string control,std::string client_token);
     std::string SendMessage(int n);
};

#endif