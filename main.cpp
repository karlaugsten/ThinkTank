#include <iostream>
#include <string>
#include "StateParser/gamestate.h"
#include "StateParser/stateparser.h"
#include "command.h"
#include <zmq.hpp>
#include <thread>
#include <unistd.h>
#include "commandchannel.h"
#include <cmath>

using namespace std;

void moveCommand_CircleDodge(CommandChannel& cmdChannel,string tankID){
    // TODO: Check for unbreakable terrain and unmovable terrain
    MoveCommand moveCommand = MoveCommand(1.0,tankID);
    cmdChannel.SendCommand(moveCommand);
    RotateCommand rotateCommand = RotateCommand(0.1,tankID);
    cmdChannel.SendCommand(rotateCommand);
}

double Distance(double x0, double y0, double x1, double y1)
{
    return sqrt((x1 - x0)*(x1 - x0) + (y1 - y0)*(y1 - y0));
}

Position getClosestTarget(GameState& state, Position& thisTank){
    double distanceBetweenTanksFast=-1;
    double distanceBetweenTanksSlow=-1;

    //Check if each tank is alive and calculate its relative distance
    if(state.GetOpponent().TankFast.alive) {
        Position enemy1 = state.GetOpponent().TankFast.position;
        distanceBetweenTanksFast=Distance(thisTank.x,thisTank.y,enemy1.x,enemy1.y);
    }
    if(state.GetOpponent().TankSlow.alive) {
        Position enemy2 = state.GetOpponent().TankSlow.position;
        distanceBetweenTanksSlow=Distance(thisTank.x,thisTank.y,enemy2.x,enemy2.y);
    }

    // TODO: CHECK FOR UNBREAKABLE BARRIER BETWEEN
    if(distanceBetweenTanksFast==-1){
        return state.GetOpponent().TankSlow.position;
    }else if(distanceBetweenTanksSlow==-1){
        return state.GetOpponent().TankFast.position;
    }else{
        if(distanceBetweenTanksFast>distanceBetweenTanksSlow){
            return state.GetOpponent().TankSlow.position;
        }else{
            return state.GetOpponent().TankFast.position;
        }
    }
}

