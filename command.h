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
    bool backwards = false;
    std::string cmd_movement_rev = "{\"tank_id\":\"%s\",\"comm_type\":\"MOVE\",\"direction\":\"REV\",\"distance\":\"%lf\",\"client_token\":\"%s\"}";
    std::string cmd_movement_fwd = "{\"tank_id\":\"%s\",\"comm_type\":\"MOVE\",\"direction\":\"FWD\",\"distance\":\"%lf\",\"client_token\":\"%s\"}";
public:
    MoveCommand(double d, std::string t, bool bwd = false){
        distance = d;
        tank_id = t;
        backwards = bwd;
    }

    ~MoveCommand(){}

    std::string GetCommandMessage(std::string client_token, char* buffer){
        if(!backwards) {
            std::sprintf(buffer, cmd_movement_fwd.c_str(), tank_id.c_str(), distance, client_token.c_str());
        } else {
            std::sprintf(buffer, cmd_movement_rev.c_str(), tank_id.c_str(), distance, client_token.c_str());
        }
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

    ~RotateCommand(){}

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

    ~FireCommand(){}

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
        if(r > acos(-1)){
            r -= 2*acos(-1);
        } else if(r < -acos(-1)){
            r += 2*acos(-1);
        }
        tank_id = t;
        rads = r;
    }

    ~RotateTurretCommand(){}

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

    ~ConnectCommand(){}

    std::string GetCommandMessage(std::string client_token, char* buffer){
        std::sprintf(buffer, cmd_connect.c_str(), match_token.c_str(), password.c_str());
        std::string ret = buffer;
        return ret;
    }
};

class StopFireCommand : public Command
{
private:
    std::string id;
    std::string cmd_stop = "{\"tank_id\":\"%s\",\"comm_type\":\"STOP\",\"control\":\"FIRE\",\"client_token\":\"%s\"}";
public:
    StopFireCommand(std::string tid){
        id = tid;
    }

    ~StopFireCommand(){}

    std::string GetCommandMessage(std::string client_token, char* buffer){
        std::sprintf(buffer, cmd_stop.c_str(), id.c_str(), client_token.c_str());
        std::string ret = buffer;
        return ret;
    }
};

#endif
