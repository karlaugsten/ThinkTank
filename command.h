#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include <zmq.hpp>

class Command
{
private:
    zmq::socket_t *messager;
    char buffer[1000];
    // For passing into every message.
    std::string client_token;

    std::string cmd_error = "{\n"
            "\"comm_type\" : \"%s\",\n"
            "\"error\" : \"%s\",\n"
            "\"message\" : \"%s\",\n"
            "}";
    std::string cmd_connect = "{\n"
            "\"comm_type\" : \"MatchConnect\",\n"
            "\"match_token\" : \"%s\",\n"
            "\"team_name\" : \"Think Tank\",\n"
            "\"password\" : \"%s\"\n"
            "}";
    std::string cmd_movement_fwd = "{\n"
            "\"tank_id\" : \"%s\",\n"
            "\"comm_type\" : \"MOVE\",\n"
            "\"direction\" : \"FWD\",\n"
            "\"distance\" : \"%lf\"\n"
            "\"client_token\" : \"%s\"\n"
            "}";
    std::string cmd_movement_rev = "{\n"
            "\"tank_id\" : \"%s\",\n"
            "\"comm_type\" : \"MOVE\",\n"
            "\"direction\" : \"REV\",\n"
            "\"distance\" : \"%lf\"\n"
            "\"client_token\" : \"%s\"\n"
            "}";
    std::string cmd_tankRotation_ccw = "{\n"
            "\"tank_id\" : \"%s\",\n"
            "\"comm_type\" : \"ROTATE\",\n"
            "\"direction\" : \"CCW\",\n"
            "\"distance\" : \"%lf\"\n"
            "\"client_token\" : \"%s\"\n"
            "}";
    std::string cmd_tankRotation_cw = "{\n"
            "\"tank_id\" : \"%s\",\n"
            "\"comm_type\" : \"ROTATE\",\n"
            "\"direction\" : \"CW\",\n"
            "\"distance\" : \"%lf\"\n"
            "\"client_token\" : \"%s\"\n"
            "}";
    std::string cmd_turretRotation_ccw = "{\n"
            "\"tank_id\" : \"%s\",\n"
            "\"comm_type\" : \"ROTATE_TURRET\",\n"
            "\"direction\" : \"CCW\",\n"
            "\"rads\" : \"%lf\"\n"
            "\"client_token\" : \"%s\"\n"
            "}";
    std::string cmd_turretRotation_cw = "{\n"
            "\"tank_id\" : \"%s\",\n"
            "\"comm_type\" : \"ROTATE_TURRET\",\n"
            "\"direction\" : \"cw\",\n"
            "\"rads\" : \"%lf\"\n"
            "\"client_token\" : \"%s\"\n"
            "}";
    std::string cmd_fire = "{\"tank_id\":\"%s\",\"comm_type\":\"FIRE\",\"client_token\":\"%s\"}";
    std::string cmd_stop = "{\n"
            "\"tank_id\" : \"%s\",\n"
            "\"comm_type\" : \"STOP\",\n"
            "\"control\" : \"%s\",\n"
            "\"client_token\" : \"%s\",\n"
            "}";

public:
    Command(zmq::context_t &ctx,std::string server_ip);
    std::string MatchConnect(std::string match_token, std::string password);
    void Movement(std::string tank_id, double distance);
    void TankRotation(std::string tank_id, double rads);
    void TurretRotation(std::string tank_id, double rads);
    void Fire(std::string tank_id);
    void Stop(std::string tank_id, std::string control);
    std::string SendMessage(int n);
};

#endif