int main(int argc, char* argv[]) {
    if(argc != 4) {

        cout << "Program must be run with arguments: ./ThinkTank server_ip match_token password" << endl;
        return -1;
    }

    std::string map = "{\n"
            "\"comm_type\": \"GAMESTATE\",\n"
            "\"timeRemaining\" : 300000.00,\n"
            "\"timestamp\" : 145433123.00,\n"
            "\"map\" : {\n"
            "\"size\" : [1280, 720],\n"
            "\"terrain\" : [\n"
            "{\n"
            "\"type\" : \"SOLID\",\n"
            "\"boundingBox\" : {\n"
            "\"corner\" : [5, 10],\n"
            "\"size\" : [6, 8]\n"
            "}\n"
            "},\n"
            "{\n"
            "\"type\" : \"IMPASSABLE\",\n"
            "\"boundingBox\" : {\n"
            "\"corner\" : [80, 80],\n"
            "\"size\" : [50, 80]\n"
            "}\n"
            "}\n"
            "]\n"
            "},\n" // End of Map
            "\"players\" : [\n"
            "{\n"
            "\"score\" : 1,\n"
            "\"name\" : \"Think Tank\",\n"
            "\"tanks\" : [\n"
            "{\n"
            "\"id\" : \"eb49f487-92dc-4f66-ac83-91ae04a4cc16\",\n"
            "\"health\" : 0,\n"
            "\"hitRadius\" : 2.0,\n"
            "\"collisionRadius\" : 1.0,\n"
            "\"type\" : \"TankSlow\",\n"
            "\"alive\" : false,\n"
            "\"position\" : [63, 33.2],\n"
            "\"tracks\" : 3.14,\n"
            "\"turret\": 0.0,\n"
            "\"speed\" : 5,\n"
            "\"projectiles\" : [\n"
            "{\n"
            "\"id\" : \"blahblah\",\n"
            "\"position\" : [5, 12],\n"
            "\"direction\" : 1.23,\n"
            "\"speed\" : 30,\n"
            "\"damage\" : 100\n"
            "}\n"
            "]\n"
            "},\n"
            "{\n"
            "\"id\" : \"eb49f487-92dc-4f66-ac83-91ae04a4cc18\",\n"
            "\"health\" : 2,\n"
            "\"hitRadius\" : 1.5,\n"
            "\"collisionRadius\" : 0.75,\n"
            "\"type\" : \"TankFast\",\n"
            "\"alive\" : true,\n"
            "\"position\" : [53, 33.2],\n"
            "\"tracks\" : 3.14,\n"
            "\"turret\" : 0.0,\n"
            "\"speed\" : 10,\n"
            "\"projectiles\" : []\n"
            "}\n"
            "]\n"
            "},\n" // end of player 1
            "{\n"
            "\"score\" : 3,\n"
            "\"name\" : \"Terrible Team BCA\",\n"
            "\"tanks\" : [\n"
            "{\n"
            "\"id\" : \"eb49f487-92dc-4f66-ac83-91ae04a4cc19\",\n"
            "\"health\" : 0,\n"
            "\"hitRadius\" : 2.0,\n"
            "\"collisionRadius\" : 1.0,\n"
            "\"type\" : \"TankSlow\",\n"
            "\"alive\" : false,\n"
            "\"position\" : [43, 33.2],\n"
            "\"tracks\" : 3.14,\n"
            "\"turret\": 0.0,\n"
            "\"speed\" : 5,\n"
            "\"projectiles\" : []\n"
            "},\n"
            "{\n"
            "\"id\" : \"eb49f487-92dc-4f66-ac83-91ae04a4cc15\",\n"
            "\"health\" : 2,\n"
            "\"hitRadius\" : 1.5,\n"
            "\"collisionRadius\" : 0.75,\n"
            "\"type\" : \"TankFast\",\n"
            "\"alive\" : true,\n"
            "\"position\" : [23, 33.2],\n"
            "\"tracks\" : 3.14,\n"
            "\"turret\" : 0.0,\n"
            "\"speed\" : 10,\n"
            "\"projectiles\" : []\n"
            "} ] } ]\n" // end of players array
            "}";

    std::string server_ip = argv[1];
    std::string match_token = argv[2];
    std::string password = argv[3];
    cout << "Server IP is: " << server_ip << endl;
    cout << "Match Token is: " << match_token << endl;

    zmq::context_t ctx (1);
    CommandChannel cmdChannel = CommandChannel(ctx, server_ip, match_token, password);
    //CommandChannel cmdChannel = CommandChannel();
    // Setup state channel TODO: This is for testing, actually encapsulate this

    zmq::socket_t sub (ctx, ZMQ_SUB);

    std::string state_channel_connection = "tcp://" + server_ip + ":5556";
    sub.connect (state_channel_connection.c_str());
    sub.setsockopt(ZMQ_SUBSCRIBE, match_token.c_str(), strlen(match_token.c_str()));

    // Spawn new thread for state parsing.
    StateParser parser = StateParser(&sub);

    std::thread parserThread = parser.Start();
    parserThread.detach();

    // Algorithm does stuff here!
    while(true){
        // TODO: not thread safe
        GameState state = parser.game;

        if(state.GetPlayer().alive){
            if(state.GetPlayer().TankFast.alive){
                // TODO: Find closest enemy tank, point turret towards him and fire.
                Position thisTank = state.GetPlayer().TankFast.position;
                double angle = state.GetPlayer().TankFast.turret;
                double tmp = thisTank.GetAngle(getClosestTarget(state, thisTank)) - angle - acos(-1);
                if (tmp > acos(-1)) {
                    tmp -= 2 * acos(-1);
                }
                if (tmp < acos(-1)) {
                    tmp += 2 * acos(-1);
                }
                angle = tmp;
                cout << "Fast Tank: " << thisTank.GetAngle(getClosestTarget(state, thisTank)) << " with angle: " << angle << endl;
                // TODO: Check for unbreakable terrain
                RotateTurretCommand rotateTurret = RotateTurretCommand(angle, state.GetPlayer().TankFast.id);
                cmdChannel.SendCommand(rotateTurret);
                FireCommand command = FireCommand(state.GetPlayer().TankFast.id);
                cmdChannel.SendCommand(command);
                moveCommand_CircleDodge(cmdChannel, state.GetPlayer().TankFast.id);
            }
            if(state.GetPlayer().TankSlow.alive){
                Position thisTank = state.GetPlayer().TankSlow.position;
                double angle = state.GetPlayer().TankSlow.turret;
                double tmp = thisTank.GetAngle(getClosestTarget(state, thisTank)) - angle - acos(-1);
                if(tmp > acos(-1)){
                    tmp -= 2*acos(-1);
                }if(tmp < acos(-1)){
                    tmp += 2*acos(-1);
                }
                angle = tmp;
                cout <<  "Slow tank " << thisTank.GetAngle(getClosestTarget(state, thisTank)) << " with angle: " << angle << endl;
                RotateTurretCommand rotateTurret = RotateTurretCommand(angle, state.GetPlayer().TankSlow.id);
                cmdChannel.SendCommand(rotateTurret);
                FireCommand command = FireCommand(state.GetPlayer().TankSlow.id);
                cmdChannel.SendCommand(command);
                moveCommand_CircleDodge(cmdChannel,state.GetPlayer().TankSlow.id);
            }
        }
    }
    return 0;
}
