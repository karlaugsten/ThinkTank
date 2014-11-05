#include <iostream>
#include <string>
#include "StateParser/gamestate.h"
#include "StateParser/stateparser.h"
#include "command.h"
#include <zmq.hpp>
#include <thread>

using namespace std;



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
    Command cmdChannel = Command(ctx,server_ip);
    std::string client_token = cmdChannel.MatchConnect(match_token, password);
    if(client_token == "") {
        cout << "Could not retrieve client token" << endl;
    } else {
        cout << "Client token is: " << client_token << endl;
    }

    // Setup message channel TODO: This is for testing, actually encapsulate this
    /*std::string match_connect = "{\n"
            "\"comm_type\" : \"MatchConnect\",\n"
            "\"match_token\" : \"" + match_token + "\",\n"
            "\"team_name\" : \"Think Tank\",\n"
            "\"password\" : \"" + password + "\"\n"
            "}";
    cout << "Match Connect msg: " << match_connect << endl;
    zmq::socket_t messager (ctx, ZMQ_REQ);
    std::string command_channel_connection = "tcp://" + server_ip + ":5557";
    messager.connect(command_channel_connection.c_str());
    zmq::message_t connect_msg(match_connect.size());
    memcpy(connect_msg.data(), match_connect.data(), match_connect.size());
    if(messager.send(connect_msg)){

        cout << "Sent match connect message!" << endl;
        zmq::message_t resp;
        messager.recv(&resp);
        std::string data(static_cast<char*>(resp.data()), resp.size());
        cout << "recieved response: " << data << endl;
    } else {
        cout << "Failed to connect to game..." << endl;
        return -1;
    }*/

    // Setup state channel TODO: This is for testing, actually encapsulate this

    zmq::socket_t sub (ctx, ZMQ_SUB);

    std::string state_channel_connection = "tcp://" + server_ip + ":5556";
    sub.connect (state_channel_connection.c_str());
    sub.setsockopt(ZMQ_SUBSCRIBE, match_token.c_str(), strlen(match_token.c_str()));

    // Spawn new thread for state parsing.
    StateParser parser = StateParser(&sub);
    /*
    thread StateChannel(&StateParser::Run, parser);
    */

    if(parser.state == NULL) cout << "wtf" << endl;
    while(true){
        parser.ReceiveAndParse();
        
        cout << "Trying to fire!" << endl;
        if(parser.state->GetPlayer() != NULL){
            cout << "Got our player" << endl;
            if(parser.state->GetPlayer()->TankFast != NULL){
                cout << "Firing fast tank!" << endl;
                cmdChannel.Fire(parser.state->GetPlayer()->TankFast->id);
            }
            if(parser.state->GetPlayer()->TankSlow != NULL){
                cmdChannel.Fire(parser.state->GetPlayer()->TankFast->id);
            }
        }else {
            cout << "player is null" << endl;
	}
    }
    return 0;
}
