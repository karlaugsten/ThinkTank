#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include <zmq.hpp>
#include <cmath>

/**
* Base class that all commands inherit from.
* Basically just constructs a command message.
*/
class Command
{
protected:
public:
    Command() {
    }

    virtual std::string GetCommandMessage(std::string client_token, char* buffer)=0;
};

/**
* Command to move tank
*/
class MoveCommand : public Command
{
private:
    double distance;
    std::string tank_id;
    std::string cmd_movement_fwd = "{\"tank_id\":\"%s\",\"comm_type\":\"MOVE\",\"direction\":\"FWD\",\"distance\":\"%lf\",\"client_token\":\"%s\"}";
public:
    MoveCommand(double d, std::string t){
        distance = d;
        tank_id = t;
    }

    std::string GetCommandMessage(std::string client_token, char* buffer){
        std::sprintf(buffer, cmd_movement_fwd.c_str(), tank_id.c_str(), distance, client_token.c_str());
        std::string ret = buffer;
        return ret;
    }
};

class RotateCommand : public Command
{
private:
    double rads;
    std::string tank_id;
    std::string cmd_tankRotation_ccw = "{\"tank_id\":\"%s\",\"comm_type\":\"ROTATE\",\"direction\":\"CCW\",\"rads\":%lf,\"client_token\":\"%s\"}";

    std::string cmd_tankRotation_cw = "{\"tank_id\":\"%s\",\"comm_type\":\"ROTATE\",\"direction\":\"CW\",\"rads\":%lf,\"client_token\":\"%s\"}";
public:
    RotateCommand(double r, std::string t){
        // Dont rotate the long way around!
        if(r > acos(-1)){
            r -= 2*acos(-1);
        } else if(r < -acos(-1)){
            r += 2*acos(-1);
        }
        rads = r;
        tank_id = t;
    }

    std::string GetCommandMessage(std::string client_token, char* buffer){
        if(rads > 0.0) {
            // TODO: This acos is slow...
            if(rads > 2*acos(-1)) {
                rads -= 2*acos(-1);
            }
            std::sprintf(buffer, cmd_tankRotation_ccw.c_str(), tank_id.c_str(), rads, client_token.c_str());
        } else {
            rads = -rads;
            if(rads > 2*acos(-1)) {
                rads -= 2*acos(-1);
            }
            std::sprintf(buffer, cmd_tankRotation_cw.c_str(), tank_id.c_str(), rads, client_token.c_str());
        }
        std::string ret = buffer;
        return ret;
    }
};

class FireCommand : public Command
{
private:
    std::string tank_id;
    std::string cmd_fire = "{\"tank_id\":\"%s\",\"comm_type\":\"FIRE\",\"client_token\":\"%s\"}";
public:
    FireCommand(std::string t){
        tank_id = t;
    }

    std::string GetCommandMessage(std::string client_token, char* buffer){
        std::sprintf(buffer, cmd_fire.c_str(), tank_id.c_str(), client_token.c_str());
        std::string ret = buffer;
        return ret;
    }
};

class RotateTurretCommand : public Command
{
private:
    double rads;
    std::string tank_id;
    std::string cmd_turretRotation_cw = "{\"tank_id\":\"%s\",\"comm_type\" : \"ROTATE_TURRET\",\"direction\":\"CW\",\"rads\":\"%lf\",\"client_token\":\"%s\"}";
    std::string cmd_turretRotation_ccw = "{\"tank_id\":\"%s\",\"comm_type\":\"ROTATE_TURRET\",\"direction\":\"CCW\",\"rads\":\"%lf\",\"client_token\":\"%s\"}";
public:
    RotateTurretCommand(double r, std::string t){
        tank_id = t;
        rads = r;
    }

    std::string GetCommandMessage(std::string client_token, char* buffer){

        if(rads > 0.0) {
            // TODO: This acos is slow...
            if(rads > 2*acos(-1)) {
                rads -= 2*acos(-1);
            }
            std::sprintf(buffer, cmd_turretRotation_ccw.c_str(), tank_id.c_str(), rads, client_token.c_str());
        } else {
            rads = -rads;
            if(rads > 2*acos(-1)) {
                rads -= 2*acos(-1);
            }
            std::sprintf(buffer, cmd_turretRotation_cw.c_str(), tank_id.c_str(), rads, client_token.c_str());
        }
        std::string ret = buffer;
        return ret;
    }
};

class ConnectCommand : public Command
{
private:
    std::string password;
    std::string match_token;
    std::string cmd_connect = "{\n"
            "\"comm_type\" : \"MatchConnect\",\n"
            "\"match_token\" : \"%s\",\n"
            "\"team_name\" : \"Think Tank\",\n"
            "\"password\" : \"%s\"\n"
            "}";
public:
    ConnectCommand(std::string m, std::string p){
        match_token = m;
        password = p;
    }

    std::string GetCommandMessage(std::string client_token, char* buffer){
        std::sprintf(buffer, cmd_connect.c_str(), match_token.c_str(), password.c_str());
        std::string ret = buffer;
        return ret;
    }
};

/*
class Command
{
private:
    zmq::socket_t *messager;
    char buffer[10000];
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
*/
#endif
